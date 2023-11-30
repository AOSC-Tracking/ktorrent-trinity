/***************************************************************************
 *   Copyright (C) 2005,2006 by                                            *
 *   Joris Guisson <joris.guisson@gmail.com>                               *
 *   Ivan Vasic <ivasic@gmail.com>                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.             *
 ***************************************************************************/
#include <tdelocale.h>
#include <tdeglobal.h>
#include <kiconloader.h>
#include <tdepopupmenu.h>
#include <krun.h>
#include <kurl.h>
#include <kurldrag.h>
#include <tdemessagebox.h>
#include <kstdguiitem.h>
#include <tdefiledialog.h>

#include <interfaces/torrentinterface.h>
#include <torrent/globals.h>
#include <util/log.h>
#include <util/functions.h>
		
#include <groups/group.h>
#include <groups/torrentdrag.h>

#include <tqcursor.h>
#include <tqheader.h>
#include <tqvaluelist.h>
#include <tqlayout.h>
		
#include "ktorrentview.h"
#include "ktorrentviewitem.h"
#include "settings.h"
#include "scandialog.h"
#include "addpeerwidget.h"
#include "ktorrentviewmenu.h"
#include "speedlimitsdlg.h"
#include "filterbar.h"

using namespace bt;
using namespace kt;


TorrentView::TorrentView(KTorrentView* parent) : TDEListView(parent),ktview(parent)
{}
	
TorrentView::~TorrentView() 
{}
	
bool TorrentView::eventFilter(TQObject* watched, TQEvent* e)
{
	if((TQHeader*)watched == header())
	{
		switch(e->type())
		{
			case TQEvent::MouseButtonPress:
			{
				if(static_cast<TQMouseEvent*>(e)->button() == TQt::RightButton)
					ktview->m_headerMenu->popup(TQCursor::pos());

				break;
			}
			default:
				break;
		}
	}

	return TDEListView::eventFilter(watched, e);
}

KTorrentView::KTorrentView(TQWidget *parent)
	: TQWidget(parent),menu(0),current_group(0),running(0),total(0),view(0),filter_bar(0)
{
	TQVBoxLayout* layout = new TQVBoxLayout(this);
	layout->setAutoAdd(true);
	view = new TorrentView(this);
	filter_bar = new FilterBar(this);
	filter_bar->setHidden(true);
	
	setupColumns();
	
	connect(view,TQT_SIGNAL(executed(TQListViewItem* )),
			this,TQT_SLOT(onExecuted(TQListViewItem* )));
	
	connect(view,TQT_SIGNAL(currentChanged(TQListViewItem* )),
			this,TQT_SLOT(onExecuted(TQListViewItem* )));
	
	connect(view,TQT_SIGNAL(contextMenu(TDEListView*, TQListViewItem*, const TQPoint& )),
			this,TQT_SLOT(showContextMenu(TDEListView*, TQListViewItem*, const TQPoint& )));
	
	connect(view,TQT_SIGNAL(selectionChanged()),this,TQT_SLOT(onSelectionChanged()));

	menu = new KTorrentViewMenu(this);
	connect(menu,TQT_SIGNAL(groupItemActivated(const TQString&)),this,TQT_SLOT(gsmItemActived(const TQString&)));
	
	connect(m_headerMenu, TQT_SIGNAL(activated(int)), this, TQT_SLOT(onColumnVisibilityChange( int )));

	view->setFrameShape(TQFrame::NoFrame);
}

KTorrentView::~KTorrentView()
{
}

void KTorrentView::insertColumn(TQString label, TQt::AlignmentFlags align)
{
	m_headerMenu->insertItem(label);
		
	int ind = view->addColumn(label);
	view->setColumnAlignment(ind, align);
}

void KTorrentView::setupColumns()
{
		//Header menu
	m_headerMenu = new TDEPopupMenu(view);
	m_headerMenu->setCheckable(true);
	m_headerMenu->insertTitle(i18n("Visible columns"));
	
	insertColumn(i18n("File"), TQt::AlignLeft);
	insertColumn(i18n("Status"), TQt::AlignLeft);
	insertColumn(i18n("Downloaded"), TQt::AlignRight);
	insertColumn(i18n("Size"), TQt::AlignRight); 
	insertColumn(i18n("Uploaded"), TQt::AlignRight);
	insertColumn(i18n("Down Speed"), TQt::AlignRight);
	insertColumn(i18n("Up Speed"), TQt::AlignRight);
	insertColumn(i18n("Time Left"), TQt::AlignCenter);
	insertColumn(i18n("Seeders"), TQt::AlignRight);
	insertColumn(i18n("Leechers"), TQt::AlignRight);
	insertColumn(i18n("% Complete"), TQt::AlignRight);
	insertColumn(i18n("Share Ratio"), TQt::AlignRight);
	insertColumn(i18n("Time Downloaded"), TQt::AlignRight);	
	insertColumn(i18n("Time Seeded"), TQt::AlignRight);
	
	view->setAllColumnsShowFocus(true);
	view->setShowSortIndicator(true);
	view->setAcceptDrops(true);
	view->setSelectionMode(TQListView::Extended);
	for (Uint32 i = 0;i < (Uint32)view->columns();i++)
	{
		view->setColumnWidth(i, 100);
		view->setColumnWidthMode(i,TQListView::Manual);
	}
}

void KTorrentView::setCurrentGroup(Group* group)
{
	if (current_group == group)
		return;
	
	current_group = group;
	
	running = 0; 
	total = 0;
	// go over the current items, if they still match keep them, else remove them
	// add new itesm if necessary
	TQMap<TorrentInterface*,KTorrentViewItem*>::iterator i = items.begin();
	while (i != items.end())
	{
		KTorrentViewItem* tvi = i.data();
		TorrentInterface* tc = i.key();
		if (current_group && !current_group->isMember(tc))
		{
			if (tvi)
			{
				delete tvi;
				i.data() = 0;
			}
		}
		else if (!tvi)
		{
			tvi = new KTorrentViewItem(this,tc);
			i.data() = tvi;
		}
		
		if (i.data())
		{
			total++;
			if (tc->getStats().running)
				running++;
		}

		i++;
	}
	
	if (current_group)
		setCaption(TQString("%1 %2/%3").arg(current_group->groupName()).arg(running).arg(total));
	else
		setCaption(i18n("All Torrents %1/%2").arg(running).arg(total));
	
	onSelectionChanged();
	onExecuted(view->currentItem());
}

void KTorrentView::saveSettings(TDEConfig* cfg,int idx)
{
	TQString group = TQString("KTorrentView-%1").arg(idx);
	view->saveLayout(cfg,group);
	cfg->setGroup(group);
	filter_bar->saveSettings(cfg);
}


void KTorrentView::loadSettings(TDEConfig* cfg,int idx)
{
	TQString group = TQString("KTorrentView-%1").arg(idx);
	view->restoreLayout(cfg,group);
	view->setDragEnabled(true);

	for(int i=0; i < view->columns();++i)
	{
		bool visible = columnVisible(i);
		
		m_headerMenu->setItemChecked(m_headerMenu->idAt(i+1), visible);
		view->header()->setResizeEnabled(visible, i);
	}
	cfg->setGroup(group);
	filter_bar->loadSettings(cfg);
}


int KTorrentView::getNumRunning()
{
	int num = 0;
	TQMap<TorrentInterface*,KTorrentViewItem*>::iterator i = items.begin();
	while (i != items.end())
	{
		KTorrentViewItem* tvi = i.data();
		if (tvi)
		{
			TorrentInterface* tc = tvi->getTC();
			num += tc->getStats().running ? 1 : 0;
		}
		i++;
	}
	return num;
}

bool KTorrentView::startDownload(kt::TorrentInterface* tc)
{
	if (tc && !tc->getStats().running)
	{
		wantToStart(tc);
		if (!tc->getStats().running && !tc->getStats().stopped_by_error)
		{
			if (tc->getStats().completed)
			{
				if (!tc->overMaxRatio() && !tc->overMaxSeedTime())
					return false;
			}
			else
				return false;
		}
	}
	return true;
}

void KTorrentView::stopDownload(kt::TorrentInterface* tc)
{
	if (tc && tc->getStats().running)
	{
		wantToStop(tc,true);
	}
}

void KTorrentView::showStartError()
{
	TQString err = i18n("Cannot start more than 1 download, ",
					   "Cannot start more than %n downloads, ",Settings::maxDownloads());
		
	err += i18n("and 1 seed. ","and %n seeds. ",Settings::maxSeeds());
	err += i18n("Go to Settings -> Configure KTorrent, if you want to change the limits.");
	KMessageBox::error(this,err);
}

void KTorrentView::startDownloads()
{
	bool err = false;
	
	TQPtrList<TQListViewItem> sel = view->selectedItems();
	for (TQPtrList<TQListViewItem>::iterator itr = sel.begin(); itr != sel.end();itr++)
	{
		KTorrentViewItem* kvi = (KTorrentViewItem*)*itr;
		TorrentInterface* tc = kvi->getTC();
		if (!startDownload(tc))
			err = true;;
	}

	/*
	if (err)
	{
		showStartError();
	}
	*/
	// make sure toolbuttons get updated
	onSelectionChanged();
}
	
void KTorrentView::stopDownloads()
{
	TQPtrList<TQListViewItem> sel = view->selectedItems();
	for (TQPtrList<TQListViewItem>::iterator itr = sel.begin(); itr != sel.end();itr++)
	{
		KTorrentViewItem* kvi = (KTorrentViewItem*)*itr;
		stopDownload(kvi->getTC());
	}
	
	// make sure toolbuttons get updated
	onSelectionChanged();
}

void KTorrentView::startAllDownloads()
{
	bool err = false;
	TQMap<TorrentInterface*,KTorrentViewItem*>::iterator i = items.begin();
	while (i != items.end())
	{
		KTorrentViewItem* tvi = i.data();
		if (tvi && !startDownload(tvi->getTC()))
			err = true;
			
		i++;
	}
	/*
	if (err)
	{
		showStartError();
	}
	*/
	onSelectionChanged();
}

void KTorrentView::stopAllDownloads()
{
	TQMap<TorrentInterface*,KTorrentViewItem*>::iterator i = items.begin();
	while (i != items.end())
	{
		KTorrentViewItem* tvi = i.data();
		if (tvi)
			stopDownload(tvi->getTC());
	
		i++;
	}
	onSelectionChanged();
}
	
void KTorrentView::removeDownloads()
{
	TQPtrList<TQListViewItem> sel = view->selectedItems();
	for (TQPtrList<TQListViewItem>::iterator itr = sel.begin(); itr != sel.end();itr++)
	{
		KTorrentViewItem* kvi = (KTorrentViewItem*)*itr;
		TorrentInterface* tc = kvi->getTC();
		bool dummy = false;
		if (tc && !tc->isCheckingData(dummy))
		{	
			const TorrentStats & s = tc->getStats();
			bool data_to = false;
			if (!s.completed)
			{
				TQString msg = i18n("The torrent %1 has not finished downloading, "
						"do you want to delete the incomplete data, too?").arg(s.torrent_name);
				int ret = KMessageBox::questionYesNoCancel(
						this,msg,i18n("Remove Download"),
						i18n("Delete Data"),i18n("Keep Data"));
				if (ret == KMessageBox::Cancel)
					return;
				else if (ret == KMessageBox::Yes)
					data_to = true;
			}
			wantToRemove(tc,data_to);
		}
	}
	
	// make sure toolbuttons get updated
	onSelectionChanged();
}

void KTorrentView::removeDownloadsAndData()
{
	TQString msg = i18n("You will lose all the downloaded data. Are you sure you want to do this?");
        // TODO: replace i18n("Remove") by KStdGuiItem::remove() in KDE4
	if (KMessageBox::warningYesNo(this,msg, i18n("Remove Torrent"), i18n("&Remove"), 
            KStdGuiItem::cancel()) == KMessageBox::No)
		return;
	
	TQPtrList<TQListViewItem> sel = view->selectedItems();
	for (TQPtrList<TQListViewItem>::iterator itr = sel.begin(); itr != sel.end();itr++)
	{
		KTorrentViewItem* kvi = (KTorrentViewItem*)*itr;
		TorrentInterface* tc = kvi->getTC();
		if (tc)
			wantToRemove(tc,true);
	}
	
	// make sure toolbuttons get updated
	onSelectionChanged();
}

void KTorrentView::manualAnnounce()
{
	TQPtrList<TQListViewItem> sel = view->selectedItems();
	for (TQPtrList<TQListViewItem>::iterator itr = sel.begin(); itr != sel.end();itr++)
	{
		KTorrentViewItem* kvi = (KTorrentViewItem*)*itr;
		TorrentInterface* tc = kvi->getTC();
		if (tc && tc->getStats().running)
			tc->updateTracker();
	}
}

void KTorrentView::previewFiles() 
{
	TQPtrList<TQListViewItem> sel = view->selectedItems();
	for (TQPtrList<TQListViewItem>::iterator itr = sel.begin(); itr != sel.end();itr++)
	{
		KTorrentViewItem* kvi = (KTorrentViewItem*)*itr;
		TorrentInterface* tc = kvi->getTC();
		if (tc && tc->readyForPreview() && !tc->getStats().multi_file_torrent)
		{
			TQFileInfo fi(tc->getTorDir()+"cache");
			new KRun(KURL::fromPathOrURL(fi.readLink()), 0, true, true);
		}
	}
}

TorrentInterface* KTorrentView::getCurrentTC()
{
	KTorrentViewItem* tvi = dynamic_cast<KTorrentViewItem*>(view->currentItem());
	if (tvi)
		return tvi->getTC();
	else
		return 0;
}

void KTorrentView::onExecuted(TQListViewItem* item)
{
	KTorrentViewItem* tvi = dynamic_cast<KTorrentViewItem*>(item);
	if (tvi)
	{
		torrentClicked(tvi->getTC());
		currentChanged(tvi->getTC());
	}
	else
	{
		currentChanged(0);
	}
}

void KTorrentView::showContextMenu(TDEListView* ,TQListViewItem*,const TQPoint & p)
{
	updateGroupsSubMenu(menu->getGroupsSubMenu());
	menu->show(p);
}

void KTorrentView::addTorrent(TorrentInterface* tc)
{
	if (current_group && !current_group->isMember(tc))
	{
		items.insert(tc,0);
	}
	else
	{
		KTorrentViewItem* tvi = new KTorrentViewItem(this,tc);
		items.insert(tc,tvi);
		tvi->update();
		if (items.count() == 1)
			currentChanged(tc);
	}
}

void KTorrentView::removeTorrent(TorrentInterface* tc)
{
	TQMap<kt::TorrentInterface*,KTorrentViewItem*>::iterator i = items.find(tc);
	if (i != items.end())
	{
		KTorrentViewItem* tvi = i.data();
		items.erase(i);
		delete tvi;
		tvi = dynamic_cast<KTorrentViewItem*>(view->currentItem());
		if (tvi)
			currentChanged(tvi->getTC());
		else
			currentChanged(0);
	}
}


void KTorrentView::update()
{	
	Uint32 r = 0;
	Uint32 t = 0;
	
	TQMap<kt::TorrentInterface*,KTorrentViewItem*>::iterator i = items.begin();
	while (i != items.end())
	{
		bool count = true;
		KTorrentViewItem* tvi = i.data();
		if (tvi)
			tvi->update();
		// check if the torrent still is part of the group
		kt::TorrentInterface* ti = i.key();
		
		bool member = (current_group->isMember(ti) && filter_bar->matchesFilter(ti));
		
		if (tvi && current_group && !member)
		{
			// torrent is no longer a member of this group so remove it from the view
			delete tvi;
			i.data() = 0;
			count = false;
		}
		else if (!tvi && (!current_group || member))
		{
			tvi = new KTorrentViewItem(this,ti);
			i.data() = tvi;
		}
		
		if (i.data())
		{
			t++;
			if (ti->getStats().running)
				r++;
		}
		
		i++;
	}
	
	if (running != r || total != t)
	{
		running = r;
		total = t;
		
		if (current_group)
			setCaption(TQString("%1 %2/%3").arg(current_group->groupName()).arg(running).arg(total));
		else
			setCaption(i18n("All Torrents %1/%2").arg(running).arg(total));
		onSelectionChanged();
	}
	
	view->sort();
}

bool KTorrentView::acceptDrag(TQDropEvent* event) const
{
	// accept uri drops only
	return KURLDrag::canDecode(event);
}

void KTorrentView::onSelectionChanged()
{
	int flags = 0;
	
	TQPtrList<TQListViewItem> sel = view->selectedItems();
	for (TQPtrList<TQListViewItem>::iterator itr = sel.begin(); itr != sel.end();itr++)
	{
		KTorrentViewItem* kvi = (KTorrentViewItem*)*itr;
		TorrentInterface* tc = kvi->getTC();
		bool dummy;
		if (tc && !tc->isCheckingData(dummy))
		{
			const TorrentStats & s = tc->getStats();
			if (!s.running)
				flags |= START;
			else
				flags |= STOP;
			
			if (flags & (START|STOP))
				break;
		}
	}
	
	if (sel.count() > 0)
		flags |= REMOVE;
	
	if (sel.count() == 1)
		flags |= SCAN;
	
	if (running > 0)
		flags |= STOP_ALL;
	
	if (running == 0 && total > 0)
		flags |= START_ALL;
	
	updateActions(flags);
}

void KTorrentView::queueSlot()
{
	TQPtrList<TQListViewItem> sel = view->selectedItems();
	for (TQPtrList<TQListViewItem>::iterator itr = sel.begin(); itr != sel.end();itr++)
	{
		KTorrentViewItem* kvi = (KTorrentViewItem*)*itr;
		TorrentInterface* tc = kvi->getTC();
		bool dummy;
		if (tc && !tc->isCheckingData(dummy))
			emit queue(tc);
	}
}


void KTorrentView::checkDataIntegrity()
{	
	TQPtrList<TQListViewItem> sel = view->selectedItems();
	if (sel.count() == 0)
		return;
	
	bool dummy = false;
	KTorrentViewItem* kvi = (KTorrentViewItem*)sel.first();
	TorrentInterface* tc = kvi->getTC();
	if (!tc->isCheckingData(dummy))
	{
		needsDataCheck(tc);
	}
	else
	{
		KMessageBox::error(0,i18n("You are already checking the data of the torrent %1 !").arg(tc->getStats().torrent_name));
	}
}

TQDragObject* KTorrentView::dragObject()
{
	TQPtrList<TQListViewItem> sel = view->selectedItems();
	if (sel.count() == 0)
		return 0;
	 
	return new TorrentDrag(this);
}

void KTorrentView::getSelection(TQValueList<kt::TorrentInterface*> & sel)
{
	TQPtrList<TQListViewItem> s = view->selectedItems();
	if (s.count() == 0)
		return;
	
	TQPtrList<TQListViewItem>::iterator i = s.begin();
	while (i != s.end())
	{
		KTorrentViewItem* kvi = (KTorrentViewItem*)*i;
		TorrentInterface* tc = kvi->getTC();
		sel.append(tc);
		i++;
	}
}

void KTorrentView::removeFromGroup()
{
	TQPtrList<TQListViewItem> s = view->selectedItems();
	if (s.count() == 0 || !current_group || current_group->isStandardGroup())
		return;
	
	TQPtrList<TQListViewItem>::iterator i = s.begin();
	while (i != s.end())
	{
		KTorrentViewItem* kvi = (KTorrentViewItem*)*i;
		TorrentInterface* tc = kvi->getTC();
		current_group->removeTorrent(tc);
		delete kvi;
		items[tc] = 0;
		i++;
	}
}

void KTorrentView::addSelectionToGroup(kt::Group* g)
{
	TQPtrList<TQListViewItem> s = view->selectedItems();
	if (s.count() == 0 || !g)
		return;
	
	TQPtrList<TQListViewItem>::iterator i = s.begin();
	while (i != s.end())
	{
		KTorrentViewItem* kvi = (KTorrentViewItem*)*i;
		TorrentInterface* tc = kvi->getTC();
		g->addTorrent(tc);
		i++;
	}
}

void KTorrentView::showAddPeersWidget()
{
	TQPtrList<TQListViewItem> sel = view->selectedItems();
	for (TQPtrList<TQListViewItem>::iterator itr = sel.begin(); itr != sel.end();itr++)
	{
		KTorrentViewItem* kvi = (KTorrentViewItem*)*itr;
		TorrentInterface* tc = kvi->getTC();
		bool dummy;
		if (tc && !tc->isCheckingData(dummy))
		{
			AddPeerWidget dlg(tc, this);
			dlg.exec();
		}
	}
}

void KTorrentView::openOutputDirectory()
{
	TQPtrList<TQListViewItem> sel = view->selectedItems();
	for (TQPtrList<TQListViewItem>::iterator itr = sel.begin(); itr != sel.end();itr++)
	{
		KTorrentViewItem* kvi = (KTorrentViewItem*)*itr;
		TorrentInterface* tc = kvi->getTC();
		if (tc)
		{
			if(tc->getStats().multi_file_torrent)
				new KRun(KURL::fromPathOrURL(tc->getStats().output_path), 0, true, true);
			else
				new KRun(KURL::fromPathOrURL(tc->getDataDir()), 0, true, true);
		}
	}
}

void KTorrentView::openTorXDirectory()
{
	TQPtrList<TQListViewItem> sel = view->selectedItems();
	for (TQPtrList<TQListViewItem>::iterator itr = sel.begin(); itr != sel.end();itr++)
	{
		KTorrentViewItem* kvi = (KTorrentViewItem*)*itr;
		TorrentInterface* tc = kvi->getTC();
		if (tc)
		{
			new KRun(KURL::fromPathOrURL(tc->getTorDir()), 0, true, true);
		}
	}
}

void KTorrentView::setDownloadLocationSlot()
{
	TQPtrList<TQListViewItem> sel = view->selectedItems();
	for (TQPtrList<TQListViewItem>::iterator itr = sel.begin(); itr != sel.end();itr++)
	{
		KTorrentViewItem* kvi = (KTorrentViewItem*)*itr;
		TorrentInterface* tc = kvi->getTC();
		if (tc)
		{
			TQString dn;
			dn = KFileDialog::getExistingDirectory(tc->getStats().output_path, this, i18n("Choose download location for %1").arg(tc->getStats().torrent_name));
								
			if(dn.isNull() || dn.isEmpty())
				continue;
								
			if(!dn.endsWith(bt::DirSeparator()))
				dn += bt::DirSeparator();
			
			if(!dn.isEmpty())
				tc->changeOutputDir(dn);
		}
	}
}

void KTorrentView::dhtSlot()
{
	TQPtrList<TQListViewItem> sel = view->selectedItems();
	for (TQPtrList<TQListViewItem>::iterator itr = sel.begin(); itr != sel.end();itr++)
	{
		KTorrentViewItem* kvi = (KTorrentViewItem*)*itr;
		TorrentInterface* tc = kvi->getTC();
		bool dummy;
		if (tc && !tc->isCheckingData(dummy))
		{
			bool on = tc->isFeatureEnabled(kt::DHT_FEATURE);
			tc->setFeatureEnabled(kt::DHT_FEATURE,!on);
		}
	}
}

void KTorrentView::utPexSlot()
{
	TQPtrList<TQListViewItem> sel = view->selectedItems();
	for (TQPtrList<TQListViewItem>::iterator itr = sel.begin(); itr != sel.end();itr++)
	{
		KTorrentViewItem* kvi = (KTorrentViewItem*)*itr;
		TorrentInterface* tc = kvi->getTC();
		bool dummy;
		if (tc && !tc->isCheckingData(dummy))
		{
			bool on = tc->isFeatureEnabled(kt::UT_PEX_FEATURE);
			tc->setFeatureEnabled(kt::UT_PEX_FEATURE,!on);
		}
	}
}

void KTorrentView::speedLimits()
{
	TQPtrList<TQListViewItem> sel = view->selectedItems();
	if (sel.count() != 1)
		return;
	
	KTorrentViewItem* kvi = (KTorrentViewItem*)sel.first();
	TorrentInterface* tc = kvi->getTC();
	SpeedLimitsDlg dlg(tc,0);
	dlg.show();
	dlg.exec();
}


void KTorrentView::columnHide(int index)
{
	view->hideColumn(index);
	view->header()->setResizeEnabled(FALSE, index);
}

void KTorrentView::columnShow(int index)
{
	view->setColumnWidth(index, 100);
	view->header()->setResizeEnabled(TRUE, index);
}

bool KTorrentView::columnVisible(int index)
{
	if (index < view->columns() && index >= 0) 
		return view->columnWidth(index) != 0;
	else
		return true;
}

void KTorrentView::onColumnVisibilityChange(int id)
{
	int mid = m_headerMenu->indexOf(id) - 1;
	if(mid == -1)
		return;

	bool visible = !columnVisible(mid);
	
	m_headerMenu->setItemChecked(id, visible);
	
	if(visible)
		columnShow(mid);
	else
		columnHide(mid);
}

void KTorrentView::gsmItemActived(const TQString & group)
{
	groupsSubMenuItemActivated(this,group);
}

void KTorrentView::updateCaption()
{
	Uint32 r = 0;
	Uint32 t = 0;
	TQMap<kt::TorrentInterface*,KTorrentViewItem*>::iterator i = items.begin();
	while (i != items.end())
	{
		if (!current_group || current_group->isMember(i.key()))
		{
			t++;
			if (i.key()->getStats().running)
				r++;
		}
		
		i++;
	}
	
	if (running != r || total != t)
	{
		running = r;
		total = t;
		
		if (current_group)
			setCaption(TQString("%1 %2/%3").arg(current_group->groupName()).arg(running).arg(total));
		else
			setCaption(i18n("All Torrents %1/%2").arg(running).arg(total));
	}
}

void KTorrentView::setupViewColumns()
{
	if (current_group->groupFlags() == kt::Group::UPLOADS_ONLY_GROUP)
	{
		// we are an uploads group so lets hide, downloaded, percentage, download speed and time left
		columnHide(2); // Downloaded
		columnHide(5); // Down speed
		columnHide(7); // Time left
		columnHide(10); // Percentage
	}
	else if (current_group->groupFlags() == kt::Group::DOWNLOADS_ONLY_GROUP)
	{
		columnHide(13); // Seed time is not necessary for a download
	}
	
	// make sure menu is OK
	for(int i=0; i< view->columns();++i)
	{
		bool visible = columnVisible(i);
		m_headerMenu->setItemChecked(m_headerMenu->idAt(i+1), visible);
		view->header()->setResizeEnabled(visible, i);
	}
}



void KTorrentView::toggleFilterBar()
{
	filter_bar->setHidden(!filter_bar->isHidden());
}

#include "ktorrentview.moc"

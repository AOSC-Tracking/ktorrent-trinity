/***************************************************************************
 *   Copyright (C) 2005 by Joris Guisson                                   *
 *   joris.guisson@gmail.com                                               *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
 ***************************************************************************/
#include <tdeglobal.h>
#include <tdelocale.h>
#include <kiconloader.h>
#include <interfaces/torrentinterface.h>
#include <groups/group.h>
#include "ktorrentviewmenu.h"
#include "ktorrentviewitem.h"
#include "ktorrentview.h"
		
using namespace kt;

KTorrentViewMenu::KTorrentViewMenu (KTorrentView *parent, const char *name )
		: TDEPopupMenu ( parent, name ),view(parent)
{
	TDEIconLoader* iload = TDEGlobal::iconLoader();
	
	stop_id = insertItem(
			iload->loadIconSet("ktstop",TDEIcon::Small),i18n("to stop", "Stop"),
			parent,TQ_SLOT(stopDownloads()));

	start_id = insertItem(
			iload->loadIconSet("ktstart",TDEIcon::Small),i18n("to start", "Start"),
			parent,TQ_SLOT(startDownloads()));

	remove_id = insertItem(
			iload->loadIconSet("ktremove",TDEIcon::Small),i18n("Remove Torrent"),
			parent,TQ_SLOT(removeDownloads()));
	
	remove_all_id = insertItem(
			iload->loadIconSet("ktremove",TDEIcon::Small),i18n("Remove Torrent and Data"),
			parent,TQ_SLOT(removeDownloadsAndData()));
	
	queue_id = insertItem(
			iload->loadIconSet("player_playlist",TDEIcon::Small),i18n("Enqueue/Dequeue"),
			parent,TQ_SLOT(queueSlot()));
	
	insertSeparator();
	
	add_peer_id = insertItem(
			iload->loadIconSet("add", TDEIcon::Small), i18n("Add Peers"),
			parent, TQ_SLOT(showAddPeersWidget())); 
	
	peer_sources_menu = new TDEPopupMenu(this);
	peer_sources_id = insertItem(i18n("Additional Peer Sources"), peer_sources_menu);
	peer_sources_menu->insertTitle(i18n("Torrent Peer Sources:"));
	peer_sources_menu->setCheckable(true);
	dht_id = peer_sources_menu->insertItem(i18n("DHT"), parent, TQ_SLOT(dhtSlot()));
	ut_pex_id = peer_sources_menu->insertItem(i18n("Peer Exchange"), parent, TQ_SLOT(utPexSlot()));
	
	insertSeparator();
	
	announce_id = insertItem(
			iload->loadIconSet("apply",TDEIcon::Small),i18n("Manual Announce"),
			parent,TQ_SLOT(manualAnnounce())); 
	
	preview_id = insertItem(
			iload->loadIconSet("frame_image",TDEIcon::Small),i18n("Preview"), 
			parent, TQ_SLOT(previewFiles()));
	
	insertSeparator();
	dirs_sub_menu = new TDEPopupMenu(this);
	dirs_id = insertItem(i18n("Open Directory"), dirs_sub_menu);
	outputdir_id = dirs_sub_menu->insertItem(iload->loadIconSet("folder",TDEIcon::Small), i18n("Data Directory"), 
											 parent, TQ_SLOT(openOutputDirectory()));
	torxdir_id = dirs_sub_menu->insertItem(iload->loadIconSet("folder",TDEIcon::Small), i18n("Temporary Directory"),
										   parent, TQ_SLOT(openTorXDirectory()));
	
	downloaddir_id = insertItem(i18n("Set Download Location"), parent, TQ_SLOT(setDownloadLocationSlot()));
	
	insertSeparator();
	remove_from_group_id =  insertItem(i18n("Remove From Group"),parent, TQ_SLOT(removeFromGroup()));
	groups_sub_menu = new TDEPopupMenu(this);
	
	add_to_group_id = insertItem(i18n("Add to Group"),groups_sub_menu);
	
	insertSeparator();
	scan_id = insertItem(i18n("Check Data Integrity"),parent, TQ_SLOT(checkDataIntegrity()));	
	
	connect(groups_sub_menu,TQ_SIGNAL(activated(int)),this,TQ_SLOT(gsmItemActived(int)));
	
	traffic_lim_id = insertItem(i18n("Speed Limits"),parent,TQ_SLOT(speedLimits()));
}


KTorrentViewMenu::~KTorrentViewMenu()
{}

void KTorrentViewMenu::gsmItemActived(int id)
{
	groupItemActivated(groups_sub_menu->text(id).remove('&'));
}

void KTorrentViewMenu::show(const TQPoint & p)
{
	bool en_start = false;
	bool en_stop = false;
	bool en_remove = false;
	bool en_prev = false;
	bool en_announce = false;
	bool en_add_peer = false;
	bool en_dirs = false;
	bool en_peer_sources = false;
	bool dummy = false;
	
	TQPtrList<TQListViewItem> sel = view->selectedItems();
	for (TQPtrList<TQListViewItem>::iterator itr = sel.begin(); itr != sel.end();itr++)
	{
		KTorrentViewItem* kvi = (KTorrentViewItem*)*itr;
		TorrentInterface* tc = kvi->getTC();
		if (tc)
		{
			const TorrentStats & s = tc->getStats();
			
			if (tc->readyForPreview() && !s.multi_file_torrent)
				en_prev = true;
			
			if (!tc->isCheckingData(dummy))
				en_remove = true;
			
			if (!s.running)
			{
				if (!tc->isCheckingData(dummy))
				{
					en_start = true;
				}
			}
			else
			{
				if (!tc->isCheckingData(dummy))
				{
					en_stop = true;
					if (tc->announceAllowed())
						en_announce = true;
				}
			}
			
			if (!s.priv_torrent && !tc->isCheckingData(dummy))
			{
				en_add_peer = true;
				en_peer_sources = true;
			}
		}
	}
	
	en_add_peer = en_add_peer && en_stop;
	
	setItemEnabled(start_id,en_start);
	setItemEnabled(stop_id,en_stop);
	setItemEnabled(remove_id,en_remove);
	setItemEnabled(remove_all_id,en_remove);
	setItemEnabled(preview_id,en_prev);
	setItemEnabled(add_peer_id, en_add_peer);
	setItemEnabled(announce_id,en_announce);
	setItemEnabled(queue_id, en_remove);
	
	const kt::Group* current_group = view->getCurrentGroup();
	setItemEnabled(remove_from_group_id,current_group && !current_group->isStandardGroup());
	setItemEnabled(add_to_group_id,groups_sub_menu->count() > 0);
	
	if (sel.count() == 1)
	{
		//enable directories
		en_dirs = true;
		
		KTorrentViewItem* kvi = (KTorrentViewItem*)sel.getFirst();
		TorrentInterface* tc = kvi->getTC();
		// no data check when we are preallocating diskspace
		if (tc->getStats().status == kt::ALLOCATING_DISKSPACE || tc->isCheckingData(dummy))
			setItemEnabled(scan_id, false);
		else
			setItemEnabled(scan_id, true);
		
		//enable additional peer sources if torrent is not private
		setItemEnabled(peer_sources_id, en_peer_sources);
		
		if (en_peer_sources)
		{
			peer_sources_menu->setItemChecked(dht_id, tc->isFeatureEnabled(kt::DHT_FEATURE));
			peer_sources_menu->setItemChecked(ut_pex_id, tc->isFeatureEnabled(kt::UT_PEX_FEATURE));
		}
	}
	else
	{
		setItemEnabled(scan_id,false);
		
		//disable peer source
		setItemEnabled(peer_sources_id, false);	
	}
	
	setItemEnabled(dirs_id, en_dirs);
	setItemEnabled(traffic_lim_id,sel.count() == 1);
	setItemEnabled(add_to_group_id,sel.count() > 0);
	setItemEnabled(downloaddir_id,sel.count() > 0);
	
	popup(p);
}

#include "ktorrentviewmenu.moc"

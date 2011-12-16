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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.             *
 ***************************************************************************/
#include "trackerview.h"
#include <interfaces/torrentinterface.h>
#include <interfaces/trackerslist.h>

#include <tqdatetime.h>
#include <tqstring.h>
#include <tqlabel.h>
#include <tqlistview.h>
#include <tqlineedit.h>
#include <tqpushbutton.h>
#include <tqtooltip.h>

#include <klocale.h>
#include <kurl.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <ksqueezedtextlabel.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klistview.h>

#include <torrent/globals.h>
#include <util/log.h>

namespace kt
{
	TrackerView::TrackerView(TQWidget *parent, const char *name)
		:TrackerViewBase(parent, name), tc(0)
	{
		KIconLoader* iload = KGlobal::iconLoader();
		btnUpdate->setIconSet(iload->loadIconSet("apply", KIcon::Small));
		btnAdd->setIconSet(iload->loadIconSet("add", KIcon::Small));
		btnRemove->setIconSet(iload->loadIconSet("remove", KIcon::Small));
		btnRestore->setIconSet(iload->loadIconSet("undo", KIcon::Small));
		
		TQPalette p = lblCurrent->palette();
		p.setColor(TQPalette::Active,TQColorGroup::Base,p.color(TQPalette::Active,TQColorGroup::Background));
		lblCurrent->setPalette(p);
	}
	
	TrackerView::~TrackerView()
	{
	}

	void TrackerView::btnAdd_clicked()
	{
		if(!tc || txtTracker->text().isEmpty())
			return;
		
		if(tc->getStats().priv_torrent)
		{
			KMessageBox::sorry(0, i18n("Cannot add a tracker to a private torrent."));
			return;
		}
		
		KURL url(txtTracker->text());
		if(!url.isValid())
		{
			KMessageBox::error(0, i18n("Malformed URL."));
			return;
		}
			
		new TQListViewItem(listTrackers, txtTracker->text());
		tc->getTrackersList()->addTracker(url,true);
	}

	void TrackerView::btnRemove_clicked()
	{
		TQListViewItem* current = listTrackers->currentItem();
		if(!current)
			return;
		
		KURL url(current->text(0));
		if(tc->getTrackersList()->removeTracker(url))
			delete current;
		else
			KMessageBox::sorry(0, i18n("Cannot remove torrent default tracker."));
	}

	void TrackerView::btnChange_clicked()
	{
		TQListViewItem* current = listTrackers->currentItem();
		if(!current)
			return;
		
		KURL url(current->text(0));
		tc->getTrackersList()->setTracker(url);
		tc->updateTracker();
	}

	void TrackerView::btnRestore_clicked()
	{
		tc->getTrackersList()->restoreDefault();
		tc->updateTracker();
		
		// update the list of trackers
		listTrackers->clear();
		
		const KURL::List trackers = tc->getTrackersList()->getTrackerURLs();
		if(trackers.empty())
			return;
		
		for (KURL::List::const_iterator i = trackers.begin();i != trackers.end();i++)
			new TQListViewItem(listTrackers, (*i).prettyURL());
	}

	void TrackerView::btnUpdate_clicked()
	{
		if(!tc)
			return;
		
		tc->updateTracker();
	}
	
	void TrackerView::listTrackers_currentChanged(TQListViewItem* item)
	{
		if(!item)
			txtTracker->clear();
		else
			txtTracker->setText(item->text(0));
	}
	
	void TrackerView::changeTC(TorrentInterface* ti)
	{
		if (tc == ti)
			return;
		
		setEnabled(ti != 0);
		torrentChanged(ti);
		update();
	}
	
	void TrackerView::update()
	{
		if(!tc)
			return;
		
		const TorrentStats & s = tc->getStats();
		if (s.running)
		{
			TQTime t;
			t = t.addSecs(tc->getTimeToNextTrackerUpdate());
			lblUpdate->setText(t.toString("mm:ss"));
		}
		
		//Update manual annunce button
		btnUpdate->setEnabled(s.running && tc->announceAllowed());
		// only enable change when we can actually change and the torrent is running
		btnChange->setEnabled(s.running && listTrackers->childCount() > 1);

		lbltqStatus->setText("<b>" + s.trackerstatus + "</b>");
		if (tc->getTrackersList())
		{
			TQString t = tc->getTrackersList()->getTrackerURL().prettyURL();
			if (lblCurrent->text() != t )
				lblCurrent->setText(t);
		}
		else
			lblCurrent->clear();
		
		btnAdd->setEnabled(txtTracker->text() != TQString() && !tc->getStats().priv_torrent);
	}
	
	void TrackerView::onLoadingFinished(const KURL & ,bool,bool)
	{
		torrentChanged(tc);
	}
	
	void TrackerView::torrentChanged(TorrentInterface* ti)
	{
		tc = ti;
		listTrackers->clear();
		if(!tc)
		{
			lbltqStatus->clear();
			lblCurrent->clear();
			lblUpdate->clear();
			txtTracker->clear();
			
			btnAdd->setEnabled(false);
			btnRemove->setEnabled(false);
			btnRestore->setEnabled(false);
			btnChange->setEnabled(false);
			btnRestore->setEnabled(false);
			return;
		}
		
		const TorrentStats & s = tc->getStats();
		
		if (s.priv_torrent)
		{
			btnAdd->setEnabled(false);
			btnRemove->setEnabled(false);
			btnRestore->setEnabled(false);
			txtTracker->setText(i18n("You cannot add trackers to a private torrent"));
			txtTracker->setEnabled(false);
		}
		else
		{
			btnAdd->setEnabled(true);
			btnRemove->setEnabled(true);
			btnRestore->setEnabled(true);
			txtTracker->clear();
			txtTracker->setEnabled(true);
		}
		
		const KURL::List trackers = tc->getTrackersList()->getTrackerURLs();
		if(trackers.empty())
		{
			new TQListViewItem(listTrackers, tc->getTrackersList()->getTrackerURL().prettyURL());
		}
		else
		{
			for (KURL::List::const_iterator i = trackers.begin();i != trackers.end();i++)
				new TQListViewItem(listTrackers, (*i).prettyURL());
		}
		
		btnUpdate->setEnabled(s.running && tc->announceAllowed());
		btnChange->setEnabled(s.running && listTrackers->childCount() > 1);
	}
}


#include "trackerview.moc"

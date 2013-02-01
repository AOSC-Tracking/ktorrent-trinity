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
#ifndef PEERVIEW_H
#define PEERVIEW_H

#include <tqmap.h>
#include <klistview.h>
#include <tqlistview.h>
#include <kpopupmenu.h>
#include <tqpoint.h>
#include <util/constants.h>

namespace kt
{
	class PeerInterface;
	class PeerView;
	
	class PeerViewItem : public TDEListViewItem
	{
		kt::PeerInterface* peer;
		TQString m_country;
		bt::Uint32 ip;
		// counter to keep track of how many PeerViewItem objects are in existence
		static bt::Uint32 pvi_count;
	public:
		PeerViewItem(PeerView* pv,kt::PeerInterface* peer);
		virtual ~PeerViewItem();
	
		void update();
		int compare(TQListViewItem * i,int col,bool) const;
		kt::PeerInterface* getPeer() { return peer; }
		
	};
	
	/**
	@author Joris Guisson
	*/
	class PeerView : public TDEListView
	{
		Q_OBJECT
  
		
		TQMap<kt::PeerInterface*,PeerViewItem*> items;
	public:
		PeerView(TQWidget *parent = 0, const char *name = 0);
		virtual ~PeerView();
		
	public slots:
		void addPeer(kt::PeerInterface* peer);
		void removePeer(kt::PeerInterface* peer);
		void banPeer(kt::PeerInterface* peer);
		void kickPeer(kt::PeerInterface* peer);
		void update();
		void removeAll();
		void showContextMenu(TDEListView* ,TQListViewItem* item,const TQPoint & p);
		void contextItem(int id);
	private:
		TDEPopupMenu* menu;
		int ban_id;
		int kick_id;
		PeerViewItem* curr;
	};
}

#endif

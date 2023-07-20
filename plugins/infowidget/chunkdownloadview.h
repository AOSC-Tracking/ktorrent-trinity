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
#ifndef BTCHUNKDOWNLOADVIEW_H
#define BTCHUNKDOWNLOADVIEW_H

#include <tdelistview.h>
#include <tqmap.h>
#include "chunkdownloadviewbase.h"

namespace kt
{
	class ChunkDownloadInterface;
	class ChunkDownloadView;
	
	class ChunkDownloadViewItem : public TDEListViewItem
	{
		kt::ChunkDownloadInterface* cd;
	public:
		ChunkDownloadViewItem(TDEListView* cdv,kt::ChunkDownloadInterface* cd);
	
		void update();
		int compare(TQListViewItem * i,int col,bool) const;
	};
	
	
	/**
	@author Joris Guisson
	*/
	class ChunkDownloadView : public ChunkDownloadViewBase
	{
		TQ_OBJECT
  
	
		TQMap<kt::ChunkDownloadInterface*,ChunkDownloadViewItem*> items;
		kt::TorrentInterface* curr_tc;
	public:
		ChunkDownloadView(TQWidget *parent = 0, const char *name = 0);
		virtual ~ChunkDownloadView();
		
		void saveLayout(TDEConfig* cfg,const TQString & group_name);
		void restoreLayout(TDEConfig* cfg,const TQString & group_name);
		void clear();
		void update();
		void changeTC(kt::TorrentInterface* tc);
	
	public slots:
		void addDownload(kt::ChunkDownloadInterface* cd);
		void removeDownload(kt::ChunkDownloadInterface* cd);
		void removeAll();
	};
}	


#endif

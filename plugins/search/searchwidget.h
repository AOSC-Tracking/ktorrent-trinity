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
#ifndef BTSEARCHWIDGET_H
#define BTSEARCHWIDGET_H

#include <tqwidget.h>
#include <tqvaluevector.h>
#include <kurl.h>

class SearchBar;
class KProgress;
class TDEPopupMenu;

namespace KParts
{
	class Part;
}

namespace kt
{
	class HTMLPart;
	class SearchPlugin;
	class SearchEngineList;
	
	
	/**
		@author Joris Guisson
		
		Widget which shows a TDEHTML window with the users search in it
	*/
	class SearchWidget : public TQWidget
	{
		Q_OBJECT
  
	public:
		SearchWidget(SearchPlugin* sp);
		virtual ~SearchWidget();
	
		TDEPopupMenu* rightClickMenu();
		
		void updateSearchEngines(const SearchEngineList & sl);
	
	public slots:
		void search(const TQString & text,int engine = 0);
		void copy();
		void onShutDown();
	
	private slots:
		void searchPressed();
		void clearPressed();
		void onURLHover(const TQString & url);
		void onFinished();
		void onOpenTorrent(const KURL & url);
		void onSaveTorrent(const KURL & url);
		void showPopupMenu(const TQString & s,const TQPoint & p);
		void onBackAvailable(bool available);
		void onFrameAdded(KParts::Part* p);
		void statusBarMsg(const TQString & url);
		void openTorrent(const KURL & url);
		void loadingProgress(int perc);
		
	private:
		HTMLPart* html_part;
		SearchBar* sbar;
		TDEPopupMenu* right_click_menu;
		int back_id;
		SearchPlugin* sp;
		KProgress* prog;
	};

}

#endif

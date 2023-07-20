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
#ifndef KTSEARCHPLUGIN_H
#define KTSEARCHPLUGIN_H

#include <tqptrlist.h>
#include <interfaces/plugin.h>
#include <interfaces/guiinterface.h>
#include "searchenginelist.h"

namespace kt
{
	class SearchWidget;
	class SearchPrefPage;
	class SearchTab;

	/**
	@author Joris Guisson
	*/
	class SearchPlugin : public Plugin, public kt::CloseTabListener
	{
		TQ_OBJECT
  
	public:
		SearchPlugin(TQObject* parent, const char* name, const TQStringList& args);
		virtual ~SearchPlugin();

		virtual void load();
		virtual void unload();
		virtual bool versionCheck(const TQString& version) const;
		
		void preferencesUpdated();
		
		const SearchEngineList & getSearchEngineList() const {return engines;}
	private slots:
		void search(const TQString & text,int engine,bool external);
		
	private:
		virtual void tabCloseRequest(kt::GUIInterface* gui,TQWidget* tab);
		
	private:
		SearchPrefPage* pref;
		SearchTab* tab;
		SearchEngineList engines;
		TQPtrList<SearchWidget> searches;
	};

}

#endif

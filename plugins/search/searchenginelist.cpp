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
#include <tqfile.h>
#include <tqtextstream.h>
#include <tqstringlist.h>
#include "searchenginelist.h"

using namespace bt;

namespace kt
{

	SearchEngineList::SearchEngineList()
	{}


	SearchEngineList::~SearchEngineList()
	{}

	void SearchEngineList::save(const TQString& file)
	{
		TQFile fptr(file);
		if (!fptr.open(IO_WriteOnly))
			return;
		
		TQTextStream out(&fptr);
		out << "# PLEASE DO NOT MODIFY THIS FILE. Use KTorrent configuration dialog for adding new search engines." << ::endl;
		out << "# SEARCH ENGINES list" << ::endl;
     
		TQValueList<SearchEngine>::iterator i = m_search_engines.begin();
		while (i != m_search_engines.end())
		{
			SearchEngine & e = *i;

			// replace spaces by %20
			TQString name = e.name;
			name = name.replace(" ","%20");
			TQString u = e.url.prettyURL();
			u = u.replace(" ","%20");
			out << name << " " << u << ::endl;
			i++;
		}
	}
	
	void SearchEngineList::load(const TQString& file)
	{
		m_search_engines.clear();
		
		TQFile fptr(file);
		
		if(!fptr.exists())
			makeDefaultFile(file);
		
		if (!fptr.open(IO_ReadOnly))
			return;
		
		TQTextStream in(&fptr);
		
		int id = 0;
		
		while (!in.atEnd())
		{
			TQString line = in.readLine();
		
			if(line.startsWith("#") || line.startsWith(" ") || line.isEmpty() ) continue;
		
			TQStringList tokens = TQStringList::split(" ", line);
		
			SearchEngine se;
			se.name = tokens[0];
			se.name = se.name.replace("%20"," ");
			se.url = KURL::fromPathOrURL(tokens[1]);
		
			for(Uint32 i=2; i<tokens.count(); ++i)
				se.url.addQueryItem(tokens[i].section("=",0,0), tokens[i].section("=", 1, 1));
		
			m_search_engines.append(se);
		}
	}
	
	void SearchEngineList::makeDefaultFile(const TQString& file)
	{
		TQFile fptr(file);
		if (!fptr.open(IO_WriteOnly))
			return;
		
		TQTextStream out(&fptr);
		out << "# PLEASE DO NOT MODIFY THIS FILE. Use KTorrent configuration dialog for adding new search engines." << ::endl;
		out << "# SEARCH ENGINES list" << ::endl;
		out << "isohunt.to http://isohunt.to/torrents/?ihq=FOOBAR"               << ::endl; 
		out << "mininova.org http://www.mininova.org/search.php?search=FOOBAR"   << ::endl; 
		out << "thepiratebay.se http://thepiratebay.se/search.php?q=FOOBAR"      << ::endl; 
		out << "kickass.to http://kickass.to/usearch/FOOBAR"                        << ::endl;
		out << "torrentfunk.com http://www.torrentfunk.com/all/torrents/FOOBAR.html" << ::endl;
		out << "yourbittorrent.com http://yourbittorrent.com/?q=FOOBAR"              << ::endl;
		out << "torlock.com http://www.torlock.com/all/torrents/FOOBAR.html"         << ::endl;
		out << "torrentz.eu http://torrentz.eu/search?f=FOOBAR"                     << ::endl;
		out << "torrentcrazy.com http://torrentcrazy.com/s/FOOBAR"                   << ::endl;
		out << "bitsnoop.com http://bitsnoop.com/search/all/FOOBAR/c/d/1/"           << ::endl;
		out << "torrents.net http://www.torrents.net/find/FOOBAR/"                   << ::endl;
		out << "btmon.com http://www.btmon.com/torrent/?f=FOOBAR"                    << ::endl;
	}
		
	KURL SearchEngineList::getSearchURL(bt::Uint32 engine) const
	{
		if (engine >= m_search_engines.count())
			return KURL();
		else
			return m_search_engines[engine].url;
	}
	
	TQString SearchEngineList::getEngineName(bt::Uint32 engine) const
	{
		if (engine >= m_search_engines.count())
			return TQString();
		else
			return m_search_engines[engine].name;
	}

}

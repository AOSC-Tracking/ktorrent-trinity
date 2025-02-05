/***************************************************************************
 *   Copyright (C) 2006 by Alan Jones                                      *
 *   skyphyr@gmail.com                                                     *
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
#ifndef KTRSSFEEDPLUGIN_H
#define KTRSSFEEDPLUGIN_H

#include <interfaces/plugin.h>

class TQString;


namespace kt
{	
	class RssFeedManager;
	
	/**
	 * @author Alan Jones <skyphyr@gmail.com>
	 * @brief KTorrent RssFeed plugin
	 * Automatically scans rssfeeds for torrent matching regular expressions and loads them.
	 */
	class RssFeedPlugin : public Plugin
	{
		TQ_OBJECT
  
	public:
		RssFeedPlugin(TQObject* parent, const char* name, const TQStringList& args);
		virtual ~RssFeedPlugin();

		virtual void load();
		virtual void unload();
		virtual bool versionCheck(const TQString& version) const;
		
		private:
			RssFeedManager * m_rssFeedManager;
			
	};

}

#endif

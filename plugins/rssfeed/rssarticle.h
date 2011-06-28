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
#ifndef RSSARTICLE_H
#define RSSARTICLE_H

#include <tqstring.h>
#include <tqdatetime.h>
#include <tqdatastream.h>

#include <kurl.h>

#include "rss/article.h"

namespace kt
{
	/**
	 * @brief RssFeed Manager Class
	 * @author Alan Jones <skyphyr@gmail.com>
	 * 
	 * 
	*/
	
	class RssArticle
	{
		public:
			
			typedef TQValueList<RssArticle> List;
			 
			/**
			 * Default constructor.
			 */
			RssArticle();
			RssArticle(RSS::Article article);
			RssArticle(const RssArticle &other);
 			RssArticle &operator=(const RssArticle &other);
 			bool operator==(const RssArticle &other) const;
			RssArticle(TQString title, KURL link, TQString description, TQDateTime pubDate, TQString guid, int downloaded = 0);
			
			void setTitle(const TQString& title) { m_title=title; }
			void setDownloaded(const int downloaded) { m_downloaded=downloaded; }
			
			TQString title() const { return m_title; }
			KURL link() const { return m_link; }
			TQString description() const { return m_description; }
			TQDateTime pubDate() const { return m_pubDate; }
			TQString guid() const { return m_guid; }
			int downloaded() const { return m_downloaded; }
			
			~RssArticle();

		private:
			KURL m_link;
			TQString m_title;
			TQString m_description;
			TQDateTime m_pubDate;
			TQString m_guid;
			int m_downloaded;
	};
	
	TQDataStream &operator<<( TQDataStream &out, const RssArticle &article );
	TQDataStream &operator>>( TQDataStream &in, RssArticle &article );

}

#endif

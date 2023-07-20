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
#ifndef RSSFEED_H
#define RSSFEED_H

#include <tqobject.h>
#include <tqstring.h>
#include <tqdatetime.h>
#include <tqvaluelist.h>
#include <tqtimer.h>
#include <tqdatastream.h>

#include <kurl.h>

#include "rss/loader.h"
#include "rss/document.h"

#include "rssarticle.h"

using namespace RSS;

namespace kt
{
	/**
	 * @brief RssFeed Class
	 * @author Alan Jones <skyphyr@gmail.com>
	 * 
	 * 
	*/
	
	class RssFeed : public TQObject
	{
			TQ_OBJECT
  
		public:
			
			/**
			 * Default constructor.
			 */
			RssFeed(TQObject * parent = 0);
			RssFeed(KURL feedUrl, TQString title = "", bool active = false, int articleAge = 3, bool ignoreTTL = false, TQTime autoRefresh = TQTime());
			RssFeed(const RssFeed &other);
 			RssFeed &operator=(const RssFeed &other);
 			~RssFeed();
 			
 			KURL feedUrl() const { return m_feedUrl; }
 			bool active() const { return m_active; }
 			int articleAge() const { return m_articleAge; }
 			TQString title() const { return m_title; }
 			TQTime autoRefresh() const { return m_autoRefresh; }
 			bool ignoreTTL() const { return m_ignoreTTL; }
 			
 			
 			RssArticle::List articles() const { return m_articles; }
 			

		public slots:
			void refreshFeed();
			void feedLoaded(Loader *feedLoader, Document doc, Status status);
			
			void clearArticles();
			
			void setFeedUrl( const KURL& url );
			void setFeedUrl( const TQString& url );
			void setActive( bool active );
			void setArticleAge( int articleAge );
			void setTitle( const TQString& title );
			void setAutoRefresh( const TQTime& autoRefresh );
			void setIgnoreTTL( bool ignoreTTL );
			void saveArticles();
			
			void setDownloaded(TQString link, int downloaded);
			
		signals:
			void feedUrlChanged( const KURL& url );
			void activeChanged( bool active );
			void articleAgeChanged( int articleAge );
			void titleChanged( const TQString& title );
			void updateTitle( const TQString& title );
			void autoRefreshChanged( const TQTime& autoRefresh );
			void ignoreTTLChanged( bool ignoreTTL );
			
			void articlesChanged( const RssArticle::List& articles );
			
			void scanRssArticle( RssArticle article );

		private:
			KURL m_feedUrl;
			bool m_active;
			int m_articleAge;
			TQString m_title;
			TQTime m_autoRefresh;
			bool m_ignoreTTL;
			RssArticle::List m_articles;
			TQTimer refreshTimer;
			
			bool feedLoading;
			
			TQString getFilename();
			void initialize();
			void startFeed();
			void cleanArticles();
			void loadArticles();
	};

	TQDataStream &operator<<( TQDataStream &out, const RssFeed &feed );
	TQDataStream &operator>>( TQDataStream &in, RssFeed &feed );

}

#endif

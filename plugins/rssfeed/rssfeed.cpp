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
#include "rssfeed.h"

#include <kglobal.h>
#include <kstandarddirs.h>
#include <krfcdate.h>
#include <tdeio/netaccess.h>
#include <tqfile.h>
#include <tqapplication.h>
#include <tqdir.h>

namespace kt
{

	void RssFeed::startFeed()
	{
		if (m_active)
			{
			refreshFeed();
			refreshTimer.start(TQTime().msecsTo(m_autoRefresh));
			}
		else
			{
			refreshTimer.stop();
			}
	}
	
	void RssFeed::initialize()
	{
		feedLoading = false;
		loadArticles();
		
		connect(&refreshTimer, TQT_SIGNAL(timeout()), this, TQT_SLOT( refreshFeed() ) );
		connect(this, TQT_SIGNAL(articlesChanged(const RssArticle::List&)), this, TQT_SLOT( saveArticles() ) );
		
		startFeed();
	}
	
	RssFeed::RssFeed(TQObject * parent) : TQObject(parent)
	{
		m_active = false;
		m_articleAge = 365;
		m_ignoreTTL = false;
		m_title = "New";
		
		initialize();
	}
	
	RssFeed::RssFeed(KURL feedUrl, TQString title, bool active, int articleAge, bool ignoreTTL, TQTime autoRefresh )
	{
		m_feedUrl = feedUrl;
		m_title = title;
		m_active = active;
		m_articleAge = articleAge;
		m_ignoreTTL = ignoreTTL;
		m_autoRefresh = autoRefresh;
		
		initialize();
	}
	
	RssFeed::RssFeed(const RssFeed &other) : TQObject()
	{
		*this = other;
	}
	
	RssFeed &RssFeed::operator=(const RssFeed &other)
	{
		if (&other != this)
			{
			m_feedUrl = other.feedUrl();
			m_title = other.title();
			m_active = other.active();
			m_articleAge = other.articleAge();
			m_ignoreTTL = other.ignoreTTL();
			m_autoRefresh = other.autoRefresh();
		}
		
		initialize();
		
		return *this;
	}
	
	void RssFeed::setFeedUrl( const KURL& url )
	{
		if (m_feedUrl != url)
		{
			m_feedUrl = url;
			loadArticles();
			startFeed();
			emit feedUrlChanged(url);
		}
	}
	
	void RssFeed::setFeedUrl( const TQString& url )
	{
		if (m_feedUrl != url)
		{
			m_feedUrl = url;
			loadArticles();
			startFeed();
			emit feedUrlChanged(url);
		}
	}
	
	void RssFeed::setActive( bool active )
	{
		if (m_active != active)
		{
			m_active = active;
			
			startFeed();
			
			emit activeChanged(active);
		}
	}
	
	void RssFeed::setArticleAge( int articleAge )
	{
		if (m_articleAge != articleAge)
		{
			if (articleAge < m_articleAge)
			{
				cleanArticles();
			}
			
			m_articleAge = articleAge;
			emit articleAgeChanged(articleAge);
		}
	}
	
	void RssFeed::setTitle( const TQString& title )
	{
		if (m_title != title)
		{
			m_title = title;
			emit titleChanged(title);
		}
	}
	
	void RssFeed::setAutoRefresh( const TQTime& autoRefresh )
	{
		if (m_autoRefresh != autoRefresh)
		{
			m_autoRefresh = autoRefresh;
			if (m_active)
			{
				refreshTimer.changeInterval(TQTime().msecsTo(m_autoRefresh));
			}
			
			emit autoRefreshChanged(autoRefresh);
		}
	}
	
	void RssFeed::setIgnoreTTL( bool ignoreTTL )
	{
		if (m_ignoreTTL != ignoreTTL)
		{
			m_ignoreTTL = ignoreTTL;
			emit ignoreTTLChanged(ignoreTTL);
		}
	}
	
	TQString RssFeed::getFilename()
	{
		TQDir directory;
		directory.mkdir(TDEGlobal::dirs()->saveLocation("data","ktorrent") + "rssfeeds");
		return TDEGlobal::dirs()->saveLocation("data","ktorrent") + "rssfeeds/" + m_feedUrl.prettyURL(-1).replace("/", "_").replace(":", "_") + ".ktr";
		
	}
	
	void RssFeed::loadArticles()
	{
		TQString filename = getFilename();
		
		//load articles from disk
		TQFile file(filename);
		
		if (file.exists())
			{
			try
				{
				file.open( IO_ReadOnly );
			TQDataStream in(&file);
			
				in >> m_articles;
				emit articlesChanged( m_articles );
				}
			catch (...)
				{
				m_articles.clear();
				}
			}
	}
	
	void RssFeed::saveArticles()
	{
		TQString filename = getFilename();
		
		//load articles from disk
		TQFile file(filename);
		
		file.open( IO_WriteOnly );
		TQDataStream out(&file);
		
		out << m_articles;
	}
	
	void RssFeed::cleanArticles()
	{
		bool removed = false;
		
		RssArticle::List::iterator it;
		for ( it = m_articles.begin(); it != m_articles.end();  )
			{
			if ((*it).pubDate().daysTo(TQDateTime::currentDateTime()) > m_articleAge)
				{
				it = m_articles.erase(it);
				removed = true;
				}
			else
				{
					it++;
				}
			}
		
		if (removed)
		{
			emit articlesChanged(m_articles);
		}
	
	}
	
	void RssFeed::clearArticles()
	{
		m_articles.clear();
	}
	
	void RssFeed::refreshFeed()
	{
		if (feedLoading)
			return;
		
		feedLoading = true;
		cleanArticles();
		Loader * feedLoader = Loader::create();
		connect( feedLoader, TQT_SIGNAL( loadingComplete( Loader *, Document, Status ) ),
			this, TQT_SLOT( feedLoaded( Loader *, Document, Status ) ) );
		feedLoader->loadFrom( m_feedUrl, new FileRetriever );
	}
	
	void RssFeed::feedLoaded(Loader *feedLoader, Document doc, Status status)
	{
		feedLoading = false;

		if ( status == Success )
		{
			bool added = false;
			
			if (m_title.isEmpty() || m_title == TQString("New"))
			{
				setTitle(doc.title());
				emit updateTitle(doc.title());
			}
			
			if (!m_ignoreTTL)
			{
				if (doc.ttl() < 0)
				{
					setAutoRefresh(TQTime().addSecs(3600));
				}
				else
				{
					setAutoRefresh(TQTime().addSecs(doc.ttl() * 60));
				}
			}
	
			RssArticle curArticle;
			
			for (int i=doc.articles().count()-1; i>=0; i--)
			{
				curArticle = doc.articles()[i];
				if (curArticle.pubDate().daysTo(TQDateTime::currentDateTime()) < m_articleAge && !m_articles.contains(curArticle))
				{
					m_articles.prepend(curArticle);
					emit scanRssArticle(curArticle);
					added = true;
				}
			}
			
			if (added)
			{
				emit articlesChanged(m_articles);
			}
		} else {
			tqDebug( "There was and error loading the feed\n");
		}
		
		disconnect( feedLoader, TQT_SIGNAL( loadingComplete( Loader *, Document, Status ) ),
			this, TQT_SLOT( feedLoaded( Loader *, Document, Status ) ) );
		feedLoader->deleteLater();

	}
	
	void RssFeed::setDownloaded(TQString link, int downloaded)
	{
		bool changed = false;
		
		RssArticle::List::iterator it;
		for ( it = m_articles.begin(); it != m_articles.end(); it++ )
			{
			if ((*it).link().prettyURL() == link)
				{
				(*it).setDownloaded( downloaded );
				changed = true;
				}
			}
		
		if (changed)
		{
			emit articlesChanged(m_articles);
		}	
	}
	
	TQDataStream &operator<<( TQDataStream &out, const RssFeed &feed )
	{
		out << feed.feedUrl() << feed.title() << int(feed.active()) << feed.articleAge() << int(feed.ignoreTTL()) << feed.autoRefresh();
		
		return out;
	}
	
	TQDataStream &operator>>( TQDataStream &in, RssFeed &feed )
	{
		KURL feedUrl;
		TQString title;
		int active;
		int articleAge;
		int ignoreTTL;
		TQTime autoRefresh;
		in >> feedUrl >> title >> active >> articleAge >> ignoreTTL >> autoRefresh;
		feed = RssFeed(feedUrl, title, active, articleAge, ignoreTTL, autoRefresh);
		
		return in;
	}
	
	RssFeed::~RssFeed()
	{
	}
}

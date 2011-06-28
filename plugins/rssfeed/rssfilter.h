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
#ifndef RSSFILTER_H
#define RSSFILTER_H

#include <tqobject.h>
#include <tqstring.h>
#include <tqtimer.h>
#include <tqdatastream.h>
#include <tqregexp.h>

#include "rssarticle.h"

using namespace RSS;

namespace kt
{
	/**
	 * @brief RssFilter Class
	 * @author Alan Jones <skyphyr@gmail.com>
	 * 
	 * 
	*/
	class FilterMatch
	{
		public:
		
			FilterMatch() { m_season = 0; m_episode = 0; m_time = TQDateTime::tqcurrentDateTime().toString(); m_link=TQString(); };
			FilterMatch(int season, int episode, TQString link, TQString time = TQDateTime::tqcurrentDateTime().toString());
			FilterMatch(const FilterMatch &other);
			FilterMatch &operator=(const FilterMatch &other);
			bool operator==(const FilterMatch &other) const;
			~FilterMatch() {};
			
			TQString link() const { return m_link; }
			int season() const { return m_season; }
			int episode() const { return m_episode; }
			TQString time() const { return m_time; }
			
			void setLink(const TQString& link) { m_link = link; }
			void setSeason(int season) { m_season = season; }
			void setEpisode(int episode) { m_episode = episode; }
			void setTime(TQString time) { m_time = time; }
			
		private:
			int m_season;
			int m_episode;
			TQString m_link;
			TQString m_time;
	};
	
	class RssFilter : public TQObject
	{
			Q_OBJECT
  TQ_OBJECT
		public:
			
			/**
			 * Default constructor.
			 */
			RssFilter(TQObject * tqparent = 0);
			RssFilter(const RssFilter &other);
			RssFilter(TQString title, bool active, TQStringList regexps, bool series, bool sansEpisode, 
					int minSeason, int minEpisode, int maxSeason, int maxEpisode, 
					TQValueList<FilterMatch> matches);
			RssFilter &operator=(const RssFilter &other);
			~RssFilter();
			
			TQString title() const { return m_title; }
			bool active() const { return m_active; }
			TQStringList regExps() const { return m_regExps; }
			bool series() const { return m_series; }
			bool sansEpisode() const { return m_sansEpisode; }
			int minSeason() const { return m_minSeason; }
			int minEpisode() const { return m_minEpisode; }
			int maxSeason() const { return m_maxSeason; }
			int maxEpisode() const { return m_maxEpisode; }
			TQValueList<FilterMatch> matches() const { return m_matches; }
			
			bool scanArticle(RssArticle article, bool ignoreMatches = true, bool saveMatch = true);
			void deleteMatch(const TQString& link);

		public slots:
			void setTitle( const TQString& title );
			void setActive( bool active );
			void setRegExps ( const TQStringList& regexps );
			void setSeries ( bool series );
			void setSansEpisode ( bool sansEpisode );
			void setMinSeason( int minSeason );
			void setMinEpisode( int minEpisode );
			void setMaxSeason( int maxSeason );
			void setMaxEpisode( int maxEpisode );
			void setMatches( const TQValueList<FilterMatch>& matches );
			
			//void scanFilter();
			
		signals:
			void titleChanged( const TQString& title );
			void activeChanged( bool active );
			void regExpsChanged( const TQStringList& regexps );
			void seriesChanged( bool series );
			void sansEpisodeChanged( bool sansEpisode );
			void minSeasonChanged (int minSeason);
			void minEpisodeChanged (int minEpisode);
			void maxSeasonChanged (int maxSeason);
			void maxEpisodeChanged (int maxEpisode);
			void matchesChanged( const TQValueList<FilterMatch>& matches );
			
			void rescanFilter();

		private:
			TQString m_title;
			bool m_active;
			TQStringList m_regExps;
			bool m_series;
			bool m_sansEpisode;
			int m_minSeason;
			int m_minEpisode;
			int m_maxSeason;
			int m_maxEpisode;
			TQValueList<FilterMatch> m_matches;
			
			bool episodeInRange(int season, int episode, bool ignoreMatches, bool& alreadyDownloaded);
			
	};

	TQDataStream &operator<<( TQDataStream &out, const FilterMatch &filterMatch );
	TQDataStream &operator>>( TQDataStream &in, FilterMatch &filterMatch );
	
	TQDataStream &operator<<( TQDataStream &out, const RssFilter &filter );
	TQDataStream &operator>>( TQDataStream &in, RssFilter &filter );

}

#endif

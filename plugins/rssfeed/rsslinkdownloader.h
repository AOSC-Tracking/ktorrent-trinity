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
#ifndef RSSLINKDOWNLOADER_H
#define RSSLINKDOWNLOADER_H

#include <tdeio/job.h>
#include <tdetempfile.h>

#include <interfaces/coreinterface.h>
#include <torrent/globals.h>
#include <util/log.h>
#include <util/constants.h>

#include <tqstring.h>

#include "rssfilter.h"
#include "rssarticle.h"

using namespace RSS;

namespace kt
{
	/**
	 * @brief RssLinkDownloader Class
	 * @author Alan Jones <skyphyr@gmail.com>
	 * 
	 * 
	*/
	
	class RssLinkDownloader : public TQObject
	{
			Q_OBJECT
  
		public:
			
			/**
			 * Default constructor.
			 */
			RssLinkDownloader(CoreInterface* core, TQString link, RssFilter * filter = 0, TQObject * parent = 0);
			
 			~RssLinkDownloader();
 			

		public slots:
			void processLink(TDEIO::Job* jobStatus);
			void suicide();
			
		signals:
			void linkDownloaded( TQString link, int downloaded );
			
		private:
			TDEIO::StoredTransferJob * curFile;
			TQString curLink, curSubLink;
			TQStringList subLinks;
			RssFilter * curFilter;
			bool firstLink;
			
			//KTempFile tempFile;
			
			CoreInterface* m_core;
	};


}

#endif

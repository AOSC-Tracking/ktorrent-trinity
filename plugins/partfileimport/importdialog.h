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

#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <util/constants.h>
#include <datachecker/datacheckerlistener.h>
#include "importdlgbase.h"

class KURL;

namespace bt
{
	class BitSet;
	class Torrent;
}

namespace TDEIO
{
	class Job;
}


namespace kt
{
	class CoreInterface;
	
	class ImportDialog : public ImportDlgBase,public bt::DataCheckerListener
	{
		TQ_OBJECT
  
	
	public:
		ImportDialog(CoreInterface* core,TQWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
		virtual ~ImportDialog();
		
	public slots:
		void onImport();
		void onTorrentGetReult(TDEIO::Job* j);
	
	private:
		void writeIndex(const TQString & file,const bt::BitSet & chunks);
		void linkTorFile(const TQString & cache_dir,const TQString & dnd_dir,
						 const KURL & data_url,const TQString & fpath,bool & dnd);
		void saveStats(const TQString & stats_file,const KURL & data_url,bt::Uint64 imported,bool custom_output_name);
		bt::Uint64 calcImportedBytes(const bt::BitSet & chunks,const bt::Torrent & tor);
		void saveFileInfo(const TQString & file_info_file,TQValueList<bt::Uint32> & dnd);
		
		virtual void progress(bt::Uint32 num,bt::Uint32 total);
		virtual void status(bt::Uint32 num_failed,bt::Uint32 num_downloaded);
		virtual void finished();
		
		void import(bt::Torrent & tor);
		
	private:
		CoreInterface* core;
	};
}

#endif


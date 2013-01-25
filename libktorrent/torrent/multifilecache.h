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
#ifndef BTMULTIFILECACHE_H
#define BTMULTIFILECACHE_H


#include <util/ptrmap.h>
#include "cache.h"
#include "settings.h"

namespace bt
{
	class DNDFile;
	class CacheFile;

	/**
	 * @author Joris Guisson
	 * @brief Cache for multi file torrents
	 *
	 * This class manages a multi file torrent cache. Everything gets stored in the
	 * correct files immediately. 
	 */
	class MultiFileCache : public Cache
	{
		TQString cache_dir,output_dir;
		PtrMap<Uint32,CacheFile> files;
		PtrMap<Uint32,DNDFile> dnd_files;
	public:
		MultiFileCache(Torrent& tor,const TQString & tmpdir,const TQString & datadir,bool custom_output_name);
		virtual ~MultiFileCache();

		virtual void changeTmpDir(const TQString& ndir);
		virtual void create();
		virtual void load(Chunk* c);
		virtual void save(Chunk* c);
		virtual bool prep(Chunk* c);
		virtual void close();
		virtual void open();
		virtual TQString getOutputPath() const;
		virtual void changeOutputPath(const TQString & outputpath);
		virtual TDEIO::Job* moveDataFiles(const TQString & ndir);
		virtual void moveDataFilesCompleted(TDEIO::Job* job);
		virtual void preallocateDiskSpace(PreallocationThread* prealloc);
		virtual bool hasMissingFiles(TQStringList & sl);
		virtual void deleteDataFiles();
		virtual Uint64 diskUsage();
	private:
		void touch(TorrentFile & tf);
		virtual void downloadStatusChanged(TorrentFile*, bool);
		TQString guessDataDir();
		void saveFirstAndLastChunk(TorrentFile* tf,const TQString & src_file,const TQString & dst_file);
		void recreateFile(TorrentFile* tf,const TQString & dnd_file,const TQString & output_file);
	};

}

#endif

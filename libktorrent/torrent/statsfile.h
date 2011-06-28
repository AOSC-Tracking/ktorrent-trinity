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
#ifndef BTSTATSFILE_H
#define BTSTATSFILE_H

#include <tqstring.h>
#include <tqfile.h>
#include <tqmap.h>

#include <util/constants.h>

namespace bt
{

	/**
	 * @brief This class is used for loading/storing torrent stats in a file.
	 * @author Ivan Vasic <ivasic@gmail.com>
	*/
	class StatsFile
	{
		public:
			/**
			 * @brief A constructor.
			 * Constructs StatsFile object and calls readSync().
			 */
			StatsFile(TQString filename);
			~StatsFile();
			
			///Closes TQFile
			void close();
			
			/**
			 * @brief Main read function.
			 * @return TQString value that correspodents to key.
			 * @param key - TQString stats key.
			 */
			TQString readString(TQString key);
			
			Uint64 readUint64(TQString key);
			bool readBoolean(TQString key);
			int readInt(TQString key);
			unsigned long readULong(TQString key);
			float readFloat(TQString key);
		
			/**
			 * @brief Writes key and value.
			 * It only inserts pair of key/value to the m_values. To make changes to file call writeSync().
			 * @param key - TQString key
			 * @param value - TQString value.
			 */
			void write(TQString key, TQString value);
			
			///Reads data from stats file to m_values.
			void readSync();
			
			///Writes data from m_values to stats file.
			void writeSync();
			
			/**
			 * See if there is a key in the stats file
			 * @param key The key
			 * @return true if key is in the stats file
			 */
			bool hasKey(const TQString & key) const {return m_values.tqcontains(key);}
			
		private:
			TQString m_filename;
			TQFile m_file;
			
			TQMap<TQString, TQString> m_values;
	};
}

#endif

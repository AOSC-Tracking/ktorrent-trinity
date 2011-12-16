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
#include "statsfile.h"

#include "globals.h"
#include <util/log.h>
#include <util/functions.h>

#include <tqstring.h>
#include <tqfile.h>
#include <tqtextstream.h>

namespace bt
{

	StatsFile::StatsFile(TQString filename)
	:m_filename(filename)
	{
		m_file.setName(filename);
		readSync();
	}

	StatsFile::~StatsFile()
	{
		close();
	}

	void StatsFile::close()
	{
		m_file.close();
	}
	
	void StatsFile::write(TQString key, TQString value)
	{
		m_values.insert(key.stripWhiteSpace(), value.stripWhiteSpace());
	}
	
	TQString StatsFile::readString(TQString key)
	{
		return m_values[key].stripWhiteSpace();
	}
	
	Uint64 StatsFile::readUint64(TQString key)
	{
		bool ok = true;
		Uint64 val = readString(key).toULongLong(&ok);
		return val;
	}
	
	int StatsFile::readInt(TQString key)
	{
		bool ok = true;
		int val = readString(key).toInt(&ok);
		return val;
	}
	
	bool StatsFile::readBoolean(TQString key)
	{
		return (bool) readInt(key);
	}
	
	unsigned long StatsFile::readULong(TQString key)
	{
		bool ok = true;
		return readString(key).toULong(&ok);
	}
	
	float bt::StatsFile::readFloat( TQString key )
	{
		bool ok = true;
		return readString(key).toFloat(&ok);
	}
	
	void StatsFile::readSync()
	{
		if (!m_file.open(IO_ReadOnly))
			return;
		
		TQTextStream in(&m_file);
		while (!in.atEnd())
		{
			TQString line = in.readLine();
			TQString tmp = line.left(line.find('='));
			m_values.insert(tmp, line.mid(tmp.length()+1));
		}
		close();
	}
	
	void StatsFile::writeSync()
	{
		if (!m_file.open(IO_WriteOnly))
			return;
		TQTextStream out(&m_file);
		TQMap<TQString, TQString>::iterator it = m_values.begin();
		while(it!=m_values.end())
		{
			out << it.key() << "=" << it.data() << ::endl;
			++it;
		}
		close();
	}
	
}

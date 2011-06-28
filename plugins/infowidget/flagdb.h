/***************************************************************************
 *   Copyright (C) 2007 by Modestas Vainius <modestas@vainius.eu>          *
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
#ifndef FLAGDB_H
#define FLAGDB_H

#include <tqpixmap.h>
#include <tqstring.h>
#include <tqstringlist.h>
#include <tqmap.h>

namespace kt
{
	class FlagDBSource
	{
	public:
		FlagDBSource();
		FlagDBSource(const char* type, const TQString& pathPattern);
		FlagDBSource(const TQString& pathPattern);
		TQString getPath(const TQString& country) const;

		const char* getType() { return type; };
		const TQString& getPathPattern() { return pathPattern; };

	private:
		const char* type;
		const TQString pathPattern;
	};

	/**
	@author Modestas Vainius
	*/
	class FlagDB
	{
	public :
		FlagDB(int preferredWidth, int preferredHeight);
		FlagDB(const FlagDB& m);
		~FlagDB();

		void addFlagSource(const FlagDBSource& source);
		void addFlagSource(const char* type, const TQString& pathPattern);
		const TQValueList<FlagDBSource>& listSources() const;
		bool isFlagAvailable(const TQString& country);
		const TQPixmap& getFlag(const TQString& country);
	private:
		static const TQPixmap& nullPixmap;
		int preferredWidth, preferredHeight;
		TQValueList<FlagDBSource> sources;
		TQMap<TQString,TQPixmap> db;
	};
}

 #endif

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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
 ***************************************************************************/

#include <tqfile.h>
#include <tqimage.h>
#include <kstandarddirs.h>
#include "flagdb.h"

kt::FlagDBSource::FlagDBSource(const char* type, const TQString& pathPattern)
	: type(type), pathPattern(pathPattern) 
{
}

kt::FlagDBSource::FlagDBSource(const TQString& pathPattern)
	: type(NULL), pathPattern(pathPattern) 
{
}

kt::FlagDBSource::FlagDBSource()
	: type(NULL), pathPattern()
{
}

TQString kt::FlagDBSource::FlagDBSource::getPath(const TQString& country) const
{
	if (type) {
		return locate(type, pathPattern.arg(country));
	} else {
		return pathPattern.arg(country);
	}
}

const TQPixmap& kt::FlagDB::nullPixmap = TQPixmap();

kt::FlagDB::FlagDB(int preferredWidth, int preferredHeight)
	: preferredWidth(preferredWidth),
	  preferredHeight(preferredHeight),
	  sources(),
	  db()
{
}


kt::FlagDB::FlagDB(const FlagDB& other)
	: preferredWidth(other.preferredWidth),
	  preferredHeight(other.preferredHeight),
	  sources(other.sources),
	  db(other.db)
{
}

kt::FlagDB::~FlagDB()
{
}

void kt::FlagDB::addFlagSource(const FlagDBSource& source)
{
	sources.append(source);
}

void kt::FlagDB::addFlagSource(const char* type, const TQString& pathPattern)
{
	addFlagSource(FlagDBSource(type, pathPattern));
}

const TQValueList<kt::FlagDBSource>& kt::FlagDB::listSources() const
{
	return sources;
}

bool kt::FlagDB::isFlagAvailable(const TQString& country)
{
	return getFlag(country).isNull();
}

const TQPixmap& kt::FlagDB::getFlag(const TQString& country)
{
	const TQString& c = country.lower();
	if (!db.contains(c)) {
		TQImage img;
		TQPixmap pixmap;
		for (TQValueList<FlagDBSource>::const_iterator it = sources.constBegin(); it != sources.constEnd(); it++) {
			const TQString& path = (*it).getPath(c);
			if (TQFile::exists(path)) {
				if (img.load(path)) {
					if (img.width() != preferredWidth || img.height() != preferredHeight) {
						const TQImage& imgScaled = img.smoothScale(preferredWidth, preferredHeight, TQImage::ScaleMin);
						if (!imgScaled.isNull()) {
							pixmap.convertFromImage(imgScaled);
							break;
						} else if (img.width() <= preferredWidth || img.height() <= preferredHeight) {
							pixmap.convertFromImage(img);
							break;
						}
					}
				}
			}
		}

		db[c] = (!pixmap.isNull()) ? pixmap : nullPixmap;
	}
	return db[c];
}

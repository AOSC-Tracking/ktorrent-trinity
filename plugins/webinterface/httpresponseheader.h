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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
 ***************************************************************************/
#ifndef KTHTTPRESPONSEHEADER_H
#define KTHTTPRESPONSEHEADER_H
		

#include <tqmap.h>
#include <tqstring.h>

namespace kt
{

	/**
		@author Joris Guisson <joris.guisson@gmail.com>
	*/
	class HttpResponseHeader
	{
		int response_code;
		TQMap<TQString,TQString> fields;
	public:
		HttpResponseHeader(int response_code);
		HttpResponseHeader(const HttpResponseHeader & hdr);
		virtual ~HttpResponseHeader();
		
		void setResponseCode(int response_code);
		void setValue(const TQString & key,const TQString & value);
		
		TQString toString() const;
	};


}

#endif

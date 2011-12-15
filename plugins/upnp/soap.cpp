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
#include "soap.h"

namespace kt 
{

	TQString SOAP::createCommand(const TQString & action,const TQString & service)
	{
		TQString comm = TQString("<?xml version=\"1.0\"?>\r\n"
				"<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
				"<SOAP-ENV:Body>"
				"<m:%1 xmlns:m=\"%2\"/>"
				"</SOAP-ENV:Body></SOAP-ENV:Envelope>"
				"\r\n").arg(action).arg(service);
		
		return comm;
	}
	
	TQString SOAP::createCommand(const TQString & action,const TQString & service,const TQValueList<Arg> & args)
	{
		TQString comm = TQString("<?xml version=\"1.0\"?>\r\n"
				"<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
				"<SOAP-ENV:Body>"
				"<m:%1 xmlns:m=\"%2\">").arg(action).arg(service);
		
		for (TQValueList<Arg>::const_iterator i = args.begin();i != args.end();i++)
		{
			const Arg & a = *i;
			comm += "<" + a.element + ">" + a.value + "</" + a.element + ">";
		}
		
		comm += TQString("</m:%1></SOAP-ENV:Body></SOAP-ENV:Envelope>\r\n").arg(action);
		return comm;
	}
}

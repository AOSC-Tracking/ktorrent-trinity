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
#include <tqstringlist.h>
#include <torrent/globals.h>
#include "httprequest.h"
#include "array.h"
#include "log.h"


namespace bt 
{

	HTTPRequest::HTTPRequest(const TQString & hdr,const TQString & payload,const TQString & host,Uint16 port,bool verbose) : hdr(hdr),payload(payload),verbose(verbose)
	{
		sock = new KNetwork::KStreamSocket(host,TQString::number(port),this,0);
		sock->enableRead(true);
		sock->enableWrite(true);
		sock->setTimeout(30000);
		sock->setBlocking(false);
		connect(sock,TQT_SIGNAL(readyRead()),this,TQT_SLOT(onReadyRead()));
		connect(sock,TQT_SIGNAL(gotError(int)),this,TQT_SLOT(onError(int )));
		connect(sock,TQT_SIGNAL(timedOut()),this,TQT_SLOT(onTimeout()));
		connect(sock,TQT_SIGNAL(connected(const KResolverEntry&)),
				this, TQT_SLOT(onConnect( const KResolverEntry& )));
	}
	
	
	HTTPRequest::~HTTPRequest()
	{
		sock->close();
		delete sock;
	}
	
	void HTTPRequest::start()
	{
		sock->connect();
	}
	
	void HTTPRequest::onConnect(const KResolverEntry&)
	{
		payload = payload.tqreplace("$LOCAL_IP",sock->localAddress().nodeName());
		hdr = hdr.tqreplace("$CONTENT_LENGTH",TQString::number(payload.length()));
			
		TQString req = hdr + payload;
		if (verbose)
		{
			Out(SYS_PNP|LOG_DEBUG) << "Sending " << endl;
			Out(SYS_PNP|LOG_DEBUG) << hdr << payload << endl;
		}
		sock->writeBlock(req.ascii(),req.length());
	}
	
	void HTTPRequest::onReadyRead()
	{
		Uint32 ba = sock->bytesAvailable();
		if (ba == 0)
		{
			error(this,false);
			sock->close();
			return;
		}
			
		Array<char> data(ba);
		ba = sock->readBlock(data,ba);
		TQString strdata((const char*)data);
		TQStringList sl = TQStringList::split("\r\n",strdata,false);	
		
		if (verbose)
		{
			Out(SYS_PNP|LOG_DEBUG) << "Got reply : " << endl;
			Out(SYS_PNP|LOG_DEBUG) << strdata << endl;
		}
		
		if (sl.first().tqcontains("HTTP") && sl.first().tqcontains("200"))
		{
			// emit reply OK
			replyOK(this,sl.last());
		}
		else
		{
			// emit reply error
			replyError(this,sl.last());
		}
		operationFinished(this);
	}
	
	void HTTPRequest::onError(int)
	{
		Out() << "HTTPRequest error : " << sock->KSocketBase::errorString() << endl;
		error(this,false);
		sock->close();
		operationFinished(this);
	}
	
	void HTTPRequest::onTimeout()
	{
		Out() << "HTTPRequest timeout" << endl;
		error(this,true);
		sock->close();
		operationFinished(this);
	}


}
#include "httprequest.moc"

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
#ifndef KTHTTPCLIENTHANDLER_H
#define KTHTTPCLIENTHANDLER_H
		

#include <tqhttp.h>
#include <tqobject.h>
#include <util/constants.h>
#include "httpresponseheader.h"
		
class TQSocket;


namespace kt
{
	class HttpServer;
	class PhpHandler;
	class PhpInterface;

	/**
		@author Joris Guisson <joris.guisson@gmail.com>
	*/
	class HttpClientHandler : public TQObject
	{
		Q_OBJECT
  
		enum State
		{
			WAITING_FOR_REQUEST,
			WAITING_FOR_CONTENT,
			PROCESSING_PHP
		};
	public:
		HttpClientHandler(HttpServer* srv,TQSocket* sock);
		virtual ~HttpClientHandler();
		
		void readyToRead();
		bool sendFile(HttpResponseHeader & hdr,const TQString & full_path);
		void sendResponse(const HttpResponseHeader & hdr);
		void send404(HttpResponseHeader & hdr,const TQString & path);
		void send500(HttpResponseHeader & hdr);
		
		void executePHPScript(PhpInterface* php_iface,
							HttpResponseHeader & hdr,
							const TQString & php_exe,
							const TQString & php_file,
							const TQMap<TQString,TQString> & args);
		
	private:
		void handleRequest();
		
	private slots:
		void onPHPFinished();
		
	private:
		HttpServer* srv;
		TQSocket* client;
		State state;
		TQHttpRequestHeader header;
		TQString header_data;
		TQByteArray request_data;
		bt::Uint32 bytes_read;
		PhpHandler* php;
		HttpResponseHeader php_response_hdr;
	};

}

#endif

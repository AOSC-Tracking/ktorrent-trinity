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
#include <tqsocket.h>
#include <tqhttp.h> 
#include <util/log.h>
#include <util/mmapfile.h>
#include "httpserver.h"
#include "httpclienthandler.h"
#include "httpresponseheader.h"
#include "php_handler.h"
		
using namespace bt;

namespace kt
{

	HttpClientHandler::HttpClientHandler(HttpServer* srv,TQSocket* sock) : srv(srv),client(sock),php_response_hdr(200)
	{
		state = WAITING_FOR_REQUEST;
		bytes_read = 0;
		php = 0;
	}


	HttpClientHandler::~HttpClientHandler()
	{
		delete client;
		delete php;
	}

	void HttpClientHandler::readyToRead()
	{
		if (state == WAITING_FOR_REQUEST)
		{
			while (client->canReadLine())
			{
				TQString line = client->readLine();
				header_data += line;
				if (header_data.endsWith("\r\n\r\n"))
				{
					// We have got the header, so lets parse it
					handleRequest();
					break;
				}
			}
		}
		else if (state == WAITING_FOR_CONTENT)
		{
			Uint32 ba = client->bytesAvailable();
			if (ba + bytes_read < header.contentLength())
			{
				client->readBlock((char*)request_data.data() + bytes_read,ba);
				bytes_read += ba;
			}
			else
			{
				Uint32 left = header.contentLength() - bytes_read;
				client->readBlock((char*)request_data.data() + bytes_read,left);
				bytes_read += left;
				srv->handlePost(this,header,request_data);
			
				header_data = "";
				request_data.resize(0);
				state = WAITING_FOR_REQUEST;
				if (client->bytesAvailable() > 0)
					readyToRead();
			}
		}
	}
		
	void HttpClientHandler::handleRequest()
	{
		header = TQHttpRequestHeader(header_data);
	//	Out(SYS_WEB|LOG_DEBUG) << "Parsing request : " << header.toString() << endl;
		if (header.method() == "POST")
		{
			if (header.hasContentLength())
			{
				request_data.resize(header.contentLength());
				state = WAITING_FOR_CONTENT;
				bytes_read = 0;
				if (client->bytesAvailable() > 0)
					readyToRead();
			}
		}
		else if (header.method() == "GET")
		{
			srv->handleGet(this,header);
			header_data = "";
			request_data.resize(0);
		}
		else
		{
			srv->handleUnsupportedMethod(this);
		}
	}
	
	bool HttpClientHandler::sendFile(HttpResponseHeader & hdr,const TQString & full_path)
	{
	//	Out(SYS_WEB|LOG_DEBUG) << "Sending file " << full_path << endl;
		// first look in cache
		MMapFile* c = srv->cacheLookup(full_path);
		
		if (!c)
		{
			// not in cache so load it
			c = new MMapFile();
			if (!c->open(full_path,MMapFile::READ))
			{
				delete c;
				Out(SYS_WEB|LOG_DEBUG) << "Failed to open file " << full_path << endl;
				return false;
			}
			srv->insertIntoCache(full_path,c);
		}
		
		hdr.setValue("Content-Length",TQString::number(c->getSize()));
		
	//	Out(SYS_WEB|LOG_DEBUG) << "HTTP header : " << endl;
	//	Out(SYS_WEB|LOG_DEBUG) << hdr.toString() << endl;
				
		TQCString d = hdr.toString().utf8();
		client->writeBlock(d.data(),d.length());

		Uint32 written = 0;
		Uint32 total = c->getSize();
		const char* data = (const char*)c->getDataPointer();
		while (written < total)
		{
			Uint32 w = client->writeBlock(data + written,total - written);
			written += w;
		}
		client->flush();
	//	Out(SYS_WEB|LOG_DEBUG) << "Finished sending " << full_path << " (" << written << " bytes)" << endl;
		return true;
	}
	
#define HTTP_404_ERROR "<html><head><title>404 Not Found</title></head><body>The requested file was not found !</body></html>"
#define HTTP_500_ERROR "<html><head><title>HTTP/1.1 500 Internal Server Error</title></head><body>HTTP/1.1 Internal Server Error<br>%1</body></html>"

	
	void HttpClientHandler::send404(HttpResponseHeader & hdr,const TQString & path)
	{
	//	Out(SYS_WEB|LOG_DEBUG) << "Sending 404 " << path << endl;
		TQString data = HTTP_404_ERROR;
		hdr.setValue("Content-Length",TQString::number(data.length()));

		TQTextStream os(client);
		os.setEncoding( TQTextStream::UnicodeUTF8 );
		os << hdr.toString();
		os << data;
	}
	
	void HttpClientHandler::send500(HttpResponseHeader & hdr)
	{
	//	Out(SYS_WEB|LOG_DEBUG) << "Sending 500 " << endl;
		TQString data = TQString(HTTP_500_ERROR).arg("An internal server error occured !");
		hdr.setValue("Content-Length",TQString::number(data.length()));

		TQTextStream os(client);
		os.setEncoding( TQTextStream::UnicodeUTF8 );
		os << hdr.toString();
		os << data;
	}
	
	void HttpClientHandler::sendResponse(const HttpResponseHeader & hdr)
	{
	//	Out(SYS_WEB|LOG_DEBUG) << "Sending response " << hdr.toString() << endl;
		TQTextStream os(client);
		os.setEncoding( TQTextStream::UnicodeUTF8 );
		os << hdr.toString();
	}

	void HttpClientHandler::executePHPScript(
			PhpInterface* php_iface,
			HttpResponseHeader & hdr,
			const TQString & php_exe,
			const TQString & php_file,
			const TQMap<TQString,TQString> & args)
	{
	//	Out(SYS_WEB|LOG_DEBUG) << "Launching PHP script " << php_file << endl;
		php = new PhpHandler(php_exe,php_iface);
		if (!php->executeScript(php_file,args))
		{
			TQString data = TQString(HTTP_500_ERROR).arg("Failed to launch PHP executable !");
			hdr.setResponseCode(500);
			hdr.setValue("Content-Length",TQString::number(data.utf8().length()));
			
			TQTextStream os(client);
			os.setEncoding( TQTextStream::UnicodeUTF8 );
			os << hdr.toString();
			os << data;
			state = WAITING_FOR_REQUEST;
		}
		else
		{
			php_response_hdr = hdr;
			connect(php,TQ_SIGNAL(finished()),this,TQ_SLOT(onPHPFinished()));
			state = PROCESSING_PHP;
		}
	}
	
	void HttpClientHandler::onPHPFinished()
	{
		const TQByteArray & output = php->getOutput();
		php_response_hdr.setValue("Content-Length",TQString::number(output.size()));
		
		TQTextStream os(client);
		os.setEncoding( TQTextStream::UnicodeUTF8 );
		os << php_response_hdr.toString();
		os.writeRawBytes(output.data(),output.size());
		
		php->deleteLater();
		php = 0;
		state = WAITING_FOR_REQUEST;
	}
}

#include "httpclienthandler.moc"
				 

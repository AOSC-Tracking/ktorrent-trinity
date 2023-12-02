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
#include <tqxml.h>
#include <tqvaluestack.h>
#include <util/fileops.h>
#include <util/log.h>
#include <torrent/globals.h>
#include "upnprouter.h"
#include "upnpdescriptionparser.h"

using namespace bt;

namespace kt
{
	
	class XMLContentHandler : public TQXmlDefaultHandler
	{
		enum Status
		{
		    TOPLEVEL,ROOT,DEVICE,SERVICE,FIELD,OTHER
		};

		TQString tmp;
		UPnPRouter* router;
		UPnPService curr_service;
		TQValueStack<Status> status_stack;
	public:
		XMLContentHandler(UPnPRouter* router);
		virtual ~XMLContentHandler();


		bool startDocument();
		bool endDocument();
		bool startElement(const TQString &, const TQString & localName, const TQString &,
		                  const TQXmlAttributes & atts);
		bool endElement(const TQString & , const TQString & localName, const TQString &  );
		bool characters(const TQString & ch);
		
		bool interestingDeviceField(const TQString & name);
		bool interestingServiceField(const TQString & name);
	};


	UPnPDescriptionParser::UPnPDescriptionParser()
	{}


	UPnPDescriptionParser::~UPnPDescriptionParser()
	{}

	bool UPnPDescriptionParser::parse(const TQString & file,UPnPRouter* router)
	{
		bool ret = true;
		{
			TQFile fptr(file);
			if (!fptr.open(IO_ReadOnly))
				return false;

			TQXmlInputSource input(&fptr);
			XMLContentHandler chandler(router);
			TQXmlSimpleReader reader;

			reader.setContentHandler(&chandler);
			ret = reader.parse(&input,false);
		}
		
		if (!ret)
		{
			Out(SYS_PNP|LOG_IMPORTANT) << "Error parsing XML" << endl;
			return false;
		}
		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////
	
	
	XMLContentHandler::XMLContentHandler(UPnPRouter* router) : router(router)
	{}

	XMLContentHandler::~XMLContentHandler()
	{}


	bool XMLContentHandler::startDocument()
	{
		status_stack.push(TOPLEVEL);
		return true;
	}

	bool XMLContentHandler::endDocument()
	{
		status_stack.pop();
		return true;
	}
	
	bool XMLContentHandler::interestingDeviceField(const TQString & name)
	{
		return name == "friendlyName" || name == "manufacturer" || name == "modelDescription" ||
				name == "modelName" || name == "modelNumber";
	}

	
	bool XMLContentHandler::interestingServiceField(const TQString & name)
	{
		return name == "serviceType" || name == "serviceId" || name == "SCPDURL" ||
				name == "controlURL" || name == "eventSubURL";
	}

	bool XMLContentHandler::startElement(const TQString &, const TQString & localName, const TQString &,
	                                     const TQXmlAttributes & )
	{
		tmp = "";
		switch (status_stack.top())
		{
		case TOPLEVEL:
			// from toplevel we can only go to root
			if (localName == "root")
				status_stack.push(ROOT);
			else
				return false;
			break;
		case ROOT:
			// from the root we can go to device or specVersion
			// we are not interested in the specVersion
			if (localName == "device")
				status_stack.push(DEVICE);
			else
				status_stack.push(OTHER);
			break;
		case DEVICE:
			// see if it is a field we are interested in
			if (interestingDeviceField(localName))
				status_stack.push(FIELD);
			else
				status_stack.push(OTHER);
			break;
		case SERVICE:
			if (interestingServiceField(localName))
				status_stack.push(FIELD);
			else
				status_stack.push(OTHER);
			break;
		case OTHER:
			if (localName == "service")
				status_stack.push(SERVICE);
			else if (localName == "device")
				status_stack.push(DEVICE);
			else
				status_stack.push(OTHER);
			break;
		case FIELD:
			break;
		}
		return true;
	}

	bool XMLContentHandler::endElement(const TQString & , const TQString & localName, const TQString &  )
	{
		switch (status_stack.top())
		{
		case FIELD:
			// we have a field so set it
			status_stack.pop();
			if (status_stack.top() == DEVICE)
			{
				// if we are in a device
				router->getDescription().setProperty(localName,tmp);
			}
			else if (status_stack.top() == SERVICE)
			{
				// set a property of a service
				curr_service.setProperty(localName,tmp);
			}
			break;
		case SERVICE:
			// add the service
			router->addService(curr_service);
			curr_service.clear();
			// pop the stack
			status_stack.pop();
			break;
		default:
			status_stack.pop();
			break;
		}

		// reset tmp
		tmp = "";
		return true;
	}


	bool XMLContentHandler::characters(const TQString & ch)
	{
		if (ch.length() > 0)
		{
			tmp += ch;
		}
		return true;
	}

}

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

#include <tdelistview.h>
#include <tdemessagebox.h>
#include <kpushbutton.h>
#include <torrent/udptrackersocket.h>
#include <torrent/globals.h>
#include <torrent/server.h>
#include <kademlia/dhtbase.h>
#include "upnpprefwidget.h"
#include <util/log.h>
#include <util/error.h>
#include <util/waitjob.h>
#include <util/httprequest.h>
#include <torrent/globals.h>
#include "upnppluginsettings.h"

using namespace bt;

namespace kt
{
	UPnPPrefWidget::UPnPPrefWidget(TQWidget* parent, const char* name, WFlags fl)
			: UPnPWidget(parent,name,fl)
	{
		def_router = 0;
		connect(m_forward_btn,TQ_SIGNAL(clicked()),this,TQ_SLOT(onForwardBtnClicked()));
		connect(m_undo_forward_btn,TQ_SIGNAL(clicked()),this,TQ_SLOT(onUndoForwardBtnClicked()));
		connect(m_rescan,TQ_SIGNAL(clicked()),this,TQ_SLOT(onRescanClicked()));
		bt::Globals::instance().getPortList().setListener(this);
	}
	
	UPnPPrefWidget::~UPnPPrefWidget()
	{
		bt::Globals::instance().getPortList().setListener(0);
	}
	
	void UPnPPrefWidget::shutdown(bt::WaitJob* job)
	{
		if (!def_router)
			return;
		
		net::PortList & pl = bt::Globals::instance().getPortList();
		if (pl.count() == 0)
			return;
		
		for (net::PortList::iterator i = pl.begin(); i != pl.end();i++)
		{
			net::Port & p = *i;
			if (p.forward)
				def_router->undoForward(p,job);
		}
	}
	
	
	void UPnPPrefWidget::addDevice(UPnPRouter* r)
	{
		connect(r,TQ_SIGNAL(updateGUI()),this,TQ_SLOT(updatePortMappings()));
		TDEListViewItem* item = new TDEListViewItem(m_device_list,r->getDescription().friendlyName);
		item->setMultiLinesEnabled(true);
		itemmap[item] = r;
		// if we have discovered the default device or there is none
		// forward it's ports
		TQString def_dev = UPnPPluginSettings::defaultDevice();
		if (def_dev == r->getServer() || def_dev.length() == 0)
		{
			Out(SYS_PNP|LOG_DEBUG) << "Doing default port mappings ..." << endl;
			UPnPPluginSettings::setDefaultDevice(r->getServer());
			UPnPPluginSettings::writeConfig();
			
			try
			{
				net::PortList & pl = bt::Globals::instance().getPortList();
			
				for (net::PortList::iterator i = pl.begin(); i != pl.end();i++)
				{
					net::Port & p = *i;
					if (p.forward)
						r->forward(p);
				}
				
				def_router = r;
			}
			catch (Error & e)
			{
				KMessageBox::error(this,e.toString());
			}
		}
	}
		
	void UPnPPrefWidget::onForwardBtnClicked()
	{
		TDEListViewItem* item = (TDEListViewItem*)m_device_list->currentItem();;
		if (!item)
			return;
		
		UPnPRouter* r = itemmap[item];
		if (!r)
			return;
		
		try
		{
			net::PortList & pl = bt::Globals::instance().getPortList();
			
			for (net::PortList::iterator i = pl.begin(); i != pl.end();i++)
			{
				net::Port & p = *i;
				if (p.forward)
					r->forward(p);
			}
			
			TQString def_dev = UPnPPluginSettings::defaultDevice();
			if (def_dev != r->getServer())
			{
				UPnPPluginSettings::setDefaultDevice(r->getServer());
				UPnPPluginSettings::writeConfig();
				def_router = r;
			}
			
		}
		catch (Error & e)
		{
			KMessageBox::error(this,e.toString());
		}	
	}
	
	void UPnPPrefWidget::onRescanClicked()
	{
		// clear the list and emit the signal
		rescan();
	}
	
	void UPnPPrefWidget::onUndoForwardBtnClicked()
	{
		TDEListViewItem* item = (TDEListViewItem*)m_device_list->currentItem();;
		if (!item)
			return;
		
		UPnPRouter* r =  itemmap[item];
		if (!r)
			return;
		
		try
		{
			net::PortList & pl = bt::Globals::instance().getPortList();
			
			for (net::PortList::iterator i = pl.begin(); i != pl.end();i++)
			{
				net::Port & p = *i;
				if (p.forward)
					r->undoForward(p, 0);
			}
			
			TQString def_dev = UPnPPluginSettings::defaultDevice();
			if (def_dev == r->getServer())
			{
				UPnPPluginSettings::setDefaultDevice(TQString());
				UPnPPluginSettings::writeConfig();
				def_router = 0;
			}
		}
		catch (Error & e)
		{
			KMessageBox::error(this,e.toString());
		}
	}

	
	void UPnPPrefWidget::updatePortMappings()
	{
		// update all port mappings
		TQMap<TDEListViewItem*,UPnPRouter*>::iterator i = itemmap.begin();
		while (i != itemmap.end())
		{
			UPnPRouter* r = i.data();
			TDEListViewItem* item = i.key();
			TQString msg,services;
			TQValueList<UPnPRouter::Forwarding>::iterator j = r->beginPortMappings();
			while (j != r->endPortMappings())
			{
				UPnPRouter::Forwarding & f = *j;
				if (!f.pending_req)
				{
					msg += TQString::number(f.port.number) + " (";
					TQString prot = (f.port.proto == net::UDP ? "UDP" : "TCP");
					msg +=  prot + ")";
					if (f.service->servicetype.contains("WANPPPConnection"))
						services += "PPP";
					else
						services += "IP";
				}
				j++;
				if (j != r->endPortMappings())
				{
					msg += "\n";
					services += "\n";
				}
			}
			item->setText(1,msg);
			item->setText(2,services);
			i++;
		}
	}
	

	void UPnPPrefWidget::portAdded(const net::Port & port)
	{
		try
		{
			if (def_router && port.forward)
				def_router->forward(port);
		}
		catch (Error & e)
		{
			Out(SYS_PNP|LOG_DEBUG) << "Error : " << e.toString() << endl;
		}
	}
	
	void UPnPPrefWidget::portRemoved(const net::Port & port)
	{
		try
		{
			if (def_router && port.forward)
				def_router->undoForward(port, 0);
		}
		catch (Error & e)
		{
			Out(SYS_PNP|LOG_DEBUG) << "Error : " << e.toString() << endl;
		}
	}
}



#include "upnpprefwidget.moc"


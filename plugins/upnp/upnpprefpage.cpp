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
#include <tdelocale.h>
#include <tdeglobal.h>
#include <kiconloader.h>
#include "upnpprefpage.h"
#include "upnpprefwidget.h"
#include "upnprouter.h"
#include "upnpmcastsocket.h"

namespace kt
{

	UPnPPrefPage::UPnPPrefPage(UPnPMCastSocket* sock): PrefPageInterface(i18n("UPnP"), i18n("UPnP Devices"),TDEGlobal::iconLoader()->loadIcon("ktupnp",TDEIcon::NoGroup)),sock(sock)
	{
		widget = 0;
	}


	UPnPPrefPage::~UPnPPrefPage()
	{}


	bool UPnPPrefPage::apply()
	{
		return true;
	}

	void UPnPPrefPage::createWidget(TQWidget* parent)
	{
		widget = new UPnPPrefWidget(parent);
		TQObject::connect(sock,TQ_SIGNAL(discovered(UPnPRouter* )),widget,TQ_SLOT(addDevice(UPnPRouter* )));
		TQObject::connect(widget,TQ_SIGNAL(rescan()),sock,TQ_SLOT(discover()));
	}

	void UPnPPrefPage::deleteWidget()
	{
		delete widget;
		widget = 0;
	}

	void UPnPPrefPage::updateData()
	{
	}
	
	void UPnPPrefPage::shutdown(bt::WaitJob* job)
	{
		widget->shutdown(job);
	}
}

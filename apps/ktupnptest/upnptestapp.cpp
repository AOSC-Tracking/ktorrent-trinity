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
#include <kpushbutton.h>
#include <tdelocale.h>
#include <tdeapplication.h>
#include <tdemessagebox.h>
#include <tqtextbrowser.h>
#include <util/error.h>
#include "upnptestapp.h"
#include "mainwidget.h"

using namespace bt;
using namespace kt;

UPnPTestApp::UPnPTestApp(TQWidget *parent, const char *name)
		: TDEMainWindow(parent, name)
{
	sock = new UPnPMCastSocket(true);
	connect(sock,TQ_SIGNAL(discovered( UPnPRouter* )),this,TQ_SLOT(discovered( UPnPRouter* )));
	mwnd = new MainWidget(this);
	setCentralWidget(mwnd);
	connect(mwnd->test_btn,TQ_SIGNAL(clicked()),this,TQ_SLOT(onTestBtn()));
	connect(mwnd->close_btn,TQ_SIGNAL(clicked()),this,TQ_SLOT(onCloseBtn()));
	bt::Log & lg = bt::Globals::instance().getLog(0);
	lg.addMonitor(this);
	Out() << "UPnPTestApp started up !" << endl;
}


UPnPTestApp::~UPnPTestApp()
{
	sock->deleteLater();
}

void UPnPTestApp::discovered(kt::UPnPRouter* router)
{
	try
	{
		router->forward(net::Port(9999,net::TCP,false));
	}
	catch (Error & e)
	{
		KMessageBox::error(this,e.toString());
	}
}

void UPnPTestApp::onTestBtn()
{
	sock->discover();
}

void UPnPTestApp::onCloseBtn()
{
	kapp->quit();
}

bool UPnPTestApp::queryExit()
{
	bt::Log & lg = bt::Globals::instance().getLog(0);
	lg.removeMonitor(this);
	return true;
}

void UPnPTestApp::message(const TQString& line, unsigned int arg)
{
	mwnd->output->append(line);
}


#include "upnptestapp.moc"

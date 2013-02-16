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
#include <stdio.h>
#include <stdlib.h>
#include <tdelocale.h>
#include <tdeaboutdata.h>
#include <tdeapplication.h>
#include <tdecmdlineargs.h>
#include <libktorrent/functions.h>


#include "upnptestapp.h"

using namespace kt;
using namespace bt;

static const char description[] =
		I18N_NOOP("A TDE KPart Application");

static const char version[] = "1.3dev";

static TDECmdLineOptions options[] =
{
//    { "+[URL]", I18N_NOOP( "Document to open" ), 0 },
	TDECmdLineLastOption
};




int main(int argc,char** argv)
{
	Globals::instance().setDebugMode(true);
	TDEAboutData about("ktupnptest", I18N_NOOP("KTUPnPTest"), version, description,
					 TDEAboutData::License_GPL, "(C) 2005 Joris Guisson", 0,
					 "http://ktorrent.org/");
	TDECmdLineArgs::init(argc, argv,&about);
	TDECmdLineArgs::addCmdLineOptions( options );
	TDEApplication app;
	Globals::instance().initLog(kt::DataDir() + "ktupnptest.log");
	UPnPTestApp* mwnd = new UPnPTestApp();

	app.setMainWidget(mwnd);
	mwnd->show();
	app.exec();
	Globals::cleanup();
	return 0;
	
	/*
	if (argc >= 2)
	{
		kt::UPnPRouter router(TQString(),"http://foobar.com");
		kt::UPnPDescriptionParser dp;
		
		if (!dp.parse(argv[1],&router))
		{
			Out() << "Cannot parse " << TQString(argv[1]) << endl;
		}
		else
		{
			Out() << "Succesfully parsed the UPnP contents" << endl;
			router.debugPrintData();
		}
	}
	
	
	Out() << "Doing second test" << endl;
	UPnPMCastSocket mcast;
	UPnPRouter* r = mcast.parseResponse(TQCString(test_ps));
	if (r)
	{
		Out() << "Succesfully parsed test_ps" << endl;
		delete r;
	}
	else
	{
		Out() << "Failed to parse test_ps" << endl;
	}
	*/
	
}

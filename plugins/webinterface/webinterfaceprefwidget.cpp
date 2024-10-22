  /***************************************************************************
 *   Copyright (C) 2006 by Diego R. Brogna                                 *
 *   dierbro@gmail.com                                               	   *
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

#include "webinterfaceprefwidget.h"
#include "webinterfacepluginsettings.h"

#include <tdelocale.h>
#include <tdeglobal.h>
#include <kiconloader.h>
#include <kstandarddirs.h>

#include <tqwidget.h>
#include <tqstring.h>
#include <tqcheckbox.h>
#include <tqcombobox.h>
#include <knuminput.h>
#include <kurlrequester.h>
#include <klineedit.h>
#include <kpassdlg.h>
#include <kmdcodec.h>
#include <kled.h>
#include <tqtooltip.h>

#include <net/portlist.h>
#include <torrent/globals.h>
using namespace bt;
namespace kt
{

WebInterfacePrefWidget::WebInterfacePrefWidget(TQWidget *parent, const char *name):WebInterfacePreference(parent,name)
{
	port->setValue(WebInterfacePluginSettings::port());
	forward->setChecked(WebInterfacePluginSettings::forward());
	sessionTTL->setValue(WebInterfacePluginSettings::sessionTTL());
	
	TQStringList dirList=TDEGlobal::instance()->dirs()->findDirs("data", "ktorrent/www");
	TQDir d(*(dirList.begin()));
	TQStringList skinList=d.entryList(TQDir::Dirs);
	for ( TQStringList::Iterator it = skinList.begin(); it != skinList.end(); ++it ){ 
		if(*it=="." || *it=="..")
			continue;
        	interfaceSkinBox->insertItem(*it);
	}

   	interfaceSkinBox->setCurrentText (WebInterfacePluginSettings::skin());
	
	if(WebInterfacePluginSettings::phpExecutablePath().isEmpty()){
		TQString phpPath=TDEStandardDirs::findExe("php");
		if(phpPath==TQString())
			phpPath=TDEStandardDirs::findExe("php-cli");
		
		if(phpPath==TQString())
			phpExecutablePath->setURL (i18n("Php executable is not in default path, please enter the path manually"));
		else
			phpExecutablePath->setURL (phpPath);
	}
	else
	phpExecutablePath->setURL (WebInterfacePluginSettings::phpExecutablePath());

	username->setText(WebInterfacePluginSettings::username());
}

bool WebInterfacePrefWidget::apply()
{
	if(WebInterfacePluginSettings::port()==port->value()){
		if(forward->isChecked())
			bt::Globals::instance().getPortList().addNewPort(port->value(),net::TCP,true);
		else
			bt::Globals::instance().getPortList().removePort(port->value(),net::TCP);
	}		
	WebInterfacePluginSettings::setPort(port->value () );
	WebInterfacePluginSettings::setForward(forward->isChecked());
	WebInterfacePluginSettings::setSessionTTL(sessionTTL->value () );
	WebInterfacePluginSettings::setSkin(interfaceSkinBox->currentText());
	WebInterfacePluginSettings::setPhpExecutablePath(phpExecutablePath->url () );
	if(!username->text().isEmpty() && !password.isEmpty()){
		WebInterfacePluginSettings::setUsername(username->text() );
		KMD5 context(password.utf8());
		WebInterfacePluginSettings::setPassword(context.hexDigest().data());
	}

	WebInterfacePluginSettings::writeConfig();
	return true;
}

void WebInterfacePrefWidget::btnUpdate_clicked()
{
	TQString passwd;
 	int result = KPasswordDialog::getNewPassword(passwd, i18n("Please enter a new password for the web interface."));
 	if (result == KPasswordDialog::Accepted)
		password=passwd;

}

void WebInterfacePrefWidget::changeLedState()
{
       TQFileInfo fi(phpExecutablePath->url());
       if(fi.isExecutable() && (fi.isFile() || fi.isSymLink())){
               TQToolTip::add( kled, i18n("%1 exists and it is executable").arg(phpExecutablePath->url()));
               kled->setColor(green);
       }
       else if (!fi.exists()){
               TQToolTip::add( kled, i18n("%1 does not exist").arg(phpExecutablePath->url()) );
               kled->setColor(red);
       }
       else if (!fi.isExecutable()){
               TQToolTip::add( kled, i18n("%1 is not executable").arg(phpExecutablePath->url()) );
               kled->setColor(red);
       }
       else if (fi.isDir()){
               TQToolTip::add( kled, i18n("%1 is a directory").arg(phpExecutablePath->url()) );
               kled->setColor(red);
       }
       else{
               TQToolTip::add( kled, i18n("%1 is not php executable path").arg(phpExecutablePath->url()) );
               kled->setColor(red);
       }
}
}
#include "webinterfaceprefwidget.moc"

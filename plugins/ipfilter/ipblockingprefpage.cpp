/***************************************************************************
 *   Copyright (C) 2005 by Joris Guisson                                   *
 *   joris.guisson@gmail.com                                               *
 *   ivasic@gmail.com                                                      *
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
#include "ipblockingprefpage.h"
#include "ipblockingpref.h"
#include "ipfilterpluginsettings.h"
#include "ipfilterplugin.h"
#include "convertdialog.h"

#include <tdeapplication.h>
#include <tdeglobal.h>
#include <kstandarddirs.h>
#include <tdelocale.h>
#include <kiconloader.h>
#include <kurlrequester.h>
#include <kurl.h>
#include <tdemessagebox.h>
#include <tdeio/netaccess.h>
#include <kprogress.h>
#include <kmimetype.h>

#include <util/log.h>
#include <torrent/globals.h>
#include <interfaces/coreinterface.h>

#include <tqthread.h>
#include <tqlabel.h>
#include <tqcheckbox.h>
#include <tqregexp.h>
#include <tqvalidator.h>
#include <tqlayout.h>
#include <tqdialog.h>
#include <tqobject.h>

using namespace bt;

#define MAX_RANGES 500

namespace kt
{
	IPBlockingPrefPageWidget::IPBlockingPrefPageWidget(TQWidget* parent) : IPBlockingPref(parent)
	{
		m_url->setURL(IPBlockingPluginSettings::filterURL());
		if (m_url->url() == "")
			m_url->setURL(TQString("http://www.bluetack.co.uk/config/splist.zip"));
		
		bool use_level1 = IPBlockingPluginSettings::useLevel1();

		checkUseLevel1->setChecked(use_level1);
		
		if(use_level1)
		{
			lbl_status1->setText(i18n("Status: Loaded and running."));
			m_url->setEnabled(true);
			btnDownload->setEnabled(true);
		}
		else
		{
			lbl_status1->setText(i18n("Status: Not loaded."));
			m_url->setEnabled(false);
			btnDownload->setEnabled(false);
		}
		
		m_plugin = 0;
	}

	void IPBlockingPrefPageWidget::apply()
	{
		IPBlockingPluginSettings::setFilterURL(m_url->url());
		IPBlockingPluginSettings::setUseLevel1(checkUseLevel1->isChecked());
		IPBlockingPluginSettings::writeConfig();
		
		if(checkUseLevel1->isChecked())
		{
			TQFile target(TDEGlobal::dirs()->saveLocation("data","ktorrent") + "level1.dat");
			if(target.exists())
				lbl_status1->setText(i18n("Status: Loaded and running."));
			else
				lbl_status1->setText(i18n("Status: <font color=\"#ff0000\">Filter file not found.</font> Download and convert filter file."));
		}
		else
			lbl_status1->setText(i18n("Status: Not loaded."));
	}

	void IPBlockingPrefPageWidget::btnDownload_clicked()
	{
		TQString target(TDEGlobal::dirs()->saveLocation("data","ktorrent") + "level1");
		TQFile target_file(target);
		TQFile txtfile(target + ".txt");
		KURL url(m_url->url());
		KURL dest(target);
		KURL temp(TDEGlobal::dirs()->saveLocation("data","ktorrent") + "level1.tmp");
		if(TDEIO::NetAccess::exists(temp,false, this))
			TDEIO::NetAccess::del(temp,this);

		bool download = true;

		if(txtfile.exists())
		{
			if((KMessageBox::questionYesNo(this, i18n("Selected file already exists, do you want to download it again?"),i18n("File Exists")) == 4))
				download = false;
			else
				TDEIO::NetAccess::move(target, temp);
		}

		if(download)
		{
			if(!url.isLocalFile())
			{
				if (TDEIO::NetAccess::download(url,target,NULL))
				{
					//Level1 list successfully downloaded, remove temporary file
					TDEIO::NetAccess::removeTempFile(target);
					TDEIO::NetAccess::del(temp, this);
				}
				else
				{
					TQString err = TDEIO::NetAccess::lastErrorString();
					if(err != TQString())
						KMessageBox::error(0,TDEIO::NetAccess::lastErrorString(),i18n("Error"));
					else
						TDEIO::NetAccess::move(temp, target);
					
					
					//we don't want to convert since download failed
					return;
				}
			}
			else
			{
				if (!TDEIO::NetAccess::file_copy(url,dest, -1, true))
				{
					KMessageBox::error(0,TDEIO::NetAccess::lastErrorString(),i18n("Error"));
					return;
				}
			}
			
			//now determine if it's ZIP or TXT file
			KMimeType::Ptr ptr = KMimeType::findByPath(target);
			if(ptr->name() == "application/x-zip")
			{
				KURL zipfile("zip:" + target + "/splist.txt");
				KURL destinationfile(target + ".txt");
				TDEIO::NetAccess::file_copy(zipfile,destinationfile, -1, true);
			}
			else
			{
				KURL zipfile(target);
				KURL destinationfile(target + ".txt");
				TDEIO::NetAccess::file_copy(zipfile,destinationfile, -1, true);
			}
			
		}
		convert();
	}
	
	void IPBlockingPrefPageWidget::checkUseLevel1_toggled(bool check)
	{
		if(check)
		{
			m_url->setEnabled(true);
			btnDownload->setEnabled(true);
		}
		else
		{
			lbl_status1->setText("");
			m_url->setEnabled(false);
			btnDownload->setEnabled(false);
		}
	}
	
	void IPBlockingPrefPageWidget::convert()
	{
		TQFile target(TDEGlobal::dirs()->saveLocation("data","ktorrent") + "level1.dat");
		if(target.exists())
		{
			if((KMessageBox::questionYesNo(this,i18n("Filter file (level1.dat) already exists, do you want to convert it again?"),i18n("File Exists")) == 4))
				return;
// 			else
// 				TDEIO::NetAccess::del(TDEGlobal::dirs()->saveLocation("data","ktorrent") + "level1.dat", NULL);
		}
		ConvertDialog dlg(m_plugin);
		dlg.exec();
	}
	
	void IPBlockingPrefPageWidget::setPlugin(IPFilterPlugin* p)
	{
		m_plugin = p;
	}
	
	void IPBlockingPrefPageWidget::setPrefPage( IPBlockingPrefPage * p )
	{
		m_prefpage = p;
	}
	
	void IPBlockingPrefPageWidget::setConverting(bool enable)
	{
		btnDownload->setEnabled(enable);
		lbl_status1->setText("");
	}
	
	
	////////////////////////////////////////////////////////////////////////////////////

	IPBlockingPrefPage::IPBlockingPrefPage(CoreInterface* core, IPFilterPlugin* p)
	: PrefPageInterface(i18n("IPBlocking Filter"), i18n("IPBlocking Filter Options"), TDEGlobal::iconLoader()->loadIcon("filter",TDEIcon::NoGroup)), m_core(core), m_plugin(p)
	{
		widget = 0;
	}

	IPBlockingPrefPage::~IPBlockingPrefPage()
	{}

	bool IPBlockingPrefPage::apply()
	{
		widget->apply();
		
		if(IPBlockingPluginSettings::useLevel1())
			m_plugin->loadAntiP2P();
		else
			m_plugin->unloadAntiP2P();
		
		return true;
	}
	
	void IPBlockingPrefPage::createWidget(TQWidget* parent)
	{
		widget = new IPBlockingPrefPageWidget(parent);
		widget->setPlugin(m_plugin);
		widget->setPrefPage(this);
	}

	void IPBlockingPrefPage::deleteWidget()
	{
		delete widget;
		widget = 0;
	}

	void IPBlockingPrefPage::updateData()
	{}
}

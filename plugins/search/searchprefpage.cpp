/***************************************************************************
 *   Copyright (C) 2005 by Joris Guisson, Ivan Vasic                       *
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
#include <kurl.h>
#include <tqtooltip.h>
#include <tqfile.h>
#include <tdelocale.h>
#include <tdeglobal.h>
#include <kstandarddirs.h>
#include <kiconloader.h>
#include <kactivelabel.h>
#include <kpushbutton.h>
#include <tdelistview.h>
#include <klineedit.h>
#include <tdemessagebox.h>
#include <tdeio/netaccess.h>
#include <klineedit.h>

#include <tqlabel.h>
#include <tqcheckbox.h>
#include <tqradiobutton.h>

#include <util/constants.h>
#include "searchprefpage.h"
#include "searchplugin.h"
#include "searchenginelist.h"
#include "searchpluginsettings.h"

using namespace bt;

namespace kt
{
	SearchPrefPageWidget::SearchPrefPageWidget(TQWidget *parent) : SEPreferences(parent)
	{
		TQString info = i18n("Use your web browser to search for the string %1"
				" (capital letters) on the search engine you want to add. <br> "
				"Then copy the URL in the addressbar after the search is finished, and paste it here.<br><br>Searching for %1"
				" on Google for example, will result in http://www.google.com/search?q=FOOBAR&ie=UTF-8&oe=UTF-8. <br> "
				"If you add this URL here, ktorrent can search using Google.").arg("FOOBAR").arg("FOOBAR");
		TQString info_short = i18n("Use your web browser to search for the string %1 (capital letters) "
				"on the search engine you want to add. Use the resulting URL below.").arg("FOOBAR");
		m_infoLabel->setText(info_short);
		TQToolTip::add(m_infoLabel,info);
		TQToolTip::add(m_engine_name,info);
		
		connect(btnAdd, TQ_SIGNAL(clicked()), this, TQ_SLOT(addClicked()));
		connect(btnRemove, TQ_SIGNAL(clicked()), this, TQ_SLOT(removeClicked()));
		connect(btn_add_default, TQ_SIGNAL(clicked()), this, TQ_SLOT(addDefaultClicked()));
		connect(btnRemoveAll, TQ_SIGNAL(clicked()), this, TQ_SLOT(removeAllClicked()));
		
		connect(useCustomBrowser, TQ_SIGNAL(toggled(bool)), this, TQ_SLOT(customToggled( bool )));
		
		useCustomBrowser->setChecked(SearchPluginSettings::useCustomBrowser());
		useDefaultBrowser->setChecked(SearchPluginSettings::useDefaultBrowser());
		customBrowser->setText(SearchPluginSettings::customBrowser());
		
		customBrowser->setEnabled(useCustomBrowser->isChecked());
		openExternal->setChecked(SearchPluginSettings::openInExternal());
	}
	
	void SearchPrefPageWidget::updateSearchEngines(const SearchEngineList & se)
	{
		m_engines->clear();
		
		for (Uint32 i = 0;i < se.getNumEngines();i++)
		{
			new TQListViewItem(m_engines,se.getEngineName(i),se.getSearchURL(i).prettyURL());
		}
	}
 
	bool SearchPrefPageWidget::apply()
	{
		saveSearchEngines();
		
		SearchPluginSettings::setUseCustomBrowser(useCustomBrowser->isChecked());
		SearchPluginSettings::setUseDefaultBrowser(useDefaultBrowser->isChecked());
		SearchPluginSettings::setCustomBrowser(customBrowser->text());
		SearchPluginSettings::setOpenInExternal(openExternal->isChecked());
		SearchPluginSettings::writeConfig();
		return true;
	}
 
	void SearchPrefPageWidget::saveSearchEngines()
	{
		TQFile fptr(TDEGlobal::dirs()->saveLocation("data","ktorrent") + "search_engines");
		if (!fptr.open(IO_WriteOnly))
			return;
		TQTextStream out(&fptr);
		out << "# PLEASE DO NOT MODIFY THIS FILE. Use KTorrent configuration dialog for adding new search engines." << ::endl;
		out << "# SEARCH ENGINES list" << ::endl;
     
		TQListViewItemIterator itr(m_engines);
		while (itr.current())
		{
			TQListViewItem* item = itr.current();
			TQString u = item->text(1);
			TQString name = item->text(0);
			out << name.replace(" ","%20") << " " << u.replace(" ","%20") <<  endl;
			itr++;
		}
	}
 
	void SearchPrefPageWidget::addClicked()
	{
		if ( m_engine_url->text().isEmpty() || m_engine_name->text().isEmpty() )
		{
			KMessageBox::error(this, i18n("You must enter the search engine's name and URL"));
		}
		else if ( m_engine_url->text().contains("FOOBAR")  )
		{
			KURL url = KURL::fromPathOrURL(m_engine_url->text());
			if ( !url.isValid() ) 
			{ 
				KMessageBox::error(this, i18n("Malformed URL.")); 
				return; 
			}
			
			if (m_engines->findItem(m_engine_name->text(), 0)) 
			{
				KMessageBox::error(this, i18n("A search engine with the same name already exists. Please use a different name.")); return; 
			}
			
			new TQListViewItem(m_engines, m_engine_name->text(), m_engine_url->text());
			m_engine_url->setText("");
			m_engine_name->setText("");
		}
		else
		{
			KMessageBox::error(this, i18n("Bad URL. You should search for FOOBAR with your Internet browser and copy/paste the exact URL here."));
		}
	}
 
	void SearchPrefPageWidget::removeClicked()
	{
		if ( m_engines->selectedItem() == 0 ) 
			return;
 
		TQListViewItem* item = m_engines->selectedItem();
		m_engines->takeItem(item);
		delete item;
	}
 
	void SearchPrefPageWidget::addDefaultClicked()
	{
		TQListViewItem* se = new TQListViewItem(m_engines, "isohunt.to", "http://isohunt.to/torrents/?ihq=FOOBAR");    
		se = new TQListViewItem(m_engines, "mininova.org", "http://www.mininova.org/search.php?search=FOOBAR");
		se = new TQListViewItem(m_engines, "thepiratebay.se", "http://thepiratebay.se/search.php?q=FOOBAR");
		se = new TQListViewItem(m_engines, "kickass.to", "http://kickass.to/usearch/FOOBAR");
		se = new TQListViewItem(m_engines, "torrentfunk.com", "http://www.torrentfunk.com/all/torrents/FOOBAR.html");     
		se = new TQListViewItem(m_engines, "yourbittorrent.com", "http://yourbittorrent.com/?q=FOOBAR");
		se = new TQListViewItem(m_engines, "torlock.com", "http://www.torlock.com/all/torrents/FOOBAR.html");
		se = new TQListViewItem(m_engines, "torrentz.eu", "http://torrentz.eu/search?f=FOOBAR");
		se = new TQListViewItem(m_engines, "torrentcrazy.com", "http://torrentcrazy.com/s/FOOBAR");
		se = new TQListViewItem(m_engines, "bitsnoop.com", "http://bitsnoop.com/search/all/FOOBAR/c/d/1/");
		se = new TQListViewItem(m_engines, "torrents.net", "http://www.torrents.net/find/FOOBAR/");
		se = new TQListViewItem(m_engines, "btmon.com", "http://www.btmon.com/torrent/?f=FOOBAR");
	}
 
	void SearchPrefPageWidget::removeAllClicked()
	{
		m_engines->clear();
	}
	
	void SearchPrefPageWidget::btnUpdate_clicked()
	{
		TQString fn = TDEGlobal::dirs()->saveLocation("data","ktorrent") + "search_engines.tmp";
		KURL source("http://www.ktorrent.org/downloads/search_engines");
		
		if (TDEIO::NetAccess::download(source,fn,NULL))
		{
			//list successfully downloaded, remove temporary file
			updateList(fn);
			saveSearchEngines();
			TDEIO::NetAccess::removeTempFile(fn);
		}
	}
	
	void SearchPrefPageWidget::updateList(TQString& source)
	{
		TQFile fptr(source);
     
		if (!fptr.open(IO_ReadOnly))
			return;
 
		TQTextStream in(&fptr);
		
		TQMap<TQString,KURL> engines;
		
		while (!in.atEnd())
		{
			TQString line = in.readLine();

			if(line.startsWith("#") || line.startsWith(" ") || line.isEmpty() )
				continue;

			TQStringList tokens = TQStringList::split(" ", line);
			TQString name = tokens[0];
			name = name.replace("%20"," ");
			
			KURL url = KURL::fromPathOrURL(tokens[1]);
			for(Uint32 i=2; i<tokens.count(); ++i)
				url.addQueryItem(tokens[i].section("=",0,0), tokens[i].section("=", 1, 1));
			
			engines.insert(name,url);
		}
		
		TQMap<TQString,KURL>::iterator i = engines.begin();
		while (i != engines.end())
		{	
			TQListViewItem* item = m_engines->findItem(i.key(),0);
			// if we have found the item, replace it if not make a new one
			if (item)
				item->setText(1, i.data().prettyURL());
			else
				new TQListViewItem(m_engines,i.key(),i.data().prettyURL());
			
			i++;
		}
	}
	
	////////////////////////////////////////////////////////////////////////////////////////////
	

	SearchPrefPage::SearchPrefPage(SearchPlugin* plugin)
		: PrefPageInterface(i18n("a noun", "Search"), i18n("Search Engine Options"),
							TDEGlobal::iconLoader()->loadIcon("viewmag",TDEIcon::NoGroup)), m_plugin(plugin)
	{
		widget = 0;
	}


	SearchPrefPage::~SearchPrefPage()
	{}


	bool SearchPrefPage::apply()
	{
		bool ret = widget->apply();
		if(ret)
			m_plugin->preferencesUpdated();
		
		return ret;
	}

	void SearchPrefPage::createWidget(TQWidget* parent)
	{
		widget = new SearchPrefPageWidget(parent);
	}

	void SearchPrefPage::deleteWidget()
	{
		delete widget;
	}

	void SearchPrefPage::updateData()
	{
		widget->updateSearchEngines(m_plugin->getSearchEngineList());
		
	}
	
	void SearchPrefPageWidget::customToggled(bool toggled)
	{
		customBrowser->setEnabled(toggled);
	}
}

#include "searchprefpage.moc"

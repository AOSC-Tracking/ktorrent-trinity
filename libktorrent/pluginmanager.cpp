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
#include <tqfile.h>
#include <textstream.h>
#include <kparts/componentfactory.h>
#include <util/log.h>
#include <util/error.h>
#include <util/fileops.h>
#include <util/waitjob.h>
#include <torrent/globals.h>
#include <interfaces/guiinterface.h>
#include "pluginmanager.h"
#include "pluginmanagerprefpage.h"

using namespace bt;

namespace kt
{

	PluginManager::PluginManager(CoreInterface* core,GUIInterface* gui) : core(core),gui(gui)
	{
		unloaded.setAutoDelete(false);
		plugins.setAutoDelete(false);
		prefpage = 0;
		pltoload.append("Info Widget");
		pltoload.append("Search");
	}

	PluginManager::~PluginManager()
	{
		delete prefpage;
		unloaded.setAutoDelete(true);
		plugins.setAutoDelete(true);
	}

	void PluginManager::loadPluginList()
	{
		KTrader::OfferList offers = KTrader::self()->query("KTorrent/Plugin");

		KTrader::OfferList::ConstIterator iter;
		for(iter = offers.begin(); iter != offers.end(); ++iter)
		{
			KService::Ptr service = *iter;
			int errCode = 0;
			Plugin* plugin =
					KParts::ComponentFactory::createInstanceFromService<kt::Plugin>
					(service, 0, 0, TQStringList(),&errCode);

			if (!plugin)
				continue;


			if (!plugin->versionCheck(kt::VERSION_STRING))
			{
				Out(SYS_GEN|LOG_NOTICE) <<
						TQString("Plugin %1 version does not match KTorrent version, unloading it.")
						.arg(service->library()) << endl;

				delete plugin;
				// unload the library again, no need to have it loaded
				KLibLoader::self()->unloadLibrary(service->library().local8Bit());
				continue;
			}

			unloaded.insert(plugin->getName(),plugin);
			if (pltoload.contains(plugin->getName()))
				load(plugin->getName());
		}
		
		if (!prefpage)
		{
			prefpage = new PluginManagerPrefPage(this);
			gui->addPrefPage(prefpage);
		}
		prefpage->updatePluginList();
	}


	void PluginManager::load(const TQString & name)
	{
		Plugin* p = unloaded.find(name);
		if (!p)
			return;

		Out(SYS_GEN|LOG_NOTICE) << "Loading plugin "<< p->getName() << endl;
		p->setCore(core);
		p->setGUI(gui);
		p->load();
		gui->mergePluginGui(p);
		unloaded.erase(name);
		plugins.insert(p->getName(),p);
		p->loaded = true;

		if (!cfg_file.isNull())
			saveConfigFile(cfg_file);
	}

	void PluginManager::unload(const TQString & name)
	{
		Plugin* p = plugins.find(name);
		if (!p)
			return;
		
		// first shut it down properly
		bt::WaitJob* wjob = new WaitJob(2000);
		try
		{
			p->shutdown(wjob);
			if (wjob->needToWait())
				bt::WaitJob::execute(wjob);
			else
				delete wjob;
		}
		catch (Error & err)
		{
			Out(SYS_GEN|LOG_NOTICE) << "Error when unloading plugin: " << err.toString() << endl;
		}
		

		gui->removePluginGui(p);
		p->unload();
		plugins.erase(name);
		unloaded.insert(p->getName(),p);
		p->loaded = false;

		if (!cfg_file.isNull())
			saveConfigFile(cfg_file);
	}

	void PluginManager::loadAll()
	{
		bt::PtrMap<TQString,Plugin>::iterator i = unloaded.begin();
		while (i != unloaded.end())
		{
			Plugin* p = i->second;
			p->setCore(core);
			p->setGUI(gui);
			p->load();
			gui->mergePluginGui(p);
			plugins.insert(p->getName(),p);
			p->loaded = true;
			i++;
		}
		unloaded.clear();
		if (!cfg_file.isNull())
			saveConfigFile(cfg_file);
	}

	void PluginManager::unloadAll(bool save)
	{
		// first properly shutdown all plugins
		bt::WaitJob* wjob = new WaitJob(2000);
		try
		{
			bt::PtrMap<TQString,Plugin>::iterator i = plugins.begin();
			while (i != plugins.end())
			{
				Plugin* p = i->second;
				p->shutdown(wjob);
				i++;
			}
			
			if (wjob->needToWait())
				bt::WaitJob::execute(wjob);
			else
				delete wjob;
		}
		catch (Error & err)
		{
			Out(SYS_GEN|LOG_NOTICE) << "Error when unloading all plugins: " << err.toString() << endl;
		}
		
		// then unload them
		bt::PtrMap<TQString,Plugin>::iterator i = plugins.begin();
		while (i != plugins.end())
		{
			Plugin* p = i->second;
			gui->removePluginGui(p);
			p->unload();
			unloaded.insert(p->getName(),p);
			p->loaded = false;
			i++;
		}
		plugins.clear();
		if (save && !cfg_file.isNull())
			saveConfigFile(cfg_file);
	}

	void PluginManager::updateGuiPlugins()
	{
		bt::PtrMap<TQString,Plugin>::iterator i = plugins.begin();
		while (i != plugins.end())
		{
			Plugin* p = i->second;
			p->guiUpdate();
			i++;
		}
	}

	void PluginManager::fillPluginList(TQPtrList<Plugin> & plist)
	{
		bt::PtrMap<TQString,Plugin>::iterator i = plugins.begin();
		while (i != plugins.end())
		{
			Plugin* p = i->second;
			plist.append(p);
			i++;
		}


		i = unloaded.begin();
		while (i != unloaded.end())
		{
			Plugin* p = i->second;
			plist.append(p);
			i++;
		}
	}

	bool PluginManager::isLoaded(const TQString & name) const
	{
		const Plugin* p = plugins.find(name);
		return p != 0;
	}

	void PluginManager::loadConfigFile(const TQString & file)
	{
		cfg_file = file;
		// make a default config file if doesn't exist
		if (!bt::Exists(file))
		{
			writeDefaultConfigFile(file);
			return;
		}

		TQFile f(file);
		if (!f.open(IO_ReadOnly))
		{
			Out(SYS_GEN|LOG_DEBUG) << "Cannot open file " << file << " : " << TQString(f.errorString()) << endl;
			return;
		}

		pltoload.clear();

		TQTextStream in(&f);
		while (!in.atEnd())
		{
			TQString l = in.readLine();
			if (l.isNull())
				break;

			pltoload.append(l);
		}
	}

	void PluginManager::saveConfigFile(const TQString & file)
	{
		cfg_file = file;
		TQFile f(file);
		if (!f.open(IO_WriteOnly))
		{
			Out(SYS_GEN|LOG_DEBUG) << "Cannot open file " << file << " : " << TQString(f.errorString()) << endl;
			return;
		}

		TQTextStream out(&f);
		bt::PtrMap<TQString,Plugin>::iterator i = plugins.begin();
		while (i != plugins.end())
		{
			Plugin* p = i->second;
			out << p->getName() << endl;
			i++;
		}
	}


	void PluginManager::writeDefaultConfigFile(const TQString & file)
	{
		// by default we will load the infowidget and searchplugin
		TQFile f(file);
		if (!f.open(IO_WriteOnly))
		{
			Out(SYS_GEN|LOG_DEBUG) << "Cannot open file " << file << " : " << TQString(f.errorString()) << endl;
			return;
		}

		TQTextStream out(&f);
		
		out << "Info Widget" << endl << "Search" << endl;

		pltoload.clear();
		pltoload.append("Info Widget");
		pltoload.append("Search");
	}
}

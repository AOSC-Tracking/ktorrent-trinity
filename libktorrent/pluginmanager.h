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
#ifndef KTPLUGINMANAGER_H
#define KTPLUGINMANAGER_H

#include <tqptrlist.h>
#include <util/ptrmap.h>
#include <interfaces/plugin.h>
#include <tqstringlist.h>


namespace kt
{
	class CoreInterface;
	class GUIInterface;
	class PluginManagerPrefPage;
	
	/**
	 * @author Joris Guisson
	 * @brief Class to manage plugins
	 *
	 * This class manages all plugins. Plugins are stored in a map
	 */
	class PluginManager
	{
		bt::PtrMap<TQString,Plugin> plugins,unloaded;
		CoreInterface* core;
		GUIInterface* gui;
		PluginManagerPrefPage* prefpage;
		TQStringList pltoload;
		TQString cfg_file;
	public:
		PluginManager(CoreInterface* core,GUIInterface* gui);
		virtual ~PluginManager();

		/**
		 * Load the list of plugins.
		 * This basically uses KTrader to get a list of available plugins, and
		 * loads those, but does not initialize them. We will consider a plugin loaded
		 * when it's load method is called.
		 */
		void loadPluginList();

		/**
		 * Loads which plugins need to be loaded from a file.
		 * @param file The file
		 */
		void loadConfigFile(const TQString & file);

		/**
		 * Saves which plugins are loaded to a file.
		 * @param file The file
		 */
		void saveConfigFile(const TQString & file);

		/**
		 * Fill a list with all available plugins.
		 * @param pllist The plugin list
		 */
		void fillPluginList(TQPtrList<Plugin> & plist);

		/**
		 * Is a plugin loaded
		 * @param name Naame of plugin.
		 * @return True if it is, false if it isn't
		 */
		bool isLoaded(const TQString & name) const;
		
		/**
		 * Load a plugin.
		 * @param name Name of the plugin
		 */
		void load(const TQString & name);
		
		/**
		 * Unload a plugin.
		 * @param name Name of the plugin
		 */
		void unload(const TQString & name);
		
		/**
		 * Load all unloaded plugins.
		*/
		void loadAll();
		
		/**
		 * Unload all loaded plugins.
		 */
		void unloadAll(bool save = true);

		/**
		 * Update all plugins who need a periodical GUI update.
		 */
		void updateGuiPlugins();
	private:
		void writeDefaultConfigFile(const TQString & file);
	};

}

#endif

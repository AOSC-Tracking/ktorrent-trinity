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
#ifndef KTPLUGIN_H
#define KTPLUGIN_H

#include <ktversion.h>
#include <tdeparts/plugin.h>

namespace bt
{
	class WaitJob;
}

namespace kt
{
	class CoreInterface;
	class GUIInterface;

	/**
	 * @author Joris Guisson
	 * @brief Base class for all plugins
	 *
	 * This is the base class for all plugins. Plugins should implement
	 * the load and unload methods, any changes made in load must be undone in
	 * unload.
	 *
	 * It's also absolutely forbidden to do any complex initialization in the constructor
	 * (setting an int to 0 is ok, creating widgets isn't).
	 * Only the name, author and description may be set in the constructor.
	 */
	class Plugin : public KParts::Plugin
	{
		TQ_OBJECT
  
	public:
		/**
		 * Constructor, set the name of the plugin, the name and e-mail of the author and
		 * a short description of the plugin.
		 * @param name Name of plugin
		 * @param gui_name Name to display in GUI (i18n version of name)
		 * @param author Author of plugin
		 * @param mail E-mail address of author
		 * @param description What does the plugin do
		 * @param icon Name of the plugin's icon
		 */
		Plugin(TQObject *parent,const char* qt_name,const TQStringList & args,
			   const TQString & name,const TQString & gui_name,const TQString & author,
			   const TQString & email,const TQString & description,
			   const TQString & icon);
		virtual ~Plugin();

		/**
		 * This gets called, when the plugin gets loaded by KTorrent.
		 * Any changes made here must be later made undone, when unload is
		 * called.
		 * Upon error a bt::Error should be thrown. And the plugin should remain
		 * in an uninitialized state. The Error contains an error message, which will
		 * get show to the user.
		 */
		virtual void load() = 0;
		
		/**
		 * Gets called when the plugin gets unloaded.
		 * Should undo anything load did.
		 */
		virtual void unload() = 0;

		/**
		 * For plugins who need to update something, the same time as the
		 * GUI updates.
		 */
		virtual void guiUpdate();
		
		/**
		 * This should be implemented by plugins who need finish of some stuff which might take some time.
		 * These operations must be finished or killed by a timeout before we can proceed with unloading the plugin. 
		 * @param job The WaitJob which monitors the plugin
		 */
		virtual void shutdown(bt::WaitJob* job);

		const TQString & getName() const {return name;}
		const TQString & getAuthor() const {return author;}
		const TQString & getEMailAddress() const {return email;}
		const TQString & getDescription() const {return description;}
		const TQString & getIcon() const {return icon;}
		const TQString & getGuiName() const {return gui_name;}

		/// Get a pointer to the CoreInterface
		CoreInterface* getCore() {return core;}

		/// Get a const pointer to the CoreInterface
		const CoreInterface* getCore() const {return core;}

		/**
		 * Set the core, used by PluginManager to set the pointer
		 * to the core.
		 * @param c Pointer to the core
		 */
		void setCore(CoreInterface* c) {core = c;}

		/// Get a pointer to the CoreInterface
		GUIInterface* getGUI() {return gui;}

		/// Get a const pointer to the CoreInterface
		const GUIInterface* getGUI() const {return gui;}

		/**
		 * Set the core, used by PluginManager to set the pointer
		 * to the core.
		 * @param c Pointer to the core
		 */
		void setGUI(GUIInterface* c) {gui = c;}

		/// See if the plugin is loaded
		bool isLoaded() const {return loaded;}
		
		/// Check wether the plugin matches the version of KT
		virtual bool versionCheck(const TQString & version) const = 0;
		
	private:
		TQString name;
		TQString author;
		TQString email;
		TQString description;
		TQString icon;
		TQString gui_name;
		CoreInterface* core;
		GUIInterface* gui;
		bool loaded;

		friend class PluginManager;
	};

}

#endif

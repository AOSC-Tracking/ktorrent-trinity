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
#ifndef KTSEARCHPLUGIN_H
#define KTSEARCHPLUGIN_H

#include <interfaces/plugin.h>

namespace kt
{
	class UPnPMCastSocket;
	class UPnPPrefPage;

	/**
	@author Joris Guisson
	*/
	class UPnPPlugin : public Plugin
	{
		TQ_OBJECT
  
	public:
		UPnPPlugin(TQObject* parent, const char* name, const TQStringList& args);
		virtual ~UPnPPlugin();

		virtual void load();
		virtual void unload();
		virtual void shutdown(bt::WaitJob* job);
		virtual bool versionCheck(const TQString& version) const;
	private:
		UPnPMCastSocket* sock;
		UPnPPrefPage* pref;
	};

}

#endif

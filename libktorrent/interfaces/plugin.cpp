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
#include "plugin.h"

namespace kt
{

	Plugin::Plugin(TQObject *parent, const char* qt_name,const TQStringList & /*args*/,
				   const TQString & name,const TQString & gui_name,const TQString & author,
				   const TQString & email,const TQString & description,
				   const TQString & icon)
	: KParts::Plugin(parent,qt_name),
	name(name),author(author),email(email),description(description),icon(icon),gui_name(gui_name)
	{
		core = 0;
		gui = 0;
		loaded = false;
	}


	Plugin::~Plugin()
	{}

	void Plugin::guiUpdate()
	{
	}
	
	void Plugin::shutdown(bt::WaitJob* )
	{
	}
}
#include "plugin.moc"

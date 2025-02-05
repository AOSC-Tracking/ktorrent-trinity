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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
 ***************************************************************************/
#include <kiconloader.h>
#include <tdeglobal.h>
#include "group.h"

namespace kt
{

	Group::Group(const TQString & name,int flags) : name(name),flags(flags)
	{}


	Group::~Group()
	{}

	void Group::save(bt::BEncoder*)
	{
	}
	
	void Group::load(bt::BDictNode* )
	{
	}

	void Group::setIconByName(const TQString & in)
	{
		icon_name = in;
		icon = TDEGlobal::iconLoader()->loadIcon(in,TDEIcon::Small);
	}
	
	void Group::rename(const TQString & nn)
	{
		name = nn;
	}
	
	void Group::torrentRemoved(TorrentInterface* )
	{}
	
	void Group::removeTorrent(TorrentInterface* )
	{}
	
	void Group::addTorrent(TorrentInterface* )
	{}
}

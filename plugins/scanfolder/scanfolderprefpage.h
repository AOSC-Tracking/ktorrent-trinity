/***************************************************************************
 *   Copyright (C) 2006 by Ivan Vasić   								   *
 *   ivasic@gmail.com   												   *
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
#ifndef KTSCANFOLDERPREFPAGE_H
#define KTSCANFOLDERPREFPAGE_H

#include <interfaces/prefpageinterface.h>

#include "scanfolderplugin.h"
#include "scanfolderprefpagewidget.h"

namespace kt
{

	/**
	 * ScanFolder plugin preferences page
	 * @author Ivan Vasić <ivasic@gmail.com>
	 */
	class ScanFolderPrefPage : public PrefPageInterface
	{
		public:
			ScanFolderPrefPage(ScanFolderPlugin* plugin);
			virtual ~ScanFolderPrefPage();

			virtual bool apply();
			virtual void createWidget(TQWidget* parent);
			virtual void updateData();
			virtual void deleteWidget();

		private:
			ScanFolderPlugin* m_plugin;
			ScanFolderPrefPageWidget* m_widget;
	};

}

#endif

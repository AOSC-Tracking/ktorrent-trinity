/***************************************************************************
 *   Copyright (C) 2007 by Lukasz Fibinger <lucke@o2.pl>                   *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef FILTERBAR_H
#define FILTERBAR_H

#include <tqwidget.h>

class TQLabel;
class TQCheckBox;
class TDEConfig;
class KLineEdit;
class KPushButton;
class TDEToolBarButton;

namespace kt
{
	class TorrentInterface;
}

/**
 * Provides a filterbar allowing to show only select items
 *
 * based on dolphin's one (made by Gregor Kališnik)
 */
class FilterBar : public TQWidget
{
	TQ_OBJECT
  

public:
	FilterBar ( TQWidget *parent = 0, const char *name = 0 );
	virtual ~FilterBar();
	
	bool matchesFilter(kt::TorrentInterface* tc);
	void saveSettings(TDEConfig* cfg);
	void loadSettings(TDEConfig* cfg);

private slots:
	void slotChangeFilter(const TQString& nameFilter);

protected:
	virtual void keyPressEvent ( TQKeyEvent* event );
	virtual void hideEvent(TQHideEvent* event);

private:
	TQLabel* m_filter;
	KLineEdit* m_filterInput;
	KPushButton* m_clear;
	TQCheckBox* m_case_sensitive;
	TDEToolBarButton* m_close;
	TQString m_name_filter;
};

#endif

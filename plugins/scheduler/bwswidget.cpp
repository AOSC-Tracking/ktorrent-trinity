/***************************************************************************
 *   Copyright (C) 2006 by Ivan Vasić   *
 *   ivasic@gmail.com   *
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
#include "bwswidget.h"
#include "schedulerpluginsettings.h"

#include <klocale.h>
#include <tqstringlist.h>
#include <tqpixmap.h>
#include <tqtable.h>
#include <tqpainter.h>
#include <tqrect.h>
#include <tqfont.h>
#include <tqcolor.h>

#include <kglobal.h>
#include <kstandarddirs.h>

namespace kt
{
	BWSWidget::BWSWidget(TQWidget* tqparent, const char* name, bool useColors)
			: TQTable(tqparent,name), m_leftCategory(1), m_rightCategory(0), draw_focus(true), right_click(false), use_colors(useColors)
	{
		use_colors = SchedulerPluginSettings::useColors();
		
		TQStringList days;
		TQStringList hours;

		days << i18n("MON") << i18n("TUE") << i18n("WED") << i18n("THU") << i18n("FRI") << i18n("SAT") << i18n("SUN");
		hours <<
		"00:00-00:59h" << "01:00-01:59h" <<
		"02:00-02:59h" << "03:00-03:59h" <<
		"04:00-04:59h" << "05:00-05:59h" <<
		"06:00-06:59h" << "07:00-07:59h" <<
		"08:00-08:59h" << "09:00-09:59h" <<
		"10:00-10:59h" << "11:00-11:59h" <<
		"12:00-12:59h" << "13:00-13:59h" <<
		"14:00-14:59h" << "15:00-15:59h" <<
		"16:00-16:59h" << "17:00-17:59h" <<
		"18:00-18:59h" << "19:00-19:59h" <<
		"20:00-20:59h" << "21:00-21:59h" <<
		"22:00-22:59h" << "23:00-23:59h";

		insertRows(0,24);
		insertColumns(0,7);

		setColumnLabels(days);
		setRowLabels(hours);
		
		setFocusPolicy(TQ_StrongFocus);

		TQFont f;
		f.setPointSize(8);
		setFont(f);

		for(int i=0; i<7; ++i) {
			setColumnWidth(i,40);
			setColumnStretchable(i, true);
		}
		for(int i=0; i<24; ++i) {
			setColumnWidth(i,40);
			setRowStretchable(i, true);
		}

		setColumnMovingEnabled(false);
		setRowMovingEnabled(false);

		setSorting(false);

		adjustSize();

		for(int i=0; i<5; ++i) {
			m_pix[i] = 0;
			m_pixf[i] = 0;
			m_color[i] = 0;
			m_colorf[i] = 0;
		}

		setType(use_colors);

		resetSchedule();

		setSelectionMode(TQTable::NoSelection);
		
		setHScrollBarMode(TQTable::AlwaysOff);
		setVScrollBarMode(TQTable::AlwaysOff);

		connect(this, TQT_SIGNAL(currentChanged( int, int )), this, TQT_SLOT(cellSelectionChanged( int, int )));
		connect(this, TQT_SIGNAL(pressed(int, int, int, const TQPoint&)), this, TQT_SLOT(cellMouseDown(int, int, int, const TQPoint& )));
	}

	BWSWidget::~BWSWidget()
	{
		for(int i=0; i<5; ++i) {
			if(m_pix[i])
				delete m_pix[i];
			if(m_pixf[i])
				delete m_pixf[i];
		}
	}

	void BWSWidget::paintFocus(TQPainter* p, const TQRect& cr)
	{
		int x = rowAt(cr.y());
		int y = columnAt(cr.x());

		if(x == 1 && y == 1) {
			int trt = 0;
			++trt;
		}

		if(lastFocused.x != x || lastFocused.y != y)
			updateCell(lastFocused.x, lastFocused.y);

		if(draw_focus) {

			if(right_click)
				drawCell(p, m_rightCategory, true);
			else
				drawCell(p, m_leftCategory, true);

			lastFocused.x = x;
			lastFocused.y = y;
		} else {
			lastFocused.x = -1;
			lastFocused.y = -1;
		}
	}

	void BWSWidget::cellSelectionChanged(int row, int col)
	{
		if(right_click)
			setText(row, col, TQString::number(m_rightCategory));
		else
			setText(row, col, TQString::number(m_leftCategory));
	}

	void BWSWidget::paintCell(TQPainter* p, int row, int col, const TQRect& cr, bool selected)
	{
		if(selected)
			return;

		bool ok;
		int cat = text(row,col).toInt(&ok);
		if((ok || cat == 0) && cat >= 0 && cat <= 4)
			drawCell(p, cat);
		else
			setText(row,col,TQString::number(0));
		// 	TQTable::paintCell(p,row,col,cr,selected);
	}

	void BWSWidget::resetSchedule()
	{
		for(int i=0; i<7; ++i)
			for(int j=0; j<24; ++j)
				setText(j,i, "0");
		draw_focus = false;
		clearSelection();
		updateHeaderStates();
	}

	void BWSWidget::repaintWidget()
	{
		for(int i=0; i<7; ++i)
			for(int j=0; j<24; ++j)
				updateCell(j,i);
	}

	void BWSWidget::cellMouseDown(int row, int col, int button, const TQPoint& mousePos)
	{
		right_click = button == 2;
		draw_focus = true;
		cellSelectionChanged(row,col);
	}

	void BWSWidget::clearSelect()
	{
		draw_focus = false;
		clearSelection();
		updateHeaderStates();
		repaintWidget();
	}

	void BWSWidget::setLeftCategory(const int& theValue)
	{
		m_leftCategory = theValue;
	}

	void BWSWidget::setRightCategory(const int& theValue)
	{
		m_rightCategory = theValue;
	}


	void BWSWidget::setUseColors(bool theValue)
	{
		use_colors = theValue;
	}

	void BWSWidget::drawCell(TQPainter* p, int category, bool focus)
	{
		if(use_colors) {
			if(focus) {
				p->fillRect(0,0,40,20,*m_colorf[category]);
			} else {
				p->fillRect(0,0,40,20,*m_color[category]);
			}

			switch(category) {
					case 0:
					// 				p->drawText(TQRect(0,0,40,20), TQt::AlignCenter | TQt::SingleLine, "normalllll");
					break;
					case 1:
					case 2:
					case 3:
					p->drawText(TQRect(0,0,40,20), TQt::AlignCenter | TQt::SingleLine, TQString::number(category));
					break;
					case 4:
					p->drawText(TQRect(0,0,40,20), TQt::AlignCenter | TQt::SingleLine, "off");
					break;
			}

			p->drawRect(0,0,40,20);
		} else {
			if(focus)
				p->drawPixmap(0,0,*m_pixf[category]);
			else
				p->drawPixmap(0,0, *m_pix[category]);
		}
	}

	void BWSWidget::setType(bool color)
	{
		if(color) {
			for(int i=0; i<5; ++i) {
				if(m_pix[i])
					delete m_pix[i];
				if(m_pixf[i])
					delete m_pixf[i];

				m_pix[i] = 0;
				m_pixf[i] = 0;
			}

			m_color[0] = new TQColor(30,165,105);
			m_color[1] = new TQColor(195,195,70);
			m_color[2] = new TQColor(195,195,70);
			m_color[3] = new TQColor(195,195,70);
			m_color[4] = new TQColor(190,30,30);

			m_colorf[0] = new TQColor(40,200,130);
			m_colorf[1] = new TQColor(210,220,130);
			m_colorf[2] = new TQColor(210,220,130);
			m_colorf[3] = new TQColor(210,220,130);
			m_colorf[4] = new TQColor(230,40,40);

		} else {
			for(int i=0; i<5; ++i) {
				if(m_color[i])
					delete m_color[i];
				if(m_colorf[i])
					delete m_colorf[i];

				m_color[i] = 0;
				m_colorf[i] = 0;
			}

			m_pix[0] = new TQPixmap(locate("data", TQString("ktorrent/icons/cell-a-0000.png")));
			m_pix[1] = new TQPixmap(locate("data", TQString("ktorrent/icons/cell-a-0001.png")));
			m_pix[2] = new TQPixmap(locate("data", TQString("ktorrent/icons/cell-a-0002.png")));
			m_pix[3] = new TQPixmap(locate("data", TQString("ktorrent/icons/cell-a-0003.png")));
			m_pix[4] = new TQPixmap(locate("data", TQString("ktorrent/icons/cell-a-0004.png")));

			m_pixf[0] = new TQPixmap(locate("data", TQString("ktorrent/icons/cell-b-0000.png")));
			m_pixf[1] = new TQPixmap(locate("data", TQString("ktorrent/icons/cell-b-0001.png")));
			m_pixf[2] = new TQPixmap(locate("data", TQString("ktorrent/icons/cell-b-0002.png")));
			m_pixf[3] = new TQPixmap(locate("data", TQString("ktorrent/icons/cell-b-0003.png")));
			m_pixf[4] = new TQPixmap(locate("data", TQString("ktorrent/icons/cell-b-0004.png")));
		}

		use_colors = color;

		repaintWidget();
	}
	
	void BWSWidget::setSchedule(const BWS& theValue)
	{
		m_schedule = theValue;
		for(int i=0; i<7; ++i)
			for(int j=0; j<24; ++j)
				setText(j,i, TQString::number((int) m_schedule.getCategory(i,j)));
	}
	
	const BWS& BWSWidget::schedule()
	{
		for(int i=0; i<7; ++i)
		{
			for(int j=0; j<24; ++j)
			{
				bool ok;
				ScheduleCategory cat = (ScheduleCategory) text(j,i).toInt(&ok);
				
				if((ok || cat == 0) && cat >= 0 && cat <= 4) //precaution
					m_schedule.setCategory(i,j, cat);
				else
					m_schedule.setCategory(i,j,(ScheduleCategory) 0);
			}
		}
		
		return m_schedule;
	}
	
	void BWSWidget::focusOutEvent(TQFocusEvent* e)
	{
		if(e->lostFocus())
			clearSelect();
	}
}

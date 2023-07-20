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
#ifndef KTBWSWIDGET_H
#define KTBWSWIDGET_H

#include <tqtable.h>
#include <tqpixmap.h>
#include <tqpoint.h>
#include <tqcolor.h>

#include "bwscheduler.h"

namespace kt
{
	typedef struct _focusedCell
	{
		int x;
		int y;
	}
	FocusedCell;

	/**
	 * @brief Bandwidth scheduler widget.
	 * @author Ivan Vasić <ivasic@gmail.com>
	 * This class is a TQTable with customized cells. It's used for bandwidth scheduling by painting each cell with mouse moves or keyboard.
	 */
	class BWSWidget : public TQTable
	{
			TQ_OBJECT
  
		public:
			BWSWidget(TQWidget* parent = 0, const char* name = 0, bool useColors = true);
			~BWSWidget();

			///Repaints the whole widget
			void repaintWidget();

			///Clears selection (removes focus)
			void clearSelect();

			///Sets category for left mouse click.
			void setLeftCategory(const int& theValue);
			///Sets category for right mouse click.
			void setRightCategory(const int& theValue);

			
			/**
			 * Sets cell paint type.
			 * @param color TRUE - paints colors. FALSE - paints pixmaps.
			 */
			void setType(bool color);
			
			///Returns current schedule.
			const BWS& schedule();
			///Sets schedule.
			void setSchedule(const BWS& theValue);

		public slots:
			void resetSchedule();
			void setUseColors(bool theValue);

		private slots:
			void cellSelectionChanged(int row, int col);
			void cellMouseDown(int row, int col, int button, const TQPoint& mousePos);

		private:
			void paintFocus(TQPainter* p, const TQRect& cr);
			void paintCell(TQPainter* p, int row, int col, const TQRect& cr, bool selected);
			void drawCell(TQPainter* p, int category, bool focus = false);
			void focusOutEvent(TQFocusEvent*);

			TQPixmap* m_pix[5];
			TQPixmap* m_pixf[5];

			TQColor* m_color[5];
			TQColor* m_colorf[5];

			int m_leftCategory;
			int m_rightCategory;

			FocusedCell lastFocused;

			bool draw_focus;
			bool right_click;

			bool use_colors;
			
			BWS m_schedule;
	};
}
#endif

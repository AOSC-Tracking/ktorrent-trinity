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
#ifndef KTEXPANDABLEWIDGET_H
#define KTEXPANDABLEWIDGET_H

#include <tqwidget.h>
#include <tqptrlist.h>
#include <interfaces/guiinterface.h>

class TQSplitter;
class TQHBoxLayout;

namespace kt
{

	

	/**
	 * @author Joris Guisson
	 * @brief Widget which can be expanded with more widgets
	 *
	 * This is a sort of container widget, which at the minimum has
	 * one child widget. It allows to add more widgets separating the new widget
	 * and everything which was previously in the container by a separator.
	*/
	class ExpandableWidget : public TQWidget
	{
		Q_OBJECT
  TQ_OBJECT
	public:
		/**
		 * Constructor, the first child must be provided.
		 * @param child The first child
		 * @param tqparent The tqparent
		 * @param name The name
		 */
		ExpandableWidget(TQWidget* child,TQWidget *tqparent = 0, const char *name = 0);
		virtual ~ExpandableWidget();



		/**
		 * Expand the widget. This will ensure the proper tqparent child relations.
		 * @param w The widget
		 * @param pos It's position relative to the current widget
		 */
		void expand(TQWidget* w,Position pos);

		/**
		 * Remove a widget. This will ensure the proper tqparent child relations.
		 * The widget w will become parentless. Note the first child will never be removed.
		 * @param w The widget
		 */
		void remove(TQWidget* w);
	private:
		struct StackElement
		{
			TQWidget* w;
			TQSplitter* s;
			Position pos;
			StackElement* next;

			StackElement() : w(0),s(0),pos(LEFT),next(0) {}
			~StackElement() {delete next;}
		};

		StackElement* begin;
		TQHBoxLayout* top_layout;
	};

}

#endif

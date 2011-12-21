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
#include <tqlayout.h>
#include <tqsplitter.h>
#include "expandablewidget.h"

namespace kt
{

	ExpandableWidget::ExpandableWidget(TQWidget* child,TQWidget *parent, const char *name)
			: TQWidget(parent, name)
	{
		top_layout = new TQHBoxLayout(this);
		child->reparent(this,TQPoint(),true);
		// make top of stack
		begin = new StackElement;
		begin->w = child;
		top_layout->add(child);
	}


	ExpandableWidget::~ExpandableWidget()
	{
		if (begin)
		{
		//	delete begin->w;
			delete begin;
		}
	}

	void ExpandableWidget::expand(TQWidget* w,Position pos)
	{
		// create new element
		StackElement* se = new StackElement;
		se->w = w;
		se->pos = pos;
		se->next = begin;

		// remove old top from layout
		top_layout->remove(begin->w);
			
		// create new toplevel splitter
		Qt::Orientation orientation = (pos == RIGHT || pos == LEFT) ? Qt::Horizontal : Qt::Vertical;
		TQSplitter* s =  new TQSplitter(orientation,this);;
		se->s = s;
		
		// reparent w and the bottom widget to s
		w->reparent(s,TQPoint(),false);
		if (begin->s)
			begin->s->reparent(s,TQPoint(),false);
		else
			begin->w->reparent(s,TQPoint(),false);
		
		// add w and the bottom widget to s
		if (pos == RIGHT || pos == ABOVE)
		{
			s->moveToFirst(w);
			s->setResizeMode(w,TQSplitter::KeepSize);
			s->moveToLast(begin->s ? begin->s : begin->w);
		}
		else
		{
			s->moveToFirst(begin->s ? begin->s : begin->w);
			s->moveToLast(w);
			s->setResizeMode(w,TQSplitter::KeepSize);
		}
		// make se new top of stack
		begin = se;

		// add toplevel splitter to layout
		top_layout->add(s);
		
		// show s
		s->show();
	}

	void ExpandableWidget::remove(TQWidget* w)
	{
		// find the correct stackelement
		StackElement* se = begin;
		StackElement* prev = 0; // element before se
		while (se->w != w && se->next)
		{
			prev = se;
			se = se->next;
		}

		// do not remove last
		if (!se->next)
			return;

		if (!prev)
		{
			// we need to remove the first
			top_layout->remove(se->s);
			// reparent current top to 0
			se->w->reparent(0,TQPoint(),false);
			se->s->reparent(0,TQPoint(),false);
			// set new top
			begin = se->next;
			

			if (begin->s)
			{
				begin->s->reparent(this,TQPoint(),false);
				top_layout->add(begin->s);
				begin->s->show();
			}
			else
			{
				begin->w->reparent(this,TQPoint(),false);
				top_layout->add(begin->w);
				begin->w->show();
			}

			se->next = 0;
			// delete splitter and se
			delete se->s;
			delete se;
		}
		else
		{
			StackElement* next = se->next;
			// isolate the node
			se->next = 0;
			prev->next = next;
			
			// reparent se to 0
			se->s->reparent(0,TQPoint(),false);
			se->w->reparent(0,TQPoint(),false);
			
			// reparent se->next to prev
			if (next->s)
				next->s->reparent(prev->s,TQPoint(),false);
			else
				next->w->reparent(prev->s,TQPoint(),false);
			
			// update prev's splitter
			if (prev->pos == RIGHT || prev->pos == ABOVE)
			{
				prev->s->moveToFirst(prev->w);
				prev->s->setResizeMode(prev->w,TQSplitter::KeepSize);
				prev->s->moveToLast(next->s ? next->s : next->w);
				prev->s->setResizeMode(next->s ? next->s : next->w,TQSplitter::KeepSize);
			}
			else
			{
				prev->s->moveToFirst(next->s ? next->s : next->w);
				prev->s->setResizeMode(next->s ? next->s : next->w,TQSplitter::KeepSize);
				prev->s->moveToLast(prev->w);
				prev->s->setResizeMode(prev->w,TQSplitter::KeepSize);
			}

			// delete se and splitter
			delete se->s;
			delete se;
			prev->next->w->show();
			prev->s->show();
		}
	}
}
#include "expandablewidget.moc"

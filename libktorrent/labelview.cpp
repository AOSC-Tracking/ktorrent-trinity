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
#include <algorithm>
#include <tqlayout.h>
#include <tqlabel.h>
#include <kiconloader.h>
#include <tdeglobalsettings.h>
#include <util/log.h>
#include "labelview.h"

using namespace bt;

namespace kt
{
	LabelViewItem::LabelViewItem(const TQString & icon,const TQString & title,const TQString & description,LabelView* view)
		: LabelViewItemBase(view),odd(false),selected(false)
	{
		icon_lbl->setPixmap(DesktopIcon(icon));
		title_lbl->setText(title);
		description_lbl->setText(description);
		setOdd(false);
	}
	
	LabelViewItem::~LabelViewItem()
	{
	}
	
	void LabelViewItem::setTitle(const TQString & title)
	{
		title_lbl->setText(title);
	}
	
	void LabelViewItem::setDescription(const TQString & d)
	{
		description_lbl->setText(d);
	}
	
	void LabelViewItem::setIcon(const TQString & icon)
	{
		icon_lbl->setPixmap(DesktopIcon(icon));
	}
	
	void LabelViewItem::setOdd(bool o)
	{
		odd = o;
		setSelected(selected);
	}
	
	void LabelViewItem::setSelected(bool sel)
	{
		selected = sel;

		if (selected)
		{
			setPaletteBackgroundColor(TDEGlobalSettings::highlightColor());
			setPaletteForegroundColor(TDEGlobalSettings::highlightedTextColor());
		}
		else if (odd)
		{
			setPaletteBackgroundColor(TDEGlobalSettings::baseColor());
			setPaletteForegroundColor(TDEGlobalSettings::textColor());
		}
		else
		{
			setPaletteBackgroundColor(TDEGlobalSettings::alternateBackgroundColor());
			setPaletteForegroundColor(TDEGlobalSettings::textColor());
		}
	}
	
	bool LabelViewItem::operator < (const LabelViewItem & item)
	{
		return title_lbl->text() < item.title_lbl->text();
	}
	
	void LabelViewItem::mousePressEvent(TQMouseEvent *e)
	{
		if (e->button() == TQt::LeftButton)
		{
			clicked(this);
		}

		setFocus();
		TQWidget::mousePressEvent(e);
	}
	
	typedef std::list<LabelViewItem*>::iterator LabelViewItr;
	typedef std::list<LabelViewItem*>::const_iterator LabelViewCItr;
	
	class LabelViewBox : public TQWidget
	{
		TQVBoxLayout* layout;
	public:
		LabelViewBox(TQWidget* parent) : TQWidget(parent)
		{
			setPaletteBackgroundColor(TDEGlobalSettings::baseColor());
			layout = new TQVBoxLayout(this);
			layout->setMargin(0);
		}
		
		virtual ~LabelViewBox()
		{}
		
		void add(LabelViewItem* item)
		{
			item->reparent(this,TQPoint(0,0));
			layout->add(item);
			item->show();
		}
		
		void remove(LabelViewItem* item)
		{
			item->hide();
			layout->remove(item);
			item->reparent(0,TQPoint(0,0));
		}
		
		void sorted(const std::list<LabelViewItem*> items)
		{
			for (LabelViewCItr i = items.begin();i != items.end();i++)
				layout->remove(*i);
			
			for (LabelViewCItr i = items.begin();i != items.end();i++)
				layout->add(*i);
		}
	};
	

	
	///////////////////////////////////////

	LabelView::LabelView ( TQWidget *parent, const char *name )
			: TQScrollView ( parent, name ),selected(0)
	{
		item_box = new LabelViewBox(this->viewport());
		setResizePolicy(TQScrollView::AutoOneFit);
	
		addChild(item_box, 0, 0);
		item_box->show();
	}


	LabelView::~LabelView()
	{}

	void LabelView::addItem(LabelViewItem* item)
	{
		item_box->add(item);
		items.push_back(item);
		item->setOdd(items.size() % 2 == 1);
		
		connect(item, TQ_SIGNAL(clicked(LabelViewItem*)),
				this, TQ_SLOT(onItemClicked(LabelViewItem*)));
	}
	
	void LabelView::removeItem(LabelViewItem* item)
	{
		LabelViewItr i = std::find(items.begin(),items.end(),item);
		if (i != items.end())
		{
			item_box->remove(item);
			items.erase(i);
			disconnect(item, TQ_SIGNAL(clicked(LabelViewItem*)),
					this, TQ_SLOT(onItemClicked(LabelViewItem*)));
			
			// check for selected being equal to item
			if (item == selected)
				selected = 0;
			
			// update odd status of each item
			updateOddStatus();	
		}
	}
	
	void LabelView::updateOddStatus()
	{
		bool odd = true;
		LabelViewItr i = items.begin();
		while (i != items.end())
		{
			LabelViewItem* item = *i;
			item->setOdd(odd);
			odd = !odd;
			i++;
		}
	}
	
	void LabelView::onItemClicked(LabelViewItem* it)
	{
		if (selected == it)
			return;
		
		if (selected)
			selected->setSelected(false);
		
		selected = it;
		selected->setSelected(true);
		currentChanged(selected);
	}
	
	void LabelView::clear()
	{
		LabelViewItr i = items.begin();
		while (i != items.end())
		{
			LabelViewItem* item = *i;
			item_box->remove(item);
			i = items.erase(i);
			delete item;
		}
		selected = 0;
	}
	
	void LabelView::update()
	{
		LabelViewItr i = items.begin();
		while (i != items.end())
		{
			LabelViewItem* item = *i;
			item->update();
			i++;
		}
	}
	
	struct LabelViewItemCmp
	{
		bool operator() (LabelViewItem* a,LabelViewItem* b)
		{
			return *a < *b;
		}
	};
	
	void LabelView::sort()
	{
		items.sort(LabelViewItemCmp());
		item_box->sorted(items);
		updateOddStatus();
	}

}
#include "labelview.moc"

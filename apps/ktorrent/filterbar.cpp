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

#include "filterbar.h"

#include <tqlabel.h>
#include <tqlayout.h>
#include <tqcheckbox.h>

#include <kdialog.h>
#include <klocale.h>
#include <kpushbutton.h>
#include <ktoolbarbutton.h>
#include <klineedit.h>
#include <kiconloader.h>
#include <kconfig.h>
#include <interfaces/torrentinterface.h>



FilterBar::FilterBar(TQWidget *parent, const char *name) :
    TQWidget(parent, name)
{
	const int gap = 3;
	
	TQVBoxLayout* foo = new TQVBoxLayout(this);
	foo->addSpacing(gap);
	
	TQHBoxLayout* layout = new TQHBoxLayout(foo);
	layout->addSpacing(gap);
	
	m_close = new KToolBarButton("fileclose",0,this);
	connect(m_close,TQT_SIGNAL(clicked()),this,TQT_SLOT(hide()));
	layout->addWidget(m_close);
	
	m_filter = new TQLabel(i18n("Find:"), this);
	layout->addWidget(m_filter);
	layout->addSpacing(KDialog::spacingHint());
	
	m_filterInput = new KLineEdit(this);
	layout->addWidget(m_filterInput);
	
	m_clear = new KPushButton(this);
	m_clear->setIconSet(SmallIcon("clear_left"));
	m_clear->setFlat(true);
	layout->addWidget(m_clear);
	layout->addSpacing(gap);
	
	m_case_sensitive = new TQCheckBox(i18n("Case sensitive"),this);
	m_case_sensitive->setChecked(true);
	layout->addWidget(m_case_sensitive);
	layout->addItem(new TQSpacerItem(10,10,TQSizePolicy::Expanding));
		
	connect(m_filterInput, TQT_SIGNAL(textChanged(const TQString&)),
		this, TQT_SLOT(slotChangeFilter(const TQString&)));
	connect(m_clear, TQT_SIGNAL(clicked()), m_filterInput, TQT_SLOT(clear()));
}

FilterBar::~FilterBar()
{
}

void FilterBar::saveSettings(KConfig* cfg)
{
	cfg->writeEntry("filter_bar_hidden",isHidden());
	cfg->writeEntry("filter_bar_text",m_name_filter);
	cfg->writeEntry("filter_bar_case_sensitive",m_case_sensitive->isChecked());
}
	
void FilterBar::loadSettings(KConfig* cfg)
{
	setHidden(cfg->readBoolEntry("filter_bar_hidden",true));
	m_case_sensitive->setChecked(cfg->readBoolEntry("filter_bar_case_sensitive",true));
	m_name_filter = cfg->readEntry("filter_bar_text",TQString());
	m_filterInput->setText(m_name_filter);
}

bool FilterBar::matchesFilter(kt::TorrentInterface* tc)
{
	bool cs = m_case_sensitive->isChecked();
	if (m_name_filter.length() == 0)
		 return true;
	else
		return tc->getStats().torrent_name.contains(m_name_filter,cs);
}

void FilterBar::slotChangeFilter(const TQString& nameFilter)
{	
	m_name_filter = nameFilter;
}

void FilterBar::keyPressEvent(TQKeyEvent* event)
{
    if ((event->key() == TQt::Key_Escape))
	{
        m_filterInput->clear();
		m_name_filter = TQString();
		//hide();
    }
	else 
		TQWidget::keyPressEvent(event);
}

void FilterBar::hideEvent(TQHideEvent* event)
{
	m_filterInput->releaseKeyboard();
	TQWidget::hideEvent(event);
}

#include "filterbar.moc"

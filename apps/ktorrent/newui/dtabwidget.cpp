/***************************************************************************
 *   Copyright (C) 2005 by Alexander Dymo                                  *
 *   adymo@tdevelop.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "dtabwidget.h"

#include <tqtoolbutton.h>
#include <tqtabbar.h>

#include <kconfig.h>
#include <kiconloader.h>
#include <kapplication.h>

DTabWidget::DTabWidget(TQWidget *parent, const char *name)
    :KTabWidget(parent, name), m_closeButton(0)
{
    setFocusPolicy(TQ_NoFocus);
    setMargin(0);

    loadSettings();
    
    if (!m_tabBarShown)
        tabBar()->hide();
    else {
        m_closeButton = new TQToolButton(this);
        m_closeButton->setIconSet(SmallIconSet("tab_remove"));
        m_closeButton->adjustSize();
        m_closeButton->hide();
        setCornerWidget(m_closeButton, TopRight);
        
        if (m_closeOnHover)
            setHoverCloseButton(true);

        setTabReorderingEnabled(true);
    }
    
    connect(this, TQT_SIGNAL(currentChanged(TQWidget*)), this, TQT_SLOT(setFocus(TQWidget*)));
//    connect(this, TQT_SIGNAL(currentChanged(TQWidget*)), this, TQT_SLOT(updateHistory(TQWidget*)));
}

void DTabWidget::loadSettings()
{
	/*
    KConfig *config = kapp->config();
    config->setGroup("UI");
//    m_tabBarShown = config->readBoolEntry("TabBarShown", true);
    m_tabBarShown = ! config->readNumEntry("TabWidgetVisibility", 0);
    m_closeOnHover = config->readBoolEntry("CloseOnHover", false);
    m_closeButtonShown = config->readBoolEntry("ShowCloseTabsButton", true);
    //we do not delay hover close buttons - that looks and feels ugly
    setHoverCloseButtonDelayed(false);
	*/
	m_tabBarShown = true;
	m_closeOnHover = false;
	m_closeButtonShown = true;
}

void DTabWidget::saveSettings()
{
}

TQToolButton *DTabWidget::closeButton() const
{
    return m_closeButton;
}

void DTabWidget::setFocus(TQWidget *w)
{
    if (w)
        w->setFocus();
}

void DTabWidget::insertTab(TQWidget *child, const TQString &label, int index)
{
    if (m_closeButton && m_closeButtonShown)
        m_closeButton->show();
    KTabWidget::insertTab(child, label, index);
    if (index != -1) tabBar()->tqrepaint();
}

void DTabWidget::insertTab(TQWidget *child, const TQIconSet &iconset, 
    const TQString &label, int index)
{
    if (m_closeButton && m_closeButtonShown)
        m_closeButton->show();
    KTabWidget::insertTab(child, iconset, label, index);
    if (index != -1) tabBar()->tqrepaint();
}

/*void DTabWidget::updateHistory(TQWidget *w)
{
    if (m_history.top() != w)
        m_history.push(w);
}*/

#include "dtabwidget.moc"

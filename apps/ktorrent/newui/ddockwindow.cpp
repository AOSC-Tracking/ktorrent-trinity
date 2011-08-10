/***************************************************************************
 *   Copyright (C) 2005 by Alexander Dymo                                  *
 *   adymo@kdevelop.org                                                    *
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
#include "ddockwindow.h"

#include <tqtoolbutton.h>
#include <tqlayout.h>
#include <tqstyle.h>
#include <tqwidgetstack.h>
#include <tqimage.h>
#include <tqapplication.h>
#include <tqpopupmenu.h>

#include <kdebug.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kcombobox.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kapplication.h>

#include "buttonbar.h"
#include "button.h"
#include "dmainwindow.h"

DDockWindow::DDockWindow(DMainWindow *tqparent, Position position)
    :TQDockWindow(TQDockWindow::InDock, tqparent), m_position(position), m_visible(false),
    m_mainWindow(tqparent), m_doNotCloseActiveWidget(false), m_toggledButton(0), m_lastContextMenuButton(0)
{
    setMovingEnabled(false);
    setResizeEnabled(true);

    Ideal::Place place = Ideal::Left;
    switch (position) {
        case DDockWindow::Bottom:
            m_name = "BottomToolWindow";
            place = Ideal::Bottom;
            m_internalLayout = new TQVBoxLayout(boxLayout(), 0);
            m_internalLayout->setDirection(TQBoxLayout::BottomToTop);
            break;
        case DDockWindow::Left:
            m_name = "LeftToolWindow";
            place = Ideal::Left;
            m_internalLayout = new TQHBoxLayout(boxLayout(), 0);
            m_internalLayout->setDirection(TQBoxLayout::LeftToRight);
            break;
        case DDockWindow::Right:
            m_name = "RightToolWindow";
            place = Ideal::Right;
            m_internalLayout = new TQHBoxLayout(boxLayout(), 0);
            m_internalLayout->setDirection(TQBoxLayout::RightToLeft);
            break;
    }

    KConfig *config = kapp->config();
    config->setGroup("UI");
    int mode = config->readNumEntry("MDIStyle", 3);
    Ideal::ButtonMode buttonMode = Ideal::Text;
    if (mode == 0)
        buttonMode = Ideal::Icons;
    else if (mode == 1)
        buttonMode = Ideal::Text;
    else if (mode == 3)
        buttonMode = Ideal::IconsAndText;

    m_bar = new Ideal::ButtonBar(place, buttonMode, this);
    m_internalLayout->addWidget(m_bar);

    m_widgetStack = new TQWidgetStack(this);
    m_internalLayout->addWidget(m_widgetStack);

    m_moveToDockLeft = new KAction( i18n("Move to left dock"), 0, TQT_TQOBJECT(this), TQT_SLOT(moveToDockLeft()), TQT_TQOBJECT(this) );
    m_moveToDockRight = new KAction( i18n("Move to right dock"), 0, TQT_TQOBJECT(this), TQT_SLOT(moveToDockRight()), TQT_TQOBJECT(this) );
    m_moveToDockBottom = new KAction( i18n("Move to bottom dock"), 0, TQT_TQOBJECT(this), TQT_SLOT(moveToDockBottom()), TQT_TQOBJECT(this) );

    setVisible(m_visible);

    loadSettings();
}

DDockWindow::~DDockWindow()
{
//done in DMainWindow now
//    saveSettings();
}

void DDockWindow::setVisible(bool v)
{
    //write dock width to the config file
    KConfig *config = kapp->config();
    TQString group = TQString("%1").tqarg(m_name);
    config->setGroup(group);

    if (m_visible)
        config->writeEntry("ViewWidth", m_position == DDockWindow::Bottom ? height() : width() );
    setResizeEnabled(v);
    setVerticallyStretchable(true);
    setHorizontallyStretchable(true);
    v ? m_widgetStack->show() : m_widgetStack->hide();
    m_visible = v;

    m_internalLayout->tqinvalidate();
    if (!m_visible)
    {
        if (m_position == DDockWindow::Bottom)
            setFixedExtentHeight(m_internalLayout->tqsizeHint().height());
        else
            setFixedExtentWidth(m_internalLayout->tqsizeHint().width());
        emit hidden();
    }
    else
    {
        //restore widget size from the config
        int size = 0;
        if (m_position == DDockWindow::Bottom)
        {
            size = config->readNumEntry("ViewWidth", m_internalLayout->tqminimumSize().height());
            setFixedExtentHeight(size);
        }
        else
        {
            size = config->readNumEntry("ViewWidth", m_internalLayout->tqminimumSize().width());
            setFixedExtentWidth(size);
        }
    }
}

void DDockWindow::loadSettings()
{
}

void DDockWindow::saveSettings()
{
    KConfig *config = kapp->config();
    TQString group = TQString("%1").tqarg(m_name);
    int invisibleWidth = 0;
    config->setGroup(group);
    if (config->hasKey("ViewWidth"))
        invisibleWidth = config->readNumEntry("ViewWidth");
    config->deleteEntry("ViewWidth");
    config->deleteEntry("ViewLastWidget");
    if (m_toggledButton && m_visible)
    {
        config->writeEntry("ViewWidth", m_position == DDockWindow::Bottom ? height() : width());
        config->writeEntry("ViewLastWidget", m_toggledButton->realTextWithoutAccel());
    }
    else if (invisibleWidth != 0)
        config->writeEntry("ViewWidth", invisibleWidth);
}

TQWidget *DDockWindow::currentWidget() const
{
    return m_widgetStack->visibleWidget();
}

void DDockWindow::addWidget(const TQString &title, TQWidget *widget, bool skipActivation)
{
    kdDebug(9000) << k_funcinfo << endl;
    TQPixmap *pm = const_cast<TQPixmap*>(widget->icon());
    Ideal::Button *button;
    if (pm != 0)
    {
        //force 16pt for now
        if (pm->height() > 16)
        {
            TQImage img = pm->convertToImage();
            img = img.smoothScale(16, 16);
            pm->convertFromImage(img);
        }
        button = new Ideal::Button(m_bar, title, *pm);
    }
    else
        button = new Ideal::Button(m_bar, title);
    m_widgets[button] = widget;
    m_buttons[widget] = button;
    m_bar->addButton(button);

    m_widgetStack->addWidget(widget);
    connect(button, TQT_SIGNAL(clicked()), TQT_TQOBJECT(this), TQT_SLOT(selectWidget()));
    connect(button, TQT_SIGNAL(contextMenu(TQPopupMenu*)), TQT_TQOBJECT(this), TQT_SLOT(contextMenu(TQPopupMenu*)) );

    if (!skipActivation)
    {
        //if the widget was selected last time the dock is deleted
        //we need to show it
        KConfig *config = kapp->config();
        TQString group = TQString("%1").tqarg(m_name);
        config->setGroup(group);
        if (config->readEntry("ViewLastWidget") == title)
        {
            kdDebug(9000) << k_funcinfo << " : activating last widget " << title << endl;
            button->setOn(true);
            selectWidget(button);
        }
    }
}

void DDockWindow::raiseWidget(TQWidget *widget)
{
    kdDebug(9000) << k_funcinfo << endl;

    if ( !widget ) return;

    Ideal::Button *button = m_buttons[widget];
    if ((button != 0) && (!button->isOn()))
    {
        button->setOn(true);
        selectWidget(button);
    }
}

void DDockWindow::lowerWidget(TQWidget * widget)
{
    kdDebug(9000) << k_funcinfo << endl;

    if ( !widget ) return;

    Ideal::Button *button = m_buttons[widget];
    if ((button != 0) && (button->isOn()))
    {
        button->setOn(false);
        selectWidget(button);
    }
}

void DDockWindow::removeWidget(TQWidget *widget)
{
    kdDebug(9000) << k_funcinfo << endl;
    if (m_widgetStack->id(widget) == -1)
        return; //not in dock

    bool changeVisibility = false;
    if (m_widgetStack->visibleWidget() == widget)
        changeVisibility = true;

    Ideal::Button *button = m_buttons[widget];
    if (button)
        m_bar->removeButton(button);
    m_widgets.remove(button);
    m_buttons.remove(widget);
    m_widgetStack->removeWidget(widget);

    if (changeVisibility)
    {
        m_toggledButton = 0;
        setVisible(false);
    }
}

void DDockWindow::selectWidget(Ideal::Button *button)
{
    bool special = m_doNotCloseActiveWidget;
    m_doNotCloseActiveWidget = false;
    kdDebug(9000) << k_funcinfo << endl;
    if (m_toggledButton == button)
    {
        if (special && m_visible && (!isActive()))
        {
            //special processing for keyboard navigation events
            m_toggledButton->setOn(true);
            m_widgets[button]->setFocus();
        }
        else
        {
            m_widgets[button]->setFocus();
            setVisible(!m_visible);
        }
        return;
    }

    if (m_toggledButton)
        m_toggledButton->setOn(false);
    m_toggledButton = button;
    setVisible(true);
    m_widgetStack->raiseWidget(m_widgets[button]);
    m_widgets[button]->setFocus();
}

void DDockWindow::selectWidget()
{
    selectWidget((Ideal::Button*)TQT_TQOBJECT(const_cast<TQT_BASE_OBJECT_NAME*>(sender())));
}

void DDockWindow::hideWidget(TQWidget *widget)
{
    Ideal::Button *button = m_buttons[widget];
    if (button != 0)
    {
        button->setOn(false);
        button->hide();
    }
    widget->hide();
    if (button == m_toggledButton)
        setVisible(false);
}

void DDockWindow::showWidget(TQWidget *widget)
{
    Ideal::Button *button = m_buttons[widget];
    if (button != 0)
        button->show();
//     widget->show();
}

void DDockWindow::setMovingEnabled(bool)
{
    //some operations on KMainWindow cause moving to be enabled
    //but we always don't want DDockWindow instances to be movable
    TQDockWindow::setMovingEnabled(false);
}

void DDockWindow::selectLastWidget()
{
    m_doNotCloseActiveWidget = true;
    if (m_toggledButton)
        m_toggledButton->animateClick();
    else if (Ideal::Button *button = m_bar->firstButton())
        button->animateClick();
}

bool DDockWindow::isActive()
{
    if (m_toggledButton)
    {
        TQWidget *w = tqApp->tqfocusWidget();
        if (!w)
            return false;
        TQWidget *toolWidget = m_widgets[m_toggledButton];
        if (toolWidget == w)
            return true;
        else
        {
            do {
                w = (TQWidget*)w->tqparent();
                if (w && (w == toolWidget)) return true;
            } while (w);
        }
    }
    return false;
}

void DDockWindow::selectNextWidget()
{
    if (!m_toggledButton)
        return;
    Ideal::Button *b = m_bar->nextTo(m_toggledButton);
    if (b)
        b->animateClick();
}

void DDockWindow::selectPrevWidget()
{
    if (!m_toggledButton)
        return;
    Ideal::Button *b = m_bar->prevTo(m_toggledButton);
    if (b)
        b->animateClick();
}

void DDockWindow::contextMenu(TQPopupMenu * menu)
{
    m_lastContextMenuButton = static_cast<Ideal::Button*>( TQT_TQWIDGET(const_cast<TQT_BASE_OBJECT_NAME*>(sender())) );

    menu->insertSeparator();

    if ( position() != DDockWindow::Left )
        m_moveToDockLeft->plug( menu );
    if ( position()!= DDockWindow::Right )
        m_moveToDockRight->plug( menu ); 
    if ( position() != DDockWindow::Bottom )
        m_moveToDockBottom->plug( menu );
}

void DDockWindow::moveToDockLeft()
{
    moveToDock( DDockWindow::Left );
}

void DDockWindow::moveToDockRight()
{
    moveToDock( DDockWindow::Right );
}

void DDockWindow::moveToDockBottom()
{
    moveToDock( DDockWindow::Bottom );
}

void DDockWindow::moveToDock(DDockWindow::Position position )
{
    if (  m_widgets.contains( m_lastContextMenuButton ) )
    {
        mainWindow()->moveWidget( position, m_widgets[ m_lastContextMenuButton ], m_lastContextMenuButton->realTextWithoutAccel() );
    }
}

bool DDockWindow::hasWidgets() const
{
	return m_widgets.count() > 0;
}

#include "ddockwindow.moc"

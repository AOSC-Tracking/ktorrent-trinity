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
#ifndef DDOCKWINDOW_H
#define DDOCKWINDOW_H

#include <tqdockwindow.h>
#include <tqvaluelist.h>

class TQBoxLayout;
class TQToolButton;
class TQWidgetStack;
class TQPopupMenu;

class KComboBox;
class KAction;

class DMainWindow;

namespace Ideal {
    class Button;
    class ButtonBar;
}

class DDockWindow : public TQDockWindow {
    Q_OBJECT
  TQ_OBJECT
public:
    enum Position { Bottom, Left, Right };

    DDockWindow(DMainWindow *parent, Position position);
    virtual ~DDockWindow();

    virtual void setVisible(bool v);
    bool visible() const { return m_visible; }
    Position position() const { return m_position; }

    virtual void addWidget(const TQString &title, TQWidget *widget, bool skipActivation = false);
    virtual void raiseWidget(TQWidget *widget);
    virtual void lowerWidget(TQWidget *widget);
    /**Removes the widget from dock. Does not delete it.*/
    virtual void removeWidget(TQWidget *widget);

    virtual void hideWidget(TQWidget *widget);
    virtual void showWidget(TQWidget *widget);

    virtual TQWidget *currentWidget() const;

    virtual void setMovingEnabled(bool b);

    virtual void saveSettings();

    DMainWindow *mainWindow() const { return m_mainWindow; }

    virtual void selectLastWidget();
    virtual void selectNextWidget();
    virtual void selectPrevWidget();

    bool isActive();
	/// Check if this dock has any widgets
	bool hasWidgets() const; 

signals:
    void hidden();

private slots:
    void selectWidget();
    void selectWidget(Ideal::Button *button);
    void contextMenu(TQPopupMenu*);
    void moveToDockLeft();
    void moveToDockRight();
    void moveToDockBottom();
    void moveToDock(DDockWindow::Position);

protected:
    virtual void loadSettings();

    Ideal::ButtonBar *m_bar;
    TQWidgetStack *m_widgetStack;

    TQMap<Ideal::Button*, TQWidget*> m_widgets;
    TQMap<TQWidget*, Ideal::Button*> m_buttons;

private:
    Position m_position;
    bool m_visible;
    TQString m_name;
    DMainWindow *m_mainWindow;
    bool m_doNotCloseActiveWidget;

    Ideal::Button *m_toggledButton;
    Ideal::Button *m_lastContextMenuButton;
    TQBoxLayout *m_internalLayout;


    KAction * m_moveToDockLeft;
    KAction * m_moveToDockRight;
    KAction * m_moveToDockBottom;
};

#endif

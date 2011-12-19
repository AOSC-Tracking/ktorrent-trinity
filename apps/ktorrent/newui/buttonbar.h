/***************************************************************************
 *   Copyright (C) 2004 by Alexander Dymo                                  *
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
#ifndef IDEALBUTTONBAR_H
#define IDEALBUTTONBAR_H

#include <tqwidget.h>
#include <tqvaluelist.h>

#include "comdefs.h"

#include <tqlayout.h>

namespace Ideal {

class Button;
class ButtonBar;

/**@short A tqlayout for a ButtonBar class.

Overrides minimumSize method to allow shrinking button bar buttons.*/
class ButtonLayout: public TQBoxLayout{
public:
    ButtonLayout(ButtonBar *parent, Direction d, int margin = 0, int spacing = -1, const char * name = 0);

    virtual TQSize minimumSize() const;

private:
    ButtonBar *m_buttonBar;
};

/**
@short A bar with tool buttons.

Looks like a toolbar but has another behaviour. It is suitable for
placing on the left(right, bottom, top) corners of a window as a bar with slider.
*/
class ButtonBar : public TQWidget {
    Q_OBJECT
  TQ_OBJECT
public:
    ButtonBar(Place place, ButtonMode mode = IconsAndText,
        TQWidget *parent = 0, const char *name = 0);
    virtual ~ButtonBar();

    /**Adds a button to the bar.*/
    virtual void addButton(Button *button);
    /**Removes a button from the bar and deletes the button.*/
    virtual void removeButton(Button *button);

    /**Sets the mode.*/
    void setMode(ButtonMode mode);
    /**@returns the mode.*/
    ButtonMode mode() const;

    /**@returns the place.*/
    Place place() const;

    bool autoResize() const;
    void setAutoResize(bool b);

    /**Shrinks the button bar so all buttons are visible. Shrinking is done by
    reducing the amount of text shown on buttons. Button icon size is a minimum size
    of a button. If a button does not have an icon, it displays "...".*/
    virtual void shrink(int preferredDimension, int actualDimension);
    virtual void deshrink(int preferredDimension, int actualDimension);
    /**Restores the size of button bar buttons.*/
    virtual void unshrink();

    Button *firstButton();
    Button *nextTo(Button *button);
    Button *prevTo(Button *button);

protected:
    virtual void resizeEvent ( TQResizeEvent *ev );

    int originalDimension();

private:
    void fixDimensions();
    void setButtonsPlace(Ideal::Place place);

    typedef TQValueList<Button*> ButtonList;
    ButtonList m_buttons;

    ButtonMode m_mode;
    Place m_place;

    ButtonLayout *l;

    bool m_shrinked;
    bool m_autoResize;
};

}

#endif

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
#ifndef IDEALBUTTON_H
#define IDEALBUTTON_H

#include <tqpushbutton.h>
#include <tqiconset.h>

#include "comdefs.h"

class KAction;

namespace Ideal {

class ButtonBar;

/**
@short A button to place onto the ButtonBar

A TQPushButton derivative with a size of a TQToolBar. Button can be rotated
(placed onto different places in ideal mode).
*/
class Button : public TQPushButton {
    Q_OBJECT
  TQ_OBJECT
public:
    Button(ButtonBar *parent, const TQString text, const TQIconSet &icon = TQIconSet(),
        const TQString &description = TQString());

    /**Sets the description used as a tooltip.*/
    void setDescription(const TQString &description);
    /**Returns the description.*/
    TQString description() const;

    /**Sets the place of a button.*/
    void setPlace(Ideal::Place place);
    /**Sets the mode of a button.*/
    void setMode(Ideal::ButtonMode mode);

    TQSize tqsizeHint() const;
    TQSize tqsizeHint(const TQString &text) const;

    /**Updates size of a widget. Used after squeezing button's text.*/
    void updateSize();

    /**Returns the real (i.e. not squeezed) text of a button.*/
    TQString realText() const;
    TQString realTextWithoutAccel() const;
    void setRealText(const TQString &text);

protected:
    ButtonMode mode();

    virtual void drawButton(TQPainter *p);
    virtual void drawButtonLabel(TQPainter *p);

    virtual void contextMenuEvent(TQContextMenuEvent *e);

protected slots:
    void assignAccel();
    void clearAccel();

signals:
    void contextMenu(TQPopupMenu*);

private:
    virtual ~Button();

    void fixDimensions(Place oldPlace);

    void enableIconSet();
    void disableIconSet();
    void enableText();
    void disableText();

    ButtonBar *m_buttonBar;

    TQString m_description;
    Place m_place;

    TQString m_realText;
    TQIconSet m_realIconSet;

    KAction *m_assignAccelAction;
    KAction *m_clearAccelAction;

friend class ButtonBar;
};

}

#endif

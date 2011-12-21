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
#include "button.h"

#include <tqpainter.h>
#include <tqtooltip.h>
#include <tqstyle.h>
#include <tqapplication.h>
#include <tqregexp.h>

#include <kdebug.h>
#include <kiconloader.h>
#include <kxmlguiclient.h>
#include <kaction.h>
#include <kpopupmenu.h>
#include <kinputdialog.h>
#include <klocale.h>
#include <kapplication.h>
#include <kconfig.h>

#include "buttonbar.h"

namespace Ideal {

Button::Button(ButtonBar *parent, const TQString text, const TQIconSet &icon,
    const TQString &description)
    :TQPushButton(icon, text, parent), m_buttonBar(parent), m_description(description),
    m_place(parent->place()), m_realText(text), m_realIconSet(icon)
{
    hide();
    setFlat(true);
    setToggleButton(true);
    setFocusPolicy(TQ_NoFocus);
    setDescription(m_description);
    setSizePolicy(TQSizePolicy::Minimum, TQSizePolicy::Minimum);
    resize(sizeHint());
    fixDimensions(Ideal::Bottom);

    TQToolTip::add(this, m_realText);

    m_assignAccelAction = new KAction(i18n("Assign Accelerator..."), 0,
        TQT_TQOBJECT(this), TQT_SLOT(assignAccel()), TQT_TQOBJECT(this));
    m_clearAccelAction = new KAction(i18n("Clear Accelerator"), 0,
        TQT_TQOBJECT(this), TQT_SLOT(clearAccel()), TQT_TQOBJECT(this));

    KConfig *config = kapp->config();
    config->setGroup("UI");
    TQString accel = config->readEntry(TQString("button_%1").arg(text), "");
    if (!accel.isEmpty())
        setRealText(TQString("&%1 %2").arg(accel).arg(m_realText));
}

Button::~Button()
{
//     m_buttonBar->removeButton(this);
    KConfig *config = kapp->config();
    config->setGroup("UI");

    TQRegExp r("^&([0-9])\\s.*");
    TQRegExp r2("^&[0-9]\\s");
    if (r.search(m_realText) > -1)
    {
        TQString text = m_realText;
        if (text.contains(r2))
            text.remove(r2);
        config->writeEntry(TQString("button_%1").arg(text), r.cap(1));
    }
    else
    {
        config->writeEntry(TQString("button_%1").arg(m_realText), "");
    }
}

void Button::setDescription(const TQString &description)
{
    m_description = description;
    TQToolTip::remove(this);
    TQToolTip::add(this, m_description);
}

TQString Button::description() const
{
    return m_description;
}

void Button::drawButton(TQPainter *p)
{
    TQRect r = rect();
    TQSize sh = r.size();
    switch (m_place)
    {
        case Ideal::Left:
        case Ideal::Right:
            sh.setHeight(r.width());
            sh.setWidth(r.height());
            break;
    }

    TQStyle::SFlags flags = TQStyle::Style_Default;
    if (isEnabled())
        flags |= TQStyle::Style_Enabled;
    if (hasFocus())
        flags |= TQStyle::Style_HasFocus;
    if (isDown())
        flags |= TQStyle::Style_Down;
    if (isOn())
        flags |= TQStyle::Style_On;
    if (! isFlat() && ! isDown())
        flags |= TQStyle::Style_Raised;
    if (isDefault())
        flags |= TQStyle::Style_ButtonDefault;

    TQPixmap pm(sh.width(), sh.height());
    pm.fill(eraseColor());
    TQPainter p2(&pm);

    tqstyle().drawControl(TQStyle::CE_PushButton,&p2,this, TQRect(0,0,pm.width(),pm.height()), colorGroup(),flags);

    style().drawControl(TQStyle::CE_PushButtonLabel, &p2, this,
                        TQRect(0,0,pm.width(),pm.height()),
                        colorGroup(), flags, TQStyleOption());

    switch (m_place)
    {
        case Ideal::Left:
                p->rotate(-90);
                p->drawPixmap(1-pm.width(), 0, pm);
                break;
        case Ideal::Right:
                p->rotate(90);
                p->drawPixmap(0, 1-pm.height(), pm);
                break;
        default:
                p->drawPixmap(0, 0, pm);
                break;
    }
}

void Button::drawButtonLabel(TQPainter */*p*/)
{
}

ButtonMode Button::mode()
{
    return m_buttonBar->mode();
}

void Button::setPlace(Ideal::Place place)
{
    Place oldPlace = m_place;
    m_place = place;
    fixDimensions(oldPlace);
}

void Button::fixDimensions(Place oldPlace)
{
    switch (m_place)
    {
        case Ideal::Left:
        case Ideal::Right:
            if ((oldPlace == Ideal::Bottom) || (oldPlace == Ideal::Top))
            {
                setFixedWidth(height());
                setMinimumHeight(sizeHint().width());
                setMaximumHeight(32767);
            }
            break;
        case Ideal::Top:
        case Ideal::Bottom:
            if ((oldPlace == Ideal::Left) || (oldPlace == Ideal::Right))
            {
                setFixedHeight(width());
                setMinimumWidth(sizeHint().height());
                setMaximumWidth(32767);
            }
            break;
    }
}

TQSize Button::sizeHint() const
{
    return sizeHint(text());
}

TQSize Button::sizeHint(const TQString &text) const
{
    constPolish();
    int w = 0, h = 0;

    if ( iconSet() && !iconSet()->isNull() && (m_buttonBar->mode() != Text) ) {
        int iw = iconSet()->pixmap( TQIconSet::Small, TQIconSet::Normal ).width() + 4;
        int ih = iconSet()->pixmap( TQIconSet::Small, TQIconSet::Normal ).height();
        w += iw;
        h = TQMAX( h, ih );
    }
    if ( isMenuButton() )
        w += style().pixelMetric(TQStyle::PM_MenuButtonIndicator, this);
    if ( pixmap() ) {
        TQPixmap *pm = (TQPixmap *)pixmap();
        w += pm->width();
        h += pm->height();
    } else if (m_buttonBar->mode() != Icons) {
        TQString s( text );
        bool empty = s.isEmpty();
        if ( empty )
            s = TQString::fromLatin1("XXXX");
        TQFontMetrics fm = fontMetrics();
        TQSize sz = fm.size( ShowPrefix, s );
        if(!empty || !w)
            w += sz.width();
        if(!empty || !h)
            h = TQMAX(h, sz.height());
    }

    return (style().tqsizeFromContents(TQStyle::CT_ToolButton, this, TQSize(w, h)).
            expandedTo(TQApplication::globalStrut()));
}

void Button::updateSize()
{
    switch (m_place)
    {
        case Ideal::Left:
        case Ideal::Right:
             setMinimumHeight(sizeHint().width());
             resize(sizeHint().height(), sizeHint().width());
            break;
        case Ideal::Top:
        case Ideal::Bottom:
            resize(sizeHint().width(), sizeHint().height());
            break;
    }
}

TQString Button::realText() const
{
    return m_realText;
}

void Button::setMode(Ideal::ButtonMode mode)
{
    switch (mode)
    {
        case Text:
            disableIconSet();
            enableText();
            break;
        case IconsAndText:
            enableIconSet();
            enableText();
            break;
        case Icons:
            disableText();
            enableIconSet();
            break;
    }
}

void Button::enableIconSet()
{
    if (!iconSet())
    {
        if (m_realIconSet.isNull())
            m_realIconSet = SmallIcon("file_new");
        setIconSet(m_realIconSet);
    }
}

void Button::disableIconSet()
{
    setIconSet(TQIconSet());
}

void Button::disableText()
{
    if (text().length() > 0)
        setText("");
}

void Button::enableText()
{
    setText(m_realText);
}

void Button::contextMenuEvent(TQContextMenuEvent *e)
{
/*    TQPopupMenu menu;

    m_assignAccelAction->plug(&menu);
    if (m_realText.contains(TQRegExp("^&[0-9]\\s")))
        m_clearAccelAction->plug(&menu);

    emit contextMenu( &menu );

    menu.exec(e->globalPos());
	*/
}

void Button::assignAccel()
{
    bool ok;
    int num = KInputDialog::getInteger(i18n("Change Button Number"), i18n("New accelerator number:"), 1, 0, 10, 1, &ok, this);
    if (ok)
    {
        TQString text = realTextWithoutAccel();
        text = TQString("&%1 %2").arg(num).arg(text);
        setRealText(text);
    }
}

void Button::setRealText(const TQString &text)
{
    m_realText = text;
    setText(text);
    updateSize();
}

void Button::clearAccel()
{
    setRealText(realTextWithoutAccel());
}

TQString Button::realTextWithoutAccel() const
{
    TQString text = m_realText;
    TQRegExp r("^&[0-9]\\s");
    if (text.contains(r))
        text.remove(r);
    return text;
}

}

#include "button.moc"

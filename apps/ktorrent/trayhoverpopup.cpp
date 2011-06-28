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
#include <tqvbox.h>
#include <tqhbox.h>
#include <tqlabel.h>
#include <tqtooltip.h>
#include <tqpixmap.h>
#include <kdialog.h>
#include <util/log.h>
#include "trayhoverpopup.h"

using namespace bt;

TrayHoverPopup::TrayHoverPopup(const TQPixmap & pix,TQWidget *tqparent, const char *name )
	: KPassivePopup(KPassivePopup::Boxed,tqparent,name),pix(pix)
{
	setTimeout(0);
	setAutoDelete(false);
	connect(&hover_timer,TQT_SIGNAL(timeout()),this,TQT_SLOT(onHoverTimeout()));
	connect(&show_timer,TQT_SIGNAL(timeout()),this,TQT_SLOT(onShowTimeout()));
	create();
	setPalette(TQToolTip::palette());
	setLineWidth(1);
	context_menu_shown = false;
	cursor_over_icon = false;
}


TrayHoverPopup::~TrayHoverPopup()
{}

void TrayHoverPopup::contextMenuAboutToShow()
{
	context_menu_shown = true;
	if (isShown())
	{
		hide();
		hover_timer.stop();
	}
}

void TrayHoverPopup::contextMenuAboutToHide()
{
	context_menu_shown = false;
}
			

void TrayHoverPopup::enterEvent()
{
	cursor_over_icon = true;
	if (isHidden() && !context_menu_shown)
	{
		// start the show timer
		show_timer.start(1000,true);
	}
	else
		hover_timer.stop(); // stop timeout
}

void TrayHoverPopup::leaveEvent()
{
	cursor_over_icon = false;
	// to avoid problems with a quick succession of enter and leave events, because the cursor
	// is on the edge, use a timer to expire the popup
	// in enterEvent we will stop the timer
	if (isShown())
		hover_timer.start(200,true);
}

void TrayHoverPopup::onHoverTimeout()
{
	hide();
	show_timer.stop();
}

void TrayHoverPopup::onShowTimeout()
{
	if (!context_menu_shown && cursor_over_icon)
		show();
}

void TrayHoverPopup::updateText(const TQString & msg)
{
	text->setText(msg);
}

void TrayHoverPopup::create()
{
	TQVBox *vb = new TQVBox(this);
	vb->setSpacing(KDialog::spacingHint());
	 
	TQHBox *hb=0;
	if (!pix.isNull()) 
	{
		hb = new TQHBox(vb);
		hb->setMargin(0);
		hb->setSpacing(KDialog::spacingHint());
		TQLabel* pix_lbl = new TQLabel(hb,"title_icon");
		pix_lbl->setPixmap(pix);
		pix_lbl->tqsetAlignment(AlignLeft);
	}
	 
	
	TQLabel* title = new TQLabel("KTorrent", hb ? hb : vb, "title_label" );
	TQFont fnt = title->font();
	fnt.setBold( true );
	title->setFont( fnt );
	title->tqsetAlignment( TQt::AlignHCenter );
	if ( hb )
		hb->setStretchFactor(title, 10 ); // enforce centering

	// text will be filled later
	text = new TQLabel( "Dummy", vb, "msg_label" );
				text->tqsetAlignment( AlignLeft );
	setView(vb);
}
	

#include "trayhoverpopup.moc"

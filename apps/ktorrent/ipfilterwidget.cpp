/***************************************************************************
 *   Copyright (C) 2006 by Ivan Vasić   								   *
 *   ivasic@gmail.com   												   *
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
#include "ipfilterwidget.h"

#include <torrent/ipblocklist.h>
#include <torrent/globals.h>
#include <util/log.h>
#include <util/constants.h>

#include <tqstring.h>
#include <tqstringlist.h>
#include <tqregexp.h>
#include <tqvalidator.h>

#include <klistview.h>
#include <klineedit.h>
#include <ksocketaddress.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kmessagebox.h>

#define MAX_RANGES 500

using namespace bt;

IPFilterWidget::IPFilterWidget(TQWidget *parent, const char *name)
		:BlacklistWidgetBase(parent, name)
{
	IPBlocklist& ipfilter = IPBlocklist::instance();
	TQStringList* blocklist = ipfilter.getBlocklist();
	
	for (TQStringList::Iterator it = blocklist->begin(); it != blocklist->end(); ++it)
	{
		new KListViewItem(lstPeers, *it);
	}
	
	delete blocklist;
}

void IPFilterWidget::btnAdd_clicked()
{
	int var=0;
	
	TQRegExp rx("([*]|[0-9]{1,3}).([*]|[0-9]{1,3}).([*]|[0-9]{1,3}).([*]|[0-9]{1,3})");
	TQRegExpValidator v( rx,0);
	
	TQString ip = peerIP->text();

	if(v.validate( ip, var ) == TQValidator::Acceptable)
	{
		if(lstPeers->findItem(ip, 0) == 0)
			new KListViewItem(lstPeers, ip);
	}
	else
		KMessageBox::sorry(0, i18n("You must enter IP in format 'XXX.XXX.XXX.XXX'. You can also use wildcards for ranges like '127.0.0.*'."));
}

void IPFilterWidget::btnRemove_clicked()
{
	if(lstPeers->currentItem())
		delete lstPeers->currentItem();
}

void IPFilterWidget::btnClear_clicked()
{
	lstPeers->clear();
}

void IPFilterWidget::btnOpen_clicked()
{
	TQString lf = KFileDialog::getOpenFileName(TQString(), "*.txt|",this,i18n("Choose a file"));

	if(lf.isEmpty())
		return;
	
	btnClear_clicked();
	
	loadFilter(lf);
}

void IPFilterWidget::btnSave_clicked()
{
	TQString sf = KFileDialog::getSaveFileName(TQString(),"*.txt|",this,i18n("Choose a filename to save under"));

	if(sf.isEmpty())
		return;
	
	saveFilter(sf);
}

void IPFilterWidget::btnOk_clicked()
{
	btnApply_clicked();
	this->accept();
}

void IPFilterWidget::btnApply_clicked()
{
	IPBlocklist& ipfilter = IPBlocklist::instance();
	
	int count = 0;
	
	TQStringList* peers = new TQStringList();
	 
	TQListViewItemIterator it(lstPeers);
	while (it.current()) 
	{
		*peers << it.current()->text(0);
		++it;
		++count;
	}
	
	ipfilter.setBlocklist(peers);
	
	delete peers;
	
	Out(SYS_IPF|LOG_NOTICE) << "Loaded " << count << " blocked IP ranges." << endl;
}

void IPFilterWidget::saveFilter(TQString& fn)
{
	TQFile fptr(fn);
	
	if (!fptr.open(IO_WriteOnly))
	{
		Out(SYS_GEN|LOG_NOTICE) << TQString("Could not open file %1 for writing.").arg(fn) << endl;
		return;
	}
	
	TQTextStream out(&fptr);
	
	TQListViewItemIterator it(lstPeers);
	while (it.current()) 
	{
		out << it.current()->text(0) << ::endl;
		++it;
	}
	
	fptr.close();
}

void IPFilterWidget::loadFilter(TQString& fn)
{
	TQFile dat(fn);
	dat.open(IO_ReadOnly);

	TQTextStream stream( &dat );
	TQString line;
	
	TQRegExp rx("([*]|[0-9]{1,3}).([*]|[0-9]{1,3}).([*]|[0-9]{1,3}).([*]|[0-9]{1,3})");
	TQRegExpValidator v( rx,0);
	
	
	int i=0;
	int var=0;
	bool err = false;
	
	while ( !stream.atEnd() && i < MAX_RANGES )
	{
		line = stream.readLine();
		if ( v.validate( line, var ) != TQValidator::Acceptable )
		{
			err = true;
			continue;
		}
				
		new KListViewItem(lstPeers, line);
		++i;
	}
	
	if(err)
		Out(SYS_IPF|LOG_NOTICE) << "Some lines could not be loaded. Check your filter file..." << endl;
	
	dat.close();
}

#include "ipfilterwidget.moc"

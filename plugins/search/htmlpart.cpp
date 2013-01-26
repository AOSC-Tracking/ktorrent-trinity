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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.             *
 ***************************************************************************/
#include <kmessagebox.h>
#include <kio/job.h>
#include <kio/jobclasses.h>
//#include <tqfile.h>
#include <tqclipboard.h>
#include <tqapplication.h>
#include <kio/netaccess.h>
#include <klocale.h>
#include <kfiledialog.h>
#include <tdeparts/browserextension.h>
#include <util/constants.h>
#include <tdehtmlview.h>
#include "htmlpart.h"

using namespace bt;

namespace kt
{
	
	HTMLPart::HTMLPart(TQWidget *parent)
			: KHTMLPart(parent)
	{
		setJScriptEnabled(true);
		setJavaEnabled(true);
		setMetaRefreshEnabled(true);
		setPluginsEnabled(false);
		setStatusMessagesEnabled(false);
		KParts::BrowserExtension* ext = this->browserExtension();
		connect(ext,TQT_SIGNAL(openURLRequest(const KURL&,const KParts::URLArgs&)),
				this,TQT_SLOT(openURLRequest(const KURL&, const KParts::URLArgs& )));
	
		ext->enableAction("copy",true);
		ext->enableAction("paste",true);
		active_job = 0;
	}
	
	
	HTMLPart::~HTMLPart()
	{}
	
	void HTMLPart::copy()
	{
		TQString txt = selectedText();
		TQClipboard *cb = TQApplication::clipboard();
		// Copy text into the clipboard
		if (cb)
			cb->setText(txt,TQClipboard::Clipboard);
	}
	
	void HTMLPart::openURLRequest(const KURL &u,const KParts::URLArgs &)
	{
		if (active_job)
		{
			active_job->kill(true);
			active_job = 0;
		}
		
		TDEIO::TransferJob* j = TDEIO::get(u,false,false);
		connect(j,TQT_SIGNAL(data(TDEIO::Job*,const TQByteArray &)),
				this,TQT_SLOT(dataRecieved(TDEIO::Job*, const TQByteArray& )));
		connect(j,TQT_SIGNAL(result(TDEIO::Job*)),this,TQT_SLOT(jobDone(TDEIO::Job* )));
		connect(j,TQT_SIGNAL(mimetype(TDEIO::Job*, const TQString &)),
				this,TQT_SLOT(mimetype(TDEIO::Job*, const TQString& )));
	
		active_job = j;
		curr_data.resize(0);
		mime_type = TQString();
		curr_url = u;
	}
	
	void HTMLPart::back()
	{
		if (history.count() <= 1)
		{
			backAvailable(false);
		}
		else
		{
			history.pop_back();
			KURL u = history.back();
			openURL(u);
			backAvailable(history.count() > 1 ? true : false);
			
		}
	}
	
	void HTMLPart::addToHistory(const KURL & url)
	{
		history.append(url);
		if (history.count() > 1)
			backAvailable(true);
	}
	
	void HTMLPart::reload()
	{
		openURL(url());
	}
	
	void HTMLPart::dataRecieved(TDEIO::Job* job,const TQByteArray & data)
	{
		if (job != active_job)
		{
			job->kill(true);
			return;
		}
		
		if (data.size() == 0)
			return;
		
		Uint32 off = curr_data.size();
		curr_data.resize(curr_data.size() + data.size());
		for (Uint32 i = 0;i < data.size();i++)
		{
			curr_data[i + off] = data[i];
		}
	}
	
	void HTMLPart::mimetype(TDEIO::Job* job,const TQString & mt)
	{
		if (job != active_job)
		{
			job->kill(true);
			return;
		}
	
		mime_type = mt;
	}
	
	void HTMLPart::jobDone(TDEIO::Job* job)
	{
		if (job != active_job)
		{
			job->kill(true);
			return;
		}
		
		if (job->error() == 0)
		{
			bool is_bencoded_data = curr_data.size() > 0 &&
					curr_data[0] == 'd' &&
					curr_data[curr_data.size()-1] == 'e';
			
			if (is_bencoded_data || mime_type == "application/x-bittorrent")
			{
				int ret = KMessageBox::questionYesNoCancel(0,
						i18n("Do you want to download or save the torrent?"),
						i18n("Download Torrent"),
						KGuiItem(i18n("to download", "Download"),"down"),
						KStdGuiItem::save());
			
				if (ret == KMessageBox::Yes)
					openTorrent(curr_url);
				else if (ret == KMessageBox::No)
					saveTorrent(curr_url);
			}
			else
			{
				addToHistory(curr_url);
				begin(curr_url);
				write(curr_data.data(),curr_data.size());
				end();
				view()->ensureVisible(0,0);
				searchFinished();
			}
		}
		else
		{
			begin(curr_url);
			write(TDEIO::buildErrorString(job->error(),job->errorText()));/*,&curr_url));**/
			end();
		}
		active_job = 0;
		curr_data.resize(0);
		curr_url = KURL();
		mime_type = TQString();
	}
}

#include "htmlpart.moc"

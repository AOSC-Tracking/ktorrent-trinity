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
#include <tdeglobal.h>
#include <tdeconfig.h>
#include <tqapplication.h>
#include "logviewer.h"
#include "logflags.h"
#include "logviewerpluginsettings.h"

namespace kt
{
	const int LOG_EVENT_TYPE = 65432;
	
	class LogEvent : public TQCustomEvent
	{
		TQString str;
	public:
		LogEvent(const TQString & str) : TQCustomEvent(LOG_EVENT_TYPE),str(str)
		{}
		
		virtual ~LogEvent()
		{}
		
		const TQString & msg() const {return str;}
	};

	LogViewer::LogViewer(TQWidget *parent, const char *name)
			: KTextBrowser(parent, name), LogMonitorInterface()
	{
		/*
		IMPORTANT: use LogText mode, so that setMaxLogLines will work, if not everything will be kept in memory.
		*/
		setTextFormat(TQt::LogText);
		setMaxLogLines(200);
		setMinimumSize(TQSize(0,50));
		setSizePolicy(TQSizePolicy::Expanding,TQSizePolicy::Expanding);
		TDEGlobal::config()->setGroup("LogViewer");
		if (TDEGlobal::config()->hasKey("LogViewerWidgetSize"))
		{
			TQSize s = TDEGlobal::config()->readSizeEntry("LogViewerWidgetSize",0);
			resize(s);
		}
		
		LogFlags::instance().setLog(this);
	}


	LogViewer::~LogViewer()
	{
		TDEGlobal::config()->setGroup("LogViewer");
		TDEGlobal::config()->writeEntry("LogViewerWidgetSize",size());
		LogFlags::instance().setLog(0);
	}


	void LogViewer::message(const TQString& line, unsigned int arg)
	{
		/*
			IMPORTANT: because TQTextBrowser is not thread safe, we must use the TQt event mechanism 
			to add strings to it, this will ensure that strings will only be added in the main application
			thread.
		*/
		if(arg==0x00 || LogFlags::instance().checkFlags(arg))
		{
			if(m_useRichText)
			{
				TQString tmp = line;
				LogEvent* le = new LogEvent(LogFlags::instance().getFormattedMessage(arg, tmp));
				TQApplication::postEvent(this,le);
			}
			else
			{
				LogEvent* le = new LogEvent(line);
				TQApplication::postEvent(this,le);
			}
		}
	}
	
	void LogViewer::customEvent(TQCustomEvent* ev)
	{
		if (ev->type() == LOG_EVENT_TYPE)
		{
			LogEvent* le = (LogEvent*)ev;
			append(le->msg());
		}
	}
	
	void LogViewer::setRichText(bool val)
	{
		m_useRichText = val;
	}
}
#include "logviewer.moc"

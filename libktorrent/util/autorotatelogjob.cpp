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
#include <kurl.h>
#include <kprocess.h>
#include <util/fileops.h>
#include "autorotatelogjob.h"
#include "log.h"

namespace bt
{

	AutoRotateLogJob::AutoRotateLogJob(const TQString & file,Log* lg)
		: TDEIO::Job(false),file(file),cnt(10),lg(lg)
	{
		update();
	}


	AutoRotateLogJob::~AutoRotateLogJob()
	{}

	void AutoRotateLogJob::kill(bool)
	{
		m_error = 0;
		emitResult();
	}
		
	void AutoRotateLogJob::update()
	{
		while (cnt > 1)
		{
			TQString prev = TQString("%1-%2.gz").arg(file).arg(cnt - 1);
			TQString curr = TQString("%1-%2.gz").arg(file).arg(cnt);
			if (bt::Exists(prev)) // if file exists start the move job
			{
				TDEIO::Job* sj = TDEIO::file_move(KURL::fromPathOrURL(prev),KURL::fromPathOrURL(curr),-1,true,false,false);
				connect(sj,TQ_SIGNAL(result(TDEIO::Job*)),this,TQ_SLOT(moveJobDone(TDEIO::Job* )));	
				return;
			}
			else
			{
				cnt--;
			}
		}
			
		if (cnt == 1)
		{
				// move current log to 1 and zip it
			bt::Move(file,file + "-1",true);
			TDEIO::Job* sj = TDEIO::file_move(KURL::fromPathOrURL(file),KURL::fromPathOrURL(file + "-1"),-1,true,false,false);
			connect(sj,TQ_SIGNAL(result(TDEIO::Job*)),this,TQ_SLOT(moveJobDone(TDEIO::Job* )));
		}
		else
		{
				// final log file is moved, now zip it and end the job
			std::system(TQString("gzip " + TDEProcess::quote(file + "-1")).local8Bit());
			m_error = 0;
			lg->logRotateDone();
			emitResult();
		}
	}
		
	
	void AutoRotateLogJob::moveJobDone(TDEIO::Job*)
	{
		cnt--; // decrease counter so the newt file will be moved in update
		update(); // don't care about result of job
	}

}
#include "autorotatelogjob.moc"

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
#include <util/log.h>
#include "movedatafilesjob.h"

namespace bt
{

	MoveDataFilesJob::MoveDataFilesJob() : TDEIO::Job(false),err(false),active_job(0)
	{}


	MoveDataFilesJob::~MoveDataFilesJob()
	{}

	void MoveDataFilesJob::addMove(const TQString & src,const TQString & dst)
	{
		todo.insert(src,dst);
	}
		
	void MoveDataFilesJob::onJobDone(TDEIO::Job* j)
	{
		if (j->error() || err)
		{
			if (!err)
				m_error = TDEIO::ERR_INTERNAL;
			
			active_job = 0;
			if (j->error())
				j->showErrorDialog();
			
			// shit happened cancel all previous moves
			err = true;
			recover();
		}
		else
		{
			success.insert(active_src,active_dst);
			active_src = active_dst = TQString();
			active_job = 0;
			startMoving();
		}
	}
	
	void MoveDataFilesJob::onCanceled(TDEIO::Job* j)
	{
		m_error = TDEIO::ERR_USER_CANCELED;
		active_job = 0;
		err = true;
		recover();
	}
	
	void MoveDataFilesJob::startMoving()
	{
		if (todo.isEmpty())
		{
			m_error = 0;
			emitResult();
			return;
		}
			
		TQMap<TQString,TQString>::iterator i = todo.begin();	
		active_job = TDEIO::move(KURL::fromPathOrURL(i.key()),KURL::fromPathOrURL(i.data()),false);
		active_src = i.key();
		active_dst = i.data();
		Out(SYS_GEN|LOG_DEBUG) << "Moving " << active_src << " -> " << active_dst << endl;
		connect(active_job,TQ_SIGNAL(result(TDEIO::Job*)),this,TQ_SLOT(onJobDone(TDEIO::Job*)));
		connect(active_job,TQ_SIGNAL(canceled(TDEIO::Job*)),this,TQ_SLOT(onCanceled(TDEIO::Job*)));
		todo.erase(i);
	}
	
	void MoveDataFilesJob::recover()
	{
		if (success.isEmpty())
		{
			emitResult();
			return;
		}
		TQMap<TQString,TQString>::iterator i = success.begin();	
		active_job = TDEIO::move(KURL::fromPathOrURL(i.data()),KURL::fromPathOrURL(i.key()),false);
		connect(active_job,TQ_SIGNAL(result(TDEIO::Job*)),this,TQ_SLOT(onJobDone(TDEIO::Job*)));
		connect(active_job,TQ_SIGNAL(canceled(TDEIO::Job*)),this,TQ_SLOT(onCanceled(TDEIO::Job*)));
		success.erase(i);
	}
}
#include "movedatafilesjob.moc"

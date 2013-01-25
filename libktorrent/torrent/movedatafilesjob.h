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
#ifndef BTMOVEDATAFILESJOB_H
#define BTMOVEDATAFILESJOB_H

#include <kio/job.h>

namespace bt
{

	/**
	 * @author Joris Guisson <joris.guisson@gmail.com>
	 * TDEIO::Job to move all the files of a torrent.
	*/
	class MoveDataFilesJob : public TDEIO::Job
	{
		Q_OBJECT
  
	public:
		MoveDataFilesJob();
		virtual ~MoveDataFilesJob();
		
		/**
		 * Add a move to the todo list.
		 * @param src File to move
		 * @param dst Where to move it to
		 */
		void addMove(const TQString & src,const TQString & dst);
		
		/**
		 * Start moving the files.
		 */
		void startMoving();
		
	private slots:
		void onJobDone(TDEIO::Job* j);
		void onCanceled(TDEIO::Job* j);
		
	private:
		void recover();

	private:
		bool err;
		TDEIO::Job* active_job;
		TQString active_src,active_dst;
		TQMap<TQString,TQString> todo;
		TQMap<TQString,TQString> success;		
	};

}

#endif

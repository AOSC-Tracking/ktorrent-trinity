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
#ifndef BTPREALLOCATIONTHREAD_H
#define BTPREALLOCATIONTHREAD_H

#include <tqstring.h>
#include <tqthread.h>
#include <tqmap.h>
#include <tqmutex.h>
#include <util/constants.h>



namespace bt
{
	class ChunkManager;

	/**
	 * @author Joris Guisson <joris.guisson@gmail.com>
	 * 
	 * Thread to preallocate diskspace
	*/
	class PreallocationThread : public TQThread
	{
		ChunkManager* cman;
		bool stopped,not_finished,done;
		TQString error_msg;
		Uint64 bytes_written;
		mutable TQMutex mutex;
	public:
		PreallocationThread(ChunkManager* cman);
		virtual ~PreallocationThread();

		virtual void run();
		
		
		/**
		 * Stop the thread. 
		 */
		void stop();
		
		/**
		 * Set an error message, also calls stop
		 * @param msg The message
		 */
		void setErrorMsg(const TQString & msg);
		
		/// See if the thread has been stopped
		bool isStopped() const;
		
		/// Did an error occur during the preallocation ?
		bool errorHappened() const;
		
		/// Get the error_msg
		const TQString & errorMessage() const {return error_msg;}
		
		/// nb Number of bytes have been written
		void written(Uint64 nb);
		
		/// Get the number of bytes written
		Uint64 bytesWritten();
		
		/// Allocation was aborted, so the next time the torrent is started it needs to be started again
		void setNotFinished();
		
		/// See if the allocation hasn't completed yet
		bool isNotFinished() const;
		
		/// See if the thread was done
		bool isDone() const;
	private:
		bool expand(const TQString & path,Uint64 max_size);
	};

}

#endif

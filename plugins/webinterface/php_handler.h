  /***************************************************************************
 *   Copyright (C) 2006 by Diego R. Brogna                                 *
 *   dierbro@gmail.com                                               	   *
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
#ifndef PHP_HANDLER_H
#define PHP_HANDLER_H
		
#include <tqmap.h>
#include <kurl.h>
#include <tqprocess.h>


namespace kt
{
	class PhpInterface;
	
	class PhpHandler : public TQProcess 
	{
		TQ_OBJECT
  
	public:
		PhpHandler(const TQString & php_exe,PhpInterface *php);
		virtual ~PhpHandler();
			
		bool executeScript(const TQString & path,const TQMap<TQString,TQString> & args);
		const TQByteArray & getOutput() const {return output;};
		
	public slots:
		void onExited();
		void onReadyReadStdout();
		
	signals:
		void finished();

	private:
		bool containsDelimiters(const TQString & str);
		
	private:
		TQByteArray output;
		PhpInterface *php_i;
		
		static TQMap<TQString,TQByteArray> scripts;
	};
}

#endif

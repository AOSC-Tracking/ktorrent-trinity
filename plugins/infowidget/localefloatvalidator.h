/***************************************************************************
 *   Copyright (C) 2005 by                                                 *
 *   Joris Guisson <joris.guisson@gmail.com>                               *
 *   Vincent Wagelaar <vincent@ricardis.tudelft.nl>                        *
 *   Jonas Widarsson <jonas@widarsson.com>                                 *
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
#ifndef LOCALEFLOATVALIDATOR_H
#define LOCALEFLOATVALIDATOR_H

#include <tqvalidator.h>
class TQRegExp;
namespace kt{
	/**
	* @author Jonas Widarsson
	*
	* A float validator that respects TDELocale::decimalSymbol()
	*/
	class LocaleFloatValidator : public TQValidator
	{
		TQ_OBJECT
  
	public:
		LocaleFloatValidator( TQObject * parent, const char *name = 0 );
	
		virtual TQValidator::State validate( TQString & str, int & pos) const;
	private:
		TQRegExp regexp;
	};
}

#endif

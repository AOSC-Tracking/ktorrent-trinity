/***************************************************************************
 *   Copyright © 2007 by Krzysztof Kundzicz                                *
 *   athantor@gmail.com                                                    *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef CHARTDRAWERDATA_H_
#define CHARTDRAWERDATA_H_

#include <tqpen.h>
#include <vector>
#include <map>

namespace kt {

class ChartDrawer;

/**
\brief Container for data used by ChartDrawer
\author Krzysztof Kundzicz <athantor@gmail.com>
*/
class ChartDrawerData
{
	friend class ChartDrawer;
	
	public:
		///Type for stroring values
		typedef std::vector<double> val_t;

	private:
		///Pen used for drawing
		TQPen * pmQp;
		///Values
		val_t * pmVals;
		///Name of set
		TQString mName;
		
	public:
		/**
		\brief Constructor
		\param rN Name
		*/
		ChartDrawerData(const TQString & rN);
		/**
		\brief Copy constructor
		\param rS Source
		*/
		ChartDrawerData(const ChartDrawerData &);
		/**
		\brief Constructor
		\param s Size
		\param rN Name
		*/
		ChartDrawerData(const size_t s,  const TQString & rN);
		/**
		\brief Constructor
		\param rQp Pen used for drawing
		\param rN Name
		*/
		ChartDrawerData(const TQPen & rQp, const TQString & rN);
		/**
		\brief Constructor
		\param rQp Pen used for drawing
		\param s Size
		\param rN Name
		*/
		ChartDrawerData(const TQPen & rQp, const size_t s,  const TQString & rN);
		
		///Destructor
		~ChartDrawerData();
		
		/**
		\brief Gets values
		\return Pointer to values container
		*/
		const val_t * GetVals() const;
		/**
		\brief Gets pen
		\return Pointer pen
		*/
		const TQPen * GetPen() const;
		/**
		\brief Gets name
		\return Name
		*/
		TQString GetName() const;
		
		/**
		\brief Sets pen
		\param rQp Pen
		*/
		void SetPen(const TQPen & rQp);
		/**
		\brief Sets name
		\param rN Name
		*/
		void SetName( const TQString & rN );
		
		/**
		\brief Finds maximum value
		\return Pair with value and position
		*/
		std::pair<double, size_t> Max() const;
		
};

} 

#endif

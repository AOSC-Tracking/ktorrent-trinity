/***************************************************************************
 *   Copyright (C) 2004 by Alexander Dymo                                  *
 *   adymo@kdevelop.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef IDEALDOCKSPLITTER_H
#define IDEALDOCKSPLITTER_H

#include <tqsplitter.h>
#include <tqvaluelist.h>

namespace Ideal {

class DockWidget;

/**
@short Splitter for docks
*/
class DockSplitter: public TQSplitter {
    TQ_OBJECT
  
public:
    DockSplitter(TQt::Orientation orientation, TQWidget *parent = 0, const char *name = 0);
    ~DockSplitter();
    
    void addDock(uint row, uint col, TQWidget *dock);
    void removeDock(uint row, uint col, bool alsoDelete = false);
    
    TQPair<uint, uint> indexOf(TQWidget *dock);
    
    int numRows() const;
    int numCols(int row) const;

protected:
    void appendSplitter();
    void createSplitters(uint index);
    void shiftWidgets(TQSplitter *splitter, uint row, uint fromCol);
    
    bool isRowEmpty(int row);
    
private:
    TQt::Orientation m_orientation;
    TQValueList<TQSplitter*> m_splitters;
    TQValueList<TQValueList<TQWidget*> > m_docks;
};

}

#endif

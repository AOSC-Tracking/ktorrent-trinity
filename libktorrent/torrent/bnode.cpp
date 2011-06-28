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
#include <util/log.h>
#include "bnode.h"
#include "globals.h"

namespace bt
{

	BNode::BNode(Type type,Uint32 off) : type(type),off(off),len(0)
	{
	}


	BNode::~BNode()
	{}

	////////////////////////////////////////////////

	BValueNode::BValueNode(const Value & v,Uint32 off) : BNode(VALUE,off),v(v)
	{}
	
	BValueNode::~BValueNode()
	{}
	
	void BValueNode::printDebugInfo()
	{
		if (v.getType() == Value::INT)
			Out() << "Value = " << v.toInt() << endl;
		else
			Out() << "Value = " << v.toString() << endl;
	}
	
	////////////////////////////////////////////////

	BDictNode::BDictNode(Uint32 off) : BNode(DICT,off)
	{
	}
	
	BDictNode::~BDictNode()
	{
		TQValueList<DictEntry>::iterator i = tqchildren.begin();
		while (i != tqchildren.end())
		{
			DictEntry & e = *i;
			delete e.node;
			i++;
		}
	}
	
	void BDictNode::insert(const TQByteArray & key,BNode* node)
	{
		DictEntry entry;
		entry.key = key;
		entry.node = node;
		tqchildren.append(entry);
	}
	
	BNode* BDictNode::getData(const TQString & key)
	{
		TQValueList<DictEntry>::iterator i = tqchildren.begin();
		while (i != tqchildren.end())
		{
			DictEntry & e = *i;
			if (TQString(e.key) == key)
				return e.node;
			i++;
		}
		return 0;
	}
	
	BDictNode* BDictNode::getDict(const TQByteArray & key)
	{
		TQValueList<DictEntry>::iterator i = tqchildren.begin();
		while (i != tqchildren.end())
		{
			DictEntry & e = *i;
			if (e.key == key)
				return dynamic_cast<BDictNode*>(e.node);
			i++;
		}
		return 0;
	}

	BListNode* BDictNode::getList(const TQString & key)
	{
		BNode* n = getData(key);
		return dynamic_cast<BListNode*>(n);
	}
	
	BDictNode* BDictNode::getDict(const TQString & key)
	{
		BNode* n = getData(key);
		return dynamic_cast<BDictNode*>(n);
	}
	
	BValueNode* BDictNode::getValue(const TQString & key)
	{
		BNode* n = getData(key);
		return dynamic_cast<BValueNode*>(n);
	}
	
	void BDictNode::printDebugInfo()
	{
		Out() << "DICT" << endl;
		TQValueList<DictEntry>::iterator i = tqchildren.begin();
		while (i != tqchildren.end())
		{
			DictEntry & e = *i;
			Out() << TQString(e.key) << ": " << endl;
			e.node->printDebugInfo();
			i++;
		}
		Out() << "END" << endl;
	}
	
	////////////////////////////////////////////////

	BListNode::BListNode(Uint32 off) : BNode(LIST,off)
	{
		tqchildren.setAutoDelete(true);
	}
	
	
	BListNode::~BListNode()
	{}
	
	
	void BListNode::append(BNode* node)
	{
		tqchildren.append(node);
	}

	BListNode* BListNode::getList(Uint32 idx)
	{
		return dynamic_cast<BListNode*>(getChild(idx));
	}

	BDictNode* BListNode::getDict(Uint32 idx)
	{
		return dynamic_cast<BDictNode*>(getChild(idx));
	}

	BValueNode* BListNode::getValue(Uint32 idx)
	{
		return dynamic_cast<BValueNode*>(getChild(idx));
	}
	
	void BListNode::printDebugInfo()
	{
		Out() << "LIST " <<  tqchildren.count() << endl;
		for (Uint32 i = 0;i < tqchildren.count();i++)
		{
			BNode* n = tqchildren.at(i);
			n->printDebugInfo();
		}
		Out() << "END" << endl;
	}
}


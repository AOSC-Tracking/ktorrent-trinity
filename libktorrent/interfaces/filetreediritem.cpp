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
#include <tdelocale.h>
#include <kiconloader.h>
#include <kmimetype.h>
#include <util/functions.h>
#include <interfaces/functions.h>
#include <torrent/globals.h>
#include "filetreediritem.h"
#include "filetreeitem.h"
#include "torrentfileinterface.h"
#include <torrent/torrentfile.h>

using namespace bt;

namespace kt
{

	FileTreeDirItem::FileTreeDirItem(TDEListView* klv,const TQString & name,FileTreeRootListener* rl)
	: TQCheckListItem(klv,TQString(),TQCheckListItem::CheckBox),name(name),root_listener(rl)
	{
		parent = 0;
		size = 0;
		setPixmap(0,TDEGlobal::iconLoader()->loadIcon("folder",TDEIcon::Small));
		setText(0,name);
		setText(1,BytesToString(size));
		setText(2,i18n("Yes"));
		manual_change = true;
		setOn(true);
		manual_change = false;
	}

	FileTreeDirItem::FileTreeDirItem(FileTreeDirItem* parent,const TQString & name)
	: TQCheckListItem(parent,TQString(),TQCheckListItem::CheckBox),
	name(name),parent(parent)
	{
		size = 0;
		setPixmap(0,TDEGlobal::iconLoader()->loadIcon("folder",TDEIcon::Small));
		setText(0,name);
		setText(1,BytesToString(size));
		setText(2,i18n("Yes"));
		manual_change = true;
		setOn(true);
		manual_change = false;
	}

	FileTreeDirItem::~FileTreeDirItem()
	{
	}

	void FileTreeDirItem::insert(const TQString & path,kt::TorrentFileInterface & file)
	{
		size += file.getSize();
		setText(1,BytesToString(size));
		int p = path.find(bt::DirSeparator());
		if (p == -1)
		{
			children.insert(path,newFileTreeItem(path,file));
		}
		else
		{
			TQString subdir = path.left(p);
			FileTreeDirItem* sd = subdirs.find(subdir);
			if (!sd)
			{
				sd = newFileTreeDirItem(subdir);
				subdirs.insert(subdir,sd);
			}
			
			sd->insert(path.mid(p+1),file);
		}
	}
	
	void FileTreeDirItem::setAllChecked(bool on,bool keep_data)
	{
		if (!manual_change)
		{
			manual_change = true;
			setOn(on);
			manual_change = false;
		}
	// first set all the child items
		bt::PtrMap<TQString,FileTreeItem>::iterator i = children.begin();
		while (i != children.end())
		{
			i->second->setChecked(on,keep_data);
			i++;
		}

	// then recursivly move on to subdirs
		bt::PtrMap<TQString,FileTreeDirItem>::iterator j = subdirs.begin();
		while (j != subdirs.end())
		{
			j->second->setAllChecked(on,keep_data);
			j++;
		}
	}
	

	void FileTreeDirItem::invertChecked()
	{
	// first set all the child items
		bt::PtrMap<TQString,FileTreeItem>::iterator i = children.begin();
		while (i != children.end())
		{
			FileTreeItem* item = i->second;
			item->setChecked(!item->isOn());
			i++;
		}

	// then recursivly move on to subdirs
		bt::PtrMap<TQString,FileTreeDirItem>::iterator j = subdirs.begin();
		while (j != subdirs.end())
		{
			j->second->invertChecked();
			j++;
		}
	}

	void FileTreeDirItem::stateChange(bool on)
	{
		if (!manual_change)
		{
			if (on)
			{
				setAllChecked(true);
			}
			else
			{
				switch (confirmationDialog())
				{
					case KEEP_DATA:
						setAllChecked(false,true);
						break;
					case THROW_AWAY_DATA:
						setAllChecked(false,false);
						break;
					case CANCELED:
					default:
						manual_change = true;
						setOn(true);
						manual_change = false;
						return;
				}
			}
			if (parent)
				parent->childStateChange();
		}
		setText(2,on ? i18n("Yes") : i18n("No"));
	}
	
	Uint64 FileTreeDirItem::bytesToDownload() const
	{
		Uint64 tot = 0;
		// first check all the child items
		bt::PtrMap<TQString,FileTreeItem>::const_iterator i = children.begin();
		while (i != children.end())
		{
			const FileTreeItem* item = i->second;
			tot += item->bytesToDownload();
			i++;
		}

		// then recursivly move on to subdirs
		bt::PtrMap<TQString,FileTreeDirItem>::const_iterator j = subdirs.begin();
		while (j != subdirs.end())
		{
			tot += j->second->bytesToDownload();
			j++;
		}
		return tot;
	}

	bool FileTreeDirItem::allChildrenOn()
	{
		// first check all the child items
		bt::PtrMap<TQString,FileTreeItem>::iterator i = children.begin();
		while (i != children.end())
		{
			FileTreeItem* item = i->second;
			if (!item->isOn())
				return false;
			i++;
		}

		// then recursivly move on to subdirs
		bt::PtrMap<TQString,FileTreeDirItem>::iterator j = subdirs.begin();
		while (j != subdirs.end())
		{
			if (!j->second->allChildrenOn())
				return false;
			j++;
		}
		return true;
	}

	void FileTreeDirItem::childStateChange()
	{
	// only set this dir on if all children are on
		manual_change = true;
		setOn(allChildrenOn());
		manual_change = false;
	
		if (parent)
			parent->childStateChange();
		else if (root_listener)
			root_listener->treeItemChanged();
			
	}

	int FileTreeDirItem::compare(TQListViewItem* i, int col, bool ascending) const
	{
		if (col == 1)
		{
			FileTreeDirItem* other = dynamic_cast<FileTreeDirItem*>(i);
			if (!other)
				return 0;
			else
				return (int)(size - other->size);
		}
		else
		{
			//return TQCheckListItem::compare(i, col, ascending);
			// case insensitive comparison
			return TQString::compare(text(col).lower(),i->text(col).lower());
		}
	}

	TorrentFileInterface & FileTreeDirItem::findTorrentFile(TQListViewItem* item)
	{
	// first check all the child items
		TorrentFileInterface & nullfile = (TorrentFileInterface &)TorrentFile::null;
		bt::PtrMap<TQString,FileTreeItem>::iterator i = children.begin();
		while (i != children.end())
		{
			FileTreeItem* file = i->second;
			if (file == (FileTreeItem*)item)
				return file->getTorrentFile();
			i++;
		}

	// then recursivly move on to subdirs
		bt::PtrMap<TQString,FileTreeDirItem>::iterator j = subdirs.begin();
		while (j != subdirs.end())
		{
			TorrentFileInterface & thefile = j->second->findTorrentFile(item);
			if(!thefile.isNull())
				return thefile;
			j++;
		}
		return nullfile;
	}

	FileTreeItem* FileTreeDirItem::newFileTreeItem(const TQString & name,TorrentFileInterface & file)
	{
		return new FileTreeItem(this,name,file);
	}

	FileTreeDirItem* FileTreeDirItem::newFileTreeDirItem(const TQString & subdir)
	{
		return new FileTreeDirItem(this,subdir);
	}

	bt::ConfirmationResult FileTreeDirItem::confirmationDialog()
	{
		return bt::THROW_AWAY_DATA;
	}
	
	TQString FileTreeDirItem::getPath() const
	{
		if (!parent)
			return bt::DirSeparator();
		else
			return parent->getPath() + name + bt::DirSeparator();
	}
}


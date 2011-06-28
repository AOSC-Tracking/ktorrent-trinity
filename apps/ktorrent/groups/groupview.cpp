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
#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kpopupmenu.h>
#include <kaction.h>
#include <kmessagebox.h>
#include <kinputdialog.h>
#include <kstandarddirs.h>
#include <kpopupmenu.h>
#include <tqheader.h>
#include <util/log.h>
#include <interfaces/torrentinterface.h>
#include "groupview.h"
#include "group.h"
#include "groupmanager.h"
#include "torrentgroup.h"
#include "../viewmanager.h"
#include "../ktorrentview.h"


using namespace bt;

namespace kt
{
	GroupViewItem::GroupViewItem(GroupView* tqparent,Group* g) : KListViewItem(tqparent),gview(tqparent)
	{
		setText(0,g->groupName());
		setPixmap(0,g->groupIcon());
	}
	
	GroupViewItem::GroupViewItem(GroupView* gview,KListViewItem* tqparent,Group* g) : KListViewItem(tqparent),gview(gview)
	{
		setText(0,g->groupName());
		setPixmap(0,g->groupIcon());
	}
	
	GroupViewItem::~GroupViewItem()
	{
	}
	
	int GroupViewItem::compare(TQListViewItem* i,int ,bool ) const
	{
		if (text(1).isNull() && i->text(1).isNull())
			return TQString::compare(text(0),i->text(0));
		else
			return TQString::compare(text(1),i->text(1));
	}

	GroupView::GroupView(ViewManager* view,KActionCollection* col,TQWidget *tqparent, const char *name)
	: KListView(tqparent, name),view(view),custom_root(0)
	{
		setFullWidth(true);
		setRootIsDecorated(true);
		setAcceptDrops(true);
		setDropHighlighter(true);
		setDropVisualizer(true);
		addColumn(i18n("Groups"));
		header()->hide();
		
		gman = new GroupManager();
	
		current = gman->allGroup();
		
		connect(this,TQT_SIGNAL(clicked(TQListViewItem*)),this,TQT_SLOT(onExecuted( TQListViewItem* )));
		connect(this,TQT_SIGNAL(contextMenu(KListView*,TQListViewItem*,const TQPoint & )),
				this,TQT_SLOT(showContextMenu( KListView*, TQListViewItem*, const TQPoint& )));
		connect(this,TQT_SIGNAL(dropped(TQDropEvent*,TQListViewItem*)),
				this,TQT_SLOT(onDropped( TQDropEvent*, TQListViewItem* )));	
		
		current_item = 0;
		menu = 0;
		createMenu(col);
		save_file = KGlobal::dirs()->saveLocation("data","ktorrent") + "groups";
		GroupViewItem* all = addGroup(gman->allGroup(),0);
		GroupViewItem* dwnld = addGroup(gman->downloadGroup(),all);
		GroupViewItem* upld = addGroup(gman->uploadGroup(),all);
		GroupViewItem* inactive = addGroup(gman->inactiveGroup(), all);
		GroupViewItem* active = addGroup(gman->activeGroup(), all);
		addGroup(gman->queuedDownloadsGroup(), dwnld);
		addGroup(gman->queuedUploadsGroup(), upld);
		addGroup(gman->userDownloadsGroup(), dwnld);
		addGroup(gman->userUploadsGroup(), upld);
		addGroup(gman->inactiveDownloadsGroup(), inactive);
		addGroup(gman->inactiveUploadsGroup(), inactive);
		addGroup(gman->activeDownloadsGroup(), active);
		addGroup(gman->activeUploadsGroup(), active);
		
		custom_root = new KListViewItem(all,i18n("Custom Groups"));
		custom_root->setPixmap(0,KGlobal::iconLoader()->loadIcon("folder",KIcon::Small));
		setOpen(custom_root,true);
	}


	GroupView::~GroupView()
	{	
		delete gman;
	}
	
	void GroupView::saveGroups()
	{
		gman->saveGroups(save_file);
	}
	
	void GroupView::loadGroups()
	{
		// load the groups from the groups file
		gman->loadGroups(save_file);
		for (GroupManager::iterator i = gman->begin();i != gman->end();i++)
		{
			addGroup(i->second,custom_root);
		}
		sort();
	}
	
	void GroupView::createMenu(KActionCollection* col)
	{
		menu = new KPopupMenu(this);
		
		new_group = new KAction(i18n("New Group"),"filenew",0,
							 TQT_TQOBJECT(this), TQT_SLOT(addGroup()),col, "New Group");
		
		edit_group = new KAction(i18n("Edit Name"),"edit",0,
							 TQT_TQOBJECT(this), TQT_SLOT(editGroupName()),col,"Edit Group Name");
		
		remove_group = new KAction(i18n("Remove Group"),"remove",0,
							 TQT_TQOBJECT(this), TQT_SLOT(removeGroup()),col,"Remove Group");
		
		open_in_new_tab = new KAction(i18n("Open Tab"),"fileopen",0,
							 TQT_TQOBJECT(this) ,TQT_SLOT(openView()),col,"Open Tab");
		
		open_in_new_tab->plug(menu);
		menu->insertSeparator();
		new_group->plug(menu);
		edit_group->plug(menu);
		remove_group->plug(menu);
	}
	
	void GroupView::addGroup()
	{
		TQString name = KInputDialog::getText(TQString(),i18n("Please enter the group name."));
		
		if (name.isNull() || name.length() == 0)
			return;
		
		if (gman->tqfind(name))
		{
			KMessageBox::error(this,i18n("The group %1 already exists.").tqarg(name));
			return;
		}
		
		addGroup(gman->newGroup(name),custom_root);
		saveGroups();
		sort();
	}
	
	void GroupView::removeGroup()
	{
		if (!current_item)
			return;
		
		Group* g = groups.tqfind(current_item);
		if (!g)
			return;
		
		groupRemoved(g);
		if (g == current)
		{
			current = gman->allGroup();
			currentGroupChanged(current);
		}
		
		groups.erase(current_item);
		gman->erase(g->groupName());
		delete current_item;
		current_item = 0;
		saveGroups();
	}
	
	void GroupView::editGroupName()
	{
		if (!current_item)
			return;
		
		Group* g = groups.tqfind(current_item);
		if (!g)
			return;
		
		TQString name = KInputDialog::getText(TQString(),i18n("Please enter the new group name."),g->groupName());
		
		if (name.isNull() || name.length() == 0)
			return;
		
		if (g->groupName() == name)
			return;
		
		if (gman->tqfind(name)) 
		{
			KMessageBox::error(this,i18n("The group %1 already exists.").tqarg(name));
		}
		else
		{
			gman->renameGroup(g->groupName(),name);
			current_item->setText(0,name);
			groupRenamed(g);
			saveGroups();
			sort();
		}
	}

	GroupViewItem* GroupView::addGroup(Group* g,KListViewItem* tqparent)
	{
		GroupViewItem* li = 0;
		if (tqparent)
		{
			li = new GroupViewItem(this,tqparent,g);
		}
		else
		{
			li = new GroupViewItem(this,g);
			li->setText(1,g->groupName());
		}
		
		groups.insert(li,g);
		if (custom_root && custom_root->childCount() == 1 && custom_root == tqparent)
			setOpen(custom_root,true);
		
		return li;
	}
	
	void GroupView::showContextMenu(KListView* ,TQListViewItem* item,const TQPoint & p)
	{
		current_item = dynamic_cast<GroupViewItem*>(item);
		
		Group* g = 0;
		if (current_item)
			g = groups.tqfind(current_item);
		
		if (!g ||!gman->canRemove(g))
		{
			edit_group->setEnabled(false);
			remove_group->setEnabled(false);
		}
		else
		{
			edit_group->setEnabled(true);
			remove_group->setEnabled(true);
		}
		
		open_in_new_tab->setEnabled(g != 0);
		
		menu->popup(p);
	}
	
	void GroupView::onExecuted(TQListViewItem* item)
	{
		if (!item) return;
		
		GroupViewItem* li = dynamic_cast<GroupViewItem*>(item);
		if (!li)
			return;
		
		Group* g = groups.tqfind(li);
		if (g)
		{
			current = g;
			currentGroupChanged(g);
		}
	}
	
	void GroupView::onDropped(TQDropEvent* e,TQListViewItem *after)
	{
		GroupViewItem* li = dynamic_cast<GroupViewItem*>(after);
		if (!li)
			return;
		
		TorrentGroup* g = dynamic_cast<TorrentGroup*>(groups.tqfind(li));
		if (g)
		{
			TQValueList<TorrentInterface*> sel;
			view->getSelection(sel);
			TQValueList<TorrentInterface*>::iterator i = sel.begin();
			while (i != sel.end())
			{
				g->add(*i);
				i++;
			}
			saveGroups();
		}
	}
	
	bool GroupView::acceptDrag(TQDropEvent* event) const
	{
		return event->provides("application/x-ktorrent-drag-object");
	}
	
	void GroupView::onTorrentRemoved(kt::TorrentInterface* tc)
	{
		gman->torrentRemoved(tc);
		saveGroups();
	}
	
	void GroupView::updateGroupsSubMenu(KPopupMenu* gsm)
	{
		gsm->clear();
		for (GroupManager::iterator i = gman->begin();i != gman->end();i++)
		{
			gsm->insertItem(i->first);
		}
	}
	
	void GroupView::onGroupsSubMenuItemActivated(KTorrentView* v,const TQString & group)
	{
		Group* g = gman->tqfind(group);
		if (g)
		{
			v->addSelectionToGroup(g);
			saveGroups();
		}
	}
	
	const Group* GroupView::findGroup(const TQString & name) const
	{
		Group* g = gman->tqfind(name);
		if (!g)
			g = gman->findDefault(name);
		
		return g;
	}
	
	void GroupView::openView()
	{
		if (!current_item)
			return;
		
		Group* g = groups.tqfind(current_item);
		if (g)
			openNewTab(g);
	}
}

#include "groupview.moc"

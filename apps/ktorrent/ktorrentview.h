/***************************************************************************
 *   Copyright (C) 2005 by                                                 *
 *   Joris Guisson <joris.guisson@gmail.com>                               *
 *   Ivan Vasic <ivasic@gmail.com>                                         *
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


#ifndef _KTORRENTVIEW_H_
#define _KTORRENTVIEW_H_

#include <tdelistview.h>

typedef TQValueList<TQCString> QCStringList;

class KURL;
class KTorrentViewItem;
class TDEPopupMenu;
class KTorrentCore;
class KTorrentViewMenu;
class KTorrentView;
class ScanDialog;
class TQString;
class FilterBar;

namespace kt
{
	class TorrentInterface;
	class Group;
}

using namespace bt;

class TorrentView : public TDEListView
{
public:
	TorrentView(KTorrentView* parent);
	virtual ~TorrentView();
	
	virtual bool eventFilter(TQObject* watched, TQEvent* e);
		
private:
	KTorrentView* ktview;
};


	

/**
 * List view which shows information about torrents.
 */
class KTorrentView : public TQWidget
{
	TQ_OBJECT
  
public:
	enum ActionEnableFlags
	{
		START = 1,
		STOP = 2,
		START_ALL = 4,
		STOP_ALL = 8,
		REMOVE = 16,
		SCAN = 32
	};
	
	/**
	 * Default constructor
	 */
	KTorrentView(TQWidget *parent);

	/**
	 * Destructor
	 */
	virtual ~KTorrentView();
	
	/// Update the caption, so the correct number of running torrents is shown in the tab
	void updateCaption();
	
	/// Trigger an updateActions signal
	void updateActions() {onSelectionChanged();}
	
	/// Get the current group
	const kt::Group* getCurrentGroup() const {return current_group;}

	/// Get the current TorrentInterface object
	kt::TorrentInterface* getCurrentTC();
	
	/// Save the views settings
	void saveSettings(TDEConfig* cfg,int idx);
	
	/// Load the views settings
	void loadSettings(TDEConfig* cfg,int idx);
	
	/**
	 * Put the current selection in a list.
	 * @param sel The list to put it in
	 */
	void getSelection(TQValueList<kt::TorrentInterface*> & sel);
	
	/**
	 * Add the current selection to a group. 
	 * @param g The group
	 */
	void addSelectionToGroup(kt::Group* g);
	
	/**
	 * Is column visible?
	 */
	bool columnVisible(int index);
	
	/**
	 * Setup the view columns, based upon the current group
	 * This will hide some columns for uploads only groups.
	 */
	void setupViewColumns();
	
	TQPtrList<TQListViewItem> selectedItems() {return view->selectedItems();}
		
	TDEListView* listView() {return view;}
	
	/**
	 * Toggle the visibility of the filter bar
	 */
	void toggleFilterBar();
	
public slots:
	void setCurrentGroup(kt::Group* group);
	void addTorrent(kt::TorrentInterface* tc);
	void removeTorrent(kt::TorrentInterface* tc);
	void update();
	void startDownloads();
	void stopDownloads();
	void startAllDownloads();
	void stopAllDownloads();
	void manualAnnounce();
	void previewFiles();
	void removeDownloads();
	void removeDownloadsAndData();
	void onSelectionChanged();
	void queueSlot();
	void checkDataIntegrity();
	void removeFromGroup();
	void showAddPeersWidget();
	void openOutputDirectory();
	void openTorXDirectory();
	void setDownloadLocationSlot();
	void dhtSlot();
	void utPexSlot();
	void speedLimits();

private slots:
	void onExecuted(TQListViewItem* item);
	void showContextMenu(TDEListView* ,TQListViewItem* item,const TQPoint & p);
	void onColumnVisibilityChange(int);
	void gsmItemActived(const TQString & group);
	
	
signals:
	void torrentClicked(kt::TorrentInterface* tc);
	void currentChanged(kt::TorrentInterface* tc);
	void wantToRemove(kt::TorrentInterface* tc,bool data_to);
	void wantToStop(kt::TorrentInterface* tc,bool user);
	void wantToStart(kt::TorrentInterface* tc);
	void viewChange(kt::TorrentInterface* tc);
	
	/**
	 * Emit that actions need to be updated
	 * @param flags OR of ActionEnableFlags
	 */
	void updateActions(int flags);
	void queue(kt::TorrentInterface* tc);
	void needsDataCheck(kt::TorrentInterface* tc);
	void updateGroupsSubMenu(TDEPopupMenu* gsm);
	void groupsSubMenuItemActivated(KTorrentView* v,const TQString & group);

private:
	bool acceptDrag(TQDropEvent* event) const;
	int getNumRunning();
	bool startDownload(kt::TorrentInterface* tc);
	void stopDownload(kt::TorrentInterface* tc);
	void showStartError();
	virtual TQDragObject* dragObject();
	void setupColumns();
	void insertColumn(TQString label, TQt::AlignmentFlags);
	void columnHide(int index);
	void columnShow(int index);	
	
	
private:
	TQMap<kt::TorrentInterface*,KTorrentViewItem*> items;
	KTorrentViewMenu* menu;
	TDEPopupMenu* m_headerMenu;
	kt::Group* current_group;
	Uint32 running;
	Uint32 total;
	TorrentView* view;
	FilterBar* filter_bar;
	
	friend class TorrentView;
};

#endif // _KTORRENTVIEW_H_

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


#ifndef _KTORRENT_H_
#define _KTORRENT_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <tdeapplication.h>
#include <newui/dmainwindow.h>
#include <tqtimer.h>
#include <interfaces/guiinterface.h>

typedef TQValueList<TQCString> QCStringList;

class TDEAction;
class TDEToggleAction;
class KURL;
class KTorrentCore;
class KTorrentView;
class TrayIcon;
class SetMaxRate;
class KTorrentDCOP;
class TQLabel;
class TQListViewItem;
class KTorrentPreferences;
class ViewManager;


namespace kt
{
	class TorrentInterface;
	class Group;
	class GroupView;
}


/**
 * This class serves as the main window for KTorrent.  It handles the
 * menus, toolbars, and status bars.
 *
 * @short Main window class
 * @author Joris Guisson <joris.guisson@gmail.com>
 * @version 0.1
 */
class KTorrent : public DMainWindow, public kt::GUIInterface
{
	Q_OBJECT
  
public:
	/**
	 * Default Constructor
	 */
	KTorrent();

	/**
	 * Default Destructor
	 */
	virtual ~KTorrent();

	/// Open a view with the given group
	void openView(const TQString & group_name);

	/// Get the core
	KTorrentCore & getCore() {return *m_core;}
	
	/**
	 * Apply the settings.
	 * @param change_port Wether or not to change the server port
	 */
	void applySettings(bool change_port = true);

	virtual void addTabPage(TQWidget* page,const TQIconSet & icon,
							const TQString & caption,kt::CloseTabListener* ctl = 0);
	virtual void addTabPage(TQWidget* page,const TQPixmap & icon,
							const TQString & caption,kt::CloseTabListener* ctl = 0);
	virtual void removeTabPage(TQWidget* page);
	virtual void addPrefPage(kt::PrefPageInterface* page);
	virtual void removePrefPage(kt::PrefPageInterface* page);
	virtual void mergePluginGui(kt::Plugin* p);
	virtual void removePluginGui(kt::Plugin* p);
	virtual void addWidgetBelowView(TQWidget* w,const TQString & icon,const TQString & caption);
	virtual void removeWidgetBelowView(TQWidget* w);
	virtual void addToolWidget(TQWidget* w,const TQString & icon,const TQString & caption,ToolDock dock);
	virtual void removeToolWidget(TQWidget* w);
	virtual const kt::TorrentInterface* getCurrentTorrent() const;
	virtual TDEToolBar* addToolBar(const char* name);
	virtual void removeToolBar(TDEToolBar* tb);
	virtual KProgress* addProgressBarToStatusBar();
	virtual void removeProgressBarFromStatusBar(KProgress* p);
	
	TQString	getStatusInfo();
	TQString	getStatusTransfer();
	TQString	getStatusSpeed();
	TQString	getStatusDHT();
	TQString getStatusFirewall();
	QCStringList getTorrentInfo(kt::TorrentInterface* tc);

public slots:
	/**
	 * Use this method to load whatever file/URL you have
	 */
	void load(const KURL& url);
	
	/**
	 * Does the same as load, but doesn't ask any questions
	*/
	void loadSilently(const KURL& url);
	
	/**
	 * Does the same as loadSilently, except accepts a directory to
	 * save to
	 */
	void loadSilentlyDir(const KURL& url, const KURL& savedir);
	
	/// Open a view with the given group
	void openView(kt::Group* g);

protected:
	/**
	 * This function is called when it is time for the app to save its
	 * properties for session management purposes.
	 */
	void saveProperties(TDEConfig *);

	/**
	 * This function is called when this app is restored.  The TDEConfig
	 * object points to the session management config file that was saved
	 * with @ref saveProperties
	 */
	void readProperties(TDEConfig *);


private slots:
	void fileOpen();
	void fileNew();
	void torrentPaste();
	void startDownload();
	void startAllDownloadsCurrentView();
	void startAllDownloads();
	void stopDownload();
	void stopAllDownloadsCurrentView();
	void stopAllDownloads();
	void showIPFilter();
	void removeDownload();
	void queueManagerShow();
	void optionsShowStatusbar();
	void optionsShowMenubar();
	void optionsConfigureKeys();
	void optionsConfigureToolbars();
	void optionsPreferences();
	void newToolbarConfig();
	void changeStatusbar(const TQString& text);
	void changeCaption(const TQString& text);
	void currentTorrentChanged(kt::TorrentInterface* tc);
	void updatedStats();
	void urlDropped(TQDropEvent*,TQListViewItem*);
	void onUpdateActions(int flags);
	void groupChanged(kt::Group* g);
	void groupRenamed(kt::Group* g);
	void groupRemoved(kt::Group* g);
	void currentTabChanged(TQWidget* w);
	void openDefaultView();
	void statusBarMsgExpired();
	void find();
	
private:
	void setupAccel();
	void setupActions();
	bool queryClose();
	bool queryExit();
	
	
	virtual void addWidgetInView(TQWidget* w,kt::Position pos);
	virtual void removeWidgetFromView(TQWidget* w);	
	virtual void closeTab();
	
private:
	kt::GroupView* m_group_view;
	ViewManager* m_view_man;
	TDEToggleAction *m_statusbarAction;
	TDEToggleAction* m_menubarAction;
	
	KTorrentCore* m_core;
	TrayIcon* m_systray_icon;
	SetMaxRate* m_set_max_upload_rate;
	SetMaxRate* m_set_max_download_rate;
	
	KTorrentDCOP* m_dcop;
	TQTimer m_gui_update_timer;
	TQTimer m_status_msg_expire;
	KTorrentPreferences* m_pref;

	TQMap<TQWidget*,kt::CloseTabListener*> m_tab_map;

	TQLabel* m_statusInfo;
	TQLabel* m_statusTransfer;
	TQLabel* m_statusSpeed;
	TQLabel* m_statusDHT;
	TQLabel* m_statusFirewall;
	
	TDEAction* m_start;
	TDEAction* m_stop;
	TDEAction* m_remove;
	TDEAction* m_startall;
	TDEAction* m_startall_systray;
	TDEAction* m_stopall;
	TDEAction* m_stopall_systray;
	TDEAction* m_pasteurl;
	TDEAction* m_queuemgr; 
	TDEAction* m_queueaction;
	TDEAction* m_datacheck;
	TDEAction* m_ipfilter;
	TDEAction* m_find;
	
	KProgress* m_status_prog;
};

#endif // _KTORRENT_H_

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


#ifndef _KTORRENTPREF_H_
#define _KTORRENTPREF_H_

#include <kdialogbase.h>
#include <tqframe.h>
#include <tqmap.h> 
#include <interfaces/prefpageinterface.h>
 

class DownloadPref;
class GeneralPref;
class AdvancedPref;
class KTorrent;
class TQListViewItem; 


class DownloadPrefPage : public kt::PrefPageInterface
{
	DownloadPref* dp;
public:
	DownloadPrefPage();
	virtual ~DownloadPrefPage();
	
	virtual bool apply();
	virtual void updateData();	
	virtual void createWidget(TQWidget* parent);
	virtual void deleteWidget();
};

class GeneralPrefPage : public TQObject,public kt::PrefPageInterface
{
	TQ_OBJECT
	
	GeneralPref* gp;
public:
	GeneralPrefPage();
	virtual ~GeneralPrefPage();
	
	virtual bool apply();
	virtual void updateData();
	virtual void createWidget(TQWidget* parent);
	virtual void deleteWidget();
	
private slots:
	void autosaveChecked(bool on);
	void customIPChecked(bool on);
	void dhtChecked(bool on);
	void useEncryptionChecked(bool on);
};

class AdvancedPrefPage : public TQObject,public kt::PrefPageInterface
{
	TQ_OBJECT
	
	 
	AdvancedPref* ap;
public:
	AdvancedPrefPage();
	virtual ~AdvancedPrefPage();
	
	virtual bool apply();
	virtual void updateData();
	virtual void createWidget(TQWidget* parent);
	virtual void deleteWidget();
	
private slots:
	void noDataCheckChecked(bool on);
	void autoRecheckChecked(bool on);
	void doNotUseKDEProxyChecked(bool on);
	void preallocDisabledChecked(bool on);
};

 
class KTorrentPreferences : public KDialogBase
{
	TQ_OBJECT
	
public:
	KTorrentPreferences(KTorrent & ktor);
	virtual ~KTorrentPreferences();
	
	void updateData();
	void addPrefPage(kt::PrefPageInterface* prefInterface);
	void removePrefPage(kt::PrefPageInterface* prefInterface);
private:
	virtual void slotOk();
	virtual void slotApply();
	
	
private:
	KTorrent & ktor;
	DownloadPrefPage* page_one;
	GeneralPrefPage* page_two;
	AdvancedPrefPage* page_three;
	TQMap<kt::PrefPageInterface*,TQFrame*> pages;
	bool validation_err;
};





#endif // _KTORRENTPREF_H_

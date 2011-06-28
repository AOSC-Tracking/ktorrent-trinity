//
// C++ Interface: $MODULE$
//
// Description: 
//
//
// Author: Joris Guisson <joris.guisson@gmail.com>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TORENTCREATORDLG_H
#define TORENTCREATORDLG_H

#include "torrentcreatordlgbase.h"

class KTorrentCore;

class TorrentCreatorDlg: public TorrentCreatorDlgBase
{
	Q_OBJECT
  TQ_OBJECT
public:
	TorrentCreatorDlg(KTorrentCore* core,TQWidget *tqparent = 0, const char *name = 0);
	virtual ~TorrentCreatorDlg();

public slots:
	void onCreate();
    virtual void btnAddNode_clicked();
    virtual void btnRemoveNode_clicked();
    virtual void m_nodeList_selectionChanged(TQListViewItem*);
    virtual void m_node_textChanged(const TQString&);
private:
	void errorMsg(const TQString & text);

private:
	KTorrentCore* core;
};

#endif

/***************************************************************************
 *   Copyright (C) 2005 by Joris Guisson, Ivan Vasic                       *
 *   joris.guisson@gmail.com                                               *
 *   ivasic@gmail.com                                                      *
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

#include <kapplication.h>
#include <tdehtmlview.h>
#include <tqlayout.h>
#include <tqfile.h> 
#include <tqtextstream.h> 
#include <tqstring.h> 
#include <tqstringlist.h> 
#include <klineedit.h>
#include <kpushbutton.h>
#include <kglobal.h>
#include <klocale.h>
#include <kstandarddirs.h> 
#include <kiconloader.h>
#include <kcombobox.h>
#include <tdepopupmenu.h>
#include <tdeparts/partmanager.h>
#include <tdeio/job.h>
#include <kmessagebox.h>
#include <tdefiledialog.h>
#include <kprogress.h>
#include <util/log.h>
#include <torrent/globals.h>
#include <interfaces/guiinterface.h>
#include <interfaces/coreinterface.h>
#include "searchwidget.h"
#include "searchbar.h"
#include "htmlpart.h"
#include "searchplugin.h"
#include "searchenginelist.h"



using namespace bt;

namespace kt
{
	
	
	SearchWidget::SearchWidget(SearchPlugin* sp) : html_part(0),sp(sp)
	{
		TQVBoxLayout* layout = new TQVBoxLayout(this);
		layout->setAutoAdd(true);
		sbar = new SearchBar(this);
		html_part = new HTMLPart(this);
	
		right_click_menu = new TDEPopupMenu(this);
		right_click_menu->insertSeparator();
		back_id = right_click_menu->insertItem(
                        TDEGlobal::iconLoader()->loadIconSet(TQApplication::reverseLayout() 
                        ? "forward" : "back",TDEIcon::Small),
				i18n("Back"),html_part,TQT_SLOT(back()));
		right_click_menu->insertItem(
				TDEGlobal::iconLoader()->loadIconSet("reload",TDEIcon::Small),
				i18n("Reload"),html_part,TQT_SLOT(reload()));
	
		right_click_menu->setItemEnabled(back_id,false);
		sbar->m_back->setEnabled(false);
		connect(sbar->m_search_button,TQT_SIGNAL(clicked()),this,TQT_SLOT(searchPressed()));
		connect(sbar->m_clear_button,TQT_SIGNAL(clicked()),this,TQT_SLOT(clearPressed()));
		connect(sbar->m_search_text,TQT_SIGNAL(returnPressed()),this,TQT_SLOT(searchPressed()));
		connect(sbar->m_back,TQT_SIGNAL(clicked()),html_part,TQT_SLOT(back()));
		connect(sbar->m_reload,TQT_SIGNAL(clicked()),html_part,TQT_SLOT(reload()));
	
		sbar->m_clear_button->setIconSet(
				TDEGlobal::iconLoader()->loadIconSet(TQApplication::reverseLayout() 
                        ? "clear_left" : "locationbar_erase",TDEIcon::Small));
		sbar->m_back->setIconSet(
                        TDEGlobal::iconLoader()->loadIconSet(TQApplication::reverseLayout() 
                        ? "forward" : "back", TDEIcon::Small));
		sbar->m_reload->setIconSet(
				TDEGlobal::iconLoader()->loadIconSet("reload",TDEIcon::Small));
		
		
		connect(html_part,TQT_SIGNAL(backAvailable(bool )),
				this,TQT_SLOT(onBackAvailable(bool )));
		connect(html_part,TQT_SIGNAL(onURL(const TQString& )),
				this,TQT_SLOT(onURLHover(const TQString& )));
		connect(html_part,TQT_SIGNAL(openTorrent(const KURL& )),
				this,TQT_SLOT(onOpenTorrent(const KURL& )));
		connect(html_part,TQT_SIGNAL(popupMenu(const TQString&, const TQPoint& )),
				this,TQT_SLOT(showPopupMenu(const TQString&, const TQPoint& )));
		connect(html_part,TQT_SIGNAL(searchFinished()),this,TQT_SLOT(onFinished()));
		connect(html_part,TQT_SIGNAL(saveTorrent(const KURL& )),
				this,TQT_SLOT(onSaveTorrent(const KURL& )));
	
		KParts::PartManager* pman = html_part->partManager();
		connect(pman,TQT_SIGNAL(partAdded(KParts::Part*)),this,TQT_SLOT(onFrameAdded(KParts::Part* )));
		
		connect(html_part->browserExtension(),TQT_SIGNAL(loadingProgress(int)),this,TQT_SLOT(loadingProgress(int)));
		prog = 0;
	}
	
	
	SearchWidget::~SearchWidget()
	{
		if (prog)
		{
			sp->getGUI()->removeProgressBarFromStatusBar(prog);
			prog = 0;
		}
	}
	
	void SearchWidget::updateSearchEngines(const SearchEngineList & sl)
	{
		int ci = sbar->m_search_engine->currentItem(); 
		sbar->m_search_engine->clear();
		for (Uint32 i = 0;i < sl.getNumEngines();i++)
		{
			sbar->m_search_engine->insertItem(sl.getEngineName(i));
		}
		sbar->m_search_engine->setCurrentItem(ci);
	}
	
	void SearchWidget::onBackAvailable(bool available)
	{
		sbar->m_back->setEnabled(available);
		right_click_menu->setItemEnabled(back_id,available);
	}
	
	void SearchWidget::onFrameAdded(KParts::Part* p)
	{
		TDEHTMLPart* frame = dynamic_cast<TDEHTMLPart*>(p);
		if (frame)
		{
			connect(frame,TQT_SIGNAL(popupMenu(const TQString&, const TQPoint& )),
					this,TQT_SLOT(showPopupMenu(const TQString&, const TQPoint& )));
		}
	}
	
	void SearchWidget::copy()
	{
		if (!html_part)
			return;
		html_part->copy();
	}
	
	void SearchWidget::search(const TQString & text,int engine)
	{
		if (!html_part)
			return;
		
		if (sbar->m_search_text->text() != text)
			sbar->m_search_text->setText(text);
		
		if (sbar->m_search_engine->currentItem() != engine)
			sbar->m_search_engine->setCurrentItem(engine);
	
		const SearchEngineList & sl = sp->getSearchEngineList();
		
		if (engine < 0 || (Uint32)engine >= sl.getNumEngines())
			engine = sbar->m_search_engine->currentItem();
		
		TQString s_url = sl.getSearchURL(engine).prettyURL();
		s_url.replace("FOOBAR", KURL::encode_string(text), true);
		KURL url = KURL::fromPathOrURL(s_url);
	
		statusBarMsg(i18n("Searching for %1...").arg(text));
		//html_part->openURL(url);
 		html_part->openURLRequest(url,KParts::URLArgs());
	}	
	
	void SearchWidget::searchPressed()
	{
		search(sbar->m_search_text->text(),sbar->m_search_engine->currentItem());
	}
	
	void SearchWidget::clearPressed()
	{
		sbar->m_search_text->clear();
	}
	
	void SearchWidget::onURLHover(const TQString & url)
	{
		statusBarMsg(url);
	}
	
	void SearchWidget::onFinished()
	{
	}
	
	void SearchWidget::onOpenTorrent(const KURL & url)
	{
		openTorrent(url);
	}
	
	void SearchWidget::onSaveTorrent(const KURL & url)
	{
		KFileDialog fdlg(TQString(),"*.torrent | " + i18n("torrent files"),this,0,true);
		fdlg.setSelection(url.fileName());
		fdlg.setOperationMode(KFileDialog::Saving);
		if (fdlg.exec() == TQDialog::Accepted)
		{
			KURL save_url = fdlg.selectedURL();
			// start a copy job
			TDEIO::Job* j = TDEIO::file_copy(url,save_url,-1,true);
			// let it deal with the errors
			j->setAutoErrorHandlingEnabled(true,0);
		}
	}
	
	void SearchWidget::showPopupMenu(const TQString & url,const TQPoint & p)
	{
		right_click_menu->popup(p);
	}
	
	TDEPopupMenu* SearchWidget::rightClickMenu()
	{
		return right_click_menu;
	}
	
	void SearchWidget::onShutDown()
	{
		delete html_part;
		html_part = 0;
	}

	void SearchWidget::statusBarMsg(const TQString & url)
	{
		sp->getGUI()->changeStatusbar(url);
	}
	
	void SearchWidget::openTorrent(const KURL & url)
	{
		sp->getCore()->load(url);
	}
	
	void SearchWidget::loadingProgress(int perc)
	{
		if (perc < 100 && !prog)
		{
			prog = sp->getGUI()->addProgressBarToStatusBar();
			if (prog)
				prog->setValue(perc);
		}
		else if (prog && perc < 100)
		{
			prog->setValue(perc);
		}
		else if (perc == 100) 
		{
			if (prog)
			{
				sp->getGUI()->removeProgressBarFromStatusBar(prog);
				prog = 0;
			}
			statusBarMsg(i18n("Search finished"));
		}
	}
}

#include "searchwidget.moc"

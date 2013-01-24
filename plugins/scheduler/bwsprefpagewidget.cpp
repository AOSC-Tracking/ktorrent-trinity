/***************************************************************************
 *   Copyright (C) 2006 by Ivan Vasić                                      *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.           *
 ***************************************************************************/
#include "bwspage.h"
#include "bwsprefpagewidget.h"
#include "bwscheduler.h"
#include "bwswidget.h"
#include "schedulerpluginsettings.h"

#include <knuminput.h>
#include <klocale.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kiconloader.h>
#include <kstdguiitem.h>
#include <kpushbutton.h>

#include <tqcombobox.h>
#include <tqfile.h>
#include <tqdatastream.h>
#include <tqlabel.h>
#include <tqcheckbox.h>
#include <tqradiobutton.h>

namespace kt
{

	/* PREF PAGE WIDGET --------------------------------------------------------*/

	BWSPrefPageWidget::BWSPrefPageWidget(TQWidget* parent, const char* name, WFlags fl)
			: BWSPage(parent,name,fl)
	{

		loadDefault();

		lblStatus->clear();

		pix_icon->setPixmap(TDEGlobal::iconLoader()->loadIcon("clock",KIcon::NoGroup));
		
		btnOk->setGuiItem(KStdGuiItem::ok());
		btnCancel->setGuiItem(KStdGuiItem::cancel());
		btnApply->setGuiItem(KStdGuiItem::apply());
		btnSave->setGuiItem(KStdGuiItem::saveAs());
		btnLoad->setGuiItem(KStdGuiItem::open());
		btnReset->setGuiItem(KStdGuiItem::reset());
		

		bool use_colors = SchedulerPluginSettings::useColors();

		if(use_colors) //set up colors
		{
			pix1->setPaletteBackgroundColor(TQColor(30,165,105));
			pix2->setPaletteBackgroundColor(TQColor(195,195,70));
			pix3->setPaletteBackgroundColor(TQColor(195,195,70));
			pix4->setPaletteBackgroundColor(TQColor(195,195,70));
			pix5->setPaletteBackgroundColor(TQColor(190,30,30));
			
			pix12->setPaletteBackgroundColor(TQColor(30,165,105));
			pix22->setPaletteBackgroundColor(TQColor(195,195,70));
			pix32->setPaletteBackgroundColor(TQColor(195,195,70));
			pix42->setPaletteBackgroundColor(TQColor(195,195,70));
			pix52->setPaletteBackgroundColor(TQColor(190,30,30));
		} else //set up pixmaps
		{
			pix1->setPixmap(TQPixmap(locate("data", TQString("ktorrent/icons/cell-a-0000.png"))));
			pix2->setPixmap(TQPixmap(locate("data", TQString("ktorrent/icons/cell-a-0001.png"))));
			pix3->setPixmap(TQPixmap(locate("data", TQString("ktorrent/icons/cell-a-0002.png"))));
			pix4->setPixmap(TQPixmap(locate("data", TQString("ktorrent/icons/cell-a-0003.png"))));
			pix5->setPixmap(TQPixmap(locate("data", TQString("ktorrent/icons/cell-a-0004.png"))));

			pix12->setPixmap(TQPixmap(locate("data", TQString("ktorrent/icons/cell-a-0000.png"))));
			pix22->setPixmap(TQPixmap(locate("data", TQString("ktorrent/icons/cell-a-0001.png"))));
			pix32->setPixmap(TQPixmap(locate("data", TQString("ktorrent/icons/cell-a-0002.png"))));
			pix42->setPixmap(TQPixmap(locate("data", TQString("ktorrent/icons/cell-a-0003.png"))));
			pix52->setPixmap(TQPixmap(locate("data", TQString("ktorrent/icons/cell-a-0004.png"))));
		}

		connect(radio1, TQT_SIGNAL(stateChanged(int)), this, TQT_SLOT(categoryChanged(int)));
		connect(radio2, TQT_SIGNAL(stateChanged(int)), this, TQT_SLOT(categoryChanged(int)));
		connect(radio3, TQT_SIGNAL(stateChanged(int)), this, TQT_SLOT(categoryChanged(int)));
		connect(radio4, TQT_SIGNAL(stateChanged(int)), this, TQT_SLOT(categoryChanged(int)));
		connect(radio5, TQT_SIGNAL(stateChanged(int)), this, TQT_SLOT(categoryChanged(int)));
		connect(radio12, TQT_SIGNAL(stateChanged(int)), this, TQT_SLOT(categoryChanged(int)));
		connect(radio22, TQT_SIGNAL(stateChanged(int)), this, TQT_SLOT(categoryChanged(int)));
		connect(radio32, TQT_SIGNAL(stateChanged(int)), this, TQT_SLOT(categoryChanged(int)));
		connect(radio42, TQT_SIGNAL(stateChanged(int)), this, TQT_SLOT(categoryChanged(int)));
		connect(radio52, TQT_SIGNAL(stateChanged(int)), this, TQT_SLOT(categoryChanged(int)));

		//pre-check default categories (say 1 for left and 0 for right)
		radio2->setChecked(true);
		radio12->setChecked(true);

		m_bwsWidget->setSchedule(schedule);
	}

	BWSPrefPageWidget::~BWSPrefPageWidget()
	{}

	void BWSPrefPageWidget::btnSave_clicked()
	{
		TQString sf = KFileDialog::getSaveFileName("/home","*",this,i18n("Choose a filename to save under"));

		if(sf.isEmpty())
			return;

		saveSchedule(sf);
	}

	void BWSPrefPageWidget::btnLoad_clicked()
	{
		TQString lf = KFileDialog::getOpenFileName("/home", "*",this,i18n("Choose a file"));

		if(lf.isEmpty())
			return;

		btnReset_clicked();
		loadSchedule(lf);
	}

	void BWSPrefPageWidget::saveSchedule(TQString& fn)
	{
		schedule = m_bwsWidget->schedule();

		TQFile file(fn);

		file.open(IO_WriteOnly);
		TQDataStream stream(&file);

		//First category
		stream << dlCat1->value();
		stream << ulCat1->value();

		//Second category
		stream << dlCat2->value();
		stream << ulCat2->value();

		//Third category
		stream << dlCat3->value();
		stream << ulCat3->value();

		//Now schedule
		for(int i=0; i<7; ++i)
			for(int j=0; j<24; ++j)
				stream << (int) schedule.getCategory(i, j);

		file.close();
		lblStatus->setText(i18n("Schedule saved."));
	}

	void BWSPrefPageWidget::loadSchedule(TQString& fn, bool showmsg)
	{
		TQFile file(fn);

		if(!file.exists()) {
			if(showmsg)
				KMessageBox::error(this, i18n("File not found."), i18n("Error"));
			return;
		}

		file.open(IO_ReadOnly);
		TQDataStream stream(&file);

		int tmp;

		stream >> tmp;
		dlCat1->setValue(tmp);
		stream >> tmp;
		ulCat1->setValue(tmp);

		stream >> tmp;
		dlCat2->setValue(tmp);
		stream >> tmp;
		ulCat2->setValue(tmp);

		stream >> tmp;
		dlCat3->setValue(tmp);
		stream >> tmp;
		ulCat3->setValue(tmp);

		for(int i=0; i<7; ++i) {
			for(int j=0; j<24; ++j) {
				stream >> tmp;
				schedule.setCategory(i, j, (ScheduleCategory) tmp);
			}
		}

		file.close();

		m_bwsWidget->setSchedule(schedule);
		lblStatus->setText(i18n("Schedule loaded."));
	}

	void BWSPrefPageWidget::loadDefault()
	{
		//read schedule from HD
		TQString fn = TDEGlobal::dirs()->saveLocation("data","ktorrent") + "bwschedule";
		loadSchedule(fn, false);
	}

	void BWSPrefPageWidget::btnReset_clicked()
	{
		schedule.reset();
		m_bwsWidget->resetSchedule();

		dlCat1->setValue(0);
		dlCat2->setValue(0);
		dlCat3->setValue(0);

		ulCat1->setValue(0);
		ulCat2->setValue(0);
		ulCat3->setValue(0);

		lblStatus->clear();
	}

	void BWSPrefPageWidget::apply()
	{
		schedule = m_bwsWidget->schedule();

		SchedulerPluginSettings::writeConfig();

		//update category values...
		schedule.setDownload(0, dlCat1->value());
		schedule.setUpload(0, ulCat1->value());
		schedule.setDownload(1, dlCat2->value());
		schedule.setUpload(1, ulCat2->value());
		schedule.setDownload(2, dlCat3->value());
		schedule.setUpload(2, ulCat3->value());

		//set new schedule
		BWScheduler::instance().setSchedule(schedule);
	}

	void BWSPrefPageWidget::btnOk_clicked()
	{
		apply();
		accept();
	}

	void BWSPrefPageWidget::btnApply_clicked()
	{
		apply();
	}
}

void kt::BWSPrefPageWidget::categoryChanged(int)
{
	if(radio1->isChecked())
		m_bwsWidget->setLeftCategory(0);
	else if(radio2->isChecked())
		m_bwsWidget->setLeftCategory(1);
	else if(radio3->isChecked())
		m_bwsWidget->setLeftCategory(2);
	else if(radio4->isChecked())
		m_bwsWidget->setLeftCategory(3);
	else if(radio5->isChecked())
		m_bwsWidget->setLeftCategory(4);

	if(radio12->isChecked())
		m_bwsWidget->setRightCategory(0);
	else if(radio22->isChecked())
		m_bwsWidget->setRightCategory(1);
	else if(radio32->isChecked())
		m_bwsWidget->setRightCategory(2);
	else if(radio42->isChecked())
		m_bwsWidget->setRightCategory(3);
	else if(radio52->isChecked())
		m_bwsWidget->setRightCategory(4);
}



#include "bwsprefpagewidget.moc"

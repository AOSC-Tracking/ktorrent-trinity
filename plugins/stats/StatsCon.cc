/***************************************************************************
 *   Copyright © 2007 by Krzysztof Kundzicz                                *
 *   athantor@gmail.com                                                    *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "StatsCon.h"

namespace kt {

StatsCon::StatsCon(TQWidget * p) :  StatsConWgt(p), pmPeersConCht(new ChartDrawer(PeersConGbw, StatsPluginSettings::connectionsMeasurements())), 
				pmDHTCht(new ChartDrawer(DHTGbw, StatsPluginSettings::dHTMeasurements()))
{
	PeersConGbw->setColumnLayout(0, Qt::Vertical );
	PeersConGbw->layout()->setSpacing( 6 );
	PeersConGbw->layout()->setMargin( 11 );
	
	pmPeersConLay = new TQVBoxLayout(PeersConGbw -> layout());
	
	DHTGbw->setColumnLayout(0, Qt::Vertical );
	DHTGbw->layout()->setSpacing( 6 );
	DHTGbw->layout()->setMargin( 11 );
	
	pmDHTLay = new TQVBoxLayout(DHTGbw -> layout());
	
	//-------------
	
	pmPeersConLay -> addWidget(pmPeersConCht);
	pmDHTLay -> addWidget(pmDHTCht);
	
	//-----------
	
	pmPeersConCht -> SetUnitName("n");
	
	pmPeersConCht -> AddValuesCnt(TQPen("#f00"), i18n("Leechers connected"));
	pmPeersConCht -> AddValuesCnt(TQPen("#900"), i18n("Leechers in swarms"));
	pmPeersConCht -> AddValuesCnt(TQPen("#00f"), i18n("Seeders connected"));
	pmPeersConCht -> AddValuesCnt(TQPen("#009"), i18n("Seeders in swarms"));
	pmPeersConCht -> AddValuesCnt(TQPen("#0a0"), i18n("Average connected leechers per torrent"));
	pmPeersConCht -> AddValuesCnt(TQPen("#060"), i18n("Average connected seeders per torrent"));
 	pmPeersConCht -> AddValuesCnt(TQPen("#099"), i18n("Average connected leechers per running torrent"));
 	pmPeersConCht -> AddValuesCnt(TQPen("#055"), i18n("Average connected seeders per running torrent"));

	
	pmDHTCht -> SetUnitName("n");
	
	pmDHTCht -> AddValuesCnt(TQPen("#f00"), i18n("Nodes"));
	pmDHTCht -> AddValuesCnt(TQPen("#00f"), i18n("Tasks"));
}

StatsCon::~StatsCon()
{
	delete pmPeersConCht;
	delete pmDHTCht;
	
	delete pmPeersConLay;
	delete pmDHTLay;
}

void StatsCon::AddPeersConVal(const size_t idx, const double val)
{
	pmPeersConCht -> AddValue(idx, val , false );
}

void StatsCon::AddDHTVal(const size_t idx, const double val)
{
	pmDHTCht -> AddValue(idx, val, false);
}

void StatsCon::UpdateCharts()
{
	pmPeersConCht -> update();
	pmDHTCht -> update();
}

void StatsCon::ZeroPeersConn(const size_t idx)
{
	pmPeersConCht -> Zero(idx);
}

void  StatsCon::ChangeConnMsmtCnt(const size_t cnt)
{
	pmPeersConCht -> SetXMax(cnt);
}

void  StatsCon::ChangeDHTMsmtCnt(const size_t cnt)
{
	pmDHTCht -> SetXMax(cnt);
}

void StatsCon::ChangeChartsMaxMode(const ChartDrawer::MaxMode mm)
{
	pmPeersConCht -> SetMaxMode(mm);
	pmDHTCht -> SetMaxMode(mm);
}

} //NS

#include "StatsCon.moc"

/***************************************************************************
 *   Copyright (C) 2005 by                                                 *
 *   Joris Guisson <joris.guisson@gmail.com>                               *
 *   Vincent Wagelaar <vincent@ricardis.tudelft.nl>                        *
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
#include <tqpainter.h>
#include <tqpen.h>
#include <tqbrush.h>
#include <tqvaluelist.h>
#include <tqpixmap.h>
#include <math.h>
#include <tqtooltip.h>
#include <tdelocale.h>
#include <tqmime.h>
#include <tqimage.h>
#include <util/log.h>
#include <interfaces/torrentinterface.h>
#include <util/bitset.h>
#include <torrent/globals.h>
#include "chunkbar.h"

using namespace bt;
using namespace kt;

namespace kt
{
		
	struct Range
	{
		int first,last;
		int fac;
	};
	
	
	static void FillAndFrameBlack(TQImage* image, uint color, int size)
	{
		image->fill(color);
		for (int i = 0; i < size; i++)
		{
			image->setPixel(0, i, 0);
			image->setPixel(size - 1, i, 0);
			image->setPixel(i, 0, 0);
			image->setPixel(i, size - 1, 0);
		}
	}
	
	
	
	static void InitializeToolTipImages(ChunkBar* bar)
	{
		static bool images_initialized = false;
		if (images_initialized)
			return;
		images_initialized = true;
		
		TQMimeSourceFactory* factory = TQMimeSourceFactory::defaultFactory();
		
		TQImage excluded(16, 16, 32);
		FillAndFrameBlack(&excluded, TQColor(bar->colorGroup().color(TQColorGroup::Mid)).pixel(), 16);
		factory->setImage("excluded_color", excluded);
		
		TQImage available(16, 16, 32);
		FillAndFrameBlack(&available, bar->colorGroup().highlight().pixel(), 16);
		factory->setImage("available_color", available);
		
		TQImage unavailable(16, 16, 32);
		FillAndFrameBlack(&unavailable, bar->colorGroup().base().pixel(), 16);
		factory->setImage("unavailable_color", unavailable);
	}
	
	ChunkBar::ChunkBar(TQWidget *parent, const char *name)
		: TQFrame(parent, name),curr_tc(0)
	{
		setFrameShape(StyledPanel);
		setFrameShadow(Sunken);
		setLineWidth(3);
		setMidLineWidth(3);
		
		show_excluded = false;
		
		InitializeToolTipImages(this);
		
		TQToolTip::add(this, i18n("<img src=\"available_color\">&nbsp; - Downloaded Chunks<br><img src=\"unavailable_color\">&nbsp; - Chunks to Download<br><img src=\"excluded_color\">&nbsp; - Excluded Chunks"));
		
	}
	
	
	ChunkBar::~ChunkBar()
	{}
	
	void ChunkBar::updateBar()
	{
		const BitSet & bs = getBitSet();
		TQSize s = contentsRect().size();
		bool changed = !(curr == bs);
		if (show_excluded && curr_tc)
		{
			BitSet ebs = curr_tc->excludedChunksBitSet();
			ebs.orBitSet(curr_tc->onlySeedChunksBitSet()),
			changed = changed || !(curr_ebs == ebs);
			curr_ebs = ebs;
		}
		
		if (changed || pixmap.isNull() || pixmap.width() != s.width())
		{
		//	PROFILE("ChunkBar::updateBar");
		//	Out() << "Pixmap : " << s.width() << " " << s.height() << endl;
			pixmap.resize(s);
			pixmap.fill(colorGroup().color(TQColorGroup::Base));
			TQPainter painter(&pixmap);
			drawBarContents(&painter);
			update();
		}
	}
	
	void ChunkBar::drawContents(TQPainter *p)
	{
		// first draw background
		if (isEnabled())
			p->setBrush(colorGroup().base());
		else
			p->setBrush(colorGroup().background());
	
		p->setPen(TQt::NoPen);
		p->drawRect(contentsRect());
		if (isEnabled())
			p->drawPixmap(contentsRect(),pixmap);
	}
	
	void ChunkBar::setTC(kt::TorrentInterface* tc)
	{
		curr_tc = tc;
		TQSize s = contentsRect().size();
		//Out() << "Pixmap : " << s.width() << " " << s.height() << endl;
		pixmap.resize(s);
		pixmap.fill(colorGroup().color(TQColorGroup::Base));
		TQPainter painter(&pixmap);
		drawBarContents(&painter);
		update();
	}
	
	void ChunkBar::drawBarContents(TQPainter *p)
	{
		p->saveWorldMatrix();
		if (curr_tc)
		{
			const TorrentStats & s = curr_tc->getStats();
			Uint32 w = contentsRect().width();
			const BitSet & bs = getBitSet();
			curr = bs;
			if (bs.allOn())
				drawAllOn(p,colorGroup().highlight());
			else if (s.total_chunks > w)
				drawMoreChunksThenPixels(p,bs,colorGroup().highlight());
			else
				drawEqual(p,bs,colorGroup().highlight());
	
			if (show_excluded && s.num_chunks_excluded > 0)
			{
				TQColor c = colorGroup().color(TQColorGroup::Mid);
				if (curr_ebs.allOn())
					drawAllOn(p,c);
				else if (s.total_chunks > w)
					drawMoreChunksThenPixels(p,curr_ebs,c);
				else
					drawEqual(p,curr_ebs,c);
			}
		}
		p->restoreWorldMatrix();
	}
	
	void ChunkBar::drawEqual(TQPainter *p,const BitSet & bs,const TQColor & color)
	{
		//p->setPen(TQPen(colorGroup().highlight(),1,TQt::SolidLine));
		TQColor c = color;
	
		Uint32 w = contentsRect().width();
		double scale = 1.0;
		Uint32 total_chunks = curr_tc->getStats().total_chunks;
		if (curr_tc->getStats().total_chunks != w)
			scale = (double)w / total_chunks;
		
		p->setPen(TQPen(c,1,TQt::SolidLine));
		p->setBrush(c);
		
		TQValueList<Range> rs;
		
		for (Uint32 i = 0;i < bs.getNumBits();i++)
		{
			if (!bs.get(i))
				continue;
			
			if (rs.empty())
			{
				Range r = {(int)i,(int)i,0};
				rs.append(r);
			}
			else
			{
				Range & l = rs.last();
				if (l.last == int(i - 1))
				{
					l.last = i;
				}
				else
				{
					Range r = {(int)i,(int)i,0};
					rs.append(r);
				}
			}
		}
	
		TQRect r = contentsRect();
	
		for (TQValueList<Range>::iterator i = rs.begin();i != rs.end();++i)
		{
			Range & ra = *i;
			int rw = ra.last - ra.first + 1;
			p->drawRect((int)(scale * ra.first),0,(int)(rw * scale),r.height());
		}
	}
	
	void ChunkBar::drawMoreChunksThenPixels(TQPainter *p,const BitSet & bs,const TQColor & color)
	{
		Uint32 w = contentsRect().width(); 
		double chunks_per_pixel = (double)bs.getNumBits() / w;
		TQValueList<Range> rs; 

		for (Uint32 i = 0;i < w;i++) 
		{ 
			Uint32 num_dl = 0; 
			Uint32 jStart = (Uint32) (i*chunks_per_pixel); 
			Uint32 jEnd = (Uint32) ((i+1)*chunks_per_pixel+0.5); 
			for (Uint32 j = jStart;j < jEnd;j++) 
				if (bs.get(j)) 
					num_dl++; 
	
			if (num_dl == 0)
				continue;
	
			int fac = int(100*((double)num_dl / (jEnd - jStart)) + 0.5);
			if (rs.empty())
			{
				Range r = {(int)i,(int)i,fac};
				rs.append(r);
			}
			else
			{
				Range & l = rs.last();
				if (l.last == int(i - 1) && l.fac == fac)
				{
					l.last = i;
				}
				else
				{
					Range r = {(int)i,(int)i,fac};
					rs.append(r);
				}
			}
		}
	
		TQRect r = contentsRect();
	
		for (TQValueList<Range>::iterator i = rs.begin();i != rs.end();++i)
		{
			Range & ra = *i;
			int rw = ra.last - ra.first + 1;
			int fac = ra.fac;
			TQColor c = color;
			if (fac < 100)
			{
				// do some rounding off
				if (fac <= 25)
					fac = 25;
				else if (fac <= 50)
					fac = 45;
				else 
					fac = 65;
				c = color.light(200-fac);
			} 
			p->setPen(TQPen(c,1,TQt::SolidLine));
			p->setBrush(c);
			p->drawRect(ra.first,0,rw,r.height());
		}
		
	}
	
	void ChunkBar::drawAllOn(TQPainter *p,const TQColor & color)
	{
		p->setPen(TQPen(color,1,TQt::SolidLine));
		p->setBrush(color);
		TQSize s = contentsRect().size();
		p->drawRect(0,0,s.width(),s.height());
	}
}

#include "chunkbar.moc"

/*
 * article.cpp
 *
 * Copyright (c) 2001, 2002, 2003, 2004 Frerich Raabe <raabe@kde.org>
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. For licensing and distribution details, check the
 * accompanying file 'COPYING'.
 */
#include "article.h"
#include "tools_p.h"

#include <kdebug.h>
#include <krfcdate.h>
#include <kurl.h>
#include <kurllabel.h>
#include <kmdcodec.h> 

#include <tqdatetime.h>
#include <tqdom.h>

using namespace RSS;
namespace RSS
{
    KMD5 md5Machine;
}

struct Article::Private : public Shared
{
	TQString title;
	KURL link;
	TQString description;
	TQDateTime pubDate;
	TQString guid;
	bool guidIsPermaLink;
    MetaInfoMap meta;
	KURL commentsLink;
	int numComments;
};

Article::Article() : d(new Private)
{
}

Article::Article(const Article &other) : d(0)
{
	*this = other;
}

Article::Article(const TQDomNode &node, Format format) : d(new Private)
{
	TQString elemText;

	d->numComments=0;

	if (!(elemText = extractNode(node, TQString::tqfromLatin1("title"))).isNull())
		d->title = elemText;
   

	TQDomNode n;
	bool foundTorrentEnclosure = false;
	for (n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
		const TQDomElement e = n.toElement();
		if ( (e.tagName()==TQString::tqfromLatin1("enclosure") ) )
			{
			TQString enclosureAttr = e.attribute(TQString::tqfromLatin1("type"));
			if (!enclosureAttr.isNull() )
				{
				if (enclosureAttr == "application/x-bittorrent")
					{
					enclosureAttr = e.attribute(TQString::tqfromLatin1("url"));
					if (!enclosureAttr.isNull() )
						{
						d->link=enclosureAttr;
						foundTorrentEnclosure = true;
						break;
						}
					}
				}
			}
		}

	if (!foundTorrentEnclosure)
		{
		if (format==AtomFeed)
		{
			TQDomNode n;
			for (n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
				const TQDomElement e = n.toElement();
				if ( (e.tagName()==TQString::tqfromLatin1("link")) &&
					(e.attribute(TQString::tqfromLatin1("rel"))==TQString::tqfromLatin1("alternate")))
					{   
						d->link=n.toElement().attribute(TQString::tqfromLatin1("href"));
						break;
					}
			}
		}
		else
		{
			if (!(elemText = extractNode(node, TQString::tqfromLatin1("link"))).isNull())
				d->link = elemText;
		}
	}


    // prefer content/content:encoded over summary/description for feeds that provide it
    TQString tagName=(format==AtomFeed)? TQString::tqfromLatin1("content"): TQString::tqfromLatin1("content:encoded");
    
    if (!(elemText = extractNode(node, tagName, false)).isNull())
        d->description = elemText;
    
    if (d->description.isEmpty())
    {
		if (!(elemText = extractNode(node, TQString::tqfromLatin1("body"), false)).isNull())
	    	d->description = elemText;
    
		if (d->description.isEmpty())  // 3rd try: see http://www.intertwingly.net/blog/1299.html
		{
			if (!(elemText = extractNode(node, TQString::tqfromLatin1((format==AtomFeed)? "summary" : "description"), false)).isNull())
				d->description = elemText;
		}
    }
    
	if (!(elemText = extractNode(node, TQString::tqfromLatin1((format==AtomFeed)? "created": "pubDate"))).isNull())
    {
		time_t _time;
		if (format==AtomFeed)
		   _time = parseISO8601Date(elemText); 
		else
		   _time = KRFCDate::parseDate(elemText);

        // 0 means invalid, not epoch (it returns epoch+1 when it parsed epoch, see the KRFCDate::parseDate() docs)
        if (_time != 0)
		  d->pubDate.setTime_t(_time);
	}
	if (!(elemText = extractNode(node, TQString::tqfromLatin1("dc:date"))).isNull())
    {
		time_t _time = parseISO8601Date(elemText);

        // 0 means invalid, not epoch (it returns epoch+1 when it parsed epoch, see the KRFCDate::parseDate() docs)
        if (_time != 0)
		  d->pubDate.setTime_t(_time);
	}

	//no luck so far - so let's set it to the current time
	if (!d->pubDate.isValid())
	{
		d->pubDate = TQDateTime::tqcurrentDateTime();
	}
	

	if (!(elemText = extractNode(node, TQString::tqfromLatin1("wfw:comment"))).isNull()) {
		d->commentsLink = elemText;
	}

    if (!(elemText = extractNode(node, TQString::tqfromLatin1("slash:comments"))).isNull()) {
        d->numComments = elemText.toInt();
    }

    tagName=(format==AtomFeed)? TQString::tqfromLatin1("id"): TQString::tqfromLatin1("guid");
    n = node.namedItem(tagName);
	if (!n.isNull()) {
		d->guidIsPermaLink = (format==AtomFeed)? false : true;
		if (n.toElement().attribute(TQString::tqfromLatin1("isPermaLink"), "true") == "false") d->guidIsPermaLink = false;

		if (!(elemText = extractNode(node, tagName)).isNull())
			d->guid = elemText;
	}

	if(d->guid.isEmpty()) {
		d->guidIsPermaLink = false;
        
		md5Machine.reset();
		TQDomNode n(node);
		md5Machine.update(d->title.utf8());
		md5Machine.update(d->description.utf8());
		d->guid = TQString(md5Machine.hexDigest().data());
        d->meta[TQString::tqfromLatin1("guidIsHash")] = TQString::tqfromLatin1("true");
	}

    for (TQDomNode i = node.firstChild(); !i.isNull(); i = i.nextSibling())
    {
        if (i.isElement() && i.toElement().tagName() == TQString::tqfromLatin1("metaInfo:meta"))
        {
            TQString type = i.toElement().attribute(TQString::tqfromLatin1("type"));
            d->meta[type] = i.toElement().text();
        }
    }
}

Article::~Article()
{
	if (d->deref())
		delete d;
}

TQString Article::title() const
{
	return d->title;
}

const KURL &Article::link() const
{
	return d->link;
}

TQString Article::description() const
{
	return d->description;
}

TQString Article::guid() const
{
	return d->guid;
}

bool Article::guidIsPermaLink() const
{
	return d->guidIsPermaLink;
}

const TQDateTime &Article::pubDate() const
{
	return d->pubDate;
}

const KURL &Article::commentsLink() const
{
	return d->commentsLink;
}

int Article::comments() const
{
	return d->numComments;
}


TQString Article::meta(const TQString &key) const
{
    return d->meta[key];
}

KURLLabel *Article::widget(TQWidget *parent, const char *name) const
{
	KURLLabel *label = new KURLLabel(d->link.url(), d->title, parent, name);
	label->setUseTips(true);
	if (!d->description.isNull())
		label->setTipText(d->description);
	
	return label;
}

Article &Article::operator=(const Article &other)
{
	if (this != &other) {
		other.d->ref();
		if (d && d->deref())
			delete d;
		d = other.d;
	}
	return *this;
}

bool Article::operator==(const Article &other) const
{
	return d->guid == other.guid();
}

// vim:noet:ts=4

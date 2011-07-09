/*
 * document.cpp
 *
 * Copyright (c) 2001, 2002, 2003 Frerich Raabe <raabe@kde.org>
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. For licensing and distribution details, check the
 * accompanying file 'COPYING'.
 *
 */
#include "document.h"
#include "article.h"
#include "image.h"
#include "textinput.h"
#include "tools_p.h"

#include <krfcdate.h>
#include <kurl.h>

#include <tqdatetime.h>
#include <tqdom.h>
#include <tqptrlist.h>

using namespace RSS;

struct Document::Private : public Shared
{
    Private() : version(v0_90), image(NULL), textInput(NULL), language(en)
    {
        format=UnknownFormat;
        valid=false;
		ttl=-1;
    }

    ~Private()
    {
        delete textInput;
        delete image;
    }

    Version version;
    TQString title;
    TQString description;
    KURL link;
    Image *image;
    TextInput *textInput;
    Article::List articles;
    Language language;
    Format format;
    TQString copyright;
    TQDateTime pubDate;
    TQDateTime lastBuildDate;
    TQString rating;
    KURL docs;
	int ttl;
    TQString managingEditor;
    TQString webMaster;
    HourList skipHours;
    DayList skipDays;
    bool valid;
};

Document::Document() : d(new Private)
{
}

Document::Document(const Document &other) : d(0)
{
    *this = other;
}

Document::Document(const TQDomDocument &doc) : d(new Private)
{
    TQString elemText;
    TQDomNode rootNode = doc.documentElement();

    // Determine the version of the present RSS markup.
    TQString attr;

    // we should probably check that it ISN'T feed or rss, rather than check if it is xhtml
    if (rootNode.toElement().tagName()==TQString::tqfromLatin1("html"))
        d->valid=false;
    else
        d->valid=true;
    
    attr = rootNode.toElement().attribute(TQString::tqfromLatin1("version"), TQString());
    if (!attr.isNull()) {
        if (rootNode.toElement().tagName()=="feed")
        {
            d->format=AtomFeed;
            if (attr == TQString::tqfromLatin1("0.3"))
                d->version = vAtom_0_3;
            else if (attr == TQString::tqfromLatin1("0.2")) /* smt -> review */
               d->version = vAtom_0_2;
            else if (attr == TQString::tqfromLatin1("0.1")) /* smt -> review */
              d->version = vAtom_0_1;
        }
        else
        {
            d->format=RSSFeed;
            if (attr == TQString::tqfromLatin1("0.91"))
                d->version = v0_91;
            else if (attr == TQString::tqfromLatin1("0.92"))
                d->version = v0_92;
            else if (attr == TQString::tqfromLatin1("0.93"))
                d->version = v0_93;
            else if (attr == TQString::tqfromLatin1("0.94"))
                d->version = v0_94;
            else if (attr.startsWith("2.0") || attr == TQString::tqfromLatin1("2")) // http://www.breuls.org/rss puts 2.00 in version (BR #0000016)
                d->version = v2_0;
        }
    }

    if (d->format==UnknownFormat)
    {
        attr = rootNode.toElement().attribute(TQString::tqfromLatin1("xmlns"), TQString());
        if (!attr.isNull()) {
        /*
         * Hardcoding these URLs is actually a bad idea, since the DTD doesn't
         * dictate a specific namespace. Still, most RSS files seem to use
         * these two, so I'll go for them now. If it turns out that many
         * mirrors of this RSS namespace are in use, I'll probably have to
         * distinguish the RSS versions by analyzing the relationship between
         * the nodes.
         */
            if (attr == TQString::tqfromLatin1("http://my.netscape.com/rdf/simple/0.9/")) {
                d->format=RSSFeed;
                d->version = v0_90;
             }
            else if (attr == TQString::tqfromLatin1("http://purl.org/rss/1.0/")) {
                d->format=RSSFeed;
                d->version = v1_0;
            }
        }
    }

    TQDomNode channelNode;

    if (d->format == AtomFeed)
        channelNode=rootNode;
    else
        channelNode=rootNode.namedItem(TQString::tqfromLatin1("channel"));

    if (!(elemText = extractNode(channelNode, TQString::tqfromLatin1("title"))).isNull())
        d->title = elemText;
    if (!(elemText = extractNode(channelNode, TQString::tqfromLatin1("description"))).isNull())
        d->description = elemText;
    if (!(elemText = extractNode(channelNode, TQString::tqfromLatin1("link"))).isNull())
        d->link = elemText;

    
    /* This is ugly but necessary since RSS 0.90 and 1.0 have a different tqparent
     * node for <image>, <textinput> and <item> than RSS 0.91-0.94 and RSS 2.0.
     */
    TQDomNode parentNode;
    if (d->version == v0_90 || d->version == v1_0 || d->format == AtomFeed)
        parentNode = rootNode;
    else
    {
	// following is a HACK for broken 0.91 feeds like xanga.com's
	if (!rootNode.namedItem(TQString::tqfromLatin1("item")).isNull())
	    parentNode = rootNode;
	else
            parentNode = channelNode;
    }
    
    // image and textinput aren't supported by Atom.. handle in case feed provides
    TQDomNode n = parentNode.namedItem(TQString::tqfromLatin1("image"));
    if (!n.isNull())
        d->image = new Image(n);

    n = parentNode.namedItem(TQString::tqfromLatin1("textinput"));
    if (!n.isNull())
        d->textInput = new TextInput(n);

    // Our (hopefully faster) version of elementsByTagName()
    TQString tagName;
    if (d->format == AtomFeed)
        tagName=TQString::tqfromLatin1("entry");
    else
        tagName=TQString::tqfromLatin1("item");

    for (n = parentNode.firstChild(); !n.isNull(); n = n.nextSibling()) {
        const TQDomElement e = n.toElement();
        if (e.tagName() == tagName)
            d->articles.append(Article(e, d->format));
    }

    if (!(elemText = extractNode(channelNode, TQString::tqfromLatin1("copyright"))).isNull())
        d->copyright = elemText;

    if (d->format == AtomFeed)
        elemText = rootNode.toElement().attribute(TQString::tqfromLatin1("xml:lang"), TQString());
    else
        elemText = extractNode(channelNode, TQString::tqfromLatin1("language"));

    if (!elemText.isNull()){
        if (elemText == TQString::tqfromLatin1("af"))
            d->language = af;
        else if (elemText == TQString::tqfromLatin1("sq"))
            d->language = sq;
        else if (elemText == TQString::tqfromLatin1("eu"))
            d->language = eu;
        else if (elemText == TQString::tqfromLatin1("be"))
            d->language = be;
        else if (elemText == TQString::tqfromLatin1("bg"))
            d->language = bg;
        else if (elemText == TQString::tqfromLatin1("ca"))
            d->language = ca;
        else if (elemText == TQString::tqfromLatin1("zh-cn"))
            d->language = zh_cn;
        else if (elemText == TQString::tqfromLatin1("zh-tw"))
            d->language = zh_tw;
        else if (elemText == TQString::tqfromLatin1("hr"))
            d->language = hr;
        else if (elemText == TQString::tqfromLatin1("cs"))
            d->language = cs;
        else if (elemText == TQString::tqfromLatin1("da"))
            d->language = da;
        else if (elemText == TQString::tqfromLatin1("nl"))
            d->language = nl;
        else if (elemText == TQString::tqfromLatin1("nl-be"))
            d->language = nl_be;
        else if (elemText == TQString::tqfromLatin1("nl-nl"))
            d->language = nl_nl;
        else if (elemText == TQString::tqfromLatin1("en"))
            d->language = en;
        else if (elemText == TQString::tqfromLatin1("en-au"))
            d->language = en_au;
        else if (elemText == TQString::tqfromLatin1("en-bz"))
            d->language = en_bz;
        else if (elemText == TQString::tqfromLatin1("en-ca"))
            d->language = en_ca;
        else if (elemText == TQString::tqfromLatin1("en-ie"))
            d->language = en_ie;
        else if (elemText == TQString::tqfromLatin1("en-jm"))
            d->language = en_jm;
        else if (elemText == TQString::tqfromLatin1("en-nz"))
            d->language = en_nz;
        else if (elemText == TQString::tqfromLatin1("en-ph"))
            d->language = en_ph;
        else if (elemText == TQString::tqfromLatin1("en-za"))
            d->language = en_za;
        else if (elemText == TQString::tqfromLatin1("en-tt"))
            d->language = en_tt;
        else if (elemText == TQString::tqfromLatin1("en-gb"))
            d->language = en_gb;
        else if (elemText == TQString::tqfromLatin1("en-us"))
            d->language = en_us;
        else if (elemText == TQString::tqfromLatin1("en-zw"))
            d->language = en_zw;
        else if (elemText == TQString::tqfromLatin1("fo"))
            d->language = fo;
        else if (elemText == TQString::tqfromLatin1("fi"))
            d->language = fi;
        else if (elemText == TQString::tqfromLatin1("fr"))
            d->language = fr;
        else if (elemText == TQString::tqfromLatin1("fr-be"))
            d->language = fr_be;
        else if (elemText == TQString::tqfromLatin1("fr-ca"))
            d->language = fr_ca;
        else if (elemText == TQString::tqfromLatin1("fr-fr"))
            d->language = fr_fr;
        else if (elemText == TQString::tqfromLatin1("fr-lu"))
            d->language = fr_lu;
        else if (elemText == TQString::tqfromLatin1("fr-mc"))
            d->language = fr_mc;
        else if (elemText == TQString::tqfromLatin1("fr-ch"))
            d->language = fr_ch;
        else if (elemText == TQString::tqfromLatin1("gl"))
            d->language = gl;
        else if (elemText == TQString::tqfromLatin1("gd"))
            d->language = gd;
        else if (elemText == TQString::tqfromLatin1("de"))
            d->language = de;
        else if (elemText == TQString::tqfromLatin1("de-at"))
            d->language = de_at;
        else if (elemText == TQString::tqfromLatin1("de-de"))
            d->language = de_de;
        else if (elemText == TQString::tqfromLatin1("de-li"))
            d->language = de_li;
        else if (elemText == TQString::tqfromLatin1("de-lu"))
            d->language = de_lu;
        else if (elemText == TQString::tqfromLatin1("de-ch"))
            d->language = de_ch;
        else if (elemText == TQString::tqfromLatin1("el"))
            d->language = el;
        else if (elemText == TQString::tqfromLatin1("hu"))
            d->language = hu;
        else if (elemText == TQString::tqfromLatin1("is"))
            d->language = is;
        else if (elemText == TQString::tqfromLatin1("id"))
            d->language = id;
        else if (elemText == TQString::tqfromLatin1("ga"))
            d->language = ga;
        else if (elemText == TQString::tqfromLatin1("it"))
            d->language = it;
        else if (elemText == TQString::tqfromLatin1("it-it"))
            d->language = it_it;
        else if (elemText == TQString::tqfromLatin1("it-ch"))
            d->language = it_ch;
        else if (elemText == TQString::tqfromLatin1("ja"))
            d->language = ja;
        else if (elemText == TQString::tqfromLatin1("ko"))
            d->language = ko;
        else if (elemText == TQString::tqfromLatin1("mk"))
            d->language = mk;
        else if (elemText == TQString::tqfromLatin1("no"))
            d->language = no;
        else if (elemText == TQString::tqfromLatin1("pl"))
            d->language = pl;
        else if (elemText == TQString::tqfromLatin1("pt"))
            d->language = pt;
        else if (elemText == TQString::tqfromLatin1("pt-br"))
            d->language = pt_br;
        else if (elemText == TQString::tqfromLatin1("pt-pt"))
            d->language = pt_pt;
        else if (elemText == TQString::tqfromLatin1("ro"))
            d->language = ro;
        else if (elemText == TQString::tqfromLatin1("ro-mo"))
            d->language = ro_mo;
        else if (elemText == TQString::tqfromLatin1("ro-ro"))
            d->language = ro_ro;
        else if (elemText == TQString::tqfromLatin1("ru"))
            d->language = ru;
        else if (elemText == TQString::tqfromLatin1("ru-mo"))
            d->language = ru_mo;
        else if (elemText == TQString::tqfromLatin1("ru-ru"))
            d->language = ru_ru;
        else if (elemText == TQString::tqfromLatin1("sr"))
            d->language = sr;
        else if (elemText == TQString::tqfromLatin1("sk"))
            d->language = sk;
        else if (elemText == TQString::tqfromLatin1("sl"))
            d->language = sl;
        else if (elemText == TQString::tqfromLatin1("es"))
            d->language = es;
        else if (elemText == TQString::tqfromLatin1("es-ar"))
            d->language = es_ar;
        else if (elemText == TQString::tqfromLatin1("es-bo"))
            d->language = es_bo;
        else if (elemText == TQString::tqfromLatin1("es-cl"))
            d->language = es_cl;
        else if (elemText == TQString::tqfromLatin1("es-co"))
            d->language = es_co;
        else if (elemText == TQString::tqfromLatin1("es-cr"))
            d->language = es_cr;
        else if (elemText == TQString::tqfromLatin1("es-do"))
            d->language = es_do;
        else if (elemText == TQString::tqfromLatin1("es-ec"))
            d->language = es_ec;
        else if (elemText == TQString::tqfromLatin1("es-sv"))
            d->language = es_sv;
        else if (elemText == TQString::tqfromLatin1("es-gt"))
            d->language = es_gt;
        else if (elemText == TQString::tqfromLatin1("es-hn"))
            d->language = es_hn;
        else if (elemText == TQString::tqfromLatin1("es-mx"))
            d->language = es_mx;
        else if (elemText == TQString::tqfromLatin1("es-ni"))
            d->language = es_ni;
        else if (elemText == TQString::tqfromLatin1("es-pa"))
            d->language = es_pa;
        else if (elemText == TQString::tqfromLatin1("es-py"))
            d->language = es_py;
        else if (elemText == TQString::tqfromLatin1("es-pe"))
            d->language = es_pe;
        else if (elemText == TQString::tqfromLatin1("es-pr"))
            d->language = es_pr;
        else if (elemText == TQString::tqfromLatin1("es-es"))
            d->language = es_es;
        else if (elemText == TQString::tqfromLatin1("es-uy"))
            d->language = es_uy;
        else if (elemText == TQString::tqfromLatin1("es-ve"))
            d->language = es_ve;
        else if (elemText == TQString::tqfromLatin1("sv"))
            d->language = sv;
        else if (elemText == TQString::tqfromLatin1("sv-fi"))
            d->language = sv_fi;
        else if (elemText == TQString::tqfromLatin1("sv-se"))
            d->language = sv_se;
        else if (elemText == TQString::tqfromLatin1("tr"))
            d->language = tr;
        else if (elemText == TQString::tqfromLatin1("uk"))
            d->language = uk;
        else
            d->language = UndefinedLanguage;
    }

    if (d->format == AtomFeed)
        tagName=TQString::tqfromLatin1("issued"); // atom doesn't specify this for feeds
                                               // but some broken feeds do this
    else
        tagName=TQString::tqfromLatin1("pubDate");

    if (!(elemText = extractNode(channelNode, tagName)).isNull()) {
        time_t _time;

        if (d->format == AtomFeed)
            _time=parseISO8601Date(elemText);
        else
            _time=KRFCDate::parseDate(elemText);
        /* \bug This isn't really the right way since it will set the date to
         * Jan 1 1970, 1:00:00 if the passed date was invalid; this means that
         * we cannot distinguish between that date, and invalid values. :-/
         */
        d->pubDate.setTime_t(_time);
    }

    if (!(elemText = extractNode(channelNode, TQString::tqfromLatin1("dc:date"))).isNull()) {
        time_t _time = parseISO8601Date(elemText);
        /* \bug This isn't really the right way since it will set the date to
         * Jan 1 1970, 1:00:00 if the passed date was invalid; this means that
         * we cannot distinguish between that date, and invalid values. :-/
         */
        d->pubDate.setTime_t(_time);
    }

    if (d->format == AtomFeed)
        tagName=TQString::tqfromLatin1("modified");
    else
        tagName=TQString::tqfromLatin1("lastBuildDate");
    if (!(elemText = extractNode(channelNode, tagName)).isNull()) {
        time_t _time;
        if (d->format == AtomFeed)
            _time = parseISO8601Date(elemText);
        else
            _time = KRFCDate::parseDate(elemText);
        d->lastBuildDate.setTime_t(_time);
    }

    if (!(elemText = extractNode(channelNode, TQString::tqfromLatin1("rating"))).isNull())
        d->rating = elemText;
    if (!(elemText = extractNode(channelNode, TQString::tqfromLatin1("docs"))).isNull())
        d->docs = elemText;
    if (!(elemText = extractNode(channelNode, TQString::tqfromLatin1((d->format == AtomFeed) ? "author" : "managingEditor"))).isNull())
        d->managingEditor = elemText;
    if (!(elemText = extractNode(channelNode, TQString::tqfromLatin1("webMaster"))).isNull())
        d->webMaster = elemText;

	if (!(elemText = extractNode(channelNode, TQString::tqfromLatin1("ttl"))).isNull())
        d->ttl = elemText.toUInt();

    n = channelNode.namedItem(TQString::tqfromLatin1("skipHours"));
    if (!n.isNull())
        for (TQDomElement e = n.firstChild().toElement(); !e.isNull(); e = e.nextSibling().toElement())
            if (e.tagName() == TQString::tqfromLatin1("hour"))
                d->skipHours.append(e.text().toUInt());

    n = channelNode.namedItem(TQString::tqfromLatin1("skipDays"));
    if (!n.isNull()) {
        Day day;
        TQString elemText;
        for (TQDomElement e = n.firstChild().toElement(); !e.isNull(); e = e.nextSibling().toElement())
            if (e.tagName() == TQString::tqfromLatin1("day")) {
                elemText = e.text().lower();
                if (elemText == TQString::tqfromLatin1("monday"))
                    day = Monday;
                else if (elemText == TQString::tqfromLatin1("tuesday"))
                    day = Tuesday;
                else if (elemText == TQString::tqfromLatin1("wednesday"))
                    day = Wednesday;
                else if (elemText == TQString::tqfromLatin1("thursday"))
                    day = Thursday;
                else if (elemText == TQString::tqfromLatin1("friday"))
                    day = Friday;
                else if (elemText == TQString::tqfromLatin1("saturday"))
                    day = Saturday;
                else if (elemText == TQString::tqfromLatin1("sunday"))
                    day = Sunday;
                else
                    day = UndefinedDay;
                if (day != UndefinedDay)
                    d->skipDays.append(day);
            }
    }
}

Document::~Document()
{
    if (d->deref())
        delete d;
}

bool Document::isValid() const
{
    return d->valid;
}

Version Document::version() const
{
    return d->version;
}

TQString Document::verbVersion() const
{
    switch (d->version) {
        case v0_90: return TQString::tqfromLatin1("0.90");
        case v0_91: return TQString::tqfromLatin1("0.91");
        case v0_92: return TQString::tqfromLatin1("0.92");
        case v0_93: return TQString::tqfromLatin1("0.93");
        case v0_94: return TQString::tqfromLatin1("0.94");
        case v1_0: return TQString::tqfromLatin1("1.0");
        case v2_0: return TQString::tqfromLatin1("2.0");
        case vAtom_0_3: return TQString::tqfromLatin1("0.3");
        case vAtom_0_2: return TQString::tqfromLatin1("0.2");
        case vAtom_0_1: return TQString::tqfromLatin1("0.1");
    }
    return TQString();
}

TQString Document::title() const
{
    return d->title;
}

TQString Document::description() const
{
    return d->description;
}

const KURL &Document::link() const
{
    return d->link;
}

Image *Document::image()
{
    return d->image;
}

const Image *Document::image() const
{
    return d->image;
}

TextInput *Document::textInput()
{
    return d->textInput;
}

const TextInput *Document::textInput() const
{
    return d->textInput;
}

const Article::List &Document::articles() const
{
    return d->articles;
}

Language Document::language() const
{
    return d->language;
}

TQString Document::copyright() const
{
    return d->copyright;
}

const TQDateTime &Document::pubDate() const
{
    return d->pubDate;
}

const TQDateTime &Document::lastBuildDate() const
{
    return d->lastBuildDate;
}

TQString Document::rating() const
{
    return d->rating;
}

const KURL &Document::docs() const
{
    return d->docs;
}

TQString Document::managingEditor() const
{
    return d->managingEditor;
}

TQString Document::webMaster() const
{
    return d->webMaster;
}

const HourList &Document::skipHours() const
{
    return d->skipHours;
}

const DayList &Document::skipDays() const
{
    return d->skipDays;
}

int Document::ttl() const
{
    return d->ttl;
}

Document &Document::operator=(const Document &other)
{
    if (this != &other) {
        other.d->ref();
        if (d && d->deref())
            delete d;
        d = other.d;
    }
    return *this;
}

// vim:noet:ts=4

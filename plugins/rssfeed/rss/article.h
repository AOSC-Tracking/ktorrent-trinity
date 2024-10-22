/*
 * article.h
 *
 * Copyright (c) 2001, 2002, 2003 Frerich Raabe <raabe@kde.org>
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. For licensing and distribution details, check the
 * accompanying file 'COPYING'.
 */
#ifndef LIBRSS_ARTICLE_H
#define LIBRSS_ARTICLE_H

#include <tqmap.h>

#include "global.h"

class TQDateTime;
class TQDomNode;
template <class> class TQValueList;
class TQString;
class TQWidget;
class KURL;
class KURLLabel;

namespace RSS
{
   /**
    * Represents an article as stored in a RSS file. You don't have to
    * instantiate one of these yourself, the common way to access instances
    * is via Document::articles().
    * @see Document::articles()
    */
   class Article
   {
      public:
         /**
          * A list of articles.
          */
         typedef TQValueList<Article> List;

         /**
          * Default constructor.
          */
         Article();

         /**
          * Copy constructor.
          * @param other The Article object to copy.
          */
         Article(const Article &other);

         /**
          * Constructs an Article from a piece of RSS markup.
          * @param node A TQDomNode which references the DOM leaf to be used
          * for constructing the Article.
          */
         Article(const TQDomNode &node, Format format);

         /**
          * Assignment operator.
          * @param other The Article object to clone.
          * @return A reference to the cloned Article object.
          */
         Article &operator=(const Article &other);

         /**
          * Compares two articles. Two articles are treated to be identical
          * if all their properties (title, link, description etc.) are
          * equal.
          * @param other The article this article should be compared with.
          * @return Whether the two articles are equal.
          */
         bool operator==(const Article &other) const;

         /**
          * Convenience method. Simply calls !operator==().
          * @param other The article this article should be compared with.
          * @return Whether the two articles are unequal.
          */
         bool operator!=(const Article &other) const { return !operator==(other); }

         /**
          * Destructor.
          */
         virtual ~Article();

         /**
          * RSS 0.90 and upwards
          * @return The headline of this article, or TQString() if
          * no headline was available.
          */
         TQString title() const;

         /**
          * RSS 0.90 and upwards
          * @return A URL referencing the complete text for this article,
          * or an empty KURL if no link was available.
          * Note that the RSS 0.91 Specification dictates that URLs not
          * starting with "http://" or "ftp://" are considered invalid.
          */
         const KURL &link() const;

         /**
          * RSS 0.91 and upwards
          * @return A story synopsis, or TQString() if no description
          * was available.
          */
         TQString description() const;

         /**
          * RSS 2.0 and upwards
          * @return An article GUID (globally unique identifier).
          */
         TQString guid() const;

         /**
          * RSS 2.0 and upwards
          * @return If this article GUID is permalink. Has no meaning when guid() is TQString().
          */
         bool guidIsPermaLink() const;

         /**
          * RSS 2.0 and upwards
          * @return The date when the article was published.
          */
         const TQDateTime &pubDate() const;
         
		 const KURL &commentsLink() const;
		 int comments() const;

		 TQString meta(const TQString &key) const;
         
         /**
          * @param parent The parent widget for the KURLLabel.
          * @param name A name for the widget which will be used internally.
          * @return a widget (a KURLLabel in this case) for the Article.
          * This makes building a user-interface which contains the
          * information in this Article object more convenient.
          * The returned KURLLabel's caption will be the title(), clicking
          * on it will emit the URL link(), and it has a TQToolTip attached
          * to it which displays the description() (in case it has one,
          * if there is no description, the URL which the label links to
          * will be used).
          * Note that you have to delete the KURLLabel object returned by
          * this method yourself.
          */
         KURLLabel *widget(TQWidget *parent = 0, const char *name = 0) const;
         
         typedef TQMap<TQString, TQString> MetaInfoMap;
         
      private:
         struct Private;
         Private *d;
   };
}

#endif // LIBRSS_ARTICLE_H

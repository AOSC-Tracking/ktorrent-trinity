/*
 * document.h
 *
 * Copyright (c) 2001, 2002, 2003 Frerich Raabe <raabe@kde.org>
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. For licensing and distribution details, check the
 * accompanying file 'COPYING'.
 */
#ifndef LIBRSS_DOCUMENT_H
#define LIBRSS_DOCUMENT_H

#include "article.h"
#include "global.h"

class TQDateTime;
class TQDomDocument;

namespace RSS
{
	class Image;
	class TextInput;
	
	/**
	 * Represents a RSS document and provides all the features and properties
	 * as stored in it. You usually don't need to instantiate this one yourself
	 * but rather use Loader::loadFrom() to produce a Document object.
	 * @see Loader::loadForm()
	 */
	class Document
	{
		public:
			/**
			 * Default constructor.
			 */
			Document();

			/**
			 * Copy constructor.
			 * @param other The Document object to copy.
			 */
			Document(const Document &other);
			
			/**
			 * Constructs a Document from a piece of XML markup.
			 */
			Document(const TQDomDocument &doc);
			
			/**
			 * Assignment operator.
			 * @param other The Document object to clone.
			 * @return A reference to the cloned Document object.
			 */
			Document &operator=(const Document &other);
			
			/**
			 * Destructor.
			 */
			~Document();

			/**
			 * @return If document is valid
			 */
			bool isValid() const;
			
			/**
			 * @return The version of this document (one of the values of the
			 * enum RSS::Version). This value can be used to determine which
			 * features this RSS document provides.
			 * @see verbVersion()
			 */
			Version version() const;

			/**
			 * Convenience method. Differs from version() only in how the result
			 * is returned.
			 * @return A TQString representing the verbose version of the
			 * document.
			 * @see version()
			 */
			TQString verbVersion() const;

			/**
			 * RSS 0.90 and upwards
			 * @return The title of the RSS document, or TQString() if no
			 * title was available. This is often the name of the news source
			 * from which the RSS document was retrieved.
			 */
			TQString title() const;

			/**
			 * RSS 0.90 and upwards
			 * @return The description of the RSS document, or TQString()
			 * if no description was available. This is usually a short slogan
			 * or description of the news source from which the RSS document
			 * was retrieved.
			 */
			TQString description() const;

			/**
			 * RSS 0.90 and upwards
			 * @return A link pointing to some website, or an empty KURL if no
			 * link was available. This URL mostly points to the homepage of
			 * the news site from which the RSS document was retrieved.
			 * Note that the RSS 0.91 Specification dictates that URLs not
			 * starting with "http://" or "ftp://" are considered invalid.
			 */
			const KURL &link() const;

			/**
			 * RSS 0.90 and upwards
			 * @return An Image object as stored in the RSS document, or a
			 * null pointer if there was no image available.
			 * @see Image
			 */
			Image *image();

			/**
			 * A version of the method above, with stricter const-ness.
			 */
			const Image *image() const;

			/**
			 * RSS 0.90 and upwards
			 * @return A TextInput object as stored in the RSS document, or a
			 * null pointer if there was no text input available.
			 * @see TextInput
			 */
			TextInput *textInput();

			/**
			 * A version of the method above, with stricter const-ness.
			 */
			const TextInput *textInput() const;

			/**
			 * RSS 0.90 and upwards
			 * @return A list of Article objects as stored in the RSS document,
			 * or a null pointer if there were no articles available. Every RSS
			 * DTD requires that there is at least one article defined, so a
			 * null pointer indicates an invalid RSS file!
			 * @see Article
			 */
			const Article::List &articles() const;
			
			/**
			 * RSS 0.91 and upwards
			 * @return The language used in the RSS document (for the article
			 * headlines etc.). This was originally introduced to assist with
			 * determining the correct page encoding but acts as a solely
			 * optional information in this library since you don't have to care
			 * about the encoding as Unicode is used in the whole library.
			 * @see RSS::Language
			 */
			Language language() const;

			/**
			 * RSS 0.91 and upwards
			 * @return A copyright of the information contained in the RSS
			 * document, or TQString() if no copyright is available.
			 */
			TQString copyright() const;

			/**
			 * RSS 0.91 and upwards
			 * @return The date when the RSS document was published.
			 */
			const TQDateTime &pubDate() const;

			/**
			 * RSS 0.91 and upwards.
			 * @return The last time the channel was modified.
			 */
			const TQDateTime &lastBuildDate() const;

			/**
			 * RSS 0.91 and upwards
			 * @return A <a href="http://www.w3.org/PICS/#Specs">PICS</a>
			 * rating for this page.
			 */
			TQString rating() const;

			/**
			 * RSS 0.91 and upwards
			 * @return This tag should contain either a URL that references a
			 * description of the channel, or a pointer to the documentation
			 * for the format used in the RSS file.
			 */
			const KURL &docs() const;

			/**
			 * RSS 0.91 and upwards
			 * @return The email address of the managing editor of the site,
			 * the person to contact for editorial inquiries. The suggested
			 * format for email addresses in RSS documents is
			 * bull@mancuso.com (Bull Mancuso).
			 * @see webMaster()
			 */
			TQString managingEditor() const;

			/**
			 * RSS 0.91 and upwards
			 * @return The email address of the webmaster for the site, the
			 * person to contact if there are technical problems with the
			 * channel, or TQString() if this information isn't available.
			 * @see managingEditor()
			 */
			TQString webMaster() const;

			/**
			 * RSS 0.91 and upwards
			 * @return A list of hours indicating the hours in the day, GMT,
			 * when the channel is unlikely to be updated. If this item is
			 * omitted, the channel is assumed to be updated hourly. Each
			 * hour should be an integer value between 0 and 23.
			 * @see skipDays()
			 */
			const HourList &skipHours() const;

			/**
			 * RSS 0.91 and upwards
			 * @return A list of <day>s of the week, in English, indicating
			 * the days of the week when the RSS document will not be updated.
			 * @see skipHours(), DayList, Day
			 */
			const DayList &skipDays() const;
			int ttl() const;
		
		private:
			struct Private;
			Private *d;
	};
}

#endif // LIBRSS_DOCUMENT_H

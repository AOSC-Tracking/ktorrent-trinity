#ifndef TESTLIBRSS_H
#define TESTLIBRSS_H

#include <tqobject.h>

#include "loader.h"
#include "document.h"
#include "article.h"
#include "global.h"

using RSS::Loader;
using RSS::Document;
using RSS::Status;

class Tester : public TQObject
{
	TQ_OBJECT
  
	public:
		void test( const TQString &url );

	private slots:
		void slotLoadingComplete( Loader *loader, Document doc, Status status );
};

#endif

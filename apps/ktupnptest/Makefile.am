INCLUDES = -I$(srcdir)/../../libktorrent -I$(srcdir)/../.. $(all_includes)
METASOURCES = AUTO
bin_PROGRAMS = ktupnptest

ktupnptest_SOURCES = main.cpp upnptestapp.cpp mainwidget.ui
ktupnptest_LDADD = ../../plugins/upnp/libktupnp.la \
	../../libktorrent/libktorrent.la $(LIB_TDEIO)
ktupnptest_LDFLAGS = $(all_libraries) $(KDE_RPATH) $(LIB_TQT) -lDCOP $(LIB_TDECORE) $(LIB_TDEUI) -ltdefx $(LIB_TDEIO) -ltdetexteditor
noinst_HEADERS = upnptestapp.h
KDE_CXXFLAGS = $(USE_EXCEPTIONS) $(USE_RTTI)

INCLUDES = -I$(srcdir)/../libktorrent -I$(srcdir)/.. $(all_includes)
METASOURCES = AUTO
noinst_HEADERS = unittest.h testrunner.h upnpparsedescriptiontest.h \
					upnpparseresponsetest.h dhtmsgparsetest.h biginttest.h rc4test.h difflehellmantest.h
bin_PROGRAMS = ktutester
ktutester_LDFLAGS = $(all_libraries) $(KDE_RPATH) $(LIB_TQT) -lDCOP $(LIB_TDECORE) $(LIB_TDEUI) -ltdefx $(LIB_TDEIO) -ltdetexteditor
ktutester_SOURCES = unittest.cpp testrunner.cpp main.cpp \
					upnpparsedescriptiontest.cpp upnpparseresponsetest.cpp dhtmsgparsetest.cpp biginttest.cpp \
		rc4test.cpp difflehellmantest.cpp
ktutester_LDADD = ../plugins/upnp/libktupnp.la \
	../libktorrent/libktorrent.la
KDE_CXXFLAGS = $(USE_EXCEPTIONS) $(USE_RTTI)

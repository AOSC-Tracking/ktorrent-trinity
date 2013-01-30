INCLUDES = -I$(srcdir)/../../libktorrent $(all_includes)
METASOURCES = AUTO

bin_PROGRAMS = ktcachecheck

ktcachecheck_SOURCES = cachecheck.cpp cachechecker.cpp singlecachechecker.cpp \
	multicachechecker.cpp
ktcachecheck_LDFLAGS = $(all_libraries) $(KDE_RPATH) $(LIB_QT) -lDCOP $(LIB_TDECORE) $(LIB_TDEUI) -ltdefx $(LIB_KIO) -ltdetexteditor
ktcachecheck_LDADD = $(LIB_KPARTS) ../../libktorrent/libktorrent.la \
	$(LIB_TDEFILE) $(LIB_KIO)
noinst_HEADERS = cachechecker.h singlecachechecker.h multicachechecker.h
KDE_CXXFLAGS = $(USE_EXCEPTIONS) $(USE_RTTI)

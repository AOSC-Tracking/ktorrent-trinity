INCLUDES = -I$(srcdir)/../../libktorrent $(all_includes)
METASOURCES = AUTO

bin_PROGRAMS = kttorinfo
kttorinfo_SOURCES = main.cpp
kttorinfo_LDFLAGS = $(all_libraries) $(KDE_RPATH) $(LIB_QT) -lDCOP $(LIB_KDECORE) $(LIB_KDEUI) -lkdefx $(LIB_KIO) -lktexteditor
kttorinfo_LDADD = ../../libktorrent/libktorrent.la $(LIB_KPARTS) $(LIB_KFILE) \
	$(LIB_KIO)

KDE_CXXFLAGS = $(USE_EXCEPTIONS) $(USE_RTTI)

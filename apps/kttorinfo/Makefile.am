INCLUDES = -I$(srcdir)/../../libktorrent $(all_includes)
METASOURCES = AUTO

bin_PROGRAMS = kttorinfo
kttorinfo_SOURCES = main.cpp
kttorinfo_LDFLAGS = $(all_libraries) $(KDE_RPATH) $(LIB_TQT) -lDCOP $(LIB_TDECORE) $(LIB_TDEUI) -ltdefx $(LIB_TDEIO) -ltdetexteditor
kttorinfo_LDADD = ../../libktorrent/libktorrent.la $(LIB_TDEPARTS) $(LIB_TDEFILE) \
	$(LIB_TDEIO)

KDE_CXXFLAGS = $(USE_EXCEPTIONS) $(USE_RTTI)

INCLUDES = -I$(top_builddir)/libktorrent -I$(top_builddir)/ktorrent/libktorrent \
	-I$(srcdir)/../../libktorrent $(all_includes)
	
METASOURCES = AUTO
		
kde_module_LTLIBRARIES = ktstatsplugin.la		
		
ktstatsplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(LIB_TDECORE) $(LIB_TDEUI) $(LIB_TDEPARTS) $(all_libraries)
ktstatsplugin_la_SOURCES = ChartDrawerData.cpp ChartDrawer.cpp statsspdwgt.ui statsconwgt.ui StatsSpd.cpp StatsCon.cpp sprefwgt.ui statspluginsettings.kcfgc \
		 StatsPluginPrefsPage.cpp StatsPluginPrefs.cpp statsplugin.cpp

ktstatsplugin_la_LIBADD = $(LIB_TQT) ../../libktorrent/libktorrent.la

noinst_HEADERS = ChartDrawerData.h ChartDrawer.h StatsSpd.h StatsCon.h StatsPluginPrefsPage.h StatsPluginPrefs.h statsplugin.h 

kde_services_DATA = ktstatsplugin.desktop
kde_kcfg_DATA = ktstatsplugin.kcfg
KDE_CXXFLAGS = $(USE_EXCEPTIONS) $(USE_RTTI)

SUBDIRS = doc libktorrent plugins apps scripts translations

# $(UTESTS_DIR)

EXTRA_DIST = AUTHORS COPYING ChangeLog INSTALL README TODO

messages: rc.cpp
	$(EXTRACTRC) `find . -name "*.rc" -o -name "*.ui" -o -name "*.kcfg"` > rc.cpp
	LIST=`find . -name \*.h -o -name \*.cpp`; \
	if test -n "$$LIST"; then \
		$(XGETTEXT) $$LIST -o $(podir)/ktorrent.pot; \
	fi
	rm -f rc.cpp


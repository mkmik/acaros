
sublibs:
	@if grep -q kernel-library Makefile; then \
	echo $$PWD/lib$(notdir $(shell echo $$PWD)).a; \
	fi
	@if [ ! "$(SUBDIRS)" = " " ] ; then \
	set $(SUBDIRS); \
	for i in $$@; do \
	$(MAKE) $(MAKEOPT) -C $$i sublibs; \
	done; fi

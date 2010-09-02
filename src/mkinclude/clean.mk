.PHONY: clean

include $(MKINCLUDE)/verbose.mk

SUBDIRS := $(SUBDIRS) $(SUBSYSTEMS)
clean:
	@if [ ! "$(SUBDIRS)" = " " ] ; then \
	set $(SUBDIRS); \
	for i in $$@; do \
	$(MAKE) $(MAKEOPT) -C $$i clean; \
	done; fi
	@rm -f $(FILES) $(SPECIAL_CLEAN) $(TARGET) .deps
	@rm -rf .libs

distclean: clean
	@find . -name "*~" -exec rm {} \;
	@rm -f include/autoconf.h

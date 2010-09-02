TOPLEVEL ?= .
MKINCLUDE = $(TOPLEVEL)/mkinclude
include $(TOPLEVEL)/config.mk

MAKESUB = for i in $(SUBSYSTEMS); do $(MAKE) -C $$i; done

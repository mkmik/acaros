TOPLEVEL=../../..
include $(TOPLEVEL)/base.mk

SUBSYSTEMS-y += mm
SUBSYSTEMS-$(CONFIG_KDB) += kdb

.PHONY: all

all: subsystems

include $(MKINCLUDE)/epilogue.mk

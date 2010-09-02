TOPLEVEL=..
include $(TOPLEVEL)/base.mk

SUBSYSTEMS-y += log mm 
SUBSYSTEMS-$(CONFIG_KDB) += kdb

TARGET = kernel

SPECIAL_CLEAN = acaros.tar.gz .libs

LIBS ?= $(shell $(MAKE) $(MAKEOPT) -s LIBS= .libs; grep -v "make\[" .libs)

default: all

# deps
$(TARGET): .libs $(SUBSYSTEMS)

# do not edit
.libs: ../include/autoconf.h
	@$(MAKE) $(MAKEOPT) -s -C .. sublibs >.libs

include $(MKINCLUDE)/kernel.mk

.PHONY: all $(TARGET)

all: subsystems $(TARGET)
	@echo building kernel  archive
	@tar cfz acaros.tar.gz kernel

include $(TOPLEVEL)/arch/$(ARCH)/kernel.mk

include $(MKINCLUDE)/epilogue.mk

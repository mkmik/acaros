TOPLEVEL=..
include $(TOPLEVEL)/base.mk

.PHONY: all

all: 
	@$(MAKE) -C $(ARCH)

%::
	@$(MAKE) -C $(ARCH) $@

#include $(MKINCLUDE)/epilogue.mk

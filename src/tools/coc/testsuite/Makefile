TOPLEVEL=../../..
include $(TOPLEVEL)/base.mk

TARGET=coctest

CFLAGS:=$(CFLAGS) -I../../../include -I../../../arch/$(ARCH)/include

include $(MKINCLUDE)/application.mk

$(TARGET): $(FILES)
	$(COC) -o $@ $(FILES) $(LIBS)

include $(MKINCLUDE)/epilogue.mk

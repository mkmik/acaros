TOPLEVEL=../../..
include $(TOPLEVEL)/base.mk

TARGET=loader
LIBS=../lib/stand/libstand.a ../lib/hat/libhat.a

include $(MKINCLUDE)/kernel.mk

$(TARGET): $(FILES) $(LIBS)
	@echo building loader
	$(CC) -o $@ $(FILES) $(LIBS) -nostdlib -lgcc -Wl,-Ttext,0x100000,-S

include $(MKINCLUDE)/epilogue.mk

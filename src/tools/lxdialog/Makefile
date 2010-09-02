TOPLEVEL=../..
include $(TOPLEVEL)/base.mk

TARGET=lxdialog
LIBS=-lncurses

include $(MKINCLUDE)/application.mk

$(TARGET): $(FILES)
	$(CC) -o $@ $(FILES) $(LIBS)

include $(MKINCLUDE)/epilogue.mk

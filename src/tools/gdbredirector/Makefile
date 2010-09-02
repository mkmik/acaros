TOPLEVEL=../..
include $(TOPLEVEL)/base.mk

TARGET=gdbredirector

include $(MKINCLUDE)/application.mk

$(TARGET): $(FILES)
	$(CC) -g3 -o $@ $(FILES)

include $(MKINCLUDE)/epilogue.mk

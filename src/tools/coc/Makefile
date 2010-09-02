TOPLEVEL=../..
include $(TOPLEVEL)/base.mk

SUBSYSTEMS-y += co-runtime testsuite
TARGET=coc
LIBS=../../lib/algo/libalgo.a
DIR=$(shell pwd)

include $(MKINCLUDE)/application.mk
CFLAGS += -I../../include -I../../arch/$(ARCH)/include

SPECIAL_CLEAN=file.c dir.h

default: all
# deps
testsuite: co-runtime $(TARGET)
co-runtime: $(TARGET)
# do not edit

all: libalgo.a $(TARGET) subsystems

$(TARGET): $(FILES) 
	$(CC) -g2 -o $@ $(FILES) $(LIBS)

# hack
libalgo.a:
	$(MAKE) -C ../../lib/algo

main.c: dir.h
filter.c: dir.h

dir.h: Makefile
	@echo "#define CO_DIR \"$(DIR)\"" >dir.h
	@echo "#define CO_INCLUDE_DIR CO_DIR \"/include\"" >>dir.h
	@echo "#define CO_LIB CO_DIR \"/co-runtime/libco-runtime.a\"" >>dir.h

include $(MKINCLUDE)/epilogue.mk

TOPLEVEL=../..
include $(TOPLEVEL)/base.mk

TARGET=conf
LIBS=

SPECIAL_CLEAN := zconf.tab.c zconf.tab.h lex.zconf.c

include $(MKINCLUDE)/application.mk

FILES := zconf.tab.o mconf.o conf.o

$(TARGET): $(FILES)
	$(CC) -o $@ $(FILES) $(LIBS)

zconf.tab.c: lex.zconf.c zconf.y 
yconf.tab.h: zconf.tab.c

%.tab.c: %.y
	$(YACC) -t -d -v -b $* -p $(notdir $*) $<

lex.%.c: %.l
	$(LEX) -P$(notdir $*) -o$@ $<

include $(MKINCLUDE)/epilogue.mk

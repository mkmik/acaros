TARGET = $(THISLIB)

include $(MKINCLUDE)/files.mk

THISLIB = lib$(notdir $(shell pwd)).a
THISLIBNAME = lib$(notdir $(shell pwd))

$(THISLIB): $(FILES)
	@echo building archive $(THISLIB)
	rm -f $(THISLIB)
	ar cru $(THISLIB) $(FILES)
	$(RANLIB) $(THISLIB)

all: subsystems $(THISLIB)
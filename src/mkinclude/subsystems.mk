.PHONY: subsystems $(SUBSYSTEMS) $(SUBSYSTEMS-y)

include $(MKINCLUDE)/verbose.mk

SUBSYSTEMS=$(SUBSYSTEMS-y)

subsystems: $(SUBSYSTEMS)

$(SUBSYSTEMS):
	@$(MAKE) $(MAKEOPT) -C $@

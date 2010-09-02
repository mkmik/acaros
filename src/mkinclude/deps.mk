.PHONY: deps

deps:
	@if [ "$(SUBSYSTEMS)" != "" ]; then \
	set $(SUBSYSTEMS); \
	for i in $$@; do \
	$(MAKE) -C $$i deps; \
	done; fi
	@echo *.c | grep -q '*.c'  || gcc -MM $(CFLAGS) *.c >.deps
	@echo *.co | grep -q '*.co' || gcc -x c -MM $(CFLAGS) *.co >>.deps

-include .deps

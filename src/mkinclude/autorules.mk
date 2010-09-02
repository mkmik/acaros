ifneq ($(SHOW_CMDLINE),yes)

%.o : %.cpp
	@echo Compiling $<
	@$(CXX) -c $(CXXFLAGS) $<

%.o : %.c
	@echo Compiling $<
	@$(CC) -c $(CFLAGS) $<

%.o : %.co
	@echo Compiling $<
	@$(COC) -c $(CFLAGS) $<

%.o : %.S
	@echo Assembling $<
	@$(CC) -c  $<

else

%.o : %.co
	$(COC) -c $(CFLAGS) $<

endif
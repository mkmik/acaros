LARG:=-Wl,-whole-archive $(LIBS) -Wl,-no-whole-archive

$(TARGET): $(FILES)
	$(COC) -o $@ $(FILES) $(LARG) -g2 -nostdlib -lgcc -Wl,-Ttext,0xE0000000

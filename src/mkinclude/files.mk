FILES := $(FILES) $(addsuffix .o,$(basename $(wildcard *.S))) 
FILES := $(FILES) $(addsuffix .o,$(basename $(wildcard *.c)))
FILES := $(FILES) $(addsuffix .o,$(basename $(wildcard *.co)))


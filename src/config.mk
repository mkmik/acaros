include $(TOPLEVEL)/version.mk

KERNELRELEASE=$(VERSION).$(PATCHLEVEL).$(SUBLEVEL)$(EXTRAVERSION)

CC=gcc
COC=coc
LD=ld
AS=as
RANLIB=ranlib
LEX=flex
YACC=bison

PATH:=$(PATH):$(TOPLEVEL)/tools/coc

#CFLAGS=-O2 -Wall -pipe 
CFLAGS=-g2 -Wall -pipe 
KERNEL_INCLUDES=-I$(TOPLEVEL)/include -I$(TOPLEVEL)/arch/$(ARCH)/include -I. \
	-I$(shell gcc --print-file-name=include)
KERNEL_DEFS=-DACAROS
KERNEL_CFLAGS=$(CFLAGS) -ffast-math -nostdinc $(KERNEL_INCLUDES) $(KERNEL_DEFS)

include $(MKINCLUDE)/arch.mk
-include $(TOPLEVEL)/.config

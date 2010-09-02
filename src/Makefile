include base.mk

SUDO = sudo

SUBSYSTEMS-y += tools arch kernel lib 
SPECIAL_CLEAN := include/version.h

default: all
# deps 
kernel: lib arch tools

# do not edit

.PHONY: all tools config 
all: include/autoconf.h include/version.h subsystems

config: 
	@$(MAKE) -C tools/lxdialog
	@$(MAKE) -C tools/kconfig
	@./tools/kconfig/conf -x arch/$(ARCH)/Kconfig

mrproper: distclean
	@rm -f .config .config.cmd .tmpconfig.h

include/autoconf.h: 
	$(error "Please issue a 'make config' first") 

include/version.h: config.mk
	@echo "#define KERNEL_RELEASE \""$(KERNELRELEASE)"\"" >include/version.h

install: all
	@echo -n "Installing loader and kernel modules ... "
	@cp arch/$(ARCH)/loader/loader loader
	@gzip loader
	@mv loader.gz loader
	@if grep -q $(dir $(shell echo $$PWD))image /proc/mounts; then \
	cp loader kernel/acaros.tar.gz ../image; \
	echo -n "loop "; \
	fi
	@if [ -e /home/tftp ]; then cp loader kernel/acaros.tar.gz \
	/home/tftp; \
	echo -n "pxe "; \
	fi
	@echo
	@sync
	@rm loader

loop_mount:
	@$(SUDO) mount -o loop,uid=$(USER) ../acaros.img ../image

loop_unmount:
	@$(SUDO) umount ../image

rebuild:
	$(MAKE) clean
	$(MAKE) deps
	$(MAKE) install

debug: install
	@bash -i -c "tools/gdbredirector/gdbredirector -q & gdb kernel/kernel; kill %tool"

include $(MKINCLUDE)/epilogue.mk


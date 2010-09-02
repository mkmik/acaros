deps_config := \
	kernel/mm/Kconfig \
	kernel/kdb/Kconfig \
	kernel/Kconfig \
	arch/i386/Kconfig

.config include/linux/autoconf.h: $(deps_config)

$(deps_config):

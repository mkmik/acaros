#include <autoconf.h>
#include <version.h>
#include <stand/stand.h>
#include <kdb/kdb.h>
#include <mal/mal.h>
#include <mal/interrupt.h>
#include <log/log.h>
#include <mm/mm.h>
#include <coinit.h>

void cotest();
void vesaTest();

void _start(struct stand_boot_context* ctx) {
  stand_init(ctx);
  stand_printIntro();
  klogf(LOG_MESSAGE, "Acaros booting (context %p)\n", ctx);

  mal_init();
  mal_interrupt_init(); // separated from mal_init to avoid problems in loader. will change

#ifdef CONFIG_KDB
  kdb_init();
#endif

  mm_init();

  co_init();
  //  cotest();
  //  vesaTest();

  /*  klogf(LOG_MESSAGE, "reading from null address: %x\n", *(int*)0x0);
  klogf(LOG_MESSAGE, "writing to null address\n");
  *(int*)0x0=0xdeadbeef; */
  panic("Simulate panic. Just a test");
}

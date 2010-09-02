#ifndef _PLATFORM_INTERRUPT_H_
#define _PLATFORM_INTERRUPT_H_

#include <platform/x86.h>

/** probe and initialize the interrupt controller */
void mal_platform_interrupt_init();

/** disable interrupts (kernel mode only) (TODO: rename) */
#define mal_disable_interrupts() asm("cli" : : : "memory")
/** enable  interrupts (kernel mode only) */
#define mal_enable_interrupts() asm("sti" : : : "memory")

#define IRQL_INTERRUPT 20
#define IRQL_DISPATCH 10
#define IRQL_KERNEL 5
#define IRQL_USER 1
struct irql {
  int level;
  u_int32_t flags;
};
typedef struct irql irql_t;

extern int mal_irq_level;

static inline int irql_get() {
  return mal_irq_level;
}

/** sets the new irq level and save provious
 * level and interrupt state.
 * interrupt state is saved separately from
 * interrupt level in order to cope with manual
 * enable/disable of interrupt, and/or isr routines.
 * you are guarantied to get back to the same state */
static inline irql_t mal_irql_set(int level) {
  irql_t old;
  __save_flags(old.flags);
  if(level >= IRQL_DISPATCH)
    mal_disable_interrupts();
  else
    mal_enable_interrupts();
  old.level =  mal_irq_level;
  return old;
}


#endif

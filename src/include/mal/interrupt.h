#ifndef _MAL_INTERRUPT_H
#define _MAL_INTERRUPT_H

// TODO: don't expose x86 arch

#include <platform/mal/idt.h>

typedef int mal_irq_t;

void mal_interrupt_init();
void mal_ack(int irq);
void mal_enableIRQ(int irq);
void mal_disableIRQ(int irq);

mal_vector_t mal_vectorFromIrq(mal_irq_t irq);
mal_irq_t mal_irqFromVector(mal_vector_t vector);

#endif

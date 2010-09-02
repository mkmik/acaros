#include <mal/interrupt.h>
#include <platform/mal/interrupt.h>
#include <platform/mal/i8259.h>
#include <log/log.h>

void mal_defaultTimerIsr() {
  klogf(LOG_DEBUG, "timer\n");
}

void mal_interrupt_init() {
  // hardcode the i8259 controller for now....
  i8259_init();
  mal_attachIsr(0, mal_defaultTimerIsr);
  mal_enable_interrupts(); // TODO FIND BETTER PLACE!!!
}

void mal_ack(int irq) {
  i8259_ack(irq);
}

void mal_enableIRQ(int irq) {
  i8259_enable_irq(irq);
}

void mal_disableIRQ(int irq) {
  i8259_disable_irq(irq);
}

mal_vector_t mal_vectorFromIrq(mal_irq_t irq) {
  return 0x20 + irq;
}

mal_irq_t mal_irqFromVector(mal_vector_t vector) {
  return vector - 0x20;
}

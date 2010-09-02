#ifndef _PLATFORM_I8259_H_
#define _PLATFORM_I8259_H_

/** initialize standard PC PIC */
void i8259_init();
/** acknowledges an interrupt */
void i8259_ack(int irq);
/** hardware unmask an irq */ 
void i8259_enable_irq(int irq);
/** hardware mask an irq */ 
void i8259_disable_irq(int irq);

#endif

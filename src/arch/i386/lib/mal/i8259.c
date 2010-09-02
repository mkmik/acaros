#include <platform/mal/i8259.h>
#include <mal/ioport.h>

static unsigned int cached_irq_mask = 0xffff;
#define cached_21 (cached_irq_mask & 0xFF)
#define cached_A1 (cached_irq_mask >> 8)


void i8259_init() {
  int auto_eoi = 0;
  int i;

  mal_outb(0xff, 0x21); /* mask all of 8259A-1 */
  mal_outb(0xff, 0xA1); /* mask all of 8259A-2 */
  
  /*
   * mal_outb_p - this has to work on a wide range of PC hardware.
   */
  mal_outb_p(0x11, 0x20);     /* ICW1: select 8259A-1 init */
  mal_outb_p(0x20 + 0, 0x21); /* ICW2: 8259A-1 IR0-7 mapped to 0x20-0x27 */
  mal_outb_p(0x04, 0x21);     /* 8259A-1 (the master) has a slave on IR2 */
  
  if (auto_eoi)
    mal_outb_p(0x03, 0x21);     /* master does Auto EOI */
  else
    mal_outb_p(0x01, 0x21);     /* master expects normal EOI */
  
  mal_outb_p(0x11, 0xA0);     /* ICW1: select 8259A-2 init */
  mal_outb_p(0x20 + 8, 0xA1); /* ICW2: 8259A-2 IR0-7 mapped to 0x28-0x2f */
  mal_outb_p(0x02, 0xA1);     /* 8259A-2 is a slave on master's IR2 */
  mal_outb_p(0x01, 0xA1);     /* (slave's support for AEOI in flat mode
			     is to be investigated) */

  //udelay(100);
  for(i=0; i< 0x1000000; i++);  

  mal_outb(0xff, 0x21); /* mask AGAIN all of 8259A-1 */
  mal_outb(0xff, 0xA1); /* mask AGAIN all of 8259A-2 */
  mal_enableIRQ(2); /* enable cascade */
}

void i8259_ack(int irq) {
  if(irq & 8) {
    mal_inb(0xA1);              /* DUMMY - (do we need this?) */
    mal_outb(cached_A1,0xA1);
    mal_outb(0x60+(irq&7),0xA0);/* 'Specific EOI' to slave */
    mal_outb(0x62,0x20);        /* 'Specific EOI' to master-IRQ2 */
  } else {
    mal_inb(0x21);              /* DUMMY - (do we need this?) */
    mal_outb(cached_21,0x21);
    mal_outb(0x60+irq,0x20);    /* 'Specific EOI' to master */
  }
}

static void update_i8259_mask(int irq) {
  if(irq & 8)
    mal_outb(cached_A1, 0xA1);
  else
    mal_outb(cached_21 & 0xFF,0x21);
}

void i8259_enable_irq(int irq) {
  cached_irq_mask &= ~(1 << irq); 
  update_i8259_mask(irq);
}

void i8259_disable_irq(int irq) {
  cached_irq_mask |= (1 << irq); 
  update_i8259_mask(irq);
}

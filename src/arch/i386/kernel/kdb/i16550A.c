#include <platform/kdb/i16550A.h>
#include <kdb/uart.h>
#include <log/log.h>
#include <misc.h>
#include <mal/ioport.h>
#include <mal/interrupt.h>
#include <platform/mal/idt.h>

void kdb_i16550A_isr(mal_vector_t vector, mal_trapFrame_t frame);

mal_trapFrame_t *kdb_uart_savedContext;

static struct {
  unsigned int base;
  int irq;
} kdb_i16550A_ports[4];

static unsigned int bios_get_serial_base(int port_num) {
  u_int16_t* base_bios = (u_int16_t*)0x400;
  base_bios += port_num;
  return (unsigned int) *base_bios ? : 0x3f8;
}

static u_int8_t get_lcr(int port) {
  return mal_inb(BASE(port) + LCR_OFFSET);
}

static void set_lcr(int port, u_int8_t value) {
  mal_outb(value, BASE(port) + LCR_OFFSET);
}

static u_int8_t get_lsr(int port) {
    return mal_inb(BASE(port) + LSR_OFFSET);
}

static u_int8_t get_iir(int port) {
    return mal_inb(BASE(port) + IIR_OFFSET);
}

static void set_dlab(int port, int en) {
  set_lcr(port, BIT_ASSIGN(get_lcr(port), LCR_DLAB_BIT, en));
}

static void set_speed(int port, int speed) {
  int base_speed = 115200;
  u_int16_t divisor = base_speed / speed;
  
  set_dlab(0, 1);
  //  outw(divisor, BASE(port) + DL_OFFSET); // bochs doesn't like
  mal_outb(divisor & 0xFF, BASE(port) + DL_OFFSET);
  mal_outb((divisor >> 8) & 0xFF, BASE(port) + DL_OFFSET + 1);
  set_dlab(0, 0);
}

static void set_stop_bits(int port, int num) {
  if(num == 1)
    set_lcr(port, BIT_CLEAR(get_lcr(port), LCR_STOP_BIT_BIT));
  else
    set_lcr(port, BIT_SET(get_lcr(port), LCR_STOP_BIT_BIT));
}

static void set_word_length(int port, int len) {
  int code;
  int lcr = get_lcr(port);
  
  if(len == 8)
    code = WORD_LENGTH_8;
  else if(len == 7)
    code = WORD_LENGTH_7;
  else if(len == 6)
    code = WORD_LENGTH_6;
  else
    code = WORD_LENGTH_6;
 
  lcr = BIT_CLEAR(lcr, LCR_WORD_LENGTH_BIT_0);
  lcr = BIT_CLEAR(lcr, LCR_WORD_LENGTH_BIT_1);
  lcr |= code << LCR_WORD_LENGTH_BIT_0; // shift useless. for "readability"
  set_lcr(port, lcr);
}

static void init_fifo() {
}

static void set_parity(int port, int par) {
  int b0=0, b1=0, b2=0;
  u_int8_t lcr;
  switch(par) {
  case PARITY_E:
    b1 = 1; 
    // fall through
  case PARITY_O:
    b0 = 1;
    // .. sticky parities...
  }
  
  lcr = get_lcr(port);
  lcr = BIT_ASSIGN(lcr, LCR_PARITY_BIT_0, b0);
  lcr = BIT_ASSIGN(lcr, LCR_PARITY_BIT_1, b1);
  lcr = BIT_ASSIGN(lcr, LCR_PARITY_BIT_2, b2);
  set_lcr(port, lcr);
}

static void init_modem(int port) {
  mal_outb(0xb, BASE(port) + MCR_OFFSET);
}

static void set_interrupt(int port) {
  u_int8_t ier = 0;
  ier = BIT_SET(ier, IER_DATA_IN_BIT);
  
  mal_outb(ier, BASE(port) + IER_OFFSET);
  mal_attachIsr(0x20 + IRQ(port), kdb_i16550A_isr);
  mal_enableIRQ(IRQ(port));
}

void kdb_i16550A_putchar(int port, char ch) {
  mal_outb(ch, BASE(port) + TX_OFFSET);
}

int kdb_i16550A_getchar(int port) {
  if(BIT_TEST(get_lsr(port), LSR_DATA_READY_BIT)) 
    return mal_inb(BASE(0) + RX_OFFSET);
  else
    return -1;
}

void kdb_i16550A_tx_ien(int port, int en) {
  mal_outb(BIT_ASSIGN(mal_inb(BASE(port) + IER_OFFSET), IER_DATA_OUT_BIT, en),
       BASE(port) + IER_OFFSET);
}

void kdb_i16550A_init() {
  int i;
  
  for(i=0; i<4; i++) {
    BASE(i) = bios_get_serial_base(i);
    IRQ(i) = (i & 1) ? 3 : 4;
  }

  //  set_speed(0, 9600);
  set_speed(0, 38400);
  set_parity(0, PARITY_N);
  set_word_length(0, 8);
  set_stop_bits(0, 1);
  init_fifo();
  init_modem(0);
  set_interrupt(0);
/*  (void)mal_inb(BASE(0));
  mal_outb('x', BASE(0)); */
}

void kdb_i16550A_isr(mal_vector_t vector, mal_trapFrame_t frame) {
  kdb_uart_savedContext = &frame;

  int port=0;
  u_int8_t iir = get_iir(port);
  u_int8_t type;
  
  // interrupt sharing. test origin
  if(!BIT_TEST(iir, IIR_PENDING_BIT)) // port 0 ? (0=pending)
    type = (iir & IIR_TYPE_MASK) >> IIR_TYPE_BIT_0;
  else if(!BIT_TEST(iir = get_iir(port = 2), IIR_PENDING_BIT)) // origin port 2
    type = (iir & IIR_TYPE_MASK) >> IIR_TYPE_BIT_0;
  else {
    type = 0;
//    printf(" ERROR no uart originator\n");
  }
    
  if(type == RX_INTERRUPT) {
    do {
      char ch = kdb_i16550A_getchar(port);
//      printf("'%c' (%d) ", ch, (unsigned int)ch);
      kdb_uart_rx_isr(port, ch);
    } while(BIT_TEST(get_lsr(port), LSR_DATA_READY_BIT));
  } else if (type == TX_INTERRUPT) {
    kdb_uart_tx_isr(port);
  }

  kdb_i16550A_ack(port);
}

void kdb_i16550A_ack(int port) {
  mal_ack(IRQ(port));
}

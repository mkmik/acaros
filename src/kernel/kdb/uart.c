#include <kdb/kdb.h>
#include <kdb/uart.h>
#include <log/log.h>
#include <platform/kdb/uart.h>

void kdb_uart_init() {
  klogf(LOG_MESSAGE, "enabling uart\n");

  kdb_i16550A_init();

  klogf(LOG_DEBUG, "uart initialized\n");
}

void kdb_uart_rx_isr_test(int port, char ch) { 
  if(ch == 's')
    kdb_suspend();
  else if(ch == 'q')
    kdb_resume();
  
  // echo
  kdb_uart_putchar(ch);
}

void kdb_uart_rx_isr(int port, char ch) { 
  kdb_receive(ch);
}

void kdb_uart_tx_isr(int port) {
  panic("GOT UART TX INTERRUPT");
}

void kdb_uart_ack() {
  kdb_i16550A_ack(0);
}

void kdb_uart_putchar(char ch) {
  kdb_platform_uart_putchar(0, ch);
}

int kdb_uart_getchar() {
  return kdb_platform_uart_getchar(0);
}


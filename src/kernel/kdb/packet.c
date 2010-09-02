#include <kdb/kdb.h>
#include <kdb/uart.h>
#include <kdb/packet.h>

static u_int8_t kdb_checksum;

void kdb_beginPacket() {
  kdb_uart_putchar('$');
  kdb_checksum = 0;
}

void kdb_endPacket() {
  kdb_uart_putchar('#');
  kdb_uart_putchar(hexdigit(kdb_checksum >> 4));
  kdb_uart_putchar(hexdigit(kdb_checksum & 0xF));
}

void kdb_putchar(char ch) {
  kdb_checksum += (u_int8_t)ch;
  kdb_uart_putchar(ch);
}

void kdb_putstr(char* str) {
  while(*str)
    kdb_putchar(*str++);
}

void kdb_putint(u_int32_t val) {
  int i;
  u_int8_t *v = (u_int8_t*)&val;
  for(i=0; i<4; i++) {
    kdb_putchar(hexdigit(v[i] >> 4));
    kdb_putchar(hexdigit(v[i] & 0xF));
  }
  //  kdb_putstr("fee1dead");
}

void kdb_sendPacket(char *str) {
  kdb_beginPacket();
  kdb_putstr(str);
  kdb_endPacket();
}

void kdb_sendOk() {
  kdb_beginPacket();
  kdb_putstr("OK");
  kdb_endPacket();
}

void kdb_sendAck() {
  kdb_uart_putchar('+');
}

void kdb_sendNack() {
  kdb_uart_putchar('-');
}

void kdb_sendNotSupported() {
  kdb_beginPacket();
  kdb_endPacket();
}

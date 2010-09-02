#include <kdb/kdb.h>
#include <kdb/packet.h>
#include <kdb/command.h>
#include <platform/kdb/kdb.h>
#include <platform/mal/interrupt.h>
#include <platform/mal/idt.h>
#include <platform/hat/initial_paging.h>
#include <kdb/uart.h>
#include <misc.h>
#include <log/log.h>

// debug
#include <mm/mm.h> 
#include <mm/pool.h> 

mal_trapFrame_t *kdb_currentContext;
volatile kdb_registers_t kdb_registers;

int kdb_attached = 0;
volatile int kdb_suspended;

void kdb_userAttention() {
  klogf(LOG_MESSAGE, "Waiting for gdb connection on ttyS0\n");
}

void kdb_waitHost() {
  kdb_breakpoint();
}

void kdb_test(int i) {
  klogf(LOG_MESSAGE, "test %d\n", i++);
}

void kdb_init() {
  kdb_command_init();

  mal_attachIsr(KDB_DEBUG_VECTOR, kdb_hardIsr);
  mal_attachIsr(KDB_BREAKPOINT_VECTOR, kdb_softIsr);

  mal_disable_interrupts();
  kdb_uart_init();  
  mal_enable_interrupts(); 

#ifdef CONFIG_KDB_WAIT
  kdb_waitHost();
#endif

#if 0
  int i=0;
  while(1) {
    kdb_test(i++);
  }
#endif
  
}

void kdb_suspend() {
  // check if already suspended
  if(kdb_suspended) {
    klogf(LOG_CRITICAL, "DOUBLE SUSPEND");
    return;
  }

  // drain fifo
  // interrupts are no more received if there are elements in the fifo
  // don't know if this is normal or just vmware. TODO check this out
  while(kdb_uart_getchar() != -1);

  kdb_uart_ack();

  kdb_suspended = 1;
  mal_enable_interrupts();
  while(kdb_suspended);
}

void kdb_resume() {
  kdb_restoreRegisters();
  kdb_suspended = 0;
}

void kdb_service() {
  if(kdb_attached) {
    kdb_sendTrapStatus();
  } else {
    kdb_userAttention();
  }
  kdb_suspend();
}

u_int8_t kdb_readByte(pointer_t addr) {
  if(hat_isValid(addr))
    return *(u_int8_t*)addr;
  return 0;
}

void kdb_writeByte(pointer_t addr, u_int8_t byte) {
  if(hat_isValid(addr))
    *(u_int8_t*)addr = byte;
  else {
    klogf(LOG_PANIC, "Trying to write %x at %x\n", byte, addr);
    panic("page not present");
  }
    
}

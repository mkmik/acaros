#include <platform/kdb/kdb.h>
#include <log/log.h>
#include <platform/x86.h>
#include <misc.h>
#include <stand/libc.h>
#include <kdb/packet.h>

void kdb_setBreakpoint(int bp, void *address) {
  switch(bp) {
  case 0:
    WRITE_DR0(address);
    break;
  case 1:
    WRITE_DR1(address);
    break;
  case 2:
    WRITE_DR2(address);
    break;
  case 3:
    WRITE_DR3(address);
    break;
  default:
    panic("x86 implements only 4 breakpoint registers");
  }

}

void kdb_enableBreakpoint(int bp) {
  u_int32_t dr7;
  READ_DR7(dr7);
  dr7 = BIT_SET(dr7, bp + 1); // global break point enable
  WRITE_DR7(dr7);
}

void kdb_disableBreakpoint(int bp) {
  u_int32_t dr7;
  READ_DR7(dr7);
  dr7 = BIT_CLEAR(dr7, bp + 1); // global break point enable
  WRITE_DR7(dr7);
}

void kdb_prepareRegisters() {
  if(kdb_currentContext == 0) {
    panic("kdb: invalid context");
  }

  kdb_registers.eax = kdb_currentContext->reg.eax;
  kdb_registers.ebx = kdb_currentContext->reg.ebx;
  kdb_registers.ecx = kdb_currentContext->reg.ecx;
  kdb_registers.edx = kdb_currentContext->reg.edx;
  kdb_registers.edi = kdb_currentContext->reg.edi;
  kdb_registers.esi = kdb_currentContext->reg.esi;
  kdb_registers.esp = kdb_currentContext->reg.esp;
  kdb_registers.ebp = kdb_currentContext->reg.ebp;

  kdb_registers.eip = kdb_currentContext->plain.eip;
  kdb_registers.eflags = kdb_currentContext->plain.eflags;
  kdb_registers.cs = 0x10;
  kdb_registers.ds = 0x18;
  kdb_registers.ss = 0x18;
  kdb_registers.es = 0x18;
  kdb_registers.fs = 0x18;
  kdb_registers.gs = 0x18;

}

void kdb_restoreRegisters() {
  if(kdb_currentContext == 0) {
    panic("kdb: invalid context");
  }

  kdb_currentContext->reg.eax = kdb_registers.eax;
  kdb_currentContext->reg.ebx = kdb_registers.ebx;
  kdb_currentContext->reg.ecx = kdb_registers.ecx;
  kdb_currentContext->reg.edx = kdb_registers.edx;
  kdb_currentContext->reg.edi = kdb_registers.edi;
  kdb_currentContext->reg.esi = kdb_registers.esi;
  kdb_currentContext->reg.esp = kdb_registers.esp;
  kdb_currentContext->reg.ebp = kdb_registers.ebp;

  kdb_currentContext->plain.eip = kdb_registers.eip;
  kdb_currentContext->plain.eflags = kdb_registers.eflags;
}

void kdb_hardIsr(mal_vector_t vector, mal_trapFrame_t frame) {
  kdb_currentContext = &frame;
  kdb_prepareRegisters();

  kdb_disableStep();
  kdb_service();

  // set RF bit in EFLAGS, in order to resume execution without generating another 
  // breakpoint 
  frame.plain.eflags = BIT_SET(frame.plain.eflags, 16);

  kdb_currentContext = 0;
}

void kdb_softIsr(mal_vector_t vector, mal_trapFrame_t frame) {
  kdb_currentContext = &frame;
  kdb_prepareRegisters();
  // decrement eip because gdb inserted an int $0x3 !!!!
  kdb_registers.eip--;

  kdb_disableStep();
  kdb_service();


  if(*(u_int8_t*)(kdb_registers.eip) == 0xcc) {
    kdb_registers.eip++;
    kdb_restoreRegisters();
  }

  kdb_currentContext = 0;
}

void kdb_enableStep() {
  kdb_registers.eflags = BIT_SET(kdb_registers.eflags, 8);
};

void kdb_disableStep() {
  kdb_registers.eflags = BIT_CLEAR(kdb_registers.eflags, 8);
};

void kdb_sendTrapStatus() {
  kdb_beginPacket();
  
  kdb_putstr("T05");

  kdb_putstr("4");
  kdb_putchar(':');
  kdb_putint(kdb_registers.esp);
  kdb_putchar(';');

  kdb_putstr("5");
  kdb_putchar(':');
  kdb_putint(kdb_registers.ebp);
  kdb_putchar(';');

  kdb_putstr("8");
  kdb_putchar(':');
  kdb_putint(kdb_registers.eip);
  kdb_putchar(';');
  

  kdb_endPacket();
}

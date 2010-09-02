#ifndef _PLATFORM_KDB_H_
#define _PLATFORM_KDB_H_

#include <kdb/kdb.h>
#include <platform/mal/idt.h>

#define KDB_DEBUG_VECTOR 1
#define KDB_BREAKPOINT_VECTOR 3

/** set a hardware breakpoint "bp" at address "address".
 * x86 defines only 4 hardware breakpoints */
void kdb_setBreakpoint(int bp, void *address);

/** enable the "bp" hardware breakpoint */
void kdb_enableBreakpoint(int bp);
/** disable the "bp" hardware breakpoint */
void kdb_disableBreakpoint(int bp);
/** manually invoke a breakpoint */
static inline void kdb_breakpoint() {
  asm("int $0x3");
}

/** enable hardware aided single stepping, by setting
 * the TRACE bit in the saved eflags register image on the current
 * trap frame. */
void kdb_enableStep();
/** enable hardware aided single stepping, by clearing
 * the TRACE bit in the saved eflags register image on the current
 * trap frame. */
void kdb_disableStep();

/** this structure defines the CPU register in the format 
 * requered by gdb.
 * this structure is filled upon entering the debugger from an interruption 
 * and restored in the trap frame before resuming execution */
struct kdb_registers {
  struct {
    u_int32_t eax;
    u_int32_t ecx;
    u_int32_t edx;
    u_int32_t ebx;
    u_int32_t esp;
    u_int32_t ebp;
    u_int32_t esi;
    u_int32_t edi;
    u_int32_t eip;
    u_int32_t eflags;
    u_int32_t cs;
    u_int32_t ss;
    u_int32_t ds;
    u_int32_t es;
    u_int32_t fs;
    u_int32_t gs;
  };
  u_int32_t reg[16];
};

typedef struct kdb_registers kdb_registers_t;

extern volatile kdb_registers_t kdb_registers;

/** the current trap frame is saved here. 
 * when an interrupt invokes the debugger it sets here 
 * the interrupted context, in order to access CPU state and 
 * to resume execution later */
extern mal_trapFrame_t *kdb_currentContext;

/** fill kdb_registers with the registers saved int kdb_saved_registers
 * plus eflags and eip retreived from the same interrupt stack frame 
 */
void kdb_prepareRegisters();
/** 
 * saves the registers from kdb_registers to the interrupted stack frame
 */
void kdb_restoreRegisters();

/**
 * sends a ``T'AAN...`:'R...`;'N...`:'R...`;'N...`:'R...`;''
 * packet to gdb indicating eip, esp
 */
void kdb_sendTrapStatus();

/** called to service a hardware initiated debug exception 
 * (single step, hardware breakpoints) */
void kdb_hardIsr(mal_vector_t vector, mal_trapFrame_t frame);
/** called to service a software initiated debug exception,
 * usually breakpoints, but also panics, and region violations */
void kdb_softIsr(mal_vector_t vector, mal_trapFrame_t frame);

#endif

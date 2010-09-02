#ifndef _PLATFORM_MAL_IDT_H_
#define _PLATFORM_MAL_IDT_H_

#include <platform/mal/desc.h>

typedef u_int16_t mal_vector_t;

void mal_idtInit();
void mal_setGateOffset(mal_gate_t *gat, pointer_t offset);
mal_gate_t* mal_gateAtVector(mal_vector_t vec);
void mal_setInterruptVector(mal_vector_t vec, void* isr);
void mal_setInterrupt(mal_vector_t vec, void* isr, int fixup);

/** attach 'isr' to vector 'vec'. it expects to be called 
 * as an normal interrupt. when invoked all interrupts are disabled  */
void mal_attachIsr(mal_vector_t vec, void* isr);
/** attach 'isr' to vector 'vec'. it does not disable interrupt.
 * some exceptions push an extra error code on the stack;
 * this function checks to see whether the requested vector behaves that way
 * and sets up a stack fixup */
void mal_attachException(mal_vector_t vec, void* isr);

/** by pusha */
struct mal_saved_registers {
  u_int32_t edi;
  u_int32_t esi;
  u_int32_t ebp;
  u_int32_t esp;
  u_int32_t ebx;
  u_int32_t edx;
  u_int32_t ecx;
  u_int32_t eax;
};

typedef struct mal_saved_registers mal_saved_registers_t;

struct mal_trapFrame {
  mal_saved_registers_t reg;
  union {
    struct {
      u_int32_t error;
      u_int32_t eip;
      u_int32_t cs;
      u_int32_t eflags;
    } withError;
    struct {
      u_int32_t eip;
      u_int32_t cs;
      u_int32_t eflags;
      u_int32_t dummy;
    } plain;
  };
};

typedef struct mal_trapFrame mal_trapFrame_t;

#endif

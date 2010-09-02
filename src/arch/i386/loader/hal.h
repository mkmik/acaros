#ifndef __HAL_H_
#define __HAL_H_

#include <types.h>

//#define PAGE_OFFSET 0x80000000 // used to shift from kernel virtual to physical mapping
#define PAGE_OFFSET 0x00000000
#define PAGE_SIZE 0x1000

/** pointer to an Interrupt Service Routine */
typedef void (*isr_t)();

/** sets the address of the isr entry point 
 * for the given interrupt vector */
void set_isr(int vector, isr_t isr);

/** disable interrupts (kernel mode only) */
#define cli() asm("cli" : : : "memory")
/** enable  interrupts (kernel mode only) */
#define sti() asm("sti" : : : "memory")

/** save EFLAGS register into "x" */
#define __save_flags(x) asm volatile("pushfl ; popl %0": "=g"(x))
/** load EFLAGS register from "x" */
#define __restore_flags(x) asm volatile("pushl %0 ; popfl":: "g"(x): "memory", "cc")

/** enable irq (should be in pic module) */
void enable_irq(int irq);
/** disable irq (should be in pic module) */
void disable_irq(int irq);

/** soft reboot the machine */
void reboot();

/**
 * Interrupt Service Routines (ISR) 
 * cannot be coded in C because they must
 * return using the "iret" instruction 
 * and because it must save all registers 
 * before doing anything.
 * This macro creates an assembler stub "name_isr" which
 * calls the C function "name_cisr" (which you must supply)
 * to handle the interrupt. 
 */
#define CREATE_ISR_DISPATCHER(name) \
void name ## _isr(); \
void name ## _cisr(); \
asm(".text\n" \
    ".globl " #name "_isr\n" \
    #name "_isr:\n" \
    "pusha\n" \
    "call " #name "_cisr\n" \
    "popa\n" \
    "iret\n" \
    ".data")

/** The exception dispatcher drops the error code
 * which some exceptions push on the stack after the
 * return address (most notably 14, page_fault) */
#define CREATE_EXCEPTION_DISPATCHER(name) \
void name ## _isr(); \
void name ## _cisr(); \
asm(".text\n" \
    ".globl " #name "_isr\n" \
    #name "_isr:\n" \
    "pusha\n" \
    "call " #name "_cisr\n" \
    "popa\n" \
    "add $4, %esp\n" \
    "iret\n" \
    ".data")


#endif

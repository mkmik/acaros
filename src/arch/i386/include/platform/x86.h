#ifndef __X86_H__
#define __X86_H__

/**
 * THIS FILE SHOULD BE GO INTO VARIOUS MODULES!!!!!!
 */

#include <autoconf.h>
#include <types.h>

/** save EFLAGS register into "x" */
#define __save_flags(x) asm volatile("pushfl ; popl %0": "=g"(x))
/** load EFLAGS register from "x" */
#define __restore_flags(x) asm volatile("pushl %0 ; popfl":: "g"(x): "memory", "cc")

/** Page Table Entry.
 * Page tables (and the page directory) are arrays of
 * pte_t elements. */
typedef struct {
  u_int32_t data;
} pte_t;


/** "paging enable" bit */
#define CR0_PG  0x80000000
/** "kernel write protect enable" bit */
#define CR0_WP  (1L<<16)
/** "page size extension enable" bit */
#define CR4_PSE (1L<<4)
/** "physical address extension enable" bit */
#define CR4_PAE (1L<<5)

#define WRITE_CR0(value) asm("mov %0, %%cr0" : :"r"(value))
#define READ_CR0(value) asm("mov %%cr0, %0" :"=r"(value))
#define WRITE_CR3(value) asm("mov %0, %%cr3" : :"r"(value))
#define READ_CR3(value) asm("mov %%cr3, %0"  :"=r"(value))
#define WRITE_CR4(value) asm("mov %0, %%cr4" : :"r"(value))
#define READ_CR4(value) asm("mov %%cr4, %0"  :"=r"(value))
#define WRITE_PAGE_DIRECTORY(value) WRITE_CR3(value)
#define READ_PAGE_DIRECTORY(value) READ_CR3(value)

/** debugger register */
#define WRITE_DR0(value) asm("mov %0, %%dr0" : :"r"(value))
#define READ_DR0(value) asm("mov %%dr0, %0" :"=r"(value))
#define WRITE_DR1(value) asm("mov %0, %%dr1" : :"r"(value))
#define READ_DR1(value) asm("mov %%dr1, %0" :"=r"(value))
#define WRITE_DR2(value) asm("mov %0, %%dr2" : :"r"(value))
#define READ_DR2(value) asm("mov %%dr2, %0" :"=r"(value))
#define WRITE_DR3(value) asm("mov %0, %%dr3" : :"r"(value))
#define READ_DR3(value) asm("mov %%dr3, %0" :"=r"(value))

#define WRITE_DR6(value) asm("mov %0, %%dr6" : :"r"(value))
#define READ_DR6(value) asm("mov %%dr6, %0" :"=r"(value))
#define WRITE_DR7(value) asm("mov %0, %%dr7" : :"r"(value))
#define READ_DR7(value) asm("mov %%dr7, %0" :"=r"(value))

/** enable paging (flushing instruction queue) */
#define ENABLE_PAGING() do { \
 u_int32_t tmp; \
 READ_CR0(tmp); \
 WRITE_CR0(tmp | CR0_PG | CR0_WP); \
 asm("jmp 1f\n1:\n\tmovl $1f,%0" : "=r"(tmp)); \
 asm("jmp *%0\n1:" : : "r"(tmp)); \
} while(0)

#define ENABLE_PSE() do { \
 u_int32_t tmp; \
 READ_CR4(tmp); \
 WRITE_CR4(tmp | CR4_PSE); \
} while(0)

/** flush only the tlb entry for the page which contains the specified address */
#if CONFIG_X86_INVLPG
#define flush_tlb_one(addr) asm volatile("invlpg %0": :"m" (*(char *) addr))
#else
#define flush_tlb_one(addr) flush_tlb()
#endif

#define flush_tlb()                                                   \
        do {                                                            \
                unsigned int tmpreg;                                    \
                                                                        \
                __asm__ __volatile__(                                   \
                        "movl %%cr3, %0;  # flush TLB \n"               \
                        "movl %0, %%cr3;              \n"               \
                        : "=r" (tmpreg)                                 \
                        :: "memory");                                   \
        } while (0)

/* code from linux msr.h: */

/* 
 * Access to machine-specific registers (available on 586 and better only)
 * Note: the rd* operations modify the parameters directly (without using
 * pointer indirection), this allows gcc to optimize better
 */

#define rdmsr(msr,val1,val2) \
     __asm__ __volatile__("rdmsr" \
                          : "=a" (val1), "=d" (val2) \
                          : "c" (msr))
#define wrmsr(msr,val1,val2) \
     __asm__ __volatile__("wrmsr" \
                          : /* no outputs */ \
                          : "c" (msr), "a" (val1), "d" (val2))

#define MSR_IA32_APICBASE               0x1b
#define MSR_IA32_APICBASE_BSP           (1<<8)
#define MSR_IA32_APICBASE_ENABLE        (1<<11)
#define MSR_IA32_APICBASE_BASE          (0xfffff<<12)
#define MSR_IA32_SYSENTER_CS            0x174
#define MSR_IA32_SYSENTER_ESP           0x175
#define MSR_IA32_SYSENTER_EIP           0x176


/*#if CONFIG_MPENTIUMIII
#define ARCH_HAS_PREFETCH
extern inline void prefetch(const void *x)
{
        __asm__ __volatile__ ("prefetchnta (%0)" : : "r"(x));
}
#else
static inline void prefetch(const void *x) {;}
#endif
*/
/*
 * Generic CPUID function
 */
static inline void cpuid(int op, int *eax, int *ebx, int *ecx, int *edx) {
        __asm__("cpuid"
                : "=a" (*eax),
                  "=b" (*ebx),
                  "=c" (*ecx),
                  "=d" (*edx)
                : "0" (op));
}

/*
 * CPUID functions returning a single datum
 */
static inline unsigned int cpuid_eax(unsigned int op) {
        unsigned int eax;

        __asm__("cpuid"
                : "=a" (eax)
                : "0" (op)
                : "bx", "cx", "dx");
        return eax;
}

static inline unsigned int cpuid_ebx(unsigned int op) {
        unsigned int eax, ebx;

        __asm__("cpuid"
                : "=a" (eax), "=b" (ebx)
                : "0" (op)
                : "cx", "dx" );
        return ebx;
}

static inline unsigned int cpuid_ecx(unsigned int op) {
        unsigned int eax, ecx;

        __asm__("cpuid"
                : "=a" (eax), "=c" (ecx)
                : "0" (op)
                : "bx", "dx" );
        return ecx;
}

static inline unsigned int cpuid_edx(unsigned int op) {
        unsigned int eax, edx;

        __asm__("cpuid"
                : "=a" (eax), "=d" (edx)
                : "0" (op)
                : "bx", "cx");
        return edx;
}


/* end linux */

#endif

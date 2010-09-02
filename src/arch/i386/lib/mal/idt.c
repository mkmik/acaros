#include <platform/mal/idt.h>
#include <log/log.h>
#include <string.h>

mal_gate_t mal_idtTable[256] __attribute__((aligned(8)));

struct {
  u_int16_t len;
  mal_gate_t* table;
} PACKED mal_idtDesc __attribute__((aligned(4))) = {256*8-1, mal_idtTable} ;

mal_gate_t* mal_gateAtVector(mal_vector_t vec) {
  return &mal_idtTable[vec];
}

void mal_setGateOffset(mal_gate_t *gat, pointer_t offset) {
  gat->offset_lo = offset & 0xFFFF;
  gat->offset_hi = offset >> 16;
}

void mal_setInterruptVector(mal_vector_t vec, void* isr) {
  mal_gate_t *in = mal_gateAtVector(vec);
  mal_setGateOffset(in, (pointer_t)isr);
  in->selector = __KERNEL_CS;
  in->present = 1;
  in->type = TYPE_INTGATE;  
}

void mal_interruptTemplate();
void mal_interruptTemplateEnd();
void mal_interruptTemplateAddress();
void mal_interruptTemplateVector();
void mal_interruptTemplateFixup();
void mal_unknownInterrupt(mal_vector_t vector) {
  panic("unknown interrupt 0x%x\n", vector);
}

typedef u_int8_t mal_isrTrampoline_t[64];
mal_isrTrampoline_t mal_isrTrampolines[256];

void mal_isrTrampolineInit(mal_vector_t vec, mal_isrTrampoline_t *tramp) {
  memcpy(tramp, mal_interruptTemplate,
	 sizeof(mal_isrTrampoline_t));
  mal_vector_t *vector = (mal_vector_t*)
    (((u_int8_t*)&mal_isrTrampolines[vec])+
     (mal_interruptTemplateVector-mal_interruptTemplate));
  *(vector-1) = (mal_vector_t)vec;
}

void mal_setInterrupt(mal_vector_t vec, void* isr, int fixup) {
  pointer_t *addr = (pointer_t*)
    (((u_int8_t*)&mal_isrTrampolines[vec])+
     (mal_interruptTemplateAddress-mal_interruptTemplate));
  *(addr-1) = (pointer_t)isr;
  u_int8_t *add = 
    (((u_int8_t*)&mal_isrTrampolines[vec])+
     (mal_interruptTemplateFixup-mal_interruptTemplate));
  *(add-1) = (u_int8_t)fixup;

  mal_idtTable[vec].type = TYPE_INTGATE;
}

void mal_attachIsr(mal_vector_t vec, void* isr) {
  mal_setInterrupt(vec, isr, 0);
}

void mal_attachException(mal_vector_t vec, void* isr) {
  int hasFixup = (vec == 8 || 
		  vec == 10 || 
		  vec == 11 || 
		  vec == 12 || 
		  vec == 13 || 
		  vec == 14 || 
		  vec == 17);
  mal_setInterrupt(vec, isr, hasFixup ? sizeof(u_int32_t) : 0);
  mal_idtTable[vec].type = TYPE_TRAPGATE;
}

void mal_idtInit() {
  klogf(LOG_MESSAGE, "Initializing IDT\n");
  asm("lidt %0" : : "m"(mal_idtDesc));

  int i;
  for(i=0; i<256; i++) {
    mal_isrTrampolineInit(i, &mal_isrTrampolines[i]);
    mal_setInterruptVector(i, &mal_isrTrampolines[i]);
  }

}

asm(".globl mal_interruptTemplate\n"
    "mal_interruptTemplate:\n"
    "pusha\n"
    "mov %esp, %ebp\n"
    "pushw $0x0DAB\n"  // align to gcc calling convention: long words naturally aligned
    "pushw $0xBAD0\n" 
    "mal_interruptTemplateVector:\n"
    "mov $mal_unknownInterrupt, %edx\n"
    "mal_interruptTemplateAddress:\n"
    "call *%edx\n"
    "mov %ebp, %esp\n"
    "popa\n"
    "add $0, %esp\n"
    "mal_interruptTemplateFixup:\n"
    "iret\n"
    ".globl mal_interruptTemplateEnd\n"
    "mal_interruptTemplateEnd:");

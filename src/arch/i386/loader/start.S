#define LOADER_STACK_SIZE 4096

.text
	.globl _start
_start:
	lea loader_stack_end, %esp
	push %ebx
	push %eax
	cld
	call start
	
.data
	.align 4
loader_stack:
	.fill LOADER_STACK_SIZE
loader_stack_end:

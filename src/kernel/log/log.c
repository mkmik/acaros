#include <log/log.h>
#include <stdarg.h>
#include <stand/libc.h>
#include <platform/mal/interrupt.h>
#include <platform/kdb/kdb.h>
#include <autoconf.h>

static log_level_t log_level = LOG_DEBUG;

void klogf(log_level_t level, const char* format, ...) {
  va_list args;
  va_start(args, format);
  
  vklogf(level , format, args);

  va_end(args);
}

void vklogf(log_level_t level, const char* format, va_list args) {
  if(level < log_level)
    return;

  if(level == LOG_CRITICAL)
    printf("CRITICAL: ");
  else if(level == LOG_PANIC) 
    printf("PANIC: ");

  vprintf(format, args);
}

/** move in platfrom specific **/
static void printMachineState() {
  u_int32_t esp;
  u_int32_t ebp;
  u_int32_t eax;
  u_int32_t ebx;
  u_int32_t ecx;
  u_int32_t edx;
  u_int32_t eflags;

  asm("pushf\n"
      "pop %0\n"
      "mov %%esp, %1\n"
      "mov %%ebp, %2\n"
      "mov %%eax, %3\n"
      "mov %%ebx, %4\n"
      "mov %%ecx, %5\n"
      "mov %%edx, %6\n" 
      :
      "=m"(eflags),
      "=m"(esp),
      "=m"(ebp),
      "=m"(eax),
      "=m"(ebx),
      "=m"(ecx),
      "=m"(edx)
      );

  klogf(LOG_MESSAGE, "\n ---- register dump ----\n");
  klogf(LOG_MESSAGE, "esp %x ebp %x eflags %x\n", esp, ebp, eflags);
  klogf(LOG_MESSAGE, "eax %x ebx %x ecx %x edx %x\n", eax, ebx, ecx, edx);
}

/** move in platfrom specific **/
static void freezeMachine() __attribute__ ((noreturn));
static void freezeMachine() {
#ifdef CONFIG_KDB
  kdb_breakpoint();
#else
  mal_disable_interrupts();
#endif
  while(1);
}

void panic(const char* format, ...) {
  va_list args;
  va_start(args, format);

  vklogf(LOG_PANIC, format, args);
  printMachineState();

  freezeMachine();
}

log_level_t log_setLevel(log_level_t min) {
  log_level_t old = log_level;
  log_level = min;
  return old;
}

log_level_t log_getLevel() {
  return log_level;
}

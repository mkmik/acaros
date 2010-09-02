#ifndef _KDB_H_
#define _KDB_H_

#include <mal/interrupt.h>

/** initializes the kernel debugger */
void kdb_init();

/** called to service a debug interruption
 * caused by debug exception, breakpoint exception or uart CTRL-C.
 *  it assumes that the caller has setup the kdb_currentContext
 * with the interrupt frame beloging to the halted thread.
 */
void kdb_service();

/** suspends the execution of the calling thread.
 * must be called from inside the uart interrupt handler.
 *  techincally, it acknowledges the interrupt but doesn't return to
 * the interrupted thread until it is explicitly released with
 * kdb_resume()
 */
void kdb_suspend();

/** resume the suspension actuated by kdb_suspend.
 */
void kdb_resume();

/** reads a byte at address addr, returning 0 if the page is invalid */
u_int8_t kdb_readByte(pointer_t addr);

/** ignores the write if the page is invalid */
void kdb_writeByte(pointer_t addr, u_int8_t byte);

/** true if a debugger is currently attached */
extern int kdb_attached;

/** true if the debugger is suspended. 
 * kdb_suspend spins on this value. kdb_resume releases kdb_suspend
 * by setting this variable to 0 */
extern volatile int kdb_suspended;

#endif

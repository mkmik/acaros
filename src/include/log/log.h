#ifndef _LOG_H_
#define _LOG_H_

#include <stdarg.h>

/** log levels, in order of importance */
typedef enum {LOG_DEBUG, LOG_MESSAGE, LOG_CRITICAL, LOG_PANIC} log_level_t; 

/** logs a string to the loggin device.
 * the loggin device can be bound to the screen, serial port,
 * or a in memory ring buffer, it doesn't matter. This functions 
 * uses the currently active method.
 *  The message is specified as in printf */
void klogf(log_level_t level, const char* format, ...) __attribute__ ((format (printf, 2, 3)));

/** Like klogf but accepts a va_list
 */
void vklogf(log_level_t level, const char* format, va_list args);
     
/** panics the system and prints the message specified as in printf */
void panic(const char* format, ...) __attribute__ ((noreturn, format (printf, 1,2)));

/** messages below (<) this level will not be displayed.
 * returns the old level */
log_level_t log_setLevel(log_level_t min);
/** returns the current log level */
log_level_t log_getLevel();

#include <debug.h>

#endif

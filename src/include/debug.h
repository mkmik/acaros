#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <types.h>
#include <log/log.h>

/** if the espression is false -> kernel panic.
 * useful for debugging */
#define assert(expression) \
 if(!(expression)) {\
   klogf(LOG_PANIC, "ASSERTION FAILED: '%s' -> %d file %s:%d %s\n", #expression, (int)expression, \
	  __FILE__, __LINE__, __FUNCTION__); \
   panic("ASSERTION FAILED"); \
 }

#endif

#ifndef __TYPES_H__
#define __TYPES_H__

/** tells GCC to keep structures as they are defined
 * without inserting padding bytes in order to align fields. 
 * useful when the structure defines an external type 
 * (hardware, network, etc) */
#define PACKED __attribute((packed))


#ifdef ACAROS

#include <platform/types.h>

/** generic size of something */
typedef unsigned int size_t;
/** generic offset of something */
typedef unsigned int offset_t;
/** boolean */
typedef int bool_t;

#endif

#endif


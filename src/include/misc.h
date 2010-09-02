#ifndef _MISC_H_
#define _MISC_H_

/** returns the value with bit bit set if en != 0 or cleared if en == 0 */
#define BIT_ASSIGN(value, bit, en) ((en) ? ((value) | (1<<(bit))) : ((value) & ~(1<<(bit))))
/** returns the value with bit bit set */
#define BIT_SET(value, bit) BIT_ASSIGN(value, bit, 1)
/** returns the value with bit bit cleared */
#define BIT_CLEAR(value, bit) BIT_ASSIGN(value, bit, 0)
/** returns 1 if the bit bit in value is set */
#define BIT_TEST(value, bit) (value & (1<<bit))

/** typesafe minimum of a and b */
#define MIN(a, b) ({__typeof__(a) aa = (a); __typeof__(b) bb = (b); (aa < bb) ? (aa) : (bb);})
/** typesafe minimum of a and b */
#define MAX(a, b) ({__typeof__(a) aa = (a); __typeof__(b) bb = (b); (aa > bb) ? (aa) : (bb);})

#endif

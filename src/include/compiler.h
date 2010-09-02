#ifndef __COMPILER_H_
#define __COMPILER_H_

/* Somewhere in the middle of the GCC 2.96 development cycle, we implemented
   a mechanism by which the user can annotate likely branch directions and
   expect the blocks to be reordered appropriately.  Define __builtin_expect
   to nothing for earlier compilers.  */

#if __GNUC__ == 2 && __GNUC_MINOR__ < 96
#define __builtin_expect(x, expected_value) (x)
#endif

/* give the compiler a hint: the expression is likely to be true
 */
#define likely(x)       __builtin_expect((x),1)
/* give the compiler a hint: the expression is likely to be false
 */
#define unlikely(x)     __builtin_expect((x),0)

#endif

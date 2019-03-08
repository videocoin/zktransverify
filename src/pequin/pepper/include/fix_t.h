#ifndef FIX_T_H
#define FIX_T_H

#include <stdint.h>

typedef int32_t fix_t;
typedef uint32_t ufix_t;

#define FIX_SCALE 0x10000
#define FIX_SCALE_LOG2 16
#define FIX_T_MIN -2147483648
#define FIX_T_MAX 2147483647
#define UFIX_T_MIN 0
#define UFIX_T_MAX 4294967295

//Arithmetic routines have no overflow handling, 
// (i.e. overflows are undefined) and always truncate towards zero
fix_t fix_add(fix_t a, fix_t b);
fix_t fix_mul(fix_t a, fix_t b);
fix_t fix_div(fix_t a, fix_t b);
fix_t fix_sqrt(fix_t a);

ufix_t ufix_add(ufix_t a, ufix_t b);
ufix_t ufix_mul(ufix_t a, ufix_t b);
ufix_t ufix_div(ufix_t a, ufix_t b);
ufix_t ufix_sqrt(ufix_t a);

#endif //FIX_T_H

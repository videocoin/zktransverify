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

//Arithmetic routines do not overflow, and always truncate towards zero
fix_t fix_add(fix_t a, fix_t b);
fix_t fix_mul(fix_t a, fix_t b);
fix_t fix_div(fix_t a, fix_t b);

// The result must be in the closed interval [0, 2^bits - 1]
uint64_t uint64sqrt(uint64_t a, int bits);

fix_t fix_sqrt(fix_t a);

fix_t int_to_fix(int32_t val);

int32_t fix_to_int(fix_t val);

fix_t fix_ceil(fix_t val);

//Arithmetic routines do not overflow, and always truncate towards zero
ufix_t ufix_add(ufix_t a, ufix_t b);
ufix_t ufix_mul(ufix_t a, ufix_t b);
ufix_t ufix_div(ufix_t a, ufix_t b);

ufix_t ufix_sqrt(ufix_t a);

ufix_t uint_to_ufix(uint32_t val);

uint32_t ufix_to_int(ufix_t val);

ufix_t ufix_ceil(ufix_t val);

#endif //FIX_T_H

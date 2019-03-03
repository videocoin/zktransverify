; TinyRAM V=1.00 W=32 K=10

; Reads image from input tape
; TODO: Read from aux tape second image
; calculates SSIM.

; begin prelude
store.w 0, r0
mov r0, iba0 ; iba0 - input base address
read r1, 0
cjmp 7
add r0, r0, 4
store.w r0, r1
jmp 2
store.w iba0, r0
; end prelude

; iba1 = iba + 1 * W / 8bit ;
; iba0 = 2147483648 ; The first word of the primary input tape should be here. (memory 0x8000 0000, this is 1<<(w-1))
; mba0 = 2147483644
; mba1 = mba0 - 1 * W / 8bit
; ...
; mbaN = mba0 - N * W / 8bit

mov r0, 16
store.w mba0, r0 ; store width in memory
store.w mba1, r0 ; store height in memory

; width >>= 2;
load.w r1, mba0
shr r1, r1, 2
store.w mba0, r1

; height >>= 2;
load.w r2, mba1
shr r2, r2, 2
store.w mba1, r2

; int (*sum0)[4] = buf;
; sum0 = mba128
mov r1, mba128
store.w mba2, r1 ; pointer to mba128
; int (*sum1)[4] = sum0 + (width >> 2) + 3;
; sum1 = mba[128 + (val(mba0) + 3) * W / 8 bit]
; sum1 = mba[128 + 7 * 4]
; sum1 = mba156
mov r1, mba156
store.w mba3, r1; pointer to mba156

; stride1
mov r0, 16
store.w mba4, r0
; stride2
mov r0, 16
store.w mba5, r0

; z = 0
mov r5, 0

; =============================================================================
;      START: for( int y = 1; y < height; y++ )
; =============================================================================
mov r6, 1 ; y
load.w r7, mba1 ; height
; start _loop1
_loop1:
cmpe r6, r7 ; y < height
cjmp _endloop1
; body
; -----------------------------------------------------------------------------

; =============================================================================
;          START: for( ; z <= y; z++ )
; =============================================================================
; start loop2
_loop2:
cmpa r5, r6
cjmp _endloop2
; body
; -----------------------------------------------------------------------------

; XCHG( void*, sum0, sum1 );
load.w r0, mba2 ; sum0
load.w r1, mba3 ; sum1
store.w mba3, r0
store.w mba2, r1

; =============================================================================
;                 START: for( int x = 0; x < width; x+=2 )
; =============================================================================
; start loop3
mov r3, 0 ; x = 0
load.w r4, mba1
; start
_loop3:
cmpae r3, r4
cjmp _endloop3
; body
; -----------------------------------------------------------------------------

; ssim_4x4x2_core(&pix1[4 * (x + z * stride1)], stride1, &pix2[4 * (x + z * stride2)], stride2, &sum0[x]);
; 1. 4 * (x + z * stride1)
mov r0, mba4 ; stride1
mull r0, r0, r5 ; z * stired1
add r0, r0, r3 ; x + z * stride1
mull r0, r0, 4 ;
store.w mba6, r0 ; first index
; 2. 4 * (x + z * stride2)
mov r0, mba5 ; stride2
mull r0, r0, r5
add r0, r0, 3
mull r0, r0, 4
store.w mba7, r0
; &sum0[x]
mull r0, r3, 4; sizeof word
load.w r1, mba2; load sum0 address
add r1, r1, r0
store.w mba8, r1

jmp _ssim4x4x2core

_endssim4x4x2core:
; -----------------------------------------------------------------------------
add r3, r3, 2
jmp _loop3
_endloop3:
; =============================================================================
;                   END: for( int x = 0; x < width; x+=2 )
; =============================================================================

; -----------------------------------------------------------------------------
add r5, r5, 1
jmp _loop2
_endloop2:
; =============================================================================
;            END: for( ; z <= y; z++ )
; =============================================================================

; -----------------------------------------------------------------------------

add r6, r6, 1 ; y++
jmp _loop1
_endloop1:
; =============================================================================
;        END: for( int y = 1; y < height; y++ )
; =============================================================================

mov r0, 0
answer r0

; ssim_4x4x2_core
; iba0 - start of image array, for now we have one image
; mba6 - offset to first image
; mba7 - offset to second image
; mba8 - sum
_ssim4x4x2core:

mov r0, iba0
load.w r1, mba6
add r0, r0, r1 ; start of pix1
store.w mba9, r0

; =============================================================================
;       START: for( int z = 0; z < 2; z++ )
; =============================================================================
mov r8, 0; z = 0
_ssimcoreloop1:
cmpe r8, 2
cjmp _endssimcoreloop1

; unsigned int s1 = 0, s2 = 0, ss = 0, s12 = 0;
; mba10 - s1
; mba11 - s2
; mba12 - ss
; mba13 - s12
mov r0, 0
store.w mba10, r0
store.w mba11, r0
store.w mba12, r0
store.w mba13, r0

; =============================================================================
;               START: for( int y = 0; y < 4; y++ )
; =============================================================================

mov r9, 0; y = 0
_ssimcoreloop2:
cmpe r9, 4
cjmp _endssimcoreloop2



; sums[z][0] = s1;

load.w r0, mba8 ; address of sums
mull r1, r8, 16 ; z + 0
add r0, r0, r1 ;  address of sums[z][0]


; sums[z][1] = s2;
; sums[z][2] = ss;
; sums[z][3] = s12;




add r9, r9, 1
jmp _ssimcoreloop2
_endssimcoreloop2:

; =============================================================================
;                 END: for( int y = 0; y < 4; y++ )
; =============================================================================


add r8, r8, 1
jmp _ssimcoreloop1
_endssimcoreloop1:
; =============================================================================
;         END: for( int z = 0; z < 2; z++ )
; =============================================================================

jmp _endssim4x4x2core

;
; ProfilerARMLowLevel.s
;
; @@@bmarine1
; TODO: differentiate between LE and BE, THUMB CPU and not THUMB CPU!
; Currently: LE only, THUMB CPU
; bmarine1@@@
;
; The assembly needed to invoke the profiler under PowerPC
;
;  Copyright © 1993-1994 Metrowerks Inc.  All rights reserved
;  

; addTime(dest, src1, src2)

  .public addTime
  .public subTime

  .text

addTime:
  ldr    r3, [ r2, #4 ]                ; src2.hi (LE ONLY!)
  ldr    r2, [ r2, #0 ]                ; src2.lo
  ldr    r12, [ r1, #4 ]               ; src1.hi (LE ONLY!)
  ldr    r1, [ r1, #0 ]                ; src1.lo
  adds   r2, r2, r1                    ; add low parts
  adc    r3, r3, r12                   ; add high parts
  str    r3, [ r0, #4 ]                ; store high part
  str    r2, [ r0, #0 ]                ; store low part
  bx     lr
  
; subTime(dest, src1, src2)

subTime:
  ldr    r3, [ r2, #4 ]                ; src2.hi (LE ONLY!)
  ldr    r2, [ r2, #0 ]                ; src2.lo
  ldr    r12, [ r1, #4 ]               ; src1.hi (LE ONLY!)
  ldr    r1, [ r1, #0 ]                ; src1.lo
  subs   r2, r2, r1                    ; sub low parts
  sbc    r3, r3, r12                   ; sub high parts
  str    r3, [ r0, #4 ]                ; store high part
  str    r2, [ r0, #0 ]                ; store low part
  bx     lr

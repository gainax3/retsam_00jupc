/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tests - 
  File:     qsort.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: qsort.c,v $
  Revision 1.6  2006/05/23 08:23:35  kitase_hirotake
  numが0,1の場合に発生する不具合の修正

  Revision 1.5  2006/01/18 02:12:28  kitase_hirotake
  do-indent

  Revision 1.4  2005/09/09 08:59:42  takano_makoto
  不具合修正

  Revision 1.3  2005/06/16 04:42:44  takano_makoto
  オーバーアクセスが発生してしまう不具合を修正

  Revision 1.2  2005/04/13 00:15:44  takano_makoto
  引数の型をintからu32に変更

  Revision 1.1  2005/04/12 10:15:15  takano_makoto
  Initial update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>

/* ------------------------------------------- *
 * 再帰非使用のクイックソート
 * ------------------------------------------- */
/*---------------------------------------------------------------------------*
  Name:         MATH_QSort
  
  Description:  再帰を使用せずにクイックソートを行ないます。
                ソート用のバッファ領域を渡す必要があり、
                必要なバッファサイズはMATH_QSORT_STACK_SIZE( num )で取得できます。
  
  Arguments:    head     ソートするデータへのポインタ
                num      ソートするデータの個数
                width    ソートするデータ１つ分のデータサイズ
                comp     比較関数ポインタ
                stackBuf ソート用バッファ
                
  Returns:      None.
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>
asm void MATH_QSort( register void* head,  register u32 num,  register u32 width,  register MATHCompareFunc comp, void* stackBuf )
{
#define stack       r4
#define stackBuf_   r5
#define left        r6
#define right       r7
#define somewhere   r6
#define somewhere_l r9
#define somewhere_r r10
#define tmp         r2
#define tmp2        r3
#define comp_       r11
#define width_      r8
    
    stmfd   sp!, {r4-r11,lr}
    cmp     num, #1
    ble     @fin
    
    ldr     stack, [sp, #36]                //  stack = stackBuf;
    mov     comp_, comp                     //  // r0-r3,r12はスクラッチレジスタなので比較関数呼び出し前に退避
    mov     width_, width
    
    cmp     stack, #0
    bne     @00
    clz     tmp, num                        //  // ワークバッファがNULLの場合はスタックから確保する。
    rsb     tmp, tmp, #32
    mov     tmp, tmp, lsl #3                //  // 必要ワークサイズは、(ILog2(num) + 1) * 4 * 2 
    sub     sp, sp, tmp
    mov     stack, sp
    str     tmp, [sp, #-4]!
    
@00:
    sub     num, num, #1            
    mla     num, num, width_, head   
    
    mov     stackBuf_, stack                //  // 初期値をPUSHする。これ以降headとnumは使用しないので破棄。
    str     head, [stack], #4               //  PUSH(head);
    str     num, [stack], #4                //  PUSH(head + (num-1) * width);
    
    clz     tmp, width_                     //  // ILog2(width)+1を計算しておく(後で基準値選びに使用)
    rsb     tmp, tmp, #32
    str     tmp, [sp, #-4]!
    
@01:
    cmp     stack, stackBuf_                //  while ( stack != stackBuf ) {
    beq     @end
    
    ldr     right, [stack, #-4]             //      POP(right)
    ldr     left,  [stack, #-8]!            //      POP(left)
    
    sub     tmp, right, left        
    cmp     tmp, width_             
    bne     @02                             //      if ( (right - left) == width ) {
    mov     r0, left
    mov     r1, right
    blx     comp_
    cmp     r0, #0                          //          if ( comp( left, right ) > 0 ) {
    ble     @01
                                            //              swap( left, right, width ); /* swapここから*/
    mov     r0, width_
    tst     r0, #3
    beq     @012
@011:
    ldrb    r1, [left]
    subs    r0, r0, #1
    swpb    r1, r1, [right]
    add     right, right, #1
    strb    r1, [left], #1
    bne     @011
    b       @01
    
@012:
    ldr     r1, [left]
    subs    r0, r0, #4
    swp     r1, r1, [right]
    add     right, right, #4
    str     r1, [left], #4
    bne     @012
    b       @01
                                            //              /* swapここまで */

                                            //          }
                                            //          continue;
@02:                                        //      }
                                            //      // 最初に計算したILog2(width)を利用して中位置あたりの基準値を選ぶ
                                            //      somewhere = (((right - left) >> ILog2(width)) * width / 2) + left;
    ldr     tmp2, [sp]
    sub     tmp, right, left
    mov     tmp, tmp, lsr tmp2
    mla     tmp, tmp, width_, left
                                            //      swap( left, left + width * 2, width ); /* swapここから */
    mov     r3, left
    mov     r0, width_
    mov     r2, tmp
    tst     r0, #3
    beq     @022
@021:
    ldrb    r1, [r2]
    subs    r0, r0, #1
    swpb    r1, r1, [r3]
    add     r3, r3, #1
    strb    r1, [r2], #1
    bne     @021
    b       @023
    
@022:
    ldr     r1, [r2]
    subs    r0, r0, #4
    swp     r1, r1, [r3]
    add     r3, r3, #4
    str     r1, [r2], #4
    bne     @022
                                            //      /* swapここまで */
@023:
    mov     somewhere_l, left               //      somewhere_l = left + width;
    mov     somewhere_r, right              //      somewhere_r = right;
    add     somewhere_l, somewhere_l, width_ 
@03:                                        //      do {
    cmp     somewhere_l, right
    bge     @04
    mov     r1, somewhere
    mov     r0, somewhere_l
    blx     comp_
    cmp     r0, #0                          //          while( somewhere_l < right && comp( somewhere_l, somewhere ) < 0 ) {
    addlt   somewhere_l, somewhere_l, width_ //             somewhere_l += width;
    blt     @03                             //          }
@04:
    mov     r1, somewhere
    mov     r0, somewhere_r
    blx     comp_
    cmp     r0, #0                          //          while( comp( somewhere_r, somewhere ) > 0 ) {
    subgt   somewhere_r, somewhere_r, width_ //             somewhere_r -= width;
    bgt     @04                             //          }
    
    cmp     somewhere_l, somewhere_r        //          if ( somewhere_l < somewhere_r ) {
    bge     @05
    
                                            //             swap( somewhere_l, somewhere_r, width ) /* swap ここから */;
    mov     r2, somewhere_l
    mov     r3, somewhere_r
    mov     r0, width_
    tst     r0, #3
    beq     @042
@041:
    ldrb    r1, [r2]
    subs    r0, r0, #1
    swpb    r1, r1, [r3]
    add     r3, r3, #1
    strb    r1, [r2], #1
    bne     @041
    b       @043
    
@042:
    ldr     r1, [r2]
    subs    r0, r0, #4
    swp     r1, r1, [r3]
    add     r3, r3, #4
    str     r1, [r2], #4
    bne     @042
                                            //              /* swapここまで */

@043:

    add     somewhere_l, somewhere_l, width_ //             somewhere_l += width;
    sub     somewhere_r, somewhere_r, width_ //             somewhere_r -= width;
                                            //          }
    cmp     somewhere_l, somewhere_r
    ble     @03                             //      } while ( somewhere_l <= smewhere_r );
    
@05:
                                            //      swap( left, somewhere_r, width ); /* swapここから*/
    mov     r2, left
    mov     r3, somewhere_r
    mov     r0, width_
    tst     r0, #3
    beq     @052
@051:
    ldrb    r1, [r2]
    subs    r0, r0, #1
    swpb    r1, r1, [r3]
    add     r3, r3, #1
    strb    r1, [r2], #1
    bne     @051
    b       @053
    
@052:
    ldr     r1, [r2]
    subs    r0, r0, #4
    swp     r1, r1, [r3]
    add     r3, r3, #4
    str     r1, [r2], #4
    bne     @052
                                            //      /* swapここまで */
@053:
    
    sub     tmp, somewhere_r, left
    sub     tmp2, right, somewhere_r        //      /* できるだけスタックを消費しない為に小さな範囲からソートしていく */
    cmp     tmp, tmp2                       //      if ( somwehre_r - left > right - somewhere_r ) {
    ble     @06
    
    sub     tmp, somewhere_r, width_        //          if ( left < somewhere_r - width ) {
    cmp     left, tmp
    
    strlt   left, [stack], #4               //              PUSH(left);
    strlt   tmp,  [stack], #4               //              PUSH(somewhere_r - width);
                                            //          }
    add     tmp, somewhere_r, width_        //          if ( somewhere_r + width < right ) {
    cmp     tmp, right
    strlt   tmp, [stack], #4                //              PUSH( somewhere_r + width );
    strlt   right, [stack], #4              //              PUSH( right );
                                            //          }
    b       @01
    
@06:                                        //      } else {
    add     tmp, somewhere_r, width_        //          if ( somewhere_r + width < right ) {
    cmp     tmp, right
    strlt   tmp, [stack], #4                //              PUSH( somewhere_r + width );
    strlt   right, [stack], #4              //              PUSH( right );
                                            //          }
    sub     tmp, somewhere_r, width_        //          if ( left < somewhere_r - width ) {
    cmp     left, tmp
    strlt   left, [stack], #4               //              PUSH(left);
    strlt   tmp,  [stack], #4               //              PUSH(somewhere_r - width);
                                            //          }
    b       @01                             //      }
                                            //  }
@end:
    add     sp, sp, #4
    sub     stack, stack, #4
    cmp     stack, sp
    ldreq   r0, [sp]
    addeq   r0, r0, #4
    addeq   sp, sp, r0
    
@fin:
    ldmfd   sp!,{r4-r11,lr}
    bx      lr
    
#undef stack
#undef stackBuf_
#undef left
#undef right
#undef somewhere
#undef somewhere_l
#undef somewhere_r
#undef tmp
#undef tmp2
#undef comp_
#undef width_

}
#include <nitro/codereset.h>

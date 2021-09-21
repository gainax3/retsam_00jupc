/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MI
  File:     mi_memory.c

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mi_memory.c,v $
  Revision 1.20  2005/08/19 11:05:50  yada
  back to 1.18

  Revision 1.19  2005/08/19 07:49:15  yada
  add MI_StrNCpy(), MI_StrCpy()

  Revision 1.18  2005/03/01 01:57:00  yosizaki
  copyright の年を修正.

  Revision 1.17  2005/02/28 05:26:25  yosizaki
  do-indent.

  Revision 1.16  2004/11/18 01:57:03  yada
  MIi_CpuClear32 is available also on ARM9

  Revision 1.15  2004/11/02 09:57:54  yada
  just fix comment

  Revision 1.14  2004/10/29 08:05:38  yasu
  MIi_CpuClear32 moves into crt0.c
  Change position of DTCM clearing

  Revision 1.13  2004/09/21 13:12:29  yada
  consider for SDK_SMALL_BUILD

  Revision 1.12  2004/09/21 12:47:03  yada
  consider for SDK_SMALL_BUILD

  Revision 1.11  2004/07/06 04:55:37  yada
  arrange comment and change a little in MI_CpuCopyFast

  Revision 1.10  2004/05/20 10:09:30  yasu
  Comment out CW_BUG_FOR_LDRH_AND_STRH

  Revision 1.9  2004/04/27 04:08:49  yosizaki
  add MI_CpuCopy8, MI_CpuFill8.

  Revision 1.8  2004/04/15 09:09:45  takano_makoto
  Add MI_Copy16B

  Revision 1.7  2004/03/23 00:58:15  yada
  only fix comment

  Revision 1.6  2004/02/13 06:28:35  yada
  only change comment

  Revision 1.5  2004/02/12 00:16:13  nishida_kenji
  Reduce use of stack in MI_CopyXX.

  Revision 1.4  2004/02/10 08:37:42  yada
  MI_Copy48B, MI_Copy128B で退避レジスタ変更

  Revision 1.3  2004/02/10 06:01:33  nishida_kenji
  Add MI_CopyxB, MI_ZeroxB.

  Revision 1.2  2004/02/10 02:27:30  yada
  code32で囲う

  Revision 1.1  2004/02/10 01:22:22  yada
  UTL_ からの移行

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/types.h>
#include <nitro/mi/memory.h>


//****バグ対策****
//  CW のバグで、ldrh や strh といった、ハーフワードアクセス命令が
//  inline assembler で通らないので、直に命令の値を dcd で書いて
//  回避する。バグがなおったら下のdefine は削除する。

//  Fixed with CodeWarrior 0.4 or later
//#define CW_BUG_FOR_LDRH_AND_STRH


#define HALFW_CONDAL  0xe0000000       // condition(ALL)
#define HALFW_CONDNE  0x10000000       // condition(NE)
#define HALFW_CONDEQ  0x00000000       // condition(EQ)

#define HALFW_OFF_PL  0x00800000       // offset plus
#define HALFW_OFF_MI  0x00000000       // offset minus
#define HALFW_LOAD    0x00100000       // load
#define HALFW_STORE   0x00000000       // store
#define HALFW_RN(n)   ((n)<<16)        // register Rn
#define HALFW_RD(n)   ((n)<<12)        // register Rd

#define HALFW_DEF1    0x004000B0       // fixed
#define HALFW_DEF2    0x014000B0       // fixed

#define HALFW_IMM(n)   ( ((n)&0xf)  |  (((n)&0xf0)<<4) )        // immediate


#define HALFW_DCD( cond, d, n, offset, sign, ldst, def ) \
   dcd (def)|(cond)|(sign)|(ldst)|HALFW_RN(n)|HALFW_RD(d)|HALFW_IMM(offset)

//---- ldrh Rn, [Rd], +#offset
#define LDRH_AD1( cond, d, n, offset ) \
   HALFW_DCD( cond, d, n, offset, HALFW_OFF_PL, HALFW_LOAD, HALFW_DEF1 )

//---- ldrh Rn, [Rd, +#offset]
#define LDRH_AD2( cond, d, n, offset ) \
   HALFW_DCD( cond, d, n, offset, HALFW_OFF_PL, HALFW_LOAD, HALFW_DEF2 )

//---- ldrh Rn, [Rd], -#offset
#define LDRH_AD3( cond, d, n, offset ) \
   HALFW_DCD( cond, d, n, offset, HALFW_OFF_MI, HALFW_LOAD, HALFW_DEF1 )

//---- ldrh Rn, [Rd, -#offset]
#define LDRH_AD4( cond, d, n, offset ) \
   HALFW_DCD( cond, d, n, offset, HALFW_OFF_MI, HALFW_LOAD, HALFW_DEF2 )

//---- strh Rn, [Rd], +#offset
#define STRH_AD1( cond, d, n, offset ) \
   HALFW_DCD( cond, d, n, offset, HALFW_OFF_PL, HALFW_STORE, HALFW_DEF1 )

//---- strh Rn, [Rd, +#offset]
#define STRH_AD2( cond, d, n, offset ) \
   HALFW_DCD( cond, d, n, offset, HALFW_OFF_PL, HALFW_STORE, HALFW_DEF2 )

//---- strh Rn, [Rd], -#offset
#define STRH_AD3( cond, d, n, offset ) \
   HALFW_DCD( cond, d, n, offset, HALFW_OFF_MI, HALFW_STORE, HALFW_DEF1 )

//---- strh Rn, [Rd, -#offset]
#define STRH_AD4( cond, d, n, offset ) \
   HALFW_DCD( cond, d, n, offset, HALFW_OFF_MI, HALFW_STORE, HALFW_DEF2 )



#include <nitro/code32.h>
//=======================================================================
//           MEMORY OPERATIONS
//=======================================================================
/*---------------------------------------------------------------------------*
  Name:         MIi_CpuClear16

  Description:  fill memory with specified data.
                16bit version

  Arguments:    data  : fill data
                destp : destination address
                size  : size (byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
asm void MIi_CpuClear16( register u16 data, register void* destp, register u32 size )
{
        mov     r3, #0                  // n = 0

@00:
        cmp     r3, r2                  // n < size ?
        blt @strlth1
        b @strlth2
@strlth1:
        strh  r0, [r1, r3]            // *((vu16 *)(destp + n)) = data
@strlth2:
        blt @addlt1
        b @addlt2
@addlt1:
        add   r3, r3, #2              // n += 2
@addlt2:
        blt     @00

        bx      lr
}

/*---------------------------------------------------------------------------*
  Name:         MIi_CpuCopy16

  Description:  copy memory by CPU
                16bit version

  Arguments:    srcp  : source address
                destp : destination address
                size  : size (byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
asm void MIi_CpuCopy16( register const void *srcp, register void *destp, register u32 size )
{
        mov     r12, #0                 // n = 0

@10:
        cmp     r12, r2                 // n < size ?

#ifndef CW_BUG_FOR_LDRH_AND_STRH
        blt @ldrlth1
        b @ldrlth2
@ldrlth1:
        ldrh  r3, [r0, r12]           // *((vu16 *)(destp + n)) = *((vu16 *)(srcp + n))
@ldrlth2:
#else
        dcd     0xb19030bc
#endif
#ifndef CW_BUG_FOR_LDRH_AND_STRH
        blt @strlth1
        b @strlth2
@strlth1:
        strh  r3, [r1, r12]
@strlth2:
#else
        dcd     0xb18130bc
#endif
        blt @addlt1
        b @addlt2
@addlt1:
        add   r12, r12, #2            // n += 2
@addlt2:
        blt     @10

        bx      lr
}

/*---------------------------------------------------------------------------*
  Name:         MIi_CpuSend16

  Description:  send u16 data to fixed address
                16bit version

  Arguments:    src   : data stream to send
                dest  : destination address. not incremented
                size  : size (byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
asm void MIi_CpuSend16( register const void *srcp, register volatile void* destp, register u32 size )
{
        mov     r12, #0                 // n = 0

@11:
        cmp     r12, r2                 // n < size ?
#ifndef CW_BUG_FOR_LDRH_AND_STRH
        blt @ldrlth1
        b @ldrlth2
@ldrlth1:
        ldrh  r3, [r0, r12]           // *((vu16 *)(destp + n)) = *((vu16 *)(srcp + n))
@ldrlth2:
#else
        dcd     0xb19030bc
#endif
        blt @strlth1
        b @strlth2
@strlth1:
        strh  r3, [r1, #0]
@strlth2:
        blt @addlt1
        b @addlt2
@addlt1:
        add   r12, r12, #2            // n += 2
@addlt2:
        blt     @11

        bx      lr
}


/*---------------------------------------------------------------------------*
  Name:         MIi_CpuClear32

  Description:  fill memory with specified data.
                32bit version

  Arguments:    data  : fill data
                destp : destination address
                size  : size (byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
asm void MIi_CpuClear32( register u32 data, register void *destp, register u32 size )
{
        add     r12, r1, r2             // r12: destEndp = destp + size

@20:
        cmp     r1, r12                 // while (destp < destEndp)
        blt @stmltia1
        b @stmltia2
@stmltia1:
        stmia r1!, {r0}               // *((vu32 *)(destp++)) = data
@stmltia2:
        blt     @20
        bx      lr
}

/*---------------------------------------------------------------------------*
  Name:         MIi_CpuCopy32

  Description:  copy memory by CPU
                32bit version

  Arguments:    srcp  : source address
                destp : destination address
                size  : size (byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
asm void MIi_CpuCopy32( register const void *srcp, register void *destp, register u32 size )
{
        add     r12, r1, r2             // r12: destEndp = destp + size

@30:
        cmp     r1, r12                 // while (destp < destEndp)
        blt @ldmltia1
        b @ldmltia2
@ldmltia1:
        ldmia r0!, {r2}               // *((vu32 *)(destp)++) = *((vu32 *)(srcp)++)
@ldmltia2:
        blt @stmltia1
        b @stmltia2
@stmltia1:
        stmia r1!, {r2}
@stmltia2:
        blt     @30

        bx      lr
}

/*---------------------------------------------------------------------------*
  Name:         MIi_CpuSend32

  Description:  send u32 data to fixed address
                32bit version

  Arguments:    src   : data stream to send
                dest  : destination address. not incremented
                size  : size (byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
asm void MIi_CpuSend32( register const void *srcp, volatile void *destp, u32 size )
{
        add     r12, r0, r2             // r12: srcEndp = srcp + size

@31:
        cmp     r0, r12                 // while (srcp < srcEndp)
        blt @ldmltia1
        b @ldmltia2
@ldmltia1:
        ldmia r0!, {r2}               // *((vu32 *)(destp)++) = *((vu32 *)(srcp)++)
@ldmltia2:
        blt @strlt1
        b @strlt2
@strlt1:
        str   r2, [r1]
@strlt2:
        blt     @31

        bx      lr
}

/*---------------------------------------------------------------------------*
  Name:         MIi_CpuClearFast

  Description:  fill memory with specified data.
                high speed by writing 32byte at a time using stm

  Arguments:    data  : fill data
                destp : destination address
                size  : size (byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
asm void MIi_CpuClearFast( register u32 data, register void *destp, register u32 size )
{
        stmfd   sp!, {r4-r9}

        add     r9, r1, r2              // r9:  destEndp = destp + size
        mov     r12, r2, lsr #5         // r12: destBlockEndp = destp + size/32*32
        add     r12, r1, r12, lsl #5

        mov     r2, r0
        mov     r3, r2
        mov     r4, r2
        mov     r5, r2
        mov     r6, r2
        mov     r7, r2
        mov     r8, r2

@40:
        cmp     r1, r12                 // while (destp < destBlockEndp)
        blt @stmltia1
        b @stmltia2
@stmltia1:
        stmia r1!, {r0, r2-r8}        // *((vu32 *)(destp++)) = data
@stmltia2:
        blt     @40
@41:
        cmp     r1, r9                  // while (destp < destEndp)
        blt @stmltia3
        b @stmltia4
@stmltia3:
        stmia r1!, {r0}                 // *((vu32 *)(destp++)) = data
@stmltia4:
        blt     @41

        ldmfd   sp!, {r4-r9}
        bx      lr
}

/*---------------------------------------------------------------------------*
  Name:         MIi_CpuCopyFast

  Description:  copy memory by CPU
                high speed by loading/writing 32byte at a time using stm/ldm

  Arguments:    srcp  : source address
                destp : destination address
                size  : size (byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
asm void MIi_CpuCopyFast( register const void *srcp, register void *destp, register u32 size )
{
        stmfd   sp!, {r4-r10}

        add     r10, r1, r2             // r10:  destEndp = destp + size
        mov     r12, r2, lsr #5         // r12: destBlockEndp = destp + size/32*32
        add     r12, r1, r12, lsl #5

@50:
        cmp     r1, r12                 // while (destp < destBlockEndp)
        blt @ldmltia1
        b @ldmltia2
@ldmltia1:
        ldmia r0!, {r2-r9}            // *((vu32 *)(destp)++) = *((vu32 *)(srcp)++)
@ldmltia2:
        blt @stmltia1
        b @stmltia2
@stmltia1:
        stmia r1!, {r2-r9}
@stmltia2:
        blt     @50
@51:
        cmp     r1, r10                 // while (destp < destEndp)
        blt @ldmltia3
        b @ldmltia4
@ldmltia3:
        ldmia r0!, {r2}               // *((vu32 *)(destp)++) = *((vu32 *)(srcp)++)
@ldmltia4:
        blt @stmltia3
        b @stmltia4
@stmltia3:
        stmia r1!, {r2}
@stmltia4:
        blt     @51

        ldmfd   sp!, {r4-r10}
        bx      lr
}

//=======================================================================
//           FOR CONVINIENCE (memory copy)
//=======================================================================
/*---------------------------------------------------------------------------*
  Name:         MI_Copy16B

  Description:  copy 16byte data by CPU

  Arguments:    srcp  : source address
                destp : destination address

  Returns:      None
 *---------------------------------------------------------------------------*/
asm void MI_Copy16B(register const void* pSrc, register void* pDest)
{
    ldmia   r0!, {r2, r3, r12}         // r0-r3, r12 need not saved
    stmia   r1!, {r2, r3, r12}
    ldmia   r0!, {r2}
    stmia   r1!, {r2}

    bx      lr
}

/*---------------------------------------------------------------------------*
  Name:         MI_Copy32B

  Description:  copy 32byte data by CPU

  Arguments:    srcp  : source address
                destp : destination address

  Returns:      None
 *---------------------------------------------------------------------------*/
asm void MI_Copy32B(register const void* pSrc, register void* pDest)
{
    ldmia   r0!, {r2, r3, r12}         // r0-r3, r12 need not saved
    stmia   r1!, {r2, r3, r12}
    ldmia   r0!, {r2, r3, r12}
    stmia   r1!, {r2, r3, r12}
    ldmia   r0!, {r2, r3}
    stmia   r1!, {r2, r3}

    bx      lr
}

/*---------------------------------------------------------------------------*
  Name:         MI_Copy36B

  Description:  copy 36byte data by CPU

  Arguments:    srcp  : source address
                destp : destination address

  Returns:      None
 *---------------------------------------------------------------------------*/
asm void MI_Copy36B(register const void* pSrc, register void* pDest)
{
    ldmia   r0!, {r2, r3, r12}         // r0-r3, r12 need not saved
    stmia   r1!, {r2, r3, r12}
    ldmia   r0!, {r2, r3, r12}
    stmia   r1!, {r2, r3, r12}
    ldmia   r0!, {r2, r3, r12}
    stmia   r1!, {r2, r3, r12}

    bx      lr
}

/*---------------------------------------------------------------------------*
  Name:         MI_Copy48B

  Description:  copy 48byte data by CPU

  Arguments:    srcp  : source address
                destp : destination address

  Returns:      None
 *---------------------------------------------------------------------------*/
asm void MI_Copy48B(register const void* pSrc, register void* pDest)
{
    ldmia   r0!, {r2, r3, r12}         // r0-r3, r12 need not saved
    stmia   r1!, {r2, r3, r12}
    ldmia   r0!, {r2, r3, r12}
    stmia   r1!, {r2, r3, r12}
    ldmia   r0!, {r2, r3, r12}
    stmia   r1!, {r2, r3, r12}
    ldmia   r0!, {r2, r3, r12}
    stmia   r1!, {r2, r3, r12}

    bx      lr
}

/*---------------------------------------------------------------------------*
  Name:         MI_Copy64B

  Description:  copy 64byte data by CPU

  Arguments:    srcp  : source address
                destp : destination address

  Returns:      None
 *---------------------------------------------------------------------------*/
asm void MI_Copy64B(register const void* pSrc, register void* pDest)
{
    ldmia   r0!, {r2, r3, r12}         // r0-r3, r12 need not saved
    stmia   r1!, {r2, r3, r12}
    ldmia   r0!, {r2, r3, r12}
    stmia   r1!, {r2, r3, r12}
    ldmia   r0!, {r2, r3, r12}
    stmia   r1!, {r2, r3, r12}
    ldmia   r0!, {r2, r3, r12}
    stmia   r1!, {r2, r3, r12}
    ldmia   r0,  {r0, r2, r3, r12}
    stmia   r1!, {r0, r2, r3, r12}

    bx      lr
}

/*---------------------------------------------------------------------------*
  Name:         MI_Copy128B

  Description:  copy 128byte data by CPU

  Arguments:    srcp  : source address
                destp : destination address

  Returns:      None
 *---------------------------------------------------------------------------*/
asm void MI_Copy128B(register const void* pSrc, register void* pDest)
{
    stmfd   sp!, {r4}

    ldmia   r0!, {r2, r3, r4, r12}         // r0-r3, r12 need not saved
    stmia   r1!, {r2, r3, r4, r12}
    ldmia   r0!, {r2, r3, r4, r12}
    stmia   r1!, {r2, r3, r4, r12}
    ldmia   r0!, {r2, r3, r4, r12}
    stmia   r1!, {r2, r3, r4, r12}
    ldmia   r0!, {r2, r3, r4, r12}
    stmia   r1!, {r2, r3, r4, r12}
    ldmia   r0!, {r2, r3, r4, r12}
    stmia   r1!, {r2, r3, r4, r12}
    ldmia   r0!, {r2, r3, r4, r12}
    stmia   r1!, {r2, r3, r4, r12}
    ldmia   r0!, {r2, r3, r4, r12}
    stmia   r1!, {r2, r3, r4, r12}
    ldmia   r0!, {r2, r3, r4, r12}
    stmia   r1!, {r2, r3, r4, r12}

    ldmfd   sp!, {r4}
    bx      lr
}

//=======================================================================
//           FOR SDK USE (needless set alignment)
//=======================================================================
/*---------------------------------------------------------------------------*
  Name:         MI_CpuFill8

  Description:  fill memory with specified data.
                consider for alignment automatically.

  Arguments:    dstp  : destination address
                data  : fill data
                size  : size (byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
#ifdef SDK_SMALL_BUILD
asm void MI_CpuFill8( register void *dstp, register u8 data, register u32 size )
{
        mov     r12, #0                 // n = 0
@1:
        cmp     r12, r2                 // n < size ?
        strltb  r1, [r0, r12]           // *((u8*)( dstp + n ) ) = data

        addlt   r12, r12, #1            // n ++
        blt     @1

        bx      lr
}
#else  //ifdef SDK_SMALL_BUILD
asm void MI_CpuFill8( register void *dstp, register u8 data, register u32 size )
{
    cmp     r2, #0
    beq @bxeq1
    b @bxeq2
@bxeq1:
    bx    lr
@bxeq2:

    // dstp を 16bit アライン.
    tst     r0, #1
    beq     @_1
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    ldrh    r12, [r0, #-1]
#else
    LDRH_AD4( HALFW_CONDAL, 12, 0, 1 ) // *** for CW BUG
#endif
    and     r12, r12, #0x00FF
    orr     r3, r12, r1, lsl #8
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    strh    r3, [r0, #-1]
#else
    STRH_AD4( HALFW_CONDAL, 3, 0, 1 ) // *** for CW BUG
#endif
    add     r0, r0, #1
    subs    r2, r2, #1
    beq @bxeq3
    b @bxeq4
@bxeq3:
    bx    lr
@bxeq4:
@_1:

    // 32bit アライン.
    cmp     r2, #2
    bcc     @_6
    orr     r1, r1, r1, lsl #8
    tst     r0, #2
    beq     @_8
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    strh    r1, [r0], #2
#else
    STRH_AD1( HALFW_CONDAL, 1, 0, 2 ) // *** for CW BUG
#endif
    subs    r2, r2, #2
    beq @bxeq5
    b @bxeq6
@bxeq5:
    bx    lr
@bxeq6:
@_8:
    // 32bit 転送.
    orr     r1, r1, r1, lsl #16
    bics    r3, r2, #3
    beq     @_10
    sub     r2, r2, r3
    add     r12, r3, r0
@_9:
    str     r1, [r0], #4
    cmp     r0, r12
    bcc     @_9

@_10:
    //  最後の 16bit 転送.
    tst     r2, #2
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    bne @strneh1
    b @strneh2
@strneh1:
    strh  r1, [r0], #2
@strneh2:
#else
    STRH_AD1( HALFW_CONDNE, 1, 0, 2 ) // *** for CW BUG
#endif

@_6:
    //  最後の 8bit 転送.
    tst     r2, #1
    beq @bxeq7
    b @bxeq8
@bxeq7:
    bx    lr
@bxeq8:
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    ldrh    r3, [r0]
#else
    LDRH_AD2( HALFW_CONDAL, 3, 0, 0 ) // *** for CW BUG
#endif
    and     r3, r3, #0xFF00
    and     r1, r1, #0x00FF
    orr     r1, r1, r3
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    strh    r1, [r0]
#else
    STRH_AD2( HALFW_CONDAL, 1, 0, 0 ) // *** for CW BUG
#endif
    bx      lr
}
#endif // ifdef SDK_SMALL_BUILD

/*---------------------------------------------------------------------------*
  Name:         MI_CpuCopy8

  Description:  copy memory by CPU
                consider for alignment automatically.

  Arguments:    srcp  : source address
                dstp  : destination address
                size  : size (byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
#ifdef SDK_SMALL_BUILD
asm void MI_CpuCopy8( register const void *srcp, register void *dstp, register u32 size )
{
        mov     r12, #0                 // n = 0
@1:
        cmp     r12, r2                 // n < size ?
        ldrltb  r3, [r0, r12]           // *((vu8 *)(destp + p)) = *((vu8 *)(srcp + n))
        strltb  r3, [r1, r12]

        addlt   r12, r12, #1            // n ++
        blt     @1

        bx      lr
}
#else  //ifdef SDK_SMALL_BUILD
asm void MI_CpuCopy8( register const void *srcp, register void *dstp, register u32 size )
{
    cmp     r2, #0
    beq @bxeq1
    b @bxeq2
@bxeq1:
    bx    lr
@bxeq2:

    // dstp を 16bit アライン.
    tst     r1, #1
    beq     @_1
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    ldrh    r12, [r1, #-1]
#else
    LDRH_AD4( HALFW_CONDAL, 12, 1, 1 ) // *** for CW BUG
#endif
    and     r12, r12, #0x00FF
    tst     r0, #1
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    bne @ldrneh1
    b @ldrneh2
@ldrneh1:
    ldrh  r3, [r0, #-1]
@ldrneh2:
#else
    LDRH_AD4( HALFW_CONDNE, 3, 0, 1 ) // *** for CW BUG
#endif
    bne @movne1
    b @movne2
@movne1:
    mov   r3, r3, lsr #8
@movne2:
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    beq @ldreqh1
    b @ldreqh2
@ldreqh1:
    ldrh  r3, [r0]
@ldreqh2:
#else
    LDRH_AD2( HALFW_CONDEQ, 3, 0, 0 ) // *** for CW BUG
#endif
    orr     r3, r12, r3, lsl #8
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    strh    r3, [r1, #-1]
#else
    STRH_AD4( HALFW_CONDAL, 3, 1, 1 ) // *** for CW BUG
#endif
    add     r0, r0, #1
    add     r1, r1, #1
    subs    r2, r2, #1
    beq @bxeq3
    b @bxeq4
@bxeq3:
    bx    lr
@bxeq4:
@_1:

    // アドレス端数の 16/32bit 同期をチェック.
    eor     r12, r1, r0
    tst     r12, #1
    beq     @_2

    // 全く同期しないので変則 16bit 転送.
    //  tmp = *(u16*)src++ >> 8;
    //  while((size -= 2) >= 0) {
    //      tmp |= (*(u16*)src++ << 8);
    //      *(u16*)dst++ = (u16)tmp;
    //      tmp >>= 16;
    //  }
    bic     r0, r0, #1
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    ldrh    r12, [r0], #2
#else
        LDRH_AD1( HALFW_CONDAL, 12, 0, 2 ) // *** for CW BUG
#endif
    mov     r3, r12, lsr #8
    subs    r2, r2, #2
    bcc     @_3
@_4:
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    ldrh    r12, [r0], #2
#else
    LDRH_AD1( HALFW_CONDAL, 12, 0, 2 ) // *** for CW BUG
#endif
    orr     r12, r3, r12, lsl #8
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    strh    r12, [r1], #2
#else
    STRH_AD1( HALFW_CONDAL, 12, 1, 2 ) // *** for CW BUG
#endif
    mov     r3, r12, lsr #16
    subs    r2, r2, #2
    bcs     @_4
    
@_3:
    //  if(size & 1)
    //      *dst = (u16)((*dst & 0xFF00) | tmp);
    //  return;
    tst     r2, #1
    beq @bxeq5
    b @bxeq6
@bxeq5:
    bx    lr
@bxeq6:
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    ldrh    r12, [r1]
#else
    LDRH_AD2( HALFW_CONDAL, 12, 1, 0 ) // *** for CW BUG
#endif
    and     r12, r12, #0xFF00
    orr     r12, r12, r3
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    strh    r12, [r1]
#else
    STRH_AD2( HALFW_CONDAL, 12, 1, 0 ) // *** for CW BUG
#endif
    bx      lr

@_2:
    tst     r12, #2
    beq     @_5
    // 16bit 転送.
    bics    r3, r2, #1
    beq     @_6
    sub     r2, r2, r3
    add     r12, r3, r1
@_7:
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    ldrh    r3, [r0], #2
#else
    LDRH_AD1( HALFW_CONDAL, 3, 0, 2 ) // *** for CW BUG
#endif
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    strh    r3, [r1], #2
#else
    STRH_AD1( HALFW_CONDAL, 3, 1, 2 ) // *** for CW BUG
#endif
    cmp     r1, r12
    bcc     @_7
    b       @_6

@_5:
    // 32bit アライン.
    cmp     r2, #2
    bcc     @_6
    tst     r1, #2
    beq     @_8
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    ldrh    r3, [r0], #2
#else
    LDRH_AD1( HALFW_CONDAL, 3, 0, 2 ) // *** for CW BUG
#endif
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    strh    r3, [r1], #2
#else
        STRH_AD1( HALFW_CONDAL, 3, 1, 2 ) // *** for CW BUG
#endif
    subs    r2, r2, #2
    beq @bxeq7
    b @bxeq8
@bxeq7:
    bx    lr
@bxeq8:
@_8:
    // 32bit 転送.
    bics    r3, r2, #3
    beq     @_10
    sub     r2, r2, r3
    add     r12, r3, r1
@_9:
    ldr     r3, [r0], #4
    str     r3, [r1], #4
    cmp     r1, r12
    bcc     @_9

@_10:
    //  最後の 16bit 転送.
    tst     r2, #2
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    bne @ldrneh3
    b @ldrneh4
@ldrneh3:
    ldrh  r3, [r0], #2
@ldrneh4:
    bne @strneh1
    b @strneh2
@strneh1:
    strh  r3, [r1], #2
@strneh2:
#else
    LDRH_AD1( HALFW_CONDNE, 3, 0, 2 ) // *** for CW BUG
    STRH_AD1( HALFW_CONDNE, 3, 1, 2 ) // *** for CW BUG
#endif

@_6:
    //  最後の 8bit 転送.
    tst     r2, #1
    beq @bxeq9
    b @bxeq10
@bxeq9:
    bx    lr
@bxeq10:
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    ldrh    r2, [r1]
    ldrh    r0, [r0]
#else
    LDRH_AD2( HALFW_CONDAL, 2, 1, 0 ) // *** for CW BUG
    LDRH_AD2( HALFW_CONDAL, 0, 0, 0 ) // *** for CW BUG
#endif
    and     r2, r2, #0xFF00
    and     r0, r0, #0x00FF
    orr     r0, r2, r0
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    strh    r0, [r1]
#else
    STRH_AD2( HALFW_CONDAL, 0, 1, 0 ) // *** for CW BUG
#endif
    bx      lr
}
#endif //ifdef SDK_SMALL_BUILD

#include <nitro/codereset.h>


#include <nitro/code16.h>
//=======================================================================
//           FOR CONVINIENCE (filling zero)
//=======================================================================
/*---------------------------------------------------------------------------*
  Name:         MI_Zero32B

  Description:  fill 32byte area with 0 by CPU

  Arguments:    pDest : destination address

  Returns:      None
 *---------------------------------------------------------------------------*/
asm void MI_Zero32B(register void* pDest)
{
    mov     r1,  #0
    mov     r2,  #0
    stmia   r0!, {r1, r2}
    mov     r3,  #0
    stmia   r0!, {r1, r2, r3}
    stmia   r0!, {r1, r2, r3}

    bx      lr
}

/*---------------------------------------------------------------------------*
  Name:         MI_Zero36B

  Description:  fill 36byte area with 0 by CPU

  Arguments:    pDest : destination address

  Returns:      None
 *---------------------------------------------------------------------------*/
asm void MI_Zero36B(register void* pDest)
{
    mov     r1,  #0
    mov     r2,  #0
    mov     r3,  #0
    stmia   r0!, {r1, r2, r3}
    stmia   r0!, {r1, r2, r3}
    stmia   r0!, {r1, r2, r3}

    bx      lr
}

/*---------------------------------------------------------------------------*
  Name:         MI_Zero48B

  Description:  fill 48byte area with 0 by CPU

  Arguments:    pDest : destination address

  Returns:      None
 *---------------------------------------------------------------------------*/
asm void MI_Zero48B(register void* pDest)
{
    mov     r1,  #0
    mov     r2,  #0
    mov     r3,  #0
    stmia   r0!, {r1, r2, r3}
    stmia   r0!, {r1, r2, r3}
    stmia   r0!, {r1, r2, r3}
    stmia   r0!, {r1, r2, r3}

    bx      lr
}

/*---------------------------------------------------------------------------*
  Name:         MI_Zero64B

  Description:  fill 64byte area with 0 by CPU

  Arguments:    pDest : destination address

  Returns:      None
 *---------------------------------------------------------------------------*/
asm void MI_Zero64B(register void* pDest)
{
    mov     r1,  #0
    mov     r2,  #0
    stmia   r0!, {r1, r2}
    mov     r3,  #0
    stmia   r0!, {r1, r2}
    stmia   r0!, {r1, r2, r3}
    stmia   r0!, {r1, r2, r3}
    stmia   r0!, {r1, r2, r3}
    stmia   r0!, {r1, r2, r3}

    bx      lr
}

//---- end limitation of THUMB-Mode
#include <nitro/codereset.h>

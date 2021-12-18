/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MI
  File:     mi_dma.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mi_dma.h,v $
  Revision 1.13  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.12  2005/02/28 05:26:05  yosizaki
  do-indent.

  Revision 1.11  2005/02/18 06:41:24  yasu
  OS_DisableInterrupts ï‘ílÇÃå^èCê≥

  Revision 1.10  2005/01/12 11:09:27  takano_makoto
  fix copyright header.

  Revision 1.9  2005/01/12 02:48:50  yada
  consider about multi-DMA

  Revision 1.8  2004/10/21 06:54:54  yada
  MI_DmaFill*() was not interrupt-safe function. Fix that.

  Revision 1.7  2004/10/08 11:59:39  yosizaki
  add MIi_ASSERT_SRC_ALIGN512.

  Revision 1.6  2004/08/30 11:53:14  yada
  in ARM9, move MIi_DmaSetParams to ITCM

  Revision 1.5  2004/08/30 04:29:42  yada
  move some definitions to public include folder

  Revision 1.4  2004/08/26 01:22:30  yada
  move function body of MIi_CheckAddressInTCM() to mi_dma.c

  Revision 1.3  2004/08/26 00:13:11  yada
  change multiple auto-DMA check method

  Revision 1.2  2004/08/25 11:56:18  yada
  add check for avoiding more than 2 auto_start_DMA run at the same time

  Revision 1.1  2004/08/25 00:48:41  yada
  initial release.
  separate definition part from mi_dma.c

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NITRO_COMMON_MI_DMA_H_
#define NITRO_COMMON_MI_DMA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro.h>

//================================================================================
//          DMA system work
//================================================================================
#ifdef SDK_ARM9
# ifdef SDK_TS
#  define MIi_DMA_CLEAR_DATA_BUF    REG_DMA0_CLR_DATA_ADDR      // ARM9-TS:  defined ARM9/ioreg_MI.h
# else
#  define MIi_DMA_CLEAR_DATA_BUF    HW_DMA_CLEAR_DATA_BUF       // ARM9-TEG: defined common/mmap_shared.h
# endif
#else  // ifdef SDK_ARM9
# define MIi_DMA_CLEAR_DATA_BUF     HW_PRV_WRAM_DMA_CLEAR_DATA_BUF      // ARM7:  defined ARM7/mmap_wram.h
#endif // ifdef SDK_ARM9

typedef union
{
    u32     b32;
    u16     b16;
}
MIiDmaClearSrc;

//================================================================================
//          setting DMA
//================================================================================
//---------------- set parameters
#ifdef SDK_ARM9

void    MIi_DmaSetParams(u32 dmaNo, u32 src, u32 dest, u32 ctrl);
void    MIi_DmaSetParams_wait(u32 dmaNo, u32 src, u32 dest, u32 ctrl);
void    MIi_DmaSetParams_noInt(u32 dmaNo, u32 src, u32 dest, u32 ctrl);
void    MIi_DmaSetParams_wait_noInt(u32 dmaNo, u32 src, u32 dest, u32 ctrl);

#else  // ifdef SDK_ARM9
static inline void MIi_DmaSetParams(u32 dmaNo, u32 src, u32 dest, u32 ctrl)
{
    OSIntrMode enabled = OS_DisableInterrupts();
    vu32   *p = (vu32 *)((u32)REG_DMA0SAD_ADDR + dmaNo * 12);
    *p = (vu32)src;
    *(p + 1) = (vu32)dest;
    *(p + 2) = (vu32)ctrl;
    (void)OS_RestoreInterrupts(enabled);
}

static inline void MIi_DmaSetParams_wait(u32 dmaNo, u32 src, u32 dest, u32 ctrl)
{
    OSIntrMode enabled = OS_DisableInterrupts();
    vu32   *p = (vu32 *)((u32)REG_DMA0SAD_ADDR + dmaNo * 12);
    *p = (vu32)src;
    *(p + 1) = (vu32)dest;
    *(p + 2) = (vu32)ctrl;

    // ARM7 must wait 2 cycle (load is 3 cycle)
    {
        u32     dummy = reg_MI_DMA0SAD;
    }

    (void)OS_RestoreInterrupts(enabled);
}

static inline void MIi_DmaSetParams_noInt(u32 dmaNo, u32 src, u32 dest, u32 ctrl)
{
    vu32   *p = (vu32 *)((u32)REG_DMA0SAD_ADDR + dmaNo * 12);
    *p = (vu32)src;
    *(p + 1) = (vu32)dest;
    *(p + 2) = (vu32)ctrl;
}

static inline void MIi_DmaSetParams_wait_noInt(u32 dmaNo, u32 src, u32 dest, u32 ctrl)
{
    vu32   *p = (vu32 *)((u32)REG_DMA0SAD_ADDR + dmaNo * 12);
    *p = (vu32)src;
    *(p + 1) = (vu32)dest;
    *(p + 2) = (vu32)ctrl;

    // ARM7 must wait 2 cycle (load is 3 cycle)
    {
        u32     dummy = reg_MI_DMA0SAD;
    }
}
#endif


static inline void MIi_DmaSetParams_src32(u32 dmaNo, u32 data, u32 dest, u32 ctrl)
{
    OSIntrMode enabled = OS_DisableInterrupts();

    MIiDmaClearSrc *srcp = (MIiDmaClearSrc *) ((u32)MIi_DMA_CLEAR_DATA_BUF + dmaNo * 4);
    srcp->b32 = data;
    MIi_DmaSetParams_noInt(dmaNo, (u32)srcp, dest, ctrl);

    (void)OS_RestoreInterrupts(enabled);
}

static inline void MIi_DmaSetParams_src16(u32 dmaNo, u16 data, u32 dest, u32 ctrl)
{
    OSIntrMode enabled = OS_DisableInterrupts();

    MIiDmaClearSrc *srcp = (MIiDmaClearSrc *) ((u32)MIi_DMA_CLEAR_DATA_BUF + dmaNo * 4);
    srcp->b16 = data;
    MIi_DmaSetParams_noInt(dmaNo, (u32)srcp, dest, ctrl);

    (void)OS_RestoreInterrupts(enabled);
}

static inline void MIi_DmaSetParams_wait_src32(u32 dmaNo, u32 data, u32 dest, u32 ctrl)
{
    OSIntrMode enabled = OS_DisableInterrupts();

    MIiDmaClearSrc *srcp = (MIiDmaClearSrc *) ((u32)MIi_DMA_CLEAR_DATA_BUF + dmaNo * 4);
    srcp->b32 = data;
    MIi_DmaSetParams_wait_noInt(dmaNo, (u32)srcp, dest, ctrl);

    (void)OS_RestoreInterrupts(enabled);
}

static inline void MIi_DmaSetParams_wait_src16(u32 dmaNo, u16 data, u32 dest, u32 ctrl)
{
    OSIntrMode enabled = OS_DisableInterrupts();

    MIiDmaClearSrc *srcp = (MIiDmaClearSrc *) ((u32)MIi_DMA_CLEAR_DATA_BUF + dmaNo * 4);
    srcp->b16 = data;
    MIi_DmaSetParams_wait_noInt(dmaNo, (u32)srcp, dest, ctrl);

    (void)OS_RestoreInterrupts(enabled);
}

//================================================================================
//          CALLBACK
//================================================================================
extern void OSi_EnterDmaCallback(u32 dmaNo, MIDmaCallback callback, void *arg);

static inline void MIi_CallCallback(MIDmaCallback callback, void *arg)
{
    if (callback)
    {
        (callback) (arg);
    }
}

//================================================================================
//          WAIT
//================================================================================
//----------------------------------------------------------------
//     for waiting DMA busy
#define MIi_Wait_BeforeDMA( dmaCntp, dmaNo )                  \
    do {                                                      \
      dmaCntp = &((vu32*)REG_DMA0SAD_ADDR)[dmaNo * 3 + 2];    \
      MIi_ASSERT_DMANO( dmaNo );                              \
      while ( *dmaCntp & REG_MI_DMA0CNT_E_MASK ) {}           \
    }while(0)

#define MIi_Wait_AfterDMA( dmaCntp )                          \
    do {                                                      \
      while ( *dmaCntp & REG_MI_DMA0CNT_E_MASK ) {}           \
    }while(0)


//================================================================================
//         ASSERT
//================================================================================
#define MIi_ASSERT_DMANO( dmaNo )        SDK_ASSERTMSG( (dmaNo) <= MI_DMA_MAX_NUM, "illegal DMA No." )
#define MIi_ASSERT_MUL2( size )          SDK_ASSERTMSG( ((size) & 1) == 0, "size & 1 must be 0" )
#define MIi_ASSERT_MUL4( size )          SDK_ASSERTMSG( ((size) & 3) == 0, "size & 3 must be 0" )
#define MIi_ASSERT_SRC_ALIGN512( src )   SDK_ASSERTMSG( ((u32)(src) & 511) == 0, "source address must be in 512-byte alignment" )
#define MIi_ASSERT_SRC_ALIGN4( src )     SDK_ASSERTMSG( ((u32)(src) & 3) == 0, "source address must be in 4-byte alignment" )
#define MIi_ASSERT_SRC_ALIGN2( src )     SDK_ASSERTMSG( ((u32)(src) & 1) == 0, "source address must be in 2-byte alignment" )
#define MIi_ASSERT_DEST_ALIGN4( dest )   SDK_ASSERTMSG( ((u32)(dest) & 3) == 0, "destination address must be in 4-byte alignment" )
#define MIi_ASSERT_DEST_ALIGN2( dest )   SDK_ASSERTMSG( ((u32)(dest) & 1) == 0, "destination address must be in 2-byte alignment" )

#ifdef SDK_ARM9
#  define MIi_ASSERT_SIZE( dmaNo, size ) SDK_ASSERTMSG( (size) <= 0x1fffff, "size too large" );
#else  //SDK_ARM7
#  define MIi_ASSERT_SIZE( dmaNo, size )                        \
    do{                                                         \
        if ( dmaNo <= 2 )                                       \
        {                                                       \
            SDK_ASSERTMSG( (size) <= 0x3fff, "size too large" );\
        }                                                       \
        else                                                    \
        {                                                       \
            SDK_ASSERTMSG( (size) <= 0xffff, "size too large" );\
        }                                                       \
    } while(0)
#endif


//================================================================================
//         CHECK
//================================================================================
//----------------------------------------------------------------
//      Check if specified area is in ITCM/DTCM.
//
#if defined( SDK_ARM9 ) && defined( SDK_DEBUG )
void    MIi_CheckAddressInTCM(u32 addr, u32 size);
#define MIi_WARNING_ADDRINTCM( addr, size )  MIi_CheckAddressInTCM( (u32)addr, (u32)size )
#else
#define MIi_WARNING_ADDRINTCM( addr, size )  ((void)0)
#endif


//----------------------------------------------------------------
//      for DMA check
//      (must avoid multiple auto start DMA)
//
#ifdef SDK_ARM9
void    MIi_CheckAnotherAutoDMA(u32 dmaNo, u32 dmaType);
#endif

//----------------------------------------------------------------
//      for DMA0 check
//       Source address which is in I/O register or cartridge bus
//       is not available.
void    MIi_CheckDma0SourceAddress(u32 dmaNo, u32 src, u32 size, u32 dir);

//================================================================================
//           dummy DMA for multi DMA problem
//================================================================================
#define MIi_DUMMY_DMA_NO   0
#define MIi_DUMMY_SRC      0
#define MIi_DUMMY_DEST     0
#define MIi_DUMMY_CNT      ( MI_DMA_ENABLE | MI_DMA_SRC_FIX | MI_DMA_DEST_FIX | MI_DMA_16BIT_BUS | 1 )

#ifdef __cplusplus
} /* extern "C" */
#endif

/* MI_COMMON_MI_DMA_H_ */
#endif

/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MATH - 
  File:     math/dgt.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: checksum.h,v $
  Revision 1.4  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.3  2005/04/13 11:58:43  seiki_masashi
  GetDigest -> GetHash

  Revision 1.2  2005/04/13 04:52:54  seiki_masashi
  Checksum16 をバイト境界に関係なく使用できるよう変更

  Revision 1.1  2005/04/12 11:11:43  seiki_masashi
  checksum.h の追加

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_MATH_CHECKSUM_H_
#define NITRO_MATH_CHECKSUM_H_

#include <nitro/misc.h>
#include <nitro/types.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
// Constant definition
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Type definition
//----------------------------------------------------------------------------

typedef u16 MATHChecksum16Context;
typedef MATHChecksum16Context MATHChecksum8Context;

//----------------------------------------------------------------------------
// Declaration of function
//----------------------------------------------------------------------------

/*****************************************************************************/
/* Checksum16                                                                */
/*****************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         MATH_Checksum16Init

  Description:  16bit の Checksum 値を求めるために使う
                MATHChecksum16Context 構造体を初期化する。
  
  Arguments:    context MATHChecksum16Context 構造体
                init    CRC の初期値
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void MATH_Checksum16Init(MATHChecksum16Context * context)
{
    *context = 0;
}

/*---------------------------------------------------------------------------*
  Name:         MATH_Checksum16Update

  Description:  16bit の Checksum 値を与えたデータで更新する。
  
  Arguments:    context MATHChecksum16Context 構造体
                input   入力データへのポインタ
                length  入力データ長
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void    MATH_Checksum16Update(MATHChecksum16Context * context, const void *input, u32 length);

/*---------------------------------------------------------------------------*
  Name:         MATH_Checksum16GetHash

  Description:  最終的な Checksum 値を得る。
  
  Arguments:    context MATHChecksum16Context 構造体
  
  Returns:      計算結果.
 *---------------------------------------------------------------------------*/
static inline u16 MATH_Checksum16GetHash(MATHChecksum16Context * context)
{
    return (u16)(~(*context));
}

/*---------------------------------------------------------------------------*
  Name:         MATH_CalcChecksum16

  Description:  16bit の Checksum を計算する。
  
  Arguments:    data    入力データのポインタ
                dataLength  入力データ長
  
  Returns:      Checksum 値.
 *---------------------------------------------------------------------------*/
u16     MATH_CalcChecksum16(const void *data, u32 dataLength);


/*****************************************************************************/
/* Checksum8                                                                 */
/*****************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         MATH_Checksum8Init

  Description:  8bit の Checksum 値を求めるために使う
                MATHChecksum8Context 構造体を初期化する。
  
  Arguments:    context MATHChecksum8Context 構造体
                init    CRC の初期値
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void MATH_Checksum8Init(MATHChecksum8Context * context)
{
    *context = 0;
}

/*---------------------------------------------------------------------------*
  Name:         MATH_Checksum8Update

  Description:  8bit の Checksum 値を与えたデータで更新する。
  
  Arguments:    context MATHChecksum8Context 構造体
                input   入力データへのポインタ
                length  入力データ長
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void    MATH_Checksum8Update(MATHChecksum8Context * context, const void *input, u32 length);

/*---------------------------------------------------------------------------*
  Name:         MATH_Checksum8GetHash

  Description:  最終的な Checksum 値を得る。
  
  Arguments:    context MATHChecksum8Context 構造体
  
  Returns:      計算結果.
 *---------------------------------------------------------------------------*/
static inline u8 MATH_Checksum8GetHash(MATHChecksum8Context * context)
{
    u32     sum = *context;
    sum = (sum >> 8) + (sum & 0xff);
    sum += (sum >> 8);

    return (u8)(~sum);
}

/*---------------------------------------------------------------------------*
  Name:         MATH_CalcChecksum8

  Description:  8bit の Checksum を計算する。
  
  Arguments:    data    入力データのポインタ
                dataLength  入力データ長
  
  Returns:      Checksum 値.
 *---------------------------------------------------------------------------*/
u8      MATH_CalcChecksum8(const void *data, u32 dataLength);



#ifdef __cplusplus
}/* extern "C" */
#endif

/* NITRO_MATH_CHECKSUM_H_ */
#endif

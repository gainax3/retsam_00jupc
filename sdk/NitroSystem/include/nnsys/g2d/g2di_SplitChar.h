/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d
  File:     g2di_SplitChar.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.3 $
 *---------------------------------------------------------------------------*/
#ifndef G2DI_SPLITCHAR_H_
#define G2DI_SPLITCHAR_H_

#include <nitro.h>

#ifdef __cplusplus
extern "C" {
#endif


// 文字切り出しコールバック
typedef u16 (*NNSiG2dSplitCharCallback)(const void** ppChar);


/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dGetChar*

  Description:  NNSiG2dGetCharCallback 型の文字コード切り出し関数です。
                バイトストリームから最初の文字の文字コードを取得するとともに
                ストリームポインタを次の文字に移動させます。

  Arguments:    ppChar: バイト配列へのポインタを格納しているバッファへのポインタ。
                        関数から返ると、このポインタの先のバッファには
                        次の文字の先頭へのポインタが格納されます。

  Returns:      *ppChar の最初の文字の文字コード。
 *---------------------------------------------------------------------------*/
u16 NNSi_G2dSplitCharUTF16(const void** ppChar);
u16 NNSi_G2dSplitCharUTF8(const void** ppChar);
u16 NNSi_G2dSplitCharShiftJIS(const void** ppChar);
u16 NNSi_G2dSplitChar1Byte(const void** ppChar);






#ifdef __cplusplus
}/* extern "C" */
#endif

#endif // G2DI_SPLITCHAR_H_


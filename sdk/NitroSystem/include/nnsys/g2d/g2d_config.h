/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d
  File:     g2d_config.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.11 $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_CONFIG_H_
#define NNS_G2D_CONFIG_H_

#include <nnsys/inline.h>
#define NNS_G2D_INLINE NNS_INLINE


#ifndef NNS_FROM_TOOL
#include <nitro.h>

#ifdef __cplusplus
extern "C" {
#endif



// NNS_G2D_DMA_NO は 3 から GX_GetDefaultDMA() へと変更になりました。
// #define NNS_G2D_DMA_NO 3 
#define NNS_G2D_DMA_NO      GX_GetDefaultDMA() 


#define NNS_G2D_ASSERTMSG               SDK_ASSERTMSG
#define NNS_G2D_ASSERT                  SDK_ASSERT
#define NNS_G2D_NULL_ASSERT             SDK_NULL_ASSERT
#define NNS_G2D_MINMAX_ASSERT           SDK_MINMAX_ASSERT
#define NNS_G2D_MIN_ASSERT              SDK_MIN_ASSERT
#define NNS_G2D_MAX_ASSERT              SDK_MAX_ASSERT
#define NNS_G2D_WARNING                 SDK_WARNING
#define NNS_G2D_NON_ZERO_ASSERT( val )  SDK_ASSERTMSG( (val) != 0, "Non zero value is expexted in the context." )


// 過去のバージョンのセルサイズ制限を有効にする
//#define NNS_G2D_LIMIT_CELL_X_128

// 旧バージョンのフォントリソースを使用する
#define NNS_G2D_FONT_USE_OLD_RESOURCE

// 縦書き/縦持ちサポートを有効にする
#define NNS_G2D_FONT_ENABLE_DIRECTION_SUPPORT

// 入力データに倍角アフィンOBJが含まれる場合、
// 倍角アフィンOBJの位置補正が行われていると仮定するか？
#define NNS_G2D_ASSUME_DOUBLEAFFINE_OBJPOS_ADJUSTED


#ifdef __cplusplus
} /* extern "C" */
#endif
#endif  // ifndef NNS_FROM_TOOL

#endif // NNS_G2D_CONFIG_H_

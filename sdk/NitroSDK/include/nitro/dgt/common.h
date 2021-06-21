/*---------------------------------------------------------------------------*
  Project:  NitroSDK - DGT - include
  File:     common.h

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: common.h,v $
  Revision 1.9  2005/07/14 02:32:02  seiki_masashi
  コンパイルスイッチの整理

  Revision 1.8  2005/07/08 12:33:50  seiki_masashi
  コンパイルスイッチの追加

  Revision 1.7  2005/03/29 03:59:26  seiki_masashi
  Copyright の修正

  Revision 1.6  2005/03/29 00:08:58  seiki_masashi
  small fix

  Revision 1.5  2005/03/28 04:18:34  seiki_masashi
  common.h の一部を dgt.h に移動

  Revision 1.4  2005/03/28 03:54:00  seiki_masashi
  math/dgt.h からの参照に伴う修正

  Revision 1.3  2004/11/05 13:27:04  yasu
  Support OverlayTable mode

  Revision 1.2  2004/09/06 05:35:17  yasu
  Add some defines

  Revision 1.1  2004/09/02 12:40:32  yasu
  Digest library

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_DGT_COMMON_H_
#define NITRO_DGT_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

//#define SDK_DGT_HASH1_CODE_SAFE
//#define SDK_DGT_HASH2_CODE_SAFE

// ダイジェスト長
#define DGT_HASH1_DIGEST_SIZE   (128/8)
#define DGT_HASH2_DIGEST_SIZE   (160/8)

// 最大のダイジェスト長
#define DGT_HASH_DIGEST_SIZE_MAX  DGT_HASH2_DIGEST_SIZE

// 処理ブロック長
#define	DGT_HASH_BLOCK_SIZE	(512/8)

#ifdef __cplusplus
}
#endif
#endif

/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - gfd - src - VramManager
  File:     gfd_TexVramMan.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.5 $
 *---------------------------------------------------------------------------*/

#include <nnsys/gfd/VramManager/gfd_VramMan.h>
#include <nnsys/gfd/VramManager/gfd_TexVramMan_Types.h>

static NNSGfdTexKey	AllocTexVram_( u32 szByte, BOOL is4x4comp, u32 opt );
static int			FreeTexVram_( NNSGfdTexKey memKey );


NNSGfdFuncAllocTexVram  NNS_GfdDefaultFuncAllocTexVram = AllocTexVram_;
NNSGfdFuncFreeTexVram   NNS_GfdDefaultFuncFreeTexVram  = FreeTexVram_;

/*---------------------------------------------------------------------------*
  Name:         AllocTexVram_

  Description:  メモリ確保のダミー関数。デフォルトのメモリ確保関数が登録され
  				ていない時、この関数が呼ばれます。
                 
  Arguments:   szByte           :  サイズ
               is4x4comp        :  4x4圧縮テクスチャか？
               opt              :  実装依存パラメータ
                            
  Returns:     テクスチャキー
 *---------------------------------------------------------------------------*/
static NNSGfdTexKey
AllocTexVram_( u32 /* szByte */, BOOL /* is4x4comp */, u32 /* opt */ )
{
    NNS_GFD_WARNING("no default AllocTexVram function.");

    // エラー：エラーを表現するTexKeyをかえす
    return NNS_GFD_ALLOC_ERROR_TEXKEY;
}

/*---------------------------------------------------------------------------*
  Name:         FreeFrmTexVram_

  Description:  メモリ開放のダミー関数。デフォルトのメモリ解放関数が登録され
  				ていない時、この関数が呼ばれます。
                 
  Arguments:   memKey          :   テクスチャキー
                            
  Returns:     成否（0 ならば 成功)
 *---------------------------------------------------------------------------*/
static int
FreeTexVram_( NNSGfdTexKey /* memKey */ )
{
    NNS_GFD_WARNING("no default FreeTexVram function.");

    return -1;
}

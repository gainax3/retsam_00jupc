/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - gfd - src - VramManager
  File:     gfd_PlttVramMan.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.5 $
 *---------------------------------------------------------------------------*/

#include <nnsys/gfd/VramManager/gfd_VramMan.h>
#include <nnsys/gfd/VramManager/gfd_PlttVramMan_Types.h>

static NNSGfdPlttKey	AllocPlttVram_( u32 szByte, BOOL b4Pltt, u32 bAllocFromLo );
static int				FreePlttVram_( NNSGfdPlttKey key );


NNSGfdFuncAllocPlttVram NNS_GfdDefaultFuncAllocPlttVram = AllocPlttVram_;
NNSGfdFuncFreePlttVram  NNS_GfdDefaultFuncFreePlttVram  = FreePlttVram_;

/*---------------------------------------------------------------------------*
  Name:         AllocPlttVram_

  Description:  メモリ確保のダミー関数。デフォルトのメモリ確保関数が登録され
  				ていない時、この関数が呼ばれます。
                 
  Arguments:   szByte           :  サイズ
               bPltt4           :  4色テクスチャか？
               bAllocFromHead   :  領域先頭（下位）から確保するか？
                            
  Returns:     テクスチャパレットキー
 *---------------------------------------------------------------------------*/
static NNSGfdPlttKey
AllocPlttVram_( u32 /* szByte */, BOOL /* b4Pltt */, u32 /* bAllocFromLo */ )
{
    NNS_GFD_WARNING("no default AllocPlttVram function.");

    // エラー：エラーを表現するPlttKeyをかえす
    return NNS_GFD_ALLOC_ERROR_PLTTKEY;
}

/*---------------------------------------------------------------------------*
  Name:         FreePlttTexVram_

  Description:  メモリ開放のダミー関数。デフォルトのメモリ解放関数が登録され
  				ていない時、この関数が呼ばれます。
                 
  Arguments:   plttKey          :   テクスチャパレットキー
                            
  Returns:     成否（0 ならば 成功)
 *---------------------------------------------------------------------------*/
static int
FreePlttVram_( NNSGfdPlttKey /* plttKey */ )
{
    NNS_GFD_WARNING("no default FreePlttVram function.");

    return -1;
}

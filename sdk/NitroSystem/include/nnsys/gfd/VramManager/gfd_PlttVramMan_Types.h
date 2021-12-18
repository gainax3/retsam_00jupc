/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - gfd - VramManager
  File:     gfd_PlttVramMan_Types.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.15 $
 *---------------------------------------------------------------------------*/
#ifndef NNS_GFD_PLTTVRAMMAN_TYPES_H_
#define NNS_GFD_PLTTVRAMMAN_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro.h>
#include <nnsys/gfd/gfd_common.h>

#define NNS_GFD_PLTTMASK_16BIT              0xFFFF


#define NNS_GFD_PLTTKEY_SIZE_SHIFT 3
#define NNS_GFD_PLTTKEY_ADDR_SHIFT 3

#define NNS_GFD_PLTTSIZE_MIN ( 0x1 << NNS_GFD_PLTTKEY_SIZE_SHIFT )
#define NNS_GFD_PLTTSIZE_MAX ( NNS_GFD_PLTTMASK_16BIT << NNS_GFD_PLTTKEY_SIZE_SHIFT )


#define NNS_GFD_ALLOC_ERROR_PLTTKEY         (u32)0x0
#define NNS_GFD_4PLTT_MAX_ADDR              0x10000



//------------------------------------------------------------------------------
//
// NNSGfdPlttKey:
// 32bitの値で、8バイト単位でテクスチャパレットスロットの領域を指定できる。
// 0-0xffffの値はエラー値として利用することができる（サイズが0なため）
//
// 31                    16  15                         0
// 3bit右シフトされたサイズ  3bit右シフトされたオフセット
//------------------------------------------------------------------------------
typedef u32 NNSGfdPlttKey;


//------------------------------------------------------------------------------
// NNSGfdFuncAllocPlttVram
// szByte:  確保するサイズをバイト単位で指定
// is4pltt: 4色パレットの格納が可能でなければいけないかどうか
// opt:     実装依存の引数(ex 前から確保するとか後ろから確保するとか)
//------------------------------------------------------------------------------
typedef NNSGfdPlttKey (*NNSGfdFuncAllocPlttVram)(u32 szByte, BOOL is4pltt, u32 opt);




//------------------------------------------------------------------------------
// NNSGfdFuncFreePlttVram
// キーを指定してテクスチャイメージスロット領域の解放を行う。
// is4plttの指定をしなくても曖昧にならないような実装である必要がある。
// 返り値は0で正常リターン。その他は実装依存のエラー。
//------------------------------------------------------------------------------
typedef int (*NNSGfdFuncFreePlttVram)(NNSGfdPlttKey plttKey);


//------------------------------------------------------------------------------
//
// ユーザーによって変更されることもある。
//
//------------------------------------------------------------------------------
extern NNSGfdFuncAllocPlttVram  NNS_GfdDefaultFuncAllocPlttVram; 
extern NNSGfdFuncFreePlttVram   NNS_GfdDefaultFuncFreePlttVram;  


//------------------------------------------------------------------------------
//
// ライブラリコードはこの関数経由でのみアクセスすることになります。
//
//------------------------------------------------------------------------------
NNS_GFD_INLINE NNSGfdPlttKey
NNS_GfdAllocPlttVram(u32 szByte, BOOL is4pltt, u32 opt)
{
    return (*NNS_GfdDefaultFuncAllocPlttVram)( szByte, is4pltt, opt );
}

NNS_GFD_INLINE int
NNS_GfdFreePlttVram(NNSGfdPlttKey plttKey)
{
    return (*NNS_GfdDefaultFuncFreePlttVram)(plttKey);
}


//------------------------------------------------------------------------------
//
// NNSGfdPlttKey 操作 関連
//
//
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// NNSGfdPlttKey が 表現可能なように、切り上げたサイズを取得します。
NNS_GFD_INLINE u32
NNSi_GfdGetPlttKeyRoundupSize( u32 size )
{
    if( size == 0 )
    {
        return NNS_GFD_PLTTSIZE_MIN;
    }else{
        return (((u32)(size) + ( NNS_GFD_PLTTSIZE_MIN-1 )) & ~( NNS_GFD_PLTTSIZE_MIN-1 ) );
    }
}

//------------------------------------------------------------------------------
NNS_GFD_INLINE NNSGfdPlttKey
NNS_GfdMakePlttKey( u32 addr, u32 size )
{
    // 切捨て誤差が発生していないか？
    SDK_ASSERT( (addr & (u32)((0x1 << NNS_GFD_PLTTKEY_ADDR_SHIFT) - 1 )) == 0 );
    SDK_ASSERT( (size & (u32)((0x1 << NNS_GFD_PLTTKEY_SIZE_SHIFT) - 1 )) == 0 );
    
    // オーバーフローは発生していないか？
    SDK_ASSERT( ( (size >> NNS_GFD_PLTTKEY_SIZE_SHIFT) & ~NNS_GFD_PLTTMASK_16BIT ) == 0 );
    SDK_ASSERT( ( (addr >> NNS_GFD_PLTTKEY_ADDR_SHIFT) & ~NNS_GFD_PLTTMASK_16BIT ) == 0 );
    
    return    ( ( size >> NNS_GFD_PLTTKEY_SIZE_SHIFT ) << 16 ) 
            | ( 0xFFFF & ( addr >> NNS_GFD_PLTTKEY_ADDR_SHIFT ) );
}

//------------------------------------------------------------------------------
NNS_GFD_INLINE u32
NNS_GfdGetPlttKeyAddr( NNSGfdPlttKey plttKey )
{
    return (u32)( ( 0x0000FFFF & plttKey )  << NNS_GFD_PLTTKEY_ADDR_SHIFT );
}

//------------------------------------------------------------------------------
NNS_GFD_INLINE u32
NNS_GfdGetPlttKeySize( NNSGfdPlttKey plttKey )
{
    return (u32)( ( ( 0xFFFF0000 & plttKey ) >> 16 ) << NNS_GFD_PLTTKEY_SIZE_SHIFT );
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_GFD_PLTTVRAMMAN_TYPES_H_

/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - gfd - VramManager
  File:     gfd_TexVramMan_Types.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.12 $
 *---------------------------------------------------------------------------*/
#ifndef NNS_GFD_TEXVRAMMAN_TYPES_H_
#define NNS_GFD_TEXVRAMMAN_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro.h>
#include <nnsys/gfd/gfd_common.h>

#define NNS_GFD_MASK_15BIT 0x7FFF
#define NNS_GFD_MASK_16BIT 0xFFFF

#define NNS_GFD_TEXKEY_SIZE_SHIFT 4
#define NNS_GFD_TEXKEY_ADDR_SHIFT 3


#define NNS_GFD_TEXSIZE_MIN ( 0x1 << NNS_GFD_TEXKEY_SIZE_SHIFT )
#define NNS_GFD_TEXSIZE_MAX ( 0x7FFF << NNS_GFD_TEXKEY_SIZE_SHIFT )




// アローケートに失敗した不正なテクスチャキー
#define NNS_GFD_ALLOC_ERROR_TEXKEY          (u32)0x0


//------------------------------------------------------------------------------
//
// NNSGfdTexKey:
// 32bitの値で、8バイト単位でテクスチャイメージスロットの領域を指定できる。
// 0-0xffffの値はエラー値として利用することができる（サイズが0なため）
//
// 31      30                    17  16                         0    
// 4x4Comp 4bit右シフトされたサイズ  3bit右シフトされたオフセット    
//
//------------------------------------------------------------------------------
typedef u32 NNSGfdTexKey;

//------------------------------------------------------------------------------
// NNSGfdFuncAllocTexVram
// szByte:    確保するサイズをバイト単位で指定
// is4x4comp: 4x4compフォーマット用かどうか
// opt:       実装依存の引数(ex 前から確保するとか後ろから確保するとか)
//
// is4x4compがTRUEの場合、返り値はテクスチャイメージの領域になる。
// テクスチャパレットインデックスの領域も別途確保されていなければならない。
//------------------------------------------------------------------------------
typedef NNSGfdTexKey (*NNSGfdFuncAllocTexVram)(u32 szByte, BOOL is4x4comp, u32 opt);




//------------------------------------------------------------------------------
// NNSGfdFuncFreeTexVram
// キーを指定してテクスチャイメージスロット領域の解放を行う。
// is4x4compの指定をしなくても曖昧にならないような実装である必要がある。
// 返り値は0で正常リターン。その他は実装依存のエラー。
//------------------------------------------------------------------------------
typedef int (*NNSGfdFuncFreeTexVram)(NNSGfdTexKey key);


//------------------------------------------------------------------------------
//
// ユーザーによって変更されることもある。
//
//------------------------------------------------------------------------------
extern NNSGfdFuncAllocTexVram   NNS_GfdDefaultFuncAllocTexVram; 
extern NNSGfdFuncFreeTexVram    NNS_GfdDefaultFuncFreeTexVram;  


//------------------------------------------------------------------------------
//
// マクロみたいなもの
// ライブラリコードはこの関数経由でのみアクセスすることになる。
//
//------------------------------------------------------------------------------
NNS_GFD_INLINE NNSGfdTexKey
NNS_GfdAllocTexVram(u32 szByte, BOOL is4x4comp, u32 opt)
{
    return (*NNS_GfdDefaultFuncAllocTexVram)(szByte, is4x4comp, opt );
}

//------------------------------------------------------------------------------
//
// マクロみたいなもの
// ライブラリコードはこの関数経由でのみアクセスすることになる。
//
//------------------------------------------------------------------------------
NNS_GFD_INLINE int
NNS_GfdFreeTexVram(NNSGfdTexKey memKey)
{
    return (*NNS_GfdDefaultFuncFreeTexVram)(memKey);
}



//------------------------------------------------------------------------------
//
// NNSGfdTexKey 操作 関連
//
//
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// NNSGfdTexKey が 表現可能なように、切り上げたサイズを取得します。
NNS_GFD_INLINE u32
NNSi_GfdGetTexKeyRoundupSize( u32 size )
{
    if( size == 0 )
    {
        return NNS_GFD_TEXSIZE_MIN;
    }else{
        
        return (((u32)(size) + ( NNS_GFD_TEXSIZE_MIN-1 )) & ~( NNS_GFD_TEXSIZE_MIN-1 ) );
    }
}    

//------------------------------------------------------------------------------
NNS_GFD_INLINE NNSGfdTexKey
NNS_GfdMakeTexKey( u32 addr, u32 size, BOOL b4x4Comp )
{
    // 切捨て誤差が発生していないか？
    SDK_ASSERT( (addr & (u32)((0x1 << NNS_GFD_TEXKEY_ADDR_SHIFT) - 1 )) == 0 );
    SDK_ASSERT( (size & (u32)((0x1 << NNS_GFD_TEXKEY_SIZE_SHIFT) - 1 )) == 0 );
    
    // オーバーフローは発生していないか？
    SDK_ASSERT( ( (size >> NNS_GFD_TEXKEY_SIZE_SHIFT) & ~NNS_GFD_MASK_15BIT ) == 0 );
    SDK_ASSERT( ( (addr >> NNS_GFD_TEXKEY_ADDR_SHIFT) & ~NNS_GFD_MASK_16BIT ) == 0 );
    
    return  ( ( size >> NNS_GFD_TEXKEY_SIZE_SHIFT ) << 16 ) 
            | ( ( NNS_GFD_MASK_16BIT & ( addr >> NNS_GFD_TEXKEY_ADDR_SHIFT ) ) ) 
            | b4x4Comp << 31;
}

//------------------------------------------------------------------------------
NNS_GFD_INLINE u32
NNS_GfdGetTexKeyAddr( NNSGfdTexKey memKey )
{
    return (u32)( ( ( 0x0000FFFF & memKey ) ) << NNS_GFD_TEXKEY_ADDR_SHIFT );
}

//------------------------------------------------------------------------------
NNS_GFD_INLINE u32
NNS_GfdGetTexKeySize( NNSGfdTexKey memKey )
{
    return (u32)( ( ( 0x7FFF0000 & memKey ) >> 16 ) << NNS_GFD_TEXKEY_SIZE_SHIFT );
}

//------------------------------------------------------------------------------
NNS_GFD_INLINE BOOL
NNS_GfdGetTexKey4x4Flag( NNSGfdTexKey memKey )
{
    return (BOOL)( (0x80000000 & memKey) >> 31 );
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_GFD_TEXVRAMMAN_TYPES_H_

/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MI - include
  File:     endian.h

  Copyright 2006,2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: endian.h,v $
  Revision 1.5  2007/02/20 00:28:11  kitase_hirotake
  indent source

  Revision 1.4  2007/01/19 04:36:00  yosizaki
  copyrightを更新.

  Revision 1.3  2007/01/16 05:56:24  yosizaki
  small fix

  Revision 1.2  2006/06/13 00:18:02  yosizaki
  add 8bit version.

  Revision 1.1  2006/04/05 07:45:21  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_MI_ENDIAN_H_
#define NITRO_MI_ENDIAN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/misc.h>
#include <nitro/types.h>
#include <nitro/platform.h>


/*****************************************************************************/
/* function */

/*---------------------------------------------------------------------------*
  Name:         MI_SwapEndian8

  Description:  8bit 値のエンディアンを変換するマクロ.
                実際には常に何の変換も行わない.

  Arguments:    val               エンディアンを変換する値.

  Returns:      エンディアンを変換した値.
 *---------------------------------------------------------------------------*/
#define MI_SwapEndian8(val) (u8)(val)

/*---------------------------------------------------------------------------*
  Name:         MI_SwapEndian16

  Description:  16bit 値のエンディアンを変換するマクロ.
                定数に対しても使用可能だが引数が複数回評価される点に注意.

  Arguments:    val               エンディアンを変換する値.

  Returns:      エンディアンを変換した値.
 *---------------------------------------------------------------------------*/
#define MI_SwapEndian16(val) (u16)( \
                              (((val) & 0xFF00UL) >> 8UL) | \
                              (((val) & 0x00FFUL) << 8UL))

/*---------------------------------------------------------------------------*
  Name:         MI_SwapEndian32

  Description:  32bit 値のエンディアンを変換するマクロ.
                定数に対しても使用可能だが引数が複数回評価される点に注意.

  Arguments:    val               エンディアンを変換する値.

  Returns:      エンディアンを変換した値.
 *---------------------------------------------------------------------------*/
#define MI_SwapEndian32(val) (u32)( \
                              (((val) & 0xFF000000UL) >> 24UL) | \
                              (((val) & 0x00FF0000UL) >> 8UL) | \
                              (((val) & 0x0000FF00UL) << 8UL) | \
                              (((val) & 0x000000FFUL) << 24UL))

#if (PLATFORM_BYTES_ENDIAN == PLATFORM_ENDIAN_LITTLE)
#define MIi_SwapEndian8IfLE(val)  MI_SwapEndian8(val)
#define MIi_SwapEndian16IfLE(val) MI_SwapEndian16(val)
#define MIi_SwapEndian32IfLE(val) MI_SwapEndian32(val)
#define MIi_SwapEndian8IfBE(val)  (val)
#define MIi_SwapEndian16IfBE(val) (val)
#define MIi_SwapEndian32IfBE(val) (val)
#else
#define MIi_SwapEndian8IfLE(val)  (val)
#define MIi_SwapEndian16IfLE(val) (val)
#define MIi_SwapEndian32IfLE(val) (val)
#define MIi_SwapEndian8IfBE(val)  MI_SwapEndian8(val)
#define MIi_SwapEndian16IfBE(val) MI_SwapEndian16(val)
#define MIi_SwapEndian32IfBE(val) MI_SwapEndian32(val)
#endif

/*---------------------------------------------------------------------------*
  Name:         MI_HTo*

  Description:  現在の環境から指定のエンディアンへ値変換.
                ニンテンドーDSではリトルエンディアン環境のため,
                *HToLE* の場合には何もしないマクロとして変換される.
                定数に対しても使用可能だが引数が複数回評価される点に注意.

  Arguments:    val               エンディアンを変換する値.

  Returns:      指定のエンディアンに変換した値.
 *---------------------------------------------------------------------------*/
#define MI_HToLE8(val)  MIi_SwapEndian8IfBE(val)
#define MI_HToBE8(val)  MIi_SwapEndian8IfLE(val)
#define MI_HToLE16(val) MIi_SwapEndian16IfBE(val)
#define MI_HToBE16(val) MIi_SwapEndian16IfLE(val)
#define MI_HToLE32(val) MIi_SwapEndian32IfBE(val)
#define MI_HToBE32(val) MIi_SwapEndian32IfLE(val)

/*---------------------------------------------------------------------------*
  Name:         MI_*ToH*

  Description:  指定のエンディアンから現在の環境へ値変換.
                ニンテンドーDSではリトルエンディアン環境のため,
                *LEToH* の場合には何もしないマクロとして変換される.
                定数に対しても使用可能だが引数が複数回評価される点に注意.

  Arguments:    val               エンディアンを変換する値.

  Returns:      指定のエンディアンから変換した値.
 *---------------------------------------------------------------------------*/
#define MI_LEToH8(val)  MIi_SwapEndian8IfBE(val)
#define MI_BEToH8(val)  MIi_SwapEndian8IfLE(val)
#define MI_LEToH16(val) MIi_SwapEndian16IfBE(val)
#define MI_BEToH16(val) MIi_SwapEndian16IfLE(val)
#define MI_LEToH32(val) MIi_SwapEndian32IfBE(val)
#define MI_BEToH32(val) MIi_SwapEndian32IfLE(val)

/*---------------------------------------------------------------------------*
  Name:         MI_LoadLE*

  Description:  指定アドレスからリトルエンディアンで値を取得.
                この関数の内容は現在の環境のエンディアンに依存しない.

  Arguments:    ptr               リトルエンディアンで読み込むデータアドレス.
                                  アラインメントを考慮する必要はない.

  Returns:      リトルエンディアンで得られた指定ビット幅の値.
 *---------------------------------------------------------------------------*/
SDK_INLINE u8 MI_LoadLE8(const void *ptr)
{
    const u8 *src = (const u8 *)ptr;
    return (u8)((src[0] << 0UL));
}
SDK_INLINE u16 MI_LoadLE16(const void *ptr)
{
    const u8 *src = (const u8 *)ptr;
    return (u16)((src[0] << 0UL) | (src[1] << 8UL));
}
SDK_INLINE u32 MI_LoadLE32(const void *ptr)
{
    const u8 *src = (const u8 *)ptr;
    return (u32)((src[0] << 0UL) | (src[1] << 8UL) | (src[2] << 16UL) | (src[3] << 24UL));
}

/*---------------------------------------------------------------------------*
  Name:         MI_LoadBE*

  Description:  指定アドレスからビッグエンディアンで値を取得.
                この関数の内容は現在の環境のエンディアンに依存しない.

  Arguments:    ptr              ビッグエンディアンで読み込むデータアドレス.
                                 アラインメントを考慮する必要はない.

  Returns:      ビッグエンディアンで得られた指定ビット幅の値.
 *---------------------------------------------------------------------------*/
SDK_INLINE u8 MI_LoadBE8(const void *ptr)
{
    const u8 *src = (const u8 *)ptr;
    return (u8)((src[0] << 0UL));
}
SDK_INLINE u16 MI_LoadBE16(const void *ptr)
{
    const u8 *src = (const u8 *)ptr;
    return (u16)((src[0] << 8UL) | (src[1] << 0UL));
}
SDK_INLINE u32 MI_LoadBE32(const void *ptr)
{
    const u8 *src = (const u8 *)ptr;
    return (u32)((src[0] << 24UL) | (src[1] << 16UL) | (src[2] << 8UL) | (src[3] << 0UL));
}

/*---------------------------------------------------------------------------*
  Name:         MI_StoreLE*

  Description:  指定アドレスへリトルエンディアンで値を格納.
                この関数の内容は現在の環境のエンディアンに依存しない.

  Arguments:    ptr               リトルエンディアンで書き込むデータアドレス.
                                  アラインメントを考慮する必要はない.
                val               書き込む値.

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void MI_StoreLE8(void *ptr, u8 val)
{
    u8     *src = (u8 *)ptr;
    src[0] = (u8)(val >> 0UL);
}
SDK_INLINE void MI_StoreLE16(void *ptr, u16 val)
{
    u8     *src = (u8 *)ptr;
    src[0] = (u8)(val >> 0UL);
    src[1] = (u8)(val >> 8UL);
}
SDK_INLINE void MI_StoreLE32(void *ptr, u32 val)
{
    u8     *src = (u8 *)ptr;
    src[0] = (u8)(val >> 0UL);
    src[1] = (u8)(val >> 8UL);
    src[2] = (u8)(val >> 16UL);
    src[3] = (u8)(val >> 24UL);
}

/*---------------------------------------------------------------------------*
  Name:         MI_StoreBE*

  Description:  指定アドレスへビッグエンディアンで値を格納.
                この関数の内容は現在の環境のエンディアンに依存しない.

  Arguments:    ptr               ビッグエンディアンで書き込むデータアドレス.
                                  アラインメントを考慮する必要はない.
                val               書き込む値.

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void MI_StoreBE8(void *ptr, u8 val)
{
    u8     *src = (u8 *)ptr;
    src[0] = (u8)(val >> 0UL);
}
SDK_INLINE void MI_StoreBE16(void *ptr, u16 val)
{
    u8     *src = (u8 *)ptr;
    src[0] = (u8)(val >> 8UL);
    src[1] = (u8)(val >> 0UL);
}
SDK_INLINE void MI_StoreBE32(void *ptr, u32 val)
{
    u8     *src = (u8 *)ptr;
    src[0] = (u8)(val >> 24UL);
    src[1] = (u8)(val >> 16UL);
    src[2] = (u8)(val >> 8UL);
    src[3] = (u8)(val >> 0UL);
}


#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_MI_ENDIAN_H_ */
#endif

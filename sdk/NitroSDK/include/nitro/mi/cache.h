/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MI - include
  File:     cache.h

  Copyright 2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/
#ifndef	NITRO_MI_CACHE_H_
#define	NITRO_MI_CACHE_H_


#include <nitro/misc.h>
#include <nitro/types.h>
#include <nitro/platform.h>
#include <nitro/mi/device.h>


#ifdef __cplusplus
extern  "C"
{
#endif


/*---------------------------------------------------------------------------*/
/* declarations */

/* メモリキャッシュを構成する個々のページ情報 */
typedef struct MICachePage
{
    struct MICachePage *next;   /* 次の要素 (リストのために使用) */
    u32                 offset; /* ページ先頭位置 (ページサイズ単位) */
    u8                 *buffer; /* ページバッファ */
}
MICachePage;

/* メモリキャッシュ情報構造体 */
typedef struct MICache
{
    u32             pagewidth;
    MICachePage    *valid;
    MICachePage    *invalid;
    MICachePage    *loading;
    int             valid_total;
    int             invalid_total;
    int             loading_total;
}
MICache;

/* 指定したページサイズと枚数を準備するのに必要なバッファサイズの算出マクロ */
#define MI_CACHE_BUFFER_WORKSIZE(page, total) \
        ((sizeof(MICachePage) + (page)) * (total))


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         MI_InitCache

  Description:  メモリキャッシュを初期化.

  Arguments:    cache            初期化するMICache構造体.
                page             1ページあたりのバッファサイズ.
                                 4以上で2のべき乗である必要がある.
                buffer           ページ管理情報に使用するバッファ.
                length           bufferのサイズ.
                                 length / (sizeof(MICachePage) + page)
                                 の数だけのページリストに分割される.
                                 各ページ(N=0,1,...)のバッファ先頭アドレスは
                                 (buffer + N * page) となることが保証される.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    MI_InitCache(MICache *cache, u32 page, void *buffer, u32 length);

/*---------------------------------------------------------------------------*
  Name:         MI_ReadCache

  Description:  キャッシュからデータを読み出し.
                ヒットしたページは有効リストの先頭に移動する.
                ヒットしなかったページはロード要求リストに追加される.

  Arguments:    cache            MICache構造体.
                buffer           転送先メモリ.
                                 NULLを指定した場合は, データを読み出さず
                                 単に該当範囲全体のキャッシュ準備のみ要求する.
                offset           転送元オフセット.
                length           転送サイズ.

  Returns:      全領域がキャッシュにヒットすればTRUE.
 *---------------------------------------------------------------------------*/
BOOL    MI_ReadCache(MICache *cache, void *buffer, u32 offset, u32 length);

/*---------------------------------------------------------------------------*
  Name:         MI_LoadCache

  Description:  ロード要求リストに存在する全ページのロード処理を実行.
                ロード要求リストが空であった場合は何もせずただちに制御を返し,
                呼び出し中にロード要求リストへ追加された場合はそれも処理する.

  Note:         この関数はデバイスがブロッキングしてもよいコンテキストから
                適切なタイミングで呼び出す必要がある.
                すなわち、割り込みハンドラなどから呼び出してはならない.

  Arguments:    cache            MICache構造体.
                device           ロード対象となるデバイス.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    MI_LoadCache(MICache *cache, MIDevice *device);

/*---------------------------------------------------------------------------*
  Name:         MI_IsCacheLoading

  Description:  ロード要求リストが空でないか判定.
                ロード要求リストはMI_ReadCache関数の呼び出しで追加され
                MI_LoadCache関数の呼び出しで空になる.

  Arguments:    cache            MICache構造体.

  Returns:      ロード要求リストが空でなければTRUE.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
BOOL    MI_IsCacheLoading(const MICache *cache)
{
    return (cache->loading != NULL);
}


/*---------------------------------------------------------------------------*/


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* NITRO_MI_CACHE_H_ */
/*---------------------------------------------------------------------------*
  $Log: cache.h,v $
  Revision 1.1  2007/04/11 08:04:56  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

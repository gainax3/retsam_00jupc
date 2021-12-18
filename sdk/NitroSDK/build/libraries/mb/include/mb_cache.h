/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - libraries
  File:     mb_cache.h

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_cache.h,v $
  Revision 1.8  2007/07/31 00:58:42  yosizaki
  update copyright

  Revision 1.7  2007/07/30 03:32:38  yosizaki
  fix about MB_RegisterFile

  Revision 1.6  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.5  2005/12/21 14:25:56  yosizaki
  fix about page-cache.

  Revision 1.4  2005/02/28 05:26:25  yosizaki
  do-indent.

  Revision 1.3  2004/12/07 10:20:30  yosizaki
  fix to reload cache from correct archive. (not only "rom")

  Revision 1.2  2004/11/11 11:53:32  yosizaki
  change argument of MBi_ReadFromCache. (const_cast)
  add some members in MBiCacheList.

  Revision 1.1  2004/11/10 13:13:42  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#if	!defined(NITRO_MB_CACHE_H_)
#define NITRO_MB_CACHE_H_


#if	defined(__cplusplus)
extern  "C"
{
#endif


/* constant ---------------------------------------------------------------- */


#define	MB_CACHE_INFO_MAX	4

#define MB_CACHE_STATE_EMPTY   0
#define MB_CACHE_STATE_BUSY    1
#define MB_CACHE_STATE_READY   2
#define MB_CACHE_STATE_LOCKED  3


/* struct ------------------------------------------------------------------ */


/*
 * ページングによる仮想メモリのための構造体.
 * この構造体配列で大容量アドレス空間(CARD等)をキャッシュ管理する.
 */
    typedef struct
    {
        u32     src;                   /* logical source address */
        u32     len;                   /* cache length */
        u8     *ptr;                   /* pointer to cache buffer */
        u32     state;                 /* if 1, ready to use */
    }
    MBiCacheInfo;

    typedef struct
    {
        u32     lifetime;              /* if hit or timeout, set 0. */
        u32     recent;                /* unused */
        MBiCacheInfo *p_list;          /* unused */
        u32     size;                  /* unused */
        char    arc_name[FS_ARCHIVE_NAME_LEN_MAX + 1];  /* target archive */
        u32     arc_name_len;          /* archive name length */
        FSArchive *arc_pointer;
        u8      reserved[32 - FS_ARCHIVE_NAME_LEN_MAX - 1 - sizeof(u32) - sizeof(FSArchive*)];
        MBiCacheInfo list[MB_CACHE_INFO_MAX];
    }
    MBiCacheList;


/* function ---------------------------------------------------------------- */



/*---------------------------------------------------------------------------*
  Name:         MBi_InitCache

  Description:  キャッシュリストを初期化する.
                

  Arguments:    pl         キャッシュリスト

  Returns:      None.
 *---------------------------------------------------------------------------*/
    void    MBi_InitCache(MBiCacheList * pl);

/*---------------------------------------------------------------------------*
  Name:         MBi_AttachCacheBuffer

  Description:  キャッシュリストへバッファを割り当てる.
                
  Arguments:    pl         キャッシュリスト
                ptr        割り当てるバッファ
                src        ptr のソースアドレス
                len        ptr のバイトサイズ
                state      指定する初期状態
                           (MB_CACHE_STATE_READY か MB_CACHE_STATE_LOCKED)

  Returns:      None.
 *---------------------------------------------------------------------------*/
    void    MBi_AttachCacheBuffer(MBiCacheList * pl, u32 src, u32 len, void *ptr, u32 state);

/*---------------------------------------------------------------------------*
  Name:         MBi_ReadFromCache

  Description:  指定したアドレスの内容をキャッシュからリードする.

  Arguments:    pl         キャッシュリスト
                src        リード元アドレス
                len        リードサイズ (BYTE)
                dst        リード先アドレス.

  Returns:      キャッシュにヒットすればリードして TRUE, そうでなければ FALSE.
 *---------------------------------------------------------------------------*/
    BOOL    MBi_ReadFromCache(MBiCacheList * pl, u32 src, void *dst, u32 len);

/*---------------------------------------------------------------------------*
  Name:         MBi_TryLoadCache

  Description:  指定したアドレスの内容をキャッシュへロードする.
                最も番地の小さい READY のページキャッシュが破棄される.

  Arguments:    pl         キャッシュリスト
                src        リロード元アドレス
                len        リロードサイズ (BYTE)

  Returns:      リロードが開始できれば TRUE, そうでなければ FALSE.
                (リロード処理のエンジンがシステム内で単一の場合,
                 前回の処理を未了ならこの関数は FALSE を返すであろう)
 *---------------------------------------------------------------------------*/
    BOOL    MBi_TryLoadCache(MBiCacheList * pl, u32 src, u32 len);



#if	defined(__cplusplus)
}
#endif


#endif                                 /* NITRO_MB_CACHE_H_ */

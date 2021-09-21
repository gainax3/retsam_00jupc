/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FS - libraries
  File:     util.h

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/


#if	!defined(NITRO_FS_UTIL_H_)
#define NITRO_FS_UTIL_H_

#include <nitro/misc.h>
#include <nitro/types.h>
#include <nitro/fs/file.h>


#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************/
/* constant */

/*
 * SDK_ARM7FS
 * ARM7 からファイルシステムを使用したい場合はこの定義を有効にする.
 */
/*
#define	SDK_ARM7FS
*/

/*
 * FS_IMPLEMENT
 * ファイルシステムのフルセットを含むべき環境のみ定義される.
 */
#if	!defined(SDK_ARM7) || defined(SDK_ARM7FS)
#define	FS_IMPLEMENT
#endif


/*****************************************************************************/
/* macro */

/* ビットとアラインメントのマクロ */
#define	BIT_MASK(n)	((1 << (n)) - 1)
#define	ALIGN_MASK(a)	((a) - 1)
#define	ALIGN_BYTE(n, a)	(((u32)(n) + ALIGN_MASK(a)) & ~ALIGN_MASK(a))

/* 関数呼び出しの正当性判定 */
#if	!defined(SDK_FINALROM)

/* assertion failure message string */
extern const char *fsi_assert_fs_format;
extern const char *fsi_assert_is_init;
extern const char *fsi_assert_is_valid_arg;
extern const char *fsi_assert_is_valid_dma_channel;
extern const char *fsi_assert_is_empty;
extern const char *fsi_assert_is_handle;
extern const char *fsi_assert_is_file;
extern const char *fsi_assert_is_dir;
extern const char *fsi_assert_is_idle;
extern const char *fsi_assert_irq_enabled;

extern const char *fsi_assert_arc_free;
extern const char *fsi_assert_arc_unloaded;
extern const char *fsi_assert_arc_not_rom;

/* global assertion */
#define	FS_ASSERT_INIT(ret)	\
	if(!FS_IsAvailable())	OS_TPanic(fsi_assert_fs_format, fsi_assert_is_init)

#define	FS_ASSERT_ARG(e, ret)	\
	if(!(e))	OS_TPanic(fsi_assert_fs_format, fsi_assert_is_valid_arg)

#define	FS_ASSERT_DMA_CHANNEL(dma, ret)	\
	if(dma == 0)	OS_TPanic(fsi_assert_fs_format, fsi_assert_is_valid_dma_channel)

/* file and directory assertion */
#define	FS_ASSERT_EMPTY(p_file, ret)	\
	if(FS_IsFile(p_file))	OS_TPanic(fsi_assert_fs_format, fsi_assert_is_empty)

#define	FS_ASSERT_HANDLE(p_file, ret)	\
	if(!FS_IsFile(p_file) && !FS_IsDir(p_file))	OS_TPanic(fsi_assert_fs_format, fsi_assert_is_handle)

#define	FS_ASSERT_FILE(p_file, ret)	\
	if(!FS_IsFile(p_file))	OS_TPanic(fsi_assert_fs_format, fsi_assert_is_file)

#define	FS_ASSERT_DIR(p_file, ret)	\
	if(!FS_IsDir(p_file))	OS_TPanic(fsi_assert_fs_format, fsi_assert_is_dir)

#define	FS_ASSERT_IDLE(p_file, ret)	\
	if(FS_IsBusy(p_file))	OS_TPanic(fsi_assert_fs_format, fsi_assert_is_idle)

#define	FS_ASSERT_IRQ_ENABLED(ret)	\
	if(OS_GetProcMode() == OS_PROCMODE_IRQ) \
	OS_TPanic(fsi_assert_fs_format, fsi_assert_irq_enabled)

/* archive assertion */
#define	FS_ASSERT_ARC_FREE(p_arc, ret)	\
	if(p_arc->name.pack)	OS_TPanic(fsi_assert_fs_format, fsi_assert_arc_free)

#define	FS_ASSERT_ARC_UNLOADED(p_arc, ret)	\
	if(FS_IsArchiveLoaded(p_arc))	OS_TPanic(fsi_assert_fs_format, fsi_assert_arc_unloaded)

#define	FS_ASSERT_ARC_NOT_ROM(p_arc, ret)	\
	if(p_arc == arc_list)	OS_TPanic(fsi_assert_fs_format, fsi_assert_arc_not_rom)

#else

#define	FS_ASSERT_INIT(ret)
#define	FS_ASSERT_ARG(e, ret)
#define	FS_ASSERT_DMA_CHANNEL(dma, ret)

#define	FS_ASSERT_EMPTY(p_file, ret)
#define	FS_ASSERT_HANDLE(p_file, ret)
#define	FS_ASSERT_FILE(p_file, ret)
#define	FS_ASSERT_DIR(p_file, ret)
#define	FS_ASSERT_IDLE(p_file, ret)
#define	FS_ASSERT_IRQ_ENABLED(ret)

#define	FS_ASSERT_ARC_FREE(p_file, ret)
#define	FS_ASSERT_ARC_UNLOADED(p_file, ret)
#define	FS_ASSERT_ARC_NOT_ROM(p_file, ret)

#endif


/*****************************************************************************/
/* function */

#if defined(FS_IMPLEMENT)

/*---------------------------------------------------------------------------*
  Name:         FSi_CutFromListCore

  Description:  内部関数.
                要素を現在の所属キューから切り取るがポインタは再初期化しない.

  Arguments:    trg              切り取る要素.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void FSi_CutFromListCore(FSFileLink *trg)
{
    FSFile *const pr = trg->prev;
    FSFile *const nx = trg->next;
    if (pr)
        pr->link.next = nx;
    if (nx)
        nx->link.prev = pr;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_CutFromList

  Description:  内部関数.
                要素を現在の所属キューから切り取る.

  Arguments:    elem             切り取る要素.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void FSi_CutFromList(FSFile *elem)
{
    FSFileLink *const trg = &elem->link;
    FSi_CutFromListCore(trg);
    trg->next = trg->prev = NULL;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_AppendToList

  Description:  内部関数.
                要素を切り取って指定リストの最後に追加.

  Arguments:    elem             追加する要素.
                list             追加先のリスト.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void FSi_AppendToList(FSFile *elem, FSFile *list)
{
    FSFileLink *const trg = &elem->link;
    FSi_CutFromListCore(trg);
    {
        while (list->link.next)
            list = list->link.next;
        list->link.next = elem;
        trg->prev = list;
        trg->next = NULL;
    }
}

#endif /* FS_IMPLEMENT */


/*---------------------------------------------------------------------------*
  Name:         FSi_IsSlash

  Description:  内部関数.
                ディレクトリ区切り文字か判定.

  Arguments:    c                判定する文字.

  Returns:      ディレクトリ区切り文字なら TRUE.
 *---------------------------------------------------------------------------*/
static inline BOOL FSi_IsSlash(u32 c)
{
    return (c == '/') || (c == '\\');
}

/*---------------------------------------------------------------------------*
  Name:         FSi_IsArchiveRunning

  Description:  内部関数.
                アーカイブが現在処理を実行中か判定.

  Arguments:    p_arc            判定するアーカイブ.

  Returns:      現在処理を実行中なら TRUE.
 *---------------------------------------------------------------------------*/
static inline BOOL FSi_IsArchiveRunning(const volatile FSArchive *p_arc)
{
    return ((p_arc->flag & FS_ARCHIVE_FLAG_RUNNING) != 0);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_IsArchiveCanceling

  Description:  内部関数.
                アーカイブ管理下のファイルがキャンセル要求中か判定.

  Arguments:    p_arc            判定するアーカイブ.

  Returns:      キャンセル要求中なら TRUE.
 *---------------------------------------------------------------------------*/
static inline BOOL FSi_IsArchiveCanceling(const volatile FSArchive *p_arc)
{
    return ((p_arc->flag & FS_ARCHIVE_FLAG_CANCELING) != 0);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_IsArchiveSuspending

  Description:  内部関数.
                アーカイブが現在サスペンド移行中か判定.

  Arguments:    p_arc            判定するアーカイブ.

  Returns:      現在サスペンド移行中なら TRUE.
 *---------------------------------------------------------------------------*/
static inline BOOL FSi_IsArchiveSuspending(const volatile FSArchive *p_arc)
{
    return ((p_arc->flag & FS_ARCHIVE_FLAG_SUSPENDING) != 0);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_IsArchiveAsync

  Description:  内部関数.
                アーカイブが現在非同期コマンド処理中か判定.

  Arguments:    p_arc            判定するアーカイブ.

  Returns:      非同期コマンド処理中なら TRUE.
 *---------------------------------------------------------------------------*/
static inline BOOL FSi_IsArchiveAsync(const volatile FSArchive *p_arc)
{
    return ((p_arc->flag & FS_ARCHIVE_FLAG_IS_ASYNC) != 0);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_IsArchiveAsync

  Description:  内部関数.
                アーカイブが現在同期コマンドのブロッキング中か判定.

  Arguments:    p_arc            判定するアーカイブ.

  Returns:      同期コマンドのブロッキング中なら TRUE.
 *---------------------------------------------------------------------------*/
static inline BOOL FSi_IsArchiveSync(const volatile FSArchive *p_arc)
{
    return ((p_arc->flag & FS_ARCHIVE_FLAG_IS_SYNC) != 0);
}


/*****************************************************************************/


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* NITRO_FS_UTIL_H_ */

/*---------------------------------------------------------------------------*
  $Log: util.h,v $
  Revision 1.19  2005/05/30 04:15:30  yosizaki
  add comments.
  change some functions to inline.

  Revision 1.18  2005/04/28 08:12:31  yosizaki
  add assertions about DMA-0 limitation.

  Revision 1.17  2005/04/18 04:19:12  yosizaki
  fix around FS_ASSERT_IRQ_ENABLED in FINALROM.

  Revision 1.16  2005/04/18 02:47:45  yosizaki
  add macro 'FS_ASSERT_IRQ_ENABLED'.

  Revision 1.15  2005/04/15 08:43:06  yosizaki
  fix assertion macros format. (remove ';' )

  Revision 1.14  2005/03/01 01:57:00  yosizaki
  copyright の年を修正.

  Revision 1.13  2005/02/28 05:26:02  yosizaki
  do-indent.

  Revision 1.12  2004/07/23 08:30:17  yosizaki
  add some macro.

  Revision 1.11  2004/07/19 13:20:35  yosizaki
  add some functions.

  Revision 1.10  2004/07/08 13:40:26  yosizaki
  add some function FSi_Is*()

  Revision 1.9  2004/06/30 04:38:50  yosizaki
  delete FSi_InitArchiveThread.

  Revision 1.8  2004/06/07 10:26:28  yosizaki
  fix around archive asynchronous operation.

  Revision 1.7  2004/06/03 13:33:13  yosizaki
  fix around archive asynchronous operation.

  Revision 1.6  2004/06/01 10:18:04  yosizaki
  change around assertion code.

  Revision 1.5  2004/05/31 02:56:20  yosizaki
  change FSi_ASSERT_ARG() to FS_ASSERT_ARG().

  Revision 1.4  2004/05/20 06:49:27  yosizaki
  small fix.

  Revision 1.3  2004/05/17 08:32:31  yosizaki
  add comment.
  add assertion of each interface function.

  Revision 1.2  2004/05/12 05:22:41  yosizaki
  prepare a option "SDK_ARM7FS".

  Revision 1.1  2004/05/11 04:28:47  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

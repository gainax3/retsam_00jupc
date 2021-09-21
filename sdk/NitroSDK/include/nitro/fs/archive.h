/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FS - libraries
  File:     archive.h

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/


#if	!defined(NITRO_FS_ARCHIVE_H_)
#define NITRO_FS_ARCHIVE_H_

#include <nitro/misc.h>
#include <nitro/types.h>
#include <nitro/os/common/thread.h>


#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************/
/* constant */

enum
{
    /* アーカイブ名の最大長 */
    FS_ARCHIVE_NAME_LEN_MAX = 3
};

/*
 * アーカイブ内部状態フラグ (インライン関数用)
 * ユーザはこれらの定数を直接使用することはありません.
 */
#define	FS_ARCHIVE_FLAG_REGISTER	0x00000001
#define	FS_ARCHIVE_FLAG_LOADED		0x00000002
#define	FS_ARCHIVE_FLAG_TABLE_LOAD	0x00000004
#define	FS_ARCHIVE_FLAG_SUSPEND		0x00000008
#define	FS_ARCHIVE_FLAG_RUNNING		0x00000010
#define	FS_ARCHIVE_FLAG_CANCELING	0x00000020
#define	FS_ARCHIVE_FLAG_SUSPENDING	0x00000040
#define	FS_ARCHIVE_FLAG_UNLOADING	0x00000080
#define	FS_ARCHIVE_FLAG_IS_ASYNC	0x00000100
#define	FS_ARCHIVE_FLAG_IS_SYNC		0x00000200

/*
 * FSArchive::flag は, アーカイブ実装のために
 * 以下のビット領域を自由に定義して使うことができます.
 */
#define	FS_ARCHIVE_FLAG_USER_RESERVED_BIT	0x00010000
#define	FS_ARCHIVE_FLAG_USER_RESERVED_MASK	0xFFFF0000

/*
 * ファイルコマンドおよびアーカイブメッセージ.
 * アーカイブ実装時にユーザプロシージャで使用します.
 */
typedef enum
{
    /* 非同期系コマンド. */
    FS_COMMAND_ASYNC_BEGIN = 0,
    FS_COMMAND_READFILE = FS_COMMAND_ASYNC_BEGIN,
    FS_COMMAND_WRITEFILE,
    FS_COMMAND_ASYNC_END,

    /* 同期系コマンド. */
    FS_COMMAND_SYNC_BEGIN = FS_COMMAND_ASYNC_END,
    FS_COMMAND_SEEKDIR = FS_COMMAND_SYNC_BEGIN,
    FS_COMMAND_READDIR,
    FS_COMMAND_FINDPATH,
    FS_COMMAND_GETPATH,
    FS_COMMAND_OPENFILEFAST,
    FS_COMMAND_OPENFILEDIRECT,
    FS_COMMAND_CLOSEFILE,
    FS_COMMAND_SYNC_END,

    /* アーカイブメッセージ. */
    FS_COMMAND_STATUS_BEGIN = FS_COMMAND_SYNC_END,
    FS_COMMAND_ACTIVATE = FS_COMMAND_STATUS_BEGIN,
    FS_COMMAND_IDLE,
    FS_COMMAND_SUSPEND,
    FS_COMMAND_RESUME,
    FS_COMMAND_STATUS_END,

    FS_COMMAND_INVALID
}
FSCommandType;

/*
 * ユーザプロシージャ設定フラグ.
 * FS_SetArchiveProc() で引数 flags に指定します.
 * コールバックを要求するものだけビットを有効にします.
 */

/* 非同期系コマンド */
#define	FS_ARCHIVE_PROC_READFILE		(1 << FS_COMMAND_READFILE)
#define	FS_ARCHIVE_PROC_WRITEFILE		(1 << FS_COMMAND_WRITEFILE)
        /* 非同期系の全コマンド */
#define	FS_ARCHIVE_PROC_ASYNC	\
	(FS_ARCHIVE_PROC_READFILE | FS_ARCHIVE_PROC_WRITEFILE)

/* 同期系のコマンド */
#define	FS_ARCHIVE_PROC_SEEKDIR			(1 << FS_COMMAND_SEEKDIR)
#define	FS_ARCHIVE_PROC_READDIR			(1 << FS_COMMAND_READDIR)
#define	FS_ARCHIVE_PROC_FINDPATH		(1 << FS_COMMAND_FINDPATH)
#define	FS_ARCHIVE_PROC_GETPATH			(1 << FS_COMMAND_GETPATH)
#define	FS_ARCHIVE_PROC_OPENFILEFAST	(1 << FS_COMMAND_OPENFILEFAST)
#define	FS_ARCHIVE_PROC_OPENFILEDIRECT	(1 << FS_COMMAND_OPENFILEDIRECT)
#define	FS_ARCHIVE_PROC_CLOSEFILE		(1 << FS_COMMAND_CLOSEFILE)
        /* 同期系の全コマンド */
#define	FS_ARCHIVE_PROC_SYNC	\
	(FS_ARCHIVE_PROC_SEEKDIR | FS_ARCHIVE_PROC_READDIR |	\
	 FS_ARCHIVE_PROC_FINDPATH | FS_ARCHIVE_PROC_GETPATH |	\
	FS_ARCHIVE_PROC_OPENFILEFAST | FS_ARCHIVE_PROC_OPENFILEDIRECT | FS_ARCHIVE_PROC_CLOSEFILE)

/* 状態変化時のメッセージ */
#define	FS_ARCHIVE_PROC_ACTIVATE		(1 << FS_COMMAND_ACTIVATE)
#define	FS_ARCHIVE_PROC_IDLE			(1 << FS_COMMAND_IDLE)
#define	FS_ARCHIVE_PROC_SUSPENDING		(1 << FS_COMMAND_SUSPEND)
#define	FS_ARCHIVE_PROC_RESUME			(1 << FS_COMMAND_RESUME)
        /* 状態変化時の全メッセージ */
#define	FS_ARCHIVE_PROC_STATUS	\
	(FS_ARCHIVE_PROC_ACTIVATE | FS_ARCHIVE_PROC_IDLE |	\
	 FS_ARCHIVE_PROC_SUSPENDING | FS_ARCHIVE_PROC_RESUME)

/* すべてのメッセージ */
#define	FS_ARCHIVE_PROC_ALL	(~0)

typedef enum
{
    /****************************************************************
	 * ユーザエラーコード
	 * FS_GetErrorCode() で取得します.
	 ****************************************************************/

    /*
     * 正しく処理した結果としての成功を示します.
     * 例えば 100 BYTE リードしようとしたが
     * 終端に達して 50 BYTE しか読めなかった場合も
     * 「成功」として扱われます.
     */
    FS_RESULT_SUCCESS = 0,

    /*
     * 正しく処理した結果としての失敗を示します.
     * ReadDir で終端を読んだときなど,
     * 処理として認められている想定内の「失敗」を指します.
     */
    FS_RESULT_FAILURE,

    /*
     * 現在コマンド処理中であることを示します.
     * 未完なので, 厳密にはエラーではありません.
     */
    FS_RESULT_BUSY,

    /*
     * コマンドをキャンセルされたことを示します.
     * ユーザ自身による明示的なコマンドキャンセルと,
     * アーカイブアンロード時の強制キャンセルがあります.
     */
    FS_RESULT_CANCELED,
    FS_RESULT_CANCELLED = FS_RESULT_CANCELED,

    /*
     * アーカイブが指定コマンドに対応しなかったことを示します.
     * このエラーはコマンドをあえて無視するアーカイブが発行し,
     * ユーザは対象アーカイブの特殊仕様を把握している必要があります.
     */
    FS_RESULT_UNSUPPORTED,

    /*
     * ファイルシステムまたはアーカイブのエラーによる失敗を示します.
     */
    FS_RESULT_ERROR,

    /****************************************************************
	 * プロシージャレベル エラーコード
	 * ユーザプロシージャが返し, FS_GetErrorCode() には含まれません.
	 ****************************************************************/

    /*
     * コマンドの処理を非同期的に行うため, ひとまず制御を返します.
     * この値を返した場合は, リード・ライトコールバックと同様に
     * 完了を FS_NotifyArchiveAsyncEnd() で通知する必要があります.
     */
    FS_RESULT_PROC_ASYNC,

    /*
     * 少なくとも今回だけはデフォルト処理に任せます.
     * 次回も当該コマンドへのユーザプロシージャは呼び出されます.
     * 単にコマンドをフックしたい場合などは毎回これを返します.
     */
    FS_RESULT_PROC_DEFAULT,

    /*
     * 今回以降のこのコマンドを全てデフォルト処理に任せます.
     * 当該コマンドへのプロシージャ呼び出しフラグは解除され,
     * 次回から呼び出されません.
     */
    FS_RESULT_PROC_UNKNOWN
}
FSResult;


/*****************************************************************************/
/* declaration */

struct FSFile;
struct FSFileLink;
struct FSArchiveFAT;
struct FSArchiveFNT;
struct FSArchive;

/*
 * ユーザプロシージャ
 * アーカイブを特殊化するためにユーザが設定します.
 */
typedef FSResult (*FS_ARCHIVE_PROC_FUNC) (struct FSFile *, FSCommandType);

/*
 * Read/Write アクセス用コールバック
 *
 * 呼び出しの中でアクセスが完了すれば FS_RESULT_SUCCESS を返す必要があります.
 * 非同期処理などで処理が継続されれば FS_RESULT_PROC_ASYNC を返し,
 * 完了時に FS_NotifyArchiveAsyncEnd() をコールする必要があります.
 *
 * 全コマンドのデフォルト処理はこのコールバックを使用して
 * FAT / FNT / ファイルイメージへアクセスすることで実現します.
 * 逆に, 必要な全てのコマンドをユーザプロシージャ内の処理で
 * 適切にオーバーライドすればこれらのコールバックは呼ばれません.
 */
typedef FSResult (*FS_ARCHIVE_READ_FUNC) (struct FSArchive *p, void *dst, u32 pos, u32 size);
typedef FSResult (*FS_ARCHIVE_WRITE_FUNC) (struct FSArchive *p, const void *src, u32 pos, u32 size);

/*
 * 以下の構造体はアーカイブ実装者の利便性のために公開されています.
 * file system API で直接これらを必要とすることはありません.
 */

typedef struct FSFileLink
{
    struct FSFile *prev;
    struct FSFile *next;
}
FSFileLink;

typedef struct FSArchiveFAT
{
    u32     top;
    u32     bottom;
}
FSArchiveFAT;

typedef struct FSArchiveFNT
{
    u32     start;
    u16     index;
    u16     parent;
}
FSArchiveFNT;

/*
 * アーカイブ構造体
 * アーカイブ実装者はこの構造体の実体をひとつ用意し,
 *
 * (1) FS_InitArchive() で初期化.
 * (2) FS_RegisterArchiveName() で名前を登録.
 * (3) FS_LoadArchive() でシステムにロード.
 *
 * の手順でファイルシステムにアーカイブを追加することができます.
 * アーカイブが動的なものであるならば
 *
 * (4) FS_UnloadArchive() でシステムからアンロード.
 *
 * によってアーカイブを解放, 再利用することができます.
 * アーカイブ名も動的に変更したい場合には
 *
 * (5) FS_ReleaseArchiveName() で名前を解放.
 *
 * ののちに (2) からやり直すことができます.
 */

typedef struct FSArchive
{
/* private: */

    /* ファイルシステムの管理用変数 */
    union
    {
        /* アーカイブを識別する一意の名前.
           英数字 1～3 文字で大小文字を区別しない */
        char    ptr[FS_ARCHIVE_NAME_LEN_MAX + 1];
        u32     pack;
    }
    name;
    /* システム内のアーカイブ連結リスト */
    struct FSArchive *next;
    struct FSArchive *prev;

    /* コマンド処理状態の管理用変数 */
    OSThreadQueue sync_q;              /* 同期処理 (SeekDir 等) */
    OSThreadQueue stat_q;              /* 状態変更 (Suspend 等) */
    u32     flag;                      /* 内部状態フラグ (FS_ARCHIVE_FLAG_*) */

    FSFileLink list;                   /* 処理待ちのコマンドリスト */

    /* ROM ファイルフォーマットに準拠する各種オフセット */
    u32     base;                      /* ベースオフセット */
    u32     fat;                       /* FAT オフセット */
    u32     fat_size;                  /* FAT サイズ */
    u32     fnt;                       /* FNT オフセット */
    u32     fnt_size;                  /* FNT サイズ */
    u32     fat_bak;                   /* プリロード前の FAT ベースオフセット */
    u32     fnt_bak;                   /* プリロード前の FNT ベースオフセット */
    void   *load_mem;                  /* ロードされたテーブルのメモリ */

    /* 基本オペレーションコールバック */
    FS_ARCHIVE_READ_FUNC read_func;
    FS_ARCHIVE_WRITE_FUNC write_func;

    /* テーブルへアクセスする際の内部コールバック.
       (内部でいちいち FS_IsArchiveTableLoaded() を使わないため) */
    FS_ARCHIVE_READ_FUNC table_func;

    /* ユーザプロシージャ (FS_ARCHIVE_PROC_*) */
    FS_ARCHIVE_PROC_FUNC proc;
    u32     proc_flag;

}
FSArchive;


/*****************************************************************************/
/* function */

/*---------------------------------------------------------------------------*
  Name:         FS_InitArchive

  Description:  アーカイブ構造体を初期化.

  Arguments:    p_arc            初期化するアーカイブ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    FS_InitArchive(FSArchive *p_arc);

/*---------------------------------------------------------------------------*
  Name:         FS_GetArchiveName

  Description:  アーカイブ名を取得.

  Arguments:    p_arc            名前を取得するアーカイブ.

  Returns:      ファイルシステムに登録されたアーカイブ名.
 *---------------------------------------------------------------------------*/
static inline const char *FS_GetArchiveName(const FSArchive *p_arc)
{
    return p_arc->name.ptr;
}

/*---------------------------------------------------------------------------*
  Name:         FS_GetArchiveBase

  Description:  アーカイブのベースオフセットを取得.

  Arguments:    p_arc            ベースオフセットを取得するアーカイブ.

  Returns:      アーカイブのベースオフセット.
 *---------------------------------------------------------------------------*/
static inline u32 FS_GetArchiveBase(const FSArchive *p_arc)
{
    return p_arc->base;
}

/*---------------------------------------------------------------------------*
  Name:         FS_GetArchiveFAT

  Description:  アーカイブの FAT オフセットを取得.

  Arguments:    p_arc            FAT オフセットを取得するアーカイブ.

  Returns:      アーカイブの FAT オフセット.
 *---------------------------------------------------------------------------*/
static inline u32 FS_GetArchiveFAT(const FSArchive *p_arc)
{
    return p_arc->fat;
}

/*---------------------------------------------------------------------------*
  Name:         FS_GetArchiveFNT

  Description:  アーカイブの FNT オフセットを取得.

  Arguments:    p_arc            FNT オフセットを取得するアーカイブ.

  Returns:      アーカイブの FNT オフセット.
 *---------------------------------------------------------------------------*/
static inline u32 FS_GetArchiveFNT(const FSArchive *p_arc)
{
    return p_arc->fnt;
}

/* アーカイブのベースからの指定位置オフセットを取得 */
/*---------------------------------------------------------------------------*
  Name:         FS_GetArchiveOffset

  Description:  アーカイブのベースからの指定位置オフセットを取得.

  Arguments:    p_arc            アーカイブ.

  Returns:      ベースを加算した指定位置オフセット.
 *---------------------------------------------------------------------------*/
static inline u32 FS_GetArchiveOffset(const FSArchive *p_arc, u32 pos)
{
    return p_arc->base + pos;
}

/*---------------------------------------------------------------------------*
  Name:         FS_IsArchiveLoaded

  Description:  アーカイブが現在ファイルシステムにロード済みか判定.

  Arguments:    p_arc            判定するアーカイブ.

  Returns:      ファイルシステムにロード済みなら TRUE.
 *---------------------------------------------------------------------------*/
static inline BOOL FS_IsArchiveLoaded(volatile const FSArchive *p_arc)
{
    return (p_arc->flag & FS_ARCHIVE_FLAG_LOADED) ? TRUE : FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         FS_IsArchiveTableLoaded

  Description:  アーカイブが現在テーブルをプリロード済みか判定.

  Arguments:    p_arc            判定するアーカイブ.

  Returns:      テーブルをプリロード済みなら TRUE.
 *---------------------------------------------------------------------------*/
static inline BOOL FS_IsArchiveTableLoaded(volatile const FSArchive *p_arc)
{
    return (p_arc->flag & FS_ARCHIVE_FLAG_TABLE_LOAD) ? TRUE : FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         FS_IsArchiveSuspended

  Description:  アーカイブが現在サスペンド中か判定.

  Arguments:    p_arc            判定するアーカイブ.

  Returns:      サスペンド中なら TRUE.
 *---------------------------------------------------------------------------*/
static inline BOOL FS_IsArchiveSuspended(volatile const FSArchive *p_arc)
{
    return (p_arc->flag & FS_ARCHIVE_FLAG_SUSPEND) ? TRUE : FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         FS_FindArchive

  Description:  アーカイブ名を検索する.
                一致する名前が無ければ NULL を返す.

  Arguments:    name             検索するアーカイブ名の文字列.
                name_len         name の文字列長.

  Returns:      検索して見つかったアーカイブのポインタか NULL.
 *---------------------------------------------------------------------------*/
FSArchive *FS_FindArchive(const char *name, int name_len);

/*---------------------------------------------------------------------------*
  Name:         FS_RegisterArchiveName

  Description:  アーカイブ名をファイルシステムへ登録し, 関連付ける.
                アーカイブ自体はまだファイルシステムにロードされない.
                アーカイブ名 "rom" はファイルシステムに予約済み.

  Arguments:    p_arc            名前を関連付けるアーカイブ.
                name             登録する名前の文字列.
                name_len         name の文字列長.

  Returns:      None.
 *---------------------------------------------------------------------------*/
BOOL    FS_RegisterArchiveName(FSArchive *p_arc, const char *name, u32 name_len);

/*---------------------------------------------------------------------------*
  Name:         FS_ReleaseArchiveName

  Description:  登録済みのアーカイブ名を解放する.
                ファイルシステムからアンロードされている必要がある.

  Arguments:    p_arc            名前を解放するアーカイブ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    FS_ReleaseArchiveName(FSArchive *p_arc);

/*---------------------------------------------------------------------------*
  Name:         FS_LoadArchive

  Description:  アーカイブをファイルシステムにロードする.
                すでにアーカイブリストに名前が登録されている必要がある.

  Arguments:    p_arc            ロードするアーカイブ.
                base             独自に使用可能な任意の u32 値.
                fat              FAT テーブルの先頭オフセット.
                fat_size         FAT テーブルのサイズ.
                fnt              FNT テーブルの先頭オフセット.
                fnt_size         FNT テーブルのサイズ.
                read_func        リードアクセスコールバック.
                write_func       ライトアクセスコールバック.

  Returns:      アーカイブが正しくロードされれば TRUE.
 *---------------------------------------------------------------------------*/
BOOL    FS_LoadArchive(FSArchive *p_arc, u32 base,
                       u32 fat, u32 fat_size, u32 fnt, u32 fnt_size,
                       FS_ARCHIVE_READ_FUNC read_func, FS_ARCHIVE_WRITE_FUNC write_func);

/*---------------------------------------------------------------------------*
  Name:         FS_UnloadArchive

  Description:  アーカイブをファイルシステムからアンロードする.
                現在処理中のタスクが全て完了するまでブロッキングする.

  Arguments:    p_arc            アンロードするアーカイブ.

  Returns:      アーカイブが正しくアンロードされれば TRUE.
 *---------------------------------------------------------------------------*/
BOOL    FS_UnloadArchive(FSArchive *p_arc);

/*---------------------------------------------------------------------------*
  Name:         FS_LoadArchiveTables

  Description:  アーカイブの FAT + FNT をメモリ上にプリロードする.
                指定サイズ以内の場合のみ読み込みを実行し, 必要サイズを返す.

  Arguments:    p_arc            テーブルをプリロードするアーカイブ.
                p_mem            テーブルデータの格納先バッファ.
                max_size         p_mem のサイズ.

  Returns:      常に, 合計のテーブルサイズを返す.
 *---------------------------------------------------------------------------*/
u32     FS_LoadArchiveTables(FSArchive *p_arc, void *p_mem, u32 max_size);

/*---------------------------------------------------------------------------*
  Name:         FS_UnloadArchiveTables

  Description:  アーカイブのプリロードメモリを解放する.

  Arguments:    p_arc            プリロードメモリを解放するアーカイブ.

  Returns:      プリロードメモリとしてユーザから与えられていたバッファ.
 *---------------------------------------------------------------------------*/
void   *FS_UnloadArchiveTables(FSArchive *p_arc);

/*---------------------------------------------------------------------------*
  Name:         FS_SuspendArchive

  Description:  アーカイブの処理機構自体を停止する.
                現在実行中の処理があれば, その完了を待機.

  Arguments:    p_arc            停止するアーカイブ.

  Returns:      呼び出し以前にサスペンド状態でなければ TRUE.
 *---------------------------------------------------------------------------*/
BOOL    FS_SuspendArchive(FSArchive *p_arc);

/*---------------------------------------------------------------------------*
  Name:         FS_ResumeArchive

  Description:  停止していたアーカイブの処理を再開する.

  Arguments:    p_arc            再開するアーカイブ.

  Returns:      呼び出し以前にサスペンド状態でなければ TRUE.
 *---------------------------------------------------------------------------*/
BOOL    FS_ResumeArchive(FSArchive *p_arc);

/*---------------------------------------------------------------------------*
  Name:         FS_SetArchiveProc

  Description:  アーカイブのユーザプロシージャを設定する.
                proc == NULL または flags = 0 なら
                単にユーザプロシージャを無効にする.

  Arguments:    p_arc            ユーザプロシージャを設定するアーカイブ.
                proc             ユーザプロシージャ.
                flags            プロシージャへフックするコマンドのビット集合.

  Returns:      常に, 合計のテーブルサイズを返す.
 *---------------------------------------------------------------------------*/
void    FS_SetArchiveProc(struct FSArchive *p_arc, FS_ARCHIVE_PROC_FUNC proc, u32 flags);

/*---------------------------------------------------------------------------*
  Name:         FS_NotifyArchiveAsyncEnd

  Description:  非同期で実行していたアーカイブ処理の完了を通知するために
                アーカイブ実装側から呼び出す.

  Arguments:    p_arc            完了を通知するアーカイブ.
                ret              処理結果.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    FS_NotifyArchiveAsyncEnd(FSArchive *p_arc, FSResult ret);

/*---------------------------------------------------------------------------*
  Name:         FSi_EndArchive

  Description:  全てのアーカイブを終了させて解放する.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    FSi_EndArchive(void);


/*****************************************************************************/


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* NITRO_FS_ARCHIVE_H_ */

/*---------------------------------------------------------------------------*
  $Log: archive.h,v $
  Revision 1.17  2007/08/24 04:34:29  okubata_ryoma
  fix about headerfiles.

  Revision 1.16  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.15  2005/09/02 08:28:29  yosizaki
  add compatible symbol 'FS_RESULT_CANCELLED'

  Revision 1.14  2005/05/30 04:20:42  yosizaki
  add FSi_EndArchive().
  add comments.

  Revision 1.13  2005/03/01 01:57:00  yosizaki
  copyright の年を修正.

  Revision 1.12  2005/02/28 05:26:01  yosizaki
  do-indent.

  Revision 1.11  2004/11/02 10:07:49  yosizaki
  fix typo.

  Revision 1.10  2004/07/23 08:32:27  yosizaki
  fix command operation.

  Revision 1.9  2004/07/19 13:12:59  yosizaki
  add all commands.

  Revision 1.8  2004/07/08 13:45:54  yosizaki
  change archive commands.

  Revision 1.7  2004/06/30 10:28:08  yosizaki
  add comments.

  Revision 1.6  2004/06/30 04:30:46  yosizaki
  implement user-procedure system.

  Revision 1.5  2004/06/22 01:49:37  yosizaki
  add user-procedure system (reserved).

  Revision 1.4  2004/06/03 13:32:53  yosizaki
  fix around archive asyncronous operation.

  Revision 1.3  2004/05/20 12:37:38  yosizaki
  fix FSFileLink definition.

  Revision 1.2  2004/05/20 06:30:33  yosizaki
  add FS_ConvertPathToFileID.
  add some structure for archive.

  Revision 1.1  2004/05/11 04:32:58  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

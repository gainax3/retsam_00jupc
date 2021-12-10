/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - libraries
  File:     mb_fileinfo.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_fileinfo.h,v $
  Revision 1.13  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.12  2005/02/28 05:26:25  yosizaki
  do-indent.

  Revision 1.11  2004/09/25 12:10:38  sato_masaki
  MBi_IsAbleToRecv()を追加。(IsAbleToRecv()からのリネーム)

  Revision 1.10  2004/09/20 12:53:46  sato_masaki
  MBi_get_blockinfo()の返り値を、BOOL値に変更。

  Revision 1.9  2004/09/14 06:34:21  sato_masaki
  MBDownloadFileInfoにreserved領域を追加。

  Revision 1.8  2004/09/07 04:33:46  sato_masaki
  MB_BlockInfoにsegment_noを追加。

  Revision 1.7  2004/09/06 09:20:01  miya
  定義削除

  Revision 1.6  2004/09/06 07:14:22  miya
  仕様変更による変更

  Revision 1.5  2004/09/06 03:48:08  miya
  認証ライブラリ追加

  Revision 1.4  2004/09/06 02:36:00  miya
  仕様変更による定義変更

  Revision 1.3  2004/09/04 11:03:00  sato_masaki
  ROM_HEADER_SIZE_FULLの定義をmb_fileinfo.cから移動。

  Revision 1.2  2004/09/04 09:56:32  sato_masaki
  セグメント、DownloadFileInfoに関する定義をこちらへ移動。

  Revision 1.1  2004/09/03 07:06:28  sato_masaki
  ファイルを機能別に分割。


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef	__MB_FILEINFO_H__
#define	__MB_FILEINFO_H__

#include <nitro.h>
#include <nitro/mb.h>

/* macro definition -------------------------------------------------------- */

#define MB_DL_SEGMENT_NUM				(3)     //      RomHeader, ARM9Static, ARM7Staticの3セグメント
#define MB_DOWNLOAD_FILEINFO_SIZE		(sizeof(MBDownloadFileInfo))

#define MB_AUTHCODE_SIZE				(136)   /* Authentication code size */

#define ROM_HEADER_SIZE_FULL			(0x160)

/* type structure definition --------------------------------------------------------- */

//---------------------------------------------------------
// 親機から子機に送信するマルチブート用ダウンロードファイル情報
//---------------------------------------------------------

/*
 * セグメントタイプ定義
 */
typedef enum MbSegmentType
{
    MB_SEG_ARM9STATIC = 0,
    MB_SEG_ARM7STATIC,
    MB_SEG_ROMHEADER
}
MbSegmentType;

/*
 * マルチブート初期ブートセグメント情報ヘッダ構造体.
 */
typedef struct MbDownloadFileInfoHeader
{
    u32     arm9EntryAddr;             /* ARM9エントリアドレス */
    u32     arm7EntryAddr;             /* ARM7エントリアドレス */
    u32     padding;
}
MbDownloadFileInfoHeader;

/*
 * セグメント構成情報.
 */
typedef struct MbSegmentInfo
{
    u32     recv_addr;                 // 受信時の一時格納アドレス
    u32     load_addr;                 // ロードアドレス       （実行アドレス）
    u32     size;                      // セグメントサイズ
    u32     target:1;                  // ターゲット           （MI_PROCESSOR_ARM9 or _ARM7。無属性のファイルはARM9とする。）
    u32     rsv:31;                    // 予約。
}
MbSegmentInfo;                         // 16byte


/*
 * 初期ブートセグメント全体の構造体.
 */
typedef struct
{
    MbDownloadFileInfoHeader header;   // ヘッダ情報(エントリーアドレスが格納される)
    MbSegmentInfo seg[MB_DL_SEGMENT_NUM];       // セグメント情報
    u32     auth_code[MB_AUTHCODE_SIZE / sizeof(u32)];  // 認証用コード
    u32     reserved[32 / sizeof(u32)]; // 予約領域
}
MBDownloadFileInfo;


/* ブロック情報参照用テーブル */
typedef struct
{
    // 各セグメントにおけるイメージ先頭からのオフセット
    u32     seg_src_offset[MB_DL_SEGMENT_NUM];
    u16     seg_head_blockno[MB_DL_SEGMENT_NUM];        // セグメントの先頭ブロック番号
    u16     block_num;                 /* 総ブロック数 */
}
MB_BlockInfoTable;


/* ブロック転送関連データ */
typedef struct
{
    u32     child_address;             // 子機の格納アドレス
    u32     size;                      // Blockのサイズ
    u32     offset;                    // イメージ先頭からのオフセット値で持つようにする
    u8      segment_no;
    u8      pad[3];
}
MB_BlockInfo;

/* const valriables -------------------------------------------------------- */

extern const MbSegmentType MBi_defaultLoadSegList[MB_DL_SEGMENT_NUM];

/* functions --------------------------------------------------------------- */

/*
 * MbDownloadFileInfoHeaderへのポインタから、
 * セグメント情報を取得
 */
static inline MbSegmentInfo *MBi_GetSegmentInfo(MbDownloadFileInfoHeader * mdfi, int i)
{
    return ((MbSegmentInfo *) (mdfi + 1)) + i;
}

BOOL    MBi_MakeBlockInfoTable(MB_BlockInfoTable * table, MbDownloadFileInfoHeader * mdfi);
BOOL    MBi_get_blockinfo(MB_BlockInfo * bi, MB_BlockInfoTable * table, u32 block,
                          MbDownloadFileInfoHeader * mdfi);
u16     MBi_get_blocknum(MB_BlockInfoTable * table);
BOOL    MBi_IsAbleToRecv(u8 segment_no, u32 address, u32 size);

#endif /* __MB_FILEINFO_H__ */

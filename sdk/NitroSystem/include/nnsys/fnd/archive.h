/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - fnd
  File:     archive.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.4 $
 *---------------------------------------------------------------------------*/

#ifndef NNS_FND_ARCHIVE_H_
#define NNS_FND_ARCHIVE_H_

//#include <stddef.h>
#include <nitro/os.h>
#include <nitro/fs.h>


#ifdef __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*
  Name:         NNSFndArchiveFatEntry

  Description:  アーカイブのＦＡＴエントリ構造体。
 *---------------------------------------------------------------------------*/
typedef struct
{
	u32						fileTop;			// ファイルイメージ先頭のオフセット
	u32						fileBottom;			// ファイルイメージ後尾のオフセット

} NNSiFndArchiveFatEntry;


/*---------------------------------------------------------------------------*
  Name:         NNSiFndArchiveFatData

  Description:  アーカイブのFATデータ構造体。
 *---------------------------------------------------------------------------*/
typedef struct
{
	u32						blockType;			// ブロックタイプ
	u32						blockSize;			// ブロックサイズ
	u16						numFiles;			// ファイルの個数。
	u16						reserved;			// 予約。
	NNSiFndArchiveFatEntry	fatEntries[1];

} NNSiFndArchiveFatData;


/*---------------------------------------------------------------------------*
  Name:         NNSiFndArchiveHeader

  Description:  アーカイブのファイルヘッダ構造体。
 *---------------------------------------------------------------------------*/
typedef struct
{
	u32						signature;			// シグネチャ（NARC）
	u16						byteOrder;			// バイトオーダー（0xfeff）
	u16						version;			// バージョン番号
	u32						fileSize;			// アーカイブファイルの大きさ
	u16						headerSize;			// アーカイブファイルヘッダの大きさ
	u16						dataBlocks;			// データブロックの数

} NNSiFndArchiveHeader;


/*---------------------------------------------------------------------------*
  Name:         NNSFndArchive

  Description:  NNSアーカイブ構造体。アーカイブに関する情報を保持します。
  				NITRO-Systemのアーカイブマネージャで、アーカイブを特定すると
  				きは、このNNSアーカイブ構造体を指定します。
 *---------------------------------------------------------------------------*/
typedef struct
{
	FSArchive				fsArchive;			// ファイルシステム用ワーク
	NNSiFndArchiveHeader*	arcBinary;			// アーカイブバイナリデータの先頭
	NNSiFndArchiveFatData*	fatData;			// アーカイブのFATデータの先頭
	u32						fileImage;			// アーカイブのファイルイメージの先頭

} NNSFndArchive;


/*---------------------------------------------------------------------------*
    関数プロトタイプ。

 *---------------------------------------------------------------------------*/

BOOL		NNS_FndMountArchive(
					NNSFndArchive*		archive,
					const char*			arcName,
					void*				arcBinary);

BOOL		NNS_FndUnmountArchive(
					NNSFndArchive*		archive);

void*		NNS_FndGetArchiveFileByName(
					const char*			path);

void*		NNS_FndGetArchiveFileByIndex(
					NNSFndArchive*		archive,
					u32					index);

BOOL		NNS_FndOpenArchiveFileByIndex(
					FSFile*				file,
					NNSFndArchive*		archive,
					u32					index);


#ifdef __cplusplus
} /* extern "C" */
#endif

/* NNS_FND_ARCHIVE_H_ */
#endif

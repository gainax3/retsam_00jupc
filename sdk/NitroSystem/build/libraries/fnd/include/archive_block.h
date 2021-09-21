/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - fnd
  File:     archive_block.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.3 $
 *---------------------------------------------------------------------------*/

#ifndef NNS_FND_ARCHIVE_BLOCK_H_
#define NNS_FND_ARCHIVE_BLOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*
  Name:         NNSiFndArchiveBlockHeader

  Description:  アーカイブファイルのデータブロックヘッダ構造体。
 *---------------------------------------------------------------------------*/
typedef struct
{
	u32			blockType;				// ブロックタイプ
	u32			blockSize;				// ブロックサイズ

} NNSiFndArchiveBlockHeader;


/*---------------------------------------------------------------------------*
  Name:         NNSiFndArchiveDirBlock
  				NNSiFndArchiveImgBlock

  Description:  アーカイブファイルのディレクトリ情報ブロックヘッダ構造体。
  				アーカイブファイルのファイルイメージブロックヘッダ構造体。
 *---------------------------------------------------------------------------*/

typedef	NNSiFndArchiveBlockHeader	NNSiFndArchiveDirBlockHeader;
typedef	NNSiFndArchiveBlockHeader	NNSiFndArchiveImgBlockHeader;


/*---------------------------------------------------------------------------*
  Name:         NNSFndArchiveFatBlock

  Description:  アーカイブファイルのFATブロックヘッダ構造体。
 *---------------------------------------------------------------------------*/
typedef struct
{
	u32			blockType;				// ブロックタイプ
	u32			blockSize;				// ブロックサイズ
	u16			numFiles;				// ファイルの個数。
	u16			reserved;				// 予約。

} NNSiFndArchiveFatBlockHeader;


#ifdef __cplusplus
} /* extern "C" */
#endif

/* NNS_FND_ARCHIVE_BLOCK_H_ */
#endif

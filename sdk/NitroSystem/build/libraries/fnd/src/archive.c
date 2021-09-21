/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - fnd
  File:     archive.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.6 $
 *---------------------------------------------------------------------------*/

#include <nnsys/misc.h>
#include <nnsys/fnd/archive.h>
#include <string.h>

#include "archive_block.h"


/*---------------------------------------------------------------------------*
  Name:         IsValidArchiveImage

  Description:  指定されたアーカイブヘッダの内容を調べ、アーカイブバイナリが
  				有効なものであるかを判定します。

  Arguments:    arcBinary:	アーカイブヘッダへのポインタ。

  Returns:      もし有効なアーカイブヘッダであれば、真を返します。
 *---------------------------------------------------------------------------*/
static BOOL
IsValidArchiveBinary(NNSiFndArchiveHeader* arcBinary)
{
	NNS_NULL_ASSERT(arcBinary);

	if (arcBinary->signature != 'CRAN')  return FALSE;
	if (arcBinary->byteOrder != 0xfffe)  return FALSE;
	if (arcBinary->version   != 0x0100)  return FALSE;

	return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndMountArchive

  Description:  指定されたアーカイブバイナリをファイルシステムにマウントしま
  				す。アーカイブをマウントする場合には、３文字までの識別名を指
  				定します。ファイルシステム上でアーカイブを特定する場合には、
  				パス名の前にこの識別名を記述することになります。

  Arguments:    archive:	NNSアーカイブ構造体へのポインタ。
  				arcName:	アーカイブをファイルシステム上で識別する為の名前。
  				arcBinary:	メモリ上に読み出されているアーカイブバイナリへの
  							ポインタ。
  				
  Returns:      もしマウントに成功すれば、真を返します。
 *---------------------------------------------------------------------------*/
BOOL
NNS_FndMountArchive(NNSFndArchive* archive, const char* arcName, void* arcBinary)
{
	NNSiFndArchiveHeader*		  arc = (NNSiFndArchiveHeader*)arcBinary;
	NNSiFndArchiveFatBlockHeader* fat = NULL;
	NNSiFndArchiveDirBlockHeader* fnt = NULL;
	NNSiFndArchiveImgBlockHeader* img = NULL;

	NNS_NULL_ASSERT(arcName);
	NNS_NULL_ASSERT(arcBinary);

	if (!IsValidArchiveBinary(arc))
	{
		OS_Printf("This archive binary is invalid.\n");
		return FALSE;
	}
	{
		int  count;
		NNSiFndArchiveBlockHeader* block = (NNSiFndArchiveBlockHeader*)((u32)arc + arc->headerSize);

		for (count=0; count<arc->dataBlocks; count++)
		{
			switch (block->blockType)
			{
				case 'FATB': fat = (NNSiFndArchiveFatBlockHeader*)block;	break;
				case 'FNTB': fnt = (NNSiFndArchiveDirBlockHeader*)block;	break;
				case 'FIMG': img = (NNSiFndArchiveImgBlockHeader*)block;	break;
			}
			block = (NNSiFndArchiveBlockHeader*)((u32)block + block->blockSize);
		}

		FS_InitArchive(&archive->fsArchive);
		archive->arcBinary = arc;
		archive->fatData   = (NNSiFndArchiveFatData*)fat;
		archive->fileImage = (u32)(img+1);

		if (!FS_RegisterArchiveName(&archive->fsArchive, arcName, strlen(arcName)))
		{
			OS_Printf("archive cannot register to file system.\n");
			return FALSE;
		}
		if (!FS_LoadArchive(&archive->fsArchive,
					(u32)(img+1),
					(u32)(fat+1) - (u32)(img+1), fat->blockSize-sizeof(NNSiFndArchiveFatBlockHeader),
					(u32)(fnt+1) - (u32)(img+1), fnt->blockSize-sizeof(NNSiFndArchiveDirBlockHeader),
					NULL, NULL)
		)
		{
			OS_Printf("archive cannot load to file system.\n");
			FS_ReleaseArchiveName(&archive->fsArchive);
			return FALSE;
		}
	}
	return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndUnmountArchive

  Description:  ファイルシステムにマウントされているアーカイブをアンマウント
  				します。

  Arguments:    archive:	NNSアーカイブ構造体へのポインタ。

  Returns:      もし、アンマウントに成功すれば、真を返します。
 *---------------------------------------------------------------------------*/
BOOL
NNS_FndUnmountArchive(NNSFndArchive* archive)
{
	NNS_NULL_ASSERT(archive);

	if (!FS_UnloadArchive(&archive->fsArchive))
	{
		return FALSE;
	}
	FS_ReleaseArchiveName(&archive->fsArchive);
	return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetArchiveFileByName

  Description:  ファイルシステムを使用して、指定されたパスに対応するアーカイ
  				ブファイルが格納されているＲＡＭアドレスを取得します。

  Arguments:    path:	アーカイブファイルのパス名。

  Returns:      アーカイブファイルが格納されているアドレス。
 *---------------------------------------------------------------------------*/
void*
NNS_FndGetArchiveFileByName(const char* path)
{
	void* addr = NULL;

#if 1
	FSFile file;

	NNS_NULL_ASSERT(path);
	FS_InitFile(&file);
	if (FS_OpenFile(&file, path))
	{
		NNSFndArchive* archive = (NNSFndArchive*)FS_GetAttachedArchive(&file);

		addr = (void*)(archive->fileImage + FS_GetFileImageTop(&file));
		(void)FS_CloseFile(&file);
	}
#else
	FSFileID fileID;

	NNS_NULL_ASSERT(path);
	if (FS_ConvertPathToFileID(&fileID, path))
	{
		NNSFndArchive* archive = (NNSFndArchive*)fileID.arc;

		addr = (void*)(archive->fileImage + archive->fatData->fatEntries[fileID.file_id].fileTop);
	}
#endif

	return addr;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetArchiveFileByIndex

  Description:  インデックスにより指定されたアーカイブファイルが格納されてい
  				るＲＡＭアドレスを取得します。

  Arguments:    archive:	NNSアーカイブ構造体へのポインタ。
  				index:		アーカイブ内のファイルのインデックス値。

  Returns:      アーカイブファイルが格納されているアドレスを返します。もし、
  				指定したインデックスに対応する無かった場合には、NULLを返しま
  				す。
 *---------------------------------------------------------------------------*/
void*
NNS_FndGetArchiveFileByIndex(NNSFndArchive* archive, u32 index)
{
	void* addr = NULL;

	NNS_NULL_ASSERT(archive);

	if (index < archive->fatData->numFiles)
	{
		addr = (void*)(archive->fileImage + archive->fatData->fatEntries[index].fileTop);
	}
	return addr;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndOpenArchiveFileByIndex

  Description:  インデックスにより指定されたアーカイブファイルをオープンしま
  				す。

  Arguments:	file:		ファイル構造体へのポインタ。
  			    archive:	NNSアーカイブ構造体へのポインタ。
  				index:		アーカイブ内のファイルのインデックス値。

  Returns:      ファイルのオープンに成功すれば真を返します。
 *---------------------------------------------------------------------------*/
BOOL
NNS_FndOpenArchiveFileByIndex(FSFile* file, NNSFndArchive* archive, u32 index)
{
	BOOL result = FALSE;

	NNS_NULL_ASSERT(file);
	NNS_NULL_ASSERT(archive);

	if (index < archive->fatData->numFiles)
	{
		FSFileID fileID;

		fileID.arc	   = &archive->fsArchive;
		fileID.file_id = index;

		result = FS_OpenFileFast(file, fileID);
	}
	return result;
}

/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - mcs
  File:     fileIOcommoni.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.9 $
 *---------------------------------------------------------------------------*/

#ifndef NNS_MCS_FILEIOCOMMONI_H_
#define NNS_MCS_FILEIOCOMMONI_H_


#if defined(_MSC_VER)
    #include "mcsStdInt.h"
#else
    #include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================
    定数定義
   ======================================================================== */

#define NNSi_MCS_FILEIO_CHANNEL  (uint16_t)('FI' +0x8000)

enum
{
    NNSi_MCS_FILEIO_CMD_FILEOPEN  ,
    NNSi_MCS_FILEIO_CMD_FILECLOSE ,
    NNSi_MCS_FILEIO_CMD_FILEREAD  ,
    NNSi_MCS_FILEIO_CMD_FILEWRITE ,
    NNSi_MCS_FILEIO_CMD_BROWSEFILE,
    NNSi_MCS_FILEIO_CMD_FINDFIRST ,
    NNSi_MCS_FILEIO_CMD_FINDNEXT  ,
    NNSi_MCS_FILEIO_CMD_FINDCLOSE ,
    NNSi_MCS_FILEIO_CMD_FILESEEK
};

enum
{
    NNSi_MCS_FILEIO_OPEN_DIRECT,
    NNSi_MCS_FILEIO_OPEN_DIALOG
};


/* ========================================================================
    型定義
   ======================================================================== */

#ifdef _WIN32

typedef uint32_t                NNSiMcsVoidPtr;
typedef uint32_t                NNSiMcsFilePtr;
typedef uint32_t                NNSiMcsFileFindDataPtr;

#else

typedef void*                   NNSiMcsVoidPtr;
typedef NNSMcsFile*             NNSiMcsFilePtr;
typedef NNSMcsFileFindData*     NNSiMcsFileFindDataPtr;

/* #ifdef _WIN32 */
#endif

/* ------------------------------------------------------------------------
    file I/O サーバに送るコマンド
   ------------------------------------------------------------------------ */

typedef struct NNSiMcsFileIOCmdHeader NNSiMcsFileIOCmdHeader;
struct NNSiMcsFileIOCmdHeader
{
    uint16_t                command;
    uint16_t                type;
};

/*
    ファイルオープン用のコマンド
*/
typedef struct NNSiMcsFileIOCmdOpen NNSiMcsFileIOCmdOpen;
struct NNSiMcsFileIOCmdOpen
{
    uint16_t                command;
    uint16_t                type;
    NNSiMcsFilePtr          pFile;
    uint32_t                flag;
    char                    filename[NNS_MCS_FILEIO_PATH_MAX];
};

/*
    ファイルクローズ用のコマンド
*/
typedef struct NNSiMcsFileIOCmdClose NNSiMcsFileIOCmdClose;
struct NNSiMcsFileIOCmdClose
{
    uint16_t                command;
    uint16_t                type;
    NNSiMcsFilePtr          pFile;
    uint32_t                handle;
};

/*
    ファイルリード用のコマンド
*/
typedef struct NNSiMcsFileIOCmdRead NNSiMcsFileIOCmdRead;
struct NNSiMcsFileIOCmdRead
{
    uint16_t                command;
    uint16_t                type;
    NNSiMcsFilePtr          pFile;
    uint32_t                handle;
    NNSiMcsVoidPtr          pBuffer;
    uint32_t                size;
};

/*
  ファイルライト用のコマンド
*/
typedef struct NNSiMcsFileIOCmdWrite NNSiMcsFileIOCmdWrite;
struct NNSiMcsFileIOCmdWrite
{
    uint16_t                command;
    uint16_t                type;
    NNSiMcsFilePtr          pFile;
    uint32_t                handle;
    uint32_t                size;
};

/*
  ファイルシーク用のコマンド
*/
typedef struct NNSiMcsFileIOCmdFileSeek NNSiMcsFileIOCmdFileSeek;
struct NNSiMcsFileIOCmdFileSeek
{
    uint16_t                command;
    uint16_t                type;
    NNSiMcsFilePtr          pFile;
    uint32_t                handle;
    int32_t                 distanceToMove;
    uint32_t                moveMethod;
};

/*
    FindFirstFile用のコマンド
*/
typedef struct NNSiMcsFileIOCmdFindFirst NNSiMcsFileIOCmdFindFirst;
struct NNSiMcsFileIOCmdFindFirst
{
    uint16_t                command;
    uint16_t                type;
    NNSiMcsFilePtr          pFile;
    NNSiMcsFileFindDataPtr  pFindData;
    char                    pPattern[NNS_MCS_FILEIO_PATH_MAX];
};

/*
    FindNextFile用コマンド
*/
typedef struct NNSiMcsFileIOCmdFindNext NNSiMcsFileIOCmdFindNext;
struct NNSiMcsFileIOCmdFindNext
{
    uint16_t                command;
    uint16_t                type;
    NNSiMcsFilePtr          pFile;
    NNSiMcsFileFindDataPtr  pFindData;
    uint32_t                handle;
};

/* ------------------------------------------------------------------------
    WindowsからのResult
   ------------------------------------------------------------------------ */

typedef struct NNSiMcsFileIOResult NNSiMcsFileIOResult;
struct NNSiMcsFileIOResult
{
    uint16_t                command;
    uint16_t                result;
    uint32_t                srvErrCode;
    NNSiMcsFilePtr          pFile;
};

/*
    FileOpenのResult
*/
typedef struct NNSiMcsFileIOResultOpen NNSiMcsFileIOResultOpen;
struct NNSiMcsFileIOResultOpen
{
    uint16_t                command;
    uint16_t                result;
    uint32_t                srvErrCode;
    NNSiMcsFilePtr          pFile;
    uint32_t                handle;
    uint32_t                filesize;
};

typedef struct NNSiMcsFileIOResultRead NNSiMcsFileIOResultRead;
struct NNSiMcsFileIOResultRead
{
    uint16_t                command;
    uint16_t                result;
    uint32_t                srvErrCode;
    NNSiMcsFilePtr          pFile;
    NNSiMcsVoidPtr          pBuffer;
    uint32_t                size;
    uint32_t                totalSize;
};

typedef struct NNSiMcsFileIOResultFileSeek NNSiMcsFileIOResultFileSeek;
struct NNSiMcsFileIOResultFileSeek
{
    uint16_t                command;
    uint16_t                result;
    uint32_t                srvErrCode;
    NNSiMcsFilePtr          pFile;
    uint32_t                filePointer;
};

typedef struct NNSiMcsFileIOResultFind NNSiMcsFileIOResultFind;
struct NNSiMcsFileIOResultFind
{
    uint16_t                command;
    uint16_t                result;
    uint32_t                srvErrCode;
    NNSiMcsFilePtr          pFile;
    NNSiMcsFileFindDataPtr  pFindData;
    uint32_t                handle;
    uint32_t                filesize;
    uint32_t                attribute;
    char                    pFilename[NNS_MCS_FILEIO_PATH_MAX];
};


#ifdef __cplusplus
} /* extern "C" */
#endif

/* NNS_MCS_FILEIOCOMMONI_H_ */
#endif

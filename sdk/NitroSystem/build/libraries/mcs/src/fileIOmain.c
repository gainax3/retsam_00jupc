/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - mcs
  File:     fileIOmain.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.17 $
 *---------------------------------------------------------------------------*/

#if ! defined(NNS_FINALROM)

#include <nitro.h>
#include <string.h>

#include <nnsys/misc.h>
#include <nnsys/mcs/baseCommon.h>
#include <nnsys/mcs/base.h>
#include <nnsys/mcs/ringBuffer.h>
#include <nnsys/mcs/fileIOcommon.h>
#include <nnsys/mcs/fileIObase.h>
#include <nnsys/mcs/config.h>

#include "baseCommoni.h"
#include "basei.h"
#include "fileIOcommoni.h"


/* ========================================================================
    定数
   ======================================================================== */

static int const SEND_RETRY_MAX = 10;


/* ========================================================================
    static変数
   ======================================================================== */

static NNSiMcsFileIOWork*   spFileIOWork = NULL;


/* ========================================================================
    static関数
   ======================================================================== */

static NNS_MCS_INLINE
BOOL
IsInitialized()
{
    return spFileIOWork != NULL;
}

static NNS_MCS_INLINE BOOL
CheckConnect()
{
    if (NNS_McsIsServerConnect())
    {
        return TRUE;
    }
    else
    {
        OS_Printf("NNS Mcs file I/O: not sever connect\n");
        return FALSE;
    }
}

static void
CopyPathString(
    char*       dst,
    const char* src
)
{
    (void)strncpy(dst, src, NNS_MCS_FILEIO_PATH_MAX - 1);
    dst[NNS_MCS_FILEIO_PATH_MAX - 1] = '\0';
}

static void
OnDefaltOperation(
    const void* pRecv,
    u32         /*recvSize*/
)
{
    NNSiMcsFileIOResult *const cmd = (NNSiMcsFileIOResult*)pRecv;

    cmd->pFile->errCode     = cmd->result;
    cmd->pFile->srvErrCode  = cmd->srvErrCode;

    cmd->pFile->bDone = 1;
}

static void
OnOpen(
    const void* pRecv,
    u32         /*recvSize*/
)
{
    NNSiMcsFileIOResultOpen *const cmd = (NNSiMcsFileIOResultOpen*)pRecv;

    cmd->pFile->handle      = cmd->handle;
    cmd->pFile->filesize    = cmd->filesize;
    cmd->pFile->errCode     = cmd->result;
    cmd->pFile->srvErrCode  = cmd->srvErrCode;

    cmd->pFile->bDone       = 1;
}

static void
OnRead(
    const void* pRecv,
    u32         recvSize
)
{
    NNSiMcsFileIOResultRead *const cmd = (NNSiMcsFileIOResultRead*)pRecv;
    const u32 size = recvSize - sizeof(NNSiMcsFileIOResultRead);

    if ( cmd->result == NNSi_MCS_FILEIO_RESULT_READING
      || cmd->result == NNS_MCS_FILEIO_ERROR_SUCCESS
    )
    {
        (void)memcpy(cmd->pBuffer, (char*)pRecv + sizeof(NNSiMcsFileIOResultRead), size);
    }

    if (cmd->result != NNSi_MCS_FILEIO_RESULT_READING)
    {
        cmd->pFile->errCode     = cmd->result;
        cmd->pFile->srvErrCode  = cmd->srvErrCode;
        cmd->pFile->tempData    = cmd->totalSize;

        cmd->pFile->bDone  = 1;
    }
}

static void
OnSeekFile(
    const void* pRecv,
    u32         /*recvSize*/
)
{
    NNSiMcsFileIOResultFileSeek *const cmd = (NNSiMcsFileIOResultFileSeek*)pRecv;

    cmd->pFile->errCode     = cmd->result;
    cmd->pFile->srvErrCode  = cmd->srvErrCode;
    cmd->pFile->tempData    = cmd->filePointer;

    cmd->pFile->bDone = 1;
}

static void
OnFind(
    const void* pRecv,
    u32         /*recvSize*/,
    u16         type
)
{
    NNSiMcsFileIOResultFind* cmd = (NNSiMcsFileIOResultFind*)pRecv;

    if (type == NNSi_MCS_FILEIO_CMD_FINDFIRST)
    {
        cmd->pFile->handle          = cmd->handle;
        cmd->pFile->filesize        = 0;
    }
    cmd->pFile->errCode         = cmd->result;
    cmd->pFile->srvErrCode      = cmd->srvErrCode;

    if (cmd->result == NNS_MCS_FILEIO_ERROR_SUCCESS)
    {
        // 成功したら FindData のメンバをセット
        cmd->pFindData->size        = cmd->filesize;
        cmd->pFindData->attribute   = cmd->attribute;
        CopyPathString(cmd->pFindData->name, cmd->pFilename);
    }

    cmd->pFile->bDone = 1;
}

static void
RecvCallback(
    const void* pRecv,
    u32         recvSize,
    u32         /*userData*/,
    u32         offset,
    u32         /*totalSize*/
)
{
    const u16 cmd = *(u16*)pRecv;

    NNS_ASSERT(offset == 0);    // 分割されていないこと
    (void)offset;

    switch (cmd)
    {
    case NNSi_MCS_FILEIO_CMD_FILEOPEN:  OnOpen           (pRecv, recvSize);         break;
    case NNSi_MCS_FILEIO_CMD_FILECLOSE: OnDefaltOperation(pRecv, recvSize);         break;
    case NNSi_MCS_FILEIO_CMD_FILEREAD:  OnRead           (pRecv, recvSize);         break;
    case NNSi_MCS_FILEIO_CMD_FILEWRITE: OnDefaltOperation(pRecv, recvSize);         break;
    case NNSi_MCS_FILEIO_CMD_FILESEEK:  OnSeekFile       (pRecv, recvSize);         break;
    case NNSi_MCS_FILEIO_CMD_FINDFIRST: OnFind           (pRecv, recvSize, cmd);    break;
    case NNSi_MCS_FILEIO_CMD_FINDNEXT:  OnFind           (pRecv, recvSize, cmd);    break;
    case NNSi_MCS_FILEIO_CMD_FINDCLOSE: OnDefaltOperation(pRecv, recvSize);         break;
    }
}

/*
    データを送信
        成功するまで数回リトライ
*/
static BOOL
WriteStream(
    NNSMcsFile* pFile,
    const void* buf,
    u32         length
)
{
    int retryCount;

    pFile->bDone  = 0;

    for (retryCount = 0; retryCount < SEND_RETRY_MAX; ++retryCount)
    {
        u32 writableLength;
        if (! NNS_McsGetStreamWritableLength(&writableLength))
        {
            return FALSE;
        }

        if (length <= writableLength)
        {
            return NNS_McsWriteStream(NNSi_MCS_FILEIO_CHANNEL, buf, length);
        }
        else
        {
            // OS_Printf("ERROR: FIO: buffer short - retry\n");
            NNSi_McsSleep(16);
        }
    }

    OS_Printf("NNS Mcs File I/O: send time out\n");
    return FALSE;
}

/*
    受信待ちをします。
*/
static void
DoLoop(NNSMcsFile* pFindInfo)
{
    do
    {
        NNS_McsPollingIdle();

        NNSi_McsSleep(8);
    }
    while (pFindInfo->bDone == 0);
}


/* ========================================================================
    外部関数
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:         NNS_McsInitFileIO

  Description:  File I/O APIの初期化。

  Arguments:    workMem:  MCSが使用するワーク用メモリ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void
NNS_McsInitFileIO(void* workMem)
{
    if (IsInitialized())
    {
        return;
    }

    NNS_NULL_ASSERT(workMem);

    spFileIOWork = (NNSiMcsFileIOWork*)workMem;

    NNS_McsRegisterRecvCallback(&spFileIOWork->cbInfo, NNSi_MCS_FILEIO_CHANNEL, RecvCallback, 0);
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsOpenFile

  Description:  既存のファイルをオープンあるいは新規のファイルを作成してオープン。

  Arguments:    pFile:     ファイル情報構造体へのポインタ。
                fileName:  ファイル名。
                openFlag:  ファイルの作成、オープン方法および読み書きの種類の指定。

  Returns:      関数が成功したら 0、失敗したら 0以外の値を返す。
 *---------------------------------------------------------------------------*/
u32
NNS_McsOpenFile(
    NNSMcsFile* pFile,
    const char* fileName,
    u32         openFlag
)
{
    NNSiMcsFileIOCmdOpen *const pCmd = (NNSiMcsFileIOCmdOpen*)spFileIOWork->sendBuffer;

    pFile->srvErrCode = 0;

    if (! CheckConnect())
    {
        return pFile->errCode = NNS_MCS_FILEIO_ERROR_NOTCONNECT;
    }

    pCmd->command   = NNSi_MCS_FILEIO_CMD_FILEOPEN;
    pCmd->pFile     = pFile;
    pCmd->flag      = openFlag;

    // ファイル名がNULLの場合は、ダイアログを開いてファイルを選択させる。
    if (fileName != NULL)
    {
        CopyPathString(pCmd->filename, fileName);
        pCmd->type      = NNSi_MCS_FILEIO_OPEN_DIRECT;
    }
    else
    {
        pCmd->type      = NNSi_MCS_FILEIO_OPEN_DIALOG;
    }

    if (! WriteStream(pFile, pCmd, sizeof(*pCmd)))
    {
        return pFile->errCode = NNS_MCS_FILEIO_ERROR_COMERROR;
    }

    DoLoop(pFile);

    return pFile->errCode;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsCloseFile

  Description:  ファイルのクローズ。

  Arguments:    pFile:  ファイル情報構造体へのポインタ。

  Returns:      関数が成功したら 0、失敗したら 0以外の値を返す。
 *---------------------------------------------------------------------------*/
u32
NNS_McsCloseFile(NNSMcsFile* pFile)
{
    NNSiMcsFileIOCmdClose *const pCmd = (NNSiMcsFileIOCmdClose*)spFileIOWork->sendBuffer;

    pFile->srvErrCode = 0;

    if (! CheckConnect())
    {
        return pFile->errCode = NNS_MCS_FILEIO_ERROR_NOTCONNECT;
    }

    pCmd->command   = NNSi_MCS_FILEIO_CMD_FILECLOSE;
    pCmd->type      = 0;
    pCmd->pFile     = pFile;
    pCmd->handle    = pFile->handle;

    if (! WriteStream(pFile, pCmd, sizeof(*pCmd)))
    {
        return pFile->errCode = NNS_MCS_FILEIO_ERROR_COMERROR;
    }

    DoLoop(pFile);

    return pFile->errCode;
}


/*---------------------------------------------------------------------------*
  Name:         NNS_McsReadFile

  Description:  ファイルからの読み込み。

  Arguments:    pFile:       ファイル情報構造体へのポインタ。
                buffer:      読み込むためのバッファ。
                length:      読み込むバイト数。
                pReadBytes:  実際に読み込んだバイト数を返す変数へのポインタ。

  Returns:      関数が成功したら 0、失敗したら 0以外の値を返す。
 *---------------------------------------------------------------------------*/
u32
NNS_McsReadFile(
    NNSMcsFile* pFile,
    void*       buffer,
    u32         length,
    u32*        pReadBytes
)
{
    NNSiMcsFileIOCmdRead *const pCmd = (NNSiMcsFileIOCmdRead*)spFileIOWork->sendBuffer;

    pFile->srvErrCode = 0;

    if (! CheckConnect())
    {
        return pFile->errCode = NNS_MCS_FILEIO_ERROR_NOTCONNECT;
    }

    pCmd->command   = NNSi_MCS_FILEIO_CMD_FILEREAD;
    pCmd->type      = 0;
    pCmd->pFile     = pFile;
    pCmd->handle    = pFile->handle;
    pCmd->pBuffer   = buffer;
    pCmd->size      = length;

    // コマンド送信
    if (! WriteStream(pFile, pCmd, sizeof(*pCmd)))
    {
        return pFile->errCode = NNS_MCS_FILEIO_ERROR_COMERROR;
    }

    // コマンド結果待ち
    DoLoop(pFile);

    if (! pFile->errCode)
    {
        *pReadBytes = pFile->tempData;  // 読み込みバイト数
    }

    return pFile->errCode;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsWriteFile

  Description:  ファイルへの書き込み。

  Arguments:    pFile:   ファイル情報構造体へのポインタ。
                buffer:  書き込む内容を保持するバッファへのポインタ。
                length:  書き込むバイト数。

  Returns:      関数が成功したら 0、失敗したら 0以外の値を返す。
 *---------------------------------------------------------------------------*/
u32
NNS_McsWriteFile(
    NNSMcsFile* pFile,
    const void* buffer,
    u32         length
)
{
    NNSiMcsFileIOCmdWrite *const pCmd = (NNSiMcsFileIOCmdWrite*)spFileIOWork->sendBuffer;
    const u8* fromBuf = buffer;
    u8* sendBuf = spFileIOWork->sendBuffer + sizeof(NNSiMcsFileIOCmdWrite);
    const u32 maxsize = sizeof(spFileIOWork->sendBuffer) - sizeof(NNSiMcsFileIOCmdWrite);

    pFile->srvErrCode = 0;

    if (! CheckConnect())
    {
        return pFile->errCode = NNS_MCS_FILEIO_ERROR_NOTCONNECT;
    }

    pCmd->command   = NNSi_MCS_FILEIO_CMD_FILEWRITE;
    pCmd->type      = 0;
    pCmd->pFile     = pFile;
    pCmd->handle    = pFile->handle;

    while (length > 0)
    {
        const u32 sendSize = length < maxsize ? length: maxsize;
        (void)memcpy(sendBuf, fromBuf, sendSize);
        pCmd->size = sendSize;

        if (! WriteStream(pFile, spFileIOWork->sendBuffer, sendSize + sizeof(NNSiMcsFileIOCmdWrite)))
        {
            return pFile->errCode = NNS_MCS_FILEIO_ERROR_COMERROR;
        }

        DoLoop(pFile);

        // エラーが発生したら抜ける
        if (pFile->errCode)
        {
            return pFile->errCode;
        }

        length -= sendSize;
        fromBuf += sendSize;
    }

    return pFile->errCode;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsSeekFile

  Description:  指定された位置にファイル ポインタを移動します。

  Arguments:    pFile:            ファイル情報構造体へのポインタ。
                distanceToMove:   ファイルポインタを移動するバイト数
                moveMethod:       ファイルポインタの移動を開始する位置
                pNewFilePointer:  新しいファイルポインタを受け取る変数へのポインタ

  Returns:      関数が成功したら 0、失敗したら 0以外の値を返す。
 *---------------------------------------------------------------------------*/
u32
NNS_McsSeekFile(
    NNSMcsFile* pFile,
    s32         distanceToMove,
    u32         moveMethod,
    u32*        pNewFilePointer
)
{
    NNSiMcsFileIOCmdFileSeek *const pCmd = (NNSiMcsFileIOCmdFileSeek*)spFileIOWork->sendBuffer;

    pFile->srvErrCode = 0;

    if (! CheckConnect())
    {
        return pFile->errCode = NNS_MCS_FILEIO_ERROR_NOTCONNECT;
    }

    pCmd->command           = NNSi_MCS_FILEIO_CMD_FILESEEK;
    pCmd->type              = 0;
    pCmd->pFile             = pFile;
    pCmd->handle            = pFile->handle;
    pCmd->distanceToMove    = distanceToMove;
    pCmd->moveMethod        = moveMethod;

    if (! WriteStream(pFile, pCmd, sizeof(*pCmd)))
    {
        return pFile->errCode = NNS_MCS_FILEIO_ERROR_COMERROR;
    }

    DoLoop(pFile);

    if (! pFile->errCode && pNewFilePointer)
    {
        *pNewFilePointer = pFile->tempData;     // 新しいファイルポインタ位置
    }

    return pFile->errCode;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsFindFirstFile

  Description:  指定されたファイル名に一致するファイルを、ディレクトリ内で検索する。

  Arguments:    pFile:      ファイル情報構造体へのポインタ。
                pFindData:  見つかったファイルに関する情報を格納する構造体へのポインタ。
                pattern:    検索するファイル名パターン。

  Returns:      関数が成功したら 0、失敗したら 0以外の値を返す。
 *---------------------------------------------------------------------------*/
u32
NNS_McsFindFirstFile(
    NNSMcsFile*         pFile,
    NNSMcsFileFindData* pFindData,
    const char*         pattern
)
{
    NNSiMcsFileIOCmdFindFirst *const pCmd = (NNSiMcsFileIOCmdFindFirst*)spFileIOWork->sendBuffer;

    pFile->srvErrCode = 0;

    if (! CheckConnect())
    {
        return pFile->errCode = NNS_MCS_FILEIO_ERROR_NOTCONNECT;
    }

    pCmd->command   = NNSi_MCS_FILEIO_CMD_FINDFIRST;
    pCmd->type      = 0;
    pCmd->pFile     = pFile;
    pCmd->pFindData = pFindData;
    CopyPathString(pCmd->pPattern, pattern);

    if (! WriteStream(pFile, pCmd, sizeof(*pCmd)))
    {
        return pFile->errCode = NNS_MCS_FILEIO_ERROR_COMERROR;
    }

    DoLoop(pFile);

    return pFile->errCode;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsFindNextFile

  Description:  NNS_McsFindFirstFile()で指定したパターンに一致するファイルの続きを検索する。

  Arguments:    pFile:      ファイル情報構造体へのポインタ。
                pFindData:  見つかったファイルに関する情報を格納する構造体へのポインタ。

  Returns:      関数が成功したら 0、失敗したら 0以外の値を返す。
 *---------------------------------------------------------------------------*/
u32
NNS_McsFindNextFile(
    NNSMcsFile*         pFile,
    NNSMcsFileFindData* pFindData
)
{
    NNSiMcsFileIOCmdFindNext *const pCmd = (NNSiMcsFileIOCmdFindNext*)spFileIOWork->sendBuffer;

    pFile->srvErrCode = 0;

    if (! CheckConnect())
    {
        return pFile->errCode = NNS_MCS_FILEIO_ERROR_NOTCONNECT;
    }

    pCmd->command   = NNSi_MCS_FILEIO_CMD_FINDNEXT;
    pCmd->type      = 0;
    pCmd->pFile     = pFile;
    pCmd->pFindData = pFindData;
    pCmd->handle    = pFile->handle;

    if (! WriteStream(pFile, pCmd, sizeof(*pCmd)))
    {
        return pFile->errCode = NNS_MCS_FILEIO_ERROR_COMERROR;
    }

    DoLoop(pFile);

    return pFile->errCode;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsCloseFind

  Description:  NNS_McsFindFirstFile()で開始した検索を終了する。

  Arguments:    pFile:  ファイル情報構造体へのポインタ。

  Returns:      関数が成功したら 0、失敗したら 0以外の値を返す。
 *---------------------------------------------------------------------------*/
u32
NNS_McsCloseFind(NNSMcsFile* pFile)
{
    NNSiMcsFileIOCmdClose *const pCmd = (NNSiMcsFileIOCmdClose*)spFileIOWork->sendBuffer;

    pFile->srvErrCode = 0;

    if (! CheckConnect())
    {
        return pFile->errCode = NNS_MCS_FILEIO_ERROR_NOTCONNECT;
    }

    pCmd->command   = NNSi_MCS_FILEIO_CMD_FINDCLOSE;
    pCmd->type      = 0;
    pCmd->pFile     = pFile;
    pCmd->handle    = pFile->handle;

    if (! WriteStream(pFile, pCmd, sizeof(*pCmd)))
    {
        return pFile->errCode = NNS_MCS_FILEIO_ERROR_COMERROR;
    }

    DoLoop(pFile);

    return pFile->errCode;
}


/* #if ! defined(NNS_FINALROM) */
#endif

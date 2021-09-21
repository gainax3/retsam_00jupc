/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - mcs
  File:     print.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.9 $
 *---------------------------------------------------------------------------*/

#if ! defined(NNS_FINALROM)


#include <nitro.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <nnsys/misc.h>
#include <nnsys/mcs/baseCommon.h>
#include <nnsys/mcs/base.h>
#include <nnsys/mcs/print.h>
#include <nnsys/mcs/config.h>

#include "basei.h"
#include "printi.h"


/* ========================================================================
    定数
   ======================================================================== */

static const int        SEND_RETRY_MAX = 10;


/* ========================================================================
    static変数
   ======================================================================== */

static NNSMcsRecvCBInfo sCBInfo;
static void*            spBuffer    = NULL;
static u32              sBufferSize = 0;


/* ========================================================================
    static関数
   ======================================================================== */

static NNS_MCS_INLINE BOOL
IsInitialize(void)
{
    return NULL != spBuffer;
}

static void
CallbackRecvDummy(
    const void* /*pRecv*/,
    u32         /*dwRecvSize*/,
    u32         /*dwUserData*/,
    u32         /*offset*/,
    u32         /*totalSize*/
)
{
    // 何もしない
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
        OS_Printf("NNS Mcs Print: not sever connect\n");
        return FALSE;
    }
}

/*
    データを送信
        成功するまで数回リトライ
  */
static BOOL
WriteStream(
    const void* buf,
    u32         length
)
{
    int retryCount;

    for (retryCount = 0; retryCount < SEND_RETRY_MAX; ++retryCount)
    {
        u32 writableLength;
        if (! NNS_McsGetStreamWritableLength(&writableLength))
        {
            return FALSE;
        }

        if (length <= writableLength)
        {
            return NNS_McsWriteStream(NNSi_MCS_PRINT_CHANNEL, buf, length);
        }
        else
        {
            // OS_Printf("NNS Mcs Print: buffer short - retry\n");
            NNSi_McsSleep(16);
        }
    }

    OS_Printf("NNS Mcs Print: send time out\n");
    return FALSE;
}


/* ========================================================================
    外部関数
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:         NNS_McsInitPrint

  Description:  File I/O APIの初期化。

  Arguments:    pBuffer:  出力用のワークバッファへのポインタ。
                buffer:   バッファサイズ(バイト)。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void
NNS_McsInitPrint(
    void*   pBuffer,
    u32     size
)
{
    NNS_ASSERT(pBuffer != NULL);
    NNS_ASSERT(size > 1);

    if (IsInitialize())
    {
        return;
    }

    spBuffer    = pBuffer;      // 初期化完了も意味する
    sBufferSize = size;

    NNS_McsRegisterRecvCallback(&sCBInfo, NNSi_MCS_PRINT_CHANNEL, CallbackRecvDummy, 0);
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsPutString

  Description:  mcsサーバのコンソールに文字列を出力する。

  Arguments:    string:  出力する文字列。

  Returns:      関数が成功したら TRUE、失敗したら FALSE を返す。
 *---------------------------------------------------------------------------*/
BOOL
NNS_McsPutString(
    const char* string
)
{
    NNS_ASSERT(IsInitialize());
    NNS_ASSERT(string);

    if (! CheckConnect())
    {
        return FALSE;
    }

    return WriteStream(string, strlen(string));
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsPrintf

  Description:  mcsサーバのコンソールに書式付きで出力する。

  Arguments:    format:  書式指定文字列。
                ...   :  省略可能な引数。

  Returns:      関数が成功したら TRUE、失敗したら FALSE を返す。
 *---------------------------------------------------------------------------*/
BOOL
NNS_McsPrintf(
    const char* format,
    ...
)
{
    int writeChars;
    va_list args;

    NNS_ASSERT(IsInitialize());
    NNS_ASSERT(format);

    if (! CheckConnect())
    {
        return FALSE;
    }

    va_start(args, format);

    writeChars = vsnprintf(spBuffer, sBufferSize, format, args);

    va_end(vlist);

    // vsnprintfでは、バッファに収まりきらない場合、バッファに入る分だけセットされる。
    // 末尾に0もセットされている。
    if (writeChars > 1)
    {
        return WriteStream(spBuffer, strlen(spBuffer));
    }

    return FALSE;
}

/* #if ! defined(NNS_FINALROM) */
#endif

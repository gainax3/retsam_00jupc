/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g3d
  File:     gecom.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.25 $
 *---------------------------------------------------------------------------*/

//
// AUTHOR: Kenji Nishida
//

#include <nnsys/g3d/gecom.h>

//
// NOTICE:
// ONLY FOR SINGLE THREADED CODE
//


////////////////////////////////////////////////////////////////////////////////
//
// スタティック変数
//

/*---------------------------------------------------------------------------*
    NNS_G3dFlagGXDmaAsync

    G3Dで使用されるMI_SendGXCommandAsyncが使用するフラグ
    0以外の場合にはDmaの転送が完了していない。
 *---------------------------------------------------------------------------*/
static volatile int NNS_G3dFlagGXDmaAsync  = 0;

//
// NOTICE:
// Ge関連APIはBufferがNULLでも正常動作しなくてはならない
//

/*---------------------------------------------------------------------------*
    NNS_G3dGeBuffer

    G3Dが使用するジオメトリコマンドバッファへのポインタ。
    通常スタック(DTCM)領域のメモリが割り当てられる。

    ディスプレイリストのＤＭＡ転送にジオメトリコマンドの送信要求があった場合、
    このバッファにコマンドを書き出すことにより、ＣＰＵの処理を次に進める
    ことができる。大きなディスプレイリストを持つオブジェクトに有効。
 *---------------------------------------------------------------------------*/
static NNSG3dGeBuffer* NNS_G3dGeBuffer = NULL;


////////////////////////////////////////////////////////////////////////////////
//
// APIs
//


/*---------------------------------------------------------------------------*
    NNS_G3dGeIsSendDLBusy

    G3Dで行うジオメトリコマンドのジオメトリエンジンへのＤＭＡ転送が
    転送中かどうかを返します。
 *---------------------------------------------------------------------------*/
BOOL
NNS_G3dGeIsSendDLBusy(void)
{
    return NNS_G3dFlagGXDmaAsync;
}


/*---------------------------------------------------------------------------*
    NNS_G3dGeIsBufferExist

    ジオメトリコマンドバッファが存在するかどうかを返します。
 *---------------------------------------------------------------------------*/
BOOL
NNS_G3dGeIsBufferExist(void)
{
    return (NNS_G3dGeBuffer != NULL);
}


/*---------------------------------------------------------------------------*
    NNS_G3dGeSetBuffer

    NNS_G3dGeBufferにまだジオメトリコマンドバッファがセットされていない場合、
    pをジオメトリコマンドバッファとしてセットします。
 *---------------------------------------------------------------------------*/
void
NNS_G3dGeSetBuffer(NNSG3dGeBuffer* p)
{
    NNS_G3D_NULL_ASSERT(p);

    if (NNS_G3dGeBuffer == NULL)
    {
        p->idx = 0;
        NNS_G3dGeBuffer = p;
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dGeReleaseBuffer

    ジオメトリコマンドバッファを安全に取り除きます。
    返り値は取り除かれたジオメトリコマンドバッファへのポインタです
 *---------------------------------------------------------------------------*/
NNSG3dGeBuffer*
NNS_G3dGeReleaseBuffer(void)
{
    NNSG3dGeBuffer* p;

    NNS_G3dGeFlushBuffer();

    p = NNS_G3dGeBuffer;
    NNS_G3dGeBuffer = NULL;
    return p;
}


static NNS_G3D_INLINE void
sendNB(const void* src, void* dst, u32 szByte)
{
    MI_CpuSend32(src, dst, szByte);
}


/*---------------------------------------------------------------------------*
    NNS_G3dGeFlushBuffer

    ジオメトリコマンドがDMA転送中であれば終了時までウェイトし、
    ジオメトリコマンドバッファがあれば、バッファの内容をジオメトリエンジンに
    書き出します。
    この関数の終了後は、ジオメトリコマンドを直接ジオメトリエンジンに送信することが
    できます(安全にSDKのG3_XXX関数を使えるようになる)。
 *---------------------------------------------------------------------------*/
void
NNS_G3dGeFlushBuffer(void)
{
    if (NNS_G3dFlagGXDmaAsync)
    {
        NNS_G3dGeWaitSendDL();
    }

    if (NNS_G3dGeBuffer &&
        NNS_G3dGeBuffer->idx > 0)
    {
        sendNB(&NNS_G3dGeBuffer->data[0],
               (void*)&reg_G3X_GXFIFO,
               NNS_G3dGeBuffer->idx << 2);
        NNS_G3dGeBuffer->idx = 0;
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dGeWaitSendDL

    ジオメトリコマンドのＤＭＡ転送(NNS_G3dGeSendDLを使用のこと)が完了するまで
    ウェイトします。
 *---------------------------------------------------------------------------*/
void
NNS_G3dGeWaitSendDL(void)
{
    while(NNS_G3dFlagGXDmaAsync)
        ;
}


/*---------------------------------------------------------------------------*
    NNS_G3dGeIsImmOK

    コマンドを直接FIFOに送信(g3imm.h)してよいかどうかを返す。
    送信してもよいのなら、TRUEを返す。
    条件はコマンドバッファが存在しないか空で、且つGXDMA転送中でないときです。
 *---------------------------------------------------------------------------*/
BOOL
NNS_G3dGeIsImmOK(void)
{
    return (NNS_G3dGeBuffer == NULL || NNS_G3dGeBuffer->idx == 0) &&
           !NNS_G3dGeIsSendDLBusy();
}


/*---------------------------------------------------------------------------*
    NNS_G3dGeIsBufferOK

    numWord個のデータをジオメトリコマンドバッファに追加可能かどうかを返す。
    可能ならばTRUEを返す。
 *---------------------------------------------------------------------------*/
BOOL
NNS_G3dGeIsBufferOK(u32 numWord)
{
    return (NNS_G3dGeBuffer != NULL) &&
           (NNS_G3dGeBuffer->idx + numWord <= NNS_G3D_SIZE_COMBUFFER);
}


//
// NNS_G3dGeSendDL専用
//
static void
simpleUnlock_(void* arg)
{
    *((volatile int*)arg) = 0;
}

#ifdef NNS_G3D_USE_FASTGXDMA
static BOOL NNS_G3dFlagUseFastDma = TRUE;
#else
static BOOL NNS_G3dFlagUseFastDma = FALSE;
#endif


/*---------------------------------------------------------------------------*
    NNS_G3dGeUseFastDma

    引数にFALSE以外を指定すると、ジオメトリコマンドのDMA転送に
    MI_SendGXCommandAsyncFast関数を使用します。その場合は、
    プログラミングマニュアル第7章の「ARM9システムバスにおける
    DMA複数チャネル並列起動時に関する注意事項」を守ってお使い下さい。
 *---------------------------------------------------------------------------*/
void
NNS_G3dGeUseFastDma(BOOL cond)
{
    NNS_G3dFlagUseFastDma = (cond);
}


/*---------------------------------------------------------------------------*
    NNS_G3dGeSendDL

    ディスプレイリストを安全にジオメトリエンジンに書き出します。
 *---------------------------------------------------------------------------*/
void
NNS_G3dGeSendDL(const void* src, u32 szByte)
{
    NNS_G3D_NULL_ASSERT(src);
    NNS_G3D_ASSERT(szByte >= 4);

    // 転送開始
    if (szByte < 256 || GX_DMAID == GX_DMA_NOT_USE)
    {
        NNS_G3dGeBufferOP_N(*(const u32*)src,
                            (const u32*)src + 1,
                            (szByte >> 2) - 1);
    }
    else
    {
        // コマンドバッファを先にフラッシュしておく
        NNS_G3dGeFlushBuffer();
        NNS_G3dFlagGXDmaAsync = 1;

        if (NNS_G3dFlagUseFastDma)
        {
            MI_SendGXCommandAsyncFast(GX_DMAID,
                                      src,
                                      szByte,
                                      &simpleUnlock_,
                                      (void*)&NNS_G3dFlagGXDmaAsync);
        }
        else
        {
            MI_SendGXCommandAsync(GX_DMAID,
                                  src,
                                  szByte,
                                  &simpleUnlock_,
                                  (void*)&NNS_G3dFlagGXDmaAsync);
        }
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dGeBufferOP_N

    引数をN個とるジオメトリコマンドを送信する。
 *---------------------------------------------------------------------------*/
void
NNS_G3dGeBufferOP_N(u32 op, const u32* args, u32 num)
{
    if (NNS_G3dGeBuffer)
    {
        if (NNS_G3dFlagGXDmaAsync)
        {
            // バッファの空き容量は十分にあるか？
            if (NNS_G3dGeBuffer->idx + 1 + num <= NNS_G3D_SIZE_COMBUFFER)
            {
                NNS_G3dGeBuffer->data[NNS_G3dGeBuffer->idx++] = op;
                if (num > 0)
                {
                    MI_CpuCopyFast(args,
                                   &NNS_G3dGeBuffer->data[NNS_G3dGeBuffer->idx],
                                   num << 2);

                    NNS_G3dGeBuffer->idx += num;
                }
                // バッファリング完了
                return;
            }
        }

        // 結局バッファに追加されていない
        // バッファを空にしてからイミディエイトで書き込む
        if (NNS_G3dGeBuffer->idx != 0)
        {
            NNS_G3dGeFlushBuffer();
        }
        else
        {
            if (NNS_G3dFlagGXDmaAsync)
            {
                NNS_G3dGeWaitSendDL();
            }
        }
    }
    else
    {
        if (NNS_G3dFlagGXDmaAsync)
        {
            NNS_G3dGeWaitSendDL();
        }
    }

    reg_G3X_GXFIFO = op;
    sendNB(args, (void*)&reg_G3X_GXFIFO, num << 2);
}

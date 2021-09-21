/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - mcs
  File:     ringBuffer.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.10 $
 *---------------------------------------------------------------------------*/

#if ! defined(_WIN32)

    #include <nitro.h>

#endif

#include <nnsys/mcs/ringBuffer.h>
#include <nnsys/mcs/config.h>

/* ========================================================================
    コンパイルスイッチ
   ======================================================================== */

#define USE_DISABLE_IRQ 0


/* ========================================================================
    マクロ関数
   ======================================================================== */

#if USE_DISABLE_IRQ
    #define DISABLE_IRQ_BLOCK(s)                    \
        do                                          \
        {                                           \
            const BOOL preIRQ = OS_DisableIrq();    \
            (s) ;                                   \
            (void)OS_RestoreIrq(preIRQ);            \
        }while(FALSE)
#else
    #define DISABLE_IRQ_BLOCK(s)    (s)
#endif

#if defined(_WIN32)
    #define GetVarFromRBMem(app, rb)                    NNSi_McsReadFromRingBufferMem(&(app), (uint32_t)&(rb), sizeof(app))
    #define SetVarToRBMem(rb, app)                      NNSi_McsWriteToRingBufferMem((uint32_t)&(rb), &(app), sizeof(rb))

    #define ReadFromRBMem(appAddr, rbAddr, bytes)       NNSi_McsReadFromRingBufferMem(appAddr, rbAddr, bytes)
    #define WriteToRBMem(rbAddr, appAddr, bytes)        NNSi_McsWriteToRingBufferMem(rbAddr, appAddr, bytes)
#else
    #define GetVarFromRBMem(app, rb)                    ((app) = (rb))
    #define SetVarToRBMem(rb, app)                      ((rb) = (app))

    #define ReadFromRBMem(appAddr, rbAddr, bytes)       MI_CpuCopy8((const void*)rbAddr, appAddr, bytes)
    #define WriteToRBMem(rbAddr, appAddr, bytes)        MI_CpuCopy8(appAddr, (void*)rbAddr, bytes)
#endif

#define RefU32(p)   (*(uint32_t*)(p))
#define RefCU32(p)  (*(const uint32_t*)(p))


/* ========================================================================
    static関数
   ======================================================================== */

static NNS_MCS_INLINE uint32_t
RoundUp(
    uint32_t    value,
    uint32_t    alignment
)
{
    return (value + alignment -1) & ~(alignment -1);
}

static NNS_MCS_INLINE uint32_t
RoundDown(
    uint32_t    value,
    uint32_t    alignment
)
{
    return value & ~(alignment -1);
}

static NNS_MCS_INLINE uint32_t
GetMin(
    uint32_t    a,
    uint32_t    b
)
{
    return a < b ? a: b;
}

static NNS_MCS_INLINE NNSiMcsUIntPtr
GetRingBufferBufferEnd(const NNSiMcsBufRgn* pBRgn)
{
    return pBRgn->buf + pBRgn->bufSize;
}

static NNS_MCS_INLINE NNSiMcsUIntPtr
GetRingBufferPtrInc(
    NNSiMcsUIntPtr          ptr,
    uint32_t                val,
    const NNSiMcsBufRgn*    pBRgn
)
{
    ptr += val;

    if (ptr >= GetRingBufferBufferEnd(pBRgn))
    {
        ptr -= pBRgn->bufSize;
    }

    return ptr;
}

static NNS_MCS_INLINE NNSiMcsUIntPtr
GetRingBufferPtrDec(
    NNSiMcsUIntPtr          ptr,
    uint32_t                val,
    const NNSiMcsBufRgn*    pBRgn
)
{
    ptr -= val;

    if (ptr < pBRgn->buf)
    {
        ptr += pBRgn->bufSize;
    }

    return ptr;
}

static uint32_t
GetRingBufferReadableBytes(const NNSiMcsMsgRange* pMRng)
{
    if(pMRng->start == pMRng->end)
    {
        return 0;
    }

    {
        uint32_t size;
        GetVarFromRBMem(size, RefCU32(pMRng->start));
        return size;
    }
}

static uint32_t
GetRingBufferWritableBytes(
    const NNSiMcsMsgRange*  pMRng,
    uint32_t                bufSize
)
{
    uint32_t writableBytes = 0;

    if (pMRng->start > pMRng->end)
    {
        writableBytes = pMRng->start - pMRng->end;
    }
    else
    {
        writableBytes = bufSize - (pMRng->end - pMRng->start);
    }

    writableBytes -= sizeof(uint32_t);

    if (writableBytes > 0)
    {
        writableBytes -= sizeof(uint32_t);
    }

    return writableBytes;
}

static void
GetRingBufferHeader(
    NNSiMcsBufRgn*      pBrgn,
    NNSiMcsMsgRange*    pMrng,
    NNSMcsRingBuffer    rb
)
{
    const NNSiMcsRingBufferHeader* pRbh = (NNSiMcsRingBufferHeader*)rb; // ターゲットのアドレス

    // ローカルにコピー
    DISABLE_IRQ_BLOCK(
        (
            GetVarFromRBMem(pBrgn->buf    , pRbh->brgn.buf    ),
            GetVarFromRBMem(pBrgn->bufSize, pRbh->brgn.bufSize),
            GetVarFromRBMem(pMrng->end    , pRbh->mrng.end    ),

            GetVarFromRBMem(pMrng->start  , pRbh->mrng.start  )
        )
    );
}

static NNSiMcsUIntPtr
WriteBuffer(
    NNSiMcsUIntPtr          pDst,
    const uint8_t*          pSrc,
    uint32_t                size,
    const NNSiMcsBufRgn*    pBrgn
)
{
    while (size > 0)
    {
        const uint32_t writeByte = GetMin(GetRingBufferBufferEnd(pBrgn) - pDst, size);
        WriteToRBMem(pDst, pSrc, writeByte);
        pDst = GetRingBufferPtrInc(pDst, RoundUp(writeByte, 4), pBrgn);
        pSrc += writeByte;
        size -= writeByte;
    }

    return pDst;
}

static void
SetState(
    NNSMcsRingBuffer    rb,
    uint32_t            addState
)
{
    NNSiMcsRingBufferHeader* pRbh = (NNSiMcsRingBufferHeader*)rb; // ターゲットのアドレス
    uint32_t state;

    DISABLE_IRQ_BLOCK(
        (
            GetVarFromRBMem(state      , pRbh->state),
            state |= addState,
            SetVarToRBMem  (pRbh->state, state      )
        )
    );
}


/* ========================================================================
    外部関数
   ======================================================================== */

NNSMcsRingBuffer
NNS_McsInitRingBuffer(
    NNSiMcsBufPtr   buf,
    uint32_t        bufSize
)
{
    const uint32_t signature = NNS_MCS_RINGBUF_SIGNATURE;
    const uint32_t state = 0;
    NNSiMcsRingBufferHeader* pRbh = (NNSiMcsRingBufferHeader*)buf;
    NNSiMcsUIntPtr startBuf = (NNSiMcsUIntPtr)buf + sizeof(NNSiMcsRingBufferHeader);
    bufSize -= sizeof(NNSiMcsRingBufferHeader);

    SetVarToRBMem(pRbh->state       , state);
    SetVarToRBMem(pRbh->brgn.buf    , startBuf);
    SetVarToRBMem(pRbh->brgn.bufSize, bufSize);
    SetVarToRBMem(pRbh->mrng.start  , startBuf);
    SetVarToRBMem(pRbh->mrng.end    , startBuf);

    SetVarToRBMem(pRbh->signature   , signature);   // 最後にsigunatureをセット(初期化完了)

    return (NNSMcsRingBuffer)pRbh;
}

uint32_t
NNS_McsGetRingBufferReadableBytes(NNSMcsRingBuffer rb)
{
    NNSiMcsRingBufferHeader* pRbh = (NNSiMcsRingBufferHeader*)rb;
    NNSiMcsMsgRange mrng;

    // ローカルにコピー
    DISABLE_IRQ_BLOCK(
        (
            GetVarFromRBMem(mrng.start, pRbh->mrng.start),

            GetVarFromRBMem(mrng.end  , pRbh->mrng.end  )
        )
    );

    return GetRingBufferReadableBytes(&mrng);
}

uint32_t
NNS_McsGetRingBufferTotalReadableBytes(NNSMcsRingBuffer rb)
{
    NNSiMcsRingBufferHeader* pRgh = (NNSiMcsRingBufferHeader*)rb;
    NNSiMcsBufRgn brgn;
    NNSiMcsMsgRange mrng;

    // ローカルにコピー
    DISABLE_IRQ_BLOCK(
        (
            GetVarFromRBMem(brgn.buf    , pRgh->brgn.buf    ),
            GetVarFromRBMem(brgn.bufSize, pRgh->brgn.bufSize),
            GetVarFromRBMem(mrng.start  , pRgh->mrng.start  ),

            GetVarFromRBMem(mrng.end    , pRgh->mrng.end    )
        )
    );

    if (mrng.start == mrng.end)
    {
        return 0;
    }

    {
        NNSiMcsUIntPtr msgPtr = mrng.start;
        uint32_t total = 0;

        while (msgPtr != mrng.end)
        {
            uint32_t size;
            GetVarFromRBMem(size, RefCU32(msgPtr));
            total += size;
            msgPtr = GetRingBufferPtrInc(msgPtr, sizeof(uint32_t) + RoundUp(size, 4), &brgn);
        }

        return total;
    }
}

uint32_t
NNS_McsGetRingBufferWritableBytes(NNSMcsRingBuffer rb)
{
    NNSiMcsRingBufferHeader* pRbh = (NNSiMcsRingBufferHeader*)rb; // ターゲットのアドレス
    NNSiMcsMsgRange mrng;
    uint32_t bufSize;

    // ローカルにコピー
    DISABLE_IRQ_BLOCK(
        (
            GetVarFromRBMem(bufSize   , pRbh->brgn.bufSize),
            GetVarFromRBMem(mrng.end  , pRbh->mrng.end  ),

            GetVarFromRBMem(mrng.start, pRbh->mrng.start)
        )
    );

    return GetRingBufferWritableBytes(&mrng, bufSize);
}

BOOL
NNS_McsReadRingBuffer(
    NNSMcsRingBuffer    rb,
    void*               buf,
    uint32_t                 size,
    uint32_t*                pReadBytes
)
{
    NNSiMcsRingBufferHeader* pRbh = (NNSiMcsRingBufferHeader*)rb; // ターゲットのアドレス
    NNSiMcsBufRgn brgn;
    NNSiMcsMsgRange mrng;
    uint32_t readableBytes;

    // ローカルにコピー
    DISABLE_IRQ_BLOCK(
        (
            GetVarFromRBMem(brgn.buf    , pRbh->brgn.buf    ),
            GetVarFromRBMem(brgn.bufSize, pRbh->brgn.bufSize),
            GetVarFromRBMem(mrng.start  , pRbh->mrng.start),

            GetVarFromRBMem(mrng.end    , pRbh->mrng.end  )
        )
    );

    readableBytes = GetRingBufferReadableBytes(&mrng);
    if (readableBytes == 0)
    {
        *pReadBytes = 0;
        return TRUE;
    }

    {
        BOOL bRet = TRUE;

        uint8_t* pDst = (uint8_t*)buf;
        NNSiMcsUIntPtr pSrc = GetRingBufferPtrInc(mrng.start, sizeof(uint32_t), &brgn);
        uint32_t restSize;

        if (readableBytes > size)
        {
            // サイズヘッダを置く必要上、全部読み出さないときは4バイトアライメントする
            restSize = RoundDown(size, 4);
        }
        else
        {
            restSize = readableBytes;
        }
        *pReadBytes = restSize;

        while (restSize > 0)
        {
            const uint32_t readByte = GetMin(GetRingBufferBufferEnd(&brgn) - pSrc, restSize);
            ReadFromRBMem(pDst, pSrc, readByte);
            pSrc = GetRingBufferPtrInc(pSrc, RoundUp(readByte, 4), &brgn);
            pDst += readByte;
            restSize -= readByte;
        }

        if (*pReadBytes != readableBytes)
        {
            const uint32_t newSize = readableBytes - *pReadBytes;
            pSrc = GetRingBufferPtrDec(pSrc, sizeof(uint32_t), &brgn);   // uint32_t分戻る
            SetVarToRBMem(RefU32(pSrc), newSize);                   // 残りのサイズをセット

            bRet = FALSE;
        }

        DISABLE_IRQ_BLOCK( SetVarToRBMem(pRbh->mrng.start, pSrc) );

        return bRet;
    }
}

/*
    end ポイントが、buf + bufSize でないようにすること。このときは、bufを指すようにする。
    
    書き込むときは、end はstartと同じ値にならないようにすること。常にend は常に startに対して 4バイト以上空きを取ることになる。
*/
BOOL
NNS_McsWriteRingBuffer(
    NNSMcsRingBuffer    rb,
    const void*         buf,
    uint32_t            size
)
{
    NNSiMcsBufRgn brgn;
    NNSiMcsMsgRange mrng;

    // ローカルにコピー
    GetRingBufferHeader(&brgn, &mrng, rb);

    if (size > GetRingBufferWritableBytes(&mrng, brgn.bufSize))
    {
        SetState(rb, NNS_MCS_RINGBUF_OVERFLOW);
        return FALSE;
    }

    SetVarToRBMem(RefU32(mrng.end), size);

    {
        NNSiMcsRingBufferHeader* pRbh = (NNSiMcsRingBufferHeader*)rb; // ターゲットのアドレス
        NNSiMcsUIntPtr pDst = GetRingBufferPtrInc(mrng.end, sizeof(uint32_t), &brgn);
        pDst = WriteBuffer(pDst, (const uint8_t*)buf, size, &brgn);
        DISABLE_IRQ_BLOCK( SetVarToRBMem(pRbh->mrng.end, pDst) );
    }

    return TRUE;
}

BOOL
NNS_McsWriteRingBufferEx(
    NNSMcsRingBuffer    rb,
    const void*         buf,
    uint32_t            offset,
    uint32_t            size,
    uint32_t            totalSize
)
{
    NNSiMcsBufRgn brgn;
    NNSiMcsMsgRange mrng;

    // ローカルにコピー
    GetRingBufferHeader(&brgn, &mrng, rb);

    if (offset + size > GetRingBufferWritableBytes(&mrng, brgn.bufSize))
    {
        SetState(rb, NNS_MCS_RINGBUF_OVERFLOW);
        return FALSE;
    }

    SetVarToRBMem(RefU32(mrng.end), totalSize);

    {
        NNSiMcsUIntPtr pDst = GetRingBufferPtrInc(mrng.end, sizeof(uint32_t) + offset, &brgn);
        pDst = WriteBuffer(pDst, (const uint8_t*)buf, size, &brgn);
        if (offset + size == totalSize)
        {
            NNSiMcsRingBufferHeader* pRbh = (NNSiMcsRingBufferHeader*)rb; // ターゲットのアドレス
            DISABLE_IRQ_BLOCK( SetVarToRBMem(pRbh->mrng.end, pDst) );
        }
    }

    return TRUE;
}

void
NNS_McsClearRingBuffer(NNSMcsRingBuffer rb)
{
    NNSiMcsRingBufferHeader* pRbh = (NNSiMcsRingBufferHeader*)rb;
    NNSiMcsUIntPtr endPtr;
    const uint32_t state = 0;

    // endポインタの値をstartポインタにセットし、ステートをクリア
    DISABLE_IRQ_BLOCK(
        (
            GetVarFromRBMem(endPtr          , pRbh->mrng.end),
            SetVarToRBMem  (pRbh->mrng.start, endPtr        ),
            SetVarToRBMem  (pRbh->state     , state         )
        )
    );
}

BOOL
NNS_McsCheckRingBuffer(NNSMcsRingBuffer rb)
{
    NNSiMcsRingBufferHeader* pRbh = (NNSiMcsRingBufferHeader*)rb; // ターゲットのアドレス
    uint32_t signature;

    DISABLE_IRQ_BLOCK( GetVarFromRBMem(signature, pRbh->signature) );

    return signature == NNS_MCS_RINGBUF_SIGNATURE;
}

uint32_t
NNS_McsGetRingBufferState(NNSMcsRingBuffer rb)
{
    NNSiMcsRingBufferHeader* pRbh = (NNSiMcsRingBufferHeader*)rb; // ターゲットのアドレス
    uint32_t state;

    DISABLE_IRQ_BLOCK( GetVarFromRBMem(state, pRbh->state) );

    return state;
}

void
NNS_McsClearRingBufferState(NNSMcsRingBuffer rb)
{
    NNSiMcsRingBufferHeader* pRbh = (NNSiMcsRingBufferHeader*)rb; // ターゲットのアドレス
    const uint32_t state = 0;

    DISABLE_IRQ_BLOCK( SetVarToRBMem(pRbh->state, state) );
}


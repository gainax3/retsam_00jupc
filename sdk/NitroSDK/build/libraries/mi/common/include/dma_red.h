/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MI - 
  File:     dma_red.h

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dma_red.h,v $
  Revision 1.4  2005/03/01 01:57:00  yosizaki
  copyright の年を修正.

  Revision 1.3  2005/02/28 05:26:05  yosizaki
  do-indent.

  Revision 1.2  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.1  2003/12/18 07:20:07  yada
  red-sdk の定義を残すため


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_MI_DMA_RED_H_
#define NITRO_MI_DMA_RED_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/misc.h>
#include <nitro/types.h>
#include <nitro/memorymap.h>
#include <nitro/os/systemCall.h>
#include <nitro/mi/dma.h>

//----------------------------------------------------------------
//      開発技術作成したインタフェース
//      要望により残しています。
//      nitro-sdk/include/nitro/mi/dma.h に入れずにここに。
//

//----------------------------------------------------------------------
//                      ＤＭＡ セット
//----------------------------------------------------------------------
//#define SIMULATOR
#ifndef SIMULATOR
#define __MI_DmaSet(dmaNo, srcp, destp, dmaCntData)                       \
{                                                                    \
    vu32 *dmaCntp = &((vu32 *)REG_DMA0SAD_ADDR)[dmaNo * 3];                  \
    dmaCntp[0] = (vu32 )(srcp);                                      \
    dmaCntp[1] = (vu32 )(destp);                                     \
    dmaCntp[2] = (vu32 )(dmaCntData);                                \
    {u32 dummy = dmaCntp[2];}                                        \
    {u32 dummy = dmaCntp[2];}                                        \
}
#define __MI_DmaSetAsync(dmaNo, srcp, destp, dmaCntData)                  \
{                                                                    \
    vu32 *dmaCntp = &((vu32 *)REG_DMA0SAD_ADDR)[dmaNo * 3];                  \
    dmaCntp[0] = (vu32 )(srcp);                                      \
    dmaCntp[1] = (vu32 )(destp);                                     \
    dmaCntp[2] = (vu32 )(dmaCntData);                                \
}
#else
#define __MI_DmaSet(dmaNo, srcp, destp, dmaCntData)                       \
{                                                                    \
    int    i;                                                        \
    for (i=0; i<(dmaCntData & 0x1ffff); i++)                         \
        if ((dmaCntData) & MI_DMA_SRC_FIX) {                            \
            if ((dmaCntData) & MI_DMA_32BIT_BUS)                        \
                    ((vu32 *)(destp))[i] = ((vu32 *)(srcp))[0];      \
            else    ((vu16 *)(destp))[i] = ((vu16 *)(srcp))[0];      \
         } else {                                                    \
            if ((dmaCntData) & MI_DMA_32BIT_BUS)                        \
                ((vu32 *)(destp))[i] = ((vu32 *)(srcp))[i];          \
            else    ((vu16 *)(destp))[i] = ((vu16 *)(srcp))[i];      \
        }                                                            \
}
#define __MI_DmaSetAsync(dmaNo, srcp, destp, dmaCntData)                  \
        __MI_DmaSet(     dmaNo, srcp, destp, dmaCntData)
#endif

//・DMAコントローラにパラメータをセットします。
//・SIMULATERを定義するとCPUでシミュレートします。
//  GDBでデバッグする場合などに有効です。
//・最後にDMA起動待ちのために"LDR"命令が挿入されます。
//
//・引数：
//  dmaNo       DMA番号
//  srcp        ソースアドレス
//  destp       デスティネーションアドレス
//  dmaCntData  パラメータデータ
//
//※CPU内部RAM上のプログラムにてDMAを起動すると
//  その次の命令が先に実行されます。
//  よって、DMA直後に転送先をCPUにて変更しようとした場合、
//  読み込み／書き戻しの間にDMAが起動されてしまいますので、
//  DMAの転送先のデータが意図していない値になる場合があります。
//  その場合にはWaitDma()を直後に挿入して、DMAが終了したか
//  どうかをチェックすると続くコードへの影響を確実に回避できます。


//----------------------------------------------------------------------
//                      ＤＭＡ クリア
//----------------------------------------------------------------------

#define __MI_DmaClear(dmaNo, data, destp, size, bit)             \
{                                                           \
    *(vu##bit *)HW_DMA_CLEAR_DATA_BUF = (vu##bit )(data);           \
    __MI_DmaSet(dmaNo, HW_DMA_CLEAR_DATA_BUF, destp, (                  \
        MI_DMA_ENABLE         | MI_DMA_TIMING_IMM |              \
        MI_DMA_SRC_FIX        | MI_DMA_DEST_INC    |              \
        MI_DMA_##bit##BIT_BUS | ((size)/(bit/8))));            \
}

#define __MI_DmaClearIf(dmaNo, data, destp, size, bit)           \
{                                                           \
    *(vu##bit *)DMA_CLEAR_DATA_BUF = (vu##bit )(data);       \
    __MI_DmaSet(dmaNo, DMA_CLEAR_DATA_BUF, destp, (                \
        MI_DMA_ENABLE         | MI_DMA_TIMING_IMM |              \
        MI_DMA_IF_ENABLE      |                                \
        MI_DMA_SRC_FIX        | MI_DMA_DEST_INC    |              \
        MI_DMA_##bit##BIT_BUS | ((size)/(bit/8))));            \
}

#define __MI_DmaClearArray(  dmaNo, data, destp, bit)            \
    __MI_DmaClear(       dmaNo, data, destp, sizeof(destp), bit)

#define MI_DmaClearArrayIf(dmaNo, data, destp, bit)            \
    __MI_DmaClearIf(     dmaNo, data, destp, sizeof(destp), bit)

//・DMAでRAMクリアします。
//・クリアデータはスタックに置かれ、それをデスティネーションへコピーします。
//・MI_DmaClearIf／MI_DmaClearArrayIfは終了時に割り込み要求を発生します。
//・MI_DmaClearArray／MI_DmaClearArrayIfはデスティネーション配列全体をクリアします。
//
//・引数：
//  dmaNo       DMA番号
//  data        クリアデータ
//  destp       デスティネーションアドレス
//  size        クリアバイト数
//  bit         転送ビット幅（16|32）
//
//
//※CPU内部RAM上のプログラムにてDMAを起動すると
//  その次の命令が先に実行されます。
//  よって、DMA直後に転送先をCPUにて変更しようとした場合、
//  読み込み／書き戻しの間にDMAが起動されてしまいますので、
//  DMAの転送先のデータが意図していない値になる場合があります。
//  その場合にはWaitDma()を直後に挿入して、DMAが終了したか
//  どうかをチェックすると続くコードへの影響を確実に回避できます。

//----------------------------------------------------------------------
//                      ＤＭＡ コピー
//----------------------------------------------------------------------

#define __MI_DmaCopy(dmaNo, srcp, destp, size, bit)              \
                                                            \
    __MI_DmaSet(dmaNo, srcp, destp,  (                           \
        MI_DMA_ENABLE         | MI_DMA_TIMING_IMM |              \
        MI_DMA_SRC_INC        | MI_DMA_DEST_INC    |              \
        MI_DMA_##bit##BIT_BUS | ((size)/((bit)/8))))

#define __MI_DmaCopyIf(dmaNo, srcp, destp, size, bit)            \
                                                            \
    __MI_DmaSet(dmaNo, srcp, destp,  (                           \
        MI_DMA_ENABLE         | MI_DMA_TIMING_IMM |              \
        MI_DMA_IF_ENABLE      |                                \
        MI_DMA_SRC_INC        | MI_DMA_DEST_INC    |              \
        MI_DMA_##bit##BIT_BUS | ((size)/(bit/8))))

#define __MI_DmaCopyArray(  dmaNo, srcp, destp, bit)             \
    __MI_DmaCopy(       dmaNo, srcp, destp, sizeof(srcp), bit)

#define __MI_DmaCopyArrayIf(dmaNo, srcp, destp, bit)             \
    __MI_DmaCopyIf(     dmaNo, srcp, destp, sizeof(srcp), bit)

//・DMAでコピーします。
//・MI_DmaCopyIf／MI_DmaCopyArrayIfは終了時に割り込み要求を発生します。
//・MI_DmaCopyArray／MI_DmaCopyArrayIfはソース配列全体をコピーします。
//
//・引数：
//  dmaNo       DMA番号
//  srcp        ソースアドレス
//  destp       デスティネーションアドレス
//  size        転送バイト数
//  bit         転送ビット幅（16|32）
//
//
//※CPU内部RAM上のプログラムにてDMAを起動すると
//  その次の命令が先に実行されます。
//  よって、DMA直後に転送先をCPUにて変更しようとした場合、
//  読み込み／書き戻しの間にDMAが起動されてしまいますので、
//  DMAの転送先のデータが意図していない値になる場合があります。
//  その場合にはWaitDma()を直後に挿入して、DMAが終了したか
//  どうかをチェックすると続くコードへの影響を確実に回避できます。

//----------------------------------------------------------------------
//                  ＨブランクＤＭＡ コピー
//----------------------------------------------------------------------

#define __MI_H_DmaCopy(dmaNo, srcp, destp, size, bit)            \
                                                            \
    __MI_DmaSet(dmaNo, srcp, destp,  (                           \
        MI_DMA_ENABLE         | MI_DMA_TIMING_H_BLANK |          \
        MI_DMA_SRC_INC        | MI_DMA_DEST_RELOAD     |          \
        MI_DMA_CONTINUOUS_ON  |                                \
        MI_DMA_##bit##BIT_BUS | ((size)/((bit)/8))))

#define __MI_H_DmaCopyIf(dmaNo, srcp, destp, size, bit)          \
                                                            \
    __MI_DmaSet(dmaNo, srcp, destp,  (                           \
        MI_DMA_ENABLE         | MI_DMA_TIMING_H_BLANK |          \
        MI_DMA_IF_ENABLE      |                                \
        MI_DMA_SRC_INC        | MI_DMA_DEST_RELOAD     |          \
        MI_DMA_CONTINUOUS_ON  |                                \
        MI_DMA_##bit##BIT_BUS | ((size)/(bit/8))))

#define __MI_H_DmaCopyArray(  dmaNo, srcp, destp, bit)           \
    __MI_H_DmaCopy(       dmaNo, srcp, destp, sizeof(srcp), bit)

#define __MI_H_DmaCopyArrayIf(dmaNo, srcp, destp, bit)           \
    __MI_H_DmaCopyIf(     dmaNo, srcp, destp, sizeof(srcp), bit)

//・Hブランクに同期してDMAでコピーします。
//・MI_H_DmaCopyIf／MI_H_DmaCopyArrayIfは終了時に割り込み要求を発生します。
//・MI_H_DmaCopyArray／MI_H_DmaCopyArrayIfはソース配列全体をコピーします。
//
//・引数：
//  dmaNo       DMA番号
//  srcp        ソースアドレス
//  destp       デスティネーションアドレス
//  size        転送バイト数
//  bit         転送ビット幅（16|32）

//----------------------------------------------------------------------
//                  ＶブランクＤＭＡ コピー
//----------------------------------------------------------------------

#define __MI_V_DmaCopy(dmaNo, srcp, destp, size, bit)            \
                                                            \
    __MI_DmaSet(dmaNo, srcp, destp,  (                           \
        MI_DMA_ENABLE         | MI_DMA_TIMING_V_BLANK |          \
        MI_DMA_SRC_INC        | MI_DMA_DEST_INC        |          \
        MI_DMA_##bit##BIT_BUS | ((size)/(bit/8))))

#define __MI_V_DmaCopyIf(dmaNo, srcp, destp, size, bit)          \
                                                            \
    __MI_DmaSet(dmaNo, srcp, destp,  (                           \
        MI_DMA_ENABLE         | MI_DMA_TIMING_V_BLANK |          \
        MI_DMA_IF_ENABLE      |                                \
        MI_DMA_SRC_INC        | MI_DMA_DEST_INC        |          \
        MI_DMA_##bit##BIT_BUS | ((size)/(bit/8))))

#define __MI_V_DmaCopyArray(  dmaNo, srcp, destp, bit)           \
    __MI_V_DmaCopy(       dmaNo, srcp, destp, sizeof(srcp), bit)

#define __MI_V_DmaCopyArrayIf(dmaNo, srcp, destp, bit)           \
    __MI_V_DmaCopyIf(     dmaNo, srcp, destp, sizeof(srcp), bit)

//・Vブランクに同期してDMAでコピーします。
//・MI_V_DmaCopyIf／MI_V_DmaCopyArrayIfは終了時に割り込み要求を発生します。
//・MI_V_DmaCopyArray／MI_V_DmaCopyArrayIfはソース配列全体をコピーします。
//
//・引数：
//  dmaNo       DMA番号
//  srcp        ソースアドレス
//  destp       デスティネーションアドレス
//  size        転送バイト数
//  bit         転送ビット幅（16|32）

//----------------------------------------------------------------------
//                  メインメモリ表示ＤＭＡ
//----------------------------------------------------------------------

#define __MI_DmaDispMainmem(dmaNo, srcp)                         \
                                                            \
    __MI_DmaSet(dmaNo, srcp, REG_DISP_MMEM_FIFO_ADDR,  (              \
        MI_DMA_ENABLE         | MI_DMA_TIMING_DISP_MMEM |        \
        MI_DMA_SRC_INC        | MI_DMA_DEST_FIX          |        \
		MI_DMA_CONTINUOUS_ON  |								\
        MI_DMA_32BIT_BUS      | (4)))

//・メインメモリ上のイメージを表示するDMA転送を行います。
//
//・引数：
//  dmaNo       DMA番号
//  srcp        ソースアドレス

//----------------------------------------------------------------------
//                  ジオメトリＦＩＦＯ－ＤＭＡ
//----------------------------------------------------------------------

#define __MI_GX_Dma(dmaNo, srcp, length)                         \
                                                            \
    __MI_DmaSetAsync(dmaNo, srcp, REG_GXFIFO_ADDR,  (                 \
        MI_DMA_ENABLE         | MI_DMA_TIMING_GXFIFO  |          \
        MI_DMA_SRC_INC        | MI_DMA_DEST_FIX        |          \
        MI_DMA_32BIT_BUS      | (length)))

#define __MI_GX_DmaIf(dmaNo, srcp, length)                       \
                                                            \
    __MI_DmaSetAsync(dmaNo, srcp, REG_GXFIFO_ADDR  (                 \
        MI_DMA_ENABLE         | MI_DMA_TIMING_GXFIFO  |          \
        MI_DMA_IF_ENABLE      |                                \
        MI_DMA_SRC_INC        | MI_DMA_DEST_FIX        |          \
        MI_DMA_32BIT_BUS      | (length)))

#define __MI_GX_DmaFast(dmaNo, srcp, length)                     \
                                                            \
    __MI_DmaSetAsync(dmaNo, srcp, REG_GXFIFO_ADDR,  (                 \
        MI_DMA_ENABLE         | MI_DMA_TIMING_IMM     |          \
        MI_DMA_SRC_INC        | MI_DMA_DEST_FIX        |          \
        MI_DMA_32BIT_BUS      | (length)))

#define __MI_GX_DmaFastIf(dmaNo, srcp, length)                   \
                                                            \
    __MI_DmaSetAsync(dmaNo, srcp, REG_GXFIFO_ADDR,  (                 \
        DMA_ENABLE         | DMA_TIMING_IMM     |          \
        DMA_IF_ENABLE      |                                \
        DMA_SRC_INC        | DMA_DEST_FIX        |          \
        DMA_32BIT_BUS      | (length)))

#define __MI_GX_DmaArray(  dmaNo, srcp, destp, bit)              \
    __MI_GX_Dma(  dmaNo, srcp, destp, sizeof(srcp), bit)

#define __MI_GX_DmaArrayIf(dmaNo, srcp, destp, bit)              \
    __MI_GX_DmaIf(dmaNo, srcp, destp, sizeof(srcp), bit)

#define __MI_GX_DmaArrayFast(  dmaNo, srcp, destp, bit)          \
    __MI_GX_DmaFast(  dmaNo, srcp, destp, sizeof(srcp), bit)

#define __MI_GX_DmaArrayFastIf(dmaNo, srcp, destp, bit)          \
    __MI_GX_DmaFastIf(dmaNo, srcp, destp, sizeof(srcp), bit)

//・ジオメトリFIFOからの要求によってDMAでコピーします。
//・MI_GX_DmaIf／MI_GX_DmaArrayIf／MI_GX_DmaFastIf／MI_GX_DmaArrayFastIf
//  は終了時に割り込み要求を発生します。
//・MI_GX_DmaArray／MI_GX_DmaArrayIf／MI_GX_DmaArrayFast／MI_GX_DmaArrayFastIf
//  はソース配列全体をコピーします。
//
//・引数：
//  dmaNo       DMA番号
//  srcp        ソースアドレス
//  size        転送バイト数


//----------------------------------------------------------------------
//                      ＤＭＡ 終了待ち
//----------------------------------------------------------------------

#define __MI_WaitDma(dmaNo)                                      \
{                                                           \
    vu32 *(dmaCntp) = &((vu32 *)REG_DMA0SAD_ADDR)[dmaNo * 3];       \
    while (dmaCntp[2] & MI_DMA_ENABLE) ;                       \
}

//・DMAの終了を待ちます。
//
//・引数：
//  dmaNo       DMA番号


//----------------------------------------------------------------------
//                      ＤＭＡ ストップ
//----------------------------------------------------------------------

#define __MI_StopDma(dmaNo)                                      \
{                                                           \
    vu16* dmaCntp = &((vu16 *)REG_DMA0SAD_ADDR)[dmaNo * 6];       \
    dmaCntp[5] &= ~((MI_DMA_TIMING_MASK | MI_DMA_CONTINUOUS_ON)  \
                   >> 16);                                  \
    dmaCntp[5] &= ~( MI_DMA_ENABLE   >> 16);                   \
    {u32 dummy = dmaCntp[5];}                               \
    {u32 dummy = dmaCntp[5];}                               \
}

//・DMAを停止します。
//・但し、処理中にDMAの自動起動が掛かった場合は一度だけDMAが実行されます。
//
//・引数：
//  dmaNo       DMA番号


//----------------------------------------------------------------------
//                      ＣＰＵ クリア
//----------------------------------------------------------------------

#define __MI_CpuClear(data, destp, size, bit)    UTL_CpuClear##bit(data, (void *)(destp), size)

#define __MI_CpuClearArray(data, destp, bit)                     \
    __MI_CpuClear(     data, destp, sizeof(destp), bit)

//・CPUでRAMクリアするシステムコールを呼び出します。
//・クリアデータはスタックに置かれ、それをデスティネーションへコピーします。
//・CpuClearArrayはデスティネーション配列全体をクリアします。
//
//・引数：
//  data        クリアデータ
//  destp       デスティネーションアドレス
//  size        クリアバイト数
//  bit         転送ビット幅（16|32）

//----------------------------------------------------------------------
//                      ＣＰＵ コピー
//----------------------------------------------------------------------

#define __MI_CpuCopy(srcp, destp, size, bit)    UTL_CpuCopy##bit((void *)(srcp), (void *)(destp), size)

#define __MI_CpuCopyArray(srcp, destp, bit)                      \
    __MI_CpuCopy(     srcp, destp, sizeof(srcp), bit)

//・CPUでコピーするシステムコールを呼び出します。
//・CpuCopyArrayはソース配列全体をコピーします。
//
//・引数：
//  srcp        ソースアドレス
//  destp       デスティネーションアドレス
//  size        転送バイト数
//  bit         転送ビット幅（16|32）

//----------------------------------------------------------------------
//                  ＣＰＵ 高速クリア(32Byte単位)
//----------------------------------------------------------------------

#define __MI_CpuClearFast(data, destp, size)  UTL_CpuClearFast(data, (void *)(destp), size)

#define MI_CpuClearArrayFast(data, destp)                      \
    __MI_CpuClearFast(     data, destp, sizeof(destp))

//・CPUで高速にRAMクリアするシステムコールを呼び出します。
//・クリアデータはスタックに置かれ、それをデスティネーションへコピーします。
//・CpuClearArrayFastはデスティネーション配列全体をクリアします。
//
//・引数：
//  data        クリアデータ
//  destp       デスティネーションアドレス
//  size        クリアバイト数

//----------------------------------------------------------------------
//                  ＣＰＵ 高速コピー(32Byte単位)
//----------------------------------------------------------------------

#define __MI_CpuCopyFast(srcp, destp, size)   UTL_CpuCopyFast((void *)(srcp), (void *)(destp), size)


#define MI_CpuCopyArrayFast(srcp, destp)                       \
    __MI_CpuCopyFast(     srcp, destp, sizeof(srcp))

//・CPUで高速にコピーするシステムコールを呼び出します。
//・CpuCopyArrayFastはソース配列全体をコピーします。
//
//・引数：
//  srcp        ソースアドレス
//  destp       デスティネーションアドレス
//  size        転送バイト数





#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_MI_DMA_RED_H_ */
#endif

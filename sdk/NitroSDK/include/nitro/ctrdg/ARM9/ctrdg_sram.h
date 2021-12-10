/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CTRDG - include
  File:     ctrdg_sram.h

  Copyright 2003,2004,2006,2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: ctrdg_sram.h,v $
  Revision 1.3  2007/02/20 00:28:11  kitase_hirotake
  indent source

  Revision 1.2  2006/04/07 03:28:23  okubata_ryoma
  コメントの修正

  Revision 1.1  2006/04/05 10:34:15  okubata_ryoma
  AGBバックアップライブラリのSDK収録のための変更


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NITRO_CTRDG_SRAM_H_
#define NITRO_CTRDG_SRAM_H_

#include <nitro.h>

#ifdef __cplusplus
extern "C" {
#endif

/* デバッグ用 */
//#ifndef __SRAM_DEBUG
#define CTRDG_AGB_SRAM_ADR          0x0A000000  /* SRAM先頭アドレス */
#define CTRDG_AGB_SRAM_SIZE_256K    0x00008000  /* 256KSRAMサイズ */
#define CTRDG_AGB_SRAM_SIZE_512K    0x00010000  /* 512KSRAMサイズ */
//#else
//#define CTRDG_AGB_SRAM_ADR        0x02018000
//#define CTRDG_AGB_SRAM_SIZE_256K    0x00000400
//#define CTRDG_AGB_SRAM_SIZE_512K    0x00000800
//#endif

/* CTRDG_WriteAgbSramEx関数リトライ回数上限 */
#define CTRDG_AGB_SRAM_RETRY_MAX    3


/*------------------------------------------------------------------
--------------------------------------------------------------------*/

/*------------------------------------------------------------------*/
/*          データ読み出し                                          */
/*------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
  Name:         CTRDG_ReadAgbSram
                CTRDG_ReadAgbSramAsync
  
                対応するAGB時の関数：extern void ReadSram(u8 *src,u8 *dst,u32 size)

  Description:  引数で指定されたSRAMアドレスから、sizeバイトのデータをワークのdst
                アドレス以降に読み出します。
                
  Arguments:    src       : リード元のSRAMのアドレス（AGBメモリマップ上のアドレス）
                dst       : リードしたデータを格納するワーク領域のアドレス（AGBメモリマップ上のアドレス）
                size      : バイト単位でのリードサイズ
                callback  : Read処理終了時に呼び出されるコールバック関数(非同期関数の場合のみ)

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern void CTRDG_ReadAgbSram(u32 src, void *dst, u32 size);
extern void CTRDG_ReadAgbSramAsync(u32 src, void *dst, u32 size, CTRDG_TASK_FUNC callback);


/*------------------------------------------------------------------*/
/*          データ書き込み                                          */
/*------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
  Name:         CTRDG_WriteAgbSram
                CTRDG_WriteAgbSramAsync
                
                対応するAGB時の関数：extern void WriteSram(u8 *dst,u8 *src,u32 size)

  Description:  引数で指定されたワーク領域アドレスから、sizeバイトのデータをSRAMの
                dstアドレス以降に書き込みます。
                
  Arguments:    dst       : ライト先のSRAMのアドレス（AGBメモリマップ上のアドレス）
                src       : ライト元のワーク領域のアドレス
                size      : バイト単位でのライトサイズ
                callback  : Write処理終了時に呼び出されるコールバック関数(非同期関数の場合のみ)

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern void CTRDG_WriteAgbSram(u32 dst, const void *src, u32 size);
extern void CTRDG_WriteAgbSramAsync(u32 dst, const void *src, u32 size, CTRDG_TASK_FUNC callback);


/*------------------------------------------------------------------*/
/*          データベリファイ                                        */
/*------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
  Name:         CTRDG_VerifyAgbSram
                CTRDG_VerifyAgbSramAsync
                
                対応するAGB時の関数：extern u32 VerifySram(u8 *tgt,u8 *src,u32 size)

  Description:  ワーク領域のsrcアドレスからのデータとSRAMのtgtアドレスのデータをsizeバイト分ベリファイします。
                本関数は、ベリファイが正常に終了したならば０を返し、ベリファイエラー
                があったならばエラーの発生したアドレスを返します。
                また、非同期版では、本ルーチンの呼び出し後に返ってくるコールバック関数の引数である
                構造体 CTRDGTaskInfo のメンバ result を参照することで、 Verify処理に成功したのかを知ることができます。
                
  Arguments:    tgt       : ベリファイ先SRAMアドレスへのポインタ (書き込み先のデータ、AGBメモリマップ上のアドレス）
                src       : ベリファイ元ワーク領域アドレスへのポインタ （オリジナルのデータ）
                size      : バイト単位でのベリファイサイズ
                callback  : Verify処理終了時に呼び出されるコールバック関数(非同期関数の場合のみ)

  Returns:      0     : 正常終了
                0以外 : ベリファイエラー時はデバイス側エラーアドレスを返します。(同期関数の場合のみ)
 *---------------------------------------------------------------------------*/
extern u32 CTRDG_VerifyAgbSram(u32 tgt, const void *src, u32 size);
extern void CTRDG_VerifyAgbSramAsync(u32 tgt, const void *src, u32 size, CTRDG_TASK_FUNC callback);


/*------------------------------------------------------------------*/
/*          データ書き込み＆ベリファイ                              */
/*------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
  Name:         CTRDG_WriteAndVerifyAgbSram
                CTRDG_WriteAndVerifyAgbSramAsync
                
                対応するAGB時の関数：extern u32 WriteSramEx(u8 *dst, u8 *src, u32 size)

  Description:  内部でCTRDG_WriteAgbSramで書き込みを行った後CTRDG_VerifyAgbSramでベリファイを行い、
                エラーの場合は最大でCTRDG_AGB_SRAM_RETRY_MAX（ctrdg_sram.hにて定義）回リトライを行います。
                
                また、非同期版では、本ルーチンの呼び出し後に返ってくるコールバック関数の引数である
                構造体 CTRDGTaskInfo のメンバ result を参照することで、 WriteAndVerify処理に成功したのかを知ることができます。
                
  Arguments:    dst       : ライト先のSRAMのアドレス（AGBメモリマップ上のアドレス）
                src       : ライト元のワーク領域のアドレス
                size      : バイト単位でのライト/ベリファイサイズ
                callback  : WritrAndVerify処理終了時に呼び出されるコールバック関数(非同期関数の場合のみ)

  Returns:      0     : 正常終了
                0以外 : ベリファイエラー時はデバイス側エラーアドレスを返します。(同期関数の場合のみ)
 *---------------------------------------------------------------------------*/
extern u32 CTRDG_WriteAndVerifyAgbSram(u32 dst, const void *src, u32 size);
extern void CTRDG_WriteAndVerifyAgbSramAsync(u32 dst, const void *src, u32 size,
                                             CTRDG_TASK_FUNC callback);


#ifdef __cplusplus
}      /* extern "C" */
#endif

#endif /* NITRO_CTRDG_SRAM_H_ */

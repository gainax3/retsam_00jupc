/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CTRDG - include
  File:     ctrdg_flash.h

  Copyright 2003,2004,2006,2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: ctrdg_flash.h,v $
  Revision 1.3  2007/02/20 00:28:11  kitase_hirotake
  indent source

  Revision 1.2  2006/04/07 03:28:43  okubata_ryoma
  コメントの修正

  Revision 1.1  2006/04/05 10:34:15  okubata_ryoma
  AGBバックアップライブラリのSDK収録のための変更


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_CTRDG_FLASH_H_
#define NITRO_CTRDG_FLASH_H_

#include <nitro.h>

#ifdef __cplusplus
extern "C" {
#endif

/* definition ****************************************************************/

/* フラッシュ先頭アドレス */
#define CTRDG_AGB_FLASH_ADR           0x0A000000

/* CTRDG_WriteAndVerifyAgbFlash関数リトライ回数上限 */
#define CTRDG_AGB_FLASH_RETRY_MAX     3


typedef struct CTRDGiFlashSectorTag
{
    u32     size;                      /* セクタサイズ */
    u16     shift;                     /* セクタサイズをシフト数に換算した値 */
    u16     count;                     /* セクタ数 */
    u16     top;                       /* 使用可能先頭セクタNo. */
    u8      reserved[2];               /* パディング */
}
CTRDGiFlashSector;

typedef struct CTRDGFlashTypeTag
{
    u32     romSize;                   /* ROMサイズ */
    CTRDGiFlashSector sector;          /* セクタ情報(※上記CTRDGiFlashSector参照。) */
    MICartridgeRamCycle agbWait[2];    /* AGBカートリッジバスRAM領域のread/writeウェイト値 */
    u16     makerID;                   /* ベンダID */
    u16     deviceID;                  /* デバイスID */
}
CTRDGFlashType;

extern void CTRDGi_SetFlashBankMx(u16 bank);
extern u16 CTRDGi_ReadFlashID(void);
extern void StartFlashTimer(u16 phase);
extern void CheckFlashTimer(void);
extern void CTRDGi_SetFlashBankMx(u16 bank);
extern u16 CTRDGi_PollingSR512kCOMMON(u16 phase, u8 *adr, u16 lastData);
extern u16 CTRDGi_PollingSR1MCOMMON(u16 phase, u8 *adr, u16 lastData);

/*------------------------------------------------------------------*/
/*          グローバル変数                                          */
/*------------------------------------------------------------------*/

/*
 * フラッシュの構成内容を示すデータへのポインタ。
 * （詳細は上記データ定義部参照)
 */
extern const CTRDGFlashType *AgbFlash;

/*
 * フラッシュライト時の進捗を示すバロメータ
 * ※ctrdg_flash_remainderは、アトメル製フラッシュでは128ずつ減少し、
 *   その他のデバイスでは1ずつ減少します。
 */
extern u16 ctrdg_flash_remainder;

/*------------------------------------------------------------------*/
/*          データ読み出し                                          */
/*------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
  Name:         CTRDG_ReadAgbFlash
                CTRDG_ReadAgbFlashAsync
  
                対応するAGB時の関数：extern void ReadFlash(u16 secNo,u32 offset,u8 *dst,u32 size)

  Description:  フラッシュの対象セクタ№内における offset バイト先のアドレスから、
                size バイト分のデータをワークの dst アドレス以降に読み出します。
                
                アクセスサイクルの設定は関数内で行いますので、事前に行う必要はありません。

                ※この関数内では一定期間カートリッジバスがロックされますので注意して下さい。

                
  Arguments:    secNo    : 対象セクタ№
                offset   : セクタ内のバイト単位のオフセット
                dst      : リードしたデータを格納するワーク領域のアドレス
                size     : バイト単位でのリードサイズ
                callback : Read処理終了時に呼び出されるコールバック関数(非同期関数の場合のみ)

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern void CTRDG_ReadAgbFlash(u16 sec_num, u32 offset, u8 *dst, u32 size);
extern void CTRDG_ReadAgbFlashAsync(u16 sec_num, u32 offset, u8 *dst, u32 size,
                                    CTRDG_TASK_FUNC callback);


/*------------------------------------------------------------------*/
/*          チップ消去                                              */
/*------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
  Name:         CTRDG_EraseAgbFlashChip
                CTRDG_EraseAgbFlashChipAsync
                
                対応するAGB時の関数：extern u16 (*EraseFlashChip)()

  Description:  チップ全体を完全にイレースします。
  
                アクセスサイクルの設定は関数内で行いますので、事前に行う必要はありません。
                また、この関数ではタイムアウト測定用にチックを使用しますので事前に 
                OS_InitTick 関数を呼んでおく必要があります。
                
                また、非同期版では、本ルーチンの呼び出し後に返ってくるコールバック関数の引数である
                構造体 CTRDGTaskInfo のメンバ result を参照することで、 Erase処理に成功したのかを知ることができます。

                ※この関数内では一定期間全ての割り込みが禁止され、カートリッジバスがロックされますので注意して下さい。
                特に、本関数コール時はダイレクトサウンドおよびＶ・Ｈブランク同期、表示同期、
                カートリッジリクエスト等の特定のタイミングで自動起動するＤＭＡは使用しないで下さい。

                
  Arguments:    callback : Erase処理終了時に呼び出されるコールバック関数(非同期関数の場合のみ)

  Returns:      0     : 正常終了       (同期関数の場合のみ)
                0以外 : イレースエラー
 *---------------------------------------------------------------------------*/
extern u16 CTRDG_EraseAgbFlashChip(void);
extern void CTRDG_EraseAgbFlashChipAsync(CTRDG_TASK_FUNC callback);


/*------------------------------------------------------------------*/
/*          セクタ消去                                              */
/*------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
  Name:         CTRDG_EraseAgbFlashSector
                CTRDG_EraseAgbFlashSectorAsync
                
                対応するAGB時の関数：extern u16 (*EraseFlashSector)(u16 secNo)

  Description:  対象セクタ № のセクタをイレースします。
                このルーチンは書き込みルーチンの中でコールされるため、
                通常は書き込み前にこのルーチンをコールする必要はありません。
                対象セクタ№が範囲外の時はエラーを返します。
                
                また、非同期版では、本ルーチンの呼び出し後に返ってくるコールバック関数の引数である
                構造体 CTRDGTaskInfo のメンバ result を参照することで、 Erase処理に成功したのかを知ることができます。
                
                アクセスサイクルの設定は関数内で行いますので、事前に行う必要はありません。
                また、この関数ではタイムアウト測定用にチックを使用しますので事前に 
                OS_InitTick 関数を呼んでおく必要があります。

                ※この関数内では一定期間全ての割り込みが禁止され、カートリッジバスがロックされますので注意して下さい。
                特に、本関数コール時はダイレクトサウンドおよびＶ・Ｈブランク同期、表示同期、
                カートリッジリクエスト等の特定のタイミングで自動起動するＤＭＡは使用しないで下さい。

                
  Arguments:    sec_num   : 対象セクタ№
                callback  : Erase処理終了時に呼び出されるコールバック関数(非同期関数の場合のみ)

  Returns:      0     : 正常終了       (同期関数の場合のみ)
                0以外 : イレースエラー
 *---------------------------------------------------------------------------*/
extern u16 CTRDG_EraseAgbFlashSector(u16 sec_num);
extern void CTRDG_EraseAgbFlashSectorAsync(u16 sec_num, CTRDG_TASK_FUNC callback);


/*------------------------------------------------------------------*/
/*          セクタ単位データ書き込み                                */
/*------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
  Name:         CTRDG_WriteAgbFlashSector
                CTRDG_WriteAgbFlashSectorAsync
                
                対応するAGB時の関数：extern u16 (*ProgramFlashSector)(u16 secNo,u8 *src)

  Description:  srcアドレスから１セクタ分(4kbyte)のデータを対象セクタ№に書き込みます。
                本ルーチン内で上記のCTRDG_EraseAgbFlashSectorをコールし、セクタを消去してから書き込みを行います。
                対象セクタ№が範囲外の時はエラーを返します。
                本ルーチンの実行中、グローバル変数ctrdg_flash_remainderを参照することで、残バイト数を知ることができます。
                
                また、非同期版では、本ルーチンの呼び出し後に返ってくるコールバック関数の引数である
                構造体 CTRDGTaskInfo のメンバ result を参照することで、 Write処理に成功したのかを知ることができます。
                
                アクセスサイクルの設定は関数内で行いますので、事前に行う必要はありません。
                また、この関数ではタイムアウト測定用にチックを使用しますので事前に 
                OS_InitTick 関数を呼んでおく必要があります。

                ※この関数内では一定期間全ての割り込みが禁止され、カートリッジバスがロックされますので注意して下さい。
                特に、本関数コール時はダイレクトサウンドおよびＶ・Ｈブランク同期、表示同期、
                カートリッジリクエスト等の特定のタイミングで自動起動するＤＭＡは使用しないで下さい。

                
  Arguments:    sec_num   : 対象セクタ№
                src       : 書き込み元アドレス
                callback  : Write処理終了時に呼び出されるコールバック関数(非同期関数の場合のみ)

  Returns:      0     : 正常終了       (同期関数の場合のみ)
                0以外 : ライトエラー
 *---------------------------------------------------------------------------*/
extern u16 CTRDG_WriteAgbFlashSector(u16 sec_num, u8 *src);
extern void CTRDG_WriteAgbFlashSectorAsync(u16 sec_num, u8 *src, CTRDG_TASK_FUNC callback);


/*------------------------------------------------------------------*/
/*          バイト単位データベリファイ                              */
/*------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
  Name:         CTRDG_VerifyAgbFlash
                CTRDG_VerifyAgbFlashAsync
                
                対応するAGB時の関数：extern u32 VerifyFlash(u16 secNo,u8 *src,u32 size)

  Description:  srcアドレスからのデータとフラッシュの対象セクタ№のデータをsizeバイト分ベリファイします。
                本関数は、ベリファイが正常に終了したならば０を返し、ベリファイエラーがあったならばエラーの発生したアドレスを返します。
                また、非同期版では、本ルーチンの呼び出し後に返ってくるコールバック関数の引数である
                構造体 CTRDGTaskInfo のメンバ result を参照することで、 Verify処理に成功したのかを知ることができます。
                
  Arguments:    sec_num   : 対象セクタ№
                src       : ベリファイ元アドレス（オリジナルデータ）
                size      : ベリファイサイズ(byte)
                callback  : Verify処理終了時に呼び出されるコールバック関数(非同期関数の場合のみ)

  Returns:      0     : 正常終了                                                (同期関数の場合のみ)
                0以外 : ベリファイエラー時はデバイス側エラーアドレスを返します。
 *---------------------------------------------------------------------------*/
extern u32 CTRDG_VerifyAgbFlash(u16 sec_num, u8 *src, u32 size);
extern void CTRDG_VerifyAgbFlashAsync(u16 sec_num, u8 *src, u32 size, CTRDG_TASK_FUNC callback);


/*------------------------------------------------------------------*/
/*          セクタ単位データ書き込み＆バイト単位ベリファイ          */
/*------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
  Name:         CTRDG_WriteAndVerifyAgbFlash
                CTRDG_WriteAndVerifyAgbFlashAsync
                
                対応するAGB時の関数：extern u32 ProgramFlashSectorEx2(u16 secNo,u8 *src,u32 verifySize)

  Description:  本関数は、内部で CTRDG_WriteAgbFlashSector 関数で書き込みを行った後
                CTRDG_VerifyAgbFlash 関数で verifysize バイト分ベリファイを行います。
                エラーの場合は最大でCTRDG_AGB_FLASH_RETRY_MAX(ctrdg_flash.hで定義）回リトライを行います。
                
                ※本関数では、ライトエラー時は32bit中の16bitでエラーコードを返しますが、
                ベリファイエラー時は32bitのデバイス側エラーアドレスを返しますので、
                エラーコード確認の際はご注意下さい。
                また、非同期版では、本ルーチンの呼び出し後に返ってくるコールバック関数の引数である
                構造体 CTRDGTaskInfo のメンバ result を参照することで、 WriteAndVerify処理に成功したのかを知ることができます。
                
                アクセスサイクルの設定は関数内で行いますので、事前に行う必要はありません。
                また、この関数ではタイムアウト測定用にチックを使用しますので事前に 
                OS_InitTick 関数を呼んでおく必要があります。

                ※この関数内では一定期間全ての割り込みが禁止され、カートリッジバスがロックされますので注意して下さい。
                特に、本関数コール時はダイレクトサウンドおよびＶ・Ｈブランク同期、表示同期、
                カートリッジリクエスト等の特定のタイミングで自動起動するＤＭＡは使用しないで下さい。

                
  Arguments:    sec_num    : 対象セクタ№
                src        : 書き込み元アドレス
                verifysize : ベリファイサイズ（byte）
                callback   : WriteAndVerify処理終了時に呼び出されるコールバック関数(非同期関数の場合のみ)

  Returns:      0     : 正常終了                                                (同期関数の場合のみ)
                0以外 : ベリファイエラー時はデバイス側エラーアドレスを返します。
 *---------------------------------------------------------------------------*/
extern u32 CTRDG_WriteAndVerifyAgbFlash(u16 sec_num, u8 *src, u32 verifysize);
extern void CTRDG_WriteAndVerifyAgbFlashAsync(u16 sec_num, u8 *src, u32 verifysize,
                                              CTRDG_TASK_FUNC callback);


#ifdef __cplusplus
}      /* extern "C" */
#endif


#endif /* NITRO_CTRDG_FLASH_H_ */

/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - include
  File:     mb_child.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_child.h,v $
  Revision 1.28  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.27  2005/03/01 01:57:00  yosizaki
  copyright の年を修正.

  Revision 1.26  2005/02/28 05:26:25  yosizaki
  do-indent.

  Revision 1.25  2004/11/22 12:53:17  takano_makoto
  MbGameInfoXXX関連の型定義、関数定義を他へ移動

  Revision 1.24  2004/10/28 11:01:42  yada
  just fix comment

  Revision 1.23  2004/10/21 08:52:09  takano_makoto
  コメントの間違い修正

  Revision 1.22  2004/09/28 12:50:06  yosiokat
  IPLブランチ1.17.2.4とマージ。

  Revision 1.17.2.4  2004/09/28 02:38:56  yosiokat
  MbGameInfoRecvListにbeaconNoとsameBeaconRecvCountを追加。

  Revision 1.21  2004/09/20 10:39:13  yosiokat
  IPLブランチRev.1.17.2.3とのマージ。

  Revision 1.17.2.3  2004/09/20 10:17:25  yosiokat
  MbGameInfoRecvListからseqNoFixed, seqNoVolatを削除。（受信側では、同gameInfo内のseqNoFixed, seqNoVolatを使用するよう修正。）

  Revision 1.17.2.2  2004/09/18 08:46:19  miya
  HEADとマージ

  Revision 1.20  2004/09/16 12:51:02  sato_masaki
  - MBi_***で定義していた関数、型を、MB_***に変更。

  Revision 1.19  2004/09/16 12:20:54  miya
  ファイル更新用の改造

  Revision 1.18  2004/09/16 08:43:32  miya
  ファイル更新用メンバ追加

  Revision 1.17  2004/09/15 06:55:38  yosiokat
  MbBeaconRecvStatusにu16 usefulGameInfoFlagを追加。

  Revision 1.16  2004/09/13 13:18:10  sato_masaki
  modify pad bytes

  Revision 1.15  2004/09/13 09:25:39  yosiokat
  ・スキャンロック時にロック対象が全く見つからない状態の対策のため、MbBeaconRecvStatusにnotFoundLockTargetCountを追加。
  ・MbGameInfoRecvList.lifeTimeCountをs16に修正。

  Revision 1.14  2004/09/13 08:11:41  sato_masaki
  MBiError, MBiErrorStatus are moved to mb.h

  Revision 1.13  2004/09/13 02:56:58  sato_masaki
  MBiErrorStatus, MBiErrorを追加。

  Revision 1.12  2004/09/11 12:39:27  yosiokat
  ・ロック時間、親機情報寿命のカウント処理修正に伴い、MbBeaconRecvStatusの要素を変更。

  Revision 1.11  2004/09/11 11:13:06  sato_masaki
  - 子機用バッファサイズ、MB_CHILD_SYSTEM_BUF_SIZE を定義。
  - 使用ワークメモリサイズ取得関数、MB_GetChildSystemBufSize()追加。

  Revision 1.10  2004/09/10 06:32:13  sato_masaki
  comment追加。

  Revision 1.9  2004/09/09 13:52:26  sato_masaki
  通信強度表示linkLevelの追加。

  Revision 1.8  2004/09/09 10:05:10  sato_masaki
  MBCommCStateにGameInfo関連の項目追加。

  Revision 1.7  2004/09/09 04:34:56  yosiokat
  MB_CARD_ROM_HEADER_ADDRESSの追加。

  Revision 1.6  2004/09/08 12:52:22  sato_masaki
  MB_COMM_CSTATE_ERRORを追加。

  Revision 1.5  2004/09/07 04:34:22  sato_masaki
  small fix

  Revision 1.4  2004/09/06 11:12:25  sato_masaki
  マルチブートRAMアドレス定義(内部使用)をmb_private.hから移動。

  Revision 1.3  2004/09/04 09:55:26  sato_masaki
  mb.hに残っていた、子機用定義を追加。

  Revision 1.2  2004/09/03 12:05:13  sato_masaki
  comment修正

  Revision 1.1  2004/09/03 07:06:28  sato_masaki
  ファイルを機能別に分割。


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

/* ==========================================================================

    MBライブラリの子機用ヘッダです。
    マルチブート子機およびIPLにて当ライブラリを使用する場合は、
    nitro/mb.hに加え、このヘッダをインクルードしてください。

   ==========================================================================*/


#ifndef _MB_CHILD_H_
#define _MB_CHILD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/types.h>
#include <nitro/memorymap.h>
#include <nitro/mb.h>
#include "mb_fileinfo.h"
#include "mb_gameinfo.h"

/* ---------------------------------------------------------------------

        define定数

   ---------------------------------------------------------------------*/

#define MB_CHILD_SYSTEM_BUF_SIZE        (0x6000)

#define MB_MAX_SEND_BUFFER_SIZE         (0x400)
#define MB_MAX_RECV_BUFFER_SIZE         (0x80)

/* ----------------------------------------------------------------------------

    マルチブートRAMアドレス定義(内部使用)

   ----------------------------------------------------------------------------*/

/* 
    マルチブートプログラム配置可能範囲

    無線マルチブートにおけるARM9コードの初期ロード最大サイズは2.5MBです。
    
    ARM9コードのロード可能領域は
    MB_LOAD_AREA_LO ～ MB_LOAD_AREA_HI
    の範囲です。
 */
#define MB_LOAD_AREA_LO                         ( HW_MAIN_MEM )
#define MB_LOAD_AREA_HI                         ( HW_MAIN_MEM + 0x002c0000 )
#define MB_LOAD_MAX_SIZE                        ( MB_LOAD_AREA_HI - MB_LOAD_AREA_LO )


/*
    マルチブート時のARM7 staticの子機側での一時受信バッファアドレス定義
    
    ARM7コードのロード可能領域は

    a) 0x02000000 ～ 0x02300000
       (MB_LOAD_AREA_LO ～ MB_ARM7_STATIC_RECV_BUFFER_END)
    b) 0x02300000 ～ 0x023fe000
       (MB_ARM7_STATIC_RECV_BUFFER_END ～ 0x023fe000)
    c) 0x037f8000 ～ 0x0380f000
       (内部WRAM)

    のいずれかになります。
    
    ※注意
    ARM7コードを内部WRAMもしくは 0x02300000 以降にロードする場合、

    0x022c0000 ～ 0x02300000
    (MB_LOAD_AREA_HI ～ MB_ARM7_STATIC_RECV_BUFFER_END)
    にバッファを設け、このバッファに受信させたのちに
    ブート時に指定アドレスに再配置するようにしています。
    
    ・0x02300000 のアドレスをまたぐ配置のARM7コード
    ・0x02300000 以降のアドレスで、サイズが 0x40000 ( MB_ARM7_STATIC_RECV_BUFFER_SIZE ) 
    　を超えるARM7コードは正常動作を保証できませんので、
    　このようなコードの作成を禁止いたします。
 */

#define MB_ARM7_STATIC_RECV_BUFFER              MB_LOAD_AREA_HI
#define MB_ARM7_STATIC_RECV_BUFFER_END          ( HW_MAIN_MEM + 0x00300000 )
#define MB_ARM7_STATIC_RECV_BUFFER_SIZE         ( MB_ARM7_STATIC_RECV_BUFFER_END - MB_LOAD_AREA_HI )
#define MB_ARM7_STATIC_LOAD_AREA_HI             ( MB_LOAD_AREA_HI + MB_ARM7_STATIC_RECV_BUFFER_SIZE )
#define MB_ARM7_STATIC_LOAD_WRAM_MAX_SIZE       ( 0x18000 - 0x1000 )    // 0x18000 = HW_WRAM_SIZE + HW_PRV_WRAM_SIZE

/* BssDescの一時格納場所 */
#define MB_BSSDESC_ADDRESS                      ( HW_MAIN_MEM + 0x003fe800 )
/* MBDownloadFileInfoの一時格納場所 */
#define MB_DOWNLOAD_FILEINFO_ADDRESS            ( MB_BSSDESC_ADDRESS + ( ( ( MB_BSSDESC_SIZE ) + (32) - 1 ) & ~((32) - 1) ) )
/* マルチブートを起動した時点で差さっているROMカードのROMヘッダ退避場所 */
#define MB_CARD_ROM_HEADER_ADDRESS              ( MB_DOWNLOAD_FILEINFO_ADDRESS + ( ( ( MB_DOWNLOAD_FILEINFO_SIZE ) + (32) - 1 ) & ~((32) - 1) ) )
/* ROM Headerの格納場所 */
#define MB_ROM_HEADER_ADDRESS                   HW_ROM_HEADER_BUF


/* ---------------------------------------------------------------------

        マルチブートライブラリ(MB)子機用API. IPL で使用されます

   ---------------------------------------------------------------------*/

/* 子機パラメータ設定 & スタート */
int     MB_StartChild(void);

/* MB子機で使用するワークメモリのサイズを取得 */
int     MB_GetChildSystemBufSize(void);

/* 子機状態通知コールバックの設定 */
void    MB_CommSetChildStateCallback(MBCommCStateCallbackFunc callback);

/* 子機状態の取得 */
int     MB_CommGetChildState(void);

/* ダウンロード開始 */
BOOL    MB_CommStartDownload(void);

/* 子機のダウンロード進捗状況を 0 ～ 100 で取得 */
u16     MB_GetChildProgressPercentage(void);

/* 親機への接続試み＆ダウンロードファイル要求 */
int     MB_CommDownloadRequest(int index);      // MbBeaconRecvStatusのリストNo.で接続要求する親機を指定。

#ifdef __cplusplus
}
#endif

#endif /*  _MB_CHILD_H_    */

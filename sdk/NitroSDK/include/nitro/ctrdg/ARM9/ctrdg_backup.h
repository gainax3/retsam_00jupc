/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CTRDG - include
  File:     ctrdg_backup.h

  Copyright 2001,2003,2004,2006,2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: ctrdg_backup.h,v $
  Revision 1.4  2007/02/20 00:28:11  kitase_hirotake
  indent source

  Revision 1.3  2006/04/07 07:56:11  okubata_ryoma
  注意コメント追加

  Revision 1.2  2006/04/07 03:29:03  okubata_ryoma
  コメントの修正

  Revision 1.1  2006/04/05 10:34:15  okubata_ryoma
  AGBバックアップライブラリのSDK収録のための変更


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_CTRDG_BACKUP_H_
#define NITRO_CTRDG_BACKUP_H_

#include <nitro.h>
#include "ctrdg_task.h"
#include "ctrdg_flash.h"
#include "ctrdg_sram.h"

// define data----------------------------------
#define CTRDG_BACKUP_PHASE_VERIFY           0x0000
#define CTRDG_BACKUP_PHASE_PROGRAM          0x0001
#define CTRDG_BACKUP_PHASE_SECTOR_ERASE     0x0002
#define CTRDG_BACKUP_PHASE_CHIP_ERASE       0x0003
#define CTRDG_BACKUP_PHASE_VERIFY_ERASE     0x0004
#define CTRDG_BACKUP_PHASE_PARAMETER_CHECK  0x00ff

#define CTRDG_BACKUP_RESULT_OK              0x0000
#define CTRDG_BACKUP_RESULT_ERROR           0x8000
#define CTRDG_BACKUP_RESULT_TIMEOUT         0x4000
#define CTRDG_BACKUP_RESULT_Q5TIMEOUT       0x2000
#define CTRDG_BACKUP_RESULT_PULLEDOUT       0x1000

/* デバッグ用 */
//#ifndef   __FLASH_DEBUG
#define CTRDG_BACKUP_VENDER_ID_ADR      (CTRDG_AGB_FLASH_ADR+0x00000000)
#define CTRDG_BACKUP_DEVICE_ID_ADR      (CTRDG_AGB_FLASH_ADR+0x00000001)
#define CTRDG_BACKUP_COM_ADR1           (CTRDG_AGB_FLASH_ADR+0x00005555)
#define CTRDG_BACKUP_COM_ADR2           (CTRDG_AGB_FLASH_ADR+0x00002aaa)
//#else
//#define   VENDER_ID_ADR       0x02003ffc
//#define   DEVICE_ID_ADR       0x02003ffd
//#define   COM_ADR1            0x02003ffe
//#define COM_ADR2          0x02003fff
//#endif

#define CTRDG_BACKUP_MEGA_512K_ID       0x1cc2
#define CTRDG_BACKUP_MEGA_1M_ID         0x09c2
//#define   CTRDG_BACKUP_MEGA_1M_ID         0x1cc2

// data type declaration------------------------
typedef struct CTRDGiFlashTypePlusTag
{
    u16     (*CTRDGi_WriteAgbFlashSector) (u16 secNo, u8 *src);
    u16     (*CTRDGi_EraseAgbFlashChip) (void);
    u16     (*CTRDGi_EraseAgbFlashSector) (u16 secNo);
    void    (*CTRDGi_WriteAgbFlashSectorAsync) (u16 secNo, u8 *src, CTRDG_TASK_FUNC callback);
    void    (*CTRDGi_EraseAgbFlashChipAsync) (CTRDG_TASK_FUNC callback);
    void    (*CTRDGi_EraseAgbFlashSectorAsync) (u16 secNo, CTRDG_TASK_FUNC callback);
    u16     (*CTRDGi_PollingSR) (u16 phase, u8 *adr, u16 lastData);
    const u16 (*maxtime);
    CTRDGFlashType type;
}
CTRDGiFlashTypePlus;

typedef enum
{
    CTRDG_BACKUP_TYPE_FLASH_512K,      /* 512kフラッシュ */
    CTRDG_BACKUP_TYPE_FLASH_1M,        /* 1Mフラッシュ */
    CTRDG_BACKUP_TYPE_SRAM             /* 256kSRAM */
}
CTRDGBackupType;

// extern variables declaration----------------------
extern u16 ctrdg_flash_remainder;
extern const u16 (*ctrdgi_fl_maxtime);
extern u16 (*CTRDGi_PollingSR) (u16 phase, u8 *adr, u16 lastData);

// function declaration-------------------------
extern const CTRDGFlashType *AgbFlash;
extern u16 (*CTRDGi_WriteAgbFlashSector) (u16 secNo, u8 *src);
extern u16 (*CTRDGi_EraseAgbFlashChip) (void);
extern u16 (*CTRDGi_EraseAgbFlashSector) (u16 secNo);
extern void (*CTRDGi_WriteAgbFlashSectorAsync) (u16 secNo, u8 *src, CTRDG_TASK_FUNC callback);
extern void (*CTRDGi_EraseAgbFlashChipAsync) (CTRDG_TASK_FUNC callback);
extern void (*CTRDGi_EraseAgbFlashSectorAsync) (u16 secNo, CTRDG_TASK_FUNC callback);
extern u16 (*pollingSR) (u16 phase, u8 *adr, u16 lastData);

/*------------------------------------------------------------------*/
/*          デバイスの識別および初期設定                            */
/*------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
  Name:         CTRDG_IdentifyAgbBackup
                
                対応するAGB時の関数：extern u16 IdentifyFlash_512K()
                対応するAGB時の関数：extern u16 IdentifyFlash_1M()
                
  Description:  NITRO-CTRDG に搭載されているバックアップデバイスを指定します。
                バックアップデバイスがフラッシュの場合はフラッシュの ID を読み出し、
                どのフラッシュがカートリッジに搭載されているかを識別して、
                フラッシュの容量やセクタサイズの取得、アクセススピードの設定、
                更に対応するフラッシュ用の各アクセス関数のセットを行います。
                取得したフラッシュのデータはグローバル変数 CTRDGFlashType *AgbFlash で参照することができます。
                (CTRDGFlashTypeの詳細は上記定義部を参照。)
                
                本関数はバックアップデバイスにアクセスするより以前（リードも含めて）に１回コールする必要があります。
                デバイスを識別できなかった場合はエラーを返し、以下のアクセス関数は使用不可となります。
                
                アクセスサイクルの設定は関数内で行いますので、事前に行う必要はありません。
                この関数ではタイムアウト測定用にチックを使用しますので事前に 
                OS_InitTick 関数を呼んでおく必要があります。

                ※この関数内では一定期間全ての割り込みが禁止され、カートリッジバスがロックされますので注意して下さい。
                特に、本関数コール時はダイレクトサウンドおよびＶ・Ｈブランク同期、表示同期、
                カートリッジリクエスト等の特定のタイミングで自動起動するＤＭＡは使用しないで下さい。
                
                また引数にCTRDG_BACKUP_TYPE_FLASH_512Kまたは、CTRDG_BACKUP_TYPE_FLASH_1Mを与えた場合、
                デバイスへの書き込み動作が発生しますので、与える引数と異なる種類のデバイスがNITRO-CTRDGに搭載されていると
                バックアップデータが破壊されることがありますので注意してください。


  Arguments:    type  : NITRO-CTRDGに搭載されているバックアップデバイスの種類

  Returns:      0     : 正常終了
                0以外 : 識別エラー
 *---------------------------------------------------------------------------*/
extern u16 CTRDG_IdentifyAgbBackup(CTRDGBackupType type);

#endif // NITRO_CTRDG_BACKUP_H_

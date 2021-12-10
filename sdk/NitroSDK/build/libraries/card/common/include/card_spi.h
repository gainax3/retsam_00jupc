/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CARD - libraries
  File:     card_spi.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: card_spi.h,v $
  Revision 1.15  2007/11/13 04:22:02  yosizaki
  support erase-SUBSECTOR.

  Revision 1.14  2007/10/04 13:11:37  yosizaki
  add test commands.

  Revision 1.13  2007/02/20 00:28:12  kitase_hirotake
  indent source

  Revision 1.12  2006/04/10 04:13:21  yosizaki
  add comments.

  Revision 1.11  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.10  2005/08/04 15:02:16  yosizaki
  add CARDi_InitStatusRegister().

  Revision 1.9  2005/03/01 01:57:00  yosizaki
  copyright の年を修正.

  Revision 1.8  2005/02/28 05:26:02  yosizaki
  do-indent.

  Revision 1.7  2004/12/08 12:38:47  yosizaki
  remove CardDeviceInfo.

  Revision 1.6  2004/08/23 10:38:23  yosizaki
  add write-command.
  add retry count on writing timeout.

  Revision 1.5  2004/08/18 08:18:54  yosizaki
  change CARDBackupType format.

  Revision 1.4  2004/07/28 13:18:14  yosizaki
  asynchronous mode support.

  Revision 1.3  2004/07/23 08:28:07  yosizaki
  change backup-routine to ARM7 only.

  Revision 1.2  2004/07/19 13:14:23  yosizaki
  add CARDi_ProgramBackupCore.

  Revision 1.1  2004/06/28 01:53:00  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef __NITRO_CARD_SPI_H__
#define __NITRO_CARD_SPI_H__


/*****************************************************************************/
/* constant */

/* for CARDi_Comm() */
#define	CSPI_CONTINUOUS_ON	0x0040
#define	CSPI_CONTINUOUS_OFF	0x0000

/* コマンド */
#define COMM_WRITE_STATUS	0x01   /* ステータスライト. */
#define COMM_PROGRAM_PAGE	0x02   /* ページプログラム. */
#define COMM_READ_ARRAY		0x03   /* リード. */
#define COMM_WRITE_DISABLE	0x04   /* ライトディセーブル. (未使用) */
#define COMM_READ_STATUS	0x05   /* ステータスリード. */
#define COMM_WRITE_ENABLE	0x06   /* ライトイネーブル. */

/* FLASH */
#define COMM_PAGE_WRITE		0x0A
#define COMM_PAGE_ERASE		0xDB
#define COMM_SECTOR_ERASE	0xD8
#define COMM_SUBSECTOR_ERASE	0x20
#define COMM_CHIP_ERASE		0xC7
#define CARDFLASH_READ_BYTES_FAST	0x0B    /* 未使用 */
#define CARDFLASH_DEEP_POWDOWN		0xB9    /* 未使用 */
#define CARDFLASH_WAKEUP			0xAB    /* 未使用 */

/* ステータスレジスタのビット */
#define	COMM_STATUS_WIP_BIT		0x01    /* WriteInProgress (bufy) */
#define	COMM_STATUS_WEL_BIT		0x02    /* WriteEnableLatch */
#define	COMM_STATUS_BP0_BIT		0x04
#define	COMM_STATUS_BP1_BIT		0x08
#define	COMM_STATUS_BP2_BIT		0x10
#define COMM_STATUS_WPBEN_BIT	0x80


/*****************************************************************************/
/* function */

/*---------------------------------------------------------------------------*
  Name:         CARDi_InitStatusRegister

  Description:  FRAM/EEPROM の場合, 起動時にステータスレジスタの補正処理を行う.
                ・FRAM は電源投入時にライトプロテクトの変化が起こりうるため.
                ・EEPROM は納入時に初期値不正がありうるため.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_InitStatusRegister(void);

/*---------------------------------------------------------------------------*
  Name:         CARDi_IdentifyBackupCore

  Description:  デバイスタイプ特定.

  Arguments:    type              特定するデバイスタイプ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_IdentifyBackupCore(CARDBackupType type);

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommandReadStatus

  Description:  リードステータス

  Arguments:    None

  Returns:      ステータス値
 *---------------------------------------------------------------------------*/
u8 CARDi_CommandReadStatus(void);

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadBackupCore

  Description:  デバイスへのリードコマンド全体.

  Arguments:    src               読み出し元のデバイスオフセット.
                dst               読み出し先のメモリアドレス.
                len               読み出しサイズ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_ReadBackupCore(u32 src, void *dst, u32 len);

/*---------------------------------------------------------------------------*
  Name:         CARDi_ProgramBackupCore

  Description:  デバイスへのプログラム(消去無し書き込み)コマンド全体.

  Arguments:    dst               書き込み先のデバイスオフセット.
                src               書き込み元のメモリアドレス.
                len               書き込みサイズ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_ProgramBackupCore(u32 dst, const void *src, u32 len);

/*---------------------------------------------------------------------------*
  Name:         CARDi_WriteBackupCore

  Description:  デバイスへのライト(消去 + プログラム)コマンド全体.

  Arguments:    dst               書き込み先のデバイスオフセット.
                src               書き込み元のメモリアドレス.
                len               書き込みサイズ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_WriteBackupCore(u32 dst, const void *src, u32 len);

/*---------------------------------------------------------------------------*
  Name:         CARDi_VerifyBackupCore

  Description:  デバイスへのベリファイ(実際はリード + 比較処理)コマンド全体.

  Arguments:    dst               比較先のデバイスオフセット.
                src               比較元のメモリアドレス.
                len               比較サイズ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_VerifyBackupCore(u32 dst, const void *src, u32 len);

/*---------------------------------------------------------------------------*
  Name:         CARDi_EraseBackupSectorCore

  Description:  デバイスへのセクタ消去コマンド全体.

  Arguments:    dst               消去先のデバイスオフセット.
                len               消去サイズ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_EraseBackupSectorCore(u32 dst, u32 len);

/*---------------------------------------------------------------------------*
  Name:         CARDi_EraseBackupSubSectorCore

  Description:  デバイスへのサブセクタ消去コマンド全体.

  Arguments:    dst               消去先のデバイスオフセット.
                len               消去サイズ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_EraseBackupSubSectorCore(u32 dst, u32 len);

/*---------------------------------------------------------------------------*
  Name:         CARDi_EraseChipCore

  Description:  デバイスへのチップ消去コマンド全体.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_EraseChipCore(void);

/*---------------------------------------------------------------------------*
  Name:         CARDi_SetWriteProtectCore

  Description:  デバイスへのライトプロテクトコマンド全体.

  Arguments:    stat              設定するプロテクトフラグ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_SetWriteProtectCore(u16 stat);


#endif  /*__NITRO_CARD_SPI_H__*/

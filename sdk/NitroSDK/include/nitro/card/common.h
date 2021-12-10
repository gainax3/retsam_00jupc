/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CARD - include
  File:     common.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/


#if	!defined(NITRO_CARD_COMMON_H_)
#define NITRO_CARD_COMMON_H_


#include <nitro/misc.h>
#include <nitro/types.h>
#include <nitro/memorymap.h>
#include <nitro/mi/dma.h>
#include <nitro/os.h>


/*****************************************************************************/
/* constant */

/* API result code */
typedef enum
{
    CARD_RESULT_SUCCESS = 0,
    CARD_RESULT_FAILURE,
    CARD_RESULT_INVALID_PARAM,
    CARD_RESULT_UNSUPPORTED,
    CARD_RESULT_TIMEOUT,
    CARD_RESULT_ERROR,
    CARD_RESULT_NO_RESPONSE,
    CARD_RESULT_CANCELED
}
CARDResult;

/* for CARD_IdentifyBackup() */
#define	CARD_BACKUP_TYPE_DEVICE_SHIFT	0
#define	CARD_BACKUP_TYPE_DEVICE_MASK	0xFF
#define	CARD_BACKUP_TYPE_DEVICE_EEPROM	1
#define	CARD_BACKUP_TYPE_DEVICE_FLASH	2
#define	CARD_BACKUP_TYPE_DEVICE_FRAM	3
#define	CARD_BACKUP_TYPE_SIZEBIT_SHIFT	8
#define	CARD_BACKUP_TYPE_SIZEBIT_MASK	0xFF
#define	CARD_BACKUP_TYPE_VENDER_SHIFT	16
#define	CARD_BACKUP_TYPE_VENDER_MASK	0xFF
#define	CARD_BACKUP_TYPE_DEFINE(type, size, vender)	\
	(((CARD_BACKUP_TYPE_DEVICE_ ## type) << CARD_BACKUP_TYPE_DEVICE_SHIFT) |	\
	((size) << CARD_BACKUP_TYPE_SIZEBIT_SHIFT) |	\
	((vender) << CARD_BACKUP_TYPE_VENDER_SHIFT))

/* バックアップデバイスタイプ */
typedef enum
{
    CARD_BACKUP_TYPE_EEPROM_4KBITS = CARD_BACKUP_TYPE_DEFINE(EEPROM, 9, 0),
    CARD_BACKUP_TYPE_EEPROM_64KBITS = CARD_BACKUP_TYPE_DEFINE(EEPROM, 13, 0),
    CARD_BACKUP_TYPE_EEPROM_512KBITS = CARD_BACKUP_TYPE_DEFINE(EEPROM, 16, 0),
    CARD_BACKUP_TYPE_FLASH_2MBITS = CARD_BACKUP_TYPE_DEFINE(FLASH, 18, 0),
    CARD_BACKUP_TYPE_FLASH_4MBITS = CARD_BACKUP_TYPE_DEFINE(FLASH, 19, 0),
    CARD_BACKUP_TYPE_FLASH_8MBITS = CARD_BACKUP_TYPE_DEFINE(FLASH, 20, 0),
    CARD_BACKUP_TYPE_FLASH_16MBITS = CARD_BACKUP_TYPE_DEFINE(FLASH, 21, 0),
    CARD_BACKUP_TYPE_FLASH_64MBITS = CARD_BACKUP_TYPE_DEFINE(FLASH, 23, 0),
    CARD_BACKUP_TYPE_FRAM_256KBITS = CARD_BACKUP_TYPE_DEFINE(FRAM, 15, 0),
    CARD_BACKUP_TYPE_NOT_USE = 0
}
CARDBackupType;

#define CARD_BACKUP_TYPE_FLASH_64MBITS_EX (CARDBackupType)CARD_BACKUP_TYPE_DEFINE(FLASH, 23, 1)

/* カードスレッドのデフォルト優先レベル */
#define	CARD_THREAD_PRIORITY_DEFAULT	4


/* コンポーネントおよび ensata エミュレータへの PXI コマンドリクエスト (内部使用) */
typedef enum
{
    CARD_REQ_INIT = 0,                 /* initialize (setting from ARM9) */
    CARD_REQ_ACK,                      /* request done (acknowledge from ARM7) */
    CARD_REQ_IDENTIFY,                 /* CARD_IdentifyBackup */
    CARD_REQ_READ_ID,                  /* CARD_ReadRomID (TEG && ARM9) */
    CARD_REQ_READ_ROM,                 /* CARD_ReadRom (TEG && ARM9) */
    CARD_REQ_WRITE_ROM,                /* (reserved) */
    CARD_REQ_READ_BACKUP,              /* CARD_ReadBackup */
    CARD_REQ_WRITE_BACKUP,             /* CARD_WriteBackup */
    CARD_REQ_PROGRAM_BACKUP,           /* CARD_ProgramBackup */
    CARD_REQ_VERIFY_BACKUP,            /* CARD_VerifyBackup */
    CARD_REQ_ERASE_PAGE_BACKUP,        /* CARD_EraseBackupPage */
    CARD_REQ_ERASE_SECTOR_BACKUP,      /* CARD_EraseBackupSector */
    CARD_REQ_ERASE_CHIP_BACKUP,        /* CARD_EraseBackupChip */
    CARD_REQ_READ_STATUS,              /* CARD_ReadStatus */
    CARD_REQ_WRITE_STATUS,             /* CARD_WriteStatus */
    CARD_REQ_ERASE_SUBSECTOR_BACKUP,   /* CARD_EraseBackupSubSector */
    CARD_REQ_MAX
}
CARDRequest;

/* コマンドリクエストの動作タイプ (内部使用) */
typedef enum
{
    CARD_REQUEST_MODE_RECV,            /* データ受信 */
    CARD_REQUEST_MODE_SEND,            /* データ送信 (単発のベリファイを含む) */
    CARD_REQUEST_MODE_SEND_VERIFY,     /* データ送信 + ベリファイ */
    CARD_REQUEST_MODE_SPECIAL          /* セクタ消去などの特殊操作 */
}
CARDRequestMode;

/* リトライ最大回数 */
#define	CARD_RETRY_COUNT_MAX	10

// PXI protocol definition
#define CARD_PXI_COMMAND_MASK              0x0000003f   // command part
#define CARD_PXI_COMMAND_SHIFT             0
#define CARD_PXI_COMMAND_PARAM_MASK        0x01ffffc0   // parameter part
#define CARD_PXI_COMMAND_PARAM_SHIFT       6

//---- PXI command
#define CARD_PXI_COMMAND_TERMINATE         0x0001       // arm9->arm7 terminate command
#define CARD_PXI_COMMAND_PULLED_OUT        0x0011       // arm7->arm9 pulled out message


/*****************************************************************************/
/* function */


#if	defined(__cplusplus)
extern  "C"
{
#endif


/* card-system settings ******************************************************/


/*---------------------------------------------------------------------------*
  Name:         CARD_Init

  Description:  initialize CARD library.
                this function is called in OS_Init().

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
    void    CARD_Init(void);


/*---------------------------------------------------------------------------*
  Name:         CARD_IsAvailable

  Description:  check if CARD library is ready

  Arguments:    None.

  Returns:      If CARD library is ready, True.
 *---------------------------------------------------------------------------*/
    BOOL    CARD_IsAvailable(void);


/*---------------------------------------------------------------------------*
  Name:         CARD_IsEnabled

  Description:  check if CARD is accessable

  Arguments:    None.

  Returns:      Return card access permission.
 *---------------------------------------------------------------------------*/
    BOOL    CARD_IsEnabled(void);


/*---------------------------------------------------------------------------*
  Name:         CARD_CheckEnabled

  Description:  Terminate program if CARD is not accessable

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
    void    CARD_CheckEnabled(void);


/*---------------------------------------------------------------------------*
  Name:         CARD_Enable

  Description:  Set card access permission mode.

  Arguments:    enable       permission mode to be set.

  Returns:      None.
 *---------------------------------------------------------------------------*/
    void    CARD_Enable(BOOL enable);


/*---------------------------------------------------------------------------*
  Name:         CARD_GetThreadPriority

  Description:  get current priority of internal thread which deal with
                asynchronous CARD-ROM tasks.

  Arguments:    None.

  Returns:      priority of internal thread.
 *---------------------------------------------------------------------------*/
    u32     CARD_GetThreadPriority(void);


/*---------------------------------------------------------------------------*
  Name:         CARD_SetThreadPriority

  Description:  set priority of internal thread which deal with
                asynchronous CARD-ROM tasks.

  Arguments:    None.

  Returns:      priority of internal thread.
 *---------------------------------------------------------------------------*/
    u32     CARD_SetThreadPriority(u32 prior);


/*---------------------------------------------------------------------------*
  Name:         CARD_GetResultCode

  Description:  get result code of latest command.

  Arguments:    None.

  Returns:      result code of latest command.
 *---------------------------------------------------------------------------*/
    CARDResult CARD_GetResultCode(void);


/*---------------------------------------------------------------------------*
  Name:         CARD_GetRomHeader

  Description:  get data buffer which contains own ROM header information.
                if wireless-downloaded program call this function,
                the data is not equal to HW_ROM_HEADER_BUF's.

  Arguments:    None.

  Returns:      pointer of CARD-ROM header information buffer.
 *---------------------------------------------------------------------------*/
    const u8 *CARD_GetRomHeader(void);

/*---------------------------------------------------------------------------*
  Name:         CARD_GetCacheFlushThreshold

  Description:  キャッシュ無効化を部分的に行うか全体へ行うかの閾値を取得.

  Arguments:    icache            命令キャッシュの無効化閾値を格納するポインタ.
                                  NULLであれば無視される.
                dcache            データキャッシュの無効化閾値を格納するポインタ.
                                  NULLであれば無視される.

  Returns:      None.
 *---------------------------------------------------------------------------*/
    void    CARD_GetCacheFlushThreshold(u32 *icache, u32 *dcache);

/*---------------------------------------------------------------------------*
  Name:         CARD_SetCacheFlushThreshold

  Description:  キャッシュ無効化を部分的に行うか全体へ行うかの閾値を設定.

  Arguments:    icache            命令キャッシュの無効化閾値.
                dcache            データキャッシュの無効化閾値.

  Returns:      None.
 *---------------------------------------------------------------------------*/
    void    CARD_SetCacheFlushThreshold(u32 icache, u32 dcache);


/* backup settings ***********************************************************/


/*---------------------------------------------------------------------------*
  Name:         CARD_IdentifyBackup

  Description:  identify the type of backup device on NITRO-CARD.

  Arguments:    type of backup device on NITRO-CARD.

  Returns:      if the reading test operation is successful, TRUE.
 *---------------------------------------------------------------------------*/
    BOOL    CARD_IdentifyBackup(CARDBackupType type);


/*---------------------------------------------------------------------------*
  Name:         CARD_GetBackupTotalSize

  Description:  get total size of backup device.

  Arguments:    None.

  Returns:      total size of backup device.
 *---------------------------------------------------------------------------*/
    u32     CARD_GetBackupTotalSize(void);


/*---------------------------------------------------------------------------*
  Name:         CARD_GetBackupSectorSize

  Description:  get sector size of backup device.

  Arguments:    None.

  Returns:      sector size of backup device.
 *---------------------------------------------------------------------------*/
    u32     CARD_GetBackupSectorSize(void);


/*---------------------------------------------------------------------------*
  Name:         CARD_GetBackupPageSize

  Description:  get page size of backup device.

  Arguments:    None.

  Returns:      page size of backup device.
 *---------------------------------------------------------------------------*/
    u32     CARD_GetBackupPageSize(void);


/*---------------------------------------------------------------------------*
  Name:         CARD_GetCurrentBackupType

  Description:  get type of current specified backup device.

  Arguments:    None.

  Returns:      type of current backup device.
 *---------------------------------------------------------------------------*/
    CARDBackupType CARD_GetCurrentBackupType(void);


#if	defined(__cplusplus)
}                                      /* extern "C" */
#endif


/* NITRO_CARD_COMMON_H_ */
#endif

/*---------------------------------------------------------------------------*

  $Log: common.h,v $
  Revision 1.29  2007/11/13 04:20:24  yosizaki
  support erase-SUBSECTOR.

  Revision 1.28  2007/10/04 13:08:51  yosizaki
  add test commands.

  Revision 1.27  2007/09/14 02:57:31  yosizaki
  add CARD_BACKUP_TYPE_FLASH_64MBITS_EX

  Revision 1.26  2007/08/22 08:30:18  yosizaki
  add CARD_BACKUP_TYPE_FLASH_64MBITS

  Revision 1.25  2007/05/28 04:14:17  yosizaki
  add CARD_SetCacheFlushThreshold.

  Revision 1.24  2007/02/23 01:29:43  yosizaki
  upport CARD_BACKUP_TYPE_FLASH_16MBITS.

  Revision 1.23  2006/04/10 04:12:00  yosizaki
  add card request mode.

  Revision 1.22  2006/04/03 01:58:22  yosizaki
  add CARD_BACKUP_TYPE_FLASH_8MBITS.

  Revision 1.21  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.20  2005/10/20 05:10:56  yosizaki
  add CARD_BACKUP_TYPE_FLASH_4MBITS.

  Revision 1.19  2005/09/02 07:15:14  yosizaki
  add CARDResult codes.

  Revision 1.18  2005/07/05 00:19:39  yosizaki
  add FRAM.

  Revision 1.17  2005/06/15 09:33:27  yosizaki
  add CARD_BACKUP_TYPE_EEPROM_512KBITS.

  Revision 1.16  2005/05/12 11:21:07  yosizaki
  add some internal parameters.

  Revision 1.15  2005/04/05 07:21:55  yosizaki
  add comment about CARD_RESULT_TIMEOUT.

  Revision 1.14  2005/03/01 01:57:00  yosizaki
  copyright の年を修正.

  Revision 1.13  2005/02/28 05:26:01  yosizaki
  do-indent.

  Revision 1.12  2004/12/15 09:52:44  yasu
  CARD アクセスイネーブラ機構の追加

  Revision 1.11  2004/12/08 12:15:52  yosizaki
  remove CARD_BACKUP_TYPE_EEPROM_512KBITS.

  Revision 1.10  2004/12/07 10:52:52  yosizaki
  add CARD_BACKUP_TYPE_EEPROM_512KBITS.
  add comments.

  Revision 1.9  2004/10/08 14:31:52  yosizaki
  add CARD_GetRomHeader().

  Revision 1.8  2004/09/27 13:40:49  yada
  add some definitions

  Revision 1.7  2004/09/09 07:11:21  yosizaki
  delete CARD_BACKUP_TYPE_EEPROM_ 512k, 1M.

  Revision 1.6  2004/08/18 13:40:49  yosizaki
  add FLASH 2Mbit.

  Revision 1.5  2004/08/18 08:19:19  yosizaki
  change CARDBackupType format.

  Revision 1.4  2004/07/28 13:18:29  yosizaki
  asynchronous mode support.

  Revision 1.3  2004/07/23 08:31:58  yosizaki
  add new function.

  Revision 1.2  2004/07/19 13:10:34  yosizaki
  add task thread.

  Revision 1.1  2004/07/05 08:10:48  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

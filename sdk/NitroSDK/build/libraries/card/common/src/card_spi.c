/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CARD - libraries
  File:     card_spi.c

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: card_spi.c,v $
  Revision 1.34  2007/11/13 04:22:01  yosizaki
  support erase-SUBSECTOR.

  Revision 1.33  2007/10/04 13:11:37  yosizaki
  add test commands.

  Revision 1.32  2007/09/14 02:57:16  yosizaki
  add CARD_BACKUP_TYPE_FLASH_64MBITS_EX

  Revision 1.31  2007/07/04 08:07:01  yosizaki
  add FLASH 32Mb.

  Revision 1.30  2007/02/20 00:28:12  kitase_hirotake
  indent source

  Revision 1.29  2006/11/16 04:32:05  yosizaki
  change spec of FLASH16M.

  Revision 1.28  2006/05/15 08:51:00  yosizaki
  add status check for FLASH16M/64M

  Revision 1.27  2006/04/28 07:43:27  yosizaki
  add DeviceCaps.

  Revision 1.26  2006/04/10 04:13:50  yosizaki
  add comments.
  small fix.

  Revision 1.25  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.24  2005/11/30 04:03:17  yosizaki
  fix warning.

  Revision 1.23  2005/11/28 02:10:59  yosizaki
  fix about timeout check.

  Revision 1.22  2005/10/13 02:06:59  yosizaki
  change WriteStatus.

  Revision 1.21  2005/09/30 01:09:01  yosizaki
  change CARDi_InitStatusRegister().

  Revision 1.20  2005/09/30 00:51:25  yosizaki
  fix about sector-size.
  change spceification of FLASH 4Mb/8Mb.

  Revision 1.19  2005/09/02 07:35:55  yosizaki
  change CARDi_WaitPrevCommand() to verify NO_RESPONSE.

  Revision 1.18  2005/08/04 15:02:08  yosizaki
  add CARDi_InitStatusRegister().

  Revision 1.17  2005/02/28 05:26:02  yosizaki
  do-indent.

  Revision 1.16  2005/02/18 11:20:29  yosizaki
  fix around hidden warnings.

  Revision 1.15  2005/01/31 07:23:26  yosizaki
  add FLASH 4Mb/8Mb.

  Revision 1.14  2005/01/18 09:35:48  yosizaki
  fix page size of EEPROM512kb.

  Revision 1.13  2005/01/14 08:49:08  yosizaki
  remove waste WriteEnable before read-command.
  fix addressing-width (from 3 to 2).

  Revision 1.12  2004/12/13 05:07:42  yosizaki
  add implement of FRAM64kb.

  Revision 1.11  2004/12/08 12:41:58  yosizaki
  remove cardi_device and change CARDi_IdentifyBackupCore.

  Revision 1.10  2004/09/10 10:33:26  yosizaki
  set CARDi_WaitPrevCommand timeout 50ms.

  Revision 1.9  2004/09/06 09:20:49  yosizaki
  fix backup access error at 1st time.

  Revision 1.8  2004/09/02 11:54:42  yosizaki
  fix CARD include directives.

  Revision 1.7  2004/08/23 10:38:10  yosizaki
  add write-command.
  add retry count on writing timeout.

  Revision 1.6  2004/08/18 08:19:02  yosizaki
  change CARDBackupType format.

  Revision 1.5  2004/07/28 13:18:06  yosizaki
  asynchronous mode support.

  Revision 1.4  2004/07/23 08:28:42  yosizaki
  change backup-routine to ARM7 only.

  Revision 1.3  2004/07/19 13:15:42  yosizaki
  add CARDi_ProgramBackupCore.

  Revision 1.2  2004/07/05 08:37:21  yosizaki
  change CardBackupType to CARDBackupType.

  Revision 1.1  2004/06/28 01:52:48  yosizaki
  (none)

 *---------------------------------------------------------------------------*/


#include <nitro.h>

#include "../include/card_common.h"
#include "../include/card_spi.h"

/* CARD-SPI の制御 */


/******************************************************************************/
/* 定数 */

/* reg_MI_MCCNT0 各ビット*/

#define MCCNT0_SPI_CLK_MASK	0x0003 /* ボーレート設定マスク */
#define MCCNT0_SPI_CLK_4M	0x0000 /* ボーレート 4.19MHz */
#define MCCNT0_SPI_CLK_2M	0x0001 /* ボーレート 2.10MHz */
#define MCCNT0_SPI_CLK_1M	0x0002 /* ボーレート 1.05MHz */
#define MCCNT0_SPI_CLK_524K	0x0003 /* ボーレート 524kHz */
#define MCCNT0_SPI_BUSY		0x0080 /* SPI ビジーフラグ */
#define	MMCNT0_SEL_MASK		0x2000 /* CARD ROM / SPI 選択マスク */
#define	MMCNT0_SEL_CARD		0x0000 /* CARD ROM 選択 */
#define	MMCNT0_SEL_SPI		0x2000 /* CARD SPI 選択 */
#define MCCNT0_INT_MASK		0x4000 /* 転送完了割り込みマスク */
#define MCCNT0_INT_ON		0x4000 /* 転送完了割り込み有効 */
#define MCCNT0_INT_OFF		0x0000 /* 転送完了割り込み無効 */
#define MCCNT0_MASTER_MASK	0x8000 /* CARD マスターマスク */
#define MCCNT0_MASTER_ON	0x8000 /* CARD マスター ON */
#define MCCNT0_MASTER_OFF	0x0000 /* CARD マスター OFF */


/******************************************************************************/
/* 変数 */

typedef struct
{                                      /* SPI 内部管理パラメータ. */
    u32     rest_comm;                 /* 全送信バイト数の残り. */
    u32     src;                       /* 転送元 */
    u32     dst;                       /* 転送先 */
    BOOL    cmp;                       /* 比較結果 */
}
CARDiParam;

static CARDiParam cardi_param;


/******************************************************************************/
/* 関数 */

static BOOL CARDi_CommandCheckBusy(void);
static void CARDi_CommArray(const void *src, void *dst, u32 n, void (*func) (CARDiParam *));
static void CARDi_CommReadCore(CARDiParam * p);
static void CARDi_CommWriteCore(CARDiParam * p);
static void CARDi_CommVerifyCore(CARDiParam * p);

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommArrayRead

  Description:  リードコマンドの後続読み出し処理.

  Arguments:    dst               読み出し先メモリ.
                len               読み出しサイズ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARDi_CommArrayRead(void *dst, u32 len)
{
    CARDi_CommArray(NULL, dst, len, CARDi_CommReadCore);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommArrayWrite

  Description:  ライトコマンドの後続書き込み処理.

  Arguments:    dst               書き込み元メモリ.
                len               書き込みサイズ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARDi_CommArrayWrite(const void *src, u32 len)
{
    CARDi_CommArray(src, NULL, len, CARDi_CommWriteCore);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommArrayVerify

  Description:  (比較のための)リードコマンドの後続比較処理.

  Arguments:    src               比較元メモリ.
                len               比較サイズ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARDi_CommArrayVerify(const void *src, u32 len)
{
    CARDi_CommArray(src, NULL, len, CARDi_CommVerifyCore);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_EnableSpi

  Description:  CARD-SPI を有効にする.

  Arguments:    cont              連続転送フラグ. (CSPI_CONTINUOUS_ON or OFF)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARDi_EnableSpi(u32 cont)
{
    /*
     * 将来クロックスピードが遅いデバイスが登場したら
     * MCCNT0_SPI_CLK_4M をプロパティ配列に追加して動的変更.
     */
    const u16 ctrl = (u16)(MCCNT0_MASTER_ON |
                           MCCNT0_INT_OFF | MMCNT0_SEL_SPI | MCCNT0_SPI_CLK_4M | cont);
    reg_MI_MCCNT0 = ctrl;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_WaitBusy

  Description:  CARD-SPI の完了を待つ.

  Arguments:    None

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARDi_WaitBusy(void)
{
    while ((reg_MI_MCCNT0 & MCCNT0_SPI_BUSY) != 0)
    {
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommandBegin

  Description:  コマンド発行開始の宣言.

  Arguments:    len               発行するコマンド長.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_CommandBegin(int len)
{
    cardi_param.rest_comm = (u32)len;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommandEnd

  Description:  コマンド送信完了処理.

  Arguments:    force_wait     強制ウェイト時間 [ms]
                timeout        タイムアウト時間 [ms]

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_CommandEnd(u32 force_wait, u32 timeout)
{
    if (force_wait + timeout > 0)
    {
        /*
         * 強制ウェイト時間があればその分だけ待機.
         * 最初の話と違って FLASH もウェイトが強制された.
         */
        if (force_wait > 0)
        {
            OS_Sleep(force_wait);
        }
        if (timeout > 0)
        {
            /*
             * PageWrite コマンドのみ「前半だけ強制ウェイト」
             * なのでこのような特殊なループになる.
             */
            int     rest = (int)(timeout - force_wait);
            while (!CARDi_CommandCheckBusy() && (rest > 0))
            {
                int     interval = (rest < 5) ? rest : 5;
                OS_Sleep((u32)interval);
                rest -= interval;
            }
        }
        /*
         * その他のコマンドは指定時間ウェイト済みなので
         * ReadStatus は 1 回だけで良い.
         */
        /* ここまできてビジーならタイムアウト */
        if (!CARDi_CommandCheckBusy())
        {
            cardi_common.cmd->result = CARD_RESULT_TIMEOUT;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommandReadStatus

  Description:  リードステータス

  Arguments:    None

  Returns:      ステータス値
 *---------------------------------------------------------------------------*/
u8 CARDi_CommandReadStatus(void)
{
    const u8    buf = COMM_READ_STATUS;
    u8          dst;
    CARDi_CommandBegin(1 + 1);
    CARDi_CommArrayWrite(&buf, 1);
    CARDi_CommArrayRead(&dst, 1);
    CARDi_CommandEnd(0, 0);
    return dst;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommandCheckBusy

  Description:  デバイスのビジーが解消されたかリードステータスコマンドで判定.

  Arguments:    None

  Returns:      ビジーでなければ TRUE.
 *---------------------------------------------------------------------------*/
static BOOL CARDi_CommandCheckBusy(void)
{
    return ((CARDi_CommandReadStatus() & COMM_STATUS_WIP_BIT) == 0);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_WaitPrevCommand

  Description:  コマンド発行前のビジーチェック.
                前回のコマンドによってビジー状態であれば 50[ms] まで待つ.
                (ライブラリの実装とカードの接触が正常な限り通常ありえない)

  Arguments:    None.

  Returns:      コマンドが正しく発行可能であれば TRUE.
 *---------------------------------------------------------------------------*/
static BOOL CARDi_WaitPrevCommand(void)
{
    CARDi_CommandEnd(0, 50);
    /* ここでタイムアウトの場合は無応答 */
    if (cardi_common.cmd->result == CARD_RESULT_TIMEOUT)
    {
        cardi_common.cmd->result = CARD_RESULT_NO_RESPONSE;
        return FALSE;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommArray

  Description:  コマンド発行の共通処理.

  Arguments:    src               処理元のメモリ. (不使用ならNULL)
                dst               処理先のメモリ. (不使用ならNULL)
                len               処理サイズ.
                func              処理内容.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_CommArray(const void *src, void *dst, u32 len, void (*func) (CARDiParam *))
{
    CARDiParam *const p = &cardi_param;
    p->src = (u32)src;
    p->dst = (u32)dst;
    CARDi_EnableSpi(CSPI_CONTINUOUS_ON);
    for (; len > 0; --len)
    {
        if (!--p->rest_comm)
        {
            CARDi_EnableSpi(CSPI_CONTINUOUS_OFF);
        }
        CARDi_WaitBusy();
        (*func) (p);
    }
    if (!p->rest_comm)
    {
        reg_MI_MCCNT0 = (u16)(MCCNT0_MASTER_OFF | MCCNT0_INT_OFF);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommReadCore

  Description:  1バイトリード処理.

  Arguments:    p                 コマンドパラメータ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_CommReadCore(CARDiParam * p)
{
    reg_MI_MCD0 = 0;
    CARDi_WaitBusy();
    MI_WriteByte((void *)p->dst, (u8)reg_MI_MCD0);
    ++p->dst;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommWriteCore

  Description:  1バイトライト処理.

  Arguments:    p                 コマンドパラメータ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_CommWriteCore(CARDiParam * p)
{
    vu16    tmp;
    reg_MI_MCD0 = (u16)MI_ReadByte((void *)p->src);
    ++p->src;
    CARDi_WaitBusy();
    tmp = reg_MI_MCD0;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommVerifyCore

  Description:  1バイト比較処理.

  Arguments:    p                 コマンドパラメータ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_CommVerifyCore(CARDiParam * p)
{
    reg_MI_MCD0 = 0;
    CARDi_WaitBusy();
    /*
     * リードして一致しなければ中断.
     * ただし連続転送を抜けなければいけないので
     * 少なくとも 1 回だけ余計に読む必要がある.
     */
    if ((u8)reg_MI_MCD0 != MI_ReadByte((void *)p->src))
    {
        p->cmp = FALSE;
        if (p->rest_comm > 1)
        {
            p->rest_comm = 1;
        }
    }
    ++p->src;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_WriteEnable

  Description:  デバイスの書き込み操作有効化. (Write 系コマンドの前に毎回必要)

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_WriteEnable(void)
{
    static const u8 arg[1] = { COMM_WRITE_ENABLE, };
    CARDi_CommandBegin(sizeof(arg));
    CARDi_CommArrayWrite(arg, sizeof(arg));
    CARDi_CommandEnd(0, 0);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_SendSpiAddressingCommand

  Description:  アドレス指定コマンドの設定.

  Arguments:    addr              捜査対象となるデバイス上のアドレス.
                mode              発行するコマンド.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_SendSpiAddressingCommand(u32 addr, u32 mode)
{
    const u32 width = cardi_common.cmd->spec.addr_width;
    u32     addr_cmd;
    switch (width)
    {
    case 1:
        /* 4kbit デバイスは [A:8] がコマンドの一部になる */
        addr_cmd = (u32)(mode | ((addr >> 5) & 0x8) | ((addr & 0xFF) << 8));
        break;
    case 2:
        addr_cmd = (u32)(mode | (addr & 0xFF00) | ((addr & 0xFF) << 16));
        break;
    case 3:
        addr_cmd = (u32)(mode |
                         ((addr >> 8) & 0xFF00) | ((addr & 0xFF00) << 8) | ((addr & 0xFF) << 24));
        break;
    default:
        SDK_ASSERT(FALSE);
        break;
    }
    CARDi_CommArrayWrite(&addr_cmd, (u32)(1 + width));
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_InitStatusRegister

  Description:  FRAM/EEPROM の場合, 起動時にステータスレジスタの補正処理を行う.
                ・FRAM は電源投入時にライトプロテクトの変化が起こりうるため.
                ・EEPROM は納入時に初期値不正がありうるため.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_InitStatusRegister(void)
{
    /*
     * ステータスレジスタが異常な初期値を取りうるデバイスに対しては
     * 初回使用時に適宜補正する.
     */
    const u8 stat = cardi_common.cmd->spec.initial_status;
    if (stat != 0xFF)
    {
        static BOOL status_checked = FALSE;
        if (!status_checked)
        {
            if (CARDi_CommandReadStatus() != stat)
            {
                CARDi_SetWriteProtectCore(stat);
            }
            status_checked = TRUE;
        }
    }
}


/********************************************************************/
/*
 * 内部直接処理.
 * この段階ではすでに排他やリクエスト等が全て完了している.
 * サイズの制限についてもすでに考慮済みのものとする.
 */

/*---------------------------------------------------------------------------*
  Name:         CARDi_IdentifyBackupCore

  Description:  デバイスタイプ特定.

  Arguments:    type              特定するデバイスタイプ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_IdentifyBackupCore(CARDBackupType type)
{
    /*
     * 取得したパラメータを CARDiCommandArg へ保存.
     * これは最終的にテーブルを廃止することで完成する.
     */
    {
        CARDiCommandArg *const p = cardi_common.cmd;

        /* まず全パラメータをクリアして NOT_USE 状態にする */
        MI_CpuFill8(&p->spec, 0, sizeof(p->spec));
        p->type = type;
        p->spec.caps = (CARD_BACKUP_CAPS_AVAILABLE | CARD_BACKUP_CAPS_READ_STATUS);
        if (type != CARD_BACKUP_TYPE_NOT_USE)
        {
            /*
             * デバイスタイプ, 総容量, ベンダは type から取得可能.
             * ベンダ番号は, 同一タイプで複数メーカーが採用され
             * かつ不具合などの理由で区別する必要が生じた場合以外 0.
             */
            const u32 size = (u32)(1 << ((type >> CARD_BACKUP_TYPE_SIZEBIT_SHIFT) &
                                         CARD_BACKUP_TYPE_SIZEBIT_MASK));
            const int device =
                ((type >> CARD_BACKUP_TYPE_DEVICE_SHIFT) & CARD_BACKUP_TYPE_DEVICE_MASK);
            const int vender =
                ((type >> CARD_BACKUP_TYPE_VENDER_SHIFT) & CARD_BACKUP_TYPE_VENDER_MASK);

            p->spec.total_size = size;
            /* ステータスレジスタの補正が不要なら 0xFF. (これが通常) */
            p->spec.initial_status = 0xFF;
            if (device == CARD_BACKUP_TYPE_DEVICE_EEPROM)
            {
                switch (size)
                {
                default:
                    goto invalid_type;
                case 0x000200:        // CARD_BACKUP_TYPE_EEPROM_4KBITS
                    p->spec.page_size = 0x10;
                    p->spec.addr_width = 1;
                    p->spec.program_page = 5;
                    p->spec.initial_status = 0xF0;
                    break;
                case 0x002000:        // CARD_BACKUP_TYPE_EEPROM_64KBITS
                    p->spec.page_size = 0x0020;
                    p->spec.addr_width = 2;
                    p->spec.program_page = 5;
                    p->spec.initial_status = 0x00;
                    break;
                case 0x010000:        // CARD_BACKUP_TYPE_EEPROM_512KBITS
                    p->spec.page_size = 0x0080;
                    p->spec.addr_width = 2;
                    p->spec.program_page = 10;
                    p->spec.initial_status = 0x00;
                    break;
/*
				case 0x020000:	// CARD_BACKUP_TYPE_EEPROM_1MBITS
					p->spec.page_size = 0x0080;
					p->spec.addr_width = 3;
					p->spec.program_page = 10;
					break;
*/
                }
                p->spec.sect_size = p->spec.page_size;
                p->spec.caps |= CARD_BACKUP_CAPS_READ;
                p->spec.caps |= CARD_BACKUP_CAPS_PROGRAM;
                p->spec.caps |= CARD_BACKUP_CAPS_VERIFY;
                p->spec.caps |= CARD_BACKUP_CAPS_WRITE_STATUS;
            }
            else if (device == CARD_BACKUP_TYPE_DEVICE_FLASH)
            {
                switch (size)
                {
                default:
                    goto invalid_type;
                case 0x040000:        // CARD_BACKUP_TYPE_FLASH_2MBITS
                case 0x080000:        // CARD_BACKUP_TYPE_FLASH_4MBITS
                case 0x100000:        // CARD_BACKUP_TYPE_FLASH_8MBITS
                    p->spec.write_page = 25;
                    p->spec.write_page_total = 300;
                    p->spec.erase_page = 300;
                    p->spec.erase_sector = 5000;
                    p->spec.caps |= CARD_BACKUP_CAPS_WRITE;
                    p->spec.caps |= CARD_BACKUP_CAPS_ERASE_PAGE;
                    break;
                case 0x200000:        // CARD_BACKUP_TYPE_FLASH_16MBITS
                    p->spec.write_page = 23;
                    p->spec.write_page_total = 300;
                    p->spec.erase_sector = 500;
                    p->spec.erase_sector_total = 5000;
                    p->spec.erase_chip = 10000;
                    p->spec.erase_chip_total = 60000;
                    p->spec.initial_status = 0x00;
                    p->spec.caps |= CARD_BACKUP_CAPS_WRITE;
                    p->spec.caps |= CARD_BACKUP_CAPS_ERASE_PAGE;
                    p->spec.caps |= CARD_BACKUP_CAPS_ERASE_CHIP;
                    p->spec.caps |= CARD_BACKUP_CAPS_WRITE_STATUS;
                    break;
                case 0x400000:        // CARD_BACKUP_TYPE_FLASH_32MBITS
                    p->spec.erase_sector = 600;
                    p->spec.erase_sector_total = 3000;
                    p->spec.erase_subsector = 70;
                    p->spec.erase_subsector_total = 150;
                    p->spec.erase_chip = 23000;
                    p->spec.erase_chip_total = 800000;
                    p->spec.initial_status = 0x00;
                    p->spec.subsect_size = 0x1000;
                    p->spec.caps |= CARD_BACKUP_CAPS_ERASE_SUBSECTOR;
                    p->spec.caps |= CARD_BACKUP_CAPS_ERASE_CHIP;
                    p->spec.caps |= CARD_BACKUP_CAPS_WRITE_STATUS;
                    break;
                case 0x800000:
                    if (vender == 0)  // CARD_BACKUP_TYPE_FLASH_64MBITS
                    {
                        p->spec.erase_sector = 1000;
                        p->spec.erase_sector_total = 3000;
                        p->spec.erase_chip = 68000;
                        p->spec.erase_chip_total = 160000;
                        p->spec.initial_status = 0x00;
                        p->spec.caps |= CARD_BACKUP_CAPS_ERASE_CHIP;
                        p->spec.caps |= CARD_BACKUP_CAPS_WRITE_STATUS;
                    }
                    else if (vender == 1)   // CARD_BACKUP_TYPE_FLASH_64MBITS_EX
                    {
                        p->spec.erase_sector = 1000;
                        p->spec.erase_sector_total = 3000;
                        p->spec.erase_chip = 68000;
                        p->spec.erase_chip_total = 160000;
                        p->spec.initial_status = 0x84;
                        p->spec.caps |= CARD_BACKUP_CAPS_ERASE_CHIP;
                        p->spec.caps |= CARD_BACKUP_CAPS_WRITE_STATUS;
                    }
                    break;
                }
                p->spec.sect_size = 0x010000;
                p->spec.page_size = 0x0100;
                p->spec.addr_width = 3;
                p->spec.program_page = 5;
                p->spec.caps |= CARD_BACKUP_CAPS_READ;
                p->spec.caps |= CARD_BACKUP_CAPS_PROGRAM;
                p->spec.caps |= CARD_BACKUP_CAPS_VERIFY;
                p->spec.caps |= CARD_BACKUP_CAPS_ERASE_SECTOR;
            }
            else if (device == CARD_BACKUP_TYPE_DEVICE_FRAM)
            {
                switch (size)
                {
                default:
                    goto invalid_type;
                case 0x002000:        // #CARD_BACKUP_TYPE_FRAM_64KBITS
                case 0x008000:        // #CARD_BACKUP_TYPE_FRAM_256KBITS
                    break;
                }
                p->spec.page_size = size;
                p->spec.sect_size = size;
                p->spec.addr_width = 2;
                p->spec.initial_status = 0x00;
                p->spec.caps |= CARD_BACKUP_CAPS_READ;
                p->spec.caps |= CARD_BACKUP_CAPS_PROGRAM;
                p->spec.caps |= CARD_BACKUP_CAPS_VERIFY;
                p->spec.caps |= CARD_BACKUP_CAPS_WRITE_STATUS;
            }
            else
            {
              invalid_type:
                p->type = CARD_BACKUP_TYPE_NOT_USE;
                p->spec.total_size = 0;
                cardi_common.cmd->result = CARD_RESULT_UNSUPPORTED;
                return;
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadBackupCore

  Description:  デバイスへのリードコマンド全体.

  Arguments:    src               読み出し元のデバイスオフセット.
                dst               読み出し先のメモリアドレス.
                len               読み出しサイズ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_ReadBackupCore(u32 src, void *dst, u32 len)
{
    if (CARDi_WaitPrevCommand())
    {
        CARDiCommandArg *const cmd = cardi_common.cmd;
        /* リードに関してはページ境界の制限が無いので一括処理 */
        CARDi_CommandBegin((int)(1 + cmd->spec.addr_width + len));
        CARDi_SendSpiAddressingCommand(src, COMM_READ_ARRAY);
        CARDi_CommArrayRead(dst, len);
        CARDi_CommandEnd(0, 0);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_ProgramBackupCore

  Description:  デバイスへのプログラム(消去無し書き込み)コマンド全体.

  Arguments:    dst               書き込み先のデバイスオフセット.
                src               書き込み元のメモリアドレス.
                len               書き込みサイズ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_ProgramBackupCore(u32 dst, const void *src, u32 len)
{
    if (CARDi_WaitPrevCommand())
    {
        CARDiCommandArg *const cmd = cardi_common.cmd;
        /* ライトはページ境界をまたがないように分割処理 */
        const u32 page = cmd->spec.page_size;
        while (len > 0)
        {
            const u32 mod = (u32)(dst & (page - 1));
            u32     size = page - mod;
            if (size > len)
            {
                size = len;
            }
            CARDi_WriteEnable();
            CARDi_CommandBegin((int)(1 + cmd->spec.addr_width + size));
            CARDi_SendSpiAddressingCommand(dst, COMM_PROGRAM_PAGE);
            CARDi_CommArrayWrite(src, size);
            CARDi_CommandEnd(cmd->spec.program_page, 0);
            if (cmd->result != CARD_RESULT_SUCCESS)
            {
                break;
            }
            src = (const void *)((u32)src + size);
            dst += size;
            len -= size;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_WriteBackupCore

  Description:  デバイスへのライト(消去 + プログラム)コマンド全体.

  Arguments:    dst               書き込み先のデバイスオフセット.
                src               書き込み元のメモリアドレス.
                len               書き込みサイズ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_WriteBackupCore(u32 dst, const void *src, u32 len)
{
    if (CARDi_WaitPrevCommand())
    {
        CARDiCommandArg *const cmd = cardi_common.cmd;
        /* ライトはページ境界をまたがないように分割処理 */
        const u32 page = cmd->spec.page_size;
        while (len > 0)
        {
            const u32 mod = (u32)(dst & (page - 1));
            u32     size = page - mod;
            if (size > len)
            {
                size = len;
            }
            CARDi_WriteEnable();
            CARDi_CommandBegin((int)(1 + cmd->spec.addr_width + size));
            CARDi_SendSpiAddressingCommand(dst, COMM_PAGE_WRITE);
            CARDi_CommArrayWrite(src, size);
            CARDi_CommandEnd(cmd->spec.write_page, cmd->spec.write_page_total);
            if (cmd->result != CARD_RESULT_SUCCESS)
            {
                break;
            }
            src = (const void *)((u32)src + size);
            dst += size;
            len -= size;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_VerifyBackupCore

  Description:  デバイスへのベリファイ(実際はリード + 比較処理)コマンド全体.

  Arguments:    dst               比較先のデバイスオフセット.
                src               比較元のメモリアドレス.
                len               比較サイズ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_VerifyBackupCore(u32 dst, const void *src, u32 len)
{
    if (CARDi_WaitPrevCommand())
    {
        CARDiCommandArg *const cmd = cardi_common.cmd;
        /* リードに関してはページ境界の制限が無いので一括処理 */
        cardi_param.cmp = TRUE;
        CARDi_CommandBegin((int)(1 + cmd->spec.addr_width + len));
        CARDi_SendSpiAddressingCommand(dst, COMM_READ_ARRAY);
        CARDi_CommArrayVerify(src, len);
        CARDi_CommandEnd(0, 0);
        if ((cmd->result == CARD_RESULT_SUCCESS) && !cardi_param.cmp)
        {
            cmd->result = CARD_RESULT_FAILURE;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_EraseBackupSectorCore

  Description:  デバイスへのセクタ消去コマンド全体.

  Arguments:    dst               消去先のデバイスオフセット.
                len               消去サイズ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_EraseBackupSectorCore(u32 dst, u32 len)
{
    CARDiCommandArg *const cmd = cardi_common.cmd;
    const u32 sector = cmd->spec.sect_size;
    /* 処理範囲がセクタの整数倍に整合していない場合は処理をしない */
    if (((dst | len) & (sector - 1)) != 0)
    {
        cmd->result = CARD_RESULT_INVALID_PARAM;
    }
    else if (CARDi_WaitPrevCommand())
    {
        /* セクタ境界単位で処理 */
        while (len > 0)
        {
            CARDi_WriteEnable();
            CARDi_CommandBegin((int)(1 + cmd->spec.addr_width + 0));
            CARDi_SendSpiAddressingCommand(dst, COMM_SECTOR_ERASE);
            CARDi_CommandEnd(cmd->spec.erase_sector, cmd->spec.erase_sector_total);
            if (cmd->result != CARD_RESULT_SUCCESS)
            {
                break;
            }
            dst += sector;
            len -= sector;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_EraseBackupSubSectorCore

  Description:  デバイスへのサブセクタ消去コマンド全体.

  Arguments:    dst               消去先のデバイスオフセット.
                len               消去サイズ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_EraseBackupSubSectorCore(u32 dst, u32 len)
{
    CARDiCommandArg *const cmd = cardi_common.cmd;
    const u32 sector = cmd->spec.subsect_size;
    /* 処理範囲がサブセクタの整数倍に整合していない場合は処理をしない */
    if (((dst | len) & (sector - 1)) != 0)
    {
        cmd->result = CARD_RESULT_INVALID_PARAM;
    }
    else if (CARDi_WaitPrevCommand())
    {
        /* セクタ境界単位で処理 */
        while (len > 0)
        {
            CARDi_WriteEnable();
            CARDi_CommandBegin((int)(1 + cmd->spec.addr_width + 0));
            CARDi_SendSpiAddressingCommand(dst, COMM_SUBSECTOR_ERASE);
            CARDi_CommandEnd(cmd->spec.erase_subsector, cmd->spec.erase_subsector_total);
            if (cmd->result != CARD_RESULT_SUCCESS)
            {
                break;
            }
            dst += sector;
            len -= sector;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_EraseChipCore

  Description:  デバイスへのチップ消去コマンド全体.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_EraseChipCore(void)
{
    if (CARDi_WaitPrevCommand())
    {
        CARDiCommandArg *const cmd = cardi_common.cmd;
        static const u8 arg[1] = { COMM_CHIP_ERASE, };
        CARDi_WriteEnable();
        CARDi_CommandBegin(sizeof(arg));
        CARDi_CommArrayWrite(arg, sizeof(arg));
        CARDi_CommandEnd(cmd->spec.erase_chip, cmd->spec.erase_chip_total);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_SetWriteProtectCore

  Description:  デバイスへのライトプロテクトコマンド全体.

  Arguments:    stat              設定するプロテクトフラグ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_SetWriteProtectCore(u16 stat)
{
    if (CARDi_WaitPrevCommand())
    {
        CARDiCommandArg *const cmd = cardi_common.cmd;
        int     retry_count = 10;
        u8      arg[2];
        arg[0] = COMM_WRITE_STATUS;
        arg[1] = (u8)stat;
        do
        {
            CARDi_WriteEnable();
            CARDi_CommandBegin(1 + 1);
            CARDi_CommArrayWrite(&arg, sizeof(arg));
            CARDi_CommandEnd(5, 0);
        }
        while ((cmd->result == CARD_RESULT_TIMEOUT) && (--retry_count > 0));
    }
}


#if	0

/********************************************************************/
/********************************************************************/
/* 検証中デバイス固有コマンド ***************************************/
/********************************************************************/
/********************************************************************/


/* ID リード */
static void CARDi_ReadIdCore(void)
{
    /*
     * EEPROM, FRAM にはこのコマンドが無い.
     * ST 製 2Mbit FLASH にもこのコマンドが無い?
     * 対応外のコマンドバイト送出時の動作定義が見当たらない.
     * とすると, このコマンドは 2Mbit 時は常に使用不可とするか
     * CARD_BACKUP_TYPE_FLASH_2MBITS_SANYO などと細分化するか.
     * いずれにせよそこまで事前に区分しているのに
     * ID が必要になるシーンがあるのだろうか...?
     * 余力があれば内部で正当性判定に使用予定.
     */
    cardi_common.cmd->result = CARD_RESULT_UNSUPPORTED;
}

/* 2M FLASH のみ使用可能なコマンド **********************************/

/* ページ消去 (FLASH) */
static void CARDi_ErasePageCore(u32 dst)
{
    CARDi_WriteEnable();
    CARDi_CommandBegin(1 + cardi_common.cmd->spec.addr_width + 0);
    CARDi_SendSpiAddressingCommand(dst, COMM_PAGE_ERASE);
    CARDi_CommandEnd(cardi_common.cmd->spec.erase_page, 0);
}


#endif

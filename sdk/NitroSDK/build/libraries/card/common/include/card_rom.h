/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CARD - libraries
  File:     card_rom.h

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/

#if !defined(NITRO_LIBRARIES_CARD_ROM_H__)
#define NITRO_LIBRARIES_CARD_ROM_H__


#include <nitro.h>
#include "../include/card_common.h"

#if defined(SDK_TEG)
#define SDK_ARM7_READROM_SUPPORT
#endif

/*****************************************************************************/
/* macro */


/*****************************************************************************/
/* constant */

/* カード制御レジスタ */
#define REG_CARD_MASTER_CNT (HW_REG_BASE + 0x1A1)
#define REG_CARDCNT         (HW_REG_BASE + 0x1A4)
#define REG_CARD_CMD        (HW_REG_BASE + 0x1A8)
#define REG_CARD_DATA       (HW_REG_BASE + 0x100010)

/* REG_CARD_MASTER_CNT 設定ビット */
#define CARDMST_SEL_ROM     0x00
#define CARDMST_IF_ENABLE   0x40
#define CARDMST_ENABLE      0x80

/* カードコマンドフォーマット (1 ページ以外はハード的に非対応) */
#define CARD_DATA_READY     0x00800000
#define CARD_COMMAND_PAGE   0x01000000
#define CARD_COMMAND_ID     0x07000000
#define CARD_COMMAND_MASK   0x07000000
#define CARD_RESET_HI       0x20000000
#define CARD_RESET_LO       0x00000000
#define CARD_ACCESS_MODE    0x40000000
#define CARD_READ_MODE      0x00000000
#define CARD_WRITE_MODE     0x40000000
#define CARD_START          0x80000000
#define CARD_LATENCY1_MASK  0x00001FFF

#define MROMOP_G_READ_ID    0xB8000000
#define MROMOP_G_READ_PAGE  0xB7000000


/*****************************************************************************/
/* declaration */

/* CARD / CARTRIDGE / PXI 全共通の状態管理構造体 */
typedef struct CARDRomStat
{
    /* ROM アクセス用パラメータ */
    void    (*read_func) (struct CARDRomStat *);
    u32     ctrl;                      /* CARD 固有 : コントロールフラグ */
    u8     *cache_page;                /* 現在のキャッシュページ */

    /* PXI 受信待ち */
    u32     dummy[5];
    /* 32 BYTE alignment */

    /* 最後に転送したページのキャッシュ (PXI / CARD) */
    u8      cache_buf[CARD_ROM_PAGE_SIZE];

}
CARDRomStat;


SDK_COMPILER_ASSERT(sizeof(CARDRomStat) % 32 == 0);


/*****************************************************************************/
/* variable */

extern CARDRomStat rom_stat;
extern u32 cardi_rom_header_addr;

/*****************************************************************************/
/* function */

/*---------------------------------------------------------------------------*
  Name:         CARDi_IsInTcm

  Description:  指定メモリ領域が一部でも TCM 範囲に該当するか判定.

  Arguments:    buf        メモリアドレス先頭
                len        メモリサイズ

  Returns:      一部でも TCM 範囲に該当すれば TRUE, そうでなければ FALSE.
 *---------------------------------------------------------------------------*/
static inline BOOL CARDi_IsInTcm(u32 buf, u32 len)
{
#if defined(SDK_ARM9)
    const u32 i = OS_GetITCMAddress();
    const u32 d = OS_GetDTCMAddress();
    return
        ((i < buf + len) && (i + HW_ITCM_SIZE > buf)) ||
        ((d < buf + len) && (d + HW_DTCM_SIZE > buf));
#else
#pragma unused(buf)
#pragma unused(len)
    return FALSE;
#endif
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_GetRomFlag

  Description:  カードコマンドコントロールパラメータを取得.

  Arguments:    flag       カードデバイスへ発行するコマンドのタイプ
                           (CARD_COMMAND_PAGE か CARD_COMMAND_ID)

  Returns:      カードコマンドコントロールパラメータ.
 *---------------------------------------------------------------------------*/
static inline u32 CARDi_GetRomFlag(u32 flag)
{
    const u32 rom_ctrl = *(vu32 *)(cardi_rom_header_addr + 0x60);
    return (u32)((rom_ctrl & ~CARD_COMMAND_MASK) | flag |
                 CARD_READ_MODE | CARD_START | CARD_RESET_HI);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CheckPulledOutCore

  Description:  カード抜け検出関数のメイン処理.
                カードバスはロックされている必要がある.

  Arguments:    id            カードから読み出された ROM-ID

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_CheckPulledOutCore(u32 id);


#if	defined(SDK_TS) || defined(SDK_ARM7)

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadRomIDCore

  Description:  カード ID の読み出し.

  Arguments:    None.

  Returns:      カード ID.
 *---------------------------------------------------------------------------*/
u32     CARDi_ReadRomIDCore(void);

#endif

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadEnd

  Description:  ROM アクセス終了処理.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void CARDi_ReadEnd(void)
{
    CARDiCommon *const p = &cardi_common;
#ifdef SDK_ARM9
    /* ここでカード抜け検出を行う. (事前の排他処理は不要) */
    CARDi_CheckPulledOutCore(CARDi_ReadRomIDCore());
#endif
    p->cmd->result = CARD_RESULT_SUCCESS;
    CARDi_EndTask(p, TRUE);
}


#if	defined(SDK_ARM7)

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadRomCore

  Description:  ARM7 からのカードアクセス.

  Arguments:    src        転送元オフセット
                src        転送元メモリアドレス
                src        転送サイズ

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_ReadRomCore(const void *src, void *dst, u32 len);

#endif


/*****************************************************************************
 * 環境依存部分
 *
 *   TEG 環境における CARD アクセスは複雑な条件のもとに実装されている.
 *   まず, 実行環境自体が CARD でない場合は CARTRIDGE で代替される.
 *   CARD 対応でも TEG 固有仕様より ARM7 しかアクセスできないため
 *   ARM9 からは  PXI 経由で ARM7 へリクエストを送る.
 *
 *****************************************************************************/


/*---------------------------------------------------------------------------*
  Name:         CARDi_GetRomAccessor

  Description:  動作環境ごとに異なるカードアクセス関数へのポインタを返す.

  Arguments:    None.

  Returns:      以下のいずれか.
                CARDi_ReadCard(), CARDi_ReadPxi(), CARDi_ReadCartridge().
 *---------------------------------------------------------------------------*/
void    (*CARDi_GetRomAccessor(void)) (CARDRomStat *);

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadCard

  Description:  設定された内容に基づいてカードアクセスを直接実行.

  Arguments:    p          アクセス内容が設定された構造体

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_ReadCard(CARDRomStat * p);

/*---------------------------------------------------------------------------*
  Name:         CARDi_TryReadCardDma

  Description:  設定された内容が DMA でカードアクセス可能な条件を満たせば
                非同期 DMA を開始する.

  Arguments:    p          アクセス内容が設定された構造体

  Returns:      非同期 DMA を開始した場合は TRUE, そうでない場合は FALSE.
 *---------------------------------------------------------------------------*/
BOOL    CARDi_TryReadCardDma(CARDRomStat * p);


#if defined(SDK_TEG)

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadCartridge

  Description:  設定された内容に基づいてカートリッジアクセスを実行.

  Arguments:    p          アクセス内容が設定された構造体

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_ReadCartridge(CARDRomStat * p);

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadPxi

  Description:  設定された内容に基づいてカードアクセスを PXI 経由で実行.

  Arguments:    p          アクセス内容が設定された構造体

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_ReadPxi(CARDRomStat * p);

#endif /* defined(SDK_TEG) */


#endif /* NITRO_LIBRARIES_CARD_ROM_H__ */


/*---------------------------------------------------------------------------*
  $Log: card_rom.h,v $
  Revision 1.7  2006/08/23 00:39:45  yosizaki
  add cardi_rom_header_addr.

  Revision 1.6  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.5  2005/11/09 07:38:50  adachi_hiroaki
  ARM7側のROM読み込みを無効化

  Revision 1.4  2005/11/07 01:09:47  okubata_ryoma
  SDK_ASSERT_ON_COMPILEからSDK_COMPILER_ASSERTに変更

  Revision 1.3  2005/10/25 01:14:19  yosizaki
  small fix for ARM7.

  Revision 1.2  2005/10/24 10:49:59  yosizaki
  add pulledout-check in CARDi_ReadEnd().

  Revision 1.1  2005/05/12 11:14:59  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

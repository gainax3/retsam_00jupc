/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CARD - include
  File:     rom.h

  Copyright 2004-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: rom.h,v $
  Revision 1.17  2006/01/18 02:12:28  kitase_hirotake
  do-indent

  Revision 1.16  2005/11/29 01:21:00  yosizaki
  fix comments and indent.

  Revision 1.15  2005/03/18 06:52:57  yosizaki
  add CARDRomHeader.

  Revision 1.14  2005/03/01 01:57:00  yosizaki
  copyright の年を修正.

  Revision 1.13  2005/02/28 05:26:01  yosizaki
  do-indent.

  Revision 1.12  2004/12/08 08:53:06  yosizaki
  delete CARDi_ReadRomID()
  add comments.

  Revision 1.11  2004/11/02 10:07:30  yosizaki
  fix comment.

  Revision 1.10  2004/09/06 13:44:31  yosizaki
  move CARD_(Lock/Unlock)Backup

  Revision 1.9  2004/09/03 07:19:12  yosizaki
  fix include directives.

  Revision 1.8  2004/09/03 06:50:43  yosizaki
  add CARD_GetRomRegionOVT.

  Revision 1.7  2004/07/23 08:32:14  yosizaki
  small fix.

  Revision 1.6  2004/07/19 13:10:47  yosizaki
  change interface.

  Revision 1.5  2004/07/10 10:20:47  yosizaki
  add CARD_LockBackup()

  Revision 1.4  2004/07/09 08:40:13  yosizaki
  change implement of synchronous mode.

  Revision 1.3  2004/07/08 13:47:42  yosizaki
  change CARD_ReadRom (for fs_rom)

  Revision 1.2  2004/07/05 08:10:41  yosizaki
  add comment.
  include card/common.h

  Revision 1.1  2004/06/28 01:55:02  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


#if	!defined(NITRO_CARD_ROM_H_)
#define NITRO_CARD_ROM_H_


#ifdef __cplusplus
extern "C"
{
#endif


#include <nitro/misc.h>
#include <nitro/types.h>
#include <nitro/memorymap.h>
#include <nitro/mi/dma.h>
#include <nitro/mi/exMemory.h>
#include <nitro/os.h>

#include <nitro/card/common.h>


/*****************************************************************************/
/* declarations */

/* ROM ヘッダ 領域情報構造体 */
typedef struct
{
    u32     offset;
    u32     length;
}
CARDRomRegion;

/* ROM ヘッダ 構造体 */
typedef struct
{

    /* 0x000-0x020 [システム予約領域] */
    char    game_name[12];             /* ソフトタイトル名 */
    u32     game_code;                 /* イニシャルコード */
    u16     maker_code;                /* メーカーコード */
    u8      product_id;                /* 本体コード */
    u8      device_type;               /* デバイスタイプ */
    u8      device_size;               /* デバイス容量 */
    u8      reserved_A[9];             /* システム予約 A */
    u8      game_version;              /* ソフトバージョン */
    u8      property;                  /* 内部使用フラグ */

    /* 0x020-0x040 [常駐モジュール用パラメータ] */
    void   *main_rom_offset;           /* ARM9 転送元 ROM オフセット */
    void   *main_entry_address;        /* ARM9 実行開始アドレス(未実装) */
    void   *main_ram_address;          /* ARM9 転送先 RAM オフセット */
    u32     main_size;                 /* ARM9 転送サイズ */
    void   *sub_rom_offset;            /* ARM7 転送元 ROM オフセット */
    void   *sub_entry_address;         /* ARM7 実行開始アドレス(未実装) */
    void   *sub_ram_address;           /* ARM7 転送先 RAM オフセット */
    u32     sub_size;                  /* ARM7 転送サイズ */

    /* 0x040-0x050 [ファイルテーブル用パラメータ] */
    CARDRomRegion fnt;                 /* ファイルネームテーブル */
    CARDRomRegion fat;                 /* ファイルアロケーションテーブル */

    /* 0x050-0x060 [オーバーレイヘッダテーブル用パラメータ] */
    CARDRomRegion main_ovt;            /* ARM9 オーバーレイヘッダテーブル */
    CARDRomRegion sub_ovt;             /* ARM7 オーバーレイヘッダテーブル */

    /* 0x060-0x070 [システム予約領域] */
    u8      rom_param_A[8];            /* マスク ROM コントロールパラメータ A */
    u32     banner_offset;             /* バナーファイル ROM オフセット */
    u16     secure_crc;                /* セキュア領域 CRC */
    u8      rom_param_B[2];            /* マスク ROM コントロールパラメータ B */

    /* 0x070-0x078 [オートロードパラメータ] */
    void   *main_autoload_done;        /* ARM9 オートロードフックアドレス */
    void   *sub_autoload_done;         /* ARM7 オートロードフックアドレス */

    /* 0x078-0x0C0 [システム予約領域] */
    u8      rom_param_C[8];            /* マスク ROM コントロールパラメータ C */
    u32     rom_size;                  /* アプリケーション最終 ROM オフセット */
    u32     header_size;               /* ROM ヘッダサイズ */
    u8      reserved_B[0x38];          /* システム予約 B */

    /* 0x0C0-0x160 [システム予約領域] */
    u8      logo_data[0x9C];           /* NINTENDO ロゴイメージデータ */
    u16     logo_crc;                  /* NINTENDO ロゴ CRC */
    u16     header_crc;                /* ROM 内登録データ CRC */

}
CARDRomHeader;


/*****************************************************************************/
/* constants */

/* ROM エリア・サイズ */
#define CARD_ROM_PAGE_SIZE	512


/*****************************************************************************/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         CARD_GetRomRegionFNT

  Description:  ROM ヘッダの FNT 領域情報を取得する.

  Arguments:    None.

  Returns:      ROM ヘッダの FNT 領域情報 へのポインタ.
 *---------------------------------------------------------------------------*/
static inline const CARDRomRegion *CARD_GetRomRegionFNT(void)
{
    return (const CARDRomRegion *)((const u8 *)HW_ROM_HEADER_BUF + 0x40);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_GetRomRegionFAT

  Description:  ROM ヘッダの FAT 領域情報を取得する.

  Arguments:    None.

  Returns:      ROM ヘッダの FAT 領域情報 へのポインタ.
 *---------------------------------------------------------------------------*/
static inline const CARDRomRegion *CARD_GetRomRegionFAT(void)
{
    return (const CARDRomRegion *)((const u8 *)HW_ROM_HEADER_BUF + 0x48);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_GetRomRegionOVT

  Description:  ROM ヘッダの OVT 領域情報を取得する.

  Arguments:    None.

  Returns:      ROM ヘッダの OVT 領域情報 へのポインタ.
 *---------------------------------------------------------------------------*/
static inline const CARDRomRegion *CARD_GetRomRegionOVT(MIProcessor target)
{
    return (target == MI_PROCESSOR_ARM9) ?
        (const CARDRomRegion *)((const u8 *)HW_ROM_HEADER_BUF + 0x50) :
        (const CARDRomRegion *)((const u8 *)HW_ROM_HEADER_BUF + 0x58);
}

/*
 *	以下の関数は CARD 上デバイスへの低水準アクセスを提供します.
 *	これらの使用に際しては必ず所定バスの設定を自己管理してください.
 *
 *	TEG かつカートリッジでの環境下においては
 *	アクセス対象を内部的にカートリッジへ変更しますので,
 *	動作特性を除いて基本的に同様な処理が可能です.
 *
 *	CARD_LockRom() / CARD_UnlockRom() /
 *	CARD_LockBackup() / CARD_UnlockBackup() は
 *	OS_LockCard() または OS_LockCartridge() のどちらを
 *	使用すべきか動的に判断してこれを呼び出します.
 *	また, ROM とバックアップの両方を同時にアクセスしないよう
 *	内部で排他制御が行われます.
 */

#if	defined(SDK_TEG)
static inline BOOL CARDi_IsTrueRom(void)
{
    return (OS_GetConsoleType() & OS_CONSOLE_DEV_CARD) != 0;
}
#endif


/*---------------------------------------------------------------------------*
  Name:         CARD_LockRom

  Description:  lock the CARD-bus before ROM operation.

  Arguments:    lock id.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARD_LockRom(u16 lock_id);

/*---------------------------------------------------------------------------*
  Name:         CARD_UnlockRom

  Description:  unlock the CARD-bus after ROM operation.

  Arguments:    lock id which is used by CARD_LockRom().

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARD_UnlockRom(u16 lock_id);

/*
 *	以下は ROM へ読み込みアクセスする同期 / 非同期関数です.
 *	関数内でのハードウェア制御は以下のとおりです.
 *
 *	・DMA を指定されていれば MI_StopDma() で明示的に停止してから使用する
 *	・DMA を範囲外に指定すると CPU 転送を使用する
 *	・前回のアクセスが完了していなければ完了までブロッキングする
 *
 *	カード転送速度そのものは最大 5.96[MB/s] ですが,
 *	実際の API ではいくらかのオーバーヘッドを含みます.
 *	CPU による同期転送の場合はほぼ 100% の速度を実現できます.
 *	DMA を使用した非同期転送の場合は 90 ～ 95%  となりますが,
 *	転送中の多くの時間で CPU が並列に実行できます.
 *	さらに, この最大値は転送条件が最良の場合に限定されます.
 */
/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadRom

  Description:  ROM リードの基本関数

  Arguments:    dma        使用する DMA チャンネル
                src        転送元オフセット
                dst        転送先メモリアドレス
                len        転送サイズ
                callback   完了コールバック (不使用なら NULL)
                arg        完了コールバックの引数 (不使用なら無視される)
                is_async   非同期モードを指定するなら TRUE

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_ReadRom(u32 dma,
                      const void *src, void *dst, u32 len,
                      MIDmaCallback callback, void *arg, BOOL is_async);

/*---------------------------------------------------------------------------*
  Name:         CARD_TryWaitRomAsync

  Description:  check if asynchronous ROM operation has been completed.

  Arguments:    None.

  Returns:      if operation has been completed, TRUE.
 *---------------------------------------------------------------------------*/
BOOL    CARD_TryWaitRomAsync(void);

/*---------------------------------------------------------------------------*
  Name:         CARD_WaitRomAsync

  Description:  wait for completion of current asynchronous ROM operation.

  Arguments:    None.

  Returns:      if the newest operation is successful, TRUE.
 *---------------------------------------------------------------------------*/
void    CARD_WaitRomAsync(void);

/*---------------------------------------------------------------------------*
  Name:         CARD_ReadRomAsync

  Description:  非同期 ROM リード

  Arguments:    dma        使用する DMA チャンネル
                src        転送元オフセット
                dst        転送先メモリアドレス
                len        転送サイズ
                callback   完了コールバック (不使用なら NULL)
                arg        完了コールバックの引数 (不使用なら無視される)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void CARD_ReadRomAsync(u32 dma,
                                     const void *src, void *dst, u32 len,
                                     MIDmaCallback callback, void *arg)
{
    CARDi_ReadRom(dma, src, dst, len, callback, arg, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_ReadRom

  Description:  同期 ROM リード

  Arguments:    dma        使用する DMA チャンネル
                src        転送元オフセット
                dst        転送先メモリアドレス
                len        転送サイズ

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void CARD_ReadRom(u32 dma, const void *src, void *dst, u32 len)
{
    CARDi_ReadRom(dma, src, dst, len, NULL, NULL, FALSE);
}


/* internal function */
u32     CARDi_ReadRomID(void);


/* I/O processror ************************************************************/

#if	defined(SDK_TEG) && defined(SDK_ARM7)

void    CARDi_CreatePxiRecvThread(u32 priority);

#endif /* defined(SDK_TEG) && defined(SDK_ARM7) */


#ifdef __cplusplus
} /* extern "C" */
#endif


/* NITRO_CARD_ROM_H_ */
#endif

/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CTRDG - include
  File:     ctrdg_common.h

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: ctrdg_common.h,v $
  Revision 1.20  2007/05/09 05:22:22  okubata_ryoma
  CTRDG_GetPhiClock 関数の追加

  Revision 1.19  2007/02/20 00:28:11  kitase_hirotake
  indent source

  Revision 1.18  2006/04/28 11:02:17  yada
  add define of PXI command for PHI setting.
  MIPhiClock enum -> CTRDGPhiClock enum

  Revision 1.17  2006/02/22 04:06:35  kitase_hirotake
  無制限にロック待ちしないように修正

  Revision 1.16  2006/02/07 08:12:18  kitase_hirotake
  CTRDG_CheckPulledOut 追加

  Revision 1.15  2006/01/18 02:11:20  kitase_hirotake
  do-indent

  Revision 1.14  2005/11/30 12:22:58  yosizaki
  add CTRDG_IsEnabled(), CTRDG_Enable(), CTRDG_CheckEnabled().

  Revision 1.13  2005/11/01 01:16:12  okubata_ryoma
  CTRDG_Exの追加による変更

  Revision 1.12  2005/04/12 06:23:03  yosizaki
  add pulled-out checkers.

  Revision 1.11  2005/02/28 05:26:03  yosizaki
  do-indent.

  Revision 1.10  2005/02/21 02:47:07  seiki_masashi
  Copyright 表記の修正

  Revision 1.9  2005/02/18 13:17:26  seiki_masashi
  warning 対策

  Revision 1.8  2004/09/17 12:25:18  yada
  add data forwarding functions

  Revision 1.7  2004/09/17 06:44:09  yada
  add line in CTRDG_CpuCopy8()

  Revision 1.6  2004/09/17 05:12:17  yada
  add CTRDG_IsOptionCartridge()

  Revision 1.5  2004/09/14 13:01:50  yada
  CallbackForPulledOut->PulledOutCallback

  Revision 1.4  2004/09/14 10:41:04  yada
  fix comment

  Revision 1.3  2004/09/14 07:23:08  yada
  fix a little

  Revision 1.2  2004/09/14 06:09:44  yada
  check cartridge pulled out in ARM7

  Revision 1.1  2004/09/13 10:49:35  yada
  initial release

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_CTRDG_COMMON_H_
#define NITRO_CTRDG_COMMON_H_


#include <nitro/types.h>
#include <nitro/memorymap.h>
#include <nitro/mi.h>
#include <nitro/os.h>
#include <nitro/pxi.h>


#ifdef __cplusplus
extern "C" {
#endif


//----------------------------------------------------------------------------

// PXIでの通信プロトコル関連定義
#define CTRDG_PXI_COMMAND_MASK              0x0000003f  // 開始ワードのコマンド部
#define CTRDG_PXI_COMMAND_SHIFT             0
#define CTRDG_PXI_COMMAND_PARAM_MASK        0x01ffffc0  // 開始ワードのパラメータ部
#define CTRDG_PXI_COMMAND_PARAM_SHIFT       6
#define CTRDG_PXI_FIXLEN_DATA_MASK          0x03ffffff  // 固定長連続パケット時のデータ部
#define CTRDG_PXI_FIXLEN_DATA_SHIFT         0
#define CTRDG_PXI_FLEXLEN_DATA_MASK         0x01ffffff  // 可変長連続パケット時のデータ部
#define CTRDG_PXI_FLEXLEN_DATA_SHIFT        0
#define CTRDG_PXI_FLEXLEN_CONTINUOUS_BIT    0x02000000  // 可変長連続パケット転送中
#define CTRDG_PXI_PACKET_MAX                4   // パケットの最大連続回数

// PXI経由でARM9よりARM7に発行される命令
#define CTRDG_PXI_COMMAND_INIT_MODULE_INFO  0x0001
#define CTRDG_PXI_COMMAND_TERMINATE         0x0002
#define CTRDG_PXI_COMMAND_SET_PHI           0x0003

#define CTRDG_PXI_COMMAND_IMI_PACKET_SIZE   2

// PXI経由でARM7よりARM9に発行される命令
#define CTRDG_PXI_COMMAND_PULLED_OUT        0x0011
#define CTRDG_PXI_COMMAND_SET_PHI_RESULT    0x0012


#ifdef SDK_ARM9
#define CTRDG_LOCKED_BY_MYPROC_FLAG     OS_MAINP_LOCKED_FLAG
#else
#define CTRDG_LOCKED_BY_MYPROC_FLAG     OS_SUBP_LOCKED_FLAG
#endif

#define CTRDG_SYSROM9_NINLOGO_ADR       0xffff0020

#define CTRDG_AGB_ROMHEADER_SIZE        0xc0
#define CTRDG_AGB_NINLOGO_SIZE          0x9c

#define CTRDG_IS_ROM_CODE               0x96


//---- PHI output control
typedef enum
{
    CTRDG_PHI_CLOCK_LOW = MIi_PHI_CLOCK_LOW,    // Low level
    CTRDG_PHI_CLOCK_4MHZ = MIi_PHI_CLOCK_4MHZ,  //  4.19 MHz
    CTRDG_PHI_CLOCK_8MHZ = MIi_PHI_CLOCK_8MHZ,  //  8.38 MHz
    CTRDG_PHI_CLOCK_16MHZ = MIi_PHI_CLOCK_16MHZ // 16.76 MHz
}
CTRDGPhiClock;

//----------------------------------------------------------------------------

// カートリッジヘッダ

typedef struct
{
    u32     startAddress;              // Start address (for AGB)
    u8      nintendoLogo[0x9c];        // NINTENDO logo data (for AGB)

    char    titleName[12];             // Soft title name (for AGB)
    u32     gameCode;                  // Game code (for AGB)
    u16     makerCode;                 // Maker code (for AGB)

    u8      isRomCode;                 // Is ROM code

    u8      machineCode;               // Machine code (for AGB)
    u8      deviceType;                // Device type (for AGB)

    u8      exLsiID[3];                // Extended LSI-ID

    u8      reserved_A[4];             // Reserved A (Set 0)
    u8      softVersion;               // Soft version (for AGB)
    u8      complement;                // Complement (for AGB)

    u16     moduleID;                  // Module ID
}
CTRDGHeader;

// 周辺機器ID

typedef struct
{
    union
    {
        struct
        {
            u8      bitID;             // Bit ID
            u8      numberID:5;        // Number ID
            u8:     2;
            u8      disableExLsiID:1;  // 拡張LSI-ID領域無効
        };
        u16     raw;
    };
}
CTRDGModuleID;

// 周辺機器情報

typedef struct
{
    CTRDGModuleID moduleID;            // Module ID
    u8      exLsiID[3];                // Extended LSI-ID
    u8      isAgbCartridge:1;          // Is AgbCartridge
    u8      detectPullOut:1;           // Detect pull out cartridge
    u8:     0;
    u16     makerCode;                 // Maker code (for AGB)
    u32     gameCode;                  // Game code (for AGB)
}
CTRDGModuleInfo;


//---- callback type for pulled out cartridge
#ifdef SDK_ARM9
typedef BOOL (*CTRDGPulledOutCallback) (void);
#endif
/*---------------------------------------------------------------------------*

  Description:  CTRDGライブラリについて

                ARM7にて定期的にカートリッジの抜け検出を行います。
                ただし、ARM9がカートリッジバスのアクセス権を解放しない場合には
                この処理が行えなくなりますので、10フレーム以上ロックし続けない
                ようにして下さい。

                内部でカートリッジのロックを試行するため、
                他方のプロセッサがロックしていた場合は
                解除するまで戻ってきません。

                ただし、自プロセッサにて既にロックしている場合には
                解除せずに呼び出して構いません。

 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         CTRDG_Init

  Description:  カートリッジ関数の初期化

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void    CTRDG_Init(void);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_IsPulledOut

  Description:  カートリッジ抜けを検出したか

                初めからカートリッジが無い場合は FALSE になります。

                IS-NITRO-DEBUGGER では抵抗アダプタカートリッジが
                装着されていなければ、初めからカートリッジが無くても
                カートリッジ抜けを検出する場合がありますのでご注意下さい。

  Arguments:    None

  Returns:      TRUE if detect pull out
 *---------------------------------------------------------------------------*/
BOOL    CTRDG_IsPulledOut(void);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_IsExisting

  Description:  カートリッジが存在しているか

  Arguments:    None

  Returns:      TRUE if existed
 *---------------------------------------------------------------------------*/
BOOL    CTRDG_IsExisting(void);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_IsBitID

  Description:  カートリッジへビットID周辺機器が存在しているか


  Arguments:    bitID  bit ID

  Returns:      TRUE if existed
 *---------------------------------------------------------------------------*/
BOOL    CTRDG_IsBitID(u8 bitID);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_IsNumberID

  Description:  カートリッジへナンバーID周辺機器が存在しているか

  Arguments:    numberID  number ID

  Returns:      TRUE if existed
 *---------------------------------------------------------------------------*/
BOOL    CTRDG_IsNumberID(u8 numberID);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_IsAgbCartridge

  Description:  AGBカートリッジが存在しているか

  Arguments:    None

  Returns:      TRUE if existed
 *---------------------------------------------------------------------------*/
BOOL    CTRDG_IsAgbCartridge(void);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_IsOptionCartridge

  Description:  オプションカートリッジが存在しているか

  Arguments:    None

  Returns:      TRUE if existed
 *---------------------------------------------------------------------------*/
BOOL    CTRDG_IsOptionCartridge(void);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_GetAgbGameCode

  Description:  AGBカートリッジのゲームコードを取得

  Arguments:    None

  Returns:      Game code if exist, FALSE if not exist
 *---------------------------------------------------------------------------*/
u32     CTRDG_GetAgbGameCode(void);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_GetAgbMakerCode

  Description:  AGBカートリッジのメーカーコードを取得

  Arguments:    None

  Returns:      Maker code if exist, FALSE if not exist
 *---------------------------------------------------------------------------*/
u16     CTRDG_GetAgbMakerCode(void);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_IsAgbCartridgePulledOut

  Description:  get whether system has detected pulled out AGB cartridge

  Arguments:    None

  Returns:      TRUE if detect pull out
 *---------------------------------------------------------------------------*/
BOOL    CTRDG_IsAgbCartridgePulledOut(void);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_IsOptionCartridgePulledOut

  Description:  get whether system has detected pulled out option cartridge

  Arguments:    None

  Returns:      TRUE if detect pull out
 *---------------------------------------------------------------------------*/
BOOL    CTRDG_IsOptionCartridgePulledOut(void);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_SetPulledOutCallback

  Description:  set user callback for being pulled out cartridge

  Arguments:    callback : callback

  Returns:      None
 *---------------------------------------------------------------------------*/
#ifdef SDK_ARM9
void    CTRDG_SetPulledOutCallback(CTRDGPulledOutCallback callback);
#endif

/*---------------------------------------------------------------------------*
  Name:         CTRDG_TerminateForPulledOut

  Description:  terminate for pulling out cartridge

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
#ifdef SDK_ARM9
void    CTRDG_TerminateForPulledOut(void);
#endif

/*---------------------------------------------------------------------------*
  Name:         CTRDG_SendToARM7

  Description:  ARM7にデータを送信

  Arguments:    arg : data to send

  Returns:      None
 *---------------------------------------------------------------------------*/
void    CTRDG_SendToARM7(void *arg);

//================================================================================
//       ACCESS DATA IN CARTRIDGE
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         CTRDG_DmaCopy16 / 32

  Description:  read cartridge data via DMA

  Arguments:    dmaNo : DMA No.
                src   : source address (in cartridge)
                dest  : destination address (in memory)
                size  : forwarding size

  Returns:      TRUE if success.
                FALSE if fail. ( no cartridge )
 *---------------------------------------------------------------------------*/
BOOL    CTRDG_DmaCopy16(u32 dmaNo, const void *src, void *dest, u32 size);
BOOL    CTRDG_DmaCopy32(u32 dmaNo, const void *src, void *dest, u32 size);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_CpuCopy8 / 16 / 32

  Description:  read cartridge data by CPU access

  Arguments:    src   : source address (in cartridge)
                dest  : destination address (in memory)
                size  : forwarding size

  Returns:      TRUE if success.
                FALSE if fail. ( no cartridge )
 *---------------------------------------------------------------------------*/
BOOL    CTRDG_CpuCopy8(const void *src, void *dest, u32 size);
BOOL    CTRDG_CpuCopy16(const void *src, void *dest, u32 size);
BOOL    CTRDG_CpuCopy32(const void *src, void *dest, u32 size);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_Read8 / 16 / 32

  Description:  read cartridge data by CPU access

  Arguments:    address  : source address (in cartridge)
                rdata    : address to store read data

  Returns:      TRUE if success.
                FALSE if fail. ( no cartridge )
 *---------------------------------------------------------------------------*/
BOOL    CTRDG_Read8(const u8 *address, u8 *rdata);
BOOL    CTRDG_Read16(const u16 *address, u16 *rdata);
BOOL    CTRDG_Read32(const u32 *address, u32 *rdata);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_Write8 / 16 / 32

  Description:  write data to cartridge

  Arguments:    address  : destination address (in cartridge)
                data     : data to write

  Returns:      TRUE if success.
                FALSE if fail. ( no cartridge )
 *---------------------------------------------------------------------------*/
BOOL    CTRDG_Write8(u8 *address, u8 data);
BOOL    CTRDG_Write16(u16 *address, u16 data);
BOOL    CTRDG_Write32(u32 *address, u32 data);


/*---------------------------------------------------------------------------*
  Name:         CTRDG_IsEnabled

  Description:  check if CTRDG is accessable

  Arguments:    None.

  Returns:      Return cartridge access permission.
 *---------------------------------------------------------------------------*/
BOOL    CTRDG_IsEnabled(void);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_Enable

  Description:  Set cartridge access permission mode.

  Arguments:    enable       permission mode to be set.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CTRDG_Enable(BOOL enable);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_CheckEnabled

  Description:  Terminate program if CTRDG is not accessable

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CTRDG_CheckEnabled(void);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_CheckPulledOut

  Description:  cartridge is pulled out

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void    CTRDG_CheckPulledOut(void);


/*---------------------------------------------------------------------------*
  Name:         CTRDG_SetPhiClock

  Description:  set ARM7 and ARM9 PHI output clock

  Arguments:    clock :    Phi clock to set

  Returns:      None.
 *---------------------------------------------------------------------------*/
#ifdef SDK_ARM9
void    CTRDG_SetPhiClock(CTRDGPhiClock clock);
#endif


/*---------------------------------------------------------------------------*
  Name:         CTRDG_GetPhiClock

  Description:  get ARM7 and ARM9 PHI output clock setting

  Arguments:    None.

  Returns:      Phi clock setting.
 *---------------------------------------------------------------------------*/
#ifdef SDK_ARM9
static inline CTRDGPhiClock CTRDG_GetPhiClock(void)
{
      return (CTRDGPhiClock)MIi_GetPhiClock();
}
#endif

//================================================================================
//       PRIVATE FUNCTION
//================================================================================
//---- private function ( don't use these CTRDGi_* function )

// ROMアクセスサイクル構造体
typedef struct CTRDGRomCycle
{
    MICartridgeRomCycle1st c1;
    MICartridgeRomCycle2nd c2;

}
CTRDGRomCycle;

// プロセッサ単位ロック情報構造体
typedef struct CTRDGLockByProc
{
    BOOL    locked;
    OSIntrMode irq;

}
CTRDGLockByProc;

void    CTRDGi_InitCommon(void);
void    CTRDGi_InitModuleInfo(void);
void    CTRDGi_SendtoPxi(u32 data);
#if defined(SDK_ARM7)
BOOL    CTRDGi_LockByProcessor(u16 lockID, CTRDGLockByProc *info);
#else
void    CTRDGi_LockByProcessor(u16 lockID, CTRDGLockByProc *info);
#endif
void    CTRDGi_UnlockByProcessor(u16 lockID, CTRDGLockByProc *info);
void    CTRDGi_ChangeLatestAccessCycle(CTRDGRomCycle *r);
void    CTRDGi_RestoreAccessCycle(CTRDGRomCycle *r);

BOOL    CTRDGi_IsBitIDAtInit(u8 bitID);
BOOL    CTRDGi_IsNumberIDAtInit(u8 numberID);
BOOL    CTRDGi_IsAgbCartridgeAtInit(void);
u32     CTRDGi_GetAgbGameCodeAtInit(void);
u16     CTRDGi_GetAgbMakerCodeAtInit(void);

// カートリッジヘッダアドレス獲得
#define CTRDGi_GetHeaderAddr()          ((CTRDGHeader *)HW_CTRDG_ROM)

// カートリッジ周辺機器IDのROMイメージアドレス獲得
#define CTRDGi_GetModuleIDImageAddr()   ((u16 *)(HW_CTRDG_ROM + 0x0001fffe))

// カートリッジ周辺機器情報のアドレス獲得
#define CTRDGi_GetModuleInfoAddr()      ((CTRDGModuleInfo *)HW_CTRDG_MODULE_INFO_BUF)




#define CTRDGi_FORWARD_TYPE_DMA   0x00000000
#define CTRDGi_FORWARD_TYPE_CPU   0x00000001
#define CTRDGi_FORWARD_TYPE_MASK  0x00000001

#define CTRDGi_FORWARD_WIDTH_8    0x00000000
#define CTRDGi_FORWARD_WIDTH_16   0x00000010
#define CTRDGi_FORWARD_WIDTH_32   0x00000020
#define CTRDGi_FORWARD_WIDTH_MASK 0x00000030

#define CTRDGi_FORWARD_DMA16   ( CTRDGi_FORWARD_TYPE_DMA | CTRDGi_FORWARD_WIDTH_16 )
#define CTRDGi_FORWARD_DMA32   ( CTRDGi_FORWARD_TYPE_DMA | CTRDGi_FORWARD_WIDTH_32 )
#define CTRDGi_FORWARD_CPU8    ( CTRDGi_FORWARD_TYPE_CPU | CTRDGi_FORWARD_WIDTH_8  )
#define CTRDGi_FORWARD_CPU16   ( CTRDGi_FORWARD_TYPE_CPU | CTRDGi_FORWARD_WIDTH_16 )
#define CTRDGi_FORWARD_CPU32   ( CTRDGi_FORWARD_TYPE_CPU | CTRDGi_FORWARD_WIDTH_32 )

#define CTRDGi_ACCESS_DIR_WRITE   0x00000000
#define CTRDGi_ACCESS_DIR_READ    0x00000001
#define CTRDGi_ACCESS_DIR_MASK    0x00000001

#define CTRDGi_ACCESS_WIDTH_8     0x00000010
#define CTRDGi_ACCESS_WIDTH_16    0x00000020
#define CTRDGi_ACCESS_WIDTH_32    0x00000040
#define CTRDGi_ACCESS_WIDTH_MASK  0x000000f0

#define CTRDGi_ACCESS_WRITE8  ( CTRDGi_ACCESS_DIR_WRITE | CTRDGi_ACCESS_WIDTH_8  )
#define CTRDGi_ACCESS_WRITE16 ( CTRDGi_ACCESS_DIR_WRITE | CTRDGi_ACCESS_WIDTH_16 )
#define CTRDGi_ACCESS_WRITE32 ( CTRDGi_ACCESS_DIR_WRITE | CTRDGi_ACCESS_WIDTH_32 )
#define CTRDGi_ACCESS_READ8   ( CTRDGi_ACCESS_DIR_READ  | CTRDGi_ACCESS_WIDTH_8  )
#define CTRDGi_ACCESS_READ16  ( CTRDGi_ACCESS_DIR_READ  | CTRDGi_ACCESS_WIDTH_16 )
#define CTRDGi_ACCESS_READ32  ( CTRDGi_ACCESS_DIR_READ  | CTRDGi_ACCESS_WIDTH_32 )

//---- internal function
BOOL    CTRDGi_CopyCommon(u32 dmaNo, const void *src, void *dest, u32 size, u32 forwardType);

//---- internal function
BOOL    CTRDGi_AccessCommon(void *address, u32 data, void *rdata, u32 accessType);

#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_CTRDG_COMMON_H_ */
#endif

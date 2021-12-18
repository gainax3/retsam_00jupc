/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS - libraries
  File:     os_vramExclusive.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_vramExclusive.c,v $
  Revision 1.4  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.3  2005/02/28 05:26:29  yosizaki
  do-indent.

  Revision 1.2  2005/02/18 06:18:49  yasu
  Signed/Unsigned 変換警告抑制

  Revision 1.1  2005/02/08 08:38:20  terui
  初版

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include        <nitro/os/ARM9/vramExclusive.h>
#include        <nitro/os/common/interrupt.h>


/*---------------------------------------------------------------------------*
    内部変数定義
 *---------------------------------------------------------------------------*/
static u32 OSi_vramExclusive;
static u16 OSi_vramLockId[OS_VRAM_BANK_KINDS];


/*===========================================================================*/

/*---------------------------------------------------------------------------*
  Name:         OsCountZeroBits

  Description:  32ビットの値の内、上位から数えて0が並んでいる数を返す。

  Arguments:    bitmap  -       調査する値。

  Returns:      u32             -       0ビットの数。
                                                        0x80000000の場合 0 、0x00000000の場合 32となる。
 *---------------------------------------------------------------------------*/
#include        <nitro/code32.h>
static asm u32
OsCountZeroBits( u32 bitmap )
{
    clz         r0,             r0
        bx              lr
        }
#include        <nitro/codereset.h>


/*---------------------------------------------------------------------------*
  Name:         OSi_InitVramExclusive

  Description:  VRAM 排他処理を初期化する。

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
OSi_InitVramExclusive( void )
{
    s32         i;

    OSi_vramExclusive = 0x0000;
    for( i = 0 ; i < OS_VRAM_BANK_KINDS ; i ++ )
    {
        OSi_vramLockId[ i ] = 0;
    }
}

/*---------------------------------------------------------------------------*
  Name:         OSi_TryLockVram

  Description:  VRAM 排他ロックを試みる。

  Arguments:    bank    -       排他ロックを試みる VRAM の ID ビットマップ。
                                lockId  -       ロックする際の鍵となる任意の ID。

  Returns:      BOOL    -       ロックに成功した場合に TRUE を返す。
 *---------------------------------------------------------------------------*/
BOOL OSi_TryLockVram(u16 bank, u16 lockId)
{
    u32     workMap;
    s32     zeroBits;
    OSIntrMode enabled = OS_DisableInterrupts();

    // 既に別 ID によって排他ロックされていないか確認
    workMap = (u32)(bank & OSi_vramExclusive);
    while (TRUE)
    {
        zeroBits = (s32)(31 - OsCountZeroBits(workMap));
        if (zeroBits < 0)
        {
            break;
        }
        workMap &= ~(0x00000001 << zeroBits);
        if (OSi_vramLockId[zeroBits] != lockId)
        {
            (void)OS_RestoreInterrupts(enabled);
            return FALSE;
        }
    }

    // 指定された ID の VRAM を全てロック
    workMap = (u32)(bank & OS_VRAM_BANK_ID_ALL);
    while (TRUE)
    {
        zeroBits = (s32)(31 - OsCountZeroBits(workMap));
        if (zeroBits < 0)
        {
            break;
        }
        workMap &= ~(0x00000001 << zeroBits);
        OSi_vramLockId[zeroBits] = lockId;
        OSi_vramExclusive |= (0x00000001 << zeroBits);
    }

    (void)OS_RestoreInterrupts(enabled);
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         OSi_UnlockVram

  Description:  VRAM 排他ロックを解除する。

  Arguments:    bank    -       排他ロックを解除する VRAM の ID ビットマップ。
                                lockId  -       ロックした際に指定した任意の ID。

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OSi_UnlockVram(u16 bank, u16 lockId)
{
    u32     workMap;
    s32     zeroBits;
    OSIntrMode enabled = OS_DisableInterrupts();

    workMap = (u32)(bank & OSi_vramExclusive & OS_VRAM_BANK_ID_ALL);
    while (TRUE)
    {
        zeroBits = (s32)(31 - OsCountZeroBits((u32)workMap));
        if (zeroBits < 0)
        {
            break;
        }
        workMap &= ~(0x00000001 << zeroBits);
        if (OSi_vramLockId[zeroBits] == lockId)
        {
            OSi_vramLockId[zeroBits] = 0;
            OSi_vramExclusive &= ~(0x00000001 << zeroBits);
        }
    }

    (void)OS_RestoreInterrupts(enabled);
}

/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS - include
  File:     vramExclusive.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: vramExclusive.h,v $
  Revision 1.3  2006/01/18 02:12:28  kitase_hirotake
  do-indent

  Revision 1.2  2005/02/28 05:26:02  yosizaki
  do-indent.

  Revision 1.1  2005/02/08 08:39:49  terui
  初版

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NITRO_OS_ARM9_VRAMEXCLUSIVE_H_
#define NITRO_OS_ARM9_VRAMEXCLUSIVE_H_

#ifdef  __cplusplus
extern "C" {
#endif

/*===========================================================================*/

#include	<nitro/types.h>


/*---------------------------------------------------------------------------*
    定数定義
 *---------------------------------------------------------------------------*/
#define		OS_VRAM_BANK_ID_A		0x0001
#define		OS_VRAM_BANK_ID_B		0x0002
#define		OS_VRAM_BANK_ID_C		0x0004
#define		OS_VRAM_BANK_ID_D		0x0008
#define		OS_VRAM_BANK_ID_E		0x0010
#define		OS_VRAM_BANK_ID_F		0x0020
#define		OS_VRAM_BANK_ID_G		0x0040
#define		OS_VRAM_BANK_ID_H		0x0080
#define		OS_VRAM_BANK_ID_I		0x0100
#define		OS_VRAM_BANK_ID_ALL		0x01ff

#define		OS_VRAM_BANK_KINDS		9


/*---------------------------------------------------------------------------*
  Name:         OSi_InitVramExclusive

  Description:  VRAM 排他処理を初期化する。

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    OSi_InitVramExclusive(void);

/*---------------------------------------------------------------------------*
  Name:         OSi_TryLockVram

  Description:  VRAM 排他ロックを試みる。

  Arguments:    bank	-	排他ロックを試みる VRAM の ID ビットマップ。
				lockId	-	ロックする際の鍵となる任意の ID。

  Returns:      BOOL	-	ロックに成功した場合に TRUE を返す。
 *---------------------------------------------------------------------------*/
BOOL    OSi_TryLockVram(u16 bank, u16 lockId);

/*---------------------------------------------------------------------------*
  Name:         OSi_InitVramExclusive

  Description:  VRAM 排他ロックを解除する。

  Arguments:    bank	-	排他ロックを解除する VRAM の ID ビットマップ。
				lockId	-	ロックした際に指定した任意の ID。

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    OSi_UnlockVram(u16 bank, u16 lockId);


/*===========================================================================*/

#ifdef  __cplusplus
}       /* extern "C" */
#endif

#endif /* NITRO_OS_ARM9_VRAMEXCLUSIVE_H_ */

/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/

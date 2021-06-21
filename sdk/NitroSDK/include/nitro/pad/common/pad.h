/*---------------------------------------------------------------------------*
  Project:  NitroSDK - PAD - include
  File:     pad.h

  Copyright 2003-2005,2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: pad.h,v $
  Revision 1.11  2007/08/24 04:34:49  okubata_ryoma
  fix about headerfiles.

  Revision 1.10  2005/03/01 01:57:00  yosizaki
  copyright の年を修正.

  Revision 1.9  2005/02/28 05:26:29  yosizaki
  do-indent.

  Revision 1.8  2004/08/06 12:43:20  yada
  change key interrupt logic enum

  Revision 1.7  2004/08/06 05:13:20  yada
  PAD_SetIrq -> PADi_SetIrq, PAD_ClearIrq -> PADi_ClearIrq

  Revision 1.6  2004/06/11 11:21:57  terui
  開閉検知関数のビット論理を反転

  Revision 1.5  2004/06/11 11:04:59  terui
  開閉判定に関する定義及び関数を追加。

  Revision 1.4  2004/04/13 07:45:56  yada
  割込みキー設定のマスク値を修正

  Revision 1.3  2004/04/07 02:03:17  yada
  fix header comment

  Revision 1.2  2004/03/16 13:03:42  takano_makoto
  Add DEBUG button.

  Revision 1.1  2004/02/18 01:23:28  yada
  pad部分をARM7/9に分けた共通部分で移動

  Revision 1.16  2004/02/17 09:22:03  yada
  ARM7対応

  Revision 1.15  2004/02/12 10:56:04  yasu
  new location of include files ARM9/ARM7

  Revision 1.14  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.13  2004/01/17 02:28:38  nishida_kenji
  convert 'inline' to 'static inline' in header files

  Revision 1.12  2004/01/08 12:03:21  yada
  キー割込み定義の修正

  Revision 1.11  2003/12/26 05:52:02  yada
  型ルール統一による変更

  Revision 1.10  2003/12/25 10:55:08  yada
  型ルール統一による変更

  Revision 1.9  2003/12/25 00:19:29  nishida_kenji
  convert INLINE to inline

  Revision 1.8  2003/12/11 08:00:31  yada
  enum 定義を修正。enableフラグは BOOL 型とした。

  Revision 1.7  2003/12/01 10:39:04  yada
  PAD_SetInterrupt()→PAD_SetIRQ()、PAD_ClearInterrupt()→PAD_ClearIRQ() に。

  Revision 1.6  2003/11/14 04:56:01  yada
  レジスタのアクセスマクロ REG_XXX → reg_XXX とした。

  Revision 1.5  2003/11/12 12:02:09  yada
  コメントの修正・インクルードするファイルの修正

  Revision 1.4  2003/11/10 07:59:47  yada
  パッドからの入力を反転

  Revision 1.3  2003/11/05 01:03:31  yada
  キャストを厳しく。とりあえずwarning が出ないように。

  Revision 1.2  2003/10/30 11:42:57  yada
  レジスタの型にREGTypexx を使うようにした。

  Revision 1.1  2003/10/30 07:41:13  yada
  初版作成

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_PAD_PAD_H_
#define NITRO_PAD_PAD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/misc.h>
#include <nitro/types.h>
#include <nitro/hw/common/mmap_shared.h>
#ifdef SDK_ARM9
#include <nitro/hw/ARM9/ioreg_PAD.h>
#else  // SDK_ARM9
#include <nitro/hw/ARM7/ioreg_PAD.h>
#endif // SDK_ARM9

//================================================================================
//    BUTTONS

//---- masked value
#define PAD_PLUS_KEY_MASK       0x00f0 // mask : cross keys
#define PAD_BUTTON_MASK         0x2f0f // mask : buttons
#define PAD_DEBUG_BUTTON_MASK   0x2000 // mask : debug button
#define PAD_ALL_MASK            0x2fff // mask : all buttons
#define PAD_RCNTPORT_MASK       0x2c00 // mask : factors ARM7 can read from RCNT register
#define PAD_KEYPORT_MASK        0x03ff // mask : factors ARM7/9 can read from KEY register

#define PAD_DETECT_FOLD_MASK    0x8000 // mask : folding

//---- button and key
#define PAD_BUTTON_A            0x0001 // A
#define PAD_BUTTON_B            0x0002 // B
#define PAD_BUTTON_SELECT       0x0004 // SELECT
#define PAD_BUTTON_START        0x0008 // START
#define PAD_KEY_RIGHT           0x0010 // RIGHT of cross key
#define PAD_KEY_LEFT            0x0020 // LEFT  of cross key
#define PAD_KEY_UP              0x0040 // UP    of cross key
#define PAD_KEY_DOWN            0x0080 // DOWN  of cross key
#define PAD_BUTTON_R            0x0100 // R
#define PAD_BUTTON_L            0x0200 // L
#define PAD_BUTTON_X            0x0400 // X
#define PAD_BUTTON_Y            0x0800 // Y
#define PAD_BUTTON_DEBUG        0x2000 // Debug button

/*---------------------------------------------------------------------------*
  Name:         PAD_Read

  Description:  read pad data.
                Xand Y button data from ARM7 are added.

  Arguments:    None.

  Returns:      pad data.
                See above defines to know the meaning of each bit.
 *---------------------------------------------------------------------------*/
static inline u16 PAD_Read(void)
{
    return (u16)(((reg_PAD_KEYINPUT | *(vu16 *)HW_BUTTON_XY_BUF) ^
                  (PAD_PLUS_KEY_MASK | PAD_BUTTON_MASK)) & (PAD_PLUS_KEY_MASK | PAD_BUTTON_MASK));
}

/*---------------------------------------------------------------------------*
  Name:         PAD_DetectFold

  Description:  detect folding Nitro.

  Arguments:    None.

  Returns:      TRUE if fold, FALSE if not.
 *---------------------------------------------------------------------------*/
static inline BOOL PAD_DetectFold(void)
{
    return (BOOL)((*(vu16 *)HW_BUTTON_XY_BUF & PAD_DETECT_FOLD_MASK) >> 15);
}


//================================================================================
//         INTERNAL FUNCTIONS
//================================================================================
//   Don't use PADi_ internal functions as you can.
//   Using key interrupt may cause some bugs
//   because there is the problem about key chattering

//---------------- interrupt key logic
typedef enum
{
    // condition for occurring interrupt
    PAD_LOGIC_OR = (0 << REG_PAD_KEYCNT_LOGIC_SHIFT),   //   pushing ONE of keys you specified
    PAD_LOGIC_AND = (1 << REG_PAD_KEYCNT_LOGIC_SHIFT),  //   pushing ALL of keys at a time you specified

    //---- for compatibility to old description
    PAD_OR_INTR = PAD_LOGIC_OR,
    PAD_AND_INTR = PAD_LOGIC_AND
}
PADLogic;

/*---------------------------------------------------------------------------*
  Name:         PADi_SetIrq

  Description:  set key interrupt

  Arguments:    logic   : key logic
                          PAD_OR_INTR / PAD_AND_INTR
                enable  : TRUE if enable interrupt, FALSE if not
                padMask : buttons to occur interrupt

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void PADi_SetIrq(PADLogic logic, BOOL enable, u16 padMask)
{
    SDK_ASSERT(!(padMask & (~PAD_KEYPORT_MASK)));       // check X, Y, Debug button
    reg_PAD_KEYCNT = (u16)(((u16)logic
                            | (enable ? REG_PAD_KEYCNT_INTR_MASK : 0)
                            | (PAD_KEYPORT_MASK & padMask)));
}

/*---------------------------------------------------------------------------*
  Name:         PADi_ClearIrq

  Description:  unset key interrupt

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void PADi_ClearIrq(void)
{
    reg_PAD_KEYCNT = 0;
}

//================================================================================

#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_PAD_PAD_H_ */
#endif

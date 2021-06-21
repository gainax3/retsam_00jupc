/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MI
  File:     mi_byteAccess.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mi_byteAccess.c,v $
  Revision 1.6  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.5  2005/03/01 01:57:00  yosizaki
  copyright の年を修正.

  Revision 1.4  2005/02/28 05:26:25  yosizaki
  do-indent.

  Revision 1.3  2004/05/20 07:24:52  yasu
  MI_WriteByte/ReadByte are valid only on the TEG

  Revision 1.2  2004/04/06 11:01:48  yosizaki
  change arg-type of MI_ReadByte. (void* -> const void*)

  Revision 1.1  2004/03/30 10:59:55  yada
  バイトアクセス関数をARM7専用から共通に

  Revision 1.3  2004/03/23 01:17:35  yada
  余計なif～defを削除

  Revision 1.2  2004/03/23 00:53:25  yada
  byteAccess.hのインクルード位置変更

  Revision 1.1  2004/03/23 00:50:36  yada
  firstRelease

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/misc.h>
#include <nitro/mi/byteAccess.h>

/*---------------------------------------------------------------------------*
  Name:         MI_ReadByte

  Description:  read bytedata

  Arguments:    address ... address to read bytedata

  Returns:      byte data
 *---------------------------------------------------------------------------*/
#ifdef	SDK_TEG
u8 MI_ReadByte(const void *address)
{
    if ((u32)address & 1)
    {
        return (u8)(*(u16 *)((u32)address & ~1) >> 8);
    }
    else
    {
        return (u8)(*(u16 *)address & 0xff);
    }
}
#endif

/*---------------------------------------------------------------------------*
  Name:         MI_WriteByte

  Description:  write bytedata

  Arguments:    address ... address to write bytedata

  Returns:      None.
 *---------------------------------------------------------------------------*/
#ifdef	SDK_TEG
void MI_WriteByte(void *address, u8 value)
{
    u16     val = *(u16 *)((u32)address & ~1);

    if ((u32)address & 1)
    {
        *(u16 *)((u32)address & ~1) = (u16)(((value & 0xff) << 8) | (val & 0xff));
    }
    else
    {
        *(u16 *)((u32)address & ~1) = (u16)((val & 0xff00) | (value & 0xff));
    }
}
#endif

/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - 
  File:     g3.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g3.c,v $
  Revision 1.27  2006/01/18 02:11:21  kitase_hirotake
  do-indent

  Revision 1.26  2005/03/15 06:51:51  takano_makoto
  G3_EndMakeDL関数で、curr_cmdポインタがリストの最後を指すように修正

  Revision 1.25  2005/03/01 01:57:00  yosizaki
  copyright の年を修正.

  Revision 1.24  2005/02/28 05:26:13  yosizaki
  do-indent.

  Revision 1.23  2004/09/21 04:30:48  takano_makoto
  small fix G3_EndMakeDL().

  Revision 1.22  2004/09/21 03:27:23  takano_makoto
    Add code for hardware bug in packed geometry command.

  Revision 1.21  2004/08/30 11:34:12  takano_makoto
  Add code for hardware bug in packed geometry command.

  Revision 1.20  2004/03/17 07:23:01  yasu
  do sdk_indent

  Revision 1.19  2004/02/18 01:01:16  yasu
  add comment for code32.h

  Revision 1.18  2004/02/17 10:43:52  nishida_kenji
  small fix.

  Revision 1.17  2004/02/12 07:06:27  nishida_kenji
  Avoid generating STRB in THUMB mode.

  Revision 1.15  2004/02/09 00:17:03  nishida_kenji
  Bug fix(G3_EndMakeDL may not return correct size).

  Revision 1.14  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.13  2004/01/29 12:18:30  nishida_kenji
  Add comments.

  Revision 1.11  2004/01/13 07:37:55  nishida_kenji
  APIs for making display lists dynamically.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/gx/g3.h>


/*---------------------------------------------------------------------------*
  Name:         G3_BeginMakeDL

  Description:  Initialize GXDLInfo before display list generation.

  Arguments:    info         a pointer to display list info.
                ptr          a pointer to the buffer where a display list is stored
                length       the length of the buffer(in bytes)

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3_BeginMakeDL(GXDLInfo *info, void *ptr, u32 length)
{
    SDK_ALIGN4_ASSERT(ptr);
    SDK_ALIGN4_ASSERT(length);

    info->length = length;
    info->bottom = (u32 *)ptr;
    info->curr_cmd = (u8 *)ptr;
    info->curr_param = (u32 *)ptr + 1;
    info->param0_cmd_flg = FALSE;
}


/*---------------------------------------------------------------------------*
  Name:         G3_EndMakeDL

  Description:  Finish generating a display list.

  Arguments:    info         a pointer to display list info

  Returns:      none
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>              // avoid byte access problems
u32 G3_EndMakeDL(GXDLInfo *info)
{
    u32     sz;
    if ((u8 *)info->bottom == info->curr_cmd)
    {
        // no display list generated
        return 0;
    }

    switch ((u32)info->curr_cmd & 3)
    {
        // padding
    case 0:
        SDK_ASSERT((u32)info->bottom < (u32)info->curr_cmd);
        return (u32)((u32)info->curr_cmd - (u32)info->bottom);
    case 1:
        *info->curr_cmd++ = 0;         // byte access
    case 2:
        *info->curr_cmd++ = 0;
    case 3:
        *info->curr_cmd++ = 0;
    };

    // code for hardware bug in geometry fifo.
    if (info->param0_cmd_flg)
    {
        *(u32 *)(info->curr_param++) = 0;
        info->param0_cmd_flg = FALSE;
    }

    info->curr_cmd = (u8 *)info->curr_param;

    SDK_ASSERT((u32)info->bottom < (u32)info->curr_cmd);
    SDK_ASSERTMSG(((u32)info->curr_cmd - (u32)info->bottom <= info->length),
                  "Buffer overflow ! : Current DL buffer doesn't have enough capacity for new commands\n");

    sz = (u32)((u32)info->curr_cmd - (u32)info->bottom);

    return sz;
}

#include <nitro/codereset.h>

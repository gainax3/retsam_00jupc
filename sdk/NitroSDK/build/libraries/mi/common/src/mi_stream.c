/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MI
  File:     mi_stream.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mi_stream.c,v $
  Revision 1.6  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.5  2005/03/01 01:57:00  yosizaki
  copyright ÇÃîNÇèCê≥.

  Revision 1.4  2005/02/28 05:26:25  yosizaki
  do-indent.

  Revision 1.3  2004/09/09 09:42:26  yada
  fix a little

  Revision 1.2  2004/07/20 08:00:24  yada
  fix header

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/mi/stream.h>
#include <nitro/mi/uncompress.h>

s32     MIi_InitReadStreamFromMemory(const u8 *srcp, void *destp, const void *paramp);
s32     MIi_TerminateReadStreamFromMemory(const u8 *srcp);
u8      MIi_ReadByteStreamFromMemory(const u8 *srcp);
u16     MIi_ReadShortStreamFromMemory(const u8 *srcp);
u32     MIi_ReadWordStreamFromMemory(const u8 *srcp);


static MIReadStreamCallbacks readMemoryStreamCallbacks = {
    MIi_InitReadStreamFromMemory,
    MIi_TerminateReadStreamFromMemory,
    MIi_ReadByteStreamFromMemory,
    MIi_ReadShortStreamFromMemory,
    MIi_ReadWordStreamFromMemory,
};

MIReadStreamCallbacks *MI_GetReadStreamFromMemoryCallbacks(void)
{
    return &readMemoryStreamCallbacks;
}

s32 MIi_InitReadStreamFromMemory(const u8 *srcp, void *destp, const void *paramp)
{
#pragma unused(destp)
    s32     retval = 0;

    if (paramp)
    {
        retval = (s32)MIi_ReadWordStreamFromMemory(srcp);

        if (((retval & MI_COMPRESSION_TYPE_EX_MASK) != MI_COMPRESSION_LZ)
            && ((retval & MI_COMPRESSION_TYPE_EX_MASK) != MI_COMPRESSION_RL)
            && ((retval & MI_COMPRESSION_TYPE_MASK) != MI_COMPRESSION_HUFFMAN))
            return -1;
    }

    return retval;
}

s32 MIi_TerminateReadStreamFromMemory(const u8 *srcp)
{
#pragma unused(srcp)
    return 0;
}

u8 MIi_ReadByteStreamFromMemory(const u8 *srcp)
{
    return *srcp;
}

u16 MIi_ReadShortStreamFromMemory(const u8 *srcp)
{
    return *(u16 *)srcp;
}

u32 MIi_ReadWordStreamFromMemory(const u8 *srcp)
{
    return *(u32 *)srcp;
}

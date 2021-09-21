/*---------------------------------------------------------------------------*
  Project:  NitroSDK - screenshot test - Ext
  File:     ext_keycontrol.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: ext_keycontrol.c,v $
  Revision 1.3  2006/01/18 02:11:20  kitase_hirotake
  do-indent

  Revision 1.2  2005/02/28 05:26:07  yosizaki
  do-indent.

  Revision 1.1  2004/05/27 09:14:39  takano_makoto
  Initial Upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/types.h>
#include <nitro/ext.h>


/*---------------------------------------------------------------------------*
  Name:         EXT_AutoKeys

  Description:  キーの入力.

  Arguments:    mode        current display mode
  
  Returns:      type of current capture mode
                ( see GXCaptureMode for more detail )
 *---------------------------------------------------------------------------*/
void EXT_AutoKeys(const EXTKeys *sequence, u16 *cont, u16 *trig)
{
    // キーシーケンスカウンタ
    static u16 absolute_cnt = 0;
    static u16 last_key = 0;
    u16     cnt;

    cnt = absolute_cnt;
    while (cnt >= sequence->count)
    {
        // count値が0のシーケンスがあれば終端とみなす
        if (sequence->count == 0)
        {
            return;
        }
        cnt -= sequence->count;
        sequence++;
    }

    *cont |= sequence->key;
    *trig = (u16)(~last_key & *cont);
    last_key = *cont;
    absolute_cnt++;
}

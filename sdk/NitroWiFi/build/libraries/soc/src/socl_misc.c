/*---------------------------------------------------------------------------*
  Project:  NitroWiFi - SOC - libraries
  File:     socl_misc.c

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: socl_misc.c,v $
  Revision 1.3  2006/03/10 09:22:19  kitase_hirotake
  INDENT SOURCE

  Revision 1.2  2005/08/18 13:19:05  yasu
  Roundup4 関数の移動

  Revision 1.1  2005/08/07 13:07:59  yasu
  エラーコードの名称の表示

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitroWiFi/soc.h>

struct errortable
{
    int code;
    const char*     name;
};

static struct errortable    errtable[] =
{
#include "errtable.h"
};

/*---------------------------------------------------------------------------*
  Name:         SOCL_GetErrName

  Description:  SOCL_EXXXX のエラーコード名を取得する

  Arguments:    エラーコード
  
  Returns:      エラーコードの Enum 名
 *---------------------------------------------------------------------------*/
const char* SOCL_GetErrName(int errcode)
{
    int i;

    for (i = 0; i < sizeof(errtable) / sizeof(errtable[0]); i++)
    {
        if (errtable[i].code == errcode)
        {
            return errtable[i].name;
        }
    }

    return "";
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_ROUNDUP4/SOCLi_RoundUp4
  
  Description:  4 の倍数に切り上げる
                コードサイズを減らすためにマクロと関数の2つを用意している
                size が定数の場合はマクロの方を使いコンパイラの最適化を期
                待する方がよい
  
  Arguments:    size     切り上げたい値
  
  Returns:      size を 4 に切り上げた数
 *---------------------------------------------------------------------------*/
u32 SOCLi_RoundUp4(u32 size)
{
    return SOCLi_ROUNDUP4(size);
}

/*---------------------------------------------------------------------------*
  Project:  NitroSDK - Tools - nitro-gcc
  File:     stdafx.h

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: stdafx.h,v $
  Revision 1.2  2006/01/18 02:12:29  kitase_hirotake
  do-indent

  Revision 1.1  2005/06/21 06:22:33  yasu
  初版作成

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include "stdafx_orig.h"
#include "nitrodll.h"

//
// コンソールメッセージ表示の抑制
//
#define  printf(...)		((void)0)

/*---------------------------------------------------------------------------*
  Description:  dll.GetDebugPrint のハック
                Makefile 内で dll.GetDebugPrint を dll_GetDebugPrint に置換し
                処理をこちらへ分岐させる．
                もしストリームに -1 が流れてきたらそこでプログラムを終了する
                -1 の次の文字がリターンバリューとなる
 *---------------------------------------------------------------------------*/
BOOL    dll_GetDebugPrint(CNITRODLL * dll, NITROArch arch, char *string, INT * string_lenp,
                          INT string_max);
BOOL dll_GetDebugPrint(CNITRODLL * dll, NITROArch arch, char *string, INT * string_lenp,
                       INT string_max)
{
    BOOL    result = dll->GetDebugPrint(arch, string, string_lenp, string_max);
    int     retval;
    int     i;
    static BOOL isexit = FALSE;

    if (result && *string_lenp)
    {
        for (i = 0; i < *string_lenp; i++)
        {
            //
            // 以前に終了トークンを受け取っているかどうか判定し、
            // 受け取っているなら終了処理を行なう．
            //
            if (isexit)
            {
                if (i > 0)             // コンソールバッファのフラッシュ
                {
                    fputs(string, stdout);
                }
                retval = string[i] - '0';
                dll->Close();
                exit(retval);
            }

            //
            // 終了トークンかどうかの判定
            //    終了なら次の値がアプリケーションの返値となるので
            //    フラグを立てる．
            //    わざわざフラグを立てるのは終了トークンと返値が 2 つの
            //    バッファにまたがって送られてくる場合の対処．
            //
            if (string[i] == (char)-1)
            {
                string[i] = '\0';
                isexit = TRUE;
            }
        }
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - libraries
  File:     wm_ks.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wm_ks.c,v $
  Revision 1.7  2006/01/18 02:12:39  kitase_hirotake
  do-indent

  Revision 1.6  2005/06/07 05:45:26  seiki_masashi
  Key Sharing に関する特別処理を低減するための変更

  Revision 1.5  2005/03/03 14:46:19  seiki_masashi
  WM_DISABLE_KEYSHARING スイッチの追加
  WM_EndKeySharing 関数が常にエラー値を返していたのを修正

  Revision 1.4  2005/02/28 05:26:35  yosizaki
  do-indent.

  Revision 1.3  2004/11/02 07:26:51  terui
  コメント修正。

  Revision 1.2  2004/10/22 04:37:28  terui
  WMErrCodeとしてint型を返していた関数について、WMErrCode列挙型を返すように変更。

  Revision 1.1  2004/09/10 11:03:30  terui
  wm.cの分割に伴い、新規upload。

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include    <nitro/wm.h>
#include    "wm_arm9_private.h"


/*---------------------------------------------------------------------------*
  Name:         WM_StartKeySharing

  Description:  キーシェアリング機能を有効にする。
                機能を有効にした後MP通信を行うことで、MP通信に付随して
                キーシェアリング通信が行われる。

  Arguments:    buf         -   キー情報を格納するバッファへのポインタ。
                                実体は WMDataSharingInfo 構造体へのポインタ。
                port        -   使用する port 番号

  Returns:      WMErrCode   -   処理結果を返す。
 *---------------------------------------------------------------------------*/
WMErrCode WM_StartKeySharing(WMKeySetBuf *buf, u16 port)
{
    return WM_StartDataSharing(buf, port, 0xffff, WM_KEYDATA_SIZE, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         WM_EndKeySharing

  Description:  キーシェアリング機能を無効にする。

  Arguments:    buf         -   キー情報を格納するバッファへのポインタ。
                                実体は WMDataSharingInfo 構造体へのポインタ。

  Returns:      WMErrCode   -   処理結果を返す。
 *---------------------------------------------------------------------------*/
WMErrCode WM_EndKeySharing(WMKeySetBuf *buf)
{
    return WM_EndDataSharing(buf);
}

/*---------------------------------------------------------------------------*
  Name:         WM_GetKeySet

  Description:  キーシェアリングされたキーセットデータをひとつ読み出す。

  Arguments:    buf         -   キー情報を格納するバッファへのポインタ。
                                実体は WMDataSharingInfo 構造体へのポインタ。
                keySet      -   キーセットを読み出すバッファへのポインタ。
                                WM_StartKeySharingにて与えたバッファとは
                                別のバッファを指定する。

  Returns:      MWErrCode   -   処理結果を返す。
 *---------------------------------------------------------------------------*/
WMErrCode WM_GetKeySet(WMKeySetBuf *buf, WMKeySet *keySet)
{
    WMErrCode result;
    u16     sendData[WM_KEYDATA_SIZE / sizeof(u16)];
    WMDataSet ds;
    WMArm9Buf *p = WMi_GetSystemWork();

    {
        sendData[0] = (u16)PAD_Read();
        result = WM_StepDataSharing(buf, sendData, &ds);
        if (result == WM_ERRCODE_SUCCESS)
        {
            keySet->seqNum = buf->currentSeqNum;

            {
                u16     iAid;
                for (iAid = 0; iAid < 16; iAid++)
                {
                    u16    *keyData;
                    keyData = WM_GetSharedDataAddress(buf, &ds, iAid);
                    if (keyData != NULL)
                    {
                        keySet->key[iAid] = keyData[0];
                    }
                    else
                    {
                        // 受信に失敗した相手は 0
                        keySet->key[iAid] = 0;
                    }
                }
            }
            return WM_ERRCODE_SUCCESS; // 成功終了
        }
        else
        {
            return result;
        }
    }
}

/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/

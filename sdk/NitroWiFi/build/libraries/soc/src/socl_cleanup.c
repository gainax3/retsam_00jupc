/*---------------------------------------------------------------------------*
  Project:  NitroWiFi - SOC - libraries
  File:     soc_cleanup.c

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: socl_cleanup.c,v $
  Revision 1.6  2006/03/10 09:22:19  kitase_hirotake
  INDENT SOURCE

  Revision 1.5  2005/10/12 01:45:29  yasu
  DCF コールバックのキャンセル

  Revision 1.4  2005/09/28 11:44:37  yasu
  コードを整理
  SOCL_CalmDown() を追加
  CPS ライブラリの正式対応がまだなので SOC 側で処理をカバーしている

  Revision 1.3  2005/09/27 14:18:09  yasu
  SOC_Close の非同期動作サポート

  Revision 1.2  2005/09/15 12:51:01  yasu
  DHCP Requested IP サポート

  Revision 1.1  2005/08/18 13:17:59  yasu
  cleanup API を別ファイルへ移動

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitroWiFi/socl.h>
#include "../wcm/include/wcm_cpsif.h"

/*---------------------------------------------------------------------------*
  Name:         SOCL_Clearup

  Description:  SOC/CPS ソケットライブラリをシャットダウンします。

  Arguments:    なし

  Returns:       0 正常終了
                -1 全てのソケットサービスが停止していません
 *---------------------------------------------------------------------------*/
int SOCL_Cleanup(void)
{
    int result;

    SDK_ASSERT(SOCLiConfigPtr);

    // IP アドレスが保存されていないなら保存しておく
    if (SOCLiRequestedIP == 0)
    {
        SOCLiRequestedIP = CPSMyIp;
    }

    // 全てのソケットをクローズしていく．
    // SOCLiUDPSendSocket もこの処理でクローズされる
    while (SOCL_EINPROGRESS == SOCL_CalmDown())
    {
        OS_Sleep(100);
    }

    result = SOCLi_CleanupCommandPacketQueue();

    if (result >= 0)
    {
#ifdef SDK_MY_DEBUG
        OS_TPrintf("CPS_Cleanup\n");
#endif

        // DHCP Release を行なうためにブロックする
        // これを避けたい場合は SOCL_CalmDown() で処理を進めておくこと
        CPS_Cleanup();

        // 非同期 Cleanup 対策
        CPS_SetScavengerCallback(NULL);

        if (!SOCLiConfigPtr->lan_buffer)    // 自力確保した領域の開放
        {
            SOCLi_Free(SOCLiCPSConfig.lan_buf);
        }

        SOCLiConfigPtr = NULL;
    }

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SOCL_CloseAll

  Description:  ユーザソケットを全て非同期でクローズする
                (特殊ソケットは残す)
  
  Arguments:    なし
  
  Returns:      SOCL_EINPROGRESS=-26 ならクローズ処理中
                SOCL_ESUCCESS   = 0  なら完了
 *---------------------------------------------------------------------------*/
int SOCL_CloseAll(void)
{
    SOCLSocket*     socket;
    OSIntrMode      enable;

    for (;;)
    {
        // クローズ呼び出し中に SocketList の状態が変化するかもしれないので
        // 毎回先頭から検索する
        enable = OS_DisableInterrupts();
        for (socket = SOCLiSocketList; socket; socket = socket->next)
        {
            // まだクローズ処理が始まっていない一般のソケットであるか？
            if ((int)socket != SOCLiUDPSendSocket && !SOCL_SocketIsWaitingClose(socket))
            {
                break;
            }
        }
        (void)OS_RestoreInterrupts(enable);

        if (!socket)
        {
            break;
        }

        (void)SOCL_Close((int)socket);
    }

    // ソケットリストが空、あるいは UDPSend ソケットのみであり廃棄リストが空なら終了
    if (SOCLiSocketList == NULL || ((int)SOCLiSocketList == SOCLiUDPSendSocket && SOCLiSocketList->next == NULL))
    {
        if (SOCLiSocketListTrash == NULL)
        {
            return SOCL_ESUCCESS;
        }
    }

    return SOCL_EINPROGRESS;
}

/*---------------------------------------------------------------------------*
  Name:         SOCL_CalmDown

  Description:  全てのソケットおよび補助ソケットを非同期でクローズし、cleanup の
                準備を行なう．CPS ライブラリの停止処理も開始する．
  
  Arguments:    なし
  
  Returns:      SOCL_EINPROGRESS=-26 ならクローズ処理中
                SOCL_ESUCCESS   = 0  なら完了
 *---------------------------------------------------------------------------*/
int SOCL_CalmDown(void)
{
    int result;

    if (SOCLiUDPSendSocket)
    {
        result = SOCL_CloseAll();

        if (result == SOCL_ESUCCESS)
        {
            (void)SOCL_Close(SOCLiUDPSendSocket);

            if (SOCL_IsClosed(SOCLiUDPSendSocket))
            {
                SOCLiUDPSendSocket = NULL;
            }

            result = SOCL_EINPROGRESS;
        }

        SOCLi_TrashSocket();
    }
    else
    {   // 非同期 Cleanup 対策
        if (CPS_CalmDown())
        {
            WCM_SetRecvDCFCallback(NULL);
            result = SOCL_ESUCCESS;
        }
        else
        {
            result = SOCL_EINPROGRESS;
        }
    }

    return result;
}

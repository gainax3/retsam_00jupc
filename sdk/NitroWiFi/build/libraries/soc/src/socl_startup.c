/*---------------------------------------------------------------------------*
  Project:  NitroWiFi - SOC - libraries
  File:     soc_startup.c

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: socl_startup.c,v $
  Revision 1.18  2006/03/10 09:22:19  kitase_hirotake
  INDENT SOURCE

  Revision 1.17  2005/10/05 11:15:45  yasu
  バージョンストリングの埋め込み

  Revision 1.16  2005/09/28 11:44:37  yasu
  コードを整理
  SOCL_CalmDown() を追加
  CPS ライブラリの正式対応がまだなので SOC 側で処理をカバーしている

  Revision 1.15  2005/09/21 11:19:40  yasu
  SOCL_SetYieldWait() の追加

  Revision 1.14  2005/09/15 13:14:46  yasu
  DHCP タイムアウトサポート

  Revision 1.13  2005/09/15 12:51:01  yasu
  DHCP Requested IP サポート

  Revision 1.12  2005/09/02 08:47:05  yasu
  SOCLConfig 内の IP アドレスの設定を変更できる API を用意

  Revision 1.11  2005/08/22 04:33:02  yasu
  UDP の受信バッファサイズを固定にした

  Revision 1.10  2005/08/19 05:05:36  yasu
  mtu/rwin 対応

  Revision 1.9  2005/08/18 13:17:59  yasu
  cleanup API を別ファイルへ移動

  Revision 1.8  2005/08/08 14:15:38  yasu
  CPS スレッドの優先度設定

  Revision 1.7  2005/07/30 22:30:14  yasu
  デモが動くように修正

  Revision 1.6  2005/07/30 15:33:45  yasu
  コマンドパイプ分離にともなう修正

  Revision 1.5  2005/07/25 14:22:24  yasu
  SOC_Startup と SOCL_Startup の連結

  Revision 1.4  2005/07/23 14:16:49  yasu
  Debug メッセージを隠匿

  Revision 1.3  2005/07/22 12:44:56  yasu
  非同期処理仮実装

  Revision 1.2  2005/07/19 14:06:33  yasu
  SOCL_Read/Write 追加

  Revision 1.1  2005/07/18 13:16:28  yasu
  送信受信スレッドを作成する

  Revision 1.1  2005/07/15 13:33:11  yasu
  soc ライブラリのソース調整

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitroWiFi/socl.h>
#include "../wcm/include/wcm_cpsif.h"

//---------------------------------------------------------------------------*
//
//  SOCL の Config 値へのポインタの保存
//  SOCL 関数の呼び出し前にここに値が設定する必要がある
//
CPSConfig       SOCLiCPSConfig;
const SOCLConfig*   SOCLiConfigPtr = NULL;
int         SOCLiUDPSendSocket = 0;
int         SOCLiDhcpState = 0;
SOCLInAddr  SOCLiRequestedIP = 0;
u32         SOCLiYieldWait = 0;

//---------------------------------------------------------------------------*
static void SOCLi_StartupCPS(void);
static int  SOCLi_StartupSOCL(void);
static void SOCLi_SetMyIP(void);
BOOL        SOCL_LinkIsOn(void);

#include <nitroWiFi/version.h>
#include <nitro/version_begin.h>
#define MODULE  "WiFi"SDK_WIFI_VERSION_STRING
static char id_string[] = SDK_MIDDLEWARE_STRING("NINTENDO", MODULE);
#include <nitro/version_end.h>

/*---------------------------------------------------------------------------*
  Name:         SOCL_Startup

  Description:  ライブラリを初期化します。
                関数を呼び出す前に SOCLConfig パラメータをライブラリ仕様に
                合わせて初期化してください.

  Arguments:    socl_config     構造体SOCLConfigへのポインタ
                socl_hostip     IP アドレスの設定値
                                NULL なら DHCP で取得する

  Returns:      0  なら正常終了
                -1 ならメモリ不足
 *---------------------------------------------------------------------------*/
int SOCL_Startup(const SOCLConfig* socl_config)
{
    SDK_USING_MIDDLEWARE(id_string);
    SDK_ASSERT(socl_config);

    if (SOCLiConfigPtr)
    {
        return 0;
    }

    SOCLiConfigPtr = socl_config;
    SOCLi_StartupCPS();                     // CPS     サービス起動
    return SOCLi_StartupSOCL();             // SOCL    サービス起動
}

static int SOCLi_StartupSOCL(void)
{
    int r;

    // Command サービス起動
    r = SOCLi_StartupCommandPacketQueue((int)SOCLiConfigPtr->cmd_packet_max);

    // UDP Send ソケットの作成
    if (r >= 0)
    {
        SOCLiUDPSendSocket = SOCL_UdpSendSocket();
    }

    return r;
}

static void SOCLi_StartupCPS(void)
{
    //
    //  CPS 関数を使用するための設定パラメータ
    //
    CPSConfig*          cps_config = &SOCLiCPSConfig;
    const SOCLConfig*   socl_config = SOCLiConfigPtr;

    SDK_ASSERT(socl_config->alloc);
    SDK_ASSERT(socl_config->free);

    MI_CpuClear8(cps_config, sizeof(CPSConfig));
    cps_config->alloc = socl_config->alloc;
    cps_config->free = socl_config->free;
    cps_config->link_is_on = SOCL_LinkIsOn; // 物理リンクの状態を返す関数
    cps_config->random_seed = 0;            // 0 なら乱数の種を OS_Tick から取得
    cps_config->yield_wait = SOCLiYieldWait;    // CPS ライブラリの yield 時の sleep 時間

    // socl_config->lan_buffer_size が 0    なら default 値を設定
    // socl_config->lan_buffer      が NULL なら alloc で自力確保する
    if (socl_config->lan_buffer_size)
    {
        cps_config->lan_buflen = socl_config->lan_buffer_size;
    }
    else
    {
        cps_config->lan_buflen = SOCL_LAN_BUFFER_SIZE_DEFAULT;
    }

    if (socl_config->lan_buffer)
    {
        cps_config->lan_buf = socl_config->lan_buffer;
    }
    else
    {
        cps_config->lan_buf = SOCLi_Alloc(cps_config->lan_buflen);
    }

    // MTU/MSS の設定
    //  socl_config->mtu が 0 なら default 値を設定
    {
        s32 mtu, mss, rwin;

        mtu = socl_config->mtu ? socl_config->mtu : SOCL_MTU_SIZE_DEFAULT;
        rwin = socl_config->rwin ? socl_config->rwin : SOCL_RWIN_SIZE_DEFAULT;
        mss = SOCLi_MTUtoMSS(mtu);

        cps_config->mymss = (u32) mss;
        SOCLSocketParamTCP.buffer.rcvbuf_size = (u16) rwin;
        SOCLSocketParamTCP.buffer.rcvbuf_consume_min = (u16) (rwin / 2);
    }

    // HostIP の設定 hostip が 0 なら DHCP を使用する
    // DHCP が引けない場合、SOCLiDhcpState で判定できる
    CPSMyIp = 0x00000000;

    if (socl_config->use_dhcp)
    {
        SOCLiDhcpState = SOCL_DHCP_REQUEST;
        cps_config->mode = 0;
        cps_config->dhcp_callback = SOCLi_DhcpTimeout;
        cps_config->requested_ip = SOCLiRequestedIP;
    }
    else
    {
        SOCLiDhcpState = 0;
        cps_config->mode = CPS_DONOTUSE_DHCP;
        cps_config->dhcp_callback = SOCLi_SetMyIP;
    }

    // CPS スレッドの優先度
    CPS_SetThreadPriority(socl_config->cps_thread_prio ? socl_config->cps_thread_prio : (u32) SOCL_CPS_SOCKET_THREAD_PRIORITY);

    WCM_SetRecvDCFCallback(CPSi_RecvCallbackFunc);

    {   // 非同期 Cleanup に対する一時的な対策
        extern void CPS_SetScavengerCallback(void (*f) (void));
        CPS_SetScavengerCallback(SOCLi_TrashSocket);
    }

#ifdef SDK_MY_DEBUG
    OS_TPrintf("CPS_Startup\n");
#endif
    CPS_Startup(cps_config);

    return;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_SetMyIPinConfig

  Description:  SOCLConfig 内に登録されている IP アドレスフィールドの書き換え

  Arguments:    なし

  Returns:      なし
 *---------------------------------------------------------------------------*/
SDK_WEAK_SYMBOL void SOCLi_SetMyIPinConfig(SOCLInAddr ip, SOCLInAddr mask, SOCLInAddr gateway)
{
    SOCLConfig*     config = (SOCLConfig*)SOCLiConfigPtr;

    if (config)
    {
        config->host_ip.my_ip = ip;
        config->host_ip.net_mask = mask;
        config->host_ip.gateway_ip = gateway;
    }

    return;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_SetMyIP

  Description:  IP アドレスの手動設定

  Arguments:    なし

  Returns:      なし
 *---------------------------------------------------------------------------*/
SDK_WEAK_SYMBOL void SOCLi_SetMyIP(void)
{
    const SOCLConfig*   config = SOCLiConfigPtr;

    SDK_ASSERT(config);
    CPSMyIp = SOCL2CPSInAddr(config->host_ip.my_ip);
    CPSNetMask = SOCL2CPSInAddr(config->host_ip.net_mask);
    CPSGatewayIp = SOCL2CPSInAddr(config->host_ip.gateway_ip);
    CPSDnsIp[0] = SOCL2CPSInAddr(config->host_ip.dns_ip[0]);
    CPSDnsIp[1] = SOCL2CPSInAddr(config->host_ip.dns_ip[1]);
    SOCLiDhcpState |= SOCL_DHCP_CALLBACK;
    return;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_DhcpTimeout

  Description:  DHCP の Timeout のコールバック

  Arguments:    なし

  Returns:      なし
 *---------------------------------------------------------------------------*/
SDK_WEAK_SYMBOL void SOCLi_DhcpTimeout(void)
{
    SOCLiDhcpState |= (SOCL_DHCP_CALLBACK | SOCL_DHCP_ERROR);
    return;
}

/*---------------------------------------------------------------------------*
  Name:         SOCL_LinkIsOn

  Description:  物理リンクの接続判定

  Arguments:    なし

  Returns:      接続しているなら TRUE
 *---------------------------------------------------------------------------*/
SDK_WEAK_SYMBOL BOOL SOCL_LinkIsOn(void)
{
    return WCM_GetApMacAddress() != NULL;
}

/*---------------------------------------------------------------------------*
  Project:  NitroWiFi - SOC - libraries
  File:     soc.c

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: soc.c,v $
  Revision 1.31  2006/07/18 02:37:09  okubata_ryoma
  NitroSDK3.2PR以前のSTD_CopyLStringと互換性を保つための変更

  Revision 1.30  2006/03/10 09:22:19  kitase_hirotake
  INDENT SOURCE

  Revision 1.29  2005/09/17 11:51:58  yasu
  ConfigError 時の判定に Link 切れを追加した

  Revision 1.28  2005/09/17 09:05:27  yasu
  リンク切れのハンドリングにともなう DHCP周りの修正

  Revision 1.27  2005/09/15 12:50:48  yasu
  Alloc/Free 周りの修正

  Revision 1.26  2005/09/08 05:35:34  yasu
  ソケットデスクリプタが有効値であるかどうかの判定を SOCL 側で行なうことにする

  Revision 1.25  2005/09/02 08:00:13  yasu
  ローカルホストの IP のアサートを削除

  Revision 1.24  2005/08/19 12:49:49  yada
  SOC_GetHostByName() and SOC_GetHostByAddr() でホスト名を返すようになった

  Revision 1.23  2005/08/19 05:05:23  yasu
  mtu/rwin 対応

  Revision 1.22  2005/08/17 11:48:13  yasu
  SOC_InetAtoN 修正

  Revision 1.21  2005/08/17 04:10:27  yasu
  SOC_Poll の機能追加

  Revision 1.20  2005/08/12 14:20:58  yasu
  SOC_InetNtoP　の返す文字列アドレスのフォーマット誤りの修正

  Revision 1.19  2005/08/12 09:53:07  yasu
  Recv/Send における引数の指定と socket 構造体内の block/noblock 設定の調和

  Revision 1.18  2005/08/09 02:01:04  yasu
  SOC_Poll のバグフィクス

  Revision 1.17  2005/08/04 04:15:37  yasu
  SOC_MSG_PEEK 仮対応

  Revision 1.16  2005/08/02 03:14:00  yasu
  Free 処理のラッパーが正常に動作していないのを修正

  Revision 1.15  2005/08/01 13:25:21  yasu
  SOC_Listen/Accept の動作をより socket らしくした

  Revision 1.14  2005/08/01 07:40:28  yasu
  コメント追加

  Revision 1.13  2005/07/30 15:34:08  yasu
  コマンドパイプ分離にともなう修正

  Revision 1.12  2005/07/28 14:14:22  seiki_masashi
  SOC_InetNtoP, SOC_InetPtoN の追加

  Revision 1.11  2005/07/28 11:26:31  yasu
  typo バグ修正

  Revision 1.10  2005/07/28 11:15:23  yasu
  SOC_Poll の実装

  Revision 1.9  2005/07/27 13:18:01  yasu
  SOC_GetHostByAddr　の追加

  Revision 1.8  2005/07/27 12:21:44  yasu
  IP_AtoN 系の命令の作業

  Revision 1.7  2005/07/27 12:01:10  yasu
  sendto の追加

  Revision 1.6  2005/07/25 14:22:24  yasu
  SOC_Startup と SOCL_Startup の連結

  Revision 1.5  2005/07/25 10:15:01  yasu
  SOC ライブラリの追加

  Revision 1.4  2005/07/22 14:11:11  yasu
  バグ修正

  Revision 1.3  2005/07/22 13:55:35  yasu
  listen accept の追加

  Revision 1.2  2005/07/22 12:44:56  yasu
  非同期処理仮実装

  Revision 1.1  2005/07/18 13:16:27  yasu
  送信受信スレッドを作成する

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitroWiFi/soc.h>
#include <nitroWiFi/socl.h>

const u8    SOCAddrAny[SOC_ALEN] = {   0,  0,  0,  0};
const u8    SOCAddrLoopback[SOC_ALEN] = { 127,  0,  0,  1};
const u8    SOCAddrLimited[SOC_ALEN] = { 255,255,255,255};

/*---------------------------------------------------------------------------*
  Name:         SOC_Socket

  Description:  ソケットを作成する
                メモリ利用パラメータはデフォルト設定を使用

  Arguments:    pf              プロトコルファミリー
                                現在は SOC_PF_INET を設定すること
                type            ソケットの種類
                                SOC_SOCK_STREAM : TCP ソケット
                                SOC_SOCK_DGRAM  : UDP ソケット
                protocol        プロトコル指定
                                現在は 0 を設定すること

  Returns:      正or 0: ソケット記述子
                  現状では NDS のアドレス空間が int 表現で正の値であることを
                  利用してポインタの値をそのまま返している．
                負    : エラー
                  現状サポートしているエラー値は以下
                    - SOC_ENOMEM:  メモリー不足
                  エラー値は追加される場合があるため、他の負の値も全て汎用
                  エラーとして扱うこと．
 *---------------------------------------------------------------------------*/
int SOC_Socket(int pf, int type, int protocol)
{
    (void)pf;   // unused
    (void)protocol;

    SDK_ASSERT(pf == SOC_PF_INET);
    SDK_ASSERT(type == SOC_SOCK_STREAM || type == SOC_SOCK_DGRAM);
    SDK_ASSERT(protocol == 0);

    return(type == SOC_SOCK_STREAM) ? SOCL_TcpSocket() : SOCL_UdpSocket();
}

/*---------------------------------------------------------------------------*
  Name:         SOC_Bind

  Description:  ソケットのローカルポート側の設定(バインド)を行なう
                CPS 関数では接続先のポートとアドレスも同時に設定して
                接続するのでここではソケット構造体内部にローカルポートの
                設定値を保存するのみ．

  Arguments:    s               ソケット記述子
                sockAddr        ローカルのポート番号・アドレス
                    SOCSockAddr 型か SOCSockAddrIn 型
                    以下は設定例

                    SOCSockAddrIn  a;
                    MI_CpuClear8(&a, sizeof(a));
                    a.len    = sizeof(SOCSockAddrIn);
                    a.family = SOC_PF_INET;
                    a.port   = SOC_HtoNs(local_port)
                    a.addr   = SOC_HtoNl(SOC_INADDR_ANY);
                    SOC_Bind(socket, &a);

  Returns:      正or 0: 成功
                負    : エラー
                  現状サポートしているエラー値は以下
                    - :
                  エラー値は追加される場合があるため、他の負の値も全て汎用
                  エラーとして扱うこと．
 *---------------------------------------------------------------------------*/
int SOC_Bind(int s, const void* sockAddr)
{
    const SOCSockAddrIn*    addr = (const SOCSockAddrIn*)sockAddr;

    SDK_ASSERT(addr != NULL);
    SDK_ASSERT(addr->len == sizeof(SOCSockAddrIn)); // not used
    SDK_ASSERT(addr->family == SOC_PF_INET);        // not used
    return SOCL_Bind(s, SOC_NtoHs(addr->port));
}

/*---------------------------------------------------------------------------*
  Name:         SOC_Connect

  Description:  ソケットのリモートポート側の設定(バインド)を行なう
                CPS 関数では接続先のポートとアドレスも同時に設定して
                接続するのでここでは SOC_Bind でソケット構造体内部に保存
                された設定値を使用する．

  Arguments:    s               ソケット記述子
                sockAddr        リモートのポート番号・アドレス
                    SOCSockAddr 型か SOCSockAddrIn 型
                    以下は設定例

                    SOCSockAddrIn  a;
                    MI_CpuClear8(&a, sizeof(a));
                    a.len    = sizeof(SOCSockAddrIn);
                    a.family = SOC_PF_INET;
                    a.port   = SOC_HtoNs(remote_port)
                    a.addr   = SOC_HtoNl(remote_IP_address);
                    SOC_Connect(socket, &a);

  Returns:      正or 0: 成功
                負    : エラー
                  現状サポートしているエラー値は以下
                    - :
                  エラー値は追加される場合があるため、他の負の値も全て汎用
                  エラーとして扱うこと．
 *---------------------------------------------------------------------------*/
int SOC_Connect(int s, const void* sockAddr)
{
    const SOCSockAddrIn*    addr = (const SOCSockAddrIn*)sockAddr;

    SDK_ASSERT(addr != NULL);
    SDK_ASSERT(addr->len == sizeof(SOCSockAddrIn)); // not used
    SDK_ASSERT(addr->family == SOC_PF_INET);        // not used
    return SOCL_Connect(s, SOC_NtoHs(addr->port), SOC_NtoHl(addr->addr.addr));
}

/*---------------------------------------------------------------------------*
  Name:         SOC_Read/SOC_Recv/SOC_RecvFrom

  Description:  ソケットからデータを読み込む．

  Arguments:    s               ソケット記述子
                buf             読み込みバッファ
                len             読み込みバッファサイズ
                flag            読み込み時の設定フラグ
                                現在は SOC_MSG_DONTWAIT のみサポート
                sockFrom        読み込みデータを送ってきたホスト情報

  Returns:      正or 0: 成功
                負    : エラー
                  現状サポートしているエラー値は以下
                    - :
                  エラー値は追加される場合があるため、他の負の値も全て汎用
                  エラーとして扱うこと．
 *---------------------------------------------------------------------------*/
int SOC_Read(int s, void* buf, int len)
{
    return SOCL_ReadFrom(s, buf, len, NULL, NULL, 0);
}

int SOC_Recv(int s, void* buf, int len, int flags)
{
    return SOCL_ReadFrom(s, buf, len, NULL, NULL, flags);
}

int SOC_RecvFrom(int s, void* buf, int len, int flags, void* sockFrom)
{
    u16         port;
    SOCLInAddr  ip;
    int         result;

    result = SOCL_ReadFrom(s, buf, len, &port, &ip, flags);

    if (result >= 0 && sockFrom)
    {
        SOCSockAddrIn*  addr = (SOCSockAddrIn*)sockFrom;
        addr->port = SOC_HtoNs(port);
        addr->addr.addr = SOC_HtoNl(ip);
    }

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SOC_Write/SOC_Send

  Description:  ソケットへデータを書き込む．

  Arguments:    s               ソケット記述子
                buf             書き込みバッファ
                len             書き込みバッファサイズ
                flag            読み込み時の設定フラグ
                                現在は SOC_MSG_DONTWAIT のみサポート

  Returns:      正or 0: 成功
                負    : エラー
                  現状サポートしているエラー値は以下
                    - :
                  エラー値は追加される場合があるため、他の負の値も全て汎用
                  エラーとして扱うこと．
 *---------------------------------------------------------------------------*/
int SOC_Write(int s, const void* buf, int len)
{
    return SOCL_WriteTo(s, buf, len, 0, (SOCLInAddr) 0, 0);
}

int SOC_Send(int s, const void* buf, int len, int flags)
{
    return SOCL_WriteTo(s, buf, len, 0, (SOCLInAddr) 0, flags);
}

int SOC_SendTo(int s, const void* buf, int len, int flags, const void* sockTo)
{
    SOCSockAddrIn*  addr;
    u16         port;
    SOCLInAddr  ip;

    if (sockTo)
    {
        addr = (SOCSockAddrIn*)sockTo;
        port = SOC_NtoHs(addr->port);
        ip = SOC_NtoHl(addr->addr.addr);
    }
    else
    {
        port = 0;
        ip = 0;
    }

    return SOCL_WriteTo(s, buf, len, port, ip, flags);
}

/*---------------------------------------------------------------------------*
  Name:         SOC_Shutdown

  Description:  ソケットを shutdown する．

  Arguments:    s               ソケット記述子
                how             シャットダウン処理の種類
                                SO_SHUT_RDWR 送受信処理

                                現在、CPS 関数の制限により
                                SOC_SHUT_RDWR のみのサポート

  Returns:      正or 0: 成功
                負    : エラー
                  現状サポートしているエラー値は以下
                    - :
                  エラー値は追加される場合があるため、他の負の値も全て汎用
                  エラーとして扱うこと．
 *---------------------------------------------------------------------------*/
int SOC_Shutdown(int s, int how)
{
    SDK_ASSERT(how == SOC_SHUT_RDWR);
    (void)how;  // unused
    return SOCL_Shutdown(s);
}

/*---------------------------------------------------------------------------*
  Name:         SOC_Close

  Description:  ソケットをクローズする．

  Arguments:    s               ソケット記述子

  Returns:      正or 0: 成功
                負    : エラー
                  現状サポートしているエラー値は以下
                    - :
                  エラー値は追加される場合があるため、他の負の値も全て汎用
                  エラーとして扱うこと．
 *---------------------------------------------------------------------------*/
int SOC_Close(int s)
{
    return SOCL_Close(s);
}

/*---------------------------------------------------------------------------*
  Name:         SOC_GetHostByName

  Description:  ホスト名からホストエントリを取得する
                CPS の制限から取得できるのは IP アドレスのみである．

  Arguments:    name      ホスト名

  Returns:      hostent   ホストのエントリ情報構造体へのポインタ
 *---------------------------------------------------------------------------*/
SOCHostEnt* SOC_GetHostByName(const char* name)
{
    static SOCHostEnt   hostent;
    static u8*          hostAddrList[2];
    static SOCLInAddr   hostip;
    static char         entName[SOC_MAXDNAME + 1];
    u32 h;

    h = (u32) SOCL_Resolve(name);
    if (h == 0)
    {
        return NULL;
    }
    
    // NitroSDK3.2PR以前のSTD_CopyLStringと互換性を保つため
    MI_CpuClear8(entName, sizeof(entName));
    //---- ホスト名コピー
    (void)STD_CopyLString(&entName[0], name, SOC_MAXDNAME + 1);

    hostent.name = &entName[0];
    hostent.aliases = NULL;
    hostent.addrType = SOC_PF_INET;
    hostent.length = sizeof(SOCLInAddr);
    hostent.addrList = hostAddrList;
    hostAddrList[0] = (u8*) &hostip;
    hostAddrList[1] = NULL;
    hostip = SOC_HtoNl(h);

    return &hostent;
}

/*---------------------------------------------------------------------------*
  Name:         SOC_GetHostByAddr

  Description:  アドレスからホストエントリを取得する
                実際はアドレスをホストエントリ構造体内部に格納するのみである．

  Arguments:    s               ソケット記述子

  Returns:      hostent   ホストのエントリ情報構造体へのポインタ
 *---------------------------------------------------------------------------*/
SOCHostEnt* SOC_GetHostByAddr(const void* addr, int len, int type)
{
    static SOCHostEnt   hostent;
    static u8*          hostAddrList[2];
    static SOCLInAddr   hostip;
    static char         entName[SOC_MAXDNAME + 1];

    SDK_ASSERT(type == SOC_PF_INET);
    SDK_ASSERT(len == sizeof(SOCLInAddr));

    //---- アドレスを文字表記に変換
    (void)SOC_InetNtoP(SOC_AF_INET, addr, &entName[0], SOC_MAXDNAME + 1);

    hostent.name = &entName[0];
    hostent.aliases = NULL;
    hostent.addrType = (s16) type;
    hostent.length = (s16) len;
    hostent.addrList = hostAddrList;
    hostAddrList[0] = (u8*) &hostip;
    hostAddrList[1] = NULL;
    hostip = *(SOCLInAddr*)addr;

    return &hostent;
}

/*---------------------------------------------------------------------------*
  Name:         SOC_GetSockName

  Description:  ソケットのバインドされているローカルアドレス/ポートを取得する

  Arguments:    s         ソケット
                sockAddr  ソケットのバインド先

                SOCSockAddrIn  a;
                a.len    = sizeof(SOCSockAddrIn);
                a.family = SOC_PF_INET;
                a.port   = SOC_HtoNs(local_port)
                a.addr   = SOC_HtoNl(local_ip);

  Returns:      成功 0
                失敗 負の値
 *---------------------------------------------------------------------------*/
int SOC_GetSockName(int s, void* sockAddr)
{
    SOCSockAddrIn*  addr = (SOCSockAddrIn*)sockAddr;
    u32 local_ip;
    u32 local_port;

    if (!s)
    {
        return SOC_ENETRESET;
    }

    local_ip = (u32) SOCL_GetHostID();
    local_port = (u32) SOCL_GetHostPort(s);

    if (local_ip == 0)
    {
        local_port = 0;
    }

    addr->len = sizeof(SOCSockAddrIn);
    addr->family = SOC_PF_INET;
    addr->port = SOC_HtoNs(local_port);
    addr->addr.addr = SOC_HtoNl(local_ip);

    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         SOC_GetHostID

  Description:  ホストの IP アドレスを取得する．

  Arguments:    なし

  Returns:      ホストの IPv4 の IP アドレス(NetworkByteOrder)
 *---------------------------------------------------------------------------*/
long SOC_GetHostID(void)
{
    u32 hostid = (u32) SOCL_GetHostID();

    return (long)SOC_HtoNl(hostid);
}

/*---------------------------------------------------------------------------*
  Name:         SOC_GetResolver/SOC_SetResolver

  Description:  現在の DNS サーバの host ip アドレスを取得したり設定したりする
                まだネットワーク接続ができていななら負の値の SOCL_ENETRESET
                を返す．
 *---------------------------------------------------------------------------*/
int SOC_SetResolver(const SOCInAddr* dns1, const SOCInAddr* dns2)
{
    return SOCL_SetResolver(SOC_NtoHl(dns1->addr), SOC_NtoHl(dns2->addr));
}

int SOC_GetResolver(SOCInAddr* dns1, SOCInAddr* dns2)
{
    u32 dns1a, dns2a;
    int result;

    result = SOCL_GetResolver(&dns1a, &dns2a);
    if (result >= 0)
    {
        dns1->addr = SOC_HtoNl(dns1a);
        dns2->addr = SOC_HtoNl(dns2a);
    }

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SOC_Listen

  Description:  ソケットからの接続待ちを行なう
                CPS の仕様より現在は何もしない

  Arguments:    s               ソケット記述子

  Returns:      0
 *---------------------------------------------------------------------------*/
int SOC_Listen(int s, int backlog)
{
    return SOCL_Listen(s, backlog);
}

/*---------------------------------------------------------------------------*
  Name:         SOC_Accept

  Description:  ソケットからの接続待ちを行なう
                現在 NOBLOCK モードでは動作しない

  Arguments:    s               ソケット記述子
                sockAddr        接続相手のアドレス

  Returns:      正or 0: 成功(Accept ソケットの記述子)
                負    : エラー
 *---------------------------------------------------------------------------*/
int SOC_Accept(int s, void* sockAddr)
{
    SOCSockAddrIn*  addr = (SOCSockAddrIn*)sockAddr;
    u16         remote_port;
    SOCLInAddr  remote_ip;
    int         result;

    result = SOCL_Accept(s, &remote_port, &remote_ip);

    if (result >= 0)
    {
        addr->port = SOC_HtoNs(remote_port);
        addr->addr.addr = SOC_HtoNl(remote_ip);
    }

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SOC_Fcntl

  Description:  ソケットの設定を変更する
                現状においては SOC_O_NONBLOCK しか設定できない

  Arguments:    s               ソケット記述子
                cmd             コマンド
                mode            設定/読取したい値

  Returns:      正or 0: 成功
                負    : エラー
 *---------------------------------------------------------------------------*/
int SOC_Fcntl(int s, int cmd, int mode)
{
    int t;

    if (!s)
    {
        return -1;
    }

    switch (cmd)
    {
    case SOC_F_GETFL:
        t = SOCL_IsBlock(s) ? 0 : SOC_O_NONBLOCK;
        return (int)t;

    case SOC_F_SETFL:
        if (mode & SOC_O_NONBLOCK)
        {
            SOCL_SetNoBlock(s);
        }
        else
        {
            SOCL_SetBlock(s);
        }
        break;

    default:
        SDK_ASSERT(0);
        break;
    }

    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         SOC_Startup

  Description:  ソケットのパラメータの設定を行なう

  Arguments:    config          設定データの構造体

  Returns:      正or 0: 成功
                負    : エラー
 *---------------------------------------------------------------------------*/
static void*  (*AllocFunc_SOC) (u32 name, s32 size);
static void* AllocFunc_SOCL(u32 size)
{
    s32     msize = (s32) (size + sizeof(s32));
    s32*    m = AllocFunc_SOC(0U, msize);

    // 呼び出しに必要なパラメータを領域の先頭に隠しておく
    if (m)
    {
        m[0] = (s32) msize;
        m++;
    }

    return (void*)m;
}

static void (*FreeFunc_SOC) (u32 name, void *ptr, s32 size);
static void FreeFunc_SOCL(void* ptr)
{
    s32*    m = (s32*)ptr;

    // 呼び出しに必要なパラメータを領域の先頭から取り出す
    if (m)
    {
        m--;
        FreeFunc_SOC(0U, (void*)m, m[0]);
    }
}

int SOC_Startup(const SOCConfig* config)
{
    static SOCLConfig   soclConfig;

    soclConfig.use_dhcp = (config->flag == SOC_FLAG_DHCP) ? TRUE : FALSE;
    soclConfig.host_ip.my_ip = SOC_NtoHl(config->addr.addr);
    soclConfig.host_ip.net_mask = SOC_NtoHl(config->netmask.addr);
    soclConfig.host_ip.gateway_ip = SOC_NtoHl(config->router.addr);
    soclConfig.host_ip.dns_ip[0] = SOC_NtoHl(config->dns1.addr);
    soclConfig.host_ip.dns_ip[1] = SOC_NtoHl(config->dns2.addr);
    soclConfig.alloc = AllocFunc_SOCL;
    soclConfig.free = FreeFunc_SOCL;
    AllocFunc_SOC = config->alloc;
    FreeFunc_SOC = config->free;
    soclConfig.cmd_packet_max = SOCL_CMDPACKET_MAX;
    soclConfig.mtu = config->mtu;
    soclConfig.rwin = config->rwin;

    return SOCL_Startup(&soclConfig);
}

/*---------------------------------------------------------------------------*
  Name:         SOC_Cleanup

  Description:  ソケットのパラメータの開放を行なう

  Arguments:

  Returns:      正or 0: 成功
                負    : エラー
 *---------------------------------------------------------------------------*/
int SOC_Cleanup(void)
{
    return SOCL_Cleanup();
}

/*---------------------------------------------------------------------------*
  Name:         SOC_InetNtoA

  Description:  IP アドレスを文字表記へ変換する

  Arguments:    in  アドレス

  Returns:      文字表記
 *---------------------------------------------------------------------------*/
char* SOC_InetNtoA(SOCInAddr in)
{
    static char buffer[SOC_INET_ADDRSTRLEN];

    (void)SOC_InetNtoP(SOC_AF_INET, &in.addr, buffer, sizeof(buffer));

    return buffer;
}

/*---------------------------------------------------------------------------*
  Name:         SOC_InetAtoN

  Description:  文字表記から IP アドレスにへ変換する

  Arguments:    cp  文字表記
                inp IP アドレス

  Returns:      TRUE 成功   FALSE 失敗
 *---------------------------------------------------------------------------*/
int SOC_InetAtoN(const char* cp, SOCInAddr* inp)
{
    u32 h = (u32) SOCL_InetAtoH(cp);

    if (h == 0)
    {
        return FALSE;
    }

    inp->addr = SOC_HtoNl(h);
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SOC_InetNtoP

  Description:  IP アドレスを文字表記へ変換する

  Arguments:    in  アドレス

  Returns:      文字表記
 *---------------------------------------------------------------------------*/
const char* SOC_InetNtoP(int af, const void* src, char* dst, unsigned len)
{
    u32 addr32;
    u8  addr[4];

    if (af != SOC_AF_INET)
    {
        return NULL;
    }

    if (len < SOC_INET_ADDRSTRLEN)
    {
        return NULL;
    }

    MI_CpuCopy8(src, &addr32, sizeof(addr32));

    SOC_U32to4U8(addr32, addr);
    (void)OS_SNPrintf(dst, SOC_INET_ADDRSTRLEN, "%d.%d.%d.%d", addr[3], addr[2], addr[1], addr[0]);

    return (const char*)dst;
}

/*---------------------------------------------------------------------------*
  Name:         SOC_InetPtoN

  Description:  文字表記から IP アドレスにへ変換する

  Arguments:    af  アドレスファミリー
                src 渡された文字列へのポインタ
                dst 数値形式のアドレスを返すバッファへのポインタ

  Returns:      0:成功 1:失敗 SOC_EAFNOSUPPORT:afがSOC_AF_INETでない
 *---------------------------------------------------------------------------*/
int SOC_InetPtoN(int af, const char* src, void* dst)
{
    u32 h;
    u32 addr;

    if (af != SOC_AF_INET)
    {
        return SOC_EAFNOSUPPORT;
    }

    h = (u32) SOCL_InetAtoH(src);

    if (h == 0)
    {
        return 1;
    }

    addr = SOC_HtoNl(h);
    MI_CpuCopy8(&addr, dst, sizeof(addr));

    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         SOC_U32to4U8

  Description:  U32 の値を u8 x 4 の配列に代入する

  Arguments:    adr_u32         u32 の値
                adr_4u8         出力先

  Returns:      なし
 *---------------------------------------------------------------------------*/
void SOC_U32to4U8(u32 adr_u32, u8* adr_4u8)
{
    adr_4u8[0] = (u8) ((adr_u32 >> 24) & 0xff);
    adr_4u8[1] = (u8) ((adr_u32 >> 16) & 0xff);
    adr_4u8[2] = (u8) ((adr_u32 >> 8) & 0xff);
    adr_4u8[3] = (u8) ((adr_u32 >> 0) & 0xff);
}

/*---------------------------------------------------------------------------*
  Name:         SOC_Poll

  Description:  指定したソケットのなかで読み込みや書き込み処理が可能になって
                いる記述子がないかどうか調べます．

  Arguments:    fds             SOCPollFD の配列
                nfds            SOCPollFD の数
                timeout         タイムアウト
                                (0 ならだめなら即時終了/
                                 負の値ならタイムアウトなし)

  Returns:      正の値: 条件に一致したソケットの数
                     0: タイムアウト
                負の値: エラー
 *---------------------------------------------------------------------------*/
#define SOC_POLLING_INTERVAL    1   // 単位 ms
int SOC_Poll(SOCPollFD* fds, unsigned int nfds, OSTick timeout)
{
    SOCPollFD*  fp;
    BOOL        is_enable_timeout;
    s64         time;
    int         i, num_good;
    int         status, status_mask;

    is_enable_timeout = (timeout != (OSTick) SOC_INFTIM);
    time = (s64) timeout;

    for (;;)
    {
        fp = fds;
        num_good = 0;

        for (i = 0; i < nfds; i++)
        {
            status_mask = fp->events | SOC_POLLERR | SOC_POLLHUP | SOC_POLLNVAL;
            status = SOCL_GetStatus(fp->fd) & status_mask;
            if (status)
            {
                num_good++;
            }

            fp->revents = (s16) status;
            fp++;
        }

        if (num_good > 0 || (is_enable_timeout && time <= 0))
        {
            break;
        }

        OS_Sleep(SOC_POLLING_INTERVAL);
        time -= (s64) OS_MilliSecondsToTicks(SOC_POLLING_INTERVAL);
    }

    return num_good;
}

/*---------------------------------------------------------------------------*
  Name:         SOC_GetLinkState

  Description:  リンクが確立していたら TRUE をかえす

  Arguments:    interface   使用しません
 *status     リンク状態
  Returns:      なし
 *---------------------------------------------------------------------------*/
void SOC_GetLinkState(SOCInterface* interface, BOOL* status)
{
    if (WCM_GetPhase() == WCM_PHASE_DCF)            // リンクが確立したらＯＫ
    {
        *status = TRUE;
    }
    else
    {
        *status = FALSE;

        if (OS_GetProcMode() != OS_PROCMODE_IRQ)    // 割り込み中でないなら
        {
            OS_Sleep(1);    // 1ms 処理を明け渡す
        }
    }
    (void)interface;
}

/*---------------------------------------------------------------------------*
  Name:         SOC_GetConfigError

  Description:  Config 処理中にエラーが発生したかどうかを調査する

  Arguments:    interface       使用しません

  Returns:      エラー要因: 0 ならエラーなし
 *---------------------------------------------------------------------------*/
s32 SOC_GetConfigError(SOCInterface* interface)
{
    s32 result = 0;

    if (WCM_GetPhase() == WCM_PHASE_DCF)
    {
        if ((SOCLiDhcpState & SOCL_DHCP_ERROR) == SOCL_DHCP_ERROR)
        {
            // 現在の WCM の仕様ではリンク切れを検出できるのが
            // 実際のリンクが切れてから何秒か経ってからなので
            // DHCP 取得のタイムアウトの方が先に成立してしまう場合がある．
            // よって DHCP に失敗したときは必ずリンク切れが発生したとみなして
            // 再接続時にはリンク確立からはじめてもらう方が安全．
            result = SOC_IP_ERR_DHCP_TIMEOUT;
        }
    }
    else
    {
        result = SOC_IP_ERR_LINK_DOWN;
    }

#ifdef SDK_MY_DEBUG
    switch (CPSNoIpReason)
    {
    case CPS_NOIP_REASON_DHCPDISCOVERY:
        OS_TPrintf("CPS_NOIP_REASON_DHCPDISCOVERY\n");
        break;

    case CPS_NOIP_REASON_LINKOFF:
        OS_TPrintf("CPS_NOIP_REASON_LINKOFF\n");
        break;

    case CPS_NOIP_REASON_LEASETIMEOUT:
        OS_TPrintf("CPS_NOIP_REASON_LEASETIMEOUT\n");
        break;

    case CPS_NOIP_REASON_COLLISION:
        OS_TPrintf("CPS_NOIP_REASON_COLLISION\n");
        break;

    default:
        OS_TPrintf("DEFAULT???\n");
    }
#endif

    (void)interface;
    return result;
}

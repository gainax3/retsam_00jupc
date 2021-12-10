/*---------------------------------------------------------------------------*
  Project:  NitroWiFi - include - soc_stub.h
  File:     soc_stub.h

  Copyright 2002,2003,2005,2006 Nintendo. All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law. They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: soc_stub.h,v $
  Revision 1.2  2006/03/10 09:20:22  kitase_hirotake
  INDENT SOURCE

  Revision 1.1  2005/08/04 08:17:47  yada
  stub added

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NITROWIFI_SOC_STUB_H_
#define NITROWIFI_SOC_STUB_H_

#include <nitro.h>
#include <nitroWiFi/wcm.h>
#include <nitroWiFi/soc.h>
#include <nitroWiFi/soc_errcode.h>

#include <string.h>

#ifdef __cplusplus

extern "C" {
#endif

//xxxxxxxxxxxxxxxx ここ以下は gctest を通らせるための一時的なものです。
//
#define UDP_HLEN                8           //xxxxxxxxxxxxxxxxxx テンポラリ
#define SOC_SOL_TCP             0           //xxxxxxxxxxxxxxxxxx テンポラリ
#define SOC_TCP_MAXSEG          0           //xxxxxxxxxxxxxxxxxx テンポラリ
#define SOC_SO_TYPE             0x00001008  //xxxxxxxxxxxxxxxxxx テンポラリ
#define SOC_SOL_IP              0           //xxxxxxxxxxxxxxxxxx テンポラリ
#define SOC_IP_ADD_MEMBERSHIP   0x0000000b  //xxxxxxxxxxxxxxxxx x テンポラリ
#define SOC_IP_MULTICAST_LOOP   0x00000009  //xxxxxxxxxxxxxxxxx x テンポラリ
#define SOC_IP_DROP_MEMBERSHIP  0x0000000c  //xxxxxxxxxxxxxxxxx x テンポラリ
#define SOC_SO_SNDLOWAT         0x00001003  //xxxxxxxxxxxxxxxxx x テンポラリ
#define SOC_SO_RCVLOWAT         0x00001004  //xxxxxxxxxxxxxxxxx x テンポラリ
#define SOC_NI_MAXHOST          10          //xxxxxxxxxxxxxxxxx x テンポラリ
#define SOC_NI_MAXSERV          10          //xxxxxxxxxxxxxxxxx x テンポラリ
#define SOC_NI_NOFQDN           1

#define SOAddrInfo              SOCAddrInfo
#define SO_NI_MAXHOST           SOC_NI_MAXHOST
#define SO_NI_MAXSERV           SOC_NI_MAXSERV
#define SO_GetAddrInfo          SOC_GetAddrInfo
#define SO_FreeAddrInfo         SOC_FreeAddrInfo
#define SO_NI_NOFQDN            SOC_NI_NOFQDN

#define DNS_NAME_MAX            10
#define IPAddrLoopback          SOCAddrLoopback
#define IP_GetMtu               SOC_GetMtu
#define IP_GetConfigError       SOC_GetConfigError

extern const u8 SOCAddrLoopback[SOC_ALEN];

typedef struct
{
    int ___temporary___;
} ICMPInfo;

typedef void (*ICMPCallback) (ICMPInfo * info, s32 result);

typedef struct IPHeader
{
    u8  verlen;             // Version and header length
    u8  tos;                // Type of service
    u16 len;                // packet length
    u16 id;                 // Identification
    u16 frag;               // Fragment information
    u8  ttl;                // Time to live
    u8  proto;              // Protocol
    u16 sum;                // Header checksum
    u8  src[SOC_ALEN];      // IP source
    u8  dst[SOC_ALEN];      // IP destination
} IPHeader;

typedef struct SOCIpMreq
{
    SOCInAddr   multiaddr;  // IP address of group
    SOCInAddr   interface;  // IP address of interface
} SOCIpMreq;

//---- soc_stub に入っている関数
extern int  SOC_GetAddrInfo(const char* nodeName, const char* servName, const SOCAddrInfo* hints, SOCAddrInfo ** res);
extern void SOC_FreeAddrInfo(SOCAddrInfo* head);
extern int  SOC_GetNameInfo(const void*  sa, char*  node, unsigned nodeLen, char*  service, unsigned serviceLen,
                            int flags);
extern void SOC_GetMtu(SOCInterface* interface, s32* mtu);

extern s32  ICMP_Cancel(ICMPInfo* info, s32 err);
extern s32  ICMP_Socket(int af, ICMPInfo* info);
extern s32  ICMP_Ping(ICMPInfo*  info, const void*  data, s32 len, const void*  remote, ICMPCallback callback,
                      s32*  result);
extern s32  ICMP_Close(ICMPInfo* info);

extern u16  IPi_CheckSum(IPHeader* ip);

#ifdef __cplusplus

}
#endif

#endif // NITROWIFI_SOC_STUB_H_

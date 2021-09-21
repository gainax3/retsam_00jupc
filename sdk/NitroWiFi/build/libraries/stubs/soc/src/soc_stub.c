/*---------------------------------------------------------------------------*
  Project:  NitroWiFi - SOC - libraries - stub - soc
  File:     soc_stub.c

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: soc_stub.c,v $
  Revision 1.4  2006/03/10 09:22:19  kitase_hirotake
  INDENT SOURCE

  Revision 1.3  2005/08/11 14:08:00  yasu
  WEAK SYMBOL âª

  Revision 1.2  2005/08/10 02:15:33  yasu
  SOC_GetMtu ÇÃçÌèú

  Revision 1.1  2005/08/04 08:17:23  yada
  stub added

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>
#include <nitroWiFi.h>
#include <nitroWiFi/soc_stub.h>

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// gctest Çí Ç∑ÇΩÇﬂÇÃÇ‡ÇÃ
// é¿ëïÇ™Ç≥ÇÍÇƒÇ¢ÇØÇŒè¡Ç¶ÇƒÇ¢Ç≠ÇÕÇ∏

//----------------------------------------------------------------
SDK_WEAK_SYMBOL int SOC_GetAddrInfo(const char*  nodeName, const char*  servName, const SOCAddrInfo*  hints,
                                    SOCAddrInfo **  res)
{
#pragma unused(nodeName, servName, hints, res)
    return 0;
}

//----------------------------------------------------------------
SDK_WEAK_SYMBOL void SOC_FreeAddrInfo(SOCAddrInfo* head)
{
#pragma unused(head)
}

//----------------------------------------------------------------
SDK_WEAK_SYMBOL int SOC_GetNameInfo(const void*  sa, char*  node, unsigned nodeLen, char*  service, unsigned serviceLen,
                                    int flags)
{
#pragma unused(sa, node, nodeLen, service, serviceLen, flags)
    return 0;
}

//----------------------------------------------------------------
SDK_WEAK_SYMBOL s32 ICMP_Cancel(ICMPInfo* info, s32 err)
{
#pragma unused(info, err)
    return 0;
}

//----------------------------------------------------------------
SDK_WEAK_SYMBOL s32 ICMP_Socket(int af, ICMPInfo* info)
{
#pragma unused(af, info)
    return 0;
}

//----------------------------------------------------------------
SDK_WEAK_SYMBOL s32 ICMP_Ping(ICMPInfo*  info, const void*  data, s32 len, const void*  remote, ICMPCallback callback,
                              s32*  result)
{
#pragma unused(info, data, len, remote, callback, result)
    return 0;
}

//----------------------------------------------------------------
SDK_WEAK_SYMBOL s32 ICMP_Close(ICMPInfo* info)
{
#pragma unused(info)
    return 0;
}

//----------------------------------------------------------------
SDK_WEAK_SYMBOL u16 IPi_CheckSum(IPHeader* ip)
{
#pragma unused(ip)
    return 0;
}

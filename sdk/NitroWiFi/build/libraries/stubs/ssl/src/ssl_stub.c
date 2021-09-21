/*---------------------------------------------------------------------------*
  Project:  NitroWiFi - SOC - libraries - stub - ssl
  File:     ssl_stub.c

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: ssl_stub.c,v $
  Revision 1.3  2006/03/10 09:22:19  kitase_hirotake
  INDENT SOURCE

  Revision 1.2  2005/09/14 06:08:50  yasu
  CPSi_SslGetCount -> CPSi_SslGetLength

  Revision 1.1  2005/09/13 08:40:27  yasu
  ssl ÇÃ stub ÇçÏê¨

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitroWiFi.h>

//----------------------------------------------------------------
SDK_WEAK_SYMBOL void CPSi_SslListen(CPSSoc* soc)
{
#pragma unused(soc)
}

//----------------------------------------------------------------
SDK_WEAK_SYMBOL u32 CPSi_SslConnect(CPSSoc* soc)
{
#pragma unused(soc)
    return 0;
}

//----------------------------------------------------------------
SDK_WEAK_SYMBOL u8* CPSi_SslRead(u32* len, CPSSoc* soc)
{
#pragma unused(len, soc)
    return NULL;
}

//----------------------------------------------------------------
SDK_WEAK_SYMBOL void CPSi_SslConsume(u32 len, CPSSoc* soc)
{
#pragma unused(len, soc)
}

//----------------------------------------------------------------
SDK_WEAK_SYMBOL s32 CPSi_SslGetLength(CPSSoc* soc)
{
#pragma unused(soc)
    return 0;
}

//----------------------------------------------------------------
SDK_WEAK_SYMBOL u32 CPSi_SslWrite2(u8* buf, u32 len, u8* buf2, u32 len2, CPSSoc* soc)
{
#pragma unused(buf, len, buf2, len2, soc)
    return 0;
}

//----------------------------------------------------------------
SDK_WEAK_SYMBOL void CPSi_SslShutdown(CPSSoc* soc)
{
#pragma unused(soc)
}

//----------------------------------------------------------------
SDK_WEAK_SYMBOL void CPSi_SslPeriodical(u32 now)
{
#pragma unused(now)
}

//----------------------------------------------------------------
SDK_WEAK_SYMBOL void CPSi_SslCleanup(void)
{
}

//----------------------------------------------------------------
SDK_WEAK_SYMBOL void CPSi_SocConsumeRaw(u32 len, CPSSoc* soc)
{
#pragma unused(len, soc)
}

//----------------------------------------------------------------
SDK_WEAK_SYMBOL u32 CPSi_TcpWrite2Raw(u8* buf, u32 len, u8* buf2, u32 len2, CPSSoc* soc)
{
#pragma unused(buf, len, buf2, len2, soc)
    return 0;
}

//----------------------------------------------------------------
SDK_WEAK_SYMBOL u32 CPSi_TcpConnectRaw(CPSSoc* soc)
{
#pragma unused(soc)
    return 0;
}

//----------------------------------------------------------------
SDK_WEAK_SYMBOL void CPSi_TcpShutdownRaw(CPSSoc* soc)
{
#pragma unused(soc)
}

//----------------------------------------------------------------
SDK_WEAK_SYMBOL void CPSi_TcpListenRaw(CPSSoc* soc)
{
#pragma unused(soc)
}

//----------------------------------------------------------------
SDK_WEAK_SYMBOL u8* CPSi_TcpReadRaw(u32* len, CPSSoc* soc)
{
#pragma unused(len, soc)
    return NULL;
}

//----------------------------------------------------------------

/*---------------------------------------------------------------------------*
  Project:  NitroWiFi - add-ins - env - include
  File:     env.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: env.h,v $
  Revision 1.1  2005/08/17 06:30:58  yada
  initial release

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifdef  NITROWIFI_ENV_H_
#endif

#ifndef NITROWIFI_ENV_H_
#define NITROWIFI_ENV_H_
#ifdef __cplusplus
extern "C" {
#endif

#ifdef	SDK_ARM9
#include <nitroWiFi/env/env_system.h>
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITROWIFI_ENV_H_ */
#endif

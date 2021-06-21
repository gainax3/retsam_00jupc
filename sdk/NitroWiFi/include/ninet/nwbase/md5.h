/*---------------------------------------------------------------------------*
  Project:  NitroWiFi - include
  File:     md5.h

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: md5.h,v $
  Revision 1.2  2006/03/10 09:20:22  kitase_hirotake
  INDENT SOURCE

  Revision 1.1  2005/07/29 05:07:08  terui
  NitroInet とのインターフェース互換のため新規追加。

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NINET_NWBASE_MD5_H_
#define NINET_NWBASE_MD5_H_

#ifdef __cplusplus

extern "C" {
#endif

/*===========================================================================*/

// Necessary to adjust interface to "NitroInet".
#include <nitro/math.h>

typedef struct
{
    unsigned long   state[4];
    unsigned long   count[2];
    unsigned char   buffer[64];
} MD5_CTX;

void    MD5Init(MD5_CTX* context);
void    MD5Update(MD5_CTX* context, unsigned char* buf, unsigned int len);
void    MD5Final(unsigned char digest[16], MD5_CTX* context);

/*===========================================================================*/
#ifdef __cplusplus

}       /* extern "C" */
#endif

#endif /* NINET_NWBASE_MD5_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/

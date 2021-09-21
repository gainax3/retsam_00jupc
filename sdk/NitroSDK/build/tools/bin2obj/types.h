/*---------------------------------------------------------------------------*
  Project:  NitroSDK - bin2obj
  File:     types.h

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: types.h,v $
  Revision 1.2  2006/01/18 02:12:28  kitase_hirotake
  do-indent

  Revision 1.1  2005/06/10 00:43:01  yasu
  èâä˙î≈

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef	TYPES_H__
#define	TYPES_H__

typedef enum
{
    TRUE = 1,
    FALSE = 0
}
BOOL;

#include <stdint.h>

typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;

#endif

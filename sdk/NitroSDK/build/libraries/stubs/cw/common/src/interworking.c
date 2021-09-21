/*---------------------------------------------------------------------------*
  Project:  NitroSDK - stubs - cw
  File:     interworking.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: interworking.c,v $
  Revision 1.5  2006/01/18 02:11:21  kitase_hirotake
  do-indent

  Revision 1.4  2005/03/04 14:24:05  seiki_masashi
  nitro/types.h Ç include Ç∑ÇÈÇÊÇ§Ç…èCê≥

  Revision 1.3  2005/03/04 11:09:27  yasu
  __declspec(weak) Ç SDK_WEAK_SYMBOL Ç…íuÇ´ä∑Ç¶

  Revision 1.2  2005/02/28 05:26:14  yosizaki
  do-indent.

  Revision 1.1  2004/02/25 09:13:11  yasu
  add interwroking code for CW

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
// Workaround for Interworking with CodeWarrior 

#include <nitro/types.h>

void    __call_via_r0(void);
void    __call_via_r1(void);
void    __call_via_r2(void);
void    __call_via_r3(void);
void    __call_via_r4(void);
void    __call_via_r5(void);
void    __call_via_r6(void);
void    __call_via_r7(void);

SDK_WEAK_SYMBOL asm void __call_via_r0( void ){ bx r0 }
SDK_WEAK_SYMBOL asm void __call_via_r1( void ){ bx r1 }
SDK_WEAK_SYMBOL asm void __call_via_r2( void ){ bx r2 }
SDK_WEAK_SYMBOL asm void __call_via_r3( void ){ bx r3 }
SDK_WEAK_SYMBOL asm void __call_via_r4( void ){ bx r4 }
SDK_WEAK_SYMBOL asm void __call_via_r5( void ){ bx r5 }
SDK_WEAK_SYMBOL asm void __call_via_r6( void ){ bx r6 }
SDK_WEAK_SYMBOL asm void __call_via_r7( void ){ bx r7 }

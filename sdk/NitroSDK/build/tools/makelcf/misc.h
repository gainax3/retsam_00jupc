/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - makelcf
  File:     misc.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: misc.h,v $
  Revision 1.5  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.4  2005/08/26 11:23:11  yasu
  ITCM/DTCM ‚Ö‚Ì overlay ‚Ì‘Î‰ž

  Revision 1.3  2005/02/28 05:26:03  yosizaki
  do-indent.

  Revision 1.2  2004/06/29 04:08:33  yasu
  add VBuffer

  Revision 1.1  2004/03/26 05:07:33  yasu
  support variables like as -DNAME=VALUE

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef	MISC_H_
#define	MISC_H_

#ifndef	NITRO_TYPES_H_
typedef enum
{
    FALSE = 0,
    TRUE = 1
}
BOOL;

typedef unsigned char u8;
typedef unsigned short int u16;
typedef unsigned long int u32;
typedef signed char s8;
typedef signed short int s16;
typedef signed long int s32;
#endif

#define	error(...)	do { fprintf(stderr, "Error: ");   \
                             fprintf(stderr, __VA_ARGS__); \
                             fprintf(stderr, "\n"); } while(0)

#define	warning(...)	do { fprintf(stderr, "Warning: "); \
                             fprintf(stderr, __VA_ARGS__); \
                             fprintf(stderr, "\n"); } while(0)

void   *Alloc(size_t size);
void    Free(void *p);

typedef struct
{
    char   *buffer;
    int     size;
}
VBuffer;

#define	VBUFFER_INITIAL_SIZE	1024
void    InitVBuffer(VBuffer * vbuf);
void    FreeVBuffer(VBuffer * vbuf);
void    PutVBuffer(VBuffer * vbuf, char c);
char   *GetVBuffer(VBuffer * vbuf);

extern BOOL DebugMode;
void    debug_printf(const char *str, ...);

#endif //MISC_H_

/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - makelcf
  File:     makelcf.h

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: makelcf.h,v $
  Revision 1.28  2007/07/09 12:17:54  yasu
  TARGET_NAME のサポート

  Revision 1.27  2007/04/10 14:13:20  yasu
  複数の SEARCH_SYMBOL 対応

  Revision 1.26  2007/02/20 00:28:10  kitase_hirotake
  indent source

  Revision 1.25  2007/01/19 04:35:14  yosizaki
  copyrightを更新.

  Revision 1.24  2007/01/15 04:58:20  yosizaki
  increase DEFAULT_IRQSTACKSIZE to 0x800.

  Revision 1.23  2006/05/10 02:06:00  yasu
  CodeWarrior 2.x への対応

  Revision 1.22  2006/04/06 09:05:42  kitase_hirotake
  .itcm.bss .dtcm.bss .wram.bss に対応

  Revision 1.21  2006/03/29 13:13:22  yasu
  IF-ELSE-ENDIF のサポート

  Revision 1.20  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.19  2005/09/01 04:30:52  yasu
  Overlay Group のサポート

  Revision 1.18  2005/08/26 11:23:11  yasu
  ITCM/DTCM への overlay の対応

  Revision 1.17  2005/06/20 12:21:48  yasu
  Surffix -> Suffix 修正

  Revision 1.16  2005/02/28 05:26:03  yosizaki
  do-indent.

  Revision 1.15  2004/07/10 04:06:17  yasu
  Support command 'Library'
  Support modifier ':x' on template
  Fix up line continue '\'

  Revision 1.14  2004/07/08 02:58:53  yasu
  Support section name for multi-objects as 'Objects' parameters

  Revision 1.13  2004/07/02 07:34:53  yasu
  Support OBJECT( )

  Revision 1.12  2004/06/24 07:18:54  yasu
  Support keyword "Autoload"

  Revision 1.11  2004/06/14 11:28:45  yasu
  support section filter "FOREACH.STATIC.OBJECTS=.sectionName"

  Revision 1.10  2004/03/26 05:07:11  yasu
  support variables like as -DNAME=VALUE

  Revision 1.9  2004/02/20 03:38:03  yasu
  change default IRQ stack size 0xa0 -> 0x400

  Revision 1.8  2004/02/13 07:13:03  yasu
  support SDK_IRQ_STACKSIZE

  Revision 1.7  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.6  2004/01/15 10:49:47  yasu
  Static StackSize の実装

  Revision 1.5  2004/01/15 06:52:55  yasu
  change default suffix

  Revision 1.4  2004/01/14 12:38:08  yasu
  Change OverlayName->OverlayDefs

  Revision 1.3  2004/01/14 01:54:01  yasu
  changes default filenames of overlay table/namefile

  Revision 1.2  2004/01/07 13:10:17  yasu
  fix all warning at compile -Wall

  Revision 1.1  2004/01/05 02:32:59  yasu
  Initial version

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef	MAKELCF_H_
#define	MAKELCF_H_

#include "misc.h"

/*============================================================================
 *  CONTAINER
 */
typedef struct tAfterList
{
    struct tAfter *head;
    struct tAfter *tail;
}
tAfterList;

typedef struct tAfter
{
    struct tAfter *next;
    const char *name;
}
tAfter;

typedef struct tObjectList
{
    struct tObject *head;
    struct tObject *tail;
}
tObjectList;

typedef enum
{
    OBJTYPE_NONE,
    OBJTYPE_FILE,                      // Regular file
    OBJTYPE_STAR,                      // "*"
    OBJTYPE_GROUP,                     // GROUP(xxxx)
    OBJTYPE_OBJECT                     // OBJECT(yyy,zzz)
}
tObjectType;

#define isNeedSection(obj)      ((obj)->objectType != OBJTYPE_OBJECT)

typedef struct tObject
{
    struct tObject *next;
    const char *objectName;
    const char *sectionName;
    tObjectType objectType;
}
tObject;

typedef struct tOverlayList
{
    struct tOverlay *head;
    struct tOverlay *tail;
    u32     num;
}
tOverlayList;

typedef enum
{
    MEMTYPE_NONE = 0,
    MEMTYPE_MAIN,
    MEMTYPE_MAINEX,
    MEMTYPE_ITCM,
    MEMTYPE_DTCM,
    MEMTYPE_ITCM_BSS,
    MEMTYPE_DTCM_BSS,
    MEMTYPE_SHARED,
    MEMTYPE_WRAM,
    MEMTYPE_WRAM_BSS,
    MEMTYPE_VRAM,
    MEMTYPE_DUMMY,
}
tMemType;

typedef struct tOverlay
{
    struct tOverlay *next;
    u32     id;
    const char *name;
    const char *group;
    u32     address;
    struct tAfterList afters;
    struct tObjectList objects;
    struct tObjectList libraries;
    struct tObjectList searchSymbols;
    tMemType memtype;

}
tOverlay;

typedef struct tStatic
{
    const char *name;
    u32     address;
    struct tObjectList objects;
    struct tObjectList libraries;
    struct tObjectList searchSymbols;
    u32     stacksize;
    u32     stacksize_irq;
    tMemType memtype;
    const char *targetName;
}
tStatic;

typedef struct
{
    const char *overlaydefs;
    const char *overlaytable;
    const char *suffix;

}
tProperty;

typedef struct
{
    int count;
    BOOL isFirst;
    BOOL isLast;
}
tForeachStatus;

typedef struct
{
    tForeachStatus static_object;
    tForeachStatus static_library;
    tForeachStatus static_searchsymbol;
    tForeachStatus autoload;
    tForeachStatus autoload_object;
    tForeachStatus autoload_library;
    tForeachStatus autoload_searchsymbol;
    tForeachStatus overlay;
    tForeachStatus overlay_object;
    tForeachStatus overlay_library;
    tForeachStatus overlay_searchsymbol;
}
tForeach;

BOOL    AddOverlay(const char *overlayName);
BOOL    AddAutoload(const char *overlayName);
BOOL    OverlaySetId(u32 id);
BOOL    OverlaySetGroup(const char *overlayGroup);
BOOL    OverlaySetAddress(u32 address);
BOOL    OverlayAddAfter(const char *overlayName);
BOOL    OverlayAddObject(const char *objectName, tObjectType objectType);
BOOL    OverlaySetObjectSection(const char *sectionName);
BOOL    OverlayAddLibrary(const char *objectName, tObjectType objectType);
BOOL    OverlaySetLibrarySection(const char *sectionName);
BOOL    OverlayAddSearchSymbol(const char *searchName);

BOOL    StaticSetTargetName(const char *targetName);
BOOL    StaticSetName(const char *fileName);
BOOL    StaticSetAddress(u32 address);
BOOL    StaticAddObject(const char *objectName, tObjectType objectType);
BOOL    StaticSetObjectSection(const char *sectionName);
BOOL    StaticAddLibrary(const char *objectName, tObjectType objectType);
BOOL    StaticSetLibrarySection(const char *sectionName);
BOOL    StaticAddSearchSymbol(const char *searchName);
BOOL    StaticSetStackSize(u32 stacksize);
BOOL    StaticSetStackSizeIrq(u32 stacksize);
BOOL    PropertySetOverlayDefs(const char *filename);
BOOL    PropertySetOverlayTable(const char *filename);
BOOL    PropertySetSuffix(const char *suffix);

BOOL    CheckSpec(void);
void    DumpSpec(void);
BOOL    isSame(const char *s1, const char *s2);

int     ParseSpecFile(const char *filename);

extern tStatic Static;
extern tProperty Property;
extern tForeach Foreach;
extern tOverlayList OverlayList;
extern tOverlayList AutoloadList;

#define	DEFAULT_OVERLAYDEFS	"%_defs"
#define	DEFAULT_OVERLAYTABLE	"%_table"
#define	DEFAULT_SUFFIX		".sbin"
/* NITRO-SDK4.0RC: increased 0x400->0x800 */
#define	DEFAULT_IRQSTACKSIZE	0x800

int     spec_yyparse(void);
int     spec_yylex(void);
void    spec_yyerror(const char *str);


/*============================================================================
 *  TOKEN BUFFER
 */
typedef struct tTokenBuffer
{
    int     id;
    const char *string;
    int     loop_end;
}
tTokenBuffer;

typedef struct tLoopStack
{
    int     id;
    int     start;
}
tLoopStack;

#define	TOKEN_LEN		65536
#define	LOOP_STACK_MAX		32

int     ParseTlcfFile(const char *filename);
int     CreateLcfFile(const char *filename);

extern tTokenBuffer tokenBuffer[TOKEN_LEN];
extern int tokenBufferEnd;

int     tlcf_yyparse(void);
int     tlcf_yylex(void);
void    tlcf_yyerror(const char *str);
void    lcf_error(const char *fmt, ...);

extern BOOL DebugMode;
void    debug_printf(const char *str, ...);

typedef struct
{
    char   *string;
    tObjectType type;
}
tObjectName;

// for IF-ELSE-ENDIF
#define	COND_STACK_MAX		32

typedef enum
{
    COND_BLOCK_NOCOND,                 // non conditional block
    COND_BLOCK_IF,                     // if(){} block
    COND_BLOCK_ELSE                    // else{} block
}
tCondBlock;

typedef struct
{
    BOOL    valid;                     // condition is TRUE or FALSE
    tCondBlock block;                  // regular/if{}/else{}
}
tCondStack;

#endif //MAKELCF_H_

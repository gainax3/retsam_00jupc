/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - makelcf
  File:     createlcf.c

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: createlcf.c,v $
  Revision 1.25  2007/07/09 12:17:54  yasu
  TARGET_NAME のサポート

  Revision 1.24  2007/07/09 07:02:00  yasu
  IF.EXIST のサポート

  Revision 1.23  2007/06/21 00:19:05  yasu
  FOREACH.AUTOLOADS=XXX 表記のサポート

  Revision 1.22  2007/04/12 03:29:45  yasu
  IF FIRST/LAST を FOREACH OBJECT/LIBRARY でも対応できるようにした

  Revision 1.21  2007/04/10 14:13:20  yasu
  複数の SEARCH_SYMBOL 対応

  Revision 1.20  2006/05/10 02:06:00  yasu
  CodeWarrior 2.x への対応

  Revision 1.19  2006/04/06 09:05:56  kitase_hirotake
  .itcm.bss .dtcm.bss .wram.bss に対応

  Revision 1.18  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.17  2005/09/01 04:30:52  yasu
  Overlay Group のサポート

  Revision 1.16  2005/08/26 11:23:11  yasu
  ITCM/DTCM への overlay の対応

  Revision 1.15  2005/06/20 12:21:48  yasu
  Surffix -> Suffix 修正

  Revision 1.14  2005/02/28 05:26:03  yosizaki
  do-indent.

  Revision 1.13  2004/09/24 13:24:04  yasu
  Support NitroStaticInit

  Revision 1.12  2004/08/25 04:01:34  yasu
  Object aaa.o (*) のときの扱い変更

  Revision 1.11  2004/07/10 04:06:17  yasu
  Support command 'Library'
  Support modifier ':x' on template
  Fix up line continue '\'

  Revision 1.10  2004/07/02 07:34:53  yasu
  Support OBJECT( )

  Revision 1.9  2004/06/24 07:18:54  yasu
  Support keyword "Autoload"

  Revision 1.8  2004/06/14 11:28:45  yasu
  support section filter "FOREACH.STATIC.OBJECTS=.sectionName"

  Revision 1.7  2004/03/26 05:07:11  yasu
  support variables like as -DNAME=VALUE

  Revision 1.6  2004/02/13 07:13:03  yasu
  support SDK_IRQ_STACKSIZE

  Revision 1.5  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.4  2004/01/15 10:51:45  yasu
  Static StackSize の実装

  Revision 1.3  2004/01/14 12:38:08  yasu
  Change OverlayName->OverlayDefs

  Revision 1.2  2004/01/07 13:10:17  yasu
  fix all warning at compile -Wall

  Revision 1.1  2004/01/05 02:32:59  yasu
  Initial version

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include	<stdio.h>
#include	<strings.h>
#include	<stdarg.h>             // va_start(),va_end()
#ifndef _WIN32
#define SDK_ARM9
#include	<nitro/memorymap.h>
#else
#include	<io.h>                 // setmode()
#include	<fcntl.h>              // setmode()
#include	<nitro_win32.h>
#endif
#include	"makelcf.h"
#include	"defval.h"
#include	"tlcf.tab.h"

static void PutString(const char *str);
static void PutAddress(u32 address);
static BOOL PutObject(const tObject * object, const char *secName);
static void PutStaticObject(const char *secName);
static void PutOverlayName(void);
static void PutOverlayGroup(void);
static void PutOverlayObject(const char *secName);
static void PutOverlayAddress(void);
static BOOL isObjectOK(const tObject * obj, const char *section, char filter_type);
static BOOL isSectionWildcard(const char *section);
static BOOL isSectionRegular(const char *section);
static int ForeachObjects(tObject ** ptrObject, tObject * start, tForeachStatus * fep, int n);
static int ForeachStaticObjects(int n);
static int ForeachOverlayObjects(int n);
static int ForeachOverlays(int n);
static int PutToken(int n);
static void PutTokenBuffer(int start, int end);
static void PutTokenBufferAll(void);


FILE   *fp_out;
tObject *StaticObject = NULL;
tObject *StaticLibrary = NULL;
tObject *StaticSearchSymbol = NULL;
tOverlay *Autoload = NULL;
tObject *AutoloadObject = NULL;
tObject *AutoloadLibrary = NULL;
tObject *AutoloadSearchSymbol = NULL;
tOverlay *Overlay = NULL;
tObject *OverlayObject = NULL;
tObject *OverlayLibrary = NULL;
tObject *OverlaySearchSymbol = NULL;

void lcf_error(const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    va_end(va);
    fprintf(stderr, "\n");
}

/*============================================================================
 *  Put Tokens
 */
static void PutString(const char *str)
{
    if (str)
        fprintf(fp_out, "%s", str);
}

static void PutAddress(u32 address)
{
    fprintf(fp_out, "0x%08lx", address);
}

static void PutNumber(u32 num)
{
    fprintf(fp_out, "%ld", num);
}


static int GetCharPos(const char *str, char target)
{
    int     n = 0;

    if (str)
    {
        while ('\0' != str[n] && target != str[n])
        {
            n++;
        }
    }
    return n;
}

static char *GetObjectParam(const char *str, char start, char end)
{
    int     pos_start = GetCharPos(str, start);
    int     pos_end = GetCharPos(str, end);
    int     len = pos_end - pos_start;
    char   *buffer;

    if (len > 0)
    {
        buffer = Alloc(len);
        strncpy(buffer, str + pos_start + 1, len - 1);
        buffer[len - 1] = '\0';
        return buffer;
    }
    return NULL;
}

static char *GetObjectSection(const char *str)
{
    return GetObjectParam(str, '=', ':');       // return NULL if no section
}

static char GetObjectModifier(const char *str)
{
    char   *mod = GetObjectParam(str, ':', '\0');
    char    ret;

    ret = mod ? mod[0] : '\0';
    Free(&mod);

    return ret;

}

static tMemType GetMemTypeParam(const char *str)
{
    char   *mod = GetObjectParam(str, '=', ':');
    int     i;

    static struct
    {
        tMemType type;
        char   *name;
    }
    table[] =
    {
        {
        MEMTYPE_MAIN, "MAIN"}
        ,
        {
        MEMTYPE_MAINEX, "MAINEX"}
        ,
        {
        MEMTYPE_ITCM, "ITCM"}
        ,
        {
        MEMTYPE_DTCM, "DTCM"}
        ,
        {
        MEMTYPE_ITCM_BSS, "ITCM.BSS"}
        ,
        {
        MEMTYPE_DTCM_BSS, "DTCM.BSS"}
        ,
        {
        MEMTYPE_SHARED, "SHARED"}
        ,
        {
        MEMTYPE_WRAM, "WRAM"}
        ,
        {
        MEMTYPE_WRAM_BSS, "WRAM.BSS"}
        ,
        {
        MEMTYPE_VRAM, "VRAM"}
        ,
        {
        MEMTYPE_NONE, "*"}
    ,};

    for (i = 0; table[i].type != MEMTYPE_NONE; i++)
    {
        if (isSame(mod, table[i].name))
        {
            break;
        }
    }
    Free(&mod);

    return table[i].type;
}

//
// name, address で示されるセクションがどのメモリ領域に属するかを判定する
//
static tMemType GetSectionMemType(const char *name, u32 address)
{
    BOOL    isARM9 = (Static.address < 0x02300000);     // 事実上の判定値
    tMemType memtype = MEMTYPE_NONE;

    if (isARM9)
    {
        if (isSame(name, "ITCM"))
            memtype = MEMTYPE_ITCM;
        else if (isSame(name, "DTCM"))
            memtype = MEMTYPE_DTCM;
        else if (isSame(name, "WRAM"))
            memtype = MEMTYPE_WRAM;
        else if (isSame(name, "ITCM.BSS"))
            memtype = MEMTYPE_ITCM_BSS;
        else if (isSame(name, "DTCM.BSS"))
            memtype = MEMTYPE_DTCM_BSS;
        else if (isSame(name, "WRAM.BSS"))
            memtype = MEMTYPE_WRAM_BSS;
        else if (HW_ITCM_IMAGE <= address && address < HW_ITCM_END)
            memtype = MEMTYPE_ITCM;
        else if (HW_MAIN_MEM_SUB <= address && address < HW_MAIN_MEM_SUB_END)
            memtype = MEMTYPE_DTCM;
        else if (HW_MAIN_MEM_MAIN <= address && address < HW_MAIN_MEM_MAIN_END)
            memtype = MEMTYPE_MAIN;
        else if (HW_MAIN_MEM_MAIN_END <= address && address < HW_MAIN_MEM_DEBUGGER)
            memtype = MEMTYPE_MAINEX;
        else if (HW_MAIN_MEM_SHARED <= address && address < HW_MAIN_MEM_SHARED_END)
            memtype = MEMTYPE_SHARED;
        else if (HW_WRAM <= address && address < HW_WRAM_END)
            memtype = MEMTYPE_WRAM;
        else if (HW_BG_PLTT <= address && address < HW_CTRDG_ROM)
            memtype = MEMTYPE_VRAM;
    }
    else
    {
        if (isSame(name, "WRAM"))
            memtype = MEMTYPE_WRAM;
        else if (HW_MAIN_MEM_MAIN <= address && address < HW_MAIN_MEM_SUB_END)
            memtype = MEMTYPE_MAIN;
        else if (HW_MAIN_MEM_SHARED <= address && address < HW_MAIN_MEM_SHARED_END)
            memtype = MEMTYPE_SHARED;
        else if (HW_WRAM <= address && address < HW_WRAM_END)
            memtype = MEMTYPE_WRAM;
        else if (HW_BG_PLTT <= address && address < HW_CTRDG_ROM)
            memtype = MEMTYPE_VRAM;
    }
    return memtype;
}

//
// name で示されるセクションに対応する overlay を返す
//
static tOverlay *GetOverlayByName(const char *name)
{
    tOverlay *t;

    for (t = OverlayList.head; t; t = t->next)
    {
        if (isSame(t->name, name))
            return t;
    }
    for (t = AutoloadList.head; t; t = t->next)
    {
        if (isSame(t->name, name))
            return t;
    }
    return NULL;
}

//
// name で示されるセクションがどのメモリ領域に属しているかを判定する
//
static tMemType RecgSectionMemTypeByName(const char *name)
{
    tOverlay *overlay;
    tMemType memtype, mt;
    tAfter *after;

    if (isSame(name, Static.name))
    {
        Static.memtype = GetSectionMemType(Static.name, Static.address);
        return Static.memtype;
    }

    overlay = GetOverlayByName(name);
    if (!overlay)
    {
        lcf_error("Unknown section %s, Ignored.", name);
        return MEMTYPE_NONE;
    }

    if (overlay->memtype == MEMTYPE_NONE)
    {
        memtype = GetSectionMemType(overlay->name, overlay->address);

        if (memtype == MEMTYPE_NONE)
        {
            overlay->memtype = MEMTYPE_DUMMY;

            for (after = overlay->afters.head; after; after = after->next)
            {
                mt = RecgSectionMemTypeByName(after->name);

                if (mt == MEMTYPE_DUMMY)
                {
                    // ループしている
                    lcf_error("Recursive AFTER reference in section %s, Ignored.", name);
                    overlay->memtype = MEMTYPE_NONE;
                    return MEMTYPE_NONE;
                }
                else if (memtype == MEMTYPE_NONE)
                {
                    memtype = mt;
                }
                else if (memtype != mt)
                {
                    // 異なる memtype の section が AFTER にまとめてある
                    lcf_error("Bad address mapping in section %s, Ignored.", name);
                    overlay->memtype = MEMTYPE_NONE;
                    return MEMTYPE_NONE;
                }
            }
        }
        overlay->memtype = memtype;
    }
    return overlay->memtype;
}


//====== COMMONS ======

/*---------------------------------------------------------------------------*
  Name:         isObjectOK

  Description:  指定された object が section および filter_type で指定された
                条件と一致するか判定する

  Arguments:    obj         : object
                              (実際に判定されるのは
                              obj->sectionName, obj->objextType )
                section     : 指定セクション名
                filter_type : 修飾オプション

  Returns:      None.
 *---------------------------------------------------------------------------*/

static BOOL isObjectOK(const tObject * obj, const char *section, char filter_type)
{
    //
    //  セクションの名称該当チェック
    //          セクションのワイルドカートとして '*' が使える
    //          section が '*' のときは、全ての obj が合格となる
    //          obj->section が '*' のときは section 指定が
    //          一般セクション(.text,.data,.bss,.init など) のとき合格となる
    //
    if (isSectionWildcard(section) ||
        (isSectionWildcard(obj->sectionName) && isSectionRegular(section)) ||
        isSame(obj->sectionName, section))
    {
        switch (filter_type)
        {
        case 'f':                     // only regular file
            if (obj->objectType == OBJTYPE_FILE)
                return TRUE;
            break;

        case '\0':                    // OK if NULL
            return TRUE;

        default:
            lcf_error("Unknown type setting Ignored.");
            break;
        }
    }
    return FALSE;
}

static BOOL isSectionWildcard(const char *section)
{
    return !section || isSame(section, "*");
}

static BOOL isSectionRegular(const char *section)
{
    return (isSame(section, ".text") ||
            isSame(section, ".rodata") ||
            isSame(section, ".sdata") ||
            isSame(section, ".data") ||
            isSame(section, ".sbss") ||
            isSame(section, ".bss") ||
            isSame(section, ".init") ||
            isSame(section, ".exception") || isSame(section, ".ctor") || isSame(section, ".sinit"));
}

// Foreach Objects
static int ForeachObjects(tObject ** ptrObject, tObject * start, tForeachStatus* fep, int n)
{
    tTokenBuffer *token;
    char   *section;
    char    filter;
    int     count;
    
    token = &tokenBuffer[n];
    section = GetObjectSection(token->string);
    filter = GetObjectModifier(token->string);

    debug_printf("section =[%s]\n", section ? section : "");
    debug_printf("filter  =[%c]\n", filter ? filter : ' ');

    count = 0;	// 再帰対策のためスタックの値をカウンタとして使う。
    while (start)
    {
        if (isObjectOK(start, section, filter))
        {
            *ptrObject = start;
            fep->isFirst = (count ? FALSE : TRUE);
            fep->isLast  = (start->next ? FALSE : TRUE);
            fep->count   = count;
            PutTokenBuffer(n + 1, token->loop_end);
            count ++;
        }
        start = start->next;
    }

    Free(&section);

    *ptrObject = NULL;
    return token->loop_end;
}

static void PutIfObject(BOOL flag, const char *str)
{
    char *p;
    
    debug_printf("str =[%s]\n", str ? str : "");
    
    if (flag)	// TRUE
    {
        p = GetObjectParam(str, '=', ':');
        
    }
    else	// FALSE
    {
        p = GetObjectParam(str, ':', '\0');
    }
    
    if (p)	// 取得できたら表示
    {
        PutString(p);
        Free(&p);
    }
}

// Object
static BOOL PutObject(const tObject * object, const char *sectionName)
{
    if (object)
    {
        char    modifier = GetObjectModifier(sectionName);

        PutString(ResolveStringModifier(object->objectName, modifier));

        if (isNeedSection(object))
        {
            char   *section = GetObjectSection(sectionName);

            if (section)
            {
                PutString(" (");
                PutString(section);
                PutString(")");
                Free(&section);
            }
        }
        return TRUE;
    }
    return FALSE;
}

// Address
static void PutSectionAddress(tOverlay * overlay)
{
    if (overlay->address)
    {
        PutAddress(overlay->address);
    }
    else
    {
        tAfter *t = overlay->afters.head;
        if (t)
        {
            fprintf(fp_out, "AFTER(%s", t->name);
            while (NULL != (t = t->next))
            {
                fprintf(fp_out, ",%s", t->name);
            }
            fprintf(fp_out, ")");
        }
    }
}


//======== STATIC ========//

// TargetName
static void PutTargetName(const char *format)
{
    char* str;
    int   pos;
    
    if (format == NULL || format[0] == '\0')
    {
        PutString(Static.targetName ? Static.targetName : Static.name);
    }
    else if (Static.targetName && Static.targetName[0])
    {
        str = GetObjectParam(format, '=', '\0');
        pos = GetCharPos(str, '%');
        if (str[pos] == '%')
        {
            str[pos] = '\0';
            PutString(str);
            PutString(Static.targetName);
            PutString(str+pos+1);
        }
        Free(&str);
    }
}

// Object
static void PutStaticObject(const char *sectionName)
{
    if (!PutObject(StaticObject, sectionName))
    {
        lcf_error("No <FOREACH.STATIC.OBJECTS> Ignored.");
    }
}

// Foreach Objects
static int ForeachStaticObjects(int n)
{
    return ForeachObjects(&StaticObject, Static.objects.head, &Foreach.static_object, n);
}

// If Object First/Last
static void PutIfStaticObjectFirst(const char *str)
{
    PutIfObject(Foreach.static_object.isFirst, str);
}

static void PutIfStaticObjectLast(const char *str)
{
    PutIfObject(Foreach.static_object.isLast, str);
}

// Library
static void PutStaticLibrary(const char *sectionName)
{
    if (!PutObject(StaticLibrary, sectionName))
    {
        lcf_error("No <FOREACH.STATIC.LIBRARIES> Ignored.");
    }
}

// Foreach Libraries
static int ForeachStaticLibraries(int n)
{
    return ForeachObjects(&StaticLibrary, Static.libraries.head, &Foreach.static_library, n);
}

// If Library First/Last
static void PutIfStaticLibraryFirst(const char *str)
{
    PutIfObject(Foreach.static_library.isFirst, str);
}

static void PutIfStaticLibraryLast(const char *str)
{
    PutIfObject(Foreach.static_library.isLast, str);
}

// SearchSymbol
static void PutStaticSearchSymbol(const char *dummy)
{
    if (!PutObject(StaticSearchSymbol, NULL))
    {
        lcf_error("No <FOREACH.STATIC.SEARCHSYMBOLS> Ignored.");
    }
}

// Foreach SearchSymbols
static int ForeachStaticSearchSymbols(int n)
{
    return ForeachObjects(&StaticSearchSymbol, Static.searchSymbols.head, &Foreach.static_searchsymbol, n);
}

// If SearchSymbol First/Last
static void PutIfStaticSearchSymbolFirst(const char *str)
{
    PutIfObject(Foreach.static_searchsymbol.isFirst, str);
}

static void PutIfStaticSearchSymbolLast(const char *str)
{
    PutIfObject(Foreach.static_searchsymbol.isLast, str);
}


//======== AUTOLOAD ========//

// Id
static void PutAutoloadId(void)
{
    if (Autoload)
        PutNumber(Autoload->id);
    else
        lcf_error("No <FOREACH.AUTOLOADS> Ignored.");
}

// Name
static void PutAutoloadName(void)
{
    if (Autoload)
        PutString(Autoload->name);
    else
        lcf_error("No <FOREACH.AUTOLOADS> Ignored.");
}

// Address
static void PutAutoloadAddress(void)
{
    if (Autoload)
        PutSectionAddress(Autoload);
    else
        lcf_error("No <FOREACH.AUTOLOADS> Ignored.");
}

// Object
static void PutAutoloadObject(const char *sectionName)
{
    if (!PutObject(AutoloadObject, sectionName))
    {
        lcf_error("No <FOREACH.AUTOLOAD.OBJECTS> Ignored.");
    }
}

// Foreach Objects
static int ForeachAutoloadObjects(int n)
{
    if (Autoload)
    {
        return ForeachObjects(&AutoloadObject, Autoload->objects.head, &Foreach.autoload_object, n);
    }
    else
    {
        lcf_error("No <FOREACH.AUTOLOADS> Ignored.");
        return tokenBuffer[n].loop_end;
    }
}

// If Object First/Last
static void PutIfAutoloadObjectFirst(const char *str)
{
    PutIfObject(Foreach.autoload_object.isFirst, str);
}

static void PutIfAutoloadObjectLast(const char *str)
{
    PutIfObject(Foreach.autoload_object.isLast, str);
}

// Library
static void PutAutoloadLibrary(const char *sectionName)
{
    if (!PutObject(AutoloadLibrary, sectionName))
    {
        lcf_error("No <FOREACH.AUTOLOAD.LIBRARIES> Ignored.");
    }
}

// Foreach Libraries
static int ForeachAutoloadLibraries(int n)
{
    if (Autoload)
    {
        return ForeachObjects(&AutoloadLibrary, Autoload->libraries.head, &Foreach.autoload_library, n);
    }
    else
    {
        lcf_error("No <FOREACH.AUTOLOADS> Ignored.");
        return tokenBuffer[n].loop_end;
    }
}

// If Library First/Last
static void PutIfAutoloadLibraryFirst(const char *str)
{
    PutIfObject(Foreach.autoload_library.isFirst, str);
}

static void PutIfAutoloadLibraryLast(const char *str)
{
    PutIfObject(Foreach.autoload_library.isLast, str);
}

// SearchSymbol
static void PutAutoloadSearchSymbol(const char *dummy)
{
    if (!PutObject(AutoloadSearchSymbol, NULL))
    {
        lcf_error("No <FOREACH.AUTOLOAD.SEARCHSYMBOLS> Ignored.");
    }
}

// Foreach SearchSymbols
static int ForeachAutoloadSearchSymbols(int n)
{
    if (Autoload)
    {
        return ForeachObjects(&AutoloadSearchSymbol, Autoload->searchSymbols.head, &Foreach.autoload_searchsymbol, n);
    }
    else
    {
        lcf_error("No <FOREACH.AUTOLOADS> Ignored.");
        return tokenBuffer[n].loop_end;
    }
}

// If SearchSymbol First/Last
static void PutIfAutoloadSearchSymbolFirst(const char *str)
{
    PutIfObject(Foreach.autoload_searchsymbol.isFirst, str);
}

static void PutIfAutoloadSearchSymbolLast(const char *str)
{
    PutIfObject(Foreach.autoload_searchsymbol.isLast, str);
}

// Foreach Autoloads
static int ForeachAutoloads(int n)
{
    tOverlay *start;
    tTokenBuffer *token = &tokenBuffer[n];
    tMemType memtype = GetMemTypeParam(token->string);
    int count, num;
    
    start = AutoloadList.head;
    num = 0;
    while (start)
    {
        if (memtype == MEMTYPE_NONE || memtype == RecgSectionMemTypeByName(start->name))
        {
            num ++;
        }
        start = start->next;
    }
    
    start = AutoloadList.head;
    count = 0;
    while (start)
    {
        if (memtype == MEMTYPE_NONE || memtype == RecgSectionMemTypeByName(start->name))
        {
            Autoload = start;
            Foreach.autoload.isFirst = (count ? FALSE : TRUE);
            Foreach.autoload.isLast  = (count == num - 1 ? TRUE : FALSE);
            Foreach.autoload.count   = count;
            PutTokenBuffer(n + 1, tokenBuffer[n].loop_end);
            count ++;
        }
            start = start->next;
    }
    Autoload = NULL;
    return tokenBuffer[n].loop_end;
}

// If Autoload First/Last
static void PutIfAutoloadFirst(const char *str)
{
    PutIfObject(Foreach.autoload.isFirst, str);
}

static void PutIfAutoloadLast(const char *str)
{
    PutIfObject(Foreach.autoload.isLast, str);
}


//======== OVERLAY ========//

// Id
static void PutOverlayId(void)
{
    if (Overlay)
        PutNumber(Overlay->id);
    else
        lcf_error("No <FOREACH.OVERLAYS> Ignored.");
}

// Name
static void PutOverlayName(void)
{
    if (Overlay)
        PutString(Overlay->name);
    else
        lcf_error("No <FOREACH.OVERLAYS> Ignored.");
}

// Group
static void PutOverlayGroup(void)
{
    if (Overlay)
    {
        // Group が設定されていない場合はオーバーレイの名前をグループとして使う
        PutString(Overlay->group ? Overlay->group : Overlay->name);
    }
    else
    {
        lcf_error("No <FOREACH.OVERLAYS> Ignored.");
    }
}

// Address
static void PutOverlayAddress(void)
{
    if (Overlay)
        PutSectionAddress(Overlay);
    else
        lcf_error("No <FOREACH.OVERLAYS> Ignored.");
}

// Object
static void PutOverlayObject(const char *sectionName)
{
    if (!PutObject(OverlayObject, sectionName))
    {
        lcf_error("No <FOREACH.OVERLAY.OBJECTS> Ignored.");
    }
}

// Foreach Objects
static int ForeachOverlayObjects(int n)
{
    if (Overlay)
    {
        return ForeachObjects(&OverlayObject, Overlay->objects.head, &Foreach.overlay_object, n);
    }
    else
    {
        lcf_error("No <FOREACH.OVERLAYS> Ignored.");
        return tokenBuffer[n].loop_end;
    }
}

// If Object First/Last
static void PutIfOverlayObjectFirst(const char *str)
{
    PutIfObject(Foreach.overlay_object.isFirst, str);
}

static void PutIfOverlayObjectLast(const char *str)
{
    PutIfObject(Foreach.overlay_object.isLast, str);
}


// Library
static void PutOverlayLibrary(const char *sectionName)
{
    if (!PutObject(OverlayLibrary, sectionName))
    {
        lcf_error("No <FOREACH.OVERLAY.LIBRARIES> Ignored.");
    }
}

// Foreach Libraries
static int ForeachOverlayLibraries(int n)
{
    if (Overlay)
    {
        return ForeachObjects(&OverlayLibrary, Overlay->libraries.head, &Foreach.overlay_library, n);
    }
    else
    {
        lcf_error("No <FOREACH.OVERLAYS> Ignored.");
        return tokenBuffer[n].loop_end;
    }
}

// If Library First/Last
static void PutIfOverlayLibraryFirst(const char *str)
{
    PutIfObject(Foreach.overlay_library.isFirst, str);
}

static void PutIfOverlayLibraryLast(const char *str)
{
    PutIfObject(Foreach.overlay_library.isLast, str);
}


// SearchSymbol
static void PutOverlaySearchSymbol(const char *dummy)
{
    if (!PutObject(OverlaySearchSymbol, NULL))
    {
        lcf_error("No <FOREACH.OVERLAY.SEARCHSYMBOLS> Ignored.");
    }
}

// Foreach SearchSymbols
static int ForeachOverlaySearchSymbols(int n)
{
    if (Overlay)
    {
        return ForeachObjects(&OverlaySearchSymbol, Overlay->searchSymbols.head, &Foreach.overlay_searchsymbol, n);
    }
    else
    {
        lcf_error("No <FOREACH.OVERLAYS> Ignored.");
        return tokenBuffer[n].loop_end;
    }
}

// If SearchSymbol First/Last
static void PutIfOverlaySearchSymbolFirst(const char *str)
{
    PutIfObject(Foreach.overlay_searchsymbol.isFirst, str);
}

static void PutIfOverlaySearchSymbolLast(const char *str)
{
    PutIfObject(Foreach.overlay_searchsymbol.isLast, str);
}

// Foreach Overlays
static int ForeachOverlays(int n)
{
    tOverlay *start;
    tTokenBuffer *token = &tokenBuffer[n];
    tMemType memtype = GetMemTypeParam(token->string);
    int count, num;
    
    start = OverlayList.head;
    num = 0;
    while (start)
    {
        if (memtype == MEMTYPE_NONE || memtype == RecgSectionMemTypeByName(start->name))
        {
            num ++;
        }
        start = start->next;
    }
    
    start = OverlayList.head;
    count = 0;
    while (start)
    {
        if (memtype == MEMTYPE_NONE || memtype == RecgSectionMemTypeByName(start->name))
        {
            Overlay = start;
            Foreach.overlay.isFirst = (count ? FALSE : TRUE);
            Foreach.overlay.isLast  = (count == num - 1 ? TRUE : FALSE);
            Foreach.overlay.count   = count;
            PutTokenBuffer(n + 1, tokenBuffer[n].loop_end);
            count ++;
        }
        start = start->next;
    }
    Overlay = NULL;
    return tokenBuffer[n].loop_end;
}

// If Overlay First/Last
static void PutIfOverlayFirst(const char *str)
{
    PutIfObject(Foreach.overlay.isFirst, str);
}

static void PutIfOverlayLast(const char *str)
{
    PutIfObject(Foreach.overlay.isLast, str);
}


//======== MISC ========//

static BOOL IsExistStatic(const char *str)
{
    return strcmp(Static.name, str) ? FALSE : TRUE;
}

static BOOL IsExistAutoload(const char *str)
{
    tOverlay *t = AutoloadList.head;
    while (t)
    {
        if (!strcmp(t->name, str))
        {
            return TRUE;
        }
        t = t->next;
    }
    return FALSE;
}

static BOOL IsExistOverlay(const char *str)
{
    tOverlay *t = OverlayList.head;
    while (t)
    {
        if (!strcmp(t->name, str))
        {
            return TRUE;
        }
        t = t->next;
    }
    return FALSE;
}

static void PutIfExistSection(const char *str)
{
    char *name = GetObjectParam(str, '.', '=');
    debug_printf("ExistSection? name = %s\n", name ? name : "");
    if (name)
    {
        PutIfObject(IsExistStatic(name)||IsExistAutoload(name)||IsExistOverlay(name), str);
        Free(&name);
    }
    return;
}

static void PutIfExistStatic(const char *str)
{
    char *name = GetObjectParam(str, '.', '=');
    debug_printf("ExistStatic? name = %s\n", name ? name : "");
    if (name)
    {
        PutIfObject(IsExistStatic(name), str);
        Free(&name);
    }
    return;
}

static void PutIfExistAutoload(const char *str)
{
    char *name = GetObjectParam(str, '.', '=');
    debug_printf("ExistAutoload? name = %s\n", name ? name : "");
    if (name)
    {
        PutIfObject(IsExistAutoload(name), str);
        Free(&name);
    }
    return;
}

static void PutIfExistOverlay(const char *str)
{
    char *name = GetObjectParam(str, '.', '=');
    debug_printf("ExistOverlay? name = %s\n", name ? name : "");
    if (name)
    {
        PutIfObject(IsExistOverlay(name), str);
        Free(&name);
    }
    return;
}


/*============================================================================
 *  Put Token List
 */
static int PutToken(int n)
{
    switch (tokenBuffer[n].id)
    {
    case tSTRING:
        PutString(tokenBuffer[n].string);
        break;

    case tTARGET_NAME:
        PutTargetName(tokenBuffer[n].string);
        break;
        
    case tSTATIC_NAME:
        PutString(Static.name);
        break;

    case tSTATIC_ADDRESS:
        PutAddress(Static.address);
        break;

    case tSTATIC_OBJECT:
        PutStaticObject(tokenBuffer[n].string);
        break;

    case tSTATIC_LIBRARY:
        PutStaticLibrary(tokenBuffer[n].string);
        break;

    case tSTATIC_SEARCHSYMBOL:
        PutStaticSearchSymbol(tokenBuffer[n].string);
        break;

    case tSTATIC_STACKSIZE:
        PutNumber(Static.stacksize);
        break;

    case tSTATIC_IRQSTACKSIZE:
        PutNumber(Static.stacksize_irq);
        break;

    case tFOREACH_STATIC_OBJECTS:
        n = ForeachStaticObjects(n);
        break;

    case tFOREACH_STATIC_LIBRARIES:
        n = ForeachStaticLibraries(n);
        break;

    case tFOREACH_STATIC_SEARCHSYMBOLS:
        n = ForeachStaticSearchSymbols(n);
        break;

    case tIF_STATIC_OBJECT_FIRST:
        PutIfStaticObjectFirst(tokenBuffer[n].string);
        break;
        
    case tIF_STATIC_OBJECT_LAST:
        PutIfStaticObjectLast(tokenBuffer[n].string);
        break;

    case tIF_STATIC_LIBRARY_FIRST:
        PutIfStaticLibraryFirst(tokenBuffer[n].string);
        break;
        
    case tIF_STATIC_LIBRARY_LAST:
        PutIfStaticLibraryLast(tokenBuffer[n].string);
        break;

    case tIF_STATIC_SEARCHSYMBOL_FIRST:
        PutIfStaticSearchSymbolFirst(tokenBuffer[n].string);
        break;
        
    case tIF_STATIC_SEARCHSYMBOL_LAST:
        PutIfStaticSearchSymbolLast(tokenBuffer[n].string);
        break;

    case tAUTOLOAD_ID:
        PutAutoloadId();
        break;

    case tAUTOLOAD_NAME:
        PutAutoloadName();
        break;

    case tAUTOLOAD_ADDRESS:
        PutAutoloadAddress();
        break;

    case tAUTOLOAD_OBJECT:
        PutAutoloadObject(tokenBuffer[n].string);
        break;

    case tAUTOLOAD_LIBRARY:
        PutAutoloadLibrary(tokenBuffer[n].string);
        break;

    case tAUTOLOAD_SEARCHSYMBOL:
        PutAutoloadSearchSymbol(tokenBuffer[n].string);
        break;

    case tNUMBER_AUTOLOADS:
        PutNumber(AutoloadList.num);
        break;

    case tFOREACH_AUTOLOADS:
        n = ForeachAutoloads(n);
        break;

    case tFOREACH_AUTOLOAD_OBJECTS:
        n = ForeachAutoloadObjects(n);
        break;

    case tFOREACH_AUTOLOAD_LIBRARIES:
        n = ForeachAutoloadLibraries(n);
        break;

    case tFOREACH_AUTOLOAD_SEARCHSYMBOLS:
        n = ForeachAutoloadSearchSymbols(n);
        break;

    case tIF_AUTOLOAD_FIRST:
        PutIfAutoloadFirst(tokenBuffer[n].string);
        break;
        
    case tIF_AUTOLOAD_LAST:
        PutIfAutoloadLast(tokenBuffer[n].string);
        break;

    case tIF_AUTOLOAD_OBJECT_FIRST:
        PutIfAutoloadObjectFirst(tokenBuffer[n].string);
        break;
        
    case tIF_AUTOLOAD_OBJECT_LAST:
        PutIfAutoloadObjectLast(tokenBuffer[n].string);
        break;

    case tIF_AUTOLOAD_LIBRARY_FIRST:
        PutIfAutoloadLibraryFirst(tokenBuffer[n].string);
        break;
        
    case tIF_AUTOLOAD_LIBRARY_LAST:
        PutIfAutoloadLibraryLast(tokenBuffer[n].string);
        break;

    case tIF_AUTOLOAD_SEARCHSYMBOL_FIRST:
        PutIfAutoloadSearchSymbolFirst(tokenBuffer[n].string);
        break;
        
    case tIF_AUTOLOAD_SEARCHSYMBOL_LAST:
        PutIfAutoloadSearchSymbolLast(tokenBuffer[n].string);
        break;

    case tOVERLAY_ID:
        PutOverlayId();
        break;

    case tOVERLAY_NAME:
        PutOverlayName();
        break;

    case tOVERLAY_GROUP:
        PutOverlayGroup();
        break;

    case tOVERLAY_ADDRESS:
        PutOverlayAddress();
        break;

    case tOVERLAY_OBJECT:
        PutOverlayObject(tokenBuffer[n].string);
        break;

    case tOVERLAY_LIBRARY:
        PutOverlayLibrary(tokenBuffer[n].string);
        break;

    case tOVERLAY_SEARCHSYMBOL:
        PutOverlaySearchSymbol(tokenBuffer[n].string);
        break;

    case tNUMBER_OVERLAYS:
        PutNumber(OverlayList.num);
        break;

    case tFOREACH_OVERLAYS:
        n = ForeachOverlays(n);
        break;

    case tFOREACH_OVERLAY_OBJECTS:
        n = ForeachOverlayObjects(n);
        break;

    case tFOREACH_OVERLAY_LIBRARIES:
        n = ForeachOverlayLibraries(n);
        break;

    case tFOREACH_OVERLAY_SEARCHSYMBOLS:
        n = ForeachOverlaySearchSymbols(n);
        break;

    case tIF_OVERLAY_FIRST:
        PutIfOverlayFirst(tokenBuffer[n].string);
        break;
        
    case tIF_OVERLAY_LAST:
        PutIfOverlayLast(tokenBuffer[n].string);
        break;

    case tIF_OVERLAY_OBJECT_FIRST:
        PutIfOverlayObjectFirst(tokenBuffer[n].string);
        break;
        
    case tIF_OVERLAY_OBJECT_LAST:
        PutIfOverlayObjectLast(tokenBuffer[n].string);
        break;

    case tIF_OVERLAY_LIBRARY_FIRST:
        PutIfOverlayLibraryFirst(tokenBuffer[n].string);
        break;
        
    case tIF_OVERLAY_LIBRARY_LAST:
        PutIfOverlayLibraryLast(tokenBuffer[n].string);
        break;

    case tIF_OVERLAY_SEARCHSYMBOL_FIRST:
        PutIfOverlaySearchSymbolFirst(tokenBuffer[n].string);
        break;
        
    case tIF_OVERLAY_SEARCHSYMBOL_LAST:
        PutIfOverlaySearchSymbolLast(tokenBuffer[n].string);
        break;

    case tPROPERTY_OVERLAYDEFS:
        PutString(Property.overlaydefs);
        break;

    case tPROPERTY_OVERLAYTABLE:
        PutString(Property.overlaytable);
        break;

    case tPROPERTY_SUFFIX:
        PutString(Property.suffix);
        break;

    case tIF_EXIST_SECTION:
        PutIfExistSection(tokenBuffer[n].string);
        break;

    case tIF_EXIST_STATIC:
        PutIfExistStatic(tokenBuffer[n].string);
        break;

    case tIF_EXIST_AUTOLOAD:
        PutIfExistAutoload(tokenBuffer[n].string);
        break;

    case tIF_EXIST_OVERLAY:
        PutIfExistOverlay(tokenBuffer[n].string);
        break;

    default:
        lcf_error("Unknown token\n");
    }
    return n;
}

static void PutTokenBuffer(int start, int end)
{
    int     i;

    for (i = start; i <= end; i++)
    {
        i = PutToken(i);
    }
}

static void PutTokenBufferAll(void)
{
    PutTokenBuffer(0, tokenBufferEnd - 1);
}


/*============================================================================
 *  Create LCF file
 */
int CreateLcfFile(const char *filename)
{
    if (filename)
    {
        if (NULL == (fp_out = fopen(filename, "wb")))
        {
            fprintf(stderr, "makelcf: Cannot write %s\n", filename);
            return 2;
        }
        PutTokenBufferAll();
        fclose(fp_out);
    }
    else
    {
#ifdef _WIN32
        setmode(1, O_BINARY);
#endif
        fp_out = stdout;
        PutTokenBufferAll();
    }

    return 0;
}

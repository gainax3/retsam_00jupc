/*---------------------------------------------------------------------------*
  Project:  NitroSDK - bin2obj
  File:     object.h

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: object.h,v $
  Revision 1.2  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.1  2005/06/10 12:55:43  yasu
  1st リファクタリング

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef	OBJECT_H__
#define	OBJECT_H__

#include "types.h"
#include "elf.h"

typedef struct
{
    u32     size;
    u8     *ptr;
}
Section;

// Elf Header
//#define       SIZE_ELFHEADER          sizeof(ELF32_ElfHeader)

// Section List
#define	MAX_SECTIONS		16
//#define       SECTION_NULL            0       // index=0 のセクションはダミー=NULL
//#define       SECTION_SHSTRTAB        1       // index=1 セクション名テーブル
//#define       SECTION_SYMTAB          2       // index=2 シンボル情報構造体
//#define       SECTION_STRTAB          3       // index=3 シンボル名テーブル
//#define       SECTION_DATA            4       // index=4 データセクション

// Symbol List
#define	MAX_SYMBOLS		16
//#define       SYMBOL_NULL             0       // index=0 のシンボルはダミー=NULL
//#define       SYMBOL_LABEL_BEGIN      1       // index=1 領域開始位置
//#define       SYMBOL_LABEL_END        2       // index=2 領域終了位置

// Data Section
#define	MAX_DATA		16

typedef struct
{
    u32     index;
    Section section;

}
DataSection;


// Object
typedef struct
{
    // Elf Header
    ELF32_ElfHeader header;

    // Section Header
    ELF32_SectionHeader section[MAX_SECTIONS];

    // Section String Section (.shstrtab)
    Section section_name;

    // Symbol Section         (.symtab)
    u32     symbol_index;
    ELF32_Symbol symbol[MAX_SYMBOLS];

    // Symbol String Section  (.strtab)
    u32     symbol_name_index;
    Section symbol_name;

    // Data Section           (.data) or (.rodata)
    u32     num_data;
    DataSection data[MAX_DATA];

}
Object;

#endif //BIN2OBJ_H__

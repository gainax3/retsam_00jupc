/*---------------------------------------------------------------------------*
  Project:  NitroSDK - bin2obj
  File:     elf.h

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: elf.h,v $
  Revision 1.6  2006/07/21 07:57:19  yasu
  PowerPC 対応

  Revision 1.5  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.4  2005/11/29 11:13:03  yasu
  ライブラリのファイルアーカイブ形式の記述追加

  Revision 1.3  2005/11/29 01:52:30  yasu
  WEAK シンボルフラグの定義追加

  Revision 1.2  2005/06/10 12:55:42  yasu
  1st リファクタリング

  Revision 1.1  2005/06/10 00:43:01  yasu
  初期版

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef	ELF_H__
#define	ELF_H__

//
// ELF HEADER
//
#define	EI_NIDENT	16

typedef struct
{
    u8      e_ident[EI_NIDENT];
    u16     e_type;
    u16     e_machine;
    u32     e_version;
    u32     e_entry;
    u32     e_phoff;                   // ProgramHeader テーブルへのオフセット
    u32     e_shoff;                   // SectionHeader テーブルへのオフセット
    u32     e_flags;
    u16     e_ehsize;                  // ElfHeader のサイズ
    u16     e_phentsize;               // ProgramHeader テーブルのサイズ
    u16     e_phnum;                   // ProgramHeader の数
    u16     e_shentsize;               // SectionHeader テーブルのサイズ
    u16     e_shnum;                   // SectionHeader の数
    u16     e_shstrndx;                // Section 名称テーブルの index 番号
}
ELF32_ElfHeader;

// e_ident
#define	ELFMAG0			0x7f
#define	ELFMAG1			'E'
#define	ELFMAG2			'L'
#define	ELFMAG3			'F'
#define ELFCLASS32		1

#define ELFDATA2UNDEF		0
#define ELFDATA2LSB		1      // LittleEndian
#define ELFDATA2MSB		2      // BigEndian

// e_type
#define	ET_REL			1
// e_machine
#define	EM_PPC			20     // PowerPC
#define	EM_ARM			40     // ARM
// e_version
#define	EV_CURRENT		1

//
// SECTION HEADER
//
typedef struct
{
    u32     sh_name;                   // Section名テーブルの開始 index
    u32     sh_type;                   // Section種  SHT_*
    u32     sh_flags;
    u32     sh_addr;
    u32     sh_offset;
    u32     sh_size;
    u32     sh_link;
    u32     sh_info;
    u32     sh_addralign;
    u32     sh_entsize;
}
ELF32_SectionHeader;

// sh_type
#define	SHT_NULL		0
#define	SHT_PROGBITS		1
#define	SHT_SYMTAB		2
#define	SHT_STRTAB		3

#define	SHF_NULL		0x0
#define	SHF_WRITE		0x1
#define	SHF_ALLOC		0x2

//
// SYMBOL TABLE
//
typedef struct
{
    u32     st_name;                   // Symbol名の開始 index
    u32     st_value;                  // Symbolの値(lvalue)
    u32     st_size;                   // Symbol自体のサイズ(objectサイズなど)
    u8      st_info;                   // STB_* と STT_* を連結する
    u8      st_other;                  // 現在は 0 固定
    u16     st_shndx;
}
ELF32_Symbol;

#define	ELF32_ST_BIND(i)	((i)>>4)
#define	ELF32_ST_TYPE(i)	((i)&0xf)
#define	ELF32_ST_INFO(b,t)	(((b)<<4)+((t)&0xf))

#define	STB_LOCAL		0
#define	STB_GLOBAL		1
#define	STB_WEAK		2
#define	STB_WEAK_CW		14     // for CodeWarrior

#define	STT_NOTYPE		0
#define	STT_OBJECT		1
#define	STT_FUNC		2
#define	STT_SECTION		3
#define	STT_FILE		4

// ARCHIVE Format
#define ARMAG_STRING		"!<arch>\n"
#define ARMAG_SIZE		(sizeof(ARMAG_STRING)-1)
#define ARFMAG_STRING		"`\n"
#define ARFMAG_SIZE		(sizeof(ARFMAG_STRING)-1)

#define AR_NAME_SIZE		16
#define AR_DATE_SIZE		12
#define AR_UID_SIZE		6
#define AR_GID_SIZE		6
#define AR_MODE_SIZE		8
#define AR_SIZE_SIZE		10
#define AR_FMAG_SIZE		2

typedef struct
{
    char    ar_name[AR_NAME_SIZE];
    char    ar_date[AR_DATE_SIZE];
    char    ar_uid[AR_UID_SIZE];
    char    ar_gid[AR_GID_SIZE];
    char    ar_mode[AR_MODE_SIZE];
    char    ar_size[AR_SIZE_SIZE];
    char    ar_fmag[AR_FMAG_SIZE];
}
ARCHIVE_MemberHeader;

#endif //ELF_H__

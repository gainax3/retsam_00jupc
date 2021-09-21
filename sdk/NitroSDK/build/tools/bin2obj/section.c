/*---------------------------------------------------------------------------*
  Project:  NitroSDK - bin2obj
  File:     section.c

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: section.c,v $
  Revision 1.5  2006/01/18 02:12:28  kitase_hirotake
  do-indent

  Revision 1.4  2005/06/14 00:57:07  yasu
  -s オプションの追加

  Revision 1.3  2005/06/13 08:52:26  yasu
  %f をシンボル名に対応

  Revision 1.2  2005/06/13 02:56:34  yasu
  オプションの修正、テスト作成による動作の確認

  Revision 1.1  2005/06/10 12:55:43  yasu
  1st リファクタリング

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include "bin2obj.h"

static s32 read_datasec(Object * obj, const char *filename);
static u32 section_add_string(Section * s, const char *string);

/*---------------------------------------------------------------------------*
  Name:         add_datasec
  
  Description:  データセクションを追加する

  Arguments:    obj                   オブジェクトへのポインタ
                section_rodata        読み込み専用のデータセクション名
                section_rwdata        読み書き可能なデータセクション名
                symbol_format_begin   シンボル名(%f 変換前)-先頭
                symbol_format_end     シンボル名(%f 変換前)-最後尾
                filename              入力バイナリファイル名
                writable              TRUE .data  FALSE .rodata
                align                 アラインメント
  Returns:      TRUE 成功  FALSE 失敗
 *---------------------------------------------------------------------------*/
BOOL add_datasec(Object * obj,
                 const char *section_rodata, const char *section_rwdata,
                 const char *symbol_format_begin, const char *symbol_format_end,
                 const char *filename, BOOL writable, u32 align)
{
    s32     n;
    DataSection *d;
    char   *symbol_begin;              // シンボル名(%f 変換後)-先頭
    char   *symbol_end;                // シンボル名(%f 変換後)-最後尾

    //
    //  データセクション読み込み
    //
    if (0 > (n = read_datasec(obj, filename)))
    {
        return FALSE;
    }

    //
    //  データセクションの登録
    //
    d = &obj->data[n];
    if (writable)                      // .rodata or .data の処理
    {
        d->index =
            add_section(obj, section_rwdata, SHT_PROGBITS, SHF_WRITE | SHF_ALLOC, d->section.size,
                        align);
    }
    else
    {
        d->index =
            add_section(obj, section_rodata, SHT_PROGBITS, SHF_ALLOC, d->section.size, align);
    }

    //
    //  シンボルの登録
    //
    symbol_begin = create_symbol_string(filename, symbol_format_begin);
    symbol_end = create_symbol_string(filename, symbol_format_end);
    (void)add_symbol(obj, symbol_begin, 0, d->section.size, d->index);
    (void)add_symbol(obj, symbol_end, d->section.size, 0, d->index);
    free(symbol_begin);
    free(symbol_end);

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         read_datasec
  
  Description:  データセクションを読み込む

  Arguments:    obj        オブジェクトへのポインタ
                filename   入力バイナリファイル名
  
  Returns:      TRUE 成功  FALSE 失敗
 *---------------------------------------------------------------------------*/
static s32 read_datasec(Object * obj, const char *filename)
{
    struct stat st;
    FILE   *fp;
    u32     n = obj->num_data;
    Section *s = &obj->data[n].section;

    //
    //  data section の領域をヒープから入力ファイルサイズ分だけ確保し、
    //  そこへ入力ファイル filename を読み込む
    //
    if (stat(filename, &st) || !S_ISREG(st.st_mode) || st.st_size < 0
        || NULL == (fp = fopen(filename, "rb")))
    {
        fprintf(stderr, "Error: Cannot open file %s.\n", filename);
        return -1;
    }

    s->size = st.st_size;

    if (NULL == (s->ptr = malloc(s->size)))
    {
        fprintf(stderr, "Error: No memory.\n");
        fclose(fp);
        return -1;
    }

    if (s->size != fread(s->ptr, sizeof(u8), s->size, fp))
    {
        fprintf(stderr, "Error: Cannot read file %s.\n", filename);
        free(s->ptr);
        fclose(fp);
        return -1;
    }
    fclose(fp);

    obj->num_data = n + 1;
    return n;
}

/*---------------------------------------------------------------------------*
  Name:         add_section_name

  Description:  セクション名の追加

  Arguments:    obj      オブジェクト
                name     セクション名
  
  Returns:      追加したセクション名のセクション名テーブルでの先頭位置
 *---------------------------------------------------------------------------*/
u32 add_section_name(Object * obj, const char *name)
{
    // セクション内への登録
    u32     pos = section_add_string(&obj->section_name, name);

    // セクション情報(サイズ)の更新
    if (obj->header.e_shstrndx > 0)
    {
        obj->section[obj->header.e_shstrndx].sh_size = obj->section_name.size;
    }
    return pos;
}

/*---------------------------------------------------------------------------*
  Name:         add_section
  
  Description:  セクションを追加

  Arguments:    obj     オブジェクトへのポインタ
                name    シンボル名(もしNULLなら NULL SECTION にする)
                type    SHT_*
                flags   SHF_*
                size    セクションのサイズ
                align   セクションのアラインメント
  
  Returns:      登録したindex
 *---------------------------------------------------------------------------*/
u32 add_section(Object * obj, const char *name, u32 type, u32 flags, u32 size, u32 align)
{
    ELF32_SectionHeader *s;
    u32     n;

    n = obj->header.e_shnum;
    s = &obj->section[n];

    if (name)
    {
        s->sh_name = add_section_name(obj, name);
        s->sh_type = type;
        s->sh_flags = flags;
        s->sh_addr = 0;
        s->sh_offset = 0;
        s->sh_size = size;
        s->sh_link = 0;
        s->sh_info = 0;
        s->sh_addralign = align;
        s->sh_entsize = 0;
    }
    else
    {
        (void)add_section_name(obj, "");
        memset(s, 0, sizeof(ELF32_SectionHeader));
    }
    obj->header.e_shnum = n + 1;

    return n;
}

/*---------------------------------------------------------------------------*
  Name:         add_symbol_name

  Description:  シンボル名の追加

  Arguments:    obj      オブジェクト
                name     シンボル名
  
  Returns:      追加したシンボル名のセクション名テーブルでの先頭位置
 *---------------------------------------------------------------------------*/
u32 add_symbol_name(Object * obj, const char *name)
{
    u32     pos;

    // セクション内への登録
    // name が NULL の場合は "" として扱われる
    pos = section_add_string(&obj->symbol_name, name ? name : "");

    // セクション情報(サイズ)の更新
    if (obj->symbol_name_index > 0)
    {
        obj->section[obj->symbol_name_index].sh_size = obj->symbol_name.size;
    }
    return pos;
}

/*---------------------------------------------------------------------------*
  Name:         add_symbol

  Description:  シンボルを追加する

  Arguments:    obj		オブジェクトへのポインタ
                symbol		シンボル名(もしNULLなら NULL SYMBOL にする)
                value		シンボルの値
                size		シンボルのサイズ
                section		関連セクション
  
  Returns:      登録した index
 *---------------------------------------------------------------------------*/
u32 add_symbol(Object * obj, const char *symbol, u32 value, u32 size, u32 section)
{
    ELF32_SectionHeader *symtab = &obj->section[obj->symbol_index];
    u32     n = symtab->sh_info + 1;
    ELF32_Symbol *l = &obj->symbol[n];

    if (obj->symbol_index > 0)
    {
        if (symbol)
        {
            // C 言語などのシンボルの場合は通常の関数名の前に '_' をつける
            l->st_name = add_symbol_name(obj, symbol);
            l->st_value = value;
            l->st_size = size;
            l->st_info = ELF32_ST_INFO(STB_GLOBAL, STT_OBJECT);
            l->st_other = 0;
            l->st_shndx = section;
        }
        else
        {
            (void)add_symbol_name(obj, NULL);
            memset(l, 0, sizeof(ELF32_Symbol));
        }

        // シンボルテーブル更新
        symtab->sh_info = n;
        symtab->sh_size = (n + 1) * symtab->sh_entsize;
    }
    else
    {
        fprintf(stderr, "Warning: no symbol section. [%s] is skipped.\n", symbol);
    }
    return n;
}

/*---------------------------------------------------------------------------*
  Name:         section_add_string

  Description:  セクションへの文字列の追加

  Arguments:    section  セクション
                string   文字列
  
  Returns:      追加した文字列のテーブルでの先頭位置(=呼び出し前のサイズ)
 *---------------------------------------------------------------------------*/
static u32 section_add_string(Section * s, const char *string)
{
    int     new_size = s->size + strlen(string) + 1;
    u8     *new_ptr;
    int     size;

    if (NULL == (new_ptr = malloc(new_size)))
    {
        fprintf(stderr, "Error: No memory.\n");
        exit(1);
    }
    memcpy(new_ptr, s->ptr, s->size);
    strcpy(new_ptr + s->size, string);
    free(s->ptr);

    size = s->size;
    s->ptr = new_ptr;
    s->size = new_size;

    return size;
}

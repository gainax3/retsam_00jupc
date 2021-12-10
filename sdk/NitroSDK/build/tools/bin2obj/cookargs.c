/*---------------------------------------------------------------------------*
  Project:  NitroSDK - bin2obj
  File:     cookargs.c

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: cookargs.c,v $
  Revision 1.6  2006/07/21 07:57:19  yasu
  PowerPC 対応

  Revision 1.5  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.4  2005/06/14 00:57:07  yasu
  -s オプションの追加

  Revision 1.3  2005/06/13 11:37:56  yasu
  bin2obj の --compatible オプション対応

  Revision 1.2  2005/06/13 08:52:26  yasu
  %f をシンボル名に対応

  Revision 1.1  2005/06/13 02:56:34  yasu
  オプションの修正、テスト作成による動作の確認

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include "bin2obj.h"
#include "version.h"

/*---------------------------------------------------------------------------*
  Name:         cook_args

  Description:  bin2obj メイン
 *---------------------------------------------------------------------------*/
void cook_args(Bin2ObjArgs * t, int argc, char *argv[])
{
    u16     default_machine;
    u8      default_endian;
    int     c;
    static struct option long_options[] = {
        {"begin", 1, 0, 'b'},
        {"end", 1, 0, 'e'},
        {"compatible", 0, 0, 'C'},
        {"section", 1, 0, 's'},
        {"align", 1, 0, 'a'},
        {"readonly", 0, 0, 'r'},
        {"big-endian", 0, 0, 'B'},
        {"little-endian", 0, 0, 'L'},
        {"machine", 0, 0, 'm'},
        {0, 0, 0, 0}
    };

    memset(t, 0, sizeof(Bin2ObjArgs));
    UnpackFileName(argv[0], NULL, &t->app_name, NULL);
    t->align = DEFAULT_ALIGN;
    t->writable = DEFALUT_WRITABLE;
    t->symbol_begin = StrDup(DEFAULT_SYMBOL_BEGIN);
    t->symbol_end = StrDup(DEFAULT_SYMBOL_END);
    t->section_rodata = StrDup(DEFAULT_SECTION_RODATA);
    t->section_rwdata = StrDup(DEFAULT_SECTION_RWDATA);
    
    t->machine = 0;
    t->endian  = ELFDATA2UNDEF;
    
    default_machine = EM_ARM;
    default_endian  = ELFDATA2LSB;
    
    while ((c = getopt_long(argc, argv, "b:e:Cs:a:rBLm:", long_options, NULL)) != -1)
    {
        switch (c)
        {
        case 'b':
            if (t->symbol_begin)
                free(t->symbol_begin);
            t->symbol_begin = StrDup(optarg);
            break;

        case 'e':
            if (t->symbol_end)
                free(t->symbol_end);
            t->symbol_end = StrDup(optarg);
            break;

        case 'C':
            // for Compatiblity with CodeWarrior BinToElf
            if (t->symbol_begin)
                free(t->symbol_begin);
            if (t->symbol_end)
                free(t->symbol_end);
            t->symbol_begin = StrDup(COMPATIBLE_SYMBOL_BEGIN);
            t->symbol_end = StrDup(COMPATIBLE_SYMBOL_END);
            break;

        case 's':
            if (t->section_rodata)
                free(t->section_rodata);
            if (t->section_rwdata)
                free(t->section_rwdata);
            t->section_rodata = StrDup(optarg);
            t->section_rwdata = StrDup(optarg);
            break;

        case 'a':
            t->align = atoi(optarg);
            if (t->align < 1)
                t->align = 1;
            break;

        case 'r':
            t->writable = FALSE;
            break;

        case 'B':
            t->endian       = ELFDATA2MSB;	// Big Endian
            default_machine = EM_PPC;
            break;
            
        case 'L':
            t->endian       = ELFDATA2LSB;	// Little Endian
            default_machine = EM_ARM;
            break;

        case 'm':
            if (!strcmp(optarg, "arm"))
            {
                t->machine     = EM_ARM;
                default_endian = ELFDATA2LSB;
            }
            else if (!strcmp(optarg, "ppc"))
            {
                t->machine     = EM_PPC;
                default_endian = ELFDATA2MSB;
            }
            break;
            
        case '?':
        case ':':
        default:
            goto usage;
        }
    }

    if (optind + 2 != argc)
    {
        goto usage;
    }

    t->binary_filename = StrDup(argv[optind + 0]);
    t->object_filename = StrDup(argv[optind + 1]);

    // default endian & machine
    if (t->machine == 0)
    {
        t->machine = default_machine;
    }
    if (t->endian == ELFDATA2UNDEF)
    {
        t->endian = default_endian;
    }
    
    return;

  usage:
    {
        fprintf(stderr,
                "NITRO/RevoEX Software Development Tool - %s - Binary to Object converter \n"
                "Build %lu\n\n"
                "Usage: %s [-b|--begin SYMBOL_BEGIN]\n"
                "               [-e|--end   SYMBOL_END]\n"
                "               [-C|--compatible]\n"
                "               [-a|--align ALIGNMENT]\n"
                "               [-r|--readonly]\n"
                "               [-s|--section SECTION]\n"
                "               [-m|--machine [arm|ppc]]\n"
                "               [-B|--big-endian]\n"
                "               [-L|--little-endian] BINARY_FILE OBJECT_FILE\n\n"
                " -b or --begin             Set symbol name for top of binary. (*)\n"
                " -e or --end               Set symbol name for bottom of binary. (*)\n"
                " -C or --compatible        Use compatible symbols with BinToElf.exe.\n"
                "                           Same as \"-b _binary_%%f -e _binary_%%f_end\".\n"
                " -a or --align             Set binary data alignment in bytes.\n"
                " -r or --readonly          Handle binary data as readonly.\n"
                " -s or --section           Set section name.\n"
                " -m or --machine [arm|ppc] Machine arch [arm|ppc].(default=arm)\n"
                " -B or --big-endian        Output in big    endian format.\n"
                " -L or --little-endian     Output in little endian format.\n\n"
                "  (*) special %% rules for symbols (ex. binary_file = \"filename.dat\")\n"
                "       %%f,%%t replaced to file name of binary   (%%f = \"filename.dat\")\n"
                "       %%b    replaced to base name of binary   (%%b = \"filename\")\n"
                "       %%e    replaced to extension of binary   (%%e = \"dat\")\n\n",
                t->app_name, SDK_DATE_OF_LATEST_FILE, t->app_name);
        free_args(t);
        exit(-1);
    }
}

/*---------------------------------------------------------------------------*
  Name:         create_symbol_string

  Description:  シンボル名をバイナリファイル名から作成する
 *---------------------------------------------------------------------------*/
char   *create_symbol_string(const char *filename, const char *symbol_format)
{
    char   *symbol;
    char   *file_base;
    char   *file_ext;
    int     i;

    // ファイル名を分析して要素ごとに分ける
    UnpackFileName(filename, NULL, &file_base, &file_ext);

    // format 指定の中で %f,%b,%e という文字が出てくるか判定し、
    // 対応するファイル名情報(ファイル主幹名、拡張子など)と置き換える
    symbol = StrDup(symbol_format);
    for (i = 0; symbol[i] != '\0'; i++)
    {
        if (symbol[i] == '%')
        {
            switch (symbol[i + 1])
            {
            case 'b':
                i = replace_word(&symbol, i, 2, file_base);
                break;

            case 'e':
                i = replace_word(&symbol, i, 2, file_ext);
                break;

            case 'f':
            case 't':
                i = replace_word(&symbol, i, 2, file_base);
                i = replace_word(&symbol, i, 0, ".");
                i = replace_word(&symbol, i, 0, file_ext);
                break;
            }
        }
    }

    // シンボルとして使用できない文字が含まれているなら、その文字を _ にする
    for (i = 0; symbol[i] != '\0'; i++)
    {
        if (!isalnum(symbol[i]) && symbol[i] != '_')
        {
            symbol[i] = '_';
        }
    }
    if (isdigit(symbol[i]))
    {
        symbol[i] = '_';               // 1文字目は数字も不可
    }

    free(file_base);
    free(file_ext);

    return symbol;
}

/*---------------------------------------------------------------------------*
  Name:         replace_word

  Description:  文字列を置換する
  
  Arguments:    *symbol   置換される文字列(realloc される)
                pos       置換位置
                len       置換により削除される文字サイズ
                str       置換位置に挿入される文字列
  
                symbol[pos]～symbol[pos+len-1] までの文字が str に置き換わる
                str のサイズが len のサイズと異なる場合、文字列全体の長さが
                変化することになる.
  
  Returns:      挿入された文字列の最後尾の位置
 *---------------------------------------------------------------------------*/
int replace_word(char **str, int pos, int len, const char *substr)
{
    int     new_len, str_len, substr_len;
    char   *new_str;

    // 置換後の文字列のサイズの計算
    str_len = strlen(*str);
    assert(str_len >= pos + len);

    if (!substr)
        substr = "";
    substr_len = strlen(substr);
    new_len = str_len - len + substr_len;

    // 置換後の文字列の作成
    new_str = Calloc(new_len + 1);     // +1 for '\0'
    strncpy(new_str, *str, pos);
    strcat(new_str, substr);
    strcat(new_str, *str + pos + len);

    // realloc 処理
    free(*str);
    *str = new_str;

    // 新しい位置を返す
    return pos + substr_len;
}

/*---------------------------------------------------------------------------*
  Name:         free_args

  Description:  Bin2ObjArgs 領域のクリア
 *---------------------------------------------------------------------------*/
void free_args(Bin2ObjArgs * t)
{
    if (t->app_name)
        free(t->app_name);
    if (t->binary_filename)
        free(t->binary_filename);
    if (t->object_filename)
        free(t->object_filename);
    if (t->section_rodata)
        free(t->section_rodata);
    if (t->section_rwdata)
        free(t->section_rwdata);
    if (t->symbol_begin)
        free(t->symbol_begin);
    if (t->symbol_end)
        free(t->symbol_end);
    memset(t, 0, sizeof(Bin2ObjArgs));
}


/*---------------------------------------------------------------------------*
  Name:         StrDup

  Description:  strdup のエラーハンドリング版
  
  Arguments:    str     コピーする文字列
  
  Returns:      コピーされた文字列
 *---------------------------------------------------------------------------*/
char   *StrDup(const char *str)
{
    char   *cp;

    if (NULL == (cp = strdup(str ? str : "")))
    {
        fprintf(stderr, "Error: No memory.");
        exit(2);
    }
    return cp;
}

/*---------------------------------------------------------------------------*
  Name:         StrNDup

  Description:  文字長指定つきの strdup のエラーハンドリング版
  
  Arguments:    str     コピーする文字列
                len     コピーする文字長('\0'を含まない)
  
  Returns:      コピーされた文字列
 *---------------------------------------------------------------------------*/
char   *StrNDup(const char *str, int len)
{
    char   *cp;

    cp = Calloc(len + 1);
    if (str && len)
        (void)strncpy(cp, str, len);
    return cp;

    // malloc で len+1 バイト分領域を確保しているので
    // strncpy 後の '\0' の別途追加は必要ない．
    // また str が NULL のときも対応できる
}

/*---------------------------------------------------------------------------*
  Name:         StrCatDup

  Description:  文字列を連結したものをヒープ領域から確保する
  
  Arguments:    str1    文字列1 ==NULL の時は "" として扱われる
                str2    文字列2 ==NULL の時は "" として扱われる
  
  Returns:      連結した文字列
 *---------------------------------------------------------------------------*/
char   *StrCatDup(const char *str1, const char *str2)
{
    int     len1, len2;
    char   *cp;

    len1 = str1 ? strlen(str1) : 0;
    len2 = str2 ? strlen(str2) : 0;

    cp = Calloc(len1 + len2 + 1);

    if (str1)
        (void)strcpy(cp, str1);
    if (str2)
        (void)strcat(cp, str2);

    return cp;
}

/*---------------------------------------------------------------------------*
  Name:         Calloc

  Description:  calloc のエラーハンドリング版(引数は一つになっている)
  
  Arguments:    size    確保する領域長
  
  Returns:      確保された領域
 *---------------------------------------------------------------------------*/
void   *Calloc(int size)
{
    void   *cp;

    if (NULL == (cp = calloc(1, size)))
    {
        fprintf(stderr, "Error: No memory.");
        exit(2);
    }
    return cp;
}

/*---------------------------------------------------------------------------*
  Name:         UnpackFileName

  Description:  ファイル名を解析しパーツ毎
                (ディレクトリ部、ファイル主幹部、拡張子)に分割する
  
  Arguments:    path   フルパス名
                dir    ディレクトリ
                base   ファイル主幹
                ext    拡張子 '.' は含まない
  
                * dir,base,ext はヒープから確保しているので free が必要
                * dir,base,ext が NULL のときは値の代入はしない
  
  Returns:      なし
 *---------------------------------------------------------------------------*/
void UnpackFileName(const char *path, char **dir, char **base, char **ext)
{
    int     i, base_top, ext_top;
    int     path_len = strlen(path);

    // 最終の '.' の位置とパス区切りの位置を検索する
    ext_top = path_len;
    for (i = path_len - 1; i >= 0; i--)
    {
        if (path[i] == '.' && ext_top == path_len)
        {
            ext_top = i + 1;
        }
        if (path[i] == '/' || path[i] == '\\' || path[i] == ':')
        {
            break;
        }
    }
    base_top = i + 1;

    // 代入する
    if (dir)
        *dir = StrNDup(path, base_top);
    if (base)
        *base = StrNDup(path + base_top, ext_top - base_top - 1);
    if (ext)
        *ext = StrDup(path + ext_top);
    return;
}

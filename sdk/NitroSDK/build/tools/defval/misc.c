/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - defval
  File:     misc.c

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: misc.c,v $
  Revision 1.2  2006/01/18 02:11:20  kitase_hirotake
  do-indent

  Revision 1.1  2005/06/20 07:07:15  yasu
  add defval

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include "misc.h"

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
  Name:         Free

  Description:  free の NULL ハンドリング版
                変数 *ptr は NULL クリアされる
  
  Arguments:    ptr  free する領域
  
  Returns:      なし
 *---------------------------------------------------------------------------*/
void Free(void **ptr)
{
    if (NULL != *ptr)
    {
        free(*ptr);
        *ptr = NULL;
    }
    return;
}

/*---------------------------------------------------------------------------*
  Name:         Realloc

  Description:  realloc のエラーハンドリング版
  
  Arguments:    buffer  元のサイズ
                size    確保する領域長
  
  Returns:      確保された領域
 *---------------------------------------------------------------------------*/
void   *Realloc(void *buffer, int size)
{
    void   *cp;

    cp = Calloc(size);
    if (buffer)
    {
        strcpy(cp, buffer);
        free(buffer);
    }
    return cp;
}

/*---------------------------------------------------------------------------*
  Name:         Fopen

  Description:  fopen の stdin/stdout を考慮したもの
                '-' を指定したときにアクセスモード 'r' のとき stdin  に
                                    アクセスモード 'w' のとき stdout に設定
  
  Arguments:    name   ファイルバッファへのポインタ(へのポインタ)
                mode   ファイルバッファのサイズ(へのポインタ)
  
  Returns:      fp  ファイルポインタ
 *---------------------------------------------------------------------------*/
FILE   *Fopen(const char *filename, const char *mode)
{
    FILE   *fp;

    DebugPrintf("filename=[%s] mode=[%s]\n", filename, mode);

    if ('-' == filename[0] && '\0' == filename[1])
    {
        fp = (mode[0] == 'r') ? stdin : stdout;
    }
    else
    {
        if (NULL == (fp = fopen(filename, mode)))
        {
            fprintf(stderr, "Error: cannot open %s\n", filename);
            return NULL;
        }
    }

    DebugPrintf("fp=[%x] stdin=[%x] stdout=[%x]\n", fp, stdin, stdout);

    return fp;
}

/*---------------------------------------------------------------------------*
  Name:         Fclose

  Description:  fclose の stdin/stdout を考慮したもの
                NULL, stdin, stdout, stderr ならクローズしない
  
  Arguments:    fp  ファイルポインタ
  
  Returns:      なし
 *---------------------------------------------------------------------------*/
void Fclose(FILE * fp)
{
    if (fp != NULL && fp != stdin && fp != stdout && fp != stderr)
    {
        fclose(fp);
    }
    return;
}

/*---------------------------------------------------------------------------*
  Name:         Fgets

  Description:  ファイルを一行づつ読む。
                通常の fgets と異なり、動的なバッファサイズのコントロールに
                よって確実に一行分を読込む
  
  Arguments:    *buffer   ファイルバッファへのポインタ(へのポインタ)
                *size     ファイルバッファのサイズ(へのポインタ)
                fp        ファイル(=NULL なら stdin から読む)
  
      buffer, size は Fgets 内部において必要に応じ realloc されます。
      引数に size を与えるのは buffer を再利用し、alloc/free 処理を
      節約するためです。
      最初に *buffer に NULL を入れて呼び出すことを想定しています。
  
  Returns:      成功したら *buffer を、ファイルの終端などで失敗したら NULL
 *---------------------------------------------------------------------------*/
char   *Fgets(char **pbuffer, int *pbuffer_size, FILE * fp)
{
    char   *buffer;
    int     buffer_size;
    int     buffer_gets;

    assert(pbuffer);
    assert(pbuffer_size);

    buffer = *pbuffer;
    buffer_size = *pbuffer_size;

    if (buffer == NULL || buffer_size == 0)
    {
        buffer_size = DEFAULT_LINE_BUFFER_SIZE;
        buffer = Calloc(buffer_size);
    }

    buffer_gets = 0;

    while (NULL != fgets(buffer + buffer_gets, buffer_size - buffer_gets, fp))
    {
        //
        //  読み込んだ行が行末に達しているなら、定義値を登録する
        //    行末の判定は、fgets による読み込みにおいて
        //      1) バッファ領域の最後目一杯までデータが読み込まれなかったとき
        //      2) 一杯までデータは読まれたがそこで行が丁度終わっているとき
        //
        buffer_gets = strlen(buffer);
        if (buffer_gets < buffer_size - 1 || buffer[buffer_gets - 1] == '\n')
        {
            *pbuffer = buffer;
            *pbuffer_size = buffer_size;
            return buffer;
        }
        //
        //  読み込んだ行が行末にまで達していない場合は、バッファを２倍に拡張し
        //  続きを読み進む
        //
        else
        {
            buffer_size *= 2;
            buffer = Realloc(buffer, buffer_size);
        }
    }

    // ファイル終了時の処理
    free(*pbuffer);
    *pbuffer = NULL;
    *pbuffer_size = 0;
    return NULL;
}

/*---------------------------------------------------------------------------*
  Name:         UnpackFileName

  Description:  ファイル名を解析しパーツ毎
                (ディレクトリ部、ファイル主幹部、拡張子)に分割する
  
  Arguments:    path   フルパス名
                dir    ディレクトリ / などを含む
                base   ファイル主幹
                ext    拡張子 '.' を含む
  
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
    base_top = 0;

    for (i = path_len - 1; i >= 0; i--)
    {
        if (path[i] == '.' && ext_top == path_len)
        {
            ext_top = i;
        }
        if (path[i] == '/' || path[i] == '\\' || path[i] == ':')
        {
            base_top = i + 1;
            break;
        }
    }

    // . と .. の処理 (この場合 base が "." or "..", 拡張子はなし)
    if (!strcmp(path + base_top, ".") || !strcmp(path + base_top, ".."))
    {
        ext_top = path_len;
    }

    // 代入する
    if (dir)
        *dir = StrNDup(path, base_top);
    if (base)
        *base = StrNDup(path + base_top, ext_top - base_top);
    if (ext)
        *ext = StrDup(path + ext_top);
    return;
}

/*---------------------------------------------------------------------------*
  Name:         SetDebugMode
  
  Description:  デバッグモードの設定
  
  Arguments:    mode   モード
  
  Returns:      なし
 *---------------------------------------------------------------------------*/
static BOOL sDebugMode = FALSE;

void SetDebugMode(BOOL mode)
{
    sDebugMode = mode;
}

/*---------------------------------------------------------------------------*
  Name:         DebugPrintf
  
  Description:  デバッグプリント
  
  Arguments:    fmt   Printf のフォーマット
                ...   プリントする変数
  
  Returns:      なし
 *---------------------------------------------------------------------------*/
void DebugPrintf(const char *fmt, ...)
{
    va_list va;

    if (sDebugMode)
    {
        va_start(va, fmt);
        vfprintf(stderr, fmt, va);
        va_end(va);
    }
}

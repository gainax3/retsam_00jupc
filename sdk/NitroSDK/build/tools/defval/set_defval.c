/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - defval
  File:     set_defval.c

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: set_defval.c,v $
  Revision 1.2  2006/01/18 02:12:28  kitase_hirotake
  do-indent

  Revision 1.1  2005/06/20 07:07:15  yasu
  add defval

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include "defval.h"

static BOOL add_dvalue(const char *name, const char *value);
static char *clip_whitespace(char *str);


//---------------------------------------------------------------------------
//  定義値保持リスト
//---------------------------------------------------------------------------
tDefineValue *gDefineValueTop = NULL;

/*---------------------------------------------------------------------------*
  Name:         add_dvalue
  
  Description:  定義値リストに新しい値を追加する
  
  Arguments:    name  変数の名前
                value 変数の値
  
                name  が NULL あるいは "" なら何もせずに終了(返値 FALSE)
                value が NULL なら "" が定義されているとみなす。
  
  Returns:      TRUE   新規に追加した
                FALSE  既に同名の値が登録済み(後のものに上書きされる)
 *---------------------------------------------------------------------------*/
static BOOL add_dvalue(const char *name, const char *value)
{
    tDefineValue *t;

    //
    // NULL のときの対処
    //   name  が NULL あるいは "" なら何もせずに終了(返値 TRUE)
    //   value が NULL なら "" が定義されているとみなす。
    //
    if (name == NULL || name[0] == '\0')
    {
        return TRUE;
    }
    if (value == NULL)
    {
        value = (const char *)"";
    }

    DebugPrintf("DEFINE:$(%s)=\"%s\"\n", name, value);

    // 同名の定義名があれば新しい値と差し替える
    if (NULL != (t = get_dvalue_listptr(name)))
    {
        if (t->value)
            free(t->value);
        t->value = StrDup(value);
        return FALSE;
    }

    // 新しい定義名ならリストを延長し登録する
    t = Calloc(sizeof(tDefineValue));
    t->name = StrDup(name);
    t->value = StrDup(value);
    t->next = gDefineValueTop;
    gDefineValueTop = t;

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         add_dvalue_by_equality
  
  Description:  定義値リストに等式で新しい値を追加する
  
  Arguments:    equality 等式 "DEFINE=VALUE"
  
                equality が NULL または "" のときは何もしません。
  
  Returns:      TRUE   新規に追加した
                FALSE  既に同名の値が登録済み(後のものに上書きされる)
 *---------------------------------------------------------------------------*/
BOOL add_dvalue_by_equality(const char *equality)
{
    char   *name;
    char   *value;
    char   *scratch;
    BOOL    result;

    //
    //  等式を変更化可能な領域にコピーし '=' の前後で name と value に分ける
    //
    scratch = StrDup(equality);
    name = scratch;
    value = scratch;
    while ('\0' != *value)
    {
        if ('=' == *value)
        {
            *value = '\0';             // '=' の位置に '\0' を入れることで分割
            value++;
            break;
        }
        value++;
    }
    name = clip_whitespace(name);      // 前後の空白を削除する
    value = clip_whitespace(value);

    //
    //  値を登録する
    //
    result = add_dvalue(name, value);
    free(scratch);

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         add_dvalue_from_file
  
  Description:  定義値リストにファイル内の等式を追加する
  
  Arguments:    filename ファイル名
                "DEFINE=VALUE" の羅列
  
  Returns:      TRUE   成功した
                FALSE  既に同名の値が登録済み(後のものに上書きされる)
 *---------------------------------------------------------------------------*/
BOOL add_dvalue_from_file(const char *filename)
{
    FILE   *fp;
    char   *buffer;
    int     buffer_size;
    int     line_num;

    if (filename[0] == '-' && filename[1] == '\0')
    {
        fp = stdin;
    }
    else if (NULL == (fp = fopen(filename, "r")))
    {
        fprintf(stderr, "Cannot open file \"%s\".\n", filename);
        return FALSE;
    }

    //
    // ファイルを一行ずつ読み込み、変数登録する
    //
    buffer = NULL;
    buffer_size = line_num = 0;
    while (NULL != Fgets(&buffer, &buffer_size, fp))
    {
        line_num++;
        if (!add_dvalue_by_equality(buffer))
        {
            fprintf(stderr, "line %d: found same entry: %s\n", line_num, buffer);
        }
    }

    if (fp != stdin)
        fclose(fp);

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         chip_whitespace
  
  Description:  文字列前後の空白を削除する
  
  Arguments:    str    処理対象文字列の先頭
  
                この str 内部は上書きされるので const 領域では動作しない
  
  Returns:      新しい先頭位置
 *---------------------------------------------------------------------------*/
static char *clip_whitespace(char *str)
{
    int     n;

    // 後の空白に '\0' を埋める
    for (n = strlen(str) - 1; n >= 0; n--)
    {
        if (!isspace(str[n]))
        {
            str[n + 1] = '\0';
            break;
        }
    }

    // 前の空白をスキップし、先頭の文字のポインタを返す
    for (n = 0; str[n] != '\0'; n++)
    {
        if (!isspace(str[n]))
        {
            break;
        }
    }
    return str + n;
}

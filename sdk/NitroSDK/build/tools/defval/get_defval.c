/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - defval
  File:     get_defval.c

  Copyright 2005-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: get_defval.c,v $
  Revision 1.5  2007/02/20 00:28:11  kitase_hirotake
  indent source

  Revision 1.4  2007/02/07 01:33:34  yasu
  著作年度修正

  Revision 1.3  2007/02/01 11:24:22  yasu
  binmode/textmode 対応

  Revision 1.2  2006/01/18 02:11:20  kitase_hirotake
  do-indent

  Revision 1.1  2005/06/20 07:07:15  yasu
  add defval

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include "defval.h"

/*---------------------------------------------------------------------------*
  Name:         get_dvalue_listptr
  
  Description:  定義名に対応するリストへのポインタを取得する
  
  Arguments:    name        変数の名前
  
  Returns:      定義リストへのポインタ
 *---------------------------------------------------------------------------*/
tDefineValue *get_dvalue_listptr(const char *name)
{
    tDefineValue *t;

    for (t = gDefineValueTop; t; t = t->next)
    {
        if (!strcmp(t->name, name))
        {
            return t;
        }
    }
    return NULL;
}

/*---------------------------------------------------------------------------*
  Name:         get_dvalue
  
  Description:  定義名の値を取得する。
                まず定義値リスト内を検索し、なければ環境変数を検索する
                それでもなければ NULL を返す。
  
  Arguments:    name        変数の名前
  
  Returns:      値文字列へのポインタ(free 不要)
                NULL: 対応する変数がない
 *---------------------------------------------------------------------------*/
static char *get_dvalue(const char *name)
{
    tDefineValue *t = get_dvalue_listptr(name);

    return t ? t->value : getenv(name);
}

/*---------------------------------------------------------------------------*
  Name:         modify_dvalue
  
  Description:  変数修飾子 :r,:t,:e,:h による変換処理を受けた後の定義値を
                複製して返す。
  
  Arguments:    value     修飾される値
                modifier  修飾子 'r', 't', 'e', 'h' のどれか
  
  Returns:      変換値文字列へのポインタ(free 必要)
 *---------------------------------------------------------------------------*/
static char *modify_dvalue(const char *value, char modifier)
{
    char   *value_dir;
    char   *value_base;
    char   *value_ext;
    char   *modified_value;

    if (value == NULL)
        return NULL;
    if (modifier == '\0')
        return StrDup(value);

    //
    // 修飾子の処理
    //   入力値をファイル名の構成要素に分割し修飾子に応じて再構成する
    //
    UnpackFileName(value, &value_dir, &value_base, &value_ext);

    switch (modifier)
    {
    case 'h':                         // 最後の /, \ つきのディレクトリ名
        modified_value = StrDup(value_dir);
        break;

    case 't':                         // ディレクトリ無しのファイル名
        modified_value = StrCatDup(value_base, value_ext);
        break;

    case 'r':                         // フルパスから拡張子を無くした名
        modified_value = StrCatDup(value_dir, value_base);
        break;

    case 'e':                         // File extension
        modified_value = StrDup(value_ext);
        break;

    default:                          // Unknown
        fprintf(stderr, "Unknown modifier ':%c'... Ignored.\n", modifier);
        modified_value = StrDup(value);
        break;
    }

    free(value_dir);
    free(value_base);
    free(value_ext);

    return modified_value;
}

/*---------------------------------------------------------------------------*
  Name:         get_modified_dvalue
  
  Description:  定義値リストから持ってきた値に、
                修飾子 :r,:t,:e,:h による変換処理をかけ、複製して返す。
  
  Arguments:    name      定義名
  
  Returns:      変換値文字列へのポインタ(free 必要)
 *---------------------------------------------------------------------------*/
static char *get_modified_dvalue(const char *name)
{
    char   *name_copy = StrDup(name);
    int     name_len = strlen(name_copy);
    char    modifier = '\0';
    char   *value;

    // 修飾子の取り出し
    if (name_len > 2 && name_copy[name_len - 2] == ':')
    {
        name_copy[name_len - 2] = '\0';
        modifier = name_copy[name_len - 1];
    }

    // 値取得
    value = modify_dvalue(get_dvalue(name_copy), modifier);
    free(name_copy);

    DebugPrintf("REFERED(%s)=[%s]\n", name, value ? value : "(NULL)");
    return value;
}

/*---------------------------------------------------------------------------*
  Name:         put_modified_dvalue
  
  Description:  文字列をファイルへ出力する
                その際に $(XXXXX) を展開する
  
  Arguments:    fp   出力ファイル名
                str  文字列
  
  Returns:      TRUE 成功
 *---------------------------------------------------------------------------*/
BOOL puts_modified_dvalue(FILE * fp, const char *str)
{
    const char *str_name;
    int     str_name_len;
    char   *name;
    char   *value;
    int     result;

    while ('\0' != *str)
    {
        //
        // $(XXX) を探し、あれば対応する値を出力する
        //
        if ('$' == *str)
        {
            if ('$' == *(str + 1))
            {
                str++;                 // $$ なら $ に変換する
            }
            else if ('(' == *(str + 1))
            {
                str_name = str + 2;
                str_name_len = 0;
                while ('\0' != str_name[str_name_len])
                {
                    if (')' == str_name[str_name_len])
                    {
                        name = StrNDup(str_name, str_name_len);
                        value = get_modified_dvalue(name);
                        free(name);

                        if (NULL != value)
                        {
                            result = fputs(value, fp);
                            free(value);
                            if (result == EOF)
                            {
                                return FALSE;
                            }
                        }
                        str = str_name + str_name_len;
                        goto skipout;
                    }
                    str_name_len++;
                }
            }
        }

        //
        // $(XXX) でないなら一文字ずつ出力する
        //
        if (EOF == fputc(*str, fp))
        {
            return FALSE;
        }
      skipout:
        str++;
    }
    return TRUE;
}

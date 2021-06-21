/*---------------------------------------------------------------------------*
  Project:  NitroSDK - bin2obj
  File:     bin2obj.c

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: bin2obj.c,v $
  Revision 1.7  2006/07/21 07:57:19  yasu
  PowerPC 対応

  Revision 1.6  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.5  2005/06/14 00:57:07  yasu
  -s オプションの追加

  Revision 1.4  2005/06/13 08:52:26  yasu
  %f をシンボル名に対応

  Revision 1.3  2005/06/13 02:56:34  yasu
  オプションの修正、テスト作成による動作の確認

  Revision 1.2  2005/06/10 12:55:42  yasu
  1st リファクタリング

  Revision 1.1  2005/06/10 00:43:01  yasu
  初期版

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include "bin2obj.h"

/*---------------------------------------------------------------------------*
  Name:         main

  Description:  bin2obj メイン
 *---------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    Bin2ObjArgs args;
    BOOL    result;

    cook_args(&args, argc, argv);
    result = bin2obj(&args);
    free_args(&args);

    return result ? 0 : 1;
}

/*---------------------------------------------------------------------------*
  Name:         bin2obj

  Description:  オブジェクト情報を出力する

  Arguments:    t     処理後の bin2obj 引数
  
  Returns:      TRUE 成功  FALSE 失敗
 *---------------------------------------------------------------------------*/
BOOL bin2obj(const Bin2ObjArgs * t)
{
    Object  object;

    //
    // オブジェクトデータ作成
    //
    object_init(&object, t->machine, t->endian);

    if (!add_datasec(&object,
                     t->section_rodata, t->section_rwdata,
                     t->symbol_begin, t->symbol_end, t->binary_filename, t->writable, t->align))
    {
        return FALSE;
    }

    map_section(&object);

    //
    // オブジェクト出力
    //
    if (!output_object(&object, t->object_filename))
    {
        return FALSE;
    }

    return TRUE;
}

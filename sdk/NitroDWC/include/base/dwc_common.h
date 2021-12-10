/*---------------------------------------------------------------------------*
  Project:  NitroDWC - base - dwc_common
  File:     dwc_common.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_common.h,v $
  Revision 1.11  2005/12/26 06:43:20  nakata
  Moved ingamesn check functions from dwc_common.c to dwc_login.c.

  Revision 1.10  2005/12/26 04:23:18  nakata
  Added NAS return code 40 related functions to Auth.
  Added DWC_GetIngamesnCheckResult() to dwc_common.c/dwc_common.h.
  Fixed the bug that incorrect ingamesn is sent when auth is called in netcheck.

  Revision 1.9  2005/12/21 03:08:19  arakit
  ワイド文字列の文字列長を返す関数DWCi_WStrLen()を作成した。

  Revision 1.8  2005/10/28 09:11:20  arakit
  DWCi_GetCurrentTime()を削除した。

  Revision 1.7  2005/09/16 07:27:13  arakit
  汎用key/value文字列の最大長を4096に変更

  Revision 1.6  2005/09/05 11:42:02  arakit
  key/value文字列操作関数の文字数上限を撤廃した。

  Revision 1.5  2005/08/29 12:25:31  arakit
  時間計測関数DWCi_GetCurrentTime()を追加した。

  Revision 1.4  2005/08/29 06:34:15  arakit
  汎用key/value型文字列読み書き関数を用意した。

  Revision 1.3  2005/08/23 13:53:05  arakit
  ランダム関数にMATH_Rand*を使うようにした。

  Revision 1.2  2005/08/20 07:01:19  sasakit
  ヘッダインクルードガードの書式を統一した。
  bm/dwc_init.h -> bm/dwc_bm_init.hに変更
  ヘッダにCopyrightをつけた。
  構造体の名前空間をできるだけルールにちかづけた。
  util_wifiidtool.hとdwc_backup.hの関係を若干修正。


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_COMMON_H_
#define DWC_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif
    

//----------------------------------------------------------------------------
// define
//----------------------------------------------------------------------------
// 汎用key/value文字列のそれぞれの最大文字列長
#define DWC_COMMONSTR_MAX_KEY_VALUE_LEN 4096
    

//----------------------------------------------------------------------------
// enum
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// typedef - function
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// struct
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// function - external
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  汎用key/value型文字列作成関数
  引数　：key       設定したいkey文字列
          value     設定したいvalue文字列
          string    key/value型文字列格納先ポインタ
          separator 各文字列を分ける区切り文字
  戻り値：セットされたkey/value文字列長（NULL終端含まず）
  用途　：key/valueの文字列を指定し、DWC汎用key/value文字列を作成する
 *---------------------------------------------------------------------------*/
extern int  DWC_SetCommonKeyValueString(const char* key, const char* value, char* string, char separator);


/*---------------------------------------------------------------------------*
  汎用key/value型文字列追加関数
  引数　：key       設定したいkey文字列
          value     設定したいvalue文字列
          string    key/value型文字列格納先ポインタ
          separator 各文字列を分ける区切り文字
  戻り値：key/value文字列が追加された後のkey/value文字列長（NULL終端含まず）
  用途　：key/valueの文字列を既存の文字列に追加する
 *---------------------------------------------------------------------------*/
extern int  DWC_AddCommonKeyValueString(const char* key, const char* value, char* string, char separator);


/*---------------------------------------------------------------------------*
  汎用key/value型文字列value取得関数
  引数　：key       取り出したいkey文字列
          value     取り出したvalue文字列の格納先ポインタ。
          string    key/value型文字列
          separator 各文字列を分ける区切り文字
  戻り値：value文字列長（NULL終端含まず）。存在しないkeyを指定した場合は-1を返す
  用途　：指定した区切り文字で区切られたDWC汎用key/value型文字列から、
          指定されたkey文字列に対応するvalue文字列を取得する。
 *---------------------------------------------------------------------------*/
extern int  DWC_GetCommonValueString(const char* key, char* value, const char* string, char separator);


//----------------------------------------------------------------------------
// function - internal
//----------------------------------------------------------------------------
extern u32  DWCi_GetMathRand32(u32 max);


extern u32  DWCi_WStrLen( const u16* str );


#ifdef __cplusplus
}
#endif


#endif // DWC_COMMON_H_

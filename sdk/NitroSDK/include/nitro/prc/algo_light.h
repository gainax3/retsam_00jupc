/*---------------------------------------------------------------------------*
  Project:  NitroSDK - PRC - include
  File:     prc/algo_light.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: algo_light.h,v $
  Revision 1.6  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.5  2005/03/04 12:45:07  seiki_masashi
  PRC_InitPrototypeDB 及び PRC_InitInputPattern が作成の成功・失敗を返すように変更

  Revision 1.4  2005/02/28 05:26:01  yosizaki
  do-indent.

  Revision 1.3  2004/11/04 07:59:53  seiki_masashi
  regularize を normalize に修正

  Revision 1.2  2004/07/19 06:52:06  seiki_masashi
  識別子名の整理

  Revision 1.1  2004/06/30 08:49:45  seiki_masashi
  algo_simple -> algo_light
  algo_standard -> algo_fine
  algo_accurate -> algo_superfine
  新しい algo_standard の追加

  Revision 1.3  2004/06/28 13:18:22  seiki_masashi
  PRC_GetInputPatternStrokes の追加

  Revision 1.2  2004/06/25 03:03:39  seiki_masashi
  各関数から bufferSize の指定を削除
  GetRecognitionBufferSize* の追加
  他の関数呼び出ししかしていない関数の inline 化

  Revision 1.1  2004/06/24 07:03:10  seiki_masashi
  ヘッダファイルを分割・整理しました

  Revision 1.1  2004/06/23 12:00:29  seiki_masashi
  add 'PRC*' to SDK

  Revision 1.3  2004/06/22 11:19:00  seiki_masashi
  動作可能バージョン(lightのみ)

  Revision 1.2  2004/06/17 08:14:52  seiki_masashi
  PRC_GetPrototypeDBBufferSizeEx

  Revision 1.1  2004/06/17 02:30:29  seiki_masashi
  認識アルゴリズムの差し替えを容易にするために prc_algo_* に分割しました

  Revision 1.2  2004/06/17 00:52:36  seiki_masashi
  PRC_InitPrototypeDBEX, PRC_InitInputPatternEX

  Revision 1.1  2004/06/16 23:56:09  seiki_masashi
  rename ptrcg to prc.

  Revision 1.3  2004/06/15 08:10:55  seiki_masashi
  ダミーのライブラリでサンプルプログラムが動作可能に

  Revision 1.2  2004/06/15 05:55:09  seiki_masashi
  インターフェイスの改善

  Revision 1.1  2004/06/15 00:54:42  seiki_masashi
  インターフェイスの設計


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NITRO_PRC_ALGO_LIGHT_H_
#define NITRO_PRC_ALGO_LIGHT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/misc.h>
#include <nitro/types.h>
#include <nitro/prc/types.h>
#include <nitro/prc/common.h>
#include <nitro/prc/resample.h>
#include <nitro/prc/algo_common.h>

/*===========================================================================*
  Constant Definitions
 *===========================================================================*/

/*===========================================================================*
  Type Definitions
 *===========================================================================*/

/*---------------------------------------------------------------------------*
  見本DBの展開に関するパラメータ
    normalizeSize       正規化サイズ.
    resampleThreshold   正規化後に行われるリサンプルの閾値.
 *---------------------------------------------------------------------------*/
typedef PRCPrototypeDBParam_Common PRCPrototypeDBParam_Light;

/*---------------------------------------------------------------------------*
  入力パターンの解釈に関するパラメータ
    normalizeSize       正規化サイズ.
    resampleThreshold   正規化後に行われるリサンプルの閾値.
 *---------------------------------------------------------------------------*/
typedef PRCInputPatternParam_Common PRCInputPatternParam_Light;

/*---------------------------------------------------------------------------*
  入力由来の認識パターン
 *---------------------------------------------------------------------------*/
typedef PRCInputPattern_Common PRCInputPattern_Light;

/*---------------------------------------------------------------------------*
  メモリに展開された見本DB
 *---------------------------------------------------------------------------*/
typedef PRCPrototypeDB_Common PRCPrototypeDB_Light;

/*---------------------------------------------------------------------------*
  認識処理に関するパラメータ
 *---------------------------------------------------------------------------*/
typedef struct PRCRecognizeParam_Light
{
    int     dummy;
}
PRCRecognizeParam_Light;



/*---------------------------------------------------------------------------*
  Name:         PRC_Init_Light

  Description:  図形認識 API を初期化します.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void PRC_Init_Light(void)
{
    PRC_Init_Common();
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetPrototypeDBBufferSize_Light

  Description:  見本DBを展開するのに必要なワーク領域の大きさを計算します.

  Arguments:    prototypeList           見本パターンの一覧

  Returns:      見本DBを展開するのに必要なメモリ量.
 *---------------------------------------------------------------------------*/
static inline u32 PRC_GetPrototypeDBBufferSize_Light(const PRCPrototypeList *prototypeList)
{
    return PRC_GetPrototypeDBBufferSize_Common(prototypeList);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetPrototypeDBBufferSizeEx_Light

  Description:  見本DBを展開するのに必要なワーク領域の大きさを計算します.

  Arguments:    prototypeList           見本パターンの一覧
                kindMask                種類指定用マスク
                ignoreDisabledEntries   enabled フラグが FALSE の見本DBエントリは
                                        そもそも展開しないかどうか
                param                   見本DBの展開に関するパラメータ

  Returns:      見本DBを展開するのに必要なメモリ量.
 *---------------------------------------------------------------------------*/
static inline u32
PRC_GetPrototypeDBBufferSizeEx_Light(const PRCPrototypeList *prototypeList,
                                     u32 kindMask,
                                     BOOL ignoreDisabledEntries,
                                     const PRCPrototypeDBParam_Light *param)
{
    return PRC_GetPrototypeDBBufferSizeEx_Common(prototypeList, kindMask, ignoreDisabledEntries,
                                                 param);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_InitPrototypeDB_Light

  Description:  PRCPrototypeDB 構造体を作成します.
                buffer には PRC_GetPrototypeDBBufferSize が返すサイズ以上の
                バッファ領域が設定されている必要があります.

  Arguments:    protoDB         初期化する見本DB構造体.
                buffer          見本DBの展開に使用するメモリ領域へのポインタ.
                                (メモリ領域サイズ>=PRC_GetPrototypeDBBufferSize の返り値)
                prototypeList   見本パターンの一覧

  Returns:      作成に成功したら真.
 *---------------------------------------------------------------------------*/
static inline BOOL
PRC_InitPrototypeDB_Light(PRCPrototypeDB_Light *protoDB,
                          void *buffer, const PRCPrototypeList *prototypeList)
{
    return PRC_InitPrototypeDB_Common(protoDB, buffer, prototypeList);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_InitPrototypeDBEx_Light

  Description:  PRCPrototypeDB 構造体を作成します.
                buffer には PRC_GetPrototypeDBBufferSize が返すサイズ以上の
                バッファ領域が設定されている必要があります.
                param によって、展開時のパラメータが指定できます.

  Arguments:    protoDB         初期化する見本DB構造体.
                buffer          見本DBの展開に使用するメモリ領域へのポインタ.
                                (メモリ領域サイズ>=PRC_GetPrototypeDBBufferSize の返り値)
                prototypeList   見本パターンの一覧
                kindMask        種類指定用マスク
                ignoreDisabledEntries   enabled フラグが FALSE の見本DBエントリは
                                        そもそも展開しないかどうか
                param           見本DBの展開に関するパラメータ

  Returns:      作成に成功したら真.
 *---------------------------------------------------------------------------*/
static inline BOOL
PRC_InitPrototypeDBEx_Light(PRCPrototypeDB_Light *protoDB,
                            void *buffer,
                            const PRCPrototypeList *prototypeList,
                            u32 kindMask,
                            BOOL ignoreDisabledEntries, const PRCPrototypeDBParam_Light *param)
{
    return PRC_InitPrototypeDBEx_Common(protoDB, buffer, prototypeList, kindMask,
                                        ignoreDisabledEntries, param);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetInputPatternBufferSize_Light

  Description:  パターンデータを比較用に展開するのに必要なワーク領域の大きさを
                計算します.

  Arguments:    maxPointCount   入力点数の上限(pen up marker を含む)
                maxStrokeCount  画数の上限

  Returns:      パターンを展開するのに必要なメモリ量.
 *---------------------------------------------------------------------------*/
static inline u32 PRC_GetInputPatternBufferSize_Light(int maxPointCount, int maxStrokeCount)
{
    return PRC_GetInputPatternBufferSize_Common(maxPointCount, maxStrokeCount);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_InitInputPattern_Light

  Description:  PRCInputPattern 構造体を作成します.

  Arguments:    pattern             初期化するパターン構造体.
                buffer              パターンの展開に使用するメモリ領域へのポインタ.
                                    (領域サイズ>=PRC_GetInputPatternBufferSize の返り値)
                strokes             整形前の生入力座標値.
                maxPointCount       入力点数の上限(pen up marker を含む)
                maxStrokeCount      画数の上限

  Returns:      作成に成功したら真.
 *---------------------------------------------------------------------------*/
static inline BOOL
PRC_InitInputPattern_Light(PRCInputPattern_Light *pattern,
                           void *buffer,
                           const PRCStrokes *strokes, int maxPointCount, int maxStrokeCount)
{
    return PRC_InitInputPattern_Common(pattern, buffer, strokes, maxPointCount, maxStrokeCount);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_InitInputPatternEx_Light

  Description:  PRCInputPattern 構造体を作成します.
                param によって、入力パターンの解釈に関するパラメータを指定できます.

  Arguments:    pattern             初期化するパターン構造体.
                buffer              パターンの展開に使用するメモリ領域へのポインタ.
                                    (領域サイズ>=PRC_GetInputPatternBufferSize の返り値)
                strokes             整形前の生入力座標値.
                param               入力パターンの解釈に関するパラメータ

  Returns:      作成に成功したら真.
 *---------------------------------------------------------------------------*/
static inline BOOL
PRC_InitInputPatternEx_Light(PRCInputPattern_Light *pattern,
                             void *buffer,
                             const PRCStrokes *strokes,
                             int maxPointCount,
                             int maxStrokeCount, const PRCInputPatternParam_Light *param)
{
    return PRC_InitInputPatternEx_Common(pattern, buffer, strokes, maxPointCount, maxStrokeCount,
                                         param);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetRecognitionBufferSizeEx_Light

  Description:  認識アルゴリズムが必要とするワーク領域の大きさを
                計算します.

  Arguments:    maxPointCount   入力点数の上限(pen up marker を含む)
                maxStrokeCount  画数の上限
                protoDB         見本DB.
                param           認識処理に関するパラメータ.

  Returns:      認識アルゴリズムが必要とするメモリ量.
 *---------------------------------------------------------------------------*/
u32
 
 
PRC_GetRecognitionBufferSizeEx_Light(int maxPointCount,
                                     int maxStrokeCount,
                                     const PRCPrototypeDB_Light *protoDB,
                                     const PRCRecognizeParam_Light *param);

/*---------------------------------------------------------------------------*
  Name:         PRC_GetRecognitionBufferSize_Light

  Description:  認識アルゴリズムが必要とするワーク領域の大きさを
                計算します.

  Arguments:    maxPointCount   入力点数の上限(pen up marker を含む)
                maxStrokeCount  画数の上限
                protoDB         見本DB.

  Returns:      認識アルゴリズムが必要とするメモリ量.
 *---------------------------------------------------------------------------*/
static inline u32
PRC_GetRecognitionBufferSize_Light(int maxPointCount,
                                   int maxStrokeCount, const PRCPrototypeDB_Light *protoDB)
{
    return PRC_GetRecognitionBufferSizeEx_Light(maxPointCount, maxStrokeCount, protoDB, NULL);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetRecognizedEntriesEx_Light

  Description:  見本DBの特定の kind のエントリと入力パターンを比較して認識し,
                結果の上位 numRanking 位を返します.

  Arguments:    resultEntries   認識結果へのポインタを入れる配列へのポインタ.
                                求める個数以下しか認識できなかった場合、
                                残りは NULL で埋められます.
                resultScores    認識結果のスコアの配列へのポインタ.
                numRanking      result* に返す個数
                buffer          認識アルゴリズムが使用するメモリ領域へのポインタ.
                                (領域サイズ>=PRC_GetRecognitionBufferSize の返り値)
                input           入力パターン.
                protoDB         見本DB.
                kindMask        各見本DBエントリの kind 値との論理積をとり,
                                非ゼロであれば有効とみなします.
                param           認識処理に関するパラメータ.

  Returns:      比較された見本DB上のパターン数.
 *---------------------------------------------------------------------------*/
int
 
 
PRC_GetRecognizedEntriesEx_Light(PRCPrototypeEntry **resultEntries,
                                 fx32 *resultScores,
                                 int numRanking,
                                 void *buffer,
                                 const PRCInputPattern_Light *input,
                                 const PRCPrototypeDB_Light *protoDB,
                                 u32 kindMask, const PRCRecognizeParam_Light *param);

/*---------------------------------------------------------------------------*
  Name:         PRC_GetRecognizedEntries_Light

  Description:  見本DBと入力パターンを比較して認識し,
                結果の上位 numRanking 位を返します.

  Arguments:    resultEntries   認識結果へのポインタを入れる配列へのポインタ.
                                求める個数以下しか認識できなかった場合、
                                残りは NULL で埋められます.
                resultScores    認識結果のスコアの配列へのポインタ.
                numRanking      result* に返してほしい個数
                buffer          認識アルゴリズムが使用するメモリ領域へのポインタ.
                                (領域サイズ>=PRC_GetRecognitionBufferSize の返り値)
                input           入力パターン.
                protoDB         見本DB.

  Returns:      比較された見本DB上のパターン数.
 *---------------------------------------------------------------------------*/
static inline int
PRC_GetRecognizedEntries_Light(PRCPrototypeEntry **resultEntries,
                               fx32 *resultScores,
                               int numRanking,
                               void *buffer,
                               const PRCInputPattern_Light *input,
                               const PRCPrototypeDB_Light *protoDB)
{
    return PRC_GetRecognizedEntriesEx_Light(resultEntries, resultScores, numRanking, buffer, input,
                                            protoDB, PRC_KIND_ALL, NULL);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetRecognizedEntryEx_Light

  Description:  見本DBの特定の kind のエントリと入力パターンを比較して認識します.

  Arguments:    resultEntry 認識された見本DBエントリへのポインタを受け取るポインタ.
                            認識できなかった場合、NULL が入ります.
                buffer      認識アルゴリズムが使用するメモリ領域へのポインタ.
                            (領域サイズ>=PRC_GetRecognitionBufferSize の返り値)
                input       入力パターン.
                protoDB     見本DB.
                kindMask    各見本DBエントリの kind 値との論理積をとり,
                            非ゼロであれば有効とみなします.
                param       認識処理に関するパラメータ.

  Returns:      スコア値.
 *---------------------------------------------------------------------------*/
static inline fx32
PRC_GetRecognizedEntryEx_Light(PRCPrototypeEntry **resultEntry,
                               void *buffer,
                               const PRCInputPattern_Light *input,
                               const PRCPrototypeDB_Light *protoDB,
                               u32 kindMask, const PRCRecognizeParam_Light *param)
{
    fx32    score;
    (void)PRC_GetRecognizedEntriesEx_Light(resultEntry, &score, 1, buffer, input, protoDB, kindMask,
                                           param);
    return score;
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetRecognizedEntry_Light

  Description:  見本DBと入力パターンを比較して認識します.

  Arguments:    resultEntry 認識された見本DBエントリへのポインタを受け取るポインタ.
                            認識できなかった場合、NULL が入ります.
                buffer      認識アルゴリズムが使用するメモリ領域へのポインタ.
                            (領域サイズ>=PRC_GetRecognitionBufferSize の返り値)
                input       入力パターン.
                protoDB     見本DB.

  Returns:      スコア値.
 *---------------------------------------------------------------------------*/
static inline fx32
PRC_GetRecognizedEntry_Light(PRCPrototypeEntry **resultEntry,
                             void *buffer,
                             const PRCInputPattern_Light *input,
                             const PRCPrototypeDB_Light *protoDB)
{
    return PRC_GetRecognizedEntryEx_Light(resultEntry, buffer, input, protoDB, PRC_KIND_ALL, NULL);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetInputPatternStrokes_Light

  Description:  PRCInputPattern 構造体から点列データを得ます.

  Arguments:    strokes         得られた点列データ.
                                書き換えてはいけません.
                input           入力パターン.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void
PRC_GetInputPatternStrokes_Light(PRCStrokes *strokes, const PRCInputPattern_Light *input)
{
    PRC_GetInputPatternStrokes_Common(strokes, input);
}


/*===========================================================================*
  Inline Functions
 *===========================================================================*/




/*===========================================================================*
  図形認識 API の標準関数として設定
 *===========================================================================*/
#ifndef PRC_DEFAULT_ALGORITHM
#define PRC_DEFAULT_ALGORITHM "Light"

/*---------------------------------------------------------------------------*
  見本DBの展開に関するパラメータ
 *---------------------------------------------------------------------------*/
typedef PRCPrototypeDBParam_Light PRCPrototypeDBParam;

/*---------------------------------------------------------------------------*
  入力パターンの解釈に関するパラメータ
 *---------------------------------------------------------------------------*/
typedef PRCInputPatternParam_Light PRCInputPatternParam;

/*---------------------------------------------------------------------------*
  入力由来の認識パターン
 *---------------------------------------------------------------------------*/
typedef PRCInputPattern_Light PRCInputPattern;

/*---------------------------------------------------------------------------*
  メモリに展開された見本DB
 *---------------------------------------------------------------------------*/
typedef PRCPrototypeDB_Light PRCPrototypeDB;

/*---------------------------------------------------------------------------*
  認識処理に関するパラメータ
 *---------------------------------------------------------------------------*/
typedef PRCRecognizeParam_Light PRCRecognizeParam;


/*---------------------------------------------------------------------------*
  関数
 *---------------------------------------------------------------------------*/
static inline void PRC_Init(void)
{
    PRC_Init_Light();
}

static inline u32 PRC_GetPrototypeDBBufferSize(const PRCPrototypeList *prototypeList)
{
    return PRC_GetPrototypeDBBufferSize_Light(prototypeList);
}

static inline u32
PRC_GetPrototypeDBBufferSizeEx(const PRCPrototypeList *prototypeList,
                               u32 kindMask,
                               BOOL ignoreDisabledEntries, const PRCPrototypeDBParam *param)
{
    return PRC_GetPrototypeDBBufferSizeEx_Light(prototypeList, kindMask, ignoreDisabledEntries,
                                                param);
}

static inline BOOL
PRC_InitPrototypeDB(PRCPrototypeDB *protoDB, void *buffer, const PRCPrototypeList *prototypeList)
{
    return PRC_InitPrototypeDB_Light(protoDB, buffer, prototypeList);
}

static inline BOOL
PRC_InitPrototypeDBEx(PRCPrototypeDB *protoDB,
                      void *buffer,
                      const PRCPrototypeList *prototypeList,
                      u32 kindMask, BOOL ignoreDisabledEntries, const PRCPrototypeDBParam *param)
{
    return PRC_InitPrototypeDBEx_Light(protoDB, buffer, prototypeList, kindMask,
                                       ignoreDisabledEntries, param);
}

static inline u32 PRC_GetInputPatternBufferSize(int maxPointCount, int maxStrokeCount)
{
    return PRC_GetInputPatternBufferSize_Light(maxPointCount, maxStrokeCount);
}

static inline BOOL
PRC_InitInputPattern(PRCInputPattern *pattern,
                     void *buffer, const PRCStrokes *strokes, int maxPointCount, int maxStrokeCount)
{
    return PRC_InitInputPattern_Light(pattern, buffer, strokes, maxPointCount, maxStrokeCount);
}

static inline BOOL
PRC_InitInputPatternEx(PRCInputPattern *pattern,
                       void *buffer,
                       const PRCStrokes *strokes,
                       int maxPointCount, int maxStrokeCount, const PRCInputPatternParam *param)
{
    return PRC_InitInputPatternEx_Light(pattern, buffer, strokes, maxPointCount, maxStrokeCount,
                                        param);
}

static inline u32
PRC_GetRecognitionBufferSize(int maxPointCount,
                             int maxStrokeCount, const PRCPrototypeDB_Light *protoDB)
{
    return PRC_GetRecognitionBufferSize_Light(maxPointCount, maxStrokeCount, protoDB);
}

static inline u32
PRC_GetRecognitionBufferSizeEx(int maxPointCount,
                               int maxStrokeCount,
                               const PRCPrototypeDB_Light *protoDB,
                               const PRCRecognizeParam_Light *param)
{
    return PRC_GetRecognitionBufferSizeEx_Light(maxPointCount, maxStrokeCount, protoDB, param);
}

static inline fx32
PRC_GetRecognizedEntry(PRCPrototypeEntry **resultEntry,
                       void *buffer, const PRCInputPattern *input, const PRCPrototypeDB *protoDB)
{
    return PRC_GetRecognizedEntry_Light(resultEntry, buffer, input, protoDB);
}

static inline fx32
PRC_GetRecognizedEntryEx(PRCPrototypeEntry **resultEntry,
                         void *buffer,
                         const PRCInputPattern *input,
                         const PRCPrototypeDB *protoDB,
                         u32 kindMask, const PRCRecognizeParam *param)
{
    return PRC_GetRecognizedEntryEx_Light(resultEntry, buffer, input, protoDB, kindMask, param);
}

static inline int
PRC_GetRecognizedEntries(PRCPrototypeEntry **resultEntries,
                         fx32 *resultScores,
                         int numRanking,
                         void *buffer, const PRCInputPattern *input, const PRCPrototypeDB *protoDB)
{
    return PRC_GetRecognizedEntries_Light(resultEntries, resultScores, numRanking, buffer, input,
                                          protoDB);
}

static inline int
PRC_GetRecognizedEntriesEx(PRCPrototypeEntry **resultEntries,
                           fx32 *resultScores,
                           int numRanking,
                           void *buffer,
                           const PRCInputPattern *input,
                           const PRCPrototypeDB *protoDB,
                           u32 kindMask, const PRCRecognizeParam *param)
{
    return PRC_GetRecognizedEntriesEx_Light(resultEntries, resultScores, numRanking, buffer, input,
                                            protoDB, kindMask, param);
}

static inline void PRC_GetInputPatternStrokes(PRCStrokes *strokes, const PRCInputPattern *input)
{
    PRC_GetInputPatternStrokes_Light(strokes, input);
}

#endif // PRC_DEFAULT_ALGORITHM




#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_PRC_ALGO_LIGHT_H_ */
#endif

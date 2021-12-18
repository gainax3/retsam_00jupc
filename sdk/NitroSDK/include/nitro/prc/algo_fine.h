/*---------------------------------------------------------------------------*
  Project:  NitroSDK - PRC - include
  File:     prc/algo_fine.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: algo_fine.h,v $
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

  Revision 1.2  2004/06/28 11:10:41  seiki_masashi
  見本パターンの一覧に regularizeSize を追加
  各種パラメータ類を整理

  Revision 1.1  2004/06/28 00:12:10  seiki_masashi
  認識アルゴリズム 'Fine' の追加


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_PRC_ALGO_FINE_H_
#define NITRO_PRC_ALGO_FINE_H_

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

#define PRC_LENGTH_FILTER_DEFAULT_THRESHOLD_FINE normalizeSize
#define PRC_LENGTH_FILTER_DEFAULT_RATIO_FINE     3

/*===========================================================================*
  Type Definitions
 *===========================================================================*/

/*---------------------------------------------------------------------------*
  見本DBの展開に関するパラメータ
    normalizeSize       正規化サイズ.
    resampleThreshold   正規化後に行われるリサンプルの閾値.
 *---------------------------------------------------------------------------*/
typedef PRCPrototypeDBParam_Common PRCPrototypeDBParam_Fine;

/*---------------------------------------------------------------------------*
  入力パターンの解釈に関するパラメータ
    normalizeSize       正規化サイズ.
    resampleThreshold   正規化後に行われるリサンプルの閾値.
 *---------------------------------------------------------------------------*/
typedef PRCInputPatternParam_Common PRCInputPatternParam_Fine;

/*---------------------------------------------------------------------------*
  入力由来の認識パターン
 *---------------------------------------------------------------------------*/
typedef PRCInputPattern_Common PRCInputPattern_Fine;

/*---------------------------------------------------------------------------*
  メモリに展開された見本DB
 *---------------------------------------------------------------------------*/
typedef PRCPrototypeDB_Common PRCPrototypeDB_Fine;

/*---------------------------------------------------------------------------*
  認識処理に関するパラメータ
 *---------------------------------------------------------------------------*/
typedef struct PRCRecognizeParam_Fine
{
    int     lengthFilterThreshold;
    int     lengthFilterRatio;
}
PRCRecognizeParam_Fine;



/*---------------------------------------------------------------------------*
  Name:         PRC_Init_Fine

  Description:  図形認識 API を初期化します.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void PRC_Init_Fine(void)
{
    PRC_Init_Common();
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetPrototypeDBBufferSize_Fine

  Description:  見本DBを展開するのに必要なワーク領域の大きさを計算します.

  Arguments:    prototypeList           見本パターンの一覧

  Returns:      見本DBを展開するのに必要なメモリ量.
 *---------------------------------------------------------------------------*/
static inline u32 PRC_GetPrototypeDBBufferSize_Fine(const PRCPrototypeList *prototypeList)
{
    return PRC_GetPrototypeDBBufferSize_Common(prototypeList);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetPrototypeDBBufferSizeEx_Fine

  Description:  見本DBを展開するのに必要なワーク領域の大きさを計算します.

  Arguments:    prototypeList           見本パターンの一覧
                kindMask                種類指定用マスク
                ignoreDisabledEntries   enabled フラグが FALSE の見本DBエントリは
                                        そもそも展開しないかどうか
                param                   見本DBの展開に関するパラメータ

  Returns:      見本DBを展開するのに必要なメモリ量.
 *---------------------------------------------------------------------------*/
static inline u32
PRC_GetPrototypeDBBufferSizeEx_Fine(const PRCPrototypeList *prototypeList,
                                    u32 kindMask,
                                    BOOL ignoreDisabledEntries,
                                    const PRCPrototypeDBParam_Fine *param)
{
    return PRC_GetPrototypeDBBufferSizeEx_Common(prototypeList, kindMask, ignoreDisabledEntries,
                                                 param);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_InitPrototypeDB_Fine

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
PRC_InitPrototypeDB_Fine(PRCPrototypeDB_Fine *protoDB,
                         void *buffer, const PRCPrototypeList *prototypeList)
{
    return PRC_InitPrototypeDB_Common(protoDB, buffer, prototypeList);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_InitPrototypeDBEx_Fine

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
PRC_InitPrototypeDBEx_Fine(PRCPrototypeDB_Fine *protoDB,
                           void *buffer,
                           const PRCPrototypeList *prototypeList,
                           u32 kindMask,
                           BOOL ignoreDisabledEntries, const PRCPrototypeDBParam_Fine *param)
{
    return PRC_InitPrototypeDBEx_Common(protoDB, buffer, prototypeList, kindMask,
                                        ignoreDisabledEntries, param);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetInputPatternBufferSize_Fine

  Description:  パターンデータを比較用に展開するのに必要なワーク領域の大きさを
                計算します.

  Arguments:    maxPointCount   入力点数の上限(pen up marker を含む)
                maxStrokeCount  画数の上限

  Returns:      パターンを展開するのに必要なメモリ量.
 *---------------------------------------------------------------------------*/
static inline u32 PRC_GetInputPatternBufferSize_Fine(int maxPointCount, int maxStrokeCount)
{
    return PRC_GetInputPatternBufferSize_Common(maxPointCount, maxStrokeCount);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_InitInputPattern_Fine

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
PRC_InitInputPattern_Fine(PRCInputPattern_Fine *pattern,
                          void *buffer,
                          const PRCStrokes *strokes, int maxPointCount, int maxStrokeCount)
{
    return PRC_InitInputPattern_Common(pattern, buffer, strokes, maxPointCount, maxStrokeCount);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_InitInputPatternEx_Fine

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
PRC_InitInputPatternEx_Fine(PRCInputPattern_Fine *pattern,
                            void *buffer,
                            const PRCStrokes *strokes,
                            int maxPointCount,
                            int maxStrokeCount, const PRCInputPatternParam_Fine *param)
{
    return PRC_InitInputPatternEx_Common(pattern, buffer, strokes, maxPointCount, maxStrokeCount,
                                         param);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetRecognitionBufferSizeEx_Fine

  Description:  認識アルゴリズムが必要とするワーク領域の大きさを
                計算します.

  Arguments:    maxPointCount   入力点数の上限(pen up marker を含む)
                maxStrokeCount  画数の上限
                protoDB         見本DB.
                param           認識処理に関するパラメータ.

  Returns:      認識アルゴリズムが必要とするメモリ量.
 *---------------------------------------------------------------------------*/
u32
 
 
PRC_GetRecognitionBufferSizeEx_Fine(int maxPointCount,
                                    int maxStrokeCount,
                                    const PRCPrototypeDB_Fine *protoDB,
                                    const PRCRecognizeParam_Fine *param);

/*---------------------------------------------------------------------------*
  Name:         PRC_GetRecognitionBufferSize_Fine

  Description:  認識アルゴリズムが必要とするワーク領域の大きさを
                計算します.

  Arguments:    maxPointCount   入力点数の上限(pen up marker を含む)
                maxStrokeCount  画数の上限
                protoDB         見本DB.

  Returns:      認識アルゴリズムが必要とするメモリ量.
 *---------------------------------------------------------------------------*/
static inline u32
PRC_GetRecognitionBufferSize_Fine(int maxPointCount,
                                  int maxStrokeCount, const PRCPrototypeDB_Fine *protoDB)
{
    return PRC_GetRecognitionBufferSizeEx_Fine(maxPointCount, maxStrokeCount, protoDB, NULL);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetRecognizedEntriesEx_Fine

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
 
 
PRC_GetRecognizedEntriesEx_Fine(PRCPrototypeEntry **resultEntries,
                                fx32 *resultScores,
                                int numRanking,
                                void *buffer,
                                const PRCInputPattern_Fine *input,
                                const PRCPrototypeDB_Fine *protoDB,
                                u32 kindMask, const PRCRecognizeParam_Fine *param);

/*---------------------------------------------------------------------------*
  Name:         PRC_GetRecognizedEntries_Fine

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
PRC_GetRecognizedEntries_Fine(PRCPrototypeEntry **resultEntries,
                              fx32 *resultScores,
                              int numRanking,
                              void *buffer,
                              const PRCInputPattern_Fine *input, const PRCPrototypeDB_Fine *protoDB)
{
    return PRC_GetRecognizedEntriesEx_Fine(resultEntries, resultScores, numRanking, buffer, input,
                                           protoDB, PRC_KIND_ALL, NULL);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetRecognizedEntryEx_Fine

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
PRC_GetRecognizedEntryEx_Fine(PRCPrototypeEntry **resultEntry,
                              void *buffer,
                              const PRCInputPattern_Fine *input,
                              const PRCPrototypeDB_Fine *protoDB,
                              u32 kindMask, const PRCRecognizeParam_Fine *param)
{
    fx32    score;
    (void)PRC_GetRecognizedEntriesEx_Fine(resultEntry, &score, 1, buffer, input, protoDB, kindMask,
                                          param);
    return score;
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetRecognizedEntry_Fine

  Description:  見本DBと入力パターンを比較して認識します.

  Arguments:    resultEntry 認識された見本DBエントリへのポインタを受け取るポインタ.
                            認識できなかった場合、NULL が入ります.
                buffer      認識アルゴリズムが使用するメモリ領域へのポインタ.
                            (領域サイズ>=PRC_GetRecognitionBufferSize の返り値)
                input       入力パターン.
                protoDB 見本DB.

  Returns:      スコア値.
 *---------------------------------------------------------------------------*/
static inline fx32
PRC_GetRecognizedEntry_Fine(PRCPrototypeEntry **resultEntry,
                            void *buffer,
                            const PRCInputPattern_Fine *input, const PRCPrototypeDB_Fine *protoDB)
{
    return PRC_GetRecognizedEntryEx_Fine(resultEntry, buffer, input, protoDB, PRC_KIND_ALL, NULL);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetInputPatternStrokes_Fine

  Description:  PRCInputPattern 構造体から点列データを得ます.

  Arguments:    strokes         得られた点列データ.
                                書き換えてはいけません.
                input           入力パターン.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void
PRC_GetInputPatternStrokes_Fine(PRCStrokes *strokes, const PRCInputPattern_Fine *input)
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
#define PRC_DEFAULT_ALGORITHM "Fine"

/*---------------------------------------------------------------------------*
  見本DBの展開に関するパラメータ
 *---------------------------------------------------------------------------*/
typedef PRCPrototypeDBParam_Fine PRCPrototypeDBParam;

/*---------------------------------------------------------------------------*
  入力パターンの解釈に関するパラメータ
 *---------------------------------------------------------------------------*/
typedef PRCInputPatternParam_Fine PRCInputPatternParam;

/*---------------------------------------------------------------------------*
  入力由来の認識パターン
 *---------------------------------------------------------------------------*/
typedef PRCInputPattern_Fine PRCInputPattern;

/*---------------------------------------------------------------------------*
  メモリに展開された見本DB
 *---------------------------------------------------------------------------*/
typedef PRCPrototypeDB_Fine PRCPrototypeDB;

/*---------------------------------------------------------------------------*
  認識処理に関するパラメータ
 *---------------------------------------------------------------------------*/
typedef PRCRecognizeParam_Fine PRCRecognizeParam;


/*---------------------------------------------------------------------------*
  関数
 *---------------------------------------------------------------------------*/
static inline void PRC_Init(void)
{
    PRC_Init_Fine();
}

static inline u32 PRC_GetPrototypeDBBufferSize(const PRCPrototypeList *prototypeList)
{
    return PRC_GetPrototypeDBBufferSize_Fine(prototypeList);
}

static inline u32
PRC_GetPrototypeDBBufferSizeEx(const PRCPrototypeList *prototypeList,
                               u32 kindMask,
                               BOOL ignoreDisabledEntries, const PRCPrototypeDBParam *param)
{
    return PRC_GetPrototypeDBBufferSizeEx_Fine(prototypeList, kindMask, ignoreDisabledEntries,
                                               param);
}

static inline BOOL
PRC_InitPrototypeDB(PRCPrototypeDB *protoDB, void *buffer, const PRCPrototypeList *prototypeList)
{
    return PRC_InitPrototypeDB_Fine(protoDB, buffer, prototypeList);
}

static inline BOOL
PRC_InitPrototypeDBEx(PRCPrototypeDB *protoDB,
                      void *buffer,
                      const PRCPrototypeList *prototypeList,
                      u32 kindMask, BOOL ignoreDisabledEntries, const PRCPrototypeDBParam *param)
{
    return PRC_InitPrototypeDBEx_Fine(protoDB, buffer, prototypeList, kindMask,
                                      ignoreDisabledEntries, param);
}

static inline u32 PRC_GetInputPatternBufferSize(int maxPointCount, int maxStrokeCount)
{
    return PRC_GetInputPatternBufferSize_Fine(maxPointCount, maxStrokeCount);
}

static inline BOOL
PRC_InitInputPattern(PRCInputPattern *pattern,
                     void *buffer, const PRCStrokes *strokes, int maxPointCount, int maxStrokeCount)
{
    return PRC_InitInputPattern_Fine(pattern, buffer, strokes, maxPointCount, maxStrokeCount);
}

static inline BOOL
PRC_InitInputPatternEx(PRCInputPattern *pattern,
                       void *buffer,
                       const PRCStrokes *strokes,
                       int maxPointCount, int maxStrokeCount, const PRCInputPatternParam *param)
{
    return PRC_InitInputPatternEx_Fine(pattern, buffer, strokes, maxPointCount, maxStrokeCount,
                                       param);
}

static inline u32
PRC_GetRecognitionBufferSize(int maxPointCount,
                             int maxStrokeCount, const PRCPrototypeDB_Fine *protoDB)
{
    return PRC_GetRecognitionBufferSize_Fine(maxPointCount, maxStrokeCount, protoDB);
}

static inline u32
PRC_GetRecognitionBufferSizeEx(int maxPointCount,
                               int maxStrokeCount,
                               const PRCPrototypeDB_Fine *protoDB,
                               const PRCRecognizeParam_Fine *param)
{
    return PRC_GetRecognitionBufferSizeEx_Fine(maxPointCount, maxStrokeCount, protoDB, param);
}

static inline fx32
PRC_GetRecognizedEntry(PRCPrototypeEntry **resultEntry,
                       void *buffer, const PRCInputPattern *input, const PRCPrototypeDB *protoDB)
{
    return PRC_GetRecognizedEntry_Fine(resultEntry, buffer, input, protoDB);
}

static inline fx32
PRC_GetRecognizedEntryEx(PRCPrototypeEntry **resultEntry,
                         void *buffer,
                         const PRCInputPattern *input,
                         const PRCPrototypeDB *protoDB,
                         u32 kindMask, const PRCRecognizeParam *param)
{
    return PRC_GetRecognizedEntryEx_Fine(resultEntry, buffer, input, protoDB, kindMask, param);
}

static inline int
PRC_GetRecognizedEntries(PRCPrototypeEntry **resultEntries,
                         fx32 *resultScores,
                         int numRanking,
                         void *buffer, const PRCInputPattern *input, const PRCPrototypeDB *protoDB)
{
    return PRC_GetRecognizedEntries_Fine(resultEntries, resultScores, numRanking, buffer, input,
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
    return PRC_GetRecognizedEntriesEx_Fine(resultEntries, resultScores, numRanking, buffer, input,
                                           protoDB, kindMask, param);
}

static inline void PRC_GetInputPatternStrokes(PRCStrokes *strokes, const PRCInputPattern *input)
{
    PRC_GetInputPatternStrokes_Fine(strokes, input);
}

#endif // PRC_DEFAULT_ALGORITHM




#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_PRC_ALGO_FINE_H_ */
#endif

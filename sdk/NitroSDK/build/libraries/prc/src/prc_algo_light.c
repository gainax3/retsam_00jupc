/*---------------------------------------------------------------------------*
  Project:  NitroSDK - PRC - 
  File:     prc_algo_light.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: prc_algo_light.c,v $
  Revision 1.6  2006/01/18 02:12:27  kitase_hirotake
  do-indent

  Revision 1.5  2005/02/28 05:26:29  yosizaki
  do-indent.

  Revision 1.4  2004/07/19 06:52:06  seiki_masashi
  識別子名の整理

  Revision 1.3  2004/07/05 02:37:54  seiki_masashi
  prevRatio の除去

  Revision 1.2  2004/06/30 12:53:44  seiki_masashi
  有害になりうる ASSERT の削除

  Revision 1.1  2004/06/30 08:49:24  seiki_masashi
  algo_simple -> algo_light
  algo_standard -> algo_fine
  algo_accurate -> algo_superfine
  新しい algo_standard の追加

  Revision 1.5  2004/06/29 07:25:02  seiki_masashi
  バグ修正
  枝切りの実装

  Revision 1.4  2004/06/25 03:02:55  seiki_masashi
  各関数から bufferSize の指定を削除
  GetRecognitionBufferSize* の追加

  Revision 1.3  2004/06/24 08:08:33  seiki_masashi
  fix typo

  Revision 1.2  2004/06/24 07:03:39  seiki_masashi
  ヘッダファイルを分割・整理しました

  Revision 1.1  2004/06/23 11:59:30  seiki_masashi
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


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nitro/prc/algo_light.h>

/*===========================================================================*
  Prototype Declarations
 *===========================================================================*/

/*===========================================================================*
  Variable Definitions
 *===========================================================================*/

/*===========================================================================*
  Functions
 *===========================================================================*/

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
                                     const PRCRecognizeParam_Light *param)
{
    (void)maxPointCount;
    (void)maxStrokeCount;
    (void)protoDB;
    (void)param;

    return 1;                          // OS_Alloc(0) はエラーとなるため
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
                                 u32 kindMask, const PRCRecognizeParam_Light *param)
{
    int     i;
    const PRCiPatternData_Common *inputData;
    int     numCompared;

    (void)buffer;
    (void)param;

    SDK_ASSERT(resultEntries);
    SDK_ASSERT(resultScores);
    SDK_ASSERT(input);
    SDK_ASSERT(protoDB);
    SDK_ASSERT(numRanking > 0);

    for (i = 0; i < numRanking; i++)
    {
        resultEntries[i] = NULL;
        resultScores[i] = FX32_ONE * 32768;
    }

    inputData = &input->data;
    numCompared = 0;

    {
        const PRCiPrototypeEntry_Common *proto;
        int     iPattern;
        fx32    border;

        border = FX32_ONE;             // 足切りライン この値を FX32_ONE/2 などすると最初から 0.5 以下のスコアが判定されない
        border *= 32768;               // 内部 score の係数

        proto = protoDB->patterns;

        for (iPattern = 0; iPattern < protoDB->patternCount; iPattern++, proto++)
        {
            const PRCiPatternData_Common *protoData;
            int     iStroke;
            fx32    wholeScore;
            fx32    patternBorder;

            if (!proto->entry->enabled || !(proto->entry->kind & kindMask))
                continue;

            protoData = &proto->data;

            if (inputData->strokeCount != protoData->strokeCount)
                continue;

            wholeScore = 0;

            // 補正の分を戻した値を足切り設定
            patternBorder = border;
            if (proto->entry->correction != 0)
            {
                // patternBorder *= 1 / (FX32_ONE - correction) を本当はしないといけないところを
                // 適当に直線で近似してより大きい値ではしょる
                if (proto->entry->correction < -FX32_ONE)
                {
                    patternBorder >>= 1;        // 1/2 以下で近似
                }
                else if (proto->entry->correction < 0)
                {
                    // -1 < correction < 0 では correction/2+1 で近似
                    patternBorder = FX_Mul(patternBorder, FX32_ONE + proto->entry->correction / 2);
                }
                else if (proto->entry->correction < FX32_ONE / 2)
                {
                    // 0 < correction < 1/2 では correction*2+1 で近似
                    patternBorder = FX_Mul(patternBorder, FX32_ONE + proto->entry->correction * 2);
                }
                else
                {
                    patternBorder = FX32_ONE * 32768;
                }                      // 足切りをあきらめる
            }

            for (iStroke = 0; iStroke < inputData->strokeCount; iStroke++)
            {
                int     iProto, iInput;
                int     protoStrokeIndex, inputStrokeIndex;
                int     protoSize, inputSize;
                const u16 *protoAngle;
                const u16 *inputAngle;
                const fx16 *protoRatio;
                const fx16 *inputRatio;
                fx16    protoNextRatio, inputNextRatio;
                fx32    score;
                fx32    localBorder;
                fx16    strokeRatio;
                int     loopEnd;

                score = 0;

                protoStrokeIndex = protoData->strokes[iStroke];
                inputStrokeIndex = inputData->strokes[iStroke];
                protoSize = protoData->strokeSizes[iStroke];
                inputSize = inputData->strokeSizes[iStroke];
                protoAngle = &protoData->lineSegmentAngleArray[protoStrokeIndex];
                inputAngle = &inputData->lineSegmentAngleArray[inputStrokeIndex];
                protoRatio = &protoData->lineSegmentRatioToStrokeArray[protoStrokeIndex];
                inputRatio = &inputData->lineSegmentRatioToStrokeArray[inputStrokeIndex];

                strokeRatio = protoData->strokeRatios[iStroke]; // ここを inputData-> にすると、入力の画の比重を重視した重み付けになる

                // 足切りレベルの設定
                if (strokeRatio == FX32_ONE)
                {
                    localBorder = patternBorder;
                }
                else if (strokeRatio >= FX32_ONE / 2)
                {
                    localBorder = patternBorder * 2;    // 多めにとる分には問題なし
                }
                else if (strokeRatio >= FX32_ONE / 3)
                {
                    localBorder = patternBorder * 3;    // 多めにとる分には問題なし
                }
                else
                {
                    localBorder = FX32_ONE * 32768;     // 足切りを考えない
                }

                SDK_ASSERT(protoSize >= 2);
                SDK_ASSERT(inputSize >= 2);

                iProto = iInput = 1;
                protoNextRatio = protoRatio[iProto];
                inputNextRatio = inputRatio[iInput];
                loopEnd = protoSize + inputSize - 3;
                for (i = 0; i < loopEnd; i++)
                {
                    int     diff;
                    SDK_ASSERT(iProto < protoSize);
                    SDK_ASSERT(iInput < inputSize);
                    diff = (s16)(protoAngle[iProto] - inputAngle[iInput]);
                    if (diff < 0)
                    {
                        diff = -diff;
                    }
                    if (protoNextRatio <= inputNextRatio)
                    {
                        score += protoNextRatio * diff;
                        iProto++;
                        inputNextRatio -= protoNextRatio;
                        protoNextRatio = protoRatio[iProto];
                    }
                    else
                    {
                        score += inputNextRatio * diff;
                        iInput++;
                        protoNextRatio -= inputNextRatio;
                        inputNextRatio = inputRatio[iInput];
                    }
                    // 足切りチェック
                    if (score > localBorder)
                    {
                        // 最低位候補にも引っかかる見込みがなくなった
                        wholeScore = FX32_ONE * 32768;
                        goto quit_compare;
                    }
                }

                wholeScore += FX_Mul(score, strokeRatio);
            }

            if (proto->entry->correction != 0)
            {
                wholeScore = FX_Mul(wholeScore, FX32_ONE - proto->entry->correction);
            }

//                wholeScore = FX_Mul(wholeScore, FX32_ONE - proto->entry->correction)
//                            + proto->entry->correction;

            // この段階では小さいスコアのほうがよいことに注意
          quit_compare:
            numCompared++;
            if (resultScores[numRanking - 1] > wholeScore)
            {
                resultScores[numRanking - 1] = wholeScore;
                resultEntries[numRanking - 1] = (PRCPrototypeEntry *)proto->entry;
                for (i = numRanking - 2; i >= 0; i--)
                {
                    if (resultScores[i] > resultScores[i + 1])
                    {
                        fx32    tmpScore;
                        PRCPrototypeEntry *tmpEntry;
                        tmpScore = resultScores[i];
                        resultScores[i] = resultScores[i + 1];
                        resultScores[i + 1] = tmpScore;
                        tmpEntry = resultEntries[i];
                        resultEntries[i] = resultEntries[i + 1];
                        resultEntries[i + 1] = tmpEntry;
                    }
                }
                // 足切りスコアの設定
                border = resultScores[numRanking - 1];
            }
        }
    }
    // スコアの正規化
    {
        int     iRank;
        fx32    score;
        for (iRank = 0; iRank < numRanking; iRank++)
        {
            if (resultEntries[iRank] == NULL)
                break;

            score = resultScores[iRank];
            score = FX32_ONE - (score / 32768);
            if (score < 0)
                score = 0;
            if (score >= FX32_ONE)
                score = FX32_ONE;

            resultScores[iRank] = score;
        }
    }

    return numCompared;
}

/*===========================================================================*
  Static Functions
 *===========================================================================*/


/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Project:  NitroSDK - PRC - 
  File:     prc_algo_common.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: prc_resample.c,v $
  Revision 1.12  2006/01/18 02:12:28  kitase_hirotake
  do-indent

  Revision 1.11  2005/03/04 14:07:53  seiki_masashi
  PRC_ResampleStrokes_* を成功・失敗を返すように変更

  Revision 1.10  2005/02/28 05:26:29  yosizaki
  do-indent.

  Revision 1.9  2004/08/31 00:47:47  seiki_masashi
  fix a comment.

  Revision 1.8  2004/07/19 06:52:06  seiki_masashi
  識別子名の整理

  Revision 1.7  2004/07/09 00:12:59  seiki_masashi
  PRCi_Resample* -> PRC_Resample*

  Revision 1.6  2004/06/29 07:23:30  seiki_masashi
  PRC_RESAMPLE_METHOD_ANGLE の修正

  Revision 1.5  2004/06/28 11:11:36  seiki_masashi
  見本パターンの一覧に regularizeSize を追加
  各種パラメータ類を整理

  Revision 1.4  2004/06/25 13:32:11  seiki_masashi
  認識アルゴリズム 'Standard' の追加

  Revision 1.3  2004/06/24 08:08:33  seiki_masashi
  fix typo

  Revision 1.2  2004/06/24 07:03:39  seiki_masashi
  ヘッダファイルを分割・整理しました

  Revision 1.1  2004/06/23 11:59:30  seiki_masashi
  add 'PRC*' to SDK

  Revision 1.1  2004/06/22 11:19:00  seiki_masashi
  動作可能バージョン(simpleのみ)


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nitro/prc/resample.h>

/*===========================================================================*
  Prototype Declarations
 *===========================================================================*/


/*===========================================================================*
  Variable Definitions
 *===========================================================================*/

/*===========================================================================*
  Functions
 *===========================================================================*/

static BOOL
PRCi_TerminateStrokes(u16 *selectedPoints,
                      int *pSelectedPointNum, int maxPointCount, const PRCStrokes *strokes)
{
    int     selectedPointNum = *pSelectedPointNum;
    const PRCPoint *inputPoints;

    inputPoints = strokes->points;
    if (selectedPointNum < 2)
    {
        // 長さが 0 や 1 の点列は無視
        *pSelectedPointNum = 0;
        return FALSE;
    }
    if (!PRC_IsPenUpMarker(&inputPoints[selectedPoints[selectedPointNum - 1]]))
    {
        // Pen Up Marker で終わっていない
        if (!PRC_IsPenUpMarker(&inputPoints[selectedPoints[selectedPointNum - 2]]))
        {
            // 2つ前も Pen Up Marker ではない
            if (selectedPointNum < maxPointCount)
            {
                // 後ろに余裕があれば Pen Up Marker を付ける
                selectedPoints[selectedPointNum] = (u16)-1;
                selectedPointNum++;
            }
            else
            {
                // 後ろに空きがない場合
                if (selectedPointNum >= 3
                    && !PRC_IsPenUpMarker(&inputPoints[selectedPoints[selectedPointNum - 3]]))
                {
                    // 3つ前も Pen Up Marker でないならば、最後を Pen Up で上書き
                    selectedPoints[selectedPointNum - 1] = (u16)-1;
                }
                else
                {
                    // さもなくば、2つ削る
                    selectedPointNum -= 2;
                }
            }
        }
        else
        {
            // 2つ前が Pen  Up Marker なら何も考えず1つ切り詰める
            selectedPointNum--;
        }
    }

    *pSelectedPointNum = selectedPointNum;
    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         PRC_ResampleStrokes_None

  Description:  リサンプリングしません.

  Arguments:    selectedPoints, pSelectedPointNum 結果が返るポインタ
                maxPointCount       入力点数の上限(pen up marker を含む)
                maxStrokeCount      画数の上限
                strokes             整形前の生入力座標値.
                threshold           リサンプルの閾値
                buffer              作業領域(sizeof(int)*maxPointCount 必要)

  Returns:      リサンプリングに成功したら真.
 *---------------------------------------------------------------------------*/
BOOL
PRC_ResampleStrokes_None(u16 *selectedPoints,
                         int *pSelectedPointNum,
                         int maxPointCount,
                         int maxStrokeCount, const PRCStrokes *strokes, int threshold, void *buffer)
{
    // リサンプルせずに生の点列を使う
    u16     iPoint;
    int     size = strokes->size;

    (void)maxStrokeCount;
    (void)threshold;
    (void)buffer;

    if (size > maxPointCount)
    {
        size = maxPointCount;
    }
    if (size < 2)
    {
        // 長さが 0 や 1 の点列は無視
        *pSelectedPointNum = 0;
    }
    else
    {
        // 何も考えずに点を選択
        for (iPoint = 0; iPoint < size; iPoint++)
        {
            selectedPoints[iPoint] = iPoint;
        }
        *pSelectedPointNum = iPoint;

        if (!PRC_IsPenUpMarker(&strokes->points[size - 1]))
        {
            // Pen Up Marker で終端されていない
            (void)PRCi_TerminateStrokes(selectedPoints, pSelectedPointNum, maxPointCount, strokes);
        }
    }

    return (*pSelectedPointNum > 0);
}

#define PRCi_ABS(x) (((x)>=0)?(x):-(x))

/*---------------------------------------------------------------------------*
  Name:         PRC_ResampleStrokes_Distance

  Description:  シティブロック距離を基準にリサンプリングします.

  Arguments:    selectedPoints, pSelectedPointNum 結果が返るポインタ
                maxPointCount       入力点数の上限(pen up marker を含む)
                maxStrokeCount      画数の上限
                strokes             整形前の生入力座標値.
                threshold           リサンプルの閾値
                buffer              作業領域(sizeof(int)*maxPointCount 必要)

  Returns:      リサンプリングに成功したら真.
 *---------------------------------------------------------------------------*/
BOOL
PRC_ResampleStrokes_Distance(u16 *selectedPoints,
                             int *pSelectedPointNum,
                             int maxPointCount,
                             int maxStrokeCount,
                             const PRCStrokes *strokes, int threshold, void *buffer)
{
    int     selectedPointNum;
    int     strokeCount;
    int     iPoint;
    int     size;
    PRCPoint prevPoint;
    PRCPoint *point;
    BOOL    newFlag;
    int     length;

    SDK_ASSERT(maxPointCount > 0);
    SDK_ASSERT(maxStrokeCount > 0);

    (void)buffer;

    selectedPointNum = 0;
    strokeCount = 0;

    size = strokes->size;
    point = strokes->points;

    newFlag = TRUE;
    for (iPoint = 0; iPoint < size && selectedPointNum < maxPointCount; iPoint++, point++)
    {
        if (!PRC_IsPenUpMarker(point))
        {
            if (newFlag)
            {
                // 始点は必ず選択
                selectedPoints[selectedPointNum] = (u16)iPoint;
                selectedPointNum++;
                length = 0;
                newFlag = FALSE;
            }
            else
            {
                length += PRCi_ABS(point->x - prevPoint.x) + PRCi_ABS(point->y - prevPoint.y);
                if (length >= threshold)
                {
                    selectedPoints[selectedPointNum] = (u16)iPoint;
                    selectedPointNum++;
                    length = 0;
                }
            }
            prevPoint = *point;
        }
        else
        {
            if (newFlag)
            {
                // 連続する Pen Up Marker は無視
                continue;
            }
            else
            {
                if (selectedPoints[selectedPointNum - 1] != iPoint - 1) // ここにくるときは常に selectedPointNum>0
                {
                    // 終点も必ず選択
                    selectedPoints[selectedPointNum] = (u16)(iPoint - 1);
                    selectedPointNum++;
                    if (selectedPointNum >= maxPointCount)
                    {
                        break;
                    }
                }
                selectedPoints[selectedPointNum] = (u16)iPoint;
                selectedPointNum++;
                newFlag = TRUE;

                strokeCount++;
                if (strokeCount >= maxStrokeCount)
                {
                    // 画数制限超過
                    iPoint++;          // ループ後の処理のため // 現時点では newFlag == TRUE なので意味なし
                    break;
                }
            }
        }
    }

    *pSelectedPointNum = selectedPointNum;

    if (!newFlag)
    {
        // Pen Up Marker で終端されていない

        // まず、終点が選択されているかを確認
        if (selectedPointNum > 0 && selectedPoints[selectedPointNum - 1] != iPoint - 1
            && selectedPointNum < maxPointCount)
        {
            // 終点も必ず選択
            selectedPoints[*pSelectedPointNum] = (u16)(iPoint - 1);
            (*pSelectedPointNum)++;
        }

        (void)PRCi_TerminateStrokes(selectedPoints, pSelectedPointNum, maxPointCount, strokes);
    }

    return (*pSelectedPointNum > 0);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_ResampleStrokes_Angle

  Description:  角度のずれを基準にリサンプリングします.

  Arguments:    selectedPoints, pSelectedPointNum 結果が返るポインタ
                maxPointCount       入力点数の上限(pen up marker を含む)
                maxStrokeCount      画数の上限
                strokes             整形前の生入力座標値.
                threshold           リサンプルの閾値
                buffer              作業領域(sizeof(int)*maxPointCount 必要)

  Returns:      リサンプリングに成功したら真.
 *---------------------------------------------------------------------------*/
BOOL
PRC_ResampleStrokes_Angle(u16 *selectedPoints,
                          int *pSelectedPointNum,
                          int maxPointCount,
                          int maxStrokeCount,
                          const PRCStrokes *strokes, int threshold, void *buffer)
{
#define PRC_RESAMPLE_ANGLE_LENGTH_THRESHOLD 6   // 6 city-block length くらい離れないと有効な角度が取れない
    int     selectedPointNum;
    int     strokeCount;
    int     iPoint;
    int     size;
    PRCPoint *point;
    BOOL    newFlag;
    u16     prevAngle;
    PRCPoint prevPoint;
    BOOL    firstFlag;

    SDK_ASSERT(maxPointCount > 0);
    SDK_ASSERT(maxStrokeCount > 0);

    (void)buffer;

    selectedPointNum = 0;
    strokeCount = 0;

    size = strokes->size;
    point = strokes->points;

    newFlag = TRUE;
    for (iPoint = 0; iPoint < size && selectedPointNum < maxPointCount; iPoint++, point++)
    {
        if (!PRC_IsPenUpMarker(point))
        {
            if (newFlag)
            {
                // 始点は必ず選択
                selectedPoints[selectedPointNum] = (u16)iPoint;
                selectedPointNum++;
                prevPoint = *point;
                newFlag = FALSE;
                firstFlag = TRUE;
                if (iPoint + 1 < size)
                {
                    prevAngle =
                        FX_Atan2Idx(((point + 1)->y - point->y) << FX32_SHIFT,
                                    ((point + 1)->x - point->x) << FX32_SHIFT);
                }
            }
            else
            {
                int     length;
                length = PRCi_ABS(point->x - prevPoint.x) + PRCi_ABS(point->y - prevPoint.y);
                if (length >= PRC_RESAMPLE_ANGLE_LENGTH_THRESHOLD)
                {
                    if (firstFlag)
                    {
                        // 2点目を選ぶところまでは、次の点への角度を見る
                        // そうしないと、最初の書き出しの方向が失われる
                        if (iPoint + 1 < size && !PRC_IsPenUpMarker(point + 1))
                            // point+1 が Pen Up Marker のときはどちらにしろ終点として選択されるので
                            // 後ろの条件はなくてもいいのですが……
                        {
                            u16     currAngle, nextAngle;
                            nextAngle =
                                FX_Atan2Idx(((point + 1)->y - point->y) << FX32_SHIFT,
                                            ((point + 1)->x - point->x) << FX32_SHIFT);
                            if (PRCi_ABS((s16)(prevAngle - nextAngle)) >= threshold)
                            {
                                currAngle =
                                    FX_Atan2Idx((point->y - prevPoint.y) << FX32_SHIFT,
                                                (point->x - prevPoint.x) << FX32_SHIFT);
                                selectedPoints[selectedPointNum] = (u16)iPoint;
                                selectedPointNum++;
                                prevAngle = currAngle;
                            }
                        }
                        firstFlag = FALSE;
                    }
                    else
                    {
                        u16     currAngle;
                        currAngle =
                            FX_Atan2Idx((point->y - prevPoint.y) << FX32_SHIFT,
                                        (point->x - prevPoint.x) << FX32_SHIFT);
                        if (PRCi_ABS((s16)(prevAngle - currAngle)) >= threshold)
                        {
                            selectedPoints[selectedPointNum] = (u16)iPoint;
                            selectedPointNum++;
                            prevAngle = currAngle;
                        }
                    }
                    prevPoint = *point;
                }
            }
        }
        else
        {
            if (newFlag)
            {
                // 連続する Pen Up Marker は無視
                continue;
            }
            else
            {
                if (selectedPoints[selectedPointNum - 1] != iPoint - 1) // ここにくるときは常に selectedPointNum>0
                {
                    // 終点も必ず選択
                    selectedPoints[selectedPointNum] = (u16)(iPoint - 1);
                    selectedPointNum++;
                    if (selectedPointNum >= maxPointCount)
                    {
                        break;
                    }
                }
                selectedPoints[selectedPointNum] = (u16)iPoint;
                selectedPointNum++;
                newFlag = TRUE;

                strokeCount++;
                if (strokeCount >= maxStrokeCount)
                {
                    // 画数制限超過
                    iPoint++;          // ループ後の処理のため // 現時点では newFlag == TRUE なので意味なし
                    break;
                }
            }
        }
    }

    *pSelectedPointNum = selectedPointNum;

    if (!newFlag)
    {
        // Pen Up Marker で終端されていない

        // まず、終点が選択されているかを確認
        if (selectedPointNum > 0 && selectedPoints[selectedPointNum - 1] != iPoint - 1
            && selectedPointNum < maxPointCount)
        {
            // 終点も必ず選択
            selectedPoints[*pSelectedPointNum] = (u16)(iPoint - 1);
            (*pSelectedPointNum)++;
        }

        (void)PRCi_TerminateStrokes(selectedPoints, pSelectedPointNum, maxPointCount, strokes);
    }

    return (*pSelectedPointNum > 0);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_ResampleStrokes_Recursive

  Description:  再帰的手法でリサンプリングします.

  Arguments:    selectedPoints, pSelectedPointNum 結果が返るポインタ
                maxPointCount       入力点数の上限(pen up marker を含む)
                maxStrokeCount      画数の上限
                strokes             整形前の生入力座標値.
                threshold           リサンプルの閾値
                buffer              作業領域(sizeof(int)*maxPointCount 必要)

  Returns:      リサンプリングに成功したら真.
 *---------------------------------------------------------------------------*/
BOOL
PRC_ResampleStrokes_Recursive(u16 *selectedPoints,
                              int *pSelectedPointNum,
                              int maxPointCount,
                              int maxStrokeCount,
                              const PRCStrokes *strokes, int threshold, void *buffer)
{
    u16     beginIndex, endIndex;
    int     stackSize;
    int     stackTop, stackTail;
    int     strokeCount;
    int     selectedPointNum;
    int     size;
    PRCPoint *inputPoints;
    u16    *stackP1;
    u16    *stackP2;
    int     squaredThreshold;

    stackP1 = (u16 *)buffer;
    stackP2 = (u16 *)buffer + maxPointCount;

    squaredThreshold = threshold * threshold;

    beginIndex = 0;
    endIndex = 0;
    strokeCount = 0;
    selectedPointNum = 0;

    inputPoints = strokes->points;
    size = strokes->size;

    while (1)
    {
        if (selectedPointNum + 3 > maxPointCount || strokeCount > maxStrokeCount)
        {
            // 次の stroke を格納するスペースがない
            // stroke を1つ格納するには、始点・終点・PenUpMarker の
            // 3つの空きが最低でも必要
            break;
        }

        // PenUpMarker を読み飛ばす
        while (endIndex < size && PRC_IsPenUpMarker(&inputPoints[endIndex]))
        {
            endIndex++;
        }

        beginIndex = endIndex;
        if (beginIndex >= size)
        {
            // 終了
            break;
        }

        // 次の PenUpMarker を探す
        while (endIndex < size && !PRC_IsPenUpMarker(&inputPoints[endIndex]))
        {
            endIndex++;
        }
        if (endIndex < size)
        {
            selectedPoints[selectedPointNum] = endIndex;
            selectedPointNum++;        // 必要な PenUpMarker は選択
        }
        else
        {
            selectedPoints[selectedPointNum] = (u16)-1;
            selectedPointNum++;        // -1 は特別に終端の PenUpMarker を示す
        }

        SDK_ASSERT(endIndex > 0);
        selectedPoints[selectedPointNum] = beginIndex;
        selectedPointNum++;
        selectedPoints[selectedPointNum] = (u16)(endIndex - 1);
        selectedPointNum++;

        strokeCount++;                 // strokeCount は maxStrokeCount での制限のためだけに数えている

        if (selectedPointNum >= maxPointCount)
        {
            // 頂点数が限界
            break;
        }

        if (endIndex - beginIndex <= 2)
            continue;

        // stack を用いて再帰的に特徴点を抽出
        stackP1[0] = beginIndex;
        stackP2[0] = (u16)(endIndex - 1);
        stackSize = 1;
        stackTop = 0;
        stackTail = 1;
        while (stackSize > 0)
        {
            u16     p1, p2;
            int     x1, y1, x2, y2, xDir, yDir, offs;
            int     lastX, lastY;
            int     i;
            int     maxDist;
            u16     maxP;

            p1 = stackP1[stackTop];
            p2 = stackP2[stackTop];
            stackTop++;
            if (stackTop >= maxPointCount)
            {
                stackTop = 0;
            }
            stackSize--;               // pop

            if (p2 - p1 <= 1)
                continue;

            x1 = inputPoints[p1].x;    // 始点
            y1 = inputPoints[p1].y;
            x2 = inputPoints[p2].x;    // 終点
            y2 = inputPoints[p2].y;
            xDir = x2 - x1;            // 方向ベクトル
            yDir = y2 - y1;
            offs = -(x1 * y2 - x2 * y1);        // 計算量削減のためにくくりだした項

            maxDist = -1;
            maxP = (u16)-1;
            lastX = -1;
            lastY = -1;
            for (i = p1 + 1; i < p2; i++)
            {
                int     dist;
                int     x, y;
                x = inputPoints[i].x;
                y = inputPoints[i].y;

                if (lastX == x && lastY == y)
                    continue;
                lastX = x;
                lastY = y;

                // 直線と点の距離の計算
                // 実際は本来の距離に始点と終点の距離が掛けられた値である
                dist = x * yDir - y * xDir + offs;
                if (dist < 0)
                {
                    dist = -dist;
                }

                if (maxDist < dist)
                {
                    maxP = (u16)i;
                    maxDist = dist;
                }
            }

            // 始点座標と終点座標が完全に一致するケースでは必ず maxDist == 0 であるので
            // dist の計算は (x, y) と始点座標のユークリッド距離を別にとることが望ましいが
            // レアケース用の処理を入れたくないため、maxDist==0, maxP==p1+1 で p1+1 が
            // 常に採用されることにしてみる(その際は xDir*xDir+yDir*yDir == 0 であることに注意)
            if (maxDist * maxDist >= (xDir * xDir + yDir * yDir) * squaredThreshold)
            {
                // 閾値以上離れた点は特徴点として採用
                // maxDist は本来の距離に始点と終点の距離が掛けられた値であったことに注意
                selectedPoints[selectedPointNum] = maxP;
                selectedPointNum++;
                stackP1[stackTail] = maxP;
                stackP2[stackTail] = p2;
                stackTail++;
                if (stackTail >= maxPointCount)
                {
                    stackTail = 0;
                }
                stackSize++;           // push
                stackP1[stackTail] = p1;
                stackP2[stackTail] = maxP;
                stackTail++;
                if (stackTail >= maxPointCount)
                {
                    stackTail = 0;
                }
                stackSize++;           // push
                SDK_ASSERT(stackSize <= maxPointCount);
                if (selectedPointNum >= maxPointCount)
                {
                    // 頂点数が限界
                    break;
                }
            }
        }
    }

    *pSelectedPointNum = selectedPointNum;

//{OSTick start, end; start = OS_GetTick();
    // 返す前に昇順にソートする
    // ToDo: quick sort でどの程度高速化されるか検討する
    {
        int     i, j;
        for (i = 0; i < selectedPointNum - 1; i++)
        {
            for (j = i + 1; j < selectedPointNum; j++)
            {
                if (selectedPoints[i] > selectedPoints[j])
                {
                    u16     tmp;
                    tmp = selectedPoints[i];
                    selectedPoints[i] = selectedPoints[j];
                    selectedPoints[j] = tmp;
                }
            }
        }
    }
//end = OS_GetTick(); OS_Printf("// sort in resample: %lldμs selectedPointNum=%d\n", OS_TicksToMicroSeconds(end-start), selectedPointNum); }
    return (*pSelectedPointNum > 0);
}























/*===========================================================================*
  Static Functions
 *===========================================================================*/


/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Project:  NitroSDK - PRC - include
  File:     prc/common.h

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: common.h,v $
  Revision 1.8  2007/08/24 04:35:20  okubata_ryoma
  fix about headerfiles.

  Revision 1.7  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.6  2005/02/28 05:26:01  yosizaki
  do-indent.

  Revision 1.5  2004/07/19 06:52:06  seiki_masashi
  識別子名の整理

  Revision 1.4  2004/06/29 23:34:54  seiki_masashi
  PRC_IsEmpty

  Revision 1.3  2004/06/29 07:26:31  seiki_masashi
  PRC_IsPenUpMarker の高速化

  Revision 1.2  2004/06/25 13:32:01  seiki_masashi
  認識アルゴリズム 'Standard' の追加

  Revision 1.1  2004/06/24 07:03:10  seiki_masashi
  ヘッダファイルを分割・整理しました

  Revision 1.1  2004/06/23 12:00:29  seiki_masashi
  add 'PRC*' to SDK

  Revision 1.5  2004/06/22 11:19:00  seiki_masashi
  動作可能バージョン(simpleのみ)

  Revision 1.4  2004/06/17 08:14:52  seiki_masashi
  PRC_GetPrototypeDBBufferSizeEx

  Revision 1.3  2004/06/17 02:30:29  seiki_masashi
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

#ifndef NITRO_PRC_COMMON_H_
#define NITRO_PRC_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/misc.h>
#include <nitro/types.h>

#include <nitro/prc/types.h>
#include <nitro/fx/fx_cp.h>
#include <nitro/fx/fx_trig.h>

/*===========================================================================*
  Constant Definitions
 *===========================================================================*/

/*===========================================================================*
  Type Definitions
 *===========================================================================*/


/*===========================================================================*
  Function Declaration
 *===========================================================================*/

/*---------------------------------------------------------------------------*
  Name:         PRCi_Init

  Description:  図形認識 API を初期化します.
                各アルゴリズムの初期化ルーチンから最初に呼び出されます.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    PRCi_Init(void);

/*---------------------------------------------------------------------------*
  Name:         PRC_GetEntryStrokes

  Description:  見本DBと見本DBエントリから、点列データを得ます.

  Arguments:    strokes         得られた点列データ.
                                書き換えてはいけません.
                prototypeList   見本パターンの一覧.
                entry           見本DBエントリ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
 
 PRC_GetEntryStrokes(PRCStrokes *strokes,
                     const PRCPrototypeList *prototypeList, const PRCPrototypeEntry *entry);

/*---------------------------------------------------------------------------*
  Name:         PRC_CopyStrokes

  Description:  点列データをディープコピーします.

  Arguments:    srcStrokes      コピー元の PRC_Strokes 構造体.
                dstStrokes      コピー先の PRC_Strokes 構造体.

  Returns:      コピーできたら TRUE.
 *---------------------------------------------------------------------------*/
BOOL    PRC_CopyStrokes(const PRCStrokes *srcStrokes, PRCStrokes *dstStrokes);

/*===========================================================================*
  Inline Functions
 *===========================================================================*/

/*---------------------------------------------------------------------------*
  Name:         PRC_GetEntryCode

  Description:  見本DBエントリに対応するコード番号を返します.

  Arguments:    entry       見本DBエントリ.

  Returns:      コード番号.
 *---------------------------------------------------------------------------*/
static inline int PRC_GetEntryCode(const PRCPrototypeEntry *entry)
{
    return (entry != NULL) ? entry->code : -1;
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetEntryData

  Description:  見本DBエントリに紐付けされたユーザデータを返します.

  Arguments:    entry       見本DBエントリ.

  Returns:      ユーザデータ.
 *---------------------------------------------------------------------------*/
static inline void *PRC_GetEntryData(const PRCPrototypeEntry *entry)
{
    return (entry != NULL) ? entry->data : NULL;
}

/*---------------------------------------------------------------------------*
  Name:         PRC_InitStrokes

  Description:  PRCStrokes 構造体を初期化します.

  Arguments:    strokes     初期化対象.
                points      実メモリ領域となる PRCPoint の配列へのポインタ.
                size        配列の個数.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void PRC_InitStrokes(PRCStrokes *strokes, PRCPoint *points, u32 size)
{
    SDK_ASSERT(strokes && points);
    strokes->points = points;
    strokes->capacity = size;
    strokes->size = 0;
}

/*---------------------------------------------------------------------------*
  Name:         PRC_AppendPoint

  Description:  PRCStrokes に点を追加する.

  Arguments:    strokes     対象.
                x, y        加える点.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void PRC_AppendPoint(PRCStrokes *strokes, int x, int y)
{
    SDK_ASSERT(strokes);
    SDK_ASSERT(strokes->size < strokes->capacity);
    strokes->points[strokes->size].x = (s16)x;
    strokes->points[strokes->size].y = (s16)y;
    (strokes->size)++;
}

/*---------------------------------------------------------------------------*
  Name:         PRC_AppendPenUpMarker

  Description:  PRCStrokes に pen up を意味するマーカーを追加します.

  Arguments:    strokes     対象.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void PRC_AppendPenUpMarker(PRCStrokes *strokes)
{
    PRC_AppendPoint(strokes, PRC_PEN_UP_MARKER_X, PRC_PEN_UP_MARKER_Y);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_Clear

  Description:  PRCStrokes を空にします.

  Arguments:    strokes     対象.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void PRC_Clear(PRCStrokes *strokes)
{
    SDK_ASSERT(strokes);
    strokes->size = 0;
}

/*---------------------------------------------------------------------------*
  Name:         PRC_IsFull

  Description:  PRCStrokes が一杯かどうかを判定します.

  Arguments:    strokes     対象.

  Returns:      一杯なら TRUE.
 *---------------------------------------------------------------------------*/
static inline BOOL PRC_IsFull(const PRCStrokes *strokes)
{
    SDK_ASSERT(strokes);
    return strokes->size >= strokes->capacity;
}

/*---------------------------------------------------------------------------*
  Name:         PRC_IsEmpty

  Description:  PRCStrokes が空かどうかを判定します.

  Arguments:    strokes     対象.

  Returns:      空なら TRUE.
 *---------------------------------------------------------------------------*/
static inline BOOL PRC_IsEmpty(const PRCStrokes *strokes)
{
    SDK_ASSERT(strokes);
    return strokes->size == 0;
}

/*---------------------------------------------------------------------------*
  Name:         PRC_IsPenUpMarker

  Description:  pen up を意味するマーカーかどうか判断します.

  Arguments:    point       調べる対象.

  Returns:      pen up のマーカーだったら TRUE.
 *---------------------------------------------------------------------------*/
static inline int PRC_IsPenUpMarker(const PRCPoint *point)
{
    SDK_ASSERT(point);
    // point->x == PRC_PEN_UP_MARKER_X ならば point->y==PRC_PEN_UP_MARKER_Y
    SDK_ASSERT((point->x != PRC_PEN_UP_MARKER_X) || (point->y == PRC_PEN_UP_MARKER_Y));
    return (point->x == PRC_PEN_UP_MARKER_X);
}

/*---------------------------------------------------------------------------*
  Name:         PRCi_CalcDistance

  Description:  ユークリッド距離を算出します.

  Arguments:    p1, p2      調べる対象.

  Returns:      ユークリッド距離(fx32).
 *---------------------------------------------------------------------------*/
static inline fx32 PRCi_CalcDistance(const PRCPoint *p1, const PRCPoint *p2)
{
    SDK_ASSERT(p1 && p2);
    return (FX_Sqrt
            (((p1->x - p2->x) * (p1->x - p2->x) +
              (p1->y - p2->y) * (p1->y - p2->y)) << FX32_SHIFT));
}

/*---------------------------------------------------------------------------*
  Name:         PRCi_CalcAngle

  Description:  角度を算出します.

  Arguments:    p1, p2      調べる対象.

  Returns:      (2pi/65536)ラジアン単位の角度(u16).
 *---------------------------------------------------------------------------*/
static inline u16 PRCi_CalcAngle(const PRCPoint *from, const PRCPoint *to)
{
    SDK_ASSERT(from && to);
    return ((u16)(FX_Atan2Idx(to->y - from->y, to->x - from->x)));
}


#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_PRC_COMMON_H_ */
#endif

/*---------------------------------------------------------------------------*
  Project:  NitroSDK - include
  File:     qsort.h

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: qsort.h,v $
  Revision 1.6  2007/02/20 00:28:10  kitase_hirotake
  indent source

  Revision 1.5  2006/07/18 07:59:38  okubata_ryoma
  ｑsortのデモでソートする個数を0にしたときにASSERTで止まる問題を修正

  Revision 1.4  2006/01/18 02:12:28  kitase_hirotake
  do-indent

  Revision 1.3  2005/04/13 00:16:03  takano_makoto
  引数の型をintからu32に変更

  Revision 1.2  2005/04/12 23:55:34  takano_makoto
  fix for warning message.

  Revision 1.1  2005/04/12 10:16:14  takano_makoto
  Initial update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_MATH_QSORT_H_
#define NITRO_MATH_QSORT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/misc.h>
#include <nitro/types.h>
#include <nitro/math/math.h>

/* 値比較用関数 */
typedef s32 (*MATHCompareFunc) (void *elem1, void *elem2);

/* 最大必要スタックサイズ */
/*---------------------------------------------------------------------------*
  Name:         MATH_QSortStackSize
  
  Description:  MATH_QSortを行なう際に必要な作業バッファサイズを計算します。
  
  Arguments:    num      ソートするデータの個数
                
  Returns:      必要バッファサイズ.
 *---------------------------------------------------------------------------*/
static inline u32 MATH_QSortStackSize(u32 num)
{
    int     tmp = MATH_ILog2(num);

    if (tmp <= 0)
    {
        return sizeof(u32);
    }
    else
    {
        return (u32)((MATH_ILog2(num) + 1) * sizeof(u32) * 2);
    }
}


/*---------------------------------------------------------------------------*
  Name:         MATH_QSort
  
  Description:  再帰を使用せずにクイックソートを行ないます。
                ソート用のバッファ領域を渡す必要があり、
                必要なバッファサイズはMATH_QSORT_STACK_SIZE( num )で取得できます。
  
  Arguments:    head     ソートするデータへのポインタ
                num      ソートするデータの個数
                width    ソートするデータ１つ分のデータサイズ
                comp     比較関数ポインタ
                stackBuf ソート用バッファ
                
  Returns:      None.
 *---------------------------------------------------------------------------*/
void    MATH_QSort(void *head, u32 num, u32 width, MATHCompareFunc comp, void *stackBuf);


#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_MATH_QSORT_H_ */
#endif

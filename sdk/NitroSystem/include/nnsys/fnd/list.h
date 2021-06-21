/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - fnd
  File:     list.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.6 $
 *---------------------------------------------------------------------------*/

#ifndef NNS_FND_LIST_H_
#define NNS_FND_LIST_H_

#include <stddef.h>
#include <nitro/types.h>


#ifdef __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*
  Name:         NNSFndLink

  Description:  双方向リンクリストのノード構造体です。この構造体をリスト構造
                で繋ぎたい構造体のメンバとして格納します。
 *---------------------------------------------------------------------------*/
typedef struct
{
    void*       prevObject;     // 前に繋がれているオブジェクトへのポインタ。
    void*       nextObject;     // 次に繋がれているオブジェクトへのポインタ。

} NNSFndLink;


/*---------------------------------------------------------------------------*
  Name:         NNSFndList

  Description:  双方向リンクリスト構造体です。
 *---------------------------------------------------------------------------*/
typedef struct 
{
    void*       headObject;     // 先頭に繋がれているオブジェクトへのポインタ。
    void*       tailObject;     // 後尾に繋がれているオブジェクトへのポインタ。
    u16         numObjects;     // リストに繋がれているオブジェクトの個数。
    u16         offset;         // NNSFndLink型の構造体メンバのオフセット。

} NNSFndList;


/*---------------------------------------------------------------------------*
  Name:         NNS_FND_INIT_LIST

  Description:  リスト構造体を初期化するためのマクロです。実際の初期化には、
                NNSFndInitList()関数により行われます。

                このマクロでは、指定された構造体名とNNSFndLink型メンバ変数名
                から、offsetofマクロを使ってオフセットを求め、NNSFndInitList
                関数に渡しています。

  Arguments:    list:       リンク構造体へのポインタ。
                structName: リストに繋げたいオブジェクトの構造体名。
                linkName:   このオブジェクトのリンクに使用されるNNSFndLink型
                            のメンバ変数名。

  Returns:      なし。
 *---------------------------------------------------------------------------*/

#define NNS_FND_INIT_LIST(list, structName, linkName) \
            NNS_FndInitList(list, offsetof(structName, linkName))


/*---------------------------------------------------------------------------*
    関数プロトタイプ。

 *---------------------------------------------------------------------------*/

void    NNS_FndInitList(
                NNSFndList*             list,
                u16                     offset);

void    NNS_FndAppendListObject(
                NNSFndList*             list,
                void*                   object);

void    NNS_FndPrependListObject(
                NNSFndList*             list,
                void*                   object);

void    NNS_FndInsertListObject(
                NNSFndList*             list,
                void*                   target,
                void*                   object);

void    NNS_FndRemoveListObject(
                NNSFndList*             list,
                void*                   object);

void*   NNS_FndGetNextListObject(
                NNSFndList*             list,
                void*                   object);

void*   NNS_FndGetPrevListObject(
                NNSFndList*             list,
                void*                   object);

void*   NNS_FndGetNthListObject(
                NNSFndList*             list,
                u16                     index);


#ifdef __cplusplus
} /* extern "C" */
#endif

/* NNS_FND_LIST_H_ */
#endif

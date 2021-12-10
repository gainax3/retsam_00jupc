/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - fnd
  File:     list.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.8 $
 *---------------------------------------------------------------------------*/

#include <nnsys/misc.h>
#include <nnsys/fnd/list.h>

#define OBJ_TO_LINK(list,obj)   ((NNSFndLink*)(((u32)(obj))+(list)->offset))


/*---------------------------------------------------------------------------*
  Name:         NNS_FndInitList

  Description:  リスト構造体を初期化します。

  Arguments:    list:   リスト構造体へのポインタ。
                offset: リストに繋げたい構造体の中に存在する、NNSFndLink型の
                        メンバ変数の構造体内でのオフセットを指定します。
                        stddef.hで定義されているoffsetofマクロを使用すると、
                        便利です。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void
NNS_FndInitList(NNSFndList* list, u16 offset)
{
    NNS_NULL_ASSERT(list);

    list->headObject = NULL;
    list->tailObject = NULL;
    list->numObjects = 0;
    list->offset     = offset;
}

/*---------------------------------------------------------------------------*
  Name:         SetFirstObject                                      [static]

  Description:  最初のオブジェクトをリストに加えます。

  Arguments:    list:   リスト構造体へのポインタ。
                object: リストに繋げたいオブジェクトへのポインタ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static void
SetFirstObject(NNSFndList* list, void* object)
{
    NNSFndLink* link;

    NNS_NULL_ASSERT(list  );
    NNS_NULL_ASSERT(object);

    link = OBJ_TO_LINK(list, object);

    link->nextObject = NULL;
    link->prevObject = NULL;
    list->headObject = object;
    list->tailObject = object;
    list->numObjects++;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndAppendListObject

  Description:  オブジェクトをリストの最後に追加します。

  Arguments:    list:   リスト構造体へのポインタ。
                object: リストに繋げたいオブジェクトへのポインタ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void
NNS_FndAppendListObject(NNSFndList* list, void* object)
{
    NNS_NULL_ASSERT(list  );
    NNS_NULL_ASSERT(object);

    if (list->headObject == NULL)
    {
        // リストが空の時。
        SetFirstObject(list, object);
    }
    else
    {
        NNSFndLink* link = OBJ_TO_LINK(list, object);

        link->prevObject = list->tailObject;
        link->nextObject = NULL;

        OBJ_TO_LINK(list, list->tailObject)->nextObject = object;
        list->tailObject = object;
        list->numObjects++;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndPrependListObject

  Description:  オブジェクトをリストの先頭に挿入します。

  Arguments:    list:   リスト構造体へのポインタ。
                object: リストに繋げたいオブジェクトへのポインタ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void
NNS_FndPrependListObject(NNSFndList* list, void* object)
{
    NNS_NULL_ASSERT(list  );
    NNS_NULL_ASSERT(object);

    if (list->headObject == NULL)
    {
        // リストが空の時。
        SetFirstObject(list, object);
    }
    else
    {
        NNSFndLink* link = OBJ_TO_LINK(list, object);

        link->prevObject = NULL;
        link->nextObject = list->headObject;

        OBJ_TO_LINK(list, list->headObject)->prevObject = object;
        list->headObject = object;
        list->numObjects++;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndInsertListObject

  Description:  オブジェクトを指定された位置に挿入します。オブジェクトは、
                targetで指定されたオブジェクトの前に挿入されます。挿入先が指
                定されていない場合（targetがNULLの場合）、オブジェクトはリス
                トの最後に追加されます。

  Arguments:    list:   リスト構造体へのポインタ。
                target: 挿入したい位置にあるオブジェクトへのポインタ。
                object: リストに繋げたいオブジェクトへのポインタ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void
NNS_FndInsertListObject(NNSFndList* list, void* target, void* object)
{
    NNS_NULL_ASSERT(list  );
    NNS_NULL_ASSERT(object);

    if (target == NULL)
    {
        // targetが指定されていない場合は、NNS_FndAppendListObject()と同じ。
        NNS_FndAppendListObject(list, object);
    }
    else if (target == list->headObject)
    {
        // targetがリストの先頭である場合はNNS_FndPrependListObject()と同じ。
        NNS_FndPrependListObject(list, object);
    }
    else
    {
        NNSFndLink* link    = OBJ_TO_LINK(list, object);
        void*       prevObj = OBJ_TO_LINK(list, target)->prevObject;
        NNSFndLink* prevLnk = OBJ_TO_LINK(list, prevObj);

        link->prevObject    = prevObj;
        link->nextObject    = target;
        prevLnk->nextObject = object;
        OBJ_TO_LINK(list, target)->prevObject = object;
        list->numObjects++;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndRemoveListObject

  Description:  オブジェクトをリストから削除します。

  Arguments:    list:   リスト構造体へのポインタ。
                object: リストから削除したいオブジェクトへのポインタ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void
NNS_FndRemoveListObject(NNSFndList* list, void* object)
{
    NNSFndLink* link;

    NNS_NULL_ASSERT(list  );
    NNS_NULL_ASSERT(object);

    link = OBJ_TO_LINK(list, object);

    if (link->prevObject == NULL)
    {
        list->headObject = link->nextObject;
    }
    else
    {
        OBJ_TO_LINK(list, link->prevObject)->nextObject = link->nextObject;
    }
    if (link->nextObject == NULL)
    {
        list->tailObject = link->prevObject;
    }
    else
    {
        OBJ_TO_LINK(list, link->nextObject)->prevObject = link->prevObject;
    }
    link->prevObject = NULL;
    link->nextObject = NULL;
    list->numObjects--;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetNextListObject

  Description:  objectで指定されたオブジェクトの次に繋がれているオブジェクト
                を返します。objectにNULLが指定されていた場合には、リストの先
                頭に繋がれているオブジェクトを返します。

  Arguments:    list:   リスト構造体へのポインタ。
                object: リスト中のオブジェクトへのポインタ。

  Returns:      指定されたオブジェクトの次のオブジェクトへのポインタを返しま
                す。もし、次のオブジェクトが無ければ、NULLを返します。
 *---------------------------------------------------------------------------*/
void*
NNS_FndGetNextListObject(NNSFndList* list, void* object)
{
    NNS_NULL_ASSERT(list);

    if (object == NULL)
    {
        return list->headObject;
    }
    return OBJ_TO_LINK(list, object)->nextObject;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetPrevListObject

  Description:  objectで指定されたオブジェクトの前に繋がれているオブジェクト
                を返します。objectにNULLが指定されていた場合には、リストの後
                尾に繋がれているオブジェクトを返します。

  Arguments:    list:   リスト構造体へのポインタ。
                object: リスト中のオブジェクトへのポインタ。

  Returns:      指定されたオブジェクトの前のオブジェクトへのポインタを返しま
                す。もし、前のオブジェクトが無ければ、NULLを返します。
 *---------------------------------------------------------------------------*/
void*
NNS_FndGetPrevListObject(NNSFndList* list, void* object)
{
    NNS_NULL_ASSERT(list);

    if (object == NULL)
    {
        return list->tailObject;
    }
    return OBJ_TO_LINK(list, object)->prevObject;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetNthListObject

  Description:  リストのＮ番目に繋がれているオブジェクトへのポインタを返しま
                す。先頭から順番にリストをたどる為、リストの後部に繋がれてい
                オブジェクト程、時間がかかります。

  Arguments:    index:  オブジェクトのインデックス。

  Returns:      オブジェクトへのポインタを返します。もし、指定されたインデッ
                クスのオブジェクトが無かった場合には、NULLが返ります。
 *---------------------------------------------------------------------------*/
void*
NNS_FndGetNthListObject(NNSFndList* list, u16 index)
{
    int         count  = 0;
    NNSFndLink* object = NULL;

    NNS_NULL_ASSERT(list);

    while ((object = NNS_FndGetNextListObject(list, object)) != NULL)
    {
        if (index == count)
        {
            return object;
        }
        count++;
    }
    return NULL;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_UTCountListNode

  Description:  リストに繋がれているオブジェクトの数をカウントします。先頭か
                ら順番にリストをカウントする為、リストが長いと時間がかかりま
                す。

  Arguments:    list:   リスト構造体へのポインタ。
 
  Returns:      リストに繋がれているオブジェクトの数。
 *---------------------------------------------------------------------------*/
#if 0
int
NNS_UTCountListNode(NNS_UTList* list)
{
    int             count  = 0;
    NNS_UTListNode* object = NULL;

    NNS_NULL_ASSERT(list);

    while ((object = NNS_UTGetNextListObject(list, object)) != NULL)
    {
        count++;
    }
    return count;
}
#endif

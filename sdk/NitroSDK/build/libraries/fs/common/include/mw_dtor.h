/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FS - libraries
  File:     mw_dtor.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/


#if	!defined(NITRO_FS_MW_DTOR_H_)
#define NITRO_FS_MW_DTOR_H_

#include <nitro/misc.h>
#include <nitro/types.h>


/* CodeWarrior コンパイラのグローバルデストラクタチェーン操作 */


#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/
/* declaration */

/*
 * デストラクタコールバック.
 * 第一引数は this 相当のオブジェクトポインタ.
 */
typedef void (*MWI_DESTRUCTOR_FUNC) (void *);

/*
 * デストラクタチェーン.
 * グローバルオブジェクトが個々にひとつ用意する.
 */
typedef struct MWiDestructorChain
{
    struct MWiDestructorChain *next;   /* next chain */
    MWI_DESTRUCTOR_FUNC dtor;          /* destructor */
    void   *obj;                       /* this */
}
MWiDestructorChain;

/*****************************************************************************/
/* variable */

/*
 * グローバルデストラクタチェーン.
 * ctor の逆順に呼ばれるよう, 先頭に挿入されていく.
 */
extern MWiDestructorChain *__global_destructor_chain;


/*****************************************************************************/
/* function */

/*---------------------------------------------------------------------------*
  Name:         __register_global_object

  Description:  処理系定義関数.
                グローバルデストラクタチェーンの先頭にチェーンをひとつ挿入.

  Arguments:    obj              解体対象オブジェクトのポインタ.
                dtor             デストラクタルーチン.
                chain            チェーン構造体のポインタ.
                                 (グローバルオブジェクトが個々にひとつ用意する)

  Returns:      None.
 *---------------------------------------------------------------------------*/

void    __register_global_object(void *obj, MWI_DESTRUCTOR_FUNC dtor, MWiDestructorChain * chain);
/*
{
	chain->next = __global_destructor_chain;
	chain->dtor = dtor;
	chain->obj = obj;
	__global_destructor_chain = chain;
}
*/


/*****************************************************************************/


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* NITRO_FS_UTIL_H_ */

/*---------------------------------------------------------------------------*
  $Log: mw_dtor.h,v $
  Revision 1.5  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.4  2005/05/30 04:13:39  yosizaki
  add comments.

  Revision 1.3  2005/03/01 01:57:00  yosizaki
  copyright の年を修正.

  Revision 1.2  2005/02/28 05:26:02  yosizaki
  do-indent.

  Revision 1.1  2004/07/08 13:40:41  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

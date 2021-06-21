/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g2d
  File:     g2d_Entity.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.15 $
 *---------------------------------------------------------------------------*/

#include <nnsys/g2d/g2d_Entity.h>
#include <nnsys/g2d/g2d_CellAnimation.h>
#include <nnsys/g2d/g2d_MultiCellAnimation.h>
#include <nnsys/g2d/load/g2d_NAN_load.h>
#include "g2d_Internal.h"

//------------------------------------------------------------------------------
static NNS_G2D_INLINE void ResetPaletteTbl_( NNSG2dEntity* pEntity )
{
    
    NNS_G2D_ASSERT_ENTITY_VALID( pEntity );
    
    
    NNS_G2D_NULL_ASSERT( pEntity );
    
    // カラーパレット変換テーブルのリセット
    NNS_G2dResetEntityPaletteTable( pEntity );
}

//------------------------------------------------------------------------------
static NNS_G2D_INLINE void SetCurrentAnimation_( NNSG2dEntity* pEntity )
{
    NNS_G2D_NULL_ASSERT( pEntity );
    NNS_G2D_ASSERT_ENTITY_VALID( pEntity );
    
    {
        const NNSG2dAnimSequence* pAnimSeq = NNS_G2dGetAnimSequenceByIdx( pEntity->pAnimDataBank,
                                                             pEntity->currentSequenceIdx );
        if( pAnimSeq )
        {
            switch( pEntity->pEntityData->type )
            {
            case NNS_G2D_ENTITYTYPE_CELL:
                NNS_G2dSetCellAnimationSequence( (NNSG2dCellAnimation*)pEntity->pDrawStuff, pAnimSeq );
                break;
            case NNS_G2D_ENTITYTYPE_MULTICELL:
                NNS_G2dSetAnimSequenceToMCAnimation( (NNSG2dMultiCellAnimation*)pEntity->pDrawStuff, pAnimSeq );
                break;
            default:
                NNS_G2D_ASSERT( FALSE );
            }
        }else{
            NNS_G2D_ASSERT( FALSE );
        }
    }   
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitEntity

  Description:  Entity 実体を初期化します
                
                
  Arguments:    pEntity         [OUT] エンティティ 
                pDrawStuff      [IN]  描画用データ( NNSG2dCellAnimation or NNSG2dMultiCellAnimation ) 
                pEntityData     [IN]  エンティティ定義データ 
                pAnimDataBank   [IN]  アニメーションデータバンク 

  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dInitEntity
( 
    NNSG2dEntity*               pEntity, 
    void*                       pDrawStuff, 
    const NNSG2dEntityData*     pEntityData, 
    const NNSG2dAnimBankData*   pAnimDataBank 
)
{
    NNS_G2D_NULL_ASSERT( pEntity );
    NNS_G2D_NULL_ASSERT( pDrawStuff );
    NNS_G2D_NULL_ASSERT( pEntityData );
    NNS_G2D_NULL_ASSERT( pAnimDataBank );
    
    
    // pEntityData->type
    pEntity->pDrawStuff             = pDrawStuff;
    pEntity->pAnimDataBank          = pAnimDataBank;
    pEntity->pEntityData            = pEntityData;
    pEntity->currentSequenceIdx     = 0;
     
    // カラーパレット変換テーブルを初期化する
    ResetPaletteTbl_( pEntity );
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetEntityCurrentAnimation

  Description:  NNSG2dEntity の現在の再生アニメーションを設定する
                
                
  Arguments:    pEntity:            [OUT] NNSG2dEntity 実体
                idx:                [IN]  NNSG2dEntity 内での AnimationSequence Index
                
                アニメーションはロード済みであること。
                ロードされていなければなにもしません。
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetEntityCurrentAnimation( NNSG2dEntity* pEntity, u16 idx )
{
    NNS_G2D_NULL_ASSERT( pEntity );
    NNS_G2D_NULL_ASSERT( pEntity->pAnimDataBank );
    NNS_G2D_NULL_ASSERT( pEntity->pEntityData );
    
    if( pEntity->pEntityData->animData.numAnimSequence > idx )
    {
        pEntity->currentSequenceIdx = pEntity->pEntityData->animData.pAnimSequenceIdxArray[idx];
        // TODO:この処理は初期化時に シーケンス番号配列の要素全部に対して行うべき！
        NNS_G2D_ASSERT( pEntity->pAnimDataBank->numSequences > pEntity->currentSequenceIdx );
        //
        // アニメーション切り替え処理
        //
        SetCurrentAnimation_( pEntity );
    }else{
        NNSI_G2D_DEBUGMSG0( FALSE, "Failure of finding animation data in NNS_G2dSetEntityCurrentAnimation()" );
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetColorPaletteTable

  Description:  NNSG2dEntity に カラーパレット変換テーブルを設定する
                
                
  Arguments:    pEntity:            [OUT] NNSG2dEntity 実体
                pPlttTbl:           [IN]  カラーパレット変換テーブル
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetEntityPaletteTable
( 
    NNSG2dEntity*           pEntity, 
    NNSG2dPaletteSwapTable* pPlttTbl 
)
{
    NNS_G2D_NULL_ASSERT( pEntity );
    NNS_G2D_NULL_ASSERT( pPlttTbl );
    
    pEntity->pPaletteTbl = pPlttTbl;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dResetEntityPaletteTable

  Description:  NNSG2dEntity の カラーパレット変換テーブルを無効に設定する
                
                
  Arguments:    pEntity:            [OUT] NNSG2dEntity 実体
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dResetEntityPaletteTable
( 
    NNSG2dEntity*           pEntity
)
{
    NNS_G2D_NULL_ASSERT( pEntity );
    pEntity->pPaletteTbl = NULL;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dIsEntityPaletteTblEnable

  Description:  NNSG2dEntity の カラーパレットが有効か調査する
                
                
  Arguments:    pEntity:            [IN]  NNSG2dEntity 実体
                
                
                
  Returns:      カラーパレットが有効ならTRUE
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dIsEntityPaletteTblEnable( const NNSG2dEntity* pEntity )
{
    NNS_G2D_NULL_ASSERT( pEntity );
    return ( pEntity->pPaletteTbl != NULL ) ? TRUE : FALSE;
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTickEntity

  Description:  NNSG2dEntity を更新する
                
                
  Arguments:    pEntity:            [OUT] NNSG2dEntity 実体
                dt:                 [IN]  進める時間（フレーム）
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void  NNS_G2dTickEntity( NNSG2dEntity*    pEntity, fx32 dt )
{
    NNS_G2D_NULL_ASSERT( pEntity );
    NNS_G2D_ASSERT_ENTITY_VALID( pEntity );
    
    switch( pEntity->pEntityData->type )
    {
    case NNS_G2D_ENTITYTYPE_CELL:
        NNS_G2dTickCellAnimation( (NNSG2dCellAnimation*)pEntity->pDrawStuff, dt );
        break;
    case NNS_G2D_ENTITYTYPE_MULTICELL:
        NNS_G2dTickMCAnimation( (NNSG2dMultiCellAnimation*)pEntity->pDrawStuff, dt );
        break;
    default:
        NNS_G2D_ASSERT(FALSE);
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetEntityCurrentFrame

  Description:  NNSG2dEntity の再生アニメーションフレームを設定します
                
                
  Arguments:    pEntity:            [OUT] NNSG2dEntity 実体
                frameIndex:         [IN]  アニメーションフレーム番号
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void  NNS_G2dSetEntityCurrentFrame
( 
    NNSG2dEntity*    pEntity,
    u16              frameIndex  
)
{
    NNS_G2D_NULL_ASSERT( pEntity );
    NNS_G2D_ASSERT_ENTITY_VALID( pEntity );
    
    switch( pEntity->pEntityData->type )
    {
    case NNS_G2D_ENTITYTYPE_CELL:
        NNS_G2dSetCellAnimationCurrentFrame( (NNSG2dCellAnimation*)pEntity->pDrawStuff, 
                                             frameIndex );
        break;
    case NNS_G2D_ENTITYTYPE_MULTICELL:
        NNS_G2dSetMCAnimationCurrentFrame( (NNSG2dMultiCellAnimation*)pEntity->pDrawStuff,
                                             frameIndex );
        break;
    default:
        NNS_G2D_ASSERT(FALSE);
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetEntitySpeed

  Description:  NNSG2dEntity のアニメーションスピードを設定する
                
                
  Arguments:    pEntity:            [OUT] NNSG2dEntity 実体
                speed:              [IN]  アニメーションスピード
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetEntitySpeed
(
    NNSG2dEntity*     pEntity,
    fx32              speed  
)
{
    NNS_G2D_NULL_ASSERT( pEntity );
    NNS_G2D_ASSERT_ENTITY_VALID( pEntity );
    
    switch( pEntity->pEntityData->type )
    {
    case NNS_G2D_ENTITYTYPE_CELL:
        NNS_G2dSetCellAnimationSpeed( (NNSG2dCellAnimation*)pEntity->pDrawStuff, 
                                             speed );
        break;
    case NNS_G2D_ENTITYTYPE_MULTICELL:
        NNS_G2dSetMCAnimationSpeed( (NNSG2dMultiCellAnimation*)pEntity->pDrawStuff,
                                             speed );
        break;
    default:
        NNS_G2D_ASSERT(FALSE);
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dIsEntityValid

  Description:  NNSG2dEntity が 有効な状態か調査します
                
                
  Arguments:    pEntity:            [IN]  NNSG2dEntity 実体
                
                
  Returns:      有効な状態ならTRUEをかえす
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dIsEntityValid( NNSG2dEntity*    pEntity )
{
    NNS_G2D_NULL_ASSERT( pEntity );
    
    if( ( pEntity->pEntityData     != NULL ) &&
        ( pEntity->pDrawStuff      != NULL ) &&
        ( pEntity->pAnimDataBank   != NULL )  )
        {
            return TRUE;
        }else{
            return FALSE;
        }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetEntityAnimCtrl

  Description:  エンティティのアニメーションコントローラを取得します。
                
                
  Arguments:    pEntity:            [IN]  NNSG2dEntity 実体
                
                
  Returns:      アニメーションコントローラ
  
 *---------------------------------------------------------------------------*/
NNSG2dAnimController* NNS_G2dGetEntityAnimCtrl( NNSG2dEntity*    pEntity )
{
    NNS_G2D_NULL_ASSERT( pEntity );
    switch( pEntity->pEntityData->type )
    {
    case NNS_G2D_ENTITYTYPE_CELL:
        return NNS_G2dGetCellAnimationAnimCtrl( (NNSG2dCellAnimation*)pEntity->pDrawStuff );
    case NNS_G2D_ENTITYTYPE_MULTICELL:
        return NNS_G2dGetMCAnimAnimCtrl( (NNSG2dMultiCellAnimation*)pEntity->pDrawStuff );
    default:
        NNS_G2D_ASSERT(FALSE);
        return NULL;
    }
}




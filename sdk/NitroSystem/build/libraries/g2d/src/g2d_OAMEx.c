/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g2d
  File:     g2d_OAMEx.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.31 $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nnsys/g2d/g2d_OAMEx.h>
#include <nnsys/g2d/fmt/g2d_Cell_data.h>

#include "g2d_Internal.h"





static GXOamAttr       defaultOam_ = { 193, 193, 193 };







//------------------------------------------------------------------------------
// 各種登録関数 が 妥当か調査します
static NNS_G2D_INLINE BOOL IsOamEntryFuncsValid_
( 
    const NNSG2dOamManagerInstanceEx*  pMan,
    const NNSG2dOamExEntryFunctions*   pF 
)
{
    const BOOL bValid = (BOOL)(( pF                       != NULL ) &&
                               ( pF->getOamCapacity       != NULL ) &&    
                               ( pF->entryNewOam          != NULL ) );
                      
    // アフィンパラメータを管理する場合には...         
    if( pMan->pAffineBuffer != NULL || pMan->lengthAffineBuffer != 0 )
    {
        // 関連するコールバックが正しく設定されている必要があります。
        return (BOOL)( bValid &&
                      ( pF->getAffineCapacity    != NULL ) &&
                      ( pF->entryNewAffine       != NULL ) );
    }else{
        return bValid;  
    }
    
}

//------------------------------------------------------------------------------
// 共有 NNSG2dOamChunk から 新しい NNSG2dOamChunk を 取得します。
// 共有 NNSG2dOamChunk は 初期化時に ユーザが設定します。
//
static NNS_G2D_INLINE NNSG2dOamChunk* GetNewOamChunk_( NNSG2dOamManagerInstanceEx* pMan, const GXOamAttr* pOam )
{
    NNSG2dOamChunk*   pRet = NULL;
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_NULL_ASSERT( pOam );
    
    if( pMan->numUsedOam < pMan->numPooledOam )
    {
        
        pRet = &pMan->pPoolOamChunks[pMan->numUsedOam];
        pMan->numUsedOam++;
        
        pRet->oam = *pOam;
        return pRet;
    }else{
        NNSI_G2D_DEBUGMSG0("We have no capacity for a new Oam.");
        return NULL;
    }
}

//------------------------------------------------------------------------------
// NNSG2dOamChunk を NNSG2dOamChunkList の先頭に加えます
static NNS_G2D_INLINE void AddFront_( NNSG2dOamChunkList* pOamList, NNSG2dOamChunk* pChunk )
{
    pChunk->pNext       = pOamList->pChunks;
    pOamList->pChunks   = pChunk;
    
    pOamList->numChunks++;
}

//------------------------------------------------------------------------------
// NNSG2dOamChunk を NNSG2dOamChunkList の終端に加えます
static NNS_G2D_INLINE void AddBack_( NNSG2dOamChunkList* pOamList, NNSG2dOamChunk* pChunk )
{
    pChunk->pNext               = NULL;
    
    if( pOamList->pLastChunk != NULL )
    {
        // 初回以降
        pOamList->pLastChunk->pNext = pChunk;
    }else{
        // 初回
        pOamList->pChunks    = pChunk;
    }
    
    // リスト終端の更新
    pOamList->pLastChunk = pChunk;
    
    pOamList->numChunks++;
}

//------------------------------------------------------------------------------
// 下位モジュールにOamを登録します
static NNS_G2D_INLINE void EntryOamToToBaseModule_( NNSG2dOamManagerInstanceEx* pMan, const GXOamAttr* pOam, u16 totalOam )
{
    (void)(*pMan->oamEntryFuncs.entryNewOam)( pOam, totalOam );
}


//------------------------------------------------------------------------------
// affineProxy が 妥当か判定します
static NNS_G2D_INLINE BOOL IsAffineProxyValid_( NNSG2dOamManagerInstanceEx* pMan )
{
    return ( pMan->pAffineBuffer != NULL && pMan->lengthAffineBuffer != 0 ) ? 
        TRUE : FALSE;
}
//------------------------------------------------------------------------------
// 新規登録に十分な容量が残っているか調査します。
static NNS_G2D_INLINE BOOL HasEnoughCapacity_( NNSG2dOamManagerInstanceEx* pMan )
{
    NNS_G2D_NULL_ASSERT( pMan );
    return ( pMan->numAffineBufferUsed < pMan->lengthAffineBuffer ) ? TRUE : FALSE;
}
//------------------------------------------------------------------------------
// affineProxyIdx => affineHWIndex への変換をします
static NNS_G2D_INLINE u16 ConvertAffineIndex_( NNSG2dOamManagerInstanceEx* pMan, u16 affineProxyIdx )
{
    NNS_G2D_ASSERT( IsAffineProxyValid_( pMan ) );
    NNS_G2D_ASSERT( affineProxyIdx < pMan->lengthAffineBuffer );
        
    return pMan->pAffineBuffer[affineProxyIdx].affineHWIndex;            
}

//------------------------------------------------------------------------------
// NNSG2dOamChunkList 中の すべての NNSG2dOamChunk の 参照 affine パラメータ データ
// を 実際に HWに設定された データに変更する
//
// アフィン変換されたOAMの描画順と通常のOAMの描画順が統一されました。
// 従来は、アフィン変換されたOAMは登録順とは逆の順番で描画されていました。
// 
// なお、NNS_G2D_OAMEX_USE_OLD_REINDEXOAMCHUNKLIST_ を定義してライブラリを
// 再ビルドすることで、過去の実装と同じ振る舞いに戻すことが可能です。
//
//#define NNS_G2D_OAMEX_USE_OLD_REINDEXOAMCHUNKLIST_ 1

static NNS_G2D_INLINE void ReindexOamChunkList_
( 
    NNSG2dOamManagerInstanceEx*     pMan, 
    NNSG2dOamChunkList*             pChunkList
)
{
#ifdef NNS_G2D_OAMEX_USE_OLD_REINDEXOAMCHUNKLIST_
    //
    // 過去の実装
    //
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_NULL_ASSERT( pChunkList );
    {
        NNSG2dOamChunk*    pChunk     = pChunkList->pAffinedChunks;
        NNSG2dOamChunk*    pNextChunk = NULL;
        
        //
        // リスト終端まで...
        //
        while( pChunk != NULL )
        {
            //
            // インデックスを AffineProxyIndexから、実際のHW Indexに 変換する
            //
            const u16 index = ConvertAffineIndex_( pMan, pChunk->affineProxyIdx );
            
            //OS_Printf("AffProxy_Idx = %d, => HW_Idx = %d \n", pChunk->affineProxyIdx, index );
            
            pNextChunk = pChunk->pNext;
            
            // インデックスが有効な値ならば...
            if( index != NNS_G2D_OAMEX_HW_ID_NOT_INIT )
            {
                // OAM の Index を 付け替える
                // G2_SetOBJEffect( &pChunk->oam, NNS_G2dGetOamManExDoubleAffineFlag( pMan ), index );
                pChunk->oam.rsParam = index;
                
                //
                // 通常の OamChunkListに移動する
                // 通常の OamChunkListに登録順とは逆順に登録されていくことになります
                //
                AddFront_( pChunkList, pChunk );
            }
            pChunk     = pNextChunk;
        }
    }
#else // NNS_G2D_OAMEX_USE_OLD_REINDEXOAMCHUNKLIST_
    //
    // 現在の実装
    //
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_NULL_ASSERT( pChunkList );
    {
        NNSG2dOamChunk*    pChunk     = pChunkList->pAffinedChunks;
        NNSG2dOamChunk*    pHeadChunk = NULL;
        NNSG2dOamChunk*    pPrevChunk = NULL;
        int                numAffinedOam = 0;
        
        
        //
        // アフィン変換されたOAMリスト終端まで...
        //
        while( pChunk != NULL )
        {
            //
            // インデックスを AffineProxyIndexから、実際のHW Indexに 変換する
            //
            const u16 index = ConvertAffineIndex_( pMan, pChunk->affineProxyIdx );
            
            // インデックスが有効な値ならば...
            if( index != NNS_G2D_OAMEX_HW_ID_NOT_INIT )
            {
                // （有効な）アフィン変換されたOAMリストの先頭を記憶しておく
                if( pHeadChunk == NULL )
                {
                    pHeadChunk = pChunk;
                }
                
                //
                // OAM の Index を 付け替える
                //
                pChunk->oam.rsParam = index;
                
                
                // 次のチャンクへ
                pPrevChunk = pChunk;
                pChunk     = pChunk->pNext;
                numAffinedOam++;
                
            }else{
                // リストから取り除く
                if( pPrevChunk != NULL )
                {
                    pPrevChunk->pNext = pChunk->pNext;
                }
                pMan->numUsedOam--;
                // 次のチャンクへ
                pChunk     = pChunk->pNext;
            }
        }
        
        //
        // pPrevChunk->pNext => アフィン変換されたOAMのリスト終端
        // アフィン変換されたOAMのリストをまとめて、通常のチャンクリストの先頭に挿入します
        // アフィン変換されたOAMのリスト内での順番の変更はおこりません
        // 
        if( numAffinedOam != 0 ) 
        {
           pPrevChunk->pNext     = pChunkList->pChunks;
           pChunkList->pChunks   = pHeadChunk;
           pChunkList->numChunks += numAffinedOam;
           pChunkList->pAffinedChunks = NULL;
        }
    }
#endif // NNS_G2D_OAMEX_USE_OLD_REINDEXOAMCHUNKLIST_
}

//------------------------------------------------------------------------------
// affine 変換された NNSG2dOamChunk を 描画可能なように処理します。
//
// affine 変換された NNSG2dOamChunk は 通常の NNSG2dOamChunk と 別のリストに格納されています。 
// NNSG2dOamChunk は 内部affineProxy配列 の Indexでaffineパラメータを参照しているため、
// 通常の affineパラメータIndex に値を書き換える必要があります。
// 
// 
static NNS_G2D_INLINE void ReindexAffinedOams_( NNSG2dOamManagerInstanceEx* pMan )
{
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_ASSERT( pMan->numAffineBufferUsed != 0 );
    NNS_G2D_ASSERT( IsAffineProxyValid_( pMan ) );
    
    {
       u16 i;
       //
       // すべての NNSG2dOamChunkList について...
       //
       for( i = 0; i < pMan->lengthOfOrderingTbl; i++ )
       {
           ReindexOamChunkList_( pMan, &pMan->pOamOrderingTbl[i] );
       }
    }
}




//------------------------------------------------------------------------------
// 指定された 描画優先度は適当か調査します。
// pMan->lengthOfOrderingTbl は マネージャ初期化時に ユーザが自由に設定する値です。
static NNS_G2D_INLINE BOOL IsPriorityValid_( NNSG2dOamManagerInstanceEx* pMan, u8 priority )
{
    return ( priority < pMan->lengthOfOrderingTbl ) ? TRUE : FALSE;
}

//------------------------------------------------------------------------------
// NNSG2dOamChunkList に AffineIndexを指定して OAMAttribute を登録します
//
// 新しい NNSG2dOamChunk は 一旦 Affine 変換された NNSG2dOamChunk 専用のリストに格納されます。
// NNSG2dOamChunk 専用のリストは、ReindexAffinedOams_() で再Index処理され、
// 通常の NNSG2dOamChunk リストに挿入されます
//
static NNS_G2D_INLINE BOOL EntryNewOamWithAffine_
( 
    NNSG2dOamChunkList*       pOamList, 
    NNSG2dOamChunk*           pOamChunk, 
    u16                       affineIdx, 
    NNSG2dOamExDrawOrder      drawOrderType
)
{
    NNS_G2D_NULL_ASSERT( pOamList );
    
    if( pOamChunk != NULL )
    {
        //
        // Affineを利用しない
        //
        if( affineIdx == NNS_G2D_OAM_AFFINE_IDX_NONE )
        {
           if( drawOrderType == NNSG2D_OAMEX_DRAWORDER_BACKWARD )
           {
               //
               // 通常の OamChunkListに移動する
               //
               AddFront_( pOamList, pOamChunk );
           }else{
               AddBack_( pOamList, pOamChunk );
           }
        }else{
            
            pOamChunk->affineProxyIdx = affineIdx;
            if( drawOrderType == NNSG2D_OAMEX_DRAWORDER_BACKWARD )
            {
               //
               // 一旦 Affine リストにしまう
               // 先頭に加える
               //
               pOamChunk->pNext            = pOamList->pAffinedChunks;
               pOamList->pAffinedChunks    = pOamChunk;
            }else{
               //
               // 一旦 Affine リストにしまう
               // 終端に加える
               //
               pOamChunk->pNext = NULL;
               if( pOamList->pLastAffinedChunk != NULL )
               {
                  pOamList->pLastAffinedChunk->pNext = pOamChunk;
               }else{
                  pOamList->pAffinedChunks = pOamChunk;
               }
               pOamList->pLastAffinedChunk = pOamChunk;
            }
        }
        return TRUE;
    }else{
        return FALSE;
    }
}



//------------------------------------------------------------------------------
// リストのポインタを num 分だけ進めます
static NNS_G2D_INLINE NNSG2dOamChunk* AdvancePointer_( NNSG2dOamChunk* pChunk, u16 num )
{
    NNS_G2D_NULL_ASSERT( pChunk );
    while( num > 0 )
    {
        pChunk = pChunk->pNext;
        num--;
        NNS_G2D_NULL_ASSERT( pChunk );
    }
    
    return pChunk;
}

//------------------------------------------------------------------------------
// OamChunkList を 描画する
// OamChunk は 同一の描画優先度を持つ OamChunk ごとにまとまって OamChunkList を形成します。
//
static NNS_G2D_INLINE u16 DrawOamChunks_
( 
    NNSG2dOamManagerInstanceEx*    pMan, 
    NNSG2dOamChunkList*             pOamList, 
    NNSG2dOamChunk*                 pChunk, 
    u16                             numOam, 
    u16                             capacityOfHW, 
    u16                             numTotalOamDrawn 
)
{
    //
    // すべてのOBJを描画し終わるか、空き容量がなくなるまで
    // 
    while( numOam > 0 && (capacityOfHW - numTotalOamDrawn) > 0 )
    {
        //
        // 値のコピー
        //
        EntryOamToToBaseModule_( pMan, &pChunk->oam, numTotalOamDrawn );
        
        
        pChunk = pChunk->pNext;
        // 
        // リスト終端に到達した場合はリスト先頭から再スタートをする
        //
        if( pChunk == NULL )
        {
            pChunk = pOamList->pChunks;
        }
        
        
        numOam--;
        numTotalOamDrawn++;
    }
    return numTotalOamDrawn;
} 



//------------------------------------------------------------------------------
// AffineProxy を 下位モジュールに反映します。
// （通常、下位モジュール は 反映を HWに伝播します）
static NNS_G2D_INLINE void LoadAffineProxyToBaseModule_( NNSG2dOamManagerInstanceEx* pMan )
{
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_ASSERT( pMan->numAffineBufferUsed != 0 );
    
    {    
       u16        count     = 0x0;
       u16        i         = pMan->lastFrameAffineIdx;
       const u16  numAffine = pMan->numAffineBufferUsed;
       const u16  capacity  = (*pMan->oamEntryFuncs.getAffineCapacity)();
           
       NNSG2dAffineParamProxy*   pAff = NULL;
       //
       // 前フレームの続きから登録するような処理を狙っています
       //
       while( ( count < numAffine ) && 
              ( count < capacity ) )
       {
           if( i >= numAffine )
           {
              i = 0;
           }
           pAff = &pMan->pAffineBuffer[ i ];
           
           // store HW affine index.
           pAff->affineHWIndex 
              = (*pMan->oamEntryFuncs.entryNewAffine)( &pAff->mtxAffine, count );
           
           // pMan->lastFrameAffineIdx = i;
           
           i++;
           count++;    
       }
       
       pMan->lastFrameAffineIdx = i;
    }
}

//------------------------------------------------------------------------------
//
// 描画登録する範囲を計算します
//
// 手順：
//
// 1．まずすべてのリストを描画されないリストとして設定します
// 2．リストを走査し、描画OAMチャンクをカウントしながら、リストに描画のための情報を設定していきます。
// 3．空き容量が無くなった、もしくはすべてのチャンクをの調査が終わったら処理を終了します。
//
static NNS_G2D_INLINE void 
CalcDrawnListArea_( NNSG2dOamManagerInstanceEx* pMan )
{
    NNSG2dOamChunk*       pChunk    = NULL;
    NNSG2dOamChunkList*   pOamList  = NULL;
    u16       numTotalOamDrawn      = 0;
    const u16 capacityOfHW          = ( *(pMan->oamEntryFuncs.getOamCapacity) )();
    
    u16       i;
    
    
    //
    // すべてのチャンクリストについて、描画フラグをリセットします
    //
    for( i = 0; i < pMan->lengthOfOrderingTbl; i++ )
    {
       pMan->pOamOrderingTbl[i].bDrawn = FALSE;
    }
    
    
    //
    // データの走査は、前フレームで終了した次の位置からスタートする
    //
    i = (u16)(pMan->lastRenderedOrderingTblIdx);
    
    //
    // すべてのチャンクリストのチャンクが調査されるまで...
    //
    while( numTotalOamDrawn < pMan->numUsedOam )
    {
       // オーダリングテーブル終端まで達したらテーブル先頭に戻る。
       if( i >= pMan->lengthOfOrderingTbl )
       {
           i = 0;
       }
       
       pOamList   = &pMan->pOamOrderingTbl[i];
       //
       // チャンクリストにチャンクが登録されていれば...
       //
       if( pOamList->numChunks != 0 )
       {
           const u16 currentCapacity = (u16)(capacityOfHW - numTotalOamDrawn);
           
           //
           // 描画されるべきチャンクリストとして設定されます
           // また、最後に描画されたチャンクリスト番号を更新します
           //
           pOamList->bDrawn                 = TRUE;
           pMan->lastRenderedOrderingTblIdx = i;
           
           //
           // リスト中のチャンクがすべて登録が可能なら
           //
           if( pOamList->numChunks <= currentCapacity )
           {
              pOamList->numLastFrameDrawn = 0;
              pOamList->numDrawn          = pOamList->numChunks;   
              
           }else{
              // もしリスト終端まで描画がされたならば、次フレームでは、本チャンクリストの次のリストから
              // 描画を行っていく。
              if( (pOamList->numDrawn + pOamList->numLastFrameDrawn) / pOamList->numChunks > 0 )
              {
                  pMan->lastRenderedOrderingTblIdx = (u16)(i+1);
              }
              
              //
              // 前フレームで描画されたチャンク数から、
              // 今フレームで描画前に与えるオフセット数を計算します
              //
              pOamList->numLastFrameDrawn = (u16)((pOamList->numDrawn + 
                                            pOamList->numLastFrameDrawn ) % pOamList->numChunks);
              
              pOamList->numDrawn          = currentCapacity;// 描けるだけ描く    
              //
              // 終了
              //
              break;
           }
           numTotalOamDrawn += pOamList->numDrawn;
       }
       i++;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dResetOamManExBuffer

  Description:  マネージャをリセットします。
                                
  Arguments:    pMan        :   [OUT] マネージャ実体
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dResetOamManExBuffer( NNSG2dOamManagerInstanceEx* pOam )
{
    NNS_G2D_NULL_ASSERT( pOam );
    pOam->numUsedOam = 0;
    pOam->numAffineBufferUsed = 0;
    
    // reset the ordering table.
    {
        u16 i = 0;
        for( i = 0;i < pOam->lengthOfOrderingTbl; i++ )
        {
            pOam->pOamOrderingTbl[i].numChunks      = 0;
            pOam->pOamOrderingTbl[i].pChunks        = NULL;
            pOam->pOamOrderingTbl[i].pAffinedChunks = NULL;
            pOam->pOamOrderingTbl[i].pLastChunk        = NULL;
            pOam->pOamOrderingTbl[i].pLastAffinedChunk = NULL;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetOamManExInstance

  Description:  マネージャ実体を生成します。


                
                
                
  Arguments:    pOam                    [OUT] 拡張OAMマネージャ 実体 
                pOamOrderingTbl         [IN]  オーダリングテーブルの先頭アドレス 
                lengthOfOrderingTbl     [IN]  オーダリングテーブルの長さ 
                numPooledOam            [IN]  OBJChunkの個数 
                pPooledOam              [IN]  OBJChunk配列へのポインタ 
                lengthAffineBuffer      [IN]  アフィンパラメータバッファ配列の長さ 
                pAffineBuffer           [IN]  アフィンパラメータバッファ配列へのポインタ 

                
  Returns:      初期化の成否(現在のところ、必ず成功する)
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dGetOamManExInstance
( 
    NNSG2dOamManagerInstanceEx*     pOam, 
    NNSG2dOamChunkList*             pOamOrderingTbl, 
    u8                              lengthOfOrderingTbl,
    u16                             numPooledOam,
    NNSG2dOamChunk*                 pPooledOam,
    u16                             lengthAffineBuffer,
    NNSG2dAffineParamProxy*         pAffineBuffer 
)
{
    NNS_G2D_NULL_ASSERT( pOam );
    NNS_G2D_ASSERT( lengthOfOrderingTbl != 0 );
    NNS_G2D_NULL_ASSERT( pPooledOam );
    NNS_G2D_ASSERT( numPooledOam != 0 );
    
    // 描画順タイプ
    // (過去のバージョンとの互換性維持のために、デフォルトは逆順)
    pOam->drawOrderType = NNSG2D_OAMEX_DRAWORDER_BACKWARD;
    
    //
    // チャンクリスト要素のゼロクリア
    //
    MI_CpuClear32( pOamOrderingTbl, lengthOfOrderingTbl * sizeof( NNSG2dOamChunkList ) );
    
    
    pOam->pOamOrderingTbl           = pOamOrderingTbl;
    pOam->lengthOfOrderingTbl       = lengthOfOrderingTbl;
    
    pOam->numPooledOam              = numPooledOam;
    pOam->pPoolOamChunks            = pPooledOam;
    
    pOam->lengthAffineBuffer    = lengthAffineBuffer;
    pOam->pAffineBuffer         = pAffineBuffer;
    
    
    
    //
    // 登録関数関連の初期化
    //
    {
        NNSG2dOamExEntryFunctions*  pFuncs = &pOam->oamEntryFuncs;
        
        pFuncs->getOamCapacity       = NULL;
        pFuncs->getAffineCapacity    = NULL;
        pFuncs->entryNewOam          = NULL;
        pFuncs->entryNewAffine       = NULL;
    }
    
    
    NNS_G2dResetOamManExBuffer( pOam );
    
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNSG2d_SetOamManExDrawOrderType

  Description:  マネージャのOAM描画順の種類を設定します。
                OAM描画順の規定値は NNSG2D_OAMEX_DRAWORDER_BACKWARD になっている
                点にご注意ください。
                これは以前のバージョンとの互換性を維持するための対策です。

  Arguments:    pOam                 :   [OUT] マネージャ実体
                drawOrderType        :   [IN]  描画順の種類
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNSG2d_SetOamManExDrawOrderType
( 
    NNSG2dOamManagerInstanceEx*    pOam, 
    NNSG2dOamExDrawOrder           drawOrderType
)
{
    NNS_G2D_NULL_ASSERT( pOam );
    
    pOam->drawOrderType = drawOrderType;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetOamManExEntryFunctions

  Description:  マネージャに 各種登録関数を設定します。
                マネージャ利用前に必ず呼び出して、登録関数を設定する必要があります。

                
                
                
  Arguments:    pMan        :   [OUT] マネージャ実体
                pSrc        :   [IN]  各種登録関数
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetOamManExEntryFunctions
( 
    NNSG2dOamManagerInstanceEx*        pMan, 
    const NNSG2dOamExEntryFunctions*   pSrc 
)
{
    NNS_G2D_NULL_ASSERT( pMan );
    {
        NNSG2dOamExEntryFunctions*  pDst = &pMan->oamEntryFuncs;    
        
        NNS_G2D_NULL_ASSERT( pSrc );

        if( pSrc->getOamCapacity != NULL )
        {
            pDst->getOamCapacity = pSrc->getOamCapacity;
        }
        
        if( pSrc->getAffineCapacity != NULL )
        {
            pDst->getAffineCapacity = pSrc->getAffineCapacity;
        }
        
        if( pSrc->entryNewOam != NULL )
        {
            pDst->entryNewOam = pSrc->entryNewOam;
        }
        
        if( pSrc->entryNewAffine != NULL )
        {
            pDst->entryNewAffine = pSrc->entryNewAffine;
        }
        
        NNS_G2D_ASSERT( IsOamEntryFuncsValid_( pMan, pDst ) );
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dEntryOamManExOam

  Description:  拡張OAMマネージャにOBJを、描画時の登録順を指定して登録します。
                実際にハードウエアに適用されるときに、指定した順番で適用を行います。

                
                十分な容量がある場合は 登録を行い TRUEを返します
                
                
                
  Arguments:    pMan        :   [OUT] マネージャ実体
                pOam        :   [IN]  OAMAttribute 
                priority    :   [IN]  描画優先度
                affineIdx   :   [IN]  アフィン・インデックス
                
  Returns:      登録の成否
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dEntryOamManExOam
( 
    NNSG2dOamManagerInstanceEx*    pMan, 
    const GXOamAttr*               pOam, 
    u8                             priority 
)
{
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_NULL_ASSERT( pOam );
    NNS_G2D_ASSERT( IsPriorityValid_( pMan, priority ) );
    
    {
        NNSG2dOamChunkList*   pOamList    = &pMan->pOamOrderingTbl[ priority ];
        NNSG2dOamChunk*       pOamChunk   = GetNewOamChunk_( pMan, pOam );
         
        return EntryNewOamWithAffine_( pOamList, 
                                       pOamChunk, 
                                       NNS_G2D_OAM_AFFINE_IDX_NONE, 
                                       pMan->drawOrderType );
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dEntryOamManExOamWithAffineIdx

  Description:  マネージャに AffineIndexを指定して OAMAttribute を登録します
                NNS_G2dEntryOamManExAffine()の返り値を指定してください。
                
                十分な容量がある場合は 登録を行い TRUEを返します
                
                
                
  Arguments:    pMan        :   [OUT] マネージャ実体
                pOam        :   [IN]  OAMAttribute 
                priority    :   [IN]  描画優先度
                affineIdx   :   [IN]  アフィン・インデックス
                
  Returns:      登録の成否
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dEntryOamManExOamWithAffineIdx
( 
    NNSG2dOamManagerInstanceEx*    pMan, 
    const GXOamAttr*               pOam, 
    u8                             priority, 
    u16                            affineIdx 
)
{
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_NULL_ASSERT( pOam );
    // アフィン変換が有効と指定されているOAMアトリビュートか？
    NNS_G2D_ASSERT( pOam->rsMode & 0x1 );
    NNS_G2D_ASSERT( IsPriorityValid_( pMan, priority ) );
    
    {
        NNSG2dOamChunkList*   pOamList    = &pMan->pOamOrderingTbl[ priority ];
        NNSG2dOamChunk*       pOamChunk   = GetNewOamChunk_( pMan, pOam );
         
        return EntryNewOamWithAffine_( pOamList, 
                                       pOamChunk, 
                                       affineIdx, 
                                       pMan->drawOrderType );
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dEntryOamManExAffine

  Description:  マネージャにAffineパラメータを登録します
                マネージャに十分な容量があれば、affine変換行列は
                内部の NNSG2dAffineParamProxy に 格納され、Index が返ります。
                アフィンパラメータ用バッファ配列が設定されていない場合は、アサートに失敗します。
                
                OBJ は マネージャ内部では 
                NNSG2dAffineParamProxy の IndexによってAffineパラメータを参照します。
                
                
                
  Arguments:    pMan:      [OUT] マネージャ実体
                mtx :      [IN]  affine変換行列
  
  Returns:      内部の NNSG2dAffineParamProxy の Index
  
 *---------------------------------------------------------------------------*/
u16 NNS_G2dEntryOamManExAffine
( 
    NNSG2dOamManagerInstanceEx*    pMan, 
    const MtxFx22*                 mtx 
)
{
    NNS_G2D_NULL_ASSERT( pMan ); 
    NNS_G2D_NULL_ASSERT( mtx );
    NNS_G2D_ASSERT( IsAffineProxyValid_( pMan ) );
    
    if( HasEnoughCapacity_( pMan ) )
    {
        NNSG2dAffineParamProxy* pAffineProxy 
           = &pMan->pAffineBuffer[pMan->numAffineBufferUsed];
           
        pAffineProxy->mtxAffine     = *mtx;
        pAffineProxy->affineHWIndex = NNS_G2D_OAMEX_HW_ID_NOT_INIT;
        
        return pMan->numAffineBufferUsed++;
    }   
    
    return NNS_G2D_OAM_AFFINE_IDX_NONE;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dApplyOamManExToBaseModule 

  Description:  マネージャの内容をHWに反映する。
                実際には HWに反映 するわけではなく、下位モジュールに反映をする。
                下位モジュールがHWへその内容の反映を行うまでは、描画に反映されない。
                
                
                
  Arguments:    pMan:      [OUT] マネージャ実体
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dApplyOamManExToBaseModule( NNSG2dOamManagerInstanceEx* pMan )
{
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_NULL_ASSERT( pMan->oamEntryFuncs.getOamCapacity );
    {
        u16       numTotalOamDrawn = 0;
        const u16 capacityOfHW     = ( *(pMan->oamEntryFuncs.getOamCapacity) )();
        
        if( pMan->numUsedOam != 0 )
        {
            //
            // affine変換されたOamの準備をする
            // 
            if( pMan->numAffineBufferUsed != 0 )
            {
                // Affine Proxy の 内容を 下位モジュールに適用し、Affine Index を決定する
                LoadAffineProxyToBaseModule_( pMan );
                // 再Index
                ReindexAffinedOams_( pMan );
            }
            
            //
            // チャンクリストの登録範囲を計算します
            //
            CalcDrawnListArea_( pMan );
            
            //
            // 外部モジュールに対して実際に描画登録を行います
            //
            {
                u16 i = 0;
                NNSG2dOamChunk*       pChunk    = NULL;
                NNSG2dOamChunkList*   pOamList  = NULL;
        
                for( i = 0; i < pMan->lengthOfOrderingTbl; i++ )
                {
                    pOamList  = &pMan->pOamOrderingTbl[i];
                    //
                    // 描画する必要のあるチャンクリストならば...
                    //  
                    if( pOamList->bDrawn )
                    {
                        NNS_G2D_ASSERT( pOamList->numLastFrameDrawn < pOamList->numChunks );
                        
                        pChunk = AdvancePointer_( pOamList->pChunks, pOamList->numLastFrameDrawn );               
                         
                        numTotalOamDrawn = DrawOamChunks_( pMan, 
                                                 pOamList, 
                                                 pChunk, 
                                                 pOamList->numDrawn, 
                                                 capacityOfHW, 
                                                 numTotalOamDrawn );
                    }
                }
            }
        }
        
        //
        // 残り容量はデフォルト値でクリアする
        //
        while( capacityOfHW > numTotalOamDrawn )
        {
            // デフォルト値でクリア
            EntryOamToToBaseModule_( pMan, &defaultOam_, numTotalOamDrawn );
            numTotalOamDrawn++;
        }
    }
}





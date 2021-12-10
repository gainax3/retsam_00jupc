/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g2d
  File:     g2di_RendererMtxState.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.10 $
 *---------------------------------------------------------------------------*/
 
#ifndef NNS_G2DI_RENDERERMTXSTATE_H_
#define NNS_G2DI_RENDERERMTXSTATE_H_

#include <nnsys/g2d/g2d_Renderer.h>

#include "g2d_Internal.h"
#include "g2di_RendererMtxStack.hpp"// For MatrixStack 
#include "g2di_RendererMtxCache.h"

//
// Matrix Cache state Manager
//
// 行列スタックの状態を管理し、
// 行列スタックのマトリクスキャッシュへのロードを処理する内部モジュール。
//
// 行列スタックへの操作をラップして、行列スタックの状態管理を行います。
// 
// レンダラモジュールによって使用されます。
// レンダラモジュールは直接行列キャッシュを操作せずに、
// すべての操作を本モジュールのメソッドを経由して行います。
//
// 行列スタックへの操作を列挙します。
//      A：カレント行列のSR値が変更された
//      B：カレント行列のSR値が行列キャッシュに読み込まれた
//      C：行列スタックがPushされた。
//
// 対応する関数呼び出しは、
//
//      A：NNSi_G2dMCMSetCurrentMtxSRChanged()
//      B：NNSi_G2dMCMStoreCurrentMtxToMtxCache( )
//      C：NNSi_G2dMCMSetMtxStackPushed( u16 newPos, u16 lastPos )
//
// となります。
//
// 対照的にレンダラコアモジュールは本モジュールを一切参照しません。
// レンダラモコアジュールは行列キャッシュモジュールのみを利用します。
//
// 関数命名 NNSi_G2dMCM.....




//------------------------------------------------------------------------------
// モジュール内限定利用 型定義
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// 行列スタック内の行列の状態をあらわす。
typedef enum MCMRndMtxStateType
{
    MCM_MTX_NOT_SR = 0,                        // SR 変換されていない
    MCM_MTX_SR_NOT_CACHELOADED,                // SR 変換されており、マトリクスキャッシュへのロードがが完了していない
    MCM_MTX_SR_NOT_CACHELOADED_STACKCHANGED,   // SR 変換されており、マトリクスキャッシュへのロードがが完了していない
                                               // スタックが操作されている
    MCM_MTX_SR_CACHELOADED                     // SR 変換されており、マトリクスキャッシュへのロードが完了している

}MCMRndMtxStateType;

//------------------------------------------------------------------------------
// 行列スタックの状態をあらわします。
// マトリクススタック中の行列と同じサイズのスタック構造をとります。
//
// 本データを利用して、マトリクスキャッシュへのロード処理を制御しています。
//
// 行列そのもの以外の情報を格納しています。
typedef struct MCMMtxState
{
    u16                      mtxCacheIdx; // 行列キャッシュ番号
    u16                      groupID;     // 同一のマトリクスキャッシュを参照するグループID
    u16                      stateType;   // MCMRndMtxStateType
    u16                      pad16;
    
}MCMMtxState;

//------------------------------------------------------------------------------
NNS_G2D_INLINE u16 GetMtxStateMtxCacheIdx_( const MCMMtxState* pMtxState )
{
    return pMtxState->mtxCacheIdx;
}

NNS_G2D_INLINE void SetMtxStateMtxCacheIdx_( MCMMtxState* pMtxState, u16 cacheIdx )
{
    pMtxState->mtxCacheIdx = cacheIdx;
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE u16 GetMtxStateGroupID_( const MCMMtxState* pMtxState )
{
    return pMtxState->groupID;
}

NNS_G2D_INLINE void SetMtxStateGroupID_( MCMMtxState* pMtxState, u16 groupID )
{
    pMtxState->groupID = groupID;
}



//------------------------------------------------------------------------------
// 行列スタックの状態をあらわすデータ
// 行列スタックと同じサイズのスタックです。
// カレント位置が、行列スタックと同調します。
static MCMMtxState           mtxStateStack_[G2Di_NUM_MTX_CACHE];
static u16                   groupID_ = 0;



//------------------------------------------------------------------------------
// 内部限定公開の関数
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// カレント行列の状態構造体を取得します
NNS_G2D_INLINE MCMMtxState* GetCuurentMtxState_()
{
    return &mtxStateStack_[NNSi_G2dGetMtxStackPos()];
}

//------------------------------------------------------------------------------
// スタックの親階層をたどり
// 親階層の groupID が同一ならば、
// それをキャシュロード済みにセットする
// (groupID が同一ならば、同一の行列キャッシュを参照します。)
NNS_G2D_INLINE void SetParentMtxStateLoaded_( u16 mtxCacheIdx, u16 groupID )
{
    int i;
    const u16 currntStackPos    = NNSi_G2dGetMtxStackPos();
    
    for( i = currntStackPos; i >= 0; i-- )
    {
        
        if( GetMtxStateGroupID_( &mtxStateStack_[i] ) != groupID )
        {
            break;
        }else{
            // キャシュロード済み に セット
            mtxStateStack_[i].stateType   = MCM_MTX_SR_CACHELOADED;
            SetMtxStateMtxCacheIdx_( &mtxStateStack_[i], mtxCacheIdx );
        }
        
        NNSI_G2D_DEBUGMSG1( "Set Loaded => mtxStateStack_[%d].mtxCacheIdx = %d\n", 
                             i, 
                             GetMtxStateMtxCacheIdx_( &mtxStateStack_[i] ) );
    }
}

//------------------------------------------------------------------------------
// 新しい、グループIDを取得します
NNS_G2D_INLINE u16 GetNewGroupID_()
{
    return groupID_++;
}

//------------------------------------------------------------------------------
// 新しい、グループIDを取得します
NNS_G2D_INLINE void InitGroupID_()
{
    groupID_ = 0;
}

//------------------------------------------------------------------------------
// 外部公開関数
//------------------------------------------------------------------------------
NNS_G2D_INLINE void NNSi_G2dMCMInitMtxCache()
{
    NNSi_G2dInitRndMtxStack();
    NNSi_RMCInitMtxCache();
    
    InitGroupID_();
    
    //
    // マトリクスステートの初期化
    //
    MI_CpuClearFast( mtxStateStack_, sizeof( mtxStateStack_ ) );
}

//------------------------------------------------------------------------------
// カレント NNSG2dRndCore2DMtxCache へのポインタを取得します
//
NNS_G2D_INLINE NNSG2dRndCore2DMtxCache*   NNSi_G2dMCMGetCurrentMtxCache()
{
    return NNSi_RMCGetMtxCacheByIdx( GetMtxStateMtxCacheIdx_( GetCuurentMtxState_() ) );
}

//------------------------------------------------------------------------------
// MtxChache の 内容を消去します
//
// 2D Graphics Engine レンタリング専用の処理です。
//
NNS_G2D_INLINE void NNSi_G2dMCMCleanupMtxCache()
{
    //
    // マトリクススタックのリセット
    //
    NNSi_G2dInitRndMtxStack();
    //
    // マトリクスキャシュのリセット
    //
    NNSi_RMCResetMtxCache();

    InitGroupID_();
    
    //
    // マトリクスステートの初期化
    //
    MI_CpuClearFast( mtxStateStack_, sizeof( mtxStateStack_ ) );
}

//------------------------------------------------------------------------------
// 現在の行列がキャッシュに読み込む必要がある状態か取得します。
NNS_G2D_INLINE BOOL NNSi_G2dMCMShouldCurrentMtxBeLoadedToMtxCache( )
{
    MCMMtxState*     pCurrMtxState = GetCuurentMtxState_();
    //
    // SR 変換されており
    // キャッシュに読み込まれていないならば...
    //       
    return (BOOL)( pCurrMtxState->stateType == MCM_MTX_SR_NOT_CACHELOADED ||
                   pCurrMtxState->stateType == MCM_MTX_SR_NOT_CACHELOADED_STACKCHANGED );
}

//------------------------------------------------------------------------------
// カレント行列の状態をSR変更が行われた状態に設定します。
NNS_G2D_INLINE void NNSi_G2dMCMSetCurrentMtxSRChanged()
{
    MCMMtxState*     pCurrMtxState = GetCuurentMtxState_();
    
    //
    // カレント行列の状態によって、処理が変化します。
    //
    switch( pCurrMtxState->stateType )
    {
    case MCM_MTX_SR_NOT_CACHELOADED:
        //
        // ロード前の状態ならば、カレント行列状態の更新処理等は必要がない。
        // (つまり、（描画直前のタイミングで実行される）ロード処理前であれば、
        // 何度SR変換を行ってもマトリクスキャッシュは消費しない。)
        //
        return;
    case MCM_MTX_NOT_SR:
    case MCM_MTX_SR_NOT_CACHELOADED_STACKCHANGED:
    case MCM_MTX_SR_CACHELOADED:
        // 行列スタック：カレント行列の状態を更新する
        {
            // 新たな 行列キャッシュ を使用する必要がある
            // 別グループとしてグループIDを設定します。
            SetMtxStateGroupID_( pCurrMtxState, GetNewGroupID_() );
            
            //
            // SRが変更されているが、キャッシュへのロードがすんでいない状態
            //
            pCurrMtxState->stateType = MCM_MTX_SR_NOT_CACHELOADED;
            
            NNSI_G2D_DEBUGMSG1( "currentMtxCachePos change to %d at %d\n", 
                                 pCurrMtxState->mtxCacheIdx,
                                 NNSi_G2dGetMtxStackPos() );
        }
        break;
    }
}


//------------------------------------------------------------------------------
// カレント行列Push操作に対応する処理を行います。
NNS_G2D_INLINE void NNSi_G2dMCMSetMtxStackPushed( u16 newPos, u16 lastPos )
{
    
    mtxStateStack_[newPos] = mtxStateStack_[lastPos];
    //
    // stateType が MCM_MTX_SR_NOT_CACHELOADED だった場合は
    // MCM_MTX_SR_NOT_CACHELOADED_STACKCHANGED へと状態を変更します。
    // 
    // （ その結果、新たにSR変換が起きNNSi_G2dMCMSetCurrentMtxSRChanged()
    //    が実行される際の振る舞いが変化します。)
    //    
    if( mtxStateStack_[lastPos].stateType == MCM_MTX_SR_NOT_CACHELOADED )
    {
        mtxStateStack_[newPos].stateType = MCM_MTX_SR_NOT_CACHELOADED_STACKCHANGED;
    }else{
        mtxStateStack_[newPos].stateType   = mtxStateStack_[lastPos].stateType;
    }
}

//------------------------------------------------------------------------------
// カレント行列を指定した MtxChacheに格納します。
// 
// flip 機能 を 使用した OBJ を affine変換する必要がある場合は 
// 専用の 行列を 生成し、格納します。
//
// 2D Graphics Engine レンタリング専用の処理です。
//
static void NNSi_G2dMCMStoreCurrentMtxToMtxCache( )
{   
    // 必要が無ければ、何もしない  
    if( NNSi_G2dMCMShouldCurrentMtxBeLoadedToMtxCache() )
    {
        MCMMtxState* pCurrentState = GetCuurentMtxState_();            
        const u16 mtxCacheIdx = NNSi_RMCUseNewMtxCache();
        const u16 groupID     = GetMtxStateGroupID_( pCurrentState );
           
        NNS_G2D_MINMAX_ASSERT( mtxCacheIdx, 0, G2Di_NUM_MTX_CACHE - 1 );
           
        //
        // キャシュインデックスの決定
        //
        SetMtxStateMtxCacheIdx_( pCurrentState, mtxCacheIdx );
           
        ////
        //// 必要があれば…
        ////
        //if( mtxCacheIdx != NNS_G2D_OAM_AFFINE_IDX_NONE )
        {
           //
           // キャッシュを初期化する
           //
           NNS_G2dInitRndCore2DMtxCache( NNSi_RMCGetMtxCacheByIdx( mtxCacheIdx ) );
                  
           //
           // キャッシュに行列をコピーする
           //    
           NNSi_G2dGetMtxRS( NNSi_G2dGetCurrentMtxFor2DHW(), 
                                &NNSi_G2dMCMGetCurrentMtxCache()->m22 );
               
           NNSI_G2D_DEBUGMSG1( "Mtx No %d is cache loaded To %d = %d\n", 
                                        mtxStateStackPos_, 
                                        pCurrMtxState->mtxCacheIdx );
        }
        //
        // SR変換を共有する 親階層の状態をロード済みに更新する
        //
        SetParentMtxStateLoaded_( mtxCacheIdx, groupID );
    }
}


#endif // NNS_G2DI_RENDERERMTXSTATE_H_
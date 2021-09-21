/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g2d
  File:     g2d_Animation.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.30 $
 *---------------------------------------------------------------------------*/

#include <nnsys/g2d/g2d_Animation.h>
#include <stdlib.h>


//------------------------------------------------------------------------------
// private funcs


//------------------------------------------------------------------------------
static NNS_G2D_INLINE const NNSG2dAnimFrame* GetFrameBegin_( const NNSG2dAnimSequence* pSequence )
{    
    NNS_G2D_NULL_ASSERT( pSequence );
    return pSequence->pAnmFrameArray;
}
//------------------------------------------------------------------------------
static NNS_G2D_INLINE const NNSG2dAnimFrame* GetFrameEnd_( const NNSG2dAnimSequence* pSequence )
{    
    NNS_G2D_NULL_ASSERT( pSequence );
    return pSequence->pAnmFrameArray + ( pSequence->numFrames );
}
//------------------------------------------------------------------------------
static NNS_G2D_INLINE const NNSG2dAnimFrame* GetFrameLoopBegin_( const NNSG2dAnimSequence* pSequence )
{
    NNS_G2D_NULL_ASSERT( pSequence );
    
    return pSequence->pAnmFrameArray + pSequence->loopStartFrameIdx;
}
//------------------------------------------------------------------------------
// TODO: NOT EFFICEANT !
static NNS_G2D_INLINE u16 GetCurrentFrameIdx_( const NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    return (u16)(( (u32)pAnimCtrl->pCurrent - (u32)pAnimCtrl->pAnimSequence->pAnmFrameArray ) 
                    / sizeof( NNSG2dAnimFrameData ));
}

//------------------------------------------------------------------------------
static NNS_G2D_INLINE NNSG2dAnimationPlayMode GetAnimationPlayMode_
( 
    const NNSG2dAnimController* pAnimCtrl 
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_NULL_ASSERT( pAnimCtrl->pAnimSequence );
    
    //
    // もし、pAnimCtrl->overriddenPlayModeが設定されていれば
    // 再生モードはオーバーライドされる。
    // 
    if( pAnimCtrl->overriddenPlayMode != NNS_G2D_ANIMATIONPLAYMODE_INVALID )
    {
        return pAnimCtrl->overriddenPlayMode;
    }else{
        return pAnimCtrl->pAnimSequence->playMode;
    }
}

//------------------------------------------------------------------------------
static NNS_G2D_INLINE BOOL IsLoopAnimSequence_( const NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_NULL_ASSERT( pAnimCtrl->pAnimSequence );
    
    {
    const NNSG2dAnimationPlayMode   playMode = GetAnimationPlayMode_( pAnimCtrl );
    

    return ( playMode == NNS_G2D_ANIMATIONPLAYMODE_FORWARD_LOOP || 
             playMode == NNS_G2D_ANIMATIONPLAYMODE_REVERSE_LOOP ) ? TRUE : FALSE;
    }
}

//------------------------------------------------------------------------------
static NNS_G2D_INLINE BOOL IsReversePlayAnim_( const NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_NULL_ASSERT( pAnimCtrl->pAnimSequence );
    
    {
    const NNSG2dAnimationPlayMode   playMode = GetAnimationPlayMode_( pAnimCtrl );
    

    return ( playMode == NNS_G2D_ANIMATIONPLAYMODE_REVERSE || 
             playMode == NNS_G2D_ANIMATIONPLAYMODE_REVERSE_LOOP ) ? TRUE : FALSE;
    }
}

//------------------------------------------------------------------------------
static NNS_G2D_INLINE BOOL IsAnimCtrlMovingForward_( const NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );    
    return ( pAnimCtrl->speed > 0 )^(pAnimCtrl->bReverse) ? TRUE : FALSE;
}




//------------------------------------------------------------------------------
static NNS_G2D_INLINE BOOL ShouldAnmCtrlMoveNext_( NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_NULL_ASSERT( pAnimCtrl->pCurrent );
    
    if( pAnimCtrl->bActive && 
        (pAnimCtrl->currentTime >= FX32_ONE * (int)pAnimCtrl->pCurrent->frames) )
    {
        return TRUE;
    }
    return FALSE;
}


//------------------------------------------------------------------------------
static NNS_G2D_INLINE  void CallbackFuncHandling_( const NNSG2dCallBackFunctor*  pFunctor, u16 currentFrameIdx )
{
    NNS_G2D_NULL_ASSERT( pFunctor );
    
    switch( pFunctor->type )
    {
    // call at the specified frame.
    case NNS_G2D_ANMCALLBACKTYPE_SPEC_FRM:
        if( currentFrameIdx == pFunctor->frameIdx )
        {
            (*pFunctor->pFunc)( pFunctor->param, currentFrameIdx );
        }   
        break;
    // call at every frames.
    case NNS_G2D_ANMCALLBACKTYPE_EVER_FRM:        
        (*pFunctor->pFunc)( pFunctor->param, currentFrameIdx );
        break;
    }
}

//------------------------------------------------------------------------------
// 外部公開のためのラッパー
void NNSi_G2dCallbackFuncHandling( const NNSG2dCallBackFunctor*  pFunctor, u16 currentFrameIdx )
{
    CallbackFuncHandling_( pFunctor, currentFrameIdx );
}

//------------------------------------------------------------------------------
static NNS_G2D_INLINE BOOL IsReachStartEdge_( const NNSG2dAnimController* pAnimCtrl, const NNSG2dAnimFrame* pFrame )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_NULL_ASSERT( pFrame );
    
    return ( pFrame <= ( GetFrameLoopBegin_( pAnimCtrl->pAnimSequence ) - 1 ) ) ? TRUE : FALSE;
}

//------------------------------------------------------------------------------
static NNS_G2D_INLINE BOOL IsReachEdge_( const NNSG2dAnimController* pAnimCtrl, const NNSG2dAnimFrame* pFrame )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_NULL_ASSERT( pFrame );
    
    
    if( IsAnimCtrlMovingForward_( pAnimCtrl ) )
    {
        // tail edge of the animation sequence.
        return ( pFrame >= GetFrameEnd_( pAnimCtrl->pAnimSequence ) ) ? TRUE : FALSE;
    }else{
        // head edge of the animation sequence.
        return IsReachStartEdge_( pAnimCtrl, pFrame );
    }
}

//------------------------------------------------------------------------------
static NNS_G2D_INLINE void MoveNext_( NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    
    if( IsAnimCtrlMovingForward_( pAnimCtrl ) )
    {
        pAnimCtrl->pCurrent++;
        
    }else{
        pAnimCtrl->pCurrent--;    
    }
}

//------------------------------------------------------------------------------
static NNS_G2D_INLINE void SequenceEdgeHandleCommon_( NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    
    //
    // NNS_G2D_ANMCALLBACKTYPE_LAST_FRM タイプの
    // コールバック呼び出しの処理
    // 
    if( pAnimCtrl->callbackFunctor.type == NNS_G2D_ANMCALLBACKTYPE_LAST_FRM )
    {
        NNS_G2D_NULL_ASSERT( pAnimCtrl->callbackFunctor.pFunc );
        (*pAnimCtrl->callbackFunctor.pFunc)( pAnimCtrl->callbackFunctor.param, pAnimCtrl->currentTime );
    }
    
    // 
    // リセット処理
    //
    if( !IsLoopAnimSequence_( pAnimCtrl ) )
    {
        // ループ再生で無ければ、更新をストップする
        NNS_G2dStopAnimCtrl( pAnimCtrl );
    }else{
        NNS_G2dResetAnimCtrlState( pAnimCtrl );
    }
}

//------------------------------------------------------------------------------
static NNS_G2D_INLINE void SequenceEdgeHandleReverse_( NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    
    // 再生方向を反転する
    pAnimCtrl->bReverse = pAnimCtrl->bReverse^TRUE;
    
    //
    // アニメーションスタートフレームか？
    // 
    if( IsReachStartEdge_( pAnimCtrl, pAnimCtrl->pCurrent ) )
    {
        SequenceEdgeHandleCommon_( pAnimCtrl );
    }
}

//------------------------------------------------------------------------------
static NNS_G2D_INLINE void SequenceEdgeHandleNormal_( NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    SequenceEdgeHandleCommon_( pAnimCtrl );
}

//------------------------------------------------------------------------------
static NNS_G2D_INLINE void ValidateAnimFrame_( NNSG2dAnimController* pAnimCtrl, const NNSG2dAnimFrame** pFrame )
{
    if( *pFrame > GetFrameEnd_( pAnimCtrl->pAnimSequence ) - 1 )
    {
        *pFrame = GetFrameEnd_( pAnimCtrl->pAnimSequence ) - 1;
    }else if( *pFrame < GetFrameBegin_( pAnimCtrl->pAnimSequence ) ){
        *pFrame = GetFrameBegin_( pAnimCtrl->pAnimSequence );
    }
}

//------------------------------------------------------------------------------
static void SequenceEdgeHandle_( NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    
    if( IsReversePlayAnim_( pAnimCtrl ) )
    {
        SequenceEdgeHandleReverse_( pAnimCtrl );
    }else{
        SequenceEdgeHandleNormal_( pAnimCtrl );
    }
    
    ValidateAnimFrame_( pAnimCtrl, &pAnimCtrl->pCurrent );
}

//------------------------------------------------------------------------------
// SetAnimCtrlCurrentFrameの実装です
// NNS_G2dSetAnimCtrlCurrentFrame()
// NNS_G2dSetAnimCtrlCurrentFrameNoResetCurrentTime()
// から呼ばれます。
//
static BOOL SetAnimCtrlCurrentFrameImpl_
( 
    NNSG2dAnimController*     pAnimCtrl, 
    u16                       index
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_NULL_ASSERT( pAnimCtrl->pAnimSequence );
    
    
    
    // 有効なIndexか？
    if( index < pAnimCtrl->pAnimSequence->numFrames )
    {
        // set
        pAnimCtrl->pCurrent = &pAnimCtrl->pAnimSequence->pAnmFrameArray[index];
        if( pAnimCtrl->pCurrent->frames != 0 )
        {
           pAnimCtrl->pActiveCurrent = pAnimCtrl->pCurrent;
        }
        return TRUE;   
    }
    return FALSE;
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetAnimCtrlCurrentElement
                NNS_G2dGetAnimCtrlNextElement

  Description:  現在（次の）のアニメ結果を取得します。
                ユーザは 本関数の結果を NNSG2dAnimController 実体に関連付けられている、
                アニメーションデータの種類に応じて適切な型にキャスト後利用します。
                
  Arguments:    pAnimCtrl:           [IN]  NNSG2dAnimController 実体
                
  Returns:      現在のアニメ結果（を指すポインタ）
  
 *---------------------------------------------------------------------------*/
void* NNS_G2dGetAnimCtrlCurrentElement( NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_NULL_ASSERT( pAnimCtrl->pActiveCurrent );
    NNS_G2D_NULL_ASSERT( (void*)pAnimCtrl->pActiveCurrent->pContent );
    
    // 有効な表示対象を返すようにします。(pCurrentは表示フレーム数がゼロの場合があります)
    return (void*)pAnimCtrl->pActiveCurrent->pContent;
}
//------------------------------------------------------------------------------
void* NNS_G2dGetAnimCtrlNextElement( NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    {
        const NNSG2dAnimFrame*      pNext = NULL;
        
        if( IsAnimCtrlMovingForward_( pAnimCtrl ) )
        {
            pNext = pAnimCtrl->pCurrent + 1;
        }else{
            pNext = pAnimCtrl->pCurrent - 1;
        }
        
        ValidateAnimFrame_( pAnimCtrl, &pNext );
        NNS_G2D_NULL_ASSERT( pNext->pContent );
        
        return (void*)pNext->pContent;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetAnimCtrlNormalizedTime

  Description:  現在表示中のアニメーションフレームの表示済み時間を
                0.0 を 表示開始時 1.0 を を 表示完了時として fx32 型で取得します。 
                
  Arguments:    pAnimCtrl:          [IN]  NNSG2dAnimController 実体
                
  Returns:      アニメーションフレームの表示時間
  
 *---------------------------------------------------------------------------*/
fx32 NNS_G2dGetAnimCtrlNormalizedTime(  NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_NULL_ASSERT( pAnimCtrl->pCurrent );
    
    return FX_Div( pAnimCtrl->currentTime, FX32_ONE * pAnimCtrl->pCurrent->frames );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTickAnimCtrl

  Description:  NNSG2dAnimController の 時間を進めます
                
  Arguments:    pAnimCtrl:          [OUT]  NNSG2dAnimController 実体
                frames:             [IN]   進める時間（フレーム）
                
                
  Returns:      再生トラックが変更になったか
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dTickAnimCtrl( NNSG2dAnimController* pAnimCtrl, fx32 frames ) 
{
    BOOL    bChangeFrame = FALSE;
    
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_NULL_ASSERT( pAnimCtrl->pCurrent );
    NNS_G2D_ASSERTMSG( frames >=  0, "frames must be Greater than zero");
    
    
    if( pAnimCtrl->bActive != TRUE )
    {
        return FALSE;
    } 
       
    //
    // update current time.
    //
    pAnimCtrl->currentTime += abs( FX_Mul( pAnimCtrl->speed, frames ) );
    
    
    //    
    // do actual works for changing current animation frame.
    //
    while( ShouldAnmCtrlMoveNext_( pAnimCtrl ) )
    {
        bChangeFrame = TRUE; // 変更があった
        
        pAnimCtrl->currentTime -= FX32_ONE * (int)pAnimCtrl->pCurrent->frames;    
        MoveNext_( pAnimCtrl );
        
        // if we reach the edge of animation, we have to reset status.
        if( IsReachEdge_( pAnimCtrl, pAnimCtrl->pCurrent ) )
        {
            SequenceEdgeHandle_( pAnimCtrl );
        }

        // 表示フレームがゼロで無ければ、
        // 有効な表示フレームとします。
        if( pAnimCtrl->pCurrent->frames != 0 )
        {
           pAnimCtrl->pActiveCurrent = pAnimCtrl->pCurrent;
        }
        
        // call the callback function.
        if( pAnimCtrl->callbackFunctor.type != NNS_G2D_ANMCALLBACKTYPE_NONE )
        {
            CallbackFuncHandling_( &pAnimCtrl->callbackFunctor, GetCurrentFrameIdx_( pAnimCtrl ) );
        }    
    }    
    return bChangeFrame;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetAnimCtrlCurrentFrame

  Description:  アニメーションコントローラの再生アニメーションフレームを設定します。
                
  Arguments:    pAnimCtrl:        [OUT]   NNSG2dAnimController 実体
                index    :        [IN]    アニメーションシーケンス番号
                        
  Returns:      変更が成功した場合はTRUE
  
 *---------------------------------------------------------------------------*/
BOOL           NNS_G2dSetAnimCtrlCurrentFrame
( 
    NNSG2dAnimController*     pAnimCtrl, 
    u16                       index
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_NULL_ASSERT( pAnimCtrl->pAnimSequence );
    {
        const BOOL result = SetAnimCtrlCurrentFrameImpl_( pAnimCtrl, index );
        
        if( result )
        {
            // 現在のアニメフレーム表示時間をリセットする
            pAnimCtrl->currentTime   = 0;
        }
        return result;
    }       
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetAnimCtrlCurrentFrameNoResetCurrentTime

  Description:  アニメーションコントローラの再生アニメーションフレームを設定します。
                フレーム変更時にアニメーションコントローラ内部の現在のフレーム表示
                時間カウンタをリセットしません。
                
                そのため、1アニメーションフレームの表示時間より短い時間でアニメーシ
                ョンフレームを切り替えるような場合でも、アニメーションシーケンスを
                進行させることが可能です。

                
  Arguments:    pAnimCtrl:           [OUT] NNSG2dAnimController 実体
                index    :           [IN]  アニメーションシーケンス番号
                        
  Returns:      変更が成功した場合はTRUE
  
 *---------------------------------------------------------------------------*/
BOOL           NNS_G2dSetAnimCtrlCurrentFrameNoResetCurrentTime
( 
    NNSG2dAnimController*     pAnimCtrl, 
    u16                       index
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_NULL_ASSERT( pAnimCtrl->pAnimSequence );
    
    //
    // 現在のアニメフレーム表示時間をリセットしない
    //
    return SetAnimCtrlCurrentFrameImpl_( pAnimCtrl, index );
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetAnimCtrlCurrentFrame

  Description:  NNSG2dAnimController 現在のアニメーションフレーム番号を取得します。
                
  Arguments:    pAnimCtrl:          [OUT] NNSG2dAnimController 実体
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
u16 NNS_G2dGetAnimCtrlCurrentFrame
(
    const NNSG2dAnimController*     pAnimCtrl
)
{
    return GetCurrentFrameIdx_( pAnimCtrl );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitAnimCtrl

  Description:  NNSG2dAnimController を初期化します
                
  Arguments:    pAnimCtrl:          [OUT] NNSG2dAnimController 実体
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dInitAnimCtrl( NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    
    NNS_G2dInitAnimCallBackFunctor( &pAnimCtrl->callbackFunctor );
    
    pAnimCtrl->pCurrent              = NULL;
    pAnimCtrl->pActiveCurrent        = NULL;
    
    pAnimCtrl->bReverse              = FALSE;
    pAnimCtrl->bActive               = TRUE;
    
    pAnimCtrl->currentTime           = 0;
    pAnimCtrl->speed                 = FX32_ONE;
    
    pAnimCtrl->overriddenPlayMode    = NNS_G2D_ANIMATIONPLAYMODE_INVALID;
    pAnimCtrl->pAnimSequence         = NULL;
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitAnimCtrlCallBackFunctor

  Description:  NNSG2dAnimController 中の コールバック を初期化します
                
  Arguments:    pAnimCtrl:          [OUT] NNSG2dAnimController 実体
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dInitAnimCtrlCallBackFunctor( NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    
    NNS_G2dInitAnimCallBackFunctor( &pAnimCtrl->callbackFunctor );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitAnimCallBackFunctor

  Description:  コールバックファンクタ を初期化します
                
  Arguments:    pCallBack:          [OUT] NNSG2dCallBackFunctor 実体
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dInitAnimCallBackFunctor( NNSG2dCallBackFunctor* pCallBack )
{
    NNS_G2D_NULL_ASSERT( pCallBack );
    
    pCallBack->type     = NNS_G2D_ANMCALLBACKTYPE_NONE;
    pCallBack->param    = 0x0;
    pCallBack->pFunc    = NULL;  
    pCallBack->frameIdx = 0;
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dResetAnimCtrlState

  Description:  NNSG2dAnimController の状態をリセットします
                
  Arguments:    pAnimCtrl:          [OUT] NNSG2dAnimController 実体
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dResetAnimCtrlState( NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    
    if( IsAnimCtrlMovingForward_( pAnimCtrl ) )
    {
        pAnimCtrl->pCurrent      = GetFrameLoopBegin_( pAnimCtrl->pAnimSequence );
    }else{
        pAnimCtrl->pCurrent      = GetFrameEnd_( pAnimCtrl->pAnimSequence ) - 1;
    }
    // 表示フレームがゼロフレームのみから構成されるシーケンスの場合
    // もNULLに設定されることはありません。
    pAnimCtrl->pActiveCurrent = pAnimCtrl->pCurrent;
    
    //
    // currentTime をリセットします
    //
    pAnimCtrl->currentTime   = 0;
    
    //
    // ゼロフレームカウンタを進めます
    // 表示フレーム長ゼロのアニメーションフレームが先頭に存在する
    // アニメーションシーケンスに対応するための処理です。
    //
    (void)NNS_G2dTickAnimCtrl( pAnimCtrl, 0 );
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dBindAnimCtrl

  Description:  NNSG2dAnimController 実体 と アニメーションデータを関連付けます
                
  Arguments:    pAnimCtrl:           [OUT]  NNSG2dAnimController 実体
                pAnimSequence:       [IN]   アニメーションデータ
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dBindAnimCtrl( NNSG2dAnimController* pAnimCtrl, const NNSG2dAnimSequence* pAnimSequence )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_NULL_ASSERT( pAnimSequence );
    
    pAnimCtrl->pAnimSequence = pAnimSequence;
    
    NNS_G2dResetAnimCtrlState( pAnimCtrl );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetAnimCtrlCallBackFunctor

  Description:  コールバックを設定します。
                指定フレーム呼び出しタイプのコールバック設定は
                NNS_G2dSetAnimCtrlCallBackFunctorAtAnimFrame()を利用してください
                
  Arguments:    pAnimCtrl:         [OUT] NNSG2dAnimController 実体
                type:              [IN]  コールバックの種類
                param:             [IN]  ユーザ定義情報
                pFunc:             [IN]  コールバック関数ポインタ
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetAnimCtrlCallBackFunctor
( 
    NNSG2dAnimController*   pAnimCtrl, 
    NNSG2dAnmCallbackType   type, 
    u32                     param, 
    NNSG2dAnmCallBackPtr    pFunc 
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_NULL_ASSERT( (void*)pFunc );
    NNS_G2D_MINMAX_ASSERT( type, NNS_G2D_ANMCALLBACKTYPE_NONE, AnmCallbackType_MAX );
    NNS_G2D_ASSERTMSG( type != NNS_G2D_ANMCALLBACKTYPE_SPEC_FRM, 
        "Use NNS_G2dSetAnimCtrlCallBackFunctorAtAnimFrame() instead." );
        
    pAnimCtrl->callbackFunctor.pFunc     = pFunc;
    pAnimCtrl->callbackFunctor.param     = param;
    pAnimCtrl->callbackFunctor.type      = type;
    pAnimCtrl->callbackFunctor.frameIdx  = 0; // meanless for these kind of type.
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetAnimCtrlCallBackFunctorAtAnimFrame

  Description:  コールバックを設定します。
                指定フレーム呼び出しタイプのコールバック設定は
                NNS_G2dSetAnimCtrlCallBackFunctorAtAnimFrame()を利用してください
                
  Arguments:    pAnimCtrl:         [OUT]    NNSG2dAnimController 実体
                param:             [IN]     ユーザ定義情報
                pFunc:             [IN]     コールバック関数ポインタ
                frameIdx:          [IN]     コールバックを呼び出すアニメーションフレーム番号
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetAnimCtrlCallBackFunctorAtAnimFrame
( 
    NNSG2dAnimController*   pAnimCtrl, 
    u32                     param, 
    NNSG2dAnmCallBackPtr    pFunc, 
    u16                     frameIdx 
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_NULL_ASSERT( (void*)pFunc );
    
    pAnimCtrl->callbackFunctor.type  = NNS_G2D_ANMCALLBACKTYPE_SPEC_FRM;
    
    pAnimCtrl->callbackFunctor.pFunc     = pFunc;
    pAnimCtrl->callbackFunctor.param     = param;
    pAnimCtrl->callbackFunctor.frameIdx  = frameIdx;
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dIsAnimCtrlLoopAnim

  Description:  アニメーションコントローラがループアニメーションを再生しているか
                調査します。
                
                [ライブラリ内部公開関数]
               
  Arguments:    pAnimCtrl:          [IN] NNSG2dAnimController 実体
                
  Returns:      ループアニメーションを再生している場合はTRUEを返します。
  
 *---------------------------------------------------------------------------*/
BOOL NNSi_G2dIsAnimCtrlLoopAnim( const NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_NULL_ASSERT( pAnimCtrl->pAnimSequence );
    
    return IsLoopAnimSequence_( pAnimCtrl );
}

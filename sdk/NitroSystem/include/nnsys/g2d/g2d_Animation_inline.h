/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d
  File:     g2d_Animation_inline.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.8 $
 *---------------------------------------------------------------------------*/
 
#ifndef NNS_G2D_ANIMATION_INLINE_H_
#define NNS_G2D_ANIMATION_INLINE_H_

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// inline functions 
//------------------------------------------------------------------------------



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetAnimCtrlType

  Description:  アニメーションコントローラ の アニメ種類を取得します。
                
  Arguments:    pAnimCtrl:            アニメーションコントローラ 実体
                
                
                
  Returns:      コントローラの種類
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE NNSG2dAnimationType NNS_G2dGetAnimCtrlType
( 
    const NNSG2dAnimController*  pAnimCtrl 
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    if( pAnimCtrl->pAnimSequence != NULL )
    {
        return NNS_G2dGetAnimSequenceAnimType( pAnimCtrl->pAnimSequence );
    }else{
        return NNS_G2D_ANIMATIONTYPE_MAX;
    }   
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetAnimCtrlSpeed

  Description:  アニメーションコントローラ の アニメスピードを設定します
                
  Arguments:    pAnimCtrl   :            アニメーションコントローラ 実体
                speed       :           スピード
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dSetAnimCtrlSpeed
( 
    NNSG2dAnimController*       pAnimCtrl, 
    fx32                        speed 
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    pAnimCtrl->speed = speed;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetAnimCtrlSpeed

  Description:  アニメーションコントローラ の アニメスピードを取得します
                
  Arguments:    pAnimCtrl:            アニメーションコントローラ 実体
                
                
                
  Returns:      アニメスピード
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE fx32 NNS_G2dGetAnimCtrlSpeed
( 
    const NNSG2dAnimController*       pAnimCtrl
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    return pAnimCtrl->speed;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dStartAnimCtrl

  Description:  アニメーションコントローラ の アニメ再生をスタートします。
                
  Arguments:    pAnimCtrl:            アニメーションコントローラ 実体
                
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dStartAnimCtrl
( 
    NNSG2dAnimController*       pAnimCtrl 
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    pAnimCtrl->bActive = TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dStopAnimCtrl

  Description:  アニメーションコントローラ の アニメ再生を終了します。
                
  Arguments:    pAnimCtrl:            アニメーションコントローラ 実体
                
                
                
  Returns:      コントローラの種類
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dStopAnimCtrl
( 
    NNSG2dAnimController*       pAnimCtrl 
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    pAnimCtrl->bActive = FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dIsAnimCtrlActive

  Description:  アニメーションコントローラ が アニメ再生中か調査します。
                
  Arguments:    pAnimCtrl:            アニメーションコントローラ 実体
                
                
                
  Returns:      アニメ再生中ならTRUE
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE BOOL NNS_G2dIsAnimCtrlActive
( 
    const NNSG2dAnimController*       pAnimCtrl 
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    return pAnimCtrl->bActive;
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetAnimCtrlPlayModeOverridden

  Description:  ファイル情報で設定されたアニメーションシーケンスの再生方式を
                オーバーライドします。
                
  Arguments:    pAnimCtrl:            アニメーションコントローラ 実体
                playMode :            アニメ再生方式
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dSetAnimCtrlPlayModeOverridden
( 
    NNSG2dAnimController*       pAnimCtrl,
    NNSG2dAnimationPlayMode     playMode 
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_MINMAX_ASSERT( playMode, NNS_G2D_ANIMATIONPLAYMODE_FORWARD,
                                 NNS_G2D_ANIMATIONPLAYMODE_REVERSE_LOOP );
                                 
    pAnimCtrl->overriddenPlayMode = playMode;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dResetAnimCtrlPlayModeOverridden

  Description:  オーバーライドされたアニメ再生方式を元に戻します。
                （ファイル情報で設定された再生方式に戻します）
                
  Arguments:    pAnimCtrl:            アニメーションコントローラ 実体
                
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dResetAnimCtrlPlayModeOverridden
( 
    NNSG2dAnimController*       pAnimCtrl
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    pAnimCtrl->overriddenPlayMode = NNS_G2D_ANIMATIONPLAYMODE_INVALID;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetAnimCtrlCurrentTime

  Description:  アニメーションコントローラ の 現在のアニメフレーム表示時間を取得します
                
  Arguments:    pAnimCtrl:            アニメーションコントローラ 実体
                
                
                
  Returns:      現在のアニメフレーム表示時間
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE fx32 NNS_G2dGetAnimCtrlCurrentTime
( 
    const NNSG2dAnimController*       pAnimCtrl 
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    return pAnimCtrl->currentTime;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetAnimCtrlCurrentTime

  Description:  アニメーションコントローラ の 現在のアニメフレーム表示時間を設定します
                
  Arguments:    pAnimCtrl:            アニメーションコントローラ 実体
                time     ：           現在のアニメフレーム表示時間
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dSetAnimCtrlCurrentTime
( 
    NNSG2dAnimController*       pAnimCtrl,
    fx32                        time
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    pAnimCtrl->currentTime = time;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetAnimCtrlCurrentElemIdxVal

  Description:  アニメーションコントローラ の 
                現在のアニメ結果からインデックスの値を取得します
                
  Arguments:    pAnimCtrl:            アニメーションコントローラ 実体
                
                
  Returns:      現在のアニメ結果のインデックス値
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE u16 NNS_G2dGetAnimCtrlCurrentElemIdxVal
( 
    const NNSG2dAnimController*       pAnimCtrl 
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    //
    // どの形式のアニメーションであっても、先頭の2バイトは必ず、
    // インデックス値として利用されています。
    //
    {
        const NNSG2dAnimData* pAnmRes 
           = (const NNSG2dAnimData*)pAnimCtrl->pCurrent->pContent;
    
        return (*pAnmRes);        
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetAnimCtrlCurrentAnimSequence

  Description:  アニメーションコントローラ の 
                現在のアニメーションシーケンスを取得します
                
  Arguments:    pAnimCtrl:            アニメーションコントローラ 実体
                
                
  Returns:      現在のアニメーションシーケンス
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE const NNSG2dAnimSequence* 
NNS_G2dGetAnimCtrlCurrentAnimSequence
( 
    const NNSG2dAnimController*       pAnimCtrl 
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    return pAnimCtrl->pAnimSequence;
}


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif // NNS_G2D_ANIMATION_INLINE_H_


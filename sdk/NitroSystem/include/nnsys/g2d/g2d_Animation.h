/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d
  File:     g2d_Animation.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.24 $
 *---------------------------------------------------------------------------*/
 
#ifndef NNS_G2D_ANIMATION_H_
#define NNS_G2D_ANIMATION_H_

#include <nitro.h>
#include <nnsys/g2d/g2d_config.h>
#include <nnsys/g2d/g2d_Data.h>

#ifdef __cplusplus
extern "C" {
#endif


//
// 改名を行った関数の別名
// 互換性維持のため別名として以前の関数を宣言します。
// 
#define NNS_G2dGetCurrentElement               NNS_G2dGetAnimCtrlCurrentElement
#define NNS_G2dGetNextElement                  NNS_G2dGetAnimCtrlNextElement
#define NNS_G2dGetNormalizedTime               NNS_G2dGetAnimCtrlNormalizedTime
#define NNS_G2dSetCallBackFunctor              NNS_G2dSetAnimCtrlCallBackFunctor
#define NNS_G2dSetCallBackFunctorAtAnimFrame   NNS_G2dSetAnimCtrlCallBackFunctorAtAnimFrame
#define NNS_G2dGetAnimSpeed                    NNS_G2dGetAnimCtrlSpeed
#define NNS_G2dSetAnimSpeed                    NNS_G2dSetAnimCtrlSpeed
#define NNS_G2dResetAnimationState             NNS_G2dResetAnimCtrlState
#define NNS_G2dInitCallBackFunctor             NNS_G2dInitAnimCallBackFunctor

#define NNS_G2dBindAnimController 						NNS_G2dBindAnimCtrl
#define NNS_G2dGetAnimControllerType 					NNS_G2dGetAnimCtrlType
#define NNS_G2dInitAnimController 						NNS_G2dInitAnimCtrl
#define NNS_G2dInitAnimControllerCallBackFunctor 	NNS_G2dInitAnimCtrlCallBackFunctor
#define NNS_G2dIsAnimControllerActive 					NNS_G2dIsAnimCtrlActive
#define NNS_G2dStartAnimController 					NNS_G2dStartAnimCtrl
#define NNS_G2dStopAnimController 						NNS_G2dStopAnimCtrl
#define NNS_G2dTickAnimController 						NNS_G2dTickAnimCtrl



#define NNS_G2D_ASSERT_ANIMATIONTYPE_VALID( val )   \
        NNS_G2D_MINMAX_ASSERT( val, NNS_G2D_ANIMATIONTYPE_CELL, NNS_G2D_ANIMATIONTYPE_MULTICELLLOCATION )


/*---------------------------------------------------------------------------*
  Name:         NNSG2dAnimFrame

  Description:  アニメーション実体へのポインタと、表示フレーム数のペアです
                アニメーションの最小単位です。
                複数個のFrame列がNNSG2dAnimSequenceを形成します。
                
 *---------------------------------------------------------------------------*/
typedef NNSG2dAnimFrameData NNSG2dAnimFrame;


/*---------------------------------------------------------------------------*
  Name:         NNSG2dAnimSequence

  Description:  一連のアニメーションデータをあらわします。
                複数個のFrame列を持ちます
                Sequence内では すべての NNSG2dAnimFrame が 同一NNSG2dAnimationTypeを持ちます。
                
 *---------------------------------------------------------------------------*/
typedef NNSG2dAnimSequenceData NNSG2dAnimSequence;

/*---------------------------------------------------------------------------*
  Name:         NNSG2dAnmCallBackPtr

  Description:  アニメーション コールバック
                
 *---------------------------------------------------------------------------*/
// data = NNSG2dAnimCallBackFunctor.param
typedef void (*NNSG2dAnmCallBackPtr)( u32 data, fx32 currentFrame );


/*---------------------------------------------------------------------------*
  
  Name:         NNSG2dAnmCallbackType

  Description:  アニメーション コールバック の種類
                
 *---------------------------------------------------------------------------*/
typedef enum NNSG2dAnmCallbackType
{    
    NNS_G2D_ANMCALLBACKTYPE_NONE = 0,  
    NNS_G2D_ANMCALLBACKTYPE_LAST_FRM,  // アニメーションシーケンスの最終フレーム終了時によびだす
    NNS_G2D_ANMCALLBACKTYPE_SPEC_FRM,  // 指定フレームの再生時に呼びだす。
    NNS_G2D_ANMCALLBACKTYPE_EVER_FRM,  // 毎フレーム呼び出す。
    AnmCallbackType_MAX

}NNSG2dAnmCallbackType;

/*---------------------------------------------------------------------------*
  
  Name:         NNSG2dAnimCallBackFunctor

  Description:  アニメーション コールバック の関数ポインタと、
                呼び出し時に引数として渡される、ユーザ定義u32データをグループ化する概念
                NNSG2dAnimController に保持されます
                // 別名 NNSG2dAnimCallBackFunctor が追加されました
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dCallBackFunctor
{   
    NNSG2dAnmCallbackType          type;            // コールバックの種類
    u32                            param;           // ユーザ利用可能パラメータ
    NNSG2dAnmCallBackPtr           pFunc;           // コールバック関数ポインタ
    u16                            frameIdx;        // フレーム番号( type == NNS_G2D_ANMCALLBACKTYPE_SPEC_FRM時に使用)
    u16                            pad16_;          // パディング
    
}NNSG2dCallBackFunctor, NNSG2dAnimCallBackFunctor;



/*---------------------------------------------------------------------------*
  
  Name:         NNSG2dAnimController

  Description:  アニメーションの状態を保持する構造
                アニメーションデータを保持します
                
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dAnimController
{
    const NNSG2dAnimFrame*      pCurrent;               // 現在のアニメーションフレーム
    const NNSG2dAnimFrame*      pActiveCurrent;         // 現在のアニメーションフレーム(有効な表示対象に限る)
                                                        // 通常は、pCurrentと同一の値を持ちます。
                                                        // 表示フレーム数ゼロフレームのアニメーションフレーム
                                                        // を正しく表示するために利用されます。
                                                        // 基本的に、表示フレームがゼロフレームでは無いフレームを
                                                        // 示しますが、以前のバージョンとの互換性維持のため、
                                                        // 表示フレームがゼロフレームのみから構成されるシーケンスの場合
                                                        // もpCurrentの初期位置と同一の位置を指し示し、
                                                        // NULLに設定されることはありません。
    
    BOOL                        bReverse;               // 逆再生フラグ
    BOOL                        bActive;                // アクティブフラグ

    fx32                        currentTime;            // 現在の時間
    fx32                        speed;                  // スピード
    
    NNSG2dAnimationPlayMode     overriddenPlayMode;     // 再生モードをプログラマがオーバーライドする際に設定されます。
                                                        // 規定値：NNS_G2D_ANIMATIONPLAYMODE_INVALID
                                                        
    const NNSG2dAnimSequence*   pAnimSequence;          // 関連付けされた、アニメーションシーケンス
    NNSG2dAnimCallBackFunctor   callbackFunctor;        // コールバックファンクタ
    
}NNSG2dAnimController;


//------------------------------------------------------------------------------
// 関数プロトタイプ
//------------------------------------------------------------------------------

void NNSi_G2dCallbackFuncHandling
( 
    const NNSG2dCallBackFunctor*  pFunctor, 
    u16 currentFrameIdx 
);


BOOL           NNS_G2dTickAnimCtrl
( 
    NNSG2dAnimController*     pAnimCtrl, 
    fx32                      frames 
);

BOOL           NNS_G2dSetAnimCtrlCurrentFrame
( 
    NNSG2dAnimController*     pAnimCtrl, 
    u16                       index
);

BOOL           NNS_G2dSetAnimCtrlCurrentFrameNoResetCurrentTime
( 
    NNSG2dAnimController*     pAnimCtrl, 
    u16                       index
);

u16 NNS_G2dGetAnimCtrlCurrentFrame
(
    const NNSG2dAnimController*     pAnimCtrl
);

void            NNS_G2dInitAnimCtrl
( 
    NNSG2dAnimController*     pAnimCtrl 
);

void            NNS_G2dInitAnimCtrlCallBackFunctor
( 
    NNSG2dAnimController*     pAnimCtrl 
);

void            NNS_G2dInitAnimCallBackFunctor
( 
    NNSG2dAnimCallBackFunctor*   pCallBack 
);

void            NNS_G2dResetAnimCtrlState
( 
    NNSG2dAnimController*     pAnimCtrl
);

void            NNS_G2dBindAnimCtrl
( 
    NNSG2dAnimController*        pAnimCtrl, 
    const NNSG2dAnimSequence*    pAnimSequence 
);

void* NNS_G2dGetAnimCtrlCurrentElement  ( NNSG2dAnimController* pAnimCtrl );
void* NNS_G2dGetAnimCtrlNextElement     ( NNSG2dAnimController* pAnimCtrl );// testing
fx32  NNS_G2dGetAnimCtrlNormalizedTime  ( NNSG2dAnimController* pAnimCtrl );// testing

void NNS_G2dSetAnimCtrlCallBackFunctor
( 
    NNSG2dAnimController*   pAnimCtrl, 
    NNSG2dAnmCallbackType   type, 
    u32                     param, 
    NNSG2dAnmCallBackPtr    pFunc 
);

void NNS_G2dSetAnimCtrlCallBackFunctorAtAnimFrame
( 
    NNSG2dAnimController*   pAnimCtrl, 
    u32                     param, 
    NNSG2dAnmCallBackPtr    pFunc, 
    u16                     frameIdx 
);

//------------------------------------------------------------------------------
// ライブラリ内部限定公開関数
BOOL NNSi_G2dIsAnimCtrlLoopAnim( const NNSG2dAnimController* pAnimCtrl );

//------------------------------------------------------------------------------
// inline functions 
//------------------------------------------------------------------------------
static NNSG2dAnimationType NNS_G2dGetAnimCtrlType
( 
    const NNSG2dAnimController*  pAnimCtrl 
);

static void NNS_G2dSetAnimCtrlSpeed
( 
    NNSG2dAnimController*       pAnimCtrl, 
    fx32                        speed 
);

static fx32 NNS_G2dGetAnimCtrlSpeed
( 
    const NNSG2dAnimController*       pAnimCtrl
);

static void NNS_G2dStartAnimCtrl
( 
    NNSG2dAnimController*       pAnimCtrl 
);

static void NNS_G2dStopAnimCtrl
( 
    NNSG2dAnimController*       pAnimCtrl 
);

static BOOL NNS_G2dIsAnimCtrlActive
( 
    const NNSG2dAnimController*       pAnimCtrl 
);

static void NNS_G2dSetAnimCtrlPlayModeOverridden
( 
    NNSG2dAnimController*       pAnimCtrl,
    NNSG2dAnimationPlayMode     playMode 
);

static void NNS_G2dResetAnimCtrlPlayModeOverridden
( 
    NNSG2dAnimController*       pAnimCtrl
);

static fx32 NNS_G2dGetAnimCtrlCurrentTime
( 
    const NNSG2dAnimController*       pAnimCtrl 
);

static void NNS_G2dSetAnimCtrlCurrentTime
( 
    NNSG2dAnimController*       pAnimCtrl,
    fx32                        time
);

static u16 NNS_G2dGetAnimCtrlCurrentElemIdxVal
( 
    const NNSG2dAnimController*       pAnimCtrl 
);

static const NNSG2dAnimSequence* 
NNS_G2dGetAnimCtrlCurrentAnimSequence
( 
    const NNSG2dAnimController*       pAnimCtrl 
);



#ifdef __cplusplus
} /* extern "C" */
#endif

#include <nnsys/g2d/g2d_Animation_inline.h>
#endif // NNS_G2D_ANIMATION_H_


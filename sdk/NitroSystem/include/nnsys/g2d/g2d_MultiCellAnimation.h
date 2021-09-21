/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d
  File:     g2d_MultiCellAnimation.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.22 $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_MULTICELLANIMATION_H_
#define NNS_G2D_MULTICELLANIMATION_H_

#include <nitro.h>
#include <nnsys/g2d/fmt/g2d_MultiCell_data.h>

#include <nnsys/g2d/g2d_Node.h>
#include <nnsys/g2d/g2d_Animation.h>
#include <nnsys/g2d/g2d_CellAnimation.h>

#ifdef __cplusplus
extern "C" {
#endif


//------------------------------------------------------------------------------
// まったく同じフォーマットですが、わかりやすさのために別名を定義します。
typedef NNSG2dAnimSequence            NNSG2dMultiCellAnimSequence;
typedef NNSG2dAnimBankData            NNSG2dMultiCellAnimBankData;


//
// マルチセル中のすべてのセルアニメーションを走査しながら呼び出される関数
// コールバック呼び出しを中断する場合はFALSEを返します。
//
typedef BOOL (*NNSG2dMCTraverseCellAnimCallBack)
( 
    u32                   userParamater,
    NNSG2dCellAnimation*  pCellAnim, 
    u16                   cellAnimIdx 
);    
//
// マルチセル中のすべてのセルアニメーションを走査しながら呼び出される関数
// コールバック呼び出しを中断する場合はFALSEを返します。
//
typedef BOOL (*NNSG2dMCTraverseNodeCallBack)
( 
    u32                                 userParamater,
    const NNSG2dMultiCellHierarchyData* pNodeData,
    NNSG2dCellAnimation*                pCellAnim, 
    u16                                 nodeIdx 
);    


//
// 改名を行った関数の別名
// 互換性維持のため別名として以前の関数を宣言します。
// 
#define NNS_G2dInitializeMCAnimation       NNS_G2dInitMCAnimation
#define NNS_G2dInitializeMCInstance        NNS_G2dInitMCInstance
#define NNS_G2dSetMCAnimSpeed              NNS_G2dSetMCAnimationSpeed

typedef enum 
{
    NNS_G2D_MCTYPE_DONOT_SHARE_CELLANIM,
    NNS_G2D_MCTYPE_SHARE_CELLANIM
    
}NNSG2dMCType;

typedef struct NNSG2dMCNodeArray
{
    NNSG2dNode*                         pNodeArray;         // Node 配列
    u16                                 numNode;            // Node 数
    u16                                 pad16_;             // パディング
    
}NNSG2dMCNodeArray;

typedef struct NNSG2dMCCellAnimation
{
    NNSG2dCellAnimation     cellAnim;
    BOOL                    bInited;
    
}NNSG2dMCCellAnimation;


typedef struct NNSG2dMCNodeCellAnimArray
{
    NNSG2dMCCellAnimation*       cellAnimArray;
    
}NNSG2dMCNodeCellAnimArray;

//------------------------------------------------------------------------------
// 注意：pCurrentMultiCell は 
//       ( numNode > pCurrentMultiCell->numNodes ) を満たす必要があります。
//       ただ、ほとんどの場合は numNode == pCurrentMultiCell->numNodes でしょう
//
//       以前 の バージョンの ComposedObj の代替概念です
typedef struct NNSG2dMultiCellInstance 
{
    const NNSG2dMultiCellData*          pCurrentMultiCell;  // マルチセルデータ
    
    const NNSG2dCellAnimBankData*       pAnimDataBank;      // MultiCellを構成する 
                                                            // NNSG2dCellAnimationが定義されている 
                                                            // NNSG2dAnimBankData
    
    //
    // 初期化する条件によって内部のデータ実体の構成がことなる
    // NNSG2dMCNodeArray か もしくは NNSG2dMCNodeCellAnimArrayとなる
    //
    //
    NNSG2dMCType                        mcType;
    void*                               pCellAnimInstasnces;
    
    /*
    NNSG2dNode*                         pNodeArray;         // Node 配列
    u16                                 numNode;            // Node 数
    u16                                 pad16_;             // パディング
    */
    
}NNSG2dMultiCellInstance;



//------------------------------------------------------------------------------
// animCtrl の 結果は NNSG2dMultiCellDataへのIndex情報を持ちます。
// ライブラリはデータベースからそのIndexをキーとして、NNSG2dMultiCellDataへのポインタを取得します。
// その後、取得した NNSG2dMultiCellData を NNSG2dMultiCellInstance に設定します。
typedef struct NNSG2dMultiCellAnimation
{
    NNSG2dAnimController              animCtrl;             // アニメーションコントローラ
    
    u16                               totalVideoFrame;      // 総ビデオフレーム
    u16                               pad16;                // パディング
    
    NNSG2dMultiCellInstance           multiCellInstance;    // マルチセル実体
    
    const NNSG2dMultiCellDataBank*    pMultiCellDataBank;   // マルチセルデータバンク
    
    NNSG2dSRTControl                  srtCtrl;              // SRT アニメーション結果
    
    
    // TODO:Bounding volume
}NNSG2dMultiCellAnimation;




//------------------------------------------------------------------------------
// 初期化関連
//------------------------------------------------------------------------------


//------------------ NEW ------------------------------
void NNS_G2dInitMCAnimationInstance
( 
    NNSG2dMultiCellAnimation*          pMultiCellAnim,  
    void*                              pWork, 
    const NNSG2dCellAnimBankData*      pAnimBank,  
    const NNSG2dCellDataBank*          pCellDataBank,
    const NNSG2dMultiCellDataBank*     pMultiCellDataBank,
    NNSG2dMCType                       mcType
);




//------------------------------------------------------------------------------
void NNS_G2dSetAnimSequenceToMCAnimation
( 
    NNSG2dMultiCellAnimation*             pMultiCellAnim, 
    const NNSG2dMultiCellAnimSequence*    pAnimSeq 
);
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
u16 NNS_G2dGetMCNumNodesRequired
( 
    const NNSG2dMultiCellAnimSequence*    pMultiCellSeq, 
    const NNSG2dMultiCellDataBank*        pMultiCellDataBank 
);
u16 NNS_G2dGetMCBankNumNodesRequired
( 
    const NNSG2dMultiCellDataBank*       pMultiCellDataBank 
);
u32 NNS_G2dGetMCWorkAreaSize
(
    const NNSG2dMultiCellDataBank*       pMultiCellDataBank,
    NNSG2dMCType                         mcType
);

//------------------------------------------------------------------------------
// アニメーション関連
//------------------------------------------------------------------------------
void NNS_G2dTickMCInstance( NNSG2dMultiCellInstance* pMultiCellAnim, fx32 frames );
void NNS_G2dTickMCAnimation( NNSG2dMultiCellAnimation* pMultiCellAnim, fx32 frames );
//------------------------------------------------------------------------------
void NNS_G2dSetMCAnimationCurrentFrame
( 
    NNSG2dMultiCellAnimation*   pMultiCellAnim, 
    u16                         frameIndex 
);

//------------------------------------------------------------------------------
// マルチセル中のセルアニメーションのアニメーションフレームを設定する
void NNS_G2dSetMCAnimationCellAnimFrame
( 
    NNSG2dMultiCellAnimation*   pMultiCellAnim, 
    u16                         caFrameIndex     
);

void NNS_G2dStartMCCellAnimationAll
(
    NNSG2dMultiCellInstance*    pMCInst 
);

void NNS_G2dRestartMCAnimation
(
    NNSG2dMultiCellAnimation*   pMultiCellAnim
);

//------------------------------------------------------------------------------
void NNS_G2dSetMCAnimationSpeed
(
    NNSG2dMultiCellAnimation*   pMultiCellAnim, 
    fx32                        speed  
);

void NNS_G2dResetMCCellAnimationAll
(
    NNSG2dMultiCellInstance*    pMCInst 
);

//------------------------------------------------------------------------------
// OAM 情報 を 生成
u16 NNS_G2dMakeSimpleMultiCellToOams
( 
    GXOamAttr*                      pDstOams, 
    u16                             numDstOams,
    const NNSG2dMultiCellInstance*  pMCellInst, 
    const MtxFx22*                  pMtxSR, 
    const NNSG2dFVec2*              pBaseTrans,
    u16                             affineIndex,
    BOOL                            bDoubleAffine 
);

//------------------------------------------------------------------------------
void NNS_G2dTraverseMCCellAnims
( 
    NNSG2dMultiCellInstance*         pMCellInst,
    NNSG2dMCTraverseCellAnimCallBack pCBFunc,
    u32                              userParamater
);
//------------------------------------------------------------------------------
void NNS_G2dTraverseMCNodes
( 
    NNSG2dMultiCellInstance*        pMCellInst,
    NNSG2dMCTraverseNodeCallBack    pCBFunc,
    u32                             userParamater
);


//------------------ OLD ------------------------------
// 古いAPI
// 過去のバージョンとの互換性維持のため残されています。
void NNS_G2dInitMCAnimation( 
    NNSG2dMultiCellAnimation*          pMultiCellAnim, 
    NNSG2dNode*                        pNodeArray, 
    NNSG2dCellAnimation*               pCellAnim, 
    u16                                numNode, 
    const NNSG2dCellAnimBankData*      pAnimBank,  
    const NNSG2dCellDataBank*          pCellDataBank,
    const NNSG2dMultiCellDataBank*     pMultiCellDataBank 
);
void NNS_G2dInitMCInstance
( 
    NNSG2dMultiCellInstance*      pMultiCell, 
    NNSG2dNode*                   pNodeArray, 
    NNSG2dCellAnimation*          pCellAnim, 
    u16                           numNode, 
    const NNSG2dCellAnimBankData* pAnimBank,  
    const NNSG2dCellDataBank*     pCellDataBank 
);
BOOL NNS_G2dSetMCDataToMCInstance( 
    NNSG2dMultiCellInstance*       pMCInst, 
    const NNSG2dMultiCellData*     pMcData
);
//------------------ OLD ------------------------------


//------------------------------------------------------------------------------
// inline funcs.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
NNS_G2D_INLINE NNSG2dAnimController* NNS_G2dGetMCAnimAnimCtrl
( 
    NNSG2dMultiCellAnimation* pMultiCellAnim 
)
{
    NNS_G2D_NULL_ASSERT( pMultiCellAnim );
    return &pMultiCellAnim->animCtrl;
}

//
// 内部ワークメモリへのポインタを取得します
//
NNS_G2D_INLINE void* NNSi_G2dGetMCInstanceWorkMemory
(
    NNSG2dMultiCellInstance*      pMultiCell
)
{
    return pMultiCell->pCellAnimInstasnces;
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_G2D_MULTICELLANIMATION_H_


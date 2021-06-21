/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d
  File:     g2d_OAMEx.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.18 $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_OAMEX_H_
#define NNS_G2D_OAMEX_H_

#include <nitro.h>
#include <nnsys/g2d/g2d_config.h>

#ifdef __cplusplus
extern "C" {
#endif


//
// 改名を行った関数の別名
// 互換性維持のため別名として以前の関数を宣言します。
// 
#define NNS_G2dEntryNewOamEx                   NNS_G2dEntryOamManExOam
#define NNS_G2dEntryNewOamWithAffineIdxEx      NNS_G2dEntryOamManExOamWithAffineIdx
#define NNS_G2dEntryNewAffineParamsEx          NNS_G2dEntryOamManExAffine
#define NNS_G2dApplyToBaseModuleEx             NNS_G2dApplyOamManExToBaseModule

#define NNS_G2dResetOamInstanceEx              NNS_G2dResetOamManExBuffer
#define NNS_G2dGetNewOamInstanceEx             NNS_G2dGetOamManExInstance

#define NNS_G2dSetOamEntryFunctionsEx          NNS_G2dSetOamManExEntryFunctions



//
// HW OAM Affine テーブルのインデックスが初期化されていない
// NNSG2dAffineParamProxy.affineHWIndex の 初期値です。
//
#define NNS_G2D_OAMEX_HW_ID_NOT_INIT       0xFFFF


//------------------------------------------------------------------------------
//
// 描画登録順の種類
//
typedef enum NNSG2dOamExDrawOrder
{
    NNSG2D_OAMEX_DRAWORDER_BACKWARD = 0x0, // 登録した順番と逆の順番に外部モジュールへの登録がおこなわれる
    NNSG2D_OAMEX_DRAWORDER_FORWARD  = 0x1  // 登録した順番と同じの順番に外部モジュールへの登録がおこなわれる

}NNSG2dOamExDrawOrder;


//------------------------------------------------------------------------------
// 各種登録関数
typedef u16     (*NNSG2dGetOamCpacityFuncPtr)(); 
typedef u16     (*NNSG2dGetOamAffineCpacityFuncPtr)(); 
typedef BOOL    (*NNSG2dEntryNewOamFuncPtr)( const GXOamAttr* pOam, u16 index ); 
typedef u16     (*NNSG2dEntryNewOamAffineFuncPtr)( const MtxFx22* mtx, u16 index ); 

typedef struct NNSG2dOamExEntryFunctions
{
    NNSG2dGetOamCpacityFuncPtr        getOamCapacity;
    NNSG2dGetOamCpacityFuncPtr        getAffineCapacity;
    NNSG2dEntryNewOamFuncPtr          entryNewOam;
    NNSG2dEntryNewOamAffineFuncPtr    entryNewAffine;
    
}NNSG2dOamExEntryFunctions;



//------------------------------------------------------------------------------
// 内部で使用する
// OAMアトリビュート管理構造
typedef struct NNSG2dOamChunk
{
    
    GXOamAttr               oam;             // OAMアトリビュート
    u16                     affineProxyIdx;  // Oam が 参照する NNSG2dAffineParamProxy Index 値
    u16                     pad16_;          // パディング
    struct NNSG2dOamChunk*  pNext;           // リスト構造を構築するためのリンク
    
}NNSG2dOamChunk; 

//------------------------------------------------------------------------------
// NNSG2dOamChunk をまとめる 概念
// 本構造の配列が描画優先度順にならんで オーダリングテーブルとなります
//
// 内部は２つの NNSG2dOamChunk リストを持ちます。
// ひとつは 通常の OBJ 用 ひとつは affine変換された OBJ 用 です
//
typedef struct NNSG2dOamChunkList
{
    u16                 numChunks;         // pChunks リストのNNSG2dOamChunk総数
    u16                 numLastFrameDrawn; // 前フレームで描画登録された NNSG2dOamChunk 番号
    
    u16                 numDrawn;          // 描画すべきチャンク数
    u16                 bDrawn;            // 描画登録すべきChunkListかどうか？
    
    NNSG2dOamChunk*     pChunks;           // NNSG2dOamChunk リスト
    NNSG2dOamChunk*     pAffinedChunks;    // affine変換された NNSG2dOamChunk リスト
    
    NNSG2dOamChunk*     pLastChunk;        // NNSG2dOamChunk リストの終端
    NNSG2dOamChunk*     pLastAffinedChunk; // affine変換された NNSG2dOamChunk リストの終端
    
}NNSG2dOamChunkList;


/*---------------------------------------------------------------------------*
  Name:         NNSG2dAffineParamProxy

  Description:  OAMEx 内部バッファ で Affine パラメータをあらわすデータ構造
                
                NNSG2dAffineParamProxy.affineHWIndex の 初期値は NNS_G2D_OAMEX_HW_ID_NOT_INIT です。
                NNSG2dAffineParamProxy.affineHWIndex は CopyAffineToOamManager_() で HWにロードされた後に             
                実際にHWにロードされたインデックスに上書きされます
                
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dAffineParamProxy
{
    MtxFx22             mtxAffine;          // Affine Paramaters
    u16                 affineHWIndex;      // HW にロードされた時のAffineIndex値
    u16                 pad16_;             // パディング
}
NNSG2dAffineParamProxy;

//------------------------------------------------------------------------------
// 拡張マネージャ２実体
typedef struct NNSG2dOamManagerInstanceEx
{
    
    NNSG2dOamChunkList*             pOamOrderingTbl;     // 描画優先度テーブル先頭へのポインタ
    
    u16                             numPooledOam;        // OamChunks 総数
    u16                             numUsedOam;          // 使用済みOamChunks 数
    NNSG2dOamChunk*                 pPoolOamChunks;      // 共有 OamChunks 配列へのポインタ
    
    u16                             lengthOfOrderingTbl; // 描画優先度テーブルの長さ:( 0 - 255 ：u8 値で初期化されます）
    u16                             lengthAffineBuffer;  // NNSG2dAffineParamProxy 総数
    u16                             numAffineBufferUsed; // 使用済みAffineParamProxy 数
    u16                             lastFrameAffineIdx;  // 前フレーム 登録 NNSG2dAffineParamProxy 番号
    NNSG2dAffineParamProxy*         pAffineBuffer;       // 共有 AffineParamProxy 配列
    
    NNSG2dOamExEntryFunctions       oamEntryFuncs;       // 登録関数
    
    
    
    u16                             lastRenderedOrderingTblIdx;
    u16                             lastRenderedChunkIdx;
    NNSG2dOamExDrawOrder            drawOrderType;       // 描画順タイプ
                                                         // (過去のバージョンとの互換性維持のために、デフォルトは逆順)

}NNSG2dOamManagerInstanceEx;




//------------------------------------------------------------------------------
void NNS_G2dResetOamManExBuffer  ( NNSG2dOamManagerInstanceEx* pOam );
//------------------------------------------------------------------------------
BOOL NNS_G2dGetOamManExInstance( 
    NNSG2dOamManagerInstanceEx*    pOam, 
    NNSG2dOamChunkList*             pOamOrderingTbl, 
    u8                              lengthOfOrderingTbl,
    u16                             numPooledOam,
    NNSG2dOamChunk*                 pPooledOam,
    u16                             lengthAffineBuffer,
    NNSG2dAffineParamProxy*         pAffineBuffer );
//------------------------------------------------------------------------------
BOOL NNS_G2dEntryOamManExOam
( 
    NNSG2dOamManagerInstanceEx*    pMan, 
    const GXOamAttr*                pOam, 
    u8                              priority 
);
//------------------------------------------------------------------------------
BOOL NNS_G2dEntryOamManExOamWithAffineIdx
( 
    NNSG2dOamManagerInstanceEx*    pMan, 
    const GXOamAttr*                pOam, 
    u8                              priority, 
    u16                             affineIdx 
);
//------------------------------------------------------------------------------
u16  NNS_G2dEntryOamManExAffine
( 
    NNSG2dOamManagerInstanceEx*    pMan, 
    const MtxFx22*                  mtx 
);
//------------------------------------------------------------------------------
void NNS_G2dApplyOamManExToBaseModule 
( 
    NNSG2dOamManagerInstanceEx* pMan 
);

//------------------------------------------------------------------------------
void NNSG2d_SetOamManExDrawOrderType
( 
    NNSG2dOamManagerInstanceEx*    pOam, 
    NNSG2dOamExDrawOrder           drawOrderType
);

//------------------------------------------------------------------------------
void NNS_G2dSetOamManExEntryFunctions
( 
    NNSG2dOamManagerInstanceEx*        pMan, 
    const NNSG2dOamExEntryFunctions*   pSrc 
);





#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_G2D_OAMEX_H_
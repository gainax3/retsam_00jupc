/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d
  File:     g2d_OAM.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.22 $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_OAM_H_
#define NNS_G2D_OAM_H_

#include <nitro.h>
#include <nnsys/g2d/fmt/g2d_Vec_data.h>
#include <nnsys/g2d/g2d_OAM_Types.h>
#include <nnsys/g2d/fmt/g2d_Oam_data.h>
#include <nnsys/g2d/fmt/g2d_Character_data.h>

#include <nnsys/g2d/g2d_Image.h> // TODO : この依存はOKか？要検討

#ifdef __cplusplus
extern "C" {
#endif

//
// 改名を行った関数の別名
// 互換性維持のため別名として以前の関数を宣言します。
// 
#define NNS_G2dInitializeOamManager         NNS_G2dInitOamManagerModule
#define NNS_G2dEntryNewOam                  NNS_G2dEntryOamManagerOam
#define NNS_G2dEntryNewOamWithAffineIdx     NNS_G2dEntryOamManagerOamWithAffineIdx
#define NNS_G2dEntryNewOamAffine            NNS_G2dEntryOamManagerAffine
#define NNS_G2dEntryOamAffine               NNS_G2dSetOamManagerAffine
#define NNS_G2dApplyToHW                    NNS_G2dApplyOamManagerToHW
#define NNS_G2dApplyToHWSoftEmu             NNS_G2dApplyOamManagerToHWSprite               
#define NNS_G2dResetBuffer                  NNS_G2dResetOamManagerBuffer
#define NNS_G2dApplyToHWAndReset            NNS_G2dApplyAndResetOamManagerBuffer
#define NNS_G2dGetOamAttrCapacity           NNS_G2dGetOamManagerOamCapacity
#define NNS_G2dGetOamAffineCapacity         NNS_G2dGetOamManagerAffineCapacity



/*---------------------------------------------------------------------------*
  Name:         NNSG2dOamType

  Description:  NNSG2dOamType の 種類
                
 *---------------------------------------------------------------------------*/
typedef enum NNSG2dOamType
{
    NNS_G2D_OAMTYPE_MAIN = 0,               // メイン2DグラフィックスエンジンのOAM
    NNS_G2D_OAMTYPE_SUB,                    // サブ2DグラフィックスエンジンのOAM
    NNS_G2D_OAMTYPE_SOFTWAREEMULATION,      // 3D Graphics Engine を 使用した エミュレーション
    NNS_G2D_OAMTYPE_INVALID,                // 不正な種類
    NNS_G2D_OAMTYPE_MAX         
    
}NNSG2dOamType;

#define ASSERT_OAMTYPE( x )                                      \
    NNS_G2D_ASSERT( (x) == NNS_G2D_OAMTYPE_INVALID                || \
                (x) == NNS_G2D_OAMTYPE_MAIN                   || \
                (x) == NNS_G2D_OAMTYPE_SUB                    || \
                (x) == NNS_G2D_OAMTYPE_SOFTWAREEMULATION      || \
                (x) == NNS_G2D_OAMTYPE_MAX       )
                
#define ASSERT_OAMTYPE_VALID( x )                  \
    NNS_G2D_ASSERT( (x) == NNS_G2D_OAMTYPE_MAIN     || \
                (x) == NNS_G2D_OAMTYPE_SUB      || \
                (x) == NNS_G2D_OAMTYPE_SOFTWAREEMULATION )



/*---------------------------------------------------------------------------*
  Name:         NNSG2dOAMManageArea

  Description:  管理領域
                
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dOAMManageArea
{
    u16     fromIdx;                // 管理領域 （開始番号）
    u16     toIdx;                  // 管理領域 （終了番号）
    u16     currentIdx;             // 現在の登録位置 番号
}NNSG2dOAMManageArea;

/*---------------------------------------------------------------------------*
  Name:         NNSG2dOamManagerInstance

  Description:  OamManager の 実体
                
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dOamManagerInstance
{
    NNSG2dOamType         type;                // Oamの種類
    
    NNSG2dOAMManageArea   managedAttrArea;     // マネージャ管理領域(Oam Attribute)
    NNSG2dOAMManageArea   managedAffineArea;   // マネージャ管理領域(Affine Paramater)
    
    u16                   GUID;                // NNSG2dOamManagerInstance を識別する ID
    u16                   pad16_;              // パディング
    BOOL                  bFastTransferEnable; // 初期化時に設定されます。(ユーザは操作しないでください)
    fx32                  spriteZoffsetStep;   // スプライト描画時に使用される自動オフセット値
    
}NNSG2dOamManagerInstance;




//------------------------------------------------------------------------------
// ライブラリ内部公開
//------------------------------------------------------------------------------
void* NNSi_G2dGetOamManagerInternalBufferForDebug( NNSG2dOamType type );




//------------------------------------------------------------------------------
// 外部公開
//------------------------------------------------------------------------------

void NNS_G2dInitOamManagerModule();

BOOL NNS_G2dGetNewManagerInstance( NNSG2dOamManagerInstance* pMan, u16 from, u16 to, NNSG2dOamType type );
BOOL NNS_G2dInitManagerInstanceAffine( NNSG2dOamManagerInstance* pMan, u16 from, u16 to );


BOOL NNS_G2dGetNewOamManagerInstance
( 
    NNSG2dOamManagerInstance*   pMan, 
    u16                         fromOBJ, 
    u16                         numOBJ, 
    u16                         fromAffine, 
    u16                         numAffine, 
    NNSG2dOamType               type 
);

BOOL NNS_G2dGetNewOamManagerInstanceAsFastTransferMode
( 
    NNSG2dOamManagerInstance*   pMan, 
    u16                         fromOBJ, 
    u16                         numOBJ,
    NNSG2dOamType               type 
);


BOOL NNS_G2dEntryOamManagerOam
( 
    NNSG2dOamManagerInstance*   pMan, 
    const GXOamAttr*            pOam, 
    u16                         num 
);

BOOL NNS_G2dEntryOamManagerOamWithAffineIdx
( 
    NNSG2dOamManagerInstance*   pMan, 
    const GXOamAttr*            pOam, 
    u16                         affineIdx 
);

void NNS_G2dSetOamManagerAffine
( 
    NNSG2dOamType               type, 
    const MtxFx22*              mtx, 
    u16                         idx 
);

u16  NNS_G2dEntryOamManagerAffine( NNSG2dOamManagerInstance* pMan, const MtxFx22* mtx );

                        
void NNS_G2dApplyOamManagerToHW( NNSG2dOamManagerInstance* pMan );

void NNS_G2dApplyOamManagerToHWSprite
( 
    NNSG2dOamManagerInstance*       pMan, 
    const NNSG2dImageAttr*          pTexImageAttr,
    u32                             texBaseAddr,
    u32                             pltBaseAddr
);
                            
void NNS_G2dResetOamManagerBuffer( NNSG2dOamManagerInstance* pMan );

                         
void NNS_G2dApplyAndResetOamManagerBuffer( NNSG2dOamManagerInstance* pMan );
u16  NNS_G2dGetOamManagerOamCapacity( NNSG2dOamManagerInstance* pMan );
u16  NNS_G2dGetOamManagerAffineCapacity( NNSG2dOamManagerInstance* pMan );

GXOamAttr* NNS_G2dGetOamBuffer( NNSG2dOamType type );

//------------------------------------------------------------------------------
// inline 
//------------------------------------------------------------------------------
NNS_G2D_INLINE void NNS_G2dSetOamManagerSpriteZoffsetStep
( 
    NNSG2dOamManagerInstance*   pMan, 
    fx32                        zStep 
)
{
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_WARNING( zStep <=  0, "AutoZOffsetStep should be smaller than zero." );
    NNS_G2D_WARNING( pMan->type == NNS_G2D_OAMTYPE_SOFTWAREEMULATION, 
       "This function works only for the manager initialized as NNS_G2D_OAMTYPE_SOFTWAREEMULATION type ");
    
    pMan->spriteZoffsetStep = zStep;
}   

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_G2D_OAM_H_
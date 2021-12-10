/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d
  File:     g2d_CellTransferManager.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.10 $
 *---------------------------------------------------------------------------*/
 
#ifndef NNS_G2D_CELL_TRANSFER_MANAGER_H_
#define NNS_G2D_CELL_TRANSFER_MANAGER_H_

#include <nitro.h>
#include <nnsys/g2d/g2d_config.h>
#include <nnsys/g2d/g2d_Image.h>

#ifdef __cplusplus
extern "C" {
#endif

// 後に不要となる？
#include <nnsys/gfd/VramTransferMan/gfd_VramTransferManager.h>

#define NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE    (u32)0xFFFFFFFF


/*---------------------------------------------------------------------------*
  Name:         VramTransferTaskRegisterFuncPtr

  Description:  Vram 転送タスクの登録を行う関数へのポインタです。
                
 *---------------------------------------------------------------------------*/
typedef BOOL (*VramTransferTaskRegisterFuncPtr)( NNS_GFD_DST_TYPE  type, 
                                              u32               dstAddr, 
                                              void*             pSrc, 
                                              u32               szByte );

//------------------------------------------------------------------------------
//
// NNSG2dCellTransferState 
//
//------------------------------------------------------------------------------
typedef struct NNSG2dCellTransferState
{
    //
    // 初期化フェーズで設定されるメンバ
    //
    NNSG2dVRamLocation    dstVramLocation;      // 転送先の画像アドレス(各領域のサイズはszDst以上であること)
    u32                   szDst;                // 転送先の領域のサイズ
    
    const void*          pSrcNCGR;             // 転送元データ（キャラクタ方式）
    const void*          pSrcNCBR;             // 転送元データ（ビットマップ方式）
    u32                   szSrcData;            // 転送元データの大きさ（同一であること）
    BOOL                  bActive;              // アクティブな状態か？
    
    //
    // 毎フレーム更新されるメンバ
    //
    u32                   bDrawn;               // 描画されたか？
                                                // グラフィックスエンジンごとに描画されたか、どうかの状態が記憶されます。
                                                // 描画されないセルのVram転送を回避する目的で使用されます。
                                                // 描画モジュールによってセットされ
                                                // 管理モジュール によって 毎フレームリフレッシュされます   
                                                // ユーザが独自の描画モジュールを構築する場合は、本メンバを適切に設定する必要があります。
                                            
    u32                   bTransferRequested;   // 転送要求があったか？
                                                // グラフィックスエンジンごとに転送要求の状態が記憶されます。
                                                // セルアニメーション制御モジュールが設定します
                                                // 管理モジュール によって 転送タスク登録を完了した際にリセットされます。
    
    u32                   srcOffset;            // ソースオフセット（転送要求の詳細）
    u32                   szByte;               // 転送サイズ      （転送要求の詳細）
    
}NNSG2dCellTransferState;



//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void 
NNS_G2dInitCellTransferStateManager
( 
    NNSG2dCellTransferState*            pCellStateArray, 
    u32                                 numCellState,
    VramTransferTaskRegisterFuncPtr     pTaskRegisterFunc
);

//------------------------------------------------------------------------------
// ハンドル取得、返却
u32 
NNS_G2dGetNewCellTransferStateHandle();
void
NNS_G2dFreeCellTransferStateHandle( u32 handle );

//------------------------------------------------------------------------------
void NNS_G2dUpdateCellTransferStateManager();

//------------------------------------------------------------------------------
// 転送リクエスト関連
// セルアニメーション実体 からよばれる
// 
void NNS_G2dSetCellTransferStateRequested
( 
    u32                                 handle, 
    u32                                 srcOffset,
    u32                                 szByte
);







//------------------------------------------------------------------------------
// internal functions 
//------------------------------------------------------------------------------
NNSG2dCellTransferState* 
NNSi_G2dGetCellTransferState
( 
    u32 handle 
);

//------------------------------------------------------------------------------
void NNSi_G2dInitCellTransferState
( 
    u32                   handle,
    
    u32                   dstAddr3D,
    u32                   dstAddr2DMain,
    u32                   dstAddr2DSub,
    u32                   szDst,
    
    const void*          pSrcNCGR,
    const void*          pSrcNCBR,
    u32                   szSrcData
);


//------------------------------------------------------------------------------
// inline functions 
//------------------------------------------------------------------------------
// フラグ操作等のアクセサ：内部公開関数です。
//------------------------------------------------------------------------------
NNS_G2D_INLINE void 
NNSi_G2dSetCellTransferStateRequestFlag( NNSG2dCellTransferState* pState, NNS_G2D_VRAM_TYPE type, BOOL flag )
{
    pState->bTransferRequested = ( pState->bTransferRequested & ~( 0x1 << type ) ) | ( flag << type );
}

NNS_G2D_INLINE void 
NNSi_G2dSetVramTransferRequestFlag( u32 handle, NNS_G2D_VRAM_TYPE type, BOOL flag )
{
    NNSG2dCellTransferState* pState = NNSi_G2dGetCellTransferState( handle );
    
    NNSi_G2dSetCellTransferStateRequestFlag( pState, type, flag );
}


//------------------------------------------------------------------------------
NNS_G2D_INLINE BOOL 
NNSi_G2dGetCellTransferStateRequestFlag( const NNSG2dCellTransferState* pState, NNS_G2D_VRAM_TYPE type )
{
    return (BOOL)( pState->bTransferRequested & ( 0x1 << type ) );
}

NNS_G2D_INLINE BOOL 
NNSi_G2dGetVramTransferRequestFlag( u32 handle, NNS_G2D_VRAM_TYPE type )
{
    const NNSG2dCellTransferState* pState = NNSi_G2dGetCellTransferState( handle );
    return NNSi_G2dGetCellTransferStateRequestFlag( pState, type );
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE void 
NNSi_G2dSetCellTransferStateCellDrawnFlag( NNSG2dCellTransferState* pState, NNS_G2D_VRAM_TYPE type, BOOL flag )
{
    pState->bDrawn = ( pState->bDrawn & ~( 0x1 << type ) ) | ( flag << type );
}

NNS_G2D_INLINE void 
NNSi_G2dSetVramTransferCellDrawnFlag( u32 handle, NNS_G2D_VRAM_TYPE type, BOOL flag )
{
    NNSG2dCellTransferState* pState = NNSi_G2dGetCellTransferState( handle );
    NNSi_G2dSetCellTransferStateCellDrawnFlag( pState, type, flag );
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE BOOL 
NNSi_G2dGetCellTransferStateCellDrawnFlag( const NNSG2dCellTransferState* pState, NNS_G2D_VRAM_TYPE type )
{
    return (BOOL)( pState->bDrawn & ( 0x1 << type ) );
}

NNS_G2D_INLINE BOOL 
NNSi_G2dGetVramTransferCellDrawnFlag( u32 handle, NNS_G2D_VRAM_TYPE type )
{
    const NNSG2dCellTransferState* pState = NNSi_G2dGetCellTransferState( handle );
    return NNSi_G2dGetCellTransferStateCellDrawnFlag( pState, type );
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_G2D_CELL_TRANSFER_MANAGER_H_

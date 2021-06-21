/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g2d
  File:     g2d_CellTransferManager.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.16 $
 *---------------------------------------------------------------------------*/

#include <nnsys/g2d/g2d_CellTransferManager.h>
#include <nnsys/gfd/VramTransferMan/gfd_VramTransferManager.h>

//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
static NNSG2dCellTransferState*                s_pCellStateArray = NULL;
static u32                                     s_numCellState = 0;

VramTransferTaskRegisterFuncPtr                s_pTaskRegisterFunc = NULL;


//------------------------------------------------------------------------------
// マネージャの状態は不正ではないか
static NNS_G2D_INLINE BOOL IsCellTransferStateManagerValid_()
{
    return (BOOL)( ( s_pCellStateArray != NULL) && 
                   ( s_numCellState != 0 ) && 
                   ( s_pTaskRegisterFunc != NULL ) ) ;
}

//------------------------------------------------------------------------------
// ハンドルは有効か
static NNS_G2D_INLINE BOOL IsValidHandle_( u32 handle )
{
    NNS_G2D_ASSERT( IsCellTransferStateManagerValid_() );
    
    if( handle != NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE )
    {    
        if( handle < s_numCellState )
        {
            if( s_pCellStateArray[handle].bActive )
            {
                return TRUE; 
            }
        }
    }
    return FALSE;
}

//------------------------------------------------------------------------------
static NNS_G2D_INLINE NNSG2dCellTransferState*
GetValidCellTransferState_( u32 validHandle )
{
    NNS_G2D_ASSERT( IsValidHandle_( validHandle ) );
    return &s_pCellStateArray[validHandle];
}



//------------------------------------------------------------------------------
// Vram転送タスクとして登録する必要があるのか?
static NNS_G2D_INLINE BOOL ShouldRegisterAsVramTransferTask_
( 
    const NNSG2dCellTransferState*    pState,
    NNS_G2D_VRAM_TYPE                   type
)
{
    NNS_G2D_NULL_ASSERT( pState );
    
    return (BOOL)(  NNSi_G2dGetCellTransferStateRequestFlag( pState, type ) &&
                    NNSi_G2dGetCellTransferStateCellDrawnFlag( pState, type ) );
}

//------------------------------------------------------------------------------
// 描画属性をリセットします
static NNS_G2D_INLINE void ResetCellTransferStateDrawnFlag_
( 
    NNSG2dCellTransferState*  pState 
)
{
    pState->bDrawn = 0x0;
}



//------------------------------------------------------------------------------
// 転送元データを取得する
static NNS_G2D_INLINE const void* GetVramTransferSrc_
( 
    const NNSG2dCellTransferState*    pState,
    NNS_G2D_VRAM_TYPE                   type
)
{
    NNS_G2D_NULL_ASSERT( pState );
    
    if( type == NNS_G2D_VRAM_TYPE_3DMAIN )
    {
        return pState->pSrcNCBR;
    }else{
        return pState->pSrcNCGR;
    }
}

//------------------------------------------------------------------------------
// 転送元データは有効か？
static NNS_G2D_INLINE BOOL IsVramTransferSrcDataValid_
( 
    const NNSG2dCellTransferState*    pState,
    NNS_G2D_VRAM_TYPE                   type
)
{
    return (BOOL)( GetVramTransferSrc_( pState, type ) != NULL );
}



//------------------------------------------------------------------------------
// 転送の準備がととのっているか 
static NNS_G2D_INLINE BOOL IsCellTransferStateValid_
( 
    const NNSG2dCellTransferState*  pState, 
    NNS_G2D_VRAM_TYPE               type
)
{
    // 転送元のデータが設定されている
    // 転送元の領域が設定されている
    return (BOOL)( NNSi_G2dIsVramLocationReadyToUse( &pState->dstVramLocation, type ) && 
                   IsVramTransferSrcDataValid_( pState, type ) );
}

//------------------------------------------------------------------------------
// 列挙子の変換
// 現在のところはただのキャスト
static NNS_G2D_INLINE NNS_GFD_DST_TYPE 
ConvertVramType_( NNS_G2D_VRAM_TYPE type )
{
    NNS_G2D_MINMAX_ASSERT( type, NNS_G2D_VRAM_TYPE_3DMAIN, NNS_G2D_VRAM_TYPE_2DSUB );
    {
        const static NNS_GFD_DST_TYPE cvtTbl []=
        {
            NNS_GFD_DST_3D_TEX_VRAM,      // NNS_G2D_VRAM_TYPE_3DMAIN
            NNS_GFD_DST_2D_OBJ_CHAR_MAIN, // NNS_G2D_VRAM_TYPE_2DMAIN
            NNS_GFD_DST_2D_OBJ_CHAR_SUB   // NNS_G2D_VRAM_TYPE_2DSUB
        };
        return cvtTbl[type];
    }
}

//------------------------------------------------------------------------------
// 
// タスクの生成
// NNSG2dCellTransferState を入力として マネージャに登録するタスクを生成します
//
static NNS_G2D_INLINE BOOL MakeVramTransferTask_
(
    const NNSG2dCellTransferState*    pState, 
    NNS_G2D_VRAM_TYPE                   type
)
{    
    NNS_G2D_NULL_ASSERT( pState );
    
    NNS_G2D_ASSERT( IsCellTransferStateValid_( pState, type ) );
    
    //
    // Vram転送タスクの登録を外部モジュールに委譲します
    //
    return (*s_pTaskRegisterFunc )( ConvertVramType_( type ),
                                    NNSi_G2dGetVramLocation( &pState->dstVramLocation, type ),
                                    (u8*)GetVramTransferSrc_( pState, type ) + pState->srcOffset,
                                    pState->szByte );
}
//------------------------------------------------------------------------------
// NNSG2dCellTransferStateのリセット
static NNS_G2D_INLINE void ResetCellTransferState_( NNSG2dCellTransferState* pState )
{
    NNS_G2D_NULL_ASSERT( pState );
    {
        NNSi_G2dInitializeVRamLocation( &pState->dstVramLocation );
        pState->szDst = 0;
        pState->pSrcNCGR = NULL;
        pState->pSrcNCBR = NULL;
        pState->szSrcData = 0;
        pState->bActive = FALSE;
        
        
        pState->bDrawn              = 0x0;
        pState->bTransferRequested  = 0x0;
        pState->srcOffset           = 0x0;
        pState->szByte              = 0x0;
    }
}
//------------------------------------------------------------------------------
// ライブラリ内部公開
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
NNSG2dCellTransferState* 
NNSi_G2dGetCellTransferState
( 
    u32 handle 
)
{
    return GetValidCellTransferState_( handle );
}

//------------------------------------------------------------------------------
// 指定ハンドルの転送状態管理オブジェクトに転送請求を設定します。
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
)
{
    NNS_G2D_ASSERT( IsValidHandle_( handle ) );
    NNS_G2D_NON_ZERO_ASSERT( szDst );
    NNS_G2D_NON_ZERO_ASSERT( szSrcData );
    NNS_G2D_NULL_ASSERT( szDst );
    
    {
        NNSG2dCellTransferState* pState 
            = GetValidCellTransferState_( handle );
        
        NNS_G2D_NULL_ASSERT( pState );
        
        // どれかが有効であること
        NNS_G2D_ASSERT( dstAddr3D       != NNS_G2D_VRAM_ADDR_NONE   ||
                        dstAddr2DMain   != NNS_G2D_VRAM_ADDR_NONE   || 
                        dstAddr2DSub    != NNS_G2D_VRAM_ADDR_NONE    );
        {
            NNSG2dVRamLocation* pImg = &pState->dstVramLocation;
            
            NNSi_G2dInitializeVRamLocation( pImg );
            
            if( dstAddr3D != NNS_G2D_VRAM_ADDR_NONE )
            {
                NNSi_G2dSetVramLocation( pImg, NNS_G2D_VRAM_TYPE_3DMAIN, dstAddr3D );
            }
            
            if( dstAddr2DMain != NNS_G2D_VRAM_ADDR_NONE )
            {
                NNSi_G2dSetVramLocation( pImg, NNS_G2D_VRAM_TYPE_2DMAIN, dstAddr2DMain );
            }
            
            if( dstAddr2DSub != NNS_G2D_VRAM_ADDR_NONE )
            {
                NNSi_G2dSetVramLocation( pImg, NNS_G2D_VRAM_TYPE_2DSUB , dstAddr2DSub );
            }
        }
        
        // 両方NULLは明らかに有効ではない
        NNS_G2D_ASSERT( pSrcNCGR != NULL || pSrcNCBR != NULL );
        
        pState->szDst     = szDst;
        pState->pSrcNCGR  = pSrcNCGR;
        pState->pSrcNCBR  = pSrcNCBR;
        pState->szSrcData = szSrcData;
    }
}




//------------------------------------------------------------------------------
// 初期化関連
//------------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitCellTransferStateManager

  Description:  セルVRAM転送状態オブジェクトマネージャを初期化します。
                セルVRAM転送状態オブジェクトバッファを引数として渡します。
                pTaskRegisterFunc は VRAM転送タスク登録を請求する関数へのポインタです。
                
                
  Arguments:    pCellStateArray          [OUT] セルVRAM転送状態オブジェクトバッファ 
                numCellState             [IN]  セルVRAM転送状態オブジェクトバッファ長 
                pTaskRegisterFunc        [IN]  VRAM転送登録関数のポインタ 
                    
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void 
NNS_G2dInitCellTransferStateManager
( 
    NNSG2dCellTransferState*            pCellStateArray, 
    u32                                 numCellState,
    VramTransferTaskRegisterFuncPtr     pTaskRegisterFunc
)
{
    NNS_G2D_NULL_ASSERT( pCellStateArray );
    NNS_G2D_NON_ZERO_ASSERT( numCellState );
    NNS_G2D_NULL_ASSERT( pTaskRegisterFunc );
    
    
    s_pTaskRegisterFunc     = pTaskRegisterFunc;
    
    s_pCellStateArray       = pCellStateArray;
    s_numCellState          = numCellState;
    
    //
    // すべての NNSG2dCellTransferState をリセットする
    //
    {
        u32     i;
        for( i = 0; i < numCellState; i++ )
        {
            ResetCellTransferState_( &pCellStateArray[i] );
        }
    }
}


//------------------------------------------------------------------------------
// 転送関連
//------------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dUpdateCellTransferStateManager

  Description:  転送状態管理マネージャを更新します。
                
                毎フレームアニメ更新、描画請求が終了した後、VRAM転送の前に呼んでください。
                (呼び出す位置が重要です。)
                
                内部で登録されているすべての転送状態オブジェクトについて、
                VRAM転送の生成の必要があるかを判定し、必要ならばタスクの生成を行っています。
                
                
                
  Arguments:    なし
                    
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dUpdateCellTransferStateManager()
{
    u32 i;
    // 使用されている、すべてのVram転送状態オブジェクトについて...
    for( i = 0; i < s_numCellState; i++ )
    {
        NNS_G2D_VRAM_TYPE           type;
        NNSG2dCellTransferState*  pState                = &s_pCellStateArray[i];
        
        // アクティブか？
        if( pState->bActive )
        {
            //
            // すべての種類の描画HWについて...
            //
            for( type = NNS_G2D_VRAM_TYPE_3DMAIN; type < NNS_G2D_VRAM_TYPE_MAX; type++ )
            {
                //
                // 転送する必要があるなら...
                //
                
                if( ShouldRegisterAsVramTransferTask_( pState, type ) )
                {
                    
                    // マネージャから取得                   
                    // 生成                    
                    if( MakeVramTransferTask_( pState, type ) )
                    {
                        // OS_Printf( "VRAM transfer task is registered ! surface_Id = %d \n", type );
                        // 登録請求状態をリセット(転送が行われたVRAM種類だけ)
                        NNSi_G2dSetCellTransferStateRequestFlag( pState, type, FALSE );                    
                    }else{
                        // 登録に失敗した
                        // TODO ：警告？: アサート
                    }
                }
            }
            
            //
            // 描画状態にリセット(すべてのVRAM種類)
            //
            ResetCellTransferStateDrawnFlag_( pState );
        }
    }
}





//------------------------------------------------------------------------------
// 登録関連
//------------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetCellTransferStateRequested

  Description:  指定ハンドルの転送状態管理オブジェクトに転送請求を設定します。
                
                
  Arguments:    handle:         [IN]  転送状態管理オブジェクトのハンドル
             srcOffset:         [IN]  転送元データ先頭からの オフセット値
                szByte:         [IN]  転送サイズ
                    
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetCellTransferStateRequested
( 
    u32                                 handle,
    u32                                 srcOffset,
    u32                                 szByte
)
{
    NNS_G2D_ASSERT( IsValidHandle_( handle ) );
    
    {
        NNSG2dCellTransferState* pState 
            = NNSi_G2dGetCellTransferState( handle );
        
        NNS_G2D_NULL_ASSERT( pState );
        NNS_G2D_ASSERT( szByte <= pState->szDst );
        //
        // 転送請求状態に推移
        //
        pState->bTransferRequested    = 0xFFFFFFFF;// memo:ONにするときは一斉にONしかありえない
        pState->srcOffset             = srcOffset;
        pState->szByte                = szByte;       
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetNewCellTransferStateHandle

  Description:  セルVRAM転送状態オブジェクトのハンドルを取得します。
                セルVRAM転送状態オブジェクトへの操作はこのハンドルを使用して行います。
                ハンドルはセルアニメーション実体のメンバとして保存されます。
                ハンドルの取得に失敗した場合は NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE を返します。
                内部でセルVRAM転送状態オブジェクトバッファを線形探索しています。
                （パフォーマンスクリティカルな場所での呼びだしは避けてください。）

                
                
  Arguments:    なし
                    
  Returns:      セルVRAM転送状態オブジェクトのハンドル
                ハンドルの取得に失敗した場合は NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE を返します。
  
 *---------------------------------------------------------------------------*/
u32 
NNS_G2dGetNewCellTransferStateHandle()
{
    NNS_G2D_ASSERT( IsCellTransferStateManagerValid_() );
    
    
    //
    // 使用されていない転送状態オブジェクトを配列の先頭から検索します
    // 
    {
        u32 i = 0;
        for( i = 0;i < s_numCellState; i++ )
        {
            if( s_pCellStateArray[ i ].bActive != TRUE )
            {
                s_pCellStateArray[ i ].bActive = TRUE;
                return i;
            }
        }
    }    
    
    // 発見できなかった
    return NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFreeCellTransferStateHandle

  Description:  セルVRAM転送状態オブジェクトのハンドルを返却します。
                使用しなくなったハンドルに対して実行してください。


                
                
  Arguments:    handle              [IN] セルVRAM転送状態オブジェクトのハンドル
                    
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void
NNS_G2dFreeCellTransferStateHandle( u32 handle )
{
    NNS_G2D_ASSERT( IsValidHandle_( handle ) );
    NNS_G2D_ASSERT( IsCellTransferStateManagerValid_() );
    
    ResetCellTransferState_( GetValidCellTransferState_( handle ) );    
}

   
    

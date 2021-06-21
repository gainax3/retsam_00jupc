/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - gfd - src - VramManager
  File:     gfd_FrameTexVramMan.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.20 $
 *---------------------------------------------------------------------------*/

#include <nnsys/gfd/gfd_common.h>
#include <nnsys/gfd/VramManager/gfd_VramMan.h>
#include <nnsys/gfd/VramManager/gfd_FrameTexVramMan.h>

//------------------------------------------------------------------------------
#define NNS_GFD_TEX_SLOT_SIZE               0x20000

#define NNS_GFD_NUM_TEX_VRAM_REGION         5
#define NNS_GFD_NUM_TEX_VRAM_REGION_4x4     2
#define NNS_GFD_INVALID_ADDR                0xFFFFFFFF

//------------------------------------------------------------------------------
typedef struct NNSGfdFrmTexRegionState
{
    u32           head;
    u32           tail;
    
    BOOL          bActive;
    
    const BOOL    bHalfSize;        // TODO:直接サイズを保持するように変更の予定
    
    const u16     index;
    const u16     pad16_;
    const u32     baseAddress;
    
}NNSGfdFrmTexRegionState;

//------------------------------------------------------------------------------
typedef struct NNSGfdFrmTexVramMnager
{
    u16     numSlot;

} NNSGfdFrmTexVramMnager;


static NNSGfdFrmTexVramMnager frmExVramMan_;

//------------------------------------------------------------------------------

static NNSGfdFrmTexRegionState  vramRegions_[NNS_GFD_NUM_TEX_VRAM_REGION] = 
{
    { NNS_GFD_INVALID_ADDR, NNS_GFD_INVALID_ADDR, FALSE, FALSE, 0, 0xFFFF, 0x0                                               },
    { NNS_GFD_INVALID_ADDR, NNS_GFD_INVALID_ADDR, FALSE, TRUE,  1, 0xFFFF, (u32)NNS_GFD_TEX_SLOT_SIZE                             },
    { NNS_GFD_INVALID_ADDR, NNS_GFD_INVALID_ADDR, FALSE, TRUE,  2, 0xFFFF, (u32)(NNS_GFD_TEX_SLOT_SIZE + NNS_GFD_TEX_SLOT_SIZE / 2) },
    { NNS_GFD_INVALID_ADDR, NNS_GFD_INVALID_ADDR, FALSE, FALSE, 3, 0xFFFF, (u32)(NNS_GFD_TEX_SLOT_SIZE * 2)                         },
    { NNS_GFD_INVALID_ADDR, NNS_GFD_INVALID_ADDR, FALSE, FALSE, 4, 0xFFFF, (u32)(NNS_GFD_TEX_SLOT_SIZE * 3)                         }
};

//------------------------------------------------------------------------------
static NNSGfdFrmTexRegionState* texNrmSearchArray_[NNS_GFD_NUM_TEX_VRAM_REGION] =
{
    &vramRegions_[4],
    &vramRegions_[3],
    &vramRegions_[0],
    &vramRegions_[2],
    &vramRegions_[1]
};

static NNSGfdFrmTexRegionState* tex4x4SearchArray_[NNS_GFD_NUM_TEX_VRAM_REGION_4x4] =
{
    &vramRegions_[0],
    &vramRegions_[3]
};


//------------------------------------------------------------------------------
// NNS_GfdDumpFrmTexVramManager()内で使用される、規定のデバックコンテキスト情報
// 
typedef struct NNSiGfdDefaultDebugContext
{
    u32     szTotalFree;
    u32     szTotalReserved;
    
}NNSiGfdDefaultDebugContext;



//------------------------------------------------------------------------------
// スロット数は適切か？
static NNS_GFD_INLINE BOOL IsSlotNumValid_( u16 numSlot )
{
    if( numSlot > 0 && numSlot <= 4 )
    {
        return TRUE;
    }else{
        return FALSE;
    }
}


//------------------------------------------------------------------------------
static NNS_GFD_INLINE void ResetRegionNormal_( NNSGfdFrmTexRegionState* pRegion )
{
    NNS_GFD_NULL_ASSERT( pRegion ); 
    
    pRegion->head = 0x0;
    pRegion->tail = NNS_GFD_TEX_SLOT_SIZE;
}
//------------------------------------------------------------------------------
static NNS_GFD_INLINE void ResetRegionHalf_( NNSGfdFrmTexRegionState* pRegion )
{
    NNS_GFD_NULL_ASSERT( pRegion ); 
    
    pRegion->head = 0x0;
    pRegion->tail = NNS_GFD_TEX_SLOT_SIZE / 2;
}



//------------------------------------------------------------------------------
static NNS_GFD_INLINE BOOL IsRegionStateValid_( const NNSGfdFrmTexRegionState* pRegion )
{
    NNS_GFD_NULL_ASSERT( pRegion ); 
    
    if( pRegion->head != NNS_GFD_INVALID_ADDR && 
        pRegion->tail != NNS_GFD_INVALID_ADDR )
    {
        if( pRegion->head <= pRegion->tail ) 
        {
            return TRUE;
        }
    }
    
    return FALSE;
}

//------------------------------------------------------------------------------
// マネージャが正常な状態か調査します
static NNS_GFD_INLINE BOOL IsVramManagerValid_( )
{
    int i;
    
    for( i = 0; i < NNS_GFD_NUM_TEX_VRAM_REGION; i++ )
    {
        if( !IsRegionStateValid_( &vramRegions_[i] ) )
        {
            return FALSE;
        }
    }
    return TRUE;
}

//------------------------------------------------------------------------------
static NNS_GFD_INLINE u32 GetReagionCapacity_( const NNSGfdFrmTexRegionState* pRegion )
{
    NNS_GFD_NULL_ASSERT( pRegion ); 
    NNS_GFD_ASSERT( IsRegionStateValid_( pRegion ) );
    
    return (u32)( pRegion->tail - pRegion->head );
}

//------------------------------------------------------------------------------
static NNS_GFD_INLINE u32 AllocFromReagionHead_( NNSGfdFrmTexRegionState* pRegion, u32 szByte )
{
    NNS_GFD_NULL_ASSERT( pRegion ); 
    NNS_GFD_ASSERT( IsRegionStateValid_( pRegion ) );
    NNS_GFD_ASSERT( GetReagionCapacity_( pRegion ) >= szByte );
    
    {
        const u32 result = pRegion->head;
    
        pRegion->head += szByte;
    
        return result;
    }
}

//------------------------------------------------------------------------------
static NNS_GFD_INLINE u32 AllocFromReagionTail_( NNSGfdFrmTexRegionState* pRegion, u32 szByte )
{
    NNS_GFD_NULL_ASSERT( pRegion ); 
    NNS_GFD_ASSERT( IsRegionStateValid_( pRegion ) );
    NNS_GFD_ASSERT( GetReagionCapacity_( pRegion ) >= szByte );
    
    {
        pRegion->tail -= szByte;
        
        return pRegion->tail;
    }
}




//------------------------------------------------------------------------------
static NNS_GFD_INLINE NNSGfdFrmTexRegionState* Get4x4IdxRegion_( const NNSGfdFrmTexRegionState* pRegion )
{
    NNS_GFD_NULL_ASSERT( pRegion );
    switch( pRegion->index )
    {
        case 0:return &vramRegions_[1];
        case 3:return &vramRegions_[2];
        // NG
        default:NNS_GFD_WARNING("Invalid region is detected. in Get4x4IdxRegion_(). ");break;
    }
    return NULL;
}

//------------------------------------------------------------------------------
// 4x4圧縮テクスチャ領域の確保
static NNS_GFD_INLINE BOOL DoAlloc4x4_( u32 szByte, u32* addr )
{
    NNS_GFD_NULL_ASSERT( addr );
    NNS_GFD_ASSERTMSG( IsVramManagerValid_(), "Make sure to Initialize the manager." );
    
    {
        int i;
        NNSGfdFrmTexRegionState* pRegion = NULL;
        NNSGfdFrmTexRegionState* pPltRegion = NULL;
        
        for( i = 0; i < NNS_GFD_NUM_TEX_VRAM_REGION_4x4; i++ )
        {        
            pRegion = tex4x4SearchArray_[i];
            
            //
            // 利用可能でかつ、空き容量があるなら...
            //
            if( pRegion->bActive && 
                GetReagionCapacity_( pRegion ) >= szByte )
            {
                pPltRegion = Get4x4IdxRegion_( pRegion );
                
                // インデックステーブル用領域について
                //
                // 利用可能でかつ、空き容量があるなら...
                //
                if( pPltRegion->bActive && 
                    GetReagionCapacity_( pPltRegion ) >= szByte / 2 )
                {
                    *addr = AllocFromReagionHead_( pRegion, szByte );
                    (void)AllocFromReagionHead_( pPltRegion, szByte / 2 );
                    
                    *addr += pRegion->baseAddress;
                    
                    return TRUE; 
                }
            }
        }
        
        //
        // NG TODO:msg 
        //
        NNS_GFD_WARNING("We have no memory for 4x4 Tex.");
        return FALSE;
    }
}



//------------------------------------------------------------------------------
// 通常のテクスチャ領域の確保
static NNS_GFD_INLINE BOOL DoAllocNrm_( u32 szByte, u32* addr )
{
    NNS_GFD_NULL_ASSERT( addr );
    NNS_GFD_ASSERTMSG( IsVramManagerValid_(), "Make sure to Initialize the manager." );
    
    {
        int i;
        NNSGfdFrmTexRegionState* pRegion = NULL;
        
        for( i = 0; i < NNS_GFD_NUM_TEX_VRAM_REGION; i++ )
        {        
            pRegion = texNrmSearchArray_[i];
            if( pRegion->bActive )
            {
                if( GetReagionCapacity_( pRegion ) >= szByte )
                {
                    *addr = AllocFromReagionTail_( pRegion, szByte );    
                    
                    *addr += pRegion->baseAddress;
                    
                    return TRUE; 
                }
            }
        }
        //
        // NG TODO:msg 
        //
        NNS_GFD_WARNING("We have no memory for Nrm Tex.");
        return FALSE;
    }
}


//------------------------------------------------------------------------------
// NNS_GfdDumpFrmTexVramManager()内で使用される、規定のデバックダンプ関数実装です。
// 
// OS_Printfで各種情報を出力します。
static void DefaultDebugDumpFunc_( 
    int index, 
    u32 startAddr, 
    u32 endAddr, 
    u32 blockMax, 
    BOOL bActive, 
    void* pUserContext 
)
{

#ifdef SDK_FINALROM
    #pragma unused(index)
#endif // SDK_FINALROM

    NNSiGfdDefaultDebugContext*        pContext = (NNSiGfdDefaultDebugContext*)pUserContext;
    NNS_GFD_NULL_ASSERT( pContext );
    
    
    if( bActive )
    {
        OS_Printf("%02d    : 0x%08x  : 0x%08x  : 0x%08x  \n", 
            index, 
            startAddr, 
            endAddr, 
            endAddr - startAddr );
        
        pContext->szTotalFree      += endAddr - startAddr;
        pContext->szTotalReserved  += blockMax;            
    }else{
        OS_Printf("%02d    : ----------  : ----------  : ----------  \n", index );
    }
}

//------------------------------------------------------------------------------
// 通常テクスチャの検索順を設定する
void NNSi_GfdSetTexNrmSearchArray
( 
    int idx1st, 
    int idx2nd,
    int idx3rd,
    int idx4th,
    int idx5th
)
{
    
    NNS_GFD_MINMAX_ASSERT( idx1st, 0, NNS_GFD_NUM_TEX_VRAM_REGION - 1);
    NNS_GFD_MINMAX_ASSERT( idx2nd, 0, NNS_GFD_NUM_TEX_VRAM_REGION - 1);
    NNS_GFD_MINMAX_ASSERT( idx3rd, 0, NNS_GFD_NUM_TEX_VRAM_REGION - 1);
    NNS_GFD_MINMAX_ASSERT( idx4th, 0, NNS_GFD_NUM_TEX_VRAM_REGION - 1);
    NNS_GFD_MINMAX_ASSERT( idx5th, 0, NNS_GFD_NUM_TEX_VRAM_REGION - 1);
    
    texNrmSearchArray_[0] = &vramRegions_[idx1st];
    texNrmSearchArray_[1] = &vramRegions_[idx2nd];
    texNrmSearchArray_[2] = &vramRegions_[idx3rd];
    texNrmSearchArray_[3] = &vramRegions_[idx4th];
    texNrmSearchArray_[4] = &vramRegions_[idx5th];
}

/*---------------------------------------------------------------------------*
  Name:         NNS_GfdDumpFrmTexVramManager

  Description:  マネージャの内容をデバック出力します。
                
                 
  Arguments:   なし

                            
  Returns:     なし
  
 *---------------------------------------------------------------------------*/
void NNS_GfdDumpFrmTexVramManager()
{
    const NNSGfdFrmTexRegionState*  pState          = NULL;
    
    NNSiGfdDefaultDebugContext         context = { 0, 0 };
    
    OS_Printf("===== NNS_Gfd FrmTexVramManager Dump =====\n");
    OS_Printf("index : head-Addr   : tail-addr   : free-size \n");
    
    // すべてのRegionStateについて、コールバック関数を呼び出します。
    NNS_GfdDumpFrmTexVramManagerEx( DefaultDebugDumpFunc_, &context );
    
    OS_Printf("    %08d / %08d bytes (%6.2f%%) used \n", 
                context.szTotalReserved - context.szTotalFree, 
                context.szTotalReserved, 
                100.0f * ( context.szTotalReserved - context.szTotalFree ) / context.szTotalReserved ); 
                
    OS_Printf("==========================================\n");
}

/*---------------------------------------------------------------------------*
  Name:         NNS_GfdDumpFrmTexVramManagerEx

  Description:  マネージャの内容をデバック出力します。
                
                 
  Arguments:   pFunc            デバックダンプコールバック関数
               pUserContext     ユーザ情報(トラバース処理のコンテキスト情報用)

                            
  Returns:     なし
  
 *---------------------------------------------------------------------------*/
void NNS_GfdDumpFrmTexVramManagerEx( NNSGfdFrmTexVramDebugDumpCallBack pFunc, void* pUserContext )
{
    int i;
    const NNSGfdFrmTexRegionState*  pState          = NULL;
    NNS_GFD_NULL_ASSERT( pFunc );
    
    for( i = 0; i < NNS_GFD_NUM_TEX_VRAM_REGION; i++ )
    {
        pState = &vramRegions_[i];
        (*pFunc)( i, 
        pState->head + pState->baseAddress, 
        pState->tail + pState->baseAddress,
        (pState->bHalfSize) ? (u32)(NNS_GFD_TEX_SLOT_SIZE / 2) : NNS_GFD_TEX_SLOT_SIZE,
        pState->bActive,
        pUserContext );
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_GfdInitFrmTexVramManager

  Description:  マネージャの初期化
                
                 
  Arguments:   numSlot      フレームテクスチャVRAMマネージャが管理するVRAMスロットの数
               useAsDefault フレームテクスチャVRAMマネージャをデフォルトとして使用するかどうか

                            
  Returns:     なし
  
 *---------------------------------------------------------------------------*/
void NNS_GfdInitFrmTexVramManager( u16 numSlot, BOOL useAsDefault )
{
    NNS_GFD_ASSERT( IsSlotNumValid_( numSlot ) );
    
    //
    // slot 2 ( Region 3 ) が使用されない場合は、
    // リージョン2 は 4x4テクスチャ用のインデックステーブルとして使用されない。
    // この場合は、リージョン2をリージョン0よりも優先して使用するべきなので、
    // そのように検索テーブルを再設定することとする。
    //
    if( numSlot <= 2 )
    {
        // ( 4, 3, 0, 2, 1 ) => ( 4, 3, 2, 0, 1 )
        NNSi_GfdSetTexNrmSearchArray( 4, 3, 2, 0, 1 ); 
    }else{
        // ( 4, 3, 2, 0, 1 ) => ( 4, 3, 0, 2, 1 )
        NNSi_GfdSetTexNrmSearchArray( 4, 3, 0, 2, 1 ); 
    }
    
    frmExVramMan_.numSlot = numSlot;
    NNS_GfdResetFrmTexVramState();

    if (useAsDefault)
    {
        NNS_GfdDefaultFuncAllocTexVram = NNS_GfdAllocFrmTexVram;
        NNS_GfdDefaultFuncFreeTexVram  = NNS_GfdFreeFrmTexVram;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_GfdResetFrmTexVramState

  Description:  マネージャの現在の状態をリセットします。
                 
  Arguments:   なし
                            
  Returns:     なし
  
 *---------------------------------------------------------------------------*/
void NNS_GfdResetFrmTexVramState()
{
    int i;
    u16 numSlot = frmExVramMan_.numSlot;
    //
    // 1 番スロットは 2つのRegionとして管理されます
    //
    const numRegion = ( numSlot > 1 ) ? numSlot + 1 : numSlot + 0;
    
    NNS_GFD_ASSERT( IsSlotNumValid_( numSlot ) );
    
    for( i = 0; i < NNS_GFD_NUM_TEX_VRAM_REGION; i++ )
    {
        //
        //
        //
        if( i < numRegion )
        {
            vramRegions_[i].bActive = TRUE;
        }else{
            vramRegions_[i].bActive = FALSE;
        }   
        
        //
        // 
        //
        if( vramRegions_[i].bHalfSize )
        {
            ResetRegionHalf_( &vramRegions_[i] );
        }else{
            ResetRegionNormal_( &vramRegions_[i] );
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_GfdAllocFrmTexVram

  Description:  メモリの確保
                
                 
  Arguments:   szByte           :  サイズ
               is4x4comp        :  4x4圧縮テクスチャか？
               opt              :  実装依存パラメータ（使用しません）   
                            
  Returns:     テクスチャキー
  
 *---------------------------------------------------------------------------*/
NNSGfdTexKey NNS_GfdAllocFrmTexVram
(
    u32     szByte, 
    BOOL    is4x4comp, 
    u32     opt 
)
{
#pragma unused(opt)

    u32     addr;
    BOOL    result;
    
    NNS_GFD_ASSERTMSG( IsVramManagerValid_(), "Make sure to Initialize the manager." );
    
    {
        //
        // テクスチャキーで表現できない小さなサイズの確保の場合、サイズを切り上げて確保します。
        //
        szByte = NNSi_GfdGetTexKeyRoundupSize( szByte );
        //
        // テクスチャキーが表現できないほど大きなサイズの確保の場合、エラーキーを返します。
        //
        if( szByte >= NNS_GFD_TEXSIZE_MAX )
        {
            NNS_GFD_WARNING("Allocation size is too big. : NNS_GfdAllocFrmTexVram()");
            return NNS_GFD_ALLOC_ERROR_TEXKEY;
        }
        
        NNS_GFD_MINMAX_ASSERT( szByte, NNS_GFD_TEXSIZE_MIN, NNS_GFD_TEXSIZE_MAX );
    }
        
    //
    // 確保を試みる
    //
    if( is4x4comp )
    {
        result = DoAlloc4x4_( szByte, &addr );
    }else{
        result = DoAllocNrm_( szByte, &addr );
    }
    
    
    //
    // 確保成功ならば、キーへと変換する
    //
    if( result )
    {
        return NNS_GfdMakeTexKey( addr, szByte, is4x4comp );
    }else{
        // エラー：エラーを表現するTexKeyをかえす
        NNS_GFD_WARNING("failure in Vram Allocation. : NNS_GfdAllocFrmTexVram()");
        return NNS_GFD_ALLOC_ERROR_TEXKEY;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_GfdFreeFrmTexVram

  Description:  メモリの開放
                実際には何もしません。
                 
  Arguments:   texKey          :   テクスチャキー
                            
  Returns:     成否（0 ならば 成功)
  
 *---------------------------------------------------------------------------*/
int NNS_GfdFreeFrmTexVram( NNSGfdTexKey texKey )
{
#pragma unused(texKey)
    
    NNS_GFD_ASSERTMSG( IsVramManagerValid_(), "Make sure to Initialize the manager." );
    
    // どの領域から、確保されたメモリか？
    
    // Lo Head( 4x4 )の場合 
    //        => inndexTableも同時開放する
    
    // Lo Tail( Nrm )
    // Hi Tail( Nrm )
    return 0;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_GfdGetFrmTexVramState

  Description:  マネージャの現在の状態を取得します。
                
                 
  Arguments:   pState          :   マネージャ状態情報
                            
  Returns:     なし
  
 *---------------------------------------------------------------------------*/
void NNS_GfdGetFrmTexVramState( NNSGfdFrmTexVramState* pState )
{
    int i;
    NNS_GFD_NULL_ASSERT( pState );
    NNS_GFD_ASSERTMSG( IsVramManagerValid_(), "Make sure to Initialize the manager." );
    
    for( i = 0; i < NNS_GFD_NUM_TEX_VRAM_REGION; i++ )
    {
        pState->address[i * 2 + 0] = vramRegions_[i].head;
        pState->address[i * 2 + 1] = vramRegions_[i].tail;
    }
}


/*---------------------------------------------------------------------------*
  Name:         NNS_GfdSetFrmTexVramState

  Description:  マネージャの状態に 指定した 状態情報 を設定します。
                 
  Arguments:   pState          :   マネージャ状態情報
                            
  Returns:     なし
  
 *---------------------------------------------------------------------------*/
void NNS_GfdSetFrmTexVramState( const NNSGfdFrmTexVramState* pState )
{
    int i;
    NNS_GFD_NULL_ASSERT( pState );
    NNS_GFD_ASSERTMSG( IsVramManagerValid_(), "Make sure to Initialize the manager." );
    
    for( i = 0; i < NNS_GFD_NUM_TEX_VRAM_REGION; i++ )
    {
        vramRegions_[i].head = pState->address[i * 2 + 0];
        vramRegions_[i].tail = pState->address[i * 2 + 1];
    }
}


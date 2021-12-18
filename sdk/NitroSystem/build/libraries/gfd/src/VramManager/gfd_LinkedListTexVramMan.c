/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - gfd - src - VramManager
  File:     gfd_LinkedListTexVramMan.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.16 $
 *---------------------------------------------------------------------------*/
#include <nnsys/gfd/gfd_common.h>
#include <nnsys/gfd/VramManager/gfd_LinkedListTexVramMan.h>
#include "gfdi_LinkedListVramMan_Common.h"

#define GFD_SLOT_SIZE        0x20000
#define GFD_SLOT0_BASEADDR   0x00000
#define GFD_SLOT1_BASEADDR   0x20000
#define GFD_SLOT2_BASEADDR   0x40000 
#define GFD_SLOT3_BASEADDR   0x60000 

#define NNS_GFD_LNK_FREE_ERROR_INVALID_SIZE 2

//
// マネージャ
//
typedef struct NNS_GfdLnkTexVramManager
{
    NNSiGfdLnkVramMan       mgrNrm;
    NNSiGfdLnkVramMan       mgr4x4;
    
    NNSiGfdLnkVramBlock*    pBlockPoolList;
    
    //
    // リセット時に使用するメンバ
    //
    u32                     szByte;
    u32                     szByteFor4x4;
    NNSiGfdLnkVramBlock*    pWorkHead;
    u32                     szByteManagementWork;

}NNS_GfdLnkTexVramManager;


typedef struct SlotData
{
    u32     szFree; // 空き領域サイズ
    u32     szNrm;  // 通常テクスチャ用サイズ
    u32     sz4x4;  // 4x4圧縮テクスチャ用サイズ

}SlotData;


static NNS_GfdLnkTexVramManager         mgr_;

//------------------------------------------------------------------------------
// デバック用関数(アサート分の中にのみ記述される関数です。内部公開限定関数です。)
//------------------------------------------------------------------------------
static u32 Dbg_GetVramManTotalFreeBlockSize_( const NNSiGfdLnkVramMan* pMgr )
{
    u32   total = 0;
    const NNSiGfdLnkVramBlock* pBlk = pMgr->pFreeList;
    while( pBlk )
    {
        total   += pBlk->szByte;
        pBlk    = pBlk->pBlkNext;
    }
    return total;
}

//------------------------------------------------------------------------------
// 初期化サイズパラメータが有効か？
static BOOL Dbg_IsInitializeSizeParamsValid_( u32 szByte, u32 szByteFor4x4 )
{
    //
    // サイズが不正ではないか？
    //
    if( szByte > 0 && szByteFor4x4 <= GFD_SLOT_SIZE * 2 )
    {   
        //
        // 4x4用のサイズ指定がある場合
        //
        if( szByteFor4x4 > 0 )
        {   
            // サイズが 0x20000 以下の場合
            if( szByteFor4x4 <= GFD_SLOT_SIZE )
            {
                // インデックステーブル分が確保可能なサイズが必須となる
                return (BOOL)(szByte >= GFD_SLOT1_BASEADDR + szByteFor4x4 / 2);
            }else{
                // インデックステーブル分が確保可能なサイズが必須となる
                // GFD_SLOT_SIZE は インデックステーブル として使用される Slot 1 のサイズ
                return (BOOL)( szByte >= szByteFor4x4 + GFD_SLOT_SIZE );
            }
        }else{
            // 最大サイズ制限をこえていないか？
            return (BOOL)( szByte <= GFD_SLOT_SIZE * 4 );
        }
    }else{
        return FALSE;
    }
}

//------------------------------------------------------------------------------
// 
static NNS_GFD_INLINE BOOL InitSlotFreeBlock_
( 
    NNSiGfdLnkVramMan*      pMgr, 
    NNSiGfdLnkVramBlock**   pPoolList, 
    u32                     baseAddr, 
    u32                     size 
)
{
    NNS_GFD_NULL_ASSERT( pMgr );
    NNS_GFD_NULL_ASSERT( pPoolList );

    NNS_GFD_ASSERT( GFD_SLOT_SIZE >= size );
    
    {
        BOOL result = TRUE;
        
        if( size > 0 )
        {
            result &= NNSi_GfdAddNewFreeBlock( pMgr, pPoolList, baseAddr, size );
        }
        return result;
    }
}

//------------------------------------------------------------------------------
// マネージャの内部状態をデバック出力します。
void NNS_GfdDumpLnkTexVramManager()
{
    OS_Printf("=== NNS_Gfd LnkTexVramManager Dump ====\n");
    OS_Printf("   address:        size    \n");   // ヘッダー行
    OS_Printf("=======================================\n");
    //
    // 通常テクスチャのフリーリストをすべて表示し、使用量の総和を計算します。
    //
    OS_Printf("------ Normal Texture Free Blocks -----\n");   
    NNSi_GfdDumpLnkVramManFreeListInfo( mgr_.mgrNrm.pFreeList, mgr_.szByte );
        
        
    //
    // 4x4テクスチャのフリーリストをすべて表示し、使用量の総和を計算します。
    //
    OS_Printf("------ 4x4    Texture Free Blocks -----\n");   
    if( mgr_.szByteFor4x4 != 0 )
    {
        NNSi_GfdDumpLnkVramManFreeListInfo( mgr_.mgr4x4.pFreeList, mgr_.szByteFor4x4 );
    }
    OS_Printf("=======================================\n");   
}

/*---------------------------------------------------------------------------*
  Name:         NNS_GfdDumpLnkTexVramManagerEx

  Description:  デバック出力処理関数を指定して、
                フリーブロック情報をデバック出力します。
                
  Arguments:    pFuncForNrm            : デバック出力処理関数(通常テクスチャ用)
                pFuncFor4x4            : デバック出力処理関数(4ｘ4圧縮テクスチャ用)
                pUserData              : デバック出力処理関数に引数として渡される、
                                         デバック出力処理用データ
               
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_GfdDumpLnkTexVramManagerEx( 
    NNSGfdLnkDumpCallBack           pFuncForNrm, 
    NNSGfdLnkDumpCallBack           pFuncFor4x4, 
    void*                           pUserData )
{
    NNS_GFD_ASSERT( pFuncForNrm != NULL || pFuncFor4x4 != NULL );
    //
    // 通常テクスチャのフリーリスト
    //
    if( pFuncForNrm != NULL )
    {
        NNSi_GfdDumpLnkVramManFreeListInfoEx( mgr_.mgrNrm.pFreeList, pFuncForNrm, pUserData );   
    }
    
    //
    // 4x4テクスチャのフリーリスト
    //
    if( mgr_.szByteFor4x4 != 0 && pFuncFor4x4 != NULL )
    {
        NNSi_GfdDumpLnkVramManFreeListInfoEx( mgr_.mgr4x4.pFreeList, pFuncFor4x4, pUserData );
    }
}


//------------------------------------------------------------------------------
u32 NNS_GfdGetLnkTexVramManagerWorkSize( u32 numMemBlk )
{
    return numMemBlk * sizeof( NNSiGfdLnkVramBlock );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_GfdInitLnkTexVramManager

  Description:  テクスチャキーからVRAM中のテクスチャ領域を開放します。
                
  Arguments:    szByte                  管理するVRAM領域のバイトサイズ。
                                        (1Slot = 0x20000,最大4Slotとして計算する) 
                szByteFor4x4            管理領域中の4x4圧縮テクスチャに使用する領域のバイトサイズ。
                                        (1Slot = 0x20000,最大2Slotとして計算する) 
                pManagementWork         管理情報として使用するメモリ領域へのポインタ。 
                szByteManagementWork    管理情報領域のサイズ。 
                useAsDefault            リンクドリストテクスチャVRAMマネージャをカレントの
                                        マネージャとして使用するかどうか。 

                
               
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_GfdInitLnkTexVramManager
( 
    u32     szByte, 
    u32     szByteFor4x4,
    void*   pManagementWork, 
    u32     szByteManagementWork,
    BOOL    useAsDefault
)
{
    NNS_GFD_ASSERT( Dbg_IsInitializeSizeParamsValid_( szByte, szByteFor4x4 ) );
    
    NNS_GFD_NULL_ASSERT( pManagementWork );
    NNS_GFD_ASSERT( szByteManagementWork != 0 );
    
    {
        mgr_.szByte         = szByte;                                                         
        mgr_.szByteFor4x4   = szByteFor4x4;
        mgr_.pWorkHead      = pManagementWork;
        mgr_.szByteManagementWork = szByteManagementWork;
        
        
        NNS_GfdResetLnkTexVramState();
        
        //
        // 初期化処理で生成された、フリーブロックの総容量が正しいものか確認します
        //
        NNS_GFD_ASSERT( mgr_.szByte - ( mgr_.szByteFor4x4 + mgr_.szByteFor4x4 / 2 ) 
            == Dbg_GetVramManTotalFreeBlockSize_( &mgr_.mgrNrm ) );
        NNS_GFD_ASSERT( mgr_.szByteFor4x4 
            == Dbg_GetVramManTotalFreeBlockSize_( &mgr_.mgr4x4 ) );

        
        //
        // デフォルトのアロケータとして使用
        //
        if( useAsDefault )
        {
            NNS_GfdDefaultFuncAllocTexVram = NNS_GfdAllocLnkTexVram;
            NNS_GfdDefaultFuncFreeTexVram  = NNS_GfdFreeLnkTexVram;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_GfdAllocLnkTexVram

  Description:  テクスチャ領域をVRAMから確保します。
                
  Arguments:    szByte       : 確保する領域バイト数
                is4x4comp    : 4x4圧縮テクスチャか？
                opt          : オプション（使用されません）
                
               
  Returns:      テクスチャキー
                確保に失敗した場合は、エラーを示すキーであるNNS_GFD_ALLOC_ERROR_TEXKEYを返します。

  
 *---------------------------------------------------------------------------*/
NNSGfdTexKey    NNS_GfdAllocLnkTexVram( u32 szByte, BOOL is4x4comp, u32 opt )
{
#pragma unused(opt)
    u32     addr;
    BOOL    result;
    
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
            NNS_GFD_WARNING("Allocation size is too big. : NNS_GfdAllocLnkTexVram()");
            return NNS_GFD_ALLOC_ERROR_TEXKEY;
        }
        
        NNS_GFD_MINMAX_ASSERT( szByte, NNS_GFD_TEXSIZE_MIN, NNS_GFD_TEXSIZE_MAX );
    }
    
    
    if( is4x4comp )
    {
        result = NNSi_GfdAllocLnkVram( &mgr_.mgr4x4, &mgr_.pBlockPoolList, &addr, szByte );
    }else{
        result = NNSi_GfdAllocLnkVram( &mgr_.mgrNrm, &mgr_.pBlockPoolList, &addr, szByte );
    }
    
        
    if( result )
    {
        return NNS_GfdMakeTexKey( addr, szByte, is4x4comp );    
    }else{
        NNS_GFD_WARNING("failure in Vram Allocation. : NNS_GfdAllocLnkTexVram()");
        return NNS_GFD_ALLOC_ERROR_TEXKEY;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_GfdFreeLnkTexVram

  Description:  テクスチャ領域をVRAMから開放します。


                
                
  Arguments:    memKey : テクスチャキー

                
  Returns:      なし

 *---------------------------------------------------------------------------*/
int             NNS_GfdFreeLnkTexVram( NNSGfdTexKey memKey )
{
    BOOL        result;
    const u32   addr     = NNS_GfdGetTexKeyAddr( memKey );
    const u32   szByte   = NNS_GfdGetTexKeySize( memKey );
    const BOOL  b4x4     = NNS_GfdGetTexKey4x4Flag( memKey );
    
    if( szByte != 0 )
    { 
        if( b4x4 )
        {
            result = NNSi_GfdFreeLnkVram( &mgr_.mgr4x4, &mgr_.pBlockPoolList, addr, szByte );
        }else{
            result = NNSi_GfdFreeLnkVram( &mgr_.mgrNrm, &mgr_.pBlockPoolList, addr, szByte );
        }
        
        if( result ) 
        {
            return 0;
        }else{
            return 1;
        }
    }else{
        return NNS_GFD_LNK_FREE_ERROR_INVALID_SIZE;    
    }
}






/*---------------------------------------------------------------------------*
  Name:         NNS_GfdGetLnkPlttVramManagerWorkSize

  Description:  リンクドリストテクスチャVRAMマネージャのテクスチャ用メモリ
                確保状態を初期状態に戻します。
      
  Arguments:    なし

                
  Returns:      なし

 *---------------------------------------------------------------------------*/
void            NNS_GfdResetLnkTexVramState( void )
{
    SlotData        sd[4] = 
    { 
        // 空き領域サイズ, 通常テクスチャ用サイズ, 4x4圧縮テクスチャ用サイズ
        { 0x20000, 0, 0 },
        { 0x20000, 0, 0 },
        { 0x20000, 0, 0 },
        { 0x20000, 0, 0 }
    };
    
    // スロット１に確保されるインデックステーブル用領域のサイズ
    // (ただし、マネージャはフリーブロックとして管理はしない）
    const u32   szIndexTbl  = mgr_.szByteFor4x4 / 2; 
    // 通常テクスチャサイズ                                                 
    u32         restNrm     = mgr_.szByte - ( mgr_.szByteFor4x4 + szIndexTbl );
    // 4x4圧縮テクスチャサイズ                                                 
    u32         rest4x4     = mgr_.szByteFor4x4;
    u32         slotNo;
    u32         val;
    
    
    //------------------------------------------------------------------------------
    // 4x4用の使用容量を計算し、結果を SlotData に格納します
    //
    for( slotNo = 0; slotNo < 4; slotNo++ )
    {
        // スロット0か2のみに4x4用領域は確保されます
        if( slotNo == 0 || slotNo == 2 )
        {
            if( sd[slotNo].szFree > 0 && rest4x4 > 0 )
            {
                if( sd[slotNo].szFree > rest4x4 )
                {
                    val = rest4x4;
                }else{
                    val = sd[slotNo].szFree;
                }
                
                sd[slotNo].sz4x4     += val;
                sd[slotNo].szFree    -= val;
                rest4x4              -= val;
            }
        }
    }
    
    //
    // スロット１：インデックステーブル領域のサイズを計算し、フリー領域から減産します
    //
    {
        sd[1].szFree    -= szIndexTbl;
        // マネージャはインデックステーブル用の領域は管理しません
    }
    
    //
    // 残った領域から、通常テクスチャ領域のサイズを計算します
    //
    for( slotNo = 0; slotNo < 4; slotNo++ )
    {
        if( sd[slotNo].szFree > 0 && restNrm > 0 )
        {
            if( sd[slotNo].szFree > restNrm )
            {
                val = restNrm;
            }else{
                val = sd[slotNo].szFree;
            }
            
            sd[slotNo].szNrm     += val;
            sd[slotNo].szFree    -= val;
            restNrm              -= val;
        }
    }
    
    //------------------------------------------------------------------------------
    //
    // 初期化処理(算出した SlotData を元にフリーブロックを初期化します。)
    //
    {
        BOOL result = TRUE;    
        
        NNSi_GfdInitLnkVramMan( &mgr_.mgrNrm );
        NNSi_GfdInitLnkVramMan( &mgr_.mgr4x4 );
        
        
        //
        // 共有管理ブロックを初期化
        //
        mgr_.pBlockPoolList 
            = NNSi_GfdInitLnkVramBlockPool( (NNSiGfdLnkVramBlock*)mgr_.pWorkHead, 
                                            mgr_.szByteManagementWork / sizeof( NNSiGfdLnkVramBlock ) );
        
        {
            //
            // 下位側に4x4、そのすぐ上位側に通常のフリーブロックを作成しマネージャに 登録する
            //
            // slot 0 
            result &= 
            InitSlotFreeBlock_( &mgr_.mgr4x4, &mgr_.pBlockPoolList , GFD_SLOT0_BASEADDR              , sd[0].sz4x4 );
            result &= 
            InitSlotFreeBlock_( &mgr_.mgrNrm, &mgr_.pBlockPoolList , GFD_SLOT0_BASEADDR + sd[0].sz4x4, sd[0].szNrm );
            // slot 2
            result &= 
            InitSlotFreeBlock_( &mgr_.mgr4x4, &mgr_.pBlockPoolList , GFD_SLOT2_BASEADDR              , sd[2].sz4x4 );
            result &= 
            InitSlotFreeBlock_( &mgr_.mgrNrm, &mgr_.pBlockPoolList , GFD_SLOT2_BASEADDR + sd[2].sz4x4, sd[2].szNrm );
            
            // slot 3
            NNS_GFD_ASSERT( sd[3].sz4x4 == 0 ); 
            result &= 
            InitSlotFreeBlock_( &mgr_.mgrNrm, &mgr_.pBlockPoolList , GFD_SLOT3_BASEADDR              , sd[3].szNrm );
            
            // slot 1
            // パレットインデックス用の領域はマネージャでは管理しない。
            // パレットインデックス用の領域以外の領域を通常テクスチャ用フリーブロックとして初期化する。
            result &= 
            InitSlotFreeBlock_( &mgr_.mgrNrm, &mgr_.pBlockPoolList , GFD_SLOT1_BASEADDR + szIndexTbl, sd[1].szNrm );
        }
        NNS_GFD_ASSERT( result );    
    }
        
    // フリーリストの結合を試みます。
    NNSi_GfdMergeAllFreeBlocks( &mgr_.mgrNrm, &mgr_.pBlockPoolList );
    NNSi_GfdMergeAllFreeBlocks( &mgr_.mgr4x4, &mgr_.pBlockPoolList );
}


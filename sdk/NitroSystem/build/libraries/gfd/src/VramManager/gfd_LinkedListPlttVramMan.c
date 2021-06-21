/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - gfd - src - VramManager
  File:     gfd_LinkedListPlttVramMan.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.13 $
 *---------------------------------------------------------------------------*/
#include <nnsys/gfd/gfd_common.h>
#include <nnsys/gfd/VramManager/gfd_LinkedListPlttVramMan.h>
#include "gfdi_LinkedListVramMan_Common.h"

#define GFD_SLOT_SIZE        0x18000

#define NNS_GFD_BARPLTT_FREE_ERROR_INVALID_SIZE 1

//
// マネージャ
//
typedef struct NNS_GfdLnkPlttVramManager
{
    NNSiGfdLnkVramMan       mgr;
    NNSiGfdLnkVramBlock*    pBlockPoolList;
    
    //
    // リセット時に使用するメンバ
    //
    u32                     szByte;
    NNSiGfdLnkVramBlock*    pWorkHead;
    u32                     szByteManagementWork;

}NNS_GfdLnkPlttVramManager;

static NNS_GfdLnkPlttVramManager         mgr_;


/*---------------------------------------------------------------------------*
  Name:         NNS_GfdDumpLnkPlttVramManager

  Description:  マネージャの内部状態をデバック出力します。

  Arguments:    なし            
  Returns:      なし

 *---------------------------------------------------------------------------*/
void NNS_GfdDumpLnkPlttVramManager()
{
    OS_Printf("=== NNS_Gfd LnkPlttVramManager Dump ===\n");
    OS_Printf("   address:        size    \n");   // ヘッダー行
    OS_Printf("=======================================\n");
    //
    // 通常テクスチャのフリーリストをすべて表示し、使用量の総和を計算します。
    //
    OS_Printf("------ Free Blocks                -----\n");   
    
    NNSi_GfdDumpLnkVramManFreeListInfo( mgr_.mgr.pFreeList, mgr_.szByte );
    
    OS_Printf("=======================================\n");   
}

/*---------------------------------------------------------------------------*
  Name:         NNS_GfdDumpLnkPlttVramManagerEx

  Description:  デバック出力処理関数を指定して、
                マネージャの内部状態をデバック出力します。

  Arguments:    pFunc               デバック出力処理関数
                pUserData           デバック出力処理関数に引数として渡されるデータ
                
  Returns:      なし

 *---------------------------------------------------------------------------*/

void NNS_GfdDumpLnkPlttVramManagerEx( 
    NNSGfdLnkDumpCallBack   pFunc, 
    void*                   pUserData )
{
    NNS_GFD_NULL_ASSERT( pFunc );
    NNSi_GfdDumpLnkVramManFreeListInfoEx( mgr_.mgr.pFreeList, pFunc, pUserData );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_GfdGetLnkPlttVramManagerWorkSize

  Description:  VRAMマネージャが管理情報のために必要とするメモリのバイトサイズを取得します。
                本関数の返値を利用して、マネージャの初期化パラメータである、
                管理領域の初期化を行ってください。 


                
                
  Arguments:    numMemBlk   確保される管理ブロック数。管理する空き領域の最大細分化数となる。 

                
  Returns:      管理領域に必要なバイトサイズ

 *---------------------------------------------------------------------------*/
u32 NNS_GfdGetLnkPlttVramManagerWorkSize( u32 numMemBlk )
{
    return numMemBlk * sizeof( NNSiGfdLnkVramBlock );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_GfdInitLnkPlttVramManager

  Description:  フレームパレットVRAMマネージャを初期化します。
                フレームパレットVRAMマネージャは、パレットRAMの先頭からszByte
                で指定されたサイズ分の領域を管理するように初期化されます。 
                
                
  Arguments:    szByte                  管理する領域のバイトサイズ。(最大 0x18000)
                pManagementWork         管理情報として使用するメモリ領域へのポインタ。 
                szByteManagementWork    管理情報領域のサイズ。 
                useAsDefault            リンクドリストパレットVRAMマネージャをカレントのマネージャとして使用するかどうか。 

                
  Returns:      なし

 *---------------------------------------------------------------------------*/
void NNS_GfdInitLnkPlttVramManager
( 
    u32     szByte,
    void*   pManagementWork, 
    u32     szByteManagementWork,
    BOOL    useAsDefault
)
{
    NNS_GFD_ASSERT( szByte <= GFD_SLOT_SIZE );
    NNS_GFD_NULL_ASSERT( pManagementWork );
    NNS_GFD_ASSERT( szByteManagementWork != 0 );
    
    {
        //
        mgr_.szByte                 = szByte;
        mgr_.pWorkHead              = pManagementWork;
        mgr_.szByteManagementWork   = szByteManagementWork;
        
        NNS_GfdResetLnkPlttVramState();
        
        //
        // デフォルトのアロケータとして使用
        //
        if( useAsDefault )
        {
            NNS_GfdDefaultFuncAllocPlttVram = NNS_GfdAllocLnkPlttVram;
            NNS_GfdDefaultFuncFreePlttVram  = NNS_GfdFreeLnkPlttVram;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_GfdAllocLnkPlttVram

  Description:  リンクドリストパレットVRAMマネージャからパレット用メモリを確保します。
                
  Arguments:    szByte：     確保したい領域のバイトサイズ 
                b4Pltt：     4色パレットか。4色パレットならTRUE。 
                opt ：       オプション。現在は使用されていません。 
                
  Returns:      確保したVRAM領域を表すパレットキーを返します。
                確保に失敗した場合は、エラーを示すキーである
                NNS_GFD_ALLOC_ERROR_PLTTKEYを返します。

 *---------------------------------------------------------------------------*/
NNSGfdPlttKey    NNS_GfdAllocLnkPlttVram( u32 szByte, BOOL b4Pltt, u32 opt )
{
#pragma unused(opt)
    u32     addr;
    BOOL    result;
    
    {
        //
        // パレットキーで表現できない小さなサイズの確保の場合、サイズを切り上げて確保します。
        //
        szByte = NNSi_GfdGetPlttKeyRoundupSize( szByte );
        //
        // パレットキーが表現できないほど大きなサイズの確保の場合、エラーキーを返します。
        //
        if( szByte >= NNS_GFD_PLTTSIZE_MAX )
        {
            NNS_GFD_WARNING("Allocation size is too big. : NNS_GfdAllocLnkPlttVram()");
            return NNS_GFD_ALLOC_ERROR_PLTTKEY;
        }
        
        NNS_GFD_MINMAX_ASSERT( szByte, NNS_GFD_PLTTSIZE_MIN, NNS_GFD_PLTTSIZE_MAX );
    }
    
    //
    // パレットフォーマット から アラインメントを設定します
    //
    if( b4Pltt )
    {
        result = NNSi_GfdAllocLnkVramAligned( &mgr_.mgr, 
                                              &mgr_.pBlockPoolList, 
                                              &addr, 
                                              szByte, 
                                              0x08 );
        // 参照不能領域かチェックする
        if( addr + szByte > NNS_GFD_4PLTT_MAX_ADDR )
        {
            // NG
            if( !NNSi_GfdFreeLnkVram( &mgr_.mgr, 
                                      &mgr_.pBlockPoolList, 
                                      addr, 
                                      szByte ) )
            {
                // 警告を表示する
            }
                                 
            return NNS_GFD_ALLOC_ERROR_PLTTKEY;
        }
    }else{
        result = NNSi_GfdAllocLnkVramAligned( &mgr_.mgr, 
                                              &mgr_.pBlockPoolList, 
                                              &addr, 
                                              szByte, 
                                              0x10 );
    }
    
    if( result )
    {
        return NNS_GfdMakePlttKey( addr, szByte );    
    }else{
        NNS_GFD_WARNING("failure in Vram Allocation. : NNS_GfdAllocLnkPlttVram()");
        return NNS_GFD_ALLOC_ERROR_PLTTKEY;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_GfdFreeLnkPlttVram

  Description:  リンクドリストパレットVRAMマネージャから確保した
                パレット用メモリを解放します。
                
  Arguments:    plttKey：    開放するVRAM領域を表すパレットキー
                
  Returns:      開放の成否を返します。
                開放に成功した場合は0を返します。
                サイズゼロなどの不正なサイズをあらわすパレットキーの開放
                があった場合は1を返します。

 *---------------------------------------------------------------------------*/
int             NNS_GfdFreeLnkPlttVram( NNSGfdPlttKey plttKey )
{
    
    const u32   addr     = NNS_GfdGetPlttKeyAddr( plttKey );
    const u32   szByte   = NNS_GfdGetPlttKeySize( plttKey );
    
    const BOOL  result   = NNSi_GfdFreeLnkVram( &mgr_.mgr, 
                                                &mgr_.pBlockPoolList, 
                                                addr, 
                                                szByte );
    
    if( result ) 
    {
        return 0;
    }else{
        return NNS_GFD_BARPLTT_FREE_ERROR_INVALID_SIZE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_GfdResetLnkPlttVramState

  Description:  マネージャを初期状態にリセットします。
                
  Arguments:    なし
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void            NNS_GfdResetLnkPlttVramState( void )
{
    
    //
    // 共有管理ブロックを初期化
    //
    mgr_.pBlockPoolList 
        = NNSi_GfdInitLnkVramBlockPool( 
            (NNSiGfdLnkVramBlock*)mgr_.pWorkHead, 
            mgr_.szByteManagementWork / sizeof( NNSiGfdLnkVramBlock ) );
    
    //
    // フリーリストの初期化
    //
    {
        BOOL    result;
        NNSi_GfdInitLnkVramMan( &mgr_.mgr );
        
        result = NNSi_GfdAddNewFreeBlock( &mgr_.mgr, 
                                          &mgr_.pBlockPoolList,
                                          0,
                                          mgr_.szByte );
        NNS_GFD_ASSERT( result );        
    }
    
    //
    // フリーリストの結合
    // 
    NNSi_GfdMergeAllFreeBlocks( &mgr_.mgr, &mgr_.pBlockPoolList );
    
}


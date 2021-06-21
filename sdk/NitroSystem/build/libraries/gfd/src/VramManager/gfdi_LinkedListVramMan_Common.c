/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - gfd - src - VramManager
  File:     gfdi_LinkedListVramMan_Common.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.14 $
 *---------------------------------------------------------------------------*/
#include <nnsys/gfd/gfd_common.h>
#include "gfdi_LinkedListVramMan_Common.h"



//------------------------------------------------------------------------------
// ブロックからリージョンを取得します。
static NNS_GFD_INLINE void 
GetRegionOfMemBlock_
(
    NNSiGfdLnkMemRegion*            pRegion,
    const NNSiGfdLnkVramBlock*      pBlk
)
{
    NNS_GFD_NULL_ASSERT( pBlk );
    NNS_GFD_NON_ZERO_ASSERT( pBlk->szByte );
    
    pRegion->start = pBlk->addr;
    pRegion->end   = pBlk->addr + pBlk->szByte;
    
    NNS_GFD_ASSERT( pRegion->end > pRegion->start );
}

//------------------------------------------------------------------------------
// リージョンからブロックを初期化します。
static NNS_GFD_INLINE void InitBlockFromRegion_
( 
    NNSiGfdLnkVramBlock*        pBlk, 
    const NNSiGfdLnkMemRegion*  pRegion
)
{
    NNS_GFD_NULL_ASSERT( pBlk );
    NNS_GFD_ASSERT( pRegion->end > pRegion->start );
    
    pBlk->addr      = pRegion->start;
    pBlk->szByte    = (u32)(pRegion->end - pRegion->start);
    pBlk->pBlkPrev  = NULL;
    pBlk->pBlkNext  = NULL;
}

//------------------------------------------------------------------------------
// パラメータ類からブロックを初期化します。
static NNS_GFD_INLINE void InitBlockFromPrams_
( 
    NNSiGfdLnkVramBlock*    pBlk, 
    u32                     addr, 
    u32                     szByte 
)
{   
    NNS_GFD_NULL_ASSERT( pBlk );
    NNS_GFD_NON_ZERO_ASSERT( szByte );
    
    pBlk->addr      = addr;
    pBlk->szByte    = szByte;
    pBlk->pBlkPrev  = NULL;
    pBlk->pBlkNext  = NULL;
}

//------------------------------------------------------------------------------
// リストの先頭へ要素を挿入します。
static NNS_GFD_INLINE void InsertBlock_
(
    NNSiGfdLnkVramBlock**   pListHead,
    NNSiGfdLnkVramBlock*    prev
)
{
    NNS_GFD_NULL_ASSERT( prev );
    
    if( (*pListHead) != NULL )
    {
        (*pListHead)->pBlkPrev    = prev;
    }
    
    prev->pBlkNext          = *pListHead;
    prev->pBlkPrev          = NULL;
    *pListHead = prev;
    
}

//------------------------------------------------------------------------------
// リストから要素を取り出します。
static NNS_GFD_INLINE void RemoveBlock_
(
    NNSiGfdLnkVramBlock**  pListHead,
    NNSiGfdLnkVramBlock*   pBlk
)
{
    NNS_GFD_NULL_ASSERT( pBlk );
    {
    
        NNSiGfdLnkVramBlock *const pPrev = pBlk->pBlkPrev;
        NNSiGfdLnkVramBlock *const pNext = pBlk->pBlkNext;

        // 前参照リンク
        if ( pPrev )
        {
            pPrev->pBlkNext = pNext;
        }else{
            *pListHead = pNext;
        }
        

        // 次参照リンク
        if ( pNext )
        {
            pNext->pBlkPrev = pPrev;
        }
    }
}

//------------------------------------------------------------------------------
// 新しいブロックを取得します。
static NNS_GFD_INLINE NNSiGfdLnkVramBlock* 
GetNewBlock_( NNSiGfdLnkVramBlock**   ppBlockPoolList )
{
    NNS_GFD_NULL_ASSERT( ppBlockPoolList );
    {
        // リスト先頭から取り出す
        NNSiGfdLnkVramBlock*    pNew = *ppBlockPoolList;
        if( pNew )
        {
            *ppBlockPoolList = pNew->pBlkNext;
        }
        
        return pNew;
    }
}   


//------------------------------------------------------------------------------
// ブロックの終端アドレスを取得します。
static NNS_GFD_INLINE u32 GetBlockEndAddr_( NNSiGfdLnkVramBlock* pBlk )
{
    NNS_GFD_NULL_ASSERT( pBlk );
    
    return (u32)(pBlk->addr + pBlk->szByte);
}

//------------------------------------------------------------------------------
// NNSi_GfdDumpLnkVramManFreeListInfo() で使用される、デバック出力処理関数。
static void DefaultDumpCallBack_( 
    u32                             addr, 
    u32                             szByte, 
    void*                           pUserData )
{

#ifdef SDK_FINALROM
    #pragma unused(addr)
#endif // SDK_FINALROM

    if( szByte != 0 )
    {
        u32*                        pszFreeTotal    = (u32*)pUserData;
           
        OS_Printf("0x%08x:  0x%08x    \n", addr, szByte );   
        (*pszFreeTotal) += szByte;
    }
}

//------------------------------------------------------------------------------
// フリーブロックを走査して、リージョンを結合します。
// 結合が起きたかどうかをBOOLで返します。
static BOOL TryToMergeBlockRegion_( 
    NNSiGfdLnkVramMan*      pMan, 
    NNSiGfdLnkVramBlock**   ppBlockPoolList,
    NNSiGfdLnkMemRegion*    pRegion )
{
    NNS_GFD_NULL_ASSERT( pMan );
    NNS_GFD_NULL_ASSERT( ppBlockPoolList );
    NNS_GFD_NULL_ASSERT( pRegion );
            
    {
        // 指定エリアに隣接したフリーエリアを検索
        NNSiGfdLnkVramBlock*        pCursor         = pMan->pFreeList;
        NNSiGfdLnkVramBlock*        pNext           = NULL;
        BOOL                        bMerged         = FALSE;

        // すべてのフリーリスト要素について...
        while( pCursor )
        {
            pNext = pCursor->pBlkNext;
              
            // 後方に隣接するブロックか?
            if( pCursor->addr == pRegion->end )   
            {
                // 空きリージョンを結合
                pRegion->end = GetBlockEndAddr_( pCursor );
                // リストから取り除き、Pool に戻す
                RemoveBlock_( &pMan->pFreeList, pCursor );
                InsertBlock_( ppBlockPoolList, pCursor );
                bMerged |= TRUE;
            }
                            
            // 前方に隣接するブロックか?
            if( GetBlockEndAddr_( pCursor ) == pRegion->start )
            {
                // 空きリージョンを結合
                pRegion->start  = pCursor->addr;
                // リストから取り除き、Pool に戻す
                RemoveBlock_( &pMan->pFreeList, pCursor );
                InsertBlock_( ppBlockPoolList, pCursor );
                bMerged |= TRUE;
            }
            
            pCursor = pNext;
        }

        return bMerged;
    }
}


/*---------------------------------------------------------------------------*
  Name:         NNSi_GfdDumpLnkVramManFreeListInfo

  Description:  フリーブロック情報をデバック出力します。
                
  Arguments:    pFreeListHead          : フリーブロック情報リストの先頭
                szReserved             : 確保されている領域のサイズ
               
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNSi_GfdDumpLnkVramManFreeListInfo
( 
    const NNSiGfdLnkVramBlock*      pFreeListHead,
    u32                             szReserved 
)
{
    
    u32                         szFreeTotal = 0; 
    const NNSiGfdLnkVramBlock*  pBlk        = pFreeListHead;
    
    // すべてのフリーリストの情報を表示
    NNSi_GfdDumpLnkVramManFreeListInfoEx( pBlk, DefaultDumpCallBack_, &szFreeTotal );
    
    // フリーリストが存在しない場合は、ダミー行の表示を行う
    if( szFreeTotal == 0 )
    {
        OS_Printf("0x--------:  0x--------    \n");
    }
    
    // 使用率の表示
    {
        const u32 szUsedTotal = (szReserved - szFreeTotal);
        OS_Printf("    %08d / %08d bytes (%6.2f%%) used \n", 
            szUsedTotal, szReserved, 100.f *  szUsedTotal / szReserved );   
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_GfdDumpLnkVramManFreeListInfoEx

  Description:  デバック出力処理関数を指定して、
                フリーブロック情報をデバック出力します。
                
  Arguments:    pFreeListHead          : フリーブロック情報リストの先頭
                pFunc                  : デバック出力処理関数
                pUserData              : デバック出力処理関数に引数として渡される、
                                         デバック出力処理用データ
               
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNSi_GfdDumpLnkVramManFreeListInfoEx( 
    const NNSiGfdLnkVramBlock*      pFreeListHead, 
    NNSGfdLnkDumpCallBack           pFunc, 
    void*                           pUserData )
{
    // すべてのフリーリストの情報を表示
    const NNSiGfdLnkVramBlock*  pBlk        = pFreeListHead;
    
    NNS_GFD_NULL_ASSERT( pFunc );
    
    while( pBlk )
    {
        (*pFunc)( pBlk->addr, pBlk->szByte, pUserData );
        pBlk          = pBlk->pBlkNext;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_GfdInitLnkVramMan

  Description:  NNSiGfdLnkVramManを初期化します。
                
  Arguments:    pMgr          : VRAMマネージャ
                
               
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void 
NNSi_GfdInitLnkVramMan( NNSiGfdLnkVramMan* pMgr )
{
    NNS_GFD_NULL_ASSERT( pMgr );
    pMgr->pFreeList = NULL;
}


/*---------------------------------------------------------------------------*
  Name:         NNSi_GfdInitLnkVramBlockPool

  Description:  管理ブロックプールを初期化します。
                管理情報要素数 == 管理可能なメモリ区分数となります
                
  Arguments:    pArrayHead          : 管理情報配列の先頭
                lengthOfArray       : 管理情報要素数
               
  Returns:      リストの先頭
  
 *---------------------------------------------------------------------------*/
NNSiGfdLnkVramBlock* 
NNSi_GfdInitLnkVramBlockPool
( 
    NNSiGfdLnkVramBlock*    pArrayHead, 
    u32                     lengthOfArray 
)
{
    NNS_GFD_NULL_ASSERT( pArrayHead );
    NNS_GFD_NON_ZERO_ASSERT( lengthOfArray );
    
    //
    // リストでつなぎます
    //
    {
        int i;
        for( i = 0; i < lengthOfArray - 1; i++ )
        {
            pArrayHead[i].pBlkNext      = &pArrayHead[i+1];
            pArrayHead[i+1].pBlkPrev    = &pArrayHead[i];
        }
        
        pArrayHead[0].pBlkPrev                  = NULL;  
        pArrayHead[lengthOfArray - 1].pBlkNext  = NULL;    
    }
    
    // リスト先頭を返します
    return &pArrayHead[0];
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_GfdInitLnkVramMan

  Description:  マネージャに新しいフリー領域のブロックを追加します。
                
                
  Arguments:    pMan                : マネージャ
                ppBlockPoolList     : 共通ブロック管理情報
                baseAddr            : 管理領域のベースアドレス
                szByte              : 確保するメモリサイズ
               
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
BOOL NNSi_GfdAddNewFreeBlock
(
    NNSiGfdLnkVramMan*      pMan, 
    NNSiGfdLnkVramBlock**   ppBlockPoolList,
    u32                     baseAddr,
    u32                     szByte
)
{
    NNS_GFD_NULL_ASSERT( pMan );
    NNS_GFD_NULL_ASSERT( ppBlockPoolList );
    NNS_GFD_NON_ZERO_ASSERT( szByte );
    
    // フリーブロックを作成する
    {
        NNSiGfdLnkVramBlock*        pNew  = GetNewBlock_( ppBlockPoolList );
        if( pNew )
        {
            InitBlockFromPrams_( pNew, baseAddr, szByte );
            InsertBlock_( &pMan->pFreeList, pNew );
            
            return TRUE;
        }else{
            return FALSE;
        }
    }
}



/*---------------------------------------------------------------------------*
  Name:         NNSi_GfdAllocLnkVram

  Description:  メモリ確保を行います。
                
                残りメモリが十分でも、
                管理ブロックが不足した場合に確保に失敗する場合があるので注意が必要です。 
                
  Arguments:    pMan                : マネージャ
                ppBlockPoolList     : 共通ブロック管理情報
                pRetAddr            : 確保するアドレスへのポインタ
                szByte              : 確保するメモリサイズ
                
  Returns:      メモリ確保の成否
  
 *---------------------------------------------------------------------------*/
BOOL
NNSi_GfdAllocLnkVram
( 
    NNSiGfdLnkVramMan*      pMan, 
    NNSiGfdLnkVramBlock**   ppBlockPoolList,
    u32*                    pRetAddr,
    u32                     szByte
)
{
    return NNSi_GfdAllocLnkVramAligned( pMan, ppBlockPoolList, pRetAddr, szByte, 0 );
}

//------------------------------------------------------------------------------
// アラインメントを指定可能なメモリ確保。
// アラインメントの際に発生した、空き領域はフリーリストに新たに登録される。
// 空き領域のための管理情報ブロックが存在しない場合には、確保失敗となるので注意が必要である。
BOOL
NNSi_GfdAllocLnkVramAligned
( 
    NNSiGfdLnkVramMan*      pMan, 
    NNSiGfdLnkVramBlock**   ppBlockPoolList,
    u32*                    pRetAddr,
    u32                     szByte,
    u32                     alignment
)
{
    NNS_GFD_NULL_ASSERT( pMan );
    NNS_GFD_NULL_ASSERT( pRetAddr );
    
    NNS_GFD_NON_ZERO_ASSERT( szByte );
    {
        //
        // フリーリストから条件にあうブロックを検索する
        //
        u32     alignedAddr;
        u32     szNeeded;
        u32     difference;
        
        NNSiGfdLnkVramBlock* pBlkFound  = NULL;
        NNSiGfdLnkVramBlock* pBlk       = pMan->pFreeList;
        
        
        while( pBlk )
        {
            //
            // 必要があれば、アライン境界に切り上げたアドレスを計算します
            //
            if( alignment > 1 )
            {
                alignedAddr = (u32)(  (pBlk->addr + (alignment - 1)) & ~(alignment - 1) );
                // 実際に必要となるサイズは切り上げた分だけ増加します
                difference  = ( alignedAddr - pBlk->addr );
                szNeeded    = szByte + difference;
            }else{
                alignedAddr = pBlk->addr;
                difference  = 0;
                szNeeded    = szByte;
            }
            
            
            // サイズが要求をみたすか？
            if( pBlk->szByte >= szNeeded )
            {
                pBlkFound = pBlk;
                break;
            }
            pBlk = pBlk->pBlkNext;
        }
        
        //
        // 条件にあるブロックが発見されたならば...
        //
        if ( pBlkFound ) 
        {
            // アライメントとしてずらした部分はフリーブロックとして登録します
            if( difference > 0 )
            {    
                NNSiGfdLnkVramBlock*        pNewFreeBlk = GetNewBlock_( ppBlockPoolList );
                if( pNewFreeBlk )
                {
                    // 登録
                    InitBlockFromPrams_( pNewFreeBlk, pBlkFound->addr, difference );
                    InsertBlock_( &pMan->pFreeList, pNewFreeBlk );
                }else{
                    // 確保失敗とする
                    goto NG_CASE;
                }
            }
            
            // 発見したフリーブロックの情報を更新
            {
                // 使用した分を減算する
                pBlkFound->szByte   -= szNeeded;
                // ブロック前方から確保する。
                pBlkFound->addr     += szNeeded; 
                
                // ちょうどの大きさだった
                if( pBlkFound->szByte == 0 )
                {
                    // フリーリストから取り除く
                    RemoveBlock_( &pMan->pFreeList, pBlkFound );
                    InsertBlock_( ppBlockPoolList, pBlkFound );
                }
            }
                    
            *pRetAddr = alignedAddr;
            return TRUE;
        }
        
NG_CASE:            
        //
        // 条件に合う、ブロックが発見できなかった。
        //
        *pRetAddr = 0;
        return FALSE;
        
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_GfdMergeAllFreeBlocks

  Description:  フリーブロックを走査して、ブロックの結合をします。
  
 *---------------------------------------------------------------------------*/
void NNSi_GfdMergeAllFreeBlocks( 
    NNSiGfdLnkVramMan*      pMan, 
    NNSiGfdLnkVramBlock**   ppBlockPoolList
)
{
    NNSiGfdLnkMemRegion         region;
    
    // すべての、フリーリストについて...
    NNSiGfdLnkVramBlock*        pCursor         = pMan->pFreeList;
    while( pCursor )
    {
        region.start    = pCursor->addr;
        region.end      = pCursor->addr + pCursor->szByte;
        
        // ブロックが結合できないか確認します。
        // 結合があれば...
        if( TryToMergeBlockRegion_( pMan, ppBlockPoolList, &region ) )
        {
            //
            // 結合試行後の、新しいサイズを設定します。
            //
            pCursor->addr    = region.start;
            pCursor->szByte  = region.end - region.start;       
            
            // リスト先頭から、再度走査します。
            pCursor = pMan->pFreeList;
        }else{
            pCursor = pCursor->pBlkNext;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_GfdFreeLnkVram

  Description:  メモリ開放を行います。
                管理ブロックが不足した場合に開放に失敗する場合があるので注意が必要です。
                (新たなフリーブロックの生成に失敗するため)
                
                
  Arguments:    pMan                : マネージャ
                ppBlockPoolList     : 共通ブロック管理情報
                addr                : 開放するアドレス
                szByte              : 開放するメモリサイズ
               
  Returns:      メモリ開放の成否
  
 *---------------------------------------------------------------------------*/
BOOL NNSi_GfdFreeLnkVram
( 
    NNSiGfdLnkVramMan*      pMan, 
    NNSiGfdLnkVramBlock**   ppBlockPoolList,
    u32                     addr,
    u32                     szByte
)
{
    NNS_GFD_NULL_ASSERT( pMan );
    NNS_GFD_NON_ZERO_ASSERT( szByte );
    
    {
        
        //------------------------------------------------------------------------------
        // 空きリージョンをフリーメモリブロックへ組み入れます。
        //      フリーブロックと隣接している場合は、フリーブロックを拡張します。
        //      フリーブロックと隣接しておらず、かつフリーブロックとするほどの
        //      サイズが無い場合は、後方に隣接する使用済みブロックのアライメント値とします。
        //      後方に隣接する使用済みブロックが無い場合は、関数は失敗します。
        {
            NNSiGfdLnkMemRegion     region;
        
            region.start    = addr;
            region.end      = addr + szByte;
        
            NNS_GFD_NULL_ASSERT( pMan );
            NNS_GFD_NULL_ASSERT( ppBlockPoolList );
            {
                (void)TryToMergeBlockRegion_( pMan, ppBlockPoolList, &region );
                                
                //
                // 新しいフリーブロックを登録する
                //
                {
                    NNSiGfdLnkVramBlock*        pNewFreeBlk = GetNewBlock_( ppBlockPoolList );
                    if( pNewFreeBlk == NULL )
                    {
                        // 管理領域用データが足りない
                        // Freeに失敗
                        return FALSE;
                    }else{
                    
                        InitBlockFromRegion_( pNewFreeBlk, &region );
                        InsertBlock_( &pMan->pFreeList, pNewFreeBlk );
                    }
                }
                
                return TRUE;
            }
        }
    }
}

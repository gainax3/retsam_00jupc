/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - gfd
  File:     gfdi_LinkedListVramMan_Common.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.9 $
 *---------------------------------------------------------------------------*/
#ifndef NNS_GFDI_LINKEDLISTVRAMMAN_COMMON_H_
#define NNS_GFDI_LINKEDLISTVRAMMAN_COMMON_H_


#include <nnsys/gfd.h>

//------------------------------------------------------------------------------
typedef struct NNSiGfdLnkVramBlock NNSiGfdLnkVramBlock;

/*---------------------------------------------------------------------------*
  Name:         NNSiGfdLnkVramBlock

  Description:  メモリ領域管理ブロック
  
 *---------------------------------------------------------------------------*/
struct NNSiGfdLnkVramBlock
{
    u32                         addr;       // 領域開始アドレス
    u32                         szByte;     // 領域サイズ(ゼロは不正です)
    
    NNSiGfdLnkVramBlock*        pBlkPrev;   // 前の領域(アドレス位置的な関連はない)
    NNSiGfdLnkVramBlock*        pBlkNext;   // 次の領域(アドレス位置的な関連はない)
    
};

/*---------------------------------------------------------------------------*
  Name:         NNSiGfdLnkMemRegion

  Description:  メモリ区間
                end > start を満たすものとします
  
 *---------------------------------------------------------------------------*/
typedef struct NNSiGfdLnkMemRegion
{
    u32       start;
    u32       end;
    
}NNSiGfdLnkMemRegion;

/*---------------------------------------------------------------------------*
  Name:         NNSiGfdLnkVramMan

  Description:  マネージャ
                通常のヒープなどとは異なり、使用済み領域の管理情報のリストを持ちません
                管理情報領域（メインメモリ上）と 管理領域アドレス（VRAM上）に関連が無く 
                TextureKey（アドレス + サイズの情報を持つ）からの使用済み領域の管理情報
                へのルックアップが難しいためです。
                フリーの際には アドレス + サイズ から 使用領域のフリー領域への返却を行っ
                ています。
                
 *---------------------------------------------------------------------------*/
typedef struct NNSiGfdLnkVramMan
{
    NNSiGfdLnkVramBlock*         pFreeList;         // 未使用領域ブロックリスト
      
}NNSiGfdLnkVramMan;




//------------------------------------------------------------------------------
// 関数宣言
//------------------------------------------------------------------------------
void NNSi_GfdDumpLnkVramManFreeListInfo
( 
    const NNSiGfdLnkVramBlock*      pFreeListHead,
    u32                             szReserved 
);

void NNSi_GfdDumpLnkVramManFreeListInfoEx( 
    const NNSiGfdLnkVramBlock*      pFreeListHead, 
    NNSGfdLnkDumpCallBack           pFunc, 
    void*                           pUserData );
    

void 
NNSi_GfdInitLnkVramMan( NNSiGfdLnkVramMan* pMgr );


NNSiGfdLnkVramBlock* 
NNSi_GfdInitLnkVramBlockPool
( 
    NNSiGfdLnkVramBlock*    pArrayHead, 
    u32                     lengthOfArray 
); 

BOOL
NNSi_GfdAddNewFreeBlock
(
    NNSiGfdLnkVramMan*      pMan, 
    NNSiGfdLnkVramBlock**   ppBlockPoolList,
    u32                     baseAddr,
    u32                     szByte
);


BOOL
NNSi_GfdAllocLnkVram
( 
    NNSiGfdLnkVramMan*      pMan, 
    NNSiGfdLnkVramBlock**   ppBlockPoolList,
    u32*                    pRetAddr,
    u32                     szByte
); 

BOOL
NNSi_GfdAllocLnkVramAligned
( 
    NNSiGfdLnkVramMan*      pMan, 
    NNSiGfdLnkVramBlock**   ppBlockPoolList,
    u32*                    pRetAddr,
    u32                     szByte,
    u32                     alignment
);

void NNSi_GfdMergeAllFreeBlocks( 
    NNSiGfdLnkVramMan*      pMan, 
    NNSiGfdLnkVramBlock**   ppBlockPoolList
);

BOOL NNSi_GfdFreeLnkVram
( 
    NNSiGfdLnkVramMan*      pMan, 
    NNSiGfdLnkVramBlock**   ppBlockPoolList,
    u32                     addr,
    u32                     szByte
);





#endif // NNS_GFDI_LINKEDLISTVRAMMAN_COMMON_H_


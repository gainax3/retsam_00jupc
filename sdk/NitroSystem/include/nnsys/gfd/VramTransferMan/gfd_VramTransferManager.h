/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - gfd - VramTransferMan
  File:     gfd_VramTransferManager.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.9 $
 *---------------------------------------------------------------------------*/
 
#ifndef NNS_GFD_VRAM_TRANSFER_MANAGER_H_
#define NNS_GFD_VRAM_TRANSFER_MANAGER_H_

#include <nitro.h>
#include <nnsys/gfd/gfd_common.h>

#ifdef __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*
  Name:         NNS_GFD_DST_TYPE

  Description:  転送先の種類
 *---------------------------------------------------------------------------*/
// see also DoTransfer_()
typedef enum NNS_GFD_DST_TYPE
{
    NNS_GFD_DST_3D_TEX_VRAM = 0,        // 3D texture
    NNS_GFD_DST_3D_TEX_PLTT,            // 3D texture palette
    NNS_GFD_DST_3D_CLRIMG_COLOR,        // 3D clear image color
    NNS_GFD_DST_3D_CLRIMG_DEPTH,        // 3D clear image depth
    
    NNS_GFD_DST_2D_BG0_CHAR_MAIN,       // 2D Main BG0 character
    NNS_GFD_DST_2D_BG1_CHAR_MAIN,       // 2D Main BG1 character
    NNS_GFD_DST_2D_BG2_CHAR_MAIN,       // 2D Main BG2 character
    NNS_GFD_DST_2D_BG3_CHAR_MAIN,       // 2D Main BG3 character
    NNS_GFD_DST_2D_BG0_SCR_MAIN,        // 2D Main BG0 screen
    NNS_GFD_DST_2D_BG1_SCR_MAIN,        // 2D Main BG1 screen
    NNS_GFD_DST_2D_BG2_SCR_MAIN,        // 2D Main BG2 screen
    NNS_GFD_DST_2D_BG3_SCR_MAIN,        // 2D Main BG3 screen
    NNS_GFD_DST_2D_BG2_BMP_MAIN,        // 2D Main BG2 bitmap
    NNS_GFD_DST_2D_BG3_BMP_MAIN,        // 2D Main BG3 bitmap
    NNS_GFD_DST_2D_OBJ_PLTT_MAIN,       // 2D Main OBJ palette
    NNS_GFD_DST_2D_BG_PLTT_MAIN,        // 2D Main BG palette
    NNS_GFD_DST_2D_OBJ_EXTPLTT_MAIN,    // 2D Main OBJ extended palette
    NNS_GFD_DST_2D_BG_EXTPLTT_MAIN,     // 2D Main BG extended palette
    NNS_GFD_DST_2D_OBJ_OAM_MAIN,        // 2D Main OBJ OAM
    NNS_GFD_DST_2D_OBJ_CHAR_MAIN,       // 2D Main OBJ character
    
    NNS_GFD_DST_2D_BG0_CHAR_SUB,        // 2D Sub BG0 character
    NNS_GFD_DST_2D_BG1_CHAR_SUB,        // 2D Sub BG1 character
    NNS_GFD_DST_2D_BG2_CHAR_SUB,        // 2D Sub BG2 character
    NNS_GFD_DST_2D_BG3_CHAR_SUB,        // 2D Sub BG3 character
    NNS_GFD_DST_2D_BG0_SCR_SUB,         // 2D Sub BG0 screen
    NNS_GFD_DST_2D_BG1_SCR_SUB,         // 2D Sub BG1 screen
    NNS_GFD_DST_2D_BG2_SCR_SUB,         // 2D Sub BG2 screen
    NNS_GFD_DST_2D_BG3_SCR_SUB,         // 2D Sub BG3 screen
    NNS_GFD_DST_2D_BG2_BMP_SUB,         // 2D Sub BG2 bitmap
    NNS_GFD_DST_2D_BG3_BMP_SUB,         // 2D Sub BG3 bitmap
    NNS_GFD_DST_2D_OBJ_PLTT_SUB,        // 2D Sub OBJ palette
    NNS_GFD_DST_2D_BG_PLTT_SUB,         // 2D Sub BG palette
    NNS_GFD_DST_2D_OBJ_EXTPLTT_SUB,     // 2D Sub OBJ extended palette
    NNS_GFD_DST_2D_BG_EXTPLTT_SUB,      // 2D Sub BG extended palette
    NNS_GFD_DST_2D_OBJ_OAM_SUB,         // 2D Sub OBJ OAM
    NNS_GFD_DST_2D_OBJ_CHAR_SUB,        // 2D Sub OBJ character
    
    NNS_GFD_DST_MAX

}NNS_GFD_DST_TYPE;

/*---------------------------------------------------------------------------*
  Name:         NNSGfdVramTransferTask

  Description:  転送タスク
 *---------------------------------------------------------------------------*/
typedef struct NNSGfdVramTransferTask
{
    NNS_GFD_DST_TYPE        type;       // 転送先の種類
    void*                   pSrc;       // 転送元のデータアドレス in Main memory
    u32                     dstAddr;    // 転送先アドレス
    u32                     szByte;     // 転送サイズ
    
}NNSGfdVramTransferTask;

/*---------------------------------------------------------------------------*
  Name:         NNSGfdVramTransferTaskQueue

  Description:  転送タスクキュー
 *---------------------------------------------------------------------------*/
typedef struct NNSGfdVramTransferTaskQueue
{
    NNSGfdVramTransferTask*     pTaskArray;     // タスク配列
    u32                         lengthOfArray;  // タスク配列長
    
    u16                         idxFront;       // 取り出し位置
    u16                         idxRear;        // エントリ位置
    u16                         numTasks;       // タスク数
    u16                         pad16_;         // パディング
    u32                         totalSize;      // 総転送量
    
}NNSGfdVramTransferTaskQueue;

/*---------------------------------------------------------------------------*
  Name:         NNSGfdVramTransferManager

  Description:  転送タスクマネージャ
 *---------------------------------------------------------------------------*/
typedef struct NNSGfdVramTransferManager
{
    NNSGfdVramTransferTaskQueue         taskQueue; // 転送タスクキュー        

}NNSGfdVramTransferManager;





//------------------------------------------------------------------------------
// public ( internal )
//------------------------------------------------------------------------------
//
// タスクキュー関連操作関係
//
BOOL 
NNSi_GfdPushVramTransferTaskQueue
( 
    NNSGfdVramTransferTaskQueue* pQueue 
);

NNSGfdVramTransferTask* 
NNSi_GfdGetFrontVramTransferTaskQueue
(
    NNSGfdVramTransferTaskQueue* pQueue
);

NNSGfdVramTransferTask* 
NNSi_GfdGetEndVramTransferTaskQueue
(
    NNSGfdVramTransferTaskQueue* pQueue
);

BOOL 
NNSi_GfdPopVramTransferTaskQueue
( 
    NNSGfdVramTransferTaskQueue* pQueue 
);




//------------------------------------------------------------------------------
// public
//------------------------------------------------------------------------------
//
// 初期化
//
void 
NNS_GfdInitVramTransferManager
( 
    NNSGfdVramTransferTask*    pTaskArray,
    u32                        lengthOfArray
);
//
// 登録タスクのクリア
//
void 
NNS_GfdClearVramTransferManagerTask( );
    
//
// 転送開始
// 登録タスクの実行
//
void 
NNS_GfdDoVramTransfer( void );

//
// タスク登録
//
BOOL 
NNS_GfdRegisterNewVramTransferTask
(
    NNS_GFD_DST_TYPE            type,
    u32                         dstAddr,
    void*                       pSrc,
    u32                         szByte
);
//
// 総転送量の取得
//
u32 
NNS_GfdGetVramTransferTaskTotalSize( void );


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_GFD_VRAM_TRANSFER_MANAGER_H_

/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - gfd - VramManager
  File:     gfd_FrameTexVramMan.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.9 $
 *---------------------------------------------------------------------------*/
#ifndef NNS_GFD_FRAMETEXVRAMMAN_H_
#define NNS_GFD_FRAMETEXVRAMMAN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro.h>
#include <nnsys/gfd/VramManager/gfd_TexVramMan_Types.h>

/*---------------------------------------------------------------------------*
  Name:         NNSGfdFrmTexVramState

  Description:  現在のマネージャの状態を記憶する構造体です。
                (実際には10のポインタ位置です）
                                            
 *---------------------------------------------------------------------------*/
typedef struct NNSGfdFrmTexVramState
{
    u32         address[10];

} NNSGfdFrmTexVramState;

//------------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  Name:         NNSGfdFrmTexVramDebugDumpCallBack

  Description:  NNS_GfdDumpFrmTexVramManagerEx()関数で利用される、
                ユーザ定義デバックダンプコールバック関数
                                            
 *---------------------------------------------------------------------------*/
 typedef void (*NNSGfdFrmTexVramDebugDumpCallBack)( 
    int index, 
    u32 startAddr, 
    u32 endAddr, 
    u32 blockMax, 
    BOOL bActive, 
    void* pUserContext );


//------------------------------------------------------------------------------
// 関数宣言
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// 内部限定
void NNSi_GfdSetTexNrmSearchArray
( 
    int idx1st, 
    int idx2nd,
    int idx3rd,
    int idx4th,
    int idx5th
);



    

void         NNS_GfdDumpFrmTexVramManager();
void         NNS_GfdDumpFrmTexVramManagerEx( NNSGfdFrmTexVramDebugDumpCallBack pFunc, void* pUserContext );

void		 NNS_GfdInitFrmTexVramManager( u16 numSlot, BOOL useAsDefault );

NNSGfdTexKey NNS_GfdAllocFrmTexVram( u32 szByte, BOOL is4x4comp, u32 opt );
int			 NNS_GfdFreeFrmTexVram( NNSGfdTexKey memKey );

void		 NNS_GfdGetFrmTexVramState( NNSGfdFrmTexVramState* pState );
void		 NNS_GfdSetFrmTexVramState( const NNSGfdFrmTexVramState* pState );
void		 NNS_GfdResetFrmTexVramState();

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_GFD_FRAMETEXVRAMMAN_H_

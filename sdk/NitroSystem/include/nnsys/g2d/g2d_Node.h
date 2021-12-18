/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d
  File:     g2d_Node.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.6 $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_NODE_H_
#define NNS_G2D_NODE_H_


#include <nitro.h>
#include <nnsys/g2d/g2d_config.h>
#include <nnsys/g2d/fmt/g2d_SRTControl_data.h>

#ifdef __cplusplus
extern "C" {
#endif


//------------------------------------------------------------------------------
typedef enum NNSG2dNodeType
{
    NNS_G2D_NODETYPE_INVALID       = 0,
    NNS_G2D_NODETYPE_CELL          ,
    NNS_G2D_NODETYPE_MULTICELL     ,
    NNS_G2D_NODETYPE_MAX
    
}NNSG2dNodeType;


#define NNS_G2D_ASSERT_NODETYPE_VALID( val )     \
        NNS_G2D_MINMAX_ASSERT( val, NNS_G2D_NODETYPE_CELL, NNS_G2D_NODETYPE_MULTICELL )

#define NNS_G2D_NODE_AFFINE_IDX_NONE    0xFFFF

//------------------------------------------------------------------------------
typedef struct NNSG2dNode
{
    void*                   pContent;   // 関連付けされた 描画実体
    NNSG2dNodeType          type;       // 関連付けされた 描画実体の種類
    BOOL                    bVisible;   // 可視フラグ
    NNSG2dSRTControl        srtCtrl;    // Scale Rotate Translate
    
}NNSG2dNode;


void NNSi_G2dInitializeNode( NNSG2dNode* pNode, NNSG2dNodeType type );


//------------------------------------------------------------------------------
// inline 関数
//------------------------------------------------------------------------------
NNS_G2D_INLINE void NNSi_G2dSetNodeVisibleFlag( NNSG2dNode* pNode, BOOL bVisible )
{
    NNS_G2D_NULL_ASSERT( pNode );
    pNode->bVisible = bVisible;
}

NNS_G2D_INLINE BOOL NNSi_G2dGetNodeVisibleFlag( NNSG2dNode* pNode )
{
    NNS_G2D_NULL_ASSERT( pNode );
    return pNode->bVisible;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_G2D_NODE_H_
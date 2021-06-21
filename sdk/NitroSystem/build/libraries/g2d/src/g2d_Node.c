/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g2d
  File:     g2d_Node.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.5 $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nnsys/g2d/g2d_Node.h>
#include <nnsys/g2d/g2d_SRTControl.h>

void NNSi_G2dInitializeNode( NNSG2dNode* pNode, NNSG2dNodeType type )
{
    NNS_G2D_NULL_ASSERT( pNode );
    NNS_G2D_ASSERT_NODETYPE_VALID( type );
    
    pNode->pContent     = NULL;
    pNode->type         = type;
    pNode->bVisible     = TRUE;
    
    
    NNSi_G2dSrtcInitControl( &pNode->srtCtrl, NNS_G2D_SRTCONTROLTYPE_SRT );
    
}






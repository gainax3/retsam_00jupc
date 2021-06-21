/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g2d
  File:     g2d_CullingUtility.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.2 $
 *---------------------------------------------------------------------------*/

#include <nnsys/g2d/g2d_CullingUtility.h>

//------------------------------------------------------------------------------
BOOL NNS_G2dIsInViewCircle( const NNSG2dFVec2* pTopLeft, const NNSG2dFVec2* pSize, const NNSG2dFVec2* pos, fx32 boundingR )
{
    
    if( pos->x < pTopLeft->x - boundingR )
        return FALSE;
    if( pos->y < pTopLeft->y - boundingR )
        return FALSE;
    
    if( pos->x > pTopLeft->x + pSize->x )
        return FALSE;
    
    if( pos->y > pTopLeft->y + pSize->y )
        return FALSE;
        
    return TRUE;
}


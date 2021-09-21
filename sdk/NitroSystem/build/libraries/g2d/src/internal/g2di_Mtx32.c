/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g2d - src - internal
  File:     g2di_Mtx32.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.5 $
 *---------------------------------------------------------------------------*/

#include "g2di_Mtx32.h"

//------------------------------------------------------------------------------
void NNSi_G2dMtxConcat32( const MtxFx32 *a, const MtxFx32 *b, MtxFx32 *ab )
{
    MtxFx32     temp;
    MtxFx32*    m;
    
    NNS_G2D_NULL_ASSERT( a );
    NNS_G2D_NULL_ASSERT( b );
    NNS_G2D_NULL_ASSERT( ab );
    
    if( ab == b )
    {
        m = &temp;
    }else{
        m = ab;
    }
    
    m->_00 = (fx32)( ((fx64)a->_00 * b->_00 + (fx64)a->_01 * b->_10    ) >> FX32_SHIFT );
    m->_01 = (fx32)( ((fx64)a->_00 * b->_01 + (fx64)a->_01 * b->_11    ) >> FX32_SHIFT );
    
    m->_10 = (fx32)( ((fx64)a->_10 * b->_00 + (fx64)a->_11 * b->_10    ) >> FX32_SHIFT );
    m->_11 = (fx32)( ((fx64)a->_10 * b->_01 + (fx64)a->_11 * b->_11    ) >> FX32_SHIFT );

    m->_20 = (fx32)( ((fx64)a->_20 * b->_00 + (fx64)a->_21 * b->_10    ) >> FX32_SHIFT ) + b->_20;
    m->_21 = (fx32)( ((fx64)a->_20 * b->_01 + (fx64)a->_21 * b->_11    ) >> FX32_SHIFT ) + b->_21;
    
    
    if( m == &temp )
    {
        *ab = temp;
    }
}


#ifndef SDK_FINALROM
//------------------------------------------------------------------------------
void NNSi_G2dDumpMtx32( const MtxFx32 *m )
{
    NNS_G2D_NULL_ASSERT( m );
    OS_Printf("---------------\n" );
    OS_Printf(" | %.5f %.5f |\n", FX_FX32_TO_F32( m->_00 ), FX_FX32_TO_F32( m->_01 ));
    OS_Printf(" | %.5f %.5f |\n", FX_FX32_TO_F32( m->_10 ), FX_FX32_TO_F32( m->_11 ));
    OS_Printf(" | %.5f %.5f |\n", FX_FX32_TO_F32( m->_20 ), FX_FX32_TO_F32( m->_21 ));
    OS_Printf("---------------\n" );
}
#endif // SDK_FINALROM

/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g2d
  File:     g2di_Debug.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.3 $
 *---------------------------------------------------------------------------*/
 
//
// This module should be exposed inside the library only.
//

#ifndef NNS_G2DI_DEBUG_H_
#define NNS_G2DI_DEBUG_H_

#include <nitro.h>
//#define NNSI_G2D_DEBUG

#ifdef NNSI_G2D_DEBUG
    #define NNSI_G2D_DEBUGMSG0( ... )    OS_Printf( __VA_ARGS__ );
#else  // NNSI_G2D_DEBUG
    #define NNSI_G2D_DEBUGMSG0( ... )     ((void) 0)
#endif // NNSI_G2D_DEBUG

#define NNSI_G2D_DEBUGMSG1( ... )     ((void) 0)



#endif // NNS_G2DI_DEBUG_H_

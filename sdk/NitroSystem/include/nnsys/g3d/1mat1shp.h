/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g3d
  File:     1mat1shp.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.4 $
 *---------------------------------------------------------------------------*/

//
// AUTHOR: Kenji Nishida
//

#ifndef NNSG3D_1MAT1SHP_H_
#define NNSG3D_1MAT1SHP_H_

#include <nnsys/g3d/config.h>
#include <nnsys/g3d/binres/res_struct.h>

#ifdef __cplusplus
extern "C" {
#endif

void NNS_G3dDraw1Mat1Shp(const NNSG3dResMdl* pResMdl, u32 matID, u32 shpID, BOOL sendMat);


#ifdef __cplusplus
}
#endif


#endif

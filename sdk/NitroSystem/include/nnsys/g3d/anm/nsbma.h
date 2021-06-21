/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g3d - anm
  File:     nsbma.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.9 $
 *---------------------------------------------------------------------------*/

//
// AUTHOR: Kenji Nishida
//

#ifndef NNSG3D_ANM_NSBMA_H_
#define NNSG3D_ANM_NSBMA_H_

#include <nnsys/g3d/config.h>
#include <nnsys/g3d/binres/res_struct.h>
#include <nnsys/g3d/kernel.h>
#include <nnsys/g3d/anm.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NNS_G3D_NSBMA_DISABLE
void NNSi_G3dAnmObjInitNsBma(NNSG3dAnmObj* pAnmObj,
                             void* pResAnm,
                             const NNSG3dResMdl* pResMdl);

void NNSi_G3dAnmCalcNsBma(NNSG3dMatAnmResult* pResult,
                          const NNSG3dAnmObj* pAnmObj,
                          u32 dataIdx);
#endif


#ifdef __cplusplus
}/* extern "C" */
#endif







#endif

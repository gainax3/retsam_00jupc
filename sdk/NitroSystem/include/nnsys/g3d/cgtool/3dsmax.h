/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g3d - cgtool
  File:     3dsmax.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.5 $
 *---------------------------------------------------------------------------*/

//
// AUTHOR: Kenji Nishida
//

#ifndef NNSG3D_CGTOOL_3DMAX_H_
#define NNSG3D_CGTOOL_3DMAX_H_

#include <nnsys/g3d/config.h>
#include <nnsys/g3d/anm.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NNS_G3D_3DSMAX_DISABLE
void NNSi_G3dSendTexSRT3dsMax(const NNSG3dMatAnmResult* anm);
#endif

#ifdef __cplusplus
}/* extern "C" */
#endif


#endif

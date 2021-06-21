/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g3d - cgtool
  File:     maya.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.8 $
 *---------------------------------------------------------------------------*/

//
// AUTHOR: Kenji Nishida
//

#ifndef NNSG3D_CGTOOL_MAYA_H_
#define NNSG3D_CGTOOL_MAYA_H_

#include <nnsys/g3d/config.h>
#include <nnsys/g3d/anm.h>
#include <nnsys/g3d/binres/res_struct.h>

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
//
// ä÷êîÇÃêÈåæ
//

#ifndef NNS_G3D_MAYA_DISABLE
void NNSi_G3dSendJointSRTMaya(const NNSG3dJntAnmResult* result);
void NNSi_G3dGetJointScaleMaya(NNSG3dJntAnmResult* pResult,
                               const fx32* p,
                               const u8* cmd,
                               u32 srtflag);
void NNSi_G3dSendTexSRTMaya(const NNSG3dMatAnmResult* anm);
#endif

#ifdef __cplusplus
}/* extern "C" */
#endif

#endif

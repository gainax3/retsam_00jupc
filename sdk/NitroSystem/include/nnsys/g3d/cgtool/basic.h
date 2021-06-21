/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g3d - cgtool
  File:     basic.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.7 $
 *---------------------------------------------------------------------------*/

//
// AUTHOR: Kenji Nishida
//

#ifndef NNSG3D_CGTOOL_BASIC_H_
#define NNSG3D_CGTOOL_BASIC_H_

#include <nnsys/g3d/config.h>
#include <nnsys/g3d/anm.h>

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
//
// ä÷êîÇÃêÈåæ
//

void NNSi_G3dSendJointSRTBasic(const NNSG3dJntAnmResult* result);
void NNSi_G3dGetJointScaleBasic(NNSG3dJntAnmResult* pResult,
                                const fx32* p,
                                const u8* cmd,
                                u32 srtflag);

#ifdef __cplusplus
}/* extern "C" */
#endif


#endif

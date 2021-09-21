/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g3d
  File:     cgtool.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.11 $
 *---------------------------------------------------------------------------*/

//
// AUTHOR: Kenji Nishida
//

#include <nnsys/g3d/cgtool.h>

#include <nnsys/g3d/cgtool/basic.h>
#include <nnsys/g3d/cgtool/maya.h>
#include <nnsys/g3d/cgtool/si3d.h>
#include <nnsys/g3d/cgtool/3dsmax.h>
#include <nnsys/g3d/cgtool/xsi.h>


//
// 各CGツール毎のスケール計算の違いを吸収するための関数ポインタ配列
//
NNSG3dGetJointScale NNS_G3dGetJointScale_FuncArray[NNS_G3D_FUNC_SENDJOINTSRT_MAX] =
{
    &NNSi_G3dGetJointScaleBasic,
#ifndef NNS_G3D_MAYA_DISABLE
    &NNSi_G3dGetJointScaleMaya,
#else
    NULL,
#endif
#ifndef NNS_G3D_SI3D_DISABLE
    &NNSi_G3dGetJointScaleSi3d
#else
    NULL
#endif
};


//
// 各CGツール毎のジョイントのSRT計算方法の違いを吸収するための関数ポインタ配列
//
NNSG3dSendJointSRT  NNS_G3dSendJointSRT_FuncArray[NNS_G3D_FUNC_SENDJOINTSRT_MAX] =
{
    &NNSi_G3dSendJointSRTBasic,
#ifndef NNS_G3D_MAYA_DISABLE
    &NNSi_G3dSendJointSRTMaya,
#else
    NULL,
#endif
#ifndef NNS_G3D_SI3D_DISABLE
    &NNSi_G3dSendJointSRTSi3d
#else
    NULL
#endif
};


//
// 各CGツール毎のテクスチャ行列の計算方法の違いを吸収するための関数ポインタ配列
//
NNSG3dSendTexSRT    NNS_G3dSendTexSRT_FuncArray[NNS_G3D_FUNC_SENDTEXSRT_MAX] =
{
#ifndef NNS_G3D_MAYA_DISABLE
    &NNSi_G3dSendTexSRTMaya,
#else
    NULL,
#endif
#ifndef NNS_G3D_SI3D_DISABLE
    &NNSi_G3dSendTexSRTSi3d,
#else
    NULL,
#endif
#ifndef NNS_G3D_3DSMAX_DISABLE
    &NNSi_G3dSendTexSRT3dsMax,
#else
    NULL,
#endif
#ifndef NNS_G3D_XSI_DISABLE
    &NNSi_G3dSendTexSRTXsi
#else
    NULL
#endif
};


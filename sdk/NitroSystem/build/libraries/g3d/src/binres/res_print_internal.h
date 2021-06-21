/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g3d - src - binres
  File:     res_print_internal.h

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

#ifndef NNSG3D_BINRES_RES_PRINT_INTERNAL_H_
#define NNSG3D_BINRES_RES_PRINT_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NNS_FROM_TOOL

#include <nitro_win32.h>
#include <stdio.h>
#define RES_PRINTF printf
#define FX_MUL_    CvtrFX_Mul
#else

#include <nitro.h>
#define RES_PRINTF OS_Printf
#define FX_MUL_    FX_Mul
#endif

#include <nnsys/g3d/binres/res_print.h>

#if defined(SDK_DEBUG) || defined(NNS_FROM_TOOL)

NNS_RES_PRINT void tabPlus_(void) NNS_RES_PRINT_END
NNS_RES_PRINT void tabMinus_(void) NNS_RES_PRINT_END
NNS_RES_PRINT void tabPrint_(void) NNS_RES_PRINT_END
NNS_RES_PRINT void printFx32_(fx32 v) NNS_RES_PRINT_END
NNS_RES_PRINT f32 fx32Tof32_(fx32 v) NNS_RES_PRINT_END
NNS_RES_PRINT void chkDict_(const NNSG3dResDict* dict,
                            const NNSG3dResName* name,
                            u32 idx) NNS_RES_PRINT_END

#endif // #if defined(SDK_DEBUG) || defined(NNS_FROM_TOOL)

#ifdef __cplusplus
}/* extern "C" */
#endif

#endif 
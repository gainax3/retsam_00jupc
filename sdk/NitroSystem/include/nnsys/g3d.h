/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys
  File:     g3d.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.5 $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G3D_H_
#define NNS_G3D_H_

//
// G3Dをお使いになられる際の注意事項
// ・ MI_SendGXCommandAsyncではなく、NNS_G3dSendDLをお使いください。
// ・ 複数のスレッドからG3Dの関数を呼び出さないでください。
// ・ SDKのG3_XXXXコマンドは、NNS_G3dGeFlushBuffer()を実行して、G3Dが持っている
//    ジオメトリコマンドキャッシュをフラッシュしてから使用してください。
//
// ADVICE
// ・ DO NOT USE MI_SendGXCommandAsync DIRECTLY. USE NNS_G3dSendDL INSTEAD.
// ・ DO NOT USE G3D FUNCTIONS FROM MULTIPLE THREADS.
// ・ Call G3_XXX on SDK after executing NNS_G3dGeFlushBuffer() --- This waits
//    until NNS_G3dSendDL completes, then flushes a geometry command buffer
//    which G3D may have.
//

#include <nnsys/g3d/config.h>
#include <nnsys/g3d/kernel.h>
#include <nnsys/g3d/anm.h>
#include <nnsys/g3d/glbstate.h>
#include <nnsys/g3d/gecom.h>
#include <nnsys/g3d/sbc.h>
#include <nnsys/g3d/1mat1shp.h>
#include <nnsys/g3d/model.h>
#include <nnsys/g3d/cgtool.h>
#include <nnsys/g3d/mem.h>
#include <nnsys/g3d/util.h>

#endif

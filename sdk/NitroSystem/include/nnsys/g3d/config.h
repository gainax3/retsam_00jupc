/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g3d
  File:     config.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.24 $
 *---------------------------------------------------------------------------*/

//
// AUTHOR: Kenji Nishida
//

#ifndef NNSG3D_CONFIG_H_
#define NNSG3D_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nnsys/inline.h>
#define NNS_G3D_INLINE NNS_INLINE

// アニメーションバイナリの初期化関数へのポインタテーブルのサイズ
#ifndef NNS_G3D_ANMFMT_MAX
#define NNS_G3D_ANMFMT_MAX 10
#endif

// ジョイント・マテリアル・シェイプの最大数
#ifndef NNS_G3D_SIZE_JNT_MAX
#define NNS_G3D_SIZE_JNT_MAX 64
#endif

#ifndef NNS_G3D_SIZE_MAT_MAX
#define NNS_G3D_SIZE_MAT_MAX 64
#endif

#ifndef NNS_G3D_SIZE_SHP_MAX
#define NNS_G3D_SIZE_SHP_MAX 64
#endif

//
// G3Dが持つジオメトリコマンドのバッファサイズ（ワード）
// ただ、デフォルトでは確保されない。
// NNS_G3dGeSetBufferで確保できる
//
#ifndef NNS_G3D_SIZE_COMBUFFER
#define NNS_G3D_SIZE_COMBUFFER 192
#endif

//
// コメントを外すとそれぞれのコールバックの有無をチェックする
// コードが無効になります。
//

//#define NNS_G3D_SBC_CALLBACK_TIMING_A_DISABLE
//#define NNS_G3D_SBC_CALLBACK_TIMING_B_DISABLE
//#define NNS_G3D_SBC_CALLBACK_TIMING_C_DISABLE


//
// コメントを外すとそれぞれのCGツール用のコードが無効になります。
//

//#define NNS_G3D_MAYA_DISABLE
//#define NNS_G3D_SI3D_DISABLE
//#define NNS_G3D_3DSMAX_DISABLE
//#define NNS_G3D_XSI_DISABLE

//
// コメントを外すとそれぞれのアニメーション用のコードが無効になります。
//

//#define NNS_G3D_NSBMA_DISABLE
//#define NNS_G3D_NSBTP_DISABLE
//#define NNS_G3D_NSBTA_DISABLE
//#define NNS_G3D_NSBCA_DISABLE
//#define NNS_G3D_NSBVA_DISABLE

// SBCのSHPコマンドが参照するディスパッチテーブルのサイズ(複数種類のMATバイナリフォーマットをサポートするため)
#ifndef NNS_G3D_SIZE_SHP_VTBL_NUM
#define NNS_G3D_SIZE_SHP_VTBL_NUM 4
#endif

// SBCのMATコマンドが参照するディスパッチテーブルのサイズ(複数種類のSHPバイナリフォーマットをサポートするため)
#ifndef NNS_G3D_SIZE_MAT_VTBL_NUM
#define NNS_G3D_SIZE_MAT_VTBL_NUM 4
#endif

// CGツール毎のジョイント計算を行うための関数テーブルのエントリの数
#ifndef NNS_G3D_FUNC_SENDJOINTSRT_MAX
#define NNS_G3D_FUNC_SENDJOINTSRT_MAX 3
#endif

// CGツール毎のテクスチャ行列計算を行うための関数テーブルのエントリの数
#ifndef NNS_G3D_FUNC_SENDTEXSRT_MAX
#define NNS_G3D_FUNC_SENDTEXSRT_MAX 4
#endif

// コメントを外すと、NNS_G3dGeSendDLによるディスプレイリストの転送に
// MI_SendGXCommandAsyncではなくMI_SendGXCommandAsyncFastを使用します
//#define NNS_G3D_USE_FASTGXDMA

//
// ウェイテッドエンベロープ使用時にキャッシュを使用しない場合は0を定義しておく
// とメモリを節約できる。
//
#ifndef NNS_G3D_USE_EVPCACHE
#define NNS_G3D_USE_EVPCACHE 1
#endif


#if defined(NNS_G3D_MAYA_DISABLE) && defined(NNS_G3D_SI3D_DISABLE) && \
    defined(NNS_G3D_3DSMAX_DISABLE) && defined(NNS_G3D_XSI_DISABLE)
#error You cannot disable all of the CG tools for G3D.
#endif

#if (NNS_G3D_FUNC_SENDJOINTSRT_MAX < 3)
#error NNS_G3D_FUNC_SENDJOINTSRT_MAX must be 3 or above.
#endif

#if (NNS_G3D_FUNC_SENDTEXSRT_MAX < 2)
#error NNS_G3D_FUNC_SENDTEXSRT_MAX must be 2 or above.
#endif


#if !defined(NNS_FROM_TOOL)
#include <nitro.h>
#define NNS_G3D_ASSERTMSG     SDK_ASSERTMSG
#define NNS_G3D_ASSERT        SDK_ASSERT
#define NNS_G3D_NULL_ASSERT   SDK_NULL_ASSERT
#define NNS_G3D_WARNING       SDK_WARNING

#else // if !defined(NNS_FROM_TOOL)

#include <nitro_win32.h>
#include <assert.h>
#define NNS_G3D_ASSERTMSG(x, y) assert((x))
#define NNS_G3D_ASSERT(x) assert((x))
#define NNS_G3D_NULL_ASSERT(x) assert(NULL != (x))
//#define NNS_G3D_WARNING(x, ...)

#endif  // if !defined(NNS_FROM_TOOL)

#ifdef __cplusplus
}
#endif

#endif

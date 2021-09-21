//==============================================================================
/**
 * @file	we_err.h
 * @brief	技エフェクト関連のデバッグ関係のヘッダ
 * @author	matsuda
 * @date	2005.07.29(金)
 */
//==============================================================================
#ifndef __WE_ERR_H__
#define __WE_ERR_H__

#ifdef PM_DEBUG

#include "wazaeffect/we_mana.h"


//==============================================================================
//	グローバル変数
//==============================================================================
extern WE_DEBUG_WORK WeDebugWork;


//==============================================================================
//	外部関数宣言
//==============================================================================
extern void DebugParticle_EmitMove(PTC_PTR ptc, EMIT_PTR emit, const VecFx32 *vec);
extern void DebugParticle_EmitMove2(PTC_PTR ptc, EMIT_PTR emit, WE_SYS_PTR we_sys);
extern void DebugParticle_CameraMove(PTC_PTR ptc);
extern void DebugParticle_CameraMove2(PTC_PTR ptc, EMIT_PTR emit, WE_SYS_PTR we_sys);
extern void DebugParticle_Axis(PTC_PTR ptc, EMIT_PTR emit);
extern void DebugParticle_AxisLoop(PTC_PTR ptc, int index, int cb_no, WE_SYS_PTR we_sys);

extern void DebugWazaEffect_LoopEmitter(PTC_PTR ptc, EMIT_PTR emit, WE_SYS_PTR we_sys);

#endif	//PM_DEBUG
#endif	//__WE_ERR_H__


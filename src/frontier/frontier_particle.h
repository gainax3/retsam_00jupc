//==============================================================================
/**
 * @file	frontier_particle.h
 * @brief	フロンティア 2Dマップ用パーティクルのヘッダ
 * @author	matsuda
 * @date	2007.06.07(木)
 */
//==============================================================================
#ifndef __FRONTIER_PARTICLE_H__
#define __FRONTIER_PARTICLE_H__

#include "system/particle.h"


//==============================================================================
//	型定義
//==============================================================================
///フロンティアパーティクルシステム構造体への不完全型ポインタ
typedef	struct _FRP_WORK * FRP_PTR;


//==============================================================================
//	外部関数宣言
//==============================================================================
extern FRP_PTR FRParticle_Init(int heap_id);
extern void FRParticle_Exit(FRP_PTR frp);
extern PTC_PTR FRParticle_SystemCreate(FRP_PTR frp, int work_id, int spa_no, int camera_type);
extern void FRParticle_SystemExit(FRP_PTR frp, int work_id);
extern int FRParticle_Main(void);
extern PTC_PTR FRParticle_PTCPtrGet(FRP_PTR frp, int work_id);
extern BOOL FRParticle_EmitNumGet(FRP_PTR frp);


#endif	//__FRONTIER_PARTICLE_H__


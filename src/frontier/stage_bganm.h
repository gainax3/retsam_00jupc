//==============================================================================
/**
 * @file	stage_bganm.h
 * @brief	バトルステージ：BGアニメのヘッダ
 * @author	matsuda
 * @date	2007.08.07(火)
 */
//==============================================================================
#ifndef __STAGE_BGANM_H__
#define __STAGE_BGANM_H__


//==============================================================================
//	型定義
//==============================================================================
///STAGE_BGANM_WORKの不定形ポインタ
typedef struct _STAGE_ANIMATION_WORK * STAGE_ANIMATION_WORK_PTR;


//==============================================================================
//	外部関数宣言
//==============================================================================
extern STAGE_ANIMATION_WORK_PTR StageAnimation_Init(GF_BGL_INI *bgl, PALETTE_FADE_PTR pfd);
extern void StageAnimation_End(STAGE_ANIMATION_WORK_PTR anmsys);


#endif	//__STAGE_BGANM_H__


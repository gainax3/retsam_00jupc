//==============================================================================
/**
 * @file	ending_setup.h
 * @brief	エンディングの各シーンの初期設定、解放処理のヘッダ
 * @author	matsuda
 * @date	2008.04.10(木)
 */
//==============================================================================
#ifndef __ENDING_SETUP_H__
#define __ENDING_SETUP_H__


//==============================================================================
//	外部関数宣言
//==============================================================================
extern void EndingSceneSetup_Load(ENDING_MAIN_WORK *emw, int scene_no);
extern void EndingSceneSetup_Unload(ENDING_MAIN_WORK *emw, int scene_no);
extern BOOL EndingSceneSetup_MainFunc(ENDING_MAIN_WORK *emw, int scene_no);

//--------------------------------------------------------------
//	各シーンのメイン関数
//--------------------------------------------------------------
extern BOOL MapScene1_Main(ENDING_MAIN_WORK *emw, ENDING_SCENE_WORK *sw);
extern BOOL MapScene2_Main(ENDING_MAIN_WORK *emw, ENDING_SCENE_WORK *sw);
extern BOOL MapScene3_Main(ENDING_MAIN_WORK *emw, ENDING_SCENE_WORK *sw);
extern BOOL MapCapScene1_Main(ENDING_MAIN_WORK *emw, ENDING_SCENE_WORK *sw);
extern BOOL MapCapScene2_Main(ENDING_MAIN_WORK *emw, ENDING_SCENE_WORK *sw);
extern BOOL MapFieldScene1_Main(ENDING_MAIN_WORK *emw, ENDING_SCENE_WORK *sw);
extern BOOL MapCopyrightScene_Main(ENDING_MAIN_WORK *emw, ENDING_SCENE_WORK *sw);


//==============================================================================
//	外部データ宣言
//==============================================================================
extern const u16 Scene3MdlSort[];

#endif	//__ENDING_SETUP_H__

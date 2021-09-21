//==============================================================================
/**
 * @file	ending_tool.h
 * @brief	エンディング：ツール類
 * @author	matsuda
 * @date	2008.04.10(木)
 */
//==============================================================================
#ifndef __ENDING_TOOL_H__
#define __ENDING_TOOL_H__


//==============================================================================
//	定数定義
//==============================================================================
///キャプチャー画像表示タイプ
enum{
	CAPTURE_TBL_TYPE_PICTURE,		///<思い出画像
	CAPTURE_TBL_TYPE_FIELD,			///<自宅前
};

///主人公の目のキャラクタデータサイズ
#define HERO_EYE_CHARBUF_SIZE	(0x20 * 8)	//目のキャラ数＝2キャラ(128kマッピングの為8キャラ分)


//==============================================================================
//	外部関数宣言
//==============================================================================
extern void EndingTool_ModelLineInit(ENDING_MAIN_WORK *emw);
extern void EndingTool_ModelLineArrangeDataInit(ENDING_MAIN_WORK *emw, int map_no);
extern BOOL EndintTool_ModelLineScroll(ENDING_MAIN_WORK *emw, s32 mdl_index_no);
extern void EndingTool_CaptureGraphicTrans(ENDING_MAIN_WORK *emw, int cap_no, 
	int frame_no, int tbl_type);
extern void EndingTool_CaptureGraphicPlttTransUpdate(ENDING_MAIN_WORK *emw);
extern void EndingTool_CaptureGraphicPlttTransFree(ENDING_MAIN_WORK *emw);
extern void EndingTool_HeroAnimeUpdate(ENDING_MAIN_WORK *emw, CATS_ACT_PTR cap);

//--------------------------------------------------------------
//	ending.c
//--------------------------------------------------------------
extern void EndingSceneVramExit(ENDING_MAIN_WORK *emw);
extern void FinalSceneVramSetup(ENDING_MAIN_WORK *emw);

#ifdef PM_DEBUG
extern void EndingTool_Debug_ModelLineArrangeSetting(ENDING_MAIN_WORK *emw, int line);
#endif	//PM_DEBUG

#endif	//__ENDING_TOOL_H__

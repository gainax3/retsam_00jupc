//==============================================================================
/**
 * @file	footprint_snd_def.h
 * @brief	足跡ボードの効果音定義ファイル
 * @author	matsuda
 * @date	2008.01.23(水)
 */
//==============================================================================
#ifndef __FOOTPRINT_SND_DEF_H__
#define __FOOTPRINT_SND_DEF_H__


//==============================================================================
//	効果音定義
//==============================================================================
///入室した人がいた時に鳴らすSE
#define FOOTPRINT_SE_ENTRY			(SEQ_SE_DP_BUTTON9)
///退室した人がいた時に鳴らすSE
#define FOOTPRINT_SE_LEAVE_ROOM		(SEQ_SE_DP_BUTTON9)

///スタンプを押した時の音
#define FOOTPRINT_SE_STAMP			(SEQ_SE_DP_BOX02)
//#define FOOTPRINT_SE_STAMP		(SEQ_SE_PL_STAMP)		//聞こえない

///インクパレットをタッチしたとき
#define FOOTPRINT_SE_TOUCH_INK		(SEQ_SE_DP_BUTTON3)
///「やめる」をタッチしたとき
#define FOOTPRINT_SE_TOUCH_EXIT		(SEQ_SE_DP_BUTTON3)

///制限時間が来た時
#define FOOTPRINT_SE_TIMEUP			(SEQ_SE_DP_PINPON)

//--------------------------------------------------------------
//	スペシャルエフェクト動作発動時に鳴らす音(エフェクトの長さが一定でないのなら個別にしないとダメ！)
//--------------------------------------------------------------
#define FOOTPRINT_SE_SP_NIJIMI			(SEQ_SE_PL_TIMER03)
#define FOOTPRINT_SE_SP_HAJIKE			(SEQ_SE_PL_TIMER03)
#define FOOTPRINT_SE_SP_ZIGZAG			(SEQ_SE_PL_TIMER03)
#define FOOTPRINT_SE_SP_DAKOU			(SEQ_SE_PL_TIMER03)
#define FOOTPRINT_SE_SP_KISEKI			(SEQ_SE_PL_TIMER03)
#define FOOTPRINT_SE_SP_YURE			(SEQ_SE_PL_TIMER03)
#define FOOTPRINT_SE_SP_KAKUDAI			(SEQ_SE_PL_TIMER03)
#define FOOTPRINT_SE_SP_BRAR_X			(SEQ_SE_PL_TIMER03)
#define FOOTPRINT_SE_SP_BRAR_Y			(SEQ_SE_PL_TIMER03)
#define FOOTPRINT_SE_SP_TARE			(SEQ_SE_PL_TIMER03)


#endif	//__FOOTPRINT_SND_DEF_H__


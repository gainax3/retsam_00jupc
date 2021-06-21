//==============================================================================
/**
 * @file	fss_test.h
 * @brief	簡単な説明を書く
 * @author	matsuda
 * @date	2007.04.04(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
#ifndef __FSS_TEST_H__
#define __FSS_TEST_H__


//==============================================================================
//	定数定義
//==============================================================================
//--------------------------------------------------------------
//	フレーム番号
//--------------------------------------------------------------
///ウィンドウ・メッセージ面のフレーム
#define FSSTEST_FRAME_WIN				(GF_BGL_FRAME1_M)
///エフェクト面のフレーム
#define FSSTEST_FRAME_EFF				(GF_BGL_FRAME2_M)
///背景面のフレーム
#define FSSTEST_FRAME_BACKGROUND			(GF_BGL_FRAME3_M)

///サブ画面：観客面のフレーム
#define FSSTEST_FRAME_SUB_AUDIENCE		(GF_BGL_FRAME0_S)

///BG番号：ウィンドウ
#define FSSTEST_BGNO_WIN			(1)
///BG番号：エフェクト
#define FSSTEST_BGNO_EFF			(2)
///BG番号：背景
#define FSSTEST_BGNO_BACKGROUND	(3)

///GX_WND番号：エフェクト
#define FSSTEST_GX_WND_EFF			(GX_WND_PLANEMASK_BG2)

///<3D面のBGプライオリティ
#define FSSTEST_3DBG_PRIORITY		(1)
///ウィンドウ面のBGプライオリティ
#define FSSTEST_BGPRI_WIN			(2)
///エフェクト面のBGプライオリティ
#define FSSTEST_BGPRI_EFF			(1)
///背景面のBGプライオリティ
#define FSSTEST_BGPRI_BACKGROUND		(3)

///<サブ画面：背景面のBGプライオリティ
///観客面のBGプライオリティ
#define FSSTEST_BGPRI_SUB_AUDIENCE	(3)



#endif	//__FSS_TEST_H__

//==============================================================================
/**
 * @file	frontier_types.h
 * @brief	フロンティア関連の不定形ポインタなど、どこでも使用するようなものの定義
 * @author	matsuda
 * @date	2007.04.06(金)
 */
//==============================================================================
#ifndef __FRONTIER_TYPES_H__
#define __FRONTIER_TYPES_H__

#include "system/clact_tool.h"
#include "fs_usescript.h"


//==============================================================================
//	定数定義
//==============================================================================
///	ＢＧパレット定義 2007.04.06(金)
//
//	0 ～ 10  : フィールドマップＢＧ用
//  11      : メッセージウインドウ
//  12      : メニューウインドウ
//  13      : メッセージフォント
//  14      : システムフォント
//	15		: 未使用（ローカライズ用）
#define FFD_FIELD_PAL_START  (0)			//  フィールドマップBGパレット開始
#define FFD_FIELD_PAL_END    (10)			//  フィールドマップBGパレット終了
#define FFD_MESFRAME_PAL     ( 11 )         //  メッセージウインドウ
#define FFD_MENUFRAME_PAL    ( 12 )         //  メニューウインドウ
#define FFD_MESFONT_PAL      ( 13 )         //  メッセージフォント
#define FFD_SYSFONT_PAL	     ( 14 )         //  システムフォント
#define FFD_LOCALIZE_PAL     ( 15 )         //	未使用（ローカライズ用）

#define FFD_FIELD_PAL_SIZE	((FFD_FIELD_PAL_END - FFD_FIELD_PAL_START + 1) * 0x20)

/*********************************************************************************************
	メイン画面のCGX割り振り		2007.04.06(金)

		ウィンドウ枠	：	409 - 511
			会話、メニュー、地名、看板

		BMPウィンドウ１	：	297 - 408
			会話

		BMPウィンドウ２	：	0 - 296
			メニュー（最大）、はい/いいえ

*********************************************************************************************/

/*********************************************************************************************
	ウィンドウ枠
*********************************************************************************************/
// 会話ウィンドウキャラ
#define	FR_TALK_WIN_CGX_SIZE	( 18+12 )
#define	FR_TALK_WIN_CGX_NUM		( 1024 - FR_TALK_WIN_CGX_SIZE )
#define	FR_TALK_WIN_PAL			( FFD_MESFRAME_PAL )

// メニューウィンドウキャラ
#define	FR_MENU_WIN_CGX_SIZE	( 9 )
#define	FR_MENU_WIN_CGX_NUM	( FR_TALK_WIN_CGX_NUM - FR_MENU_WIN_CGX_SIZE )
#define	FR_MENU_WIN_PAL		( FFD_MENUFRAME_PAL )

/*********************************************************************************************
	BMPウィンドウ
*********************************************************************************************/
// 会話ウィンドウ（メイン）
#define	FFD_MSG_WIN_PX		( 2 )
#define	FFD_MSG_WIN_PY		( 19 )
#define	FFD_MSG_WIN_SX		( 27 )
#define	FFD_MSG_WIN_SY		( 4 )
#define	FFD_MSG_WIN_PAL		( FFD_MESFONT_PAL )
#define	FFD_MSG_WIN_CGX		( FR_MENU_WIN_CGX_NUM - ( FFD_MSG_WIN_SX * FFD_MSG_WIN_SY ) )

// メニューウィンドウ（メイン）
#define	FFD_MENU_WIN_PX		( 20 )
#define	FFD_MENU_WIN_PY		( 1 )
#define	FFD_MENU_WIN_SX		( 11 )
#define	FFD_MENU_WIN_SY		( 22 )
#define	FFD_MENU_WIN_PAL	( FFD_SYSFONT_PAL )
#define	FFD_MENU_WIN_CGX	( FFD_MSG_WIN_CGX - ( FFD_MENU_WIN_SX * FFD_MENU_WIN_SY ) )

// はい/いいえウィンドウ（メイン）（メニューと同じ位置（メニューより小さい））
#define	FFD_YESNO_WIN_PX	( 25 )
#define	FFD_YESNO_WIN_PY	( 13 )
#define	FFD_YESNO_WIN_SX	( 6 )
#define	FFD_YESNO_WIN_SY	( 4 )
#define	FFD_YESNO_WIN_PAL	( FFD_SYSFONT_PAL )
#define	FFD_YESNO_WIN_CGX	( FFD_MSG_WIN_CGX - ( FFD_YESNO_WIN_SX * FFD_YESNO_WIN_SY ) )

// 名前＋CPウィンドウ(プレイヤー用)
#define	FFD_CP_WIN_PX		( 1 )
#define	FFD_CP_WIN_PY		( 1 )
#define	FFD_CP_WIN_SX		( 10 )
#define	FFD_CP_WIN_SY		( 4 )
#define	FFD_CP_WIN_PAL		( FFD_SYSFONT_PAL )
#define	FFD_CP_WIN_CGX		( FFD_MENU_WIN_CGX - ( FFD_CP_WIN_SX * FFD_CP_WIN_SY ) )

// 名前＋CPウィンドウ(パートナー用)
#define	FFD_CP2_WIN_PX		( 21 )
#define	FFD_CP2_WIN_PY		( 1 )
#define	FFD_CP2_WIN_SX		( FFD_CP_WIN_SX )
#define	FFD_CP2_WIN_SY		( FFD_CP_WIN_SY )
#define	FFD_CP2_WIN_PAL		( FFD_SYSFONT_PAL )
#define	FFD_CP2_WIN_CGX		( FFD_CP_WIN_CGX - ( FFD_CP2_WIN_SX * FFD_CP2_WIN_SY ) )

// BMPウィンドウ フリー開始位置
#define FFD_FREE_CGX		(1)


//==============================================================================
//	型定義
//==============================================================================
///FS_SYSTEMの不定形ポインタ
typedef struct _FRONTIERMAP_PROC_WORK * FMAP_PTR;

///FRONTIER_MAIN_WORKの不定形ポインタ
typedef struct _FRONTIER_MAIN_WORK * FMAIN_PTR;

//typedef struct _TAG_FIELD_OBJ * TARGET_OBJ_PTR;
typedef u32 * TARGET_OBJ_PTR;	//ターゲットOBJの型がまだ未定なのでとりあえずu32で定義しておく

///FS_SYSTEMの不定形ポインタ
typedef struct _FS_SYSTEM * FSS_PTR;

///FS_SYSTEMの不定形ポインタ
typedef struct _FSEV_WIN * FSEVWIN_PTR;


//==============================================================================
//	構造体定義
//==============================================================================
///配置物アクターのシステム内部で使うサブ構造体。アクターパラメータ関連
typedef struct{
	s16 x;				///<X座標
	s16 y;				///<Y座標

	u8 act_id;			///<アクターID
	u8 anm_seq;			///<現在のアニメーションシーケンスNo
	
	u16 anm_frame:13;	///<現在のアニメーションフレーム数
	u16 anm_stop:1;		///<アニメ停止フラグ
	u16 visible:1;		///<表示・非表示設定
	u16 occ:1;			///<この領域のパラメータの有効・無効フラグ(FALSE＝使われていない)
}PUSH_ARTICLE_ACT_PARAM;

///配置物アクターのシステム構造体
typedef struct{
	u16 resource_id[ACTWORK_RESOURCE_MAX];	///<スクリプト上で生成したアクターリソースのID保存場所
//	u32 anmbit;				///<アニメ動作の有無(1ビット単位で管理)

	PUSH_ARTICLE_ACT_PARAM act_param[ACTWORK_MAX];	///<アクターパラメータ
	//CATS_ACT_PTR act[ACTWORK_MAX];///<スクリプト上で生成して管理する場合のポインタ保存場所
}PUSH_ARTICLE_WORK;


#endif	//__FRONTIER_TYPES_H__

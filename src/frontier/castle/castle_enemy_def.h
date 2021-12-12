//==============================================================================================
/**
 * @file	castle_enemy_def.h
 * @brief	「バトルキャッスル」敵トレーナー画面の定義
 * @author	Satoshi Nohara
 * @date	07.07.17
 *
 * "castle_enemy.c"のみinclude可
 */
//==============================================================================================
#ifndef _CASTLE_ENEMY_DEF_H_
#define _CASTLE_ENEMY_DEF_H_


//==============================================================================================
//
//	定義
//
//==============================================================================================

//----------------------------------------------------------------------------------------------
//メッセージ関連の定義
//----------------------------------------------------------------------------------------------
#define CASTLE_MENU_BUF_MAX		(3)								//メニューバッファの最大数
#define BC_FONT					(FONT_SYSTEM)					//フォント種類
#define CASTLE_MSG_BUF_SIZE		(600)//(800)//(1024)			//会話のメッセージsize
#define CASTLE_MENU_BUF_SIZE	(32)							//メニューのメッセージsize
#define PLAYER_NAME_BUF_SIZE	(PERSON_NAME_SIZE + EOM_SIZE)	//プレイヤー名のメッセージsize
#define POKE_NAME_BUF_SIZE		(MONS_NAME_SIZE + EOM_SIZE)		//ポケモン名のメッセージsize

//フォントカラー
#define	COL_BLUE	( GF_PRINTCOLOR_MAKE(FBMP_COL_BLUE,FBMP_COL_BLU_SDW,FBMP_COL_NULL) )//青
#define	COL_RED		( GF_PRINTCOLOR_MAKE(FBMP_COL_RED,FBMP_COL_RED_SDW,FBMP_COL_NULL) )	//赤

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
#define CSR_POS_NONE				(0xff)						//カーソル何も選んでいない
#define CASTLE_ENEMY_DECIDE_NONE	(0xff)						//決定無効

//使用CP
#define CP_USE_TEMOTI		(1)			//しらべる
#define CP_USE_LEVEL_PLUS	(1)			//レベル＋５
#define CP_USE_LEVEL_MINUS	(15)		//レベルー５
#define CP_USE_TUYOSA		(2)			//つよさ
#define CP_USE_WAZA			(5)		//わざ

//----------------------------------------------------------------------------------------------
//ポケモン名の表示位置
//----------------------------------------------------------------------------------------------
enum{
	 POKE_NAME_PX			= (10*8),
	 POKE_NAME_PY			= (21*8),
	 POKE_NAME_SX			= (9*8),
	 POKE_NAME_SY			= (2*8),
};

//----------------------------------------------------------------------------------------------
//アイコンの位置
//----------------------------------------------------------------------------------------------
enum{
	ICON_START_X			= (64),
	ICON_MULTI_START_X		= (32),
	ICON_START_Y			= (58),
	ICON_WIDTH_X			= (64),
};

//----------------------------------------------------------------------------------------------
//ランクアップエフェクトの表示位置
//----------------------------------------------------------------------------------------------
enum{
	RANKUP_EFF_START_X			= (ICON_START_X),
	RANKUP_EFF_MULTI_START_X	= (ICON_MULTI_START_X),		//通信
	RANKUP_EFF_START_Y			= (ICON_START_Y + 4),
	RANKUP_EFF_WIDTH_X			= (ICON_WIDTH_X),
};

//----------------------------------------------------------------------------------------------
//モンスターボールの位置
//----------------------------------------------------------------------------------------------
enum{
	BALL_START_X			= (60),
	BALL_MULTI_START_X		= (28),
	BALL_START_Y			= (62),
	BALL_WIDTH_X			= (64),
};

//----------------------------------------------------------------------------------------------
//レベルアップエフェクトの位置
//----------------------------------------------------------------------------------------------
enum{
	LV_UPDOWN_START_X			= (ICON_START_X + 16),
	LV_UPDOWN_MULTI_START_X		= (ICON_MULTI_START_X + 16),
	LV_UPDOWN_START_Y			= (ICON_START_Y - 8),
	LV_UPDOWN_WIDTH_X			= (ICON_WIDTH_X),
};

//----------------------------------------------------------------------------------------------
//情報メッセージの表示位置
//----------------------------------------------------------------------------------------------
enum{
	INFO_SINGLE_OFFSET_X	= (4*8),
	INFO_MULTI_L_OFFSET_X	= (0),				//ペアのオフセット(Ｌ)
	INFO_MULTI_R_OFFSET_X	= (15*8),			//ペアのオフセット(Ｒ) // MatchComment: 16*8-4 -> 15*8

	INFO_PLAYER_X			= (0), // MatchComment: 1 -> 0 
	INFO_PLAYER_Y			= (0), // MatchComment: 1 -> 0
	INFO_PLAYER_CP_X		= (13*8),
	INFO_PLAYER_CP_Y		= (INFO_PLAYER_Y),
};

//----------------------------------------------------------------------------------------------
//HPの表示位置
//----------------------------------------------------------------------------------------------
enum{
	CASTLE_HP_START_X		= (4*8+4),
	CASTLE_HPSLASH_START_X	= (3*8),
	CASTLE_HPMAX_START_X	= (1*8 + CASTLE_HPSLASH_START_X),
	CASTLE_MULTI_HP_START_X	= (4),
	CASTLE_HP_START_Y		= (1),
	CASTLE_HP_WIDTH_X		= (8*8),					//幅
};

//----------------------------------------------------------------------------------------------
//LVの表示位置
//----------------------------------------------------------------------------------------------
enum{
	CASTLE_LV_START_X		= (CASTLE_HP_START_X+4),
	CASTLE_MULTI_LV_START_X	= (8),
	CASTLE_LV_START_Y		= (1),
	CASTLE_LV_WIDTH_X		= (CASTLE_HP_WIDTH_X),		//幅
};

//----------------------------------------------------------------------------------------------
//性別の表示位置
//----------------------------------------------------------------------------------------------
enum{
	CASTLE_SEX_START_X		= (CASTLE_LV_START_X + (5*8)),
	CASTLE_MULTI_SEX_START_X= (CASTLE_MULTI_LV_START_X + (5*8)),
	CASTLE_SEX_START_Y		= (1),
	CASTLE_SEX_WIDTH_X		= (CASTLE_HP_WIDTH_X),		//幅
};

//----------------------------------------------------------------------------------------------
//選択しているポケモンウィンドウの表示位置
//----------------------------------------------------------------------------------------------
enum{
	CASTLE_POKE_SEL_START_X			= (72),
	CASTLE_POKE_SEL_START_X2		= (CASTLE_HP_WIDTH_X+72),
	CASTLE_POKE_SEL_START_X3		= (CASTLE_HP_WIDTH_X*2+72),
	CASTLE_MULTI_POKE_SEL_START_X	= (40),
	CASTLE_MULTI_POKE_SEL_START_X2	= (CASTLE_MULTI_POKE_SEL_START_X+CASTLE_HP_WIDTH_X),
	CASTLE_MULTI_POKE_SEL_START_X3	= (168),
	CASTLE_MULTI_POKE_SEL_START_X4	= (CASTLE_MULTI_POKE_SEL_START_X3+CASTLE_HP_WIDTH_X),
	CASTLE_POKE_SEL_START_Y			= (88),
};

//----------------------------------------------------------------------------------------------
//カーソル関連
//----------------------------------------------------------------------------------------------
enum{
	//開始位置
	CSR_MULTI_START_X		= (1*8),
	CSR_START_Y				= (12*8),

	//幅
	CSR_WIDTH_X				= (8*8),
	CSR_WIDTH_Y				= (2*8),

	//並び数
	CSR_H_MAX				= (3),
	CSR_V_MAX				= (4),

	//もどる
	ENEMY_CSR_MODORU_X		= (28*8),
	ENEMY_CSR_MODORU_Y		= (20*8),

	SEL_CSR_L_OFFSET		= (10),			//左側カーソルのオフセット
	SEL_CSR_R_OFFSET		= (55),			//右側カーソルのオフセット
};


#endif //_CASTLE_ENEMY_DEF_H_



//==============================================================================================
/**
 * @file	castle_mine_def.h
 * @brief	「バトルキャッスル」手持ち画面の定義
 * @author	Satoshi Nohara
 * @date	07.07.17
 *
 * "castle_mine.c"のみinclude可
 */
//==============================================================================================
#ifndef _CASTLE_MINE_DEF_H_
#define _CASTLE_MINE_DEF_H_


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
#define CSR_POS_NONE			(0xff)							//カーソル何も選んでいない
#define CASTLE_MINE_DECIDE_NONE	(0xff)							//決定無効

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
//アイテム情報の表示位置
//----------------------------------------------------------------------------------------------
enum{
	 ITEM_INFO_PX			= (0), // MatchComment: 1 -> 0
	 ITEM_INFO_PY			= (6), // MatchComment: change from 0 to 6
	 ITEM_INFO_SX			= (23*8),
	 ITEM_INFO_SY			= (4*8),
};

//----------------------------------------------------------------------------------------------
//ポケモンアイコンの表示位置
//----------------------------------------------------------------------------------------------
enum{
	ICON_START_X			= (64),
	ICON_MULTI_START_X		= (32),				//通信
	ICON_START_Y			= (58),
	ICON_WIDTH_X			= (64),
};

//----------------------------------------------------------------------------------------------
//アイテムアイコン表示位置
//----------------------------------------------------------------------------------------------
enum{
	CASTLE_ITEMICON_X		= (24), // MatchComment: 28 -> 24
	CASTLE_ITEMICON_Y		= (162), // MatchComment: 164 -> 162
};

//----------------------------------------------------------------------------------------------
//回復、ランクアップエフェクトの表示位置
//----------------------------------------------------------------------------------------------
enum{
	KAIHUKU_EFF_START_X			= (ICON_START_X),
	KAIHUKU_EFF_MULTI_START_X	= (ICON_MULTI_START_X),		//通信
	KAIHUKU_EFF_START_Y			= (ICON_START_Y + 4),
	KAIHUKU_EFF_WIDTH_X			= (ICON_WIDTH_X),
};

//----------------------------------------------------------------------------------------------
//ランクアップ旗エフェクトの表示位置
//----------------------------------------------------------------------------------------------
enum{
	RANKUP_HATA_EFF_START_X			= (ICON_START_X),
	RANKUP_HATA_EFF_MULTI_START_X	= (ICON_MULTI_START_X),		//通信
	RANKUP_HATA_EFF_START_Y			= (ICON_START_Y + 64),
	RANKUP_HATA_EFF_WIDTH_X			= (16),
};


//----------------------------------------------------------------------------------------------
//情報メッセージの表示位置(てもち、レベル、、、)
//----------------------------------------------------------------------------------------------
enum{
	INFO_SINGLE_OFFSET_X	= (3*8+4),
	INFO_MULTI_L_OFFSET_X	= 0,				//ペアのオフセット(Ｌ)
	INFO_MULTI_R_OFFSET_X	= (15*8),			//ペアのオフセット(Ｒ) // MatchComment: 16*8-4 -> 15*8

	INFO_PLAYER_X			= 0, // MatchComment: 4 -> 0
	INFO_PLAYER_Y			= 0, // MatchComment: 1 -> 0
	INFO_PLAYER_CP_X		= (7*8), 
	INFO_PLAYER_CP_Y		= INFO_PLAYER_Y,
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
//カーソルの並び、位置など
//----------------------------------------------------------------------------------------------
enum{
	//開始位置
	CSR_MULTI_START_X		= (1*8),
	CSR_START_Y				= (12*8),

	//幅
	CSR_WIDTH_X				= (8*8),
	CSR_WIDTH_Y				= (2*8),

	//並び数
	CSR_H_MAX				= 3,
	CSR_V_MAX				= 4,

	//「もどる」
	MINE_CSR_MODORU_X		= (28*8),
	MINE_CSR_MODORU_Y		= (20*8),

	SEL_CSR_L_OFFSET		= (14),						//左側カーソルのオフセット
	SEL_CSR_R_OFFSET		= (58),						//右側カーソルのオフセット
};

//----------------------------------------------------------------------------------------------
//リストのスクロールカーソル表示位置
//----------------------------------------------------------------------------------------------
enum{
	CASTLE_SCR_U_X			= (160),
	CASTLE_SCR_U_Y			= (10),
	CASTLE_SCR_D_X			= (CASTLE_SCR_U_X),
	CASTLE_SCR_D_Y			= (124),
};

//----------------------------------------------------------------------------------------------
//アイテムウィンドウのポケモンアイコンの表示位置
//----------------------------------------------------------------------------------------------
enum{
	CASTLE_ITEM_POKE_ICON_X	= (40),
	CASTLE_ITEM_POKE_ICON_Y	= (80),
	CASTLE_ITEMKEEP_OFFSET_X	= (8),
	CASTLE_ITEMKEEP_OFFSET_Y	= (4),
};

//----------------------------------------------------------------------------------------------
//リストのアイテムＣＰ値の表示位置
//----------------------------------------------------------------------------------------------
enum{
	CASTLE_LIST_ITEM_CP_X	= (16*8),//(10*8), // MatchComment: 11*8 -> 16*8
};


#endif //_CASTLE_MINE_DEF_H_



//============================================================================================
/**
 * @file	vs_demo.h
 * @brief	通信対戦デモ
 * @author	Hiroyuki Nakamura
 * @date	06.04.26
 */
//============================================================================================
#ifndef VS_DEMO_H
#define VS_DEMO_H
#undef GLOBAL
#ifdef VS_DEMO_H_GLOBAL
#define GLOBAL	/*	*/
#else
#define GLOBAL	extern
#endif


//============================================================================================
//	定数定義
//============================================================================================

// 表示位置
enum {
	VSD_PARTY_LEFT1 = 0,	// 左側パーティのプレイヤー１
	VSD_PARTY_RIGHT1,		// 右側パーティのプレイヤー２
	VSD_PARTY_LEFT2,		// 左側パーティのプレイヤー１
	VSD_PARTY_RIGHT2,		// 右側パーティのプレイヤー２
	VSD_PARTY_MAX
};

// 外部作成データ
typedef struct {
	BATTLE_PARAM * bp;				// 戦闘パラメータ
	POKEPARTY * pp[VSD_PARTY_MAX];	// ポケモンデータ
	STRBUF * name[VSD_PARTY_MAX];	// プレーヤー名
	u32	heap;		// ヒープID
	u8	mode;		// デモモード
	u8	type;		// デモタイプ
	u8	result;		// 対戦結果
	u8	end_flg;	// 終了監視フラグ
	
	u8 rec_mode;	// 録画モード
	u8 dummy[3];
}VS_DEMO_DATA;

// 表示モード
enum {
	VSD_MODE_MULTI = 0,		// 参加ポケモン確認
	VSD_MODE_IN,			// 対戦開始
	VSD_MODE_END			// 対戦結果
};

// デモタイプ
enum {
	VSD_TYPE_NORMAL = 0,	// 対戦（通常）
	VSD_TYPE_MULTI,			// 対戦（マルチ）

	VSD_TYPE_LEFT = 0,		// 参加確認・左（自分）
	VSD_TYPE_RIGHT			// 参加確認・右（自分）
};

// 結果
enum {
	VSD_RESULT_WIN = 1,		// 勝ち
	VSD_RESULT_LOSE,		// 負け
	VSD_RESULT_DRAW,		// 引き分け
};


//============================================================================================
//	プロトタイプ宣言
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * タスク追加
 *
 * @param	dat		バッグデータ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void VSDemo_TaskAdd( VS_DEMO_DATA * dat );


#undef GLOBAL
#endif	/* VS_DEMO_H */

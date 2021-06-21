//==============================================================================
/**
 *
 * 戦闘録画のサブ構造体や定数の定義など
 *
 */
//==============================================================================
#ifndef __GT_GDS_BATTLE_REC_SUB_H__
#define __GT_GDS_BATTLE_REC_SUB_H__


#include "gds_define.h"


//==============================================================================
//	定数定義
//==============================================================================
///録画データの存在有無
#define GT_REC_OCC_MAGIC_KEY		(0xe281)

///録画ヘッダに格納するポケモン番号の最大要素数
#define GT_HEADER_MONSNO_MAX		(12)

#define	GT_WAZA_TEMOTI_MAX		(4)		///<1体のポケモンがもてる技の最大値

#define GT_PERSON_NAME_SIZE	7	// 人物の名前の長さ（自分も含む）
#define GT_MONS_NAME_SIZE		10	// ポケモン名の長さ(バッファサイズ EOM_含まず)
#define	GT_EOM_SIZE			1	// 終了コードの長さ

#define	GT_CLIENT_MAX			(4)		///<同時に起動するクライアントの最大数

#define GT_TEMOTI_POKEMAX		6	///< 手持ちポケモン最大数

#define	GT_REC_BUFFER_SIZE	(0x0400)	///<対戦録画バッファサイズ

#define GT_REGULATION_NAME_SIZE   (11)      // ルール名の長さ 11文字22バイト +EOM2byte

///バトルモード
typedef enum{
	//--------------------------------------------------------------
	//	コロシアム
	//--------------------------------------------------------------
	GT_BATTLE_MODE_COLOSSEUM_SINGLE,			//シングルバトル(ミックス含む)：制限なし
	GT_BATTLE_MODE_COLOSSEUM_SINGLE_STANDARD,	//シングルバトル(ミックス含む)：スタンダードカップ
	GT_BATTLE_MODE_COLOSSEUM_SINGLE_FANCY,		//シングルバトル(ミックス含む)：ファンシーカップ
	GT_BATTLE_MODE_COLOSSEUM_SINGLE_LITTLE,		//シングルバトル(ミックス含む)：リトルカップ
	GT_BATTLE_MODE_COLOSSEUM_SINGLE_LIGHT,		//シングルバトル(ミックス含む)：ライトカップ
	GT_BATTLE_MODE_COLOSSEUM_SINGLE_DOUBLE,		//シングルバトル(ミックス含む)：ダブルカップ
	GT_BATTLE_MODE_COLOSSEUM_SINGLE_ETC,		//シングルバトル(ミックス含む)：その他のカップ
	GT_BATTLE_MODE_COLOSSEUM_DOUBLE,			//ダブルバトル：制限なし
	GT_BATTLE_MODE_COLOSSEUM_DOUBLE_STANDARD,	//ダブルバトル：スタンダードカップ
	GT_BATTLE_MODE_COLOSSEUM_DOUBLE_FANCY,		//ダブルバトル：ファンシーカップ
	GT_BATTLE_MODE_COLOSSEUM_DOUBLE_LITTLE,		//ダブルバトル：リトルカップ
	GT_BATTLE_MODE_COLOSSEUM_DOUBLE_LIGHT,		//ダブルバトル：ライトカップ
	GT_BATTLE_MODE_COLOSSEUM_DOUBLE_DOUBLE,		//ダブルバトル：ダブルカップ
	GT_BATTLE_MODE_COLOSSEUM_DOUBLE_ETC,		//ダブルバトル：その他のカップ
	GT_BATTLE_MODE_COLOSSEUM_MULTI,				//マルチバトル

	//--------------------------------------------------------------
	//	フロンティア
	//--------------------------------------------------------------
	//タワー
	GT_BATTLE_MODE_TOWER_SINGLE,		//WIFI_DL含む
	GT_BATTLE_MODE_TOWER_DOUBLE,
	GT_BATTLE_MODE_TOWER_MULTI,		//NPC, COMM, WIFI
	//ファクトリー用INDEX開始
	GT_BATTLE_MODE_FACTORY50_SINGLE,
	GT_BATTLE_MODE_FACTORY50_DOUBLE,
	GT_BATTLE_MODE_FACTORY50_MULTI,	//COMM, WIFI
	GT_BATTLE_MODE_FACTORY100_SINGLE,
	GT_BATTLE_MODE_FACTORY100_DOUBLE,
	GT_BATTLE_MODE_FACTORY100_MULTI,	//COMM, WIFI
	//ステージ用INDEX開始
	GT_BATTLE_MODE_STAGE_SINGLE,
	GT_BATTLE_MODE_STAGE_DOUBLE,
	GT_BATTLE_MODE_STAGE_MULTI,		//COMM, WIFI
	//キャッスル用INDEX開始
	GT_BATTLE_MODE_CASTLE_SINGLE,
	GT_BATTLE_MODE_CASTLE_DOUBLE,
	GT_BATTLE_MODE_CASTLE_MULTI,		//COMM, WIFI
	//ルーレット用INDEX開始
	GT_BATTLE_MODE_ROULETTE_SINGLE,
	GT_BATTLE_MODE_ROULETTE_DOUBLE,
	GT_BATTLE_MODE_ROULETTE_MULTI,		//COMM, WIFI

	//--------------------------------------------------------------
	//	指定なし
	//--------------------------------------------------------------
	GT_BATTLE_MODE_COLOSSEUM_SINGLE_NO_REGULATION = 0xfa,	//コロシアム/シングル 制限なし
	GT_BATTLE_MODE_COLOSSEUM_SINGLE_REGULATION = 0xfb,		//コロシアム/シングル カップ戦
	GT_BATTLE_MODE_COLOSSEUM_DOUBLE_NO_REGULATION = 0xfc,	//コロシアム/ダブル 制限なし
	GT_BATTLE_MODE_COLOSSEUM_DOUBLE_REGULATION = 0xfd,		//コロシアム/ダブル カップ戦
	GT_BATTLE_MODE_EXCLUSION_FRONTIER = 0xfe,				//フロンティア施設を除外
	GT_BATTLE_MODE_NONE = 0xff,			//指定なし
}GT_BATTLE_MODE;

//--------------------------------------------------------------
//	検索条件”指定なし”の定義
//--------------------------------------------------------------
///ポケモン指定無し
#define GT_BATTLE_REC_SEARCH_MONSNO_NONE			(0xffff)
///バトルモード指定無し
#define GT_BATTLE_REC_SEARCH_BATTLE_MODE_NONE		(0xff)
///国コード指定無し
#define GT_BATTLE_REC_SEARCH_COUNTRY_CODE_NONE		(0xff)
///地方コード指定無し
#define GT_BATTLE_REC_SEARCH_LOCAL_CODE_NONE		(0xff)

//==============================================================================
//	型定義
//==============================================================================
///データナンバーの型
typedef u64 GT_DATA_NUMBER;


//----------------------------------------------------------
/**
 * @brief	自分状態データ型定義
 */
//----------------------------------------------------------
typedef struct {
	GT_STRCODE name[GT_PERSON_NAME_SIZE + GT_EOM_SIZE];		// 16
	u32 id;											// 20
	u32 gold;										// 24

	u8 sex;	
	u8 region_code;									//26

	u8 badge;										
	u8 trainer_view;	// ユニオンルーム内での見た目フラグ //28
	u8 rom_code;		// 0:ダイヤ  1:パール		// 29

	u8 dp_clear:1;		// DPクリアしているかフラグ
	u8 dp_zenkoku:1;	// DP全国図鑑手に入れているかフラグ
	u8 dummy1:6;									// 30
	u8 dummy2;										// 31
	u8 dummy3;
}GT_MYSTATUS;


//--------------------------------------------------------------
/**
 * 対戦録画用にカスタマイズされたポケモンパラメータ
 * 112byte
 */
//--------------------------------------------------------------
typedef struct{
	u32	personal_rnd;							//04h	個性乱数
	u16	pp_fast_mode		:1;					//06h	暗号／復号／チェックサム生成を後回しにして、処理を高速化モード
	u16	ppp_fast_mode		:1;					//06h	暗号／復号／チェックサム生成を後回しにして、処理を高速化モード
	u16	fusei_tamago_flag	:1;					//06h	ダメタマゴフラグ
	u16						:13;				//06h
	
	u16	monsno;								//02h	モンスターナンバー		
	u16 item;								//04h	所持アイテムナンバー	
	u32	id_no;								//08h	IDNo
	u32	exp;								//0ch	経験値
	u8	friendshipness;						//0dh	なつき度
	u8	speabino;							//0eh	特殊能力
	u8	hp_exp;								//11h	HP努力値
	u8	pow_exp;							//12h	攻撃力努力値
	u8	def_exp;							//13h	防御力努力値
	u8	agi_exp;							//14h	素早さ努力値
	u8	spepow_exp;							//15h	特攻努力値
	u8	spedef_exp;							//16h	特防努力値

	u16	waza[GT_WAZA_TEMOTI_MAX];				//08h	所持技
	u8	pp[GT_WAZA_TEMOTI_MAX];				//0ch	所持技PP
	u8	pp_count[GT_WAZA_TEMOTI_MAX];			//10h	所持技PP_COUNT
	u32	hp_rnd			:5;					//		HP乱数
	u32	pow_rnd			:5;					//		攻撃力乱数
	u32	def_rnd			:5;					//		防御力乱数
	u32	agi_rnd			:5;					//		素早さ乱数
	u32	spepow_rnd		:5;					//		特攻乱数
	u32	spedef_rnd		:5;					//		特防乱数
	u32	tamago_flag		:1;					//		タマゴフラグ（0:タマゴじゃない　1:タマゴだよ）
	u32	nickname_flag	:1;					//14h	ニックネームをつけたかどうかフラグ
	u8	event_get_flag	:1;					//		イベントで配布したことを示すフラグ
	u8	sex				:2;					//   	ポケモンの性別
	u8	form_no			:5;					//19h	形状ナンバー
	
	GT_STRCODE	nickname[GT_MONS_NAME_SIZE+GT_EOM_SIZE];	//16h	ニックネーム
	
	GT_STRCODE	oyaname[GT_PERSON_NAME_SIZE+GT_EOM_SIZE];	//10h	親の名前
	u8	get_ball;								//1ch	捕まえたボール
	u8	language;								// 言語コード
	
	u32			condition;							//04h	コンディション
	u8			level;								//05h	レベル
	u8			cb_id;								//06h	カスタムボールID
	u16			hp;									//08h	HP
	u16			hpmax;								//0ah	HPMAX
	u16			pow;								//0ch	攻撃力
	u16			def;								//0eh	防御力
	u16			agi;								//10h	素早さ
	u16			spepow;								//12h	特攻
	u16			spedef;								//14h	特防
	
} GT_REC_POKEPARA;

//--------------------------------------------------------------
/**
 *	戦闘録画用にカスタマイズされたPOKEPARTY
 *	676 byte
 */
//--------------------------------------------------------------
typedef struct{
	///	保持できるポケモン数の最大
	u16 PokeCountMax;
	///	現在保持しているポケモン数
	u16 PokeCount;
	///	ポケモンデータ
	GT_REC_POKEPARA member[GT_TEMOTI_POKEMAX];
}GT_REC_POKEPARTY;

///<対戦録画用データ構造体宣言
typedef u8 GT_REC_DATA;

///<対戦録画用ワーク構造体宣言
//	4096byte
typedef struct
{
	GT_REC_DATA	rec_buffer[GT_CLIENT_MAX][GT_REC_BUFFER_SIZE];
}GT_RECORD_PARAM;


//---------------------------------------------------------------------------
/**
 * @brief	設定データ構造体定義
 *			2byte
 */
//---------------------------------------------------------------------------
typedef struct {
	u16 msg_speed:4;			///<MSGSPEED		ＭＳＧ送りの速度
	u16 sound_mode:2;			///<SOUNDMODE		サウンド出力
	u16 battle_rule:1;			///<BATTLERULE		戦闘ルール
	u16 wazaeff_mode:1;			///<WAZAEFF_MODE	わざエフェクト
	u16 input_mode:2;			///<INPUTMODE		入力モード
	u16 window_type:5;			///<WINTYPE			ウィンドウタイプ
	u16 dummy:1;
}GT_CONFIG;

//--------------------------------------------------------------
/**
 *	@brief	トレーナーデータパラメータ
 *			52byte
 */
//--------------------------------------------------------------
typedef struct{
	u8			data_type;					//データタイプ
	u8			tr_type;					//トレーナー分類
	u8			tr_gra;						//トレーナーグラフィック
	u8			poke_count;					//所持ポケモン数

	u16			use_item[4];				//使用道具

	u32			aibit;						//AIパターン
	u32			fight_type;					//戦闘タイプ（1vs1or2vs2）

	GT_STRCODE		name[GT_PERSON_NAME_SIZE+GT_EOM_SIZE];	//トレーナー名
	GT_PMS_DATA	win_word;
	GT_PMS_DATA	lose_word;
}GT_TRAINER_DATA;

//--------------------------------------------------------------
/**
 * @brief   戦闘開始パラメータ
 *			336 byte
 */
//--------------------------------------------------------------
typedef struct{
	u32					fight_type;					///<戦闘種別フラグ
	int					win_lose_flag;				///<勝ち負けフラグ
	int					trainer_id[GT_CLIENT_MAX];		///<トレーナーのID
	GT_TRAINER_DATA		trainer_data[GT_CLIENT_MAX];	///<トレーナーデータ
	int					bg_id;						///<背景ID
	int					ground_id;					///<地形ID
	int					place_id;					///<地域名ID
	int					zone_id;					///<ゾーンID
	int					time_zone;					///<時間帯定義
	int					shinka_place_mode;			///<場所進化情報（SHINKA_PLACE～）
	int					contest_see_flag;			///<コンテストを見たかどうかのフラグ
	int					mizuki_flag;				///<ミズキにあったかどうかのフラグ
	int					get_pokemon_client;			///<捕獲したポケモンのClientNo（POKEPARTY_ENEMYかPOKEPARTY_ENEMY_PAIR）
	int					weather;					///<天候
	int					level_up_flag;				///<レベルアップしたポケモンフラグ（進化チェックをする）
	u32					server_version[GT_CLIENT_MAX];	///<戦闘サーバプログラムのバージョン（通信対戦時に使用）
	u32					battle_status_flag;			///<戦闘内分岐用フラグ
	int					safari_ball;				///<サファリボールの数
	u32					regulation_flag;			///<レギュレーションフラグ
	u32					rand;						///<戦闘専用乱数の種
	int					comm_stand_no[GT_CLIENT_MAX];	///<通信対戦部屋での自分の立ち位置
	u16					comm_id;					///<自分の通信ID
	u16					dummy;						///<パディング
	int					total_turn;					///<戦闘にかかったターン数
	u8					voice_waza_param[GT_CLIENT_MAX];	///<ぺラップボイスのおしゃべり時のパラメータ（録画データ再生時に使用）
}GT_REC_BATTLE_PARAM;

//----------------------------------------------------------
/**
 * @brief	バトルレギュレーションデータ型定義		32byte
 */
//----------------------------------------------------------
typedef struct {
	GT_STRCODE cupName[GT_REGULATION_NAME_SIZE + GT_EOM_SIZE];	//24
	u16 totalLevel;
	u8 num;
	u8 level;
    s8 height;
    s8 weight;
    u8 evolution:1;    //  
    u8 bLegend:1;
    u8 bBothMonster:1;
    u8 bBothItem:1;
    u8 bFixDamage:1;
    u8 padding;
}GT_REGULATION;


#endif	//__GT_GDS_BATTLE_REC_SUB_H__


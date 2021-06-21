//==============================================================================
/**
 * @file	frontier_savedata.h
 * @brief	フロンティアセーブ
 * @author	matsuda
 * @date	2007.04.26(木)
 */
//==============================================================================
#ifndef __FRONTIER_SAVEDATA_H__
#define __FRONTIER_SAVEDATA_H__


//==============================================================================
//	定数定義
//==============================================================================

///友達手帳データでないレコードを取得する時にfriend_noに指定する値
#define FRONTIER_RECORD_NOT_FRIEND		(0xff)

///連勝記録のリミット値
#define FRONTIER_RECORD_LIMIT		(9999)

//--------------------------------------------------------------
/**
 *	フロンティア各施設の記録データのID
 */
//--------------------------------------------------------------
enum{
	//タワー用INDEX開始
	FRID_TOWER_SINGLE_RENSHOU,
	FRID_TOWER_SINGLE_RENSHOU_CNT,
	FRID_TOWER_DOUBLE_RENSHOU,
	FRID_TOWER_DOUBLE_RENSHOU_CNT,
	FRID_TOWER_MULTI_NPC_RENSHOU,
	FRID_TOWER_MULTI_NPC_RENSHOU_CNT,
	FRID_TOWER_MULTI_COMM_RENSHOU,
	FRID_TOWER_MULTI_COMM_RENSHOU_CNT,
	FRID_TOWER_WIFI_DL_RENSHOU,
	FRID_TOWER_WIFI_DL_RENSHOU_CNT,
	
	//ファクトリー用INDEX開始(並びを変更してはダメ！)
	FRID_FACTORY_SINGLE_RENSHOU,
	FRID_FACTORY_SINGLE_RENSHOU_CNT,
	FRID_FACTORY_SINGLE_TRADE,
	FRID_FACTORY_SINGLE_TRADE_CNT,
	FRID_FACTORY_SINGLE_RENSHOU100,
	FRID_FACTORY_SINGLE_RENSHOU100_CNT,
	FRID_FACTORY_SINGLE_TRADE100,
	FRID_FACTORY_SINGLE_TRADE100_CNT,
	FRID_FACTORY_DOUBLE_RENSHOU,
	FRID_FACTORY_DOUBLE_RENSHOU_CNT,
	FRID_FACTORY_DOUBLE_TRADE,
	FRID_FACTORY_DOUBLE_TRADE_CNT,
	FRID_FACTORY_DOUBLE_RENSHOU100,
	FRID_FACTORY_DOUBLE_RENSHOU100_CNT,
	FRID_FACTORY_DOUBLE_TRADE100,
	FRID_FACTORY_DOUBLE_TRADE100_CNT,
	FRID_FACTORY_MULTI_COMM_RENSHOU,
	FRID_FACTORY_MULTI_COMM_RENSHOU_CNT,
	FRID_FACTORY_MULTI_COMM_TRADE,
	FRID_FACTORY_MULTI_COMM_TRADE_CNT,
	FRID_FACTORY_MULTI_COMM_RENSHOU100,
	FRID_FACTORY_MULTI_COMM_RENSHOU100_CNT,
	FRID_FACTORY_MULTI_COMM_TRADE100,
	FRID_FACTORY_MULTI_COMM_TRADE100_CNT,

	//ステージ用INDEX開始
	FRID_STAGE_SINGLE_RENSHOU,				///<ダミー(ポケモン毎の最大連勝数になる予定)
	FRID_STAGE_SINGLE_RENSHOU_CNT,			///<ステージ：シングル現在の連勝数
	FRID_STAGE_SINGLE_MONSNO,				///<ステージ：シングル挑戦しているポケモン番号
	FRID_STAGE_SINGLE_TYPE_LEVEL_0,
	FRID_STAGE_SINGLE_TYPE_LEVEL_1,
	FRID_STAGE_SINGLE_TYPE_LEVEL_2,
	FRID_STAGE_SINGLE_TYPE_LEVEL_3,
	FRID_STAGE_SINGLE_TYPE_LEVEL_4,
	FRID_STAGE_SINGLE_TYPE_LEVEL_5,
	FRID_STAGE_SINGLE_TYPE_LEVEL_6,
	FRID_STAGE_SINGLE_TYPE_LEVEL_7,
	FRID_STAGE_SINGLE_TYPE_LEVEL_8,
	FRID_STAGE_DOUBLE_RENSHOU,				///<ダミー(ポケモン毎の最大連勝数になる予定)
	FRID_STAGE_DOUBLE_RENSHOU_CNT,			///<ステージ：ダブル現在の連勝数
	FRID_STAGE_DOUBLE_MONSNO,				///<ステージ：ダブル挑戦しているポケモン番号
	FRID_STAGE_DOUBLE_TYPE_LEVEL_0,
	FRID_STAGE_DOUBLE_TYPE_LEVEL_1,
	FRID_STAGE_DOUBLE_TYPE_LEVEL_2,
	FRID_STAGE_DOUBLE_TYPE_LEVEL_3,
	FRID_STAGE_DOUBLE_TYPE_LEVEL_4,
	FRID_STAGE_DOUBLE_TYPE_LEVEL_5,
	FRID_STAGE_DOUBLE_TYPE_LEVEL_6,
	FRID_STAGE_DOUBLE_TYPE_LEVEL_7,
	FRID_STAGE_DOUBLE_TYPE_LEVEL_8,
	FRID_STAGE_MULTI_COMM_RENSHOU,			///<ダミー(ポケモン毎の最大連勝数になる予定)
	FRID_STAGE_MULTI_COMM_RENSHOU_CNT,		///<ステージ：ワイヤレス現在の連勝数
	FRID_STAGE_MULTI_COMM_MONSNO,			///<ステージ：ワイヤレス挑戦しているポケモン番号
	FRID_STAGE_MULTI_COMM_TYPE_LEVEL_0,
	FRID_STAGE_MULTI_COMM_TYPE_LEVEL_1,
	FRID_STAGE_MULTI_COMM_TYPE_LEVEL_2,
	FRID_STAGE_MULTI_COMM_TYPE_LEVEL_3,
	FRID_STAGE_MULTI_COMM_TYPE_LEVEL_4,
	FRID_STAGE_MULTI_COMM_TYPE_LEVEL_5,
	FRID_STAGE_MULTI_COMM_TYPE_LEVEL_6,
	FRID_STAGE_MULTI_COMM_TYPE_LEVEL_7,
	FRID_STAGE_MULTI_COMM_TYPE_LEVEL_8,
	
	//キャッスル用INDEX開始
	FRID_CASTLE_SINGLE_RENSHOU,				///<キャッスル：シングル最大連勝数
	FRID_CASTLE_SINGLE_RENSHOU_CNT,			///<キャッスル：シングル現在の連勝数
	FRID_CASTLE_SINGLE_CP,					///<キャッスル：シングル現在のキャッスルポイント
	FRID_CASTLE_SINGLE_USED_CP,				///<キャッスル：シングル使用したキャッスルポイント
	FRID_CASTLE_SINGLE_REMAINDER_CP,		///<キャッスル：シングル最大連勝数を記録した時の残りCP
	FRID_CASTLE_SINGLE_RANK_RECOVER,	///<キャッスル：どこまで上げたか記憶
	FRID_CASTLE_SINGLE_RANK_RENTAL,		///<キャッスル：どこまで上げたか記憶
	FRID_CASTLE_SINGLE_RANK_INFO,		///<キャッスル：どこまで上げたか記憶
	FRID_CASTLE_DOUBLE_RENSHOU,				///<キャッスル：ダブル最大連勝数
	FRID_CASTLE_DOUBLE_RENSHOU_CNT,			///<キャッスル：ダブル現在の連勝数
	FRID_CASTLE_DOUBLE_CP,					///<キャッスル：ダブル現在のキャッスルポイント
	FRID_CASTLE_DOUBLE_USED_CP,				///<キャッスル：ダブル使用したキャッスルポイント
	FRID_CASTLE_DOUBLE_REMAINDER_CP,		///<キャッスル：ダブル最大連勝数を記録した時の残りCP
	FRID_CASTLE_DOUBLE_RANK_RECOVER,	///<キャッスル：どこまで上げたか記憶
	FRID_CASTLE_DOUBLE_RANK_RENTAL,		///<キャッスル：どこまで上げたか記憶
	FRID_CASTLE_DOUBLE_RANK_INFO,		///<キャッスル：どこまで上げたか記憶
	FRID_CASTLE_MULTI_COMM_RENSHOU,			///<キャッスル：ワイヤレス最大連勝数
	FRID_CASTLE_MULTI_COMM_RENSHOU_CNT,		///<キャッスル：ワイヤレス現在の連勝数
	FRID_CASTLE_MULTI_COMM_CP,				///<キャッスル：ワイヤレス現在のキャッスルポイント
	FRID_CASTLE_MULTI_COMM_USED_CP,			///<キャッスル：ワイヤレス使用したキャッスルポイント
	FRID_CASTLE_MULTI_COMM_REMAINDER_CP,///<キャッスル：ワイヤレス最大連勝数を記録した時の残りCP
	FRID_CASTLE_MULTI_COMM_RANK_RECOVER,	///<キャッスル：どこまで上げたか記憶
	FRID_CASTLE_MULTI_COMM_RANK_RENTAL,		///<キャッスル：どこまで上げたか記憶
	FRID_CASTLE_MULTI_COMM_RANK_INFO,		///<キャッスル：どこまで上げたか記憶
	
	//ルーレット用INDEX開始
	FRID_ROULETTE_SINGLE_RENSHOU,			///<ルーレット：シングル最大連勝数
	FRID_ROULETTE_SINGLE_RENSHOU_CNT,		///<ルーレット：シングル現在の連勝数
	FRID_ROULETTE_DOUBLE_RENSHOU,			///<ルーレット：ダブル最大連勝数
	FRID_ROULETTE_DOUBLE_RENSHOU_CNT,		///<ルーレット：ダブル現在の連勝数
	FRID_ROULETTE_MULTI_COMM_RENSHOU,		///<ルーレット：ワイヤレス最大連勝数
	FRID_ROULETTE_MULTI_COMM_RENSHOU_CNT,	///<ルーレット：ワイヤレス現在の連勝数

	FRID_SINGLE_END,	//一人用のデータ終端
	
	//WIFIの友達毎のクリアフラグ(ビット単位で管理しています)
	FRID_TOWER_MULTI_WIFI_CLEAR_BIT = FRID_SINGLE_END,	//FrontierRecord_Set,Get関数のみ対応
	FRID_TOWER_MULTI_WIFI_CLEAR_BIT_SUB,		//※使用禁止　領域確保のみ
	FRID_FACTORY_MULTI_WIFI_CLEAR_BIT,			//FrontierRecord_Set,Get関数のみ対応
	FRID_FACTORY_MULTI_WIFI_CLEAR_BIT_SUB,		//※使用禁止　領域確保のみ
	FRID_FACTORY_MULTI_WIFI_CLEAR100_BIT,		//FrontierRecord_Set,Get関数のみ対応
	FRID_FACTORY_MULTI_WIFI_CLEAR100_BIT_SUB,	//※使用禁止　領域確保のみ
	FRID_STAGE_MULTI_WIFI_CLEAR_BIT,			//FrontierRecord_Set,Get関数のみ対応
	FRID_STAGE_MULTI_WIFI_CLEAR_BIT_SUB,		//※使用禁止　領域確保のみ
	FRID_CASTLE_MULTI_WIFI_CLEAR_BIT,			//FrontierRecord_Set,Get関数のみ対応
	FRID_CASTLE_MULTI_WIFI_CLEAR_BIT_SUB,		//※使用禁止　領域確保のみ
	FRID_ROULETTE_MULTI_WIFI_CLEAR_BIT,			//FrontierRecord_Set,Get関数のみ対応
	FRID_ROULETTE_MULTI_WIFI_CLEAR_BIT_SUB,		//※使用禁止　領域確保のみ

	//終端
	FRID_MAX,

	//--------------------------------------------------------------
	//	友達手帳毎に持つレコード
	//--------------------------------------------------------------
	//Wifiタワー
	FRID_TOWER_MULTI_WIFI_RENSHOU = FRID_MAX,
	FRID_TOWER_MULTI_WIFI_RENSHOU_CNT,
	
	//Wifiファクトリー(並びを変更してはダメ！)
	FRID_FACTORY_MULTI_WIFI_RENSHOU,
	FRID_FACTORY_MULTI_WIFI_RENSHOU_CNT,
	FRID_FACTORY_MULTI_WIFI_TRADE,
	FRID_FACTORY_MULTI_WIFI_TRADE_CNT,
	FRID_FACTORY_MULTI_WIFI_RENSHOU100,
	FRID_FACTORY_MULTI_WIFI_RENSHOU100_CNT,
	FRID_FACTORY_MULTI_WIFI_TRADE100,
	FRID_FACTORY_MULTI_WIFI_TRADE100_CNT,
	
	//Wifiステージ							//↓は別に外部でポケモンごとの最大連勝数をもっている
	FRID_STAGE_MULTI_WIFI_RENSHOU,			//今やっているポケモンの最大連勝数
	FRID_STAGE_MULTI_WIFI_RENSHOU_CNT,		//今やっているポケモンの連勝数
	FRID_STAGE_MULTI_WIFI_MONSNO,
	FRID_STAGE_MULTI_WIFI_TYPE_LEVEL_0,
	FRID_STAGE_MULTI_WIFI_TYPE_LEVEL_1,
	FRID_STAGE_MULTI_WIFI_TYPE_LEVEL_2,
	FRID_STAGE_MULTI_WIFI_TYPE_LEVEL_3,
	FRID_STAGE_MULTI_WIFI_TYPE_LEVEL_4,
	FRID_STAGE_MULTI_WIFI_TYPE_LEVEL_5,
	FRID_STAGE_MULTI_WIFI_TYPE_LEVEL_6,
	FRID_STAGE_MULTI_WIFI_TYPE_LEVEL_7,
	FRID_STAGE_MULTI_WIFI_TYPE_LEVEL_8,
	
	//Wifiキャッスル
	FRID_CASTLE_MULTI_WIFI_RENSHOU,
	FRID_CASTLE_MULTI_WIFI_RENSHOU_CNT,
	FRID_CASTLE_MULTI_WIFI_CP,
	FRID_CASTLE_MULTI_WIFI_USED_CP,
	FRID_CASTLE_MULTI_WIFI_REMAINDER_CP,	///<キャッスル：WIFI最大連勝数を記録した時の残りCP
	FRID_CASTLE_MULTI_WIFI_RANK_RECOVER,	///<キャッスル：どこまで上げたか記憶
	FRID_CASTLE_MULTI_WIFI_RANK_RENTAL,		///<キャッスル：どこまで上げたか記憶
	FRID_CASTLE_MULTI_WIFI_RANK_INFO,		///<キャッスル：どこまで上げたか記憶

	//Wifiルーレット
	FRID_ROULETTE_MULTI_WIFI_RENSHOU,
	FRID_ROULETTE_MULTI_WIFI_RENSHOU_CNT,
	
	//Wifi終端
	FRID_WIFI_MAX,
};


//==============================================================================
//	構造体定義
//==============================================================================
/**
 *	@brief	フロンティア　ブロックデータ構造体への不完全型ポインタ
 *
 * 中身は見えませんがポインタ経由で参照できます
 */
typedef struct _FRONTIER_SAVEWORK	FRONTIER_SAVEWORK;


//==============================================================================
//	外部関数宣言
//==============================================================================
extern int FrontierData_GetWorkSize(void);
extern void FrontierData_Init(FRONTIER_SAVEWORK* save);
extern u16 FrontierRecord_Get(FRONTIER_SAVEWORK* save, int id, int friend_no);
extern u16 FrontierRecord_Set(FRONTIER_SAVEWORK* save, int id, int friend_no, u16 data);
extern u16 FrontierRecord_Add(FRONTIER_SAVEWORK* save, int id, int friend_no, int add_value);
extern u16 FrontierRecord_Sub(FRONTIER_SAVEWORK* save, int id, int friend_no, int sub_value);
extern u16 FrontierRecord_Inc(FRONTIER_SAVEWORK* save, int id, int friend_no);
extern FRONTIER_SAVEWORK * SaveData_GetFrontier(SAVEDATA * sv);
extern u16 FrontierRecord_SetIfLarge(FRONTIER_SAVEWORK *save, int id, int friend_no, u16 data);
extern void FrontierRecord_ResetData(FRONTIER_SAVEWORK* save, int friend_no);
extern void FrontierRecord_DataMarge(FRONTIER_SAVEWORK* save, int delNo, int friend_no);
extern void FrontierRecord_DataCrear(FRONTIER_SAVEWORK *save);


#endif	//__FRONTIER_SAVEDATA_H__

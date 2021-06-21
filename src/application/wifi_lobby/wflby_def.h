//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_def.h
 *	@brief		WiFiロビー	通信構造体
 *	@author		tomoya	takahashi	
 *	@data		2007.09.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WFLBY_DEF_H__
#define __WFLBY_DEF_H__

#include <nitro.h>
#include "system/buflen.h"
#include "system/pms_word.h"

#include "battle/battle_common.h"

#include "msgdata/msg_wifi_place_msg_world.h"


//-----------------------------------------------------------------------------
/**
 *			全体デバック
 */
//-----------------------------------------------------------------------------
#ifdef PM_DEBUG
//#define WFLBY_DEBUG_ROOM_WLDTIMER_AUTO	// オートで世界時計に入ったり出たり
//#define WFLBY_DEBUG_ROOM_MINIGAME_AUTO	// オートでミニゲーム出たり入ったり
#endif

#ifdef WFLBY_DEBUG_ROOM_WLDTIMER_AUTO
extern BOOL WFLBY_DEBUG_ROOM_WFLBY_TIMER_AUTO;
#endif

#ifdef WFLBY_DEBUG_ROOM_MINIGAME_AUTO
extern BOOL WFLBY_DEBUG_ROOM_MINIGAME_AUTO_FLAG;
#endif


//-----------------------------------------------------------------------------
/**
 *				全共通
 */
//-----------------------------------------------------------------------------

//-------------------------------------
/// 部屋終了イベントから部屋を終了させるまでの時間
//=====================================
#define WFLBY_END_OVERTIME		( 30*30 )	// 30秒
#define WFLBY_END_BGMFADEOUT	( 28*30 )	// のこり28秒になったらBGMをフェードアウトさせる

//-------------------------------------
/// 通常のイベントが受信してから反映させるまでの時間
//=====================================
#define WFLBY_EVENT_STARTTIME	( 10*30 )	// 10秒

//-------------------------------------
///	1部屋の人数
//=====================================
#define WFLBY_PLAYER_MAX	(20)

//-------------------------------------
///	マップ配置人物のプレイヤーNO
//=====================================
#define WFLBY_MAPPERSON_ID			( 0xff )	// カウンターにいる人
#define WFLBY_MAPPERSON_PIERROT_ID	( 0xfe )	// ピエロ（ピエロは人と同じように話しかけることが出来るのでplayidのチェックをしてください）

//-------------------------------------
///	ミニゲーム人数
//=====================================
#define WFLBY_MINIGAME_MAX	(4)

//-------------------------------------
///	ミニゲーム親BCANCEL	参加ロック時間
//=====================================
#define WFLBY_MINIGAME_LOCK_TIME	(30*30)


//-------------------------------------
///	足跡ボード人数
//=====================================
#define WFLBY_FOOT_MAX	(8)

//-------------------------------------
///	1部屋に出すフロートの最大数
//=====================================
#define WFLBY_FLOAT_MAX		(9)

//-------------------------------------
///	1フロートの定員
//=====================================
#define WFLBY_FLOAT_ON_NUM		(3)

//-------------------------------------
///		世界データ 無効国ID
//=====================================
#define WFLBY_WLDDATA_NATION_NONE		( country000 )	// 無効な国ID

//-------------------------------------
///	1人と会話できる最大数
//=====================================
#define WFLBY_TALK_MAX		( 6 )

//-------------------------------------
///	会話応答待ち時間
//=====================================
#define WFLBY_TALK_RECV_WAIT		( 900 )		// 30秒
#define WFLBY_TALK_RECV_HALF_WAIT	( 450 )		// 15秒	中間メッセージの表示

//-------------------------------------
///	不正な言語コードのとき
//=====================================
#define WFLBY_REGIONCODE_ERR	( LANG_ENGLISH )	// 英国の人として扱います

//-------------------------------------
///	行ったことの履歴　保持数
//=====================================
#define WFLBY_LASTACT_BUFFNUM	( 12 )

//-------------------------------------
///	ユーザがプロフィール用に選択する属性タイプ
//=====================================
#define WFLBY_SELECT_TYPENUM		(2)								// 選択数
typedef enum{
	WFLBY_POKEWAZA_TYPE_NONE,	// 選ばない
	WFLBY_POKEWAZA_TYPE_NORMAL,
	WFLBY_POKEWAZA_TYPE_BATTLE,
	WFLBY_POKEWAZA_TYPE_HIKOU,
	WFLBY_POKEWAZA_TYPE_POISON,
	WFLBY_POKEWAZA_TYPE_JIMEN,
	WFLBY_POKEWAZA_TYPE_IWA,
	WFLBY_POKEWAZA_TYPE_MUSHI,
	WFLBY_POKEWAZA_TYPE_GHOST,
	WFLBY_POKEWAZA_TYPE_METAL,
	WFLBY_POKEWAZA_TYPE_FIRE,
	WFLBY_POKEWAZA_TYPE_WATER,
	WFLBY_POKEWAZA_TYPE_KUSA,
	WFLBY_POKEWAZA_TYPE_ELECTRIC,
	WFLBY_POKEWAZA_TYPE_SP,
	WFLBY_POKEWAZA_TYPE_KOORI,
	WFLBY_POKEWAZA_TYPE_DRAGON,
	WFLBY_POKEWAZA_TYPE_AKU,
	WFLBY_POKEWAZA_TYPE_NUM,
} WFLBY_POKEWAZA_TYPE;


//-------------------------------------
///	ミニゲームタイプ
//=====================================
typedef enum{
	WFLBY_GAME_BALLSLOW,	// 玉投げ
	WFLBY_GAME_BALANCEBALL,	// 玉乗り
	WFLBY_GAME_BALLOON,		// ふうせんわり
	WFLBY_GAME_FOOTWHITE,	// 足跡ボード　白
	WFLBY_GAME_FOOTBLACK,	// 足跡ボード　黒
	WFLBY_GAME_WLDTIMER,	// 世界時計
	WFLBY_GAME_NEWS,		// ロビーニュース
	WFLBY_GAME_TALK,		// おしゃべり
	WFLBY_GAME_ITEM,		// ガジェット
	WFLBY_GAME_ITEMCHG,		// ガジェット交換

	WFLBY_GAME_UNKNOWN,		// わからない

	WFLBY_GAME_NUM,			// 数

	// P2P通信するミニゲームの数
	WFLBY_GAME_P2PGAME_NUM = WFLBY_GAME_BALLOON + 1,
} WFLBY_GAMETYPE;

//-------------------------------------
///	時間イベントタイプ
//=====================================
typedef enum{
	WFLBY_EVENT_NEON,		// ネオン
	WFLBY_EVENT_FIRE,		// ファイアー
	WFLBY_EVENT_PARADE,		// パレード
	WFLBY_EVENT_LIGHT_OFF,	// ライトオフ
	WFLBY_EVENT_CLOSE,		// クローズ

	WFLBY_EVENT_UNKNOWN,	// わからない
	
	WFLBY_EVENT_NUM,		// 数
} WFLBY_EVENTTYPE;

//-------------------------------------
///	タイムイベント	メッセージ定数
//=====================================
typedef enum{
	WFLBY_EVENT_GMM_NEON,		// ネオン
	WFLBY_EVENT_GMM_FIRE,		// ファイアー
	WFLBY_EVENT_GMM_PARADE,		// パレード
	
	WFLBY_EVENT_GMM_NUM,		// 数
} WFLBY_EVENTGMM_TYPE;

//-------------------------------------
///	アイテムタイプ
//=====================================
enum{
	WFLBY_ITEM_BELL00,		// ベル小
	WFLBY_ITEM_BELL01,		// ベル中
	WFLBY_ITEM_BELL02,		// ベル大
	WFLBY_ITEM_DRAM00,		// ドラム小
	WFLBY_ITEM_DRAM01,		// ドラム中
	WFLBY_ITEM_DRAM02,		// ドラム大
	WFLBY_ITEM_CYMBALS00,	// シンバル小
	WFLBY_ITEM_CYMBALS01,	// シンバル中
	WFLBY_ITEM_CYMBALS02,	// シンバル大
	WFLBY_ITEM_RIPPRU00,	// リップル小
	WFLBY_ITEM_RIPPRU01,	// リップル中
	WFLBY_ITEM_RIPPRU02,	// リップル大
	WFLBY_ITEM_SIGNAL00,	// シグナル小
	WFLBY_ITEM_SIGNAL01,	// シグナル中
	WFLBY_ITEM_SIGNAL02,	// シグナル大
	WFLBY_ITEM_WINK00,		// ウィンク小
	WFLBY_ITEM_WINK01,		// ウィンク中
	WFLBY_ITEM_WINK02,		// ウィンク大
	WFLBY_ITEM_CRACKER00,	// クラッカー小
	WFLBY_ITEM_CRACKER01,	// クラッカー中
	WFLBY_ITEM_CRACKER02,	// クラッカー大
	WFLBY_ITEM_SPARK00,		// スパークル小
	WFLBY_ITEM_SPARK01,		// スパークル中
	WFLBY_ITEM_SPARK02,		// スパークル大
	WFLBY_ITEM_BALLOON00,	// バルーン小
	WFLBY_ITEM_BALLOON01,	// バルーン中
	WFLBY_ITEM_BALLOON02,	// バルーン大
	WFLBY_ITEM_NUM,		// 数

	// ガジェット初期値	（まだ何をもらったか不明なとき）
	WFLBY_ITEM_INIT = 0xff,

	// ひとつの種類のアイテムにある段階数
	WFLBY_ITEM_DANKAI = 3,
} ;
// ガジェットのGROUPの種類
#define WFLBY_ITEM_GROUPNUM (WFLBY_ITEM_NUM/WFLBY_ITEM_DANKAI) 
typedef u32 WFLBY_ITEMTYPE;	// アイテム用型宣言

//-------------------------------------
///	トピックスデータ
//=====================================
typedef enum {	// トピックスタイプ
	NEWS_TOPICTYPE_CONNECT,			// 挨拶
	NEWS_TOPICTYPE_ITEM,			// ガジェットの交換
	NEWS_TOPICTYPE_MINIGAME,		// ミニゲーム
	NEWS_TOPICTYPE_FOOTBOARD,		// あしあとボード
	NEWS_TOPICTYPE_WORLDTIMER,		// 世界時計
	NEWS_TOPICTYPE_LOBBYNEWS,		// ロビーニュース
	NEWS_TOPICTYPE_TIMEEVENT,		// 時間イベント
	NEWS_TOPICTYPE_VIPIN,			// VIPが入室したときのニュース
	NEWS_TOPICTYPE_MGRESULT,		// ミニゲーム結果

	NEWS_TOPICTYPE_NUM,		// トピックタイプ数
} NEWS_TOPICTYPE;

//-------------------------------------
///	部屋タイプ
//=====================================
typedef enum{
	WFLBY_ROOM_FIRE,			// 火
	WFLBY_ROOM_WARTER,			// 水
	WFLBY_ROOM_ELEC,			// 電気
	WFLBY_ROOM_GRASS,			// 草
	WFLBY_ROOM_SPECIAL,			// 特殊
	WFLBY_ROOM_NUM,				// 部屋数
} WFLBY_ROOM_TYPE;

//-------------------------------------
///	シーズンタイプ
//=====================================
typedef enum {
	WFLBY_SEASON_NONE,
	WFLBY_SEASON_SPRING,
	WFLBY_SEASON_SUMMER,
	WFLBY_SEASON_FALL,
	WFLBY_SEASON_WINTER,
	WFLBY_SEASON_NUM,
} WFLBY_SEASON_TYPE;

//-------------------------------------
///	ステータス
//=====================================
typedef enum {
	WFLBY_STATUS_NONE,
	WFLBY_STATUS_LOGIN,			// ログイン状態(通常)
	WFLBY_STATUS_BALLSLOW,		// 玉いれ
	WFLBY_STATUS_BALANCEBALL,	// 玉乗り
	WFLBY_STATUS_BALLOON,		// 風船わり
	WFLBY_STATUS_FOOTBOAD00,	// 足跡ボード
	WFLBY_STATUS_FOOTBOAD01,	// 足跡ボード
	WFLBY_STATUS_WORLDTIMER,	// 世界時計
	WFLBY_STATUS_TOPIC,			// ニュース
	WFLBY_STATUS_TALK,			// 会話
	WFLBY_STATUS_FLOAT,			// フロート
	WFLBY_STATUS_LOGOUT,		// 退室
	WFLBY_STATUS_MATCHING,		// ミニゲームのマッチング中
	WFLBY_STATUS_ANKETO,		// アンケート中
	

	WFLBY_STATUS_UNKNOWN,		// 不明
	
	WFLBY_STATUS_NUM,
} WFLBY_STATUS_TYPE;

//-------------------------------------
///	挨拶時間帯
//=====================================
typedef enum {
	WFLBY_AISATSU_MORNING,
	WFLBY_AISATSU_NOON,
	WFLBY_AISATSU_NIGHT,
} WFLBY_AISATSU_TIMEZONE;



//-------------------------------------
///	ライトデータ
//=====================================
typedef enum {
	WFLBY_DRAW_LIGHT_ROOM,	// 室内ライト
	WFLBY_DRAW_LIGHT_LAMP,	// ランプライト
	WFLBY_DRAW_LIGHT_CLOFF,	// クローズ後消えるライト
	WFLBY_DRAW_LIGHT_FLOAT,	// フロート用ライト
} WFLBY_DRAW_LIGHT_TYPE;




//-------------------------------------
///	室内ネオン諧調
//=====================================
typedef enum {
	WFLBY_LIGHT_NEON_ROOM0,
	WFLBY_LIGHT_NEON_ROOM1,
	WFLBY_LIGHT_NEON_ROOM2,
	WFLBY_LIGHT_NEON_ROOM3,
	WFLBY_LIGHT_NEON_ROOM4,
	WFLBY_LIGHT_NEON_ROOM5,
	WFLBY_LIGHT_NEON_ROOMNUM,
} WFLBY_LIGHT_NEON_ROOMTYPE;

//-------------------------------------
///	床ネオン諧調
//=====================================
typedef enum {
	WFLBY_LIGHT_NEON_FLOOR0,
	WFLBY_LIGHT_NEON_FLOOR1,
	WFLBY_LIGHT_NEON_FLOOR2,
	WFLBY_LIGHT_NEON_FLOOR3,
	WFLBY_LIGHT_NEON_FLOORNUM,
} WFLBY_LIGHT_NEON_FLOORTYPE;

//-------------------------------------
///	モニュメントネオン諧調
//=====================================
typedef enum {
	WFLBY_LIGHT_NEON_MONU0,
	WFLBY_LIGHT_NEON_MONU1,
	WFLBY_LIGHT_NEON_MONU2,
	WFLBY_LIGHT_NEON_MONU3,
	WFLBY_LIGHT_NEON_MONUNUM,
} WFLBY_LIGHT_NEON_MONUTYPE;


//-------------------------------------
///	履歴定数
//=====================================
typedef enum {
	WFLBY_LASTACTION_BOY1,			// 人物との接触
	WFLBY_LASTACTION_BOY3,
	WFLBY_LASTACTION_MAN3,
	WFLBY_LASTACTION_BADMAN,
	WFLBY_LASTACTION_EXPLORE,
	WFLBY_LASTACTION_FIGHTER,
	WFLBY_LASTACTION_GORGGEOUSM,
	WFLBY_LASTACTION_MYSTERY,
	WFLBY_LASTACTION_GIRL1,
	WFLBY_LASTACTION_GIRL2,
	WFLBY_LASTACTION_WOMAN2,
	WFLBY_LASTACTION_WOMAN3,
	WFLBY_LASTACTION_IDOL,
	WFLBY_LASTACTION_LADY,
	WFLBY_LASTACTION_COWGIRL,
	WFLBY_LASTACTION_GORGGEOUSW,

	WFLBY_LASTACTION_BALLSLOW,		// 玉投げ
	WFLBY_LASTACTION_BALANCEBALL,	// 玉乗り
	WFLBY_LASTACTION_BALLOON,		// ふうせんわり
	WFLBY_LASTACTION_FOOTWHITE,		// 足跡ボード　白
	WFLBY_LASTACTION_FOOTBLACK,		// 足跡ボード　黒
	WFLBY_LASTACTION_WLDTIMER,		// 世界時計
	WFLBY_LASTACTION_NEWS,			// ロビーニュース
	WFLBY_LASTACTION_ANKETO,		// アンケート

	// プラチナ後はこれ以降に追加

	WFLBY_LASTACTION_MAX,
} WFLBY_LASTACTION_TYPE;



//-------------------------------------
///	アンケート基本情報
//=====================================
#define ANKETO_NORMAL_Q_NUM		( 50 )
#define ANKETO_SPECIAL_Q_NUM	( 10 )
enum{
	ANKETO_ANSWER_A,
	ANKETO_ANSWER_B,
	ANKETO_ANSWER_C,
	ANKETO_ANSWER_NUM,
};



//-------------------------------------
///	時間		4byte
//=====================================
typedef struct {
	union{
		u32 time;
		struct{
			u8		hour;
			u8		minute;
			u8		second;
			u8		dummy;
		};
	};
} WFLBY_TIME;
// 操作関数
// cp_time cp_add p_anm　すべて同じものでも大丈夫です
extern void WFLBY_TIME_Set( WFLBY_TIME* p_time, const s64* cp_settime );
extern void WFLBY_TIME_Add( const WFLBY_TIME* cp_time, const WFLBY_TIME* cp_add, WFLBY_TIME* p_ans );
extern void WFLBY_TIME_Sub( const WFLBY_TIME* cp_time, const WFLBY_TIME* cp_sub, WFLBY_TIME* p_ans );


//-----------------------------------------------------------------------------
/**
 *				ロビーニュース　トピックデータ
 */
//-----------------------------------------------------------------------------
#define NEWS_TOPICNAME_NUM	( 4 )	// 表示される人の最大数

//-------------------------------------
///	ロビーニュース　トピック送信データ
//	20byte
//=====================================
typedef struct{
	s32	lobby_id[NEWS_TOPICNAME_NUM];	// 広場のID	32byte
	u8	num;							// 参加者
	u8	topic_type;						// トピックタイプ		NEWS_TOPICTYPE
	u8	item;							// アイテムナンバー		WFLBY_ITEMTYPE
	u8	minigame:7;						// ミニゲームナンバー	WFLBY_GAMETYPE
	u8	play:1;							// ミニゲームを遊んでいる常態か
} WFLBY_TOPIC;


//-----------------------------------------------------------------------------
/**
 *				世界地図データ
 */
//-----------------------------------------------------------------------------
#define WFLBY_WLDTIMER_DATA_MAX		(50)	// 最大格納地域数
//-------------------------------------
///	1地域データ
//	4byte
//=====================================
typedef struct {
	u16 nation;			///<国
	u8  area;			///<地域
	u8  outside:4;		///<退室フラグ
	u8  flag:4;			///<データ有無	（データが格納されているのかどうか）
} WFLBY_COUNTRY;
//-------------------------------------
///	世界地図地域データ
//	8*WFLBY_WLDTIMER_DATA_MAX	byte
//=====================================
typedef struct _WFLBY_WLDTIMER{
	WFLBY_COUNTRY	data[WFLBY_WLDTIMER_DATA_MAX];
} WFLBY_WLDTIMER;
//-------------------------------------
///	アクセス関数
//=====================================
extern void WFLBY_WLDTIMER_SetData( WFLBY_WLDTIMER* p_data, u16 nation, u8 area, BOOL outside );
extern u16 WFLBY_WLDTIMER_GetNation( const WFLBY_WLDTIMER* cp_data, u8 index );
extern u8 WFLBY_WLDTIMER_GetArea( const WFLBY_WLDTIMER* cp_data, u8 index );
extern BOOL WFLBY_WLDTIMER_GetOutSide( const WFLBY_WLDTIMER* cp_data, u8 index );
extern BOOL WFLBY_WLDTIMER_GetDataFlag( const WFLBY_WLDTIMER* cp_data, u8 index );





//-----------------------------------------------------------------------------
/**
 *			足跡データ
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	1足跡データ
//	12byte
//=====================================
typedef struct {
	u16		monsno;		///< モンスターナンバー
	u16		col;		///< 足跡の色
	s16		x;			///< ｘ座標
	s16		y;			///< ｙ座標
	u8		formno;		///< フォルムナンバー
	u8		move;		///< 動作タイプ

	u16		pad;
} WFLBY_FOOT;



#if 0
#define WFLBY_FTBRD_DATA_MAX	( 600 )	// 最大件数
//-------------------------------------
///	1足跡データ
//	8byte
//=====================================
typedef struct {
	u16		monsno;		///< モンスターナンバー
	u8		formno;		///< フォルムナンバー
	u8		sex;		///< 性別
	u16		col;		///< 足跡の色
	u8		flag;		///< データ有無	
	u8		dummy;		///< 
} WFLBY_FOOT;

//-------------------------------------
///	足跡データ
//	8*WFLBY_FTBRD_DATA_MAX	byte
//=====================================
typedef struct _WFLBY_FTBRD{
	WFLBY_FOOT	data[WFLBY_FTBRD_DATA_MAX];
} WFLBY_FTBRD;
#endif

//-----------------------------------------------------------------------------
/**
 *			ライトリソース操作
 *				ライトの影響を受けるモデルはここを通す
 */
//-----------------------------------------------------------------------------
extern void WFLBY_LIGHT_SetUpMdl( void* p_file );





//-----------------------------------------------------------------------------
/**
 *			あいことば	データ
 */
//-----------------------------------------------------------------------------
// ワード数
#define WFLBY_AIKOTOBA_WORD_NUM	( 4 )	

//-------------------------------------
///	あいことばバッファ
//=====================================
typedef struct { 
	PMS_WORD word[ WFLBY_AIKOTOBA_WORD_NUM ];
} WFLBY_AIKOTOBA_DATA;




//-----------------------------------------------------------------------------
/**
 *			P2Pミニゲームなどに使用するワーク
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	VIPフラグバッファ
//=====================================
typedef struct {
	u8  vip[ WFLBY_PLAYER_MAX ];	// 全プレイヤー分のVIPフラグ
} WFLBY_VIPFLAG;

//-------------------------------------
///	ミニゲーム用PLIDXバッファ
//=====================================
typedef struct {
	u8  plidx[ WFLBY_MINIGAME_MAX ];	// 全プレイヤー分のロビー内人物IDX
} WFLBY_MINIGAME_PLIDX;


//-------------------------------------
///	アンケート保持ワーク
//=====================================
typedef struct {
	u32	anketo_no;		// アンケート通しナンバー
	u32	select;			// 選択ナンバー
} WFLBY_ANKETO;


//-------------------------------------
///	アンケート結果情報
//=====================================
typedef struct {
	u32	ans[ ANKETO_ANSWER_NUM ];	
	u64 ans_all;
} ANKETO_QUESTION_RESULT;




//-----------------------------------------------------------------------------
/**
 *			地域があるのかチェック
 */
//-----------------------------------------------------------------------------
extern BOOL WFLBY_AREA_Check( u16 nation, u16 area );


//-----------------------------------------------------------------------------
/**
 *			広場内技タイプ定数から戦闘用技タイプ定数を求める関数
 */
//-----------------------------------------------------------------------------
extern u32 WFLBY_BattleWazaType_Get( WFLBY_POKEWAZA_TYPE type );




//-----------------------------------------------------------------------------
/**
 *			エラー処理
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	その後どうするべきかの定数
//=====================================
typedef enum {
	WFLBY_ERR_TYPE_RETRY,	// 再接続
	WFLBY_ERR_TYPE_END,		// 終了へ
} WFLBY_ERR_TYPE;


extern int WFLBY_ERR_GetStrID( int errno,int errtype );
extern WFLBY_ERR_TYPE WFLBY_ERR_GetErrType( int errno,int errtype );

// エラーが発生中なのかをチェック
extern BOOL WFLBY_ERR_CheckError( void );



#endif		// __WFLBY_DEF_H__


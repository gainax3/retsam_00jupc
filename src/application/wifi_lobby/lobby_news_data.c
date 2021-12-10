//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		lobby_news_data.c
 *	@brief		ロビーニュース	データ格納部分
 *	@author		tomoya takahashi
 *	@data		2007.10.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"

#include "gflib/strbuf.h"

#include "system/buflen.h"
#include "system/wordset.h"
#include "system/msgdata.h"

#include "msgdata/msg.naix"
#include "msgdata/msg_wflby_news.h"

#include "lobby_news_data.h"

//-----------------------------------------------------------------------------
/**
 *					コーディング規約
 *		●関数名
 *				１文字目は大文字それ以降は小文字にする
 *		●変数名
 *				・変数共通
 *						constには c_ を付ける
 *						staticには s_ を付ける
 *						ポインタには p_ を付ける
 *						全て合わさると csp_ となる
 *				・グローバル変数
 *						１文字目は大文字
 *				・関数内変数
 *						小文字と”＿”と数字を使用する 関数の引数もこれと同じ
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	トピック関連
//=====================================
#define NEWS_TOPIC_MAX		( 8 )							// トピック最大数
#define NEWS_TOPICNAME_LEN	( PERSON_NAME_SIZE+EOM_SIZE )	// トレーナ文字列サイズ

enum{		// 優先順位
	NEWS_TOPIC_PRI_NONE,
	NEWS_TOPIC_PRI_VIPIN,
	NEWS_TOPIC_PRI_MINIGAME_02,
	NEWS_TOPIC_PRI_MINIGAME_01,
	NEWS_TOPIC_PRI_MINIGAME_00,
	NEWS_TOPIC_PRI_FOOTBOARD_00,
	NEWS_TOPIC_PRI_FOOTBOARD_01,
	NEWS_TOPIC_PRI_ITEM,
	NEWS_TOPIC_PRI_CONNECT,
	NEWS_TOPIC_PRI_MGRESULT02,	// 結果	どんどんソーナンス
	NEWS_TOPIC_PRI_MGRESULT01,	// 結果	マネネ
	NEWS_TOPIC_PRI_MGRESULT00,	// 結果 マルノーム
	NEWS_TOPIC_PRI_WORLDTIMER,
	NEWS_TOPIC_PRI_LOBBYNEWS,
	NEWS_TOPIC_PRI_TIMEEVENT,
};
enum{		// 各トピックのワーク使用方法

	NEWS_TOPIC_WK_NUM		= 3,	// ワークの総数

	// 挨拶

	// アイテムシェア
	NEWS_TOPIC_WK_ITEMNO		= 0,

	// ミニゲーム
	NEWS_TOPIC_WK_GAME_NO		= 0,
	NEWS_TOPIC_WK_GAME_PLAYNUM,
	NEWS_TOPIC_WK_GAME_PLAYFLAG,

	// あしあと
	NEWS_TOPIC_WK_FOOT_NO		= 0,
	NEWS_TOPIC_WK_FOOT_PLAYNUM,

	// 世界時計
	NEWS_TOPIC_WK_WLDT_PLAYNUM	= 0,

	// ロビーニュース
	NEWS_TOPIC_WK_LBYN_PLAYNUM	= 0,

	// 時間イベント
	NEWS_TOPIC_WK_TIEV_TIME		= 0,
	NEWS_TOPIC_WK_TIEV_NO,

	// VIP入室
	NEWS_TOPIC_WK_VIPIN_IDX		= 0,

	// ミニゲーム結果
	NEWS_TOPIC_WK_MGRESULT_NO		= 0,
	NEWS_TOPIC_WK_MGRESULT_PLAYNUM,
};
enum{		// 各トピックの破棄カウント
	// ミニゲーム
	NEWS_TOPIC_DEST_VIPIN		= 60 * 30,
	NEWS_TOPIC_DEST_GAME		= 30 * 30,
	NEWS_TOPIC_DEST_FOOT		= 30 * 30,
	NEWS_TOPIC_DEST_ITEM		= 20 * 30,
	NEWS_TOPIC_DEST_CONNECT		= 20 * 30,
	NEWS_TOPIC_DEST_MGRESULT	= 15 * 30,
	NEWS_TOPIC_DEST_WLDTIME		= 15 * 30,
	NEWS_TOPIC_DEST_LBYNEWS		= 15 * 30,
	NEWS_TOPIC_DEST_TIEV		= 30 * 30,
};


//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	接続プレイヤーデータ
//=====================================
//1プレイヤーデータ
typedef struct {
	u16 sex;
	u16 special;
} NEWS_ROOMPLAYER_ONE;
// プレイヤー全データ
typedef struct {
	NEWS_ROOMPLAYER_ONE	player[WFLBY_PLAYER_MAX];
	u16					event;				// プレイヤーイベント
	u16					new_player;			// 新しく入ってきたプレイヤー
} NEWS_ROOMPLAYER;

//-------------------------------------
///	トピックスデータ
//=====================================
// 1トピックデータ
typedef struct _NEWS_TOPIC{
	u8		type;							// データタイプ
	u32		work[NEWS_TOPIC_WK_NUM];		// 汎用ワーク
	STRBUF* p_name[ NEWS_TOPICNAME_NUM ];	// 人の名前用バッファ
	u16		pl_idx[ NEWS_TOPICNAME_NUM ];	// 人の識別インデックス
	u16		pri;							// 優先順位
	s16		dest;							// 破棄カウンタ

	// リスト構造
	struct _NEWS_TOPIC* p_next;
	struct _NEWS_TOPIC* p_last;
} NEWS_TOPIC;
// トピック全データ
typedef struct {
	NEWS_TOPIC		topic_buf[ NEWS_TOPIC_MAX ];// トピック格納バッファ
	NEWS_TOPIC		topic_top;					// トピック先頭データ
} NEWS_TOPICBUF;



//-------------------------------------
///	WiFiロビーニュースデータベース
//=====================================
typedef struct _NEWS_DATA {
	WFLBY_TIME		lock_time;					// LOCKタイム
	BOOL			lock_flag;					// LOCKされているか
	NEWS_ROOMPLAYER	player;						// 接続プレイヤーバッファ
	NEWS_TOPICBUF	topic;						// トピックバッファ

} NEWS_DATA;


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	接続プレイヤー	関連
//=====================================
static void NEWS_ROOMPLAYER_Init( NEWS_ROOMPLAYER* p_wk );
static void NEWS_ROOMPLAYER_SetData( NEWS_ROOMPLAYER* p_wk, u32 player, u32 sex, NEWS_ROOMSP special );
static void NEWS_ROOMPLAYER_SetIn( NEWS_ROOMPLAYER* p_wk, u32 player, u32 sex, NEWS_ROOMSP special );
static void NEWS_ROOMPLAYER_SetOut( NEWS_ROOMPLAYER* p_wk, u32 player );
static void NEWS_ROOMPLAYER_SetSpecial( NEWS_ROOMPLAYER* p_wk, u32 player, NEWS_ROOMSP special );
static void NEWS_ROOMPLAYER_SetEvent( NEWS_ROOMPLAYER* p_wk, NEWS_ROOMEV event );
static void NEWS_ROOMPLAYER_SetNewPlayer( NEWS_ROOMPLAYER* p_wk, u32 player );
static BOOL NEWS_ROOMPLAYER_CheckInside( const NEWS_ROOMPLAYER* cp_wk, u32 player );
static u32 NEWS_ROOMPLAYER_GetSex( const NEWS_ROOMPLAYER* cp_wk, u32 player );
static BOOL NEWS_ROOMPLAYER_GetSpecialMy( const NEWS_ROOMPLAYER* cp_wk, u32 player );
static BOOL NEWS_ROOMPLAYER_GetSpecialOld( const NEWS_ROOMPLAYER* cp_wk, u32 player );
static NEWS_ROOMSP NEWS_ROOMPLAYER_GetSpecial( const NEWS_ROOMPLAYER* cp_wk, u32 player );
static NEWS_ROOMEV NEWS_ROOMPLAYER_GetEvent( const NEWS_ROOMPLAYER* cp_wk );
static BOOL NEWS_ROOMPLAYER_GetNewPlayer( const NEWS_ROOMPLAYER* cp_wk, u32 player );


//-------------------------------------
///	トピックス	関連
//=====================================
static void NEWS_TOPIC_Init( NEWS_TOPICBUF* p_wk, u32 heapID );
static void NEWS_TOPIC_Exit( NEWS_TOPICBUF* p_wk );
static void NEWS_TOPIC_Main( NEWS_TOPICBUF* p_wk );
static void NEWS_TOPIC_RemoveTop( NEWS_TOPICBUF* p_wk );
static NEWS_TOPIC* NEWS_TOPIC_GetCleanWork( NEWS_TOPICBUF* p_wk, u16 pri );
static void NEWS_TOPIC_AddList( NEWS_TOPIC* p_last, NEWS_TOPIC* p_obj );
static void NEWS_TOPIC_Add( NEWS_TOPICBUF* p_wk, NEWS_TOPIC* p_obj );
static void NEWS_TOPIC_Remove( NEWS_TOPICBUF* p_wk, NEWS_TOPIC* p_obj );
static void NEWS_TOPIC_SetData( NEWS_TOPIC* p_obj, u32 data0, u32 data1, u32 data2, const MYSTATUS* cp_p1, const MYSTATUS* cp_p2, const MYSTATUS* cp_p3, const MYSTATUS* cp_p4, u16 idx_p1, u16 idx_p2, u16 idx_p3, u16 idx_p4, u32 dest, u32 pri, u32 type );

static BOOL NEWS_TOPIC_GetSTRConnect( NEWS_TOPIC* p_obj, const WFLBY_VIPFLAG* cp_vip, WORDSET* p_wordset, MSGDATA_MANAGER* p_msgman, STRBUF* p_str, u32 heapID );
static BOOL NEWS_TOPIC_GetSTRItem( NEWS_TOPIC* p_obj, const WFLBY_VIPFLAG* cp_vip, WORDSET* p_wordset, MSGDATA_MANAGER* p_msgman, STRBUF* p_str, u32 heapID );
static BOOL NEWS_TOPIC_GetSTRMiniGame( NEWS_TOPIC* p_obj, const WFLBY_VIPFLAG* cp_vip, WORDSET* p_wordset, MSGDATA_MANAGER* p_msgman, STRBUF* p_str, u32 heapID );
static BOOL NEWS_TOPIC_GetSTRFoot( NEWS_TOPIC* p_obj, const WFLBY_VIPFLAG* cp_vip, WORDSET* p_wordset, MSGDATA_MANAGER* p_msgman, STRBUF* p_str, u32 heapID );
static BOOL NEWS_TOPIC_GetSTRWldTimer( NEWS_TOPIC* p_obj, const WFLBY_VIPFLAG* cp_vip, WORDSET* p_wordset, MSGDATA_MANAGER* p_msgman, STRBUF* p_str, u32 heapID );
static BOOL NEWS_TOPIC_GetSTREvent( NEWS_TOPIC* p_obj, const WFLBY_VIPFLAG* cp_vip, WORDSET* p_wordset, MSGDATA_MANAGER* p_msgman, STRBUF* p_str, u32 heapID );
static BOOL NEWS_TOPIC_GetSTRVipIn( NEWS_TOPIC* p_obj, const WFLBY_VIPFLAG* cp_vip, WORDSET* p_wordset, MSGDATA_MANAGER* p_msgman, STRBUF* p_str, u32 heapID );
static BOOL NEWS_TOPIC_GetSTRMgResultIn( NEWS_TOPIC* p_obj, const WFLBY_VIPFLAG* cp_vip, WORDSET* p_wordset, MSGDATA_MANAGER* p_msgman, STRBUF* p_str, u32 heapID );

static u32 NEWS_TOPIC_GetUserIdxConnect( const NEWS_TOPIC* cp_obj, u32 count );
static u32 NEWS_TOPIC_GetUserIdxItem( const NEWS_TOPIC* cp_obj, u32 count );
static u32 NEWS_TOPIC_GetUserIdxMiniGame( const NEWS_TOPIC* cp_obj, u32 count );
static u32 NEWS_TOPIC_GetUserIdxFoot( const NEWS_TOPIC* cp_obj, u32 count );
static u32 NEWS_TOPIC_GetUserIdxWldTimer( const NEWS_TOPIC* cp_obj, u32 count );
static u32 NEWS_TOPIC_GetUserIdxEvent( const NEWS_TOPIC* cp_obj, u32 count );
static u32 NEWS_TOPIC_GetUserIdxVipIn( const NEWS_TOPIC* cp_obj, u32 count );
static u32 NEWS_TOPIC_GetUserIdxMgResult( const NEWS_TOPIC* cp_obj, u32 count );

static BOOL NEWS_TOPIC_MGRESULT_CheckPlayerNum( WFLBY_GAMETYPE minigame, u32 num );


//----------------------------------------------------------------------------
/**
 *	@brief	ロビーニュース	バッファ作成
 *
 *	@param	heapID	ヒープ
 *
 *	@return	ロビーニュースバッファ
 */
//-----------------------------------------------------------------------------
NEWS_DATA* NEWS_DSET_Init( u32 heapID )
{
	NEWS_DATA* p_data;

	p_data = sys_AllocMemory( heapID, sizeof( NEWS_DATA ) );
	memset( p_data, 0, sizeof(NEWS_DATA) );

	// 接続プレイヤー　初期化
	NEWS_ROOMPLAYER_Init( &p_data->player );

	// トピックバッファ初期化
	NEWS_TOPIC_Init( &p_data->topic, heapID );


	return p_data;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ロビーニュース	バッファ破棄
 *
 *	@param	p_data	ワーク
 */
//-----------------------------------------------------------------------------
void NEWS_DSET_Exit( NEWS_DATA* p_data )
{
	
	// トピックバッファ破棄
	NEWS_TOPIC_Exit( &p_data->topic );
	
	sys_FreeMemoryEz( p_data );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ロビーニュース	メイン処理	（トピックスを時間経過により破棄する処理など）
 *
 *	@param	p_data	ワーク
 */
//-----------------------------------------------------------------------------
void NEWS_DSET_Main( NEWS_DATA* p_data )
{
	// トピックス
	NEWS_TOPIC_Main( &p_data->topic );

	// 設定したイベントを破棄
	// 新しく入ってきた人データ(player.new_player)も破棄したいところだが、
	// lobby_newsで表示するときにずっととっておいたほうが
	// 都合がよいので破棄しない
	NEWS_ROOMPLAYER_SetEvent( &p_data->player, NEWS_ROOMEV_NONE );
	
}


//-------------------------------------
///	データ格納処理
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	LOCK時間の設定
 *
 *	@param	p_data	ワーク
 *	@param	cp_time	時間データ
 */
//-----------------------------------------------------------------------------
void NEWS_DSET_SetLockTime( NEWS_DATA* p_data, const WFLBY_TIME* cp_time )
{
	p_data->lock_time = *cp_time;
	p_data->lock_flag = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	部屋に入ったデータ設定
 *
 *	@param	p_data		ワーク
 *	@param	player		入ったプレイヤーNO
 *	@param	sex			入ったプレイヤーの性別
 *	@param	special_msk	入ったプレイヤーの特殊状態マスク
 */
//-----------------------------------------------------------------------------
void NEWS_DSET_SetRoomIn( NEWS_DATA* p_data, u32 player,  u32 sex, NEWS_ROOMSP special_msk )
{
	NEWS_ROOMPLAYER_SetIn( &p_data->player, player, sex, special_msk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	部屋を出たデータ設定
 *
 *	@param	p_data		ワーク
 *	@param	player		出たプレイヤーNO
 *
 *	@retval	TRUE	一番古い人だった場合
 *	@retval	FALSE	一番古くない人だった場合
 */
//-----------------------------------------------------------------------------
BOOL NEWS_DSET_SetRoomOut( NEWS_DATA* p_data, u32 player )
{
	BOOL result;
	
	result = NEWS_ROOMPLAYER_GetSpecialOld( &p_data->player, player );
	
	NEWS_ROOMPLAYER_SetOut( &p_data->player, player );

	return result;
}

//----------------------------------------------------------------------------
/**
 *	@brief	一番古い人設定を行う
 *
 *	@param	p_data		ワーク
 *	@param	player		プレイヤー
 */	
//-----------------------------------------------------------------------------
void NEWS_DSET_SetRoomOld( NEWS_DATA* p_data, u32 player )
{
	u32 special;
	int i;

	// 今まで一番古いデータだった人がいたらフラグを落とす
	for( i=0; i<WFLBY_PLAYER_MAX; i++ ){
		special = NEWS_ROOMPLAYER_GetSpecial( &p_data->player, i );
		if( special & NEWS_ROOMSP_OLD ){
			special &= ~NEWS_ROOMSP_OLD;
			NEWS_ROOMPLAYER_SetSpecial( &p_data->player, i, special );
		}
	}

	special = NEWS_ROOMPLAYER_GetSpecial( &p_data->player, player );
	NEWS_ROOMPLAYER_SetSpecial( &p_data->player, player, special | NEWS_ROOMSP_OLD );
}

//-----------------------------------------------------------------------------
/**
 *	@brief	トピックの設定
 *
 *	@param	p_data		ワーク
 *	各設定データ
 */
//-----------------------------------------------------------------------------
void NEWS_DSET_SetConnect( NEWS_DATA* p_data,  const NEWS_DATA_SET_CONNECT* cp_data )
{
	NEWS_TOPIC* p_obj;
	p_obj = NEWS_TOPIC_GetCleanWork( &p_data->topic, NEWS_TOPIC_PRI_CONNECT );
	if( p_obj == NULL ){
		return ;
	}
	NEWS_TOPIC_SetData( p_obj, 
			0, 0, 0,
			cp_data->cp_p1, cp_data->cp_p2,
			NULL, NULL,
			cp_data->idx_p1, cp_data->idx_p2,
			0, 0,
			NEWS_TOPIC_DEST_CONNECT, NEWS_TOPIC_PRI_CONNECT, 
			NEWS_TOPICTYPE_CONNECT );
	NEWS_TOPIC_Add( &p_data->topic, p_obj );
}

void NEWS_DSET_SetItem( NEWS_DATA* p_data, const NEWS_DATA_SET_ITEM* cp_data )
{
	NEWS_TOPIC* p_obj;

	// アイテムタイプが不正なら設定しない
	if( cp_data->item >= WFLBY_ITEM_NUM  ){
		return ;
	}
	
	p_obj = NEWS_TOPIC_GetCleanWork( &p_data->topic, NEWS_TOPIC_PRI_ITEM );
	if( p_obj == NULL ){
		return ;
	}
	NEWS_TOPIC_SetData( p_obj, 
			cp_data->item, 0, 0,
			cp_data->cp_p1, cp_data->cp_p2,
			NULL, NULL,
			cp_data->idx_p1, cp_data->idx_p2,
			0, 0,
			NEWS_TOPIC_DEST_ITEM, NEWS_TOPIC_PRI_ITEM, 
			NEWS_TOPICTYPE_ITEM );
	NEWS_TOPIC_Add( &p_data->topic, p_obj );
}

void NEWS_DSET_SetMiniGame( NEWS_DATA* p_data, const NEWS_DATA_SET_MINIGAME* cp_data )
{
	NEWS_TOPIC* p_obj;
	u32 minigame;
	static const sc_NEWS_DSET_MINIGAME_PRI[ WFLBY_GAME_P2PGAME_NUM ] = {
		NEWS_TOPIC_PRI_MINIGAME_00,
		NEWS_TOPIC_PRI_MINIGAME_01,
		NEWS_TOPIC_PRI_MINIGAME_02,
	};

	// ミニゲームタイプが不正なら設定しない
	if( (cp_data->minigame != WFLBY_GAME_BALLSLOW) && 
		(cp_data->minigame != WFLBY_GAME_BALANCEBALL) && 
		(cp_data->minigame != WFLBY_GAME_BALLOON) ){
		return ;
	}

	// 接続人数がPlay=TRUEなら２以上である必要がある
	// ４以下である必要もある
	if( cp_data->play == TRUE ){
		if( (cp_data->num < 2) || (cp_data->num > 4) ){
			return ;
		}
	}else{
		// 接続前なら1以上である必要がある
		if( (cp_data->num < 1) || (cp_data->num > 4) ){
			return ;
		}
	}

	// ミニゲーム内での数字に変換
	minigame = cp_data->minigame - WFLBY_GAME_BALLSLOW;
	
	p_obj = NEWS_TOPIC_GetCleanWork( &p_data->topic, sc_NEWS_DSET_MINIGAME_PRI[minigame] );
	if( p_obj == NULL ){
		return ;
	}
	NEWS_TOPIC_SetData( p_obj, 
			minigame, cp_data->num, cp_data->play,
			cp_data->cp_p1, cp_data->cp_p2,
			cp_data->cp_p3, cp_data->cp_p4,
			cp_data->idx_p1, cp_data->idx_p2,
			cp_data->idx_p3, cp_data->idx_p4,
			NEWS_TOPIC_DEST_GAME, sc_NEWS_DSET_MINIGAME_PRI[minigame], 
			NEWS_TOPICTYPE_MINIGAME );
	NEWS_TOPIC_Add( &p_data->topic, p_obj );
}

void NEWS_DSET_SetFootBoard( NEWS_DATA* p_data, const NEWS_DATA_SET_FOOTBOARD* cp_data )
{
	NEWS_TOPIC* p_obj;

	// 足跡ボードタイプが不正なら設定しない
	if( (cp_data->board != WFLBY_GAME_FOOTWHITE) && 
		(cp_data->board != WFLBY_GAME_FOOTBLACK) ){
		return ;
	}

	p_obj = NEWS_TOPIC_GetCleanWork( &p_data->topic, NEWS_TOPIC_PRI_FOOTBOARD_00+cp_data->board );
	if( p_obj == NULL ){
		return ;
	}
	NEWS_TOPIC_SetData( p_obj, 
			cp_data->board, cp_data->num, 0,
			cp_data->cp_p1, NULL,
			NULL, NULL,
			cp_data->idx_p1, 0,
			0, 0,
			NEWS_TOPIC_DEST_FOOT, NEWS_TOPIC_PRI_FOOTBOARD_00+cp_data->board, 
			NEWS_TOPICTYPE_FOOTBOARD );
	NEWS_TOPIC_Add( &p_data->topic, p_obj );
}

void NEWS_DSET_SetWorldTimer( NEWS_DATA* p_data, const NEWS_DATA_SET_WORLDTIMER* cp_data )
{
	NEWS_TOPIC* p_obj;

	p_obj = NEWS_TOPIC_GetCleanWork( &p_data->topic, NEWS_TOPIC_PRI_WORLDTIMER );
	if( p_obj == NULL ){
		return ;
	}
	NEWS_TOPIC_SetData( p_obj, 
			cp_data->num, 0, 0,
			cp_data->cp_p1, cp_data->cp_p2,
			cp_data->cp_p3, cp_data->cp_p4,
			cp_data->idx_p1, cp_data->idx_p2,
			cp_data->idx_p3, cp_data->idx_p4,
			NEWS_TOPIC_DEST_WLDTIME, NEWS_TOPIC_PRI_WORLDTIMER, 
			NEWS_TOPICTYPE_WORLDTIMER );
	NEWS_TOPIC_Add( &p_data->topic, p_obj );
}

void NEWS_DSET_SetLobbyNews( NEWS_DATA* p_data, const NEWS_DATA_SET_LOBBYNEWS* cp_data )
{
	NEWS_TOPIC* p_obj;

	p_obj = NEWS_TOPIC_GetCleanWork( &p_data->topic, NEWS_TOPIC_PRI_LOBBYNEWS );
	if( p_obj == NULL ){
		return ;
	}
	NEWS_TOPIC_SetData( p_obj, 
			cp_data->num, 0, 0,
			cp_data->cp_p1, cp_data->cp_p2,
			cp_data->cp_p3, cp_data->cp_p4,
			cp_data->idx_p1, cp_data->idx_p2,
			cp_data->idx_p3, cp_data->idx_p4,
			NEWS_TOPIC_DEST_LBYNEWS, NEWS_TOPIC_PRI_LOBBYNEWS, 
			NEWS_TOPICTYPE_LOBBYNEWS );
	NEWS_TOPIC_Add( &p_data->topic, p_obj );
}

void NEWS_DSET_SetTimeEvent( NEWS_DATA* p_data, const NEWS_DATA_SET_TIMEEVENT* cp_data )
{
	NEWS_TOPIC* p_obj;

	// イベントが不正なら設定しない
	switch( cp_data->event_no ){
	
	// 表示するイベント
	case WFLBY_EVENT_NEON:		// ネオン
	case WFLBY_EVENT_FIRE:		// 花火
	case WFLBY_EVENT_PARADE:		// パレード
	case WFLBY_EVENT_CLOSE:		// クローズ
		break;
	
	// 表示しないイベント
	default:
		return;
	}
	
	p_obj = NEWS_TOPIC_GetCleanWork( &p_data->topic, NEWS_TOPIC_PRI_TIMEEVENT );
	if( p_obj == NULL ){
		return ;
	}
	NEWS_TOPIC_SetData( p_obj, 
			cp_data->cp_time->time, cp_data->event_no, 0,
			NULL, NULL,	NULL, NULL,
			0, 0, 0, 0,
			NEWS_TOPIC_DEST_TIEV, NEWS_TOPIC_PRI_TIMEEVENT, 
			NEWS_TOPICTYPE_TIMEEVENT );
	NEWS_TOPIC_Add( &p_data->topic, p_obj );
}

//----------------------------------------------------------------------------
/**
 *	@brief	VIPの入場
 */
//-----------------------------------------------------------------------------
void NEWS_DSET_SetVipIn( NEWS_DATA* p_data, const NEWS_DATA_SET_VIPIN* cp_data, const WFLBY_VIPFLAG* cp_vip )
{
	NEWS_TOPIC* p_obj;
	int i;

	// そのユーザIDXは正しいかチェック
	if( cp_data->vip_plidx >= WFLBY_PLAYER_MAX ){
		return ;
	}

	// その人がVIPなのかチェックする
	if( cp_vip->vip[ cp_data->vip_plidx ] == FALSE ){
		return ;	// VIPじゃないので追加しない
	}

	p_obj = NEWS_TOPIC_GetCleanWork( &p_data->topic, NEWS_TOPIC_PRI_VIPIN );
	if( p_obj == NULL ){
		return ;
	}
	NEWS_TOPIC_SetData( p_obj, 
			cp_data->vip_plidx, 0, 0,
			NULL, NULL,	NULL, NULL,
			0, 0, 0, 0,
			NEWS_TOPIC_DEST_VIPIN, NEWS_TOPIC_PRI_VIPIN, 
			NEWS_TOPICTYPE_VIPIN );
	NEWS_TOPIC_Add( &p_data->topic, p_obj );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ミニゲーム結果
 */
//-----------------------------------------------------------------------------
void NEWS_DSET_SetMgResult( NEWS_DATA* p_data, const NEWS_DATA_SET_MGRESULT* cp_data )
{
	NEWS_TOPIC* p_obj;
	u32 minigame;
	static const u8 sc_NEWS_DSET_MGRESULT_PRI[ 3 ] = {
		NEWS_TOPIC_PRI_MGRESULT00,
		NEWS_TOPIC_PRI_MGRESULT01,
		NEWS_TOPIC_PRI_MGRESULT02,
	};


	// ミニゲームタイプが不正なら設定しない
	if( (cp_data->minigame != WFLBY_GAME_BALLSLOW) && 
		(cp_data->minigame != WFLBY_GAME_BALANCEBALL) && 
		(cp_data->minigame != WFLBY_GAME_BALLOON) ){
		return ;
	}

	// 優勝人数チェック
	if( NEWS_TOPIC_MGRESULT_CheckPlayerNum( cp_data->minigame, cp_data->num ) == FALSE ){
		return ;
	}

	// ミニゲーム内での数字に変換
	minigame = cp_data->minigame - WFLBY_GAME_BALLSLOW;
	
	p_obj = NEWS_TOPIC_GetCleanWork( &p_data->topic, sc_NEWS_DSET_MGRESULT_PRI[minigame] );
	if( p_obj == NULL ){
		return ;
	}
	NEWS_TOPIC_SetData( p_obj, 
			minigame, cp_data->num, 0,
			cp_data->cp_p1, cp_data->cp_p2,
			cp_data->cp_p3, cp_data->cp_p4,
			cp_data->idx_p1, cp_data->idx_p2,
			cp_data->idx_p3, cp_data->idx_p4,
			NEWS_TOPIC_DEST_MGRESULT, sc_NEWS_DSET_MGRESULT_PRI[minigame], 
			NEWS_TOPICTYPE_MGRESULT );
	NEWS_TOPIC_Add( &p_data->topic, p_obj );
}


//----------------------------------------------------------------------------
/**
 *	@brief	先頭トピックを破棄する
 *
 *	@param	p_data	ワーク
 */
//-----------------------------------------------------------------------------
void NEWS_DSET_RemoveTopTopic( NEWS_DATA* p_data )
{
	NEWS_TOPIC_RemoveTop( &p_data->topic );
}


//-------------------------------------
///	データ取得処理
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	LOCK時間を取得する
 *
 *	@param	cp_data		ワーク
 *	@param	p_time		時間格納先
 *
 *	@retval	TRUE	LOCKされている
 *	@retval	FALSE	LOCKされていないから時間が無い
 */
//-----------------------------------------------------------------------------
BOOL NEWS_DSET_GetLockTime( const NEWS_DATA* cp_data, WFLBY_TIME* p_time )
{
	*p_time = cp_data->lock_time;
	return cp_data->lock_flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤーが部屋の中に入っているかチェック
 *
 *	@param	cp_wk	ワーク
 *	@param	player	プレイヤーNO
 *
 *	@retval	TRUE	入っている
 *	@retval	FALSE	入っていない
 */
//-----------------------------------------------------------------------------
BOOL NEWS_DSET_CheckPlayerInside( const NEWS_DATA* cp_wk, u32 player )
{
	return NEWS_ROOMPLAYER_CheckInside( &cp_wk->player, player );
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤーの性別を取得
 *
 *	@param	cp_wk		ワーク
 *	@param	player		プレイヤー
 *
 *	@return	性別
 */
//-----------------------------------------------------------------------------
u32 NEWS_DSET_GetPlayerSex( const NEWS_DATA* cp_wk, u32 player )
{
	return NEWS_ROOMPLAYER_GetSex( &cp_wk->player, player );
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤーが自分かチェック
 *
 *	@param	cp_wk		ワーク
 *	@param	player		プレイヤー
 *
 *	@retval	TRUE	自分
 *	@retval	FALSE	ちがう
 */
//-----------------------------------------------------------------------------
BOOL NEWS_DSET_GetPlayerSpecialMy( const NEWS_DATA* cp_wk, u32 player )
{
	return NEWS_ROOMPLAYER_GetSpecialMy( &cp_wk->player, player );
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤーが一番昔からいるかチェック
 *
 *	@param	cp_wk		ワーク
 *	@param	player		プレイヤー
 *
 *	@retval	TRUE	ふるい
 *	@retval	FALSE	ふるくない
 */
//-----------------------------------------------------------------------------
BOOL NEWS_DSET_GetPlayerSpecialOld( const NEWS_DATA* cp_wk, u32 player )
{
	return NEWS_ROOMPLAYER_GetSpecialOld( &cp_wk->player, player );
}

//----------------------------------------------------------------------------
/**
 *	@brief	その人が新しく入ってきた人かチェック
 *
 *	@param	cp_wk		ワーク
 *	@param	player		プレイヤー
 *
 *	@retval	TRUE	新しく入ってきた人
 *	@retval	FALSE	違う人
 */
//-----------------------------------------------------------------------------
BOOL NEWS_DSET_GetPlayerNewPlayer( const NEWS_DATA* cp_wk, u32 player )
{
	return NEWS_ROOMPLAYER_GetNewPlayer( &cp_wk->player, player );
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤーイベント取得
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	NEWS_ROOMEV_NONE,
 *	@retval	NEWS_ROOMEV_IN,		// 誰か入ってきた
 *	@retval	NEWS_ROOMEV_OUT,	// 誰か出て行った
 */
//-----------------------------------------------------------------------------
NEWS_ROOMEV NEWS_DSET_GetPlayerEvent( const NEWS_DATA* cp_wk )
{
	return NEWS_ROOMPLAYER_GetEvent( &cp_wk->player );
}

//----------------------------------------------------------------------------
/**
 *	@brief	トピックがあるかチェック
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	TRUE	トピックがある
 *	@retval	FALSE	トピックがない
 */
//-----------------------------------------------------------------------------
BOOL NEWS_DSET_CheckToppic( const NEWS_DATA* cp_wk )
{
	if( cp_wk->topic.topic_top.p_next != &cp_wk->topic.topic_top ){
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	先頭のトピックスタイプの取得
 *
 *	@param	cp_wk		ワーク 
 *
 *	@retval	NEWS_TOPICTYPE_CONNECT,			// 挨拶
 *	@retval	NEWS_TOPICTYPE_ITEM,			// ガジェットの交換
 *	@retval	NEWS_TOPICTYPE_MINIGAME,		// ミニゲーム
 *	@retval	NEWS_TOPICTYPE_FOOTBOARD,		// あしあとボード
 *	@retval	NEWS_TOPICTYPE_WORLDTIMER,		// 世界時計
 *	@retval	NEWS_TOPICTYPE_LOBBYNEWS,		// ロビーニュース
 *	@retval	NEWS_TOPICTYPE_TIMEEVENT,		// 時間イベント
 */
//-----------------------------------------------------------------------------
NEWS_TOPICTYPE NEWS_DSET_GetTopicType( const NEWS_DATA* cp_wk )
{
	// データがあるかチェック
	GF_ASSERT( NEWS_DSET_CheckToppic( cp_wk ) );
	
	return cp_wk->topic.topic_top.p_next->type;
}

//----------------------------------------------------------------------------
/**
 *	@brief	先頭データのトピックを文字列に変換して返す
 *
 *	@param	cp_wk		ワーク
 *	@param	cp_vip		VIPフラグ
 *	@param	type		トピックタイプ
 *	@param	p_str		文字列格納先
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	文字列を作れた
 *	@retval	FALSE	文字列をつくれなかった
 */
//-----------------------------------------------------------------------------
BOOL NEWS_DSET_GetTopicData( const NEWS_DATA* cp_wk, const WFLBY_VIPFLAG* cp_vip, NEWS_TOPICTYPE type, STRBUF* p_str, u32 heapID )
{
	WORDSET*			p_wordset;
	MSGDATA_MANAGER*	p_msgman;
	NEWS_TOPIC*			p_obj;
	BOOL				result;
	static BOOL (* const p_func[])( NEWS_TOPIC* p_obj, const WFLBY_VIPFLAG* cp_vip, WORDSET* p_wordset, MSGDATA_MANAGER* p_msgman, STRBUF* p_str, u32 heapID ) = {
		NEWS_TOPIC_GetSTRConnect,
		NEWS_TOPIC_GetSTRItem,
		NEWS_TOPIC_GetSTRMiniGame,
		NEWS_TOPIC_GetSTRFoot,
		NEWS_TOPIC_GetSTRWldTimer,
		NEWS_TOPIC_GetSTRWldTimer,
		NEWS_TOPIC_GetSTREvent,
		NEWS_TOPIC_GetSTRVipIn,
		NEWS_TOPIC_GetSTRMgResultIn,
	};

	GF_ASSERT( NEWS_DSET_CheckToppic( cp_wk ) );	// トピックがあるかチェック
	
	//  先頭トピック取得
	p_obj = cp_wk->topic.topic_top.p_next;

	// メッセージデータ初期化
	p_msgman	= MSGMAN_Create(MSGMAN_TYPE_DIRECT,ARC_MSG,NARC_msg_wflby_news_dat,heapID );
	p_wordset	= WORDSET_Create( heapID );

	// 文字列作成
	if( p_obj->type < NEWS_TOPICTYPE_NUM ){
		result = p_func[ p_obj->type ]( p_obj, cp_vip, p_wordset, p_msgman, p_str, heapID );
	}else{
		result = FALSE;
	}
	
	MSGMAN_Delete( p_msgman );
	WORDSET_Delete( p_wordset );

	return result;
}

//----------------------------------------------------------------------------
/**
 *	@brief	先頭データのトピックを文字列に変換して返す
 *
 *	@param	cp_wk		ワーク
 *	@param	type		トピックタイプ
 *	@param	count		トピックデータ内のトレーナのインデックス
 *
 *	@retval	WFLBY_PLAYER_MAX	そのインデックスにいる人はいないよ
 *	@retval	そのた				そのインデックスのプレイヤーのロービー内インデックス
 */
//-----------------------------------------------------------------------------
u32 NEWS_DSET_GetTopicUserIdx( const NEWS_DATA* cp_wk, NEWS_TOPICTYPE type, u32 count )
{
	NEWS_TOPIC*			p_obj;
	u32 ret;
	static u32 (* const p_func[])( const NEWS_TOPIC* cp_obj, u32 count ) = {
		NEWS_TOPIC_GetUserIdxConnect,
		NEWS_TOPIC_GetUserIdxItem,
		NEWS_TOPIC_GetUserIdxMiniGame,
		NEWS_TOPIC_GetUserIdxFoot,
		NEWS_TOPIC_GetUserIdxWldTimer,
		NEWS_TOPIC_GetUserIdxWldTimer,
		NEWS_TOPIC_GetUserIdxEvent,
		NEWS_TOPIC_GetUserIdxVipIn,
		NEWS_TOPIC_GetUserIdxMgResult,
	};

	GF_ASSERT( NEWS_DSET_CheckToppic( cp_wk ) );	// トピックがあるかチェック
	
	//  先頭トピック取得
	p_obj = cp_wk->topic.topic_top.p_next;

	// 文字列作成
	if( p_obj->type < NEWS_TOPICTYPE_NUM ){
		ret = p_func[ p_obj->type ]( p_obj, count );
	}else{
		GF_ASSERT( 0 );	// トピックタイプエラー
		ret = WFLBY_PLAYER_MAX;
	}

	return ret;
}






//-----------------------------------------------------------------------------
/**
 *		プライベート関数
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	接続プレイヤー	関連
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	初期化処理
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void NEWS_ROOMPLAYER_Init( NEWS_ROOMPLAYER* p_wk )
{
	int i;
	
	for( i=0; i<WFLBY_PLAYER_MAX; i++ ){
		NEWS_ROOMPLAYER_SetData( p_wk, i, PM_NEUTRAL, 0 );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	データ設定
 *
 *	@param	p_wk		ワーク
 *	@param	player		設定プレイヤーNO
 *	@param	sex			性別
 *	@param	special		特殊状態
 */
//-----------------------------------------------------------------------------
static void NEWS_ROOMPLAYER_SetData( NEWS_ROOMPLAYER* p_wk, u32 player, u32 sex, NEWS_ROOMSP special )
{
	GF_ASSERT( player < WFLBY_PLAYER_MAX );
	p_wk->player[ player ].sex		= sex;
	p_wk->player[ player ].special	= special;
}

//----------------------------------------------------------------------------
/**
 *	@brief	入室設定
 *
 *	@param	p_wk		ワーク
 *	@param	player		プレイヤーNO
 *	@param	sex			性別
 *	@param	special		特殊状態
 */
//-----------------------------------------------------------------------------
static void NEWS_ROOMPLAYER_SetIn( NEWS_ROOMPLAYER* p_wk, u32 player, u32 sex, NEWS_ROOMSP special )
{
	NEWS_ROOMPLAYER_SetData( p_wk, player, sex, special );
	NEWS_ROOMPLAYER_SetEvent( p_wk, NEWS_ROOMEV_IN );
	NEWS_ROOMPLAYER_SetNewPlayer( p_wk, player );
}

//----------------------------------------------------------------------------
/**
 *	@brief	退室設定
 *
 *	@param	p_wk	ワーク
 *	@param	player	プレイヤーNO
 */
//-----------------------------------------------------------------------------
static void NEWS_ROOMPLAYER_SetOut( NEWS_ROOMPLAYER* p_wk, u32 player )
{
	NEWS_ROOMPLAYER_SetData( p_wk, player, PM_NEUTRAL, 0 );
	NEWS_ROOMPLAYER_SetEvent( p_wk, NEWS_ROOMEV_OUT );
}

//----------------------------------------------------------------------------
/**
 *	@brief	スペシャルデータだけ設定
 *
 *	@param	p_wk		ワーク
 *	@param	player		プレイヤー
 *	@param	special		スペシャルデータ
 */
//-----------------------------------------------------------------------------
static void NEWS_ROOMPLAYER_SetSpecial( NEWS_ROOMPLAYER* p_wk, u32 player, NEWS_ROOMSP special )
{
	GF_ASSERT( player < WFLBY_PLAYER_MAX );
	p_wk->player[ player ].special	= special;
}

//----------------------------------------------------------------------------
/**
 *	@brief	イベントデータを設定する
 *
 *	@param	p_wk		ワーク
 *	@param	event		イベント
 */
//-----------------------------------------------------------------------------
static void NEWS_ROOMPLAYER_SetEvent( NEWS_ROOMPLAYER* p_wk, NEWS_ROOMEV event )
{
	GF_ASSERT( event <= NEWS_ROOMSP_OLD );
	p_wk->event = event;
}

//----------------------------------------------------------------------------
/**
 *	@brief	新しく入ってきた人を保存する
 *
 *	@param	p_wk		ワーク
 *	@param	player		プレイヤー
 */
//-----------------------------------------------------------------------------
static void NEWS_ROOMPLAYER_SetNewPlayer( NEWS_ROOMPLAYER* p_wk, u32 player )
{
	GF_ASSERT( player < WFLBY_PLAYER_MAX );
	p_wk->new_player = player;
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤーが部屋に入っているかチェック
 *
 *	@param	cp_wk		ワーク
 *	@param	player		プレイヤー
 *
 *	@retval	TRUE	入っている
 *	@retval	FALSE	入っていない
 */
//-----------------------------------------------------------------------------
static BOOL NEWS_ROOMPLAYER_CheckInside( const NEWS_ROOMPLAYER* cp_wk, u32 player )
{
	u32 sex;

	sex = NEWS_ROOMPLAYER_GetSex( cp_wk, player );
	if( sex == PM_NEUTRAL ){
		return FALSE;
	}
	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤーの性別を取得
 *
 *	@param	cp_wk		ワーク
 *	@param	player		プレイヤー
 *
 *	@return	性別
 */
//-----------------------------------------------------------------------------
static u32 NEWS_ROOMPLAYER_GetSex( const NEWS_ROOMPLAYER* cp_wk, u32 player )
{
	GF_ASSERT( player < WFLBY_PLAYER_MAX );
	return cp_wk->player[ player ].sex;
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤーが自分かチェック
 *
 *	@param	cp_wk		ワーク
 *	@param	player		プレイヤー
 *
 *	@retval	TRUE	自分のデータ
 *	@retval	FALSE	他人のデータ
 */
//-----------------------------------------------------------------------------
static BOOL NEWS_ROOMPLAYER_GetSpecialMy( const NEWS_ROOMPLAYER* cp_wk, u32 player )
{
	GF_ASSERT( player < WFLBY_PLAYER_MAX );
	if( cp_wk->player[ player ].special & NEWS_ROOMSP_MYDATA ){
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤーがもっとも古い人かチェック
 *
 *	@param	cp_wk		ワーク
 *	@param	player		プレイヤー
 *
 *	@retval	TRUE		もっとも古い人
 *	@retval	FALSE		もっとも古くない
 */
//-----------------------------------------------------------------------------
static BOOL NEWS_ROOMPLAYER_GetSpecialOld( const NEWS_ROOMPLAYER* cp_wk, u32 player )
{
	GF_ASSERT( player < WFLBY_PLAYER_MAX );
	if( cp_wk->player[ player ].special & NEWS_ROOMSP_OLD ){
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤーの特殊状態マスクを取得する
 *
 *	@param	p_wk		ワーク
 *	@param	player		プレイヤー
 *
 *	@return	マスク
 */
//-----------------------------------------------------------------------------
static NEWS_ROOMSP NEWS_ROOMPLAYER_GetSpecial( const NEWS_ROOMPLAYER* cp_wk, u32 player )
{
	GF_ASSERT( player < WFLBY_PLAYER_MAX );
	return cp_wk->player[ player ].special;
}

//----------------------------------------------------------------------------
/**
 *	@brief	イベントの取得
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	NEWS_ROOMEV_IN,		// 誰か入ってきた
 *	@retval NEWS_ROOMEV_OUT,	// 誰か出て行った
 */
//-----------------------------------------------------------------------------
static NEWS_ROOMEV NEWS_ROOMPLAYER_GetEvent( const NEWS_ROOMPLAYER* cp_wk )
{
	return cp_wk->event;
}

//----------------------------------------------------------------------------
/**
 *	@brief	その人が新しく入ってきた人かチェック
 *
 *	@param	cp_wk		ワーク
 *	@param	player		プレイヤー
 *
 *	@retval	TRUE	新しく入ってきた人
 *	@retval	FALSE	違う人
 */
//-----------------------------------------------------------------------------
static BOOL NEWS_ROOMPLAYER_GetNewPlayer( const NEWS_ROOMPLAYER* cp_wk, u32 player )
{
	GF_ASSERT( player < WFLBY_PLAYER_MAX );
	
	if( cp_wk->new_player == player ){
		return TRUE;
	}
	return FALSE;
}




//-------------------------------------
///	トピックス	関連
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	トピックスシステム	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void NEWS_TOPIC_Init( NEWS_TOPICBUF* p_wk, u32 heapID )
{
	int i, j;

	memset( p_wk, 0, sizeof(NEWS_TOPICBUF) );

	// トピックス格納バッファ初期化
	for( i=0; i<NEWS_TOPIC_MAX; i++ ){
		for( j=0; j<NEWS_TOPICNAME_NUM; j++ ){
			p_wk->topic_buf[ i ].p_name[j] = STRBUF_Create( NEWS_TOPICNAME_LEN, heapID );
		}
	}

	// トピックリスト先頭ダミーデータ
	p_wk->topic_top.p_next = &p_wk->topic_top;
	p_wk->topic_top.p_last = &p_wk->topic_top;
}

//----------------------------------------------------------------------------
/**
 *	@brief	トピックスシステム	破棄処理
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void NEWS_TOPIC_Exit( NEWS_TOPICBUF* p_wk )
{
	int i, j;

	// トピックス格納バッファ破棄
	for( i=0; i<NEWS_TOPIC_MAX; i++ ){
		for( j=0; j<NEWS_TOPICNAME_NUM; j++ ){
			STRBUF_Delete( p_wk->topic_buf[ i ].p_name[j] );
		}
	}

	memset( p_wk, 0, sizeof(NEWS_TOPICBUF) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	トピック	トピック管理処理
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void NEWS_TOPIC_Main( NEWS_TOPICBUF* p_wk )
{
	NEWS_TOPIC* p_next;
	NEWS_TOPIC* p_now;

	// 破棄カウンタチェック
	// 優先順位管理
	p_now = p_wk->topic_top.p_next;
	while( p_now != &p_wk->topic_top ){
		p_next = p_now->p_next;

		// 優先順位設定をなくす
		p_now->pri = NEWS_TOPIC_PRI_NONE;

		// 破棄カウント
		if( (p_now->dest - 1) > 0 ){
			p_now->dest --;
		}else{
			// こいつは破棄
			NEWS_TOPIC_Remove( p_wk, p_now );
		}

		p_now = p_next;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	先頭データを破棄する
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void NEWS_TOPIC_RemoveTop( NEWS_TOPICBUF* p_wk )
{
	NEWS_TOPIC_Remove( p_wk, p_wk->topic_top.p_next );
}

//----------------------------------------------------------------------------
/**
 *	@brief	空いているワークを取得する
 *
 *	@param	p_wk	トピックシステムワーク
 *	@param	pri		優先順位
 *
 *	@return	空いてるワーク
 */
//-----------------------------------------------------------------------------
static NEWS_TOPIC* NEWS_TOPIC_GetCleanWork( NEWS_TOPICBUF* p_wk, u16 pri )
{
	int i;
	NEWS_TOPIC* p_ret;

	p_ret = NULL;
	for( i=0; i<NEWS_TOPIC_MAX; i++ ){
		if( p_wk->topic_buf[i].p_next == NULL ){
			p_ret = &p_wk->topic_buf[i];
			break;
		}
	}

	if( p_ret == NULL ){
		// 優先順位が自分以上なトピックを消して、空いているワークにする
		for( i=0; i<NEWS_TOPIC_MAX; i++ ){
			if( p_wk->topic_buf[i].pri >= pri ){
				// リンクを破棄
				NEWS_TOPIC_Remove( p_wk, &p_wk->topic_buf[i] );
				p_ret = &p_wk->topic_buf[i];
				break;
			}
		}
	}
	return p_ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief	リスト連結をおこなす
 *
 *	@param	p_last		前のデータ
 *	@param	p_obj		自分
 */
//-----------------------------------------------------------------------------
static void NEWS_TOPIC_AddList( NEWS_TOPIC* p_last, NEWS_TOPIC* p_obj )
{
	p_obj->p_next			= p_last->p_next;
	p_obj->p_last			= p_last;
	p_last->p_next			= p_obj;
	p_obj->p_next->p_last	= p_obj;
}

//----------------------------------------------------------------------------
/**
 *	@brief	トピックスをリストに追加する
 *
 *	@param	p_wk		ワーク
 *	@param	p_obj		追加するトピック
 */
//-----------------------------------------------------------------------------
static void NEWS_TOPIC_Add( NEWS_TOPICBUF* p_wk, NEWS_TOPIC* p_obj )
{
	// 優先順位の小さい順番に昇順で追加していく
	// けつからサーチしていく

	// 下から優先順位に従って追加する
	{
		NEWS_TOPIC* p_last;

		p_last = p_wk->topic_top.p_last;
		while( p_last != &p_wk->topic_top ){

			// 優先順位が大きくなったら抜ける
			if( p_last->pri < p_obj->pri ){
				break;
			}

			p_last = p_last->p_last;
		}

		NEWS_TOPIC_AddList( p_last, p_obj );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	トピックスを破棄する
 *
 *	@param	p_wk		ワーク
 *	@param	p_obj		破棄するトピックス
 */
//-----------------------------------------------------------------------------
static void NEWS_TOPIC_Remove( NEWS_TOPICBUF* p_wk, NEWS_TOPIC* p_obj )
{
	p_obj->p_last->p_next = p_obj->p_next;
	p_obj->p_next->p_last = p_obj->p_last;

	//  データの破棄
	p_obj->p_last = NULL;
    p_obj->p_next = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	トピックデータの設定
 *
 *	@param	p_obj		ワーク
 *	@param	data0		ワークデータ１
 *	@param	data1		ワークデータ２
 *	@param	data2		ワークデータ３
 *	@param	cp_p1		プレイヤー名１
 *	@param	cp_p2		プレイヤー名２
 *	@param	cp_p3		プレイヤー名３
 *	@param	cp_p4		プレイヤー名４
 *	@param	idx_p1		プレイヤーインデックス１
 *	@param	idx_p2		プレイヤーインデックス２
 *	@param	idx_p3		プレイヤーインデックス３
 *	@param	idx_p4		プレイヤーインデックス４
 *	@param	dest		破棄カウント値
 *	@param	pri			優先順位
 *	@param	type		タイプ
 */
//-----------------------------------------------------------------------------
static void NEWS_TOPIC_SetData( NEWS_TOPIC* p_obj, u32 data0, u32 data1, u32 data2, const MYSTATUS* cp_p1, const MYSTATUS* cp_p2, const MYSTATUS* cp_p3, const MYSTATUS* cp_p4, u16 idx_p1, u16 idx_p2, u16 idx_p3, u16 idx_p4, u32 dest, u32 pri, u32 type )
{
	GF_ASSERT( type < NEWS_TOPICTYPE_NUM );
	p_obj->type		= type;
	p_obj->dest		= dest;
	p_obj->pri		= pri;
	p_obj->work[0]	= data0;
	p_obj->work[1]	= data1;
	p_obj->work[2]	= data2;
	p_obj->pl_idx[0]= idx_p1;
	p_obj->pl_idx[1]= idx_p2;
	p_obj->pl_idx[2]= idx_p3;
	p_obj->pl_idx[3]= idx_p4;
	if( cp_p1 ){
		MyStatus_CopyNameString( cp_p1, p_obj->p_name[0] );
	}
	if( cp_p2 ){
		MyStatus_CopyNameString( cp_p2, p_obj->p_name[1] );
	}
	if( cp_p3 ){
		MyStatus_CopyNameString( cp_p3, p_obj->p_name[2] );
	}
	if( cp_p4 ){
		MyStatus_CopyNameString( cp_p4, p_obj->p_name[3] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	名前を格納する
 *
 *	@param	p_obj		ワーク
 *	@param	no			格納するバッファナンバー
 *	@param	cp_player	プレイヤーステータス
 */
//-----------------------------------------------------------------------------
static void NEWS_TOPIC_SetName( NEWS_TOPIC* p_obj, u32 no, const MYSTATUS* cp_player )
{
	MyStatus_CopyNameString( cp_player, p_obj->p_name[no] );
}


//----------------------------------------------------------------------------
/**
 *	@brief	接続
 *
 *	@param	p_obj			トピックオブジェ
 *	@param	cp_vip			VIPフラグ
 *	@param	p_wordset		ワードセット
 *	@param	p_msgman		メッセージマネージャ
 *	@param	p_str			文字列格納先
 *	@param	heapID			ヒープＩＤ
 */
//-----------------------------------------------------------------------------
static BOOL NEWS_TOPIC_GetSTRConnect( NEWS_TOPIC* p_obj, const WFLBY_VIPFLAG* cp_vip, WORDSET* p_wordset, MSGDATA_MANAGER* p_msgman, STRBUF* p_str, u32 heapID )
{
	STRBUF*				p_tmpstr;

	WORDSET_RegisterWord( p_wordset, 0,  p_obj->p_name[0], 0, TRUE, CasetteLanguage );	// 名前
	WORDSET_RegisterWord( p_wordset, 1,  p_obj->p_name[1], 0, TRUE, CasetteLanguage );	// 名前

	// 展開する文字列作成
	p_tmpstr = MSGMAN_AllocString( p_msgman, wflby_topic_03 );

	// 展開
	WORDSET_ExpandStr( p_wordset, p_str, p_tmpstr );

	// メッセージデータ破棄
	STRBUF_Delete( p_tmpstr );

	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	アイテム交換
 */
//-----------------------------------------------------------------------------
static BOOL NEWS_TOPIC_GetSTRItem( NEWS_TOPIC* p_obj, const WFLBY_VIPFLAG* cp_vip, WORDSET* p_wordset, MSGDATA_MANAGER* p_msgman, STRBUF* p_str, u32 heapID )
{
	STRBUF*				p_tmpstr;

	WORDSET_RegisterWord( p_wordset, 0,  p_obj->p_name[0], 0, TRUE, CasetteLanguage );	// 名前
	WORDSET_RegisterWord( p_wordset, 1,  p_obj->p_name[1], 0, TRUE, CasetteLanguage );	// 名前
	WORDSET_RegisterWiFiLobbyItemName( p_wordset, 2, p_obj->work[ NEWS_TOPIC_WK_ITEMNO ] );

	// 展開する文字列作成
	p_tmpstr = MSGMAN_AllocString( p_msgman, wflby_topic_04 );

	// 展開
	WORDSET_ExpandStr( p_wordset, p_str, p_tmpstr );

	// メッセージデータ破棄
	STRBUF_Delete( p_tmpstr );


	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ミニゲーム募集
 */
//-----------------------------------------------------------------------------
static BOOL NEWS_TOPIC_GetSTRMiniGame( NEWS_TOPIC* p_obj, const WFLBY_VIPFLAG* cp_vip, WORDSET* p_wordset, MSGDATA_MANAGER* p_msgman, STRBUF* p_str, u32 heapID )
{
	STRBUF*				p_tmpstr;


	// ミニゲーム名
	WORDSET_RegisterWiFiLobbyGameName( p_wordset, 0, p_obj->work[NEWS_TOPIC_WK_GAME_NO] );
	
	// 人数で文字列を変更
	if( p_obj->work[NEWS_TOPIC_WK_GAME_PLAYFLAG] == TRUE ){
		u32 msgidx;

		// 展開する文字列作成
		switch( p_obj->work[ NEWS_TOPIC_WK_GAME_PLAYNUM ] ){
		case 3:
			WORDSET_RegisterWord( p_wordset, 1,  p_obj->p_name[0], 0, TRUE, CasetteLanguage );	// 名前
			WORDSET_RegisterWord( p_wordset, 2,  p_obj->p_name[1], 0, TRUE, CasetteLanguage );	// 名前
			WORDSET_RegisterWord( p_wordset, 3,  p_obj->p_name[2], 0, TRUE, CasetteLanguage );	// 名前
			msgidx = wflby_topic_14;
			break;

		case 2:
			WORDSET_RegisterWord( p_wordset, 1,  p_obj->p_name[0], 0, TRUE, CasetteLanguage );	// 名前
			WORDSET_RegisterWord( p_wordset, 2,  p_obj->p_name[1], 0, TRUE, CasetteLanguage );	// 名前
			msgidx = wflby_topic_15;
			break;

		case 4:
			WORDSET_RegisterWord( p_wordset, 1,  p_obj->p_name[0], 0, TRUE, CasetteLanguage );	// 名前
			WORDSET_RegisterWord( p_wordset, 2,  p_obj->p_name[1], 0, TRUE, CasetteLanguage );	// 名前
			WORDSET_RegisterWord( p_wordset, 3,  p_obj->p_name[2], 0, TRUE, CasetteLanguage );	// 名前
			WORDSET_RegisterWord( p_wordset, 4,  p_obj->p_name[3], 0, TRUE, CasetteLanguage );	// 名前
			msgidx = wflby_topic_02;
			break;
		
		default:
			return FALSE;	// 文字列作成不可能 
		}
		p_tmpstr = MSGMAN_AllocString( p_msgman, msgidx );
	}else{

		WORDSET_RegisterWord( p_wordset, 1,  p_obj->p_name[0], 0, TRUE, CasetteLanguage );	// 名前
		WORDSET_RegisterNumber( p_wordset, 2, WFLBY_MINIGAME_MAX - p_obj->work[ NEWS_TOPIC_WK_GAME_PLAYNUM ],
				1, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT );

		// 展開する文字列作成
		p_tmpstr = MSGMAN_AllocString( p_msgman, wflby_topic_01 );
	}

	// 展開
	WORDSET_ExpandStr( p_wordset, p_str, p_tmpstr );

	// メッセージデータ破棄
	STRBUF_Delete( p_tmpstr );


	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief		足跡ボード
 */
//-----------------------------------------------------------------------------
static BOOL NEWS_TOPIC_GetSTRFoot( NEWS_TOPIC* p_obj, const WFLBY_VIPFLAG* cp_vip, WORDSET* p_wordset, MSGDATA_MANAGER* p_msgman, STRBUF* p_str, u32 heapID )
{
	STRBUF*				p_tmpstr;

	WORDSET_RegisterWiFiLobbyGameName( p_wordset, 0, p_obj->work[NEWS_TOPIC_WK_FOOT_NO] );
	WORDSET_RegisterWord( p_wordset, 1,  p_obj->p_name[0], 0, TRUE, CasetteLanguage );	// 名前
	WORDSET_RegisterNumber( p_wordset, 2, WFLBY_FOOT_MAX - p_obj->work[ NEWS_TOPIC_WK_FOOT_PLAYNUM ],
			1, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT );

	// 展開する文字列作成
	p_tmpstr = MSGMAN_AllocString( p_msgman, wflby_topic_05 );

	// 展開
	WORDSET_ExpandStr( p_wordset, p_str, p_tmpstr );

	// メッセージデータ破棄
	STRBUF_Delete( p_tmpstr );


	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	世界時計
 */
//-----------------------------------------------------------------------------
static BOOL NEWS_TOPIC_GetSTRWldTimer( NEWS_TOPIC* p_obj, const WFLBY_VIPFLAG* cp_vip, WORDSET* p_wordset, MSGDATA_MANAGER* p_msgman, STRBUF* p_str, u32 heapID )
{
	STRBUF*				p_tmpstr;
	int i;
	u32 stridx;

	// 名前設定
	for( i=0; i<p_obj->work[NEWS_TOPIC_WK_WLDT_PLAYNUM]; i++ ){
		WORDSET_RegisterWord( p_wordset, i,  p_obj->p_name[i], 0, TRUE, CasetteLanguage );	// 名前
	}
	if( p_obj->type == NEWS_TOPICTYPE_WORLDTIMER ){
		WORDSET_RegisterWiFiLobbyGameName( p_wordset, 
				p_obj->work[NEWS_TOPIC_WK_WLDT_PLAYNUM], WFLBY_GAME_WLDTIMER );
	}else{
		WORDSET_RegisterWiFiLobbyGameName( p_wordset, 
				p_obj->work[NEWS_TOPIC_WK_WLDT_PLAYNUM], WFLBY_GAME_NEWS );
	}

	// 展開する文字列作成
	p_tmpstr = MSGMAN_AllocString( p_msgman, wflby_topic_06+(WFLBY_MINIGAME_MAX - p_obj->work[NEWS_TOPIC_WK_WLDT_PLAYNUM]) );

	// 展開
	WORDSET_ExpandStr( p_wordset, p_str, p_tmpstr );

	// メッセージデータ破棄
	STRBUF_Delete( p_tmpstr );


	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	イベント設定
 */
//-----------------------------------------------------------------------------
static BOOL NEWS_TOPIC_GetSTREvent( NEWS_TOPIC* p_obj, const WFLBY_VIPFLAG* cp_vip, WORDSET* p_wordset, MSGDATA_MANAGER* p_msgman, STRBUF* p_str, u32 heapID )
{
	STRBUF*				p_tmpstr;
	u32 stridx;

	switch( p_obj->work[NEWS_TOPIC_WK_TIEV_NO] ){
	case WFLBY_EVENT_NEON:
		WORDSET_RegisterWiFiLobbyEventName( p_wordset, 0, WFLBY_EVENT_GMM_NEON );
		stridx = wflby_topic_11;
		break;
	case WFLBY_EVENT_FIRE:
		WORDSET_RegisterWiFiLobbyEventName( p_wordset, 0, WFLBY_EVENT_GMM_FIRE );
		stridx = wflby_topic_16;
		break;
	case WFLBY_EVENT_PARADE:
		WORDSET_RegisterWiFiLobbyEventName( p_wordset, 0, WFLBY_EVENT_GMM_PARADE );
		stridx = wflby_topic_12;
		break;

	case WFLBY_EVENT_CLOSE:		// クローズ
		stridx = wflby_topic_13;
		break;

	default:
		return FALSE;
	}

	// 展開する文字列作成
	p_tmpstr = MSGMAN_AllocString( p_msgman, stridx );

	// 展開
	WORDSET_ExpandStr( p_wordset, p_str, p_tmpstr );

	// メッセージデータ破棄
	STRBUF_Delete( p_tmpstr );


	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	VIPが入ってきた設定
 */
//-----------------------------------------------------------------------------
static BOOL NEWS_TOPIC_GetSTRVipIn( NEWS_TOPIC* p_obj, const WFLBY_VIPFLAG* cp_vip, WORDSET* p_wordset, MSGDATA_MANAGER* p_msgman, STRBUF* p_str, u32 heapID )
{
	// ユーザIDが信用できるかチェック
	if( p_obj->work[NEWS_TOPIC_WK_VIPIN_IDX] >= WFLBY_PLAYER_MAX ){
		return FALSE;
	}
	
	// その人がVIPかどうかチェックする
	if( cp_vip->vip[ p_obj->work[NEWS_TOPIC_WK_VIPIN_IDX] ] == FALSE ){
		return FALSE;
	}
	
	MSGMAN_GetString( p_msgman, wflby_topic_17, p_str );

	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ミニゲーム結果
 */
//-----------------------------------------------------------------------------
static BOOL NEWS_TOPIC_GetSTRMgResultIn( NEWS_TOPIC* p_obj, const WFLBY_VIPFLAG* cp_vip, WORDSET* p_wordset, MSGDATA_MANAGER* p_msgman, STRBUF* p_str, u32 heapID )
{
	STRBUF*				p_tmpstr;
	u32 msgidx;
	int i;
	static const u16 sc_NEWS_TOPIC_BALLOON_MSG[ WFLBY_MINIGAME_MAX ] = {
		0,
		wflby_topic_23,
		wflby_topic_24,
		wflby_topic_19,
	};
	static const u16 sc_NEWS_TOPIC_BALLSLOWBALANCE_MSG[ WFLBY_MINIGAME_MAX ] = {
		wflby_topic_18,
		wflby_topic_20,
		wflby_topic_21,
		wflby_topic_22,
	};

	// 人数がおかしかったらトピック作成不可能 
	if( NEWS_TOPIC_MGRESULT_CheckPlayerNum( p_obj->work[NEWS_TOPIC_WK_MGRESULT_NO], 
				p_obj->work[NEWS_TOPIC_WK_MGRESULT_PLAYNUM] ) == FALSE ){
		return FALSE;
	}

	// ミニゲーム名
	WORDSET_RegisterWiFiLobbyGameName( p_wordset, 0, p_obj->work[NEWS_TOPIC_WK_MGRESULT_NO] );
	
	// 人数で文字列を変更
	// 展開する文字列作成
	for( i=0; i<p_obj->work[ NEWS_TOPIC_WK_MGRESULT_PLAYNUM ]; i++ ){
		WORDSET_RegisterWord( p_wordset, i+1,  p_obj->p_name[i], 0, TRUE, CasetteLanguage );	// 名前設定
	}
	if( (p_obj->work[NEWS_TOPIC_WK_MGRESULT_NO] == WFLBY_GAME_BALLSLOW) ||
		(p_obj->work[NEWS_TOPIC_WK_MGRESULT_NO] == WFLBY_GAME_BALANCEBALL) ){
		p_tmpstr = MSGMAN_AllocString( p_msgman, 
				sc_NEWS_TOPIC_BALLSLOWBALANCE_MSG[ p_obj->work[NEWS_TOPIC_WK_MGRESULT_PLAYNUM]-1] );
	}else{
		p_tmpstr = MSGMAN_AllocString( p_msgman, 
				sc_NEWS_TOPIC_BALLOON_MSG[ p_obj->work[NEWS_TOPIC_WK_MGRESULT_PLAYNUM]-1 ] );
	}

	// 展開
	WORDSET_ExpandStr( p_wordset, p_str, p_tmpstr );

	// メッセージデータ破棄
	STRBUF_Delete( p_tmpstr );

	return TRUE;
}



//----------------------------------------------------------------------------
/**
 *	@brief	それぞれのトピックユーザ識別IDXを取得する
 *
 *	@param	cp_obj		オブジェ
 *	@param	count		カウント
 *
 *	@retval	WFLBY_PLAYER_MAX	そのインデックスにいる人はいないよ
 *	@retval	そのた				そのインデックスのプレイヤーのロービー内インデックス
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	会話
 */
//-----------------------------------------------------------------------------
static u32 NEWS_TOPIC_GetUserIdxConnect( const NEWS_TOPIC* cp_obj, u32 count )
{

	// 2以上はない
	if( count >= 2 ){
		return WFLBY_PLAYER_MAX;
	}
	return cp_obj->pl_idx[count];
}
	
//----------------------------------------------------------------------------
/**
 *	@brief	アイテム交換
 */
//-----------------------------------------------------------------------------
static u32 NEWS_TOPIC_GetUserIdxItem( const NEWS_TOPIC* cp_obj, u32 count )
{
	// 2以上はない
	if( count >= 2 ){
		return WFLBY_PLAYER_MAX;
	}
	return cp_obj->pl_idx[count];
}

//----------------------------------------------------------------------------
/**
 *	@brief	ミニゲーム
 */
//-----------------------------------------------------------------------------
static u32 NEWS_TOPIC_GetUserIdxMiniGame( const NEWS_TOPIC* cp_obj, u32 count )
{
	if( cp_obj->work[ NEWS_TOPIC_WK_GAME_PLAYNUM ] <= count ){
		return WFLBY_PLAYER_MAX;
	}
	return cp_obj->pl_idx[ count ];
}

//----------------------------------------------------------------------------
/**
 *	@brief	足跡ボード
 */
//-----------------------------------------------------------------------------
static u32 NEWS_TOPIC_GetUserIdxFoot( const NEWS_TOPIC* cp_obj, u32 count )
{
	if( count >= 1 ){
		return WFLBY_PLAYER_MAX;
	}
	return cp_obj->pl_idx[count];
}

//----------------------------------------------------------------------------
/**
 *	@brief	世界時計
 */
//-----------------------------------------------------------------------------
static u32 NEWS_TOPIC_GetUserIdxWldTimer( const NEWS_TOPIC* cp_obj, u32 count )
{
	if( cp_obj->work[NEWS_TOPIC_WK_WLDT_PLAYNUM] <= count ){
		return WFLBY_PLAYER_MAX;
	}
	return cp_obj->pl_idx[count];
}

//----------------------------------------------------------------------------
/**
 *	@brief	イベント	
 */
//-----------------------------------------------------------------------------
static u32 NEWS_TOPIC_GetUserIdxEvent( const NEWS_TOPIC* cp_obj, u32 count )
{
	return WFLBY_PLAYER_MAX;
}

//----------------------------------------------------------------------------
/**
 *	@brief	VIP入場
 */
//-----------------------------------------------------------------------------
static u32 NEWS_TOPIC_GetUserIdxVipIn( const NEWS_TOPIC* cp_obj, u32 count )
{
	return WFLBY_PLAYER_MAX;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ミニゲーム結果
 */
//-----------------------------------------------------------------------------
static u32 NEWS_TOPIC_GetUserIdxMgResult( const NEWS_TOPIC* cp_obj, u32 count )
{
	if( cp_obj->work[ NEWS_TOPIC_WK_MGRESULT_PLAYNUM ] <= count ){
		return WFLBY_PLAYER_MAX;
	}
	return cp_obj->pl_idx[ count ];
}



//----------------------------------------------------------------------------
/**
 *	@brief	ミニゲーム結果トピック	人数チェック
 *
 *	@param	minigame		ミニゲームタイプ
 *	@param	num				人数
 *
 *	@retval	TRUE	大丈夫
 *	@retval	FALSE	不正
 */
//-----------------------------------------------------------------------------
static BOOL NEWS_TOPIC_MGRESULT_CheckPlayerNum( WFLBY_GAMETYPE minigame, u32 num )
{
	if( (minigame == WFLBY_GAME_BALLSLOW) || (minigame == WFLBY_GAME_BALANCEBALL) ){
		if( (num < 1) || (num > WFLBY_MINIGAME_MAX) ){
			return FALSE;
		}
	}else{
		if( (num < 2) || (num > WFLBY_MINIGAME_MAX) ){
			return FALSE;
		}
	}
	return TRUE;
}


//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_ev_mg.c
 *	@brief		ミニゲームイベント
 *	@author		tomoya takahashi
 *	@data		2008.01.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"

#include "system/msgdata.h"
#include "system/fontproc.h"

#include "communication/communication.h"

#include "msgdata/msg_wifi_h_info.h"

#include "wflby_ev_mg.h"
#include "wflby_snd.h"

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
#ifdef PM_DEBUG
//#define	WFLBY_EV_MG_DEBUG_MATCHING_STATE	// マッチングステータス表示
#endif

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------


//-------------------------------------
///	みにげーむ　マッチング　タイムアウト
//=====================================
#define WFLBY_EV_MG_MATCHWAIT_TIMEOUT	( 30*30 )


//-------------------------------------
///	メッセージを変更するタイミング
//=====================================
#define WFLBY_EV_MG_MSGCHANGE_TIMING	( 11*30 )

//-------------------------------------
///	ミニゲーム参加シーケンス
//=====================================
enum {
	WFLBY_EV_MG_INIT,
	WFLBY_EV_MG_ENTRY_CHK,		// 参加できるかチェック
	WFLBY_EV_MG_ENTRY_SEL_MSG,	// 参加するか選択
	WFLBY_EV_MG_ENTRY_SEL,		// 参加するか選択
	WFLBY_EV_MG_ENTRY_SELWAIT,	// 参加するか選択待ち
	WFLBY_EV_MG_HELP,			// ヘルプ表示
	WFLBY_EV_MG_MATCHSTART,		// マッチング開始
	WFLBY_EV_MG_MATCHWAIT,		// マッチングウエイト
	WFLBY_EV_MG_MATCHOKWAIT,	// タイムリミットを越えた後にマッチング完了待ち
	WFLBY_EV_MG_INFO_INIT,		// CommInfoの初期化	
	WFLBY_EV_MG_INFO_POKE,		// CommInfoのPokeData送信
	WFLBY_EV_MG_STARTWAIT,		// ゲーム開始待ち
	WFLBY_EV_MG_PLIDX_RESET,	// PLAYER_IDX初期化
	WFLBY_EV_MG_PLIDX_SEND,		// PLAYER_IDX送信
	WFLBY_EV_MG_PLIDX_CHECK,	// PLAYER_IDXチェック
	WFLBY_EV_MG_COMM_START,		// 通信開始
	WFLBY_EV_MG_CANCEL_MSG,		// CANCEL処理チェック
	WFLBY_EV_MG_CANCEL_MSG_WAIT,// CANCEL処理チェック
	WFLBY_EV_MG_CANCEL_CHECK,	// CANCEL処理チェック
	WFLBY_EV_MG_CANCEL_WAIT,	// CANCEL処理チェック
	WFLBY_EV_MG_CANCEL_MW,		// CANCEL処理チェック	MATCHWAITに戻る
	WFLBY_EV_MG_CANCEL_ININ,	// CANCEL処理チェック	INFO_INITに行く
	WFLBY_EV_MG_CANCEL_MOKW,	// CANCEL処理チェック	MATCHOKWAITに行く
	WFLBY_EV_MG_ERREND,			// 通信切断終了
	WFLBY_EV_MG_ERRENDWAIT,		// 通信切断終了待ち
	WFLBY_EV_MG_MSG_WAIT,		// メッセージ終了待ち
	WFLBY_EV_MG_MSG_SYNC,		// メッセージ終了待ち同期処理
	WFLBY_EV_MG_MSG_SYNCWAIT,	// メッセージ終了待ち同期待ち
	WFLBY_EV_MG_END,
} ;

//-------------------------------------
///	ミニゲーム通信切断シーケンス
//=====================================
enum {
	WFLBY_EV_MG_END_REQ,
	WFLBY_EV_MG_END_WAIT,
} ;


#define _TIMING_GAME_CHECK			(13)// つながった直後
#define _TIMING_GAME_CHECK2			(14)// つながった直後
#define _TIMING_GAME_START			(15)// タイミングをそろえる
#define _TIMING_GAME_START2			(18)// タイミングをそろえる
#define _TIMING_POKEDATA_SEND		(17)// タイミングをそろえる

#define WFLBY_EV_MG_SYNC_RETRANS_TIMING	(5*30)	// 通人同期コマンドを再転送するタイミング


//-------------------------------------
///	ミニゲームカンバン描画モード
//=====================================
enum {
	WFLBY_EV_MG_KANBAN_MODE_NORMAL,		// 通常
	WFLBY_EV_MG_KANBAN_MODE_COUNTDOWN,	// カウントダウン機能つき
	WFLBY_EV_MG_KANBAN_MODE_NUM,
} ;
enum{
	WFLBY_EV_MG_KANBAN_DRAW_MSK_TITLE		= 1 << 0,
	WFLBY_EV_MG_KANBAN_DRAW_MSK_PLAYERNUM	= 1 << 1,
	WFLBY_EV_MG_KANBAN_DRAW_MSK_PLAY		= 1 << 2,
	WFLBY_EV_MG_KANBAN_DRAW_MSK_COUNT		= 1 << 3,	
};
#define WFLBY_EV_MG_KANBAN_DRAW_MSK_ALL	( 0xff )




//-------------------------------------
///	プレイヤーミニゲーム掲示板
//=====================================
#define WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_X				( 18 )
#define WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_Y				( 3 )
#define WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_SIZX			( 13 )
#define WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_SIZY			( 8 )
#define WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_COUNT_SIZY	( 10 )

#define WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_TITLE_X	( 0 )
#define WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_TITLE_Y	( 0 )
#define WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_PLAYING_X	( 36 )
#define WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_PLAYING_Y	( 16 )
#define WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_PLAYNUM_X	( 0 )
#define WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_PLAYNUM_Y	( 32 )
#define WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_COUNT_X	( 0 )
#define WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_COUNT_Y	( 64 )


//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------


//-------------------------------------
///	カンバンワーク
//=====================================
typedef struct {
	u8	recruit;
	u8	num;
	u8	play;
	u8	minigame_type;
	s16 number;
	u8	mode;
	u8	write;
} WFLBY_EV_MG_KANBANWK;



//-------------------------------------
///	ミニゲーム参加イベントワーク
//=====================================
typedef struct {
	s16 timeout;
	u16 start_game_set;
	u8  msg_ret_seq;
	s8	msg_wait;
	u8	msg_sync;
	u8	last_entry_num;
	u16 match_entry_num;
	u16 sync_count;	// 同期処理にどのくらいかかっているか

	s32 tmp_userid;

	s32 match_timeout;

	// 10秒前メッセージ変更
	BOOL timelimit_10;
	

	// ビットマップリスト専用
	BMPLIST_HEADER		bmplist;
	u32					bmplist_num;	// ビットマップリスト数

	// カンバンワーク
	WFLBY_EV_MG_KANBANWK	kanban;
} WFLBY_EV_MG_WK;



//-------------------------------------
///	プレイヤー看板イベントワーク
//=====================================
typedef struct {
	WFLBY_EV_MG_KANBANWK data;
} WFLBY_EV_DEF_PLAYER_KANBANWK;


//-----------------------------------------------------------------------------
/**
 *		メッセージリストデータ
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	基本ヘッダー
//=====================================
static const BMPLIST_HEADER sc_WFLBY_EV_MG_HEADER = {
	NULL,
	NULL,
	NULL,
	NULL,

	0,	//リスト項目数
	3,	//表示最大項目数
	0,								//ラベル表示Ｘ座標
	8,								//項目表示Ｘ座標
	0,								//カーソル表示Ｘ座標
	0,								//表示Ｙ座標
	FBMP_COL_BLACK,					//表示文字色
	FBMP_COL_WHITE,					//表示背景色
	FBMP_COL_BLK_SDW,				//表示文字影色
	0,								//文字間隔Ｘ
	16,								//文字間隔Ｙ
	BMPLIST_NO_SKIP,				//ページスキップタイプ
	FONT_SYSTEM,					//文字指定(本来は u8 だけど、そんなに作らないと思うので)
	0,								//ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)

	NULL
};
//-------------------------------------
///	参加選択リスト
//=====================================
enum {
	WFLBY_EV_MG_SELLIST_ENTRY,	// 参加する
	WFLBY_EV_MG_SELLIST_HELP,	// せつめい
	WFLBY_EV_MG_SELLIST_NO,		// 参加しない
	WFLBY_EV_MG_SELLIST_NUM,	// 参加人数
} ;


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
static void WFLBY_EV_MG_MsgPrint( WFLBY_EV_MG_WK* p_wk, WFLBY_EVENTWK* p_eventwk, WFLBY_ROOMWK* p_rmwk, u32 msgidx, u32 retseq );
static void WFLBY_EV_MG_MsgPrintSync( WFLBY_EV_MG_WK* p_wk, WFLBY_EVENTWK* p_eventwk, WFLBY_ROOMWK* p_rmwk, u32 msgidx, u32 retseq, u32 syncno, u32 msgwait );
static void WFLBY_EV_MG_Sync( WFLBY_EV_MG_WK* p_wk, WFLBY_EVENTWK* p_eventwk, u32 retseq, u32 syncno );
static void WFLBY_EV_MG_WK_InitBmpList( WFLBY_EV_MG_WK* p_evwk, WFLBY_ROOMWK* p_rmwk, u32 num, u32 msg_start );
static void WFLBY_EV_MG_WK_ExitBmpList( WFLBY_EV_MG_WK* p_evwk, WFLBY_ROOMWK* p_rmwk );

static BOOL WFLBY_EV_MG_Cancel_MatchWait( WFLBY_EV_MG_WK* p_evwk, WFLBY_ROOMWK* p_rmwk, WFLBY_EV_MG_PARAM* p_param, WFLBY_EVENTWK* p_wk, u32 plno );

// カンバン
static void WFLBY_EV_MG_MINIGAME_PrintInit( WFLBY_EV_MG_KANBANWK* p_evwk, WFLBY_ROOMWK* p_rmwk, u32 minigame_type, u32 kanban_mode,  s32 count );
static void WFLBY_EV_MG_MINIGAME_PrintExit( WFLBY_EV_MG_KANBANWK* p_evwk, WFLBY_ROOMWK* p_rmwk );
static void WFLBY_EV_MG_MINIGAME_PrintSetCount( WFLBY_EV_MG_KANBANWK* p_evwk, s32 count );
static void WFLBY_EV_MG_MINIGAME_PrintMinigame( WFLBY_EV_MG_KANBANWK* p_evwk, WFLBY_ROOMWK* p_rmwk, BOOL force );
static void WFLBY_EV_MG_MINIGAME_PrintMinigame_Recruit( WFLBY_EV_MG_KANBANWK* p_evwk, WFLBY_ROOMWK* p_rmwk, BOOL force );
static void WFLBY_EV_MG_MINIGAME_PrintMinigame_Core( WFLBY_EV_MG_KANBANWK* p_evwk, WFLBY_ROOMWK* p_rmwk, BOOL force, BOOL play_write );

//----------------------------------------------------------------------------
/**
 *	@brief		ミニゲームの開始イベント
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_EV_MG_Start( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno )
{
	WFLBY_EV_MG_WK* p_evwk;
	WFLBY_EV_MG_PARAM* p_param;
	WFLBY_SYSTEM* p_system;
	
	p_param	= WFLBY_EVENTWK_GetParam( p_wk );
	p_evwk	= WFLBY_EVENTWK_GetWk( p_wk );

	p_system = WFLBY_ROOM_GetSystemData( p_rmwk );

//	OS_TPrintf( "mg start seq=%d\n", WFLBY_EVENTWK_GetSeq( p_wk ) );


	// ２重でswitch分のチェックがかかるがエラーチェックを
	// いろんなシーケンスに入れるのがいやなのでここで一気にチェックする
	switch( WFLBY_EVENTWK_GetSeq( p_wk ) ){

	// マッチ完了後
	case WFLBY_EV_MG_INFO_INIT:		// CommInfoの初期化	
	case WFLBY_EV_MG_INFO_POKE:		// CommInfoのPokeData送信
	case WFLBY_EV_MG_STARTWAIT:		// ゲーム開始待ち
	case WFLBY_EV_MG_PLIDX_RESET:	// PLAYER_IDX初期化
	case WFLBY_EV_MG_PLIDX_SEND:		// PLAYER_IDX送信
	case WFLBY_EV_MG_PLIDX_CHECK:	// PLAYER_IDXチェック
	case WFLBY_EV_MG_MSG_SYNC:		// メッセージ終了待ち同期処理
	case WFLBY_EV_MG_MSG_SYNCWAIT:	// メッセージ終了待ち同期待ち
		// 接続人数が減ったらエラーへ
		if( p_evwk->match_entry_num > DWC_LOBBY_MG_GetEntryNum( p_param->mg_type ) ){
			OS_TPrintf( "p_evwk->match_entry_num > DWC_LOBBY_MG_GetEntryNum( p_param->mg_type ) = %d > %d\n", p_evwk->match_entry_num,  DWC_LOBBY_MG_GetEntryNum( p_param->mg_type ) );
			p_param->in_ok = WFLBY_EV_MG_RET_NG_DISCON;
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERREND );
			break;
		}

		// 通信人数よりエントリー人数が多いときはおかしい
		if( p_evwk->match_entry_num > CommGetConnectNum() ){
			OS_TPrintf( "p_evwk->match_entry_num > CommGetConnectNum() = %d > %d\n", p_evwk->match_entry_num, CommGetConnectNum() );
			p_param->in_ok = WFLBY_EV_MG_RET_NG_DISCON;
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERREND );
			break;
		}

		// 接続が切断されたら終了
		if( CommStateWifiP2PGetConnectState() == COMMSTATE_WIFIP2P_CONNECT_NONE ){
			OS_TPrintf( "CommStateWifiP2PGetConnectState() == COMMSTATE_WIFIP2P_CONNECT_NONE\n" );
			p_param->in_ok = WFLBY_EV_MG_RET_NG_DISCON;
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERREND );
			break;
		}

	// タイムリミットを越えた後にマッチング完了待ち
	case WFLBY_EV_MG_MATCHOKWAIT:	
		// 親で・接続人数自分だけになったらオワリ
		if( DWC_LOBBY_MG_MyParent() == TRUE ){
			if( DWC_LOBBY_MG_GetEntryNum( p_param->mg_type ) <= 1 ){
				OS_TPrintf( "DWC_LOBBY_MG_GetEntryNum( p_param->mg_type ) <= 1\n" );
				p_param->in_ok = WFLBY_EV_MG_RET_NG_DISCON;
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERREND );
				break;
			}
		}

		// タイムアウトチェック
		p_evwk->match_timeout --;
		if( p_evwk->match_timeout < 0 ){
			OS_TPrintf( "application timeout\n" );
			p_param->in_ok = WFLBY_EV_MG_RET_NG_DISCON;
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERREND );
			break;
		}



	// マッチ完了後＋マッチ前
	case WFLBY_EV_MG_MATCHWAIT:		// マッチングウエイト
	case WFLBY_EV_MG_CANCEL_CHECK:	// CANCEL処理チェック
	case WFLBY_EV_MG_CANCEL_MSG_WAIT:// CANCEL処理チェック
	case WFLBY_EV_MG_CANCEL_WAIT:	// CANCEL処理チェック
	case WFLBY_EV_MG_CANCEL_MW:		// CANCEL処理チェック	MATCHWAITに戻る
	case WFLBY_EV_MG_CANCEL_ININ:	// CANCEL処理チェック	INFO_INITに行く
	case WFLBY_EV_MG_CANCEL_MOKW:	// CANCEL処理チェック	MATCHOKWAITに行く
		// エラーチェック
		switch( CommWifiIsMatched() ){
		case 2:		// エラーやCANCEL
		case 3:		// タイムアウト
		case 4:		// 切断
		case 5:		// 軽度なえらー
			OS_TPrintf( "err CommWifiIsMatched()==%d\n", CommWifiIsMatched() );
			p_param->in_ok = WFLBY_EV_MG_RET_NG_DISCON;
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERREND );
			break;
		// その他のエラーや接続中などはそのままにしておく
		// その他のエラーはROOMでチェックしてる
		default:
			break;
		}

		// 切断エラーチェック
		
		break;	


	// その他はエラーチェックいらない
	default:
		break;
	}






	
	
	switch( WFLBY_EVENTWK_GetSeq( p_wk ) ){
	// マッチング開始
	case WFLBY_EV_MG_INIT:
		p_evwk = WFLBY_EVENTWK_AllocWk( p_wk, sizeof(WFLBY_EV_MG_WK) );

		// シーケンスを進めておく
		WFLBY_EVENTWK_AddSeq( p_wk );
		break;

	// 参加できるかチェック
	case WFLBY_EV_MG_ENTRY_CHK:		

		// 参加ロックがかかってないか？
		if( WFLBY_SYSTEM_MGFLAG_GetLock( p_system ) ){
			p_param->in_ok = WFLBY_EV_MG_RET_NG_LOCK;
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_END );
			break;
		}
		
		// 消灯してないか？
		if( WFLBY_SYSTEM_Event_GetMiniGameStop( p_system ) ){
			// もうミニゲーム開催してる
			p_param->in_ok = WFLBY_EV_MG_RET_NG_TIME;
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_END );
			break;
		}

		Snd_SePlay( WFLBY_SND_MINIGAME );

		// シーケンスを進める
		WFLBY_EVENTWK_AddSeq( p_wk );
		break;

	// 参加するか選択
	case WFLBY_EV_MG_ENTRY_SEL_MSG:
		// 今のところ参加できる
		// 参加するか質問
		WFLBY_ROOM_MSG_SetMinigame( p_rmwk, p_param->wflby_mg_type, 0 );
		WFLBY_EV_MG_MsgPrint( p_evwk, p_wk, p_rmwk, msg_wifi_h_info_01_01, WFLBY_EV_MG_ENTRY_SEL );
		break;

	// 参加するか選択
	case WFLBY_EV_MG_ENTRY_SEL:		
		// ビットマップリストの表示
		WFLBY_EV_MG_WK_InitBmpList( p_evwk, p_rmwk, WFLBY_EV_MG_SELLIST_NUM, msg_wifi_h_info_01_02 );
		WFLBY_ROOM_LISTWIN_Start( p_rmwk, &p_evwk->bmplist, 0, 0 );

		// 選択待ちへ
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ENTRY_SELWAIT );
		break;
		
	// 参加するか選択待ち
	case WFLBY_EV_MG_ENTRY_SELWAIT:	
		{
			u32 sel;
			BOOL end = FALSE;
			sel = WFLBY_ROOM_LISTWIN_Main( p_rmwk );
			switch( sel ){
			// 切断へ
			case BMPLIST_CANCEL:
			case WFLBY_EV_MG_SELLIST_NO:
				p_param->in_ok = WFLBY_EV_MG_RET_NG_MY;
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_END );
				end = TRUE;
				break;

			// 説明へ
			case WFLBY_EV_MG_SELLIST_HELP:
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_HELP );
				end = TRUE;
				break;

			// 接続へ
			case WFLBY_EV_MG_SELLIST_ENTRY:
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_MATCHSTART );
				end = TRUE;
				break;

			default:
				break;
			}
			
			if( end == TRUE ){
				WFLBY_ROOM_LISTWIN_End( p_rmwk, NULL, NULL );
				WFLBY_EV_MG_WK_ExitBmpList( p_evwk, p_rmwk );
			}
		}
		break;

	// ヘルプ表示
	case WFLBY_EV_MG_HELP:			
		WFLBY_ROOM_MSG_SetMinigame( p_rmwk, p_param->wflby_mg_type, 0 );
		WFLBY_EV_MG_MsgPrint( p_evwk, p_wk, p_rmwk, p_param->wflby_mg_type+msg_wifi_h_info_07_01, WFLBY_EV_MG_ENTRY_SEL_MSG );
		break;

	// マッチング開始
	case WFLBY_EV_MG_MATCHSTART:

		// 念のためもう一度消灯していないかチェックする	
		// 消灯してないか？
		if( WFLBY_SYSTEM_Event_GetMiniGameStop( p_system ) ){
			// もうミニゲーム開催してる
			p_param->in_ok = WFLBY_EV_MG_RET_NG_TIME;
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_END );
			break;
		}

		// 募集中で
		if( DWC_LOBBY_MG_CheckRecruit( p_param->mg_type ) == TRUE ){

			// もう入れないかチェック
			if( (DWC_LOBBY_MG_GetRest( p_param->mg_type ) == 0) ||
				(DWC_LOBBY_MG_CheckEntryOk( p_param->mg_type ) == FALSE)){
				// もうミニゲーム開催してる
				p_param->in_ok = WFLBY_EV_MG_RET_NG_PLAYING;
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_END );
				break;
			}

			// リミットがすでに0じゃないかチェック
			if( DWC_LOBBY_MG_GetTimeLimit( p_param->mg_type ) == 0 ){
				p_param->in_ok = WFLBY_EV_MG_RET_NG_LOCK;
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_END );
				break;
			}
		}

		
		// 今のリミット時間を取得
		p_evwk->timeout = DWC_LOBBY_MG_GetTimeLimit( p_param->mg_type );

		// マッチングタイムアウトの設定
		p_evwk->match_timeout = WFLBY_EV_MG_MATCHWAIT_TIMEOUT;
		
		// 他のメンバーを待っていますとメッセージ
		// 10秒前になっていたら、もうすぐゲーム始まるよ！
		{
			STRBUF* p_str;
			
			if( p_evwk->timeout > WFLBY_EV_MG_MSGCHANGE_TIMING ){
				p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_INFO, msg_wifi_h_info_02_02 );
			}else{
				p_evwk->timelimit_10 = TRUE;
				p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_INFO, msg_wifi_h_info_02_09 );
			}
			WFLBY_ROOM_TALKWIN_PrintStrAllPut( p_rmwk, p_str );
			WFLBY_ROOM_TALKWIN_StartTimeWait( p_rmwk );
		}

		// P2P接続開始
		CommStateWifiP2PStart( p_param->mg_type );

		// 募集開始を教える
		if( DWC_LOBBY_MG_MyParent() == TRUE ){
			// 親なら募集開始を教える
			WFLBY_SYSTEM_TOPIC_SendMiniGame( p_system, p_param->wflby_mg_type, 1, 
					plno, 0, 0, 0, FALSE );
			p_evwk->last_entry_num = 1;
		}

		//	ミニゲームカンバン表示
		WFLBY_EV_MG_MINIGAME_PrintInit( &p_evwk->kanban, p_rmwk, 
				p_param->wflby_mg_type, WFLBY_EV_MG_KANBAN_MODE_COUNTDOWN,
				p_evwk->timeout );
		WFLBY_EV_MG_MINIGAME_PrintMinigame( &p_evwk->kanban, p_rmwk, TRUE );

		// シーケンスを進めておく
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_MATCHWAIT );
		break;	

	// マッチング待ち
	case WFLBY_EV_MG_MATCHWAIT:
		{
			u32 state;
			u32 con_num;

			state = CommStateWifiP2PGetConnectState();
			con_num = DWC_LOBBY_MG_GetEntryNum( p_param->mg_type );

#ifdef WFLBY_EV_MG_DEBUG_MATCHING_STATE
			{
				DWCMatchState match_state;
				match_state = DWC_GetMatchingState();
				OS_TPrintf( "matching state = %d\n", match_state );
			}
#endif

			// タイムアウトカウント
			p_evwk->timeout  = DWC_LOBBY_MG_GetTimeLimit( p_param->mg_type );
			WFLBY_EV_MG_MINIGAME_PrintSetCount( &p_evwk->kanban, p_evwk->timeout );


			// 10秒前メッセージチェンジ
			if( (p_evwk->timeout <= WFLBY_EV_MG_MSGCHANGE_TIMING) && (p_evwk->timelimit_10==FALSE) ){
				STRBUF* p_str;
				
				WFLBY_ROOM_TALKWIN_StopTimeWait( p_rmwk );
				p_evwk->timelimit_10 = TRUE;
				p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_INFO, msg_wifi_h_info_02_09 );
				WFLBY_ROOM_TALKWIN_PrintStrAllPut( p_rmwk, p_str );
				WFLBY_ROOM_TALKWIN_StartTimeWait( p_rmwk );
			}

			// カンバン描画更新
			WFLBY_EV_MG_MINIGAME_PrintMinigame_Recruit( &p_evwk->kanban, p_rmwk, FALSE );


			// マッチング失敗
			if( state == COMMSTATE_WIFIP2P_CONNECT_NONE ){
				p_param->in_ok = WFLBY_EV_MG_RET_NG_DISCON;
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERREND );
				break;

			}

			// マッチング成功
			if( state == COMMSTATE_WIFIP2P_CONNECT_MATCH ){
				
				p_param->in_ok = WFLBY_EV_MG_RET_OK;

				// 人数によってメッセージを変更する
				{
					u32 msgidx;
					STRBUF* p_str;

					if( con_num == WFLBY_MINIGAME_MAX ){
						msgidx = msg_wifi_h_info_02_04;
					}else{
						msgidx = msg_wifi_h_info_03_01;
					}
					
					WFLBY_ROOM_TALKWIN_StopTimeWait( p_rmwk );
					p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_INFO, msgidx );
					WFLBY_ROOM_TALKWIN_PrintStrAllPut( p_rmwk, p_str );
					WFLBY_ROOM_TALKWIN_StartTimeWait( p_rmwk );
					WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_INFO_INIT );

					// 今の接続人数を保存
					p_evwk->match_entry_num	= con_num;
				}

				// カンバン描画更新
				WFLBY_EV_MG_MINIGAME_PrintSetCount( &p_evwk->kanban, 0 );
				WFLBY_EV_MG_MINIGAME_PrintMinigame_Recruit( &p_evwk->kanban, p_rmwk, FALSE );
				break;
			}


			// Bボタンで参加Cancel
			if( p_evwk->timelimit_10 == FALSE ){
				if( sys.trg & PAD_BUTTON_CANCEL ){

					Snd_SePlay( SEQ_SE_DP_SELECT );
					
					if( DWC_LOBBY_MG_MyParent() == FALSE ){
						// 子機用処理
						p_param->in_ok = WFLBY_EV_MG_RET_NG_BCAN;
						// タイムウエイトアイコン破棄
						WFLBY_ROOM_TALKWIN_StopTimeWait( p_rmwk );
						WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERREND );
					}else{
						// 親機はチェックを入れる
						p_param->in_ok = WFLBY_EV_MG_RET_NG_BCAN;
						// タイムウエイトアイコン破棄
						WFLBY_ROOM_TALKWIN_StopTimeWait( p_rmwk );
						WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_CANCEL_MSG );
					}
					OS_TPrintf( "B cancel\n" );
					break;
				}
			}
			

			// 親ならタイムアウトチェック
			// 募集人数変更トピック生成
			if( DWC_LOBBY_MG_MyParent() == TRUE ){
				// 人数の変更をチェック
				if( p_evwk->last_entry_num != con_num ){
					p_evwk->last_entry_num = con_num;
					if( con_num != WFLBY_MINIGAME_MAX ){	// 4人になったらもうださない
						WFLBY_SYSTEM_TOPIC_SendMiniGame( p_system, 
								p_param->wflby_mg_type, con_num, plno,
								0,0,0, FALSE );
					}
				}
				
				if( p_evwk->timeout == 0 ){
					// 人数が１人ならタイムアウト
					if( DWC_LOBBY_MG_GetEntryNum( p_param->mg_type ) <= 1 ){
						
						// タイムアウト
						p_param->in_ok = WFLBY_EV_MG_RET_NG_MIN;
						WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERREND );
					
						OS_TPrintf( "lobby mg timeout \n" );
						break;
					}
				}
			}

			// 募集が存続してあそんでたら、人数に入れなかった
			if( (DWC_LOBBY_MG_CheckRecruit( p_param->mg_type ) == TRUE) &&
				(DWC_LOBBY_MG_CheckEntryOk( p_param->mg_type ) == FALSE) ){
				p_param->in_ok = WFLBY_EV_MG_RET_NG_NUM;
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERREND );
				break;
			}

			// タイムアウトになったら接続完了待ちへ
			if( p_evwk->timeout == 0 ){
				// 時間が来たので進む  マッチングの結果待ちへ
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_MATCHOKWAIT );
				{
					STRBUF* p_str;

					WFLBY_ROOM_TALKWIN_StopTimeWait( p_rmwk );
					p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_INFO, msg_wifi_h_info_03_01 );
					WFLBY_ROOM_TALKWIN_PrintStrAllPut( p_rmwk, p_str );
					WFLBY_ROOM_TALKWIN_StartTimeWait( p_rmwk );
				}
				OS_TPrintf( "match ok wait start\n" );
				break;
			}
		}
		break;

	// タイムリミットを越えた後にマッチング完了待ち
	// ただ単にマッチングが完了するのを待つ
	case WFLBY_EV_MG_MATCHOKWAIT:
		{
			u32 state;
			u32 con_num;

			// カンバン描画更新
			WFLBY_EV_MG_MINIGAME_PrintSetCount( &p_evwk->kanban, 0 );
			WFLBY_EV_MG_MINIGAME_PrintMinigame_Recruit( &p_evwk->kanban, p_rmwk, FALSE );

			state = CommStateWifiP2PGetConnectState();
			con_num = DWC_LOBBY_MG_GetEntryNum( p_param->mg_type );

#ifdef WFLBY_EV_MG_DEBUG_MATCHING_STATE
			{
				DWCMatchState match_state;
				match_state = DWC_GetMatchingState();
				OS_TPrintf( "matching state = %d\n", match_state );
			}
#endif

			// エラーチェック
			switch( CommWifiIsMatched() ){
			case 3:
			case 4:
				p_param->in_ok = WFLBY_EV_MG_RET_NG_DISCON;
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERREND );
				break;
			// その他のエラーや接続中などはそのままにしておく
			// その他のエラーはROOMでチェックしてる
			default:
				break;
			}

			// マッチング失敗
			if( state == COMMSTATE_WIFIP2P_CONNECT_NONE ){
				p_param->in_ok = WFLBY_EV_MG_RET_NG_DISCON;
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERREND );
				break;

			}

			// マッチング成功
			if( state == COMMSTATE_WIFIP2P_CONNECT_MATCH ){
				
				p_param->in_ok = WFLBY_EV_MG_RET_OK;
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_INFO_INIT );

				// 今の接続人数を保存
				p_evwk->match_entry_num	= con_num;
				break;
			}

			// 募集が存続してあそんでたら、人数に入れなかった
			if( (DWC_LOBBY_MG_CheckRecruit( p_param->mg_type ) == TRUE) &&
				(DWC_LOBBY_MG_CheckEntryOk( p_param->mg_type ) == FALSE) ){
				p_param->in_ok = WFLBY_EV_MG_RET_NG_NUM;
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERREND );
				break;
			}
		}
		break;

	// CommInfoの初期化
	case WFLBY_EV_MG_INFO_INIT:	

		// カンバン描画更新
		WFLBY_EV_MG_MINIGAME_PrintSetCount( &p_evwk->kanban, 0 );
		WFLBY_EV_MG_MINIGAME_PrintMinigame_Recruit( &p_evwk->kanban, p_rmwk, FALSE );

		// ステータスを変更する
		WFLBY_SYSTEM_SetMyStatus( WFLBY_ROOM_GetSystemData(p_rmwk), p_param->wflby_mg_status );
		

		OS_TPrintf( "matchok netid%d\n", CommGetCurrentID() );

		// 4人接続モードにする
		CommStateChangeWiFiLobbyMinigame();

		// Info
		CommInfoInitialize( WFLBY_SYSTEM_GetSaveData( WFLBY_ROOM_GetSystemData( p_rmwk ) ), NULL );

		// 広場のMYSTATUSを設定する
		CommInfoSetWiFiPlaceMyStatus( WFLBY_SYSTEM_GetMgMyStatus( p_system ) );

		// 同期なし
		CommSetWifiBothNet(FALSE);

		
		// 同期開始
		WFLBY_EV_MG_Sync( p_evwk, p_wk, WFLBY_EV_MG_INFO_POKE, _TIMING_POKEDATA_SEND );
		break;
	
	// CommInfoのPokeData送信
	case WFLBY_EV_MG_INFO_POKE:

		// カンバン描画更新
		WFLBY_EV_MG_MINIGAME_PrintSetCount( &p_evwk->kanban, 0 );
		WFLBY_EV_MG_MINIGAME_PrintMinigame_Recruit( &p_evwk->kanban, p_rmwk, FALSE );

		// 自分を教える
		CommInfoSendPokeData();

		// 自分はエントリー
		CommInfoSetEntry( CommGetCurrentID() );

		// 通信開始命令待ちへ
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_STARTWAIT );
		break;

	// ゲーム開始待ち
	case WFLBY_EV_MG_STARTWAIT:

		// カンバン描画更新
		WFLBY_EV_MG_MINIGAME_PrintSetCount( &p_evwk->kanban, 0 );
		WFLBY_EV_MG_MINIGAME_PrintMinigame_Recruit( &p_evwk->kanban, p_rmwk, FALSE );


		// 新しく来た人をENTRY状態にする
		{
			int netid;
			while( (netid = CommInfoGetNewNameID()) != INVALID_NETID ){
				CommInfoSetEntry(netid);
				OS_TPrintf( "Entry=%d  EntryNum=%d\n", netid, CommInfoGetEntryNum() );
			}
		}

		/*
        if( CommGetCurrentID() == 0 ){
            // 新規POKEDATAを受信したらみんなに送信
			CommInfoSendArray_ServerSide();	// みんなからもらったPOKEDATAを送信
        }
		//*/
		// 人数がそろったらゲームを開始させる
		if( CommInfoGetEntryNum() >= DWC_LOBBY_MG_GetEntryNum( p_param->mg_type ) ){
			if( DWC_LOBBY_MG_MyParent() == TRUE ){
				if( p_evwk->start_game_set == FALSE ){
					DWC_LOBBY_MG_StartGame();
					p_evwk->start_game_set = TRUE;
				}
			}

			// 親からゲーム開始命令がきたらゲームを開始する
			if( DWC_LOBBY_MG_CheckStartGame() == TRUE ){
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_PLIDX_RESET );
			}
		}else{

			// 人数がそろう前に親からゲーム開始がきたら、自分たちは参加していないので終わる
			if( DWC_LOBBY_MG_CheckStartGame() == TRUE ){
				p_param->in_ok = WFLBY_EV_MG_RET_NG_DISCON;
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERREND );
				OS_TPrintf( "mg not parent commnum_diff\n" );
			}
		}
		break;

	// PLAYER_IDX初期化
	case WFLBY_EV_MG_PLIDX_RESET:	

		// カンバン描画更新
		WFLBY_EV_MG_MINIGAME_PrintSetCount( &p_evwk->kanban, 0 );
		WFLBY_EV_MG_MINIGAME_PrintMinigame_Recruit( &p_evwk->kanban, p_rmwk, FALSE );

		CommToolTempDataReset();
		WFLBY_SYSTEM_PLIDX_Clear( p_system );	// P2P通信相手ロビー内PLIDXバッファクリア
		WFLBY_EV_MG_Sync( p_evwk, p_wk, WFLBY_EV_MG_PLIDX_SEND, _TIMING_GAME_CHECK2 );
		break;

	// PLAYER_IDX送信
	case WFLBY_EV_MG_PLIDX_SEND:
		{
			BOOL result;

			// カンバン描画更新
			WFLBY_EV_MG_MINIGAME_PrintSetCount( &p_evwk->kanban, 0 );
			WFLBY_EV_MG_MINIGAME_PrintMinigame_Recruit( &p_evwk->kanban, p_rmwk, FALSE );


			p_evwk->tmp_userid = WFLBY_SYSTEM_GetMyUserID( p_system );
			result = CommToolSetTempData( CommGetCurrentID(), &p_evwk->tmp_userid );
			if( result == TRUE ){
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_PLIDX_CHECK );
			}
		}
		break;	

	// PLAYER_IDXチェック
	case WFLBY_EV_MG_PLIDX_CHECK:

		// カンバン描画更新
		WFLBY_EV_MG_MINIGAME_PrintSetCount( &p_evwk->kanban, 0 );
		WFLBY_EV_MG_MINIGAME_PrintMinigame_Recruit( &p_evwk->kanban, p_rmwk, FALSE );

		{
			int i;
			int con_num;
			int current_id;
			const s32* cp_data;
			int ok_num;
			BOOL parent_man;
			s32  parent_id;


			// エントリー数を取得
			con_num = CommInfoGetEntryNum();

			current_id = CommGetCurrentID();

			ok_num = 0;

			parent_man  = FALSE;
			parent_id	= DWC_LOBBY_MG_GetParentUserID();

			for( i=0; i<con_num; i++ ){

				if( current_id != i ){

					// PLAYER_IDXが一緒かチェック
					cp_data = CommToolGetTempData(i);	

					// ステータスの受信が完了しているかチェック
					if( cp_data != NULL ){

						// P2P通信している相手のPLIDXを保存する
						WFLBY_SYSTEM_PLIDX_Set( p_system, cp_data[0], i );	// P2P通信相手ロビー内PLIDXバッファクリア
						if( cp_data[0] == parent_id ){
							parent_man = TRUE;
						}
						ok_num ++;
					}
				}else{
					// 自分
					WFLBY_SYSTEM_PLIDX_SetMyData( p_system, i );	// P2P通信相手ロビー内PLIDXバッファクリア
					if( DWC_LOBBY_MG_MyParent() == TRUE ){
						parent_man = TRUE;
					}
					ok_num ++;
				}
			}


			// 全員のステータスを受信してチェック結果がOKなら次の処理へ
			if( ok_num == con_num ){

				if( parent_man == TRUE ){
					// 通信メンバー中にちゃんと親がいるかをチェック
					WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_COMM_START );
				}else{
					// 自分の接続しているメンバーの中に親はいないので、切断	
					p_param->in_ok = WFLBY_EV_MG_RET_NG_DISCON;
					WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERREND );
					OS_TPrintf( "mg not parent\n" );
				}
			}
		}
		break;
		
	// 通信開始
	case WFLBY_EV_MG_COMM_START:

		CommStateSetErrorCheck(FALSE,TRUE);	// 切断はエラーにしない

		// 親ならゲームをしていることを送信
		if( DWC_LOBBY_MG_MyParent() == TRUE ){
			int count;
			WFLBY_MINIGAME_PLIDX plidx;

			// ミニゲームに参加している人を取得
			WFLBY_SYSTEM_PLIDX_Get( p_system, &plidx );
			count = DWC_LOBBY_MG_GetEntryNum( p_param->mg_type );
			
			// 親ならゲーム開始トピック作成
			WFLBY_SYSTEM_TOPIC_SendMiniGame( p_system, p_param->wflby_mg_type,
					count, plidx.plidx[0], plidx.plidx[1], plidx.plidx[2], plidx.plidx[3], TRUE );

			OS_TPrintf( "mgame start topic send count==%d\n", count );
		}

		// ボウケンノート
//		WFLBY_SYSTEM_FNOTE_SetPlayMinigame( p_system, p_param->wflby_mg_type );	// ev_def.cでやってる
				
		// 同期を取ったら終了
		WFLBY_ROOM_TALKWIN_StopTimeWait( p_rmwk );
		WFLBY_EV_MG_Sync( p_evwk, p_wk,
				WFLBY_EV_MG_END, _TIMING_GAME_START2 ); 
		break;

	// CANCEL処理チェック		メッセージ表示
	case WFLBY_EV_MG_CANCEL_MSG:
		// メッセージ表示
		{
			STRBUF* p_str;

			p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_INFO, msg_wifi_h_info_05_07 );
			WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
		}

		// メッセージ待ちへ
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_CANCEL_MSG_WAIT );
		
		// サブウィンドウを破棄
		WFLBY_EV_MG_MINIGAME_PrintExit( &p_evwk->kanban, p_rmwk );
		break;

	// CANCEL処理メッセージウエイト
	case WFLBY_EV_MG_CANCEL_MSG_WAIT:
		{
			BOOL result;
			result = WFLBY_EV_MG_Cancel_MatchWait( p_evwk, p_rmwk, p_param, p_wk, plno );
			if( result == FALSE ){
				if( WFLBY_ROOM_TALKWIN_WaitEnd( p_rmwk ) == TRUE ){
					WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_CANCEL_CHECK );
				}
			}
		}
		break;

	// CANCEL処理チェック		YESNOウィンドウ表示
	case WFLBY_EV_MG_CANCEL_CHECK:
		// YESNOウィンドウON
		WFLBY_ROOM_YESNOWIN_StartNo( p_rmwk );
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_CANCEL_WAIT );
		break;

	// CANCEL処理チェック		YESNO選択待ち
	case WFLBY_EV_MG_CANCEL_WAIT:
		{
			WFLBY_ROOM_YESNO_RET result;
			result = WFLBY_ROOM_YESNOWIN_Main( p_rmwk );
			switch( result ){
			// はい
			case WFLBY_ROOM_YESNO_OK:	
				// YESNOウィンドウを消しす
				WFLBY_ROOM_YESNOWIN_End( p_rmwk );
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERREND );

				// 参加ロック設定
				WFLBY_SYSTEM_MGFLAG_SetLock( p_system );
				break;
				
			// いいえ
			case WFLBY_ROOM_YESNO_NO:
				// YESNOウィンドウを消し,MATCHWAITに戻る
				WFLBY_ROOM_YESNOWIN_End( p_rmwk );
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_CANCEL_MW );
				break;

			// 選び中
			case WFLBY_ROOM_YESNO_WAIT:	
				// マッチング完了を待つ
				{
					BOOL result;
					result = WFLBY_EV_MG_Cancel_MatchWait( p_evwk, p_rmwk, p_param, p_wk, plno );
					if( result == TRUE ){
						WFLBY_ROOM_YESNOWIN_End( p_rmwk );
					}
				}
				break;
			}
		}
		break;

	// CANCEL処理チェック	MATCHWAITに戻る
	case WFLBY_EV_MG_CANCEL_MW:		

		p_evwk->timeout  = DWC_LOBBY_MG_GetTimeLimit( p_param->mg_type );
		WFLBY_EV_MG_MINIGAME_PrintInit( &p_evwk->kanban, p_rmwk, 
				p_param->wflby_mg_type, WFLBY_EV_MG_KANBAN_MODE_COUNTDOWN,
				p_evwk->timeout );
		WFLBY_EV_MG_MINIGAME_PrintMinigame( &p_evwk->kanban, p_rmwk, TRUE );

		// 他のメンバーを待っていますとメッセージ
		{
			STRBUF* p_str;
			if( p_evwk->timeout > WFLBY_EV_MG_MSGCHANGE_TIMING ){
				p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_INFO, msg_wifi_h_info_02_02 );
			}else{
				p_evwk->timelimit_10 = TRUE;
				p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_INFO, msg_wifi_h_info_02_09 );
			}
			WFLBY_ROOM_TALKWIN_PrintStrAllPut( p_rmwk, p_str );
			WFLBY_ROOM_TALKWIN_StartTimeWait( p_rmwk );
		}
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_MATCHWAIT );
		break;
		
	// CANCEL処理チェック	INFO_INITに行く
	case WFLBY_EV_MG_CANCEL_ININ:	
		{
			u32 con_num;

			con_num = DWC_LOBBY_MG_GetEntryNum( p_param->mg_type );

			p_param->in_ok = WFLBY_EV_MG_RET_OK;

			// 人数によってメッセージを変更する
			{
				u32 msgidx;
				STRBUF* p_str;

				if( con_num == WFLBY_MINIGAME_MAX ){
					msgidx = msg_wifi_h_info_02_04;
				}else{
					msgidx = msg_wifi_h_info_03_01;
				}
				
				p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_INFO, msgidx );
				WFLBY_ROOM_TALKWIN_PrintStrAllPut( p_rmwk, p_str );
				WFLBY_ROOM_TALKWIN_StartTimeWait( p_rmwk );
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_INFO_INIT );

			}

			// カンバンを再表示
			WFLBY_EV_MG_MINIGAME_PrintInit( &p_evwk->kanban, p_rmwk, 
					p_param->wflby_mg_type, WFLBY_EV_MG_KANBAN_MODE_COUNTDOWN,
					0 );
			WFLBY_EV_MG_MINIGAME_PrintMinigame( &p_evwk->kanban, p_rmwk, TRUE );
		}
		break;

	// CANCEL処理チェック	MATCHOKWAITに行く
	case WFLBY_EV_MG_CANCEL_MOKW:
		{
			STRBUF* p_str;

			p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_INFO, msg_wifi_h_info_03_01 );
			WFLBY_ROOM_TALKWIN_PrintStrAllPut( p_rmwk, p_str );
			WFLBY_ROOM_TALKWIN_StartTimeWait( p_rmwk );

			// カンバンを再表示
			WFLBY_EV_MG_MINIGAME_PrintInit( &p_evwk->kanban, p_rmwk, 
					p_param->wflby_mg_type, WFLBY_EV_MG_KANBAN_MODE_COUNTDOWN,
					0 );
			WFLBY_EV_MG_MINIGAME_PrintSetCount( &p_evwk->kanban, 0 );
			WFLBY_EV_MG_MINIGAME_PrintMinigame_Recruit( &p_evwk->kanban, p_rmwk, TRUE );
	
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_MATCHOKWAIT );
			OS_TPrintf( "match ok wait start\n" );
		}
		break;
		
	// 通信切断終了
	case WFLBY_EV_MG_ERREND:
		// 通信切断
        CommInfoFinalize();
		CommStateWifiP2PEnd();
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERRENDWAIT );
		break;

	// 通信切断終了待ち
	case WFLBY_EV_MG_ERRENDWAIT:
		{
			u32 status;
			status = CommStateWifiP2PGetConnectState();
			if( status == COMMSTATE_WIFIP2P_CONNECT_NONE ){
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_END );
			}
		}
		break;

	// メッセージ終了待ち
	case WFLBY_EV_MG_MSG_WAIT:		
		if( WFLBY_ROOM_TALKWIN_WaitEnd( p_rmwk ) == TRUE ){
			WFLBY_EVENTWK_SetSeq( p_wk, p_evwk->msg_ret_seq );
		}
		break;

	// メッセージ終了待ち同期処理
	case WFLBY_EV_MG_MSG_SYNC:
		if( p_evwk->msg_wait > 0 ){
			p_evwk->msg_wait--;
		}
		if( p_evwk->msg_wait == 0 ){
			// 同期開始
			CommTimingSyncStart(p_evwk->msg_sync);
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_MSG_SYNCWAIT );
		}
		break;

	// メッセージ終了待ち同期処理
	case WFLBY_EV_MG_MSG_SYNCWAIT:
		// コマンド再転送処理
		p_evwk->sync_count ++;
		if( p_evwk->sync_count >= WFLBY_EV_MG_SYNC_RETRANS_TIMING ){
			OS_TPrintf( "re sync start \n" );
			CommTimingSyncStart(p_evwk->msg_sync);
			p_evwk->sync_count = 0;
		}
		
		// 同期完了待ち
		if(CommIsTimingSync(p_evwk->msg_sync)){
			WFLBY_EVENTWK_SetSeq( p_wk, p_evwk->msg_ret_seq );
		}
		break;

	// 終了
	case WFLBY_EV_MG_END:

		//あとしまつ
		{
			WFLBY_ROOM_TALKWIN_Off( p_rmwk );
			WFLBY_EV_MG_WK_ExitBmpList( p_evwk, p_rmwk );
			WFLBY_EV_MG_MINIGAME_PrintExit( &p_evwk->kanban, p_rmwk );
		}
		
		WFLBY_EVENTWK_DeleteWk( p_wk );
		return TRUE;
	}

	return FALSE;
}



//----------------------------------------------------------------------------
/**
 *	@brief		ミニゲームからの退室処理
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_EV_MG_End( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno )
{
	switch( WFLBY_EVENTWK_GetSeq( p_wk ) ){
	// 切断リクエスト
	case WFLBY_EV_MG_END_REQ:
		// P2P通信を切断する
		CommStateSetErrorCheck(FALSE,FALSE);

		// 通信切断
		if( CommStateIsWifiLoginMatchState() == FALSE ){	// Login状態じゃなかったらやる
			// Info終了
			CommInfoFinalize();
			CommStateWifiP2PEnd();
		}

		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_END_WAIT );
		break;
		
	// 切断待ち
	case WFLBY_EV_MG_END_WAIT:
		if( CommStateIsWifiLoginMatchState() == TRUE ){
			return TRUE;
		}
		break;
	}
	
	
	return FALSE;
}










//----------------------------------------------------------------------------
/**
 *	@brief		ミニゲームカンバンイベント
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_EV_DEF_PlayerA_MINIGAME_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno )
{
	WFLBY_EV_DEF_PLAYER_KANBANWK* p_evwk;

	p_evwk = WFLBY_EVENTWK_GetWk( p_wk );
	


	switch( WFLBY_EVENTWK_GetSeq( p_wk ) ){
	case 0:
		{
			STRBUF* p_str;
			u32 evno;
			u32 msgidx;
			u32 minigame_type;

			p_evwk = WFLBY_EVENTWK_AllocWk( p_wk, sizeof(WFLBY_EV_DEF_PLAYER_KANBANWK) );

			evno = WFLBY_ROOM_GetEvNo( p_rmwk );

			switch( evno ){
			case WFLBY_MAPEVID_EV_KANBAN00:
				msgidx = msg_hiroba_bord_02;
				minigame_type = WFLBY_GAME_BALLSLOW;
				break;
			case WFLBY_MAPEVID_EV_KANBAN01:
				msgidx = msg_hiroba_bord_01;
				minigame_type = WFLBY_GAME_BALANCEBALL;
				break;
			case WFLBY_MAPEVID_EV_KANBAN02:
				msgidx = msg_hiroba_bord_03;
				minigame_type = WFLBY_GAME_BALLOON;
				break;
			default:
				GF_ASSERT(0);
				return TRUE;
			}

			WFLBY_ROOM_MSG_SetMinigame( p_rmwk, minigame_type, 0 );
			p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msgidx );
			WFLBY_ROOM_BOARDWIN_PrintStr( p_rmwk, p_str );

			// 接続状態を表示
			WFLBY_EV_MG_MINIGAME_PrintInit( &p_evwk->data, p_rmwk, minigame_type, WFLBY_EV_MG_KANBAN_MODE_NORMAL, 0 );
			// 
			WFLBY_EV_MG_MINIGAME_PrintMinigame( &p_evwk->data, p_rmwk, TRUE );
			
			WFLBY_EVENTWK_AddSeq( p_wk );
		}
		break;

	case 1:
		if( (sys.trg & PAD_BUTTON_DECIDE) || (sys.trg & PAD_KEY_RIGHT) || (sys.trg & PAD_KEY_LEFT) || (sys.trg & PAD_KEY_DOWN) ){
			WFLBY_EV_MG_MINIGAME_PrintExit( &p_evwk->data, p_rmwk );
			WFLBY_ROOM_TALKWIN_Off( p_rmwk );
			{
				WFLBY_3DOBJCONT* p_objcont;
				WFLBY_3DPERSON* p_person;

				p_objcont	= WFLBY_ROOM_GetObjCont( p_rmwk );
				p_person	= WFLBY_3DOBJCONT_GetPlayer( p_objcont );
				WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_person, WFLBY_3DOBJCONT_MOVEPLAYER );
			}
			WFLBY_EVENTWK_DeleteWk( p_wk );
			return TRUE;
		}else{
			WFLBY_EV_MG_MINIGAME_PrintMinigame( &p_evwk->data, p_rmwk, FALSE );
		}
		break;
	}

	return FALSE;
}



//-----------------------------------------------------------------------------
/**
 *			プライベート関数
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	メッセージ表示処理
 *
 *	@param	p_wk		ワーク
 *	@param	p_eventwk	イベントワーク
 *	@param	p_rmwk		部屋ワーク
 *	@param	msgidx		メッセージIDX
 *	@param	retseq		戻るシーケンス
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_MG_MsgPrint( WFLBY_EV_MG_WK* p_wk, WFLBY_EVENTWK* p_eventwk, WFLBY_ROOMWK* p_rmwk, u32 msgidx, u32 retseq )
{
	STRBUF* p_str;

	p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_INFO, msgidx );
	WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

	// 戻るシーケンス設定
	p_wk->msg_ret_seq = retseq;

	
	WFLBY_EVENTWK_SetSeq( p_eventwk, WFLBY_EV_MG_MSG_WAIT );
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージ表示処理	通信同期
 *
 *	@param	p_wk		ワーク
 *	@param	p_eventwk	イベントワーク
 *	@param	p_rmwk		部屋ワーク
 *	@param	msgidx		メッセージIDX
 *	@param	retseq		戻るシーケンス
 *	@param	syncno		同期ナンバー
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_MG_MsgPrintSync( WFLBY_EV_MG_WK* p_wk, WFLBY_EVENTWK* p_eventwk, WFLBY_ROOMWK* p_rmwk, u32 msgidx, u32 retseq, u32 syncno, u32 msgwait )
{
	STRBUF* p_str;

	p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_INFO, msgidx );
	WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

	// 戻るシーケンス設定
	p_wk->msg_ret_seq = retseq;

	// 同期ナンバー設定
	p_wk->msg_sync = syncno;

	// ウエイト設定
	p_wk->msg_wait = msgwait;

	// シンクロ完了待ちウエイト
	p_wk->sync_count = 0;

	
	WFLBY_EVENTWK_SetSeq( p_eventwk, WFLBY_EV_MG_MSG_SYNC );
}

//----------------------------------------------------------------------------
/**
 *	@brief	通信同期
 *
 *	@param	p_wk			ワーク
 *	@param	p_eventwk		イベントワーク
 *	@param	retseq			次のシーケンス
 *	@param	syncno			同期定数
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_MG_Sync( WFLBY_EV_MG_WK* p_wk, WFLBY_EVENTWK* p_eventwk, u32 retseq, u32 syncno )
{
	// 戻るシーケンス設定
	p_wk->msg_ret_seq = retseq;

	// 同期ナンバー設定
	p_wk->msg_sync = syncno;

	// ウエイト設定
	p_wk->msg_wait = 0;

	// シンクロ完了待ちウエイト
	p_wk->sync_count = 0;

	
	WFLBY_EVENTWK_SetSeq( p_eventwk, WFLBY_EV_MG_MSG_SYNC );
}


//----------------------------------------------------------------------------
/**
 *	@brief	ビットマップリストの作成
 *
 *	@param	p_evwk			イベントワーク
 *	@param	p_rmwk			部屋ワーク
 *	@param	num				リスト数
 *	@param	msg_start		最初の文字列
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_MG_WK_InitBmpList( WFLBY_EV_MG_WK* p_evwk, WFLBY_ROOMWK* p_rmwk, u32 num, u32 msg_start )
{
	int i;
	STRBUF* p_str;

	// バッファ作成
	WFLBY_ROOM_LISTWIN_CreateBmpList( p_rmwk, num );
	p_evwk->bmplist_num = num;

	// データ作成
	for( i=0; i<num; i++ ){
		p_str = WFLBY_ROOM_MSG_Get(p_rmwk, WFLBY_DEFMSG_TYPE_INFO, msg_start+i );
		WFLBY_ROOM_LISTWIN_SetBmpListStr( p_rmwk, p_str, i );
	}

	// ビットマップリストヘッダーに設定
	p_evwk->bmplist			= sc_WFLBY_EV_MG_HEADER;
	p_evwk->bmplist.count	= p_evwk->bmplist_num;
	if( p_evwk->bmplist.line > p_evwk->bmplist_num ){
		p_evwk->bmplist.line = p_evwk->bmplist_num;
	}
	p_evwk->bmplist.list	= WFLBY_ROOM_LISTWIN_GetBmpList( p_rmwk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ビットマップリスト破棄処理
 *
 *	@param	p_evwk	イベントワーク
 *	@param	p_rmwk	部屋ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_MG_WK_ExitBmpList( WFLBY_EV_MG_WK* p_evwk, WFLBY_ROOMWK* p_rmwk )
{
	WFLBY_ROOM_LISTWIN_DeleteBmpList( p_rmwk );
}


//----------------------------------------------------------------------------
/**
 *	@brief	CANCEL中のマッチング完了待ち共通処理
 *
 *	@param	p_evwk		ワーク
 *	@param	p_rmwk		部屋ワーク
 *	@param	p_param		ミニゲームパラメータ
 *	@param	p_wk		イベントワーク
 *
 *	@retval	TRUE	マッチングが完了したか、タイムアウトか
 *	@retval	FALSE	なにもなかった
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_EV_MG_Cancel_MatchWait( WFLBY_EV_MG_WK* p_evwk, WFLBY_ROOMWK* p_rmwk, WFLBY_EV_MG_PARAM* p_param, WFLBY_EVENTWK* p_wk, u32 plno )
{
	u32 state;
	u32 con_num;
	WFLBY_SYSTEM* p_system;

	p_system = WFLBY_ROOM_GetSystemData( p_rmwk );
	state	= CommStateWifiP2PGetConnectState();
	con_num = DWC_LOBBY_MG_GetEntryNum( p_param->mg_type );


	// マッチング失敗
	if( state == COMMSTATE_WIFIP2P_CONNECT_NONE ){
		p_param->in_ok = WFLBY_EV_MG_RET_NG_DISCON;
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERREND );
		return TRUE;
	}

	// マッチング成功
	if( state == COMMSTATE_WIFIP2P_CONNECT_MATCH ){

		// 今の接続人数を保存
		p_evwk->match_entry_num	= con_num;

		// INFO_INITに行く
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_CANCEL_ININ );
		return TRUE;
	}

	// 人数の変更をチェック
	if( p_evwk->last_entry_num != con_num ){
		p_evwk->last_entry_num = con_num;
		if( con_num != WFLBY_MINIGAME_MAX ){	// 4人になったらもうださない
			WFLBY_SYSTEM_TOPIC_SendMiniGame( p_system, 
					p_param->wflby_mg_type, con_num, plno,
					0,0,0, FALSE );
		}
	}
	
	// タイムアウトチェック
	p_evwk->timeout  = DWC_LOBBY_MG_GetTimeLimit( p_param->mg_type );
	if( p_evwk->timeout == 0 ){
		// 人数が１人ならタイムアウト
		if( DWC_LOBBY_MG_GetEntryNum( p_param->mg_type ) <= 1 ){
			
			// タイムアウト
			p_param->in_ok = WFLBY_EV_MG_RET_NG_MIN;
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERREND );
		
			OS_TPrintf( "lobby mg timeout \n" );
			return TRUE;
		}

		// そうでなければ、
		// 時間が来たので進む  マッチングの結果待ちへ
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_CANCEL_MOKW );
		return TRUE;
	}else{

		// １０秒前になったらマッチング待ちへ
		if( p_evwk->timeout <= WFLBY_EV_MG_MSGCHANGE_TIMING ){
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_CANCEL_MW );
			return TRUE;
		}
	}

	return FALSE;
}





//----------------------------------------------------------------------------
/**
 *	@brief	カンバンワークの初期化
 *
 *	@param	p_evwk			ワーク
 *	@param	minigame_type	ミニゲームタイプ
 *	@param	kanban_mode		カンバンモード
 *	@param	count			カウントの値
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_MG_MINIGAME_PrintInit( WFLBY_EV_MG_KANBANWK* p_evwk, WFLBY_ROOMWK* p_rmwk, u32 minigame_type, u32 kanban_mode,  s32 count )
{
	static const u8 sc_WINSIZY[ WFLBY_EV_MG_KANBAN_MODE_NUM ] = {
		WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_SIZY,
		WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_COUNT_SIZY,
	};
	
	GF_ASSERT( kanban_mode < WFLBY_EV_MG_KANBAN_MODE_NUM );

	memset( p_evwk, 0, sizeof(WFLBY_EV_MG_KANBANWK) );

	p_evwk->mode			= kanban_mode;
	p_evwk->minigame_type	= minigame_type;
	WFLBY_EV_MG_MINIGAME_PrintSetCount( p_evwk, count );

	// 接続状態を表示
	WFLBY_ROOM_SUBWIN_Start( p_rmwk, 
			WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_X,
			WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_Y,
			WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_SIZX,
			sc_WINSIZY[kanban_mode] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	カンバンワークの破棄
 *
 *	@param	p_evwk			ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_MG_MINIGAME_PrintExit( WFLBY_EV_MG_KANBANWK* p_evwk, WFLBY_ROOMWK* p_rmwk )
{
	memset( p_evwk, 0, sizeof(WFLBY_EV_MG_KANBANWK) );
	WFLBY_ROOM_SUBWIN_End( p_rmwk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	カンバンで表示するカウンタの値を設定
 *
 *	@param	p_evwk		ワーク
 *	@param	count		カウンタの値	（シンク数単位）
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_MG_MINIGAME_PrintSetCount( WFLBY_EV_MG_KANBANWK* p_evwk, s32 count )
{
	s16 set_num;

	set_num = count / 30;
	// 秒に変更
	if( set_num != p_evwk->number ){
		p_evwk->number = set_num;
		p_evwk->write |= WFLBY_EV_MG_KANBAN_DRAW_MSK_COUNT;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	募集中の表示
 *
 *	@param	p_evwk		看板データワーク
 *	@param	p_rmwk		部屋ワーク
 *	@param	force		絶対に書く
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_MG_MINIGAME_PrintMinigame_Recruit( WFLBY_EV_MG_KANBANWK* p_evwk, WFLBY_ROOMWK* p_rmwk, BOOL force )
{
	WFLBY_EV_MG_MINIGAME_PrintMinigame_Core( p_evwk, p_rmwk, force, FALSE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ミニゲーム描画
 *
 *	@param	p_evwk		カンバンデータワーク
 *	@param	p_rmwk		部屋ワーク
 *	@param	force		絶対に書く
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_MG_MINIGAME_PrintMinigame( WFLBY_EV_MG_KANBANWK* p_evwk, WFLBY_ROOMWK* p_rmwk, BOOL force )
{
	WFLBY_EV_MG_MINIGAME_PrintMinigame_Core( p_evwk, p_rmwk, force, TRUE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ミニゲーム描画
 *
 *	@param	p_evwk		カンバンデータワーク
 *	@param	p_rmwk		部屋ワーク
 *	@param	force		絶対に書く
 *	@param	play_write	遊び中を書き込むか	TRUE：書き込む
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_MG_MINIGAME_PrintMinigame_Core( WFLBY_EV_MG_KANBANWK* p_evwk, WFLBY_ROOMWK* p_rmwk, BOOL force, BOOL play_write )
{
	STRBUF* p_str;
	u32 dwc_minigame;
	BOOL recruit;
	BOOL play;
	u32 num;


	// フラグ立て処理
	if( force ){
		p_evwk->write = WFLBY_EV_MG_KANBAN_DRAW_MSK_ALL;
	}

	// DWCライブラリのミニゲームタイプに変更
	switch( p_evwk->minigame_type ){
	case WFLBY_GAME_BALLSLOW:	// 玉投げ
		dwc_minigame = DWC_LOBBY_MG_BALLSLOW;
		break;
	case WFLBY_GAME_BALANCEBALL:	// 玉乗り
		dwc_minigame = DWC_LOBBY_MG_BALANCEBALL;
		break;
	case WFLBY_GAME_BALLOON:		// ふうせんわり
		dwc_minigame = DWC_LOBBY_MG_BALLOON;
		break;
	
	default:
		GF_ASSERT(0);
		dwc_minigame = DWC_LOBBY_MG_BALLOON;
		break;
	}

	// 遊んでるかチェック
	{
		recruit = DWC_LOBBY_MG_CheckRecruit( dwc_minigame );
		if( p_evwk->recruit != recruit ){
			p_evwk->write |= WFLBY_EV_MG_KANBAN_DRAW_MSK_PLAY;
		}
		p_evwk->recruit = recruit;

		play = DWC_LOBBY_MG_CheckEntryOk( dwc_minigame );
		if( p_evwk->play != play ){
			p_evwk->write |= WFLBY_EV_MG_KANBAN_DRAW_MSK_PLAY;
		}
		p_evwk->play = play;
	}

	// 人数チェック
	{
		if( recruit == TRUE ){
			num = DWC_LOBBY_MG_GetRest( dwc_minigame );
		}else{
			num = WFLBY_MINIGAME_MAX;
		}
		if( p_evwk->num != num ){
			p_evwk->write |= WFLBY_EV_MG_KANBAN_DRAW_MSK_PLAYERNUM;
		}
		p_evwk->num = num;
	}

	


	// 描画処理
	// タイトル
	if( p_evwk->write & WFLBY_EV_MG_KANBAN_DRAW_MSK_TITLE ){
		// 表示クリア
		WFLBY_ROOM_SUBWIN_ClearRect( p_rmwk, 
				WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_TITLE_X, WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_TITLE_Y,
				WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_SIZX*8, 16 );

		// ミニゲーム名描画
		WFLBY_ROOM_MSG_SetMinigame( p_rmwk, p_evwk->minigame_type, 0 );
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_profile_05_06 );
		WFLBY_ROOM_SUBWIN_Print( p_rmwk, p_str, 
				WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_TITLE_X,
				WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_TITLE_Y );
	}

	
	// 遊んでるか
	if( p_evwk->write & WFLBY_EV_MG_KANBAN_DRAW_MSK_PLAY ){
		// 表示クリア
		WFLBY_ROOM_SUBWIN_ClearRect( p_rmwk, 
				WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_PLAYING_X, WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_PLAYING_Y,
				WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_SIZX*8, 16 );

		//  書き込むか？
		if( play_write == TRUE ){

			// 遊んでるときだけ出す
			if( (p_evwk->play == FALSE) && (p_evwk->recruit == TRUE) ){
				p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_bord2_02 );
				WFLBY_ROOM_SUBWIN_Print( p_rmwk, p_str, 
						WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_PLAYING_X,
						WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_PLAYING_Y );
			}
		}
	}

	// 参加人数と残り人数
	if( p_evwk->write & WFLBY_EV_MG_KANBAN_DRAW_MSK_PLAYERNUM ){
		// 表示クリア->後ろのほうだけ
		WFLBY_ROOM_SUBWIN_ClearRect( p_rmwk, 
				WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_PLAYNUM_X, 
				WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_PLAYNUM_Y,
				WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_SIZX*8, 32 );

		// 描画
		WFLBY_ROOM_MSG_SetNumber( p_rmwk, p_evwk->num, 1, 1, NUMBER_DISPTYPE_ZERO );
		WFLBY_ROOM_MSG_SetNumber( p_rmwk, WFLBY_MINIGAME_MAX - p_evwk->num, 1, 0, NUMBER_DISPTYPE_ZERO );
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_bord2_01 );
		WFLBY_ROOM_SUBWIN_Print( p_rmwk, p_str, 
				WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_PLAYNUM_X,
				WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_PLAYNUM_Y );
	}

	// カウント値
	if( p_evwk->mode == WFLBY_EV_MG_KANBAN_MODE_COUNTDOWN ){
		if( p_evwk->write & WFLBY_EV_MG_KANBAN_DRAW_MSK_COUNT ){

			// 表示クリア
			WFLBY_ROOM_SUBWIN_ClearRect( p_rmwk, 
					WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_COUNT_X, 
					WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_COUNT_Y,
					WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_SIZX*8, 16 );

			// 描画
			WFLBY_ROOM_MSG_SetNumber( p_rmwk, p_evwk->number, 2, 0, NUMBER_DISPTYPE_ZERO );
			p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_INFO, msg_wifi_h_info_02_03 );
			WFLBY_ROOM_SUBWIN_Print( p_rmwk, p_str, 
					WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_COUNT_X,
					WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_COUNT_Y );
			
		}
	}

	// 描画フラグクリア
	p_evwk->write = 0;
}


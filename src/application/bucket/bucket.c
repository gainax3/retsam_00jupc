//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		bucket.c
 *	@brief		バケットゲーム
 *	@author		tomoya takahashi
 *	@data		2007.06.19
 *
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"
#include "system/wipe.h"
#include "system/snd_tool.h"
#include "system/arc_util.h"

#include "ballslow_data.naix"

#include "application/wifi_lobby/minigame_tool.h"

#include "bct_common.h"
#include "application/bucket/comm_bct_command.h"
#include "application/bucket/comm_bct_command_func.h"
#include "application/bucket/bct_surver.h"
#include "application/bucket/bct_client.h"
#include "application/bucket/bct_local.h"
#include "application/bucket.h"


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
#define BCT_VRAMTR_MAN_NUM	( 32 )

// メインシーケンス
enum{
	BCT_MAINSEQ_ENTRY_INIT,
	BCT_MAINSEQ_ENTRY_WAIT,
	BCT_MAINSEQ_INIT,
	BCT_MAINSEQ_WIPE_IN,
	BCT_MAINSEQ_WIPE_INWAIT,
	BCT_MAINSEQ_STARTWAIT,
	BCT_MAINSEQ_COUNTDOWN,
	BCT_MAINSEQ_MAIN,
	BCT_MAINSEQ_SCORECOMM,
	BCT_MAINSEQ_SCORECOMMWAIT,
	BCT_MAINSEQ_RESULT,
	BCT_MAINSEQ_WIPE_OUT,
	BCT_MAINSEQ_WIPE_OUTWAIT,
	BCT_MAINSEQ_DELETE,
	BCT_MAINSEQ_RESULT_INIT,
	BCT_MAINSEQ_RESULT_WAIT,
};

// PLNETIDのテーブルに初期値として入っている値
#define BCT_PLNETID_DEF		( 0xffff )

// とりあえず
#ifdef BCT_DEBUG_TIMESELECT
static u32 BCT_TIME_LIMIT = 1500;	// 50
#else
#define BCT_TIME_LIMIT	(30*40)
#endif


#ifdef PM_DEBUG
//#define BCT_DEBUG_ENTRY_CHG	// エントリー画面をいろいろ変える
#endif
#ifdef BCT_DEBUG_ENTRY_CHG
static int DEBUG_ENTRY_COUNT = 0;
#endif

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------



//-------------------------------------
///	バケットゲームワーク
//=====================================
typedef struct _BUCKET_WK{
	MNGM_ENTRYWK*		p_entry;
	MNGM_RESULTWK*		p_result;
	MNGM_ENRES_PARAM	enres_param;
	MNGM_RESULT_PARAM	result_param;
	BCT_SURVER*			p_surver;
	BCT_CLIENT*			p_client;
	BOOL start;
	BOOL end;
	BOOL score_get;	// みんなの得点を受信したか
	u16 netid;
	u16 plno;
	BCT_GAMEDATA		gamedata;	// ゲームデータ
	u32 raregame;		// rareゲーム
	u32	comm_num;

	u32 err_seq;	// 途中中断シーケンス
	 


	u32 check_middle_score_count;
	BOOL all_middle_count_ok;		// 親から子に通信許可がきたか

	u8 middle_score_get_count[ BCT_PLAYER_NUM ];	// みんなの途中得点を受信したか
} ;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
static void BCT_VBlank( void* p_work );
static void BCT_ClientNutsSend( BUCKET_WK* p_wk );
static void BCT_ClientMiddleScoreSend( BUCKET_WK* p_wk );
static void BCT_GAMEDATA_Load( BUCKET_WK* p_wk, u32 heapID );
static void BCT_GAMEDATA_Release( BUCKET_WK* p_wk );

static void BCT_ErrAllSysEnd( BUCKET_WK* p_wk, BUCKET_PROC_WORK* pp );

static BOOL BCT_GAMESendData( BUCKET_WK* p_wk, int command, const void* data, int size );


//----------------------------------------------------------------------------
/**
 *	@brief	バケットゲームプロセス　作成
 *
 *	@param	p_proc		ワーク
 *	@param	p_seq		シーケンス
 *
 *	@retval	PROC_RES_CONTINUE = 0,		///<動作継続中
 *	@retval	PROC_RES_FINISH,			///<動作終了
 */
//-----------------------------------------------------------------------------
PROC_RESULT BucketProc_Init( PROC * p_proc, int * p_seq )
{
	BUCKET_WK* p_wk;
	BUCKET_PROC_WORK* pp = PROC_GetParentWork(p_proc);
	BOOL result;

#ifdef BCT_DEBUG_TIMESELECT
	OS_Printf( "モードをえらんでください x[40] a[45] b[50] y[55]\n" );
	result = FALSE;

	if( sys.cont & PAD_BUTTON_X ){
		BCT_TIME_LIMIT = 30*40;
		result = TRUE;
		OS_Printf( "40秒モード\n" );
	}else if( sys.cont & PAD_BUTTON_A ){
		BCT_TIME_LIMIT = 30*45;
		result = TRUE;
		OS_Printf( "45秒モード\n" );
	}else if( sys.cont & PAD_BUTTON_B ){
		BCT_TIME_LIMIT = 30*50;
		result = TRUE;
		OS_Printf( "50秒モード\n" );
	}else if( sys.cont & PAD_BUTTON_Y ){
		BCT_TIME_LIMIT = 30*55;
		result = TRUE;
		OS_Printf( "55秒モード\n" );
	}
	if( result == FALSE ){
		return PROC_RES_CONTINUE;
	}
#endif

	// ヒープ作成
	sys_CreateHeap( HEAPID_BASE_APP, HEAPID_BUCKET, 0x60000 );

	// ワーク作成
	p_wk = PROC_AllocWork( p_proc, sizeof(BUCKET_WK), HEAPID_BUCKET );
	memset( p_wk, 0, sizeof(BUCKET_WK) );


	// エントリー・結果画面パラメータ作成
	MNGM_ENRES_PARAM_Init( &p_wk->enres_param, pp->wifi_lobby, pp->p_save, pp->vchat, &pp->lobby_wk );

	// ゲーム構成データ読み込み
	BCT_GAMEDATA_Load( p_wk, HEAPID_BUCKET );


	return PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	バケットゲームプロセス　実行
 *
 *	@param	p_proc		ワーク
 *	@param	p_seq		シーケンス
 *
 *	@retval	PROC_RES_CONTINUE = 0,		///<動作継続中
 *	@retval	PROC_RES_FINISH,			///<動作終了
 */
//-----------------------------------------------------------------------------
PROC_RESULT BucketProc_Main( PROC* p_proc, int* p_seq )
{
	BUCKET_WK* p_wk = PROC_GetWork( p_proc );
	BUCKET_PROC_WORK* pp = PROC_GetParentWork(p_proc);
	BOOL result;
	u32 check;

	// 切断エラー処理
	if( (MNGM_ERROR_CheckDisconnect( &p_wk->enres_param ) == TRUE) ){

		switch( p_wk->err_seq ){
		case 0:
			// 真っ黒にする
			if( WIPE_SYS_EndCheck() == TRUE ){
				WIPE_SYS_ExeEnd();
			}
			WIPE_SetBrightnessFadeOut(WIPE_FADE_BLACK);
			p_wk->err_seq ++;
			break;

		case 1:
			// 通信切断
			if( MNGM_ERROR_DisconnectWait( &p_wk->enres_param ) == TRUE ){
				p_wk->err_seq ++;
			}
			break;

		case 2:
			// ワーク破棄
			// 全システム停止＆破棄
			BCT_ErrAllSysEnd( p_wk, pp );
			return PROC_RES_FINISH;

		defalut:
			GF_ASSERT(0);
			return PROC_RES_FINISH;
		}
		return PROC_RES_CONTINUE;
	}

	
	// メインループ処理
	switch( (*p_seq) ){
	case BCT_MAINSEQ_ENTRY_INIT:
#ifdef BCT_DEBUG_ENTRY_CHG
		switch(DEBUG_ENTRY_COUNT){
		case 0:
			TOMOYA_PRINT( "slow\n" );
			p_wk->p_entry = MNGM_ENTRY_InitBallSlow( &p_wk->enres_param, HEAPID_BUCKET );
			break;

		case 1:
			TOMOYA_PRINT( "balance\n" );
			p_wk->p_entry = MNGM_ENTRY_InitBalanceBall( &p_wk->enres_param, HEAPID_BUCKET );
			break;

		case 2:
			TOMOYA_PRINT( "balloon\n" );
			p_wk->p_entry = MNGM_ENTRY_InitBalloon( &p_wk->enres_param, HEAPID_BUCKET );
			break;
		}
#else
		p_wk->p_entry = MNGM_ENTRY_InitBallSlow( &p_wk->enres_param, HEAPID_BUCKET );
#endif
		(*p_seq) ++;
		break;

	case BCT_MAINSEQ_ENTRY_WAIT:
		if( MNGM_ENTRY_Wait( p_wk->p_entry ) == TRUE ){
			p_wk->raregame	= MNGM_ENTRY_GetRareGame( p_wk->p_entry );
			MNGM_ENTRY_Exit( p_wk->p_entry );
			p_wk->p_entry = NULL;
			(*p_seq) ++;
		}
		break;
		
	case BCT_MAINSEQ_INIT:
		if( sys.cont & PAD_BUTTON_A ){
			break;
		}

		// VブランクHブランク関数設定
		sys_VBlankFuncChange( BCT_VBlank, p_wk );	// VBlankセット
		sys_HBlankIntrStop();	//HBlank割り込み停止

		// 各種フラグの初期化
		p_wk->start		= FALSE;
		p_wk->end		= FALSE;
		p_wk->score_get = FALSE;
		memset( p_wk->middle_score_get_count, 0, sizeof(u8)*BCT_PLAYER_NUM );

		// VramTransferManager初期化
		initVramTransferManagerHeap( BCT_VRAMTR_MAN_NUM, HEAPID_BUCKET );

		// 通信開始
		CommCommandBCTInitialize( p_wk );

		// 最初は通信許可
		p_wk->all_middle_count_ok = TRUE;

		// 通信人数を取得
		p_wk->comm_num = CommInfoGetEntryNum();
		GF_ASSERT( p_wk->comm_num > 1 );

		// 通信ID取得
		p_wk->netid = CommGetCurrentID();
		p_wk->plno = MNGM_ENRES_PARAM_GetNetIDtoPlNO( &p_wk->enres_param, p_wk->netid );

		// サーバー初期化
		if( p_wk->netid == COMM_PARENT_ID ){
			p_wk->p_surver = BCT_SURVER_Init( HEAPID_BUCKET, BCT_TIME_LIMIT, p_wk->comm_num, &p_wk->gamedata );

			// 途中経過スコア　カウンタ値を設定
			p_wk->check_middle_score_count = 1;
		}
		p_wk->p_client = BCT_CLIENT_Init( HEAPID_BUCKET, BCT_TIME_LIMIT, p_wk->comm_num, p_wk->plno, &p_wk->gamedata );

		WirelessIconEasy();  // 接続中なのでアイコン表示

		// VChatOn
		if( pp->vchat ){
			// ボイスチャット開始
			mydwc_startvchat( HEAPID_BUCKET );
			TOMOYA_PRINT( "vct on\n" );
		}

		// レアゲームチェックを行う
		{
			BCT_GAME_TYPE_WK gametype;

			TOMOYA_PRINT( "p_wk->raregame = %d\n", p_wk->raregame );

			switch( p_wk->raregame ){
			case MNGM_RAREGAME_BUCKET_NORMAL:
				// そのまま
				gametype.scale_rev	= FALSE;
				gametype.rota_rev	= FALSE;
				break;
			case MNGM_RAREGAME_BUCKET_REVERSE:
				// 逆回転
				gametype.scale_rev	= FALSE;
				gametype.rota_rev	= TRUE;
				break;
			case MNGM_RAREGAME_BUCKET_BIG:
				// スケール反転
				gametype.scale_rev	= TRUE;
				gametype.rota_rev	= FALSE;
				break;

			default:
				GF_ASSERT(0);
				break;
			}

			BCT_CLIENT_GameTypeSet( p_wk->p_client, &gametype );
		}
		

//		Snd_DataSetByScene( SND_SCENE_BCT, SEQ_KINOMI1, 1 );


		CommTimingSyncStart(BCT_SYNCID_GAMESTART);

		TOMOYA_PRINT( "init\n" );
		(*p_seq) ++;
		break;

	case BCT_MAINSEQ_WIPE_IN:

		// 同期が完了するまで待つ
		if(!CommIsTimingSync(BCT_SYNCID_GAMESTART)){
			break;
		}

		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_DOORIN, 
				WIPE_TYPE_DOORIN, WIPE_FADE_OUTCOLOR, 
				WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_BUCKET );
		(*p_seq) ++;
		break;

	case BCT_MAINSEQ_WIPE_INWAIT:
		BCT_CLIENT_StartMain( p_wk->p_client, BCT_CLIENT_STARTEVENT_NONE );	// マルノームをまわしておく
		if( WIPE_SYS_EndCheck() ){
			// 親ならゲームスタートを送信
			if( p_wk->netid == COMM_PARENT_ID ){
				result = CommSendData( CNM_BCT_START, NULL,  0 );
				if( result ){
					(*p_seq) ++;
				}
			}else{
				(*p_seq) ++;
			}
		}
		break;

	case BCT_MAINSEQ_STARTWAIT:
		BCT_CLIENT_StartMain( p_wk->p_client, BCT_CLIENT_STARTEVENT_NONE );	// マルノームをまわしておく
		if( p_wk->start ){
			(*p_seq) ++;
		}
		break;

	case BCT_MAINSEQ_COUNTDOWN:	// カウントダウン処理
		result = BCT_CLIENT_StartMain( p_wk->p_client, BCT_CLIENT_STARTEVENT_COUNT_START );	// カウントダウん
		if( result == FALSE ){
			(*p_seq) ++;
		}
		break;

	case BCT_MAINSEQ_MAIN:
		if( (p_wk->end) ){
			u32 score;
			score = BCT_CLIENT_ScoreGet( p_wk->p_client );
			// 自分のスコアを送信
			result = CommSendData( CNM_BCT_SCORE, &score, sizeof(u32) );

			if( result ){
				(*p_seq) ++;
			}
			break;
		}

		
		if( p_wk->netid == COMM_PARENT_ID ){
			result = BCT_SURVER_Main( p_wk->p_surver );

			// ゲームレベルの変更を送信
			if( BCT_SURVER_CheckGameLevelChange( p_wk->p_surver ) ){
				u32 level;
				BOOL result;
				level = BCT_SURVER_GetGameLevel( p_wk->p_surver );
				result = BCT_GAMESendData( p_wk, CNM_BCT_GAME_LEVEL, &level, sizeof(u32) );

				// 送信できたかチェック
				if( result == TRUE ){
					// 送信したのでフラグOFF
					BCT_SURVER_ClearGameLevelChange( p_wk->p_surver );
				}
			}
			
			// 終了の送信
			if( result == FALSE ){
				BCT_GAMESendData( p_wk, CNM_BCT_END, NULL,  0 );
			}
#ifdef BCT_DEBUG_NOT_TIMECOUNT
			if( sys.trg & PAD_BUTTON_CANCEL ){	
				BCT_GAMESendData( p_wk, CNM_BCT_END, NULL,  0 );
			}
#endif	// BCT_DEBUG_NOT_TIMECOUNT
		}
		BCT_CLIENT_Main( p_wk->p_client );

		// 木の実データ送信
		BCT_ClientNutsSend( p_wk );

		// 途中経過の送信チェック
		BCT_ClientMiddleScoreSend( p_wk );
		break;

	case BCT_MAINSEQ_SCORECOMM:
		BCT_CLIENT_EndMain( p_wk->p_client, BCT_CLIENT_STARTEVENT_NONE );	
		if( p_wk->netid == COMM_PARENT_ID ){
			// 親はみんなから得点を収集してみんなに送る
			result = BCT_SURVER_ScoreAllUserGetCheck( p_wk->p_surver );
			if( result == TRUE ){
				BCT_SCORE_COMM score;
				BCT_SURVER_ScoreGet( p_wk->p_surver, &score );
				result = CommSendData( CNM_BCT_ALLSCORE, &score, sizeof(BCT_SCORE_COMM) );
				if( result ){
					(*p_seq) ++;
				}
			}
		}else{
			// クライアントはすぐに結果受信町へ
			(*p_seq) ++;
		}
		break;

	case BCT_MAINSEQ_SCORECOMMWAIT:
		BCT_CLIENT_EndMain( p_wk->p_client, BCT_CLIENT_STARTEVENT_NONE );	
		if( p_wk->score_get == TRUE ){
			(*p_seq) ++;
		}
		break;
		
	case BCT_MAINSEQ_RESULT:
		// 結果発表 
		result = BCT_CLIENT_EndMain( p_wk->p_client, BCT_CLIENT_ENDEVENT_RESULTON );	
		if( result == FALSE ){
			(*p_seq)++;
		}
		break;

	case BCT_MAINSEQ_WIPE_OUT:
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_DOOROUT, 
				WIPE_TYPE_DOOROUT, WIPE_FADE_OUTCOLOR, 
				WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_BUCKET );
		BCT_CLIENT_EndMain( p_wk->p_client, BCT_CLIENT_ENDEVENT_RESULTON );	
		(*p_seq) ++;
		break;

	case BCT_MAINSEQ_WIPE_OUTWAIT:
		BCT_CLIENT_EndMain( p_wk->p_client, BCT_CLIENT_ENDEVENT_RESULTON );	
		if( WIPE_SYS_EndCheck() ){

			// 終了同期開始
			CommTimingSyncStart(BCT_SYNCID_END);
			(*p_seq) ++;
		}
		break;

	case BCT_MAINSEQ_DELETE:
		// 同期が完了するまで待つ
		if(!CommIsTimingSync(BCT_SYNCID_END)){
			TOMOYA_PRINT( "sync_wait\n" );
			return PROC_RES_CONTINUE;
		}

		// VChatOff
		if( pp->vchat ){
			// ボイスチャット終了
			mydwc_stopvchat();
			TOMOYA_PRINT( "vct off\n" );
		}

		if( p_wk->netid == COMM_PARENT_ID ){
			BCT_SURVER_Delete( p_wk->p_surver );
			p_wk->p_surver = NULL;
		}
		BCT_CLIENT_Delete( p_wk->p_client );
		p_wk->p_client = NULL;

		sys_VBlankFuncChange( NULL, NULL );	// VBlankセット
		sys_HBlankIntrStop();	//HBlank割り込み停止

		//
		DellVramTransferManager();

		(*p_seq)++;
		break;


	case BCT_MAINSEQ_RESULT_INIT:
		{
			u32 rank;

			// 順位計算
			MNGM_RESULT_CalcRank( &p_wk->result_param, p_wk->enres_param.num );

			// 順位からガジェットアップチェック
			rank = p_wk->result_param.result[ p_wk->plno ];
			
		}
#ifdef BCT_DEBUG_ENTRY_CHG
		switch(DEBUG_ENTRY_COUNT){
		case 0:
			TOMOYA_PRINT( "slow\n" );
			p_wk->p_result = MNGM_RESULT_InitBallSlow( &p_wk->enres_param, &p_wk->result_param, HEAPID_BUCKET );
			break;

		case 1:
			TOMOYA_PRINT( "balance\n" );
			p_wk->p_result = MNGM_RESULT_InitBalanceBall( &p_wk->enres_param, &p_wk->result_param, HEAPID_BUCKET );
			break;

		case 2:
			TOMOYA_PRINT( "balloon\n" );
			p_wk->result_param.balloon = 14;
			p_wk->p_result = MNGM_RESULT_InitBalloon( &p_wk->enres_param, &p_wk->result_param, HEAPID_BUCKET );
			break;
		}

		DEBUG_ENTRY_COUNT  = (DEBUG_ENTRY_COUNT + 1) % 3;
#else
		p_wk->p_result = MNGM_RESULT_InitBallSlow( &p_wk->enres_param, &p_wk->result_param, HEAPID_BUCKET );
#endif
		(*p_seq)++;
		break;
		
	case BCT_MAINSEQ_RESULT_WAIT:
		{
			BOOL replay;
			
			if( MNGM_RESULT_Wait( p_wk->p_result ) == TRUE ){
				
				// replayチェック
				replay = MNGM_RESULT_GetReplay( p_wk->p_result );
				
				MNGM_RESULT_Exit( p_wk->p_result );
				p_wk->p_result = NULL;
				if( replay == FALSE ){
					return PROC_RES_FINISH;
				}else{
					(*p_seq) = BCT_MAINSEQ_ENTRY_INIT;
				}
			}
		}
		break;
	}

	return PROC_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	バケットゲームプロセス　破棄
 *
 *	@param	p_proc		ワーク
 *	@param	p_seq		シーケンス
 *
 *	@retval	PROC_RES_CONTINUE = 0,		///<動作継続中
 *	@retval	PROC_RES_FINISH,			///<動作終了
 */
//-----------------------------------------------------------------------------
PROC_RESULT BucketProc_End( PROC* p_proc, int* p_seq )
{
	BUCKET_WK* p_wk = PROC_GetWork( p_proc );
	BUCKET_PROC_WORK* pp = PROC_GetParentWork(p_proc);
	BOOL dis_error;

	switch( *p_seq ){
	case 0:


		dis_error = MNGM_ERROR_CheckDisconnect( &p_wk->enres_param );
		
		// ゲーム構成データ破棄
		BCT_GAMEDATA_Release( p_wk );

		// ワーク破棄
		PROC_FreeWork( p_proc );

		// ヒープ破棄
		sys_DeleteHeap( HEAPID_BUCKET );

		CommStateSetErrorCheck(FALSE,TRUE);

		// 切断エラーが発生しているならそのまま終わる
		if( dis_error == TRUE ){
			return PROC_RES_FINISH;
		}
		

		// 通信同期
		CommTimingSyncStart(BCT_SYNCID_ERR_END);
		(*p_seq)++;
		break;

	case 1:
		if(	CommIsTimingSync(BCT_SYNCID_ERR_END) || 
			(CommGetConnectNum() < CommInfoGetEntryNum()) ){	// 人数が少なくなったらそのまま抜ける
			return PROC_RES_FINISH;
		}
		break;

	default:
		break;
	}
	

	return PROC_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	バケット開始
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void Bucket_StartSet( BUCKET_WK* p_wk )
{
	p_wk->start = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	バケット終了
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void Bucket_EndSet( BUCKET_WK* p_wk )
{
	p_wk->end = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	サーバーに木の実データを渡す
 *
 *	@param	p_wk		ワーク
 *	@param	cp_data		木の実データ
 *	@param	netid		ネットID
 */
//-----------------------------------------------------------------------------
void Bucket_SurverNutsSet( BUCKET_WK* p_wk, const BCT_NUT_COMM* cp_data, u32 netid )
{
	u32 pl_no;
	pl_no = MNGM_ENRES_PARAM_GetNetIDtoPlNO( &p_wk->enres_param, netid );	// プレイヤーNOにして渡す
	BCT_SURVER_SetNutData( p_wk->p_surver, cp_data, pl_no );
}

//----------------------------------------------------------------------------
/**
 *	@brief	クライアントに木の実データを渡す
 *
 *	@param	p_wk		ワーク
 *	@param	cp_data		木の実データ
 *	@param	netid		ネットID
 */
//-----------------------------------------------------------------------------
void Bucket_ClientNutsSet( BUCKET_WK* p_wk, const BCT_NUT_COMM* cp_data, u32 netid )
{
	BCT_CLIENT_NutsDataSet( p_wk->p_client, cp_data );
}

//----------------------------------------------------------------------------
/**
 *	@brief	サーバーワークにスコアを設定する
 *
 *	@param	p_wk		ワーク
 *	@param	score		スコア
 *	@param	netid		ネットID
 */
//-----------------------------------------------------------------------------
void Bucket_SurverScoreSet( BUCKET_WK* p_wk, u32 score, u32 netid )
{
	u32 pl_no;
	pl_no = MNGM_ENRES_PARAM_GetNetIDtoPlNO( &p_wk->enres_param, netid );	// プレイヤーNOにして渡す
	if( p_wk->p_surver != NULL ){
		BCT_SURVER_ScoreSet( p_wk->p_surver, score, pl_no );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	全員分のスコアを受信
 *
 *	@param	p_wk		ワーク
 *	@param	cp_data		データ
 *	@param	netid		ネットID
 */
//-----------------------------------------------------------------------------
void Bucket_ClientAllScoreSet( BUCKET_WK* p_wk, const BCT_SCORE_COMM* cp_data, u32 netid )
{
	BCT_CLIENT_AllScoreSet( p_wk->p_client, cp_data );
	p_wk->score_get = TRUE;


	// スコアから結果発表パラメータを作成する
	{
		int i;
		for( i=0; i<BCT_PLAYER_NUM; i++ ){
			p_wk->result_param.score[i]		= cp_data->score[i];
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	クライアントワークにゲームデータの段階を設定
 *
 *	@param	p_wk	ワーク
 *	@param	idx		ゲームデータの段階
 */
//-----------------------------------------------------------------------------
void Bucket_ClientGameDataIdxSet( BUCKET_WK* p_wk, u32 idx )
{
	BCT_CLIENT_GameDataIdxSet( p_wk->p_client, idx );
}

//----------------------------------------------------------------------------
/**
 *	@brief	クライアントに途中の得点を設定する
 *
 *	@param	p_wk		ワーク
 *	@param	score		得点
 *	@param	netid		ネットID
 */
//-----------------------------------------------------------------------------
void Bucket_ClientMiddleScoreSet( BUCKET_WK* p_wk, u32 score, u32 netid )
{
	u32 pl_no;
	int i;


	pl_no = MNGM_ENRES_PARAM_GetNetIDtoPlNO( &p_wk->enres_param, netid );	// プレイヤーNOにして渡す
	BCT_CLIENT_MiddleScoreSet( p_wk->p_client, score, pl_no );

	// 途中の得点受信
	p_wk->middle_score_get_count[ pl_no ] ++;

	// 親なら全員から途中経過がきたら子機に通信許可を出す
	if( p_wk->netid == COMM_PARENT_ID ){
		BOOL send_ok = TRUE;

		for( i=0; i<p_wk->comm_num; i++ ){
			if( p_wk->check_middle_score_count > p_wk->middle_score_get_count[i] ){
				send_ok = FALSE;
			}
		}

		if( send_ok == TRUE ){
			OS_TPrintf( "親 middle score ok\n" );
			p_wk->check_middle_score_count ++;	// 次のカウント値へ
			CommSendData( CNM_BCT_MIDDLESCORE_OK, NULL, 0 );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	途中経過スコア取得	（通信許可）
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void Bucket_ClientMiddleScoreOkSet( BUCKET_WK* p_wk )
{
	OS_TPrintf( "middle score ok\n" );
	
	p_wk->all_middle_count_ok = TRUE;

	// クライアントシステムのタイムカウントを進める
	// OFF
/*	BCT_CLIENT_TimeCountFlagSet( p_wk->p_client, TRUE );
	if( p_wk->netid == COMM_PARENT_ID ){
		BCT_SURVER_SetCountDown( p_wk->p_surver, TRUE );
	}//*/
}


//-----------------------------------------------------------------------------
/**
 *				プライベート関数
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	VBlank関数
 */
//-----------------------------------------------------------------------------
static void BCT_VBlank( void* p_work )
{
	BUCKET_WK* p_wk = p_work;

	BCT_CLIENT_VBlank( p_wk->p_client );
}

//----------------------------------------------------------------------------
/**
 *	@brief	クライアントの木の実データを送信する
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_ClientNutsSend( BUCKET_WK* p_wk )
{
	BCT_NUT_COMM comm;
	
#ifndef BCT_DEBUG_AUTOSLOW
	while( BCT_CLIENT_NutsDataGet( p_wk->p_client, &comm ) == TRUE ){
		// 送信できないときもあるけれど、それはそれでしょうがないとする
		BCT_GAMESendData( p_wk, CNM_BCT_NUTS, &comm, sizeof(BCT_NUT_COMM) );
	}
#else

	static u8 timing = 2;
	static u8 count = 0;

	while( BCT_CLIENT_NutsDataGet( p_wk->p_client, &comm ) == TRUE ){
	}

	// あるタイミングで送信する
	count ++;
	if( count >= timing ){
		comm.pl_no = p_wk->plno;
		comm.in_flag = TRUE;
		comm.way.x = 2251;
		comm.way.y = 3422;
		comm.way.z = 3;
		comm.power = 18002;
		comm.mat.x = -266;
		comm.mat.y = 0;
		comm.mat.z = 2;
			
		count = 0;
		BCT_GAMESendData( p_wk, CNM_BCT_NUTS, &comm, sizeof(BCT_NUT_COMM) );
	}

	if( sys.trg & PAD_KEY_UP ){
		timing ++;
		OS_Printf( "send timing %d\n", timing );
	}else if( sys.trg & PAD_KEY_DOWN ){
		timing --;
		OS_Printf( "send timing %d\n", timing );
	}
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	途中経過得点を送信する
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_ClientMiddleScoreSend( BUCKET_WK* p_wk )
{
	s32 time;
	u32 score;
	BOOL result;

	time = BCT_CLIENT_GetTime( p_wk->p_client );

	// 最初は送らない
	if( time == 0 ){
		return;
	}

	if( (time % BCT_MIDDLE_SCORE_SEND_TIMING) == 0 ){

		if( p_wk->all_middle_count_ok == TRUE ){
			// 送信
			score = BCT_CLIENT_ScoreGet( p_wk->p_client );
			result = BCT_GAMESendData( p_wk, CNM_BCT_MIDDLESCORE, &score, sizeof(u32) );
			GF_ASSERT( result );

			// OFF
/*			// クライアントシステムのタイムカウントを停止
			BCT_CLIENT_TimeCountFlagSet( p_wk->p_client, FALSE );
			if( p_wk->netid == COMM_PARENT_ID ){
				BCT_SURVER_SetCountDown( p_wk->p_surver, FALSE );
			}
//*/
			// 親から通信許可を待つ
			p_wk->all_middle_count_ok = FALSE;

			OS_TPrintf( "middle score ok wait\n" );
		}
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief	ゲーム構成データ読み込み
 *
 *	@param	p_wk		ワーク
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void BCT_GAMEDATA_Load( BUCKET_WK* p_wk, u32 heapID )
{
	u32 size;
	int i;
	
	p_wk->gamedata.p_tbl	= ArcUtil_LoadEx( ARC_BUCKET_DATA, NARC_ballslow_data_bucket_data_bin, FALSE, heapID, ALLOC_TOP, &size );
	p_wk->gamedata.tblnum	= size / sizeof(BCT_GAMEDATA_ONE);

	// MYSTATUSTBL
	for( i=0; i<p_wk->enres_param.num; i++ ){
		p_wk->gamedata.cp_status[i]	= MNGM_ENRES_PARAM_GetMystatus( &p_wk->enres_param, i );
		p_wk->gamedata.vip[i]		= MNGM_ENRES_PARAM_GetVipFlag( &p_wk->enres_param, i );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ゲーム構成データ破棄
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_GAMEDATA_Release( BUCKET_WK* p_wk )
{
	sys_FreeMemoryEz( p_wk->gamedata.p_tbl );
	p_wk->gamedata.p_tbl	= NULL;
	p_wk->gamedata.tblnum	= 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	通信エラー	終了処理
 *
 *	@param	p_wk	ワーク
 *	@param	pp		パラメータ
 */
//-----------------------------------------------------------------------------
static void BCT_ErrAllSysEnd( BUCKET_WK* p_wk, BUCKET_PROC_WORK* pp )
{
	sys_VBlankFuncChange( NULL, NULL );	// VBlankセット
	sys_HBlankIntrStop();	//HBlank割り込み停止

	// 全メモリ破棄
	if( p_wk->p_entry != NULL ){
		MNGM_ENTRY_Exit( p_wk->p_entry );
		p_wk->p_entry = NULL;
	}
	if( p_wk->p_client ){
		if( p_wk->netid == COMM_PARENT_ID ){
			BCT_SURVER_Delete( p_wk->p_surver );
		}
		BCT_CLIENT_Delete( p_wk->p_client );

		DellVramTransferManager();
	}
	if( pp->vchat ){
		// ボイスチャット終了
		mydwc_stopvchat();
	}
	if( p_wk->p_result != NULL ){
		MNGM_RESULT_Exit( p_wk->p_result );
		p_wk->p_result = NULL;
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief	ゲーム情報（木の実や途中経過スコアデータ）の送信用関数
 *
 *	@param	p_wk		ワーク
 *	@param	command		コマンド
 *	@param	data		データ
 *	@param	size		サイズ
 *
 *	@retval	TRUE	送信できた
 *	@retval	FALSE	送信失敗
 */
//-----------------------------------------------------------------------------
static BOOL BCT_GAMESendData( BUCKET_WK* p_wk, int command, const void* data, int size )
{
	if( p_wk->all_middle_count_ok == FALSE ){
		OS_TPrintf( "!!!!!!not send data!!!!\n" );
		return FALSE;	// 一緒のところまで来てないので送信できない
	}
	
	// 参加プレイヤー全員の途中経過受信カウンタが同じでないと送信してはいけない
	return CommSendData( command, data, size );
}

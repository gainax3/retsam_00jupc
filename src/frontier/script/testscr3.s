//==============================================================================
/**
 * @file	testscr3.s
 * @brief	簡単な説明を書く
 * @author	matsuda
 * @date	2007.03.30(金)
 */
//==============================================================================

	.text

	.include	"../frontier_seq_def.h"
	.include	"../../../include/msgdata/msg_cmsg_dance.h"


//--------------------------------------------------------------------
//					     スクリプト本体
//--------------------------------------------------------------------
_EVENT_DATA		test_article		//一番上のEVENT_DATAは自動実行
_EVENT_DATA		test_scr_obj0		
_EVENT_DATA		test_scr_obj1		//SCRID_TEST_SCR_OBJ1
_EVENT_DATA		test_scr_obj2		//SCRID_TEST_SCR_OBJ2
_EVENT_DATA_END						//終了

#define EVENTID_TEST_SCR_ARTICLE	(0)
#define EVENTID_TEST_SCR_OBJ0		(1)
#define EVENTID_TEST_SCR_OBJ1		(2)
#define EVENTID_TEST_SCR_OBJ2		(3)


//==============================================================================
//	配置物アクター
//==============================================================================
//--------------------------------------------------------------
//	配置物リソースラベル
//--------------------------------------------------------------
_ARTICLE_RESOURCE_LABEL	article_ball_resource
	_ARTICLE_RESOURCE_DATA	ACTID_TEST_BALL
	_ARTICLE_RESOURCE_DATA_END

//--------------------------------------------------------------
//	配置物アクター
//--------------------------------------------------------------
_ARTICLE_ACTOR_LABEL	article_ball_actor
	_ARTICLE_ACTOR_DATA		ACTID_TEST_BALL, 50, 50, ON, OFF, ACTWORK_0
	_ARTICLE_ACTOR_DATA		ACTID_TEST_BALL, 100, 90, ON, OFF, ACTWORK_1
	_ARTICLE_ACTOR_DATA		ACTID_TEST_BALL, 10, 20, ON, ON, ACTWORK_2
	_ARTICLE_ACTOR_DATA_END


//==============================================================================
//	配置物アクターテスト
//==============================================================================
test_article:
	_BLACK_IN		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	//リソース＆アクター登録
	_ARTICLE_RESOURCE_SET	article_ball_resource
	_ARTICLE_ACTOR_SET		article_ball_actor
	
	_TIME_WAIT		90, FSW_LOCAL3
	

	//--------------------------------------------------------------
	//
	//	アニメ動作テスト
	//
	//--------------------------------------------------------------
	_ARTICLE_ACTOR_ANIME_START	ACTWORK_0, 1	//1番のアニメ実行
	_ARTICLE_ACTOR_ANIME_WAIT	ACTWORK_0		//アニメ終了待ち
	
	_ARTICLE_ACTOR_ANIME_START	ACTWORK_1, 2	//2番のアニメ実行
	_TIME_WAIT		60, FSW_LOCAL3
	_ARTICLE_ACTOR_ANIME_STOP	ACTWORK_1		//アニメ停止
	
//	_TIME_WAIT		60, FSW_LOCAL3

	//別画面に行っても現在の状態の退避、復帰がなっているか確認
	_NAMEIN_PROC
	_BLACK_IN		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK
	_ARTICLE_ACTOR_ANIME_START	ACTWORK_0, 1		//アニメ開始
	_TIME_WAIT		60, FSW_LOCAL3
	_ARTICLE_ACTOR_ANIME_START	ACTWORK_1, 2		//アニメ開始
	_ARTICLE_ACTOR_ANIME_STOP	ACTWORK_0		//アニメ停止
	_ARTICLE_ACTOR_ANIME_START	ACTWORK_1, 0		//アニメ開始
	_TIME_WAIT		60, FSW_LOCAL3
	
	//--------------------------------------------------------------
	//	リソースとアクターの削除テスト
	//		実際の運用時は_SCRIPT_FINISHのタイミングで自動解放されるので
	//		VRAMやアクターワークを使い回す必要がないなら、削除はしなくてOK
	//--------------------------------------------------------------
/*	
	//次々とアクター削除
	_ARTICLE_ACTOR_FREE		ACTWORK_1
	_TIME_WAIT		30, FSW_LOCAL3
	_ARTICLE_ACTOR_FREE		ACTWORK_2
	_TIME_WAIT		30, FSW_LOCAL3
	_ARTICLE_ACTOR_FREE		ACTWORK_0
	_TIME_WAIT		30, FSW_LOCAL3
	
	//リソース解放(アクターはワーク番号、リソースはACTIDで解放する事に注意)
	_ARTICLE_RESOURCE_FREE	ACTID_TEST_BALL
*/	
	_SCRIPT_FINISH


//==============================================================================
//	マップ切り替えテスト
//==============================================================================
test_scr_obj0:
	_BLACK_IN		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK
	_TALKMSG	msg_con_dance_begin_host_03
	_JUMP		test_end

test_scr_obj1:
	_BLACK_IN		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK
	_TALKMSG	msg_con_dance_end_host_01
	_JUMP		test_end

test_scr_obj2:
	_BLACK_IN		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK
	_TALKMSG	msg_con_d_tut_02
	_JUMP		test_end


test_end:
	_TIME_WAIT		30, FSW_LOCAL3
	_TALK_CLOSE

	_BLACK_OUT		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	//_END
	_SCRIPT_FINISH


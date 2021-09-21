//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_ev_anketo.c
 *	@brief		アンケート関係のイベント
 *	@author		tomoya takahashi
 *	@data		2008.05.30
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"


#include "msgdata/msg_wifi_hiroba.h"

#include "wflby_def.h"
#include "wflby_system_def.h"
#include "wflby_room_def.h"
#include "wflby_ev_anketo.h"

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
//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	シーケンス
//=====================================
enum {
	WFLBY_EV_ANKETO_CHECK,			// 何はなすか決める
	WFLBY_EV_ANKETO_BEFORE_YESNO,	// 説明を聞くか？
	WFLBY_EV_ANKETO_BEFORE_YESNOWAIT,// 説明を聞くか？
	WFLBY_EV_ANKETO_BEFORE_MSG00,	// アンケート答える前

	WFLBY_EV_ANKETO_END,			// 終了
	
	WFLBY_EV_ANKETO_MSGWAIT,		// アンケート答える前
} ;


//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	アンケートピエロワーク
//=====================================
typedef struct {
	u32 ret_seq;
} WFLBY_EV_ANKETO_PIERO;


//----------------------------------------------------------------------------
/**
 *	@brief	ピエロはなしかけ		割り込みイベント
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_EV_TALK_Piero( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno )
{
	WFLBY_EV_ANKETO_PIERO* p_evwk;
	WFLBY_SYSTEM* p_system;
	WFLBY_3DOBJCONT* p_objcont;
	WFLBY_3DPERSON* p_pierrot;
	WFLBY_3DPERSON* p_player;
	STRBUF* p_str;

	p_evwk		= WFLBY_EVENTWK_GetWk( p_wk );
	p_system	= WFLBY_ROOM_GetSystemData( p_rmwk );
	p_objcont	= WFLBY_ROOM_GetObjCont( p_rmwk );
	p_pierrot	= WFLBY_3DOBJCONT_GetPierrot( p_objcont );
	p_player	= WFLBY_3DOBJCONT_GetPlayer( p_objcont );

	switch( WFLBY_EVENTWK_GetSeq( p_wk ) ){
	case WFLBY_EV_ANKETO_CHECK:			// 何はなすか決める

		p_evwk = WFLBY_EVENTWK_AllocWk( p_wk, sizeof(WFLBY_EV_ANKETO_PIERO) );


		Snd_SePlay( WFLBY_SND_STAFF );
		
		
		{
			u32 way;

			way			= WFLBY_3DOBJCONT_GetWkObjData( p_player,  WF2DMAP_OBJPM_WAY );
			way			= WF2DMPA_OBJToolRetWayGet( way );

			// ピエロを自分のほうに向けって、さらに動きを止める
			WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_pierrot, WFLBY_3DOBJCONT_MOVENONE );
			WFLBY_3DOBJCONT_DRAW_SetUpdata( p_pierrot, FALSE );
			WFLBY_3DOBJCONT_DRAW_SetWay( p_pierrot, way );
		}


		// 広場CM終了時
		if( WFLBY_SYSTEM_Event_GetEndCM( p_system ) ){
			// メッセージ表示して終わり
			p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infoa_04 );
			p_evwk->ret_seq = WFLBY_EV_ANKETO_END;
			WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_ANKETO_MSGWAIT );
			break;
		}

		// 結果発表中かチェック
		if( (WFLBY_SYSTEM_Event_GetNeon( p_system ) == TRUE) ){
			// メッセージ表示して終わり
			p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_survey_guide_a_02 );
			p_evwk->ret_seq = WFLBY_EV_ANKETO_END;
			WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_ANKETO_MSGWAIT );
			break;
		}else{
			// 投票ずみ？
			if( WFLBY_SYSTEM_FLAG_GetAnketoInput( p_system ) == TRUE ){
				p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_survey_guide_a_01 );
				p_evwk->ret_seq = WFLBY_EV_ANKETO_END;
				WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_ANKETO_MSGWAIT );
				break;
			}else{

				p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_survey_guide_b_01 );
				p_evwk->ret_seq = WFLBY_EV_ANKETO_BEFORE_YESNO;
				WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_ANKETO_MSGWAIT );
			}
		}
		
		break;

	case WFLBY_EV_ANKETO_BEFORE_YESNO:	// 説明を聞くか？
		WFLBY_ROOM_YESNOWIN_Start( p_rmwk );
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_ANKETO_BEFORE_YESNOWAIT );
		break;

	case WFLBY_EV_ANKETO_BEFORE_YESNOWAIT:// 説明を聞くか？
		{
			WFLBY_ROOM_YESNO_RET result;
			
			result = WFLBY_ROOM_YESNOWIN_Main( p_rmwk );
			switch( result ){
			case WFLBY_ROOM_YESNO_OK:	// はい
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_ANKETO_BEFORE_MSG00 );
				WFLBY_ROOM_YESNOWIN_End( p_rmwk );
				break;
				
			case WFLBY_ROOM_YESNO_NO:	// いいえ
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_ANKETO_END );
				WFLBY_ROOM_YESNOWIN_End( p_rmwk );
				break;
				
			default:
			case WFLBY_ROOM_YESNO_WAIT:	// 選び中
				break;
			}
		}
		break;

	case WFLBY_EV_ANKETO_BEFORE_MSG00:	// アンケート答える前
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_survey_guide_b_05 );
		p_evwk->ret_seq = WFLBY_EV_ANKETO_END;
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_ANKETO_MSGWAIT );
		break;

	case WFLBY_EV_ANKETO_END:			// 終了
		WFLBY_ROOM_TALKWIN_Off( p_rmwk );
		WFLBY_ROOM_YESNOWIN_End( p_rmwk );
		WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_player, WFLBY_3DOBJCONT_MOVEPLAYER );
		WFLBY_EVENTWK_DeleteWk( p_wk );

		// ピエロ動作復帰
		WFLBY_3DOBJCONT_DRAW_SetUpdata( p_pierrot, TRUE );
		// 下向ける
		WFLBY_3DOBJCONT_SetWkActCmd( p_objcont, p_pierrot, WF2DMAP_CMD_NONE, WF2DMAP_WAY_DOWN );
		return TRUE;

	case WFLBY_EV_ANKETO_MSGWAIT:		// アンケート答える前
		// メッセージ終了待ち
		if( WFLBY_ROOM_TALKWIN_WaitEnd( p_rmwk ) == TRUE ){

			WFLBY_EVENTWK_SetSeq( p_wk, p_evwk->ret_seq );
		}
		break;

	default:
		GF_ASSERT(0);
		break;
	}

	return FALSE;
}

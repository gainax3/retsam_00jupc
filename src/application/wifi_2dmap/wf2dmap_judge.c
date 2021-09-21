//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wf2dmap_judge.c
 *	@brief		コマンド判断モジュール＆リクエストコマンドバッファ
 *	@author		tomoya takahashi
 *	@data		2007.03.15
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"

#define __WF2DMAP_JUDGE_H_GLOBAL
#include "application/wifi_2dmap/wf2dmap_judge.h"

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
 *			コマンドジャッジモジュール
 */
//-----------------------------------------------------------------------------
#define WF2DMAP_JUDGEWALLWALK_WAIT_FRAME	(8)


//-----------------------------------------------------------------------------
/**
 *			プロトタイプ宣言
 */
//-----------------------------------------------------------------------------
static BOOL WF2DMAP_JUDGESysNoneJudge( const WF2DMAP_OBJWK* cp_wk, const WF2DMAP_MAPSYS* cp_mapsys, const WF2DMAP_OBJSYS* cp_objsys, const WF2DMAP_REQCMD* cp_req, WF2DMAP_ACTCMD* p_act );
static BOOL WF2DMAP_JUDGESysTurnJudge( const WF2DMAP_OBJWK* cp_wk, const WF2DMAP_MAPSYS* cp_mapsys, const WF2DMAP_OBJSYS* cp_objsys, const WF2DMAP_REQCMD* cp_req, WF2DMAP_ACTCMD* p_act );
static BOOL WF2DMAP_JUDGESysWalkJudge( const WF2DMAP_OBJWK* cp_wk, const WF2DMAP_MAPSYS* cp_mapsys, const WF2DMAP_OBJSYS* cp_objsys, const WF2DMAP_REQCMD* cp_req, WF2DMAP_ACTCMD* p_act );
static BOOL WF2DMAP_JUDGESysRunJudge( const WF2DMAP_OBJWK* cp_wk, const WF2DMAP_MAPSYS* cp_mapsys, const WF2DMAP_OBJSYS* cp_objsys, const WF2DMAP_REQCMD* cp_req, WF2DMAP_ACTCMD* p_act );
static BOOL WF2DMAP_JUDGESysBusyJudge( const WF2DMAP_OBJWK* cp_wk, const WF2DMAP_MAPSYS* cp_mapsys, const WF2DMAP_OBJSYS* cp_objsys, const WF2DMAP_REQCMD* cp_req, WF2DMAP_ACTCMD* p_act );
static BOOL WF2DMAP_JUDGESysWallWalkJudge( const WF2DMAP_OBJWK* cp_wk, const WF2DMAP_MAPSYS* cp_mapsys, const WF2DMAP_OBJSYS* cp_objsys, const WF2DMAP_REQCMD* cp_req, WF2DMAP_ACTCMD* p_act );
static BOOL WF2DMAP_JUDGESysSlowWalkJudge( const WF2DMAP_OBJWK* cp_wk, const WF2DMAP_MAPSYS* cp_mapsys, const WF2DMAP_OBJSYS* cp_objsys, const WF2DMAP_REQCMD* cp_req, WF2DMAP_ACTCMD* p_act );
static BOOL WF2DMAP_JUDGESysStayWalkJudge( const WF2DMAP_OBJWK* cp_wk, const WF2DMAP_MAPSYS* cp_mapsys, const WF2DMAP_OBJSYS* cp_objsys, const WF2DMAP_REQCMD* cp_req, WF2DMAP_ACTCMD* p_act );

static BOOL WF2DMAP_JUDGESysMapHitCheck( const WF2DMAP_OBJWK* cp_wk, const WF2DMAP_MAPSYS* cp_objsys, WF2DMAP_WAY way );
static BOOL WF2DMAP_JUDGESysObjHitCheck( const WF2DMAP_OBJWK* cp_wk, const WF2DMAP_OBJSYS* cp_objsys, WF2DMAP_WAY way );

static void WF2DMAP_JUDGESysACTCMDSet( WF2DMAP_ACTCMD* p_act, const WF2DMAP_OBJWK* cp_wk, WF2DMAP_CMD cmd, u32 playid, WF2DMAP_WAY way );

//----------------------------------------------------------------------------
/**
 *	@brief	コマンド実行判断処理
 *
 *	@param	cp_mapsys		マップデータ管理システム
 *	@param	cp_objsys		オブジェクトデータ管理システム
 *	@param	cp_req			リクエストコマンドデータ
 *	@param	p_act			実行コマンド格納先
 *
 *	@retval	TRUE	コマンド実行可能
 *	@retval	FALSE	コマンド実行不可能
 *
 *	コマンド実行可能時にp_actに実行コマンドが格納されます
 */
//-----------------------------------------------------------------------------
BOOL WF2DMAP_JUDGESysCmdJudge( const WF2DMAP_MAPSYS* cp_mapsys, const WF2DMAP_OBJSYS* cp_objsys, const WF2DMAP_REQCMD* cp_req, WF2DMAP_ACTCMD* p_act )
{
	const WF2DMAP_OBJWK* cp_wk;
	static BOOL (* const pJudge[ WF2DMAP_CMD_NUM ])( const WF2DMAP_OBJWK* cp_wk, const WF2DMAP_MAPSYS* cp_mapsys, const WF2DMAP_OBJSYS* cp_objsys, const WF2DMAP_REQCMD* cp_req, WF2DMAP_ACTCMD* p_act ) = {
		WF2DMAP_JUDGESysNoneJudge,
		WF2DMAP_JUDGESysTurnJudge,
		WF2DMAP_JUDGESysWalkJudge,
		WF2DMAP_JUDGESysRunJudge,
		WF2DMAP_JUDGESysBusyJudge,
		WF2DMAP_JUDGESysWallWalkJudge,
		WF2DMAP_JUDGESysSlowWalkJudge,
		WF2DMAP_JUDGESysWalkJudge,
		WF2DMAP_JUDGESysStayWalkJudge,
		WF2DMAP_JUDGESysStayWalkJudge,
		WF2DMAP_JUDGESysStayWalkJudge,
		WF2DMAP_JUDGESysStayWalkJudge,
	};
	BOOL result;

	GF_ASSERT( cp_mapsys );
	GF_ASSERT( cp_objsys );
	GF_ASSERT( cp_req );
	GF_ASSERT( p_act );

	// playIDのワークを取得
	cp_wk = WF2DMAP_OBJWkConstGet( cp_objsys, cp_req->playid );
	GF_ASSERT(cp_wk);

	// 動作開始してよいかチェック
	result = pJudge[ cp_req->cmd ]( cp_wk, cp_mapsys, cp_objsys, cp_req, p_act );
	return result;
}  


//-----------------------------------------------------------------------------
/**
 *				プライベート関数
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	コマンドを実行してよいかチェックする関数群
 *
 *	@param	cp_wk			ワーク
 *	@param	cp_mapsys		マップデータ管理システム
 *	@param	cp_objsys		オブジェクトデータ管理システム
 *	@param	cp_req			リクエストコマンド
 *
 *	@retval	TRUE	実行してよい
 *	@retval	FALSE	実行してはいけない
 */
//-----------------------------------------------------------------------------
// WF2DMAP_CMD_NONE
static BOOL WF2DMAP_JUDGESysNoneJudge( const WF2DMAP_OBJWK* cp_wk, const WF2DMAP_MAPSYS* cp_mapsys, const WF2DMAP_OBJSYS* cp_objsys, const WF2DMAP_REQCMD* cp_req, WF2DMAP_ACTCMD* p_act )
{
	s32 status;

	status = WF2DMAP_OBJWkDataGet( cp_wk, WF2DMAP_OBJPM_ST );
	
	if( (status == WF2DMAP_OBJST_NONE) || (status == WF2DMAP_OBJST_BUSY) ){

		// 動作して大丈夫ならアクションコマンド作成
		WF2DMAP_JUDGESysACTCMDSet( p_act, cp_wk, cp_req->cmd, cp_req->playid, cp_req->way );
		return TRUE;
	}
	return FALSE;
}

// WF2DMAP_CMD_TURN
static BOOL WF2DMAP_JUDGESysTurnJudge( const WF2DMAP_OBJWK* cp_wk, const WF2DMAP_MAPSYS* cp_mapsys, const WF2DMAP_OBJSYS* cp_objsys, const WF2DMAP_REQCMD* cp_req, WF2DMAP_ACTCMD* p_act )
{
	s32 status;
	s32 way;

	status = WF2DMAP_OBJWkDataGet( cp_wk, WF2DMAP_OBJPM_ST );
	way = WF2DMAP_OBJWkDataGet( cp_wk, WF2DMAP_OBJPM_WAY );
	
	if( (status == WF2DMAP_OBJST_NONE) ){
		if( way != cp_req->way ){
			// 動作して大丈夫ならアクションコマンド作成
			WF2DMAP_JUDGESysACTCMDSet( p_act, cp_wk, cp_req->cmd, cp_req->playid, cp_req->way );
			return TRUE;
		}
	}
	return FALSE;
	
}

// WF2DMAP_CMD_WALK
static BOOL WF2DMAP_JUDGESysWalkJudge( const WF2DMAP_OBJWK* cp_wk, const WF2DMAP_MAPSYS* cp_mapsys, const WF2DMAP_OBJSYS* cp_objsys, const WF2DMAP_REQCMD* cp_req, WF2DMAP_ACTCMD* p_act )
{
	s32 status;
	BOOL hit;
	s32 st_frame;

	do{
		status = WF2DMAP_OBJWkDataGet( cp_wk, WF2DMAP_OBJPM_ST );
	
		// 待機状態でなくてはならない
		if( (status != WF2DMAP_OBJST_NONE) ){
			break;
		}

		// マップとの当たり判定
		hit = WF2DMAP_JUDGESysMapHitCheck( cp_wk, cp_mapsys, cp_req->way );

		// 当たり判定のないマップでないといけない
		if( hit == TRUE ){
			// 当たり判定あるので進めない
			break;
		}

		// 他の人物オブジェクトとかぶってはいけない
		hit = WF2DMAP_JUDGESysObjHitCheck( cp_wk, cp_objsys, cp_req->way );
		// 当たり判定のないマップでないといけない
		if( hit == TRUE ){
			// 当たり判定あるので進めない
			break;
		}
		
		// ここまで何事もなければあるいてＯＫ
		WF2DMAP_JUDGESysACTCMDSet( p_act, cp_wk, cp_req->cmd, cp_req->playid, cp_req->way );
		return TRUE;

	}while(0);
	
	
	// 待機状態がWF2DMAP_JUDGEWALLWALK_WAIT_FRAMEフレーム以上たったら壁歩き
	if( (status == WF2DMAP_OBJST_NONE) ){
		st_frame = WF2DMAP_OBJWkDataGet( cp_wk, WF2DMAP_OBJPM_STFRAME );
//		TOMOYA_PRINT( "st_frame %d\n", st_frame );
		if( st_frame >= WF2DMAP_JUDGEWALLWALK_WAIT_FRAME ){
			// ダミーループから抜けてきた場合は歩くことができない
			WF2DMAP_JUDGESysACTCMDSet( p_act, cp_wk, WF2DMAP_CMD_WALLWALK, cp_req->playid, cp_req->way );
			return TRUE;
		}
	}
	return FALSE;
}

// WF2DMAP_CMD_RUN
static BOOL WF2DMAP_JUDGESysRunJudge( const WF2DMAP_OBJWK* cp_wk, const WF2DMAP_MAPSYS* cp_mapsys, const WF2DMAP_OBJSYS* cp_objsys, const WF2DMAP_REQCMD* cp_req, WF2DMAP_ACTCMD* p_act )
{
	// 歩きと一緒
	return WF2DMAP_JUDGESysWalkJudge( cp_wk, cp_mapsys, cp_objsys, cp_req, p_act );
	
}

// WF2DMAP_CMD_BUSY
static BOOL WF2DMAP_JUDGESysBusyJudge( const WF2DMAP_OBJWK* cp_wk, const WF2DMAP_MAPSYS* cp_mapsys, const WF2DMAP_OBJSYS* cp_objsys, const WF2DMAP_REQCMD* cp_req, WF2DMAP_ACTCMD* p_act )
{
	s32 status;

	status = WF2DMAP_OBJWkDataGet( cp_wk, WF2DMAP_OBJPM_ST );
	
	if( (status == WF2DMAP_OBJST_NONE) ){
		WF2DMAP_JUDGESysACTCMDSet( p_act, cp_wk, cp_req->cmd, cp_req->playid, cp_req->way );
		return TRUE;
	}
	return FALSE;
}

// WF2DMAP_CMD_WALLWALK
static BOOL WF2DMAP_JUDGESysWallWalkJudge( const WF2DMAP_OBJWK* cp_wk, const WF2DMAP_MAPSYS* cp_mapsys, const WF2DMAP_OBJSYS* cp_objsys, const WF2DMAP_REQCMD* cp_req, WF2DMAP_ACTCMD* p_act )
{
	GF_ASSERT( 0 );	// WALLWALKはジャッジしません
	return FALSE;
}

// WF2DMAP_CMD_SLOWWALK
static BOOL WF2DMAP_JUDGESysSlowWalkJudge( const WF2DMAP_OBJWK* cp_wk, const WF2DMAP_MAPSYS* cp_mapsys, const WF2DMAP_OBJSYS* cp_objsys, const WF2DMAP_REQCMD* cp_req, WF2DMAP_ACTCMD* p_act )
{
	// 歩きと一緒
	return WF2DMAP_JUDGESysWalkJudge( cp_wk, cp_mapsys, cp_objsys, cp_req, p_act );
}

// WF2DMAP_CMD_STAYWALK2
static BOOL WF2DMAP_JUDGESysStayWalkJudge( const WF2DMAP_OBJWK* cp_wk, const WF2DMAP_MAPSYS* cp_mapsys, const WF2DMAP_OBJSYS* cp_objsys, const WF2DMAP_REQCMD* cp_req, WF2DMAP_ACTCMD* p_act )
{
	s32 status;

	status = WF2DMAP_OBJWkDataGet( cp_wk, WF2DMAP_OBJPM_ST );
	
	// 何もなし状態なら許す
	if( (status == WF2DMAP_OBJST_NONE) ){
		WF2DMAP_JUDGESysACTCMDSet( p_act, cp_wk, cp_req->cmd, cp_req->playid, cp_req->way );
		return TRUE;
	}
	return FALSE;
}







//----------------------------------------------------------------------------
/**
 *	@brief	マップとの当たり判定
 *
 *	@param	cp_wk			オブジェクトデータ
 *	@param	cp_mapsys		マップデータ管理システム
 *	@param	way				進む方向
 *
 *	@retval	TRUE	あたる
 *	@retval	FALSE	あたらない
 */
//-----------------------------------------------------------------------------
static BOOL WF2DMAP_JUDGESysMapHitCheck( const WF2DMAP_OBJWK* cp_wk, const WF2DMAP_MAPSYS* cp_mapsys, WF2DMAP_WAY way )
{
	WF2DMAP_POS pos;

	// 進んだ先の座標を取得
	pos = WF2DMAP_OBJWkMatrixGet( cp_wk );
	pos = WF2DMAP_OBJToolWayPosGet( pos, way );

	// 進んだ先のマップデータ取得
	return WF2DMAP_MAPSysHitGet( cp_mapsys,
			WF2DMAP_POS2GRID(pos.x), WF2DMAP_POS2GRID(pos.y) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	オブジェクトとの当たり判定
 *
 *	@param	cp_wk			オブジェクトデータ
 *	@param	cp_objsys		オブジェクトデータ管理システム
 *	@param	way				進む方向
 *
 *	@retval	TRUE	あたる
 *	@retval	FALSE	あたらない
 */
//-----------------------------------------------------------------------------
static BOOL WF2DMAP_JUDGESysObjHitCheck( const WF2DMAP_OBJWK* cp_wk, const WF2DMAP_OBJSYS* cp_objsys, WF2DMAP_WAY way )
{
	const WF2DMAP_OBJWK* cp_retwk;
	
	cp_retwk = WF2DMAP_OBJSysHitCheck( cp_wk, cp_objsys, way );
	if( cp_retwk == NULL ){
		return FALSE;
	}
	return TRUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief	アクションコマンド設定
 *
 *	@param	p_act		設定先アクションコマンド
 *	@param	cp_wk		オブジェクトワーク
 *	@param	cmd			コマンド
 *	@param	playid		プレイID
 *	@param	way			方向
 */
//-----------------------------------------------------------------------------
static void WF2DMAP_JUDGESysACTCMDSet( WF2DMAP_ACTCMD* p_act, const WF2DMAP_OBJWK* cp_wk, WF2DMAP_CMD cmd, u32 playid, WF2DMAP_WAY way )
{
	p_act->pos = WF2DMAP_OBJWkMatrixGet( cp_wk );
	p_act->cmd = cmd;
	p_act->playid = playid;
	p_act->way = way;
}

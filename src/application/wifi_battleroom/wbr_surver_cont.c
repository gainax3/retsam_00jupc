//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wbr_surver_cont.c
 *	@brief		サーバー　オブジェクト管理システム
 *	@author		tomoya takahashi
 *	@data		2007.04.19
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"

#include "application/wifi_2dmap/wf2dmap_map.h"
#include "application/wifi_2dmap/wf2dmap_obj.h"
#include "application/wifi_2dmap/wf2dmap_judge.h"
#include "application/wifi_2dmap/wf2dmap_cmdq.h"

#define __WBR_SURVER_CONT_H_GLOBAL
#include  "wbr_surver_cont.h"

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
#define WBR_SURVERCONT_GRID_X		( 16 )		// 管理グリッド数
#define WBR_SURVERCONT_GRID_Y		( 12 )		// 管理グリッド数
#define WBR_SURVERCONT_GRID_BUFF	( WBR_SURVERCONT_GRID_X*WBR_SURVERCONT_GRID_Y )

//-------------------------------------
///	マップパラメータ
//=====================================
enum {
	WBR_MAPPM_NONE,
	WBR_MAPPM_EXIT,
	WBR_MAPPM_NUM
} ;
#define WBR_MAPDATA_NONE	(WF2DMAP_MAPDATA(0, WBR_MAPPM_NONE))
#define WBR_MAPDATA_EXIT	(WF2DMAP_MAPDATA(0, WBR_MAPPM_EXIT))

//-----------------------------------------------------------------------------
/**
 *					マップデータ
 */
//-----------------------------------------------------------------------------
static const WF2DMAP_MAP	WBR_SurverContMapData[ WBR_SURVERCONT_GRID_BUFF ] = {
	// 0
	WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE,
	WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE,

	// 1
	WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE,
	WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE,

	// 2
	WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE,
	WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE,

	// 3
	WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE,
	WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE,

	// 4
	WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE,
	WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE,

	// 5
	WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE,
	WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE,

	// 6
	WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE,
	WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE,

	// 7
	WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE,
	WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE,

	// 8
	WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE,
	WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE,

	// 9
	WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE,
	WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE,

	// 10
	WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE,
	WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE,

	// 11
	WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_EXIT, WBR_MAPDATA_EXIT,
	WBR_MAPDATA_EXIT, WBR_MAPDATA_EXIT, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE, WBR_MAPDATA_NONE,
};

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------


//-------------------------------------
///	オブジェクトデータ
//=====================================
typedef struct {
	WF2DMAP_OBJWK* p_obj;
	WBR_TALK_DATA talk_data;
	WBR_TALK_REQ talk_req;
	BOOL talk_req_flag;
} WBR_SURVEROBJ;


//-------------------------------------
///	サーバーオブジェクト管理
//=====================================
typedef struct _WBR_SURVERCONT {
	WF2DMAP_MAPSYS*		p_mapsys;
	WF2DMAP_OBJSYS*		p_objsys;
	WF2DMAP_REQCMDQ*	p_reqQ;
	WF2DMAP_ACTCMDQ*	p_actQ;

	WBR_SURVEROBJ* p_wk;
	u32 wknum;
} WBR_SURVERCONT;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
static void WBR_SurverCont_ObjInit( WBR_SURVERCONT* p_sys, WBR_SURVEROBJ* p_wk, u32 aid );
static BOOL WBR_SurverCont_ExitCheck( const WBR_SURVERCONT* cp_sys );
static WF2DMAP_CMD WBR_SurverCont_OBJST2CMDGet( WF2DMAP_OBJST st );
static void WBR_SurverCont_TalkMain( WBR_SURVERCONT* p_sys );
static void WBR_SurverCont_ObjTalkStart( WBR_SURVERCONT* p_sys, WBR_SURVEROBJ* p_wk );
static void WBR_SurverCont_ObjTalkEnd( WBR_SURVERCONT* p_sys, WBR_SURVEROBJ* p_wk );

//----------------------------------------------------------------------------
/**
 *	@brief	サーバーオブジェクト管理システム
 *	
 *	@param	objnum		オブジェクト数
 *	@param	heapID		ヒープID
 *
 *	@return	サーバー管理ワーク
 */
//-----------------------------------------------------------------------------
WBR_SURVERCONT* WBR_SurverContInit( u32 objnum, u32 heapID )
{
	WBR_SURVERCONT* p_sys;
	int i;

	// メモリ確保
	p_sys = sys_AllocMemory( heapID, sizeof(WBR_SURVERCONT) );
	memset( p_sys, 0, sizeof(WBR_SURVERCONT) );

	{	// オブジェクトワーク作成
		p_sys->wknum = objnum;
		p_sys->p_wk = sys_AllocMemory( heapID, sizeof(WBR_SURVEROBJ)*p_sys->wknum );
		memset( p_sys->p_wk, 0, sizeof(WBR_SURVEROBJ)*p_sys->wknum );
	}

	{	// マップデータ作成
		p_sys->p_mapsys = WF2DMAP_MAPSysInit( WBR_SURVERCONT_GRID_X, WBR_SURVERCONT_GRID_Y, heapID );
		WF2DMAP_MAPSysDataSet( p_sys->p_mapsys, WBR_SurverContMapData );
	}

	{	// オブジェクトデータ作成
		p_sys->p_objsys = WF2DMAP_OBJSysInit( p_sys->wknum, heapID );
	}

	{	// コマンドQ
		p_sys->p_reqQ = WF2DMAP_REQCMDQSysInit( WBR_CMDQ_BUFFNUM, heapID );
		p_sys->p_actQ = WF2DMAP_ACTCMDQSysInit( WBR_CMDQ_BUFFNUM, heapID );
	}

	{
		// 人数分のオブジェクト作成
		for( i=0; i<p_sys->wknum; i++ ){
			WBR_SurverCont_ObjInit( p_sys,  &p_sys->p_wk[i], i );
		}
	}

	return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief	管理データ破棄
 *
 *	@param	p_sys	システムワーク
 */
//-----------------------------------------------------------------------------
void WBR_SurverContExit( WBR_SURVERCONT* p_sys )
{
	// 各システムワーク破棄
	WF2DMAP_MAPSysExit( p_sys->p_mapsys );
	WF2DMAP_OBJSysExit( p_sys->p_objsys );
	WF2DMAP_REQCMDQSysExit( p_sys->p_reqQ );
	WF2DMAP_ACTCMDQSysExit( p_sys->p_actQ );

	// オブジェクトワーク破棄
	sys_FreeMemoryEz( p_sys->p_wk );

	// システムワーク破棄
	sys_FreeMemoryEz( p_sys );
}


//----------------------------------------------------------------------------
/**
 *	@brief	動作処理
 *
 *	@param	p_sys		システムワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
BOOL WBR_SurverContMain( WBR_SURVERCONT* p_sys )
{
	BOOL result;
	
	// オブジェクト動作
	WF2DMAP_OBJSysMain( p_sys->p_objsys );

	// 会話開始チェック
	WBR_SurverCont_TalkMain( p_sys );

	// 終了チェック
	result = WBR_SurverCont_ExitCheck( p_sys );
	if( result ){	// 終了
		return result;
	}
	
	// ジャッジ部分
	{
		WF2DMAP_REQCMD	req;
		WF2DMAP_ACTCMD	act;
		while( WF2DMAP_REQCMDQSysCmdPop( p_sys->p_reqQ, &req ) == TRUE ){

			// ジャッジ
			result = WF2DMAP_JUDGESysCmdJudge( p_sys->p_mapsys, p_sys->p_objsys, &req, &act );
			if( result == TRUE ){
				// オブジェにアクションコマンドを反映
				WF2DMAP_OBJSysCmdSet( p_sys->p_objsys, &act );

				// アクションコマンドキューに設定
				WF2DMAP_ACTCMDQSysCmdPush( p_sys->p_actQ, &act );
			}
		}
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	今の状態のコマンドをアクションコマンドQに設定する
 *
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
void WBR_SurverContNowStatusActCmdSet( WBR_SURVERCONT* p_sys )
{
	int i;
	WF2DMAP_ACTCMD	act;

	for( i=0; i<p_sys->wknum; i++ ){
		act.playid = WF2DMAP_OBJWkDataGet( p_sys->p_wk[i].p_obj, WF2DMAP_OBJPM_PLID );
		act.way = WF2DMAP_OBJWkDataGet( p_sys->p_wk[i].p_obj, WF2DMAP_OBJPM_WAY );
		act.cmd = WF2DMAP_OBJWkDataGet( p_sys->p_wk[i].p_obj, WF2DMAP_OBJPM_ST );
		act.cmd = WBR_SurverCont_OBJST2CMDGet( act.cmd );

		if( (act.cmd == WF2DMAP_CMD_WALK) || (act.cmd == WF2DMAP_CMD_RUN) ){
			act.pos = WF2DMAP_OBJWkLastMatrixGet( p_sys->p_wk[i].p_obj );
		}else{
			act.pos = WF2DMAP_OBJWkMatrixGet( p_sys->p_wk[i].p_obj );
		}
		
		// アクションコマンドキューに設定
		WF2DMAP_ACTCMDQSysCmdPush( p_sys->p_actQ, &act );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	受信したリクエストコマンドを設定
 *
 *	@param	p_sys		システムワーク
 *	@param	cp_req		リクエストコマンドワーク
 */
//-----------------------------------------------------------------------------
void WBR_SurverContReqCmdSet( WBR_SURVERCONT* p_sys, const WF2DMAP_REQCMD* cp_req )
{
	WF2DMAP_REQCMDQSysCmdPush( p_sys->p_reqQ, cp_req );
}

//----------------------------------------------------------------------------
/**
 *	@brief	話データを設定
 *
 *	@param	p_sys		システムデータ
 *	@param	aid			AID
 *	@param	cp_data		話データ
 */
//-----------------------------------------------------------------------------
void WBR_SurverContTalkDataSet( WBR_SURVERCONT* p_sys, u32 aid, const WBR_TALK_DATA* cp_data )
{
	p_sys->p_wk[ aid ].talk_data = *cp_data;
}

//----------------------------------------------------------------------------
/**
 *	@brief	話しかけリクエストデータを格納
 *
 *	@param	p_sys		システムワーク
 *	@param	cp_data		データ
 */
//-----------------------------------------------------------------------------
void WBR_SurverContTalkReqDataSet( WBR_SURVERCONT* p_sys, const WBR_TALK_REQ* cp_data )
{
	p_sys->p_wk[ cp_data->netid ].talk_req = *cp_data;
	p_sys->p_wk[ cp_data->netid ].talk_req_flag = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	おや送信データを取得する
 *
 *	@param	p_sys		システムワーク
 *	@param	p_buff		送信データ格納先
 *
 *	@retval	TRUE	データがあった
 *	@retval	FALSE	もうデータがない
 */
//-----------------------------------------------------------------------------
BOOL WBR_SurverContSendDataPop( WBR_SURVERCONT* p_sys, WBR_OBJ_DATA* p_buff )
{
	BOOL result;

	// アクションコマンドを取得
	result = WF2DMAP_ACTCMDQSysCmdPop( p_sys->p_actQ, &p_buff->act );
	if( result ){
		// 会話データも格納	
		p_buff->talk_data = p_sys->p_wk[ p_buff->act.playid ].talk_data;

		return TRUE;
	}
	return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *		プライベート関数
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	オブジェクトデータ初期化
 *
 *	@param	p_sys		システムワーク
 *	@param	p_wk		データ格納先ワーク
 *	@param	aid			通信ID
 */
//-----------------------------------------------------------------------------
static void WBR_SurverCont_ObjInit( WBR_SURVERCONT* p_sys, WBR_SURVEROBJ* p_wk, u32 aid )
{
	static WF2DMAP_OBJDATA	data = {
		0, 0, 0,
		WF2DMAP_OBJST_NONE, WF2DMAP_WAY_UP,
		HERO,
	};
	static const u8 player_pos[2][2] = {
		{ WF2DMAP_GRID2POS(7), WF2DMAP_GRID2POS(10) },
		{ WF2DMAP_GRID2POS(8), WF2DMAP_GRID2POS(10) },
	};

	// 登録座標を設定
	data.x = player_pos[aid][0];
	data.y = player_pos[aid][1];
	data.playid = aid;

	// オブジェクトデータ登録
	p_sys->p_wk[ aid ].p_obj = WF2DMAP_OBJWkNew( p_sys->p_objsys, &data );
}

//----------------------------------------------------------------------------
/**
 *	@brief	出口チェック
 *
 *	@param	cp_sys	システムワーク
 *
 *	@retval	TRUE	出口に立っている
 *	@retval	FALSE	出口にいない
 */
//-----------------------------------------------------------------------------
static BOOL WBR_SurverCont_ExitCheck( const WBR_SURVERCONT* cp_sys )
{
	int i;
	u32 status;
	u32 way;
	WF2DMAP_POS pos;
	u32 param;
	u32 exit_count = 0;
	
	for( i=0; i<cp_sys->wknum; i++ ){
		status = WF2DMAP_OBJWkDataGet( cp_sys->p_wk[i].p_obj, WF2DMAP_OBJPM_ST );
		way = WF2DMAP_OBJWkDataGet( cp_sys->p_wk[i].p_obj, WF2DMAP_OBJPM_WAY );
		if( (status == WF2DMAP_OBJST_NONE) && (way == WF2DMAP_WAY_DOWN) ){
			pos = WF2DMAP_OBJWkMatrixGet( cp_sys->p_wk[i].p_obj );
			param = WF2DMAP_MAPSysParamGet( cp_sys->p_mapsys, 
					WF2DMAP_POS2GRID(pos.x), WF2DMAP_POS2GRID(pos.y) );

			if( param == WBR_MAPPM_EXIT ){
				exit_count ++;
			}
		}
	}
	// みんな出口にきていたら終わり
	if( exit_count >= cp_sys->wknum ){
		return TRUE;
	}
	return FALSE;
}	

//----------------------------------------------------------------------------
/**
 *	@brief	オブジェクト状態をコマンドに変更する
 *
 *	@param	st	オブジェクト状態
 */
//-----------------------------------------------------------------------------
static WF2DMAP_CMD WBR_SurverCont_OBJST2CMDGet( WF2DMAP_OBJST st )
{
	static const u32 Cmd[ WF2DMAP_OBJST_NUM ] = {
		WF2DMAP_CMD_NONE,
		WF2DMAP_CMD_TURN,
		WF2DMAP_CMD_WALK,
		WF2DMAP_CMD_RUN,
		WF2DMAP_CMD_BUSY,
	};
	return Cmd[ st ];
}

//----------------------------------------------------------------------------
/**
 *	@brief	話しかけチェックメイン関数
 *
 *	@param	p_sys	システムワーク
 */
//-----------------------------------------------------------------------------
static void WBR_SurverCont_TalkMain( WBR_SURVERCONT* p_sys )
{
	int i;
	for( i=0; i<p_sys->wknum; i++ ){
	
		// リクエストが来ているかチェック
		if( p_sys->p_wk[i].talk_req_flag == TRUE ){
			if( p_sys->p_wk[i].talk_req.flag == WBR_TALK_REQ_START ){
				WBR_SurverCont_ObjTalkStart( p_sys, &p_sys->p_wk[i] );
			}else{
				WBR_SurverCont_ObjTalkEnd( p_sys, &p_sys->p_wk[i] );
			}

			p_sys->p_wk[i].talk_req_flag = FALSE;
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	話しかけチェック処理
 *
 *	@param	p_sys		システムワーク
 *	@param	p_wk		話しかけチェックするオブジェクト
 */
//-----------------------------------------------------------------------------
static void WBR_SurverCont_ObjTalkStart( WBR_SURVERCONT* p_sys, WBR_SURVEROBJ* p_wk )
{
	u32 status;
	u32 way;
	u32 playid;
	u32 to_status;
	u32 to_way;
	u32 to_playid;
	WF2DMAP_POS pos;
	WF2DMAP_POS to_pos;
	const WF2DMAP_OBJWK* cp_hitobj;
	WBR_SURVEROBJ* p_talkobj;
	WF2DMAP_ACTCMD	act;

	status = WF2DMAP_OBJWkDataGet( p_wk->p_obj, WF2DMAP_OBJPM_ST );
	way = WF2DMAP_OBJWkDataGet( p_wk->p_obj, WF2DMAP_OBJPM_WAY );
	playid = WF2DMAP_OBJWkDataGet( p_wk->p_obj, WF2DMAP_OBJPM_PLID );
	pos = WF2DMAP_OBJWkMatrixGet( p_wk->p_obj );

	// 自分が動作停止状態かチェック
	if( status != WF2DMAP_OBJST_NONE ){
		return ;
	}

	// 自分の目の前に人がいるかチェック
	cp_hitobj = WF2DMAP_OBJSysHitCheck( p_wk->p_obj, p_sys->p_objsys, way );	
	if( cp_hitobj == NULL ){
		return ;
	}

	// 話し相手データ取得
	to_status = WF2DMAP_OBJWkDataGet( cp_hitobj, WF2DMAP_OBJPM_ST );
	to_way = WF2DMAP_OBJWkDataGet( cp_hitobj, WF2DMAP_OBJPM_WAY );
	to_playid = WF2DMAP_OBJWkDataGet( cp_hitobj, WF2DMAP_OBJPM_PLID );
	to_pos = WF2DMAP_OBJWkMatrixGet( cp_hitobj );

	// 話し相手が待機状態でナイト話せない
	if( to_status != WF2DMAP_OBJST_NONE ){
		return ;
	}

	// その人のサーバーワークを取得
	p_talkobj = &p_sys->p_wk[ to_playid ];

	// ２人ともビジー状態に変更
	// 話データを設定し、アクションコマンドを設定
	{
		// 話しかけ先の設定
		act.cmd = WF2DMAP_CMD_BUSY;
		act.way = way;
		act.playid = playid;
		act.pos = pos;
		WF2DMAP_ACTCMDQSysCmdPush( p_sys->p_actQ, &act );
		WF2DMAP_OBJSysCmdSet( p_sys->p_objsys, &act );		
		p_wk->talk_data.netid = playid;
		p_wk->talk_data.talk_mode = WBR_TALK_OYA;
		p_wk->talk_data.talk_playid = to_playid;
		p_wk->talk_data.talk_seq = WBR_TALK_SEQ_START;
	}
	{
		// 話しかけされ先設定
		act.cmd = WF2DMAP_CMD_BUSY;
		act.way = WF2DMPA_OBJToolRetWayGet( way );
		act.playid = to_playid;
		act.pos = to_pos;
		WF2DMAP_ACTCMDQSysCmdPush( p_sys->p_actQ, &act );
		WF2DMAP_OBJSysCmdSet( p_sys->p_objsys, &act );		
		p_talkobj->talk_data.netid = to_playid;
		p_talkobj->talk_data.talk_mode = WBR_TALK_KO;
		p_talkobj->talk_data.talk_playid = playid;
		p_talkobj->talk_data.talk_seq = WBR_TALK_SEQ_START;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	話しかけ終了処理
 *
 *	@param	p_sys		システムワーク
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void WBR_SurverCont_ObjTalkEnd( WBR_SURVERCONT* p_sys, WBR_SURVEROBJ* p_wk )
{
	WBR_SURVEROBJ* p_talkobj;
	WF2DMAP_ACTCMD act;
	u32 way;
	u32 to_way;
	u32 playid;
	u32 to_playid;
	WF2DMAP_POS pos;
	WF2DMAP_POS to_pos;

	
	// 話しかけ中かチェック
	if( p_wk->talk_data.talk_mode == WBR_TALK_NONE ){
		return ;
	}

	// 話しかけさきオブジェ取得
	p_talkobj = &p_sys->p_wk[ p_wk->talk_data.talk_playid ];

	// データ取得
	way = WF2DMAP_OBJWkDataGet( p_wk->p_obj, WF2DMAP_OBJPM_WAY );
	playid = WF2DMAP_OBJWkDataGet( p_wk->p_obj, WF2DMAP_OBJPM_PLID );
	pos = WF2DMAP_OBJWkMatrixGet( p_wk->p_obj );
	to_way = WF2DMAP_OBJWkDataGet( p_talkobj->p_obj, WF2DMAP_OBJPM_WAY );
	to_playid = WF2DMAP_OBJWkDataGet( p_talkobj->p_obj, WF2DMAP_OBJPM_PLID );
	to_pos = WF2DMAP_OBJWkMatrixGet( p_talkobj->p_obj );

	// 話しかけ終了
	{
		// 話しかけ親
		act.cmd = WF2DMAP_CMD_NONE;
		act.way = way;
		act.playid = playid;
		act.pos = pos;
		WF2DMAP_ACTCMDQSysCmdPush( p_sys->p_actQ, &act );
		WF2DMAP_OBJSysCmdSet( p_sys->p_objsys, &act );
		memset( &p_wk->talk_data, 0, sizeof(WBR_TALK_DATA) );
	}
	{
		// 話しかけ子
		act.cmd = WF2DMAP_CMD_NONE;
		act.way = to_way;
		act.playid = to_playid;
		act.pos = to_pos;
		WF2DMAP_ACTCMDQSysCmdPush( p_sys->p_actQ, &act );
		WF2DMAP_OBJSysCmdSet( p_sys->p_objsys, &act );
		memset( &p_talkobj->talk_data, 0, sizeof(WBR_TALK_DATA) );
	}
}

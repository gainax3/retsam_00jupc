//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wbr_client_cont.c
 *	@brief		クライアントオブジェクト管理システム
 *	@author		tomoya takahashi
 *	@data		2007.04.19
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"
#include "application/wifi_2dmap/wf2dmap_common.h"
#include "application/wifi_2dmap/wf2dmap_obj.h"
#include "application/wifi_2dmap/wf2dmap_objdraw.h"
#include "application/wifi_2dmap/wf2dmap_cmdq.h"

#include "field/fieldobj_code.h"
#include "system/clact_util.h"
#include "system/render_oam.h"
#include "system/wipe.h"

#include "system/msgdata.h"
#include "msgdata/msg.naix"
#include "msgdata/msg_wifi_br.h"
#include "system/wordset.h"
#include "system/fontproc.h"
#include "system/window.h"

#define __WBR_CLIENT_CONT_H_GLOBAL
#include "wbr_client_cont.h"

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
#define WBR_CHARMAN_CONTNUM	( 32 )	// キャラクタマネージャ管理数
#define WBR_PLTTMAN_CONTNUM	( 32 )	// キャラクタマネージャ管理数
#define WBR_CLACT_CONTNUM	( 64 )	// セルアクター管理数
#define WBR_CHARVRAM_MSIZE	( 128*1024 )	// OAMキャラクタサイズ
#define WBR_CHARVRAM_SSIZE	( 16*1024 )	// OAMキャラクタサイズ
#define WBR_BGFRAME_BACK	( GF_BGL_FRAME0_M )	// 背景面
#define WBR_BGFRAME_MSG		( GF_BGL_FRAME1_M )	// メッセージ面
#define WBR_BGMSK_BACK		( GX_PLANEMASK_BG0 )	// 背景面
#define WBR_BGMSK_MSG		( GX_PLANEMASK_BG1 )	// メッセージ面

// messageウィンドウ
#define WBR_MESSAGE_FONTBMP_X	( 4 )			// 文字面ビットマップsize
#define WBR_MESSAGE_FONTBMP_Y	( 19 )			// 文字面ビットマップsize
#define WBR_MESSAGE_FONTBMP_CX	( 23 )			// 文字面ビットマップsize
#define WBR_MESSAGE_FONTBMP_CY	( 4 )			// 文字面ビットマップsize
#define WBR_MESSAGE_FONT_COLOR		( 6 )		// 枠用カラーパレット
#define WBR_MESSAGE_FONT_CG_OFS		( 1 )		// 枠用CGX
#define WBR_MESSAGE_BMP_CG_OFS		( TALK_WIN_CGX_SIZ + 1 )
#define WBR_MESSAGE_FONT_STR_COLOR	( 5 )		// フォント用カラーパレット
#define WBR_MESSAGE_FONTBMP_PRICOLOR	( GF_PRINTCOLOR_MAKE(1,2,15) )

#define WBR_MESSAGE_STRBUFF_NUM	( 64 )


// オブジェクト データ
#define WBR_OBJ_BG_PRI	(1)
#define WBR_OBJ_CMDSEND_TIMING	(4)	// 動作終了何フレーム前からコマンドを転送するか


//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	歩き回りオブジェクトデータ
//=====================================
typedef struct {
	BOOL recv_wait;
	WF2DMAP_OBJWK*		p_obj;
	WF2DMAP_OBJDRAWWK*	p_draw;
	WBR_TALK_DATA talk_data;
	BOOL talk_recv;			// 話データ更新
	BOOL talk_recv_wait;	// 話データ更新待ち
} WBR_CLIENTCONTOBJ;

//-------------------------------------
///	messageデータ
//=====================================
typedef struct {
	MSGDATA_MANAGER *p_man;	///<マネージャ
	WORDSET* p_wordset;		///<ワーク
	STRBUF* p_tmpstr;
	u32 okuri;

	// メッセージウィンドウ
	GF_BGL_BMPWIN* p_message;
} WBR_CLIENTCONTOBJ_MSG;


//-------------------------------------
///	グラフィック関係ワーク
//=====================================
typedef struct {
	GF_BGL_INI*	p_bgl;
	CLACT_SET_PTR	p_clset;
	CLACT_U_EASYRENDER_DATA	renddata;

	WF2DMAP_OBJDRAWSYS* p_objdraw;

	// messageデータ
	WBR_CLIENTCONTOBJ_MSG	msg;

} WBR_CLIENTCONT_GRAPHIC;


//-------------------------------------
///	歩き回りシステム
//=====================================
typedef struct _WBR_CLIENTCONT{
	WBR_CLIENT_COMMON* p_prt;		// クライアントワークとの共通データ
	WBR_CLIENTCONTOBJ	obj[WBR_COMM_NUM];
	WBR_CLIENTCONTOBJ* p_player;	// 主人公
	WBR_CLIENTCONT_GRAPHIC graphic;	// グラフィックデータ
	WF2DMAP_OBJSYS*		p_objsys;	// オブジェクト管理システム
	WF2DMAP_ACTCMDQ*	p_actQ;		// アクションコマンドキュー
	BOOL end_flag;					// 親から終了の通達
} WBR_CLIENTCONT;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
static void WBR_ClientCont_Vblank( void* p_wk );
static BOOL WBR_ClientCont_ObjDataRecvCheck( const WBR_CLIENTCONT* p_sys );
static WF2DMAP_WAY WBR_ClientCont_KayWayGet( int cont );


// オブジェクトデータ構築
static void WBR_ClientCont_ObjInit( WBR_CLIENTCONT* p_sys, WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT_GRAPHIC* p_graphic, WBR_CLIENT_COMMON* p_client, int id, BOOL hero );
static void WBR_ClientCont_ObjDel( WBR_CLIENTCONTOBJ* p_obj );
static void WBR_ClientCont_PlayerMove( WBR_CLIENTCONT* p_sys, WBR_CLIENTCONTOBJ* p_player );
static BOOL WBR_ClientCont_PlayerReqCmdSendOkCheck( const WBR_CLIENTCONTOBJ* cp_obj );
static BOOL WBR_ClientCont_PlayerTalkCheck( const WBR_CLIENTCONTOBJ* cp_obj );
static WBR_CLIENTCONT_RETTYPE WBR_ClientCont_PlayerTalkMove( WBR_CLIENTCONT* p_sys, WBR_CLIENTCONTOBJ* p_player );


// グラフィック関係
static void WBR_ClientCont_GraphicInit( WBR_CLIENTCONT_GRAPHIC* p_gra, u32 heapID );
static void WBR_ClientCont_GraphicExit( WBR_CLIENTCONT_GRAPHIC* p_gra );
static void WBR_ClientCont_GraphicVblank( WBR_CLIENTCONT_GRAPHIC* p_gra );
static void WBR_ClientCont_BankInit( void );
static void WBR_ClientCont_BankExit( void );
static void WBR_ClientCont_BgInit( WBR_CLIENTCONT_GRAPHIC* p_gra, u32 heapID );
static void WBR_ClientCont_BgExit( WBR_CLIENTCONT_GRAPHIC* p_gra );
static void WBR_ClientCont_BgLoad( WBR_CLIENTCONT_GRAPHIC* p_gra, u32 heapID );
static void WBR_ClientCont_BgRelease( WBR_CLIENTCONT_GRAPHIC* p_gra );
static void WBR_ClientCont_ClactInit( WBR_CLIENTCONT_GRAPHIC* p_gra, u32 heapID );
static void WBR_ClientCont_ClactExit( WBR_CLIENTCONT_GRAPHIC* p_gra );
static void WBR_ClientCont_ClactLoad( WBR_CLIENTCONT_GRAPHIC* p_gra, u32 heapID );
static void WBR_ClientCont_ClactRelease( WBR_CLIENTCONT_GRAPHIC* p_gra );
static void WBR_ClientCont_MessageInit( WBR_CLIENTCONT_GRAPHIC* p_gra, u32 heapID );
static void WBR_ClientCont_MessageExit( WBR_CLIENTCONT_GRAPHIC* p_gra );

// message操作
static void WBR_ClientCont_MessageOn( WBR_CLIENTCONT_GRAPHIC* p_gra );
static void WBR_ClientCont_MessageOff( WBR_CLIENTCONT_GRAPHIC* p_gra );
static void WBR_ClientCont_MessageTrNameSet( WBR_CLIENTCONT_GRAPHIC* p_gra, u32 buffid, const MYSTATUS* p_status );
static void WBR_ClientCont_MessagePrint( WBR_CLIENTCONT_GRAPHIC* p_gra, u32 strid );
static BOOL WBR_ClientCont_MessageEndCheck( const WBR_CLIENTCONT_GRAPHIC* cp_gra );


// 話データ初期化
static void WBR_ClientCont_Talk_StartInit( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys );
static void WBR_ClientCont_Talk_NextInit( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys );
static void WBR_ClientCont_Talk_EndInit( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys );
static void WBR_ClientCont_Talk_ExitInit( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys );
static void WBR_ClientCont_Talk_TrCardReqInit( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys );
static void WBR_ClientCont_Talk_TrCardStartInit( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys );
static void WBR_ClientCont_Talk_TrCardEndWaitInit( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys );
static void WBR_ClientCont_Talk_TrCardEndInit( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys );

// 話データメイン
static WBR_CLIENTCONT_RETTYPE WBR_ClientCont_Talk_StartMain( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys );
static WBR_CLIENTCONT_RETTYPE WBR_ClientCont_Talk_NextMain( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys );
static WBR_CLIENTCONT_RETTYPE WBR_ClientCont_Talk_EndMain( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys );
static WBR_CLIENTCONT_RETTYPE WBR_ClientCont_Talk_ExitMain( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys );
static WBR_CLIENTCONT_RETTYPE WBR_ClientCont_Talk_TrCardReqMain( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys );
static WBR_CLIENTCONT_RETTYPE WBR_ClientCont_Talk_TrCardStartMain( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys );
static WBR_CLIENTCONT_RETTYPE WBR_ClientCont_Talk_TrCardEndWaitMain( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys );
static WBR_CLIENTCONT_RETTYPE WBR_ClientCont_Talk_TrCardEndMain( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys );

//----------------------------------------------------------------------------
/**
 *	@brief	初期化
 *
 *	@param	p_proc		プロセスワーク
 *	@param	p_seq		シーケンス
 *
 *	@retval		PROC_RES_CONTINUE = 0,		///<動作継続中
 *	@retval		PROC_RES_FINISH,			///<動作終了
 */
//-----------------------------------------------------------------------------
PROC_RESULT WBR_ClientCont_Init( PROC* p_proc, int* p_seq )
{
	WBR_CLIENTCONT* p_sys = PROC_GetWork( p_proc );
	WBR_CLIENT_COMMON* p_prt = PROC_GetParentWork( p_proc );
	int i;
	BOOL hero;

	switch( *p_seq ){
	case 0:
		
		// システムワーク作成
		p_sys = PROC_AllocWork( p_proc, sizeof(WBR_CLIENTCONT), p_prt->heapID );	
		memset( p_sys, 0, sizeof(WBR_CLIENTCONT) );

		// 上からもたったデータを保存
		p_sys->p_prt = p_prt;

		// 割り込み設定
		sys_VBlankFuncChange( WBR_ClientCont_Vblank, p_sys );	// VBlankセット
		sys_HBlankIntrStop();	//HBlank割り込み停止

		// オブジェクト管理ワーク生成
		p_sys->p_objsys = WF2DMAP_OBJSysInit( WBR_COMM_NUM, p_prt->heapID );

		// アクションコマンドキュー生成
		p_sys->p_actQ = WF2DMAP_ACTCMDQSysInit( WBR_CMDQ_BUFFNUM, p_prt->heapID );

		// グラフィック初期化
		WBR_ClientCont_GraphicInit( &p_sys->graphic, p_prt->heapID );

		// オブジェクトデータ構築
		for( i=0; i<WBR_COMM_NUM; i++ ){
			if( i==p_prt->netid ){
				hero = TRUE;
			}else{
				hero = FALSE;
			}
			WBR_ClientCont_ObjInit( p_sys, &p_sys->obj[i], &p_sys->graphic, p_prt, i, hero );
			if( hero == TRUE ){	// 主人公ワークを取っておく
				p_sys->p_player = &p_sys->obj[i];
			}
		}

		// 上からもらっているワークに自分のポインタを設定
		// 受信したデータなどをもらうときに使用する
		p_prt->p_procw = p_sys;

		// 今の状態を受信要請
		CommSendData( CNM_WBR_KO_OYADATA_GET, NULL, 0 );

		(*p_seq) ++;
		break;
	
	case 1:
		// 最新状態を取得するまでまつ
		if( WBR_ClientCont_ObjDataRecvCheck( p_sys ) == TRUE ){
			WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, 
					WIPE_FADE_OUTCOLOR, WIPE_DEF_DIV, WIPE_DEF_SYNC, p_prt->heapID );
			
			(*p_seq) ++;
		}
		break;

	case 2:
		if( WIPE_SYS_EndCheck() == TRUE ){
			return PROC_RES_FINISH;
		}
		break;
	}

	return PROC_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	メイン
 *
 *	@param	p_proc		プロセスワーク
 *	@param	p_seq		シーケンス
 *
 *	@retval		PROC_RES_CONTINUE = 0,		///<動作継続中
 *	@retval		PROC_RES_FINISH,			///<動作終了
 */
//-----------------------------------------------------------------------------
PROC_RESULT WBR_ClientCont_Main( PROC* p_proc, int* p_seq )
{
	WBR_CLIENTCONT* p_sys = PROC_GetWork( p_proc );
	WBR_CLIENT_COMMON* p_prt = PROC_GetParentWork( p_proc );
	int i;
	WBR_CLIENTCONT_RETTYPE ret_type;
	BOOL end_flag = FALSE;
	BOOL talk_flag;

	// 終了リクエストがかかった
	if( p_sys->end_flag == TRUE ){
		p_prt->next_seq = WBR_CLIENTCONT_RET_END;
		end_flag = TRUE;
	}

	// オブジェクト動作
	WF2DMAP_OBJSysMain( p_sys->p_objsys );

	//  アクションコマンドの設定
	{
		WF2DMAP_ACTCMD act;
		while( WF2DMAP_ACTCMDQSysCmdPop( p_sys->p_actQ, &act ) == TRUE ){
			WF2DMAP_OBJSysCmdSet( p_sys->p_objsys, &act );
		}
	}
	
	talk_flag = WBR_ClientCont_PlayerTalkCheck( p_sys->p_player );
	if( talk_flag == FALSE ){
		// オブジェクト動作
		WBR_ClientCont_PlayerMove( p_sys, p_sys->p_player );
	}else{
		// 会話動作
		ret_type = WBR_ClientCont_PlayerTalkMove( p_sys, p_sys->p_player );
		if( ret_type != WBR_CLIENTCONT_RET_NONE ){
			p_prt->next_seq = ret_type;
			end_flag = TRUE;
		}
	}

	// オブジェクト描画処理
	WF2DMAP_OBJDrawSysUpdata( p_sys->graphic.p_objdraw );
 
	// アクターの表示
	CLACT_Draw( p_sys->graphic.p_clset );
	
	if( end_flag == TRUE ){
		return PROC_RES_FINISH;
	}
	return PROC_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	終了
 *
 *	@param	p_proc		プロセスワーク
 *	@param	p_seq		シーケンス
 *
 *	@retval		PROC_RES_CONTINUE = 0,		///<動作継続中
 *	@retval		PROC_RES_FINISH,			///<動作終了
 */
//-----------------------------------------------------------------------------
PROC_RESULT WBR_ClientCont_End( PROC* p_proc, int* p_seq )
{
	WBR_CLIENTCONT* p_sys = PROC_GetWork( p_proc );
	WBR_CLIENT_COMMON* p_prt = PROC_GetParentWork( p_proc );
	int i; 

	switch( *p_seq ){
	case 0:
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
				WIPE_FADE_OUTCOLOR, WIPE_DEF_DIV, WIPE_DEF_SYNC, p_prt->heapID );
		
		(*p_seq) ++;
		break;

	case 1:
		if( WIPE_SYS_EndCheck() == TRUE ){
			(*p_seq)++;
		}
		break;

	case 2:

		// 割り込み終了
		sys_VBlankFuncChange( NULL, NULL );	// VBlankセット
		sys_HBlankIntrStop();	//HBlank割り込み停止
		
		// オブジェクト破棄
		for( i=0; i<WBR_COMM_NUM; i++ ){
			WBR_ClientCont_ObjDel( &p_sys->obj[i] );
		}

		WF2DMAP_OBJSysExit( p_sys->p_objsys );
		WF2DMAP_ACTCMDQSysExit( p_sys->p_actQ );
		
		// グラフィック破棄
		WBR_ClientCont_GraphicExit( &p_sys->graphic );	

		// 上に伝えていたワークにもNULLを入れておく
		p_prt->p_procw = NULL;

		// プロセスワーク破棄
		PROC_FreeWork(p_proc);
		return PROC_RES_FINISH;
	}

	return PROC_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	おやから受信したゲームデータ設定
 *
 *	@param	p_procw		ワーク
 *	@param	cp_oyadata	親データ
 */
//-----------------------------------------------------------------------------
void WBR_ClientCont_SetOyaGameData( void* p_procw, const WBR_OYA_COMM_GAME* cp_oyadata )
{
	WBR_CLIENTCONT* p_wk = p_procw;
	u32 playid;

	if( p_wk ){
		playid = cp_oyadata->obj_data.act.playid;

		//  受信完了
		p_wk->obj[ playid ].recv_wait = TRUE;

		// 話データ設定
		p_wk->obj[ playid ].talk_data = cp_oyadata->obj_data.talk_data;
		p_wk->obj[ playid ].talk_recv = TRUE;

		// アクションコマンドをキューに設定
		WF2DMAP_ACTCMDQSysCmdPush( p_wk->p_actQ, &cp_oyadata->obj_data.act );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	会話データを取得
 *
 *	@param	p_procw			ワーク
 *	@param	cp_talkdata		会話データ
 */
//-----------------------------------------------------------------------------
void WBR_ClientCont_SetTalkData( void* p_procw, const WBR_KO_COMM_TALK* cp_talkdata )
{
	WBR_CLIENTCONT* p_wk = p_procw;
	u32 netid;

	if( p_wk ){

		OS_TPrintf( "netid %d talk_seq %d talk_netid %d\n", cp_talkdata->talk_data.netid, cp_talkdata->talk_data.talk_seq, cp_talkdata->talk_data.talk_playid );

		// 自分宛かチェック
		if( (p_wk->p_prt->netid != cp_talkdata->talk_data.talk_playid) &&
			(p_wk->p_prt->netid != cp_talkdata->talk_data.netid) ){
			return ;
		}

		// シーケンスのモードにより受信したときの動作を分ける
		// トレーナーカード見せモード終了
		if( cp_talkdata->talk_data.talk_seq == WBR_TALK_SEQ_TRCARD_END ){
			// 親機からきたのであれば、次のシーケンスへ
			if( cp_talkdata->talk_data.talk_mode == WBR_TALK_OYA ){
				p_wk->p_player->talk_data.talk_seq = cp_talkdata->talk_data.talk_seq;
				p_wk->p_player->talk_recv = TRUE;
			}else{

				// 子機の場合は、その人のシーケンスデータを動かす
				p_wk->obj[ cp_talkdata->talk_data.netid ].talk_data.talk_seq = cp_talkdata->talk_data.talk_seq;
			}
		}
		// トレーナーカード終了待ち
		else if( cp_talkdata->talk_data.talk_seq == WBR_TALK_SEQ_TRCARD_ENDWAIT ){
			// 何もしない
			p_wk->p_player->talk_data.talk_seq = cp_talkdata->talk_data.talk_seq;
//			p_wk->p_player->talk_recv = TRUE;
		}
		// それ以外
		else {
			// シーケンスが変わるかチェック
			if( p_wk->p_player->talk_data.talk_seq == cp_talkdata->talk_data.talk_seq ){
				GF_ASSERT(0);
				return ;
			}

			p_wk->p_player->talk_data.talk_seq = cp_talkdata->talk_data.talk_seq;
			p_wk->p_player->talk_recv = TRUE;
		}

	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	終了リクエスト
 *	
 *	@param	p_procw		システムワーク
 */
//-----------------------------------------------------------------------------
void WBR_ClientCont_SetEndFlag( void* p_procw )
{
	WBR_CLIENTCONT* p_wk = p_procw;

	if( p_wk ){
		p_wk->end_flag = TRUE;
	}
}



//-----------------------------------------------------------------------------
/**
 *				プライベート関数
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief	VBlank関数
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
static void WBR_ClientCont_Vblank( void* p_wk )
{
	WBR_CLIENTCONT* p_wbr = p_wk;

	// グラフィックVブランク関数
	WBR_ClientCont_GraphicVblank( &p_wbr->graphic );
}

//----------------------------------------------------------------------------
/**
 *	@brief	親からのデータ受信チェック
 *
 *	@param	p_sys	システムワーク
 */
//-----------------------------------------------------------------------------
static BOOL WBR_ClientCont_ObjDataRecvCheck( const WBR_CLIENTCONT* p_sys )
{
	int i;
	for( i=0; i<WBR_COMM_NUM; i++ ){
		if( p_sys->obj[i].recv_wait == FALSE ){
			return FALSE;
		}
	}
	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	キー情報から押された方向を返す
 *
 *	@param	cont	キー情報
 *
 *	@return	方向
 */
//-----------------------------------------------------------------------------
static WF2DMAP_WAY WBR_ClientCont_KayWayGet( int cont )
{
	WF2DMAP_WAY move_way = WF2DMAP_WAY_NUM;
	
	if( sys.cont & PAD_KEY_UP ){
		move_way = WF2DMAP_WAY_UP;
	}else if( sys.cont & PAD_KEY_DOWN ){
		move_way = WF2DMAP_WAY_DOWN;
	}else if( sys.cont & PAD_KEY_LEFT ){
		move_way = WF2DMAP_WAY_LEFT;
	}else if( sys.cont & PAD_KEY_RIGHT ){
		move_way = WF2DMAP_WAY_RIGHT;
	}
	return move_way;
}


//----------------------------------------------------------------------------
/**
 *	@brief	グラフィック初期化
 *
 *	@param	p_gra		グラフィックワーク
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WBR_ClientCont_GraphicInit( WBR_CLIENTCONT_GRAPHIC* p_gra, u32 heapID )
{
	// バンク設定
	WBR_ClientCont_BankInit();

	// BGL初期化
	WBR_ClientCont_BgInit( p_gra, heapID );

	// セルアクター初期化
	WBR_ClientCont_ClactInit( p_gra, heapID );

	// BG読み込み
	WBR_ClientCont_BgLoad( p_gra, heapID );

	// セルアクター読み込み
	WBR_ClientCont_ClactLoad( p_gra, heapID );

	// message作成
	WBR_ClientCont_MessageInit( p_gra, heapID );
}


//----------------------------------------------------------------------------
/**
 *	@brief	グラフィック破棄
 *
 *	@param	p_gra		グラフィックワーク
 */
//-----------------------------------------------------------------------------
static void WBR_ClientCont_GraphicExit( WBR_CLIENTCONT_GRAPHIC* p_gra )
{
	// message破棄
	WBR_ClientCont_MessageExit( p_gra );
	
	// BG読み込み
	WBR_ClientCont_BgRelease( p_gra );

	// セルアクター読み込み
	WBR_ClientCont_ClactRelease( p_gra );

	// BGL初期化
	WBR_ClientCont_BgExit( p_gra );

	// セルアクター初期化
	WBR_ClientCont_ClactExit( p_gra );

	// バンク設定
	WBR_ClientCont_BankExit();
}

//----------------------------------------------------------------------------
/**
 *	@brief	VBlank関数
 *
 *	@param	p_gra	グラフィックワーク
 */
//-----------------------------------------------------------------------------
static void WBR_ClientCont_GraphicVblank( WBR_CLIENTCONT_GRAPHIC* p_gra )
{
    // BG書き換え
	GF_BGL_VBlankFunc( p_gra->p_bgl );

    // レンダラ共有OAMマネージャVram転送
    REND_OAMTrans();
}

//----------------------------------------------------------------------------
/**
 *	@brief	バンク設定
 */
//-----------------------------------------------------------------------------
static void WBR_ClientCont_BankInit( void )
{
    GF_BGL_DISPVRAM tbl = {
        GX_VRAM_BG_256_AC,				// メイン2DエンジンのBG
        GX_VRAM_BGEXTPLTT_NONE,			// メイン2DエンジンのBG拡張パレット
        GX_VRAM_SUB_BG_32_H,			// サブ2DエンジンのBG
        GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット
		GX_VRAM_OBJ_128_B,				// メイン2DエンジンのOBJ
        GX_VRAM_OBJEXTPLTT_NONE,		// メイン2DエンジンのOBJ拡張パレット
        GX_VRAM_SUB_OBJ_16_I,			// サブ2DエンジンのOBJ
        GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット
        GX_VRAM_TEX_NONE,				// テクスチャイメージスロット
        GX_VRAM_TEXPLTT_NONE			// テクスチャパレットスロット
	};
    GF_Disp_SetBank( &tbl );

	// 表示面設定
	sys.disp3DSW = DISP_3D_TO_MAIN;
	GF_Disp_DispSelect();
}

//----------------------------------------------------------------------------
/**
 *	@brief	バンク設定破棄
 */
//-----------------------------------------------------------------------------
static void WBR_ClientCont_BankExit( void )
{
	// VRAM設定リセット
	GX_ResetBankForBG();			// メイン2DエンジンのBG
	GX_ResetBankForBGExtPltt();		// メイン2DエンジンのBG拡張パレット
	GX_ResetBankForSubBG();			// サブ2DエンジンのBG
	GX_ResetBankForSubBGExtPltt();	// サブ2DエンジンのBG拡張パレット
	GX_ResetBankForOBJ();			// メイン2DエンジンのOBJ
	GX_ResetBankForOBJExtPltt();	// メイン2DエンジンのOBJ拡張パレット
	GX_ResetBankForSubOBJ();		// サブ2DエンジンのOBJ
	GX_ResetBankForSubOBJExtPltt();	// サブ2DエンジンのOBJ拡張パレット
	GX_ResetBankForTex();			// テクスチャイメージ
	GX_ResetBankForTexPltt();		// テクスチャパレット
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG設定
 *
 *	@param	p_gra	グラフィックワーク
 *	@param	heapID	ヒープID
 */
//-----------------------------------------------------------------------------
static void WBR_ClientCont_BgInit( WBR_CLIENTCONT_GRAPHIC* p_gra, u32 heapID )
{
	GF_ASSERT( p_gra->p_bgl == NULL );
	
	// BGL作成
	p_gra->p_bgl = GF_BGL_BglIniAlloc( heapID );

	
	{
		GF_BGL_SYS_HEADER BGsys_data = {
			GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0, GX_BG0_AS_3D
		};
		GF_BGL_InitBG(&BGsys_data);
	}

	{	// 背景
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x1000, 0, GF_BGL_SCRSIZ_512x256, GX_BG_COLORMODE_256,
			GX_BG_SCRBASE_0xc000, GX_BG_CHARBASE_0x20000, GX_BG_EXTPLTT_01,
			3, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( p_gra->p_bgl, WBR_BGFRAME_BACK, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ClearCharSet( WBR_BGFRAME_BACK, 32, 0, heapID );
		GF_BGL_ScrClear( p_gra->p_bgl, WBR_BGFRAME_BACK );
	}

	{	// メッセージ
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xd000, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( p_gra->p_bgl, WBR_BGFRAME_MSG, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ClearCharSet( WBR_BGFRAME_MSG, 32, 0, heapID );
		GF_BGL_ScrClear( p_gra->p_bgl, WBR_BGFRAME_MSG );
	}

/*	// とりあえず非表示
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
//*/
	
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG設定破棄
 *
 *	@param	p_gra	グラフィックワーク
 */
//-----------------------------------------------------------------------------
static void WBR_ClientCont_BgExit( WBR_CLIENTCONT_GRAPHIC* p_gra )
{
    GF_BGL_BGControlExit( p_gra->p_bgl, WBR_BGFRAME_BACK );
    GF_BGL_BGControlExit( p_gra->p_bgl, WBR_BGFRAME_MSG	 );
	sys_FreeMemoryEz( p_gra->p_bgl );
	p_gra->p_bgl = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	BGデータ読み込み
 *
 *	@param	p_gra	グラフィックワーク
 *	@param	heapID	ヒープID
 */
//-----------------------------------------------------------------------------
static void WBR_ClientCont_BgLoad( WBR_CLIENTCONT_GRAPHIC* p_gra, u32 heapID )
{
	// 会話フォントパレット読み込み
	TalkFontPaletteLoad( PALTYPE_MAIN_BG, WBR_MESSAGE_FONT_STR_COLOR*32, heapID );
	
	// 枠用グラフィック読み込み
	TalkWinGraphicSet( p_gra->p_bgl, WBR_BGFRAME_MSG, 
			WBR_MESSAGE_FONT_CG_OFS, WBR_MESSAGE_FONT_COLOR, 0,
			heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	BGデータ破棄
 *
 *	@param	p_gra	グラフィックワーク
 */
//-----------------------------------------------------------------------------
static void WBR_ClientCont_BgRelease( WBR_CLIENTCONT_GRAPHIC* p_gra )
{
}

//----------------------------------------------------------------------------
/**
 *	@brief	セルアクターシステム初期化
 *
 *	@param	p_gra	グラフィックワーク
 *	@param	heapID	ヒープID
 */
//-----------------------------------------------------------------------------
static void WBR_ClientCont_ClactInit( WBR_CLIENTCONT_GRAPHIC* p_gra, u32 heapID )
{
	// OAMマネージャ初期化
    NNS_G2dInitOamManagerModule();

    // 共有OAMマネージャ作成
    // レンダラ用OAMマネージャ作成
    // ここで作成したOAMマネージャをみんなで共有する
    REND_OAMInit(
        0, 124,		// メイン画面OAM管理領域
        0, 31,		// メイン画面アフィン管理領域
        0, 124,		// サブ画面OAM管理領域
        0, 31,		// サブ画面アフィン管理領域
		heapID );
	
	// キャラクタマネージャー初期化
	{
		CHAR_MANAGER_MAKE cm = {
			WBR_CHARMAN_CONTNUM,
			WBR_CHARVRAM_MSIZE,
			WBR_CHARVRAM_SSIZE,
			0
		};
		cm.heap = heapID;
		InitCharManagerReg(&cm, GX_OBJVRAMMODE_CHAR_1D_128K, GX_OBJVRAMMODE_CHAR_1D_32K );
	}
	// パレットマネージャー初期化
	InitPlttManager(WBR_PLTTMAN_CONTNUM, heapID);

	// 読み込み開始位置を初期化
	CharLoadStartAll();
	PlttLoadStartAll();

	// 通信アイコン用にキャラ＆パレット制限
	CLACT_U_WmIcon_SetReserveAreaCharManager(NNS_G2D_VRAM_TYPE_2DMAIN, GX_OBJVRAMMODE_CHAR_1D_128K);
	CLACT_U_WmIcon_SetReserveAreaPlttManager(NNS_G2D_VRAM_TYPE_2DMAIN);

	// セルアクターセット作成
    p_gra->p_clset = CLACT_U_SetEasyInit( WBR_CLACT_CONTNUM, &p_gra->renddata, heapID );

	// オブジェクト描画システム作成
	p_gra->p_objdraw = WF2DMAP_OBJDrawSysInit( p_gra->p_clset, NULL, WBR_CLACT_CONTNUM, NNS_G2D_VRAM_TYPE_2DMAIN, heapID );


	// 表示
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
}

//----------------------------------------------------------------------------
/**
 *	@brief	セルアクターシステム破棄
 *
 *	@param	p_gra	グラフィックデータワーク
 */
//-----------------------------------------------------------------------------
static void WBR_ClientCont_ClactExit( WBR_CLIENTCONT_GRAPHIC* p_gra )
{
	// 2Dcharシステム破棄
	WF2DMAP_OBJDrawSysExit( p_gra->p_objdraw );
	
    //OAMレンダラー破棄
    REND_OAM_Delete();

    // リソース解放
    DeleteCharManager();
    DeletePlttManager();

	// セルアクターセット破棄
    CLACT_DestSet(p_gra->p_clset);
}

//----------------------------------------------------------------------------
/**
 *	@brief	セルアクターデータ読み込み
 *
 *	@param	p_gra		グラフィックデータワーク
 *	@param	heapID		ヒープID
 */	
//-----------------------------------------------------------------------------
static void WBR_ClientCont_ClactLoad( WBR_CLIENTCONT_GRAPHIC* p_gra, u32 heapID )
{
	// 主人公とヒロインを読み込み
	WF2DMAP_OBJDrawSysResSet( p_gra->p_objdraw, HERO, WF_2DC_MOVERUN, heapID );	
	WF2DMAP_OBJDrawSysResSet( p_gra->p_objdraw, HEROINE, WF_2DC_MOVERUN, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	セルアクターデータ破棄
 *
 *	@param	p_gra		グラフィックデータワーク
 */
//-----------------------------------------------------------------------------
static void WBR_ClientCont_ClactRelease( WBR_CLIENTCONT_GRAPHIC* p_gra )
{
	// 主人公とヒロインを破棄
	WF2DMAP_OBJDrawSysResDel( p_gra->p_objdraw, HERO );	
	WF2DMAP_OBJDrawSysResDel( p_gra->p_objdraw, HEROINE );
}


//----------------------------------------------------------------------------
/**
 *	@brief	message初期化
 *
 *	@param	p_gra	ワーク
 *	@param	heapID	ヒープID
 */
//-----------------------------------------------------------------------------
static void WBR_ClientCont_MessageInit( WBR_CLIENTCONT_GRAPHIC* p_gra, u32 heapID )
{
	// メッセージビットマップ作成
	p_gra->msg.p_message = GF_BGL_BmpWinAllocGet( heapID, 1 );

	// ビットマップウィンドウ登録
	GF_BGL_BmpWinAdd( p_gra->p_bgl, p_gra->msg.p_message,
			WBR_BGFRAME_MSG, 
			WBR_MESSAGE_FONTBMP_X,
			WBR_MESSAGE_FONTBMP_Y,
			WBR_MESSAGE_FONTBMP_CX, 
			WBR_MESSAGE_FONTBMP_CY, 
			WBR_MESSAGE_FONT_STR_COLOR,
			WBR_MESSAGE_BMP_CG_OFS );

	// クリーン
	GF_BGL_BmpWinDataFill( p_gra->msg.p_message, 15 );

	// 枠の書き込み
	BmpTalkWinWrite( p_gra->msg.p_message, WINDOW_TRANS_OFF,
			WBR_MESSAGE_FONT_CG_OFS,
			WBR_MESSAGE_FONT_COLOR );

	// messageマネージャ
	p_gra->msg.p_man = MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, NARC_msg_wifi_br_dat, heapID );	
	p_gra->msg.p_wordset = WORDSET_Create( heapID );	

	// STRバッファ作成
	p_gra->msg.p_tmpstr = STRBUF_Create( WBR_MESSAGE_STRBUFF_NUM, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	message破棄
 *
 *	@param	p_gra	ワーク
 */
//-----------------------------------------------------------------------------
static void WBR_ClientCont_MessageExit( WBR_CLIENTCONT_GRAPHIC* p_gra )
{
	GF_BGL_BmpWinDel( p_gra->msg.p_message );
	GF_BGL_BmpWinFree( p_gra->msg.p_message, 1 );

	STRBUF_Delete( p_gra->msg.p_tmpstr );
	WORDSET_Delete( p_gra->msg.p_wordset );
	MSGMAN_Delete( p_gra->msg.p_man );
}

//----------------------------------------------------------------------------
/**
 *	@brief	messageON
 *
 *	@param	p_gra	ワーク
 */
//-----------------------------------------------------------------------------
static void WBR_ClientCont_MessageOn( WBR_CLIENTCONT_GRAPHIC* p_gra )
{
	GF_BGL_BmpWinOn( p_gra->msg.p_message );
	GF_Disp_GX_VisibleControl( WBR_BGMSK_MSG, VISIBLE_ON );
}

//----------------------------------------------------------------------------
/**
 *	@brief	messageOFF
 *
 *	@param	p_gra	ワーク
 */
//-----------------------------------------------------------------------------
static void WBR_ClientCont_MessageOff( WBR_CLIENTCONT_GRAPHIC* p_gra )
{
	GF_Disp_GX_VisibleControl( WBR_BGMSK_MSG, VISIBLE_OFF );
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナー名設定
 *
 *	@param	p_gra		ワーク
 *	@param	buffid		バッファID
 *	@param	p_status	ステータスデータ
 */
//-----------------------------------------------------------------------------
static void WBR_ClientCont_MessageTrNameSet( WBR_CLIENTCONT_GRAPHIC* p_gra, u32 buffid, const MYSTATUS* p_status )
{
	WORDSET_RegisterPlayerName( p_gra->msg.p_wordset, buffid, p_status );
}

//----------------------------------------------------------------------------
/**
 *	@brief	messageプリント
 *
 *	@param	p_gra		ワーク
 *	@param	strid		STRID
 */
//-----------------------------------------------------------------------------
static void WBR_ClientCont_MessagePrint( WBR_CLIENTCONT_GRAPHIC* p_gra, u32 strid )
{
	STRBUF* p_str;

	// クリーン
	GF_BGL_BmpWinDataFill( p_gra->msg.p_message, 15 );

	p_str = MSGMAN_AllocString( p_gra->msg.p_man, strid );
	WORDSET_ExpandStr( p_gra->msg.p_wordset, p_gra->msg.p_tmpstr, p_str );

	p_gra->msg.okuri = GF_STR_PrintColor( p_gra->msg.p_message, FONT_TALK, p_gra->msg.p_tmpstr, 0, 0, 1, WBR_MESSAGE_FONTBMP_PRICOLOR, NULL );

	STRBUF_Delete( p_str );
}

//----------------------------------------------------------------------------
/**
 *	@brief	messageプリント終了待ち
 *
 *	@param	cp_gra	ワーク
 *
 *	@retval	TRUE	終了
 */
//-----------------------------------------------------------------------------
static BOOL WBR_ClientCont_MessageEndCheck( const WBR_CLIENTCONT_GRAPHIC* cp_gra )
{
	if( GF_MSG_PrintEndCheck( cp_gra->msg.okuri ) == 0 ){
		return TRUE;
	}
	return FALSE;
}	


//----------------------------------------------------------------------------
/**
 *	@brief	オブジェクトデータ生成
 *
 *	@param	p_sys		システムワーク
 *	@param	p_obj		オブジェクトワーク
 *	@param	p_graphic	グラフィックデータ
 *	@param	p_client	クライアントデータ
 *	@param	id			AID
 *	@param	hero		主人公フラグ
 */
//-----------------------------------------------------------------------------
static void WBR_ClientCont_ObjInit( WBR_CLIENTCONT* p_sys, WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT_GRAPHIC* p_graphic, WBR_CLIENT_COMMON* p_client, int id, BOOL hero )
{
	WF2DMAP_OBJDATA add = {
		0, 0, 0,
		WF2DMAP_OBJST_NONE,
		WF2DMAP_WAY_UP,
		0
	};
	u32 sex;
	
	// ぷれいやーIDを設定
	add.playid = id;
	
	// mystatusから性別を取得
	sex = MyStatus_GetMySex( p_client->cp_mystatus->cp_mystatus[ id ] );
	if( sex == PM_MALE ){
		add.charaid = HERO;
	}else{
		add.charaid = HEROINE;
	}
	
	// オブジェクトデータ生成
	p_obj->p_obj = WF2DMAP_OBJWkNew( p_sys->p_objsys, &add );
	p_obj->p_draw = WF2DMAP_OBJDrawWkNew( p_graphic->p_objdraw, 
			p_obj->p_obj, hero, p_client->heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	オブジェクトデータ破棄
 *
 *	@param	p_obj	オブジェクトワーク
 */
//-----------------------------------------------------------------------------
static void WBR_ClientCont_ObjDel( WBR_CLIENTCONTOBJ* p_obj )
{
	WF2DMAP_OBJDrawWkDel( p_obj->p_draw );
	WF2DMAP_OBJWkDel( p_obj->p_obj );
}

//----------------------------------------------------------------------------
/**
 *	@brief	主人公動作
 *	
 *	@param	p_sys		システムワーク
 *	@param	p_player	主人公ワーク
 */
//-----------------------------------------------------------------------------
static void WBR_ClientCont_PlayerMove( WBR_CLIENTCONT* p_sys, WBR_CLIENTCONTOBJ* p_player )
{
	u32 way;
	WBR_KO_COMM_GAME comm_data;
	u32 move_way;
	BOOL send_check;
	WBR_TALK_REQ talk_req;

	// このフレームコマンドを転送するかチェック
	send_check = WBR_ClientCont_PlayerReqCmdSendOkCheck( p_player );
	if( send_check == FALSE ){
		return ;
	}
	
	// 話しかけチェック
	if( sys.trg & PAD_BUTTON_A ){
		talk_req.netid = WF2DMAP_OBJWkDataGet( p_player->p_obj, WF2DMAP_OBJPM_PLID );
		talk_req.flag = WBR_TALK_REQ_START;
		// 送信
		CommSendData( CNM_WBR_KO_TALKREQ, &talk_req, sizeof(WBR_TALK_REQ) );
	}else{
	
		// キー情報から方向を取得
		move_way = WBR_ClientCont_KayWayGet( sys.cont );

		if( move_way != WF2DMAP_WAY_NUM ){

			// 今向いている方向と比べてコマンド発行
			way = WF2DMAP_OBJWkDataGet( p_player->p_obj, WF2DMAP_OBJPM_WAY );
			comm_data.req.way = move_way;
			comm_data.req.playid = WF2DMAP_OBJWkDataGet( p_player->p_obj, WF2DMAP_OBJPM_PLID );
			if( move_way == way ){
				if( sys.cont & PAD_BUTTON_B ){
					comm_data.req.cmd = WF2DMAP_CMD_RUN;
				}else{
					comm_data.req.cmd = WF2DMAP_CMD_WALK;
				}
			}else{
				comm_data.req.cmd = WF2DMAP_CMD_TURN;
			}

			// 送信
			CommSendData( CNM_WBR_KO_GAMEDATA, &comm_data, sizeof(WBR_KO_COMM_GAME) );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	リクエストコマンドを転送するする必要があるかチェック
 *
 *	@param	cp_obj	オブジェクトワーク
 *
 *	@retval	TRUE	リクエストコマンドを転送して大丈夫
 *	@retval	FALSE	リクエストコマンドは転送しなくてよい
 */
//-----------------------------------------------------------------------------
static BOOL WBR_ClientCont_PlayerReqCmdSendOkCheck( const WBR_CLIENTCONTOBJ* cp_obj )
{
	s32 frame;
	s32 end_frame;
	u32 status;
	
	// 待機状態なら無条件で転送
	status = WF2DMAP_OBJWkDataGet( cp_obj->p_obj, WF2DMAP_OBJPM_ST );
	if( status == WF2DMAP_OBJST_NONE ){
		return TRUE;
	}
	// ビジー状態なら無条件で転送しない
	if( status == WF2DMAP_OBJST_BUSY ){
		return FALSE;
	}

	frame = WF2DMAP_OBJWkDataGet( cp_obj->p_obj, WF2DMAP_OBJPM_FRAME );
	end_frame = WF2DMAP_OBJWkDataGet( cp_obj->p_obj, WF2DMAP_OBJPM_ENDFRAME );

	if( frame >= end_frame - WBR_OBJ_CMDSEND_TIMING ){
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	会話中チェック
 *
 *	@param	cp_obj	オブジェクトワーク
 *
 *	@retval	TRUE	会話中
 *	@retval	FALSE	会話してない
 */
//-----------------------------------------------------------------------------
static BOOL WBR_ClientCont_PlayerTalkCheck( const WBR_CLIENTCONTOBJ* cp_obj )
{
	if( cp_obj->talk_data.talk_mode == WBR_TALK_NONE ){
		return FALSE;
	}
	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	話しかけ動作
 *
 *	@param	p_sys		システムワーク
 *	@param	p_player	動作ワーク
 */
//-----------------------------------------------------------------------------
static WBR_CLIENTCONT_RETTYPE WBR_ClientCont_PlayerTalkMove( WBR_CLIENTCONT* p_sys, WBR_CLIENTCONTOBJ* p_player )
{
	static void (* const pTalkInit[])( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys ) = {
		WBR_ClientCont_Talk_StartInit,
		WBR_ClientCont_Talk_NextInit,
		WBR_ClientCont_Talk_EndInit,
		WBR_ClientCont_Talk_ExitInit,
		WBR_ClientCont_Talk_TrCardReqInit,
		WBR_ClientCont_Talk_TrCardStartInit,
		WBR_ClientCont_Talk_TrCardEndWaitInit,
		WBR_ClientCont_Talk_TrCardEndInit,
	};
	static WBR_CLIENTCONT_RETTYPE (* const pTalkMain[])( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys ) = {
		WBR_ClientCont_Talk_StartMain,
		WBR_ClientCont_Talk_NextMain,
		WBR_ClientCont_Talk_EndMain,
		WBR_ClientCont_Talk_ExitMain,
		WBR_ClientCont_Talk_TrCardReqMain,
		WBR_ClientCont_Talk_TrCardStartMain,
		WBR_ClientCont_Talk_TrCardEndWaitMain,
		WBR_ClientCont_Talk_TrCardEndMain,
	};

	// 初期化処理
	if( p_player->talk_recv ){
		p_player->talk_recv = FALSE;
		pTalkInit[ p_player->talk_data.talk_seq ]( p_player, p_sys );
		p_player->talk_recv_wait = FALSE;
	}

	// メイン
	if( p_player->talk_recv_wait == FALSE ){
		return pTalkMain[ p_player->talk_data.talk_seq ]( p_player, p_sys );
	}

	return WBR_CLIENTCONT_RET_NONE;
}



//----------------------------------------------------------------------------
/**
 *	@brief	話しかけ開始
 *
 *	@param	p_obj		オブジェクト
 *	@param	p_sys		システム
 */
//-----------------------------------------------------------------------------
static void WBR_ClientCont_Talk_StartInit( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys )
{
	const MYSTATUS* cp_mystatus;
	
	// 相手トレーナー名を設定
	cp_mystatus = p_sys->p_prt->cp_mystatus->cp_mystatus[ p_obj->talk_data.talk_playid ];
	WBR_ClientCont_MessageTrNameSet( &p_sys->graphic, 0, cp_mystatus );

	// 親か子によってmessageを変更
	if( p_obj->talk_data.talk_mode == WBR_TALK_OYA ){
		WBR_ClientCont_MessagePrint( &p_sys->graphic, WBR_000 );
	}else{
		WBR_ClientCont_MessagePrint( &p_sys->graphic, WBR_001 );
	}
	WBR_ClientCont_MessageOn( &p_sys->graphic );
}

//----------------------------------------------------------------------------
/**
 *	@brief	続いての話しかけ　初期化
 *
 *	@param	p_obj	オブジェクトワーク
 *	@param	p_sys	システムワーク
 */
//-----------------------------------------------------------------------------
static void WBR_ClientCont_Talk_NextInit( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys )
{
	const MYSTATUS* cp_mystatus;
	
	// 相手トレーナー名を設定
	cp_mystatus = p_sys->p_prt->cp_mystatus->cp_mystatus[ p_obj->talk_data.talk_playid ];
	WBR_ClientCont_MessageTrNameSet( &p_sys->graphic, 0, cp_mystatus );

/*	通常
	// 親か子によってmessageを変更
	if( p_obj->obj_data.talk_mode == WBR_TALK_OYA ){
		WBR_ClientCont_MessagePrint( &p_sys->graphic, WBR_000 );
	}else{
		WBR_ClientCont_MessagePrint( &p_sys->graphic, WBR_001 );
	}
//*/
	WBR_ClientCont_MessagePrint( &p_sys->graphic, WBR_005 );
}

//----------------------------------------------------------------------------
/**
 *	@brief	話しかけ終了	初期化
 *
 *	@param	p_obj		オブジェクトワーク
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
static void WBR_ClientCont_Talk_EndInit( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys )
{
	WBR_ClientCont_MessageOff( &p_sys->graphic );
}

//----------------------------------------------------------------------------
/**
 *	@brief	話しかけ抜ける	初期化
 *
 *	@param	p_obj		オブジェクトワーク
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
static void WBR_ClientCont_Talk_ExitInit( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys )
{
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナーカードリクエスト
 *
 *	@param	p_obj		オブジェクトワーク
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
static void WBR_ClientCont_Talk_TrCardReqInit( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys )
{
	if( p_obj->talk_data.talk_mode == WBR_TALK_OYA ){
		WBR_ClientCont_MessagePrint( &p_sys->graphic, WBR_002 );
	}else{
		WBR_ClientCont_MessagePrint( &p_sys->graphic, WBR_003 );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナーカード初期化
 *
 *	@param	p_obj		オブジェクトワーク
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
static void WBR_ClientCont_Talk_TrCardStartInit( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys )
{
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナーカード終了待ち	初期化
 *
 *	@param	p_obj		オブジェクトワーク
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
static void WBR_ClientCont_Talk_TrCardEndWaitInit( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys )
{
	// ほかの人の終了をまっている状態
	WBR_ClientCont_MessagePrint( &p_sys->graphic, WBR_004 );
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナーカード終了	初期化
 *
 *	@param	p_obj		オブジェクトワーク
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
static void WBR_ClientCont_Talk_TrCardEndInit( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys )
{
}


//----------------------------------------------------------------------------
/**
 *	@brief	話しかけ開始	メイン
 *
 *	@param	p_obj		オブジェクトワーク
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
static WBR_CLIENTCONT_RETTYPE WBR_ClientCont_Talk_StartMain( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys )
{
	WBR_KO_COMM_TALK comm_data;
	
	if( p_obj->talk_data.talk_mode == WBR_TALK_OYA ){
		if( WBR_ClientCont_MessageEndCheck( &p_sys->graphic ) == TRUE ){
			// messageが終わったらトレーナーカードリクエストへ
			comm_data.talk_data = p_obj->talk_data;
			comm_data.talk_data.talk_seq = WBR_TALK_SEQ_TRCARD_REQ;
			CommSendData( CNM_WBR_KO_TALKDATA, &comm_data, sizeof(WBR_KO_COMM_TALK) );
			p_obj->talk_recv_wait = 1;	// 受信待ち
		}
	}
	return WBR_CLIENTCONT_RET_NONE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	話しかけ続き
 *
 *	@param	p_obj		オブジェクトワーク
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
static WBR_CLIENTCONT_RETTYPE WBR_ClientCont_Talk_NextMain( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys )
{
	WBR_KO_COMM_TALK comm_data;
/*	通常
	if( p_obj->talk_data.talk_mode == WBR_TALK_OYA ){
		if( WBR_ClientCont_MessageEndCheck( &p_sys->graphic ) == TRUE ){
			// messageが終わったらトレーナーカードリクエストへ
			comm_data.talk_data = p_obj->talk_data;
			comm_data.talk_data.talk_seq = WBR_TALK_SEQ_TRCARD_REQ;
			CommSendData( CNM_WBR_KO_TALKDATA, &p_obj->ko_comm_talk, sizeof(WBR_KO_COMM_TALK) );
			p_obj->talk_recv_wait = 1;	// 受信待ち
		}
	}
//*/
	if( p_obj->talk_data.talk_mode == WBR_TALK_OYA ){
		if( WBR_ClientCont_MessageEndCheck( &p_sys->graphic ) == TRUE ){
			// messageが終わったらトレーナーカードリクエストへ
			comm_data.talk_data = p_obj->talk_data;
			comm_data.talk_data.talk_seq = WBR_TALK_SEQ_END;
			CommSendData( CNM_WBR_KO_TALKDATA, &comm_data, sizeof(WBR_KO_COMM_TALK) );
			p_obj->talk_recv_wait = 1;	// 受信待ち
		}
	}
	return WBR_CLIENTCONT_RET_NONE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	話しかけ終了	メイン
 *	
 *	@param	p_obj		オブジェクトワーク
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
static WBR_CLIENTCONT_RETTYPE WBR_ClientCont_Talk_EndMain( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys )
{
	WBR_KO_COMM_TALK comm_data;
	if( p_obj->talk_data.talk_mode == WBR_TALK_OYA ){
		if( WBR_ClientCont_MessageEndCheck( &p_sys->graphic ) == TRUE ){
			// messageが終わったら終了へ
			comm_data.talk_data = p_obj->talk_data;
			comm_data.talk_data.talk_seq = WBR_TALK_SEQ_EXIT;
			CommSendData( CNM_WBR_KO_TALKDATA, &comm_data, sizeof(WBR_KO_COMM_TALK) );
			p_obj->talk_recv_wait = 1;	// 受信待ち
		}
	}

	return WBR_CLIENTCONT_RET_NONE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	話しかけ抜ける	メイン
 *
 *	@param	p_obj		オブジェクトワーク
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
static WBR_CLIENTCONT_RETTYPE WBR_ClientCont_Talk_ExitMain( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys )
{
	WBR_TALK_REQ talk_req;

	if( p_obj->talk_data.talk_mode == WBR_TALK_OYA ){
		talk_req.flag = WBR_TALK_REQ_END;
		talk_req.netid = WF2DMAP_OBJWkDataGet( p_obj->p_obj, WF2DMAP_OBJPM_PLID );
		CommSendData( CNM_WBR_KO_TALKREQ, &talk_req, sizeof(WBR_TALK_REQ) );
		p_obj->talk_recv_wait = 1;	// 受信待ち
	}
	return WBR_CLIENTCONT_RET_NONE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナーカードリクエストメイン
 *
 *	@param	p_obj		オブジェクトワーク
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
static WBR_CLIENTCONT_RETTYPE WBR_ClientCont_Talk_TrCardReqMain( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys )
{
	WBR_KO_COMM_TALK comm_data;
	
	// 子ならトレーナーカード実行へ
	if( p_obj->talk_data.talk_mode == WBR_TALK_KO ){
		if( WBR_ClientCont_MessageEndCheck( &p_sys->graphic ) == TRUE ){
			// messageが終わったらトレーナーカードへ
			comm_data.talk_data = p_obj->talk_data;
			comm_data.talk_data.talk_seq = WBR_TALK_SEQ_TRCARD_START;
			CommSendData( CNM_WBR_KO_TALKDATA, &comm_data, sizeof(WBR_KO_COMM_TALK) );
			p_obj->talk_recv_wait = 1;	// 受信待ち
		}
	}
	return WBR_CLIENTCONT_RET_NONE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナーカード　メイン
 *
 *	@param	p_obj		オブジェクトワーク
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
static WBR_CLIENTCONT_RETTYPE WBR_ClientCont_Talk_TrCardStartMain( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys )
{
	WBR_KO_COMM_TALK comm_data;
	// シーケンスを待機まちにしておく
	comm_data.talk_data = p_obj->talk_data;
	comm_data.talk_data.talk_seq = WBR_TALK_SEQ_TRCARD_ENDWAIT;
	CommSendData( CNM_WBR_KO_TALKDATA, &comm_data, sizeof(WBR_KO_COMM_TALK) );
	p_obj->talk_recv_wait = 1;	// 受信待ち
	
	// 開始なので、開始させる
	return WBR_CLIENTCONT_RET_TRCARD;
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナーカード終了待ち
 *
 *	@param	p_obj		オブジェクトワーク
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
static WBR_CLIENTCONT_RETTYPE WBR_ClientCont_Talk_TrCardEndWaitMain( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys )
{
	WBR_KO_COMM_TALK comm_data;
	
	// 親機は子機がトレーナーカード終了になるのを待つ
	if( p_obj->talk_data.talk_mode == WBR_TALK_OYA ){
		// 子機の話データが次のシーケンスに言っていたらOK
		if( p_sys->obj[ p_obj->talk_data.talk_playid ].talk_data.talk_seq == WBR_TALK_SEQ_TRCARD_END ){
			comm_data.talk_data = p_obj->talk_data;
			comm_data.talk_data.talk_seq = WBR_TALK_SEQ_TRCARD_END;
			CommSendData( CNM_WBR_KO_TALKDATA, &comm_data, sizeof(WBR_KO_COMM_TALK) );
			p_obj->talk_recv_wait = 1;	// 受信待ち
		}
	}else{
		// 子機はおやきに終了を転送
		comm_data.talk_data = p_obj->talk_data;
		comm_data.talk_data.talk_seq = WBR_TALK_SEQ_TRCARD_END;
		CommSendData( CNM_WBR_KO_TALKDATA, &comm_data, sizeof(WBR_KO_COMM_TALK) );
		p_obj->talk_recv_wait = 1;	// 受信待ち
	}
	return WBR_CLIENTCONT_RET_NONE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナーカード終了	メイン
 *
 *	@param	p_obj		オブジェクトワーク
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
static WBR_CLIENTCONT_RETTYPE WBR_ClientCont_Talk_TrCardEndMain( WBR_CLIENTCONTOBJ* p_obj, WBR_CLIENTCONT* p_sys )
{
	WBR_KO_COMM_TALK comm_data;

	if( p_obj->talk_data.talk_mode == WBR_TALK_OYA ){
		comm_data.talk_data = p_obj->talk_data;
		comm_data.talk_data.talk_seq = WBR_TALK_SEQ_NEXT;
		CommSendData( CNM_WBR_KO_TALKDATA, &comm_data, sizeof(WBR_KO_COMM_TALK) );
		p_obj->talk_recv_wait = 1;	// 受信待ち
	}
	return WBR_CLIENTCONT_RET_NONE;
}


//==============================================================================================
/**
 * @file	stage.c
 * @brief	「バトルステージ」メインソース
 * @author	Satoshi Nohara
 * @date	07.06.08
 */
//==============================================================================================
#include "system/main.h"
#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "system/clact_tool.h"
#include "system/brightness.h"
#include "system/wipe.h"
#include "system/snd_tool.h"
#include "system/pm_str.h"
#include "system/buflen.h"
#include "system/wordset.h"
#include "system/bmp_menu.h"
#include "system/window.h"
#include "system/fontproc.h"
#include "system/numfont.h"
#include "gflib/strbuf_family.h"
#include "savedata/config.h"
#include "application/p_status.h"
#include "poketool/pokeparty.h"
#include "poketool/poke_number.h"
#include "poketool/pokeicon.h"
#include "savedata/b_tower.h"

#include "communication/comm_system.h"
#include "communication/comm_tool.h"
#include "communication/comm_def.h"
#include "communication/comm_info.h"
#include "communication/wm_icon.h"

#include "stage_sys.h"
#include "stage_clact.h"
#include "stage_bmp.h"
#include "stage_obj.h"
#include "application/stage.h"
#include "../frontier_tool.h"					//Frontier_PokeParaMake
#include "../frontier_def.h"
#include "../stage_tool.h"						//STAGE_POKE_RANGE
#include "../stage_def.h"
#include "../comm_command_frontier.h"

#include "../../field/comm_direct_counter_def.h"//

#include "msgdata\msg.naix"						//NARC_msg_??_dat
#include "msgdata\msg_stage_room.h"				//msg_??

#include "../graphic/frontier_obj_def.h"
#include "../graphic/frontier_bg_def.h"


#include "system/pm_overlay.h"
FS_EXTERN_OVERLAY(frontier_common);


//==============================================================================================
//
//	デバック用
//
//==============================================================================================
#ifdef PM_DEBUG
//PROC_DATA* p_proc;
//#define DEBUG_BRAIN_SET							//有効で、ブレーン登場にする
#endif


//==============================================================================================
//
//	定義
//
//==============================================================================================
//シーケンス定義
enum {
	SEQ_GAME_INIT,											//初期化
	SEQ_GAME_PSD,											//ポケモンステータス処理

	SEQ_GAME_TYPE_SEL,										//タイプ選択中

	SEQ_GAME_SEND_RECV,										//送受信

	SEQ_GAME_END_MULTI,										//(通信)終了
	SEQ_GAME_END,											//終了

	SEQ_GAME_PAIR_END,										//パートナーが決定したので強制終了
};

//フォントカラー
#define	COL_BLUE	( GF_PRINTCOLOR_MAKE(FBMP_COL_BLUE,FBMP_COL_BLU_SDW,FBMP_COL_NULL) )//青
#define	COL_RED		( GF_PRINTCOLOR_MAKE(FBMP_COL_RED,FBMP_COL_RED_SDW,FBMP_COL_NULL) )	//赤

//タイプ表示位置
#define TYPE_START_X	(1)				//開始Ｘ
#define TYPE_START_Y	(16)			//開始Ｙ
#define TYPE_X			(64)			//間隔Ｘ
#define TYPE_Y			(36)			//間隔Ｙ

//ランク表示位置
//#define RANK_START_X	(12)			//開始Ｘ
//#define RANK_START_X	(14)			//開始Ｘ
#define RANK_START_X	(18)			//開始Ｘ
#define RANK_START_Y	(4)				//開始Ｙ
#define RANK_X			(64)			//間隔Ｘ
#define RANK_Y			(36)			//間隔Ｙ

//カーソル表示位置
#define CSR_START_X		(32)//(16)		//開始Ｘ
#define CSR_START_Y		(16)			//開始Ｙ
#define CSR_X			(64)			//間隔Ｘ
#define CSR_Y			(36)			//間隔Ｙ

//ポケモン名の表示位置
#define POKE_NAME_PX	(10*8)
#define POKE_NAME_PY	(21*8)
#define POKE_NAME_SX	(9*8)
#define POKE_NAME_SY	(2*8)

//選択した時のカラー
#define DECIDE_BG_PL_DAT	( 0x3f00 )

enum{
	NORMAL_PAL_CHG = 0,
	DECIDE_PAL_CHG,
	PAIR_PAL_CHG,
	MAX_PAL_CHG,
};

#define NORMAL_PAL		(0)				//通常パレット番号
#define NORMAL_PAL2		(1)				//通常パレット番号(2本使用している)(前半のタイプ)
#define NORMAL_PAL3		(2)				//通常パレット番号(2本使用している)(後半のタイプ)
#define DECIDE_PAL		(5)				//プレイヤー決定パレット番号
#define PAIR_PAL		(4)				//パートナー決定パレット番号
#define MAX_PAL			(3)				//レベル最高パレット番号

//選んだポケモンタイプに対しての回答
enum{
	FINAL_ANSWER_NONE = 0,				//何もなし
	FINAL_ANSWER_YES,					//選んだポケモンタイプで進む
	FINAL_ANSWER_NO,					//やっぱり選び直す
};

#define CSR_POS_NONE	(0xff)			//カーソル何も選んでいない

//パートナーが終了したことを知らせるメッセージウェイト
//#define STAGE_PAIR_END_WAIT		(30)

//アイコンの位置
#define ICON_X			(160)
#define ICON_Y			(160)//(168)

//カーソルがポケモンを選択している時の位置
#define CSR_POKE_X		(128)
#define CSR_POKE_Y		(168)//(169)

//選択してメニューが出るまでのウェイト
#define STAGE_MENU_WAIT	(8)//(15)

#define STAGE_DECIDE_NONE	(0xff)

#define STAGE_END_WAIT		(10)				//終了するまでの表示ウェイト
#define STAGE_MSG_WAIT		(10)				//友達が選んでいるの表示ウェイト

#define STAGE_BRAIN_CSR_POS_NONE	(0x75)		//ブレーンを選ぶ前のカーソル位置の初期値


//==============================================================================================
//
//	メッセージ関連の定義
//
//==============================================================================================
#define STAGE_MENU_BUF_MAX		(2)						//メニューバッファの最大数
#define BS_FONT					(FONT_SYSTEM)			//フォント種類
#define STAGE_MSG_BUF_SIZE		(600)//(800)//(1024)			//会話のメッセージsize
#define STAGE_MENU_BUF_SIZE		(32)					//メニューのメッセージsize
#define PLAYER_NAME_BUF_SIZE	(PERSON_NAME_SIZE + EOM_SIZE)	//プレイヤー名のメッセージsize
#define POKE_NAME_BUF_SIZE		(MONS_NAME_SIZE + EOM_SIZE)		//ポケモン名のメッセージsize


//==============================================================================================
//
//	(ファクトリー)通常のステータス画面ページ
//
//==============================================================================================
static const u8 PST_PageTbl_Normal[] = {
	PST_PAGE_INFO,			// 「ポケモンじょうほう」
	PST_PAGE_MEMO,			// 「トレーナーメモ」
	PST_PAGE_PARAM,			// 「ポケモンのうりょく」
	PST_PAGE_CONDITION,		// 「コンディション」
	PST_PAGE_B_SKILL,		// 「たたかうわざ」
	PST_PAGE_C_SKILL,		// 「コンテストわざ」
	PST_PAGE_RIBBON,		// 「きねんリボン」
	PST_PAGE_RET,			// 「もどる」
	PST_PAGE_MAX
};


//==============================================================================================
//
//	構造体宣言
//
//==============================================================================================
struct _STAGE_WORK{

	PROC* proc;										//PROCへのポインタ
	PROC* child_proc;								//PROCへのポインタ

	u8	sub_seq;									//シーケンス
	u8	type;										//引数として渡されたバトルタイプ
	u8	msg_index;									//メッセージindex
	u8	recover;									//復帰フラグ

	u8	tmp_csr_pos;								//退避してあるカーソル位置
	u8	csr_pos;									//現在のカーソル位置
	u8	master_flag;								//全てレベル10
	u8	menu_flag;									//メニュー表示中かフラグ

	u16	psd_flag;									//ポケモンステータス中フラグ
	u16 init_flag;									//初回フラグ

	u16	counter;
	u8	typesel_recieve_count;						//受信カウント
	u8	send_req;									//タイプ決定の送信リクエスト

	u8	parent_decide_pos;
	u8	type_sel_return_flag;						//TYPE_SELシーケンスに戻るフラグ
	u8	decide_csr_pos;								//決定したカーソル位置
	u8	wait;

	u16 btl_count;									//今何戦目か
	u8	brain_appear;								//ブレーン登場かフラグ
	u8	brain_before_csr_pos;						//ブレーンを選ぶ前のカーソル位置

	MSGDATA_MANAGER* msgman;						//メッセージマネージャー
	WORDSET* wordset;								//単語セット
	STRBUF* msg_buf;								//メッセージバッファポインタ
	STRBUF* tmp_buf;								//テンポラリバッファポインタ

	STRBUF* menu_buf[STAGE_MENU_BUF_MAX];			//メニューバッファポインタ
	STRCODE str[PERSON_NAME_SIZE + EOM_SIZE];		//メニューのメッセージ

	GF_BGL_INI*	bgl;								//BGLへのポインタ
	GF_BGL_BMPWIN bmpwin[STAGE_BMPWIN_MAX];			//BMPウィンドウデータ

	//BMPメニュー(bmp_menu.h)
	BMPMENU_HEADER MenuH;							//BMPメニューヘッダー
	BMPMENU_WORK* mw;								//BMPメニューワーク
	BMPMENU_DATA Data[STAGE_MENU_BUF_MAX];			//BMPメニューデータ

	PALETTE_FADE_PTR pfd;							//パレットフェード

	NUMFONT* num_font;								//8x8フォント

	//const CONFIG* config;							//コンフィグポインタ
	CONFIG* config;									//コンフィグポインタ
	SAVEDATA* sv;									//セーブデータポインタ

	PSTATUS_DATA* psd;								//ポケモンステータス

	STAGE_CLACT stage_clact;						//セルアクタデータ
	STAGE_OBJ* p_csr;								//カーソルOBJのポインタ格納テーブル
	STAGE_OBJ* p_icon;								//アイコンOBJのポインタ格納テーブル

	int psd_pos;									//ポケモンステータスの選択結果取得

	u16* p_ret_work;								//STAGE_CALL_WORKの戻り値ワークへのポインタ

	//バトルステージ用
	POKEPARTY* p_party;
	u8* p_rank;										//ランクデータのポインタ(0-9,10で渡される)
	u16* p_pair_poke_level;							//相手のポケモンのレベル

	ARCHANDLE* hdl;

	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//STAGE_COMM stage_comm;
#if 1
	//通信用：データバッファ
	u16	send_buf[STAGE_COMM_BUF_LEN];

	//通信用
	u8	pair_csr_pos;								//パートナーが選んだポケモンタイプ(カーソル位置)
	u8	pair_final_answer;							//0=何もなし、1=最終決定、2=キャンセル
	u8	pair_dummy;
#endif
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	
	u32 dummy_work;
};


//==============================================================================================
//
//	プロトタイプ宣言
//
//==============================================================================================
//PROC
PROC_RESULT StageProc_Init( PROC * proc, int * seq );
PROC_RESULT StageProc_Main( PROC * proc, int * seq );
PROC_RESULT StageProc_End( PROC * proc, int * seq );

//シーケンス
static BOOL Seq_GameInit( STAGE_WORK* wk );
static BOOL Seq_GamePsd( STAGE_WORK* wk );
static void TypeSelInit( STAGE_WORK* wk );
static BOOL Seq_GameTypeSel( STAGE_WORK* wk );
static BOOL Seq_GameSendRecv( STAGE_WORK* wk );
static BOOL Seq_GameEndMulti( STAGE_WORK* wk );
static BOOL Seq_GameEnd( STAGE_WORK* wk );
static BOOL Seq_GamePairEnd( STAGE_WORK* wk );

//共通処理
static void StageCommon_Delete( STAGE_WORK* wk );
static void Stage_Recover( STAGE_WORK* wk );
static void Stage_InitSub1( void );
static void Stage_InitSub2( STAGE_WORK* wk );

//共通初期化、終了
static void Stage_ObjInit( STAGE_WORK* wk );
static void Stage_BgInit( STAGE_WORK* wk );
static void Stage_BgExit( GF_BGL_INI * ini );

//設定
static void VBlankFunc( void * work );
static void SetVramBank(void);
static void SetBgHeader( GF_BGL_INI * ini );

//BGグラフィックデータ
static void Stage_SetMainBgGraphic( STAGE_WORK * wk, u32 frm  );
static void Stage_SetMainBgPalette( void );
static void Stage_SetSubBgGraphic( STAGE_WORK * wk, u32 frm  );

//メッセージ
static u8 StageWriteMsg( STAGE_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font );
static u8 StageWriteMsgSimple( STAGE_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font );
static u8 Stage_EasyMsg( STAGE_WORK* wk, int msg_id, u8 font );

//メニュー
static void StageInitMenu( STAGE_WORK* wk, GF_BGL_BMPWIN* win, u8 y_max );
static void StageSetMenuData( STAGE_WORK* wk, u8 no, u8 param, int msg_id );
static void Stage_SetMenu2( STAGE_WORK* wk );

//文字列
static void Stage_SetNumber( STAGE_WORK* wk, u32 bufID, s32 number );
static void Stage_SetPokeName( STAGE_WORK* wk, u32 bufID, POKEMON_PASO_PARAM* ppp );
static void Stage_SetPlayerName( STAGE_WORK* wk, u32 bufID );
static void PlayerNameWrite( STAGE_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 font );
static void PairNameWrite( STAGE_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 font );
static void PokeNameWrite( STAGE_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 f_col, u8 s_col, u8 b_col, u8 font );
static void PokeNameWriteEx( STAGE_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 f_col, u8 s_col, u8 b_col, u8 font, u16 monsno, u8 sex );
static void TypeWrite( STAGE_WORK* wk, GF_BGL_BMPWIN* win, u8 f_col, u8 s_col, u8 b_col, u8 font );
static void RankWrite( STAGE_WORK* wk, GF_BGL_BMPWIN* win, u8 num, u32 x, u32 y );
static void RankWriteAll( STAGE_WORK* wk, GF_BGL_BMPWIN* win );

//ポケモンステータス
static void Stage_SetPSD( STAGE_WORK* wk );

//ツール
static BOOL Stage_CheckType( STAGE_WORK* wk, u8 type );
static void NextSeq( STAGE_WORK* wk, int* seq, int next );
static int KeyCheck( int key );
static void BgCheck( STAGE_WORK* wk );
static void CsrMove( STAGE_WORK* wk, int key );
static u16 GetCsrX( STAGE_WORK* wk );
static u16 GetCsrY( STAGE_WORK* wk );
static void ScrnPalChg( GF_BGL_INI* ini, u8 csr_pos, u8 flag );
static void GameStartTypeScrnPalChg( STAGE_WORK* wk, GF_BGL_INI* ini );
static void StageMasterFlagSet( STAGE_WORK* wk );
static u8 Stage_GetCsrPos( u8 csr_pos );
static BOOL Stage_BtlCountLeaderCheck( STAGE_WORK* wk );
static BOOL Stage_BrainAppearCheck( STAGE_WORK* wk );

//通信
BOOL Stage_CommSetSendBuf( STAGE_WORK* wk, u16 type, u16 param );
//BOOL Stage_CommWaitRecvBuf( STAGE_WORK* wk, u16 type );
void Stage_CommSendBufBasicData( STAGE_WORK* wk, u16 type );
void Stage_CommRecvBufBasicData(int id_no,int size,void *pData,void *work);
void Stage_CommSendBufPokeTypeData( STAGE_WORK* wk, u16 type, u16 param );
void Stage_CommRecvBufPokeTypeData(int id_no,int size,void *pData,void *work);
void Stage_CommSendBufFinalAnswerData( STAGE_WORK* wk, u16 type, u16 param );
void Stage_CommRecvBufFinalAnswerData(int id_no,int size,void *pData,void *work);

//サブシーケンス
static void Stage_SeqSubMenuWinClear( STAGE_WORK* wk );
static void Stage_SeqSubTypeSelYesNo( STAGE_WORK* wk );
static void Stage_SeqSubTypeSelNo( STAGE_WORK* wk );
static void Stage_SeqSubWinRet( STAGE_WORK* wk );


//==============================================================================================
//
//	PROC
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	プロセス関数：初期化
 *
 * @param	proc	プロセスデータ
 * @param	seq		シーケンス
 *
 * @return	"処理状況"
 */
//--------------------------------------------------------------
PROC_RESULT StageProc_Init( PROC * proc, int * seq )
{
	int i;
	STAGE_WORK* wk;
	STAGE_CALL_WORK* stage_call;

	Overlay_Load( FS_OVERLAY_ID(frontier_common), OVERLAY_LOAD_NOT_SYNCHRONIZE );

	Stage_InitSub1();

	sys_CreateHeap( HEAPID_BASE_APP, HEAPID_STAGE, 0x20000 );

	wk = PROC_AllocWork( proc, sizeof(STAGE_WORK), HEAPID_STAGE );
	memset( wk, 0, sizeof(STAGE_WORK) );

	wk->bgl				= GF_BGL_BglIniAlloc( HEAPID_STAGE );
	wk->proc			= proc;
	stage_call			= (STAGE_CALL_WORK*)PROC_GetParentWork( proc );
	wk->sv				= stage_call->sv;
	wk->type			= stage_call->type;
	wk->p_ret_work		= &stage_call->ret_work;
	wk->config			= SaveData_GetConfig( wk->sv );			//コンフィグポインタを取得
	wk->p_party			= stage_call->p_party;
	wk->p_rank			= stage_call->p_rank;					//0オリジン
	wk->p_pair_poke_level = stage_call->p_pair_poke_level;		//
	wk->btl_count		= (stage_call->btl_count + 1);			//今何戦目か
	wk->csr_pos			= stage_call->csr_pos;					//初期カーソル位置

	wk->pair_csr_pos	= CSR_POS_NONE;

	wk->parent_decide_pos = STAGE_DECIDE_NONE;

	wk->brain_before_csr_pos = STAGE_BRAIN_CSR_POS_NONE;		//ブレーンを選ぶ前のカーソル位置

	Stage_InitSub2( wk );

	if( Stage_CommCheck(wk->type) == TRUE ){
		CommCommandFrontierInitialize( wk );
	}

	(*seq) = SEQ_GAME_INIT;
	OS_Printf( "(*seq) = %d\n", (*seq) );

	return PROC_RES_FINISH;
}

//--------------------------------------------------------------
/**
 * @brief	プロセス関数：メイン
 *
 * @param	proc	プロセスデータ
 * @param	seq		シーケンス
 *
 * @return	"処理状況"
 */
//--------------------------------------------------------------
PROC_RESULT StageProc_Main( PROC * proc, int * seq )
{
	STAGE_WORK* wk  = PROC_GetWork( proc );
	//OS_Printf( "(*seq) = %d\n", (*seq) );

#ifdef DEBUG_FRONTIER_LOOP
	sys.cont |= PAD_BUTTON_A;
	sys.trg |= PAD_BUTTON_A;
#endif	//DEBUG_FRONTIER_LOOP

	if( wk->parent_decide_pos != STAGE_DECIDE_NONE ){
		switch( *seq ){

		//送受信へ強制移動
		case SEQ_GAME_TYPE_SEL:

			//ステータスに切り替える時以外
			if( wk->psd_flag == 0 ){
				Stage_SeqSubMenuWinClear( wk );
				//BmpTalkWinClear( &wk->bmpwin[BMPWIN_TALK], WINDOW_TRANS_ON );
				NextSeq( wk, seq, SEQ_GAME_SEND_RECV );
			}
			break;

		//何もしない
		//case SEQ_GAME_INIT:
		//case SEQ_GAME_PSD:					//ポケモンステータス処理
		//case SEQ_GAME_SEND_RECV:
		//case SEQ_GAME_END_MULTI:
		//case SEQ_GAME_END:
		//case SEQ_GAME_PAIR_END:
		//	break;
		};
	}

	switch( *seq ){

#if 1
	//-----------------------------------
	//ポケモンステータス処理
	case SEQ_GAME_PSD:
		if( Seq_GamePsd(wk) == TRUE ){
			NextSeq( wk, seq, SEQ_GAME_INIT );
		}else{
			return PROC_RES_CONTINUE;	//注意！]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
		}
		//break;	//注意！]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#endif

	//-----------------------------------
	//初期化
	case SEQ_GAME_INIT:
		if( Seq_GameInit(wk) == TRUE ){
			NextSeq( wk, seq, SEQ_GAME_TYPE_SEL );
		}
		break;

	//-----------------------------------
	//タイプ選択中
	case SEQ_GAME_TYPE_SEL:
		if( Seq_GameTypeSel(wk) == TRUE ){

			if( wk->send_req == 1 ){
				NextSeq( wk, seq, SEQ_GAME_SEND_RECV );					//送受信へ
			}else{

				if( wk->recover == 1 ){
					NextSeq( wk, seq, SEQ_GAME_PSD );					//ポケモンステータス
					return PROC_RES_CONTINUE;	//注意！]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
				}else{

					//通信タイプの時
					if( Stage_CommCheck(wk->type) == TRUE ){
						NextSeq( wk, seq, SEQ_GAME_END_MULTI );			//(通信)終了へ
					}else{
						NextSeq( wk, seq, SEQ_GAME_END );				//終了へ
					}
				}
			}
		}
		break;

	//-----------------------------------
	//送受信
	case SEQ_GAME_SEND_RECV:
		if( Seq_GameSendRecv(wk) == TRUE ){

			if( wk->type_sel_return_flag == 1 ){
				wk->type_sel_return_flag = 0;
				NextSeq( wk, seq, SEQ_GAME_TYPE_SEL );						//タイプ選択中へ
			}else{

				//通信タイプの時
				if( Stage_CommCheck(wk->type) == TRUE ){
					NextSeq( wk, seq, SEQ_GAME_END_MULTI );					//(通信)終了へ
				}else{
					NextSeq( wk, seq, SEQ_GAME_END );						//終了へ
				}
			}
		}
		break;

	//-----------------------------------
	//通信交換終了
	case SEQ_GAME_END_MULTI:
		if( Seq_GameEndMulti(wk) == TRUE ){
			NextSeq( wk, seq, SEQ_GAME_END );							//終了へ
		}
		break;

	//-----------------------------------
	//終了
	case SEQ_GAME_END:
		if( Seq_GameEnd(wk) == TRUE ){
			return PROC_RES_FINISH;
		}
		break;

	//-----------------------------------
	//パートナーがポケモンタイプを決定したので強制終了へ
	case SEQ_GAME_PAIR_END:
		if( Seq_GamePairEnd(wk) == TRUE ){
			NextSeq( wk, seq, SEQ_GAME_END_MULTI );						//(通信)終了へ
		}
		break;
	}

	CLACT_Draw( wk->stage_clact.ClactSet );		//セルアクター常駐関数

	return PROC_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief	プロセス関数：終了
 *
 * @param	proc	プロセスデータ
 * @param	seq		シーケンス
 *
 * @return	"処理状況"
 */
//--------------------------------------------------------------
PROC_RESULT StageProc_End( PROC * proc, int * seq )
{
	int i;
	STAGE_WORK* wk = PROC_GetWork( proc );

	*(wk->p_ret_work) = wk->csr_pos;					//戻り値格納ワークへ代入(カーソル位置)

	OS_Printf( "*(wk->p_ret_work) = %d\n", *(wk->p_ret_work) );

	StageCommon_Delete( wk );							//削除処理

	PROC_FreeWork( proc );								//ワーク開放

	sys_VBlankFuncChange( NULL, NULL );					//VBlankセット
	sys_DeleteHeap( HEAPID_STAGE );

	Overlay_UnloadID( FS_OVERLAY_ID(frontier_common) );

	return PROC_RES_FINISH;
}


//==============================================================================================
//
//	シーケンス
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	シーケンス：ゲーム初期化
 *
 * @param	wk		STAGE_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameInit( STAGE_WORK* wk )
{
	switch( wk->sub_seq ){

	//コマンド設定後の同期
	case 0:
		if( (wk->init_flag == 0) && (Stage_CommCheck(wk->type) == TRUE) ){
			CommToolTempDataReset();
			CommTimingSyncStart( DBC_TIM_STAGE_TYPE_INIT );
		}
		wk->sub_seq++;
		break;

	//タイミングコマンドが届いたか確認
	case 1:
		if( (wk->init_flag == 0) && (Stage_CommCheck(wk->type) == TRUE) ){
			if( CommIsTimingSync(DBC_TIM_STAGE_TYPE_INIT) == TRUE ){
				CommToolTempDataReset();
				wk->init_flag = 1;
				wk->sub_seq++;
			}
		}else{
			wk->sub_seq++;
		}
		break;

	//ブラックイン
	case 2:
		TypeSelInit( wk );

		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, 
						WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC*3, HEAPID_STAGE );

		wk->sub_seq++;
		break;
		
	//フェード終了待ち
	case 3:
		if( WIPE_SYS_EndCheck() == TRUE ){
			return TRUE;
		}
		break;
	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	wk		STAGE_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static void TypeSelInit( STAGE_WORK* wk )
{
	int i;

	//タイプ表示
	TypeWrite(	wk, &wk->bmpwin[BMPWIN_TYPE], FBMP_COL_BLACK, FBMP_COL_BLK_SDW, 
				FBMP_COL_NULL, BS_FONT );

	//ランク表示
	RankWriteAll( wk, &wk->bmpwin[BMPWIN_TYPE] );

	//ポケモン名を表示
	PokeNameWrite(	wk, &wk->bmpwin[BMPWIN_POKE1], 0, 0,
					FBMP_COL_BLACK, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BS_FONT );

	//画面復帰時
	if( wk->recover == 1 ){
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	シーケンス：ポケモンステータス処理
 *
 * @param	wk		STAGE_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GamePsd( STAGE_WORK* wk )
{
	switch( wk->sub_seq ){

	case 0:
		SDK_ASSERTMSG( wk->child_proc != NULL, "child_procがNULLです！" );

		if( ProcMain(wk->child_proc) == TRUE ){
			wk->psd_pos = wk->psd->pos;									//結果取得
			OS_Printf( "wk->psd_pos = %d\n", wk->psd_pos );
			sys_FreeMemoryEz( wk->psd );
			sys_FreeMemoryEz( wk->child_proc );
			wk->child_proc = NULL;
			Stage_Recover( wk );
			wk->psd_flag = 0;
			return TRUE;
		}
		break;
	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	ゲームタイプ選択中
 *
 * @param	wk		STAGE_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
enum{
	SEQ_SUB_START = 0,
	SEQ_SUB_TYPE_SEL,
	SEQ_SUB_MENU_WAIT,
	SEQ_SUB_MENU_MAIN,
	SEQ_SUB_BRAIN_MSG,
	SEQ_SUB_BRAIN_MSG_WAIT,
	SEQ_SUB_PSD_CALL,
};

static BOOL Seq_GameTypeSel( STAGE_WORK* wk )
{
	int i;
	u32 ret,color;

	switch( wk->sub_seq ){

	case SEQ_SUB_START:
		wk->recover = 0;
		wk->sub_seq = SEQ_SUB_TYPE_SEL;
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//タイプを選択中
	case SEQ_SUB_TYPE_SEL:
		CsrMove( wk, sys.trg );

		//決定ボタンを押した時
		if( sys.trg & PAD_BUTTON_A ){

#if 0
			//全てのランクが10ではない時
			if( wk->master_flag == 0 ){
				//ランクがマックスだったら選択出来ない(0-9,10)
				if( Stage_GetTypeLevel(Stage_GetCsrPos(wk->csr_pos),wk->p_rank) >= 
																		STAGE_TYPE_LEVEL_MAX ){

					Snd_SePlay( SEQ_SE_DP_BOX03 );
					return FALSE;					//注意！]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
				}
			}
#endif

			//ダミータイプの時
			if( Stage_GetPokeType(wk->csr_pos) == DUMMY_TYPE ){
				Snd_SePlay( SEQ_SE_DP_DECIDE );
				WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
								WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_STAGE );

				wk->psd_flag = 1;
				wk->sub_seq = SEQ_SUB_PSD_CALL;					//ポケモンステータスへ
			}else{

				//ブレーンが登場する戦闘回数だったら
				//if( Stage_BtlCountLeaderCheck(wk->btl_count) == TRUE ){
				if( Stage_BrainAppearCheck(wk) == TRUE ){

					//「???」以外は選べない
					if( Stage_GetPokeType(wk->csr_pos) != HATE_TYPE ){
						Snd_SePlay( SEQ_SE_DP_BOX03 );
						return FALSE;					//注意！]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
					}
				}else{

					//ランクがマックスだったら選択出来ない(0-9,10)
					if( Stage_GetTypeLevel(Stage_GetCsrPos(wk->csr_pos),wk->p_rank) >= 
																		STAGE_TYPE_LEVEL_MAX ){
						Snd_SePlay( SEQ_SE_DP_BOX03 );

#ifdef DEBUG_FRONTIER_LOOP
						wk->csr_pos++;
						StageObj_SetObjPos( wk->p_csr, GetCsrX(wk), GetCsrY(wk) );
#endif	//DEBUG_FRONTIER_LOOP

						return FALSE;					//注意！]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
					}

					//通常は「???」は選べない
					if( Stage_GetPokeType(wk->csr_pos) == HATE_TYPE ){
						Snd_SePlay( SEQ_SE_DP_BOX03 );
						return FALSE;					//注意！]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
					}
				}

				Snd_SePlay( SEQ_SE_DP_DECIDE );

				//タイプが会話ウィンドウで隠されてしまうことがあるので、
				//先に選択したタイプを色替えしてウェイトを入れる
				if( Stage_CommCheck(wk->type) == FALSE ){
					//指定フレームのスクリーンの指定位置のパレットを変更する
					ScrnPalChg( wk->bgl, wk->csr_pos, DECIDE_PAL_CHG );
					GF_BGL_LoadScreenV_Req( wk->bgl, BS_FRAME_BG );	//スクリーンデータ転送
				}

				wk->counter = STAGE_MENU_WAIT;
				wk->sub_seq = SEQ_SUB_MENU_WAIT;				//メニューウェイトへ
			}
		}

		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//メニューウェイト
	case SEQ_SUB_MENU_WAIT:
		wk->counter--;
		if( wk->counter == 0 ){

			if( Stage_CommCheck(wk->type) == FALSE ){
				Stage_SeqSubTypeSelYesNo( wk );
				wk->sub_seq = SEQ_SUB_MENU_MAIN;				//メニューへ
				break;
			}else{

#if 1
				StageTalkWinPut( &wk->bmpwin[BMPWIN_TALK], CONFIG_GetWindowType(wk->config) );
				wk->msg_index = Stage_EasyMsg( wk, msg_stage_room_wait, FONT_TALK );
#endif

				wk->decide_csr_pos = wk->csr_pos;
				wk->send_req = 1;
				return TRUE;
			}
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//メニュー処理
	case SEQ_SUB_MENU_MAIN:
		ret = BmpMenuMain( wk->mw );

		switch( ret ){

		case BMPMENU_NULL:
			break;

		//「はい」
		case 0:
			Stage_SeqSubMenuWinClear( wk );

			//ブレーンが登場する戦闘回数で、まだ登場メッセージを表示していなかったら
			if( (Stage_BtlCountLeaderCheck(wk) == TRUE) &&
				(Stage_BrainAppearCheck(wk) == FALSE) ){

				//全てのタイプがLV10かチェックしてフラグをセット
				//StageMasterFlagSet( wk );

				wk->brain_before_csr_pos = wk->csr_pos;		//ブレーンを選ぶ前のカーソル位置
				wk->sub_seq = SEQ_SUB_BRAIN_MSG;
			}else{
				Snd_SeStopBySeqNo( SEQ_SE_DP_SELECT, 0 );
				Snd_SePlay( SEQ_SE_DP_UG_020 );		//決定SE

				if( wk->brain_before_csr_pos != STAGE_BRAIN_CSR_POS_NONE ){
					wk->csr_pos = wk->brain_before_csr_pos;	//ブレーンを選ぶ前のカーソル位置に戻す
				}
				return TRUE;						//注意！]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
			}
			break;
			
		//「いいえ」
		case 1:
		case BMPMENU_CANCEL:
			Stage_SeqSubMenuWinClear( wk );
			Stage_SeqSubTypeSelNo( wk );
			wk->sub_seq = SEQ_SUB_TYPE_SEL;
			break;
			//return TRUE;						//注意！]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
		};

		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//ブレーン登場メッセージ
	case SEQ_SUB_BRAIN_MSG:
		//会話ウィンドウ表示
		StageTalkWinPut( &wk->bmpwin[BMPWIN_TALK], CONFIG_GetWindowType(wk->config) );
		//wk->msg_index = Stage_EasyMsg( wk, msg_stage_room_boss_01, FONT_SYSTEM );
		wk->msg_index = StageWriteMsg(	wk, &wk->bmpwin[BMPWIN_TALK], msg_stage_room_boss_01, 
										1, 1, 
										CONFIG_GetMsgPrintSpeed(SaveData_GetConfig(wk->sv)),
										//MSG_ALLPUT, 
										FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_WHITE, FONT_TALK );
		GF_BGL_BmpWinOn( &wk->bmpwin[BMPWIN_TALK] );
		wk->sub_seq = SEQ_SUB_BRAIN_MSG_WAIT;
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//ブレーン登場メッセージ待ち
	case SEQ_SUB_BRAIN_MSG_WAIT:
		if( GF_MSG_PrintEndCheck(wk->msg_index) == 0 ){
			wk->brain_appear = 1;
			Stage_SeqSubWinRet( wk );

			//アニメ変更
			StageObj_AnmChg( wk->p_csr, ANM_CSR_MOVE );

			//???スクリーンのパレットを元に戻す
			ScrnPalChg( wk->bgl, (TYPE_XY_MAX-1), NORMAL_PAL_CHG );

			//タイプのパレット番号を変える
			GameStartTypeScrnPalChg( wk, wk->bgl );

			//カーソル位置を???に合わせる
			wk->csr_pos = (TYPE_XY_MAX - 1);
			StageObj_SetObjPos( wk->p_csr, GetCsrX(wk), GetCsrY(wk) );

			wk->sub_seq = SEQ_SUB_START;
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	case SEQ_SUB_PSD_CALL:
		//フェード終了待ち
		if( WIPE_SYS_EndCheck() == TRUE ){
			Stage_SetPSD( wk );
			StageCommon_Delete( wk );			//削除処理
			wk->child_proc = PROC_Create( &PokeStatusProcData, wk->psd, HEAPID_STAGE );
			//p_proc = (PROC_DATA*)wk->child_proc;
			wk->recover = 1;
			//wk->sub_seq++;
			return TRUE;						//注意！]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
		}
		break;

	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	送受信
 *
 * @param	wk		STAGE_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
enum{
	SEQ_SEND_RECV_START = 0,
	SEQ_SEND_RECV_WAIT,
	SEQ_SEND_RECV_DECIDE_MINE,
	SEQ_SEND_RECV_DECIDE_PAIR,
	SEQ_SEND_RECV_DECIDE_PAIR2,
	SEQ_SEND_RECV_MINE_MENU_WAIT,
	SEQ_SEND_RECV_MINE_MENU_YES,
	SEQ_SEND_RECV_MINE_MENU_NO,
	SEQ_SEND_RECV_MINE_MENU_DECIDE,
	SEQ_SEND_RECV_MINE_MENU_DECIDE_WAIT,
};

static BOOL Seq_GameSendRecv( STAGE_WORK* wk )
{
	u8 get_csr_pos;
	u32 ret;

	switch( wk->sub_seq ){

	///////////////////////////////////////////
	//選んだポケモンタイプを送信
	///////////////////////////////////////////
	case SEQ_SEND_RECV_START:
		get_csr_pos = Stage_GetCsrPos( wk->decide_csr_pos );
		if(Stage_CommSetSendBuf(wk,STAGE_COMM_PAIR_POKETYPE,get_csr_pos) ==	TRUE ){

#if 0		//08.03.26　移動
			//会話ウィンドウ表示
			StageObj_Vanish( wk->p_icon, STAGE_VANISH_ON );	//非表示
			StageTalkWinPut( &wk->bmpwin[BMPWIN_TALK], CONFIG_GetWindowType(wk->config) );
			Frontier_PairNameWordSet( wk->wordset, 0 );
			wk->msg_index = Stage_EasyMsg( wk, msg_stage_room_10, FONT_TALK );
			wk->wait = STAGE_MSG_WAIT;
#else
			wk->wait = 0;
#endif

			wk->decide_csr_pos = 0;
			wk->send_req = 0;
			wk->sub_seq = SEQ_SEND_RECV_WAIT;
		}
		break;

	///////////////////////////////////////////
	//受信待ち
	///////////////////////////////////////////
	case SEQ_SEND_RECV_WAIT:
		if( wk->wait > 0 ){
			wk->wait--;
			break;
		}

		//親の決定が先の時のみ、parent_decide_posに値が先に入ってしまうので、
		//両方が受信したら先に進むようにする
		if( wk->typesel_recieve_count < STAGE_COMM_PLAYER_NUM ){
		//if( wk->parent_decide_pos == STAGE_DECIDE_NONE ){
			break;
		}

		wk->typesel_recieve_count = 0;

		//親の決定が有効
		if( wk->parent_decide_pos < TYPE_XY_MAX ){
			if( CommGetCurrentID() == COMM_PARENT_ID ){
				wk->sub_seq = SEQ_SEND_RECV_DECIDE_MINE;
			}else{
				wk->sub_seq = SEQ_SEND_RECV_DECIDE_PAIR;
			}

		//子の決定が有効
		}else{
			if( CommGetCurrentID() == COMM_PARENT_ID ){
				wk->sub_seq = SEQ_SEND_RECV_DECIDE_PAIR;
			}else{
				wk->sub_seq = SEQ_SEND_RECV_DECIDE_MINE;
			}
		}
		break;

	///////////////////////////////////////////
	//相手が決定(準備)
	///////////////////////////////////////////
	case SEQ_SEND_RECV_DECIDE_PAIR:

#if 1	//08.03.26　移動
		//会話ウィンドウ表示
		StageObj_Vanish( wk->p_icon, STAGE_VANISH_ON );	//非表示
		StageTalkWinPut( &wk->bmpwin[BMPWIN_TALK], CONFIG_GetWindowType(wk->config) );
		Frontier_PairNameWordSet( wk->wordset, 0 );
		wk->msg_index = Stage_EasyMsg( wk, msg_stage_room_10, FONT_TALK );
		wk->wait = STAGE_MSG_WAIT;
#endif

#if 1
		//指定フレームのスクリーンの指定位置のパレットを変更する
		ScrnPalChg( wk->bgl, wk->pair_csr_pos, PAIR_PAL_CHG );
		GF_BGL_LoadScreenV_Req( wk->bgl, BS_FRAME_BG );	//スクリーンデータ転送

		//会話ウィンドウ表示
		//StageObj_Vanish( wk->p_icon, STAGE_VANISH_ON );	//非表示
		//StageTalkWinPut( &wk->bmpwin[BMPWIN_TALK], CONFIG_GetWindowType(wk->config) );

		//「○さんが選んでいます　しばらくお待ちください」
		//Frontier_PairNameWordSet( wk->wordset, 0 );
		//wk->msg_index = Stage_EasyMsg( wk, msg_stage_room_10, FONT_TALK );
#endif
		wk->sub_seq = SEQ_SEND_RECV_DECIDE_PAIR2;
		break;

	///////////////////////////////////////////
	//相手が決定(待ち)
	///////////////////////////////////////////
	case SEQ_SEND_RECV_DECIDE_PAIR2:

		//相手の最終確認を受信
		if( wk->pair_final_answer == FINAL_ANSWER_NONE ){
			break;
		}

		//タイプを最終決定
		if( wk->pair_final_answer == FINAL_ANSWER_YES ){

			//強制終了へ
			wk->csr_pos = wk->pair_csr_pos;
			return TRUE;

		//タイプをキャンセル
		}else{
			Stage_SeqSubWinRet( wk );

			//指定フレームのスクリーンの指定位置のパレットを変更する
			ScrnPalChg( wk->bgl, wk->pair_csr_pos, NORMAL_PAL_CHG );
			GF_BGL_LoadScreenV_Req( wk->bgl, BS_FRAME_BG );	//スクリーンデータ転送

			//クリアして継続
			wk->pair_csr_pos		= CSR_POS_NONE;
			wk->pair_final_answer	= FINAL_ANSWER_NONE;
			wk->parent_decide_pos	= STAGE_DECIDE_NONE;

			wk->type_sel_return_flag = 1;
			return TRUE;
		}
		break;

	///////////////////////////////////////////
	//自分が決定
	///////////////////////////////////////////
	case SEQ_SEND_RECV_DECIDE_MINE:
		//指定フレームのスクリーンの指定位置のパレットを変更する
		ScrnPalChg( wk->bgl, wk->csr_pos, DECIDE_PAL_CHG );
		GF_BGL_LoadScreenV_Req( wk->bgl, BS_FRAME_BG );	//スクリーンデータ転送
		Stage_SeqSubTypeSelYesNo( wk );
		wk->sub_seq = SEQ_SEND_RECV_MINE_MENU_WAIT;
		break;

	///////////////////////////////////////////
	//メニュー待ち
	///////////////////////////////////////////
	case SEQ_SEND_RECV_MINE_MENU_WAIT:
		ret = BmpMenuMain( wk->mw );

		switch( ret ){

		case BMPMENU_NULL:
			break;

		//「はい」
		case 0:
			Snd_SeStopBySeqNo( SEQ_SE_DP_SELECT, 0 );
			Snd_SePlay( SEQ_SE_DP_UG_020 );		//決定SE
			Stage_SeqSubMenuWinClear( wk );
			wk->sub_seq = SEQ_SEND_RECV_MINE_MENU_YES;
			break;

		//「いいえ」
		case 1:
		case BMPMENU_CANCEL:
			Stage_SeqSubMenuWinClear( wk );
			wk->sub_seq = SEQ_SEND_RECV_MINE_MENU_NO;
			break;
		};

		break;
		
	///////////////////////////////////////////
	//「はい」
	///////////////////////////////////////////
	case SEQ_SEND_RECV_MINE_MENU_YES:
		//ポケモンタイプを最終決定で送信
		if( Stage_CommSetSendBuf(wk,STAGE_COMM_PAIR_FINAL_ANSWER,FINAL_ANSWER_YES) == TRUE ){
			return TRUE;
		}
		break;

	///////////////////////////////////////////
	//「いいえ」
	///////////////////////////////////////////
	case SEQ_SEND_RECV_MINE_MENU_NO:
		//ポケモンタイプをキャンセルで送信
		if( Stage_CommSetSendBuf(wk,STAGE_COMM_PAIR_FINAL_ANSWER,FINAL_ANSWER_NO) == TRUE ){
			Stage_SeqSubTypeSelNo( wk );
			wk->pair_csr_pos = CSR_POS_NONE;
			wk->parent_decide_pos = STAGE_DECIDE_NONE;
			wk->type_sel_return_flag = 1;
			return TRUE;
		};
		break;

	///////////////////////////////////////////
	//最終決定した
	///////////////////////////////////////////
	case SEQ_SEND_RECV_MINE_MENU_DECIDE:
		//同期開始
		CommToolTempDataReset();
		CommTimingSyncStart( DBC_TIM_STAGE_END );
		wk->sub_seq = SEQ_SEND_RECV_MINE_MENU_DECIDE_WAIT;
		break;

	///////////////////////////////////////////
	//同期
	///////////////////////////////////////////
	case SEQ_SEND_RECV_MINE_MENU_DECIDE_WAIT:
		//タイミングコマンドが届いたか確認
		if( CommIsTimingSync(DBC_TIM_STAGE_END) == TRUE ){
			CommToolTempDataReset();
			CommToolInitialize( HEAPID_STAGE );	//timingSyncEnd=0xff
			wk->parent_decide_pos = STAGE_DECIDE_NONE;
			return TRUE;
		}
		break;

	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	シーケンス：マルチ終了へ
 *
 * @param	wk		STAGE_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameEndMulti( STAGE_WORK* wk )
{
	int i;

	switch( wk->sub_seq ){

	case 0:
		//同期開始
		CommToolTempDataReset();
		CommTimingSyncStart( DBC_TIM_STAGE_END );
		
		wk->sub_seq++;
		break;

	//同期待ち
	case 1:
		//タイミングコマンドが届いたか確認
		if( CommIsTimingSync(DBC_TIM_STAGE_END) == TRUE ){
			CommToolTempDataReset();
			return TRUE;
		}
		break;
	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	シーケンス：終了へ
 *
 * @param	wk		STAGE_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameEnd( STAGE_WORK* wk )
{
	int i;

	switch( wk->sub_seq ){

	case 0:
		wk->wait = STAGE_END_WAIT;
		wk->sub_seq++;
		break;

	//フェードアウト
	case 1:
		wk->wait--;
		if( wk->wait == 0 ){
			Stage_SeqSubWinRet( wk );
			WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
					WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_STAGE );
			wk->sub_seq++;
		}
		break;

	//フェード終了待ち
	case 2:
		if( WIPE_SYS_EndCheck() == TRUE ){
			return TRUE;
		}
		break;
	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	シーケンス：パートナーがポケモンタイプを決定したので強制終了へ
 *
 * @param	wk		STAGE_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GamePairEnd( STAGE_WORK* wk )
{
	int i;

	switch( wk->sub_seq ){

	case 0:
		Stage_SeqSubMenuWinClear( wk );
		StageObj_Vanish( wk->p_icon, STAGE_VANISH_OFF );	//表示
		BmpTalkWinClear( &wk->bmpwin[BMPWIN_TALK], WINDOW_TRANS_ON );

		//wk->wait_count = STAGE_PAIR_END_WAIT;
		wk->sub_seq++;
		break;

	case 1:
		//少しウェイトを入れる
		//wk->wait_count--;
		//if( wk->wait_count == 0 ){
			return TRUE;
		//}
		break;
	};

	return FALSE;
}


//==============================================================================================
//
//	共通処理
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	共通 削除
 *
 * @param	wk		STAGE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void StageCommon_Delete( STAGE_WORK* wk )
{
	int i;

	if( wk->p_csr != NULL ){
		StageObj_Delete( wk->p_csr );
	}

	if( wk->p_icon != NULL ){
		StageObj_Delete( wk->p_icon );
	}

	//通信アイコン削除
	WirelessIconEasyEnd();

	//パレットフェード開放
	PaletteFadeWorkAllocFree( wk->pfd, FADE_MAIN_OBJ );
	PaletteFadeWorkAllocFree( wk->pfd, FADE_MAIN_BG );

	//パレットフェードシステム開放
	PaletteFadeFree( wk->pfd );
	wk->pfd = NULL;

	StageClact_DeleteCellObject(&wk->stage_clact);		//2Dオブジェクト関連領域開放

	MSGMAN_Delete( wk->msgman );						//メッセージマネージャ開放
	WORDSET_Delete( wk->wordset );
	STRBUF_Delete( wk->msg_buf );						//メッセージバッファ開放
	STRBUF_Delete( wk->tmp_buf );						//メッセージバッファ開放
	NUMFONT_Delete( wk->num_font );

	for( i=0; i < STAGE_MENU_BUF_MAX ;i++ ){
		STRBUF_Delete( wk->menu_buf[i] );				//メニューバッファ開放
	}

	StageExitBmpWin( wk->bmpwin );						//BMPウィンドウ開放
	Stage_BgExit( wk->bgl );							//BGL削除
	ArchiveDataHandleClose( wk->hdl );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	復帰
 *
 * @param	wk		STAGE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Stage_Recover( STAGE_WORK* wk )
{
	int i;

	Stage_InitSub1();

	wk->bgl	= GF_BGL_BglIniAlloc( HEAPID_STAGE );

	Stage_InitSub2( wk );

	//現在のカーソル位置がダミータイプかでアニメを変更する
	if( Stage_GetPokeType(wk->csr_pos) == DUMMY_TYPE ){
		StageObj_AnmChg( wk->p_csr, ANM_CSR_POKE );			//アニメ変更
		StageObj_SetObjPos( wk->p_csr, CSR_POKE_X, CSR_POKE_Y );
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	共通 初期化1
 *
 * @param	wk		STAGE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Stage_InitSub1( void )
{
	sys_VBlankFuncChange( NULL, NULL );					//VBlankセット
	sys_HBlankIntrSet( NULL,NULL );						//HBlankセット
	GF_Disp_GX_VisibleControlInit();
	GF_Disp_GXS_VisibleControlInit();
	GX_SetVisiblePlane( 0 );
	GXS_SetVisiblePlane( 0 );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	共通 初期化2
 *
 * @param	wk		STAGE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Stage_InitSub2( STAGE_WORK* wk )
{
	int i,flip;

	wk->hdl = ArchiveDataHandleOpen( ARC_FRONTIER_BG, HEAPID_STAGE );
	Stage_BgInit( wk );								//BG初期化
	Stage_ObjInit( wk );								//OBJ初期化

	//メッセージデータマネージャー作成
	wk->msgman = MSGMAN_Create( MSGMAN_TYPE_DIRECT, ARC_MSG, 
								NARC_msg_stage_room_dat, HEAPID_STAGE );

	wk->wordset = WORDSET_Create( HEAPID_STAGE );
	wk->msg_buf = STRBUF_Create( STAGE_MSG_BUF_SIZE, HEAPID_STAGE );
	wk->tmp_buf = STRBUF_Create( STAGE_MSG_BUF_SIZE, HEAPID_STAGE );

	//文字列バッファ作成
	for( i=0; i < STAGE_MENU_BUF_MAX ;i++ ){
		wk->menu_buf[i] = STRBUF_Create( STAGE_MENU_BUF_SIZE, HEAPID_STAGE );
	}

	//フォントパレット
	SystemFontPaletteLoad( PALTYPE_MAIN_BG, BS_FONT_PAL * 32, HEAPID_STAGE );
	TalkFontPaletteLoad( PALTYPE_MAIN_BG, BS_MSGFONT_PAL * 32, HEAPID_STAGE );

	//8x8フォント作成(使用bmpwin[BMPWIN_TYPE]のパレット(BS_FONT_PAL)を使用
	wk->num_font = NUMFONT_Create( 15, 14, FBMP_COL_NULL, HEAPID_STAGE );

	//ビットマップ追加
	StageAddBmpWin( wk->bgl, wk->bmpwin );

	//タイプのパレット番号を変える
	GameStartTypeScrnPalChg( wk, wk->bgl );

	GF_Disp_DispOn();

	//カーソルOBJ追加
	wk->p_csr = StageObj_Create(	&wk->stage_clact, STAGE_ID_OBJ_CSR, ANM_CSR_MOVE, 
									GetCsrX(wk), GetCsrY(wk), NULL );

	//アイコンOBJ追加
	wk->p_icon = StageObj_Create(	&wk->stage_clact, STAGE_ID_OBJ_ICON, POKEICON_ANM_HPMAX,
									ICON_X, ICON_Y, NULL );

	//パレット切り替え
	StageObj_IconPalChg( wk->p_icon, PokeParty_GetMemberPointer(wk->p_party,0) );

	//通信アイコンセット
	if( CommIsInitialize() ){
//CLACT_U_WmIcon_SetReserveAreaCharManager( NNS_G2D_VRAM_TYPE_2DMAIN,GX_OBJVRAMMODE_CHAR_1D_64K );
		CLACT_U_WmIcon_SetReserveAreaCharManager( NNS_G2D_VRAM_TYPE_2DMAIN,
												  GX_OBJVRAMMODE_CHAR_1D_32K );
		CLACT_U_WmIcon_SetReserveAreaPlttManager( NNS_G2D_VRAM_TYPE_2DMAIN );
		WirelessIconEasy();
	}

	sys_VBlankFuncChange( VBlankFunc, (void*)wk );		//VBlankセット
	return;
}


//==============================================================================================
//
//	共通初期化、終了
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	BG関連初期化
 *
 * @param	wk		STAGE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Stage_BgInit( STAGE_WORK* wk )
{
	SetVramBank();
	SetBgHeader( wk->bgl );

	//パレットフェードシステムワーク作成
	wk->pfd = PaletteFadeInit( HEAPID_STAGE );

	//リクエストデータをmallocしてセット
	PaletteFadeWorkAllocSet( wk->pfd, FADE_MAIN_OBJ, FADE_PAL_ALL_SIZE, HEAPID_STAGE );
	PaletteFadeWorkAllocSet( wk->pfd, FADE_MAIN_BG, FADE_PAL_ALL_SIZE, HEAPID_STAGE );

	//上画面背景
	Stage_SetMainBgGraphic( wk, BS_FRAME_BG );
	Stage_SetMainBgPalette();
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );		//BG非表示

	//下画面背景、パレットセット
	Stage_SetSubBgGraphic( wk, BS_FRAME_SUB );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	OBJ関連初期化
 *
 * @param	wk		STAGE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Stage_ObjInit( STAGE_WORK* wk )
{
	StageClact_InitCellActor(	&wk->stage_clact, 
								PokeParty_GetMemberPointer(wk->p_party,0) );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	BG解放
 *
 * @param	ini		BGLデータ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Stage_BgExit( GF_BGL_INI * ini )
{
	//メイン画面の各面の表示コントロール(表示OFF)
	GF_Disp_GX_VisibleControl(	GX_PLANEMASK_BG0 | 
								GX_PLANEMASK_BG1 | 
								GX_PLANEMASK_BG2 |
								GX_PLANEMASK_BG3 | 
								GX_PLANEMASK_OBJ, 
								VISIBLE_OFF );

	//サブ画面の各面の表示コントロール(表示OFF)
	GF_Disp_GXS_VisibleControl(	GX_PLANEMASK_BG0 | 
								GX_PLANEMASK_BG1 | 
								GX_PLANEMASK_BG2 | 
								GX_PLANEMASK_BG3 | 
								GX_PLANEMASK_OBJ, 
								VISIBLE_OFF );

	//GF_BGL_BGControlSetで取得したメモリを開放
	GF_BGL_BGControlExit( ini, BS_FRAME_BG );			//3
	GF_BGL_BGControlExit( ini, BS_FRAME_TYPE );
	GF_BGL_BGControlExit( ini, BS_FRAME_WIN );
	GF_BGL_BGControlExit( ini, BS_FRAME_SUB );

	sys_FreeMemoryEz( ini );
	return;
}


//==============================================================================================
//
//	設定
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	VBlank関数
 *
 * @param	work	ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------
static void VBlankFunc( void * work )
{
	STAGE_WORK* wk = work;

	//ポケモンステータス表示中
	if( wk->child_proc != NULL ){
		return;
	}

	//パレット転送
	if( wk->pfd != NULL ){
		PaletteFadeTrans( wk->pfd );
	}

	GF_BGL_VBlankFunc( wk->bgl );

	//セルアクター
	//Vram転送マネージャー実行
	DoVramTransferManager();

	//レンダラ共有OAMマネージャVram転送
	REND_OAMTrans();	

	OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}

//--------------------------------------------------------------
/**
 * @brief	VRAM設定
 *
 * @param	none
 *
 * @return	none
 *
 * 細かく設定していないので注意！
 */
//--------------------------------------------------------------
static void SetVramBank(void)
{
	GF_BGL_DISPVRAM tbl = {
		GX_VRAM_BG_128_C,				//メイン2DエンジンのBG
		GX_VRAM_BGEXTPLTT_NONE,			//メイン2DエンジンのBG拡張パレット

		GX_VRAM_SUB_BG_32_H,			//サブ2DエンジンのBG
		GX_VRAM_SUB_BGEXTPLTT_NONE,		//サブ2DエンジンのBG拡張パレット

		GX_VRAM_OBJ_64_E,				//メイン2DエンジンのOBJ
		GX_VRAM_OBJEXTPLTT_NONE,		//メイン2DエンジンのOBJ拡張パレット

		GX_VRAM_SUB_OBJ_16_I,			//サブ2DエンジンのOBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	//サブ2DエンジンのOBJ拡張パレット

		GX_VRAM_TEX_01_AB,				//テクスチャイメージスロット
		GX_VRAM_TEXPLTT_01_FG			//テクスチャパレットスロット
	};

	GF_Disp_SetBank( &tbl );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	BG設定
 *
 * @param	init	BGLデータ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void SetBgHeader( GF_BGL_INI * ini )
{
	{	//BG SYSTEM
		GF_BGL_SYS_HEADER BGsys_data = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
		};
		GF_BGL_InitBG( &BGsys_data );
	}

	{	//BG(フォント)(会話、メニュー)
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, BS_FRAME_WIN, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ClearCharSet( BS_FRAME_WIN, 32, 0, HEAPID_STAGE );
		GF_BGL_ScrClear( ini, BS_FRAME_WIN );
	}

	{	//BG(フォント)(タイプ表示)
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			//GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x0c000, GX_BG_EXTPLTT_01,
			GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x0c000, GX_BG_EXTPLTT_01,
			1, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, BS_FRAME_TYPE, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ClearCharSet( BS_FRAME_TYPE, 32, 0, HEAPID_STAGE );
		GF_BGL_ScrClear( ini, BS_FRAME_TYPE );
	}

	{	//上画面(背景)
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x08000, GX_BG_EXTPLTT_01,
			2, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, BS_FRAME_BG, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear( ini, BS_FRAME_BG );
	}

	//--------------------------------------------------------------------------------
	{	//下画面(ボール)
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x3000, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, BS_FRAME_SUB, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear( ini, BS_FRAME_SUB );
	}

	G2_SetBG0Priority( 0 );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
	return;
}


//==============================================================================================
//
//	BGグラフィックデータ
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	グラフィックデータセット：上画面背景
 *
 * @param	wk		STAGE_WORK型のポインタ
 * @param	frm		フレームナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Stage_SetMainBgGraphic( STAGE_WORK * wk, u32 frm  )
{
	ArcUtil_HDL_BgCharSet(	wk->hdl, BS_SELECT_NCGR_BIN, 
							wk->bgl, frm, 0, 0, 1, HEAPID_STAGE );

	ArcUtil_HDL_ScrnSet(wk->hdl, BS_SELECT_NSCR_BIN, 
						wk->bgl, frm, 0, 0, 1, HEAPID_STAGE );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	上画面背景パレット設定
 *
 * @param	none
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Stage_SetMainBgPalette( void )
{
	void *buf;
	NNSG2dPaletteData *dat;

	buf = ArcUtil_PalDataGet( ARC_FRONTIER_BG, BS_SELECT_NCLR, &dat, HEAPID_STAGE );

	DC_FlushRange( dat->pRawData, (sizeof(u16)*16*6) );
	GX_LoadBGPltt( dat->pRawData, 0, (sizeof(u16)*16*6) );		//メイン

	sys_FreeMemoryEz(buf);
	return;
}

//--------------------------------------------------------------
/**
 * @brief	グラフィックデータセット：下画面背景、パレットセット
 *
 * @param	wk		STAGE_WORK型のポインタ
 * @param	frm		フレームナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Stage_SetSubBgGraphic( STAGE_WORK * wk, u32 frm  )
{
	ArcUtil_HDL_BgCharSet(	wk->hdl, BATT_FRONTIER_NCGR_BIN, wk->bgl, frm,
							0, 0, TRUE, HEAPID_STAGE );

	ArcUtil_HDL_ScrnSet(wk->hdl, BATT_FRONTIER_NSCR_BIN, wk->bgl, frm,
						0, 0, TRUE, HEAPID_STAGE );

	ArcUtil_HDL_PalSet(	wk->hdl, BATT_FRONTIER_NCLR, PALTYPE_SUB_BG,
						0, 0x20, HEAPID_STAGE );
	return;
}


//==============================================================================================
//
//	メッセージ関連
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	メッセージ表示
 *
 * @param	wk		STAGE_WORK型のポインタ
 * @param	win		ビットマップウィンドウ
 * @param	msg_id	メッセージID
 * @param	x		X座標(ドット単位)
 * @param	y		Y座標(ドット単位)
 * @param	wait	文字表示ウェイト
 * @param	f_col	文字色ナンバー(塗りつぶしカラーコード)
 * @param	s_col	影色ナンバー
 * @param	b_col	背景色ナンバー
 * @param	font	フォント種類
 *
 * @return	"文字描画ルーチンのインデックス"
 */
//--------------------------------------------------------------
static u8 StageWriteMsg( STAGE_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font )
{
	GF_BGL_BmpWinDataFill( win, b_col );			//塗りつぶし
	MSGMAN_GetString( wk->msgman, msg_id, wk->tmp_buf );

	//登録された単語を使って文字列展開する
	WORDSET_ExpandStr( wk->wordset, wk->msg_buf, wk->tmp_buf );

	return GF_STR_PrintColor( win, font, wk->msg_buf, x, y, wait, 
								GF_PRINTCOLOR_MAKE(f_col,s_col,b_col), NULL );
}

//--------------------------------------------------------------
/**
 * @brief	メッセージ表示(塗りつぶしなし)
 *
 * @param	wk		STAGE_WORK型のポインタ
 * @param	win		ビットマップウィンドウ
 * @param	msg_id	メッセージID
 * @param	x		X座標(ドット単位)
 * @param	y		Y座標(ドット単位)
 * @param	wait	文字表示ウェイト
 * @param	f_col	文字色ナンバー(塗りつぶしカラーコード)
 * @param	s_col	影色ナンバー
 * @param	b_col	背景色ナンバー
 * @param	font	フォント種類
 *
 * @return	"文字描画ルーチンのインデックス"
 *
 * 塗りつぶしなし
 */
//--------------------------------------------------------------
static u8 StageWriteMsgSimple( STAGE_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font )
{
	MSGMAN_GetString( wk->msgman, msg_id, wk->tmp_buf );

	//登録された単語を使って文字列展開する
	WORDSET_ExpandStr( wk->wordset, wk->msg_buf, wk->tmp_buf );

	return GF_STR_PrintColor( win, font, wk->msg_buf, x, y, wait, 
								GF_PRINTCOLOR_MAKE(f_col,s_col,b_col), NULL );
}

//--------------------------------------------------------------
/**
 * @brief	簡単メッセージ表示
 *
 * @param	wk		STAGE_WORK型のポインタ
 * @param	msg_id	メッセージID
 *
 * @return	"msg_index"
 */
//--------------------------------------------------------------
static u8 Stage_EasyMsg( STAGE_WORK* wk, int msg_id, u8 font )
{
	u8 msg_index;

	msg_index = StageWriteMsg(wk, &wk->bmpwin[BMPWIN_TALK], msg_id, 
								1, 1, MSG_ALLPUT, 
								FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_WHITE, font );

	GF_BGL_BmpWinOn( &wk->bmpwin[BMPWIN_TALK] );

	return msg_index;
}


//==============================================================================================
//
//	メニュー関連
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	BMPメニュー	ヘッダー初期設定
 *
 * @param	wk		STAGE_WORK型のポインタ
 * @param	win		GF_BGL_BMPWIN型のポインタ
 * @param	y_max	項目最大数
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void StageInitMenu( STAGE_WORK* wk, GF_BGL_BMPWIN* win, u8 y_max )
{
	int i;

	for( i=0; i < STAGE_MENU_BUF_MAX ;i++ ){
		wk->Data[i].str	  = NULL;
		wk->Data[i].param = 0;
	}

	wk->MenuH.menu		= wk->Data;
	wk->MenuH.win		= win;
	wk->MenuH.font		= BS_FONT;
	wk->MenuH.x_max		= 1;
	wk->MenuH.y_max		= y_max;
	wk->MenuH.line_spc	= 0;
	wk->MenuH.c_disp_f	= 0;
	//wk->MenuH.c_disp_f	= 1;			//カーソルなし
	//wk->MenuH.loop_f	= 0;			//ループ無し
	wk->MenuH.loop_f	= 1;			//ループ有り
	return;
}

//--------------------------------------------------------------
/**
 * @brief	BMPメニュー	データセット
 *
 * @param	wk		STAGE_WORK型のポインタ
 * @param	no		セットする場所(戻り値)
 * @param	param	戻り値
 * @param	msg_id	メッセージID
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void StageSetMenuData( STAGE_WORK* wk, u8 no, u8 param, int msg_id )
{
	int i;
	void* msg;

	SDK_ASSERTMSG( no < STAGE_MENU_BUF_MAX, "メニュー項目数オーバー！" );

	MSGMAN_GetString( wk->msgman, msg_id, wk->menu_buf[no] );

	wk->Data[ no ].str = (const void *)wk->menu_buf[no];
	wk->Data[ no ].param = param;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	共通 メニューセット2「はい、いいえ」
 *
 * @param	wk		STAGE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Stage_SetMenu2( STAGE_WORK* wk )
{
	wk->menu_flag = 1;
	StageWriteMenuWin( wk->bgl, &wk->bmpwin[BMPWIN_YESNO] );
	StageInitMenu( wk, &wk->bmpwin[BMPWIN_YESNO], 2 );
	StageSetMenuData( wk, 0, 0, msg_stage_room_choice_05 );				//はい
	StageSetMenuData( wk, 1, 1, msg_stage_room_choice_06 );				//いいえ
	wk->mw = BmpMenuAddEx( &wk->MenuH, 8, 0, 0, HEAPID_STAGE, PAD_BUTTON_CANCEL );
	return;
}


//==============================================================================================
//
//	文字列セット
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	数値をセット
 *
 * @param	wk		STAGE_WORK型のポインタ
 * @param	bufID	バッファID
 * @param	number	セットする数値
 *
 * @retval	none
 *
 * 桁を固定にしている
 */
//--------------------------------------------------------------
static void Stage_SetNumber( STAGE_WORK* wk, u32 bufID, s32 number )
{
	WORDSET_RegisterNumber( wk->wordset, bufID, number, 2, 
							NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	ポケモン名をセット
 *
 * @param	wk		STAGE_WORK型のポインタ
 * @param	bufID	バッファID
 * @param	ppp		POKEMON_PASO_PARAM
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void Stage_SetPokeName( STAGE_WORK* wk, u32 bufID, POKEMON_PASO_PARAM* ppp )
{
	WORDSET_RegisterPokeMonsName( wk->wordset, bufID, ppp );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	プレイヤー名をセット
 *
 * @param	wk		STAGE_WORK型のポインタ
 * @param	bufID	バッファID
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void Stage_SetPlayerName( STAGE_WORK* wk, u32 bufID )
{
	WORDSET_RegisterPlayerName( wk->wordset, bufID, SaveData_GetMyStatus(wk->sv) );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	プレイヤー名を表示
 *
 * @param	wk		STAGE_WORK型のポインタ
 * @param	win		ビットマップウィンドウ
 * @param	x		X座標(ドット単位)
 * @param	y		Y座標(ドット単位)
 * @param	f_col	文字色ナンバー(塗りつぶしカラーコード)
 * @param	s_col	影色ナンバー
 * @param	b_col	背景色ナンバー
 * @param	font	フォント種類
 *
 * @return	none
 */
//--------------------------------------------------------------
static void PlayerNameWrite( STAGE_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 font )
{
	u32 col;
	const MYSTATUS* my;
	STRBUF* player_buf;								//プレイヤー名バッファポインタ

	my = SaveData_GetMyStatus( wk->sv );
	player_buf = STRBUF_Create( PLAYER_NAME_BUF_SIZE, HEAPID_STAGE );
	
	GF_BGL_BmpWinDataFill( win, FBMP_COL_NULL );			//塗りつぶし

	STRBUF_SetStringCode( player_buf, MyStatus_GetMyName(my) );

	if( MyStatus_GetMySex(my) == PM_MALE ){
		col = COL_BLUE;
	}else{
		col = COL_RED;
	}

	GF_STR_PrintColor( win, font, player_buf, x, y, MSG_ALLPUT, col, NULL );

	STRBUF_Delete( player_buf );					//メッセージバッファ開放

	GF_BGL_BmpWinOn( win );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	パートナー名を表示
 *
 * @param	wk		STAGE_WORK型のポインタ
 * @param	win		ビットマップウィンドウ
 * @param	x		X座標(ドット単位)
 * @param	y		Y座標(ドット単位)
 * @param	f_col	文字色ナンバー(塗りつぶしカラーコード)
 * @param	s_col	影色ナンバー
 * @param	b_col	背景色ナンバー
 * @param	font	フォント種類
 *
 * @return	none
 */
//--------------------------------------------------------------
static void PairNameWrite( STAGE_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 font )
{
	u32 col;
	STRBUF* player_buf;								//プレイヤー名バッファポインタ
	MYSTATUS* my;									//パートナーのMyStatus

	//パートナーのMyStatusを取得
	my = CommInfoGetMyStatus( 1 - CommGetCurrentID() );

	player_buf = STRBUF_Create( PLAYER_NAME_BUF_SIZE, HEAPID_STAGE );
	
	GF_BGL_BmpWinDataFill( win, FBMP_COL_NULL );			//塗りつぶし

	//名前取得(STRBUFにコピー)
	MyStatus_CopyNameString( my, player_buf );

	if( MyStatus_GetMySex(my) == PM_MALE ){
		col = COL_BLUE;
	}else{
		col = COL_RED;
	}

	GF_STR_PrintColor( win, font, player_buf, x, y, MSG_ALLPUT, col, NULL );

	STRBUF_Delete( player_buf );					//メッセージバッファ開放

	GF_BGL_BmpWinOn( win );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	ポケモン名を表示
 *
 * @param	wk		STAGE_WORK型のポインタ
 * @param	win		ビットマップウィンドウ
 * @param	x		X座標(ドット単位)
 * @param	y		Y座標(ドット単位)
 * @param	f_col	文字色ナンバー(塗りつぶしカラーコード)
 * @param	s_col	影色ナンバー
 * @param	b_col	背景色ナンバー
 * @param	font	フォント種類
 *
 * @return	none
 */
//--------------------------------------------------------------
static void PokeNameWrite( STAGE_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 f_col, u8 s_col, u8 b_col, u8 font )
{
	u8 x_pos;
	u32 sex,msg_id,col;
	STRBUF* buf;
	POKEMON_PARAM* poke;
	STRCODE sel_poke_buf[POKE_NAME_BUF_SIZE];					//ポケモン名バッファポインタ

	poke =  PokeParty_GetMemberPointer( wk->p_party, 0 );
	PokeParaGet( poke, ID_PARA_default_name, sel_poke_buf );	//ポケモンのデフォルト名を取得

	GF_BGL_BmpWinDataFill( win, b_col );						//塗りつぶし

	buf = STRBUF_Create( POKE_NAME_BUF_SIZE, HEAPID_STAGE );	//STRBUF生成
	STRBUF_SetStringCode( buf, sel_poke_buf );					//STRCODE→STRBUF

	GF_STR_PrintColor( win, font, buf, x, y, MSG_ALLPUT, 
								GF_PRINTCOLOR_MAKE(f_col,s_col,b_col), NULL );

	STRBUF_Delete( buf );

	GF_BGL_BmpWinOn( win );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	ポケモン名を表示(性別指定)
 *
 * @param	wk		STAGE_WORK型のポインタ
 * @param	win		ビットマップウィンドウ
 * @param	x		X座標(ドット単位)
 * @param	y		Y座標(ドット単位)
 * @param	f_col	文字色ナンバー(塗りつぶしカラーコード)
 * @param	s_col	影色ナンバー
 * @param	b_col	背景色ナンバー
 * @param	font	フォント種類
 * @param	monsno	モンスターナンバー
 * @param	sex		性別
 *
 * @return	none
 */
//--------------------------------------------------------------
static void PokeNameWriteEx( STAGE_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 f_col, u8 s_col, u8 b_col, u8 font, u16 monsno, u8 sex )
{
	u8 x_pos;
	u32 msg_id,col;
	MSGDATA_MANAGER* man;
	STRBUF* buf;
	POKEMON_PARAM* poke;
	STRCODE sel_poke_buf[POKE_NAME_BUF_SIZE];					//ポケモン名バッファポインタ

	GF_BGL_BmpWinDataFill( win, b_col );						//塗りつぶし

	//ポケモン名を取得
	man = MSGMAN_Create( MSGMAN_TYPE_DIRECT, ARC_MSG, NARC_msg_monsname_dat, HEAPID_STAGE );
	buf = MSGMAN_AllocString( man, monsno );
	MSGMAN_Delete( man );

	GF_STR_PrintColor( win, font, buf, x, y, MSG_ALLPUT, 
								GF_PRINTCOLOR_MAKE(f_col,s_col,b_col), NULL );

	//右端に性別コードを表示
	x_pos = GF_BGL_BmpWinGet_SizeX(win) - 1;

	msg_id = (sex == PARA_MALE) ? msg_stage_male : msg_stage_female;
	col = (sex == PARA_MALE) ? COL_BLUE : COL_RED;

	STRBUF_Clear( buf );
	MSGMAN_GetString( wk->msgman, msg_id, buf );
	GF_STR_PrintColor( win, font, buf, x_pos*8, y, MSG_ALLPUT, col, NULL );

	STRBUF_Delete( buf );

	GF_BGL_BmpWinOn( win );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	タイプを表示
 *
 * @param	wk		STAGE_WORK型のポインタ
 * @param	win		ビットマップウィンドウ
 * @param	f_col	文字色ナンバー(塗りつぶしカラーコード)
 * @param	s_col	影色ナンバー
 * @param	b_col	背景色ナンバー
 * @param	font	フォント種類
 *
 * @return	none
 */
//--------------------------------------------------------------
static void TypeWrite( STAGE_WORK* wk, GF_BGL_BMPWIN* win, u8 f_col, u8 s_col, u8 b_col, u8 font )
{
	int i,j;
	STRBUF* buf;
	MSGDATA_MANAGER* man;

	GF_BGL_BmpWinDataFill( win, b_col );						//塗りつぶし

	man = MSGMAN_Create( MSGMAN_TYPE_DIRECT, ARC_MSG, NARC_msg_typename_dat, HEAPID_STAGE );
	buf = STRBUF_Create( POKE_NAME_BUF_SIZE, HEAPID_STAGE );	//STRBUF生成

	for( i=0; i < TYPE_Y_MAX ;i++ ){
		for( j=0; j < TYPE_X_MAX ;j++ ){

			//ダミータイプは無視する
			if( Stage_GetPokeType(i*TYPE_X_MAX+j) != DUMMY_TYPE ){

				STRBUF_Clear( buf );
				MSGMAN_GetString( man, Stage_GetPokeType(i*TYPE_X_MAX+j), buf );
				GF_STR_PrintColor(	win, font, buf, 
									TYPE_START_X + (TYPE_X * j), 
									TYPE_START_Y + (TYPE_Y * i), 
									MSG_ALLPUT, 
									GF_PRINTCOLOR_MAKE(f_col,s_col,b_col), NULL );
			}
		}
	}

	STRBUF_Delete( buf );
	MSGMAN_Delete(man);
	GF_BGL_BmpWinOn( win );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	ランク表示
 *
 * @param	wk			STAGE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void RankWrite( STAGE_WORK* wk, GF_BGL_BMPWIN* win, u8 num, u32 x, u32 y )
{
	NUMFONT_WriteNumber( wk->num_font, num, 2, NUMFONT_MODE_LEFT, win, x, y );
	GF_BGL_BmpWinOnVReq( win );
	return;
}

static void RankWriteAll( STAGE_WORK* wk, GF_BGL_BMPWIN* win )
{
	int i,j;
	u8 rank,pos,type;

	for( i=0; i < TYPE_Y_MAX ;i++ ){
		for( j=0; j < TYPE_X_MAX ;j++ ){

			type = Stage_GetPokeType( i * TYPE_X_MAX + j );

			//ダミータイプは無視する(???もレベルいらないで外す)
			if( (type != DUMMY_TYPE) && (type != HATE_TYPE ) ){

				pos = ( i * TYPE_X_MAX + j );
				rank = Stage_GetTypeLevel( Stage_GetCsrPos(pos), wk->p_rank );

				//0オリジンなので
				rank++; 
				if( rank > STAGE_TYPE_LEVEL_MAX ){
					rank = STAGE_TYPE_LEVEL_MAX;		//表示は10が最大
				}
				
				RankWrite(	wk, win, rank, 
							RANK_START_X + (RANK_X * j), 
							RANK_START_Y + (RANK_Y * i) );
			}
		}
	}

	return;
}


//==============================================================================================
//
//	ポケモンステータス呼び出し関連
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	psdのセット
 *
 * @param	wk		STAGE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Stage_SetPSD( STAGE_WORK* wk )
{
	wk->psd = sys_AllocMemory( HEAPID_STAGE, sizeof(PSTATUS_DATA) );
	memset( wk->psd, 0, sizeof(PSTATUS_DATA) );

	wk->psd->ppd	= wk->p_party;
	wk->psd->ppt	= PST_PP_TYPE_POKEPARTY;		//パラメータタイプ

	wk->psd->cfg	= wk->config;

	//あとで確認
	//wk->psd->mode = PST_MODE_NORMAL;
	wk->psd->mode = PST_MODE_NO_WAZACHG;

	wk->psd->max	= PokeParty_GetPokeCount( wk->p_party );
	wk->psd->pos	= 0;

	//あとで確認
	wk->psd->waza	= 0;
	wk->psd->zukan_mode = PMNumber_GetMode( wk->sv );
	wk->psd->ev_contest = PokeStatus_ContestFlagGet( wk->sv );

	//[[[[[[ステータスのページはこれでよいのか？]]]]]]]
	PokeStatus_PageSet( wk->psd, PST_PageTbl_Normal );
	PokeStatus_PlayerSet( wk->psd, SaveData_GetMyStatus(wk->sv) );
	return;
}


//==============================================================================================
//
//	ツール
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	現在のtypeチェック関数
 *
 * @param	wk		STAGE_WORK型のポインタ
 * @param	type	チェックするタイプ
 *
 * @return	"TRUE = typeが同じ、FALSE = typeが違う"
 */
//--------------------------------------------------------------
static BOOL Stage_CheckType( STAGE_WORK* wk, u8 type )
{
	if( wk->type == type ){
		return TRUE;
	}

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	シーケンス変更
 *
 * @param	wk		STAGE_WORK型のポインタ
 * @param	seq		シーケンスのポインタ
 * @param	next	次のシーケンス定義
 *
 * @return	none
 */
//--------------------------------------------------------------
static void NextSeq( STAGE_WORK* wk, int* seq, int next )
{
	wk->sub_seq = 0;
	*seq		= next;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	キーチェック
 *
 * @param	key		チェックするキー
 *
 * @return	"結果"
 */
//--------------------------------------------------------------
static int KeyCheck( int key )
{
	return (sys.cont & key);
}

//--------------------------------------------------------------
/**
 * @brief	BGチェック
 *
 * @param	wk		STAGE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void BgCheck( STAGE_WORK* wk )
{
	if( sys.cont & PAD_KEY_UP ){
		if( sys.cont & PAD_BUTTON_A ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
		}else if( sys.cont & PAD_BUTTON_B ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_OFF );
		}
	}else if( sys.cont & PAD_KEY_DOWN ){
		if( sys.cont & PAD_BUTTON_A ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
		}else if( sys.cont & PAD_BUTTON_B ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
		}else if( sys.cont & PAD_BUTTON_L ){
		}else if( sys.cont & PAD_BUTTON_R ){
		}
	}else if( sys.cont & PAD_KEY_LEFT ){
		if( sys.cont & PAD_BUTTON_A ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
		}else if( sys.cont & PAD_BUTTON_B ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
		}
	}else if( sys.cont & PAD_KEY_RIGHT ){
		if( sys.cont & PAD_BUTTON_A ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
		}else if( sys.cont & PAD_BUTTON_B ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
		}
	}else if( sys.cont & PAD_BUTTON_SELECT ){
		if( sys.cont & PAD_BUTTON_A ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
		}else if( sys.cont & PAD_BUTTON_B ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );
		}
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	十字キー操作
 *
 * @param	key		チェックするキー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CsrMove( STAGE_WORK* wk, int key )
{
	int flag;
	flag = 0;

	///////////////////////////////////////////////////////////////////////////////////////////
	if( sys.trg & PAD_KEY_LEFT ){

		//現在のカーソル位置がダミータイプでない時は、
		//カーソル位置を保存しておく
		if( Stage_GetPokeType(wk->csr_pos) != DUMMY_TYPE ){
			wk->tmp_csr_pos = wk->csr_pos;
		}else{
		}

		if( (wk->csr_pos % TYPE_X_MAX) == 0 ){					//0,4,8,12,16
			wk->csr_pos+=(TYPE_X_MAX-1);						//0→3
		}else if( Stage_GetPokeType(wk->csr_pos) == DUMMY_TYPE ){
			wk->csr_pos = 16;									//16(直値なので注意！)
		}else{
			wk->csr_pos--;
		}

		flag = 1;
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	if( sys.trg & PAD_KEY_RIGHT ){

		//現在のカーソル位置がダミータイプでない時は、
		//カーソル位置を保存しておく
		if( Stage_GetPokeType(wk->csr_pos) != DUMMY_TYPE ){
			wk->tmp_csr_pos = wk->csr_pos;
		}else{
		}

		if( (wk->csr_pos % TYPE_X_MAX) == (TYPE_X_MAX-1) ){		//3,7,11,15,19
			wk->csr_pos-=(TYPE_X_MAX-1);						//0←3
		}else if( Stage_GetPokeType(wk->csr_pos) == DUMMY_TYPE ){
			wk->csr_pos = 19;									//19(直値なので注意！)
		}else{
			wk->csr_pos++;
		}

		flag = 1;
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	if( sys.trg & PAD_KEY_UP ){

		//現在のカーソル位置がダミータイプでない時は、
		//カーソル位置を保存しておく
		if( Stage_GetPokeType(wk->csr_pos) != DUMMY_TYPE ){
			wk->tmp_csr_pos = wk->csr_pos;
		}else{
		}

		if( wk->csr_pos < TYPE_X_MAX ){							//0,1,2,3
			wk->csr_pos+=(TYPE_X_MAX*(TYPE_Y_MAX-1));			//0→16
		}else if( Stage_GetPokeType(wk->csr_pos) == DUMMY_TYPE ){
			if( wk->tmp_csr_pos == 16 ){										//左にいた
				wk->csr_pos = 13;
			}else if( wk->tmp_csr_pos == 19 ){									//右にいた
				wk->csr_pos = 14;
			}else if( (wk->tmp_csr_pos == 13) || (wk->tmp_csr_pos == 1 ) ){		//左上にいた
				wk->csr_pos = 13;
			}else if( (wk->tmp_csr_pos == 14) || (wk->tmp_csr_pos == 2 ) ){		//右上にいた
				wk->csr_pos = 14;
			}
		}else{
			wk->csr_pos-=TYPE_X_MAX;
		}

		flag = 1;
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	if( sys.trg & PAD_KEY_DOWN ){

		//現在のカーソル位置がダミータイプでない時は、
		//カーソル位置を保存しておく
		if( Stage_GetPokeType(wk->csr_pos) != DUMMY_TYPE ){
			wk->tmp_csr_pos = wk->csr_pos;
		}else{
		}

		if( wk->csr_pos >= TYPE_X_MAX*(TYPE_Y_MAX-1) ){							//16,17,18,19
			wk->csr_pos-=(TYPE_X_MAX*(TYPE_Y_MAX-1));							//16→0
		}else if( Stage_GetPokeType(wk->csr_pos) == DUMMY_TYPE ){
			if( wk->tmp_csr_pos == 16 ){										//左にいた
				wk->csr_pos = 1;
			}else if( wk->tmp_csr_pos == 19 ){									//右にいた
				wk->csr_pos = 2;
			}else if( (wk->tmp_csr_pos == 13) || (wk->tmp_csr_pos == 1 ) ){		//左上にいた
				wk->csr_pos = 1;
			}else if( (wk->tmp_csr_pos == 14) || (wk->tmp_csr_pos == 2 ) ){		//右上にいた
				wk->csr_pos = 2;
			}
		}else{
			wk->csr_pos+=TYPE_X_MAX;
		}

		flag = 1;
	}

	if( flag == 1 ){
		Snd_SePlay( SEQ_SE_DP_SELECT );
		StageObj_SetObjPos( wk->p_csr, GetCsrX(wk), GetCsrY(wk) );
	}

	//現在のカーソル位置がダミータイプかでアニメを変更する
	if( Stage_GetPokeType(wk->csr_pos) == DUMMY_TYPE ){
		StageObj_AnmChg( wk->p_csr, ANM_CSR_POKE );			//アニメ変更
		StageObj_SetObjPos( wk->p_csr, CSR_POKE_X, CSR_POKE_Y );
	}else{
		StageObj_AnmChg( wk->p_csr, ANM_CSR_MOVE );			//アニメ変更
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	カーソルＸ取得
 *
 * @param	wk		STAGE_WORK型のポインタ
 *
 * @return	"X"
 */
//--------------------------------------------------------------
static u16 GetCsrX( STAGE_WORK* wk )
{
	u8 pos;
	pos = wk->csr_pos;

	return (pos % TYPE_X_MAX) * TYPE_X + CSR_START_X;
}

//--------------------------------------------------------------
/**
 * @brief	カーソルＹ取得
 *
 * @param	wk		STAGE_WORK型のポインタ
 *
 * @return	"Y"
 */
//--------------------------------------------------------------
static u16 GetCsrY( STAGE_WORK* wk )
{
	return (wk->csr_pos / TYPE_X_MAX) * TYPE_Y + CSR_START_Y;
}

//--------------------------------------------------------------
/**
 * @brief	タイプのパレット番号を変更
 *
 * @param	wk		STAGE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void GameStartTypeScrnPalChg( STAGE_WORK* wk, GF_BGL_INI* ini )
{
	int i;

	//ブレーンが登場する戦闘回数だったら、「???」以外はパレット変更
	//if( Stage_BtlCountLeaderCheck(wk->btl_count) == TRUE ){
	if( Stage_BrainAppearCheck(wk) == TRUE ){

		//「???」は含まず
		//for( i=0; i < (STAGE_TR_TYPE_MAX-1) ;i++ ){
		for( i=0; i < (TYPE_XY_MAX-3) ;i++ ){			//status左、status右、???の3つを抜かす
			ScrnPalChg( ini, i, MAX_PAL_CHG );
		}

	}else{

		//開始時にレベルがマックスになっているものはパレット番号を変更
		//for( i=0; i < STAGE_TR_TYPE_MAX ;i++ ){
		for( i=0; i < (STAGE_TR_TYPE_MAX-1) ;i++ ){
			if( Stage_GetTypeLevel(i,wk->p_rank) >= STAGE_TYPE_LEVEL_MAX ){		//0-9,10
				//指定フレームのスクリーンの指定位置のパレットを変更する
				ScrnPalChg( ini, i, MAX_PAL_CHG );
			}
		}

		//「???」をパレット変更
		ScrnPalChg( ini, (TYPE_XY_MAX - 1), MAX_PAL_CHG );
	}

	//スクリーンデータ転送
	GF_BGL_LoadScreenV_Req( ini, BS_FRAME_BG );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	指定フレームのスクリーンの指定位置のパレットを変更
 *
 * @param	wk		STAGE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void ScrnPalChg( GF_BGL_INI* ini, u8 csr_pos, u8 flag )
{
	u8 px,py,sx,sy,pal;

	if( flag == NORMAL_PAL_CHG ){
		pal = NORMAL_PAL;					//通常パレット
	}else if( flag == DECIDE_PAL_CHG ){
		pal = DECIDE_PAL;					//プレイヤー決定パレット
	}else if( flag == PAIR_PAL_CHG ){
		pal = PAIR_PAL;						//パートナー決定パレット
	}else{
		pal = MAX_PAL;						//レベル最高パレット
	}

	sx = 8;
	px = (csr_pos % TYPE_X_MAX) * sx;

	//スクリーンのＹサイズが場所によって違うので調整
	if( (csr_pos % 8) < 4 ){
		sy = 5;
	}else{
		sy = 4;
	}

	//
	if( csr_pos < TYPE_X_MAX ){
		py = 0;
	}else if( csr_pos < (TYPE_X_MAX * 2) ){
		py = 5;
	}else if( csr_pos < (TYPE_X_MAX * 3) ){
		py = 9;
		}else if( csr_pos < (TYPE_X_MAX * 4) ){
		py = 14;
	}else{
		py = 18;
	}

	//OS_Printf( "px = %d\n", px );
	//OS_Printf( "py = %d\n", py );
	//OS_Printf( "sx = %d\n", sx );
	//OS_Printf( "sy = %d\n", sy );
	GF_BGL_ScrPalChange( ini, BS_FRAME_BG, px, py, sx, sy, pal );

	/////////////////////////////////////////////////////////////////////////
	//通常パレットは2本使われているのでさらに上書き
	if( flag == NORMAL_PAL_CHG ){
		pal = NORMAL_PAL;

		sx = 1;
		px = (csr_pos % TYPE_X_MAX) * 8;

		//スクリーンのＹサイズが場所によって違うので調整
		if( (csr_pos % 8) < 4 ){
			sy = 2;
		}else{
			sy = 3;
		}

		if( csr_pos < TYPE_X_MAX ){
			py = 2;
		}else if( csr_pos < (TYPE_X_MAX * 2) ){
			py = 6;
		}else if( csr_pos < (TYPE_X_MAX * 3) ){
			py = 11;
			}else if( csr_pos < (TYPE_X_MAX * 4) ){
			py = 15;
		}else{
			py = 20;
		}

		//OS_Printf( "px = %d\n", px );
		//OS_Printf( "py = %d\n", py );
		//OS_Printf( "sx = %d\n", sx );
		//OS_Printf( "sy = %d\n", sy );
		
		//書き換えるパレットが違う
		if( csr_pos < 9 ){
			GF_BGL_ScrPalChange( ini, BS_FRAME_BG, px, py, sx, sy, NORMAL_PAL2 );	//前半
		}else{
			GF_BGL_ScrPalChange( ini, BS_FRAME_BG, px, py, sx, sy, NORMAL_PAL3 );	//後半
		}
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	全てのタイプがLV10かチェックしてフラグをセット
 *
 * @param	wk		STAGE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void StageMasterFlagSet( STAGE_WORK* wk )
{
	int i;

	//「???」は含まず
	//for( i=0; i < STAGE_TR_TYPE_MAX ;i++ ){
	for( i=0; i < (STAGE_TR_TYPE_MAX-1) ;i++ ){
		if( Stage_GetTypeLevel(i,wk->p_rank) < STAGE_TYPE_LEVEL_MAX ){
			wk->master_flag = 0;
			return;
		}
	}

	wk->master_flag = 1;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	データを参照する時のカーソル位置を調整した値を取得
 *
 * @param	csr_pos		カーソル位置
 *
 * @return	"調整したカーソル位置"
 */
//--------------------------------------------------------------
static u8 Stage_GetCsrPos( u8 csr_pos )
{
	if( csr_pos >= 17 ){
		return STAGE_TR_TYPE_MAX - 1;
	}

	return csr_pos;
}

//--------------------------------------------------------------
/**
 * @brief	ブレーンが登場する戦闘回数かを取得
 *
 * @param	btl_count	戦闘回数
 *
 * @return	"TRUE = ブレーン登場、FALSE = しない"
 */
//--------------------------------------------------------------
static BOOL Stage_BtlCountLeaderCheck( STAGE_WORK* wk )
{
#ifdef DEBUG_BRAIN_SET
	return TRUE;
#endif

	//シングルのみ
	if( wk->type == STAGE_TYPE_SINGLE ){

		//ブレーンが登場する戦闘回数だったら、「???」以外はパレット変更
		if( (wk->btl_count == STAGE_LEADER_SET_1ST) || (wk->btl_count == STAGE_LEADER_SET_2ND) ){
			return TRUE;
		}
	}

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	ブレーンが登場するフラグをチェックを取得
 *
 * @param	wk		STAGE_WORK型のポインタ
 *
 * @return	"TRUE = ブレーン登場、FALSE = しない"
 */
//--------------------------------------------------------------
static BOOL Stage_BrainAppearCheck( STAGE_WORK* wk )
{
	return wk->brain_appear;
}


//==============================================================================================
//
//	通信(CommStart)
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	送信ウェイト　
 *
 * @param	wk			STAGE_WORK型のポインタ
 * @param	type		送信タイプ
 *
 * @retval	none
 */
//--------------------------------------------------------------
BOOL Stage_CommSetSendBuf( STAGE_WORK* wk, u16 type, u16 param )
{
	int ret,command;

	//通信タイプでない時
	if( Stage_CommCheck(wk->type) == FALSE ){
		return FALSE;
	}

	switch( type ){

	//名前
	case STAGE_COMM_PAIR:
		command = FC_STAGE_PAIR;
		Stage_CommSendBufBasicData( wk, type );
		break;

	//タイプ選択画面で決定したポケモンタイプ
	case STAGE_COMM_PAIR_POKETYPE:
		command = FC_STAGE_PAIR_POKETYPE;
		Stage_CommSendBufPokeTypeData( wk, type, param );
		break;

	//ポケモンタイプを最終決定かキャンセルか
	case STAGE_COMM_PAIR_FINAL_ANSWER:
		command = FC_STAGE_PAIR_FINAL_ANSWER;
		Stage_CommSendBufFinalAnswerData( wk, type, param );
		break;
	};

	if( CommSendData(command,wk->send_buf,STAGE_COMM_BUF_LEN) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief	send_bufに基本情報をセット
 *
 * @param	wk		STAGE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
void Stage_CommSendBufBasicData( STAGE_WORK* wk, u16 type )
{
	MYSTATUS* my;
	
	my	= SaveData_GetMyStatus( wk->sv );

	wk->send_buf[0] = type;
	OS_Printf( "送信：type = %d\n", wk->send_buf[0] );
	return;
}

//--------------------------------------------------------------
/**
 * @brief   基本情報 通信受信処理
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		FRONTIER_SYSTEMへのポインタ
 */
//--------------------------------------------------------------
void Stage_CommRecvBufBasicData(int id_no,int size,void *pData,void *work)
{
	int i,num;
	STAGE_WORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******ステージ****** 基本情報受信\n" );

	num = 0;
	//wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	OS_Printf( "id_no = %d\n", id_no );
	OS_Printf( "size = %d\n", size );
	//OS_Printf( "pData[0] = %d\n", ((u8*)pData)[0] );
	OS_Printf( "recv_buf[0] = %d\n", recv_buf[0] );

	//type = recv_buf[0];
	
	//wk-> = 
	return;
}

//--------------------------------------------------------------
/**
 * @brief	send_bufにタイプ選択画面で決定したポケモンタイプをセット
 *
 * @param	wk		STAGE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
void Stage_CommSendBufPokeTypeData( STAGE_WORK* wk, u16 type, u16 param )
{
	int i,num;
	POKEMON_PARAM* poke;
	POKEPARTY* party;
	
	num = 0;

	//コマンド
	wk->send_buf[0] = type;
	OS_Printf( "送信：type = %d\n", wk->send_buf[0] );

	//選択している位置
	wk->send_buf[1] = param;
	OS_Printf( "送信：csr_pos = %d\n", wk->send_buf[1] );

	//先に子の選択がきていなくて、まだ値が入っていない時は、親の決定はセットしてしまう
	if( CommGetCurrentID() == COMM_PARENT_ID ){
		if( wk->parent_decide_pos == STAGE_DECIDE_NONE ){
			wk->parent_decide_pos = param;
		}
	}

	//親の決定タイプ
	wk->send_buf[2] = wk->parent_decide_pos;
	OS_Printf( "送信：parent_decide_pos = %d\n", wk->send_buf[2] );

	poke =  PokeParty_GetMemberPointer( wk->p_party, 0 );
	wk->send_buf[3] = PokeParaGet( poke, ID_PARA_level, NULL );
	OS_Printf( "送信：poke level = %d\n", wk->send_buf[3] );
	return;
}

//--------------------------------------------------------------
/**
 * @brief   recv_bufのタイプ選択画面で決定したポケモンタイプ 通信受信処理
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		FRONTIER_SYSTEMへのポインタ
 */
//--------------------------------------------------------------
void Stage_CommRecvBufPokeTypeData(int id_no,int size,void *pData,void *work)
{
	int i,num;
	STAGE_WORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******ステージ****** 基本情報受信\n" );

	num = 0;
	wk->typesel_recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	//type = recv_buf[0];
	
	wk->pair_csr_pos = recv_buf[1];	//パートナーが選んだポケモンタイプ(カーソル位置)
	OS_Printf( "受信：wk->pair_csr_pos = %d\n", wk->pair_csr_pos );

	////////////////////////////////////////////////////////////////////////
	//親
	if( CommGetCurrentID() == COMM_PARENT_ID ){

		//親の決定がすでに決まっていたら、子の選択は無効
		if( wk->parent_decide_pos != STAGE_DECIDE_NONE ){
			wk->pair_csr_pos = 0;
		}else{

			//子の選択が採用されたことがわかるようにオフセット(TYPE_XY_MAX)を加える
			wk->parent_decide_pos	= wk->pair_csr_pos + TYPE_XY_MAX;
		}
	////////////////////////////////////////////////////////////////////////
	//子
	}else{
		//親の決定タイプ
		wk->parent_decide_pos = recv_buf[2];
	}

	OS_Printf( "受信：wk->parent_decide_pos = %d\n", wk->parent_decide_pos );

	//////////////////////////////////////////////////////////////////////////////////////
	
	*(wk->p_pair_poke_level) = recv_buf[3];
	OS_Printf( "受信：wk->p_pair_poke_level = %d\n", *(wk->p_pair_poke_level) );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	send_bufにポケモンタイプを最終決定かキャンセルかをセット
 *
 * @param	wk		STAGE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
void Stage_CommSendBufFinalAnswerData( STAGE_WORK* wk, u16 type, u16 param )
{
	wk->send_buf[0] = type;
	OS_Printf( "送信：type = %d\n", wk->send_buf[0] );

	wk->send_buf[1] = param;
	OS_Printf( "送信：pair_final_answer = %d\n", wk->send_buf[1] );
	return;
}

//--------------------------------------------------------------
/**
 * @brief   recv_bufのポケモンタイプを最終決定かキャンセルか 通信受信処理
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		FRONTIER_SYSTEMへのポインタ
 */
//--------------------------------------------------------------
void Stage_CommRecvBufFinalAnswerData(int id_no,int size,void *pData,void *work)
{
	int i,num;
	STAGE_WORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******ステージ****** 基本情報受信\n" );

	num = 0;
	//wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	OS_Printf( "id_no = %d\n", id_no );
	OS_Printf( "size = %d\n", size );
	//OS_Printf( "pData[0] = %d\n", ((u8*)pData)[0] );
	OS_Printf( "recv_buf[0] = %d\n", recv_buf[0] );

	//何もなしはそのまま
	if( recv_buf[1] == FINAL_ANSWER_NONE ){
		return;
	}
	
	wk->pair_final_answer = recv_buf[1];		//0=何もなし、1=最終決定、2=キャンセル
	OS_Printf( "受信：wk->pair_final_answer = %d\n", wk->pair_final_answer );
	return;
}


//==============================================================================================
//
//	サブシーケンス
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	メニューウィンドウクリア
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Stage_SeqSubMenuWinClear( STAGE_WORK* wk )
{
	if( wk->menu_flag == 1 ){
		wk->menu_flag = 0;
		BmpMenuExit( wk->mw, NULL );
		BmpMenuWinClear( wk->MenuH.win, WINDOW_TRANS_ON );
	}
	return;
}

//--------------------------------------------------------------
/**
 * @brief	タイプを選んで「はい、いいえ」中
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Stage_SeqSubTypeSelYesNo( STAGE_WORK* wk )
{
	u8 num;
	int msg_id;

	//会話ウィンドウ表示
	StageObj_Vanish( wk->p_icon, STAGE_VANISH_ON );	//非表示
	StageTalkWinPut( &wk->bmpwin[BMPWIN_TALK], CONFIG_GetWindowType(wk->config) );

	//「○タイプの○ランクとバトルしますか？」
	WORDSET_RegisterPokeTypeName( wk->wordset, 0, Stage_GetPokeType(wk->csr_pos) );

	num = Stage_GetTypeLevel( Stage_GetCsrPos(wk->csr_pos),wk->p_rank );	//0-9,10
	num++;
	if( num > STAGE_TYPE_LEVEL_MAX ){
		num = STAGE_TYPE_LEVEL_MAX;		//表示は10が最大
	}

	Stage_SetNumber(wk, 1, num );

	if( Stage_BrainAppearCheck(wk) == TRUE ){
		msg_id = msg_stage_room_44;		//ブレーン
	}else{
		msg_id = msg_stage_room_41;		//通常
	}

	wk->msg_index = Stage_EasyMsg( wk, msg_id, FONT_TALK );

	//アニメ変更
	StageObj_AnmChg( wk->p_csr, ANM_CSR_STOP );

	//「はい・いいえ」
	Stage_SetMenu2( wk );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	タイプを選んで「いいえ」
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Stage_SeqSubTypeSelNo( STAGE_WORK* wk )
{
	Stage_SeqSubWinRet( wk );

	//指定フレームのスクリーンの指定位置のパレットを変更する
	ScrnPalChg( wk->bgl, wk->csr_pos, NORMAL_PAL_CHG );
	GF_BGL_LoadScreenV_Req( wk->bgl, BS_FRAME_BG );	//スクリーンデータ転送

	//アニメ変更
	StageObj_AnmChg( wk->p_csr, ANM_CSR_MOVE );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	ウィンドウを元に戻す
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Stage_SeqSubWinRet( STAGE_WORK* wk )
{
	StageObj_Vanish( wk->p_icon, STAGE_VANISH_OFF );					//ポケモン表示
	BmpTalkWinClear( &wk->bmpwin[BMPWIN_TALK], WINDOW_TRANS_ON );		//会話ウィンドウ削除
	PokeNameWrite(	wk, &wk->bmpwin[BMPWIN_POKE1], 0, 0,				//ポケモン名を表示
					FBMP_COL_BLACK, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BS_FONT );
	return;
}



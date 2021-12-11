//==============================================================================================
/**
 * @file	castle_enemy.c
 * @brief	「バトルキャッスル 敵トレーナー画面」メインソース
 * @author	Satoshi Nohara
 * @date	07.07.05
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
#include "system/bmp_list.h"
#include "system/window.h"
#include "system/fontproc.h"
#include "system/numfont.h"
#include "gflib/strbuf_family.h"
#include "savedata/config.h"
#include "application/p_status.h"
#include "application/app_tool.h"
#include "poketool/pokeparty.h"
#include "poketool/poke_number.h"
#include "poketool/pokeicon.h"
#include "savedata/b_tower.h"
#include "savedata/frontier_savedata.h"

#include "communication/comm_system.h"
#include "communication/comm_tool.h"
#include "communication/comm_info.h"
#include "communication/comm_def.h"
#include "communication/wm_icon.h"

#include "castle_sys.h"
#include "castle_common.h"
#include "castle_clact.h"
#include "castle_bmp.h"
#include "castle_obj.h"
#include "castle_enemy_def.h"
#include "application/castle.h"
#include "../frontier_tool.h"						//Frontier_PokeParaMake
#include "../castle_tool.h"							//
#include "../castle_def.h"
#include "../comm_command_frontier.h"

#include "../../field/comm_direct_counter_def.h"	//
#include "../../field/fieldobj.h"
#include "../../field/scr_tool.h"

#include "msgdata/msg.naix"							//NARC_msg_??_dat
#include "msgdata/msg_castle_trainer.h"				//msg_??

#include "../graphic/frontier_obj_def.h"
#include "../graphic/frontier_bg_def.h"


#include "system/pm_overlay.h"
FS_EXTERN_OVERLAY(frontier_common);


//==============================================================================================
//
//	デバック用
//
//==============================================================================================
//PROC_DATA* p_proc;


//==============================================================================================
//
//	定義
//
//==============================================================================================
//シーケンス定義
enum {
	SEQ_GAME_INIT,											//初期化

	SEQ_GAME_TYPE_SEL,										//タイプ選択中

	SEQ_GAME_SEND_RECV,										//送受信

	SEQ_GAME_END_MULTI,										//(通信)終了
	SEQ_GAME_END,											//終了
};

//選択肢
enum{
	SEL_TEMOTI = 0,
	SEL_LEVEL,
	SEL_TUYOSA,
	SEL_WAZA,
	SEL_TOZIRU,
};

#define INFO_RANKUP_POINT			(50)					//ランクアップに必要なポイント

//何を決定したか定義
enum{
	FC_PARAM_TEMOTI = 0,				//てもち
	FC_PARAM_LEVEL,						//レベル
	FC_PARAM_INFO,						//じょうほう
	FC_PARAM_TUYOSA,					//つよさ
	FC_PARAM_WAZA,						//わざ
	/////////////////////////////
	FC_PARAM_INFO_RANKUP,			//(じょうほう)ランクアップ
	/////////////////////////////
	FC_PARAM_TOZIRU,					//とじる
};

//TYPE_SELに戻した時の復帰処理の分岐定義
#define RECOVER_NONE				(0)
#define RECOVER_NORMAL				(1)
#define RECOVER_RANKUP				(2)
#define RECOVER_STATUS				(3)

//----------------------------------------------------------------------------------------------
//旗の表示位置
//----------------------------------------------------------------------------------------------
enum{
	CASTLE_HATA_START_X			= (96),
	CASTLE_MULTI_HATA_START_X	= (64),
	CASTLE_HATA_START_Y			= (60),
};
#define CASTLE_HATA_WIDTH_X		(64)				//表示する幅
#define CASTLE_HATA_WIDTH_Y		(12)				//表示する幅

//ランクアップエフェクト表示位置
#define CASTLE_ENEMY_RANKUP_X	(211)
#define CASTLE_ENEMY_RANKUP_Y	(105)


//==============================================================================================
//
//	構造体宣言
//
//==============================================================================================
struct _CASTLE_ENEMY_WORK{

	PROC* proc;										//PROCへのポインタ
	FRONTIER_SAVEWORK* fro_sv;						//

	u8	sub_seq;									//シーケンス
	u8	type;										//引数として渡されたバトルタイプ
	u8	msg_index;									//メッセージindex
	u8	recover;									//復帰フラグ

	u8	tmp_csr_pos;								//退避してあるカーソル位置
	u8	csr_pos;									//現在のカーソル位置
	u8	level_sel;									//
	u8	eff_init_flag:1;							//決定エフェクト初期化フラグ
	u8	list_flag:1;								//リスト表示中かフラグ
	u8	menu_flag:1;								//メニュー表示中かフラグ
	u8	recover_flag:5;								//TYPE_SELに戻った時の分岐フラグ

	u8	send_req;
	u8	parent_decide_pos;							//決定したカーソル位置(どのポケモンか)
	u8	parent_decide_updown;
	u8	parent_decide_type;							//決定した項目(FC_PARAM_??)

	u8	h_max;
	u8	modoru_pos;
	u8	wait;
	u8	recieve_count;								//受信カウント

	u16 list_csr_pos;								//リストで選択した位置
	u16	dummy26;

	u16	basic_list_lp;
	u16	basic_list_cp;

	MSGDATA_MANAGER* msgman;						//メッセージマネージャー
	WORDSET* wordset;								//単語セット
	STRBUF* msg_buf;								//メッセージバッファポインタ
	STRBUF* tmp_buf;								//テンポラリバッファポインタ

	STRBUF* menu_buf[CASTLE_MENU_BUF_MAX];			//メニューバッファポインタ
	STRCODE str[PERSON_NAME_SIZE + EOM_SIZE];		//メニューのメッセージ

	GF_BGL_INI*	bgl;								//BGLへのポインタ
	GF_BGL_BMPWIN bmpwin[CASTLE_ENEMY_BMPWIN_MAX];	//BMPウィンドウデータ

	//BMPメニュー(bmp_menu.h)
	BMPMENU_HEADER MenuH;							//BMPメニューヘッダー
	BMPMENU_WORK* mw;								//BMPメニューワーク
	BMPMENU_DATA Data[CASTLE_MENU_BUF_MAX];			//BMPメニューデータ

	//BMPリスト
	BMPLIST_WORK* lw;								//BMPリストデータ
	BMPLIST_DATA* menulist;							//

	PALETTE_FADE_PTR pfd;							//パレットフェード

	NUMFONT* num_font;								//8x8フォント

	//const CONFIG* config;							//コンフィグポインタ
	CONFIG* config;									//コンフィグポインタ
	SAVEDATA* sv;									//セーブデータポインタ
	CASTLEDATA* castle_sv;							//キャッスルセーブデータポインタ
	CASTLESCORE* score_sv;							//キャッスルセーブデータポインタ

	CASTLE_CLACT castle_clact;						//セルアクタデータ
	CASTLE_OBJ* p_icon[CASTLE_COMM_POKE_TOTAL_NUM];	//アイコンOBJのポインタ格納テーブル
	CASTLE_OBJ* p_ball[CASTLE_COMM_POKE_TOTAL_NUM];	//ボールOBJのポインタ格納テーブル
	CASTLE_OBJ* p_eff;								//エフェクトOBJのポインタ格納テーブル
	CASTLE_OBJ* p_poke_sel;							//ポケモン選択ウィンOBJのポインタ
	CASTLE_OBJ* p_pair_poke_sel;					//ペアのポケモン選択ウィンOBJのポインタ
	CASTLE_OBJ* p_hp[CASTLE_COMM_POKE_TOTAL_NUM];	//HP状態OBJのポインタ
	CASTLE_OBJ* p_hata[CASTLE_COMM_POKE_TOTAL_NUM][2];	//旗OBJのポインタ格納テーブル
	CASTLE_OBJ* p_eff_rankup;						//ランクアップOBJのポインタ格納テーブル

	u16* p_ret_work;								//CASTLE_CALL_WORKの戻り値ワークへのポインタ

	//敵トレーナー画面の情報が公開されたかフラグ
	u8*	p_temoti_flag;
	u8*	p_level_flag;
	u8*	p_tuyosa_flag;
	u8* p_waza_flag;

	//バトルキャッスル用
	POKEPARTY* p_party;

	ARCHANDLE* hdl;

	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//CASTLE_COMM castle_comm;
#if 1
	//通信用：データバッファ
	u16	send_buf[CASTLE_COMM_BUF_LEN];

	//通信用
	u8	pair_csr_pos;								//パートナーのカーソル位置
	u8	pair_rankup_type;							//パートナーのランクアップしたいタイプ
	u8	pair_modoru_flag;							//パートナーの「戻る」選択したか
	u8	pair_rank[CASTLE_RANK_TYPE_MAX];			//パートナーのランク
	u16 pair_cp;									//パートナーのCP

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
PROC_RESULT CastleEnemyProc_Init( PROC * proc, int * seq );
PROC_RESULT CastleEnemyProc_Main( PROC * proc, int * seq );
PROC_RESULT CastleEnemyProc_End( PROC * proc, int * seq );

//シーケンス
static BOOL Seq_GameInit( CASTLE_ENEMY_WORK* wk );
static void TypeSelInit( CASTLE_ENEMY_WORK* wk );
static BOOL Seq_GameTypeSel( CASTLE_ENEMY_WORK* wk );
static BOOL Seq_GameSendRecv( CASTLE_ENEMY_WORK* wk );
static BOOL Seq_GameEndMulti( CASTLE_ENEMY_WORK* wk );
static BOOL Seq_GameEnd( CASTLE_ENEMY_WORK* wk );

//共通処理
static void CastleCommon_Delete( CASTLE_ENEMY_WORK* wk );
static void Castle_Recover( CASTLE_ENEMY_WORK* wk );
static void Castle_InitSub1( void );
static void Castle_InitSub2( CASTLE_ENEMY_WORK* wk );

//共通初期化、終了
static void Castle_ObjInit( CASTLE_ENEMY_WORK* wk );
static void Castle_BgInit( CASTLE_ENEMY_WORK* wk );
static void Castle_BgExit( GF_BGL_INI * ini );

//設定
static void VBlankFunc( void * work );
static void SetVramBank(void);
static void SetBgHeader( GF_BGL_INI * ini );

//BGグラフィックデータ
static void Castle_SetMainBgGraphic( CASTLE_ENEMY_WORK * wk, u32 frm  );
static void Castle_SetMainBgPalette( void );
static void Castle_SetStatusBgGraphic( CASTLE_ENEMY_WORK * wk, u32 frm  );
static void Castle_SetWazaBgGraphic( CASTLE_ENEMY_WORK * wk, u32 frm  );
static void Castle_SetStatusBgPalette( void );
static void Castle_SetSubBgGraphic( CASTLE_ENEMY_WORK * wk, u32 frm  );

//メッセージ
static u8 CastleWriteMsg( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font );
// MatchComment: new function prototype
static u8 CastleWriteMsg_Full_ov107_2247680( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font, u32 a10_mode );
static u8 CastleWriteMsgSimple( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font );
// MatchComment: new function prototype
static u8 CastleWriteMsgSimple_Full_ov107_2247744( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font, u32 a10_mode );
static u8 Castle_EasyMsg( CASTLE_ENEMY_WORK* wk, int msg_id, u8 font );
static void Castle_StatusMsgWrite( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win, POKEMON_PARAM* poke );
// MatchComment: add arg a5_mode
static void StMsgWriteSub( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win, int msg, u16 x, u16 y, u32 a5_mode );
static void Castle_WazaMsgWrite( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win, POKEMON_PARAM* poke );
static void WazaMsgWriteSub( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win, u8 no, u32 msg_id, u32 msg_id2, POKEMON_PARAM* poke, u32 id, u32 id2, u32 id3 );
static void Castle_PokeHpMsgWrite( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win );
static void Castle_PokeLvMsgWrite( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win );

//メニュー
static void CastleInitMenu( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win, u8 y_max );
static void CastleSetMenuData( CASTLE_ENEMY_WORK* wk, u8 no, u8 param, int msg_id );
static void Castle_SetMenu2( CASTLE_ENEMY_WORK* wk );
static void Castle_SetMenu3( CASTLE_ENEMY_WORK* wk );

//リスト
static void Castle_BasicListMake( CASTLE_ENEMY_WORK* wk );
static void Castle_CsrMoveCallBack( BMPLIST_WORK* work, u32 param, u8 mode );
static void Castle_Basic2ListMake( CASTLE_ENEMY_WORK* wk );
static void Castle_Basic2CsrMoveCallBack( BMPLIST_WORK* work, u32 param, u8 mode );
static void Castle_Basic2LineWriteCallBack( BMPLIST_WORK* work, u32 param, u8 y );

//文字列
static void Castle_SetNumber( CASTLE_ENEMY_WORK* wk, u32 bufID, s32 number, u32 keta, NUMBER_DISPTYPE disp );
static void Castle_SetPokeName( CASTLE_ENEMY_WORK* wk, u32 bufID, POKEMON_PASO_PARAM* ppp );
static void Castle_SetPlayerName( CASTLE_ENEMY_WORK* wk, u32 bufID );
static void PlayerNameWrite( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 font );
static void PairNameWrite( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 font );
static void PokeNameWrite( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 f_col, u8 s_col, u8 b_col, u8 font );
static void PokeNameWriteEx( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 f_col, u8 s_col, u8 b_col, u8 font, u16 monsno, u8 sex );
static void PokeSexWrite( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 font, u8 sex );
static void CastleEnemy_Default_Write( CASTLE_ENEMY_WORK* wk );
static void CastleEnemy_Default_Del( CASTLE_ENEMY_WORK* wk );
static void CastleEnemy_Basic_Write( CASTLE_ENEMY_WORK* wk );
static void CastleEnemy_Basic_Del( CASTLE_ENEMY_WORK* wk );
static void CastleEnemy_Basic2_Write( CASTLE_ENEMY_WORK* wk );
static void CastleEnemy_Basic2_Del( CASTLE_ENEMY_WORK* wk );

//ツール
static BOOL Castle_CheckType( CASTLE_ENEMY_WORK* wk, u8 type );
static void NextSeq( CASTLE_ENEMY_WORK* wk, int* seq, int next );
static int KeyCheck( int key );
static void BgCheck( CASTLE_ENEMY_WORK* wk );
static void CsrMove( CASTLE_ENEMY_WORK* wk, int key );
static void CsrMoveSub( CASTLE_ENEMY_WORK* wk );
static void PokeSelMoveSub( CASTLE_ENEMY_WORK* wk, u8 csr_pos, u8 flag );
static void CastleEnemy_GetPokeSelXY( CASTLE_ENEMY_WORK* wk, u32* x, u32* y, u8 csr_pos );
static u16 GetCsrX( CASTLE_ENEMY_WORK* wk, u8 csr_pos, u8 lr );
static u16 GetCsrY( CASTLE_ENEMY_WORK* wk, u8 csr_pos );
static void Castle_GetOffset( CASTLE_ENEMY_WORK* wk, u16* offset_x, u16* offset_y, u16* pair_offset_x, u16* pair_offset_y );
static BOOL CastleEnemy_DecideEff( CASTLE_ENEMY_WORK* wk, u8 csr_pos, u8 sel_type );
static BOOL CastleEnemy_DecideEff2( CASTLE_ENEMY_WORK* wk, u8 csr_pos, u8 sel_type );
static u16 CastleEnemy_GetLvCP( u8 level_sel );
static void CastleEnemy_TuyosaWinChg( CASTLE_ENEMY_WORK* wk, s8 add_sub );
static void CastleEnemy_WazaWinChg( CASTLE_ENEMY_WORK* wk, s8 add_sub );
static void CastleEnemy_PairDecideDel( CASTLE_ENEMY_WORK* wk );
static void BmpTalkWinClearSub( GF_BGL_BMPWIN* win );

//通信
BOOL CastleEnemy_CommSetSendBuf( CASTLE_ENEMY_WORK* wk, u16 type, u16 param );
void CastleEnemy_CommSendBufBasicData( CASTLE_ENEMY_WORK* wk, u16 type );
void CastleEnemy_CommRecvBufBasicData(int id_no,int size,void *pData,void *work);
void CastleEnemy_CommSendBufRankUpType( CASTLE_ENEMY_WORK* wk, u16 type, u16 param );
void CastleEnemy_CommRecvBufReqType(int id_no,int size,void *pData,void *work);
void CastleEnemy_CommSendBufCsrPos( CASTLE_ENEMY_WORK* wk, u16 type );
void CastleEnemy_CommRecvBufCsrPos(int id_no,int size,void *pData,void *work);
void CastleEnemy_CommSendBufModoru( CASTLE_ENEMY_WORK* wk );
void CastleEnemy_CommRecvBufModoru(int id_no,int size,void *pData,void *work);

//サブシーケンス
static void CastleEnemy_SeqSubTuyosa( CASTLE_ENEMY_WORK* wk, u8 csr_pos );
static void CastleEnemy_SeqSubTuyosaFlagOn( CASTLE_ENEMY_WORK* wk, u8 csr_pos );
static void CastleEnemy_SeqSubTuyosaMsgSet( CASTLE_ENEMY_WORK* wk, u8 csr_pos );
static void CastleEnemy_SeqSubWaza( CASTLE_ENEMY_WORK* wk, u8 csr_pos );
static void CastleEnemy_SeqSubWazaFlagOn( CASTLE_ENEMY_WORK* wk, u8 csr_pos );
static void CastleEnemy_SeqSubWazaMsgSet( CASTLE_ENEMY_WORK* wk, u8 csr_pos );
static void CastleEnemy_SeqSubMenuWinClear( CASTLE_ENEMY_WORK* wk );
static void CastleEnemy_SeqSubTalkWinOn( CASTLE_ENEMY_WORK* wk );
static void CastleEnemy_SeqSubCPWrite( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win );
static void CastleEnemy_SeqSubLevelMsg( CASTLE_ENEMY_WORK* wk, u8 level_sel );
static void CastleEnemy_SeqSubNameWrite( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win );
static void CastleEnemy_SeqSubRankUp( CASTLE_ENEMY_WORK* wk, u8 decide_type, u8 parent_decide_type );
static void CastleEnemy_SeqSubTemoti( CASTLE_ENEMY_WORK* wk, u8 csr_pos );
static void CastleEnemy_SeqSubLevel( CASTLE_ENEMY_WORK* wk, u8 csr_pos, u8 level_sel );
static void CastleEnemy_SeqSubStatusWinDel( CASTLE_ENEMY_WORK* wk );
static void CastleEnemy_SeqSubListEnd( CASTLE_ENEMY_WORK* wk );
static void CastleEnemy_IconSelAnm( CASTLE_ENEMY_WORK* wk );
static BOOL CastleEnemy_SeqSubTuyosaWazaYes( CASTLE_ENEMY_WORK* wk, u16 use_cp, u16 cp_not_msg );

//castle_rank.c
static void CastleRank_SeqSubRankUp(CASTLE_ENEMY_WORK* wk, u8 parent_decide_pos, u8 param);
static void CastleRank_SeqSubHataVanish( CASTLE_ENEMY_WORK* wk );
static void Castle_HataVanish( CASTLE_ENEMY_WORK* wk, u8 no );
static void Castle_GetHataXY( CASTLE_ENEMY_WORK* wk, u32* x, u32* y );


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
PROC_RESULT CastleEnemyProc_Init( PROC * proc, int * seq )
{
	int i;
	CASTLE_ENEMY_WORK* wk;
	CASTLE_CALL_WORK* castle_call;

	Overlay_Load( FS_OVERLAY_ID(frontier_common), OVERLAY_LOAD_NOT_SYNCHRONIZE );

	Castle_InitSub1();

	sys_CreateHeap( HEAPID_BASE_APP, HEAPID_CASTLE, 0x20000 );

	wk = PROC_AllocWork( proc, sizeof(CASTLE_ENEMY_WORK), HEAPID_CASTLE );
	memset( wk, 0, sizeof(CASTLE_ENEMY_WORK) );

	wk->bgl				= GF_BGL_BglIniAlloc( HEAPID_CASTLE );
	wk->proc			= proc;
	castle_call			= (CASTLE_CALL_WORK*)PROC_GetParentWork( proc );
	wk->sv				= castle_call->sv;
	wk->castle_sv		= SaveData_GetCastleData( wk->sv );
	wk->score_sv		= SaveData_GetCastleScore( wk->sv );
	wk->type			= castle_call->type;
	wk->p_ret_work		= &castle_call->ret_work;
	wk->config			= SaveData_GetConfig( wk->sv );			//コンフィグポインタを取得
	wk->p_party			= castle_call->p_e_party;
	wk->p_temoti_flag	= &castle_call->enemy_temoti_flag[0];
	wk->p_level_flag	= &castle_call->enemy_level_flag[0];
	wk->p_tuyosa_flag	= &castle_call->enemy_tuyosa_flag[0];
	wk->p_waza_flag		= &castle_call->enemy_waza_flag[0];
	wk->parent_decide_pos = CASTLE_ENEMY_DECIDE_NONE;
	wk->pair_cp			= castle_call->pair_cp;
	wk->fro_sv			= SaveData_GetFrontier( wk->sv );

	for( i=0; i < CASTLE_RANK_TYPE_MAX; i++ ){
		wk->pair_rank[i] = 1;									//パートナーのランク1オリジン
	}

	//通信タイプでない時
	if( Castle_CommCheck(wk->type) == FALSE ){
		wk->h_max = 3;
	}else{
		wk->h_max = 4;
	}
	wk->modoru_pos = wk->h_max;

	//Bボタン押して、上キーを押した時に戻る位置
	wk->tmp_csr_pos = (wk->modoru_pos - 1);

	wk->pair_csr_pos	= 0;

	Castle_InitSub2( wk );

	if( Castle_CommCheck(wk->type) == TRUE ){
		CommCommandFrontierInitialize( wk );
	}

	//OS_Printf( "CheckCharRest() = %d\n", CheckCharRest() );

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
PROC_RESULT CastleEnemyProc_Main( PROC * proc, int * seq )
{
	CASTLE_ENEMY_WORK* wk  = PROC_GetWork( proc );
	//OS_Printf( "(*seq) = %d\n", (*seq) );

	//相手が「戻る」を選んだ時
	if( wk->pair_modoru_flag == 1 ){

		switch( *seq ){

		//通信終了へ
		case SEQ_GAME_TYPE_SEL:
		//case SEQ_GAME_SEND_RECV:						//08.06.17 送受信中は強制終了しない
			wk->pair_modoru_flag = 0;
			CastleEnemy_PairDecideDel( wk );
			CastleEnemy_SeqSubStatusWinDel( wk );

			//エフェクト中も終了するようにした
			if( wk->p_eff != NULL ){
				CastleObj_Delete( wk->p_eff );
				wk->eff_init_flag = 0;
			}

			//「○さんが戻るを選択しました！」
			CastleTalkWinPut( &wk->bmpwin[ENEMY_BMPWIN_TALK], CONFIG_GetWindowType(wk->config));
			Frontier_PairNameWordSet( wk->wordset, 0 );
			wk->msg_index = Castle_EasyMsg( wk, msg_castle_trainer_08, FONT_TALK );

			NextSeq( wk, seq, SEQ_GAME_END_MULTI );
			break;
		};

	//
	}else if( wk->parent_decide_pos != CASTLE_ENEMY_DECIDE_NONE ){
		switch( *seq ){

		//送受信へ強制移動
		case SEQ_GAME_TYPE_SEL:
		case SEQ_GAME_END_MULTI:						//08.06.17 先に「戻る」中の時は、
			wk->pair_modoru_flag = 0;					//08.06.17 「送受信」に戻す
			CastleEnemy_PairDecideDel( wk );
			CastleEnemy_SeqSubStatusWinDel( wk );
			NextSeq( wk, seq, SEQ_GAME_SEND_RECV );
			break;

		//何もしない
		//case SEQ_GAME_INIT:
		//case SEQ_GAME_SEND_RECV:
		//case SEQ_GAME_END_MULTI:
		//case SEQ_GAME_END:
		//	break;
		};
	}

	switch( *seq ){

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

				//通信タイプの時
				if( Castle_CommCheck(wk->type) == TRUE ){
					NextSeq( wk, seq, SEQ_GAME_END_MULTI );				//(通信)終了へ
				}else{
					NextSeq( wk, seq, SEQ_GAME_END );					//終了へ
				}
			}
		}
		break;

	//-----------------------------------
	//送受信
	case SEQ_GAME_SEND_RECV:
		if( Seq_GameSendRecv(wk) == TRUE ){
			NextSeq( wk, seq, SEQ_GAME_TYPE_SEL );						//タイプ選択中へ
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

	}

	//アイコン上下アニメ
	CastleEnemy_IconSelAnm( wk );

	//CastleObj_IconAnm( wk->p_icon );				//アイコンアニメ
	CLACT_Draw( wk->castle_clact.ClactSet );		//セルアクター常駐関数

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
PROC_RESULT CastleEnemyProc_End( PROC * proc, int * seq )
{
	int i;
	CASTLE_ENEMY_WORK* wk = PROC_GetWork( proc );

	*(wk->p_ret_work) = wk->csr_pos;					//戻り値格納ワークへ代入(カーソル位置)

	OS_Printf( "*(wk->p_ret_work) = %d\n", *(wk->p_ret_work) );

	DellVramTransferManager();

	CastleCommon_Delete( wk );							//削除処理

	PROC_FreeWork( proc );								//ワーク開放

	sys_VBlankFuncChange( NULL, NULL );					//VBlankセット
	sys_DeleteHeap( HEAPID_CASTLE );

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
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameInit( CASTLE_ENEMY_WORK* wk )
{
	switch( wk->sub_seq ){

	//コマンド設定後の同期
	case 0:
		if( Castle_CommCheck(wk->type) == TRUE ){
			CommToolTempDataReset();
			CommTimingSyncStart( DBC_TIM_CASTLE_ENEMY_INIT );
		}
		wk->sub_seq++;
		break;

	//タイミングコマンドが届いたか確認
	case 1:
		if( Castle_CommCheck(wk->type) == TRUE ){
			if( CommIsTimingSync(DBC_TIM_CASTLE_ENEMY_INIT) == TRUE ){
				CommToolTempDataReset();
				wk->sub_seq++;
			}
		}else{
			wk->sub_seq++;
		}
		break;
		
	//基本情報送信
	case 2:
		if( Castle_CommCheck(wk->type) == TRUE ){
			//基本情報を送信
			if( CastleEnemy_CommSetSendBuf(wk,CASTLE_COMM_ENEMY_PAIR,0) == TRUE ){
				wk->sub_seq++;
			}
		}else{
			TypeSelInit( wk );

			WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, 
							WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC*3, HEAPID_CASTLE );
			wk->sub_seq++;
		}
		break;

	//ブラックイン
	case 3:
		if( Castle_CommCheck(wk->type) == TRUE ){
			if( wk->recieve_count >= CASTLE_COMM_PLAYER_NUM ){
				wk->recieve_count = 0;

				TypeSelInit( wk );

				WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, 
								WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC*3, HEAPID_CASTLE );

				wk->sub_seq++;
			}
		}else{
			wk->sub_seq++;
		}
		break;
		
	//フェード終了待ち
	case 4:
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
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static void TypeSelInit( CASTLE_ENEMY_WORK* wk )
{
	u16 x,y,offset_x,offset_y,pair_offset_x,pair_offset_y,icon_offset_x,ball_offset_x;
	GF_BGL_BMPWIN* win;

	//通信しているかで表示のオフセットが変わる
	Castle_GetOffset( wk, &offset_x, &offset_y, &pair_offset_x, &pair_offset_y );

	///////////////////////////////////////////////////////////////////////////////
	win = &wk->bmpwin[ENEMY_BMPWIN_TR1];
	GF_BGL_BmpWinDataFill( win, FBMP_COL_NULL );					//塗りつぶし
	CastleEnemy_SeqSubNameWrite( wk, win );							//プレイヤー、パートナー名表示
	CastleEnemy_SeqSubCPWrite( wk, win );							//CP表示

	///////////////////////////////////////////////////////////////////////////////
	//HPとLV表示
	Castle_PokeHpMsgWrite( wk, &wk->bmpwin[ENEMY_BMPWIN_HP] );
	Castle_PokeLvMsgWrite( wk, &wk->bmpwin[ENEMY_BMPWIN_LV] );

	///////////////////////////////////////////////////////////////////////////////
	CastleEnemy_Default_Write( wk );								//TALKMENU,もどる

	GF_Disp_DispOn();
	return;
}

//--------------------------------------------------------------
/**
 * @brief	ゲームタイプ選択中
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
enum{
	SEQ_SUB_START = 0,
	SEQ_SUB_TYPE_SEL,
	SEQ_SUB_MENU_MAIN,
	//////////////////////////////////////////////////////////////////
	SEQ_SUB_TEMOTI_YES_NO,
	//////////////////////////////////////////////////////////////////
	SEQ_SUB_LEVEL_LIST,
	SEQ_SUB_LEVEL_YES_NO,
	//////////////////////////////////////////////////////////////////
	SEQ_SUB_INFO_LIST,
	SEQ_SUB_INFO_TUYOSA_YES_NO,
	SEQ_SUB_INFO_WAZA_YES_NO,
	SEQ_SUB_INFO_RANKUP_YES_NO,
	SEQ_SUB_INFO_RANKUP_DECIDE_EFF_WAIT,
	//////////////////////////////////////////////////////////////////
	SEQ_SUB_DECIDE_EFF_WAIT,				//決定エフェクト待ち
	SEQ_SUB_DECIDE_EFF_WAIT_NEXT_WIN_OFF,	//決定エフェクト待ち→ウィンドオフへ
	SEQ_SUB_DECIDE_EFF_WAIT_WIN_OFF,		//決定エフェクト待ち→ウィンドオフ、最初の画面に戻る
	//////////////////////////////////////////////////////////////////
	SEQ_SUB_INFO_KEY_WAIT_LIST_RETURN,		//キー待ちして、じょうほうリストに戻る
	SEQ_SUB_KEY_WAIT,						//キー待ち、最初の画面に戻る
	SEQ_SUB_INFO_TUYOSA_KEY_WAIT_WIN_OFF,	//キー待ち、ステータスウィンドウ消す、BASICリストへ戻る
	SEQ_SUB_INFO_WAZA_KEY_WAIT_WIN_OFF,		//キー待ち、ステータスウィンドウ消す、BASICリストへ戻る
};

static BOOL Seq_GameTypeSel( CASTLE_ENEMY_WORK* wk )
{
	u8 check_pos;
	u16 now_cp,check_cp;
	int i,ans;
	u32 color,now_rank,ret;
	POKEMON_PARAM* poke;

	switch( wk->sub_seq ){

	case SEQ_SUB_START:
		wk->recover = 0;
		wk->sub_seq = SEQ_SUB_TYPE_SEL;

		//ランクアップした時に戻り先が特殊かチェック
		if( wk->recover_flag == RECOVER_NORMAL ){
			BmpTalkWinClearSub( &wk->bmpwin[ENEMY_BMPWIN_TALK] );
			CastleEnemy_Default_Write( wk );								//TALKMENU,もどる

		//ランクアップした時に戻り先が特殊かチェック
		}else if( wk->recover_flag == RECOVER_RANKUP ){
			BmpTalkWinClearSub( &wk->bmpwin[ENEMY_BMPWIN_TALK] );
			CastleEnemy_Basic2_Write( wk );		//TALK,BASIC2

			//ランクアップエフェクト
			CastleObj_RankUpEff( wk->p_eff_rankup, CASTLE_ENEMY_RANKUP_X, CASTLE_ENEMY_RANKUP_Y );

			wk->sub_seq = SEQ_SUB_INFO_LIST;

		//通信で自分がステータスを開放したかフラグをチェック
		}else if( wk->recover_flag == RECOVER_STATUS ){
			wk->sub_seq = SEQ_SUB_DECIDE_EFF_WAIT_WIN_OFF;
		}

		wk->recover_flag = RECOVER_NONE;			//TYPE_SELに戻った時の分岐フラグクリア
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//タイプを選択中
	case SEQ_SUB_TYPE_SEL:
		CsrMove( wk, sys.trg );

		//決定ボタンを押した時
		if( sys.trg & PAD_BUTTON_A ){
			Snd_SePlay( SEQ_SE_DP_SELECT );

			//「もどる」
			if( wk->csr_pos >= wk->modoru_pos ){
				return TRUE;
			}else{
				CastleEnemy_Default_Del( wk );		//TALKMENU,もどる
				CastleEnemy_Basic_Write( wk );		//TALKMENU2,BASIC
				wk->sub_seq = SEQ_SUB_MENU_MAIN;
				break;
			}

		}else if( sys.trg & PAD_BUTTON_B ){
			//すでに「もどる」位置の時は何もしない
			if( wk->csr_pos != wk->modoru_pos ){
				Snd_SePlay( SEQ_SE_DP_SELECT );
				wk->csr_pos = wk->modoru_pos;
				CsrMoveSub( wk );
			}
			break;
		}

		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//やることを選択中
	case SEQ_SUB_MENU_MAIN:
		ret = BmpListMain( wk->lw );
		Castle_ListSeCall( ret, SEQ_SE_DP_SELECT );
		BmpListDirectPosGet( wk->lw, &wk->list_csr_pos );					//カーソル位置

		switch( ret ){

		case BMPLIST_NULL:
			break;

		case BMPLIST_CANCEL:
			CastleEnemy_Basic_Del( wk );			//TALKMENU2,BASIC
			CastleEnemy_Default_Write( wk );		//TALKMENU,もどる
			wk->sub_seq = SEQ_SUB_START;
			break;

		//しょうたい
		case FC_PARAM_TEMOTI:
			wk->parent_decide_type = ret;			//パラメータ

			//正体が公開されていなかったら
			if( wk->p_temoti_flag[GetCsrPokePos(wk->h_max,wk->csr_pos)] == 0 ){
				CastleEnemy_Basic_Del( wk );		//TALKMENU2,BASIC

				//「ポケモンの　しょうたいが　わかります　○ＣＰを　つかい　ますか？」
				CastleEnemy_SeqSubTalkWinOn( wk );
				Castle_SetNumber(	wk, 0, CP_USE_TEMOTI, CASTLE_KETA_CP, 
									NUMBER_DISPTYPE_LEFT );
				wk->msg_index = Castle_EasyMsg( wk, msg_castle_trainer_04, FONT_TALK );
				Castle_SetMenu2( wk );						//「はい・いいえ」
				wk->sub_seq = SEQ_SUB_TEMOTI_YES_NO;
			}else{
				CastleEnemy_Basic_Del( wk );		//TALKMENU2,BASIC

				poke =  PokeParty_GetMemberPointer( wk->p_party, 
													GetCsrPokePos(wk->h_max,wk->csr_pos) );

				/*「ポケモンの　しょうたいは　○　でした！」*/
				CastleEnemy_SeqSubTalkWinOn( wk );
				Castle_SetPokeName( wk, 0, PPPPointerGet(poke) );
				wk->msg_index = Castle_EasyMsg( wk, msg_castle_trainer_04_03, FONT_TALK );
				wk->sub_seq = SEQ_SUB_KEY_WAIT;
				//何もしない(リスト継続)
				//Snd_SeStopBySeqNo( SEQ_SE_DP_SELECT, 0 );
				//Snd_SePlay( SEQ_SE_DP_BOX03 );
			}
			break;

		//レベル
		case FC_PARAM_LEVEL:
			wk->parent_decide_type = ret;		//パラメータ
			CastleEnemy_Basic_Del( wk );		//TALKMENU2,BASIC

			poke =  PokeParty_GetMemberPointer( wk->p_party, 
												GetCsrPokePos(wk->h_max,wk->csr_pos) );

			//「○の　レベルを　かえられます　どちらに　しますか？」
			CastleEnemy_SeqSubTalkWinOn( wk );
			Castle_SetPokeName( wk, 0, PPPPointerGet(poke) );
			wk->msg_index = Castle_EasyMsg( wk, msg_castle_trainer_05, FONT_TALK );
			Castle_SetMenu3( wk );				//「レベル＋５・レベルー５・やめる」
			wk->sub_seq = SEQ_SUB_LEVEL_LIST;
			break;

		//じょうほう
		case FC_PARAM_INFO:
			CastleEnemy_Basic_Del( wk );		//TALKMENU2,BASIC
			CastleEnemy_Basic2_Write( wk );		//TALK,BASIC2
			wk->sub_seq = SEQ_SUB_INFO_LIST;
			break;

		//とじる
		case FC_PARAM_TOZIRU:
			CastleEnemy_Basic_Del( wk );		//TALKMENU2,BASIC
			CastleEnemy_Default_Write( wk );	//TALKMENU,もどる
			wk->sub_seq = SEQ_SUB_START;
			break;
		};

		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//「てもち」「はい・いいえ」メニュー処理
	case SEQ_SUB_TEMOTI_YES_NO:
		ret = BmpMenuMain( wk->mw );

		switch( ret ){

		case BMPMENU_NULL:
			break;

		//「はい」
		case 0:
			CastleEnemy_SeqSubMenuWinClear( wk );
			BmpTalkWinClearSub( &wk->bmpwin[ENEMY_BMPWIN_TALK] );

			now_cp = FrontierRecord_Get(wk->fro_sv, CastleScr_GetCPRecordID(wk->type),
										//FRONTIER_RECORD_NOT_FRIEND );		//現在のCP
										Frontier_GetFriendIndex(CastleScr_GetCPRecordID(wk->type)));

			//CPが足りない時
			if( now_cp < CP_USE_TEMOTI ){
				//改行が入っているので相談
				CastleEnemy_SeqSubTalkWinOn( wk );
				wk->msg_index = Castle_EasyMsg( wk, msg_castle_trainer_03, FONT_TALK );
				wk->sub_seq = SEQ_SUB_KEY_WAIT;
				break;
			}

			if( Castle_CommCheck(wk->type) == FALSE ){

				Castle_CPRecord_Sub( wk->fro_sv, wk->type, CP_USE_TEMOTI );
				CastleEnemy_SeqSubCPWrite( wk, &wk->bmpwin[ENEMY_BMPWIN_TR1] );

				//モンスターボールのアイコンをポケモンアイコンにする
				//レベル、性別、体力を表示
				CastleEnemy_SeqSubTemoti( wk, wk->csr_pos );
			
				wk->sub_seq = SEQ_SUB_DECIDE_EFF_WAIT;
			}else{
				wk->send_req = 1;
				return TRUE;
			}
			break;
			
		//「いいえ」
		case 1:
		case BMPMENU_CANCEL:
			CastleEnemy_SeqSubMenuWinClear( wk );
			BmpTalkWinClearSub( &wk->bmpwin[ENEMY_BMPWIN_TALK] );
			CastleEnemy_Basic_Write( wk );		//TALKMENU2,BASIC
			wk->sub_seq = SEQ_SUB_MENU_MAIN;
			break;
		};

		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//「レベル」メニュー処理
	case SEQ_SUB_LEVEL_LIST:
		ret = BmpMenuMain( wk->mw );

		switch( ret ){

		case BMPMENU_NULL:
			break;

		//「レベル＋５」
		case 0:
			CastleEnemy_SeqSubMenuWinClear( wk );
			now_cp = FrontierRecord_Get(wk->fro_sv, CastleScr_GetCPRecordID(wk->type),
									Frontier_GetFriendIndex(CastleScr_GetCPRecordID(wk->type)) );

			//すでに+5していたら出来ない
			if( wk->p_level_flag[GetCsrPokePos(wk->h_max,wk->csr_pos)] == 1 ){
				CastleEnemy_SeqSubTalkWinOn( wk );
				wk->msg_index = Castle_EasyMsg( wk, msg_castle_trainer_05_04, FONT_TALK );
				wk->sub_seq = SEQ_SUB_KEY_WAIT;
				break;
			}else{
				CastleEnemy_SeqSubLevelMsg( wk, 1 );
				wk->sub_seq = SEQ_SUB_LEVEL_YES_NO;
			}
			break;
			
		//「レベル－５」
		case 1:
			CastleEnemy_SeqSubMenuWinClear( wk );
			now_cp = FrontierRecord_Get(wk->fro_sv, CastleScr_GetCPRecordID(wk->type),
									Frontier_GetFriendIndex(CastleScr_GetCPRecordID(wk->type)) );

			//すでに-5していたら出来ない
			if( wk->p_level_flag[GetCsrPokePos(wk->h_max,wk->csr_pos)] == 2 ){
				CastleEnemy_SeqSubTalkWinOn( wk );
				wk->msg_index = Castle_EasyMsg( wk, msg_castle_trainer_05_05, FONT_TALK );
				wk->sub_seq = SEQ_SUB_KEY_WAIT;
				break;
			}else{
				CastleEnemy_SeqSubLevelMsg( wk, 2 );
				wk->sub_seq = SEQ_SUB_LEVEL_YES_NO;
			}
			break;
			
		case 2:
		case BMPMENU_CANCEL:
			CastleEnemy_SeqSubMenuWinClear( wk );
			BmpTalkWinClearSub( &wk->bmpwin[ENEMY_BMPWIN_TALK] );
			CastleEnemy_Basic_Write( wk );		//TALKMENU2,BASIC
			wk->sub_seq = SEQ_SUB_MENU_MAIN;
			break;
		};

		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//「レベル」「はい・いいえ」メニュー処理
	case SEQ_SUB_LEVEL_YES_NO:
		ret = BmpMenuMain( wk->mw );

		switch( ret ){

		case BMPMENU_NULL:
			break;

		//「はい」
		case 0:
			CastleEnemy_SeqSubMenuWinClear( wk );

			now_cp = FrontierRecord_Get(wk->fro_sv, CastleScr_GetCPRecordID(wk->type),
									Frontier_GetFriendIndex(CastleScr_GetCPRecordID(wk->type)) );
			check_cp = CastleEnemy_GetLvCP( wk->level_sel );			//使用CP

			//CPが足りない時
			if( now_cp < check_cp ){
				CastleEnemy_SeqSubTalkWinOn( wk );
				wk->msg_index = Castle_EasyMsg( wk, msg_castle_trainer_03, FONT_TALK );
				wk->sub_seq = SEQ_SUB_KEY_WAIT;
				return FALSE;
			}

			//代入されていたら処理が変わるところが、通信、非通信で特にないのでセットして問題なし
			wk->parent_decide_updown = wk->level_sel;

			if( Castle_CommCheck(wk->type) == FALSE ){
				BmpTalkWinClearSub( &wk->bmpwin[ENEMY_BMPWIN_TALK] );
				Castle_CPRecord_Sub(wk->fro_sv, wk->type,
									CastleEnemy_GetLvCP(wk->level_sel));
				CastleEnemy_SeqSubCPWrite( wk, &wk->bmpwin[ENEMY_BMPWIN_TR1] );
				CastleEnemy_SeqSubLevel( wk, wk->csr_pos, wk->level_sel );
				wk->sub_seq = SEQ_SUB_DECIDE_EFF_WAIT;
			}else{
				wk->send_req = 1;
				return TRUE;
			}
			break;
			
		//「いいえ」
		case 1:
		case BMPMENU_CANCEL:
			CastleEnemy_SeqSubMenuWinClear( wk );
			BmpTalkWinClearSub( &wk->bmpwin[ENEMY_BMPWIN_TALK] );
			CastleEnemy_Basic_Write( wk );		//TALKMENU2,BASIC
			wk->sub_seq = SEQ_SUB_MENU_MAIN;
			break;
		};

		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//「じょうほう」リスト処理
	case SEQ_SUB_INFO_LIST:
		ret = BmpListMain( wk->lw );
		Castle_ListSeCall( ret, SEQ_SE_DP_SELECT );
		BmpListDirectPosGet( wk->lw, &wk->list_csr_pos );					//カーソル位置

		switch( ret ){

		case BMPLIST_NULL:
			break;

		case BMPLIST_CANCEL:
			BmpTalkWinClearSub( &wk->bmpwin[ENEMY_BMPWIN_TALK] );
			CastleEnemy_Basic2_Del( wk );		//TALK,BASIC2
			CastleEnemy_Basic_Write( wk );		//TALKMENU2,BASIC
			wk->sub_seq = SEQ_SUB_MENU_MAIN;
			break;

		case FC_PARAM_TUYOSA:
			wk->parent_decide_type = ret;		//パラメータ
			CastleEnemy_Basic2_Del( wk );		//TALK,BASIC2

			//つよさが公開されていなかったら
			if( wk->p_tuyosa_flag[GetCsrPokePos(wk->h_max,wk->csr_pos)] == 0 ){
				//「ポケモンの　つよさが　わかります　○ＣＰを　つかい　ますか？」
				CastleEnemy_SeqSubTalkWinOn( wk );
				Castle_SetNumber( wk, 0, CP_USE_TUYOSA, CASTLE_KETA_CP, NUMBER_DISPTYPE_LEFT);
				wk->msg_index = Castle_EasyMsg( wk, msg_castle_trainer_06, FONT_TALK );
				Castle_SetMenu2( wk );						//「はい・いいえ」
				wk->sub_seq = SEQ_SUB_INFO_TUYOSA_YES_NO;
			}else{
				CastleEnemy_SeqSubTuyosa( wk, wk->csr_pos );
				wk->sub_seq = SEQ_SUB_INFO_TUYOSA_KEY_WAIT_WIN_OFF;
			}
			break;

		case FC_PARAM_WAZA:
			wk->parent_decide_type = ret;		//パラメータ
			CastleEnemy_Basic2_Del( wk );		//TALK,BASIC2

			//ランクが足りない時
			now_rank = Castle_GetRank( wk->sv, wk->type, CASTLE_RANK_TYPE_INFO );
			if( now_rank == 1 ){
				wk->msg_index = Castle_EasyMsg( wk, msg_castle_trainer_09_10, FONT_TALK );
				wk->sub_seq = SEQ_SUB_INFO_KEY_WAIT_LIST_RETURN;
				return FALSE;
			}

			//会話ウィンドウ表示
			//CastleTalkWinPut( &wk->bmpwin[ENEMY_BMPWIN_TALK], CONFIG_GetWindowType(wk->config));

			//技が公開されていなかったら
			if( wk->p_waza_flag[GetCsrPokePos(wk->h_max,wk->csr_pos)] == 0 ){
				//「もっている　わざが　わかります　○ＣＰを　つかい　ますか？」
				CastleEnemy_SeqSubTalkWinOn( wk );
				Castle_SetNumber( wk, 0, CP_USE_WAZA, CASTLE_KETA_CP, NUMBER_DISPTYPE_LEFT);
				wk->msg_index = Castle_EasyMsg( wk, msg_castle_trainer_07, FONT_TALK );
				Castle_SetMenu2( wk );						//「はい・いいえ」
				wk->sub_seq = SEQ_SUB_INFO_WAZA_YES_NO;
			}else{
				CastleEnemy_SeqSubWaza( wk, wk->csr_pos );	//技名、ＰＰ名を表示
				wk->sub_seq = SEQ_SUB_INFO_WAZA_KEY_WAIT_WIN_OFF;
			}
			break;

		case FC_PARAM_INFO_RANKUP:
			now_rank = Castle_GetRank( wk->sv, wk->type, CASTLE_RANK_TYPE_INFO );
				
			//ランクアップＭＡＸを選択
			//if( now_rank == CASTLE_RANK_MAX ){
			if( now_rank == (CASTLE_RANK_MAX-1) ){				//ここだけ2が最高なので！
				//「これ　いじょう　ランクは　あがりません！」
				Snd_SeStopBySeqNo( SEQ_SE_DP_SELECT, 0 );
				Snd_SePlay( SEQ_SE_DP_BOX03 );
				//wk->msg_index = Castle_EasyMsg( wk, msg_castle_trainer_09_08, FONT_TALK );
				//wk->sub_seq = SEQ_SUB_INFO_KEY_WAIT_LIST_RETURN;
			}else{
				wk->parent_decide_type = ret;		//パラメータ
				CastleEnemy_Basic2_Del( wk );		//TALK,BASIC2

				//必要なCPポイント取得
				now_cp = FrontierRecord_Get(wk->fro_sv, CastleScr_GetCPRecordID(wk->type),
										Frontier_GetFriendIndex(CastleScr_GetCPRecordID(wk->type)));

				/*「○ＣＰを使ってランクをあげますか？」*/
				Castle_SetNumber(wk, 0, INFO_RANKUP_POINT, 
								 CASTLE_KETA_CP, NUMBER_DISPTYPE_LEFT );
				wk->msg_index = Castle_EasyMsg( wk, msg_castle_trainer_11_01, FONT_TALK );
				Castle_SetMenu2( wk );						//「はい・いいえ」
				wk->sub_seq = SEQ_SUB_INFO_RANKUP_YES_NO;
			}
			break;
		};
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//「つよさ」「はい・いいえ」メニュー処理
	case SEQ_SUB_INFO_TUYOSA_YES_NO:
		ret = BmpMenuMain( wk->mw );

		switch( ret ){

		case BMPMENU_NULL:
			break;

		//「はい」
		case 0:
			ans = CastleEnemy_SeqSubTuyosaWazaYes( wk, CP_USE_TUYOSA, msg_castle_trainer_06_05 );
			if( ans == TRUE ){
				return TRUE;
			}
			break;
			
		//「いいえ」
		case 1:
		case BMPMENU_CANCEL:
			CastleEnemy_SeqSubMenuWinClear( wk );
			CastleEnemy_Basic2_Write( wk );		//TALK,BASIC2
			wk->sub_seq = SEQ_SUB_INFO_LIST;
			break;
		};

		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//「わざ」「はい・いいえ」メニュー処理
	case SEQ_SUB_INFO_WAZA_YES_NO:
		ret = BmpMenuMain( wk->mw );

		switch( ret ){

		case BMPMENU_NULL:
			break;

		//「はい」
		case 0:
			ans = CastleEnemy_SeqSubTuyosaWazaYes( wk, CP_USE_WAZA, msg_castle_trainer_07_03 );
			if( ans == TRUE ){
				return TRUE;
			}
			break;
			
		//「いいえ」
		case 1:
		case BMPMENU_CANCEL:
			CastleEnemy_SeqSubMenuWinClear( wk );
			CastleEnemy_Basic2_Write( wk );		//TALK,BASIC2
			wk->sub_seq = SEQ_SUB_INFO_LIST;
			break;
		};

		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//「ランクアップ」「はい・いいえ」メニュー処理
	case SEQ_SUB_INFO_RANKUP_YES_NO:
		ret = BmpMenuMain( wk->mw );

		switch( ret ){

		case BMPMENU_NULL:
			break;

		//「はい」
		case 0:
			CastleEnemy_SeqSubMenuWinClear( wk );

			now_cp = FrontierRecord_Get(wk->fro_sv, CastleScr_GetCPRecordID(wk->type),
										Frontier_GetFriendIndex(CastleScr_GetCPRecordID(wk->type)));
			now_rank = Castle_GetRank( wk->sv, wk->type, CASTLE_RANK_TYPE_INFO );

			//CPの比較
			if( now_cp < INFO_RANKUP_POINT ){
				//「ＣＰが　たりません」
				CastleEnemy_SeqSubMenuWinClear( wk );
				CastleTalkWinPut( &wk->bmpwin[ENEMY_BMPWIN_TALK], CONFIG_GetWindowType(wk->config));
				wk->msg_index = Castle_EasyMsg( wk, msg_castle_trainer_07_03, FONT_TALK );
				wk->sub_seq = SEQ_SUB_INFO_KEY_WAIT_LIST_RETURN;
				break;
			}
						
			//ランクアップ
			if( Castle_CommCheck(wk->type) == FALSE ){
				CastleRank_SeqSubRankUp( wk, wk->csr_pos, FC_PARAM_INFO_RANKUP );
				wk->sub_seq = SEQ_SUB_INFO_RANKUP_DECIDE_EFF_WAIT;
				break;
			}else{
				wk->send_req		= 1;
				return TRUE;
			}

			break;
			
		//「いいえ」
		case 1:
		case BMPMENU_CANCEL:
			CastleEnemy_SeqSubMenuWinClear( wk );
			//BmpTalkWinClear( &wk->bmpwin[ENEMY_BMPWIN_TALK], WINDOW_TRANS_OFF );
			CastleEnemy_Basic2_Write( wk );		//TALK,BASIC2
			wk->sub_seq = SEQ_SUB_INFO_LIST;
			break;
		};
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//情報ランクアップ決定エフェクト待ち
	case SEQ_SUB_INFO_RANKUP_DECIDE_EFF_WAIT:
		if( sys.trg & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL) ){
		//if( CastleEnemy_DecideEff(wk,wk->csr_pos,wk->parent_decide_type) == TRUE ){
			CastleEnemy_SeqSubMenuWinClear( wk );
			CastleEnemy_Basic2_Write( wk );		//TALK,BASIC2

			//ランクアップエフェクト
			CastleObj_RankUpEff( wk->p_eff_rankup, CASTLE_ENEMY_RANKUP_X, CASTLE_ENEMY_RANKUP_Y );

			wk->sub_seq = SEQ_SUB_INFO_LIST;
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//決定エフェクト待ち
	case SEQ_SUB_DECIDE_EFF_WAIT:
		if( CastleEnemy_DecideEff(wk,wk->csr_pos,wk->parent_decide_type) == TRUE ){
			wk->sub_seq = SEQ_SUB_KEY_WAIT;
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//決定エフェクト待ち→ウィンドオフへ
	case SEQ_SUB_DECIDE_EFF_WAIT_NEXT_WIN_OFF:
		if( CastleEnemy_DecideEff(wk,wk->csr_pos,wk->parent_decide_type) == TRUE ){
			wk->sub_seq = SEQ_SUB_DECIDE_EFF_WAIT_WIN_OFF;
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//決定エフェクト待ち→ウィンドオフして最初の画面に戻る
	case SEQ_SUB_DECIDE_EFF_WAIT_WIN_OFF:
		if( wk->parent_decide_type == FC_PARAM_TUYOSA ){
			wk->sub_seq = SEQ_SUB_INFO_TUYOSA_KEY_WAIT_WIN_OFF;
		}else{
			wk->sub_seq = SEQ_SUB_INFO_WAZA_KEY_WAIT_WIN_OFF;
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//キー待ちして、じょうほうリストに戻る
	case SEQ_SUB_INFO_KEY_WAIT_LIST_RETURN:
		if( sys.trg & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL) ){
			Snd_SePlay( SEQ_SE_DP_SELECT );
			CastleEnemy_Basic2_Write( wk );		//TALK,BASIC2
			wk->sub_seq = SEQ_SUB_INFO_LIST;
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//キー待ち
	case SEQ_SUB_KEY_WAIT:
		if( sys.trg & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL) ){
			Snd_SePlay( SEQ_SE_DP_SELECT );
			BmpTalkWinClearSub( &wk->bmpwin[ENEMY_BMPWIN_TALK] );
			CastleEnemy_Default_Write( wk );		//TALKMENU,もどる
			wk->sub_seq = SEQ_SUB_START;
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//つよさ：キー待ちしてステータスなどのウィンドウを消して、BASICリストへ戻る
	case SEQ_SUB_INFO_TUYOSA_KEY_WAIT_WIN_OFF:
		if( sys.trg & PAD_KEY_LEFT ){
			CastleEnemy_TuyosaWinChg( wk, -1 );
		}else if( sys.trg & PAD_KEY_RIGHT ){
			CastleEnemy_TuyosaWinChg( wk, 1 );
		}else if( sys.trg & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL) ){
			Snd_SePlay( SEQ_SE_DP_SELECT );
			CastleEnemy_SeqSubStatusWinDel( wk );
			CastleEnemy_Basic_Write( wk );		//TALKMENU2,BASIC
			wk->sub_seq = SEQ_SUB_MENU_MAIN;
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//わざ：キー待ちしてステータスなどのウィンドウを消して、BASICリストへ戻る
	case SEQ_SUB_INFO_WAZA_KEY_WAIT_WIN_OFF:
		if( sys.trg & PAD_KEY_LEFT ){
			CastleEnemy_WazaWinChg( wk, -1 );
		}else if( sys.trg & PAD_KEY_RIGHT ){
			CastleEnemy_WazaWinChg( wk, 1 );
		}else if( sys.trg & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL) ){
			Snd_SePlay( SEQ_SE_DP_SELECT );
			CastleEnemy_SeqSubStatusWinDel( wk );
			CastleEnemy_Basic_Write( wk );		//TALKMENU2,BASIC
			wk->sub_seq = SEQ_SUB_MENU_MAIN;
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	送受信
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameSendRecv( CASTLE_ENEMY_WORK* wk )
{
	u8 sel_pos;

	switch( wk->sub_seq ){

	case 0:
		wk->recover_flag = RECOVER_NORMAL;	//ランクアップした時に戻り先が特殊フラグOFF

		//選んだリクエスト(やりたいこと)を送信
		if( CastleEnemy_CommSetSendBuf(wk,CASTLE_COMM_ENEMY_UP_TYPE,wk->csr_pos) == TRUE ){
			wk->send_req = 0;
			wk->sub_seq++;
		}
		break;

	case 1:
		if( wk->parent_decide_pos == CASTLE_ENEMY_DECIDE_NONE ){
			break;
		}

		//親が決めたランクとLRをランクアップさせる
		wk->recieve_count = 0;

		//ランクを上げる処理とそれ以外で分けている
		if( wk->parent_decide_type == FC_PARAM_INFO_RANKUP ){ 
			CastleRank_SeqSubRankUp( wk, wk->parent_decide_pos, FC_PARAM_INFO_RANKUP );
		}else{
			CastleEnemy_SeqSubRankUp( wk, wk->parent_decide_pos, wk->parent_decide_type );
		}

		wk->sub_seq++;
		break;

	//決定エフェクト待ち
	case 2:
		sel_pos = GetCommSelCsrPos( wk->modoru_pos, wk->parent_decide_pos );	//カーソル位置取得
		if( CastleEnemy_DecideEff(wk,sel_pos,wk->parent_decide_type) == TRUE ){
			wk->wait = CASTLE_COMM_WAIT;
			wk->sub_seq++;
		}
		break;

	case 3:
		//if( sys.trg & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL) ){
		//Snd_SePlay( SEQ_SE_DP_SELECT );
		wk->wait--;
		if( wk->wait == 0 ){
			//同期開始
			CommToolTempDataReset();
			CommTimingSyncStart( DBC_TIM_CASTLE_ENEMY_UP );
			wk->sub_seq++;
			break;
		}
		break;

	//同期
	case 4:
		//タイミングコマンドが届いたか確認
		if( CommIsTimingSync(DBC_TIM_CASTLE_ENEMY_UP) == TRUE ){
			wk->sub_seq++;
		}
		break;

	case 5:
		//エフェクトタイミングが2箇所になったので追加
		sel_pos = GetCommSelCsrPos( wk->modoru_pos, wk->parent_decide_pos );	//カーソル位置取得
		if( CastleEnemy_DecideEff2(wk,sel_pos,wk->parent_decide_type) == TRUE ){
			CommToolTempDataReset();
			CommToolInitialize( HEAPID_CASTLE );	//timingSyncEnd=0xff

			//08.03.28削除
			//BmpTalkWinClearSub( &wk->bmpwin[ENEMY_BMPWIN_TALK] );
			//CastleEnemy_SeqSubStatusWinDel( wk );

			//CastleEnemy_SeqSubAllInfoWrite( wk );								//情報更新
			wk->parent_decide_pos = CASTLE_ENEMY_DECIDE_NONE;

#if 0
			//08.03.28削除
			//ランクアップした時に戻り先が特殊かチェック
			if( wk->recover_flag == RECOVER_NORMAL ){
				CastleEnemy_Default_Write( wk );								//TALKMENU,もどる
			}
#endif

			//「戻る｣リクエストが先にきていても、なかったことにする
			wk->pair_modoru_flag = 0;
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
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameEndMulti( CASTLE_ENEMY_WORK* wk )
{
	int i;

	switch( wk->sub_seq ){

	//「戻る」送信
	case 0:
		if( CastleEnemy_CommSetSendBuf(wk,CASTLE_COMM_ENEMY_MODORU,0) == TRUE ){
			wk->wait = CASTLE_MODORU_WAIT;
			wk->sub_seq++;
		}
		break;

	//同期開始
	case 1:
		if( wk->wait > 0 ){
			wk->wait--;
		}

		if( wk->wait == 0 ){
			CommToolTempDataReset();
			CommTimingSyncStart( DBC_TIM_CASTLE_ENEMY_END );
			wk->sub_seq++;
		}
		break;

	//同期待ち
	case 2:
		//タイミングコマンドが届いたか確認
		if( CommIsTimingSync(DBC_TIM_CASTLE_ENEMY_END) == TRUE ){
			CommToolTempDataReset();
			BmpTalkWinClearSub( &wk->bmpwin[ENEMY_BMPWIN_TALK] );
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
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameEnd( CASTLE_ENEMY_WORK* wk )
{
	int i;

	switch( wk->sub_seq ){

	//フェードアウト
	case 0:
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
					WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_CASTLE );

		wk->sub_seq++;
		break;

	//フェード終了待ち
	case 1:
		if( WIPE_SYS_EndCheck() == TRUE ){
			return TRUE;
		}
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
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleCommon_Delete( CASTLE_ENEMY_WORK* wk )
{
	u8 e_max;
	int i,j;

	CastleObj_Delete( wk->p_poke_sel );
	CastleObj_Delete( wk->p_pair_poke_sel );
	//CastleObj_Delete( wk->p_eff );		//いらない
	CastleObj_Delete( wk->p_eff_rankup );
	
	//つよさ、わざ
	for( i=0; i < CASTLE_COMM_POKE_TOTAL_NUM ;i++ ){
		for( j=0; j < 2 ;j++ ){
			CastleObj_Delete( wk->p_hata[i][j] );
		}
	}

	e_max = Castle_GetEnemyPokeNum( wk->type, CASTLE_FLAG_TOTAL );
	for( i=0; i < e_max ;i++ ){
		CastleObj_Delete( wk->p_ball[i] );
		CastleObj_Delete( wk->p_icon[i] );
		CastleObj_Delete( wk->p_hp[i] );
	}

	//通信アイコン削除
	WirelessIconEasyEnd();

	//パレットフェード開放
	PaletteFadeWorkAllocFree( wk->pfd, FADE_MAIN_OBJ );
	PaletteFadeWorkAllocFree( wk->pfd, FADE_MAIN_BG );

	//パレットフェードシステム開放
	PaletteFadeFree( wk->pfd );
	wk->pfd = NULL;

	CastleClact_DeleteCellObject(&wk->castle_clact);		//2Dオブジェクト関連領域開放

	MSGMAN_Delete( wk->msgman );						//メッセージマネージャ開放
	WORDSET_Delete( wk->wordset );
	STRBUF_Delete( wk->msg_buf );						//メッセージバッファ開放
	STRBUF_Delete( wk->tmp_buf );						//メッセージバッファ開放
	NUMFONT_Delete( wk->num_font );

	for( i=0; i < CASTLE_MENU_BUF_MAX ;i++ ){
		STRBUF_Delete( wk->menu_buf[i] );				//メニューバッファ開放
	}

	CastleExitBmpWin( wk->bmpwin, CASTLE_BMP_ENEMY );	//BMPウィンドウ開放
	Castle_BgExit( wk->bgl );							//BGL削除
	ArchiveDataHandleClose( wk->hdl );

	return;
}

//--------------------------------------------------------------
/**
 * @brief	復帰
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_Recover( CASTLE_ENEMY_WORK* wk )
{
	return;
}

//--------------------------------------------------------------
/**
 * @brief	共通 初期化1
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_InitSub1( void )
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
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_InitSub2( CASTLE_ENEMY_WORK* wk )
{
	u32 start_x,start_y;
	u32 hp,hp_max;
	u8 e_max;
	u16 x,y,offset_x,offset_y,pair_offset_x,pair_offset_y,icon_offset_x,ball_offset_x;
	int i,j,flip;
	GF_BGL_BMPWIN* win;

	wk->hdl = ArchiveDataHandleOpen( ARC_FRONTIER_BG, HEAPID_CASTLE );
	Castle_BgInit( wk );								//BG初期化
	Castle_ObjInit( wk );								//OBJ初期化

	//メッセージデータマネージャー作成
	wk->msgman = MSGMAN_Create( MSGMAN_TYPE_DIRECT, ARC_MSG, 
								NARC_msg_castle_trainer_dat, HEAPID_CASTLE );

	wk->wordset = WORDSET_Create( HEAPID_CASTLE );
	wk->msg_buf = STRBUF_Create( CASTLE_MSG_BUF_SIZE, HEAPID_CASTLE );
	wk->tmp_buf = STRBUF_Create( CASTLE_MSG_BUF_SIZE, HEAPID_CASTLE );

	//文字列バッファ作成
	for( i=0; i < CASTLE_MENU_BUF_MAX ;i++ ){
		wk->menu_buf[i] = STRBUF_Create( CASTLE_MENU_BUF_SIZE, HEAPID_CASTLE );
	}

	//フォントパレット
	SystemFontPaletteLoad( PALTYPE_MAIN_BG, BC_FONT_PAL * 32, HEAPID_CASTLE );
	TalkFontPaletteLoad( PALTYPE_MAIN_BG, BC_MSGFONT_PAL * 32, HEAPID_CASTLE );

	//8x8フォント作成(使用bmpwin[ENEMY_BMPWIN_TYPE]のパレット(BC_FONT_PAL)を使用
	//wk->num_font = NUMFONT_Create( 15, 14, FBMP_COL_NULL, HEAPID_CASTLE );
	wk->num_font = NUMFONT_Create( FBMP_COL_BLACK, FBMP_COL_BLK_SDW, FBMP_COL_NULL, HEAPID_CASTLE );

	//ビットマップ追加
	CastleAddBmpWin( wk->bgl, wk->bmpwin, CASTLE_BMP_ENEMY );

	//通信しているかで表示のオフセットが変わる
	Castle_GetOffset( wk, &offset_x, &offset_y, &pair_offset_x, &pair_offset_y );

	////////////////////////////////////////////////////////////////////////////////////////
	if( Castle_CommCheck(wk->type) == FALSE ){
		icon_offset_x = ICON_START_X;
		ball_offset_x = BALL_START_X;
	}else{
		icon_offset_x = ICON_MULTI_START_X;
		ball_offset_x = BALL_MULTI_START_X;
	}

	//ボールOBJ
	e_max = Castle_GetEnemyPokeNum( wk->type, CASTLE_FLAG_TOTAL );
	for( i=0; i < e_max ;i++ ){
		wk->p_ball[i] = CastleObj_Create(	&wk->castle_clact, 
											ID_CHAR_CSR, ID_PLTT_CSR,
											ID_CELL_CSR, CASTLE_ANM_BALL_MOVE, 
											BALL_WIDTH_X*i+ball_offset_x, 
											BALL_START_Y, CASTLE_BG_PRI_LOW, NULL );

		//HP状態OBJ追加
		wk->p_hp[i] = CastleObj_Create(	&wk->castle_clact,
										ID_CHAR_CSR, ID_PLTT_CSR, 
										ID_CELL_CSR, CASTLE_ANM_HP_MIDORI,
										ICON_WIDTH_X*i+icon_offset_x, 
										ICON_START_Y+20, CASTLE_BG_PRI_BACK, NULL );

		//ポケモンアイコンOBJ追加
		wk->p_icon[i] = CastleObj_Create(	&wk->castle_clact, 
											ID_CHAR_ICON1+i, ID_PLTT_ICON,
											ID_CELL_ICON, POKEICON_ANM_HPMAX,
											ICON_WIDTH_X*i+icon_offset_x, 
											ICON_START_Y, CASTLE_BG_PRI_LOW, NULL );

		//ポケモンアイコンのパレット切り替え
		CastleObj_IconPalChg( wk->p_icon[i], PokeParty_GetMemberPointer(wk->p_party,i) );

		//手持ちが公開されていなかったら
		if( wk->p_temoti_flag[i] == 0 ){
			CastleObj_Vanish( wk->p_ball[i], CASTLE_VANISH_OFF );	//ボール表示
			CastleObj_Vanish( wk->p_icon[i], CASTLE_VANISH_ON );	//ポケアイコン非表示
			CastleObj_Vanish( wk->p_hp[i], CASTLE_VANISH_ON );		//HP状態アイコン非表示
		}else{
			CastleObj_Vanish( wk->p_ball[i], CASTLE_VANISH_ON );	//ボール非表示
			CastleObj_Vanish( wk->p_icon[i], CASTLE_VANISH_OFF );	//ポケアイコン表示
			CastleObj_Vanish( wk->p_hp[i], CASTLE_VANISH_OFF );		//HP状態アイコン表示
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////
	//ポケモン選択ウィンOBJ追加
	CastleEnemy_GetPokeSelXY( wk, &start_x, &start_y, 0 );
	wk->p_poke_sel = CastleObj_Create(	&wk->castle_clact, 
										ID_CHAR_CSR, ID_PLTT_CSR, 
										ID_CELL_CSR, CASTLE_ANM_SELECT_ZIBUN, 
										start_x, start_y, 
										CASTLE_BG_PRI_LOW, NULL );

	wk->p_pair_poke_sel = CastleObj_Create(	&wk->castle_clact, 
											ID_CHAR_CSR, ID_PLTT_CSR, 
											ID_CELL_CSR, CASTLE_ANM_SELECT_AITE, 
											start_x, start_y, 
											CASTLE_BG_PRI_LOW, NULL );

	if( Castle_CommCheck(wk->type) == FALSE ){
		CastleObj_Vanish( wk->p_pair_poke_sel, CASTLE_VANISH_ON );			//非表示
	}

	////////////////////////////////////////////////////////////////////////////////////////
	//旗OBJ追加
	for( i=0; i < CASTLE_COMM_POKE_TOTAL_NUM ;i++ ){
		for( j=0; j < 2 ;j++ ){
			Castle_GetHataXY( wk, &start_x, &start_y );

			wk->p_hata[i][j] = CastleObj_Create(&wk->castle_clact, 
										ID_CHAR_CSR, ID_PLTT_CSR, 
										ID_CELL_CSR, CASTLE_ANM_HATA, 
										start_x + (CASTLE_HATA_WIDTH_X * i), 
										start_y + (CASTLE_HATA_WIDTH_Y * j), 
										CASTLE_BG_PRI_LOW, NULL );

			CastleObj_Vanish( wk->p_hata[i][j], CASTLE_VANISH_ON );			//非表示
		}
	}

	//つよさ、わざが公開されていたら旗を表示する
	CastleRank_SeqSubHataVanish( wk );

	////////////////////////////////////////////////////////////////////////////////////////
	//ランクアップOBJ追加
	wk->p_eff_rankup = CastleObj_Create(&wk->castle_clact, 
										ID_CHAR_CSR, ID_PLTT_CSR, 
										ID_CELL_CSR, CASTLE_ANM_RANK_UP, 
										20, 20,			//値適当
										CASTLE_BG_PRI_HIGH, NULL );
	CastleObj_Vanish( wk->p_eff_rankup, CASTLE_VANISH_ON );					//非表示

	////////////////////////////////////////////////////////////////////////////////////////
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
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_BgInit( CASTLE_ENEMY_WORK* wk )
{
	SetVramBank();
	SetBgHeader( wk->bgl );

	//パレットフェードシステムワーク作成
	wk->pfd = PaletteFadeInit( HEAPID_CASTLE );

	//リクエストデータをmallocしてセット
	PaletteFadeWorkAllocSet( wk->pfd, FADE_MAIN_OBJ, FADE_PAL_ALL_SIZE, HEAPID_CASTLE );
	PaletteFadeWorkAllocSet( wk->pfd, FADE_MAIN_BG, FADE_PAL_ALL_SIZE, HEAPID_CASTLE );

	//上画面背景
	Castle_SetMainBgGraphic( wk, BC_FRAME_BG );
	Castle_SetMainBgPalette();
	Castle_SetStatusBgGraphic( wk, BC_FRAME_SLIDE );
	//Castle_SetWazaBgGraphic( wk, BC_FRAME_SLIDE );
	Castle_SetStatusBgPalette();
	//GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );		//BG非表示
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );		//BG非表示

	//下画面背景、パレットセット
	Castle_SetSubBgGraphic( wk, BC_FRAME_SUB );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	OBJ関連初期化
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_ObjInit( CASTLE_ENEMY_WORK* wk )
{
	//CastleClact_InitCellActor( &wk->castle_clact );
	CastleClact_InitCellActor(	&wk->castle_clact, 
								//PokeParty_GetMemberPointer(wk->p_party,0) );
								//wk->p_party, wk->type );
								wk->p_party, Castle_CommCheck(wk->type) );
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
static void Castle_BgExit( GF_BGL_INI * ini )
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
	GF_BGL_BGControlExit( ini, BC_FRAME_BG );			//3
	GF_BGL_BGControlExit( ini, BC_FRAME_SLIDE );
	GF_BGL_BGControlExit( ini, BC_FRAME_TYPE );
	GF_BGL_BGControlExit( ini, BC_FRAME_WIN );
	GF_BGL_BGControlExit( ini, BC_FRAME_SUB );

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
	CASTLE_ENEMY_WORK* wk = work;

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
			2, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, BC_FRAME_WIN, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ClearCharSet( BC_FRAME_WIN, 32, 0, HEAPID_CASTLE );
		GF_BGL_ScrClear( ini, BC_FRAME_WIN );
	}

	{	//BG(フォント)(タイプ表示)
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			//GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x0c000, GX_BG_EXTPLTT_01,
			GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x0c000, GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, BC_FRAME_TYPE, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ClearCharSet( BC_FRAME_TYPE, 32, 0, HEAPID_CASTLE );
		GF_BGL_ScrClear( ini, BC_FRAME_TYPE );
	}

	{	//上画面(ステータス)
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			//GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x0c000, GX_BG_EXTPLTT_01,
			//GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
			GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x14000, GX_BG_EXTPLTT_01,
			1, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, BC_FRAME_SLIDE, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ClearCharSet( BC_FRAME_SLIDE, 32, 0, HEAPID_CASTLE );
		GF_BGL_ScrClear( ini, BC_FRAME_SLIDE );
	}

	{	//上画面(SINGLE,DOUBLE)
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x08000, GX_BG_EXTPLTT_01,
			3, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, BC_FRAME_BG, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ClearCharSet( BC_FRAME_BG, 32, 0, HEAPID_CASTLE );
		GF_BGL_ScrClear( ini, BC_FRAME_BG );
	}

	//--------------------------------------------------------------------------------
	{	//下画面(ボール)
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x3000, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, BC_FRAME_SUB, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear( ini, BC_FRAME_SUB );
	}

	G2_SetBG0Priority( 0 );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
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
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 * @param	frm		フレームナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_SetMainBgGraphic( CASTLE_ENEMY_WORK * wk, u32 frm  )
{
	ArcUtil_HDL_BgCharSet(	wk->hdl, BC_TAISEN_NCGR_BIN, 
							wk->bgl, frm, 0, 0, 1, HEAPID_CASTLE );

	if( Castle_CommCheck(wk->type) == FALSE ){
		ArcUtil_HDL_ScrnSet(wk->hdl, BC_TAISEN01_NSCR_BIN, 
							wk->bgl, frm, 0, 0, 1, HEAPID_CASTLE );
	}else{
		ArcUtil_HDL_ScrnSet(wk->hdl, BC_TAISEN02_NSCR_BIN, 
							wk->bgl, frm, 0, 0, 1, HEAPID_CASTLE );
	}
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
static void Castle_SetMainBgPalette( void )
{
	void *buf;
	NNSG2dPaletteData *dat;

	buf = ArcUtil_PalDataGet( ARC_FRONTIER_BG, BC_TAISEN_NCLR, &dat, HEAPID_CASTLE );

	DC_FlushRange( dat->pRawData, (sizeof(u16)*16*4) );
	GX_LoadBGPltt( dat->pRawData, 0, (sizeof(u16)*16*4) );		//メイン

	sys_FreeMemoryEz(buf);
	return;
}

//--------------------------------------------------------------
/**
 * @brief	グラフィックデータセット：上画面ステータス
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 * @param	frm		フレームナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_SetStatusBgGraphic( CASTLE_ENEMY_WORK * wk, u32 frm  )
{
	ArcUtil_HDL_BgCharSet(	wk->hdl, BC_TAISEN_NCGR_BIN, 
							wk->bgl, frm, 0, 0, 1, HEAPID_CASTLE );

	ArcUtil_HDL_ScrnSet(wk->hdl, BC_TAISEN_STATUS_NSCR_BIN, 
						wk->bgl, frm, 0, 0, 1, HEAPID_CASTLE );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	上画面ステータスパレット設定
 *
 * @param	none
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_SetStatusBgPalette( void )
{
	void *buf;
	NNSG2dPaletteData *dat;

	buf = ArcUtil_PalDataGet( ARC_FRONTIER_BG, BC_TAISEN_NCLR, &dat, HEAPID_CASTLE );

	DC_FlushRange( dat->pRawData, (sizeof(u16)*16*4) );
	GX_LoadBGPltt( dat->pRawData, 0, (sizeof(u16)*16*4) );		//メイン

	sys_FreeMemoryEz(buf);
	return;
}

//--------------------------------------------------------------
/**
 * @brief	グラフィックデータセット：上画面わざ
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 * @param	frm		フレームナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_SetWazaBgGraphic( CASTLE_ENEMY_WORK * wk, u32 frm  )
{
	ArcUtil_HDL_BgCharSet(	wk->hdl, BC_TAISEN_NCGR_BIN, 
							wk->bgl, frm, 0, 0, 1, HEAPID_CASTLE );

	ArcUtil_HDL_ScrnSet(wk->hdl, BC_TAISEN_WAZA_NSCR_BIN, 
						wk->bgl, frm, 0, 0, 1, HEAPID_CASTLE );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	グラフィックデータセット：下画面背景、パレットセット
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 * @param	frm		フレームナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_SetSubBgGraphic( CASTLE_ENEMY_WORK * wk, u32 frm  )
{
	ArcUtil_HDL_BgCharSet(	wk->hdl, BATT_FRONTIER_NCGR_BIN, wk->bgl, frm,
							0, 0, TRUE, HEAPID_CASTLE );

	ArcUtil_HDL_ScrnSet(wk->hdl, BATT_FRONTIER_NSCR_BIN, wk->bgl, frm,
						0, 0, TRUE, HEAPID_CASTLE );

	ArcUtil_HDL_PalSet(	wk->hdl, BATT_FRONTIER_NCLR, PALTYPE_SUB_BG,
						0, 0x20, HEAPID_CASTLE );
	return;
}


//==============================================================================================
//
//	メッセージ関連
//
//==============================================================================================

extern void ov107_2247650(void);

static u8 CastleWriteMsg( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font )
{
    return CastleWriteMsg_Full_ov107_2247680( wk, win, msg_id, x, y, wait, f_col, s_col, b_col, font, 0 );
}

//--------------------------------------------------------------
/**
 * @brief	メッセージ表示
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
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
static u8 CastleWriteMsg_Full_ov107_2247680( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font, u32 a10_mode )
{
	u8 msg_index;

	GF_BGL_BmpWinDataFill( win, b_col );			//塗りつぶし
	MSGMAN_GetString( wk->msgman, msg_id, wk->tmp_buf );

	//登録された単語を使って文字列展開する
	WORDSET_ExpandStr( wk->wordset, wk->msg_buf, wk->tmp_buf );

    switch(a10_mode){
    case 1:
        x -= (FontProc_GetPrintStrWidth(FONT_SYSTEM, wk->msg_buf, 0) + 1) / 2;
        break;
    case 2:
        x -= FontProc_GetPrintStrWidth(FONT_SYSTEM, wk->msg_buf, 0);
        break;
    }

	msg_index = GF_STR_PrintColor( win, font, wk->msg_buf, x, y, wait, 
								GF_PRINTCOLOR_MAKE(f_col,s_col,b_col), NULL );

	GF_BGL_BmpWinOnVReq( win );
	return msg_index;
}

static u8 CastleWriteMsgSimple( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font )
{
    return CastleWriteMsgSimple_Full_ov107_2247744( wk, win, msg_id, x, y, wait, f_col, s_col, b_col, font, 0 );
}

//--------------------------------------------------------------
/**
 * @brief	メッセージ表示(塗りつぶしなし)
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
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
static u8 CastleWriteMsgSimple_Full_ov107_2247744( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font, u32 a10_mode )
{
	u8 msg_index;

	MSGMAN_GetString( wk->msgman, msg_id, wk->tmp_buf );

	//登録された単語を使って文字列展開する
	WORDSET_ExpandStr( wk->wordset, wk->msg_buf, wk->tmp_buf );

    switch(a10_mode){
    case 1:
        x -= (FontProc_GetPrintStrWidth(FONT_SYSTEM, wk->msg_buf, 0) + 1) / 2;
        break;
    case 2:
        x -= FontProc_GetPrintStrWidth(FONT_SYSTEM, wk->msg_buf, 0);
        break;
    }

	msg_index = GF_STR_PrintColor( win, font, wk->msg_buf, x, y, wait, 
								GF_PRINTCOLOR_MAKE(f_col,s_col,b_col), NULL );

	GF_BGL_BmpWinOnVReq( win );
	return msg_index;
}

//--------------------------------------------------------------
/**
 * @brief	簡単メッセージ表示
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 * @param	msg_id	メッセージID
 *
 * @return	"msg_index"
 */
//--------------------------------------------------------------
static u8 Castle_EasyMsg( CASTLE_ENEMY_WORK* wk, int msg_id, u8 font )
{
	u8 msg_index;

	msg_index = CastleWriteMsg(wk, &wk->bmpwin[ENEMY_BMPWIN_TALK], msg_id, 
								1, 1, MSG_NO_PUT, 
								FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_WHITE, font );

	GF_BGL_BmpWinOnVReq( &wk->bmpwin[ENEMY_BMPWIN_TALK] );
	return msg_index;
}

//--------------------------------------------------------------
/**
 * @brief	ステータスメッセージ表示
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
enum{
	//もちもの
	STATUS_MOTIMONO_X =		(1*8),
	STATUS_MOTIMONO_Y =		(1*8),
	STATUS_MOTIMONO_NUM_X =	(9*8),

	//せいかく　○
	STATUS_SEIKAKU_X =		(1*8),
	STATUS_SEIKAKU_Y =		(3*8),
	STATUS_SEIKAKU_NUM_X =	(9*8),

	//とくせい
	STATUS_TOKUSEI_X =		(1*8),
	STATUS_TOKUSEI_Y =		(5*8),
	STATUS_TOKUSEI_NUM_X =	(9*8),

	//こうげき
	STATUS_KOUGEKI_X =		(1*8),
	STATUS_KOUGEKI_Y =		(7*8),
	STATUS_KOUGEKI_NUM_X =	(8*8),

	//ぼうぎょ
	STATUS_BOUGYO_X =		(12*8),
	STATUS_BOUGYO_Y =		(STATUS_KOUGEKI_Y),
	STATUS_BOUGYO_NUM_X =	(19*8),

	//とくこう
	STATUS_TOKUKOU_X =		(1*8),
	STATUS_TOKUKOU_Y =		(9*8),
	STATUS_TOKUKOU_NUM_X =	(STATUS_KOUGEKI_NUM_X),

	//とくぼう
	STATUS_TOKUBOU_X =		(12*8),
	STATUS_TOKUBOU_Y =		(STATUS_TOKUKOU_Y),
	STATUS_TOKUBOU_NUM_X =	(STATUS_BOUGYO_NUM_X),

	//すばやさ
	STATUS_SUBAYASA_X =		(1*8),
	STATUS_SUBAYASA_Y =		(11*8),
	STATUS_SUBAYASA_NUM_X =	(STATUS_KOUGEKI_NUM_X),
};

#ifdef NONEQUIVALENT
static void Castle_StatusMsgWrite( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win, POKEMON_PARAM* poke )
{
	GF_BGL_BmpWinDataFill( win, FBMP_COL_NULL );	//塗りつぶし

	//「もちもの」
	WORDSET_RegisterItemName( wk->wordset, 0, PokeParaGet(poke,ID_PARA_item,NULL) );
	StMsgWriteSub( wk, win, msg_castle_trainer_status_01, STATUS_MOTIMONO_X, STATUS_MOTIMONO_Y );
	StMsgWriteSub( wk, win,msg_castle_trainer_status_01_01,STATUS_MOTIMONO_NUM_X,STATUS_MOTIMONO_Y);

	//「せいかく」
	WORDSET_RegisterSeikaku( wk->wordset, 0, PokeSeikakuGet(poke) );
	StMsgWriteSub( wk, win, msg_castle_trainer_status_02, STATUS_SEIKAKU_X, STATUS_SEIKAKU_Y );
	StMsgWriteSub( wk, win, msg_castle_trainer_status_02_01, STATUS_SEIKAKU_NUM_X,STATUS_SEIKAKU_Y);

	//「とくせい」
	WORDSET_RegisterTokuseiName( wk->wordset, 0, PokeParaGet(poke,ID_PARA_speabino,NULL) );
	StMsgWriteSub( wk, win, msg_castle_trainer_status_03, STATUS_TOKUSEI_X, STATUS_TOKUSEI_Y );
	StMsgWriteSub( wk, win, msg_castle_trainer_status_03_01, STATUS_TOKUSEI_NUM_X,STATUS_TOKUSEI_Y);

	//「こうげき」
	//OS_Printf( "pow = %d\n", PokeParaGet(poke,ID_PARA_pow,NULL) );
	Castle_SetNumber(	wk, 0, PokeParaGet(poke,ID_PARA_pow,NULL), CASTLE_KETA_STATUS,
						NUMBER_DISPTYPE_SPACE );
	StMsgWriteSub( wk, win, msg_castle_trainer_status_04, STATUS_KOUGEKI_X, STATUS_KOUGEKI_Y );
	StMsgWriteSub( wk, win, msg_castle_trainer_status_04_01, STATUS_KOUGEKI_NUM_X,STATUS_KOUGEKI_Y);

	//「ぼうぎょ」
	Castle_SetNumber(	wk, 0, PokeParaGet(poke,ID_PARA_def,NULL), CASTLE_KETA_STATUS,
						NUMBER_DISPTYPE_SPACE );
	StMsgWriteSub( wk, win, msg_castle_trainer_status_05, STATUS_BOUGYO_X, STATUS_BOUGYO_Y );
	StMsgWriteSub( wk, win, msg_castle_trainer_status_05_01, STATUS_BOUGYO_NUM_X, STATUS_BOUGYO_Y );

	//「とくこう」
	Castle_SetNumber(	wk, 0, PokeParaGet(poke,ID_PARA_spepow,NULL), CASTLE_KETA_STATUS,
						NUMBER_DISPTYPE_SPACE );
	StMsgWriteSub( wk, win, msg_castle_trainer_status_06, STATUS_TOKUKOU_X, STATUS_TOKUKOU_Y );
	StMsgWriteSub( wk, win, msg_castle_trainer_status_06_01, STATUS_TOKUKOU_NUM_X,STATUS_TOKUKOU_Y);

	//「とくぼう」
	Castle_SetNumber(	wk, 0, PokeParaGet(poke,ID_PARA_spedef,NULL), CASTLE_KETA_STATUS,
						NUMBER_DISPTYPE_SPACE );
	StMsgWriteSub( wk, win, msg_castle_trainer_status_07, STATUS_TOKUBOU_X, STATUS_TOKUBOU_Y );
	StMsgWriteSub( wk, win, msg_castle_trainer_status_07_01, STATUS_TOKUBOU_NUM_X,STATUS_TOKUBOU_Y);

	//「すばやさ」
	Castle_SetNumber(	wk, 0, PokeParaGet(poke,ID_PARA_agi,NULL), CASTLE_KETA_STATUS,
						NUMBER_DISPTYPE_SPACE );
	StMsgWriteSub( wk, win, msg_castle_trainer_status_08, STATUS_SUBAYASA_X, STATUS_SUBAYASA_Y );
	StMsgWriteSub( wk, win,msg_castle_trainer_status_08_01,STATUS_SUBAYASA_NUM_X,STATUS_SUBAYASA_Y);

	GF_BGL_BmpWinOnVReq( win );
	return;
}
#else
asm static void Castle_StatusMsgWrite( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win, POKEMON_PARAM* poke )
{
	push {r4, r5, r6, lr}
	sub sp, #8
	add r4, r1, #0
	add r5, r0, #0
	add r6, r2, #0
	add r0, r4, #0
	mov r1, #0
	bl GF_BGL_BmpWinDataFill
	add r0, r6, #0
	mov r1, #6
	mov r2, #0
	bl PokeParaGet
	add r2, r0, #0
	ldr r0, [r5, #0x24]
	mov r1, #0
	bl WORDSET_RegisterItemName
	mov r3, #8
	str r3, [sp]
	mov r0, #0
	str r0, [sp, #4]
	add r0, r5, #0
	add r1, r4, #0
	mov r2, #0x30
	bl StMsgWriteSub
	mov r0, #8
	str r0, [sp]
	mov r0, #0
	str r0, [sp, #4]
	add r0, r5, #0
	add r1, r4, #0
	mov r2, #0x31
	mov r3, #0x48
	bl StMsgWriteSub
	add r0, r6, #0
	bl PokeSeikakuGet
	add r2, r0, #0
	ldr r0, [r5, #0x24]
	mov r1, #0
	bl WORDSET_RegisterSeikaku
	mov r0, #0x18
	str r0, [sp]
	mov r0, #0
	str r0, [sp, #4]
	add r0, r5, #0
	add r1, r4, #0
	mov r2, #0x32
	mov r3, #8
	bl StMsgWriteSub
	mov r0, #0x18
	str r0, [sp]
	mov r0, #0
	str r0, [sp, #4]
	add r0, r5, #0
	add r1, r4, #0
	mov r2, #0x33
	mov r3, #0x48
	bl StMsgWriteSub
	add r0, r6, #0
	mov r1, #0xa
	mov r2, #0
	bl PokeParaGet
	add r2, r0, #0
	ldr r0, [r5, #0x24]
	mov r1, #0
	bl WORDSET_RegisterTokuseiName
	mov r0, #0x28
	str r0, [sp]
	mov r0, #0
	str r0, [sp, #4]
	add r0, r5, #0
	add r1, r4, #0
	mov r2, #0x34
	mov r3, #8
	bl StMsgWriteSub
	mov r0, #0x28
	str r0, [sp]
	mov r0, #0
	str r0, [sp, #4]
	add r0, r5, #0
	add r1, r4, #0
	mov r2, #0x35
	mov r3, #0x48
	bl StMsgWriteSub
	add r0, r6, #0
	mov r1, #0xa5
	mov r2, #0
	bl PokeParaGet
	add r2, r0, #0
	mov r0, #1
	str r0, [sp]
	add r0, r5, #0
	mov r1, #0
	mov r3, #3
	bl Castle_SetNumber
	mov r0, #0x38
	str r0, [sp]
	mov r0, #0
	str r0, [sp, #4]
	add r0, r5, #0
	add r1, r4, #0
	mov r2, #0x36
	mov r3, #8
	bl StMsgWriteSub
	mov r0, #0x38
	str r0, [sp]
	mov r0, #2
	str r0, [sp, #4]
	add r0, r5, #0
	add r1, r4, #0
	mov r2, #0x37
	mov r3, #0x58
	bl StMsgWriteSub
	add r0, r6, #0
	mov r1, #0xa6
	mov r2, #0
	bl PokeParaGet
	add r2, r0, #0
	mov r0, #1
	str r0, [sp]
	add r0, r5, #0
	mov r1, #0
	mov r3, #3
	bl Castle_SetNumber
	mov r2, #0x38
	str r2, [sp]
	mov r0, #0
	str r0, [sp, #4]
	add r0, r5, #0
	add r1, r4, #0
	mov r3, #0x68
	bl StMsgWriteSub
	mov r0, #0x38
	str r0, [sp]
	mov r0, #2
	str r0, [sp, #4]
	add r0, r5, #0
	add r1, r4, #0
	mov r2, #0x39
	mov r3, #0xb8
	bl StMsgWriteSub
	add r0, r6, #0
	mov r1, #0xa8
	mov r2, #0
	bl PokeParaGet
	add r2, r0, #0
	mov r0, #1
	str r0, [sp]
	add r0, r5, #0
	mov r1, #0
	mov r3, #3
	bl Castle_SetNumber
	mov r0, #0x48
	str r0, [sp]
	mov r0, #0
	str r0, [sp, #4]
	add r0, r5, #0
	add r1, r4, #0
	mov r2, #0x3a
	mov r3, #8
	bl StMsgWriteSub
	mov r0, #0x48
	str r0, [sp]
	mov r0, #2
	str r0, [sp, #4]
	add r0, r5, #0
	add r1, r4, #0
	mov r2, #0x3b
	mov r3, #0x58
	bl StMsgWriteSub
	add r0, r6, #0
	mov r1, #0xa9
	mov r2, #0
	bl PokeParaGet
	add r2, r0, #0
	mov r0, #1
	str r0, [sp]
	add r0, r5, #0
	mov r1, #0
	mov r3, #3
	bl Castle_SetNumber
	mov r0, #0x48
	str r0, [sp]
	mov r0, #0
	str r0, [sp, #4]
	add r0, r5, #0
	add r1, r4, #0
	mov r2, #0x3c
	mov r3, #0x68
	bl StMsgWriteSub
	mov r0, #0x48
	str r0, [sp]
	mov r0, #2
	str r0, [sp, #4]
	add r0, r5, #0
	add r1, r4, #0
	mov r2, #0x3d
	mov r3, #0xb8
	bl StMsgWriteSub
	add r0, r6, #0
	mov r1, #0xa7
	mov r2, #0
	bl PokeParaGet
	add r2, r0, #0
	mov r0, #1
	str r0, [sp]
	add r0, r5, #0
	mov r1, #0
	mov r3, #3
	bl Castle_SetNumber
	mov r0, #0x58
	str r0, [sp]
	mov r0, #0
	str r0, [sp, #4]
	add r0, r5, #0
	add r1, r4, #0
	mov r2, #0x3e
	mov r3, #8
	bl StMsgWriteSub
	mov r3, #0x58
	str r3, [sp]
	mov r0, #2
	str r0, [sp, #4]
	add r0, r5, #0
	add r1, r4, #0
	mov r2, #0x3f
	bl StMsgWriteSub
	add r0, r4, #0
	bl GF_BGL_BmpWinOnVReq
	add sp, #8
	pop {r4, r5, r6, pc}
}
#endif

static void StMsgWriteSub( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win, int msg, u16 x, u16 y, u32 a5_mode )
{
	CastleWriteMsgSimple_Full_ov107_2247744(	wk, win, msg, x, y, MSG_NO_PUT,
							FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, BC_FONT, a5_mode );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	技メッセージ表示
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
enum{
	//技名
	STATUS_WAZA_X =			(4*8), // MatchComment: 3*8 -> 4*8
	STATUS_WAZA_Y =			(2*8-4), // MatchComment: 2*8 -> 2*8-4

	//技ポイント
	STATUS_POINT_X =		(12*8),
	STATUS_POINT_Y =		(0*8), // MatchComment: 2*8 -> 0*8
};

static void Castle_WazaMsgWrite( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win, POKEMON_PARAM* poke )
{
	GF_BGL_BmpWinDataFill( win, FBMP_COL_NULL );	//塗りつぶし

	//id+iは保証されるかわからないので、やらない。

	WazaMsgWriteSub(	wk, win, 0, msg_castle_trainer_waza_01_01, 
						msg_castle_trainer_waza_02, poke, ID_PARA_waza1, 
						ID_PARA_pp1, ID_PARA_pp_max1 );

	WazaMsgWriteSub(	wk, win, 1, msg_castle_trainer_waza_01_02, 
						msg_castle_trainer_waza_02, poke, ID_PARA_waza2, 
						ID_PARA_pp2, ID_PARA_pp_max2 );

	WazaMsgWriteSub(	wk, win, 2, msg_castle_trainer_waza_01_03, 
						msg_castle_trainer_waza_02, poke, ID_PARA_waza3, 
						ID_PARA_pp3, ID_PARA_pp_max3 );

	WazaMsgWriteSub(	wk, win, 3, msg_castle_trainer_waza_01_04, 
						msg_castle_trainer_waza_02, poke, ID_PARA_waza4, 
						ID_PARA_pp4, ID_PARA_pp_max4 );

	GF_BGL_BmpWinOnVReq( win );
	return;
}

#ifdef NONEQUIVALENT
static void WazaMsgWriteSub( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win, u8 no, u32 msg_id, u32 msg_id2, POKEMON_PARAM* poke, u32 id, u32 id2, u32 id3 )
{
	u16 x,y,point_x,point_y;

	x = STATUS_WAZA_X;
	y = STATUS_WAZA_Y + (no * 24);
	point_x = STATUS_POINT_X;
	point_y = STATUS_POINT_Y + (no * 24);

	//「技名」
	//WORDSET_RegisterWazaName( wk->wordset, 0, PokeParaGet(poke,id,NULL) );
	WORDSET_RegisterWazaName( wk->wordset, no, PokeParaGet(poke,id,NULL) );
	wk->msg_index = CastleWriteMsgSimple( wk, win,
									msg_id, x, y, MSG_NO_PUT, 
									FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, BC_FONT );

	//「技ポイント」
	//Castle_SetNumber( wk, 0, PokeParaGet(poke,id2,NULL), CASTLE_KETA_STATUS );
	//Castle_SetNumber( wk, 1, PokeParaGet(poke,id3,NULL), CASTLE_KETA_STATUS );
	Castle_SetNumber(	wk, 4, PokeParaGet(poke,id2,NULL), CASTLE_KETA_STATUS,
						NUMBER_DISPTYPE_SPACE );
	Castle_SetNumber(	wk, 5, PokeParaGet(poke,id3,NULL), CASTLE_KETA_STATUS,
						NUMBER_DISPTYPE_LEFT );
    // MatchComment: use CastleWriteMsgSimple_Full_ov107_2247744 instead of CastleWriteMsgSimple
    // has additional argument of 1
	wk->msg_index = CastleWriteMsgSimple_Full_ov107_2247744( wk, win,
									msg_id2, point_x, point_y, MSG_NO_PUT,
									FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, BC_FONT, 1 );
	return;
}
#else
asm static void WazaMsgWriteSub( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win, u8 no, u32 msg_id, u32 msg_id2, POKEMON_PARAM* poke, u32 id, u32 id2, u32 id3 )
{
	push {r4, r5, r6, r7, lr}
	sub sp, #0x1c
	add r4, r0, #0
	add r6, r1, #0
	add r5, r2, #0
	ldr r0, [sp, #0x34]
	ldr r1, [sp, #0x38]
	mov r2, #0
	add r7, r3, #0
	bl PokeParaGet
	add r2, r0, #0
	ldr r0, [r4, #0x24]
	add r1, r5, #0
	bl WORDSET_RegisterWazaName
	mov r0, #0x18
	mul r0, r5
	add r0, #0xc
	lsl r0, r0, #0x10
	lsr r5, r0, #0x10
	str r5, [sp]
	mov r0, #0xff
	str r0, [sp, #4]
	mov r0, #1
	str r0, [sp, #8]
	mov r0, #2
	str r0, [sp, #0xc]
	mov r0, #0
	str r0, [sp, #0x10]
	str r0, [sp, #0x14]
	add r0, r4, #0
	add r1, r6, #0
	add r2, r7, #0
	mov r3, #0x20
	bl CastleWriteMsgSimple
	strb r0, [r4, #0xa]
	ldr r0, [sp, #0x34]
	ldr r1, [sp, #0x3c]
	mov r2, #0
	bl PokeParaGet
	add r2, r0, #0
	mov r0, #0
	str r0, [sp]
	add r0, r4, #0
	mov r1, #4
	mov r3, #3
	bl Castle_SetNumber
	ldr r0, [sp, #0x34]
	ldr r1, [sp, #0x40]
	mov r2, #0
	bl PokeParaGet
	add r2, r0, #0
	mov r0, #0
	str r0, [sp]
	add r0, r4, #0
	mov r1, #5
	mov r3, #3
	bl Castle_SetNumber
	str r5, [sp]
	mov r0, #0xff
	str r0, [sp, #4]
	mov r1, #1
	str r1, [sp, #8]
	mov r0, #2
	str r0, [sp, #0xc]
	mov r0, #0
	str r0, [sp, #0x10]
	str r0, [sp, #0x14]
	str r1, [sp, #0x18]
	ldr r2, [sp, #0x30]
	add r0, r4, #0
	add r1, r6, #0
	mov r3, #0x91
	bl CastleWriteMsgSimple_Full_ov107_2247744
	strb r0, [r4, #0xa]
	add sp, #0x1c
	pop {r4, r5, r6, r7, pc}
}
#endif

//--------------------------------------------------------------------------------------------
/**
 * HPとMHPのスラッシュ表示
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void Castle_PokeHpMsgWrite( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win )
{
	u16 hp_offset_x;
	u8 e_max;
	int i;
	POKEMON_PARAM* poke;

	GF_BGL_BmpWinDataFill( win, FBMP_COL_NULL );	//塗りつぶし

	if( Castle_CommCheck(wk->type) == FALSE ){
		hp_offset_x = CASTLE_HP_START_X;
	}else{
		hp_offset_x = CASTLE_MULTI_HP_START_X;
	}

	e_max = Castle_GetEnemyPokeNum( wk->type, CASTLE_FLAG_TOTAL );

	for( i=0;i < e_max ;i++ ){

		//正体が公開されていなかったら何も表示しない
		if( wk->p_temoti_flag[i] == 0 ){
			GF_BGL_BmpWinFill(	win, FBMP_COL_NULL, 
								hp_offset_x + (CASTLE_HP_WIDTH_X * i), CASTLE_HP_START_Y,
								CASTLE_HP_WIDTH_X, 2*8 );

			continue;
		}

		poke =  PokeParty_GetMemberPointer( wk->p_party, i );

		//HPは右づめ
		NUMFONT_WriteNumber(wk->num_font, PokeParaGet(poke,ID_PARA_hp,NULL), CASTLE_KETA_HP, 
							NUMFONT_MODE_SPACE, win, 
							hp_offset_x + (CASTLE_HP_WIDTH_X * i), CASTLE_HP_START_Y );

		//HPとMHPのスラッシュ(記号描画)
		NUMFONT_WriteMark(	wk->num_font, NUMFONT_MARK_SLASH, win, 
							CASTLE_HPSLASH_START_X + hp_offset_x + (CASTLE_HP_WIDTH_X * i),
							CASTLE_HP_START_Y );

		//HPMAXは左づめ
		NUMFONT_WriteNumber(wk->num_font, PokeParaGet(poke,ID_PARA_hpmax,NULL), CASTLE_KETA_HP, 
							NUMFONT_MODE_LEFT, win, 
							CASTLE_HPMAX_START_X + hp_offset_x + (CASTLE_HP_WIDTH_X * i), 
							CASTLE_HP_START_Y );
	}

	GF_BGL_BmpWinOnVReq( win );
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * レベル表示
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void Castle_PokeLvMsgWrite( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win )
{
	u32 sex;
	u16 lv_offset_x,sex_offset_x;
	u8 e_max;
	int i;
	POKEMON_PARAM* poke;

	GF_BGL_BmpWinDataFill( win, FBMP_COL_NULL );	//塗りつぶし

	if( Castle_CommCheck(wk->type) == FALSE ){
		lv_offset_x = CASTLE_LV_START_X;
		sex_offset_x = CASTLE_SEX_START_X;
	}else{
		lv_offset_x = CASTLE_MULTI_LV_START_X;
		sex_offset_x = CASTLE_MULTI_SEX_START_X;
	}

	e_max = Castle_GetEnemyPokeNum( wk->type, CASTLE_FLAG_TOTAL );

	for( i=0;i < e_max ;i++ ){
		poke =  PokeParty_GetMemberPointer( wk->p_party, i );

		//LV(記号と数字をセットで描画)
		NUMFONT_WriteSet(	wk->num_font, NUMFONT_MARK_LV, PokeParaGet(poke,ID_PARA_level,NULL),
							CASTLE_KETA_LV, NUMFONT_MODE_LEFT, win,
							lv_offset_x + (CASTLE_LV_WIDTH_X * i), CASTLE_LV_START_Y );

		//性別表示
		sex = PokeParaGet( poke, ID_PARA_sex, NULL );
		PokeSexWrite(	wk, win, sex_offset_x + (CASTLE_SEX_WIDTH_X * i), CASTLE_SEX_START_Y, 
						BC_FONT, sex );

	}

	GF_BGL_BmpWinOnVReq( win );
	return;
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
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 * @param	win		GF_BGL_BMPWIN型のポインタ
 * @param	y_max	項目最大数
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void CastleInitMenu( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win, u8 y_max )
{
	int i;

	for( i=0; i < CASTLE_MENU_BUF_MAX ;i++ ){
		wk->Data[i].str	  = NULL;
		wk->Data[i].param = 0;
	}

	wk->MenuH.menu		= wk->Data;
	wk->MenuH.win		= win;
	wk->MenuH.font		= BC_FONT;
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
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 * @param	no		セットする場所(戻り値)
 * @param	param	戻り値
 * @param	msg_id	メッセージID
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void CastleSetMenuData( CASTLE_ENEMY_WORK* wk, u8 no, u8 param, int msg_id )
{
	int i;
	void* msg;

	SDK_ASSERTMSG( no < CASTLE_MENU_BUF_MAX, "メニュー項目数オーバー！" );

	MSGMAN_GetString( wk->msgman, msg_id, wk->menu_buf[no] );

	wk->Data[ no ].str = (const void *)wk->menu_buf[no];
	wk->Data[ no ].param = param;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	共通 メニューセット2「はい、いいえ」
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_SetMenu2( CASTLE_ENEMY_WORK* wk )
{
	CastleWriteMenuWin( wk->bgl, &wk->bmpwin[ENEMY_BMPWIN_YESNO] );
	CastleInitMenu( wk, &wk->bmpwin[ENEMY_BMPWIN_YESNO], 2 );
	CastleSetMenuData( wk, 0, 0, msg_castle_trainer_02_01 );				//はい
	CastleSetMenuData( wk, 1, 1, msg_castle_trainer_02_02 );				//いいえ
	wk->mw = BmpMenuAddEx( &wk->MenuH, 8, 0, 0, HEAPID_CASTLE, PAD_BUTTON_CANCEL );
	wk->menu_flag = 1;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	共通 メニューセット3「レベル＋５、レベルー５、やめる」
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_SetMenu3( CASTLE_ENEMY_WORK* wk )
{
	CastleWriteMenuWin( wk->bgl, &wk->bmpwin[ENEMY_BMPWIN_SEL] );
	CastleInitMenu( wk, &wk->bmpwin[ENEMY_BMPWIN_SEL], 3 );
	CastleSetMenuData( wk, 0, 0, msg_castle_trainer_05_01 );				//レベル＋５
	CastleSetMenuData( wk, 1, 1, msg_castle_trainer_05_02 );				//レベルー５
	CastleSetMenuData( wk, 2, 2, msg_castle_trainer_05_03 );				//やめる
	wk->mw = BmpMenuAddEx( &wk->MenuH, 8, 0, 0, HEAPID_CASTLE, PAD_BUTTON_CANCEL );
	wk->menu_flag = 1;
	return;
}


//==============================================================================================
//
//	「しょうたい、レベル、じょうほう、とじる」
//
//==============================================================================================
static const BMPLIST_HEADER CastleListH = {
	NULL,						//表示文字データポインタ

	NULL,						//カーソル移動ごとのコールバック関数
	NULL,						//一列表示ごとのコールバック関数

	NULL,						//GF_BGL_BMPWINのポインタ

	0,							//リスト項目数
	0,							//表示最大項目数

	0,							//ラベル表示Ｘ座標
	8,							//項目表示Ｘ座標
	0,							//カーソル表示Ｘ座標
	0,							//表示Ｙ座標
	FBMP_COL_BLACK,				//文字色
	FBMP_COL_NULL,				//背景色
	FBMP_COL_BLK_SDW,			//文字影色
	0,							//文字間隔Ｘ
	16,							//文字間隔Ｙ
	BMPLIST_NO_SKIP,			//ページスキップタイプ
	FONT_SYSTEM,				//文字指定(本来は u8 だけど、そんなに作らないと思うので)
	0,							//ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)
	NULL,
};

static const u32 basic_msg_tbl[][2] = {
	{ msg_castle_trainer_choice_01,	FC_PARAM_TEMOTI },	//しらべる
	{ msg_castle_trainer_choice_02,	FC_PARAM_LEVEL },	//レベル
	{ msg_castle_trainer_choice_03,	FC_PARAM_INFO },	//じょうほう
	{ msg_castle_trainer_choice_04,	BMPLIST_CANCEL },	//とじる
};
#define BASIC_MSG_TBL_MAX	( NELEMS(basic_msg_tbl) )

static const u32 basic_under_msg_tbl[] = {
	msg_castle_trainer_00_03,	//しらべる
	msg_castle_trainer_00_04,	//レベル
	msg_castle_trainer_00_05,	//じょうほう
	msg_castle_trainer_00_06,	//とじる
};

//--------------------------------------------------------------
/**
 * @brief	「しらべる、レベル、じょうほう、とじる」リスト作成
 *
 * @param	wk			CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_BasicListMake( CASTLE_ENEMY_WORK* wk )
{
	int i;
	BMPLIST_HEADER list_h;

	//メニューウィンドウを描画
	CastleWriteMenuWin( wk->bgl, &wk->bmpwin[ENEMY_BMPWIN_BASIC_LIST] );
	GF_BGL_BmpWinDataFill( &wk->bmpwin[ENEMY_BMPWIN_BASIC_LIST], FBMP_COL_WHITE );	//塗りつぶし

	wk->menulist = BMP_MENULIST_Create( BASIC_MSG_TBL_MAX, HEAPID_CASTLE );

	for( i=0; i < BASIC_MSG_TBL_MAX; i++ ){
		BMP_MENULIST_AddArchiveString(	wk->menulist, wk->msgman, 
										basic_msg_tbl[i][0], basic_msg_tbl[i][1] );
	}

	list_h			= CastleListH;
	list_h.list		= wk->menulist;
	list_h.win		= &wk->bmpwin[ENEMY_BMPWIN_BASIC_LIST];
	list_h.work		= wk;
	list_h.call_back= Castle_CsrMoveCallBack;				//カーソル移動ごとのコールバック関数
	list_h.icon		= NULL;									//一列表示ごとのコールバック関数
	list_h.count	= BASIC_MSG_TBL_MAX;					//リスト項目数
	list_h.b_col	= FBMP_COL_WHITE;						//背景色
	list_h.line		= 4;									//表示最大項目数
	wk->lw			= BmpListSet( &list_h, wk->basic_list_lp, wk->basic_list_cp, HEAPID_CASTLE );

	wk->list_flag	= 1;
	GF_BGL_BmpWinOnVReq( &wk->bmpwin[ENEMY_BMPWIN_BASIC_LIST] );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	カーソル移動ごとのコールバック
 *
 * @param	wk			CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_CsrMoveCallBack( BMPLIST_WORK* work, u32 param, u8 mode )
{
	u32 count,line;
	u16 item,list_bak,pos_bak;
	CASTLE_ENEMY_WORK* wk = (CASTLE_ENEMY_WORK*)BmpListParamGet( work, BMPLIST_ID_WORK );

	//初期化時は鳴らさない
	if( mode == 0 ){
		Snd_SePlay( SEQ_SE_DP_SELECT );
	}

	BmpListDirectPosGet( work, &pos_bak );				//カーソル座標取得

	CastleWriteMsg( wk, &wk->bmpwin[ENEMY_BMPWIN_TALKMENU2], 
					basic_under_msg_tbl[pos_bak], 1, 1, MSG_NO_PUT, 
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_WHITE, FONT_TALK );

	return;
}


//==============================================================================================
//
//	「つよさ、わざ、ランクアップ、やめる」リスト
//
//==============================================================================================
static const u32 basic2_msg_tbl[][2] = {
	{ msg_castle_trainer_09_01,	FC_PARAM_TUYOSA },		//つよさ
	{ msg_castle_trainer_09_02,	FC_PARAM_WAZA },		//わざ
	{ msg_castle_trainer_09_03,	FC_PARAM_INFO_RANKUP },	//ランクアップ
	{ msg_castle_trainer_09_04,	BMPLIST_CANCEL },		//やめる
};
#define BASIC2_MSG_TBL_MAX			( NELEMS(basic2_msg_tbl) )

static const u16 basic2_under_msg_tbl[][2] = {
	{ msg_castle_trainer_09_05, msg_castle_trainer_09_05 },		//つよさ
	{ msg_castle_trainer_09_06,	msg_castle_trainer_09_06 },		//わざ
	{ msg_castle_trainer_09_07, msg_castle_trainer_09_08 },		//ランクアップ(ここがかわる)
	{ msg_castle_trainer_09_09,	msg_castle_trainer_09_09 },		//やめる
};

//--------------------------------------------------------------
/**
 * @brief	「つよさ、わざ、ランクアップ、やめる」リスト作成
 *
 * @param	wk			CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_Basic2ListMake( CASTLE_ENEMY_WORK* wk )
{
	int i;
	BMPLIST_HEADER list_h;

	//メニューウィンドウを描画
	CastleWriteMenuWin( wk->bgl, &wk->bmpwin[ENEMY_BMPWIN_BASIC2_LIST] );
	GF_BGL_BmpWinDataFill( &wk->bmpwin[ENEMY_BMPWIN_BASIC2_LIST], FBMP_COL_WHITE );	//塗りつぶし

	wk->menulist = BMP_MENULIST_Create( BASIC2_MSG_TBL_MAX, HEAPID_CASTLE );

	for( i=0; i < BASIC2_MSG_TBL_MAX; i++ ){
		BMP_MENULIST_AddArchiveString(	wk->menulist, wk->msgman, 
										basic2_msg_tbl[i][0], basic2_msg_tbl[i][1] );
	}

	list_h			= CastleListH;
	list_h.list		= wk->menulist;
	list_h.win		= &wk->bmpwin[ENEMY_BMPWIN_BASIC2_LIST];
	list_h.work		= wk;
	list_h.call_back= Castle_Basic2CsrMoveCallBack;			//カーソル移動ごとのコールバック関数
	list_h.icon		= Castle_Basic2LineWriteCallBack;		//一列表示ごとのコールバック関数
	list_h.count	= BASIC2_MSG_TBL_MAX;					//リスト項目数
	list_h.b_col	= FBMP_COL_WHITE;						//背景色
	list_h.line		= 4;									//表示最大項目数
	wk->lw			= BmpListSet( &list_h, 0, 0, HEAPID_CASTLE );

	wk->list_flag	= 1;
	GF_BGL_BmpWinOnVReq( &wk->bmpwin[ENEMY_BMPWIN_BASIC2_LIST] );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	カーソル移動ごとのコールバック
 *
 * @param	wk			CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_Basic2CsrMoveCallBack( BMPLIST_WORK* work, u32 param, u8 mode )
{
	u8 now_rank,no;
	u16 pos_bak;
	CASTLE_ENEMY_WORK* wk = (CASTLE_ENEMY_WORK*)BmpListParamGet( work, BMPLIST_ID_WORK );

	//初期化時は鳴らさない
	if( mode == 0 ){
		Snd_SePlay( SEQ_SE_DP_SELECT );
	}

	BmpListDirectPosGet( work, &pos_bak );				//カーソル座標取得

	//ランクによってメッセージが変わる
	now_rank = Castle_GetRank( wk->sv, wk->type, CASTLE_RANK_TYPE_INFO );
	if( now_rank == 1 ){
		no= 0;
	}else{
		no = 1;
	}

	//下のウィンドウに表示するメッセージが変わる
	CastleWriteMsg( wk, &wk->bmpwin[ENEMY_BMPWIN_TALK], 
					basic2_under_msg_tbl[pos_bak][no], 1, 1, MSG_NO_PUT, 
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_WHITE, FONT_TALK );

	return;
}

//--------------------------------------------------------------
/**
 * @brief	一列表示ごとのコールバック
 *
 * @param	wk			CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_Basic2LineWriteCallBack( BMPLIST_WORK* work, u32 param, u8 y )
{
	u8 now_rank,f_col;
	CASTLE_ENEMY_WORK* wk = (CASTLE_ENEMY_WORK*)BmpListParamGet( work, BMPLIST_ID_WORK );

	now_rank = Castle_GetRank( wk->sv, wk->type, CASTLE_RANK_TYPE_INFO );

	f_col = FBMP_COL_BLACK;

	switch( param ){
	case FC_PARAM_WAZA:
		//ランクを見て表示色を変える
		if( now_rank == 1 ){
			f_col = FBMP_COL_BLK_SDW;
		}else{
			f_col = FBMP_COL_BLACK;
		}
		break;

	case FC_PARAM_INFO_RANKUP:
		//ランクを見て表示色を変える
		if( now_rank == (CASTLE_RANK_MAX-1) ){
			f_col = FBMP_COL_BLK_SDW;
		}else{
			f_col = FBMP_COL_BLACK;
		}
		break;

	//case FC_PARAM_TUYOSA:
	default:
		f_col = FBMP_COL_BLACK;
		break;
	};

	BmpListColorControl( work, f_col, FBMP_COL_WHITE, FBMP_COL_BLK_SDW );
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
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 * @param	bufID	バッファID
 * @param	number	セットする数値
 *
 * @retval	none
 *
 * 桁を固定にしている
 */
//--------------------------------------------------------------
static void Castle_SetNumber( CASTLE_ENEMY_WORK* wk, u32 bufID, s32 number, u32 keta, NUMBER_DISPTYPE disp )
{
	WORDSET_RegisterNumber( wk->wordset, bufID, number, keta, disp, NUMBER_CODETYPE_DEFAULT );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	ポケモン名をセット
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 * @param	bufID	バッファID
 * @param	ppp		POKEMON_PASO_PARAM
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void Castle_SetPokeName( CASTLE_ENEMY_WORK* wk, u32 bufID, POKEMON_PASO_PARAM* ppp )
{
	WORDSET_RegisterPokeMonsName( wk->wordset, bufID, ppp );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	プレイヤー名をセット
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 * @param	bufID	バッファID
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void Castle_SetPlayerName( CASTLE_ENEMY_WORK* wk, u32 bufID )
{
	WORDSET_RegisterPlayerName( wk->wordset, bufID, SaveData_GetMyStatus(wk->sv) );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	プレイヤー名を表示
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
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
static void PlayerNameWrite( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 font )
{
	u32 col;
	const MYSTATUS* my;
	STRBUF* player_buf;								//プレイヤー名バッファポインタ

	my = SaveData_GetMyStatus( wk->sv );
	player_buf = STRBUF_Create( PLAYER_NAME_BUF_SIZE, HEAPID_CASTLE );
	
	//GF_BGL_BmpWinDataFill( win, FBMP_COL_NULL );			//塗りつぶし

	STRBUF_SetStringCode( player_buf, MyStatus_GetMyName(my) );

	if( MyStatus_GetMySex(my) == PM_MALE ){
		col = COL_BLUE;
	}else{
		col = COL_RED;
	}
	//OS_Printf( "col = %d\n", col );

	GF_STR_PrintColor( win, font, player_buf, x, y, MSG_NO_PUT, col, NULL );

	STRBUF_Delete( player_buf );					//メッセージバッファ開放
	return;
}

//--------------------------------------------------------------
/**
 * @brief	パートナー名を表示
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
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
static void PairNameWrite( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 font )
{
	MYSTATUS* my;
	u32 col;

	//パートナーのMyStatus取得
	my = CommInfoGetMyStatus( (CommGetCurrentID() ^ 1) );

	if( MyStatus_GetMySex(my) == PM_MALE ){
		col = COL_BLUE;
	}else{
		col = COL_RED;
	}

	WORDSET_RegisterPlayerName( wk->wordset, 0, my );

	CastleWriteMsgSimple(	wk, win, msg_castle_trainer_name_02, 
							x, y, MSG_NO_PUT, 
							GF_PRINTCOLOR_GET_LETTER(col),
							GF_PRINTCOLOR_GET_SHADOW(col),
							GF_PRINTCOLOR_GET_GROUND(col),
							//FBMP_COL_NULL,
							BC_FONT );
								
	//GF_BGL_BmpWinOnVReq( win );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	ポケモン名を表示
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
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
static void PokeNameWrite( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 f_col, u8 s_col, u8 b_col, u8 font )
{
	u8 x_pos;
	u32 sex,msg_id,col;
	STRBUF* buf;
	POKEMON_PARAM* poke;
	STRCODE sel_poke_buf[POKE_NAME_BUF_SIZE];					//ポケモン名バッファポインタ

	poke =  PokeParty_GetMemberPointer( wk->p_party, 0 );
	PokeParaGet( poke, ID_PARA_default_name, sel_poke_buf );	//ポケモンのデフォルト名を取得

	GF_BGL_BmpWinDataFill( win, b_col );						//塗りつぶし

	buf = STRBUF_Create( POKE_NAME_BUF_SIZE, HEAPID_CASTLE );	//STRBUF生成
	STRBUF_SetStringCode( buf, sel_poke_buf );					//STRCODE→STRBUF

	GF_STR_PrintColor( win, font, buf, x, y, MSG_NO_PUT, 
								GF_PRINTCOLOR_MAKE(f_col,s_col,b_col), NULL );
	GF_BGL_BmpWinOnVReq( win );

	STRBUF_Delete( buf );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	ポケモン名を表示(性別指定)
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
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
static void PokeNameWriteEx( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 f_col, u8 s_col, u8 b_col, u8 font, u16 monsno, u8 sex )
{
	u8 x_pos;
	u32 msg_id,col;
	MSGDATA_MANAGER* man;
	STRBUF* buf;
	POKEMON_PARAM* poke;
	STRCODE sel_poke_buf[POKE_NAME_BUF_SIZE];					//ポケモン名バッファポインタ

	GF_BGL_BmpWinDataFill( win, b_col );						//塗りつぶし

	//ポケモン名を取得
	man = MSGMAN_Create( MSGMAN_TYPE_DIRECT, ARC_MSG, NARC_msg_monsname_dat, HEAPID_CASTLE );
	buf = MSGMAN_AllocString( man, monsno );
	MSGMAN_Delete( man );

	GF_STR_PrintColor( win, font, buf, x, y, MSG_NO_PUT, 
								GF_PRINTCOLOR_MAKE(f_col,s_col,b_col), NULL );

/*
	//右端に性別コードを表示
	x_pos = GF_BGL_BmpWinGet_SizeX(win) - 1;

	msg_id = (sex == PARA_MALE) ? msg_castle_male : msg_castle_female;
	col = (sex == PARA_MALE) ? COL_BLUE : COL_RED;

	STRBUF_Clear( buf );
	MSGMAN_GetString( wk->msgman, msg_id, buf );
	GF_STR_PrintColor( win, font, buf, x_pos*8, y, MSG_NO_PUT, col, NULL );
*/

	STRBUF_Delete( buf );

	GF_BGL_BmpWinOnVReq( win );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	ポケモンの性別を表示
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 * @param	win		ビットマップウィンドウ
 * @param	x		X座標(ドット単位)
 * @param	y		Y座標(ドット単位)
 * @param	f_col	文字色ナンバー(塗りつぶしカラーコード)
 * @param	s_col	影色ナンバー
 * @param	b_col	背景色ナンバー
 * @param	font	フォント種類
 * @param	sex		性別
 *
 * @return	none
 */
//--------------------------------------------------------------
static void PokeSexWrite( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 font, u8 sex )
{
	u32 msg_id,col;
	u8 f_col,s_col,b_col;

	if( sex == PARA_MALE ){
		msg_id = msg_castle_trainer_status_10_01;
		f_col = FBMP_COL_BLUE;
		s_col = FBMP_COL_BLU_SDW;
		b_col = FBMP_COL_NULL;
	}else if( sex == PARA_FEMALE ){
		msg_id = msg_castle_trainer_status_10_02;
		f_col = FBMP_COL_RED;
		s_col = FBMP_COL_RED_SDW;
		b_col = FBMP_COL_NULL;
	}else{
		return;		//何もしない
	}

	CastleWriteMsgSimple(	wk, win, msg_id, 
							x, y, MSG_NO_PUT, 
							f_col, s_col, b_col, font );

	//GF_BGL_BmpWinOnVReq( win );
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * 初期状態を表示
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
#ifdef NONEQUIVALENT
static void CastleEnemy_Default_Write( CASTLE_ENEMY_WORK* wk )
{
	//「もどる」
	wk->msg_index = CastleWriteMsg( wk, &wk->bmpwin[ENEMY_BMPWIN_MODORU], 
									msg_castle_trainer_00_02, 1, 1+4 , MSG_NO_PUT, 
									FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM );

	//下にメニューウィンドウで説明を表示
	//CastleWriteMenuWin( wk->bgl, &wk->bmpwin[ENEMY_BMPWIN_TALKMENU] );
	CastleTalkWinPut( &wk->bmpwin[ENEMY_BMPWIN_TALKMENU], CONFIG_GetWindowType(wk->config));

	wk->msg_index = CastleWriteMsg( wk, &wk->bmpwin[ENEMY_BMPWIN_TALKMENU], 
									msg_castle_trainer_00_01, 1, 1, MSG_NO_PUT, 
									FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_WHITE, FONT_TALK );
	return;
}
#else
asm static void CastleEnemy_Default_Write( CASTLE_ENEMY_WORK* wk )
{
	push {r3, r4, lr}
	sub sp, #0x1c
	mov r2, #5
	str r2, [sp]
	mov r1, #0xff
	str r1, [sp, #4]
	mov r3, #1
	str r3, [sp, #8]
	mov r1, #2
	str r1, [sp, #0xc]
	mov r1, #0
	str r1, [sp, #0x10]
	str r1, [sp, #0x14]
	add r4, r0, #0
	add r1, r4, #0
	str r3, [sp, #0x18]
	add r1, #0x60
	mov r3, #0x10
	bl CastleWriteMsg_Full_ov107_2247680
	strb r0, [r4, #0xa]
	mov r0, #0x52
	lsl r0, r0, #2
	ldr r0, [r4, r0]
	bl CONFIG_GetWindowType
	add r1, r0, #0
	add r0, r4, #0
	add r0, #0xd0
	bl CastleTalkWinPut
	mov r3, #1
	add r1, r4, #0
	str r3, [sp]
	mov r0, #0xff
	str r0, [sp, #4]
	str r3, [sp, #8]
	mov r0, #2
	str r0, [sp, #0xc]
	mov r0, #0xf
	str r0, [sp, #0x10]
	add r0, r4, #0
	add r1, #0xd0
	mov r2, #4
	str r3, [sp, #0x14]
	bl CastleWriteMsg
	strb r0, [r4, #0xa]
	add sp, #0x1c
	pop {r3, r4, pc}
}
#endif

//--------------------------------------------------------------------------------------------
/**
 * 初期状態を削除
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CastleEnemy_Default_Del( CASTLE_ENEMY_WORK* wk )
{
	//BmpMenuWinClear( &wk->bmpwin[ENEMY_BMPWIN_TALKMENU], WINDOW_TRANS_OFF );
	//GF_BGL_BmpWinOffVReq( &wk->bmpwin[ENEMY_BMPWIN_TALKMENU] );
	BmpTalkWinClearSub( &wk->bmpwin[ENEMY_BMPWIN_TALKMENU] );
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * 基本状態を表示
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CastleEnemy_Basic_Write( CASTLE_ENEMY_WORK* wk )
{
	//下にメニューウィンドウ2で説明を表示
	//CastleWriteMenuWin( wk->bgl, &wk->bmpwin[ENEMY_BMPWIN_TALKMENU2] );
	CastleTalkWinPut( &wk->bmpwin[ENEMY_BMPWIN_TALKMENU2], CONFIG_GetWindowType(wk->config));

	/*「ポケモンの正体がわかります」*/
	//wk->msg_index = CastleWriteMsg( wk, &wk->bmpwin[ENEMY_BMPWIN_TALKMENU2], 
	//								msg_castle_trainer_00_03, 1, 1, MSG_NO_PUT, 
	//								FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_WHITE, BC_FONT );

	wk->list_csr_pos = 0;
	Castle_BasicListMake( wk );
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * 基本状態を削除
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CastleEnemy_Basic_Del( CASTLE_ENEMY_WORK* wk )
{
	BmpListPosGet( wk->lw, &wk->basic_list_lp, &wk->basic_list_cp );
	//BmpMenuWinClear( &wk->bmpwin[ENEMY_BMPWIN_TALKMENU2], WINDOW_TRANS_OFF );
	//GF_BGL_BmpWinOffVReq( &wk->bmpwin[ENEMY_BMPWIN_TALKMENU2] );
	BmpTalkWinClearSub( &wk->bmpwin[ENEMY_BMPWIN_TALKMENU2] );
	CastleEnemy_SeqSubListEnd( wk );
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * 「つよさ、わざ、ランクアップ、やめる」状態を表示
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CastleEnemy_Basic2_Write( CASTLE_ENEMY_WORK* wk )
{
	//下にメニューウィンドウ2で説明を表示
	//CastleWriteMenuWin( wk->bgl, &wk->bmpwin[ENEMY_BMPWIN_TALKMENU2] );
	CastleTalkWinPut( &wk->bmpwin[ENEMY_BMPWIN_TALK], CONFIG_GetWindowType(wk->config));

	/*「ポケモンのつよさがわかります」*/
	wk->msg_index = CastleWriteMsg( wk, &wk->bmpwin[ENEMY_BMPWIN_TALK], 
									msg_castle_trainer_09_05, 1, 1, MSG_NO_PUT, 
									FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_WHITE, FONT_TALK );

	wk->list_csr_pos = 0;
	Castle_Basic2ListMake( wk );
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * 「つよさ、わざ、ランクアップ、やめる」状態を削除
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CastleEnemy_Basic2_Del( CASTLE_ENEMY_WORK* wk )
{
	//BmpMenuWinClear( &wk->bmpwin[ENEMY_BMPWIN_TALKMENU2], WINDOW_TRANS_OFF );
	//GF_BGL_BmpWinOffVReq( &wk->bmpwin[ENEMY_BMPWIN_TALKMENU2] );
	CastleEnemy_SeqSubListEnd( wk );
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
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 * @param	type	チェックするタイプ
 *
 * @return	"TRUE = typeが同じ、FALSE = typeが違う"
 */
//--------------------------------------------------------------
static BOOL Castle_CheckType( CASTLE_ENEMY_WORK* wk, u8 type )
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
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 * @param	seq		シーケンスのポインタ
 * @param	next	次のシーケンス定義
 *
 * @return	none
 */
//--------------------------------------------------------------
static void NextSeq( CASTLE_ENEMY_WORK* wk, int* seq, int next )
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
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void BgCheck( CASTLE_ENEMY_WORK* wk )
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
static void CsrMove( CASTLE_ENEMY_WORK* wk, int key )
{
	u8 poke_pos;
	int flag;

	flag = 0;
	poke_pos = GetCsrPokePos( wk->h_max, wk->csr_pos );

	///////////////////////////////////////////////////////////////////////////////////////////
	if( sys.trg & PAD_KEY_LEFT ){

		if( wk->csr_pos == wk->modoru_pos ){
			return;
		}

		if( poke_pos == 0 ){
			wk->csr_pos+=(wk->h_max-1);
		}else{
			wk->csr_pos--;
		}

		flag = 1;
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	if( sys.trg & PAD_KEY_RIGHT ){

		if( wk->csr_pos == wk->modoru_pos ){
			return;
		}

		if( poke_pos == (wk->h_max - 1) ){
			wk->csr_pos-=(wk->h_max-1);
		}else{
			wk->csr_pos++;
		}

		flag = 1;
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	if( sys.trg & PAD_KEY_UP ){

		//「もどる」と縦にきれいに並んでいないので保留
		if( wk->csr_pos < wk->h_max ){
			return;
		}

		wk->csr_pos = wk->tmp_csr_pos;
		flag = 1;
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	if( sys.trg & PAD_KEY_DOWN ){

		//「もどる」と縦にきれいに並んでいないので保留
		if( wk->csr_pos >= wk->modoru_pos ){
			return;
		}

		wk->tmp_csr_pos = wk->csr_pos;
		wk->csr_pos = wk->modoru_pos;
		flag = 1;
	}

	if( flag == 1 ){
		CsrMoveSub( wk );
	}

	return;
}

static void CsrMoveSub( CASTLE_ENEMY_WORK* wk )
{
	Snd_SePlay( SEQ_SE_DP_SELECT );

	//通信タイプの時は、カーソル位置を送信
	if( Castle_CommCheck(wk->type) == TRUE ){
		CastleEnemy_CommSetSendBuf( wk, CASTLE_COMM_ENEMY_CSR_POS, wk->csr_pos );
	}

	//ポケモン選択ウィンドウ
	//PokeSelMoveSub( wk, wk->csr_pos, CommGetCurrentID() );
	PokeSelMoveSub( wk, wk->csr_pos, 0 );
	return;
}

//ポケモン選択ウィンドウ
static void PokeSelMoveSub( CASTLE_ENEMY_WORK* wk, u8 csr_pos, u8 flag )
{
	CASTLE_OBJ* obj;
	u32 start_x,start_y,anm_no,modoru_anm_no;

	if( flag == 0 ){
		obj = wk->p_poke_sel;
		anm_no = CASTLE_ANM_SELECT_ZIBUN;
		modoru_anm_no = CASTLE_ANM_MODORU_ZIBUN;
	}else{
		obj = wk->p_pair_poke_sel;
		anm_no = CASTLE_ANM_SELECT_AITE;
		modoru_anm_no = CASTLE_ANM_MODORU_AITE;
	}

	//「もどる」ポケモン選択ウィンドのの表示切替
	if( csr_pos >= wk->modoru_pos ){
		CastleObj_AnmChg( obj, modoru_anm_no );
		CastleObj_SetObjPos( obj, ENEMY_CSR_MODORU_X, ENEMY_CSR_MODORU_Y );
	}else{
		CastleObj_AnmChg( obj, anm_no );
		CastleEnemy_GetPokeSelXY( wk, &start_x, &start_y, csr_pos );
		CastleObj_SetObjPos( obj, start_x, start_y );
	}

	return;
}

//ポケモン選択ウィンドウオフセット取得
static void CastleEnemy_GetPokeSelXY( CASTLE_ENEMY_WORK* wk, u32* x, u32* y, u8 csr_pos )
{
	if( Castle_CommCheck(wk->type) == TRUE ){
		if( csr_pos == 0 ){
			*x = CASTLE_MULTI_POKE_SEL_START_X;
		}else if( csr_pos == 1 ){
			*x = CASTLE_MULTI_POKE_SEL_START_X2;
		}else if( csr_pos == 2 ){
			*x = CASTLE_MULTI_POKE_SEL_START_X3;
		}else{
			*x = CASTLE_MULTI_POKE_SEL_START_X4;
		}
	}else{
		if( csr_pos == 0 ){
			*x = CASTLE_POKE_SEL_START_X;
		}else if( csr_pos == 1 ){
			*x = CASTLE_POKE_SEL_START_X2;
		}else{
			*x = CASTLE_POKE_SEL_START_X3;
		}
	}

	*y = CASTLE_POKE_SEL_START_Y;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	カーソルＸ取得
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	"X"
 */
//--------------------------------------------------------------
static u16 GetCsrX( CASTLE_ENEMY_WORK* wk, u8 csr_pos, u8 lr )
{
	u8 poke_pos;
	int ret;
	u16 x,y,offset_x,offset_y,pair_offset_x,pair_offset_y,lr_offset_x;

	poke_pos = GetCsrPokePos( wk->h_max, csr_pos );

	if( lr == 0 ){						//左
		lr_offset_x = SEL_CSR_L_OFFSET;
	}else{								//右
		lr_offset_x = SEL_CSR_R_OFFSET;
	}

	if( csr_pos >= wk->modoru_pos ){
		ret = (ENEMY_CSR_MODORU_X + lr_offset_x);
		return ret;
	}

	Castle_GetOffset( wk, &offset_x, &offset_y, &pair_offset_x, &pair_offset_y );

	if( Castle_CommCheck(wk->type) == TRUE ){

		if( poke_pos < wk->h_max ){
			ret = poke_pos * CSR_WIDTH_X + lr_offset_x + offset_x;
		}else{
			ret = poke_pos * CSR_WIDTH_X + lr_offset_x + pair_offset_x;
		}

		//OS_Printf( "GetCsrX = %d\n", ret );
		return ret;
	}

	ret = poke_pos * CSR_WIDTH_X + lr_offset_x + offset_x;
	return ret;
}

//--------------------------------------------------------------
/**
 * @brief	カーソルＹ取得
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	"Y"
 */
//--------------------------------------------------------------
static u16 GetCsrY( CASTLE_ENEMY_WORK* wk, u8 csr_pos )
{
	u16 ret;

	if( csr_pos >= wk->modoru_pos ){
		return ENEMY_CSR_MODORU_Y;
	}

	ret = CSR_START_Y;
	//OS_Printf( " GetCsrY = %d\n", ret );
	return ret;
}

//--------------------------------------------------------------
/**
 * @brief	オフセット取得
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_GetOffset( CASTLE_ENEMY_WORK* wk, u16* offset_x, u16* offset_y, u16* pair_offset_x, u16* pair_offset_y )
{
	//通信しているかで表示のオフセットが変わる
	if( Castle_CommCheck(wk->type) == FALSE ){
		*offset_x		= INFO_SINGLE_OFFSET_X;
		*offset_y		= 0;
		*pair_offset_x	= 0;
		*pair_offset_y	= 0;
	}else{
		*offset_x		= INFO_MULTI_L_OFFSET_X;
		*offset_y		= 0;
		*pair_offset_x	= INFO_MULTI_R_OFFSET_X;
		*pair_offset_y	= 0;
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	選択されたカーソル位置を渡して、必要な演出を処理する
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	"TRUE=終了、FALSE=継続"
 */
//--------------------------------------------------------------
static BOOL CastleEnemy_DecideEff( CASTLE_ENEMY_WORK* wk, u8 csr_pos, u8 sel_type )
{
	u16 eff_offset_x;
	u32 anm_no;
	u8 poke_pos,type_offset;
	POKEMON_PARAM* poke;

	//オフセット
	type_offset = wk->modoru_pos;

	poke_pos = GetCsrPokePos( wk->h_max, csr_pos );
	poke =  PokeParty_GetMemberPointer( wk->p_party, poke_pos );

	switch( sel_type ){

	//「てもち」
	case FC_PARAM_TEMOTI:
		wk->eff_init_flag = 0;
		return TRUE;
		break;

	//「レベル」
	case FC_PARAM_LEVEL:

		//初期設定
		if( wk->eff_init_flag == 0 ){
			wk->eff_init_flag = 1;

			//アニメナンバーを取得
			//if( wk->level_sel == 1 ){
			if( wk->parent_decide_updown == 1 ){
				anm_no = CASTLE_ANM_LV_UP;
			}else{
				anm_no = CASTLE_ANM_LV_DOWN;
			}

			if( Castle_CommCheck(wk->type) == FALSE ){
				eff_offset_x	= LV_UPDOWN_START_X;
			}else{
				eff_offset_x	= LV_UPDOWN_MULTI_START_X;
			}

			//エフェクトOBJ追加
			wk->p_eff = CastleObj_Create(	&wk->castle_clact, 
											ID_CHAR_CSR, ID_PLTT_CSR, 
											ID_CELL_CSR, anm_no, 
											eff_offset_x + (LV_UPDOWN_WIDTH_X * poke_pos),
											LV_UPDOWN_START_Y,
											CASTLE_BG_PRI_HIGH, NULL );
		}

		//アニメーション終了待ち
		if( CastleObj_AnmActiveCheck(wk->p_eff) == FALSE ){
			CastleObj_Delete( wk->p_eff );
			wk->p_eff = NULL;
			wk->eff_init_flag = 0;
			return TRUE;
		}
		break;

	//「つよさ」
	case FC_PARAM_TUYOSA:
		if( Castle_CommCheck(wk->type) == FALSE ){
			if( sys.trg & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL) ){
				Snd_SePlay( SEQ_SE_DP_SELECT );
				CastleEnemy_SeqSubTuyosa( wk, csr_pos );					//つよさ
				wk->eff_init_flag = 0;
				return TRUE;
			}
		}else{
			wk->eff_init_flag = 0;
			return TRUE;
		}
		break;

	//「わざ」
	case FC_PARAM_WAZA:
		if( Castle_CommCheck(wk->type) == FALSE ){
			if( sys.trg & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL) ){
				Snd_SePlay( SEQ_SE_DP_SELECT );
				CastleEnemy_SeqSubWaza( wk, csr_pos );								//わざ
				wk->eff_init_flag = 0;
				return TRUE;
			}
		}else{
			wk->eff_init_flag = 0;
			return TRUE;
		}
		break;

	//「じょうほうランクアップ」
	case FC_PARAM_INFO_RANKUP:
		wk->eff_init_flag = 0;
		return TRUE;
		break;

	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	選択されたカーソル位置を渡して、必要な演出を処理する
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	"TRUE=終了、FALSE=継続"
 */
//--------------------------------------------------------------
static BOOL CastleEnemy_DecideEff2( CASTLE_ENEMY_WORK* wk, u8 csr_pos, u8 sel_type )
{
	u32 anm_no;
	u8 poke_pos,type_offset;
	POKEMON_PARAM* poke;

	//オフセット
	type_offset = wk->modoru_pos;

	poke_pos = GetCsrPokePos( wk->h_max, csr_pos );
	poke =  PokeParty_GetMemberPointer( wk->p_party, poke_pos );

	switch( sel_type ){

	//「てもち」
	//「レベル」
	//「じょうほうランクアップ」
	case FC_PARAM_TEMOTI:
	case FC_PARAM_LEVEL:
	case FC_PARAM_INFO_RANKUP:
		wk->eff_init_flag = 0;
		return TRUE;
		break;

	//「つよさ」
	case FC_PARAM_TUYOSA:
		if( Castle_CommCheck(wk->type) == TRUE ){
			if( CommGetCurrentID() == COMM_PARENT_ID ){						//親
				if( wk->parent_decide_pos >= type_offset ){					//親：子の決定が有効なら
					wk->eff_init_flag = 0;
					return TRUE;
				}
			}else{															//子
				if( wk->parent_decide_pos < type_offset ){					//子：親の決定が有効なら
					wk->eff_init_flag = 0;
					return TRUE;
				}
			}
		}

#if 0
	
		if( wk->eff_init_flag == 0 ){
			CastleEnemy_SeqSubTuyosa( wk, csr_pos );						//つよさ
			wk->eff_init_flag = 1;
			return FALSE;
		}

		if( sys.trg & PAD_KEY_LEFT ){
			CastleEnemy_TuyosaWinChg( wk, -1 );
		}else if( sys.trg & PAD_KEY_RIGHT ){
			CastleEnemy_TuyosaWinChg( wk, 1 );
		}else if( sys.trg & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL) ){
			Snd_SePlay( SEQ_SE_DP_SELECT );
			wk->eff_init_flag = 0;
			return TRUE;
		}
#else
		if( wk->eff_init_flag == 0 ){
			wk->recover_flag = RECOVER_STATUS;						//通信で自分がステータス開放した
			CastleEnemy_SeqSubTuyosa( wk, csr_pos );						//つよさ
			wk->eff_init_flag = 0;
			return TRUE;
		}
#endif
		break;

	//「わざ」
	case FC_PARAM_WAZA:
		if( Castle_CommCheck(wk->type) == TRUE ){
			if( CommGetCurrentID() == COMM_PARENT_ID ){						//親
				if( wk->parent_decide_pos >= type_offset ){					//親：子の決定が有効なら
					wk->eff_init_flag = 0;
					return TRUE;
				}
			}else{															//子
				if( wk->parent_decide_pos < type_offset ){					//子：親の決定が有効なら
					wk->eff_init_flag = 0;
					return TRUE;
				}
			}
		}

#if 0
		if( wk->eff_init_flag == 0 ){
			CastleEnemy_SeqSubWaza( wk, csr_pos );							//わざ
			wk->eff_init_flag = 1;
			return FALSE;
		}

		if( sys.trg & PAD_KEY_LEFT ){
			CastleEnemy_WazaWinChg( wk, -1 );
		}else if( sys.trg & PAD_KEY_RIGHT ){
			CastleEnemy_WazaWinChg( wk, 1 );
		}else if( sys.trg & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL) ){
			Snd_SePlay( SEQ_SE_DP_SELECT );
			wk->eff_init_flag = 0;
			return TRUE;
		}
#else
		if( wk->eff_init_flag == 0 ){
			wk->recover_flag = RECOVER_STATUS;						//通信で自分がステータス開放した
			CastleEnemy_SeqSubWaza( wk, csr_pos );							//わざ
			wk->eff_init_flag = 0;
			return TRUE;
		}
#endif
		break;

	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	レベル操作の使用CPを取得
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static u16 CastleEnemy_GetLvCP( u8 level_sel )
{
	if( level_sel == 1 ){
		return CP_USE_LEVEL_PLUS;
	}

	return CP_USE_LEVEL_MINUS;
}

//--------------------------------------------------------------
/**
 * @brief	つよさウィンドウを表示している時に左右で切り替え
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleEnemy_TuyosaWinChg( CASTLE_ENEMY_WORK* wk, s8 add_sub )
{
	POKEMON_PARAM* poke;
	s8 pos;

	pos	= wk->csr_pos;

	while( 1 ){

		pos += add_sub;

		if( pos < 0 ){
			pos = (wk->h_max - 1);
		}else if( pos >= wk->h_max ){
			pos = 0;
		}

		//一周チェックした
		if( pos == wk->csr_pos ){
			return;
		}

		//ウィンドウを切り替え
		if( wk->p_tuyosa_flag[pos] == 1 ){			//フラグON
			wk->csr_pos = pos;
			CsrMoveSub( wk );
			poke =  PokeParty_GetMemberPointer( wk->p_party, GetCsrPokePos(wk->h_max,wk->csr_pos) );
			Castle_StatusMsgWrite( wk, &wk->bmpwin[ENEMY_BMPWIN_STATUS], poke );
			return;
		}
	};

	return;
}

//--------------------------------------------------------------
/**
 * @brief	わざウィンドウを表示している時に左右で切り替え
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleEnemy_WazaWinChg( CASTLE_ENEMY_WORK* wk, s8 add_sub )
{
	POKEMON_PARAM* poke;
	s8 pos;

	pos	= wk->csr_pos;

	while( 1 ){

		pos += add_sub;

		if( pos < 0 ){
			pos = (wk->h_max - 1);
		}else if( pos >= wk->h_max ){
			pos = 0;
		}

		//一周チェックした
		if( pos == wk->csr_pos ){
			return;
		}

		//ウィンドウを切り替え
		if( wk->p_waza_flag[pos] == 1 ){			//フラグON
			wk->csr_pos = pos;
			CsrMoveSub( wk );
			poke =  PokeParty_GetMemberPointer( wk->p_party, GetCsrPokePos(wk->h_max,wk->csr_pos) );
			Castle_WazaMsgWrite( wk, &wk->bmpwin[ENEMY_BMPWIN_STATUS], poke );
			return;
		}
	};

	return;
}

//--------------------------------------------------------------
/**
 * @brief	ペアが何か決定した時の強制終了処理
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleEnemy_PairDecideDel( CASTLE_ENEMY_WORK* wk )
{
	CastleEnemy_SeqSubMenuWinClear( wk );
	CastleEnemy_SeqSubListEnd( wk );
	BmpTalkWinClearSub( &wk->bmpwin[ENEMY_BMPWIN_TALK] );
	CastleObj_Vanish( wk->p_eff_rankup, CASTLE_VANISH_ON );			//非表示
	return;
}

//--------------------------------------------------------------
/**
 * @brief	メッセージウィンドウ削除
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void BmpTalkWinClearSub( GF_BGL_BMPWIN* win )
{
	BmpTalkWinClear( win, WINDOW_TRANS_OFF );
	GF_BGL_BmpWinOffVReq( win );
	return;
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
 * @param	wk			CASTLE_RANK_WORK型のポインタ
 * @param	type		送信タイプ
 *
 * @retval	none
 */
//--------------------------------------------------------------
BOOL CastleEnemy_CommSetSendBuf( CASTLE_ENEMY_WORK* wk, u16 type, u16 param )
{
	int ret,command;

	switch( type ){

	//名前
	case CASTLE_COMM_ENEMY_PAIR:
		command = FC_CASTLE_ENEMY_PAIR;
		CastleEnemy_CommSendBufBasicData( wk, type );
		break;

	//リクエスト
	case CASTLE_COMM_ENEMY_UP_TYPE:
		command = FC_CASTLE_ENEMY_REQ_TYPE;
		CastleEnemy_CommSendBufRankUpType( wk, type, param );
		break;

	//カーソル位置
	case CASTLE_COMM_ENEMY_CSR_POS:
		command = FC_CASTLE_ENEMY_CSR_POS;
		CastleEnemy_CommSendBufCsrPos( wk, type );
		break;

	//「戻る」
	case CASTLE_COMM_ENEMY_MODORU:
		command = FC_CASTLE_ENEMY_MODORU;
		CastleEnemy_CommSendBufModoru( wk );
		break;
	};

	if( CommSendData(command,wk->send_buf,CASTLE_COMM_BUF_LEN) == TRUE ){
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
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
void CastleEnemy_CommSendBufBasicData( CASTLE_ENEMY_WORK* wk, u16 type )
{
	int i,num;
	MYSTATUS* my;
	
	OS_Printf( "******キャッスル****** 基本情報送信\n" );

	num = 0;
	my	= SaveData_GetMyStatus( wk->sv );

	wk->send_buf[num] = type;
	OS_Printf( "送信：type = %d\n", wk->send_buf[0] );
	num+=1;														//1

	wk->send_buf[num]	= MyStatus_GetMySex(my);				//性別
	OS_Printf( "送信：my sex = %d\n", wk->send_buf[1] );
	num+=1;														//2

	//ランク
	for( i=0; i < CASTLE_RANK_TYPE_MAX ;i++ ){
		wk->send_buf[num+i] = Castle_GetRank( wk->sv, wk->type, i );
		OS_Printf( "送信：rank = %d\n", wk->send_buf[num+i] );
	}
	num+=CASTLE_RANK_TYPE_MAX;									//5

	//wk->send_buf[num] = CASTLESCORE_GetCP( wk->score_sv );
	//OS_Printf( "送信：cp = %d\n", wk->send_buf[num] );
	//num+=1;														//6

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
void CastleEnemy_CommRecvBufBasicData(int id_no,int size,void *pData,void *work)
{
	int i,num;
	CASTLE_ENEMY_WORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******キャッスル****** 基本情報受信\n" );

	num = 0;
	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	//type = recv_buf[0];
	num+=1;													//1
	
	//wk-> = ;
	num+=1;													//2

	for( i=0; i < CASTLE_RANK_TYPE_MAX ;i++ ){
		wk->pair_rank[i] = (u8)recv_buf[num+i];				//ランク
		OS_Printf( "受信：wk->pair_rank[%d] = %d\n", i, wk->pair_rank[i] );
	}
	num+=CASTLE_RANK_TYPE_MAX;								//5

	//wk->pair_cp = (u16)recv_buf[num];						//CP
	//OS_Printf( "受信：wk->pair_cp = %d\n", wk->pair_cp );
	//num+=1;													//6

	return;
}

//--------------------------------------------------------------
/**
 * @brief	send_bufにリクエスト(どれをランクアップしたい)をセット
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
void CastleEnemy_CommSendBufRankUpType( CASTLE_ENEMY_WORK* wk, u16 type, u16 param )
{
	OS_Printf( "******キャッスル****** ランクアップしたいリクエスト情報送信\n" );

	//コマンド
	wk->send_buf[0] = type;
	OS_Printf( "送信：type = %d\n", wk->send_buf[0] );

	//カーソル位置
	wk->send_buf[1] = param;
	OS_Printf( "送信：csr_pos = %d\n", wk->send_buf[1] );

	//先に子の選択がきていなくて、まだ値が入っていない時は、親の決定はセットしてしまう
	if( CommGetCurrentID() == COMM_PARENT_ID ){
		if( wk->parent_decide_pos == CASTLE_ENEMY_DECIDE_NONE ){
			wk->parent_decide_pos = param;
			//wk->parent_decide_lr = (wk->csr_pos % wk->h_max);
		}
	}

	//親の決定タイプ
	wk->send_buf[2] = wk->parent_decide_pos;
	OS_Printf( "送信：parent_decide_pos = %d\n", wk->send_buf[2] );

	//パートナーが左、右で、自分、相手のどちらのランクを上げようとしているか
	//wk->send_buf[3] = (wk->csr_pos % wk->h_max);
	//OS_Printf( "送信：rankup_lr = %d\n", wk->send_buf[3] );

	//レベル上げ下げ
	wk->send_buf[4] = wk->parent_decide_updown;
	OS_Printf( "送信：lv_updown = %d\n", wk->send_buf[4] );

	//決定したこと
	wk->send_buf[5] = wk->parent_decide_type;
	OS_Printf( "送信：parent_decide_type = %d\n", wk->send_buf[5] );

	return;
}

//--------------------------------------------------------------
/**
 * @brief   ランクアップしたいリクエスト 通信受信処理
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		FRONTIER_SYSTEMへのポインタ
 */
//--------------------------------------------------------------
void CastleEnemy_CommRecvBufRankUpType(int id_no,int size,void *pData,void *work)
{
	int i,num;
	CASTLE_ENEMY_WORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******キャッスル****** ランクアップしたいリクエスト情報受信\n" );

	num = 0;
	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	//コマンド
	//recv_buf[0]
	
	wk->pair_rankup_type = recv_buf[1];
	OS_Printf( "受信：wk->pair_rankup_type = %d\n", wk->pair_rankup_type );

	////////////////////////////////////////////////////////////////////////
	//親
	if( CommGetCurrentID() == COMM_PARENT_ID ){

		//親の決定がすでに決まっていたら、子の選択は無効
		if( wk->parent_decide_pos != CASTLE_ENEMY_DECIDE_NONE ){
			wk->pair_rankup_type= 0;
			//wk->parent_decide_lr= (wk->csr_pos % wk->h_max);
		}else{

			//親の決定が決まっていない時は、
			//親が送信する時に「子にそれでいいよ」と送信する
			//wk->parent_decide_pos	= wk->pair_rankup_type;
			
			//子の選択が採用されたことがわかるようにオフセット("modoru_pos")を加える
			wk->parent_decide_pos	= wk->pair_rankup_type + wk->modoru_pos;
			wk->parent_decide_updown= recv_buf[4];
			wk->parent_decide_type  = recv_buf[5];
		}
	////////////////////////////////////////////////////////////////////////
	//子
	}else{
		//親の決定タイプ
		wk->parent_decide_pos = recv_buf[2];
		
		//パートナーが左、右で、自分、相手のどちらのランクを上げようとしているか
		//wk->parent_decide_lr	= recv_buf[3];
		wk->parent_decide_updown= recv_buf[4];
		wk->parent_decide_type  = recv_buf[5];
	}

	OS_Printf( "受信：wk->parent_decide_pos = %d\n", wk->parent_decide_pos );
	//OS_Printf( "受信：parent_decide_lr = %d\n", wk->parent_decide_lr );
	OS_Printf( "受信：parent_decide_updown = %d\n", wk->parent_decide_updown );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	send_bufにカーソル位置をセット
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
void CastleEnemy_CommSendBufCsrPos( CASTLE_ENEMY_WORK* wk, u16 type )
{
	wk->send_buf[0] = type;
	OS_Printf( "送信：type = %d\n", wk->send_buf[0] );

	wk->send_buf[1]	= wk->csr_pos;
	OS_Printf( "送信：csr_pos = %d\n", wk->send_buf[1] );

	return;
}

//--------------------------------------------------------------
/**
 * @brief   カーソル位置 通信受信処理
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		FRONTIER_SYSTEMへのポインタ
 */
//--------------------------------------------------------------
void CastleEnemy_CommRecvBufCsrPos(int id_no,int size,void *pData,void *work)
{
	CASTLE_ENEMY_WORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******キャッスル****** カーソル位置情報受信\n" );

	//wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	//type = recv_buf[0];
	
	wk->pair_csr_pos = (u8)recv_buf[1];						//カーソル位置
	OS_Printf( "受信：wk->pair_csr_pos = %d\n", wk->pair_csr_pos );

	//ポケモン選択ウィンドウ
	//PokeSelMoveSub( wk, wk->pair_csr_pos, (CommGetCurrentID() ^ 1) );
	PokeSelMoveSub( wk, wk->pair_csr_pos, 1 );

	return;
}

//--------------------------------------------------------------
/**
 * @brief	send_bufに「戻る」をセット
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
void CastleEnemy_CommSendBufModoru( CASTLE_ENEMY_WORK* wk )
{
	OS_Printf( "******キャッスル****** 「戻る」情報送信\n" );

	wk->send_buf[0] = 1;
	OS_Printf( "送信：戻る = %d\n", wk->send_buf[0] );
	return;
}

//--------------------------------------------------------------
/**
 * @brief   「戻る」 通信受信処理
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		FRONTIER_SYSTEMへのポインタ
 */
//--------------------------------------------------------------
void CastleEnemy_CommRecvBufModoru(int id_no,int size,void *pData,void *work)
{
	CASTLE_ENEMY_WORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******キャッスル****** 「戻る」情報受信\n" );

	//wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	wk->pair_modoru_flag = (u8)recv_buf[0];
	OS_Printf( "受信：wk->pair_modoru_flag = %d\n", wk->pair_modoru_flag );
	return;
}


//==============================================================================================
//
//	サブシーケンス
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief		持っている道具、性格、特性、各能力値を表示
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleEnemy_SeqSubTuyosa( CASTLE_ENEMY_WORK* wk, u8 csr_pos )
{
	POKEMON_PARAM* poke;

	//つよさ、わざが公開されていたら旗を表示する
	//CastleEnemy_SeqSubTuyosaFlagOn( wk, csr_pos );
	//CastleRank_SeqSubHataVanish( wk );

	Castle_SetStatusBgGraphic( wk, BC_FRAME_SLIDE );
	BmpTalkWinClearSub( &wk->bmpwin[ENEMY_BMPWIN_TALK] );
	GF_BGL_BmpWinOffVReq( &wk->bmpwin[ENEMY_BMPWIN_TALK] );
	poke =  PokeParty_GetMemberPointer( wk->p_party, GetCsrPokePos(wk->h_max,csr_pos) );
	Castle_StatusMsgWrite( wk, &wk->bmpwin[ENEMY_BMPWIN_STATUS], poke );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );

	//Snd_SePlay( SEQ_SE_DP_DENSI16 );
	//Snd_SePlay( SEQ_SE_DP_UG_020 );
	return;
}

//--------------------------------------------------------------
/**
 * @brief		つよさ公開フラグオン
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleEnemy_SeqSubTuyosaFlagOn( CASTLE_ENEMY_WORK* wk, u8 csr_pos )
{
	wk->p_tuyosa_flag[GetCsrPokePos(wk->h_max,csr_pos)] = 1;			//フラグON
	return;
}

//「つよさ」見えるようになったMSG
static void CastleEnemy_SeqSubTuyosaMsgSet( CASTLE_ENEMY_WORK* wk, u8 csr_pos )
{
	POKEMON_PARAM* poke;

	CastleTalkWinPut( &wk->bmpwin[ENEMY_BMPWIN_TALK], CONFIG_GetWindowType(wk->config));
	poke =  PokeParty_GetMemberPointer( wk->p_party, GetCsrPokePos(wk->h_max,csr_pos) );
	Castle_SetPokeName( wk, 0, PPPPointerGet(poke) );
	wk->msg_index = Castle_EasyMsg( wk, msg_castle_trainer_06_04, FONT_TALK );

	//つよさが公開されていたら旗を表示する
	CastleEnemy_SeqSubTuyosaFlagOn( wk, csr_pos );
	CastleRank_SeqSubHataVanish( wk );
	Snd_SePlay( SEQ_SE_DP_UG_020 );
	return;
}

//--------------------------------------------------------------
/**
 * @brief		技名、ＰＰ名を表示
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleEnemy_SeqSubWaza( CASTLE_ENEMY_WORK* wk, u8 csr_pos )
{
	POKEMON_PARAM* poke;

	//つよさ、わざが公開されていたら旗を表示する
	//CastleEnemy_SeqSubWazaFlagOn( wk, csr_pos );
	//CastleRank_SeqSubHataVanish( wk );

	Castle_SetWazaBgGraphic( wk, BC_FRAME_SLIDE );
	BmpTalkWinClearSub( &wk->bmpwin[ENEMY_BMPWIN_TALK] );
	GF_BGL_BmpWinOffVReq( &wk->bmpwin[ENEMY_BMPWIN_TALK] );
	poke =  PokeParty_GetMemberPointer( wk->p_party, GetCsrPokePos(wk->h_max,csr_pos) );
	Castle_WazaMsgWrite( wk, &wk->bmpwin[ENEMY_BMPWIN_STATUS], poke );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );

	//Snd_SePlay( SEQ_SE_DP_DENSI16 );
	//Snd_SePlay( SEQ_SE_DP_UG_020 );
	return;
}

//--------------------------------------------------------------
/**
 * @brief		わざ公開フラグオン
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleEnemy_SeqSubWazaFlagOn( CASTLE_ENEMY_WORK* wk, u8 csr_pos )
{
	wk->p_waza_flag[GetCsrPokePos(wk->h_max,csr_pos)] = 1;					//フラグON
	return;
}

static void CastleEnemy_SeqSubWazaMsgSet( CASTLE_ENEMY_WORK* wk, u8 csr_pos )
{
	POKEMON_PARAM* poke;

	CastleTalkWinPut( &wk->bmpwin[ENEMY_BMPWIN_TALK], CONFIG_GetWindowType(wk->config));
	poke =  PokeParty_GetMemberPointer( wk->p_party, GetCsrPokePos(wk->h_max,csr_pos) );
	Castle_SetPokeName( wk, 0, PPPPointerGet(poke) );
	wk->msg_index = Castle_EasyMsg( wk, msg_castle_trainer_07_04, FONT_TALK );

	//わざが公開されていたら旗を表示する
	CastleEnemy_SeqSubWazaFlagOn( wk, csr_pos );
	CastleRank_SeqSubHataVanish( wk );
	Snd_SePlay( SEQ_SE_DP_UG_020 );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	メニューウィンドウクリア
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleEnemy_SeqSubMenuWinClear( CASTLE_ENEMY_WORK* wk )
{
	if( wk->menu_flag == 1 ){
		wk->menu_flag = 0;
		BmpMenuExit( wk->mw, NULL );
		BmpMenuWinClear( wk->MenuH.win, WINDOW_TRANS_OFF );
		GF_BGL_BmpWinOffVReq( wk->MenuH.win );
	}
	return;
}

//--------------------------------------------------------------
/**
 * @brief	会話ウィンドウON
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleEnemy_SeqSubTalkWinOn( CASTLE_ENEMY_WORK* wk )
{
	CastleTalkWinPut( &wk->bmpwin[ENEMY_BMPWIN_TALK], CONFIG_GetWindowType(wk->config));
	return;
}

//--------------------------------------------------------------
/**
 * @brief	CP更新
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
#ifdef NONEQUIVALENT
static void CastleEnemy_SeqSubCPWrite( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win )
{
	u16 x,y,offset_x,offset_y,pair_offset_x,pair_offset_y,now_cp;

	Castle_GetOffset( wk, &offset_x, &offset_y, &pair_offset_x, &pair_offset_y );

	if( Castle_CommCheck(wk->type) == FALSE ){
		x = offset_x + INFO_PLAYER_CP_X;
		y = offset_y + INFO_PLAYER_CP_Y;
		GF_BGL_BmpWinFill( win, FBMP_COL_NULL, x, y, 6*8, 2*8 );

		now_cp = FrontierRecord_Get(wk->fro_sv, CastleScr_GetCPRecordID(wk->type),
									Frontier_GetFriendIndex(CastleScr_GetCPRecordID(wk->type)) );

		Castle_SetNumber(	wk, 0, now_cp, 
							CASTLE_KETA_CP, NUMBER_DISPTYPE_SPACE );
		wk->msg_index = CastleWriteMsgSimple( wk, win, 
									msg_castle_trainer_cp_01, x, y , MSG_NO_PUT, 
									FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, BC_FONT );
	}else{

		//親、子の画面とも、親、子の順番にCPが表示されるようにする

		//////////////////////////////////////////////////////////////////////////////
		//親なら
		if( CommGetCurrentID() == COMM_PARENT_ID ){
			x = offset_x + INFO_PLAYER_CP_X;
			y = offset_y + INFO_PLAYER_CP_Y;
			GF_BGL_BmpWinFill( win, FBMP_COL_NULL, x, y, 6*8, 2*8 );

			now_cp = FrontierRecord_Get(wk->fro_sv, CastleScr_GetCPRecordID(wk->type),
									Frontier_GetFriendIndex(CastleScr_GetCPRecordID(wk->type)) );

			Castle_SetNumber(	wk, 0, now_cp, 
								CASTLE_KETA_CP, NUMBER_DISPTYPE_SPACE );
			wk->msg_index = CastleWriteMsgSimple( wk, win, 
										msg_castle_trainer_cp_01, x, y , MSG_NO_PUT, 
										FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, BC_FONT );

			//パートナーのCPを表示
			x = pair_offset_x + INFO_PLAYER_CP_X;
			y = pair_offset_y + INFO_PLAYER_CP_Y;
			GF_BGL_BmpWinFill( win, FBMP_COL_NULL, x, y, 6*8, 2*8 );
			Castle_SetNumber( wk, 0, wk->pair_cp, CASTLE_KETA_CP, NUMBER_DISPTYPE_SPACE );
			wk->msg_index = CastleWriteMsgSimple( wk, win, 
										msg_castle_trainer_cp_01, x, y , MSG_NO_PUT, 
										FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, BC_FONT );

		//////////////////////////////////////////////////////////////////////////////
		//子なら
		}else{
			//パートナーのCPを表示
			x = offset_x + INFO_PLAYER_CP_X;
			y = offset_y + INFO_PLAYER_CP_Y;
			GF_BGL_BmpWinFill( win, FBMP_COL_NULL, x, y, 6*8, 2*8 );
			Castle_SetNumber( wk, 0, wk->pair_cp, CASTLE_KETA_CP, NUMBER_DISPTYPE_SPACE );
			wk->msg_index = CastleWriteMsgSimple( wk, win, 
										msg_castle_trainer_cp_01, x, y , MSG_NO_PUT, 
										FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, BC_FONT );

			x = pair_offset_x + INFO_PLAYER_CP_X;
			y = pair_offset_y + INFO_PLAYER_CP_Y;
			GF_BGL_BmpWinFill( win, FBMP_COL_NULL, x, y, 6*8, 2*8 );

			now_cp = FrontierRecord_Get(wk->fro_sv, CastleScr_GetCPRecordID(wk->type),
									Frontier_GetFriendIndex(CastleScr_GetCPRecordID(wk->type)) );

			Castle_SetNumber(	wk, 0, now_cp, 
								CASTLE_KETA_CP, NUMBER_DISPTYPE_SPACE );
			wk->msg_index = CastleWriteMsgSimple( wk, win, 
										msg_castle_trainer_cp_01, x, y , MSG_NO_PUT, 
										FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, BC_FONT );
		}
	}

	GF_BGL_BmpWinOnVReq( win );
	return;
}
#else
asm static void CastleEnemy_SeqSubCPWrite( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win )
{
	push {r3, r4, r5, r6, r7, lr}
	sub sp, #0x30
	add r4, r1, #0
	add r1, sp, #0x28
	str r1, [sp]
	add r1, sp, #0x2c
	add r3, sp, #0x28
	add r5, r0, #0
	add r1, #2
	add r2, sp, #0x2c
	add r3, #2
	bl Castle_GetOffset
	ldrb r0, [r5, #9]
	bl Castle_CommCheck
	cmp r0, #0
	bne _02248CA2
	add r1, sp, #0x28
	ldrh r0, [r1, #6]
	ldrh r6, [r1, #4]
	mov r1, #0
	add r0, #0x68
	lsl r0, r0, #0x10
	lsr r7, r0, #0x10
	add r2, r7, #0
	mov r0, #0x30
	sub r2, #0x30
	str r0, [sp]
	mov r0, #0x10
	lsl r2, r2, #0x10
	str r0, [sp, #4]
	add r0, r4, #0
	lsr r2, r2, #0x10
	add r3, r6, #0
	bl GF_BGL_BmpWinFill
	ldrb r0, [r5, #9]
	bl CastleScr_GetCPRecordID
	str r0, [sp, #0x1c]
	ldrb r0, [r5, #9]
	bl CastleScr_GetCPRecordID
	bl Frontier_GetFriendIndex
	add r2, r0, #0
	ldr r0, [r5, #4]
	ldr r1, [sp, #0x1c]
	bl FrontierRecord_Get
	add r2, r0, #0
	mov r0, #1
	str r0, [sp]
	add r0, r5, #0
	mov r1, #0
	mov r3, #4
	bl Castle_SetNumber
	str r6, [sp]
	mov r0, #0xff
	str r0, [sp, #4]
	mov r0, #1
	str r0, [sp, #8]
	mov r2, #2
	str r2, [sp, #0xc]
	mov r0, #0
	str r0, [sp, #0x10]
	str r0, [sp, #0x14]
	add r0, r5, #0
	add r1, r4, #0
	add r3, r7, #0
	str r2, [sp, #0x18]
	bl CastleWriteMsgSimple_Full_ov107_2247744
	strb r0, [r5, #0xa]
	b _02248E46
_02248CA2:
	bl CommGetCurrentID
	cmp r0, #0
	add r1, sp, #0x28
	bne _02248D7A
	ldrh r0, [r1, #6]
	ldrh r6, [r1, #4]
	mov r1, #0
	add r0, #0x68
	lsl r0, r0, #0x10
	lsr r7, r0, #0x10
	add r2, r7, #0
	mov r0, #0x30
	sub r2, #0x30
	str r0, [sp]
	mov r0, #0x10
	lsl r2, r2, #0x10
	str r0, [sp, #4]
	add r0, r4, #0
	lsr r2, r2, #0x10
	add r3, r6, #0
	bl GF_BGL_BmpWinFill
	ldrb r0, [r5, #9]
	bl CastleScr_GetCPRecordID
	str r0, [sp, #0x20]
	ldrb r0, [r5, #9]
	bl CastleScr_GetCPRecordID
	bl Frontier_GetFriendIndex
	add r2, r0, #0
	ldr r0, [r5, #4]
	ldr r1, [sp, #0x20]
	bl FrontierRecord_Get
	add r2, r0, #0
	mov r0, #1
	str r0, [sp]
	add r0, r5, #0
	mov r1, #0
	mov r3, #4
	bl Castle_SetNumber
	str r6, [sp]
	mov r0, #0xff
	str r0, [sp, #4]
	mov r0, #1
	str r0, [sp, #8]
	mov r2, #2
	str r2, [sp, #0xc]
	mov r0, #0
	str r0, [sp, #0x10]
	str r0, [sp, #0x14]
	add r0, r5, #0
	add r1, r4, #0
	add r3, r7, #0
	str r2, [sp, #0x18]
	bl CastleWriteMsgSimple_Full_ov107_2247744
	add r1, sp, #0x28
	strb r0, [r5, #0xa]
	ldrh r0, [r1, #2]
	ldrh r6, [r1]
	mov r1, #0
	add r0, #0x68
	lsl r0, r0, #0x10
	lsr r7, r0, #0x10
	add r2, r7, #0
	mov r0, #0x30
	sub r2, #0x30
	str r0, [sp]
	mov r0, #0x10
	lsl r2, r2, #0x10
	str r0, [sp, #4]
	add r0, r4, #0
	lsr r2, r2, #0x10
	add r3, r6, #0
	bl GF_BGL_BmpWinFill
	mov r0, #1
	str r0, [sp]
	ldr r2, =0x000003D6 // _02248E50
	add r0, r5, #0
	ldrh r2, [r5, r2]
	mov r1, #0
	mov r3, #4
	bl Castle_SetNumber
	str r6, [sp]
	mov r0, #0xff
	str r0, [sp, #4]
	mov r0, #1
	str r0, [sp, #8]
	mov r2, #2
	str r2, [sp, #0xc]
	mov r0, #0
	str r0, [sp, #0x10]
	str r0, [sp, #0x14]
	add r0, r5, #0
	add r1, r4, #0
	add r3, r7, #0
	str r2, [sp, #0x18]
	bl CastleWriteMsgSimple_Full_ov107_2247744
	strb r0, [r5, #0xa]
	b _02248E46
_02248D7A:
	ldrh r0, [r1, #6]
	ldrh r6, [r1, #4]
	mov r1, #0
	add r0, #0x68
	lsl r0, r0, #0x10
	lsr r7, r0, #0x10
	add r2, r7, #0
	mov r0, #0x30
	sub r2, #0x30
	str r0, [sp]
	mov r0, #0x10
	lsl r2, r2, #0x10
	str r0, [sp, #4]
	add r0, r4, #0
	lsr r2, r2, #0x10
	add r3, r6, #0
	bl GF_BGL_BmpWinFill
	mov r0, #1
	str r0, [sp]
	ldr r2, =0x000003D6 // _02248E50
	add r0, r5, #0
	ldrh r2, [r5, r2]
	mov r1, #0
	mov r3, #4
	bl Castle_SetNumber
	str r6, [sp]
	mov r0, #0xff
	str r0, [sp, #4]
	mov r0, #1
	str r0, [sp, #8]
	mov r2, #2
	str r2, [sp, #0xc]
	mov r0, #0
	str r0, [sp, #0x10]
	str r0, [sp, #0x14]
	add r0, r5, #0
	add r1, r4, #0
	add r3, r7, #0
	str r2, [sp, #0x18]
	bl CastleWriteMsgSimple_Full_ov107_2247744
	add r1, sp, #0x28
	strb r0, [r5, #0xa]
	ldrh r0, [r1, #2]
	ldrh r6, [r1]
	mov r1, #0
	add r0, #0x68
	lsl r0, r0, #0x10
	lsr r7, r0, #0x10
	add r2, r7, #0
	mov r0, #0x30
	sub r2, #0x30
	str r0, [sp]
	mov r0, #0x10
	lsl r2, r2, #0x10
	str r0, [sp, #4]
	add r0, r4, #0
	lsr r2, r2, #0x10
	add r3, r6, #0
	bl GF_BGL_BmpWinFill
	ldrb r0, [r5, #9]
	bl CastleScr_GetCPRecordID
	str r0, [sp, #0x24]
	ldrb r0, [r5, #9]
	bl CastleScr_GetCPRecordID
	bl Frontier_GetFriendIndex
	add r2, r0, #0
	ldr r0, [r5, #4]
	ldr r1, [sp, #0x24]
	bl FrontierRecord_Get
	add r2, r0, #0
	mov r0, #1
	str r0, [sp]
	add r0, r5, #0
	mov r1, #0
	mov r3, #4
	bl Castle_SetNumber
	str r6, [sp]
	mov r0, #0xff
	str r0, [sp, #4]
	mov r0, #1
	str r0, [sp, #8]
	mov r2, #2
	str r2, [sp, #0xc]
	mov r0, #0
	str r0, [sp, #0x10]
	str r0, [sp, #0x14]
	add r0, r5, #0
	add r1, r4, #0
	add r3, r7, #0
	str r2, [sp, #0x18]
	bl CastleWriteMsgSimple_Full_ov107_2247744
	strb r0, [r5, #0xa]
_02248E46:
	add r0, r4, #0
	bl GF_BGL_BmpWinOnVReq
	add sp, #0x30
	pop {r3, r4, r5, r6, r7, pc}
	// .align 2, 0
// _02248E50: .4byte 0x000003D6
}
#endif

//--------------------------------------------------------------
/**
 * @brief	LEVEL調整CP確認
 *
 * @param	level_sel	1=+5、2=-5
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleEnemy_SeqSubLevelMsg( CASTLE_ENEMY_WORK* wk, u8 level_sel )
{
	Castle_SetNumber( wk, 0, CastleEnemy_GetLvCP(level_sel), CASTLE_KETA_CP, NUMBER_DISPTYPE_LEFT );
	wk->msg_index = Castle_EasyMsg( wk, msg_castle_trainer_02, FONT_TALK );
	Castle_SetMenu2( wk );										//「はい・いいえ」
	wk->level_sel = level_sel;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	プレイヤー、パートナー名表示
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleEnemy_SeqSubNameWrite( CASTLE_ENEMY_WORK* wk, GF_BGL_BMPWIN* win )
{
	u16 x,y,offset_x,offset_y,pair_offset_x,pair_offset_y;

	//オフセット取得
	Castle_GetOffset( wk, &offset_x, &offset_y, &pair_offset_x, &pair_offset_y );

	if( Castle_CommCheck(wk->type) == FALSE ){
		x = offset_x + INFO_PLAYER_X;
		y = offset_y + INFO_PLAYER_Y;
		PlayerNameWrite( wk, win, x, y, BC_FONT );			//プレイヤー名を表示
		//GF_BGL_BmpWinOnVReq( win );
	}else{

		//親、子の画面とも、親、子の順番に名前が表示されるようにする

		//親なら
		if( CommGetCurrentID() == COMM_PARENT_ID ){
			x = offset_x + INFO_PLAYER_X;
			y = offset_y + INFO_PLAYER_Y;
			PlayerNameWrite( wk, win, x, y, BC_FONT );		//プレイヤー名を表示

			x = pair_offset_x + INFO_PLAYER_X;
			y = pair_offset_y + INFO_PLAYER_Y;
			PairNameWrite( wk, win, x, y, BC_FONT );		//パートナー名を表示

		//子なら
		}else{
			x = offset_x + INFO_PLAYER_X;
			y = offset_y + INFO_PLAYER_Y;
			PairNameWrite( wk, win, x, y, BC_FONT );		//パートナー名を表示

			x = pair_offset_x + INFO_PLAYER_X;
			y = pair_offset_y + INFO_PLAYER_Y;
			PlayerNameWrite( wk, win, x, y, BC_FONT );		//プレイヤー名を表示
		}

		//GF_BGL_BmpWinOnVReq( win );
	}

	GF_BGL_BmpWinOnVReq( win );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	ランクアップ
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 *
 * 親、子ともに画面の敵ポケモンの並びは同じ
 */
//--------------------------------------------------------------
static void CastleEnemy_SeqSubRankUp( CASTLE_ENEMY_WORK* wk, u8 parent_decide_pos, u8 parent_decide_type )
{
	u8	lr,sel_pos,type_offset;
	u16 sub_cp;
	u16 buf16[4];
	u16 offset_x,offset_y,pair_offset_x,pair_offset_y;

	Snd_SePlay( SEQ_SE_DP_PIRORIRO2 );

	//オフセット
	type_offset = wk->modoru_pos;

	//選んだカーソル位置を取得
	sel_pos = GetCommSelCsrPos( type_offset, parent_decide_pos );

	////////////////////////////////////////////////
	//使用CP
	switch( parent_decide_type ){

	case FC_PARAM_TEMOTI:
		sub_cp = CP_USE_TEMOTI;
		break;

	case FC_PARAM_LEVEL:
		sub_cp = CastleEnemy_GetLvCP( wk->parent_decide_updown );			//使用CP
		break;

	case FC_PARAM_TUYOSA:
		sub_cp = CP_USE_TUYOSA;
		break;

	case FC_PARAM_WAZA:
		sub_cp = CP_USE_WAZA;
		break;
	};

	OS_Printf( "parent_decide_type = %d\n", parent_decide_type );
	OS_Printf( "sub_cp = %d\n", sub_cp );
	OS_Printf( "wk->pair_cp = %d\n", wk->pair_cp );

	////////////////////////////////////////////////
	//親
	if( CommGetCurrentID() == COMM_PARENT_ID ){

		//親：親の決定が有効なら
		if( parent_decide_pos < type_offset ){
			Castle_SetPlayerName( wk, 5 );
			Castle_CPRecord_Sub( wk->fro_sv, wk->type, sub_cp );//自分のCP減らす

		//親：子の決定が有効なら
		}else{
			Castle_SetPairName( wk->wordset, 5 );
			wk->pair_cp -= sub_cp;								//相手のCP減らす
		}

	///////////////////////////////////////////////
	//子
	}else{

		//子：親の決定が有効なら
		if( parent_decide_pos < type_offset ){
			Castle_SetPairName( wk->wordset, 5 );
			wk->pair_cp -= sub_cp;								//相手のCP減らす

		//子：子の決定が有効なら
		}else{
			Castle_SetPlayerName( wk, 5 );
			Castle_CPRecord_Sub( wk->fro_sv, wk->type, sub_cp );//自分のCP減らす
		}
	}

	OS_Printf( "wk->pair_cp = %d\n", wk->pair_cp );

	CastleEnemy_SeqSubCPWrite( wk, &wk->bmpwin[ENEMY_BMPWIN_TR1] );	//CP表示を更新
	CastleEnemy_SeqSubMenuWinClear( wk );							//メニューを表示していたら削除

	switch( parent_decide_type ){

	case FC_PARAM_TEMOTI:
		CastleEnemy_SeqSubTemoti( wk, sel_pos );							//てもち
		break;

	case FC_PARAM_LEVEL:
		CastleEnemy_SeqSubLevel( wk, sel_pos, wk->parent_decide_updown );	//レベル
		break;

	case FC_PARAM_TUYOSA:
		CastleEnemy_SeqSubTuyosaMsgSet( wk, sel_pos );						//つよさ見えるMSG
		break;

	case FC_PARAM_WAZA:
		CastleEnemy_SeqSubWazaMsgSet( wk, sel_pos );						//わざ見えるMSG
		break;
	};

	return;
}

//--------------------------------------------------------------
/**
 * @brief	手持ち
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleEnemy_SeqSubTemoti( CASTLE_ENEMY_WORK* wk, u8 csr_pos )
{
	POKEMON_PARAM* poke;

	OS_Printf( "GetCsrPokePos(wk->h_max,csr_pos) = %d\n", GetCsrPokePos(wk->h_max,csr_pos) );

	poke =  PokeParty_GetMemberPointer( wk->p_party, GetCsrPokePos(wk->h_max,csr_pos) );
	CastleTalkWinPut( &wk->bmpwin[ENEMY_BMPWIN_TALK], CONFIG_GetWindowType(wk->config));
	Castle_SetPokeName( wk, 0, PPPPointerGet(poke) );
	wk->msg_index = Castle_EasyMsg( wk, msg_castle_trainer_04_03, FONT_TALK );

	//フラグON
	wk->p_temoti_flag[GetCsrPokePos(wk->h_max,csr_pos)] = 1;

	CastleObj_Vanish( wk->p_ball[GetCsrPokePos(wk->h_max,csr_pos)], CASTLE_VANISH_ON );	//非表示
	CastleObj_Vanish( wk->p_icon[GetCsrPokePos(wk->h_max,csr_pos)], CASTLE_VANISH_OFF );//表示

	//HPとLV表示
	Castle_PokeHpMsgWrite( wk, &wk->bmpwin[ENEMY_BMPWIN_HP] );
	Castle_PokeLvMsgWrite( wk, &wk->bmpwin[ENEMY_BMPWIN_LV] );

	CastleObj_Vanish( wk->p_hp[GetCsrPokePos(wk->h_max,csr_pos)], CASTLE_VANISH_OFF );	//HP状態表示

	//Snd_SePlay( SEQ_SE_DP_DENSI16 );
	Snd_SePlay( SEQ_SE_DP_UG_020 );
	//Snd_SePlay( SEQ_SE_DP_DENSI15 );
	//Snd_SePlay( SEQ_SE_DP_DENSI06 );		//鳴らない
	return;
}

//--------------------------------------------------------------
/**
 * @brief	レベル
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleEnemy_SeqSubLevel( CASTLE_ENEMY_WORK* wk, u8 csr_pos, u8 level_sel )
{
	u32 exp;
	POKEMON_PARAM* temp_poke;

	CastleTalkWinPut( &wk->bmpwin[ENEMY_BMPWIN_TALK], CONFIG_GetWindowType(wk->config));
	temp_poke =  PokeParty_GetMemberPointer( wk->p_party, GetCsrPokePos(wk->h_max,csr_pos) );
	Castle_SetPokeName( wk, 0, PPPPointerGet(temp_poke) );
	if( level_sel == 1 ){
		wk->msg_index = Castle_EasyMsg( wk, msg_castle_trainer_05_06, FONT_TALK );
		//Snd_SePlay( SEQ_SE_DP_DENSI16 );
		//Snd_SePlay( SEQ_SE_DP_ZUKAN02 );
		Snd_SePlay( SEQ_SE_DP_OPEN7 );
	}else{
		wk->msg_index = Castle_EasyMsg( wk, msg_castle_trainer_05_07, FONT_TALK );
		//Snd_SePlay( SEQ_SE_DP_DENSI16 );
		//Snd_SePlay( SEQ_SE_DP_ZUKAN02 );
		Snd_SePlay( SEQ_SE_DP_CLOSE7 );
	}

	//レベルが上がったり、下がったり
	//表示されている情報も更新する
			
	//フラグON
	if( wk->p_level_flag[GetCsrPokePos(wk->h_max,csr_pos)] == 0 ){
		wk->p_level_flag[GetCsrPokePos(wk->h_max,csr_pos)] = level_sel;
	}else{
		wk->p_level_flag[GetCsrPokePos(wk->h_max,csr_pos)] = 0;		//一度上下させたものを元に戻した
	}

	//レベル調整
	if( wk->p_level_flag[GetCsrPokePos(wk->h_max,csr_pos)] == 0 ){
		exp = PokeLevelExpGet( PokeParaGet(temp_poke,ID_PARA_monsno,NULL), 50 );
		PokeParaPut( temp_poke, ID_PARA_exp, &exp );
		PokeParaCalc( temp_poke );
	}else if( wk->p_level_flag[GetCsrPokePos(wk->h_max,csr_pos)] == 1 ){
		exp = PokeLevelExpGet( PokeParaGet(temp_poke,ID_PARA_monsno,NULL), 55 );
		PokeParaPut( temp_poke, ID_PARA_exp, &exp );
		PokeParaCalc( temp_poke );
	}else{
		exp = PokeLevelExpGet( PokeParaGet(temp_poke,ID_PARA_monsno,NULL), 45 );
		PokeParaPut( temp_poke, ID_PARA_exp, &exp );
		PokeParaCalc( temp_poke );
	}

	//HPとLV表示
	Castle_PokeHpMsgWrite( wk, &wk->bmpwin[ENEMY_BMPWIN_HP] );
	Castle_PokeLvMsgWrite( wk, &wk->bmpwin[ENEMY_BMPWIN_LV] );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	ステータスウィンドウを消す
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleEnemy_SeqSubStatusWinDel( CASTLE_ENEMY_WORK* wk )
{
	GF_BGL_BmpWinDataFill( &wk->bmpwin[ENEMY_BMPWIN_STATUS], FBMP_COL_NULL );	//塗りつぶし
	//GF_BGL_BmpWinOnVReq( &wk->bmpwin[ENEMY_BMPWIN_STATUS] );
	//GF_BGL_BmpWinOffVReq( &wk->bmpwin[ENEMY_BMPWIN_STATUS] );
	GF_BGL_BmpWinOff( &wk->bmpwin[ENEMY_BMPWIN_STATUS] );						//★VReqしない
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	リスト終了
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleEnemy_SeqSubListEnd( CASTLE_ENEMY_WORK* wk )
{
	GF_BGL_BMPWIN* p_list_win;

	if( wk->list_flag == 1 ){
		wk->list_flag = 0;
		//wk->list_csr_pos = 0;	//注意！
		p_list_win = (GF_BGL_BMPWIN*)BmpListParamGet( wk->lw, BMPLIST_ID_WIN );

#if 0
		CastleEnemy_SeqSubMenuWinClear( wk );
#else
		//BmpMenuExit( wk->mw, NULL );
		//BmpMenuWinClear( wk->MenuH.win, WINDOW_TRANS_OFF );
		BmpMenuWinClear( p_list_win, WINDOW_TRANS_OFF );
#endif

		GF_BGL_BmpWinDataFill( p_list_win, FBMP_COL_NULL );	//塗りつぶし
		GF_BGL_BmpWinOffVReq( p_list_win );

		//リスト削除
		BMP_MENULIST_Delete( wk->menulist );
		BmpListExit( wk->lw, NULL, NULL );
	}

	return;;
}

//--------------------------------------------------------------
/**
 * @brief	アイコン上下アニメ
 *
 * @param	wk		CASTLE_RANK_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleEnemy_IconSelAnm( CASTLE_ENEMY_WORK* wk )
{
	int	i;
	u8 m_max,anm;
	POKEMON_PARAM* poke;

	m_max = Castle_GetMinePokeNum( wk->type, CASTLE_FLAG_TOTAL );

	for( i=0; i < m_max ;i++ ){
		poke =  PokeParty_GetMemberPointer( wk->p_party, i );

		anm = Frontier_PokeIconAnmNoGet(PokeParaGet(poke,ID_PARA_hp,NULL),
										PokeParaGet(poke,ID_PARA_hpmax,NULL) );

		if( wk->p_icon[i] != NULL ){
			CastleObj_PokeIconAnmChg( wk->p_icon[i], anm );

			//選択しているポケモンではない時か、「もどる」選択中の時
			if((i != GetCsrPokePos(wk->h_max,wk->csr_pos)) || (wk->csr_pos >= wk->modoru_pos) ){
				CastleObj_PokeIconPosSet( wk->p_icon[i], 0 );
			}else{
				CastleObj_PokeIconPosSet( wk->p_icon[i], 1 );
			}
		}
	}
	return;
}


//--------------------------------------------------------------
/**
 * @brief	つよさ、わざ「はい」の共通処理
 *
 * @param	
 *
 * @return	"FALSE = その後break, TRUE = その後return TRUE"
 */
//--------------------------------------------------------------
static BOOL CastleEnemy_SeqSubTuyosaWazaYes( CASTLE_ENEMY_WORK* wk, u16 use_cp, u16 cp_not_msg )
{
	u16 now_cp;

	CastleEnemy_SeqSubMenuWinClear( wk );

	now_cp = FrontierRecord_Get(wk->fro_sv, CastleScr_GetCPRecordID(wk->type),
								Frontier_GetFriendIndex(CastleScr_GetCPRecordID(wk->type)) );

	//CPが足りない時
	if( now_cp < use_cp ){
		CastleEnemy_SeqSubTalkWinOn( wk );
		wk->msg_index = Castle_EasyMsg( wk, cp_not_msg, FONT_TALK );
		wk->sub_seq = SEQ_SUB_KEY_WAIT;
		return FALSE;
	}

	if( Castle_CommCheck(wk->type) == FALSE ){
		BmpTalkWinClearSub( &wk->bmpwin[ENEMY_BMPWIN_TALK] );
		Castle_CPRecord_Sub( wk->fro_sv, wk->type, use_cp );
		CastleEnemy_SeqSubCPWrite( wk, &wk->bmpwin[ENEMY_BMPWIN_TR1] );

		if( use_cp == CP_USE_TUYOSA ){
			CastleEnemy_SeqSubTuyosaMsgSet( wk, wk->csr_pos );		//つよさ見えるMSG
		}else{
			CastleEnemy_SeqSubWazaMsgSet( wk, wk->csr_pos );		//わざ見えるMSG
		}

		wk->sub_seq = SEQ_SUB_DECIDE_EFF_WAIT_NEXT_WIN_OFF;
		return FALSE;

	}else{
		wk->send_req = 1;
		return TRUE;
	}

	return FALSE;
}


//==============================================================================================
//
//	castle_rank.c
//
//==============================================================================================

//通信相手に表示されるメッセージ
static const u16 rankup_pair_msg_tbl[CASTLE_RANK_TYPE_MAX][CASTLE_RANK_MAX] = {
//●回復のランクアップ２
//●回復のランクアップ３
	{	0, 0, 0 },							//未使用

//●道具のランクアップ２
//●道具のランクアップ３
	{	0, 0, 0 },							//未使用

//●情報のランクアップ２
//●情報のランクアップ３
//「わざのじょうほうをかうことができるようになります」
//「わざのじょうほうをかうことができるようになります」
	{ 0, msg_castle_trainer_11_05, msg_castle_trainer_11_05 },
};

//--------------------------------------------------------------
/**
 * @brief	ランクアップ(castle_rank.cから移動)
 *
 * @param	wk		CASTLE_RANK_WORK型のポインタ
 * @param	param	FC_PARAM_INFO_RANKUP
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleRank_SeqSubRankUp(CASTLE_ENEMY_WORK* wk, u8 parent_decide_pos, u8 param)
{
	u8	type_offset,sel_pos,type,m_max;
	u32 now_rank;
	u16 buf16[4];

	m_max = Castle_GetMinePokeNum( wk->type, CASTLE_FLAG_SOLO );

	if( param == FC_PARAM_INFO_RANKUP ){
		type = CASTLE_RANK_TYPE_INFO;
	}else{
		OS_Printf( "CastleRank_SeqSubRankUp 引数paramが不正です！\n" );
		GF_ASSERT( 0 );
	}

	//オフセット
	type_offset = wk->modoru_pos;	

	//選んだカーソル位置を取得
	sel_pos = GetCommSelCsrPos( type_offset, parent_decide_pos );

	////////////////////////////////////////////////
	//親
	if( CommGetCurrentID() == COMM_PARENT_ID ){

		//親：親の決定が有効なら
		if( parent_decide_pos < type_offset ){
			Castle_SetPlayerName( wk, 5 );
			now_rank = Castle_GetRank( wk->sv, wk->type, type );

			Castle_CPRecord_Sub( wk->fro_sv, wk->type, INFO_RANKUP_POINT );

			//ランクアップ
			now_rank = Castle_GetRank( wk->sv, wk->type, type );

			buf16[0] = (now_rank + 1);
			FrontierRecord_Set(	SaveData_GetFrontier(wk->sv), 
				CastleScr_GetRankRecordID(wk->type,type),
				Frontier_GetFriendIndex(CastleScr_GetRankRecordID(wk->type,type)), (now_rank + 1) );
			OS_Printf( "親：親をランクアップ %d\n", (now_rank+1) );

			//ランクアップした時に戻り先が特殊フラグON
			if( Castle_CommCheck(wk->type) == TRUE ){
				wk->recover_flag = RECOVER_RANKUP;
			}

		//親：子の決定が有効なら
		}else{
			Castle_SetPairName( wk->wordset, 5 );
			now_rank= wk->pair_rank[type];
			wk->pair_cp -= INFO_RANKUP_POINT;						//CP減らす
			wk->pair_rank[type]++;
		}

	///////////////////////////////////////////////
	//子
	}else{

		//子：親の決定が有効なら
		if( parent_decide_pos < type_offset ){
			Castle_SetPairName( wk->wordset, 5 );
			now_rank= wk->pair_rank[type];
			wk->pair_cp -= INFO_RANKUP_POINT;						//CP減らす
			wk->pair_rank[type]++;

		//子：子の決定が有効なら
		}else{
			Castle_SetPlayerName( wk, 5 );
			now_rank = Castle_GetRank( wk->sv, wk->type, type );

			Castle_CPRecord_Sub( wk->fro_sv, wk->type, INFO_RANKUP_POINT );

			now_rank = Castle_GetRank( wk->sv, wk->type, type );

			buf16[0] = (now_rank + 1);
			FrontierRecord_Set(	SaveData_GetFrontier(wk->sv), 
				CastleScr_GetRankRecordID(wk->type,type),
				Frontier_GetFriendIndex(CastleScr_GetRankRecordID(wk->type,type)), (now_rank + 1) );
			OS_Printf( "子：子をランクアップ %d\n", (now_rank+1) );

			//ランクアップした時に戻り先が特殊フラグON
			if( Castle_CommCheck(wk->type) == TRUE ){
				wk->recover_flag = RECOVER_RANKUP;
			}
		}
	}

	//メニューを表示していたら削除
	CastleEnemy_SeqSubMenuWinClear( wk );

	//CP表示を更新
	CastleEnemy_SeqSubCPWrite( wk, &wk->bmpwin[ENEMY_BMPWIN_TR1] );

	//CastleRank_SeqSubAllInfoWrite( wk );							//情報更新

	//会話ウィンドウ表示
	CastleTalkWinPut( &wk->bmpwin[ENEMY_BMPWIN_TALK], CONFIG_GetWindowType(wk->config) );

	//「ランクがアップしました！」
	wk->msg_index = Castle_EasyMsg( wk, rankup_pair_msg_tbl[type][now_rank], FONT_TALK );
	//Snd_SePlay( SEQ_SE_DP_PIRORIRO2 );
	//Snd_SePlay( SEQ_SE_DP_DANSA4 );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	ランクを見て旗を表示する
 *
 * @param	wk		CASTLE_ENEMY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleRank_SeqSubHataVanish( CASTLE_ENEMY_WORK* wk )
{
	u8 m_max;
	int i;

	m_max = Castle_GetEnemyPokeNum( wk->type, CASTLE_FLAG_TOTAL );

	for( i=0; i < m_max ;i++ ){
		Castle_HataVanish( wk, i );
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	ランクのレベルから、旗のバニッシュを操作(表示にする)
 *
 * @param	wk			CASTLE_ENEMY_WORK型のポインタ
 * @param	no			どれか
 * @param	level		現在のランク
 * @param	flag		親か子か
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void Castle_HataVanish( CASTLE_ENEMY_WORK* wk, u8 no )
{
	if( wk->p_tuyosa_flag[no] == 1 ){
		CastleObj_Vanish( wk->p_hata[no][0], CASTLE_VANISH_OFF );		//表示
	}

	if( wk->p_waza_flag[no] == 1 ){
		CastleObj_Vanish( wk->p_hata[no][1], CASTLE_VANISH_OFF );		//表示
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	旗OBJオフセット取得
 *
 * @param	
 *
 * @return	"オフセット"
 */
//--------------------------------------------------------------
static void Castle_GetHataXY( CASTLE_ENEMY_WORK* wk, u32* x, u32* y )
{
	if( Castle_CommCheck(wk->type) == TRUE ){
		*x = CASTLE_MULTI_HATA_START_X;
	}else{
		*x = CASTLE_HATA_START_X;
	}

	*y = CASTLE_HATA_START_Y;
	return;
}




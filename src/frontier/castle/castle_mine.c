//==============================================================================================
/**
 * @file	castle_mine.c
 * @brief	「バトルキャッスル 手持ちポケモン画面」メインソース
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
#include "poketool/status_rcv.h"
#include "itemtool/item.h"
#include "savedata/b_tower.h"
#include "savedata/frontier_savedata.h"

#include "communication/comm_system.h"
#include "communication/comm_tool.h"
#include "communication/comm_def.h"
#include "communication/comm_info.h"
#include "communication/wm_icon.h"

#include "castle_sys.h"
#include "castle_common.h"
#include "castle_clact.h"
#include "castle_bmp.h"
#include "castle_obj.h"
#include "application/castle.h"
#include "../frontier_tool.h"						//Frontier_PokeParaMake
#include "../castle_tool.h"							//
#include "../castle_def.h"
#include "../comm_command_frontier.h"

#include "../../field/comm_direct_counter_def.h"	//
#include "../../field/fieldobj.h"
#include "../../field/scr_tool.h"

#include "msgdata/msg.naix"							//NARC_msg_??_dat
#include "msgdata/msg_castle_poke.h"				//msg_??

#include "../graphic/frontier_obj_def.h"
#include "../graphic/frontier_bg_def.h"

#include "castle_item.dat"							//道具、木の実データ
#include "castle_mine_def.h"						//

#include "system/pm_overlay.h"
FS_EXTERN_OVERLAY(frontier_common);


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

//基本リストの選択肢
enum{
	SEL_KAIHUKU = 0,
	SEL_RENTAL,
	SEL_TUYOSA,
	SEL_WAZA,
	SEL_TOZIRU,
	CASTLE_MINE_SEL_MAX,
};

//何を決定したか定義
enum{
	FC_PARAM_KAIHUKU = 0,								//かいふく
	FC_PARAM_LIST_START_KAIHUKU,						//"かいふくリストオフセット"
	FC_PARAM_HP_KAIHUKU = FC_PARAM_LIST_START_KAIHUKU,	//ＨＰかいふく
	FC_PARAM_PP_KAIHUKU,								//ＰＰかいふく
	FC_PARAM_HPPP_KAIHUKU,								//すべてかいふく
	FC_PARAM_KAIHUKU_RANKUP,							//(回復)ランクアップ
	/////////////////////////////
	FC_PARAM_RENTAL,									//レンタル
	FC_PARAM_LIST_START_RENTAL,							//"レンタルリストオフセット"
	FC_PARAM_KINOMI = FC_PARAM_LIST_START_RENTAL,		//きのみ
	FC_PARAM_ITEM,										//どうぐ
	FC_PARAM_RENTAL_RANKUP,								//(レンタル)ランクアップ
	/////////////////////////////
	FC_PARAM_TUYOSA,									//つよさ
	FC_PARAM_WAZA,										//わざ
	FC_PARAM_TOZIRU,									//とじる
};

//ランクアップエフェクト表示位置
#define CASTLE_MINE_RANKUP_X	(204)
#define CASTLE_MINE_RANKUP_Y	(100)
#define CASTLE_MINE_RANKUP_X2	(211)
#define CASTLE_MINE_RANKUP_Y2	(106)


//==============================================================================================
//
//	回復関連の定義
//
//==============================================================================================
//回復使用した時のメッセージテーブル
static const u16 kaihuku_msg[] = {
	msg_castle_poke_04,			//ＨＰかいふく
	msg_castle_poke_05,			//ＰＰかいふく
	msg_castle_poke_06,			//すべてかいふく
};

//回復の使用CP
static const u16 kaihuku_cp_tbl[] = {
	//HPかいふく、PPかいふく、すべてかいふく
	10,8,12
};

//回復を選んだ時に表示するリスト
static const u16 kaihuku_item_tbl[][3] = {
	//ランク、アイテム、リストパラメータ
	{ 1, msg_castle_kaifuku_01_01,	FC_PARAM_HP_KAIHUKU },			//ＨＰかいふく
	{ 2, msg_castle_kaifuku_01_02,	FC_PARAM_PP_KAIHUKU },			//ＰＰかいふく
	{ 3, msg_castle_kaifuku_01_03,	FC_PARAM_HPPP_KAIHUKU },		//すべてかいふく
	//ランクアップ
	//やめる
};

//回復リストメッセージテーブル
static const u32 kaihuku_msg_tbl[][3] = {
	//ランク、msg_id、リストパラメータ
	{ 1, msg_castle_kaifuku_01_01,	FC_PARAM_HP_KAIHUKU },			//ＨＰかいふく
	{ 2, msg_castle_kaifuku_01_02,	FC_PARAM_PP_KAIHUKU },			//ＰＰかいふく
	{ 3, msg_castle_kaifuku_01_03,	FC_PARAM_HPPP_KAIHUKU }, 		//すべてかいふく
	{ 1, msg_castle_kaifuku_02,		FC_PARAM_KAIHUKU_RANKUP }, 		//ランクアップ
	{ 1, msg_castle_kaifuku_03,		BMPLIST_CANCEL },				//やめる
};
#define KAIHUKU_MSG_TBL_MAX		( NELEMS(kaihuku_msg_tbl) )


//==============================================================================================
//
//	ランクアップ関連の定義
//
//==============================================================================================
//ランクアップに必要なポイント
static const u16 rankup_point_tbl[CASTLE_RANK_TYPE_MAX][CASTLE_RANK_MAX] = {
	{ 0, 100, 100 },		//回復
	{ 0, 100, 150 },		//レンタル
	{ 0, 50, 50 },		//情報(castle_enemy.cのINFO_RANKUP_POINTを使用している)
};

//通信相手に表示されるメッセージ
static const u16 rankup_pair_msg_tbl[CASTLE_RANK_TYPE_MAX][CASTLE_RANK_MAX] = {
//●回復のランクアップ２
//「いいキズぐすりで　かいふく　できるように　なりました！」
//●回復のランクアップ３
//「すごいキズぐすりと　ピーピーマックスで　かいふく　できるように　なりました！」
	{	0, msg_castle_poke_27,	msg_castle_poke_28 },

//●道具のランクアップ２
//●道具のランクアップ３
//「どうぐを　レンタル　できるように　なりました！」
//「レンタルできる　きのみと　どうぐの しゅるいが　ふえました！」
	{	0, msg_castle_poke_38,	msg_castle_poke_39 },

//●情報のランクアップ２
//●情報のランクアップ３
	{	0, 0, 0 },							//未使用
};


//==============================================================================================
//
//	構造体宣言
//
//==============================================================================================
struct _CASTLE_MINE_WORK{

	PROC* proc;										//PROCへのポインタ
	FRONTIER_SAVEWORK* fro_sv;						//

	u8	sub_seq;									//シーケンス
	u8	type;										//引数として渡されたバトルタイプ
	u8	msg_index;									//メッセージindex
	u8	wait;

	u8	tmp_csr_pos;								//退避してあるカーソル位置
	u8	csr_pos;									//現在のカーソル位置
	u8	list_flag:1;								//リスト表示中かフラグ
	u8	send_req:1;									//送信リクエストフラグ
	u8	eff_init_flag:1;							//決定エフェクト初期化フラグ
	u8	item_list_flag:1;							//アイテムリスト表示中かフラグ
	u8	menu_flag:1;								//メニュー表示中かフラグ
	u8	rankup_recover_flag:2;						//ランクアップした時に戻り先が特殊フラグ
	u8	dummy25_flag:1;								//ランクアップした時に戻り先が特殊フラグ
	u8	recieve_count;								//受信カウント

	u16 parent_decide_item;							//決定したアイテムナンバー
	u8	parent_decide_pos;							//決定したカーソル位置(どのポケモンか)
	u8	parent_decide_type;							//決定した項目(FC_PARAM_??)

	u8	h_max;
	u8	modoru_pos;
	u16 list_csr_pos;								//リストで選択した位置

	//u32 before_hp;

	u16	basic_list_lp;
	u16	basic_list_cp;

	MSGDATA_MANAGER* msgman_iteminfo;				//メッセージマネージャー
	MSGDATA_MANAGER* msgman;						//メッセージマネージャー
	WORDSET* wordset;								//単語セット
	STRBUF* msg_buf;								//メッセージバッファポインタ
	STRBUF* tmp_buf;								//テンポラリバッファポインタ

	STRBUF* menu_buf[CASTLE_MENU_BUF_MAX];			//メニューバッファポインタ
	STRCODE str[PERSON_NAME_SIZE + EOM_SIZE];		//メニューのメッセージ

	GF_BGL_INI*	bgl;								//BGLへのポインタ
	GF_BGL_BMPWIN bmpwin[CASTLE_MINE_BMPWIN_MAX];	//BMPウィンドウデータ

	//BMPメニュー(bmp_menu.h)
	BMPMENU_HEADER MenuH;							//BMPメニューヘッダー
	BMPMENU_WORK* mw;								//BMPメニューワーク
	BMPMENU_DATA Data[CASTLE_MENU_BUF_MAX];			//BMPメニューデータ

	//BMPリスト
	BMPLIST_WORK* lw;								//BMPリストデータ
	BMPLIST_DATA* menulist;							//
	BMPLIST_HEADER list_h;

	PALETTE_FADE_PTR pfd;							//パレットフェード

	NUMFONT* num_font;								//8x8フォント

	//const CONFIG* config;							//コンフィグポインタ
	CONFIG* config;									//コンフィグポインタ
	SAVEDATA* sv;									//セーブデータポインタ
	CASTLEDATA* castle_sv;							//キャッスルセーブデータポインタ
	CASTLESCORE* score_sv;							//キャッスルセーブデータポインタ

	CASTLE_CLACT castle_clact;						//セルアクタデータ
	CASTLE_OBJ* p_scr_u;							//スクロールカーソルOBJのポインタ格納テーブル
	CASTLE_OBJ* p_scr_d;							//スクロールカーソルOBJのポインタ格納テーブル
	CASTLE_OBJ* p_itemicon;							//アイテムアイコンOBJのポインタ格納テーブル
	CASTLE_OBJ* p_icon[CASTLE_COMM_POKE_TOTAL_NUM];	//アイコンOBJのポインタ格納テーブル
	CASTLE_OBJ* p_itemkeep[CASTLE_COMM_POKE_TOTAL_NUM];	//アイテム持っているアイコンOBJのポインタ
	CASTLE_OBJ* p_eff;								//エフェクトOBJのポインタ格納テーブル
	CASTLE_OBJ* p_poke_sel;							//ポケモン選択ウィンOBJのポインタ
	CASTLE_OBJ* p_pair_poke_sel;					//ペアのポケモン選択ウィンOBJのポインタ
	CASTLE_OBJ* p_hp[CASTLE_COMM_POKE_TOTAL_NUM];	//HP状態OBJのポインタ
	CASTLE_OBJ* p_eff_rankup;						//ランクアップOBJのポインタ格納テーブル
	CASTLE_OBJ* p_item_csr;							//アイテムカーソルOBJのポインタ格納テーブル

	u16* p_ret_work;								//CASTLE_CALL_WORKの戻り値ワークへのポインタ
	POKEPARTY* p_party;

	ARCHANDLE* hdl;

	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//CASTLE_COMM castle_comm;
#if 1
	//通信用：データバッファ
	u16	send_buf[CASTLE_COMM_BUF_LEN];

	//通信用
	u8	pair_csr_pos;								//パートナーのカーソル位置
	u8	pair_sel_pos;								//パートナーの選択した位置
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
PROC_RESULT CastleMineProc_Init( PROC * proc, int * seq );
PROC_RESULT CastleMineProc_Main( PROC * proc, int * seq );
PROC_RESULT CastleMineProc_End( PROC * proc, int * seq );

//シーケンス
static BOOL Seq_GameInit( CASTLE_MINE_WORK* wk );
static void TypeSelInit( CASTLE_MINE_WORK* wk );
static BOOL Seq_GameTypeSel( CASTLE_MINE_WORK* wk );
static BOOL Seq_GameSendRecv( CASTLE_MINE_WORK* wk );
static BOOL Seq_GameEndMulti( CASTLE_MINE_WORK* wk );
static BOOL Seq_GameEnd( CASTLE_MINE_WORK* wk );

//共通処理
static void CastleCommon_Delete( CASTLE_MINE_WORK* wk );
static void Castle_InitSub1( void );
static void Castle_InitSub2( CASTLE_MINE_WORK* wk );

//共通初期化、終了
static void Castle_ObjInit( CASTLE_MINE_WORK* wk );
static void Castle_BgInit( CASTLE_MINE_WORK* wk );
static void Castle_BgExit( GF_BGL_INI * ini );

//設定
static void VBlankFunc( void * work );
static void SetVramBank(void);
static void SetBgHeader( GF_BGL_INI * ini );

//BGグラフィックデータ
static void Castle_SetMainBgGraphic( CASTLE_MINE_WORK * wk, u32 frm  );
static void Castle_SetMainBgPalette( void );
static void Castle_SetStatusBgGraphic( CASTLE_MINE_WORK * wk, u32 frm  );
static void Castle_SetWazaBgGraphic( CASTLE_MINE_WORK * wk, u32 frm  );
static void Castle_SetItemBgGraphic( CASTLE_MINE_WORK * wk, u32 frm  );
static void Castle_SetStatusBgPalette( void );
static void Castle_SetSubBgGraphic( CASTLE_MINE_WORK * wk, u32 frm  );

//メッセージ
static u8 CastleWriteMsg( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font );
// TODO__fix_me prototype of below function probably wrong
static u8 CastleWriteMsg_Full_ov107_22437CC( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font );
static u8 CastleWriteMsgSimple( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font );
static u8 CastleWriteMsgSimple_Full_ov107_2243890( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font, u32 a10_unk_mode );
static u8 Castle_EasyMsg( CASTLE_MINE_WORK* wk, int msg_id, u8 font );
static void Castle_StatusMsgWrite( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, POKEMON_PARAM* poke );
static void StMsgWriteSub( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, int msg, u16 x, u16 y );
static void Castle_WazaMsgWrite( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, POKEMON_PARAM* poke );
static void WazaMsgWriteSub( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, u8 no, u32 msg_id, u32 msg_id2, POKEMON_PARAM* poke, u32 id, u32 id2, u32 id3 );
static void Castle_PokeHpMsgWrite( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win );
static void Castle_PokeHpMsgWriteSub( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, u8 no, u8 flag );
static void Castle_PokeLvMsgWrite( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win );
static void Castle_PokeLvMsgWriteSub( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, u8 no, u8 flag );
static void Castle_NameCPWinWrite( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win );
static void Castle_NameCPWrite( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win );
static void Castle_PokeHpEff( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, u8 no, u32 hp );
static void CastleMine_Default_Write( CASTLE_MINE_WORK* wk );
static void CastleMine_Default_Del( CASTLE_MINE_WORK* wk );
static void CastleMine_Basic_Write( CASTLE_MINE_WORK* wk );
static void CastleMine_Basic_Del( CASTLE_MINE_WORK* wk );
static void CastleMine_Kaihuku_Write( CASTLE_MINE_WORK* wk );
static void CastleMine_Kaihuku_Del( CASTLE_MINE_WORK* wk );
static void CastleMine_Rental_Write( CASTLE_MINE_WORK* wk );
static void CastleMine_Rental_Del( CASTLE_MINE_WORK* wk );
static void CastleMine_ItemSeed_Write( CASTLE_MINE_WORK* wk, u8 type );
static void CastleMine_ItemSeed_Del( CASTLE_MINE_WORK* wk );

//メニュー
static void CastleInitMenu( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, u8 y_max );
static void CastleSetMenuData( CASTLE_MINE_WORK* wk, u8 no, u8 param, int msg_id );
static void Castle_SetMenu2( CASTLE_MINE_WORK* wk );

//リスト
static void Castle_ItemListMake( CASTLE_MINE_WORK* wk, u8 decide_type );
static void Castle_CsrMoveCallBack( BMPLIST_WORK* work, u32 param, u8 mode );
static void Castle_LineWriteCallBack( BMPLIST_WORK* work, u32 param, u8 y );
static void Castle_KaihukuListMake( CASTLE_MINE_WORK* wk );
static void Castle_KaihukuCsrMoveCallBack( BMPLIST_WORK* work, u32 param, u8 mode );
static void Castle_KaihukuLineWriteCallBack( BMPLIST_WORK* work, u32 param, u8 y );
static void Castle_RentalListMake( CASTLE_MINE_WORK* wk );
static void Castle_RentalCsrMoveCallBack( BMPLIST_WORK* work, u32 param, u8 mode );
static void Castle_RentalLineWriteCallBack( BMPLIST_WORK* work, u32 param, u8 y );
static void Castle_BasicListMake( CASTLE_MINE_WORK* wk );
static void Castle_BasicCsrMoveCallBack( BMPLIST_WORK* work, u32 param, u8 mode );

//文字列
static void Castle_SetNumber( CASTLE_MINE_WORK* wk, u32 bufID, s32 number, u32 keta, NUMBER_DISPTYPE disp );
static void Castle_SetPokeName( CASTLE_MINE_WORK* wk, u32 bufID, POKEMON_PASO_PARAM* ppp );
static void Castle_SetPlayerName( CASTLE_MINE_WORK* wk, u32 bufID );
static void PlayerNameWrite( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 font );
static void PairNameWrite( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 font );
static void PokeNameWrite( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 f_col, u8 s_col, u8 b_col, u8 font );
static void PokeNameWriteEx( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 f_col, u8 s_col, u8 b_col, u8 font, u16 monsno, u8 sex );
static void PokeSexWrite( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 font, u8 sex );
static void TalkWinWrite( CASTLE_MINE_WORK* wk );
static u8 CastleItemInfoWriteMsg( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, u16 item );

//ツール
static BOOL Castle_CheckType( CASTLE_MINE_WORK* wk, u8 type );
static void NextSeq( CASTLE_MINE_WORK* wk, int* seq, int next );
static int KeyCheck( int key );
static void BgCheck( CASTLE_MINE_WORK* wk );
static void CsrMove( CASTLE_MINE_WORK* wk, int key );
static void CsrMoveSub( CASTLE_MINE_WORK* wk );
static void PokeSelMoveSub( CASTLE_MINE_WORK* wk, u8 csr_pos, u8 flag );
static void CastleMine_GetPokeSelXY( CASTLE_MINE_WORK* wk, u32* x, u32* y, u8 csr_pos );
static u16 GetCsrX( CASTLE_MINE_WORK* wk, u8 csr_pos, u8 lr );
static u16 GetCsrY( CASTLE_MINE_WORK* wk, u8 csr_pos );
static u16 GetItemCP( CASTLE_MINE_WORK* wk, u16 csr_pos, u8 type );
static u16 GetItemCPByItemNo( u16 item );
static u16 GetItemNo( CASTLE_MINE_WORK*wk, u16 csr_pos, u8 type );
static void Castle_GetOffset( CASTLE_MINE_WORK* wk, u16* offset_x, u16* offset_y, u16* pair_offset_x, u16* pair_offset_y );
static BOOL CastleMine_DecideEff( CASTLE_MINE_WORK* wk, u8 csr_pos, u8 sel_type );
static void CastleMine_ItemWinPokeIconMove( CASTLE_MINE_WORK* wk, u8 csr_pos, u8 flag );
static u32 CastleMine_GetPokeAnmNo( CASTLE_MINE_WORK* wk, u8 gauge_color );
static u32 CastleMine_GetHpAnmNo( CASTLE_MINE_WORK* wk, u8 gauge_color );
static void CastleMine_StatusWinChg( CASTLE_MINE_WORK* wk, s8 add_sub );
static void CastleMine_WazaWinChg( CASTLE_MINE_WORK* wk, s8 add_sub );
static void CastleMine_PairDecideDel( CASTLE_MINE_WORK* wk );
static BOOL CastleMine_PPRecoverCheck( POKEMON_PARAM* poke );
static void BmpTalkWinClearSub( GF_BGL_BMPWIN* win );
static void BmpTalkWinPutSub( CASTLE_MINE_WORK* wk );

//通信
BOOL CastleMine_CommSetSendBuf( CASTLE_MINE_WORK* wk, u16 type, u16 param );
void CastleMine_CommSendBufBasicData( CASTLE_MINE_WORK* wk, u16 type );
void CastleMine_CommRecvBufBasicData(int id_no,int size,void *pData,void *work);
void CastleMine_CommSendBufRankUpType( CASTLE_MINE_WORK* wk, u16 type, u16 param );
void CastleMine_CommRecvBufReqType(int id_no,int size,void *pData,void *work);
void CastleMine_CommSendBufCsrPos( CASTLE_MINE_WORK* wk, u16 type );
void CastleMine_CommRecvBufCsrPos(int id_no,int size,void *pData,void *work);
void CastleMine_CommSendBufModoru( CASTLE_MINE_WORK* wk );
void CastleMine_CommRecvBufModoru(int id_no,int size,void *pData,void *work);

//回復処理
static void CastleMine_ItemUse( POKEMON_PARAM* pp, u16 item );

//サブシーケンス
static void CastleMine_SeqSubNameWrite( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win );
static void CastleMine_SeqSubKaihuku( CASTLE_MINE_WORK* wk, u8 csr_pos, u8 type );
static void CastleMine_SeqSubItem( CASTLE_MINE_WORK* wk, u8 csr_pos, u16 item );
static void CastleMine_SeqSubMenuWinClear( CASTLE_MINE_WORK* wk );
static void CastleMine_SeqSubListStart( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win );
static void CastleMine_SeqSubItemListEnd( CASTLE_MINE_WORK* wk );
static void CastleMine_SeqSubItemListReturn( CASTLE_MINE_WORK* wk );
static void CastleMine_SeqSubListEnd( CASTLE_MINE_WORK* wk );
static void CastleMine_SeqSubCPWrite( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win );
static void CastleMine_SeqSubRankUp( CASTLE_MINE_WORK* wk, u8 parent_decide_pos, u8 parent_decide_type );
static void CastleMine_SeqSubStatusWin( CASTLE_MINE_WORK* wk, u8 csr_pos );
static void CastleMine_SeqSubWazaWin( CASTLE_MINE_WORK* wk, u8 csr_pos );
static void CastleMine_SeqSubStatusWazaDel( CASTLE_MINE_WORK* wk );
static void CastleMine_IconSelAnm( CASTLE_MINE_WORK* wk );

//デバック
static void Debug_HpDown( CASTLE_MINE_WORK* wk, u8 no );

//ランクアップ処理(castle_rank.cより移動)
static void CastleRank_SeqSubRankUp( CASTLE_MINE_WORK* wk, u8 parent_decide_pos, u8 param );


//==============================================================================================
//
//	リストデータ
//
//==============================================================================================
static const BMPLIST_HEADER CastleListH = {
	NULL,						//表示文字データポインタ

	Castle_CsrMoveCallBack,		//カーソル移動ごとのコールバック関数
	Castle_LineWriteCallBack,	//一列表示ごとのコールバック関数

	NULL,						//GF_BGL_BMPWINのポインタ

	(CASTLE_ITEM_ALL_MAX+1),	//リスト項目数
	7,							//表示最大項目数

	0,							//ラベル表示Ｘ座標
	8,							//項目表示Ｘ座標
	0,							//カーソル表示Ｘ座標
	0,							//表示Ｙ座標
	FBMP_COL_BLACK,				//文字色
	//FBMP_COL_WHITE,			//背景色
	FBMP_COL_NULL,				//背景色
	FBMP_COL_BLK_SDW,			//文字影色
	0,							//文字間隔Ｘ
	16,							//文字間隔Ｙ
	BMPLIST_NO_SKIP,			//ページスキップタイプ
	FONT_SYSTEM,				//文字指定(本来は u8 だけど、そんなに作らないと思うので)
	0,							//ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)

	NULL,
};


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
PROC_RESULT CastleMineProc_Init( PROC * proc, int * seq )
{
	int i;
	CASTLE_MINE_WORK* wk;
	CASTLE_CALL_WORK* castle_call;

	Overlay_Load( FS_OVERLAY_ID(frontier_common), OVERLAY_LOAD_NOT_SYNCHRONIZE );

	Castle_InitSub1();

	//sys_CreateHeap( HEAPID_BASE_APP, HEAPID_CASTLE, 0x20000 );
	sys_CreateHeap( HEAPID_BASE_APP, HEAPID_CASTLE, 0x25000 );

	wk = PROC_AllocWork( proc, sizeof(CASTLE_MINE_WORK), HEAPID_CASTLE );
	memset( wk, 0, sizeof(CASTLE_MINE_WORK) );

	wk->bgl				= GF_BGL_BglIniAlloc( HEAPID_CASTLE );
	wk->proc			= proc;
	castle_call			= (CASTLE_CALL_WORK*)PROC_GetParentWork( proc );
	wk->sv				= castle_call->sv;
	wk->castle_sv		= SaveData_GetCastleData( wk->sv );
	wk->score_sv		= SaveData_GetCastleScore( wk->sv );
	wk->type			= castle_call->type;
	wk->p_ret_work		= &castle_call->ret_work;
	wk->config			= SaveData_GetConfig( wk->sv );			//コンフィグポインタを取得
	wk->p_party			= castle_call->p_m_party;
	wk->parent_decide_pos = CASTLE_MINE_DECIDE_NONE;
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

	Castle_InitSub2( wk );

	if( Castle_CommCheck(wk->type) == TRUE ){
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
PROC_RESULT CastleMineProc_Main( PROC * proc, int * seq )
{
	CASTLE_MINE_WORK* wk  = PROC_GetWork( proc );

	//OS_Printf( " *seq = %d\n", *seq );

	//相手が「戻る」を選んだ時
	if( wk->pair_modoru_flag == 1 ){

		switch( *seq ){

		//通信終了へ
		case SEQ_GAME_TYPE_SEL:
		//case SEQ_GAME_SEND_RECV:					//08.06.17 送受信中は強制終了しない
			wk->pair_modoru_flag = 0;
			CastleMine_PairDecideDel( wk );

			//エフェクト中も終了するようにした
			if( wk->p_eff != NULL ){
				CastleObj_Delete( wk->p_eff );
				wk->eff_init_flag = 0;
			}

			//「○さんが戻るを選択しました！」
			CastleTalkWinPut( &wk->bmpwin[MINE_BMPWIN_TALK], CONFIG_GetWindowType(wk->config));
			Frontier_PairNameWordSet( wk->wordset, 0 );

			wk->msg_index = Castle_EasyMsg( wk, msg_castle_poke_01_01, FONT_TALK );

			NextSeq( wk, seq, SEQ_GAME_END_MULTI );
			break;
		};

	//相手が何か決定した時
	}else if( wk->parent_decide_pos != CASTLE_MINE_DECIDE_NONE ){
		switch( *seq ){

		//送受信へ強制移動
		case SEQ_GAME_TYPE_SEL:
		case SEQ_GAME_END_MULTI:					//08.06.17 先に「戻る」中の時は、
			wk->pair_modoru_flag = 0;				//08.06.17 「送受信」に戻す
			CastleMine_PairDecideDel( wk );
			NextSeq( wk, seq, SEQ_GAME_SEND_RECV );
			break;

		//何もしない
		//case SEQ_GAME_INIT:
		//case SEQ_GAME_SEND_RECV:
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

		//BgCheck( wk );
#if 0
		if( sys.cont & PAD_BUTTON_L ){
			//CastleObj_SetObjPos(wk->p_icon[0], 
			//					CASTLE_ITEM_POKE_ICON_X, CASTLE_ITEM_POKE_ICON_Y);
			Debug_HpDown( wk, 0 );
			Debug_HpDown( wk, 1 );
			Debug_HpDown( wk, 2 );
			//Debug_HpDown( wk, 3 );
			Castle_PokeHpMsgWrite( wk, &wk->bmpwin[MINE_BMPWIN_HP] );
			break;
		}
#endif

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

	CastleMine_IconSelAnm( wk );					//アイコン上下アニメ
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
PROC_RESULT CastleMineProc_End( PROC * proc, int * seq )
{
	int i;
	CASTLE_MINE_WORK* wk = PROC_GetWork( proc );

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
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameInit( CASTLE_MINE_WORK* wk )
{
	switch( wk->sub_seq ){

	//コマンド設定後の同期
	case 0:
		if( Castle_CommCheck(wk->type) == TRUE ){
			CommToolTempDataReset();
			CommTimingSyncStart( DBC_TIM_CASTLE_MINE_INIT );
		}
		wk->sub_seq++;
		break;

	//タイミングコマンドが届いたか確認
	case 1:
		if( Castle_CommCheck(wk->type) == TRUE ){
			if( CommIsTimingSync(DBC_TIM_CASTLE_MINE_INIT) == TRUE ){
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
			if( CastleMine_CommSetSendBuf(wk,CASTLE_COMM_MINE_PAIR,0) == TRUE ){
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
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static void TypeSelInit( CASTLE_MINE_WORK* wk )
{
	u16 x,y,offset_x,offset_y,pair_offset_x,pair_offset_y,icon_offset_x;
	GF_BGL_BMPWIN* win;

	//通信しているかで表示のオフセットが変わる
	Castle_GetOffset( wk, &offset_x, &offset_y, &pair_offset_x, &pair_offset_y );

	///////////////////////////////////////////////////////////////////////////////
	win = &wk->bmpwin[MINE_BMPWIN_TR1];
	GF_BGL_BmpWinDataFill( win, FBMP_COL_NULL );					//塗りつぶし
	CastleMine_SeqSubNameWrite( wk, win );							//プレイヤー、パートナー名表示
	CastleMine_SeqSubCPWrite( wk, win );							//CP表示

	///////////////////////////////////////////////////////////////////////////////
	//HPとLV表示
	Castle_PokeHpMsgWrite( wk, &wk->bmpwin[MINE_BMPWIN_HP] );
	Castle_PokeLvMsgWrite( wk, &wk->bmpwin[MINE_BMPWIN_LV] );

	///////////////////////////////////////////////////////////////////////////////
	CastleMine_Default_Write( wk );									//TALKMENU,もどる

	GF_Disp_DispOn();
	return;
}

//--------------------------------------------------------------
/**
 * @brief	ゲームタイプ選択中
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
enum{
	SEQ_SUB_START = 0,
	SEQ_SUB_MENU_MAIN,
	/////////////////////////////////////////////////////////////////////
	SEQ_SUB_KAIHUKU_ITEM_LIST,					//回復するアイテムを選ぶ
	SEQ_SUB_KAIHUKU_YES_NO,						//それでよい「はい・いいえ」
	SEQ_SUB_KAIHUKU_RANKUP_YES_NO,				//回復ランクアップ
	SEQ_SUB_KAIHUKU_RANKUP_DECIDE_EFF_WAIT,		//回復ランクアップ決定エフェクト
	SEQ_SUB_KAIHUKU_RANKUP_DECIDE_EFF_KEY_WAIT,	//回復ランクアップ決定エフェクトキー待ち
	SEQ_SUB_KAIHUKU_KEY_WAIT_LIST_RETURN,		//キー待ちして、リストへ戻る
	/////////////////////////////////////////////////////////////////////
	SEQ_SUB_RENTAL_LIST,						//レンタルすることを選ぶ
	SEQ_SUB_RENTAL_ITEM_SEED_LIST,				//きのみ、アイテムのリスト
	SEQ_SUB_RENTAL_YES_NO,						//それでよい「はい・いいえ」
	SEQ_SUB_RENTAL_RANKUP_YES_NO,				//レンタルランクアップ
	SEQ_SUB_RENTAL_RANKUP_DECIDE_EFF_WAIT,		//レンタルランクアップ決定エフェクト
	SEQ_SUB_RENTAL_RANKUP_DECIDE_EFF_KEY_WAIT,	//レンタルランクアップ決定エフェクトキー待ち
	SEQ_SUB_RENTAL_KEY_WAIT_LIST_RETURN,		//キー待ちして、レンタルリストへ戻る
	SEQ_SUB_RENTAL_KEY_WAIT_ITEM_SEED_RETURN,	//キー待ちして、きのみ、道具リストへ戻る
	/////////////////////////////////////////////////////////////////////
	SEQ_SUB_RENTAL_ITEM_TRADE,					//すでにアイテムもってる
	SEQ_SUB_RENTAL_ITEM_TRADE_YES_NO,			//それでよい「はい・いいえ」
	/////////////////////////////////////////////////////////////////////
	SEQ_SUB_DECIDE_EFF_WAIT,					//決定エフェクト
	SEQ_SUB_DECIDE_KEY_WAIT,					//決定エフェクト後のキー待ち
	/////////////////////////////////////////////////////////////////////
	SEQ_SUB_INFO_TUYOSA_KEY_WAIT_WIN_OFF,		//キー待ちしてウィンドウオフしてBASICリストへ
	SEQ_SUB_INFO_WAZA_KEY_WAIT_WIN_OFF,			//キー待ちしてウィンドウオフしてBASICリストへ
};

static BOOL Seq_GameTypeSel( CASTLE_MINE_WORK* wk )
{
	u8 sel_pos,sel_type;
	u8 now_rank;
	u16 now_cp;
	int i;
	u32 color,ret;
	POKEMON_PARAM* poke;

	switch( wk->sub_seq ){

	//////////////////////////////////////////////////////////////////////////////////////
	//タイプを選択中
	case SEQ_SUB_START:

		//ランクアップした時に戻り先が特殊かチェック
		if( wk->rankup_recover_flag == 1 ){
			//BmpTalkWinClear( &wk->bmpwin[MINE_BMPWIN_TALK], WINDOW_TRANS_OFF );
			CastleMine_Kaihuku_Write( wk );

			//ランクアップエフェクト
			CastleObj_RankUpEff( wk->p_eff_rankup, CASTLE_MINE_RANKUP_X, CASTLE_MINE_RANKUP_Y );

			wk->sub_seq = SEQ_SUB_KAIHUKU_ITEM_LIST;
			wk->rankup_recover_flag = 0;	//ランクアップした時に戻り先が特殊フラグOFF
			return FALSE;
		}else if( wk->rankup_recover_flag == 2 ){
			//BmpTalkWinClear( &wk->bmpwin[MINE_BMPWIN_TALK], WINDOW_TRANS_OFF );
			CastleMine_Rental_Write( wk );								//TALK,RENTAL

			//ランクアップエフェクト
			CastleObj_RankUpEff( wk->p_eff_rankup, CASTLE_MINE_RANKUP_X2, CASTLE_MINE_RANKUP_Y2 );

			wk->sub_seq = SEQ_SUB_RENTAL_LIST;
			wk->rankup_recover_flag = 0;	//ランクアップした時に戻り先が特殊フラグOFF
			return FALSE;
		}

		CsrMove( wk, sys.trg );

		//決定ボタンを押した時
		if( sys.trg & PAD_BUTTON_A ){
			Snd_SePlay( SEQ_SE_DP_SELECT );

			//「もどる」
			if( wk->csr_pos >= wk->modoru_pos ){
				return TRUE;
			}else{
				CastleMine_Default_Del( wk );		//TALKMENU,もどる
				CastleMine_Basic_Write( wk );		//TALKMENU2,BASIC
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

		switch( ret ){

		case BMPLIST_NULL:
			break;

		case BMPLIST_CANCEL:
			CastleMine_Basic_Del( wk );			//TALKMENU2,BASIC
			CastleMine_Default_Write( wk );		//TALKMENU,もどる
			wk->sub_seq = SEQ_SUB_START;
			break;

		//回復
		case FC_PARAM_KAIHUKU:
			CastleMine_Basic_Del( wk );		//TALKMENU2,BASIC
			CastleMine_Kaihuku_Write( wk );
			wk->sub_seq = SEQ_SUB_KAIHUKU_ITEM_LIST;
			break;

		//レンタル
		case FC_PARAM_RENTAL:
			CastleMine_Basic_Del( wk );		//TALKMENU2,BASIC
			CastleMine_Rental_Write( wk );	//TALK,RENTAL
			wk->sub_seq = SEQ_SUB_RENTAL_LIST;
			break;

		//持っている道具、性格、特性、各能力値を表示
		case FC_PARAM_TUYOSA:
			CastleMine_Basic_Del( wk );		//TALKMENU2,BASIC
			CastleMine_SeqSubStatusWin( wk, wk->csr_pos );
			wk->sub_seq = SEQ_SUB_INFO_TUYOSA_KEY_WAIT_WIN_OFF;
			break;

		//技名、ＰＰ名を表示
		case FC_PARAM_WAZA:
			CastleMine_Basic_Del( wk );		//TALKMENU2,BASIC
			CastleMine_SeqSubWazaWin( wk, wk->csr_pos );
			wk->sub_seq = SEQ_SUB_INFO_WAZA_KEY_WAIT_WIN_OFF;
			break;

		//とじる
		case FC_PARAM_TOZIRU:
			CastleMine_Basic_Del( wk );		//TALKMENU2,BASIC
			CastleMine_Default_Write( wk );	//TALKMENU,もどる
			wk->sub_seq = SEQ_SUB_START;
			break;
		};
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//「かいふく」使うアイテムリスト処理
	case SEQ_SUB_KAIHUKU_ITEM_LIST:

#if 1
		//リスト処理を強引にループさせる
		BmpListDirectPosGet( wk->lw, &wk->list_csr_pos );					//カーソル位置

		if( sys.trg & PAD_KEY_UP ){
			if( wk->list_csr_pos == 0 ){
				BmpListMainOutControlEx( wk->lw, (BMPLIST_HEADER*)&wk->list_h, 
					0, (KAIHUKU_MSG_TBL_MAX-1), 1, PAD_KEY_DOWN, NULL, NULL );
				BmpListRewrite( wk->lw );
				Snd_SePlay( SEQ_SE_DP_SELECT );

				CastleWriteMsg( wk, &wk->bmpwin[MINE_BMPWIN_TALK], 
								msg_castle_poke_20, 1, 1, MSG_NO_PUT, 
								FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_WHITE, FONT_TALK );
				return FALSE;
			}
		}else if( sys.trg & PAD_KEY_DOWN ){
			if( wk->list_csr_pos == (KAIHUKU_MSG_TBL_MAX-1) ){
				BmpListMainOutControlEx( wk->lw, (BMPLIST_HEADER*)&wk->list_h, 
					0, 0, 1, PAD_KEY_UP, NULL, NULL );
				BmpListRewrite( wk->lw );
				Snd_SePlay( SEQ_SE_DP_SELECT );

				CastleWriteMsg( wk, &wk->bmpwin[MINE_BMPWIN_TALK], 
								msg_castle_poke_13, 1, 1, MSG_NO_PUT, 
								FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_WHITE, FONT_TALK );
				return FALSE;
			}
		}
#endif

		ret = BmpListMain( wk->lw );
		Castle_ListSeCall( ret, SEQ_SE_DP_SELECT );
		BmpListDirectPosGet( wk->lw, &wk->list_csr_pos );					//カーソル位置

		switch( ret ){

		case BMPLIST_NULL:
			break;

		case BMPLIST_CANCEL:
			BmpTalkWinClearSub( &wk->bmpwin[MINE_BMPWIN_TALK] );
			CastleMine_Kaihuku_Del( wk );
			CastleMine_Basic_Write( wk );		//TALKMENU2,BASIC
			wk->sub_seq = SEQ_SUB_MENU_MAIN;
			break;

		case FC_PARAM_HP_KAIHUKU:
		case FC_PARAM_PP_KAIHUKU:
		case FC_PARAM_HPPP_KAIHUKU:
			wk->parent_decide_type = ret;									//パラメータ
			CastleMine_Kaihuku_Del( wk );

			//会話ウィンドウ表示
			CastleTalkWinPut( &wk->bmpwin[MINE_BMPWIN_TALK], CONFIG_GetWindowType(wk->config));

			//ランクが足りない時
			now_rank = Castle_GetRank( wk->sv, wk->type, CASTLE_RANK_TYPE_KAIHUKU );
			if( now_rank < kaihuku_msg_tbl[wk->list_csr_pos][0] ){
				wk->msg_index = Castle_EasyMsg( wk, msg_castle_poke_14, FONT_TALK );
				wk->sub_seq = SEQ_SUB_KAIHUKU_KEY_WAIT_LIST_RETURN;
			}else{
				//「○ＣＰ　かかりますが　よろしい　ですか？」
				Castle_SetNumber(	wk, 0, kaihuku_cp_tbl[wk->list_csr_pos], 
									CASTLE_KETA_STATUS, NUMBER_DISPTYPE_LEFT );
				wk->msg_index = Castle_EasyMsg( wk, msg_castle_poke_02, FONT_TALK );
				Castle_SetMenu2( wk );						//「はい・いいえ」
				wk->sub_seq = SEQ_SUB_KAIHUKU_YES_NO;
			}
			break;

		case FC_PARAM_KAIHUKU_RANKUP:
			now_rank = Castle_GetRank( wk->sv, wk->type, CASTLE_RANK_TYPE_KAIHUKU );
			
			//ランクアップＭＡＸを選択
			if( now_rank == CASTLE_RANK_MAX ){
				//「これ　いじょう　ランクは　あがりません！」
				//wk->msg_index = Castle_EasyMsg( wk, msg_castle_poke_23, FONT_TALK );
				//wk->sub_seq = SEQ_SUB_KAIHUKU_KEY_WAIT_LIST_RETURN;
				Snd_SeStopBySeqNo( SEQ_SE_DP_SELECT, 0 );
				Snd_SePlay( SEQ_SE_DP_BOX03 );
			}else{
				wk->parent_decide_type = ret;									//パラメータ
				CastleMine_Kaihuku_Del( wk );

				//必要なCPポイント取得
				now_cp = FrontierRecord_Get(wk->fro_sv, CastleScr_GetCPRecordID(wk->type),
									Frontier_GetFriendIndex(CastleScr_GetCPRecordID(wk->type)));

				/*「○ＣＰを使ってランクをあげますか？」*/
				Castle_SetNumber(wk, 0, rankup_point_tbl[CASTLE_RANK_TYPE_KAIHUKU][now_rank], 
								 CASTLE_KETA_CP, NUMBER_DISPTYPE_LEFT );
				wk->msg_index = Castle_EasyMsg( wk, msg_castle_poke_24, FONT_TALK );
				Castle_SetMenu2( wk );						//「はい・いいえ」
				wk->sub_seq = SEQ_SUB_KAIHUKU_RANKUP_YES_NO;
			}
			break;

		};
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//「回復」そのアイテムでよいか「はい・いいえ」メニュー処理
	case SEQ_SUB_KAIHUKU_YES_NO:
		ret = BmpMenuMain( wk->mw );

		switch( ret ){

		case BMPMENU_NULL:
			break;

		//「はい」
		case 0:
			poke =  PokeParty_GetMemberPointer( wk->p_party, GetCsrPokePos(wk->h_max,wk->csr_pos) );
			CastleMine_SeqSubMenuWinClear( wk );					//メニューを表示していたら削除

			now_cp = FrontierRecord_Get(wk->fro_sv, CastleScr_GetCPRecordID(wk->type),
										//FRONTIER_RECORD_NOT_FRIEND );		//現在のCP
										Frontier_GetFriendIndex(CastleScr_GetCPRecordID(wk->type)));

			//ランクが足りない時
			now_rank = Castle_GetRank( wk->sv, wk->type, CASTLE_RANK_TYPE_KAIHUKU );
			if( now_rank < kaihuku_msg_tbl[wk->list_csr_pos][0] ){
				CastleTalkWinPut( &wk->bmpwin[MINE_BMPWIN_TALK], CONFIG_GetWindowType(wk->config) );
				wk->msg_index = Castle_EasyMsg( wk, msg_castle_poke_14, FONT_TALK );
				wk->sub_seq = SEQ_SUB_KAIHUKU_KEY_WAIT_LIST_RETURN;
				break;
			}
			
			//CPが足りない時
			if( now_cp < kaihuku_cp_tbl[wk->list_csr_pos] ){
				//改行が入っているので相談
				CastleTalkWinPut( &wk->bmpwin[MINE_BMPWIN_TALK], CONFIG_GetWindowType(wk->config) );
				wk->msg_index = Castle_EasyMsg( wk, msg_castle_poke_03, FONT_TALK );
				wk->sub_seq = SEQ_SUB_KAIHUKU_KEY_WAIT_LIST_RETURN;
				break;
			}

			if( wk->list_csr_pos == 0 ){

				//HP回復の必要がない時
				if( PokeParaGet(poke,ID_PARA_hp,NULL) == PokeParaGet(poke,ID_PARA_hpmax,NULL) ){
					wk->msg_index = Castle_EasyMsg( wk, msg_castle_poke_10, FONT_TALK );
					wk->sub_seq = SEQ_SUB_KAIHUKU_KEY_WAIT_LIST_RETURN;
					break;
				}
			}else if( wk->list_csr_pos == 1 ){

				//PP回復の必要がない時
				if( CastleMine_PPRecoverCheck(poke) == FALSE ){
					wk->msg_index = Castle_EasyMsg( wk, msg_castle_poke_10, FONT_TALK );
					wk->sub_seq = SEQ_SUB_KAIHUKU_KEY_WAIT_LIST_RETURN;
					break;
				}

			}else{

				//HP,PP回復の必要がない時
				if( (PokeParaGet(poke,ID_PARA_hp,NULL) == PokeParaGet(poke,ID_PARA_hpmax,NULL)) &&
					(CastleMine_PPRecoverCheck(poke) == FALSE) ){
					wk->msg_index = Castle_EasyMsg( wk, msg_castle_poke_10, FONT_TALK );
					wk->sub_seq = SEQ_SUB_KAIHUKU_KEY_WAIT_LIST_RETURN;
					break;
				}
			}

			BmpTalkWinClearSub( &wk->bmpwin[MINE_BMPWIN_TALK] );

			if( Castle_CommCheck(wk->type) == FALSE ){

#if 1
				//CP減らす
				Castle_CPRecord_Sub(wk->fro_sv, wk->type,
							kaihuku_cp_tbl[wk->parent_decide_type-FC_PARAM_LIST_START_KAIHUKU] );

				CastleMine_SeqSubCPWrite( wk, &wk->bmpwin[MINE_BMPWIN_TR1] );			//CP表示
#endif

				//選んだポケモンの位置、カーソルの位置を渡す
				CastleMine_SeqSubKaihuku( wk, wk->csr_pos, wk->parent_decide_type );	//回復
				wk->sub_seq = SEQ_SUB_DECIDE_EFF_WAIT;
			}else{
				wk->send_req = 1;
				return TRUE;
			}
			break;
			
		//「いいえ」
		case 1:
		case BMPMENU_CANCEL:
			CastleMine_SeqSubMenuWinClear( wk );					//メニューを表示していたら削除
			BmpTalkWinClearSub( &wk->bmpwin[MINE_BMPWIN_TALK] );
			CastleMine_Kaihuku_Write( wk );
			wk->sub_seq = SEQ_SUB_KAIHUKU_ITEM_LIST;
			break;
		};

		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//回復ランクアップしますか「はい・いいえ」選択中
	case SEQ_SUB_KAIHUKU_RANKUP_YES_NO:

		ret = BmpMenuMain( wk->mw );

		switch( ret ){

		case BMPMENU_NULL:
			break;

		//「はい」
		case 0:
			CastleMine_SeqSubMenuWinClear( wk );					//メニューを表示していたら削除

			now_cp = FrontierRecord_Get(wk->fro_sv, CastleScr_GetCPRecordID(wk->type),
										//FRONTIER_RECORD_NOT_FRIEND );		//現在のCP
										Frontier_GetFriendIndex(CastleScr_GetCPRecordID(wk->type)));
			now_rank = Castle_GetRank( wk->sv, wk->type, CASTLE_RANK_TYPE_KAIHUKU );

			//CPの比較
			if( now_cp < rankup_point_tbl[CASTLE_RANK_TYPE_KAIHUKU][now_rank] ){
				//「ＣＰが　たりません」
				CastleMine_SeqSubMenuWinClear( wk );				//メニューを表示していたら削除
				CastleTalkWinPut( &wk->bmpwin[MINE_BMPWIN_TALK], CONFIG_GetWindowType(wk->config) );
				wk->msg_index = Castle_EasyMsg( wk, msg_castle_poke_40, FONT_TALK );
				wk->sub_seq = SEQ_SUB_KAIHUKU_KEY_WAIT_LIST_RETURN;
				break;
			}
				
			//ランクアップ
			if( Castle_CommCheck(wk->type) == FALSE ){
				CastleRank_SeqSubRankUp( wk, wk->csr_pos, FC_PARAM_KAIHUKU_RANKUP );
				wk->sub_seq = SEQ_SUB_KAIHUKU_RANKUP_DECIDE_EFF_WAIT;
				break;
			}else{
				wk->send_req		= 1;
				return TRUE;
			}

			break;
			
		//「いいえ」
		case 1:
		case BMPMENU_CANCEL:
			CastleMine_SeqSubMenuWinClear( wk );				//メニューを表示していたら削除
			BmpTalkWinClearSub( &wk->bmpwin[MINE_BMPWIN_TALK] );
			CastleMine_Kaihuku_Write( wk );
			wk->sub_seq = SEQ_SUB_KAIHUKU_ITEM_LIST;
			break;
		};
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//回復ランクアップ決定エフェクト待ち
	case SEQ_SUB_KAIHUKU_RANKUP_DECIDE_EFF_WAIT:
		if( CastleMine_DecideEff(wk,wk->csr_pos,wk->parent_decide_type) == TRUE ){
			wk->sub_seq = SEQ_SUB_KAIHUKU_RANKUP_DECIDE_EFF_KEY_WAIT;
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//回復ランクアップ決定エフェクト待ちキー待ち
	case SEQ_SUB_KAIHUKU_RANKUP_DECIDE_EFF_KEY_WAIT:
		if( sys.trg & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL) ){
			Snd_SePlay( SEQ_SE_DP_SELECT );
			CastleMine_SeqSubMenuWinClear( wk );				//メニューを表示していたら削除
			//BmpTalkWinClear( &wk->bmpwin[MINE_BMPWIN_TALK], WINDOW_TRANS_OFF );
			CastleMine_Kaihuku_Write( wk );

			//ランクアップエフェクト
			CastleObj_RankUpEff( wk->p_eff_rankup, CASTLE_MINE_RANKUP_X, CASTLE_MINE_RANKUP_Y );

			wk->sub_seq = SEQ_SUB_KAIHUKU_ITEM_LIST;
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//「回復」キー待ちして、リストへ戻る
	case SEQ_SUB_KAIHUKU_KEY_WAIT_LIST_RETURN:
		if( sys.trg & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL) ){
			Snd_SePlay( SEQ_SE_DP_SELECT );
			BmpTalkWinClearSub( &wk->bmpwin[MINE_BMPWIN_TALK] );
			CastleMine_Kaihuku_Write( wk );
			wk->sub_seq = SEQ_SUB_KAIHUKU_ITEM_LIST;
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//「レンタル」リスト処理
	case SEQ_SUB_RENTAL_LIST:
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );			//TEST

		ret = BmpListMain( wk->lw );
		Castle_ListSeCall( ret, SEQ_SE_DP_SELECT );
		BmpListDirectPosGet( wk->lw, &wk->list_csr_pos );					//カーソル位置

		switch( ret ){

		case BMPLIST_NULL:
			break;

		case BMPLIST_CANCEL:
		//case (RENTAL_MSG_TBL_MAX - 1):			//やめる
			BmpTalkWinClearSub( &wk->bmpwin[MINE_BMPWIN_TALK] );
			CastleMine_Rental_Del( wk );		//TALKMENU2,RENTAL
			CastleMine_Basic_Write( wk );		//TALKMENU2,BASIC
			wk->sub_seq = SEQ_SUB_MENU_MAIN;
			break;

		case FC_PARAM_KINOMI:
			wk->parent_decide_type = ret;									//パラメータ
			BmpTalkWinClearSub( &wk->bmpwin[MINE_BMPWIN_TALK] );
			CastleMine_Rental_Del( wk );		//TALKMENU2,RENTAL
			CastleMine_ItemSeed_Write( wk, FC_PARAM_KINOMI );
			wk->sub_seq = SEQ_SUB_RENTAL_ITEM_SEED_LIST;
			break;

		case FC_PARAM_ITEM:
			wk->parent_decide_type = ret;		//パラメータ
			BmpTalkWinClearSub( &wk->bmpwin[MINE_BMPWIN_TALK] );
			CastleMine_Rental_Del( wk );		//TALKMENU2,RENTAL

			//「どうぐ」を選択している時(ランクが足りない時)
			now_rank = Castle_GetRank( wk->sv, wk->type, CASTLE_RANK_TYPE_RENTAL );
			if( now_rank == 1 ){
				CastleTalkWinPut( &wk->bmpwin[MINE_BMPWIN_TALK], CONFIG_GetWindowType(wk->config) );
				wk->msg_index = Castle_EasyMsg( wk, msg_castle_poke_29, FONT_TALK );
				wk->sub_seq = SEQ_SUB_RENTAL_KEY_WAIT_LIST_RETURN;
				return FALSE;	//注意！
			}

			CastleMine_ItemSeed_Write( wk, FC_PARAM_ITEM );
			wk->sub_seq = SEQ_SUB_RENTAL_ITEM_SEED_LIST;
			break;

		case FC_PARAM_RENTAL_RANKUP:
			now_rank = Castle_GetRank( wk->sv, wk->type, CASTLE_RANK_TYPE_RENTAL );

			//ランクアップＭＡＸを選択
			if( now_rank == CASTLE_RANK_MAX ){
				//「これ　いじょう　ランクは　あがりません！」
				//wk->msg_index = Castle_EasyMsg( wk, msg_castle_poke_23, FONT_TALK );
				//wk->sub_seq = SEQ_SUB_RENTAL_KEY_WAIT_LIST_RETURN;
				Snd_SeStopBySeqNo( SEQ_SE_DP_SELECT, 0 );
				Snd_SePlay( SEQ_SE_DP_BOX03 );
			}else{
				wk->parent_decide_type = ret;		//パラメータ
				CastleMine_Rental_Del( wk );		//TALKMENU2,RENTAL

				/*「○ＣＰを使ってランクをあげますか？」*/
				Castle_SetNumber(wk, 0, rankup_point_tbl[CASTLE_RANK_TYPE_RENTAL][now_rank], 
								 CASTLE_KETA_CP, NUMBER_DISPTYPE_LEFT );
				wk->msg_index = Castle_EasyMsg( wk, msg_castle_poke_24, FONT_TALK );
				Castle_SetMenu2( wk );						//「はい・いいえ」
				wk->sub_seq = SEQ_SUB_RENTAL_RANKUP_YES_NO;
			}
			break;

		};
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//「きのみ、どうぐ」リスト処理
	case SEQ_SUB_RENTAL_ITEM_SEED_LIST:
		ret = BmpListMain( wk->lw );
		Castle_ListSeCall( ret, SEQ_SE_DP_SELECT );
		BmpListDirectPosGet( wk->lw, &wk->list_csr_pos );					//カーソル位置

		switch( ret ){

		case BMPLIST_NULL:
			break;

		case BMPLIST_CANCEL:
			CastleMine_ItemSeed_Del( wk );
			CastleMine_Rental_Write( wk );
			wk->sub_seq = SEQ_SUB_RENTAL_LIST;
			break;

		default:
			GF_BGL_BmpWinOffVReq( &wk->bmpwin[MINE_BMPWIN_ITEMINFO] );

			//会話ウィンドウ表示
			CastleTalkWinPut( &wk->bmpwin[MINE_BMPWIN_TALK], CONFIG_GetWindowType(wk->config));

			//「○ＣＰ　かかりますが　よろしい　ですか？」
			Castle_SetNumber(	wk, 0, GetItemCP(wk,wk->list_csr_pos,wk->parent_decide_type), 
								CASTLE_KETA_STATUS, NUMBER_DISPTYPE_LEFT );
			wk->msg_index = Castle_EasyMsg( wk, msg_castle_poke_02, FONT_TALK );
			Castle_SetMenu2( wk );						//「はい・いいえ」
			CastleObj_Vanish( wk->p_itemicon, CASTLE_VANISH_ON );					//非表示
			wk->sub_seq = SEQ_SUB_RENTAL_YES_NO;
			break;
		};

		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//「道具」そのアイテムでよい「はい・いいえ」メニュー処理
	case SEQ_SUB_RENTAL_YES_NO:
		poke =  PokeParty_GetMemberPointer( wk->p_party, GetCsrPokePos(wk->h_max,wk->csr_pos) );
		ret = BmpMenuMain( wk->mw );

		switch( ret ){

		case BMPMENU_NULL:
			break;

		//「はい」
		case 0:
			CastleMine_SeqSubMenuWinClear( wk );				//メニューを表示していたら削除

			now_cp = FrontierRecord_Get(wk->fro_sv, CastleScr_GetCPRecordID(wk->type),
										//FRONTIER_RECORD_NOT_FRIEND );		//現在のCP
										Frontier_GetFriendIndex(CastleScr_GetCPRecordID(wk->type)));

			//CPが足りない時
			if( now_cp < GetItemCP(wk,wk->list_csr_pos,wk->parent_decide_type) ){
				//改行が入っているので相談
				CastleTalkWinPut( &wk->bmpwin[MINE_BMPWIN_TALK], CONFIG_GetWindowType(wk->config) );
				wk->msg_index = Castle_EasyMsg( wk, msg_castle_poke_03, FONT_TALK );
				CastleObj_Vanish( wk->p_itemicon, CASTLE_VANISH_ON );	//非表示
				wk->sub_seq = SEQ_SUB_RENTAL_KEY_WAIT_ITEM_SEED_RETURN;
				break;
			}

			//アイテムを持っていないポケモンの時
			if( PokeParaGet(poke,ID_PARA_item,NULL) == 0 ){

				if( Castle_CommCheck(wk->type) == FALSE ){
					CastleMine_ItemSeed_Del( wk );
					CastleTalkWinPut(	&wk->bmpwin[MINE_BMPWIN_TALK], 
										CONFIG_GetWindowType(wk->config));

					//「○に○を持たせました！」
					Castle_CPRecord_Sub(wk->fro_sv, wk->type, 
										GetItemCP(wk,wk->list_csr_pos,wk->parent_decide_type));

					CastleMine_SeqSubCPWrite( wk, &wk->bmpwin[MINE_BMPWIN_TR1] );	//CP表示
					CastleMine_SeqSubItem(	wk, wk->csr_pos, 
											GetItemNo(wk,wk->list_csr_pos,wk->parent_decide_type) );
					wk->sub_seq = SEQ_SUB_DECIDE_EFF_WAIT;
				}else{
					wk->parent_decide_item = GetItemNo(wk,wk->list_csr_pos,wk->parent_decide_type);
					CastleMine_ItemSeed_Del( wk );
					BmpTalkWinClearSub( &wk->bmpwin[MINE_BMPWIN_TALK] );
					wk->send_req = 1;
					return TRUE;
				}

			}else{

				//アイテムを持っているポケモンの時

				//「○はすでに○を持っています！」
				//「持っている道具を取り替えますか？」
				Castle_SetPokeName( wk, 0, PPPPointerGet(poke) );
                // MatchComment: WORDSET_RegisterItemName -> WORDSET_RegisterItemNameIndefinate
				WORDSET_RegisterItemNameIndefinate( wk->wordset, 1, PokeParaGet(poke,ID_PARA_item,NULL) );
				wk->msg_index = Castle_EasyMsg( wk, msg_castle_poke_08, FONT_TALK );
				wk->sub_seq = SEQ_SUB_RENTAL_ITEM_TRADE;					//すでにアイテム持ってる
			}
			
			break;
			
		//「いいえ」
		case 1:
		case BMPMENU_CANCEL:
			CastleMine_SeqSubMenuWinClear( wk );					//メニューを表示していたら削除
			CastleMine_SeqSubItemListReturn( wk );
			wk->sub_seq = SEQ_SUB_RENTAL_ITEM_SEED_LIST;			//リスト処理へ戻る
			break;
		};

		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//レンタルランクアップしますか「はい・いいえ」選択中
	case SEQ_SUB_RENTAL_RANKUP_YES_NO:

		ret = BmpMenuMain( wk->mw );

		switch( ret ){

		case BMPMENU_NULL:
			break;

		//「はい」
		case 0:
			CastleMine_SeqSubMenuWinClear( wk );					//メニューを表示していたら削除

			now_cp = FrontierRecord_Get(wk->fro_sv, CastleScr_GetCPRecordID(wk->type),
										//FRONTIER_RECORD_NOT_FRIEND );		//現在のCP
										Frontier_GetFriendIndex(CastleScr_GetCPRecordID(wk->type)));
			now_rank = Castle_GetRank( wk->sv, wk->type, CASTLE_RANK_TYPE_RENTAL );

			//CPの比較
			if( now_cp < rankup_point_tbl[CASTLE_RANK_TYPE_RENTAL][now_rank] ){
				//「ＣＰが　たりません」
				CastleMine_SeqSubMenuWinClear( wk );				//メニューを表示していたら削除
				CastleTalkWinPut( &wk->bmpwin[MINE_BMPWIN_TALK], CONFIG_GetWindowType(wk->config) );
				wk->msg_index = Castle_EasyMsg( wk, msg_castle_poke_40, FONT_TALK );
				wk->sub_seq = SEQ_SUB_RENTAL_KEY_WAIT_LIST_RETURN;
				break;
			}
				
			//ランクアップ
			if( Castle_CommCheck(wk->type) == FALSE ){
				CastleRank_SeqSubRankUp( wk, wk->csr_pos, FC_PARAM_RENTAL_RANKUP );
				wk->sub_seq = SEQ_SUB_RENTAL_RANKUP_DECIDE_EFF_WAIT;
				break;
			}else{
				wk->send_req		= 1;
				return TRUE;
			}
			break;
			
		//「いいえ」
		case 1:
		case BMPMENU_CANCEL:
			CastleMine_SeqSubMenuWinClear( wk );				//メニューを表示していたら削除
			BmpTalkWinClearSub( &wk->bmpwin[MINE_BMPWIN_TALK] );
			CastleMine_Rental_Write( wk );
			wk->sub_seq = SEQ_SUB_RENTAL_LIST;
			break;
		};
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//レンタルランクアップ決定エフェクト待ち
	case SEQ_SUB_RENTAL_RANKUP_DECIDE_EFF_WAIT:
		if( CastleMine_DecideEff(wk,wk->csr_pos,wk->parent_decide_type) == TRUE ){
			wk->sub_seq = SEQ_SUB_RENTAL_RANKUP_DECIDE_EFF_KEY_WAIT;
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//レンタルランクアップ決定エフェクト待ちキー待ち
	case SEQ_SUB_RENTAL_RANKUP_DECIDE_EFF_KEY_WAIT:
		if( sys.trg & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL) ){
			Snd_SePlay( SEQ_SE_DP_SELECT );
			CastleMine_SeqSubMenuWinClear( wk );				//メニューを表示していたら削除
			//BmpTalkWinClear( &wk->bmpwin[MINE_BMPWIN_TALK], WINDOW_TRANS_OFF );
			CastleMine_Rental_Write( wk );

			//ランクアップエフェクト
			CastleObj_RankUpEff( wk->p_eff_rankup, CASTLE_MINE_RANKUP_X2, CASTLE_MINE_RANKUP_Y2 );

			wk->sub_seq = SEQ_SUB_RENTAL_LIST;
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//「道具」キー待ちして、レンタルリストへ戻る
	case SEQ_SUB_RENTAL_KEY_WAIT_LIST_RETURN:
		if( sys.trg & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL) ){
			Snd_SePlay( SEQ_SE_DP_SELECT );
			BmpTalkWinClearSub( &wk->bmpwin[MINE_BMPWIN_TALK] );
			CastleMine_Rental_Write( wk );
			wk->sub_seq = SEQ_SUB_RENTAL_LIST;
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//「道具」キー待ちして、きのみ、道具リストへ戻る
	case SEQ_SUB_RENTAL_KEY_WAIT_ITEM_SEED_RETURN:
		if( sys.trg & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL) ){
			Snd_SePlay( SEQ_SE_DP_SELECT );
			CastleMine_SeqSubItemListReturn( wk );
			wk->sub_seq = SEQ_SUB_RENTAL_ITEM_SEED_LIST;				//リスト処理へ戻る
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//すでに道具もっています
	case SEQ_SUB_RENTAL_ITEM_TRADE:
		if( sys.trg & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL) ){
			Snd_SePlay( SEQ_SE_DP_SELECT );
			wk->msg_index = Castle_EasyMsg( wk, msg_castle_poke_09, FONT_TALK );
			Castle_SetMenu2( wk );										//「はい・いいえ」
			//BmpTalkWinClear( &wk->bmpwin[MINE_BMPWIN_TALK], WINDOW_TRANS_OFF );
			wk->sub_seq = SEQ_SUB_RENTAL_ITEM_TRADE_YES_NO;				//交換する？
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//道具交換する？「はい・いいえ」
	case SEQ_SUB_RENTAL_ITEM_TRADE_YES_NO:
		ret = BmpMenuMain( wk->mw );

		switch( ret ){

		case BMPMENU_NULL:
			break;

		//「はい」
		case 0:
			CastleMine_SeqSubMenuWinClear( wk );				//メニューを表示していたら削除

			if( Castle_CommCheck(wk->type) == FALSE ){
				CastleMine_ItemSeed_Del( wk );
				CastleTalkWinPut( &wk->bmpwin[MINE_BMPWIN_TALK], CONFIG_GetWindowType(wk->config));

				//「○に○を持たせました！」
				Castle_CPRecord_Sub(wk->fro_sv, wk->type,
									GetItemCP(wk,wk->list_csr_pos,wk->parent_decide_type) );
				CastleMine_SeqSubCPWrite( wk, &wk->bmpwin[MINE_BMPWIN_TR1] );	//CP表示
				CastleMine_SeqSubItem(	wk, wk->csr_pos, 
										GetItemNo(wk,wk->list_csr_pos,wk->parent_decide_type) );
				wk->sub_seq = SEQ_SUB_DECIDE_EFF_WAIT;
			}else{
				wk->parent_decide_item = GetItemNo( wk, wk->list_csr_pos, wk->parent_decide_type );
				CastleMine_ItemSeed_Del( wk );
				BmpTalkWinClearSub( &wk->bmpwin[MINE_BMPWIN_TALK] );
				wk->send_req = 1;
				return TRUE;
			}
			break;
			
		//「いいえ」
		case 1:
		case BMPMENU_CANCEL:
			CastleMine_SeqSubMenuWinClear( wk );				//メニューを表示していたら削除
			CastleMine_SeqSubItemListReturn( wk );
			wk->sub_seq = SEQ_SUB_RENTAL_ITEM_SEED_LIST;						//リスト処理へ戻る
			break;
		};

		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//決定エフェクト待ち
	case SEQ_SUB_DECIDE_EFF_WAIT:
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );				//TEST
		if( CastleMine_DecideEff(wk,wk->csr_pos,wk->parent_decide_type) == TRUE ){
			wk->sub_seq = SEQ_SUB_DECIDE_KEY_WAIT;
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//決定エフェクト後のキー待ち
	case SEQ_SUB_DECIDE_KEY_WAIT:
		if( sys.trg & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL) ){
			Snd_SePlay( SEQ_SE_DP_SELECT );
			BmpTalkWinClearSub( &wk->bmpwin[MINE_BMPWIN_TALK] );
			CastleMine_Default_Write( wk );		//TALKMENU,もどる
			wk->sub_seq = SEQ_SUB_START;
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//つよさ：キー待ちしてステータスなどのウィンドウを消してBASICリストへ
	case SEQ_SUB_INFO_TUYOSA_KEY_WAIT_WIN_OFF:
		if( sys.trg & PAD_KEY_LEFT ){
			CastleMine_StatusWinChg( wk, -1 );
		}else if( sys.trg & PAD_KEY_RIGHT ){
			CastleMine_StatusWinChg( wk, 1 );
		}else if( sys.trg & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL) ){
			Snd_SePlay( SEQ_SE_DP_SELECT );
			CastleMine_SeqSubStatusWazaDel( wk );							//ステータス・技を削除
			CastleMine_Basic_Write( wk );		//TALKMENU2,BASIC
			wk->sub_seq = SEQ_SUB_MENU_MAIN;
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//わざ：キー待ちしてステータスなどのウィンドウを消してBASICリストへ
	case SEQ_SUB_INFO_WAZA_KEY_WAIT_WIN_OFF:
		if( sys.trg & PAD_KEY_LEFT ){
			CastleMine_WazaWinChg( wk, -1 );
		}else if( sys.trg & PAD_KEY_RIGHT ){
			CastleMine_WazaWinChg( wk, 1 );
		}else if( sys.trg & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL) ){
			Snd_SePlay( SEQ_SE_DP_SELECT );
			CastleMine_SeqSubStatusWazaDel( wk );							//ステータス・技を削除
			CastleMine_Basic_Write( wk );		//TALKMENU2,BASIC
			wk->sub_seq = SEQ_SUB_MENU_MAIN;
		}
		break;

	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	送受信
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameSendRecv( CASTLE_MINE_WORK* wk )
{
	u8 sel_pos;
	POKEMON_PARAM* poke;

	switch( wk->sub_seq ){

	case 0:
		wk->rankup_recover_flag = 0;	//ランクアップした時に戻り先が特殊フラグOFF

		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );		//TEST

		//選んだリクエスト(やりたいこと)を送信
		if( CastleMine_CommSetSendBuf(wk,CASTLE_COMM_MINE_UP_TYPE,wk->csr_pos) == TRUE ){
			wk->send_req = 0;
			wk->sub_seq++;
		}
		break;

	case 1:
		if( wk->parent_decide_pos == CASTLE_MINE_DECIDE_NONE ){
			break;
		}

		//親が決めたランクとLRをランクアップさせる
		wk->recieve_count = 0;

		//ランクを上げる処理とそれ以外で分けている
		if( (wk->parent_decide_type == FC_PARAM_KAIHUKU_RANKUP) ||
			(wk->parent_decide_type == FC_PARAM_RENTAL_RANKUP) ){
			CastleRank_SeqSubRankUp( wk, wk->parent_decide_pos, wk->parent_decide_type );
		}else{
			CastleMine_SeqSubRankUp( wk, wk->parent_decide_pos, wk->parent_decide_type );
		}

		wk->sub_seq++;
		break;

	//決定エフェクト待ち
	case 2:
		sel_pos = GetCommSelCsrPos( wk->modoru_pos, wk->parent_decide_pos );	//カーソル位置取得
		if( CastleMine_DecideEff(wk,sel_pos,wk->parent_decide_type) == TRUE ){
			wk->wait = CASTLE_COMM_WAIT;
			wk->sub_seq++;
		}
		break;

	//キー待ち
	case 3:
		//if( sys.trg & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL) ){
		//Snd_SePlay( SEQ_SE_DP_SELECT );
		wk->wait--;
		if( wk->wait == 0 ){
			//同期開始
			CommToolTempDataReset();
			CommTimingSyncStart( DBC_TIM_CASTLE_MINE_UP );
			wk->sub_seq++;
			break;
		}
		break;

	//同期
	case 4:
		//タイミングコマンドが届いたか確認
		if( CommIsTimingSync(DBC_TIM_CASTLE_MINE_UP) == TRUE ){
			CommToolTempDataReset();
			CommToolInitialize( HEAPID_CASTLE );	//timingSyncEnd=0xff
			wk->parent_decide_pos = CASTLE_MINE_DECIDE_NONE;

			//ランクアップした時に戻り先が特殊かチェック
			if( wk->rankup_recover_flag == 0 ){
				BmpTalkWinClearSub( &wk->bmpwin[MINE_BMPWIN_TALK] );
				CastleMine_Default_Write( wk );								//TALKMENU,もどる
			}

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
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameEndMulti( CASTLE_MINE_WORK* wk )
{
	int i;

	switch( wk->sub_seq ){

	//「戻る」送信
	case 0:
		if( CastleMine_CommSetSendBuf(wk,CASTLE_COMM_MINE_MODORU,0) == TRUE ){
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
			CommTimingSyncStart( DBC_TIM_CASTLE_MINE_END );
			wk->sub_seq++;
		}
		break;

	//同期待ち
	case 2:
		//タイミングコマンドが届いたか確認
		if( CommIsTimingSync(DBC_TIM_CASTLE_MINE_END) == TRUE ){
			CommToolTempDataReset();
			BmpTalkWinClearSub( &wk->bmpwin[MINE_BMPWIN_TALK] );
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
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameEnd( CASTLE_MINE_WORK* wk )
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
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleCommon_Delete( CASTLE_MINE_WORK* wk )
{
	u8 m_max;
	int i,j;

	CastleObj_Delete( wk->p_scr_u );
	CastleObj_Delete( wk->p_scr_d );
	CastleObj_Delete( wk->p_itemicon );
	//CastleObj_Delete( wk->p_eff );		//いらない
	CastleObj_Delete( wk->p_eff_rankup );
	CastleObj_Delete( wk->p_poke_sel );
	CastleObj_Delete( wk->p_pair_poke_sel );
	CastleObj_Delete( wk->p_item_csr );

	m_max = Castle_GetMinePokeNum( wk->type, CASTLE_FLAG_TOTAL );
	for( i=0; i < m_max ;i++ ){
		CastleObj_Delete( wk->p_icon[i] );
		CastleObj_Delete( wk->p_itemkeep[i] );
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

	CastleClact_DeleteCellObject(&wk->castle_clact);	//2Dオブジェクト関連領域開放

	MSGMAN_Delete( wk->msgman );						//メッセージマネージャ開放
	MSGMAN_Delete( wk->msgman_iteminfo );				//メッセージマネージャ開放
	WORDSET_Delete( wk->wordset );
	STRBUF_Delete( wk->msg_buf );						//メッセージバッファ開放
	STRBUF_Delete( wk->tmp_buf );						//メッセージバッファ開放
	NUMFONT_Delete( wk->num_font );

	for( i=0; i < CASTLE_MENU_BUF_MAX ;i++ ){
		STRBUF_Delete( wk->menu_buf[i] );				//メニューバッファ開放
	}

	CastleExitBmpWin( wk->bmpwin, CASTLE_BMP_MINE );	//BMPウィンドウ開放
	Castle_BgExit( wk->bgl );							//BGL削除
	ArchiveDataHandleClose( wk->hdl );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	共通 初期化1
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
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
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_InitSub2( CASTLE_MINE_WORK* wk )
{
	u32 start_x,start_y;
	u32 hp,hp_max,anm_no,hp_anm_no;
	u8 m_max;
	u16 x,y,offset_x,offset_y,pair_offset_x,pair_offset_y,icon_offset_x,itemkeep_offset_x;
	int i,j,flip;
	GF_BGL_BMPWIN* win;
	POKEMON_PARAM* poke;

	wk->hdl = ArchiveDataHandleOpen( ARC_FRONTIER_BG, HEAPID_CASTLE );
	Castle_BgInit( wk );								//BG初期化
	Castle_ObjInit( wk );								//OBJ初期化

	//メッセージデータマネージャー作成
	wk->msgman = MSGMAN_Create( MSGMAN_TYPE_DIRECT, ARC_MSG, 
								NARC_msg_castle_poke_dat, HEAPID_CASTLE );

	//アイテム情報
	//wk->msgman_iteminfo = MSGMAN_Create(MSGMAN_TYPE_NORMAL, ARC_MSG, 
	wk->msgman_iteminfo = MSGMAN_Create(MSGMAN_TYPE_DIRECT, ARC_MSG, 
										NARC_msg_iteminfo_dat, HEAPID_CASTLE );
										
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

	//8x8フォント作成(使用bmpwin[MINE_BMPWIN_TYPE]のパレット(BC_FONT_PAL)を使用
	//wk->num_font = NUMFONT_Create( 15, 14, FBMP_COL_NULL, HEAPID_CASTLE );
	wk->num_font = NUMFONT_Create( FBMP_COL_BLACK, FBMP_COL_BLK_SDW, FBMP_COL_NULL, HEAPID_CASTLE );

	//ビットマップ追加
	CastleAddBmpWin( wk->bgl, wk->bmpwin, CASTLE_BMP_MINE );

	//通信しているかで表示のオフセットが変わる
	Castle_GetOffset( wk, &offset_x, &offset_y, &pair_offset_x, &pair_offset_y );

	////////////////////////////////////////////////////////////////////////////////////////
	//リストスクロールカーソルOBJ追加
	wk->p_scr_u = CastleObj_Create(	&wk->castle_clact, 
									ID_CHAR_CSR, ID_PLTT_CSR, 
									ID_CELL_CSR, CASTLE_ANM_SCR_CURSOR_U, 
									CASTLE_SCR_U_X, CASTLE_SCR_U_Y, CASTLE_BG_PRI_HIGH, NULL );
	wk->p_scr_d = CastleObj_Create(	&wk->castle_clact,
									ID_CHAR_CSR, ID_PLTT_CSR, 
									ID_CELL_CSR, CASTLE_ANM_SCR_CURSOR_D, 
									CASTLE_SCR_D_X, CASTLE_SCR_D_Y, CASTLE_BG_PRI_HIGH, NULL );
	CastleObj_Vanish( wk->p_scr_u, CASTLE_VANISH_ON );					//非表示
	CastleObj_Vanish( wk->p_scr_d, CASTLE_VANISH_ON );					//非表示

	////////////////////////////////////////////////////////////////////////////////////////
	if( Castle_CommCheck(wk->type) == FALSE ){
		icon_offset_x		= ICON_START_X;
		itemkeep_offset_x	= ICON_START_X + CASTLE_ITEMKEEP_OFFSET_X;
	}else{
		icon_offset_x		= ICON_MULTI_START_X;
		itemkeep_offset_x	= ICON_MULTI_START_X + CASTLE_ITEMKEEP_OFFSET_X;
	}

	m_max = Castle_GetMinePokeNum( wk->type, CASTLE_FLAG_TOTAL );
	for( i=0; i < m_max ;i++ ){

		//アイテム持っているアイコンOBJ追加
		wk->p_itemkeep[i] = CastleObj_Create(	&wk->castle_clact,
												ID_CHAR_ITEMKEEP, ID_PLTT_ITEMKEEP, 
												ID_CELL_ITEMKEEP, 0,
												ICON_WIDTH_X*i+itemkeep_offset_x, 
												ICON_START_Y + CASTLE_ITEMKEEP_OFFSET_Y, 
												CASTLE_BG_PRI_LOW, NULL );

		//アイテムを持っていないポケモンの時はアイテム持っているアイコンを非表示
		poke =  PokeParty_GetMemberPointer( wk->p_party, i );
		if( PokeParaGet(poke,ID_PARA_item,NULL) == 0 ){
			CastleObj_Vanish( wk->p_itemkeep[i], CASTLE_VANISH_ON );			//非表示
		}

		//ポケモンアイコンのアニメナンバーを取得
		hp = PokeParaGet( poke, ID_PARA_hp, NULL );
		hp_max = PokeParaGet( poke, ID_PARA_hpmax, NULL );
		anm_no = CastleMine_GetPokeAnmNo( wk, GetHPGaugeDottoColor(hp,hp_max,48) );		//POKEアニメ
		hp_anm_no = CastleMine_GetHpAnmNo( wk, GetHPGaugeDottoColor(hp,hp_max,48) );	//HPアニメ

		//HP状態OBJ追加
		wk->p_hp[i] = CastleObj_Create(	&wk->castle_clact,
										ID_CHAR_CSR, ID_PLTT_CSR, 
										ID_CELL_CSR, hp_anm_no,
										ICON_WIDTH_X*i+icon_offset_x, 
										ICON_START_Y+20, CASTLE_BG_PRI_BACK, NULL );

		//ポケモンアイコンOBJ追加
		wk->p_icon[i] = CastleObj_Create(	&wk->castle_clact,
											ID_CHAR_ICON1+i, ID_PLTT_ICON, 
											ID_CELL_ICON, anm_no,
											ICON_WIDTH_X*i+icon_offset_x, 
											ICON_START_Y, CASTLE_BG_PRI_LOW, NULL );

		//パレット切り替え
		CastleObj_IconPalChg( wk->p_icon[i], PokeParty_GetMemberPointer(wk->p_party,i) );
	}

	////////////////////////////////////////////////////////////////////////////////////////
	//アイテムアイコンOBJ
	wk->p_itemicon = CastleObj_Create(	&wk->castle_clact, 
										ID_CHAR_ITEMICON, ID_PLTT_ITEMICON,
										ID_CELL_ITEMICON, 0, 
										CASTLE_ITEMICON_X, CASTLE_ITEMICON_Y, 
										CASTLE_BG_PRI_HIGH, NULL );

	//アイテムアイコン(CHAR、PLTT変更)
	CastleClact_ItemIconCharChange( &wk->castle_clact, ITEM_KIZUGUSURI );
	CastleClact_ItemIconPlttChange( &wk->castle_clact, ITEM_KIZUGUSURI );

	CastleObj_Vanish( wk->p_itemicon, CASTLE_VANISH_ON );					//非表示

	////////////////////////////////////////////////////////////////////////////////////////
	//ポケモン選択ウィンOBJ追加
	CastleMine_GetPokeSelXY( wk, &start_x, &start_y, 0 );
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
	//ランクアップOBJ追加
	wk->p_eff_rankup = CastleObj_Create(&wk->castle_clact, 
										ID_CHAR_CSR, ID_PLTT_CSR, 
										ID_CELL_CSR, CASTLE_ANM_RANK_UP, 
										20, 20,			//値適当
										CASTLE_BG_PRI_HIGH, NULL );
	CastleObj_Vanish( wk->p_eff_rankup, CASTLE_VANISH_ON );					//非表示

	////////////////////////////////////////////////////////////////////////////////////////
	//アイテムカーソルOBJ追加
	wk->p_item_csr = CastleObj_Create(	&wk->castle_clact, 
										ID_CHAR_CSR, ID_PLTT_CSR, 
										ID_CELL_CSR, CASTLE_ANM_CURSOR_ITEM, 
										20, 20,			//値適当
										CASTLE_BG_PRI_NORMAL, NULL );
	CastleObj_Vanish( wk->p_item_csr, CASTLE_VANISH_ON );					//非表示

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
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_BgInit( CASTLE_MINE_WORK* wk )
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
	//Castle_SetItemBgGraphic( wk, BC_FRAME_SLIDE );
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
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_ObjInit( CASTLE_MINE_WORK* wk )
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
	CASTLE_MINE_WORK* wk = work;

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
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 * @param	frm		フレームナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_SetMainBgGraphic( CASTLE_MINE_WORK * wk, u32 frm  )
{
	ArcUtil_HDL_BgCharSet(	wk->hdl, BC_TEMOCHI_NCGR_BIN, 
							wk->bgl, frm, 0, 0, 1, HEAPID_CASTLE );

	if( Castle_CommCheck(wk->type) == FALSE ){
		ArcUtil_HDL_ScrnSet(wk->hdl, BC_TEMOCHI01_NSCR_BIN, 
							wk->bgl, frm, 0, 0, 1, HEAPID_CASTLE );
	}else{
		ArcUtil_HDL_ScrnSet(wk->hdl, BC_TEMOCHI02_NSCR_BIN, 
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

	buf = ArcUtil_PalDataGet( ARC_FRONTIER_BG, BC_TEMOCHI_NCLR, &dat, HEAPID_CASTLE );

	DC_FlushRange( dat->pRawData, (sizeof(u16)*16*4) );
	GX_LoadBGPltt( dat->pRawData, 0, (sizeof(u16)*16*4) );		//メイン

	sys_FreeMemoryEz(buf);
	return;
}

//--------------------------------------------------------------
/**
 * @brief	グラフィックデータセット：上画面ステータス
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 * @param	frm		フレームナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_SetStatusBgGraphic( CASTLE_MINE_WORK * wk, u32 frm  )
{
	ArcUtil_HDL_BgCharSet(	wk->hdl, BC_TEMOCHI_NCGR_BIN, 
							wk->bgl, frm, 0, 0, 1, HEAPID_CASTLE );

	ArcUtil_HDL_ScrnSet(wk->hdl, BC_TEMOCHI_STATUS_NSCR_BIN, 
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

	buf = ArcUtil_PalDataGet( ARC_FRONTIER_BG, BC_TEMOCHI_NCLR, &dat, HEAPID_CASTLE );

	DC_FlushRange( dat->pRawData, (sizeof(u16)*16*4) );
	GX_LoadBGPltt( dat->pRawData, 0, (sizeof(u16)*16*4) );		//メイン

	sys_FreeMemoryEz(buf);
	return;
}

//--------------------------------------------------------------
/**
 * @brief	グラフィックデータセット：上画面わざ
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 * @param	frm		フレームナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_SetWazaBgGraphic( CASTLE_MINE_WORK * wk, u32 frm  )
{
	ArcUtil_HDL_BgCharSet(	wk->hdl, BC_TEMOCHI_NCGR_BIN, 
							wk->bgl, frm, 0, 0, 1, HEAPID_CASTLE );

	ArcUtil_HDL_ScrnSet(wk->hdl, BC_TEMOCHI_WAZA_NSCR_BIN, 
						wk->bgl, frm, 0, 0, 1, HEAPID_CASTLE );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	グラフィックデータセット：上画面道具
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 * @param	frm		フレームナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_SetItemBgGraphic( CASTLE_MINE_WORK * wk, u32 frm  )
{
	ArcUtil_HDL_BgCharSet(	wk->hdl, BC_TEMOCHI_NCGR_BIN, 
							wk->bgl, frm, 0, 0, 1, HEAPID_CASTLE );

	ArcUtil_HDL_ScrnSet(wk->hdl, BC_TEMOCHI_ITEM_NSCR_BIN, 
						wk->bgl, frm, 0, 0, 1, HEAPID_CASTLE );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	グラフィックデータセット：下画面背景、パレットセット
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 * @param	frm		フレームナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_SetSubBgGraphic( CASTLE_MINE_WORK * wk, u32 frm  )
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

// NONMATCHING
asm static u8 CastleWriteMsg( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font )
{
	push {r4, r5, lr}
	sub sp, #0x1c
	ldr r4, [sp, #0x28]
	str r4, [sp]
	ldr r4, [sp, #0x2c]
	str r4, [sp, #4]
	add r4, sp, #0x18
	ldrb r5, [r4, #0x18]
	str r5, [sp, #8]
	ldrb r4, [r4, #0x1c]
	str r4, [sp, #0xc]
	add r4, sp, #0x38
	ldrb r4, [r4]
	str r4, [sp, #0x10]
	add r4, sp, #0x3c
	ldrb r4, [r4]
	str r4, [sp, #0x14]
	mov r4, #0
	str r4, [sp, #0x18]
	bl CastleWriteMsg_Full_ov107_22437CC
	add sp, #0x1c
	pop {r4, r5, pc}
	// .align 2, 0
}


//--------------------------------------------------------------
/**
 * @brief	メッセージ表示
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
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
#ifdef NONEQUIVALENT
// prototype probably wrong
static u8 CastleWriteMsg_Full_ov107_22437CC( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font )
{
	u8 msg_index;

	GF_BGL_BmpWinDataFill( win, b_col );			//"********塗りつぶし********"
	MSGMAN_GetString( wk->msgman, msg_id, wk->tmp_buf );

	//登録された単語を使って文字列展開する
	WORDSET_ExpandStr( wk->wordset, wk->msg_buf, wk->tmp_buf );

	msg_index = GF_STR_PrintColor( win, font, wk->msg_buf, x, y, wait, 
								GF_PRINTCOLOR_MAKE(f_col,s_col,b_col), NULL );

	GF_BGL_BmpWinOnVReq( win );
	return msg_index;
}
#else
asm static u8 CastleWriteMsg_Full_ov107_22437CC( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font )
{
	push {r3, r4, r5, r6, r7, lr}
	sub sp, #0x10
	add r6, r1, #0
	add r1, sp, #0x38
	ldrb r1, [r1]
	add r5, r0, #0
	add r0, r6, #0
	add r7, r2, #0
	add r4, r3, #0
	bl GF_BGL_BmpWinDataFill
	ldr r0, [r5, #0x20]
	ldr r2, [r5, #0x2c]
	add r1, r7, #0
	bl MSGMAN_GetString
	ldr r0, [r5, #0x24]
	ldr r1, [r5, #0x28]
	ldr r2, [r5, #0x2c]
	bl WORDSET_ExpandStr
	ldr r0, [sp, #0x40]
	cmp r0, #1
	beq _02243802
	cmp r0, #2
	beq _02243814
	b _02243820
_02243802:
	mov r0, #0
	ldr r1, [r5, #0x28]
	add r2, r0, #0
	bl FontProc_GetPrintStrWidth
	add r0, r0, #1
	lsr r0, r0, #1
	sub r4, r4, r0
	b _02243820
_02243814:
	mov r0, #0
	ldr r1, [r5, #0x28]
	add r2, r0, #0
	bl FontProc_GetPrintStrWidth
	sub r4, r4, r0
_02243820:
	ldr r0, [sp, #0x28]
	add r2, sp, #0x18
	str r0, [sp]
	ldr r0, [sp, #0x2c]
	add r3, r4, #0
	str r0, [sp, #4]
	add r0, sp, #0x38
	ldrb r1, [r0]
	ldrb r0, [r2, #0x18]
	ldrb r2, [r2, #0x1c]
	lsl r0, r0, #0x18
	lsl r2, r2, #0x18
	lsr r0, r0, #8
	lsr r2, r2, #0x10
	orr r0, r2
	orr r0, r1
	str r0, [sp, #8]
	mov r0, #0
	str r0, [sp, #0xc]
	add r1, sp, #0x3c
	ldrb r1, [r1]
	ldr r2, [r5, #0x28]
	add r0, r6, #0
	bl GF_STR_PrintColor
	add r4, r0, #0
	add r0, r6, #0
	bl GF_BGL_BmpWinOnVReq
	add r0, r4, #0
	add sp, #0x10
	pop {r3, r4, r5, r6, r7, pc}
}
#endif

extern void ov107_2243860(void);

asm static u8 CastleWriteMsgSimple( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font )
{
	push {r4, r5, lr}
	sub sp, #0x1c
	ldr r4, [sp, #0x28]
	str r4, [sp]
	ldr r4, [sp, #0x2c]
	str r4, [sp, #4]
	add r4, sp, #0x18
	ldrb r5, [r4, #0x18]
	str r5, [sp, #8]
	ldrb r4, [r4, #0x1c]
	str r4, [sp, #0xc]
	add r4, sp, #0x38
	ldrb r4, [r4]
	str r4, [sp, #0x10]
	add r4, sp, #0x3c
	ldrb r4, [r4]
	str r4, [sp, #0x14]
	mov r4, #0
	str r4, [sp, #0x18]
	bl CastleWriteMsgSimple_Full_ov107_2243890
	add sp, #0x1c
	pop {r4, r5, pc}
	// .align 2, 0
}

//--------------------------------------------------------------
/**
 * @brief	メッセージ表示(塗りつぶしなし)
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
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
#ifdef NONEQUIVALENT
static u8 CastleWriteMsgSimple_Full_ov107_2243890( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font, u32 a10_unk_mode )
{
	u8 msg_index;

	MSGMAN_GetString( wk->msgman, msg_id, wk->tmp_buf );

	//登録された単語を使って文字列展開する
	WORDSET_ExpandStr( wk->wordset, wk->msg_buf, wk->tmp_buf );

	msg_index = GF_STR_PrintColor( win, font, wk->msg_buf, x, y, wait, 
								GF_PRINTCOLOR_MAKE(f_col,s_col,b_col), NULL );

	GF_BGL_BmpWinOnVReq( win );
	return msg_index;
}
#else
asm static u8 CastleWriteMsgSimple_Full_ov107_2243890( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font, u32 a10_unk_mode )
{
	push {r4, r5, r6, lr}
	sub sp, #0x10
	add r5, r0, #0
	add r6, r1, #0
	add r1, r2, #0
	ldr r0, [r5, #0x20]
	ldr r2, [r5, #0x2c]
	add r4, r3, #0
	bl MSGMAN_GetString
	ldr r0, [r5, #0x24]
	ldr r1, [r5, #0x28]
	ldr r2, [r5, #0x2c]
	bl WORDSET_ExpandStr
	ldr r0, [sp, #0x38]
	cmp r0, #1
	beq _022438BA
	cmp r0, #2
	beq _022438CC
	b _022438D8
_022438BA:
	mov r0, #0
	ldr r1, [r5, #0x28]
	add r2, r0, #0
	bl FontProc_GetPrintStrWidth
	add r0, r0, #1
	lsr r0, r0, #1
	sub r4, r4, r0
	b _022438D8
_022438CC:
	mov r0, #0
	ldr r1, [r5, #0x28]
	add r2, r0, #0
	bl FontProc_GetPrintStrWidth
	sub r4, r4, r0
_022438D8:
	ldr r0, [sp, #0x20]
	add r2, sp, #0x10
	str r0, [sp]
	ldr r0, [sp, #0x24]
	add r3, r4, #0
	str r0, [sp, #4]
	add r0, sp, #0x30
	ldrb r1, [r0]
	ldrb r0, [r2, #0x18]
	ldrb r2, [r2, #0x1c]
	lsl r0, r0, #0x18
	lsl r2, r2, #0x18
	lsr r0, r0, #8
	lsr r2, r2, #0x10
	orr r0, r2
	orr r0, r1
	str r0, [sp, #8]
	mov r0, #0
	str r0, [sp, #0xc]
	add r1, sp, #0x34
	ldrb r1, [r1]
	ldr r2, [r5, #0x28]
	add r0, r6, #0
	bl GF_STR_PrintColor
	add r4, r0, #0
	add r0, r6, #0
	bl GF_BGL_BmpWinOnVReq
	add r0, r4, #0
	add sp, #0x10
	pop {r4, r5, r6, pc}
}
#endif

//--------------------------------------------------------------
/**
 * @brief	簡単メッセージ表示
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 * @param	msg_id	メッセージID
 *
 * @return	"msg_index"
 */
//--------------------------------------------------------------
static u8 Castle_EasyMsg( CASTLE_MINE_WORK* wk, int msg_id, u8 font )
{
	u8 msg_index;

	msg_index = CastleWriteMsg(	wk, &wk->bmpwin[MINE_BMPWIN_TALK], msg_id, 
								1, 1, MSG_NO_PUT, 
								FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_WHITE, font );

	GF_BGL_BmpWinOnVReq( &wk->bmpwin[MINE_BMPWIN_TALK] );
	return msg_index;
}
//--------------------------------------------------------------
/**
 * @brief	ステータスメッセージ表示
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
enum{
	//もちもの
	STATUS_MOTIMONO_X =		(1),
	STATUS_MOTIMONO_Y =		(1*8),
	STATUS_MOTIMONO_NUM_X =	(8*8),

	//せいかく　○
	STATUS_SEIKAKU_X =		(1),
	STATUS_SEIKAKU_Y =		(3*8),
	STATUS_SEIKAKU_NUM_X =	(8*8),

	//とくせい
	STATUS_TOKUSEI_X =		(1),
	STATUS_TOKUSEI_Y =		(5*8),
	STATUS_TOKUSEI_NUM_X =	(8*8),

	//こうげき
	STATUS_KOUGEKI_X =		(1),
	STATUS_KOUGEKI_Y =		(7*8),
	STATUS_KOUGEKI_NUM_X =	(7*8),

	//ぼうぎょ
	STATUS_BOUGYO_X =		(11*8),
	STATUS_BOUGYO_Y =		(STATUS_KOUGEKI_Y),
	STATUS_BOUGYO_NUM_X =	(18*8),

	//とくこう
	STATUS_TOKUKOU_X =		(1),
	STATUS_TOKUKOU_Y =		(9*8),
	STATUS_TOKUKOU_NUM_X =	(STATUS_KOUGEKI_NUM_X),

	//とくぼう
	STATUS_TOKUBOU_X =		(11*8),
	STATUS_TOKUBOU_Y =		(STATUS_TOKUKOU_Y),
	STATUS_TOKUBOU_NUM_X =	(STATUS_BOUGYO_NUM_X),

	//すばやさ
	STATUS_SUBAYASA_X =		(1),
	STATUS_SUBAYASA_Y =		(11*8),
	STATUS_SUBAYASA_NUM_X =	(STATUS_KOUGEKI_NUM_X),
};

#ifdef NONEQUIVALENT
static void Castle_StatusMsgWrite( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, POKEMON_PARAM* poke )
{
	GF_BGL_BmpWinDataFill( win,FBMP_COL_NULL );			//"********塗りつぶし********"

	//「もちもの」
	WORDSET_RegisterItemName( wk->wordset, 0, PokeParaGet(poke,ID_PARA_item,NULL) );
	StMsgWriteSub( wk, win, msg_castle_poke_status_01, STATUS_MOTIMONO_X, STATUS_MOTIMONO_Y );
	StMsgWriteSub( wk, win,msg_castle_poke_status_01_01,STATUS_MOTIMONO_NUM_X,STATUS_MOTIMONO_Y);

	//「せいかく」
	WORDSET_RegisterSeikaku( wk->wordset, 0, PokeSeikakuGet(poke) );
	StMsgWriteSub( wk, win, msg_castle_poke_status_02, STATUS_SEIKAKU_X, STATUS_SEIKAKU_Y );
	StMsgWriteSub( wk, win, msg_castle_poke_status_02_01, STATUS_SEIKAKU_NUM_X, STATUS_SEIKAKU_Y );

	//「とくせい」
	WORDSET_RegisterTokuseiName( wk->wordset, 0, PokeParaGet(poke,ID_PARA_speabino,NULL) );
	StMsgWriteSub( wk, win, msg_castle_poke_status_03, STATUS_TOKUSEI_X, STATUS_TOKUSEI_Y );
	StMsgWriteSub( wk, win, msg_castle_poke_status_03_01, STATUS_TOKUSEI_NUM_X,STATUS_TOKUSEI_Y);

	//「こうげき」
	//OS_Printf( "pow = %d\n", PokeParaGet(poke,ID_PARA_pow,NULL) );
	Castle_SetNumber(	wk, 0, PokeParaGet(poke,ID_PARA_pow,NULL), CASTLE_KETA_STATUS,
						NUMBER_DISPTYPE_SPACE );
	StMsgWriteSub( wk, win, msg_castle_poke_status_04, STATUS_KOUGEKI_X, STATUS_KOUGEKI_Y );
	StMsgWriteSub( wk, win, msg_castle_poke_status_04_01, STATUS_KOUGEKI_NUM_X,STATUS_KOUGEKI_Y);

	//「ぼうぎょ」
	Castle_SetNumber(	wk, 0, PokeParaGet(poke,ID_PARA_def,NULL), CASTLE_KETA_STATUS,
						NUMBER_DISPTYPE_SPACE );
	StMsgWriteSub( wk, win, msg_castle_poke_status_05, STATUS_BOUGYO_X, STATUS_BOUGYO_Y );
	StMsgWriteSub( wk, win, msg_castle_poke_status_05_01, STATUS_BOUGYO_NUM_X, STATUS_BOUGYO_Y );

	//「とくこう」
	Castle_SetNumber(	wk, 0, PokeParaGet(poke,ID_PARA_spepow,NULL), CASTLE_KETA_STATUS,
						NUMBER_DISPTYPE_SPACE );
	StMsgWriteSub( wk, win, msg_castle_poke_status_06, STATUS_TOKUKOU_X, STATUS_TOKUKOU_Y );
	StMsgWriteSub( wk, win, msg_castle_poke_status_06_01, STATUS_TOKUKOU_NUM_X,STATUS_TOKUKOU_Y);

	//「とくぼう」
	Castle_SetNumber(	wk, 0, PokeParaGet(poke,ID_PARA_spedef,NULL), CASTLE_KETA_STATUS,
						NUMBER_DISPTYPE_SPACE );
	StMsgWriteSub( wk, win, msg_castle_poke_status_07, STATUS_TOKUBOU_X, STATUS_TOKUBOU_Y );
	StMsgWriteSub( wk, win, msg_castle_poke_status_07_01, STATUS_TOKUBOU_NUM_X,STATUS_TOKUBOU_Y);

	//「すばやさ」
	Castle_SetNumber(	wk, 0, PokeParaGet(poke,ID_PARA_agi,NULL), CASTLE_KETA_STATUS,
						NUMBER_DISPTYPE_SPACE );
	StMsgWriteSub( wk, win, msg_castle_poke_status_08, STATUS_SUBAYASA_X, STATUS_SUBAYASA_Y );
	StMsgWriteSub( wk, win,msg_castle_poke_status_08_01,STATUS_SUBAYASA_NUM_X,STATUS_SUBAYASA_Y);

	GF_BGL_BmpWinOnVReq( win );
	return;
}
#else
asm static void Castle_StatusMsgWrite( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, POKEMON_PARAM* poke )
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
	mov r0, #8
	str r0, [sp]
	mov r3, #0
	add r0, r5, #0
	add r1, r4, #0
	mov r2, #0x46
	str r3, [sp, #4]
	bl StMsgWriteSub
	mov r0, #8
	str r0, [sp]
	mov r0, #0
	str r0, [sp, #4]
	add r0, r5, #0
	add r1, r4, #0
	mov r2, #0x47
	mov r3, #0x40
	bl StMsgWriteSub
	add r0, r6, #0
	bl PokeSeikakuGet
	add r2, r0, #0
	ldr r0, [r5, #0x24]
	mov r1, #0
	bl WORDSET_RegisterSeikaku
	mov r0, #0x18
	str r0, [sp]
	mov r3, #0
	add r0, r5, #0
	add r1, r4, #0
	mov r2, #0x48
	str r3, [sp, #4]
	bl StMsgWriteSub
	mov r0, #0x18
	str r0, [sp]
	mov r0, #0
	str r0, [sp, #4]
	add r0, r5, #0
	add r1, r4, #0
	mov r2, #0x49
	mov r3, #0x40
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
	mov r3, #0
	add r0, r5, #0
	add r1, r4, #0
	mov r2, #0x4a
	str r3, [sp, #4]
	bl StMsgWriteSub
	mov r0, #0x28
	str r0, [sp]
	mov r0, #0
	str r0, [sp, #4]
	add r0, r5, #0
	add r1, r4, #0
	mov r2, #0x4b
	mov r3, #0x40
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
	mov r3, #0
	add r0, r5, #0
	add r1, r4, #0
	mov r2, #0x4c
	str r3, [sp, #4]
	bl StMsgWriteSub
	mov r0, #0x38
	str r0, [sp]
	mov r0, #2
	str r0, [sp, #4]
	add r0, r5, #0
	add r1, r4, #0
	mov r2, #0x4d
	mov r3, #0x50
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
	mov r0, #0x38
	str r0, [sp]
	mov r0, #0
	str r0, [sp, #4]
	add r0, r5, #0
	add r1, r4, #0
	mov r2, #0x4e
	mov r3, #0x60
	bl StMsgWriteSub
	mov r0, #0x38
	str r0, [sp]
	mov r0, #2
	str r0, [sp, #4]
	add r0, r5, #0
	add r1, r4, #0
	mov r2, #0x4f
	mov r3, #0xb0
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
	mov r3, #0
	add r0, r5, #0
	add r1, r4, #0
	mov r2, #0x50
	str r3, [sp, #4]
	bl StMsgWriteSub
	mov r0, #0x48
	str r0, [sp]
	mov r0, #2
	str r0, [sp, #4]
	add r0, r5, #0
	add r1, r4, #0
	mov r2, #0x51
	mov r3, #0x50
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
	mov r2, #0x52
	mov r3, #0x60
	bl StMsgWriteSub
	mov r0, #0x48
	str r0, [sp]
	mov r0, #2
	str r0, [sp, #4]
	add r0, r5, #0
	add r1, r4, #0
	mov r2, #0x53
	mov r3, #0xb0
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
	mov r3, #0
	add r0, r5, #0
	add r1, r4, #0
	mov r2, #0x54
	str r3, [sp, #4]
	bl StMsgWriteSub
	mov r0, #0x58
	str r0, [sp]
	mov r0, #2
	str r0, [sp, #4]
	add r0, r5, #0
	add r1, r4, #0
	mov r2, #0x55
	mov r3, #0x50
	bl StMsgWriteSub
	add r0, r4, #0
	bl GF_BGL_BmpWinOnVReq
	add sp, #8
	pop {r4, r5, r6, pc}
	// .align 2, 0
}
#endif

#ifdef NONEQUIVALENT
static void StMsgWriteSub( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, int msg, u16 x, u16 y )
{
	CastleWriteMsgSimple(	wk, win, msg, x, y, MSG_NO_PUT,
							FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, BC_FONT );
	return;
}
#else
asm static void StMsgWriteSub( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, int msg, u16 x, u16 y )
{
	push {r3, r4, lr}
	sub sp, #0x1c
	add r4, sp, #0x18
	ldrh r4, [r4, #0x10]
	str r4, [sp]
	mov r4, #0xff
	str r4, [sp, #4]
	mov r4, #1
	str r4, [sp, #8]
	mov r4, #2
	str r4, [sp, #0xc]
	mov r4, #0
	str r4, [sp, #0x10]
	str r4, [sp, #0x14]
	ldr r4, [sp, #0x2c]
	str r4, [sp, #0x18]
	bl CastleWriteMsgSimple_Full_ov107_2243890
	add sp, #0x1c
	pop {r3, r4, pc}
}
#endif

//--------------------------------------------------------------
/**
 * @brief	技メッセージ表示
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
enum{
	//技名
	STATUS_WAZA_X =			(3*8),
	STATUS_WAZA_Y =			(1*8+4),

	//技ポイント
	STATUS_POINT_X =		(17*8+1), // MatchComment: 12*8 -> 17*8+1
	STATUS_POINT_Y =		(1*8+4),
};

static void Castle_WazaMsgWrite( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, POKEMON_PARAM* poke )
{
	GF_BGL_BmpWinDataFill( win,FBMP_COL_NULL );			//"********塗りつぶし********"

	//id+iは保証されるかわからないので、やらない。

	WazaMsgWriteSub(	wk, win, 0, msg_castle_poke_waza_01_01, 
						msg_castle_poke_waza_02, poke, ID_PARA_waza1, 
						ID_PARA_pp1, ID_PARA_pp_max1 );

	WazaMsgWriteSub(	wk, win, 1, msg_castle_poke_waza_01_02, 
						msg_castle_poke_waza_02, poke, ID_PARA_waza2, 
						ID_PARA_pp2, ID_PARA_pp_max2 );

	WazaMsgWriteSub(	wk, win, 2, msg_castle_poke_waza_01_03, 
						msg_castle_poke_waza_02, poke, ID_PARA_waza3, 
						ID_PARA_pp3, ID_PARA_pp_max3 );

	WazaMsgWriteSub(	wk, win, 3, msg_castle_poke_waza_01_04, 
						msg_castle_poke_waza_02, poke, ID_PARA_waza4, 
						ID_PARA_pp4, ID_PARA_pp_max4 );

	GF_BGL_BmpWinOnVReq( win );
	return;
}

static void WazaMsgWriteSub( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, u8 no, u32 msg_id, u32 msg_id2, POKEMON_PARAM* poke, u32 id, u32 id2, u32 id3 )
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
    // MatchComment: NUMBER_DISPTYPE_SPACE -> NUMBER_DISPTYPE_LEFT
	Castle_SetNumber(	wk, 4, PokeParaGet(poke,id2,NULL), CASTLE_KETA_STATUS,
						NUMBER_DISPTYPE_LEFT );
	Castle_SetNumber(	wk, 5, PokeParaGet(poke,id3,NULL), CASTLE_KETA_STATUS,
						NUMBER_DISPTYPE_LEFT );
    // MatchComment: call CastleWriteMsgSimple_Full_ov107_2243890 instead of CastleWriteMsgSimple
    // has additional argument of 1
	wk->msg_index = CastleWriteMsgSimple_Full_ov107_2243890( wk, win,
									msg_id2, point_x, point_y, MSG_NO_PUT,
									FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, BC_FONT, 1 );
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * HPとMHPのスラッシュ表示
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void Castle_PokeHpMsgWrite( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win )
{
	int i;
	u8 m_max;

	GF_BGL_BmpWinDataFill( win, FBMP_COL_NULL );					//塗りつぶし

	m_max = Castle_GetMinePokeNum( wk->type, CASTLE_FLAG_TOTAL );

	for( i=0;i < m_max ;i++ ){
		Castle_PokeHpMsgWriteSub( wk, win, i, 0 );
	}

	GF_BGL_BmpWinOnVReq( win );
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * HPとMHPのスラッシュ表示
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 * @param	flag	0=通常、1=アイテムウィンドウ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void Castle_PokeHpMsgWriteSub( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, u8 no, u8 flag )
{
	u16 hp_offset_x,hp_x,hp_y,hpmax_x,hpmax_y,slash_x,slash_y;
	POKEMON_PARAM* poke;

	poke =  PokeParty_GetMemberPointer( wk->p_party, no );
	OS_Printf( "*********poke = %d\n", poke );

	if( Castle_CommCheck(wk->type) == FALSE ){
		hp_offset_x = CASTLE_HP_START_X;
	}else{
		hp_offset_x = CASTLE_MULTI_HP_START_X;
	}

	//0=通常、1=アイテムウィンドウ
	if( flag == 0 ){
		hp_x = hp_offset_x + (CASTLE_HP_WIDTH_X * no);
		hp_y = CASTLE_HP_START_Y;
		slash_x = CASTLE_HPSLASH_START_X + hp_offset_x + (CASTLE_HP_WIDTH_X * no);
		slash_y = CASTLE_HP_START_Y;
		hpmax_x = CASTLE_HPMAX_START_X + hp_offset_x + (CASTLE_HP_WIDTH_X * no);
		hpmax_y = CASTLE_HP_START_Y;
	}else{
		hp_x = 4;
		hp_y = 0;
		slash_x = CASTLE_HPSLASH_START_X + 4;
		slash_y = 0;
		hpmax_x = CASTLE_HPMAX_START_X + 4;
		hpmax_y = 0;
	}

	//HPは右づめ
	NUMFONT_WriteNumber(wk->num_font, PokeParaGet(poke,ID_PARA_hp,NULL), CASTLE_KETA_HP, 
						NUMFONT_MODE_SPACE, win, hp_x, hp_y );

	//HPとMHPのスラッシュ(記号描画)
	NUMFONT_WriteMark(	wk->num_font, NUMFONT_MARK_SLASH, win, slash_x, slash_y );

	//HPMAXは左づめ
	NUMFONT_WriteNumber(wk->num_font, PokeParaGet(poke,ID_PARA_hpmax,NULL), CASTLE_KETA_HP, 
						NUMFONT_MODE_LEFT, win, hpmax_x, hpmax_y );

	return;
}

//--------------------------------------------------------------------------------------------
/**
 * レベル表示
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void Castle_PokeLvMsgWrite( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win )
{
	int i;
	u8 m_max;

	m_max = Castle_GetMinePokeNum( wk->type, CASTLE_FLAG_TOTAL );

	for( i=0;i < m_max ;i++ ){
		Castle_PokeLvMsgWriteSub( wk, win, i, 0 );
	}

	GF_BGL_BmpWinOnVReq( win );
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * レベル表示
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 * @param	flag	0=通常、1=アイテムウィンドウ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void Castle_PokeLvMsgWriteSub( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, u8 no, u8 flag )
{
	u32 sex,lv_x,lv_y,sex_x,sex_y;
	u16 lv_offset_x,sex_offset_x;
	POKEMON_PARAM* poke;

	if( Castle_CommCheck(wk->type) == FALSE ){
		lv_offset_x = CASTLE_LV_START_X;
		sex_offset_x = CASTLE_SEX_START_X;
	}else{
		lv_offset_x = CASTLE_MULTI_LV_START_X;
		sex_offset_x = CASTLE_MULTI_SEX_START_X;
	}

	poke =  PokeParty_GetMemberPointer( wk->p_party, no );

	//0=通常、1=アイテムウィンドウ
	if( flag == 0 ){
		lv_x = lv_offset_x + (CASTLE_LV_WIDTH_X * no);
		lv_y = CASTLE_LV_START_Y;
		sex_x = sex_offset_x + (CASTLE_SEX_WIDTH_X * no);
		sex_y = CASTLE_SEX_START_Y;
	}else{
		lv_x = 4;
		lv_y = 0;
		sex_x = (8*6);
		sex_y = 0;
	}

	//LV(記号と数字をセットで描画)
	NUMFONT_WriteSet(	wk->num_font, NUMFONT_MARK_LV, PokeParaGet(poke,ID_PARA_level,NULL),
						CASTLE_KETA_LV, NUMFONT_MODE_LEFT, win, lv_x, lv_y );

	//性別表示
	sex = PokeParaGet( poke, ID_PARA_sex, NULL );
	PokeSexWrite( wk, win, sex_x, sex_y, BC_FONT, sex );
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * 名前＋CPウィンドウ表示
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void Castle_NameCPWinWrite( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win )
{
	u16 now_cp;

	now_cp = FrontierRecord_Get(wk->fro_sv, CastleScr_GetCPRecordID(wk->type),
								Frontier_GetFriendIndex(CastleScr_GetCPRecordID(wk->type)) );

	//メニューウィンドウを描画
	CastleWriteMenuWin( wk->bgl, win );

	GF_BGL_BmpWinDataFill( win, FBMP_COL_WHITE );			//塗りつぶし
	PlayerNameWrite( wk, win, 1, 1, BC_FONT );				//プレイヤー名を表示

	Castle_SetNumber( wk, 0, now_cp, CASTLE_KETA_CP, NUMBER_DISPTYPE_SPACE );
	wk->msg_index = CastleWriteMsgSimple( wk, win, msg_castle_poke_cp_03,
									CASTLE_NAME_CP_START_X, 1*16 , MSG_NO_PUT, 
									FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, BC_FONT );

	GF_BGL_BmpWinOnVReq( win );
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * CPウィンドウ更新
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void Castle_NameCPWrite( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win )
{
	u16 now_cp;

	now_cp = FrontierRecord_Get(wk->fro_sv, CastleScr_GetCPRecordID(wk->type),
								Frontier_GetFriendIndex(CastleScr_GetCPRecordID(wk->type)) );

	GF_BGL_BmpWinFill( win, FBMP_COL_NULL, 0, 7*8, 16, 16 );
	//GF_BGL_BmpWinDataFill( win, FBMP_COL_NULL );				//塗りつぶし

	//PlayerNameWrite( wk, win, 1, 1, BC_FONT );				//プレイヤー名を表示

	Castle_SetNumber( wk, 0, now_cp, CASTLE_KETA_CP, NUMBER_DISPTYPE_SPACE );
	wk->msg_index = CastleWriteMsgSimple( wk, win, 
									msg_castle_poke_cp_03, 1, 1*16 , MSG_NO_PUT, 
									FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, BC_FONT );

	GF_BGL_BmpWinOnVReq( win );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	道具情報メッセージ表示
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 * @param	msg_id	メッセージID
 *
 * @return	"msg_index"
 */
//--------------------------------------------------------------
static u8 CastleItemInfoWriteMsg( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, u16 item )
{
	u8 msg_index;

	GF_BGL_BmpWinDataFill( win, FBMP_COL_NULL );			//塗りつぶし

	MSGMAN_GetString( wk->msgman_iteminfo, item, wk->tmp_buf );

	//登録された単語を使って文字列展開する
	WORDSET_ExpandStr( wk->wordset, wk->msg_buf, wk->tmp_buf );

	msg_index = GF_STR_PrintColor(win,BC_FONT, wk->msg_buf, ITEM_INFO_PX, ITEM_INFO_PY, MSG_NO_PUT, 
								GF_PRINTCOLOR_MAKE(FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL), 
								NULL );

	GF_BGL_BmpWinOnVReq( win );
	return msg_index;
}

//--------------------------------------------------------------------------------------------
/**
 * HP表示を更新
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void Castle_PokeHpEff( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, u8 no, u32 hp )
{
	u16 hp_offset_x;

	if( Castle_CommCheck(wk->type) == FALSE ){
		hp_offset_x = CASTLE_HP_START_X;
	}else{
		hp_offset_x = CASTLE_MULTI_HP_START_X;
	}

	GF_BGL_BmpWinFill(	win, FBMP_COL_NULL, hp_offset_x + (CASTLE_HP_WIDTH_X * no), 0, 
						//CASTLE_HP_WIDTH_X, 8 );
						//8*3, 8 );
						8*3, 9 );

	//HPは右づめ
	NUMFONT_WriteNumber(wk->num_font, hp, CASTLE_KETA_HP, 
						NUMFONT_MODE_SPACE, win, 
						hp_offset_x + (CASTLE_HP_WIDTH_X * no), CASTLE_HP_START_Y );

	GF_BGL_BmpWinOnVReq( win );
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * 初期状態を表示
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
#ifdef NONEQUIVALENT
static void CastleMine_Default_Write( CASTLE_MINE_WORK* wk )
{
	//「もどる」
	wk->msg_index = CastleWriteMsg( wk, &wk->bmpwin[MINE_BMPWIN_MODORU], 
									msg_castle_poke_00_02, 1, 1+4 , MSG_NO_PUT, 
									FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM );

	//下にメニューウィンドウで説明を表示
	//CastleWriteMenuWin( wk->bgl, &wk->bmpwin[MINE_BMPWIN_TALKMENU] );
	CastleTalkWinPut( &wk->bmpwin[MINE_BMPWIN_TALKMENU], CONFIG_GetWindowType(wk->config) );

	wk->msg_index = CastleWriteMsg( wk, &wk->bmpwin[MINE_BMPWIN_TALKMENU], 
									msg_castle_poke_00_01, 1, 1, MSG_NO_PUT, 
									FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_WHITE, FONT_TALK );
	return;
}
#else
asm static void CastleMine_Default_Write( CASTLE_MINE_WORK* wk )
{
	push {r3, r4, lr}
	sub sp, #0x1c
	mov r1, #5
	str r1, [sp]
	mov r1, #0xff
	str r1, [sp, #4]
	mov r2, #1
	str r2, [sp, #8]
	mov r1, #2
	str r1, [sp, #0xc]
	mov r1, #0
	str r1, [sp, #0x10]
	str r1, [sp, #0x14]
	add r4, r0, #0
	add r1, r4, #0
	str r2, [sp, #0x18]
	add r1, #0x60
	mov r2, #6
	mov r3, #0x10
	bl CastleWriteMsg_Full_ov107_22437CC
	strb r0, [r4, #0xa]
	mov r0, #0x72
	lsl r0, r0, #2
	ldr r0, [r4, r0]
	bl CONFIG_GetWindowType
	add r1, r0, #0
	add r0, r4, #0
	add r0, #0xc0
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
	add r1, #0xc0
	mov r2, #5
	str r3, [sp, #0x14]
	bl CastleWriteMsg
	strb r0, [r4, #0xa]
	add sp, #0x1c
	pop {r3, r4, pc}
	// .align 2, 0
}
#endif

//--------------------------------------------------------------------------------------------
/**
 * 初期状態を削除
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CastleMine_Default_Del( CASTLE_MINE_WORK* wk )
{
	//BmpMenuWinClear( &wk->bmpwin[MINE_BMPWIN_TALKMENU], WINDOW_TRANS_OFF );
	//GF_BGL_BmpWinOffVReq( &wk->bmpwin[MINE_BMPWIN_TALKMENU] );
	BmpTalkWinClearSub( &wk->bmpwin[MINE_BMPWIN_TALKMENU] );
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * 基本状態を表示
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CastleMine_Basic_Write( CASTLE_MINE_WORK* wk )
{
	//下にメニューウィンドウ2で説明を表示
	//CastleWriteMenuWin( wk->bgl, &wk->bmpwin[MINE_BMPWIN_TALKMENU2] );
	CastleTalkWinPut( &wk->bmpwin[MINE_BMPWIN_TALKMENU2], CONFIG_GetWindowType(wk->config) );

	/*「ＨＰやＰＰをかいふくできます」*/
	//wk->msg_index = CastleWriteMsg( wk, &wk->bmpwin[MINE_BMPWIN_TALKMENU2], 
	//								msg_castle_poke_choice_01, 1, 1, MSG_NO_PUT, 
	//								FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_WHITE, BC_FONT );

	wk->list_csr_pos = 0;
	Castle_BasicListMake( wk );
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * 基本状態を削除
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CastleMine_Basic_Del( CASTLE_MINE_WORK* wk )
{
	BmpListPosGet( wk->lw, &wk->basic_list_lp, &wk->basic_list_cp );
	//BmpMenuWinClear( &wk->bmpwin[MINE_BMPWIN_TALKMENU2], WINDOW_TRANS_OFF );
	//GF_BGL_BmpWinOffVReq( &wk->bmpwin[MINE_BMPWIN_TALKMENU2] );
	BmpTalkWinClearSub( &wk->bmpwin[MINE_BMPWIN_TALKMENU2] );
	CastleMine_SeqSubListEnd( wk );
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * 回復項目状態を表示
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CastleMine_Kaihuku_Write( CASTLE_MINE_WORK* wk )
{
	//会話ウィンドウ表示
	CastleTalkWinPut(	&wk->bmpwin[MINE_BMPWIN_TALK], 
						CONFIG_GetWindowType(wk->config));
	//BmpTalkWinPutSub( wk );

	//「どれを使いますか？」
	wk->msg_index = Castle_EasyMsg( wk, msg_castle_poke_13, FONT_TALK );

	wk->list_csr_pos = 0;
	Castle_KaihukuListMake( wk );
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * 回復項目状態を削除
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CastleMine_Kaihuku_Del( CASTLE_MINE_WORK* wk )
{
	CastleMine_SeqSubListEnd( wk );
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * レンタル項目状態を表示
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CastleMine_Rental_Write( CASTLE_MINE_WORK* wk )
{
	//会話ウィンドウ表示
	CastleTalkWinPut(	&wk->bmpwin[MINE_BMPWIN_TALK], 
						CONFIG_GetWindowType(wk->config));

	//wk->msg_index = Castle_EasyMsg( wk, msg_castle_poke_41, FONT_TALK );
	
	wk->list_csr_pos = 0;
	Castle_RentalListMake( wk );
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * レンタル項目状態を削除
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CastleMine_Rental_Del( CASTLE_MINE_WORK* wk )
{
	CastleMine_SeqSubListEnd( wk );
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * アイテムきのみ項目状態を表示
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CastleMine_ItemSeed_Write( CASTLE_MINE_WORK* wk, u8 type )
{
	wk->item_list_flag = 1;

	BmpTalkWinClearSub( &wk->bmpwin[MINE_BMPWIN_TALK] );

	//ここはすぐに消す
	GF_BGL_BmpWinOff( &wk->bmpwin[MINE_BMPWIN_TALK] );
	GF_BGL_BmpWinOff( &wk->bmpwin[MINE_BMPWIN_BASIC_LIST] );

	Castle_SetItemBgGraphic( wk, BC_FRAME_SLIDE );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
	GF_BGL_BmpWinOffVReq( &wk->bmpwin[MINE_BMPWIN_STATUS] );
	
	wk->list_csr_pos = 0;
	Castle_ItemListMake( wk, type );
	Castle_NameCPWinWrite( wk, &wk->bmpwin[MINE_BMPWIN_CP] );
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * アイテムきのみ項目状態を削除
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CastleMine_ItemSeed_Del( CASTLE_MINE_WORK* wk )
{
	if( wk->item_list_flag == 1 ){
		wk->item_list_flag = 0;
		GF_BGL_BmpWinOffVReq( &wk->bmpwin[MINE_BMPWIN_ITEMINFO] );
		CastleMine_SeqSubListEnd( wk );
		CastleMine_SeqSubItemListEnd( wk );				//リスト終了
	}
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
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 * @param	win		GF_BGL_BMPWIN型のポインタ
 * @param	y_max	項目最大数
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void CastleInitMenu( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, u8 y_max )
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
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 * @param	no		セットする場所(戻り値)
 * @param	param	戻り値
 * @param	msg_id	メッセージID
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void CastleSetMenuData( CASTLE_MINE_WORK* wk, u8 no, u8 param, int msg_id )
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
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_SetMenu2( CASTLE_MINE_WORK* wk )
{
	CastleWriteMenuWin( wk->bgl, &wk->bmpwin[MINE_BMPWIN_YESNO] );
	CastleInitMenu( wk, &wk->bmpwin[MINE_BMPWIN_YESNO], 2 );
	CastleSetMenuData( wk, 0, 0, msg_castle_poke_02_01 );				//はい
	CastleSetMenuData( wk, 1, 1, msg_castle_poke_02_02 );				//いいえ
	wk->mw = BmpMenuAddEx( &wk->MenuH, 8, 0, 0, HEAPID_CASTLE, PAD_BUTTON_CANCEL );
	wk->menu_flag = 1;
	return;
}


//==============================================================================================
//
//	リスト関連
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	アイテムリスト作成
 *
 * @param	wk			CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_ItemListMake( CASTLE_MINE_WORK* wk, u8 decide_type )
{
	u8 now_rank,list_max,poke_pos;
	u16 item;
	int i;
	MSGDATA_MANAGER* msgman_itemname;			//メッセージマネージャー

	//ポケモンアイコンの位置を変更
	CastleMine_ItemWinPokeIconMove( wk, wk->csr_pos, 1 );

	//HP表示、LV表示
	poke_pos = GetCsrPokePos( wk->h_max, wk->csr_pos );
	Castle_PokeHpMsgWriteSub( wk, &wk->bmpwin[MINE_BMPWIN_ITEM_HP], poke_pos, 1 );
	GF_BGL_BmpWinOnVReq( &wk->bmpwin[MINE_BMPWIN_ITEM_HP] );
	Castle_PokeLvMsgWriteSub( wk, &wk->bmpwin[MINE_BMPWIN_ITEM_LV], poke_pos, 1 );
	GF_BGL_BmpWinOnVReq( &wk->bmpwin[MINE_BMPWIN_ITEM_LV] );

	CastleObj_Vanish( wk->p_itemicon, CASTLE_VANISH_OFF );				//表示
	CastleObj_Vanish( wk->p_scr_u, CASTLE_VANISH_OFF );					//表示
	CastleObj_Vanish( wk->p_scr_d, CASTLE_VANISH_OFF );					//表示
	CastleObj_Vanish( wk->p_item_csr, CASTLE_VANISH_OFF );				//表示

	msgman_itemname = MSGMAN_Create(MSGMAN_TYPE_DIRECT, ARC_MSG, 
								NARC_msg_itemname_dat, HEAPID_CASTLE );

	now_rank = Castle_GetRank( wk->sv, wk->type, CASTLE_RANK_TYPE_RENTAL );

	if( decide_type == FC_PARAM_KINOMI ){
		list_max = castle_seed_rank[now_rank-1];
	}else{
		list_max = castle_item_rank[now_rank-1];
	}
	wk->menulist = BMP_MENULIST_Create( (list_max+1), HEAPID_CASTLE );

	for( i=0; i < list_max; i++ ){

		if( decide_type == FC_PARAM_KINOMI ){
			item = castle_seed[i];
		}else{
			item = castle_item[i];
		}
		BMP_MENULIST_AddArchiveString(	wk->menulist, msgman_itemname, item, i );
	}

	//「やめる」＊メッセージがない
	BMP_MENULIST_AddArchiveString(	wk->menulist, wk->msgman, 
									msg_castle_poke_choice_05, BMPLIST_CANCEL );


	wk->list_h			= CastleListH;
	wk->list_h.list		= wk->menulist;
	wk->list_h.win		= &wk->bmpwin[MINE_BMPWIN_LIST];
	wk->list_h.work		= wk;
	wk->list_h.call_back= Castle_CsrMoveCallBack;				//カーソル移動ごとのコールバック関数
	wk->list_h.icon		= Castle_LineWriteCallBack;				//一列表示ごとのコールバック関数
	wk->list_h.count	= (list_max + 1);						//リスト項目数
	wk->list_h.b_col	= FBMP_COL_NULL;						//背景色
	wk->list_h.line		= ( 6 );								//表示最大項目数
	wk->list_h.data_x	= ( 0 );								//項目表示Ｘ座標
	wk->list_h.c_disp_f = 1;									//カーソル(allow)(0:ON,1:OFF)
	//wk->list_page_skip	= BMPLIST_LRKEY_SKIP;				//ページスキップタイプ(アイテムのみ)

	//★この中でGF_BGL_BmpWinOnが呼ばれている
	wk->lw			= BmpListSet( &wk->list_h, 0, 0, HEAPID_CASTLE );

	CastleMine_SeqSubListStart( wk, &wk->bmpwin[MINE_BMPWIN_LIST] );
	//GF_BGL_BmpWinOnVReq( &wk->bmpwin[MINE_BMPWIN_LIST] );

	MSGMAN_Delete( msgman_itemname );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	カーソル移動ごとのコールバック
 *
 * @param	wk			CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_CsrMoveCallBack( BMPLIST_WORK* work, u32 param, u8 mode )
{
	u32 count,line;
	u16 item,list_bak,cursor_bak,pos_bak;
	CASTLE_MINE_WORK* wk = (CASTLE_MINE_WORK*)BmpListParamGet( work, BMPLIST_ID_WORK );

	//初期化時は鳴らさない
	if( mode == 0 ){
		Snd_SePlay( SEQ_SE_DP_SELECT );
		BmpListDirectPosGet( wk->lw, &pos_bak );
	}else{
		pos_bak = 0;	//初期化時は値がおかしい(というかどこかで初期値セットしている？)
	}

	count = BmpListParamGet( work, BMPLIST_ID_COUNT );							//リスト項目数
	line  = BmpListParamGet( work, BMPLIST_ID_LINE );							//表示最大項目数
	BmpListPosGet( work, &list_bak, &cursor_bak );
	if( list_bak == 0 ){
		CastleObj_Vanish( wk->p_scr_u, CASTLE_VANISH_ON );						//非表示
		CastleObj_Vanish( wk->p_scr_d, CASTLE_VANISH_OFF );						//表示
	}else if( list_bak == (count-line) ){
		CastleObj_Vanish( wk->p_scr_u, CASTLE_VANISH_OFF );						//表示
		CastleObj_Vanish( wk->p_scr_d, CASTLE_VANISH_ON );						//非表示
	}else{
		CastleObj_Vanish( wk->p_scr_u, CASTLE_VANISH_OFF );						//表示
		CastleObj_Vanish( wk->p_scr_d, CASTLE_VANISH_OFF );						//表示
	}

	//カーソル位置を調整
	//CastleObj_SetObjPos( wk->p_item_csr, 160, (24 + cursor_bak * 16) );
	CastleObj_SetObjPos( wk->p_item_csr, 158, (24 + cursor_bak * 16) );
	//OS_Printf( "list_bak = %d\n", list_bak );
	//OS_Printf( "cursor_bak = %d\n", cursor_bak );

	//OS_Printf( "pos_bak = %d\n", pos_bak );
	if( param != BMPLIST_CANCEL ){

		//道具情報更新
		CastleItemInfoWriteMsg( wk, &wk->bmpwin[MINE_BMPWIN_ITEMINFO], 
								GetItemNo(wk,pos_bak,wk->parent_decide_type) );
		//GF_BGL_BmpWinOnVReq( &wk->bmpwin[MINE_BMPWIN_ITEMINFO] );

		//アイテムアイコン(CHAR、PLTT変更)
		CastleClact_ItemIconCharChange( &wk->castle_clact, 
										GetItemNo(wk,pos_bak,wk->parent_decide_type) );
		CastleClact_ItemIconPlttChange( &wk->castle_clact, 
										GetItemNo(wk,pos_bak,wk->parent_decide_type) );
	}else{
		//もどる
		CastleClact_ItemIconCharChange( &wk->castle_clact, ITEM_RETURN_ID );
		CastleClact_ItemIconPlttChange( &wk->castle_clact, ITEM_RETURN_ID );
		GF_BGL_BmpWinDataFill( &wk->bmpwin[MINE_BMPWIN_ITEMINFO], FBMP_COL_NULL );		//塗りつぶし
		GF_BGL_BmpWinOnVReq( &wk->bmpwin[MINE_BMPWIN_ITEMINFO] );
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	一列表示ごとのコールバック
 *
 * @param	wk			CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_LineWriteCallBack( BMPLIST_WORK* work, u32 param, u8 y )
{
	CASTLE_MINE_WORK* wk = (CASTLE_MINE_WORK*)BmpListParamGet( work, BMPLIST_ID_WORK );

	if( param != BMPLIST_CANCEL ){

		///< 値段などの表示部分
		//price = 100;
		//expb  = STRBUF_Create( 16, HEAPID_CASTLE );
		//strb  = MSGMAN_AllocString( wk->msgman, msg_castle_poke_cp_03 );
		//WORDSET_ExpandStr( wk->wset, expb, strb );
		//siz = FontProc_GetPrintStrWidth( FONT_SYSTEM, expb, 0 );
		//GF_STR_PrintColor(
		//	&wk->win[WIN_IDX_LIST], FONT_SYSTEM, expb,
		//	LIST_PRICE_EX-siz, y, MSG_NO_PUT, SHOPCOL_N_BLACK, NULL );

		//ここは、for文でPARAMをセットしているのでこれでよい
		Castle_SetNumber(	wk, 0, GetItemCP(wk,param,wk->parent_decide_type), 
							CASTLE_KETA_CP, NUMBER_DISPTYPE_SPACE );
        // MatchComment: use CastleWriteMsgSimple_Full_ov107_2243890 instead of CastleWriteMsgSimple
        // has additional arg of 2
		wk->msg_index = CastleWriteMsgSimple_Full_ov107_2243890(	wk, &wk->bmpwin[MINE_BMPWIN_LIST], 
												msg_castle_poke_cp_03, 
												CASTLE_LIST_ITEM_CP_X, y , MSG_NO_PUT, 
												FBMP_COL_BLACK, FBMP_COL_BLK_SDW, FBMP_COL_NULL, 
												BC_FONT, 2 );
		GF_BGL_BmpWinOnVReq( &wk->bmpwin[MINE_BMPWIN_LIST] );

		//STRBUF_Delete( strb );
		//STRBUF_Delete( expb );
	}

	return;
}

//==============================================================================================
//
//	「かいふく」を選んだ時に表示するリスト
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	回復を選んだ時に表示するリスト作成
 *
 * @param	wk			CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_KaihukuListMake( CASTLE_MINE_WORK* wk )
{
	u8 now_rank,list_max;
	u16 item;
	int i;

	//メニューウィンドウを描画
	CastleWriteMenuWin( wk->bgl, &wk->bmpwin[MINE_BMPWIN_KAIHUKU_LIST] );
	GF_BGL_BmpWinDataFill( &wk->bmpwin[MINE_BMPWIN_KAIHUKU_LIST], FBMP_COL_WHITE );	//塗りつぶし

	wk->menulist = BMP_MENULIST_Create( KAIHUKU_MSG_TBL_MAX, HEAPID_CASTLE );

	for( i=0; i < KAIHUKU_MSG_TBL_MAX; i++ ){
		BMP_MENULIST_AddArchiveString(	wk->menulist, wk->msgman, 
										kaihuku_msg_tbl[i][1], kaihuku_msg_tbl[i][2] );
	}

	wk->list_h			= CastleListH;
	wk->list_h.list		= wk->menulist;
	wk->list_h.win		= &wk->bmpwin[MINE_BMPWIN_KAIHUKU_LIST];
	wk->list_h.work		= wk;
	wk->list_h.call_back= Castle_KaihukuCsrMoveCallBack;		//カーソル移動ごとのコールバック関数
	wk->list_h.icon		= Castle_KaihukuLineWriteCallBack;		//一列表示ごとのコールバック関数
	wk->list_h.count	= KAIHUKU_MSG_TBL_MAX;					//リスト項目数
	wk->list_h.line		= KAIHUKU_MSG_TBL_MAX;					//表示最大項目数
	wk->list_h.b_col	= FBMP_COL_WHITE;						//背景色

	//★この中でGF_BGL_BmpWinOnが呼ばれている
	wk->lw			= BmpListSet( &wk->list_h, 0, 0, HEAPID_CASTLE );
	//GF_BGL_BmpWinOnVReq( &wk->bmpwin[MINE_BMPWIN_KAIHUKU_LIST] );

	CastleMine_SeqSubListStart( wk, &wk->bmpwin[MINE_BMPWIN_KAIHUKU_LIST] );

	return;
}

//--------------------------------------------------------------
/**
 * @brief	カーソル移動ごとのコールバック
 *
 * @param	wk			CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_KaihukuCsrMoveCallBack( BMPLIST_WORK* work, u32 param, u8 mode )
{
	u8 now_rank;
	u16 msg_id;
	CASTLE_MINE_WORK* wk = (CASTLE_MINE_WORK*)BmpListParamGet( work, BMPLIST_ID_WORK );

	//初期化時は鳴らさない
	if( mode == 0 ){
		Snd_SePlay( SEQ_SE_DP_SELECT );
	}

	now_rank = Castle_GetRank( wk->sv, wk->type, CASTLE_RANK_TYPE_KAIHUKU );

	switch( param ){
	case FC_PARAM_KAIHUKU_RANKUP:
		//下のウィンドウに表示するメッセージがランクによって変わる
		if( now_rank == CASTLE_RANK_MAX ){
			msg_id = msg_castle_poke_23;
		}else if( now_rank == 1 ){
			msg_id = msg_castle_poke_21;
		}else{
			msg_id = msg_castle_poke_22;
		}
		break;
	case BMPLIST_CANCEL:
		msg_id = msg_castle_poke_20;
		break;
	default:
		msg_id = msg_castle_poke_13;
		break;
	};

	CastleWriteMsg( wk, &wk->bmpwin[MINE_BMPWIN_TALK], 
					msg_id, 1, 1, MSG_NO_PUT, 
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_WHITE, FONT_TALK );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	一列表示ごとのコールバック
 *
 * @param	wk			CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_KaihukuLineWriteCallBack( BMPLIST_WORK* work, u32 param, u8 y )
{
	u16 pos_bak;
	u8 now_rank,f_col;
	CASTLE_MINE_WORK* wk = (CASTLE_MINE_WORK*)BmpListParamGet( work, BMPLIST_ID_WORK );

	BmpListDirectPosGet( work, &pos_bak );				//カーソル座標取得
	now_rank = Castle_GetRank( wk->sv, wk->type, CASTLE_RANK_TYPE_KAIHUKU );

	switch( param ){
	case FC_PARAM_HP_KAIHUKU:			//ＨＰかいふく
	case FC_PARAM_PP_KAIHUKU:			//ＰＰかいふく
	case FC_PARAM_HPPP_KAIHUKU:			//すべてかいふく
		//ランクを見て表示色を変える
		if( now_rank >= kaihuku_msg_tbl[param-FC_PARAM_LIST_START_KAIHUKU][0] ){
			f_col = FBMP_COL_BLACK;
		}else{
			f_col = FBMP_COL_BLK_SDW;
		}
		break;

	case FC_PARAM_KAIHUKU_RANKUP:		//ランクアップ
		//ランクを見て表示色を変える
		if( now_rank == CASTLE_RANK_MAX ){
			f_col = FBMP_COL_BLK_SDW;
		}else{
			f_col = FBMP_COL_BLACK;
		}
		break;

	default:
		f_col = FBMP_COL_BLACK;
		break;
	};

	BmpListColorControl( work, f_col, FBMP_COL_WHITE, FBMP_COL_BLK_SDW );
	return;
}


//==============================================================================================
//
//	「レンタル」を選んだ時に表示するリスト
//
//==============================================================================================
static const u32 rental_msg_tbl[][3] = {
	//ランク、msg_id、リストパラメータ
	{ 1, msg_castle_rental_01,	FC_PARAM_KINOMI },			//きのみ
	{ 2, msg_castle_rental_02,	FC_PARAM_ITEM },			//どうぐ
	{ 1, msg_castle_rental_03,	FC_PARAM_RENTAL_RANKUP },	//ランクアップ
	{ 1, msg_castle_rental_04,	BMPLIST_CANCEL },			//やめる
};
#define RENTAL_MSG_TBL_MAX		( NELEMS(rental_msg_tbl) )

//--------------------------------------------------------------
/**
 * @brief	レンタルを選んだ時に表示するリスト作成
 *
 * @param	wk			CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_RentalListMake( CASTLE_MINE_WORK* wk )
{
	u8 now_rank,list_max;
	u16 item;
	int i;

	//指定値でクリアされたキャラをセット
	//GF_BGL_CharFill(	wk->bgl, BC_FRAME_TYPE, 0, 
	//					(MINE_WIN_RENTAL_LIST_SX * MINE_WIN_RENTAL_LIST_SY),
	//					MINE_WIN_RENTAL_LIST_CGX );

	GF_BGL_VisibleSet( BC_FRAME_TYPE, VISIBLE_OFF );

	//メニューウィンドウを描画
	CastleWriteMenuWin( wk->bgl, &wk->bmpwin[MINE_BMPWIN_RENTAL_LIST] );
	GF_BGL_BmpWinDataFill( &wk->bmpwin[MINE_BMPWIN_RENTAL_LIST], FBMP_COL_WHITE );	//塗りつぶし

	wk->menulist = BMP_MENULIST_Create( RENTAL_MSG_TBL_MAX, HEAPID_CASTLE );

	for( i=0; i < RENTAL_MSG_TBL_MAX; i++ ){
		BMP_MENULIST_AddArchiveString(	wk->menulist, wk->msgman, 
										rental_msg_tbl[i][1], rental_msg_tbl[i][2] );
	}

	wk->list_h			= CastleListH;
	wk->list_h.list		= wk->menulist;
	wk->list_h.win		= &wk->bmpwin[MINE_BMPWIN_RENTAL_LIST];
	wk->list_h.work		= wk;
	wk->list_h.call_back= Castle_RentalCsrMoveCallBack;			//カーソル移動ごとのコールバック関数
	wk->list_h.icon		= Castle_RentalLineWriteCallBack;		//一列表示ごとのコールバック関数
	wk->list_h.count	= RENTAL_MSG_TBL_MAX;					//リスト項目数
	wk->list_h.line		= RENTAL_MSG_TBL_MAX;					//表示最大項目数
	wk->list_h.b_col	= FBMP_COL_WHITE;						//背景色

	//★この中でGF_BGL_BmpWinOnが呼ばれている
	wk->lw			= BmpListSet( &wk->list_h, 0, 0, HEAPID_CASTLE );

	CastleMine_SeqSubListStart( wk, &wk->bmpwin[MINE_BMPWIN_RENTAL_LIST] );

	GF_BGL_BmpWinOnVReq( &wk->bmpwin[MINE_BMPWIN_RENTAL_LIST] );

	GF_BGL_VisibleSet( BC_FRAME_TYPE, VISIBLE_ON );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	カーソル移動ごとのコールバック
 *
 * @param	wk			CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_RentalCsrMoveCallBack( BMPLIST_WORK* work, u32 param, u8 mode )
{
	u16 msg_id;
	u8 now_rank;
	CASTLE_MINE_WORK* wk = (CASTLE_MINE_WORK*)BmpListParamGet( work, BMPLIST_ID_WORK );

	//初期化時は鳴らさない
	if( mode == 0 ){
		Snd_SePlay( SEQ_SE_DP_SELECT );
	}

	now_rank = Castle_GetRank( wk->sv, wk->type, CASTLE_RANK_TYPE_RENTAL );

	switch( param ){
	case FC_PARAM_KINOMI:
		msg_id = msg_castle_poke_41;
		break;
	case FC_PARAM_ITEM:
		msg_id = msg_castle_poke_42;
		break;
	case FC_PARAM_RENTAL_RANKUP:
		//下のウィンドウに表示するメッセージがランクによって変わる
		if( now_rank == CASTLE_RANK_MAX ){
			msg_id = msg_castle_poke_32;
		}else if( now_rank == 1 ){
			msg_id = msg_castle_poke_30;
		}else{
			msg_id = msg_castle_poke_31;
		}
		break;
	default:
		msg_id = msg_castle_poke_33;
		break;
	};

	CastleWriteMsg( wk, &wk->bmpwin[MINE_BMPWIN_TALK], 
					msg_id, 1, 1, MSG_NO_PUT, 
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_WHITE, FONT_TALK );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	一列表示ごとのコールバック
 *
 * @param	wk			CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_RentalLineWriteCallBack( BMPLIST_WORK* work, u32 param, u8 y )
{
	u8 now_rank,f_col;
	CASTLE_MINE_WORK* wk = (CASTLE_MINE_WORK*)BmpListParamGet( work, BMPLIST_ID_WORK );

	now_rank = Castle_GetRank( wk->sv, wk->type, CASTLE_RANK_TYPE_RENTAL );

	switch( param ){

	case FC_PARAM_ITEM:
		//ランクを見て表示色を変える
		//if( now_rank >= rental_msg_tbl[param][0] ){
		if( now_rank >= rental_msg_tbl[param-FC_PARAM_LIST_START_RENTAL][0] ){
			f_col = FBMP_COL_BLACK;
		}else{
			f_col = FBMP_COL_BLK_SDW;
		}
		break;

	case FC_PARAM_RENTAL_RANKUP:
		//ランクを見て表示色を変える
		if( now_rank == CASTLE_RANK_MAX ){
			f_col = FBMP_COL_BLK_SDW;
		}else{
			f_col = FBMP_COL_BLACK;
		}
		break;
	
	//case FC_PARAM_KINOMI:
	default:
		f_col = FBMP_COL_BLACK;
		break;
	};

	BmpListColorControl( work, f_col, FBMP_COL_WHITE, FBMP_COL_BLK_SDW );
	return;
}


//==============================================================================================
//
//	基本リスト関連(かいふく、レンタル、つよさ、わざ、とじる)
//
//==============================================================================================
#define CASTLE_MINE_BASIC_LIST_MAX	(5)

static const u32 basic_msg_tbl[][2] = {
	{ msg_castle_poke_choice_01,	FC_PARAM_KAIHUKU},
	{ msg_castle_poke_choice_02,	FC_PARAM_RENTAL	},
	{ msg_castle_poke_choice_03,	FC_PARAM_TUYOSA	},
	{ msg_castle_poke_choice_04,	FC_PARAM_WAZA	},
	{ msg_castle_poke_choice_05,	BMPLIST_CANCEL	},
};

static const u16 basic_under_msg_tbl[] = {
	msg_castle_poke_00_03,
	msg_castle_poke_00_04,
	msg_castle_poke_00_05,
	msg_castle_poke_00_06,
	msg_castle_poke_00_07,
};

//--------------------------------------------------------------
/**
 * @brief	基本メニューを表示するリスト作成
 *
 * @param	wk			CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_BasicListMake( CASTLE_MINE_WORK* wk )
{
	int i;

	//メニューウィンドウを描画
	CastleWriteMenuWin( wk->bgl, &wk->bmpwin[MINE_BMPWIN_BASIC_LIST] );
	GF_BGL_BmpWinDataFill( &wk->bmpwin[MINE_BMPWIN_BASIC_LIST], FBMP_COL_WHITE );	//塗りつぶし

	wk->menulist = BMP_MENULIST_Create( CASTLE_MINE_BASIC_LIST_MAX, HEAPID_CASTLE );

	for( i=0; i < CASTLE_MINE_BASIC_LIST_MAX; i++ ){
		BMP_MENULIST_AddArchiveString(	wk->menulist, wk->msgman, 
										basic_msg_tbl[i][0], basic_msg_tbl[i][1] );
	}

	wk->list_h			= CastleListH;
	wk->list_h.list		= wk->menulist;
	wk->list_h.win		= &wk->bmpwin[MINE_BMPWIN_BASIC_LIST];
	wk->list_h.work		= wk;
	wk->list_h.call_back= Castle_BasicCsrMoveCallBack;			//カーソル移動ごとのコールバック関数
	wk->list_h.icon		= NULL;									//一列表示ごとのコールバック関数
	wk->list_h.count	= CASTLE_MINE_BASIC_LIST_MAX;			//リスト項目数
	wk->list_h.line		= CASTLE_MINE_BASIC_LIST_MAX;			//表示最大項目数
	wk->list_h.b_col	= FBMP_COL_WHITE;						//背景色
	wk->list_h.work		= wk;

	//★この中でGF_BGL_BmpWinOnが呼ばれている
	wk->lw			= BmpListSet( &wk->list_h, wk->basic_list_lp, wk->basic_list_cp, HEAPID_CASTLE);

	CastleMine_SeqSubListStart( wk, &wk->bmpwin[MINE_BMPWIN_BASIC_LIST] );

	GF_BGL_BmpWinOnVReq( &wk->bmpwin[MINE_BMPWIN_BASIC_LIST] );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	カーソル移動ごとのコールバック
 *
 * @param	wk			CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_BasicCsrMoveCallBack( BMPLIST_WORK* work, u32 param, u8 mode )
{
	CASTLE_MINE_WORK* wk;
	u16 pos_bak;

	wk = (CASTLE_MINE_WORK*)BmpListParamGet( work, BMPLIST_ID_WORK );

	//初期化時は鳴らさない
	if( mode == 0 ){
		Snd_SePlay( SEQ_SE_DP_SELECT );
	}

	//BmpListDirectPosGet( work, &wk->cursor_bak );		//カーソル座標取得
	BmpListDirectPosGet( work, &pos_bak );				//カーソル座標取得

	CastleWriteMsg( wk, &wk->bmpwin[MINE_BMPWIN_TALKMENU2], 
					basic_under_msg_tbl[pos_bak], 1, 1, MSG_NO_PUT, 
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_WHITE, FONT_TALK );
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
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 * @param	bufID	バッファID
 * @param	number	セットする数値
 *
 * @retval	none
 *
 * 桁を固定にしている
 */
//--------------------------------------------------------------
static void Castle_SetNumber( CASTLE_MINE_WORK* wk, u32 bufID, s32 number, u32 keta, NUMBER_DISPTYPE disp )
{
	WORDSET_RegisterNumber( wk->wordset, bufID, number, keta, disp, NUMBER_CODETYPE_DEFAULT );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	ポケモン名をセット
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 * @param	bufID	バッファID
 * @param	ppp		POKEMON_PASO_PARAM
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void Castle_SetPokeName( CASTLE_MINE_WORK* wk, u32 bufID, POKEMON_PASO_PARAM* ppp )
{
	WORDSET_RegisterPokeMonsName( wk->wordset, bufID, ppp );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	プレイヤー名をセット
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 * @param	bufID	バッファID
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void Castle_SetPlayerName( CASTLE_MINE_WORK* wk, u32 bufID )
{
	WORDSET_RegisterPlayerName( wk->wordset, bufID, SaveData_GetMyStatus(wk->sv) );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	プレイヤー名を表示
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
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
static void PlayerNameWrite( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 font )
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

	GF_STR_PrintColor( win, font, player_buf, x, y, MSG_NO_PUT, col, NULL );
	GF_BGL_BmpWinOnVReq( win );

	STRBUF_Delete( player_buf );					//メッセージバッファ開放
	return;
}

//--------------------------------------------------------------
/**
 * @brief	パートナー名を表示
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
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
static void PairNameWrite( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 font )
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

	CastleWriteMsgSimple(	wk, win, msg_castle_poke_name_02, 
							x, y, MSG_NO_PUT, 
							GF_PRINTCOLOR_GET_LETTER(col),
							GF_PRINTCOLOR_GET_SHADOW(col),
							GF_PRINTCOLOR_GET_GROUND(col),
							//FBMP_COL_NULL,
							BC_FONT );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	ポケモン名を表示
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
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
static void PokeNameWrite( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 f_col, u8 s_col, u8 b_col, u8 font )
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

/*
	//右端に性別コードを表示
	x_pos = GF_BGL_BmpWinGet_SizeX(win) - 1;
	sex = PokeParaGet( poke, ID_PARA_sex, NULL );

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
 * @brief	ポケモン名を表示(性別指定)
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
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
static void PokeNameWriteEx( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 f_col, u8 s_col, u8 b_col, u8 font, u16 monsno, u8 sex )
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
 * @param	wk		CASTLE_MINE_WORK型のポインタ
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
static void PokeSexWrite( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 font, u8 sex )
{
	u32 msg_id,col;
	u8 f_col,s_col,b_col;

	if( sex == PARA_MALE ){
		msg_id = msg_castle_poke_status_10_01;
		f_col = FBMP_COL_BLUE;
		s_col = FBMP_COL_BLU_SDW;
		b_col = FBMP_COL_NULL;
	}else if( sex == PARA_FEMALE ){
		msg_id = msg_castle_poke_status_10_02;
		f_col = FBMP_COL_RED;
		s_col = FBMP_COL_RED_SDW;
		b_col = FBMP_COL_NULL;
	}else{
		return;	//性別なし
	}

	CastleWriteMsgSimple(	wk, win, msg_id, 
							x, y, MSG_NO_PUT, 
							f_col, s_col, b_col, font );

	//GF_BGL_BmpWinOnVReq( win );
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
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 * @param	type	チェックするタイプ
 *
 * @return	"TRUE = typeが同じ、FALSE = typeが違う"
 */
//--------------------------------------------------------------
static BOOL Castle_CheckType( CASTLE_MINE_WORK* wk, u8 type )
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
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 * @param	seq		シーケンスのポインタ
 * @param	next	次のシーケンス定義
 *
 * @return	none
 */
//--------------------------------------------------------------
static void NextSeq( CASTLE_MINE_WORK* wk, int* seq, int next )
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
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void BgCheck( CASTLE_MINE_WORK* wk )
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
static void CsrMove( CASTLE_MINE_WORK* wk, int key )
{
	u8 poke_pos;
	int flag;

	flag = 0;
	poke_pos = GetCsrPokePos( wk->h_max, wk->csr_pos );

	///////////////////////////////////////////////////////////////////////////////////////////
	if( sys.trg & PAD_KEY_LEFT ){

		//「もどる」選択中
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

		//「もどる」選択中
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

static void CsrMoveSub( CASTLE_MINE_WORK* wk )
{
	Snd_SePlay( SEQ_SE_DP_SELECT );

	//通信タイプの時は、カーソル位置を送信
	if( Castle_CommCheck(wk->type) == TRUE ){
		CastleMine_CommSetSendBuf( wk, CASTLE_COMM_MINE_CSR_POS, wk->csr_pos );
	}

	//ポケモン選択ウィンドウ
	//PokeSelMoveSub( wk, wk->csr_pos, CommGetCurrentID() );
	PokeSelMoveSub( wk, wk->csr_pos, 0 );
	return;
}

//ポケモン選択ウィンドウ
static void PokeSelMoveSub( CASTLE_MINE_WORK* wk, u8 csr_pos, u8 flag )
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
		CastleObj_SetObjPos( obj, MINE_CSR_MODORU_X, MINE_CSR_MODORU_Y );
	}else{
		CastleObj_AnmChg( obj, anm_no );
		CastleMine_GetPokeSelXY( wk, &start_x, &start_y, csr_pos );
		CastleObj_SetObjPos( obj, start_x, start_y );
	}

	return;
}

//ポケモン選択ウィンドウオフセット取得
static void CastleMine_GetPokeSelXY( CASTLE_MINE_WORK* wk, u32* x, u32* y, u8 csr_pos )
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
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	"X"
 */
//--------------------------------------------------------------
static u16 GetCsrX( CASTLE_MINE_WORK* wk, u8 csr_pos, u8 lr )
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
		ret = (MINE_CSR_MODORU_X + lr_offset_x);
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
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	"Y"
 */
//--------------------------------------------------------------
static u16 GetCsrY( CASTLE_MINE_WORK* wk, u8 csr_pos )
{
	u16 ret;

	if( csr_pos >= wk->modoru_pos ){
		return MINE_CSR_MODORU_Y;
	}

	ret = CSR_START_Y;
	return ret;
}

//--------------------------------------------------------------
/**
 * @brief	アイテムＣＰ取得
 *
 * @param	csr_pos		カーソル位置
 *
 * @return	"ＣＰ"
 */
//--------------------------------------------------------------
static u16 GetItemCP( CASTLE_MINE_WORK* wk, u16 csr_pos, u8 type )
{
	u8 now_rank,list_max,item_max,seed_max;

	now_rank = Castle_GetRank( wk->sv, wk->type, CASTLE_RANK_TYPE_RENTAL );
	item_max = castle_item_rank[now_rank-1];
	seed_max = castle_seed_rank[now_rank-1];

	if( type == FC_PARAM_KINOMI ){
		return castle_seed_cp[csr_pos];
	}

	return castle_item_cp[csr_pos];
}

//--------------------------------------------------------------
/**
 * @brief	アイテムＣＰ取得(アイテムナンバーから取得)
 *
 * @param	item		アイテムナンバー
 *
 * @return	"ＣＰ"
 */
//--------------------------------------------------------------
static u16 GetItemCPByItemNo( u16 item )
{
	int i;

	for( i=0; i < CASTLE_ITEM_MAX ;i++ ){
		if( castle_item[i] == item ){
			return castle_item_cp[i];
		}
	}

	for( i=0; i < CASTLE_SEED_MAX ;i++ ){
		if( castle_seed[i] == item ){
			return castle_seed_cp[i];
		}
	}

	GF_ASSERT( 0 );
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	アイテムナンバー取得
 *
 * @param	csr_pos		カーソル位置
 *
 * @return	"アイテムナンバー"
 */
//--------------------------------------------------------------
static u16 GetItemNo( CASTLE_MINE_WORK* wk, u16 csr_pos, u8 type )
{
	u8 now_rank,list_max,item_max,seed_max;

	now_rank = Castle_GetRank( wk->sv, wk->type, CASTLE_RANK_TYPE_RENTAL );
	item_max = castle_item_rank[now_rank-1];
	seed_max = castle_seed_rank[now_rank-1];

	if( type == FC_PARAM_KINOMI ){
		return castle_seed[csr_pos];
	}

	return castle_item[csr_pos];
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
static void Castle_GetOffset( CASTLE_MINE_WORK* wk, u16* offset_x, u16* offset_y, u16* pair_offset_x, u16* pair_offset_y )
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
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	"TRUE=終了、FALSE=継続"
 */
//--------------------------------------------------------------
static BOOL CastleMine_DecideEff( CASTLE_MINE_WORK* wk, u8 csr_pos, u8 sel_type )
{
	u32 hp,hp_max,anm_no,hp_anm_no;
	u16 eff_offset_x;
	u8 poke_pos;
	POKEMON_PARAM* poke;

	poke_pos	= GetCsrPokePos( wk->h_max, csr_pos );
	poke		= PokeParty_GetMemberPointer( wk->p_party, poke_pos );
	hp			= PokeParaGet( poke, ID_PARA_hp, NULL );
	hp_max		= PokeParaGet( poke, ID_PARA_hpmax, NULL );
	anm_no		= CastleMine_GetPokeAnmNo( wk, GetHPGaugeDottoColor(hp,hp_max,48) );	//POKEアニメ
	hp_anm_no	= CastleMine_GetHpAnmNo( wk, GetHPGaugeDottoColor(hp,hp_max,48) );		//HPアニメ

	switch( sel_type ){

	//「ランクアップ」
	case FC_PARAM_KAIHUKU_RANKUP:		//(回復)ランクアップ
	case FC_PARAM_RENTAL_RANKUP:		//(レンタル)ランクアップ
		wk->eff_init_flag = 0;
		return TRUE;
		break;

	//「かいふく」
	//case SEL_KAIHUKU:
	case FC_PARAM_HP_KAIHUKU:			//ＨＰかいふく
	case FC_PARAM_PP_KAIHUKU:			//ＰＰかいふく
	case FC_PARAM_HPPP_KAIHUKU:			//すべてかいふく

		//初期設定
		if( wk->eff_init_flag == 0 ){
			wk->eff_init_flag = 1;

			if( Castle_CommCheck(wk->type) == FALSE ){
				eff_offset_x		= KAIHUKU_EFF_START_X;
			}else{
				eff_offset_x		= KAIHUKU_EFF_MULTI_START_X;
			}

			//エフェクトOBJ追加
			wk->p_eff = CastleObj_Create(	&wk->castle_clact, 
											ID_CHAR_CSR, ID_PLTT_CSR, 
											ID_CELL_CSR, CASTLE_ANM_KAIHUKU_EFF, 
											eff_offset_x + (KAIHUKU_EFF_WIDTH_X * poke_pos),
											KAIHUKU_EFF_START_Y,
											CASTLE_BG_PRI_HIGH, NULL );
		}

		//アニメーション終了が終了していたら削除
		//(HPの数値が書き終わるのが長い時があるので分けている)
		if( wk->p_eff != NULL ){
			if( CastleObj_AnmActiveCheck(wk->p_eff) == FALSE ){
				CastleObj_Delete( wk->p_eff );
				wk->p_eff = NULL;
			}
		}

#if 0
		//HPの数値を書き換えていく
		if( wk->before_hp != PokeParaGet(poke,ID_PARA_hp,NULL) ){
			//OS_Printf( "before_hp = %d\n", wk->before_hp );
			//OS_Printf( "para_hp = %d\n", PokeParaGet(poke,ID_PARA_hp,NULL) );
			wk->before_hp++;
			Castle_PokeHpEff( wk, &wk->bmpwin[MINE_BMPWIN_HP], poke_pos, wk->before_hp );
		}else{
			//HP回復が終了していて、回復演出アニメが終了していたら
			if( wk->p_eff == NULL ){
				CastleObj_AnmChg( wk->p_hp[poke_pos], hp_anm_no );			//HPアニメ切り替え
				CastleObj_PokeIconAnmChg( wk->p_icon[poke_pos], anm_no );	//POKEアニメ切り替え
				wk->eff_init_flag = 0;
				return TRUE;
			}
		}
#else
		//回復演出アニメが終了していたら
		if( wk->p_eff == NULL ){
			Castle_PokeHpEff(	wk, &wk->bmpwin[MINE_BMPWIN_HP], poke_pos, 
								PokeParaGet(poke,ID_PARA_hp,NULL) );

			CastleObj_AnmChg( wk->p_hp[poke_pos], hp_anm_no );			//HPアニメ切り替え
			CastleObj_PokeIconAnmChg( wk->p_icon[poke_pos], anm_no );	//POKEアニメ切り替え
			wk->eff_init_flag = 0;
			return TRUE;
		}
#endif
		break;

	//「どうぐ」
	//case SEL_RENTAL:
	case FC_PARAM_KINOMI:				//きのみ
	case FC_PARAM_ITEM:					//どうぐ

		//初期設定
		if( wk->eff_init_flag == 0 ){
			wk->eff_init_flag = 1;

			if( Castle_CommCheck(wk->type) == FALSE ){
				eff_offset_x		= KAIHUKU_EFF_START_X;
			}else{
				eff_offset_x		= KAIHUKU_EFF_MULTI_START_X;
			}

			//エフェクトOBJ追加
			wk->p_eff = CastleObj_Create(	&wk->castle_clact, 
											ID_CHAR_CSR, ID_PLTT_CSR, 
											ID_CELL_CSR, CASTLE_ANM_ITEMGET_EFF, 
											eff_offset_x + (KAIHUKU_EFF_WIDTH_X * poke_pos),
											KAIHUKU_EFF_START_Y,
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
	//case SEL_TUYOSA:
	case FC_PARAM_TUYOSA:				//つよさ
		wk->eff_init_flag = 0;
		return TRUE;
		break;

	//「わざ」
	//case SEL_WAZA:
	case FC_PARAM_WAZA:					//わざ
		wk->eff_init_flag = 0;
		return TRUE;
		break;
	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	アイテムウィンドウの左に選択しているポケモンアイコンを移動
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 * @param	flag	1=アイテムウィンドウ位置に移動、0=元の位置に戻す
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleMine_ItemWinPokeIconMove( CASTLE_MINE_WORK* wk, u8 csr_pos, u8 flag )
{
	u8 poke_pos,pri;
	u16 icon_offset_x;
	s16 x,y,itemkeep_x,itemkeep_y;

	poke_pos = GetCsrPokePos( wk->h_max, csr_pos );

	if( Castle_CommCheck(wk->type) == FALSE ){
		icon_offset_x = ICON_START_X;
	}else{
		icon_offset_x = ICON_MULTI_START_X;
	}

	//ポケモンアイコンの位置を変更
	if( flag == 1 ){
		x = CASTLE_ITEM_POKE_ICON_X;
		y = CASTLE_ITEM_POKE_ICON_Y;
		pri = CASTLE_BG_PRI_HIGH;

		//ポケモンアイコンと、アイテム持っているアイコンを非表示
		//CastleObj_Vanish( wk->p_icon[poke_pos], CASTLE_VANISH_OFF );	//表示
		//CastleObj_Vanish( wk->p_itemkeep[poke_pos], CASTLE_VANISH_OFF );//表示
	}else{
		x = (ICON_WIDTH_X * poke_pos + icon_offset_x);
		y = ICON_START_Y;
		pri = CASTLE_BG_PRI_LOW;

		//ポケモンアイコンと、アイテム持っているアイコンを非表示
		//CastleObj_Vanish( wk->p_icon[poke_pos], CASTLE_VANISH_ON );		//非表示
		//CastleObj_Vanish( wk->p_itemkeep[poke_pos], CASTLE_VANISH_ON );	//非表示
	}

	//アイテム持っているアイコン
	itemkeep_x = ( x + CASTLE_ITEMKEEP_OFFSET_X );
	itemkeep_y = ( y + CASTLE_ITEMKEEP_OFFSET_Y );
	CastleObj_SetInitXY( wk->p_itemkeep[poke_pos], itemkeep_x, itemkeep_y );
	CastleObj_SetObjPos(wk->p_itemkeep[poke_pos], itemkeep_x, itemkeep_y );
	CastleObj_PriorityChg( wk->p_itemkeep[poke_pos], pri );

	//////////////////////////////////////////////////////////////////////////////////

	//ポケモンアニメで初期座標を使用しているので書き換える
	CastleObj_SetInitXY( wk->p_icon[poke_pos], x, y );
	CastleObj_SetObjPos(wk->p_icon[poke_pos], x, y );
	CastleObj_PriorityChg( wk->p_icon[poke_pos], pri );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	ゲージカラーからポケモンアニメナンバー取得
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	"アニメナンバー"
 */
//--------------------------------------------------------------
static u32 CastleMine_GetPokeAnmNo( CASTLE_MINE_WORK* wk, u8 gauge_color )
{
	u32 anm_no;

	switch( gauge_color ){

	case HP_DOTTO_MAX:
		anm_no = POKEICON_ANM_HPMAX;
		break;

	case HP_DOTTO_GREEN:	// 緑
		anm_no = POKEICON_ANM_HPGREEN;
		break;

	case HP_DOTTO_YELLOW:	// 黄
		anm_no = POKEICON_ANM_HPYERROW;
		break;

	case HP_DOTTO_RED:		// 赤
		anm_no = POKEICON_ANM_HPRED;
		break;
	};

	return anm_no;
}

//--------------------------------------------------------------
/**
 * @brief	ゲージカラーからHPアニメナンバー取得
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	"アニメナンバー"
 */
//--------------------------------------------------------------
static u32 CastleMine_GetHpAnmNo( CASTLE_MINE_WORK* wk, u8 gauge_color )
{
	u32 hp_anm_no;

	switch( gauge_color ){

	case HP_DOTTO_MAX:
		hp_anm_no = CASTLE_ANM_HP_MIDORI;
		break;

	case HP_DOTTO_GREEN:	// 緑
		hp_anm_no = CASTLE_ANM_HP_MIDORI;
		break;

	case HP_DOTTO_YELLOW:	// 黄
		hp_anm_no = CASTLE_ANM_HP_KIIRO;
		break;

	case HP_DOTTO_RED:		// 赤
		hp_anm_no = CASTLE_ANM_HP_AKA;
		break;
	};

	return hp_anm_no;
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
static void CastleMine_StatusWinChg( CASTLE_MINE_WORK* wk, s8 add_sub )
{
	POKEMON_PARAM* poke;
	s8 pos;

	pos	= wk->csr_pos;

	pos += add_sub;

	if( pos < 0 ){
		pos = (wk->h_max - 1);
	}else if( pos >= wk->h_max ){
		pos = 0;
	}

	//ウィンドウを切り替え
	wk->csr_pos = pos;
	CsrMoveSub( wk );
	poke =  PokeParty_GetMemberPointer( wk->p_party, GetCsrPokePos(wk->h_max,wk->csr_pos) );
	Castle_StatusMsgWrite( wk, &wk->bmpwin[MINE_BMPWIN_STATUS], poke );
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
static void CastleMine_WazaWinChg( CASTLE_MINE_WORK* wk, s8 add_sub )
{
	POKEMON_PARAM* poke;
	s8 pos;

	pos	= wk->csr_pos;

	pos += add_sub;

	if( pos < 0 ){
		pos = (wk->h_max - 1);
	}else if( pos >= wk->h_max ){
		pos = 0;
	}

	//ウィンドウを切り替え
	wk->csr_pos = pos;
	CsrMoveSub( wk );
	poke =  PokeParty_GetMemberPointer( wk->p_party, GetCsrPokePos(wk->h_max,wk->csr_pos) );
	Castle_WazaMsgWrite( wk, &wk->bmpwin[MINE_BMPWIN_STATUS], poke );
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
static void CastleMine_PairDecideDel( CASTLE_MINE_WORK* wk )
{
	CastleMine_SeqSubMenuWinClear( wk );							//メニューを表示していたら削除
	CastleMine_SeqSubListEnd( wk );
	CastleMine_ItemSeed_Del( wk );
	BmpTalkWinClearSub( &wk->bmpwin[MINE_BMPWIN_TALK] );
	CastleMine_SeqSubStatusWazaDel( wk );							//ステータス・技を削除
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );		//TEST
	CastleObj_Vanish( wk->p_eff_rankup, CASTLE_VANISH_ON );			//非表示
	return;
}

//--------------------------------------------------------------
/**
 * @brief	HP,PPの回復の必要があるかチェック
 *
 * @param	
 *
 * @return	"TRUE = 回復の必要あり、FALSE = 回復の必要なし"
 */
//--------------------------------------------------------------
static BOOL CastleMine_PPRecoverCheck( POKEMON_PARAM* poke )
{
	u8 flag;

	flag = FALSE;

	//回復の必要があるかチェック
	if( PokeParaGet(poke,ID_PARA_pp1,NULL) != PokeParaGet(poke,ID_PARA_pp_max1,NULL) ){
		flag = TRUE;
	}

	if( PokeParaGet(poke,ID_PARA_pp2,NULL) != PokeParaGet(poke,ID_PARA_pp_max2,NULL) ){
		flag = TRUE;
	}

	if( PokeParaGet(poke,ID_PARA_pp3,NULL) != PokeParaGet(poke,ID_PARA_pp_max3,NULL) ){
		flag = TRUE;
	}

	if( PokeParaGet(poke,ID_PARA_pp4,NULL) != PokeParaGet(poke,ID_PARA_pp_max4,NULL) ){
		flag = TRUE;
	}

	return flag;
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

//--------------------------------------------------------------
/**
 * @brief	メッセージウィンドウ表示
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void BmpTalkWinPutSub( CASTLE_MINE_WORK* wk )
{
	GF_BGL_BmpWinOff( &wk->bmpwin[MINE_BMPWIN_TALKMENU] );
	GF_BGL_BmpWinDataFill( &wk->bmpwin[MINE_BMPWIN_TALKMENU], FBMP_COL_WHITE );

	GF_BGL_BmpWinOff( &wk->bmpwin[MINE_BMPWIN_TALKMENU2] );
	GF_BGL_BmpWinDataFill( &wk->bmpwin[MINE_BMPWIN_TALKMENU2], FBMP_COL_WHITE );

	CastleTalkWinPut( &wk->bmpwin[MINE_BMPWIN_TALK], CONFIG_GetWindowType(wk->config) );
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
 * @param	wk			CASTLE_MINE_WORK型のポインタ
 * @param	type		送信タイプ
 *
 * @retval	none
 */
//--------------------------------------------------------------
BOOL CastleMine_CommSetSendBuf( CASTLE_MINE_WORK* wk, u16 type, u16 param )
{
	int ret,command;

	switch( type ){

	//名前
	case CASTLE_COMM_MINE_PAIR:
		command = FC_CASTLE_MINE_PAIR;
		CastleMine_CommSendBufBasicData( wk, type );
		break;

	//リクエスト
	case CASTLE_COMM_MINE_UP_TYPE:
		command = FC_CASTLE_MINE_REQ_TYPE;
		CastleMine_CommSendBufRankUpType( wk, type, param );
		break;

	//カーソル位置
	case CASTLE_COMM_MINE_CSR_POS:
		command = FC_CASTLE_MINE_CSR_POS;
		CastleMine_CommSendBufCsrPos( wk, type );
		break;

	//「戻る」
	case CASTLE_COMM_MINE_MODORU:
		command = FC_CASTLE_MINE_MODORU;
		CastleMine_CommSendBufModoru( wk );
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
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
void CastleMine_CommSendBufBasicData( CASTLE_MINE_WORK* wk, u16 type )
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
void CastleMine_CommRecvBufBasicData(int id_no,int size,void *pData,void *work)
{
	int i,num;
	CASTLE_MINE_WORK* wk = work;
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
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
void CastleMine_CommSendBufRankUpType( CASTLE_MINE_WORK* wk, u16 type, u16 param )
{
	OS_Printf( "******キャッスル****** ランクアップしたいリクエスト情報送信\n" );

	//コマンド
	wk->send_buf[0] = type;
	OS_Printf( "送信：type = %d\n", wk->send_buf[0] );

	//選択している位置
	wk->send_buf[1] = param;
	OS_Printf( "送信：csr_pos = %d\n", wk->send_buf[1] );

	//先に子の選択がきていなくて、まだ値が入っていない時は、親の決定はセットしてしまう
	if( CommGetCurrentID() == COMM_PARENT_ID ){
		if( wk->parent_decide_pos == CASTLE_MINE_DECIDE_NONE ){
			wk->parent_decide_pos	= param;
			//wk->parent_decide_lr	= (wk->csr_pos % wk->h_max);
		}
	}

	//親の決定タイプ
	wk->send_buf[2] = wk->parent_decide_pos;
	OS_Printf( "送信：parent_decide_pos = %d\n", wk->send_buf[2] );

	//パートナーが左、右で、自分、相手のどちらのランクを上げようとしているか
	//wk->send_buf[3] = (wk->csr_pos % wk->h_max);
	//OS_Printf( "送信：rankup_lr = %d\n", wk->send_buf[3] );

	//道具のアイテムナンバー
	wk->send_buf[4] = wk->parent_decide_item;
	OS_Printf( "送信：item = %d\n", wk->send_buf[4] );

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
void CastleMine_CommRecvBufRankUpType(int id_no,int size,void *pData,void *work)
{
	int i,num;
	CASTLE_MINE_WORK* wk = work;
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
	
	wk->pair_sel_pos = recv_buf[1];
	OS_Printf( "受信：wk->pair_sel_pos = %d\n", wk->pair_sel_pos );

	////////////////////////////////////////////////////////////////////////
	//親
	if( CommGetCurrentID() == COMM_PARENT_ID ){

		//親の決定がすでに決まっていたら、子の選択は無効
		if( wk->parent_decide_pos != CASTLE_MINE_DECIDE_NONE ){
			wk->pair_sel_pos = 0;
			//wk->parent_decide_lr= (wk->csr_pos % wk->h_max);
		}else{

			//親の決定が決まっていない時は、
			//親が送信する時に「子にそれでいいよ」と送信する
			//wk->parent_decide_pos	= wk->pair_sel_pos;
			
			//子の選択が採用されたことがわかるようにオフセット("modoru_pos")を加える
			wk->parent_decide_pos	= wk->pair_sel_pos + wk->modoru_pos;
			wk->parent_decide_item	= recv_buf[4];
			wk->parent_decide_type	= recv_buf[5];
		}
	////////////////////////////////////////////////////////////////////////
	//子
	}else{
		//親の決定タイプ
		wk->parent_decide_pos = recv_buf[2];
		
		//パートナーが左、右で、自分、相手のどちらのランクを上げようとしているか
		//wk->parent_decide_lr	= recv_buf[3];
		wk->parent_decide_item	= recv_buf[4];
		wk->parent_decide_type	= recv_buf[5];
	}

	OS_Printf( "受信：wk->parent_decide_pos = %d\n", wk->parent_decide_pos );
	//OS_Printf( "受信：parent_decide_lr = %d\n", wk->parent_decide_lr );
	OS_Printf( "受信：parent_decide_item = %d\n", wk->parent_decide_item );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	send_bufにカーソル位置をセット
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
void CastleMine_CommSendBufCsrPos( CASTLE_MINE_WORK* wk, u16 type )
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
void CastleMine_CommRecvBufCsrPos(int id_no,int size,void *pData,void *work)
{
	CASTLE_MINE_WORK* wk = work;
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
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
void CastleMine_CommSendBufModoru( CASTLE_MINE_WORK* wk )
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
void CastleMine_CommRecvBufModoru(int id_no,int size,void *pData,void *work)
{
	CASTLE_MINE_WORK* wk = work;
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
//	回復処理
//
//==============================================================================================
#define PLACE_CASTLE	(0)		//場所(捕獲した場所と等しいかをチェックしてなつき度を操作)

//--------------------------------------------------------------
/**
 * @brief	アイテム使用
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleMine_ItemUse( POKEMON_PARAM* pp, u16 item )
{
	StatusRecover( pp, item, 0, PLACE_CASTLE, HEAPID_CASTLE );
	return;
}


//==============================================================================================
//
//	サブシーケンス
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	プレイヤー、パートナー名表示
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleMine_SeqSubNameWrite( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win )
{
	u16 x,y,offset_x,offset_y,pair_offset_x,pair_offset_y;

	//オフセット取得
	Castle_GetOffset( wk, &offset_x, &offset_y, &pair_offset_x, &pair_offset_y );

	if( Castle_CommCheck(wk->type) == FALSE ){
		x = offset_x + INFO_PLAYER_X;
		y = offset_y + INFO_PLAYER_Y;
		PlayerNameWrite( wk, win, x, y, BC_FONT );			//プレイヤー名を表示
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
	}

	GF_BGL_BmpWinOnVReq( win );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	回復
 *
 * @param	type	FC_PARAM_??
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleMine_SeqSubKaihuku( CASTLE_MINE_WORK* wk, u8 csr_pos, u8 type )
{
	u32 now_rank;
	POKEMON_PARAM* poke;

	OS_Printf( "**********csr_pos = %d\n", csr_pos );
	poke =  PokeParty_GetMemberPointer( wk->p_party, GetCsrPokePos(wk->h_max,csr_pos) );
	OS_Printf( "**********poke = %d\n", poke );

#if 0
	//CP減らす
	Castle_CPRecord_Sub(wk->fro_sv, wk->type,
						kaihuku_cp_tbl[type-FC_PARAM_LIST_START_KAIHUKU] );

	CastleMine_SeqSubCPWrite( wk, &wk->bmpwin[MINE_BMPWIN_TR1] );		//CP表示
#endif

	//現在の回復ランクに応じたメッセージ
	now_rank = Castle_GetRank( wk->sv, wk->type, CASTLE_RANK_TYPE_KAIHUKU );
	Castle_SetPokeName( wk, 0, PPPPointerGet(poke) );
	CastleTalkWinPut( &wk->bmpwin[MINE_BMPWIN_TALK], CONFIG_GetWindowType(wk->config));
	wk->msg_index = Castle_EasyMsg( wk, kaihuku_msg[type-FC_PARAM_LIST_START_KAIHUKU], FONT_TALK );

	//回復前のHPを保存しておく
	//wk->before_hp = PokeParaGet( poke, ID_PARA_hp, NULL );

	//実際の回復処理
	switch( type ){

	case FC_PARAM_HP_KAIHUKU:
		CastleMine_ItemUse( poke, ITEM_MANTANNOKUSURI );
		break;

	case FC_PARAM_PP_KAIHUKU:
		CastleMine_ItemUse( poke, ITEM_PIIPIIMAKKUSU );
		break;

	case FC_PARAM_HPPP_KAIHUKU:
		CastleMine_ItemUse( poke, ITEM_MANTANNOKUSURI );
		CastleMine_ItemUse( poke, ITEM_PIIPIIMAKKUSU );
		break;

	default:
		GF_ASSERT( 0 );
	};

	Snd_SePlay( SEQ_SE_DP_KAIFUKU );
	//Castle_PokeHpMsgWrite( wk, &wk->bmpwin[MINE_BMPWIN_HP] );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	決定したアイテムのＣＰ減らす、持たせる、メッセージ表示
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleMine_SeqSubItem( CASTLE_MINE_WORK* wk, u8 csr_pos, u16 item )
{
	POKEMON_PARAM* poke;

	poke =  PokeParty_GetMemberPointer( wk->p_party, GetCsrPokePos(wk->h_max,csr_pos) );

	//アイテム持たせる
	PokeParaPut( poke, ID_PARA_item, &item );

	//アイテム持っているアイコンを表示
	CastleObj_Vanish( wk->p_itemkeep[GetCsrPokePos(wk->h_max,csr_pos)], CASTLE_VANISH_OFF );//表示

	//「○に○を持たせました！」
	Castle_SetPokeName( wk, 0, PPPPointerGet(poke) );
	WORDSET_RegisterItemName( wk->wordset, 1, item );
	wk->msg_index = Castle_EasyMsg( wk, msg_castle_poke_07, FONT_TALK );

	//Snd_SePlay( SEQ_SE_DP_DENSI16 );
	//Snd_SePlay( SEQ_SE_DP_SUTYA );
	//Snd_SePlay( SEQ_SE_PL_BAG_030 );
	Snd_SePlay( SEQ_SE_DP_UG_020 );

	//Castle_NameCPWinWrite( wk, &wk->bmpwin[MINE_BMPWIN_CP] );	//リスト終了するので更新なし
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
static void CastleMine_SeqSubMenuWinClear( CASTLE_MINE_WORK* wk )
{
	if( wk->menu_flag == 1 ){
		wk->menu_flag = 0;
		BmpMenuExit( wk->mw, NULL );
		BmpMenuWinClear( wk->MenuH.win, WINDOW_TRANS_OFF );
		//BmpMenuWinClear( wk->MenuH.win, WINDOW_TRANS_OFF );
		//GF_BGL_BmpWinOnVReq( wk->MenuH.win );
		GF_BGL_BmpWinOffVReq( wk->MenuH.win );
	}
	return;
}

//--------------------------------------------------------------
/**
 * @brief	リスト開始フラグセット
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleMine_SeqSubListStart( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win )
{
	SDK_ASSERTMSG(wk->list_flag == 0,"リストが生きているのに、再度リストを開始しようとしています");
	wk->list_flag	= 1;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	アイテムリストを終了した時にする処理
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleMine_SeqSubItemListEnd( CASTLE_MINE_WORK* wk )
{
	CastleObj_Vanish( wk->p_itemicon, CASTLE_VANISH_ON );					//非表示
	CastleObj_Vanish( wk->p_scr_u, CASTLE_VANISH_ON );						//非表示
	CastleObj_Vanish( wk->p_scr_d, CASTLE_VANISH_ON );						//非表示
	CastleObj_Vanish( wk->p_item_csr, CASTLE_VANISH_ON );					//非表示

	//名前＋CPウィンドウを消す
	BmpMenuWinClear( &wk->bmpwin[MINE_BMPWIN_CP], WINDOW_TRANS_OFF );
	GF_BGL_BmpWinOffVReq( &wk->bmpwin[MINE_BMPWIN_CP] );

	//ポケモンアイコンの位置を戻す
	CastleMine_ItemWinPokeIconMove( wk, wk->csr_pos, 0 );

	//HP非表示、LV非表示
	GF_BGL_BmpWinDataFill( &wk->bmpwin[MINE_BMPWIN_ITEM_HP], FBMP_COL_NULL );	//塗りつぶし
	GF_BGL_BmpWinOffVReq( &wk->bmpwin[MINE_BMPWIN_ITEM_HP] );
	GF_BGL_BmpWinDataFill( &wk->bmpwin[MINE_BMPWIN_ITEM_LV], FBMP_COL_NULL );	//塗りつぶし
	GF_BGL_BmpWinOffVReq( &wk->bmpwin[MINE_BMPWIN_ITEM_LV] );

	//GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_OFF );		//OBJ非表示
	//GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );		//TEST
	return;;
}

//--------------------------------------------------------------
/**
 * @brief	リストへ戻る
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleMine_SeqSubItemListReturn( CASTLE_MINE_WORK* wk )
{
	u16 pos_bak;

	BmpTalkWinClearSub( &wk->bmpwin[MINE_BMPWIN_TALK] );
	CastleObj_Vanish( wk->p_itemicon, CASTLE_VANISH_OFF );		//表示
	BmpListRewrite( wk->lw );
	BmpListDirectPosGet( wk->lw, &pos_bak );
	CastleItemInfoWriteMsg( wk, &wk->bmpwin[MINE_BMPWIN_ITEMINFO],
							GetItemNo(wk,pos_bak,wk->parent_decide_type) );
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
static void CastleMine_SeqSubListEnd( CASTLE_MINE_WORK* wk )
{
	GF_BGL_BMPWIN* p_list_win;

	if( wk->list_flag == 1 ){
		wk->list_flag = 0;
		p_list_win = (GF_BGL_BMPWIN*)BmpListParamGet( wk->lw, BMPLIST_ID_WIN );

		BmpMenuWinClear( p_list_win, WINDOW_TRANS_OFF );
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
 * @brief	CP更新
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
#ifdef NONEQUIVALENT
static void CastleMine_SeqSubCPWrite( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win )
{
	u16 x,y,offset_x,offset_y,pair_offset_x,pair_offset_y,now_cp;

	Castle_GetOffset( wk, &offset_x, &offset_y, &pair_offset_x, &pair_offset_y );

	if( Castle_CommCheck(wk->type) == FALSE ){
		x = offset_x + INFO_PLAYER_CP_X;
		y = offset_y + INFO_PLAYER_CP_Y;
		GF_BGL_BmpWinFill( win, FBMP_COL_NULL, x, y, 6*8, 2*8 );

		now_cp = FrontierRecord_Get(wk->fro_sv, CastleScr_GetCPRecordID(wk->type),
									//FRONTIER_RECORD_NOT_FRIEND );		//現在のCP
									Frontier_GetFriendIndex(CastleScr_GetCPRecordID(wk->type)) );

		Castle_SetNumber(	wk, 0, now_cp, CASTLE_KETA_CP,
							NUMBER_DISPTYPE_SPACE);
		wk->msg_index = CastleWriteMsgSimple( wk, win, 
									msg_castle_poke_cp_01, x, y , MSG_NO_PUT, 
									FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, BC_FONT );
	}else{

		//親、子の画面とも、親、子の順番にCPが表示されるようにする

		////////////////////////////////////////////////////////////////
		//親なら
		if( CommGetCurrentID() == COMM_PARENT_ID ){
			x = offset_x + INFO_PLAYER_CP_X;
			y = offset_y + INFO_PLAYER_CP_Y;
			GF_BGL_BmpWinFill( win, FBMP_COL_NULL, x, y, 6*8, 2*8 );

			now_cp = FrontierRecord_Get(wk->fro_sv, CastleScr_GetCPRecordID(wk->type),
									//FRONTIER_RECORD_NOT_FRIEND );		//現在のCP
									Frontier_GetFriendIndex(CastleScr_GetCPRecordID(wk->type)) );

			Castle_SetNumber(	wk, 0, now_cp, CASTLE_KETA_CP,
								NUMBER_DISPTYPE_SPACE);
			wk->msg_index = CastleWriteMsgSimple( wk, win, 
									msg_castle_poke_cp_01, x, y , MSG_NO_PUT, 
									FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, BC_FONT );

			//パートナーのCPを表示
			x = pair_offset_x + INFO_PLAYER_CP_X;
			y = pair_offset_y + INFO_PLAYER_CP_Y;
			GF_BGL_BmpWinFill( win, FBMP_COL_NULL, x, y, 6*8, 2*8 );
			Castle_SetNumber( wk, 0, wk->pair_cp, CASTLE_KETA_CP, NUMBER_DISPTYPE_SPACE );
			wk->msg_index = CastleWriteMsgSimple( wk, win, 
									msg_castle_poke_cp_02, x, y , MSG_NO_PUT, 
									FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, BC_FONT );

		////////////////////////////////////////////////////////////////
		//子なら
		}else{
			//パートナーのCPを表示
			x = offset_x + INFO_PLAYER_CP_X;
			y = offset_y + INFO_PLAYER_CP_Y;
			GF_BGL_BmpWinFill( win, FBMP_COL_NULL, x, y, 6*8, 2*8 );
			Castle_SetNumber( wk, 0, wk->pair_cp, CASTLE_KETA_CP, NUMBER_DISPTYPE_SPACE );
			wk->msg_index = CastleWriteMsgSimple( wk, win, 
									msg_castle_poke_cp_02, x, y , MSG_NO_PUT, 
									FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, BC_FONT );

			x = pair_offset_x + INFO_PLAYER_CP_X;
			y = pair_offset_y + INFO_PLAYER_CP_Y;
			GF_BGL_BmpWinFill( win, FBMP_COL_NULL, x, y, 6*8, 2*8 );

			now_cp = FrontierRecord_Get(wk->fro_sv, CastleScr_GetCPRecordID(wk->type),
									//FRONTIER_RECORD_NOT_FRIEND );		//現在のCP
									Frontier_GetFriendIndex(CastleScr_GetCPRecordID(wk->type)) );

			Castle_SetNumber(	wk, 0, now_cp, CASTLE_KETA_CP,
								NUMBER_DISPTYPE_SPACE);
			wk->msg_index = CastleWriteMsgSimple( wk, win, 
									msg_castle_poke_cp_01, x, y , MSG_NO_PUT, 
									FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, BC_FONT );
		}
	}

	GF_BGL_BmpWinOnVReq( win );
	return;
}
#else
asm static void CastleMine_SeqSubCPWrite( CASTLE_MINE_WORK* wk, GF_BGL_BMPWIN* win )
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
	bne _0224581A
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
	bl CastleWriteMsgSimple_Full_ov107_2243890
	strb r0, [r5, #0xa]
	b _022459C2
_0224581A:
	bl CommGetCurrentID
	cmp r0, #0
	add r1, sp, #0x28
	bne _022458F4
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
	bl CastleWriteMsgSimple_Full_ov107_2243890
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
	ldr r2, =0x00000436 // _022459CC
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
	mov r1, #2
	str r1, [sp, #0xc]
	mov r0, #0
	str r0, [sp, #0x10]
	str r0, [sp, #0x14]
	str r1, [sp, #0x18]
	add r0, r5, #0
	add r1, r4, #0
	mov r2, #3
	add r3, r7, #0
	bl CastleWriteMsgSimple_Full_ov107_2243890
	strb r0, [r5, #0xa]
	b _022459C2
_022458F4:
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
	ldr r2, =0x00000436 // _022459CC
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
	mov r1, #2
	str r1, [sp, #0xc]
	mov r0, #0
	str r0, [sp, #0x10]
	str r0, [sp, #0x14]
	str r1, [sp, #0x18]
	add r0, r5, #0
	add r1, r4, #0
	mov r2, #3
	add r3, r7, #0
	bl CastleWriteMsgSimple_Full_ov107_2243890
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
	bl CastleWriteMsgSimple_Full_ov107_2243890
	strb r0, [r5, #0xa]
_022459C2:
	add r0, r4, #0
	bl GF_BGL_BmpWinOnVReq
	add sp, #0x30
	pop {r3, r4, r5, r6, r7, pc}
	// .align 2, 0
// _022459CC: .4byte 0x00000436
}
#endif

//--------------------------------------------------------------
/**
 * @brief	ランクアップ
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleMine_SeqSubRankUp( CASTLE_MINE_WORK* wk, u8 parent_decide_pos, u8 parent_decide_type )
{
	u8	sel_pos,type_offset;
	u16 sub_cp;
	u16 buf16[4];
	u16 offset_x,offset_y,pair_offset_y;

	Snd_SePlay( SEQ_SE_DP_PIRORIRO2 );

	//オフセット
	type_offset = wk->modoru_pos;

	//選んだカーソル位置を取得
	sel_pos = GetCommSelCsrPos( type_offset, parent_decide_pos );

	////////////////////////////////////////////////
	//道具の種類によって引かれるCPがかわる
	//switch( GetCsrSelType(wk->h_max,sel_pos) ){
	switch( parent_decide_type ){

	//case SEL_KAIHUKU:
	case FC_PARAM_HP_KAIHUKU:			//ＨＰかいふく
	case FC_PARAM_PP_KAIHUKU:			//ＰＰかいふく
	case FC_PARAM_HPPP_KAIHUKU:			//すべてかいふく
		sub_cp = kaihuku_cp_tbl[parent_decide_type-FC_PARAM_LIST_START_KAIHUKU];;
		break;

	//case SEL_RENTAL:
	case FC_PARAM_KINOMI:				//きのみ
	case FC_PARAM_ITEM:					//どうぐ
		sub_cp = GetItemCPByItemNo( wk->parent_decide_item );
		break;

	//case SEL_TUYOSA:
	//case SEL_WAZA:
	case FC_PARAM_TUYOSA:				//つよさ
	case FC_PARAM_WAZA:					//わざ
		sub_cp = 0;
		break;
	};

	////////////////////////////////////////////////
	//親
	if( CommGetCurrentID() == COMM_PARENT_ID ){

		//親：親の決定が有効なら、LRはそのまま
		if( parent_decide_pos < type_offset ){
			Castle_SetPlayerName( wk, 5 );
			Castle_CPRecord_Sub( wk->fro_sv, wk->type,sub_cp );

		//親：子の決定が有効なら、LRは逆になる
		}else{
			Castle_SetPairName( wk->wordset, 5 );
			wk->pair_cp -= sub_cp;								//相手のCP減らす
		}

	///////////////////////////////////////////////
	//子
	}else{

		//子：親の決定が有効なら、LRは逆になる
		if( parent_decide_pos < type_offset ){
			Castle_SetPairName( wk->wordset, 5 );
			wk->pair_cp -= sub_cp;								//相手のCP減らす

		//子：子の決定が有効なら、LRはそのまま
		}else{
			Castle_SetPlayerName( wk, 5 );
			Castle_CPRecord_Sub(wk->fro_sv, wk->type, sub_cp );	//自分のCP減らす
		}
	}

	CastleMine_SeqSubCPWrite( wk, &wk->bmpwin[MINE_BMPWIN_TR1] );	//CP表示を更新
	CastleMine_SeqSubMenuWinClear( wk );							//メニューを表示していたら削除
	CastleMine_SeqSubStatusWazaDel( wk );							//ステータス・技を削除

	CastleMine_ItemSeed_Del( wk );									//アイテムリスト表示してたら削除
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );		//TEST

	BmpTalkWinClearSub( &wk->bmpwin[MINE_BMPWIN_TALK] );

	//switch( GetCsrSelType(wk->h_max,sel_pos) ){
	switch( parent_decide_type ){

	//case SEL_KAIHUKU:
	case FC_PARAM_HP_KAIHUKU:											//ＨＰかいふく
	case FC_PARAM_PP_KAIHUKU:											//ＰＰかいふく
	case FC_PARAM_HPPP_KAIHUKU:											//すべてかいふく
		CastleTalkWinPut( &wk->bmpwin[MINE_BMPWIN_TALK], CONFIG_GetWindowType(wk->config));
		CastleMine_SeqSubKaihuku( wk, sel_pos, parent_decide_type );
		break;

	//case SEL_RENTAL:
	case FC_PARAM_KINOMI:												//きのみ
	case FC_PARAM_ITEM:													//どうぐ
		CastleTalkWinPut( &wk->bmpwin[MINE_BMPWIN_TALK], CONFIG_GetWindowType(wk->config));
		CastleMine_SeqSubItem( wk, sel_pos, wk->parent_decide_item );	//どうぐ
		break;

	//case SEL_TUYOSA:
	case FC_PARAM_TUYOSA:												//つよさ
		CastleMine_SeqSubStatusWin( wk, sel_pos );						//つよさ
		break;

	//case SEL_WAZA:
	case FC_PARAM_WAZA:													//わざ
		CastleMine_SeqSubWazaWin( wk, sel_pos );						//わざ
		break;
	};

	return;
}

//--------------------------------------------------------------
/**
 * @brief	つよさ
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleMine_SeqSubStatusWin( CASTLE_MINE_WORK* wk, u8 csr_pos )
{
	POKEMON_PARAM* poke;
	//GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
	Castle_SetStatusBgGraphic( wk, BC_FRAME_SLIDE );
	GF_BGL_BmpWinOffVReq( &wk->bmpwin[MINE_BMPWIN_TALK] );
	GF_BGL_BmpWinOffVReq( &wk->bmpwin[MINE_BMPWIN_LIST] );
	GF_BGL_BmpWinOffVReq( &wk->bmpwin[MINE_BMPWIN_CP] );
	poke =  PokeParty_GetMemberPointer( wk->p_party, GetCsrPokePos(wk->h_max,csr_pos) );
	Castle_StatusMsgWrite( wk, &wk->bmpwin[MINE_BMPWIN_STATUS], poke );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	わざ
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleMine_SeqSubWazaWin( CASTLE_MINE_WORK* wk, u8 csr_pos )
{
	POKEMON_PARAM* poke;
	//GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
	Castle_SetWazaBgGraphic( wk, BC_FRAME_SLIDE );
	GF_BGL_BmpWinOffVReq( &wk->bmpwin[MINE_BMPWIN_TALK] );
	GF_BGL_BmpWinOffVReq( &wk->bmpwin[MINE_BMPWIN_LIST] );
	GF_BGL_BmpWinOffVReq( &wk->bmpwin[MINE_BMPWIN_CP] );
	poke =  PokeParty_GetMemberPointer( wk->p_party, GetCsrPokePos(wk->h_max,csr_pos) );
	Castle_WazaMsgWrite( wk, &wk->bmpwin[MINE_BMPWIN_STATUS], poke );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	つよさ、わざウィンドウを消す
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleMine_SeqSubStatusWazaDel( CASTLE_MINE_WORK* wk )
{
	GF_BGL_BmpWinDataFill( &wk->bmpwin[MINE_BMPWIN_STATUS], FBMP_COL_NULL );	//塗りつぶし
	GF_BGL_BmpWinOff( &wk->bmpwin[MINE_BMPWIN_STATUS] );						//★VReqしない
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	アイコン上下アニメ
 *
 * @param	wk		CASTLE_MINE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleMine_IconSelAnm( CASTLE_MINE_WORK* wk )
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


//==============================================================================================
//
//	デバック
//
//==============================================================================================
static void Debug_HpDown( CASTLE_MINE_WORK* wk, u8 no )
{
	u32 hp;
	POKEMON_PARAM* poke;

	//ポケモンへのポインタ取得
	poke =  PokeParty_GetMemberPointer( wk->p_party, no );

	hp = 5;
	PokeParaPut( poke, ID_PARA_hp, &hp );
	//PokeParaCalc( poke );
	return;
}


//==============================================================================================
//
//	castle_rank.c
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	ランクアップ(castle_rank.cから移動)
 *
 * @param	wk		CASTLE_RANK_WORK型のポインタ
 * @param	param	FC_PARAM_KAIHUKU_RANKUP,FC_PARAM_RENTAL_RANKUP
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CastleRank_SeqSubRankUp( CASTLE_MINE_WORK* wk, u8 parent_decide_pos, u8 param )
{
	u8	type_offset,sel_pos,type,m_max;
	u32 now_rank;
	u16 buf16[4];

	m_max = Castle_GetMinePokeNum( wk->type, CASTLE_FLAG_SOLO );

	//取得するランクタイプ
	if( param == FC_PARAM_KAIHUKU_RANKUP ){
		type = CASTLE_RANK_TYPE_KAIHUKU;
	}else{
		type = CASTLE_RANK_TYPE_RENTAL;
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

			Castle_CPRecord_Sub( wk->fro_sv, wk->type, rankup_point_tbl[type][now_rank] );

			//ランクアップ
			now_rank = Castle_GetRank( wk->sv, wk->type, type );

			buf16[0] = (now_rank + 1);
			FrontierRecord_Set(	SaveData_GetFrontier(wk->sv), 
				CastleScr_GetRankRecordID(wk->type,type),
				Frontier_GetFriendIndex(CastleScr_GetRankRecordID(wk->type,type)), (now_rank + 1) );

			OS_Printf( "親：親をランクアップ %d\n", (now_rank+1) );

			//ランクアップした時に戻り先が特殊フラグON
			if( Castle_CommCheck(wk->type) == TRUE ){
				if( param == FC_PARAM_KAIHUKU_RANKUP ){
					wk->rankup_recover_flag = 1;
				}else{
					wk->rankup_recover_flag = 2;
				}
			}

		//親：子の決定が有効なら
		}else{
			Castle_SetPairName( wk->wordset, 5 );
			now_rank= wk->pair_rank[type];
			wk->pair_cp -= rankup_point_tbl[type][now_rank];						//CP減らす
			wk->pair_rank[type]++;
		}

	///////////////////////////////////////////////
	//子
	}else{

		//子：親の決定が有効なら
		if( parent_decide_pos < type_offset ){
			Castle_SetPairName( wk->wordset, 5 );
			now_rank= wk->pair_rank[type];
			wk->pair_cp -= rankup_point_tbl[type][now_rank];						//CP減らす
			wk->pair_rank[type]++;

		//子：子の決定が有効なら
		}else{
			Castle_SetPlayerName( wk, 5 );
			now_rank = Castle_GetRank( wk->sv, wk->type, type );

			Castle_CPRecord_Sub( wk->fro_sv, wk->type, rankup_point_tbl[type][now_rank] );

			now_rank = Castle_GetRank( wk->sv, wk->type, type );

			buf16[0] = (now_rank + 1);
			FrontierRecord_Set(	SaveData_GetFrontier(wk->sv), 
				CastleScr_GetRankRecordID(wk->type,type),
				Frontier_GetFriendIndex(CastleScr_GetRankRecordID(wk->type,type)), (now_rank + 1) );
			OS_Printf( "子：子をランクアップ %d\n", (now_rank+1) );

			//ランクアップした時に戻り先が特殊フラグON
			if( Castle_CommCheck(wk->type) == TRUE ){
				if( param == FC_PARAM_KAIHUKU_RANKUP ){
					wk->rankup_recover_flag = 1;
				}else{
					wk->rankup_recover_flag = 2;
				}
			}
		}
	}

	CastleMine_SeqSubMenuWinClear( wk );							//メニューを表示していたら削除

	//CP表示を更新
	CastleMine_SeqSubCPWrite( wk, &wk->bmpwin[MINE_BMPWIN_TR1] );

	//CastleRank_SeqSubAllInfoWrite( wk );							//情報更新

	//会話ウィンドウ表示
	CastleTalkWinPut( &wk->bmpwin[MINE_BMPWIN_TALK], CONFIG_GetWindowType(wk->config) );

	//「ランクがアップしました！」
	wk->msg_index = Castle_EasyMsg( wk, rankup_pair_msg_tbl[type][now_rank], FONT_TALK );
	//Snd_SePlay( SEQ_SE_DP_PIRORIRO2 );
	//Snd_SePlay( SEQ_SE_DP_DANSA4 );
	return;
}



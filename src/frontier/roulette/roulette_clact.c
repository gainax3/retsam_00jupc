//==============================================================================================
/**
 * @file	roulette_clact.c
 * @brief	「バトルルーレット」セルアクター
 * @author	Satoshi Nohara
 * @date	06.09.05
 */
//==============================================================================================
#include "common.h"
#include "src_os_print.h"
#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "poketool/pokeicon.h"
#include "poketool/pokeparty.h"
#include "itemtool/item.h"
#include "system/procsys.h"
#include "application/pokelist.h"

#include "../roulette_def.h"
#include "roulette_sys.h"
#include "roulette_clact.h"
#include "../graphic/frontier_obj_def.h"
#include "../graphic/frontier_bg_def.h"


//==============================================================================================
//
//	定義
//
//==============================================================================================
//キャラクターマネージャー
#define CHAR_CONT_NUM						(32)//(8)//(6)//(16)//(3)		//キャラクタ制御数
#if 0
#define CHAR_VRAMTRANS_MAIN_SIZE			(2048)
#define CHAR_VRAMTRANS_SUB_SIZE				(2048)
#else
#define CHAR_VRAMTRANS_MAIN_SIZE			(1024)
#define CHAR_VRAMTRANS_SUB_SIZE				(1024)
//#define CHAR_VRAMTRANS_MAIN_SIZE			(4096)
//#define CHAR_VRAMTRANS_SUB_SIZE				(4096)
#endif

//パレットマネージャー
#define PLTT_ITEMKEEP_NUM					(1)			//アイテム持っているアイコンパレット

enum{
	DISP_MAIN_OBJ_PAL = 8,								//
	DISP_SUB_OBJ_PAL = 2,								//
};


//==============================================================================================
//
//	データ
//
//==============================================================================================
//リソースタイプ列挙に合わせる(include/system/clact_util_res.h)
//リソースマネージャー登録数テーブル
static const u8 ResEntryNumTbl[ROULETTE_RESOURCE_NUM] = {
#if 0
	2,			//キャラリソース
	3,			//パレットリソース
	2,			//セルリソース
	2,			//セルアニメリソース
#else
	ROULETTE_RES_OBJ_MAX,		//キャラリソース
	ROULETTE_RES_OBJ_MAX,		//パレットリソース
	ROULETTE_RES_OBJ_MAX,		//セルリソース
	ROULETTE_RES_OBJ_MAX,		//セルアニメリソース
#endif
};


//==============================================================================================
//
//	プロトタイプ宣言
//
//==============================================================================================
//RouletteClact関数
//void			RouletteClact_InitCellActor( ROULETTE_CLACT* wk );
void			RouletteClact_InitCellActor( ROULETTE_CLACT* wk, POKEPARTY* m_party, POKEPARTY* e_pary, u8 type );
CLACT_WORK_PTR	RouletteClact_SetActor( ROULETTE_CLACT* wk, u32 char_no, u32 pltt_no, u32 cell_no, u32 anm_no, u32 pri, int bg_pri, u8 disp );
void			RouletteClact_DeleteCellObject( ROULETTE_CLACT* wk );

//static
static void		InitCharPlttManager( void );
static void		RouletteClact_Load( ROULETTE_CLACT* wk );
static void		RouletteClact_ItemKeepLoad( ROULETTE_CLACT* wk );
static void		RouletteClact_IconLoad( ROULETTE_CLACT* wk, POKEPARTY* m_party, POKEPARTY* e_party, u8 type );
static void		RouletteClact_ButtonLoad( ROULETTE_CLACT* wk );


//==============================================================================================
//
//	関数
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	セルアクター初期化
 *
 * @param	wk		ROULETTE_CLACT型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
//void RouletteClact_InitCellActor( ROULETTE_CLACT* wk )
void RouletteClact_InitCellActor( ROULETTE_CLACT* wk, POKEPARTY* m_party, POKEPARTY* e_party, u8 type )
{
	int i;

	initVramTransferManagerHeap( 32, HEAPID_ROULETTE );

	//キャラクター・パレットマネージャー初期化
	InitCharPlttManager();

	//OAMマネージャーの初期化
	NNS_G2dInitOamManagerModule();

	//共有OAMマネージャ作成
	//レンダラ用OAMマネージャ作成
	//ここで作成したOAMマネージャをみんなで共有する
	REND_OAMInit(	0, 128,				//メイン画面OAM管理領域
					0, 32,				//メイン画面アフィン管理領域
					0, 128,				//サブ画面OAM管理領域
					0, 32,				//サブ画面アフィン管理領域
					HEAPID_ROULETTE);
	
	//セルアクターセットの簡単初期化(作成されたセルアクターセットが返る)
	wk->ClactSet = CLACT_U_SetEasyInit( ROULETTE_CLACT_OBJ_MAX, &wk->RendData, HEAPID_ROULETTE );
	
	//リソースマネージャー初期化
	for( i=0; i < ROULETTE_RESOURCE_NUM ;i++ ){		//リソースマネージャー作成
		wk->ResMan[i] = CLACT_U_ResManagerInit( ResEntryNumTbl[i], i, HEAPID_ROULETTE );
	}

	/***************/
	//	下画面
	/***************/
	RouletteClact_ButtonLoad( wk );

	/***************/
	//	上画面
	/***************/
	RouletteClact_Load( wk );

	/******************************/
	//	上画面(アイテム持っているかアイコン)
	/******************************/
	RouletteClact_ItemKeepLoad( wk );

	/******************************/
	//	上画面(ポケモンアイコン)
	/******************************/
	RouletteClact_IconLoad( wk, m_party, e_party, type );

	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	
	//リソースマネージャーから転送
	//for( i=0; i < ROULETTE_RES_OBJ_MAX ;i++ ){
	for( i=0; i < ROULETTE_ID_CHAR_MAX ;i++ ){
		CLACT_U_CharManagerSet( wk->ResObjTbl[i][CLACT_U_CHAR_RES] );	//Char転送
		//CLACT_U_PlttManagerSet( wk->ResObjTbl[i][CLACT_U_PLTT_RES] );	//パレット転送
	}

	//リソースマネージャーから転送
	//for( i=0; i < ROULETTE_RES_OBJ_MAX ;i++ ){
	for( i=0; i < ROULETTE_ID_PLTT_MAX ;i++ ){
		CLACT_U_PlttManagerSet( wk->ResObjTbl[i][CLACT_U_PLTT_RES] );	//パレット転送
	}


	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );			//SUB DISP OBJ ON
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );			//MAIN DISP OBJ ON

	return;
}

//--------------------------------------------------------------
/**
 * @brief	セルアクターをセット
 *
 * @param	wk		ROULETTE_CLACT型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
CLACT_WORK_PTR	RouletteClact_SetActor( ROULETTE_CLACT* wk, u32 char_no, u32 pltt_no, u32 cell_no, u32 anm_no, u32 pri, int bg_pri, u8 disp )
{
	int i;
	CLACT_HEADER cl_act_header;
	CLACT_WORK_PTR act;
	
	//OS_Printf( "cell_no = %d\n", cell_no );

	//セルアクターヘッダ作成	
	CLACT_U_MakeHeader(	&cl_act_header,
						//登録したリソースのID(ResObjTbl[id])
						//data_no, data_no, data_no, data_no,
						char_no, pltt_no, cell_no, cell_no,
						CLACT_U_HEADER_DATA_NONE, CLACT_U_HEADER_DATA_NONE,
						0, bg_pri,						//VRAM転送かフラグ、BGとの優先順位
						wk->ResMan[CLACT_U_CHAR_RES],
						wk->ResMan[CLACT_U_PLTT_RES],
						wk->ResMan[CLACT_U_CELL_RES],
						wk->ResMan[CLACT_U_CELLANM_RES],
						NULL,NULL);

	{
		//登録情報格納
		CLACT_ADD add;

		add.ClActSet	= wk->ClactSet;
		add.ClActHeader	= &cl_act_header;

		//add.mat.x		= 0;//FX32_CONST(32) ;
		//add.mat.y		= 0;//FX32_CONST(96) ;		//画面は上下連続している（MAINが上、SUBが下）
		add.mat.x		= 0;
		add.mat.y		= 0;						//画面は上下連続している（MAINが上、SUBが下）
		add.mat.z		= 0;
		add.sca.x		= FX32_ONE;
		add.sca.y		= FX32_ONE;
		add.sca.z		= FX32_ONE;
		add.rot			= 0;
		add.pri			= pri;
		add.heap		= HEAPID_ROULETTE;

		if( disp == DISP_MAIN ){
			add.DrawArea= NNS_G2D_VRAM_TYPE_2DMAIN;
		}else{
			add.DrawArea= NNS_G2D_VRAM_TYPE_2DSUB;
			add.mat.y += SUB_SURFACE_Y;				//座標を補正
		}

		//セルアクター表示開始
		act = CLACT_Add(&add);
		//CLACT_SetAnmFlag( act, 0 );				//非アニメ
		CLACT_SetAnmFlag( act, 1 );					//オートアニメ
		CLACT_SetAnmFrame( act, FX32_ONE );			//オートアニメーションフレームを設定
		CLACT_AnmChg( act, anm_no );				//アニメーションのシーケンスをチェンジする
	}	

	return act;
}

//--------------------------------------------------------------
/**
 * @brief	2Dセルオブジェクト解放
 *
 * @param	wk		ROULETTE_CLACT型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
void RouletteClact_DeleteCellObject(ROULETTE_CLACT* wk)
{
	u8 i;

	//セルアクターリソース解放
	//for( i=0; i < ROULETTE_RES_OBJ_MAX ;i++ ){
	for( i=0; i < ROULETTE_ID_CHAR_MAX ;i++ ){
		CLACT_U_CharManagerDelete( wk->ResObjTbl[i][CLACT_U_CHAR_RES] );//char転送マネージャー破棄
	}

	//for( i=0; i < ROULETTE_RES_OBJ_MAX ;i++ ){
	for( i=0; i < ROULETTE_ID_PLTT_MAX ;i++ ){
		CLACT_U_PlttManagerDelete( wk->ResObjTbl[i][CLACT_U_PLTT_RES] );//pltt転送マネージャー破棄
	}
		
	//キャラ・パレット・セル・セルアニメのリソースマネージャー破棄
	for( i=0; i < ROULETTE_RESOURCE_NUM ;i++ ){
		CLACT_U_ResManagerDelete( wk->ResMan[i] );
	}

	//セルアクターセット破棄
	CLACT_DestSet(wk->ClactSet);

	//OAMレンダラー破棄
	REND_OAM_Delete();

	DeleteCharManager();
	DeletePlttManager();

	return;
}


//==============================================================================================
//
//	static
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	キャラクター・パレットマネージャー初期化
 *
 * @param	none
 *
 * @return	none
 */
//--------------------------------------------------------------
static void InitCharPlttManager(void)
{
	//キャラクタマネージャー初期化
	{
		CHAR_MANAGER_MAKE cm = {
			CHAR_CONT_NUM,						//キャラクタ制御数
			CHAR_VRAMTRANS_MAIN_SIZE,			//メイン画面のVram転送用に用意するVramサイズ
			CHAR_VRAMTRANS_SUB_SIZE,			//サブ画面のVram転送用に用意するVramサイズ
			HEAPID_ROULETTE						//使用するヒープ
		};
		InitCharManagerReg( &cm, GX_OBJVRAMMODE_CHAR_1D_32K, GX_OBJVRAMMODE_CHAR_1D_32K );
	}

	//パレットマネージャー初期化
	//InitPlttManager((DISP_MAIN_OBJ_PAL + PLTT_ITEMKEEP_NUM + POKEICON_PAL_MAX), HEAPID_ROULETTE);
	InitPlttManager( (DISP_MAIN_OBJ_PAL + DISP_SUB_OBJ_PAL + 
						PLTT_ITEMKEEP_NUM + POKEICON_PAL_MAX), HEAPID_ROULETTE );

	//読み込み開始位置を初期化
	CharLoadStartAll();
	PlttLoadStartAll();

	return;
}

//--------------------------------------------------------------------------------------------
/**
 * ルーレットデータのロード
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void RouletteClact_Load( ROULETTE_CLACT* wk )
{
	//chara読み込み
	wk->ResObjTbl[ROULETTE_RES_OBJ_CSR][CLACT_U_CHAR_RES] = CLACT_U_ResManagerResAddArcChar(
					wk->ResMan[CLACT_U_CHAR_RES],
					ARC_FRONTIER_OBJ, BR_OBJ_NCGR_BIN,
					1, ROULETTE_ID_OBJ_CSR, NNS_G2D_VRAM_TYPE_2DMAIN, HEAPID_ROULETTE);

	//pal読み込み
	wk->ResObjTbl[ROULETTE_RES_OBJ_CSR][CLACT_U_PLTT_RES] = CLACT_U_ResManagerResAddArcPltt(
					wk->ResMan[CLACT_U_PLTT_RES],
					ARC_FRONTIER_OBJ, BR_OBJ_NCLR,
					FALSE, ROULETTE_ID_OBJ_CSR, NNS_G2D_VRAM_TYPE_2DMAIN, 
					DISP_MAIN_OBJ_PAL,HEAPID_ROULETTE);

	//cell読み込み
	wk->ResObjTbl[ROULETTE_RES_OBJ_CSR][CLACT_U_CELL_RES] = CLACT_U_ResManagerResAddArcKindCell(
					wk->ResMan[CLACT_U_CELL_RES],
					ARC_FRONTIER_OBJ, BR_OBJ_NCER_BIN,
					1, ROULETTE_ID_OBJ_CSR, CLACT_U_CELL_RES, HEAPID_ROULETTE);

	//同じ関数でanim読み込み
	wk->ResObjTbl[ROULETTE_RES_OBJ_CSR][CLACT_U_CELLANM_RES] = CLACT_U_ResManagerResAddArcKindCell(
					wk->ResMan[CLACT_U_CELLANM_RES],
					ARC_FRONTIER_OBJ, BR_OBJ_NANR_BIN,
					1, ROULETTE_ID_OBJ_CSR, CLACT_U_CELLANM_RES, HEAPID_ROULETTE);

	return;
}

//--------------------------------------------------------------------------------------------
/**
 * アイテム持っているアイコンのロード
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void RouletteClact_ItemKeepLoad( ROULETTE_CLACT* wk )
{
	ARCHANDLE* p_handle;
	u32	i;
	
	p_handle = ArchiveDataHandleOpen( ARC_PLIST_GRA,  HEAPID_ROULETTE ); 
	
	//chara読み込み
	wk->ResObjTbl[ROULETTE_RES_OBJ_ITEMKEEP][CLACT_U_CHAR_RES] = 
		CLACT_U_ResManagerResAddArcChar_ArcHandle(
		wk->ResMan[CLACT_U_CHAR_RES], p_handle, Pokelist_ItemIconCgxArcGet(),
		0, ROULETTE_ID_OBJ_ITEMKEEP, NNS_G2D_VRAM_TYPE_2DMAIN, HEAPID_ROULETTE );

	//pal読み込み
	wk->ResObjTbl[ROULETTE_RES_OBJ_ITEMKEEP][CLACT_U_PLTT_RES] = 
		CLACT_U_ResManagerResAddArcPltt(
		wk->ResMan[CLACT_U_PLTT_RES], ARC_PLIST_GRA, Pokelist_ItemIconPalArcGet(),
		FALSE, ROULETTE_ID_OBJ_ITEMKEEP, NNS_G2D_VRAM_TYPE_2DMAIN,POKEICON_PAL_MAX,HEAPID_ROULETTE );

	//cell読み込み
	wk->ResObjTbl[ROULETTE_RES_OBJ_ITEMKEEP][CLACT_U_CELL_RES] =
		CLACT_U_ResManagerResAddArcKindCell_ArcHandle(
		wk->ResMan[CLACT_U_CELL_RES], p_handle, Pokelist_ItemIconCellArcGet(),
		0, ROULETTE_ID_OBJ_ITEMKEEP, CLACT_U_CELL_RES, HEAPID_ROULETTE );

	//同じ関数でanim読み込み
	wk->ResObjTbl[ROULETTE_RES_OBJ_ITEMKEEP][CLACT_U_CELLANM_RES] = 
		CLACT_U_ResManagerResAddArcKindCell_ArcHandle(
		wk->ResMan[CLACT_U_CELLANM_RES], p_handle, Pokelist_ItemIconCAnmArcGet(),
		0, ROULETTE_ID_OBJ_ITEMKEEP, CLACT_U_CELLANM_RES, HEAPID_ROULETTE );

	ArchiveDataHandleClose( p_handle );	
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * ポケモンアイコンのロード
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void RouletteClact_IconLoad( ROULETTE_CLACT* wk, POKEPARTY* m_party, POKEPARTY* e_party, u8 type )
{
	POKEMON_PARAM* m_poke;
	POKEMON_PARAM* e_poke;
	ARCHANDLE* p_handle;
	u32	i;

	//chara読み込み
	p_handle = ArchiveDataHandleOpen( ARC_POKEICON, HEAPID_ROULETTE );	

	//pal読み込み
	wk->ResObjTbl[ROULETTE_RES_OBJ_ICON1][CLACT_U_PLTT_RES] = 
			CLACT_U_ResManagerResAddArcPltt(
				wk->ResMan[CLACT_U_PLTT_RES],
				ARC_POKEICON, PokeIconPalArcIndexGet(),
				FALSE, ROULETTE_ID_PLTT_ICON, NNS_G2D_VRAM_TYPE_2DMAIN,
				POKEICON_PAL_MAX,HEAPID_ROULETTE);

	//cell読み込み
	wk->ResObjTbl[ROULETTE_RES_OBJ_ICON1][CLACT_U_CELL_RES] = 
			CLACT_U_ResManagerResAddArcKindCell_ArcHandle(
				wk->ResMan[CLACT_U_CELL_RES],
				//p_handle, PokeIcon64kCellArcIndexGet(),
				//p_handle, PokeIconCellArcIndexGet(),
				p_handle, PokeIconAnmCellArcIndexGet(),
				0, ROULETTE_ID_CELL_ICON, CLACT_U_CELL_RES, HEAPID_ROULETTE);

	//同じ関数でanim読み込み
	wk->ResObjTbl[ROULETTE_RES_OBJ_ICON1][CLACT_U_CELLANM_RES] = 
			CLACT_U_ResManagerResAddArcKindCell_ArcHandle(
				wk->ResMan[CLACT_U_CELLANM_RES],
				//p_handle, PokeIcon64kCellAnmArcIndexGet(),
				//p_handle, PokeIconCellAnmArcIndexGet(),
				p_handle, PokeIconAnmCellAnmArcIndexGet(),
				0, ROULETTE_ID_CELLANM_ICON, CLACT_U_CELLANM_RES, HEAPID_ROULETTE);

	//通信対戦時の味方2+2 敵2+2の最大8匹
	for( i=0; i < ROULETTE_COMM_POKE_TOTAL_NUM ;i++ ){

		if( i == 3 ){
			//通信タイプでない時
			if( type == FALSE ){
				m_poke = PokeParty_GetMemberPointer( m_party, 0 );	//使用しないが代入のみしておく
				e_poke = PokeParty_GetMemberPointer( e_party, 0 );	//使用しないが代入のみしておく
			}else{
				m_poke = PokeParty_GetMemberPointer( m_party, i );
				e_poke = PokeParty_GetMemberPointer( e_party, i );
			}
		}else{
			m_poke = PokeParty_GetMemberPointer( m_party, i );
			e_poke = PokeParty_GetMemberPointer( e_party, i );
		}

		wk->ResObjTbl[ROULETTE_RES_OBJ_ICON1+i][CLACT_U_CHAR_RES] = 
					CLACT_U_ResManagerResAddArcChar_ArcHandle(
					wk->ResMan[CLACT_U_CHAR_RES],
					p_handle, PokeIconCgxArcIndexGetByPP(m_poke),
					0, ROULETTE_ID_CHAR_ICON1+i, NNS_G2D_VRAM_TYPE_2DMAIN, HEAPID_ROULETTE );

		wk->ResObjTbl[ROULETTE_RES_OBJ_ICON5+i][CLACT_U_CHAR_RES] = 
					CLACT_U_ResManagerResAddArcChar_ArcHandle(
					wk->ResMan[CLACT_U_CHAR_RES],
					p_handle, PokeIconCgxArcIndexGetByPP(e_poke),
					0, ROULETTE_ID_CHAR_ICON5+i, NNS_G2D_VRAM_TYPE_2DMAIN, HEAPID_ROULETTE );
	}

	ArchiveDataHandleClose( p_handle );
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * ルーレット停止ボタンデータのロード
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void RouletteClact_ButtonLoad( ROULETTE_CLACT* wk )
{
	//chara読み込み
	wk->ResObjTbl[ROULETTE_RES_OBJ_BUTTON][CLACT_U_CHAR_RES] = CLACT_U_ResManagerResAddArcChar(
					wk->ResMan[CLACT_U_CHAR_RES],
					ARC_FRONTIER_OBJ, BR_BUTTON_NCGR_BIN,
					1, ROULETTE_ID_OBJ_BUTTON, NNS_G2D_VRAM_TYPE_2DSUB, HEAPID_ROULETTE);

	//pal読み込み
	wk->ResObjTbl[ROULETTE_RES_OBJ_BUTTON][CLACT_U_PLTT_RES] = CLACT_U_ResManagerResAddArcPltt(
					wk->ResMan[CLACT_U_PLTT_RES],
					ARC_FRONTIER_OBJ, BR_BUTTON_NCLR,
					FALSE, ROULETTE_ID_OBJ_BUTTON, NNS_G2D_VRAM_TYPE_2DSUB, 
					DISP_SUB_OBJ_PAL,HEAPID_ROULETTE);

	//cell読み込み
	wk->ResObjTbl[ROULETTE_RES_OBJ_BUTTON][CLACT_U_CELL_RES] = CLACT_U_ResManagerResAddArcKindCell(
					wk->ResMan[CLACT_U_CELL_RES],
					ARC_FRONTIER_OBJ, BR_BUTTON_NCER_BIN,
					1, ROULETTE_ID_OBJ_BUTTON, CLACT_U_CELL_RES, HEAPID_ROULETTE);

	//同じ関数でanim読み込み
	wk->ResObjTbl[ROULETTE_RES_OBJ_BUTTON][CLACT_U_CELLANM_RES]=CLACT_U_ResManagerResAddArcKindCell(
					wk->ResMan[CLACT_U_CELLANM_RES],
					ARC_FRONTIER_OBJ, BR_BUTTON_NANR_BIN,
					1, ROULETTE_ID_OBJ_BUTTON, CLACT_U_CELLANM_RES, HEAPID_ROULETTE);

	return;
}



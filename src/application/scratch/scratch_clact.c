//==============================================================================================
/**
 * @file	scratch_clact.c
 * @brief	「スクラッチ」セルアクター
 * @author	Satoshi Nohara
 * @date	06.12.11
 */
//==============================================================================================
#include "common.h"
#include "src_os_print.h"
#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "poketool/pokeicon.h"

#include "scratch_sys.h"
#include "scratch_clact.h"
#include "scratch.naix"


//==============================================================================================
//
//	定義
//
//==============================================================================================
//キャラクターマネージャー
//#define CHAR_CONT_NUM						(4)		//キャラクタ制御数
#define CHAR_CONT_NUM						(5)		//キャラクタ制御数(08.02.09)

#if 0
//.ncgのサイズ(poke.ncg = 160x40 = 6400 / 2 = 約3200 )
//.ncgのサイズ(1画面.ncg = 256x192 = 49152 / 2 = 約24576 )
//#define CHAR_VRAMTRANS_MAIN_SIZE			(68224)				//poke + card + l_oam02サイズ
//#define CHAR_VRAMTRANS_SUB_SIZE			(4096)				//poke サイズ
#else
//.ncgのサイズ(poke.ncg = 200x40 = 8000 / 2 = 約4000 )
//.ncgのサイズ(card.ncg = 384x256 = 34048 / 2 = 約17024 )
//.ncgのサイズ(l_oam02.ncg = 224x152 = 98304 / 2 = 約49152 )
#endif

//#define CHAR_VRAMTRANS_MAIN_SIZE			(128*1024)
//#define CHAR_VRAMTRANS_SUB_SIZE			(16*1024)

//#define CHAR_VRAMTRANS_MAIN_SIZE			(43000)				//poke + card + l_oam02サイズ
#define CHAR_VRAMTRANS_MAIN_SIZE			(3000)				//poke + card + l_oam02サイズ
#define CHAR_VRAMTRANS_SUB_SIZE				(5120)				//poke サイズ

///キャラマネージャ：メイン画面サイズ(byte単位)
//#define FMAP_CHAR_VRAMSIZE_MAIN			(0x10000)	//(1024 * 0x40)	//64K
///キャラマネージャ：サブ画面サイズ(byte単位)
//#define FMAP_CHAR_VRAMSIZE_SUB			(512 * 0x20)	//32K

enum{
	DISP_MAIN_OBJ_PAL	= 5,				//poke
	DISP_SUB_CARD_PAL	= 4,				//l_oam00
	DISP_SUB2_BUTTON_PAL= 4,				//l_oam01
	DISP_SUB3_ATARI_PAL = 1,				//l_oam02
	
	//DISP_PAL_NUM		= 15,
	DISP_PAL_NUM		= 14,
};
//パレットの使っている1本だけの場所に、
//次のパレット1本分を上書きでよいが、
//リソースのパレットをオフセット指定がどの関数が見あたらなかったので、
//別リソースに分けて、リソース切り替えにした


//==============================================================================================
//
//	データ
//
//==============================================================================================
//リソースタイプ列挙に合わせる(include/system/clact_util_res.h)
//リソースマネージャー登録数テーブル
static const u8 ResEntryNumTbl[SCRATCH_RESOURCE_NUM] = {
#if 0
	2,			//キャラリソース
	3,			//パレットリソース
	2,			//セルリソース
	2,			//セルアニメリソース
#else
	SCRATCH_RES_OBJ_MAX,		//キャラリソース
	SCRATCH_RES_OBJ_MAX,		//パレットリソース
	SCRATCH_RES_OBJ_MAX,		//セルリソース
	SCRATCH_RES_OBJ_MAX,		//セルアニメリソース
#endif
};


//==============================================================================================
//
//	プロトタイプ宣言
//
//==============================================================================================
//ScratchClact関数
void			ScratchClact_InitCellActor( SCRATCH_CLACT* wk );
CLACT_WORK_PTR ScratchClact_SetActor( SCRATCH_CLACT* wk, u32 data_no, u32 anm_no, u32 bg_pri, u32 pri, u8 disp );
void			ScratchClact_DeleteCellObject( SCRATCH_CLACT* wk );
void			ScratchClact_ResButtonChg( SCRATCH_CLACT* wk );
void			ScratchClact_ButtonResourcePalChg( SCRATCH_CLACT* wk, int no );

//static
static void		InitCharPlttManager( void );
static void		ScratchClact_ResPokeSet( SCRATCH_CLACT* wk, int res_id, int obj_id, int vram );
static void		ScratchClact_ResCardSet( SCRATCH_CLACT* wk, int vram );
static void		ScratchClact_ResButtonSet( SCRATCH_CLACT* wk, int vram );
static void		ScratchClact_ResAtariSet( SCRATCH_CLACT* wk, int vram );
static void		ScratchClact_DeleteCellObjectOne( SCRATCH_CLACT* wk, int res_id );


//==============================================================================================
//
//	関数
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	セルアクター初期化
 *
 * @param	wk		SCRATCH_CLACT型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
void ScratchClact_InitCellActor( SCRATCH_CLACT* wk )
{
	int i;
	ARCHANDLE* p_handle;

	initVramTransferManagerHeap( 32, HEAPID_SCRATCH );

	//キャラクター・パレットマネージャー初期化
	InitCharPlttManager();

	//REND_OAM_UtilOamRamClear_Main( HEAPID_SCRATCH );
	//REND_OAM_UtilOamRamClear_Sub( HEAPID_SCRATCH );	

	//OAMマネージャーの初期化
	NNS_G2dInitOamManagerModule();

	//共有OAMマネージャ作成
	//レンダラ用OAMマネージャ作成
	//ここで作成したOAMマネージャをみんなで共有する
	REND_OAMInit(	0, 128,				//メイン画面OAM管理領域
					0, 32,				//メイン画面アフィン管理領域
					0, 128,				//サブ画面OAM管理領域
					0, 32,				//サブ画面アフィン管理領域
					HEAPID_SCRATCH);
	
	//セルアクターセットの簡単初期化(作成されたセルアクターセットが返る)
	wk->ClactSet = CLACT_U_SetEasyInit( SCRATCH_CLACT_OBJ_MAX, &wk->RendData, HEAPID_SCRATCH );
	
	//CLACTで定義しているセルが大きすぎてサブ画面に影響がでてしまうので離してみる
	CLACT_U_SetSubSurfaceMatrix( &wk->RendData, 0, SCRATCH_SUB_ACTOR_DISTANCE );

	//リソースマネージャー初期化
	for( i=0; i < SCRATCH_RESOURCE_NUM ;i++ ){		//リソースマネージャー作成
		wk->ResMan[i] = CLACT_U_ResManagerInit( ResEntryNumTbl[i], i, HEAPID_SCRATCH );
	}

	/***************/
	//	下画面
	/***************/
	ScratchClact_ResCardSet( wk, NNS_G2D_VRAM_TYPE_2DMAIN );					//カード
	ScratchClact_ResPokeSet( wk, SCRATCH_RES_OBJ_D_POKE, 
							 SCRATCH_ID_OBJ_D_POKE, NNS_G2D_VRAM_TYPE_2DMAIN );	//ポケモン絵柄
	ScratchClact_ResButtonSet( wk, NNS_G2D_VRAM_TYPE_2DMAIN );					//ボタン
	ScratchClact_ResAtariSet( wk, NNS_G2D_VRAM_TYPE_2DMAIN );					//当たり
	
	/***************/
	//	上画面
	/***************/
	ScratchClact_ResPokeSet( wk, SCRATCH_RES_OBJ_U_POKE, 
							 SCRATCH_ID_OBJ_U_POKE, NNS_G2D_VRAM_TYPE_2DSUB );	//ポケモン絵柄

	//リソースマネージャーから転送
	for( i=0; i < SCRATCH_RES_OBJ_MAX ;i++ ){
		CLACT_U_CharManagerSet( wk->ResObjTbl[i][CLACT_U_CHAR_RES] );	//Char転送
		CLACT_U_PlttManagerSet( wk->ResObjTbl[i][CLACT_U_PLTT_RES] );	//パレット転送
	}

	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );			//SUB DISP OBJ ON
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );			//MAIN DISP OBJ ON

	//ArchiveDataHandleClose( p_handle );	
	return;
}

//--------------------------------------------------------------
/**
 * @brief	リソース：カードセット
 *
 * @param	wk		SCRATCH_CLACT型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void ScratchClact_ResCardSet( SCRATCH_CLACT* wk, int vram )
{
	//chara読み込み
	wk->ResObjTbl[SCRATCH_RES_OBJ_D_CARD][CLACT_U_CHAR_RES] = CLACT_U_ResManagerResAddArcChar(
					wk->ResMan[CLACT_U_CHAR_RES],
					ARC_SCRATCH, NARC_scratch_l_oam00_NCGR,
					FALSE, SCRATCH_ID_OBJ_D_CARD, NNS_G2D_VRAM_TYPE_2DMAIN, HEAPID_SCRATCH);

	//pal読み込み
	wk->ResObjTbl[SCRATCH_RES_OBJ_D_CARD][CLACT_U_PLTT_RES] = CLACT_U_ResManagerResAddArcPltt(
					wk->ResMan[CLACT_U_PLTT_RES],
					ARC_SCRATCH, NARC_scratch_l_oam00_NCLR,
					FALSE, SCRATCH_ID_OBJ_D_CARD, NNS_G2D_VRAM_TYPE_2DMAIN, 
					DISP_SUB_CARD_PAL, HEAPID_SCRATCH);

	//cell読み込み
	wk->ResObjTbl[SCRATCH_RES_OBJ_D_CARD][CLACT_U_CELL_RES] = CLACT_U_ResManagerResAddArcKindCell(
					wk->ResMan[CLACT_U_CELL_RES],
					ARC_SCRATCH, NARC_scratch_l_oam00_NCER,
					FALSE, SCRATCH_ID_OBJ_D_CARD, CLACT_U_CELL_RES, HEAPID_SCRATCH);

	//同じ関数でanim読み込み
	wk->ResObjTbl[SCRATCH_RES_OBJ_D_CARD][CLACT_U_CELLANM_RES]=CLACT_U_ResManagerResAddArcKindCell(
					wk->ResMan[CLACT_U_CELLANM_RES],
					ARC_SCRATCH, NARC_scratch_l_oam00_NANR,
					FALSE, SCRATCH_ID_OBJ_D_CARD, CLACT_U_CELLANM_RES, HEAPID_SCRATCH);

	return;
}

//--------------------------------------------------------------
/**
 * @brief	リソース：ポケモン絵柄セット
 *
 * @param	wk		SCRATCH_CLACT型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void ScratchClact_ResPokeSet( SCRATCH_CLACT* wk, int res_id, int obj_id, int vram )
{
	//chara読み込み
	wk->ResObjTbl[res_id][CLACT_U_CHAR_RES] = CLACT_U_ResManagerResAddArcChar(
					wk->ResMan[CLACT_U_CHAR_RES],
					ARC_SCRATCH, NARC_scratch_poke_NCGR,
					0, obj_id, vram, HEAPID_SCRATCH);

	//pal読み込み
	wk->ResObjTbl[res_id][CLACT_U_PLTT_RES] = CLACT_U_ResManagerResAddArcPltt(
					wk->ResMan[CLACT_U_PLTT_RES],
					ARC_SCRATCH, NARC_scratch_poke_NCLR,
					FALSE, obj_id, vram, 
					DISP_MAIN_OBJ_PAL,HEAPID_SCRATCH);

	//cell読み込み
	wk->ResObjTbl[res_id][CLACT_U_CELL_RES] = CLACT_U_ResManagerResAddArcKindCell(
					wk->ResMan[CLACT_U_CELL_RES],
					ARC_SCRATCH, NARC_scratch_poke_NCER,
					0, obj_id, CLACT_U_CELL_RES, HEAPID_SCRATCH);

	//同じ関数でanim読み込み
	wk->ResObjTbl[res_id][CLACT_U_CELLANM_RES] = CLACT_U_ResManagerResAddArcKindCell(
					wk->ResMan[CLACT_U_CELLANM_RES],
					ARC_SCRATCH, NARC_scratch_poke_NANR,
					0, obj_id, CLACT_U_CELLANM_RES, HEAPID_SCRATCH);

	return;
}

//--------------------------------------------------------------
/**
 * @brief	リソース切り替え(カード→ボタン)
 *
 * @param	wk		SCRATCH_CLACT型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
void ScratchClact_ResButtonChg( SCRATCH_CLACT* wk )
{
	ScratchClact_DeleteCellObjectOne( wk, SCRATCH_RES_OBJ_D_CARD );	//カードリソース削除
	ScratchClact_ResButtonSet( wk, NNS_G2D_VRAM_TYPE_2DMAIN );		//ボタンリソース追加

	CLACT_U_CharManagerSet( wk->ResObjTbl[SCRATCH_RES_OBJ_D_BUTTON][CLACT_U_CHAR_RES] );//Char転送
	CLACT_U_PlttManagerSet( wk->ResObjTbl[SCRATCH_RES_OBJ_D_BUTTON][CLACT_U_PLTT_RES] );//pltt転送
	return;
}

//--------------------------------------------------------------
/**
 * @brief	リソース：ボタンセット
 *
 * @param	wk		SCRATCH_CLACT型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void ScratchClact_ResButtonSet( SCRATCH_CLACT* wk, int vram )
{
	//chara読み込み
	wk->ResObjTbl[SCRATCH_RES_OBJ_D_BUTTON][CLACT_U_CHAR_RES] = CLACT_U_ResManagerResAddArcChar(
					wk->ResMan[CLACT_U_CHAR_RES],
					ARC_SCRATCH, NARC_scratch_l_oam01_NCGR,
					FALSE, SCRATCH_ID_OBJ_D_BUTTON, NNS_G2D_VRAM_TYPE_2DMAIN, HEAPID_SCRATCH);

	//pal読み込み
	wk->ResObjTbl[SCRATCH_RES_OBJ_D_BUTTON][CLACT_U_PLTT_RES] = CLACT_U_ResManagerResAddArcPltt(
					wk->ResMan[CLACT_U_PLTT_RES],
					ARC_SCRATCH, NARC_scratch_l_oam01_NCLR,
					FALSE, SCRATCH_ID_OBJ_D_BUTTON, NNS_G2D_VRAM_TYPE_2DMAIN, 
					DISP_SUB2_BUTTON_PAL, HEAPID_SCRATCH);

	//cell読み込み
	wk->ResObjTbl[SCRATCH_RES_OBJ_D_BUTTON][CLACT_U_CELL_RES] = CLACT_U_ResManagerResAddArcKindCell(
					wk->ResMan[CLACT_U_CELL_RES],
					ARC_SCRATCH, NARC_scratch_l_oam01_NCER,
					FALSE, SCRATCH_ID_OBJ_D_BUTTON, CLACT_U_CELL_RES, HEAPID_SCRATCH);

	//同じ関数でanim読み込み
	wk->ResObjTbl[SCRATCH_RES_OBJ_D_BUTTON][CLACT_U_CELLANM_RES] = CLACT_U_ResManagerResAddArcKindCell(
					wk->ResMan[CLACT_U_CELLANM_RES],
					ARC_SCRATCH, NARC_scratch_l_oam01_NANR,
					FALSE, SCRATCH_ID_OBJ_D_BUTTON, CLACT_U_CELLANM_RES, HEAPID_SCRATCH);

	return;
}

//--------------------------------------------------------------
/**
 * @brief	リソース：当たりセット
 *
 * @param	wk		SCRATCH_CLACT型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void ScratchClact_ResAtariSet( SCRATCH_CLACT* wk, int vram )
{
	//chara読み込み
	wk->ResObjTbl[SCRATCH_RES_OBJ_D_ATARI][CLACT_U_CHAR_RES] = CLACT_U_ResManagerResAddArcChar(
					wk->ResMan[CLACT_U_CHAR_RES],
					ARC_SCRATCH, NARC_scratch_l_oam02_NCGR,
					FALSE, SCRATCH_ID_OBJ_D_ATARI, NNS_G2D_VRAM_TYPE_2DMAIN, HEAPID_SCRATCH);

	//pal読み込み
	wk->ResObjTbl[SCRATCH_RES_OBJ_D_ATARI][CLACT_U_PLTT_RES] = CLACT_U_ResManagerResAddArcPltt(
					wk->ResMan[CLACT_U_PLTT_RES],
					ARC_SCRATCH, NARC_scratch_l_oam02_NCLR,
					FALSE, SCRATCH_ID_OBJ_D_ATARI, NNS_G2D_VRAM_TYPE_2DMAIN, 
					DISP_SUB3_ATARI_PAL, HEAPID_SCRATCH);

	//cell読み込み
	wk->ResObjTbl[SCRATCH_RES_OBJ_D_ATARI][CLACT_U_CELL_RES] = CLACT_U_ResManagerResAddArcKindCell(
					wk->ResMan[CLACT_U_CELL_RES],
					ARC_SCRATCH, NARC_scratch_l_oam02_NCER,
					FALSE, SCRATCH_ID_OBJ_D_ATARI, CLACT_U_CELL_RES, HEAPID_SCRATCH);

	//同じ関数でanim読み込み
	wk->ResObjTbl[SCRATCH_RES_OBJ_D_ATARI][CLACT_U_CELLANM_RES]=CLACT_U_ResManagerResAddArcKindCell(
					wk->ResMan[CLACT_U_CELLANM_RES],
					ARC_SCRATCH, NARC_scratch_l_oam02_NANR,
					FALSE, SCRATCH_ID_OBJ_D_ATARI, CLACT_U_CELLANM_RES, HEAPID_SCRATCH);

	return;
}

//--------------------------------------------------------------
/**
 * @brief	セルアクターをセット
 *
 * @param	wk		SCRATCH_CLACT型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
CLACT_WORK_PTR ScratchClact_SetActor( SCRATCH_CLACT* wk, u32 data_no, u32 anm_no, u32 bg_pri, u32 pri, u8 disp )
{
	int i;
	CLACT_HEADER cl_act_header;
	CLACT_WORK_PTR act;
	
	//セルアクターヘッダ作成	
	CLACT_U_MakeHeader(	&cl_act_header,
						//登録したリソースのID(ResObjTbl[id])
						data_no, data_no, data_no, data_no,
						CLACT_U_HEADER_DATA_NONE, CLACT_U_HEADER_DATA_NONE,
						//0, 0,							//VRAM転送かフラグ、BGとの優先順位
						0, bg_pri,						//VRAM転送かフラグ、BGとの優先順位(2)
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
		add.heap		= HEAPID_SCRATCH;

		if( disp == DISP_MAIN ){
			add.DrawArea= NNS_G2D_VRAM_TYPE_2DMAIN;
		}else{
			add.DrawArea= NNS_G2D_VRAM_TYPE_2DSUB;
		}

		//if( disp == DISP_SUB ){
		//	add.mat.y += SUB_SURFACE_Y;				//座標を補正
		//	add.mat.y += SCRATCH_SUB_ACTOR_DISTANCE;//座標を補正
		//}

		//セルアクター表示開始
		act = CLACT_Add( &add );
		//if( act == NULL ){
		//	OS_Printf( "CLACT_Add失敗\n" );
		//	GF_ASSERT(0);
		//}

#if 0
		CLACT_SetAnmFlag( act, 1 );					//オートアニメ
#else
		CLACT_SetAnmFlag( act, 0 );					//非アニメ
#endif

		CLACT_SetAnmFrame( act, FX32_ONE );			//オートアニメーションフレームを設定
		CLACT_AnmChg( act, anm_no );				//アニメーションのシーケンスをチェンジする
	}	

	return act;
}

//--------------------------------------------------------------
/**
 * @brief	2Dセルオブジェクト解放
 *
 * @param	wk		SCRATCH_CLACT型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
void ScratchClact_DeleteCellObject(SCRATCH_CLACT* wk)
{
	u8 i;

	//セルアクターリソース解放
	for( i=0; i < SCRATCH_RES_OBJ_MAX ;i++ ){
		CLACT_U_CharManagerDelete( wk->ResObjTbl[i][CLACT_U_CHAR_RES] );//char転送マネージャー破棄
		CLACT_U_PlttManagerDelete( wk->ResObjTbl[i][CLACT_U_PLTT_RES] );//pltt転送マネージャー破棄
	}
		
	//キャラ・パレット・セル・セルアニメのリソースマネージャー破棄
	for( i=0; i < SCRATCH_RESOURCE_NUM ;i++ ){
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
			HEAPID_SCRATCH						//使用するヒープ
		};
		//InitCharManager( &cm );				//OBJマッピングモード設定
		//InitCharManagerReg( &cm, GX_OBJVRAMMODE_CHAR_1D_32K, GX_OBJVRAMMODE_CHAR_1D_32K );
		//InitCharManagerReg( &cm, GX_OBJVRAMMODE_CHAR_1D_64K, GX_OBJVRAMMODE_CHAR_1D_64K );
		InitCharManagerReg( &cm, GX_OBJVRAMMODE_CHAR_1D_128K, GX_OBJVRAMMODE_CHAR_1D_128K );
	}

	//パレットマネージャー初期化
#if 0
	InitPlttManager( PLTT_CONT_NUM, HEAPID_SCRATCH );
#else
	InitPlttManager( DISP_PAL_NUM, HEAPID_SCRATCH );
#endif

	//読み込み開始位置を初期化
	CharLoadStartAll();
	PlttLoadStartAll();

	return;
}

//--------------------------------------------------------------
/**
 * @brief	パレット変更
 *
 * @param	wk		SCRATCH_CLACT型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
//ワクのパレットリソースのインデックス
static u8 waku_pal_index_tbl[WAKU_PAL_NUM] = {
	NARC_scratch_l_oam_waku01_NCLR,
	NARC_scratch_l_oam_waku02_NCLR,
	NARC_scratch_l_oam_waku03_NCLR,
	NARC_scratch_l_oam_waku04_NCLR,
	NARC_scratch_l_oam_waku05_NCLR,
	NARC_scratch_l_oam_waku06_NCLR,
	NARC_scratch_l_oam_waku07_NCLR,
	NARC_scratch_l_oam_waku08_NCLR,
};

#if 1
void ScratchClact_ButtonResourcePalChg( SCRATCH_CLACT* wk, int no )
{
	CLACT_U_RES_OBJ_PTR	obj;

	if( no >= WAKU_PAL_NUM ){
		GF_ASSERT(0);
	}
	
	obj = CLACT_U_ResManagerGetIDResObjPtr(wk->ResMan[CLACT_U_PLTT_RES], SCRATCH_RES_OBJ_D_BUTTON);

	//パレットデータのリソースを変更
	CLACT_U_ResManagerResChgArcPltt(wk->ResMan[CLACT_U_PLTT_RES],
									obj,
									ARC_SCRATCH, waku_pal_index_tbl[no],
									FALSE, HEAPID_SCRATCH );

	//この関数から、CharDataChg→AddVramTransferManagerが呼ばれる(gflib/vram_transfer_manager.c)
	CLACT_U_PlttManagerReTrans( obj );
	return;
}
#endif

//--------------------------------------------------------------
/**
 * @brief	リソース削除
 *
 * @param	wk		SCRATCH_CLACT型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void ScratchClact_DeleteCellObjectOne( SCRATCH_CLACT* wk, int res_id )
{
	u8 i;

	//セルアクターリソース解放
	CLACT_U_CharManagerDelete( wk->ResObjTbl[res_id][CLACT_U_CHAR_RES] );//char転送マネージャー破棄
	CLACT_U_PlttManagerDelete( wk->ResObjTbl[res_id][CLACT_U_PLTT_RES] );//pltt転送マネージャー破棄
		
	CLACT_U_ResManagerResDelete( wk->ResMan[res_id], wk->ResObjTbl[res_id][CLACT_U_CHAR_RES] );
	CLACT_U_ResManagerResDelete( wk->ResMan[res_id], wk->ResObjTbl[res_id][CLACT_U_PLTT_RES] );
	CLACT_U_ResManagerResDelete( wk->ResMan[res_id], wk->ResObjTbl[res_id][CLACT_U_CELL_RES] );
	CLACT_U_ResManagerResDelete( wk->ResMan[res_id], wk->ResObjTbl[res_id][CLACT_U_CELLANM_RES] );
	return;
}



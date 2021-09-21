//==============================================================================================
/**
 * @file	factory_clact.c
 * @brief	「バトルファクトリー」セルアクター
 * @author	Satoshi Nohara
 * @date	2007.03.14
 */
//==============================================================================================
#include "common.h"
#include "src_os_print.h"
#include "system/arc_tool.h"
#include "system/arc_util.h"

#include "factory_sys.h"
#include "factory_clact.h"
#include "../graphic/frontier_obj_def.h"
#include "../graphic/frontier_bg_def.h"


//==============================================================================================
//
//	定義
//
//==============================================================================================
//キャラクターマネージャー
#define CHAR_CONT_NUM						(32)//(3)		//キャラクタ制御数
#define CHAR_VRAMTRANS_MAIN_SIZE			(2048)
#define CHAR_VRAMTRANS_SUB_SIZE				(2048)
//#define CHAR_VRAMTRANS_MAIN_SIZE			(1024)
//#define CHAR_VRAMTRANS_SUB_SIZE				(1024)
//#define CHAR_VRAMTRANS_MAIN_SIZE			(3096)
//#define CHAR_VRAMTRANS_SUB_SIZE				(3096)

#define DISP_MAIN_OBJ_PAL					(8)

//パレットマネージャー
#define PLTT_CONT_NUM						(DISP_MAIN_OBJ_PAL)		//管理パレット数


//==============================================================================================
//
//	データ
//
//==============================================================================================
//リソースタイプ列挙に合わせる(include/system/clact_util_res.h)
//リソースマネージャー登録数テーブル
static const u8 ResEntryNumTbl[FACTORY_RESOURCE_NUM] = {
#if 0
	2,			//キャラリソース
	3,			//パレットリソース
	2,			//セルリソース
	2,			//セルアニメリソース
#else
	FACTORY_RES_OBJ_MAX,		//キャラリソース
	FACTORY_RES_OBJ_MAX,		//パレットリソース
	FACTORY_RES_OBJ_MAX,		//セルリソース
	FACTORY_RES_OBJ_MAX,		//セルアニメリソース
#endif
};


//==============================================================================================
//
//	プロトタイプ宣言
//
//==============================================================================================
//FactoryClact関数
void			FactoryClact_InitCellActor( FACTORY_CLACT* wk );
CLACT_WORK_PTR	FactoryClact_SetActor( FACTORY_CLACT* wk, u32 data_no, u32 anm_no, u32 pri, int bg_pri, u8 disp );
void			FactoryClact_DeleteCellObject( FACTORY_CLACT* wk );

//static
static void		InitCharPlttManager( void );


//==============================================================================================
//
//	関数
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	セルアクター初期化
 *
 * @param	wk		FACTORY_CLACT型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
void FactoryClact_InitCellActor( FACTORY_CLACT* wk )
{
	int i;

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
					HEAPID_FACTORY);
	
	//セルアクターセットの簡単初期化(作成されたセルアクターセットが返る)
	wk->ClactSet = CLACT_U_SetEasyInit( FACTORY_CLACT_OBJ_MAX, &wk->RendData, HEAPID_FACTORY );
	
	//リソースマネージャー初期化
	for( i=0; i < FACTORY_RESOURCE_NUM ;i++ ){		//リソースマネージャー作成
		wk->ResMan[i] = CLACT_U_ResManagerInit( ResEntryNumTbl[i], i, HEAPID_FACTORY );
	}

	/***************/
	//	下画面
	/***************/

	/***************/
	//	上画面
	/***************/
	//chara読み込み
	wk->ResObjTbl[FACTORY_RES_OBJ_BALL][CLACT_U_CHAR_RES] = CLACT_U_ResManagerResAddArcChar(
					wk->ResMan[CLACT_U_CHAR_RES],
					ARC_FRONTIER_OBJ, BF_RENTAL_OAM_NCGR_BIN,
					1, FACTORY_ID_OBJ_BALL, NNS_G2D_VRAM_TYPE_2DMAIN, HEAPID_FACTORY);

	//pal読み込み
	wk->ResObjTbl[FACTORY_RES_OBJ_BALL][CLACT_U_PLTT_RES] = CLACT_U_ResManagerResAddArcPltt(
			wk->ResMan[CLACT_U_PLTT_RES],
			ARC_FRONTIER_OBJ, BF_RENTAL_OAM_NCLR,
			FALSE, FACTORY_ID_OBJ_BALL, NNS_G2D_VRAM_TYPE_2DMAIN, DISP_MAIN_OBJ_PAL,HEAPID_FACTORY);

	//cell読み込み
	wk->ResObjTbl[FACTORY_RES_OBJ_BALL][CLACT_U_CELL_RES] = CLACT_U_ResManagerResAddArcKindCell(
					wk->ResMan[CLACT_U_CELL_RES],
					ARC_FRONTIER_OBJ, BF_RENTAL_OAM_NCER_BIN,
					1, FACTORY_ID_OBJ_BALL, CLACT_U_CELL_RES, HEAPID_FACTORY);

	//同じ関数でanim読み込み
	wk->ResObjTbl[FACTORY_RES_OBJ_BALL][CLACT_U_CELLANM_RES] = CLACT_U_ResManagerResAddArcKindCell(
					wk->ResMan[CLACT_U_CELLANM_RES],
					ARC_FRONTIER_OBJ, BF_RENTAL_OAM_NANR_BIN,
					1, FACTORY_ID_OBJ_BALL, CLACT_U_CELLANM_RES, HEAPID_FACTORY);

	//リソースマネージャーから転送
	for( i=0; i < FACTORY_RES_OBJ_MAX ;i++ ){
		CLACT_U_CharManagerSet( wk->ResObjTbl[i][CLACT_U_CHAR_RES] );	//Char転送
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
 * @param	wk		FACTORY_CLACT型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
CLACT_WORK_PTR FactoryClact_SetActor( FACTORY_CLACT* wk, u32 data_no, u32 anm_no, u32 pri, int bg_pri, u8 disp )
{
	int i;
	CLACT_HEADER cl_act_header;
	CLACT_WORK_PTR act;
	
	//セルアクターヘッダ作成	
	CLACT_U_MakeHeader(	&cl_act_header,
						//登録したリソースのID(ResObjTbl[id])
						data_no, data_no, data_no, data_no,
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

		if( disp == DISP_MAIN ){
			add.DrawArea= NNS_G2D_VRAM_TYPE_2DMAIN;
		}else{
			add.DrawArea= NNS_G2D_VRAM_TYPE_2DSUB;
		}

		add.heap		= HEAPID_FACTORY;

		if( disp == DISP_SUB ){						//座標を補正
			add.mat.y += SUB_SURFACE_Y;
		}

		//セルアクター表示開始
		act = CLACT_Add(&add);
		//CLACT_SetAnmFlag( act, 0 );				//非アニメ
		CLACT_SetAnmFlag( act, 1 );					//オートアニメ
		CLACT_AnmChg( act, anm_no );
	}	

	return act;
}

//--------------------------------------------------------------
/**
 * @brief	2Dセルオブジェクト解放
 *
 * @param	wk		FACTORY_CLACT型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
void FactoryClact_DeleteCellObject(FACTORY_CLACT* wk)
{
	u8 i;

	//セルアクターリソース解放
	for( i=0; i < FACTORY_RES_OBJ_MAX ;i++ ){
		CLACT_U_CharManagerDelete( wk->ResObjTbl[i][CLACT_U_CHAR_RES] );//char転送マネージャー破棄
		CLACT_U_PlttManagerDelete( wk->ResObjTbl[i][CLACT_U_PLTT_RES] );//pltt転送マネージャー破棄
	}
		
	//キャラ・パレット・セル・セルアニメのリソースマネージャー破棄
	for( i=0; i < FACTORY_RESOURCE_NUM ;i++ ){
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
			HEAPID_FACTORY						//使用するヒープ
		};
		//InitCharManager(&cm);					//OBJマッピングモード設定
		//InitCharManagerReg( &cm, GX_OBJVRAMMODE_CHAR_1D_32K, GX_OBJVRAMMODE_CHAR_1D_32K );
		InitCharManagerReg( &cm, GX_OBJVRAMMODE_CHAR_1D_64K, GX_OBJVRAMMODE_CHAR_1D_64K );
		//InitCharManagerReg( &cm, GX_OBJVRAMMODE_CHAR_1D_128K, GX_OBJVRAMMODE_CHAR_1D_128K );
	}

	//パレットマネージャー初期化
	InitPlttManager( PLTT_CONT_NUM, HEAPID_FACTORY );

	//読み込み開始位置を初期化
	CharLoadStartAll();
	PlttLoadStartAll();

	return;
}



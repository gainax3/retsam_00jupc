//=============================================================================
/**
 * @file	opening_demo_obj.c
 * @brief	オープニングデモＯＢＪ
 * @author	Nozomu Saito
 * @date    2006.06.03
 */
//=============================================================================
#include "common.h"
#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "op_demo.naix"
#include "opening_kira_obj.h"
#include "opening_demo_obj.h"
#include "op_frame_def.h"


#define RESOURCE_NUM	(4)
#define PERSON_ACT_MAX	(3 + 1 + 1 + 2 + 4)	//プラチナでサブ画面主人公男+博士＋ボール(2個)+流星4個を追加
#define BIRD_ACT_MAX	(6)
#define KIRA_ACT_MAX	(16)

#define OPD_CHAR_CONT_NUM				(ACT_ID_MAX)
#define OPD_CHAR_VRAMTRANS_MAIN_SIZE	(0x8000)
#define OPD_CHAR_VRAMTRANS_SUB_SIZE		(0x4000)
#define OPD_PLTT_CONT_NUM				(ACT_ID_MAX)


#define PALETTE_NUM	(1)		//パレット１本

#define OP_DEMO_ACT_MAX	( PERSON_ACT_MAX+BIRD_ACT_MAX+KIRA_ACT_MAX )
#define CLACT_WORK_MAX	( PERSON_ACT_MAX+BIRD_ACT_MAX )
SDK_COMPILER_ASSERT(CLACT_WORK_MAX == ACT_WORK_MAX);	//+=プラチナ追加分

///Vブランク転送アニメを使用するアクターヘッダのインデックス
enum{
	VBLANK_TRANS_ANM_HERO,
	VBLANK_TRANS_ANM_HEROINE,
	VBLANK_TRANS_ANM_HERO_SUB,
	
	VBLANK_TRANS_ANM_MAX,
};

#define OPDEMO_VRAM_TRANSFER_TASK_NUM		(VBLANK_TRANS_ANM_MAX * 2)	//一応2倍持つ

#define HERO_POS_X		(128-48)	//男主人公のＸ位置（ドット）
#define HEROINE_POS_X	(128+48)	//女主人公のＸ位置（ドット）
#define HERO_POS_Y		(14*8)		//男主人公のＹ位置（ドット）
#define HEROINE_POS_Y	(14*8)		//女主人公のＹ位置（ドット）
#define RIVAL_POS_X		(-64)	//ライバルのＸ位置（ドット）
#define RIVAL_POS_Y		(14*8)		//ライバルのＹ位置（ドット）
#define RIVAL_POS_END_X	(FX32_ONE*(HEROINE_POS_X))	//スライドイン終了時のライバル位置（ドット）
#define HAKASE_POS_X	(256+64)	//博士のＸ位置（ドット）
#define HAKASE_POS_Y	(14*8)		//博士のＹ位置（ドット）
#define HAKASE_POS_END_X	(FX32_ONE*(HERO_POS_X))	//スライドイン終了時の博士位置（ドット）
#define BALL_POS_X		(128)		//ボールのＸ位置（ドット）
#define BALL_POS_Y		(12*8)		//ボールのＹ位置（ドット）
#define STREAM_POS_X	(128)		//流星のＸ位置（ドット）
#define STREAM_POS_Y	(0xd*8)		//流星のＹ位置（ドット）
#define BIRD_1_POS_X	(256+8)		//鳥1のＸ位置（ドット）
#define BIRD_1_POS_Y	(16*8)		//鳥1のＹ位置（ドット）
#define BIRD_2_POS_X	(256+32)	//鳥2のＸ位置（ドット）
#define BIRD_2_POS_Y	(14*8)		//鳥2のＹ位置（ドット）
#define BIRD_3_POS_X	(256+32)	//鳥3のＸ位置（ドット）
#define BIRD_3_POS_Y	(18*8)		//鳥3のＹ位置（ドット）

#define BIRD_4_POS_X	(256+32)		//鳥4のＸ位置（ドット）
#define BIRD_4_POS_Y	(20*8)		//鳥4のＹ位置（ドット）
#define BIRD_5_POS_X	(256+32)	//鳥5のＸ位置（ドット）
#define BIRD_5_POS_Y	(18*8)		//鳥5のＹ位置（ドット）
#define BIRD_6_POS_X	(256+32)	//鳥6のＸ位置（ドット）
#define BIRD_6_POS_Y	(22*8)		//鳥6のＹ位置（ドット）

#define HERO_SLIDE_SPEED	(0x0200)	//fx32
#define HEROINE_SLIDE_SPEED	(HERO_SLIDE_SPEED)

#define RIVAL_SLIDE_SPEED	(FX32_ONE * 18)	//(FX32_ONE*14)
#define HAKASE_SLIDE_SPEED	(-RIVAL_SLIDE_SPEED)

#define BALL_ROTATE_SPEED	(0x1c00)	//(0x1800)

typedef enum {
	ACT_ID_HERO,
	ACT_ID_HERO_SUB,
	ACT_ID_HEROINE,
	ACT_ID_RIVAL,
	ACT_ID_BIRD,
	ACT_ID_KIRA,
	ACT_ID_HAKASE,
	ACT_ID_BALL_MAIN,
	ACT_ID_BALL_SUB,
	ACT_ID_STREAM_MAIN,
	ACT_ID_STREAM_SUB,

	ACT_ID_MAX,
};

typedef struct OP_DEMO_OBJ_WORK_tag
{
	CLACT_SET_PTR 			ClactSet;								// セルアクターセット
	CLACT_U_EASYRENDER_DATA	RendData;								// 簡易レンダーデータ
	CLACT_U_RES_MANAGER_PTR	ResMan[RESOURCE_NUM];				// リソースマネージャ
	CLACT_U_RES_OBJ_PTR 	ResObjTbl[ACT_ID_MAX][RESOURCE_NUM];		// リソースオブジェテーブル
	CLACT_WORK_PTR			ClActWork[CLACT_WORK_MAX];				// セルアクターワークポインタ配列
	CLACT_HEADER			vblank_trans_anm_header[VBLANK_TRANS_ANM_MAX];	//Vブランク転送アニメを使用するアクターのヘッダ
	NNSG2dCellTransferState *cell_transfer_state;	//Vブランク転送アニメバッファ
}OP_DEMO_OBJ_WORK;


//リソースマネージャー登録数テーブル
static const u8 ResEntryNumTbl[RESOURCE_NUM] = {
	ACT_ID_MAX,			//キャラリソース
	ACT_ID_MAX,			//パレットリソース
	ACT_ID_MAX,			//セルリソース
	ACT_ID_MAX,			//セルアニメリソース
};

//アクターＩＤとアクターワークの対応表
static int ActIDTbl[CLACT_WORK_MAX] = {
	ACT_ID_HERO,			//主人公男
	ACT_ID_HERO_SUB,		//主人公男(サブ画面)
	ACT_ID_HEROINE,			//主人公女
	ACT_ID_RIVAL,			//ライバル
	ACT_ID_BIRD,			//鳥
	ACT_ID_BIRD,			//鳥
	ACT_ID_BIRD,			//鳥
	ACT_ID_KIRA,			//キラキラ
	ACT_ID_HAKASE,			//博士
	ACT_ID_BALL_MAIN,		//ボール(メイン画面)
	ACT_ID_BALL_SUB,		//ボール(サブ画面)
};

static u16 BlackPalette[16] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

static fx32 BirdXSpeed1[3] = {
	(FX32_ONE/2)*12,(FX32_ONE/2)*11,(FX32_ONE/2)*10,
};

static fx32 BirdXSpeed2[1] = {
	(FX32_ONE/2)*10
};

static fx32 BirdXSpeed3[2] = {
	(FX32_ONE/2)*8,(FX32_ONE/2)*11,
};

//-------------------------------------
//
//	キャラクタマネージャー
//	パレットマネージャーの初期化
//
//=====================================
static void OpdInitCharPlttManager()
{
	const int heapid = HEAPID_OP_DEMO;
	// キャラクタマネージャー初期化
	{
		CHAR_MANAGER_MAKE cm = {
			OPD_CHAR_CONT_NUM,
			OPD_CHAR_VRAMTRANS_MAIN_SIZE,
			OPD_CHAR_VRAMTRANS_SUB_SIZE,
			heapid
		};
		InitCharManager(&cm);
	}
	// パレットマネージャー初期化
	InitPlttManager(OPD_PLTT_CONT_NUM, heapid);

	// 読み込み開始位置を初期化
	CharLoadStartAll();
	PlttLoadStartAll();

	//Vブランク転送アニメ用
	initVramTransferManagerHeap(OPDEMO_VRAM_TRANSFER_TASK_NUM, heapid);
}

//--------------------------------------------------------------------------------------------
/**
 * セルアクターヘッダ作成
 *
 * @param	inActID		登録ＩＤ
 * @param	wk			オープニングＯＢＪデータポインタ
 * @param	outAdd		アクターヘッダ格納場所
 * @param   vram_trans	TRUE:VRAM転送アニメ
 * @param   draw_area	NNS_G2D_VRAM_TYPE_2DMAIN or NNS_G2D_VRAM_TYPE_2DSUB
 * 
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void MakeClActHeader(const int inActID, OPD_OBJ_PTR wk,
							const int inPriority,
							CLACT_ADD *outAdd, CLACT_HEADER *outClActHeader,
							int vram_trans, int draw_area)
{
	// セルアクターヘッダ作成	
	CLACT_U_MakeHeader(	outClActHeader,
						inActID, inActID, inActID, inActID,
						CLACT_U_HEADER_DATA_NONE, CLACT_U_HEADER_DATA_NONE,
						vram_trans, inPriority,
						wk->ResMan[CLACT_U_CHAR_RES],
						wk->ResMan[CLACT_U_PLTT_RES],
						wk->ResMan[CLACT_U_CELL_RES],
						wk->ResMan[CLACT_U_CELLANM_RES],
						NULL,NULL);

	{
		outAdd->ClActSet	= wk->ClactSet;
		outAdd->ClActHeader	= outClActHeader;

		outAdd->mat.x		= 0;//FX32_CONST(32) ;
		outAdd->mat.y		= 0;//FX32_CONST(96) ;		//画面は上下連続している
		outAdd->mat.z		= 0;
		outAdd->sca.x		= FX32_ONE;
		outAdd->sca.y		= FX32_ONE;
		outAdd->sca.z		= FX32_ONE;
		outAdd->rot			= 0;
		outAdd->pri			= 1;
		outAdd->DrawArea	= draw_area;
		outAdd->heap		= HEAPID_OP_DEMO;
	}	
}

//--------------------------------------------------------------------------------------------
/**
 * オープニングＯＢＪデータ構造体サイズ取得関数
 *
 * @param	void
 *
 * @return	int		サイズ
 */
//--------------------------------------------------------------------------------------------
const int OPD_Obj_GetWorkSize( void )
{
	return sizeof(OP_DEMO_OBJ_WORK);
}

//--------------------------------------------------------------------------------------------
/**
 * セルアクター初期化
 *
 * @param	wk	OBJワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void OPD_Obj_InitCellActor( OPD_OBJ_PTR wk )
{
	int i;
	const int heapid = HEAPID_OP_DEMO;

	//マッピングモード設定
	GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_1D_32K);
	GXS_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_1D_32K);
	
	OpdInitCharPlttManager();
	// OAMマネージャーの初期化
	NNS_G2dInitOamManagerModule();

	// 共有OAMマネージャ作成
	// レンダラ用OAMマネージャ作成
	REND_OAMInit( 
			0, 128,		// メイン画面OAM管理領域
			0, 32,		// メイン画面アフィン管理領域
			0, 128,		// サブ画面OAM管理領域
			0, 32,		// サブ画面アフィン管理領域
			heapid);
	
	// セルアクター初期化
	wk->ClactSet = CLACT_U_SetEasyInit( OP_DEMO_ACT_MAX, &wk->RendData, heapid );
	
	wk->cell_transfer_state = InitCellTransfer(VBLANK_TRANS_ANM_MAX, heapid);

	//リソースマネージャー初期化
	for(i=0;i<RESOURCE_NUM;i++){		//リソースマネージャー作成
		wk->ResMan[i] = CLACT_U_ResManagerInit(ResEntryNumTbl[i], i, heapid);
	}

	//--chara読み込み
										
	//主人公　男
	wk->ResObjTbl[ACT_ID_HERO][CLACT_U_CHAR_RES] = CLACT_U_ResManagerResAddArcChar(
										wk->ResMan[CLACT_U_CHAR_RES],
										ARC_OP_DEMO, NARC_op_demo_op_hero_NCGR,
										FALSE, ACT_ID_HERO, NNS_G2D_VRAM_TYPE_2DMAIN, heapid);
	//主人公　男(サブ画面)
	wk->ResObjTbl[ACT_ID_HERO_SUB][CLACT_U_CHAR_RES] = CLACT_U_ResManagerResAddArcChar(
										wk->ResMan[CLACT_U_CHAR_RES],
										ARC_OP_DEMO, NARC_op_demo_op_hero_NCGR,
										FALSE, ACT_ID_HERO_SUB, NNS_G2D_VRAM_TYPE_2DSUB, heapid);
	//主人公　女
	wk->ResObjTbl[ACT_ID_HEROINE][CLACT_U_CHAR_RES] = CLACT_U_ResManagerResAddArcChar(
										wk->ResMan[CLACT_U_CHAR_RES],
										ARC_OP_DEMO, NARC_op_demo_op_heroine_NCGR,
										FALSE, ACT_ID_HEROINE, NNS_G2D_VRAM_TYPE_2DMAIN, heapid);
	//ライバル
	wk->ResObjTbl[ACT_ID_RIVAL][CLACT_U_CHAR_RES] = CLACT_U_ResManagerResAddArcChar(
										wk->ResMan[CLACT_U_CHAR_RES],
										ARC_OP_DEMO, NARC_op_demo_op_hansam_NCGR,
										FALSE, ACT_ID_RIVAL, NNS_G2D_VRAM_TYPE_2DMAIN, heapid);

	//鳥
	wk->ResObjTbl[ACT_ID_BIRD][CLACT_U_CHAR_RES] = CLACT_U_ResManagerResAddArcChar(
										wk->ResMan[CLACT_U_CHAR_RES],
										ARC_OP_DEMO, NARC_op_demo_title_bird_NCGR,
										FALSE, ACT_ID_BIRD, NNS_G2D_VRAM_TYPE_2DMAIN, heapid);

	//キラキラ
	wk->ResObjTbl[ACT_ID_KIRA][CLACT_U_CHAR_RES] = CLACT_U_ResManagerResAddArcChar(
										wk->ResMan[CLACT_U_CHAR_RES],
										ARC_OP_DEMO, NARC_op_demo_logo_kira_NCGR,
										FALSE, ACT_ID_KIRA, NNS_G2D_VRAM_TYPE_2DSUB, heapid);	

	//博士
	wk->ResObjTbl[ACT_ID_HAKASE][CLACT_U_CHAR_RES] = CLACT_U_ResManagerResAddArcChar(
										wk->ResMan[CLACT_U_CHAR_RES],
										ARC_OP_DEMO, NARC_op_demo_op_pluto_NCGR,
										FALSE, ACT_ID_HAKASE, NNS_G2D_VRAM_TYPE_2DMAIN, heapid);

	//ボール
	wk->ResObjTbl[ACT_ID_BALL_MAIN][CLACT_U_CHAR_RES] = CLACT_U_ResManagerResAddArcChar(
										wk->ResMan[CLACT_U_CHAR_RES],
										ARC_OP_DEMO, NARC_op_demo_op_mball_NCGR,
										FALSE, ACT_ID_BALL_MAIN, NNS_G2D_VRAM_TYPE_2DMAIN, heapid);
	wk->ResObjTbl[ACT_ID_BALL_SUB][CLACT_U_CHAR_RES] = CLACT_U_ResManagerResAddArcChar(
										wk->ResMan[CLACT_U_CHAR_RES],
										ARC_OP_DEMO, NARC_op_demo_op_mball_NCGR,
										FALSE, ACT_ID_BALL_SUB, NNS_G2D_VRAM_TYPE_2DSUB, heapid);

	//流星
	wk->ResObjTbl[ACT_ID_STREAM_MAIN][CLACT_U_CHAR_RES] = CLACT_U_ResManagerResAddArcChar(
									wk->ResMan[CLACT_U_CHAR_RES],
									ARC_OP_DEMO, NARC_op_demo_op_pokebg_NCGR,
									FALSE, ACT_ID_STREAM_MAIN, NNS_G2D_VRAM_TYPE_2DMAIN, heapid);
	wk->ResObjTbl[ACT_ID_STREAM_SUB][CLACT_U_CHAR_RES] = CLACT_U_ResManagerResAddArcChar(
									wk->ResMan[CLACT_U_CHAR_RES],
									ARC_OP_DEMO, NARC_op_demo_op_pokebg_NCGR,
									FALSE, ACT_ID_STREAM_SUB, NNS_G2D_VRAM_TYPE_2DSUB, heapid);

	//--pal読み込み

	//主人公　男
	wk->ResObjTbl[ACT_ID_HERO][CLACT_U_PLTT_RES] = CLACT_U_ResManagerResAddArcPltt(
										wk->ResMan[CLACT_U_PLTT_RES],
										ARC_OP_DEMO, NARC_op_demo_op_hero_NCLR,
										FALSE, ACT_ID_HERO, NNS_G2D_VRAM_TYPE_2DMAIN, PALETTE_NUM, heapid);
	//主人公　男(サブ画面)
	wk->ResObjTbl[ACT_ID_HERO_SUB][CLACT_U_PLTT_RES] = CLACT_U_ResManagerResAddArcPltt(
										wk->ResMan[CLACT_U_PLTT_RES],
										ARC_OP_DEMO, NARC_op_demo_op_hero_NCLR,
										FALSE, ACT_ID_HERO_SUB, NNS_G2D_VRAM_TYPE_2DSUB, PALETTE_NUM, heapid);
	//主人公　女
	wk->ResObjTbl[ACT_ID_HEROINE][CLACT_U_PLTT_RES] = CLACT_U_ResManagerResAddArcPltt(
										wk->ResMan[CLACT_U_PLTT_RES],
										ARC_OP_DEMO, NARC_op_demo_op_heroine_NCLR,
										FALSE, ACT_ID_HEROINE, NNS_G2D_VRAM_TYPE_2DMAIN, PALETTE_NUM, heapid);
	//ライバル
	wk->ResObjTbl[ACT_ID_RIVAL][CLACT_U_PLTT_RES] = CLACT_U_ResManagerResAddArcPltt(
										wk->ResMan[CLACT_U_PLTT_RES],
										ARC_OP_DEMO, NARC_op_demo_op_hansam_NCLR,
										FALSE, ACT_ID_RIVAL, NNS_G2D_VRAM_TYPE_2DMAIN, PALETTE_NUM, heapid);

	//鳥
	wk->ResObjTbl[ACT_ID_BIRD][CLACT_U_PLTT_RES] = CLACT_U_ResManagerResAddArcPltt(
										wk->ResMan[CLACT_U_PLTT_RES],
										ARC_OP_DEMO, NARC_op_demo_title_bird_NCLR,
										FALSE, ACT_ID_BIRD, NNS_G2D_VRAM_TYPE_2DMAIN, PALETTE_NUM, heapid);

	//キラキラ
	wk->ResObjTbl[ACT_ID_KIRA][CLACT_U_PLTT_RES] = CLACT_U_ResManagerResAddArcPltt(
										wk->ResMan[CLACT_U_PLTT_RES],
										ARC_OP_DEMO, NARC_op_demo_logo_kira_NCLR,
										FALSE, ACT_ID_KIRA, NNS_G2D_VRAM_TYPE_2DSUB, PALETTE_NUM, heapid);

	//博士
	wk->ResObjTbl[ACT_ID_HAKASE][CLACT_U_PLTT_RES] = CLACT_U_ResManagerResAddArcPltt(
										wk->ResMan[CLACT_U_PLTT_RES],
										ARC_OP_DEMO, NARC_op_demo_op_pluto_NCLR,
										FALSE, ACT_ID_HAKASE, NNS_G2D_VRAM_TYPE_2DMAIN, PALETTE_NUM, heapid);

	//ボール
	wk->ResObjTbl[ACT_ID_BALL_MAIN][CLACT_U_PLTT_RES] = CLACT_U_ResManagerResAddArcPltt(
										wk->ResMan[CLACT_U_PLTT_RES],
										ARC_OP_DEMO, NARC_op_demo_op_mball_NCLR,
										FALSE, ACT_ID_BALL_MAIN, NNS_G2D_VRAM_TYPE_2DMAIN, PALETTE_NUM, heapid);
	wk->ResObjTbl[ACT_ID_BALL_SUB][CLACT_U_PLTT_RES] = CLACT_U_ResManagerResAddArcPltt(
										wk->ResMan[CLACT_U_PLTT_RES],
										ARC_OP_DEMO, NARC_op_demo_op_mball_NCLR,
										FALSE, ACT_ID_BALL_SUB, NNS_G2D_VRAM_TYPE_2DSUB, PALETTE_NUM, heapid);

	//流星
	wk->ResObjTbl[ACT_ID_STREAM_MAIN][CLACT_U_PLTT_RES] = CLACT_U_ResManagerResAddArcPltt(
										wk->ResMan[CLACT_U_PLTT_RES],
										ARC_OP_DEMO, NARC_op_demo_op_pokebg_NCLR,
										FALSE, ACT_ID_STREAM_MAIN, NNS_G2D_VRAM_TYPE_2DMAIN, 3, heapid);
	wk->ResObjTbl[ACT_ID_STREAM_SUB][CLACT_U_PLTT_RES] = CLACT_U_ResManagerResAddArcPltt(
										wk->ResMan[CLACT_U_PLTT_RES],
										ARC_OP_DEMO, NARC_op_demo_op_pokebg_NCLR,
										FALSE, ACT_ID_STREAM_SUB, NNS_G2D_VRAM_TYPE_2DSUB, 3, heapid);

	//--cell読み込み
										
	//主人公　男
	wk->ResObjTbl[ACT_ID_HERO][CLACT_U_CELL_RES] = CLACT_U_ResManagerResAddArcKindCell(
										wk->ResMan[CLACT_U_CELL_RES],
										ARC_OP_DEMO, NARC_op_demo_op_hero_NCER,
										FALSE, ACT_ID_HERO, CLACT_U_CELL_RES, heapid);
	//主人公　男(サブ画面)
	wk->ResObjTbl[ACT_ID_HERO_SUB][CLACT_U_CELL_RES] = CLACT_U_ResManagerResAddArcKindCell(
										wk->ResMan[CLACT_U_CELL_RES],
										ARC_OP_DEMO, NARC_op_demo_op_hero_NCER,
										FALSE, ACT_ID_HERO_SUB, CLACT_U_CELL_RES, heapid);
	//主人公　女
	wk->ResObjTbl[ACT_ID_HEROINE][CLACT_U_CELL_RES] = CLACT_U_ResManagerResAddArcKindCell(
										wk->ResMan[CLACT_U_CELL_RES],
										ARC_OP_DEMO, NARC_op_demo_op_heroine_NCER,
										FALSE, ACT_ID_HEROINE, CLACT_U_CELL_RES, heapid);
	//ライバル
	wk->ResObjTbl[ACT_ID_RIVAL][CLACT_U_CELL_RES] = CLACT_U_ResManagerResAddArcKindCell(
										wk->ResMan[CLACT_U_CELL_RES],
										ARC_OP_DEMO, NARC_op_demo_op_hansam_NCER,
										FALSE, ACT_ID_RIVAL, CLACT_U_CELL_RES, heapid);

	//鳥
	wk->ResObjTbl[ACT_ID_BIRD][CLACT_U_CELL_RES] = CLACT_U_ResManagerResAddArcKindCell(
										wk->ResMan[CLACT_U_CELL_RES],
										ARC_OP_DEMO, NARC_op_demo_title_bird_NCER,
										FALSE, ACT_ID_BIRD, CLACT_U_CELL_RES, heapid);

	//博士
	wk->ResObjTbl[ACT_ID_HAKASE][CLACT_U_CELL_RES] = CLACT_U_ResManagerResAddArcKindCell(
										wk->ResMan[CLACT_U_CELL_RES],
										ARC_OP_DEMO, NARC_op_demo_op_pluto_NCER,
										FALSE, ACT_ID_HAKASE, CLACT_U_CELL_RES, heapid);

	//ボール
	wk->ResObjTbl[ACT_ID_BALL_MAIN][CLACT_U_CELL_RES] = CLACT_U_ResManagerResAddArcKindCell(
										wk->ResMan[CLACT_U_CELL_RES],
										ARC_OP_DEMO, NARC_op_demo_op_mball_NCER,
										FALSE, ACT_ID_BALL_MAIN, CLACT_U_CELL_RES, heapid);
	wk->ResObjTbl[ACT_ID_BALL_SUB][CLACT_U_CELL_RES] = CLACT_U_ResManagerResAddArcKindCell(
										wk->ResMan[CLACT_U_CELL_RES],
										ARC_OP_DEMO, NARC_op_demo_op_mball_NCER,
										FALSE, ACT_ID_BALL_SUB, CLACT_U_CELL_RES, heapid);

	//流星
	wk->ResObjTbl[ACT_ID_STREAM_MAIN][CLACT_U_CELL_RES] = CLACT_U_ResManagerResAddArcKindCell(
										wk->ResMan[CLACT_U_CELL_RES],
										ARC_OP_DEMO, NARC_op_demo_op_pokebg_NCER,
										FALSE, ACT_ID_STREAM_MAIN, CLACT_U_CELL_RES, heapid);
	wk->ResObjTbl[ACT_ID_STREAM_SUB][CLACT_U_CELL_RES] = CLACT_U_ResManagerResAddArcKindCell(
										wk->ResMan[CLACT_U_CELL_RES],
										ARC_OP_DEMO, NARC_op_demo_op_pokebg_NCER,
										FALSE, ACT_ID_STREAM_SUB, CLACT_U_CELL_RES, heapid);

	//キラキラ
	wk->ResObjTbl[ACT_ID_KIRA][CLACT_U_CELL_RES] = CLACT_U_ResManagerResAddArcKindCell(
										wk->ResMan[CLACT_U_CELL_RES],
										ARC_OP_DEMO, NARC_op_demo_logo_kira_NCER,
										FALSE, ACT_ID_KIRA, CLACT_U_CELL_RES, heapid);
	//--同じ関数でanim読み込み
										
	//主人公　男
	wk->ResObjTbl[ACT_ID_HERO][CLACT_U_CELLANM_RES] = CLACT_U_ResManagerResAddArcKindCell(
										wk->ResMan[CLACT_U_CELLANM_RES],
										ARC_OP_DEMO, NARC_op_demo_op_hero_NANR,
										FALSE, ACT_ID_HERO, CLACT_U_CELLANM_RES, heapid);
	//主人公　男(サブ画面)
	wk->ResObjTbl[ACT_ID_HERO_SUB][CLACT_U_CELLANM_RES] = CLACT_U_ResManagerResAddArcKindCell(
										wk->ResMan[CLACT_U_CELLANM_RES],
										ARC_OP_DEMO, NARC_op_demo_op_hero_NANR,
										FALSE, ACT_ID_HERO_SUB, CLACT_U_CELLANM_RES, heapid);
	//主人公　女
	wk->ResObjTbl[ACT_ID_HEROINE][CLACT_U_CELLANM_RES] = CLACT_U_ResManagerResAddArcKindCell(
										wk->ResMan[CLACT_U_CELLANM_RES],
										ARC_OP_DEMO, NARC_op_demo_op_heroine_NANR,
										FALSE, ACT_ID_HEROINE, CLACT_U_CELLANM_RES, heapid);
	//ライバル
	wk->ResObjTbl[ACT_ID_RIVAL][CLACT_U_CELLANM_RES] = CLACT_U_ResManagerResAddArcKindCell(
										wk->ResMan[CLACT_U_CELLANM_RES],
										ARC_OP_DEMO, NARC_op_demo_op_hansam_NANR,
										FALSE, ACT_ID_RIVAL, CLACT_U_CELLANM_RES, heapid);

	//鳥
	wk->ResObjTbl[ACT_ID_BIRD][CLACT_U_CELLANM_RES] = CLACT_U_ResManagerResAddArcKindCell(
										wk->ResMan[CLACT_U_CELLANM_RES],
										ARC_OP_DEMO, NARC_op_demo_title_bird_NANR,
										FALSE, ACT_ID_BIRD, CLACT_U_CELLANM_RES, heapid);

	//キラキラ
	wk->ResObjTbl[ACT_ID_KIRA][CLACT_U_CELLANM_RES] = CLACT_U_ResManagerResAddArcKindCell(
										wk->ResMan[CLACT_U_CELLANM_RES],
										ARC_OP_DEMO, NARC_op_demo_logo_kira_NANR,
										FALSE, ACT_ID_KIRA, CLACT_U_CELLANM_RES, heapid);

	//博士
	wk->ResObjTbl[ACT_ID_HAKASE][CLACT_U_CELLANM_RES] = CLACT_U_ResManagerResAddArcKindCell(
										wk->ResMan[CLACT_U_CELLANM_RES],
										ARC_OP_DEMO, NARC_op_demo_op_pluto_NANR,
										FALSE, ACT_ID_HAKASE, CLACT_U_CELLANM_RES, heapid);

	//ボール
	wk->ResObjTbl[ACT_ID_BALL_MAIN][CLACT_U_CELLANM_RES] = CLACT_U_ResManagerResAddArcKindCell(
										wk->ResMan[CLACT_U_CELLANM_RES],
										ARC_OP_DEMO, NARC_op_demo_op_mball_NANR,
										FALSE, ACT_ID_BALL_MAIN, CLACT_U_CELLANM_RES, heapid);
	wk->ResObjTbl[ACT_ID_BALL_SUB][CLACT_U_CELLANM_RES] = CLACT_U_ResManagerResAddArcKindCell(
										wk->ResMan[CLACT_U_CELLANM_RES],
										ARC_OP_DEMO, NARC_op_demo_op_mball_NANR,
										FALSE, ACT_ID_BALL_SUB, CLACT_U_CELLANM_RES, heapid);

	//流星
	wk->ResObjTbl[ACT_ID_STREAM_MAIN][CLACT_U_CELLANM_RES] = CLACT_U_ResManagerResAddArcKindCell(
										wk->ResMan[CLACT_U_CELLANM_RES],
										ARC_OP_DEMO, NARC_op_demo_op_pokebg_NANR,
										FALSE, ACT_ID_STREAM_MAIN, CLACT_U_CELLANM_RES, heapid);
	wk->ResObjTbl[ACT_ID_STREAM_SUB][CLACT_U_CELLANM_RES] = CLACT_U_ResManagerResAddArcKindCell(
										wk->ResMan[CLACT_U_CELLANM_RES],
										ARC_OP_DEMO, NARC_op_demo_op_pokebg_NANR,
										FALSE, ACT_ID_STREAM_SUB, CLACT_U_CELLANM_RES, heapid);
	
	// リソースマネージャーから転送

	for (i=0;i<ACT_ID_MAX;i++){
		// Chara転送
		CLACT_U_CharManagerSetAreaCont( wk->ResObjTbl[i][CLACT_U_CHAR_RES] );
		// パレット転送
		CLACT_U_PlttManagerSetCleanArea( wk->ResObjTbl[i][CLACT_U_PLTT_RES] );
	}

	GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );		// MAIN DISP OBJ ON
}

//--------------------------------------------------------------------------------------------
/**
 * セルアクターを終了
 *
 * @param	wk	OBJワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void OPD_Obj_EndCellActor( OPD_OBJ_PTR wk )
{
	u8 i;
	// セルアクターリソース解放

	for (i=0;i<ACT_ID_MAX;i++){
		// キャラ転送マネージャー破棄
		CLACT_U_CharManagerDelete(wk->ResObjTbl[i][CLACT_U_CHAR_RES]);
		// パレット転送マネージャー破棄
		CLACT_U_PlttManagerDelete(wk->ResObjTbl[i][CLACT_U_PLTT_RES]);
	}
	
	// キャラ・パレット・セル・セルアニメのリソースマネージャー破棄
	for(i=0;i<RESOURCE_NUM;i++){
		CLACT_U_ResManagerDelete(wk->ResMan[i]);
	}
	// セルアクターセット破棄
	CLACT_DestSet(wk->ClactSet);

	//Vブランク転送アニメ用のヘッダ削除
	for(i = 0; i < VBLANK_TRANS_ANM_MAX; i++){
		CLACT_U_DeleteInHeaderVramTransferArea(&wk->vblank_trans_anm_header[i]);
	}
	if(wk->cell_transfer_state != NULL){
		DeleteCellTransfer(wk->cell_transfer_state);
		wk->cell_transfer_state = NULL;
	}
	
	//OAMレンダラー破棄
	REND_OAM_Delete();

	DeleteCharManager();
	DeletePlttManager();
	
	DellVramTransferManager();
}

//--------------------------------------------------------------------------------------------
/**
 * セルアクターをセット
 *
 * @param	wk	OBJワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void OPD_Obj_SetActor( OPD_OBJ_PTR wk )
{
	int i;
	CLACT_HEADER			cl_act_header;
	CLACT_ADD add;

	const int bird_act[BIRD_ACT_MAX] = {
		ACT_WORK_BIRD_1,
		ACT_WORK_BIRD_2,
		ACT_WORK_BIRD_3,
		ACT_WORK_BIRD_4,
		ACT_WORK_BIRD_5,
		ACT_WORK_BIRD_6,
	};

	const int bird_pos_x[BIRD_ACT_MAX] = {
		BIRD_1_POS_X,
		BIRD_2_POS_X,
		BIRD_3_POS_X,
		BIRD_4_POS_X,
		BIRD_5_POS_X,
		BIRD_6_POS_X,
	};

	const int bird_pos_y[BIRD_ACT_MAX] = {
		BIRD_1_POS_Y,
		BIRD_2_POS_Y,
		BIRD_3_POS_Y,
		BIRD_4_POS_Y,
		BIRD_5_POS_Y,
		BIRD_6_POS_Y,
	};

	{
		//セルアクター表示開始
		// 下画面(メイン画面)
		
		//主人公　男
		MakeClActHeader( ACT_ID_HERO, wk, 0, &add, 
			&wk->vblank_trans_anm_header[VBLANK_TRANS_ANM_HERO], TRUE, NNS_G2D_VRAM_TYPE_2DMAIN );
		add.mat.x = FX32_ONE * HERO_POS_X;
		add.mat.y = FX32_ONE * HERO_POS_Y;
		wk->ClActWork[ACT_WORK_HERO] = CLACT_Add(&add);
		CLACT_SetAnmFlag(wk->ClActWork[ACT_WORK_HERO],0);
		CLACT_SetDrawFlag(wk->ClActWork[ACT_WORK_HERO], 0);	//非表示

		//主人公　男(サブ画面)
		MakeClActHeader( ACT_ID_HERO_SUB, wk, 0, &add, 
			&wk->vblank_trans_anm_header[VBLANK_TRANS_ANM_HERO_SUB], 
			TRUE, NNS_G2D_VRAM_TYPE_2DSUB );
		add.mat.x = FX32_ONE * HERO_POS_X;
		add.mat.y = FX32_ONE * HERO_POS_Y + SUB_SURFACE_Y;
		wk->ClActWork[ACT_WORK_HERO_SUB] = CLACT_Add(&add);
		CLACT_SetAnmFlag(wk->ClActWork[ACT_WORK_HERO_SUB],0);
		CLACT_SetDrawFlag(wk->ClActWork[ACT_WORK_HERO_SUB], 0);	//非表示
		
		//主人公　女
		MakeClActHeader( ACT_ID_HEROINE, wk, 0, &add, 
			&wk->vblank_trans_anm_header[VBLANK_TRANS_ANM_HEROINE], 
			TRUE, NNS_G2D_VRAM_TYPE_2DMAIN );
		add.mat.x = FX32_ONE * HEROINE_POS_X;
		add.mat.y = FX32_ONE * HEROINE_POS_Y;
		wk->ClActWork[ACT_WORK_HEROINE] = CLACT_Add(&add);
		CLACT_SetAnmFlag(wk->ClActWork[ACT_WORK_HEROINE],0);
		CLACT_SetDrawFlag(wk->ClActWork[ACT_WORK_HEROINE], 0);	//非表示

		//主人公アニメ
	//	OPD_Obj_ChangeHeroHeroineAnime(wk, 0);
		
		//ライバル
		MakeClActHeader( ACT_ID_RIVAL, wk, 0, &add, &cl_act_header, 
			FALSE, NNS_G2D_VRAM_TYPE_2DMAIN );
		add.mat.x = FX32_ONE * RIVAL_POS_X;
		add.mat.y = FX32_ONE * RIVAL_POS_Y;
		wk->ClActWork[ACT_WORK_RIVAL] = CLACT_Add(&add);
		CLACT_SetAnmFlag(wk->ClActWork[ACT_WORK_RIVAL],1);
		CLACT_SetDrawFlag(wk->ClActWork[ACT_WORK_RIVAL], 0);	//非表示

		//博士
		MakeClActHeader( ACT_ID_HAKASE, wk, 0, &add, &cl_act_header, 
			FALSE, NNS_G2D_VRAM_TYPE_2DMAIN );
		add.mat.x = FX32_ONE * HAKASE_POS_X;
		add.mat.y = FX32_ONE * HAKASE_POS_Y;
		wk->ClActWork[ACT_WORK_HAKASE] = CLACT_Add(&add);
	//	CLACT_SetFlip( wk->ClActWork[ACT_WORK_HAKASE], CLACT_FLIP_H );
		CLACT_SetAnmFlag(wk->ClActWork[ACT_WORK_HAKASE],1);
		CLACT_SetDrawFlag(wk->ClActWork[ACT_WORK_HAKASE], 0);	//非表示

		//ボール(メイン画面)
		MakeClActHeader( ACT_ID_BALL_MAIN, wk, 1, &add, &cl_act_header, 
			FALSE, NNS_G2D_VRAM_TYPE_2DMAIN );
		add.mat.x = FX32_ONE * BALL_POS_X;
		add.mat.y = FX32_ONE * BALL_POS_Y;
		wk->ClActWork[ACT_WORK_BALL_MAIN] = CLACT_Add(&add);
		CLACT_SetAnmFlag(wk->ClActWork[ACT_WORK_BALL_MAIN],1);
		CLACT_SetDrawFlag(wk->ClActWork[ACT_WORK_BALL_MAIN], 0);	//非表示
		CLACT_SetAffineParam(wk->ClActWork[ACT_WORK_BALL_MAIN], CLACT_AFFINE_NORMAL);	//Affine ON
		//ボール(サブ画面)
		MakeClActHeader( ACT_ID_BALL_SUB, wk, 1, &add, &cl_act_header, 
			FALSE, NNS_G2D_VRAM_TYPE_2DSUB );
		add.mat.x = FX32_ONE * BALL_POS_X;
		add.mat.y = FX32_ONE * BALL_POS_Y + SUB_SURFACE_Y;
		wk->ClActWork[ACT_WORK_BALL_SUB] = CLACT_Add(&add);
		CLACT_SetAnmFlag(wk->ClActWork[ACT_WORK_BALL_SUB],1);
		CLACT_SetDrawFlag(wk->ClActWork[ACT_WORK_BALL_SUB], 0);	//非表示
		CLACT_SetAffineParam(wk->ClActWork[ACT_WORK_BALL_SUB], CLACT_AFFINE_NORMAL);	//Affine ON

		//流星(メイン画面)
		MakeClActHeader( ACT_ID_STREAM_MAIN, wk, 3, &add, &cl_act_header, 
			FALSE, NNS_G2D_VRAM_TYPE_2DMAIN );
		add.mat.x = FX32_ONE * STREAM_POS_X;
		add.mat.y = FX32_ONE * STREAM_POS_Y;
		wk->ClActWork[ACT_WORK_STREAM_MAIN_0] = CLACT_Add(&add);
		CLACT_SetAnmFlag(wk->ClActWork[ACT_WORK_STREAM_MAIN_0],1);
		CLACT_SetDrawFlag(wk->ClActWork[ACT_WORK_STREAM_MAIN_0], 0);	//非表示
		//2個目
		wk->ClActWork[ACT_WORK_STREAM_MAIN_1] = CLACT_Add(&add);
		CLACT_SetAnmFlag(wk->ClActWork[ACT_WORK_STREAM_MAIN_1],1);
		CLACT_SetDrawFlag(wk->ClActWork[ACT_WORK_STREAM_MAIN_1], 0);	//非表示
		//流星(サブ画面)
		MakeClActHeader( ACT_ID_STREAM_SUB, wk, 3, &add, &cl_act_header, 
			FALSE, NNS_G2D_VRAM_TYPE_2DSUB );
		add.mat.x = FX32_ONE * STREAM_POS_X;
		add.mat.y = FX32_ONE * STREAM_POS_Y + SUB_SURFACE_Y;
		wk->ClActWork[ACT_WORK_STREAM_SUB_0] = CLACT_Add(&add);
		CLACT_SetAnmFlag(wk->ClActWork[ACT_WORK_STREAM_SUB_0],1);
		CLACT_SetDrawFlag(wk->ClActWork[ACT_WORK_STREAM_SUB_0], 0);	//非表示
		//2個目
		wk->ClActWork[ACT_WORK_STREAM_SUB_1] = CLACT_Add(&add);
		CLACT_SetAnmFlag(wk->ClActWork[ACT_WORK_STREAM_SUB_1],1);
		CLACT_SetDrawFlag(wk->ClActWork[ACT_WORK_STREAM_SUB_1], 0);	//非表示

		//鳥
		for(i=0;i<BIRD_ACT_MAX;i++){
			MakeClActHeader( ACT_ID_BIRD, wk, 0, &add, &cl_act_header, 
				FALSE, NNS_G2D_VRAM_TYPE_2DMAIN );
			add.mat.x = FX32_ONE * bird_pos_x[i];
			add.mat.y = FX32_ONE * bird_pos_y[i];
			wk->ClActWork[ bird_act[i] ] = CLACT_Add(&add);
			CLACT_SetAnmFlag(wk->ClActWork[ bird_act[i] ],1);
			CLACT_SetDrawFlag(wk->ClActWork[ bird_act[i] ], 0);	//非表示
		}
	}	
}

//--------------------------------------------------------------------------------------------
/**
 * セルアクターをセット(キラキラ)
 *
 * @param	wk		OBJワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void OPD_Obj_SetKiraActor( OPD_OBJ_PTR wk, OPD_KIRA_DATLST_PTR pKiraDataListPtr )
{
	int i;
	CLACT_HEADER			cl_act_header;
	CLACT_ADD add;

	CLACT_WORK_PTR *act_work;

	{
		//セルアクター表示開始
		// 上画面(サブ画面)
		
		//キラキラ
		MakeClActHeader( ACT_ID_KIRA, wk, 0, &add, &cl_act_header, FALSE, NNS_G2D_VRAM_TYPE_2DSUB);

		for (i=0;i<KIRA_ACT_MAX;i++){
///			add.mat.x = FX32_ONE * (gf_rand()%256);
///			add.mat.y = FX32_ONE * (192+gf_rand()%192);
			
			act_work = OPDKira_GetActWorkPtrFromList( pKiraDataListPtr, i );
			(*act_work) = CLACT_Add(&add);
			CLACT_SetAnmFlag(*act_work,0);
			CLACT_SetDrawFlag(*act_work, 0);	//非表示
		}
	}	
}

//--------------------------------------------------------------------------------------------
/**
 * キラキラセルアクターを非表示にする
 *
 * @param	wk		OBJワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void OPD_Obj_KiraActorDrawOff( OPD_OBJ_PTR wk, OPD_KIRA_DATLST_PTR pKiraDataListPtr )
{
	int i;
	CLACT_WORK_PTR *act_work;

	for (i=0;i<KIRA_ACT_MAX;i++){
		act_work = OPDKira_GetActWorkPtrFromList( pKiraDataListPtr, i );
		CLACT_SetDrawFlag(*act_work, 0);	//非表示
	}
}

//--------------------------------------------------------------------------------------------
/**
 * セルアクター描画
 *
 * @param	wk	OBJワーク	
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void OPD_Obj_DrawActor( OPD_OBJ_PTR wk )
{
	CLACT_Draw( wk->ClactSet );
	if(wk->cell_transfer_state != NULL){
		UpdateCellTransfer();
	}
}

//--------------------------------------------------------------------------------------------
/**
 * OBJの表示・非表示
 *
 * @param	wk				OBJワーク
 * @param	inActWorkNo		アクターワークナンバー
 * @param	inFlg			表示フラグ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void OPD_Obj_DispOnOff( OPD_OBJ_PTR wk, const u8 inActWorkNo, const u8 inFlg )
{
	CLACT_SetDrawFlag(wk->ClActWork[inActWorkNo], inFlg);
}

//--------------------------------------------------------------------------------------------
/**
 * OBJの位置変更
 *
 * @param	wk				OBJワーク
 * @param	inActWorkNo		アクターワークナンバー
 * @param	inFlg			表示フラグ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void OPD_Obj_ChangePos( OPD_OBJ_PTR wk, const u8 inActWorkNo, const int inX, const int inY )
{
	VecFx32 vec;
	
	vec =  *CLACT_GetMatrix( wk->ClActWork[inActWorkNo] );
	vec.x = inX;
	vec.y = inY;
	CLACT_SetMatrix( wk->ClActWork[inActWorkNo], &vec );
}

//--------------------------------------------------------------------------------------------
/**
 * ヒロインOBJの位置変更
 *
 * @param	wk				OBJワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void OPD_Obj_ChangeHeroinePos( OPD_OBJ_PTR wk )
{
	OPD_Obj_ChangePos( wk, ACT_WORK_HEROINE, FX32_ONE*HEROINE_POS_X, FX32_ONE*HEROINE_POS_Y );
}

//--------------------------------------------------------------------------------------------
/**
 * ヒーロー・ヒロインOBJのアニメ変更
 *
 * @param	wk				OBJワーク
 * @param	inNo			アニメナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void OPD_Obj_ChangeHeroHeroineAnime( OPD_OBJ_PTR wk, const u8 inNo )
{
	CLACT_AnmChg( wk->ClActWork[ACT_WORK_HERO], inNo );
	CLACT_AnmChg( wk->ClActWork[ACT_WORK_HEROINE], inNo );
	CLACT_AnmChg( wk->ClActWork[ACT_WORK_HERO_SUB], inNo );
}

//--------------------------------------------------------------------------------------------
/**
 * ヒーロー・ヒロインOBJのアニメ開始・停止
 *
 * @param	wk				OBJワーク
 * @param	inNo			アニメナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void OPD_Obj_OnOffHeroHeroineAnime( OPD_OBJ_PTR wk, const u8 inAnime )
{
	CLACT_SetAnmFlag(wk->ClActWork[ACT_WORK_HERO],inAnime);
	CLACT_SetAnmFlag(wk->ClActWork[ACT_WORK_HEROINE],inAnime);
	CLACT_SetAnmFlag(wk->ClActWork[ACT_WORK_HERO_SUB],inAnime);
}

//--------------------------------------------------------------
/**
 * @brief   ヒーローとヒロインをスライド移動させる
 *
 * @param   wk		OBJワーク
 */
//--------------------------------------------------------------
void OPD_Obj_HeroHeroineMove(OPD_OBJ_PTR wk)
{
	VecFx32 vec;

	vec =  *CLACT_GetMatrix( wk->ClActWork[ACT_WORK_HERO_SUB] );
	vec.x -= HERO_SLIDE_SPEED;
	CLACT_SetMatrix( wk->ClActWork[ACT_WORK_HERO_SUB], &vec );
	
	vec =  *CLACT_GetMatrix( wk->ClActWork[ACT_WORK_HEROINE] );
	vec.x += HEROINE_SLIDE_SPEED;
	CLACT_SetMatrix( wk->ClActWork[ACT_WORK_HEROINE], &vec );
}

//--------------------------------------------------------------------------------------------
/**
 * ライバルスライドイン
 *
 * @param	wk	OBJワーク	
 *
 * @return	BOOL	TRUEでスライド終了
 */
//--------------------------------------------------------------------------------------------
BOOL OPD_Obj_SlideInRival(OPD_OBJ_PTR wk, int inCounter)
{
	VecFx32 vec;
	
	if(inCounter < FM_RIVAL_APPEAR){
		return FALSE;
	}
	
	if(inCounter == FM_RIVAL_APPEAR){
		OPD_Obj_DispOnOff( wk, ACT_WORK_RIVAL, 1 );
	}
	
	if(inCounter < FM_RIVAL_OUT){
		vec =  *CLACT_GetMatrix( wk->ClActWork[ACT_WORK_RIVAL] );
		vec.x += RIVAL_SLIDE_SPEED;

		if (vec.x >= RIVAL_POS_END_X){
			vec.x = RIVAL_POS_END_X;
			CLACT_SetMatrix( wk->ClActWork[ACT_WORK_RIVAL], &vec );
			return TRUE;
		}
		CLACT_SetMatrix( wk->ClActWork[ACT_WORK_RIVAL], &vec );
		return FALSE;
	}
	else{
		vec =  *CLACT_GetMatrix( wk->ClActWork[ACT_WORK_RIVAL] );
		vec.x += RIVAL_SLIDE_SPEED;

		if (vec.x >= (256+64)*FX32_ONE){
			vec.x = (256+64)*FX32_ONE;
			CLACT_SetMatrix( wk->ClActWork[ACT_WORK_RIVAL], &vec );
			OPD_Obj_DispOnOff( wk, ACT_WORK_RIVAL, 0 );
			return TRUE;
		}
		CLACT_SetMatrix( wk->ClActWork[ACT_WORK_RIVAL], &vec );
		return FALSE;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * 博士スライドイン
 *
 * @param	wk	OBJワーク	
 *
 * @return	BOOL	TRUEでスライド終了
 */
//--------------------------------------------------------------------------------------------
BOOL OPD_Obj_SlideInHakase(OPD_OBJ_PTR wk, int inCounter)
{
	VecFx32 vec;
	
	if(inCounter < FM_HAKASE_APPEAR){
		return FALSE;
	}

	if(inCounter == FM_HAKASE_APPEAR){
		OPD_Obj_DispOnOff( wk, ACT_WORK_HAKASE, 1 );
	}

	if(inCounter < FM_HAKASE_OUT){
		vec =  *CLACT_GetMatrix( wk->ClActWork[ACT_WORK_HAKASE] );
		vec.x += HAKASE_SLIDE_SPEED;

		if (vec.x <= HAKASE_POS_END_X){
			vec.x = HAKASE_POS_END_X;
			CLACT_SetMatrix( wk->ClActWork[ACT_WORK_HAKASE], &vec );
			return TRUE;
		}
		CLACT_SetMatrix( wk->ClActWork[ACT_WORK_HAKASE], &vec );
		return FALSE;
	}
	else{
		vec =  *CLACT_GetMatrix( wk->ClActWork[ACT_WORK_HAKASE] );
		vec.x += HAKASE_SLIDE_SPEED;

		if (vec.x <= (-64)*FX32_ONE){
			vec.x = (-64)*FX32_ONE;
			CLACT_SetMatrix( wk->ClActWork[ACT_WORK_HAKASE], &vec );
			OPD_Obj_DispOnOff( wk, ACT_WORK_HAKASE, 0 );
			return TRUE;
		}
		CLACT_SetMatrix( wk->ClActWork[ACT_WORK_HAKASE], &vec );
		return FALSE;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * 流星OBJスクロール
 *
 * @param	wk	OBJワーク	
 */
//--------------------------------------------------------------------------------------------
void OPD_Obj_StreamScroll(OPD_OBJ_PTR wk, fx32 speed)
{
	const VecFx32* p_vec;
	VecFx32 vec;
	int main_id, sub_id;
	
	if(sys.disp3DSW == DISP_3D_TO_MAIN){
		main_id = ACT_WORK_STREAM_MAIN_0;
		sub_id = ACT_WORK_STREAM_SUB_0;
	}
	else{
		main_id = ACT_WORK_STREAM_SUB_0;
		sub_id = ACT_WORK_STREAM_MAIN_0;
	}
	
	//main
	p_vec =  CLACT_GetMatrix( wk->ClActWork[main_id] );
	vec = *p_vec;
	vec.x += speed;
	if(vec.x >= (STREAM_POS_X + 256)*FX32_ONE){
		vec.x -= 256*FX32_ONE;
	}
	CLACT_SetMatrix( wk->ClActWork[main_id], &vec );
	vec.x -= 256*FX32_ONE;
	CLACT_SetMatrix( wk->ClActWork[main_id + 1], &vec );

	//sub
	p_vec =  CLACT_GetMatrix( wk->ClActWork[sub_id] );
	vec = *p_vec;
	vec.x -= speed;
	if(vec.x <= (STREAM_POS_X - 256)*FX32_ONE){
		vec.x += 256*FX32_ONE;
	}
	CLACT_SetMatrix( wk->ClActWork[sub_id], &vec );
	vec.x += 256*FX32_ONE;
	CLACT_SetMatrix( wk->ClActWork[sub_id + 1], &vec );
}

//--------------------------------------------------------------
/**
 * @brief   流星OBJをメイン画面とサブ画面の切り替えによる再設定処理を行う
 *
 * @param   wk		
 * @param   final_in	TRUE:最後のポケモンINの時
 */
//--------------------------------------------------------------
void OPD_Obj_StreamDispReset(OPD_OBJ_PTR wk, int final_in)
{
	const VecFx32 *p_vec;
	VecFx32 vec;
	
	if(final_in == FALSE){
		if(sys.disp3DSW == DISP_3D_TO_MAIN){
			CLACT_PaletteOffsetChgAddTransPlttNo(wk->ClActWork[ACT_WORK_STREAM_MAIN_0], 0);
			CLACT_PaletteOffsetChgAddTransPlttNo(wk->ClActWork[ACT_WORK_STREAM_MAIN_1], 0);
			CLACT_PaletteOffsetChgAddTransPlttNo(wk->ClActWork[ACT_WORK_STREAM_SUB_0], 1);
			CLACT_PaletteOffsetChgAddTransPlttNo(wk->ClActWork[ACT_WORK_STREAM_SUB_1], 1);
		}
		else{
			CLACT_PaletteOffsetChgAddTransPlttNo(wk->ClActWork[ACT_WORK_STREAM_MAIN_0], 1);
			CLACT_PaletteOffsetChgAddTransPlttNo(wk->ClActWork[ACT_WORK_STREAM_MAIN_1], 1);
			CLACT_PaletteOffsetChgAddTransPlttNo(wk->ClActWork[ACT_WORK_STREAM_SUB_0], 0);
			CLACT_PaletteOffsetChgAddTransPlttNo(wk->ClActWork[ACT_WORK_STREAM_SUB_1], 0);
		}
	}
	else{
		CLACT_PaletteOffsetChgAddTransPlttNo(wk->ClActWork[ACT_WORK_STREAM_MAIN_0], 2);
		CLACT_PaletteOffsetChgAddTransPlttNo(wk->ClActWork[ACT_WORK_STREAM_MAIN_1], 2);
		CLACT_PaletteOffsetChgAddTransPlttNo(wk->ClActWork[ACT_WORK_STREAM_SUB_0], 2);
		CLACT_PaletteOffsetChgAddTransPlttNo(wk->ClActWork[ACT_WORK_STREAM_SUB_1], 2);
	}
	
	p_vec = CLACT_GetMatrix( wk->ClActWork[ACT_WORK_STREAM_MAIN_0] );
	vec = *p_vec;
	vec.x = STREAM_POS_X * FX32_ONE;
	CLACT_SetMatrix( wk->ClActWork[ACT_WORK_STREAM_MAIN_0], &vec );

	p_vec = CLACT_GetMatrix( wk->ClActWork[ACT_WORK_STREAM_SUB_0] );
	vec = *p_vec;
	vec.x = STREAM_POS_X * FX32_ONE;
	CLACT_SetMatrix( wk->ClActWork[ACT_WORK_STREAM_SUB_0], &vec );
}

//--------------------------------------------------------------
/**
 * @brief   モンスターボール回転
 *
 * @param   wk		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
void OPD_Obj_BallRotate(OPD_OBJ_PTR wk, int inCounter)
{
	u16 rotate;
	const VecFx32 *p_vec;
	VecFx32 vec;
	s32 add_rotate;
	
	add_rotate = inCounter - FM_BALL_HAND_RELEASE;
	if(add_rotate < 0){
		add_rotate = 0;
	}
	else{
//		add_rotate *= 0xc0;	//時間が経つごとに回転を早くしていく
	}
	
	rotate = CLACT_GetRotation(wk->ClActWork[ACT_WORK_BALL_MAIN]);
	CLACT_SetRotation(wk->ClActWork[ACT_WORK_BALL_MAIN], rotate + BALL_ROTATE_SPEED + add_rotate);
	
	rotate = CLACT_GetRotation(wk->ClActWork[ACT_WORK_BALL_SUB]);
	CLACT_SetRotation(wk->ClActWork[ACT_WORK_BALL_SUB], rotate - BALL_ROTATE_SPEED - add_rotate);
	
	if(inCounter >= FM_BALL_HAND_RELEASE){
	#if 1	//ボールをスクロールさせないようにした
		p_vec = CLACT_GetMatrix( wk->ClActWork[ACT_WORK_BALL_MAIN] );
		vec = *p_vec;
		vec.x += 0x1000;
		CLACT_SetMatrix( wk->ClActWork[ACT_WORK_BALL_MAIN], &vec );

		p_vec = CLACT_GetMatrix( wk->ClActWork[ACT_WORK_BALL_SUB] );
		vec = *p_vec;
		vec.x -= 0x1000;
		CLACT_SetMatrix( wk->ClActWork[ACT_WORK_BALL_SUB], &vec );
	#endif
	}
}

//--------------------------------------------------------------------------------------------
/**
 * OBJの色変更（黒＜==＞元の色）
 *
 * @param	wk				OBJワーク
 * @param	inActWorkNo		アクターワークナンバー
 * @param	inColChgMode	色変更モード
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void OPD_Obj_ChangeCollor(OPD_OBJ_PTR wk, const int inActWorkNo, const u8 inColChgMode)
{
	NNSG2dImagePaletteProxy *proxy = CLACT_PaletteProxyGet( wk->ClActWork[inActWorkNo] );
	u32 palette_vram = NNS_G2dGetImagePaletteLocation( proxy, NNS_G2D_VRAM_TYPE_2DMAIN );
	if (inColChgMode == CHG_MODE_BLACK){	
		GX_LoadOBJPltt( BlackPalette, palette_vram, 16*2 );	//32byte 1Palette
	}else{
		NNSG2dPaletteData *data;
		data = CLACT_U_ResManagerGetResObjResPltt(wk->ResObjTbl[ ActIDTbl[inActWorkNo] ][CLACT_U_PLTT_RES]);
		GX_LoadOBJPltt( data->pRawData, palette_vram, 16*2 );	//32byte 1Palette
	}
}

//--------------------------------------------------------------------------------------------
/**
 * 鳥スライドイン
 *
 * @param	wk				OBJワーク
 * @param	inCounter		フレームカウンタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void  OPD_Obj_SlideInBird(OPD_OBJ_PTR wk, const int inCounter)
{	
	u8 i;
	VecFx32 vec;

	const int bird_act1[3] = {
		ACT_WORK_BIRD_1,
		ACT_WORK_BIRD_2,
		ACT_WORK_BIRD_3,
	};
	const int bird_act2[1] = {
		ACT_WORK_BIRD_4,
	};
	const int bird_act3[] = {
		ACT_WORK_BIRD_5,
		ACT_WORK_BIRD_6,
	};

	if (inCounter >= FM_BIRD_IN_1){
		for (i=0;i<3;i++){
			vec =  *CLACT_GetMatrix( wk->ClActWork[ bird_act1[i] ] );
			vec.x -= BirdXSpeed1[i];
			vec.y -= (FX32_ONE*4);
			CLACT_SetMatrix( wk->ClActWork[ bird_act1[i] ], &vec );
		}
	}

	if (inCounter >= FM_BIRD_IN_2){
		for (i=0;i<1;i++){
			vec =  *CLACT_GetMatrix( wk->ClActWork[ bird_act2[i] ] );
			vec.x -= BirdXSpeed2[i];
			vec.y -= (FX32_ONE*3);
			CLACT_SetMatrix( wk->ClActWork[ bird_act2[i] ], &vec );
		}
	}

	if (inCounter >= FM_BIRD_IN_3){
		for (i=0;i<2;i++){
			vec =  *CLACT_GetMatrix( wk->ClActWork[ bird_act3[i] ] );
			vec.x -= BirdXSpeed3[i];
			vec.y -= (FX32_ONE*3);
			CLACT_SetMatrix( wk->ClActWork[ bird_act3[i] ], &vec );
		}
	}
}

//============================================================================================
/**
 * @file	opening_demo.c
 * @brief	オープニングデモ
 * @author	Nozomu Saito
 * @date	2006.05.31
 */
//============================================================================================
#include "common.h"
#include "system/palanm.h"
#include "system/lib_pack.h"
#include "system/snd_tool.h"
#include "system/brightness.h"
#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "system/wipe.h"
#include "system/wipe_def.h"
#include "system/main.h"

#include "op_demo.naix"
#include "titledemo.naix"

#include "system/particle.h"
#include "particledata/particledata.h"
#include "particledata/particledata.naix"

#include "field/field.h"
#include "demo/title.h"
#include "opening_kira_obj.h"
#include "opening_demo_obj.h"
#include "op_poke_cutin.h"

#include "op_frame_def.h"

#include "field/weather_sys.naix"	//木漏れ日

#ifdef PM_DEBUG
#define COPYRIGHT_SKIP		//<<有効にすると著作権情報をスキップできます
#endif

#ifdef PM_DEBUG
//#define DEBUG_MATSUDA_SCENE2_SKIP	//有効にしているとシーン２をスキップ
//#define DEBUG_MATSUDA_LOGO_SKIP		//有効にしているとGFロゴをスキップ
#endif

#define PAL_ONE_SIZE	(2)		//２バイト

//シーン1
#define	COPY_LIGHT_FRAME	(GF_BGL_FRAME1_M)
#define	GAME_FREAK_LOGO_M_FRAME	(GF_BGL_FRAME2_M)
#define	GAME_FREAK_LOGO_S_FRAME	(GF_BGL_FRAME2_S)
#define	BACK_BASE_M_FRAME	(GF_BGL_FRAME3_M)
#define	BACK_BASE_S_FRAME	(GF_BGL_FRAME3_S)

#define BLD_MASK_COPY_LIGHT		(GX_BLEND_PLANEMASK_BG1)
#define BLD_MASK_GF_LOGO_M		(GX_BLEND_PLANEMASK_BG2)
#define BLD_MASK_BACK_BASE_M	(GX_BLEND_PLANEMASK_BG3)
#define BLD_MASK_GF_LOGO_S		(GX_BLEND_PLANEMASK_BG2)
#define BLD_MASK_BACK_BASE_S	(GX_BLEND_PLANEMASK_BG3)

#define COPY_LIGHT_BLD_IN_COUNT_MAX	(6)	//ブレンドする速度6フレに１回変更
#define COPY_LIGHT_BLD_COUNT_MAX	(4)	//ブレンドする速度4フレに１回変更
#define OPDEMO_BRIGHTNESS_SYNC	(18)

// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ENGLISH) imatake 2007/02/07
// 北米版では ESRB Notice を下画面に追加
#if PM_LANG == LANG_ENGLISH
#define	ESRB_NOTICE_FRAME		(GF_BGL_FRAME1_S)
#define BLD_MASK_ESRB_NOTICE	(GX_BLEND_PLANEMASK_BG1)
#endif
// ----------------------------------------------------------------------------


//シーン2
#define TITLE_LOGO_WHITE_IN_SYNC	(8)
#define TITLE_LOGO_NEW_WHITE_IN_SYNC	(4)
#define BG_ON	(1)
#define BG_OFF	(2)
#define MAIN_BG0_FRAME		(GF_BGL_FRAME0_M)
#define MAIN_BG3_FRAME		(GF_BGL_FRAME3_M)
#define TOWN_FRAME		(GF_BGL_FRAME1_M)
#define SKY_FRAME		(GF_BGL_FRAME2_M)
#define WB_TITLE_FRAME	(GF_BGL_FRAME0_S)
#define SUB_BG1_FRAME	(GF_BGL_FRAME1_S)
#define SUB_BG2_FRAME	(GF_BGL_FRAME2_S)
#define SUB_BG3_FRAME	(GF_BGL_FRAME3_S)
#define SCENE2_MAPCHANGE_SYNC	(4)		//シーン2マップ切り替えのワイプ速度
#define TOWN_BG_SPEED	(-1)	//空ＢＧの移動スピード
#define SKY_BG_SPEED	(1)	//空ＢＧの移動スピード
#define BATTLE_BG_SPEED	(-16)	//バトルＢＧスクロールスピード
#define BALL_THROW_STREAM_BG_SPEED	(16)	//ボールを投げる時の流星BGスクロールスピード
#define HAND_BG_SPEED	(12)	//手がスクロールアウトする時のスピード
#define HAND_Y_BG_SPEED	(-1)	//手がスクロールアウトする時のスピード

///街並みのスクロール速度(fx32)
#define SCENE2_SCROLL_SP_TOWN		(0x0280)
///木々のスクロール速度(fx32)
#define SCENE2_SCROLL_SP_TREE		(0x0110)

#define SCENE2_POKE_APPEAR_FLASH_SYNC	(4)		//シーン2バトル面切り替えブラックインアウトスピード
#define SCENE2_POKE_APPEAR_AFTER_FLASH_SYNC	(64)		//シーン2バトル面切り替えブラックインアウトスピード
#define POKE_BG_SLIDE_IN_SPEED			(20)	//ポケモン登場スライドインの速度
#define POKE_STREAM_SPEED				(BALL_THROW_STREAM_BG_SPEED << FX32_SHIFT)

#define SCENE2_DISP_CHG_SYNC	(4)		//シーン2バトル面切り替えブラックインアウトスピード
#define SCENE2_DISP_CHG_SYNC_WAZA_AFTER	(16)		//シーン2技全て終了後ブラックインスピード
#define SCENE2_TOWN_SCROLL_MARGIN	(24)
#define SCENE2_SKY_SCROLL_MARGIN	(3)
#define SCENE2_BATTLE_SCROLL_MARGIN	(1)

#define POKE_ATTACK_SCROLL_OFFSET_X		(60)	//体当たりの距離
#define POKE_ATTACK_SCROLL_SPEED		(12)		//体当たりの速度
#define SCENE2_DISP_CHG_SYNC_THUNDER	(6)//(12)	//雷の後のフェードアウト速度

#define OP_MAP_BLOCK_MAX	(6)
#define SONOO_TOWN_HEIGHT	(FX32_ONE*16*5)
#define SCENE2_KIRA_MAX		(16)
#define	SCENE2_KIRA_MARGIN1	(4)
#define	SCENE2_KIRA_MARGIN2	(16)

//シーン3
#define MT_TENGAN_FRAME	(GF_BGL_FRAME2_M)

#define TENGAN_WH_OUT_SYNC	(18)
#define TENGAN_AFF_VAL	(0x400)
#define TENGAN_AFF_VAL2	(0x800)
//#define LAKE_AFF_VAL	(0x40)
#define AFF_LIMIT_1	(0x3000)
//#define LAVE_AFF_LIMIT	(0xa80)

#define SCENE3_START_IN_SYNC	(90)//(120)		//シーン3開始ブラックインスピード

enum{
	SCENE2_BG_TRANS_STATUS_NORMAL,
	SCENE2_BG_TRANS_STATUS_POKE,		//ポケモンBG展開後
	SCENE2_BG_TRANS_STATUS_THUNDER,		//雷展開後
};

typedef enum{
	DISP_MAIN,
	DISP_SUB,
};

typedef struct BLOCK_3D_DATA_tag
{
	NNSG3dRenderObj			RenderObj;
	NNSG3dResFileHeader* ResFile;
}BLOCK_3D_DATA;

typedef struct SCENE2_3D_DATA_tag
{
	void			*TexFile;		//テクスチャーファイル実体へのポインタ
	NNSG3dResTex	*Texture;
	GF_CAMERA_PTR CameraPtr;		//カメラ
	BLOCK_3D_DATA	Block[OP_MAP_BLOCK_MAX];
	int block_num;		///<Blockに展開している数
}SCENE2_3D_DATA;

typedef struct SCENE3_THUNDER_DATA_tag
{
	u8 Request;
	u8 NowPalTblNo;	//パレットテーブル番号
	u8 Margin;		//パレット変更間隔
	u8 Counter;
}SCENE3_THUNDER_DATA;


//コピーライト～ゲーフリロゴまでのデータ構造体
typedef struct SCENE1_WORK_tag
{
	u8 Seq;
	u8 SceneSeq;
	u8 InitOK;			//initシーケンス通過したか？
	u8 LoadOK;			//loadシーケンス通過したか？
	u16 Wait;
	u8 Alpha;
	u8 Counter;
	u8 BlendEnd;
	GF_BGL_INI *bgl;
	TCB_PTR	BlendTcb;
	OPD_K_G_MNG_PTR KiraGeneMngPtr;
	u8 *SkipOKPtr;
}SCENE1_WORK;

//タイトル～バトルまでのデータ構造体
typedef struct SCENE2_WORK_tag
{
	u8 Seq;
	u8 SceneSeq;
	u8 InitOK;				//initシーケンス通過したか？
	u8 LoadOK;				//loadシーケンス通過したか？
	u8 Load3DOK;			//load3Dシーケンスを通過したか？
	u8 Load3D_DivMode;		//分割読み込み中
	u8 Disp3DFlg;			//空撮フラグ
	u8 KiraMargin;
	u8 KiraMarginCore;
	u8 div_seq;
	int BG_ScrollMargin;	//スクロールセット間隔
	GF_BGL_INI *bgl;
	OPD_OBJ_PTR	ObjWorkPtr;
	OPD_KIRA_DATLST_PTR KiraListPtr;
	OPD_SSM_PTR OpSsmPtr;
	SCENE2_3D_DATA Data;
	
	u16 *pokebg_palbuf;

	u8 kira_vanish;			///<TRUE:キラキラアクター非表示状態
	u8 window_on_req;		///<TRUE:WINDOW ON リクエスト
	u8 window_off_req;		///<TRUE:WINDOW OFF リクエスト
	u8 bg_visible_on_req;	///<VblankでBGの表示設定を行う(ビット指定)
	u8 bg_visible_off_req;	///<VblankでBGの非表示設定を行う(ビット指定)
	
	s16 wnd0_m_left;		///<ウィンドウ0(メイン画面)の左座標
	s16 wnd0_m_right;		///<ウィンドウ0(メイン画面)の右座標
	s16 wnd0_m_top;			///<ウィンドウ0(メイン画面)の上座標
	s16 wnd0_m_bottom;		///<ウィンドウ0(メイン画面)の下座標
	s16 wnd0_s_left;		///<ウィンドウ0(サブ画面)の左座標
	s16 wnd0_s_right;		///<ウィンドウ0(サブ画面)の右座標
	s16 wnd0_s_top;			///<ウィンドウ0(サブ画面)の上座標
	s16 wnd0_s_bottom;		///<ウィンドウ0(サブ画面)の下座標

	u8 bg_trans_status;		///<0=雷BG展開前のBG構成、TRUE=展開後のBG構成
	u8 v_req_scene2_map_disp_change;	///<TRUE:Vブランクでメインとサブの切り替え
	u8 map_draw_ok;			///<TRUE:マップ描画OK
	u8 particle_occ;		///<TRUE:パーティクルシステム有効
	
	fx32 scroll_sp_town;	///<背景(街)のスクロール速度
	fx32 scroll_sp_tree;	///<背景(木)のスクロール速度
}SCENE2_WORK;

//テンガンザンシーンのデータ構造体
typedef struct SCENE3_WORK_tag
{
	u8 Seq;
	u8 SceneSeq;
	u8 InitOK;			//initシーケンス通過したか？
	u8 LoadOK;			//loadシーケンス通過したか？
//	u8 TenganAffMargin;	//テンガンザンスケール変更間隔
//	u8 LakeAffMargin;	//湖スケール変更間隔
	
	fx32 akagi_scr_y;	//アカギBGスクロール値
	GF_BGL_INI *bgl;
	SCENE3_THUNDER_DATA ThunderData;
}SCENE3_WORK;

///	タイトル全体制御用ワーク構造体
typedef struct OPENING_DEMO_WORK_tag
{
	int	heapID;
	int FrameCounter;
	BOOL SkipFlg;

	GF_BGL_INI*		bgl;	// BGマネージャ
	GF_G3DMAN*		g3Dman;	// 3Dマネージャ

	u32 RandSeed;

	SCENE1_WORK		SceneWork1;
	SCENE2_WORK		SceneWork2;
	SCENE3_WORK		SceneWork3;

	u8 SkipOK;

}OPENING_DEMO_WORK;

extern const PROC_DATA TitleProcData;
extern void		Main_SetNextProc(FSOverlayID ov_id, const PROC_DATA * proc_data);
FS_EXTERN_OVERLAY( title );

static PROC_RESULT OpDemo_Init(PROC * proc, int * seq);
static PROC_RESULT OpDemo_Main(PROC * proc, int * seq);
static PROC_RESULT OpDemo_Exit(PROC * proc, int * seq);

static BOOL GemeFreakLogoScene(OPENING_DEMO_WORK * wk);
static BOOL Title_3D_BattleScene(OPENING_DEMO_WORK * wk);
static BOOL MtTenganScene(OPENING_DEMO_WORK * wk);

static void SetUp3D(void);

static void Scene2_SetWindow(void);
static void Scene2_WindowOff(void);

static void Scene2_Load3D_Exit(OPENING_DEMO_WORK *wk);
static void Scene2_Load2D_BG_Exit(OPENING_DEMO_WORK *wk, BOOL bgl_free);

//=================================================================================================
//
// ＰＲＯＣ定義テーブル
//
//=================================================================================================
const PROC_DATA OpDemoProcData = {
	OpDemo_Init,
	OpDemo_Main,
	OpDemo_Exit,
	NO_OVERLAY_ID
};

static const int TexsetArcTbl[] = {
	NARC_op_demo_op_map01_texset_nsbtx,
	NARC_op_demo_op_map02_texset_nsbtx,
	NARC_op_demo_op_map03_texset_nsbtx,
};

const int ModelArcTbl[][OP_MAP_BLOCK_MAX] = {
	{
		NARC_op_demo_op_map01_00_00_nsbmd,
		NARC_op_demo_op_map01_00_01_nsbmd,
		0,
		0,
		0,
		0,
	#if 0
		NARC_op_demo_titlemap05_20_nsbmd,
		NARC_op_demo_titlemap05_21_nsbmd,
		NARC_op_demo_titlemap06_20_nsbmd,
		NARC_op_demo_titlemap06_19_nsbmd,
		NARC_op_demo_titlemap06_18_nsbmd,
		NARC_op_demo_titlemap06_17_nsbmd, 
	#endif
	},
	{
		NARC_op_demo_op_map02_00_00_nsbmd,
		NARC_op_demo_op_map02_00_01_nsbmd,
		NARC_op_demo_op_map02_01_00_nsbmd,
		NARC_op_demo_op_map02_01_01_nsbmd,
		NARC_op_demo_op_map02_02_00_nsbmd,
		NARC_op_demo_op_map02_02_01_nsbmd,
	},
	{
		NARC_op_demo_op_map03_00_00_nsbmd,
		NARC_op_demo_op_map03_00_01_nsbmd,
		0,
		0,
		0,
		0,
	},
};

//カメラ設定
static const GF_CAMERA_PARAM CameraData = {
	0x29aec1,
	{
		-0x29fe,0,0
	},
	GF_CAMERA_PERSPECTIV,
	0x05c1,
	0	//dummy
};

//エッジマーキングテーブル
static const GXRgb gEdgeColor[8] = {
    GX_RGB(0, 0, 0),
    GX_RGB(4, 4, 4),
    GX_RGB(4, 4, 4),
    GX_RGB(4, 4, 4),
    GX_RGB(4, 4, 4),
    GX_RGB(4, 4, 4),
    GX_RGB(4, 4, 4),
    GX_RGB(4, 4, 4)
};

//ライトパラメータ
const VecFx32 LightVec[4] = {
	{-2043,-3548,110},
	{0,0,0},
	{0,0,4096},
	{0,0,4096},
};

const GXRgb LightColor[4] = {
	GX_RGB(24,24,18),
	GX_RGB(0,0,0),
	GX_RGB(2,2,12),
	GX_RGB(31,31,31),	
};

//----------------------------------
//ＶＲＡＭ設定
//----------------------------------
static void OpDemo_VramBankSet(void)
{
	GF_BGL_DISPVRAM vramSetTable = {
		GX_VRAM_BG_128_B,				// メイン2DエンジンのBG
		GX_VRAM_BGEXTPLTT_NONE,			// メイン2DエンジンのBG拡張パレット
		GX_VRAM_SUB_BG_128_C,			// サブ2DエンジンのBG
		GX_VRAM_SUB_BGEXTPLTT_NONE,	// サブ2DエンジンのBG拡張パレット
		GX_VRAM_OBJ_80_EF,				// メイン2DエンジンのOBJ
		GX_VRAM_OBJEXTPLTT_NONE,		// メイン2DエンジンのOBJ拡張パレット
		GX_VRAM_SUB_OBJ_16_I,			// サブ2DエンジンのOBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット
		GX_VRAM_TEX_0_A,				// テクスチャイメージスロット
		GX_VRAM_TEXPLTT_0_G				// テクスチャパレットスロット
	};
	GF_Disp_SetBank( &vramSetTable );
}
//----------------------------------
//ＶＲＡＭ設定（タイトル～3Ｄ～バトル）
//----------------------------------
static void OpDemo_Scene2_VramBankSet(void)
{
	GF_BGL_DISPVRAM vramSetTable = {
		GX_VRAM_BG_128_B,				// メイン2DエンジンのBG
		GX_VRAM_BGEXTPLTT_NONE,			// メイン2DエンジンのBG拡張パレット
		GX_VRAM_SUB_BG_128_C,			// サブ2DエンジンのBG
		GX_VRAM_SUB_BGEXTPLTT_NONE,	// サブ2DエンジンのBG拡張パレット
		GX_VRAM_OBJ_32_FG,				// メイン2DエンジンのOBJ
		GX_VRAM_OBJEXTPLTT_NONE,		// メイン2DエンジンのOBJ拡張パレット
		GX_VRAM_SUB_OBJ_16_I,			// サブ2DエンジンのOBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット
		GX_VRAM_TEX_0_A,				// テクスチャイメージスロット
		GX_VRAM_TEXPLTT_0123_E				// テクスチャパレットスロット
	};
	GF_Disp_SetBank( &vramSetTable );
}

//----------------------------------
//ＶＲＡＭ設定（テンガンザン）
//----------------------------------
static void OpDemo_Scene3_VramBankSet(void)
{
	GF_BGL_DISPVRAM vramSetTable = {
		GX_VRAM_BG_256_AB,				// メイン2DエンジンのBG
		GX_VRAM_BGEXTPLTT_NONE,			// メイン2DエンジンのBG拡張パレット
		GX_VRAM_SUB_BG_128_C,			// サブ2DエンジンのBG
		GX_VRAM_SUB_BGEXTPLTT_NONE,	// サブ2DエンジンのBG拡張パレット
		GX_VRAM_OBJ_NONE,				// メイン2DエンジンのOBJ
		GX_VRAM_OBJEXTPLTT_NONE,		// メイン2DエンジンのOBJ拡張パレット
		GX_VRAM_SUB_OBJ_NONE,			// サブ2DエンジンのOBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット
		GX_VRAM_TEX_NONE,				// テクスチャイメージスロット
		GX_VRAM_TEXPLTT_NONE				// テクスチャパレットスロット
	};
	GF_Disp_SetBank( &vramSetTable );
}

//----------------------------------
//
//	２ＤＢＧ初期化＆終了
//
//----------------------------------
static void OpDemo2DBgSet( OPENING_DEMO_WORK* wk )
{
	//BGライブラリ用メモリ確保
	wk->bgl = GF_BGL_BglIniAlloc(wk->heapID);

	{	// BGシステム設定
		GF_BGL_SYS_HEADER BGsys_data = {
			GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_2D
		};	
		GF_BGL_InitBG( &BGsys_data );
	}

	
	{	// コピーライト ＢＧ設定(メイン1)
		GF_BGL_BGCNT_HEADER CopyLight_Data = {
			0, 0, 0x800, 0,
			GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE			//プライオリティ0
		};
		GF_BGL_BGControlSet( wk->bgl, COPY_LIGHT_FRAME, &CopyLight_Data, GF_BGL_MODE_TEXT );
	}

	{	// ゲーフリロゴ ＢＧ設定(メイン2)
		GF_BGL_BGCNT_HEADER GF_Logo_m_Data = {
			0, 0, 0x800, 0,
			GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x08000,
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE			//プライオリティ1
		};
		GF_BGL_BGControlSet( wk->bgl, GAME_FREAK_LOGO_M_FRAME, &GF_Logo_m_Data, GF_BGL_MODE_TEXT );
	}

	{	// ベース背景 ＢＧ設定(メイン3)
		GF_BGL_BGCNT_HEADER GF_Back_m_Data = {
			0, 0, 0x800, 0,
			GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x08000,
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE			//プライオリティ2
		};
		GF_BGL_BGControlSet( wk->bgl, BACK_BASE_M_FRAME, &GF_Back_m_Data, GF_BGL_MODE_TEXT );
	}

// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ENGLISH) imatake 2007/02/07
// 北米版では ESRB Notice を下画面に追加

#if PM_LANG == LANG_ENGLISH
	{	// コピーライト ＢＧ設定(サブ1)
		GF_BGL_BGCNT_HEADER EsrbNotice_Data = {
			0, 0, 0x800, 0,
			GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x08000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE			//プライオリティ0
		};
		GF_BGL_BGControlSet( wk->bgl, ESRB_NOTICE_FRAME, &EsrbNotice_Data, GF_BGL_MODE_TEXT );
// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ENGLISH) imatake 2007/02/10
// 体験版では ESRB Notice を表示しないように変更
#ifdef PG5_TRIAL
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
#endif
// ----------------------------------------------------------------------------
	}
#endif

// ----------------------------------------------------------------------------

	{	// ゲーフリロゴ ＢＧ設定(サブ2)
		GF_BGL_BGCNT_HEADER GF_Logo_s_Data = {
			0, 0, 0x800, 0,
			GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000,
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE			//プライオリティ1
		};
		GF_BGL_BGControlSet( wk->bgl, GAME_FREAK_LOGO_S_FRAME, &GF_Logo_s_Data, GF_BGL_MODE_TEXT );
	}

// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ENGLISH) imatake 2007/02/07
// 北米版では ESRB Notice を下画面に追加
	{	// ベース背景 ＢＧ設定(サブ3)
		GF_BGL_BGCNT_HEADER GF_Back_s_Data = {
			0, 0, 0x800, 0,
			GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x04000,
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE			//プライオリティ2
		};
		GF_BGL_BGControlSet( wk->bgl, BACK_BASE_S_FRAME, &GF_Back_s_Data, GF_BGL_MODE_TEXT );
	}
// ----------------------------------------------------------------------------


	GF_BGL_BackGroundColorSet( GF_BGL_FRAME0_M, 0x0000 );	//背景色初期化（メイン画面）
	GF_BGL_BackGroundColorSet( GF_BGL_FRAME0_S, 0x0000 );	//背景色初期化（サブ画面）
}

//----------------------------------
//
//	２ＤＢＧ初期化＆終了
//
//----------------------------------
static void OpDemo_Scene2_2DBgSet( OPENING_DEMO_WORK* wk )
{
	//BGライブラリ用メモリ確保
	wk->bgl = GF_BGL_BglIniAlloc(wk->heapID);

	{	// BGシステム設定
		GF_BGL_SYS_HEADER BGsys_data = {
			GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_3D
		};	
		GF_BGL_InitBG( &BGsys_data );
	}

	{	// 街 ＢＧ設定(メイン1)	木漏れ日
		GF_BGL_BGCNT_HEADER Town_Data = {
			0, 0, 0, 0,
			GF_BGL_SCRSIZ_512x512, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x10000,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE			//プライオリティ3
		};
		GF_BGL_BGControlSet( wk->bgl, TOWN_FRAME, &Town_Data, GF_BGL_MODE_TEXT );
	}

	{	// 空 ＢＧ設定(メイン2)	GFロゴ
		GF_BGL_BGCNT_HEADER Sky_Data = {
			0, 0, 0, 0,
			GF_BGL_SCRSIZ_256x512, GX_BG_COLORMODE_256,
			GX_BG_SCRBASE_0x4000, GX_BG_CHARBASE_0x18000,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE			//プライオリティ3
		};
		GF_BGL_BGControlSet( wk->bgl, SKY_FRAME, &Sky_Data, GF_BGL_MODE_TEXT );
	}

	{	// ＢＧ設定(メイン3)	GFロゴ
		GF_BGL_BGCNT_HEADER bg3_Data = {
			0, 0, 0, 0,
			GF_BGL_SCRSIZ_256x512, GX_BG_COLORMODE_256,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x08000,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE			//プライオリティ3
		};
		GF_BGL_BGControlSet( wk->bgl, MAIN_BG3_FRAME, &bg3_Data, GF_BGL_MODE_TEXT );
	}

	//サブ面
	{	// ゲーフリロゴ ＢＧ設定(サブ0)
		GF_BGL_BGCNT_HEADER GF_Logo_Data = {
			0, 0, 0x1000, 0,
			GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_256,
			GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x14000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE			//プライオリティ0
		};
		GF_BGL_BGControlSet( wk->bgl, WB_TITLE_FRAME, &GF_Logo_Data, GF_BGL_MODE_TEXT );
	}
	
	{	//サブ面残り
		GF_BGL_BGCNT_HEADER SubBg_Data[] = {
			{
				0, 0, 0, 0,
				GF_BGL_SCRSIZ_512x512, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x08000,
				GX_BG_EXTPLTT_01, 3, 0, 0, FALSE			//プライオリティ3
			},
			{
				0, 0, 0, 0,
				GF_BGL_SCRSIZ_256x512, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x0c000,
				GX_BG_EXTPLTT_01, 3, 0, 0, FALSE			//プライオリティ3
			},
			{
				0, 0, 0, 0,
				GF_BGL_SCRSIZ_256x512, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x4000, GX_BG_CHARBASE_0x10000,
				GX_BG_EXTPLTT_01, 3, 0, 0, FALSE			//プライオリティ3
			},
		};
		GF_BGL_BGControlSet( wk->bgl, SUB_BG1_FRAME, &SubBg_Data[0], GF_BGL_MODE_TEXT );
		GF_BGL_BGControlSet( wk->bgl, SUB_BG2_FRAME, &SubBg_Data[1], GF_BGL_MODE_TEXT );
		GF_BGL_BGControlSet( wk->bgl, SUB_BG3_FRAME, &SubBg_Data[2], GF_BGL_MODE_TEXT );
	}

	GF_BGL_BackGroundColorSet( GF_BGL_FRAME1_M, 0x0000 );	//背景色初期化（メイン画面）
	GF_BGL_BackGroundColorSet( GF_BGL_FRAME0_S, 0x0000 );	//背景色初期化（サブ画面）
}

//--------------------------------------------------------------
//	ポケモンBG展開の為のBG構成設定
//--------------------------------------------------------------
static void OpDemo_Scene2_2DBgSet_PokeBG( OPENING_DEMO_WORK* wk )
{
	Scene2_Load2D_BG_Exit(wk, FALSE);	//一旦BG構成を破棄

	{	// ＢＧ設定(メイン3)
		GF_BGL_BGCNT_HEADER bg3_Data = {
			0, 0, 0, 0,
			GF_BGL_SCRSIZ_512x512, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x08000,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE			//プライオリティ3
		};
		GF_BGL_BGControlSet( wk->bgl, MAIN_BG3_FRAME, &bg3_Data, GF_BGL_MODE_TEXT );
	}

	{	// 街 ＢＧ設定(メイン1)
		GF_BGL_BGCNT_HEADER Town_Data = {
			0, 0, 0, 0,
			GF_BGL_SCRSIZ_512x512, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x0c000,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE			//プライオリティ3
		};
		GF_BGL_BGControlSet( wk->bgl, TOWN_FRAME, &Town_Data, GF_BGL_MODE_TEXT );
	}

	{	// 空 ＢＧ設定(メイン2)
		GF_BGL_BGCNT_HEADER Sky_Data = {
			0, 0, 0, 0,
			GF_BGL_SCRSIZ_512x512, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x4000, GX_BG_CHARBASE_0x10000,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE			//プライオリティ3
		};
		GF_BGL_BGControlSet( wk->bgl, SKY_FRAME, &Sky_Data, GF_BGL_MODE_TEXT );
	}

	//サブ面
	{	//サブ面残り
		GF_BGL_BGCNT_HEADER SubBg_Data[] = {
			{
				0, 0, 0, 0,
				GF_BGL_SCRSIZ_512x512, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x08000,
				GX_BG_EXTPLTT_01, 3, 0, 0, FALSE			//プライオリティ3
			},
			{
				0, 0, 0, 0,
				GF_BGL_SCRSIZ_512x512, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x0c000,
				GX_BG_EXTPLTT_01, 3, 0, 0, FALSE			//プライオリティ3
			},
			{
				0, 0, 0, 0,
				GF_BGL_SCRSIZ_512x512, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x4000, GX_BG_CHARBASE_0x10000,
				GX_BG_EXTPLTT_01, 3, 0, 0, FALSE			//プライオリティ3
			},
		};
		GF_BGL_BGControlSet( wk->bgl, SUB_BG1_FRAME, &SubBg_Data[0], GF_BGL_MODE_TEXT );
		GF_BGL_BGControlSet( wk->bgl, SUB_BG2_FRAME, &SubBg_Data[1], GF_BGL_MODE_TEXT );
		GF_BGL_BGControlSet( wk->bgl, SUB_BG3_FRAME, &SubBg_Data[2], GF_BGL_MODE_TEXT );
	}

	GF_BGL_BackGroundColorSet( GF_BGL_FRAME1_M, 0x0000 );	//背景色初期化（メイン画面）
	GF_BGL_BackGroundColorSet( GF_BGL_FRAME0_S, 0x0000 );	//背景色初期化（サブ画面）

	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );	//GFロゴOFF

	wk->SceneWork2.bg_trans_status = SCENE2_BG_TRANS_STATUS_POKE;
}

//--------------------------------------------------------------
//	雷BG展開の為のBG構成設定
//--------------------------------------------------------------
static void OpDemo_Scene2_2DBgSet_Thunder( OPENING_DEMO_WORK* wk )
{
	Scene2_Load3D_Exit(wk);		//3D破棄
	//Scene2_Load2D_BG_Exit(wk, FALSE);	//一旦BG構成を破棄
	OpDemo_Scene2_2DBgSet_PokeBG(wk);	//一旦BG構成を破棄＆ポケモンBG構成状態にはしておく
	
	//今あるBGLに追加する
	{	// BGシステム設定
		GF_BGL_SYS_HEADER BGsys_data = {
			GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_2D
		};	
		GF_BGL_InitBG( &BGsys_data );
	}
	
	{	//雷BG設定(メイン0)
		GF_BGL_BGCNT_HEADER bg0_Data = {
			0, 0, 0, 0,
			GF_BGL_SCRSIZ_256x512, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x14000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE			//プライオリティ0
		};
		GF_BGL_BGControlSet( wk->bgl, MAIN_BG0_FRAME, &bg0_Data, GF_BGL_MODE_TEXT );
	}
	
	//サブ面
	{	// 雷 ＢＧ設定(サブ0)
		GF_BGL_BGCNT_HEADER sub_bg0_Data = {
			0, 0, 0, 0,
			GF_BGL_SCRSIZ_256x512, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x14000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE			//プライオリティ0
		};
		GF_BGL_BGControlSet( wk->bgl, WB_TITLE_FRAME, &sub_bg0_Data, GF_BGL_MODE_TEXT );
	}

	GF_BGL_ScrollSet( wk->bgl, MAIN_BG0_FRAME, GF_BGL_SCROLL_X_SET, 0 );
	GF_BGL_ScrollSet( wk->bgl, MAIN_BG0_FRAME, GF_BGL_SCROLL_Y_SET, 0 );
	GF_BGL_ScrollSet( wk->bgl, WB_TITLE_FRAME, GF_BGL_SCROLL_X_SET, 0 );
	GF_BGL_ScrollSet( wk->bgl, WB_TITLE_FRAME, GF_BGL_SCROLL_Y_SET, 0 );
	
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
	
	wk->SceneWork2.bg_trans_status = SCENE2_BG_TRANS_STATUS_THUNDER;
}

//----------------------------------
//
//	２ＤＢＧ初期化
//
//----------------------------------
static void OpDemo_Scene3_2DBgSet( OPENING_DEMO_WORK* wk )
{
	//BGライブラリ用メモリ確保
	wk->bgl = GF_BGL_BglIniAlloc(wk->heapID);

	{	// BGシステム設定
		GF_BGL_SYS_HEADER BGsys_data = {
			GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_2D
		};	
		GF_BGL_InitBG( &BGsys_data );
	}
	
	{	// テンガンザン山 ＢＧ設定(メイン2)
		GF_BGL_BGCNT_HEADER Tengan_Data = {
			0, 0, 0x1000, 0,
			GF_BGL_SCRSIZ_256x512, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x08000,
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE			//プライオリティ1
		};
		GF_BGL_BGControlSet( wk->bgl, MT_TENGAN_FRAME, &Tengan_Data, GF_BGL_MODE_TEXT );
	}

	GF_BGL_BackGroundColorSet( MT_TENGAN_FRAME, 0x0000 );	//背景色初期化（メイン画面）
}

//----------------------------------
//
//	３Ｄ初期化＆終了
//
//----------------------------------
static void OpDemo3D_Init( OPENING_DEMO_WORK* wk )
{
	NNSGfdTexKey texKey;
	NNSGfdPlttKey plttKey;
	u32 tex_addrs, pltt_addrs;
	
	wk->g3Dman = GF_G3DMAN_Init(wk->heapID, 
			GF_G3DMAN_LNK, GF_G3DTEX_128K, GF_G3DMAN_LNK, GF_G3DPLT_64K, SetUp3D );

	
	// ポケモンデータ転送領域のVramを確保＆転送アドレスとサイズを設定	
	texKey = NNS_GfdAllocTexVram(0x2000*4, FALSE, 0);
	plttKey = NNS_GfdAllocPlttVram(0x20*4, FALSE, NNS_GFD_ALLOC_FROM_LOW);

	tex_addrs = NNS_GfdGetTexKeyAddr(texKey);
	pltt_addrs = NNS_GfdGetPlttKeyAddr(plttKey);
	OS_TPrintf("オープニングポケモン用に確保したテクスチャVramの先頭アドレス＝%d\n", tex_addrs);
	OS_TPrintf("オープニングポケモン用に確保したパレットVramの先頭アドレス＝%d\n", pltt_addrs);
}

static void SetUp3D(void)
{
	NNS_G3dGeFlushBuffer();
	
	// ３Ｄ使用面の設定(表示＆プライオリティー)
    G2_SetBG0Priority(1);

	// 各種描画モードの設定(シェード＆アンチエイリアス＆半透明)
    G3X_SetShading( GX_SHADING_TOON );
    G3X_AntiAlias( TRUE );
	G3X_AlphaTest( FALSE, 0 );	// アルファテスト　　オフ
	G3X_AlphaBlend( TRUE );		// アルファブレンド　オン
	G3X_EdgeMarking( TRUE );
	G3X_SetEdgeColorTable(gEdgeColor);
	G3X_SetFog( FALSE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x8000, 0 );

	// クリアカラーの設定
    G3X_SetClearColor(GX_RGB(0,0,0),0,0x7fff,63,FALSE);	//color,alpha,depth,polygonID,fog

	// ビューポートの設定
    G3_ViewPort(0, 0, 255, 191);
}

//----------------------------------
//ＶＢＬＡＮＫ関数
//----------------------------------
static void Scene1_VBlankFunc(void* work)
{
	
	OPENING_DEMO_WORK* wk = work;
//	GF_BGL_VBlankFunc(wk->bgl);

	// レンダラ共有OAMマネージャVram転送
	REND_OAMTrans();

}

//----------------------------------
//ＶＢＬＡＮＫ関数
//----------------------------------
static void Scene2_VBlankFunc(void* work)
{
	int frame_no;
	OPENING_DEMO_WORK* wk = work;
	SCENE2_WORK *scene_wk = &wk->SceneWork2;
	
	if(scene_wk->v_req_scene2_map_disp_change == TRUE){
		//３Ｄ描画スイッチ
		sys.disp3DSW = DISP_3D_TO_SUB;
		GF_Disp_DispSelect();
		
		//サブ画面のGFロゴを表示し、メイン画面のは非表示にする
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
		scene_wk->v_req_scene2_map_disp_change = 0;

		//カメラをマップに戻す
		GFC_SetCameraView(GF_CAMERA_PERSPECTIV, scene_wk->Data.CameraPtr);
		GFC_AttachCamera(scene_wk->Data.CameraPtr);
	}
	
	if(wk->bgl != NULL){
		GF_BGL_VBlankFunc(wk->bgl);
	}

	for(frame_no = 0; frame_no < GF_BGL_FRAME3_S+1; frame_no++){
		if(scene_wk->bg_visible_on_req & 1){
			GF_BGL_VisibleSet(frame_no, VISIBLE_ON);
		}
		if(scene_wk->bg_visible_off_req & 1){
			GF_BGL_VisibleSet(frame_no, VISIBLE_OFF);
		}
		scene_wk->bg_visible_on_req >>= 1;
		scene_wk->bg_visible_off_req >>= 1;
	}

	G2_SetWnd0Position( scene_wk->wnd0_m_left, scene_wk->wnd0_m_top, 
		scene_wk->wnd0_m_right, scene_wk->wnd0_m_bottom);
	G2S_SetWnd0Position( scene_wk->wnd0_s_left, scene_wk->wnd0_s_top, 
		scene_wk->wnd0_s_right, scene_wk->wnd0_s_bottom);
	if(scene_wk->window_on_req == TRUE){
		Scene2_SetWindow();
		scene_wk->window_on_req = FALSE;
	}
	if(scene_wk->window_off_req == TRUE){
		Scene2_WindowOff();
		scene_wk->window_off_req = FALSE;
	}

	DoVramTransferManager();
	
	// レンダラ共有OAMマネージャVram転送
	REND_OAMTrans();

	OPD_Poke_VBlank(wk->SceneWork2.OpSsmPtr);

}

//--------------------------------------------------------------------------------------------
/**
 * ウィンドウ設定
 *
 * @param	none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void Scene2_SetWindow(void)
{
	//メイン
	GX_SetVisibleWnd( GX_WNDMASK_W0 );
	G2_SetWnd0InsidePlane(	GX_WND_PLANEMASK_BG0|
							GX_WND_PLANEMASK_BG1|
							GX_WND_PLANEMASK_BG2|
							GX_WND_PLANEMASK_BG3|
							GX_WND_PLANEMASK_OBJ, 1 );
	G2_SetWndOutsidePlane( GX_WND_PLANEMASK_BG0|GX_WND_PLANEMASK_OBJ, 1 );

	//サブ
	GXS_SetVisibleWnd( GX_WNDMASK_W0 );
	G2S_SetWnd0InsidePlane(	GX_WND_PLANEMASK_BG0|
							GX_WND_PLANEMASK_BG1|
							GX_WND_PLANEMASK_BG2|
							GX_WND_PLANEMASK_BG3|
							GX_WND_PLANEMASK_OBJ, 1 );
	G2S_SetWndOutsidePlane( GX_WND_PLANEMASK_BG0|GX_WND_PLANEMASK_OBJ, 1 );
}

static void Scene2_WindowOff(void)
{
	GX_SetVisibleWnd( 0 );
	GXS_SetVisibleWnd( 0 );
}

//----------------------------------
//ＰＲＯＣ初期化
//----------------------------------
static PROC_RESULT OpDemo_Init(PROC * proc, int * seq)
{
	OPENING_DEMO_WORK * wk;
	int	heapID;

	heapID = HEAPID_OP_DEMO;

	BrightnessChgInit();
	
	//マスター輝度をあげておく
	WIPE_SetBrightness( WIPE_DISP_MAIN,WIPE_FADE_WHITE );
	WIPE_SetBrightness( WIPE_DISP_SUB,WIPE_FADE_WHITE );
	
	sys_VBlankFuncChange( NULL, NULL );	// VBlankセット
	sys_HBlankIntrSet( NULL,NULL );		// HBlankセット

	GF_Disp_GX_VisibleControlInit();
	GF_Disp_GXS_VisibleControlInit();
///	GX_SetVisiblePlane(0);
///	GXS_SetVisiblePlane(0);

	sys_KeyRepeatSpeedSet( SYS_KEYREPEAT_SPEED_DEF, SYS_KEYREPEAT_WAIT_DEF );
	sys_CreateHeap( HEAPID_BASE_APP, heapID, 0xa0000 );

	wk = PROC_AllocWork(proc,sizeof(OPENING_DEMO_WORK),heapID);
	memset(wk,0,sizeof(OPENING_DEMO_WORK));

	wk->heapID = heapID;

	wk->SkipFlg = FALSE;

#ifdef COPYRIGHT_SKIP
	wk->SkipOK = 1;
#else
	//始めはスキップ不可
	wk->SkipOK = 0;
#endif
	//３Ｄ描画スイッチ
	sys.disp3DSW = DISP_3D_TO_MAIN;
	GF_Disp_DispSelect();

	//乱数の種退避
	wk->RandSeed = gf_get_seed();
	
	//乱数初期化
	gf_srand(0);

	return	PROC_RES_FINISH;
}

//----------------------------------
//ＰＲＯＣメイン
//----------------------------------
static PROC_RESULT OpDemo_Main(PROC * proc, int * seq)
{
	OPENING_DEMO_WORK * wk = PROC_GetWork( proc );
#if 0	
	if (sys.trg & PAD_BUTTON_X){
		OS_Printf("frame:%d\n",wk->FrameCounter);
		GF_ASSERT(0);
	}
#endif

#ifdef DEBUG_MATSUDA_SCENE2_SKIP
	if(wk->SkipOK){
		sys.trg = PAD_BUTTON_A;
	}
#endif

	if ( (wk->SkipOK)&&( (sys.trg & PAD_BUTTON_A)||(sys.trg & PAD_BUTTON_START) ) )
	{
		wk->SkipFlg = TRUE;
		sys.DS_Boot_Flag = FALSE;	//ブートフラグセット
		WIPE_SetBrightness( WIPE_DISP_MAIN,WIPE_FADE_BLACK );
		WIPE_SetBrightness( WIPE_DISP_SUB,WIPE_FADE_BLACK );
	}

	switch(*seq){
	case 0:
		wk->SceneWork1.SkipOKPtr = &wk->SkipOK;
		(*seq)++;
		break;
	case 1:	
		if ( GemeFreakLogoScene(wk) ){
			//次のシーケンス
			(*seq)++;
		}
		break;
	case 2:
		if ( Title_3D_BattleScene(wk) ){
			//次のシーケンス
			(*seq)++;
		}
		break;
	case 3:
		if ( MtTenganScene(wk) ){
			//次のシーケンス
			(*seq)++;
		}
		break;
	case 4:
		if (wk->FrameCounter >= FM_END){
			return	PROC_RES_FINISH;
		}
		break;
	default:
		GF_ASSERT(0);
		break;
	}

	if (wk->SkipFlg){
		OS_Printf("スキップにより、メインプロセス終了\n");
		return	PROC_RES_FINISH;
	}

	wk->FrameCounter++;
	OS_TPrintf("wk->FrameCounter = %d\n", wk->FrameCounter);
	return	PROC_RES_CONTINUE;
}

static PROC_RESULT OpDemo_Exit(PROC * proc, int * seq)
{
	OPENING_DEMO_WORK * wk = PROC_GetWork( proc );
	//ワイプの強制終了
	if(WIPE_SYS_EndCheck() == FALSE){
		WIPE_SYS_ExeEnd();
	}

	//乱数の種復帰
	gf_srand(wk->RandSeed);
	
	PROC_FreeWork( proc );				// ワーク開放
	sys_DeleteHeap( HEAPID_OP_DEMO );
	
	Main_SetNextProc( FS_OVERLAY_ID(title), &TitleProcData);
	
	return	PROC_RES_FINISH;
}

static void Scene1_Init(OPENING_DEMO_WORK * wk);
static void Scene1_Load(SCENE1_WORK *scene_wk);
static BOOL Scene1_Main(SCENE1_WORK *scene_wk, const int inCounter);
static void Scene1_End(OPENING_DEMO_WORK * wk);
static void TCB_BlendInCopyLight( TCB_PTR tcb, void* work );
static void TCB_BlendOutCopyLight( TCB_PTR tcb, void* work );
static void TCB_BlendInGFLogoM( TCB_PTR tcb, void* work );
static void TCB_BlendInGFLogoS( TCB_PTR tcb, void* work );

//コピーライト・ゲーフリロゴの表示シーン
static BOOL GemeFreakLogoScene(OPENING_DEMO_WORK * wk)
{
	u8 *seq = &(wk->SceneWork1.Seq);
	
	if (wk->SkipFlg){
		(*seq) = 3;		//スキップ実行
	}
	
	switch(*seq){
	case 0:		//初期化
		Scene1_Init(wk);
		(*seq)++;
		break;
	case 1:			//グラフィックロード
		Scene1_Load(&wk->SceneWork1);
		(*seq)++;
		break;
	case 2:			//メイン
	#ifdef DEBUG_MATSUDA_LOGO_SKIP
		wk->FrameCounter = FM_TITLE_LOGO_IN - 10;
		(*seq)++;
		break;
	#endif

		if ( Scene1_Main( &wk->SceneWork1, wk->FrameCounter ) ){
			(*seq)++;
		}
		break;
	case 3:			//終了
		Scene1_End(wk);
		
		return TRUE;
	}
	return FALSE;
}

static void Scene1_Init(OPENING_DEMO_WORK * wk)
{
	OpDemo_VramBankSet();
	OpDemo2DBgSet(wk);
	wk->SceneWork1.bgl = wk->bgl;
	wk->SceneWork1.KiraGeneMngPtr = OPDKira_AllocGeneMng();

	sys_VBlankFuncChange(Scene1_VBlankFunc,(void*)wk);

	wk->SceneWork1.InitOK = 1;	//init通過
}

static void Scene1_Load(SCENE1_WORK *scene_wk)
{
	//--ゲーフリロゴデータロード
	//キャラ(１ファイルになってるので二面に送る。分けてもらったほうがいいかも)
	ArcUtil_BgCharSet(ARC_OP_DEMO, NARC_op_demo_op_demoBG0_logo_NCGR,
						scene_wk->bgl, GAME_FREAK_LOGO_M_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
	ArcUtil_BgCharSet(ARC_OP_DEMO, NARC_op_demo_op_demoBG0_logo_NCGR,
						scene_wk->bgl, GAME_FREAK_LOGO_S_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);	
	//スクリーン
	ArcUtil_ScrnSet(ARC_OP_DEMO, NARC_op_demo_op_demoBG0_logo_m_NSCR,
						scene_wk->bgl, GAME_FREAK_LOGO_M_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
	ArcUtil_ScrnSet(ARC_OP_DEMO, NARC_op_demo_op_demoBG0_logo_s_NSCR,
						scene_wk->bgl, GAME_FREAK_LOGO_S_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
	//パレット
	ArcUtil_PalSet( ARC_OP_DEMO, NARC_op_demo_op_demoBG0_NCLR,
						PALTYPE_MAIN_BG, 0, 0, HEAPID_OP_DEMO );
	ArcUtil_PalSet( ARC_OP_DEMO, NARC_op_demo_op_demoBG0_NCLR,
						PALTYPE_SUB_BG, 0, 0, HEAPID_OP_DEMO );
	
	//--コピーライトデータロード
	//キャラ
	ArcUtil_BgCharSet(ARC_TITLE_PL, NARC_titledemo_cp_light_NCGR,
						scene_wk->bgl, COPY_LIGHT_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
	//スクリーン
	ArcUtil_ScrnSet(ARC_TITLE_PL, NARC_titledemo_cp_light_NSCR,
						scene_wk->bgl, COPY_LIGHT_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
	//パレット
	ArcUtil_PalSet( ARC_TITLE_PL, NARC_titledemo_cp_light_NCLR,
						PALTYPE_MAIN_BG, 0, PAL_ONE_SIZE*16*1, HEAPID_OP_DEMO );	//０番に１６色ロード
	
	//--ベース背景ロード
	//スクリーン
	ArcUtil_ScrnSet(ARC_OP_DEMO, NARC_op_demo_op_demoBG0_logo_back_NSCR,
						scene_wk->bgl, BACK_BASE_M_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
	ArcUtil_ScrnSet(ARC_OP_DEMO, NARC_op_demo_op_demoBG0_logo_back_NSCR,
						scene_wk->bgl, BACK_BASE_S_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);

// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ENGLISH) imatake 2007/02/07
// 北米版では ESRB Notice を下画面に追加

#if PM_LANG == LANG_ENGLISH
	//--コピーライトデータロード
	//キャラ
	ArcUtil_BgCharSet(ARC_OP_DEMO, NARC_op_demo_esrb_notice_NCGR,
						scene_wk->bgl, ESRB_NOTICE_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
	//スクリーン
	ArcUtil_ScrnSet(ARC_OP_DEMO, NARC_op_demo_esrb_notice_NSCR,
						scene_wk->bgl, ESRB_NOTICE_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
	//パレット
	ArcUtil_PalSet( ARC_OP_DEMO, NARC_op_demo_esrb_notice_NCLR,
						PALTYPE_SUB_BG, 0, PAL_ONE_SIZE*16*1, HEAPID_OP_DEMO );	//０番に１６色ロード
#endif

// ----------------------------------------------------------------------------


	//--初期ＢＧ設定
	//BG0・上下画面ゲーフリロゴ非表示
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
	//ベース背景とコピーライトをブレンド
	G2_SetBlendAlpha( BLD_MASK_COPY_LIGHT, BLD_MASK_BACK_BASE_M, 0, 16);
// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ENGLISH) imatake 2007/02/07
// 北米版では ESRB Notice を下画面に追加
#if PM_LANG == LANG_ENGLISH
	G2S_SetBlendAlpha( BLD_MASK_ESRB_NOTICE, BLD_MASK_BACK_BASE_S, 0, 16);
#endif
// ----------------------------------------------------------------------------

	OS_WaitIrq(TRUE, OS_IE_V_BLANK); 	// Ｖブランク待ち
	//マスター輝度を戻す
	WIPE_ResetBrightness( WIPE_DISP_MAIN );
	WIPE_ResetBrightness( WIPE_DISP_SUB );

	GF_Disp_DispOn();
}

static BOOL Scene1_Main(SCENE1_WORK *scene_wk, const int inCounter)
{
	u8 *seq = &(scene_wk->SceneSeq);

	KiraGenerateMain(scene_wk->KiraGeneMngPtr, inCounter);
	
	switch(*seq){
	case 0:	//コピーライトブレンドイン
		scene_wk->Counter = 0;
		scene_wk->Alpha = 0;
		scene_wk->BlendEnd = 0;
		scene_wk->BlendTcb = TCB_Add( TCB_BlendInCopyLight, scene_wk, 0 );
		//ＢＧＭ開始
		Snd_DataSetByScene( SND_SCENE_TITLE, SEQ_TITLE00, 1 );
		(*seq)++;
		break;
	case 1:	//よきところでコピーライトブレンドアウト
		if ( (scene_wk->BlendEnd)&&(inCounter >= FM_COPY_LIGHT_OUT) ){
			scene_wk->Counter = 0;
			scene_wk->Alpha = 16;
			scene_wk->BlendEnd = 0;
			scene_wk->BlendTcb = TCB_Add( TCB_BlendOutCopyLight, scene_wk, 0 );
			(*seq)++;
		}
		break;
	case 2:	//ゲーフリロゴブレンドイン
		if ( (scene_wk->BlendEnd)&&(inCounter>=FM_FG_LOGO_IN) ){
			//コピーライト面非表示
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ENGLISH) imatake 2007/02/10
// ESRB Notice を消去
#if PM_LANG == LANG_ENGLISH
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
#endif
// ----------------------------------------------------------------------------
			//ブレンド初期設定
			G2_SetBlendAlpha( BLD_MASK_GF_LOGO_M, BLD_MASK_BACK_BASE_M, 0, 16);
			G2S_SetBlendAlpha( BLD_MASK_GF_LOGO_S, BLD_MASK_BACK_BASE_S, 0, 16);
			//ゲーフリロゴ面表示
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
			
			scene_wk->Counter = 0;
			scene_wk->Alpha = 0;
			scene_wk->BlendEnd = 0;
			scene_wk->BlendTcb = TCB_Add( TCB_BlendInGFLogoM, scene_wk, 0 );
			(*seq)++;
		}
		break;
	case 3:	//下場面ブレンドイン
		if ( (scene_wk->BlendEnd) ){
			scene_wk->Counter = 0;
			scene_wk->Alpha = 0;
			scene_wk->BlendEnd = 0;
			scene_wk->BlendTcb = TCB_Add( TCB_BlendInGFLogoS, scene_wk, 0 );
			(*seq)++;
		}
		break;
	case 4:	//下画面上画面の順で全部ブラックアウト
		if ( (scene_wk->BlendEnd)&&(inCounter>=FM_GF_LOGO_OUT) ){
			//ワイプスタート	ホワイトイン
			WIPE_SYS_Start(	WIPE_PATTERN_FSAM, WIPE_TYPE_FADEOUT,
							WIPE_TYPE_FADEOUT,0x0000,OPDEMO_BRIGHTNESS_SYNC,1,HEAPID_OP_DEMO );
			(*seq)++;
		}
		break;
	case 5:
		if ( WIPE_SYS_EndCheck() ){
			return TRUE;
		}
	}
	return FALSE;
}

static void Scene1_End(OPENING_DEMO_WORK * wk)
{
	sys_VBlankFuncChange( NULL, NULL );	// VBlankセット
	
	if (wk->SceneWork1.InitOK){
		//ジェネレーター解放
		OPDKira_FreeGeneMng(wk->SceneWork1.KiraGeneMngPtr);
		//ＢＧＬ解放
		GF_BGL_BGControlExit( wk->bgl, COPY_LIGHT_FRAME );
		GF_BGL_BGControlExit( wk->bgl, GAME_FREAK_LOGO_M_FRAME );
		GF_BGL_BGControlExit( wk->bgl, GAME_FREAK_LOGO_S_FRAME );
		GF_BGL_BGControlExit( wk->bgl, BACK_BASE_M_FRAME );
		GF_BGL_BGControlExit( wk->bgl, BACK_BASE_S_FRAME );
// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ENGLISH) imatake 2007/02/07
// 北米版では ESRB Notice を下画面に追加
#if PM_LANG == LANG_ENGLISH
		GF_BGL_BGControlExit( wk->bgl, ESRB_NOTICE_FRAME );
#endif
// ----------------------------------------------------------------------------
		sys_FreeMemoryEz( wk->bgl );
		wk->SceneWork1.InitOK = 0;
	}

	//タスクが動いているならば終了
	if (wk->SceneWork1.BlendTcb != NULL){
		GF_ASSERT(wk->SkipFlg);
		OS_Printf("スキップにより、TCBを外から解放しました\n");
		TCB_Delete(wk->SceneWork1.BlendTcb);
		wk->SceneWork1.BlendTcb = NULL;
	}
}

//コピーライドブレンドインＴＣＢ
static void TCB_BlendInCopyLight( TCB_PTR tcb, void* work )
{
	SCENE1_WORK *s_work = work;

	s_work->Counter++;
	if (s_work->Counter >= COPY_LIGHT_BLD_IN_COUNT_MAX){
		s_work->Counter = 0;
		s_work->Alpha++;
	}

	if (s_work->Alpha >= 16){
		TCB_Delete(tcb);
		s_work->BlendTcb = NULL;
		s_work->BlendEnd = 1;
	}

	G2_SetBlendAlpha( BLD_MASK_COPY_LIGHT, BLD_MASK_BACK_BASE_M, s_work->Alpha, 16);
// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ENGLISH) imatake 2007/02/07
// 北米版では ESRB Notice を下画面に追加
#if PM_LANG == LANG_ENGLISH
	G2S_SetBlendAlpha( BLD_MASK_ESRB_NOTICE, BLD_MASK_BACK_BASE_S, s_work->Alpha, 16);
#endif
// ----------------------------------------------------------------------------
}

//コピーライドブレンドアウトＴＣＢ
static void TCB_BlendOutCopyLight( TCB_PTR tcb, void* work )
{
	SCENE1_WORK *s_work = work;

	s_work->Counter++;
	if (s_work->Counter >= COPY_LIGHT_BLD_COUNT_MAX){
		s_work->Counter = 0;
		s_work->Alpha--;
	}

	if (s_work->Alpha == 0){
		TCB_Delete(tcb);
		s_work->BlendTcb = NULL;
		s_work->BlendEnd = 1;
		*(s_work->SkipOKPtr) = 1;	//スキップ許可
	}

	G2_SetBlendAlpha( BLD_MASK_COPY_LIGHT, BLD_MASK_BACK_BASE_M, s_work->Alpha, 16);
// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ENGLISH) imatake 2007/02/07
// 北米版では ESRB Notice を下画面に追加
#if PM_LANG == LANG_ENGLISH
	G2S_SetBlendAlpha( BLD_MASK_ESRB_NOTICE, BLD_MASK_BACK_BASE_S, s_work->Alpha, 16);
#endif
// ----------------------------------------------------------------------------
}

//メイン画面ＧＦロゴブレンドインＴＣＢ
static void TCB_BlendInGFLogoM( TCB_PTR tcb, void* work )
{
	SCENE1_WORK *s_work = work;

	s_work->Counter++;
	if (s_work->Counter >= COPY_LIGHT_BLD_COUNT_MAX){
		s_work->Counter = 0;
		s_work->Alpha++;
	}

	if (s_work->Alpha >= 16){
		TCB_Delete(tcb);
		s_work->BlendTcb = NULL;
		s_work->BlendEnd = 1;
	}

	G2_SetBlendAlpha( BLD_MASK_GF_LOGO_M, BLD_MASK_BACK_BASE_M, s_work->Alpha, 16);
}

//サブ画面ＧＦロゴブレンドインＴＣＢ
static void TCB_BlendInGFLogoS( TCB_PTR tcb, void* work )
{
	SCENE1_WORK *s_work = work;

	s_work->Counter++;
	if (s_work->Counter >= COPY_LIGHT_BLD_COUNT_MAX){
		s_work->Counter = 0;
		s_work->Alpha++;
	}

	if (s_work->Alpha >= 16){
		TCB_Delete(tcb);
		s_work->BlendTcb = NULL;
		s_work->BlendEnd = 1;
	}

	G2S_SetBlendAlpha( BLD_MASK_GF_LOGO_S, BLD_MASK_BACK_BASE_S, s_work->Alpha, 16);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//

static void Scene2_Init(OPENING_DEMO_WORK * wk);
static void Scene2_Load(SCENE2_WORK *scene_wk);
static void Scene2_HeroBGTrans(OPENING_DEMO_WORK *wk, SCENE2_WORK *scene_wk);
static void Scene2_PokeBG_MainSub_DispSetting(SCENE2_WORK *scene_wk);
static void Scene2_PokeBGTrans(OPENING_DEMO_WORK *wk, SCENE2_WORK *scene_wk);
static void Scene2_Load3D(SCENE2_WORK *scene_wk, int map_resource_no);
static BOOL Scene2_Load3D_Div(SCENE2_WORK *scene_wk, int map_resource_no);
static void Scene2_Disp3D(SCENE2_WORK *scene_wk);
static BOOL Scene2_Main(OPENING_DEMO_WORK *wk, SCENE2_WORK *scene_wk, const int inCounter);
static void Scene2_ScrollTown(SCENE2_WORK *scene_wk);
static void Scene2_ScrollSky(SCENE2_WORK *scene_wk);
static void Scene2_ScrollBattle(SCENE2_WORK *scene_wk);
static void Scene2_ScrollBallThrowStream(SCENE2_WORK *scene_wk);
static void Scene2_ScrollHand(SCENE2_WORK *scene_wk, int inCounter);
static void Scene2_PokeAppearSlideIn(SCENE2_WORK *scene_wk, int inCounter);
static void Scene2_ScrollPokeAttack(SCENE2_WORK *scene_wk);
static void Scene2_CameraMoveMain(SCENE2_WORK *scene_wk, const int inCounter);
static void Scene2_MapChange(OPENING_DEMO_WORK *wk, SCENE2_WORK *scene_wk, int inCounter);
static void Scene2_End(OPENING_DEMO_WORK * wk);

//タイトル表示～3Ｄ～バトルのシーン
static BOOL Title_3D_BattleScene(OPENING_DEMO_WORK * wk)
{
	u8 *seq = &(wk->SceneWork2.Seq);

	if (wk->SkipFlg){
		(*seq) = 4;		//スキップ実行
	}

	switch(*seq){
	case 0:		//初期化
		Scene2_Init(wk);
		(*seq)++;
		break;
	case 1:			//グラフィックロード
		Scene2_Load(&wk->SceneWork2);
		(*seq)++;
		break;
	case 2:
		if(Scene2_Load3D_Div(&wk->SceneWork2, 0) == TRUE){
			wk->SceneWork2.Disp3DFlg = 1;	//空撮フラグオン
			(*seq)++;
		}
		break;
	case 3:			//メイン
		if ( Scene2_Main(wk, &wk->SceneWork2, wk->FrameCounter ) ){
			(*seq)++;
		}
		OPD_Obj_DrawActor( wk->SceneWork2.ObjWorkPtr );
		if (wk->SceneWork2.Disp3DFlg){
			if(wk->SceneWork2.Load3D_DivMode == 0){
				Scene2_Disp3D(&wk->SceneWork2);
			}
		}else{
			OPD_Poke_Main( wk->SceneWork2.OpSsmPtr );
		}
		break;
	case 4:			//終了
		Scene2_End(wk);
		return TRUE;
	}
	return FALSE;
}

static void Scene2_Init(OPENING_DEMO_WORK * wk)
{
	u8 i;
	int obj_work_size;
	int ssm_size;
	OpDemo_Scene2_VramBankSet();
	OpDemo_Scene2_2DBgSet(wk);
	OpDemo3D_Init(wk);

	obj_work_size = OPD_Obj_GetWorkSize();
	ssm_size = OPD_Poke_GetSSMSize();
	
	wk->SceneWork2.ObjWorkPtr = sys_AllocMemory(HEAPID_OP_DEMO, obj_work_size);
	wk->SceneWork2.KiraListPtr = OPDKira_CreateList(SCENE2_KIRA_MAX);
	wk->SceneWork2.OpSsmPtr = sys_AllocMemory(HEAPID_OP_DEMO, ssm_size);
	memset(wk->SceneWork2.ObjWorkPtr, 0, obj_work_size);
	memset(wk->SceneWork2.OpSsmPtr, 0, ssm_size);

	wk->SceneWork2.KiraMarginCore = SCENE2_KIRA_MARGIN1;
	
	OPD_Poke_Init(wk->SceneWork2.OpSsmPtr);

	wk->SceneWork2.bgl = wk->bgl;

	//３Ｄ描画スイッチ	上をメインにする
	sys.disp3DSW = DISP_3D_TO_MAIN;
	GF_Disp_DispSelect();

	for(i=0;i<4;i++){
		//ライトセット
		NNS_G3dGlbLightVector(i,
							LightVec[i].x,
							LightVec[i].y,
							LightVec[i].z);
		NNS_G3dGlbLightColor(i, LightColor[i]);
	}

	//グローバルステート書き換え
	NNS_G3dGlbMaterialColorDiffAmb(
				GX_RGB(16,16,16),
				GX_RGB(10,12,12),
				FALSE);
	
	NNS_G3dGlbMaterialColorSpecEmi(
				GX_RGB(18,18,18),
				GX_RGB(14,14,14),
				FALSE);
	
	// ポリゴンアトリビュート
	NNS_G3dGlbPolygonAttr(
				13,		//<ライト1以外有効（1101）
				GX_POLYGONMODE_MODULATE,
				GX_CULL_BACK,
				0,
				31,
				GX_POLYGON_ATTR_MISC_FOG);
	
	sys_VBlankFuncChange(Scene2_VBlankFunc,(void*)wk);

	//タイトルロゴの輝度を上げておく
	SetBrightness( BRIGHTNESS_BLACK, GX_BLEND_PLANEMASK_BG3, MASK_MAIN_DISPLAY );
	
	wk->SceneWork2.InitOK = 1;		//init通過
}

static void Scene2_Load3D_Exit(OPENING_DEMO_WORK *wk)
{
	int i;
	
	if (wk->SceneWork2.Load3DOK || wk->SceneWork2.Load3D_DivMode){
		//カメラ削除
		GFC_PurgeCamera();
		GFC_FreeCamera(wk->SceneWork2.Data.CameraPtr);
		//モデリングデータ削除
		for (i=0;i<wk->SceneWork2.Data.block_num;i++){
			sys_FreeMemoryEz(wk->SceneWork2.Data.Block[i].ResFile);
		}
		wk->SceneWork2.Data.block_num = 0;
		//テクスチャ削除
		sys_FreeMemoryEz(wk->SceneWork2.Data.TexFile);
		wk->SceneWork2.Load3DOK = 0;
		wk->SceneWork2.Load3D_DivMode = 0;
	}
}

static void Scene2_Load2D_BG_Exit(OPENING_DEMO_WORK *wk, BOOL bgl_free)
{
	if (wk->SceneWork2.InitOK && wk->bgl != NULL){
		if(wk->SceneWork2.bg_trans_status == SCENE2_BG_TRANS_STATUS_NORMAL){
			GF_BGL_BGControlExit( wk->bgl, MAIN_BG3_FRAME );
			GF_BGL_BGControlExit( wk->bgl, TOWN_FRAME );
			GF_BGL_BGControlExit( wk->bgl, SKY_FRAME );
			GF_BGL_BGControlExit( wk->bgl, WB_TITLE_FRAME );
			GF_BGL_BGControlExit( wk->bgl, SUB_BG1_FRAME );
			GF_BGL_BGControlExit( wk->bgl, SUB_BG2_FRAME );
			GF_BGL_BGControlExit( wk->bgl, SUB_BG3_FRAME );
		}
		else if(wk->SceneWork2.bg_trans_status == SCENE2_BG_TRANS_STATUS_POKE){
			GF_BGL_BGControlExit( wk->bgl, MAIN_BG3_FRAME );
			GF_BGL_BGControlExit( wk->bgl, TOWN_FRAME );
			GF_BGL_BGControlExit( wk->bgl, SKY_FRAME );
		//	GF_BGL_BGControlExit( wk->bgl, WB_TITLE_FRAME );
			GF_BGL_BGControlExit( wk->bgl, SUB_BG1_FRAME );
			GF_BGL_BGControlExit( wk->bgl, SUB_BG2_FRAME );
			GF_BGL_BGControlExit( wk->bgl, SUB_BG3_FRAME );
		}
		else{
			GF_BGL_BGControlExit( wk->bgl, MAIN_BG0_FRAME );
			GF_BGL_BGControlExit( wk->bgl, MAIN_BG3_FRAME );
			GF_BGL_BGControlExit( wk->bgl, TOWN_FRAME );
			GF_BGL_BGControlExit( wk->bgl, SKY_FRAME );
			GF_BGL_BGControlExit( wk->bgl, WB_TITLE_FRAME );
			GF_BGL_BGControlExit( wk->bgl, SUB_BG1_FRAME );
			GF_BGL_BGControlExit( wk->bgl, SUB_BG2_FRAME );
			GF_BGL_BGControlExit( wk->bgl, SUB_BG3_FRAME );
		}
		
		if(bgl_free == TRUE){
			sys_FreeMemoryEz( wk->bgl );
			wk->bgl = NULL;
		}
	}
}

static void Scene2_End(OPENING_DEMO_WORK * wk)
{	
	u8 i;

	//ウィンドウオフ
	GX_SetVisibleWnd( GX_WNDMASK_NONE );

	Scene2_Load3D_Exit(wk);

	if (wk->SceneWork2.LoadOK){
		//アクター関連解放
		OPD_Obj_EndCellActor( wk->SceneWork2.ObjWorkPtr );
		wk->SceneWork2.LoadOK = 0;
	}

	if (wk->SceneWork2.InitOK){
		//キラエフェクト解放
		if (wk->SceneWork2.KiraListPtr != NULL){
			OPDKira_FreeList(wk->SceneWork2.KiraListPtr);
			wk->SceneWork2.KiraListPtr = NULL;
		}
		if (wk->SceneWork2.ObjWorkPtr != NULL){
			sys_FreeMemoryEz(wk->SceneWork2.ObjWorkPtr);
			wk->SceneWork2.ObjWorkPtr = NULL;
		}
		if (wk->SceneWork2.OpSsmPtr != NULL){
			//スプライト・パーティクル解放
			OPD_Poke_End(wk->SceneWork2.OpSsmPtr);
			sys_FreeMemoryEz(wk->SceneWork2.OpSsmPtr);
			wk->SceneWork2.OpSsmPtr = NULL;
		}
	
		Scene2_Load2D_BG_Exit(wk, TRUE);
		
		//
		if(wk->SceneWork2.pokebg_palbuf != NULL){
			sys_FreeMemoryEz(wk->SceneWork2.pokebg_palbuf);
			wk->SceneWork2.pokebg_palbuf = NULL;
		}
		
		//3D解放
		GF_G3D_Exit( wk->g3Dman );
		wk->SceneWork2.InitOK = 0;
	}

	//FOG OFF
	NNS_G3dGeFlushBuffer();
	G3X_SetFog( FALSE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x8000, 0 );

	//ウィンドウOFF
	GX_SetVisibleWnd( 0 );
	GXS_SetVisibleWnd( 0 );

	sys_VBlankFuncChange( NULL, NULL );	// VBlankセット	
}

static void Scene2_Load(SCENE2_WORK *scene_wk)
{
	u8 i;
	ARCHANDLE* hdl;
	
	hdl  = ArchiveDataHandleOpen(ARC_OP_DEMO, HEAPID_OP_DEMO); 

	//-- メイン画面(下) --//
	{
	#if 0	//木漏れ日に変更
		ArcUtil_HDL_PalSet(hdl, NARC_op_demo_op_heroinebg_NCLR, 
			PALTYPE_MAIN_BG, 0, 0, HEAPID_OP_DEMO);

		//BG1	手のアップ
		ArcUtil_HDL_BgCharSet(hdl, NARC_op_demo_op_heroine1_NCGR,
			scene_wk->bgl, TOWN_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
		ArcUtil_HDL_ScrnSet(hdl, NARC_op_demo_op_heroine2a_NSCR,
			scene_wk->bgl, TOWN_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
		//BG2	主人公ボール投げ
		ArcUtil_HDL_BgCharSet(hdl, NARC_op_demo_op_heroine1_NCGR,
			scene_wk->bgl, SKY_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
		ArcUtil_HDL_ScrnSet(hdl, NARC_op_demo_op_heroine1a_NSCR,
			scene_wk->bgl, SKY_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
		//BG3	背景
		ArcUtil_HDL_BgCharSet(hdl, NARC_op_demo_op_heroinebg_NCGR,
			scene_wk->bgl, MAIN_BG3_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
		ArcUtil_HDL_ScrnSet(hdl, NARC_op_demo_op_heroine1b_NSCR,
			scene_wk->bgl, MAIN_BG3_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
	#else
		//BG3	GFロゴ(スカスカのやつ)
		ArcUtil_HDL_PalSetEx(hdl, NARC_op_demo_g_logo0_NCLR,
				PALTYPE_MAIN_BG,0x20*0xc, 0x20*0xc, 0x20*2, HEAPID_OP_DEMO);
		ArcUtil_HDL_BgCharSet(hdl, NARC_op_demo_g_logo0_NCGR,
							scene_wk->bgl, MAIN_BG3_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
		ArcUtil_HDL_ScrnSet(hdl, NARC_op_demo_g_logo_NSCR,
							scene_wk->bgl, MAIN_BG3_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);

		//BG2	GFロゴ(色が入っている方)
		ArcUtil_HDL_PalSetEx(hdl, NARC_op_demo_g_logo2_NCLR,
				PALTYPE_MAIN_BG,0x20*0xe, 0x20*0xe, 0x20*2, HEAPID_OP_DEMO);
		ArcUtil_HDL_BgCharSet(hdl, NARC_op_demo_g_logo2_NCGR,
							scene_wk->bgl, SKY_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
		ArcUtil_HDL_ScrnSet(hdl, NARC_op_demo_g_logo_NSCR,
							scene_wk->bgl, SKY_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);

		//BG1	木漏れ日
		{
			ARCHANDLE* hdl;
			
			hdl  = ArchiveDataHandleOpen(ARC_WEATHER_SYS_GRA, HEAPID_OP_DEMO); 
			
			ArcUtil_HDL_BgCharSet(hdl, NARC_WEATHER_SYS_KOGOREBI_NCGR,
				scene_wk->bgl, TOWN_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
			ArcUtil_HDL_ScrnSet(hdl, NARC_WEATHER_SYS_KOGOREBI_NSCR,
				scene_wk->bgl, TOWN_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
			//スクリーンを512x512にしているので隣にもスクリーンを転送する
			ArcUtil_HDL_ScrnSet(hdl, NARC_WEATHER_SYS_KOGOREBI_NSCR,
				scene_wk->bgl, TOWN_FRAME, 32*32, 0, FALSE, HEAPID_OP_DEMO);
			ArcUtil_HDL_PalSet(hdl, NARC_WEATHER_SYS_KOGOREBI_NCLR, 
				PALTYPE_MAIN_BG, 0x20 * 0, 0x20, HEAPID_OP_DEMO);
			
			ArchiveDataHandleClose( hdl );
		}
	#endif
	}

	//-- サブ画面(上) --//
	{
		ArcUtil_HDL_PalSet(hdl, NARC_op_demo_op_herobg_NCLR, 
			PALTYPE_SUB_BG, 0, 0, HEAPID_OP_DEMO);

		//BG1	手のアップ
		ArcUtil_HDL_BgCharSet(hdl, NARC_op_demo_op_hero2_NCGR,
			scene_wk->bgl, SUB_BG1_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
		ArcUtil_HDL_ScrnSet(hdl, NARC_op_demo_op_hero2a_NSCR,
			scene_wk->bgl, SUB_BG1_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
		//BG2	主人公ボール投げ
		ArcUtil_HDL_BgCharSet(hdl, NARC_op_demo_op_hero1_NCGR,
			scene_wk->bgl, SUB_BG2_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
		ArcUtil_HDL_ScrnSet(hdl, NARC_op_demo_op_hero1a_NSCR,
			scene_wk->bgl, SUB_BG2_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
		//BG3	背景
		ArcUtil_HDL_BgCharSet(hdl, NARC_op_demo_op_herobg_NCGR,
			scene_wk->bgl, SUB_BG3_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
		ArcUtil_HDL_ScrnSet(hdl, NARC_op_demo_op_hero1b_NSCR,
			scene_wk->bgl, SUB_BG3_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
	}

	//--サブ画面(上) 256タイトルデータロード
	{
		ArcUtil_HDL_BgCharSet(hdl, NARC_op_demo_g_logo2_NCGR,
							scene_wk->bgl, WB_TITLE_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
		ArcUtil_HDL_ScrnSet(hdl, NARC_op_demo_g_logo_NSCR,
							scene_wk->bgl, WB_TITLE_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
		ArcUtil_HDL_PalSetEx(hdl, NARC_op_demo_g_logo2_NCLR,
					PALTYPE_SUB_BG,0x20*0xe, 0x20*0xe, 0x20*2, HEAPID_OP_DEMO);
	}

	//--アクターセット
	OPD_Obj_InitCellActor( scene_wk->ObjWorkPtr );
	OPD_Obj_SetActor( scene_wk->ObjWorkPtr );
	
	//--キラキラアクターセット
	OPD_Obj_SetKiraActor( scene_wk->ObjWorkPtr,
						  scene_wk->KiraListPtr );

	//バッググラウンド色を黒にする
	MI_CpuClear16((void*)HW_BG_PLTT, 2);
	MI_CpuClear16((void*)HW_DB_BG_PLTT, 2);

	//--初期ＢＧ設定
	//全部非表示
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3, VISIBLE_OFF );
	
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG0|GX_PLANEMASK_BG1|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3, VISIBLE_OFF );

	WIPE_ResetBrightness( WIPE_DISP_MAIN );
	WIPE_ResetBrightness( WIPE_DISP_SUB );


	ArchiveDataHandleClose( hdl );
	scene_wk->LoadOK = 1;		//load通過
}

//--------------------------------------------------------------
/**
 * @brief   シーン2:2D背景と主人公ボール投げグラフィックを転送
 * 			(技を出すごとにメイン、サブが切り替わるので、共に同じものを転送)
 *
 * @param   scene_wk		
 */
//--------------------------------------------------------------
static void Scene2_HeroBGTrans(OPENING_DEMO_WORK *wk, SCENE2_WORK *scene_wk)
{
	u8 i;
	ARCHANDLE* hdl;

	//HeroBGではBG構成を変える必要はないのでグラフィックデータの展開だけ行う
//	OpDemo_Scene2_2DBgSet_PokeBG(wk);	//ポケモンBG用の新しい構成を設定

	//GFロゴ用に設定した256色モードを16色に変更
	GF_BGL_BGControlReset(scene_wk->bgl, SKY_FRAME, BGL_RESET_COLOR, GX_BG_COLORMODE_16);
	GF_BGL_BGControlReset(scene_wk->bgl, MAIN_BG3_FRAME, BGL_RESET_COLOR, GX_BG_COLORMODE_16);
	
    //木漏れ日用に変更していたプライオリティを戻す
    G2_SetBG0Priority(0);
	GF_BGL_PrioritySet( TOWN_FRAME, 3 );
	
	hdl  = ArchiveDataHandleOpen(ARC_OP_DEMO, HEAPID_OP_DEMO); 

	ArcUtil_HDL_PalSet(hdl, NARC_op_demo_op_heroinebg_NCLR, 
		PALTYPE_MAIN_BG, 0, 0, HEAPID_OP_DEMO);

	//BG1	手のアップ
	ArcUtil_HDL_BgCharSet(hdl, NARC_op_demo_op_heroine2_NCGR,
		scene_wk->bgl, TOWN_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
	ArcUtil_HDL_ScrnSet(hdl, NARC_op_demo_op_heroine2a_NSCR,
		scene_wk->bgl, TOWN_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
	//BG2	主人公ボール投げ
	ArcUtil_HDL_BgCharSet(hdl, NARC_op_demo_op_heroine1_NCGR,
		scene_wk->bgl, SKY_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
	ArcUtil_HDL_ScrnSet(hdl, NARC_op_demo_op_heroine1a_NSCR,
		scene_wk->bgl, SKY_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
	//BG3	背景
	ArcUtil_HDL_BgCharSet(hdl, NARC_op_demo_op_heroinebg_NCGR,
		scene_wk->bgl, MAIN_BG3_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
	ArcUtil_HDL_ScrnSet(hdl, NARC_op_demo_op_heroine1b_NSCR,
		scene_wk->bgl, MAIN_BG3_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);

	//バッググラウンド色を黒にする
	MI_CpuClear16((void*)HW_BG_PLTT, 2);
	MI_CpuClear16((void*)HW_DB_BG_PLTT, 2);

	//初期座標設定
	GF_BGL_ScrollSet( scene_wk->bgl, TOWN_FRAME, 0, 0);
	GF_BGL_ScrollSet( scene_wk->bgl, SKY_FRAME, 0, 0);
	GF_BGL_ScrollSet( scene_wk->bgl, MAIN_BG3_FRAME, 0, 0);
	GF_BGL_ScrollSet( scene_wk->bgl, SUB_BG1_FRAME, 0, 0);
	GF_BGL_ScrollSet( scene_wk->bgl, SUB_BG2_FRAME, 0, 0);
	GF_BGL_ScrollSet( scene_wk->bgl, SUB_BG3_FRAME, 0, 0);

	ArchiveDataHandleClose( hdl );
}

//--------------------------------------------------------------
/**
 * @brief   シーン2:ポケモンBGグラフィックを転送
 * 			(技を出すごとにメイン、サブが切り替わるので、共に同じものを転送)
 *
 * @param   scene_wk		
 */
//--------------------------------------------------------------
static void Scene2_PokeBGTrans(OPENING_DEMO_WORK *wk, SCENE2_WORK *scene_wk)
{
	u8 i;
	ARCHANDLE* hdl;

	OpDemo_Scene2_2DBgSet_PokeBG(wk);	//ポケモンBG用の新しい構成を設定
	
	hdl  = ArchiveDataHandleOpen(ARC_OP_DEMO, HEAPID_OP_DEMO); 

	//パレット
	ArcUtil_HDL_PalSet(hdl, NARC_op_demo_op_poke_NCLR, PALTYPE_MAIN_BG, 0, 0, HEAPID_OP_DEMO);
	ArcUtil_HDL_PalSet(hdl, NARC_op_demo_op_poke_NCLR, PALTYPE_SUB_BG, 0, 0, HEAPID_OP_DEMO);
	scene_wk->pokebg_palbuf = sys_AllocMemory(HEAPID_OP_DEMO, 0x200);
	MI_CpuCopy16((void*)HW_BG_PLTT, scene_wk->pokebg_palbuf, 0x200);
	MI_CpuClear16(scene_wk->pokebg_palbuf, 2);	//バッググラウンドを黒にする

	//BG1 大
	ArcUtil_HDL_BgCharSet(hdl, NARC_op_demo_op_poke_l_NCGR,
		scene_wk->bgl, TOWN_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
	ArcUtil_HDL_ScrnSet(hdl, NARC_op_demo_op_poke_l_NSCR,
		scene_wk->bgl, TOWN_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
	ArcUtil_HDL_BgCharSet(hdl, NARC_op_demo_op_poke_l_NCGR,
		scene_wk->bgl, SUB_BG1_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
	ArcUtil_HDL_ScrnSet(hdl, NARC_op_demo_op_poke_l_NSCR,
		scene_wk->bgl, SUB_BG1_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
	
	//BG2　中
	ArcUtil_HDL_BgCharSet(hdl, NARC_op_demo_op_poke_m_NCGR,
		scene_wk->bgl, SKY_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
	ArcUtil_HDL_ScrnSet(hdl, NARC_op_demo_op_poke_m_NSCR,
		scene_wk->bgl, SKY_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
	ArcUtil_HDL_BgCharSet(hdl, NARC_op_demo_op_poke_m_NCGR,
		scene_wk->bgl, SUB_BG2_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
	ArcUtil_HDL_ScrnSet(hdl, NARC_op_demo_op_poke_m_NSCR,
		scene_wk->bgl, SUB_BG2_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
	
	//BG3　小
	ArcUtil_HDL_BgCharSet(hdl, NARC_op_demo_op_poke_s_NCGR,
		scene_wk->bgl, MAIN_BG3_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
	ArcUtil_HDL_ScrnSet(hdl, NARC_op_demo_op_poke_s_NSCR,
		scene_wk->bgl, MAIN_BG3_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
	ArcUtil_HDL_BgCharSet(hdl, NARC_op_demo_op_poke_s_NCGR,
		scene_wk->bgl, SUB_BG3_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
	ArcUtil_HDL_ScrnSet(hdl, NARC_op_demo_op_poke_s_NSCR,
		scene_wk->bgl, SUB_BG3_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);

	//バッググラウンド色を黒にする
	MI_CpuClear16((void*)HW_BG_PLTT, 2);
	MI_CpuClear16((void*)HW_DB_BG_PLTT, 2);

	//初期座標設定
	GF_BGL_ScrollSet( scene_wk->bgl, TOWN_FRAME, 
		GF_BGL_SCROLL_X_SET, -128);
	GF_BGL_ScrollSet( scene_wk->bgl, SKY_FRAME, 
		GF_BGL_SCROLL_X_SET, -200);
	GF_BGL_ScrollSet( scene_wk->bgl, MAIN_BG3_FRAME, 
		GF_BGL_SCROLL_X_SET, -152);
	GF_BGL_ScrollSet( scene_wk->bgl, SUB_BG1_FRAME, 
		GF_BGL_SCROLL_X_SET, 128);
	GF_BGL_ScrollSet( scene_wk->bgl, SUB_BG2_FRAME, 
		GF_BGL_SCROLL_X_SET, 0x18*8);
	GF_BGL_ScrollSet( scene_wk->bgl, SUB_BG3_FRAME, 
		GF_BGL_SCROLL_X_SET, 0x16*8);

	ArchiveDataHandleClose( hdl );
}

//--------------------------------------------------------------
/**
 * @brief   シーン2:雷グラフィックを転送
 *
 * @param   scene_wk		
 */
//--------------------------------------------------------------
static void Scene2_ThunderBGTrans(OPENING_DEMO_WORK *wk, SCENE2_WORK *scene_wk)
{
	u8 i;
	ARCHANDLE* hdl;

	OpDemo_Scene2_2DBgSet_Thunder(wk);	//雷BG用の新しい構成を設定
	
	hdl  = ArchiveDataHandleOpen(ARC_OP_DEMO, HEAPID_OP_DEMO); 

	ArcUtil_HDL_BgCharSet(hdl, NARC_op_demo_op_thunder1_NCGR,
		scene_wk->bgl, MAIN_BG0_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
	ArcUtil_HDL_ScrnSet(hdl, NARC_op_demo_op_thunder1_NSCR,
		scene_wk->bgl, MAIN_BG0_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
	ArcUtil_HDL_BgCharSet(hdl, NARC_op_demo_op_thunder2_NCGR,
		scene_wk->bgl, WB_TITLE_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
	ArcUtil_HDL_ScrnSet(hdl, NARC_op_demo_op_thunder2_NSCR,
		scene_wk->bgl, WB_TITLE_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);

	ArchiveDataHandleClose( hdl );
}

//--------------------------------------------------------------
/**
 * @brief   ポケモンBGをメイン画面がどちらになっているかによって、BGプライオリティとY座標を設定する
 *
 * @param   scene_wk		
 */
//--------------------------------------------------------------
static void Scene2_PokeBG_MainSub_DispSetting(SCENE2_WORK *scene_wk)
{
	if(sys.disp3DSW == DISP_3D_TO_MAIN){	//上画面がメイン
		GF_BGL_ScrollSet( scene_wk->bgl, TOWN_FRAME, GF_BGL_SCROLL_Y_SET, 0 );
		GF_BGL_ScrollSet( scene_wk->bgl, SKY_FRAME, GF_BGL_SCROLL_Y_SET, 0 );
		GF_BGL_ScrollSet( scene_wk->bgl, MAIN_BG3_FRAME, GF_BGL_SCROLL_Y_SET, 0 );
		GF_BGL_ScrollSet( scene_wk->bgl, SUB_BG1_FRAME, GF_BGL_SCROLL_Y_SET, 256 );
		GF_BGL_ScrollSet( scene_wk->bgl, SUB_BG2_FRAME, GF_BGL_SCROLL_Y_SET, 256 );
		GF_BGL_ScrollSet( scene_wk->bgl, SUB_BG3_FRAME, GF_BGL_SCROLL_Y_SET, 256 );
		
		GF_BGL_PrioritySet( TOWN_FRAME, 1 );
		GF_BGL_PrioritySet( SKY_FRAME, 2 );
		GF_BGL_PrioritySet( MAIN_BG3_FRAME, 0 );
		GF_BGL_PrioritySet( SUB_BG1_FRAME, 1 );
		GF_BGL_PrioritySet( SUB_BG2_FRAME, 2 );
		GF_BGL_PrioritySet( SUB_BG3_FRAME, 0 );
	}
	else{
		GF_BGL_ScrollSet( scene_wk->bgl, TOWN_FRAME, GF_BGL_SCROLL_Y_SET, 256 );
		GF_BGL_ScrollSet( scene_wk->bgl, SKY_FRAME, GF_BGL_SCROLL_Y_SET, 256 );
		GF_BGL_ScrollSet( scene_wk->bgl, MAIN_BG3_FRAME, GF_BGL_SCROLL_Y_SET, 256 );
		GF_BGL_ScrollSet( scene_wk->bgl, SUB_BG1_FRAME, GF_BGL_SCROLL_Y_SET, 0 );
		GF_BGL_ScrollSet( scene_wk->bgl, SUB_BG2_FRAME, GF_BGL_SCROLL_Y_SET, 0 );
		GF_BGL_ScrollSet( scene_wk->bgl, SUB_BG3_FRAME, GF_BGL_SCROLL_Y_SET, 0 );

		GF_BGL_PrioritySet( TOWN_FRAME, 1 );
		GF_BGL_PrioritySet( SKY_FRAME, 2 );
		GF_BGL_PrioritySet( MAIN_BG3_FRAME, 0 );
		GF_BGL_PrioritySet( SUB_BG1_FRAME, 1 );
		GF_BGL_PrioritySet( SUB_BG2_FRAME, 2 );
		GF_BGL_PrioritySet( SUB_BG3_FRAME, 0 );
	}

	OPD_Obj_StreamDispReset(scene_wk->ObjWorkPtr, FALSE);
}

static void Scene2_Load3D_Tex(SCENE2_WORK *scene_wk, int map_resource_no)
{
	//テクスチャロード
	scene_wk->Data.TexFile = ArchiveDataLoadMalloc(	ARC_OP_DEMO,
													TexsetArcTbl[map_resource_no],
													HEAPID_OP_DEMO );

	scene_wk->Data.Texture = NNS_G3dGetTex((NNSG3dResFileHeader*) scene_wk->Data.TexFile );	// テクスチャリソース取得

	{
		BOOL rc;
		rc = LoadVRAMTexture(scene_wk->Data.Texture);
		GF_ASSERT(rc && "オープニング：テクスチャのVRAM確保失敗");
	}

	//実体を破棄
	{
		u8* texImgStartAddr;
		u32 newSize;

		GF_ASSERT(scene_wk->Data.Texture->texInfo.ofsTex != 0);
		texImgStartAddr = (u8*)scene_wk->Data.Texture + scene_wk->Data.Texture->texInfo.ofsTex;
		// ヒープの先頭からテクスチャイメージまでのサイズ
		newSize = (u32)(texImgStartAddr - (u8*)scene_wk->Data.TexFile);
		sys_CutMemoryBlockSize( scene_wk->Data.TexFile, newSize );
	}
}

static void Scene2_Load3D_MapModel(SCENE2_WORK *scene_wk, int map_resource_no, int map_start, int map_end)
{
	u8 i;
	ARCHANDLE* handle;
	BOOL status;
	NNSG3dResMdlSet* model_set;
	NNSG3dResMdl *model;
	
	if(map_end > OP_MAP_BLOCK_MAX){
		map_end = OP_MAP_BLOCK_MAX;
	}
	
	//アーカイブハンドルオープン
	handle = ArchiveDataHandleOpen( ARC_OP_DEMO, HEAPID_OP_DEMO );
	
	for (i = map_start; i < map_end; i++){
		if(ModelArcTbl[map_resource_no][i] == 0){
			break;
		}
		
		//モデリングデータをロード
		scene_wk->Data.Block[i].ResFile = ArchiveDataLoadAllocByHandle( handle, ModelArcTbl[map_resource_no][i], HEAPID_OP_DEMO );

		//テクスチャバインド
		status = BindTexture(scene_wk->Data.Block[i].ResFile, scene_wk->Data.Texture);
		GF_ASSERT(status&&"BindTexture failed");
	
		model_set = NNS_G3dGetMdlSet(scene_wk->Data.Block[i].ResFile);

		GF_ASSERT(model_set->dict.numEntry==1&&"モデルセットの中に複数モデルが存在");

		model = NNS_G3dGetMdlByIdx(NNS_G3dGetMdlSet(scene_wk->Data.Block[i].ResFile), 0);
		GF_ASSERT(model&&"load failed");
		NNS_G3dRenderObjInit(&scene_wk->Data.Block[i].RenderObj, model);

		//モデリングデータはグローバルステートを使用する
		NNS_G3dMdlUseGlbDiff(model);			// ディフューズ
		NNS_G3dMdlUseGlbAmb(model);				// アンビエント
		NNS_G3dMdlUseGlbSpec(model);			// スペキュラー
		NNS_G3dMdlUseGlbEmi(model);				// エミッション
	
		NNS_G3dMdlUseGlbLightEnableFlag(model);
		
		scene_wk->Data.block_num++;
	}
	//ハンドルクローズ
	ArchiveDataHandleClose( handle );
}

static void Scene2_Load3D_Camera(SCENE2_WORK *scene_wk, int map_resource_no)
{
	scene_wk->Data.CameraPtr = GFC_AllocCamera(HEAPID_OP_DEMO);	//カメラ作成
	{
		VecFx32 target = {0,0,FX32_ONE*16*6};
		//カメラセット
		GFC_InitCameraTDA( &target,
			CameraData.Distance,	// 距離
			&CameraData.Angle,	//
			CameraData.PerspWay,	// 投射影の角度
			CameraData.View,
			FALSE,				//バインドしない
			scene_wk->Data.CameraPtr
			);
		GFC_SetCameraPerspWay(0x0981,scene_wk->Data.CameraPtr);

		//ソノオタウンの高さにあわせる
		{
			VecFx32 move = {0,SONOO_TOWN_HEIGHT,0};
			GFC_ShiftCamera(&move,scene_wk->Data.CameraPtr);
		}

		GFC_AttachCamera(scene_wk->Data.CameraPtr);
		
//		GFC_SetCameraClip(FX32_ONE * 150, FX32_ONE * 1800, scene_wk->Data.CameraPtr);
	}
}

static void Scene2_Load3D(SCENE2_WORK *scene_wk, int map_resource_no)
{
	Scene2_Load3D_Tex(scene_wk, map_resource_no);
	Scene2_Load3D_MapModel(scene_wk, map_resource_no, 0, OP_MAP_BLOCK_MAX);
	Scene2_Load3D_Camera(scene_wk, map_resource_no);
	
	scene_wk->Load3DOK = 1;		//3Dload通過
}

static BOOL Scene2_Load3D_Div(SCENE2_WORK *scene_wk, int map_resource_no)
{
	if(map_resource_no == 0){
		switch(scene_wk->div_seq){
		case 0:
			scene_wk->Load3D_DivMode = TRUE;
			Scene2_Load3D_Tex(scene_wk, map_resource_no);
			Scene2_Load3D_Camera(scene_wk, map_resource_no);
			break;
		case 1:
			Scene2_Load3D_MapModel(scene_wk, map_resource_no, 0, 1);
			break;
		case 2:
		default:
			Scene2_Load3D_MapModel(scene_wk, map_resource_no, 1, OP_MAP_BLOCK_MAX);
			scene_wk->div_seq = 0;
			scene_wk->Load3D_DivMode = 0;
			scene_wk->Load3DOK = 1;		//3Dload通過
			return TRUE;
		}
	}
	else{
		switch(scene_wk->div_seq){
		case 0:
			scene_wk->Load3D_DivMode = TRUE;
			Scene2_Load3D_Tex(scene_wk, map_resource_no);
			Scene2_Load3D_Camera(scene_wk, map_resource_no);
			break;
		case 1:
			Scene2_Load3D_MapModel(scene_wk, map_resource_no, 0, 2);
			break;
		case 2:
			Scene2_Load3D_MapModel(scene_wk, map_resource_no, 2, 4);
			break;
		case 3:
		default:
			Scene2_Load3D_MapModel(scene_wk, map_resource_no, 4, OP_MAP_BLOCK_MAX);
			scene_wk->div_seq = 0;
			scene_wk->Load3D_DivMode = 0;
			scene_wk->Load3DOK = 1;		//3Dload通過
			return TRUE;
		}
	}
	
	scene_wk->div_seq++;
	return FALSE;
}

static void Scene2_Disp3D(SCENE2_WORK *scene_wk)
{
	u8 i;
	MtxFx33 rotate_tmp = {FX32_ONE,0,0,0,FX32_ONE,0,0,0,FX32_ONE};
	VecFx32 scale = {FX32_ONE,FX32_ONE,FX32_ONE};
	VecFx32 trans = {0,0,0};
	NNS_G3dGeFlushBuffer();
	GF_G3X_Reset();
	GFC_CameraLookAt();

	if(scene_wk->map_draw_ok == TRUE){
		for(i=0;i<scene_wk->Data.block_num;i++){
			DC_FlushAll();
			simple_3DModelDraw(	&scene_wk->Data.Block[i].RenderObj,
								&trans,
								&rotate_tmp,
								&scale);
		}
	}

	if(scene_wk->particle_occ == TRUE){
		NNS_G3dGeFlushBuffer();
		GF_G3X_Reset();
		Particle_DrawAll();	//パーティクル描画
		Particle_CalcAll();	//パーティクル計算
	}

	GF_G3_RequestSwapBuffers(GX_SORTMODE_AUTO,GX_BUFFERMODE_W);
}

static BOOL Scene2_Main(OPENING_DEMO_WORK *wk, SCENE2_WORK *scene_wk, const int inCounter)
{
	u8 *seq = &(scene_wk->SceneSeq);

	if (!scene_wk->KiraMargin){
		Scene2_KiraGenerateMain(scene_wk->KiraListPtr, scene_wk->kira_vanish);
	}
	scene_wk->KiraMargin = (scene_wk->KiraMargin+1)%scene_wk->KiraMarginCore;
	
	switch(*seq){
	case 0:	//タイミングを取って、バーン！とポケモンロゴ
		if(inCounter == FM_TITLE_LOGO_IN - 1){
		}
		
		if (inCounter >= FM_TITLE_LOGO_IN){
			GF_BGL_VisibleSet(MAIN_BG3_FRAME, VISIBLE_ON);
		//	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
			//タイトルロゴの輝度を元に戻す
			ChangeBrightnessRequest(TITLE_LOGO_WHITE_IN_SYNC, BRIGHTNESS_NORMAL, BRIGHTNESS_BLACK,
								GX_BLEND_PLANEMASK_BG3, MASK_MAIN_DISPLAY);
			(*seq)++;
		}
		break;
	case 1:
		if(IsFinishedBrightnessChg(MASK_MAIN_DISPLAY) == TRUE 
				&& inCounter == FM_TITLE_LOGO_PARTICLE){
			GF_BGL_VisibleSet(MAIN_BG0_FRAME, VISIBLE_ON);
			OPD_EmitterSet(scene_wk->OpSsmPtr, TITLE_SCENE1_OP_TITLE01);
			OPD_EmitterSet(scene_wk->OpSsmPtr, TITLE_SCENE1_OP_TITLE02);
			OPD_EmitterSet(scene_wk->OpSsmPtr, TITLE_SCENE1_OP_TITLE03);
			G2_SetBlendAlpha( 0, GX_BLEND_PLANEMASK_BG0|GX_BLEND_PLANEMASK_BG1|GX_BLEND_PLANEMASK_BG2|GX_BLEND_PLANEMASK_BG3 , 0, 0);
			scene_wk->particle_occ = TRUE;

			//主人公アニメ開始
			OPD_Obj_OnOffHeroHeroineAnime( scene_wk->ObjWorkPtr, 1 );
			(*seq)++;
		}
/**		
		//よきところで下面ホワイトアウト
		if (inCounter >= FM_TITLE_WHITE_OUT){
			WIPE_SYS_Start(	WIPE_PATTERN_M, WIPE_TYPE_FADEOUT,
								WIPE_TYPE_FADEOUT,0x7fff,OPDEMO_BRIGHTNESS_SYNC,1,HEAPID_OP_DEMO );
			(*seq)++;
		}
*/
		break;
	case 2:
		if(inCounter == FM_TITLE_LOGO_FLASH_OUT){
			//ホワイトアウト
			WIPE_SYS_Start(	WIPE_PATTERN_M, WIPE_TYPE_FADEOUT,
				WIPE_TYPE_FADEOUT,0x7fff,TITLE_LOGO_NEW_WHITE_IN_SYNC,1,HEAPID_OP_DEMO );
		}
		if(inCounter == FM_TITLE_LOGO_FLASH_IN){
			GF_ASSERT(WIPE_SYS_EndCheck() == TRUE);

			scene_wk->bg_visible_on_req = (1<<SKY_FRAME);
			scene_wk->bg_visible_off_req = (1<<MAIN_BG3_FRAME);
			//ホワイトイン
			WIPE_SYS_Start(	WIPE_PATTERN_M, WIPE_TYPE_FADEIN,
				WIPE_TYPE_FADEIN,0x7fff,TITLE_LOGO_NEW_WHITE_IN_SYNC,1,HEAPID_OP_DEMO );
		}
		
		if(inCounter == FM_KIRA_DISP_START -1){
			GF_Disp_GX_VisibleControl(GX_PLANEMASK_BG0, VISIBLE_OFF);
			scene_wk->v_req_scene2_map_disp_change = TRUE;
			scene_wk->particle_occ = FALSE;
			scene_wk->map_draw_ok = TRUE;

			//もしまだ存在しているならここでエミッタ削除
			GF_ASSERT(Particle_GetEmitterNum(OPD_Poke_PtcGet(scene_wk->OpSsmPtr)) == 0);
		}
		if (inCounter==FM_KIRA_DISP_START){
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
		}
		
		if ( /*WIPE_SYS_EndCheck()&&*/(inCounter>=FM_SKY_CAMERA_START) ){
			//メイン3D・ＯＢＪ表示
			GF_Disp_GX_VisibleControl(GX_PLANEMASK_BG0, VISIBLE_ON);
			//ホワイトイン
			WIPE_SYS_Start(	WIPE_PATTERN_M, WIPE_TYPE_FADEIN,
							WIPE_TYPE_FADEIN,0x7fff,OPDEMO_BRIGHTNESS_SYNC,1,HEAPID_OP_DEMO );


			//主人公男表示
		//	OPD_Obj_DispOnOff( scene_wk->ObjWorkPtr, ACT_WORK_HERO, 1 );

			scene_wk->KiraMarginCore = SCENE2_KIRA_MARGIN2;
			
			(*seq)++;
		}
		break;
	case 3:	//インしたら、3Ｄカメラ移動シーン（裏で2Ｄ面の表示はしておく）
		if ( /*(inCounter>=FM_SKY_CAMERA_START)*/1 ){
			Scene2_MapChange(wk, scene_wk, inCounter);
			//空撮
			Scene2_CameraMoveMain(scene_wk,inCounter);
			//鳥スライド
			OPD_Obj_SlideInBird(scene_wk->ObjWorkPtr, inCounter);
			
			if(inCounter == FM_HEROINE_APPEAR){
				//主人公女表示
		//		OPD_Obj_DispOnOff( scene_wk->ObjWorkPtr, ACT_WORK_HEROINE, 1 );
		//		OPD_Obj_DispOnOff( scene_wk->ObjWorkPtr, ACT_WORK_HERO, 0 );
			}
			if(inCounter == FM_CAMEA2_END){
		//		OPD_Obj_DispOnOff( scene_wk->ObjWorkPtr, ACT_WORK_HEROINE, 0 );
			}
			OPD_Obj_SlideInRival(scene_wk->ObjWorkPtr, inCounter);
			OPD_Obj_SlideInHakase(scene_wk->ObjWorkPtr, inCounter);

			if (inCounter >= FM_3D_WHITE_OUT){
				//空撮終了ホワイトアウトリクエスト
				WIPE_SYS_Start(	WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT,
								WIPE_TYPE_FADEOUT,0x7fff,OPDEMO_BRIGHTNESS_SYNC,1,HEAPID_OP_DEMO );
				(*seq)++;
			}
		}
		break;
	case 4:
		//空撮
		Scene2_CameraMoveMain(scene_wk,inCounter);
		//鳥スライド
		OPD_Obj_SlideInBird(scene_wk->ObjWorkPtr, inCounter);
		//ライバル＆博士
		OPD_Obj_SlideInRival(scene_wk->ObjWorkPtr, inCounter);
		OPD_Obj_SlideInHakase(scene_wk->ObjWorkPtr, inCounter);
		//ホワイトアウト待ち
		if ( WIPE_SYS_EndCheck()&&(inCounter >= FM_HERO_WALK) ){
			GF_BGL_VisibleSet(WB_TITLE_FRAME, VISIBLE_OFF);	//GFロゴを消す

			//キラキラ削除
			OPD_Obj_KiraActorDrawOff(scene_wk->ObjWorkPtr, scene_wk->KiraListPtr );
			scene_wk->kira_vanish = TRUE;
		//	OPDKira_FreeList(scene_wk->KiraListPtr);
		//	scene_wk->KiraListPtr = NULL;

			//真っ白な間にBGを再構成
			Scene2_HeroBGTrans(wk, scene_wk);

			//両画面に主人公表示
			OPD_Obj_DispOnOff( scene_wk->ObjWorkPtr, ACT_WORK_HERO_SUB, 1 );
			OPD_Obj_DispOnOff( scene_wk->ObjWorkPtr, ACT_WORK_HEROINE, 1 );
			
			//街と空BG表示
			GF_BGL_VisibleSet(TOWN_FRAME, VISIBLE_ON);
			GF_BGL_VisibleSet(SKY_FRAME, VISIBLE_ON);
			GF_BGL_VisibleSet(MAIN_BG3_FRAME, VISIBLE_ON);
			GF_BGL_VisibleSet(SUB_BG1_FRAME, VISIBLE_ON);
			GF_BGL_VisibleSet(SUB_BG2_FRAME, VISIBLE_ON);
			GF_BGL_VisibleSet(SUB_BG3_FRAME, VISIBLE_ON);
			GF_BGL_ScrollSet( scene_wk->bgl, TOWN_FRAME, GF_BGL_SCROLL_X_SET, 256 );
			GF_BGL_ScrollSet( scene_wk->bgl, TOWN_FRAME, GF_BGL_SCROLL_Y_SET, 0 );
			GF_BGL_ScrollSet( scene_wk->bgl, SKY_FRAME, GF_BGL_SCROLL_X_SET, 0 );
			GF_BGL_ScrollSet( scene_wk->bgl, SKY_FRAME, GF_BGL_SCROLL_Y_SET, 256 );
			GF_BGL_ScrollSet( scene_wk->bgl, MAIN_BG3_FRAME, GF_BGL_SCROLL_X_SET, 0 );
			GF_BGL_ScrollSet( scene_wk->bgl, MAIN_BG3_FRAME, GF_BGL_SCROLL_Y_SET, 0 );
			GF_BGL_ScrollSet( scene_wk->bgl, SUB_BG1_FRAME, GF_BGL_SCROLL_X_SET, 256 );
			GF_BGL_ScrollSet( scene_wk->bgl, SUB_BG1_FRAME, GF_BGL_SCROLL_Y_SET, 0 );
			GF_BGL_ScrollSet( scene_wk->bgl, SUB_BG2_FRAME, GF_BGL_SCROLL_X_SET, 0 );
			GF_BGL_ScrollSet( scene_wk->bgl, SUB_BG2_FRAME, GF_BGL_SCROLL_Y_SET, 256 );
			GF_BGL_ScrollSet( scene_wk->bgl, SUB_BG3_FRAME, GF_BGL_SCROLL_X_SET, 0 );
			GF_BGL_ScrollSet( scene_wk->bgl, SUB_BG3_FRAME, GF_BGL_SCROLL_Y_SET, 0 );
			scene_wk->scroll_sp_tree += 0;
			scene_wk->scroll_sp_town = 256 << FX32_SHIFT;
			
			//ウィンドウ表示
			scene_wk->wnd0_m_left = 0;
			scene_wk->wnd0_m_right = 255;
			scene_wk->wnd0_m_top = 32;
			scene_wk->wnd0_m_bottom = 159;
			scene_wk->wnd0_s_left = 0;
			scene_wk->wnd0_s_right = 255;
			scene_wk->wnd0_s_top = 32;
			scene_wk->wnd0_s_bottom = 159;
			scene_wk->window_on_req = TRUE;
			//3Dモデルドロー関数を無効
			scene_wk->Disp3DFlg = 0;
			
			//BLEND無効化
			G2_BlendNone();
			G2S_BlendNone();

			//ホワイトインリクエスト
			WIPE_SYS_Start(	WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN,
							WIPE_TYPE_FADEIN,0x7fff,OPDEMO_BRIGHTNESS_SYNC,1,HEAPID_OP_DEMO );
			(*seq)++;
		}
		break;
	case 5:	//両画面で主人公が走る
		//町スクロール
		Scene2_ScrollTown(scene_wk);
		//主人公スクロール
		OPD_Obj_HeroHeroineMove(scene_wk->ObjWorkPtr);
		//ホワイトイン待ち
		if ( WIPE_SYS_EndCheck() ){
			if(inCounter >= FM_BALL_THROW){
				//流星BGに座標を合わせる
				GF_BGL_ScrollReq(scene_wk->bgl, MAIN_BG3_FRAME, GF_BGL_SCROLL_X_SET, 0);
				GF_BGL_ScrollReq(scene_wk->bgl, MAIN_BG3_FRAME, GF_BGL_SCROLL_Y_SET, 256);
				GF_BGL_ScrollReq(scene_wk->bgl, SUB_BG3_FRAME, GF_BGL_SCROLL_X_SET, 0);
				GF_BGL_ScrollReq(scene_wk->bgl, SUB_BG3_FRAME, GF_BGL_SCROLL_Y_SET, 256);

				GF_BGL_ScrollReq(scene_wk->bgl, SKY_FRAME, GF_BGL_SCROLL_X_SET, 0);
				GF_BGL_ScrollReq(scene_wk->bgl, SKY_FRAME, GF_BGL_SCROLL_Y_SET, 0);
				GF_BGL_ScrollReq(scene_wk->bgl, SUB_BG2_FRAME, GF_BGL_SCROLL_X_SET, 0);
				GF_BGL_ScrollReq(scene_wk->bgl, SUB_BG2_FRAME, GF_BGL_SCROLL_Y_SET, 0);
				
			//	scene_wk->bg_visible_on_req = (1<<SKY_FRAME) | (1<<SUB_BG2_FRAME);
				scene_wk->bg_visible_off_req = (1<<TOWN_FRAME) | (1<<SUB_BG1_FRAME);

				scene_wk->window_off_req = TRUE;
				
				//主人公を消す
				OPD_Obj_DispOnOff( scene_wk->ObjWorkPtr, ACT_WORK_HERO_SUB, 0 );
				OPD_Obj_DispOnOff( scene_wk->ObjWorkPtr, ACT_WORK_HEROINE, 0 );

				(*seq)++;
			}
		}
		break;
	case 6:	//ボールを投げる
		Scene2_ScrollBallThrowStream(scene_wk);
		Scene2_ScrollHand(scene_wk, inCounter);
		if(inCounter == FM_BALL_HAND){
			WIPE_SYS_Start(	WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT,
				WIPE_TYPE_FADEOUT,0x0000,SCENE2_POKE_APPEAR_FLASH_SYNC,1,HEAPID_OP_DEMO );
			(*seq)++;
		}
		break;
	case 7:	//手のアップ
		Scene2_ScrollBallThrowStream(scene_wk);
		Scene2_ScrollHand(scene_wk, inCounter);
		if(WIPE_SYS_EndCheck()){
			//手のアップBGを表示
			scene_wk->bg_visible_on_req = (1<<TOWN_FRAME) | (1<<SUB_BG1_FRAME);
			scene_wk->bg_visible_off_req = (1<<SKY_FRAME) | (1<<SUB_BG2_FRAME);
			GF_BGL_ScrollReq(scene_wk->bgl, TOWN_FRAME, GF_BGL_SCROLL_X_SET, 0);
			GF_BGL_ScrollReq(scene_wk->bgl, TOWN_FRAME, GF_BGL_SCROLL_Y_SET, 0);
			GF_BGL_ScrollReq(scene_wk->bgl, SUB_BG1_FRAME, GF_BGL_SCROLL_X_SET, 0);
			GF_BGL_ScrollReq(scene_wk->bgl, SUB_BG1_FRAME, GF_BGL_SCROLL_Y_SET, 0);

			WIPE_SYS_Start(	WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN,
				WIPE_TYPE_FADEIN,0x0000,SCENE2_POKE_APPEAR_FLASH_SYNC,1,HEAPID_OP_DEMO );
			(*seq)++;
		}
		break;
	case 8:	//ボールが手から離れる
		Scene2_ScrollBallThrowStream(scene_wk);
		Scene2_ScrollHand(scene_wk, inCounter);
		OPD_Obj_BallRotate(scene_wk->ObjWorkPtr, inCounter);
		if(WIPE_SYS_EndCheck()){
			if(inCounter == FM_BALL_HAND_RELEASE){
				//手からボールが離れる
				
				//手のアニメ2コマ目を表示
				GF_BGL_ScrollReq(scene_wk->bgl, TOWN_FRAME, GF_BGL_SCROLL_X_SET, 0);
				GF_BGL_ScrollReq(scene_wk->bgl, TOWN_FRAME, GF_BGL_SCROLL_Y_SET, 256);
				GF_BGL_ScrollReq(scene_wk->bgl, SUB_BG1_FRAME, GF_BGL_SCROLL_X_SET, 0);
				GF_BGL_ScrollReq(scene_wk->bgl, SUB_BG1_FRAME, GF_BGL_SCROLL_Y_SET, 256);
				
				//ボールアクター表示
				OPD_Obj_DispOnOff( scene_wk->ObjWorkPtr, ACT_WORK_BALL_MAIN, 1 );
				OPD_Obj_DispOnOff( scene_wk->ObjWorkPtr, ACT_WORK_BALL_SUB, 1 );
			}
			else if(inCounter >= FM_POKE_APPEAR_FLASH){
				WIPE_SYS_Start(	WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT,
					WIPE_TYPE_FADEOUT,0x7fff,SCENE2_POKE_APPEAR_FLASH_SYNC,1,HEAPID_OP_DEMO );
				(*seq)++;
			}
		}
		break;
	case 9:
		OPD_Obj_BallRotate(scene_wk->ObjWorkPtr, inCounter);
		if ( WIPE_SYS_EndCheck() ){
			WIPE_SYS_Start(	WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN,
				WIPE_TYPE_FADEIN,0x7fff,SCENE2_POKE_APPEAR_AFTER_FLASH_SYNC,1,HEAPID_OP_DEMO );

			OPD_Obj_DispOnOff( scene_wk->ObjWorkPtr, ACT_WORK_BALL_MAIN, 0 );
			OPD_Obj_DispOnOff( scene_wk->ObjWorkPtr, ACT_WORK_BALL_SUB, 0 );
			OPD_Obj_DispOnOff( scene_wk->ObjWorkPtr, ACT_WORK_STREAM_MAIN_0, 1 );
			OPD_Obj_DispOnOff( scene_wk->ObjWorkPtr, ACT_WORK_STREAM_MAIN_1, 1 );
			OPD_Obj_DispOnOff( scene_wk->ObjWorkPtr, ACT_WORK_STREAM_SUB_0, 1 );
			OPD_Obj_DispOnOff( scene_wk->ObjWorkPtr, ACT_WORK_STREAM_SUB_1, 1 );
			OPD_Obj_StreamDispReset(scene_wk->ObjWorkPtr, FALSE);

			//真っ白な間にBGを再構成
			Scene2_PokeBGTrans(wk, scene_wk);

			//上画面から技を繰り出す為、メイン画面変更
			sys.disp3DSW = DISP_3D_TO_MAIN;
			GF_Disp_DispSelect();
			Scene2_PokeBG_MainSub_DispSetting(scene_wk);
			
		//	scene_wk->bg_visible_on_req |= (1<<TOWN_FRAME) | (1<<SKY_FRAME) | (1<<MAIN_BG3_FRAME);
		//	scene_wk->bg_visible_on_req |= (1<<SUB_BG1_FRAME) 
		//		| (1<<SUB_BG2_FRAME) | (1<<SUB_BG3_FRAME);
			
			scene_wk->window_off_req = TRUE;
			(*seq)++;
		}
		else{
			//BG非表示リクエストやスクロール更新が被る可能性があるので
			//こいつだけWipeチェックした後で行う。
			Scene2_ScrollBallThrowStream(scene_wk);
			Scene2_ScrollHand(scene_wk, inCounter);
		}
		break;
	case 10:	//ポケモン登場
		OPD_Obj_StreamScroll(scene_wk->ObjWorkPtr, POKE_STREAM_SPEED);
		Scene2_PokeAppearSlideIn(scene_wk, inCounter);
		
		if ( WIPE_SYS_EndCheck() ){
			if(inCounter >= FM_WAZA_RIGHT){
				//ブラックアウト速め
				WIPE_SYS_Start(	WIPE_PATTERN_M, WIPE_TYPE_FADEOUT,
							WIPE_TYPE_FADEOUT,0x0000,SCENE2_DISP_CHG_SYNC,1,HEAPID_OP_DEMO );
				(*seq)++;
			}
		}
		break;
	case 11:	//ブラックアウトしたら下記処理
		OPD_Obj_StreamScroll(scene_wk->ObjWorkPtr, POKE_STREAM_SPEED);
		if ( WIPE_SYS_EndCheck() ){
			G2_SetBlendAlpha(GX_BLEND_PLANEMASK_NONE,
				GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG1 
				| GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD, 0,0);
			
			//技を出す方のポケモンBGのパレットを真っ黒にする
			MI_CpuClear16((void*)HW_BG_PLTT, 0x200);
			
			//ブラックイン速め
			WIPE_SYS_Start(	WIPE_PATTERN_M, WIPE_TYPE_FADEIN,
							WIPE_TYPE_FADEIN,0x0000,SCENE2_DISP_CHG_SYNC,1,HEAPID_OP_DEMO );
			(*seq)++;
		}
		break;
	case 12:	//ポッチャマカットイン
		OPD_Obj_StreamScroll(scene_wk->ObjWorkPtr, POKE_STREAM_SPEED);
		if ( WIPE_SYS_EndCheck() ){
			if ( OPD_Poke_CutIn(scene_wk->OpSsmPtr, CUT_IN_POKE_0, inCounter) ){
				//ブラックアウト速め
				WIPE_SYS_Start(	WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT,
							WIPE_TYPE_FADEOUT,0x0000,SCENE2_DISP_CHG_SYNC,1,HEAPID_OP_DEMO );
				(*seq)++;
			}
		}
		break;

	case 13:	//ポッチャマの技終了待ち
		OPD_Obj_StreamScroll(scene_wk->ObjWorkPtr, POKE_STREAM_SPEED);
		if ( WIPE_SYS_EndCheck() ){
			if (1){//inCounter >= FM_RIVAL_SETUP){
				//下画面から技を繰り出す為、メイン画面変更
				sys.disp3DSW = DISP_3D_TO_SUB;
				GF_Disp_DispSelect();
				Scene2_PokeBG_MainSub_DispSetting(scene_wk);
				
				//ブラックイン速め
				WIPE_SYS_Start(	WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN,
								WIPE_TYPE_FADEIN,0x0000,SCENE2_DISP_CHG_SYNC,1,HEAPID_OP_DEMO );
				(*seq)++;
			}
		}
		break;
	case 14:	//ヒコザルカットイン
		OPD_Obj_StreamScroll(scene_wk->ObjWorkPtr, POKE_STREAM_SPEED);
		if ( WIPE_SYS_EndCheck() ){
			if(inCounter >= FM_WAZA_LEFT){
				if ( OPD_Poke_CutIn(scene_wk->OpSsmPtr, CUT_IN_POKE_1, inCounter) ){
					//ブラックアウト速め
					WIPE_SYS_Start(	WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT,
								WIPE_TYPE_FADEOUT,0x0000,SCENE2_DISP_CHG_SYNC,1,HEAPID_OP_DEMO );
					scene_wk->BG_ScrollMargin = 0;	//スクロールに備え初期化
					(*seq)++;
				}
			}
		}
		break;
	case 15:	//ヒコザルの技終了待ち
		OPD_Obj_StreamScroll(scene_wk->ObjWorkPtr, POKE_STREAM_SPEED);
		if ( WIPE_SYS_EndCheck() ){
			//メイン画面のポケモンBGのパレットを元に戻す
			MI_CpuCopy16(scene_wk->pokebg_palbuf, (void*)HW_BG_PLTT, 0x200);

			//メイン画面を上に戻す(雷を落とすとき考えやすいので)
			sys.disp3DSW = DISP_3D_TO_MAIN;
			GF_Disp_DispSelect();
			
			//真っ黒な間にBG再構成
			Scene2_ThunderBGTrans(wk, scene_wk);
			Scene2_PokeBG_MainSub_DispSetting(scene_wk);

			//流星消す
		//	OPD_Obj_DispOnOff( scene_wk->ObjWorkPtr, ACT_WORK_STREAM_MAIN_0, 0 );
		//	OPD_Obj_DispOnOff( scene_wk->ObjWorkPtr, ACT_WORK_STREAM_MAIN_1, 0 );
		//	OPD_Obj_DispOnOff( scene_wk->ObjWorkPtr, ACT_WORK_STREAM_SUB_0, 0 );
		//	OPD_Obj_DispOnOff( scene_wk->ObjWorkPtr, ACT_WORK_STREAM_SUB_1, 0 );
			OPD_Obj_StreamDispReset(scene_wk->ObjWorkPtr, TRUE);

			//体当たりさせるので、ポケモンを全体的に後ろに下げる
			GF_BGL_ScrollReq(scene_wk->bgl, TOWN_FRAME, GF_BGL_SCROLL_X_SET, 
				-POKE_ATTACK_SCROLL_OFFSET_X);
			GF_BGL_ScrollReq(scene_wk->bgl, SKY_FRAME, GF_BGL_SCROLL_X_SET, 
				-POKE_ATTACK_SCROLL_OFFSET_X);
			GF_BGL_ScrollReq(scene_wk->bgl, MAIN_BG3_FRAME, GF_BGL_SCROLL_X_SET, 
				-POKE_ATTACK_SCROLL_OFFSET_X);
			GF_BGL_ScrollReq(scene_wk->bgl, SUB_BG1_FRAME, GF_BGL_SCROLL_X_SET, 
				POKE_ATTACK_SCROLL_OFFSET_X);
			GF_BGL_ScrollReq(scene_wk->bgl, SUB_BG2_FRAME, GF_BGL_SCROLL_X_SET, 
				POKE_ATTACK_SCROLL_OFFSET_X);
			GF_BGL_ScrollReq(scene_wk->bgl, SUB_BG3_FRAME, GF_BGL_SCROLL_X_SET, 
				POKE_ATTACK_SCROLL_OFFSET_X);
			
			//ブラックイン少しゆっくり
			WIPE_SYS_Start(	WIPE_PATTERN_M, WIPE_TYPE_FADEIN,
					WIPE_TYPE_FADEIN,0x0000,SCENE2_DISP_CHG_SYNC_WAZA_AFTER,1,HEAPID_OP_DEMO );
			(*seq)++;
		}
		break;
	case 16:
		OPD_Obj_StreamScroll(scene_wk->ObjWorkPtr, POKE_STREAM_SPEED);
		if ( WIPE_SYS_EndCheck() ){
			WIPE_SYS_Start(	WIPE_PATTERN_S, WIPE_TYPE_FADEIN,
					WIPE_TYPE_FADEIN,0x0000,SCENE2_DISP_CHG_SYNC_WAZA_AFTER,1,HEAPID_OP_DEMO );
			(*seq)++;
		}
		break;
	case 17:	//体当たり
		OPD_Obj_StreamScroll(scene_wk->ObjWorkPtr, POKE_STREAM_SPEED);
		if ( WIPE_SYS_EndCheck() ){
			if(inCounter >= FM_POKE_ATTACK){
				Scene2_ScrollPokeAttack(scene_wk);
			}
			if(inCounter >= FM_SCENE2_END_WIPE){
				//雷表示
				scene_wk->bg_visible_on_req |= (1<<MAIN_BG0_FRAME)|(1<<WB_TITLE_FRAME);
				
				//ホワイトアウト
				WIPE_SYS_Start(	WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT,
					WIPE_TYPE_FADEOUT,0x7fff,SCENE2_DISP_CHG_SYNC_THUNDER,1,HEAPID_OP_DEMO );
				(*seq)++;
			}
		}
		break;
	case 18:
		Scene2_ScrollPokeAttack(scene_wk);
		OPD_Obj_StreamScroll(scene_wk->ObjWorkPtr, POKE_STREAM_SPEED);
		//フェードアウト待ち
		if ( WIPE_SYS_EndCheck() ){
			sys.disp3DSW = DISP_3D_TO_SUB;
			GF_Disp_DispSelect();
			(*seq)++;
		}
		break;
	case 19:	//終了
		return TRUE;
		break;
	}	
	return FALSE;
}

static void Scene2_ScrollTown(SCENE2_WORK *scene_wk)
{
	scene_wk->scroll_sp_town += SCENE2_SCROLL_SP_TOWN;
	scene_wk->scroll_sp_tree += SCENE2_SCROLL_SP_TREE;
	
//	if (scene_wk->BG_ScrollMargin++ > SCENE2_TOWN_SCROLL_MARGIN){
		GF_BGL_ScrollSet( scene_wk->bgl, TOWN_FRAME, GF_BGL_SCROLL_Y_SET, 
			-(scene_wk->scroll_sp_tree >> FX32_SHIFT) );
		GF_BGL_ScrollSet( scene_wk->bgl, SKY_FRAME, GF_BGL_SCROLL_Y_SET, 
			-(scene_wk->scroll_sp_town >> FX32_SHIFT) );

		GF_BGL_ScrollSet( scene_wk->bgl, SUB_BG1_FRAME, GF_BGL_SCROLL_Y_SET, 
			-(scene_wk->scroll_sp_tree >> FX32_SHIFT) );
		GF_BGL_ScrollSet( scene_wk->bgl, SUB_BG2_FRAME, GF_BGL_SCROLL_Y_SET, 
			-(scene_wk->scroll_sp_town >> FX32_SHIFT) );

		scene_wk->BG_ScrollMargin = 0;
//	}
}

static void Scene2_ScrollSky(SCENE2_WORK *scene_wk)
{
	if (scene_wk->BG_ScrollMargin++ > SCENE2_SKY_SCROLL_MARGIN){
		GF_BGL_ScrollSet( scene_wk->bgl, SKY_FRAME, GF_BGL_SCROLL_X_INC, SKY_BG_SPEED );
		scene_wk->BG_ScrollMargin = 0;
	}
}

static void Scene2_ScrollBattle(SCENE2_WORK *scene_wk)
{
///	if (scene_wk->BG_ScrollMargin++ > SCENE2_BATTLE_SCROLL_MARGIN){
		GF_BGL_ScrollSet( scene_wk->bgl, MAIN_BG3_FRAME, GF_BGL_SCROLL_X_INC, BATTLE_BG_SPEED );
		scene_wk->BG_ScrollMargin = 0;
///	}
}

static void Scene2_ScrollBallThrowStream(SCENE2_WORK *scene_wk)
{
	GF_BGL_ScrollReq( scene_wk->bgl, MAIN_BG3_FRAME, 
		GF_BGL_SCROLL_X_INC, BALL_THROW_STREAM_BG_SPEED);
	GF_BGL_ScrollReq( scene_wk->bgl, SUB_BG3_FRAME, 
		GF_BGL_SCROLL_X_INC, -BALL_THROW_STREAM_BG_SPEED);
}

static void Scene2_ScrollHand(SCENE2_WORK *scene_wk, int inCounter)
{
	if(inCounter == FM_BALL_HAND_RELEASE){
		//ウィンドウでスクロールの回り込みが発生しないように隠す
		//このタイミングの時はまだスクロールしていないので、Vブランクでやらなくても大丈夫
		
		//メイン　左にスクロールする為、ウィンドウ内は表示ON、外は表示OFF
		G2_SetWnd0InsidePlane(	GX_WND_PLANEMASK_BG0|
								GX_WND_PLANEMASK_BG1|
								GX_WND_PLANEMASK_BG2|
								GX_WND_PLANEMASK_BG3|
								GX_WND_PLANEMASK_OBJ, 1 );
		G2_SetWndOutsidePlane( GX_WND_PLANEMASK_BG0 |
								GX_WND_PLANEMASK_BG2|
								GX_WND_PLANEMASK_BG3|
								GX_WND_PLANEMASK_OBJ, 1 );
		scene_wk->wnd0_m_left = 0;
		scene_wk->wnd0_m_right = 255;
		scene_wk->wnd0_m_top = 0;
		scene_wk->wnd0_m_bottom = 8*0x11;//192;
		G2_SetWnd0Position( scene_wk->wnd0_m_left, scene_wk->wnd0_m_top, 
			scene_wk->wnd0_m_right, scene_wk->wnd0_m_bottom);
		GX_SetVisibleWnd( GX_WNDMASK_W0 );	//座標設定が済んで最後に表示ON
		//サブ
		G2S_SetWnd0InsidePlane(	GX_WND_PLANEMASK_BG0|
								GX_WND_PLANEMASK_BG1|
								GX_WND_PLANEMASK_BG2|
								GX_WND_PLANEMASK_BG3|
								GX_WND_PLANEMASK_OBJ, 1 );
		G2S_SetWndOutsidePlane(	GX_WND_PLANEMASK_BG0|
								GX_WND_PLANEMASK_BG2|
								GX_WND_PLANEMASK_BG3|
								GX_WND_PLANEMASK_OBJ, 1 );
		//右側を囲う為2枚使う
		G2S_SetWnd1InsidePlane(	GX_WND_PLANEMASK_BG0|
								GX_WND_PLANEMASK_BG1|
								GX_WND_PLANEMASK_BG2|
								GX_WND_PLANEMASK_BG3|
								GX_WND_PLANEMASK_OBJ, 1 );
		scene_wk->wnd0_s_left = 0;
		scene_wk->wnd0_s_right = 255;
		scene_wk->wnd0_s_top = 0;
		scene_wk->wnd0_s_bottom = 8*0x11;//192;
		G2S_SetWnd0Position( scene_wk->wnd0_s_left, scene_wk->wnd0_s_top, 
			scene_wk->wnd0_s_right, scene_wk->wnd0_s_bottom);
		G2S_SetWnd1Position( 1, scene_wk->wnd0_s_top, 
			0, scene_wk->wnd0_s_bottom);
		GXS_SetVisibleWnd( GX_WNDMASK_W0 | GX_WNDMASK_W1 );	//座標設定が済んで最後に表示ON
	}
	else if(inCounter > FM_BALL_HAND_RELEASE){
		if(GF_BGL_ScreenScrollXGet(scene_wk->bgl, TOWN_FRAME) >= 256){
			scene_wk->bg_visible_off_req |= (1<<TOWN_FRAME);
		}
		if(GF_BGL_ScreenScrollXGet(scene_wk->bgl, SUB_BG1_FRAME) <= -256){
			scene_wk->bg_visible_off_req |= (1<<SUB_BG1_FRAME);
		}
		GF_BGL_ScrollReq(scene_wk->bgl, TOWN_FRAME, GF_BGL_SCROLL_X_INC, HAND_BG_SPEED);
		GF_BGL_ScrollReq(scene_wk->bgl, TOWN_FRAME, GF_BGL_SCROLL_Y_INC, HAND_Y_BG_SPEED);
		GF_BGL_ScrollReq(scene_wk->bgl, SUB_BG1_FRAME, GF_BGL_SCROLL_X_INC, -HAND_BG_SPEED);
		GF_BGL_ScrollReq(scene_wk->bgl, SUB_BG1_FRAME, GF_BGL_SCROLL_Y_INC, HAND_Y_BG_SPEED);
	}
}

static void Scene2_PokeAppearSlideIn(SCENE2_WORK *scene_wk, int inCounter)
{
	int scr_x, scr_y;
	
	if(inCounter > FM_POKE_APPEAR_FIRST){
		scr_x = GF_BGL_ScrollGetX( scene_wk->bgl, TOWN_FRAME ) + POKE_BG_SLIDE_IN_SPEED;
		if(scr_x > 0){
			scr_x = 0;
		}
		GF_BGL_ScrollReq( scene_wk->bgl, TOWN_FRAME, GF_BGL_SCROLL_X_SET, scr_x);

		scr_x = GF_BGL_ScrollGetX( scene_wk->bgl, SUB_BG1_FRAME ) - POKE_BG_SLIDE_IN_SPEED;
		if(scr_x < 0){
			scr_x = 0;
		}
		GF_BGL_ScrollReq( scene_wk->bgl, SUB_BG1_FRAME, GF_BGL_SCROLL_X_SET, scr_x);
	}
	if(inCounter >= FM_POKE_APPEAR_SECOND){
		scr_x = GF_BGL_ScrollGetX( scene_wk->bgl, SKY_FRAME ) + POKE_BG_SLIDE_IN_SPEED;
		if(scr_x > 0){
			scr_x = 0;
		}
		GF_BGL_ScrollReq( scene_wk->bgl, SKY_FRAME, GF_BGL_SCROLL_X_SET, scr_x);

		scr_x = GF_BGL_ScrollGetX( scene_wk->bgl, SUB_BG2_FRAME ) - POKE_BG_SLIDE_IN_SPEED;
		if(scr_x < 0){
			scr_x = 0;
		}
		GF_BGL_ScrollReq( scene_wk->bgl, SUB_BG2_FRAME, GF_BGL_SCROLL_X_SET, scr_x);
	}
	if(inCounter >= FM_POKE_APPEAR_THIRD){
		scr_x = GF_BGL_ScrollGetX( scene_wk->bgl, MAIN_BG3_FRAME ) + POKE_BG_SLIDE_IN_SPEED;
		if(scr_x > 0){
			scr_x = 0;
		}
		GF_BGL_ScrollReq( scene_wk->bgl, MAIN_BG3_FRAME, GF_BGL_SCROLL_X_SET, scr_x);

		scr_x = GF_BGL_ScrollGetX( scene_wk->bgl, SUB_BG3_FRAME ) - POKE_BG_SLIDE_IN_SPEED;
		if(scr_x < 0){
			scr_x = 0;
		}
		GF_BGL_ScrollReq( scene_wk->bgl, SUB_BG3_FRAME, GF_BGL_SCROLL_X_SET, scr_x);
	}
}

static void Scene2_ScrollPokeAttack(SCENE2_WORK *scene_wk)
{
	int scr_x, scr_y;
	
	scr_x = GF_BGL_ScrollGetX( scene_wk->bgl, TOWN_FRAME ) + POKE_ATTACK_SCROLL_SPEED;
	if(scr_x > 0){
		scr_x = 0;
	}
	GF_BGL_ScrollReq( scene_wk->bgl, TOWN_FRAME, GF_BGL_SCROLL_X_SET, scr_x);

	scr_x = GF_BGL_ScrollGetX( scene_wk->bgl, SUB_BG1_FRAME ) - POKE_ATTACK_SCROLL_SPEED;
	if(scr_x < 0){
		scr_x = 0;
	}
	GF_BGL_ScrollReq( scene_wk->bgl, SUB_BG1_FRAME, GF_BGL_SCROLL_X_SET, scr_x);

	scr_x = GF_BGL_ScrollGetX( scene_wk->bgl, SKY_FRAME ) + POKE_ATTACK_SCROLL_SPEED;
	if(scr_x > 0){
		scr_x = 0;
	}
	GF_BGL_ScrollReq( scene_wk->bgl, SKY_FRAME, GF_BGL_SCROLL_X_SET, scr_x);

	scr_x = GF_BGL_ScrollGetX( scene_wk->bgl, SUB_BG2_FRAME ) - POKE_ATTACK_SCROLL_SPEED;
	if(scr_x < 0){
		scr_x = 0;
	}
	GF_BGL_ScrollReq( scene_wk->bgl, SUB_BG2_FRAME, GF_BGL_SCROLL_X_SET, scr_x);

	scr_x = GF_BGL_ScrollGetX( scene_wk->bgl, MAIN_BG3_FRAME ) + POKE_ATTACK_SCROLL_SPEED;
	if(scr_x > 0){
		scr_x = 0;
	}
	GF_BGL_ScrollReq( scene_wk->bgl, MAIN_BG3_FRAME, GF_BGL_SCROLL_X_SET, scr_x);

	scr_x = GF_BGL_ScrollGetX( scene_wk->bgl, SUB_BG3_FRAME ) - POKE_ATTACK_SCROLL_SPEED;
	if(scr_x < 0){
		scr_x = 0;
	}
	GF_BGL_ScrollReq( scene_wk->bgl, SUB_BG3_FRAME, GF_BGL_SCROLL_X_SET, scr_x);
}

#define ANGLE_MOVE	(-0x680)	//(-0x980)

static void Scene2_CameraMoveMain(SCENE2_WORK *scene_wk, const int inCounter)
{
	VecFx32 move = {0,0,0};
	if (inCounter<FM_MAP2_LOAD){
		//カメラズーム
		{
			u16 persp_way;
			persp_way = GFC_GetCameraPerspWay(scene_wk->Data.CameraPtr);
			if (persp_way-32>0x05c1){
				GFC_AddCameraPerspWay(-32,scene_wk->Data.CameraPtr);
			}else{
				GFC_SetCameraPerspWay(0x05c1,scene_wk->Data.CameraPtr);
			}
		}
		//上に平行移動
		move.z -= FX32_ONE*2;
		GFC_ShiftCamera(&move, scene_wk->Data.CameraPtr);
	}
	else if(inCounter == FM_MAP2_LOAD){
		VecFx32 target = {-(FX32_ONE*16*4),0,-FX32_ONE*16*3};

		//カメラセット
		GFC_InitCameraTDA( &target,
			CameraData.Distance,	// 距離
			&CameraData.Angle,	//
			CameraData.PerspWay,	// 投射影の角度
			CameraData.View,
			FALSE,				//バインドしない
			scene_wk->Data.CameraPtr
		);
		//ソノオタウンの高さにあわせる
		{
			VecFx32 move = {0,SONOO_TOWN_HEIGHT,0};
			GFC_ShiftCamera(&move,scene_wk->Data.CameraPtr);
		}
		//角度つける
		{
	//		CAMERA_ANGLE angle = {0,ANGLE_MOVE,0,0};
	//		GFC_AddCameraAngleRot(&angle,scene_wk->Data.CameraPtr);
		}

		//フォグ設定
		{
			NNS_G3dGeFlushBuffer();
			G3X_SetFog( TRUE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x1000, 4500);	//24927
			G3X_SetFogColor(GX_RGB(31, 31, 31), 31);
			{
				int i;
				u32 fog_table[8];
				for (i = 0; i < 8; i++){
					fog_table[i] =
						(u32)(((i * 16) << 0) | ((i * 16 + 4) << 8) | ((i * 16 + 8) << 16) 
						| ((i * 16 + 12) << 24));
				}
				G3X_SetFogTable(fog_table);
			}
		}
		//2DBGアルファ設定
		{
			G2_SetBlendAlpha(GX_BLEND_PLANEMASK_BG1, 
				GX_BLEND_PLANEMASK_BG0|GX_BLEND_PLANEMASK_BD, 7, 16 - 7);
			
			GF_BGL_PrioritySet( TOWN_FRAME, 0 );
			GF_BGL_VisibleSet(TOWN_FRAME, VISIBLE_ON);
		}
	}
	else if(inCounter<FM_MAP3_LOAD){
		//右に平行移動
		move.x += FX32_ONE * 4;
		GFC_ShiftCamera(&move, scene_wk->Data.CameraPtr);
		GF_BGL_ScrollReq( scene_wk->bgl, TOWN_FRAME, GF_BGL_SCROLL_X_INC, 4);
	}
	else if(inCounter == FM_MAP3_LOAD){
		VecFx32 target = {FX32_ONE*16*30,0,FX32_ONE*0 - 0x70000};
		//カメラセット
		GFC_InitCameraTDA( &target,
			CameraData.Distance,	// 距離
			&CameraData.Angle,	//
			CameraData.PerspWay,	// 投射影の角度
			CameraData.View,
			FALSE,				//バインドしない
			scene_wk->Data.CameraPtr
		);
	//	GFC_SetCameraPerspWay(0x05c1,scene_wk->Data.CameraPtr);
		//ソノオタウンの高さにあわせる
		{
			VecFx32 move = {0,SONOO_TOWN_HEIGHT,0};
			GFC_ShiftCamera(&move,scene_wk->Data.CameraPtr);
		}
		//角度つける
		{
			CAMERA_ANGLE angle = {0,-ANGLE_MOVE,0,0};
			GFC_AddCameraAngleRot(&angle,scene_wk->Data.CameraPtr);
		}
		//フォグ設定
		{
			NNS_G3dGeFlushBuffer();
			G3X_SetFog( FALSE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x8000, 0 );
		}
		//木漏れ日OFF
		{
			G2_BlendNone();
			GF_BGL_VisibleSet(TOWN_FRAME, VISIBLE_OFF);
		}
	}else {
		//下に平行移動（よきところで、カメラ引く）
		if (inCounter>=FM_CAM_ZOOM_OUT_START){
			u16 persp_way;
			persp_way = GFC_GetCameraPerspWay(scene_wk->Data.CameraPtr);
		#if 0	//引く速度をちょっと遅くする
			if (persp_way+32<0x4000){
				GFC_AddCameraPerspWay(32,scene_wk->Data.CameraPtr);
			}
		#else
			if (persp_way+24<0x4000){
				GFC_AddCameraPerspWay(24,scene_wk->Data.CameraPtr);
			}
		#endif
		}
		move.x += -0x2000;
		GFC_ShiftCamera(&move, scene_wk->Data.CameraPtr);
	}
}

static void Scene2_MapChange(OPENING_DEMO_WORK *wk, SCENE2_WORK *scene_wk, int inCounter)
{
	BOOL ret;
	
	switch(inCounter){
	case FM_CAMEA1_END:
		WIPE_SYS_Start(	WIPE_PATTERN_M, WIPE_TYPE_FADEOUT,
					WIPE_TYPE_FADEOUT,0x0000,SCENE2_MAPCHANGE_SYNC,1,HEAPID_OP_DEMO );
		break;
	case FM_MAP2_LOAD:
		//この時点でWIPEが終了していなかったらWIPEの速度をもっとあげるか、
		//FM_MAP2_LOADの数値を後ろに下げる
		GF_ASSERT(WIPE_SYS_EndCheck() == TRUE);
		
		//マップを解放して、次のを読み込む
		Scene2_Load3D_Exit(wk);
		Scene2_Load3D_Div(scene_wk, 1);
		//Scene2_Load3D(scene_wk, 1);
		break;
	case FM_MAP2_LOAD_1:
		Scene2_Load3D_Div(scene_wk, 1);
		break;
	case FM_MAP2_LOAD_2:
		Scene2_Load3D_Div(scene_wk, 1);
		break;
	case FM_MAP2_LOAD_3:
		ret = Scene2_Load3D_Div(scene_wk, 1);
		GF_ASSERT(ret == TRUE);
		WIPE_SYS_Start(	WIPE_PATTERN_M, WIPE_TYPE_FADEIN,
					WIPE_TYPE_FADEIN,0x0000,SCENE2_MAPCHANGE_SYNC,1,HEAPID_OP_DEMO );
		break;
	case FM_CAMEA2_END:
		WIPE_SYS_Start(	WIPE_PATTERN_M, WIPE_TYPE_FADEOUT,
					WIPE_TYPE_FADEOUT,0x0000,SCENE2_MAPCHANGE_SYNC,1,HEAPID_OP_DEMO );
		break;
	case FM_MAP3_LOAD:
		//この時点でWIPEが終了していなかったらWIPEの速度をもっとあげるか、
		//FM_MAP2_LOADの数値を後ろに下げる
		GF_ASSERT(WIPE_SYS_EndCheck() == TRUE);
		
		//マップを解放して、次のを読み込む
		Scene2_Load3D_Exit(wk);
		Scene2_Load3D(scene_wk, 2);

		WIPE_SYS_Start(	WIPE_PATTERN_M, WIPE_TYPE_FADEIN,
					WIPE_TYPE_FADEIN,0x0000,SCENE2_MAPCHANGE_SYNC,1,HEAPID_OP_DEMO );
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//シーン3（テンガンザン）
static void Scene3_Init(OPENING_DEMO_WORK * wk);
static void Scene3_Load(SCENE3_WORK *scene_wk);
static BOOL Scene3_Main(SCENE3_WORK *scene_wk, const int inCounter);
static void Scene3_End(OPENING_DEMO_WORK * wk);

static BOOL MtTenganScene(OPENING_DEMO_WORK * wk)
{
	u8 *seq = &(wk->SceneWork3.Seq);
	if (wk->SkipFlg){
		(*seq) = 3;		//スキップ実行
	}
	
	switch(*seq){
	case 0:		//初期化
		Scene3_Init(wk);
		(*seq)++;
		break;
	case 1:			//グラフィックロード
		Scene3_Load(&wk->SceneWork3);
		(*seq)++;
		break;
	case 2:			//メイン
		if ( Scene3_Main( &wk->SceneWork3, wk->FrameCounter ) ){
			(*seq)++;
		}
		break;
	case 3:			//終了
		Scene3_End(wk);
		return TRUE;
	}
	return FALSE;
}

static void Scene3_Init(OPENING_DEMO_WORK * wk)
{
	int obj_work_size;
	int ssm_size;
	OpDemo_Scene3_VramBankSet();
	OpDemo_Scene3_2DBgSet(wk);

	wk->SceneWork3.bgl = wk->bgl;

	//マスター輝度を０にする
	WIPE_SetBrightness( WIPE_DISP_MAIN,WIPE_FADE_WHITE );
	WIPE_SetBrightness( WIPE_DISP_SUB,WIPE_FADE_WHITE );
	//カラー特殊効果無効
	G2_BlendNone();

	wk->SceneWork3.InitOK = 1;	//init通過
}

static void Scene3_Load(SCENE3_WORK *scene_wk)
{
/**
	const int pal_arc_idx[BTL_PAL_MAX] = {
		NARC_op_demo_op_demoBG2_btl1_NCLR,
		NARC_op_demo_op_demoBG2_btl2_NCLR,
		NARC_op_demo_op_demoBG2_btl3_NCLR,
		NARC_op_demo_op_demoBG2_btl4_NCLR
	};
*/	

	//パレット
	ArcUtil_PalSet( ARC_OP_DEMO, NARC_op_demo_op_akagi_NCLR,
						PALTYPE_MAIN_BG, 0, 0, HEAPID_OP_DEMO );
	
	//--アカギデータロード
	//キャラ
	ArcUtil_BgCharSet(ARC_OP_DEMO, NARC_op_demo_op_akagi_NCGR,
						scene_wk->bgl, MT_TENGAN_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
	//スクリーン
	ArcUtil_ScrnSet(ARC_OP_DEMO, NARC_op_demo_op_akagi_NSCR,
						scene_wk->bgl, MT_TENGAN_FRAME, 0, 0, FALSE, HEAPID_OP_DEMO);
	
	//--初期ＢＧ設定
	GF_Disp_GX_VisibleControlInit();
	GF_Disp_GX_VisibleControlDirect( 0 );
	GF_Disp_GXS_VisibleControlDirect( 0 );

	scene_wk->akagi_scr_y = -24 * FX32_ONE;
	GF_BGL_ScrollSet( scene_wk->bgl, MT_TENGAN_FRAME, GF_BGL_SCROLL_X_SET, 0);
	GF_BGL_ScrollSet( scene_wk->bgl, MT_TENGAN_FRAME, GF_BGL_SCROLL_Y_SET, 
		scene_wk->akagi_scr_y / FX32_ONE);
	
	scene_wk->LoadOK = 1;		//load通過
}

static BOOL Scene3_Main(SCENE3_WORK *scene_wk, const int inCounter)
{
	u8 *seq = &(scene_wk->SceneSeq);
	
	switch(*seq){
	case 0:	//ゆっくりブラックイン
		if (inCounter>=FM_TENGAN_IN){
			WIPE_SYS_Start(	WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN,
								WIPE_TYPE_FADEIN,0x7fff,30,1,HEAPID_OP_DEMO );
			(*seq)++;
		}
		break;
	case 1:
		if(WIPE_SYS_EndCheck()){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2 , VISIBLE_ON );
			WIPE_SYS_Start(	WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN,
								WIPE_TYPE_FADEIN,0x0000,SCENE3_START_IN_SYNC,1,HEAPID_OP_DEMO );
			(*seq)++;
		}
		break;
	case 2:
		if ( WIPE_SYS_EndCheck() ){
			(*seq)++;
		}
		break;
	case 3://よきところで両面ホワイトアウト
		//ＢＧスクロール・拡縮メイン
		if(inCounter >= FM_TENGAN_OUT){
			WIPE_SYS_Start(	WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT,
							WIPE_TYPE_FADEOUT,0x0000,8,1,HEAPID_OP_DEMO );
//			WIPE_SetMstBrightness( WIPE_DISP_MAIN, -16 );
			(*seq)++;
		}
		
	#if 0	//テンガンザンの拡大なくした
		if (inCounter>=FM_MT_AFFINE){
			Scene3_Affine(scene_wk);
		}
		
		if ( (inCounter>=FM_TENGAN_OUT) ){
			WIPE_SYS_Start(	WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT,
							WIPE_TYPE_FADEOUT,0x7fff,TENGAN_WH_OUT_SYNC,1,HEAPID_OP_DEMO );
			Scene3_Affine2(scene_wk);
			(*seq)++;
		}
	#endif
		break;
	case 4:	//ホワイトアウト待ち
//		Scene3_Affine(scene_wk);
	//	Scene3_Affine2(scene_wk);
		if( WIPE_SYS_EndCheck() ){
			return TRUE;
		}
	}

	scene_wk->akagi_scr_y += 0x400;
	GF_BGL_ScrollSet(scene_wk->bgl, MT_TENGAN_FRAME, GF_BGL_SCROLL_Y_SET, 
		scene_wk->akagi_scr_y / FX32_ONE);

	return FALSE;
}

static void Scene3_End(OPENING_DEMO_WORK * wk)
{
	if (wk->SceneWork3.InitOK){
		//bgl解放
		GF_BGL_BGControlExit( wk->bgl, MT_TENGAN_FRAME );
		sys_FreeMemoryEz( wk->bgl );
		wk->SceneWork3.InitOK = 0;
	}
	if (wk->SceneWork3.LoadOK){		//load通過
		//パレット解放
		wk->SceneWork3.LoadOK = 0;
	}
}


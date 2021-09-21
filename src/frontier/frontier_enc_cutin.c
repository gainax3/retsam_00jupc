//==============================================================================
/**
 * @file	frontier_enc_cutin.c
 * @brief	フロンティアブレーン戦のカットインエフェクト
 * @author	matsuda
 * @date	2008.01.28(月)
 */
//==============================================================================
#include "common.h"
#include "system/pm_overlay.h"
#include "system/procsys.h"
#include "system/pmfprint.h"
#include "system/palanm.h"
#include "system/wipe.h"
#include "system/snd_tool.h"
#include "system/bmp_list.h"
#include "system/font_arc.h"
#include "system/window.h"
//#include "script.h"
#include "..\fielddata\script\connect_def.h"
#include "system/clact_tool.h"
#include "communication/wm_icon.h"

#include "system/msgdata.h"
#include "msgdata/msg.naix"
#include "msgdata/msg_debug_matsu.h"

#include "frontier_types.h"
#include "fss_scene.h"
#include "frontier_main.h"
#include "frontier_map.h"
#include "frontier_tcb_pri.h"
#include "frontier_particle.h"
#include "frontier_actor.h"

#include "graphic/frontier_bg_def.h"

#include "system/fontoam.h"
#include "msgdata/msg_trname.h"
#include "field/field_encounteffect.naix"
#include "frontier_id.h"
#include "frontier_tcb_pri.h"
#include "system/fontproc.h"
#include "system/lib_pack.h"
#include "frontier_enc_cutin.h"
#include "system/brightness.h"


//==============================================================================
//	定数定義
//==============================================================================
// OAMデータ
// アクターデータ数
#define ENCOUNT_SPTR_BRAIN_CELL_NUM		( 8 )	// ワーク数
#define ENCOUNT_SPTR_BRAIN_CELL_RES_NUM	( 3 )	// リソース数
#define ENCOUNT_SPTR_BRAIN_CELL_CONT_ID	( 600000 )
#define ENCOUNT_SPTR_BRAIN_LEADER_DRAW_BOTTOM	( 30 )	// 下はこのサイズまで表示する
#define ENCOUNT_SPTR_BRAIN_LEADER_COLOR_FADE		( GX_RGB( 0,0,0 ) )
#define ENCOUNT_SPTR_BRAIN_LEADER_COLOR_FADE_EVY	( 14 )
#define ENCOUNT_SPTR_BRAIN_LEADER_CHAR_OFS_CX	(0)	// キャラクタデータ書き出す時の、Xキャラクタオフセット値

#define ENCOUNT_SPTR_BRAIN_ZIGUZAGU_IN_WND ( GX_WND_PLANEMASK_BG0|GX_WND_PLANEMASK_BG1|GX_WND_PLANEMASK_BG2|GX_WND_PLANEMASK_BG3|GX_WND_PLANEMASK_OBJ)				// ジグザグ動さウィンドウ中
#define ENCOUNT_SPTR_BRAIN_ZIGUZAGU_OUT_WND ( GX_WND_PLANEMASK_BG0|GX_WND_PLANEMASK_BG1|GX_WND_PLANEMASK_BG2|GX_WND_PLANEMASK_OBJ)				// ジグザグ動さウィンドウ外
enum{	//VSOAM
	ENCOUNT_SPTR_BRAIN_VSOAM_ANM0,
	ENCOUNT_SPTR_BRAIN_VSOAM_ANM1,
	ENCOUNT_SPTR_BRAIN_VSOAM_ANM2,
	ENCOUNT_SPTR_BRAIN_VSOAM,
	ENCOUNT_SPTR_BRAIN_VSOAM_NUM,
};
#define ENCOUNT_SPTR_BRAIN_TRNAME_X		( 0 )	// トレーナー名Xキャラ座標
#define ENCOUNT_SPTR_BRAIN_TRNAME_Y		( 11 )	// トレーナー名Yキャラ座標
#define ENCOUNT_SPTR_BRAIN_TRNAME_SIZX	( 16 )	// トレーナー名BMPXキャラサイズ
#define ENCOUNT_SPTR_BRAIN_TRNAME_SIZY	( 2 )	// トレーナー名BMPYキャラサイズ
#define ENCOUNT_SPTR_BRAIN_TRNAME_CGX		( FFD_FREE_CGX )
#define ENCOUNT_SPTR_BRAIN_TRNAME_PAL		( FFD_MESFONT_PAL )
#define ENCOUNT_SPTR_BRAIN_TRNAME_COL		( GF_PRINTCOLOR_MAKE( 1, 2, 0 ) )

// 動さパラメータ	（触ってよい部分）
#define ENCOUNT_SPTR_BRAIN_OAM_START_X	( 272*FX32_ONE )	// OAM動さ開始X
#define ENCOUNT_SPTR_BRAIN_OAM_X_SS		( -64*FX32_ONE )	// 初期速度
#define ENCOUNT_SPTR_BRAIN_OAM_START_Y	( 66*FX32_ONE )		// OAMY座標
#define ENCOUNT_SPTR_BRAIN_OAM_MOVE_SYNC	( 4 )				// OAM動さシンク
#define ENCOUNT_SPTR_BRAIN_TRNAME_OFS_X	( -92 )				// OAMの位置から見てTRNAMEをどの位置に表示するか
#define ENCOUNT_SPTR_BRAIN_ZIGUZAGU_DOT	( 8 )				// ジグザグ区切りYドット数
#define ENCOUNT_SPTR_BRAIN_ZIGUZAGU_WIDTH	( 16 )				// ジグザグ幅
#define ENCOUNT_SPTR_BRAIN_ZIGUZAGU_SYNC	( 6 )				// ジグザグ動さシンク
#define ENCOUNT_SPTR_BRAIN_BG_CY			( 68 )	// BGの中心
#define ENCOUNT_SPTR_BRAIN_BG_AUTOMOVE_X	( 30 )	// AUTO動さスピード
#define ENCOUNT_SPTR_BRAIN_BG_AFTER_WAIT	( 10 )				// BG動さ後ウエイト
#define	ENCOUNT_SPTR_BRAIN_FLASHOUT_SYNC	( 3 )				// フラッシュアウトシンク数
#define	ENCOUNT_SPTR_BRAIN_FLASHIN_SYNC	( 3 )				// フラッシュインシンク数
#define ENCOUNT_SPTR_BRAIN_FLASH_AFTER_WAIT	( 26 )			// フラッシュ後のウエイト
#define ENCOUNT_SPTR_BRAIN_SCALE_S			( FX32_CONST(1.0f) )	// 拡大開始
#define ENCOUNT_SPTR_BRAIN_SCALE_E			( FX32_CONST(1.0f) )	// 拡大開始
#define ENCOUNT_SPTR_BRAIN_FADEOUT_SYNC		( 15 )			// フェードアウトシンク
#define ENCOUNT_SPTR_BRAIN_VSMARK_X		( FX32_CONST( 72 ) )	// VSMARKの位置
#define ENCOUNT_SPTR_BRAIN_VSMARK_Y		( FX32_CONST( 82 ) )	// VSMARKの位置
#define ENCOUNT_SPTR_BRAIN_VSANMSYNC		( 6 )	// １つの拡縮アニメのシンク数
#define ENCOUNT_SPTR_BRAIN_VSANMTIMING	( 3 )	//	次を出すタイミング
#define ENCOUNT_SPTR_BRAIN_3DAREA_BRIGHTNESS	( -14 )// 3D面を暗くする値

//--------------------------------------------------------------
//	新規定義
//--------------------------------------------------------------
///カットインBGのパレット展開位置
#define ENCOUNT_CUTIN_BG_PALNO		( FFD_SYSFONT_PAL )
///フォントOAMの使用数(トレーナー名の表示のみに使用)
#define ENCOUNT_FONT_OAM_MAX		(4)
///ジグザグBGのパレット展開位置
#define ZIGZAG_BG_PALNO				(FFD_MENUFRAME_PAL)
///ジグザグBGのパレットアニメ本数
#define ZIGZAG_BG_PAL_ANM_NUM		(8)
///ジグザグBGのパレットアニメのウェイト
#define ZIGZAG_BGPALANM_WAIT		(0)

enum{
	WINEFF_OPEN,
	WINEFF_CLOSE,
};

///ウィンドウを開く最大ドット数
#define WNDOPEN_EFF_OPEN_Y_DOT		(8*8 + 4)
///ウィンドウを開く速度(下位8ビット小数)
#define WNDOPEN_EFF_ADD_Y			(0x0800)
///ウィンドウのセンター座標
#define WNDOPEN_EFF_CENTER_Y		(10*8)

///顔登場
enum{
	FACE_IN_END_X = 256 - 6 * 8,
	FACE_IN_START_X = FACE_IN_END_X + 6 * 8,
	FACE_IN_START_Y = 10 * 8,
	FACE_IN_SPEED_X = 0x0f00,	//インしてくる時の速度(下位8ビット小数)
};

enum{
	FONTOAM_LEFT,		///<X左端座標
	FONTOAM_CENTER,		///<X中心座標
};


///フォントOAM作成時、外側でBMP指定する場合に使用
typedef struct{
	GF_BGL_BMPWIN bmpwin;
	u16 char_len;
	u16 font_len;
}FONT_EX_BMPWIN;

///フォントアクターワーク
typedef struct{
	FONTOAM_OBJ_PTR fontoam;
	CHAR_MANAGER_ALLOCDATA cma;
	u16 font_len;
}FONT_ACTOR;

///ウィンドウ機能の開け閉じエフェクト用ワーク
typedef struct{
	s32 y1;						///<ウィンドウ座標Y(上側)：下位8ビット小数
	s32 y2;						///<ウィンドウ座標Y(下側)：下位8ビット小数
	
	u8 end_flag;				///<終了後TRUEがセットされる
	u8 seq;						///<シーケンス番号

	u8 wnd0_x1;						///<ウィンドウ0のX1座標
	u8 wnd0_y1;
	u8 wnd0_x2;
	u8 wnd0_y2;

	u8 wnd1_x1;
	u8 wnd1_y1;
	u8 wnd1_x2;
	u8 wnd1_y2;
}WNDOPEN_EFF_WORK;

///顔スライドイン制御ワーク
typedef struct{
	int x;
	int seq;
}FACE_MOVE_WORK;

//-------------------------------------
//
//	エンカウントエフェクトワーク
//
//=====================================
typedef struct _ENCOUNT_EFFECT_WORK{
	int seq;
	int wait;
	int count;
	void* work;		// 各自、自由に使用
	GF_BGL_INI *bgl;
	CATS_SYS_PTR csp;
	CATS_RES_PTR crp;
	PALETTE_FADE_PTR pfd;
	u16* end;		// 終了ﾁｪｯｸ用
	ARCHANDLE* p_handle;	// アーカイブハンドル
	s16 map_x;
	s16 map_y;
	
	u8 effect_no;			///<エフェクト番号
	FONTOAM_SYS_PTR fontoam_sys;		///<フォントOAMシステムへのポインタ

	TCB_PTR tcb_vwait;		///<Vwait更新TCB
	
	//BGパレットアニメ
	int palanm_wait;		///<パレットアニメのウェイト
	int palanm_no;			///<パレットアニメ位置
	u16 palbuf[ZIGZAG_BG_PAL_ANM_NUM * 16];	///パレットアニメバッファ
	TCB_PTR tcb_bgpalanm;	///<パレットアニメ更新TCB

	//ウィンドウ
	WNDOPEN_EFF_WORK wineff;
	
	//顔スライドイン
	FACE_MOVE_WORK face_move_work;
	int face_palno;
	u32 obj_palbit;
}ENCOUNT_EFFECT_WORK;

//-------------------------------------
//	汎用動作
//=====================================
typedef struct {
	fx32 x;
	fx32 s_x;
	fx32 dis_x;
	int count;
	int count_max;
} ENC_MOVE_WORK_FX;

//-------------------------------------
///	VSアニメ
//=====================================
typedef struct {
	s16 timing;
	s16 movenum;
	CATS_ACT_PTR cap_vs[ ENCOUNT_SPTR_BRAIN_VSOAM_NUM ];
	ENC_MOVE_WORK_FX vsscale[ ENCOUNT_SPTR_BRAIN_VSOAM_NUM ];
} ENCOUNT_SPTR_BRAIN_VSANM;
//
//-------------------------------------
//	ジムリーダー
//=====================================
typedef struct {
	ENC_MOVE_WORK_FX scale;
	ENCOUNT_SPTR_BRAIN_VSANM vsanm;
	s32 wait;
	
	//新規定義
	FONT_ACTOR fontoam_trainer_name;	///フォントOAM(トレーナー名)
	CATS_ACT_PTR cap_face;
} ENCOUNT_SPTR_BRAIN;


// それぞれで変わるところ
typedef struct {
	u32	trno;
	u8	nclr;
	u8	ncgr;
	u8	ncer;
	u8	nanr;
	u8	bg_nclr;
	u8	bg_ncgr;
	u8	bg_nscr;
	u8	pad;
} ENCOUNT_SPTR_BRAIN_PARAM;

// それぞれの定数
static const ENCOUNT_SPTR_BRAIN_PARAM ENCOUNT_SPTR_BrainDef[] = {
	{	//FRONTIER_NO_TOWER
		TR_KUROTUGU_01,			// リーダータイプ
		NARC_field_encounteffect_trpl_fbface05_NCLR,
		NARC_field_encounteffect_trpl_fbface05_NCGR,
		NARC_field_encounteffect_trpl_fbface05_NCER,
		NARC_field_encounteffect_trpl_fbface05_NANR,
		NARC_field_encounteffect_cutin_fbrain05_NCLR,
		NARC_field_encounteffect_cutin_fbrain05_NCGR,
		NARC_field_encounteffect_cutin_fbrain05_NSCR,
		0,
	},
	{	//FRONTIER_NO_FACTORY_LV50
		TR_FBOSS2_01,			// リーダータイプ
		NARC_field_encounteffect_trpl_fbface02_NCLR,
		NARC_field_encounteffect_trpl_fbface02_NCGR,
		NARC_field_encounteffect_trpl_fbface02_NCER,
		NARC_field_encounteffect_trpl_fbface02_NANR,
		NARC_field_encounteffect_cutin_fbrain02_NCLR,
		NARC_field_encounteffect_cutin_fbrain02_NCGR,
		NARC_field_encounteffect_cutin_fbrain02_NSCR,
		0,
	},
	{	//FRONTIER_NO_FACTORY_LV100
		TR_FBOSS2_01,			// リーダータイプ
		NARC_field_encounteffect_trpl_fbface02_NCLR,
		NARC_field_encounteffect_trpl_fbface02_NCGR,
		NARC_field_encounteffect_trpl_fbface02_NCER,
		NARC_field_encounteffect_trpl_fbface02_NANR,
		NARC_field_encounteffect_cutin_fbrain02_NCLR,
		NARC_field_encounteffect_cutin_fbrain02_NCGR,
		NARC_field_encounteffect_cutin_fbrain02_NSCR,
		0,
	},
	{	//FRONTIER_NO_CASTLE
		TR_FBOSS4_01,			// リーダータイプ
		NARC_field_encounteffect_trpl_fbface01_NCLR,
		NARC_field_encounteffect_trpl_fbface01_NCGR,
		NARC_field_encounteffect_trpl_fbface01_NCER,
		NARC_field_encounteffect_trpl_fbface01_NANR,
		NARC_field_encounteffect_cutin_fbrain01_NCLR,
		NARC_field_encounteffect_cutin_fbrain01_NCGR,
		NARC_field_encounteffect_cutin_fbrain01_NSCR,
		0,
	},
	{	//FRONTIER_NO_STAGE
		TR_FBOSS1_01,			// リーダータイプ
		NARC_field_encounteffect_trpl_fbface04_NCLR,
		NARC_field_encounteffect_trpl_fbface04_NCGR,
		NARC_field_encounteffect_trpl_fbface04_NCER,
		NARC_field_encounteffect_trpl_fbface04_NANR,
		NARC_field_encounteffect_cutin_fbrain04_NCLR,
		NARC_field_encounteffect_cutin_fbrain04_NCGR,
		NARC_field_encounteffect_cutin_fbrain04_NSCR,
		0,
	},
	{	//FRONTIER_NO_ROULETTE
		TR_FBOSS3_01,			// リーダータイプ
		NARC_field_encounteffect_trpl_fbface03_NCLR,
		NARC_field_encounteffect_trpl_fbface03_NCGR,
		NARC_field_encounteffect_trpl_fbface03_NCER,
		NARC_field_encounteffect_trpl_fbface03_NANR,
		NARC_field_encounteffect_cutin_fbrain03_NCLR,
		NARC_field_encounteffect_cutin_fbrain03_NCGR,
		NARC_field_encounteffect_cutin_fbrain03_NSCR,
		0,
	},
};

///アクターソフトプライオリティ
enum{
	CUTIN_SOFTPRI_VS = 10,
	CUTIN_SOFTPRI_NAME,
	CUTIN_SOFTPRI_FACE,
};

///アクターBGプライオリティ
#define CUTIN_BGPRI_VS			(0)
#define CUTIN_BGPRI_NAME		(0)
#define CUTIN_BGPRI_FACE		(0)

///顔 アクターヘッダ
static const TCATS_OBJECT_ADD_PARAM_S FaceObjParam = {
	0,0,0,		//X,Y,Z
	0, CUTIN_SOFTPRI_FACE, 0,		//アニメ番号、優先順位、パレット番号
	NNS_G2D_VRAM_TYPE_2DMAIN,		//描画エリア
	{	//使用リソースIDテーブル
		FR_CHARID_CUTIN_FACE,			//キャラ
		FR_PLTTID_CUTIN_FACE,			//パレット
		FR_CELLID_CUTIN_FACE,			//セル
		FR_CELLANMID_CUTIN_FACE,		//セルアニメ
		CLACT_U_HEADER_DATA_NONE,		//マルチセル
		CLACT_U_HEADER_DATA_NONE,		//マルチセルアニメ
	},
	CUTIN_BGPRI_FACE,			//BGプライオリティ
	0,			//Vram転送フラグ
};

///VS アクターヘッダ
static const TCATS_OBJECT_ADD_PARAM_S VsObjParam = {
	0,0,0,		//X,Y,Z
	0, CUTIN_SOFTPRI_VS, 0,		//アニメ番号、優先順位、パレット番号
	NNS_G2D_VRAM_TYPE_2DMAIN,		//描画エリア
	{	//使用リソースIDテーブル
		FR_CHARID_CUTIN_VS,			//キャラ
		FR_PLTTID_CUTIN_VS,			//パレット
		FR_CELLID_CUTIN_VS,			//セル
		FR_CELLANMID_CUTIN_VS,		//セルアニメ
		CLACT_U_HEADER_DATA_NONE,		//マルチセル
		CLACT_U_HEADER_DATA_NONE,		//マルチセルアニメ
	},
	CUTIN_BGPRI_VS,			//BGプライオリティ
	0,			//Vram転送フラグ
};


//==============================================================================
//	プロトタイプ宣言
//==============================================================================
static void ENC_End( ENCOUNT_EFFECT_WORK* eew, TCB_PTR tcb );
static void VWait_EncountUpdateTCB(TCB_PTR tcb, void *work);
static void CutinEffect_MainTCB(TCB_PTR tcb, void* work);
static BOOL EncountEffect_FBrainMain( ENCOUNT_EFFECT_WORK *eew, u32 heapID, const ENCOUNT_SPTR_BRAIN_PARAM* cp_def );
static void Sub_FontOamCreate(ENCOUNT_EFFECT_WORK *eew, FONT_ACTOR *font_actor, const STRBUF *str, 
	FONT_TYPE font_type, GF_PRINTCOLOR color, int pal_offset, int pal_id, 
	int x, int y, int pos_center, FONT_EX_BMPWIN *ex_bmpwin);
static void Sub_FontOamDelete(FONT_ACTOR *font_actor);
static void EncountEffect_SpTr_VsAnmInit(ENCOUNT_EFFECT_WORK *eew, ENCOUNT_SPTR_BRAIN_VSANM* p_wk, fx32 x, fx32 y, u32 heapID );
static void EncountEffect_SpTr_VsAnmExit( ENCOUNT_SPTR_BRAIN_VSANM* p_wk );
static BOOL EncountEffect_SpTr_VsAnmMain( ENCOUNT_SPTR_BRAIN_VSANM* p_wk );
static void ENC_MoveReqFx( ENC_MOVE_WORK_FX* p_work, fx32 s_x, fx32 e_x, int count_max );
static BOOL ENC_MoveMainFx( ENC_MOVE_WORK_FX* p_work );
static VecFx32 ENC_MakeVec( fx32 x, fx32 y, fx32 z );
static void ZigzagBG_Load(ENCOUNT_EFFECT_WORK *eew, const ENCOUNT_SPTR_BRAIN_PARAM *cp_def);
static void ZigzagPalAnmTCB(TCB_PTR tcb, void *work);
static void WndOpenEffStart(ENCOUNT_EFFECT_WORK *eew, int open_close);
static void WndEffOpenTask(TCB_PTR tcb, void *work);
static void WndEffCloseTask(TCB_PTR tcb, void *work);
static BOOL FaceIn_Main(ENCOUNT_EFFECT_WORK *eew, ENCOUNT_SPTR_BRAIN* task_w, FACE_MOVE_WORK *fmw);
static void FontLenGet(const STRBUF *str, FONT_TYPE font_type, int *ret_dot_len, int *ret_char_len);



//----------------------------------------------------------------------------
/**
 *
 *@brief	エンカウントエフェクト開始
 *
 *@param	No		エンカウントエフェクトNO(FRONTIER_NO_???)
 *@param	*fsw	フィールドシステムワークのポインタ
 *@param	end		終了チェック用　TRUE：終了した 
 *
 *@return	none
 *
 *
 */
//-----------------------------------------------------------------------------
void BrainCutin_EffectStart(int No, GF_BGL_INI *bgl, CATS_SYS_PTR csp, CATS_RES_PTR crp, PALETTE_FADE_PTR pfd, u16* end, s16 map_x, s16 map_y )
{
	TCB_PTR tcb;
	ENCOUNT_EFFECT_WORK *eew;
	
	tcb = PMDS_taskAdd(CutinEffect_MainTCB, sizeof(ENCOUNT_EFFECT_WORK), TCBPRI_ENC_CUTIN, HEAPID_FRONTIERMAP);
	eew = TCB_GetWork(tcb);
	eew->bgl = bgl;
	eew->csp = csp;
	eew->crp = crp;
	eew->pfd = pfd;
	eew->end = end;		// 終了フラグ設定
	eew->map_x = map_x;
	eew->map_y = map_y;
	eew->p_handle = ArchiveDataHandleOpen( ARC_ENCOUNTEFFECT, HEAPID_FRONTIERMAP );
	if(eew->end != NULL){
		*(eew->end) = FALSE;
	}
	eew->effect_no = No - 1;	//0originにする

	//VRAMクリア	スクリーンのクリアコードが0ではない為、どこがクリアコードになっていても
	//				問題ないように一度全てのキャラクタ領域をクリアしておく
	MI_CpuClear32(GF_BGL_CgxGet(FRMAP_FRAME_WIN), 0x8000);

	GF_BGL_ScrollReq(bgl, FRMAP_FRAME_WIN, GF_BGL_SCROLL_X_SET, 0);
	GF_BGL_ScrollReq(bgl, FRMAP_FRAME_WIN, GF_BGL_SCROLL_Y_SET, 0);

	// BG設定
	ZigzagBG_Load(eew, &ENCOUNT_SPTR_BrainDef[eew->effect_no]);
	//Vwait更新タスク生成
	eew->tcb_vwait = VWaitTCB_Add(VWait_EncountUpdateTCB, eew, 1);
}

//----------------------------------------------------------------------------
/**
 * encount_effect_sub以外の外部非公開
 *	@brief	ワークとタスク破棄関数
 *
 *	@param	eew エンカウントエフェクトワーク
 */
//-----------------------------------------------------------------------------
static void ENC_End( ENCOUNT_EFFECT_WORK* eew, TCB_PTR tcb )
{
	TCB_Delete(eew->tcb_bgpalanm);
	TCB_Delete(eew->tcb_vwait);
	
	ArchiveDataHandleClose( eew->p_handle );
	sys_FreeMemory(HEAPID_FRONTIERMAP, eew->work);
	PMDS_taskDel(tcb);	//タスク終了
}

static void VWait_EncountUpdateTCB(TCB_PTR tcb, void *work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	
	//ウィンドウ座標
	G2_SetWnd0Position(eew->wineff.wnd0_x1, eew->wineff.wnd0_y1, 
		eew->wineff.wnd0_x2, eew->wineff.wnd0_y2);
	G2_SetWnd1Position(eew->wineff.wnd1_x1, eew->wineff.wnd1_y1, 
		eew->wineff.wnd1_x2, eew->wineff.wnd1_y2);
}

//----------------------------------------------------------------------------
/**
 *	@brief		各ジム
 */
//-----------------------------------------------------------------------------
static void CutinEffect_MainTCB(TCB_PTR tcb, void* work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	BOOL result;
	result = EncountEffect_FBrainMain( eew, HEAPID_FRONTIERMAP, &ENCOUNT_SPTR_BrainDef[eew->effect_no]);
	if( result == TRUE ){
		ENC_End( eew, tcb );
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief	フロンティアブレーン専用エンカウントエフェクト
 *
 *	@param	eew		エフェクトワーク
 *	@param	heapID	ヒープID
 *	@param	cp_def	デファイン定義
 *
 *	@retval	TRUE		終了
 *	@retval	FALSE		途中
 */
//-----------------------------------------------------------------------------
static BOOL EncountEffect_FBrainMain( ENCOUNT_EFFECT_WORK *eew, u32 heapID, const ENCOUNT_SPTR_BRAIN_PARAM* cp_def )
{
	ENCOUNT_SPTR_BRAIN* task_w = eew->work;
	BOOL result;
	const VecFx32* cp_matrix;
	VecFx32 matrix;
	VecFx32 scale;
	int bottom_y;
	int i, palno = 0;
	STRBUF* p_str;
	enum{
		ENCOUNT_SPTR_BRAIN_START,
		ENCOUNT_SPTR_BRAIN_FLASH_INI,
		ENCOUNT_SPTR_BRAIN_FLASH_WAIT,
		ENCOUNT_SPTR_BRAIN_FLASH_INI_2,
		ENCOUNT_SPTR_BRAIN_FLASH_WAIT_2,
		ENCOUNT_SPTR_BRAIN_BG_IN,
		ENCOUNT_SPTR_BRAIN_BG_INWAIT,
		ENCOUNT_SPTR_BRAIN_VS_ANMWAIT,
		ENCOUNT_SPTR_BRAIN_OAM_IN,
		ENCOUNT_SPTR_BRAIN_OAM_INWAIT,
		ENCOUNT_SPTR_BRAIN_OAM_FLASH_OUT,
		ENCOUNT_SPTR_BRAIN_OAM_FLASH_OUT_START,
		ENCOUNT_SPTR_BRAIN_OAM_FLASH_OUT_WAIT,
		ENCOUNT_SPTR_BRAIN_OAM_FLASH_OUT_IN_START,
		ENCOUNT_SPTR_BRAIN_OAM_FLASH_OUT_IN_WAIT,
		ENCOUNT_SPTR_BRAIN_WAIT,
		ENCOUNT_SPTR_BRAIN_WIPE,
		ENCOUNT_SPTR_BRAIN_WIPEWAIT,
		ENCOUNT_SPTR_BRAIN_END,
	};

	switch( eew->seq ){
	case ENCOUNT_SPTR_BRAIN_START:
		eew->work = sys_AllocMemory( heapID, sizeof(ENCOUNT_SPTR_BRAIN) );	
		memset( eew->work, 0, sizeof(ENCOUNT_SPTR_BRAIN) );
		task_w = eew->work;

		// フォントOAMパレット読み込み	
		palno = CATS_LoadResourcePlttWorkArcH(eew->pfd, FADE_MAIN_OBJ, eew->csp, eew->crp, 
			eew->p_handle, NARC_field_encounteffect_cutin_gym_font_NCLR, 0, 1, 
			NNS_G2D_VRAM_TYPE_2DMAIN, FR_PLTTID_CUTIN_NAME);
		OS_TPrintf("fontoam palno = %d\n", palno);
		eew->obj_palbit |= 1 << palno;
		
		//トレーナー名表示の為のFONTOAMを作成
		eew->fontoam_sys = FONTOAM_SysInit(ENCOUNT_FONT_OAM_MAX, HEAPID_FRONTIERMAP);
		{
			MSGDATA_MANAGER *man;
			STRBUF *name;
			
			man = MSGMAN_Create(MSGMAN_TYPE_DIRECT, ARC_MSG, NARC_msg_trname_dat, heapID);
			name = MSGMAN_AllocString(man, cp_def->trno);
			Sub_FontOamCreate(eew, &task_w->fontoam_trainer_name, name, FONT_SYSTEM,
				ENCOUNT_SPTR_BRAIN_TRNAME_COL, 0, 
				FR_PLTTID_CUTIN_NAME, FACE_IN_END_X + ENCOUNT_SPTR_BRAIN_TRNAME_OFS_X + eew->map_x,
				ENCOUNT_SPTR_BRAIN_TRNAME_Y * 8 + eew->map_y, FONTOAM_LEFT, NULL);
			FONTOAM_SetDrawFlag(task_w->fontoam_trainer_name.fontoam, FALSE);	//非表示
			STRBUF_Delete(name);
			MSGMAN_Delete(man);
		}
		
		//リソースロード
		{
			//-- 顔 --//
			eew->face_palno = CATS_LoadResourcePlttWorkArcH(
				eew->pfd, FADE_MAIN_OBJ, eew->csp, eew->crp, 
				eew->p_handle, cp_def->nclr, 0, 1, 
				NNS_G2D_VRAM_TYPE_2DMAIN, FR_PLTTID_CUTIN_FACE);
			eew->obj_palbit |= 1 << eew->face_palno;
			CATS_LoadResourceCharArcH(eew->csp, eew->crp, eew->p_handle, 
				cp_def->ncgr, 0, NNS_G2D_VRAM_TYPE_2DMAIN, FR_CHARID_CUTIN_FACE);
			CATS_LoadResourceCellArcH(eew->csp, eew->crp, eew->p_handle, 
				cp_def->ncer, 0, FR_CELLID_CUTIN_FACE);
			CATS_LoadResourceCellAnmArcH(eew->csp, eew->crp, eew->p_handle, 
				cp_def->nanr, 0, FR_CELLANMID_CUTIN_FACE);
			SoftFadePfd(eew->pfd, FADE_MAIN_OBJ, eew->face_palno * 16, 16, 
				ENCOUNT_SPTR_BRAIN_LEADER_COLOR_FADE_EVY, ENCOUNT_SPTR_BRAIN_LEADER_COLOR_FADE);
			//-- VS --//
			palno = CATS_LoadResourcePlttWorkArcH(eew->pfd, FADE_MAIN_OBJ, eew->csp, eew->crp, 
				eew->p_handle, NARC_field_encounteffect_cutin_gym_vs_NCLR, 0, 1, 
				NNS_G2D_VRAM_TYPE_2DMAIN, FR_PLTTID_CUTIN_VS);
			eew->obj_palbit |= 1 << palno;
			CATS_LoadResourceCharArcH(eew->csp, eew->crp, eew->p_handle, 
				NARC_field_encounteffect_cutin_gym_vs128k_NCGR, 0, 
				NNS_G2D_VRAM_TYPE_2DMAIN, FR_CHARID_CUTIN_VS);
			CATS_LoadResourceCellArcH(eew->csp, eew->crp, eew->p_handle, 
				NARC_field_encounteffect_cutin_gym_vs128k_NCER, 0, FR_CELLID_CUTIN_VS);
			CATS_LoadResourceCellAnmArcH(eew->csp, eew->crp, eew->p_handle, 
				NARC_field_encounteffect_cutin_gym_vs128k_NANR, 0, FR_CELLANMID_CUTIN_VS);
		}

		// アクター登録
		task_w->cap_face = CATS_ObjectAdd_S(eew->csp, eew->crp, &FaceObjParam);
		CATS_ObjectEnableCap(task_w->cap_face, CATS_ENABLE_FALSE);
		CATS_ObjectUpdate(task_w->cap_face->act);
		
		EncountEffect_SpTr_VsAnmInit(eew, &task_w->vsanm, 
			ENCOUNT_SPTR_BRAIN_VSMARK_X + FX32_CONST(eew->map_x), 
			ENCOUNT_SPTR_BRAIN_VSMARK_Y + FX32_CONST(eew->map_y), heapID );

		// カラーパレットを設定
	//	ENC_CLACT_ResColorChange( task_w->clact, heapID, cp_def->type, ENCOUNT_SPTR_BRAIN_LEADER_COLOR_FADE_EVY, ENCOUNT_SPTR_BRAIN_LEADER_COLOR_FADE );

		// ジグザグウィンドウ初期化
	//	task_w->p_ziguzagu = ENC_BG_WndZiguzagu_Alloc();

		eew->seq ++;
		break;
		
	case ENCOUNT_SPTR_BRAIN_FLASH_INI:
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
						WIPE_FADE_WHITE, 3, WIPE_DEF_SYNC, heapID );
		eew->seq++;
		break;
		
	case ENCOUNT_SPTR_BRAIN_FLASH_WAIT:
		if( WIPE_SYS_EndCheck() ) {
			eew->seq++;
		}
		break;
	case ENCOUNT_SPTR_BRAIN_FLASH_INI_2:
		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, 
						WIPE_FADE_WHITE, 3, WIPE_DEF_SYNC, heapID );
		eew->seq++;
		break;
		
	case ENCOUNT_SPTR_BRAIN_FLASH_WAIT_2:
		if( WIPE_SYS_EndCheck() ) {
			eew->seq++;
		}
		break;

	case ENCOUNT_SPTR_BRAIN_BG_IN:
		//ウィンドウオープン
		WndOpenEffStart(eew, WINEFF_OPEN);
		eew->seq++;
		break;
		
	case ENCOUNT_SPTR_BRAIN_BG_INWAIT:
		if(eew->wineff.end_flag == TRUE){
			eew->seq++;
			task_w->wait = ENCOUNT_SPTR_BRAIN_BG_AFTER_WAIT;
		}
		break;

	case ENCOUNT_SPTR_BRAIN_VS_ANMWAIT:

		task_w->wait --;
		if( task_w->wait >= 0 ){
			break;
		}

		// VSを出す
		result = EncountEffect_SpTr_VsAnmMain( &task_w->vsanm );
		if( result == TRUE ){
			eew->seq++;
		}
		break;
		
		
	case ENCOUNT_SPTR_BRAIN_OAM_IN:
		eew->seq++;
		break;
		
	case ENCOUNT_SPTR_BRAIN_OAM_INWAIT:
		result = FaceIn_Main(eew, task_w, &eew->face_move_work);
		if( result == TRUE ){
			eew->seq++;
		}
		break;

	case ENCOUNT_SPTR_BRAIN_OAM_FLASH_OUT:
		task_w->wait = ENCOUNT_SPTR_BRAIN_BG_AFTER_WAIT;
		eew->seq++;
		break;

	case ENCOUNT_SPTR_BRAIN_OAM_FLASH_OUT_START:
		task_w->wait --;
		if( task_w->wait >= 0 ){
			break;
		}

		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
						WIPE_FADE_WHITE, 3, WIPE_DEF_SYNC, heapID );
		eew->seq++;
		break;
		
	case ENCOUNT_SPTR_BRAIN_OAM_FLASH_OUT_WAIT:
		if( WIPE_SYS_EndCheck() ) {
			//エンカウントエフェクトでAddした以外のOBJの輝度を落とす
			OS_TPrintf("color conce palbit = %d, %d\n", eew->obj_palbit, eew->obj_palbit^0x3fff);
			ColorConceChangePfd(eew->pfd, FADE_MAIN_OBJ, eew->obj_palbit ^ 0x3fff, 
				ENCOUNT_SPTR_BRAIN_LEADER_COLOR_FADE_EVY, 0x0000);
			// 顔のカラーパレット変更
			SoftFadePfd(eew->pfd, FADE_MAIN_OBJ, eew->face_palno * 16, 16, 
				0, ENCOUNT_SPTR_BRAIN_LEADER_COLOR_FADE);

			// ３D面にブライトネスをかける
			SetBrightness( ENCOUNT_SPTR_BRAIN_3DAREA_BRIGHTNESS,
					PLANEMASK_BG2|PLANEMASK_BG3|PLANEMASK_BD, MASK_MAIN_DISPLAY );

			//　文字を出す	ジムリーダーOAMの位置を基準に出す
			FONTOAM_SetDrawFlag(task_w->fontoam_trainer_name.fontoam, TRUE);
			eew->seq++;
		}
		break;
	case ENCOUNT_SPTR_BRAIN_OAM_FLASH_OUT_IN_START:
		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, 
						WIPE_FADE_WHITE, 3, WIPE_DEF_SYNC, heapID );
		eew->seq++;
		break;
		
	case ENCOUNT_SPTR_BRAIN_OAM_FLASH_OUT_IN_WAIT:
		if( WIPE_SYS_EndCheck() ) {
			task_w->wait = ENCOUNT_SPTR_BRAIN_FLASH_AFTER_WAIT;
			eew->seq++;
		}
		break;
		
	case ENCOUNT_SPTR_BRAIN_WAIT:
		task_w->wait --;
		if( task_w->wait < 0 ){
			eew->seq ++;
		}
		break;
		
	case ENCOUNT_SPTR_BRAIN_WIPE:

		// ホワイトアウト
		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_WHITE, ENCOUNT_SPTR_BRAIN_FADEOUT_SYNC, 1, HEAPID_FRONTIERMAP );
		eew->seq ++;
		break;
		
	case ENCOUNT_SPTR_BRAIN_WIPEWAIT:
		if( WIPE_SYS_EndCheck() ) {
			eew->seq ++;
		}
		break;
		
	case ENCOUNT_SPTR_BRAIN_END:
		WIPE_SetBrightness( WIPE_DISP_SUB, WIPE_FADE_WHITE );
		
		if(eew->end != NULL){
			*(eew->end) = TRUE;		// タスク終了を報告
		}
		
		//FONTOAM破棄
		Sub_FontOamDelete(&task_w->fontoam_trainer_name);
		FONTOAM_SysDelete(eew->fontoam_sys);
		
		// OAM破棄
		CATS_ActorPointerDelete_S(task_w->cap_face);
		EncountEffect_SpTr_VsAnmExit( &task_w->vsanm );

		// ウィンドウ破棄
	//	GX_SetVisibleWnd(GX_WNDMASK_NONE);

		// ブライトネスはき
	//	SetBrightness( BRIGHTNESS_NORMAL,
	//			PLANEMASK_NONE, MASK_MAIN_DISPLAY );
		
		return TRUE;
	}

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   フォントOAMを作成する
 *
 * @param   aci			BIシステムワークへのポインタ
 * @param   font_actor	生成したフォントOAM関連のワーク代入先
 * @param   str			文字列
 * @param   font_type	フォントタイプ(FONT_SYSTEM等)
 * @param   color		フォントカラー構成
 * @param   pal_offset	パレット番号オフセット
 * @param   pal_id		登録開始パレットID
 * @param   x			座標X
 * @param   y			座標Y
 * @param   pos_center  FONTOAM_LEFT(X左端座標) or FONTOAM_CENTER(X中心座標)
 * @param   ex_bmpwin	呼び出し側でフォントOAMに関連付けるBMPWINを持っている場合はここで渡す。
 *          			NULLの場合は中で作成されます。(ex_bmpwinを使用する場合はstrはNULLでOK)
 */
//--------------------------------------------------------------
static void Sub_FontOamCreate(ENCOUNT_EFFECT_WORK *eew, FONT_ACTOR *font_actor, const STRBUF *str, 
	FONT_TYPE font_type, GF_PRINTCOLOR color, int pal_offset, int pal_id, 
	int x, int y, int pos_center, FONT_EX_BMPWIN *ex_bmpwin)
{
	FONTOAM_INIT finit;
	GF_BGL_BMPWIN bmpwin;
	CHAR_MANAGER_ALLOCDATA cma;
	int vram_size;
	FONTOAM_OBJ_PTR fontoam;
	GF_BGL_INI *bgl;
	CATS_RES_PTR crp;
	int font_len, char_len;
	
	GF_ASSERT(font_actor->fontoam == NULL);
	
	bgl = eew->bgl;
	crp = eew->crp;
	
	//文字列のドット幅から、使用するキャラ数を算出する
	if(ex_bmpwin == NULL){
		FontLenGet(str, font_type, &font_len, &char_len);
	}
	else{
		font_len = ex_bmpwin->font_len;
		char_len = ex_bmpwin->char_len;
	}

	//BMP作成
	if(ex_bmpwin == NULL){
		GF_BGL_BmpWinInit(&bmpwin);
		GF_BGL_BmpWinObjAdd(bgl, &bmpwin, char_len, 16 / 8, 0, 0);
		GF_STR_PrintExpand(&bmpwin, font_type, str, 0, 0, MSG_NO_PUT, color, 
			0, 0, NULL);
//		GF_STR_PrintColor(&bmpwin, font_type, str, 0, 0, MSG_NO_PUT, color, NULL );
	}
	else{
		bmpwin = ex_bmpwin->bmpwin;
	}

	vram_size = FONTOAM_NeedCharSize(&bmpwin, NNS_G2D_VRAM_TYPE_2DMAIN,  HEAPID_FRONTIERMAP);
	CharVramAreaAlloc(vram_size, CHARM_CONT_AREACONT, NNS_G2D_VRAM_TYPE_2DMAIN, &cma);
	
	//座標位置修正
	if(pos_center == FONTOAM_CENTER){
		x -= font_len / 2;
	}
//	y += ACTIN_SUB_ACTOR_DISTANCE_INTEGER - 8;
	y -= 8;
	
	finit.fontoam_sys = eew->fontoam_sys;
	finit.bmp = &bmpwin;
	finit.clact_set = CATS_GetClactSetPtr(crp);
	finit.pltt = CATS_PlttProxy(crp, pal_id);
	finit.parent = NULL;
	finit.char_ofs = cma.alloc_ofs;
	finit.x = x;
	finit.y = y;
	finit.bg_pri = CUTIN_BGPRI_NAME;
	finit.soft_pri = CUTIN_SOFTPRI_NAME;
	finit.draw_area = NNS_G2D_VRAM_TYPE_2DMAIN;
	finit.heap = HEAPID_FRONTIERMAP;
	
	fontoam = FONTOAM_Init(&finit);
//	FONTOAM_SetPaletteOffset(fontoam, pal_offset);
	FONTOAM_SetPaletteOffsetAddTransPlttNo(fontoam, pal_offset);
	FONTOAM_SetMat(fontoam, x, y);
	
	//解放処理
	if(ex_bmpwin == NULL){
		GF_BGL_BmpWinDel(&bmpwin);
	}
	
	font_actor->fontoam = fontoam;
	font_actor->cma = cma;
	font_actor->font_len = font_len;
}

//--------------------------------------------------------------
/**
 * @brief   生成されているフォントOAMを全て削除をする
 * @param   eew		BIシステムワークへのポインタ
 */
//--------------------------------------------------------------
static void Sub_FontOamDelete(FONT_ACTOR *font_actor)
{
	FONTOAM_Delete(font_actor->fontoam);
	CharVramAreaFree(&font_actor->cma);
}

//--------------------------------------------------------------
/**
 * @brief   文字列の長さを取得する
 *
 * @param   str				文字列へのポインタ
 * @param   font_type		フォントタイプ
 * @param   ret_dot_len		ドット幅代入先
 * @param   ret_char_len	キャラ幅代入先
 */
//--------------------------------------------------------------
static void FontLenGet(const STRBUF *str, FONT_TYPE font_type, int *ret_dot_len, int *ret_char_len)
{
	int dot_len, char_len;
	
	//文字列のドット幅から、使用するキャラ数を算出する
	dot_len = FontProc_GetPrintStrWidth(font_type, str, 0);
	char_len = dot_len / 8;
	if(FX_ModS32(dot_len, 8) != 0){
		char_len++;
	}
	
	*ret_dot_len = dot_len;
	*ret_char_len = char_len;
}

//----------------------------------------------------------------------------
/**
 *	@brief	VSアニメ	初期化
 *	
 *	@param	task_w		タスクワーク
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void EncountEffect_SpTr_VsAnmInit(ENCOUNT_EFFECT_WORK *eew, ENCOUNT_SPTR_BRAIN_VSANM* p_wk, fx32 x, fx32 y, u32 heapID )
{
	int i;
	TCATS_OBJECT_ADD_PARAM_S head = VsObjParam;
	
	head.x = x >> FX32_SHIFT;
	head.y = y >> FX32_SHIFT;
	
	p_wk->timing = 0;
	p_wk->movenum = 0;

	for( i=0; i<ENCOUNT_SPTR_BRAIN_VSOAM_NUM; i++ ){
		
		p_wk->cap_vs[i] = CATS_ObjectAdd_S(eew->csp, eew->crp, &head);
		CATS_ObjectEnableCap(p_wk->cap_vs[i], CATS_ENABLE_FALSE);

		if( i!=ENCOUNT_SPTR_BRAIN_VSOAM ){	// 最後の１つが文字
			CLACT_SetAffineParam( p_wk->cap_vs[i]->act, CLACT_AFFINE_DOUBLE );
			CLACT_AnmChg( p_wk->cap_vs[i]->act, 1 );
			// 拡大縮小データ作成
			ENC_MoveReqFx( &p_wk->vsscale[i],
					FX32_CONST(2),
					FX32_CONST(1),
					ENCOUNT_SPTR_BRAIN_VSANMSYNC );
		}else{
			// 拡大縮小データ作成
			ENC_MoveReqFx( &p_wk->vsscale[i],
					FX32_CONST(1),
					FX32_CONST(1),
					ENCOUNT_SPTR_BRAIN_VSANMSYNC );
		}
	}
}
static void EncountEffect_SpTr_VsAnmExit( ENCOUNT_SPTR_BRAIN_VSANM* p_wk )
{
	int i;
	for( i=0; i<ENCOUNT_SPTR_BRAIN_VSOAM_NUM; i++ ){
		CATS_ActorPointerDelete_S(p_wk->cap_vs[i]);
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	VSアニメ	メイン
 *
 *	@param	task_w		タスクワーク
 *
 *	@retval	TRUE	完了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
static BOOL EncountEffect_SpTr_VsAnmMain( ENCOUNT_SPTR_BRAIN_VSANM* p_wk )
{
	int i;
	BOOL result;
	BOOL ret = TRUE;
	VecFx32 scale;
		
	// みんなを動かすタイミングを制御
	if( p_wk->movenum < ENCOUNT_SPTR_BRAIN_VSOAM_NUM ){
		ret = FALSE;	// まだ全部動ききってない
		p_wk->timing--;
		if( p_wk->timing <= 0 ){
			p_wk->timing = ENCOUNT_SPTR_BRAIN_VSANMTIMING;
			p_wk->movenum ++;
		}
	}

	// 動かしていい人数分動かす
	for( i=0; i<p_wk->movenum; i++ ){
		result = ENC_MoveMainFx( &p_wk->vsscale[i] );

		scale = ENC_MakeVec( 
				p_wk->vsscale[i].x, 
				p_wk->vsscale[i].x, p_wk->vsscale[i].x );
		CLACT_SetScale( p_wk->cap_vs[i]->act, &scale );
		CLACT_SetDrawFlag( p_wk->cap_vs[i]->act, TRUE );
		// 1つでもまだ動ききっていないのがあるなら途中
		if( result == FALSE ){
			ret = FALSE;
		}
	}

	return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief	動作初期化
 *
 *	@param	p_work			動作ワーク
 *	@param	s_x				開始x座標
 *	@param	e_x				終了x座標
 *	@param	count_max		カウント最大値
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void ENC_MoveReqFx( ENC_MOVE_WORK_FX* p_work, fx32 s_x, fx32 e_x, int count_max )
{
	p_work->x = s_x;
	p_work->s_x = s_x;
	p_work->dis_x = e_x - s_x;
	p_work->count_max = count_max;
	p_work->count = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	動作メイン
 *
 *	@param	p_work	ワーク
 *
 *	@retval	TRUE	終了
 *	@retval FALSE	途中
 */
//-----------------------------------------------------------------------------
static BOOL ENC_MoveMainFx( ENC_MOVE_WORK_FX* p_work )
{
	fx32 w_x;

	// 現在座標取得
	w_x = FX_Mul( p_work->dis_x, p_work->count << FX32_SHIFT );
	w_x = FX_Div( w_x, p_work->count_max << FX32_SHIFT );
	
	p_work->x = w_x + p_work->s_x;

	if( (p_work->count + 1) <= p_work->count_max ){
		p_work->count++;
		return FALSE;
	}

	p_work->count = p_work->count_max;
	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ベクトル作成
 *
 *	@param	x	ｘ値
 *	@param	y	ｙ値
 *	@param	z	ｚ値
 *
 *	@return	ベクトル
 */
//-----------------------------------------------------------------------------
static VecFx32 ENC_MakeVec( fx32 x, fx32 y, fx32 z )
{
	VecFx32 vec;
	vec.x = x;
	vec.y = y;
	vec.z = z;
	return vec;
}

//--------------------------------------------------------------
/**
 * @brief   ジグザグBGをロードする
 *
 * @param   eew			
 * @param   cp_def		
 */
//--------------------------------------------------------------
static void ZigzagBG_Load(ENCOUNT_EFFECT_WORK *eew, const ENCOUNT_SPTR_BRAIN_PARAM *cp_def)
{
	//ウィンドウ機能を使用して、ウィンドウ内しか表示されないようにする
	GX_SetVisibleWnd(GX_WNDMASK_W0 | GX_WNDMASK_W1);
	G2_SetWnd0InsidePlane(GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | 
		GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ, TRUE);
	G2_SetWnd1InsidePlane(GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | 
		GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ, TRUE);
	G2_SetWndOutsidePlane((GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | 
		GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3| GX_WND_PLANEMASK_OBJ) ^ GX_WND_PLANEMASK_BG1,
		TRUE);
	G2_SetWnd0Position(0,0,0,0);
	G2_SetWnd1Position(0,0,0,0);


	//パレット読み込み
	PaletteWorkSet_Arc(eew->pfd, ARC_ENCOUNTEFFECT, 
		cp_def->bg_nclr, HEAPID_FRONTIERMAP, FADE_MAIN_BG, 0x20, 
		ZIGZAG_BG_PALNO * 16);

	ArcUtil_HDL_BgCharSet(eew->p_handle, cp_def->bg_ncgr, eew->bgl, FRMAP_FRAME_WIN, 
		0, 0, FALSE, HEAPID_FRONTIERMAP );

	// スクリーンデータ読み込み
	ArcUtil_HDL_ScrnSet(eew->p_handle, cp_def->bg_nscr, eew->bgl, 
		FRMAP_FRAME_WIN, 0, 0, 0, HEAPID_FRONTIERMAP);

	// パレットをあわせる
	GF_BGL_ScrPalChange(eew->bgl, FRMAP_FRAME_WIN, 0, 0, 32, 32, ZIGZAG_BG_PALNO);

	// スクリーンデータ転送
	GF_BGL_LoadScreenV_Req(eew->bgl, FRMAP_FRAME_WIN);
	
	{//パレットアニメ用のデータをバッファにコピー
		void *plbuf;
		NNSG2dPaletteData* paldata;
		
		plbuf = ArcUtil_PalDataGet(ARC_ENCOUNTEFFECT, 
			cp_def->bg_nclr, &paldata, HEAPID_FRONTIERMAP);
		MI_CpuCopy16(paldata->pRawData, eew->palbuf, ZIGZAG_BG_PAL_ANM_NUM * 32);
		sys_FreeMemoryEz(plbuf);
	}
	
	//パレットアニメ更新用のTCBを生成
	eew->tcb_bgpalanm = TCB_Add(ZigzagPalAnmTCB, eew, TCBPRI_ENC_CUTIN_BGPALANM);
}

//--------------------------------------------------------------
/**
 * @brief   ジグザグBGのパレットアニメ更新TCB
 *
 * @param   tcb		
 * @param   work		
 */
//--------------------------------------------------------------
static void ZigzagPalAnmTCB(TCB_PTR tcb, void *work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	
	eew->palanm_wait++;
	if(eew->palanm_wait < ZIGZAG_BGPALANM_WAIT){
		return;
	}
	eew->palanm_wait = 0;
	eew->palanm_no++;
	if(eew->palanm_no >= ZIGZAG_BG_PAL_ANM_NUM){
		eew->palanm_no = 0;
	}
	PaletteWorkSet(eew->pfd, &eew->palbuf[eew->palanm_no * 16], 
		FADE_MAIN_BG, ZIGZAG_BG_PALNO * 16, 0x20);
}



//--------------------------------------------------------------
/**
 * @brief   ウィンドウ機能の開け閉じエフェクト開始
 *
 * @param   open_close		WINEFF_OPEN or WINEFF_CLOSE
 * @param   end_flag		エフェクト終了後、このワークにTRUEがセットされます
 */
//--------------------------------------------------------------
static void WndOpenEffStart(ENCOUNT_EFFECT_WORK *eew, int open_close)
{
	WNDOPEN_EFF_WORK *wew;
	
	wew = &eew->wineff;
	MI_CpuClear8(wew, sizeof(WNDOPEN_EFF_WORK));
	
	if(open_close == WINEFF_OPEN){
		wew->y1 = WNDOPEN_EFF_CENTER_Y << 8;
		wew->y2 = WNDOPEN_EFF_CENTER_Y << 8;
		TCB_Add(WndEffOpenTask, wew, TCBPRI_ENC_CUTIN);
	}
	else{
		wew->y1 = (WNDOPEN_EFF_CENTER_Y - WNDOPEN_EFF_OPEN_Y_DOT/2) << 8;
		wew->y2 = (WNDOPEN_EFF_CENTER_Y + WNDOPEN_EFF_OPEN_Y_DOT/2) << 8;
		TCB_Add(WndEffCloseTask, wew, TCBPRI_ENC_CUTIN);
	}
}

//--------------------------------------------------------------
/**
 * @brief   ウィンドウ機能：オープンエフェクト実行
 * @param   tcb			TCBへのポインタ
 * @param   work		WNDOPEN_EFF_WORK構造体
 */
//--------------------------------------------------------------
static void WndEffOpenTask(TCB_PTR tcb, void *work)
{
	WNDOPEN_EFF_WORK *wew = work;
	
	switch(wew->seq){
	case 0:
		wew->y1 -= WNDOPEN_EFF_ADD_Y;
		wew->y2 += WNDOPEN_EFF_ADD_Y;
		if(wew->y1 <= (WNDOPEN_EFF_CENTER_Y<<8) - ((WNDOPEN_EFF_OPEN_Y_DOT/2)<<8)){
			wew->y1 = (WNDOPEN_EFF_CENTER_Y - WNDOPEN_EFF_OPEN_Y_DOT/2) << 8;
			wew->y2 = (WNDOPEN_EFF_CENTER_Y + WNDOPEN_EFF_OPEN_Y_DOT/2) << 8;
			wew->seq++;
		}
		//left=0, right=255でやると右端が1ドット残るので2枚使って無理矢理囲む
		wew->wnd0_x1 = 0;
		wew->wnd0_y1 = wew->y1 >> 8;
		wew->wnd0_x2 = 255;
		wew->wnd0_y2 = wew->y2 >> 8;
		wew->wnd1_x1 = 1;
		wew->wnd1_y1 = wew->y1 >> 8;
		wew->wnd1_x2 = 0;
		wew->wnd1_y2 = wew->y2 >> 8;
		break;
	default:
		wew->end_flag = TRUE;
		TCB_Delete(tcb);
		return;
	}
}

//--------------------------------------------------------------
/**
 * @brief   ウィンドウ機能：クローズエフェクト実行
 * @param   tcb			TCBへのポインタ
 * @param   work		WNDOPEN_EFF_WORK構造体
 */
//--------------------------------------------------------------
static void WndEffCloseTask(TCB_PTR tcb, void *work)
{
	WNDOPEN_EFF_WORK *wew = work;
	
	switch(wew->seq){
	case 0:
		wew->y1 += WNDOPEN_EFF_ADD_Y;
		wew->y2 -= WNDOPEN_EFF_ADD_Y;
		if(wew->y1 >= (WNDOPEN_EFF_CENTER_Y<<8)){
			wew->y1 = WNDOPEN_EFF_CENTER_Y << 8;
			wew->y2 = WNDOPEN_EFF_CENTER_Y << 8;
			wew->seq++;
		}
		//left=0, right=255でやると右端が1ドット残るので2枚使って無理矢理囲む
		wew->wnd0_x1 = 0;
		wew->wnd0_y1 = wew->y1 >> 8;
		wew->wnd0_x2 = 255;
		wew->wnd0_y2 = wew->y2 >> 8;
		wew->wnd1_x1 = 1;
		wew->wnd1_y1 = wew->y1 >> 8;
		wew->wnd1_x2 = 0;
		wew->wnd1_y2 = wew->y2 >> 8;
		break;
	default:
		wew->end_flag = TRUE;
		TCB_Delete(tcb);
		return;
	}
}

//--------------------------------------------------------------
/**
 * @brief   顔スライドインのメイン処理
 *
 * @param   task_w		
 * @param   fmw			
 *
 * @retval  TRUE:スライドイン完了
 */
//--------------------------------------------------------------
static BOOL FaceIn_Main(ENCOUNT_EFFECT_WORK *eew, ENCOUNT_SPTR_BRAIN* task_w, FACE_MOVE_WORK *fmw)
{
	int end = 0;
	
	switch(fmw->seq){
	case 0:
		CATS_ObjectPosSetCap_SubSurface(task_w->cap_face, 
			FACE_IN_START_X + eew->map_x, FACE_IN_START_Y + eew->map_y, 
			FMAP_SUB_ACTOR_DISTANCE);
		CATS_ObjectEnableCap(task_w->cap_face, CATS_ENABLE_TRUE);
		fmw->x = FACE_IN_START_X << 8;
		fmw->seq++;
		break;
	case 1:
		fmw->x -= FACE_IN_SPEED_X;
		if(fmw->x <= (FACE_IN_END_X << 8)){
			fmw->x = FACE_IN_END_X << 8;
			fmw->seq++;
		}
		CATS_ObjectPosSetCap_SubSurface(
			task_w->cap_face, fmw->x >> 8, FACE_IN_START_Y + eew->map_y, 
			FMAP_SUB_ACTOR_DISTANCE);
		break;
	default:
		return TRUE;
	}
	return FALSE;
}


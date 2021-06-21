//==============================================================================
/**
 * @file	stage_bg.c
 * @brief	バトルステージ：BGアニメ
 * @author	matsuda
 * @date	2007.08.07(火)
 */
//==============================================================================
#include "common.h"
#include "frontier_types.h"
#include "frontier_tcb_pri.h"
#include "stage_bganm.h"
#include "graphic/frontier_bg_def.h"
#include "system/arc_tool.h"
#include "frontier_map.h"


//==============================================================================
//	定数定義
//==============================================================================
//--------------------------------------------------------------
//	BGアニメ
//--------------------------------------------------------------
///BGアニメパターン数
#define STAGE_BG_ANIME_PATTERN		(2)
///スクリーンバイトサイズ
#define STAGE_BG_SCREEN_SIZE		(0x1000)
///キャラクタバイトサイズ
#define STAGE_BG_CHAR_SIZE		(0x10000)
///BGアニメウェイト
#define STAGE_BG_ANIME_WAIT			(12)

//--------------------------------------------------------------
//	パレットアニメ
//--------------------------------------------------------------
///パレットアニメ開始パレット番号
#define PALANM_START_PALETTE		(5)
///パレットアニメのパレット本数
#define PALANM_PALETTE_NUM			(3)
///パレットアニメウェイト
#define PALANM_ANIME_WAIT			(8)
///パレット転送位置
#define PALANM_TRANS_POS			(6)	//スクリーンが6番パレットで描かれているので
///パレットアニメ開始番号
#define PALANM_START_ANMNO			(2)	//スクリーンがPALANM_START_PALETTEで始まっていないので

//--------------------------------------------------------------
//	ラスター
//--------------------------------------------------------------
///ラスター開始ライン(Yドット位置)
#define LASTER_START_LINE	(0x13 * 8 - 1)
///ラスター終了ライン(Yドット位置)
#define LASTER_END_LINE		(0x1d * 8 + 1)

///ラスター加算X値(下位8ビット小数)
#define LASTER_ADD_X		(0x0200)
///最大X位置(ここまで来たら折り返し)
#define LASTER_MAX_X		(256 - 80)
///折り返し地点に来てから再度動き出すまでのウェイト
#define LASTER_RETURN_WAIT	(0)

//--------------------------------------------------------------
//	半透明
//--------------------------------------------------------------
///半透明第1対象面
#define STAGE_BLD_PLANE_1		(GX_BLEND_PLANEMASK_BG2)
///半透明第2対象面
#define STAGE_BLD_PLANE_2		(GX_BLEND_BGALL | GX_BLEND_PLANEMASK_OBJ)
///第1対象面に対するαブレンディング係数
#define STAGE_BLD_ALPHA_1		(8)
///第2対象面に対するαブレンディング係数
#define STAGE_BLD_ALPHA_2		(9)


//==============================================================================
//	構造体定義
//==============================================================================
///BGアニメーション管理ワーク
typedef struct _STAGE_BGANM_WORK{
	TCB_PTR tcb;				///<制御タスクへのポインタ
	GF_BGL_INI *bgl;
	u16 scrn[STAGE_BG_ANIME_PATTERN][STAGE_BG_SCREEN_SIZE / 2];	///<転送スクリーンデータ
	u8 charcter[STAGE_BG_ANIME_PATTERN][STAGE_BG_CHAR_SIZE];	///<転送キャラクタデータ
	u8 wait;					///<アニメウェイト
	u8 work_no;					///<転送するスクリーンバッファの番号
}STAGE_BGANM_WORK;

///パレットアニメーション管理ワーク
typedef struct{
	TCB_PTR tcb;				///<制御タスクへのポインタ
	PALETTE_FADE_PTR pfd;		///<PFDへのポインタ
	u16 palette[PALANM_PALETTE_NUM][16];	///<パレットバッファ
	u8 anm_no;					///<アニメーション番号
	u8 wait;					///<アニメウェイト
}STAGE_PALANM_WORK;

///ラスターアニメーション管理ワーク
typedef struct{
	TCB_PTR tcb;				///<制御タスクへのポインタ
	TCB_PTR v_tcb;				///<Vブランク制御タスクへのポインタ
	GF_BGL_INI *bgl;			///<BGLへのポインタ
	int default_x;				///<ラスターしない状態のX座標
	int calc_x;					///<ラスターX座標計算領域
	int set_x;					///<ラスターでセットするX座標
	u8 move_seq;				///<シーケンス番号
	u8 wait;					///<ウェイト
	u8 init;					///<1回以上コントロールタスクが実行された
}STAGE_LASTER_WORK;

//--------------------------------------------------------------
/**
 *	バトルステージアニメーション管理ワーク
 */
//--------------------------------------------------------------
typedef struct _STAGE_ANIMATION_WORK{
	STAGE_BGANM_WORK *bganm;		///<BGアニメーション管理ワーク
	STAGE_PALANM_WORK *palanm;	///<パレットアニメーション管理ワーク
	STAGE_LASTER_WORK *laster;	///<ラスターアニメーション管理ワーク
}STAGE_ANIMATION_WORK;


//==============================================================================
//	データ
//==============================================================================
///スクリーン転送アニメのスクリーンアーカイブID
static const u16 AnimeScreenIndex[] = {
	BS_STAGE_01_A_NSCR_BIN,
	BS_STAGE_02_A_NSCR_BIN,
};


//==============================================================================
//	プロトタイプ宣言
//==============================================================================
static STAGE_BGANM_WORK * StageBGAnime_Init(GF_BGL_INI *bgl);
static void StageBGAnime_End(STAGE_BGANM_WORK *sbw);
static void StageBG_AnimeControl(TCB_PTR tcb, void *work);
static STAGE_PALANM_WORK * StagePaletteAnime_Init(PALETTE_FADE_PTR pfd);
static void StagePaletteAnime_End(STAGE_PALANM_WORK *pw);
static void StagePalette_AnimeControl(TCB_PTR tcb, void *work);
static STAGE_LASTER_WORK * StageLasterAnime_Init(GF_BGL_INI *bgl);
static void StageLasterAnime_End(STAGE_LASTER_WORK *laster);
static void StageLaster_AnimeControl(TCB_PTR tcb, void *work);
static void StageLaster_VIntrTCB(TCB_PTR tcb, void *work);
static void StageLaster_HBlank(void *work);




//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   バトルステージ：アニメーションシステム作成
 *
 * @param   bgl		BGLへのポインタ
 * @param   pfd		PFDへのポインタ
 *
 * @retval  ステージアニメーション管理ワークのポインタ
 */
//--------------------------------------------------------------
STAGE_ANIMATION_WORK_PTR StageAnimation_Init(GF_BGL_INI *bgl, PALETTE_FADE_PTR pfd)
{
	STAGE_ANIMATION_WORK *anmsys;
	
	anmsys = sys_AllocMemory(HEAPID_FRONTIERMAP, sizeof(STAGE_ANIMATION_WORK));
	MI_CpuClear8(anmsys, sizeof(STAGE_ANIMATION_WORK));
	
	anmsys->bganm = StageBGAnime_Init(bgl);
	anmsys->palanm = StagePaletteAnime_Init(pfd);
//	anmsys->laster = StageLasterAnime_Init(bgl);

	//ブレンド設定
	G2_SetBlendAlpha(STAGE_BLD_PLANE_1, STAGE_BLD_PLANE_2, STAGE_BLD_ALPHA_1, STAGE_BLD_ALPHA_2);
	
	return anmsys;
}

//--------------------------------------------------------------
/**
 * @brief   バトルステージ：アニメーションシステム終了
 *
 * @param   anmsys		ステージアニメーション管理ワークのポインタ
 */
//--------------------------------------------------------------
void StageAnimation_End(STAGE_ANIMATION_WORK_PTR anmsys)
{
	StageBGAnime_End(anmsys->bganm);
	StagePaletteAnime_End(anmsys->palanm);
//	StageLasterAnime_End(anmsys->laster);
	
	sys_FreeMemoryEz(anmsys);
}


//==============================================================================
//	BGアニメ
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   バトルステージBGアニメシステム作成
 *
 * @param   bgl				BGLへのポインタ
 *
 * @retval  BGアニメ制御ワークのポインタ
 */
//--------------------------------------------------------------
static STAGE_BGANM_WORK * StageBGAnime_Init(GF_BGL_INI *bgl)
{
	STAGE_BGANM_WORK *sbw;
	
	GF_ASSERT(STAGE_BG_ANIME_PATTERN == NELEMS(AnimeScreenIndex));
	
	sbw = sys_AllocMemory(HEAPID_FRONTIERMAP, sizeof(STAGE_BGANM_WORK));
	MI_CpuClear8(sbw, sizeof(STAGE_BGANM_WORK));
	sbw->bgl = bgl;
	
	//スクリーン転送アニメの準備
	{
		ARCHANDLE* hdl_bg;
		void* arc_data;
		NNSG2dScreenData *scrnData;
		int i;
		
		hdl_bg  = ArchiveDataHandleOpen(ARC_FRONTIER_BG,  HEAPID_FRONTIERMAP);
		for(i = 0; i < STAGE_BG_ANIME_PATTERN; i++){
			arc_data = ArcUtil_HDL_ScrnDataGet(hdl_bg, AnimeScreenIndex[i], 1, 
				&scrnData, HEAPID_FRONTIERMAP);
			MI_CpuCopy32(scrnData->rawData, sbw->scrn[i], STAGE_BG_SCREEN_SIZE);
			sys_FreeMemoryEz(arc_data);
		}
		ArchiveDataHandleClose( hdl_bg );
	}

	//キャラクタ転送アニメの準備
	{
		ARCHANDLE *hdl_bg;
		void *arc_data;
		NNSG2dCharacterData *charData;
		
		hdl_bg = ArchiveDataHandleOpen( ARC_FRONTIER_BG, HEAPID_FRONTIERMAP );

		arc_data = ArcUtil_HDL_CharDataGet(
			hdl_bg, BS_STAGE_NCGR_BIN, 1, &charData, HEAPID_FRONTIERMAP);
		MI_CpuCopy32(charData->pRawData, sbw->charcter[0], charData->szByte);
		sys_FreeMemoryEz(arc_data);
		
		arc_data = ArcUtil_HDL_CharDataGet(
			hdl_bg, BS_STAGE02_NCGR_BIN, 1, &charData, HEAPID_FRONTIERMAP);
		MI_CpuCopy32(charData->pRawData, sbw->charcter[1], charData->szByte);
		sys_FreeMemoryEz(arc_data);

		ArchiveDataHandleClose(hdl_bg);
	}

	//スクリーン転送アニメ管理タスクAdd
	sbw->tcb = TCB_Add(StageBG_AnimeControl, sbw, TCBPRI_STAGE_BGANIME_CONTROL);
	
	return sbw;
}

//--------------------------------------------------------------
/**
 * @brief   バトルステージBGアニメシステム終了
 *
 * @param   sbw		BGアニメ制御ワークのポインタ
 */
//--------------------------------------------------------------
static void StageBGAnime_End(STAGE_BGANM_WORK *sbw)
{
	TCB_Delete(sbw->tcb);
	sys_FreeMemoryEz(sbw);
}

//--------------------------------------------------------------
/**
 * @brief   BGアニメコントロールタスク
 *
 * @param   tcb			TCBへのポインタ
 * @param   work		STAGE_BGANM_WORK構造体
 */
//--------------------------------------------------------------
static void StageBG_AnimeControl(TCB_PTR tcb, void *work)
{
	u32 idx;
	STAGE_BGANM_WORK *sbw = work;
	
	if(sbw->wait < STAGE_BG_ANIME_WAIT){
		sbw->wait++;
		return;
	}
	sbw->wait = 0;
	
	{
		GF_BGL_LoadCharacter(sbw->bgl, FRMAP_FRAME_MAP, 
			sbw->charcter[sbw->work_no], STAGE_BG_CHAR_SIZE, 0);
	}

	GF_BGL_ScreenBufSet(sbw->bgl, FRMAP_FRAME_MAP, sbw->scrn[sbw->work_no], STAGE_BG_SCREEN_SIZE);
	GF_BGL_LoadScreenV_Req(sbw->bgl, FRMAP_FRAME_MAP);
	sbw->work_no ^= 1;
}


//==============================================================================
//	パレットアニメ
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   バトルステージパレットアニメシステム作成
 *
 * @param   pfd				PFDへのポインタ
 *
 * @retval  パレットアニメ制御ワークのポインタ
 */
//--------------------------------------------------------------
static STAGE_PALANM_WORK * StagePaletteAnime_Init(PALETTE_FADE_PTR pfd)
{
	STAGE_PALANM_WORK *pw;
	
	pw = sys_AllocMemory(HEAPID_FRONTIERMAP, sizeof(STAGE_PALANM_WORK));
	MI_CpuClear8(pw, sizeof(STAGE_PALANM_WORK));
	pw->pfd = pfd;
	pw->anm_no = PALANM_START_ANMNO;
	
	//パレットをバッファにコピー
	{
		u16 *src_pal;
		src_pal = PaletteWorkDefaultWorkGet(pfd, FADE_MAIN_BG);
		MI_CpuCopy16(&src_pal[16 * PALANM_START_PALETTE], 
			pw->palette, PALANM_PALETTE_NUM * 16 * sizeof(u16));
	}
	
	//パレットアニメ管理タスクAdd
	pw->tcb = TCB_Add(StagePalette_AnimeControl, pw, TCBPRI_STAGE_PALANIME_CONTROL);
	
	return pw;
}

//--------------------------------------------------------------
/**
 * @brief   バトルステージBGアニメシステム終了
 *
 * @param   pw		BGアニメ制御ワークのポインタ
 */
//--------------------------------------------------------------
static void StagePaletteAnime_End(STAGE_PALANM_WORK *pw)
{
	TCB_Delete(pw->tcb);
	sys_FreeMemoryEz(pw);
}

//--------------------------------------------------------------
/**
 * @brief   パレットアニメコントロールタスク
 *
 * @param   tcb			TCBへのポインタ
 * @param   work		STAGE_PALANM_WORK構造体
 */
//--------------------------------------------------------------
static void StagePalette_AnimeControl(TCB_PTR tcb, void *work)
{
	STAGE_PALANM_WORK *pw = work;
	
	if(pw->wait < PALANM_ANIME_WAIT){
		pw->wait++;
		return;
	}
	pw->wait = 0;
	
	PaletteWorkSet(pw->pfd, pw->palette[pw->anm_no], FADE_MAIN_BG, PALANM_TRANS_POS * 16, 0x20);
	pw->anm_no++;
	if(pw->anm_no >= PALANM_PALETTE_NUM){
		pw->anm_no = 0;
	}
}


//==============================================================================
//	ラスターアニメ
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   バトルステージラスターアニメシステム作成
 *
 * @param   bgl				BGLへのポインタ
 *
 * @retval  ラスターアニメ制御ワークのポインタ
 */
//--------------------------------------------------------------
static STAGE_LASTER_WORK * StageLasterAnime_Init(GF_BGL_INI *bgl)
{
	STAGE_LASTER_WORK *laster;
	BOOL ret;
	
	laster = sys_AllocMemory(HEAPID_FRONTIERMAP, sizeof(STAGE_LASTER_WORK));
	MI_CpuClear8(laster, sizeof(STAGE_LASTER_WORK));
	laster->bgl = bgl;
	
	laster->tcb = TCB_Add(StageLaster_AnimeControl, laster, TCBPRI_STAGE_LASTER_CONTROL);
	laster->v_tcb = VIntrTCB_Add(StageLaster_VIntrTCB, laster, 10);
	ret = sys_HBlankIntrSet(StageLaster_HBlank, laster);
	GF_ASSERT(ret == TRUE);
	
	return laster;
}

//--------------------------------------------------------------
/**
 * @brief   バトルステージラスターアニメシステム終了
 *
 * @param   laster		ラスターアニメ制御ワークのポインタ
 */
//--------------------------------------------------------------
static void StageLasterAnime_End(STAGE_LASTER_WORK *laster)
{
	TCB_Delete(laster->tcb);
	TCB_Delete(laster->v_tcb);
	sys_FreeMemoryEz(laster);
	sys_HBlankIntrStop();	//HBlank割り込み停止
}

//--------------------------------------------------------------
/**
 * @brief   バトルステージ：ラスターアニメ制御タスク
 *
 * @param   tcb			TCBへのポインタ
 * @param   work		STAGE_LASTER_WORK構造体
 */
//--------------------------------------------------------------
static void StageLaster_AnimeControl(TCB_PTR tcb, void *work)
{
	STAGE_LASTER_WORK *laster = work;
	
	//ラスターX座標計算
	{
		enum{
			SEQ_MOVE,
			SEQ_RETURN_WAIT,
			SEQ_BACK_MOVE,
			SEQ_BACK_RETURN_WAIT,
		};
		
		switch(laster->move_seq){
		case SEQ_MOVE:
			laster->calc_x += LASTER_ADD_X;
			if((laster->calc_x >> 8) > LASTER_MAX_X){
				laster->calc_x = (LASTER_MAX_X << 8) | 0xff;
				laster->move_seq++;
			}
			break;
		case SEQ_RETURN_WAIT:
		case SEQ_BACK_RETURN_WAIT:
			laster->wait++;
			if(laster->wait > LASTER_RETURN_WAIT){
				laster->wait = 0;
				if(laster->move_seq == SEQ_BACK_RETURN_WAIT){
					laster->move_seq = 0;
				}
				else{
					laster->move_seq++;
				}
			}
			break;
		case SEQ_BACK_MOVE:
			laster->calc_x -= LASTER_ADD_X;
			if(laster->calc_x < 0x100){
				laster->calc_x = 0;
				laster->move_seq++;
			}
			break;
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   ラスターアニメVブランクタスク
 *
 * @param   tcb			TCBへのポインタ
 * @param   work		STAGE_LASTER_WORK構造体
 *
 * ※Vブランクタスク
 */
//--------------------------------------------------------------
static void StageLaster_VIntrTCB(TCB_PTR tcb, void *work)
{
	STAGE_LASTER_WORK *laster = work;
	int scr_y, start, end;
	
	laster->init = TRUE;
	laster->default_x = 0;//GF_BGL_ScrollGetX(laster->bgl, FRMAP_FRAME_EFF);
	laster->set_x = -(laster->calc_x >> 8);
	
	//ラスター開始ラインが画面上部に行っている場合の処理
	scr_y = GF_BGL_ScrollGetY(laster->bgl, FRMAP_FRAME_EFF);
	start = LASTER_START_LINE - scr_y;
	end = LASTER_END_LINE - scr_y;
	if(start <= 1 && end > 0){
		GF_BGL_ScrollSet(laster->bgl, FRMAP_FRAME_EFF, GF_BGL_SCROLL_X_SET, laster->set_x);
	}
	else{
		GF_BGL_ScrollSet(laster->bgl, FRMAP_FRAME_EFF, GF_BGL_SCROLL_X_SET, laster->default_x);
	}
}

//--------------------------------------------------------------
/**
 * @brief   ラスターアニメ：Hブランクでのスクロール座標設定
 *
 * @param   work		STAGE_LASTER_WORK構造体
 */
//--------------------------------------------------------------
static void StageLaster_HBlank(void *work)
{
	STAGE_LASTER_WORK *laster = work;
	s32 v_count;
	int scr_y, start, end;
	
	if(laster->init == 0){
		return;	//まだコントロールタスクが一度も実行されていない
	}
	
	v_count = GX_GetVCount();
	scr_y = GF_BGL_ScrollGetY(laster->bgl, FRMAP_FRAME_EFF);
	start = LASTER_START_LINE - scr_y;
	end = LASTER_END_LINE - scr_y;
	
	if(v_count == start){
		GF_BGL_ScrollSet(laster->bgl, FRMAP_FRAME_EFF, GF_BGL_SCROLL_X_SET, laster->set_x);
	}
	else if(v_count == end){
		GF_BGL_ScrollSet(laster->bgl, FRMAP_FRAME_EFF, GF_BGL_SCROLL_X_SET, laster->default_x);
	}
	
}

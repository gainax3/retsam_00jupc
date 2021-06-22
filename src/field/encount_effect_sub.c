//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		encount_effect_sub.c
 *	@brief		エンカウントエフェクト
 *	@author		tomoya takahshi
 *	@data		2006.05.07
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"
#include "system/lib_pack.h"
#include "system/brightness.h"
#include "fieldsys.h"
#include "fieldmap_work.h"
#include "field_event.h"
#include "fld_motion_bl.h"
#include "encount_effect_def.h"
#include "system/wipe.h"
#include "system/laster.h"
#include "system/fontproc.h"
#include "field_encounteffect.naix"
#include "savedata/mystatus.h"
#include "include/battle/trtype_def.h"
#include "include/battle/trno_def.h"
#include "include/msgdata/msg.naix"
#include "include/msgdata/msg_encount_effect.h"

#include "field/fieldmap.h"

#define	__ECNT_TOMO_H_GLOBAL
#include "encount_effect_sub.h"

//-----------------------------------------------------------------------------
/**
 *					コーディング規約
 *		●関数名
 *				１文字目は大文字それ以降は小文字にする
 *		●変数名
 *				・変数共通
 *						constには c_ を付ける
 *						staticには s_ を付ける
 *						ポインタには p_ を付ける
 *						全て合わさると csp_ となる
 *				・グローバル変数
 *						１文字目は大文字
 *				・関数内変数
 *						小文字と”＿”と数字を使用する 関数の引数もこれと同じ
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *	ジャンプ！
 *
 *	野生		[ FIELD ]		
 *	トレーナー	[ TRAINER ]
 *	特殊		[ SPECIAL ]
 *	銀河団		[ GINGA ]
 *	伝説ポケモン[ LEGEND ]
 *	特殊トレーナー[ SP_TR ]
 * 
 *
 */
//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
/**
 * 
 *		フィールド　野生エンカウント [ FIELD ]
 * 
 */
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
//	ラスタースクロール　タスク優先順位
//=====================================
#define ENCOUNT_EFF_LASTER_SCRLL_VWAIT_BUFF_SWITCH_TASK_PRI	( 1024 )
#define ENCOUNT_EFF_LASTER_SCRLL_VINTR_DMA_SET_TASK_PRI	( 1024 )


//-------------------------------------
//	野生　草　レベル高い
//=====================================
// シーケンス
enum{
	ENCOUNT_GRASS_HIGH_START,
	ENCOUNT_GRASS_HIGH_FLASH_INI,
	ENCOUNT_GRASS_HIGH_FLASH_WAIT,
	ENCOUNT_GRASS_HIGH_MOVE00,
	ENCOUNT_GRASS_HIGH_MOVE01,
	ENCOUNT_GRASS_HIGH_END,
};
#define ENCOUNT_GRASS_HIGH_MOVE00_SYNC	( 6 )		// シンク数
#define ENCOUNT_GRASS_HIGH_MOVE01_SYNC	( 6 )

#define ENCOUNT_GRASS_HIGH_MOVE00_CAMERA_MOVE	( FX32_ONE * 50 )	// カメラ動作
#define ENCOUNT_GRASS_HIGH_MOVE00_CAMERA_SS		( FX32_ONE * 30 )	// カメラ動作
#define ENCOUNT_GRASS_HIGH_MOVE00_BGSL_SS	( FX32_ONE * -12 )	// 初速度
#define ENCOUNT_GRASS_HIGH_MOVE00_BGSL_X	( -3 * FX32_ONE )	// 動さｘ

#define ENCOUNT_GRASS_HIGH_MOVE01_CAMERA_MOVE	( -FX32_ONE * 50 )
#define ENCOUNT_GRASS_HIGH_MOVE01_CAMERA_SS	( -FX32_ONE * 255 )
#define ENCOUNT_GRASS_HIGH_MOVE01_BGSL_SS	( FX32_ONE * 30 )		// 初速度
#define ENCOUNT_GRASS_HIGH_MOVE01_BGSL_X	( 255 * FX32_ONE )	// 動さｘ
#define ENCOUNT_GRASS_HIGH_DOT			( 2 )

//-------------------------------------
//	野生　草　レベル低い
//=====================================
// シーケンス
enum{
	ENCOUNT_GRASS_LOW_START,
	ENCOUNT_GRASS_LOW_FLASH_INI,
	ENCOUNT_GRASS_LOW_FLASH_WAIT,
	ENCOUNT_GRASS_LOW_MOVE00,
	ENCOUNT_GRASS_LOW_MOVE01,
	ENCOUNT_GRASS_LOW_END,
};
#define ENCOUNT_GRASS_LOW_MOVE00_SYNC	( 6 )		// シンク数
#define ENCOUNT_GRASS_LOW_MOVE01_SYNC	( 6 )

#define ENCOUNT_GRASS_LOW_MOVE00_CAMERA_MOVE	( FX32_ONE * 50 )	// カメラ動作
#define ENCOUNT_GRASS_LOW_MOVE00_CAMERA_SS		( FX32_ONE * 30 )	// カメラ動作
#define ENCOUNT_GRASS_LOW_MOVE00_BGSL_SS	( FX32_ONE * -12 )	// 初速度
#define ENCOUNT_GRASS_LOW_MOVE00_BGSL_X	( -2 * FX32_ONE )	// 動さｘ

#define ENCOUNT_GRASS_LOW_MOVE01_CAMERA_MOVE	( -FX32_ONE * 30 )
#define ENCOUNT_GRASS_LOW_MOVE01_CAMERA_SS	( -FX32_ONE * 100 )
#define ENCOUNT_GRASS_LOW_MOVE01_BGSL_SS	( FX32_ONE *30 )		// 初速度
#define ENCOUNT_GRASS_LOW_MOVE01_BGSL_X	( 255 * FX32_ONE )	// 動さｘ
#define ENCOUNT_GRASS_LOW_DOT			( 5 )

//-------------------------------------
//	野生　水　レベル低い
//=====================================
// シーケンス
enum{
	ENCOUNT_WATER_LOW_START,
	ENCOUNT_WATER_LOW_FLASH_INI,
	ENCOUNT_WATER_LOW_FLASH_WAIT,
	ENCOUNT_WATER_LOW_MOVE,
	ENCOUNT_WATER_LOW_WIPE,
	ENCOUNT_WATER_LOW_WIPE_WAIT,
	ENCOUNT_WATER_LOW_END,
};

#define ENCOUNT_WATER_LOW_FLASH_DO_WAIT	( 10 )
#define ENCOUNT_WATER_LOW_MOVE_SYNC	( 9 )
#define ENCOUNT_WATER_LOW_MOVE_WAIT	( 12 )
#define ENCOUNT_WATER_LOW_SIN_R		( FX32_CONST( 12 ) )	// 半径
#define ENCOUNT_WATER_LOW_LASTER_SP	( 800 )	// ラスタースピード
#define ENCOUNT_WATER_LOW_SIN_ADDR	( (0xffff/192) * 2 )// ラスター角度スピード
#define ENCOUNT_WATER_LOW_LASTER_TASK_PRI	( TCB_TSK_PRI - 1 )
#define ENCOUNT_WATER_LOW_WIPE_SYNC	( 8 )

#define ENCOUNT_WATER_LOW_MOVE_Y_S	( 192 )
#define ENCOUNT_WATER_LOW_MOVE_Y_E	( 0 )


//-------------------------------------
//	野生　水　レベル高い
//=====================================
// シーケンス
enum{
	ENCOUNT_WATER_HIGH_START,
	ENCOUNT_WATER_HIGH_FLASH_INI,
	ENCOUNT_WATER_HIGH_FLASH_WAIT,
	ENCOUNT_WATER_HIGH_MOVE,
	ENCOUNT_WATER_HIGH_WIPE,
	ENCOUNT_WATER_HIGH_WIPE_WAIT,
	ENCOUNT_WATER_HIGH_END,
};

#define ENCOUNT_WATER_HIGH_FLASH_DO_WAIT	( 10  )
#define ENCOUNT_WATER_HIGH_MOVE_SYNC	( 9 )
#define ENCOUNT_WATER_HIGH_MOVE_WAIT	( 12 )
#define ENCOUNT_WATER_HIGH_SIN_R		( FX32_CONST( 15 ) )	// 半径
#define ENCOUNT_WATER_HIGH_LASTER_SP	( 800 )	// ラスタースピード
#define ENCOUNT_WATER_HIGH_SIN_ADDR	( (0xffff/192) * 3 )// ラスター角度スピード
#define ENCOUNT_WATER_HIGH_WIPE_SYNC	( 8 )
#define ENCOUNT_WATER_HIGH_LASTER_TASK_PRI	( TCB_TSK_PRI - 1 )

#define ENCOUNT_WATER_HIGH_MOVE_Y_S	( 192 )
#define ENCOUNT_WATER_HIGH_MOVE_Y_E	( 0 )
#define ENCOUNT_WATER_HIGH_LASTER_X_MUL	( 3 )
#define ENCOUNT_WATER_HIGH_LASTER_Y_MUL	( 1 )

//-------------------------------------
//	野生　ダンジョン　レベル低
//=====================================
// シーケンス
enum{
	ENCOUNT_DAN_LOW_START,
	ENCOUNT_DAN_LOW_FLASH_INI,
	ENCOUNT_DAN_LOW_FLASH_WAIT,
	ENCOUNT_DAN_LOW_WIPE,
	ENCOUNT_DAN_LOW_WIPE_WAIT,
	ENCOUNT_DAN_LOW_END,
};

#define ENCOUNT_DAN_LOW_MOVE_SYNC	( 12 )
#define ENCOUNT_DAN_LOW_MOVE00_CAMERA_MOVE		( -FX32_ONE * 400 )	// カメラ動作
#define ENCOUNT_DAN_LOW_MOVE00_CAMERA_SS		( -FX32_ONE * 2 )	// カメラ動作

//-------------------------------------
//	野生　ダンジョン　レベル高
//=====================================
// シーケンス
enum{
	ENCOUNT_DAN_HIGH_START,
	ENCOUNT_DAN_HIGH_FLASH_INI,
	ENCOUNT_DAN_HIGH_FLASH_WAIT,
	ENCOUNT_DAN_HIGH_WIPE,
	ENCOUNT_DAN_HIGH_WIPE_WAIT,
	ENCOUNT_DAN_HIGH_END,
};

#define ENCOUNT_DAN_HIGH_MOVE_SYNC	( 12 )
#define ENCOUNT_DAN_HIGH_MOVE00_CAMERA_MOVE		( -FX32_ONE * 800 )	// カメラ動作
#define ENCOUNT_DAN_HIGH_MOVE00_CAMERA_SS		( -FX32_ONE * 5 )	// カメラ動作




//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
//	野生　草　レベル高い
//=====================================
typedef struct{
	GF_CAMERA_PTR	camera;				// カメラデータ
	ENC_ADDMOVE_WORK_FX camera_dist;
	ENC_HB_BG_SLICE* p_slice;
} ENCOUNT_GRASS_HIGH;

//-------------------------------------
//	野生　草　レベル低い
//=====================================
typedef struct{
	GF_CAMERA_PTR	camera;				// カメラデータ
	ENC_ADDMOVE_WORK_FX camera_dist;
	ENC_HB_BG_SLICE* p_slice;
} ENCOUNT_GRASS_LOW;

//-------------------------------------
//	p_lasterスクロールワーク
//=====================================
typedef struct {
	LASTER_SCROLL_PTR p_laster;
	TCB_PTR VDma;
	u32 dmacount;
} ENCOUNT_LASTER_SCROLL;


//-------------------------------------
//	野生　水　レベル低い
//=====================================
typedef struct{
	ENCOUNT_LASTER_SCROLL laster;
//	ENC_MOVE_WORK move_y;
	s32 wait;
} ENCOUNT_WATER_LOW;

//-------------------------------------
//	野生　水　レベル高い
//=====================================
typedef struct{
	ENCOUNT_LASTER_SCROLL laster;
//	ENC_MOVE_WORK move_y;
	s32 wait;
} ENCOUNT_WATER_HIGH;

//-------------------------------------
//	野生　ダンジョン　レベル低い
//=====================================
typedef struct{
	GF_CAMERA_PTR	camera;				// カメラデータ
	ENC_ADDMOVE_WORK_FX camera_dist;
} ENCOUNT_DAN_LOW;

//-------------------------------------
//	野生　ダンジョン　レベル高い
//=====================================
typedef struct{
	GF_CAMERA_PTR	camera;				// カメラデータ
	ENC_ADDMOVE_WORK_FX camera_dist;
} ENCOUNT_DAN_HIGH;


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
static TCB_PTR EncountEffect_LasterScroll_VDmaTaskReq( ENCOUNT_LASTER_SCROLL* p_lstscr );
static void EncountEffect_LasterScroll_VDmaTCB( TCB_PTR tcb, void* p_work );
static void EF_Laster_Init( ENCOUNT_LASTER_SCROLL* p_laster, u32 heapID );
static void EF_Laster_Delete( ENCOUNT_LASTER_SCROLL* p_laster );
static void EF_Laster_Start( ENCOUNT_LASTER_SCROLL* p_laster, u8 start, u8 end, u16 add_r, fx32 r_w, s16 scr_sp, u32 bg_no, u32 init_num, u32 tsk_pri );

static void EF_Laster_BuffYSet( ENCOUNT_LASTER_SCROLL* p_laster, s16 start_y );
static void EF_Laster_BuffXFlip( ENCOUNT_LASTER_SCROLL* p_laster, u32 dot );

//----------------------------------------------------------------------------
/**
 *	@brief	野生　草　レベル高い
 *
 *	@param	tcb		タスクワーク
 *	@param	work	ワーク
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
void EncountEffect_Field_GrassLevelHigh(TCB_PTR tcb, void* work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	ENCOUNT_GRASS_HIGH* task_w = eew->work;
	fx32 distans;
	BOOL result;

	switch(eew->seq){
	case ENCOUNT_GRASS_HIGH_START:
		eew->work = sys_AllocMemory(HEAPID_FIELD, sizeof(ENCOUNT_GRASS_HIGH));
		memset( eew->work, 0, sizeof(ENCOUNT_GRASS_HIGH) );
		task_w = eew->work;

		// スライスワーク作成
		task_w->p_slice = ENC_BG_Slice_Alloc();

		eew->seq++;
		break;
		
	case ENCOUNT_GRASS_HIGH_FLASH_INI:
		EncountFlashTask(MASK_MAIN_DISPLAY, 16, -16,  &eew->wait, 2);

		eew->seq++;
		break;
		
	case ENCOUNT_GRASS_HIGH_FLASH_WAIT:
		if(eew->wait){
			eew->wait = 0;
			eew->seq++;

			// 動作開始
			ENC_BG_Slice_Start( eew, task_w->p_slice, 
					ENCOUNT_GRASS_HIGH_DOT,
					ENCOUNT_GRASS_HIGH_MOVE00_SYNC + 1, 
					0, ENCOUNT_GRASS_HIGH_MOVE00_BGSL_X,
					ENCOUNT_GRASS_HIGH_MOVE00_BGSL_SS );
			// 動作ワーク初期化
			task_w->camera = eew->fsw->camera_ptr;
			distans = GFC_GetCameraDistance( task_w->camera );
			ENC_AddMoveReqFx( &task_w->camera_dist, distans, distans + ENCOUNT_GRASS_HIGH_MOVE00_CAMERA_MOVE, ENCOUNT_GRASS_HIGH_MOVE00_CAMERA_SS, ENCOUNT_GRASS_HIGH_MOVE00_SYNC );
		}
		break;
	
	case ENCOUNT_GRASS_HIGH_MOVE00:
		result = ENC_AddMoveMainFx( &task_w->camera_dist );
		GFC_SetCameraDistance( task_w->camera_dist.x, task_w->camera );

		if( result == TRUE ){
			eew->seq++;
			// 動作開始
			ENC_BG_Slice_Change( eew, task_w->p_slice, ENCOUNT_GRASS_HIGH_DOT, ENCOUNT_GRASS_HIGH_MOVE01_SYNC, ENCOUNT_GRASS_HIGH_MOVE00_BGSL_X, ENCOUNT_GRASS_HIGH_MOVE01_BGSL_X, ENCOUNT_GRASS_HIGH_MOVE01_BGSL_SS );
			// 動作ワーク初期化
			task_w->camera = eew->fsw->camera_ptr;
			distans = GFC_GetCameraDistance( task_w->camera );
			ENC_AddMoveReqFx( &task_w->camera_dist, distans, distans + ENCOUNT_GRASS_HIGH_MOVE01_CAMERA_MOVE, ENCOUNT_GRASS_HIGH_MOVE01_CAMERA_SS, ENCOUNT_GRASS_HIGH_MOVE01_SYNC );
		}
		break;

	case ENCOUNT_GRASS_HIGH_MOVE01:
		result = ENC_AddMoveMainFx( &task_w->camera_dist );
		GFC_SetCameraDistance( task_w->camera_dist.x, task_w->camera );

		if( (result == TRUE) && (ENC_HBlankEndCheck( eew ) == TRUE) ){
			eew->seq++;
		}
		break;

	case ENCOUNT_GRASS_HIGH_END:
		WIPE_SetBrightnessFadeOut( WIPE_FADE_BLACK );

		G2_SetBG0Offset(0, 0);
		G2_SetBG1Offset(0, 0);
		G2_SetBG2Offset(0, 0);
		G2_SetBG3Offset(0, 0);

		if(eew->end != NULL){
			*(eew->end) = TRUE;		// タスク終了を報告
		}
		ENC_BG_Slice_Delete( task_w->p_slice );
		ENC_End( eew, tcb );
		break;

	default:
		break;
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief	野生　草　レベル低い
 *
 *	@param	tcb		タスクワーク
 *	@param	work	ワーク
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
void EncountEffect_Field_GrassLevelLow(TCB_PTR tcb, void* work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	ENCOUNT_GRASS_LOW* task_w = eew->work;
	fx32 distans;
	BOOL result;

	switch(eew->seq){
	case ENCOUNT_GRASS_LOW_START:
		eew->work = sys_AllocMemory(HEAPID_FIELD, sizeof(ENCOUNT_GRASS_LOW));
		memset( eew->work, 0, sizeof(ENCOUNT_GRASS_LOW) );
		task_w = eew->work;

		// スライスワーク作成
		task_w->p_slice = ENC_BG_Slice_Alloc();

		eew->seq++;
		break;
		
	case ENCOUNT_GRASS_LOW_FLASH_INI:
		EncountFlashTask(MASK_MAIN_DISPLAY, -16, -16,  &eew->wait, 2);

		eew->seq++;
		break;
		
	case ENCOUNT_GRASS_LOW_FLASH_WAIT:
		if(eew->wait){
			eew->wait = 0;
			eew->seq++;

			// 動作開始
			ENC_BG_Slice_Start( eew, task_w->p_slice, 
					ENCOUNT_GRASS_LOW_DOT,
					ENCOUNT_GRASS_LOW_MOVE00_SYNC + 1,	///<下で動作中に値変更するため
					0, ENCOUNT_GRASS_LOW_MOVE00_BGSL_X,
					ENCOUNT_GRASS_LOW_MOVE00_BGSL_SS );
			// 動作ワーク初期化
			task_w->camera = eew->fsw->camera_ptr;
			distans = GFC_GetCameraDistance( task_w->camera );
			ENC_AddMoveReqFx( &task_w->camera_dist, distans, distans + ENCOUNT_GRASS_LOW_MOVE00_CAMERA_MOVE, ENCOUNT_GRASS_LOW_MOVE00_CAMERA_SS, ENCOUNT_GRASS_LOW_MOVE00_SYNC );
		}
		break;
	
	case ENCOUNT_GRASS_LOW_MOVE00:
		result = ENC_AddMoveMainFx( &task_w->camera_dist );
		GFC_SetCameraDistance( task_w->camera_dist.x, task_w->camera );

		if( (result == TRUE) ){
			eew->seq++;
			// 動作開始
			ENC_BG_Slice_Change( eew, task_w->p_slice, ENCOUNT_GRASS_LOW_DOT, ENCOUNT_GRASS_LOW_MOVE01_SYNC, ENCOUNT_GRASS_LOW_MOVE00_BGSL_X, ENCOUNT_GRASS_LOW_MOVE01_BGSL_X, ENCOUNT_GRASS_LOW_MOVE01_BGSL_SS );
			// 動作ワーク初期化
			task_w->camera = eew->fsw->camera_ptr;
			distans = GFC_GetCameraDistance( task_w->camera );
			ENC_AddMoveReqFx( &task_w->camera_dist, distans, distans + ENCOUNT_GRASS_LOW_MOVE01_CAMERA_MOVE, ENCOUNT_GRASS_LOW_MOVE01_CAMERA_SS, ENCOUNT_GRASS_LOW_MOVE01_SYNC );
		}
		break;

	case ENCOUNT_GRASS_LOW_MOVE01:
		result = ENC_AddMoveMainFx( &task_w->camera_dist );
		GFC_SetCameraDistance( task_w->camera_dist.x, task_w->camera );

		if( (result == TRUE) && (ENC_HBlankEndCheck( eew ) == TRUE) ){
			eew->seq++;
		}
		break;

	case ENCOUNT_GRASS_LOW_END:
		WIPE_SetBrightnessFadeOut( WIPE_FADE_BLACK );

		G2_SetBG0Offset(0, 0);
		G2_SetBG1Offset(0, 0);
		G2_SetBG2Offset(0, 0);
		G2_SetBG3Offset(0, 0);

		if(eew->end != NULL){
			*(eew->end) = TRUE;		// タスク終了を報告
		}
		ENC_BG_Slice_Delete( task_w->p_slice );
		ENC_End( eew, tcb );
		break;

	default:
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	水中エンカウントエフェクト
 *
 *	@param	tcb		タスクワーク
 *	@param	work	ワーク
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
void EncountEffect_Field_WaterLevelLow(TCB_PTR tcb, void* work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	ENCOUNT_WATER_LOW* task_w = eew->work;

	switch(eew->seq){
	case ENCOUNT_WATER_LOW_START:
		eew->work = sys_AllocMemory(HEAPID_FIELD, sizeof(ENCOUNT_WATER_LOW));
		memset( eew->work, 0, sizeof(ENCOUNT_WATER_LOW) );
		task_w = eew->work;

		// ラスター初期化
		EF_Laster_Init( &task_w->laster, HEAPID_FIELD );
		
		// フィールドHブランクOFF
		FLDHBLANK_SYS_Stop( eew->fsw->fldmap->hblanksys );

		eew->seq++;
		break;
		
	case ENCOUNT_WATER_LOW_FLASH_INI:
		EncountFlashTask(MASK_MAIN_DISPLAY, -16, -16,  &eew->wait, 2);
		task_w->wait = ENCOUNT_WATER_LOW_FLASH_DO_WAIT;
		eew->seq++;
		break;
		
	case ENCOUNT_WATER_LOW_FLASH_WAIT:
		task_w->wait --;
		if(task_w->wait < 0){
			eew->seq++;
			task_w->wait = ENCOUNT_WATER_LOW_MOVE_WAIT;
			EF_Laster_Start( &task_w->laster,
					0, 191, ENCOUNT_WATER_LOW_SIN_ADDR, 
					ENCOUNT_WATER_LOW_SIN_R, ENCOUNT_WATER_LOW_LASTER_SP,
					LASTER_SCROLL_MBG0, 0, ENCOUNT_WATER_LOW_LASTER_TASK_PRI );

/*			ENC_MoveReq( &task_w->move_y, 
					ENCOUNT_WATER_LOW_MOVE_Y_S,
					ENCOUNT_WATER_LOW_MOVE_Y_E,
					ENCOUNT_WATER_LOW_MOVE_SYNC );//*/
		}
		break;
		
	case ENCOUNT_WATER_LOW_MOVE:
//		ENC_MoveMain( &task_w->move_y );
//		EF_Laster_BuffYSet( &task_w->laster, task_w->move_y.x );
		task_w->wait --;
		if( task_w->wait < 0 ){
			eew->seq++;
		}
		break;
		
	case ENCOUNT_WATER_LOW_WIPE:
		//ブラックアウト
		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_BLINDOUT_H, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, ENCOUNT_WATER_LOW_WIPE_SYNC, 1, HEAPID_FIELD );
		eew->seq++;
		break;
		
	case ENCOUNT_WATER_LOW_WIPE_WAIT:
		if( WIPE_SYS_EndCheck() ){
			eew->seq++;
		}
		break;
		
	case ENCOUNT_WATER_LOW_END:

		// ラスター破棄
		EF_Laster_Delete( &task_w->laster );
		
		// フィールドHブランクON
		FLDHBLANK_SYS_Start( eew->fsw->fldmap->hblanksys );

		if(eew->end != NULL){
			*(eew->end) = TRUE;		// タスク終了を報告
		}
		ENC_End( eew, tcb );
		WIPE_SetBrightness( WIPE_DISP_SUB, WIPE_FADE_BLACK );
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	水中エンカウント
 *
 *	@param	tcb		タスクワーク
 *	@param	work	ワーク
 *
 *	@return
 */
//-----------------------------------------------------------------------------
void EncountEffect_Field_WaterLevelHigh(TCB_PTR tcb, void* work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	ENCOUNT_WATER_HIGH* task_w = eew->work;

	switch(eew->seq){
	case ENCOUNT_WATER_HIGH_START:
		eew->work = sys_AllocMemory(HEAPID_FIELD, sizeof(ENCOUNT_WATER_HIGH));
		memset( eew->work, 0, sizeof(ENCOUNT_WATER_HIGH) );
		task_w = eew->work;

		// ラスター初期化
		EF_Laster_Init( &task_w->laster, HEAPID_FIELD );

		// フィールドHブランクOFF
		FLDHBLANK_SYS_Stop( eew->fsw->fldmap->hblanksys );

		eew->seq++;
		break;
		
	case ENCOUNT_WATER_HIGH_FLASH_INI:
		EncountFlashTask(MASK_MAIN_DISPLAY, 16, -16,  &eew->wait, 2);
		task_w->wait = ENCOUNT_WATER_HIGH_FLASH_DO_WAIT;
		eew->seq++;
		break;
		
	case ENCOUNT_WATER_HIGH_FLASH_WAIT:
		task_w->wait --;
		if(task_w->wait < 0){
			eew->seq++;
			task_w->wait = ENCOUNT_WATER_HIGH_MOVE_WAIT;
			EF_Laster_Start( &task_w->laster,
					0, 191, ENCOUNT_WATER_HIGH_SIN_ADDR, 
					ENCOUNT_WATER_HIGH_SIN_R, ENCOUNT_WATER_HIGH_LASTER_SP,
					LASTER_SCROLL_MBG0, 0, ENCOUNT_WATER_HIGH_LASTER_TASK_PRI );
		}
		break;
		
	case ENCOUNT_WATER_HIGH_MOVE:

		task_w->wait --;
		if( task_w->wait < 0 ){
			eew->seq++;
		}
		break;
		
	case ENCOUNT_WATER_HIGH_WIPE:

		//ブラックアウト
		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_BLINDOUT_H, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, ENCOUNT_WATER_HIGH_WIPE_SYNC, 1, HEAPID_FIELD );
		eew->seq++;
		break;
		
	case ENCOUNT_WATER_HIGH_WIPE_WAIT:

		if( WIPE_SYS_EndCheck() ){
			eew->seq++;
		}
		break;
		
	case ENCOUNT_WATER_HIGH_END:
		// ラスター破棄
		EF_Laster_Delete( &task_w->laster );

		// フィールドHブランクON
		FLDHBLANK_SYS_Start( eew->fsw->fldmap->hblanksys );

		if(eew->end != NULL){
			*(eew->end) = TRUE;		// タスク終了を報告
		}
		ENC_End( eew, tcb );
		WIPE_SetBrightness( WIPE_DISP_SUB, WIPE_FADE_BLACK );
		break;
	}
}



//----------------------------------------------------------------------------
/**
 *	@brief	ラスタースクロールシステム　VブランクDMA転送リクエストタスク
 *
 *	@param	p_lstscr	ラスタースクロールシステム
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static TCB_PTR EncountEffect_LasterScroll_VDmaTaskReq( ENCOUNT_LASTER_SCROLL* p_lstscr )
{
	return VIntrTCB_Add( EncountEffect_LasterScroll_VDmaTCB,
			p_lstscr, ENCOUNT_EFF_LASTER_SCRLL_VINTR_DMA_SET_TASK_PRI );
}

//----------------------------------------------------------------------------
/**
 *	@brief	DMA転送タスク
 *
 *	@param	tcb		タスクワーク
 *	@param	p_work	ワーク
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void EncountEffect_LasterScroll_VDmaTCB( TCB_PTR tcb, void* p_work )
{
	ENCOUNT_LASTER_SCROLL* p_lstscr = p_work;

	if( p_lstscr->dmacount >= 2 ){
		LASTER_ScrollVBuffSwitch( p_lstscr->p_laster );
		p_lstscr->dmacount = 0;
	}

	LASTER_ScrollVDma( p_lstscr->p_laster );
	p_lstscr->dmacount ++;
}


//----------------------------------------------------------------------------
/**
 *	@brief	ラスター初期化
 *
 *	@param	p_laster	ラスターワーク
 *	@param	heapID		ヒープID
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void EF_Laster_Init( ENCOUNT_LASTER_SCROLL* p_laster, u32 heapID )
{
	p_laster->p_laster = LASTER_ScrollInit( heapID );
	p_laster->dmacount = 0;
	p_laster->VDma = EncountEffect_LasterScroll_VDmaTaskReq( p_laster );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ラスター破棄
 *
 *	@param	p_laster	ラスターワーク
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void EF_Laster_Delete( ENCOUNT_LASTER_SCROLL* p_laster )
{
	TCB_Delete( p_laster->VDma );
	LASTER_ScrollEnd( p_laster->p_laster );
	LASTER_ScrollDelete( p_laster->p_laster );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ラスター開始
 *
 *	@param	p_laster	ワーク
 *	@param	start		ラスタースクロールさせる開始ｙ座標
 *	@param	end			ラスタースクロールさせる終了ｙ座標
 *	@param	add_r		sinカーブテーブル作成時の角度を足していく値(0～65535)
 *	@param	r_w			sinカーブの半径
 *	@param	scr_sp		ラスタースクロールスピード	整数*100した値	マイナス有効
 *	@param	bg_no		ラスターをかけるBGの番号
 *	@param	init_num	ワークを初期化する値
 *	@param	tsk_pri		タスク優先順位
 *
 *	@return	none
 *
 * bg_no
 * 		LASTER_SCROLL_MBG0,
 *		LASTER_SCROLL_MBG1,
 *		LASTER_SCROLL_MBG2,
 *		LASTER_SCROLL_MBG3,
 *		LASTER_SCROLL_SBG0,
 *		LASTER_SCROLL_SBG1,
 *		LASTER_SCROLL_SBG2,
 *		LASTER_SCROLL_SBG3
 */
//-----------------------------------------------------------------------------
static void EF_Laster_Start( ENCOUNT_LASTER_SCROLL* p_laster, u8 start, u8 end, u16 add_r, fx32 r_w, s16 scr_sp, u32 bg_no, u32 init_num, u32 tsk_pri )
{
	LASTER_ScrollStart( p_laster->p_laster,
			start, end, add_r, 
			r_w, scr_sp,
			bg_no, init_num, tsk_pri );
}


//----------------------------------------------------------------------------
/**
 *	@brief	ラスター開始Y位置設定
 *
 *	@param	p_laster
 *	@param	start_y 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void EF_Laster_BuffYSet( ENCOUNT_LASTER_SCROLL* p_laster, s16 start_y )
{
	u32* p_buff;
	u32* p_write;
	int i;
	u32 num;

	p_write = sys_AllocMemory( HEAPID_FIELD, sizeof(u32) * LASTER_SCROLL_BUFF_SIZE );
	memset( p_write, 9, sizeof(u32) * LASTER_SCROLL_BUFF_SIZE );

	p_buff = LASTER_GetScrollWriteBuff( p_laster->p_laster );
	
	for( i=0; i<192; i++ ){
		if( ((-start_y + i) >= 0) &&
			((-start_y + i) < 192 ) ){
			num = p_buff[ (-start_y + i) ];
		}else{
			num = 0;
		}
		p_write[i] = num;
	}

	memcpy( p_buff, p_write, sizeof(u32) * LASTER_SCROLL_BUFF_SIZE );
	
	sys_FreeMemoryEz( p_write );	
}

//----------------------------------------------------------------------------
/**
 *	@brief	ラスターの横座標をdotごとに反転させる
 *
 *	@param	p_laster	ラスターワーク
 *	@param	dot			ドット数
 */
//-----------------------------------------------------------------------------
static void EF_Laster_BuffXFlip( ENCOUNT_LASTER_SCROLL* p_laster, u32 dot )
{
	u32* p_buff;
	int i;
	s16 x;
	
	p_buff = LASTER_GetScrollWriteBuff( p_laster->p_laster );

	for( i=0; i<192; i++ ){
		x = p_buff[i] & 0xffff;
		if( ((i / dot) % 2) == 0 ){
			p_buff[i] = (x & 0xffff);
		}else{
			p_buff[i] = (-x & 0xffff);
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	野生　ダンジョン　レベル低
 *
 *	@param	tcb		タスクワーク
 *	@param	work	ワーク
 *	
 *	@return	none
 */
//-----------------------------------------------------------------------------
void EncountEffect_Field_DanLevelLow(TCB_PTR tcb, void* work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	ENCOUNT_DAN_LOW* task_w = eew->work;
	fx32 distans;
	
	switch( eew->seq ){
	case ENCOUNT_DAN_LOW_START:
		eew->work = sys_AllocMemory(HEAPID_FIELD, sizeof(ENCOUNT_DAN_LOW));
		memset( eew->work, 0, sizeof(ENCOUNT_DAN_LOW) );
		task_w = eew->work;
		eew->seq++;
		break;
		
	case ENCOUNT_DAN_LOW_FLASH_INI:
		EncountFlashTask(MASK_MAIN_DISPLAY, -16, -16,  &eew->wait, 2);
		eew->seq++;
		break;
		
	case ENCOUNT_DAN_LOW_FLASH_WAIT:
		if( eew->wait ){
			eew->seq++;
		}
		break;
		
	case ENCOUNT_DAN_LOW_WIPE:
		// フィールドHブランクON
		FLDHBLANK_SYS_Stop( eew->fsw->fldmap->hblanksys );

		// カメラ近づけながらワイプ
		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_HOLEOUT, WIPE_TYPE_HOLEOUT, WIPE_FADE_BLACK, ENCOUNT_DAN_LOW_MOVE_SYNC, 1, HEAPID_FIELD );

		task_w->camera = eew->fsw->camera_ptr;
		distans = GFC_GetCameraDistance( task_w->camera );

		ENC_AddMoveReqFx( &task_w->camera_dist, distans, distans + ENCOUNT_DAN_LOW_MOVE00_CAMERA_MOVE, ENCOUNT_DAN_LOW_MOVE00_CAMERA_SS, ENCOUNT_DAN_LOW_MOVE_SYNC );

		eew->seq++;
		break;
		
	case ENCOUNT_DAN_LOW_WIPE_WAIT:
		ENC_AddMoveMainFx( &task_w->camera_dist );
		GFC_SetCameraDistance( task_w->camera_dist.x, task_w->camera );
		if( WIPE_SYS_EndCheck() ){
			eew->seq ++;
		}
		break;
		
	case ENCOUNT_DAN_LOW_END:
		// フィールドHブランクON
		FLDHBLANK_SYS_Start( eew->fsw->fldmap->hblanksys );

		if(eew->end != NULL){
			*(eew->end) = TRUE;		// タスク終了を報告
		}
		ENC_End( eew, tcb );
		WIPE_SetBrightness( WIPE_DISP_SUB, WIPE_FADE_BLACK );
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	野生	ダンジョン	レベル高
 *
 *	@param	tcb		タスクワーク
 *	@param	work	ワーク
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
void EncountEffect_Field_DanLevelHigh(TCB_PTR tcb, void* work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	ENCOUNT_DAN_HIGH* task_w = eew->work;
	fx32 distans;
	
	switch( eew->seq ){
	case ENCOUNT_DAN_HIGH_START:
		eew->work = sys_AllocMemory(HEAPID_FIELD, sizeof(ENCOUNT_DAN_HIGH));
		memset( eew->work, 0, sizeof(ENCOUNT_DAN_HIGH) );
		task_w = eew->work;
		eew->seq++;
		break;
		
	case ENCOUNT_DAN_HIGH_FLASH_INI:
		EncountFlashTask(MASK_MAIN_DISPLAY, 16, -16,  &eew->wait, 2);
		eew->seq++;
		break;
		
	case ENCOUNT_DAN_HIGH_FLASH_WAIT:
		if( eew->wait ){
			eew->seq++;
		}
		break;
		
	case ENCOUNT_DAN_HIGH_WIPE:
		// フィールドHブランクON
		FLDHBLANK_SYS_Stop( eew->fsw->fldmap->hblanksys );

		// カメラ近づけながらワイプ
		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_HOLEOUT, WIPE_TYPE_HOLEOUT, WIPE_FADE_BLACK, ENCOUNT_DAN_HIGH_MOVE_SYNC, 1, HEAPID_FIELD );

		task_w->camera = eew->fsw->camera_ptr;
		distans = GFC_GetCameraDistance( task_w->camera );

		ENC_AddMoveReqFx( &task_w->camera_dist, distans, distans + ENCOUNT_DAN_HIGH_MOVE00_CAMERA_MOVE, ENCOUNT_DAN_HIGH_MOVE00_CAMERA_SS, ENCOUNT_DAN_HIGH_MOVE_SYNC );

		eew->seq++;
		break;
		
	case ENCOUNT_DAN_HIGH_WIPE_WAIT:
		ENC_AddMoveMainFx( &task_w->camera_dist );
		GFC_SetCameraDistance( task_w->camera_dist.x, task_w->camera );
		if( WIPE_SYS_EndCheck() ){
			eew->seq ++;
		}
		break;
		
	case ENCOUNT_DAN_HIGH_END:
		// フィールドHブランクON
		FLDHBLANK_SYS_Start( eew->fsw->fldmap->hblanksys );

		if(eew->end != NULL){
			*(eew->end) = TRUE;		// タスク終了を報告
		}
		ENC_End( eew, tcb );
		WIPE_SetBrightness( WIPE_DISP_SUB, WIPE_FADE_BLACK );
		break;
	}
}




//-----------------------------------------------------------------------------
/**
 * 
 *		【フィールド　トレーナーエンカウント】 [ TRAINER ]
 * 
 */
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
//	トレーナー　草　レベル低
//=====================================
// シーケンス
enum{
	ENCOUNT_TR_GRASS_LOW_START,
	ENCOUNT_TR_GRASS_LOW_FLASH_INI,
	ENCOUNT_TR_GRASS_LOW_FLASH_WAIT,
	ENCOUNT_TR_GRASS_LOW_BGSCALE,
	ENCOUNT_TR_GRASS_LOW_BGSCALE_WAIT,
	ENCOUNT_TR_GRASS_LOW_BGCUT,
	ENCOUNT_TR_GRASS_LOW_BGCUTWAIT,
	ENCOUNT_TR_GRASS_LOW_END,
};
// OAMデータ
// アクターデータ数
enum{
	ENCOUNT_TR_GRASS_LOW_CELL_BIG,
	ENCOUNT_TR_GRASS_LOW_CELL_BOTTOM,
	ENCOUNT_TR_GRASS_LOW_CELL_NUM,
};
#define ENCOUNT_TR_GRASS_LOW_CELL_RES_NUM	( 1 )	// リソース数
#define ENCOUNT_TR_GRASS_LOW_CELL_CONT_ID	( 600000 )

// 動さパラメータ	（触ってよい部分）
#define ENCOUNT_TR_GRASS_LOW_BALL_MAT_X		( 128*FX32_ONE )	// 開始座標
#define ENCOUNT_TR_GRASS_LOW_BALL_MAT_Y		( 96*FX32_ONE )	// 開始座標
#define ENCOUNT_TR_GRASS_LOW_SCALE_SYNC		( 10 )		// 拡大シンク数

#define ENCOUNT_TR_GRASS_LOW_SLICE_SYNC		( 6 )		// 画面割りシンク数

#define ENCOUNT_TR_GRASS_LOW_SCALE_S		( FX32_CONST(0.01f) )	// 拡大開始
#define ENCOUNT_TR_GRASS_LOW_SCALE_E		( FX32_CONST(1.0f) )	// 拡大終了
#define ENCOUNT_TR_GRASS_LOW_SCALE_SS		( 2 )		// 初速度

#define ENCOUNT_TR_GRASS_LOW_SLICE_BGSL_SS	( FX32_ONE * 10 )		// 初速度
#define ENCOUNT_TR_GRASS_LOW_SLICE_BGSL_X	( 255 * FX32_ONE )	// 動さｘ
#define ENCOUNT_TR_GRASS_LOW_DOT			( 96 )

#define ENCOUNT_TR_GRASS_LOW_CAMERA_DIST	( -FX32_CONST( 500 ) )	// カメラ移動距離
#define ENCOUNT_TR_GRASS_LOW_CAMERA_SS		( -FX32_CONST( 10 ) )	// カメラ初速度

#define ENCOUNT_TR_GRASS_LOW_ROTA_SCALE		( 0xffff * 1 )	// 拡大時の回転角


//-------------------------------------
//	トレーナー　草　レベル高
//=====================================
// シーケンス
enum{
	ENCOUNT_TR_GRASS_HIGH_START,
	ENCOUNT_TR_GRASS_HIGH_FLASH_INI,
	ENCOUNT_TR_GRASS_HIGH_FLASH_WAIT,
	ENCOUNT_TR_GRASS_HIGH_OAM_MOVE,
	ENCOUNT_TR_GRASS_HIGH_OAM_MOVEWAIT,
	ENCOUNT_TR_GRASS_HIGH_BGCUT,
	ENCOUNT_TR_GRASS_HIGH_BGCUTWAIT,
	ENCOUNT_TR_GRASS_HIGH_END,
};
// OAMデータ
// アクターデータ数
enum{
	ENCOUNT_TR_GRASS_HIGH_CELL_UP,
	ENCOUNT_TR_GRASS_HIGH_CELL_DOWN,
	ENCOUNT_TR_GRASS_HIGH_CELL_NUM,
};
#define ENCOUNT_TR_GRASS_HIGH_CELL_RES_NUM	( 1 )	// リソース数
#define ENCOUNT_TR_GRASS_HIGH_CELL_CONT_ID	( 600000 )

// 動さパラメータ	（触ってよい部分）
#define ENCOUNT_TR_GRASS_HIGH_BALL_MAT_X		( 128*FX32_ONE )// 開始座標
#define ENCOUNT_TR_GRASS_HIGH_BALL_MAT_Y_UP		( 64*FX32_ONE )	// 開始座標
#define ENCOUNT_TR_GRASS_HIGH_BALL_MAT_Y_DOWN	( 128*FX32_ONE )	// 開始座標
#define ENCOUNT_TR_GRASS_HIGH_MOVE_SYNC			( 8 )		// OAM動さシンク数

#define ENCOUNT_TR_GRASS_HIGH_SLICE_SYNC		( 8 )		// 画面割りワイプシンク数

#define ENCOUNT_TR_GRASS_HIGH_BALLMOVE_S		( -192 * FX32_ONE )		// BALL動さ幅
#define ENCOUNT_TR_GRASS_HIGH_BALLMOVE_E		( 192 * FX32_ONE )		// BALL動さ幅

#define ENCOUNT_TR_GRASS_HIGH_SLICE_BGSL_SS_X	( FX32_ONE * 1 )	// 初速度
#define ENCOUNT_TR_GRASS_HIGH_SLICE_BGSL_SS_Y	( FX32_ONE * 1 )	// 初速度

#define ENCOUNT_TR_GRASS_HIGH_CAMERA_DIST	( -FX32_CONST( 500 ) )	// カメラ移動距離
#define ENCOUNT_TR_GRASS_HIGH_CAMERA_SS		( -FX32_CONST( 10 ) )	// カメラ初速度

#define ENCOUNT_TR_GRASS_HIGH_ROTA			( 0xffff *2)			// 回転角


//-------------------------------------
//	トレーナー　水　レベル低
//=====================================
// シーケンス
enum{
	ENCOUNT_TR_WATER_LOW_START,
	ENCOUNT_TR_WATER_LOW_FLASH_INI,
	ENCOUNT_TR_WATER_LOW_FLASH_WAIT,
	ENCOUNT_TR_WATER_LOW_OAM_FADEIN,
	ENCOUNT_TR_WATER_LOW_OAM_FADEINWAIT,
	ENCOUNT_TR_WATER_LOW_OAM_SCALE,
	ENCOUNT_TR_WATER_LOW_OAM_SCALEWAIT,
	ENCOUNT_TR_WATER_LOW_END,
};
// OAMデータ
#define ENCOUNT_TR_WATER_LOW_CELL_NUM		( 2 )
#define ENCOUNT_TR_WATER_LOW_CELL_RES_NUM	( 1 )	// リソース数
#define ENCOUNT_TR_WATER_LOW_CELL_CONT_ID	( 600000 )

// 動さパラメータ	（触ってよい部分）
#define ENCOUNT_TR_WATER_LOW_MOVE_WAIT	( 12 )			// ラスター掛けるタイミング

#define ENCOUNT_TR_WATER_LOW_BALL_MAT_X		( 128*FX32_ONE )// 開始座標
#define ENCOUNT_TR_WATER_LOW_BALL_MAT_Y		( 96*FX32_ONE )	// 開始座標

#define ENCOUNT_TR_WATER_LOW_ALPHA_SYNC		( 8 )		// OAM動さシンク数

#define ENCOUNT_TR_WATER_LOW_SCALE_SYNC		( 8 )		// 拡縮シンク数

#define ENCOUNT_TR_WATER_LOW_ALPHA_S		( 0 )	// α開始
#define ENCOUNT_TR_WATER_LOW_ALPHA_E		( 16 )	// α終了

#define ENCOUNT_TR_WATER_LOW_SCALE_S		( FX32_CONST( 1.0f ) )	// 拡縮開始
#define ENCOUNT_TR_WATER_LOW_SCALE_E		( FX32_CONST( 0.01f ) )	// 拡縮終了

#define ENCOUNT_TR_WATER_LOW_SCALE_SS		( FX32_CONST( 0.1f ) )	// 拡縮初速度
#define ENCOUNT_TR_WATER_LOW_SIN_R		( FX32_CONST( 12 ) )	// 半径
#define ENCOUNT_TR_WATER_LOW_LASTER_SP	( 800 )	// ラスタースピード
#define ENCOUNT_TR_WATER_LOW_SIN_ADDR	( (0xffff/192) * 2 )// ラスター角度スピード

#define ENCOUNT_TR_WATER_LOW_LASTER_TASK_PRI	( TCB_TSK_PRI - 1 )
#define ENCOUNT_TR_WATER_LOW_LASTER_FLIP_DOT	( 2 )

#define ENCOUNT_TR_WATER_LOW_CAMERA_DIST	( -FX32_CONST( 500 ) )	// カメラ移動距離
#define ENCOUNT_TR_WATER_LOW_CAMERA_SS		( -FX32_CONST( 10 ) )	// カメラ初速度
#define ENCOUNT_TR_WATER_LOW_ROTA			( 0xffff )	// 回転角



//-------------------------------------
//	トレーナー　水　レベル高
//=====================================
// シーケンス
enum{
	ENCOUNT_TR_WATER_HIGH_START,
	ENCOUNT_TR_WATER_HIGH_FLASH_INI,
	ENCOUNT_TR_WATER_HIGH_FLASH_WAIT,
	ENCOUNT_TR_WATER_HIGH_OAM_0_S,
	ENCOUNT_TR_WATER_HIGH_OAM_1_S,
	ENCOUNT_TR_WATER_HIGH_OAM_2_S,
	ENCOUNT_TR_WATER_HIGH_OAM_WAIT,
	ENCOUNT_TR_WATER_HIGH_END,
};
// OAMデータ
#define ENCOUNT_TR_WATER_HIGH_CELL_NUM		( 3 )
#define ENCOUNT_TR_WATER_HIGH_CELL_RES_NUM	( 1 )	// リソース数
#define ENCOUNT_TR_WATER_HIGH_CELL_CONT_ID	( 600000 )

// 動さパラメータ	（触ってよい部分）
#define ENCOUNT_TR_WATER_HIGH_MOVE_WAIT		( 14 )		// ラスターがかかるまでのウエイト
#define ENCOUNT_TR_WATER_HIGH_CELLMOVE_SYNC	( 6 )		// 動さシンク数
#define ENCOUNT_TR_WATER_HIGH_CELL0_WAIT	( 6 )		// セル動さウエイト
#define ENCOUNT_TR_WATER_HIGH_CELL1_WAIT	( 4 )		// セル動さウエイト
#define ENCOUNT_TR_WATER_HIGH_CELL2_WAIT	( 2 )		// セル動さウエイト
#define ENCOUNT_TR_WATER_HIGH_CELL0_X		( 43 )
#define ENCOUNT_TR_WATER_HIGH_CELL1_X		( 215 )
#define ENCOUNT_TR_WATER_HIGH_CELL2_X		( 129 )
#define ENCOUNT_TR_WATER_HIGH_CELL_Y		( 231 )		// OBJ動さ開始座標
#define ENCOUNT_TR_WATER_HIGH_BMP_Y			( 312 )		// BMP動さ開始座標
#define ENCOUNT_TR_WATER_HIGH_CELL_Y_E		( -32 )		// OBJ動さ終了座標
#define ENCOUNT_TR_WATER_HIGH_BMP_Y_E		( 0 )		// BMP動さ開始座標

#define ENCOUNT_TR_WATER_HIGH_BMP_FILL_WIDTH	( 86 )	// 塗りつぶし幅
#define ENCOUNT_TR_WATER_HIGH_BMP_FILL_HEIGHT	( 64 )	// 塗りつぶし高さ
#define ENCOUNT_TR_WATER_HIGH_BMP_FILL_PLTT		( 0 )	// パレット０番目を使用する
#define ENCOUNT_TR_WATER_HIGH_BMP_FILL_COL		( 15 )	// 塗りつぶしカラー

#define ENCOUNT_TR_WATER_HIGH_SIN_R		( FX32_CONST( 12 ) )	// 半径
#define ENCOUNT_TR_WATER_HIGH_LASTER_SP	( 800 )	// ラスタースピード
#define ENCOUNT_TR_WATER_HIGH_SIN_ADDR	( (0xffff/192) * 2 )// ラスター角度スピード
#define ENCOUNT_TR_WATER_HIGH_LASTER_TASK_PRI	( TCB_TSK_PRI - 1 )
#define ENCOUNT_TR_WATER_HIGH_LASTER_FLIP_DOT	( 2 )

#define ENCOUNT_TR_WATER_HIGH_CAMERA_SYNC	( 16 )	// カメラ移動距離
#define ENCOUNT_TR_WATER_HIGH_CAMERA_DIST	( -FX32_CONST( 500 ) )	// カメラ移動距離
#define ENCOUNT_TR_WATER_HIGH_CAMERA_SS		( -FX32_CONST( 10 ) )	// カメラ初速度

#define ENCOUNT_TR_WATER_LOW_CELL0_ROTA	( 0xffff * 1 )	// 回転角
#define ENCOUNT_TR_WATER_LOW_CELL1_ROTA	( 0xffff * -1 )	// 回転角
#define ENCOUNT_TR_WATER_LOW_CELL2_ROTA	( 0xffff * 1 )	// 回転角


//-------------------------------------
//	トレーナー　ダン　レベル低
//=====================================
// シーケンス
enum{
	ENCOUNT_TR_DAN_LOW_START,
	ENCOUNT_TR_DAN_LOW_FLASH_INI,
	ENCOUNT_TR_DAN_LOW_FLASH_WAIT,
	ENCOUNT_TR_DAN_LOW_MOVE_Y,
	ENCOUNT_TR_DAN_LOW_MOVE_YWAIT,
	ENCOUNT_TR_DAN_LOW_WIPE,
	ENCOUNT_TR_DAN_LOW_WIPEWAIT,
	ENCOUNT_TR_DAN_LOW_END,
};
// OAMデータ
#define ENCOUNT_TR_DAN_LOW_CELL_NUM		( 1 )
#define ENCOUNT_TR_DAN_LOW_CELL_RES_NUM	( 1 )	// リソース数
#define ENCOUNT_TR_DAN_LOW_CELL_CONT_ID	( 600000 )

// 動さパラメータ	（触ってよい部分）
#define ENCOUNT_TR_DAN_LOW_BALL_MOVE_SYNC	( 12 )	// 移動シンク
#define ENCOUNT_TR_DAN_LOW_BALL_WIPE_SYNC	( 8 )	// ワイプシンク
#define ENCOUNT_TR_DAN_LOW_BALL_MAT_X		( 128*FX32_ONE )// 開始座標
#define ENCOUNT_TR_DAN_LOW_BALL_MAT_Y		( -32*FX32_ONE )	// 開始座標
#define ENCOUNT_TR_DAN_LOW_BALL_MOVE_Y		( 256*FX32_ONE )	// 移動距離
#define ENCOUNT_TR_DAN_LOW_BALL_MOVE_Y_SS	( 2*FX32_ONE )	// 初速度
#define ENCOUNT_TR_DAN_LOW_BALL_SCALE_X_S	( FX32_CONST(0.10f) )// 拡大開始
#define ENCOUNT_TR_DAN_LOW_BALL_SCALE_X_E	( FX32_CONST(2.0f) )// 拡大終了
#define ENCOUNT_TR_DAN_LOW_BALL_SCALE_X_SS	( FX32_CONST(0.0f) )// 初拡大率
#define ENCOUNT_TR_DAN_LOW_BALL_SCALE_Y_S	( FX32_CONST(0.10f) )// 拡大開始
#define ENCOUNT_TR_DAN_LOW_BALL_SCALE_Y_E	( FX32_CONST(2.0f) )// 拡大終了
#define ENCOUNT_TR_DAN_LOW_BALL_SCALE_Y_SS	( FX32_CONST(0.0f)	)// 初拡大率

#define ENCOUNT_TR_DAN_LOW_CAMERA_DIST	( -FX32_CONST( 1000 ) )	// カメラ移動距離
#define ENCOUNT_TR_DAN_LOW_CAMERA_SS		( FX32_CONST( 10 ) )	// カメラ初速度

#define ENCOUNT_TR_DAN_LOW_ROTA		( 0xffff * 1 )	// 回転角

//-------------------------------------
//	トレーナー　ダン　レベル高
//=====================================
// シーケンス
enum{
	ENCOUNT_TR_DAN_HIGH_START,
	ENCOUNT_TR_DAN_HIGH_FLASH_INI,
	ENCOUNT_TR_DAN_HIGH_FLASH_WAIT,
	ENCOUNT_TR_DAN_HIGH_MOVE_0Y,
	ENCOUNT_TR_DAN_HIGH_MOVE_1Y,
	ENCOUNT_TR_DAN_HIGH_MOVE_2Y,
	ENCOUNT_TR_DAN_HIGH_MOVE_YWAIT,
	ENCOUNT_TR_DAN_HIGH_WIPE,
	ENCOUNT_TR_DAN_HIGH_WIPE_WAIT,
	ENCOUNT_TR_DAN_HIGH_END,
};
// OAMデータ
#define ENCOUNT_TR_DAN_HIGH_CELL_NUM		( 3 )
#define ENCOUNT_TR_DAN_HIGH_CELL_RES_NUM	( 1 )	// リソース数
#define ENCOUNT_TR_DAN_HIGH_CELL_CONT_ID	( 600000 )

#define ENCOUNT_TR_DAN_HIGH_BLOCK_FILLCOLOR	( 15 )	// 塗りつぶし

// 動さパラメータ	（触ってよい部分）
#define ENCOUNT_TR_DAN_HIGH_BALL_MOVE_SYNC	( 5 )	// 移動シンク
#define ENCOUNT_TR_DAN_HIGH_BLOCK_SYNC		( 1 )	// ワイプシンク
#define ENCOUNT_TR_DAN_HIGH_BLOCK_START_SYNC	( 1 )	// ワイプシンク

#define ENCOUNT_TR_DAN_HIGH_BALL_WAIT_0		( 0 )// 開始待ち
#define ENCOUNT_TR_DAN_HIGH_BALL_WAIT_1		( 1 )// 開始待ち
#define ENCOUNT_TR_DAN_HIGH_BALL_WAIT_2		( 3 )// 開始待ち

#define ENCOUNT_TR_DAN_HIGH_BALL_MAT_0X		( 128 )// 開始座標
#define ENCOUNT_TR_DAN_HIGH_BALL_MAT_1X		( 208 )// 開始座標
#define ENCOUNT_TR_DAN_HIGH_BALL_MAT_2X		( 48 )// 開始座標
#define ENCOUNT_TR_DAN_HIGH_BALL_MAT_Y		( -32 )// 開始座標
#define ENCOUNT_TR_DAN_HIGH_BALL_MOVE_Y		( 224 )	// 移動距離

#define ENCOUNT_TR_DAN_HIGH_CAMERA_SYNC	( 64 )	// カメラ移動シンク数48
#define ENCOUNT_TR_DAN_HIGH_CAMERA_DIST	( -FX32_CONST( 1000 ) )	// カメラ移動距離
#define ENCOUNT_TR_DAN_HIGH_CAMERA_SS		( FX32_CONST( 10 ) )	// カメラ初速度

#define ENCOUNT_TR_DAN_HIGH_BALL0_ROTA	( 0xffff * 1 )
#define ENCOUNT_TR_DAN_HIGH_BALL1_ROTA	( 0xffff * -1 )
#define ENCOUNT_TR_DAN_HIGH_BALL2_ROTA	( 0xffff * 1 )


//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
//	トレーナー　草　レベル低
//=====================================
typedef struct{
	ENC_ADDMOVE_WORK_FX oam_scale;
	ENC_MOVE_WORK	oam_rota;
	ENC_ADDMOVE_WORK_FX oam_slice;
	ENC_HB_BG_SLICE* p_slice;

	ENC_CLACT_SYS	clact_sys;
	ENC_CLACT_RES_WORK clact_res;
	CLACT_WORK_PTR clact[ ENCOUNT_TR_GRASS_LOW_CELL_NUM ];

	GF_CAMERA_PTR	camera;				// カメラデータ
	ENC_ADDMOVE_WORK_FX camera_dist;

	s32 wait;
} ENCOUNT_TR_GRASS_LOW;

//-------------------------------------
//	トレーナー　草　レベル高
//=====================================
typedef struct{
	ENC_MOVE_WORK_FX oam_slice;
	ENC_MOVE_WORK	oam_rota;
	ENC_HB_BG_WND_SLANT* p_slice;

	ENC_CLACT_SYS	clact_sys;
	ENC_CLACT_RES_WORK clact_res;
	CLACT_WORK_PTR clact[ ENCOUNT_TR_GRASS_HIGH_CELL_NUM ];

	GF_CAMERA_PTR	camera;				// カメラデータ
	ENC_ADDMOVE_WORK_FX camera_dist;

	s32 wait;
} ENCOUNT_TR_GRASS_HIGH;

//-------------------------------------
//	トレーナー　水　レベル低
//=====================================
typedef struct{
	ENC_ADDMOVE_WORK_FX oam_scale;
	ENC_MOVE_WORK		oam_alpha;
	ENC_MOVE_WORK	oam_rota;

	ENCOUNT_LASTER_SCROLL laster;
	BOOL laster_flg;

	ENC_CLACT_SYS	clact_sys;
	ENC_CLACT_RES_WORK clact_res;
	CLACT_WORK_PTR clact[ENCOUNT_TR_WATER_LOW_CELL_NUM];

	GF_CAMERA_PTR	camera;				// カメラデータ
	ENC_ADDMOVE_WORK_FX camera_dist;

	s32 wait;
} ENCOUNT_TR_WATER_LOW;

//-------------------------------------
//	トレーナー　水　レベル高
//=====================================
typedef struct{
	ENC_CLACT_SYS	clact_sys;
	ENC_CLACT_RES_WORK clact_res;
	CLACT_WORK_PTR clact[ ENCOUNT_TR_WATER_HIGH_CELL_NUM ];
	ENC_MOVE_WORK obj_move[ ENCOUNT_TR_WATER_HIGH_CELL_NUM ];
	ENC_MOVE_WORK	oam_rota[ ENCOUNT_TR_WATER_HIGH_CELL_NUM ];
	ENC_BMP_FILL_OBJ* p_bmp_fill[ ENCOUNT_TR_WATER_HIGH_CELL_NUM ];
	BOOL		obj_move_flg[ ENCOUNT_TR_WATER_HIGH_CELL_NUM ];
	GF_BGL_BMPWIN* p_bmp;

	ENCOUNT_LASTER_SCROLL laster;
	BOOL laster_flg;

	GF_CAMERA_PTR	camera;				// カメラデータ
	ENC_ADDMOVE_WORK_FX camera_dist;

	s32 wait;
} ENCOUNT_TR_WATER_HIGH;

//-------------------------------------
//	トレーナー　ダン　レベル低
//=====================================
typedef struct{
	ENC_ADDMOVE_WORK_FX		oam_move;
	ENC_ADDMOVE_WORK_FX		oam_scale_x;
	ENC_ADDMOVE_WORK_FX		oam_scale_y;
	ENC_MOVE_WORK			oam_rota;

	ENC_CLACT_SYS	clact_sys;
	ENC_CLACT_RES_WORK clact_res;
	CLACT_WORK_PTR clact;

	GF_CAMERA_PTR	camera;				// カメラデータ
	ENC_ADDMOVE_WORK_FX camera_dist;

	s32 wait;
} ENCOUNT_TR_DAN_LOW;

//-------------------------------------
//	トレーナー　ダン　レベル高
//=====================================
typedef struct{
	ENC_CLACT_SYS	clact_sys;
	ENC_CLACT_RES_WORK clact_res;
	CLACT_WORK_PTR clact[ ENCOUNT_TR_DAN_HIGH_CELL_NUM ];
	ENC_MOVE_WORK oam_move[ ENCOUNT_TR_DAN_HIGH_CELL_NUM ];
	ENC_MOVE_WORK oam_rota[ ENCOUNT_TR_DAN_HIGH_CELL_NUM ];
	BOOL oam_move_flg[ ENCOUNT_TR_DAN_HIGH_CELL_NUM ];
	
	GF_BGL_BMPWIN* p_bmp;
	ENC_BMP_FILL_BLOCK_MOVE* p_block;

	GF_CAMERA_PTR	camera;				// カメラデータ
	ENC_ADDMOVE_WORK_FX camera_dist;

	s16 wait;
} ENCOUNT_TR_DAN_HIGH;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナー　草　レベル低
 */
//-----------------------------------------------------------------------------
void EncountEffect_Trainer_GrassLevelLow(TCB_PTR tcb, void* work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	ENCOUNT_TR_GRASS_LOW* task_w = eew->work;
	BOOL result;
	fx32 dist;
	int i;
	VecFx32 scale;
	u16 last_rota;

	switch( eew->seq ){
	case ENCOUNT_TR_GRASS_LOW_START:
		eew->work = sys_AllocMemory( HEAPID_FIELD, sizeof(ENCOUNT_TR_GRASS_LOW) );	
		memset( eew->work, 0, sizeof(ENCOUNT_TR_GRASS_LOW) );
		task_w = eew->work;

		// カメラポインタ取得
		task_w->camera = eew->fsw->camera_ptr;

		// スライスワーク作成
		task_w->p_slice = ENC_BG_Slice_Alloc();

		// セルアクターセット作成
		ENC_CLACT_Init( &task_w->clact_sys, ENCOUNT_TR_GRASS_LOW_CELL_NUM, ENCOUNT_TR_GRASS_LOW_CELL_RES_NUM );

		// グラフィック設定
		ENC_CLACT_ResLoadEasy(
				eew->p_handle,
				&task_w->clact_sys,
				&task_w->clact_res,
				NARC_field_encounteffect_monsterball_NCLR, 1,
				NARC_field_encounteffect_monstarballbig_oam_NCGR,
				NARC_field_encounteffect_monsterballbig_oam_NCER,
				NARC_field_encounteffect_monsterballbig_oam_NANR,
				ENCOUNT_TR_GRASS_LOW_CELL_CONT_ID
				);

		// アクター登録
		for( i=0; i<ENCOUNT_TR_GRASS_LOW_CELL_NUM; i++ ){
			task_w->clact[i] = ENC_CLACT_Add( 
					&task_w->clact_sys, &task_w->clact_res,
					ENCOUNT_TR_GRASS_LOW_BALL_MAT_X, 
					ENCOUNT_TR_GRASS_LOW_BALL_MAT_Y,0, 0);
			CLACT_SetDrawFlag( task_w->clact[i], FALSE );
			CLACT_DrawPriorityChg( task_w->clact[i], i*2 );
		}
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

		eew->seq ++;
		break;
		
	case ENCOUNT_TR_GRASS_LOW_FLASH_INI:

		EncountFlashTask(MASK_MAIN_DISPLAY, -16, -16,  &eew->wait, 2);
		eew->seq++;
		break;
		
	case ENCOUNT_TR_GRASS_LOW_FLASH_WAIT:
		if( eew->wait ){
			eew->seq++;
		}
		break;
		
	case ENCOUNT_TR_GRASS_LOW_BGSCALE:
		ENC_AddMoveReqFx( &task_w->oam_scale, 
				ENCOUNT_TR_GRASS_LOW_SCALE_S,
				ENCOUNT_TR_GRASS_LOW_SCALE_E,
				ENCOUNT_TR_GRASS_LOW_SCALE_SS,
				ENCOUNT_TR_GRASS_LOW_SCALE_SYNC );

		scale = ENC_MakeVec( task_w->oam_scale.x, 
				task_w->oam_scale.x, task_w->oam_scale.x );
		for( i=0; i<ENCOUNT_TR_GRASS_LOW_CELL_NUM; i++ ){
			CLACT_SetDrawFlag(
					task_w->clact[i],
					TRUE );
			CLACT_SetAffineParam(
					task_w->clact[i], 
					CLACT_AFFINE_DOUBLE );
			CLACT_SetScale( 
					task_w->clact[i], 
					&scale );
		}
		// 回転動さ
		ENC_MoveReq( &task_w->oam_rota, 
				0,
				ENCOUNT_TR_GRASS_LOW_ROTA_SCALE, 
				ENCOUNT_TR_GRASS_LOW_SCALE_SYNC );
		eew->seq++;
		break;

	case ENCOUNT_TR_GRASS_LOW_BGSCALE_WAIT:
		result = ENC_AddMoveMainFx( &task_w->oam_scale );
		scale = ENC_MakeVec( task_w->oam_scale.x, 
					task_w->oam_scale.x, task_w->oam_scale.x );

		// １つ前の回転角
		last_rota = task_w->oam_rota.x;
		
		ENC_MoveMain( &task_w->oam_rota );

		for( i=0; i<ENCOUNT_TR_GRASS_LOW_CELL_NUM; i++ ){
			CLACT_SetScale( 
					task_w->clact[i], 
					&scale );
		}
		CLACT_SetRotation( 
				task_w->clact[ENCOUNT_TR_GRASS_LOW_CELL_BIG],
				0xffff & task_w->oam_rota.x );

		CLACT_SetRotation( 
				task_w->clact[ENCOUNT_TR_GRASS_LOW_CELL_BOTTOM],
				0xffff & (task_w->oam_rota.x - 0x100) );	// 黒い点防止の為-0x100
  
		if( result == TRUE ){
			eew->seq ++;
		}
		break;
		
	case ENCOUNT_TR_GRASS_LOW_BGCUT:
		// 動作開始
		ENC_BG_Slice_Start( eew, task_w->p_slice, 
				ENCOUNT_TR_GRASS_LOW_DOT,
				ENCOUNT_TR_GRASS_LOW_SLICE_SYNC, 
				0, ENCOUNT_TR_GRASS_LOW_SLICE_BGSL_X,
				ENCOUNT_TR_GRASS_LOW_SLICE_BGSL_SS );

		// カットグラフィックにOAMを変更
		CLACT_AnmChg( 
				task_w->clact[ENCOUNT_TR_GRASS_LOW_CELL_BIG], 
				1 );
		CLACT_AnmChg( 
				task_w->clact[ENCOUNT_TR_GRASS_LOW_CELL_BOTTOM], 
				2 );

		// OAMスライド動さパラメータ作成
		ENC_AddMoveReqFx( &task_w->oam_slice, 
				0,
				ENCOUNT_TR_GRASS_LOW_SLICE_BGSL_X,
				ENCOUNT_TR_GRASS_LOW_SLICE_BGSL_SS,
				ENCOUNT_TR_GRASS_LOW_SLICE_SYNC );
		
		// カメラ動さ作成
		dist = GFC_GetCameraDistance( task_w->camera );
		ENC_AddMoveReqFx( &task_w->camera_dist,
				dist,
				dist + ENCOUNT_TR_GRASS_LOW_CAMERA_DIST,
				ENCOUNT_TR_GRASS_LOW_CAMERA_SS,
				ENCOUNT_TR_GRASS_LOW_SLICE_SYNC );
		
		CLACT_SetRotation( 
				task_w->clact[ENCOUNT_TR_GRASS_LOW_CELL_BIG],
				0xffff & 0 );
		CLACT_SetRotation( 
				task_w->clact[ENCOUNT_TR_GRASS_LOW_CELL_BOTTOM],
				0xffff & 0 );

		eew->seq ++;
		break;
		
	case ENCOUNT_TR_GRASS_LOW_BGCUTWAIT:
		// OAM動さ
		result = ENC_AddMoveMainFx( &task_w->oam_slice );
		{
			VecFx32 mat = ENC_MakeVec( ENCOUNT_TR_GRASS_LOW_BALL_MAT_X - task_w->oam_slice.x, 
					ENCOUNT_TR_GRASS_LOW_BALL_MAT_Y, 0 );
			VecFx32 ret_mat = ENC_MakeVec( ENCOUNT_TR_GRASS_LOW_BALL_MAT_X + task_w->oam_slice.x, 
					ENCOUNT_TR_GRASS_LOW_BALL_MAT_Y, 0 );
			CLACT_SetMatrix( 
					task_w->clact[ENCOUNT_TR_GRASS_LOW_CELL_BIG], 
					&mat );
			CLACT_SetMatrix( 
					task_w->clact[ENCOUNT_TR_GRASS_LOW_CELL_BOTTOM], 
					&ret_mat );
		}

		// カメラ動さ
		ENC_AddMoveMainFx( &task_w->camera_dist );
		GFC_SetCameraDistance( task_w->camera_dist.x, task_w->camera );

		if( ENC_HBlankEndCheck( eew ) ){
			eew->seq ++;
		}
		break;
		
	case ENCOUNT_TR_GRASS_LOW_END:
		WIPE_SetBrightnessFadeOut( WIPE_FADE_BLACK );
		if(eew->end != NULL){
			*(eew->end) = TRUE;		// タスク終了を報告
		}
		// OAM破棄
		{
			int i;
			for( i=0; i<ENCOUNT_TR_GRASS_LOW_CELL_NUM; i++ ){
				CLACT_Delete( task_w->clact[i] );
			}
		}
		ENC_CLACT_ResDeleteEasy( &task_w->clact_sys, &task_w->clact_res );
		ENC_CLACT_Delete( &task_w->clact_sys );
		ENC_BG_Slice_Delete( task_w->p_slice );
		ENC_End( eew, tcb );
		break;
	}

	if( eew->seq != ENCOUNT_TR_GRASS_LOW_END ){
		CLACT_Draw( task_w->clact_sys.cas );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナー　草　レベル高
 */
//-----------------------------------------------------------------------------
void EncountEffect_Trainer_GrassLevelHigh(TCB_PTR tcb, void* work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	ENCOUNT_TR_GRASS_HIGH* task_w = eew->work;
	BOOL result;
	fx32 dist;

	switch( eew->seq ){
	case ENCOUNT_TR_GRASS_HIGH_START:
		eew->work = sys_AllocMemory( HEAPID_FIELD, sizeof(ENCOUNT_TR_GRASS_HIGH) );	
		memset( eew->work, 0, sizeof(ENCOUNT_TR_GRASS_HIGH) );
		task_w = eew->work;

		// カメラポインタ取得
		task_w->camera = eew->fsw->camera_ptr;

		// スライスワーク作成
		task_w->p_slice = ENC_BG_WndSlant_Alloc();

		// セルアクターセット作成
		ENC_CLACT_Init( &task_w->clact_sys, ENCOUNT_TR_GRASS_HIGH_CELL_NUM, ENCOUNT_TR_GRASS_HIGH_CELL_RES_NUM );

		// グラフィック設定
		ENC_CLACT_ResLoadEasy(
				eew->p_handle,
				&task_w->clact_sys,
				&task_w->clact_res,
				NARC_field_encounteffect_monsterball_NCLR, 1,
				NARC_field_encounteffect_monstaerball_oam_NCGR,
				NARC_field_encounteffect_monsterball_oam_NCER,
				NARC_field_encounteffect_monsterball_oam_NANR,
				ENCOUNT_TR_GRASS_HIGH_CELL_CONT_ID
				);

		// アクター登録
		{
			int i;
			for( i=0; i<ENCOUNT_TR_GRASS_HIGH_CELL_NUM; i++ ){
				task_w->clact[i] = ENC_CLACT_Add( 
						&task_w->clact_sys, &task_w->clact_res,
						ENCOUNT_TR_GRASS_HIGH_BALL_MAT_X, 
						0,0, 0);
				CLACT_SetDrawFlag( task_w->clact[i], FALSE );
				CLACT_SetAffineParam( task_w->clact[i], CLACT_AFFINE_DOUBLE );
			}
		}
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

		eew->seq ++;
		break;
		
	case ENCOUNT_TR_GRASS_HIGH_FLASH_INI:

		EncountFlashTask(MASK_MAIN_DISPLAY, 16, -16,  &eew->wait, 2);
		eew->seq++;
		break;
		
	case ENCOUNT_TR_GRASS_HIGH_FLASH_WAIT:
		if( eew->wait ){
			eew->seq++;
		}
		break;
		
	case ENCOUNT_TR_GRASS_HIGH_OAM_MOVE:
		ENC_MoveReqFx( &task_w->oam_slice, 
				ENCOUNT_TR_GRASS_HIGH_BALLMOVE_S,
				ENCOUNT_TR_GRASS_HIGH_BALLMOVE_E,
				ENCOUNT_TR_GRASS_HIGH_MOVE_SYNC );
		CLACT_SetDrawFlag(
				task_w->clact[ENCOUNT_TR_GRASS_HIGH_CELL_UP],
				TRUE );
		CLACT_SetDrawFlag(
				task_w->clact[ENCOUNT_TR_GRASS_HIGH_CELL_DOWN],
				TRUE );

		{
			VecFx32 mat = ENC_MakeVec( ENCOUNT_TR_GRASS_HIGH_BALL_MAT_X - task_w->oam_slice.x, 
					ENCOUNT_TR_GRASS_HIGH_BALL_MAT_Y_UP, 0 );
			VecFx32 ret_mat = ENC_MakeVec( ENCOUNT_TR_GRASS_HIGH_BALL_MAT_X + task_w->oam_slice.x, 
					ENCOUNT_TR_GRASS_HIGH_BALL_MAT_Y_DOWN, 0 );
			CLACT_SetMatrix( 
					task_w->clact[ENCOUNT_TR_GRASS_HIGH_CELL_UP], 
					&mat );
			CLACT_SetMatrix( 
					task_w->clact[ENCOUNT_TR_GRASS_HIGH_CELL_DOWN], 
					&ret_mat );
		}

		// 回転動さ
		ENC_MoveReq( &task_w->oam_rota,
				0, ENCOUNT_TR_GRASS_HIGH_ROTA,
				ENCOUNT_TR_GRASS_HIGH_MOVE_SYNC );
		
		eew->seq++;
		break;

	case ENCOUNT_TR_GRASS_HIGH_OAM_MOVEWAIT:
		result = ENC_MoveMainFx( &task_w->oam_slice );

		{
			VecFx32 mat = ENC_MakeVec( ENCOUNT_TR_GRASS_HIGH_BALL_MAT_X - task_w->oam_slice.x, 
					ENCOUNT_TR_GRASS_HIGH_BALL_MAT_Y_UP, 0 );
			VecFx32 ret_mat = ENC_MakeVec( ENCOUNT_TR_GRASS_HIGH_BALL_MAT_X + task_w->oam_slice.x, 
					ENCOUNT_TR_GRASS_HIGH_BALL_MAT_Y_DOWN, 0 );
			CLACT_SetMatrix( 
					task_w->clact[ENCOUNT_TR_GRASS_HIGH_CELL_UP], 
					&mat );
			CLACT_SetMatrix( 
					task_w->clact[ENCOUNT_TR_GRASS_HIGH_CELL_DOWN], 
					&ret_mat );
		}

		ENC_MoveMain( &task_w->oam_rota );
		CLACT_SetRotation( 
				task_w->clact[ENCOUNT_TR_GRASS_HIGH_CELL_UP],
				task_w->oam_rota.x );
		CLACT_SetRotation( 
				task_w->clact[ENCOUNT_TR_GRASS_HIGH_CELL_DOWN],
				-task_w->oam_rota.x );

		if( result == TRUE ){
			eew->seq ++;
		}
		break;
		
	case ENCOUNT_TR_GRASS_HIGH_BGCUT:
		// 動作開始
		ENC_BG_WndSlant_Start( eew, task_w->p_slice, 
				 ENCOUNT_TR_GRASS_HIGH_SLICE_SYNC,
				 ENCOUNT_TR_GRASS_HIGH_SLICE_BGSL_SS_X,
				 ENCOUNT_TR_GRASS_HIGH_SLICE_BGSL_SS_Y);

		// カメラ動さ作成
		dist = GFC_GetCameraDistance( task_w->camera );
		ENC_AddMoveReqFx( &task_w->camera_dist,
				dist,
				dist + ENCOUNT_TR_GRASS_HIGH_CAMERA_DIST,
				ENCOUNT_TR_GRASS_HIGH_CAMERA_SS,
				ENCOUNT_TR_GRASS_HIGH_SLICE_SYNC );

		eew->seq ++;
		break;
		
	case ENCOUNT_TR_GRASS_HIGH_BGCUTWAIT:

		// カメラ動さ
		ENC_AddMoveMainFx( &task_w->camera_dist );
		GFC_SetCameraDistance( task_w->camera_dist.x, task_w->camera );

		if( ENC_HBlankEndCheck( eew ) == TRUE ){
			eew->seq ++;
		}
		break;
		
	case ENCOUNT_TR_GRASS_HIGH_END:
		WIPE_SetBrightnessFadeOut( WIPE_FADE_BLACK );
		if(eew->end != NULL){
			*(eew->end) = TRUE;		// タスク終了を報告
		}
		// OAM破棄
		{
			int i;
			for( i=0; i<ENCOUNT_TR_GRASS_HIGH_CELL_NUM; i++ ){
				CLACT_Delete( task_w->clact[i] );
			}
		}
		ENC_CLACT_ResDeleteEasy( &task_w->clact_sys, &task_w->clact_res );
		ENC_CLACT_Delete( &task_w->clact_sys );
		ENC_BG_WndSlant_Delete( task_w->p_slice );
		ENC_End( eew, tcb );
		break;
	}

	if( eew->seq != ENCOUNT_TR_GRASS_HIGH_END ){
		CLACT_Draw( task_w->clact_sys.cas );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナー　水　レベル低
 */
//-----------------------------------------------------------------------------
void EncountEffect_Trainer_WaterLevelLow(TCB_PTR tcb, void* work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	ENCOUNT_TR_WATER_LOW* task_w = eew->work;
	BOOL result;
	BOOL result2;
	fx32 dist;
	int i;
	u16 last_rota;


	switch( eew->seq ){
	case ENCOUNT_TR_WATER_LOW_START:
		eew->work = sys_AllocMemory( HEAPID_FIELD, sizeof(ENCOUNT_TR_WATER_LOW) );	
		memset( eew->work, 0, sizeof(ENCOUNT_TR_WATER_LOW) );
		task_w = eew->work;

		// カメラポインタ取得
		task_w->camera = eew->fsw->camera_ptr;

		// ラスター初期化
		EF_Laster_Init( &task_w->laster, HEAPID_FIELD );
		task_w->wait = ENCOUNT_TR_WATER_LOW_MOVE_WAIT;

		// セルアクターセット作成
		ENC_CLACT_Init( &task_w->clact_sys, ENCOUNT_TR_WATER_LOW_CELL_NUM, ENCOUNT_TR_WATER_LOW_CELL_RES_NUM );

		// グラフィック設定
		ENC_CLACT_ResLoadEasy(
				eew->p_handle,
				&task_w->clact_sys,
				&task_w->clact_res,
				NARC_field_encounteffect_monsterball_NCLR, 1,
				NARC_field_encounteffect_monstarballbig_oam_NCGR,
				NARC_field_encounteffect_monsterballbig_oam_NCER,
				NARC_field_encounteffect_monsterballbig_oam_NANR,
				ENCOUNT_TR_WATER_LOW_CELL_CONT_ID
				);

		// アクター登録
		for( i=0; i<ENCOUNT_TR_WATER_LOW_CELL_NUM; i++ ){
			task_w->clact[i] = ENC_CLACT_Add( 
					&task_w->clact_sys, &task_w->clact_res,
					ENCOUNT_TR_WATER_LOW_BALL_MAT_X, 
					ENCOUNT_TR_WATER_LOW_BALL_MAT_Y, 0, 0);
			CLACT_SetDrawFlag( task_w->clact[i], FALSE );
			CLACT_DrawPriorityChg( task_w->clact[i], i );
		}
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

		eew->seq ++;
		break;
		
	case ENCOUNT_TR_WATER_LOW_FLASH_INI:

		EncountFlashTask(MASK_MAIN_DISPLAY, -16, -16,  &eew->wait, 2);
		eew->seq++;
		break;
		
	case ENCOUNT_TR_WATER_LOW_FLASH_WAIT:
		task_w->wait--;
		if( task_w->wait == 0 ){
			EF_Laster_Start( &task_w->laster,
					0, 191, ENCOUNT_TR_WATER_LOW_SIN_ADDR, 
					ENCOUNT_TR_WATER_LOW_SIN_R, ENCOUNT_TR_WATER_LOW_LASTER_SP,
					LASTER_SCROLL_MBG0, 0, ENCOUNT_TR_WATER_LOW_LASTER_TASK_PRI );
			task_w->laster_flg = TRUE;
		}
		if( eew->wait ){
			eew->seq++;
		}
		break;

	case ENCOUNT_TR_WATER_LOW_OAM_FADEIN:
		ENC_MoveReq( &task_w->oam_alpha, 
				ENCOUNT_TR_WATER_LOW_ALPHA_S, ENCOUNT_TR_WATER_LOW_ALPHA_E,
				ENCOUNT_TR_WATER_LOW_ALPHA_SYNC );
		G2_SetBlendAlpha( GX_BLEND_PLANEMASK_NONE, 
				GX_BLEND_PLANEMASK_BG0|GX_BLEND_PLANEMASK_BG1|GX_BLEND_PLANEMASK_BG2|GX_BLEND_PLANEMASK_BG3,
				task_w->oam_alpha.x, 16 - task_w->oam_alpha.x );

		for( i=0; i<ENCOUNT_TR_WATER_LOW_CELL_NUM; i++ ){
			CLACT_SetDrawFlag(
					task_w->clact[i],
					TRUE );

			CLACT_SetAffineParam(
					task_w->clact[i], 
					CLACT_AFFINE_DOUBLE );
			CLACT_ObjModeSet( task_w->clact[i], GX_OAM_MODE_XLU );
		}

		// 回転
		ENC_MoveReq( &task_w->oam_rota,
				0,
				ENCOUNT_TR_WATER_LOW_ROTA,
				ENCOUNT_TR_WATER_LOW_SCALE_SYNC );

		eew->seq++;
		break;
		
	case ENCOUNT_TR_WATER_LOW_OAM_FADEINWAIT:
		result = ENC_MoveMain( &task_w->oam_alpha );
		G2_ChangeBlendAlpha( task_w->oam_alpha.x, 16 - task_w->oam_alpha.x );

		last_rota = task_w->oam_rota.x;
		result2 = ENC_MoveMain( &task_w->oam_rota );

		if( result2 == FALSE ){
			CLACT_SetRotation( task_w->clact[0], 0xffff & task_w->oam_rota.x );
			CLACT_SetRotation( task_w->clact[1], 0xffff & last_rota );
		}else{
			CLACT_SetRotation( task_w->clact[0], 0 );
			CLACT_SetRotation( task_w->clact[1], 0 );
		}

		if( result == TRUE ){
			G2_BlendNone();
			for( i=0; i<ENCOUNT_TR_WATER_LOW_CELL_NUM; i++ ){
				CLACT_ObjModeSet( task_w->clact[i], GX_OAM_MODE_NORMAL );
			}
			eew->seq++;
		}
		break;

	case ENCOUNT_TR_WATER_LOW_OAM_SCALE:

		ENC_AddMoveReqFx( &task_w->oam_scale, 
				ENCOUNT_TR_WATER_LOW_SCALE_S,
				ENCOUNT_TR_WATER_LOW_SCALE_E,
				ENCOUNT_TR_WATER_LOW_SCALE_SS,
				ENCOUNT_TR_WATER_LOW_SCALE_SYNC );

		{
			VecFx32 scale = ENC_MakeVec( task_w->oam_scale.x, 
					task_w->oam_scale.x, task_w->oam_scale.x );
			for( i=0; i<ENCOUNT_TR_WATER_LOW_CELL_NUM; i++ ){
				CLACT_SetScale( 
						task_w->clact[i], 
						&scale );
			}
		}


		// カメラ動さ作成
		dist = GFC_GetCameraDistance( task_w->camera );
		ENC_AddMoveReqFx( &task_w->camera_dist,
				dist,
				dist + ENCOUNT_TR_WATER_LOW_CAMERA_DIST,
				ENCOUNT_TR_WATER_LOW_CAMERA_SS,
				ENCOUNT_TR_WATER_LOW_SCALE_SYNC );

		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_BOXOUT_INSIDE, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, ENCOUNT_TR_WATER_LOW_SCALE_SYNC, 1, HEAPID_FIELD );
		eew->seq++;
		break;
		
	case ENCOUNT_TR_WATER_LOW_OAM_SCALEWAIT:
		result = ENC_AddMoveMainFx( &task_w->oam_scale );
		{
			VecFx32 scale = ENC_MakeVec( task_w->oam_scale.x, 
					task_w->oam_scale.x, task_w->oam_scale.x );
			for( i=0; i<ENCOUNT_TR_WATER_LOW_CELL_NUM; i++ ){
				CLACT_SetScale( 
						task_w->clact[i], 
						&scale );
			}
		}

		// カメラ動さ
		ENC_AddMoveMainFx( &task_w->camera_dist );
		GFC_SetCameraDistance( task_w->camera_dist.x, task_w->camera );

		if( (result == TRUE) && ( WIPE_SYS_EndCheck() == TRUE ) ){
			eew->seq ++;
		}
		break;
		
	case ENCOUNT_TR_WATER_LOW_END:
		WIPE_SetBrightness( WIPE_DISP_SUB, WIPE_FADE_BLACK );
		if(eew->end != NULL){
			*(eew->end) = TRUE;		// タスク終了を報告
		}

		// ラスター破棄
		EF_Laster_Delete( &task_w->laster );
		task_w->laster_flg = FALSE;
		
		// OAM破棄
		for( i=0; i<ENCOUNT_TR_WATER_LOW_CELL_NUM; i++ ){
			CLACT_Delete( task_w->clact[i] );
		}
		ENC_CLACT_ResDeleteEasy( &task_w->clact_sys, &task_w->clact_res );
		ENC_CLACT_Delete( &task_w->clact_sys );
		ENC_End( eew, tcb );
		return ;
	}

	// ラスターフリップ機能 
	if( task_w->laster_flg == TRUE ){
		EF_Laster_BuffXFlip( &task_w->laster, ENCOUNT_TR_WATER_LOW_LASTER_FLIP_DOT );
	}


	if( eew->seq != ENCOUNT_TR_WATER_LOW_END ){
		CLACT_Draw( task_w->clact_sys.cas );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナー　水　レベル高
 */
//-----------------------------------------------------------------------------
void EncountEffect_Trainer_WaterLevelHigh(TCB_PTR tcb, void* work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	ENCOUNT_TR_WATER_HIGH* task_w = eew->work;
	BOOL result;
	int i;
	VecFx32 matrix;
	fx32 dist;


	switch( eew->seq ){
	case ENCOUNT_TR_WATER_HIGH_START:
		eew->work = sys_AllocMemory( HEAPID_FIELD, sizeof(ENCOUNT_TR_WATER_HIGH) );	
		memset( eew->work, 0, sizeof(ENCOUNT_TR_WATER_HIGH) );
		task_w = eew->work;

		// カメラポインタ取得
		task_w->camera = eew->fsw->camera_ptr;

		// ラスター初期化
		EF_Laster_Init( &task_w->laster, HEAPID_FIELD );
		task_w->wait = ENCOUNT_TR_WATER_HIGH_MOVE_WAIT;

		// セルアクターセット作成
		ENC_CLACT_Init( &task_w->clact_sys, ENCOUNT_TR_WATER_HIGH_CELL_NUM, ENCOUNT_TR_WATER_HIGH_CELL_RES_NUM );

		// グラフィック設定
		ENC_CLACT_ResLoadEasy(
				eew->p_handle,
				&task_w->clact_sys,
				&task_w->clact_res,
				NARC_field_encounteffect_monsterball_NCLR, 1,
				NARC_field_encounteffect_monstaerball_oam_NCGR,
				NARC_field_encounteffect_monsterball_oam_NCER,
				NARC_field_encounteffect_monsterball_oam_NANR,
				ENCOUNT_TR_WATER_HIGH_CELL_CONT_ID
				);

		// アクター登録とBMP塗りつぶしオブジェ
		for( i=0; i<ENCOUNT_TR_WATER_HIGH_CELL_NUM; i++ ){
			task_w->clact[i] = ENC_CLACT_Add( 
					&task_w->clact_sys, &task_w->clact_res,
					0, 0, 0, 0);
			CLACT_SetDrawFlag( task_w->clact[i], FALSE );
			CLACT_SetAffineParam( task_w->clact[i], CLACT_AFFINE_DOUBLE );

			task_w->p_bmp_fill[i] = ENC_BMP_FillObjAlloc( HEAPID_FIELD );
		}
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

		// ビットマップ作成
		task_w->p_bmp = GF_BGL_BmpWinAllocGet( HEAPID_FIELD, 1 );
		GF_BGL_BmpWinAdd( eew->fsw->bgl, task_w->p_bmp, GF_BGL_FRAME3_M, 0, 0, 32, 32, 0, 0 );

		// カラーパレット転送
		{
			GXRgb color = 0;
			GF_BGL_PaletteSet( GF_BGL_FRAME3_M, &color, sizeof(short), 2*ENCOUNT_TR_WATER_HIGH_BMP_FILL_COL );
		}

		GF_BGL_BmpWinDataFill( task_w->p_bmp, 0 );
		GF_BGL_BmpWinOnVReq( task_w->p_bmp );

		eew->seq ++;
		break;
		
	case ENCOUNT_TR_WATER_HIGH_FLASH_INI:

		EncountFlashTask(MASK_MAIN_DISPLAY, 16, -16,  &eew->wait, 2);
		eew->seq++;
		break;
		
	case ENCOUNT_TR_WATER_HIGH_FLASH_WAIT:
		task_w->wait--;
		if( task_w->wait == 0 ){
			EF_Laster_Start( &task_w->laster,
					0, 191, ENCOUNT_TR_WATER_HIGH_SIN_ADDR, 
					ENCOUNT_TR_WATER_HIGH_SIN_R, ENCOUNT_TR_WATER_HIGH_LASTER_SP,
					LASTER_SCROLL_MBG0, 0, ENCOUNT_TR_WATER_HIGH_LASTER_TASK_PRI );
			task_w->laster_flg = TRUE;
		}
		if( eew->wait ){
			task_w->wait = ENCOUNT_TR_WATER_HIGH_CELL0_WAIT;
			eew->seq++;
		}
		break;

	case ENCOUNT_TR_WATER_HIGH_OAM_0_S:
		task_w->wait --;
		if( task_w->wait >= 0 ){
			// waitがマイナスになるのを待つ
			break;
		}

		// カメラ動さ作成
		dist = GFC_GetCameraDistance( task_w->camera );
		ENC_AddMoveReqFx( &task_w->camera_dist,
				dist,
				dist + ENCOUNT_TR_WATER_HIGH_CAMERA_DIST,
				ENCOUNT_TR_WATER_HIGH_CAMERA_SS,
				ENCOUNT_TR_WATER_HIGH_CAMERA_SYNC );

		// 回転動さ
		ENC_MoveReq( &task_w->oam_rota[0], 
				0,
				ENCOUNT_TR_WATER_LOW_CELL0_ROTA,
				ENCOUNT_TR_WATER_HIGH_CELLMOVE_SYNC );
		
		ENC_MoveReq( &task_w->obj_move[0], ENCOUNT_TR_WATER_HIGH_CELL_Y, ENCOUNT_TR_WATER_HIGH_CELL_Y_E, ENCOUNT_TR_WATER_HIGH_CELLMOVE_SYNC );
		
		ENC_BMP_FillObjStart( task_w->p_bmp_fill[0], 
				ENCOUNT_TR_WATER_HIGH_CELL0_X, ENCOUNT_TR_WATER_HIGH_CELL0_X,
				ENCOUNT_TR_WATER_HIGH_BMP_Y, ENCOUNT_TR_WATER_HIGH_BMP_Y_E,
				ENCOUNT_TR_WATER_HIGH_CELLMOVE_SYNC,
				task_w->p_bmp, 
				ENCOUNT_TR_WATER_HIGH_BMP_FILL_WIDTH,
				ENCOUNT_TR_WATER_HIGH_BMP_FILL_HEIGHT,
				ENCOUNT_TR_WATER_HIGH_BMP_FILL_COL );

		matrix = ENC_MakeVec( 
				ENCOUNT_TR_WATER_HIGH_CELL0_X * FX32_ONE, 
				ENCOUNT_TR_WATER_HIGH_CELL_Y * FX32_ONE, 0 );
		CLACT_SetMatrix( task_w->clact[0], &matrix );
		CLACT_SetDrawFlag( task_w->clact[0], TRUE );
		task_w->obj_move_flg[0] = TRUE;
		eew->seq ++;
		task_w->wait = ENCOUNT_TR_WATER_HIGH_CELL1_WAIT;
		break;
		
	case ENCOUNT_TR_WATER_HIGH_OAM_1_S:
		task_w->wait --;
		if( task_w->wait >= 0 ){
			// waitがマイナスになるのを待つ
			break;
		}
		
		ENC_MoveReq( &task_w->obj_move[1], ENCOUNT_TR_WATER_HIGH_CELL_Y, ENCOUNT_TR_WATER_HIGH_CELL_Y_E, ENCOUNT_TR_WATER_HIGH_CELLMOVE_SYNC );

		// 回転動さ
		ENC_MoveReq( &task_w->oam_rota[1], 
				0,
				ENCOUNT_TR_WATER_LOW_CELL1_ROTA,
				ENCOUNT_TR_WATER_HIGH_CELLMOVE_SYNC );

		ENC_BMP_FillObjStart( task_w->p_bmp_fill[1], 
				ENCOUNT_TR_WATER_HIGH_CELL1_X, ENCOUNT_TR_WATER_HIGH_CELL1_X,
				ENCOUNT_TR_WATER_HIGH_BMP_Y, ENCOUNT_TR_WATER_HIGH_BMP_Y_E,
				ENCOUNT_TR_WATER_HIGH_CELLMOVE_SYNC,
				task_w->p_bmp, 
				ENCOUNT_TR_WATER_HIGH_BMP_FILL_WIDTH,
				ENCOUNT_TR_WATER_HIGH_BMP_FILL_HEIGHT,
				ENCOUNT_TR_WATER_HIGH_BMP_FILL_COL );
		matrix = ENC_MakeVec( 
				ENCOUNT_TR_WATER_HIGH_CELL1_X * FX32_ONE, 
				ENCOUNT_TR_WATER_HIGH_CELL_Y * FX32_ONE, 1 );
		CLACT_SetMatrix( task_w->clact[1], &matrix );
		CLACT_SetDrawFlag( task_w->clact[1], TRUE );
		task_w->obj_move_flg[1] = TRUE;
		eew->seq ++;
		task_w->wait = ENCOUNT_TR_WATER_HIGH_CELL2_WAIT;
		break;
		
	case ENCOUNT_TR_WATER_HIGH_OAM_2_S:
		task_w->wait --;
		if( task_w->wait >= 0 ){
			// waitがマイナスになるのを待つ
			break;
		}
		
		ENC_MoveReq( &task_w->obj_move[2], ENCOUNT_TR_WATER_HIGH_CELL_Y, ENCOUNT_TR_WATER_HIGH_CELL_Y_E, ENCOUNT_TR_WATER_HIGH_CELLMOVE_SYNC );

		// 回転動さ
		ENC_MoveReq( &task_w->oam_rota[2], 
				0,
				ENCOUNT_TR_WATER_LOW_CELL2_ROTA,
				ENCOUNT_TR_WATER_HIGH_CELLMOVE_SYNC );

		ENC_BMP_FillObjStart( task_w->p_bmp_fill[2], 
				ENCOUNT_TR_WATER_HIGH_CELL2_X, ENCOUNT_TR_WATER_HIGH_CELL2_X,
				ENCOUNT_TR_WATER_HIGH_BMP_Y, ENCOUNT_TR_WATER_HIGH_BMP_Y_E,
				ENCOUNT_TR_WATER_HIGH_CELLMOVE_SYNC,
				task_w->p_bmp, 
				ENCOUNT_TR_WATER_HIGH_BMP_FILL_WIDTH,
				ENCOUNT_TR_WATER_HIGH_BMP_FILL_HEIGHT,
				ENCOUNT_TR_WATER_HIGH_BMP_FILL_COL );
		matrix = ENC_MakeVec( 
				ENCOUNT_TR_WATER_HIGH_CELL2_X * FX32_ONE, 
				ENCOUNT_TR_WATER_HIGH_CELL_Y * FX32_ONE, 2 );
		CLACT_SetMatrix( task_w->clact[2], &matrix );
		CLACT_SetDrawFlag( task_w->clact[2], TRUE );
		task_w->obj_move_flg[2] = TRUE;

		eew->seq ++;
		break;
		
	case ENCOUNT_TR_WATER_HIGH_OAM_WAIT:

		// カメラ動さ
		ENC_AddMoveMainFx( &task_w->camera_dist );
		GFC_SetCameraDistance( task_w->camera_dist.x, task_w->camera );
		
		// 全動さ完了チェック
		if( (task_w->obj_move_flg[0] == FALSE) &&
			(task_w->obj_move_flg[1] == FALSE) &&
			(task_w->obj_move_flg[2] == FALSE)	){
			eew->seq ++;
		}
		break;
		
	case ENCOUNT_TR_WATER_HIGH_END:
		WIPE_SetBrightnessFadeOut( WIPE_FADE_BLACK );
		if(eew->end != NULL){
			*(eew->end) = TRUE;		// タスク終了を報告
		}

		// ラスター破棄
		EF_Laster_Delete( &task_w->laster );
		task_w->laster_flg = FALSE;
		
		// OAM破棄	ビットマップ塗りつぶしオブジェ破棄
		for( i=0; i<ENCOUNT_TR_WATER_HIGH_CELL_NUM; i++ ){
			CLACT_Delete( task_w->clact[i] );
			ENC_BMP_FillObjDelete( task_w->p_bmp_fill[i] );	
		}
		ENC_CLACT_ResDeleteEasy( &task_w->clact_sys, &task_w->clact_res );
		ENC_CLACT_Delete( &task_w->clact_sys );

		// ビットマップ破棄
		GF_BGL_BmpWinOff( task_w->p_bmp );
		GF_BGL_BmpWinDel( task_w->p_bmp );
		GF_BGL_BmpWinFree( task_w->p_bmp, 1 );

		// スクリーンクリーン
		GF_BGL_ClearCharSet( GF_BGL_FRAME3_M, 32, 0, HEAPID_FIELD );
		GF_BGL_ScrClear( eew->fsw->bgl, GF_BGL_FRAME3_M );
		
		ENC_End( eew, tcb );
		return ;
	}

	// ラスターフリップ機能 
	if( task_w->laster_flg == TRUE ){
		EF_Laster_BuffXFlip( &task_w->laster, ENCOUNT_TR_WATER_HIGH_LASTER_FLIP_DOT );
	}

	// オブジェクト動さ
	for( i=0; i<ENCOUNT_TR_WATER_HIGH_CELL_NUM; i++ ){
		
		if( task_w->obj_move_flg[i] == TRUE ){
			const VecFx32* cp_mat;
			VecFx32 mat;
			
			result = ENC_BMP_FillObjMain( task_w->p_bmp_fill[i] );
			if( result ){
				task_w->obj_move_flg[i] = FALSE;
			}
			ENC_MoveMain( &task_w->obj_move[i] );
			ENC_MoveMain( &task_w->oam_rota[i] );

			CLACT_SetRotation( task_w->clact[i], task_w->oam_rota[i].x );
			
			cp_mat = CLACT_GetMatrix( task_w->clact[i] );
			mat = *cp_mat;
			mat.y = task_w->obj_move[i].x * FX32_ONE;
			CLACT_SetMatrix( task_w->clact[i], &mat );
		}
	}
	// 転送リクエスト
	GF_BGL_BmpWinOnVReq( task_w->p_bmp );

	if( eew->seq != ENCOUNT_TR_WATER_HIGH_END ){
		CLACT_Draw( task_w->clact_sys.cas );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナー　ダンジョン　レベル低
 */
//-----------------------------------------------------------------------------
void EncountEffect_Trainer_DanLevelLow(TCB_PTR tcb, void* work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	ENCOUNT_TR_DAN_LOW* task_w = eew->work;
	BOOL result;
	fx32 dist;
	VecFx32 scale;

	switch( eew->seq ){
	case ENCOUNT_TR_DAN_LOW_START:
		eew->work = sys_AllocMemory( HEAPID_FIELD, sizeof(ENCOUNT_TR_DAN_LOW) );	
		memset( eew->work, 0, sizeof(ENCOUNT_TR_DAN_LOW) );
		task_w = eew->work;

		// カメラポインタ取得
		task_w->camera = eew->fsw->camera_ptr;

		// セルアクターセット作成
		ENC_CLACT_Init( &task_w->clact_sys, ENCOUNT_TR_DAN_LOW_CELL_NUM, ENCOUNT_TR_DAN_LOW_CELL_RES_NUM );

		// グラフィック設定
		ENC_CLACT_ResLoadEasy(
				eew->p_handle,
				&task_w->clact_sys,
				&task_w->clact_res,
				NARC_field_encounteffect_monsterball_NCLR, 1,
				NARC_field_encounteffect_monstaerball_oam_NCGR,
				NARC_field_encounteffect_monsterball_oam_NCER,
				NARC_field_encounteffect_monsterball_oam_NANR,
				ENCOUNT_TR_DAN_LOW_CELL_CONT_ID
				);

		// アクター登録
		task_w->clact = ENC_CLACT_Add( 
				&task_w->clact_sys, &task_w->clact_res,
				ENCOUNT_TR_DAN_LOW_BALL_MAT_X, 
				ENCOUNT_TR_DAN_LOW_BALL_MAT_Y, 0, 0);
		CLACT_SetDrawFlag( task_w->clact, FALSE );
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

		eew->seq ++;
		break;
		
	case ENCOUNT_TR_DAN_LOW_FLASH_INI:

		EncountFlashTask(MASK_MAIN_DISPLAY, -16, -16,  &eew->wait, 2);
		eew->seq++;
		break;
		
	case ENCOUNT_TR_DAN_LOW_FLASH_WAIT:
		if( eew->wait ){
			eew->seq++;
		}
		break;

	case ENCOUNT_TR_DAN_LOW_MOVE_Y:
		ENC_AddMoveReqFx( &task_w->oam_move, 0, 
				ENCOUNT_TR_DAN_LOW_BALL_MOVE_Y, 
				ENCOUNT_TR_DAN_LOW_BALL_MOVE_Y_SS,
				ENCOUNT_TR_DAN_LOW_BALL_MOVE_SYNC );
		CLACT_SetDrawFlag(
				task_w->clact,
				TRUE );

		ENC_AddMoveReqFx( &task_w->oam_scale_x,
				ENCOUNT_TR_DAN_LOW_BALL_SCALE_X_S, 
				ENCOUNT_TR_DAN_LOW_BALL_SCALE_X_E, 
				ENCOUNT_TR_DAN_LOW_BALL_SCALE_X_SS,
				ENCOUNT_TR_DAN_LOW_BALL_MOVE_SYNC );

		ENC_AddMoveReqFx( &task_w->oam_scale_y,
				ENCOUNT_TR_DAN_LOW_BALL_SCALE_Y_S, 
				ENCOUNT_TR_DAN_LOW_BALL_SCALE_Y_E, 
				ENCOUNT_TR_DAN_LOW_BALL_SCALE_Y_SS,
				ENCOUNT_TR_DAN_LOW_BALL_MOVE_SYNC );

		scale = ENC_MakeVec( task_w->oam_scale_x.x, 
					task_w->oam_scale_y.x,
					0 );
		CLACT_SetScaleAffine( task_w->clact, &scale, CLACT_AFFINE_DOUBLE );

		// 回転
		ENC_MoveReq( &task_w->oam_rota,
				0, 
				ENCOUNT_TR_DAN_LOW_ROTA,
				ENCOUNT_TR_DAN_LOW_BALL_MOVE_SYNC );
		eew->seq++;
		break;
		
	case ENCOUNT_TR_DAN_LOW_MOVE_YWAIT:
		result = ENC_AddMoveMainFx( &task_w->oam_move );
		{
			VecFx32 mat = ENC_MakeVec( ENCOUNT_TR_DAN_LOW_BALL_MAT_X, 
					ENCOUNT_TR_DAN_LOW_BALL_MAT_Y + (task_w->oam_move.x),
					0 );
			CLACT_SetMatrix( 
					task_w->clact, 
					&mat );
		}
		ENC_AddMoveMainFx( &task_w->oam_scale_x );
		ENC_AddMoveMainFx( &task_w->oam_scale_y );
		scale = ENC_MakeVec( task_w->oam_scale_x.x, 
					task_w->oam_scale_y.x,
					0 );
		CLACT_SetScale( task_w->clact, &scale );

		ENC_MoveMain( &task_w->oam_rota );
		CLACT_SetRotation( task_w->clact, task_w->oam_rota.x );

		if( result == TRUE ){
			CLACT_SetDrawFlag( 
					task_w->clact, 
					FALSE );
			eew->seq++;
		}
		break;
		
	case ENCOUNT_TR_DAN_LOW_WIPE:
		// フィールドHブランクOFF
		FLDHBLANK_SYS_Stop( eew->fsw->fldmap->hblanksys );

		// カメラ動さ作成
		dist = GFC_GetCameraDistance( task_w->camera );
		ENC_AddMoveReqFx( &task_w->camera_dist,
				dist,
				dist + ENCOUNT_TR_DAN_LOW_CAMERA_DIST,
				ENCOUNT_TR_DAN_LOW_CAMERA_SS,
				ENCOUNT_TR_DAN_LOW_BALL_WIPE_SYNC );

		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_TUNNELOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, ENCOUNT_TR_DAN_LOW_BALL_WIPE_SYNC, 1, HEAPID_FIELD );
		eew->seq++;
		break;
		
	case ENCOUNT_TR_DAN_LOW_WIPEWAIT:
		// カメラ動さ
		ENC_AddMoveMainFx( &task_w->camera_dist );
		GFC_SetCameraDistance( task_w->camera_dist.x, task_w->camera );
		
		if( WIPE_SYS_EndCheck() ){
			eew->seq++;
		}
		break;
		
	case ENCOUNT_TR_DAN_LOW_END:
		WIPE_SetBrightness( WIPE_DISP_SUB, WIPE_FADE_BLACK );
		// フィールドHブランクON
		FLDHBLANK_SYS_Start( eew->fsw->fldmap->hblanksys );
		if(eew->end != NULL){
			*(eew->end) = TRUE;		// タスク終了を報告
		}
		// OAM破棄
		CLACT_Delete( task_w->clact );
		ENC_CLACT_ResDeleteEasy( &task_w->clact_sys, &task_w->clact_res );
		ENC_CLACT_Delete( &task_w->clact_sys );
		ENC_End( eew, tcb );
		break;
	}

	if( eew->seq != ENCOUNT_TR_DAN_LOW_END ){
		CLACT_Draw( task_w->clact_sys.cas );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナー　ダンジョン　レベル高
 */
//-----------------------------------------------------------------------------
void EncountEffect_Trainer_DanLevelHigh(TCB_PTR tcb, void* work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	ENCOUNT_TR_DAN_HIGH* task_w = eew->work;
	VecFx32 matrix;
	BOOL result;
	int i;
	fx32 dist;

	switch( eew->seq ){
	case ENCOUNT_TR_DAN_HIGH_START:
		eew->work = sys_AllocMemory( HEAPID_FIELD, sizeof(ENCOUNT_TR_DAN_HIGH) );	
		memset( eew->work, 0, sizeof(ENCOUNT_TR_DAN_HIGH) );
		task_w = eew->work;

		// カメラポインタ取得
		task_w->camera = eew->fsw->camera_ptr;

		// セルアクターセット作成
		ENC_CLACT_Init( &task_w->clact_sys, ENCOUNT_TR_DAN_HIGH_CELL_NUM, ENCOUNT_TR_DAN_HIGH_CELL_RES_NUM );

		// グラフィック設定
		ENC_CLACT_ResLoadEasy(
				eew->p_handle,
				&task_w->clact_sys,
				&task_w->clact_res,
				NARC_field_encounteffect_monsterball_NCLR, 1,
				NARC_field_encounteffect_monstaerball_oam_NCGR,
				NARC_field_encounteffect_monsterball_oam_NCER,
				NARC_field_encounteffect_monsterball_oam_NANR,
				ENCOUNT_TR_DAN_HIGH_CELL_CONT_ID
				);

		// アクター登録
		for( i=0; i<ENCOUNT_TR_DAN_HIGH_CELL_NUM; i++ ){
			task_w->clact[i] = ENC_CLACT_Add( 
					&task_w->clact_sys, &task_w->clact_res,
					0, 
					ENCOUNT_TR_DAN_HIGH_BALL_MAT_Y, 0, 0);
			CLACT_SetDrawFlag( task_w->clact[i], FALSE );
		}

		GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

		// ビットマップウィンドウ作成
		task_w->p_bmp = GF_BGL_BmpWinAllocGet( HEAPID_FIELD, 1 );
		GF_BGL_BmpWinAdd( eew->fsw->bgl, task_w->p_bmp, GF_BGL_FRAME3_M, 0, 0, 32, 32, 0, 0 );

		// カラーパレット転送
		{
			GXRgb color = 0;
			GF_BGL_PaletteSet( GF_BGL_FRAME3_M, &color, sizeof(short), 2*ENCOUNT_TR_DAN_HIGH_BLOCK_FILLCOLOR );
		}

		GF_BGL_BmpWinDataFill( task_w->p_bmp, 0 );
		GF_BGL_BmpWinOnVReq( task_w->p_bmp );

		// ブロックオブジェクト作成
		task_w->p_block = ENC_BMP_FillBlockMoveAlloc( HEAPID_FIELD );

		eew->seq ++;
		break;
		
	case ENCOUNT_TR_DAN_HIGH_FLASH_INI:

		EncountFlashTask(MASK_MAIN_DISPLAY, 16, -16,  &eew->wait, 2);
		eew->seq++;
		break;
		
	case ENCOUNT_TR_DAN_HIGH_FLASH_WAIT:
		if( eew->wait ){
			eew->seq++;
			task_w->wait = ENCOUNT_TR_DAN_HIGH_BALL_WAIT_0;
		}
		break;

	case ENCOUNT_TR_DAN_HIGH_MOVE_0Y:
		task_w->wait --;
		if( task_w->wait > 0 ){
			break;
		}
		
		ENC_MoveReq( &task_w->oam_move[0], 
				ENCOUNT_TR_DAN_HIGH_BALL_MAT_Y, 
				ENCOUNT_TR_DAN_HIGH_BALL_MOVE_Y, 
				ENCOUNT_TR_DAN_HIGH_BALL_MOVE_SYNC );
		matrix = ENC_MakeVec( 
				ENCOUNT_TR_DAN_HIGH_BALL_MAT_0X * FX32_ONE, 
				ENCOUNT_TR_DAN_HIGH_BALL_MAT_Y * FX32_ONE, 0 );
		CLACT_SetMatrix( task_w->clact[0], &matrix );
		CLACT_SetDrawFlag(
				task_w->clact[0],
				TRUE );

		// 回転
		ENC_MoveReq( &task_w->oam_rota[0], 
				0, 
				ENCOUNT_TR_DAN_HIGH_BALL0_ROTA, 
				ENCOUNT_TR_DAN_HIGH_BALL_MOVE_SYNC );
		CLACT_SetAffineParam( task_w->clact[0], CLACT_AFFINE_DOUBLE );
		
		task_w->oam_move_flg[0] = TRUE;
		eew->seq++;
		task_w->wait = ENCOUNT_TR_DAN_HIGH_BALL_WAIT_1;
		break;

	case ENCOUNT_TR_DAN_HIGH_MOVE_1Y:
		task_w->wait --;
		if( task_w->wait > 0 ){
			break;
		}

		ENC_MoveReq( &task_w->oam_move[1], 
				ENCOUNT_TR_DAN_HIGH_BALL_MAT_Y, 
				ENCOUNT_TR_DAN_HIGH_BALL_MOVE_Y, 
				ENCOUNT_TR_DAN_HIGH_BALL_MOVE_SYNC );
		matrix = ENC_MakeVec( 
				ENCOUNT_TR_DAN_HIGH_BALL_MAT_1X * FX32_ONE, 
				ENCOUNT_TR_DAN_HIGH_BALL_MAT_Y * FX32_ONE, 0 );
		CLACT_SetMatrix( task_w->clact[1], &matrix );
		CLACT_SetDrawFlag(
				task_w->clact[1],
				TRUE );

		// 回転
		ENC_MoveReq( &task_w->oam_rota[1], 
				0, 
				ENCOUNT_TR_DAN_HIGH_BALL1_ROTA, 
				ENCOUNT_TR_DAN_HIGH_BALL_MOVE_SYNC );

		CLACT_SetAffineParam( task_w->clact[1], CLACT_AFFINE_DOUBLE );
		task_w->oam_move_flg[1] = TRUE;
		eew->seq++;
		task_w->wait = ENCOUNT_TR_DAN_HIGH_BALL_WAIT_2;
		break;

	case ENCOUNT_TR_DAN_HIGH_MOVE_2Y:
		task_w->wait --;
		if( task_w->wait > 0 ){
			break;
		}

		ENC_MoveReq( &task_w->oam_move[2], 
				ENCOUNT_TR_DAN_HIGH_BALL_MAT_Y, 
				ENCOUNT_TR_DAN_HIGH_BALL_MOVE_Y, 
				ENCOUNT_TR_DAN_HIGH_BALL_MOVE_SYNC );
		matrix = ENC_MakeVec( 
				ENCOUNT_TR_DAN_HIGH_BALL_MAT_2X * FX32_ONE, 
				ENCOUNT_TR_DAN_HIGH_BALL_MAT_Y * FX32_ONE, 0 );
		CLACT_SetMatrix( task_w->clact[2], &matrix );
		CLACT_SetDrawFlag(
				task_w->clact[2],
				TRUE );

		// 回転
		ENC_MoveReq( &task_w->oam_rota[2], 
				0, 
				ENCOUNT_TR_DAN_HIGH_BALL2_ROTA, 
				ENCOUNT_TR_DAN_HIGH_BALL_MOVE_SYNC );
		CLACT_SetAffineParam( task_w->clact[2], CLACT_AFFINE_DOUBLE );

		task_w->oam_move_flg[2] = TRUE;
		eew->seq++;
		break;
		
	case ENCOUNT_TR_DAN_HIGH_MOVE_YWAIT:

		if( (task_w->oam_move_flg[0] == FALSE) &&
			(task_w->oam_move_flg[1] == FALSE) &&
			(task_w->oam_move_flg[2] == FALSE) ){
			for( i=0; i<ENCOUNT_TR_DAN_HIGH_CELL_NUM; i++ ){
				CLACT_SetDrawFlag( 
						task_w->clact[i], 
						FALSE );
			}
			eew->seq++;
		}
		break;
		
	case ENCOUNT_TR_DAN_HIGH_WIPE:
		ENC_BMP_FillBlockMoveStart( task_w->p_block, 
				ENCOUNT_TR_DAN_HIGH_BLOCK_SYNC,
				ENCOUNT_TR_DAN_HIGH_BLOCK_START_SYNC,
				task_w->p_bmp, ENCOUNT_TR_DAN_HIGH_BLOCK_FILLCOLOR );	

		// カメラ動さ作成
		dist = GFC_GetCameraDistance( task_w->camera );
		ENC_AddMoveReqFx( &task_w->camera_dist,
				dist,
				dist + ENCOUNT_TR_DAN_HIGH_CAMERA_DIST,
				ENCOUNT_TR_DAN_HIGH_CAMERA_SS,
				ENCOUNT_TR_DAN_HIGH_CAMERA_SYNC );

		eew->seq++;
		break;

	case ENCOUNT_TR_DAN_HIGH_WIPE_WAIT:
		result = ENC_BMP_FillBlockMoveMain( task_w->p_block );
		GF_BGL_BmpWinOnVReq( task_w->p_bmp );

		// カメラ動さ
		ENC_AddMoveMainFx( &task_w->camera_dist );
		GFC_SetCameraDistance( task_w->camera_dist.x, task_w->camera );
		
		if( result == TRUE ){
			eew->seq ++;
		}
		break;
		
	case ENCOUNT_TR_DAN_HIGH_END:
		WIPE_SetBrightnessFadeOut( WIPE_FADE_BLACK );
		if(eew->end != NULL){
			*(eew->end) = TRUE;		// タスク終了を報告
		}

		// OAM破棄
		for( i=0; i<ENCOUNT_TR_DAN_HIGH_CELL_NUM; i++ ){
			CLACT_Delete( task_w->clact[i] );
		}
		ENC_CLACT_ResDeleteEasy( &task_w->clact_sys, &task_w->clact_res );
		ENC_CLACT_Delete( &task_w->clact_sys );

		// ブロックワーク破棄
		ENC_BMP_FillBlockMoveDelete( task_w->p_block );

		// ビットマップ破棄
		GF_BGL_BmpWinOff( task_w->p_bmp );
		GF_BGL_BmpWinDel( task_w->p_bmp );
		GF_BGL_BmpWinFree( task_w->p_bmp, 1 );

		// スクリーンクリーン
		GF_BGL_ClearCharSet( GF_BGL_FRAME3_M, 32, 0, HEAPID_FIELD );
		GF_BGL_ScrClear( eew->fsw->bgl, GF_BGL_FRAME3_M );
		
		ENC_End( eew, tcb );
		break;
	}


	// オブジェクト動さ
	for( i=0; i<ENCOUNT_TR_DAN_HIGH_CELL_NUM; i++ ){
		
		if( task_w->oam_move_flg[i] == TRUE ){
			const VecFx32* cp_mat;
			VecFx32 mat;
			
			result = ENC_MoveMain( &task_w->oam_move[i] );
			ENC_MoveMain( &task_w->oam_rota[i] );
			if( result ){
				task_w->oam_move_flg[i] = FALSE;
			}
			cp_mat = CLACT_GetMatrix( task_w->clact[i] );
			mat = *cp_mat;
			mat.y = task_w->oam_move[i].x * FX32_ONE;
			CLACT_SetMatrix( task_w->clact[i], &mat );
			CLACT_SetRotation( task_w->clact[i], (0xffff & task_w->oam_rota[i].x) );
		}
	}

	if( eew->seq != ENCOUNT_TR_DAN_HIGH_END ){
		CLACT_Draw( task_w->clact_sys.cas );
	}
}




//-----------------------------------------------------------------------------
/**
 * 
 *		【特殊】 [ SPECIAL ]
 * 
 */
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
//	バトルタワー　ファクトリー
//=====================================
// シーケンス
enum{
	ENCOUNT_SP_TOWER_START,
	ENCOUNT_SP_TOWER_FLASH_INI,
	ENCOUNT_SP_TOWER_FLASH_WAIT,
	ENCOUNT_SP_TOWER_ALPHA,
	ENCOUNT_SP_TOWER_ALPHAWAIT,
	ENCOUNT_SP_TOWER_WIPE,
	ENCOUNT_SP_TOWER_WIPEWAIT,
	ENCOUNT_SP_TOWER_END,
};
// OAMデータ
// アクターデータ数
#define ENCOUNT_SP_TOWER_CELL_NUM		( 1 )	// ワーク数
#define ENCOUNT_SP_TOWER_CELL_RES_NUM	( 1 )	// リソース数
#define ENCOUNT_SP_TOWER_CELL_CONT_ID	( 600000 )

// 動さパラメータ	（触ってよい部分）
#define ENCOUNT_SP_TOWER_BALL_MAT_X		( 128*FX32_ONE )	// 開始座標
#define ENCOUNT_SP_TOWER_BALL_MAT_Y		( 96*FX32_ONE )	// 開始座標
#define ENCOUNT_SP_TOWER_FADE_SYNC		( 12 )		// フェードシンク数
#define ENCOUNT_SP_TOWER_SCALE_SYNC		( 6 )		// 拡大シンク数
#define ENCOUNT_SP_TOWER_SCALE_S		( FX32_CONST(1.0f) )	// 拡大開始
#define ENCOUNT_SP_TOWER_SCALE_E		( FX32_CONST(0.1f) )	// 拡大終了
#define ENCOUNT_SP_TOWER_SCALE_SS		( 1 )		// 初速度
#define ENCOUNT_SP_TOWER_FADE_S			( 0 )		// フェード開始
#define ENCOUNT_SP_TOWER_FADE_E			( 16 )		// フェード終了





//-------------------------------------
//	ダブルバトル
//=====================================
// シーケンス
enum{
	ENCOUNT_SP_DOUBLE_START,
	ENCOUNT_SP_DOUBLE_FLASH_INI,
	ENCOUNT_SP_DOUBLE_FLASH_WAIT,
	ENCOUNT_SP_DOUBLE_OAM_MOVE,
	ENCOUNT_SP_DOUBLE_OAM_MOVEWAIT,
	ENCOUNT_SP_DOUBLE_WIPE,
	ENCOUNT_SP_DOUBLE_WIPEWAIT,
	ENCOUNT_SP_DOUBLE_END,
};
// OAMデータ
// アクターデータ数
#define ENCOUNT_SP_DOUBLE_CELL_NUM		( 4 )	// ワーク数
#define ENCOUNT_SP_DOUBLE_CELL_RES_NUM	( 1 )	// リソース数
#define ENCOUNT_SP_DOUBLE_CELL_CONT_ID	( 600000 )

// 動さパラメータ	（触ってよい部分）
#define ENCOUNT_SP_DOUBLE_BALL_MAT_X		( 128*FX32_ONE )	// 開始座標
#define ENCOUNT_SP_DOUBLE_BALL_MAT_Y		( 96*FX32_ONE )	// 開始座標
#define ENCOUNT_SP_DOUBLE_MOVE_SYNC		( 4 )		// ４方向動さシンク数
#define ENCOUNT_SP_DOUBLE_WIPE_SYNC		( 8 )		// ワイプシンク数
#define ENCOUNT_SP_DOUBLE_MOVE_X	( 160*FX32_ONE )	// 動く距離
#define ENCOUNT_SP_DOUBLE_MOVE_Y	( 128*FX32_ONE )	// 動く距離
#define ENCOUNT_SP_DOUBLE_MOVE_SS		(  FX32_CONST( 0.1f ) )	// 初速度


//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
//	バトルタワー　ファクトリー
//=====================================
typedef struct {
	ENC_MOVE_WORK	oam_alpha;
	ENC_ADDMOVE_WORK_FX oam_scale;

	ENC_CLACT_SYS	clact_sys;
	ENC_CLACT_RES_WORK clact_res;
	CLACT_WORK_PTR clact;

	s32 wait;
} ENCOUNT_SP_TOWER;

//-------------------------------------
//	ダブルバトル
//=====================================
typedef struct {

	ENC_CLACT_SYS	clact_sys;
	ENC_CLACT_RES_WORK clact_res;
	CLACT_WORK_PTR clact[ ENCOUNT_SP_DOUBLE_CELL_NUM ];
	ENC_ADDMOVE_WORK_FX oam_move[ 2 ];
} ENCOUNT_SP_DOUBLE;




//----------------------------------------------------------------------------
/**
 *	@brief	バトルTOWER　バトルファクトリー
 */
//-----------------------------------------------------------------------------
void EncountEffect_Special_Tower(TCB_PTR tcb, void* work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	ENCOUNT_SP_TOWER* task_w = eew->work;
	BOOL result;

	switch( eew->seq ){
	case ENCOUNT_SP_TOWER_START:
		eew->work = sys_AllocMemory( HEAPID_FIELD, sizeof(ENCOUNT_SP_TOWER) );	
		memset( eew->work, 0, sizeof(ENCOUNT_SP_TOWER) );
		task_w = eew->work;

		// セルアクターセット作成
		ENC_CLACT_Init( &task_w->clact_sys, ENCOUNT_SP_TOWER_CELL_NUM, ENCOUNT_SP_TOWER_CELL_RES_NUM );

		// グラフィック設定
		ENC_CLACT_ResLoadEasy(
				eew->p_handle,
				&task_w->clact_sys,
				&task_w->clact_res,
				NARC_field_encounteffect_monsterball_NCLR, 1,
				NARC_field_encounteffect_monstarballbig_oam_NCGR,
				NARC_field_encounteffect_monsterballbig_oam_NCER,
				NARC_field_encounteffect_monsterballbig_oam_NANR,
				ENCOUNT_TR_GRASS_LOW_CELL_CONT_ID
				);

		// アクター登録
		task_w->clact = ENC_CLACT_Add( 
				&task_w->clact_sys, &task_w->clact_res,
				ENCOUNT_SP_TOWER_BALL_MAT_X, 
				ENCOUNT_SP_TOWER_BALL_MAT_Y,0, 0);
		CLACT_SetDrawFlag( task_w->clact, FALSE );
		CLACT_ObjModeSet( task_w->clact, GX_OAM_MODE_XLU );
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

		eew->seq ++;
		break;
		
	case ENCOUNT_SP_TOWER_FLASH_INI:

		EncountFlashTask(MASK_MAIN_DISPLAY, 16, -16,  &eew->wait, 2);
		eew->seq++;
		break;
		
	case ENCOUNT_SP_TOWER_FLASH_WAIT:
		if( eew->wait ){
			eew->seq++;
		}
		break;

	case ENCOUNT_SP_TOWER_ALPHA:
		ENC_MoveReq( &task_w->oam_alpha, 
				ENCOUNT_SP_TOWER_FADE_S, ENCOUNT_SP_TOWER_FADE_E,
				ENCOUNT_SP_TOWER_FADE_SYNC );
		G2_SetBlendAlpha( GX_BLEND_PLANEMASK_NONE, 
				GX_BLEND_PLANEMASK_BG0|GX_BLEND_PLANEMASK_BG1|GX_BLEND_PLANEMASK_BG2|GX_BLEND_PLANEMASK_BG3,
				task_w->oam_alpha.x, 16 - task_w->oam_alpha.x );

		CLACT_SetDrawFlag(
				task_w->clact,
				TRUE );
		eew->seq ++;
		break;
		
	case ENCOUNT_SP_TOWER_ALPHAWAIT:
		result = ENC_MoveMain( &task_w->oam_alpha );
		G2_ChangeBlendAlpha( task_w->oam_alpha.x, 16 - task_w->oam_alpha.x );
		if( result == TRUE ){
			G2_BlendNone();
			CLACT_ObjModeSet( task_w->clact, GX_OAM_MODE_NORMAL );
			eew->seq++;
		}
		break;
		
	case ENCOUNT_SP_TOWER_WIPE:
		ENC_AddMoveReqFx( &task_w->oam_scale, 
				ENCOUNT_SP_TOWER_SCALE_S,
				ENCOUNT_SP_TOWER_SCALE_E,
				ENCOUNT_SP_TOWER_SCALE_SS,
				ENCOUNT_SP_TOWER_SCALE_SYNC );

		CLACT_SetAffineParam(
				task_w->clact, 
				CLACT_AFFINE_DOUBLE );
		{
			VecFx32 scale = ENC_MakeVec( task_w->oam_scale.x, 
					task_w->oam_scale.x, task_w->oam_scale.x );
			CLACT_SetScale( 
					task_w->clact, 
					&scale );
		}

		// フィールドHブランクOFF
		FLDHBLANK_SYS_Stop( eew->fsw->fldmap->hblanksys );
		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_HOLEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, ENCOUNT_SP_TOWER_SCALE_SYNC, 1, HEAPID_FIELD );
		eew->seq++;
		break;
		
	case ENCOUNT_SP_TOWER_WIPEWAIT:
		result = ENC_AddMoveMainFx( &task_w->oam_scale );
		{
			VecFx32 scale = ENC_MakeVec( task_w->oam_scale.x, 
					task_w->oam_scale.x, task_w->oam_scale.x );
			CLACT_SetScale( 
					task_w->clact, 
					&scale );
		}
		if( (result == TRUE) && ( WIPE_SYS_EndCheck() == TRUE ) ){
			eew->seq ++;
		}
		break;
		
	case ENCOUNT_SP_TOWER_END:
		WIPE_SetBrightness( WIPE_DISP_SUB, WIPE_FADE_BLACK );
		
		// フィールドHブランクON
		FLDHBLANK_SYS_Start( eew->fsw->fldmap->hblanksys );
		
		if(eew->end != NULL){
			*(eew->end) = TRUE;		// タスク終了を報告
		}
		// OAM破棄
		CLACT_Delete( task_w->clact );
		ENC_CLACT_ResDeleteEasy( &task_w->clact_sys, &task_w->clact_res );
		ENC_CLACT_Delete( &task_w->clact_sys );
		ENC_End( eew, tcb );
		break;
	}

	if( eew->seq != ENCOUNT_SP_TOWER_END ){
		CLACT_Draw( task_w->clact_sys.cas );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ダブルバトル
 */
//-----------------------------------------------------------------------------
void EncountEffect_Special_Double(TCB_PTR tcb, void* work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	ENCOUNT_SP_DOUBLE* task_w = eew->work;
	BOOL result;
	int i;
	VecFx32 matrix;

	switch( eew->seq ){
	case ENCOUNT_SP_DOUBLE_START:
		eew->work = sys_AllocMemory( HEAPID_FIELD, sizeof(ENCOUNT_SP_DOUBLE) );	
		memset( eew->work, 0, sizeof(ENCOUNT_SP_DOUBLE) );
		task_w = eew->work;

		// セルアクターセット作成
		ENC_CLACT_Init( &task_w->clact_sys, ENCOUNT_SP_DOUBLE_CELL_NUM, ENCOUNT_SP_DOUBLE_CELL_RES_NUM );

		// グラフィック設定
		ENC_CLACT_ResLoadEasy(
				eew->p_handle,
				&task_w->clact_sys,
				&task_w->clact_res,
				NARC_field_encounteffect_monsterball_NCLR, 1,
				NARC_field_encounteffect_monstaerball_oam_NCGR,
				NARC_field_encounteffect_monsterball_oam_NCER,
				NARC_field_encounteffect_monsterball_oam_NANR,
				ENCOUNT_TR_GRASS_LOW_CELL_CONT_ID
				);

		// アクター登録
		for( i=0; i<ENCOUNT_SP_DOUBLE_CELL_NUM; i++ ){
			task_w->clact[i] = ENC_CLACT_Add( 
					&task_w->clact_sys, &task_w->clact_res,
					ENCOUNT_SP_DOUBLE_BALL_MAT_X, 
					ENCOUNT_SP_DOUBLE_BALL_MAT_Y,0, 0);
			CLACT_SetDrawFlag( task_w->clact[i], FALSE );
		}
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

		eew->seq ++;
		break;
		
	case ENCOUNT_SP_DOUBLE_FLASH_INI:

		EncountFlashTask(MASK_MAIN_DISPLAY, 16, -16,  &eew->wait, 2);
		eew->seq++;
		break;
		
	case ENCOUNT_SP_DOUBLE_FLASH_WAIT:
		if( eew->wait ){
			eew->seq++;
		}
		break;

	case ENCOUNT_SP_DOUBLE_OAM_MOVE:
		ENC_AddMoveReqFx( &task_w->oam_move[0],
				0, ENCOUNT_SP_DOUBLE_MOVE_Y,
				ENCOUNT_SP_DOUBLE_MOVE_SS, ENCOUNT_SP_DOUBLE_MOVE_SYNC );
		ENC_AddMoveReqFx( &task_w->oam_move[1],
				0, ENCOUNT_SP_DOUBLE_MOVE_X,
				ENCOUNT_SP_DOUBLE_MOVE_SS, ENCOUNT_SP_DOUBLE_MOVE_SYNC );

		for( i=0; i<ENCOUNT_SP_DOUBLE_CELL_NUM; i++ ){
			CLACT_SetDrawFlag( task_w->clact[i], TRUE );
		}
		eew->seq++;
		break;
		
	case ENCOUNT_SP_DOUBLE_OAM_MOVEWAIT:
		for( i=0; i<2; i++ ){
			result = ENC_AddMoveMainFx( &task_w->oam_move[i] );
		}
		// トップ
		matrix = ENC_MakeVec( 
				ENCOUNT_SP_DOUBLE_BALL_MAT_X,
				ENCOUNT_SP_DOUBLE_BALL_MAT_Y - task_w->oam_move[0].x,
				0 );
		CLACT_SetMatrix( task_w->clact[0], &matrix );

		// ボトム
		matrix = ENC_MakeVec( 
				ENCOUNT_SP_DOUBLE_BALL_MAT_X,
				ENCOUNT_SP_DOUBLE_BALL_MAT_Y + task_w->oam_move[0].x,
				0 );
		CLACT_SetMatrix( task_w->clact[1], &matrix );

		// レフと
		matrix = ENC_MakeVec( 
				ENCOUNT_SP_DOUBLE_BALL_MAT_X - task_w->oam_move[1].x,
				ENCOUNT_SP_DOUBLE_BALL_MAT_Y,
				0 );
		CLACT_SetMatrix( task_w->clact[2], &matrix );

		// LIGHT
		matrix = ENC_MakeVec( 
				ENCOUNT_SP_DOUBLE_BALL_MAT_X + task_w->oam_move[1].x,
				ENCOUNT_SP_DOUBLE_BALL_MAT_Y,
				0 );
		CLACT_SetMatrix( task_w->clact[3], &matrix );

		if( result == TRUE ){
			eew->seq ++;
		}
		break;
		
	case ENCOUNT_SP_DOUBLE_WIPE:
		// フィールドHブランクOFF
		FLDHBLANK_SYS_Stop( eew->fsw->fldmap->hblanksys );
		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FLASHOUT_FOREWAY, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, ENCOUNT_SP_DOUBLE_WIPE_SYNC, 1, HEAPID_FIELD );
		eew->seq++;
		break;
		
	case ENCOUNT_SP_DOUBLE_WIPEWAIT:
		if( WIPE_SYS_EndCheck() == TRUE ){
			eew->seq ++;
		}
		break;
		
	case ENCOUNT_SP_DOUBLE_END:
		WIPE_SetBrightness( WIPE_DISP_SUB, WIPE_FADE_BLACK );
		
		// フィールドHブランクON
		FLDHBLANK_SYS_Start( eew->fsw->fldmap->hblanksys );
		
		if(eew->end != NULL){
			*(eew->end) = TRUE;		// タスク終了を報告
		}
		// OAM破棄
		for( i=0; i<ENCOUNT_SP_DOUBLE_CELL_NUM; i++ ){
			CLACT_Delete( task_w->clact[i] );
		}
		ENC_CLACT_ResDeleteEasy( &task_w->clact_sys, &task_w->clact_res );
		ENC_CLACT_Delete( &task_w->clact_sys );
		ENC_End( eew, tcb );
		break;
	}

	if( eew->seq != ENCOUNT_SP_DOUBLE_END ){
		CLACT_Draw( task_w->clact_sys.cas );
	}
}



//-----------------------------------------------------------------------------
/**
 * 
 *		銀河団	エンカウント [ GINGA ]
 * 
 */
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
//	銀河団通常
//=====================================
// シーケンス
enum{
	ENCOUNT_GINGA_NORMAL_START,
	ENCOUNT_GINGA_NORMAL_FLASH_INI,
	ENCOUNT_GINGA_NORMAL_FLASH_WAIT,
	ENCOUNT_GINGA_NORMAL_OAM,
	ENCOUNT_GINGA_NORMAL_OAM_WAIT,
	ENCOUNT_GINGA_NORMAL_FLASHOUT,
	ENCOUNT_GINGA_NORMAL_FLASHOUT_WAIT,
	ENCOUNT_GINGA_NORMAL_END,
};
// OAMデータ
// アクターデータ数
#define ENCOUNT_GINGA_NORMAL_CELL_NUM		( 6 )	// ワーク数
#define ENCOUNT_GINGA_NORMAL_CELL_RES_NUM	( 1 )	// リソース数
#define ENCOUNT_GINGA_NORMAL_CELL_CONT_ID	( 600000 )

// 動さパラメータ	（触ってよい部分）
#define ENCOUNT_GINGA_NORMAL_OAM_MOVE_SYNC	( 8 )	// OAM1つ動さシンク
#define ENCOUNT_GINGA_NORMAL_FLASHOUT_SYNC	( 12 )	// フラッシュアウトシンク

#define ENCOUNT_GINGA_NORMAL_OAM_S_SCA	( FX32_CONST(2.0f) )	//	開始スケール
#define ENCOUNT_GINGA_NORMAL_OAM_E_SCA	( FX32_CONST(0.01f) )	//	終了スケール
#define ENCOUNT_GINGA_NORMAL_OAM_SS_SCA	( -FX32_CONST(0.40f) )	//	初速度スケール

enum{
	ENCOUNT_GINGA_NORMAL_OAM_PARAM_SX,
	ENCOUNT_GINGA_NORMAL_OAM_PARAM_EX,
	ENCOUNT_GINGA_NORMAL_OAM_PARAM_SSX,
	ENCOUNT_GINGA_NORMAL_OAM_PARAM_SY,
	ENCOUNT_GINGA_NORMAL_OAM_PARAM_EY,
	ENCOUNT_GINGA_NORMAL_OAM_PARAM_SSY,
	ENCOUNT_GINGA_NORMAL_OAM_PARAM_WAIT,
	ENCOUNT_GINGA_NORMAL_OAM_PARAM_ROTA,
	ENCOUNT_GINGA_NORMAL_OAM_PARAM_NUM,
};

// ボール動さパラメータ
// ENCOUNT_GINGA_NORMAL_CELL_NUM　と下のテーブル数が一緒なら
// 何個にでも出来ます
static const s32 ENCOUNT_GINGA_NORMAL_OAM_Param[ ENCOUNT_GINGA_NORMAL_CELL_NUM ][ ENCOUNT_GINGA_NORMAL_OAM_PARAM_NUM ] = {
	{	// 0
		FX32_CONST( 260 ),		// 開始X座標
        FX32_CONST( 128 ),		// 終了X座標
        -FX32_CONST( 30 ),		// 初速度X
		FX32_CONST( 0 ),		// 開始Y座標
		FX32_CONST( 100 ),		// 終了Y座標
		FX32_CONST( 20 ),		// 初速度Y
		4,						// 開始ウエイト
		0xffff * 2				// 回転量
	},
	{// 1
		FX32_CONST( -16 ),		// 開始X座標
		FX32_CONST( 128 ),      // 終了X座標
		FX32_CONST( 30 ),       // 初速度X
		FX32_CONST(160),        // 開始Y座標
		FX32_CONST(100),        // 終了Y座標
		-FX32_CONST(20),		// 初速度Y
		3,						// 開始ウエイト
		0xffff * 1				// 回転量
	},                         
	{// 2
		FX32_CONST(0),			// 開始X座標
		FX32_CONST(128),         // 終了X座標
		FX32_CONST(30),         // 初速度X
		FX32_CONST(-16),        // 開始Y座標
		FX32_CONST(100),         // 終了Y座標
		FX32_CONST(20),         // 初速度Y
		4,                       // 開始ウエイト
		-0xffff * 3				// 回転量
	},
	{// 3
		FX32_CONST(140),			// 開始X座標
		FX32_CONST(128),         // 終了X座標
		-FX32_CONST(10),         // 初速度X
		FX32_CONST(160),        // 開始Y座標
		FX32_CONST(100),         // 終了Y座標
		-FX32_CONST(20),         // 初速度Y
		2,                       // 開始ウエイト
		-0xffff * 2				// 回転量
	},
	{// 4
		FX32_CONST(260),			// 開始X座標
		FX32_CONST(128),         // 終了X座標
		-FX32_CONST(30),         // 初速度X
		FX32_CONST(80),        // 開始Y座標
		FX32_CONST(100),         // 終了Y座標
		FX32_CONST(1),         // 初速度Y
		3,                       // 開始ウエイト
		-0xffff * 3				// 回転量
	},
	{// 5
		FX32_CONST(0),			// 開始X座標
		FX32_CONST(128),         // 終了X座標
		FX32_CONST(30),         // 初速度X
		FX32_CONST(160),        // 開始Y座標
		FX32_CONST(100),         // 終了Y座標
		-FX32_CONST(20),         // 初速度Y
		3,                       // 開始ウエイト
		0xffff * 1				// 回転量
	},
};






//-------------------------------------
//	銀河団　幹部クラス
//=====================================
// シーケンス
enum{
	ENCOUNT_GINGA_SPECIAL_START,
	ENCOUNT_GINGA_SPECIAL_FLASH_INI,
	ENCOUNT_GINGA_SPECIAL_FLASH_WAIT,
	ENCOUNT_GINGA_SPECIAL_ALPHA_IN,
	ENCOUNT_GINGA_SPECIAL_ALPHA_INWAIT,
	ENCOUNT_GINGA_SPECIAL_FLASHOUT,
	ENCOUNT_GINGA_SPECIAL_FLASHOUT_WAIT,
	ENCOUNT_GINGA_SPECIAL_END,
};
// OAMデータ
// アクターデータ数
#define ENCOUNT_GINGA_SPECIAL_CELL_NUM		( 1 )	// ワーク数
#define ENCOUNT_GINGA_SPECIAL_CELL_RES_NUM	( 1 )	// リソース数
#define ENCOUNT_GINGA_SPECIAL_CELL_CONT_ID	( 600000 )

// ビットマップ関係
#define ENCOUNT_GINGA_SPECIAL_BMP_COL	( 15 )

// 動さパラメータ	（触ってよい部分）
#define ENCOUNT_GINGA_SPECIAL_MAT_X		( 128*FX32_ONE )	// 開始座標
#define ENCOUNT_GINGA_SPECIAL_MAT_Y		( 96*FX32_ONE )	// 開始座標

#define ENCOUNT_GINGA_SPECIAL_OAM_ALPHA_SYNC	( 15 )	// αシンク
#define ENCOUNT_GINGA_SPECIAL_FLASHMOSAIC_WAIT		( 16 )	// モザイクとフラッシュアウトの開始ウエイト
#define ENCOUNT_GINGA_SPECIAL_OAM_MOSAIC_SYNC	( 16 )	// モザイクシンク数

#define ENCOUNT_GINGA_SPECIAL_FLASHOUT_SYNC	( 16 )		// フラッシュアウトシンク
#define ENCOUNT_GINGA_SPECIAL_ALPHA_S	( 0 )	// α開始
#define ENCOUNT_GINGA_SPECIAL_ALPHA_E	( 16 )	// α終了	(0～16)

#define ENCOUNT_GINGA_SPECIAL_MOSAIC_S	( 0 )	// モザイク開始
#define ENCOUNT_GINGA_SPECIAL_MOSAIC_E	( 14 )	// モザイク終了(0～15)


//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
//	銀河団通常
//=====================================
typedef struct {
	ENC_CLACT_SYS	clact_sys;
	ENC_CLACT_RES_WORK clact_res;
	CLACT_WORK_PTR clact[ ENCOUNT_GINGA_NORMAL_CELL_NUM ];
	ENC_ADDMOVE_WORK_FX oam_scale[ ENCOUNT_GINGA_NORMAL_CELL_NUM ];
	ENC_ADDMOVE_WORK_FX oam_move_x[ ENCOUNT_GINGA_NORMAL_CELL_NUM ];
	ENC_ADDMOVE_WORK_FX oam_move_y[ ENCOUNT_GINGA_NORMAL_CELL_NUM ];
	ENC_MOVE_WORK		oam_rota[ ENCOUNT_GINGA_NORMAL_CELL_NUM ];
	BOOL oam_move_flg[ ENCOUNT_GINGA_NORMAL_CELL_NUM ];

	s32 count;
	s32 wait;
} ENCOUNT_GINGA_NORMAL;

//-------------------------------------
//	銀河団　幹部クラス
//=====================================
typedef struct {
	GF_BGL_BMPWIN* p_bmp;
	ENC_BMP_FLASH_OUT* p_flashout;

	ENC_CLACT_SYS	clact_sys;
	ENC_CLACT_RES_WORK clact_res;
	CLACT_WORK_PTR clact;
	ENC_MOVE_WORK mosaic;
	ENC_MOVE_WORK alpha;
	ENC_MOVE_WORK rota;

	s32 wait;
} ENCOUNT_GINGA_SPECIAL;



//----------------------------------------------------------------------------
/**
 *	@brief	銀河団　通常
 */
//-----------------------------------------------------------------------------
void EncountEffect_Ginga_Normal(TCB_PTR tcb, void* work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	ENCOUNT_GINGA_NORMAL* task_w = eew->work;
	BOOL result;
	int i;
	VecFx32 matrix;
	VecFx32 scale;

	switch( eew->seq ){
	case ENCOUNT_GINGA_NORMAL_START:
		eew->work = sys_AllocMemory( HEAPID_FIELD, sizeof(ENCOUNT_GINGA_NORMAL) );	
		memset( eew->work, 0, sizeof(ENCOUNT_GINGA_NORMAL) );
		task_w = eew->work;

		// セルアクターセット作成
		ENC_CLACT_Init( &task_w->clact_sys, ENCOUNT_GINGA_NORMAL_CELL_NUM, ENCOUNT_GINGA_NORMAL_CELL_RES_NUM );

		// グラフィック設定
		ENC_CLACT_ResLoadEasy(
				eew->p_handle,
				&task_w->clact_sys,
				&task_w->clact_res,
				NARC_field_encounteffect_monsterball_NCLR, 1,
				NARC_field_encounteffect_monstaerball_oam_NCGR,
				NARC_field_encounteffect_monsterball_oam_NCER,
				NARC_field_encounteffect_monsterball_oam_NANR,
				ENCOUNT_TR_GRASS_LOW_CELL_CONT_ID
				);

		// アクター登録
		for( i=0; i<ENCOUNT_GINGA_NORMAL_CELL_NUM; i++ ){
			task_w->clact[i] = ENC_CLACT_Add( 
					&task_w->clact_sys, &task_w->clact_res,
					0, 
					0,0, 0);
			CLACT_SetDrawFlag( task_w->clact[i], FALSE );
		}
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

		eew->seq ++;
		break;
		
	case ENCOUNT_GINGA_NORMAL_FLASH_INI:

		EncountFlashTask(MASK_MAIN_DISPLAY, -16, -16,  &eew->wait, 2);
		eew->seq++;
		break;
		
	case ENCOUNT_GINGA_NORMAL_FLASH_WAIT:
		if( eew->wait ){
			eew->seq++;
			task_w->count = 0;
			task_w->wait = ENCOUNT_GINGA_NORMAL_OAM_Param[task_w->count][ENCOUNT_GINGA_NORMAL_OAM_PARAM_WAIT];
		}
		break;

	case ENCOUNT_GINGA_NORMAL_OAM:
		task_w->wait --;
		if( task_w->wait < 0 ){
			ENC_AddMoveReqFx( &task_w->oam_move_x[task_w->count], 
					ENCOUNT_GINGA_NORMAL_OAM_Param[task_w->count][ENCOUNT_GINGA_NORMAL_OAM_PARAM_SX],
					ENCOUNT_GINGA_NORMAL_OAM_Param[task_w->count][ENCOUNT_GINGA_NORMAL_OAM_PARAM_EX],
					ENCOUNT_GINGA_NORMAL_OAM_Param[task_w->count][ENCOUNT_GINGA_NORMAL_OAM_PARAM_SSX],
					ENCOUNT_GINGA_NORMAL_OAM_MOVE_SYNC );

			ENC_AddMoveReqFx( &task_w->oam_move_y[task_w->count], 
					ENCOUNT_GINGA_NORMAL_OAM_Param[task_w->count][ENCOUNT_GINGA_NORMAL_OAM_PARAM_SY],
					ENCOUNT_GINGA_NORMAL_OAM_Param[task_w->count][ENCOUNT_GINGA_NORMAL_OAM_PARAM_EY],
					ENCOUNT_GINGA_NORMAL_OAM_Param[task_w->count][ENCOUNT_GINGA_NORMAL_OAM_PARAM_SSY],
					ENCOUNT_GINGA_NORMAL_OAM_MOVE_SYNC );

			ENC_AddMoveReqFx( &task_w->oam_scale[task_w->count], 
					ENCOUNT_GINGA_NORMAL_OAM_S_SCA,
					ENCOUNT_GINGA_NORMAL_OAM_E_SCA,
					ENCOUNT_GINGA_NORMAL_OAM_SS_SCA,
					ENCOUNT_GINGA_NORMAL_OAM_MOVE_SYNC );

			ENC_MoveReq( &task_w->oam_rota[task_w->count], 
					0,
					ENCOUNT_GINGA_NORMAL_OAM_Param[ task_w->count ][ENCOUNT_GINGA_NORMAL_OAM_PARAM_ROTA],
					ENCOUNT_GINGA_NORMAL_OAM_MOVE_SYNC );

			CLACT_SetDrawFlag( task_w->clact[task_w->count], TRUE );
			matrix = ENC_MakeVec( 
					ENCOUNT_GINGA_NORMAL_OAM_Param[task_w->count][ENCOUNT_GINGA_NORMAL_OAM_PARAM_SX], 
					ENCOUNT_GINGA_NORMAL_OAM_Param[task_w->count][ENCOUNT_GINGA_NORMAL_OAM_PARAM_SY], 0 );
			CLACT_SetMatrix( task_w->clact[task_w->count], &matrix );
			scale = ENC_MakeVec( 
					ENCOUNT_GINGA_NORMAL_OAM_S_SCA, 
					ENCOUNT_GINGA_NORMAL_OAM_S_SCA, 0 );
			CLACT_SetScaleAffine( task_w->clact[task_w->count], &scale, CLACT_AFFINE_DOUBLE );

			task_w->oam_move_flg[task_w->count] = TRUE;

			task_w->count++;

			if( task_w->count >= ENCOUNT_GINGA_NORMAL_CELL_NUM ){
				eew->seq++;
			}else{
				task_w->wait = ENCOUNT_GINGA_NORMAL_OAM_Param[task_w->count][ENCOUNT_GINGA_NORMAL_OAM_PARAM_WAIT];
			}
		}
		break;

	case ENCOUNT_GINGA_NORMAL_OAM_WAIT:
		// 全動さ完了チェック
		if( task_w->oam_move_flg[ ENCOUNT_GINGA_NORMAL_CELL_NUM - 1 ] == FALSE ){
			eew->seq ++;
		}
		break;

	case ENCOUNT_GINGA_NORMAL_FLASHOUT:
		// フィールドHブランクOFF
		FLDHBLANK_SYS_Stop( eew->fsw->fldmap->hblanksys );

		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FLASHOUT_FOREWAY, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, ENCOUNT_GINGA_NORMAL_FLASHOUT_SYNC, 1, HEAPID_FIELD );
		eew->seq++;
		break;
		
	case ENCOUNT_GINGA_NORMAL_FLASHOUT_WAIT:
		if( WIPE_SYS_EndCheck() ){
			eew->seq++;
		}
		break;
		
	case ENCOUNT_GINGA_NORMAL_END:
		WIPE_SetBrightness( WIPE_DISP_SUB, WIPE_FADE_BLACK );

		// フィールドHブランクON
		FLDHBLANK_SYS_Start( eew->fsw->fldmap->hblanksys );
		
		if(eew->end != NULL){
			*(eew->end) = TRUE;		// タスク終了を報告
		}
		// OAM破棄
		for( i=0; i<ENCOUNT_GINGA_NORMAL_CELL_NUM; i++ ){
			CLACT_Delete( task_w->clact[i] );
		}
		ENC_CLACT_ResDeleteEasy( &task_w->clact_sys, &task_w->clact_res );
		ENC_CLACT_Delete( &task_w->clact_sys );
		
		ENC_End( eew, tcb );
		break;
	}


	// オブジェクト動さ
	for( i=0; i<ENCOUNT_GINGA_NORMAL_CELL_NUM; i++ ){
		
		if( task_w->oam_move_flg[i] == TRUE ){
			
			result = ENC_AddMoveMainFx( &task_w->oam_move_x[i] );
			ENC_AddMoveMainFx( &task_w->oam_move_y[i] );
			ENC_AddMoveMainFx( &task_w->oam_scale[i] );
			ENC_MoveMain( &task_w->oam_rota[i] );
			if( result ){
				task_w->oam_move_flg[i] = FALSE;
				CLACT_SetDrawFlag( task_w->clact[i], FALSE );
			}
			matrix = ENC_MakeVec( 
					task_w->oam_move_x[i].x, 
					task_w->oam_move_y[i].x, 0 );
			CLACT_SetMatrix( task_w->clact[i], &matrix );
			scale = ENC_MakeVec( 
					task_w->oam_scale[i].x, 
					task_w->oam_scale[i].x, 0 );
			CLACT_SetScale( task_w->clact[i], &scale );
			CLACT_SetRotation( task_w->clact[i], task_w->oam_rota[i].x );
		}
	}

	if( eew->seq != ENCOUNT_GINGA_NORMAL_END ){
		CLACT_Draw( task_w->clact_sys.cas );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	銀河団　幹部クラス
 */
//-----------------------------------------------------------------------------
void EncountEffect_Ginga_Special(TCB_PTR tcb, void* work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	ENCOUNT_GINGA_SPECIAL* task_w = eew->work;
	BOOL result;

	switch( eew->seq ){
	case ENCOUNT_GINGA_SPECIAL_START:
		eew->work = sys_AllocMemory( HEAPID_FIELD, sizeof(ENCOUNT_GINGA_SPECIAL) );	
		memset( eew->work, 0, sizeof(ENCOUNT_GINGA_SPECIAL) );
		task_w = eew->work;

		// セルアクターセット作成
		ENC_CLACT_Init( &task_w->clact_sys, ENCOUNT_GINGA_SPECIAL_CELL_NUM, ENCOUNT_GINGA_SPECIAL_CELL_RES_NUM );

		// グラフィック設定
		ENC_CLACT_ResLoadEasy(
				eew->p_handle,
				&task_w->clact_sys,
				&task_w->clact_res,
				NARC_field_encounteffect_ginga_NCLR, 1,
				NARC_field_encounteffect_ginga_NCGR,
				NARC_field_encounteffect_ginga_NCER,
				NARC_field_encounteffect_ginga_NANR,
				ENCOUNT_TR_GRASS_LOW_CELL_CONT_ID
				);

		// アクター登録
		task_w->clact = ENC_CLACT_Add( 
				&task_w->clact_sys, &task_w->clact_res,
				ENCOUNT_GINGA_SPECIAL_MAT_X, 
				ENCOUNT_GINGA_SPECIAL_MAT_Y, 0, 0);
		CLACT_SetDrawFlag( task_w->clact, FALSE );
		CLACT_ObjModeSet( task_w->clact, GX_OAM_MODE_XLU );
		CLACT_BGPriorityChg( task_w->clact, 1 );
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

		// フラッシュアウトオブジェ作成
		task_w->p_flashout = ENC_BMP_FlashOutAlloc( HEAPID_FIELD );

		// ビットマップウィンドウ作成
		task_w->p_bmp = GF_BGL_BmpWinAllocGet( HEAPID_FIELD, 1 );
		GF_BGL_BmpWinAdd( eew->fsw->bgl, task_w->p_bmp, GF_BGL_FRAME3_M, 0, 0, 32, 32, 0, 0 );

		// カラーパレット転送
		{
			GXRgb color = 0;
			GF_BGL_PaletteSet( GF_BGL_FRAME3_M, &color, sizeof(short), 2*ENCOUNT_GINGA_SPECIAL_BMP_COL );
		}

		GF_BGL_BmpWinDataFill( task_w->p_bmp, 0 );
		GF_BGL_BmpWinOnVReq( task_w->p_bmp );

		eew->seq ++;
		break;
		
	case ENCOUNT_GINGA_SPECIAL_FLASH_INI:

		EncountFlashTask(MASK_MAIN_DISPLAY, 16, -16,  &eew->wait, 2);
		eew->seq++;
		break;
		
	case ENCOUNT_GINGA_SPECIAL_FLASH_WAIT:
		if( eew->wait ){
			eew->seq++;
		}
		break;

	case ENCOUNT_GINGA_SPECIAL_ALPHA_IN:
		ENC_MoveReq( &task_w->alpha, ENCOUNT_GINGA_SPECIAL_ALPHA_S, ENCOUNT_GINGA_SPECIAL_ALPHA_E, ENCOUNT_GINGA_SPECIAL_OAM_ALPHA_SYNC );
		CLACT_SetDrawFlag( task_w->clact, TRUE );
		G2_SetBlendAlpha( GX_BLEND_PLANEMASK_NONE, 
				GX_BLEND_PLANEMASK_BG0|GX_BLEND_PLANEMASK_BG1|GX_BLEND_PLANEMASK_BG2|GX_BLEND_PLANEMASK_BG3,
				task_w->alpha.x, 16 - task_w->alpha.x );
		eew->seq++;
		break;
		
	case ENCOUNT_GINGA_SPECIAL_ALPHA_INWAIT:
		result = ENC_MoveMain( &task_w->alpha );
		G2_ChangeBlendAlpha( task_w->alpha.x, 16 - task_w->alpha.x );
		if( result == TRUE ){
			G2_BlendNone();
			CLACT_ObjModeSet( task_w->clact, GX_OAM_MODE_NORMAL );
			eew->seq++;
			task_w->wait = ENCOUNT_GINGA_SPECIAL_FLASHMOSAIC_WAIT;
		}
		break;

	case ENCOUNT_GINGA_SPECIAL_FLASHOUT:
		task_w->wait --;
		if( task_w->wait > 0 ){
			break;
		}
		
		ENC_MoveReq( &task_w->mosaic, ENCOUNT_GINGA_SPECIAL_MOSAIC_S, ENCOUNT_GINGA_SPECIAL_MOSAIC_E, ENCOUNT_GINGA_SPECIAL_OAM_MOSAIC_SYNC );
		ENC_BMP_FlashOutStart( task_w->p_flashout, ENCOUNT_GINGA_SPECIAL_FLASHOUT_SYNC, task_w->p_bmp, ENCOUNT_GINGA_SPECIAL_BMP_COL );
		eew->seq++;
		break;
		
	case ENCOUNT_GINGA_SPECIAL_FLASHOUT_WAIT:
		ENC_MoveMain( &task_w->mosaic );
		G2_SetOBJMosaicSize( task_w->mosaic.x, task_w->mosaic.x );
		if( ENC_BMP_FlashOutMain( task_w->p_flashout ) ){
			eew->seq++;
		}
		GF_BGL_BmpWinOnVReq( task_w->p_bmp );
		break;
		
	case ENCOUNT_GINGA_SPECIAL_END:
		WIPE_SetBrightnessFadeOut( WIPE_FADE_BLACK );
		
		// フラッシュアウトオブジェ破棄
		ENC_BMP_FlashOutDelete( task_w->p_flashout );
		
		if(eew->end != NULL){
			*(eew->end) = TRUE;		// タスク終了を報告
		}
		// OAM破棄
		CLACT_Delete( task_w->clact );
		ENC_CLACT_ResDeleteEasy( &task_w->clact_sys, &task_w->clact_res );
		ENC_CLACT_Delete( &task_w->clact_sys );

		// ビットマップ破棄
		GF_BGL_BmpWinOff( task_w->p_bmp );
		GF_BGL_BmpWinDel( task_w->p_bmp );
		GF_BGL_BmpWinFree( task_w->p_bmp, 1 );

		// スクリーンクリーン
		GF_BGL_ClearCharSet( GF_BGL_FRAME3_M, 32, 0, HEAPID_FIELD );
		GF_BGL_ScrClear( eew->fsw->bgl, GF_BGL_FRAME3_M );
		
		G2_SetOBJMosaicSize( 0, 0 );
		
		ENC_End( eew, tcb );
		break;
	}


	if( eew->seq != ENCOUNT_GINGA_SPECIAL_END ){
		CLACT_Draw( task_w->clact_sys.cas );
	}
}


//-----------------------------------------------------------------------------
/**
 * 
 *		伝説ポケモン　エンカウント [ LEGEND ]
 * 
 */
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
//	伝説ポケモン
//=====================================
// シーケンス
enum{
	ENCOUNT_LEGEND_NORMAL_START,
	ENCOUNT_LEGEND_NORMAL_FLASH_INI,
	ENCOUNT_LEGEND_NORMAL_FLASH_WAIT,
	ENCOUNT_LEGEND_NORMAL_MOTIONBL_S,
	ENCOUNT_LEGEND_NORMAL_CAMERA_SET,
	ENCOUNT_LEGEND_NORMAL_FADE,
	ENCOUNT_LEGEND_NORMAL_FADEWAIT,
	ENCOUNT_LEGEND_NORMAL_END,
};

// カメラテーブル
typedef struct{
	u32 dist;		// 距離
	u16 angle_x;	// Angle
	u16 angle_y;
	u16 purse;		// パース
	u16 wait;
} ENCOUNT_LEGEND_CAMERA_TBL;


// カメラテーブル	(触ってよい)
#define ENCOUNT_LEGEND_CAMERA_TBL_NUM	( 16 )	// ここを増やし、下のテーブルの数を合わせると、テーブルの数も変更できます。
static const ENCOUNT_LEGEND_CAMERA_TBL EncLegendNormalCamera[ ENCOUNT_LEGEND_CAMERA_TBL_NUM ] = {
	{	// 0
		0x29AEC1,		// 距離
		0xD602,			// AngleX
		0x0,			// AngleY
		0x5C1,			// パース
		4,				// ウエイト
	},
	{	// 1
		0x29AEC1,		// 距離
		0xcf02,			// AngleX
		0xff00,			// AngleY
		0x601,			// パース
		4,				// ウエイト
	},
	{	// 2
		0x29AEC1,		// 距離
		0xe602,			// AngleX
		0x1000,			// AngleY
		0x691,			// パース
		4,				// ウエイト
	},
	{	// 3
		0x29AEC1,		// 距離
		0xD602,			// AngleX
		0xa00,			// AngleY
		0x711,			// パース
		3,				// ウエイト
	},
	{	// 4
		0x29AEC1,		// 距離
		0xe102,			// AngleX
		0xf000,			// AngleY
		0x780,			// パース
		3,				// ウエイト
	},
	{	// 5
		0x29AEC1,		// 距離
		0xc602,			// AngleX
		0x0000,			// AngleY
		0x751,			// パース
		3,				// ウエイト
	},
	{	// 6
		0x29AEC1,		// 距離
		0xe002,			// AngleX
		0xf000,			// AngleY
		0x800,			// パース
		3,				// ウエイト
	},
	{	// 7
		0x29AEC1,		// 距離
		0xD602,			// AngleX
		0,				// AngleY
		0x802,			// パース
		3,				// ウエイト
	},
	{	// 8
		0x29AEC1,		// 距離
		0xD002,			// AngleX
		0x1000,			// AngleY
		0x800,			// パース
		3,				// ウエイト
	},
	{	// 9
		0x29AEC1,		// 距離
		0xD902,			// AngleX
		0xf500,			// AngleY
		0x751,			// パース
		3,				// ウエイト
	},
	{	// 10
		0x29AEC1,		// 距離
		0xD002,			// AngleX
		0xa00,			// AngleY
		0x4C1,			// パース
		2,				// ウエイト
	},
	{	// 11
		0x29AEC1,		// 距離
		0xe002,			// AngleX
		0xf000,			// AngleY
		0x3C1,			// パース
		2,				// ウエイト
	},
	{	// 12
		0x29AEC1,		// 距離
		0xD002,			// AngleX
		0xf000,			// AngleY
		0x650,			// パース
		1,				// ウエイト
	},
	{	// 13
		0x29AEC1,		// 距離
		0xe002,			// AngleX
		0xa000,			// AngleY
		0x241,			// パース
		1,				// ウエイト
	},
	{	// 14
		0x29AEC1,		// 距離
		0xe1a2,			// AngleX
		0x500,			// AngleY
		0x500,			// パース
		1,				// ウエイト
	},
	{	// 15
		0x29AEC1,		// 距離
		0xD602,			// AngleX
		0,				// AngleY
		0x241,			// パース
		1,				// ウエイト
	},
};

// 動さパラメータ	（触ってよい部分）
#define ENCOUNT_LEGEND_NORMAL_FADE_SYNC		( 10 )	// フェードシンク
#define ENCOUNT_LEGEND_NORMAL_MOTIONBL_EVA	( 3 )	// モーションブラー係数　新しくブレンドされる絵
#define ENCOUNT_LEGEND_NORMAL_MOTIONBL_EAB	( 15 )	// モーションブラー係数　すでにバッファされている絵


//-------------------------------------
//	パッケージポケモン
//=====================================
// シーケンス
enum{
	ENCOUNT_LEGEND_PACKAGE_START,
	ENCOUNT_LEGEND_PACKAGE_FLASH_INI,
	ENCOUNT_LEGEND_PACKAGE_FLASH_WAIT,
	ENCOUNT_LEGEND_PACKAGE_MOTIONBL_S,
	ENCOUNT_LEGEND_PACKAGE_PURSE,
	ENCOUNT_LEGEND_PACKAGE_CAMERA,
	ENCOUNT_LEGEND_PACKAGE_CAMERAWAIT,
	ENCOUNT_LEGEND_PACKAGE_FADE,
	ENCOUNT_LEGEND_PACKAGE_FADEWAIT,
	ENCOUNT_LEGEND_PACKAGE_END,
};

// 動さパラメータ	（触ってよい部分）
#define ENCOUNT_LEGEND_PACKAGE_PURSE_SYNC	( 40 )		// パースシンク
#define ENCOUNT_LEGEND_PACKAGE_CAMERA_MOVE_WAIT		( 5 )	// カメラ待ちシンク
#define ENCOUNT_LEGEND_PACKAGE_CAMERA_SYNC	( 8 )	// カメラシンク

#define ENCOUNT_LEGEND_PACKAGE_FADE_SYNC	( 60 )	// フェードシンク

#define ENCOUNT_LEGEND_PACKAGE_PURSE_MOVE	( 0x100 )	// パース変動値 0b0

#define ENCOUNT_LEGEND_PACKAGE_CAMERA_MOVE	( -FX32_CONST( 2350 ) )	// カメラ動作値 200
#define ENCOUNT_LEGEND_PACKAGE_CAMERA_SS	( FX32_CONST( 0.5 ) ) //0.0f

#define ENCOUNT_LEGEND_PACKAGE_MOTIONBL_EVA	( 5 )	// モーションブラー係数　新しくブレンドされる絵
#define ENCOUNT_LEGEND_PACKAGE_MOTIONBL_EAB	( 13 )	// モーションブラー係数　すでにバッファされている絵




//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
//	伝説ポケモン
//=====================================
typedef struct {
	FLD_MOTION_BL_DATA_PTR	motion;
	GF_CAMERA_PTR	camera;		// カメラデータ
	u32 camera_count;
	s32 wait;
} ENCOUNT_LEGEND_NORMAL;

//-------------------------------------
//	PACKAGEポケモン
//=====================================
typedef struct {
	FLD_MOTION_BL_DATA_PTR	motion;
	GF_CAMERA_PTR	camera;		// カメラデータ
	ENC_MOVE_WORK purse;
	ENC_ADDMOVE_WORK_FX camra_zoom;
	s32 wait;
} ENCOUNT_LEGEND_PACKAGE;



// カメラデータ設定
static void ENC_LegendCameraSet( FIELDSYS_WORK* fsys, const ENCOUNT_LEGEND_CAMERA_TBL* cp_data )
{
	CAMERA_ANGLE angle;
	
	// カメラパラメータ設定
	GFC_SetCameraPerspWay( cp_data->purse, fsys->camera_ptr );	
	GFC_SetCameraDistance( cp_data->dist, fsys->camera_ptr );
	angle.x = cp_data->angle_x;
	angle.y = cp_data->angle_y;
	angle.z = 0;
	GFC_SetCameraAngleRev( &angle, fsys->camera_ptr );
}


//----------------------------------------------------------------------------
/**
 *	@brief	伝説ポケモン　エンカウント
 */
//-----------------------------------------------------------------------------
void EncountEffect_Legend_Normal(TCB_PTR tcb, void* work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	ENCOUNT_LEGEND_NORMAL* task_w = eew->work;

	switch(eew->seq){
	case ENCOUNT_LEGEND_NORMAL_START:	// 初期化処理
		eew->work = sys_AllocMemory(HEAPID_FIELD, sizeof(ENCOUNT_LEGEND_NORMAL));
		memset( eew->work, 0, sizeof(ENCOUNT_LEGEND_NORMAL) );
		
		GF_Disp_GX_VisibleControl(GX_PLANEMASK_BG1, VISIBLE_OFF);
		GF_Disp_GX_VisibleControl(GX_PLANEMASK_BG2, VISIBLE_OFF);
		GF_Disp_GX_VisibleControl(GX_PLANEMASK_BG3, VISIBLE_OFF);
		
		eew->seq++;
		break;
		
	case ENCOUNT_LEGEND_NORMAL_FLASH_INI:
		EncountFlashTask(MASK_MAIN_DISPLAY, 16, 16,  &eew->wait, 1);

		eew->seq++;
		break;

	case ENCOUNT_LEGEND_NORMAL_FLASH_WAIT:
		if(eew->wait){
			eew->seq++;
		}
		break;

	case ENCOUNT_LEGEND_NORMAL_MOTIONBL_S:	// モーションブラーを使用する準備
		task_w->motion = FLDMotionBl_Init(ENCOUNT_LEGEND_NORMAL_MOTIONBL_EVA, ENCOUNT_LEGEND_NORMAL_MOTIONBL_EAB);
		task_w->camera_count = 0;
		task_w->wait = EncLegendNormalCamera[ task_w->camera_count ].wait;
		eew->seq++;
		break;

	case ENCOUNT_LEGEND_NORMAL_CAMERA_SET:
		task_w->wait --;
		if( task_w->wait < 0 ){

			// カメラ設定
			ENC_LegendCameraSet( eew->fsw, &EncLegendNormalCamera[ task_w->camera_count ] );	

			task_w->camera_count++;
			if( task_w->camera_count >= ENCOUNT_LEGEND_CAMERA_TBL_NUM ){
				eew->seq++;
			}else{
				task_w->wait = EncLegendNormalCamera[ task_w->camera_count ].wait;
			}
		}
		break;
		
	case ENCOUNT_LEGEND_NORMAL_FADE:
		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_WHITE, ENCOUNT_LEGEND_NORMAL_FADE_SYNC, 1, HEAPID_FIELD );
		eew->seq++;
		break;
		
	case ENCOUNT_LEGEND_NORMAL_FADEWAIT:
		if( WIPE_SYS_EndCheck() ){
			eew->wait = 0;

			eew->seq++;
		}
		break;

	
	case ENCOUNT_LEGEND_NORMAL_END:		// 後始末
		WIPE_SetBrightness( WIPE_DISP_SUB, WIPE_FADE_WHITE );

		// 元に戻す
		FLDMotionBl_Delete(&task_w->motion);

		// ブライトネス解除
		G2_BlendNone();
		
		if(eew->end != NULL){
			*(eew->end) = TRUE;		// タスク終了を報告
		}
		ENC_End( eew, tcb );
		WIPE_SetBrightness( WIPE_DISP_SUB, WIPE_FADE_WHITE );
		break;
	}	
}


//----------------------------------------------------------------------------
/**
 *	@brief	ﾊﾟｯｹｰｼﾞポケモン　エンカウント
 */
//-----------------------------------------------------------------------------
void EncountEffect_Legend_Package(TCB_PTR tcb, void* work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	ENCOUNT_LEGEND_PACKAGE* task_w = eew->work;
	BOOL result;

	switch(eew->seq){
	case ENCOUNT_LEGEND_PACKAGE_START:	// 初期化処理
		eew->work = sys_AllocMemory(HEAPID_FIELD, sizeof(ENCOUNT_LEGEND_PACKAGE));
		memset( eew->work, 0, sizeof(ENCOUNT_LEGEND_PACKAGE) );
		
		GF_Disp_GX_VisibleControl(GX_PLANEMASK_BG1, VISIBLE_OFF);
		GF_Disp_GX_VisibleControl(GX_PLANEMASK_BG2, VISIBLE_OFF);
		GF_Disp_GX_VisibleControl(GX_PLANEMASK_BG3, VISIBLE_OFF);
		
		eew->seq++;
		break;
		
	case ENCOUNT_LEGEND_PACKAGE_FLASH_INI:
		EncountFlashTask(MASK_MAIN_DISPLAY, 16, 16,  &eew->wait, 1);

		eew->seq++;
		break;

	case ENCOUNT_LEGEND_PACKAGE_FLASH_WAIT:
		if(eew->wait){
			eew->seq++;
		}
		break;

	case ENCOUNT_LEGEND_PACKAGE_MOTIONBL_S:	// モーションブラーを使用する準備
		task_w->motion = FLDMotionBl_Init(ENCOUNT_LEGEND_PACKAGE_MOTIONBL_EVA, ENCOUNT_LEGEND_PACKAGE_MOTIONBL_EAB);

		// パース設定
		{
			u16 pers = GFC_GetCameraPerspWay( eew->fsw->camera_ptr );
			ENC_MoveReq( &task_w->purse, pers, pers + ENCOUNT_LEGEND_PACKAGE_PURSE_MOVE, ENCOUNT_LEGEND_PACKAGE_PURSE_SYNC );
		}
		
		eew->seq++;
		break;

	case ENCOUNT_LEGEND_PACKAGE_PURSE:
		result = ENC_MoveMain( &task_w->purse );
		GFC_SetCameraPerspWay( task_w->purse.x, eew->fsw->camera_ptr );	
		if( result == TRUE ){
			eew->seq++;
			task_w->wait = ENCOUNT_LEGEND_PACKAGE_CAMERA_MOVE_WAIT;
		}
		break;

	case ENCOUNT_LEGEND_PACKAGE_CAMERA:
		task_w->wait --;
		if( task_w->wait < 0 ){
			fx32 dist = GFC_GetCameraDistance( eew->fsw->camera_ptr );	
			ENC_AddMoveReqFx( &task_w->camra_zoom, 
					dist, 
					dist + ENCOUNT_LEGEND_PACKAGE_CAMERA_MOVE,
					ENCOUNT_LEGEND_PACKAGE_CAMERA_SS, 
					ENCOUNT_LEGEND_PACKAGE_CAMERA_SYNC );

			eew->seq ++;
		}
		break;
		
	case ENCOUNT_LEGEND_PACKAGE_CAMERAWAIT:
		result = ENC_AddMoveMainFx( &task_w->camra_zoom );
		GFC_SetCameraDistance( task_w->camra_zoom.x, eew->fsw->camera_ptr );
		if( result == TRUE ){
			eew->seq++;
		}
		break;
		
	case ENCOUNT_LEGEND_PACKAGE_FADE:
		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_WHITE, ENCOUNT_LEGEND_PACKAGE_FADE_SYNC, 1, HEAPID_FIELD );
		eew->seq++;
		break;
		
	case ENCOUNT_LEGEND_PACKAGE_FADEWAIT:
		if( WIPE_SYS_EndCheck() ){
			eew->wait = 0;

			eew->seq++;
		}
		break;

	
	case ENCOUNT_LEGEND_PACKAGE_END:		// 後始末
		WIPE_SetBrightness( WIPE_DISP_SUB, WIPE_FADE_WHITE );

		// 元に戻す
		FLDMotionBl_Delete(&task_w->motion);

		// ブライトネス解除
		G2_BlendNone();
		
		if(eew->end != NULL){
			*(eew->end) = TRUE;		// タスク終了を報告
		}
		ENC_End( eew, tcb );
		WIPE_SetBrightness( WIPE_DISP_SUB, WIPE_FADE_WHITE );
		break;
	}	
}

//-----------------------------------------------------------------------------
/**
 * 
 *			特殊トレーナー　エンカウント[ SP_TR ]
 * 
 */
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
//	ジムリーダー
//=====================================
// シーケンス
enum{
	ENCOUNT_SPTR_GYM_START,
	ENCOUNT_SPTR_GYM_FLASH_INI,
	ENCOUNT_SPTR_GYM_FLASH_WAIT,
	ENCOUNT_SPTR_GYM_BG_IN,
	ENCOUNT_SPTR_GYM_BG_INWAIT,
	ENCOUNT_SPTR_GYM_VS_ANMWAIT,
	ENCOUNT_SPTR_GYM_OAM_IN,
	ENCOUNT_SPTR_GYM_OAM_INWAIT,
	ENCOUNT_SPTR_GYM_OAM_FLASH_OUT,
	ENCOUNT_SPTR_GYM_OAM_FLASH_OUT_WAIT,
	ENCOUNT_SPTR_GYM_OAM_FLASH_IN,
	ENCOUNT_SPTR_GYM_OAM_FLASH_IN_WAIT,
	ENCOUNT_SPTR_GYM_WAIT,
	ENCOUNT_SPTR_GYM_WIPE,
	ENCOUNT_SPTR_GYM_WIPEWAIT,
	ENCOUNT_SPTR_GYM_END,
};
// OAMデータ
// アクターデータ数
#define ENCOUNT_SPTR_GYM_CELL_NUM		( 8 )	// ワーク数
#define ENCOUNT_SPTR_GYM_CELL_RES_NUM	( 3 )	// リソース数
#define ENCOUNT_SPTR_GYM_CELL_CONT_ID	( 600000 )
#define ENCOUNT_SPTR_GYM_LEADER_SCALE_DEF	( FX32_CONST( 1.0f ) )	// リーダーの基本の拡大値
#define ENCOUNT_SPTR_GYM_LEADER_DRAW_BOTTOM	( 30 )	// 下はこのサイズまで表示する
#define ENCOUNT_SPTR_GYM_LEADER_COLOR_FADE		( GX_RGB( 0,0,0 ) )
#define ENCOUNT_SPTR_GYM_LEADER_COLOR_FADE_EVY	( 14 )
#define ENCOUNT_SPTR_GYM_LEADER_CHAR_OFS_CX	(0)	// キャラクタデータ書き出す時の、Xキャラクタオフセット値

#define ENCOUNT_SPTR_GYM_ZIGUZAGU_IN_WND ( GX_WND_PLANEMASK_BG0|GX_WND_PLANEMASK_BG1|GX_WND_PLANEMASK_BG2|GX_WND_PLANEMASK_BG3|GX_WND_PLANEMASK_OBJ)				// ジグザグ動さウィンドウ中
#define ENCOUNT_SPTR_GYM_ZIGUZAGU_OUT_WND ( GX_WND_PLANEMASK_BG0|GX_WND_PLANEMASK_BG1|GX_WND_PLANEMASK_BG2|GX_WND_PLANEMASK_OBJ)				// ジグザグ動さウィンドウ外
enum{	//VSOAM
	ENCOUNT_SPTR_GYM_VSOAM_ANM0,
	ENCOUNT_SPTR_GYM_VSOAM_ANM1,
	ENCOUNT_SPTR_GYM_VSOAM_ANM2,
	ENCOUNT_SPTR_GYM_VSOAM,
	ENCOUNT_SPTR_GYM_VSOAM_NUM,
};
#define ENCOUNT_SPTR_GYM_TRNAME_X		( 0 )	// トレーナー名Xキャラ座標
#define ENCOUNT_SPTR_GYM_TRNAME_Y		( 10 )	// トレーナー名Yキャラ座標
#define ENCOUNT_SPTR_GYM_TRNAME_SIZX	( 16 )	// トレーナー名BMPXキャラサイズ
#define ENCOUNT_SPTR_GYM_TRNAME_SIZY	( 2 )	// トレーナー名BMPYキャラサイズ
#define ENCOUNT_SPTR_GYM_TRNAME_CGX		( 1 )
#define ENCOUNT_SPTR_GYM_TRNAME_PAL		( 2 )
#define ENCOUNT_SPTR_GYM_TRNAME_COL		( GF_PRINTCOLOR_MAKE( 1, 2, 0 ) )

// 動さパラメータ	（触ってよい部分）
#define ENCOUNT_SPTR_GYM_OAM_START_X	( 272*FX32_ONE )	// OAM動さ開始X
#define ENCOUNT_SPTR_GYM_OAM_X_SS		( -64*FX32_ONE )	// 初期速度
#define ENCOUNT_SPTR_GYM_OAM_START_Y	( 66*FX32_ONE )		// OAMY座標
#define ENCOUNT_SPTR_GYM_OAM_MOVE_SYNC	( 4 )				// OAM動さシンク
#define ENCOUNT_SPTR_GYM_TRNAME_OFS_X	( -92 )				// OAMの位置から見てTRNAMEをどの位置に表示するか
#define ENCOUNT_SPTR_GYM_ZIGUZAGU_DOT	( 8 )				// ジグザグ区切りYドット数
#define ENCOUNT_SPTR_GYM_ZIGUZAGU_WIDTH	( 16 )				// ジグザグ幅
#define ENCOUNT_SPTR_GYM_ZIGUZAGU_SYNC	( 6 )				// ジグザグ動さシンク
#define ENCOUNT_SPTR_GYM_BG_CY			( 68 )	// BGの中心
#define ENCOUNT_SPTR_GYM_BG_AUTOMOVE_X	( 30 )	// AUTO動さスピード
#define ENCOUNT_SPTR_GYM_BG_AFTER_WAIT	( 10 )				// BG動さ後ウエイト
#define	ENCOUNT_SPTR_GYM_FLASHOUT_SYNC	( 3 )				// フラッシュアウトシンク数
#define	ENCOUNT_SPTR_GYM_FLASHIN_SYNC	( 3 )				// フラッシュインシンク数
#define ENCOUNT_SPTR_GYM_FLASH_AFTER_WAIT	( 26 )			// フラッシュ後のウエイト
#define ENCOUNT_SPTR_GYM_SCALE_S			( FX32_CONST(1.0f) )	// 拡大開始
#define ENCOUNT_SPTR_GYM_SCALE_E			( FX32_CONST(1.0f) )	// 拡大開始
#define ENCOUNT_SPTR_GYM_FADEOUT_SYNC		( 15 )			// フェードアウトシンク
#define ENCOUNT_SPTR_GYM_VSMARK_X		( FX32_CONST( 72 ) )	// VSMARKの位置
#define ENCOUNT_SPTR_GYM_VSMARK_Y		( FX32_CONST( 74 ) )	// VSMARKの位置
#define ENCOUNT_SPTR_GYM_VSANMSYNC		( 6 )	// １つの拡縮アニメのシンク数
#define ENCOUNT_SPTR_GYM_VSANMTIMING	( 3 )	//	次を出すタイミング
#define ENCOUNT_SPTR_GYM_3DAREA_BRIGHTNESS	( -14 )// 3D面を暗くする値


	
// それぞれで変わるところ
typedef struct {
	fx32 end_x;
	u32	trno;
	u16 type;
	u16 tr_oam_chr_ofs_cx;
	u8	nclr;
	u8	ncgr;
	u8	ncer;
	u8	nanr;
	u8	bg_nclr;
	u8	bg_ncgr;
	u8	bg_nscr;
	u8	pad;
} ENCOUNT_SPTR_GYM_PARAM;
// それぞれの定数
static const ENCOUNT_SPTR_GYM_PARAM ENCOUNT_SPTR_GymDef[ 8 ] = {
	{	// 岩
		214*FX32_ONE,			// 動さ完了X座標
		LEADER1_01,
		TRTYPE_LEADER1,			// リーダータイプ
		ENC_OAM_TR_CHAR_CUT_CX,	// OAMキャラのオフセット
		NARC_field_encounteffect_trpl_face01_NCLR,
		NARC_field_encounteffect_trpl_face01_NCGR,
		NARC_field_encounteffect_trpl_face01_NCER,
		NARC_field_encounteffect_trpl_face01_NANR,
		NARC_field_encounteffect_cutin_gym01_NCLR,
		NARC_field_encounteffect_cutin_gym01_NCGR,
		NARC_field_encounteffect_cutin_gym01_NSCR,
		0,
	},
	{	// 草
		214*FX32_ONE,			// 動さ完了X座標
		LEADER2_01,
		TRTYPE_LEADER2,			// リーダータイプ
		ENC_OAM_TR_CHAR_CUT_CX,	// OAMキャラのオフセット
		NARC_field_encounteffect_trpl_face02_NCLR,
		NARC_field_encounteffect_trpl_face02_NCGR,
		NARC_field_encounteffect_trpl_face02_NCER,
		NARC_field_encounteffect_trpl_face02_NANR,
		NARC_field_encounteffect_cutin_gym02_NCLR,
		NARC_field_encounteffect_cutin_gym02_NCGR,
		NARC_field_encounteffect_cutin_gym02_NSCR,
		0,
	},
	{	// 水
		214*FX32_ONE,			// 動さ完了X座標
		LEADER3_01,
		TRTYPE_LEADER3,			// リーダータイプ
		ENCOUNT_SPTR_GYM_LEADER_CHAR_OFS_CX,// OAMキャラのオフセット値	基本的に水だけが、ENCOUNT_SPTR_GYM_LEADER_CHAR_OFS_CX
		NARC_field_encounteffect_trpl_face03_NCLR,
		NARC_field_encounteffect_trpl_face03_NCGR,
		NARC_field_encounteffect_trpl_face03_NCER,
		NARC_field_encounteffect_trpl_face03_NANR,
		NARC_field_encounteffect_cutin_gym03_NCLR,
		NARC_field_encounteffect_cutin_gym03_NCGR,
		NARC_field_encounteffect_cutin_gym03_NSCR,
		0,
	},
	{	// 格闘
		214*FX32_ONE,			// 動さ完了X座標
		LEADER4_01,
		TRTYPE_LEADER4,			// リーダータイプ
		ENC_OAM_TR_CHAR_CUT_CX,	// OAMキャラのオフセット
		NARC_field_encounteffect_trpl_face04_NCLR,
		NARC_field_encounteffect_trpl_face04_NCGR,
		NARC_field_encounteffect_trpl_face04_NCER,
		NARC_field_encounteffect_trpl_face04_NANR,
		NARC_field_encounteffect_cutin_gym04_NCLR,
		NARC_field_encounteffect_cutin_gym04_NCGR,
		NARC_field_encounteffect_cutin_gym04_NSCR,
		0,
	},
	{	// ゴースト
		214*FX32_ONE,			// 動さ完了X座標
		LEADER5_01,
		TRTYPE_LEADER5,			// リーダータイプ
		ENC_OAM_TR_CHAR_CUT_CX,	// OAMキャラのオフセット
		NARC_field_encounteffect_trpl_face05_NCLR,
		NARC_field_encounteffect_trpl_face05_NCGR,
		NARC_field_encounteffect_trpl_face05_NCER,
		NARC_field_encounteffect_trpl_face05_NANR,
		NARC_field_encounteffect_cutin_gym05_NCLR,
		NARC_field_encounteffect_cutin_gym05_NCGR,
		NARC_field_encounteffect_cutin_gym05_NSCR,
		0,
	},
	{	// 氷
		214*FX32_ONE,			// 動さ完了X座標
		LEADER6_01,
		TRTYPE_LEADER6,			// リーダータイプ
		ENC_OAM_TR_CHAR_CUT_CX,	// OAMキャラのオフセット
		NARC_field_encounteffect_trpl_face06_NCLR,
		NARC_field_encounteffect_trpl_face06_NCGR,
		NARC_field_encounteffect_trpl_face06_NCER,
		NARC_field_encounteffect_trpl_face06_NANR,
		NARC_field_encounteffect_cutin_gym06_NCLR,
		NARC_field_encounteffect_cutin_gym06_NCGR,
		NARC_field_encounteffect_cutin_gym06_NSCR,
		0,
	},
	{	// 鋼
		214*FX32_ONE,			// 動さ完了X座標
		LEADER7_01,
		TRTYPE_LEADER7,			// リーダータイプ
		ENC_OAM_TR_CHAR_CUT_CX,	// OAMキャラのオフセット
		NARC_field_encounteffect_trpl_face07_NCLR,
		NARC_field_encounteffect_trpl_face07_NCGR,
		NARC_field_encounteffect_trpl_face07_NCER,
		NARC_field_encounteffect_trpl_face07_NANR,
		NARC_field_encounteffect_cutin_gym07_NCLR,
		NARC_field_encounteffect_cutin_gym07_NCGR,
		NARC_field_encounteffect_cutin_gym07_NSCR,
		0,
	},
	{	// 電気
		214*FX32_ONE,			// 動さ完了X座標
		LEADER8_01,
		TRTYPE_LEADER8,			// リーダータイプ
		ENC_OAM_TR_CHAR_CUT_CX,	// OAMキャラのオフセット
		NARC_field_encounteffect_trpl_face08_NCLR,
		NARC_field_encounteffect_trpl_face08_NCGR,
		NARC_field_encounteffect_trpl_face08_NCER,
		NARC_field_encounteffect_trpl_face08_NANR,
		NARC_field_encounteffect_cutin_gym08_NCLR,
		NARC_field_encounteffect_cutin_gym08_NCGR,
		NARC_field_encounteffect_cutin_gym08_NSCR,
		0,
	},
};


//-------------------------------------
//	四天王　チャンピオン　リーダー
//=====================================
// シーケンス
enum{
	ENCOUNT_SPTR_CHANP_START,
	ENCOUNT_SPTR_CHANP_START2,
	ENCOUNT_SPTR_CHANP_FLASH_INI,
	ENCOUNT_SPTR_CHANP_FLASH_WAIT,
	ENCOUNT_SPTR_CHANP_GPRINT_WAIT,
	ENCOUNT_SPTR_CHANP_HERO_IN,
	ENCOUNT_SPTR_CHANP_HERO_INWAIT,
	ENCOUNT_SPTR_CHANP_HERO_FLASHOUT,
	ENCOUNT_SPTR_CHANP_HERO_FLASHOUTWAIT,
	ENCOUNT_SPTR_CHANP_HERO_FLASHIN,
	ENCOUNT_SPTR_CHANP_HERO_FLASHINWAIT,
	ENCOUNT_SPTR_CHANP_BT_IN,
	ENCOUNT_SPTR_CHANP_BT_INWAIT,
	ENCOUNT_SPTR_CHANP_WIPEWAIT,
	ENCOUNT_SPTR_CHANP_END,
};
// OAMデータ
// アクターデータ数
#define ENCOUNT_SPTR_CHANP_CELLACT_NUM		( 10 )	// ワーク数
#define ENCOUNT_SPTR_CHANP_CELL_NUM		( 4 )	// ワーク数
#define ENCOUNT_SPTR_CHANP_CELL_RES_NUM	( 4 )	// リソース数
#define ENCOUNT_SPTR_CHANP_CELL_CONT_ID	( 600000 )
#define ENCOUNT_SPTR_CHANP_CELL_SCALE	( FX32_CONST( 2.0f ) )
#define ENCOUNT_SPTR_CHANP_LEADER_DRAW_BOTTOM	( 30 )	// 下はこのサイズまで表示する
#define ENCOUNT_SPTR_CHANP_COLOR_FADE	( GX_RGB( 0,0,0 ) )
#define ENCOUNT_SPTR_CHANP_COLOR_FADE_EVY	(14)

// 動さパラメータ	（触ってよい部分）
// 主人公動さ
#define ENCOUNT_SPTR_CHANP_HERO_MOVE_SX		( -128*FX32_ONE )	// 主人公　登場開始X座標
#define ENCOUNT_SPTR_CHANP_HERO_MOVE_SS		( 80*FX32_ONE )		// 主人公　登場初速度
#define ENCOUNT_SPTR_CHANP_HERO_MOVE_EX		( 56*FX32_ONE )		// 主人公　登場終了X座標
#define ENCOUNT_SPTR_CHANP_HERO_Y			( 92*FX32_ONE )		// 主人公　Y座標
#define ENCOUNT_SPTR_CHANP_HERO_Y_WAKU_OFS	( 4*FX32_ONE )		// 枠が離れる座標
#define ENCOUNT_SPTR_CHANP_HERO_X_WAKU_OFS	( 16*FX32_ONE )		// 枠が離れる座標
#define ENCOUNT_SPTR_CHANP_HERO_MOVE_SYNC	( 6 )				// 主人公　登場シンク数
#define ENCOUNT_SPTR_CHANP_HERO_FLASHOUT_SYNC	( 3 )		// 主人公フラッシュアウト　シンク数
#define ENCOUNT_SPTR_CHANP_HERO_FLASHIN_SYNC	( 6 )		// 主人公フラッシュイン　シンク数
#define ENCOUNT_SPTR_CHANP_HERO_EFFECT_WAIT		( 3 )

// 四天王　チャンピオン動さ
#define ENCOUNT_SPTR_CHANP_ENEMY_MOVE_SX		( 384*FX32_ONE )	// 主人公　登場開始X座標
#define ENCOUNT_SPTR_CHANP_ENEMY_MOVE_SS		( -80*FX32_ONE )		// 主人公　登場初速度
#define ENCOUNT_SPTR_CHANP_ENEMY_MOVE_EX		( 200*FX32_ONE )		// 主人公　登場終了X座標
#define ENCOUNT_SPTR_CHANP_ENEMY_Y			( 92*FX32_ONE )	// 主人公　Y座標
#define ENCOUNT_SPTR_CHANP_ENEMY_Y_WAKU_OFS	( 4*FX32_ONE )		// 枠が離れる座標
#define ENCOUNT_SPTR_CHANP_ENEMY_X_WAKU_OFS	( -16*FX32_ONE )		// 枠が離れる座標
#define ENCOUNT_SPTR_CHANP_ENEMY_MOVE_SYNC	( 6 )				// 主人公　登場シンク数

//VSアニメ
#define ENCOUNT_SPTR_CHANP_VSMARK_X		( FX32_CONST( 128 ) )	// VSMARKの位置
#define ENCOUNT_SPTR_CHANP_VSMARK_Y		( FX32_CONST( 96 ) )	// VSMARKの位置

// 名前
#define ENCOUNT_SPTR_CHANP_TRNAME_X		( 21 )	// トレーナー名Xキャラ座標
#define ENCOUNT_SPTR_CHANP_TRNAME_Y		( 13 )	// トレーナー名Yキャラ座標
#define ENCOUNT_SPTR_CHANP_TRNAME_SIZX	( 11 )	// トレーナー名BMPXキャラサイズ
#define ENCOUNT_SPTR_CHANP_TRNAME_SIZY	( 2 )	// トレーナー名BMPYキャラサイズ
#define ENCOUNT_SPTR_CHANP_TRNAME_CGX	( 1 )
#define ENCOUNT_SPTR_CHANP_TRNAME_PAL	( 2 )
#define ENCOUNT_SPTR_CHANP_TRNAME_COL	( GF_PRINTCOLOR_MAKE( 1, 2, 0 ) )

// さらにぶつかり合うところの動作
#define ENCOUNT_SPTR_CHANP_BT_IN_WAIT			( 8 )	// 動作開始までのウエイト
#define ENCOUNT_SPTR_CHANP_BT_IN_MOVE_SX		( 0 )	//
#define ENCOUNT_SPTR_CHANP_BT_IN_MOVE_EX		( -FX32_CONST(2) )	// 移動する値
#define ENCOUNT_SPTR_CHANP_BT_IN_MOVE_SSX		( 0 )	// 初速度
#define ENCOUNT_SPTR_CHANP_BT_IN_MOVE_SY		( 0 )	//
#define ENCOUNT_SPTR_CHANP_BT_IN_MOVE_EY		( -FX32_CONST(2) )	// 移動する値
#define ENCOUNT_SPTR_CHANP_BT_IN_MOVE_SSY		( 0 )	// 初速度
#define ENCOUNT_SPTR_CHANP_BT_IN_MOVE_SYNC		( 32 )
#define ENCOUNT_SPTR_CHANP_BT_IN_MOVE_SYNC_C	( 9 )	// シロナ
#define ENCOUNT_SPTR_CHANP_BT_IN_MOVE_CHG		( 2	)	// 動く方向を変更するタイミング

// 最後の白くなるとこ
#define ENCOUNT_SPTR_CHANP_WIPE_OUT_SYNC		( 16 )	// ワイプアウトシンク数
#define ENCOUNT_SPTR_CHANP_OMA_MOVE_X			( FX32_CONST( 192.0f ) )	// OAM動さ距離
#define ENCOUNT_SPTR_CHANP_OMA_MOVE_Y			( FX32_CONST( 192.0f ) )	// OAM動さ距離
#define ENCOUNT_SPTR_CHANP_OAM_MOVE_XSS			( FX32_CONST( 24.0f ) )	// OAM動さ初速度
#define ENCOUNT_SPTR_CHANP_OAM_MOVE_YSS			( FX32_CONST( 24.0f ) )	// OAM動さ初速度
#define ENCOUNT_SPTR_CHANP_WIPE_OUT_SYNC_WIPE	( 8 )	// ワイプアウトシンク数
  

// それぞれの定数
typedef struct{
	u16 enemy_ncl;
	u8	waku_oam_cl;
	u8	bt_in_move_sync;
	u16	enemy_tr_type;
	u16 tr_no;
} ENCOUNT_SPTR_CHANP_PARAM;

static const ENCOUNT_SPTR_CHANP_PARAM EncountSpTr_ChanpParam[ 5 ] = {
	{	// 四天王１
		NARC_field_encounteffect_trpl_face_bigfour1_NCLR,
		NARC_field_encounteffect_cutin_chanpion00_NCLR,
		ENCOUNT_SPTR_CHANP_BT_IN_MOVE_SYNC,
		TRTYPE_BIGFOUR1,
		BIGFOUR1_01
	},
	{	// 四天王２
		NARC_field_encounteffect_trpl_face_bigfour2_NCLR,
		NARC_field_encounteffect_cutin_chanpion01_NCLR,
		ENCOUNT_SPTR_CHANP_BT_IN_MOVE_SYNC,
		TRTYPE_BIGFOUR2,
		BIGFOUR2_01
	},
	{	// 四天王３
		NARC_field_encounteffect_trpl_face_bigfour3_NCLR,
		NARC_field_encounteffect_cutin_chanpion02_NCLR,
		ENCOUNT_SPTR_CHANP_BT_IN_MOVE_SYNC,
		TRTYPE_BIGFOUR3,
		BIGFOUR3_01
	},
	{	// 四天王４
		NARC_field_encounteffect_trpl_face_bigfour4_NCLR,
		NARC_field_encounteffect_cutin_chanpion03_NCLR,
		ENCOUNT_SPTR_CHANP_BT_IN_MOVE_SYNC,
		TRTYPE_BIGFOUR4,
		BIGFOUR4_01
	},
	{	// チャンピオン
		NARC_field_encounteffect_trpl_face_champion_NCLR,
		NARC_field_encounteffect_cutin_chanpion04_NCLR,
		ENCOUNT_SPTR_CHANP_BT_IN_MOVE_SYNC_C,
		TRTYPE_CHAMPION,
		CHAMPION_01
	},

};

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	VSアニメ
//=====================================
typedef struct {
	s16 timing;
	s16 movenum;
	CLACT_WORK_PTR vs[ ENCOUNT_SPTR_GYM_VSOAM_NUM ];
	ENC_MOVE_WORK_FX vsscale[ ENCOUNT_SPTR_GYM_VSOAM_NUM ];
} ENCOUNT_SPTR_GYM_VSANM;
//
//-------------------------------------
//	ジムリーダー
//=====================================
typedef struct {
	ENC_ADDMOVE_WORK_FX oam_move_x;
	ENC_MOVE_WORK flash;
	ENC_MOVE_WORK_FX scale;

	ENC_HB_BG_WND_ZIGUZAGU* p_ziguzagu;
	
	ENC_CLACT_SYS	clact_sys;
	ENC_CLACT_RES_WORK clact_res[2];
	CLACT_WORK_PTR clact;
	ENCOUNT_SPTR_GYM_VSANM vsanm;
	ENC_WND_SETPOSITION wnd_posi;
	ENC_BG_SET_AFFINEPARAM bg_affine;

	GF_BGL_BMPWIN trnamewin;

	BOOL bg_auto_move;
	s32 auto_move_x;

	s32 wait;
} ENCOUNT_SPTR_GYM;

//-------------------------------------
//	四天王　チャンピオン
//=====================================
typedef struct {
	ENC_ADDMOVE_WORK_FX oam_move_x;
	ENC_ADDMOVE_WORK_FX oam_move_y;
	ENC_ADDMOVE_WORK_FX enemy_oam_move_x;
	ENC_MOVE_WORK flash;

	ENC_CLACT_SYS	clact_sys;
	ENC_CLACT_RES_WORK clact_res[ENCOUNT_SPTR_CHANP_CELL_RES_NUM];
	CLACT_WORK_PTR clact[ENCOUNT_SPTR_CHANP_CELL_NUM];
	VecFx32			pos_hero;
	VecFx32			pos_enemy;

	// VSアニメ 
	ENCOUNT_SPTR_GYM_VSANM vsanm;

	// トレーナ名用ビットマップ
	GF_BGL_BMPWIN trnamewin;

	u32 hero_type;	// 主人公タイプ

	s32 wait;
	s32 count;
} ENCOUNT_SPTR_CHANP;


//----------------------------------------------------------------------------
/**
 *	@brief	VSアニメ	初期化
 *	
 *	@param	task_w		タスクワーク
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void EncountEffect_SpTr_VsAnmInit( ENCOUNT_SPTR_GYM_VSANM* p_wk, ENC_CLACT_SYS* p_clset, ENC_CLACT_RES_WORK* p_res, fx32 x, fx32 y, u32 heapID )
{
	int i;

	p_wk->timing = 0;
	p_wk->movenum = 0;

	for( i=0; i<ENCOUNT_SPTR_GYM_VSOAM_NUM; i++ ){
		p_wk->vs[i] = ENC_CLACT_Add( 
				p_clset, p_res,
				x, 
				y,0, 0);
		CLACT_SetDrawFlag( p_wk->vs[i], FALSE );
		if( i!=ENCOUNT_SPTR_GYM_VSOAM ){	// 最後の１つが文字
			CLACT_SetAffineParam( p_wk->vs[i], CLACT_AFFINE_DOUBLE );
			CLACT_AnmChg( p_wk->vs[i], 1 );
			// 拡大縮小データ作成
			ENC_MoveReqFx( &p_wk->vsscale[i],
					FX32_CONST(2),
					FX32_CONST(1),
					ENCOUNT_SPTR_GYM_VSANMSYNC );
		}else{
			// 拡大縮小データ作成
			ENC_MoveReqFx( &p_wk->vsscale[i],
					FX32_CONST(1),
					FX32_CONST(1),
					ENCOUNT_SPTR_GYM_VSANMSYNC );
		}
		
	}
}
static void EncountEffect_SpTr_VsAnmExit( ENCOUNT_SPTR_GYM_VSANM* p_wk )
{
	int i;
	for( i=0; i<ENCOUNT_SPTR_GYM_VSOAM_NUM; i++ ){
		CLACT_Delete( p_wk->vs[i] );
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
static BOOL EncountEffect_SpTr_VsAnmMain( ENCOUNT_SPTR_GYM_VSANM* p_wk )
{
	int i;
	BOOL result;
	BOOL ret = TRUE;
	VecFx32 scale;
		
	// みんなを動かすタイミングを制御
	if( p_wk->movenum < ENCOUNT_SPTR_GYM_VSOAM_NUM ){
		ret = FALSE;	// まだ全部動ききってない
		p_wk->timing--;
		if( p_wk->timing <= 0 ){
			p_wk->timing = ENCOUNT_SPTR_GYM_VSANMTIMING;
			p_wk->movenum ++;
		}
	}

	// 動かしていい人数分動かす
	for( i=0; i<p_wk->movenum; i++ ){
		result = ENC_MoveMainFx( &p_wk->vsscale[i] );

		scale = ENC_MakeVec( 
				p_wk->vsscale[i].x, 
				p_wk->vsscale[i].x, p_wk->vsscale[i].x );
		CLACT_SetScale( p_wk->vs[i], &scale );
		CLACT_SetDrawFlag( p_wk->vs[i], TRUE );
		// 1つでもまだ動ききっていないのがあるなら途中
		if( result == FALSE ){
			ret = FALSE;
		}
	}

	return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナータイプから名前を取得する
 *
 *	@param	type		トレーナータイプ
 *	@param	heapID		ヒープID
 *
 *	@return	名前	破棄が必要　STRBUF_Delete()
 */
//-----------------------------------------------------------------------------
static STRBUF* EncountEffect_SpTr_Gym_TrTypeNameGet( u32 type, u32 heapID )
{
	WORDSET* p_wordset;
	MSGDATA_MANAGER* p_man;
	STRBUF* p_str;
	STRBUF* p_tmp;
	
	p_man = MSGMAN_Create(MSGMAN_TYPE_DIRECT, ARC_MSG, NARC_msg_encount_effect_dat, heapID);
	p_wordset = WORDSET_Create( heapID );
	p_str = STRBUF_Create( 128, heapID );
	p_tmp = STRBUF_Create( 128, heapID );
	MSGMAN_GetString( p_man, msg_00, p_tmp );
	WORDSET_RegisterTrainerName( p_wordset, 0, type );
	WORDSET_ExpandStr( p_wordset, p_str, p_tmp );

	MSGMAN_Delete( p_man );
	WORDSET_Delete( p_wordset );
	STRBUF_Delete( p_tmp );
	return p_str;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ジム専用エンカウントエフェクト
 *
 *	@param	eew		エフェクトワーク
 *	@param	heapID	ヒープID
 *	@param	cp_def	デファイン定義
 *
 *	@retval	TRUE		終了
 *	@retval	FALSE		途中
 */
//-----------------------------------------------------------------------------
static BOOL EncountEffect_SpTr_GymMain( ENCOUNT_EFFECT_WORK *eew, u32 heapID, const ENCOUNT_SPTR_GYM_PARAM* cp_def )
{
	ENCOUNT_SPTR_GYM* task_w = eew->work;
	BOOL result;
	const VecFx32* cp_matrix;
	VecFx32 matrix;
	VecFx32 scale;
	int bottom_y;
	int i;
	STRBUF* p_str;

	switch( eew->seq ){
	case ENCOUNT_SPTR_GYM_START:
		eew->work = sys_AllocMemory( heapID, sizeof(ENCOUNT_SPTR_GYM) );	
		memset( eew->work, 0, sizeof(ENCOUNT_SPTR_GYM) );
		task_w = eew->work;

		// パレット読み込み	
		ArcUtil_HDL_PalSet( eew->p_handle, 
				NARC_field_encounteffect_cutin_gym_font_NCLR,
				PALTYPE_MAIN_BG, ENCOUNT_SPTR_GYM_TRNAME_PAL*0x20, 0x20, heapID );

		// トレーナー名表示用ビットマップ作成
		GF_Disp_GX_VisibleControl(GX_PLANEMASK_BG2, VISIBLE_OFF);
		GF_BGL_BmpWinAdd( eew->fsw->bgl, &task_w->trnamewin, 
				GF_BGL_FRAME2_M, ENCOUNT_SPTR_GYM_TRNAME_X, ENCOUNT_SPTR_GYM_TRNAME_Y,
				ENCOUNT_SPTR_GYM_TRNAME_SIZX, ENCOUNT_SPTR_GYM_TRNAME_SIZY,
				ENCOUNT_SPTR_GYM_TRNAME_PAL, ENCOUNT_SPTR_GYM_TRNAME_CGX );
		GF_BGL_BmpWinDataFill( &task_w->trnamewin, 0 );
		p_str = EncountEffect_SpTr_Gym_TrTypeNameGet( cp_def->trno, heapID );
		GF_STR_PrintColor( &task_w->trnamewin, FONT_SYSTEM,
                                       p_str, 0, 0, MSG_ALLPUT, 
									   ENCOUNT_SPTR_GYM_TRNAME_COL, NULL);
		STRBUF_Delete( p_str );	// 破棄わすれ注意！
 

		// セルアクターセット作成
		ENC_CLACT_Init( &task_w->clact_sys, ENCOUNT_SPTR_GYM_CELL_NUM, ENCOUNT_SPTR_GYM_CELL_RES_NUM );

		// グラフィック設定
		ENC_CLACT_ResLoadEasy(
				eew->p_handle,
				&task_w->clact_sys,
				&task_w->clact_res[0],
				cp_def->nclr, 1,
				cp_def->ncgr,
				cp_def->ncer,
				cp_def->nanr,
				ENCOUNT_SPTR_GYM_CELL_CONT_ID
				);

		// VSグラフィック
		ENC_CLACT_ResLoadEasy(
				eew->p_handle,
				&task_w->clact_sys,
				&task_w->clact_res[1],
				NARC_field_encounteffect_cutin_gym_vs_NCLR, 1,
				NARC_field_encounteffect_cutin_gym_vs_NCGR,
				NARC_field_encounteffect_cutin_gym_vs_NCER,
				NARC_field_encounteffect_cutin_gym_vs_NANR,
				ENCOUNT_SPTR_GYM_CELL_CONT_ID + 1
				);

		// アクター登録
		task_w->clact = ENC_CLACT_Add( 
				&task_w->clact_sys, &task_w->clact_res[0],
				ENCOUNT_SPTR_GYM_OAM_START_X, 
				ENCOUNT_SPTR_GYM_OAM_START_Y,0, 0);
		CLACT_SetDrawFlag( task_w->clact, FALSE );
		EncountEffect_SpTr_VsAnmInit( &task_w->vsanm, &task_w->clact_sys, &task_w->clact_res[1], ENCOUNT_SPTR_GYM_VSMARK_X, ENCOUNT_SPTR_GYM_VSMARK_Y, heapID );
//		CLACT_SetAffineParam( task_w->clact, CLACT_AFFINE_DOUBLE );

		// カラーパレットを設定
		ENC_CLACT_ResColorChange( task_w->clact, heapID, cp_def->type, ENCOUNT_SPTR_GYM_LEADER_COLOR_FADE_EVY, ENCOUNT_SPTR_GYM_LEADER_COLOR_FADE );
		// ジムリーダーキャラクタパレット転送
//		ENC_CLACT_ResSetSoftSpriteDataTrOam( task_w->clact, heapID, cp_def->type, ENCOUNT_SPTR_GYM_LEADER_COLOR_FADE_EVY, ENCOUNT_SPTR_GYM_LEADER_COLOR_FADE, cp_def->tr_oam_chr_ofs_cx );

		// アフィンBG設定
//		ENC_BG_SetAffineCont( eew->fsw->bgl );

		// ジグザグウィンドウ初期化
		task_w->p_ziguzagu = ENC_BG_WndZiguzagu_Alloc();

		eew->seq ++;
		break;
		
	case ENCOUNT_SPTR_GYM_FLASH_INI:

		EncountFlashTask(MASK_MAIN_DISPLAY, 16, 16,  &eew->wait, 1);
		eew->seq++;
		break;
		
	case ENCOUNT_SPTR_GYM_FLASH_WAIT:
		if( eew->wait ){
			eew->seq++;
		}
		break;

	case ENCOUNT_SPTR_GYM_BG_IN:
		
		// BG設定
		ENC_BG_SetNormalBG(
				eew->p_handle,
				cp_def->bg_nscr,
				cp_def->bg_ncgr,
				cp_def->bg_nclr,
				0, 1,
				eew->fsw->bgl, GF_BGL_FRAME3_M );
		task_w->bg_auto_move = TRUE;	// BGAUTO動さ

		// ジグザグウィンドウスタート
		ENC_BG_WndZiguzagu_Start( eew, task_w->p_ziguzagu, 
				ENCOUNT_SPTR_GYM_ZIGUZAGU_SYNC,
				ENCOUNT_SPTR_GYM_ZIGUZAGU_DOT,
				ENCOUNT_SPTR_GYM_ZIGUZAGU_WIDTH,
				ENCOUNT_SPTR_GYM_ZIGUZAGU_IN_WND,
				ENCOUNT_SPTR_GYM_ZIGUZAGU_OUT_WND );
/*
		// BGAUTO動さのために拡大パラメータをここで作成
		ENC_MoveReqFx( &task_w->scale,
				ENCOUNT_SPTR_GYM_SCALE_S,
				ENCOUNT_SPTR_GYM_SCALE_E,
				ENCOUNT_SPTR_GYM_FADEOUT_SYNC );
//*/
		
		// BG面表示
		GF_Disp_GX_VisibleControl(GX_PLANEMASK_BG3, VISIBLE_ON);
		
		eew->seq++;
		break;
		
	case ENCOUNT_SPTR_GYM_BG_INWAIT:

		if( ENC_HBlankEndCheck( eew ) ){
			eew->seq++;
			// ウィンドウエフェクト破棄
			ENC_BG_WndZiguzagu_Delete( task_w->p_ziguzagu );
		
			task_w->wait = ENCOUNT_SPTR_GYM_BG_AFTER_WAIT;
		}
		break;

	case ENCOUNT_SPTR_GYM_VS_ANMWAIT:

		task_w->wait --;
		if( task_w->wait >= 0 ){
			break;
		}
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

		// VSを出す
		result = EncountEffect_SpTr_VsAnmMain( &task_w->vsanm );
		if( result == TRUE ){
			eew->seq++;
		}
		break;
		
		
	case ENCOUNT_SPTR_GYM_OAM_IN:

#if 0
		// ジムリーダー基本拡大値設定
		scale = ENC_MakeVec( 
				ENCOUNT_SPTR_GYM_LEADER_SCALE_DEF, 
				ENCOUNT_SPTR_GYM_LEADER_SCALE_DEF, 0 );
		CLACT_SetScale( task_w->clact, &scale );
		
		// 囲いウィンドウ設定
		ENC_WND_SetScaleWnd( 
				ENCOUNT_SPTR_GYM_OAM_START_Y,
				ENCOUNT_SPTR_GYM_BG_CY+ENCOUNT_SPTR_GYM_LEADER_DRAW_BOTTOM,
				ENCOUNT_SPTR_GYM_LEADER_SCALE_DEF,
				64, 0, &task_w->wnd_posi );
		G2_SetWnd0InsidePlane( GX_WND_PLANEMASK_BG0|GX_WND_PLANEMASK_BG1|GX_WND_PLANEMASK_BG2|GX_WND_PLANEMASK_BG3|GX_WND_PLANEMASK_OBJ, TRUE );
		G2_SetWndOutsidePlane( GX_WND_PLANEMASK_BG0|GX_WND_PLANEMASK_BG1|GX_WND_PLANEMASK_BG2|GX_WND_PLANEMASK_BG3, TRUE );
		GX_SetVisibleWnd(GX_WNDMASK_W0);
#endif
		
		ENC_AddMoveReqFx( &task_w->oam_move_x, 
				ENCOUNT_SPTR_GYM_OAM_START_X,
				cp_def->end_x,
				ENCOUNT_SPTR_GYM_OAM_X_SS,
				ENCOUNT_SPTR_GYM_OAM_MOVE_SYNC );
		CLACT_SetDrawFlag( task_w->clact, TRUE );
		CLACT_BGPriorityChg( task_w->clact, 0 );

		matrix = ENC_MakeVec( 
				task_w->oam_move_x.x, 
				ENCOUNT_SPTR_GYM_OAM_START_Y, 0 );
		CLACT_SetMatrix( task_w->clact, &matrix );

		eew->seq++;
		break;
		
	case ENCOUNT_SPTR_GYM_OAM_INWAIT:

		result = ENC_AddMoveMainFx( &task_w->oam_move_x );
		matrix = ENC_MakeVec( 
				task_w->oam_move_x.x, 
				ENCOUNT_SPTR_GYM_OAM_START_Y, 0 );
		CLACT_SetMatrix( task_w->clact, &matrix );
		
#if 0
		// 囲いウィンドウ設定	OAMは反映が1シンク遅い
		cp_matrix = CLACT_GetMatrix( task_w->clact );
		ENC_WND_SetScaleWnd( 
				cp_matrix->y,
				ENCOUNT_SPTR_GYM_BG_CY+ENCOUNT_SPTR_GYM_LEADER_DRAW_BOTTOM,
				ENCOUNT_SPTR_GYM_LEADER_SCALE_DEF,
				64, 0, &task_w->wnd_posi);
#endif

		if( result == TRUE ){
			eew->seq++;
		}
		break;

	case ENCOUNT_SPTR_GYM_OAM_FLASH_OUT:
		ENC_MoveReq( &task_w->flash, 0, 16, ENCOUNT_SPTR_GYM_FLASHOUT_SYNC );
		task_w->wait = ENCOUNT_SPTR_GYM_BG_AFTER_WAIT;
		eew->seq++;
		break;
		
	case ENCOUNT_SPTR_GYM_OAM_FLASH_OUT_WAIT:
		task_w->wait --;
		if( task_w->wait >= 0 ){
			break;
		}

		result = ENC_MoveMain( &task_w->flash );
		ENC_SetMasterBrightnessVblank( &task_w->flash.x );
		if( result == TRUE ){
			// OAMのカラーパレット変更
			ENC_CLACT_ResColorChange( task_w->clact, heapID, cp_def->type, 0, ENCOUNT_SPTR_GYM_LEADER_COLOR_FADE );

			// ３D面にブライトネスをカケル（GYMで半透明は使ってないだろうということで半透明つかっちゃいます）
			SetBrightness( ENCOUNT_SPTR_GYM_3DAREA_BRIGHTNESS,
					PLANEMASK_BG0|PLANEMASK_BD, MASK_MAIN_DISPLAY );

			//　文字を出す	ジムリーダーOAMの位置を基準に出す
			GF_BGL_ScrollReq( eew->fsw->bgl, GF_BGL_FRAME2_M, GF_BGL_SCROLL_X_SET, -((task_w->oam_move_x.x>>FX32_SHIFT) + ENCOUNT_SPTR_GYM_TRNAME_OFS_X) );
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_BGL_PrioritySet( GF_BGL_FRAME2_M, 0 );
			eew->seq++;
		}
		break;
		
	case ENCOUNT_SPTR_GYM_OAM_FLASH_IN:
		ENC_MoveReq( &task_w->flash, 16, 0, ENCOUNT_SPTR_GYM_FLASHIN_SYNC );
		eew->seq++;
		break;
		
	case ENCOUNT_SPTR_GYM_OAM_FLASH_IN_WAIT:
		result = ENC_MoveMain( &task_w->flash );
		ENC_SetMasterBrightnessVblank( &task_w->flash.x );
		if( result == TRUE ){
			eew->seq++;
			task_w->wait = ENCOUNT_SPTR_GYM_FLASH_AFTER_WAIT;
		}
		break;
		
	case ENCOUNT_SPTR_GYM_WAIT:
		task_w->wait --;
		if( task_w->wait < 0 ){
			eew->seq ++;
		}
		break;
		
	case ENCOUNT_SPTR_GYM_WIPE:

		// ホワイトアウト
		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_WHITE, ENCOUNT_SPTR_GYM_FADEOUT_SYNC, 1, HEAPID_FIELD );
		eew->seq ++;
		break;
		
	case ENCOUNT_SPTR_GYM_WIPEWAIT:
#if 0
		ENC_MoveMainFx( &task_w->scale );
		// FX32_ONE引くのは、scaleの開始が1からだから
		scale = ENC_MakeVec( 
				ENCOUNT_SPTR_GYM_LEADER_SCALE_DEF + task_w->scale.x - FX32_ONE, 
				ENCOUNT_SPTR_GYM_LEADER_SCALE_DEF + task_w->scale.x - FX32_ONE, 0 );
		CLACT_SetScale( task_w->clact, &scale );

		cp_matrix = CLACT_GetMatrix( task_w->clact );

		// 底辺の座標を求める
		bottom_y = ENC_WND_SetScaleWndToolMakeBottomY( task_w->scale.x, ENCOUNT_SPTR_GYM_BG_CY, ENCOUNT_SPTR_GYM_LEADER_DRAW_BOTTOM );
		// 囲いウィンドウ設定
		ENC_WND_SetScaleWnd( 
				cp_matrix->y,
				bottom_y,
				scale.y, 64, 0, &task_w->wnd_posi);
#endif	
		
		if( WIPE_SYS_EndCheck() ) {
			eew->seq ++;
		}
		break;
		
	case ENCOUNT_SPTR_GYM_END:
		WIPE_SetBrightness( WIPE_DISP_SUB, WIPE_FADE_WHITE );
		
		if(eew->end != NULL){
			*(eew->end) = TRUE;		// タスク終了を報告
		}
		// OAM破棄
		CLACT_Delete( task_w->clact );
		EncountEffect_SpTr_VsAnmExit( &task_w->vsanm );
		ENC_CLACT_ResDeleteEasy( &task_w->clact_sys, &task_w->clact_res[0] );
		ENC_CLACT_ResDeleteEasy( &task_w->clact_sys, &task_w->clact_res[1] );
		ENC_CLACT_Delete( &task_w->clact_sys );

		// BMP
		GF_BGL_BmpWinDel( &task_w->trnamewin );

		// ウィンドウ破棄
		GX_SetVisibleWnd(GX_WNDMASK_NONE);

		// ブライトネスはき
		SetBrightness( BRIGHTNESS_NORMAL,
				PLANEMASK_NONE, MASK_MAIN_DISPLAY );

		// スクロール
		GF_BGL_ScrollSet( eew->fsw->bgl, GF_BGL_FRAME2_M, GF_BGL_SCROLL_X_SET, 0 );

		return TRUE;
	}

	if( task_w->bg_auto_move == TRUE ){
		GF_BGL_ScrollReq( eew->fsw->bgl, GF_BGL_FRAME3_M, 
				GF_BGL_SCROLL_X_SET, task_w->auto_move_x );

		task_w->auto_move_x = (task_w->auto_move_x + ENCOUNT_SPTR_GYM_BG_AUTOMOVE_X) % 512;
	}

	if( eew->seq != ENCOUNT_SPTR_GYM_END ){
		CLACT_Draw( task_w->clact_sys.cas );
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief		各ジム
 */
//-----------------------------------------------------------------------------
void EncountEffect_SPTR_IwaGym(TCB_PTR tcb, void* work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	BOOL result;
	result = EncountEffect_SpTr_GymMain( eew, HEAPID_FIELD, &ENCOUNT_SPTR_GymDef[0] );
	if( result == TRUE ){
		ENC_End( eew, tcb );
	}
}
void EncountEffect_SPTR_KusaGym(TCB_PTR tcb, void* work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	BOOL result;
	result = EncountEffect_SpTr_GymMain( eew, HEAPID_FIELD, &ENCOUNT_SPTR_GymDef[1] );
	if( result == TRUE ){
		ENC_End( eew, tcb );
	}
}
void EncountEffect_SPTR_MizuGym(TCB_PTR tcb, void* work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	BOOL result;
	result = EncountEffect_SpTr_GymMain( eew, HEAPID_FIELD, &ENCOUNT_SPTR_GymDef[2] );
	if( result == TRUE ){
		ENC_End( eew, tcb );
	}
}
void EncountEffect_SPTR_KakutouGym(TCB_PTR tcb, void* work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	BOOL result;
	result = EncountEffect_SpTr_GymMain( eew, HEAPID_FIELD, &ENCOUNT_SPTR_GymDef[3] );
	if( result == TRUE ){
		ENC_End( eew, tcb );
	}
}
void EncountEffect_SPTR_GoosutoGym(TCB_PTR tcb, void* work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	BOOL result;
	result = EncountEffect_SpTr_GymMain( eew, HEAPID_FIELD, &ENCOUNT_SPTR_GymDef[4] );
	if( result == TRUE ){
		ENC_End( eew, tcb );
	}
}
void EncountEffect_SPTR_KooriGym(TCB_PTR tcb, void* work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	BOOL result;
	result = EncountEffect_SpTr_GymMain( eew, HEAPID_FIELD, &ENCOUNT_SPTR_GymDef[5] );
	if( result == TRUE ){
		ENC_End( eew, tcb );
	}
}
void EncountEffect_SPTR_HaganeGym(TCB_PTR tcb, void* work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	BOOL result;
	result = EncountEffect_SpTr_GymMain( eew, HEAPID_FIELD, &ENCOUNT_SPTR_GymDef[6] );
	if( result == TRUE ){
		ENC_End( eew, tcb );
	}
}
void EncountEffect_SPTR_DenkiGym(TCB_PTR tcb, void* work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	BOOL result;
	result = EncountEffect_SpTr_GymMain( eew, HEAPID_FIELD, &ENCOUNT_SPTR_GymDef[7] );
	if( result == TRUE ){
		ENC_End( eew, tcb );
	}
}



//----------------------------------------------------------------------------
/**
 *	@brief	性別取得
 */
//-----------------------------------------------------------------------------
static u32 EncountEffect_SPTR_ChanpSexCheck( FIELDSYS_WORK* p_fsys )
{	
	MYSTATUS* p_mystatus = SaveData_GetMyStatus( p_fsys->savedata );
	return MyStatus_GetMySex( p_mystatus );
}
//----------------------------------------------------------------------------
/**
 *	@brief	四天王　チャンピオン専用エンカウントエフェクト
 *
 *	@param	eew		エフェクトワーク
 *	@param	heapID	ヒープID
 *	@param	cp_def	デファイン定義
 *
 *	@retval	TRUE		終了
 *	@retval	FALSE		途中
 */
//-----------------------------------------------------------------------------
static BOOL EncountEffect_SpTr_ChanpMain( ENCOUNT_EFFECT_WORK *eew, u32 heapID, const ENCOUNT_SPTR_CHANP_PARAM* cp_def )
{
	ENCOUNT_SPTR_CHANP* task_w = eew->work;
	BOOL result, result2;
	VecFx32 matrix;
	VecFx32 scale;
	int i;
	fx32 dist;
	ENC_CLACT_RES_WORK* p_clact_res;
	int seq;

	switch( eew->seq ){
	case ENCOUNT_SPTR_CHANP_START:
		eew->work = sys_AllocMemory( heapID, sizeof(ENCOUNT_SPTR_CHANP) );	
		memset( eew->work, 0, sizeof(ENCOUNT_SPTR_CHANP) );
		task_w = eew->work;

		// セルアクターセット作成
		ENC_CLACT_Init( &task_w->clact_sys, ENCOUNT_SPTR_CHANP_CELLACT_NUM, ENCOUNT_SPTR_CHANP_CELL_RES_NUM );

		// グラフィック設定
		// 主人公男女チェック
		if( EncountEffect_SPTR_ChanpSexCheck( eew->fsw ) == 0 ){
			// 男
			ENC_CLACT_ResLoadEasy(
					eew->p_handle,
					&task_w->clact_sys,
					&task_w->clact_res[0],
					NARC_field_encounteffect_trpl_boyface_NCLR, 1,
					NARC_field_encounteffect_trpl_boyface_NCGR,
					NARC_field_encounteffect_trpl_boyface_NCER,
					NARC_field_encounteffect_trpl_boyface_NANR,
					ENCOUNT_TR_GRASS_LOW_CELL_CONT_ID
					);

			task_w->hero_type  = TRTYPE_BOY;
		}else{
			// 女
			ENC_CLACT_ResLoadEasy(
					eew->p_handle,
					&task_w->clact_sys,
					&task_w->clact_res[0],
					NARC_field_encounteffect_trpl_girlface_NCLR, 1,
					NARC_field_encounteffect_trpl_girlface_NCGR,
					NARC_field_encounteffect_trpl_girlface_NCER,
					NARC_field_encounteffect_trpl_girlface_NANR,
					ENCOUNT_TR_GRASS_LOW_CELL_CONT_ID
					);
			task_w->hero_type  = TRTYPE_GIRL;
		}
		// 敵読み込み
		ENC_CLACT_ResLoadEasy(
				eew->p_handle,
				&task_w->clact_sys,
				&task_w->clact_res[1],
				cp_def->enemy_ncl, 1,
				cp_def->enemy_ncl+1,
				cp_def->enemy_ncl+2,
				cp_def->enemy_ncl+3,
				ENCOUNT_TR_GRASS_LOW_CELL_CONT_ID + 1
				);

		// 背景読み込み
		ENC_CLACT_ResLoadEasy(
				eew->p_handle,
				&task_w->clact_sys,
				&task_w->clact_res[2],
				cp_def->waku_oam_cl, 0xC,
				NARC_field_encounteffect_cutin_chanpion00_NCGR,
				NARC_field_encounteffect_cutin_chanpion00_NCER,
				NARC_field_encounteffect_cutin_chanpion00_NANR,
				ENCOUNT_TR_GRASS_LOW_CELL_CONT_ID + 2
				);

		// VSグラフィック
		ENC_CLACT_ResLoadEasy(
				eew->p_handle,
				&task_w->clact_sys,
				&task_w->clact_res[3],
				NARC_field_encounteffect_cutin_gym_vs_NCLR, 1,
				NARC_field_encounteffect_cutin_gym_vs_NCGR,
				NARC_field_encounteffect_cutin_gym_vs_NCER,
				NARC_field_encounteffect_cutin_gym_vs_NANR,
				ENCOUNT_TR_GRASS_LOW_CELL_CONT_ID + 3
				);


		eew->seq ++;
		break;

	case ENCOUNT_SPTR_CHANP_START2:

		// アクター登録
		scale = ENC_MakeVec( 
				ENCOUNT_SPTR_CHANP_CELL_SCALE, 
				ENCOUNT_SPTR_CHANP_CELL_SCALE, 0 );
		for( i=0; i<ENCOUNT_SPTR_CHANP_CELL_NUM; i++ ){
			if( i < (ENCOUNT_SPTR_CHANP_CELL_NUM - 1) ){
				p_clact_res = &task_w->clact_res[i];
				seq = 0;
			}else{
				p_clact_res = &task_w->clact_res[i - 1];
				seq = 1;
			}

			task_w->clact[i] = ENC_CLACT_Add( 
					&task_w->clact_sys, p_clact_res,
					0, 0, 0, 0);
			CLACT_SetDrawFlag( task_w->clact[i], FALSE );
			CLACT_AnmChg( task_w->clact[i], seq );
			CLACT_BGPriorityChg( task_w->clact[i], 1 );
		}

		// 主人公グラフィック登録
		if( task_w->hero_type ){
			// 男
			ENC_CLACT_ResColorChange( task_w->clact[0], heapID, TRTYPE_BOY, ENCOUNT_SPTR_CHANP_COLOR_FADE_EVY, ENCOUNT_SPTR_CHANP_COLOR_FADE );
		}else{
			// 女
			ENC_CLACT_ResColorChange( task_w->clact[0], heapID, TRTYPE_GIRL, ENCOUNT_SPTR_CHANP_COLOR_FADE_EVY, ENCOUNT_SPTR_CHANP_COLOR_FADE );
		}
//		CLACT_SetAffineParam( task_w->clact[0], CLACT_AFFINE_DOUBLE );
//		CLACT_SetScale( task_w->clact[0], &scale );

		// 四天王グラフィック登録
		ENC_CLACT_ResColorChange( task_w->clact[1], heapID, cp_def->enemy_tr_type, ENCOUNT_SPTR_CHANP_COLOR_FADE_EVY, ENCOUNT_SPTR_CHANP_COLOR_FADE );
//		CLACT_SetAffineParam( task_w->clact[1], CLACT_AFFINE_DOUBLE );
//		CLACT_SetScale( task_w->clact[1], &scale );

		// BG面表示
		GF_Disp_GX_VisibleControl(GX_PLANEMASK_OBJ, VISIBLE_ON);

		// VSアニメ初期化
		EncountEffect_SpTr_VsAnmInit( &task_w->vsanm, &task_w->clact_sys, &task_w->clact_res[3], ENCOUNT_SPTR_CHANP_VSMARK_X, ENCOUNT_SPTR_CHANP_VSMARK_Y, heapID );


		// ３D面を２D面に書き込む
		ENC_GPrint_Init( eew->fsw );

		eew->seq ++;
		break;
		
	case ENCOUNT_SPTR_CHANP_FLASH_INI:

		EncountFlashTask(MASK_MAIN_DISPLAY, 16, 16,  &eew->wait, 1);
		eew->count = 0;
		eew->seq++;
		break;
		
	case ENCOUNT_SPTR_CHANP_FLASH_WAIT:
		eew->count ++;
		if( eew->count == 8 ){
			ENC_GPrint_SwitchOn();	// 3D表示と２D表示を切り替える
		}
		if( eew->wait ){
			eew->seq++;
		}
		break;

	case ENCOUNT_SPTR_CHANP_GPRINT_WAIT:
		if( ENC_GPrint_Check() == TRUE ){

			// パーティクルを読み込んでおく
			ENC_GPrint_PTC_Load( eew->p_handle, NARC_field_encounteffect_chanpion_spa );

			// 3D面アルファ設定
			G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG0, 
					GX_BLEND_PLANEMASK_BG1|GX_BLEND_PLANEMASK_BG2|GX_BLEND_PLANEMASK_BG3|GX_BLEND_PLANEMASK_OBJ,
					0, 8 );
			eew->seq++;
		}
		break;

	case ENCOUNT_SPTR_CHANP_HERO_IN:
		ENC_AddMoveReqFx( &task_w->oam_move_x,
				ENCOUNT_SPTR_CHANP_HERO_MOVE_SX,
				ENCOUNT_SPTR_CHANP_HERO_MOVE_EX,
				ENCOUNT_SPTR_CHANP_HERO_MOVE_SS,
				ENCOUNT_SPTR_CHANP_HERO_MOVE_SYNC );
		matrix = ENC_MakeVec( 
				task_w->oam_move_x.x, 
				ENCOUNT_SPTR_CHANP_HERO_Y, 0 );
		CLACT_SetMatrix( task_w->clact[0], &matrix );
		matrix.y += ENCOUNT_SPTR_CHANP_HERO_Y_WAKU_OFS;
		matrix.x += ENCOUNT_SPTR_CHANP_HERO_X_WAKU_OFS;
		CLACT_SetMatrix( task_w->clact[2], &matrix );
		CLACT_SetDrawFlag( task_w->clact[0], TRUE );
		CLACT_SetDrawFlag( task_w->clact[2], TRUE );


		// 敵
		ENC_AddMoveReqFx( &task_w->enemy_oam_move_x,
				ENCOUNT_SPTR_CHANP_ENEMY_MOVE_SX,
				ENCOUNT_SPTR_CHANP_ENEMY_MOVE_EX,
				ENCOUNT_SPTR_CHANP_ENEMY_MOVE_SS,
				ENCOUNT_SPTR_CHANP_ENEMY_MOVE_SYNC );
		matrix = ENC_MakeVec( 
				task_w->enemy_oam_move_x.x, 
				ENCOUNT_SPTR_CHANP_ENEMY_Y, 0 );
		CLACT_SetMatrix( task_w->clact[1], &matrix );
		matrix.y += ENCOUNT_SPTR_CHANP_ENEMY_Y_WAKU_OFS;
		matrix.x += ENCOUNT_SPTR_CHANP_ENEMY_X_WAKU_OFS;
		CLACT_SetMatrix( task_w->clact[3], &matrix );
		CLACT_SetDrawFlag( task_w->clact[1], TRUE );
		CLACT_SetDrawFlag( task_w->clact[3], TRUE );


		// トレーナー名表示用ビットマップ作成
		{
			STRBUF* p_str;

			// パレット読み込み	
			ArcUtil_HDL_PalSet( eew->p_handle, 
					NARC_field_encounteffect_cutin_gym_font_NCLR,
					PALTYPE_MAIN_BG, ENCOUNT_SPTR_CHANP_TRNAME_PAL*0x20, 0x20, heapID );

			GF_Disp_GX_VisibleControl(GX_PLANEMASK_BG2, VISIBLE_OFF);
			GF_BGL_BmpWinAdd( eew->fsw->bgl, &task_w->trnamewin, 
					GF_BGL_FRAME2_M, ENCOUNT_SPTR_CHANP_TRNAME_X, ENCOUNT_SPTR_CHANP_TRNAME_Y,
					ENCOUNT_SPTR_CHANP_TRNAME_SIZX, ENCOUNT_SPTR_CHANP_TRNAME_SIZY,
					ENCOUNT_SPTR_CHANP_TRNAME_PAL, ENCOUNT_SPTR_CHANP_TRNAME_CGX );
			GF_BGL_BmpWinDataFill( &task_w->trnamewin, 0 );
			p_str = EncountEffect_SpTr_Gym_TrTypeNameGet( cp_def->tr_no, heapID );
			GF_STR_PrintColor( &task_w->trnamewin, FONT_SYSTEM,
										   p_str, 0, 0, MSG_ALLPUT, 
										   ENCOUNT_SPTR_CHANP_TRNAME_COL, NULL);
			STRBUF_Delete( p_str );	// 破棄わすれ注意！
		}

		eew->wait = ENCOUNT_SPTR_CHANP_HERO_EFFECT_WAIT;
		eew->seq ++;
		break;
		
	case ENCOUNT_SPTR_CHANP_HERO_INWAIT:

		// パーティクル開始タイミングを計算
		if( eew->wait > 0 ){
			eew->wait --;
			if( eew->wait == 0 ){
				// パーティクル開始
				ENC_GPrint_PTC_Start( 3 );
				GF_Disp_GX_VisibleControl(
					GX_PLANEMASK_BG0, VISIBLE_ON );

				// trainer名ON
				GF_Disp_GX_VisibleControl(GX_PLANEMASK_BG2, VISIBLE_ON);
			}
		}else{

			// その他ならVSアニメを流す
			EncountEffect_SpTr_VsAnmMain( &task_w->vsanm );
		}
		
		result = ENC_AddMoveMainFx( &task_w->oam_move_x );
		matrix = ENC_MakeVec( 
				task_w->oam_move_x.x, 
				ENCOUNT_SPTR_CHANP_HERO_Y, 0 );
		task_w->pos_hero = matrix;	// 基準座標
		CLACT_SetMatrix( task_w->clact[0], &matrix );
		matrix.y += ENCOUNT_SPTR_CHANP_HERO_Y_WAKU_OFS;
		matrix.x += ENCOUNT_SPTR_CHANP_HERO_X_WAKU_OFS;
		CLACT_SetMatrix( task_w->clact[2], &matrix );

		// 敵
		result = ENC_AddMoveMainFx( &task_w->enemy_oam_move_x );
		matrix = ENC_MakeVec( 
				task_w->enemy_oam_move_x.x, 
				ENCOUNT_SPTR_CHANP_ENEMY_Y, 0 );
		task_w->pos_enemy = matrix;	// 基準座標
		CLACT_SetMatrix( task_w->clact[1], &matrix );
		matrix.y += ENCOUNT_SPTR_CHANP_ENEMY_Y_WAKU_OFS;
		matrix.x += ENCOUNT_SPTR_CHANP_ENEMY_X_WAKU_OFS;
		CLACT_SetMatrix( task_w->clact[3], &matrix );

		if( result == TRUE ){
			eew->seq ++;
		}
		break;


	case ENCOUNT_SPTR_CHANP_HERO_FLASHOUT:
		result = EncountEffect_SpTr_VsAnmMain( &task_w->vsanm );
		result2 = ENC_GPrint_PTC_EndCheck();
		if( (result == FALSE) || (result2 == FALSE) ){
			break;
		}
		ENC_MoveReq( &task_w->flash,
				0, 16,
				ENCOUNT_SPTR_CHANP_HERO_FLASHOUT_SYNC);

		// このパーティクルを破棄
		ENC_GPrint_PTC_Delete();

		eew->seq ++;
		break;
		
		
	case ENCOUNT_SPTR_CHANP_HERO_FLASHOUTWAIT:
		result = ENC_MoveMain( &task_w->flash );
		ENC_SetMasterBrightnessVblank( &task_w->flash.x );
		if( result == TRUE ){
			// 主人公のカラーパレット変更
			// OAMのカラーパレット変更
			ENC_CLACT_ResColorChange( task_w->clact[0], heapID, task_w->hero_type, 0, ENCOUNT_SPTR_CHANP_COLOR_FADE );

			// 敵
			ENC_CLACT_ResColorChange( task_w->clact[1], heapID, cp_def->enemy_tr_type, 0,ENCOUNT_SPTR_CHANP_COLOR_FADE );

			// 土台のアニメ開始
			CLACT_SetAnmFlag( task_w->clact[2], TRUE );
			CLACT_SetAnmFrame( task_w->clact[2], FX32_ONE*2 );
			CLACT_SetAnmFlag( task_w->clact[3], TRUE );
			CLACT_SetAnmFrame( task_w->clact[3], FX32_ONE*2 );


			//  次のパーティクルを読み込む
			//  動きが止まるのがわかってもよいように真っ白のときに行う
			ENC_GPrint_PTC_Load( eew->p_handle, NARC_field_encounteffect_chanpion2_spa );

			eew->seq++;
		}
		break;

	case ENCOUNT_SPTR_CHANP_HERO_FLASHIN:
		ENC_MoveReq( &task_w->flash,
				16,0,
				ENCOUNT_SPTR_CHANP_HERO_FLASHIN_SYNC);

		// 背面パーティクル開始
		ENC_GPrint_PTC_Start( 4 );
		GF_BGL_PrioritySet( GF_BGL_FRAME0_M, 1 );	

		eew->seq++;
		break;
		
	case ENCOUNT_SPTR_CHANP_HERO_FLASHINWAIT:
		result = ENC_MoveMain( &task_w->flash );
		ENC_SetMasterBrightnessVblank( &task_w->flash.x );
		if( result == TRUE ){
			eew->seq++;

			// 主人公がこすれるまでのウエイト
			eew->wait = ENCOUNT_SPTR_CHANP_BT_IN_WAIT;

		}
		break;

	// こすれるアニメ
	case ENCOUNT_SPTR_CHANP_BT_IN:

		// ウエイト
		if( eew->wait > 0 ){
			eew->wait --;
			break;
		}


		// 動作する値
		ENC_AddMoveReqFx( &task_w->oam_move_x,
				ENCOUNT_SPTR_CHANP_BT_IN_MOVE_SX,
				ENCOUNT_SPTR_CHANP_BT_IN_MOVE_EX,
				ENCOUNT_SPTR_CHANP_BT_IN_MOVE_SSX,
				cp_def->bt_in_move_sync );
		ENC_AddMoveReqFx( &task_w->oam_move_y,
				ENCOUNT_SPTR_CHANP_BT_IN_MOVE_SY,
				ENCOUNT_SPTR_CHANP_BT_IN_MOVE_EY,
				ENCOUNT_SPTR_CHANP_BT_IN_MOVE_SSY,
				cp_def->bt_in_move_sync );

		eew->wait = 0;

		eew->seq++;
		break;
		
	case ENCOUNT_SPTR_CHANP_BT_INWAIT:

		eew->wait ++;
		
		result = ENC_AddMoveMainFx( &task_w->oam_move_x );
		ENC_AddMoveMainFx( &task_w->oam_move_y );
		
		// 主人公
		if( ((eew->wait / ENCOUNT_SPTR_CHANP_BT_IN_MOVE_CHG)%2) == 0 ){
			matrix = ENC_MakeVec( 
					task_w->pos_hero.x + task_w->oam_move_x.x, 
					task_w->pos_hero.y + task_w->oam_move_y.x, 0 );
		}else{
			matrix = ENC_MakeVec( 
					task_w->pos_hero.x - task_w->oam_move_x.x, 
					task_w->pos_hero.y - task_w->oam_move_y.x, 0 );
		}
		if( result ){
			task_w->pos_hero = matrix;	// 基準座標
		}
		CLACT_SetMatrix( task_w->clact[0], &matrix );
		matrix.y += ENCOUNT_SPTR_CHANP_HERO_Y_WAKU_OFS;
		matrix.x += ENCOUNT_SPTR_CHANP_HERO_X_WAKU_OFS;
		CLACT_SetMatrix( task_w->clact[2], &matrix );

		// 敵
		if( ((eew->wait / ENCOUNT_SPTR_CHANP_BT_IN_MOVE_CHG)%2) == 0 ){
			matrix = ENC_MakeVec( 
					task_w->pos_enemy.x - task_w->oam_move_x.x, 
					task_w->pos_enemy.y - task_w->oam_move_y.x, 0 );
		}else{
			matrix = ENC_MakeVec( 
					task_w->pos_enemy.x + task_w->oam_move_x.x, 
					task_w->pos_enemy.y + task_w->oam_move_y.x, 0 );
		}
		if( result ){
			task_w->pos_enemy = matrix;	// 基準座標
		}
		CLACT_SetMatrix( task_w->clact[1], &matrix );
		matrix.y += ENCOUNT_SPTR_CHANP_ENEMY_Y_WAKU_OFS;
		matrix.x += ENCOUNT_SPTR_CHANP_ENEMY_X_WAKU_OFS;
		CLACT_SetMatrix( task_w->clact[3], &matrix );

		if( result ){
			eew->seq ++;

			// トレーナー名OFF
			GF_Disp_GX_VisibleControl(GX_PLANEMASK_BG2, VISIBLE_OFF);

			// OAM
			ENC_AddMoveReqFx( &task_w->oam_move_x,
					0, ENCOUNT_SPTR_CHANP_OMA_MOVE_X,
					ENCOUNT_SPTR_CHANP_OAM_MOVE_XSS,
					ENCOUNT_SPTR_CHANP_WIPE_OUT_SYNC );	
			ENC_AddMoveReqFx( &task_w->oam_move_y,
					0, ENCOUNT_SPTR_CHANP_OMA_MOVE_Y,
					ENCOUNT_SPTR_CHANP_OAM_MOVE_YSS,
					ENCOUNT_SPTR_CHANP_WIPE_OUT_SYNC );	

			// ホワイトアウト
			WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_WHITE, ENCOUNT_SPTR_CHANP_WIPE_OUT_SYNC_WIPE, 1, HEAPID_FIELD );
		}
		break;
		
	case ENCOUNT_SPTR_CHANP_WIPEWAIT:
		ENC_AddMoveMainFx( &task_w->oam_move_x );
		ENC_AddMoveMainFx( &task_w->oam_move_y );

		// 主人公側
		matrix = ENC_MakeVec( 
				task_w->pos_hero.x - task_w->oam_move_x.x, 
				task_w->pos_hero.y - task_w->oam_move_y.x, 0 );
		CLACT_SetMatrix( task_w->clact[0], &matrix );
		matrix.y += ENCOUNT_SPTR_CHANP_HERO_Y_WAKU_OFS;
		matrix.x += ENCOUNT_SPTR_CHANP_HERO_X_WAKU_OFS;
		CLACT_SetMatrix( task_w->clact[2], &matrix );

		// 敵公側
		matrix = ENC_MakeVec( 
				task_w->pos_enemy.x + task_w->oam_move_x.x, 
				task_w->pos_enemy.y + task_w->oam_move_y.x, 0 );
		CLACT_SetMatrix( task_w->clact[1], &matrix );
		matrix.y += ENCOUNT_SPTR_CHANP_ENEMY_Y_WAKU_OFS;
		matrix.x += ENCOUNT_SPTR_CHANP_ENEMY_X_WAKU_OFS;
		CLACT_SetMatrix( task_w->clact[3], &matrix );


		if( WIPE_SYS_EndCheck() ) {
			eew->seq ++;
		}
		break;
		
	case ENCOUNT_SPTR_CHANP_END:
		WIPE_SetBrightness( WIPE_DISP_SUB, WIPE_FADE_WHITE );
		
		if(eew->end != NULL){
			*(eew->end) = TRUE;		// タスク終了を報告
		}

		// 破棄
		EncountEffect_SpTr_VsAnmExit( &task_w->vsanm );

		// trainer名BMP
		GF_BGL_BmpWinDel( &task_w->trnamewin );
		
		// OAM破棄
		for( i=0; i<ENCOUNT_SPTR_CHANP_CELL_NUM; i++ ){
			CLACT_Delete( task_w->clact[i] );
		}
		for( i=0; i<ENCOUNT_SPTR_CHANP_CELL_RES_NUM; i++ ){
			ENC_CLACT_ResDeleteEasy( &task_w->clact_sys, &task_w->clact_res[i] );
		}
		ENC_CLACT_Delete( &task_w->clact_sys );

		ENC_GPrint_PTC_Delete();

		ENC_GPrint_Exit();

		G2_BlendNone();

		return TRUE;
	}

	if( eew->seq != ENCOUNT_SPTR_CHANP_END ){
		CLACT_Draw( task_w->clact_sys.cas );

		if( ENCOUNT_SPTR_CHANP_GPRINT_WAIT < eew->seq ){
			GF_G3X_Reset();
			ENC_GPrint_PTC_Move();
			ENC_GPrint_Draw();
			GF_G3_RequestSwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_Z);
		}
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	四天王　チャンピオン
 */
//-----------------------------------------------------------------------------
void EncountEffect_SPTR_Siten00(TCB_PTR tcb, void* work)
{
#if 0
	ENCOUNT_EFFECT_WORK *eew = work;
	BOOL result;
	static ENC_GPRINT_WK debug_wk;
	
	switch( eew->seq ){
	case 0:
		ENC_GPrint_Init( &debug_wk, eew->fsw );
		eew->seq ++;
		break;

	case 1:
		result = ENC_GPrint_Check( &debug_wk );
		if( result ){
			eew->seq ++;
		}
		break;
	
	case 2:
		{
			//  勝手に３D描画処理
			GF_G3X_Reset();

			GFC_CameraLookAtZ();

			/* ジオメトリ＆レンダリングエンジン関連メモリのスワップ */
			GF_G3_RequestSwapBuffers(GX_SORTMODE_AUTO, SwapBuffMode);

			ENC_End( eew, tcb );
		}
		break;
	}

#endif
	
#if 1
	ENCOUNT_EFFECT_WORK *eew = work;
	BOOL result;
	result = EncountEffect_SpTr_ChanpMain( eew, HEAPID_FIELD, &EncountSpTr_ChanpParam[0] );
	if( result == TRUE ){
		ENC_End( eew, tcb );
	}
#endif
}
void EncountEffect_SPTR_Siten01(TCB_PTR tcb, void* work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	BOOL result;
	result = EncountEffect_SpTr_ChanpMain( eew, HEAPID_FIELD, &EncountSpTr_ChanpParam[1] );
	if( result == TRUE ){
		ENC_End( eew, tcb );
	}
}
void EncountEffect_SPTR_Siten02(TCB_PTR tcb, void* work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	BOOL result;
	result = EncountEffect_SpTr_ChanpMain( eew, HEAPID_FIELD, &EncountSpTr_ChanpParam[2] );
	if( result == TRUE ){
		ENC_End( eew, tcb );
	}
}
void EncountEffect_SPTR_Siten03(TCB_PTR tcb, void* work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	BOOL result;
	result = EncountEffect_SpTr_ChanpMain( eew, HEAPID_FIELD, &EncountSpTr_ChanpParam[3] );
	if( result == TRUE ){
		ENC_End( eew, tcb );
	}
}
void EncountEffect_SPTR_Chanpion(TCB_PTR tcb, void* work)
{
	ENCOUNT_EFFECT_WORK *eew = work;
	BOOL result;
	result = EncountEffect_SpTr_ChanpMain( eew, HEAPID_FIELD, &EncountSpTr_ChanpParam[4] );
	if( result == TRUE ){
		ENC_End( eew, tcb );
	}
}

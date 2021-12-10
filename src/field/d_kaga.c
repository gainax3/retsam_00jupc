//******************************************************************************
/**
 *
 * @file	d_kaga.c
 * @brief	フィールドデバッグメニュー　かがや
 * @author	kagaya
 * @data	05.07.13
 *
 */
//******************************************************************************
//----
#ifdef PM_DEBUG
//----
#include "common.h"
#include "fieldsys.h"
#include "fieldmap.h"
#include "field_effect.h"
#include "fieldobj.h"
#include "player.h"

#include "system/bmp_list.h"
#include "system/fontproc.h"
#include "system/pm_str.h"

#include "msgdata/msg.naix"
#include "msgdata/msg_debug_kaga.h"

#include "field_cutin.h"
#include "field_fishing.h"
#include "field_ananuke.h"

#include "sysflag.h"

#include "system/wipe.h"

#include "field_subproc.h"
#include "ev_mapchange.h"

#include "gym_init.h"
#include "gym.h"
#include "include/savedata/gimmickwork.h"
#include "field_gimmick_def.h"

#include "field_tornworld.h"

//==============================================================================
//	define
//==============================================================================
#define HEAPID_D_KAGA (HEAPID_FIELD)

enum	//デバッグメニューリスト
{
	MENU_GHOST_GYM,
	MENU_CAMERA,
	MENU_YABURE,
	MENU_KAGUALL,
	MENU_NAMINORI,
	MENU_KAIRIKIFLAG,
	MENU_TW_GIRAEV,
	MENU_TW_AKAGIEV,
	
	#ifdef DEBUG_TWORLD_CAPTURE
	MENU_TW_STEP_VANISH,
	MENU_TW_CLOUD_STOP,
	MENU_TW_BG_VANISH,
	#endif
	
	MENU_LIST_MAX,
};

#define MENU_Y_CHAR_ONE	(2)			//１文字 2
#define MENU_X_CHAR_SIZE (14)		//デバッグメニュー横幅　キャラサイズ
#define MENU_Y_CHAR_SIZE_MAX (11*MENU_Y_CHAR_ONE)//デバッグメニュー縦幅最大
#define MENU_Y_CHAR_SIZE_ALL (MENU_LIST_MAX*MENU_Y_CHAR_ONE)

#define RADIDX8(a) (((a)*256)&0xffff)
#define SINIDX8(a) FX_FX16_TO_F32( (FX_SinIdx(RADIDX8(a))) )
#define COSIDX8(a) FX_FX16_TO_F32( (FX_CosIdx(RADIDX8(a))) )
		
//==============================================================================
//	typedef struct
//==============================================================================
//--------------------------------------------------------------
//	メニュー呼び出し関数
//--------------------------------------------------------------
typedef void (*MENU_PROC)(void*);
typedef BOOL (*MENU_FUNC)(void*);

//--------------------------------------------------------------
//	メッセージリスト
//--------------------------------------------------------------
typedef struct{
	u32  str_id;
	u32  param;
}DEBUG_MENU_LIST;

//--------------------------------------------------------------
//	デバッグワーク
//--------------------------------------------------------------
typedef struct
{
	int seq_no;
	FIELDSYS_WORK *fsys;
	
	MENU_PROC select_func;
	GF_BGL_BMPWIN bmpwin;
	BMPLIST_WORK *bmplist;
	BMPLIST_DATA *menulist;
	MSGDATA_MANAGER* msgman;						//メッセージマネージャー
	WORDSET* wordset;								//単語セット
}D_KAGA_WORK;

//==============================================================================
//	プロトタイプ
//==============================================================================
void DebugKagayaMenuInit( FIELDSYS_WORK *fsys );

static void D_KagaMenuMain( TCB_PTR tcb, void *wk );
static void D_KagaMenuEnd( D_KAGA_WORK *work, TCB_PTR tcb );

static void * D_KagaAllocMemory( int size );
static void D_KagaPrintNum(
	GF_BGL_BMPWIN *win, int x, int y, int num, u32 keta );
static void D_KagaPrintStr(
	GF_BGL_BMPWIN *win, int x, int y, const STRBUF *msg );
static void D_KagaCalc360Fx( fx32 *rot, fx32 val );

static const DEBUG_MENU_LIST DATA_MenuList[MENU_LIST_MAX];
static const BMPLIST_HEADER DATA_MenuListHeader;

//==============================================================================
//	デバッグメニュー
//==============================================================================
//--------------------------------------------------------------
///	メニュー初期化
//--------------------------------------------------------------
void DebugKagayaMenuInit( FIELDSYS_WORK *fsys )
{
	BMPLIST_HEADER	list_h;
	GF_BGL_INI *bgl = FieldBglIniGet( fsys );
	D_KAGA_WORK *work = D_KagaAllocMemory( sizeof(D_KAGA_WORK) );
	
	work->fsys = fsys;
	
	{
		int y;
		y = MENU_Y_CHAR_SIZE_ALL;
		if( y >= MENU_Y_CHAR_SIZE_MAX ){ y = MENU_Y_CHAR_SIZE_MAX; }
		GF_BGL_BmpWinAdd( bgl, &work->bmpwin, FLD_MBGFRM_FONT,
				1, 1, MENU_X_CHAR_SIZE, y, 13, 1 );
	}
	
	work->menulist = BMP_MENULIST_Create(
			NELEMS(DATA_MenuList), HEAPID_D_KAGA );
	
	work->wordset = WORDSET_Create( HEAPID_D_KAGA );
	
	work->msgman = MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, 
							NARC_msg_debug_kaga_dat, HEAPID_D_KAGA );
	{
		int i;
		
		for( i = 0; i < MENU_LIST_MAX; i++ ){
			BMP_MENULIST_AddArchiveString( work->menulist, work->msgman, 
						DATA_MenuList[i].str_id, DATA_MenuList[i].param );
		}
	}
	
	list_h = DATA_MenuListHeader;
	list_h.list = work->menulist;
	list_h.win = &work->bmpwin;
	work->bmplist = BmpListSet( &list_h, 0, 0, HEAPID_D_KAGA );
	
	GF_BGL_BmpWinOn( &work->bmpwin );
	TCB_Add( D_KagaMenuMain, work, 0 );
}

//--------------------------------------------------------------
//	メイン
//--------------------------------------------------------------
static void D_KagaMenuMain( TCB_PTR tcb, void *wk )
{
	u32 ret;
	D_KAGA_WORK *work = wk;
	
	switch( work->seq_no ){
	case 0:
		ret = BmpListMain( work->bmplist );
		
		switch( ret ){
		case BMPLIST_NULL:
			break;
		case BMPLIST_CANCEL:
			D_KagaMenuEnd( work, tcb );
			FieldSystemProc_SeqHoldEnd();
			break;
		default:
			work->select_func = (MENU_PROC)ret;
			work->seq_no++;
		}
		
		break;
	case 1:
		work->select_func( work );
		D_KagaMenuEnd( work, tcb );
		FieldSystemProc_SeqHoldEnd();
		break;
	}
}

//--------------------------------------------------------------
//	メニュー終了
//--------------------------------------------------------------
static void D_KagaMenuEnd( D_KAGA_WORK *work, TCB_PTR tcb )
{
	MSGMAN_Delete( work->msgman );
	WORDSET_Delete( work->wordset );
	
	BMP_MENULIST_Delete( work->menulist );
	BmpListExit( work->bmplist, NULL, NULL );
	
	GF_BGL_BmpWinOff( &work->bmpwin );
	GF_BGL_BmpWinDel( &work->bmpwin );
	
	sys_FreeMemoryEz( work );
	TCB_Delete( tcb );
}

//==============================================================================
//	パーツ
//==============================================================================
//--------------------------------------------------------------
//	メモリ確
//--------------------------------------------------------------
static void * D_KagaAllocMemory( int size )
{
	void *mem = sys_AllocMemoryLo( HEAPID_D_KAGA, size );
	memset( mem, 0, size );
	return( mem );
}

//--------------------------------------------------------------
//	数値描画 x,y = dot 足りない桁は0で埋まる
//--------------------------------------------------------------
static void D_KagaPrintNum(
	GF_BGL_BMPWIN *win, int x, int y, int num, u32 keta )
{
	STRBUF *str = STRBUF_Create( 12, HEAPID_D_KAGA );
	STRBUF_SetNumber(
		str, num, keta, NUMBER_DISPTYPE_ZERO, NUMBER_CODETYPE_DEFAULT );
	GF_STR_PrintSimple( win, FONT_SYSTEM, str, x, y, 0, NULL );
	STRBUF_Delete( str );
}

//--------------------------------------------------------------
//	文字描画 x,y = dot
//--------------------------------------------------------------
static void D_KagaPrintStr(
	GF_BGL_BMPWIN *win, int x, int y, const STRBUF *msg )
{
	GF_STR_PrintSimple( win, FONT_TALK, msg, x, y, MSG_NO_PUT, NULL );
}

//--------------------------------------------------------------
//	360増減 fx
//--------------------------------------------------------------
static void D_KagaCalc360Fx( fx32 *rot, fx32 val )
{
	(*rot) += val;
	while( (*rot) < 0 ){ (*rot) += NUM_FX32(360); }
	(*rot) %= NUM_FX32( 360 );
}

//==============================================================================
//	ゴーストジム
//==============================================================================
//--------------------------------------------------------------
//	define
//--------------------------------------------------------------
#ifdef DEBUG_PLGHOSTGYM_CAPTURE
#define D_GHOSTGYM_MENULIST_MAX (6)
#else
#define D_GHOSTGYM_MENULIST_MAX (5)
#endif

#define D_GHOSTGYM_CHAR_SIZE_X (16)
#define D_GHOSTGYM_CHAR_SIZE_Y (D_GHOSTGYM_MENULIST_MAX*MENU_Y_CHAR_ONE)

//--------------------------------------------------------------
//	D_GHOSTGYM_WORK
//--------------------------------------------------------------
typedef struct
{
	int seq_no;
	int func_seq_no;
	FIELDSYS_WORK *fsys;
	int save_key_repeat_wait;
	int save_key_repeat_speed;
	
	MENU_FUNC select_func;
	GF_BGL_BMPWIN bmpwin;
	BMPLIST_WORK *bmplist;
	BMPLIST_DATA *menulist;
	MSGDATA_MANAGER* msgman;						//メッセージマネージャー
	WORDSET* wordset;								//単語セット
}D_GHOSTGYM_WORK;

//--------------------------------------------------------------
//	static
//--------------------------------------------------------------
static BOOL D_GhostGymEvent( GMEVENT_CONTROL *ev );
static void D_GhostGymFogPrintSet( D_GHOSTGYM_WORK *work );

static const DEBUG_MENU_LIST DATA_GhostGymMenuList[D_GHOSTGYM_MENULIST_MAX];
static const BMPLIST_HEADER DATA_GhostGymMenuListHeader;

//--------------------------------------------------------------
//	デバッグメニュー：ゴーストジム
//--------------------------------------------------------------
static void D_GhostGymInit( D_KAGA_WORK *kaga )
{
	{
		GIMMICKWORK *gmc;
		gmc = SaveData_GetGimmickWork( GameSystem_GetSaveData(kaga->fsys) );
		if( GIMMICKWORK_GetAssignID(gmc) != FLD_GIMMICK_GHOST_GYM){
			OS_Printf( "ゴーストジムではありません\n" );
			return;
		}
	}
	
	{
		D_GHOSTGYM_WORK *work;
		work = D_KagaAllocMemory( sizeof(D_GHOSTGYM_WORK) );
		work->fsys = kaga->fsys;
		FieldEvent_Set( work->fsys, D_GhostGymEvent, work );
	}
}

//--------------------------------------------------------------
//	ゴーストジム：イベント
//--------------------------------------------------------------
static BOOL D_GhostGymEvent( GMEVENT_CONTROL *ev )
{
	D_GHOSTGYM_WORK *work = FieldEvent_GetSpecialWork( ev );
	
	switch( work->seq_no ){
	case 0:
		{	
			int i;
			BMPLIST_HEADER list_h;
			GF_BGL_INI *bgl = FieldBglIniGet( work->fsys );
			
			GF_BGL_BmpWinAdd(
				bgl, &work->bmpwin, FLD_MBGFRM_FONT,
				1, 1,
				D_GHOSTGYM_CHAR_SIZE_X, D_GHOSTGYM_CHAR_SIZE_X, 13, 1 );
		
			work->menulist = BMP_MENULIST_Create(
					NELEMS(DATA_GhostGymMenuList), HEAPID_D_KAGA );
			work->wordset = WORDSET_Create( HEAPID_D_KAGA );
			work->msgman = MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, 
							NARC_msg_debug_kaga_dat, HEAPID_D_KAGA );
		
			for( i = 0; i < D_GHOSTGYM_MENULIST_MAX; i++ ){
				BMP_MENULIST_AddArchiveString(
					work->menulist, work->msgman, 
					DATA_GhostGymMenuList[i].str_id,
					DATA_GhostGymMenuList[i].param );
			}
			
			list_h = DATA_GhostGymMenuListHeader;
			list_h.list = work->menulist;
			list_h.win = &work->bmpwin;
			work->bmplist = BmpListSet( &list_h, 0, 0, HEAPID_D_KAGA );
			
			GF_BGL_BmpWinOn( &work->bmpwin );
		}
		
		work->save_key_repeat_wait = sys.repeatWait;
		work->save_key_repeat_speed = sys.repeatSpeed;
		work->seq_no++;
		break;
	case 1:
		{
			u32 ret = BmpListMain( work->bmplist );
			
			switch( ret ){
			case BMPLIST_NULL:
				break;
			case BMPLIST_CANCEL:
				work->seq_no = 3;
				break;
			default:
				work->select_func = (MENU_FUNC)ret;
				work->seq_no = 2;
			}
			
			D_GhostGymFogPrintSet( work );
		}
		break;
	case 2:
		if( work->select_func(work) == TRUE ){
			work->seq_no = 1;
		}
		
		D_GhostGymFogPrintSet( work );
		break;
	case 3:
		MSGMAN_Delete( work->msgman );
		WORDSET_Delete( work->wordset );
		BMP_MENULIST_Delete( work->menulist );
		BmpListExit( work->bmplist, NULL, NULL );
		GF_BGL_BmpWinOff( &work->bmpwin );
		GF_BGL_BmpWinDel( &work->bmpwin );
		
		sys_FreeMemoryEz( work );
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * ゴーストジム　フォグ情報描画と反映
 * @param	work	D_GHOSTGYM_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void D_GhostGymFogPrintSet( D_GHOSTGYM_WORK *work )
{
	u8 *p;
	int a;
	
	p = DEBUG_GYM_PLGhostGym_FogRedGet( work->fsys );
	a = *p;
	D_KagaPrintNum( &work->bmpwin, 8*13, 8*0, a, 2 );
	p = DEBUG_GYM_PLGhostGym_FogGreenGet( work->fsys );
	a = *p;
	D_KagaPrintNum( &work->bmpwin, 8*13, 8*2, a, 2 );
	p = DEBUG_GYM_PLGhostGym_FogBlueGet( work->fsys );
	a = *p;
	D_KagaPrintNum( &work->bmpwin, 8*13, 8*4, a, 2 );
	p = DEBUG_GYM_PLGhostGym_FogAlphaGet( work->fsys );
	a = *p;
	D_KagaPrintNum( &work->bmpwin, 8*13, 8*6, a, 2 );
	a = DEBUG_GYM_PLGhostGym_FogTblGet( work->fsys );
	D_KagaPrintNum( &work->bmpwin, 8*13, 8*8, a, 3 );
	DEBUG_GYM_PLGhostGym_FogSet( work->fsys );
}

//--------------------------------------------------------------
/**
 * ゴーストジム　フォグ　設定	上下キーで更新
 * @param	work	D_GHOSTGYM_WORK
 * @param	flag	0=R,1=G,2=B,3=Alpha,4=tbl
 * @retval	BOOL	TRUE=終了
 */
//--------------------------------------------------------------
static BOOL D_GhostGymMenuFunc_FogParamSet( D_GHOSTGYM_WORK *work, int flag )
{
	switch( work->func_seq_no ){
	case 0:
		sys_KeyRepeatSpeedSet( 2, 4 );
		work->func_seq_no++;
		break;
	case 1:
		if( sys.trg & PAD_BUTTON_B ){
			work->func_seq_no++;
		}else{
			if( flag < 4 ){
				u8 *p;
				switch( flag ){
				case 0:	p = DEBUG_GYM_PLGhostGym_FogRedGet( work->fsys );
					break;
				case 1: p = DEBUG_GYM_PLGhostGym_FogGreenGet( work->fsys );
					break;
				case 2: p = DEBUG_GYM_PLGhostGym_FogBlueGet( work->fsys );
					break;
				case 3: p = DEBUG_GYM_PLGhostGym_FogAlphaGet( work->fsys );
					break;
				}
				
				if( sys.repeat & PAD_KEY_UP ){
					(*p)--;
					if( (*p) >= 32 ){ (*p) = 31; }
				}else if( sys.repeat & PAD_KEY_DOWN ){
					(*p)++;
					if( (*p) >= 32 ){ (*p) = 0; }
				}
			}else{
				char p = DEBUG_GYM_PLGhostGym_FogTblGet( work->fsys );
				
				if( sys.repeat & PAD_KEY_UP ){
					(p)--;
					if( (p) < 0 ){ (p) = 127; }
				}else if( sys.repeat & PAD_KEY_DOWN ){
					(p)++;
					if( (p) <= -1 ){ (p) = 0; }
				}
				
				DEBUG_GYM_PLGhostGym_FogTblSet( work->fsys, p );
			}
		}
		break;
	case 2:
		sys_KeyRepeatSpeedSet(
			work->save_key_repeat_speed, work->save_key_repeat_wait );
		work->func_seq_no = 0;
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * ゴーストジム　フォグ　赤色設定	上下キーで更新
 * @param	work	D_GHOSTGYM_WORK
 * @retval	BOOL	TRUE=終了
 */
//--------------------------------------------------------------
static BOOL D_GhostGymMenuFunc_FogRedSet( D_GHOSTGYM_WORK *work )
{
	if( D_GhostGymMenuFunc_FogParamSet(work,0) == TRUE ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * ゴーストジム　フォグ　緑設定	上下キーで更新
 * @param	work	D_GHOSTGYM_WORK
 * @retval	BOOL	TRUE=終了
 */
//--------------------------------------------------------------
static BOOL D_GhostGymMenuFunc_FogGreenSet( D_GHOSTGYM_WORK *work )
{
	if( D_GhostGymMenuFunc_FogParamSet(work,1) == TRUE ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * ゴーストジム　フォグ　青色設定	上下キーで更新
 * @param	work	D_GHOSTGYM_WORK
 * @retval	BOOL	TRUE=終了
 */
//--------------------------------------------------------------
static BOOL D_GhostGymMenuFunc_FogBlueSet( D_GHOSTGYM_WORK *work )
{
	if( D_GhostGymMenuFunc_FogParamSet(work,2) == TRUE ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * ゴーストジム　フォグ　アルファ設定	上下キーで更新
 * @param	work	D_GHOSTGYM_WORK
 * @retval	BOOL	TRUE=終了
 */
//--------------------------------------------------------------
static BOOL D_GhostGymMenuFunc_FogAlphaSet( D_GHOSTGYM_WORK *work )
{
	if( D_GhostGymMenuFunc_FogParamSet(work,3) == TRUE ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * ゴーストジム　フォグ　テーブル設定	上下キーで更新
 * @param	work	D_GHOSTGYM_WORK
 * @retval	BOOL	TRUE=終了
 */
//--------------------------------------------------------------
static BOOL D_GhostGymMenuFunc_FogTblSet( D_GHOSTGYM_WORK *work )
{
	if( D_GhostGymMenuFunc_FogParamSet(work,4) == TRUE ){
		return( TRUE );
	}
	return( FALSE );
}

#ifdef DEBUG_PLGHOSTGYM_CAPTURE
//--------------------------------------------------------------
/**
 * ゴーストジム　ライト ON,OFF
 * @param	work	D_GHOSTGYM_WORK
 * @retval	BOOL	TRUE=終了
 */
//--------------------------------------------------------------
static BOOL D_GhostGymMenuFunc_LightVanish( D_GHOSTGYM_WORK *work )
{
	DEBUG_PlGhostGym_LightVanish( work->fsys );
	return( TRUE );
}
#endif

//--------------------------------------------------------------
//	ゴーストジム：data
//--------------------------------------------------------------
static const DEBUG_MENU_LIST DATA_GhostGymMenuList[D_GHOSTGYM_MENULIST_MAX] =
{
	{ debug_kaga_msg_01, (u32)D_GhostGymMenuFunc_FogRedSet},
	{ debug_kaga_msg_02, (u32)D_GhostGymMenuFunc_FogGreenSet},
	{ debug_kaga_msg_03, (u32)D_GhostGymMenuFunc_FogBlueSet},
	{ debug_kaga_msg_04, (u32)D_GhostGymMenuFunc_FogAlphaSet},
	{ debug_kaga_msg_05, (u32)D_GhostGymMenuFunc_FogTblSet},
	#ifdef DEBUG_PLGHOSTGYM_CAPTURE
	{ debug_kaga_msg_30, (u32)D_GhostGymMenuFunc_LightVanish},
	#endif
};

static const BMPLIST_HEADER DATA_GhostGymMenuListHeader =
{
	NULL,					// 表示文字データポインタ
	NULL,					// カーソル移動ごとのコールバック関数
	NULL,					// 一列表示ごとのコールバック関数
	NULL,
	D_GHOSTGYM_MENULIST_MAX,	// リスト項目数
	11,						// 表示最大項目数
	0,						// ラベル表示Ｘ座標
	12,						// 項目表示Ｘ座標
	0,						// カーソル表示Ｘ座標
	0,						// 表示Ｙ座標
	FBMP_COL_WHITE,			//文字色
	FBMP_COL_BLACK,			//背景色
	FBMP_COL_BLK_SDW,		//文字影色
	0,						// 文字間隔Ｘ
	16,						// 文字間隔Ｙ
	BMPLIST_LRKEY_SKIP,		// ページスキップタイプ
	FONT_SYSTEM,	// 文字指定(本来は u8 だけど、そんなに作らないと思うので)
	0						// ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)
};

//==============================================================================
//	カメラ設定
//==============================================================================

//NNS_G3dGlbGetCameraMtx

//--------------------------------------------------------------
//	define
//--------------------------------------------------------------
#define D_CAMERA_MENULIST_MAX (8)
#define D_CAMERA_BMPWIN_CHAR_SIZE_X (12)
#define D_CAMERA_BMPWIN_CHAR_SIZE_Y (D_CAMERA_MENULIST_MAX*MENU_Y_CHAR_ONE)

#define D_CAMERA_MOVE 	(0x0040)
#define D_CAMERA_MOVE2	(D_CAMERA_MOVE*4)
#define D_TARGET_MOVE (0x0800)

//--------------------------------------------------------------
//	D_CAMERA_WORK
//--------------------------------------------------------------
typedef struct
{
	int seq_no;
	int func_seq_no;
	FIELDSYS_WORK *fsys;
	FIELD_CAMERA_TYPE save_camera_type;
	
	GF_CAMERA_PTR camera;
	VecFx32 d_target_pos;
	
	MENU_FUNC select_func;
	GF_BGL_BMPWIN bmpwin;
	BMPLIST_WORK *bmplist;
	BMPLIST_DATA *menulist;
	MSGDATA_MANAGER* msgman;						//メッセージマネージャー
	WORDSET* wordset;								//単語セット
}D_CAMERA_WORK;

//--------------------------------------------------------------
//	static
//--------------------------------------------------------------
static BOOL D_CameraEvent( GMEVENT_CONTROL *ev );

#ifdef DEBUG_TWORLD_CAPTURE
static BOOL D_CameraMenuFunc_KoutenTwCap( D_CAMERA_WORK *work );
#endif

static const DEBUG_MENU_LIST DATA_CameraMenuList[D_CAMERA_MENULIST_MAX];
static const BMPLIST_HEADER DATA_CameraMenuListHeader;

//--------------------------------------------------------------
// デバッグメニュー：カメラ設定
//--------------------------------------------------------------
static void D_CameraInit( D_KAGA_WORK *kaga )
{
	D_CAMERA_WORK *work;
	work = D_KagaAllocMemory( sizeof(D_CAMERA_WORK) );
	work->fsys = kaga->fsys;
	FieldEvent_Set( work->fsys, D_CameraEvent, work );
}

//--------------------------------------------------------------
//	カメラ設定：イベント
//--------------------------------------------------------------
static BOOL D_CameraEvent( GMEVENT_CONTROL *ev )
{
	D_CAMERA_WORK *work = FieldEvent_GetSpecialWork( ev );
	
	switch( work->seq_no ){
	case 0:
		{
			int i;
			BMPLIST_HEADER list_h;
			GF_BGL_INI *bgl = FieldBglIniGet( work->fsys );
			
			GF_BGL_BmpWinAdd(
				bgl, &work->bmpwin, FLD_MBGFRM_FONT,
				1, 1,
				D_CAMERA_BMPWIN_CHAR_SIZE_X, D_CAMERA_BMPWIN_CHAR_SIZE_Y,
				13, 1 );
		
			work->menulist = BMP_MENULIST_Create(
					NELEMS(DATA_CameraMenuList), HEAPID_D_KAGA );
			work->wordset = WORDSET_Create( HEAPID_D_KAGA );
			work->msgman = MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, 
							NARC_msg_debug_kaga_dat, HEAPID_D_KAGA );
		
			for( i = 0; i < D_CAMERA_MENULIST_MAX; i++ ){
				BMP_MENULIST_AddArchiveString(
					work->menulist, work->msgman, 
					DATA_CameraMenuList[i].str_id,
					DATA_CameraMenuList[i].param );
			}
			
			list_h = DATA_CameraMenuListHeader;
			list_h.list = work->menulist;
			list_h.win = &work->bmpwin;
			work->bmplist = BmpListSet( &list_h, 0, 0, HEAPID_D_KAGA );
			
			GF_BGL_BmpWinOn( &work->bmpwin );
			
			work->save_camera_type = work->fsys->camera_type;
			work->fsys->camera_type = FIELD_CAMERA_LOOKAT_Z;
		}
		
		#ifndef DEBUG_TWORLD_CAPTURE
		{
			work->camera = GFC_AllocCamera( HEAPID_D_KAGA );
			GFC_CopyCamera( work->fsys->camera_ptr, work->camera );
			GFC_AttachCamera( work->camera );
		}
		#else
		{
			work->camera = GFC_AllocCamera( HEAPID_D_KAGA );
			GFC_CopyCamera( work->fsys->camera_ptr, work->camera );
			if( GYM_GimmickCodeCheck(
				work->fsys,FLD_GIMMICK_TORNWORLD) == 0 ){
				GFC_AttachCamera( work->camera );
			}
		}
		#endif
		work->seq_no++;
		break;
	case 1:
		{
			u32 ret = BmpListMain( work->bmplist );
			
			switch( ret ){
			case BMPLIST_NULL:
				break;
			case BMPLIST_CANCEL:
				work->seq_no = 3;
				break;
			default:
				work->select_func = (MENU_FUNC)ret;
				work->seq_no = 2;
			}
		}
		break;
	case 2:
		if( work->select_func(work) == TRUE ){
			work->seq_no = 1;
		}
		break;
	case 3:
		MSGMAN_Delete( work->msgman );
		WORDSET_Delete( work->wordset );
		BMP_MENULIST_Delete( work->menulist );
		BmpListExit( work->bmplist, NULL, NULL );
		GF_BGL_BmpWinOff( &work->bmpwin );
		GF_BGL_BmpWinDel( &work->bmpwin );
		
		#ifndef DEBUG_TWORLD_CAPTURE
		GFC_CopyCamera( work->camera, work->fsys->camera_ptr );
		GFC_FreeCamera( work->camera );
		GFC_AttachCamera( work->fsys->camera_ptr );
		#else
		if( GYM_GimmickCodeCheck(work->fsys,FLD_GIMMICK_TORNWORLD) ){
			GFC_FreeCamera( work->camera );
		}else{
			GFC_CopyCamera( work->camera, work->fsys->camera_ptr );
			GFC_FreeCamera( work->camera );
			GFC_AttachCamera( work->fsys->camera_ptr );
		}
		#endif
		
		work->fsys->camera_type = work->save_camera_type;
		
		sys_FreeMemoryEz( work );
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
//	カメラ設定：カメラ公転
//--------------------------------------------------------------
static BOOL D_CameraMenuFunc_Kouten( D_CAMERA_WORK *work )
{
	int pflag = FALSE;
	u32 trg = sys.trg;
	u32 cont = sys.cont;
	CAMERA_ANGLE angle = {0,0,0,0};
	
	#ifdef DEBUG_TWORLD_CAPTURE
	if( GYM_GimmickCodeCheck(work->fsys,FLD_GIMMICK_TORNWORLD) == TRUE ){
		return( D_CameraMenuFunc_KoutenTwCap(work) );
	}
	#endif
	
	if( cont & PAD_BUTTON_Y ){
		if( cont & PAD_KEY_UP ){
			pflag = TRUE;
			angle.z = -D_CAMERA_MOVE2;
		}else if( cont & PAD_KEY_DOWN ){
			pflag = TRUE;
			angle.z = D_CAMERA_MOVE2;
		}
		GFC_AddCameraAngleRev( &angle, work->camera );
	}else{
		if( cont & PAD_KEY_UP ){
			pflag = TRUE;
			angle.x = D_CAMERA_MOVE2;
		}else if( cont & PAD_KEY_DOWN ){
			pflag = TRUE;
			angle.x = -D_CAMERA_MOVE2;
		}
		
		if( cont & PAD_KEY_LEFT ){
			pflag = TRUE;
			angle.y = -D_CAMERA_MOVE2;
		}else if( cont & PAD_KEY_RIGHT ){
			pflag = TRUE;
			angle.y = D_CAMERA_MOVE2;
		}
		
		GFC_AddCameraAngleRev( &angle, work->camera );
	}
	
#if 0	
	if( trg & PAD_BUTTON_X ){
		CAMERA_ANGLE c = GFC_GetCameraAngle( work->camera );
		OS_Printf( "カメラテスト カメラ角度　X=0x%x, Y=0x%x, Z=0x%x\n",
				c.x, c.y, c.z );
	}
#else
	if( pflag == TRUE || (trg&PAD_BUTTON_X)  ){
		CAMERA_ANGLE c = GFC_GetCameraAngle( work->camera );
		OS_Printf( "カメラテスト カメラ角度　X=0x%x, Y=0x%x, Z=0x%x\n",
				c.x, c.y, c.z );
	}
#endif
	
	if( trg & PAD_BUTTON_B ){
		return( TRUE );
	}
	
	return( FALSE );
}

//カメラ公転　やぶれたせかい
#ifdef DEBUG_TWORLD_CAPTURE
static BOOL D_CameraMenuFunc_KoutenTwCap( D_CAMERA_WORK *work )
{
	CAMERA_ANGLE *angle;
	u32 trg = sys.trg;
	u32 cont = sys.cont;
	
	angle = DEBUG_FldTornWorld_CameraAngleOffsetGet( work->fsys );
	
	if( cont & PAD_BUTTON_Y ){
		if( cont & PAD_KEY_UP ){
			angle->z += -D_CAMERA_MOVE2;
		}else if( cont & PAD_KEY_DOWN ){
			angle->z += D_CAMERA_MOVE2;
		}
	}else{
		if( cont & PAD_KEY_UP ){
			angle->x += D_CAMERA_MOVE2;
		}else if( cont & PAD_KEY_DOWN ){
			angle->x += -D_CAMERA_MOVE2;
		}
		
		if( cont & PAD_KEY_LEFT ){
			angle->y += -D_CAMERA_MOVE2;
		}else if( cont & PAD_KEY_RIGHT ){
			angle->y += D_CAMERA_MOVE2;
		}
	}
	
	if( trg & PAD_BUTTON_B ){
		return( TRUE );
	}
	
	return( FALSE );
}
#endif

//--------------------------------------------------------------
//	カメラ設定：カメラ自転
//--------------------------------------------------------------
static BOOL D_CameraMenuFunc_Jiten( D_CAMERA_WORK *work )
{
	u32 trg = sys.trg;
	u32 cont = sys.cont;
	CAMERA_ANGLE angle = {0,0,0,0};
	
	if( cont & PAD_BUTTON_Y ){
		if( cont & PAD_KEY_UP ){
			angle.z = -D_CAMERA_MOVE;
		}else if( cont & PAD_KEY_DOWN ){
			angle.z = D_CAMERA_MOVE;
		}
		GFC_AddCameraAngleRot( &angle, work->camera );
	}else{
		if( cont & PAD_KEY_UP ){
			angle.x = D_CAMERA_MOVE;
		}else if( cont & PAD_KEY_DOWN ){
			angle.x = -D_CAMERA_MOVE;
		}
		
		if( cont & PAD_KEY_LEFT ){
			angle.y = D_CAMERA_MOVE;
		}else if( cont & PAD_KEY_RIGHT ){
			angle.y = -D_CAMERA_MOVE;
		}
		
		GFC_AddCameraAngleRot( &angle, work->camera );
	}
	
	if( trg & PAD_BUTTON_X ){
		CAMERA_ANGLE c = GFC_GetCameraAngle( work->camera );
		OS_Printf( "カメラテスト カメラ角度　X=0x%x, Y=0x%x, Z=0x%x\n",
				c.x, c.y, c.z );
	}
	
	if( trg & PAD_BUTTON_B ){
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
//	カメラ設定：カメラズーム
//--------------------------------------------------------------
static BOOL D_CameraMenuFunc_Zoom( D_CAMERA_WORK *work )
{
	u32 trg = sys.trg;
	u32 cont = sys.cont;
	u16 persp = GFC_GetCameraPerspWay( work->camera );
	fx32 dist;
	
	if( cont & PAD_KEY_UP ){
		GFC_AddCameraPerspWay( 64, work->camera );
	}else if( cont & PAD_KEY_DOWN ){
		GFC_AddCameraPerspWay( -64, work->camera );
	}else if( cont & PAD_KEY_LEFT ){
		GFC_AddCameraPerspWay( 64, work->camera );
		persp = GFC_GetCameraPerspWay( work->camera );
		dist = FX_Div( FX_Mul(FX_CosIdx(persp),FX_F32_TO_FX32(96)),
				FX_SinIdx(persp) );
		GFC_SetCameraDistance( dist, work->camera );
	}else if( cont & PAD_KEY_RIGHT ){
		GFC_AddCameraPerspWay( -64, work->camera );
		persp = GFC_GetCameraPerspWay( work->camera );
		dist = FX_Div( FX_Mul(FX_CosIdx(persp),FX_F32_TO_FX32(96)),
				FX_SinIdx(persp) );
		GFC_SetCameraDistance( dist, work->camera );
	}
	
	if( trg & PAD_BUTTON_X ){
		persp = GFC_GetCameraPerspWay( work->camera );
		dist = GFC_GetCameraDistance( work->camera );
		OS_Printf( "カメラテスト パース 0x%x, 距離 0x%x\n", persp, dist );
	}
	
	if( trg & PAD_BUTTON_B ){
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
//	カメラ設定：カメラ距離
//--------------------------------------------------------------
static BOOL D_CameraMenuFunc_Distance( D_CAMERA_WORK *work )
{
	u32 trg = sys.trg;
	u32 cont = sys.cont;
	fx32 dist;
	
	if( cont & PAD_KEY_UP ){
		GFC_AddCameraDistance( -FX32_ONE, work->camera );
	}else if( cont & PAD_KEY_DOWN ){
		GFC_AddCameraDistance( FX32_ONE, work->camera );
	}
	
	if( trg & PAD_BUTTON_X ){
		dist = GFC_GetCameraDistance( work->camera );
		OS_Printf( "カメラテスト 距離 0x%x\n", dist );
	}
	
	if( trg & PAD_BUTTON_B ){
		return( TRUE );
	}
	
	return( FALSE );
}

#if 0
//--------------------------------------------------------------
//	カメラ設定：視点移動
//--------------------------------------------------------------
static BOOL D_CameraMenuFunc_ViewPointMove( D_CAMERA_WORK *work )
{
	u32 trg = sys.trg;
	u32 cont = sys.cont;
	VecFx32 *tget = &work->d_target_pos;
	
	if( work->func_seq_no == 0 ){
		Player_VecPosGet( work->fsys->player, tget );
		GFC_BindCameraTarget( tget, work->camera );
		work->func_seq_no++;
		return( FALSE );
	}
	
	if( cont & PAD_BUTTON_Y ){
		if( cont & PAD_KEY_UP ){
			tget->y += D_TARGET_MOVE;
		}else if( cont & PAD_KEY_DOWN ){
			tget->y += -D_TARGET_MOVE;
		}
	}else{
		if( cont & PAD_KEY_UP ){
			tget->z += D_TARGET_MOVE;
		}else if( cont & PAD_KEY_DOWN ){
			tget->z += -D_TARGET_MOVE;
		}
		
		if( cont & PAD_KEY_LEFT ){
			tget->x += -D_TARGET_MOVE;
		}else if( cont & PAD_KEY_RIGHT ){
			tget->x += D_TARGET_MOVE;
		}
	}
	
	if( trg & PAD_BUTTON_X ){
		OS_Printf( "カメラテスト 目標移動 X=%x,Y=%x,Z=%x\n",
				tget->x, tget->y, tget->z );
	}
	
	if( trg & PAD_BUTTON_B ){
		work->func_seq_no = 0;
		return( TRUE );
	}
	
	return( FALSE );
}
#endif

//--------------------------------------------------------------
//	カメラ設定：目標移動
//--------------------------------------------------------------
static BOOL D_CameraMenuFunc_TargetMove( D_CAMERA_WORK *work )
{
	u32 trg = sys.trg;
	u32 cont = sys.cont;
	VecFx32 *tget = &work->d_target_pos;
	
	if( work->func_seq_no == 0 ){
		Player_VecPosGet( work->fsys->player, tget );
		GFC_BindCameraTarget( tget, work->camera );
		work->func_seq_no++;
		return( FALSE );
	}
	
	if( cont & PAD_BUTTON_Y ){
		if( cont & PAD_KEY_UP ){
			tget->y += D_TARGET_MOVE;
		}else if( cont & PAD_KEY_DOWN ){
			tget->y += -D_TARGET_MOVE;
		}
	}else{
		if( cont & PAD_KEY_UP ){
			tget->z += D_TARGET_MOVE;
		}else if( cont & PAD_KEY_DOWN ){
			tget->z += -D_TARGET_MOVE;
		}
		
		if( cont & PAD_KEY_LEFT ){
			tget->x += -D_TARGET_MOVE;
		}else if( cont & PAD_KEY_RIGHT ){
			tget->x += D_TARGET_MOVE;
		}
	}
	
	if( trg & PAD_BUTTON_X ){
		OS_Printf( "カメラテスト 目標移動 X=%x,Y=%x,Z=%x\n",
				tget->x, tget->y, tget->z );
	}
	
	if( trg & PAD_BUTTON_B ){
		work->func_seq_no = 0;
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
//	カメラ設定：射影変更
//--------------------------------------------------------------
static BOOL D_CameraMenuFunc_Syaei( D_CAMERA_WORK *work )
{
	u32 view = GFC_GetCameraView( work->camera );
	view ^= 1;
	GFC_SetCameraView( view, work->camera );
	
	if( view ){	
		OS_Printf( "カメラテスト　正射影に変更\n" );
	}else{
		OS_Printf( "カメラテスト　透視射影に変更\n" );
	}
	
	return( TRUE );
}

//--------------------------------------------------------------
//	カメラ設定：ロングクリップ
//--------------------------------------------------------------
static BOOL D_CameraMenuFunc_LongClip( D_CAMERA_WORK *work )
{
	GFC_SetCameraClip(
		FX32_ONE * 100,FX32_ONE * 1700,
		work->fsys->camera_ptr );
	return( TRUE );
}

//--------------------------------------------------------------
//	カメラ設定：リセット
//--------------------------------------------------------------
static BOOL D_CameraMenuFunc_Reset( D_CAMERA_WORK *work )
{
	GF_CAMERA_PARAM cmp =
	{
		/*0x30aec1*/0x29aec1,
		{-0x29fe,0,0},
		GF_CAMERA_PERSPECTIV,
		0x05c1,
		0	//dummy
	};
	
	#ifdef DEBUG_TWORLD_CAPTURE
	if( GYM_GimmickCodeCheck(work->fsys,FLD_GIMMICK_TORNWORLD) == TRUE ){
		DEBUG_FldTornWorld_CameraReset( work->fsys );
		return( TRUE );
	}
	#endif
	
	GFC_ReSetCameraTarget(
		Player_VecPosPtrGet(work->fsys->player), work->camera );
	GFC_SetCameraDistance( cmp.Distance, work->camera );
	GFC_SetCameraAngleRev( &cmp.Angle, work->camera );
	GFC_SetCameraPerspWay( cmp.PerspWay, work->camera );
	GFC_SetCameraView( cmp.View, work->camera );
	GFC_BindCameraTarget(
		Player_VecPosPtrGet(work->fsys->player), work->camera );
	GFC_SetCameraClip(
		FX32_ONE * 150, FX32_ONE * 900, work->camera );
	return( TRUE );
}

//--------------------------------------------------------------
//	ゴーストジム：data
//--------------------------------------------------------------
static const DEBUG_MENU_LIST DATA_CameraMenuList[D_CAMERA_MENULIST_MAX] =
{
	{ debug_kaga_msg_07, (u32)D_CameraMenuFunc_Kouten },
	{ debug_kaga_msg_08, (u32)D_CameraMenuFunc_Jiten },
	{ debug_kaga_msg_09, (u32)D_CameraMenuFunc_Zoom },
	{ debug_kaga_msg_10, (u32)D_CameraMenuFunc_Distance },
	{ debug_kaga_msg_11, (u32)D_CameraMenuFunc_TargetMove },
	{ debug_kaga_msg_12, (u32)D_CameraMenuFunc_Syaei },
	{ debug_kaga_msg_21, (u32)D_CameraMenuFunc_LongClip },
	{ debug_kaga_msg_13, (u32)D_CameraMenuFunc_Reset },
};

static const BMPLIST_HEADER DATA_CameraMenuListHeader =
{
	NULL,					// 表示文字データポインタ
	NULL,					// カーソル移動ごとのコールバック関数
	NULL,					// 一列表示ごとのコールバック関数
	NULL,
	D_CAMERA_MENULIST_MAX,	// リスト項目数
	11,						// 表示最大項目数
	0,						// ラベル表示Ｘ座標
	12,						// 項目表示Ｘ座標
	0,						// カーソル表示Ｘ座標
	0,						// 表示Ｙ座標
	FBMP_COL_WHITE,			//文字色
	FBMP_COL_BLACK,			//背景色
	FBMP_COL_BLK_SDW,		//文字影色
	0,						// 文字間隔Ｘ
	16,						// 文字間隔Ｙ
	BMPLIST_LRKEY_SKIP,		// ページスキップタイプ
	FONT_SYSTEM,	// 文字指定(本来は u8 だけど、そんなに作らないと思うので)
	0						// ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)
};

//--------------------------------------------------------------
///	
//--------------------------------------------------------------

//==============================================================================
//	やぶれたせかい
//==============================================================================
//--------------------------------------------------------------
//	define
//--------------------------------------------------------------
#define D_TORNWORLD_MENULIST_MAX (6)
#define D_TORNWORLD_BMPWIN_CHAR_SIZE_X (13)
#define D_TORNWORLD_BMPWIN_CHAR_SIZE_Y (D_TORNWORLD_MENULIST_MAX*MENU_Y_CHAR_ONE)

#define D_TORNWORLD_JIKI_D_RY (292)

#define D_JIKI_ROT_VAL (4)

//--------------------------------------------------------------
//	D_TORNWORLD_WORK
//--------------------------------------------------------------
typedef struct
{
	int seq_no;
	int func_seq_no;
	FIELDSYS_WORK *fsys;
	int save_jiki_code;
	
	void *p_func_work;
	
	MENU_FUNC select_func;
	GF_BGL_BMPWIN bmpwin;
	BMPLIST_WORK *bmplist;
	BMPLIST_DATA *menulist;
	MSGDATA_MANAGER* msgman;						//メッセージマネージャー
	WORDSET* wordset;								//単語セット
}D_TORNWORLD_WORK;

//--------------------------------------------------------------
//	static
//--------------------------------------------------------------
static BOOL D_TornWorldEvent( GMEVENT_CONTROL *ev );

static BOOL D_TornWorldMenuFunc_JikiReset( D_TORNWORLD_WORK *work );

static const DEBUG_MENU_LIST DATA_TornWorldMenuList[D_TORNWORLD_MENULIST_MAX];
static const BMPLIST_HEADER DATA_TornWorldMenuListHeader;

//--------------------------------------------------------------
/**
 * デバッグメニュー：やぶれたせかい
 * @param	kaga	D_KAGA_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void D_TornWorldInit( D_KAGA_WORK *kaga )
{
	D_TORNWORLD_WORK *work;
	work = D_KagaAllocMemory( sizeof(D_TORNWORLD_WORK) );
	work->fsys = kaga->fsys;
	FieldEvent_Set( work->fsys, D_TornWorldEvent, work );
}

//--------------------------------------------------------------
//	やぶれたせかい：イベント
//--------------------------------------------------------------
static BOOL D_TornWorldEvent( GMEVENT_CONTROL *ev )
{
	D_TORNWORLD_WORK *work = FieldEvent_GetSpecialWork( ev );
	
	switch( work->seq_no ){
	case 0:
		{
			int i;
			BMPLIST_HEADER list_h;
			GF_BGL_INI *bgl = FieldBglIniGet( work->fsys );
			
			GF_BGL_BmpWinAdd(
				bgl, &work->bmpwin, FLD_MBGFRM_FONT,
				1, 1,
				D_TORNWORLD_BMPWIN_CHAR_SIZE_X,
				D_TORNWORLD_BMPWIN_CHAR_SIZE_Y,
				13, 1 );
			
			work->menulist = BMP_MENULIST_Create(
					NELEMS(DATA_TornWorldMenuList), HEAPID_D_KAGA );
			work->wordset = WORDSET_Create( HEAPID_D_KAGA );
			work->msgman = MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, 
							NARC_msg_debug_kaga_dat, HEAPID_D_KAGA );
			
			for( i = 0; i < D_TORNWORLD_MENULIST_MAX; i++ ){
				BMP_MENULIST_AddArchiveString(
					work->menulist, work->msgman, 
					DATA_TornWorldMenuList[i].str_id,
					DATA_TornWorldMenuList[i].param );
			}
			
			list_h = DATA_TornWorldMenuListHeader;
			list_h.list = work->menulist;
			list_h.win = &work->bmpwin;
			work->bmplist = BmpListSet( &list_h, 0, 0, HEAPID_D_KAGA );
			
			GF_BGL_BmpWinOn( &work->bmpwin );
		}
		
		{	//自機をやぶれたせかい専用に
			FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( work->fsys->player );
			work->save_jiki_code = 	FieldOBJ_OBJCodeGet( fldobj );
			FieldOBJ_DrawReset( fldobj, RTHERO );
			FieldOBJ_DrawBlActOff_RotateXSet( fldobj, D_TORNWORLD_JIKI_D_RY );
		}
		
		work->seq_no++;
		break;
	case 1:	//自機リセット完了まで待つ
		{
			FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( work->fsys->player );
			BLACT_WORK_PTR act = FieldOBJ_DrawBlAct00_BlActPtrGet( fldobj );
			if( act != NULL ){
				work->seq_no++;
			}
		}
		break;
	case 2:
		{
			u32 ret = BmpListMain( work->bmplist );
			
			switch( ret ){
			case BMPLIST_NULL:
				break;
			case BMPLIST_CANCEL:
				work->seq_no = 4;
				break;
			default:
				work->select_func = (MENU_FUNC)ret;
				work->seq_no = 3;
			}
		}
		break;
	case 3:
		if( work->select_func(work) == TRUE ){
			if( (u32)work->select_func ==
				(u32)D_TornWorldMenuFunc_JikiReset ){
				work->seq_no = 4;
			}else{
				work->seq_no = 2;
			}
		}
		break;
	case 4:
		MSGMAN_Delete( work->msgman );
		WORDSET_Delete( work->wordset );
		BMP_MENULIST_Delete( work->menulist );
		BmpListExit( work->bmplist, NULL, NULL );
		GF_BGL_BmpWinOff( &work->bmpwin );
		GF_BGL_BmpWinDel( &work->bmpwin );
		work->seq_no++;
		break;
	case 5:
		sys_FreeMemoryEz( work );
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
//	360増減
//--------------------------------------------------------------
static void d_tornw_rc( u16 *r, s16 c )
{
	s16 w = (s16)(*r) + c;
	while( w < 0 ){ w += 360; }
	*r = (u16)w % 360;
}

//--------------------------------------------------------------
//	やぶれたせかい：壁歩き 左
//--------------------------------------------------------------
static BOOL D_TornWorldMenuFunc_WallWalkLeft( D_TORNWORLD_WORK *work )
{
	PLAYER_STATE_PTR jiki = work->fsys->player;
	FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( jiki );
	BLACT_WORK_PTR act = FieldOBJ_DrawBlAct00_BlActPtrGet( fldobj );
	
	switch( work->func_seq_no ){
	case 0:
//		Player_RequestSet( jiki, HERO_REQBIT_GL );
//		Player_Request( jiki );
		work->func_seq_no++;
		break;
	case 1:
		if( FieldOBJ_AcmdSetCheck(fldobj) == TRUE ){
			FieldOBJ_AcmdSet( fldobj, AC_STAY_WALK_L_32F );
			work->func_seq_no++;
		}
		
		break;
	case 2:
		{
			VecFx32 offs;
			u16 y = FieldOBJ_DrawBlActOff_RotateYGet( fldobj );
			u16 oy = y;
			
			if( y != 260 ){
				d_tornw_rc( &y, -2 );
				if( oy >= 260 && y < 260 ){
					y = 260;
				}
			}
			
			FieldOBJ_DrawBlActOff_RotateYSet( fldobj, y );
			
			FieldOBJ_VecDrawOffsGet( fldobj, &offs );
			
			if( offs.x > NUM_FX32(-8) ){
				offs.x -= NUM_FX32( 1 );
			}
			
			FieldOBJ_VecDrawOffsSet( fldobj, &offs );
			
			if( y == 260 && offs.x <= NUM_FX32(-8) ){
				work->func_seq_no++;
			}
		}
		break;
	case 3:	
		if( FieldOBJ_AcmdEndCheck(fldobj) == TRUE ){
			work->func_seq_no = 0;
			return( TRUE );
		}
		break;
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
//	やぶれたせかい：壁歩き 右
//--------------------------------------------------------------
static BOOL D_TornWorldMenuFunc_WallWalkRight( D_TORNWORLD_WORK *work )
{
	PLAYER_STATE_PTR jiki = work->fsys->player;
	FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( jiki );
	BLACT_WORK_PTR act = FieldOBJ_DrawBlAct00_BlActPtrGet( fldobj );
	
	switch( work->func_seq_no ){
	case 0:
//		Player_RequestSet( jiki, HERO_REQBIT_GR );
//		Player_Request( jiki );
		work->func_seq_no++;
		break;
	case 1:
		if( FieldOBJ_AcmdSetCheck(fldobj) == TRUE ){
			FieldOBJ_AcmdSet( fldobj, AC_STAY_WALK_R_32F );
			work->func_seq_no++;
		}
		
		break;
	case 2:
		{
			VecFx32 offs;
			u16 y = FieldOBJ_DrawBlActOff_RotateYGet( fldobj );
			u16 oy = y;
			
			if( y != 90 ){
				d_tornw_rc( &y, 2 );
				if( oy <= 90 && y > 90 ){
					y = 90;
				}
			}
			
			FieldOBJ_DrawBlActOff_RotateYSet( fldobj, y );
			
			FieldOBJ_VecDrawOffsGet( fldobj, &offs );
			
			if( offs.x < NUM_FX32(8) ){
				offs.x += NUM_FX32( 1 );
			}
			
			FieldOBJ_VecDrawOffsSet( fldobj, &offs );
			
			if( y == 90 && offs.x >= NUM_FX32(8) ){
				work->func_seq_no++;
			}
		}
		break;
	case 3:	
		if( FieldOBJ_AcmdEndCheck(fldobj) == TRUE ){
			work->func_seq_no = 0;
			return( TRUE );
		}
		break;
	}
	
	return( FALSE );
}


//--------------------------------------------------------------
//	やぶれたせかい：螺旋移動
//--------------------------------------------------------------
#define D_SPIRAL_AC (AC_WALK_U_32F)
#define D_SPIRAL_FRAME	(32)
#define D_SPIRAL_GRID 	(6)
#define D_SPIRAL_GRID_W	(2)
#define D_SPIRAL_GRID_H	(2)
		
typedef struct
{
	int dir;
	int frame;
	int grid_max;
	int frame_max;
	fx32 height_max_fx;
	fx32 height_move_val;
	fx32 wide_rad_fx;
	fx32 wide_idx_fx;
	fx32 wide_move_val;
	u32 rotate_save;
	fx32 rotate_fx;
	fx32 rotate_fx_val;
	VecFx32 offs;
}D_TORNW_SPIRAL_WORK;

static BOOL D_TornWorldMenuFunc_SpiralWalkH( D_TORNWORLD_WORK *work )
{
	D_TORNW_SPIRAL_WORK *fwk;
	PLAYER_STATE_PTR jiki = work->fsys->player;
	FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( jiki );
	
	if( work->p_func_work == NULL ){
		work->p_func_work = D_KagaAllocMemory( sizeof(D_TORNW_SPIRAL_WORK) );
	}
	
	fwk = work->p_func_work;
	
	switch( work->func_seq_no ){
	case 0:		//螺旋移動フレームに合う様に円移動設定
		fwk->dir = FieldOBJ_DirDispGet( fldobj );
		fwk->grid_max = D_SPIRAL_GRID;
		fwk->frame_max = fwk->grid_max * D_SPIRAL_FRAME;
		
		fwk->height_max_fx = GRID_SIZE_FX32( D_SPIRAL_GRID_H );
		fwk->height_move_val = -fwk->height_max_fx / fwk->frame_max;
		
		fwk->wide_rad_fx = GRID_SIZE_FX32( D_SPIRAL_GRID_W ) / 2;
		fwk->wide_idx_fx = NUM_FX32(0);
		fwk->wide_move_val = NUM_FX32(-256) / fwk->frame_max;
		
		fwk->rotate_save = FieldOBJ_DrawBlActOff_RotateYGet( fldobj );
		fwk->rotate_fx = NUM_FX32( fwk->rotate_save );
		fwk->rotate_fx_val = NUM_FX32(360) / fwk->frame_max;
		work->func_seq_no++;
	case 1:
		if( FieldOBJ_AcmdSetCheck(fldobj) == TRUE ){
			int code = FieldOBJ_AcmdCodeDirChange( fwk->dir, D_SPIRAL_AC );
			FieldOBJ_AcmdSet( fldobj, code );
			work->func_seq_no++;
		}
		break;
	case 2:
		{	//円筒移動
			fwk->wide_idx_fx += fwk->wide_move_val;
			fwk->offs.z = SINIDX8(
				FX32_NUM(fwk->wide_idx_fx)) * fwk->wide_rad_fx;
			fwk->offs.y += fwk->height_move_val;
			FieldOBJ_VecDrawOffsOutSideSet( fldobj, &fwk->offs );
			
			fwk->frame++;
			
			if( fwk->frame == fwk->frame_max/2 ){
				fwk->height_move_val = -fwk->height_move_val;
			}
			
			D_KagaCalc360Fx( &fwk->rotate_fx, fwk->rotate_fx_val );
			FieldOBJ_DrawBlActOff_RotateYSet(
					fldobj, FX32_NUM(fwk->rotate_fx) );
		}
		
		if( FieldOBJ_AcmdEndCheck(fldobj) == TRUE ){
			fwk->grid_max--;
			if( fwk->grid_max == 0 ){
				work->func_seq_no++;
			}else{
				work->func_seq_no = 1;
			}
		}
		break;
	case 3:
		{
			VecFx32 zero = {0,0,0};
			FieldOBJ_VecDrawOffsOutSideSet( fldobj, &zero );
			FieldOBJ_DrawBlActOff_RotateYSet( fldobj, fwk->rotate_save );
		}
		sys_FreeMemoryEz( fwk );
		work->p_func_work = NULL;
		work->func_seq_no = 0;
		return( TRUE );
	}
	
	return( FALSE );
}

static BOOL D_TornWorldMenuFunc_SpiralWalkV( D_TORNWORLD_WORK *work )
{
	D_TORNW_SPIRAL_WORK *fwk;
	PLAYER_STATE_PTR jiki = work->fsys->player;
	FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( jiki );
	
	if( work->p_func_work == NULL ){
		work->p_func_work = D_KagaAllocMemory( sizeof(D_TORNW_SPIRAL_WORK) );
	}
	
	fwk = work->p_func_work;
	
	switch( work->func_seq_no ){
	case 0:		//螺旋移動フレームに合う様に円移動設定
		fwk->dir = FieldOBJ_DirDispGet( fldobj );
		fwk->grid_max = D_SPIRAL_GRID;
		fwk->frame_max = fwk->grid_max * D_SPIRAL_FRAME;
		
		fwk->height_max_fx = GRID_SIZE_FX32( D_SPIRAL_GRID_H );
		fwk->height_move_val = -fwk->height_max_fx / fwk->frame_max;
		
		fwk->wide_rad_fx = GRID_SIZE_FX32( D_SPIRAL_GRID_W ) / 2;
		fwk->wide_idx_fx = NUM_FX32(0);
		fwk->wide_move_val = NUM_FX32(256) / fwk->frame_max;
		
		fwk->rotate_save = FieldOBJ_DrawBlActOff_RotateYGet( fldobj );
		fwk->rotate_fx = NUM_FX32( fwk->rotate_save );
		fwk->rotate_fx_val = NUM_FX32(-360) / fwk->frame_max;
		work->func_seq_no++;
	case 1:
		if( FieldOBJ_AcmdSetCheck(fldobj) == TRUE ){
			int code = FieldOBJ_AcmdCodeDirChange( fwk->dir, D_SPIRAL_AC );
			FieldOBJ_AcmdSet( fldobj, code );
			work->func_seq_no++;
		}
		break;
	case 2:
		{	//円筒移動
			fwk->wide_idx_fx += fwk->wide_move_val;
			fwk->offs.x = SINIDX8(
				FX32_NUM(fwk->wide_idx_fx)) * fwk->wide_rad_fx;
			fwk->offs.y += fwk->height_move_val;
			FieldOBJ_VecDrawOffsOutSideSet( fldobj, &fwk->offs );
			
			fwk->frame++;
			
			if( fwk->frame == fwk->frame_max/2 ){
				fwk->height_move_val = -fwk->height_move_val;
			}
			
			D_KagaCalc360Fx( &fwk->rotate_fx, fwk->rotate_fx_val );
			FieldOBJ_DrawBlActOff_RotateYSet(
					fldobj, FX32_NUM(fwk->rotate_fx) );
		}
		
		if( FieldOBJ_AcmdEndCheck(fldobj) == TRUE ){
			fwk->grid_max--;
			if( fwk->grid_max == 0 ){
				work->func_seq_no++;
			}else{
				work->func_seq_no = 1;
			}
		}
		break;
	case 3:
		{
			VecFx32 zero = {0,0,0};
			FieldOBJ_VecDrawOffsOutSideSet( fldobj, &zero );
			FieldOBJ_DrawBlActOff_RotateYSet( fldobj, fwk->rotate_save );
		}
		sys_FreeMemoryEz( fwk );
		work->p_func_work = NULL;
		work->func_seq_no = 0;
		return( TRUE );
	}
	
	return( FALSE );
}

static BOOL D_TornWorldMenuFunc_SpiralWalk( D_TORNWORLD_WORK *work )
{
	PLAYER_STATE_PTR jiki = work->fsys->player;
	FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( jiki );
	int dir = FieldOBJ_DirDispGet( fldobj );
	
	switch( dir ){
	case DIR_UP:
		return( D_TornWorldMenuFunc_SpiralWalkV(work) );
	case DIR_LEFT:
		return( D_TornWorldMenuFunc_SpiralWalkH(work) );
	}
	
	return( TRUE );
}

//--------------------------------------------------------------
//	やぶれたせかい：裏返し
//--------------------------------------------------------------
static BOOL D_TornWorldMenuFunc_TurnWalk( D_TORNWORLD_WORK *work )
{
	return( TRUE );
}

//--------------------------------------------------------------
//	やぶれたせかい：自機回転
//--------------------------------------------------------------
static BOOL D_TornWorldMenuFunc_JikiRotate( D_TORNWORLD_WORK *work )
{
	u16 ro;
	u32 trg = sys.trg;
	u32 cont = sys.cont;
	FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( work->fsys->player );
	
	if( trg & PAD_BUTTON_B ){
		work->func_seq_no = 0;
		return( TRUE );
	}
		
	if( cont & PAD_BUTTON_Y ){
		ro = FieldOBJ_DrawBlActOff_RotateZGet( fldobj );
		
		if( cont & PAD_KEY_UP ){
			d_tornw_rc( &ro, -D_JIKI_ROT_VAL );
			FieldOBJ_DrawBlActOff_RotateZSet( fldobj, ro );
		}else if( cont & PAD_KEY_DOWN ){
			d_tornw_rc( &ro, D_JIKI_ROT_VAL );
			FieldOBJ_DrawBlActOff_RotateZSet( fldobj, ro );
		}
	}else{
		ro = FieldOBJ_DrawBlActOff_RotateYGet( fldobj );
		
		if( cont & PAD_KEY_UP ){
			d_tornw_rc( &ro, -D_JIKI_ROT_VAL );
			FieldOBJ_DrawBlActOff_RotateYSet( fldobj, ro );
		}else if( cont & PAD_KEY_DOWN ){
			d_tornw_rc( &ro, D_JIKI_ROT_VAL );
			FieldOBJ_DrawBlActOff_RotateYSet( fldobj, ro );
		}
		
		ro = FieldOBJ_DrawBlActOff_RotateXGet( fldobj );
		
		if( cont & PAD_KEY_LEFT ){
			d_tornw_rc( &ro, -D_JIKI_ROT_VAL );
			FieldOBJ_DrawBlActOff_RotateXSet( fldobj, ro );
		}else if( cont & PAD_KEY_RIGHT ){
			d_tornw_rc( &ro, D_JIKI_ROT_VAL );
			FieldOBJ_DrawBlActOff_RotateXSet( fldobj, ro );
		}
	}
		
	if( trg & PAD_BUTTON_X ){
		u16 rx = FieldOBJ_DrawBlActOff_RotateXGet( fldobj );
		u16 ry = FieldOBJ_DrawBlActOff_RotateYGet( fldobj );
		u16 rz = FieldOBJ_DrawBlActOff_RotateZGet( fldobj );
		OS_Printf( "やぶれ　自機回転 X=%d, Y=%d, Z=%d\n", rx, ry, rz );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
//	やぶれたせかい：自機リセット
//--------------------------------------------------------------
static BOOL D_TornWorldMenuFunc_JikiReset( D_TORNWORLD_WORK *work )
{
	VecFx32 offs = {0,0,0};
	PLAYER_STATE_PTR jiki = work->fsys->player;
	FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( jiki );
	FieldOBJ_VecDrawOffsSet( fldobj, &offs );
	Player_RequestSet( work->fsys->player, HERO_REQBIT_NORMAL );
	return( TRUE );
}

//--------------------------------------------------------------
//	やぶれたせかい：data
//--------------------------------------------------------------
static const DEBUG_MENU_LIST DATA_TornWorldMenuList[D_TORNWORLD_MENULIST_MAX] =
{
	{ debug_kaga_msg_15, (u32)D_TornWorldMenuFunc_WallWalkLeft },
	{ debug_kaga_msg_18, (u32)D_TornWorldMenuFunc_WallWalkRight },
	{ debug_kaga_msg_16, (u32)D_TornWorldMenuFunc_SpiralWalk },
	{ debug_kaga_msg_17, (u32)D_TornWorldMenuFunc_TurnWalk },
	{ debug_kaga_msg_19, (u32)D_TornWorldMenuFunc_JikiRotate },
	{ debug_kaga_msg_20, (u32)D_TornWorldMenuFunc_JikiReset },
};

static const BMPLIST_HEADER DATA_TornWorldMenuListHeader =
{
	NULL,					// 表示文字データポインタ
	NULL,					// カーソル移動ごとのコールバック関数
	NULL,					// 一列表示ごとのコールバック関数
	NULL,
	D_TORNWORLD_MENULIST_MAX,	// リスト項目数
	11,						// 表示最大項目数
	0,						// ラベル表示Ｘ座標
	12,						// 項目表示Ｘ座標
	0,						// カーソル表示Ｘ座標
	0,						// 表示Ｙ座標
	FBMP_COL_WHITE,			//文字色
	FBMP_COL_BLACK,			//背景色
	FBMP_COL_BLK_SDW,		//文字影色
	0,						// 文字間隔Ｘ
	16,						// 文字間隔Ｙ
	BMPLIST_LRKEY_SKIP,		// ページスキップタイプ
	FONT_SYSTEM,	// 文字指定(本来は u8 だけど、そんなに作らないと思うので)
	0						// ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)
};

//==============================================================================
//	別荘
//==============================================================================
//--------------------------------------------------------------
//	別荘：家具全て置く
//--------------------------------------------------------------
static void D_VillaKaguAllPut( D_KAGA_WORK *kaga )
{
	int i;
	EVENTWORK *ev = SaveData_GetEventWork( kaga->fsys->savedata );
	for( i = 0; i < VILLA_FTURE_MAX; i++ ){
		SysFlag_VillaFurniture( ev, SYSFLAG_MODE_SET, i );
	}
}

//==============================================================================
//	自機自由移動
//==============================================================================
#if 0
//--------------------------------------------------------------
///	自由移動　ワーク
//--------------------------------------------------------------
typedef struct
{
	int seq_no;
	FIELDSYS_WORK *fsys;
	PLAYER_STATE_PTR jiki;
}D_JIKI_FREE_MOVE_WORK;

#define D_JIKI_FREE_MOVE_WORK_SIZE (sizeof(D_JIKI_FREE_MOVE_WORK))

#define FREE_X_SPEED (FX32_ONE * 2)
#define FREE_Z_SPEED (FX32_ONE * 2)
#define FREE_Y_SPEED (FX32_ONE * 2)
#define FREE_SPEED_UP (4)

//--------------------------------------------------------------
//	自由移動イベント
//--------------------------------------------------------------
static BOOL D_KagaEvent_FreeMove( GMEVENT_CONTROL *ev )
{
	D_JIKI_FREE_MOVE_WORK *work = FieldEvent_GetSpecialWork( ev );
	FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( work->jiki );
	u16 trg = sys.trg;
	u16 prs = sys.cont;
	int speed = 1;
	VecFx32 vec;
	
	if( FieldOBJ_AcmdSetCheck(fldobj) == FALSE ){
		return( FALSE );
	}
	
	FieldOBJ_VecPosGet( fldobj, &vec );
	
	if( (trg & PAD_BUTTON_B) ){
		Player_HeightGet_ON_OFF( work->jiki, TRUE );
		D_KagaFreeMemory( work );
		OS_Printf( "デバッグ 自由移動終了\n" );
		return( TRUE );
	}
	
	if( (prs & PAD_BUTTON_R) ){
		speed = FREE_SPEED_UP;
	}
	
	if( (prs & PAD_BUTTON_Y) ){
		if( (prs & PAD_KEY_UP) ){
			vec.y -= FREE_Y_SPEED * speed;
			OS_Printf( "デバッグ自機高さ y = 0x%x, grid y = 0x%x\n",
					vec.y, SIZE_H_GRID_FX32(vec.y) );
		}else if( (prs & PAD_KEY_DOWN) ){
			vec.y += FREE_Y_SPEED * speed;
			OS_Printf( "デバッグ自機高さ y = 0x%x, grid y = 0x%x\n",
					vec.y, SIZE_H_GRID_FX32(vec.y) );
		}
		
		FieldOBJ_VecPosSet( fldobj, &vec );
	}else{
		int ac = ACMD_NOT;
		
		if( (prs & PAD_KEY_UP) ){
			if( speed == FREE_SPEED_UP ){ ac = AC_WALK_U_2F; }
			else{ ac = AC_WALK_U_8F; }
		}else if( (prs & PAD_KEY_DOWN) ){
			if( speed == FREE_SPEED_UP ){ ac = AC_WALK_D_2F; }
			else{ ac = AC_WALK_D_8F; }
		}else if( (prs & PAD_KEY_LEFT) ){
			if( speed == FREE_SPEED_UP ){ ac = AC_WALK_L_2F; }
			else{ ac = AC_WALK_L_8F; }
		}else if( (prs & PAD_KEY_RIGHT) ){
			if( speed == FREE_SPEED_UP ){ ac = AC_WALK_R_2F; }
			else{ ac = AC_WALK_R_8F; }
		}
		
		if( ac != ACMD_NOT ){
			FieldOBJ_AcmdSet( fldobj, ac );
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
//	自由移動
//--------------------------------------------------------------
static void D_KagaJikiFreeMoveInit( D_KAGA_WORK *kaga )
{
	D_JIKI_FREE_MOVE_WORK *work =
		D_KagaAllocMemory(sizeof(D_JIKI_FREE_MOVE_WORK) );
	
	work->fsys = kaga->fsys;
	work->jiki = kaga->fsys->player;
	
	Player_HeightGet_ON_OFF( work->jiki, FALSE );
	FieldEvent_Set( work->fsys, D_KagaEvent_FreeMove, work );
	OS_Printf( "デバッグ　自由移動開始\n" );
}
#endif


//==============================================================================
//	ひでんわざ　なみのり
//==============================================================================
//--------------------------------------------------------------
//	D_NAMI_WORK
//--------------------------------------------------------------
typedef struct
{
	int seq_no;
	FIELDSYS_WORK *fsys;
}D_NAMI_WORK;

static BOOL D_NaminoriEvent( GMEVENT_CONTROL *ev );

//--------------------------------------------------------------
//	秘伝技波乗り 
//--------------------------------------------------------------
static void D_NaminoriInit( D_KAGA_WORK *kaga )
{
	D_NAMI_WORK *work;
	
	work = D_KagaAllocMemory( sizeof(D_NAMI_WORK) );
	work->fsys = kaga->fsys;
	FieldEvent_Set( work->fsys, D_NaminoriEvent, work );
}

//--------------------------------------------------------------
//	波乗り：イベント
//--------------------------------------------------------------
static BOOL D_NaminoriEvent( GMEVENT_CONTROL *ev )
{
	D_NAMI_WORK *work = FieldEvent_GetSpecialWork( ev );
	
	switch( work->seq_no ){
	case 0:
		{
			int dir = Player_DirMoveGet( work->fsys->player );
			EventCmd_NaminoriCall( ev, dir, 0 );
			work->seq_no++;
		}
		break;
	case 1:
		sys_FreeMemoryEz( work );
		return( TRUE );
	}
	
	return( FALSE );
}

//==============================================================================
//	ひでんわざ　怪力フラグ
//==============================================================================
//--------------------------------------------------------------
//	怪力フラグセット
//--------------------------------------------------------------
static void D_KairikiFlagInit( D_KAGA_WORK *kaga )
{
	EVENTWORK *ev = SaveData_GetEventWork( kaga->fsys->savedata );
	SysFlag_Kairiki( ev, SYSFLAG_MODE_SET );
}

//==============================================================================
//	やぶれたせかい　ギラティナイベントへ
//==============================================================================
#include "mapdefine.h"	//zone_id
#include "field_tornworld.h"
#include "syswork.h"

//--------------------------------------------------------------
//	やぶれたせかい　ギラティナイベントへ
//--------------------------------------------------------------
static void D_TWorldGiratinaEventInit( D_KAGA_WORK *kaga )
{
	EVENTWORK *ev = SaveData_GetEventWork( kaga->fsys->savedata );
	SysWork_TornWorldEventSeqNoSet( ev, TWEVSEQ_08_02 );
	EventSet_EasyMapChange( kaga->fsys, ZONE_ID_D34R0110,
			DOOR_ID_JUMP_CODE, 15, 25, DIR_UP );
}

//--------------------------------------------------------------
//	やぶれたせかい　アカギ対決イベントへ
//--------------------------------------------------------------
static void D_TWorldAkagiFightEventInit( D_KAGA_WORK *kaga )
{
	EVENTWORK *ev = SaveData_GetEventWork( kaga->fsys->savedata );
	SysWork_TornWorldEventSeqNoSet( ev, TWEVSEQ_04_00 );
	SysFlag_TornWorldRockFinish( ev, SYSFLAG_MODE_SET );
	EventSet_EasyMapChange( kaga->fsys, ZONE_ID_D34R0108,
			DOOR_ID_JUMP_CODE, 85, 79, DIR_DOWN );
}

//==============================================================================
//	やぶれたせかい	攻略本関連
//==============================================================================
//----
#ifdef DEBUG_TWORLD_CAPTURE
//----
//--------------------------------------------------------------
//	やぶれたせかい　飛び石ON,OFF
//--------------------------------------------------------------
static void D_TWorldStepVanish( D_KAGA_WORK *kaga )
{
	GIMMICKWORK *gmc;
	gmc = SaveData_GetGimmickWork( GameSystem_GetSaveData(kaga->fsys) );
	if( GIMMICKWORK_GetAssignID(gmc) == FLD_GIMMICK_TORNWORLD){
		DEBUG_FldTornWorld_StepVanish();
	}
}

//--------------------------------------------------------------
//	やぶれたせかい　雲動作ON,OFF
//--------------------------------------------------------------
static void D_TWorldCloudStop( D_KAGA_WORK *kaga )
{
	GIMMICKWORK *gmc;
	gmc = SaveData_GetGimmickWork( GameSystem_GetSaveData(kaga->fsys) );
	if( GIMMICKWORK_GetAssignID(gmc) == FLD_GIMMICK_TORNWORLD){
		DEBUG_FldTornWorld_CloudStop();
	}
}

//--------------------------------------------------------------
//	やぶれたせかい　背景ON,OFF
//--------------------------------------------------------------
static void D_TWorldBGVanish( D_KAGA_WORK *kaga )
{
	GIMMICKWORK *gmc;
	gmc = SaveData_GetGimmickWork( GameSystem_GetSaveData(kaga->fsys) );
	if( GIMMICKWORK_GetAssignID(gmc) == FLD_GIMMICK_TORNWORLD){
		DEBUG_FldTornWorld_BGVanish( kaga->fsys );
	}
}
//----
#endif //DEBUG_TWORLD_CAPTURE
//----

//==============================================================================
//	data
//==============================================================================
static const DEBUG_MENU_LIST DATA_MenuList[MENU_LIST_MAX] =
{
	{ debug_kaga_msg_00, (u32)D_GhostGymInit },
	{ debug_kaga_msg_06, (u32)D_CameraInit },
	{ debug_kaga_msg_14, (u32)D_TornWorldInit },
	{ debug_kaga_msg_22, (u32)D_VillaKaguAllPut },
	{ debug_kaga_msg_23, (u32)D_NaminoriInit },
	{ debug_kaga_msg_24, (u32)D_KairikiFlagInit },
	{ debug_kaga_msg_25, (u32)D_TWorldGiratinaEventInit },
	{ debug_kaga_msg_26, (u32)D_TWorldAkagiFightEventInit },
	#ifdef DEBUG_TWORLD_CAPTURE
	{ debug_kaga_msg_27, (u32)D_TWorldStepVanish },
	{ debug_kaga_msg_28, (u32)D_TWorldCloudStop },
	{ debug_kaga_msg_29, (u32)D_TWorldBGVanish },
	#endif
};

static const BMPLIST_HEADER DATA_MenuListHeader =
{
	NULL,					// 表示文字データポインタ
	NULL,					// カーソル移動ごとのコールバック関数
	NULL,					// 一列表示ごとのコールバック関数
	NULL,
	NELEMS(DATA_MenuList),	// リスト項目数
	11,						// 表示最大項目数
	0,						// ラベル表示Ｘ座標
	12,						// 項目表示Ｘ座標
	0,						// カーソル表示Ｘ座標
	0,						// 表示Ｙ座標
	FBMP_COL_WHITE,			//文字色
	FBMP_COL_BLACK,			//背景色
	FBMP_COL_BLK_SDW,		//文字影色
	0,						// 文字間隔Ｘ
	16,						// 文字間隔Ｙ
	BMPLIST_LRKEY_SKIP,		// ページスキップタイプ
	FONT_SYSTEM,	// 文字指定(本来は u8 だけど、そんなに作らないと思うので)
	0						// ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)
};

/*
	NNS_GfdDumpFrmTexVramManager();
	NNS_GfdDumpLnkTexVramManager();
*/

//----
#endif	//PM_DEBUG
//----

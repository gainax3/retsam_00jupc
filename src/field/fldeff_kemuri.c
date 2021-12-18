//******************************************************************************
/**
 * 
 * @file	fldeff_kemuri.c
 * @brief	フィールドOBJ土煙
 * @author	kagaya
 * @data	05.07.13
 *
 */
//******************************************************************************
#include "common.h"
#include "fieldsys.h"
#include "field_effect.h"
#include "fieldobj.h"

#include "fldeff_kemuri.h"
#include "map_tool.h"

#include "field_tornworld.h"

//==============================================================================
//	define
//==============================================================================
#define KEMURI_ANIME_FRAME (9)				///<土煙アニメフレーム

#define KEMURI_DRAW_Z_OFFS (NUM_FX32(8))	///<表示座標Zオフセット

//==============================================================================
//	typedef struct
//==============================================================================
//--------------------------------------------------------------
///	FE_KEMURI_PTR型
//--------------------------------------------------------------
typedef struct _TAG_FE_KEMURI * FE_KEMURI_PTR;

//--------------------------------------------------------------
///	FE_KEMURI構造体
//--------------------------------------------------------------
typedef struct _TAG_FE_KEMURI
{
	u32 tw_flag;
	FE_SYS *fes;
}FE_KEMURI;

#define FE_KEMURI_SIZE (sizeof(FE_KEMURI))	///<FE_KEMURIサイズ

//--------------------------------------------------------------
///	KEMURI_ADD_H構造体
//--------------------------------------------------------------
typedef struct
{
	FIELDSYS_WORK *fsys;
	FE_SYS *fes;
	FE_KEMURI_PTR kemu;
	FIELD_OBJ_PTR fldobj;
}KEMURI_ADD_H;

//--------------------------------------------------------------
///	KEMURI_WORK構造体
//--------------------------------------------------------------
typedef struct
{
	int seq_no;
	int obj_id;
	int zone_id;
	int frame;
	KEMURI_ADD_H head;
	BLACT_WORK_PTR act;
}KEMURI_WORK;

#define KEMURI_WORK_SIZE (sizeof(KEMURI_WORK))

//==============================================================================
//	プロトタイプ
//==============================================================================
static void Kemuri_GraphicInit( FE_KEMURI_PTR kemu );
static void Kemuri_TwGraphicAdd( FE_KEMURI_PTR kemu );
static void Kemuri_GraphicDelete( FE_KEMURI_PTR kemu );

static const EOA_H_NPP DATA_EoaH_Kemuri;
const BLACT_ANIME_TBL DATA_BlActAnmTbl_Kemuri[];

//==============================================================================
//	土煙	システム
//==============================================================================
//--------------------------------------------------------------
/**
 * 土煙初期化
 * @param	fes		FE_SYS_PTR
 * @retval	FE_KEMURI_PTR FE_KEMURI_PTR
 */
//--------------------------------------------------------------
void * FE_Kemuri_Init( FE_SYS *fes )
{
	FE_KEMURI_PTR kemu;
	
	kemu = FE_AllocClearMemory( fes, FE_KEMURI_SIZE, ALLOC_FR, 0 );
	kemu->fes = fes;
	
	Kemuri_GraphicInit( kemu );
	return( kemu );
}

//--------------------------------------------------------------
/**
 * 土煙削除
 * @param	kusa		FE_KEMURI_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FE_Kemuri_Delete( void *work )
{
	FE_KEMURI_PTR kemu = work;
	
	Kemuri_GraphicDelete( kemu );
	FE_FreeMemory( kemu );
}

//--------------------------------------------------------------
/**
 * 土煙 破れた世界対応
 * @param	fes	FE_SYS_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FE_Kemuri_TornWorldON( FE_SYS *fes )
{
	FE_KEMURI_PTR kemu;
	
	kemu = FE_EffectWorkGet( fes, FE_FLD_KEMURI );
	
	if( kemu->tw_flag != TRUE ){
		kemu->tw_flag = TRUE;
		Kemuri_TwGraphicAdd( kemu );
	}
}

//==============================================================================
//	土煙　グラフィック
//==============================================================================
//--------------------------------------------------------------
/**
 * 草　グラフィック初期化
 * @param	kemu	FE_GRASS_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Kemuri_GraphicInit( FE_KEMURI_PTR kemu )
{
	FE_BlActResAdd_MdlArc( kemu->fes, FE_RESID_MDL_KEMURI, NARC_fldeff_hero_kemu_nsbmd );
	FE_BlActResAdd_AnmArc( kemu->fes, FE_RESID_ANM_KEMURI, NARC_fldeff_hero_kemu_itpcv_dat );
	FE_BlActResAdd_TexArc( kemu->fes,
			FE_RESID_TEX_KEMURI, NARC_fldeff_hero_kemu_nsbtx, TEXRESM_TEX_CUT_TRUE );
	
	FE_BlActHeaderManageAddResmID( kemu->fes, FE_BLACT_H_ID_KEMURI,
			FE_RESID_MDL_KEMURI, FE_RESID_ANM_KEMURI,
			FE_RESID_TEX_KEMURI, FE_BLACT_TEX_VRAM,
			DATA_BlActAnmTbl_Kemuri );
}

//--------------------------------------------------------------
/**
 * 草　グラフィック初期化　破れた世界用上左右追加
 * @param	kemu	FE_GRASS_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Kemuri_TwGraphicAdd( FE_KEMURI_PTR kemu )
{
	FE_BlActResAdd_MdlArc(
		kemu->fes, FE_RESID_MDL_KEMURI_L, NARC_fldeff_hero_kemu_l_nsbmd );
	FE_BlActResAdd_MdlArc(
		kemu->fes, FE_RESID_MDL_KEMURI_R, NARC_fldeff_hero_kemu_r_nsbmd );
	FE_BlActResAdd_MdlArc(
		kemu->fes, FE_RESID_MDL_KEMURI_U, NARC_fldeff_hero_kemu_u_nsbmd );
	
	FE_BlActHeaderManageAddResmID(
			kemu->fes, FE_BLACT_H_ID_KEMURI_L,
			FE_RESID_MDL_KEMURI_L, FE_RESID_ANM_KEMURI,
			FE_RESID_TEX_KEMURI, FE_BLACT_TEX_VRAM,
			DATA_BlActAnmTbl_Kemuri );
	FE_BlActHeaderManageAddResmID(
			kemu->fes, FE_BLACT_H_ID_KEMURI_R,
			FE_RESID_MDL_KEMURI_R, FE_RESID_ANM_KEMURI,
			FE_RESID_TEX_KEMURI, FE_BLACT_TEX_VRAM,
			DATA_BlActAnmTbl_Kemuri );
	FE_BlActHeaderManageAddResmID(
			kemu->fes, FE_BLACT_H_ID_KEMURI_U,
			FE_RESID_MDL_KEMURI_U, FE_RESID_ANM_KEMURI,
			FE_RESID_TEX_KEMURI, FE_BLACT_TEX_VRAM,
			DATA_BlActAnmTbl_Kemuri );
}

//--------------------------------------------------------------
/**
 * 土煙　グラフィック削除
 * @param	kemu	FE_KEMURI_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Kemuri_GraphicDelete( FE_KEMURI_PTR kemu )
{
	FE_BlActResDelete_Mdl( kemu->fes, FE_RESID_MDL_KEMURI );
	FE_BlActResDelete_Anm( kemu->fes, FE_RESID_ANM_KEMURI );
	FE_BlActResDelete_Tex( kemu->fes, FE_RESID_TEX_KEMURI );
	FE_BlActHeaderManageFree( kemu->fes, FE_BLACT_H_ID_KEMURI );
	
	if( kemu->tw_flag == TRUE ){
		FE_BlActResDelete_Mdl( kemu->fes, FE_RESID_MDL_KEMURI_L );
		FE_BlActResDelete_Mdl( kemu->fes, FE_RESID_MDL_KEMURI_R );
		FE_BlActResDelete_Mdl( kemu->fes, FE_RESID_MDL_KEMURI_U );
		FE_BlActHeaderManageFree( kemu->fes, FE_BLACT_H_ID_KEMURI_L );
		FE_BlActHeaderManageFree( kemu->fes, FE_BLACT_H_ID_KEMURI_R );
		FE_BlActHeaderManageFree( kemu->fes, FE_BLACT_H_ID_KEMURI_U );
	}
}

//==============================================================================
//	EOA 土煙
//==============================================================================
//--------------------------------------------------------------
/**
 * フィールドOBJ用土煙追加 ヘッダー作成
 * @param	fes			初期化済みFE_SYS*
 * @param	fldobj		追加するFIELD_OBJ_PTR
 * @param	head		初期化するKEMURI_ADD_H
 * @param	pos			座標格納先
 * @param	pri			初期化するTCBプライオリティ
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Kemuri_HeaderMake( FE_SYS *fes,
	FIELD_OBJ_PTR fldobj, KEMURI_ADD_H *head, VecFx32 *pos, int *pri )
{
	head->fsys = FieldOBJ_FieldSysWorkGet( fldobj );
	head->fes = fes;
	head->kemu = FE_EffectWorkGet( fes, FE_FLD_KEMURI );
	head->fldobj = fldobj;
	
	FieldOBJ_VecPosGet( fldobj, pos );
	FieldOBJTool_GridCenterPosGet(
		FieldOBJ_NowPosGX_Get(fldobj), FieldOBJ_NowPosGZ_Get(fldobj), pos );
	
	*pri = FieldOBJ_TCBPriGet( fldobj, FLDOBJ_TCBPRI_OFFS_AFTER );
}

//--------------------------------------------------------------
/**
 * フィールドOBJ用土煙追加
 * @param	fldobj		FIELD_OBJ_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FE_FldOBJKemuri_Add( FIELD_OBJ_PTR fldobj )
{
	int pri;
	FE_SYS *fes;
	EOA_PTR eoa;
	VecFx32 pos;
	KEMURI_ADD_H head;
	
	fes = FE_FieldOBJ_FE_SYS_Get( fldobj );
	Kemuri_HeaderMake( fes, fldobj, &head, &pos, &pri );
	pos.z += KEMURI_DRAW_Z_OFFS;
	eoa = FE_EoaAddNpp( fes,
		&DATA_EoaH_Kemuri, &pos, FE_KEMURI_TYPE_FLAT, &head, pri );
}

#if 0 //old DP
void FE_FldOBJKemuri_Add( FIELD_OBJ_PTR fldobj )
{
	KEMURI_ADD_H head;
	FE_SYS *fes;
	EOA_PTR eoa;
	VecFx32 vec;
	int pri;
	
	fes = FE_FieldOBJ_FE_SYS_Get( fldobj );
	
	head.fsys = FieldOBJ_FieldSysWorkGet( fldobj );
	head.fes = fes;
	head.kemu = FE_EffectWorkGet( fes, FE_FLD_KEMURI );
	head.fldobj = fldobj;
	
	FieldOBJ_VecPosGet( fldobj, &vec );
	FieldOBJTool_GridCenterPosGet(
		FieldOBJ_NowPosGX_Get(fldobj), FieldOBJ_NowPosGZ_Get(fldobj), &vec );
	
	pri = FieldOBJ_TCBPriGet( fldobj, FLDOBJ_TCBPRI_OFFS_AFTER );
	eoa = FE_EoaAddNpp( fes, &DATA_EoaH_Kemuri, &vec, 0, &head, pri );
}
#endif

//--------------------------------------------------------------
/**
 * フィールドOBJ用土煙追加 破れた世界用方向指定版
 * @param	fldobj		FIELD_OBJ_PTR
 @ @param	type		FE_KEMURI_TYPE
 * @retval	nothing
 */
//--------------------------------------------------------------
void FE_FldOBJKemuriTornWorld_Add( FIELD_OBJ_PTR fldobj, FE_KEMURI_TYPE type )
{
	int pri;
	FE_SYS *fes;
	EOA_PTR eoa;
	VecFx32 pos;
	KEMURI_ADD_H head;
	
	VecFx32 offs[FE_KEMURI_TYPE_MAX] =
	{ {0,0,KEMURI_DRAW_Z_OFFS},
	  {NUM_FX32(11),NUM_FX32(14),NUM_FX32(17)},
	  {NUM_FX32(-11),NUM_FX32(14),NUM_FX32(17)},
	  {0,NUM_FX32(32),KEMURI_DRAW_Z_OFFS} };
	  
	fes = FE_FieldOBJ_FE_SYS_Get( fldobj );
	Kemuri_HeaderMake( fes, fldobj, &head, &pos, &pri );
	GF_ASSERT( head.kemu->tw_flag == TRUE );
	pos.x += offs[type].x;
	pos.y += offs[type].y;
	pos.z += offs[type].z;
	eoa = FE_EoaAddNpp( fes, &DATA_EoaH_Kemuri, &pos, type, &head, pri );
}

//--------------------------------------------------------------
/**
 * EOA 土煙　初期化
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	int		TRUE=正常終了。FALSE=異常終了
 */
//--------------------------------------------------------------
static int EoaKemuri_Init( EOA_PTR eoa, void *wk )
{
	int dir;
	VecFx32 vec;
	KEMURI_WORK *work;
	const KEMURI_ADD_H *head;
	
	work = wk;
	head = EOA_AddPtrGet( eoa );
	
	work->head = *head;
	work->obj_id = FieldOBJ_OBJIDGet( work->head.fldobj );
	work->zone_id = FieldOBJ_ZoneIDGet( work->head.fldobj );
	EOA_MatrixGet( eoa, &vec );
	
	{
		u32 id[FE_KEMURI_TYPE_MAX] =
		 {FE_BLACT_H_ID_KEMURI,FE_BLACT_H_ID_KEMURI_L,
		  FE_BLACT_H_ID_KEMURI_R,FE_BLACT_H_ID_KEMURI_U};
		FE_KEMURI_TYPE type = EOA_AddParamGet( eoa );
		work->act = FE_BlActAddID( work->head.fes, id[type], &vec );
	}
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * EOA 土煙　削除
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaKemuri_Delete( EOA_PTR eoa, void *wk )
{
	KEMURI_WORK *work;
	
	work = wk;
	
	BLACT_Delete( work->act );
}

//--------------------------------------------------------------
/**
 * EOA 土煙　動作
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaKemuri_Move( EOA_PTR eoa, void *wk )
{
	int frame;
	KEMURI_WORK *work;
	FIELD_OBJ_PTR fldobj;
	
	work = wk;
	fldobj = work->head.fldobj;
	
	switch( work->seq_no ){
	case 0:													//揺れ
		BLACT_AnmFrameChg( work->act, FX32_ONE );
		frame = BLACT_AnmFrameGetOffs( work->act ) / FX32_ONE;
		
		if( frame >= KEMURI_ANIME_FRAME ){
			FE_EoaDelete( eoa );
			return;
		}
		
		break;
	}
}

//--------------------------------------------------------------
/**
 * EOA 土煙　描画
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaKemuri_Draw( EOA_PTR eoa, void *wk )
{
	VecFx32 vec,d_vec;
	KEMURI_WORK *work;
	FIELD_OBJ_PTR fldobj;
	
	work = wk;
	fldobj = work->head.fldobj;
	
	EOA_MatrixGet( eoa, &vec );
	BLACT_MatrixSet( work->act, &vec );
}

//==============================================================================
//	data
//==============================================================================
//--------------------------------------------------------------
///	土煙EOA_H
//--------------------------------------------------------------
static const EOA_H_NPP DATA_EoaH_Kemuri =
{
	KEMURI_WORK_SIZE,
	EoaKemuri_Init,
	EoaKemuri_Delete,
	EoaKemuri_Move,
	EoaKemuri_Draw,
};

//--------------------------------------------------------------
///	土煙アニメ
//--------------------------------------------------------------
static const BLACT_ANIME_TBL DATA_BlActAnmTbl_Kemuri[] =
{
	{ 0, KEMURI_ANIME_FRAME, BLACT_ANIM_END },
	{ 0, 0, BLACT_ANIM_CMD_MAX },
};

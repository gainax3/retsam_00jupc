//******************************************************************************
/**
 * 
 * @file	fldeff_footmark.c
 * @brief	フィールドOBJ足跡
 * @author	kagaya
 * @data	05.07.13
 *
 */
//******************************************************************************
#include "common.h"
#include "fieldsys.h"
#include "field_effect.h"
#include "fieldobj.h"
#include "fldeff_footmark.h"

//==============================================================================
//	define
//==============================================================================
#define FOOTMARK_SNOWCYCLE_OFF	///<定義で影雪の自転車足跡を無効に

#define FOOTMARK_VANISH_START_FRAME (16)			///<点滅開始フレーム(赤緑40)
#define FOOTMARK_VANISH_END_FRAME (28)				///<点滅終了フレーム(赤緑56)

//--------------------------------------------------------------
///	足跡識別
//--------------------------------------------------------------
enum
{
	FOOTMARK_WALK_UP = 0,							///<足跡　上
	FOOTMARK_WALK_DOWN,								///<足跡　下
	FOOTMARK_WALK_LEFT,								///<足跡　左
	FOOTMARK_WALK_RIGHT,							///<足跡　右
	FOOTMARK_CYCLE_UD,								///<自転車足跡 上下
	FOOTMARK_CYCLE_LR,								///<自転車足跡 左右
	FOOTMARK_CYCLE_UL,								///<自転車足跡 上左
	FOOTMARK_CYCLE_UR,								///<自転車足跡 上右
	FOOTMARK_CYCLE_DL,								///<自転車足跡 下左
	FOOTMARK_CYCLE_DR,								///<自転車足跡 下右
	FOOTMARK_MAX,									///<足跡最大数
};

#define FOOTMARK_CYCLE_START (FOOTMARK_CYCLE_UD)
#define FOOTMARK_MAX_CYCLE (FOOTMARK_MAX-FOOTMARK_CYCLE_START)

//--------------------------------------------------------------
///	足跡タイプ	レンダOBJの種類と表示位置に影響する
//--------------------------------------------------------------
typedef enum
{
	FOOTMARKTYPE_NORMAL = 0,		///<足跡タイプ　２足
	FOOTMARKTYPE_CYCLE,				///<足跡タイプ　自転車
	FOOTMARKTYPE_SNOW,				///<足跡タイプ　雪
	FOOTMARKTYPE_SNOWDEEP,			///<足跡タイプ　深い雪
	FOOTMARKTYPE_SHADOWSNOW,		///<足跡タイプ　影表示雪　二足
	FOOTMARKTYPE_SHADOWSNOW_CYCLE,	///<足跡タイプ　影表示雪　自転車
}FOOTMARKTYPE;

//==============================================================================
//	typedef struct
//==============================================================================
//--------------------------------------------------------------
///	FE_FOOTMARK型
//--------------------------------------------------------------
typedef struct _TAG_FE_FOOTMARK * FE_FOOTMARK_PTR;

//--------------------------------------------------------------
///	FE_FOOTMARK構造体
//--------------------------------------------------------------
typedef struct _TAG_FE_FOOTMARK
{
	FE_SYS *fes;
	FRO_MDL rmdl_gnd[FOOTMARK_MAX];
	FRO_MDL rmdl_snow[FOOTMARK_MAX];
	#ifndef FOOTMARK_SNOWCYCLE_OFF
	FRO_MDL rmdl_ssnow_c[FOOTMARK_MAX];
	#endif
	
	FRO_OBJ robj_gnd[FOOTMARK_MAX];
	FRO_OBJ robj_snow[FOOTMARK_MAX];
	#ifndef FOOTMARK_SNOWCYCLE_OFF
	FRO_OBJ robj_ssnow_c[FOOTMARK_MAX_CYCLE];
	#endif
}FE_FOOTMARK;

#define FE_FOOTMARK_SIZE (sizeof(FE_FOOTMARK)) ///<FE_FOOTMARKサイズ

//--------------------------------------------------------------
///	FOOTMARK_ADD_H構造体
//--------------------------------------------------------------
typedef struct
{
	FOOTMARKTYPE type;
	FRO_MDL *rmdl;
	FRO_OBJ *robj;
}FOOTMARK_ADD_H;

//--------------------------------------------------------------
///	FOOTMARK_WORK構造体
//--------------------------------------------------------------
typedef struct
{
	int seq_no;
	int frame;
	int vanish_sw;
	int alpha;
	FOOTMARKTYPE type;
	FRO_MDL *rmdl;
	FRO_OBJ *robj;
}FOOTMARK_WORK;

#define FOOTMARK_WORK_SIZE (sizeof(FOOTMARK_WORK))

//==============================================================================
//	プロトタイプ
//==============================================================================
static void FootMark_GraphicInit( FE_FOOTMARK_PTR fm );
static void FootMark_GraphicDelete( FE_FOOTMARK_PTR fm );

static FRO_OBJ * FootMark_TypeObjGet(
		FE_FOOTMARK_PTR fm, FOOTMARKTYPE type, int now_dir, int old_dir );
static FRO_MDL * FootMark_TypeMdlGet(
		FE_FOOTMARK_PTR fm, FOOTMARKTYPE type, int now_dir, int old_dir );

static EOA_PTR FldOBJFootMark_AddMain( CONST_FIELD_OBJ_PTR fldobj, FOOTMARKTYPE type );

static const u32 DATA_3DModelArcID_FootMark[FOOTMARK_MAX];
static const u32 DATA_3DModelArcID_FootMarkSnow[FOOTMARK_MAX];

#ifndef FOOTMARK_SNOWCYCLE_OFF
static const u32 DATA_3DModelArcID_FootMarkShadowSnowCycle[FOOTMARK_MAX_CYCLE];
#endif
static const int DATA_FootMarkCycleDirTbl[DIR_4_MAX][DIR_4_MAX];
static const EOA_H_NPP DATA_EoaH_FootMark;

//==============================================================================
//	足跡　システム
//==============================================================================
//--------------------------------------------------------------
/**
 * 足跡初期化
 * @param	fes		FE_SYS_PTR
 * @retval	FE_FOOTMARK_PTR		FE_FOOTMARK_PTR
 */
//--------------------------------------------------------------
void * FE_FootMark_Init( FE_SYS *fes )
{
	FE_FOOTMARK_PTR fm;
	
	fm = FE_AllocClearMemory( fes, FE_FOOTMARK_SIZE, ALLOC_FR, 0 );
	fm->fes = fes;
	
	FootMark_GraphicInit( fm );
	return( fm );
}

//--------------------------------------------------------------
/**
 * 足跡削除
 * @param	fm		FE_FOOTMARK_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FE_FootMark_Delete( void *work )
{
	FE_FOOTMARK_PTR fm = work;
	FootMark_GraphicDelete( fm );
	FE_FreeMemory( fm );
}

//==============================================================================
//	足跡　グラフィック
//==============================================================================
//--------------------------------------------------------------
/**
 * 足跡 グラフィック初期化
 * @param	fm		FE_FOOTMARK_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FootMark_GraphicInit( FE_FOOTMARK_PTR fm )
{
	u32 i,idx;
	
	for( i = 0; i < FOOTMARK_MAX; i++ ){
		idx = DATA_3DModelArcID_FootMark[i];
		FE_FROMdl_ResSetArcLoadTexTrans(
			fm->fes, &fm->rmdl_gnd[i], 0, idx, ALLOC_FR );
		FRO_OBJ_InitInMdl( &fm->robj_gnd[i], &fm->rmdl_gnd[i] );
		
		idx = DATA_3DModelArcID_FootMarkSnow[i];
		FE_FROMdl_ResSetArcLoadTexTrans(
			fm->fes, &fm->rmdl_snow[i], 0, idx, ALLOC_FR );
		FRO_OBJ_InitInMdl( &fm->robj_snow[i], &fm->rmdl_snow[i] );
	}
	
	#ifdef DEBUG_ONLY_FOR_kagaya
	{
		u32 free = sys_GetHeapFreeSize( HEAPID_FIELD );
		OS_Printf( "FldEff FootMark0 FIELD HEAP FreeSize %xH\n", free );
	}
	#endif
	
	#ifndef FOOTMARK_SNOWCYCLE_OFF
	for( i = 0; i < FOOTMARK_MAX_CYCLE; i++ ){
		idx = DATA_3DModelArcID_FootMarkShadowSnowCycle[i];
		FE_FROMdl_ResSetArcLoadTexTrans(
			fm->fes, &fm->rmdl_ssnow_c[i], 0, idx, ALLOC_FR );
		FRO_OBJ_InitInMdl( &fm->robj_ssnow_c[i], &fm->rmdl_ssnow_c[i] );
	}
	#endif
	
	#ifdef DEBUG_ONLY_FOR_kagaya
	{
		u32 free = sys_GetHeapFreeSize( HEAPID_FIELD );
		OS_Printf( "FldEff FootMark1 FIELD HEAP FreeSize %xH\n", free );
	}
	#endif
}

//--------------------------------------------------------------
/**
 * 足跡　グラフィック削除
 * @param	fm		FE_FOOTMARK_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FootMark_GraphicDelete( FE_FOOTMARK_PTR fm )
{
	int i;
	
	for( i = 0; i < FOOTMARK_MAX; i++ ){
		FRO_MDL_DeleteAll( &fm->rmdl_gnd[i] );
		FRO_MDL_DeleteAll( &fm->rmdl_snow[i] );
	}
	
	#ifndef FOOTMARK_SNOWCYCLE_OFF
	for( i = 0; i < FOOTMARK_MAX_CYCLE; i++ ){
		FRO_MDL_DeleteAll( &fm->rmdl_ssnow_c[i] );
	}
	#endif
}

//--------------------------------------------------------------
/**
 * 指定タイプのレンダ取得
 * @param	fm		FE_FOOTMARK_PTR
 * @param	type	FOOTMARK_TYPE_NORMAL等
 * @param	now_dir	現在の向き DIR_UP等
 * @param	old_dir	過去の向き DIR_UP等
 * @param	attr	FOOTMARK_ATTR
 * @retval	NNSG3dRenderObj 指定のレンダ
 */
//--------------------------------------------------------------
static FRO_OBJ * FootMark_TypeObjGet(
		FE_FOOTMARK_PTR fm, FOOTMARKTYPE type, int now_dir, int old_dir )
{
	int no;
	FRO_OBJ *robj = NULL;
	
	switch( type ){
	case FOOTMARKTYPE_NORMAL:
		no = FOOTMARK_WALK_UP + now_dir;
		robj = &fm->robj_gnd[no];
		break;
	case FOOTMARKTYPE_CYCLE:
		no = DATA_FootMarkCycleDirTbl[old_dir][now_dir];
		robj = &fm->robj_gnd[no];
		break;
	case FOOTMARKTYPE_SNOW:
	case FOOTMARKTYPE_SHADOWSNOW:
		no = FOOTMARK_WALK_UP + now_dir;
		robj = &fm->robj_snow[no];
		break;
	case FOOTMARKTYPE_SNOWDEEP:
		no = DATA_FootMarkCycleDirTbl[old_dir][now_dir];
		robj = &fm->robj_snow[no];
		break;
	#ifndef FOOTMARK_SNOWCYCLE_OFF
	case FOOTMARKTYPE_SHADOWSNOW_CYCLE:
		no = DATA_FootMarkCycleDirTbl[old_dir][now_dir];
		no -= FOOTMARK_CYCLE_START;
		GF_ASSERT( no < FOOTMARK_MAX_CYCLE );
		robj = &fm->robj_ssnow_c[no];
		break;
	#endif
	}
	
	GF_ASSERT( robj != NULL );
	return( robj );
}

//--------------------------------------------------------------
/**
 * 指定タイプのモデル取得
 * @param	fm		FE_FOOTMARK_PTR
 * @param	type	FOOTMARK_TYPE_NORMAL等
 * @param	now_dir	現在の向き DIR_UP等
 * @param	old_dir	過去の向き DIR_UP等
 * @param	attr	FOOTMARK_ATTR
 * @retval	NNSG3dMdl 指定のレンダ
 */
//--------------------------------------------------------------
static FRO_MDL * FootMark_TypeMdlGet(
		FE_FOOTMARK_PTR fm, FOOTMARKTYPE type, int now_dir, int old_dir )
{
	int no = 0;
	FRO_MDL *rmdl = NULL;
	
	switch( type ){
	case FOOTMARKTYPE_NORMAL:
		no = FOOTMARK_WALK_UP + now_dir;
		rmdl = &fm->rmdl_gnd[no];
		break;
	case FOOTMARKTYPE_CYCLE:
		no = DATA_FootMarkCycleDirTbl[old_dir][now_dir];
		rmdl = &fm->rmdl_gnd[no];
		break;
	case FOOTMARKTYPE_SNOW:
	case FOOTMARKTYPE_SHADOWSNOW:
		no = FOOTMARK_WALK_UP + now_dir;
		rmdl = &fm->rmdl_snow[no];
		break;
	case FOOTMARKTYPE_SNOWDEEP:
		no = DATA_FootMarkCycleDirTbl[old_dir][now_dir];
		rmdl = &fm->rmdl_snow[no];
		break;
	#ifndef FOOTMARK_SNOWCYCLE_OFF
	case FOOTMARKTYPE_SHADOWSNOW_CYCLE:
		no = DATA_FootMarkCycleDirTbl[old_dir][now_dir];
		no -= FOOTMARK_CYCLE_START;
		GF_ASSERT( no < FOOTMARK_MAX_CYCLE );
		rmdl = &fm->rmdl_ssnow_c[no];
		break;
	#endif
	}
	
	GF_ASSERT( rmdl != NULL );
	return( rmdl );
}

//==============================================================================
//	足跡　EOA
//==============================================================================
//--------------------------------------------------------------
/**
 * フィールドOBJ ２速足跡追加
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	eoa		EOA_PTR
 */
//--------------------------------------------------------------
EOA_PTR FE_FldOBJFootMarkNormal_Add( CONST_FIELD_OBJ_PTR fldobj )
{
	return( FldOBJFootMark_AddMain(fldobj,FOOTMARKTYPE_NORMAL) );
}

//--------------------------------------------------------------
/**
 * フィールドOBJ 自転車足跡追加
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	eoa		EOA_PTR
 */
//--------------------------------------------------------------
EOA_PTR FE_FldOBJFootMarkCycle_Add( CONST_FIELD_OBJ_PTR fldobj )
{
	return( FldOBJFootMark_AddMain(fldobj,FOOTMARKTYPE_CYCLE) );
}

//--------------------------------------------------------------
/**
 * フィールドOBJ 雪足跡追加
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	eoa		EOA_PTR
 */
//--------------------------------------------------------------
EOA_PTR FE_FldOBJFootMarkSnow_Add( CONST_FIELD_OBJ_PTR fldobj )
{
	return( FldOBJFootMark_AddMain(fldobj,FOOTMARKTYPE_SNOW) );
}

//--------------------------------------------------------------
/**
 * フィールドOBJ 深い雪足跡追加
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	eoa		EOA_PTR
 */
//--------------------------------------------------------------
EOA_PTR FE_FldOBJFootMarkSnowDeep_Add( CONST_FIELD_OBJ_PTR fldobj )
{
	return( FldOBJFootMark_AddMain(fldobj,FOOTMARKTYPE_SNOWDEEP) );
}

//--------------------------------------------------------------
/**
 * フィールドOBJ 影表示付き雪足跡追加
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	eoa		EOA_PTR
 */
//--------------------------------------------------------------
EOA_PTR FE_FldOBJFootMarkShadowSnow_Add( CONST_FIELD_OBJ_PTR fldobj )
{
	return( FldOBJFootMark_AddMain(fldobj,FOOTMARKTYPE_SHADOWSNOW) );
}

//--------------------------------------------------------------
/**
 * フィールドOBJ 影表示付き雪自転車足跡追加
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	eoa		EOA_PTR
 */
//--------------------------------------------------------------
EOA_PTR FE_FldOBJFootMarkShadowSnowCycle_Add( CONST_FIELD_OBJ_PTR fldobj )
{
	#ifndef FOOTMARK_SNOWCYCLE_OFF
	return( FldOBJFootMark_AddMain(fldobj,FOOTMARKTYPE_SHADOWSNOW_CYCLE) );
	#else
	return( NULL );
	#endif
}

//--------------------------------------------------------------
/**
 * フィールドOBJ足跡追加　メイン
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	attr	FOOTMARK_ATTR
 * @retval	eoa		EOA_PTR
 */
//--------------------------------------------------------------
static EOA_PTR FldOBJFootMark_AddMain( CONST_FIELD_OBJ_PTR fldobj, FOOTMARKTYPE type )
{
	VecFx32 mtx;
	EOA_PTR eoa;
	FOOTMARK_ADD_H head;
	int gx = FieldOBJ_OldPosGX_Get( fldobj );
	int gz = FieldOBJ_OldPosGZ_Get( fldobj );
	fx32 y = FieldOBJ_VecPosYGet( fldobj );
	int now_dir = FieldOBJ_DirDispGet( fldobj );
	int old_dir = FieldOBJ_DirDispOldGet( fldobj );
	int pri = FieldOBJ_TCBPriGet( fldobj, FLDOBJ_TCBPRI_OFFS_AFTER );
	FE_SYS *fes = FE_FieldOBJ_FE_SYS_Get( fldobj );
	FE_FOOTMARK_PTR fm = FE_EffectWorkGet( fes, FE_FLD_FOOTMARK );
	
	head.type = type;
	head.robj = FootMark_TypeObjGet( fm, type, now_dir, old_dir );
	head.rmdl = FootMark_TypeMdlGet( fm, type, now_dir, old_dir );
	
	FieldOBJTool_GridCenterPosGet( gx, gz, &mtx );
	mtx.y = y;
	
	switch( type ){
	case FOOTMARKTYPE_NORMAL:
	case FOOTMARKTYPE_CYCLE:
	case FOOTMARKTYPE_SHADOWSNOW:
	case FOOTMARKTYPE_SHADOWSNOW_CYCLE:
		mtx.y += NUM_FX32( -8 );
			
		if( type == FOOTMARK_WALK_LEFT || type == FOOTMARK_WALK_RIGHT ){
			mtx.z += NUM_FX32( -2 );
		}else{
			mtx.z += NUM_FX32( 1 );
		}
		
		break;
	case FOOTMARKTYPE_SNOW:
	case FOOTMARKTYPE_SNOWDEEP:
		mtx.y += NUM_FX32( -10 );
			
		if( type == FOOTMARK_WALK_LEFT || type == FOOTMARK_WALK_RIGHT ){
			mtx.z += NUM_FX32( -2 );
		}else{
			mtx.z += NUM_FX32( 2 );
		}
	}
	
	eoa = FE_EoaAddNpp( fes, &DATA_EoaH_FootMark, &mtx, 0, &head, pri );
	return( eoa );
}

//--------------------------------------------------------------
/**
 * EOA 足跡　初期化
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work *
 * @retval	int TRUE=正常終了。FALSE=異常終了
 */
//--------------------------------------------------------------
static int EoaFootMark_Init( EOA_PTR eoa, void *wk )
{
	VecFx32 mtx;
	FOOTMARK_WORK *work;
	const FOOTMARK_ADD_H *head;
	
	work = wk;
	work->type = EOA_AddParamGet( eoa );
	
	head = EOA_AddPtrGet( eoa );
	work->robj = head->robj;
	work->rmdl = head->rmdl;
	work->alpha = 31;
	work->type = head->type;
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * EOA 足跡　削除
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaFootMark_Delete( EOA_PTR eoa, void *wk )
{
}

//--------------------------------------------------------------
/**
 * EOA 足跡　動作
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaFootMark_Move( EOA_PTR eoa, void *wk )
{
	FOOTMARK_WORK *work;
	
	work = wk;
	
	switch( work->seq_no ){
	case 0:
		work->frame++;
		
		if( work->frame >= FOOTMARK_VANISH_START_FRAME ){
			work->seq_no++;
		}
		
		break;
	case 1:
		work->alpha -= 2;
		
		if( work->alpha < 0 ){
			FE_EoaDelete( eoa );
			return;
		}
	}
}

//--------------------------------------------------------------
/**
 * EOA 足跡　描画
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaFootMark_Draw( EOA_PTR eoa, void *wk )
{
	FOOTMARK_WORK *work = wk;
	
	if( work->vanish_sw == FALSE ){
		VecFx32 pos;
		EOA_MatrixGet( eoa, &pos );
		NNS_G3dMdlUseMdlAlpha( work->rmdl->pResMdl );
		NNS_G3dMdlSetMdlAlphaAll( work->rmdl->pResMdl, work->alpha );
		FRO_OBJ_DrawPos( work->robj, &pos );
	}
}

//==============================================================================
//	data
//==============================================================================
//--------------------------------------------------------------
///	足跡imdテーブル　並びはFOOTMARK_WALK_UP等の値に一致
//--------------------------------------------------------------
static const u32 DATA_3DModelArcID_FootMark[FOOTMARK_MAX] =
{
	NARC_fldeff_f_mark_u_nsbmd,
	NARC_fldeff_f_mark_d_nsbmd,
	NARC_fldeff_f_mark_l_nsbmd,
	NARC_fldeff_f_mark_r_nsbmd,
	NARC_fldeff_c_mark_u_nsbmd,
	NARC_fldeff_c_mark_l_nsbmd,
	NARC_fldeff_c_mark_ul_nsbmd,
	NARC_fldeff_c_mark_ur_nsbmd,
	NARC_fldeff_c_mark_dl_nsbmd,
	NARC_fldeff_c_mark_dr_nsbmd,
};

//--------------------------------------------------------------
///	足跡imdテーブル　並びはFOOTMARK_WALK_UP等の値に一致　雪
//--------------------------------------------------------------
static const u32 DATA_3DModelArcID_FootMarkSnow[FOOTMARK_MAX] =
{
	NARC_fldeff_nf_mark_u_nsbmd,
	NARC_fldeff_nf_mark_d_nsbmd,
	NARC_fldeff_nf_mark_l_nsbmd,
	NARC_fldeff_nf_mark_r_nsbmd,
	NARC_fldeff_nc_mark_u_nsbmd,
	NARC_fldeff_nc_mark_l_nsbmd,
	NARC_fldeff_nc_mark_ul_nsbmd,
	NARC_fldeff_nc_mark_ur_nsbmd,
	NARC_fldeff_nc_mark_dl_nsbmd,
	NARC_fldeff_nc_mark_dr_nsbmd,
};

//--------------------------------------------------------------
///	足跡imdテーブル　並びはFOOTMARK_WALK_UP等の値に一致　影表示雪　自転車
//--------------------------------------------------------------
#ifndef FOOTMARK_SNOWCYCLE_OFF
static const u32 DATA_3DModelArcID_FootMarkShadowSnowCycle[FOOTMARK_MAX_CYCLE] =
{
	NARC_fldeff_sc_mark_u_nsbmd,
	NARC_fldeff_sc_mark_l_nsbmd,
	NARC_fldeff_sc_mark_ul_nsbmd,
	NARC_fldeff_sc_mark_ur_nsbmd,
	NARC_fldeff_sc_mark_dl_nsbmd,
	NARC_fldeff_sc_mark_dr_nsbmd,
};
#endif

//--------------------------------------------------------------
///	自転車足跡テーブル [過去方向][現在方向]
//--------------------------------------------------------------
static const int DATA_FootMarkCycleDirTbl[DIR_4_MAX][DIR_4_MAX] =
{
	{FOOTMARK_CYCLE_UD,FOOTMARK_CYCLE_UD,FOOTMARK_CYCLE_UR,FOOTMARK_CYCLE_UL},
	{FOOTMARK_CYCLE_UD,FOOTMARK_CYCLE_UD,FOOTMARK_CYCLE_DR,FOOTMARK_CYCLE_DL},
	{FOOTMARK_CYCLE_DL,FOOTMARK_CYCLE_UL,FOOTMARK_CYCLE_LR,FOOTMARK_CYCLE_LR},
	{FOOTMARK_CYCLE_DR,FOOTMARK_CYCLE_UR,FOOTMARK_CYCLE_LR,FOOTMARK_CYCLE_LR},
};

//--------------------------------------------------------------
///	足跡EOA_H
//--------------------------------------------------------------
static const EOA_H_NPP DATA_EoaH_FootMark =
{
	FOOTMARK_WORK_SIZE,
	EoaFootMark_Init,
	EoaFootMark_Delete,
	EoaFootMark_Move,
	EoaFootMark_Draw,
};


//******************************************************************************
/**
 * 
 * @file	fldeff_gym07eff.c
 * @brief	格闘ジムエフェクト
 * @author	kagaya
 * @data	05.07.13
 *
 */
//******************************************************************************
#include "common.h"
#include "fieldsys.h"
#include "field_effect.h"
#include "fieldobj.h"
#include "fldeff_gym07eff.h"

//==============================================================================
//	define
//==============================================================================
#define BOX_OY_FX (NUM_FX32(6))

//--------------------------------------------------------------
///	モデル種類
//--------------------------------------------------------------
enum
{
	GYM07MDL_BOX_BREAK,	///<木箱破壊
	GYM07MDL_BOX,		///<木箱
	GYM07MDL_SBAG,		///<サンドバッグ
	GYM07MDL_MAX,		///<モデル最大
};

//--------------------------------------------------------------
///	アニメ種類
//--------------------------------------------------------------
enum
{
	GYM07ANM_SBAG01_U,	///<サンドバッグ
	GYM07ANM_SBAG01_D,	///<サンドバッグ
	GYM07ANM_SBAG01_L,	///<サンドバッグ
	GYM07ANM_SBAG01_R,	///<サンドバッグ
	GYM07ANM_SBAG02_U,	///<サンドバッグ
	GYM07ANM_SBAG02_D,	///<サンドバッグ
	GYM07ANM_SBAG02_L,	///<サンドバッグ
	GYM07ANM_SBAG02_R,	///<サンドバッグ
	GYM07ANM_BOX_BREAK_ICA,	///<箱破壊アニメ
	GYM07ANM_BOX_BREAK_IMA,	///<箱破壊アニメ
	GYM07ANM_BOX_BREAK_ITA,	///<箱破壊アニメ
	GYM07ANM_BOX_BREAK_ITP,	///<箱破壊アニメ
	GYM07ANM_MAX,		///<アニメ最大
};

//--------------------------------------------------------------
///	箱アニメ種類
//--------------------------------------------------------------
enum
{
	BOXANM_ICA,
	BOXANM_IMA,
	BOXANM_ITA,
	BOXANM_ITP,
	BOXANM_MAX,
};

//--------------------------------------------------------------
///	箱アニメビット
//--------------------------------------------------------------
#define BOXANMBIT_ICA (1<<0)
#define BOXANMBIT_IMA (1<<1)
#define BOXANMBIT_ITA (1<<2)
#define BOXANMBIT_ITP (1<<3)
#define BOXANMBIT_ALL (BOXANMBIT_ICA|BOXANMBIT_IMA|BOXANMBIT_ITA|BOXANMBIT_ITP)
		
//--------------------------------------------------------------
///	EOA種類
//--------------------------------------------------------------
enum
{
	GYM07_BOX,
	GYM07_BAG,
};

//--------------------------------------------------------------
///	箱シーケンス
//--------------------------------------------------------------
enum
{
	SEQNO_BOX_NON,
	SEQNO_BOX_ANM,
};

//--------------------------------------------------------------
///	サンドバッグシーケンス
//--------------------------------------------------------------
enum
{
	SEQNO_BAG_NON,
	SEQNO_BAG_ANM,
};

//==============================================================================
//	typedef struct
//==============================================================================
//--------------------------------------------------------------
///	FE_GYM07EFF型
//--------------------------------------------------------------
typedef struct _TAG_FE_GYM07EFF * FE_GYM07EFF_PTR;

//--------------------------------------------------------------
///	FE_GYM07EFF構造体
//--------------------------------------------------------------
typedef struct _TAG_FE_GYM07EFF
{
	int box_max;
	int bag_max;
	FE_SYS *fes;
	FRO_MDL m_rmdl[GYM07MDL_MAX];
	FRO_ANM m_ranm[GYM07ANM_MAX];
	EOA_PTR *eoa_box_tbl;
	EOA_PTR *eoa_bag_tbl;
}FE_GYM07EFF;

#define FE_GYM07EFF_SIZE (sizeof(FE_GYM07EFF)) ///<FE_GYM07EFFサイズ

//--------------------------------------------------------------
///	GYM07EFF_ADD_H
//--------------------------------------------------------------
typedef struct
{
	u8 type;
	s8 init_dir;
	s16 init_gx;
	s16 init_gz;
	fx32 oy;
	FE_GYM07EFF_PTR gym07;
	FIELDSYS_WORK *fsys;
}GYM07EFF_ADD_H;

//--------------------------------------------------------------
///	GYM07EFF_EOA_WORK
//--------------------------------------------------------------
typedef struct
{
	u8 seq_no:6;
	u8 anm_flag:1;
	u8 anm_end:1;
	s8 dir;
	s16 gx;
	s16 gz;
	fx32 anm_speed;
	FRO_ANM ranm;
	FRO_OBJ robj;
	GYM07EFF_ADD_H head;
}GYM07EOA_WORK;

//--------------------------------------------------------------
///	GYM07BOX_ANM_WORK
//--------------------------------------------------------------
typedef struct
{
	u32 anm_end;
	FRO_ANM ranm[BOXANM_MAX];
}GYM07BOX_ANM_WORK;

//--------------------------------------------------------------
///	GYM07BOX_EOA_WORK
//--------------------------------------------------------------
typedef struct
{
	int seq_no;
	s16 gx;
	s16 gz;
	s16 dir;
	FRO_OBJ robj;
	GYM07EFF_ADD_H head;
	GYM07BOX_ANM_WORK *bk_anm;
}GYM07BOX_EOA_WORK;

//==============================================================================
//	プロトタイプ
//==============================================================================
static void Gym07Eff_GraphicInit( FE_GYM07EFF_PTR gym07eff );
static void Gym07Eff_GraphicDelete( FE_GYM07EFF_PTR gym07eff );

static EOA_PTR * EoaGym07EffAddWorkInit(
		FIELDSYS_WORK *fsys,
		int gx, int gz, fx32 oy, int type, GYM07EFF_ADD_H *head );
static void EoaGym07WorkInit( EOA_PTR eoa, GYM07EOA_WORK *work );

static const EOA_H_NPP DATA_EoaH_Gym07Box;
static const EOA_H_NPP DATA_EoaH_Gym07Bag;

static const u32 DATA_Gym07MdlArcIdxTbl[GYM07MDL_MAX];
static const u32 DATA_Gym07AnmArcIdxTbl[GYM07ANM_MAX];
static const u32 DATA_BoxAnmNo_ResNo[BOXANM_MAX];

//==============================================================================
//	格闘ジムエフェクト　システム
//==============================================================================
//--------------------------------------------------------------
/**
 * 格闘ジムエフェクト初期化
 * @param	fes		FE_SYS_PTR
 * @retval	FE_GYM07EFF_PTR	FE_GYM07EFF_PTR
 */
//--------------------------------------------------------------
void * FE_Gym07Eff_Init( FE_SYS *fes )
{
	FE_GYM07EFF_PTR gym07eff;
	
	gym07eff = FE_AllocClearMemory( fes, FE_GYM07EFF_SIZE, ALLOC_FR, 0 );
	gym07eff->fes = fes;
	
	Gym07Eff_GraphicInit( gym07eff );
	return( gym07eff );
}

//--------------------------------------------------------------
/**
 * 格闘ジムエフェクト削除
 * @param	gym07eff		FE_GYM07EFF_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FE_Gym07Eff_Delete( void *work )
{
	FE_GYM07EFF_PTR gym07eff = work;
	
	if( gym07eff->eoa_box_tbl != NULL ){
		sys_FreeMemoryEz( gym07eff->eoa_box_tbl );
	}
		
	if( gym07eff->eoa_bag_tbl != NULL ){
		sys_FreeMemoryEz( gym07eff->eoa_bag_tbl );
	}
	
	Gym07Eff_GraphicDelete( gym07eff );
	FE_FreeMemory( gym07eff );
}

//--------------------------------------------------------------
/**
 * 格闘ジムエフェクト　サンドバッグ、箱最大数を設定
 * @param	fes	FE_SYS_PTR
 * @param	box_max	BOX最大
 * @param	bag サンドバッグ最大
 * @retval	nothing
 */
//--------------------------------------------------------------
void FE_Gym07Eff_BoxSandbagMaxSet( FE_SYS *fes, int box_max, int bag_max )
{
	int max;
	EOA_PTR eoa;
	FE_GYM07EFF *gym07;
	
	gym07 = FE_EffectWorkGet( fes, FE_FLD_GYM07EFF );
	gym07->bag_max = bag_max;
	gym07->box_max = box_max;
	
	if( box_max ){
		max = sizeof(eoa) * box_max;
		gym07->eoa_box_tbl = FE_AllocClearMemory( fes, max, ALLOC_FR, 0 );
	}
	
	if( bag_max ){
		max = sizeof(eoa) * bag_max;
		gym07->eoa_bag_tbl = FE_AllocClearMemory( fes, max, ALLOC_FR, 0 );
	}
}

//==============================================================================
//	格闘ジムエフェクト　グラフィック
//==============================================================================
//--------------------------------------------------------------
/**
 * 格闘ジムエフェクト グラフィック初期化
 * @param	gym07eff	FE_GYM07EFF_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Gym07Eff_GraphicInit( FE_GYM07EFF_PTR gym07 )
{
	int i;
	
	for( i = 0; i < GYM07MDL_MAX; i++ ){
		FE_FROMdl_ResSetArcLoadTexTrans(
			gym07->fes, &gym07->m_rmdl[i], 0,
			DATA_Gym07MdlArcIdxTbl[i], ALLOC_FR );
	}
	
	for( i = 0; i < GYM07ANM_MAX; i++ ){
		FE_FROAnm_AnmResSetArcLoad(
			gym07->fes, &gym07->m_ranm[i], 0,
			DATA_Gym07AnmArcIdxTbl[i], ALLOC_FR );
	}
}

//--------------------------------------------------------------
/**
 * 格闘ジムエフェクト グラフィック削除
 * @param	gym07eff	FE_GYM07EFF_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Gym07Eff_GraphicDelete( FE_GYM07EFF_PTR gym07 )
{
	int i;
	
	for( i = 0; i < GYM07MDL_MAX; i++ ){
		FRO_MDL_DeleteAll( &gym07->m_rmdl[i] );
	}
	
	for( i = 0; i < GYM07ANM_MAX; i++ ){
		FRO_ANM_DeleteAll( &gym07->m_ranm[i] );
	}
}

//==============================================================================
//	eoa	パーツ
//==============================================================================
//--------------------------------------------------------------
/**
 * EOA追加共通部分
 * @param	fes	FE_SYS
 * @param	 gx	グリッドX
 * @param	 gz	グリッドZ
 * @param	 oy Y座標オフセット
 * @retval	EOA_PTR	空のEOA_PTR
 */
//--------------------------------------------------------------
static EOA_PTR * EoaGym07EffAddWorkInit(
		FIELDSYS_WORK *fsys,
		int gx, int gz, fx32 oy, int type, GYM07EFF_ADD_H *head )
{
	int i,max;
	FE_SYS *fes;
	FE_GYM07EFF *gym07;
	EOA_PTR *eoa_p_tbl;
	
	fes = fsys->fes;
	gym07 = FE_EffectWorkGet( fes, FE_FLD_GYM07EFF );
	
	if( type == GYM07_BOX ){
		max = gym07->box_max;
		eoa_p_tbl = gym07->eoa_box_tbl;
	}else{
		max = gym07->bag_max;
		eoa_p_tbl = gym07->eoa_bag_tbl;
	}
	
	GF_ASSERT( max );
	
	head->init_gx = gx;
	head->init_gz = gz;
	head->oy = oy;
	head->fsys = fsys;
	head->gym07 = gym07;
	
	for( i = 0; i < max; i++ ){
		if( eoa_p_tbl[i] == NULL ){
			return( &eoa_p_tbl[i] );
		}
	}
	
	GF_ASSERT( 0 );
	return( NULL );
}

//--------------------------------------------------------------
/**
 * GYM07 EOA 初期化共通部分
 * @param	eoa		EOA_PTR
 * @param	work	GYM07EOA_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaGym07WorkInit( EOA_PTR eoa, GYM07EOA_WORK *work )
{
	VecFx32 pos;
	const GYM07EFF_ADD_H *head = EOA_AddPtrGet( eoa );
	
	work->head = *head;
	work->gx = head->init_gx;
	work->gz = head->init_gz;
	work->dir = head->init_dir;
	work->anm_speed = FX32_ONE;
	
	FieldOBJTool_GridCenterPosGet( work->gx, work->gz, &pos );
	FieldOBJTool_GetHeight( work->head.fsys, &pos );
	pos.y += BOX_OY_FX + work->head.oy;
	EOA_MatrixSet( eoa, &pos );
}

//==============================================================================
//	eoa	箱
//==============================================================================
//--------------------------------------------------------------
/**
 * 箱を追加
 * @param	fes	FE_SYS
 * @param	 gx	グリッドX
 * @param	 gz	グリッドZ
 * @param	 oy Y座標オフセット
 * @retval	EOA_PTR	追加したEOA_PTR
 */
//--------------------------------------------------------------
EOA_PTR FE_Gym07Eff_BoxAdd( FIELDSYS_WORK *fsys, int gx, int gz, fx32 oy )
{
	FE_SYS *fes;
	EOA_PTR *eoa_p;
	GYM07EFF_ADD_H head;
	
	fes = fsys->fes;
	eoa_p = EoaGym07EffAddWorkInit( fsys, gx, gz, oy, GYM07_BOX, &head );
	*eoa_p = FE_EoaAddNpp( fes, &DATA_EoaH_Gym07Box, NULL, 0, &head, 0 );
	return( *eoa_p );
}

//--------------------------------------------------------------
/**
 * 箱破壊アニメセット
 * @param	eoa	EOA_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FE_Gym07Eff_BoxBreakAnmSet( EOA_PTR eoa )
{
	int i;
	FE_GYM07EFF_PTR gym07;
	GYM07BOX_EOA_WORK *work;
	
	work = EOA_LocalWorkGet( eoa );
	gym07 = work->head.gym07;
	
	GF_ASSERT( work->bk_anm == NULL );	//セット済み
	
	work->seq_no = SEQNO_BOX_ANM;
	
	work->bk_anm = FE_AllocClearMemory(
			gym07->fes, sizeof(GYM07BOX_ANM_WORK), ALLOC_LO, 0 );
	
	FRO_OBJ_InitInMdl(
		&work->robj, &work->head.gym07->m_rmdl[GYM07MDL_BOX_BREAK] );
	
	for( i = 0; i < BOXANM_MAX; i++ ){
		FE_FROAnm_AnmResObjInitSame(
			gym07->fes,
			&work->bk_anm->ranm[i], &gym07->m_rmdl[GYM07MDL_BOX_BREAK],
			&gym07->m_ranm[DATA_BoxAnmNo_ResNo[i]], 0 );
		FRO_OBJ_AddAnmInAnm( &work->robj, &work->bk_anm->ranm[i] );
	}
}

//--------------------------------------------------------------
/**
 * 箱破壊アニメ終了チェック
 * @param	eoa	EOA_PTR
 * @retval	BOOL	TRUE=終了
 */
//--------------------------------------------------------------
BOOL FE_Gym07Eff_BoxBreakAnmCheck( EOA_PTR eoa )
{
	GYM07BOX_EOA_WORK *work;
	
	work = EOA_LocalWorkGet( eoa );
	GF_ASSERT( work->bk_anm != NULL );	//未セット
	
	if( work->bk_anm->anm_end == BOXANMBIT_ALL ){
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * EOA 箱　初期化
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work *
 * @retval	int	TRUE=正常終了 FALSE=異常終了
 */
//--------------------------------------------------------------
static int EoaGym07Box_Init( EOA_PTR eoa, void *wk )
{
	VecFx32 pos;
	GYM07BOX_EOA_WORK *work = wk;
	const GYM07EFF_ADD_H *head = EOA_AddPtrGet( eoa );
	
	work->head = *head;
	work->gx = head->init_gx;
	work->gz = head->init_gz;
	work->dir = head->init_dir;
	
	FieldOBJTool_GridCenterPosGet( work->gx, work->gz, &pos );
	FieldOBJTool_GetHeight( work->head.fsys, &pos );
	pos.y += BOX_OY_FX + work->head.oy;
	EOA_MatrixSet( eoa, &pos );
	
	FRO_OBJ_InitInMdl(
		&work->robj, &work->head.gym07->m_rmdl[GYM07MDL_BOX] );
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * EOA 箱　削除
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaGym07Box_Delete( EOA_PTR eoa, void *wk )
{
	GYM07BOX_EOA_WORK *work = wk;
	
	if( work->bk_anm != NULL ){
		int i;
		
		for( i = 0; i < BOXANM_MAX; i++ ){
			FRO_ANM_DeleteAll( &work->bk_anm->ranm[i] );
		}
		
		sys_FreeMemoryEz( work->bk_anm );
	}
}

//--------------------------------------------------------------
/**
 * EOA 箱　動作
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaGym07Box_Move( EOA_PTR eoa, void *wk )
{
	GYM07BOX_EOA_WORK *work = wk;
	
	switch( work->seq_no ){
	case SEQNO_BOX_NON:
		break;
	case SEQNO_BOX_ANM:
		{
			int i;
			GYM07BOX_ANM_WORK *anm;
			
			GF_ASSERT( work->bk_anm != NULL );	//未セット
			anm = work->bk_anm;
			
			for( i = 0; i < BOXANM_MAX; i++ ){
				if( FRO_ANM_Play(&anm->ranm[i],FX32_ONE,ANMLOOP_OFF) ){
					anm->anm_end |= 1 << i;
				}
			}
			
			if( anm->anm_end == BOXANMBIT_ALL ){
				work->seq_no = SEQNO_BOX_NON;
			}
		}
		break;
	}
}

//--------------------------------------------------------------
/**
 * EOA 箱　描画
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaGym07Box_Draw( EOA_PTR eoa, void *wk )
{
	VecFx32 pos;
	GYM07BOX_EOA_WORK *work = wk;
	EOA_MatrixGet( eoa, &pos );
	FRO_OBJ_DrawPos( &work->robj, &pos );
}

//--------------------------------------------------------------
///	EOA 箱 ヘッダ
//--------------------------------------------------------------
static const EOA_H_NPP DATA_EoaH_Gym07Box =
{
	sizeof(GYM07BOX_EOA_WORK),
	EoaGym07Box_Init,
	EoaGym07Box_Delete,
	EoaGym07Box_Move,
	EoaGym07Box_Draw,
};

//==============================================================================
//	eoa	サンドバッグ
//==============================================================================
//--------------------------------------------------------------
/**
 * サンドバッグを追加
 * @param	fes	FE_SYS
 * @param	 gx	グリッドX
 * @param	 gz	グリッドZ
 * @param	 oy Y座標オフセット
 * @retval	EOA_PTR	追加したEOA_PTR
 */
//--------------------------------------------------------------
EOA_PTR FE_Gym07Eff_SandbagAdd( FIELDSYS_WORK *fsys, int gx, int gz, fx32 oy )
{
	FE_SYS *fes;
	EOA_PTR *eoa_p;
	GYM07EFF_ADD_H head;
	
	fes = fsys->fes;
	eoa_p = EoaGym07EffAddWorkInit( fsys, gx, gz, oy, GYM07_BAG, &head );
	*eoa_p = FE_EoaAddNpp( fes, &DATA_EoaH_Gym07Bag, NULL, 0, &head, 0 );
	return( *eoa_p );
}

//--------------------------------------------------------------
/**
 * サンドバッグ　揺れアニメセット
 * @param	EOA_PTR	eoa
 * @param	dir	揺れる方向
 * @param	big	TRUE=大きく揺れる
 * @retval	nothing
 */
//--------------------------------------------------------------
void FE_Gym07Eff_SandbagShakeSet( EOA_PTR eoa, int dir, BOOL big )
{
	FRO_ANM *ranm;
	FE_GYM07EFF_PTR gym07;
	GYM07EOA_WORK *work;
	
	work = EOA_LocalWorkGet( eoa );
	gym07 = work->head.gym07;
	
	if( work->anm_flag == TRUE ){
		FRO_ANM_AnmObjFree( &work->ranm );
	}
	
	work->anm_flag = TRUE;
	work->anm_end = FALSE;
	work->seq_no = SEQNO_BAG_ANM;
	
	dir = FieldOBJTool_DirFlip( dir );
	
	if( big == TRUE ){
		dir += GYM07ANM_SBAG02_U;
	}else{
		dir += GYM07ANM_SBAG01_U;
	}
	
	FE_FROAnm_AnmResObjInitSame( gym07->fes, &work->ranm,
		&gym07->m_rmdl[GYM07MDL_SBAG], &gym07->m_ranm[dir], 0 );
	FRO_OBJ_InitAll(
		&work->robj, &gym07->m_rmdl[GYM07MDL_SBAG], &work->ranm );
}

//--------------------------------------------------------------
/**
 * サンドバッグ　アニメ速度セット
 * @param	EOA_PTR	eoa
 * @param	speed	速度
 * @retval	nothing
 */
//--------------------------------------------------------------
void FE_Gym07Eff_SandbagAnmSpeedSet( EOA_PTR eoa, fx32 speed )
{
	FRO_ANM *ranm;
	FE_GYM07EFF_PTR gym07;
	GYM07EOA_WORK *work;
	
	work = EOA_LocalWorkGet( eoa );
	work->anm_speed = speed;
}

//--------------------------------------------------------------
/**
 * サンドバッグ　アニメ終了チェック
 * @param	EOA_PTR	eoa
 * @retval	BOOL TRUE=終了
 */
//--------------------------------------------------------------
BOOL FE_Gym07Eff_SandbagAnmEndCheck( EOA_PTR eoa )
{
	GYM07EOA_WORK *work;
	work = EOA_LocalWorkGet( eoa );
	return( work->anm_end );
}

//--------------------------------------------------------------
/**
 * EOA サンドバッグ　初期化
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work *
 * @retval	int	TRUE=正常終了 FALSE=異常終了
 */
//--------------------------------------------------------------
static int EoaGym07Bag_Init( EOA_PTR eoa, void *wk )
{
	GYM07EOA_WORK *work = wk;
	EoaGym07WorkInit( eoa, work );
	
	FRO_OBJ_InitInMdl(
		&work->robj, &work->head.gym07->m_rmdl[GYM07MDL_SBAG] );
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * EOA サンドバッグ　削除
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaGym07Bag_Delete( EOA_PTR eoa, void *wk )
{
	GYM07EOA_WORK *work = wk;
	
	if( work->anm_flag ){
		FRO_ANM_DeleteAll( &work->ranm );
	}
}

//--------------------------------------------------------------
/**
 * EOA サンドバッグ　動作
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaGym07Bag_Move( EOA_PTR eoa, void *wk )
{
	GYM07EOA_WORK *work = wk;
	
	switch( work->seq_no ){
	case SEQNO_BAG_NON:
		break;
	case SEQNO_BAG_ANM:
		if( FRO_ANM_Play(&work->ranm,work->anm_speed,ANMLOOP_OFF) ){
			work->seq_no = SEQNO_BAG_NON;
			work->anm_end = TRUE;
		}
		
		break;
	}
}

//--------------------------------------------------------------
/**
 * EOA サンドバッグ　描画
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaGym07Bag_Draw( EOA_PTR eoa, void *wk )
{
	VecFx32 pos;
	GYM07EOA_WORK *work = wk;
	EOA_MatrixGet( eoa, &pos );
	FRO_OBJ_DrawPos( &work->robj, &pos );
}

//--------------------------------------------------------------
///	EOA サンドバッグ ヘッダ
//--------------------------------------------------------------
static const EOA_H_NPP DATA_EoaH_Gym07Bag =
{
	sizeof(GYM07EOA_WORK),
	EoaGym07Bag_Init,
	EoaGym07Bag_Delete,
	EoaGym07Bag_Move,
	EoaGym07Bag_Draw,
};

//==============================================================================
//	data
//==============================================================================
//--------------------------------------------------------------
///	モデルデータ　アーカイブインデックステーブル
//--------------------------------------------------------------
static const u32 DATA_Gym07MdlArcIdxTbl[GYM07MDL_MAX] =
{
	NARC_fldeff_gym07_woodbox_nsbmd,
	NARC_fldeff_gym07_woodbox0_nsbmd,
	NARC_fldeff_gym07_sandbag_nsbmd,
};

//--------------------------------------------------------------
///	アニメデータ　アーカイブインデックステーブル
//--------------------------------------------------------------
static const u32 DATA_Gym07AnmArcIdxTbl[GYM07ANM_MAX] =
{
	NARC_fldeff_sandbag01_u_nsbca,
	NARC_fldeff_sandbag01_d_nsbca,
	NARC_fldeff_sandbag01_l_nsbca,
	NARC_fldeff_sandbag01_r_nsbca,
	NARC_fldeff_sandbag02_u_nsbca,
	NARC_fldeff_sandbag02_d_nsbca,
	NARC_fldeff_sandbag02_l_nsbca,
	NARC_fldeff_sandbag02_r_nsbca,
	NARC_fldeff_gym07_woodbox_nsbca,
	NARC_fldeff_gym07_woodbox_nsbma,
	NARC_fldeff_gym07_woodbox_nsbta,
	NARC_fldeff_gym07_woodbox_nsbtp,
};

//--------------------------------------------------------------
///	箱アニメ種類->アニメインデックス
//--------------------------------------------------------------
static const u32 DATA_BoxAnmNo_ResNo[BOXANM_MAX] =
{
	GYM07ANM_BOX_BREAK_ICA,
	GYM07ANM_BOX_BREAK_IMA,
	GYM07ANM_BOX_BREAK_ITA,
	GYM07ANM_BOX_BREAK_ITP,
};

//******************************************************************************
/**
 * 
 * @file	fldeff_gym05eff.c
 * @brief	ゴーストジムエフェクト
 * @author	kagaya
 * @data	05.05.13
 *
 */
//******************************************************************************
#include "common.h"
#include "fieldsys.h"
#include "field_effect.h"
#include "fieldobj.h"

#include "gym.h"

//==============================================================================
//	define
//==============================================================================
#ifdef PM_DEBUG
//#define DEBUG_LIGHT_ON
#endif

//#define LIGHT_DRAW_ROTATE_OX (84+180)	//old 080221
//#define LIGHT_DRAW_ROTATE_OY (0+180)	//old 080221
#define LIGHT_DRAW_ROTATE_OX (0)
#define LIGHT_DRAW_ROTATE_OY (0)
#define LIGHT_DRAW_ROTATE_OZ (0)
#define LIGHT_DRAW_OX_FX (0)
#define LIGHT_DRAW_OY_FX (NUM_FX32(16))
#define LIGHT_DRAW_OZ_FX (NUM_FX32(0))

#define LIGHT_POS_SPEED (0x2000)
#define LIGHT_SCALE_SPEED (0x0080)

#define HINTP_OFFS_X (NUM_FX32(0))
#define HINTP_OFFS_Y (NUM_FX32(-3))
#define HINTP_OFFS_Z (NUM_FX32(-4))

//--------------------------------------------------------------
///	モデル種類
//--------------------------------------------------------------
enum
{
	GYM05MDL_LIGHT,
	GYM05MDL_HINT1,
	GYM05MDL_HINT2,
	GYM05MDL_HINT3,
	GYM05MDL_HINT4,
	GYM05MDL_HINT5,
	GYM05MDL_HINT6,
	GYM05MDL_HINT7,
	GYM05MDL_HINT8,
	GYM05MDL_MAX,
};

//--------------------------------------------------------------
///	アニメ種類
//--------------------------------------------------------------
enum
{
	GYM05ANM_LIGHT,
	GYM05ANM_MAX,
};

//--------------------------------------------------------------
///	ライト距離　グリッド単位
//--------------------------------------------------------------
enum
{
	LIGHT_RANGE_0,
	LIGHT_RANGE_1,
	LIGHT_RANGE_2,
	LIGHT_RANGE_3,
	LIGHT_RANGE_4,
	LIGHT_RANGE_5,
	LIGHT_RANGE_MAX,
};

//==============================================================================
//	typedef struct
//==============================================================================
//--------------------------------------------------------------
//	デバッグ
//--------------------------------------------------------------
#ifdef DEBUG_LIGHT_ON
typedef struct
{
	int mode;
}DEBUG_LIGHT_WORK;
#endif

//--------------------------------------------------------------
///	FE_GYM05EFF型
//--------------------------------------------------------------
typedef struct _TAG_FE_GYM05EFF * FE_GYM05EFF_PTR;

//--------------------------------------------------------------
///	FE_GYM05EFF構造体
//--------------------------------------------------------------
typedef struct _TAG_FE_GYM05EFF
{
	FE_SYS *fes;
	FRO_MDL m_rmdl[GYM05MDL_MAX];
	FRO_ANM m_ranm[GYM05ANM_MAX];
	
#ifdef DEBUG_LIGHT_ON
	DEBUG_LIGHT_WORK debug;
#endif
}FE_GYM05EFF;

#define FE_GYM05EFF_SIZE (sizeof(FE_GYM05EFF)) ///<FE_GYM05EFFサイズ

//--------------------------------------------------------------
///	EOALIGHT_HEAD
//--------------------------------------------------------------
typedef struct
{
	int range;
	FE_SYS *fes;
	FE_GYM05EFF *gym05;
	CONST_FIELD_OBJ_PTR fldobj;
}EOALIGHT_HEAD;

//--------------------------------------------------------------
///	EOA_LIGHT_WORK
//--------------------------------------------------------------
typedef struct
{
	s8 dir;
	s8 range;
	u8 vanish;
	u8 dmy;
	VecFx32 offs;
	VecFx32 scale;
	ROTATE rotate;
	EOALIGHT_HEAD head;
	FRO_OBJ robj;
	FRO_ANM *ranm_p;
	
#ifdef DEBUG_LIGHT_ON	
	ROTATE d_rot;
#endif
}EOA_LIGHT_WORK;

//--------------------------------------------------------------
///	EOA_HINTP_WORK
//--------------------------------------------------------------
typedef struct
{
	int no;
	FE_SYS *fes;
	FE_GYM05EFF_PTR gym05;
	FRO_OBJ robj;
}EOA_HINTP_WORK;

//==============================================================================
//	プロトタイプ
//==============================================================================
static void Gym05Eff_GraphicInit( FE_GYM05EFF_PTR gym05eff );
static void Gym05Eff_GraphicDelete( FE_GYM05EFF_PTR gym05eff );

static const EOA_H_NPP DATA_EoaH_Gym05Light;

static void LightDrawPosSet( EOA_PTR eoa, EOA_LIGHT_WORK *work );
static void LightDrawStateSet( EOA_PTR eoa, EOA_LIGHT_WORK *work );
static void AngleAdd( u16 *angle, s16 add );
static int LightRangeHitCheck(
	CONST_FIELD_OBJ_PTR fldobj, int range, VecFx32 *hit_pos );
static void LightHitScaleOffsSet(
		int dir, int max_range,
		const VecFx32 *hit_pos, const VecFx32 *pos,
		VecFx32 *light_offs, VecFx32 *light_scale );

static const EOA_H_NPP DATA_EoaH_HintPanel;

static const u32 DATA_Gym05MdlArcIdxTbl[GYM05MDL_MAX];
static const u32 DATA_Gym05AnmArcIdxTbl[GYM05ANM_MAX];
static const ROTATE DATA_LightDirRotateTbl[DIR_4_MAX];
static const VecFx32 DATA_LightDirRangeScaleTbl[DIR_4_MAX][LIGHT_RANGE_MAX];
static const VecFx32 DATA_LightDirPosTbl[DIR_4_MAX][LIGHT_RANGE_MAX];

#ifdef DEBUG_LIGHT_ON
static void DEBUG_Light( EOA_PTR eoa, EOA_LIGHT_WORK *work );
#endif

//==============================================================================
//	ゴーストジムエフェクト　システム
//==============================================================================
//--------------------------------------------------------------
/**
 * ゴーストジムエフェクト初期化
 * @param	fes		FE_SYS_PTR
 * @retval	FE_GYM05EFF_PTR	FE_GYM05EFF_PTR
 */
//--------------------------------------------------------------
void * FE_Gym05Eff_Init( FE_SYS *fes )
{
	FE_GYM05EFF_PTR gym05eff;
	
	gym05eff = FE_AllocClearMemory( fes, FE_GYM05EFF_SIZE, ALLOC_FR, 0 );
	gym05eff->fes = fes;
	
	Gym05Eff_GraphicInit( gym05eff );
	return( gym05eff );
}

//--------------------------------------------------------------
/**
 * ゴーストジムエフェクト削除
 * @param	gym05eff		FE_GYM05EFF_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FE_Gym05Eff_Delete( void *work )
{
	FE_GYM05EFF_PTR gym05eff = work;
	
	Gym05Eff_GraphicDelete( gym05eff );
	FE_FreeMemory( gym05eff );
}

//==============================================================================
//	ゴーストジムエフェクト　グラフィック
//==============================================================================
//--------------------------------------------------------------
/**
 * ゴーストジムエフェクト グラフィック初期化
 * @param	gym05eff	FE_GYM05EFF_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Gym05Eff_GraphicInit( FE_GYM05EFF_PTR gym05 )
{
	int i;
	
	for( i = 0; i < GYM05MDL_MAX; i++ ){
		FE_FROMdl_ResSetArcLoadTexTrans( 
			gym05->fes, &gym05->m_rmdl[i], 0,
			DATA_Gym05MdlArcIdxTbl[i], ALLOC_FR );
	}
	
	NNS_G3dMdlSetMdlFogEnableFlagAll(
		gym05->m_rmdl[GYM05MDL_LIGHT].pResMdl, 0 );
	
	for( i = 0; i < GYM05ANM_MAX; i++ ){
		FE_FROAnm_AnmResSetArcLoad( gym05->fes, &gym05->m_ranm[i], 0,
			DATA_Gym05AnmArcIdxTbl[i], ALLOC_FR );
	}
}

//--------------------------------------------------------------
/**
 * ゴーストジムエフェクト グラフィック削除
 * @param	gym05eff	FE_GYM05EFF_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Gym05Eff_GraphicDelete( FE_GYM05EFF_PTR gym05 )
{
	int i;
	
	for( i = 0; i < GYM05MDL_MAX; i++ ){
		FRO_MDL_DeleteAll( &gym05->m_rmdl[i] );
	}
	
	for( i = 0; i < GYM05ANM_MAX; i++ ){
		FRO_ANM_DeleteAll( &gym05->m_ranm[i] );
	}
}

//==============================================================================
//	eoa	ライト
//==============================================================================
//--------------------------------------------------------------
/**
 * ライトを追加
 * @param	fes	FE_SYS
 * @param	fldobj	ライトを持たせるFIELD_OBJ_PTR
 * @param	range	ライト距離　グリッド単位
 * @retval	EOA_PTR	追加したEOA_PTR
 */
//--------------------------------------------------------------
EOA_PTR FE_Gym05Eff_LightAdd(
		FE_SYS *fes, CONST_FIELD_OBJ_PTR fldobj, int range, int tcb_offs )
{
	int pri;
	EOA_PTR eoa;
	EOALIGHT_HEAD head;
	
	head.range = range;
	head.fes = fes;
	head.gym05 = FE_EffectWorkGet( fes, FE_FLD_GYM05EFF );
	head.fldobj = fldobj;
	
	pri = FieldOBJ_TCBPriGet( fldobj, tcb_offs );
	eoa = FE_EoaAddNpp( fes, &DATA_EoaH_Gym05Light, NULL, 0, &head, pri );
	return( eoa );
}

//--------------------------------------------------------------
/**
 * ライト距離を設定
 * @param	eoa		ライトEOA
 * @param	range	ライト距離
 * @retval	nothing
 */
//--------------------------------------------------------------
void FE_Gym05Eff_LightRangeSet( EOA_PTR eoa, int range )
{
	EOA_LIGHT_WORK *work = EOA_LocalWorkGet( eoa );
	work->head.range = range;
}

//--------------------------------------------------------------
/**
 * ライト　初期化
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work *
 * @retval	int TRUE=正常終了 FALSE=異常終了
 */
//--------------------------------------------------------------
static int EoaGym05Light_Init( EOA_PTR eoa, void *wk )
{
	EOA_LIGHT_WORK *work = wk;
	const EOALIGHT_HEAD *head = EOA_AddPtrGet( eoa );
	
	work->head = *head;
	work->dir = FieldOBJ_DirDispGet( head->fldobj );
	
	work->ranm_p = FE_AllocMemory( head->fes, sizeof(FRO_ANM), ALLOC_FR );
	
	FRO_OBJ_InitInMdl(
		&work->robj, &work->head.gym05->m_rmdl[GYM05MDL_LIGHT] );
	FE_FROAnm_AnmResObjInitSame( work->head.fes, work->ranm_p,
			&work->head.gym05->m_rmdl[GYM05MDL_LIGHT],
			&work->head.gym05->m_ranm[GYM05ANM_LIGHT], 0 );
	FRO_OBJ_AddAnmInAnm( &work->robj, work->ranm_p );
	LightDrawStateSet( eoa, work );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * ライト　削除
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaGym05Light_Delete( EOA_PTR eoa, void *wk )
{
	EOA_LIGHT_WORK *work = wk;
	FRO_ANM_AnmObjFree( work->ranm_p );
	sys_FreeMemoryEz( work->ranm_p );
}

//--------------------------------------------------------------
/**
 * ライト　動作
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaGym05Light_Move( EOA_PTR eoa, void *wk )
{
	int range;
	VecFx32 hit_pos,obj_pos;
	const VecFx32 *dat;
	EOA_LIGHT_WORK *work = wk;
	CONST_FIELD_OBJ_PTR fldobj = work->head.fldobj;
	int dir = FieldOBJ_DirDispGet( fldobj );
	
	FieldOBJ_VecPosGet( fldobj, &obj_pos );
	range = LightRangeHitCheck( fldobj, work->head.range, &hit_pos );
	
	#ifdef DEBUG_LIGHT_ON
	if( dir != work->dir ){
		work->dir = dir;
		work->range = range;
		LightDrawStateSet( eoa, work );
	}
	#endif
	
	work->dir = dir;
	work->range = range;
	
	#ifdef DEBUG_LIGHT_ON
	if( FieldOBJ_OBJIDGet(fldobj) == FLDOBJ_ID_PLAYER ){
		DEBUG_Light( eoa, work );
		return;
	}
	#endif
	
	if( work->range == work->head.range ){	//制限無し
		LightDrawStateSet( eoa, work );
	}else{
		LightHitScaleOffsSet( work->dir, work->head.range,
			&hit_pos, &obj_pos, &work->offs, &work->scale );
		LightDrawPosSet( eoa, work );
	}
	
	FRO_ANM_Play( work->ranm_p, FX32_ONE, ANMLOOP_ON );
}

//--------------------------------------------------------------
/**
 * ライト　描画
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaGym05Light_Draw( EOA_PTR eoa, void *wk )
{
EOA_LIGHT_WORK *work = wk;
	
	if( work->vanish == FALSE ){
		ROTATE rot;
		VecFx32 pos;
		EOA_MatrixGet( eoa, &pos );
		pos.x += LIGHT_DRAW_OX_FX;
		pos.y += LIGHT_DRAW_OY_FX;
		pos.z += LIGHT_DRAW_OZ_FX;
		rot = work->rotate;
		AngleAdd( &rot.x, LIGHT_DRAW_ROTATE_OX );
		AngleAdd( &rot.y, LIGHT_DRAW_ROTATE_OY );
		AngleAdd( &rot.z, LIGHT_DRAW_ROTATE_OZ );
		#ifdef DEBUG_LIGHT_ON
		AngleAdd( &rot.x, work->d_rot.x );
		AngleAdd( &rot.y, work->d_rot.y );
		AngleAdd( &rot.z, work->d_rot.z );
		#endif
		FRO_OBJ_DrawScaleRotate( &work->robj, &pos, &work->scale, &rot );
	}
}

//--------------------------------------------------------------
///	EOA	ライト　ヘッダ
//--------------------------------------------------------------
static const EOA_H_NPP DATA_EoaH_Gym05Light =
{
	sizeof(EOA_LIGHT_WORK),
	EoaGym05Light_Init,
	EoaGym05Light_Delete,
	EoaGym05Light_Move,
	EoaGym05Light_Draw,
};

//==============================================================================
//	ライトパーツ
//==============================================================================
//--------------------------------------------------------------
/**
 * ライト 座標セット
 * @param	eoa		EOA_PTR
 * @param	work	EOA_LIGHT_WORK *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void LightDrawPosSet( EOA_PTR eoa, EOA_LIGHT_WORK *work )
{
	VecFx32 pos;
	CONST_FIELD_OBJ_PTR fldobj = work->head.fldobj;
	
	work->rotate = DATA_LightDirRotateTbl[work->dir];
	
	FieldOBJ_VecPosGet( work->head.fldobj, &pos );
	pos.x += work->offs.x;
	pos.y += work->offs.y; 
	pos.z += work->offs.z;
	EOA_MatrixSet( eoa, &pos );
}

//--------------------------------------------------------------
/**
 * ライト角度、拡縮率、座標セット
 * @param	eoa		EOA_PTR
 * @param	work	EOA_LIGHT_WORK *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void LightDrawStateSet( EOA_PTR eoa, EOA_LIGHT_WORK *work )
{
	VecFx32 pos;
	CONST_FIELD_OBJ_PTR fldobj = work->head.fldobj;
	
	work->scale = DATA_LightDirRangeScaleTbl[work->dir][work->range];
	work->offs = DATA_LightDirPosTbl[work->dir][work->range];
	LightDrawPosSet( eoa, work );
}

//--------------------------------------------------------------
/**
 * 角度増減
 * @param	angle	角度
 * @param	add		増減値
 * @retval	nothing
 */
//--------------------------------------------------------------
static void AngleAdd( u16 *angle, s16 add )
{
	(*angle) += add;
	if( (s16)(*angle) < 0 ){ do{(*angle)+=360;}while((s16)(*angle)<0); }
	else{ (*angle) %= 360; }
}

//--------------------------------------------------------------
/**
 * ライトあたり判定　移動制限は無視する
 * @param	fldobj	対象FIELD_OBJ_PTR
 * @param	range	ライトレンジ最大　グリッド単位
 * @retval	nothing
 */
//--------------------------------------------------------------
static int LightRangeHitCheck( CONST_FIELD_OBJ_PTR fldobj, int range, VecFx32 *hit_pos )
{
	u32 hit;
	VecFx32 pos;
	int grid,dir,x,y,z;
	
	GF_ASSERT( range );
	
	x = FieldOBJ_NowPosGX_Get( fldobj );
	y = FieldOBJ_NowPosGY_Get( fldobj );
	z = FieldOBJ_NowPosGZ_Get( fldobj );
	dir = FieldOBJ_DirDispGet( fldobj );
	FieldOBJ_VecPosGet( fldobj, &pos );
	
	for( grid = 0; grid < range; grid++ ){
		x += FieldOBJ_DirAddValueGX( dir );
		z += FieldOBJ_DirAddValueGZ( dir );
		hit = FieldOBJ_MoveHitCheck( fldobj, &pos, x, y, z, dir );
		
		if( hit != FLDOBJ_MOVE_HIT_BIT_NON &&	//当たり有り
			hit != FLDOBJ_MOVE_HIT_BIT_LIM ){	//移動制限ヒットではない
			if( hit == FLDOBJ_MOVE_HIT_BIT_OBJ ){	//OBJヒットである場合
				FIELD_OBJ_PTR hitobj;
				hitobj = FieldOBJ_SearchGridPos(
					FieldOBJ_FieldOBJSysGet(fldobj), x, z, TRUE );
				GF_ASSERT( hitobj != NULL );
				FieldOBJ_VecPosGet( hitobj, hit_pos );
			}else{
				FieldOBJTool_GridCenterPosGet( x, z, hit_pos );
			}
			
			return( grid );
		}
	}
	
	FieldOBJTool_GridCenterPosGet( x, z, hit_pos );
	return( grid );
}

//--------------------------------------------------------------
/**
 * 障害物までのライト座標、拡縮率を求める
 * @param	dir	ライト方向 DIR_UP等
 * @param	max_range	ライト最大範囲 グリッド単位
 * @param	hit_pos	障害物の座標
 * @param	pos	ライト座標
 * @param	light_offs	ライト座標オフセット格納先
 * @param	light_scale	ライト拡縮率格納先
 * @retval	nothing
 */
//--------------------------------------------------------------
static void LightHitScaleOffsSet(
		int dir, int max_range,
		const VecFx32 *hit_pos, const VecFx32 *pos,
		VecFx32 *light_offs, VecFx32 *light_scale )
{
	fx32 sabun;
	int j_range,h_range,s;
	const VecFx32 *h_scale,*h_offs;
	
	switch( dir ){
	case DIR_UP:	sabun = (pos->z - hit_pos->z); break; 
	case DIR_DOWN:	sabun = (hit_pos->z - pos->z); break;
	case DIR_LEFT:	sabun = (pos->x - hit_pos->x); break;
	case DIR_RIGHT:	sabun = (hit_pos->x - pos->x); break;
	}
	
	j_range = sabun / GRID_FX32;	//差分をグリッド単位にし現時点の範囲に
	
	if( j_range < 0 ){
		j_range = -j_range;
	}
	
	if( j_range > max_range ){
		j_range = max_range;
	}
	
	j_range--;						//-1=自身の位置
	
	if( j_range < 0 ){
		j_range = 0;
	}
	
	h_range = j_range;
	
	sabun %= GRID_FX32;
	
	if( sabun < 0 ){				//差分による目標設定
		h_range--;
	}else if( sabun > 0 ){
		h_range++;
	}
	
	if( h_range < 0 ){
		h_range = 0;
	}else if( h_range > max_range ){
		h_range = max_range;
	}
	
	s = sabun / (GRID_FX32/10);		// 差分を%化
	
	if( s < 0 ){
		s = -s;
	}
	
	h_offs = &DATA_LightDirPosTbl[dir][h_range];
	*light_offs = DATA_LightDirPosTbl[dir][j_range];
	h_scale = &DATA_LightDirRangeScaleTbl[dir][h_range];
	*light_scale = DATA_LightDirRangeScaleTbl[dir][j_range];
	
	light_offs->x += ((h_offs->x - light_offs->x) / 10) * s;
	light_offs->y += ((h_offs->y - light_offs->y) / 10) * s;
	light_offs->z += ((h_offs->z - light_offs->z) / 10) * s;
	light_scale->x += ((h_scale->x - light_scale->x) / 10) * s;
	light_scale->y += ((h_scale->y - light_scale->y) / 10) * s;
	light_scale->z += ((h_scale->z - light_scale->z) / 10) * s;
}

//==============================================================================
//	ヒントパネル
//==============================================================================
//--------------------------------------------------------------
/**
 * ヒントパネル配置
 * @param	fea	FE_SYS
 * @param	gx	配置するグリッドGX
 * @param	gz	配置するグリッドGZ
 * @param	no	ヒント番号0-7
 * @retval	EOA_PTR	ヒントパネルEOA_PTR
 */
//--------------------------------------------------------------
EOA_PTR FE_Gym05Eff_HintPanelAdd( FE_SYS *fes, int gx, int gz, int no )
{
	EOA_PTR eoa;
	VecFx32 pos = {0,0,0};
	
	GF_ASSERT( no < GYM_PLGHOST_HINT_MAX );
	
	FieldOBJTool_GridCenterPosGet( gx, gz, &pos );
	FieldOBJTool_GetHeight( FE_FieldSysWorkGet(fes), &pos );
	
	pos.x += HINTP_OFFS_X;
	pos.y += HINTP_OFFS_Y;
	pos.z += HINTP_OFFS_Z;
	
	eoa = FE_EoaAddNpp( fes, &DATA_EoaH_HintPanel, &pos, no, fes, 0 );
	return( eoa );
}

//--------------------------------------------------------------
/**
 * EOA ヒントパネル　初期化
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	int		TRUE=正常終了。FALSE=異常終了
 */
//--------------------------------------------------------------
static int EoaHintPanel_Init( EOA_PTR eoa, void *wk )
{
	EOA_HINTP_WORK *work = wk;
	
	work->no = EOA_AddParamGet( eoa );
	work->fes = (FE_SYS *)EOA_AddPtrGet( eoa );
	work->gym05 = FE_EffectWorkGet( work->fes, FE_FLD_GYM05EFF );
	
	FRO_OBJ_InitInMdl(
		&work->robj, &work->gym05->m_rmdl[GYM05MDL_HINT1+work->no] );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * EOA ヒントパネル　削除
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaHintPanel_Delete( EOA_PTR eoa, void *wk )
{
//	EOA_HINTP_WORK *work = wk;
}

//--------------------------------------------------------------
/**
 * EOA ヒントパネル　動作
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaHintPanel_Move( EOA_PTR eoa, void *wk )
{
//	EOA_HINTP_WORK *work = wk;
}

//--------------------------------------------------------------
/**
 * EOA ヒントパネル　描画
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaHintPanel_Draw( EOA_PTR eoa, void *wk )
{
	VecFx32 pos;
	EOA_HINTP_WORK *work = wk;
	EOA_MatrixGet( eoa, &pos );
	FRO_OBJ_DrawPos( &work->robj, &pos );
}

//--------------------------------------------------------------
///	EOA ヒントパネル　ヘッダー
//--------------------------------------------------------------
static const EOA_H_NPP DATA_EoaH_HintPanel =
{
	sizeof(EOA_HINTP_WORK),
	EoaHintPanel_Init,
	EoaHintPanel_Delete,
	EoaHintPanel_Move,
	EoaHintPanel_Draw,
};

//==============================================================================
//	data
//==============================================================================
//--------------------------------------------------------------
///	モデルデータ　アーカイブインデックステーブル
//--------------------------------------------------------------
static const u32 DATA_Gym05MdlArcIdxTbl[GYM05MDL_MAX] =
{
	NARC_fldeff_gym_light1_nsbmd,
	NARC_fldeff_gym05_hint01_nsbmd,
	NARC_fldeff_gym05_hint02_nsbmd,
	NARC_fldeff_gym05_hint03_nsbmd,
	NARC_fldeff_gym05_hint04_nsbmd,
	NARC_fldeff_gym05_hint05_nsbmd,
	NARC_fldeff_gym05_hint06_nsbmd,
	NARC_fldeff_gym05_hint07_nsbmd,
	NARC_fldeff_gym05_hint08_nsbmd,
};

//--------------------------------------------------------------
///	アニメデータ　アーカイブインデックステーブル
//--------------------------------------------------------------
static const u32 DATA_Gym05AnmArcIdxTbl[GYM05ANM_MAX] =
{
	NARC_fldeff_gym_light1_nsbta,
};

//--------------------------------------------------------------
///	ライト　方向別回転角度
//--------------------------------------------------------------
static const ROTATE DATA_LightDirRotateTbl[DIR_4_MAX] =
{
	{270,180,0},
	{90,180,180},
	{0,270,270},
	{0,90,90},
};

//old 080221
#if 0
static const ROTATE DATA_LightDirRotateTbl[DIR_4_MAX] =
{
	{0,0,0},
	{0,180,0},
	{0,90,0},
	{0,270,0},
};
#endif

//--------------------------------------------------------------
///	ライト　方向、距離別拡縮率
//--------------------------------------------------------------
static const VecFx32 DATA_LightDirRangeScaleTbl[DIR_4_MAX][LIGHT_RANGE_MAX] =
{
	{
		{0x0c00,0x0100,0x1000},
		{0x1000,0x0500,0x1000},	
		{0x1000,0x0800,0x1000},	//2
		{0x1000,0x0c00,0x1000},	//3
		{0x1000,0x1000,0x1000},	//4
		{0x1000,0x1400,0x1000},
	},
	{
		{0x0c00,0x0100,0x1000},
		{0x1000,0x0500,0x1000},	
		{0x1000,0x0800,0x1000},	//2
		{0x1000,0x0c00,0x1000},	//3
		{0x1000,0x1000,0x1000},	//4
		{0x1000,0x1400,0x1000},
	},
	{
		{0x1000,0x0200,0x1000},
		{0x1000,0x0600,0x1000},
		{0x1000,0x0a00,0x1000},
		{0x1000,0x0e00,0x1000},
		{0x1000,0x1200,0x1000},
		{0x1000,0x1400,0x1000},
	},
	{
		{0x1000,0x0200,0x1000},
		{0x1000,0x0600,0x1000},
		{0x1000,0x0a00,0x1000},
		{0x1000,0x0e00,0x1000},
		{0x1000,0x1200,0x1000},
		{0x1000,0x1400,0x1000},
	},
};

//--------------------------------------------------------------
///	ライト　方向、距離別表示位置
//--------------------------------------------------------------
static const VecFx32 DATA_LightDirPosTbl[DIR_4_MAX][LIGHT_RANGE_MAX] =
{
	{
		{NUM_FX32(0),NUM_FX32(-15),NUM_FX32(-15)},
		{NUM_FX32(0),NUM_FX32(-15),NUM_FX32(-31)},
		{NUM_FX32(0),NUM_FX32(-15),NUM_FX32(-47)},
		{NUM_FX32(0),NUM_FX32(-15),NUM_FX32(-63)},
		{NUM_FX32(0),NUM_FX32(-15),NUM_FX32(-79)},
		{NUM_FX32(0),NUM_FX32(-15),NUM_FX32(-95)},
	},
	{
		{NUM_FX32(0),NUM_FX32(0),NUM_FX32(13)},
		{NUM_FX32(0),NUM_FX32(0),NUM_FX32(27)},
		{NUM_FX32(0),NUM_FX32(0),NUM_FX32(44)},
		{NUM_FX32(0),NUM_FX32(0),NUM_FX32(60)},
		{NUM_FX32(0),NUM_FX32(0),NUM_FX32(78)},
		{NUM_FX32(0),NUM_FX32(0),NUM_FX32(93)},
	},
	{
		{NUM_FX32(-13),NUM_FX32(0),NUM_FX32(6)},	//o
		{NUM_FX32(-29),NUM_FX32(0),NUM_FX32(9)},	//o
		{NUM_FX32(-45),NUM_FX32(0),NUM_FX32(10)},	//o
		{NUM_FX32(-61),NUM_FX32(0),NUM_FX32(10)},	//o
		{NUM_FX32(-78),NUM_FX32(0),NUM_FX32(10)},	//o
		{NUM_FX32(-85),NUM_FX32(0),NUM_FX32(10)},
	},
	{
		{NUM_FX32(13),NUM_FX32(0),NUM_FX32(6)},	//o
		{NUM_FX32(29),NUM_FX32(0),NUM_FX32(9)},	//o
		{NUM_FX32(45),NUM_FX32(0),NUM_FX32(10)},	//o
		{NUM_FX32(61),NUM_FX32(0),NUM_FX32(10)},	//o
		{NUM_FX32(78),NUM_FX32(0),NUM_FX32(10)},	//o
		{NUM_FX32(85),NUM_FX32(0),NUM_FX32(10)},
	},
};

//==============================================================================
//	debug
//==============================================================================
#ifdef DEBUG_PLGHOSTGYM_CAPTURE
void DEBUG_FE_Gym05Eff_LightVanish( EOA_PTR eoa )
{
	EOA_LIGHT_WORK *work = EOA_LocalWorkGet( eoa );
	work->vanish ^= 1;
}
#endif

#ifdef DEBUG_LIGHT_ON

#define DEBUG_SCALE_SPEED (0x0100)

static void DEBUG_Light( EOA_PTR eoa, EOA_LIGHT_WORK *work )
{
	u32 trg = sys.trg;
	u32 cnt = sys.cont;
	FE_GYM05EFF_PTR gym05 = work->head.gym05;
	DEBUG_LIGHT_WORK *debug = &gym05->debug;
	
#if 0	
	if( cnt & PAD_BUTTON_L ){
		if( trg & PAD_BUTTON_B ){
			debug->mode++;
			debug->mode &= 0x01;
			if( debug->mode )OS_Printf( "モード　拡縮\n" );
			else OS_Printf( "モード 座標\n" );
		}
		
		if( debug->mode == 0 ){
			if( cnt & PAD_BUTTON_Y ){
				if( cnt & PAD_KEY_UP ){
					work->offs.y += NUM_FX32( 1 );
				}else if( cnt & PAD_KEY_DOWN ){
					work->offs.y -= NUM_FX32( 1 );
				}
			}else if( cnt & PAD_KEY_UP ){
				work->offs.z -= NUM_FX32( 1 );
			}else if( cnt & PAD_KEY_DOWN ){
				work->offs.z += NUM_FX32( 1 );
			}else if( cnt & PAD_KEY_LEFT ){
				work->offs.x -= NUM_FX32( 1 );
			}else if( cnt & PAD_KEY_RIGHT ){
				work->offs.x += NUM_FX32( 1 );
			}
			
			if( trg & PAD_BUTTON_START ){
				OS_Printf( "ライト座標　x=%d,y=%d,z=%d\n",
					FX32_NUM(work->offs.x),
					FX32_NUM(work->offs.y),
					FX32_NUM(work->offs.z) );
			}
		}else{
			if( cnt & PAD_BUTTON_Y ){
				if( trg & PAD_KEY_UP ){
					work->scale.z -= DEBUG_SCALE_SPEED;
				}else if( trg & PAD_KEY_DOWN ){
					work->scale.z += DEBUG_SCALE_SPEED;
				}
			}else{
				if( trg & PAD_KEY_UP ){
					work->scale.y -= DEBUG_SCALE_SPEED;
				}else if( trg & PAD_KEY_DOWN ){
					work->scale.y += DEBUG_SCALE_SPEED;
				}else if( trg & PAD_KEY_LEFT ){
					work->scale.x -= DEBUG_SCALE_SPEED;
				}else if( trg & PAD_KEY_RIGHT ){
					work->scale.x += DEBUG_SCALE_SPEED;
				}
			}
			
			if( trg & PAD_BUTTON_START ){
				OS_Printf( "ライト拡縮率 " );
				OS_Printf( "X = 0x%x ", work->scale.x );
				OS_Printf( "Y = 0x%x ", work->scale.y );
				OS_Printf( "Z = 0x%x\n", work->scale.z );
			}
		}
	}
#else
	if( (trg & PAD_BUTTON_B) ){
		debug->mode++;
		debug->mode %= 3;
		switch( debug->mode ){
		case 0: OS_Printf( "Mode RotateX\n" ); break;
		case 1: OS_Printf( "Mode RotateY\n" ); break;
		case 2: OS_Printf( "Mode RotateZ\n" ); break;
		}
	}
	
	if( (cnt & PAD_BUTTON_A) ){
		if( (cnt & PAD_BUTTON_L) ){
			switch( debug->mode ){
			case 0: AngleAdd( &work->d_rot.x, -2 ); break;
			case 1: AngleAdd( &work->d_rot.y, -2 ); break;
			case 2: AngleAdd( &work->d_rot.z, -2 ); break;
			}
		}else if( (cnt & PAD_BUTTON_R) ){
			switch( debug->mode ){
			case 0: AngleAdd( &work->d_rot.x, 2 ); break;
			case 1: AngleAdd( &work->d_rot.y, 2 ); break;
			case 2: AngleAdd( &work->d_rot.z, 2 ); break;
			}
		}
	}
	
	if( trg & PAD_BUTTON_START ){
		OS_Printf( "Rotate X=%d,Y=%d,Z=%d\n",
			work->d_rot.x, work->d_rot.y, work->d_rot.z );
	}
#endif
	
	LightDrawPosSet( eoa, work );
}
#endif

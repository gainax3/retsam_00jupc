//******************************************************************************
/**
 * @file	fldeff_fldrobj.c
 * @brief	フィールドエフェクト　フィールドレンダリングOBJ表示
 * @author	kagaya
 * @data	05.07.13
 */
//******************************************************************************
#include "common.h"
#include "fieldsys.h"
#include "field_effect.h"
#include "fieldobj.h"

#include "fldeff_fldrobj.h"

//==============================================================================
//	define
//==============================================================================

//==============================================================================
//	typedef struct
//==============================================================================
//--------------------------------------------------------------
///	FRO_CODE
//--------------------------------------------------------------
typedef struct
{
	u32 code;
	FRO_MDL rmdl;
	FRO_OBJ robj;
}MDLDATA;

//--------------------------------------------------------------
///	FE_FLDROBJ
//--------------------------------------------------------------
typedef struct
{
	FE_SYS *fes;			///<FE_SYS
	u32 obj_max;			///<OBJ最大数
	MDLDATA *mdldata;		///<コード別レンダ格納
}FE_FLDROBJ;

//--------------------------------------------------------------
///	FLDROBJ_H
//--------------------------------------------------------------
typedef struct
{
	u32 code;
	FE_SYS *fes;
	CONST_FIELD_OBJ_PTR fldobj;
}FLDROBJ_H;

//--------------------------------------------------------------
///	EOA_FLDROBJ_WORK
//--------------------------------------------------------------
typedef struct
{
	int seq_no;
	int obj_id;
	int zone_id;
	VecFx32 offs;
	FRO_OBJ *robj;
	FLDROBJ_H head;
}EOA_FLDROBJ_WORK;

//--------------------------------------------------------------
///	CODEARCIDX
//--------------------------------------------------------------
typedef struct
{
	u32 code;
	u32 arc_idx;
}CODEARCIDX;

//==============================================================================
//	プロトタイプ
//==============================================================================
static void MdlDataInit( MDLDATA *mdat, u32 code, u32 arc_idx, FE_SYS *fes );
static void MdlDataDelete( MDLDATA *mdat );
static void MdlDataDeleteAll( FE_FLDROBJ *frobj );
static MDLDATA * MdlDataSearch( FE_FLDROBJ *frobj, u32 code );

static const EOA_H_NPP DATA_EoaH_FldRObj;

const CODEARCIDX DATA_FieldOBJRenderOBJArcIdx[];

//==============================================================================
//	data
//==============================================================================
//--------------------------------------------------------------
///	レンダOBJ使用表示コード
//--------------------------------------------------------------
static const CODEARCIDX DATA_FieldOBJRenderOBJArcIdx[] =
{
	{ BOARD_A, NARC_fldeff_board_a_nsbmd },
	{ BOARD_B, NARC_fldeff_board_b_nsbmd },
	{ BOARD_C, NARC_fldeff_board_c_nsbmd },
	{ BOARD_D, NARC_fldeff_board_d_nsbmd },
	{ BOARD_E, NARC_fldeff_board_e_nsbmd },
	{ BOARD_F, NARC_fldeff_board_f_nsbmd },
	{ BOOK, NARC_fldeff_book_nsbmd },
	{ DOOR2, NARC_fldeff_door2_nsbmd },
	{ ROTOMWALL, NARC_fldeff_rotomwall_nsbmd },
};

//==============================================================================
//	FE_FLDROBJ　システム
//==============================================================================
//--------------------------------------------------------------
/**
 * フィールドレンダリングOBJ初期化
 * @param	fes		FE_SYS_PTR
 * @retval	FE_FLDROBJ
 */
//--------------------------------------------------------------
void * FE_FldROBJ_Init( FE_SYS *fes )
{
	FE_FLDROBJ *frobj;
	frobj = FE_AllocClearMemory( fes, sizeof(FE_FLDROBJ), ALLOC_FR, 0 );
	frobj->fes = fes;
	return( frobj );
}

//--------------------------------------------------------------
/**
 * フィールドレンダリングOBJ削除
 * @param	work	FE_FLDROBJ
 * @retval	nothing
 */
//--------------------------------------------------------------
void FE_FldROBJ_Delete( void *work )
{
	FE_FLDROBJ *frobj = work;
	MdlDataDeleteAll( frobj );
	if( frobj->mdldata != NULL ){ FE_FreeMemory(frobj->mdldata); }
	FE_FreeMemory( frobj );
}

//==============================================================================
//	レンダOBJ　グラフィック
//==============================================================================
//--------------------------------------------------------------
/**
 * モデルセット
 * @param	mdat	MDLDATA
 * @param	arc_idx	アーカイブインデックス
 * @retval
 */
//--------------------------------------------------------------
static void MdlDataInit( MDLDATA *mdat, u32 code, u32 arc_idx, FE_SYS *fes )
{
	mdat->code = code;
	FE_FROMdl_ResSetArcLoadTexTrans( fes, &mdat->rmdl, 0, arc_idx, ALLOC_FR );
	FRO_OBJ_InitInMdl( &mdat->robj, &mdat->rmdl );
}

//--------------------------------------------------------------
/**
 * モデル削除
 * @param	mdat	MDLDATA
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MdlDataDelete( MDLDATA *mdat )
{
	mdat->code = OBJCODEMAX;
	FRO_MDL_DeleteAll( &mdat->rmdl );
}

//--------------------------------------------------------------
/**
 * モデル全削除
 * @param	frobj	FE_FLDROBJ
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MdlDataDeleteAll( FE_FLDROBJ *frobj )
{
	u32 i = 0, max = frobj->obj_max;
	MDLDATA *mdata = frobj->mdldata;
	while(i<max){if(mdata->code!=OBJCODEMAX){MdlDataDelete(mdata);}mdata++;i++;}
}

//--------------------------------------------------------------
/**
 * MDLDATA検索
 * @param	frobj		FE_FLDROBJ
 * @param	code		検索コード
 * @retval	MDLDATA		NULL=一致なし	
 */
//--------------------------------------------------------------
static MDLDATA * MdlDataSearch( FE_FLDROBJ *frobj, u32 code )
{
	u32 max = frobj->obj_max;
	MDLDATA *mdata = frobj->mdldata;
	do{ if(mdata->code==code){ return(mdata); } mdata++; max--; }while( max );
	return( NULL );
}

//--------------------------------------------------------------
/**
 * モデルデータの初期化
 * @paraum	
 * @retval	nothing
 */
//--------------------------------------------------------------
void FE_FldROBJ_MdlWorkInit( FE_SYS *fes, u32 max )
{
	MDLDATA *mdata;
	FE_FLDROBJ *frobj = FE_EffectWorkGet( fes, FE_FLD_FLDROBJ );
	GF_ASSERT( max );
	frobj->obj_max = max;
	mdata = FE_AllocClearMemory( fes, sizeof(MDLDATA)*max, ALLOC_FR, 0 );
	frobj->mdldata = mdata;
	do{ mdata->code = OBJCODEMAX; mdata++; max--; }while( max );
}

//--------------------------------------------------------------
/**
 * モデルデータの登録
 * @param	fes		FE_SYS
 * @param	code	登録コード
 * @param	arc_idx	アーカイブインデックス
 * @retval	nothing
 */
//--------------------------------------------------------------
void FE_FldROBJ_MdlAdd( FE_SYS *fes, u32 code, u32 arc_idx )
{
	MDLDATA *mdata;
	FE_FLDROBJ *frobj = FE_EffectWorkGet( fes, FE_FLD_FLDROBJ );
	mdata = MdlDataSearch( frobj, code );
	if( mdata != NULL ){ return; }
	mdata = MdlDataSearch( frobj, OBJCODEMAX );
	if( mdata == NULL ){ GF_ASSERT(0); return; }
	MdlDataInit( mdata, code, arc_idx, fes );
}

//--------------------------------------------------------------
/**
 * コードからFRO_MDL取得
 * @param	frobj	FE_FLDROBJ
 * @param	code	表示コード
 * @retval	FRO_MDL
 */
//--------------------------------------------------------------
FRO_MDL * FE_FldROBJ_FROMdlSearch( FE_SYS *fes, u32 code )
{
	FE_FLDROBJ *frobj = FE_EffectWorkGet( fes, FE_FLD_FLDROBJ );
	MDLDATA *mdata = MdlDataSearch( frobj, code );
	GF_ASSERT( mdata != NULL );
	return( &mdata->rmdl );
}

//--------------------------------------------------------------
/**
 * コードからFRO_OBJ取得
 * @param	frobj	FE_FLDROBJ
 * @param	code	表示コード
 * @retval	FRO_OBJ
 */
//--------------------------------------------------------------
FRO_OBJ * FE_FldROBJ_FROObjSearch( FE_SYS *fes, u32 code )
{
	FE_FLDROBJ *frobj = FE_EffectWorkGet( fes, FE_FLD_FLDROBJ );
	MDLDATA *mdata = MdlDataSearch( frobj, code );
	GF_ASSERT( mdata != NULL );
	return( &mdata->robj );
}

//==============================================================================
//	フィールドOBJ レンダ表示　EOA
//==============================================================================
//--------------------------------------------------------------
/**
 * フィールドOBJレンダ追加
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	offs	表示オフセット
 * @retval	eoa		EOA_PTR
 */
//--------------------------------------------------------------
EOA_PTR FE_FldROBJ_Add( CONST_FIELD_OBJ_PTR fldobj, const VecFx32 *offs )
{
	int pri;
	EOA_PTR eoa;
	FLDROBJ_H head;
	FE_SYS *fes = FE_FieldOBJ_FE_SYS_Get( fldobj );
	
	head.code = FieldOBJ_OBJCodeGet( fldobj );
	head.fes = fes;
	head.fldobj = fldobj;
	
	pri = FieldOBJ_TCBPriGet( fldobj, FLDOBJ_TCBPRI_OFFS_AFTER );
	eoa = FE_EoaAddNpp( fes, &DATA_EoaH_FldRObj, offs, 0, &head, pri );
	return( eoa );
}

//--------------------------------------------------------------
/**
 * EOA フィールドOBJレンダ表示　初期化
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work *
 * @retval	int TRUE=正常終了。FALSE=異常終了
 */
//--------------------------------------------------------------
static int EoaFldRObj_Init( EOA_PTR eoa, void *wk )
{
	EOA_FLDROBJ_WORK *work = wk;
	const FLDROBJ_H *head;
	
	head = EOA_AddPtrGet( eoa );
	work->head = *head;
	work->obj_id = FieldOBJ_OBJIDGet( work->head.fldobj );
	work->robj = FE_FldROBJ_FROObjSearch( work->head.fes, work->head.code );
	
	if( FieldOBJ_StatusBitCheck_Alies(work->head.fldobj) == TRUE ){
		work->zone_id = FieldOBJ_ZoneIDGetAlies( work->head.fldobj );
	}else{
		work->zone_id = FieldOBJ_ZoneIDGet( work->head.fldobj );
	}
	
	EOA_MatrixGet( eoa, &work->offs );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * EOA フィールドOBJレンダ表示　削除
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaFldRObj_Delete( EOA_PTR eoa, void *wk )
{
}

//--------------------------------------------------------------
/**
 * EOA フィールドOBJレンダ表示　動作
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaFldRObj_Move( EOA_PTR eoa, void *wk )
{
	EOA_FLDROBJ_WORK *work = wk;
	CONST_FIELD_OBJ_PTR fldobj = work->head.fldobj;
	
	if( FieldOBJ_CheckSameID(fldobj,work->obj_id,work->zone_id) == FALSE ){
		FE_EoaDelete( eoa );
		return;
	}
	
	{
		VecFx32 mtx,d_mtx;
		FieldOBJ_VecPosGet( fldobj, &mtx );
		FieldOBJ_VecDrawOffsGet( fldobj, &d_mtx );
		mtx.x += d_mtx.x + work->offs.x;
		mtx.y += d_mtx.y + work->offs.y;
		mtx.z += d_mtx.z + work->offs.z;
		EOA_MatrixSet( eoa, &mtx );
	}
}

//--------------------------------------------------------------
/**
 * EOA フィールドOBJレンダ表示　描画
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaFldRObj_Draw( EOA_PTR eoa, void *wk )
{
	EOA_FLDROBJ_WORK *work = wk;
	VecFx32 pos;
	EOA_MatrixGet( eoa, &pos );
	FRO_OBJ_DrawPos( work->robj, &pos );
}

//--------------------------------------------------------------
///	EOA_H
//--------------------------------------------------------------
static const EOA_H_NPP DATA_EoaH_FldRObj =
{
	sizeof(EOA_FLDROBJ_WORK),
	EoaFldRObj_Init,
	EoaFldRObj_Delete,
	EoaFldRObj_Move,
	EoaFldRObj_Draw,
};

//==============================================================================
//	フィールドOBJ　フィールドエフェクトレンダリングOBJ表示
//==============================================================================
//--------------------------------------------------------------
/**
 * フィールドOBJ フィールドエフェクト使用レンダリングOBJ初期化
 * @param	fes		FE_SYS
 * @retval	nothing
 */
//--------------------------------------------------------------
void FE_FieldRenderOBJInit( FE_SYS *fes )
{
	const CODEARCIDX *idx = DATA_FieldOBJRenderOBJArcIdx;
	u32 max = NELEMS( DATA_FieldOBJRenderOBJArcIdx );
	FE_FldROBJ_MdlWorkInit( fes, max );
	OS_Printf( "field effect robj max = %d\n", max );
	
	do{
		FE_FldROBJ_MdlAdd( fes, idx->code, idx->arc_idx );
		idx++; max--;
	}while( max );
}


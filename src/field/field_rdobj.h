//==============================================================================
/**
 * @file	field_rdobj.h
 * @brief	フィールド　レンダリングOBJ
 * @author	kagaya
 * @data	05.07.13
 */
//==============================================================================
#ifndef FIELD_RDOBJ_H_FILE
#define FIELD_RDOBJ_H_FILE

#include "system/arc_util.h"

//==============================================================================
/*
	フィールド　レンダリングOBJ　簡易説明
	
	<ソースファイル>
	field_rdobj.c
	field_rdobj.h
	
	<OBJ追加手順　モデル>
	●手順１　FRO_MDL構造体を用意
		FRO_MDL rmdl;
	
	●手順２　rmdlへリソースファイルヘッダーをセット
		FRO_MDL_ResSetFileHeader(); または FRO_MDL_ResSetArcLoad();
	
	●手順３　rmdlへセットしたモデルリソースにテクスチャをセット
		VBlank中にFRO_MDL_TexTransBind()か、
		FRO_MDL_TexTransBindVTaskAdd();
	
	<OBJ追加手順　アニメ>
	●手順１　FRO_ANM構造体を用意
		FRO_ANM ranm;
		
	●手順２　ranmへアニメリソースをセット
		リソースをメインとしてセットする場合は
		FRO_ANM_AnmResSetMain(),FRO_ANM_AnmResSetArcLoad()のいずれかを使用する。
		既にロード済みのリソースを使用する場合は
		FRO_ANM_AnmResSetSub()を使用する。
	
	●手順３　ranm内にアニメオブジェリソースを確保する
		FRO_ANM_AllocAnmObj(),もしくは
		FRO_MDLを参照するFRO_ANM_AllocAnmObjInMdl()か、
		FRO_ANM_AllocAnmObjInMdlAlloc()を使用する
	
	<OBJ追加手順 OBJ>
	●手順１　FRO_OBJ構造体を用意
		FRO_OBJ robj;
	
	●手順２　robjを初期化
		FRO_OBJ_Init()もしくは
		初期化に合わせFRO_MDL内のモデルとリンクするFRO_OBJ_InitInMdl()。
		
	●手順３　必要があればrobjへアニメをリンク
		FRO_OBJ_AddAnm()もしくはFRO_OBJ_AddAnmInAnm()。
		
		FRO_OBJ_InitAddAnm()を呼ぶと手順２，３を纏めて行う。
	
	<OBJアニメ>
		FRO_ANM_Play()を呼ぶ。

	<OBJ描画>
		FRO_OBJ_Draw()を呼ぶ。
	
	<OBJ削除>
	●FRO_MDL削除
		FRO_MDL_DeleteAll()を呼ぶ。
		
	●FRO_ANM削除
		FRO_ANM_DeleteAll()を呼ぶ。
	
	●FRO_OBJ削除
		特になし。
		ワークをクリアしたい場合はFRO_OBJ_Clear()。
	
	<アニメ リソース流用例>
		FRO_ANM ranm0,ranm1;
		FRO_ANM_AnmResSetArcLoad(ranm0);
		void *pRes = FRO_ANM_ResAnmGet(ranm0);
		FRO_ANM_AnmResSetSub(ranm1,pRes);
		もしくは
		FRO_ANM_AnmResObjInitSame()等
		
	<アニメ　参照、変更>
		FRO_ANM_FrameSet()
		FRO_ANM_FrameGet()
		FRO_ANM_MaxFrameGet()
		FRO_ANM_AnmEndCheck()
*/
//==============================================================================

//==============================================================================
//	define
//==============================================================================
//--------------------------------------------------------------
//	debug
//--------------------------------------------------------------
#ifdef PM_DEBUG
//#define DEBUG_FRO_VINTR_COUNT		//定義でVIntrTCB起動数カウント

#ifdef	DEBUG_FRO_VINTR_COUNT
//#define DEBUG_FRO_VINTR_COUNT_PRINT //定義でVIntrTCB起動数プリント
#define DEBUG_FRO_VINTR_COUNT_ASSERT //定義でVIntrTCBエラー停止
#endif

#endif	//PM_DEBUG

#define VTCBPRI_FRO_TEXBIND (0xffff)		///<テクスチャバインドVBlankTCBPriorty

//--------------------------------------------------------------
///	loop?
//--------------------------------------------------------------
typedef enum
{
	ANMLOOP_OFF = 0,
	ANMLOOP_ON,
}ANMLOOPTYPE;

//==============================================================================
//	typedef
//==============================================================================
//--------------------------------------------------------------
//	typedef
//--------------------------------------------------------------
typedef struct _TAG_FRO_MDL	FRO_MDL;
typedef struct _TAG_FRO_ANM	FRO_ANM;
typedef struct _TAG_FRO_OBJ FRO_OBJ;

//--------------------------------------------------------------
///	ROTATE
//--------------------------------------------------------------
typedef struct
{
	u16 x;
	u16 y;
	u16 z;
	u16 dmy;
}ROTATE;

//--------------------------------------------------------------
///	FRO_MDL	
//--------------------------------------------------------------
typedef struct _TAG_FRO_MDL
{
	BOOL bind_tex_flag;
	NNSG3dResFileHeader *pResFileHeader;
	NNSG3dResMdlSet *pResMdlSet;
	NNSG3dResMdl *pResMdl;
	NNSG3dResTex *pResTex;
};

//--------------------------------------------------------------
///	FRO_ANM
//--------------------------------------------------------------
typedef struct _TAG_FRO_ANM
{
	u32 status_bit;
	fx32 frame;
	void *pResAnm;
	void *pResAnmIdx;
	NNSG3dAnmObj *pAnmObj;
	NNSFndAllocator Allocator;
};

//--------------------------------------------------------------
///	FRO_OBJ
//--------------------------------------------------------------
typedef struct _TAG_FRO_OBJ
{
#if 0
	BOOL vanish;
	VecFx32 matrix;
	VecFx32 scale;
	MtxFx33 rotate;
#endif
	NNSG3dRenderObj RenderObj;
};

//==============================================================================
//	extern
//==============================================================================
extern void FRO_MDL_Clear( FRO_MDL *rmdl );
extern void FRO_MDL_ResSetFileHeader( FRO_MDL *rmdl, NNSG3dResFileHeader *head, u32 idx );
extern void FRO_MDL_ResSetArcLoad( FRO_MDL *rmdl, u32 idx,
		ARCHANDLE *handle, u32 arc_idx, u32 heap_id, ALLOC_TYPE alloc_type );
extern void FRO_MDL_TexTransBind( FRO_MDL *rmdl );
extern void FRO_MDL_TexTransBindVTaskAdd( FRO_MDL *rmdl );
extern void FRO_MDL_ResHeaderFree( FRO_MDL *rmdl );
extern void FRO_MDL_TexFree( FRO_MDL *rmdl );
extern void FRO_MDL_DeleteAll( FRO_MDL *rmdl );
extern BOOL FRO_MDL_TexBindCheck( FRO_MDL *rmdl );

extern void FRO_ANM_Clear( FRO_ANM *ranm );
extern void FRO_ANM_AnmResSetMain( FRO_ANM *ranm, void *pRes, u32 idx );
extern void FRO_ANM_AnmResSetSub( FRO_ANM *ranm, void *pRes, u32 idx );
extern void FRO_ANM_AnmResSetSame( FRO_ANM *ranm, FRO_ANM *m_ranm, u32 idx );
extern void FRO_ANM_AnmResSetArcLoad( FRO_ANM *ranm, u32 idx,
		ARCHANDLE *handle, u32 arc_idx,	u32 heap_id, ALLOC_TYPE alloc_type );
extern void FRO_ANM_AnmResObjInitMain( FRO_ANM *ranm, const FRO_MDL *rmdl,
		void *pRes, u32 idx, u32 heap_id );
extern void FRO_ANM_AnmResObjInitSub( FRO_ANM *ranm, const FRO_MDL *rmdl,
		void *pRes, u32 idx, u32 heap_id );
extern void FRO_ANM_AnmResObjInitSame( FRO_ANM *ranm, const FRO_MDL *rmdl,
		FRO_ANM *m_ranm, u32 idx, u32 heap_id );
extern void FRO_ANM_AnmResFree( FRO_ANM *ranm );
extern void FRO_ANM_AllocAnmObj( FRO_ANM *ranm, const NNSG3dResMdl *pMdl, u32 heap_id );
extern void FRO_ANM_AllocAnmObjInMdl( FRO_ANM *ranm, const FRO_MDL *rmdl, u32 heap_id );
extern void FRO_ANM_AnmObjInit( FRO_ANM *ranm,
	const NNSG3dResMdl* pResMdl, const NNSG3dResTex* pResTex );
extern void FRO_ANM_AnmObjInitInMdl( FRO_ANM *ranm, const FRO_MDL *rmdl );
extern void FRO_ANM_AnmObjFree( FRO_ANM *ranm );
extern void FRO_ANM_DeleteAll( FRO_ANM *ranm );
extern BOOL FRO_ANM_Play( FRO_ANM *ranm, fx32 speed, ANMLOOPTYPE loop );
extern void FRO_ANM_FrameSet( FRO_ANM *ranm, fx32 frame );
extern fx32 FRO_ANM_FrameGet( const FRO_ANM *ranm );
extern fx32 FRO_ANM_MaxFrameGet( const FRO_ANM *ranm );
extern BOOL FRO_ANM_AnmEndCheck( const FRO_ANM *ranm );
extern void * FRO_ANM_ResAnmGet( FRO_ANM *ranm );

extern void FRO_OBJ_Clear( FRO_OBJ *robj );
extern void FRO_OBJ_Init( FRO_OBJ *robj, NNSG3dResMdl *pResMdl );
extern void FRO_OBJ_InitInMdl( FRO_OBJ *robj, FRO_MDL *rmdl );
extern void FRO_OBJ_AddAnm( FRO_OBJ *robj, NNSG3dAnmObj *pAnmObj );
extern void FRO_OBJ_DelAnm( FRO_OBJ *robj, NNSG3dAnmObj *pAnmObj );
extern void FRO_OBJ_AddAnmInAnm( FRO_OBJ *robj, FRO_ANM *ranm );
extern void FRO_OBJ_DelAnmInAnm( FRO_OBJ *robj, FRO_ANM *ranm );
extern void FRO_OBJ_InitAll( FRO_OBJ *robj, FRO_MDL *rmdl, FRO_ANM *ranm );
extern void FRO_OBJ_Draw( FRO_OBJ *robj,
	const VecFx32 *pos, const VecFx32 *scale, const MtxFx33 *rotate );
extern void FRO_OBJ_DrawPos( FRO_OBJ *robj, const VecFx32 *pos );
extern void FRO_OBJ_DrawScaleRotate( FRO_OBJ *robj,
	const VecFx32 *pos, const VecFx32 *scale, const ROTATE *rotate );
extern void FRO_OBJ_DrawRotate33(
	FRO_OBJ *robj, const VecFx32 *pos, const MtxFx33 *rotate );
extern void FRO_OBJ_DrawRotate(
	FRO_OBJ *robj, const  VecFx32 *pos, const ROTATE *rotate );
extern void FRO_OBJ_Draw1Mat1Shape( FRO_OBJ *robj, const VecFx32 *, const VecFx32 *, const MtxFx33 * );

#if 0
extern void FRO_Rotate360Set( const ROTATE *r360, MtxFx33 *rotate );
#endif

#if 0
extern void FRO_OBJ_Draw( FRO_OBJ *robj );
extern void FRO_OBJ_Draw1Mat1Shape( FRO_OBJ *robj );

extern BOOL FRO_OBJ_VanishCheck( const FRO_OBJ *robj );
extern void FRO_OBJ_VanishSet( FRO_OBJ *robj, BOOL flag );
extern VecFx32 * FRO_OBJ_pMatrixGet( FRO_OBJ *robj );
extern void FRO_OBJ_MatrixSet( FRO_OBJ *robj, const VecFx32 *mtx );
extern VecFx32 * FRO_OBJ_pScaleGet( FRO_OBJ *robj );
extern void FRO_OBJ_ScaleSet( FRO_OBJ *robj, const VecFx32 *scale );
extern MtxFx33 * FRO_OBJ_pRotateGet( FRO_OBJ *robj );
extern void FRO_OBJ_RotateSet( FRO_OBJ *robj, const MtxFx33 *rotate );
extern void FRO_OBJ_Rotate360Set( FRO_OBJ *robj, const ROTATE *r360 );
#endif

#ifdef DEBUG_FRO_VINTR_COUNT
extern int DEBUG_FroVIntrCount;
#endif

#endif //FIELD_RDOBJ_H_FILE

//==============================================================================
/**
 * @file	cur.c
 * @brief	簡単な説明を書く
 * @author	goto
 * @date	2007.07.23(月)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
#include "common.h"
#include "system/clact_tool.h"
//#include "system/lib_pack.h"

#include "src/application/battle_recorder/batt_rec_gra.naix"
//#include "codein_gra.naix"
#include "cur.h"

#define	ID_HCUR					( 22222 )
#define	HEAPID_CURSOR( n )		( n->param.heap_id )

//==============================================================
// Prototype
//==============================================================
static BOOL Homing( s16 x1, s16 y1, f32 x2, f32 y2, f32* mx, f32* my, f32 speed, s16 range );
static void Cursor_Tcb( TCB_PTR tcb, void* work );
extern CURSOR_WORK*  Cursor_SystemCreate( CURSOR_PARAM param, CATS_SYS_PTR csp, CATS_RES_PTR crp, PALETTE_FADE_PTR pfd );
extern void Cursor_OAM_Load( CURSOR_WORK* wk, int color );
extern void Cursor_OAM_Create( CURSOR_WORK* wk );
extern void Cursor_OAM_Delete( CURSOR_WORK* wk );
extern BOOL Cursor_Create( CURSOR_WORK* wk, int color );
extern BOOL Cursor_Active( CURSOR_WORK* wk, BOOL active );
extern BOOL Cursor_PosSet( CURSOR_WORK* wk, s16 x, s16 y );
extern BOOL Cursor_PosSetFix( CURSOR_WORK* wk, s16 x, s16 y );
extern BOOL Cursor_Delete( CURSOR_WORK* wk );
extern BOOL Cursor_Visible( CURSOR_WORK* wk, BOOL flag );

static BOOL Cursor_PosCheck( CURSOR_WORK* wk, s16 x, s16 y );


static const int color_table[] = {
	NARC_batt_rec_gra_hcur_b_NCLR,
	NARC_batt_rec_gra_hcur_b2_NCLR,
	NARC_batt_rec_gra_hcur_b3_NCLR,
	NARC_batt_rec_gra_hcur_b4_NCLR,
	NARC_batt_rec_gra_hcur_b5_NCLR,
	NARC_batt_rec_gra_hcur_b6_NCLR,
	NARC_batt_rec_gra_hcur_b7_NCLR,
//	NARC_batt_rec_gra_hcur_b8_NCLR,
//	NARC_batt_rec_gra_hcur_b9_NCLR,
};


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	param	
 * @param	csp	
 * @param	crp	
 * @param	pfd	
 *
 * @retval	CURSOR_WORK*	
 *
 */
//--------------------------------------------------------------
CURSOR_WORK*  Cursor_SystemCreate( CURSOR_PARAM param, CATS_SYS_PTR csp, CATS_RES_PTR crp, PALETTE_FADE_PTR pfd )
{
	CURSOR_WORK* wk;
	
	wk = sys_AllocMemory( param.heap_id, sizeof( CURSOR_WORK ) );
	
	wk->param.use		= param.use;
	wk->param.heap_id	= param.heap_id;
	wk->param.d_area	= param.d_area;
	wk->param.type		= param.type;
	wk->param.surface_y	= param.surface_y;
	
	wk->sys.csp = csp;
	wk->sys.crp = crp;
	wk->sys.pfd = pfd;
	
	wk->seq		= 0;
	
	wk->range_rx = CUR_R_X;
	wk->range_ry = CUR_R_Y;
	
	wk->tcb		= NULL;
	wk->bTcb	= FALSE;

	return wk;
}

typedef struct {
	f32 x;
	f32 y;	
} POINT;

static BOOL Homing( s16 x1, s16 y1, f32 x2, f32 y2, f32* mx, f32* my, f32 speed, s16 range )
{
	f32 rg;
	f32 dat;
	POINT v;
	POINT t;
	POINT o;
	
	o.x = 0;
	o.y = 0;

	t.x = ( x1 - x2 );
	t.y = ( y1 - y2 );
		
	v.x = 0;
	v.y = 0;
	
//	rg  = Sqrt( ( t.x * t.x ) + ( t.y * t.y ) );
	dat = ( t.x * t.x ) + ( t.y * t.y );
	rg  = FX_Sqrt( FX_F32_TO_FX32( dat ) );
	rg  = FX_FX32_TO_F32( rg );
	
	if ( rg < speed || range > rg || rg == 0 ){ return FALSE; }

	v.x = ( t.x * speed ) / rg;
	v.y = ( t.y * speed ) / rg;
	
	*mx = ( v.x + o.x );
	*my = ( v.y + o.y );
	
	return TRUE;
}


//--------------------------------------------------------------
/**
 * @brief	TCB
 *
 * @param	tcb	
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void Cursor_Tcb( TCB_PTR tcb, void* work )
{
	CURSOR_WORK* wk = work;
	
	if ( wk->bTcb == FALSE ){ return; }

#ifdef CUR_VBLANK_UPDATE	
	wk->vcount++;
	wk->vcount %= 2;
	if ( wk->vcount ){ return; }
#endif
//	Cursor_Visible( wk, TRUE );
	{
		int i;
		int e = 0;
		f32 x;
		f32 y;
		fx32 nx;
		fx32 ny;
		f32 mx;
		f32 my;
		BOOL bMove;
				
		for ( i = 0; i < wk->param.use; i++ ){
			
			if ( i != 0 ){
				
				wk->cur[ i ].tx = wk->cur[ i - 1 ].tx;
				wk->cur[ i ].ty = wk->cur[ i - 1 ].ty;
			}
			
			CATS_ObjectPosGetCapFx32_SubSurface( wk->cur[ i ].cap, &nx, &ny, wk->param.surface_y );
			x = FX_FX32_TO_F32( nx );
			y = FX_FX32_TO_F32( ny );			
			
			bMove = Homing( wk->cur[ i ].tx, wk->cur[ i ].ty, x, y, &mx, &my, ( 8.0f - ( ( i - e ) * 0.5f ) ) / CUR_F, 16 );
			
			if ( bMove && wk->cur[ i ].rot_seq == 0 ){
				CATS_ObjectPosMoveCapFx32( wk->cur[ i ].cap, FX_F32_TO_FX32( mx ), FX_F32_TO_FX32( my ) );
			}
			else {
				{
					int n;
					int rad;
					s16 px, py;
					fx32 ox, oy;
					switch( wk->cur[ i ].rot_seq ){
					case 0:
						CATS_ObjectPosGetCap_SubSurface( wk->cur[ i ].cap, &px, &py, wk->param.surface_y );
						if ( wk->f_idx == 0xFF ){
							wk->f_idx = i;
							rad = FX_Atan2Idx( FX_F32_TO_FX32( wk->cur[ 0 ].ty - py ), FX_F32_TO_FX32( wk->cur[ 0 ].tx - px ) );
							wk->cur[ i ].rad = rad;
						}
						else {
							wk->cur[ i ].rad = wk->cur[ wk->f_idx ].rad - ( ( CUR_RAD / wk->param.use ) * wk->f_cnt );
						}
						wk->cur[ i ].rad %= CUR_RAD;
						wk->f_cnt++;
						wk->cur[ i ].rot_seq++;
						break;
					case 1:						
						wk->cur[ i ].rad += CUR_RAD_ADD;
						wk->cur[ i ].rad %= CUR_RAD;
						ox = ( wk->cur[ 0 ].tx << FX32_SHIFT ) + ( Sin360R( wk->cur[ i ].rad ) * wk->range_rx );
						oy = ( wk->cur[ 0 ].ty << FX32_SHIFT ) + ( Cos360R( wk->cur[ i ].rad ) * wk->range_ry );
						CATS_ObjectPosSetCapFx32_SubSurface( wk->cur[ i ].cap, ox, oy, wk->param.surface_y );
						break;
					default:
						break;
					}
				}
				e++;
			}
		}
	}
}

void Cursor_ColorTypeChange( CURSOR_WORK* wk, int color )
{
	ARCHANDLE*		 hdl;
	CATS_SYS_PTR	 csp;
	CATS_RES_PTR	 crp;
	PALETTE_FADE_PTR pfd;

	csp = wk->sys.csp;
	crp = wk->sys.crp;
	pfd = wk->sys.pfd;
	hdl = wk->sys.p_handle;	
	
	CATS_FreeResourcePltt( crp, ID_HCUR + CATS_D_AREA_MAIN);
	CATS_FreeResourcePltt( crp, ID_HCUR + CATS_D_AREA_SUB );
	CATS_LoadResourcePlttWorkArcH( pfd, FADE_MAIN_OBJ, csp, crp, hdl, color_table[ color ], FALSE, 1, NNS_G2D_VRAM_TYPE_2DMAIN, ID_HCUR + CATS_D_AREA_MAIN );
	CATS_LoadResourcePlttWorkArcH( pfd, FADE_SUB_OBJ,  csp, crp, hdl, color_table[ color ], FALSE, 1, NNS_G2D_VRAM_TYPE_2DSUB,  ID_HCUR + CATS_D_AREA_SUB );
}

void Cursor_OAM_Load( CURSOR_WORK* wk, int color )
{
	ARCHANDLE*		 hdl;
	CATS_SYS_PTR	 csp;
	CATS_RES_PTR	 crp;
	PALETTE_FADE_PTR pfd;
	int id = ID_HCUR + wk->param.d_area;

	csp = wk->sys.csp;
	crp = wk->sys.crp;
	pfd = wk->sys.pfd;
	hdl = wk->sys.p_handle;
	
	if ( wk->param.d_area == CATS_D_AREA_MAIN ){
		if ( wk->param.type == 0 ){
			CATS_LoadResourcePlttWorkArcH( pfd, FADE_MAIN_OBJ, csp, crp, hdl, color_table[ color ], FALSE, 1, NNS_G2D_VRAM_TYPE_2DMAIN, id );
		}
		else {
			CATS_LoadResourcePlttWorkArcH( pfd, FADE_MAIN_OBJ, csp, crp, hdl, NARC_batt_rec_gra_hcur_NCLR, FALSE, 1, NNS_G2D_VRAM_TYPE_2DMAIN, id );
		}

		CATS_LoadResourceCharArcH( csp, crp, hdl, NARC_batt_rec_gra_hcur_NCGR, FALSE, NNS_G2D_VRAM_TYPE_2DMAIN, id );
	}
	else {
		if ( wk->param.type == 0 ){
			CATS_LoadResourcePlttWorkArcH( pfd, FADE_SUB_OBJ, csp, crp, hdl, color_table[ color ], FALSE, 1, NNS_G2D_VRAM_TYPE_2DSUB, id );
		}
		else {
			CATS_LoadResourcePlttWorkArcH( pfd, FADE_SUB_OBJ, csp, crp, hdl, NARC_batt_rec_gra_hcur_NCLR, FALSE, 1, NNS_G2D_VRAM_TYPE_2DSUB, id );
		}
		CATS_LoadResourceCharArcH( csp, crp, hdl, NARC_batt_rec_gra_hcur_NCGR, FALSE, NNS_G2D_VRAM_TYPE_2DSUB, id );
	}
	CATS_LoadResourceCellArcH( csp, crp, hdl, NARC_batt_rec_gra_hcur_NCER, FALSE, id );
	CATS_LoadResourceCellAnmArcH( csp, crp, hdl, NARC_batt_rec_gra_hcur_NANR, FALSE, id );
}

void Cursor_OAM_Create( CURSOR_WORK* wk )
{
	int i;
	
	TCATS_OBJECT_ADD_PARAM_S coap;
	CATS_SYS_PTR csp;
	CATS_RES_PTR crp;
	PALETTE_FADE_PTR pfd;

	csp = wk->sys.csp;
	crp = wk->sys.crp;
	pfd = wk->sys.pfd;
	
	coap.x			= 128;
	coap.y			= 96;
	coap.z			= 0;		
	coap.anm		= 0;
	coap.pri		= 0;
	coap.d_area		= wk->param.d_area;//CATS_D_AREA_MAIN;
	coap.bg_pri		= 0;
	coap.vram_trans = 0;
	coap.pal		= 0;
	coap.id[0]		= ID_HCUR + wk->param.d_area;
	coap.id[1]		= ID_HCUR + wk->param.d_area;
	coap.id[2]		= ID_HCUR + wk->param.d_area;
	coap.id[3]		= ID_HCUR + wk->param.d_area;
	coap.id[4]		= CLACT_U_HEADER_DATA_NONE;
	coap.id[5]		= CLACT_U_HEADER_DATA_NONE;
	
	for ( i = 0; i < wk->param.use; i++ ){
		wk->cur[ i ].cap = CATS_ObjectAdd_S( csp, crp, &coap );
		CATS_ObjectUpdateCap( wk->cur[ i ].cap );
	//	CATS_ObjectObjModeSetCap( wk->cur[ i ].cap, GX_OAM_MODE_XLU );				///< 半透明
		CATS_ObjectPosSetCap( wk->cur[ i ].cap, 256 / 2, 192 / 2 );
		#if 0
		CATS_ObjectPosSetCap( wk->cur[ i ].cap, 72 + ( i * 8 ) + 4, POS_CODE_Y );
		CATS_ObjectAnimeSeqSetCap( wk->cur[ i ].cap, 0 );
		#endif
	}
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void Cursor_OAM_Delete( CURSOR_WORK* wk )
{
	int i;
	
	for ( i = 0; i < wk->param.use; i++ ){
		CATS_FreeResourceChar( wk->sys.crp, ID_HCUR + wk->param.d_area );
//		CATS_FreeResourcePltt( wk->sys.crp, ID_HCUR );
		CATS_FreeResourceCell( wk->sys.crp, ID_HCUR + wk->param.d_area );
		CATS_FreeResourceCellAnm( wk->sys.crp, ID_HCUR + wk->param.d_area );
		CATS_ActorPointerDelete_S( wk->cur[ i ].cap );
	}
}

//--------------------------------------------------------------
/**
 * @brief	準備	OAM作成とかTcb作成とか
 *
 * @param	wk	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL Cursor_Create( CURSOR_WORK* wk, int color )
{
	wk->sys.p_handle = ArchiveDataHandleOpen( ARC_BR_GRA, HEAPID_CURSOR( wk ) );
	
	///< 読み込み + 作成
	Cursor_OAM_Load( wk, color );
	Cursor_OAM_Create( wk );
	
//	wk->tcb = TCB_Add( Cursor_Tcb, wk, 0x1000 );
	wk->tcb = VIntrTCB_Add( Cursor_Tcb, wk, 0x1000 );
	
	return TRUE;
}


//--------------------------------------------------------------
/**
 * @brief	動作設定
 *
 * @param	wk	
 * @param	active	TRUE = 動くFALSE = 止まる
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL Cursor_Active( CURSOR_WORK* wk, BOOL active )
{
	wk->bTcb = active;
	
	return TRUE;
}


//--------------------------------------------------------------
/**
 * @brief	座標の設定
 *
 * @param	wk	
 * @param	x	
 * @param	y	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL Cursor_PosSet( CURSOR_WORK* wk, s16 x, s16 y )
{	
	wk->cur[ 0 ].tx = x;
	wk->cur[ 0 ].ty = y;
	
	wk->f_idx = 0xFF;
	wk->f_cnt = 0;
	
	{
		int i;
		for ( i = 0; i < wk->param.use; i++ ){
			
			wk->cur[ i ].rot_seq = 0;
		}
	}
	
	return TRUE;
}

BOOL Cursor_PosSet_Check( CURSOR_WORK* wk, s16 x, s16 y )
{
	if ( Cursor_PosCheck( wk, x, y ) == FALSE ){
		
		return FALSE;
	}
	
	wk->cur[ 0 ].tx = x;
	wk->cur[ 0 ].ty = y;
	
	wk->f_idx = 0xFF;
	wk->f_cnt = 0;
	
	{
		int i;
		for ( i = 0; i < wk->param.use; i++ ){
			
			wk->cur[ i ].rot_seq = 0;
		}
	}
	
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief	カーソルのターゲットが同じ場合は再設定しない。
 *
 * @param	wk	
 * @param	x	
 * @param	y	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL Cursor_PosCheck( CURSOR_WORK* wk, s16 x, s16 y )
{
	if ( wk->cur[ 0 ].tx == x && wk->cur[ 0 ].ty == y ){
		
		return FALSE;
	}
	
	return TRUE;
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 * @param	x	
 * @param	y	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL Cursor_PosSetFix( CURSOR_WORK* wk, s16 x, s16 y )
{
	{
		int i;		

		wk->f_idx = 0xFF;
		wk->f_cnt = 0;	
						
		for ( i = 0; i < wk->param.use; i++ ){
			wk->cur[ i ].tx = x;
			wk->cur[ i ].ty = y;			
			CATS_ObjectPosSetCap_SubSurface( wk->cur[ i ].cap, x, y, wk->param.surface_y );
		}
	}
	
	return TRUE;	
}

//--------------------------------------------------------------
/**
 * @brief	削除
 *
 * @param	wk	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL Cursor_Delete( CURSOR_WORK* wk )
{
	switch( wk->seq ){
	case 0:
		Cursor_Active( wk, FALSE );
		wk->seq++;
		break;
		
	case 1:
		TCB_Delete( wk->tcb );
		wk->seq++;
		break;
		
	default:
		Cursor_OAM_Delete( wk );
		ArchiveDataHandleClose( wk->sys.p_handle );		
		sys_FreeMemoryEz( wk );
		
		return FALSE;
	}
	
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 * @param	flag	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL Cursor_Visible( CURSOR_WORK* wk, BOOL flag )
{	
	{
		int i;
		for ( i = 0; i < wk->param.use; i++ ){
			
			CATS_ObjectEnableCap( wk->cur[ i ].cap, flag );
		}
	}
	
	return TRUE;
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void Cursor_R_Set( CURSOR_WORK* wk, int rx, int ry )
{
	
	if ( rx != 0 && ry != 0 ){
		wk->range_rx = rx;
		wk->range_ry = ry;
	}
	else {
		wk->range_rx = CUR_R_X;
		wk->range_ry = CUR_R_Y;
	}
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void Cursor_SizeL( CURSOR_WORK* wk )
{
	int i;	
	for ( i = 0; i < wk->param.use; i++ ){
		CATS_ObjectAnimeSeqSetCap( wk->cur[ i ].cap, 1 );
	}
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void Cursor_SizeS( CURSOR_WORK* wk )
{
	int i;	
	for ( i = 0; i < wk->param.use; i++ ){
		CATS_ObjectAnimeSeqSetCap( wk->cur[ i ].cap, 0 );
	}
}

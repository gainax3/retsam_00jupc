
#include "common.h"
#include "gflib/camera.h"

#include "system/procsys.h"
#include "system/msgdata.h"
#include "system/font_arc.h"
#include "system/brightness.h"
#include "system/arc_tool.h"
#include "system/clact_tool.h"

#include "system/laster.h"
#include "system/wipe.h"

#include "demo_tengan_common.h"

//==============================================================
// Prototype
//==============================================================
static BOOL Homing( s16 x1, s16 y1, f32 x2, f32 y2, f32* mx, f32* my, f32 speed, s16 range );
static BOOL Homing_Move( ILLUMINANT* wk, s16 tx, s16 ty, f32 speed, int r );
static void AI_Set( ILLUMINANT* wk );
static void AI_Turn( ILLUMINANT* wk );
static void AI_Turn_Ex( ILLUMINANT* wk, int x, int y, int xr, int yr );
static void AI_Settling( ILLUMINANT* wk );
static void AI_Irregularity( ILLUMINANT* wk );
static void Illuminant_Z_Update( ILLUMINANT* wk );


#define TARGET_X	( 128 )
#define TARGET_Y	(  48 )
#define TARGET_XR	(  64 )
#define TARGET_YR	(  24 )
#define SCALE_SIZ	( TARGET_Y + TARGET_YR )

enum {
	
	eAI_TYPE_SET,				///< 特定の位置へ移動する
	eAI_TYPE_TURN,				///< ある点を中心に回転する
	eAI_TYPE_SETTLING,			///< ある点に収束する
	eAI_TYPE_IRREGULARITY,		///< 不規則な動きをする
		
};

typedef struct {
	f32 x;
	f32 y;	
} POINT;


static void ( * const AI_Table[] )( ILLUMINANT* wk ) = {
	AI_Set,
	AI_Turn,
	AI_Settling,
	AI_Irregularity,
};

//--------------------------------------------------------------
/**
 * @brief	点から点＋半径までの移動
 *
 * @param	x1	
 * @param	y1	
 * @param	x2	
 * @param	y2	
 * @param	mx	
 * @param	my	
 * @param	speed	
 * @param	range	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
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
	
//	OS_Printf( "rg = %f speed = %f range = %d\n", rg, speed, range );
	
	if ( rg < speed || range > rg || rg == 0 ){ return FALSE; }

	v.x = ( t.x * speed ) / rg;
	v.y = ( t.y * speed ) / rg;
	
	*mx = ( v.x + o.x );
	*my = ( v.y + o.y );
	
	return TRUE;
}


//--------------------------------------------------------------
/**
 * @brief	ホーミングさせる関数
 *
 * @param	wk	
 * @param	tx	
 * @param	ty	
 * @param	speed	
 * @param	r	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL Homing_Move( ILLUMINANT* wk, s16 tx, s16 ty, f32 speed, int r )
{
	f32 x;
	f32 y;
	fx32 nx;
	fx32 ny;
	f32 mx;
	f32 my;
	BOOL bMove;
	fx32 ox, oy;

	CATS_ObjectPosGetCapFx32_SubSurface( wk->cap, &nx, &ny, FX32_CONST( 192 ) );
	x = FX_FX32_TO_F32( nx );
	y = FX_FX32_TO_F32( ny );
	
	bMove = Homing( tx, ty, x, y, &mx, &my, speed, r );
	
	if ( bMove ){
		
		ox = FX_F32_TO_FX32( mx );
		oy = FX_F32_TO_FX32( my );	
			
		wk->rad2 += 8;
		wk->rad2 %= 360;
		ox += ( Sin360R( wk->rad2 ) * 1 );
		oy += ( Cos360R( wk->rad2 ) * 1 );
		
		CATS_ObjectPosMoveCapFx32( wk->cap, ox, oy );
	}
	
	return bMove;
}


//--------------------------------------------------------------
/**
 * @brief	特定の位置まで行く
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void AI_Set( ILLUMINANT* wk )
{
	BOOL bMove = Homing_Move( wk, 128, 40, 5.0f, 32 );
}


//--------------------------------------------------------------
/**
 * @brief	その場で回転する
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void AI_Turn( ILLUMINANT* wk )
{
	fx32 ox, oy;

	wk->rad += 8;
	wk->rad %= 360;

	ox = ( TARGET_X << FX32_SHIFT ) + ( Sin360R( wk->rad ) * TARGET_XR );
	oy = ( TARGET_Y << FX32_SHIFT ) + ( Cos360R( wk->rad ) * TARGET_YR );
	
	CATS_ObjectPosSetCapFx32_SubSurface( wk->cap, ox, oy, FX32_CONST( 192 ) );
	
	wk->z = oy / FX32_ONE;
}

static void AI_Turn_Ex( ILLUMINANT* wk, int x, int y, int xr, int yr )
{
	fx32 ox, oy;

	wk->rad += 8;
	wk->rad %= 360;

	ox = ( x << FX32_SHIFT ) + ( Sin360R( wk->rad ) * xr );
	oy = ( y << FX32_SHIFT ) + ( Cos360R( wk->rad ) * yr );
	
	CATS_ObjectPosSetCapFx32_SubSurface( wk->cap, ox, oy, FX32_CONST( 192 ) );
}


//--------------------------------------------------------------
/**
 * @brief	ある点に向かって移動する
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void AI_Settling( ILLUMINANT* wk )
{
	fx32 ox, oy;
	static s16 target[][ 2 ] = {
		{  97,  80 },{  87, 106 },
		{ 102,  94 },{ 182,  83 },
		{ 129, 116 },{  97, 116 },
		{ 172,  84 },{ 187,  99 },
		{ 168, 123 },{ 108,  93 }, 
	};
	BOOL bMove;
	
	bMove = Homing_Move( wk, target[ wk->para[ 2 ] ][ 0 ], target[ wk->para[ 2 ] ][ 1 ], 3.0f, 10 );
	
	if ( bMove == FALSE ){
		wk->para[ 2 ]++;
		wk->para[ 2 ] %= NELEMS( target );
		wk->para[ 2 ] = gf_rand() % NELEMS( target );
		wk->ai = eAI_TYPE_IRREGULARITY;
	}
	
	CATS_ObjectPosGetCapFx32_SubSurface( wk->cap, &ox, &oy, FX32_CONST( 192 ) );
	
	wk->z = ( oy / FX32_ONE );
}


//--------------------------------------------------------------
/**
 * @brief	ある点で収束する
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void AI_Irregularity( ILLUMINANT* wk )
{	
	fx32 ox, oy;
	
	wk->rad2 += 8;
	wk->rad2 %= 360;
	
	if ( wk->rad2 < 12 ){
		wk->count++;
		wk->count %= 3;
		wk->para[ 0 ] = ( gf_rand() % wk->count ) + 1;
		wk->para[ 1 ] = ( gf_rand() % wk->count ) + 1;
		wk->ai = eAI_TYPE_SETTLING;
	}
	
	CATS_ObjectPosGetCapFx32_SubSurface( wk->cap, &ox, &oy, FX32_CONST( 192 ) );

	ox += ( Sin360R( wk->rad2 ) * ( 3 * wk->para[ 0 ] ) );
	oy += ( Cos360R( wk->rad2 ) * ( 3 * wk->para[ 1 ] ) );
	
	CATS_ObjectPosSetCapFx32_SubSurface( wk->cap, ox, oy, FX32_CONST( 192 ) );
	
	wk->z = oy / FX32_ONE;
}


//--------------------------------------------------------------
/**
 * @brief	Z座標での優先度アップデート
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void Illuminant_Z_Update( ILLUMINANT* wk )
{
	f32 scale;

	scale = wk->z;
	scale /= 100.0f;
	scale = 0.8f - scale;
	if ( scale <= 0.2f ){ scale = 0.2f; }
	if ( scale >= 0.6f ){ scale = 0.6f; }
	
	CATS_ObjectScaleSetCap( wk->cap, scale, scale );
	CATS_ObjectPriSetCap( wk->cap, wk->z );
}



//--------------------------------------------------------------
/**
 * @brief	常時動くTCB
 *
 * @param	tcb	
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void Illuminant_AI_TCB( TCB_PTR tcb, void* work )
{
	ILLUMINANT* wk = work;
	
	AI_Table[ wk->ai ]( wk );
		
	Illuminant_Z_Update( wk );
	
	CATS_ObjectUpdateCapEx( wk->cap );
}

//--------------------------------------------------------------
/**
 * @brief	シーン０用
 *
 * @param	tcb	
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void Illuminant_S0_AI_TCB( TCB_PTR tcb, void* work )
{
	ILLUMINANT* wk = work;
	
	if ( wk->ai ){
		AI_Turn_Ex( wk, TARGET_X, TARGET_Y - 12, 32, 12 );
	}
	else {
		s16	x, y;
		BOOL bMove; 
		
		if ( wk->cap_dmy && wk->bActive ){
			
			CATS_ObjectPosGetCap( wk->cap_dmy, &x, &y );
			
			bMove = Homing_Move( wk, x, y, 3.0f, 1 );
						
			{
				f32 sx, sy;
				
				CATS_ObjectScaleGetCap( wk->cap, &sx, &sy );

//				OS_Printf( "sx = %f\n", sx );
				
				if ( sx > 0.10f ){
					 sx = sx - 0.002f;
				}

//				OS_Printf( "sx = %f\n", sx );
				
				CATS_ObjectScaleSetCap( wk->cap, sx, sx );
			}
		}
	}
#if 0
	AI_Table[ wk->ai ]( wk );
		
	Illuminant_Z_Update( wk );
#endif	
	CATS_ObjectUpdateCapEx( wk->cap );
}


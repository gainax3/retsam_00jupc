//==============================================================================
/**
 * @file	bb_tool.c
 * @brief	簡単な説明を書く
 * @author	goto
 * @date	2008.02.02(土)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================

#include "common.h"
#include "bb_common.h"

#if 0
	まず、まねねの足元の１点の座標を付く津！
	まねねのクォータニオンの４３行列ににかけてあげつ！
	その結果の座標のYがある一定の値以下ならおちつ！	
	Lv80
	くぉーたにおんのベクトルのy軸の値[2]と[1][3]の割合から
	なんとかする
#endif


typedef struct {
	
	CATS_ACT_PTR cap;
	
	fx32 x;
	fx32 y;
	fx32 px;
	fx32 py;
	fx32 ox;		///< offset
	fx32 oy;
	fx32 rx;		///< range
	fx32 ry;
	u8	 type;
	u8	 vanish;
	u8	 cnt;
	u8	 dir;
	int  angle;
	s16  fall_y;
	s16	 range_x;
	s16	 range_y;
	s16	 wait;
	
} EFFECT_ONE;

typedef struct {
	
	BB_CLIENT* client;

	int			seq;
	int			wait;
	int			num;
	BOOL*		flag;
	
	EFFECT_ONE	pe1[ BB_KAMI_MAX ];

} EFFECT_WORK;

//==============================================================
// Prototype
//==============================================================
static void BB_Stardust_TCB( TCB_PTR tcb, void* work );
static inline void BB_Client_HanabiRadSet( EFFECT_ONE* wk );
static inline void BB_Client_HanabiPosUpdate( EFFECT_ONE* wk, fx32 x, fx32 y );
static void BB_Client_Effect_TCB( TCB_PTR tcb, void* work );
static void EFF_1( BB_EFF_WORK* wk );
static void EFF_2( BB_EFF_WORK* wk );
static void EFF_3( BB_EFF_WORK* wk );
static void EFF_5( BB_EFF_WORK* wk );
static void BB_Effect_Main_TCB( TCB_PTR tcb, void* work );
static void BB_Fever_Open( BB_FEVER* wk );
static void BB_Fever_Close( BB_FEVER* wk );
static int BB_MOVE_SPEED_GET( int speed );
static void BB_Fever_Main_TCB( TCB_PTR tcb, void* work );
static void Fever01_TCB( TCB_PTR tcb, void* work );
static void MoveInit( BB_LIGHT* wk );
static BOOL FrameOut( BB_LIGHT* wk, int speed, int dir_lr, int dir_ud );
static BOOL FrameIn( BB_LIGHT* wk, int speed, int dir_lr, int dir_ud );
static BOOL FrameInM( BB_LIGHT* wk, int speed, int dir_lr, int dir_ud );
static BOOL Roll( BB_LIGHT* wk, int speed, int dir_lr, int num );
static void Fever02_TCB( TCB_PTR tcb, void* work );
static void Fever03_TCB( TCB_PTR tcb, void* work );
static void Fever04_TCB( TCB_PTR tcb, void* work );
static void Fever05_TCB( TCB_PTR tcb, void* work );
extern BOOL Quaternion_Rotation( BB_3D_MODEL* wk, int x, int y, int ox, int oy, f32 pow, BOOL pow_get_set );
extern BOOL Quaternion_Rotation_Pow( BB_3D_MODEL* wk, f32 pow );
extern int NetID_To_PlayerNo( BB_WORK* wk, int net_id );
extern void BB_AddMoveReqFx( BB_ADDMOVE_WORK_FX* p_work, fx32 s_x, fx32 e_x, fx32 s_s, int count_max );
extern BOOL BB_AddMoveMainFx( BB_ADDMOVE_WORK_FX* p_work );
extern void BB_MoveInit_FX( BB_ADDMOVE_WORK_FX* p_data, fx32 s_num, fx32 e_num, s32 count );
extern BOOL BB_MoveMain_FX( BB_ADDMOVE_WORK_FX* p_data );
extern void BB_Stardust_Call( BB_CLIENT* wk, s16 x, s16 y );
extern void BB_Client_EffectStart( BB_CLIENT* wk, BOOL* flag );
extern void BB_Effect_Call( BB_CLIENT* wk );
extern void BB_Fever_Call( BB_CLIENT* wk );







//--------------------------------------------------------------
/**
 * @brief	クォータニオン回転
 *
 * @param	wk	
 * @param	x	
 * @param	y	
 * @param	ox	
 * @param	oy	
 * @param	pow	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL Quaternion_Rotation( BB_3D_MODEL* wk, int x, int y, int ox, int oy, f32 pow, BOOL pow_get_set )
{
	f32 a;
	fx32 sq;
	f32 dx, dy;
	
	dx = ( x - ox ) * BB_COEFFICIENT_DX * pow;
	dy = ( y - oy ) * BB_COEFFICIENT_DY * pow;
	
	if ( pow_get_set ){
		wk->get_pow_x = dx;
		wk->get_pow_y = dy;
		wk->pow_count = 8;
		wk->get_pow_tx = wk->get_pow_x / wk->pow_count;
		wk->get_pow_ty = wk->get_pow_y / wk->pow_count;
	}
	
	{
		f32	 tmp = ( ( dx * dx ) + ( dy * dy ) );
		fx32 tmp2 = FX_F32_TO_FX32( tmp );
		sq = FX_Sqrt( tmp2 );
		a = FX_FX32_TO_F32( sq );
		a *= BB_COEFFICIENT_A;
	}

	if ( a != 0.0 ) {
		fx32 fxa = sq;
		fx64c sin64c = FX_SinFx64c( fxa );
		fx64c cos64c = FX_CosFx64c( fxa );
		f32 sin = FX_FX64C_TO_F32( sin64c );
		f32 cos = FX_FX64C_TO_F32( cos64c );
		f32 as = sin / a;

		QUATERNION dq;

		dq.q[0] = cos;
		dq.q[1] = dy * as;
		dq.q[2] = dx * as;
		dq.q[3] = 0.0;

		Quaternion_Mul( &wk->tq, &dq, &wk->cq );
		Quaternion_SetMtx44( &wk->rt, &wk->tq );
		Quaternion_Copy( &wk->cq, &wk->tq );
		Quaternion_SetMtx44_to_MtxFx43( &wk->rt, &wk->tmp43 );		
		{
			QUATERNION tmp;
			f32 f = Quaternion_GetNorm( &wk->tq );
			tmp = wk->tq;
			Quaternion_DivReal( &wk->tq, &tmp, f );
		}		
		return TRUE;
	}	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	パワーを指定する
 *
 * @param	wk	
 * @param	pow	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL Quaternion_Rotation_Pow( BB_3D_MODEL* wk, f32 pow )
{
	f32 a;
	fx32 sq;
	f32 dx, dy;
	
	dx = wk->get_pow_x;
	dy = wk->get_pow_y;

	wk->get_pow_x -= wk->get_pow_tx;
	wk->get_pow_y -= wk->get_pow_ty;
	wk->pow_count--;
	
	{
		f32	 tmp = ( ( dx * dx ) + ( dy * dy ) );
		fx32 tmp2 = FX_F32_TO_FX32( tmp );
		sq = FX_Sqrt( tmp2 );
		a = FX_FX32_TO_F32( sq );
		a *= BB_COEFFICIENT_A;
	}

	if ( a != 0.0 ) {
		fx32 fxa = sq;
		fx64c sin64c = FX_SinFx64c( fxa );
		fx64c cos64c = FX_CosFx64c( fxa );
		f32 sin = FX_FX64C_TO_F32( sin64c );
		f32 cos = FX_FX64C_TO_F32( cos64c );
		f32 as = sin / a;

		QUATERNION dq;

		dq.q[0] = cos;
		dq.q[1] = dy * as;
		dq.q[2] = dx * as;
		dq.q[3] = 0.0;

		Quaternion_Mul( &wk->tq, &dq, &wk->cq );
		Quaternion_SetMtx44( &wk->rt, &wk->tq );
		Quaternion_Copy( &wk->cq, &wk->tq );
		Quaternion_SetMtx44_to_MtxFx43( &wk->rt, &wk->tmp43 );
		
		return TRUE;
	}	
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	NetID から PlayerNo取得
 *
 * @param	wk	
 * @param	net_id	
 *
 * @retval	int	
 *
 */
//--------------------------------------------------------------
int NetID_To_PlayerNo( BB_WORK* wk, int net_id )
{
	int i;
	
	for( i = 0; i < 4; i++ ){
		
		if( wk->table[ i ] == net_id ){
			
			return i;
		}
	}
	
	return 0xFF;
}


//----------------------------------------------------------------------------
/**
 *	@brief	加速動作汎用　初期化
 *
 *	@param	p_work	ワーク
 *	@param	s_x		開始座標
 *	@param	e_x		終了座標
 *	@param	s_s		開始速度
 *	@param	count_max ｶｳﾝﾀ値
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
void BB_AddMoveReqFx( BB_ADDMOVE_WORK_FX* p_work, fx32 s_x, fx32 e_x, fx32 s_s, int count_max )
{
	fx32 t_x_t;	// タイムの２乗
	fx32 vot;	// 初速度＊タイム
	fx32 dis;
	fx32 a;

	dis = e_x - s_x;
	
	// 加速値を求める
	// a = 2(x - vot)/(t*t)
	t_x_t = (count_max * count_max) << FX32_SHIFT;
	vot = FX_Mul( s_s, count_max * FX32_ONE );
	vot = dis - vot;
	vot = FX_Mul( vot, 2*FX32_ONE );
	a = FX_Div( vot, t_x_t );

	p_work->x = s_x;
	p_work->s_x = s_x;
	p_work->s_s = s_s;
	p_work->s_a = a;
	p_work->count = 0;
	p_work->count_max = count_max;
}

//----------------------------------------------------------------------------
/**
 *	@brief	加速動作メイン
 *
 *	@param	p_work	ワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
BOOL BB_AddMoveMainFx( BB_ADDMOVE_WORK_FX* p_work )
{
	fx32 dis;
	fx32 t_x_t;
	fx32 calc_work;
	fx32 vot;
	
	// 等加速度運動
	// dis = vot + 1/2( a*(t*t) )
	vot = FX_Mul( p_work->s_s, p_work->count << FX32_SHIFT );
	t_x_t = (p_work->count * p_work->count) << FX32_SHIFT;
	calc_work = FX_Mul( p_work->s_a, t_x_t );
	calc_work = FX_Div( calc_work, 2*FX32_ONE );	// 1/2(a*(t*t))
	dis = vot + calc_work;	///<移動距離

	p_work->x = p_work->s_x + dis;


	if( (p_work->count + 1) <= p_work->count_max ){
		p_work->count++;
		return FALSE;
	}

	p_work->count = p_work->count_max;
	return TRUE;
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	p_data	
 * @param	s_num	
 * @param	e_num	
 * @param	count	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_MoveInit_FX( BB_ADDMOVE_WORK_FX* p_data, fx32 s_num, fx32 e_num, s32 count )
{
	p_data->x			= s_num;
	p_data->s_x			= s_num;
	p_data->s_a			= e_num - s_num;
	p_data->count		= 0;
	p_data->count_max	= count;
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	p_data	
 * @param	count	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL BB_MoveMain_FX( BB_ADDMOVE_WORK_FX* p_data )
{
	s32 local_count;
	BOOL ret;
	fx32 num;
	
	if( p_data->count >= p_data->count_max ){
		local_count = p_data->count_max;
		ret = TRUE;
	}else{
		local_count = p_data->count;
		p_data->count++;
		ret = FALSE;
	}

	// 計算
	num = FX_Mul( p_data->s_a, local_count << FX32_SHIFT );
	num = FX_Div( num, p_data->count_max << FX32_SHIFT );
	num += p_data->s_x;

	// 設定
	p_data->x = num;

	return ret;
}


//--------------------------------------------------------------
/**
 * @brief	星くず
 *
 * @param	tcb	
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BB_Stardust_TCB( TCB_PTR tcb, void* work )
{
	BB_STARDUST* wk = work;

	int i;
	fx32 x, y;
	BOOL bEnd[ 4 ][ 2 ];
	BOOL bNext = TRUE;
	const fx32 tbl1[] = { FX32_CONST( -5 ), FX32_CONST( +3 ), FX32_CONST( +4 ), FX32_CONST( -3 ), FX32_CONST( +3 ), FX32_CONST( -2 ) };
	const fx32 tbl2[] = { FX32_CONST( +4 ), FX32_CONST( +5 ), FX32_CONST( -4 ), FX32_CONST( -5 ), FX32_CONST( -3 ), FX32_CONST( +2 ) };
	
	if ( (WIPE_SYS_EndCheck() == FALSE) || (wk->sys->comm_err_data.dis_err == TRUE) ){
		for ( i = 0; i < STAR_DUST_MAX; i++ ){
			CATS_ActorPointerDelete_S( wk->cap[ i ] );
		}
		wk->active = FALSE;
		TCB_Delete( tcb );
		return;
	}
	
	switch ( wk->seq ){
	case 0:
		for ( i = 0; i < STAR_DUST_MAX; i++ ){
			CATS_ObjectPosGetCapFx32( wk->cap[ i ], &x, &y );
			BB_AddMoveReqFx( &wk->data[ i ][ 0 ], x, x + tbl1[ i ], FX_F32_TO_FX32( 0.3f ), STAR_DUST_EFF_WAIT );
			BB_AddMoveReqFx( &wk->data[ i ][ 1 ], y, y + tbl2[ i ], FX_F32_TO_FX32( 0.2f ), STAR_DUST_EFF_WAIT );
		}
		wk->seq++;
		break;
	
	case 1:
		for ( i = 0; i < STAR_DUST_MAX; i++ ){
			bEnd[ i ][ 0 ] = BB_AddMoveMainFx( &wk->data[ i ][ 0 ] );
			bEnd[ i ][ 1 ] = BB_AddMoveMainFx( &wk->data[ i ][ 1 ] );

			CATS_ObjectPosSetCapFx32( wk->cap[ i ], wk->data[ i ][ 0 ].x, wk->data[ i ][ 1 ].x );
			
			if ( bEnd[ i ][ 0 ] == FALSE || bEnd[ i ][ 1 ] == FALSE ){
				bNext = FALSE;
			}

			CATS_ObjectUpdateCap( wk->cap[ i ] );
		}
		if ( bNext ){ wk->seq++; }
		break;

	default:
		for ( i = 0; i < STAR_DUST_MAX; i++ ){
			CATS_ActorPointerDelete_S( wk->cap[ i ] );
		}
		wk->active = FALSE;
		TCB_Delete( tcb );
		break;
	}	
}


//--------------------------------------------------------------
/**
 * @brief	星くず呼び出し
 *
 * @param	wk	
 * @param	x	
 * @param	y	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_Stardust_Call( BB_CLIENT* wk, s16 x, s16 y )
{
	int i;
	int j;
	
	for ( i = 0; i < STAR_DUST_EFF_MAX; i++ ){
		if ( wk->star[ i ].active == TRUE ){ continue; }
		
		wk->star[ i ].sys = wk->sys;
		wk->star[ i ].seq = 0;
		wk->star[ i ].active = TRUE;
		
		for ( j = 0; j < STAR_DUST_MAX; j++ ){
			wk->star[ i ].cap[ j ] = BB_disp_Stardust_Add( wk, x, y );
			CATS_ObjectAnimeSeqSetCap( wk->star[ i ].cap[ j ], ( ( x * ( i + 1 ) ) + ( y * ( j + 1 ) ) ) % 3 );
		}
		
		TCB_Add( BB_Stardust_TCB, &wk->star[ i ], BB_TCB_PRI_1 );
		break;
	}
}

static inline void BB_Client_HanabiRadSet( EFFECT_ONE* wk )
{
	wk->rx  = FX_Mul( Sin360R( wk->angle ), FX32_CONST( wk->range_x ) );
	wk->ry  = FX_Mul( Cos360R( wk->angle ), FX32_CONST( wk->range_y ) );
}

static inline void BB_Client_HanabiPosUpdate( EFFECT_ONE* wk, fx32 x, fx32 y )
{	
	wk->x  += x;
	wk->y  += y;
	
	CATS_ObjectPosSetCapFx32_SubSurface( wk->cap, wk->ox + wk->px + wk->rx + wk->x, wk->oy + wk->py + wk->ry + wk->y, FX32_CONST( BB_SURFACE_LIMIT ) );
}


static void BB_Client_Effect_TCB( TCB_PTR tcb, void* work )
{
	EFFECT_WORK* wk = work;
	BB_SYS* sys = wk->client->sys;

	if ( (WIPE_SYS_EndCheck() == FALSE) || (sys->comm_err_data.dis_err == TRUE) ){
		*wk->flag = FALSE;		
		TCB_Delete( tcb );
		sys_FreeMemoryEz( wk );
		return;
	}
	
	switch ( wk->seq ){
	case 0:
		wk->num = 0;
		wk->seq++;
		
	case 1:		
		{
			int i;
			s16 pos_tbl1[][ 2 ] = {	{  96, 58 },
									{ 230, 48 },
									{ 128, 32 },
									{  40, 64 },
									{ 180, 48 }, };
			u32 rand_x1 = pos_tbl1[ wk->num ][ 0 ];
			u32 rand_y1 = pos_tbl1[ wk->num ][ 1 ];

			wk->wait = 0;
			for ( i = 0; i < BB_KAMI_MAX; i++ ){
				wk->pe1[ i ].cnt		= i % 2;
				wk->pe1[ i ].range_x = 6 - ( i % BB_KAMI_BLOCK );
				wk->pe1[ i ].range_y = 6 - ( i % BB_KAMI_BLOCK );
				wk->pe1[ i ].cap		= wk->client->cap_hanabi[ i ];
				wk->pe1[ i ].angle	= ( ( i / BB_KAMI_BLOCK ) * ( 360 / BB_KAMI_LINE ) );
				wk->pe1[ i ].angle	%= 360;
				wk->pe1[ i ].fall_y	= 0;
				wk->pe1[ i ].wait	= -( ( i % BB_KAMI_BLOCK ) * 2 );
				wk->pe1[ i ].px		= FX32_CONST( rand_x1 );
				wk->pe1[ i ].py		= FX32_CONST( rand_y1 );
				wk->pe1[ i ].rx		= 0;
				wk->pe1[ i ].ry		= 0;
				wk->pe1[ i ].x		= 0;
				wk->pe1[ i ].y		= 0;
				wk->pe1[ i ].ox		= 0;
				wk->pe1[ i ].oy		= 0;
				wk->pe1[ i ].type	= 0;
				wk->pe1[ i ].vanish	= TRUE;
				wk->pe1[ i ].dir	= 0;
				
				if ( wk->pe1[ i ].angle == 180 ){
					wk->pe1[ i ].type = 2;
				}
				if ( wk->pe1[ i ].angle >= 270 && wk->pe1[ i ].angle <= 90 ){
					wk->pe1[ i ].type = 1;
				}
				BB_Client_HanabiRadSet( &wk->pe1[ i ] );
				BB_Client_HanabiPosUpdate( &wk->pe1[ i ], FX32_CONST( 0 ), FX32_CONST( wk->pe1[ i ].fall_y ) );
			}
		}		
		wk->seq++;
		break;
	
	case 2:
		{
			int i;
			s16 dx, dy;
			BOOL bEnd = TRUE;

			if ( wk->wait == 0 ){
				 wk->wait++;
				 BB_disp_Hanabi_OAM_Enable( wk->client, TRUE, 0 );
				 Snd_SePlay( BB_SND_HANABI );			///< 花火どがーん
			}
			if ( wk->wait > 30 ){
				wk->seq = 1;
				wk->num++;
				wk->num %= 5;
				BB_disp_Hanabi_OAM_Enable( wk->client, FALSE, 1 );
			}
			else {				
				wk->wait++;								
				for ( i = 0; i < BB_KAMI_MAX; i++ ){					
					if ( (++wk->pe1[ i ].wait) < 0 ){ continue; }
					
					if ( wk->pe1[ i ].wait >= ( 10 + wk->pe1[ i ].cnt ) ){
						
						if ( wk->pe1[ i ].wait == ( 10 + wk->pe1[ i ].cnt ) ){
							CATS_ObjectAnimeSeqSetCap( wk->pe1[ i ].cap, 0 );
						}
						if ( wk->pe1[ i ].wait == ( 18 + wk->pe1[ i ].cnt ) ){
							wk->pe1[ i ].vanish = 0;																					
						}
						else {
							if ( wk->pe1[ i ].wait % 2 ){
								wk->pe1[ i ].vanish ^= 1;
							}
						}
						
						CATS_ObjectEnableCap( wk->pe1[ i ].cap, wk->pe1[ i ].vanish );						
						switch ( wk->pe1[ i ].dir ){
						case 0:	wk->pe1[ i ].ox = FX32_CONST( -1 );	break;
						case 1:	wk->pe1[ i ].ox = FX32_CONST( +0 );	break;
						case 2:	wk->pe1[ i ].ox = FX32_CONST( +1 );	break;
						}
						wk->pe1[ i ].dir++;
						wk->pe1[ i ].dir %= 3;
					}					
					if ( wk->pe1[ i ].wait > 5 ){					
						switch( wk->pe1[ i ].type ){
						case 2:	wk->pe1[ i ].fall_y = ( wk->pe1[i].wait + 5 ) / 6;	break;
						case 1:	wk->pe1[ i ].fall_y = ( wk->pe1[i].wait + 5 ) / 6;	break;
						case 0:	wk->pe1[ i ].fall_y = ( wk->pe1[i].wait + 5 ) / 7;	break;
						}
					}
					
					switch( wk->pe1[ i ].type ){
					case 2:
						dx = ( 30 - ( wk->pe1[ i ].wait + 5 ) ) / 8;
						dy = ( 30 - ( wk->pe1[ i ].wait + 7 ) ) / 8;
						break;
					case 1:
					default:
						dx = ( 30 - ( wk->pe1[ i ].wait + 5 ) ) / 8;
						dy = ( 30 - ( wk->pe1[ i ].wait + 6 ) ) / 8;
						break;
					case 0:
						dx = ( 30 - ( wk->pe1[ i ].wait + 5 ) ) / 8;
						dy = ( 30 - ( wk->pe1[ i ].wait + 4 ) ) / 8;
						break;
					}
					if ( dx > 0 ){
						wk->pe1[ i ].range_x += dx;
					}
					else {
						wk->pe1[ i ].range_x++;
					}
					if ( dy > 0 ){
						wk->pe1[ i ].range_y += dy;
					}
					else {
						wk->pe1[ i ].range_y++;
					}					
					BB_Client_HanabiRadSet( &wk->pe1[ i ] );
					BB_Client_HanabiPosUpdate( &wk->pe1[ i ], FX32_CONST( 0 ), FX32_CONST( wk->pe1[ i ].fall_y ) );
				}
			}
			if ( *wk->flag ){
				wk->seq++;
			}
		}
		break;

	default:
		BB_disp_Hanabi_OAM_Enable( wk->client, FALSE, 0 );		
		sys_FreeMemoryEz( wk );
		TCB_Delete( tcb );
		break;
	}	
	BB_disp_Hanabi_OAM_Update( wk->client );
}

//--------------------------------------------------------------
/**
 * @brief	エフェクト開始
 *
 * @param	wk	
 * @param	level	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_Client_EffectStart( BB_CLIENT* wk, BOOL* flag )
{
	EFFECT_WORK* sub_wk = sys_AllocMemory( HEAPID_BB, sizeof( EFFECT_WORK ) );
	
	sub_wk->client	= wk;
	sub_wk->seq		= 0;
	sub_wk->wait	= 0;
	sub_wk->flag	= flag;
	
	TCB_Add( BB_Client_Effect_TCB, sub_wk, BB_TCB_PRI_1 );
}

#define TEMP_WAIT	( 15 )

//--------------------------------------------------------------
/**
 * @brief	ライトのエフェクト
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void EFF_1( BB_EFF_WORK* wk )
{
	BB_3D_MODEL* 	 mdl  = wk->data[ 0 ];
	int*			 col1 = wk->data[ 1 ];
	int*			 col2 = wk->data[ 3 ];
	PALETTE_FADE_PTR pfd  = wk->data[ 2 ];

	if ( wk->active == FALSE ){ return; }
	
	switch ( wk->seq ){
	case 0:
		if ( *col1 < BB_COLOR_FADE_MAX ){
			*col1 += 1;
		}
		else {
			*col1 = BB_COLOR_FADE_MAX;
			wk->seq++;
		}
		ColorConceChangePfd( pfd, FADE_MAIN_BG, BB_COLOR_FADE_PAL_OFS, *col1 + *col2, BB_COLOR_FADE_COL );
		ColorConceChangePfd( pfd, FADE_SUB_BG,  BB_COLOR_FADE_PAL_OFS, *col1 + *col2, BB_COLOR_FADE_COL );
		break;

	case 1:
		D3DOBJ_SetDraw( &mdl->obj, TRUE );
		wk->temp[ 0 ] = 0;
		wk->seq++;
		break;
	
	case 2:
		///< 待機中
		if ( wk->flag[ 0 ] == TRUE ){
			if ( ++wk->temp[ 0 ] >= TEMP_WAIT ){
				wk->seq++;
			}
		}
		break;
	
	case 3:
		if ( *col1 > 0 ){
			*col1 -= 1;
		}
		else {
			*col1 = 0;
			wk->seq++;
		}
		ColorConceChangePfd( pfd, FADE_MAIN_BG, BB_COLOR_FADE_PAL_OFS, *col1 + *col2, BB_COLOR_FADE_COL );
		ColorConceChangePfd( pfd, FADE_SUB_BG,  BB_COLOR_FADE_PAL_OFS, *col1 + *col2, BB_COLOR_FADE_COL );
		break;

	default:
		wk->flag[ 0 ] = FALSE;
		wk->active	  = FALSE;
		wk->seq		  = 0;
		D3DOBJ_SetDraw( &mdl->obj, FALSE );
		break;
	}
}

static void EFF_2( BB_EFF_WORK* wk )
{
	int i;
	s16 x, y;
	s16 pos_in[] = { 188, 188, 180, 180 };
	BOOL bEnd[ 4 ] = { TRUE,TRUE,TRUE,TRUE };
	CATS_ACT_PTR cap;
	
	if ( wk->active == FALSE ){ return; }
	
	switch ( wk->seq ){
	case 0:
		for ( i = 0; i < BB_HAND_MAX; i++ ){
			cap = wk->data[ i ];
			if ( cap == NULL ){ continue; }
			CATS_ObjectPosGetCap( cap, &x, &y );
			
			if ( y > pos_in[ i ] ){				
				y -= 6;	
				CATS_ObjectPosSetCap( cap, x, y );
				bEnd[ i ] = FALSE;
			}
		}
		if ( bEnd[ 0 ] && bEnd[ 1 ] && bEnd[ 2 ] && bEnd[ 2 ] ){
			wk->temp[ 0 ] = 0;
			OS_Printf( "拍手で終わった = %d\n", wk->flag[ 0 ] );
			wk->flag[ 0 ] = FALSE;
			wk->seq++;
		}
		break;
	
	case 1:
		///< 待機中
		if ( wk->flag[ 0 ] == TRUE ){
			if ( ++wk->temp[ 0 ] >= TEMP_WAIT ){
				OS_Printf( "拍手の下げるリクエスト発行済み = %d\n" );
				wk->seq++;
			}
		}
		break;
	
	case 2:
		for ( i = 0; i < BB_HAND_MAX; i++ ){
			cap = wk->data[ i ];
			if ( cap == NULL ){ continue; }
			CATS_ObjectPosGetCap( cap, &x, &y );
			if ( y < BB_SURFACE_LIMIT ){				
				y += 6;		
				CATS_ObjectPosSetCap( cap, x, y );
				bEnd[ i ] = FALSE;
			}
		}
		if ( bEnd[ 0 ] && bEnd[ 1 ] && bEnd[ 2 ] && bEnd[ 2 ] ){
			wk->seq++;
		}
		break;
	
	default:
		wk->flag[ 0 ] = FALSE;
		wk->active	  = FALSE;
		wk->seq		  = 0;
		break;
	}	
	for ( i = 0; i < BB_HAND_MAX; i++ ){
		cap = wk->data[ i ];
		if ( cap == NULL ){ continue; }
		
		CATS_ObjectUpdateCapEx( cap );
	}
}

static void EFF_3( BB_EFF_WORK* wk )
{
	int i;
	
	if ( wk->active == FALSE ){ return; }
	
	switch ( wk->seq ){
	case 0:
		for ( i = 0; i < BB_KAMI_HUBUKI_MAX; i++ ){
			BB_3D_MODEL* b3d = wk->data[ i ];
			D3DOBJ_AnmSet( &b3d->anm[ 0 ], ( i * 20 ) % 40 );
			D3DOBJ_SetDraw( &b3d->obj, TRUE ); 
		}
		wk->temp[ 9 ] = 0;
		wk->seq++;
		break;
		
	case 1:
		if ( wk->flag[ 0 ] == TRUE ){
			if ( ++wk->temp[ 9 ] >= TEMP_WAIT ){
				wk->seq++;
			}
		}
		break;
		
	default:
		for ( i = 0; i < BB_KAMI_HUBUKI_MAX; i++ ){
			BB_3D_MODEL* b3d = wk->data[ i ];
			D3DOBJ_SetDraw( &b3d->obj, FALSE );  
		}
		wk->flag[ 0 ] = FALSE;
		wk->active	  = FALSE;
		wk->seq		  = 0;
		break;
	}
}

static void EFF_5( BB_EFF_WORK* wk )
{
	if ( wk->active == FALSE ){ return; }
	
	switch ( wk->seq ){
	case 0:
		BB_Client_EffectStart( wk->data[ 0 ], &wk->flag[ 1 ] );
		wk->temp[ 0 ] = 0;
		wk->seq++;
		break;
	
	case 1:
		if ( wk->flag[ 0 ] == TRUE ){
			if ( ++wk->temp[ 0 ] >= TEMP_WAIT ){
				wk->flag[ 1 ] = TRUE;
				wk->seq++;
			}
		}
		break;
		
	default:
		if ( wk->flag[ 1 ] == FALSE ){
			wk->flag[ 0 ] = FALSE;
			wk->active	  = FALSE;
			wk->seq		  = 0;
		}
		break;
	}		
}

//--------------------------------------------------------------
/**
 * @brief	エフェクト メインTCB
 *
 * @param	tcb	
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BB_Effect_Main_TCB( TCB_PTR tcb, void* work )
{
	BB_EFFECT* wk = work;
	BB_SYS* sys = wk->sys;

	if ( (WIPE_SYS_EndCheck() == FALSE) || (sys->comm_err_data.dis_err == TRUE) ){
		TCB_Delete( tcb );
		return;
	}
	
	switch( *wk->level ){
	case 5:
		if ( wk->work[ 4 ].active == FALSE ){
			wk->work[ 4 ].active = TRUE;		
			wk->work[ 4 ].flag[ 0 ] = FALSE;
		}
	case 4:
		if ( wk->work[ 3 ].active == FALSE ){
			Snd_SePlay( BB_SND_KANSEI );		///< 歓声
			wk->work[ 3 ].active = TRUE;		
			wk->work[ 3 ].flag[ 0 ] = FALSE;
		}
	case 3:
		if ( wk->work[ 2 ].active == FALSE ){
			wk->work[ 2 ].active = TRUE;		
			wk->work[ 2 ].flag[ 0 ] = FALSE;
		}
	case 2:
		if ( wk->work[ 1 ].active == FALSE ){
			Snd_SePlay( BB_SND_HAND );			///< 拍手
			wk->work[ 1 ].active = TRUE;	
			wk->work[ 1 ].flag[ 0 ] = FALSE;	
		}
	case 1:
		if ( wk->work[ 0 ].active == FALSE ){
			wk->work[ 0 ].active = TRUE;
			wk->work[ 0 ].flag[ 0 ] = FALSE;
		}
		break;
	
	default:
		if ( *wk->level != wk->old_level ){
			wk->work[ 0 ].flag[ 0 ] = TRUE;	///< ライト消して
			wk->work[ 1 ].flag[ 0 ] = TRUE;	///< 拍手x2とめる
			wk->work[ 2 ].flag[ 0 ] = TRUE;	///< 紙ふぶき終了
			wk->work[ 3 ].flag[ 0 ] = TRUE;	///< 拍手x4とめる
			wk->work[ 4 ].flag[ 0 ] = TRUE;	///< 花火とめる
			Snd_SeStopBySeqNo( BB_SND_HAND, 0 );
		}
		break;
	}

	EFF_1( &wk->work[ 0 ] );				///< ライト
	EFF_2( &wk->work[ 1 ] );				///< 拍手x2
	EFF_3( &wk->work[ 2 ] );				///< 紙ふぶき
	EFF_2( &wk->work[ 3 ] );				///< 拍手x4
	EFF_5( &wk->work[ 4 ] );				///< 花火
	
	wk->old_level = *wk->level;
}


//--------------------------------------------------------------
/**
 * @brief	エフェクト呼び出し
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_Effect_Call( BB_CLIENT* wk )
{
	int i;

	wk->eff_sys.sys		  = wk->sys;
	wk->eff_sys.level	  = &wk->game_sys.eff_level;
	wk->eff_sys.old_level = *wk->eff_sys.level;
	
	for ( i = 0; i < BB_EFFECT_MAX; i++ ){
		BB_EFF_WORK* ewk = &wk->eff_sys.work[ i ];
		ewk->seq	 = 0;
		ewk->active = FALSE;
		
		switch ( i ){
		case 0:
			///< ライト初期化
			ewk->data[ 0 ] = &wk->bb3d_spot;	///< スポットライト
			ewk->data[ 1 ] = &wk->color1;		///< カラー
			ewk->data[ 3 ] = &wk->color2;		///< カラー
			ewk->data[ 2 ] = wk->sys->pfd;		///< パレットフェード
			break;

		case 1:
			///< 拍手１初期化
			ewk->data[ 0 ] = wk->cap_hand[ 0 ];
			ewk->data[ 1 ] = wk->cap_hand[ 1 ];
			ewk->data[ 2 ] = NULL;
			ewk->data[ 3 ] = NULL;
			{
				s16 x, y;
				CATS_ObjectPosGetCap( wk->cap_hand[ 0 ], &x, &y );
				CATS_ObjectPosSetCap( wk->cap_hand[ 0 ], x, BB_SURFACE_LIMIT );
				CATS_ObjectPosGetCap( wk->cap_hand[ 1 ], &x, &y );
				CATS_ObjectPosSetCap( wk->cap_hand[ 1 ], x, BB_SURFACE_LIMIT );
			}
			break;

		case 2:
			///< 紙吹雪初期化
			{
				int j;				
				for ( j = 0; j < BB_KAMI_HUBUKI_MAX; j++ ){
					ewk->data[ j ] = &wk->bb3d_kami[ j ];
				}
			}
			break;

		case 3:
			///< 拍手２初期化
			ewk->data[ 0 ] = NULL;
			ewk->data[ 1 ] = NULL;
			ewk->data[ 2 ] = wk->cap_hand[ 2 ];
			ewk->data[ 3 ] = wk->cap_hand[ 3 ];
			{
				s16 x, y;
				CATS_ObjectPosGetCap( wk->cap_hand[ 2 ], &x, &y );
				CATS_ObjectPosSetCap( wk->cap_hand[ 2 ], x, BB_SURFACE_LIMIT );
				CATS_ObjectPosGetCap( wk->cap_hand[ 3 ], &x, &y );
				CATS_ObjectPosSetCap( wk->cap_hand[ 3 ], x, BB_SURFACE_LIMIT );
			}
			break;

		case 4:
			///< 花火初期化
			ewk->data[ 0 ] = wk;
			break;
		}
	}
	
	wk->eff_sys.tcb = TCB_Add( BB_Effect_Main_TCB, &wk->eff_sys, BB_TCB_PRI_1 );
}


//--------------------------------------------------------------
/**
 * @brief	開く処理
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
#define BB_FEVER_CURTAIN_FRAME	( 10 )
static void BB_Fever_Open( BB_FEVER* wk )
{
	int* col1 = wk->col1;
	int* col2 = wk->col2;
	BOOL bEnd[ 5 ];
	
	/// ↑40　⇔80dot /// 56dot
	
	switch ( wk->seq ){
	case 0:
		BB_MoveInit_FX( &wk->scr_move[ 0 ], FX32_CONST( 0 ), FX32_CONST( +40 ), BB_FEVER_CURTAIN_FRAME );	///< ↑
		BB_MoveInit_FX( &wk->scr_move[ 1 ], FX32_CONST( 0 ), FX32_CONST( +80 ), BB_FEVER_CURTAIN_FRAME );	///< ←
		BB_MoveInit_FX( &wk->scr_move[ 2 ], FX32_CONST( 0 ), FX32_CONST( -80 ), BB_FEVER_CURTAIN_FRAME );	///< →
		wk->seq++;
		break;
	
	case 1:
		{
			int i;
			
			for ( i = 0; i < 3; i++ ){
				bEnd[ i ] = BB_MoveMain_FX( &wk->scr_move[ i ] );
			}
			GF_BGL_ScrollReq( wk->sys->bgl, GF_BGL_FRAME3_S, GF_BGL_SCROLL_Y_SET, wk->scr_move[ 0 ].x >> FX32_SHIFT );						
			GF_BGL_ScrollReq( wk->sys->bgl, GF_BGL_FRAME1_S, GF_BGL_SCROLL_X_SET, wk->scr_move[ 1 ].x >> FX32_SHIFT );			
			GF_BGL_ScrollReq( wk->sys->bgl, GF_BGL_FRAME2_S, GF_BGL_SCROLL_X_SET, wk->scr_move[ 2 ].x >> FX32_SHIFT );
			GF_BGL_ScrollReq( wk->sys->bgl, GF_BGL_FRAME1_M, GF_BGL_SCROLL_X_SET, wk->scr_move[ 1 ].x >> FX32_SHIFT );			
			GF_BGL_ScrollReq( wk->sys->bgl, GF_BGL_FRAME2_M, GF_BGL_SCROLL_X_SET, wk->scr_move[ 2 ].x >> FX32_SHIFT );			
			if ( bEnd[ 0 ] && bEnd[ 1 ] && bEnd[ 2 ] ){
				wk->seq++;
			}
		}
		break;
	
	case 2:
		wk->bOpenClose = TRUE;
		wk->seq	  = 0;
		break;
	}
	
	if ( *col2 < BB_COLOR_FADE_F_MAX ){
		 *col2 += 1;
	}
	else {
		 *col2 = BB_COLOR_FADE_F_MAX;
	}
	ColorConceChangePfd( wk->sys->pfd, FADE_MAIN_BG, BB_COLOR_FADE_PAL_OFS, *col1 + *col2, BB_COLOR_FADE_COL );
	ColorConceChangePfd( wk->sys->pfd, FADE_SUB_BG,  BB_COLOR_FADE_PAL_OFS, *col1 + *col2, BB_COLOR_FADE_COL );
}

static void BB_Fever_Close( BB_FEVER* wk )
{
	int* col1 = wk->col1;
	int* col2 = wk->col2;
	BOOL bEnd[ 5 ];
	
	/// ↑40　⇔80dot /// 56dot
	
	switch ( wk->seq ){
	case 0:
		BB_MoveInit_FX( &wk->scr_move[ 0 ], FX32_CONST( +40 ), FX32_CONST( 0 ), BB_FEVER_CURTAIN_FRAME );	///< ↑
		BB_MoveInit_FX( &wk->scr_move[ 1 ], FX32_CONST( +80 ), FX32_CONST( 0 ), BB_FEVER_CURTAIN_FRAME );	///< ←
		BB_MoveInit_FX( &wk->scr_move[ 2 ], FX32_CONST( -80 ), FX32_CONST( 0 ), BB_FEVER_CURTAIN_FRAME );	///< →
		wk->seq++;
		break;
	
	case 1:
		{
			int i;
			
			for ( i = 0; i < 3; i++ ){
				bEnd[ i ] = BB_MoveMain_FX( &wk->scr_move[ i ] );
			}
			GF_BGL_ScrollReq( wk->sys->bgl, GF_BGL_FRAME3_S, GF_BGL_SCROLL_Y_SET, wk->scr_move[ 0 ].x >> FX32_SHIFT );						
			GF_BGL_ScrollReq( wk->sys->bgl, GF_BGL_FRAME1_S, GF_BGL_SCROLL_X_SET, wk->scr_move[ 1 ].x >> FX32_SHIFT );			
			GF_BGL_ScrollReq( wk->sys->bgl, GF_BGL_FRAME2_S, GF_BGL_SCROLL_X_SET, wk->scr_move[ 2 ].x >> FX32_SHIFT );
			GF_BGL_ScrollReq( wk->sys->bgl, GF_BGL_FRAME1_M, GF_BGL_SCROLL_X_SET, wk->scr_move[ 1 ].x >> FX32_SHIFT );			
			GF_BGL_ScrollReq( wk->sys->bgl, GF_BGL_FRAME2_M, GF_BGL_SCROLL_X_SET, wk->scr_move[ 2 ].x >> FX32_SHIFT );			
			if ( bEnd[ 0 ] && bEnd[ 1 ] && bEnd[ 2 ] ){
				wk->seq++;
			}
		}
		break;
	
	case 2:
		wk->bOpenClose = FALSE;
		wk->seq	  = 0;
		break;
	}
	
	if ( *col2 > BB_COLOR_FADE_F_MIN ){
		 *col2 -= 1;
	}
	else {
		 *col2 = 0;
	}
	ColorConceChangePfd( wk->sys->pfd, FADE_MAIN_BG, BB_COLOR_FADE_PAL_OFS, *col1 + *col2, BB_COLOR_FADE_COL );
	ColorConceChangePfd( wk->sys->pfd, FADE_SUB_BG,  BB_COLOR_FADE_PAL_OFS, *col1 + *col2, BB_COLOR_FADE_COL );
}

///GF_BGL_FRAME1_M = ←
///GF_BGL_FRAME2_M = →

///GF_BGL_FRAME1_S = ←
///GF_BGL_FRAME2_S = →
///GF_BGL_FRAME3_S = ↑

//--------------------------------------------------------------
/**
 * @brief	シンクロメイン
 *
 * @param	tcb	
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
#define BB_FEVER_EFFECT_WAIT	( 30 * 5 )
//#define BB_MOVE_SPEED_GET( n )	( 30 / n )
static int BB_MOVE_SPEED_GET( int speed )
{
	switch ( speed ){
	case 1:	return 28;
	case 2:	return 14;
	case 3:	return 12;
	case 4:	return 10;
	case 5:	return  9;

	case  6:return 32 * 1;
	case  7:return 14 * 1;
	case  8:return 12 * 1;
	case  9:return 11 * 1;
	case 10:return 11 * 1;

	default: return 9;
	}
}

typedef struct {
	
	int wait;
	int num;
	int speed;
	int col[ 3 ];
	int col2[ 3 ];
	
	void* tcb_func;
	
} FEVER_PARAM;

static const FEVER_PARAM fever_para[] = {
	{ BB_FEVER_EFFECT_WAIT, 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, NULL },
	{ BB_FEVER_EFFECT_WAIT, 2, 1, { 0, 0, 0 }, { 0, 0, 0 }, Fever01_TCB },
	{ BB_FEVER_EFFECT_WAIT, 3, 2, { 0, 0, 0 }, { 0, 0, 0 }, Fever02_TCB },
	{ BB_FEVER_EFFECT_WAIT, 3, 2, { 0, 1, 0 }, { 1, 0, 0 }, Fever02_TCB },
	{ BB_FEVER_EFFECT_WAIT, 3, 3, { 0, 1, 2 }, { 2, 0, 0 }, Fever03_TCB },
	{ BB_FEVER_EFFECT_WAIT, 3, 4, { 0, 1, 2 }, { 2, 0, 0 }, Fever03_TCB },
	{ BB_FEVER_EFFECT_WAIT, 3, 5, { 2, 1, 0 }, { 1, 0, 0 }, Fever04_TCB },
	{ BB_FEVER_EFFECT_WAIT, 3, 5, { 2, 1, 0 }, { 2, 0, 0 }, Fever05_TCB },
	{ BB_FEVER_EFFECT_WAIT, 3, 5, { 2, 1, 0 }, { 2, 0, 0 }, Fever05_TCB },
};

static void BB_Fever_Main_TCB( TCB_PTR tcb, void* work )
{
	int i;
	int col;
	int level;
	BB_FEVER* wk = work;
	BB_LIGHT_MOVE* mvwk = &wk->mvwk;
	
	level = *wk->level;

	if ( (WIPE_SYS_EndCheck() == FALSE) || (wk->sys->comm_err_data.dis_err == TRUE) ){
		TCB_Delete( tcb );
		return;
	}

	if ( wk->bActive ){
		if ( wk->bOpenClose == FALSE ){
			BB_Fever_Open( wk );
		}
		if ( wk->wait == 0 ){
			if ( level && level < ( BB_FEVER_LEVEL_MAX - 0 ) ){	///< 1 - 7 まで
				wk->wait	= fever_para[ level ].wait;
				wk->eff_seq = 0;
				mvwk->num	= fever_para[ level ].num - 1;		///< 上３個はありえないそーなので
				mvwk->speed	  = fever_para[ level ].speed;
				mvwk->speed_m = fever_para[ level ].speed + 5;
				
				for ( i = 0; i < BB_LIGHT_MAX; i++ ){
					int col = fever_para[ level ].col[ i ];
					CATS_ObjectPaletteOffsetSetCap( mvwk->main[ i ].cap, eBB_OAM_PAL_BD_LIGHT  + col );
					col = fever_para[ level ].col2[ i ];
					CATS_ObjectPaletteOffsetSetCap( mvwk->sub[ i ].cap,  eBB_OAM_PAL_TD_MANENE + col ); 
				}
				
				TCB_Add( fever_para[ level ].tcb_func, wk, BB_TCB_PRI_2 );
//				OS_Printf( " level %d の エフェクトを登録\n ", level );
			}
		}
	}
	else if ( wk->bActive == FALSE && wk->wait == 0 ) {
		if ( wk->bOpenClose == TRUE ){
			BB_Fever_Close( wk );
		}
	}
	if ( wk->wait ) { wk->wait--; }
}

//--------------------------------------------------------------
/**
 * @brief	シンクロタスク呼び出し
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_Fever_Call( BB_CLIENT* wk )
{
	wk->fever_sys.level = &wk->game_sys.fever_level;
	wk->fever_sys.col1	= &wk->color1;
	wk->fever_sys.col2	= &wk->color2;
	wk->fever_sys.sys	= wk->sys;
	wk->fever_sys.wait	= 0;
	wk->fever_sys.seq	= 0;
	wk->fever_sys.bActive = FALSE;

	{
		int i;
		
		for ( i = 0; i < BB_LIGHT_MAX; i++ ){
			wk->fever_sys.mvwk.main[ i ].cap = wk->cap_light_m[ i ];
			wk->fever_sys.mvwk.sub[ i ].cap  = wk->cap_light_s[ i ];
		}
	}

	wk->fever_sys.tcb	= TCB_Add( BB_Fever_Main_TCB, &wk->fever_sys, BB_TCB_PRI_1 );
}

enum {
	
	BB_DIR_L = 0,
	BB_DIR_R,
	BB_DIR_U,
	BB_DIR_D,
};

#define BB_LIGHT_CENTER	( 128 + 32 )
#define BB_LIGHT_RY		( 24 )

//--------------------------------------------------------------
/**
 * @brief	2個のライトが動くやつ
 *
 * @param	tcb	
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void Fever01_TCB( TCB_PTR tcb, void* work )
{
	BB_FEVER* wk = work;
	BB_LIGHT_MOVE* mvwk = &wk->mvwk;
	BOOL bEnd[ 2 ];
	fx32 ox, oy;
	fx32 px, py;

	if ( (WIPE_SYS_EndCheck() == FALSE) || (wk->sys->comm_err_data.dis_err == TRUE) ){
		TCB_Delete( tcb );
		return;
	}
	
	switch ( wk->eff_seq ){
	case 0:
		mvwk->main[ 0 ].rad	= 0;
		mvwk->main[ 0 ].cx	= 128;
		mvwk->main[ 0 ].cy	= BB_LIGHT_CENTER;
		mvwk->main[ 0 ].rx	= 64;
		mvwk->main[ 0 ].ry	= BB_LIGHT_RY;
		CATS_ObjectEnableCap( mvwk->main[ 0 ].cap, TRUE );

		mvwk->sub[ 0 ].rad	= 0;
		mvwk->sub[ 0 ].cx	= 128;
		mvwk->sub[ 0 ].cy	= BB_LIGHT_CENTER;
		mvwk->sub[ 0 ].rx	= 64;
		mvwk->sub[ 0 ].ry	= BB_LIGHT_RY;
		CATS_ObjectEnableCap( mvwk->sub[ 0 ].cap, TRUE );
		{
			CATS_ObjectPosGetCapFx32_SubSurface( mvwk->main[ 0 ].cap, &px, &py, BB_SURFACE_Y_FX );			
			ox = ( mvwk->main[ 0 ].cx << FX32_SHIFT ) + ( Sin360R( mvwk->main[ 0 ].rad ) * mvwk->main[ 0 ].rx );
			oy = ( mvwk->main[ 0 ].cy << FX32_SHIFT ) + ( Cos360R( mvwk->main[ 0 ].rad ) * mvwk->main[ 0 ].ry );			
			CATS_ObjectPosSetCapFx32_SubSurface( mvwk->main[ 0 ].cap, ox, oy, BB_SURFACE_Y_FX );
			BB_MoveInit_FX( &mvwk->main[ 0 ].mx, ox, ox + FX32_CONST( 180 ), BB_MOVE_SPEED_GET( mvwk->speed_m ) );
			BB_MoveInit_FX( &mvwk->main[ 0 ].my, oy, oy - FX32_CONST(  BB_LIGHT_RY ), BB_MOVE_SPEED_GET( mvwk->speed_m ) );
			
			CATS_ObjectPosGetCapFx32_SubSurface( mvwk->sub[ 0 ].cap, &px, &py, BB_SURFACE_Y_FX );			
			ox = ( mvwk->sub[ 0 ].cx << FX32_SHIFT ) + ( Sin360R( mvwk->sub[ 0 ].rad ) * mvwk->sub[ 0 ].rx );
			oy = ( mvwk->sub[ 0 ].cy << FX32_SHIFT ) - ( Cos360R( mvwk->sub[ 0 ].rad ) * mvwk->sub[ 0 ].ry );			
			CATS_ObjectPosSetCapFx32_SubSurface( mvwk->sub[ 0 ].cap, ox, oy, BB_SURFACE_Y_FX );
			BB_MoveInit_FX( &mvwk->sub[ 0 ].mx, ox, ox - FX32_CONST( 180 ), BB_MOVE_SPEED_GET( mvwk->speed ) );
			BB_MoveInit_FX( &mvwk->sub[ 0 ].my, oy, oy + FX32_CONST(  BB_LIGHT_RY ), BB_MOVE_SPEED_GET( mvwk->speed ) );
		}
		MoveInit( &mvwk->sub[ 0 ] );
		MoveInit( &mvwk->main[ 0 ] );
		wk->eff_seq++;

	case 1:		
		bEnd[ 0 ] = Roll( &mvwk->sub[ 0 ], mvwk->speed, BB_DIR_L, 1 );
		bEnd[ 1 ] = FrameInM( &mvwk->main[ 0 ], mvwk->speed_m, BB_DIR_R, BB_DIR_D );		
		if ( bEnd[ 0 ] && bEnd[ 1 ] ){
			MoveInit( &mvwk->sub[ 0 ] );
			MoveInit( &mvwk->main[ 0 ] );
			wk->eff_seq++;
		}
		break;
	
	case 2:		
		bEnd[ 0 ] = FrameOut( &mvwk->sub[ 0 ], mvwk->speed, BB_DIR_L, BB_DIR_D );
		bEnd[ 1 ] = FrameOut( &mvwk->main[ 0 ], mvwk->speed_m, BB_DIR_R, BB_DIR_U );		
		if ( bEnd[ 0 ] && bEnd[ 1 ] ){
			MoveInit( &mvwk->sub[ 0 ] );
			MoveInit( &mvwk->main[ 0 ] );
			wk->eff_seq++;
		}
		break;
	
	case 3:		
		bEnd[ 0 ] = FrameIn( &mvwk->sub[ 0 ], mvwk->speed, BB_DIR_L, BB_DIR_U );
		bEnd[ 1 ] = FrameInM( &mvwk->main[ 0 ], mvwk->speed_m, BB_DIR_R, BB_DIR_D );		
		if ( bEnd[ 0 ] && bEnd[ 1 ] ){
			MoveInit( &mvwk->sub[ 0 ] );
			MoveInit( &mvwk->main[ 0 ] );
			wk->eff_seq++;
		}
		break;
	
	case 4:
		bEnd[ 0 ] = Roll( &mvwk->sub[ 0 ], mvwk->speed, BB_DIR_L, 1 );
		bEnd[ 1 ] = FrameOut( &mvwk->main[ 0 ], mvwk->speed_m, BB_DIR_R, BB_DIR_U );		
		if ( bEnd[ 0 ] && bEnd[ 1 ] ){
			MoveInit( &mvwk->sub[ 0 ] );
			MoveInit( &mvwk->main[ 0 ] );
			wk->eff_seq++;
		}
		break;

	default:
//		OS_Printf( " TYPE 1 EFFECT END = %d\n", wk->wait );
		TCB_Delete( tcb );
		break;
	}
}


static void MoveInit( BB_LIGHT* wk )
{
	wk->seq = 0;
}

static BOOL FrameOut( BB_LIGHT* wk, int speed, int dir_lr, int dir_ud )
{
	BOOL bEnd[ 2 ];
	fx32 ox, oy;
	fx32 px, py;
	
	switch ( wk->seq ){
	case 0:
		CATS_ObjectPosGetCapFx32_SubSurface( wk->cap, &ox, &oy, BB_SURFACE_Y_FX );	
		if ( dir_lr == BB_DIR_L ){
			BB_MoveInit_FX( &wk->mx, ox, ox - FX32_CONST( 180 ), BB_MOVE_SPEED_GET( speed ) );
		}
		else {
			BB_MoveInit_FX( &wk->mx, ox, ox + FX32_CONST( 180 ), BB_MOVE_SPEED_GET( speed ) );
		}
		if ( dir_ud == BB_DIR_U ){
			BB_MoveInit_FX( &wk->my, oy, oy - FX32_CONST(  BB_LIGHT_RY ), BB_MOVE_SPEED_GET( speed ) );
		}
		else {
			BB_MoveInit_FX( &wk->my, oy, oy + FX32_CONST(  BB_LIGHT_RY ), BB_MOVE_SPEED_GET( speed ) );
		}
		wk->seq++;

	case 1:
		bEnd[ 0 ] = BB_MoveMain_FX( &wk->mx );
					BB_MoveMain_FX( &wk->my );
		CATS_ObjectPosSetCapFx32_SubSurface( wk->cap, wk->mx.x, wk->my.x, BB_SURFACE_Y_FX );
		if ( bEnd[ 0 ] ){
			return TRUE;
		}
		break;

	default:
		return TRUE;
	}
	
	return FALSE;
}

static BOOL FrameIn( BB_LIGHT* wk, int speed, int dir_lr, int dir_ud )
{
	BOOL bEnd[ 2 ];
	fx32 ox, oy;
	fx32 px, py;
	
	switch ( wk->seq ){
	case 0:		
		if ( dir_lr == BB_DIR_L ){
			BB_MoveInit_FX( &wk->mx, FX32_CONST( 308 ), FX32_CONST( 128 ), BB_MOVE_SPEED_GET( speed ) );
		}
		else {
			BB_MoveInit_FX( &wk->mx, FX32_CONST( -62 ), FX32_CONST( 128 ), BB_MOVE_SPEED_GET( speed ) );
		}
		if ( dir_ud == BB_DIR_U ){
			BB_MoveInit_FX( &wk->my, FX32_CONST(  BB_LIGHT_CENTER ), FX32_CONST( BB_LIGHT_CENTER - BB_LIGHT_RY ), BB_MOVE_SPEED_GET( speed ) );
		}
		else {
			BB_MoveInit_FX( &wk->my, FX32_CONST(  BB_LIGHT_CENTER ), FX32_CONST( BB_LIGHT_CENTER + BB_LIGHT_RY ), BB_MOVE_SPEED_GET( speed ) );
		}
		wk->seq++;

	case 1:
		bEnd[ 0 ] = BB_MoveMain_FX( &wk->mx );
					BB_MoveMain_FX( &wk->my );
		CATS_ObjectPosSetCapFx32_SubSurface( wk->cap, wk->mx.x, wk->my.x, BB_SURFACE_Y_FX );
		if ( bEnd[ 0 ] ){
			return TRUE;
		}
		break;
		
	default:
		return TRUE;
	}
	
	return FALSE;
}

static BOOL FrameInM( BB_LIGHT* wk, int speed, int dir_lr, int dir_ud )
{
	BOOL bEnd[ 2 ];
	fx32 ox, oy;
	fx32 px, py;
	
	switch ( wk->seq ){
	case 0:		
		if ( dir_lr == BB_DIR_L ){
			BB_MoveInit_FX( &wk->mx, FX32_CONST( 308 ), FX32_CONST( 128 ), BB_MOVE_SPEED_GET( speed ) );
		}
		else {
			BB_MoveInit_FX( &wk->mx, FX32_CONST( -62 ), FX32_CONST( 128 ), BB_MOVE_SPEED_GET( speed ) );
		}
		if ( dir_ud == BB_DIR_U ){
			BB_MoveInit_FX( &wk->my, FX32_CONST(  96 ), FX32_CONST( 96 - BB_LIGHT_RY ), BB_MOVE_SPEED_GET( speed ) );
		}
		else {
			BB_MoveInit_FX( &wk->my, FX32_CONST(  96 ), FX32_CONST( 96 + BB_LIGHT_RY ), BB_MOVE_SPEED_GET( speed ) );
		}
		wk->seq++;

	case 1:
		bEnd[ 0 ] = BB_MoveMain_FX( &wk->mx );
					BB_MoveMain_FX( &wk->my );
		CATS_ObjectPosSetCapFx32_SubSurface( wk->cap, wk->mx.x, wk->my.x, BB_SURFACE_Y_FX );
		if ( bEnd[ 0 ] ){
			return TRUE;
		}
		break;
		
	default:
		return TRUE;
	}
	
	return FALSE;
}

static BOOL Roll( BB_LIGHT* wk, int speed, int dir_lr, int num )
{
	BOOL bEnd[ 2 ];
	fx32 ox, oy;
	fx32 px, py;
	
	switch ( wk->seq ){
	case 0:
		wk->num = 0;
		wk->rad = 0;
		wk->seq++;
		
	case 1:
		wk->rad += ( 360 / BB_MOVE_SPEED_GET( speed ) );
		if ( wk->rad >= 360 ){
			 wk->rad = 0;
		}
		CATS_ObjectPosGetCapFx32_SubSurface( wk->cap, &px, &py, BB_SURFACE_Y_FX );		
		if ( dir_lr == BB_DIR_L ){
			ox = ( wk->cx << FX32_SHIFT ) - ( Sin360R( wk->rad ) * wk->rx );
			oy = ( wk->cy << FX32_SHIFT ) - ( Cos360R( wk->rad ) * wk->ry );			
		}
		else {
			ox = ( wk->cx << FX32_SHIFT ) + ( Sin360R( wk->rad ) * wk->rx );
			oy = ( wk->cy << FX32_SHIFT ) - ( Cos360R( wk->rad ) * wk->ry );
		}
		CATS_ObjectPosSetCapFx32_SubSurface( wk->cap, ox, oy, BB_SURFACE_Y_FX );
		if ( wk->rad == 0 ){
			wk->num++;
			if ( wk->num >= num ){
				wk->seq++;
				return TRUE;
			}
			else {
				wk->seq = 1;
			}
		}
		break;

	default:
		return TRUE;
	}	
	return FALSE;
}

static void Fever02_TCB( TCB_PTR tcb, void* work )
{
	int i;
	BB_FEVER* wk = work;
	BB_LIGHT_MOVE* mvwk = &wk->mvwk;
	BOOL bEnd[ 4 ] = { TRUE, TRUE, TRUE, TRUE };
	fx32 ox, oy;
	fx32 px, py;

	if ( (WIPE_SYS_EndCheck() == FALSE) || (wk->sys->comm_err_data.dis_err == TRUE) ){
		TCB_Delete( tcb );
		return;
	}
	
	switch ( wk->eff_seq ){
	case 0:		
		for ( i = 0; i < mvwk->num; i++ ){
			mvwk->main[ i ].rad	= 0;
			mvwk->main[ i ].cx	= 128;
			mvwk->main[ i ].cy	= BB_LIGHT_CENTER;
			mvwk->main[ i ].rx	= 64;
			mvwk->main[ i ].ry	= BB_LIGHT_RY;
			CATS_ObjectEnableCap( mvwk->main[ i ].cap, TRUE );

			mvwk->sub[ i ].rad	= 0;
			mvwk->sub[ i ].cx	= 128;
			mvwk->sub[ i ].cy	= BB_LIGHT_CENTER;
			mvwk->sub[ i ].rx	= 64;
			mvwk->sub[ i ].ry	= BB_LIGHT_RY;
			CATS_ObjectEnableCap( mvwk->sub[ i ].cap, TRUE );
			
			CATS_ObjectPosGetCapFx32_SubSurface( mvwk->main[ i ].cap, &px, &py, BB_SURFACE_Y_FX );			
			ox = ( mvwk->main[ i ].cx << FX32_SHIFT ) + ( Sin360R( mvwk->main[ i ].rad ) * mvwk->main[ i ].rx );
			oy = ( mvwk->main[ i ].cy << FX32_SHIFT ) + ( Cos360R( mvwk->main[ i ].rad ) * mvwk->main[ i ].ry );			
			CATS_ObjectPosSetCapFx32_SubSurface( mvwk->main[ i ].cap, ox, oy, BB_SURFACE_Y_FX );
			
			CATS_ObjectPosGetCapFx32_SubSurface( mvwk->sub[ i ].cap, &px, &py, BB_SURFACE_Y_FX );			
			ox = ( mvwk->sub[ i ].cx << FX32_SHIFT ) + ( Sin360R( mvwk->sub[ i ].rad ) * mvwk->sub[ i ].rx );
			oy = ( mvwk->sub[ i ].cy << FX32_SHIFT ) - ( Cos360R( mvwk->sub[ i ].rad ) * mvwk->sub[ i ].ry );			
			CATS_ObjectPosSetCapFx32_SubSurface( mvwk->sub[ i ].cap, ox, oy, BB_SURFACE_Y_FX );
		}
		MoveInit( &mvwk->sub[ 0 ] );
		MoveInit( &mvwk->sub[ 1 ] );
		MoveInit( &mvwk->main[ 0 ] );
		MoveInit( &mvwk->main[ 1 ] );
		CATS_ObjectEnableCap( mvwk->main[ 1 ].cap, FALSE );
		wk->eff_seq++;
	
	case 1:		
		bEnd[ 0 ] = FrameOut( &mvwk->sub[ 0 ], mvwk->speed, BB_DIR_L, BB_DIR_D );
		bEnd[ 1 ] = FrameIn( &mvwk->sub[ 1 ], mvwk->speed, BB_DIR_L, BB_DIR_U );
		bEnd[ 2 ] = FrameInM( &mvwk->main[ 0 ], mvwk->speed_m, BB_DIR_R, BB_DIR_D );
		bEnd[ 3 ] = TRUE;
		if ( bEnd[ 0 ] && bEnd[ 1 ] && bEnd[ 2 ] && bEnd[ 3 ] ){
			MoveInit( &mvwk->sub[ 0 ] );
			MoveInit( &mvwk->sub[ 1 ] );
			MoveInit( &mvwk->main[ 0 ] );
			MoveInit( &mvwk->main[ 1 ] );
			CATS_ObjectEnableCap( mvwk->main[ 1 ].cap, TRUE );
			wk->eff_seq++;
		}
		else {
			break;
		}
	
	case 2:			
		bEnd[ 0 ] = TRUE;
		bEnd[ 1 ] = Roll( &mvwk->sub[ 1 ], mvwk->speed, BB_DIR_L, 1 );
		bEnd[ 2 ] = FrameOut( &mvwk->main[ 0 ], mvwk->speed_m, BB_DIR_R, BB_DIR_U );
		bEnd[ 3 ] = FrameInM( &mvwk->main[ 1 ], mvwk->speed_m, BB_DIR_R, BB_DIR_D );
		
		if ( bEnd[ 0 ] && bEnd[ 1 ] && bEnd[ 2 ] && bEnd[ 3 ] ){
			MoveInit( &mvwk->sub[ 0 ] );
			MoveInit( &mvwk->sub[ 1 ] );
			MoveInit( &mvwk->main[ 0 ] );
			MoveInit( &mvwk->main[ 1 ] );
			wk->eff_seq++;
		}
		else {
			break;
		}
	
	case 3:	
		bEnd[ 0 ] = FrameIn( &mvwk->sub[ 0 ], mvwk->speed, BB_DIR_L, BB_DIR_U );
		bEnd[ 1 ] = FrameOut( &mvwk->sub[ 1 ], mvwk->speed, BB_DIR_L, BB_DIR_D );
		bEnd[ 2 ] = TRUE;
		bEnd[ 3 ] = FrameOut( &mvwk->main[ 1 ], mvwk->speed_m, BB_DIR_R, BB_DIR_U );
		
		if ( bEnd[ 0 ] && bEnd[ 1 ] && bEnd[ 2 ] && bEnd[ 3 ] ){
			MoveInit( &mvwk->sub[ 0 ] );
			MoveInit( &mvwk->sub[ 1 ] );
			MoveInit( &mvwk->main[ 0 ] );
			MoveInit( &mvwk->main[ 1 ] );
			wk->eff_seq++;
		}
		else {
			break;
		}
	
	case 4:			
		bEnd[ 0 ] = Roll( &mvwk->sub[ 0 ], mvwk->speed, BB_DIR_L, 1 );
		bEnd[ 1 ] = TRUE;
		bEnd[ 2 ] = TRUE;//FrameInM( &mvwk->main[ 0 ], mvwk->speed_m, BB_DIR_R, BB_DIR_D );
		bEnd[ 3 ] = FrameOut( &mvwk->main[ 1 ], mvwk->speed_m, BB_DIR_R, BB_DIR_U );
		
		if ( bEnd[ 0 ] && bEnd[ 1 ] && bEnd[ 2 ] && bEnd[ 3 ] ){
			MoveInit( &mvwk->sub[ 0 ] );
			MoveInit( &mvwk->sub[ 1 ] );
			MoveInit( &mvwk->main[ 0 ] );
			MoveInit( &mvwk->main[ 1 ] );
			
			if ( mvwk->cnt != 1 ){
				mvwk->cnt++;
				wk->eff_seq = 1;
			}
			else {
				mvwk->cnt = 0;
				wk->eff_seq++;
			}
		}
		break;
	
	default:
//		OS_Printf( " TYPE 2 EFFECT END = %d\n", wk->wait );
		TCB_Delete( tcb );	
		break;
	}
}

static void Fever03_TCB( TCB_PTR tcb, void* work )
{
	int i;
	BB_FEVER* wk = work;
	BB_LIGHT_MOVE* mvwk = &wk->mvwk;
	BOOL bEnd[ 4 ] = { TRUE, TRUE, TRUE, TRUE };
	fx32 ox, oy;
	fx32 px, py;

	if ( (WIPE_SYS_EndCheck() == FALSE) || (wk->sys->comm_err_data.dis_err == TRUE) ){
		TCB_Delete( tcb );
		return;
	}

	switch ( wk->eff_seq ){
	case 0:		
		for ( i = 0; i < mvwk->num; i++ ){
			mvwk->main[ i ].rad	= 0;
			mvwk->main[ i ].cx	= 128;
			mvwk->main[ i ].cy	= BB_LIGHT_CENTER;
			mvwk->main[ i ].rx	= 64;
			mvwk->main[ i ].ry	= BB_LIGHT_RY;
			CATS_ObjectEnableCap( mvwk->main[ i ].cap, TRUE );

			mvwk->sub[ i ].rad	= 0;
			mvwk->sub[ i ].cx	= 128;
			mvwk->sub[ i ].cy	= BB_LIGHT_CENTER;
			mvwk->sub[ i ].rx	= 64;
			mvwk->sub[ i ].ry	= BB_LIGHT_RY;
			CATS_ObjectEnableCap( mvwk->sub[ i ].cap, TRUE );
			
			CATS_ObjectPosGetCapFx32_SubSurface( mvwk->main[ i ].cap, &px, &py, BB_SURFACE_Y_FX );			
			ox = ( mvwk->main[ i ].cx << FX32_SHIFT ) + ( Sin360R( mvwk->main[ i ].rad ) * mvwk->main[ i ].rx );
			oy = ( mvwk->main[ i ].cy << FX32_SHIFT ) + ( Cos360R( mvwk->main[ i ].rad ) * mvwk->main[ i ].ry );			
			CATS_ObjectPosSetCapFx32_SubSurface( mvwk->main[ i ].cap, ox, oy, BB_SURFACE_Y_FX );
			
			CATS_ObjectPosGetCapFx32_SubSurface( mvwk->sub[ i ].cap, &px, &py, BB_SURFACE_Y_FX );			
			ox = ( mvwk->sub[ i ].cx << FX32_SHIFT ) + ( Sin360R( mvwk->sub[ i ].rad ) * mvwk->sub[ i ].rx );
			oy = ( mvwk->sub[ i ].cy << FX32_SHIFT ) - ( Cos360R( mvwk->sub[ i ].rad ) * mvwk->sub[ i ].ry );			
			CATS_ObjectPosSetCapFx32_SubSurface( mvwk->sub[ i ].cap, ox, oy, BB_SURFACE_Y_FX );
		}
		MoveInit( &mvwk->sub[ 0 ] );
		MoveInit( &mvwk->sub[ 1 ] );
		MoveInit( &mvwk->main[ 0 ] );
		MoveInit( &mvwk->main[ 1 ] );
		CATS_ObjectEnableCap( mvwk->main[ 1 ].cap, FALSE );
		wk->eff_seq++;
	
	case 1:		
		bEnd[ 0 ] = FrameOut( &mvwk->sub[ 0 ], mvwk->speed, BB_DIR_L, BB_DIR_D );
		bEnd[ 1 ] = FrameIn( &mvwk->sub[ 1 ], mvwk->speed, BB_DIR_L, BB_DIR_U );
		bEnd[ 2 ] = FrameInM( &mvwk->main[ 0 ], mvwk->speed_m, BB_DIR_R, BB_DIR_D );
		bEnd[ 3 ] = TRUE;
		if ( bEnd[ 0 ] && bEnd[ 1 ] && bEnd[ 2 ] && bEnd[ 3 ] ){
			MoveInit( &mvwk->sub[ 0 ] );
			MoveInit( &mvwk->sub[ 1 ] );
			MoveInit( &mvwk->main[ 0 ] );
			MoveInit( &mvwk->main[ 1 ] );
			CATS_ObjectEnableCap( mvwk->main[ 1 ].cap, TRUE );
			wk->eff_seq++;
		}
		else {
			break;
		}
	
	case 2:			
		bEnd[ 0 ] = TRUE;
		bEnd[ 1 ] = Roll( &mvwk->sub[ 1 ], mvwk->speed, BB_DIR_L, 2 );
		bEnd[ 2 ] = FrameOut( &mvwk->main[ 0 ], mvwk->speed_m, BB_DIR_R, BB_DIR_U );
		bEnd[ 3 ] = FrameInM( &mvwk->main[ 1 ], mvwk->speed_m, BB_DIR_R, BB_DIR_D );
		
		if ( bEnd[ 0 ] && bEnd[ 1 ] && bEnd[ 2 ] && bEnd[ 3 ] ){
			MoveInit( &mvwk->sub[ 0 ] );
			MoveInit( &mvwk->sub[ 1 ] );
			MoveInit( &mvwk->main[ 0 ] );
			MoveInit( &mvwk->main[ 1 ] );
			wk->eff_seq++;
		}
		else {
			break;
		}
	
	case 3:	
		bEnd[ 0 ] = FrameIn( &mvwk->sub[ 0 ], mvwk->speed, BB_DIR_L, BB_DIR_U );
		bEnd[ 1 ] = FrameOut( &mvwk->sub[ 1 ], mvwk->speed, BB_DIR_L, BB_DIR_D );
		bEnd[ 2 ] = TRUE;
		bEnd[ 3 ] = FrameOut( &mvwk->main[ 1 ], mvwk->speed_m, BB_DIR_R, BB_DIR_U );
		
		if ( bEnd[ 0 ] && bEnd[ 1 ] && bEnd[ 2 ] && bEnd[ 3 ] ){
			MoveInit( &mvwk->sub[ 0 ] );
			MoveInit( &mvwk->sub[ 1 ] );
			MoveInit( &mvwk->main[ 0 ] );
			MoveInit( &mvwk->main[ 1 ] );
			wk->eff_seq++;
		}
		else {
			break;
		}
	
	case 4:			
		bEnd[ 0 ] = Roll( &mvwk->sub[ 0 ], mvwk->speed, BB_DIR_L, 2 );
		bEnd[ 1 ] = TRUE;
		bEnd[ 2 ] = TRUE;//FrameInM( &mvwk->main[ 0 ], mvwk->speed_m, BB_DIR_R, BB_DIR_D );
		bEnd[ 3 ] = FrameOut( &mvwk->main[ 1 ], mvwk->speed_m, BB_DIR_R, BB_DIR_U );
		
		if ( bEnd[ 0 ] && bEnd[ 1 ] && bEnd[ 2 ] && bEnd[ 3 ] ){
			MoveInit( &mvwk->sub[ 0 ] );
			MoveInit( &mvwk->sub[ 1 ] );
			MoveInit( &mvwk->main[ 0 ] );
			MoveInit( &mvwk->main[ 1 ] );
			
			if ( mvwk->cnt != 1 ){
				mvwk->cnt++;
				wk->eff_seq = 1;
			}
			else {
				mvwk->cnt = 0;
				wk->eff_seq++;
			}
		}
		break;
	
	default:
//		OS_Printf( " TYPE 3 EFFECT END = %d\n", wk->wait );
		TCB_Delete( tcb );	
		break;
	}
}

static void Fever04_TCB( TCB_PTR tcb, void* work )
{
	int i;
	BB_FEVER* wk = work;
	BB_LIGHT_MOVE* mvwk = &wk->mvwk;
	BOOL bEnd[ 4 ] = { TRUE, TRUE, TRUE, TRUE };
	fx32 ox, oy;
	fx32 px, py;

	if ( (WIPE_SYS_EndCheck() == FALSE) || (wk->sys->comm_err_data.dis_err == TRUE) ){
		TCB_Delete( tcb );
		return;
	}

	switch ( wk->eff_seq ){
	case 0:		
		for ( i = 0; i < mvwk->num; i++ ){
			mvwk->main[ i ].rad	= 0;
			mvwk->main[ i ].cx	= 128;
			mvwk->main[ i ].cy	= BB_LIGHT_CENTER;
			mvwk->main[ i ].rx	= 64;
			mvwk->main[ i ].ry	= BB_LIGHT_RY;
			CATS_ObjectEnableCap( mvwk->main[ i ].cap, TRUE );

			mvwk->sub[ i ].rad	= 0;
			mvwk->sub[ i ].cx	= 128;
			mvwk->sub[ i ].cy	= BB_LIGHT_CENTER;
			mvwk->sub[ i ].rx	= 64;
			mvwk->sub[ i ].ry	= BB_LIGHT_RY;
			CATS_ObjectEnableCap( mvwk->sub[ i ].cap, TRUE );
			
			CATS_ObjectPosGetCapFx32_SubSurface( mvwk->main[ i ].cap, &px, &py, BB_SURFACE_Y_FX );			
			ox = ( mvwk->main[ i ].cx << FX32_SHIFT ) + ( Sin360R( mvwk->main[ i ].rad ) * mvwk->main[ i ].rx );
			oy = ( mvwk->main[ i ].cy << FX32_SHIFT ) + ( Cos360R( mvwk->main[ i ].rad ) * mvwk->main[ i ].ry );			
			CATS_ObjectPosSetCapFx32_SubSurface( mvwk->main[ i ].cap, ox, oy, BB_SURFACE_Y_FX );
			
			CATS_ObjectPosGetCapFx32_SubSurface( mvwk->sub[ i ].cap, &px, &py, BB_SURFACE_Y_FX );			
			ox = ( mvwk->sub[ i ].cx << FX32_SHIFT ) + ( Sin360R( mvwk->sub[ i ].rad ) * mvwk->sub[ i ].rx );
			oy = ( mvwk->sub[ i ].cy << FX32_SHIFT ) - ( Cos360R( mvwk->sub[ i ].rad ) * mvwk->sub[ i ].ry );			
			CATS_ObjectPosSetCapFx32_SubSurface( mvwk->sub[ i ].cap, ox, oy, BB_SURFACE_Y_FX );
		}
		MoveInit( &mvwk->sub[ 0 ] );
		MoveInit( &mvwk->sub[ 1 ] );
		MoveInit( &mvwk->main[ 0 ] );
		MoveInit( &mvwk->main[ 1 ] );
		CATS_ObjectEnableCap( mvwk->main[ 1 ].cap, FALSE );
		wk->eff_seq++;
	
	case 1:		
		bEnd[ 0 ] = FrameOut( &mvwk->sub[ 0 ], mvwk->speed, BB_DIR_L, BB_DIR_D );
		bEnd[ 1 ] = FrameIn( &mvwk->sub[ 1 ], mvwk->speed, BB_DIR_L, BB_DIR_U );
		bEnd[ 2 ] = FrameInM( &mvwk->main[ 0 ], mvwk->speed_m, BB_DIR_R, BB_DIR_D );
		bEnd[ 3 ] = TRUE;
		if ( bEnd[ 0 ] && bEnd[ 1 ] && bEnd[ 2 ] && bEnd[ 3 ] ){
			MoveInit( &mvwk->sub[ 0 ] );
			MoveInit( &mvwk->sub[ 1 ] );
			MoveInit( &mvwk->main[ 0 ] );
			MoveInit( &mvwk->main[ 1 ] );
			CATS_ObjectEnableCap( mvwk->main[ 1 ].cap, TRUE );
			wk->eff_seq++;
		}
		else {
			break;
		}
	
	case 2:			
		bEnd[ 0 ] = TRUE;
		bEnd[ 1 ] = Roll( &mvwk->sub[ 1 ], mvwk->speed, BB_DIR_L, 1 );
		bEnd[ 2 ] = FrameOut( &mvwk->main[ 0 ], mvwk->speed_m, BB_DIR_R, BB_DIR_U );
		bEnd[ 3 ] = FrameInM( &mvwk->main[ 1 ], mvwk->speed_m, BB_DIR_R, BB_DIR_D );
		
		if ( bEnd[ 0 ] && bEnd[ 1 ] && bEnd[ 2 ] && bEnd[ 3 ] ){
			MoveInit( &mvwk->sub[ 0 ] );
			MoveInit( &mvwk->sub[ 1 ] );
			MoveInit( &mvwk->main[ 0 ] );
			MoveInit( &mvwk->main[ 1 ] );
			wk->eff_seq++;
		}
		else {
			break;
		}
	
	case 3:
		bEnd[ 1 ] = Roll( &mvwk->sub[ 1 ], mvwk->speed, BB_DIR_R, 1 );
		if ( bEnd[ 1 ] ){
			MoveInit( &mvwk->sub[ 1 ] );
			wk->eff_seq++;
		}
		else {
			break;
		}
	
	case 4:	
		bEnd[ 0 ] = FrameIn( &mvwk->sub[ 0 ], mvwk->speed, BB_DIR_L, BB_DIR_U );
		bEnd[ 1 ] = FrameOut( &mvwk->sub[ 1 ], mvwk->speed, BB_DIR_L, BB_DIR_D );
		bEnd[ 2 ] = TRUE;
		bEnd[ 3 ] = FrameOut( &mvwk->main[ 1 ], mvwk->speed_m, BB_DIR_R, BB_DIR_U );
		
		if ( bEnd[ 0 ] && bEnd[ 1 ] && bEnd[ 2 ] && bEnd[ 3 ] ){
			MoveInit( &mvwk->sub[ 0 ] );
			MoveInit( &mvwk->sub[ 1 ] );
			MoveInit( &mvwk->main[ 0 ] );
			MoveInit( &mvwk->main[ 1 ] );
			wk->eff_seq++;
		}
		else {
			break;
		}
	
	case 5:			
		bEnd[ 0 ] = Roll( &mvwk->sub[ 0 ], mvwk->speed, BB_DIR_L, 1 );
		bEnd[ 1 ] = TRUE;
		bEnd[ 2 ] = TRUE;//FrameInM( &mvwk->main[ 0 ], mvwk->speed_m, BB_DIR_R, BB_DIR_D );
		bEnd[ 3 ] = FrameOut( &mvwk->main[ 1 ], mvwk->speed_m, BB_DIR_R, BB_DIR_U );
		
		if ( bEnd[ 0 ] && bEnd[ 1 ] && bEnd[ 2 ] && bEnd[ 3 ] ){
			MoveInit( &mvwk->sub[ 0 ] );
			MoveInit( &mvwk->sub[ 1 ] );
			MoveInit( &mvwk->main[ 0 ] );
			MoveInit( &mvwk->main[ 1 ] );
			wk->eff_seq++;
		}
		else {
			break;
		}
	
	case 6:
		bEnd[ 0 ] = Roll( &mvwk->sub[ 0 ], mvwk->speed, BB_DIR_R, 1 );		
		if ( bEnd[ 0 ] ){
			MoveInit( &mvwk->sub[ 0 ] );
			if ( mvwk->cnt != 1 ){
				mvwk->cnt++;
				wk->eff_seq = 1;
			}
			else {
				mvwk->cnt = 0;
				wk->eff_seq++;
			}
		}
		break;
	
	default:
//		OS_Printf( " TYPE 4 EFFECT END = %d\n", wk->wait );
		TCB_Delete( tcb );	
		break;
	}
}

static void Fever05_TCB( TCB_PTR tcb, void* work )
{
	int i;
	BB_FEVER* wk = work;
	BB_LIGHT_MOVE* mvwk = &wk->mvwk;
	BOOL bEnd[ 4 ] = { TRUE, TRUE, TRUE, TRUE };
	fx32 ox, oy;
	fx32 px, py;

	if ( (WIPE_SYS_EndCheck() == FALSE) || (wk->sys->comm_err_data.dis_err == TRUE) ){
		TCB_Delete( tcb );
		return;
	}

	switch ( wk->eff_seq ){
	case 0:		
		for ( i = 0; i < mvwk->num; i++ ){
			mvwk->main[ i ].rad	= 0;
			mvwk->main[ i ].cx	= 128;
			mvwk->main[ i ].cy	= BB_LIGHT_CENTER;
			mvwk->main[ i ].rx	= 64;
			mvwk->main[ i ].ry	= BB_LIGHT_RY;
			CATS_ObjectEnableCap( mvwk->main[ i ].cap, TRUE );

			mvwk->sub[ i ].rad	= 0;
			mvwk->sub[ i ].cx	= 128;
			mvwk->sub[ i ].cy	= BB_LIGHT_CENTER;
			mvwk->sub[ i ].rx	= 64;
			mvwk->sub[ i ].ry	= BB_LIGHT_RY;
			CATS_ObjectEnableCap( mvwk->sub[ i ].cap, TRUE );
			
			CATS_ObjectPosGetCapFx32_SubSurface( mvwk->main[ i ].cap, &px, &py, BB_SURFACE_Y_FX );			
			ox = ( mvwk->main[ i ].cx << FX32_SHIFT ) + ( Sin360R( mvwk->main[ i ].rad ) * mvwk->main[ i ].rx );
			oy = ( mvwk->main[ i ].cy << FX32_SHIFT ) + ( Cos360R( mvwk->main[ i ].rad ) * mvwk->main[ i ].ry );			
			CATS_ObjectPosSetCapFx32_SubSurface( mvwk->main[ i ].cap, ox, oy, BB_SURFACE_Y_FX );
			
			CATS_ObjectPosGetCapFx32_SubSurface( mvwk->sub[ i ].cap, &px, &py, BB_SURFACE_Y_FX );			
			ox = ( mvwk->sub[ i ].cx << FX32_SHIFT ) + ( Sin360R( mvwk->sub[ i ].rad ) * mvwk->sub[ i ].rx );
			oy = ( mvwk->sub[ i ].cy << FX32_SHIFT ) - ( Cos360R( mvwk->sub[ i ].rad ) * mvwk->sub[ i ].ry );			
			CATS_ObjectPosSetCapFx32_SubSurface( mvwk->sub[ i ].cap, ox, oy, BB_SURFACE_Y_FX );
		}
		OS_Printf( " こここ\n" );
		MoveInit( &mvwk->sub[ 0 ] );
		MoveInit( &mvwk->sub[ 1 ] );
		MoveInit( &mvwk->main[ 0 ] );
		MoveInit( &mvwk->main[ 1 ] );
		CATS_ObjectEnableCap( mvwk->main[ 1 ].cap, FALSE );
		wk->eff_seq++;
	
	case 1:		
		bEnd[ 0 ] = FrameOut( &mvwk->sub[ 0 ], mvwk->speed, BB_DIR_L, BB_DIR_D );
		bEnd[ 1 ] = FrameIn( &mvwk->sub[ 1 ], mvwk->speed, BB_DIR_L, BB_DIR_U );
		bEnd[ 2 ] = FrameInM( &mvwk->main[ 0 ], mvwk->speed_m, BB_DIR_R, BB_DIR_D );
		bEnd[ 3 ] = TRUE;
		if ( bEnd[ 0 ] && bEnd[ 1 ] && bEnd[ 2 ] && bEnd[ 3 ] ){
			MoveInit( &mvwk->sub[ 0 ] );
			MoveInit( &mvwk->sub[ 1 ] );
			MoveInit( &mvwk->main[ 0 ] );
			MoveInit( &mvwk->main[ 1 ] );
			CATS_ObjectEnableCap( mvwk->main[ 1 ].cap, TRUE );
			wk->eff_seq++;
		}
		else {
			break;
		}
	
	case 2:			
		bEnd[ 0 ] = TRUE;
		bEnd[ 1 ] = Roll( &mvwk->sub[ 1 ], mvwk->speed, BB_DIR_L, 1 );
		bEnd[ 2 ] = FrameOut( &mvwk->main[ 0 ], mvwk->speed_m, BB_DIR_R, BB_DIR_U );
		bEnd[ 3 ] = FrameInM( &mvwk->main[ 1 ], mvwk->speed_m, BB_DIR_R, BB_DIR_D );
		
		if ( bEnd[ 0 ] && bEnd[ 1 ] && bEnd[ 2 ] && bEnd[ 3 ] ){
			MoveInit( &mvwk->sub[ 0 ] );
			MoveInit( &mvwk->sub[ 1 ] );
			MoveInit( &mvwk->main[ 0 ] );
			MoveInit( &mvwk->main[ 1 ] );
			wk->eff_seq++;
		}
		else {
			break;
		}
	
	case 3:
		bEnd[ 1 ] = Roll( &mvwk->sub[ 1 ], mvwk->speed, BB_DIR_R, 1 );
		if ( bEnd[ 1 ] ){
			MoveInit( &mvwk->sub[ 1 ] );
			wk->eff_seq++;
		}
		else {
			break;
		}
	
	case 4:	
		bEnd[ 0 ] = FrameIn( &mvwk->sub[ 0 ], mvwk->speed, BB_DIR_L, BB_DIR_U );
		bEnd[ 1 ] = FrameOut( &mvwk->sub[ 1 ], mvwk->speed, BB_DIR_L, BB_DIR_D );
		bEnd[ 2 ] = TRUE;
		bEnd[ 3 ] = FrameOut( &mvwk->main[ 1 ], mvwk->speed_m, BB_DIR_R, BB_DIR_U );
		
		if ( bEnd[ 0 ] && bEnd[ 1 ] && bEnd[ 2 ] && bEnd[ 3 ] ){
			MoveInit( &mvwk->sub[ 0 ] );
			MoveInit( &mvwk->sub[ 1 ] );
			MoveInit( &mvwk->main[ 0 ] );
			MoveInit( &mvwk->main[ 1 ] );
			wk->eff_seq++;
		}
		else {
			break;
		}
	
	case 5:			
		bEnd[ 0 ] = Roll( &mvwk->sub[ 0 ], mvwk->speed, BB_DIR_L, 1 );
		bEnd[ 1 ] = TRUE;
		bEnd[ 2 ] = TRUE;//FrameInM( &mvwk->main[ 0 ], mvwk->speed_m, BB_DIR_R, BB_DIR_D );
		bEnd[ 3 ] = FrameOut( &mvwk->main[ 1 ], mvwk->speed_m, BB_DIR_R, BB_DIR_U );
		
		if ( bEnd[ 0 ] && bEnd[ 1 ] && bEnd[ 2 ] && bEnd[ 3 ] ){
			MoveInit( &mvwk->sub[ 0 ] );
			MoveInit( &mvwk->sub[ 1 ] );
			MoveInit( &mvwk->main[ 0 ] );
			MoveInit( &mvwk->main[ 1 ] );
			wk->eff_seq++;
		}
		else {
			break;
		}
	
	case 6:
		bEnd[ 0 ] = Roll( &mvwk->sub[ 0 ], mvwk->speed, BB_DIR_R, 1 );		
		if ( bEnd[ 0 ] ){
			MoveInit( &mvwk->sub[ 0 ] );
			if ( mvwk->cnt != 8 ){
				mvwk->cnt++;
				wk->eff_seq = 1;
			}
			else {
				mvwk->cnt = 0;
				wk->eff_seq++;
			}
		}
		break;
	
	default:
//		OS_Printf( " TYPE 4 EFFECT END = %d\n", wk->wait );
		TCB_Delete( tcb );	
		break;
	}
}

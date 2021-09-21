//==============================================================================
/**
 * @file	bb_client.c
 * @brief	簡単な説明を書く
 * @author	goto
 * @date	2007.10.01(月)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================

#include "common.h"
#include "bb_client.h"
#include "bb_comm_cmd.h"

#include "bb_data.dat"

//==============================================================
// Prototype
//==============================================================
static void BB_TP_Check( u32* p, s16 mv, u32* old, u32 now );
static void BB_Client_Manene_Action_Call( BB_CLIENT* wk, int netid );
static void BB_Client_LevelUp_Action_Call( BB_CLIENT* wk, int netid );
static void BB_Client_3D_LevelUp_Action_Call( BB_CLIENT* wk, int netid );
static void BB_Manene_3D_Fall_Call( BB_CLIENT* wk );
static void BB_Client_Manene_3D_RecoverAction_Call( BB_CLIENT* wk );
static void BB_Client_LevelUp_Action_TCB( TCB_PTR tcb, void* work );
static void BB_Client_3D_LevelUp_Action_TCB( TCB_PTR tcb, void* work );
static void BB_Client_ScoreUpMain( BB_CLIENT* wk );
static void BB_Client_BonusPoint( BB_CLIENT* wk );
static BOOL BB_Client_LevelUpMain( BB_CLIENT* wk );
static BOOL BB_Client_LevelUpCheck( BB_CLIENT* wk, int time, int* level );
static void BB_Client_OtherUpdateMain( BB_CLIENT* wk );
static void BB_Client_MyUpdateMain( BB_CLIENT* wk );
static inline u32 BB_Rand( void );
static inline s8 BB_inline_GetSign( void );
static inline s8 BB_inline_GetSign_to_Value( s16 val );

s16 Action_MoveValue_3Y( void )
{
	int i;
	s16 y = 0;
	
	for ( i = 0; i < NELEMS( action_3d_y_tbl ); i++ ){
		
		if ( action_3d_y_tbl[ i ] == SKIP_CODE_1
		||	 action_3d_y_tbl[ i ] == SKIP_CODE_2 ){ continue; }
		
		y += action_3d_y_tbl[ i ];
	}
	
	return y;
}

s16 Action_MoveValue_3Z( void )
{
	int i;
	s16 z = 0;
	
	for ( i = 0; i < NELEMS( action_3d_z_tbl ); i++ ){
		
		if ( action_3d_z_tbl[ i ] == SKIP_CODE_1
		||	 action_3d_z_tbl[ i ] == SKIP_CODE_2 ){ continue; }
		
		z += action_3d_z_tbl[ i ];
	}
	
	return z;
}

s16 Action_MoveValue_2Y( void )
{
	int i;
	s16 y = 0;
	
	for ( i = 0; i < NELEMS( action_2d_y_tbl ); i++ ){
		
		y += action_2d_y_tbl[ i ];
	}
	
	return y;
}

//--------------------------------------------------------------
/**
 * @brief	メモリ確保
 *
 * @param	comm_num	
 * @param	netid	
 *
 * @retval	BB_CLIENT*	
 *
 */
//--------------------------------------------------------------
BB_CLIENT* BB_Client_AllocMemory( int comm_num, u32 netid, BB_SYS* sys )
{
	BB_CLIENT* wk = sys_AllocMemory( HEAPID_BB, sizeof( BB_CLIENT ) );
	
	memset( wk, 0, sizeof( BB_CLIENT ) );
	
	wk->comm_num = comm_num;
	wk->netid	 = netid;
	wk->seq		 = 0;
	wk->seq2	 = 0;
	wk->sys		 = sys;
	wk->bAction	 = FALSE;

	///< 個人エフェクト管理ワーク作成
	{
		wk->eff_sys.old_level = wk->game_sys.level;
		wk->eff_sys.level	  = &wk->game_sys.level;
	}	
	
	wk->game_sys.bFirstTouch = FALSE;
	
	BB_Client_3D_PosInit( wk );
	
	return wk;
}


//--------------------------------------------------------------
/**
 * @brief	3D初期化
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_Client_3D_PosInit( BB_CLIENT* wk )
{
	int i;

	wk->mane_pos.x = 0;
	wk->mane_pos.y = BB_POINT_Y;
	wk->mane_pos.z = 0;
	
    wk->bb3d_mane[ 0 ].get_pow_x = 0.0f;
    wk->bb3d_mane[ 0 ].get_pow_y = 0.0f;
    wk->bb3d_ball.get_pow_x = 0.0f;
    wk->bb3d_ball.get_pow_y = 0.0f;    
    wk->bb3d_mane[ 0 ].pow_count = 0;
    wk->bb3d_ball.pow_count = 0;

	Quaternion_Identity( &wk->bb3d_ball.tq );
	Quaternion_Identity( &wk->bb3d_ball.cq );
	Quaternion_Identity( &wk->bb3d_mane[ 0 ].tq );
	Quaternion_Identity( &wk->bb3d_mane[ 0 ].cq );
	Quaternion_Identity( &wk->bb3d_spot.tq );
	Quaternion_Identity( &wk->bb3d_spot.cq );

	for ( i = 0; i < BB_KAMI_HUBUKI_MAX; i++ ){
		Quaternion_Identity( &wk->bb3d_kami[ i ].tq );
		Quaternion_Identity( &wk->bb3d_kami[ i ].cq );
		Quaternion_Rotation( &wk->bb3d_kami[ i ], 0, 0, +5, +5, BB_COEFFICIENT_AMI, FALSE );
		Quaternion_Rotation( &wk->bb3d_kami[ i ], 0, 0, -5, -5, BB_COEFFICIENT_AMI, FALSE );	
	}

	Quaternion_Rotation( &wk->bb3d_ball, 0, 0, +5, +5, BB_COEFFICIENT_AB, FALSE );
	Quaternion_Rotation( &wk->bb3d_ball, 0, 0, -5, -5, BB_COEFFICIENT_AB, FALSE );
	Quaternion_Rotation( &wk->bb3d_mane[ 0 ], 0, 0, +5, +5, BB_COEFFICIENT_AMI, FALSE );
	Quaternion_Rotation( &wk->bb3d_mane[ 0 ], 0, 0, -5, -5, BB_COEFFICIENT_AMI, FALSE );	
	Quaternion_Rotation( &wk->bb3d_spot, 0, 0, +5, +5, BB_COEFFICIENT_AB, FALSE );
	Quaternion_Rotation( &wk->bb3d_spot, 0, 0, -5, -5, BB_COEFFICIENT_AMI, FALSE );
}


//--------------------------------------------------------------
/**
 * @brief	メモリ解放
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_Client_FreeMemory( BB_CLIENT* wk )
{
	sys_FreeMemoryEz( wk );
}


//--------------------------------------------------------------
/**
 * @brief	Touch panel Call Back
 *
 * @param	button	
 * @param	event	
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BB_TP_Check( u32* p, s16 mv, u32* old, u32 now )
{
	if ( mv * mv < 64 ){ return; }
	
	*old = now;
	
	if ( *p + mv < 0 ){
		*p = 360;
	}
	else {
		*p = *p + mv;
	}
}


//--------------------------------------------------------------
/**
 * @brief	位置から落ちるかどうかの判定をする
 *
 * @param	wk	
 * @param	*param	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL BB_Client_IsSafety( BB_CLIENT* wk, int* state )
{
	int pos;
	int check;
	
	pos		= wk->mane_pos.y >> FX32_SHIFT;
	check	= BB_POINT_FALL  >> FX32_SHIFT;
	
	if ( pos <= check ){
		
		return TRUE;
	}
	
	return FALSE;	
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
void BB_ManeneAnime_CheckSet( BB_CLIENT* wk ){
	
	int pos;
	int check1;
	int check2;
	pos	= wk->mane_pos.y >> FX32_SHIFT;	
	
	///< あららanime
	check1 = BB_POINT_1S >> FX32_SHIFT;	
	check2 = BB_POINT_1E >> FX32_SHIFT;
	if ( pos < check1 && pos > check2 ){
		ManeneAnime_Set( wk, ePAT_3D_ARARA );
		return;
	}	
	
	///< おっとっとanime
	check1 = BB_POINT_2S >> FX32_SHIFT;	
	check2 = BB_POINT_2E >> FX32_SHIFT;
	if ( pos < check1 && pos > check2 ){
		ManeneAnime_Set( wk, ePAT_3D_OTTOTTO );
		return;
	}
	ManeneAnime_Set( wk, ePAT_3D_ARUKU );
}


//--------------------------------------------------------------
/**
 * @brief	マネネアクション呼び出し
 *
 * @param	wk	
 * @param	netid	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BB_Client_Manene_Action_Call( BB_CLIENT* wk, int netid )
{
	BB_MANENE_ACTION* sub_wk = sys_AllocMemory( HEAPID_BB, sizeof( BB_MANENE_ACTION ) );
	
	memset( sub_wk, 0, sizeof( BB_MANENE_ACTION ) );
	
	sub_wk->cap_mane = wk->cap_mane[ wk->netid_to_capid[ netid ] ];
	sub_wk->cap_ball = wk->cap_ball[ wk->netid_to_capid[ netid ] ];
	sub_wk->seq = 0;
	sub_wk->type = 0;

	sub_wk->sys = wk->sys;	//  切断終了のため追加	tomoya 

	TCB_Add( BB_Client_Manene_Action_TCB, sub_wk, BB_TCB_PRI_1 );
}


//--------------------------------------------------------------
/**
 * @brief	レベルアップ呼び出し
 *
 * @param	wk	
 * @param	netid	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BB_Client_LevelUp_Action_Call( BB_CLIENT* wk, int netid )
{
	BB_LEVEL_UP* sub_wk;

	sub_wk = &wk->level_sys[ netid ];
	
	if ( sub_wk->bStart == TRUE ){ return; }

	sub_wk->rare_game  = wk->rare_game;
	sub_wk->bStart = TRUE;
	sub_wk->level = &wk->game_sys.o_level[ netid ];
	sub_wk->old_level = *sub_wk->level;
	sub_wk->cap_kage = wk->cap_kage[ wk->netid_to_capid[ netid ] ];
	sub_wk->seq = 0;
	sub_wk->type = 0;

	sub_wk->sys = wk->sys;	// 切断エラーチェック用
	
	TCB_Add( BB_Client_LevelUp_Action_TCB, sub_wk, BB_TCB_PRI_1 );
}

static void BB_Client_3D_LevelUp_Action_Call( BB_CLIENT* wk, int netid )
{
	BB_LEVEL_UP* sub_wk;

	sub_wk = &wk->level_sys[ netid ];
	
	if ( sub_wk->bStart == TRUE ){ return; }

	sub_wk->rare_game  = wk->rare_game;
	sub_wk->bStart = TRUE;
	sub_wk->level = &wk->game_sys.level;
	sub_wk->old_level = *sub_wk->level;
	sub_wk->model_bs = &wk->bb3d_lvbs;
	sub_wk->model_up = &wk->bb3d_lvup;
	sub_wk->seq = 0;
	sub_wk->type = 0;	
	sub_wk->sys	= wk->sys;
	TCB_Add( BB_Client_3D_LevelUp_Action_TCB, sub_wk, BB_TCB_PRI_1 );
}

//--------------------------------------------------------------
/**
 * @brief	マネネアクション呼び出し
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BB_Manene_3D_Fall_Call( BB_CLIENT* wk )
{
	BB_MANENE_3D_ACTION* sub_wk = sys_AllocMemory( HEAPID_BB, sizeof( BB_MANENE_3D_ACTION ) );

	memset( sub_wk, 0, sizeof( BB_MANENE_3D_ACTION ) );

	sub_wk->anime_type = &wk->anime_type;
	sub_wk->model	   = &wk->bb3d_mane[ 0 ];
	sub_wk->model_ball = &wk->bb3d_ball;
	sub_wk->mane_pos   = wk->mane_pos;
	sub_wk->rare_game  = wk->rare_game;

	sub_wk->sys = wk->sys;	//  切断終了のため追加	tomoya 
	
	ManeneAnime_Set( wk, ePAT_3D_ARUKU );
	
	///< 落下と復帰のanimeを初期化しておく
	D3DOBJ_AnmSet( &wk->bb3d_mane[ 0 ].anm[ 0 ], 0 );
	D3DOBJ_AnmSet( &wk->bb3d_mane[ 1 ].anm[ 0 ], 0 );
	D3DOBJ_AnmSet( &wk->bb3d_mane[ 2 ].anm[ 0 ], 0 );
	
	TCB_Add( BB_Manene_3D_Fall_TCB, sub_wk, BB_TCB_PRI_1 );
}


//--------------------------------------------------------------
/**
 * @brief	復帰アニメ
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BB_Client_Manene_3D_RecoverAction_Call( BB_CLIENT* wk )
{
	BB_MANENE_3D_ACTION* sub_wk = sys_AllocMemory( HEAPID_BB, sizeof( BB_MANENE_3D_ACTION ) );
	
	memset( sub_wk, 0, sizeof( BB_MANENE_3D_ACTION ) );
	
	sub_wk->model	   = &wk->bb3d_mane[ 0 ];
	sub_wk->model_ball = &wk->bb3d_ball;
	sub_wk->mane_pos   = wk->mane_pos;
	sub_wk->rare_game  = wk->rare_game;

	sub_wk->sys = wk->sys;	//  切断終了のため追加	tomoya 
	
	TCB_Add( BB_Client_Manene_3D_Recover_Action_TCB, sub_wk, BB_TCB_PRI_1 );
}


//--------------------------------------------------------------
/**
 * @brief	マネネアクション3D　落ちて戻るＴＣＢ
 *
 * @param	tcb	
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_Manene_3D_Fall_TCB( TCB_PTR tcb, void* work )
{
	BB_MANENE_3D_ACTION* wk = work;	

	s16 pos_y;
	s16 pos_z;
	s16 py = wk->model->pos.y >> FX32_SHIFT;
	s16 pz = wk->model->pos.z >> FX32_SHIFT;
	s16 check_z = wk->mane_pos.z >> FX32_SHIFT;
	fx32 z_value;
	fx32 y_value1;
	fx32 y_value2;
	
	if ( wk->rare_game ){
	//	z_value = FX32_CONST( 36 );
	//	y_value1 = FX32_CONST( 20 );
	//	y_value2 = FX32_CONST( 56 );
		z_value = FX32_CONST( 16 );
		y_value1 = FX32_CONST( 12 );
		y_value2 = FX32_CONST( 48 );
	}
	else {
		z_value = FX32_CONST( 16 );
		y_value1 = FX32_CONST( 12 );
		y_value2 = FX32_CONST( 48 );
	}
	
	if ( (WIPE_SYS_EndCheck() == FALSE) || (wk->sys->comm_err_data.dis_err == TRUE) ){
		TCB_Delete( tcb );
		sys_FreeMemoryEz( wk );
		return;
	}
	
	switch ( wk->seq ){
	case 0:
		if ( wk->type == 0 ){
			BB_MoveInit_FX( &wk->dy, wk->model->pos.y, wk->model->pos.y + y_value1, 4 );
			if ( check_z > 0 ){
				BB_MoveInit_FX( &wk->dz, wk->model->pos.z, wk->model->pos.z + z_value, 4 );
			}
			else {
				BB_MoveInit_FX( &wk->dz, wk->model->pos.z, wk->model->pos.z - z_value, 4 );
			}
		}
		else {
			BB_MoveInit_FX( &wk->dy, wk->model->pos.y, wk->model->pos.y + y_value2, 8 );
			if ( check_z > 0 ){
				BB_MoveInit_FX( &wk->dz, wk->model->pos.z, wk->model->pos.z - z_value, 8 );
			}
			else {
				BB_MoveInit_FX( &wk->dz, wk->model->pos.z, wk->model->pos.z + z_value, 4 );
			}
		}
		Snd_SePlay( BB_SND_JUMP_OUT );	///< 落ちる音
		wk->wait = 0;
		wk->seq++;
		break;
	
	case 1:		
		{
			BOOL bEnd[ 2 ];
			bEnd[ 0 ] = BB_MoveMain_FX( &wk->dy );
			bEnd[ 1 ] = BB_MoveMain_FX( &wk->dz );			
			wk->model->pos.y = wk->dy.x;
			wk->model->pos.z = wk->dz.x;			
			if ( bEnd[ 0 ] && bEnd[ 1 ] ){				
				if ( wk->type == 0 ){
					*wk->anime_type = eANM_CODE_FALL;
					Quaternion_Identity( &wk->model->tq );
					Quaternion_Identity( &wk->model->cq );
					Quaternion_Rotation( wk->model, 0, 0, +5, +5, BB_COEFFICIENT_AMI, FALSE );
					Quaternion_Rotation( wk->model, 0, 0, -5, -5, BB_COEFFICIENT_AMI, FALSE );
					BB_MoveInit_FX( &wk->dy, wk->model->pos.y, wk->model->pos.y - y_value2, 8 );
					if ( check_z > 0 ){
						BB_MoveInit_FX( &wk->dz, wk->model->pos.z, wk->model->pos.z + z_value, 8 );
					}
					else {
						BB_MoveInit_FX( &wk->dz, wk->model->pos.z, wk->model->pos.z - z_value, 8 );
					}
				}
				else {
					BB_MoveInit_FX( &wk->dy, wk->model->pos.y, wk->model->pos.y - y_value1, 4 );
					if ( check_z > 0 ){
						BB_MoveInit_FX( &wk->dz, wk->model->pos.z, wk->model->pos.z - z_value, 4 );
					}
					else {
						BB_MoveInit_FX( &wk->dz, wk->model->pos.z, wk->model->pos.z + z_value, 4 );
					}
				}				
				wk->seq++;
			}
		}
		break;
		
	case 2:		
		{
			BOOL bEnd[ 2 ];
			bEnd[ 0 ] = BB_MoveMain_FX( &wk->dy );
			bEnd[ 1 ] = BB_MoveMain_FX( &wk->dz );			
			wk->model->pos.y = wk->dy.x;
			wk->model->pos.z = wk->dz.x;			
			if ( bEnd[ 0 ] && bEnd[ 1 ] ){
				if ( wk->type == 0 ){
					Snd_SePlay( BB_SND_LANDING );	///< 着地
				//	*wk->anime_type = eANM_CODE_RECOVER;
				}
				wk->seq++;
			}
		}
		break;

	case 3:
		if ( wk->type == 0 ){
			//if ( wk->wait == ( BB_RECOVER_WAIT - 55 ) ){		///< ネジがおちたとき
			//if ( wk->wait == ( BB_RECOVER_WAIT - 75 ) ){		///< ネジがおちたとき
			if ( wk->wait == ( BB_RECOVER_WAIT - 82 ) ){		///< ネジがおちたとき
				Snd_SePlay( BB_SND_NEJI );
			}
			if ( (++wk->wait) >= ( BB_RECOVER_WAIT - ( 50 ) ) ){
				*wk->anime_type = eANM_CODE_RECOVER;
				wk->seq++;
			}
		}
		else {
			wk->seq++;
		}
		break;
		
	default:
		if ( wk->type == 0 ){
			if ( (++wk->wait) >= ( BB_RECOVER_WAIT - ( 45 ) ) ){
				*wk->anime_type = eANM_CODE_DEFAULT;
				Snd_SePlay( BB_SND_JUMP_OUT );		///< 上る
				wk->seq = 0;
				wk->type++;
			}
		}
		else {
			if ( (++wk->wait) >= 10 ){
				if ( wk->rare_game ){
					wk->model->pos.y = BB_MODEL_OFS_Y2;
				}
				else {
					wk->model->pos.y = BB_MODEL_OFS_Y;
				}
				wk->model->pos.z = BB_MODEL_OFS_Z;			
				TCB_Delete( tcb );
				sys_FreeMemoryEz( wk );
			}
		}
		break;
	}
}


void BB_Client_Manene_3D_Recover_Action_TCB( TCB_PTR tcb, void* work )
{
	BB_MANENE_3D_ACTION* wk = work;	

	s16 pos_y;
	s16 pos_z;
	s16 py = wk->model->pos.y >> FX32_SHIFT;
	s16 pz = wk->model->pos.z >> FX32_SHIFT;
	s16 check_z = wk->mane_pos.z >> FX32_SHIFT;
	
	if ( (WIPE_SYS_EndCheck() == FALSE) || (wk->sys->comm_err_data.dis_err == TRUE) ){
		TCB_Delete( tcb );
		sys_FreeMemoryEz( wk );
		return;
	}

	pos_y = action_3d_y_tbl_2[ wk->seq ];
	
	if ( pos_y == SKIP_CODE_1 ){
		Quaternion_Identity( &wk->model->tq );
		Quaternion_Identity( &wk->model->cq );	
		Quaternion_Rotation( wk->model, 0, 0, +5, +5, BB_COEFFICIENT_AMI, FALSE );
		Quaternion_Rotation( wk->model, 0, 0, -5, -5, BB_COEFFICIENT_AMI, FALSE );
	
		wk->seq++;
		
		if ( wk->rare_game ){
			wk->model->pos.y = BB_MODEL_OFS_Y2 + FX32_CONST( 12 );
		}
		else {
			wk->model->pos.y = BB_MODEL_OFS_Y + FX32_CONST( 12 );
		}
		wk->model->pos.z = BB_MODEL_OFS_Z;
		wk->seq++;
		
		return;
	}
	else if ( pos_y == SKIP_CODE_2 ){
		
		if ( wk->rare_game ){
			wk->model->pos.y = BB_MODEL_OFS_Y2;
		}
		else {
			wk->model->pos.y = BB_MODEL_OFS_Y;
		}
		wk->model->pos.z = BB_MODEL_OFS_Z;
		
		TCB_Delete( tcb );
		sys_FreeMemoryEz( wk );	
		return;	
	}
			
	py -= pos_y;
	wk->seq++;
	
	wk->model->pos.y = FX32_CONST( py );
}

//--------------------------------------------------------------
/**
 * @brief	アクション　(animeと密接なTCB)
 *
 * @param	tcb	
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_Client_Manene_Action_TCB( TCB_PTR tcb, void* work )
{
	BB_MANENE_ACTION* wk = work;
	
	int frame = CATS_ObjectAnimeFrameGetCap( wk->cap_mane );
	
	if ( (WIPE_SYS_EndCheck() == FALSE) || (wk->sys->comm_err_data.dis_err == TRUE) ){
		TCB_Delete( tcb );
		sys_FreeMemoryEz( wk );
		return;
	}
	
	switch ( wk->seq ){
	case 0:
		///< 初期化
		{
			fx32 x, y;
			fx32 dat;
			wk->wait = 0;		
			CATS_ObjectPosGetCapFx32( wk->cap_mane, &x, &y );		
			if ( wk->type == 0 ){
				BB_MoveInit_FX( &wk->data, y, y + FX32_CONST( 32 ), 16 );
			}
			else {
				BB_MoveInit_FX( &wk->data, y, y - FX32_CONST( 32 ), 4 );
			}
	//		OS_Printf( "落下\n" );			
			wk->seq++;
		}
		break;
	
	case 1:
		///< 落下メイン
		{
			fx32 x, y;
			BOOL bEnd;			
			bEnd = BB_MoveMain_FX( &wk->data );
			CATS_ObjectPosGetCapFx32( wk->cap_mane, &x, &y );
			CATS_ObjectPosSetCapFx32( wk->cap_mane, x, wk->data.x );			
			if ( bEnd ){
				wk->seq++;
			}
		}
		break;
	
	default:
		///< 落下処理
		if ( wk->type == 0 ){
			if ( (++wk->wait) >= 35 ){
				wk->type++;
				wk->seq	 = 0;		
			}
		}
		else {
			if ( (++wk->wait) >= 10 ){
				TCB_Delete( tcb );
				sys_FreeMemoryEz( wk );
			}
		}
		break;
	}
}


//--------------------------------------------------------------
/**
 * @brief	LvUp	影が、オーラ？に変わって縦伸びするエフェクト
 *
 * @param	tcb	
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BB_Client_LevelUp_Action_TCB( TCB_PTR tcb, void* work )
{
	BB_LEVEL_UP* wk = work;

	if ( (WIPE_SYS_EndCheck() == FALSE) || (wk->sys->comm_err_data.dis_err == TRUE) ){
		wk->bStart = FALSE;
		TCB_Delete( tcb );
		return;
	}

	switch ( wk->seq ){
	case 0:
		{
			f32 x, y;
			CATS_ObjectAnimeSeqSetCap( wk->cap_kage, eANM_LEVEL_EFF );
			CATS_ObjectPriSetCap( wk->cap_kage, 0 );			
			CATS_ObjectAffineSetCap( wk->cap_kage, CLACT_AFFINE_DOUBLE );
			CATS_ObjectScaleGetCap( wk->cap_kage, &x, &y );
			BB_MoveInit_FX( &wk->data, FX_F32_TO_FX32( y ), FX_F32_TO_FX32( 2.0f ), 2 );
			
			{
				int ofs = ( *wk->level ) - 1;				
				CATS_ObjectPaletteOffsetSetCap( wk->cap_kage, ofs );
			}
			wk->seq++;
		}
	
	case 1:
		{
			fx32 x, y;
			BOOL bEnd;
			bEnd = BB_MoveMain_FX( &wk->data );			
			CATS_ObjectScaleSetCap( wk->cap_kage, 1.0f, FX_FX32_TO_F32( wk->data.x ) );
			if ( bEnd ){
				wk->seq++;
			}
		}
		break;
	
	case 2:
		BB_MoveInit_FX( &wk->data, FX_F32_TO_FX32( 2.0f ), FX_F32_TO_FX32( 0.4f ), 4 );
		wk->seq++;
		
	case 3:
		{
			fx32 x, y;
			BOOL bEnd;
			bEnd = BB_MoveMain_FX( &wk->data );			
			CATS_ObjectScaleSetCap( wk->cap_kage, 1.0f, FX_FX32_TO_F32( wk->data.x ) );
			if ( bEnd ){
				wk->seq++;
			}
		}
		break;
	
	case 4:
		if ( wk->old_level != *wk->level ){
			wk->seq = 0;						
		}
		else {
			if ( wk->type == 0 ){
				BB_MoveInit_FX( &wk->data, FX_F32_TO_FX32( 0.4f ), FX_F32_TO_FX32( 0.8f ), 2 );
			}
			else {
				BB_MoveInit_FX( &wk->data, FX_F32_TO_FX32( 0.8f ), FX_F32_TO_FX32( 0.4f ), 4 );
			}
			wk->seq++;
		}
		wk->old_level = *wk->level;
		if ( *wk->level == 0 ){	wk->seq = 0xFF; }
		break;
	
	case 5:
		{
			BOOL bEnd = BB_MoveMain_FX( &wk->data );			
			CATS_ObjectScaleSetCap( wk->cap_kage, 1.0f, FX_FX32_TO_F32( wk->data.x ) );
			if ( bEnd ){
				wk->seq = 4;
				wk->type ^= 1;
			}		
		}
		break;
	
	default:
		///< 落下処理
		CATS_ObjectScaleSetCap( wk->cap_kage, 1.0f, 1.0f );
		CATS_ObjectAnimeSeqSetCap( wk->cap_kage, eANM_KAGE );
		CATS_ObjectPriSetCap( wk->cap_kage, 2 );
		CATS_ObjectAffineSetCap( wk->cap_kage, CLACT_AFFINE_NONE );
		wk->bStart = FALSE;
		TCB_Delete( tcb );
	//	sys_FreeMemoryEz( wk );
		break;
	}
}

static const int data[ ][ 2 ] = {
	{ NARC_manene_hikari1_base_nsbmd, NARC_manene_hikari1_josho_nsbmd },
	{ NARC_manene_hikari2_base_nsbmd, NARC_manene_hikari2_josho_nsbmd },
	{ NARC_manene_hikari3_base_nsbmd, NARC_manene_hikari3_josho_nsbmd },
	{ NARC_manene_hikari4_base_nsbmd, NARC_manene_hikari4_josho_nsbmd },
	{ NARC_manene_hikari4_base_nsbmd, NARC_manene_hikari4_josho_nsbmd },
};
static const int anime[ ][ 2 ] = {
	{ NARC_manene_hikari_base_nsbca,  NARC_manene_hikari_base_nsbta },
	{ NARC_manene_hikari_josho_nsbca, NARC_manene_hikari_josho_nsbta },
	{ NARC_manene_hikari_josho_nsbca, NARC_manene_hikari_josho_nsbta },
};
static void BB_Client_3D_LevelUp_Action_TCB( TCB_PTR tcb, void* work )
{
	int level_tmp;
	BB_LEVEL_UP* wk = work;
	BB_3D_MODEL* bs = wk->model_bs;
	BB_3D_MODEL* up = wk->model_up;

	if ( (WIPE_SYS_EndCheck() == FALSE) || (wk->sys->comm_err_data.dis_err == TRUE) ){
		BB_disp_Model_Delete( bs );	
		BB_disp_Model_Delete( up );
		D3DOBJ_AnmDelete( &bs->anm[ 0 ], &wk->sys->allocator );
		D3DOBJ_AnmDelete( &bs->anm[ 1 ], &wk->sys->allocator );
		D3DOBJ_AnmDelete( &up->anm[ 0 ], &wk->sys->allocator );
		D3DOBJ_AnmDelete( &up->anm[ 1 ], &wk->sys->allocator );
		wk->bStart = FALSE;
		TCB_Delete( tcb );
		return;
	}
	
	level_tmp = ( *wk->level ) - 1;

	if ( level_tmp < 0 || level_tmp > BB_GANE_LEVEL_MAX ){
		level_tmp = 0; 
	}

	switch ( wk->seq ){
	case 0:
		///< モデルの読み込み
		BB_disp_Model_Load( bs, wk->sys->p_handle_bb, data[ level_tmp ][ 0 ] );
		BB_disp_Model_Init( bs, wk->rare_game );
		wk->seq++;
		break;
	
	case 1:
		///< アニメ読み込み
		D3DOBJ_AnmLoadH( &bs->anm[ 0 ], &bs->mdl, wk->sys->p_handle_bb, anime[ 0 ][ 0 ], HEAPID_BB, &wk->sys->allocator );								 
		D3DOBJ_AnmLoadH( &bs->anm[ 1 ], &bs->mdl, wk->sys->p_handle_bb, anime[ 0 ][ 1 ], HEAPID_BB, &wk->sys->allocator );
		D3DOBJ_AddAnm( &bs->obj, &bs->anm[ 0 ] );
		D3DOBJ_AddAnm( &bs->obj, &bs->anm[ 1 ] );
		D3DOBJ_AnmSet( &bs->anm[ 0 ], 0 );
		D3DOBJ_AnmSet( &bs->anm[ 1 ], 0 );
		D3DOBJ_SetDraw( &bs->obj, FALSE );
		bs->bAnime = FALSE;					
		Quaternion_Identity( &bs->tq );
		Quaternion_Identity( &bs->cq );
		Quaternion_Rotation( bs, 0, 0, +5, +5, BB_COEFFICIENT_AMI, FALSE );
		Quaternion_Rotation( bs, 0, 0, -5, -5, BB_COEFFICIENT_AMI, FALSE );	
		wk->seq++;
		break;
	
	case 2:
		///< モデル読み込み
		BB_disp_Model_Load( up, wk->sys->p_handle_bb, data[ level_tmp ][ 1 ] );
		BB_disp_Model_Init( up, wk->rare_game );
		wk->seq++;
		break;		
	
	case 3:
		///< アニメ読み込み			
		D3DOBJ_AnmLoadH( &up->anm[ 0 ], &up->mdl, wk->sys->p_handle_bb, anime[ 1 ][ 0 ], HEAPID_BB, &wk->sys->allocator );								 
		D3DOBJ_AnmLoadH( &up->anm[ 1 ], &up->mdl, wk->sys->p_handle_bb, anime[ 1 ][ 1 ], HEAPID_BB, &wk->sys->allocator );
		D3DOBJ_AddAnm( &up->obj, &up->anm[ 0 ] );
		D3DOBJ_AddAnm( &up->obj, &up->anm[ 1 ] );
		D3DOBJ_AnmSet( &up->anm[ 0 ], 0 );
		D3DOBJ_AnmSet( &up->anm[ 1 ], 0 );
		D3DOBJ_SetDraw( &up->obj, TRUE );
		up->bAnime = TRUE;
		Quaternion_Identity( &up->tq );
		Quaternion_Identity( &up->cq );
		Quaternion_Rotation( up, 0, 0, +5, +5, BB_COEFFICIENT_AMI, FALSE );
		Quaternion_Rotation( up, 0, 0, -5, -5, BB_COEFFICIENT_AMI, FALSE );
		
		if ( wk->rare_game ){
			D3DOBJ_SetMatrix( &bs->obj, 0, BB_MODEL_OFS_Y2, 0 );
			D3DOBJ_SetMatrix( &up->obj, 0, BB_MODEL_OFS_Y2, 0 );
		    bs->pos.y = BB_MODEL_OFS_Y2;
		    up->pos.y = BB_MODEL_OFS_Y2;
		}
		wk->seq++;
		break;
	
	case 4:
		if ( up->bAnime == FALSE ){
			D3DOBJ_SetDraw( &bs->obj, TRUE );
			D3DOBJ_SetDraw( &up->obj, FALSE );
			bs->bAnime = TRUE;
			wk->seq++;
		}
		break;
	
	case 5:		
		if ( *wk->level == 0 ){
			wk->seq = 0xFF;
		}
		else if ( wk->old_level != *wk->level ){
			BB_disp_Model_Delete( bs );	
			BB_disp_Model_Delete( up );
			D3DOBJ_AnmDelete( &bs->anm[ 0 ], &wk->sys->allocator );
			D3DOBJ_AnmDelete( &bs->anm[ 1 ], &wk->sys->allocator );
			D3DOBJ_AnmDelete( &up->anm[ 0 ], &wk->sys->allocator );
			D3DOBJ_AnmDelete( &up->anm[ 1 ], &wk->sys->allocator );			
			wk->seq = 0;						
		}
		wk->old_level = *wk->level;
		break;
	
	default:
		BB_disp_Model_Delete( bs );	
		BB_disp_Model_Delete( up );
		D3DOBJ_AnmDelete( &bs->anm[ 0 ], &wk->sys->allocator );
		D3DOBJ_AnmDelete( &bs->anm[ 1 ], &wk->sys->allocator );
		D3DOBJ_AnmDelete( &up->anm[ 0 ], &wk->sys->allocator );
		D3DOBJ_AnmDelete( &up->anm[ 1 ], &wk->sys->allocator );
		wk->bStart = FALSE;
		TCB_Delete( tcb );
	//	sys_FreeMemoryEz( wk );
		break;
	}
}


//--------------------------------------------------------------
/**
 * @brief	スコアを計算し続けるメイン
 *
 * @param	wk
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BB_Client_ScoreUpMain( BB_CLIENT* wk )
{
	const u16 base_point[]  = { BB_P_LV1_BASE, BB_P_LV2_BASE, BB_P_LV3_BASE, BB_P_LV4_BASE  };	///< ベース
	
	wk->game_sys.point += base_point[ wk->game_sys.level ];
	
//	OS_Printf( " point = %d\n", wk->game_sys.point );
}

//--------------------------------------------------------------
/**
 * @brief	ボーナスポイントの設定
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BB_Client_BonusPoint( BB_CLIENT* wk )
{
	const u16 bonus_point[] = { BB_P_LV1_BONUS,BB_P_LV2_BONUS,BB_P_LV3_BONUS,BB_P_LV4_BONUS };	///< ボーナス

	wk->game_sys.point += bonus_point[ wk->game_sys.level - 1 ];
	
//	OS_Printf( " point = %d\n", wk->game_sys.point );
}


//--------------------------------------------------------------
/**
 * @brief	レベルアップの監視
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static BOOL BB_Client_LevelUpMain( BB_CLIENT* wk )
{
	switch ( wk->game_sys.result_time ){
	case BB_MY_EFF_1:
		wk->game_sys.eff_level = 1;
		break;
	case BB_MY_EFF_2:
		wk->game_sys.eff_level = 2;
		break;
	case BB_MY_EFF_3:
		wk->game_sys.eff_level = 3;
		break;
	case BB_MY_EFF_4:
		wk->game_sys.eff_level = 4;
		break;
	case BB_MY_EFF_5:
		wk->game_sys.eff_level = 5;
		break;
	case 0:
		wk->game_sys.eff_level = 0;
		break;
	}
	
	return BB_Client_LevelUpCheck( wk, wk->game_sys.result_time, &wk->game_sys.level );
}


//--------------------------------------------------------------
/**
 * @brief	LvUpチェック
 *
 * @param	wk	
 * @param	time	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL BB_Client_LevelUpCheck( BB_CLIENT* wk, int time, int* level )
{	
	switch ( time ){
	case BB_GAME_LV_2UP:
		*level = 1;											///< レベルアップ
		return TRUE;
		
	case BB_GAME_LV_3UP:
		*level = 2;											///< レベルアップ
		return TRUE;
		
	case BB_GAME_LV_4UP:
		*level = 3;											///< レベルアップ
		return TRUE;
		
	case BB_GAME_LV_MAX:
		*level = 4;											///< レベルアップ
		return TRUE;
	
	case 0:													///< その他
		*level = 0;
		break;
	}	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	他人の更新
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BB_Client_OtherUpdateMain( BB_CLIENT* wk )
{
	int i;
	int	comm_num = wk->comm_num;
	
	for ( i = 0; i < comm_num; i++ ){
		if ( i == wk->netid ){ continue; }
				
		switch ( wk->state[ i ].state ){
		case eBB_STATE_NONE:
			///< 通常
			if ( BB_disp_Manene_OAM_AnimeCheck( wk, i ) != eANM_CODE_ARUKU ){
				BB_disp_Manene_OAM_AnimeChange( wk, i, eANM_CODE_ARUKU );
			}
			if ( BB_Client_LevelUpCheck( wk, wk->game_sys.l_time[ i ], &wk->game_sys.o_level[ i ] ) ){
				BB_Client_LevelUp_Action_Call( wk, i );
	//			OS_Printf( " 他人のレベル = %d\n", wk->game_sys.o_level[ i ] );
			}
			wk->game_sys.l_time[ i ]++;
			break;

		case eBB_STATE_FALL:
			///< 落下
			wk->game_sys.fever_time	  = 0;
			wk->game_sys.l_time[ i ]  = 0;
			wk->game_sys.o_level[ i ] = 0;
			wk->fever_sys.bActive = FALSE;
			if ( BB_disp_Manene_OAM_AnimeCheck( wk, i ) != eANM_CODE_OCHIRU ){
				///< アクションしてるフラグチェック
				if ( wk->bAction == FALSE )
				{
					BB_disp_Manene_OAM_AnimeChange( wk, i, eANM_CODE_OCHIRU );
					BB_Client_Manene_Action_Call( wk, i );
					wk->bAction = TRUE;
				}
			}
			break;

		case eBB_STATE_RECOVER:
			///< 復帰
			wk->bAction = FALSE;
			wk->game_sys.fever_time	  = 0;
			wk->game_sys.l_time[ i ]  = 0;
			wk->game_sys.o_level[ i ] = 0;
			wk->state[ i ].state = eBB_STATE_NONE;
			break;
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief	自分自身の更新
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BB_Client_MyUpdateMain( BB_CLIENT* wk )
{
	switch ( wk->state[ wk->netid ].state ){

	case eBB_STATE_NONE:
		///< 通常
		if ( BB_Client_IsSafety( wk, NULL ) ){
			wk->game_sys.recover_wait = 0;
			BB_Manene_3D_Fall_Call( wk );				///< マネネ落ちて戻る
			BB_Client_GameReset( wk );					///< ゲーム部分リセット

			wk->state[ wk->netid ].state = eBB_STATE_FALL;		///< 落下状態送信
			CommSendData( CCMD_BB_STATE, &wk->state[ wk->netid ], sizeof( BB_STATE_COMM ) );
			wk->game_sys.result_time = 0;
			wk->game_sys.fever_time  = 0;
			wk->fever_sys.bActive	 = FALSE;
		}
		else {
			BB_ManeneAnime_CheckSet( wk );
			BB_Client_TouchPanel_Main( wk );					///< ペン操作
			BB_Client_GameUpdate( wk );							///< ゲーム更新
			BB_Client_ScoreUpMain( wk );						///< スコア更新	
			wk->game_sys.l_time[ wk->netid ]++;					///< 
		}
		break;

	case eBB_STATE_FALL:
		///< 落下
		if ( ( ++wk->game_sys.recover_wait ) >= BB_RECOVER_WAIT ){
			wk->state[ wk->netid ].state = eBB_STATE_RECOVER;
		}
		wk->game_sys.result_time		 = 0;
		wk->game_sys.fever_time			 = 0;
		wk->game_sys.l_time[ wk->netid ] = 0;
		break;

	case eBB_STATE_RECOVER:
		///< 落下復帰
		if ( wk->game_sys.recover_wait != 0 ){
			wk->game_sys.result_time		 = 0;
			wk->game_sys.fever_time			 = 0;
			wk->game_sys.recover_wait		 = 0;
			wk->game_sys.l_time[ wk->netid ] = 0;			
			if ( wk->game_sys.fever_level_old != wk->game_sys.fever_level ){
				wk->game_sys.fever_level = wk->game_sys.fever_level_old;
			}
			BB_Client_3D_PosInit( wk );
			CommSendData( CCMD_BB_STATE, &wk->state[ wk->netid ], sizeof( BB_STATE_COMM ) );
		}
		else {
		//	BB_Client_GameReset( wk );							///< ゲーム部分リセット
			wk->state[ wk->netid ].state = eBB_STATE_NONE;
		}
		break;
		
	case eBB_STATE_RESET:
		if ( (++wk->game_sys.recover_wait) >= RECOVER_TABLE_NUM ){
			wk->game_sys.recover_wait = 0;
			BB_Client_3D_PosInit( wk );
			wk->state[ wk->netid ].state = eBB_STATE_NONE;
		}
		break;
	}	
}

//--------------------------------------------------------------
/**
 * @brief	クライアントのゲームコア
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_Client_GameCore( BB_CLIENT* wk )
{
	int	i;
	int	comm_num = wk->comm_num;

	if ( wk->game_sys.game_timer < BB_GAME_TIMER ){
		
		BB_Client_MyUpdateMain( wk );						///< 自分の情報更新
		BB_Client_OtherUpdateMain( wk );					///< 他のプレイヤー情報更新
//		BB_Client_ScoreUpMain( wk );						///< スコア更新	

		if ( BB_Client_LevelUpMain( wk ) == TRUE ){			///< レベルアップ更新
		
			if ( wk->state[ wk->netid ].state == eBB_STATE_NONE ){
			//	wk->game_sys.recover_wait = 0;
			//	wk->state[ wk->netid ].state = eBB_STATE_RESET;
			//	BB_Client_Manene_3D_RecoverAction_Call( wk );
			}
			BB_Client_3D_LevelUp_Action_Call( wk, wk->netid );
			BB_Client_BonusPoint( wk );						///< ボーナスポイント付加
			Snd_SePlay( BB_SND_LV_UP );
		}

		BB_Client_GameFeverUpdate( wk );					///< フィーバーできるかの判定
		wk->game_sys.result_time++;
		wk->game_sys.game_timer++;
		wk->game_sys.fever_time++;
	}
}

static const TP_HIT_TBL ball_hit_tbl[] = {
	{
		TP_USE_CIRCLE,
		128,
		 96 + 32,
		 48,
	},
	{ TP_HIT_END,0,0,0, }
};

static const TP_HIT_TBL ball_hit_tbl2[] = {
	{
		TP_USE_CIRCLE,
		128,
		 96 + 32,
		 64,
	},
	{ TP_HIT_END,0,0,0, }
};


//--------------------------------------------------------------
/**
 * @brief	タッチパネル処理
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_Client_TouchPanel_Main( BB_CLIENT* wk )
{	
	{
		u32 x, y;
		s16 mx, my;
		BOOL bMove;
		BOOL bHit = GF_TP_GetPointCont( &x, &y );
		TP_HIT_TBL* tbl;
		int hitdata;

		wk->game_sys.touch = FALSE;
		if ( wk->rare_game ){
			hitdata = GF_TP_HitSelf( ball_hit_tbl2, x, y );
		}
		else {
			hitdata = GF_TP_HitSelf( ball_hit_tbl, x, y );
		}
		
		if ( hitdata == TP_HIT_NONE ){
			
			wk->control.old_x = 0;
			wk->control.old_y = 0;
			
			if ( wk->bb3d_ball.pow_count != 0 ){
				Quaternion_Rotation_Pow( &wk->bb3d_ball, 1.0f );
				bMove = Quaternion_Rotation_Pow( &wk->bb3d_mane[ 0 ], 0.5f );
				if ( bMove = TRUE )	{
					VecFx32 vec = { 0, BB_POINT_Y, 0 };
					VecFx32 tmp;
					MTX_MultVec43( &vec, &wk->bb3d_mane[ 0 ].tmp43, &wk->mane_pos );
					
					wk->game_sys.touch = TRUE;
				}				
			}
			else {
				wk->bb3d_ball.get_pow_x = 0.0f;
				wk->bb3d_ball.get_pow_y = 0.0f;
				Quaternion_Rotation( &wk->bb3d_ball, wk->control.old_x, wk->control.old_y, wk->control.old_x, wk->control.old_y, BB_COEFFICIENT_AB, FALSE );
				Quaternion_Rotation( &wk->bb3d_mane[ 0 ], wk->control.old_x, wk->control.old_y, wk->control.old_x, wk->control.old_y, BB_COEFFICIENT_AM, FALSE );
			}			
			//Snd_SeStopBySeqNo( BB_SND_KIRAKIRA, 0 );
		}
		else {
			f32 py = BB_COEFFICIENT_PEN( wk->mane_pos.y ); //100 - ( wk->mane_pos.y >> FX32_SHIFT ) ) / 40;
			
			if ( ( wk->mane_pos.y >> FX32_SHIFT ) >= 95 ){			///< 角度 5%までは ゆるい補正
				py = 0.1f;
			}
			if ( py >= BB_COEFFICIENT_AM ){
				 py = BB_COEFFICIENT_AM;
			}
			else {
				py = BB_COEFFICIENT_AM - py;
			}
			
			if ( wk->control.old_x == 0 ){
				wk->control.old_x = x;
				wk->control.old_y = y;
			}
			
			Quaternion_Rotation( &wk->bb3d_ball, x, y, wk->control.old_x, wk->control.old_y, BB_COEFFICIENT_AB_PEN, TRUE );
			bMove = Quaternion_Rotation( &wk->bb3d_mane[ 0 ], x, y, wk->control.old_x, wk->control.old_y, py, TRUE );
			
			if ( bMove = TRUE ){
				VecFx32 vec = { 0, BB_POINT_Y, 0 };
				VecFx32 tmp;
				MTX_MultVec43( &vec, &wk->bb3d_mane[ 0 ].tmp43, &wk->mane_pos );
				wk->game_sys.touch = TRUE;
				if ( wk->game_sys.bFirstTouch == FALSE ){
					wk->game_sys.bFirstTouch = TRUE;
				}
			}
			if ( x != 0 ){
				if ( Snd_SePlayCheck( BB_SND_KIRAKIRA ) == 0 ){
					Snd_SePlay( BB_SND_KIRAKIRA );			///< きらきら
				}
				BB_Stardust_Call( wk, x, y );
			}
			
			wk->control.old_x = x;
			wk->control.old_y = y;
		}
	}
}


typedef struct {
	
	int pow;
	int percent;	
	
} BB_GAME_DATA;

static const BB_GAME_DATA game_data[][ 4 ] = {
	{	///< Lv 1
		{ LV1_0_X, LV1_0_P },
		{ LV1_1_X, LV1_1_P },
		{ LV1_2_X, LV1_2_P },
		{ LV1_3_X, LV1_3_P },
	},
	{	///< Lv 2
		{ LV2_0_X, LV2_0_P },
		{ LV2_1_X, LV2_1_P },
		{ LV2_2_X, LV2_2_P },
		{ LV2_3_X, LV2_3_P },
	},
	{	///< Lv 3
		{ LV3_0_X, LV3_0_P },
		{ LV3_1_X, LV3_1_P },
		{ LV3_2_X, LV3_2_P },
		{ LV3_3_X, LV3_3_P },
	},
	{	///< Lv 4
		{ LV4_0_X, LV4_0_P },
		{ LV4_1_X, LV4_1_P },
		{ LV4_2_X, LV4_2_P },
		{ LV4_3_X, LV4_3_P },
	},
};

static inline u32 BB_Rand( void )
{
	u32 seed = gf_get_seed();
	u32 rand;
	
	rand = gf_rand();
	
	gf_srand( seed );
	
//	OS_Printf( "引いた1 = %d\n", seed );
//	OS_Printf( "引いた = %d\n", gf_get_seed() );
	
	return rand;
}

static inline s8 BB_inline_GetSign( void )
{
//	if ( BB_Rand() % 2 ){
	if ( gf_rand() % 2 ){
//	if ( 0 ){
		return +1;
	}
	else {
		return -1;
	}
}

static inline s8 BB_inline_GetSign_to_Value( s16 val )
{
	if ( val >= 0 ){
		return +1;
	}
	else {
		return -1;
	}	
}


//--------------------------------------------------------------
/**
 * @brief	仕切りなおす部分
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_Client_GameReset( BB_CLIENT* wk )
{
	wk->game_sys.frame		= 0;
	wk->game_sys.lot_timer	= 0;
	wk->game_sys.touch		= FALSE;
	wk->game_sys.inertia	= FALSE;
	wk->game_sys.init_ok	= FALSE;
	wk->game_sys.boot_init	= FALSE;
	
	///< --- 新規 ---
	wk->game_sys.level		= 0;
}


//--------------------------------------------------------------
/**
 * @brief	フィーバーいけるか監視
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_Client_GameFeverUpdate( BB_CLIENT* wk )
{
	BOOL fever = TRUE; 
	
	if ( wk->game_sys.fever_time < BB_FEVER_COUNT ){ return; }	
	if ( wk->game_sys.fever_level == BB_FEVER_LEVEL_MAX ){ return; }
	
	wk->game_sys.fever_level_old = wk->game_sys.fever_level;
	wk->game_sys.fever_level++;
	OS_Printf( "level = %d  old_level = %d\n", wk->game_sys.fever_level, wk->game_sys.fever_level_old );
	wk->game_sys.fever_time = 0;
	wk->fever_sys.bActive = TRUE;
	
// 	OS_Printf( " fever level = %d\n", wk->game_sys.fever_level );
}

//--------------------------------------------------------------
/**
 * @brief	ゲームアップデート
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_Client_GameUpdate( BB_CLIENT* wk )
{
	BOOL bb;	
	BOOL bm;	
	BOOL bMove_x;
	BOOL bMove_y;
	fx32 oldx;
	fx32 oldy;
	s16 mx = 0;
	s16 my = 0;
	s16 update_tbl[] = { BB_ROOL_UPDATE_LV1, BB_ROOL_UPDATE_LV2, BB_ROOL_UPDATE_LV3, BB_ROOL_UPDATE_LV4 };

#ifdef BB_SW_GAME_UPDATE_OFF

	if ( sys.cont & PAD_BUTTON_A ){
		return;
	}
	
#endif
	wk->game_sys.lot_timer++;

	///< 抽選	
	if ( wk->game_sys.frame != 0 ){	
		if ( wk->game_sys.roll_seq < BB_CONST_ROLL_FRAME - 1 ){		///< 最初にかかる力で動く部分
			if ( wk->game_sys.roll_seq <= BB_CONST_INI_FRAME ){
				oldx = wk->game_sys.ini_amx.x;
				oldy = wk->game_sys.ini_amy.x;							
				bMove_x = BB_AddMoveMainFx( &wk->game_sys.ini_amx );
				bMove_y = BB_AddMoveMainFx( &wk->game_sys.ini_amy );		
				mx = ( wk->game_sys.ini_amx.x - oldx ) >> FX32_SHIFT;
				my = ( wk->game_sys.ini_amy.x - oldy ) >> FX32_SHIFT;								
				if ( bMove_x == TRUE || bMove_y == TRUE ){
					wk->game_sys.ini_amx.x -= oldx;
					wk->game_sys.ini_amy.x -= oldy;
				}
			}
			else {
				mx = wk->game_sys.ini_amx.x >> FX32_SHIFT;
				my = wk->game_sys.ini_amy.x >> FX32_SHIFT;
				
			}
		}
		else {														///< 抽選結果で動く部分
			oldx = wk->game_sys.amx.x;
			oldy = wk->game_sys.amy.x;						
			bMove_x = BB_AddMoveMainFx( &wk->game_sys.amx );
			bMove_y = BB_AddMoveMainFx( &wk->game_sys.amy );			
			mx = ( wk->game_sys.amx.x - oldx ) >> FX32_SHIFT;
			my = ( wk->game_sys.amy.x - oldy ) >> FX32_SHIFT;			
			if ( bMove_x == TRUE || bMove_y == TRUE ){				
				if ( wk->game_sys.inertia == FALSE ){
					wk->game_sys.inertia = TRUE;					
					wk->game_sys.amx.x -= oldx;
					wk->game_sys.amy.x -= oldy;
				}
			}
		}		
		bb = Quaternion_Rotation( &wk->bb3d_ball, 0, 0, mx, my, BB_COEFFICIENT_AB, FALSE );	
		bm = Quaternion_Rotation( &wk->bb3d_mane[ 0 ], 0, 0, mx, my, BB_COEFFICIENT_AMB, FALSE );		
		if ( bm )
		{
			VecFx32 vec = { 0, BB_POINT_Y, 0 };
			VecFx32 tmp;
			MTX_MultVec43( &vec, &wk->bb3d_mane[ 0 ].tmp43, &wk->mane_pos );
		}
		wk->game_sys.roll_seq++;
		wk->game_sys.frame--;
	}
	else {
		///< 次の準備まで慣性で動く部分
		if ( ( wk->game_sys.lot_timer % update_tbl[ wk->game_sys.level ] ) == 0 ){
			wk->game_sys.lot_timer = 0;
			wk->game_sys.init_ok = TRUE;
		}
	
		if ( wk->game_sys.touch == FALSE
		&&	 wk->game_sys.inertia == TRUE )
		{
			mx = wk->game_sys.amx.x >> FX32_SHIFT;
			my = wk->game_sys.amy.x >> FX32_SHIFT;
		
			bb = Quaternion_Rotation( &wk->bb3d_ball, 0, 0, mx, my, BB_COEFFICIENT_AB, FALSE );	
			bm = Quaternion_Rotation( &wk->bb3d_mane[ 0 ], 0, 0, mx, my, BB_COEFFICIENT_AMB, FALSE );		
			if ( bm )
			{
				VecFx32 vec = { 0, BB_POINT_Y, 0 };
				VecFx32 tmp;
				MTX_MultVec43( &vec, &wk->bb3d_mane[ 0 ].tmp43, &wk->mane_pos );
			}
		}
		else if ( wk->game_sys.touch == TRUE && wk->game_sys.inertia == TRUE )
		{
			mx = wk->game_sys.amx.x >> FX32_SHIFT;
			my = wk->game_sys.amy.x >> FX32_SHIFT;
		
			bb = Quaternion_Rotation( &wk->bb3d_ball, 0, 0, mx, my, BB_COEFFICIENT_AB, FALSE );	
			bm = Quaternion_Rotation( &wk->bb3d_mane[ 0 ], 0, 0, mx, my, BB_COEFFICIENT_AMB, FALSE );		
			if ( bm )
			{
				VecFx32 vec = { 0, BB_POINT_Y, 0 };
				VecFx32 tmp;
				MTX_MultVec43( &vec, &wk->bb3d_mane[ 0 ].tmp43, &wk->mane_pos );
			}
		}	
		
		if ( wk->game_sys.boot_init == FALSE
		|| ( wk->game_sys.init_ok == TRUE && wk->game_sys.inertia == TRUE ) )
		{
			///< 初期化部分
			int dat = BB_Rand() % 100;
			int pow = 0;
			int i;
			int per = game_data[ wk->game_sys.level ][ 0 ].percent;
			s16 mx, my;
			s16 imx, imy;
			s16 cx, cz, cy;
			
//			OS_Printf( "%d - %d - %d \n", wk->game_sys.boot_init, wk->game_sys.init_ok, wk->game_sys.inertia );
			
			mx = my = 0;
			
			for ( i = 0; i < 3; i++ ){
				
				if ( dat < per ){ break; }
							
				per += game_data[ wk->game_sys.level ][ i + 1 ].percent;
			}
			
			pow  = game_data[ wk->game_sys.level ][ i ].pow;
			pow *= BB_COEFFICIENT_POW;
			
			///< まねねの位置から、次の傾きを検出
			{
				cx = wk->mane_pos.x >> FX32_SHIFT;
				cy = wk->mane_pos.y >> FX32_SHIFT;
				cz = wk->mane_pos.z >> FX32_SHIFT;
				
			//	if ( cz == 0 ){
				if ( wk->mane_pos.z == FX32_CONST( 0 ) ){
					cz = BB_inline_GetSign();
				}
				else {
					cz = -BB_inline_GetSign_to_Value( cz );
					OS_Printf( "傾きから計算\n");
				}
				
			//	if ( cx == 0 ){
				if ( wk->mane_pos.x == FX32_CONST( 0 ) ){
					cx = BB_inline_GetSign();
				}
				else {
					cx = BB_inline_GetSign_to_Value( cx );
					if ( cz < 0 ){
						cx *= -1;
					}
					OS_Printf( "傾きから計算\n");
				}
				
				///< level == 0 横の動きを消す
				if ( wk->game_sys.level == 0 ){	cx = 0;	}

				mx = pow * cx;
				my = pow * cz;
			}
			imx = LVA_DEFAUT_POW*cx;
			imy = LVA_DEFAUT_POW*cz;
			wk->game_sys.set_pow_x = mx;
			wk->game_sys.set_pow_y = my;
			BB_AddMoveReqFx( &wk->game_sys.ini_amx, FX32_CONST( 0 ), FX32_CONST( imx ), FX32_CONST( 0 ), BB_CONST_INI_FRAME );
			BB_AddMoveReqFx( &wk->game_sys.ini_amy, FX32_CONST( 0 ), FX32_CONST( imy ), FX32_CONST( 0 ), BB_CONST_INI_FRAME );
			BB_AddMoveReqFx( &wk->game_sys.amx, FX32_CONST( 0 ), FX32_CONST( mx ), FX32_CONST( 0 ), BB_ACCEL_ROLL_FRAME );
			BB_AddMoveReqFx( &wk->game_sys.amy, FX32_CONST( 0 ), FX32_CONST( my ), FX32_CONST( 0 ), BB_ACCEL_ROLL_FRAME );
			wk->game_sys.frame = BB_ACCEL_ROLL_FRAME + BB_CONST_ROLL_FRAME;
			wk->game_sys.roll_seq = 0;
			
			wk->game_sys.pow_x	 = mx;//cx;	///< 変更した4月2日
			wk->game_sys.pow_y	 = my;//cz;
			
			wk->game_sys.touch	 = FALSE;
			wk->game_sys.init_ok = FALSE;
			wk->game_sys.inertia = FALSE;
			
			wk->game_sys.boot_init = TRUE;

		//	OS_Printf( "準備できたんで次の回転始まります\n" );
		}
	}
}



//--------------------------------------------------------------
/**
 * @brief	ボールに飛び乗る
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
BOOL BB_Client_JumpOnToBall( BB_CLIENT* wk )
{
	int i;
	int max1 = NELEMS( action_2d_y_tbl );	
	int max2 = NELEMS( action_3d_y_tbl );	
	s16 pos_2d_y = action_2d_y_tbl[ max1 - 1 - wk->seq ];
	s16 pos_3d_y = action_3d_y_tbl[ max2 - 1 - wk->seq2 ];
	s16 pos_3d_z = action_3d_z_tbl[ max2 - 1 - wk->seq2 ];
	BB_3D_MODEL* model = &wk->bb3d_mane[ 0 ];
	s16 py = model->pos.y >> FX32_SHIFT;
	s16 pz = model->pos.z >> FX32_SHIFT;

	if ( (++wk->wait) < BB_JUMP_BALL_WAIT ){ return FALSE; }
	
	///< 上画面
	for ( i = 0; i < BB_PLAYER_NUM - 1; i++ ){
		if ( wk->cap_mane[ i ] == NULL ){
			continue;
		}
		CATS_ObjectPosMoveCap( wk->cap_mane[ i ], 0, -pos_2d_y );
	}
	
	///< 下画面
	if ( pos_3d_y == SKIP_CODE_1 || pos_3d_y == SKIP_CODE_2 ){
		if ( wk->seq2 == 0 ){
			Snd_SePlay( BB_SND_JUMP_IN );	///< 上る音
		}
		wk->seq2++;
		pos_3d_y = action_3d_y_tbl[ max2 - 1 - wk->seq2 ];
		pos_3d_z = action_3d_z_tbl[ max2 - 1 - wk->seq2 ];		
	}
//	OS_Printf( "%d, %d\n", 		pos_3d_y, pos_3d_z );
	py += pos_3d_y;
	pz -= pos_3d_z;

	model->pos.y = FX32_CONST( py );
	model->pos.z = FX32_CONST( pz );
	
	wk->seq2++;
	if ( (++wk->seq) >= max1 ){

		model->pos.y = BB_MODEL_OFS_Y;
		model->pos.z = BB_MODEL_OFS_Z;
		
		wk->seq = 0;
		wk->seq2= 0;
		
		return TRUE;
	}	
	
	return FALSE;
}

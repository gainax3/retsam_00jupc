//==============================================================================
/**
 * @file	balance_ball.c
 * @brief	簡単な説明を書く
 * @author	goto
 * @date	2007.09.25(火)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================

#include "common.h"
#include "system/wipe.h"

#include "application/balance_ball.h"

#include "bb_common.h"
#include "bb_comm_cmd.h"

#include "bb_server.h"
#include "bb_client.h"

#define BB_COMM_END_CMD	( 999 )

enum{
	BB_DIS_ERROR_NONE,			// エラーなし
	BB_DIS_ERROR_CLOSEING,		// 切断エラー	終了中
	BB_DIS_ERROR_CLOSED,		// 切断エラー	終了
};

//==============================================================
// Prototype
//==============================================================
static void BB_SystemInit( BB_WORK* wk );
static void BB_VramBankSet( GF_BGL_INI* bgl );
static void BB_CATS_Init( BB_WORK* wk );
static BOOL BB_WipeStart( int type );
static void BB_MainSeq_Change( BB_WORK* wk, BOOL bEnd, int next_seq, int* seq );
static void BB_VBlank( void* work );
static void MainResource_Delete( BB_WORK* wk );
static void Reset_GameData( BB_WORK* wk );

static u32 BB_DIS_ERROR_Check( BB_WORK* wk );

#define BB_TRANSFER_NUM				( 32 )
#define BB_WIRE_RESS_ICON_PAL		( 14 )
#define BB_WIRE_RESS_ICON_PAL_SIZ	( 32 )

enum {
	
	eBB_SEQ_ENTRY_INIT		= 0,
	eBB_SEQ_ENTRY_MAIN,
	eBB_SEQ_ENTRY_EXIT,
	
	eBB_SEQ_FADE_IN,
	eBB_SEQ_FADE_IN_WAIT,
	
	eBB_SEQ_START,
	eBB_SEQ_PEN_DEMO,
	eBB_SEQ_START_WAIT,
	
	eBB_SEQ_COUNT_DOWN,
	eBB_SEQ_COUNT_DOWN_WAIT,
	
	eBB_SEQ_MAIN,
	
	eBB_SEQ_MAIN_END_INIT,
	eBB_SEQ_MAIN_END,
		
	eBB_SEQ_RESULT,
	eBB_SEQ_RESULT_WAIT,
	
	eBB_SEQ_FADE_OUT,
	eBB_SEQ_FADE_OUT_WAIT,

	eBB_SEQ_SCORE,
	eBB_SEQ_SCORE_WAIT,
	
	eBB_SEQ_END,
	eBB_SEQ_END_WAIT,
};

static void BalanceBall_MainInit( BB_WORK* wk )
{
//	BB_WORK* wk = PROC_AllocWork( proc, sizeof( BB_WORK ), HEAPID_BB );
//	memset( wk, 0, sizeof( BB_WORK ) );

	wk->seed_tmp = gf_get_seed();		///< 乱数のタネ退避

	BB_SystemInit( wk );
	
	sys_VBlankFuncChange( BB_VBlank, wk );
	sys_HBlankIntrStop();

	initVramTransferManagerHeap( BB_TRANSFER_NUM, HEAPID_BB );	

	///< wi-fi アイコン( パレットをpalette_fade に渡す )
    CLACT_U_WmIcon_SetReserveAreaCharManager( NNS_G2D_VRAM_TYPE_2DMAIN, GX_OBJVRAMMODE_CHAR_1D_128K );
    CLACT_U_WmIcon_SetReserveAreaPlttManager( NNS_G2D_VRAM_TYPE_2DMAIN );
	WirelessIconEasy();
	{
		NNSG2dPaletteData *palData;
		void* dat = WirelessIconPlttResGet( HEAPID_BB );

		NNS_G2dGetUnpackedPaletteData( dat, &palData );		
		PaletteWorkSet( wk->sys.pfd, palData->pRawData, FADE_MAIN_OBJ,
						BB_WIRE_RESS_ICON_PAL * 16, BB_WIRE_RESS_ICON_PAL_SIZ );

		sys_FreeMemoryEz( dat );
	}

	///< 通信部分
	{
		int comm_num = CommInfoGetEntryNum();		
		wk->netid = CommGetCurrentID();		
		BB_CommCommandInit( wk );		
		if ( IsParentID( wk ) == TRUE ){			
			wk->p_server = BB_Server_AllocMemory( comm_num, &wk->sys );
		}
		wk->p_client = BB_Client_AllocMemory( comm_num, wk->netid, &wk->sys );		
		wk->p_client->rare_game = wk->rule.ball_type;
		wk->p_client->seed_use = wk->seed_tmp;
	}
	
	///< Touch Panel
	{
		u32 active;
		InitTPSystem();
		active = InitTPNoBuff( 4 );
	//	wk->sys.btn = BMN_Create( ball_hit_tbl, BB_TOUCH_HIT_NUM, BB_Client_TouchPanel_CallBack, wk, HEAPID_BB );
	}
	
	///< 2D リソース読み込み
	BB_disp_BG_Load( wk );
	BB_disp_ResourceLoad( wk );
	BB_disp_Manene_Add( wk, wk->p_client );
	BB_disp_Hand_Add( wk->p_client );
	BB_disp_Hanabi_OAM_Add( wk->p_client );
	BB_disp_Light_Add( wk->p_client );
	BB_disp_Pen_Add( wk->p_client );
	BR_ColorChange( wk, NetID_To_PlayerNo( wk, wk->netid ) );
	
	///< 3D リソース
	{		
		///< 新規登録したモデルは、クォータニオンの行列初期化を BB_Client_3D_PosInit ですること
		{	
			int i;
			int data[][2] = {
				{ NARC_manene_kami_blue_nsbmd, NARC_manene_kami_blue_nsbca	},
				{ NARC_manene_kami_eme_nsbmd,  NARC_manene_kami_eme_nsbca	},
				{ NARC_manene_kami_mizu_nsbmd, NARC_manene_kami_mizu_nsbca	},
				{ NARC_manene_kami_pur_nsbmd,  NARC_manene_kami_pur_nsbca	},
				{ NARC_manene_kami_blue_nsbmd, NARC_manene_kami_blue_nsbca	},
				{ NARC_manene_kami_eme_nsbmd,  NARC_manene_kami_eme_nsbca	},
				{ NARC_manene_kami_mizu_nsbmd, NARC_manene_kami_mizu_nsbca	},
				{ NARC_manene_kami_pur_nsbmd,  NARC_manene_kami_pur_nsbca	},
				{ NARC_manene_kami_blue_nsbmd, NARC_manene_kami_blue_nsbca	},
				{ NARC_manene_kami_eme_nsbmd,  NARC_manene_kami_eme_nsbca	},
				{ NARC_manene_kami_mizu_nsbmd, NARC_manene_kami_mizu_nsbca	},
				{ NARC_manene_kami_pur_nsbmd,  NARC_manene_kami_pur_nsbca	},
				{ NARC_manene_kami_blue_nsbmd, NARC_manene_kami_blue_nsbca	},
				{ NARC_manene_kami_eme_nsbmd,  NARC_manene_kami_eme_nsbca	},
				{ NARC_manene_kami_mizu_nsbmd, NARC_manene_kami_mizu_nsbca	},
				{ NARC_manene_kami_pur_nsbmd,  NARC_manene_kami_pur_nsbca	},
			};
			for ( i = 0; i < BB_KAMI_HUBUKI_MAX; i++ ){
				BB_disp_Model_Load( &wk->p_client->bb3d_kami[ i ], wk->sys.p_handle_bb, data[ i ][ 0 ] );
				BB_disp_Model_Init( &wk->p_client->bb3d_kami[ i ], wk->rule.ball_type ); 
			
				D3DOBJ_AnmLoadH( &wk->p_client->bb3d_kami[ i ].anm[ 0 ], &wk->p_client->bb3d_kami[ i ].mdl,
								 wk->sys.p_handle_bb, data[ i ][ 1 ], HEAPID_BB, &wk->sys.allocator );
				D3DOBJ_AddAnm( &wk->p_client->bb3d_kami[ i ].obj, &wk->p_client->bb3d_kami[ i ].anm[ 0 ] );
				D3DOBJ_AnmSet( &wk->p_client->bb3d_kami[ i ].anm[ 0 ], 0 );
				D3DOBJ_SetDraw( &wk->p_client->bb3d_kami[ i ].obj, FALSE );
				
				{
					int ofs = 5;
					
					if ( wk->rule.ball_type ){
						ofs = 5;
					}
							
					if ( i % 2 ){
						wk->p_client->bb3d_kami[ i ].pos.x += FX32_CONST( i * 5 );
					}
					else {
						wk->p_client->bb3d_kami[ i ].pos.x -= FX32_CONST( i * 5 );
					}
					
					if ( i < BB_KAMI_HUBUKI_MAX ){
						wk->p_client->bb3d_kami[ i ].pos.z += FX32_CONST( ( i + 1 ) * ofs );
					}
					else {
						wk->p_client->bb3d_kami[ i ].pos.z -= FX32_CONST( i * 5 );
					}
				}					
			}
		}	
		BB_disp_Model_Load( &wk->p_client->bb3d_spot, wk->sys.p_handle_bb, NARC_manene_spotlight_nsbmd );
		BB_disp_Model_Init( &wk->p_client->bb3d_spot, wk->rule.ball_type );
		D3DOBJ_SetDraw( &wk->p_client->bb3d_spot.obj, FALSE ); 

		{
			int ball_res[] = {
				NARC_manene_ball_red_nsbmd,
				NARC_manene_ball_blue_nsbmd,
				NARC_manene_ball_yellow_nsbmd,
				NARC_manene_ball_green_nsbmd,
			};
			int res_id = ball_res[ NetID_To_PlayerNo( wk, wk->netid ) ];
			BB_disp_Model_Load( &wk->p_client->bb3d_ball, wk->sys.p_handle_bb, res_id );
			BB_disp_Model_Init( &wk->p_client->bb3d_ball, wk->rule.ball_type );
			
			BB_disp_Model_Load( &wk->p_client->bb3d_mane[ 0 ], wk->sys.p_handle_bb, NARC_manene_manene_nsbmd );
			BB_disp_Model_Load( &wk->p_client->bb3d_mane[ 1 ], wk->sys.p_handle_bb, NARC_manene_manene_ochita_nsbmd );
			BB_disp_Model_Load( &wk->p_client->bb3d_mane[ 2 ], wk->sys.p_handle_bb, NARC_manene_manene_fukki_nsbmd );		
			BB_disp_Model_Init( &wk->p_client->bb3d_mane[ 0 ], wk->rule.ball_type );
			BB_disp_Model_Init( &wk->p_client->bb3d_mane[ 1 ], wk->rule.ball_type );
			BB_disp_Model_Init( &wk->p_client->bb3d_mane[ 2 ], wk->rule.ball_type );

			///< アニメ設定
			{
				
				BB_3D_MODEL* b3d;
				int anime;
				
				///< 通常アニメ
				b3d = &wk->p_client->bb3d_mane[ 0 ];
				
				///< 歩く
				anime = NARC_manene_manene_aruku_nsbca;
				D3DOBJ_AnmLoadH( &b3d->anm[ 0 ], &b3d->mdl, wk->sys.p_handle_bb, anime, HEAPID_BB, &wk->sys.allocator );
				D3DOBJ_AddAnm( &b3d->obj, &b3d->anm[ 0 ] );
				D3DOBJ_AnmSet( &b3d->anm[ 0 ], 0 );
				
				///< あらら
				anime = NARC_manene_manene_arara_nsbca;
				D3DOBJ_AnmLoadH( &b3d->anm[ 1 ], &b3d->mdl, wk->sys.p_handle_bb, anime, HEAPID_BB, &wk->sys.allocator );
				D3DOBJ_AnmSet( &b3d->anm[ 1 ], 0 );
				
				///< おっとっと
				anime = NARC_manene_manene_ottoto_nsbca;
				D3DOBJ_AnmLoadH( &b3d->anm[ 2 ], &b3d->mdl, wk->sys.p_handle_bb, anime, HEAPID_BB, &wk->sys.allocator );
				D3DOBJ_AnmSet( &b3d->anm[ 2 ], 0 );
				
				///< 落ちるアニメ
				b3d = &wk->p_client->bb3d_mane[ 1 ];
				anime = NARC_manene_manene_ochita_nsbca;			
				D3DOBJ_AnmLoadH( &b3d->anm[ 0 ], &b3d->mdl, wk->sys.p_handle_bb, anime, HEAPID_BB, &wk->sys.allocator );
				D3DOBJ_AddAnm( &b3d->obj, &b3d->anm[ 0 ] );
				D3DOBJ_AnmSet( &b3d->anm[ 0 ], 0 );
				
				///< 復帰アニメ
				b3d = &wk->p_client->bb3d_mane[ 2 ];
				anime = NARC_manene_manene_fukki_nsbca;			
				D3DOBJ_AnmLoadH( &b3d->anm[ 0 ], &b3d->mdl, wk->sys.p_handle_bb, anime, HEAPID_BB, &wk->sys.allocator );
				D3DOBJ_AddAnm( &b3d->obj, &b3d->anm[ 0 ] );
				D3DOBJ_AnmSet( &b3d->anm[ 0 ], 0 );
			}
		}
		
	    if ( wk->rule.ball_type ){
		    wk->p_client->bb3d_ball.pos.y = BB_MODEL_OFS_Y2;
		    wk->p_client->bb3d_mane[ 0 ].pos.y = BB_MODEL_OFS_Y2;
		    wk->p_client->bb3d_mane[ 1 ].pos.y = BB_MODEL_OFS_Y2;
		    wk->p_client->bb3d_mane[ 2 ].pos.y = BB_MODEL_OFS_Y2;
		}
	}		
//	wk->p_client->bb3d_mane[ 0 ].pos.y -= FX32_CONST( Action_MoveValue_3Y() );
//	wk->p_client->bb3d_mane[ 0 ].pos.z += FX32_CONST( Action_MoveValue_3Z() );
	wk->count_down = MNGM_COUNT_Init( CATS_GetClactSetPtr( wk->sys.crp ), HEAPID_BB );	
	PaletteWorkSet_VramCopy( wk->sys.pfd, FADE_MAIN_OBJ, 0 * 16, 16 * 0x20 );
	
	if ( wk->parent_wk->vchat ){
		mydwc_startvchat( HEAPID_BB );
	}
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	proc	
 * @param	seq	
 *
 * @retval	PROC_RESULT	
 *
 */
//--------------------------------------------------------------
PROC_RESULT BalanceBallProc_Init( PROC* proc, int* seq )
{
	BB_WORK* wk;
	
	sys_CreateHeap( HEAPID_BASE_APP, HEAPID_BB, HEAP_SIZE_BB );
	
	wk = PROC_AllocWork( proc, sizeof( BB_WORK ), HEAPID_BB );
	memset( wk, 0, sizeof( BB_WORK ) );
	
	wk->parent_wk = PROC_GetParentWork( proc );
	
	{
			
		int i;
		int no = 0;
		
		for( i = 0; i < 4; i++ ){
			
			wk->table[ i ] = 0xFF;			
			if( CommInfoGetMyStatus( i ) != NULL ){
				wk->table[ no ] = i;
				no++;
			}
		}
	}

	MNGM_ENRES_PARAM_Init( &wk->entry_param, wk->parent_wk->wifi_lobby, wk->parent_wk->p_save, wk->parent_wk->vchat, &wk->parent_wk->lobby_wk );

	return PROC_RES_FINISH;
}

static void BB_PenDemo( BB_CLIENT* wk, u32 x, u32 y )
{
	s16 mx, my;
	BOOL bMove;
	
	if ( x == 0 && y == 0 ){		
		wk->control.old_x = 0;
		wk->control.old_y = 0;		
		if ( wk->bb3d_ball.pow_count != 0 ){
			bMove = Quaternion_Rotation_Pow( &wk->bb3d_ball, 1.0f );
		//	bMove = Quaternion_Rotation_Pow( &wk->bb3d_mane[ 0 ], 0.5f );
			if ( bMove = TRUE )	{
				VecFx32 vec = { 0, BB_POINT_Y, 0 };
				VecFx32 tmp;
				MTX_MultVec43( &vec, &wk->bb3d_mane[ 0 ].tmp43, &wk->mane_pos );
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
		if ( wk->control.old_x == 0 ){
			wk->control.old_x = x;
			wk->control.old_y = y;
		}		
		bMove = Quaternion_Rotation( &wk->bb3d_ball, x, y, wk->control.old_x, wk->control.old_y, BB_COEFFICIENT_AB_PEN, TRUE );
	//	bMove = Quaternion_Rotation( &wk->bb3d_mane[ 0 ], x, y, wk->control.old_x, wk->control.old_y, BB_COEFFICIENT_AM - py, TRUE );		
		if ( bMove = TRUE ){
			VecFx32 vec = { 0, BB_POINT_Y, 0 };
			VecFx32 tmp;
			MTX_MultVec43( &vec, &wk->bb3d_mane[ 0 ].tmp43, &wk->mane_pos );
		}		
		if ( Snd_SePlayCheck( BB_SND_KIRAKIRA ) == 0 ){
			Snd_SePlay( BB_SND_KIRAKIRA );			///< きらきら
		}	
		wk->control.old_x = x;
		wk->control.old_y = y;
	}
}

static BOOL PenDemo( BB_CLIENT* wk )
{
	int now;
	int index;
	int x, y;
	s16 pos[][ 2 ] = {
		{ 0,0 },{ 0,-40 },{ 0,-30 },{ 0,-20 },{ 0,-10 },{ 0,0 },{ 0,10 },{ 0,20 },{ 0,30 },{ 0,40 },{ 0,00 },{ 0,0 },
		{ 0,0 },{ 0,-40 },{ 0,-30 },{ 0,-20 },{ 0,-10 },{ 0,0 },{ 0,10 },{ 0,20 },{ 0,30 },{ 0,40 },{ 0,00 },{ 0,0 },
		{ 0,0 },{ 10,0 },{ 20,0 },{ 30,0 },{ 40,0 },{ 50,0 },{ 60,0 },{ 70,0 },{ 80,0 },{ 90,0 },{ 0,0 },{ 0,0 },
		{ 0,0 },{ 10,0 },{ 20,0 },{ 30,0 },{ 40,0 },{ 50,0 },{ 60,0 },{ 70,0 },{ 80,0 },{ 90,0 },{ 0,0 },{ 0,0 },
	};
	
	now = CATS_ObjectAnimeFrameGetCap( wk->cap_pen );
//	OS_Printf( "now = %d\n", now );
	index = ( wk->seq2 * 12 ) + now;
	x = pos[ index ][ 0 ];
	y = pos[ index ][ 1 ];

	if ( x == 0 && y == 0 ){
		BB_PenDemo( wk, 0, 0 );
	}
	else {
		BB_PenDemo( wk, 128 + x, 128 + y );		
		if ( x != 0 ){
			BB_Stardust_Call( wk,  88 + x, 128 + y );			
		}
		else {
			BB_Stardust_Call( wk, 128 + x, 128 + y );
		}
	}
	
	if ( now == 0 ){
		if ( wk->seq != now ){
			wk->seq2++;
		}
	}
	wk->seq = now;
	if ( wk->seq2 == 2 ){
		if ( CATS_ObjectAnimeSeqGetCap( wk->cap_pen ) != 2 ){
			CATS_ObjectPosSetCap( wk->cap_pen, 128, 100 );
			CATS_ObjectAnimeSeqCheckSetCap( wk->cap_pen, 2 );
			Quaternion_Identity( &wk->bb3d_ball.tq );
			Quaternion_Identity( &wk->bb3d_ball.cq );
			Quaternion_Rotation( &wk->bb3d_ball, 0, 0, +5, +5, BB_COEFFICIENT_AMI, FALSE );
			Quaternion_Rotation( &wk->bb3d_ball, 0, 0, -5, -5, BB_COEFFICIENT_AMI, FALSE );	
		}
	}
	if ( wk->seq2 == 3 && now == 11 ){
		wk->seq  = 0;
		wk->seq2 = 0;
		CATS_ObjectEnableCap( wk->cap_pen, FALSE );
		Quaternion_Identity( &wk->bb3d_ball.tq );
		Quaternion_Identity( &wk->bb3d_ball.cq );
		Quaternion_Rotation( &wk->bb3d_ball, 0, 0, +5, +5, BB_COEFFICIENT_AMI, FALSE );
		Quaternion_Rotation( &wk->bb3d_ball, 0, 0, -5, -5, BB_COEFFICIENT_AMI, FALSE );	
		return TRUE;
	}
	CATS_ObjectUpdateCapEx( wk->cap_pen );
	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	proc	
 * @param	seq	
 *
 * @retval	PROC_RESULT	
 *
 */
//--------------------------------------------------------------
PROC_RESULT BalanceBallProc_Main( PROC* proc, int* seq )
{
	BB_WORK* wk = PROC_GetWork( proc );
	BOOL bEnd = FALSE;
	u32 dis_error;

	// 切断エラーチェック
	dis_error = BB_DIS_ERROR_Check( wk );
	switch( dis_error ){
	case BB_DIS_ERROR_CLOSEING:		// 切断エラー	終了中
		return PROC_RES_CONTINUE;

	case BB_DIS_ERROR_CLOSED:		// 切断エラー	終了
		return PROC_RES_FINISH;

	case BB_DIS_ERROR_NONE:			// エラーなし
	default:
		break;
	}

		
	switch ( *seq ){
	
	case eBB_SEQ_ENTRY_INIT:
		///< エントリー初期化
		bEnd = TRUE;
		wk->entry_work = MNGM_ENTRY_InitBalanceBall( &wk->entry_param, HEAPID_BB );
		BB_MainSeq_Change( wk, bEnd, eBB_SEQ_ENTRY_MAIN, seq );
		break;

	case eBB_SEQ_ENTRY_MAIN:
		///< エントリー中
		bEnd = MNGM_ENTRY_Wait( wk->entry_work );
		BB_MainSeq_Change( wk, bEnd, eBB_SEQ_ENTRY_EXIT, seq );
		break;

	case eBB_SEQ_ENTRY_EXIT:
		///< エントリー終了
		bEnd = TRUE;
		wk->rule.ball_type = MNGM_ENTRY_GetRareGame( wk->entry_work );
		MNGM_ENTRY_Exit( wk->entry_work );
		wk->entry_work = NULL;
#ifdef PM_DEBUG
		if ( sys.cont & PAD_BUTTON_A ){
			wk->rule.ball_type = 1;
		}
#endif
		BalanceBall_MainInit( wk );
		BB_MainSeq_Change( wk, bEnd, eBB_SEQ_FADE_IN, seq );
		break;
	
	case eBB_SEQ_FADE_IN:
		///< 画面フェードイン開始
		bEnd = BB_WipeStart( WIPE_TYPE_DOORIN );
		BB_disp_NameWinAdd( wk, wk->p_client );
		BB_MainSeq_Change( wk, bEnd, eBB_SEQ_FADE_IN_WAIT, seq );
		break;
	
	
	case eBB_SEQ_FADE_IN_WAIT:
		///< 画面フェードイン中
		bEnd = WIPE_SYS_EndCheck();
		
	#ifdef PM_DEBUG
	#ifdef BB_GAME_SETUP_ON		
		if ( bEnd ){
			
			BOOL bSetup = Debug_GameSetup( wk );
			
			if ( bSetup == FALSE ){ return PROC_RES_CONTINUE; }
		}
	#endif
	#endif
		
		BB_MainSeq_Change( wk, bEnd, eBB_SEQ_START, seq );
		break;
		
	case eBB_SEQ_START:
		///< スタートリクエスト
		bEnd = TRUE;//BB_Client_JumpOnToBall( wk->p_client );
		if ( bEnd == FALSE ){ break; }		
	//	BB_disp_NameWinDel( wk->p_client );
		BB_disp_InfoWinAdd( wk->p_client );
		BB_Effect_Call( wk->p_client );
		BB_Fever_Call( wk->p_client );
		BB_MainSeq_Change( wk, bEnd, eBB_SEQ_PEN_DEMO, seq );
		break;

	case eBB_SEQ_PEN_DEMO:
		///< ペンデモ
		bEnd = PenDemo( wk->p_client );
		if ( bEnd ){
			if ( IsParentID( wk ) == TRUE ){
				wk->rule.rand	   = wk->seed_tmp;
				CommSendData( CCMD_BB_START, &wk->rule, sizeof( BB_RULE_COMM ) );
			}
		}
		BB_MainSeq_Change( wk, bEnd, eBB_SEQ_START_WAIT, seq );
		break;	
	
	case eBB_SEQ_START_WAIT:
		///< スタート待ち
		bEnd = wk->bStart;	
		BB_MainSeq_Change( wk, bEnd, eBB_SEQ_COUNT_DOWN, seq );
		break;
	
	
	case eBB_SEQ_COUNT_DOWN:
		///< カウントダウン
		bEnd = TRUE;		
		MNGM_COUNT_StartStart( wk->count_down );		
		BB_MainSeq_Change( wk, bEnd, eBB_SEQ_COUNT_DOWN_WAIT, seq );
		break;
	
		
	case eBB_SEQ_COUNT_DOWN_WAIT:
		///< カウントダウン中
		bEnd = TRUE;
		bEnd = MNGM_COUNT_Wait( wk->count_down );		
		if ( bEnd ){
			BB_disp_NameWinDel( wk->p_client );
			BB_disp_InfoWinDel( wk->p_client );
		}	
		BB_MainSeq_Change( wk, bEnd, eBB_SEQ_MAIN, seq );
		break;
	
			
	case eBB_SEQ_MAIN:
		bEnd = wk->bEnd;
		if ( wk->bEnd ){
			if ( wk->p_client->game_sys.game_timer < BB_GAME_TIMER ){
				bEnd = FALSE;
				OS_Printf( " 親機が終了して、子機はまだ終わってない\n" );
			}
		}				
		if ( bEnd == FALSE ){			
			if ( IsParentID( wk ) == TRUE ){
				
				if ( BB_Server_Main( wk->p_server ) == FALSE ){
			#ifdef BB_GAME_ENDLESS	
					if ( sys.trg & PAD_BUTTON_B ){
						CommSendData( CCMD_BB_END, NULL, 0 );
						break;	
					}
			#else
					Snd_SeStopBySeqNo( BB_SND_HAND, 0 );
					CommSendData( CCMD_BB_END, NULL, 0 );
			#endif
				}
			}	
			BB_Client_GameCore( wk->p_client );
		}
		BB_MainSeq_Change( wk, bEnd, eBB_SEQ_MAIN_END_INIT, seq );
		break;
	
	case eBB_SEQ_MAIN_END_INIT:
		bEnd = TRUE;
		Snd_SeStopBySeqNo( BB_SND_HANABI, 0 );			///< 花火どがーん停止
		Snd_SeStopBySeqNo( BB_SND_HAND, 0 );			///< 拍手も停止
		MNGM_COUNT_StartTimeUp( wk->count_down );	
		BB_MainSeq_Change( wk, bEnd, eBB_SEQ_MAIN_END, seq );
		break;
		
	case eBB_SEQ_MAIN_END:
		bEnd = MNGM_COUNT_Wait( wk->count_down );
		BB_MainSeq_Change( wk, bEnd, eBB_SEQ_RESULT, seq );
		break;
		
	case eBB_SEQ_RESULT:
		bEnd = TRUE;
		wk->result.score = BB_P_TO_SCORE( wk->p_client->game_sys.point );//wk->p_client->game_sys.result_time;
		if ( wk->result.score >= BB_SCORE_MAX ){
			 wk->result.score = BB_SCORE_MAX;
		}
		if ( wk->p_client->game_sys.bFirstTouch == FALSE ){
			 wk->result.score = BB_SCORE_MIN_CHK;
		}
//		OS_Printf( "触った？ = %d\n", wk->p_client->game_sys.bFirstTouch ); 
//		OS_Printf( "point    = %d\n", wk->p_client->game_sys.point ); 
//		OS_Printf( "score    = %d\n", wk->result.score ); 
		CommSendData( CCMD_BB_RESULT, &wk->result, sizeof( BB_RESULT_COMM ) );
		BB_MainSeq_Change( wk, bEnd, eBB_SEQ_RESULT_WAIT, seq );
		break;
	
		
	case eBB_SEQ_RESULT_WAIT:
		bEnd = FALSE;
		{
			int i;
			int num = 0;
			
			for ( i = 0; i < 4; i++ ){
				if ( wk->mem_result.result[ i ].score != 0 ){
					num++;
				}
			}
			if ( num == wk->p_client->comm_num ){
				bEnd = TRUE;
			}
		}
		BB_MainSeq_Change( wk, bEnd, eBB_SEQ_FADE_OUT, seq );
		break;
	
	case eBB_SEQ_FADE_OUT:
		bEnd = BB_WipeStart( WIPE_TYPE_DOOROUT );
		
		BB_MainSeq_Change( wk, bEnd, eBB_SEQ_FADE_OUT_WAIT, seq );
		break;
	
	
	case eBB_SEQ_FADE_OUT_WAIT:
		bEnd = WIPE_SYS_EndCheck();
		
		if ( bEnd ){
			MainResource_Delete( wk );
		}
		
		BB_MainSeq_Change( wk, bEnd, eBB_SEQ_SCORE, seq );
		break;
	
	
	case eBB_SEQ_SCORE:
		bEnd = TRUE;
		{
			int i;
			int id;			
			for ( i = 0; i < 4; i++ ){
				id = NetID_To_PlayerNo( wk, i );
				if ( id == 0xFF ){ continue; }
				wk->result_param.score[ id ]  = wk->mem_result.result[ i ].score;
				if ( wk->result_param.score[ id ] <= BB_SCORE_MIN_CHK ){
					wk->result_param.score[ id ] = BB_SCORE_SET_MIN;
				}
			}
			// ランキング計算
//			wk->result_param.p_gadget = &wk->parent_wk->gadget;	// GADGETなくしました　tomoya
			MNGM_RESULT_CalcRank( &wk->result_param, wk->entry_param.num );
		}
		wk->result_work = MNGM_RESULT_InitBalanceBall( &wk->entry_param, &wk->result_param, HEAPID_BB );
		

		BB_MainSeq_Change( wk, bEnd, eBB_SEQ_SCORE_WAIT, seq );
		break;
	
	
	case eBB_SEQ_SCORE_WAIT:
		bEnd = MNGM_RESULT_Wait( wk->result_work );
		
		if ( bEnd ){
			BOOL replay = MNGM_RESULT_GetReplay( wk->result_work );
			MNGM_RESULT_Exit( wk->result_work );
			wk->result_work = NULL;
			
			if ( replay ){
				wk->bRePlay = TRUE;
				Reset_GameData( wk );				
				BB_MainSeq_Change( wk, bEnd, eBB_SEQ_ENTRY_INIT, seq );
				if ( wk->parent_wk->vchat ){
					mydwc_stopvchat();
				}
				break;
			}
		}	
		BB_MainSeq_Change( wk, bEnd, eBB_SEQ_END, seq );
		break;
	
	case eBB_SEQ_END:
		bEnd = TRUE;
		
		CommTimingSyncStart( CCMD_BB_CONNECT_END );
			
		if ( wk->parent_wk->vchat ){
			mydwc_stopvchat();
		}
		BB_MainSeq_Change( wk, bEnd, eBB_SEQ_END_WAIT, seq );
		break;
	
		
	case eBB_SEQ_END_WAIT:
	default:
		bEnd = CommIsTimingSync( CCMD_BB_CONNECT_END );
		return ( bEnd == TRUE ) ? PROC_RES_FINISH : PROC_RES_CONTINUE;
		break;
	}
	
	if ( *seq >= eBB_SEQ_FADE_IN  && *seq < eBB_SEQ_FADE_OUT_WAIT ){		
		BB_disp_Manene_OAM_Update( wk->p_client );
//		BB_disp_Hanabi_OAM_Update( wk->p_client );
		BB_disp_Draw( wk );
	}
	
	return PROC_RES_CONTINUE;
}

static void MainResource_Delete( BB_WORK* wk )
{
	sys_VBlankFuncChange( NULL, NULL );
	sys_HBlankIntrStop();
	DellVramTransferManager();

	///< touch panel
	{
		{
			u32 active;
			active = StopTP();
		}
	//	BMN_Delete( wk->sys.btn );
	}
	
	///< 乱数のタネ戻す
	gf_srand( wk->seed_tmp );

	///<

	///< OAM リソース解放
	BB_disp_Manene_OAM_Del( wk->p_client );
	BB_disp_Hand_Del( wk->p_client );
	BB_disp_Hanabi_OAM_Del( wk->p_client );
	BB_disp_Light_Del( wk->p_client );
	BB_disp_Pen_Del( wk->p_client );
	
	///< 3D 解放	
	BB_disp_Model_Delete( &wk->p_client->bb3d_ball );
	BB_disp_Model_Delete( &wk->p_client->bb3d_mane[ 0 ] );
	BB_disp_Model_Delete( &wk->p_client->bb3d_mane[ 1 ] );
	BB_disp_Model_Delete( &wk->p_client->bb3d_mane[ 2 ] );
	BB_disp_Model_Delete( &wk->p_client->bb3d_spot );
	
//	D3DOBJ_AnmDelete( &wk->p_client->bb3d_spot.anm[ 0 ], &wk->sys.allocator );
	D3DOBJ_AnmDelete( &wk->p_client->bb3d_mane[ 0 ].anm[ 0 ], &wk->sys.allocator );
	D3DOBJ_AnmDelete( &wk->p_client->bb3d_mane[ 0 ].anm[ 1 ], &wk->sys.allocator );
	D3DOBJ_AnmDelete( &wk->p_client->bb3d_mane[ 0 ].anm[ 2 ], &wk->sys.allocator );
	D3DOBJ_AnmDelete( &wk->p_client->bb3d_mane[ 1 ].anm[ 0 ], &wk->sys.allocator );
	D3DOBJ_AnmDelete( &wk->p_client->bb3d_mane[ 2 ].anm[ 0 ], &wk->sys.allocator );
	
	{
		int i;
		for ( i = 0; i < BB_KAMI_HUBUKI_MAX; i++ ){
			BB_disp_Model_Delete( &wk->p_client->bb3d_kami[ i ] );
			D3DOBJ_AnmDelete( &wk->p_client->bb3d_kami[ i ].anm[ 0 ], &wk->sys.allocator );
			D3DOBJ_AnmDelete( &wk->p_client->bb3d_kami[ i ].anm[ 1 ], &wk->sys.allocator );
		}
	}

	MNGM_COUNT_Exit( wk->count_down );

	BB_SystemExit( wk );

	///< ワーク解放
	if ( IsParentID( wk ) == TRUE ){
		
		BB_Server_FreeMemory( wk->p_server );
	}
	BB_Client_FreeMemory( wk->p_client );

	wk->p_server = NULL;
	wk->p_client = NULL;
}


//--------------------------------------------------------------
/**
 * @brief	ゲームリセットしなくちゃいけない部分
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void Reset_GameData( BB_WORK* wk )
{
	wk->bEnd	= FALSE;
	wk->bStart	= FALSE;
	wk->bResult = FALSE;
	
	memset( &wk->result, 0, sizeof( BB_RESULT_COMM ) );
	memset( &wk->mem_result, 0, sizeof( BB_MENBER_RESULT_COMM ) );
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	proc	
 * @param	seq	
 *
 * @retval	PROC_RESULT	
 *
 */
//--------------------------------------------------------------
PROC_RESULT BalanceBallProc_Exit( PROC* proc, int* seq )
{	
	switch ( *seq ){
	case 0:
		{
			u32 dis_error;
			BB_WORK* wk = PROC_GetWork( proc );
			dis_error = BB_DIS_ERROR_Check( wk );	// 切断エラーチェック
	
			PROC_FreeWork( proc );
			sys_DeleteHeap( HEAPID_BB );
			CommStateSetErrorCheck( FALSE, TRUE );

			if( dis_error != BB_DIS_ERROR_NONE ){
				return PROC_RES_FINISH;
			}

			CommTimingSyncStart( BB_COMM_END_CMD );
			(*seq)++;
		}
		break;

	default:
		if( (CommIsTimingSync( BB_COMM_END_CMD ) == TRUE) || 
			(CommGetConnectNum() < CommInfoGetEntryNum()) ){	// 人数が少なくなったらそのまま抜ける
			return PROC_RES_FINISH;
		}
		break;
	}

	return PROC_RES_CONTINUE;
}








//--------------------------------------------------------------
/**
 * @brief	SYSTEM 初期化
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BB_SystemInit( BB_WORK* wk )
{
	wk->sys.p_handle_bb = ArchiveDataHandleOpen( ARCID_BB_RES, HEAPID_BB );	;
	wk->sys.p_handle_cr = ArchiveDataHandleOpen( ARCID_COMMON_RES, HEAPID_BB );	
	wk->sys.bgl			= GF_BGL_BglIniAlloc( HEAPID_BB );
	wk->sys.pfd			= PaletteFadeInit( HEAPID_BB );
	wk->sys.g3Dman		= GF_G3DMAN_Init( HEAPID_BB, GF_G3DMAN_LNK, GF_G3DTEX_128K, GF_G3DMAN_LNK, GF_G3DPLT_64K, NULL );
	
	wk->sys.camera_p	= GFC_AllocCamera( HEAPID_BB );
	BB_disp_CameraSet( wk );
	
	PaletteTrans_AutoSet( wk->sys.pfd, TRUE );
	PaletteFadeWorkAllocSet( wk->sys.pfd, FADE_MAIN_BG,	 0x200, HEAPID_BB );
	PaletteFadeWorkAllocSet( wk->sys.pfd, FADE_SUB_BG,	 0x200, HEAPID_BB );
	PaletteFadeWorkAllocSet( wk->sys.pfd, FADE_MAIN_OBJ, 0x200, HEAPID_BB );
	PaletteFadeWorkAllocSet( wk->sys.pfd, FADE_SUB_OBJ,	 0x200, HEAPID_BB );

	BB_VramBankSet( wk->sys.bgl );
	BB_CATS_Init( wk );
	
	G3X_AlphaBlend( TRUE );
	
	{
		static const GXRgb edge_color_table[ 8 ] = {
			GX_RGB( 0, 0, 0),GX_RGB( 4, 4, 4),GX_RGB( 4, 4, 4),GX_RGB( 4, 4, 4),
			GX_RGB( 4, 4, 4),GX_RGB( 4, 4, 4),GX_RGB( 4, 4, 4),GX_RGB( 4, 4, 4),
		};	
		G3X_EdgeMarking( TRUE );
		G3X_SetEdgeColorTable( edge_color_table );
	}
	
	sys_InitAllocator( &wk->sys.allocator, HEAPID_BB, 32 );
}


//--------------------------------------------------------------
/**
 * @brief	SYSTEM 解放
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_SystemExit( BB_WORK* wk )
{	
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0,  VISIBLE_OFF );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1,  VISIBLE_OFF );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2,  VISIBLE_OFF );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3,  VISIBLE_OFF );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );
	GF_BGL_BGControlExit( wk->sys.bgl, GF_BGL_FRAME0_M );
	GF_BGL_BGControlExit( wk->sys.bgl, GF_BGL_FRAME1_M );
	GF_BGL_BGControlExit( wk->sys.bgl, GF_BGL_FRAME2_M );
	GF_BGL_BGControlExit( wk->sys.bgl, GF_BGL_FRAME3_M );
	GF_BGL_BGControlExit( wk->sys.bgl, GF_BGL_FRAME0_S );
	GF_BGL_BGControlExit( wk->sys.bgl, GF_BGL_FRAME1_S );
	GF_BGL_BGControlExit( wk->sys.bgl, GF_BGL_FRAME2_S );
	GF_BGL_BGControlExit( wk->sys.bgl, GF_BGL_FRAME3_S );
	sys_FreeMemoryEz( wk->sys.bgl );

	PaletteFadeWorkAllocFree( wk->sys.pfd, FADE_MAIN_BG );
	PaletteFadeWorkAllocFree( wk->sys.pfd, FADE_SUB_BG );
	PaletteFadeWorkAllocFree( wk->sys.pfd, FADE_MAIN_OBJ );
	PaletteFadeWorkAllocFree( wk->sys.pfd, FADE_SUB_OBJ );
	PaletteFadeFree( wk->sys.pfd );

	ArchiveDataHandleClose( wk->sys.p_handle_bb );
	ArchiveDataHandleClose( wk->sys.p_handle_cr );
	
	GF_G3D_Exit( wk->sys.g3Dman );
	GFC_FreeCamera( wk->sys.camera_p );
	
	
	CATS_ResourceDestructor_S( wk->sys.csp, wk->sys.crp );
	CATS_FreeMemory( wk->sys.csp );
	
	sys.disp3DSW = DISP_3D_TO_MAIN;
	GF_Disp_DispSelect();
	
	G3X_AlphaBlend( FALSE );
}

//--------------------------------------------------------------
/**
 * @brief	VRAM 設定
 *
 * @param	bgl	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BB_VramBankSet( GF_BGL_INI* bgl )
{
	GF_Disp_GX_VisibleControlInit();
	
	{	// BG SYSTEM
		GF_BGL_SYS_HEADER BGsys_data = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_3D,
		};
		GF_BGL_InitBG( &BGsys_data );
	}
	
	//VRAM設定
	{
		GF_BGL_DISPVRAM vramSetTable = {
			GX_VRAM_BG_128_A,				// メイン2DエンジンのBG
			GX_VRAM_BGEXTPLTT_NONE,			// メイン2DエンジンのBG拡張パレット
			GX_VRAM_SUB_BG_128_C,			// サブ2DエンジンのBG
			GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット
			GX_VRAM_OBJ_64_E,				// メイン2DエンジンのOBJ
			GX_VRAM_OBJEXTPLTT_NONE,		// メイン2DエンジンのOBJ拡張パレット
			GX_VRAM_SUB_OBJ_16_I,			// サブ2DエンジンのOBJ
			GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット
			GX_VRAM_TEX_0_B,				// テクスチャイメージスロット
			GX_VRAM_TEXPLTT_01_FG			// テクスチャパレットスロット
		};
		GF_Disp_SetBank( &vramSetTable );

		//VRAMクリア
		MI_CpuClear32((void*)HW_BG_VRAM, HW_BG_VRAM_SIZE);
		MI_CpuClear32((void*)HW_DB_BG_VRAM, HW_DB_BG_VRAM_SIZE);
		MI_CpuClear32((void*)HW_OBJ_VRAM, HW_OBJ_VRAM_SIZE);
		MI_CpuClear32((void*)HW_DB_OBJ_VRAM, HW_DB_OBJ_VRAM_SIZE);
	}
	
	//メイン画面フレーム設定
	{
		GF_BGL_BGCNT_HEADER TextBgCntDat[] = {
			///<FRAME0_M　
			{
				0, 0, 0x0800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
				3, 0, 0, FALSE
			},
			///<FRAME1_M　左幕
			{
				0, 0, 0x1000, 0, GF_BGL_SCRSIZ_512x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x2800, GX_BG_CHARBASE_0x14000, GX_BG_EXTPLTT_01,
				2, 0, 0, FALSE
			},
			///<FRAME2_M　右幕
			{
				0, 0, 0x1000, 0, GF_BGL_SCRSIZ_512x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x3800, GX_BG_CHARBASE_0x14000, GX_BG_EXTPLTT_01,
				2, 0, 0, FALSE
			},
			///<FRAME3_M　背景
			{
				0, 0, 0x0800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x4800, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
				3, 0, 0, FALSE
			},
		};
//		GF_BGL_ClearCharSet( GF_BGL_FRAME0_M, 32, 0, HEAPID_BB );
		GF_BGL_ClearCharSet( GF_BGL_FRAME1_M, 32, 0, HEAPID_BB );
		GF_BGL_ClearCharSet( GF_BGL_FRAME2_M, 32, 0, HEAPID_BB );
		GF_BGL_ClearCharSet( GF_BGL_FRAME3_M, 32, 0, HEAPID_BB );
//		GF_BGL_BGControlSet( bgl, GF_BGL_FRAME0_M, &TextBgCntDat[ 0 ], GF_BGL_MODE_TEXT );
		GF_BGL_BGControlSet( bgl, GF_BGL_FRAME1_M, &TextBgCntDat[ 1 ], GF_BGL_MODE_TEXT );
		GF_BGL_BGControlSet( bgl, GF_BGL_FRAME2_M, &TextBgCntDat[ 2 ], GF_BGL_MODE_TEXT );
		GF_BGL_BGControlSet( bgl, GF_BGL_FRAME3_M, &TextBgCntDat[ 3 ], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear( bgl, GF_BGL_FRAME0_M );
		GF_BGL_ScrClear( bgl, GF_BGL_FRAME1_M );
		GF_BGL_ScrClear( bgl, GF_BGL_FRAME2_M );
		GF_BGL_ScrClear( bgl, GF_BGL_FRAME3_M );
	}
	///< サブ画面フレーム設定
	{
		GF_BGL_BGCNT_HEADER TextBgCntDat[] = {
			///<FRAME0_S　背景
			{
				0, 0, 0x0800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0xb000, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
				3, 0, 0, FALSE
			},
			///<FRAME1_S　←幕
			{
				0, 0, 0x1000, 0, GF_BGL_SCRSIZ_512x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0xb800, GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01,
				1, 0, 0, FALSE
			},
			///<FRAME2_S　→幕
			{
				0, 0, 0x1000, 0, GF_BGL_SCRSIZ_512x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0xc800, GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01,
				1, 0, 0, FALSE
			},
			///<FRAME3_S　↑幕
			{
				0, 0, 0x1000, 0, GF_BGL_SCRSIZ_256x512, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0xd800, GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01,
				0, 0, 0, FALSE
			},
		};
		GF_BGL_ClearCharSet( GF_BGL_FRAME0_S, 32, 0, HEAPID_BB );
		GF_BGL_ClearCharSet( GF_BGL_FRAME1_S, 32, 0, HEAPID_BB );
		GF_BGL_ClearCharSet( GF_BGL_FRAME2_S, 32, 0, HEAPID_BB );
		GF_BGL_ClearCharSet( GF_BGL_FRAME3_S, 32, 0, HEAPID_BB );
		GF_BGL_BGControlSet( bgl, GF_BGL_FRAME0_S, &TextBgCntDat[ 0 ], GF_BGL_MODE_TEXT );
		GF_BGL_BGControlSet( bgl, GF_BGL_FRAME1_S, &TextBgCntDat[ 1 ], GF_BGL_MODE_TEXT );
		GF_BGL_BGControlSet( bgl, GF_BGL_FRAME2_S, &TextBgCntDat[ 2 ], GF_BGL_MODE_TEXT );
		GF_BGL_BGControlSet( bgl, GF_BGL_FRAME3_S, &TextBgCntDat[ 3 ], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear( bgl, GF_BGL_FRAME0_S );
		GF_BGL_ScrClear( bgl, GF_BGL_FRAME1_S );
		GF_BGL_ScrClear( bgl, GF_BGL_FRAME2_S );
		GF_BGL_ScrClear( bgl, GF_BGL_FRAME3_S );
	}
	GF_BGL_ClearCharSet( GF_BGL_FRAME0_S, 32, 0, HEAPID_BB );
	GF_BGL_ClearCharSet( GF_BGL_FRAME1_S, 32, 0, HEAPID_BB );
	GF_BGL_ClearCharSet( GF_BGL_FRAME2_S, 32, 0, HEAPID_BB );
	GF_BGL_ClearCharSet( GF_BGL_FRAME3_S, 32, 0, HEAPID_BB );
	
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
	
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
	
	///< SUB画面をメイン画面にするため
	sys.disp3DSW = DISP_3D_TO_SUB;
	GF_Disp_DispSelect();	
	
	G2_SetBlendAlpha(  0, GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ, 7, 10 );					  
	G2S_SetBlendAlpha( 0, GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_OBJ, 7, 10 );
}

//--------------------------------------------------------------
/**
 * @brief	CATS の 初期化
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BB_CATS_Init( BB_WORK* wk )
{
	wk->sys.csp = CATS_AllocMemory( HEAPID_BB );
	{
		const TCATS_OAM_INIT OamInit = {
			0, 128, 0, 32,
			0, 128, 0, 32,
		};
		const TCATS_CHAR_MANAGER_MAKE Ccmm = {
			64 + 64,			///< ID管理 main + sub
			1024 * 0x40,		///< 64k
			 512 * 0x20,		///< 32k
			GX_OBJVRAMMODE_CHAR_1D_128K,
			GX_OBJVRAMMODE_CHAR_1D_32K
		};
		CATS_SystemInit( wk->sys.csp, &OamInit, &Ccmm, 16 + 16 );
	}
	
	{
		BOOL active;
		const TCATS_RESOURCE_NUM_LIST ResourceList = {
			64 + 64,	///< キャラ登録数 	main + sub
			16 + 16,	///< パレット登録数	main + sub
			128,		///< セル
			128,		///< セルアニメ
			16,			///< マルチセル
			16,			///< マルチセルアニメ
		};
		
		wk->sys.crp = CATS_ResourceCreate( wk->sys.csp );

		active   = CATS_ClactSetInit( wk->sys.csp, wk->sys.crp, 255 );
		GF_ASSERT( active );
		
		active	  = CATS_ResourceManagerInit( wk->sys.csp, wk->sys.crp, &ResourceList );
		GF_ASSERT( active );
	}
	CLACT_U_SetSubSurfaceMatrix( CATS_EasyRenderGet( wk->sys.csp ), 0, BB_SURFACE_Y << FX32_SHIFT );
}

//--------------------------------------------------------------
/**
 * @brief	ワイプ処理
 *
 * @param	type	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static BOOL BB_WipeStart( int type )
{
	WIPE_SYS_Start( WIPE_PATTERN_WMS, type, type,
					WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_BB );
	
	return TRUE;
}


//--------------------------------------------------------------
/**
 * @brief	シーケンス切り替え
 *
 * @param	wk			処理するワーク
 * @param	bEnd		終了フラグ
 * @param	next_seq	次のシーケンス
 * @param	seq			シーケンス
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BB_MainSeq_Change( BB_WORK* wk, BOOL bEnd, int next_seq, int* seq )
{
	if ( bEnd ){

		*seq = next_seq;
	}
}


//--------------------------------------------------------------
/**
 * @brief	VBlank 関数
 *
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BB_VBlank( void* work )
{
	BB_WORK* wk = work;
	
	DoVramTransferManager();
	
	CATS_RenderOamTrans();

	PaletteFadeTrans( wk->sys.pfd );
	
	GF_BGL_VBlankFunc( wk->sys.bgl );
	
	OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}



















/// コマンド系

/// public

//--------------------------------------------------------------
/**
 * @brief	親機か？
 *
 * @param	wk	
 *
 * @retval	BOOL	TRUE=親
 *
 */
//--------------------------------------------------------------
BOOL IsParentID( BB_WORK* wk )
{	
	return ( wk->netid == COMM_PARENT_ID ) ? TRUE : FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	開始コマンド
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_cmd_StartSet( BB_WORK* wk )
{
	wk->bStart = TRUE;
}


//--------------------------------------------------------------
/**
 * @brief	終了コマンド
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_cmd_EndSet( BB_WORK* wk )
{
	wk->bEnd = TRUE;
}



//----------------------------------------------------------------------------
/**
 *	@brief	切断エラーチェック　終了管理関数
 *
 *	@param	wk		ワーク
 *
 *	@retval	BB_DIS_ERROR_NONE,			// エラーなし
 *	@retval	BB_DIS_ERROR_CLOSEING,		// 切断エラー	終了中
 *	@retval	BB_DIS_ERROR_CLOSED,		// 切断エラー	終了
 */
//-----------------------------------------------------------------------------
static u32 BB_DIS_ERROR_Check( BB_WORK* wk )
{
	// 通信切断チェック
	if( MNGM_ERROR_CheckDisconnect( &wk->entry_param ) == TRUE ){

		if( wk->sys.comm_err_data.dis_err == FALSE ){
			// 真っ黒にする
			if( WIPE_SYS_EndCheck() == TRUE ){
				WIPE_SYS_ExeEnd();
			}
			WIPE_SetBrightnessFadeOut(WIPE_FADE_BLACK);

			// 終了宣言
			wk->sys.comm_err_data.dis_err = TRUE;

			return BB_DIS_ERROR_CLOSEING;
		}else{

			// まづは通信切断
			if( MNGM_ERROR_DisconnectWait( &wk->entry_param ) == FALSE ){
				return BB_DIS_ERROR_CLOSEING;
			}

			// 全リソース破棄
			if( wk->p_client != NULL ){
				BB_disp_NameWinDel( wk->p_client );
				BB_disp_InfoWinDel( wk->p_client );
				MainResource_Delete( wk );
			}

			if( wk->result_work != NULL ){
				MNGM_RESULT_Exit( wk->result_work );
				wk->result_work = NULL;
			}

			if( wk->entry_work != NULL ){
				MNGM_ENTRY_Exit( wk->entry_work );
				wk->entry_work = NULL;
			}

			return BB_DIS_ERROR_CLOSED;
		}
	}

	return BB_DIS_ERROR_NONE;
}

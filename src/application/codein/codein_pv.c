//==============================================================================
/**
 * @file	codein_pv.c
 * @brief	文字入力インターフェース
 * @author	goto
 * @date	2007.07.11(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
#include "common.h"
#include "system/procsys.h"
#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "system/pm_str.h"
#include "system/clact_tool.h"
#include "system/msgdata_util.h"
#include "system/wordset.h"
#include "system/window.h"
#include "msgdata/msg.naix"
#include "msgdata/msg_fightmsg_dp.h"
#include "system/wipe.h"
#include "system/brightness.h"
#include "system/fontproc.h"
#include "system/lib_pack.h"
#include "system/fontoam.h"
#include "system/msg_ds_icon.h"
#include "gflib/msg_print.h"
#include "gflib/touchpanel.h"
#include "poketool/poke_tool.h"
#include "poketool/monsno.h"
#include "poketool/pokeicon.h"
#include "poketool/boxdata.h"
#include "system/bmp_menu.h"
#include "system/snd_tool.h"
#include "gflib/strbuf_family.h"
#include "communication/wm_icon.h"

#include "codein_pv.h"
#include "codein_snd.h"



//--------------------------------------------------------------
/**
 * @brief	フォーカスセットして拡大縮小対象を設定する
 *
 * @param	next_focus	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_FocusSet( CODEIN_WORK* wk, int next_focus )
{
	int index;
	wk->focus_old = wk->focus_now;
	wk->focus_now = next_focus;
	
	wk->ls = 0;
	wk->le = 0;
	wk->ss = 0;
	wk->se = 0;
#if 0	
	if ( wk->focus_now != 0 ){		
		wk->ls = ( wk->focus_now - 1 ) * wk->param.block[ wk->focus_now - 0 ];
		wk->le = ( wk->focus_now - 0 ) * wk->param.block[ wk->focus_now - 1 ];
	}

	if ( wk->focus_old != 0 ){		
		wk->ss = ( wk->focus_old - 1 ) * wk->param.block[ wk->focus_old - 0 ];
		wk->se = ( wk->focus_old - 0 ) * wk->param.block[ wk->focus_old - 1 ];
	}
#endif

	if ( wk->focus_now != 0 ){
		wk->ls = wk->b_tbl[ wk->focus_now - 1 ][ 0 ];
		wk->le = wk->b_tbl[ wk->focus_now - 1 ][ 1 ];
	}
	if ( wk->focus_old != 0 ){
		wk->ss = wk->b_tbl[ wk->focus_old - 1 ][ 0 ];
		wk->se = wk->b_tbl[ wk->focus_old - 1 ][ 1 ];
	}

	OS_Printf( "fn = %2d | fo =  %2d\n", wk->focus_now, wk->focus_old );
	OS_Printf( "L = %2d ~ %2d\n", wk->ls, wk->le );
	OS_Printf( "S = %2d ~ %2d\n", wk->ss, wk->se );
}

//--------------------------------------------------------------
/**
 * @brief	各種データを初期化する
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_ParamInit( CODEIN_WORK* wk )
{
	int i;

	///< Touch パネルモード
	wk->sys.touch = TRUE;
    
    {
        u16 r5 = 0;
        for ( i = 0; i < CODE_BLOCK_MAX + 1; i++) {
            wk->b_tbl[ i ][ 0 ] = r5;
            r5 += wk->param.block[ i ];
            wk->b_tbl[ i ][ 1 ] = r5;
        }
    }
    
    //wk->b_tbl[
	/////< block table の作成
	//{
	//	wk->b_tbl[ 0 ][ 0 ] = 0;
	//	wk->b_tbl[ 0 ][ 1 ] = wk->param.block[ 0 ];
	//	wk->b_tbl[ 1 ][ 0 ] = wk->param.block[ 0 ];
	//	wk->b_tbl[ 1 ][ 1 ] = wk->param.block[ 0 ] + wk->param.block[ 1 ];
	//	wk->b_tbl[ 2 ][ 0 ] = wk->param.block[ 0 ] + wk->param.block[ 1 ];
	//	wk->b_tbl[ 2 ][ 1 ] = wk->param.block[ 0 ] + wk->param.block[ 1 ] + wk->param.block[ 2 ];
	//}		
	///< focusの設定
	CI_pv_FocusSet( wk, wk->param.unk24 + 1 );

    ///< block 総数からcodeの最大数を求める
    for ( i = 0; i < CODE_BLOCK_MAX; i++ ){
        if (wk->param.block[i] == 0) {
            break;
        }

        wk->code_max += wk->param.block[ i ];
        wk->unk3ec++;
    }

    {
        int intermediate;

        wk->unk3ec--;
        intermediate = 8 * (wk->code_max + wk->unk3ec);
        wk->x_tbl[ 0 ] = 112 - intermediate / 2;

        for (i = 0; i < CODE_BLOCK_MAX; i++) {
            intermediate = 8 * wk->unk3ec + (8 * (wk->code_max - wk->param.block[i]) + 32 * wk->param.block[i]);
            wk->x_tbl[ i + 1 ] = 112 - intermediate / 2;
        }

    }

    wk->x_tbl[ 1 ] += 12;

	///< table 作成
	//{
	//	int sw = 0;
	//	const x_tbl[][ CODE_BLOCK_MAX ] = {
	//		{ POS_4_4_4_p1, POS_4_4_4_p2, POS_4_4_4_p3 },
	//		{ POS_2_5_5_p1, POS_2_5_5_p2, POS_2_5_5_p3 },
	//	};
	//	if ( wk->param.block[ 0 ] != 4 ){
	//		sw = 1;
	//	}
	//	for ( i = 0; i < CODE_BLOCK_MAX; i++ ){
	//		
	//		wk->x_tbl[ i ] = x_tbl[ sw ][ i ];
	//	}
	//	wk->x_tbl[ i ] = x_tbl[ sw ][ i - 1 ];
	//}
	
	///< code列内でのbarの位置
	{
		int pos = 0;
        //if (wk->unk3ec > 0) {
            for ( i = 0; i < wk->unk3ec; i++ ){
                pos += wk->param.block[ i ];
                wk->bar[ i ].state = pos - 1;
                OS_Printf( "bar pos = %2d\n", wk->bar[ i ].state );
            }
        //}
	}
	
    
    
	///< グループの設定
	{
		int j;
		int bno = 0;
		i = 0;
		do {
			
			for ( j = 0; j < wk->param.block[ bno ]; j++ ){
				
				wk->code[ i ].group = bno + 1;
				OS_Printf( "group = %2d\n", wk->code[ i ].group );
				i++;
			}
			bno++;			
		} while ( i < wk->code_max );
	}

    {
        for (i = 0; i < wk->param.unk24; i++) {
            //r2 = 0x3bc;
            //r3 = 0x3dc;
            //r6 = wk->param.block[ i ]
            //r7 = wk->unk3f0;
            //r6 = r7 + r6;
            wk->unk3f0 += wk->param.block[ i ];
        }
    }
}

//--------------------------------------------------------------
/**
 * @brief	シーケンス切り替え
 *
 * @param	wk	
 * @param	seq	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_SeqChange( CODEIN_WORK* wk, int seq )
{
	wk->seq			= seq;
	wk->wait		= 0;
	wk->gene_seq	= 0;
	wk->gene_cnt	= 0;
}

//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	wk	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL CI_pv_MainInit( CODEIN_WORK* wk )
{
	CI_pv_disp_CodeRes_Load( wk );	
	CI_pv_disp_SizeFlagSet( wk );	

	CI_pv_disp_CodeOAM_Create( wk );
	CI_pv_disp_MovePosSet( wk, eMPS_SET );
	
	CI_pv_disp_CurOAM_Create( wk );
	CI_pv_disp_BtnOAM_Create( wk );
	
	CI_pv_disp_HitTableSet( wk );
	
	
	CI_pv_FontOam_SysInit( wk );
	CI_pv_FontOam_Add( wk );

	CI_pv_disp_BMP_WindowAdd( wk->sys.bgl, &wk->sys.win, GF_BGL_FRAME0_S, 2, 21, 27, 2, 100, wk->param.unk2c );

    if (wk->param.unk30 != 0) {
        NNSG2dPaletteData *palData;
        void* dat = WirelessIconPlttResGet( HEAPID_CODEIN );
        NNS_G2dGetUnpackedPaletteData( dat, &palData );
        PaletteWorkSet( wk->sys.pfd, palData->pRawData, PF_BIT_MAIN_BG | PF_BIT_SUB_BG, 0xe0, 0x20 );
        sys_FreeMemoryEz( dat );
    }

    CI_pv_SeqChange( wk, eSEQ_INPUT );

	WIPE_SYS_Start( WIPE_PATTERN_WMS,
				    WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN,
				    WIPE_FADE_BLACK,  WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_CODEIN );

	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	終了
 *
 * @param	wk	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL CI_pv_MainEnd( CODEIN_WORK* wk )
{
	switch ( wk->gene_seq ){
	case 0:
		WIPE_SYS_Start( WIPE_PATTERN_WMS,
			    WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT,
			    WIPE_FADE_BLACK,  WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_CODEIN );
		wk->gene_seq++;
		break;
		
	case 1:
		if ( WIPE_SYS_EndCheck() == TRUE ){
			
			wk->gene_seq++;
		}
		break;
		
	default:
	
		return TRUE;
	}
	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	入力
 *
 * @param	wk	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL CI_pv_MainInput( CODEIN_WORK* wk )
{
	switch ( wk->gene_seq ){
	case 0:
		if ( WIPE_SYS_EndCheck() == TRUE ){
			
			wk->gene_seq++;
		}
		break;

	default:	
		CI_pv_StateUpdate( wk );	
		
		BMN_Main( wk->sys.btn );
		CI_KEY_Main( wk );
		break;
	}
	
	return FALSE;	
}

//--------------------------------------------------------------
/**
 * @brief	フォーカスの移動
 *
 * @param	wk	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL CI_pv_MainFocusMove( CODEIN_WORK* wk )
{
	int i;
	static f32 scale_l[] = {
		0.5f, 0.2f, 0.5f, 1.0f, 1.2f, 1.0f, 1.0f
	};
	static f32 scale_s[] = {
		0.8f, 0.6f, 0.4f, 0.2f, 0.8f, 1.0f, 1.0f
	};

	switch( wk->gene_seq ){
	///< 拡大縮小前半
	case 0:
		CI_pv_disp_CurOAM_Visible( wk, 0, FALSE );
		{
			///< コード部分の処理
			for ( i = 0; i < wk->code_max; i++ ){
				
				if ( wk->code[ i ].move_wk.wait == 0 ){ continue; }
				
				CATS_ObjectPosMoveCap( wk->code[ i ].cap, wk->code[ i ].move_wk.x, wk->code[ i ].move_wk.y );
				wk->code[ i ].move_wk.wait--;				
				
				
				if ( i >= wk->ls && i < wk->le ){
					CATS_ObjectScaleSetCap( wk->code[ i ].cap, scale_l[ wk->code[ i ].move_wk.scale ], scale_l[ wk->code[ i ].move_wk.scale ] );
					wk->code[ i ].move_wk.scale++;
				}
				
				if ( i >= wk->ss && i < wk->se ){
					CATS_ObjectScaleSetCap( wk->code[ i ].cap, scale_s[ wk->code[ i ].move_wk.scale ], scale_s[ wk->code[ i ].move_wk.scale ] );
					wk->code[ i ].move_wk.scale++;
				}
			}
			
			///< バー部分の処理
			for ( i = 0; i < wk->unk3ec; i++ ){
				if ( wk->bar[ i ].move_wk.wait == 0 ){ continue; }
				CATS_ObjectPosMoveCap( wk->bar[ i ].cap, wk->bar[ i ].move_wk.x, wk->bar[ i ].move_wk.y );
				wk->bar[ i ].move_wk.wait--;
			}
			
			///< 判定
			if ( wk->code[ 0 ].move_wk.wait == 0 ){
				for ( i = wk->ls; i < wk->le; i++ ){
					CATS_ObjectAnimeSeqSetCap( wk->code[ i ].cap, CI_pv_disp_CodeAnimeGet( wk->code[ i ].state, wk->code[ i ].size ) );///< anime pat を 3 に。
					CATS_ObjectUpdateCap( wk->code[ i ].cap );
				}
				for ( i = wk->ss; i < wk->se; i++ ){
					CATS_ObjectAnimeSeqSetCap( wk->code[ i ].cap, CI_pv_disp_CodeAnimeGet( wk->code[ i ].state, wk->code[ i ].size ) );///< anime pat を 3 に。
					CATS_ObjectUpdateCap( wk->code[ i ].cap );
				}
				wk->gene_seq++;
			}
			wk->gene_cnt++;
		}
		break;
		
	///< 拡大縮小後半
	case 1:
		for ( i = wk->ls; i < wk->le; i++ ){
			if ( wk->code[ i ].move_wk.scale == SCALE_CNT_F ){ continue; }
			CATS_ObjectScaleSetCap( wk->code[ i ].cap, scale_l[ wk->code[ i ].move_wk.scale ], scale_l[ wk->code[ i ].move_wk.scale ] );
			wk->code[ i ].move_wk.scale++;
		}
		for ( i = wk->ss; i < wk->se; i++ ){
			if ( wk->code[ i ].move_wk.scale == SCALE_CNT_F ){ continue; }
			CATS_ObjectScaleSetCap( wk->code[ i ].cap, scale_s[ wk->code[ i ].move_wk.scale ], scale_s[ wk->code[ i ].move_wk.scale ] );
			wk->code[ i ].move_wk.scale++;
		}
		wk->gene_cnt++;
		if ( wk->gene_cnt == SCALE_CNT_F ){
			
			wk->gene_seq++;		
		}
		break;
	
	///< 終了
	default:
		CI_pv_disp_HitTableSet( wk );				///< 当たり判定のテーブル更新
		
		if ( wk->state.work == 0 ){
			CI_pv_disp_CurBar_PosSet( wk, CI_pv_FocusTopSerach( wk, wk->state.target ) );
		}
		else {
			CI_pv_disp_CurBar_PosSet( wk, CI_pv_FocusBottomSerach( wk, wk->state.target ) );
		}
		
		if ( wk->focus_now != 0 ){
			CI_pv_disp_CurOAM_Visible( wk, 0, TRUE );
		}
		
		CI_pv_StateInit( wk );		
		CI_pv_SeqChange( wk, eSEQ_INPUT );
		break;
	}
	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	関数テーブル
 *
 * @param	CI_pv_MainTable[]	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL (* const CI_pv_MainTable[])( CODEIN_WORK* wk ) = {
	CI_pv_MainInit,
	CI_pv_MainInput,
	CI_pv_MainFocusMove,
	CI_pv_MainEnd,
};


//--------------------------------------------------------------
/**
 * @brief	メイン
 *
 * @param	wk	
 *
 * @retval	BOOL	TRUE = 終了
 *
 */
//--------------------------------------------------------------
BOOL CI_pv_MainUpdate( CODEIN_WORK* wk )
{
	BOOL bActive;
	
	bActive = CI_pv_MainTable[ wk->seq ]( wk );	
	
//	BMN_Main( wk->sys.btn );
//	CI_KEY_Main( wk );
	
	CI_pv_disp_CurUpdate( wk );
	CI_pv_disp_BtnUpdate( wk );
	CATS_Draw( wk->sys.crp );
	
	return bActive;
}


//--------------------------------------------------------------
/**
 * @brief	キー入力メイン
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
#ifdef NONEQUIVALENT
void CI_KEY_Main( CODEIN_WORK* wk )
{
	const int key_tbl[][ 5 ] = {
		{ 0, 1, 2, 3, 4 },
		{ 5, 6, 7, 8, 9 },
		{ 10,10,10,11,11 },
	};
	BOOL bMove = FALSE;

	int dat = key_tbl[ wk->cur[ 1 ].move_wk.y ][ wk->cur[ 1 ].move_wk.x ];

    if ( wk->seq != eSEQ_INPUT || wk->state.param == eSTATE_FOCUS_MOVE) {
        return;
    }

	if ( wk->sys.touch == TRUE ){
		if ( sys.trg ){			
			wk->sys.touch = FALSE;
			CI_pv_disp_CurSQ_PosSet( wk, dat );
			if ( dat == 10 || dat == 11 ){
				if ( wk->cur[ 1 ].state != eANM_CUR_SQ_L ){
					wk->cur[ 1 ].state = eANM_CUR_SQ_L;
				}
			}
			else {
				if ( wk->cur[ 1 ].state != eANM_CUR_SQ ){
					wk->cur[ 1 ].state = eANM_CUR_SQ;
				}
			}
			return;
		}
	}
	
	if ( sys.repeat & PAD_KEY_UP ){
		if ( wk->cur[ 1 ].move_wk.y > 0 ){
			wk->cur[ 1 ].move_wk.y--;
		}
		else {
			wk->cur[ 1 ].move_wk.y = 2;
		}
		bMove = TRUE;
		
//		Snd_SePlay( CI_SE_MOVE );
	}
	else if ( sys.repeat & PAD_KEY_DOWN ){
		wk->cur[ 1 ].move_wk.y++;
		wk->cur[ 1 ].move_wk.y %= 3;
		bMove = TRUE;

//		Snd_SePlay( CI_SE_MOVE );
	}
	else if ( sys.repeat & PAD_KEY_RIGHT ){
		
		if ( dat == 10 ){
			wk->cur[ 1 ].move_wk.x = 3;
		}
		else if ( dat == 11 ){
			wk->cur[ 1 ].move_wk.x = 0;			
		}
		else {				
			wk->cur[ 1 ].move_wk.x++;
			wk->cur[ 1 ].move_wk.x %= 5;
		}
		bMove = TRUE;

//		Snd_SePlay( CI_SE_MOVE );
	}
	else if ( sys.repeat & PAD_KEY_LEFT ){
		
		if ( dat == 10 ){
			wk->cur[ 1 ].move_wk.x = 3;
		}
		else if ( dat == 11 ){
			wk->cur[ 1 ].move_wk.x = 0;			
		}
		else {				
			if ( wk->cur[ 1 ].move_wk.x > 0 ){
				wk->cur[ 1 ].move_wk.x--;
			}
			else {
				wk->cur[ 1 ].move_wk.x = 4;
			}
		}
		bMove = TRUE;

//		Snd_SePlay( CI_SE_MOVE );
	}
	else if ( sys.trg & PAD_BUTTON_A ){
		///< A ボタン押した
		
		int cur_p;
		int now_g;
		int next_g;
		
		if ( dat == 10 ){
			///< Back 押された場合	
			CI_pv_Input_back( wk );

			Snd_SePlay( CI_SE_BACK );
		}		
		else if ( dat == 11 ){
			///< End 押された場合
			CI_pv_Input_End( wk );	

			Snd_SePlay( CI_SE_END );				
		}
		else {
			
			///< 数字部分で押された場合
			
			if ( wk->focus_now == 0 ){ return; }
						
			cur_p = wk->cur[ 0 ].state;				
			wk->code[ cur_p ].state = dat + 1;
			CI_pv_disp_CurOAM_Visible( wk, 1, FALSE );
			CI_pv_disp_CurOAM_Visible( wk, 2, TRUE );
			CI_pv_disp_CurSQ_PosSetEx( wk, dat, 2 );
			CATS_ObjectAnimeSeqSetCap( wk->code[ cur_p ].cap, CI_pv_disp_CodeAnimeGet( wk->code[ cur_p ].state, wk->code[ cur_p ].size ) );
			CATS_ObjectAnimeSeqSetCap( wk->cur[ 2 ].cap, eANM_CUR_TOUCH );
			
			now_g = wk->code[ cur_p ].group;
			cur_p++;
			if ( cur_p == wk->code_max ){
									
				wk->state.param	 = eSTATE_FOCUS_MOVE;
				wk->state.target = 0;				
				wk->cur[ 1 ].move_wk.x = 3;
				wk->cur[ 1 ].move_wk.y = 2;
				bMove = TRUE;
			}
			else {					
				next_g = wk->code[ cur_p ].group;
				
				if ( now_g != next_g ){
					
					wk->state.param	 = eSTATE_FOCUS_MOVE;
					wk->state.target = next_g;
				}
				else {
					
					wk->state.param	 = eSTATE_CUR_MOVE;
					wk->state.target = cur_p;
				}
				Snd_SePlay( CI_SE_TOUCH );
			}
		}
	}
	else if ( sys.trg & PAD_BUTTON_B ){
		///< 戻ると一緒
		CI_pv_Input_back( wk );

		Snd_SePlay( CI_SE_TOUCH );
	}
	else if ( sys.repeat & PAD_BUTTON_L ){
		
		int button = wk->cur[ 0 ].state;
		
		if ( button == 0 ){
			wk->cur[ 0 ].state = wk->code_max - 1;
		}
		else {
			wk->cur[ 0 ].state--;
		}
		button = wk->cur[ 0 ].state;
		
		if ( wk->code[ button ].size == TRUE ){
			///< 入力部分の移動
			
			wk->state.param		= eSTATE_CUR_MOVE;
			wk->state.target	= button;
		}
		else {
			///< 拡縮カレント移動操作
			
			wk->state.param		= eSTATE_FOCUS_MOVE;
			wk->state.target	= wk->code[ button ].group;
			wk->state.work	 = 1;
		}
		
		Snd_SePlay( CI_SE_MOVE );
	}
	else if ( sys.repeat & PAD_BUTTON_R ){
		
		int button = wk->cur[ 0 ].state;
		
		if ( button == wk->code_max - 1 ){
			wk->cur[ 0 ].state = 0;
		}
		else {
			wk->cur[ 0 ].state++;
		}
		button = wk->cur[ 0 ].state;
		
		if ( wk->code[ button ].size == TRUE ){
			///< 入力部分の移動
			
			wk->state.param		= eSTATE_CUR_MOVE;
			wk->state.target	= button;
		}
		else {
			///< 拡縮カレント移動操作
			
			wk->state.param		= eSTATE_FOCUS_MOVE;
			wk->state.target	= wk->code[ button ].group;
		}
		
		Snd_SePlay( CI_SE_MOVE );
	}
	
	
	if ( bMove == TRUE ){
		Snd_SePlay( CI_SE_MOVE );

		dat = key_tbl[ wk->cur[ 1 ].move_wk.y ][ wk->cur[ 1 ].move_wk.x ];
		CI_pv_disp_CurSQ_PosSet( wk, dat );
		
		if ( dat == 10 || dat == 11 ){
			if ( wk->cur[ 1 ].state != eANM_CUR_SQ_L ){
				wk->cur[ 1 ].state = eANM_CUR_SQ_L;
			}
		}
		else {
			if ( wk->cur[ 1 ].state != eANM_CUR_SQ ){
				wk->cur[ 1 ].state = eANM_CUR_SQ;
			}
		}
	}
}
#else
const int key_tbl[][ 5 ] = {
    { 0, 1, 2, 3, 4 },
    { 5, 6, 7, 8, 9 },
    { 10,10,10,11,11 },
};

extern void _s32_div_f(void);

asm void CI_KEY_Main( CODEIN_WORK* wk )
{
	push {r3, r4, r5, r6, r7, lr}
	sub sp, #0x40
	ldr r5, =key_tbl // _02089F00
	add r4, r0, #0
	add r3, sp, #4
	mov r2, #7
_02089C2C:
	ldmia r5!, {r0, r1}
	stmia r3!, {r0, r1}
	sub r2, r2, #1
	bne _02089C2C
	ldr r0, [r5, #0]
	mov r1, #0x14
	str r0, [r3]
	mov r3, #0x91
	lsl r3, r3, #2
	add r0, r3, #2
	ldrsh r0, [r4, r0]
	ldrsh r7, [r4, r3]
	mov r6, #0
	mov ip, r0
	mul r1, r0
	add r0, sp, #4
	lsl r2, r7, #2
	add r0, r0, r1
	ldr r5, [r2, r0]
	add r0, r3, #0
	add r0, #0x7c
	ldr r0, [r4, r0]
	cmp r0, #1
	bne _02089C80
	mov r2, #0xeb
	lsl r2, r2, #2
	ldr r0, [r4, r2]
	cmp r0, #1
	beq _02089C80
	add r0, r2, #0
	sub r0, #0x38
	ldr r0, [r4, r0]
	cmp r0, #1
	bne _02089CBC
	ldr r0, =sys // _02089F04
	ldr r0, [r0, #0x48]
	cmp r0, #0
	beq _02089C80
	bl GF_TP_GetCont
	cmp r0, #0
	beq _02089C82
_02089C80:
	b _02089F78
_02089C82:
	mov r0, #0xdd
	add r1, r6, #0
	lsl r0, r0, #2
	str r1, [r4, r0]
	add r0, r4, #0
	add r1, r5, #0
	bl CI_pv_disp_CurSQ_PosSet
	sub r5, #0xa
	cmp r5, #1
	bhi _02089CAA
	mov r0, #0x23
	lsl r0, r0, #4
	ldr r1, [r4, r0]
	cmp r1, #2
	beq _02089D8C
	mov r1, #2
	add sp, #0x40
	str r1, [r4, r0]
	pop {r3, r4, r5, r6, r7, pc}
_02089CAA:
	mov r0, #0x23
	lsl r0, r0, #4
	ldr r1, [r4, r0]
	cmp r1, #1
	beq _02089D8C
	mov r1, #1
	add sp, #0x40
	str r1, [r4, r0]
	pop {r3, r4, r5, r6, r7, pc}
_02089CBC:
	ldr r0, =sys // _02089F04
	mov r1, #0x40
	ldr r0, [r0, #0x4c]
	tst r1, r0
	beq _02089CDE
	mov r0, ip
	cmp r0, #0
	ble _02089CD4
	add r0, r3, #2
	ldrsh r0, [r4, r0]
	sub r1, r0, #1
	b _02089CD6
_02089CD4:
	mov r1, #2
_02089CD6:
	add r0, r3, #2
	strh r1, [r4, r0]
	mov r6, #1
	b _02089F2A
_02089CDE:
	mov r1, #0x80
	tst r1, r0
	beq _02089CFE
	add r0, r3, #2
	ldrsh r0, [r4, r0]
	add r1, r0, #1
	add r0, r3, #2
	strh r1, [r4, r0]
	ldrsh r0, [r4, r0]
	mov r1, #3
	bl _s32_div_f
	ldr r0, =0x00000246 // _02089F08
	mov r6, #1
	strh r1, [r4, r0]
	b _02089F2A
_02089CFE:
	mov r1, #0x10
	tst r1, r0
	beq _02089D2C
	cmp r5, #0xa
	bne _02089D0E
	mov r0, #3
	strh r0, [r4, r3]
	b _02089D28
_02089D0E:
	cmp r5, #0xb
	bne _02089D16
	strh r6, [r4, r3]
	b _02089D28
_02089D16:
	add r0, r7, #1
	strh r0, [r4, r3]
	ldrsh r0, [r4, r3]
	mov r1, #5
	bl _s32_div_f
	mov r0, #0x91
	lsl r0, r0, #2
	strh r1, [r4, r0]
_02089D28:
	mov r6, #1
	b _02089F2A
_02089D2C:
	mov r1, #0x20
	tst r1, r0
	beq _02089D56
	cmp r5, #0xa
	bne _02089D3C
	mov r0, #3
	strh r0, [r4, r3]
	b _02089D52
_02089D3C:
	cmp r5, #0xb
	bne _02089D44
	strh r6, [r4, r3]
	b _02089D52
_02089D44:
	cmp r7, #0
	ble _02089D4E
	sub r0, r7, #1
	strh r0, [r4, r3]
	b _02089D52
_02089D4E:
	mov r0, #4
	strh r0, [r4, r3]
_02089D52:
	mov r6, #1
	b _02089F2A
_02089D56:
	ldr r1, =sys // _02089F04
	ldr r7, [r1, #0x48]
	mov r1, #1
	tst r1, r7
	beq _02089E3C
	cmp r5, #0xa
	bne _02089D72
	add r0, r4, #0
	bl CI_pv_Input_back
	ldr r0, =0x000005E5 // _02089F0C
	bl Snd_SePlay
	b _02089F2A
_02089D72:
	cmp r5, #0xb
	bne _02089D84
	add r0, r4, #0
	bl CI_pv_Input_End
	ldr r0, =0x000005E2 // _02089F10
	bl Snd_SePlay
	b _02089F2A
_02089D84:
	sub r2, #0xd8
	ldr r0, [r4, r2]
	cmp r0, #0
	bne _02089D8E
_02089D8C:
	b _02089F78
_02089D8E:
	sub r3, #0x30
	ldr r0, [r4, r3]
	mov r1, #0x1c
	add r7, r0, #0
	str r0, [sp]
	mul r7, r1
	add r0, r5, #1
	str r0, [r4, r7]
	add r0, r4, #0
	mov r1, #1
	add r2, r6, #0
	bl CI_pv_disp_CurOAM_Visible
	add r0, r4, #0
	mov r1, #2
	mov r2, #1
	bl CI_pv_disp_CurOAM_Visible
	add r0, r4, #0
	add r1, r5, #0
	mov r2, #2
	bl CI_pv_disp_CurSQ_PosSetEx
	add r1, r4, r7
	ldr r0, [r4, r7]
	ldr r1, [r1, #8]
	bl CI_pv_disp_CodeAnimeGet
	add r1, r0, #0
	add r0, r4, r7
	ldr r0, [r0, #0xc]
	bl CATS_ObjectAnimeSeqSetCap
	mov r0, #0x96
	lsl r0, r0, #2
	ldr r0, [r4, r0]
	mov r1, #3
	bl CATS_ObjectAnimeSeqSetCap
	add r0, r4, r7
	ldr r3, [r0, #4]
	ldr r0, [sp]
	add r2, r0, #1
	mov r0, #0x2d
	lsl r0, r0, #4
	ldr r1, [r4, r0]
	cmp r2, r1
	bne _02089E0E
	add r1, r0, #0
	mov r6, #1
	add r1, #0xdc
	str r6, [r4, r1]
	add r1, r0, #0
	mov r2, #0
	add r1, #0xe0
	str r2, [r4, r1]
	add r1, r0, #0
	mov r2, #3
	sub r1, #0x8c
	strh r2, [r4, r1]
	mov r1, #2
	sub r0, #0x8a
	strh r1, [r4, r0]
	b _02089F2A
_02089E0E:
	mov r1, #0x1c
	mul r1, r2
	add r1, r4, r1
	ldr r1, [r1, #4]
	cmp r3, r1
	beq _02089E28
	add r2, r0, #0
	mov r3, #1
	add r2, #0xdc
	str r3, [r4, r2]
	add r0, #0xe0
	str r1, [r4, r0]
	b _02089E34
_02089E28:
	add r1, r0, #0
	mov r3, #2
	add r1, #0xdc
	str r3, [r4, r1]
	add r0, #0xe0
	str r2, [r4, r0]
_02089E34:
	ldr r0, =0x000005E5 // _02089F0C
	bl Snd_SePlay
	b _02089F2A
_02089E3C:
	mov r1, #2
	add r5, r7, #0
	tst r5, r1
	beq _02089E52
	add r0, r4, #0
	bl CI_pv_Input_back
	ldr r0, =0x000005E5 // _02089F0C
	bl Snd_SePlay
	b _02089F2A
_02089E52:
	lsl r5, r1, #8
	tst r5, r0
	beq _02089EB6
	add r0, r3, #0
	sub r0, #0x30
	ldr r1, [r4, r0]
	add r0, r2, #0
	add r0, #0x44
	ldr r0, [r4, r0]
	cmp r1, r0
	bne _02089E6E
	sub r2, #0xdc
	ldr r0, [r4, r2]
	b _02089E74
_02089E6E:
	add r0, r3, #0
	sub r0, #0x30
	ldr r0, [r4, r0]
_02089E74:
	sub r0, r0, #1
	sub r3, #0x30
	str r0, [r4, r3]
	mov r0, #0x85
	lsl r0, r0, #2
	ldr r0, [r4, r0]
	mov r1, #0x1c
	mul r1, r0
	add r2, r4, r1
	ldr r1, [r2, #8]
	cmp r1, #1
	bne _02089E9A
	mov r1, #0xeb
	mov r2, #2
	lsl r1, r1, #2
	str r2, [r4, r1]
	add r1, r1, #4
	str r0, [r4, r1]
	b _02089EAC
_02089E9A:
	mov r1, #0xeb
	lsl r1, r1, #2
	mov r3, #1
	str r3, [r4, r1]
	add r0, r1, #4
	ldr r2, [r2, #4]
	add r1, #8
	str r2, [r4, r0]
	str r3, [r4, r1]
_02089EAC:
	mov r0, #0x5e
	lsl r0, r0, #4
	bl Snd_SePlay
	b _02089F2A
_02089EB6:
	add r1, #0xfe
	tst r0, r1
	beq _02089F2A
	add r1, r2, #0
	add r0, r3, #0
	sub r1, #0xdc
	sub r0, #0x30
	ldr r1, [r4, r1]
	ldr r0, [r4, r0]
	sub r1, r1, #1
	cmp r0, r1
	bne _02089ED4
	add r2, #0x44
	ldr r0, [r4, r2]
	b _02089EDC
_02089ED4:
	add r0, r3, #0
	sub r0, #0x30
	ldr r0, [r4, r0]
	add r0, r0, #1
_02089EDC:
	sub r3, #0x30
	str r0, [r4, r3]
	mov r0, #0x85
	lsl r0, r0, #2
	ldr r0, [r4, r0]
	mov r1, #0x1c
	mul r1, r0
	add r2, r4, r1
	ldr r1, [r2, #8]
	cmp r1, #1
	bne _02089F14
	mov r1, #0xeb
	mov r2, #2
	lsl r1, r1, #2
	str r2, [r4, r1]
	add r1, r1, #4
	str r0, [r4, r1]
	b _02089F22
	// .align 2, 0
// _02089F00: .4byte key_tbl
// _02089F04: .4byte sys
// _02089F08: .4byte 0x00000246
// _02089F0C: .4byte 0x000005E5
// _02089F10: .4byte 0x000005E2
_02089F14:
	mov r0, #0xeb
	mov r1, #1
	lsl r0, r0, #2
	str r1, [r4, r0]
	ldr r1, [r2, #4]
	add r0, r0, #4
	str r1, [r4, r0]
_02089F22:
	mov r0, #0x5e
	lsl r0, r0, #4
	bl Snd_SePlay
_02089F2A:
	cmp r6, #1
	bne _02089F78
	mov r0, #0x5e
	lsl r0, r0, #4
	bl Snd_SePlay
	ldr r1, =0x00000246 // _02089F7C
	mov r0, #0x14
	ldrsh r2, [r4, r1]
	sub r1, r1, #2
	ldrsh r1, [r4, r1]
	mul r0, r2
	add r2, sp, #4
	lsl r1, r1, #2
	add r0, r2, r0
	ldr r5, [r1, r0]
	add r0, r4, #0
	add r1, r5, #0
	bl CI_pv_disp_CurSQ_PosSet
	sub r5, #0xa
	cmp r5, #1
	bhi _02089F6A
	mov r0, #0x23
	lsl r0, r0, #4
	ldr r1, [r4, r0]
	cmp r1, #2
	beq _02089F78
	mov r1, #2
	add sp, #0x40
	str r1, [r4, r0]
	pop {r3, r4, r5, r6, r7, pc}
_02089F6A:
	mov r0, #0x23
	lsl r0, r0, #4
	ldr r1, [r4, r0]
	cmp r1, #1
	beq _02089F78
	mov r1, #1
	str r1, [r4, r0]
_02089F78:
	add sp, #0x40
	pop {r3, r4, r5, r6, r7, pc}
}
#endif

//--------------------------------------------------------------
/**
 * @brief	終了
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_Input_End( CODEIN_WORK* wk )
{
	int i;
	u32 num = 0;
	
	STRBUF* str_dest = STRBUF_Create( 100, HEAPID_CODEIN );
	
	wk->btn[ 1 ].state = 1;
	wk->btn[ 1 ].move_wk.wait = 0;
	
	for ( i = 0; i < wk->code_max; i++ ){
		
		if ( wk->code[ i ].state == 0 ){
			 wk->code[ i ].state = 1;
			 CATS_ObjectAnimeSeqSetCap( wk->code[ i ].cap, CI_pv_disp_CodeAnimeGet( wk->code[ i ].state, wk->code[ i ].size ) );
		}
		
		num = wk->code[ i ].state - 1;		
		STRBUF_SetNumber( str_dest, num, 1, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT );
		STRBUF_AddStr( wk->param.strbuf, str_dest );
	}
	
	STRBUF_Delete( str_dest );
	
	CI_pv_SeqChange( wk, eSEQ_END );
}

//--------------------------------------------------------------
/**
 * @brief	戻る
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_Input_back( CODEIN_WORK* wk )
{	///< 戻る
					
	int cur_p;
	int now_g;
	int next_g;
	
	wk->btn[ 0 ].state = 1;
	wk->btn[ 0 ].move_wk.wait = 0;
	
	if ( wk->focus_now == 0 ){
		cur_p = wk->cur[ 0 ].state = wk->code_max - 1;				
		next_g = wk->code[ cur_p ].group;
		wk->state.param	 = eSTATE_FOCUS_MOVE;
		wk->state.target = next_g;
		wk->state.work	 = 1;
		return;
	}
	
	cur_p = wk->cur[ 0 ].state;				
	wk->code[ cur_p ].state = eANM_CODE_LN;
	
	CATS_ObjectAnimeSeqSetCap( wk->code[ cur_p ].cap, CI_pv_disp_CodeAnimeGet( wk->code[ cur_p ].state, wk->code[ cur_p ].size ) );
									
	now_g = wk->code[ cur_p ].group;
    // MatchComment: change from 0 to wk->unk3f0
	if ( cur_p > wk->unk3f0 ){						 
		cur_p--;
		CATS_ObjectAnimeSeqSetCap( wk->code[ cur_p ].cap, CI_pv_disp_CodeAnimeGet( wk->code[ cur_p ].state, wk->code[ cur_p ].size ) );
		
		next_g = wk->code[ cur_p ].group;
		
		if ( now_g != next_g ){
			
			wk->state.param	 = eSTATE_FOCUS_MOVE;
			wk->state.target = next_g;
			wk->state.work	 = 1;
		}
		else {
			
			wk->state.param	 = eSTATE_CUR_MOVE;
			wk->state.target = cur_p;
		}
	}
}



//--------------------------------------------------------------
/**
 * @brief	ボタン用マネージャー設定
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_ButtonManagerInit( CODEIN_WORK* wk )
{
	int i;
	
	///< コードの当たり判定
	for ( i = 0; i < eHRT_CODE_15 + 1; i ++ ){
				
		wk->code[ i ].hit = &wk->sys.rht[ i ];
	}
	
	///< ボタン部分の当たり判定
	{
		///< x, y, sx, sy,
		const s16 tbl[][ 4 ] = {
			// 0 - 4
			{  32,  80, 20, 20 },{  80,  80, 20, 20 },{ 128,  80, 20, 20 },{ 176,  80, 20, 20 },{ 224,  80, 20, 20 },
			
			// 5 - 9
			{  32, 128, 20, 20 },{  80, 128, 20, 20 },{ 128, 128, 20, 20 },{ 176, 128, 20, 20 },{ 224, 128, 20, 20 },
			
			// back - end
			{  64, 176, 60, 12 },{ 192, 176, 60, 12 },
		};
		for ( ; i < eHRT_MAX; i++ ){
			
			wk->sys.rht[ i ].rect.top		= tbl[ i - eHRT_NUM_0 ][ 1 ] - tbl[ i - eHRT_NUM_0 ][ 3 ];
			wk->sys.rht[ i ].rect.left		= tbl[ i - eHRT_NUM_0 ][ 0 ] - tbl[ i - eHRT_NUM_0 ][ 2 ];
			wk->sys.rht[ i ].rect.bottom	= tbl[ i - eHRT_NUM_0 ][ 1 ] + tbl[ i - eHRT_NUM_0 ][ 3 ];
			wk->sys.rht[ i ].rect.right		= tbl[ i - eHRT_NUM_0 ][ 0 ] + tbl[ i - eHRT_NUM_0 ][ 2 ];
		}	
	}

	wk->sys.btn = BMN_Create( wk->sys.rht, eHRT_MAX, 
							  CI_pv_ButtonManagerCallBack, wk, HEAPID_CODEIN );
}


//--------------------------------------------------------------
/**
 * @brief	ボタン用マネージャーコールバック
 *
 * @param	button	
 * @param	event	
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_ButtonManagerCallBack( u32 button, u32 event, void* work )
{
	CODEIN_WORK* wk = work;
	
	///< 入力じゃなければ飛ばす
	if ( wk->seq != eSEQ_INPUT ){ return; }
	
	///< Touch じゃ無ければ Touch にして飛ばす
	if ( wk->sys.touch != TRUE ){
		 wk->sys.touch = TRUE;
	}
	
	if ( event == BMN_EVENT_TOUCH ){
		
		///< 文字入力部分が押された
        // MatchComment: change eHRT_CODE_11 -> eHRT_NUM_0, condition from <= to <
		if ( button >= eHRT_CODE_0 && button < eHRT_NUM_0 ){
            // MatchComment: add this check
			if (button < wk->unk3f0) {
                return;
            }
			OS_Printf( "button = %3d\n", button );
			
			if ( wk->code[ button ].size == TRUE ){
				///< 入力部分の移動
				
				wk->state.param		= eSTATE_CUR_MOVE;
				wk->state.target	= button;
			}
			else {
				///< 拡縮カレント移動操作
				
				wk->state.param		= eSTATE_FOCUS_MOVE;
				wk->state.target	= wk->code[ button ].group;
			}
			Snd_SePlay( CI_SE_BLOCK );
		}
		///< その他のボタン
		else {
			
			///< キー位置にも変換
			if ( button == eHRT_BACK ){
				wk->cur[ 1 ].move_wk.x = 0;
				wk->cur[ 1 ].move_wk.y = 2;
				Snd_SePlay( CI_SE_BACK );
			}
			else if ( button == eHRT_END ){
				wk->cur[ 1 ].move_wk.x = 3;
				wk->cur[ 1 ].move_wk.y = 2;
				Snd_SePlay( CI_SE_END );
			}
			else {
				wk->cur[ 1 ].move_wk.x = ( button - eHRT_NUM_0 ) % 5;
				wk->cur[ 1 ].move_wk.y = ( button - eHRT_NUM_0 ) / 5;
				Snd_SePlay( CI_SE_TOUCH );
			}
			
			///< 数字パネル
			if ( button >= eHRT_NUM_0 && button <= eHRT_NUM_9 ){
				
				int cur_p;
				int now_g;
				int next_g;
				
				if ( wk->focus_now == 0 ){ return; }
				
				cur_p = wk->cur[ 0 ].state;				
				wk->code[ cur_p ].state = button - eHRT_NUM_0 + 1;
				
				CATS_ObjectAnimeSeqSetCap( wk->code[ cur_p ].cap, CI_pv_disp_CodeAnimeGet( wk->code[ cur_p ].state, wk->code[ cur_p ].size ) );
				
				///< カーソル
				CI_pv_disp_CurOAM_Visible( wk, 1, TRUE );
				CI_pv_disp_CurSQ_PosSet( wk, button - eHRT_NUM_0 );
				CI_pv_disp_CurOAM_Visible( wk, 1, FALSE );
				CI_pv_disp_CurOAM_Visible( wk, 2, TRUE );
				CI_pv_disp_CurSQ_PosSetEx( wk, button - eHRT_NUM_0, 2 );
				CATS_ObjectAnimeSeqSetCap( wk->cur[ 2 ].cap, eANM_CUR_TOUCH );
							
				now_g = wk->code[ cur_p ].group;
				cur_p++;
				if ( cur_p == wk->code_max ){
										
					wk->state.param	 = eSTATE_FOCUS_MOVE;
					wk->state.target = 0;
					wk->state.work	 = 0;
				}
				else {					
					next_g = wk->code[ cur_p ].group;
					
					if ( now_g != next_g ){
						
						wk->state.param	 = eSTATE_FOCUS_MOVE;
						wk->state.target = next_g;
						wk->state.work	 = 0;
					}
					else {
						
						wk->state.param	 = eSTATE_CUR_MOVE;
						wk->state.target = cur_p;
					}
				}
			}
			else {
				///< back End
				
				if ( button == eHRT_BACK ){
					///< 戻る
					CI_pv_Input_back( wk );
				}
				else {
					///< 決定
					CI_pv_Input_End( wk );					
				}
			}
		}
	}
}


//--------------------------------------------------------------
/**
 * @brief	状態の更新処理
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_StateUpdate( CODEIN_WORK* wk )
{
	switch ( wk->state.param ){
	
	case eSTATE_INPUT:
		break;
	
	///< フォーカスを移動する
	case eSTATE_FOCUS_MOVE:
		CI_pv_FocusSet( wk, wk->state.target );				///< 次のフォーカス設定
		CI_pv_disp_SizeFlagSet( wk );						///< サイズ設定
		CI_pv_disp_MovePosSet( wk, eMPS_MOVE_SET );			///< 移動地設定
		
		CI_pv_SeqChange( wk, eSEQ_FOCUS_MOVE );		
		wk->state.param	= eSTATE_BLANK;
		break;
	
	///< 入力位置を移動する
	case eSTATE_CUR_MOVE:
		
		CI_pv_disp_CurBar_PosSet( wk, wk->state.target );	///< カーソル位置を変更
		CI_pv_StateInit( wk );
		break;
	
	case eSTATE_BLANK:
		break;
	}
}


//--------------------------------------------------------------
/**
 * @brief	状態の初期化
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_StateInit( CODEIN_WORK* wk )
{	
	wk->state.param		= eSTATE_INPUT;
	wk->state.target	= 0;	
	wk->state.work	 	= 0;
}


//--------------------------------------------------------------
/**
 * @brief	対象フォーカスの先頭ブロックを取得
 *
 * @param	wk	
 * @param	next	
 *
 * @retval	int	
 *
 */
//--------------------------------------------------------------
int CI_pv_FocusTopSerach( CODEIN_WORK* wk, int next )
{
	int i;
	
	for ( i = 0; i < wk->code_max; i++ ){
		
		if ( wk->code[ i ].group == next ){
			
			return i;
		}
	}
	
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	対象フォーカスの最後尾ブロックを取得
 *
 * @param	wk	
 * @param	next	
 *
 * @retval	int	
 *
 */
//--------------------------------------------------------------
int CI_pv_FocusBottomSerach( CODEIN_WORK* wk, int next )
{
	int i;
	int block = 0;
	int search = 0;
	
	for ( i = 0; i < wk->code_max; i++ ){
		
		if ( wk->code[ i ].group == next ){
			
			search = 1;
		}
		else {
			
			if ( search == 1 ){
				
				return i - 1;
			}
		}
	}
	
	return wk->code_max - 1;
}


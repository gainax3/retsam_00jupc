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
#include "msgdata/msg.naix"
#include "system/brightness.h"
#include "system/fontproc.h"
#include "system/lib_pack.h"
#include "gflib/msg_print.h"
#include "gflib/touchpanel.h"
#include "poketool/poke_tool.h"
#include "poketool/pokeicon.h"
#include "poketool/boxdata.h"
#include "system/snd_tool.h"
#include "gflib/strbuf_family.h"
#include "system/pm_overlay.h"
#include "system/wipe.h"

#include "br_private.h"
#include "br_codein.h"

//==============================================================
// Prototype
//==============================================================
static inline void CODE_HitRectSet( BR_CODEIN_WORK* wk, int no, s16 sx, s16 sy );

//--------------------------------------------------------------
/**
 * @brief	CODEIN_PARAM のワークを作成する
 *
 * @param	heap_id	
 * @param	word_len	
 * @param	block	
 *
 * @retval	CODEIN_PARAM*	
 *
 */
//--------------------------------------------------------------
BR_CODEIN_PARAM* BRCodeInput_ParamCreate( int heap_id, int word_len, int block[], CONFIG* cfg )
{
	int i;
	BR_CODEIN_PARAM* wk = NULL;
	
	wk = sys_AllocMemory( heap_id, sizeof( BR_CODEIN_PARAM ) );
	MI_CpuFill8( wk, 0, sizeof( BR_CODEIN_PARAM ) );

	wk->word_len 	= word_len;	
	wk->strbuf		= STRBUF_Create( word_len + 1, heap_id );
	wk->cfg			= cfg;
	
	for ( i = 0; i < CODE_BLOCK_MAX; i++ ){
		wk->block[ i ] = block[ i ];
	//	OS_Printf( "block %d = %d\n", i, wk->block[ i ] );
	}
	wk->block[ i ] = block[ i - 1 ];

	return wk;	
}

//--------------------------------------------------------------
/**
 * @brief	CODEIN_PARAM のワークを解放
 *
 * @param	codein_param	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BRCodeInput_ParamDelete( BR_CODEIN_PARAM* codein_param )
{
	GF_ASSERT( codein_param->strbuf != NULL );
	GF_ASSERT( codein_param != NULL );
	
	STRBUF_Delete( codein_param->strbuf );
	sys_FreeMemoryEz( codein_param );	
}

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
void BRCI_pv_FocusSet( BR_CODEIN_WORK* wk, int next_focus )
{
	int index;
	wk->focus_old = wk->focus_now;
	wk->focus_now = next_focus;
	
	wk->ls = 0;
	wk->le = 0;
	wk->ss = 0;
	wk->se = 0;

	if ( wk->focus_now != 0 ){
		wk->ls = wk->b_tbl[ wk->focus_now - 1 ][ 0 ];
		wk->le = wk->b_tbl[ wk->focus_now - 1 ][ 1 ];
	}
	if ( wk->focus_old != 0 ){
		wk->ss = wk->b_tbl[ wk->focus_old - 1 ][ 0 ];
		wk->se = wk->b_tbl[ wk->focus_old - 1 ][ 1 ];
	}
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
void BRCI_pv_ParamInit( BR_CODEIN_WORK* wk )
{
	int i;
	
	///< Touch パネルモード
	wk->sys.touch = TRUE;
	
	///< block table の作成
	{
		wk->b_tbl[ 0 ][ 0 ] = 0;
		wk->b_tbl[ 0 ][ 1 ] = wk->param.block[ 0 ];
		wk->b_tbl[ 1 ][ 0 ] = wk->param.block[ 0 ];
		wk->b_tbl[ 1 ][ 1 ] = wk->param.block[ 0 ] + wk->param.block[ 1 ];
		wk->b_tbl[ 2 ][ 0 ] = wk->param.block[ 0 ] + wk->param.block[ 1 ];
		wk->b_tbl[ 2 ][ 1 ] = wk->param.block[ 0 ] + wk->param.block[ 1 ] + wk->param.block[ 2 ];
	}		
	///< focusの設定
	BRCI_pv_FocusSet( wk, 1 );

	///< block 総数からcodeの最大数を求める
	for ( i = 0; i < CODE_BLOCK_MAX; i++ ){		
		wk->code_max += wk->param.block[ i ];
	}
	
	///< table 作成
	{
		int sw = 0;
		const x_tbl[][ CODE_BLOCK_MAX ] = {
			{ POS_4_4_4_p1, POS_4_4_4_p2, POS_4_4_4_p3 },
			{ POS_2_5_5_p1, POS_2_5_5_p2, POS_2_5_5_p3 },
		};
		if ( wk->param.block[ 0 ] != 4 ){
			sw = 1;
		}
		for ( i = 0; i < CODE_BLOCK_MAX; i++ ){
			
			wk->x_tbl[ i ] = x_tbl[ sw ][ i ];
		}
		wk->x_tbl[ i ] = x_tbl[ sw ][ i - 1 ];
	}
	
	///< code列内でのbarの位置
	{
		int pos = 0;			
		for ( i = 0; i < BAR_OAM_MAX; i++ ){
			
			pos += wk->param.block[ i ];
			wk->bar[ i ].state = pos - 1;
		//	OS_Printf( "bar pos = %2d\n", wk->bar[ i ].state );
		}
	}
	
	///< グループの設定
	{
		int j;
		int bno = 0;
		i = 0;
		do {
			
			for ( j = 0; j < wk->param.block[ bno ]; j++ ){
				
				wk->code[ i ].group = bno + 1;
			//	OS_Printf( "group = %2d\n", wk->code[ i ].group );
				i++;
			}
			bno++;			
		} while ( i < wk->code_max );
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
void BRCI_pv_SeqChange( BR_CODEIN_WORK* wk, int seq )
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
BOOL BRCI_pv_MainInit( BR_CODEIN_WORK* wk )
{	
	BRCI_pv_ParamInit( wk );
	BRCI_pv_ButtonManagerInit( wk );
	BRCI_pv_disp_CodeRes_Load( wk );
	
	BRCI_pv_disp_SizeFlagSet( wk );
	BRCI_pv_disp_CodeOAM_Create( wk );
	BRCI_pv_disp_MovePosSet( wk, eMPS_SET );
	
	BRCI_pv_disp_CurOAM_Create( wk );
	BRCI_pv_disp_HitTableSet( wk );

	BRCI_pv_SeqChange( wk, eSEQ_INPUT );
				   
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
BOOL BRCI_pv_MainEnd( BR_CODEIN_WORK* wk )
{
//	Cursor_Visible( wk->br_work->cur_wk, FALSE );
	
	BMN_Delete( wk->sys.btn );
	BRCI_pv_disp_CodeRes_Delete( wk );

	return TRUE;
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
BOOL BRCI_pv_MainInput( BR_CODEIN_WORK* wk )
{
	switch ( wk->gene_seq ){
	case 0:
		wk->gene_seq++;
		break;

	default:	
		BRCI_pv_StateUpdate( wk );		
		BMN_Main( wk->sys.btn );
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
BOOL BRCI_pv_MainFocusMove( BR_CODEIN_WORK* wk )
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
		BRCI_pv_disp_CurOAM_Visible( wk, 0, FALSE );
		{
			///< コード部分の処理
			for ( i = 0; i < wk->code_max; i++ ){
				
				if ( wk->code[ i ].move_wk.wait == 0 ){ continue; }
				
				CATS_ObjectPosMoveCap( wk->code[ i ].cap, wk->code[ i ].move_wk.x, wk->code[ i ].move_wk.y );
				wk->code[ i ].move_wk.wait--;				
				
				
				if ( i >= wk->ls && i < wk->le ){
					CATS_ObjectScaleSetCap( wk->code[ i ].cap, scale_l[ wk->code[ i ].move_wk.scale ], scale_l[ wk->code[ i ].move_wk.scale ] );
					wk->code[ i ].move_wk.scale++;
					CATS_ObjectAffineSetCap( wk->code[ i ].cap, CLACT_AFFINE_DOUBLE );
				}
				
				if ( i >= wk->ss && i < wk->se ){
					CATS_ObjectScaleSetCap( wk->code[ i ].cap, scale_s[ wk->code[ i ].move_wk.scale ], scale_s[ wk->code[ i ].move_wk.scale ] );
					wk->code[ i ].move_wk.scale++;
					CATS_ObjectAffineSetCap( wk->code[ i ].cap, CLACT_AFFINE_DOUBLE );
				}
			}
			
			///< バー部分の処理
			for ( i = 0; i < BAR_OAM_MAX; i++ ){
				if ( wk->bar[ i ].move_wk.wait == 0 ){ continue; }
				CATS_ObjectPosMoveCap( wk->bar[ i ].cap, wk->bar[ i ].move_wk.x, wk->bar[ i ].move_wk.y );
				wk->bar[ i ].move_wk.wait--;
			}
			
			///< 判定
			if ( wk->code[ 0 ].move_wk.wait == 0 ){
				for ( i = wk->ls; i < wk->le; i++ ){
					CATS_ObjectAnimeSeqSetCap( wk->code[ i ].cap, BRCI_pv_disp_CodeAnimeGet( wk->code[ i ].state, wk->code[ i ].size ) );///< anime pat を 3 に。
					CATS_ObjectUpdateCap( wk->code[ i ].cap );
				}
				for ( i = wk->ss; i < wk->se; i++ ){
					CATS_ObjectAnimeSeqSetCap( wk->code[ i ].cap, BRCI_pv_disp_CodeAnimeGet( wk->code[ i ].state, wk->code[ i ].size ) );///< anime pat を 3 に。
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
			if ( wk->code[ i ].move_wk.scale == SCALE_CNT_F ){
				CATS_ObjectAffineSetCap( wk->code[ i ].cap, CLACT_AFFINE_NORMAL );
				continue;
			}
			CATS_ObjectScaleSetCap( wk->code[ i ].cap, scale_l[ wk->code[ i ].move_wk.scale ], scale_l[ wk->code[ i ].move_wk.scale ] );
			wk->code[ i ].move_wk.scale++;
		}
		for ( i = wk->ss; i < wk->se; i++ ){
			if ( wk->code[ i ].move_wk.scale == SCALE_CNT_F ){
				CATS_ObjectAffineSetCap( wk->code[ i ].cap, CLACT_AFFINE_NORMAL );
				continue;
			}
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
		BRCI_pv_disp_HitTableSet( wk );				///< 当たり判定のテーブル更新
		
		if ( wk->state.work == 0 ){
			BRCI_pv_disp_CurBar_PosSet( wk, BRCI_pv_FocusTopSerach( wk, wk->state.target ) );
		}
		else {
			BRCI_pv_disp_CurBar_PosSet( wk, BRCI_pv_FocusBottomSerach( wk, wk->state.target ) );
		}
		
		if ( wk->focus_now != 0 ){
			BRCI_pv_disp_CurOAM_Visible( wk, 0, TRUE );
		}
		
		BRCI_pv_StateInit( wk );		
		BRCI_pv_SeqChange( wk, eSEQ_INPUT );
		break;
	}
	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	関数テーブル
 *
 * @param	BRCI_pv_MainTable[]	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL (* const BRCI_pv_MainTable[])( BR_CODEIN_WORK* wk ) = {
	BRCI_pv_MainInit,
	BRCI_pv_MainInput,
	BRCI_pv_MainFocusMove,
	BRCI_pv_MainEnd,
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
BOOL BRCI_pv_MainUpdate( BR_CODEIN_WORK* wk )
{
	BOOL bActive;
	
	bActive = BRCI_pv_MainTable[ wk->seq ]( wk );
	
	if ( bActive == FALSE ){
		BRCI_pv_disp_CurUpdate( wk );
	}
	
	return bActive;
}

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
void BRCI_pv_Input_End( BR_CODEIN_WORK* wk )
{
	int i;
	u32 num = 0;
	
	STRBUF* str_dest = STRBUF_Create( 100, HEAPID_BR );
	
//	wk->btn[ 1 ].state = 1;
//	wk->btn[ 1 ].move_wk.wait = 0;
	
	wk->push_ok = TRUE;
	
	wk->data_number = 0;
	for ( i = 0; i < wk->code_max; i++ ){
		
		if ( wk->code[ i ].state == 0 ){
			 wk->code[ i ].state = 1;
			 CATS_ObjectAnimeSeqSetCap( wk->code[ i ].cap, BRCI_pv_disp_CodeAnimeGet( wk->code[ i ].state, wk->code[ i ].size ) );
		}
		
		num = wk->code[ i ].state - 1;
		if ( i != 0 ){
			wk->data_number *= 10;
		}
		wk->data_number += num;
		STRBUF_SetNumber( str_dest, num, 1, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT );
		STRBUF_AddStr( wk->param.strbuf, str_dest );
	//	OS_Printf( "%d\n", num );
	//	OS_Printf( "%d\n", wk->data_number );
	}
//	OS_Printf( "%d\n", wk->data_number );
	
	STRBUF_Delete( str_dest );
	
	BRCI_pv_SeqChange( wk, eSEQ_END );
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
void BRCI_pv_Input_back( BR_CODEIN_WORK* wk )
{	///< 戻る
					
	int cur_p;
	int now_g;
	int next_g;
	
//	wk->btn[ 0 ].state = 1;
//	wk->btn[ 0 ].move_wk.wait = 0;
	
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
	
	CATS_ObjectAnimeSeqSetCap( wk->code[ cur_p ].cap, BRCI_pv_disp_CodeAnimeGet( wk->code[ cur_p ].state, wk->code[ cur_p ].size ) );
									
	now_g = wk->code[ cur_p ].group;
	if ( cur_p > 0 ){						 
		cur_p--;
		CATS_ObjectAnimeSeqSetCap( wk->code[ cur_p ].cap, BRCI_pv_disp_CodeAnimeGet( wk->code[ cur_p ].state, wk->code[ cur_p ].size ) );
		
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
	else {
		wk->push_ok = FALSE;
		BRCI_pv_SeqChange( wk, eSEQ_END );
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
void BRCI_pv_ButtonManagerInit( BR_CODEIN_WORK* wk )
{
	int i;
	
	///< コードの当たり判定
	for ( i = 0; i < eHRT_CODE_11 + 1; i ++ ){
				
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
			{  64+12, 176, 60, 12 },{ 192, 176, 60, 12 },
		};
		for ( ; i < eBR_CODEIN_HIT_MAX; i++ ){
			
			wk->sys.rht[ i ].rect.top		= tbl[ i - eHRT_NUM_0 ][ 1 ] - tbl[ i - eHRT_NUM_0 ][ 3 ];
			wk->sys.rht[ i ].rect.left		= tbl[ i - eHRT_NUM_0 ][ 0 ] - tbl[ i - eHRT_NUM_0 ][ 2 ];
			wk->sys.rht[ i ].rect.bottom	= tbl[ i - eHRT_NUM_0 ][ 1 ] + tbl[ i - eHRT_NUM_0 ][ 3 ];
			wk->sys.rht[ i ].rect.right		= tbl[ i - eHRT_NUM_0 ][ 0 ] + tbl[ i - eHRT_NUM_0 ][ 2 ];
		}
		wk->sys.rht[ eHRT_BACK ].rect.top		= 152;
		wk->sys.rht[ eHRT_BACK ].rect.left		= 32;
		wk->sys.rht[ eHRT_BACK ].rect.bottom	= 152+32;
		wk->sys.rht[ eHRT_BACK ].rect.right		= 128;
		
		wk->sys.rht[ eHRT_END  ].rect.top		= 152;
		wk->sys.rht[ eHRT_END  ].rect.left		= 128;
		wk->sys.rht[ eHRT_END  ].rect.bottom	= 152+32;
		wk->sys.rht[ eHRT_END  ].rect.right		= 224;
			
	}
	wk->sys.btn = BMN_Create( wk->sys.rht, eBR_CODEIN_HIT_MAX, BRCI_pv_ButtonManagerCallBack, wk, HEAPID_BR );
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
void BRCI_pv_ButtonManagerCallBack( u32 button, u32 event, void* work )
{
	BR_CODEIN_WORK* wk = work;
	
	///< 入力じゃなければ飛ばす
	if ( wk->seq != eSEQ_INPUT ){ return; }
	
	///< Touch じゃ無ければ Touch にして飛ばす
	if ( wk->sys.touch != TRUE ){
		 wk->sys.touch = TRUE;
	}
	
	if ( event == BMN_EVENT_TOUCH ){
		
		///< 文字入力部分が押された
		if ( button >= eHRT_CODE_0 && button <= eHRT_CODE_11 ){
			
		//	OS_Printf( "button = %3d\n", button );
			
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
			Snd_SePlay( eBR_SND_TOUCH );
		}
		///< その他のボタン
		else {
			
			///< キー位置にも変換
			if ( button == eHRT_BACK ){
				wk->cur[ 1 ].move_wk.x = 0;
				wk->cur[ 1 ].move_wk.y = 2;
			}
			else if ( button == eHRT_END ){
				wk->cur[ 1 ].move_wk.x = 3;
				wk->cur[ 1 ].move_wk.y = 2;
			}
			else {
				wk->cur[ 1 ].move_wk.x = ( button - eHRT_NUM_0 ) % 5;
				wk->cur[ 1 ].move_wk.y = ( button - eHRT_NUM_0 ) / 5;
			}
			
			///< 数字パネル
			if ( button >= eHRT_NUM_0 && button <= eHRT_NUM_9 ){
				
				int cur_p;
				int now_g;
				int next_g;
				
				if ( wk->focus_now == 0 ){ return; }
				Snd_SePlay( eBR_SND_TOUCH );
				
				cur_p = wk->cur[ 0 ].state;				
				wk->code[ cur_p ].state = button - eHRT_NUM_0 + 1;
				
				CATS_ObjectAnimeSeqSetCap( wk->code[ cur_p ].cap, BRCI_pv_disp_CodeAnimeGet( wk->code[ cur_p ].state, wk->code[ cur_p ].size ) );
				
				///< カーソル
				BRCI_pv_disp_CurOAM_Visible( wk, 1, TRUE );
				BRCI_pv_disp_CurSQ_PosSet( wk, button - eHRT_NUM_0 );
				BRCI_pv_disp_CurOAM_Visible( wk, 1, FALSE );
				BRCI_pv_disp_CurOAM_Visible( wk, 2, TRUE );
				BRCI_pv_disp_CurSQ_PosSetEx( wk, button - eHRT_NUM_0, 2 );
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
					Snd_SePlay( eBR_SND_TOUCH );
				//	TouchAction( wk->br_work );
					BRCI_pv_Input_back( wk );
				}
				else {
					///< 決定
					Snd_SePlay( eBR_SND_TOUCH );
				//	TouchAction( wk->br_work );
					BRCI_pv_Input_End( wk );					
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
void BRCI_pv_StateUpdate( BR_CODEIN_WORK* wk )
{
	switch ( wk->state.param ){
	
	case eSTATE_INPUT:
		break;
	
	///< フォーカスを移動する
	case eSTATE_FOCUS_MOVE:
		BRCI_pv_FocusSet( wk, wk->state.target );				///< 次のフォーカス設定
		BRCI_pv_disp_SizeFlagSet( wk );						///< サイズ設定
		BRCI_pv_disp_MovePosSet( wk, eMPS_MOVE_SET );			///< 移動地設定
		
		BRCI_pv_SeqChange( wk, eSEQ_FOCUS_MOVE );		
		wk->state.param	= eSTATE_BLANK;
		break;
	
	///< 入力位置を移動する
	case eSTATE_CUR_MOVE:
		
		BRCI_pv_disp_CurBar_PosSet( wk, wk->state.target );	///< カーソル位置を変更
		BRCI_pv_StateInit( wk );
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
void BRCI_pv_StateInit( BR_CODEIN_WORK* wk )
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
int BRCI_pv_FocusTopSerach( BR_CODEIN_WORK* wk, int next )
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
int BRCI_pv_FocusBottomSerach( BR_CODEIN_WORK* wk, int next )
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

//--------------------------------------------------------------
/**
 * @brief	コード用データ読み込み
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BRCI_pv_disp_CodeRes_Load( BR_CODEIN_WORK* wk )
{
	ARCHANDLE*		 hdl;
	GF_BGL_INI*		 bgl;
	CATS_SYS_PTR	 csp;
	CATS_RES_PTR	 crp;
	PALETTE_FADE_PTR pfd;

	csp = wk->sys.csp;
	crp = wk->sys.crp;
	pfd = wk->sys.pfd;	
	bgl = wk->sys.bgl;
	hdl = wk->sys.p_handle;
	
	///< bg
	ArcUtil_HDL_BgCharSet( hdl, NARC_batt_rec_gra_batt_rec_gds_bg_NCGR, wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
	ArcUtil_HDL_ScrnSet( hdl, 	NARC_batt_rec_gra_batt_rec_nam_NSCR, bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
	
	///< code
	CATS_LoadResourcePlttWorkArcH( pfd, FADE_MAIN_OBJ, csp, crp, hdl, NARC_batt_rec_gra_batt_rec_nam_NCLR, FALSE, 1, NNS_G2D_VRAM_TYPE_2DMAIN, eID_CODE_OAM );
	CATS_LoadResourceCharArcH( csp, crp, hdl, 	 NARC_batt_rec_gra_batt_rec_nam_obj_NCGR, FALSE, NNS_G2D_VRAM_TYPE_2DMAIN, eID_CODE_OAM );
	CATS_LoadResourceCellArcH( csp, crp, hdl, 	 NARC_batt_rec_gra_batt_rec_nam_obj_NCER, FALSE, eID_CODE_OAM );
	CATS_LoadResourceCellAnmArcH( csp, crp, hdl, NARC_batt_rec_gra_batt_rec_nam_obj_NANR, FALSE, eID_CODE_OAM );	
	
	///< cur
	CATS_LoadResourcePlttWorkArcH( pfd, FADE_MAIN_OBJ, csp, crp, hdl, NARC_batt_rec_gra_battle_rec_code_cur_NCLR, FALSE, 1, NNS_G2D_VRAM_TYPE_2DMAIN, eID_CUR_OAM );
	CATS_LoadResourceCharArcH( csp, crp, hdl, NARC_batt_rec_gra_battle_rec_code_cur_NCGR, FALSE, NNS_G2D_VRAM_TYPE_2DMAIN, eID_CUR_OAM );
	CATS_LoadResourceCellArcH( csp, crp, hdl, NARC_batt_rec_gra_battle_rec_code_cur_NCER, FALSE, eID_CUR_OAM );
	CATS_LoadResourceCellAnmArcH( csp, crp, hdl, NARC_batt_rec_gra_battle_rec_code_cur_NANR, FALSE, eID_CUR_OAM );	
	
	///< button
//	CATS_LoadResourcePlttWorkArcH( pfd, FADE_MAIN_OBJ, csp, crp, hdl, NARC_codein_gra_code_button_NCLR, FALSE, 2, NNS_G2D_VRAM_TYPE_2DMAIN, eID_BTN_OAM );
//	CATS_LoadResourceCharArcH( csp, crp, hdl, NARC_codein_gra_code_button_NCGR, FALSE, NNS_G2D_VRAM_TYPE_2DMAIN, eID_BTN_OAM );
//	CATS_LoadResourceCellArcH( csp, crp, hdl, NARC_codein_gra_code_button_NCER, FALSE, eID_BTN_OAM );
//	CATS_LoadResourceCellAnmArcH( csp, crp, hdl, NARC_codein_gra_code_button_NANR, FALSE, eID_BTN_OAM );
}


//--------------------------------------------------------------
/**
 * @brief	もろもろ解放
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BRCI_pv_disp_CodeRes_Delete( BR_CODEIN_WORK* wk )
{
	int i;
	
	for ( i = 0; i < wk->code_max; i++ ){
		CATS_ActorPointerDelete_S( wk->code[ i ].cap );
	}
	
	for ( i = 0; i < 2; i++ ){
		CATS_ActorPointerDelete_S( wk->bar[ i ].cap );
	}
	
	for ( i = 0; i < 3; i++ ){
		CATS_ActorPointerDelete_S( wk->cur[ i ].cap );
	}
	
	CATS_FreeResourceChar( wk->sys.crp, eID_CODE_OAM );
	CATS_FreeResourcePltt( wk->sys.crp, eID_CODE_OAM );
	CATS_FreeResourceCell( wk->sys.crp, eID_CODE_OAM );
	CATS_FreeResourceCellAnm( wk->sys.crp, eID_CODE_OAM );
	
	CATS_FreeResourceChar( wk->sys.crp, eID_CUR_OAM );
	CATS_FreeResourcePltt( wk->sys.crp, eID_CUR_OAM );
	CATS_FreeResourceCell( wk->sys.crp, eID_CUR_OAM );
	CATS_FreeResourceCellAnm( wk->sys.crp, eID_CUR_OAM );
	
//	BRCI_pv_FontOam_SysDelete( wk );
	
//	GF_BGL_BmpWinDel( &wk->sys.win );
	
//	CATS_ResourceDestructor_S( wk->sys.csp, wk->sys.crp );
}

//--------------------------------------------------------------
/**
 * @brief	コードOAM作成
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BRCI_pv_disp_CodeOAM_Create( BR_CODEIN_WORK* wk )
{
	int i;
	int i_c = 0;
	int i_b = 0;
	
	TCATS_OBJECT_ADD_PARAM_S coap;
	CATS_SYS_PTR csp;
	CATS_RES_PTR crp;
	PALETTE_FADE_PTR pfd;

	csp = wk->sys.csp;
	crp = wk->sys.crp;
	pfd = wk->sys.pfd;
	
	coap.x			= 0;
	coap.y			= 0;
	coap.z			= 0;		
	coap.anm		= 0;
	coap.pri		= 10;
	coap.d_area		= CATS_D_AREA_MAIN;
	coap.bg_pri		= 0;
	coap.vram_trans = 0;
	coap.pal		= 0;
	coap.id[0]		= eID_CODE_OAM;
	coap.id[1]		= eID_CODE_OAM;
	coap.id[2]		= eID_CODE_OAM;
	coap.id[3]		= eID_CODE_OAM;
	coap.id[4]		= CLACT_U_HEADER_DATA_NONE;
	coap.id[5]		= CLACT_U_HEADER_DATA_NONE;	
	
	for ( i = 0; i < wk->code_max + BAR_OAM_MAX; i++ ){
		
		if ( i == ( wk->bar[ i_b ].state + i_b + 1 ) ){
			
			wk->bar[ i_b ].cap = CATS_ObjectAdd_S( csp, crp, &coap );

			CATS_ObjectPosSetCap( wk->bar[ i_b ].cap, 72 + ( i * 8 ) + 4, POS_CODE_Y );
			CATS_ObjectAnimeSeqSetCap( wk->bar[ i_b ].cap, eANM_CODE_BAR );
			CATS_ObjectUpdateCap( wk->bar[ i_b ].cap );
			i_b++;
		}
		else {			
			wk->code[ i_c ].cap = CATS_ObjectAdd_S( csp, crp, &coap );

			CATS_ObjectPosSetCap( wk->code[ i_c ].cap, 72 + ( i * 8 ) + 4, POS_CODE_Y );
			CATS_ObjectAnimeSeqSetCap( wk->code[ i_c ].cap, BRCI_pv_disp_CodeAnimeGet( wk->code[ i_c ].state, wk->code[ i_c ].size ) );
			CATS_ObjectAffineSetCap( wk->code[ i_c ].cap, CLACT_AFFINE_NONE );//CLACT_AFFINE_DOUBLE );
			CATS_ObjectUpdateCap( wk->code[ i_c ].cap );
			i_c++;
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief	カーソルOAMの作成
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BRCI_pv_disp_CurOAM_Create( BR_CODEIN_WORK* wk )
{	
	TCATS_OBJECT_ADD_PARAM_S coap;
	CATS_SYS_PTR csp;
	CATS_RES_PTR crp;
	PALETTE_FADE_PTR pfd;

	csp = wk->sys.csp;
	crp = wk->sys.crp;
	pfd = wk->sys.pfd;
	
	coap.x			= 0;
	coap.y			= 0;
	coap.z			= 0;		
	coap.anm		= 0;
	coap.pri		= 0;
	coap.d_area		= CATS_D_AREA_MAIN;
	coap.bg_pri		= 0;
	coap.vram_trans = 0;
	coap.pal		= 0;
	coap.id[0]		= eID_CUR_OAM;
	coap.id[1]		= eID_CUR_OAM;
	coap.id[2]		= eID_CUR_OAM;
	coap.id[3]		= eID_CUR_OAM;
	coap.id[4]		= CLACT_U_HEADER_DATA_NONE;
	coap.id[5]		= CLACT_U_HEADER_DATA_NONE;	
	
	wk->cur[ 0 ].cap = CATS_ObjectAdd_S( csp, crp, &coap );
	wk->cur[ 1 ].cap = CATS_ObjectAdd_S( csp, crp, &coap );
	wk->cur[ 2 ].cap = CATS_ObjectAdd_S( csp, crp, &coap );
	
	BRCI_pv_disp_CurBar_PosSet( wk, 0 );
	CATS_ObjectAnimeSeqSetCap( wk->cur[ 0 ].cap, 0 );
	CATS_ObjectUpdateCap( wk->cur[ 0 ].cap );
	
	wk->cur[ 1 ].move_wk.x = 0;
	wk->cur[ 1 ].move_wk.y = 0;
	wk->cur[ 1 ].state = eANM_CUR_SQ;
	BRCI_pv_disp_CurSQ_PosSet( wk, 0 );
	CATS_ObjectAnimeSeqSetCap( wk->cur[ 1 ].cap, wk->cur[ 1 ].state );
	CATS_ObjectUpdateCap( wk->cur[ 1 ].cap );
	CATS_ObjectObjModeSetCap( wk->cur[ 1 ].cap, GX_OAM_MODE_XLU );
	
	wk->cur[ 2 ].move_wk.x = 0;
	wk->cur[ 2 ].move_wk.y = 0;
	wk->cur[ 2 ].state = eANM_CUR_SQ;
	BRCI_pv_disp_CurSQ_PosSet( wk, 0 );
	CATS_ObjectAnimeSeqSetCap( wk->cur[ 2 ].cap, wk->cur[ 2 ].state );
	CATS_ObjectUpdateCap( wk->cur[ 2 ].cap );
	CATS_ObjectObjModeSetCap( wk->cur[ 2 ].cap, GX_OAM_MODE_XLU );
	
	BRCI_pv_disp_CurOAM_Visible( wk, 1, FALSE );
	BRCI_pv_disp_CurOAM_Visible( wk, 2, FALSE );
}


//--------------------------------------------------------------
/**
 * @brief	Button OAM 作成
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------


//--------------------------------------------------------------
/**
 * @brief	カーソルOAMのONOFF
 *
 * @param	wk	
 * @param	no	
 * @param	flag	TRUE = ON
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BRCI_pv_disp_CurOAM_Visible( BR_CODEIN_WORK* wk, int no, BOOL flag )
{
	if ( flag == TRUE ){
		
		CATS_ObjectEnableCap( wk->cur[ no ].cap, CATS_ENABLE_TRUE );
	}
	else {
		
		CATS_ObjectEnableCap( wk->cur[ no ].cap, CATS_ENABLE_FALSE );		
	}
}


//--------------------------------------------------------------
/**
 * @brief	カーソルのONOFF取得
 *
 * @param	wk	
 * @param	no	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL BRCI_pv_disp_CurOAM_VisibleGet( BR_CODEIN_WORK* wk, int no )
{
	int enable;
	
	enable = CATS_ObjectEnableGetCap( wk->cur[ no ].cap );
	
	return enable == CATS_ENABLE_TRUE ? TRUE : FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	アニメ切り替え
 *
 * @param	wk	
 * @param	no	
 * @param	anime	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BRCI_pv_disp_CurOAM_AnimeChange( BR_CODEIN_WORK* wk, int no, int anime )
{
	CATS_ObjectAnimeSeqSetCap( wk->cur[ no ].cap, anime );
}


//--------------------------------------------------------------
/**
 * @brief	カーソルの位置を入力位置に
 *
 * @param	wk	
 * @param	id	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BRCI_pv_disp_CurBar_PosSet( BR_CODEIN_WORK* wk, int id )
{
	s16 x, y;
	CATS_ACT_PTR cap;
	
	cap = wk->code[ id ].cap;
	
	wk->cur[ 0 ].state = id;
	
	CATS_ObjectPosGetCap( cap, &x, &y );
	CATS_ObjectPosSetCap( wk->cur[ 0 ].cap, x, y + CUR_BAR_OY );
}

//--------------------------------------------------------------
/**
 * @brief	カーソル位置をパネルに
 *
 * @param	wk	
 * @param	id	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BRCI_pv_disp_CurSQ_PosSet( BR_CODEIN_WORK* wk, int id )
{
	s16 x, y;
	
	x = ( wk->sys.rht[ id + eHRT_NUM_0 ].rect.left + wk->sys.rht[ id + eHRT_NUM_0 ].rect.right )  / 2;
	y = ( wk->sys.rht[ id + eHRT_NUM_0 ].rect.top  + wk->sys.rht[ id + eHRT_NUM_0 ].rect.bottom ) / 2;
	
	CATS_ObjectPosSetCap( wk->cur[ 1 ].cap, x - 0, y - 8 );
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 * @param	id	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BRCI_pv_disp_CurSQ_PosSetEx( BR_CODEIN_WORK* wk, int id, int cur_id )
{
	s16 x, y;

	x = ( wk->sys.rht[ id + eHRT_NUM_0 ].rect.left + wk->sys.rht[ id + eHRT_NUM_0 ].rect.right )  / 2;
	y = ( wk->sys.rht[ id + eHRT_NUM_0 ].rect.top  + wk->sys.rht[ id + eHRT_NUM_0 ].rect.bottom ) / 2;

	CATS_ObjectPosSetCap( wk->cur[ cur_id ].cap, x - 0, y - 8 );
}


//--------------------------------------------------------------
/**
 * @brief	カーソル更新
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BRCI_pv_disp_CurUpdate( BR_CODEIN_WORK* wk )
{
	CATS_ObjectUpdateCap( wk->cur[ 0 ].cap );
	CATS_ObjectUpdateCap( wk->cur[ 1 ].cap );
	CATS_ObjectUpdateCap( wk->cur[ 2 ].cap );
	
	{
		
		int id = 2;
		int anime;
		BOOL bAnime;
		
		for ( id = 1; id < 3; id++ ){
			
			anime = CATS_ObjectAnimeSeqGetCap( wk->cur[ id ].cap );
			
			if ( anime == eANM_CUR_TOUCH ){
				
				bAnime = CATS_ObjectAnimeActiveCheckCap( wk->cur[ id ].cap );
				
				if ( bAnime == FALSE ){
					
					CATS_ObjectAnimeSeqSetCap( wk->cur[ id ].cap, wk->cur[ id ].state );
			
					if ( wk->sys.touch == TRUE ){
							BRCI_pv_disp_CurOAM_Visible( wk, 1, FALSE );
					}
					else {
							BRCI_pv_disp_CurOAM_Visible( wk, 1, TRUE );
					}
					BRCI_pv_disp_CurOAM_Visible( wk, 2, FALSE );
				}
			}
			else {
				
				if ( anime != wk->cur[ id ].state ){
					CATS_ObjectAnimeSeqSetCap( wk->cur[ id ].cap, wk->cur[ id ].state );
				}
				{
					anime = CATS_ObjectAnimeSeqGetCap( wk->cur[ 2 ].cap );
					if ( anime != eANM_CUR_TOUCH ){
						if ( wk->sys.touch == TRUE ){
							BRCI_pv_disp_CurOAM_Visible( wk, 1, FALSE );
						}
						else {
							BRCI_pv_disp_CurOAM_Visible( wk, 1, TRUE );
						}	
					}
				}
			}
		}
	}
}




//--------------------------------------------------------------
/**
 * @brief	アニメコードを取得
 *
 * @param	state	
 * @param	size	TRUE = Large
 *
 * @retval	int	
 *
 */
//--------------------------------------------------------------
int BRCI_pv_disp_CodeAnimeGet( int state, BOOL size )
{
	int anime = eANM_CODE_LN;
	
	if ( size == FALSE ){
		
		 anime = eANM_CODE_SN;
	}
	
	anime += state;
	
	return anime;
}

//--------------------------------------------------------------
/**
 * @brief	サイズフラグの設定
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BRCI_pv_disp_SizeFlagSet( BR_CODEIN_WORK* wk )
{
	int i;
	
	for ( i = 0; i < wk->code_max; i++ ){
		
		if ( i >= wk->ls && i < wk->le ){
			
			wk->code[ i ].size = TRUE;
		}
		else {
			
			wk->code[ i ].size = FALSE;
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief	移動先の位置を設定する
 *
 * @param	wk	
 * @param	mode	0 = set 1 = move_wk set
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BRCI_pv_disp_MovePosSet( BR_CODEIN_WORK* wk, int mode )
{
	int i;
	int bp;
	s16 base;
	s16 x, y;
	
//	OS_Printf( "focus %d\n", wk->focus_now );
	base = wk->x_tbl[ wk->focus_now ];
	bp	 = 0;
	
	for ( i = 0; i < wk->code_max; i++ ){
				
		if ( i >= wk->ls && i < wk->le ){
			
			if ( i == wk->ls ){
				base += M_SIZE;
			}
			else {
				base += L_SIZE;
			}
		}
		else {
			if ( i == 0 ){
				base += M_SIZE;
			}
			else {
				base += S_SIZE;
			}			
		}
		
		CATS_ObjectPosGetCap( wk->code[ i ].cap, &x, &y );
		if ( mode == eMPS_SET ){
			
			CATS_ObjectPosSetCap( wk->code[ i ].cap, base, y );										
		}
		else {
			wk->code[ i ].move_wk.x		= ( base - x ) / MOVE_WAIT;
			wk->code[ i ].move_wk.y		= 0;
			wk->code[ i ].move_wk.wait	= MOVE_WAIT;
			wk->code[ i ].move_wk.scale	= 0;
		}
		
		if ( i == wk->bar[ bp ].state && bp != BAR_OAM_MAX ){
			
			CATS_ObjectPosGetCap( wk->bar[ bp ].cap, &x, &y );
			
			if ( wk->ls == wk->le ){
				base += S_SIZE;
			}
			else {
				if ( i > wk->ls && i < wk->le ){
					
					base += M_SIZE;
				}
				else {
					
					base += S_SIZE;
				}
			}
			
			if ( mode == eMPS_SET ){
				
				CATS_ObjectPosSetCap( wk->bar[ bp ].cap, base, y );
			}
			else {
				
				wk->bar[ bp ].move_wk.x		= ( base - x ) / MOVE_WAIT;
				wk->bar[ bp ].move_wk.y		= 0;
				wk->bar[ bp ].move_wk.wait	= MOVE_WAIT;
			}
			bp++;				
		}		
	}
}


//--------------------------------------------------------------
/**
 * @brief	当たり判定の作成
 *
 * @param	wk	
 * @param	no	
 * @param	sx	
 * @param	sy	
 *
 * @retval	static inline void	
 *
 */
//--------------------------------------------------------------
static inline void CODE_HitRectSet( BR_CODEIN_WORK* wk, int no, s16 sx, s16 sy )
{
	s16 x, y;	
	CODE_OAM* code;	
	
	code = &wk->code[ no ];
	
	CATS_ObjectPosGetCap( code->cap, &x, &y );
	
	code->hit->rect.top		= y - sy;
	code->hit->rect.left	= x - sx;
	code->hit->rect.bottom	= y + sy;
	code->hit->rect.right	= x + sx;
	
	///< 左端が押しにくいので、4ドット広げとく
	if ( no == 0 ){
		code->hit->rect.left -= 4;
	}
}


//--------------------------------------------------------------
/**
 * @brief	当たり判定の設定
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BRCI_pv_disp_HitTableSet( BR_CODEIN_WORK* wk )
{
	int i;
	s16 sx;
	s16 sy;
	
	for ( i = 0; i < wk->code_max; i++ ){
				
		if ( i >= wk->ls && i < wk->le ){
			
			sx = L_SIZE / 2;
			sy = L_SIZE / 2;
		}
		else {
			
			sx = S_SIZE / 2;
			sy = S_SIZE;
		}
		CODE_HitRectSet( wk, i, sx, sy );
	}
}


//==============================================================================
/**
 * @file	ファイル名
 * @brief	簡単な説明を書く
 * @author	goto
 * @date	2008.01.21(月)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
#include "common.h"

#include "savedata/savedata.h"

#include "system/pms_data.h"
#include "system/pm_str.h"

#include "gds_battle_rec.h"
#include "gds_ranking.h"
#include "gds_boxshot.h"
#include "gds_dressup.h"

#include "savedata/gds_profile.h"
#include "savedata/imageclip_data.h"
#include "poketool/boxdata.h"
#include "poketool/monsno.h"

#include "system/msgdata_util.h"
#include "system/wordset.h"
#include "msgdata/msg.naix"
#include "system/fontproc.h"
#include "gflib/strbuf_family.h"
#include "system/wipe.h"

#include "system/procsys.h"
#include "communication/wm_icon.h"
#include "communication/comm_system.h"
#include "communication/comm_state.h"
#include "communication/comm_def.h"

#include "savedata/system_data.h"

#include "gds_rap.h"

#include "br_private.h"

//==============================================================
// Prototype
//==============================================================
static inline BOOL gdsrap_init_check( BR_WORK* wk );



//--------------------------------------------------------------
/**
 * @brief	初期化した？
 *
 * @param	wk	
 *
 * @retval	static inline BOOL	
 *
 */
//--------------------------------------------------------------
static inline BOOL gdsrap_init_check( BR_WORK* wk )
{
	return wk->sys.gdsrap.comm_initialize_ok;
}


//--------------------------------------------------------------
/**
 * @brief	ワーク取得
 *
 * @param	wk	
 *
 * @retval	GDS_RAP_WORK*	
 *
 */
//--------------------------------------------------------------
GDS_RAP_WORK* BR_GDSRAP_WorkGet( BR_WORK* wk )
{
	return &wk->sys.gdsrap;
}


//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_GDSRAP_Init( BR_WORK* wk )
{
	GDSRAP_INIT_DATA init_data;

	init_data.gs_profile_id = SYSTEMDATA_GetDpwInfo( SaveData_GetSystemData( wk->save ) );
	init_data.heap_id		= HEAPID_BR;
	init_data.savedata		= wk->save;

	init_data.response_callback.callback_work				= wk;
	init_data.response_callback.func_dressupshot_regist		= BR_Response_DressRegist;
	init_data.response_callback.func_dressupshot_get		= BR_Response_DressGet;
	init_data.response_callback.func_boxshot_regist			= BR_Response_BoxRegist;
	init_data.response_callback.func_boxshot_get			= BR_Response_BoxGet;
	init_data.response_callback.func_ranking_type_get		= BR_Response_RankingType;
	init_data.response_callback.func_ranking_update_get		= BR_Response_RankingUpdate;
	init_data.response_callback.func_battle_video_regist	= BR_Response_VideoRegist;
	init_data.response_callback.func_battle_video_search_get= BR_Response_BattleVideoSearch;
	init_data.response_callback.func_battle_video_data_get 	= BR_Response_VideoGet;
	init_data.response_callback.func_battle_video_favorite_regist = BR_Response_VideoDL;

	init_data.callback_work				= wk;
	init_data.callback_error_msg_wide	= NULL;//自前でエラーメッセージ表示　BR_Error_CallBack;	//< エラー用

	GDSRAP_Init( &wk->sys.gdsrap, &init_data );		//通信ライブラリ初期化	
	
//	OS_Printf( " バトルレコーダー通信ライブラリ初期化 \n" );
}


//--------------------------------------------------------------
/**
 * @brief	状態チェック
 *
 * @param	wk	
 *
 * @retval	BOOL
 *
 */
//--------------------------------------------------------------
BOOL BR_GDSRAP_MoveStatusAllCheck( BR_WORK* wk )
{
	if ( gdsrap_init_check( wk ) == FALSE ){ return FALSE; }

	return GDSRAP_MoveStatusAllCheck( BR_GDSRAP_WorkGet( wk ) );
}


//--------------------------------------------------------------
/**
 * @brief	通信メイン 毎フレーム呼ぶやつ
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_GDSRAP_Main( BR_WORK* wk )
{
	if ( gdsrap_init_check( wk ) == FALSE ){ return; }
	
	GDSRAP_Main( BR_GDSRAP_WorkGet( wk ) );
	
//	OS_Printf( " バトルレコーダー通信 \n" );
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
void BR_GDSRAP_Exit( BR_WORK* wk )
{
	if ( gdsrap_init_check( wk ) == FALSE ){ return; }

	GDSRAP_Exit( BR_GDSRAP_WorkGet( wk ) );
	
	
//	OS_Printf( " バトルレコーダー通信終了 \n" );
}


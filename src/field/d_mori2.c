#ifdef PM_DEBUG
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		d_mori2.c
 *	@brief		森さんのデバックメニュー常駐部分
 *	@author		tomoya takahashi
 *	@data		2007.04.17
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"
#include "gflib/strbuf.h"
#include "gflib/strbuf_family.h"
#include "system/lib_pack.h"
#include "system/bmp_list.h"
#include "fieldmap.h"
#include "worldmap.h"
#include "fieldsys.h"
#include "field_event.h"
#include "system/fontproc.h"
#include "system/arc_util.h"
#include "system/window.h"
#include "fld_bmp.h"
#include "system/pm_str.h"
#include "fld_debug.h"
#include "talk_msg.h"
#include "encount_effect.h"
#include "poketool/poke_tool.h"
#include "poketool/pokeparty.h"
#include "msgdata/msg.naix"
#include "system/snd_play.h"
#include "system/snd_tool.h"
#include "savedata/savedata.h"
#include "savedata/wifilist.h"
#include "savedata/config.h"
#include "savedata/perapvoice.h"
#include "savedata/sodateyadata.h"
#include "sodateya.h"
#include "itemtool/item.h"

#include "msgdata/msg_debug_mori.h"
#include "application/namein.h"
#include "application/tradelist.h"
#include "communication/communication.h"

#include "communication/wpb.h"
#include "communication/wpbpokemon.h"
#include "communication/wpbutil.h"
#include  "communication/wm_icon.h"

#define __D_MORI2_H_GLOBAL
#include "d_mori2.h"

//-----------------------------------------------------------------------------
/**
 *					コーディング規約
 *		●関数名
 *				１文字目は大文字それ以降は小文字にする
 *		●変数名
 *				・変数共通
 *						constには c_ を付ける
 *						staticには s_ を付ける
 *						ポインタには p_ を付ける
 *						全て合わさると csp_ となる
 *				・グローバル変数
 *						１文字目は大文字
 *				・関数内変数
 *						小文字と”＿”と数字を使用する 関数の引数もこれと同じ
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/**
 *		名前入力呼び出しテスト
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *
 *	@brief 名前入力呼び出しテスト
 *
 *	@param	TCB_PTR tcb
 *	@param	work 
 *
 *	@return
 *
 *
 */
//-----------------------------------------------------------------------------
void TestNameIn_Call( TCB_PTR tcb, void* work ) 
{
	TEST_PROC_WORK* wk = work;
	
	switch( wk->seq ){
	case 0:		// フェードアウト
		FieldSystemProc_SeqHoldEnd();		
		wk->seq++;
		break;

	case 1:		// フィールド破棄
		if(wk->sub == 0){
			wk->sub++;

			// 名前入力用引数ワーク作成
			wk->NameInParam = NameIn_ParamAllocMake(HEAPID_WORLD, NAMEIN_MYNAME 
								/* NAMEIN_FRIENDNAME */ /*NAMEIN_FRIENDCODE*/ /*NAMEIN_POKEMON*/ , 
								2, NAMEIN_PERSON_LENGTH, SaveData_GetConfig(wk->fsys->savedata) );
//			wk->NameInParam = NameIn_ParamAllocMake(HEAPID_WORLD, NAMEIN_MYNAME, PM_MALE, 5);
//			PM_strcpy(wk->NameInParam->str,test_name);
			wk->NameInParam->info = MyStatus_GetMySex(SaveData_GetMyStatus(wk->fsys->savedata));
			wk->NameInParam->get_msg_id = 0;
			wk->NameInParam->sex = MyStatus_GetMySex(SaveData_GetMyStatus(wk->fsys->savedata));
			wk->NameInParam->boxdata = SaveData_GetBoxData(wk->fsys->savedata);
			OS_Printf("BOXDATA Addr = %08x\n",wk->NameInParam->boxdata);
			// 名前入力PROC作成
			NameInput_SetProc( wk->fsys, wk->NameInParam );

			GameSystem_FinishFieldProc( wk->fsys );

		}else{

			// 終了が完了したかチェック
			if( FieldEvent_Cmd_WaitMapProcStart( wk->fsys ) == FALSE ){
				wk->sub = 0;
				wk->seq++;
			}
		}
		break;

	case 2:		// 名前入力初期化
		wk->seq++;
		break;
	case 3:		// 名前入力終了待ち
		if( GameSystem_CheckSubProcExists( wk->fsys ) == FALSE ){
			{
				int i;
				OS_Printf("書いた名前は ");
				for(i=0;i<5;i++){
					OS_Printf("%x",wk->NameInParam->str[i]);
				}
				OS_Printf("\n");
				OS_Printf("キャンセル？ = %d\n",wk->NameInParam->cancel);
			}
//			sys_FreeMemory( HEAPID_WORLD, wk->NameInParam );

			// 書いた名前をMYSTATUSに反映させる
			if(!wk->NameInParam->cancel){
				MyStatus_SetMyName(SaveData_GetMyStatus(wk->fsys->savedata),wk->NameInParam->str);
			}
			NameIn_ParamDelete( wk->NameInParam );
			// フィールド復帰
			GameSystem_CreateFieldProc( wk->fsys );
			wk->seq++;

		}
		break;
	case 4:
		if( GameSystem_CheckFieldProcExists(wk->fsys) ){
			FieldFadeWipeSet( FLD_DISP_BRIGHT_BLACKIN );
			// タスク終了
			TCB_Delete( tcb );
			sys_FreeMemoryEz( work );
		}
		break;
	}
}


//------------------------------------------------------------------
/**
 * 交換リスト呼び出しテスト
 *
 * @param   tcb		
 * @param   work		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
void TestTradeList_Call( TCB_PTR tcb, void* work ) 
{
	TEST_PROC_WORK* wk = work;
	
	switch( wk->seq ){
	case 0:		// フェードアウト
		FieldSystemProc_SeqHoldEnd();		
		wk->seq++;
		break;

	case 1:		// フィールド破棄
		if(wk->sub == 0){
			wk->sub++;

			// 交換リストPROC作成
//			TradeList_SetProc( wk->fsys );
//			OekakiBoard_SetProc( wk->fsys );
			Field_MySign_SetProc( wk->fsys );

			GameSystem_FinishFieldProc( wk->fsys );

		}else{

			// 終了が完了したかチェック
			if( FieldEvent_Cmd_WaitMapProcStart( wk->fsys ) == FALSE ){
				wk->sub = 0;
				wk->seq++;
			}
		}
		break;
	case 2:		// 名前入力終了待ち
		if( GameSystem_CheckSubProcExists( wk->fsys ) == FALSE ){
			// フィールド復帰
			GameSystem_CreateFieldProc( wk->fsys );
			wk->seq++;

		}
		break;
	case 3:
		if( GameSystem_CheckFieldProcExists(wk->fsys) ){
			FieldFadeWipeSet( FLD_DISP_BRIGHT_BLACKIN );
			// タスク終了
			TCB_Delete( tcb );
			sys_FreeMemoryEz( work );
		}
		break;
	}
}

//------------------------------------------------------------------
/**
 * 世界交換呼び出しテスト
 *
 * @param   tcb		
 * @param   work		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
void TestWorldTrade_Call( TCB_PTR tcb, void* work ) 
{
	TEST_PROC_WORK* wk = work;
	
	switch( wk->seq ){
	case 0:		// フェードアウト
		FieldSystemProc_SeqHoldEnd();		
		wk->seq++;
		break;

	case 1:		// フィールド破棄
		if(wk->sub == 0){
			wk->sub++;

			// 世界交換PROC作成
			Field_WorldTrade_SetProc( wk->fsys, 0 );

			GameSystem_FinishFieldProc( wk->fsys );

		}else{

			// 終了が完了したかチェック
			if( FieldEvent_Cmd_WaitMapProcStart( wk->fsys ) == FALSE ){
				wk->sub = 0;
				wk->seq++;
			}
		}
		break;
	case 2:		// 世界交換終了待ち
		if( GameSystem_CheckSubProcExists( wk->fsys ) == FALSE ){
			// フィールド復帰
			GameSystem_CreateFieldProc( wk->fsys );
			wk->seq++;

		}
		break;
	case 3:
		if( GameSystem_CheckFieldProcExists(wk->fsys) ){
			FieldFadeWipeSet( FLD_DISP_BRIGHT_BLACKIN );
			// タスク終了
			TCB_Delete( tcb );
			sys_FreeMemoryEz( work );
		}
		break;
	}
}
#endif // PM_DEBUG

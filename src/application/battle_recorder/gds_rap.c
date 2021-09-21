//==============================================================================
/**
 * @file	gds_rap.h
 * @brief	GDSライブラリをラッパーしたもの
 * @author	matsuda
 * @date	2008.01.09(水)
 */
//==============================================================================
#include "common.h"
#include <dwc.h>
#include "wifi/dwc_rap.h"
#include "savedata\savedata.h"
#include "gds_battle_rec.h"
#include "gds_ranking.h"
#include "gds_boxshot.h"
#include "gds_dressup.h"

#include "savedata/imageclip_data.h"
#include "poketool/boxdata.h"
#include "poketool/monsno.h"
#include "gflib/strbuf_family.h"
#include "savedata/gds_profile.h"
#include "savedata/gds_profile_types.h"

#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "system/msgdata_util.h"
#include "system/wordset.h"
#include "msgdata/msg.naix"
#include "system/fontproc.h"
#include "gflib/strbuf_family.h"

#include "communication\comm_system.h"
#include "communication\comm_state.h"
#include "communication\comm_def.h"
#include "communication/wm_icon.h"
#include "communication\communication.h"

#include "gds_battle_rec.h"
#include "gds_ranking.h"
#include "gds_boxshot.h"
#include "gds_dressup.h"

#include "gds_rap.h"
#include "gds_rap_response.h"
#include "gds_data_conv.h"

#include "msgdata/msg_wifi_system.h"
#include "savedata/playtime.h"
#include "savedata/battle_rec.h"

#include "battle/battle_common.h"

#include "gflib/strcode.h"
#include "gflib\msg_print.h"
#include "system\fontproc.h"
#include "system\msgdata.h"
#include "msgdata/msg.naix"
#include "msgdata/msg_battle_rec.h"


//==============================================================================
//	定数定義
//==============================================================================
///DWCに渡すワークのサイズ
#define MYDWC_HEAPSIZE		0x20000

///エラーメッセージ展開用バッファのサイズ
#define DWC_ERROR_BUF_NUM		(16*8*2)

///送信前のウェイト(送信しているメッセージを見せる為にウェイトを入れている)
#define GDS_SEND_BEFORE_WAIT	(60)

//--------------------------------------------------------------
//	サブシーケンスの戻り値
//--------------------------------------------------------------
enum{
	SUBSEQ_CONTINUE,		///<サブシーケンス：維持
	SUBSEQ_NEXT,			///<サブシーケンス：次のサブシーケンスへ
	SUBSEQ_PROCCESS_CHANGE,	///<サブシーケンス：サブシーケンステーブル変更
};

//==============================================================================
//	構造体定義
//==============================================================================
///サブプロセスシーケンスの関数型定義
typedef int (*SUBPROCESS_FUNC)(GDS_RAP_WORK *, GDS_RAP_SUB_PROCCESS_WORK *sub_work);


//==============================================================================
//	プロトタイプ宣言
//==============================================================================
static int GDSRAP_SEQ_GDS_Release(GDS_RAP_WORK *gdsrap, GDS_RAP_SUB_PROCCESS_WORK *sub_work);
static int GDSRAP_SEQ_Error_Cleanup(GDS_RAP_WORK *gdsrap, GDS_RAP_SUB_PROCCESS_WORK *sub_work);
static int GDSRAP_SEQ_Error_DWC_Message(GDS_RAP_WORK *gdsrap, GDS_RAP_SUB_PROCCESS_WORK *sub_work);
static int GDSRAP_SEQ_Error_DWC_MessageWait(GDS_RAP_WORK *gdsrap, GDS_RAP_SUB_PROCCESS_WORK *sub_work);

static int GDSRAP_MAIN_Send(GDS_RAP_WORK *gdsrap);
static int GDSRAP_MAIN_Recv(GDS_RAP_WORK *gdsrap);
static int GDSRAP_ProccessCheck(GDS_RAP_WORK *gdsrap);

static void _systemMessagePrint(GDS_RAP_WORK *gdsrap, STRBUF *str_buf);
static void errorDisp(GDS_RAP_WORK *gdsrap, int type, int code);

static int Local_GetResponse(GDS_RAP_WORK *gdsrap);

static void * LIB_Heap_Init(int heap_id);
static void LIB_Heap_Exit(void);

static BOOL RecvSubProccess_Normal(void *work_gdsrap, void *work_recv_sub_work);
static BOOL RecvSubProccess_VideoSendFlagResetSave(void *work_gdsrap, void *work_recv_sub_work);
static BOOL RecvSubProccess_DataNumberSetSave(void *work_gdsrap, void *work_recv_sub_work);
static BOOL RecvSubProccess_DressSendFlagResetSave(void *work_gdsrap, void *work_recv_sub_work);
static BOOL RecvSubProccess_BoxSendFlagResetSave(void *work_gdsrap, void *work_recv_sub_work);
static BOOL RecvSubProccess_SystemError(void *work_gdsrap, void *work_recv_sub_work);

static SAVE_RESULT GdsDivSave(GDS_RAP_WORK *gdsrap);

//==============================================================================
//	シーケンステーブル
//==============================================================================
///Wifi接続エラー発生時の強制切断処理
static const SUBPROCESS_FUNC SubSeq_WifiError[] = {
	GDSRAP_SEQ_Error_Cleanup,
	GDSRAP_SEQ_GDS_Release,
	GDSRAP_SEQ_Error_DWC_Message,
	GDSRAP_SEQ_Error_DWC_MessageWait,
	NULL,	//終端
};

///サーバーサービスエラー発生時の強制切断処理(ネットは接続出来ているが、サーバーが止まっている場合)
static const SUBPROCESS_FUNC SubSeq_ServerServiceError[] = {
//	GDSRAP_SEQ_Error_ServerService_Message,
//	GDSRAP_SEQ_Error_ServerService_MessageWait,
	NULL,	//終端
};

//--------------------------------------------------------------
//	サブプロセステーブル
//--------------------------------------------------------------
///サブプロセステーブル
///		※GDSRAP_PROCESS_REQと並びを同じにしておくこと！！
static const SUBPROCESS_FUNC * const GDSRapSeqTbl[] = {
	NULL,
	
	//以下、システム内部でのみ呼び出し
	SubSeq_WifiError,			//GDSRAP_PROCESS_REQ_WIFI_ERROR
	SubSeq_ServerServiceError,	//GDSRAP_PROCESS_REQ_SERVER_ERROR
};



//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   GDSライブラリ初期化処理
 *
 * @param   gdsrap				GDSラッパーワークへのポインタ
 * @param   init_data			初期化データ
 *
 * @retval  TRUE
 */
//--------------------------------------------------------------
int GDSRAP_Init(GDS_RAP_WORK *gdsrap, const GDSRAP_INIT_DATA *init_data)
{
	int ret;
	
	MI_CpuClear8(gdsrap, sizeof(GDS_RAP_WORK));
	gdsrap->heap_id = init_data->heap_id;
	gdsrap->savedata = init_data->savedata;
	gdsrap->response_callback = init_data->response_callback;
	gdsrap->callback_work = init_data->callback_work;
	gdsrap->callback_error_msg_wide = init_data->callback_error_msg_wide;
	gdsrap->laststat = -1;
	gdsrap->send_req = POKE_NET_GDS_REQCODE_LAST;
	gdsrap->recv_wait_req = POKE_NET_GDS_REQCODE_LAST;
	
	gdsrap->response = sys_AllocMemory(init_data->heap_id, POKE_NET_GDS_GetResponseMaxSize(-1));
	MI_CpuClear8(gdsrap->response, POKE_NET_GDS_GetResponseMaxSize(-1));
	OS_TPrintf("レスポンスバッファのサイズ＝%x\n", POKE_NET_GDS_GetResponseMaxSize(-1));

	//メッセージマネージャ作成
	gdsrap->msgman_wifisys = MSGMAN_Create(MSGMAN_TYPE_NORMAL, 
		ARC_MSG, NARC_msg_wifi_system_dat, init_data->heap_id);
	gdsrap->wordset = WORDSET_Create(init_data->heap_id);	//単語バッファ作成
	gdsrap->ErrorString = STRBUF_Create(DWC_ERROR_BUF_NUM, init_data->heap_id);
	
	//※check　暫定ヒープ作成
	gdsrap->areanaLo = LIB_Heap_Init(init_data->heap_id);

	gdsrap->pokenet_auth.PID = init_data->gs_profile_id;
	gdsrap->pokenet_auth.ROMCode = PM_VERSION;
	gdsrap->pokenet_auth.LangCode = PM_LANG;
	ret = POKE_NET_GDS_Initialize(&gdsrap->pokenet_auth);
	GF_ASSERT(ret == TRUE);		//初期化に失敗する事はありえないはず
	gdsrap->gdslib_initialize = TRUE;

	gdsrap->comm_initialize_ok = TRUE;
	
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   GDSライブラリ破棄
 *
 * @param   gdsrap		
 */
//--------------------------------------------------------------
void GDSRAP_Exit(GDS_RAP_WORK *gdsrap)
{
	//GDSライブラリ解放
	POKE_NET_GDS_Release();
	gdsrap->gdslib_initialize = FALSE;

	//メッセージマネージャ削除
	STRBUF_Delete(gdsrap->ErrorString);
	WORDSET_Delete(gdsrap->wordset);
	MSGMAN_Delete(gdsrap->msgman_wifisys);

	sys_FreeMemoryEz(gdsrap->response);

	//※check　暫定ヒープ解放
	LIB_Heap_Exit();
	sys_FreeMemoryEz(gdsrap->areanaLo);
}

//--------------------------------------------------------------
/**
 * @brief   ポケモンWifiGDSライブラリの解放を行います
 *
 * @param   gdsrap		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
static int GDSRAP_SEQ_GDS_Release(GDS_RAP_WORK *gdsrap, GDS_RAP_SUB_PROCCESS_WORK *sub_work)
{
	POKE_NET_GDS_Release();
	
	gdsrap->gdslib_initialize = FALSE;
	
	return SUBSEQ_NEXT;
}

//--------------------------------------------------------------
/**
 * @brief   Wifi接続エラーが発生した時の切断処理を行います
 *
 * @param   gdsrap		
 *
 * @retval  
 */
//--------------------------------------------------------------
static int GDSRAP_SEQ_Error_Cleanup(GDS_RAP_WORK *gdsrap, GDS_RAP_SUB_PROCCESS_WORK *sub_work)
{
	OS_TPrintf("----エラーの為、WIFI強制切断\n");
	
	DWC_ClearError();
	DWC_CleanupInet();
	CommStateWifiDPWEnd();
	
#if 0	//GDSではそもそもDpw_Tr_Initを使っていないので無効
	//画面を抜けずに2度連続で「Eメール設定」をしてWifiに繋げようとすると
	//"dpw_tr.c:150 Panic:dpw tr is already initialized."
	//のエラーが出るのできちんとこの終了関数を呼ぶようにする 2007.10.26(金) matsuda
	Dpw_Tr_End();
#endif

	gdsrap->connect_success = FALSE;

	return SUBSEQ_NEXT;
}

//--------------------------------------------------------------
/**
 * @brief   Wifi接続エラーメッセージ表示処理
 *
 * @param   gdsrap		
 *
 * @retval  
 */
//--------------------------------------------------------------
static int GDSRAP_SEQ_Error_DWC_Message(GDS_RAP_WORK *gdsrap, GDS_RAP_SUB_PROCCESS_WORK *sub_work)
{
	int type, msgno;
	
    type =  mydwc_errorType(-gdsrap->ErrorCode, gdsrap->ErrorType);
	OS_Printf("error code = %d, type = %d\n", gdsrap->ErrorCode, type);

    errorDisp(gdsrap, type, -gdsrap->ErrorCode);
	
	return SUBSEQ_NEXT;
}

//--------------------------------------------------------------
/**
 * @brief   Wifi接続エラーメッセージ表示：決定キーでの終了待ち
 *
 * @param   gdsrap		
 *
 * @retval  
 */
//--------------------------------------------------------------
static int GDSRAP_SEQ_Error_DWC_MessageWait(GDS_RAP_WORK *gdsrap, GDS_RAP_SUB_PROCCESS_WORK *sub_work)
{
	if((sys.trg & PAD_BUTTON_DECIDE) || (sys.trg & PAD_BUTTON_CANCEL)){
		_systemMessagePrint(gdsrap, NULL);
		return SUBSEQ_NEXT;
	}
	return SUBSEQ_CONTINUE;
}



//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   送信リクエスト：ドレスアップショット登録
 *
 * @param   gdsrap			
 * @param   gt_profile		
 * @param   gt_dress		
 *
 * @retval  TRUE：リクエストを受け付けた
 * @retval  FALSE：受け付けられなかった
 */
//--------------------------------------------------------------
int GDSRAP_Tool_Send_DressupUpload(GDS_RAP_WORK *gdsrap, GDS_PROFILE_PTR gpp, IMC_TELEVISION_SAVEDATA * dress)
{
	if(GDSRAP_MoveStatusAllCheck(gdsrap) == FALSE){
		return FALSE;
	}

	//GDSプロフィールをGT_GDSプロフィールへ変換
	GDS_CONV_GDSProfile_to_GTGDSProfile(
		gdsrap->savedata, gpp, &gdsrap->send_buf.gt_dress_send.profile);
	//GDSドレスをGT_GDSドレスへ変換
	GDS_CONV_Dress_to_GTDress(gdsrap->savedata, dress, &gdsrap->send_buf.gt_dress_send.dressup);
	
	gdsrap->send_before_wait = GDS_SEND_BEFORE_WAIT;
	gdsrap->send_req = POKE_NET_GDS_REQCODE_DRESSUPSHOT_REGIST;
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   送信リクエスト：ドレスアップショット ダウンロード
 *
 * @param   gdsrap			
 * @param   monsno			受信するポケモン番号
 *
 * @retval  TRUE：リクエストを受け付けた
 * @retval  FALSE：受け付けられなかった
 */
//--------------------------------------------------------------
int GDSRAP_Tool_Send_DressupDownload(GDS_RAP_WORK *gdsrap, int monsno)
{
	if(GDSRAP_MoveStatusAllCheck(gdsrap) == FALSE){
		return FALSE;
	}

	gdsrap->send_buf.sub_para.dressup.recv_monsno = monsno;
	
	gdsrap->send_req = POKE_NET_GDS_REQCODE_DRESSUPSHOT_GET;
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   送信リクエスト：ボックスショット アップロード
 *
 * @param   gdsrap		
 * @param   category_no		登録するカテゴリー番号
 * @param   gpp				GDSプロフィールへのポインタ
 * @param   boxdata			ボックスデータへのポインタ
 * @param   tray_number		ボックスのトレイ番号
 *
 * @retval  TRUE：リクエストを受け付けた
 * @retval  FALSE：受け付けられなかった
 */
//--------------------------------------------------------------
int GDSRAP_Tool_Send_BoxshotUpload(GDS_RAP_WORK *gdsrap, int category_no, GDS_PROFILE_PTR gpp, const BOX_DATA *boxdata, int tray_number)
{
	if(GDSRAP_MoveStatusAllCheck(gdsrap) == FALSE){
		return FALSE;
	}

	gdsrap->send_buf.sub_para.box.category_no = category_no;
	gdsrap->send_buf.sub_para.box.tray_number = tray_number;	//送信済みフラグを戻す為必要

	//GDSプロフィールをGT_GDSプロフィールへ変換
	GDS_CONV_GDSProfile_to_GTGDSProfile(
		gdsrap->savedata, gpp, &gdsrap->send_buf.gt_box_send.profile);
	//ボックスデータをGTボックスデータへ変換
	GDS_CONV_Box_to_GTBox(gdsrap->savedata, boxdata, tray_number, 
		&gdsrap->send_buf.gt_box_send.box_shot, gdsrap->heap_id);
	
	//GTボックスデータにカテゴリー番号をセット
	GDS_GTBoxShot_SetCategoryNo(gdsrap->savedata, 
		&gdsrap->send_buf.gt_box_send.box_shot, category_no);
	
	gdsrap->send_before_wait = GDS_SEND_BEFORE_WAIT;
	gdsrap->send_req = POKE_NET_GDS_REQCODE_BOXSHOT_REGIST;
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   送信リクエスト：ボックスショット ダウンロード
 *
 * @param   gdsrap			
 * @param   category_no		受信するカテゴリー番号
 *
 * @retval  TRUE：リクエストを受け付けた
 * @retval  FALSE：受け付けられなかった
 */
//--------------------------------------------------------------
int GDSRAP_Tool_Send_BoxshotDownload(GDS_RAP_WORK *gdsrap, int category_no)
{
	if(GDSRAP_MoveStatusAllCheck(gdsrap) == FALSE){
		return FALSE;
	}

	gdsrap->send_buf.sub_para.box.category_no = category_no;
	
	gdsrap->send_req = POKE_NET_GDS_REQCODE_BOXSHOT_GET;
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   送信リクエスト：開催中のランキングタイプ　ダウンロード
 *
 * @param   gdsrap		
 *
 * @retval  TRUE：リクエストを受け付けた
 * @retval  FALSE：受け付けられなかった
 */
//--------------------------------------------------------------
int GDSRAP_Tool_Send_RankingTypeDownload(GDS_RAP_WORK *gdsrap)
{
	if(GDSRAP_MoveStatusAllCheck(gdsrap) == FALSE){
		return FALSE;
	}
	
	gdsrap->send_req = POKE_NET_GDS_REQCODE_RANKING_GETTYPE;
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   送信リクエスト：ランキング更新(自分のデータ送信＆結果受信)
 *
 * @param   gdsrap		
 * @param   gpp						GDSプロフィールへのポインタ
 * @param   ranking_mydata[]		自分のランキングデータ(配列へのポインタ)
 *
 * @retval  TRUE：リクエストを受け付けた
 * @retval  FALSE：受け付けられなかった
 */
//--------------------------------------------------------------
int GDSRAP_Tool_Send_RankingUpdate(GDS_RAP_WORK *gdsrap, GDS_PROFILE_PTR gpp, GT_RANKING_MYDATA ranking_mydata[])
{
	PLAYTIME *playtime;
	int i;
	
	if(GDSRAP_MoveStatusAllCheck(gdsrap) == FALSE){
		return FALSE;
	}

	//プレイ時間セット
	playtime = SaveData_GetPlayTime(gdsrap->savedata);
	MI_CpuCopy8(playtime, &gdsrap->send_buf.gt_ranking_mydata_send.playtime, sizeof(GT_PLAYTIME));

	//GDSプロフィールをランキングプロフィールへ変換
	GDS_CONV_GDSProfile_to_GTRankingProfile(gdsrap->savedata, gpp, 
		&gdsrap->send_buf.gt_ranking_mydata_send.profile);
	
	//ランキングデータ
	for(i = 0; i < GT_RANKING_WEEK_NUM; i++){
		gdsrap->send_buf.gt_ranking_mydata_send.my_data[i] = ranking_mydata[i];
	}

	gdsrap->send_req = POKE_NET_GDS_REQCODE_RANKING_UPDATE;
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   送信リクエスト：バトルデータ登録
 *
 * @param   gdsrap		
 * @param   gpp						GDSプロフィールへのポインタ
 *
 * @retval  TRUE：リクエストを受け付けた
 * @retval  FALSE：受け付けられなかった
 * 
 * 録画データはサイズが巨大な為、コピーせずに、brsで読み込んでいるデータをそのまま送信します。
 * brsにデータを展開してからこの関数を使用するようにしてください。
 *
 * ！！この関数使用後、brsに読み込んでいるプロフィールデータの書き換え、
 * 	   録画データ本体の暗号化を行うのでこのままセーブしたり再生しないように注意！！
 */
//--------------------------------------------------------------
int GDSRAP_Tool_Send_BattleVideoUpload(GDS_RAP_WORK *gdsrap, GDS_PROFILE_PTR gpp)
{
	GDS_PROFILE_PTR profile_ptr;
	
	if(GDSRAP_MoveStatusAllCheck(gdsrap) == FALSE){
		return FALSE;
	}

	//録画データとGDSプロフィールはGT系と全く同じ型のはず
	GF_ASSERT((BattleRec_GetWorkSize()-sizeof(EX_CERTIFY_SAVE_KEY)) == sizeof(GT_BATTLE_REC_SEND));
	GF_ASSERT(sizeof(GT_GDS_PROFILE) == sizeof(GDS_PROFILE));

	//録画データは巨大な為、コピーせずに、そのままbrsのデータを送信する
	gdsrap->send_buf.gt_battle_rec_send_ptr = (GT_BATTLE_REC_SEND *)BattleRec_RecWorkAdrsGet();
	//brsに展開されている録画データ本体は復号化されているので、送信する前に再度暗号化する
	BattleRec_GDS_SendData_Conv(gdsrap->savedata);
	
	//GDSプロフィールのみ、最新のを適用する為、上書きする
	profile_ptr = BattleRec_GDSProfilePtrGet();
	MI_CpuCopy8(gpp, profile_ptr, sizeof(GT_GDS_PROFILE));
	
	gdsrap->send_before_wait = GDS_SEND_BEFORE_WAIT;
	gdsrap->send_req = POKE_NET_GDS_REQCODE_BATTLEDATA_REGIST;
	return TRUE;
}

///強制的にフリーワードの自己紹介メッセージをセットする
void DEBUG_GDSRAP_SendVideoProfileFreeWordSet(GDS_RAP_WORK *gdsrap, u16 *set_code)
{
#ifdef PM_DEBUG
	GDS_PROFILE_PTR profile_ptr;

	profile_ptr = BattleRec_GDSProfilePtrGet();
	MI_CpuCopy16(set_code, profile_ptr->event_self_introduction, GT_EVENT_SELF_INTRO*2);
	profile_ptr->message_flag = 1;
	DEBUG_BattleRec_SecureFlagSet(gdsrap->savedata);
#endif
}

//--------------------------------------------------------------
/**
 * @brief   送信リクエスト：バトルビデオダウンロード(詳細検索)
 *
 * @param   gdsrap		
 * @param   monsno			ポケモン番号(指定なし：GT_BATTLE_REC_SEARCH_MONSNO_NONE)
 * @param   battle_mode		検索施設(指定なし：クリア前=GT_BATTLE_MODE_EXCLUSION_FRONTIER
 * 											   クリア後=GT_BATTLE_REC_SEARCH_BATTLE_MODE_NONE)
 * @param   country_code	国コード(指定なし：GT_BATTLE_REC_SEARCH_COUNTRY_CODE_NONE)
 * @param   local_code		地方コード(指定なし：GT_BATTLE_REC_SEARCH_LOCAL_CODE_NONE)
 *
 * @retval  TRUE：リクエストを受け付けた
 * @retval  FALSE：受け付けられなかった
 */
//--------------------------------------------------------------
int GDSRAP_Tool_Send_BattleVideoSearchDownload(GDS_RAP_WORK *gdsrap, u16 monsno, u8 battle_mode, u8 country_code, u8 local_code)
{
	if(GDSRAP_MoveStatusAllCheck(gdsrap) == FALSE){
		return FALSE;
	}

	MI_CpuClear8(&gdsrap->send_buf.gt_battle_rec_search, sizeof(GT_BATTLE_REC_SEARCH_SEND));
	gdsrap->send_buf.gt_battle_rec_search.monsno = monsno;
	gdsrap->send_buf.gt_battle_rec_search.battle_mode = battle_mode;
	gdsrap->send_buf.gt_battle_rec_search.country_code = country_code;
	gdsrap->send_buf.gt_battle_rec_search.local_code = local_code;
	gdsrap->send_buf.gt_battle_rec_search.server_version = BATTLE_SERVER_VERSION;

	gdsrap->send_req_option = POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE_CONDITION;
	gdsrap->send_req = POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH;
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   送信リクエスト：バトルビデオダウンロード(最新30件:コロシアムのみ)
 *
 * @param   gdsrap		
 *
 * @retval  TRUE：リクエストを受け付けた
 * @retval  FALSE：受け付けられなかった
 */
//--------------------------------------------------------------
int GDSRAP_Tool_Send_BattleVideoNewDownload_ColosseumOnly(GDS_RAP_WORK *gdsrap)
{
	if(GDSRAP_MoveStatusAllCheck(gdsrap) == FALSE){
		return FALSE;
	}

	MI_CpuClear8(&gdsrap->send_buf.gt_battle_rec_search, sizeof(GT_BATTLE_REC_SEARCH_SEND));
	gdsrap->send_buf.gt_battle_rec_search.monsno = GT_BATTLE_REC_SEARCH_MONSNO_NONE;
	gdsrap->send_buf.gt_battle_rec_search.battle_mode = GT_BATTLE_MODE_EXCLUSION_FRONTIER;
	gdsrap->send_buf.gt_battle_rec_search.country_code = GT_BATTLE_REC_SEARCH_COUNTRY_CODE_NONE;
	gdsrap->send_buf.gt_battle_rec_search.local_code = GT_BATTLE_REC_SEARCH_LOCAL_CODE_NONE;
	gdsrap->send_buf.gt_battle_rec_search.server_version = BATTLE_SERVER_VERSION;

	gdsrap->send_req_option = POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE_CONDITION;
	gdsrap->send_req = POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH;
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   送信リクエスト：バトルビデオダウンロード(最新30件)
 *
 * @param   gdsrap		
 *
 * @retval  TRUE：リクエストを受け付けた
 * @retval  FALSE：受け付けられなかった
 */
//--------------------------------------------------------------
int GDSRAP_Tool_Send_BattleVideoNewDownload(GDS_RAP_WORK *gdsrap)
{
	if(GDSRAP_MoveStatusAllCheck(gdsrap) == FALSE){
		return FALSE;
	}

	MI_CpuClear8(&gdsrap->send_buf.gt_battle_rec_search, sizeof(GT_BATTLE_REC_SEARCH_SEND));
	gdsrap->send_buf.gt_battle_rec_search.monsno = GT_BATTLE_REC_SEARCH_MONSNO_NONE;
	gdsrap->send_buf.gt_battle_rec_search.battle_mode = GT_BATTLE_REC_SEARCH_BATTLE_MODE_NONE;
	gdsrap->send_buf.gt_battle_rec_search.country_code = GT_BATTLE_REC_SEARCH_COUNTRY_CODE_NONE;
	gdsrap->send_buf.gt_battle_rec_search.local_code = GT_BATTLE_REC_SEARCH_LOCAL_CODE_NONE;
	gdsrap->send_buf.gt_battle_rec_search.server_version = BATTLE_SERVER_VERSION;

	gdsrap->send_req_option = POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE_CONDITION;
	gdsrap->send_req = POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH;
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   送信リクエスト：バトルビデオダウンロード(コロシアムお気に入りランキング上位30件)
 *
 * @param   gdsrap		
 *
 * @retval  TRUE：リクエストを受け付けた
 * @retval  FALSE：受け付けられなかった
 */
//--------------------------------------------------------------
int GDSRAP_Tool_Send_BattleVideoFavoriteDownload_Colosseum(GDS_RAP_WORK *gdsrap)
{
	if(GDSRAP_MoveStatusAllCheck(gdsrap) == FALSE){
		return FALSE;
	}

	MI_CpuClear8(&gdsrap->send_buf.gt_battle_rec_ranking_search, 
		sizeof(GT_BATTLE_REC_RANKING_SEARCH_SEND));
	gdsrap->send_buf.gt_battle_rec_ranking_search.server_version = BATTLE_SERVER_VERSION;

	gdsrap->send_req_option = POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE_RANKING;
	gdsrap->send_req = POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH;
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   送信リクエスト：バトルビデオダウンロード(フロンティアお気に入りランキング上位30件)
 *
 * @param   gdsrap		
 *
 * @retval  TRUE：リクエストを受け付けた
 * @retval  FALSE：受け付けられなかった
 */
//--------------------------------------------------------------
int GDSRAP_Tool_Send_BattleVideoFavoriteDownload_Frontier(GDS_RAP_WORK *gdsrap)
{
	if(GDSRAP_MoveStatusAllCheck(gdsrap) == FALSE){
		return FALSE;
	}

	MI_CpuClear8(&gdsrap->send_buf.gt_battle_rec_ranking_search, 
		sizeof(GT_BATTLE_REC_RANKING_SEARCH_SEND));
	gdsrap->send_buf.gt_battle_rec_ranking_search.server_version = BATTLE_SERVER_VERSION;

	gdsrap->send_req_option = POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE_EXRANKING;
	gdsrap->send_req = POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH;
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   送信リクエスト：バトルビデオ取得
 *
 * @param   gdsrap		
 * @param   data_number	データナンバー
 *
 * @retval  TRUE：リクエストを受け付けた
 * @retval  FALSE：受け付けられなかった
 */
//--------------------------------------------------------------
int GDSRAP_Tool_Send_BattleVideo_DataDownload(GDS_RAP_WORK *gdsrap, u64 data_number)
{
	if(GDSRAP_MoveStatusAllCheck(gdsrap) == FALSE){
		return FALSE;
	}

	gdsrap->send_buf.sub_para.rec.data_number = data_number;

	gdsrap->send_req = POKE_NET_GDS_REQCODE_BATTLEDATA_GET;
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   送信リクエスト：バトルビデオお気に入り登録
 *
 * @param   gdsrap		
 * @param   data_number	データナンバー
 *
 * @retval  TRUE：リクエストを受け付けた
 * @retval  FALSE：受け付けられなかった
 */
//--------------------------------------------------------------
int GDSRAP_Tool_Send_BattleVideo_FavoriteUpload(GDS_RAP_WORK *gdsrap, u64 data_number)
{
	if(GDSRAP_MoveStatusAllCheck(gdsrap) == FALSE){
		return FALSE;
	}

	gdsrap->send_buf.sub_para.rec.data_number = data_number;

	gdsrap->send_req = POKE_NET_GDS_REQCODE_BATTLEDATA_FAVORITE;
	return TRUE;
}




//--------------------------------------------------------------
/**
 * @brief   メイン処理
 *
 * @param   gdsrap		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
int GDSRAP_Main(GDS_RAP_WORK *gdsrap)
{
	int ret;

#if 0	//gds_main.c で呼ばれるようになった
	if(gdsrap->comm_initialize_ok){
		// 受信強度リンクを反映させる
		DWC_UpdateConnection();

		// Dpw_Tr_Main() だけは例外的にいつでも呼べる
	//	Dpw_Tr_Main();

		// 通信状態を確認してアイコンの表示を変える
		WirelessIconEasy_SetLevel( WM_LINK_LEVEL_3 - DWC_GetLinkLevel() );
	}
#endif

	//ネット接続中のみの処理
	if(gdsrap->gdslib_initialize == TRUE){
//		DWC_ProcessInet();
		
		//送信データ確認
		if(gdsrap->send_req != POKE_NET_GDS_REQCODE_LAST){
			if(gdsrap->send_before_wait > 0){
				gdsrap->send_before_wait--;
			}
			else{
				switch(POKE_NET_GDS_GetStatus()){
				case POKE_NET_GDS_STATUS_INITIALIZED:	//初期化済み
				case POKE_NET_GDS_STATUS_ABORTED:		//!< 中断終了
				case POKE_NET_GDS_STATUS_FINISHED:		//!< 正常終了
				case POKE_NET_GDS_STATUS_ERROR:			//!< エラー終了
					ret = GDSRAP_MAIN_Send(gdsrap);
					if(ret == TRUE){
						OS_TPrintf("data send! req_code = %d\n", gdsrap->send_req);
						gdsrap->send_req = POKE_NET_GDS_REQCODE_LAST;
					}
					break;
				default:	//今は送信出来ない
					OS_TPrintf("send wait ... status = %d\n", POKE_NET_GDS_GetStatus());
					break;
				}
			}
		}
		
		//受信データ確認
		GDSRAP_MAIN_Recv(gdsrap);
		
		//受信サブプロセス実行(データ受信後、制御をフリーに戻す前に実行する処理)
		if(gdsrap->recv_sub_work.recv_sub_proccess != NULL){
			if(gdsrap->recv_sub_work.recv_sub_proccess(gdsrap, &gdsrap->recv_sub_work) == TRUE){
				gdsrap->recv_sub_work.recv_sub_proccess = NULL;
				//受信データのコールバック呼び出し
				if(gdsrap->recv_sub_work.user_callback_func != NULL){
					gdsrap->recv_sub_work.user_callback_func(
						gdsrap->response_callback.callback_work, &gdsrap->error_info);
					gdsrap->recv_sub_work.user_callback_func = NULL;
				}
				MI_CpuClear8(&gdsrap->recv_sub_work, sizeof(GDS_RECV_SUB_PROCCESS_WORK));
				gdsrap->recv_wait_req = POKE_NET_GDS_REQCODE_LAST;
			}
		}
	}
	
	//サブシーケンス実行
	if(GDSRapSeqTbl[gdsrap->proccess_tblno] != NULL){
		int sub_ret;
		
		sub_ret = GDSRapSeqTbl[gdsrap->proccess_tblno][gdsrap->proccess_seqno](gdsrap, &gdsrap->sub_work);
		switch(sub_ret){
		case SUBSEQ_CONTINUE:
			break;
		case SUBSEQ_NEXT:
			MI_CpuClear8(&gdsrap->sub_work, sizeof(GDS_RAP_SUB_PROCCESS_WORK));
			gdsrap->proccess_seqno++;
			MI_CpuClear8(&gdsrap->sub_work, sizeof(GDS_RAP_SUB_PROCCESS_WORK));
			if(GDSRapSeqTbl[gdsrap->proccess_tblno][gdsrap->proccess_seqno] == NULL){
				gdsrap->proccess_seqno = 0;
				gdsrap->proccess_tblno = GDSRAP_PROCESS_REQ_NULL;
			}
			break;
		case SUBSEQ_PROCCESS_CHANGE:
			MI_CpuClear8(&gdsrap->sub_work, sizeof(GDS_RAP_SUB_PROCCESS_WORK));
			gdsrap->proccess_seqno = 0;
			break;
		}
	}
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   送信処理
 *
 * @param   gdsrap		
 *
 * @retval  TRUE:送信成功
 * @retval  FALSE:送信失敗
 */
//--------------------------------------------------------------
static int GDSRAP_MAIN_Send(GDS_RAP_WORK *gdsrap)
{
	int ret = FALSE;
	
	//リクエストにデータが貯まっていれば送信
	switch(gdsrap->send_req){
	case POKE_NET_GDS_REQCODE_DRESSUPSHOT_REGIST:	// ドレスアップショット登録
		ret = POKE_NET_GDS_DressupShotRegist(&gdsrap->send_buf.gt_dress_send, gdsrap->response);
		break;
	case POKE_NET_GDS_REQCODE_DRESSUPSHOT_GET:		// ドレスアップショット取得
		ret = POKE_NET_GDS_DressupShotGet(gdsrap->send_buf.sub_para.dressup.recv_monsno, 
			gdsrap->response);
		break;
	case POKE_NET_GDS_REQCODE_BOXSHOT_REGIST:		// ボックスショット登録
		ret = POKE_NET_GDS_BoxShotRegist(gdsrap->send_buf.sub_para.box.category_no, 
			&gdsrap->send_buf.gt_box_send ,gdsrap->response);
		if(ret == TRUE){
			OS_TPrintf("box shot登録リクエスト完了 category = %d\n", gdsrap->send_buf.sub_para.box.category_no);
		}
		break;
	case POKE_NET_GDS_REQCODE_BOXSHOT_GET:			// ボックスショット取得
		ret = POKE_NET_GDS_BoxShotGet(gdsrap->send_buf.sub_para.box.category_no ,gdsrap->response);
		if(ret == TRUE){
			OS_TPrintf("box shot取得リクエスト完了 category = %d\n", gdsrap->send_buf.sub_para.box.category_no);
		}
		break;
	case POKE_NET_GDS_REQCODE_RANKING_GETTYPE:		// 開催しているランキングのタイプを取得
		ret = POKE_NET_GDS_RankingGetType(gdsrap->response);
		if(ret == TRUE){
			OS_TPrintf("ranking type 取得リクエスト完了\n");
		}
		break;
	case POKE_NET_GDS_REQCODE_RANKING_UPDATE:		// ランキングの更新
		ret = POKE_NET_GDS_RankingUpdate(&gdsrap->send_buf.gt_ranking_mydata_send, 
			gdsrap->response);
		if(ret == TRUE){
			OS_TPrintf("ranking更新リクエスト完了\n");
		}
		break;
	case POKE_NET_GDS_REQCODE_BATTLEDATA_REGIST:	// バトルビデオ登録
		ret = POKE_NET_GDS_BattleDataRegist(gdsrap->send_buf.gt_battle_rec_send_ptr, 
			gdsrap->response);
		if(ret == TRUE){
			OS_TPrintf("バトルビデオ登録リクエスト完了\n");
		}
		break;
	case POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH:	// バトルビデオ検索
		switch(gdsrap->send_req_option){
		case POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE_CONDITION:	//詳細検索
			ret = POKE_NET_GDS_BattleDataSearchCondition(&gdsrap->send_buf.gt_battle_rec_search,
				gdsrap->response);
			break;
		case POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE_RANKING:	//最新30件
			ret = POKE_NET_GDS_BattleDataSearchRanking(
				&gdsrap->send_buf.gt_battle_rec_ranking_search, gdsrap->response);
			break;
		case POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE_EXRANKING:	//お気に入り上位30件
			ret = POKE_NET_GDS_BattleDataSearchExRanking(
				&gdsrap->send_buf.gt_battle_rec_ranking_search, gdsrap->response);
			break;
		}
		if(ret == TRUE){
			OS_TPrintf("バトルビデオ検索リクエスト完了\n");
		}
		break;
	case POKE_NET_GDS_REQCODE_BATTLEDATA_GET:	// バトルビデオ取得
		ret = POKE_NET_GDS_BattleDataGet(gdsrap->send_buf.sub_para.rec.data_number, 
			BATTLE_SERVER_VERSION, gdsrap->response);
		if(ret == TRUE){
			OS_TPrintf("バトルビデオ取得リクエスト完了\n");
		}
		break;
	case POKE_NET_GDS_REQCODE_BATTLEDATA_FAVORITE:	// バトルビデオお気に入り登録
		ret = POKE_NET_GDS_BattleDataFavorite(gdsrap->send_buf.sub_para.rec.data_number, 
			gdsrap->response);
		if(ret == TRUE){
			OS_TPrintf("バトルビデオお気に入り登録リクエスト完了\n");
		}
		break;
	default:
		OS_Printf("-not supported\n");
		GF_ASSERT(0);
		break;
	}
	
	if(ret == TRUE){
		OS_TPrintf("送信成功 send_req = %d\n", gdsrap->send_req);
		gdsrap->recv_wait_req = gdsrap->send_req;
	}
	else{
		//※check 送信失敗時、現状は成功するまで毎フレーム挑戦し続けているが、
		//        それでいいのか、エラーメッセージを出す必要があるのか確認
		OS_TPrintf("送信失敗。send_req = %d\n", gdsrap->send_req);
	}
	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   受信ステータス確認
 *
 * @param   gdsrap		
 *
 * @retval  
 */
//--------------------------------------------------------------
static int GDSRAP_MAIN_Recv(GDS_RAP_WORK *gdsrap)
{
	if(gdsrap->recv_wait_req == POKE_NET_GDS_REQCODE_LAST){
		return TRUE;
	}
	
	// ステータス取得
	gdsrap->stat = POKE_NET_GDS_GetStatus();

	if(	gdsrap->stat != gdsrap->laststat ){
		// ステータスに変化があった
		switch(gdsrap->stat){
		case POKE_NET_GDS_STATUS_INACTIVE:					// 非稼動中
			OS_Printf("-inactive\n");
			break;
		case POKE_NET_GDS_STATUS_INITIALIZED:				// 初期化済み
			OS_Printf("-initialized\n");
			break;
		case POKE_NET_GDS_STATUS_REQUEST:					// リクエスト発行
			OS_Printf("-request\n");
			break;
		case POKE_NET_GDS_STATUS_NETSETTING:				// ネットワーク設定中
			OS_Printf("-netsetting\n");
			break;
		case POKE_NET_GDS_STATUS_CONNECTING:				// 接続中
			OS_Printf("-connecting\n");
			break;
		case POKE_NET_GDS_STATUS_SENDING:					// 送信中
			OS_Printf("-sending\n");
			break;
		case POKE_NET_GDS_STATUS_RECEIVING:					// 受信中
			OS_Printf("-receiving\n");
			break;
		case POKE_NET_GDS_STATUS_ABORTED:					// 中断
			OS_Printf("-aborted\n");
			gdsrap->error_info.type = GDS_ERROR_TYPE_STATUS;
			gdsrap->error_info.req_code = 0;
			gdsrap->error_info.result = POKE_NET_GDS_STATUS_ABORTED;
			gdsrap->error_info.occ = TRUE;
		//	gdsrap->recv_wait_req = POKE_NET_GDS_REQCODE_LAST;
			gdsrap->recv_sub_work.recv_sub_proccess = RecvSubProccess_SystemError;
			break;
		case POKE_NET_GDS_STATUS_FINISHED:					// 正常終了
			OS_Printf("-finished\n");
			gdsrap->recv_sub_work.recv_sub_proccess = RecvSubProccess_Normal;
			// レスポンス処理
			Local_GetResponse(gdsrap);
			break;
		case POKE_NET_GDS_STATUS_ERROR:						// エラー終了
			OS_Printf("-error lasterrorcode:%d\n" ,POKE_NET_GDS_GetLastErrorCode());
			gdsrap->error_info.type = GDS_ERROR_TYPE_LIB;
			gdsrap->error_info.req_code = 0;
			gdsrap->error_info.result = POKE_NET_GDS_GetLastErrorCode();
			gdsrap->error_info.occ = TRUE;
		//	gdsrap->recv_wait_req = POKE_NET_GDS_REQCODE_LAST;
			gdsrap->recv_sub_work.recv_sub_proccess = RecvSubProccess_SystemError;
			break;
		}
		gdsrap->laststat = gdsrap->stat;
	}
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   分割セーブ
 *
 * @param   gdsrap		
 *
 * @retval  SAVE_RESULT_CONTINUE	セーブ処理継続中
 * @retval  SAVE_RESULT_LAST		セーブ処理継続中、最後の一つ前
 * @retval  SAVE_RESULT_OK			セーブ正常終了
 * @retval  SAVE_RESULT_NG			セーブ失敗終了
 */
//--------------------------------------------------------------
static SAVE_RESULT GdsDivSave(GDS_RAP_WORK *gdsrap)
{
	SAVE_RESULT result;
	
	switch(gdsrap->div_save_seq){
	case 0:
		//result = SaveData_Save(sv);
		OS_TPrintf("分割セーブ開始\n");
		SaveData_DivSave_Init(gdsrap->savedata, SVBLK_ID_MAX);
		gdsrap->div_save_seq++;
		return SAVE_RESULT_CONTINUE;
	case 1:
		result = SaveData_DivSave_Main(gdsrap->savedata);
		if(result == SAVE_RESULT_OK || result == SAVE_RESULT_NG){
			gdsrap->div_save_seq = 0;
			OS_TPrintf("分割セーブ完了\n");
		}
		return result;
	}
	return SAVE_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   データ受信後のサブプロセス：特に何も処理しない場合
 *
 * @param   gdsrap		
 *
 * @retval  TRUE:処理完了。　FALSE:処理継続中
 */
//--------------------------------------------------------------
static BOOL RecvSubProccess_Normal(void *work_gdsrap, void *work_recv_sub_work)
{
	GDS_RAP_WORK *gdsrap = work_gdsrap;
	GDS_RECV_SUB_PROCCESS_WORK *recv_sub_work = work_recv_sub_work;

	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   データ受信後のサブプロセス：ビデオ送信済みフラグをリセットしてセーブする
 *
 * @param   gdsrap		
 *
 * @retval  
 */
//--------------------------------------------------------------
static BOOL RecvSubProccess_VideoSendFlagResetSave(void *work_gdsrap, void *work_recv_sub_work)
{
	//送信前のセーブは無くなったし、ローカルでの送信済みチェックも無くなったので何もする必要無し
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   データ受信後のサブプロセス：録画データにデータナンバーをセットしてセーブする
 *
 * @param   gdsrap		
 * @param   recv_sub_work		
 *
 * @retval  
 */
//--------------------------------------------------------------
static BOOL RecvSubProccess_DataNumberSetSave(void *work_gdsrap, void *work_recv_sub_work)
{
	GDS_RAP_WORK *gdsrap = work_gdsrap;
	GDS_RECV_SUB_PROCCESS_WORK *recv_sub_work = work_recv_sub_work;
	POKE_NET_RESPONSE *res;
	POKE_NET_GDS_RESPONSE_BATTLEDATA_REGIST *param;
	SAVE_RESULT ret;
	
	res = POKE_NET_GDS_GetResponse();
	param = (POKE_NET_GDS_RESPONSE_BATTLEDATA_REGIST *)(res->Param);
	ret = BattleRec_GDS_MySendData_DataNumberSetSave(
		gdsrap->savedata, param->Code, 
		&recv_sub_work->recv_save_seq0, &recv_sub_work->recv_save_seq1);
	if(ret == SAVE_RESULT_OK || ret == SAVE_RESULT_NG){
		OS_TPrintf("外部セーブ完了\n");
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   データ受信後のサブプロセス：ドレス送信済みフラグをリセットしてセーブする
 *
 * @param   gdsrap		
 *
 * @retval  
 */
//--------------------------------------------------------------
static BOOL RecvSubProccess_DressSendFlagResetSave(void *work_gdsrap, void *work_recv_sub_work)
{
	//送信前のセーブは無くなったし、ローカルでの送信済みチェックも無くなったので何もする必要無し
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   データ受信後のサブプロセス：ボックス送信済みフラグをリセットしてセーブする
 *
 * @param   gdsrap		
 *
 * @retval  
 */
//--------------------------------------------------------------
static BOOL RecvSubProccess_BoxSendFlagResetSave(void *work_gdsrap, void *work_recv_sub_work)
{
	//送信前のセーブは無くなったし、ローカルでの送信済みチェックも無くなったので何もする必要無し
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   データ受信後のサブプロセス：システムエラーが発生した場合のケア処理
 *
 * @param   gdsrap		
 *
 * @retval  
 */
//--------------------------------------------------------------
static BOOL RecvSubProccess_SystemError(void *work_gdsrap, void *work_recv_sub_work)
{
	GDS_RAP_WORK *gdsrap = work_gdsrap;
	GDS_RECV_SUB_PROCCESS_WORK *recv_sub_work = work_recv_sub_work;
	
	OS_TPrintf("システムエラーが発生した為、ケア処理中 laststat=%d, recv_wait_req=%d\n", gdsrap->laststat, gdsrap->recv_wait_req);
	
	//送信前のセーブは無くなったし、ローカルでの送信済みチェックも無くなったので
	//ケア処理は無くなった
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   受信データ解釈
 *
 * @param   gdsrap		
 *
 * @retval  TRUE:正常終了。　FALSE:エラー発生(エラー情報はerror_info参照)
 */
//--------------------------------------------------------------
static int Local_GetResponse(GDS_RAP_WORK *gdsrap)
{
	POKE_NET_RESPONSE *res;
	int size;
	int result = TRUE;
	
	res = POKE_NET_GDS_GetResponse();
	size = POKE_NET_GDS_GetResponseSize();
	
	GF_ASSERT(gdsrap->recv_wait_req == res->ReqCode);
	
	gdsrap->recv_sub_work.user_callback_func = NULL;
	
	// == ここでそれぞれのレスポンスに対する処理を行う ==
	switch(res->ReqCode){
	case POKE_NET_GDS_REQCODE_DRESSUPSHOT_REGIST:		// ドレスアップショット登録
		OS_TPrintf("受信：ドレスアップショットアップロード\n");
		result = GDS_RAP_RESPONSE_DressupShot_Upload(gdsrap, res);
		gdsrap->recv_sub_work.user_callback_func = gdsrap->response_callback.func_dressupshot_regist;
		if(result == FALSE){	//送信済みフラグを戻して再セーブ
			gdsrap->recv_sub_work.recv_sub_proccess = RecvSubProccess_DressSendFlagResetSave;
		}
		break;
	case POKE_NET_GDS_REQCODE_DRESSUPSHOT_GET:			// ドレスアップショット取得
		OS_TPrintf("受信：ドレスアップショット取得\n");
		result = GDS_RAP_RESPONSE_DressupShot_Download(gdsrap, res);
		gdsrap->recv_sub_work.user_callback_func = gdsrap->response_callback.func_dressupshot_get;
		break;
	case POKE_NET_GDS_REQCODE_BOXSHOT_REGIST:			// ボックスショット登録
		OS_TPrintf("受信：ボックスショット登録\n");
		result = GDS_RAP_RESPONSE_Boxshot_Upload(gdsrap, res);
		gdsrap->recv_sub_work.user_callback_func = gdsrap->response_callback.func_boxshot_regist;
		if(result == FALSE){	//送信済みフラグを戻して再セーブ
			gdsrap->recv_sub_work.recv_sub_proccess = RecvSubProccess_BoxSendFlagResetSave;
		}
		break;
	case POKE_NET_GDS_REQCODE_BOXSHOT_GET:				// ボックスショット取得
		OS_TPrintf("受信：ボックスショット取得\n");
		result = GDS_RAP_RESPONSE_Boxshot_Download(gdsrap, res);
		gdsrap->recv_sub_work.user_callback_func = gdsrap->response_callback.func_boxshot_get;
		break;
	case POKE_NET_GDS_REQCODE_RANKING_GETTYPE:			// ランキングタイプ取得
		OS_TPrintf("受信：ランキングタイプ取得\n");
		result = GDS_RAP_RESPONSE_RankingType_Download(gdsrap, res);
		gdsrap->recv_sub_work.user_callback_func = gdsrap->response_callback.func_ranking_type_get;
		break;
	case POKE_NET_GDS_REQCODE_RANKING_UPDATE:			// ランキング更新
		OS_TPrintf("受信：ランキング更新取得\n");
		result = GDS_RAP_RESPONSE_RankingUpdate_Download(gdsrap, res);
		gdsrap->recv_sub_work.user_callback_func = gdsrap->response_callback.func_ranking_update_get;
		break;
	case POKE_NET_GDS_REQCODE_BATTLEDATA_REGIST:		// バトルビデオ登録
		OS_TPrintf("受信：バトルビデオ登録\n");
		result = GDS_RAP_RESPONSE_BattleVideo_Upload(gdsrap, res);
		gdsrap->recv_sub_work.user_callback_func = gdsrap->response_callback.func_battle_video_regist;
		if(result == FALSE){	//送信済みフラグを戻して再セーブ
			gdsrap->recv_sub_work.recv_sub_proccess = RecvSubProccess_VideoSendFlagResetSave;
		}
		else{
			//登録コードを自分のデータに入れて、外部セーブ実行
			gdsrap->recv_sub_work.recv_sub_proccess = RecvSubProccess_DataNumberSetSave;
		}
		break;
	case POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH:		// バトルビデオ検索
		OS_TPrintf("受信：バトルビデオ検索\n");
		result = GDS_RAP_RESPONSE_BattleVideo_Search_Download(gdsrap, res);
		gdsrap->recv_sub_work.user_callback_func = gdsrap->response_callback.func_battle_video_search_get;
		break;
	case POKE_NET_GDS_REQCODE_BATTLEDATA_GET:		// バトルビデオ取得
		OS_TPrintf("受信：バトルビデオ取得\n");
		result = GDS_RAP_RESPONSE_BattleVideo_Data_Download(gdsrap, res);
		gdsrap->recv_sub_work.user_callback_func = gdsrap->response_callback.func_battle_video_data_get;
		break;
	case POKE_NET_GDS_REQCODE_BATTLEDATA_FAVORITE:		// バトルビデオお気に入り登録
		OS_TPrintf("受信：バトルビデオお気に入り登録\n");
		result = GDS_RAP_RESPONSE_BattleVideo_Favorite_Upload(gdsrap, res);
		gdsrap->recv_sub_work.user_callback_func = gdsrap->response_callback.func_battle_video_favorite_regist;
		break;
		
	case POKE_NET_GDS_REQCODE_DEBUG_MESSAGE:			// デバッグ用リクエスト
	default:
		OS_TPrintf("受信：未定義なもの\n");
		break;
	}
	
	//エラー情報をセット
	if(result == FALSE){
		gdsrap->error_info.type = GDS_ERROR_TYPE_APP;
		gdsrap->error_info.req_code = res->ReqCode;
		gdsrap->error_info.result = res->Result;
		gdsrap->error_info.occ = TRUE;
	}
	else{
		gdsrap->error_info.occ = FALSE;
	}
	
	return result;
}

//--------------------------------------------------------------
/**
 * @brief   エラー情報を取得する
 *
 * @param   gdsrap			
 * @param   error_info		エラー情報代入先
 *
 * @retval  TRUE:エラーが発生している
 * @retval  FALSE:エラー無し
 */
//--------------------------------------------------------------
BOOL GDSRAP_ErrorInfoGet(GDS_RAP_WORK *gdsrap, GDS_RAP_ERROR_INFO **error_info)
{
	*error_info = &gdsrap->error_info;
	return gdsrap->error_info.occ;
}

//--------------------------------------------------------------
/**
 * @brief   エラー情報をクリアする
 *
 * @param   gdsrap		
 */
//--------------------------------------------------------------
void GDSRAP_ErrorInfoClear(GDS_RAP_WORK *gdsrap)
{
	MI_CpuClear8(&gdsrap->error_info, sizeof(GDS_RAP_ERROR_INFO));
}

//------------------------------------------------------------------
/**
 * @brief   WIFIエラーメッセージ：ワイド表示コールバック呼び出し
 *
 * @param   wk		
 * @param   msgno		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void _systemMessagePrint(GDS_RAP_WORK *gdsrap, STRBUF *str_buf)
{
	gdsrap->callback_error_msg_wide(gdsrap->callback_work, str_buf);
}

//------------------------------------------------------------------
/**
 * @brief   Wifiコネクションエラーの表示
 *
 * @param   gdsrap		
 * @param   type	
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void errorDisp(GDS_RAP_WORK *gdsrap, int type, int code)
{
    int msgno;
	STRBUF *tempstr;

    if(type != -1){
        msgno = dwc_error_0001 + type;
    }
    else{
        msgno = dwc_error_0012;
    }
    
    WORDSET_RegisterNumber(gdsrap->wordset, 0, code,
                           5, NUMBER_DISPTYPE_ZERO, NUMBER_CODETYPE_DEFAULT);

	tempstr = MSGMAN_AllocString( gdsrap->msgman_wifisys, msgno );
	WORDSET_ExpandStr( gdsrap->wordset, gdsrap->ErrorString, tempstr );
	STRBUF_Delete(tempstr);

    _systemMessagePrint(gdsrap, gdsrap->ErrorString);
}

//--------------------------------------------------------------
/**
 * @brief   GDSRAPが何らかの動作を実行、又は受付、待ち中、などをしているか確認する
 *
 * @param   gdsrap		
 *
 * @retval  TRUE:何の処理も実行、受付していない
 * @retval  FALSE:何らかの処理を実行、受付、待っている
 */
//--------------------------------------------------------------
BOOL GDSRAP_MoveStatusAllCheck(GDS_RAP_WORK *gdsrap)
{
	if(GDSRAP_ProccessCheck(gdsrap) == GDSRAP_PROCESS_REQ_NULL
			&& gdsrap->send_req == POKE_NET_GDS_REQCODE_LAST
			&& gdsrap->recv_wait_req == POKE_NET_GDS_REQCODE_LAST){
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   GDSRAPが実行中のプロセス番号を取得する
 *
 * @param   gdsrap		
 *
 * @retval  GDSRAP_PROCESS_REQ_???(実行中の処理がない場合はGDSRAP_PROCESS_REQ_NULL)
 */
//--------------------------------------------------------------
static int GDSRAP_ProccessCheck(GDS_RAP_WORK *gdsrap)
{
	return gdsrap->proccess_tblno;
}

//--------------------------------------------------------------
/**
 * @brief   GDSRAPに対して処理のリクエストを行う
 *
 * @param   gdsrap				
 * @param   proccess_req		GDSRAP_PROCESS_REQ_???
 *
 * @retval  TRUE:リクエスト受付成功
 * @retval  FALSE:リクエスト受付失敗
 */
//--------------------------------------------------------------
BOOL GDSRAP_ProccessReq(GDS_RAP_WORK *gdsrap, GDSRAP_PROCESS_REQ proccess_req)
{
	gdsrap->proccess_tblno = proccess_req;
	OS_TPrintf("サブプロセスリクエスト：tblno = %d\n", proccess_req);
	
	return TRUE;
}


//==============================================================================
//	デバッグ用ツール
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   デバッグツール：セーブ実行フラグをOFFする
 *
 * @param   gdsrap		
 */
//--------------------------------------------------------------
void DEBUG_GDSRAP_SaveFlagReset(GDS_RAP_WORK *gdsrap)
{
#ifdef PM_DEBUG
//	gdsrap->send_before_div_save = 0;
#endif
}

//==============================================================================
//
//	※check　暫定　GDSライブラリがOS_AllocFromMain関数を使用しているので
//				一時的にヒープを作成する
//
//==============================================================================
#define ROUND(n, a)		(((u32)(n)+(a)-1) & ~((a)-1))
static OSHeapHandle sHandle;

static void * LIB_Heap_Init(int heap_id)
{
	void*    arenaLo;
	void*    arenaHi;
	void*	alloc_ptr;
	
	int heap_size = 0x2000;
	
	arenaLo = sys_AllocMemory(heap_id, heap_size);
	alloc_ptr = arenaLo;
	arenaHi = (void*)((u32)arenaLo + heap_size);
	
    arenaLo = OS_InitAlloc(OS_ARENA_MAIN, arenaLo, arenaHi, 1);
    OS_SetArenaLo(OS_ARENA_MAIN, arenaLo);

    // [nakata] ポインタを32bit境界にアラインする
    arenaLo = (void*)ROUND(arenaLo, 32);
    arenaHi = (void*)ROUND(arenaHi, 32);

    // [nakata] ヒープ領域の作成
    sHandle = OS_CreateHeap(OS_ARENA_MAIN, arenaLo, arenaHi);
    OS_SetCurrentHeap(OS_ARENA_MAIN, sHandle );

    // From here on out, OS_Alloc and OS_Free behave like malloc and free respectively
//    OS_SetArenaLo(OS_ARENA_MAIN, arenaHi);

	return alloc_ptr;
}

static void LIB_Heap_Exit(void)
{
	OS_ClearAlloc(OS_ARENA_MAIN);
}

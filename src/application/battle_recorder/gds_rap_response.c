//==============================================================================
/**
 * @file	gds_rap_response.c
 * @brief	gds_rap.cから受信データの判定や取得部分を独立させたもの
 * @author	matsuda
 * @date	2008.01.13(日)
 */
//==============================================================================
#include "common.h"
#include <dwc.h>
#include "wifi/dwc_rap.h"
#include "savedata/savedata.h"
#include "gds_battle_rec.h"
#include "gds_ranking.h"
#include "gds_boxshot.h"
#include "gds_dressup.h"

#include "savedata/imageclip_data.h"
#include "poketool/boxdata.h"
#include "poketool/monsno.h"
#include "gflib/strbuf_family.h"
#include "savedata/gds_profile.h"

#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "system/msgdata_util.h"
#include "system/wordset.h"
#include "msgdata/msg.naix"
#include "system/fontproc.h"
#include "gflib/strbuf_family.h"

#include "communication/comm_system.h"
#include "communication/comm_state.h"
#include "communication/comm_def.h"
#include "communication/wm_icon.h"

#include "gds_battle_rec.h"
#include "gds_ranking.h"
#include "gds_boxshot.h"
#include "gds_dressup.h"

#include "gds_rap.h"
#include "gds_rap_response.h"
#include "gds_data_conv.h"

#include "msgdata/msg_wifi_system.h"
#include "savedata/battle_rec.h"



//--------------------------------------------------------------
/**
 * @brief   レスポンスデータ解釈：ドレスアップショット アップロード
 *
 * @param   gdsrap		
 * @param   res		
 *
 * @retval  TRUE:成功
 * @retval  FALSE:何らかのエラーが発生(内容はres->Result参照)
 */
//--------------------------------------------------------------
int GDS_RAP_RESPONSE_DressupShot_Upload(GDS_RAP_WORK *gdsrap, POKE_NET_RESPONSE *res)
{
	POKE_NET_GDS_RESPONSE_DRESSUPSHOT_REGIST *param;
	int ret = FALSE;
	
	switch(res->Result){
	//!< 登録成功
	case POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_REGIST_SUCCESS:
		param = (POKE_NET_GDS_RESPONSE_DRESSUPSHOT_REGIST *)(res->Param);
		OS_TPrintf("ドレスアップショットアップロード成功。登録コード = %d\n", param->Code);
		ret = TRUE;
		break;
	//!< ユーザー認証エラー
	case POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_REGIST_ERROR_AUTH:
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("ドレスアップショット登録エラー！:ユーザー認証エラー\n");
		break;
	//!< すでに登録されている
	case POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_REGIST_ERROR_ALREADY:
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("ドレスアップショット登録エラー！:既に登録されている\n");
		break;
	//!< 不正なデータ
	case POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_REGIST_ERROR_ILLEGAL:
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("ドレスアップショット登録エラー！:不正データ\n");
		break;
	//!< 不正なユーザープロフィール
	case POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_REGIST_ERROR_ILLEGALPROFILE:
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("ドレスアップショット登録エラー！:不正プロフィール\n");
		break;
	//!< その他エラー
	case POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_REGIST_ERROR_UNKNOWN:
	default:
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("ドレスアップショット登録エラー！:その他のエラー\n");
		break;
	}
	
	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   レスポンスデータ解釈：ドレスアップショット ダウンロード
 *
 * @param   gdsrap		
 * @param   res		
 *
 * @retval  TRUE:成功
 * @retval  FALSE:何らかのエラーが発生(内容はres->Result参照)
 */
//--------------------------------------------------------------
int GDS_RAP_RESPONSE_DressupShot_Download(GDS_RAP_WORK *gdsrap, POKE_NET_RESPONSE *res)
{
	POKE_NET_GDS_RESPONSE_DRESSUPSHOT_GET *param;
	int ret = FALSE;
	
	switch(res->Result){
	//!< 取得成功
	case POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_GET_SUCCESS:
		param = (POKE_NET_GDS_RESPONSE_DRESSUPSHOT_GET *)(res->Param);
		OS_TPrintf("ドレスアップショット受信成功。ヒット件数 = %d\n", param->HitCount);
		ret = TRUE;
		break;
	//!< ユーザー認証エラー
	case POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_GET_ERROR_AUTH:
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("ドレスアップショット受信エラー！:ユーザー認証エラー\n");
		break;
	//!< ポケモン番号エラー
	case POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_GET_ERROR_ILLEGALPOKEMON:
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("ドレスアップショット受信エラー！:ポケモン番号エラー\n");
		break;
	//!< その他エラー
	case POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_GET_ERROR_UNKNOWN:
	default:
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("ドレスアップショット受信エラー！:その他のエラー\n");
		break;
	}
	
	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   受信バッファからデータが入っている部分のポインタを取得：ドレスアップショット
 *
 * @param   gdsrap		
 * @param   dress_array		ポインタをセットする配列データ
 * @param   array_max		dress_arrayの配列要素数
 *
 * @retval  受信件数
 *
 * dress_array[0] ～ dress_array[x] まで受信データへのポインタがセットされます
 */
//--------------------------------------------------------------
int GDS_RAP_RESPONSE_DressupShot_Download_RecvPtr_Set(GDS_RAP_WORK *gdsrap, GT_GDS_DRESS_RECV **dress_array, int array_max)
{
	POKE_NET_RESPONSE *res;
	POKE_NET_GDS_RESPONSE_DRESSUPSHOT_GET *param;
	POKE_NET_GDS_RESPONSE_DRESSUPSHOT_RECVDATA *dress_recv;
	int i, hit_count;
	
	res = POKE_NET_GDS_GetResponse();
	param = (POKE_NET_GDS_RESPONSE_DRESSUPSHOT_GET *)(res->Param);
	
	hit_count = param->HitCount;
	if(hit_count > array_max){
		hit_count = array_max;
	//	GF_ASSERT(0);	//用意しているバッファよりも受信件数が多い
	}

	dress_recv = (POKE_NET_GDS_RESPONSE_DRESSUPSHOT_RECVDATA *)(param->Data);
	for(i = 0; i < hit_count; i++){
		dress_array[i] = &dress_recv[i].Data;
	}
	for( ; i < array_max; i++){
		dress_array[i] = NULL;
	}

	return hit_count;
}

//--------------------------------------------------------------
/**
 * @brief   レスポンスデータ解釈：ボックスショット アップロード
 *
 * @param   gdsrap		
 * @param   res		
 *
 * @retval  TRUE:成功
 * @retval  FALSE:何らかのエラーが発生(内容はres->Result参照)
 */
//--------------------------------------------------------------
int GDS_RAP_RESPONSE_Boxshot_Upload(GDS_RAP_WORK *gdsrap, POKE_NET_RESPONSE *res)
{
	POKE_NET_GDS_RESPONSE_BOXSHOT_REGIST *param;
	int ret = FALSE;
	
	switch(res->Result){
	//!< 登録成功
	case POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_REGIST_SUCCESS:
		param = (POKE_NET_GDS_RESPONSE_BOXSHOT_REGIST *)(res->Param);
		OS_TPrintf("ボックスショットアップロード成功。登録コード = %d\n", param->Code);
		ret = TRUE;
		break;
	//!< ユーザー認証エラー
	case POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_REGIST_ERROR_AUTH:
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("ボックスショット登録エラー！:ユーザー認証エラー\n");
		break;
	//!< すでに登録されている
	case POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_REGIST_ERROR_ALREADY:
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("ボックスショット登録エラー！:既に登録されている\n");
		break;
	//!< 不正なデータ
	case POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_REGIST_ERROR_ILLEGAL:
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("ボックスショット登録エラー！:不正なデータ\n");
		break;
	//!< 不正なユーザープロフィール
	case POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_REGIST_ERROR_ILLEGALPROFILE:
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("ボックスショット登録エラー！:不正プロフィール\n");
		break;
	//!< 不正なグループ番号
	case POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_REGIST_ERROR_ILLEGALGROUP:
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("ボックスショット登録エラー！:不正グループ番号\n");
		break;
	//!< その他エラー
	case POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_REGIST_ERROR_UNKNOWN:
	default:
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("ボックスショット登録エラー！:その他エラー\n");
		break;
	}
	
	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   レスポンスデータ解釈：ボックスショット ダウンロード
 *
 * @param   gdsrap		
 * @param   res		
 *
 * @retval  TRUE:成功
 * @retval  FALSE:何らかのエラーが発生(内容はres->Result参照)
 */
//--------------------------------------------------------------
int GDS_RAP_RESPONSE_Boxshot_Download(GDS_RAP_WORK *gdsrap, POKE_NET_RESPONSE *res)
{
	POKE_NET_GDS_RESPONSE_BOXSHOT_GET *param;
	int ret = FALSE;
	
	switch(res->Result){
	case POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_GET_SUCCESS:				//!< 取得成功
		param = (POKE_NET_GDS_RESPONSE_BOXSHOT_GET *)(res->Param);
		OS_TPrintf("ボックスショット受信成功。ヒット件数 = %d\n", param->HitCount);
		ret = TRUE;
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_GET_ERROR_AUTH:			//!< ユーザー認証エラー
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("ボックスショット受信エラー！:ユーザー認証エラー\n");
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_GET_ERROR_ILLEGALGROUP:	//!< グループ番号エラー
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("ボックスショット受信エラー！グループ番号エラー\n");
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_GET_ERROR_UNKNOWN:		//!< その他エラー
	default:
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("ボックスショット受信エラー！:その他エラー\n");
		break;
	}
	
	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   受信バッファからデータが入っている部分のポインタを取得：ボックスショット
 *
 * @param   gdsrap		
 * @param   box_array		ポインタをセットする配列データ
 * @param   array_max		dress_arrayの配列要素数
 *
 * @retval  受信件数
 *
 * box_array[0] ～ box_array[x] まで受信データへのポインタがセットされます
 */
//--------------------------------------------------------------
int GDS_RAP_RESPONSE_Boxshot_Download_RecvPtr_Set(GDS_RAP_WORK *gdsrap, GT_BOX_SHOT_RECV **box_array, int array_max)
{
	POKE_NET_RESPONSE *res;
	POKE_NET_GDS_RESPONSE_BOXSHOT_GET *param;
	POKE_NET_GDS_RESPONSE_BOXSHOT_RECVDATA *box_recv;
	int i, hit_count;
	
	res = POKE_NET_GDS_GetResponse();
	param = (POKE_NET_GDS_RESPONSE_BOXSHOT_GET *)(res->Param);
	
	hit_count = param->HitCount;
	if(hit_count > array_max){
		hit_count = array_max;
		GF_ASSERT(0);	//用意しているバッファよりも受信件数が多い
	}

	box_recv = (POKE_NET_GDS_RESPONSE_BOXSHOT_RECVDATA *)(param->Data);
	for(i = 0; i < hit_count; i++){
		box_array[i] = &box_recv[i].Data;
	}
	for( ; i < array_max; i++){
		box_array[i] = NULL;
	}
	
	return hit_count;
}

//--------------------------------------------------------------
/**
 * @brief   レスポンスデータ解釈：ランキングタイプ ダウンロード
 *
 * @param   gdsrap		
 * @param   res		
 *
 * @retval  TRUE:成功
 * @retval  FALSE:何らかのエラーが発生(内容はres->Result参照)
 */
//--------------------------------------------------------------
int GDS_RAP_RESPONSE_RankingType_Download(GDS_RAP_WORK *gdsrap, POKE_NET_RESPONSE *res)
{
	POKE_NET_GDS_RESPONSE_RANKING_GETTYPE *param;
	int ret = FALSE;
	
	switch(res->Result){
	case POKE_NET_GDS_RESPONSE_RESULT_RANKING_GETTYPE_SUCCESS:				//!< 取得成功
		param = (POKE_NET_GDS_RESPONSE_RANKING_GETTYPE *)(res->Param);
		OS_TPrintf("ランキングタイプ受信成功。\n");
		ret = TRUE;
	#if 0
		{
			int i;
			GT_RANKING_TYPE_RECV *recv = &param->Data;
			for(i = 0; i < GT_RANKING_WEEK_NUM; i++){
				OS_TPrintf("開催中のタイプ その%d = %d\n", i, recv->ranking_type[i]);
			}
		}
	#endif
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_RANKING_GETTYPE_ERROR_AUTH:			//!< ユーザー認証エラー
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("ランキングタイプ受信エラー！:ユーザー認証エラー\n");
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_RANKING_GETTYPE_ERROR_UNKNOWN:		//!< その他エラー
	default:
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("ランキングタイプ受信エラー！:その他のエラー\n");
		break;
	}
	
	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   受信バッファからデータが入っている部分のポインタを取得：開催中のランキングタイプ
 *
 * @param   gdsrap		
 * @param   ranking_type_recv		受信データへのポインタ代入先
 */
//--------------------------------------------------------------
void GDS_RAP_RESPONSE_RankingType_Download_RecvPtr_Set(GDS_RAP_WORK *gdsrap, GT_RANKING_TYPE_RECV **ranking_type_recv)
{
	POKE_NET_RESPONSE *res;
	POKE_NET_GDS_RESPONSE_RANKING_GETTYPE *param;
	
	res = POKE_NET_GDS_GetResponse();
	param = (POKE_NET_GDS_RESPONSE_RANKING_GETTYPE *)(res->Param);
	
	*ranking_type_recv = &param->Data;
}

//--------------------------------------------------------------
/**
 * @brief   レスポンスデータ解釈：ランキング更新 ダウンロード
 *
 * @param   gdsrap		
 * @param   res		
 *
 * @retval  TRUE:成功
 * @retval  FALSE:何らかのエラーが発生(内容はres->Result参照)
 */
//--------------------------------------------------------------
int GDS_RAP_RESPONSE_RankingUpdate_Download(GDS_RAP_WORK *gdsrap, POKE_NET_RESPONSE *res)
{
	POKE_NET_GDS_RESPONSE_RANKING_UPDATE *param;
	int ret = FALSE;
	
	switch(res->Result){
	case POKE_NET_GDS_RESPONSE_RESULT_RANKING_UPDATE_SUCCESS:			//!< 更新成功
		param = (POKE_NET_GDS_RESPONSE_RANKING_UPDATE *)(res->Param);
		OS_TPrintf("ランキング更新受信成功\n");
		ret = TRUE;
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_RANKING_UPDATE_ERROR_AUTH:		//!< ユーザー認証エラー
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("ランキング更新受信エラー！:ユーザー認証エラー\n");
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_RANKING_UPDATE_ERROR_ILLEGALTYPE:	//!< ランキングタイプエラー
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("ランキング更新受信エラー！:ランキングタイプエラー\n");
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_RANKING_UPDATE_ERROR_ILLEGALDATA:	//!< 不正なデータエラー
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("ランキング更新受信エラー！:不正なデータ\n");
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_RANKING_UPDATE_ERROR_ILLEGALPROFILE:	//!< 不正なプロフィール
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("ランキング更新受信エラー！:不正なプロフィール\n");
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_RANKING_UPDATE_ERROR_UNKNOWN:			//!< その他エラー
	default:
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("ランキング更新受信エラー！:その他のエラー\n");
		break;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   受信バッファからデータが入っている部分のポインタを取得：ランキング更新(先週と今週の結果)
 *
 * @param   gdsrap		
 * @param   last_week		先週の結果へのポインタ代入先
 * @param   this_week		今週の結果へのポインタ代入先
 */
//--------------------------------------------------------------
void GDS_RAP_RESPONSE_RankingUpdate_Download_RecvPtr_Set(GDS_RAP_WORK *gdsrap, GT_LAST_WEEK_RANKING_ALL_RECV **last_week, GT_THIS_WEEK_RANKING_DATA_ALL_RECV **this_week)
{
	POKE_NET_RESPONSE *res;
	POKE_NET_GDS_RESPONSE_RANKING_UPDATE *param;
	
	res = POKE_NET_GDS_GetResponse();
	param = (POKE_NET_GDS_RESPONSE_RANKING_UPDATE *)(res->Param);
	
	*last_week = &param->LastWeek;
	*this_week = &param->ThisWeek;
}

//--------------------------------------------------------------
/**
 * @brief   レスポンスデータ解釈：バトルビデオ登録
 *
 * @param   gdsrap		
 * @param   res		
 *
 * @retval  TRUE:成功
 * @retval  FALSE:何らかのエラーが発生(内容はres->Result参照)
 */
//--------------------------------------------------------------
int GDS_RAP_RESPONSE_BattleVideo_Upload(GDS_RAP_WORK *gdsrap, POKE_NET_RESPONSE *res)
{
	POKE_NET_GDS_RESPONSE_BATTLEDATA_REGIST *param;
	int ret = FALSE;
	
	switch(res->Result){
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_SUCCESS:		//!< 登録成功
		param = (POKE_NET_GDS_RESPONSE_BATTLEDATA_REGIST *)(res->Param);
		OS_TPrintf("バトルビデオ登録受信成功：登録コード%d\n", param->Code);
		ret = TRUE;
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_AUTH:		//!< ユーザー認証エラー
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("バトルビデオ登録受信エラー！:ユーザー認証エラー\n");
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_ALREADY:	//!< すでに登録されている
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("バトルビデオ登録受信エラー！:既に登録されている\n");
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_ILLEGAL:	//!< 不正なデータ
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("バトルビデオ登録受信エラー！:不正データ\n");
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_ILLEGALPROFILE:	//!< 不正なユーザープロフィール
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("バトルビデオ登録受信エラー！:不正なユーザー\n");
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_UNKNOWN:	//!< その他エラー
	default:
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("バトルビデオ登録受信エラー！:その他のエラー\n");
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_ILLEGALPOKEMON_MINE: //自分のポケモンでエラー
		OS_TPrintf("バトルビデオ登録受信エラー！:自分のポケモンが不正\n");
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_ILLEGALPOKEMON_OTHER:	//相手のポケモンでエラ
		OS_TPrintf("バトルビデオ登録受信エラー！:その他のエラー\n");
		break;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   受信バッファからデータ取得：バトルビデオ登録
 *
 * @param   gdsrap		
 * 
 * @retval	登録コード
 */
//--------------------------------------------------------------
u64 GDS_RAP_RESPONSE_BattleVideo_Upload_DataGet(GDS_RAP_WORK *gdsrap)
{
	POKE_NET_RESPONSE *res;
	POKE_NET_GDS_RESPONSE_BATTLEDATA_REGIST *param;
	
	res = POKE_NET_GDS_GetResponse();
	param = (POKE_NET_GDS_RESPONSE_BATTLEDATA_REGIST *)(res->Param);
	return param->Code;
}

//--------------------------------------------------------------
/**
 * @brief   レスポンスデータ解釈：バトルビデオ検索
 *
 * @param   gdsrap		
 * @param   res		
 *
 * @retval  TRUE:成功
 * @retval  FALSE:何らかのエラーが発生(内容はres->Result参照)
 */
//--------------------------------------------------------------
int GDS_RAP_RESPONSE_BattleVideo_Search_Download(GDS_RAP_WORK *gdsrap, POKE_NET_RESPONSE *res)
{
	POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH *param;
	int ret = FALSE;
	
	switch(res->Result){
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH_SUCCESS:		//!< 取得成功
		param = (POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH *)(res->Param);
		OS_TPrintf("バトルビデオ検索受信成功：ヒット件数=%d\n", param->HitCount);
		ret = TRUE;
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH_ERROR_AUTH:		//!< ユーザー認証エラー
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("バトルビデオ検索受信エラー！:ユーザー認証エラー\n");
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH_ERROR_ILLEGALPARAM:	//!< 検索パラメータエラー
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("バトルビデオ検索受信エラー！:検索パラメータエラー\n");
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH_ERROR_UNKNOWN:	//!< その他エラー
	default:
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("バトルビデオ検索受信エラー！:その他のエラー\n");
		break;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   受信バッファからデータが入っている部分のポインタを取得：バトルビデオ検索
 *
 * @param   gdsrap		
 * @param   outline_array	ポインタをセットする配列データ
 * @param   array_max		outline_arrayの配列要素数
 *
 * @retval  受信件数
 *
 * outline_array[0] ～ outline_array[x] まで受信データへのポインタがセットされます
 */
//--------------------------------------------------------------
int GDS_RAP_RESPONSE_BattleVideoSearch_Download_RecvPtr_Set(GDS_RAP_WORK *gdsrap, GT_BATTLE_REC_OUTLINE_RECV **outline_array, int array_max)
{
	POKE_NET_RESPONSE *res;
	POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH *param;
	POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH_RECVDATA *outline_recv;
	int i, hit_count;
	
	res = POKE_NET_GDS_GetResponse();
	param = (POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH *)(res->Param);
	
	hit_count = param->HitCount;
	if(hit_count > array_max){
		hit_count = array_max;
		GF_ASSERT(0);	//用意しているバッファよりも受信件数が多い
	}

	outline_recv = (POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH_RECVDATA *)(param->Data);
	for(i = 0; i < hit_count; i++){
		outline_array[i] = &outline_recv[i].Data;
		if(outline_recv[i].Code != outline_array[i]->head.data_number){
			//ヘッダとデータナンバーが違う場合はヘッダにナンバーを入れられてないのでこっちで入れる
			outline_array[i]->head.data_number = outline_recv[i].Code;
			//CRC作成
			outline_array[i]->head.crc.crc16ccitt_hash = SaveData_CalcCRC(gdsrap->savedata, 
				&outline_array[i]->head, 
				sizeof(GT_BATTLE_REC_HEADER) - GT_GDS_CRC_SIZE - sizeof(outline_recv[i].Code));
		}
	}
	for( ; i < array_max; i++){
		outline_array[i] = NULL;
	}
	
	return hit_count;
}

//--------------------------------------------------------------
/**
 * @brief   受信バッファからデータをコピーします：バトルビデオ検索
 *
 * @param   gdsrap		
 * @param   outline_array	ポインタをセットする配列データ
 * @param   array_max		outline_arrayの配列要素数
 *
 * @retval  受信件数
 *
 * outline_array[0] ～ outline_array[x] まで受信データへのポインタがセットされます
 */
//--------------------------------------------------------------
int GDS_RAP_RESPONSE_BattleVideoSearch_Download_DataCopy(GDS_RAP_WORK *gdsrap, GT_BATTLE_REC_OUTLINE_RECV *outline_array, int array_max)
{
	POKE_NET_RESPONSE *res;
	POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH *param;
	POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH_RECVDATA *outline_recv;
	int i, hit_count;
	
	res = POKE_NET_GDS_GetResponse();
	param = (POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH *)(res->Param);
	
	hit_count = param->HitCount;
	if(hit_count > array_max){
		hit_count = array_max;
		GF_ASSERT(0);	//用意しているバッファよりも受信件数が多い
	}

	MI_CpuClear8(outline_array, sizeof(GT_BATTLE_REC_OUTLINE_RECV) * array_max);
	outline_recv = (POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH_RECVDATA *)(param->Data);
	for(i = 0; i < hit_count; i++){
		if(outline_recv[i].Code != outline_recv[i].Data.head.data_number){
			//ヘッダとデータナンバーが違う場合はヘッダにナンバーを入れられてないのでこっちで入れる
			outline_recv[i].Data.head.data_number = outline_recv[i].Code;
			//CRC作成
			outline_recv[i].Data.head.crc.crc16ccitt_hash = SaveData_CalcCRC(gdsrap->savedata, 
				&outline_recv[i].Data.head, 
				sizeof(GT_BATTLE_REC_HEADER) - GT_GDS_CRC_SIZE - sizeof(outline_recv[i].Code));
		}
		outline_array[i] = outline_recv[i].Data;
	}
	
	return hit_count;
}

//--------------------------------------------------------------
/**
 * @brief   レスポンスデータ解釈：バトルビデオデータ取得
 *
 * @param   gdsrap		
 * @param   res		
 *
 * @retval  TRUE:成功
 * @retval  FALSE:何らかのエラーが発生(内容はres->Result参照)
 */
//--------------------------------------------------------------
int GDS_RAP_RESPONSE_BattleVideo_Data_Download(GDS_RAP_WORK *gdsrap, POKE_NET_RESPONSE *res)
{
	POKE_NET_GDS_RESPONSE_BATTLEDATA_GET *param;
	int ret = FALSE;
	
	switch(res->Result){
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_SUCCESS:			//!< 取得成功
		param = (POKE_NET_GDS_RESPONSE_BATTLEDATA_GET *)(res->Param);
		OS_TPrintf("バトルビデオデータ取得受信成功\n");
		ret = TRUE;
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_ERROR_AUTH:		//!< ユーザー認証エラー
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("バトルビデオデータ取得受信エラー！:ユーザー認証エラー\n");
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_ERROR_ILLEGALCODE:	//!< コードエラー
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("バトルビデオデータ取得受信エラー！:コードエラー\n");
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_ERROR_UNKNOWN:		//!< その他エラー
	default:
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("バトルビデオデータ取得受信エラー！:その他のエラー\n");
		break;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   受信バッファからデータが入っている部分のポインタを取得：バトルビデオデータ取得
 *
 * @param   gdsrap		
 * @param   rec			録画データが入っているデータへのポインタ代入先
 *
 * @retval  登録コード
 */
//--------------------------------------------------------------
int GDS_RAP_RESPONSE_BattleVideoData_Download_RecvPtr_Set(GDS_RAP_WORK *gdsrap, GT_BATTLE_REC_RECV **rec)
{
	POKE_NET_RESPONSE *res;
	POKE_NET_GDS_RESPONSE_BATTLEDATA_GET *param;
	
	res = POKE_NET_GDS_GetResponse();
	param = (POKE_NET_GDS_RESPONSE_BATTLEDATA_GET *)(res->Param);
	
	*rec = &param->Data;
	if(param->Code != param->Data.head.data_number){
		//ヘッダとデータナンバーが違う場合はヘッダにナンバーを入れられてないのでこっちで入れる
		param->Data.head.data_number = param->Code;
		//CRC作成
		(*rec)->head.crc.crc16ccitt_hash = SaveData_CalcCRC(gdsrap->savedata, &(*rec)->head, 
			sizeof(GT_BATTLE_REC_HEADER) - GT_GDS_CRC_SIZE - sizeof(param->Code));
	}
	
	return param->Code;
}

//--------------------------------------------------------------
/**
 * @brief   レスポンスデータ解釈：バトルビデオお気に入り登録
 *
 * @param   gdsrap		
 * @param   res		
 *
 * @retval  TRUE:成功
 * @retval  FALSE:何らかのエラーが発生(内容はres->Result参照)
 */
//--------------------------------------------------------------
int GDS_RAP_RESPONSE_BattleVideo_Favorite_Upload(GDS_RAP_WORK *gdsrap, POKE_NET_RESPONSE *res)
{
//	POKE_NET_GDS_RESPONSE_BATTLEDATA_FAVORITE *param;
	int ret = FALSE;
	
	switch(res->Result){
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_FAVORITE_SUCCESS:			//!< お気に入り登録成功
//		param = (POKE_NET_GDS_RESPONSE_BATTLEDATA_FAVORITE *)(res->Param);
		OS_TPrintf("バトルビデオデータお気に入り登録受信成功\n");
		ret = TRUE;
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_FAVORITE_ERROR_AUTH:		//!< ユーザー認証エラー
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("バトルビデオお気に入り登録受信エラー！:ユーザー認証エラー\n");
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_FAVORITE_ERROR_ILLEGALCODE:	//!< コードエラー
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("バトルビデオお気に入り登録受信エラー！:コードエラー\n");
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_FAVORITE_ERROR_UNKNOWN:	//!< その他エラー
	default:
		//※check　エラーごとに専用のケアとメッセージを入れる
		OS_TPrintf("バトルビデオお気に入り登録受信エラー！:その他のエラー\n");
		break;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   受信バッファからデータが入っている部分のポインタを取得：バトルビデオお気に入り登録
 *
 * @param   gdsrap		
 *
 * @retval  登録コード
 */
//--------------------------------------------------------------
void GDS_RAP_RESPONSE_BattleVideoFavorite_Upload_RecvPtr_Set(GDS_RAP_WORK *gdsrap)
{
	//現状、取得するものは何も無い
	return;
}

//============================================================================================
/**
 * @file	tvwork_record.c
 * @brief	テレビデータのレコード処理
 * @author	tamada GAME FREAK inc.
 * @date	2006.05.31
 *
 * レコード処理から呼ばれるために常駐領域に配置されていない。
 * そのためどこからでも呼べるわけではないことに注意
 */
//============================================================================================

#include "common.h"
#include "savedata/savedata_def.h"
#include "system/gamedata.h"

#include "gflib/gf_date.h"

#include "savedata/tv_work.h"
#include "tvwork_local.h"

#include "field/tv_topic.h"

#include "system/pm_rtc.h"
#include "system/pm_str.h"

#include "savedata/savedata.h"
#include "communication/comm_info.h"

#ifdef	DEBUG_ONLY_FOR_tamada
#define	DEBUG_PRT_TAMADA(...)	OS_TPrintf( __VA_ARGS__ )
#else
#define	DEBUG_PRT_TAMADA(...)	((void)0)
#endif


//==============================================================================
//	データ
//==============================================================================
///このROMで放送可能トピックID一覧：質問	※金銀で放送トピックが変わるならここを編集する
ALIGN4 static const u8 TvQandTopic[] = {
	2,4,6,7,9,10,11,13,14,15,16,17,18,19,0xff,
};
///このROMで放送可能トピックID一覧：監視	※金銀で放送トピックが変わるならここを編集する
ALIGN4 static const u8 TvWatchTopic[] = {
	1,2,3,4,6,7,8,10,13,15,16,18,19,20,21,22,23,25,26,0xff,
};
///このROMで放送可能トピックID一覧：ギネス	※金銀で放送トピックが変わるならここを編集する
ALIGN4 static const u8 TvRecordTopic[] = {
	1,3,4,5,8,9,10,11,0xff,
};

///DPで放送可能トピックID一覧：質問
ALIGN4 static const u8 DP_TvQandTopic[] = {
	2,4,6,7,9,10,11,13,0xff
};
///DPで放送可能トピックID一覧：監視
ALIGN4 static const u8 DP_TvWatchTopic[] = {
	1,2,3,4,6,10,13,15,17,18,19,20,21,22,23,0xff
};
///DPで放送可能トピックID一覧：ギネス
ALIGN4 static const u8 DP_TvRecordTopic[] = {
	1,3,4,6,8,9,0xff
};

typedef enum{
	TV_DATA_TYPE_QAND,				///このROMで放送可能なトピック：質問
	TV_DATA_TYPE_WATCH,				///このROMで放送可能なトピック：監視
	TV_DATA_TYPE_RECORD,			///このROMで放送可能なトピック：ギネス
	TV_DATA_TYPE_DP_QAND,			///DPで放送可能なトピック：質問
	TV_DATA_TYPE_DP_WATCH,			///DPで放送可能なトピック：監視
	TV_DATA_TYPE_DP_RECORD,			///DPで放送可能なトピック：ギネス
}TV_DATA_TYPE;

static const u8 * const TvTopicID_Tbl[] = {
	TvQandTopic,				//TV_DATA_TYPE_QAND,
	TvWatchTopic,           	//TV_DATA_TYPE_WATCH,
	TvRecordTopic,          	//TV_DATA_TYPE_RECORD,
	DP_TvQandTopic,         	//TV_DATA_TYPE_DP_QAND,
	DP_TvWatchTopic,        	//TV_DATA_TYPE_DP_WATCH,
	DP_TvRecordTopic,       	//TV_DATA_TYPE_DP_RECORD,
};


//============================================================================================
//
//			定義など
//
//============================================================================================
//----------------------------------------------------------
/**
 * @brief	送受信するテレビトピックの数
 */
//----------------------------------------------------------
#define	TV_RECV_MAX			(4)			

//----------------------------------------------------------
/**
 * @brief	通信する人数
 *
 * 想定できないのでとりあえず。実際は5人確定か？
 */
//----------------------------------------------------------
#define	TV_COMM_MEMBER_MAX	(16)

//============================================================================================
//
//			送信データサイズ取得
//
//============================================================================================
//----------------------------------------------------------
/**
 * @brief	テレビ用送信データサイズ
 * @param	sv		セーブデータへのアドレス
 * @return	u32		送信データのサイズ
 */
//----------------------------------------------------------
u32 TVWORK_GetSendQandADataSize(const SAVEDATA * sv)
{
	return sizeof(SEND_TOPIC) * MY_QANDA_MAX;
}
//----------------------------------------------------------
/**
 * @brief	テレビ用送信データサイズ
 * @param	sv		セーブデータへのアドレス
 * @return	u32		送信データのサイズ
 */
//----------------------------------------------------------
u32 TVWORK_GetSendWatchDataSize(const SAVEDATA * sv)
{
	return sizeof(SEND_TOPIC) * MY_WATCH_MAX;
}

//----------------------------------------------------------
/**
 * @brief	テレビ用送信データサイズ
 * @param	sv		セーブデータへのアドレス
 * @return	u32		送信データのサイズ
 */
//----------------------------------------------------------
u32 TVWORK_GetSendRecordDataSize(const SAVEDATA * sv)
{
	return sizeof(SEND_TOPIC) * MY_RECORD_MAX;
}


//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   指定TV_DATA_TYPEで放送出来るトピックかチェック
 *
 * @param   data_type		TV_DATA_TYPE
 * @param   topic_id		トピックID
 *
 * @retval  TRUE:放送可
 * @retval  FALSE:放送不可
 */
//--------------------------------------------------------------
static BOOL TvTopicIDCheck(TV_DATA_TYPE data_type, int topic_id)
{
	int i;
	
	for(i = 0; TvTopicID_Tbl[data_type][i] != 0xff; i++){
		if(TvTopicID_Tbl[data_type][i] == topic_id){
			return TRUE;
		}
	}
	return FALSE;
}


//============================================================================================
//
//
//			送信データ生成
//
//
//============================================================================================
//----------------------------------------------------------
/**
 * @brief		自分データから送信データを作成する
 * @param		sv		セーブデータへのアドレス
 * @param		send	送信データを格納するバッファ
 * @param		mt		自分データへのポインタ
 * @param		max		自分データの数
 * @param		dp_mode	TRUE:DP用のテレビデータを作成。　FALSE:プラチナ以降用のテレビデータ作成
 * @param		data_type	TV_DATA_TYPE(dp_mode==TRUE時のみ使用)
 */
//----------------------------------------------------------
static void MakeSendTopicByMyTopic(SAVEDATA * sv, SEND_TOPIC * send,
		MY_TOPIC * mt, int max, int dp_mode, int data_type)
{
	int i;
	MYSTATUS * my = SaveData_GetMyStatus(sv);

	for (i = 0; i < max; i++, send++) {
		MI_CpuClearFast(send, sizeof(SEND_TOPIC));

		//送信者情報を埋め込む
		send->owner_id = MyStatus_GetID(my);
		send->sex = MyStatus_GetMySex(my);
		send->rom_ver = PM_VERSION;
		send->region_code = PM_LANG;
		PM_strncpy(send->owner_name, MyStatus_GetMyName(my), PERSON_NAME_SIZE + EOM_SIZE);

		send->topic = mt[i].topic;
		send->topic.see_count = 0;	//送信データの視聴カウントはクリア
		if(dp_mode == TRUE && TvTopicIDCheck(data_type, send->topic.topic_id) == FALSE){
			send->topic.topic_id = 0;	//放送出来ないものはトピックID無しにする
		}
	}
	//送信データを生成したら自分データは削除する
	//MI_CpuClearFast(mt, sizeof(MY_TOPIC) * max);
}
//----------------------------------------------------------
/**
 * @brief	質問型トピック生成データをAlloc(DP向けテレビデータ)
 * @param	sv
 * @param	headID
 * @param	size
 */
//----------------------------------------------------------
SEND_TOPIC * TVWORK_AllocSendQandAData(SAVEDATA * sv, int headID, u32 size)
{
	SEND_TOPIC * buf;
	TV_WORK * tvwk;
	buf = sys_AllocMemoryLo(headID, size);
	tvwk = SaveData_GetTvWork(sv);
	GF_ASSERT(size == TVWORK_GetSendQandADataSize(sv));
	MakeSendTopicByMyTopic(sv, buf, tvwk->MyQandATopic, MY_QANDA_MAX, TRUE, TV_DATA_TYPE_DP_QAND);
	return buf;
}
//----------------------------------------------------------
/**
 * @brief	監視型トピック生成データをAlloc(DP向けテレビデータ)
 * @param	sv
 * @param	headID
 * @param	size
 */
//----------------------------------------------------------
SEND_TOPIC * TVWORK_AllocSendWatchData(SAVEDATA * sv, int headID, u32 size)
{
	SEND_TOPIC * buf;
	TV_WORK * tvwk;
	buf = sys_AllocMemoryLo(headID, size);
	tvwk = SaveData_GetTvWork(sv);
	GF_ASSERT(size == TVWORK_GetSendWatchDataSize(sv));
	MakeSendTopicByMyTopic(sv, buf, tvwk->MyWatchTopic, MY_WATCH_MAX, TRUE, TV_DATA_TYPE_DP_WATCH);
	return buf;
}
//----------------------------------------------------------
/**
 * @brief	ギネス型トピック生成データをAlloc(DP向けテレビデータ)
 * @param	sv
 * @param	headID
 * @param	size
 */
//----------------------------------------------------------
SEND_TOPIC * TVWORK_AllocSendRecordData(SAVEDATA * sv, int headID, u32 size)
{
	SEND_TOPIC * buf;
	TV_WORK * tvwk;
	buf = sys_AllocMemoryLo(headID, size);
	tvwk = SaveData_GetTvWork(sv);
	GF_ASSERT(size == TVWORK_GetSendRecordDataSize(sv));
	MakeSendTopicByMyTopic(sv, buf, tvwk->MyRecordTopic, MY_RECORD_MAX, TRUE, TV_DATA_TYPE_DP_RECORD);
	return buf;
}
//----------------------------------------------------------
/**
 * @brief	質問型トピック生成データをAlloc(プラチナ以降テレビデータ)
 * @param	sv
 * @param	headID
 * @param	size
 */
//----------------------------------------------------------
SEND_TOPIC * TVWORK_AllocSendQandADataEx(SAVEDATA * sv, int headID, u32 size)
{
	SEND_TOPIC * buf;
	TV_WORK * tvwk;
	buf = sys_AllocMemoryLo(headID, size);
	tvwk = SaveData_GetTvWork(sv);
	GF_ASSERT(size == TVWORK_GetSendQandADataSize(sv));
	MakeSendTopicByMyTopic(sv, buf, tvwk->MyQandATopic, MY_QANDA_MAX, 0, TV_DATA_TYPE_QAND);
	return buf;
}
//----------------------------------------------------------
/**
 * @brief	監視型トピック生成データをAlloc(プラチナ以降テレビデータ)
 * @param	sv
 * @param	headID
 * @param	size
 */
//----------------------------------------------------------
SEND_TOPIC * TVWORK_AllocSendWatchDataEx(SAVEDATA * sv, int headID, u32 size)
{
	SEND_TOPIC * buf;
	TV_WORK * tvwk;
	buf = sys_AllocMemoryLo(headID, size);
	tvwk = SaveData_GetTvWork(sv);
	GF_ASSERT(size == TVWORK_GetSendWatchDataSize(sv));
	MakeSendTopicByMyTopic(sv, buf, tvwk->MyWatchTopic, MY_WATCH_MAX, 0, TV_DATA_TYPE_WATCH);
	return buf;
}
//----------------------------------------------------------
/**
 * @brief	ギネス型トピック生成データをAlloc(プラチナ以降テレビデータ)
 * @param	sv
 * @param	headID
 * @param	size
 */
//----------------------------------------------------------
SEND_TOPIC * TVWORK_AllocSendRecordDataEx(SAVEDATA * sv, int headID, u32 size)
{
	SEND_TOPIC * buf;
	TV_WORK * tvwk;
	buf = sys_AllocMemoryLo(headID, size);
	tvwk = SaveData_GetTvWork(sv);
	GF_ASSERT(size == TVWORK_GetSendRecordDataSize(sv));
	MakeSendTopicByMyTopic(sv, buf, tvwk->MyRecordTopic, MY_RECORD_MAX, 0, TV_DATA_TYPE_RECORD);
	return buf;
}


//============================================================================================
//
//
//			受信データ処理
//
//
//============================================================================================
//----------------------------------------------------------
/**
 * @brief	受信トピックを他人トピック領域にセットする
 * @param	ot			他人トピック領域へのポインタ
 * @param	max			他人トピック領域のサイズ
 * @param	st			受信トピックデータ
 * @param   data_type	TV_DATA_TYPE
 * @retval	TRUE		新たにセットした
 * @retval	FALSE		セットしなかった
 * @retval	-1			放送出来ないトピックの為セットしなかった
 */
//----------------------------------------------------------
static BOOL AddOtherTopic(OTHER_TOPIC * ot, int max, const SEND_TOPIC * st, TV_DATA_TYPE data_type)
{
	int i;
	if (st->topic.topic_id == 0) {
		DEBUG_PRT_TAMADA("NULL topic tr_id=%d\n",st->owner_id & 0xffff);
		//空のトピックデータが来た場合
		return FALSE;
	}
	if(TvTopicIDCheck(data_type, st->topic.topic_id) == FALSE){
		return -1;	//放送出来ないトピック
	}
	
	for (i = 0; i < max; i++) {
		if (ot[i].topic.topic_id == 0) {
			DEBUG_PRT_TAMADA("SET! pos=%d topic=%d tr_id=%d\n",i, st->topic.topic_id, st->owner_id & 0xffff);
			ot[i] = *st;
			return TRUE;
		}
	}
	for (i = 0; i < max; i++) {
		if (ot[i].topic.see_count >= BROADCAST_MAX) {
			DEBUG_PRT_TAMADA("SET! pos=%d topic=%d tr_id=%d\n",i, st->topic.topic_id, st->owner_id & 0xffff);
			ot[i] = *st;
			return TRUE;
		}
	}
	return FALSE;
}

//----------------------------------------------------------
/**
 * @brief	テレビ受信データまぜる処理本体
 * @param	ot			他人トピック領域へのポインタ
 * @param	max			他人トピック領域のサイズ
 * @param	member		通信人数
 * @param	my_id		自分の通信ID
 * @param	darray		受信データへのポインタの配列へのポインタ
 * @param	darray		DPテレビ受信データへのポインタの配列へのポインタ
 */
//----------------------------------------------------------
static const void ** MixRecvBufferSelect(int net_id, const void ** darray, const void ** dp_darray)
{
	MYSTATUS *myst;

	myst = CommInfoGetMyStatus(net_id);
	if(myst == NULL){
		return NULL;	//処理の流れ的にありえないけど一応
	}
	if(MyStatus_GetRomCode(myst) == POKEMON_DP_ROM_CODE){
		return dp_darray;
	}
	return darray;
}

static void MixRecvData(OTHER_TOPIC * ot, int max, int member, int my_id, const void ** darray, const void ** dp_darray, TV_DATA_TYPE data_type)
{
	u8 count[TV_COMM_MEMBER_MAX];
	int pos, i, j;
	const void **buffer;
	
	DEBUG_PRT_TAMADA("OT=%08x max=%2d my_id=%5d\n", ot, max, my_id);
	for (i = 0; i < member; i++) {
		DEBUG_PRT_TAMADA("ID[%d]%08x\n", i, darray[i]);
	}

	MI_CpuClear8(count, TV_COMM_MEMBER_MAX);

	pos = 0;
	for (i = 0; i < member; i++) {
		if (darray[i] == NULL) { continue; }//いない場合は次の人へ
		for (j = 0; j < TV_RECV_MAX; j ++) {
			while (darray[pos] == NULL || pos == i || count[pos] == TV_RECV_MAX) {
				//いない人と自分とすでにもらいすぎの人はパス
				pos ++;
				pos %= member;
			}
			count[pos] ++;
			DEBUG_PRT_TAMADA(" %d[%d]->%d[%d]",i,j, pos, count[pos]);
			if (pos != my_id) {
				//人のデータの場合、追加する
				buffer = MixRecvBufferSelect(pos, darray, dp_darray);
				if(buffer != NULL){
					AddOtherTopic(ot, max, &((SEND_TOPIC *)buffer[pos])[j], data_type);
				}
			}
		}
		DEBUG_PRT_TAMADA("\n");
	}
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TVWORK)
	SVLD_SetCrc(GMDATA_ID_TVWORK);
#endif //CRC_LOADCHECK
}

//----------------------------------------------------------
/**
 * @brief	テレビ用データ混ぜる処理（監視型）DP向け
 * @param	tvwk			テレビデータへのポインタ
 * @param	member		通信人数
 * @param	my_id		自分の通信ID
 * @param	darray		受信データへのポインタの配列へのポインタ
 * @param	dp_darray	DPテレビ受信データへのポインタの配列へのポインタ
 *
 */
//----------------------------------------------------------
void TVWORK_MixWatchData(TV_WORK * tv, int member, int my_id, const void ** darray, const void ** dp_darray)
{
	MixRecvData(tv->OtherWatchTopic, OTHER_WATCH_MAX, member, my_id, darray, dp_darray, TV_DATA_TYPE_WATCH);
}

//----------------------------------------------------------
/**
 * @brief	テレビ用データ混ぜる処理（質問型）DP向け
 * @param	tvwk			テレビデータへのポインタ
 * @param	member		通信人数
 * @param	my_id		自分の通信ID
 * @param	darray		受信データへのポインタの配列へのポインタ
 * @param	dp_darray	DPテレビ受信データへのポインタの配列へのポインタ
 *
 */
//----------------------------------------------------------
void TVWORK_MixRecordData(TV_WORK * tv, int member, int my_id, const void ** darray, const void ** dp_darray)
{
	MixRecvData(tv->OtherRecordTopic, OTHER_RECORD_MAX, member, my_id, darray, dp_darray, TV_DATA_TYPE_RECORD);
}

//----------------------------------------------------------
/**
 * @brief	テレビ用データまぜる処理（質問型）DP向け
 * @param	tvwk			テレビデータへのポインタ
 * @param	member		通信人数
 * @param	my_id		自分の通信ID
 * @param	darray		受信データへのポインタの配列へのポインタ
 * @param	dp_darray	DPテレビ受信データへのポインタの配列へのポインタ
 *
 */
//----------------------------------------------------------
void TVWORK_MixQandAData(TV_WORK * tv, int member, int my_id, const void ** darray, const void ** dp_darray)
{
	MixRecvData(tv->OtherQandATopic, OTHER_QANDA_MAX, member, my_id, darray, dp_darray, TV_DATA_TYPE_QAND);
}


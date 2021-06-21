//==============================================================================
/**
 * @file	battle_rec_ov.c
 * @brief	オーバーレイbattle_recorderでのみ使用する録画関連ツール類
 * @author	matsuda
 * @date	2008.05.15(木)
 *
 * 本来ならbattle_rec.cに含めるものだが、常駐メモリの関係上、バトルレコーダーでしか
 * 使用しないようなものは、ここに移動
 */
//==============================================================================
#include "common.h"
#include "savedata\savedata.h"
#include "system\gamedata.h"
#include "gflib\strbuf_family.h"
#include "poketool\pokeparty.h"
#include "savedata\pokeparty_local.h"
#include "savedata\mystatus_local.h"
#include "savedata\config.h"

#include "gds_local_common.h"
#include "savedata\battle_rec.h"
#include "itemtool/itemsym.h"
#include "battle/wazano_def.h"
#include "gds_profile_types.h"
#include "savedata/gds_profile.h"
#include "system/snd_perap.h"

#include "application/battle_recorder/gds_battle_rec.h"
#include "savedata/misc.h"
#include "savedata/regulation_data.h"

#include "battle_rec_local.h"
#include "gflib/strcode.h"
#include "gflib\msg_print.h"
#include "system\fontproc.h"
#include "system\msgdata.h"
#include "msgdata/msg.naix"
#include "msgdata/msg_battle_rec.h"



//==============================================================================
//	プロトタイプ宣言
//==============================================================================
static void ErrorNameSet(STRBUF *dest_str, int heap_id);

//==============================================================================
//	
//==============================================================================
extern BATTLE_REC_SAVEDATA * brs;



//--------------------------------------------------------------
/**
 * @brief   BATTLE_PARAMのトレーナー名、ポケモン名を調べて不明な文字列の場合はケア処理を入れる
 *
 * @param   src			録画セーブデータへのポインタ
 * @param   bp			バトルパラメータへのポインタ
 * @param   heap_id		テンポラリで使用するヒープID
 */
//--------------------------------------------------------------
void BattleRecTool_ErrorStrCheck(BATTLE_REC_SAVEDATA *src, BATTLE_PARAM *bp, int heap_id)
{
	BATTLE_REC_WORK *rec = &src->rec;
	BATTLE_REC_HEADER *head = &src->head;
	int client_max, temoti_max, client, temoti;
	STRBUF *check_str, *temp_str;
	STRCODE *check_strcode;
	int strcode_size = 64;	//適当に大きく
	int s, no, poke_count;
	POKEMON_PARAM *pp;
	
	BattleRec_ClientTemotiGet(head->mode, &client_max, &temoti_max);
	check_str = STRBUF_Create(strcode_size, heap_id);
	temp_str = STRBUF_Create(strcode_size, heap_id);
	check_strcode = sys_AllocMemory(heap_id, sizeof(STRCODE) * strcode_size);
	
	//トレーナー名の文字列チェック
	for(client = 0; client < client_max; client++){
		//終端があるか
		for(s = BUFLEN_PERSON_NAME - 1; s > -1; s--){
			if(bp->trainer_data[client].name[s] == EOM_){
				break;
			}
		}
		if(s == -1){	//文字コードがない
			OS_TPrintf("トレーナー名 %d に終端コードがない\n", client);
			ErrorNameSet(temp_str, heap_id);
			STRBUF_GetStringCode(temp_str, bp->trainer_data[client].name, BUFLEN_PERSON_NAME);
			continue;
		}
		
		//不明文字コードチェック
		STRBUF_Clear(check_str);
		STRBUF_SetStringCode(check_str, bp->trainer_data[client].name);
		if(FontProc_ErrorStrCheck(FONT_SYSTEM, check_str, temp_str) == FALSE){
			OS_TPrintf("トレーナー名 %d に不明な文字コードが入っている\n", client);
			ErrorNameSet(temp_str, heap_id);
			STRBUF_GetStringCode(temp_str, bp->trainer_data[client].name, BUFLEN_PERSON_NAME);
			continue;
		}
	}
	
	//ポケモン名の文字列チェック
	for(client = 0; client < client_max; client++){
		poke_count = PokeParty_GetPokeCount(bp->poke_party[client]);
		for(no = 0; no < poke_count; no++){
			pp = PokeParty_GetMemberPointer(bp->poke_party[client], no);
			if(PokeParaGet(pp, ID_PARA_poke_exist, NULL) == FALSE){
				break;
			}
			//ID_PARA_nicknameで取得すれば必ず終端がついているので終端チェックはしない
			//戦闘でもID_PARA_nicknameしか使用していないので。
			MI_CpuClear16(check_strcode, sizeof(STRCODE) * strcode_size);
			PokeParaGet(pp, ID_PARA_nickname, check_strcode);
			//不明文字コードチェック
			STRBUF_Clear(check_str);
			STRBUF_SetStringCode(check_str, check_strcode);
			if(FontProc_ErrorStrCheck(FONT_SYSTEM, check_str, temp_str) == FALSE){
				OS_TPrintf("ポケモン名 client:%d no:%d に不明な文字コードが入っている\n", client, no);
				//デフォルト名を再セット
				PokeParaPut(pp, ID_PARA_default_name, NULL);
				continue;
			}
		}
	}
	
	STRBUF_Delete(check_str);
	STRBUF_Delete(temp_str);
	sys_FreeMemoryEz(check_strcode);
}

//--------------------------------------------------------------
/**
 * @brief   エラー文字だった場合に表示する代理名をバッファにセット
 *
 * @param   dest_str		代入先
 * @param   heap_id			テンポラリで使用するヒープ
 */
//--------------------------------------------------------------
static void ErrorNameSet(STRBUF *dest_str, int heap_id)
{
	MSGDATA_MANAGER * msgman;

	STRBUF_Clear(dest_str);
	
	msgman = MSGMAN_Create( MSGMAN_TYPE_DIRECT, ARC_MSG, NARC_msg_battle_rec_dat, heap_id );
	MSGMAN_GetString( msgman, msg_error_trainer_name, dest_str );
	MSGMAN_Delete( msgman );
}


//==============================================================================
//	
//==============================================================================

//--------------------------------------------------------------
/**
 * @brief   GDSで受信したデータをセーブする
 *
 * @param   sv				セーブデータへのポインタ
 * @param   recv_data		GT_BATTLE_REC_RECV構造体
 * @param   num				LOADDATA_MYREC、LOADDATA_DOWNLOAD0、LOADDATA_DOWNLOAD1…
 * @param   secure		視聴済みフラグ(TRUE:視聴済みにしてセーブする。
 * 						FALSE:フラグ操作しない(サーバーでsecureを立てているなら立ったままになる)
 * @param   work0		セーブ進行を制御するワークへのポインタ(最初は0クリアした状態で呼んで下さい)
 * @param   work1		セーブ進行を制御するワークへのポインタ(最初は0クリアした状態で呼んで下さい)
 *
 * @retval  SAVE_RESULT_CONTINUE	セーブ処理継続中
 * @retval  SAVE_RESULT_LAST		セーブ処理継続中、最後の一つ前
 * @retval  SAVE_RESULT_OK			セーブ正常終了
 * @retval  SAVE_RESULT_NG			セーブ失敗終了
 *
 * GDSで受信したデータはbrsにコピーした時点(BattleRec_DataSet)で復号されているので、
 * 内部で暗号化してからセーブを行う
 */
//--------------------------------------------------------------
SAVE_RESULT BattleRec_GDS_RecvData_Save(SAVEDATA *sv, int num, u8 secure, u16 *work0, u16 *work1)
{
	SAVE_RESULT result;
	
	switch(*work0){
	case 0:
		GF_ASSERT(brs);
		
		if(secure == TRUE){
			brs->head.secure = secure;
			//secureフラグが更新されたので、再度CRCを作り直す
			brs->head.magic_key = REC_OCC_MAGIC_KEY;
			brs->head.crc.crc16ccitt_hash = SaveData_CalcCRC(sv, &brs->head, 
				sizeof(BATTLE_REC_HEADER) - GDS_CRC_SIZE - DATANUMBER_SIZE);
		}
		
		//CRCをキーにして暗号化
		//※GDSで受信したデータはbrsにコピーした時点(BattleRec_DataSet)で復号されているので。
		BattleRec_Coded(&brs->rec, sizeof(BATTLE_REC_WORK) - GDS_CRC_SIZE, 
			brs->rec.crc.crc16ccitt_hash + ((brs->rec.crc.crc16ccitt_hash ^ 0xffff) << 16));
		(*work0)++;
		break;
	case 1:
		result = Local_BattleRecSave(sv, brs, num, work1);
		return result;
	}
	return SAVE_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   GDSで送信する為に、データに変換処理を行う
 *
 * @param   sv		セーブデータへのポインタ
 *
 * この関数使用後のbrsは、このまま再生したり保存してはいけない形になっているので、
 * GDSで送信し終わったら破棄すること！
 * 但し、BattleRec_GDS_MySendData_DataNumberSetSave関数を使用する場合はOK。
 */
//--------------------------------------------------------------
void BattleRec_GDS_SendData_Conv(SAVEDATA *sv)
{
	GF_ASSERT(brs);
	
	//※brsに展開されているデータは、本体が複合化されているので送信前に再度暗号化を行う
	BattleRec_Coded(&brs->rec, sizeof(BATTLE_REC_WORK) - GDS_CRC_SIZE, 
		brs->rec.crc.crc16ccitt_hash + ((brs->rec.crc.crc16ccitt_hash ^ 0xffff) << 16));
}

//--------------------------------------------------------------
/**
 * @brief   GDSで送信したbrsに対して、データナンバーをセットしセーブを行う
 *
 * @param   sv				セーブデータへのポインタ
 * @param   data_number		データナンバー
 * @param   work		セーブ進行を制御するワークへのポインタ(最初は0クリアした状態で呼んで下さい)
 * @param   work		セーブ進行を制御するワークへのポインタ(最初は0クリアした状態で呼んで下さい)
 * 
 * @retval	SAVE_RESULT_OK		セーブ成功
 * @retval	SAVE_RESULT_NG		セーブ失敗
 */
//--------------------------------------------------------------
SAVE_RESULT BattleRec_GDS_MySendData_DataNumberSetSave(SAVEDATA *sv, u64 data_number, u16 *work0, u16 *work1)
{
	SAVE_RESULT result;

	switch(*work0){
	case 0:
		GF_ASSERT(brs);
		
		//データナンバーは元々CRCの対象からはずれているので、CRCを再作成する必要はない
		brs->head.data_number = data_number;
		(*work0)++;
		break;
	case 1:
		//送信時に行っているBattleRec_GDS_SendData_Conv関数で既に暗号化しているはずなので
		//そのままセーブを行う
		result = Local_BattleRecSave(sv, brs, LOADDATA_MYREC, work1);
		return result;
	}
	return SAVE_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   バトルレコーダー(オフライン)でビデオを見た後、brsに読み込んでいるバトルビデオに
 * 			視聴済みフラグをセットしてセーブする(一括セーブ)
 * 			※通常セーブ込み
 *
 * @param   sv		セーブデータへのポインタ
 * @param	num		セーブするデータナンバー（LOADDATA_MYREC、LOADDATA_DOWNLOAD0、LOADDATA_DOWNLOAD1…）
 *
 * @retval  SAVE_RESULT_???
 *
 * 内部で本体の暗号化処理を行う為、この関数以後はbrsは解放してください。
 */
//--------------------------------------------------------------
SAVE_RESULT BattleRec_SecureSetSave(SAVEDATA *sv, int num)
{
	SAVE_RESULT result;

	GF_ASSERT(brs != NULL);
	
	brs->head.secure = TRUE;
	//secureフラグが更新されたので、再度CRCを作り直す
	brs->head.magic_key = REC_OCC_MAGIC_KEY;
	brs->head.crc.crc16ccitt_hash = SaveData_CalcCRC(sv, &brs->head, 
		sizeof(BATTLE_REC_HEADER) - GDS_CRC_SIZE - DATANUMBER_SIZE);

	//CRCをキーにして暗号化
	BattleRec_Coded(&brs->rec, sizeof(BATTLE_REC_WORK) - GDS_CRC_SIZE, 
		brs->rec.crc.crc16ccitt_hash + ((brs->rec.crc.crc16ccitt_hash ^ 0xffff) << 16));
	
	sys_SoftResetNG(SOFTRESET_TYPE_VIDEO);
	result = SaveData_Extra_SaveBattleRecData(sv, brs, num);
	if(result == SAVE_RESULT_OK){
		result = SaveData_Save(sv);
	}
	sys_SoftResetOK(SOFTRESET_TYPE_VIDEO);
	return result;
}



#ifdef PM_DEBUG
//--------------------------------------------------------------
/**
 * @brief   読み込み済みのbrsに対してsecureフラグをセットする
 *
 * @param   sv		
 */
//--------------------------------------------------------------
void DEBUG_BattleRec_SecureFlagSet(SAVEDATA *sv)
{
	GF_ASSERT(brs != NULL);
	
	brs->head.secure = TRUE;
	//secureフラグが更新されたので、再度CRCを作り直す
	brs->head.magic_key = REC_OCC_MAGIC_KEY;
	brs->head.crc.crc16ccitt_hash = SaveData_CalcCRC(sv, &brs->head, 
		sizeof(BATTLE_REC_HEADER) - GDS_CRC_SIZE - DATANUMBER_SIZE);
}
#endif	//PM_DEBUG

//==============================================================================
/**
 * @file	frontier_ex.c
 * @brief	バトルフロンティア外部データ(進行に関係ない記録のみ)
 * @author	matsuda
 * @date	2007.07.18(水)
 */
//==============================================================================
#include "common.h"
#include "savedata/savedata.h"
#include "system/gamedata.h"
#include "gflib/strbuf_family.h"
#include "poketool/pokeparty.h"
#include "savedata/pokeparty_local.h"
#include "savedata/mystatus_local.h"
#include "savedata/config.h"

#include "savedata/frontier_savedata.h"
#include "savedata/frontier_ex.h"


//==============================================================================
//	構造体定義
//==============================================================================
///バトルステージ外部記録データ
typedef struct{
	u16 renshou_single[MONSNO_MAX];		///<シングル連勝記録
	u16 renshou_double[MONSNO_MAX];		///<ダブル連勝記録
	u16 renshou_multi[MONSNO_MAX];		///<ワイヤレス連勝記録
//	u16 renshou_wifi[MONSNO_MAX];		///<Wifi連勝記録

	u16 padding;	//4バイト境界オフセット
}STAGE_EX;

//----------------------------------------------------------
/**
 *	バトルフロンティア外部データ構造体
 */
//----------------------------------------------------------
struct _FRONTIER_EX_SAVEDATA {
	//殿堂入り以外の外部セーブは必ず一番最初にEX_SAVE_KEY構造体を配置しておくこと
	EX_CERTIFY_SAVE_KEY save_key;			///<認証キー

	STAGE_EX stage;		//バトルステージ外部記録データ
};


//==============================================================================
//	プロトタイプ宣言
//==============================================================================
static u16 FrontierEx_StageRenshou_Set(FRONTIER_EX_SAVEDATA *fes, int id, int monsno, u16 data);
static u16 FrontierEx_StageRenshou_SetIfLarge(SAVEDATA *sv, FRONTIER_EX_SAVEDATA *fes, int id, int monsno, u16 data);


//==============================================================================
//
//	
//
//==============================================================================

//------------------------------------------------------------------
/**
 * セーブデータサイズを返す
 *
 * @retval  int		
 */
//------------------------------------------------------------------
int FrontierEx_GetWorkSize( void )
{
	GF_ASSERT(sizeof(FRONTIER_EX_SAVEDATA) % 4 == 0);	//4バイト境界になっているか確認
	return sizeof(FRONTIER_EX_SAVEDATA);
}

//------------------------------------------------------------------
/**
 * データ初期化
 *
 * @param   data		
 *
 */
//------------------------------------------------------------------
void FrontierEx_Init(FRONTIER_EX_SAVEDATA *work)
{
	MI_CpuClear8(work, sizeof(FRONTIER_EX_SAVEDATA));
	work->save_key = EX_CERTIFY_SAVE_KEY_NO_DATA;
}

//------------------------------------------------------------------
/**
 * フロンティア外部データのロード
 *
 * @param	sv		セーブデータ構造体へのポインタ
 * @param	heapID	データをロードするメモリを確保するためのヒープID
 * @param	result	ロード結果を格納するワーク
 *
 * @retval	読み込んだフロンティア外部データへのポインタ
 */
//------------------------------------------------------------------
FRONTIER_EX_SAVEDATA * FrontierEx_Load(SAVEDATA *sv,int heapID,LOAD_RESULT *result)
{
	FRONTIER_EX_SAVEDATA *fes;
	
	fes = SaveData_Extra_LoadFrontierEx(sv,heapID,result);
	return fes;
}

//------------------------------------------------------------------
/**
 * フロンティア外部データのセーブ
 *
 * @param	sv		セーブデータ構造体へのポインタ
 * @param	data	セーブデータへのポインタ(CHECK_TAIL_DATA構造体分を含めたものである
 * 						必要があるのでFrontierEx_Load関数の戻り値で手に入れた
 *						バッファポインタを渡す必要がある。
 *
 * @retval	セーブ結果
 */
//------------------------------------------------------------------
SAVE_RESULT FrontierEx_Save(SAVEDATA *sv, FRONTIER_EX_SAVEDATA *fes)
{
	SAVE_RESULT result;
	
	result = SaveData_Extra_SaveFrontierEx(sv, fes);
	result |= SaveData_Save(sv);	//外部の後は通常セーブ
	return result;
}


//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   ポケモン毎のステージ最大連勝数取得
 *
 * @param   fes		フロンティア外部データへのポインタ
 * @param   id			FREXID_STAGE_RENSHOU_???
 * @param   monsno		ポケモン番号
 *
 * @retval  最大連勝数
 */
//--------------------------------------------------------------
u16 FrontierEx_StageRenshou_Get(SAVEDATA *sv, FRONTIER_EX_SAVEDATA *fes, int id, int monsno)
{
	if(SaveData_GetExtraInitFlag(sv) == FALSE){
		return 0;	//外部セーブが初期化されていないのでデータ無しと判定し、常に0を返す
	}
	
	switch(id){
	case FREXID_STAGE_RENSHOU_SINGLE:
		return fes->stage.renshou_single[monsno];
	case FREXID_STAGE_RENSHOU_DOUBLE:
		return fes->stage.renshou_double[monsno];
	case FREXID_STAGE_RENSHOU_MULTI:
		return fes->stage.renshou_multi[monsno];
//	case FREXID_STAGE_RENSHOU_WIFI:
//		return fes->stage.renshou_wifi[monsno];
	}
	GF_ASSERT(0);
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief   ポケモン毎のステージ最大連勝数セット
 *
 * @param   fes		フロンティア外部データへのポインタ
 * @param   id			FREXID_STAGE_RENSHOU_???
 * @param   monsno		ポケモン番号
 * @param   data		セットする値
 *
 * @retval  セット後の値
 */
//--------------------------------------------------------------
static u16 FrontierEx_StageRenshou_Set(FRONTIER_EX_SAVEDATA *fes, int id, int monsno, u16 data)
{
	if(data > FRONTIER_RECORD_LIMIT){
		data = FRONTIER_RECORD_LIMIT;
	}
	
	switch(id){
	case FREXID_STAGE_RENSHOU_SINGLE:
		fes->stage.renshou_single[monsno] = data;
		break;
	case FREXID_STAGE_RENSHOU_DOUBLE:
		fes->stage.renshou_double[monsno] = data;
		break;
	case FREXID_STAGE_RENSHOU_MULTI:
		fes->stage.renshou_multi[monsno] = data;
		break;
//	case FREXID_STAGE_RENSHOU_WIFI:
//		fes->stage.renshou_wifi[monsno] = data;
//		break;
	default:
		GF_ASSERT(0);
		return 0;
	}
	
	return data;
}

//デバック用
u16 Debug_FrontierEx_StageRenshou_Set(FRONTIER_EX_SAVEDATA *fes, int id, int monsno, u16 data)
{
	return FrontierEx_StageRenshou_Set(fes, id, monsno, data);
}

//--------------------------------------------------------------
/**
 * @brief   大きければ最大連勝数を更新する
 *
 * @param   fes		フロンティア外部データへのポインタ
 * @param   id			FREXID_STAGE_RENSHOU_???
 * @param   monsno		ポケモン番号
 * @param   data		セット(比較)する値
 *
 * @retval  結果の値
 */
//--------------------------------------------------------------
static u16 FrontierEx_StageRenshou_SetIfLarge(SAVEDATA *sv, FRONTIER_EX_SAVEDATA *fes, int id, int monsno, u16 data)
{
	u16 now;
	
	if(SaveData_GetExtraInitFlag(sv) == FALSE){
		return 0;	//外部セーブが初期化されていないのでデータ無しと判定し、常に0を返す
	}
	
	now = FrontierEx_StageRenshou_Get(sv, fes, id, monsno);
	OS_Printf( "現在のnow = %d\n", now );
	if(now < data){
		return FrontierEx_StageRenshou_Set(fes, id, monsno, data);
	}
	else{
		if(now > FRONTIER_RECORD_LIMIT){	//現在値がリミットオーバーしていた時の訂正処理
			return FrontierEx_StageRenshou_Set(fes, id, monsno, FRONTIER_RECORD_LIMIT);
		}
		return now;
	}
}

//--------------------------------------------------------------
/**
 * @brief   バトルステージの外部連勝記録を書き込み、セーブを行う
 *
 * @param   sv					セーブデータへのポインタ
 * @param   record_id			FRID_STAGE_???
 * @param   record_monsno_id	FRID_STAGE_SINGLE_MONSNO | DOUBLE | MULTI
 * @param   friend_no			友達手帳番号(友達データでない場合はFRONTIER_RECORD_NOT_FRIEND指定)
 * @param   id					FREXID_STAGE_RENSHOU_???
 * @param   heap_id				テンポラリとして使用するヒープID
 * @param   load_result			外部セーブのロード結果
 * @param   save_result			外部セーブのセーブ結果
 *
 * @retval  TRUE:外部セーブ＆通常セーブ実行
 * @retval  FALSE:外部も通常もセーブはしなかった
 *
 * 外部セーブファイルをロード　＞　常駐に配置されているフロンティアレコードからセットする値を取得
 * 　＞　外部セーブの値と比較し、大きければセット　＞　外部セーブ実行
 * 以上の処理を行います
 */
//--------------------------------------------------------------
BOOL FrontierEx_StageRenshou_RenshouCopyExtra(SAVEDATA *sv, int record_id, int record_monsno_id, 
	int friend_no, int save_id, int heap_id, LOAD_RESULT *load_result, SAVE_RESULT *save_result)
{
	FRONTIER_SAVEWORK *fsv;
	FRONTIER_EX_SAVEDATA * fes;
	u16 ret_num, now, before;
	int monsno;
	BOOL ret = FALSE;
	
	GF_ASSERT(record_id >= FRID_STAGE_SINGLE_RENSHOU && record_id <= FRID_STAGE_MULTI_COMM_MONSNO);
	GF_ASSERT(record_monsno_id == FRID_STAGE_SINGLE_MONSNO || record_monsno_id == FRID_STAGE_DOUBLE_MONSNO || record_monsno_id == FRID_STAGE_MULTI_COMM_MONSNO);
	
	*load_result = LOAD_RESULT_OK;
	*save_result = SAVE_RESULT_OK;
	
	if(friend_no != FRONTIER_RECORD_NOT_FRIEND){
		return ret;//0;	//Wifiの時は無視
	}
	
	if(SaveData_GetExtraInitFlag(sv) == FALSE){
		return ret;//0;	//外部セーブが初期化されていないのでデータ無しと判定
	}

	fsv = SaveData_GetFrontier(sv);
	now = FrontierRecord_Get(fsv, record_id, friend_no);
	monsno = FrontierRecord_Get(fsv, record_monsno_id, friend_no);
	OS_Printf( "now = %d\n", now );
	OS_Printf( "monsno = %d\n", monsno );
	
	fes = FrontierEx_Load(sv, heap_id, load_result);
	if(*load_result != LOAD_RESULT_OK){
		before = 0;	//セーブ破壊orキーと一致しない、場合は新規データと解釈し、カウンタ0とする
	}
	else{
		before = FrontierEx_StageRenshou_Get(sv, fes, save_id, monsno);
	}
	
	ret_num = FrontierEx_StageRenshou_SetIfLarge(sv, fes, save_id, monsno, now);
	OS_Printf( "ret_num = %d\n", ret_num );
	OS_Printf( "now = %d\n", now );

	//07.12.27変更
	if(now != before){
	//if(now != ret_num){
		*save_result = FrontierEx_Save(sv, fes);	//値が更新された時のみセーブ
		ret = TRUE;
		OS_Printf( "値が更新された！ = %d\n", now );
	}
	
	if( fes != NULL ){
		sys_FreeMemoryEz(fes);
	}

	return ret;//ret_num;
}

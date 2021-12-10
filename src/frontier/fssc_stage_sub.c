//============================================================================================
/**
 * @file	fss_stage_sub.c
 * @bfief	フロンティアシステムスクリプトコマンドサブ：ステージ
 * @author	Satoshi Nohara
 * @date	07.06.08
 */
//============================================================================================
#include "common.h"
#include "savedata/savedata_def.h"
#include "savedata/record.h"
#include "system/buflen.h"
#include "poketool/poke_tool.h"
#include "poketool/pokeparty.h"
#include "system/gamedata.h"
#include "gflib/strbuf_family.h"
#include "../field/fieldsys.h"
#include "savedata/frontier_savedata.h"
#include "savedata/frontier_ex.h"
#include "savedata/b_tower.h"
#include "field/eventflag.h"
#include "field/evwkdef.h"
#include "../field/sysflag.h"
#include "../field/syswork.h"

#include "application/stage.h"
#include "frontier_tool.h"
#include "frontier_def.h"
//#include "frontier_comm.h"
#include "stage_tool.h"
#include "comm_command_frontier.h"
#include "fssc_stage_sub.h"
#include "../field/scr_tool.h"		//StageScr_GetWinRecordID
#include "itemtool/itemsym.h"

#include "communication/comm_system.h"
#include "communication/comm_tool.h"
#include "communication/comm_def.h"
#include "communication/comm_info.h"


//============================================================================================
//
//	プロトタイプ宣言
//
//============================================================================================
STAGE_SCRWORK* StageScr_WorkAlloc( SAVEDATA* savedata, u16 init, u8 type, u8 pos, u8 pos2 );
void StageScr_WorkEnemySet( STAGE_SCRWORK* wk, u16 init );
static void StageScr_InitEnemySet( STAGE_SCRWORK* wk );
void StageScr_WorkRelease( STAGE_SCRWORK* wk );
void StageScr_GetResult( STAGE_SCRWORK* wk, void* stage_call );
u16 StageCall_GetRetWork( void* param, u8 pos );
void StageScr_StageRenshouCopyExtra( STAGE_SCRWORK* wk, u16* ret_wk );

//通信
BOOL StageScr_CommSetSendBuf( STAGE_SCRWORK* wk, u16 type, u16 param );

static u16 StageScr_TypeLevelRecordGet( SAVEDATA* sv, u8 type, u8 csr_pos, u16* l_num, u16* h_num );
static void StageScr_TypeLevelRecordSet( SAVEDATA* sv, u8 type, u8 csr_pos, u8 num );
void StageScr_TypeLevelMaxOff( STAGE_SCRWORK* wk );
static u16 StageScr_GetEnemyLevel( STAGE_SCRWORK* wk );


//============================================================================================
//
//	関数
//
//============================================================================================

//--------------------------------------------------------------
/**
 * @brief	ワークエリア確保
 *
 * @param	savedata		セーブデータのポインタ
 * @param	init			最初からか続きからか
 * @param	type			タイプ(シングル、ダブルなど)
 * @param	pos				選択したポケモン位置
 * @param	pos2			選択したポケモン位置
 * @param	none
 *	
 * ＊かならず StageScr_WorkRelease()で領域を開放すること
 */
//--------------------------------------------------------------
STAGE_SCRWORK* StageScr_WorkAlloc( SAVEDATA* savedata, u16 init, u8 type, u8 pos, u8 pos2 )
{
	STAGEDATA* stage_sv;
	STAGESCORE* score_sv;
	u8 clear_flag,level,m_max;
	u16	i,l_num,h_num,set_num;
	POKEMON_PARAM* temp_poke;
	static STAGE_SCRWORK* wk;					//あとで確認]]]]]]]]]]]]]]]]]]]]]]]]]

	wk = sys_AllocMemory( HEAPID_WORLD, sizeof(STAGE_SCRWORK) );
	MI_CpuClear8( wk, sizeof(STAGE_SCRWORK) );

	//セーブデータ取得
	wk->stage_savedata	= SaveData_GetStageData( savedata );
	wk->sv				= savedata;
	wk->heapID			= HEAPID_WORLD;
	wk->p_party			= PokeParty_AllocPartyWork( HEAPID_WORLD );		//パーティメモリ確保
	wk->pair_poke		= PokemonParam_AllocWork( HEAPID_WORLD );

	stage_sv = wk->stage_savedata;
	score_sv = SaveData_GetStageScore( savedata );

	//新規か、継続か
	if( init == 0 ){
		wk->type	= type;
		m_max		= Stage_GetMinePokeNum( wk->type );
		wk->round	= 0;

		STAGEDATA_Init( stage_sv );	//中断データ初期化

		//WIFIのみ特殊
		if( wk->type == STAGE_TYPE_WIFI_MULTI ){
#if 0
			clear_flag = FrontierRecord_Get(SaveData_GetFrontier(wk->sv), 
										FRID_STAGE_MULTI_WIFI_CLEAR_BIT,
										Frontier_GetFriendIndex(FRID_STAGE_MULTI_WIFI_CLEAR_BIT) );
#else
			clear_flag = SysWork_WifiFrClearFlagGet( SaveData_GetEventWork(wk->sv) );
#endif

		}else{
			//クリアしたかフラグを取得
			clear_flag = (u8)STAGESCORE_GetScoreData(	score_sv, STAGESCORE_ID_CLEAR_FLAG, 
														wk->type, 0, NULL );
		}

		if( clear_flag == 1 ){
			wk->rensyou = FrontierRecord_Get(	SaveData_GetFrontier(wk->sv), 
									StageScr_GetWinRecordID(wk->type),
									Frontier_GetFriendIndex(StageScr_GetWinRecordID(wk->type)) );
		}else{
			wk->rensyou = 0;

			//現在のバトルタイプのタイプレベルをクリアする
			for( i=0; i < STAGE_TR_TYPE_MAX ;i++ ){
				StageScr_TypeLevelRecordSet( wk->sv, wk->type, i, 0 );
			}
		}

		//選択した手持ちポケモンの位置
		wk->mine_poke_pos[0] = pos;
		wk->mine_poke_pos[1] = pos2;

	}else{
		//現在のプレイ進行データ取得
		wk->type	= (u8)STAGEDATA_GetPlayData( stage_sv, STAGEDATA_ID_TYPE, 0, 0, NULL );
		m_max = Stage_GetMinePokeNum( wk->type );
		wk->round	= (u8)STAGEDATA_GetPlayData( stage_sv, STAGEDATA_ID_ROUND,0, 0, NULL );
		wk->rensyou = FrontierRecord_Get(	SaveData_GetFrontier(wk->sv), 
									StageScr_GetWinRecordID(wk->type),
									Frontier_GetFriendIndex(StageScr_GetWinRecordID(wk->type)) );

		//---------------------------------------------------------------------------------------
		//保存しておいた手持ちポケモン位置を取得
		for( i=0; i < m_max ;i++ ){
			wk->mine_poke_pos[i] = 
				(u8)STAGEDATA_GetPlayData(stage_sv, STAGEDATA_ID_MINE_POKE_POS, i, 0, NULL );
			OS_Printf( "wk->mine_poke_pos[%d] = %d\n", i, wk->mine_poke_pos[i] );
		}

		//---------------------------------------------------------------------------------------
		//保存しておいた敵トレーナーのindex取得
		for( i=0; i < STAGE_LAP_MULTI_ENEMY_MAX ;i++ ){
			wk->tr_index[i] = (u16)STAGEDATA_GetPlayData(	stage_sv, STAGEDATA_ID_TR_INDEX, 
															i, 0, NULL);
			OS_Printf( "tr_index[%d] = %d\n", i, wk->tr_index[i] );
		}

		//---------------------------------------------------------------------------------------
		//保存しておいた敵ポケモンのindexを取得
		for( i=0; i < STAGE_LAP_MULTI_ENEMY_MAX ;i++ ){
			wk->enemy_poke_index[i] = (u8)STAGEDATA_GetPlayData( stage_sv, 
														STAGEDATA_ID_ENEMY_POKE_INDEX, 
														i, 0, NULL );
			OS_Printf( "wk->enemy_poke_index[%d] = %d\n", i, wk->enemy_poke_index[i] );
		}

	}

	//所持アイテムを保存しておく
	for( i=0; i < m_max; i++ ){
		temp_poke = PokeParty_GetMemberPointer( SaveData_GetTemotiPokemon(wk->sv), 
												wk->mine_poke_pos[i] );
		wk->itemno[i] = PokeParaGet( temp_poke, ID_PARA_item, NULL );
	}

	wk->win_cnt	= 0;
	wk->lap		= (u16)(wk->rensyou / STAGE_LAP_ENEMY_MAX);

	//通信マルチのみランクレベルを10固定にしてスタートさせる
	if( wk->type == STAGE_TYPE_MULTI ){

		//ここは全てのタイプを取得してセットなので(/2)しなくてよい
		for( i=0; i < STAGE_TR_TYPE_MAX ;i++ ){
			//★ここはレコードではない！
			Stage_SetTypeLevel( i, &wk->type_level[STAGE_TYPE_MULTI][0], (STAGE_TYPE_LEVEL_MAX-1) );
		}
	}else{

		//ここは全てのタイプを取得してセットなので(/2)しなくてよい
		for( i=0; i < STAGE_TR_TYPE_MAX ;i++ ){
			set_num = StageScr_TypeLevelRecordGet(	savedata, wk->type, i, &l_num, &h_num );

			//★ここはレコードではない！
			Stage_SetTypeLevel( i, &wk->type_level[wk->type][0], set_num );
		}
	}

	//地球儀登録
	if( Stage_CommCheck(wk->type) == TRUE ){
		FrontierTool_WifiHistory( wk->sv );
	}

	return wk;
}

//--------------------------------------------------------------
/**
 * @brief	敵トレーナーと敵ポケモンを選出する
 *
 * @param	savedata	セーブデータへのポインタ
 * @param	init		初期化モード BTWR_PLAY_NEW:初めから、BTWR_PLAY_CONTINE:続きから
 */
//--------------------------------------------------------------
void StageScr_WorkEnemySet( STAGE_SCRWORK* wk, u16 init )
{
	//OS_Printf( "前 継続 問題のwk = %d\n", wk );
	
	//新規か、継続か
	if( init == 0 ){
		StageScr_InitEnemySet( wk );
	}else{
		StageScr_InitEnemySet( wk );
	}

	//OS_Printf( "後 新規 問題のwk = %d\n", wk );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	新規：敵トレーナー、敵ポケモンデータを生成
 *
 * @param	wk		STAGE_SCRWORKへのポインタ
 */
//--------------------------------------------------------------
static void StageScr_InitEnemySet( STAGE_SCRWORK* wk )
{
	u8 level,set_num,set_pos,brain_count;
	int i,mine_lv;
	POKEMON_PARAM* temp_poke;
	B_TOWER_POKEMON poke;

	OS_Printf( "新規：データを生成\n" );

	brain_count = 0;

	//ポケモンタイプを選んでからでないとトレーナーのインデックスを決定することができない
	
	//トレーナーindex作成
	
	//決定するポケモン数がシングル以外は2匹になる
	set_num = 1;
	if( wk->type != STAGE_TYPE_SINGLE ){
		set_num = STAGE_ENTRY_POKE_MAX;
	}

	//★ここはレコードではない！
	level = Stage_GetTypeLevel( wk->csr_pos, &wk->type_level[wk->type][0] );

	//敵のレベルを取得
	wk->enemy_level = StageScr_GetEnemyLevel( wk );

	Stage_CreateTrType( wk->csr_pos, set_num, level, wk->round, wk->tr_index );
	Stage_CreateTrIndex( wk->type, set_num, wk->lap, level, wk->round, wk->tr_index );

	//被らないようにするために10匹最初に生成したい所だが、
	//どのポケモンタイプを選ぶかはその時でないとわからないので、
	//毎回生成する

	//ブレーンの時は、選出するランクを自分のレベルに合わせる
	set_pos = (wk->round * STAGE_ENTRY_POKE_MAX);		//セットする位置
	if( (wk->tr_index[set_pos] == STAGE_LEADER_TR_INDEX_1ST) ||
		(wk->tr_index[set_pos] == STAGE_LEADER_TR_INDEX_2ND) ){

		//手持ちポケモンのレベルからランクを取得
		mine_lv = Stage_GetMineLevelRank( wk );
		level = mine_lv;
		OS_Printf( "ブレーン登場時　手持ちから算出したランク = %d\n", level );
	}

	if( wk->tr_index[set_pos] == STAGE_LEADER_TR_INDEX_1ST ){
		brain_count = 1;
	}

	if( wk->tr_index[set_pos] == STAGE_LEADER_TR_INDEX_2ND ){
		brain_count = 2;
	}

	//ポケモンindex作成
	temp_poke = PokeParty_GetMemberPointer( wk->p_party, 0 );

	//生成するポケモンは1つでよい(同じポケモンを出す仕組みなので)
	//Stage_CreatePokeIndex(	set_num, wk->poke_type, level, wk->round,
	Stage_CreatePokeIndex(	1, wk->poke_type, level, wk->round,
							PokeParaGet(temp_poke,ID_PARA_monsno,NULL),
							wk->enemy_poke_index, brain_count );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	ワークエリアを開放する
 */
//--------------------------------------------------------------
void StageScr_WorkRelease( STAGE_SCRWORK* wk )
{
	if( wk == NULL ){
		return;
	}

	if( wk->p_party != NULL ){
		sys_FreeMemoryEz( wk->p_party );
	}

	if( wk->pair_poke != NULL ){
		sys_FreeMemoryEz( wk->pair_poke );
	}

	MI_CpuClear8( wk, sizeof(STAGE_SCRWORK) );
	sys_FreeMemoryEz( wk );
	wk = NULL;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	タイプ選択画面呼び出し後の結果取得
 *
 * @param	wk		STAGE_SCRWORKのポインタ
 */
//--------------------------------------------------------------
void StageScr_GetResult( STAGE_SCRWORK* wk, void* stage_call )
{
	int i;

	//結果を取得する
	wk->ret_work	= StageCall_GetRetWork( stage_call, 0 );
	wk->csr_pos		= wk->ret_work;								//カーソル位置
	wk->poke_type	= Stage_GetPokeType( wk->ret_work );		//カーソル位置のポケモンタイプ取得

	//カーソル位置を調整
	if( wk->csr_pos >= 17 ){
		wk->csr_pos = STAGE_TR_TYPE_MAX - 1;					//17
	}

	OS_Printf( "wk->ret_work = %d\n", wk->ret_work );
	return;
}

//----------------------------------------------------------------------------
/**
 * @brief	バトルステージのポケモン選択画面での結果を取得
 *
 * @param	param	STAGE_CALL_WORKのポインタ
 * @param	pos		ret_work[pos](0-5)
 *
 * @return	"結果"
 */
//----------------------------------------------------------------------------
u16 StageCall_GetRetWork( void* param, u8 pos )
{
	STAGE_CALL_WORK* stage_call = param;
	return stage_call->ret_work;
}

//--------------------------------------------------------------
/**
 * @brief   バトルステージの外部連勝記録を書き込み、セーブを行う
 *
 * @param	wk		STAGE_SCRWORKのポインタ
 */
//--------------------------------------------------------------
void StageScr_StageRenshouCopyExtra( STAGE_SCRWORK* wk, u16* ret_wk )
{
	LOAD_RESULT load_result;
	SAVE_RESULT save_result;

	//外部セーブファイルをロード
	//常駐に配置されているフロンティアレコードからセットする値を取得
	//外部セーブの値と比較し、大きければセット　＞　外部セーブ実行

	//WIFI以外は処理する
	if( wk->type != STAGE_TYPE_WIFI_MULTI ){
		*ret_wk = FrontierEx_StageRenshou_RenshouCopyExtra( wk->sv, 
									StageScr_GetMaxWinRecordID(wk->type),
									StageScr_GetMonsNoRecordID(wk->type),
									Frontier_GetFriendIndex(StageScr_GetMaxWinRecordID(wk->type)),
									StageScr_GetExMaxWinRecordID(wk->type),
									HEAPID_WORLD, &load_result, &save_result );
		OS_Printf( "ロード結果 = %d\n", load_result );
		OS_Printf( "セーブ結果 = %d\n", save_result );
	}else{
		*ret_wk = FALSE;
	}

	return;
}


/*****************************************************/
/*****************************************************/
/*****************************************************/
/*****************************************************/
/*****************************************************/
/*****************************************************/
/*****************************************************/
/*****************************************************/
/*****************************************************/
/*****************************************************/
/*****************************************************/
/*****************************************************/
void StageScr_SaveRestPlayData( STAGE_SCRWORK* wk, u8 mode );
//void StageScr_SetNG( STAGE_SCRWORK* wk );
u16	StageScr_IncRound( STAGE_SCRWORK* wk );
u16	StageScr_GetRound( STAGE_SCRWORK* wk );
u16 StageScr_GetEnemyObjCode( STAGE_SCRWORK* wk, u8 param );
void StageScr_SetLose( STAGE_SCRWORK* wk );
void StageScr_SetClear( STAGE_SCRWORK* wk );

//--------------------------------------------------------------
/**
 * @brief	休むときに現在のプレイ状況をセーブに書き出す
 *
 * @param	wk		STAGE_SCRWORKのポインタ
 * @param	mode	FR_MODE_CLEAR="クリア",FR_MODE_LOSE="負け",FR_MODE_REST="休む"
 */
//--------------------------------------------------------------
void StageScr_SaveRestPlayData( STAGE_SCRWORK* wk, u8 mode )
{
	u16	i;
	u8	buf8[4];
	u16	buf16[4];
	u32	buf32[4];
	FRONTIER_SAVEWORK* fr_sv;
	u16 l_num,h_num,set_num,record_monsno,now_monsno;
	POKEMON_PARAM* temp_poke;
	STAGEDATA* f_sv = wk->stage_savedata;
	STAGESCORE* score_sv = SaveData_GetStageScore( wk->sv );

	fr_sv = SaveData_GetFrontier( wk->sv );

	//"シングル、ダブル、マルチ、wifiマルチ"書き出し
	buf8[0] = wk->type;
	STAGEDATA_PutPlayData( wk->stage_savedata, STAGEDATA_ID_TYPE, 0, 0, buf8 );
	
	//セーブフラグを有効状態にリセット
	STAGEDATA_SetSaveFlag( wk->stage_savedata, TRUE );

	//現在挑戦中のポケモン
	temp_poke = PokeParty_GetMemberPointer( SaveData_GetTemotiPokemon(wk->sv), 
											wk->mine_poke_pos[0] );
	now_monsno = PokeParaGet( temp_poke, ID_PARA_monsno, NULL );

	//"連勝中のポケモンナンバー"取得
	record_monsno = FrontierRecord_Get(	fr_sv, 
						StageScr_GetMonsNoRecordID(wk->type),
						Frontier_GetFriendIndex(StageScr_GetMonsNoRecordID(wk->type)) );

	//"ラウンド数"書き出し(0-17人目の何人目かをあわらす)
	buf8[0] = wk->round;
	//OS_Printf( "wk->round = %d\n", wk->round );
	//OS_Printf( "buf8[0] = %d\n", buf8[0] );
	STAGEDATA_PutPlayData( wk->stage_savedata, STAGEDATA_ID_ROUND, 0, 0, buf8 );

#if 0
	//連勝 / 10 = 周回数
	//連勝 % 10 = 何人目か
#endif
	//"連勝数"書き出し(「次は27人目です」というように使う)
	//OS_Printf( "rensyou = %d\n", wk->rensyou );
	FrontierRecord_Set(	fr_sv, StageScr_GetWinRecordID(wk->type), 
						Frontier_GetFriendIndex(StageScr_GetWinRecordID(wk->type)), wk->rensyou );
	
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//"休む"以外
	if( mode != FR_MODE_REST ){

		//ここは1匹分しか領域ないので必ず上書きする
		//
		//StageScr_StageRenshouCopyExtra(...)の中で、今(↑)と外部データに保存されているデータを、
		//比較して、外部データの方にも今を上書きするか判別している
		//
		//この領域のデータは成績ボードでも見ないので大丈夫なはず。

		//WIFI以外
		if( wk->type != STAGE_TYPE_WIFI_MULTI ){

			//1匹分しか領域ないので必ず上書きする
			FrontierRecord_Set(	fr_sv, 
								StageScr_GetMaxWinRecordID(wk->type),
								Frontier_GetFriendIndex(StageScr_GetMaxWinRecordID(wk->type)),
								wk->rensyou );

		}else{

			//記録のポケモンと、現在のポケモンが一致しない時
			if( record_monsno != now_monsno ){

				//1匹分しか領域ないので必ず上書きする
				FrontierRecord_Set(	fr_sv, 
									StageScr_GetMaxWinRecordID(wk->type),
									Frontier_GetFriendIndex(StageScr_GetMaxWinRecordID(wk->type)),
									wk->rensyou );
			}else{

				//同じなら、比較してセット
				FrontierRecord_SetIfLarge(	fr_sv,
									StageScr_GetMaxWinRecordID(wk->type),
									Frontier_GetFriendIndex(StageScr_GetMaxWinRecordID(wk->type)),
									wk->rensyou );
			}
		}

		//"10連勝(クリア)したかフラグ"書き出し
		buf8[0] = wk->clear_flag;
		STAGESCORE_PutScoreData( score_sv, STAGESCORE_ID_CLEAR_FLAG, wk->type, 0, buf8 );

		//WIFIのみ特殊
		if( wk->type == STAGE_TYPE_WIFI_MULTI ){
			FrontierRecord_Set(	fr_sv, 
						FRID_STAGE_MULTI_WIFI_CLEAR_BIT,
						Frontier_GetFriendIndex(FRID_STAGE_MULTI_WIFI_CLEAR_BIT), wk->clear_flag );
		}
	}
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

	//"敵トレーナーインデックス"書き出し
	for( i=0; i < STAGE_LAP_MULTI_ENEMY_MAX ;i++ ){
		buf16[0] = wk->tr_index[i];
		STAGEDATA_PutPlayData( wk->stage_savedata, STAGEDATA_ID_TR_INDEX, i, 0, buf16 );
	}

	//"参加している手持ちポケモンの位置"書き出し
	for( i=0; i < STAGE_ENTRY_POKE_MAX ;i++ ){
		buf8[0] = wk->mine_poke_pos[i];
		STAGEDATA_PutPlayData( wk->stage_savedata, STAGEDATA_ID_MINE_POKE_POS, i, 0, buf8 );
	}

	//ここは全てのタイプをセットなので(/2)しなくてよい
	//"タイプごとのレベル"書き出し
	for( i=0; i < STAGE_TR_TYPE_MAX ;i++ ){
		//★ここはレコードではない！
		buf8[0] = Stage_GetTypeLevel( i, &wk->type_level[wk->type][0] );
		StageScr_TypeLevelRecordSet( wk->sv, wk->type, i, buf8[0] );
	}
	
	//"敵ポケモンデータが被らないようにする"書き出し
	for( i=0; i < STAGE_LAP_MULTI_ENEMY_MAX ;i++ ){
		buf16[0] = wk->enemy_poke_index[i];
		STAGEDATA_PutPlayData( wk->stage_savedata, STAGEDATA_ID_ENEMY_POKE_INDEX, i, 0, buf16 );
	}

	//"連勝中のポケモンナンバー"書き出し
	FrontierRecord_Set(	fr_sv, 
						StageScr_GetMonsNoRecordID(wk->type),
						Frontier_GetFriendIndex(StageScr_GetMonsNoRecordID(wk->type)), 
						PokeParaGet(temp_poke,ID_PARA_monsno,NULL) );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	ラウンド数をインクリメント
 *
 * @param	wk		STAGE_SCRWORKのポインタ
 */
//--------------------------------------------------------------
u16	StageScr_IncRound( STAGE_SCRWORK* wk )
{
	wk->round++;
	return wk->round;
}

//--------------------------------------------------------------
/**
 * @brief	ラウンド数を取得
 *
 * @param	wk		STAGE_SCRWORKのポインタ
 */
//--------------------------------------------------------------
u16	StageScr_GetRound( STAGE_SCRWORK* wk )
{
	return wk->round;
}

//--------------------------------------------------------------
/**
 * @brief	対戦トレーナーOBJコード取得
 *
 * @param	wk		STAGE_SCRWORKのポインタ
 * @param	param	0=1人目、1=2人目(敵のパートナ－)
 */
//--------------------------------------------------------------
u16 StageScr_GetEnemyObjCode( STAGE_SCRWORK* wk, u8 param )
{
	B_TOWER_TRAINER bt_trd;
	B_TOWER_TRAINER_ROM_DATA* p_rom_tr;
	u8 index;

	//取得するトレーナーデータのインデックスを取得
	index = (wk->round * STAGE_ENTRY_POKE_MAX) + param;

	//ROMからトレーナーデータを確保
	p_rom_tr = Frontier_TrainerDataGet( &bt_trd, wk->tr_index[index], HEAPID_WORLD, ARC_PL_BTD_TR );
	sys_FreeMemoryEz( p_rom_tr );

	//トレーナータイプからOBJコードを取得してくる
	return Frontier_TrType2ObjCode( bt_trd.tr_type );
}

//--------------------------------------------------------------
/**
 * @brief	敗戦した時の処理
 *
 * @param	wk		STAGE_SCRWORKのポインタ
 */
//--------------------------------------------------------------
void StageScr_SetLose( STAGE_SCRWORK* wk )
{
	int i;
	OS_Printf( "\nバトルステージデータ　敗戦セット\n" );

	//★ここはレコードではない！
	//レベルをクリア
	Stage_ClearTypeLevel( &wk->type_level[wk->type][0] );	//STAGE_SCRWORK

	//現在の5連勝などを保存する必要がある！
	StageScr_SaveRestPlayData( wk, FR_MODE_LOSE );		//セーブデータに代入

	//新規か、継続かは、WK_SCENE_STAGE_LOBBYに格納されているので、
	//ここでroundなどをクリアしなくても、
	//WK_SCENE_STAGE_LOBBYが継続ではない状態でセーブされるので、
	//受付に話しかけても、新規判定になりワークはクリアされる。
	return;
}

//--------------------------------------------------------------
/**
 * @brief	10連勝(クリア)した時の処理
 *
 * @param	wk		STAGE_SCRWORKのポインタ
 */
//--------------------------------------------------------------
void StageScr_SetClear( STAGE_SCRWORK* wk )
{
	POKEMON_PARAM* temp_poke;
	OS_Printf( "\nバトルステージデータ　10連勝(クリア)セット\n" );
	
	wk->clear_flag = 1;						//10連勝(クリア)したかフラグON

	//連勝中のポケモンナンバー
	//temp_poke = PokeParty_GetMemberPointer( SaveData_GetTemotiPokemon(wk->sv), 
	//										wk->mine_poke_pos[0] );
	//wk->clear_monsno[wk->type] = PokeParaGet( temp_poke, ID_PARA_monsno, NULL );

	//wk->rensyou		= 0;				//現在の連勝数
	if( wk->lap < STAGE_LAP_MAX ){
		wk->lap++;							//周回数のインクリメント
	}

#if 0
	//StageScr_CommGetLapでずれが出ないようにペアも更新
	if( wk->pair_lap < STAGE_LAP_MAX ){
		wk->pair_lap++;						//周回数のインクリメント
	}
#endif

	wk->round			= 0;				//今何人目？
	StageScr_SaveRestPlayData( wk, FR_MODE_CLEAR );		//セーブデータに代入
	return;
}


//==============================================================================================
//
//	通信(CommStart)
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	送信ウェイト　
 *
 * @param	wk			STAGE_SCRWORK型のポインタ
 * @param	type		送信タイプ
 *
 * @retval	none
 */
//--------------------------------------------------------------
BOOL StageScr_CommSetSendBuf( STAGE_SCRWORK* wk, u16 type, u16 param )
{
	int ret;

	switch( type ){

	//
	case STAGE_COMM_BASIC:
		ret = CommStageSendBasicData( wk );
		break;

	case STAGE_COMM_TR:
		ret = CommStageSendTrData( wk );
		break;

	case STAGE_COMM_ENEMY:
		ret = CommStageSendEnemyPokeData( wk );
		break;

	case STAGE_COMM_RETIRE:
		ret = CommStageSendRetireFlag( wk, param );
		break;

	case STAGE_COMM_TEMOTI:
		ret = CommStageSendBufTemotiPokeData( wk );
		break;

	};

	return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * ステージクリア時に貰えるバトルポイントを取得
 *
 * @param	wk
 *
 * @return	"貰えるバトルポイント"
 *
 * 通信マルチは固定。シングル、ダブルは自分のみ。
 * WIFIは相手と周回数同じ。
 * なので周回数チェックはいらない。
 */
//--------------------------------------------------------------------------------------------
u16	StageScr_GetAddBtlPoint( STAGE_SCRWORK* wk );
u16	StageScr_GetAddBtlPoint( STAGE_SCRWORK* wk )
{
	u8 add_bp;
	static const u8 bppoint_normal[STAGE_LAP_MAX+1] = {0,
									1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 
									4, 6, 6, 8, 8, 10, 10, 12 };		//0,1周～18周以降

	static const u8 bppoint_multi[STAGE_LAP_MAX+1] = {0,
									6, 6, 6, 8, 8, 8, 10, 10, 10, 12, 
									12, 14, 15, 17, 17, 20, 20, 23 };	//0,1周～18周以降

	OS_Printf( "周回数 = %d\n", wk->lap );
	OS_Printf( "連勝数 = %d\n", wk->rensyou );

	//シングル、ダブル
	if( (wk->type == STAGE_TYPE_SINGLE) || (wk->type == STAGE_TYPE_DOUBLE) ){

		//最大周回数チェック
		if( wk->lap >= STAGE_LAP_MAX ){
			add_bp = bppoint_normal[ STAGE_LAP_MAX ];		//テーブルが+1しているので
		}else{
			add_bp = bppoint_normal[ wk->lap ];
		}

	//マルチ、WIFI
	}else{

		//最大周回数チェック
		if( wk->lap >= STAGE_LAP_MAX ){
			add_bp = bppoint_multi[ STAGE_LAP_MAX ];		//テーブルが+1しているので
		}else{
			add_bp = bppoint_multi[ wk->lap ];
		}
	}

	//ブレーンチェック
	if( wk->type == STAGE_TYPE_SINGLE ){
		if( (wk->rensyou == STAGE_LEADER_SET_1ST) || (wk->rensyou == STAGE_LEADER_SET_2ND) ){
			add_bp = 20;
		}

	//通信マルチはBP固定！
	}else if( wk->type == STAGE_TYPE_MULTI ){
		add_bp = 12;
	}

	return add_bp;
}


//==============================================================================================
//
//	タイプレベル関連
//
//==============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * タイプレベルのレコードを取得
 *
 * @param	wk
 *
 * @return	"レコード"
 */
//--------------------------------------------------------------------------------------------
static u16 StageScr_TypeLevelRecordGet( SAVEDATA* sv, u8 type, u8 csr_pos, u16* l_num, u16* h_num )
{
	u16 now_type_level;

	//現在のタイプレベルを取得
	now_type_level = FrontierRecord_Get(SaveData_GetFrontier(sv), 
							StageScr_GetTypeLevelRecordID(type,csr_pos),
							Frontier_GetFriendIndex(StageScr_GetTypeLevelRecordID(type,csr_pos)) );

	now_type_level &= 0xff;				//1byteしか使わない
	*l_num = (now_type_level & 0xf);	//タイプ[0]
	*h_num = (now_type_level >> 4);		//タイプ[1]というように格納されている

	OS_Printf( "\n**************************\n" );
	OS_Printf( "csr_pos = %d\n", csr_pos );
	OS_Printf( "now_type_level = %d\n", now_type_level );
	OS_Printf( "*l_num = %d\n", *l_num );
	OS_Printf( "*h_num = %d\n", *h_num );

	if( (csr_pos % 2) == 0 ){
		return *l_num;
	}else{
		return *h_num;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * タイプレベルのレコードをセット
 *
 * @param	wk
 *
 * @return	"レコード"
 */
//--------------------------------------------------------------------------------------------
static void StageScr_TypeLevelRecordSet( SAVEDATA* sv, u8 type, u8 csr_pos, u8 num )
{
	u8 offset;
	u8 param;
	u8 set_num,total;
	u16 l_num,h_num;

	OS_Printf( "レコード変更前の値\n" );
	StageScr_TypeLevelRecordGet( sv, type, csr_pos, &l_num, &h_num );
	total = ( (h_num << 4) | l_num );
	OS_Printf( "total = %d\n", total );

	offset	= ( csr_pos / 2 );			//フラグのオフセット[0,1][2,3],,,
	param	= ( csr_pos % 2 );			//フラグのどちらの4bitか取得

	//セットするほうのみ消す
	if( param == 0 ){
		total = total & 0xf0;
	}else{
		total = total & 0x0f;
	}
	OS_Printf( "total = %d\n", total );

	//セットする値をビット調整
	set_num = (num << (4 * param));
	OS_Printf( "set_num = %d\n", set_num );

	//格納
	total |= set_num;
	OS_Printf( "total = %d\n", total );

	FrontierRecord_Set(	SaveData_GetFrontier(sv), 
					StageScr_GetTypeLevelRecordID(type,csr_pos),
					Frontier_GetFriendIndex(StageScr_GetTypeLevelRecordID(type,csr_pos)), total );

#ifdef PM_DEBUG
	OS_Printf( "レコード変更後の値\n" );
	StageScr_TypeLevelRecordGet( sv, type, csr_pos, &l_num, &h_num );
#endif

	return;
}

//--------------------------------------------------------------
/**
 * @brief	全てのタイプのランクが挑戦済みの最大になっていたら挑戦していない状態にする
 *
 * @param	wk		STAGE_SCRWORKのポインタ
 *
 * ★レコードではない
 */
//--------------------------------------------------------------
void StageScr_TypeLevelMaxOff( STAGE_SCRWORK* wk )
{
	u8 rank;
	int i;

	//マルチ以外の時
	if( wk->type != STAGE_TYPE_MULTI ){

		//「???」は抜かすので17
		//for( i=0; i < STAGE_TR_TYPE_MAX ;i++ ){
		for( i=0; i < (STAGE_TR_TYPE_MAX-1) ;i++ ){

			//★ここはレコードではない！
			rank = Stage_GetTypeLevel(	i, &wk->type_level[wk->type][0] );
			if( rank < STAGE_TYPE_LEVEL_MAX ){
				break;
			}
		}

		//全てのタイプのランクが挑戦済みの最大になっていた
		//if( i == STAGE_TR_TYPE_MAX ){
		if( i == (STAGE_TR_TYPE_MAX-1) ){

			//挑戦していない最大にする
			//for( i=0; i < STAGE_TR_TYPE_MAX ;i++ ){
			for( i=0; i < (STAGE_TR_TYPE_MAX-1) ;i++ ){
				//★ここはレコードではない！
				Stage_SetTypeLevel( i, &wk->type_level[wk->type][0], 
									(STAGE_TYPE_LEVEL_MAX-1) );
			}
		}
	}

#if 0	//デバックセット

	//挑戦していない最大にする
	//for( i=0; i < STAGE_TR_TYPE_MAX ;i++ ){
	for( i=0; i < (STAGE_TR_TYPE_MAX-1) ;i++ ){
		//★ここはレコードではない！
		Stage_SetTypeLevel( i, &wk->type_level[wk->type][0], 
							(STAGE_TYPE_LEVEL_MAX-1) );
	}
#endif

	return;
}

//--------------------------------------------------------------
/**
 * @brief	平方根を計算
 *
 * @param	wk		STAGE_SCRWORKのポインタ
 */
//--------------------------------------------------------------
void StageScr_Sqrt( STAGE_SCRWORK* wk )
{
	int poke_level;

	//一番高いポケモンのレベルを取得
	poke_level = StageScr_CommGetPokeLevel( wk );
	OS_Printf( "一番高いpoke_level = %d\n", poke_level );

	//平方根
	wk->sqrt = StageScr_GetSqrt32( poke_level );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	レベルを決定
 *
 * @param	wk		STAGE_SCRWORKのポインタ
 */
//--------------------------------------------------------------
static u16 StageScr_GetEnemyLevel( STAGE_SCRWORK* wk )
{
	fx32 start_level,temp1;
	int poke_level,i;
	int ret_level;
	u8 level;
	float temp_f,hosei_zouka;

	//OS_Printf( "temp_f = %d\n", temp3 );		//10進
	//OS_Printf( "temp_f = %x\n", temp3 );		//16進
	//OS_Printf( "temp_f = %f\n", temp3 );		//小数
	//FX_FX32_TO_F32(start_level)				//fx32→f32

	//★ここはレコードではない！
	level = Stage_GetTypeLevel( wk->csr_pos, &wk->type_level[wk->type][0] );
	OS_Printf( "タイプのlevel = %x\n", level );

	//一番高いポケモンのレベルを取得
	poke_level = StageScr_CommGetPokeLevel( wk );
	OS_Printf( "一番高いpoke_level = %d\n", poke_level );

	//マルチの時は、計算はしなくてよい
	if( wk->type == STAGE_TYPE_MULTI ){
		OS_Printf( "マルチの敵のレベル = %d\n", poke_level );
		return poke_level;
	}

	///////////////////////////////////////////////////////////////////////////
	//初期レベル設定
	start_level = ( FX32_CONST(poke_level) - (wk->sqrt * 3) );
	OS_Printf( "初期レベル = %x\n", start_level );

	///////////////////////////////////////////////////////////////////////////
	//同タイプ選択補正(0オリジンなので+1する)
	temp_f = ( FX_FX32_TO_F32(wk->sqrt) * 5.0 );

	OS_Printf( "(poke_level / temp_f) = %f\n", (poke_level / temp_f) );
	if( (poke_level / temp_f) < 1.0 ){
		temp_f = 1.0;
		OS_Printf( "1未満 同タイプ選択補正 = %f\n", temp_f );
		temp_f = (float)(level+1-1);
	}else{
		temp1 = FX32_CONST( (level+1-1) * poke_level );
		OS_Printf( "temp1 = %x\n", temp1 );

		temp_f = ( FX_FX32_TO_F32(temp1) / temp_f );
		OS_Printf( "temp_f = %f\n", (FX_FX32_TO_F32(temp1) / temp_f) );		//OK 49/35=1.4
	}
	OS_Printf( "同タイプ選択補正 = %f\n", temp_f );

	///////////////////////////////////////////////////////////////////////////
	//一度でも選んだタイプの数を取得
	hosei_zouka = 0.0;
	for( i=0; i < STAGE_TR_TYPE_MAX ;i++ ){

		//戦闘勝利後にタイプレベルを上げているので、
		//今選択しているカーソル位置は無条件でカウントするようにしている
		if( i == wk->csr_pos ){
			hosei_zouka+=1.0;
			OS_Printf( "今選択している位置はカウントする\n" );
		}else{
			//★ここはレコードではない！
			if( Stage_GetTypeLevel(i,&wk->type_level[wk->type][0]) > 0 ){
				hosei_zouka+=1.0;
				OS_Printf( "hosei_zouka ++\n" );
			}
		}
	}
	//タイプ増加選択補正
	if( hosei_zouka != 0.0 ){
		hosei_zouka-=1.0;
	}
	OS_Printf( "タイプ増加選択補正 = %d\n", hosei_zouka );
	hosei_zouka*=0.5;
	OS_Printf( "タイプ増加選択補正 = %d\n", hosei_zouka );

	///////////////////////////////////////////////////////////////////////////
	OS_Printf( "to_f32 start_level = %f\n", (FX_FX32_TO_F32(start_level)) );
	OS_Printf( "temp_f = %f\n", temp_f );
	OS_Printf( "hosei_zouka = %f\n", hosei_zouka );
	temp_f = (	FX_FX32_TO_F32(start_level) + temp_f + hosei_zouka );

	OS_Printf( "result temp_f = %f\n", temp_f );
	OS_Printf( "10 int result temp_f = %d\n", (int)temp_f );

	ret_level = (int)(temp_f);

#if 1
	if( temp_f != (int)temp_f ){
		ret_level++; 
		OS_Printf( "繰上げ\n" );
	}
#else
	if( (float)(temp_f - (int)temp_f) >= 0.5 ){
		ret_level++; 
		OS_Printf( "繰上げ\n" );
	}
#endif

	//自分のポケモンのレベルを上回ったら、自分のポケモンのレベルにする
	if( ret_level > poke_level ){
		ret_level = poke_level;
	}

	if( ret_level > 100 ){
		ret_level = 100;
	}

	OS_Printf( "敵のレベル = %d\n", ret_level );
	return ret_level;
}



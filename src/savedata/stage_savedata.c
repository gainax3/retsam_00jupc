//==============================================================================================
/**
 * @file	stage_savedata.c
 * @brief	「バトルステージ」セーブデータ
 * @author	Satoshi Nohara
 * @date	2007.06.07
 */
//==============================================================================================
#include "common.h"
#include "savedata/savedata_def.h"
#include "savedata/savedata.h"
#include "savedata/frontier_savedata.h"
#include "savedata/stage_savedata.h"
#include "frontier_savedata_local.h"
#include "stage_savedata_local.h"
#include "gflib/assert.h"
#include "gflib/heapsys.h"
#include "../frontier/stage_def.h"


//============================================================================================
//
//	プロトタイプ宣言
//
//============================================================================================
int STAGEDATA_GetWorkSize( void );
void STAGEDATA_Init( STAGEDATA* wk );
STAGEDATA * SaveData_GetStageData( SAVEDATA * sv );

BOOL STAGEDATA_GetSaveFlag( STAGEDATA* wk );
void STAGEDATA_SetSaveFlag( STAGEDATA* wk, BOOL flag );
void STAGEDATA_PutPlayData( STAGEDATA* wk, u8 id, u8 param, u8 param2, const void* buf );
u32 STAGEDATA_GetPlayData( STAGEDATA* wk, u8 id, u8 param, u8 param2, void* buf );

u8 Stage_GetTypeLevel( u8 csr_pos, u8* p_rank );
void Stage_SetTypeLevel( u8 csr_pos, u8* p_rank, u8 num );
void Stage_ClearTypeLevel( u8* p_rank );

//成績
int STAGESCORE_GetWorkSize( void );
void STAGESCORE_Init( STAGESCORE* wk );
STAGESCORE * SaveData_GetStageScore( SAVEDATA * sv );
void STAGESCORE_PutScoreData( STAGESCORE* wk, u8 id, u8 param, u8 param2, const void* buf );
u32 STAGESCORE_GetScoreData( STAGESCORE* wk, u8 id, u8 param, u8 param2, void* buf );


//============================================================================================
//
//	セーブデータシステムが依存する関数
//
//============================================================================================

//----------------------------------------------------------
/**
 * @brief	(中断データ)ステージセーブワークのサイズ取得
 *
 * @param	none
 *
 * @return	int		サイズ(バイト単位)
 */
//----------------------------------------------------------
int STAGEDATA_GetWorkSize( void )
{
	return sizeof(STAGEDATA);
}

//----------------------------------------------------------
/**
 * @brief	(中断データ)ステージワークの初期化
 *
 * @param	wk		STAGEDATAへのポインタ
 *
 * @return	none
 */
//----------------------------------------------------------
void STAGEDATA_Init( STAGEDATA* wk )
{
	//MI_CpuClear32( wk, sizeof(STAGEDATA) );
	MI_CpuClear8( wk, sizeof(STAGEDATA) );
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
	SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
	return;
}

//----------------------------------------------------------
/**
 * @brief	ステージセーブデータの取得(プレイデータ)
 *
 * @param	sv		セーブデータへのポインタ
 *
 * @return	STAGEDATAへのポインタ
 */
//----------------------------------------------------------
STAGEDATA * SaveData_GetStageData( SAVEDATA * sv )
{
	//return (STAGEDATA*)SaveData_Get( sv, GMDATA_ID_STAGE );
	FRONTIER_SAVEWORK* data = SaveData_Get( sv, GMDATA_ID_FRONTIER );
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
	SVLD_CheckCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
	return &data->play_stage;
}


//============================================================================================
//
//	メンバアクセス
//
//============================================================================================

//----------------------------------------------------------
/**
 * @brief	プレイデータ　正しくセーブ済みかどうか？
 *
 * @param	wk		STAGEDATA型のポインタ
 *
 * @retval	TRUE	正しくセーブされている
 * @retval	FALSE	セーブされていない
 */
//----------------------------------------------------------
BOOL STAGEDATA_GetSaveFlag( STAGEDATA* wk )
{
	return wk->save_flag;
}

//----------------------------------------------------------
/**
 * @brief	プレイデータ　セーブ状態フラグをセット
 *
 * @param	wk		STAGEDATA型のポインタ
 * @param	flag	TRUE、FALSE
 *
 * @retval	none
 */
//----------------------------------------------------------
void STAGEDATA_SetSaveFlag( STAGEDATA* wk, BOOL flag )
{
	wk->save_flag = flag;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
	SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
	return;
}

//----------------------------------------------------------
/**
 * @brief	プレイデータ　セット
 *
 * @param	wk		STAGEDATA型のポインタ
 * @param	id		取得するデータID
 * @param	buf		void*:データを格納したバッファへのポインタ
 *
 * @retval	none
 */
//----------------------------------------------------------
void STAGEDATA_PutPlayData( STAGEDATA* wk, u8 id, u8 param, u8 param2, const void* buf )
{
	u32	*buf32	= (u32 *)buf;
	u16	*buf16	= (u16 *)buf;
	u8	*buf8	= (u8 *)buf;

	switch( id ){

	//シングル、ダブル、マルチ、wifiマルチ
	case STAGEDATA_ID_TYPE:
		wk->type = buf8[0];
		OS_Printf( "wk->type = %d\n", wk->type );
		break;

	//今何人目？
	case STAGEDATA_ID_ROUND:
		wk->round = buf8[0];
		OS_Printf( "wk->round = %d\n", wk->round );
		break;

	//敵トレーナーデータインデックス
	case STAGEDATA_ID_TR_INDEX:
		wk->tr_index[param] = buf16[0];
		OS_Printf( "wk->tr_index[%d] = %d\n", param, wk->tr_index[param] );
		break;

	//参加している手持ちポケモンの位置情報
	case STAGEDATA_ID_MINE_POKE_POS:
		wk->mine_poke_pos[param] = buf8[0];
		OS_Printf( "wk->mine_poke_pos[%d] = %d\n", param, wk->mine_poke_pos[param] );
		break;

	//敵ポケモンデータが被らないようにするために保存
	case STAGEDATA_ID_ENEMY_POKE_INDEX:
		wk->enemy_poke_index[param] = buf16[0];
		OS_Printf("wk->enemy_poke_index[%d] = %d\n", param, wk->enemy_poke_index[param] );
		break;

	default:
		GF_ASSERT(0);
		break;
	};

#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
	SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
	return;
}

//----------------------------------------------------------
/**
 * @brief	プレイデータ　取得
 *
 * @param	wk		STAGEDATA型のポインタ
 * @param	id		取得するデータID
 * @param	buf		void*:データ取得ポインタ
 *
 * @retval	none
 *
 * 戻り値を取得する時に型キャストを使用しているのでコピペ注意！
 */
//----------------------------------------------------------
u32 STAGEDATA_GetPlayData( STAGEDATA* wk, u8 id, u8 param, u8 param2, void* buf )
{
	switch( id ){

	//シングル、ダブル、マルチ、wifiマルチ
	case STAGEDATA_ID_TYPE:
		return (u32)wk->type;

	//今何人目？
	case STAGEDATA_ID_ROUND:
		return (u32)wk->round;

	//敵トレーナーデータインデックス
	case STAGEDATA_ID_TR_INDEX:
		return (u32)wk->tr_index[param];

	//参加している手持ちポケモンの位置情報
	case STAGEDATA_ID_MINE_POKE_POS:
		return wk->mine_poke_pos[param];

	//敵ポケモンデータが被らないようにするために保存
	case STAGEDATA_ID_ENEMY_POKE_INDEX:
		return wk->enemy_poke_index[param];

	default:
		GF_ASSERT(0);
		break;
	};

	return 0;
}


//============================================================================================
//
//	タイプごとのレベルが4bitで持つことになったのでアクセス関数
//
//	タイプレベルがレコードになったので、
//	この関数を使ってセーブデータへはアクセスしていない。
//	STAGE_SCRWORKのtype_levelの操作のみに使用している(はず)。
//
//	タイプレベルのレコードへはfssc_stage_sub.cにあるStageScr_TypeLevelRecordGet,Setを使用。
//
//============================================================================================

//--------------------------------------------------------------
/**
 * @brief   タイプごとのレベルを取得
 *
 * @param   csr_pos		カーソル位置
 * @param   p_rank		ランクデータのポインタ
 *
 * @retval  "レベル"
 */
//--------------------------------------------------------------
u8 Stage_GetTypeLevel( u8 csr_pos, u8* p_rank )
{
	u8 offset;
	u8 param;
	u8 ret;

	offset	= ( csr_pos / 2 );			//フラグのオフセット[0,1][2,3],,,
	param	= ( csr_pos % 2 );			//フラグのどちらの4bitか取得

	ret = (p_rank[offset] >> (4 * param)) & 0x0f;

	OS_Printf( "p_rank[%d] = %d\n", offset, p_rank[offset] );
	OS_Printf( "ret = %d\n", ret );
	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   タイプごとのレベルをセット
 *
 * @param   csr_pos		カーソル位置
 * @param   p_rank		ランクデータのポインタ
 * @param   num			セットする値
 *
 * @retval  none
 */
//--------------------------------------------------------------
void Stage_SetTypeLevel( u8 csr_pos, u8* p_rank, u8 num )
{
	u8 offset;
	u8 param;
	u8 set_num;

	offset	= ( csr_pos / 2 );			//フラグのオフセット[0,1][2,3],,,
	param	= ( csr_pos % 2 );			//フラグのどちらの4bitか取得

	//OS_Printf( "************************\n" );
	//OS_Printf( "before p_rank[%d] = %d\n", offset, p_rank[offset] );

	if( param == 0 ){
		p_rank[offset] = p_rank[offset] & 0xf0;
	}else{
		p_rank[offset] = p_rank[offset] & 0x0f;
	}

	set_num = (num << (4 * param));
	OS_Printf( "set_num = %d\n", set_num );

	p_rank[offset] |= set_num;
	OS_Printf( "after p_rank[%d] = %d\n", offset, p_rank[offset] );
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
	SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
	return;
}

//----------------------------------------------------------
/**
 * @brief	プレイデータ　指定したバトルタイプのレベルをクリア
 *
 * @param   p_rank		ランクデータのポインタ
 *
 * @retval	none
 */
//----------------------------------------------------------
void Stage_ClearTypeLevel( u8* p_rank )
{
	int i;

	for( i=0; i < STAGE_TR_TYPE_MAX ;i++ ){
		Stage_SetTypeLevel( i, p_rank, 0 );		//0オリジン
	}
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
	SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
	return;
}


//============================================================================================
//
//	成績関連
//
//============================================================================================

//----------------------------------------------------------
/**
 * @brief	(成績データ)ステージセーブワークのサイズ取得
 *
 * @param	none
 *
 * @return	int		サイズ(バイト単位)
 */
//----------------------------------------------------------
int STAGESCORE_GetWorkSize( void )
{
	return sizeof(STAGESCORE);
}

//----------------------------------------------------------
/**
 * @brief	(成績データ)ステージワークの初期化
 *
 * @param	wk		STAGESCORE型のポインタ
 *
 * @return	none
 */
//----------------------------------------------------------
void STAGESCORE_Init( STAGESCORE* wk )
{
	//MI_CpuClear32( wk, sizeof(STAGESCORE) );
	MI_CpuClear8( wk, sizeof(STAGESCORE) );

#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
	SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
	return;
}

//----------------------------------------------------------
/**
 * @brief	ステージセーブデータの取得(成績データ)
 *
 * @param	sv		セーブデータへのポインタ
 *
 * @return	STAGEDATAへのポインタ
 */
//----------------------------------------------------------
STAGESCORE * SaveData_GetStageScore( SAVEDATA * sv )
{
	//return (STAGEDATA*)SaveData_Get( sv, GMDATA_ID_STAGE );
	FRONTIER_SAVEWORK* data = SaveData_Get( sv, GMDATA_ID_FRONTIER );
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
	SVLD_CheckCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
	return &data->stage.score;
}

//----------------------------------------------------------
/**
 * @brief	成績データ　セット
 *
 * @param	wk		STAGESCORE型のポインタ
 * @param	id		取得するデータID
 * @param	buf		void*:データを格納したバッファへのポインタ
 *
 * @retval	none
 */
//----------------------------------------------------------
void STAGESCORE_PutScoreData( STAGESCORE* wk, u8 id, u8 param, u8 param2, const void* buf )
{
	u32	*buf32	= (u32 *)buf;
	u16	*buf16	= (u16 *)buf;
	u8	*buf8	= (u8 *)buf;

	switch( id ){

	//10連勝(クリア)したかフラグ
	case STAGESCORE_ID_CLEAR_FLAG:
		OS_Printf( "before clear_flag = %d\n", wk->clear_flag );
		if( buf8[0] >= 1 ){
			wk->clear_flag |= (1 << param);					//セット
		}else{
			wk->clear_flag &= (0xff ^ (1 << param));		//リセット
		}
		OS_Printf( "after clear_flag = %d\n", wk->clear_flag );
		break;

	default:
		GF_ASSERT(0);
		break;
	};

#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
	SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
	return;
}

//----------------------------------------------------------
/**
 * @brief	成績データ　取得
 *
 * @param	wk		STAGESCORE型のポインタ
 * @param	id		取得するデータID
 * @param	buf		void*:データ取得ポインタ
 *
 * @retval	none
 *
 * 戻り値を取得する時に型キャストを使用しているのでコピペ注意！
 */
//----------------------------------------------------------
u32 STAGESCORE_GetScoreData( STAGESCORE* wk, u8 id, u8 param, u8 param2, void* buf )
{
	//OS_Printf( "STAGEDATA = %d\n", sizeof(STAGEDATA) );
	//OS_Printf( "STAGESCORE = %d\n", sizeof(STAGESCORE) );

	switch( id ){

	//10連勝(クリア)したかフラグ
	case STAGESCORE_ID_CLEAR_FLAG:
		OS_Printf( "get clear_flag = %d\n", wk->clear_flag );
		return (u32)((wk->clear_flag >> param) & 0x01);

	default:
		GF_ASSERT(0);
		break;
	};

	return 0;
}



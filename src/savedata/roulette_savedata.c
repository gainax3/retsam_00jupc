//==============================================================================================
/**
 * @file	roulette_savedata.c
 * @brief	「バトルルーレット」セーブデータ
 * @author	Satoshi Nohara
 * @date	2007.09.05
 */
//==============================================================================================
#include "common.h"
#include "savedata/savedata_def.h"
#include "savedata/savedata.h"
#include "savedata/frontier_savedata.h"
#include "savedata/roulette_savedata.h"
#include "frontier_savedata_local.h"
#include "roulette_savedata_local.h"
#include "gflib/assert.h"
#include "gflib/heapsys.h"
#include "../frontier/roulette_def.h"


//============================================================================================
//
//	プロトタイプ宣言
//
//============================================================================================
int ROULETTEDATA_GetWorkSize( void );
void ROULETTEDATA_Init( ROULETTEDATA* wk );
ROULETTEDATA * SaveData_GetRouletteData( SAVEDATA * sv );

BOOL ROULETTEDATA_GetSaveFlag( ROULETTEDATA* wk );
void ROULETTEDATA_SetSaveFlag( ROULETTEDATA* wk, BOOL flag );
void ROULETTEDATA_PutPlayData( ROULETTEDATA* wk, u8 id, u8 param, u8 param2, const void* buf );
u32 ROULETTEDATA_GetPlayData( ROULETTEDATA* wk, u8 id, u8 param, u8 param2, void* buf );

//成績
int ROULETTESCORE_GetWorkSize( void );
void ROULETTESCORE_Init( ROULETTESCORE* wk );
ROULETTESCORE * SaveData_GetRouletteScore( SAVEDATA * sv );
void ROULETTESCORE_PutScoreData( ROULETTESCORE* wk, u8 id, u8 param, u8 param2, const void* buf );
u32 ROULETTESCORE_GetScoreData( ROULETTESCORE* wk, u8 id, u8 param, u8 param2, void* buf );


//============================================================================================
//
//	セーブデータシステムが依存する関数
//
//============================================================================================

//----------------------------------------------------------
/**
 * @brief	(中断データ)ルーレットセーブワークのサイズ取得
 *
 * @param	none
 *
 * @return	int		サイズ(バイト単位)
 */
//----------------------------------------------------------
int ROULETTEDATA_GetWorkSize( void )
{
	return sizeof(ROULETTEDATA);
}

//----------------------------------------------------------
/**
 * @brief	(中断データ)ルーレットワークの初期化
 *
 * @param	wk		ROULETTEDATAへのポインタ
 *
 * @return	none
 */
//----------------------------------------------------------
void ROULETTEDATA_Init( ROULETTEDATA* wk )
{
	//MI_CpuClear32( wk, sizeof(ROULETTEDATA) );
	MI_CpuClear8( wk, sizeof(ROULETTEDATA) );
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
	SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
	return;
}

//----------------------------------------------------------
/**
 * @brief	ルーレットセーブデータの取得(プレイデータ)
 *
 * @param	sv		セーブデータへのポインタ
 *
 * @return	ROULETTEDATAへのポインタ
 */
//----------------------------------------------------------
ROULETTEDATA * SaveData_GetRouletteData( SAVEDATA * sv )
{
	//return (ROULETTEDATA*)SaveData_Get( sv, GMDATA_ID_ROULETTE );
	FRONTIER_SAVEWORK* data = SaveData_Get( sv, GMDATA_ID_FRONTIER );
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
	SVLD_CheckCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
	return &data->play_roulette;
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
 * @param	wk		ROULETTEDATA型のポインタ
 *
 * @retval	TRUE	正しくセーブされている
 * @retval	FALSE	セーブされていない
 */
//----------------------------------------------------------
BOOL ROULETTEDATA_GetSaveFlag( ROULETTEDATA* wk )
{
	return wk->save_flag;
}

//----------------------------------------------------------
/**
 * @brief	プレイデータ　セーブ状態フラグをセット
 *
 * @param	wk		ROULETTEDATA型のポインタ
 * @param	flag	TRUE、FALSE
 *
 * @retval	none
 */
//----------------------------------------------------------
void ROULETTEDATA_SetSaveFlag( ROULETTEDATA* wk, BOOL flag )
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
 * @param	wk		ROULETTEDATA型のポインタ
 * @param	id		取得するデータID
 * @param	buf		void*:データを格納したバッファへのポインタ
 *
 * @retval	none
 */
//----------------------------------------------------------
void ROULETTEDATA_PutPlayData( ROULETTEDATA* wk, u8 id, u8 param, u8 param2, const void* buf )
{
	u32	*buf32	= (u32 *)buf;
	u16	*buf16	= (u16 *)buf;
	u8	*buf8	= (u8 *)buf;

	switch( id ){

	//シングル、ダブル、マルチ、wifiマルチ
	case ROULETTEDATA_ID_TYPE:
		wk->type = buf8[0];
		OS_Printf( "wk->type = %d\n", wk->type );
		break;

	//カーソルランダム移動フラグ
	case ROULETTEDATA_ID_RANDOM_FLAG:
		wk->random_flag = buf8[0];
		OS_Printf( "wk->random_flag = %d\n", wk->random_flag );
		break;

	//今何人目？
	case ROULETTEDATA_ID_ROUND:
		wk->round = buf8[0];
		OS_Printf( "wk->round = %d\n", wk->round );
		break;

	//カーソルのスピードレベル
	case ROULETTEDATA_ID_CSR_SPEED_LEVEL:
		wk->csr_speed_level = buf8[0];
		OS_Printf( "wk->csr_speed_level = %d\n", wk->csr_speed_level );
		break;

	//手持ちポケモンのITEM
	case ROULETTEDATA_ID_TEMOTI_ITEM:
		wk->item[param] = buf16[0];
		OS_Printf( "wk->item[%d] = %d\n", param, wk->item[param] );
		break;

	//敵トレーナーデータインデックス
	case ROULETTEDATA_ID_TR_INDEX:
		wk->tr_index[param] = buf16[0];
		OS_Printf( "wk->tr_index[%d] = %d\n", param, wk->tr_index[param] );
		break;

	//参加している手持ちポケモンの位置情報
	case ROULETTEDATA_ID_MINE_POKE_POS:
		wk->mine_poke_pos[param] = buf8[0];
		OS_Printf( "wk->mine_poke_pos[%d] = %d\n", param, wk->mine_poke_pos[param] );
		break;

	//敵ポケモンデータが被らないようにするために保存
	case ROULETTEDATA_ID_ENEMY_POKE_INDEX:
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
 * @param	wk		ROULETTEDATA型のポインタ
 * @param	id		取得するデータID
 * @param	buf		void*:データ取得ポインタ
 *
 * @retval	none
 *
 * 戻り値を取得する時に型キャストを使用しているのでコピペ注意！
 */
//----------------------------------------------------------
u32 ROULETTEDATA_GetPlayData( ROULETTEDATA* wk, u8 id, u8 param, u8 param2, void* buf )
{
	switch( id ){

	//シングル、ダブル、マルチ、wifiマルチ
	case ROULETTEDATA_ID_TYPE:
		return (u32)wk->type;

	//カーソルランダム移動フラグ
	case ROULETTEDATA_ID_RANDOM_FLAG:
		return (u32)wk->random_flag;

	//今何人目？
	case ROULETTEDATA_ID_ROUND:
		return (u32)wk->round;

	//カーソルスピードレベル
	case ROULETTEDATA_ID_CSR_SPEED_LEVEL:
		return (u32)wk->csr_speed_level;

	//手持ちポケモンのITEM
	case ROULETTEDATA_ID_TEMOTI_ITEM:
		return (u32)wk->item[param];

	//敵トレーナーデータインデックス
	case ROULETTEDATA_ID_TR_INDEX:
		return (u32)wk->tr_index[param];

	//参加している手持ちポケモンの位置情報
	case ROULETTEDATA_ID_MINE_POKE_POS:
		return wk->mine_poke_pos[param];

	//敵ポケモンデータが被らないようにするために保存
	case ROULETTEDATA_ID_ENEMY_POKE_INDEX:
		return wk->enemy_poke_index[param];

	default:
		GF_ASSERT(0);
		break;
	};

	return 0;
}


//============================================================================================
//
//	成績関連
//
//============================================================================================

//----------------------------------------------------------
/**
 * @brief	(成績データ)ルーレットセーブワークのサイズ取得
 *
 * @param	none
 *
 * @return	int		サイズ(バイト単位)
 */
//----------------------------------------------------------
int ROULETTESCORE_GetWorkSize( void )
{
	return sizeof(ROULETTESCORE);
}

//----------------------------------------------------------
/**
 * @brief	(成績データ)ルーレットワークの初期化
 *
 * @param	wk		ROULETTEDATAへのポインタ
 *
 * @return	none
 */
//----------------------------------------------------------
void ROULETTESCORE_Init( ROULETTESCORE* wk )
{
	//MI_CpuClear32( wk, sizeof(ROULETTESCORE) );
	MI_CpuClear8( wk, sizeof(ROULETTESCORE) );
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
	SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
	return;
}

//----------------------------------------------------------
/**
 * @brief	ルーレットセーブデータの取得(成績データ)
 *
 * @param	sv		セーブデータへのポインタ
 *
 * @return	ROULETTEDATAへのポインタ
 */
//----------------------------------------------------------
ROULETTESCORE * SaveData_GetRouletteScore( SAVEDATA * sv )
{
	//return (ROULETTEDATA*)SaveData_Get( sv, GMDATA_ID_ROULETTE );
	FRONTIER_SAVEWORK* data = SaveData_Get( sv, GMDATA_ID_FRONTIER );
	return &data->roulette.score;
}

//----------------------------------------------------------
/**
 * @brief	成績データ　セット
 *
 * @param	wk		ROULETTESCORE型のポインタ
 * @param	id		取得するデータID
 * @param	buf		void*:データを格納したバッファへのポインタ
 *
 * @retval	none
 */
//----------------------------------------------------------
void ROULETTESCORE_PutScoreData( ROULETTESCORE* wk, u8 id, u8 param, u8 param2, const void* buf )
{
	u32	*buf32	= (u32 *)buf;
	u16	*buf16	= (u16 *)buf;
	u8	*buf8	= (u8 *)buf;

	switch( id ){

	//7連勝(クリア)したかフラグ
	case ROULETTESCORE_ID_CLEAR_FLAG:
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
 * @brief	プレイデータ　取得
 *
 * @param	wk		ROULETTESCORE型のポインタ
 * @param	id		取得するデータID
 * @param	buf		void*:データ取得ポインタ
 *
 * @retval	none
 *
 * 戻り値を取得する時に型キャストを使用しているのでコピペ注意！
 */
//----------------------------------------------------------
u32 ROULETTESCORE_GetScoreData( ROULETTESCORE* wk, u8 id, u8 param, u8 param2, void* buf )
{
	//OS_Printf( "ROULETTEDATA = %d\n", sizeof(ROULETTEDATA) );
	//OS_Printf( "ROULETTESCORE = %d\n", sizeof(ROULETTESCORE) );

	switch( id ){

	//7連勝(クリア)したかフラグ
	case ROULETTESCORE_ID_CLEAR_FLAG:
		OS_Printf( "get clear_flag = %d\n", wk->clear_flag );
		return (u32)((wk->clear_flag >> param) & 0x01);

	default:
		GF_ASSERT(0);
		break;
	};

	return 0;
}



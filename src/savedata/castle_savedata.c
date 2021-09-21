//==============================================================================================
/**
 * @file	castle_savedata.c
 * @brief	「バトルキャッスル」セーブデータ
 * @author	Satoshi Nohara
 * @date	2007.06.27
 */
//==============================================================================================
#include "common.h"
#include "savedata/savedata_def.h"
#include "savedata/savedata.h"
#include "savedata/frontier_savedata.h"
#include "savedata/castle_savedata.h"
#include "frontier_savedata_local.h"
#include "castle_savedata_local.h"
#include "gflib/assert.h"
#include "gflib/heapsys.h"
#include "../frontier/castle_def.h"


//============================================================================================
//
//	プロトタイプ宣言
//
//============================================================================================
int CASTLEDATA_GetWorkSize( void );
void CASTLEDATA_Init( CASTLEDATA* wk );
CASTLEDATA * SaveData_GetCastleData( SAVEDATA * sv );

BOOL CASTLEDATA_GetSaveFlag( CASTLEDATA* wk );
void CASTLEDATA_SetSaveFlag( CASTLEDATA* wk, BOOL flag );
void CASTLEDATA_PutPlayData( CASTLEDATA* wk, u8 id, u8 param, u8 param2, const void* buf );
u32 CASTLEDATA_GetPlayData( CASTLEDATA* wk, u8 id, u8 param, u8 param2, void* buf );

//成績
int CASTLESCORE_GetWorkSize( void );
void CASTLESCORE_Init( CASTLESCORE* wk );
CASTLESCORE * SaveData_GetCastleScore( SAVEDATA * sv );
void CASTLESCORE_PutScoreData( CASTLESCORE* wk, u8 id, u8 param, u8 param2, const void* buf );
u32 CASTLESCORE_GetScoreData( CASTLESCORE* wk, u8 id, u8 param, u8 param2, void* buf );


//============================================================================================
//
//	セーブデータシステムが依存する関数
//
//============================================================================================

//----------------------------------------------------------
/**
 * @brief	(中断データ)キャッスルセーブワークのサイズ取得
 *
 * @param	none
 *
 * @return	int		サイズ(バイト単位)
 */
//----------------------------------------------------------
int CASTLEDATA_GetWorkSize( void )
{
	return sizeof(CASTLEDATA);
}

//----------------------------------------------------------
/**
 * @brief	(中断データ)キャッスルワークの初期化
 *
 * @param	wk		CASTLEDATAへのポインタ
 *
 * @return	none
 */
//----------------------------------------------------------
void CASTLEDATA_Init( CASTLEDATA* wk )
{
	//MI_CpuClear32( wk, sizeof(CASTLEDATA) );
	MI_CpuClear8( wk, sizeof(CASTLEDATA) );
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
	SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
	return;
}

//----------------------------------------------------------
/**
 * @brief	キャッスルセーブデータの取得(プレイデータ)
 *
 * @param	sv		セーブデータへのポインタ
 *
 * @return	CASTLEDATAへのポインタ
 */
//----------------------------------------------------------
CASTLEDATA * SaveData_GetCastleData( SAVEDATA * sv )
{
	//return (CASTLEDATA*)SaveData_Get( sv, GMDATA_ID_CASTLE );
	FRONTIER_SAVEWORK* data = SaveData_Get( sv, GMDATA_ID_FRONTIER );
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
	SVLD_CheckCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
	return &data->play_castle;
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
 * @param	wk		CASTLEDATA型のポインタ
 *
 * @retval	TRUE	正しくセーブされている
 * @retval	FALSE	セーブされていない
 */
//----------------------------------------------------------
BOOL CASTLEDATA_GetSaveFlag( CASTLEDATA* wk )
{
	return wk->save_flag;
}

//----------------------------------------------------------
/**
 * @brief	プレイデータ　セーブ状態フラグをセット
 *
 * @param	wk		CASTLEDATA型のポインタ
 * @param	flag	TRUE、FALSE
 *
 * @retval	none
 */
//----------------------------------------------------------
void CASTLEDATA_SetSaveFlag( CASTLEDATA* wk, BOOL flag )
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
 * @param	wk		CASTLEDATA型のポインタ
 * @param	id		取得するデータID
 * @param	buf		void*:データを格納したバッファへのポインタ
 *
 * @retval	none
 */
//----------------------------------------------------------
void CASTLEDATA_PutPlayData( CASTLEDATA* wk, u8 id, u8 param, u8 param2, const void* buf )
{
	u32	*buf32	= (u32 *)buf;
	u16	*buf16	= (u16 *)buf;
	u8	*buf8	= (u8 *)buf;

	switch( id ){

	//シングル、ダブル、マルチ、wifiマルチ
	case CASTLEDATA_ID_TYPE:
		wk->type = buf8[0];
		OS_Printf( "wk->type = %d\n", wk->type );
		break;

	//今何人目？
	case CASTLEDATA_ID_ROUND:
		wk->round = buf8[0];
		OS_Printf( "wk->round = %d\n", wk->round );
		break;

	//手持ちポケモンのHP
	case CASTLEDATA_ID_TEMOTI_HP:
		wk->hp[param] = buf16[0];
		OS_Printf( "wk->hp[%d] = %d\n", param, wk->hp[param] );
		break;

	//手持ちポケモンのPP
	case CASTLEDATA_ID_TEMOTI_PP:
		wk->pp[param][param2] = buf8[0];
		OS_Printf( "wk->pp[%d][%d] = %d\n", param, param2, wk->pp[param][param2] );
		break;

	//手持ちポケモンのCONDITION
	case CASTLEDATA_ID_TEMOTI_CONDITION:
		wk->condition[param] = buf32[0];
		OS_Printf( "wk->condition[%d] = %d\n", param, wk->condition[param] );
		break;

	//手持ちポケモンのITEM
	case CASTLEDATA_ID_TEMOTI_ITEM:
		wk->item[param] = buf16[0];
		OS_Printf( "wk->item[%d] = %d\n", param, wk->item[param] );
		break;

	//敵トレーナーデータインデックス
	case CASTLEDATA_ID_TR_INDEX:
		wk->tr_index[param] = buf16[0];
		OS_Printf( "wk->tr_index[%d] = %d\n", param, wk->tr_index[param] );
		break;

	//参加している手持ちポケモンの位置情報
	case CASTLEDATA_ID_MINE_POKE_POS:
		wk->mine_poke_pos[param] = buf8[0];
		OS_Printf( "wk->mine_poke_pos[%d] = %d\n", param, wk->mine_poke_pos[param] );
		break;

	//敵ポケモンデータが被らないようにするために保存
	case CASTLEDATA_ID_ENEMY_POKE_INDEX:
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
 * @param	wk		CASTLEDATA型のポインタ
 * @param	id		取得するデータID
 * @param	buf		void*:データ取得ポインタ
 *
 * @retval	none
 *
 * 戻り値を取得する時に型キャストを使用しているのでコピペ注意！
 */
//----------------------------------------------------------
u32 CASTLEDATA_GetPlayData( CASTLEDATA* wk, u8 id, u8 param, u8 param2, void* buf )
{
	switch( id ){

	//シングル、ダブル、マルチ、wifiマルチ
	case CASTLEDATA_ID_TYPE:
		return (u32)wk->type;

	//今何人目？
	case CASTLEDATA_ID_ROUND:
		return (u32)wk->round;

	//手持ちポケモンのHP
	case CASTLEDATA_ID_TEMOTI_HP:
		return (u32)wk->hp[param];

	//手持ちポケモンのPP
	case CASTLEDATA_ID_TEMOTI_PP:
		return (u32)wk->pp[param][param2];

	//手持ちポケモンのCONDITION
	case CASTLEDATA_ID_TEMOTI_CONDITION:
		return (u32)wk->condition[param];

	//手持ちポケモンのITEM
	case CASTLEDATA_ID_TEMOTI_ITEM:
		return (u32)wk->item[param];

	//敵トレーナーデータインデックス
	case CASTLEDATA_ID_TR_INDEX:
		return (u32)wk->tr_index[param];

	//参加している手持ちポケモンの位置情報
	case CASTLEDATA_ID_MINE_POKE_POS:
		return wk->mine_poke_pos[param];

	//敵ポケモンデータが被らないようにするために保存
	case CASTLEDATA_ID_ENEMY_POKE_INDEX:
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
 * @brief	(成績データ)キャッスルセーブワークのサイズ取得
 *
 * @param	none
 *
 * @return	int		サイズ(バイト単位)
 */
//----------------------------------------------------------
int CASTLESCORE_GetWorkSize( void )
{
	return sizeof(CASTLESCORE);
}

//----------------------------------------------------------
/**
 * @brief	(成績データ)キャッスルワークの初期化
 *
 * @param	wk		CASTLESCOREへのポインタ
 *
 * @return	none
 */
//----------------------------------------------------------
void CASTLESCORE_Init( CASTLESCORE* wk )
{
	int i,j;

	//MI_CpuClear32( wk, sizeof(CASTLESCORE) );
	MI_CpuClear8( wk, sizeof(CASTLESCORE) );

#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
	SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
	return;
}

//----------------------------------------------------------
/**
 * @brief	キャッスルセーブデータの取得(成績データ)
 *
 * @param	sv		セーブデータへのポインタ
 *
 * @return	CASTLEDATAへのポインタ
 */
//----------------------------------------------------------
CASTLESCORE * SaveData_GetCastleScore( SAVEDATA * sv )
{
	FRONTIER_SAVEWORK* data = SaveData_Get( sv, GMDATA_ID_FRONTIER );
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
	SVLD_CheckCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
	return &data->castle.score;
}

//----------------------------------------------------------
/**
 * @brief	成績データ　セット
 *
 * @param	wk		CASTLESCORE型のポインタ
 * @param	id		取得するデータID
 * @param	buf		void*:データを格納したバッファへのポインタ
 *
 * @retval	none
 */
//----------------------------------------------------------
void CASTLESCORE_PutScoreData( CASTLESCORE* wk, u8 id, u8 param, u8 param2, const void* buf )
{
	u32	*buf32	= (u32 *)buf;
	u16	*buf16	= (u16 *)buf;
	u8	*buf8	= (u8 *)buf;

	switch( id ){

	//7連勝(クリア)したかフラグ
	case CASTLESCORE_ID_CLEAR_FLAG:
		OS_Printf( "before clear_flag = %d\n", wk->clear_flag );
		if( buf8[0] >= 1 ){
			wk->clear_flag |= (1 << param);					//セット
		}else{
			wk->clear_flag &= (0xff ^ (1 << param));		//リセット
		}
		OS_Printf( "after clear_flag = %d\n", wk->clear_flag );
		break;

	//説明受けたかフラグ
	case CASTLESCORE_ID_GUIDE_FLAG:
		wk->guide_flag = 1;
		OS_Printf( "guide_flag = %d\n", wk->guide_flag );
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
 * @param	wk		CASTLESCORE型のポインタ
 * @param	id		取得するデータID
 * @param	buf		void*:データ取得ポインタ
 *
 * @retval	none
 *
 * 戻り値を取得する時に型キャストを使用しているのでコピペ注意！
 */
//----------------------------------------------------------
u32 CASTLESCORE_GetScoreData( CASTLESCORE* wk, u8 id, u8 param, u8 param2, void* buf )
{
	OS_Printf( "CASTLEDATA = %d\n", sizeof(CASTLEDATA) );
	OS_Printf( "CASTLESCORE = %d\n", sizeof(CASTLESCORE) );

	switch( id ){

	//7連勝(クリア)したかフラグ
	case CASTLESCORE_ID_CLEAR_FLAG:
		OS_Printf( "get clear_flag = %d\n", wk->clear_flag );
		return (u32)((wk->clear_flag >> param) & 0x01);

	//説明受けたかフラグ
	case CASTLESCORE_ID_GUIDE_FLAG:
		OS_Printf( "guide_flag = %d\n", wk->guide_flag );
		return (u32)wk->guide_flag;

	default:
		GF_ASSERT(0);
		break;
	};

	return 0;
}



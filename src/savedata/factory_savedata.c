//==============================================================================================
/**
 * @file	factory_savedata.c
 * @brief	「バトルファクトリー」セーブデータ
 * @author	Satoshi Nohara
 * @date	2007.03.28
 */
//==============================================================================================
#include "common.h"
#include "savedata/savedata_def.h"
#include "savedata/savedata.h"
#include "savedata/frontier_savedata.h"
#include "savedata/factory_savedata.h"
#include "frontier_savedata_local.h"
#include "factory_savedata_local.h"
#include "gflib/assert.h"
#include "gflib/heapsys.h"
#include "../frontier/factory_def.h"


//============================================================================================
//
//	セーブデータシステムが依存する関数
//
//============================================================================================
int FACTORYDATA_GetWorkSize( void );
void FACTORYDATA_Init( FACTORYDATA* wk );
//FACTORYDATA* FACTORYDATA_AllocWork( int heapID );
FACTORYDATA * SaveData_GetFactoryData( SAVEDATA * sv );

//成績
int FACTORYSCORE_GetWorkSize( void );
void FACTORYSCORE_Init( FACTORYSCORE* wk );
void FACTORYSCORE_PutScoreData( FACTORYSCORE* wk, u8 id, u8 param, const void* buf );
u32 FACTORYSCORE_GetScoreData( FACTORYSCORE* wk, u8 id, u8 param, void* buf );
FACTORYSCORE * SaveData_GetFactoryScore( SAVEDATA * sv );

//----------------------------------------------------------
/**
 * @brief	(中断データ)ファクトリーセーブワークのサイズ取得
 *
 * @param	none
 *
 * @return	int		サイズ(バイト単位)
 */
//----------------------------------------------------------
int FACTORYDATA_GetWorkSize( void )
{
	return sizeof(FACTORYDATA);
}

//----------------------------------------------------------
/**
 * @brief	(中断データ)ファクトリーワークの初期化
 *
 * @param	wk		FACTORYDATAへのポインタ
 *
 * @return	none
 */
//----------------------------------------------------------
void FACTORYDATA_Init( FACTORYDATA* wk )
{
	//MI_CpuClear32( wk, sizeof(FACTORYDATA) );
	MI_CpuClear8( wk, sizeof(FACTORYDATA) );
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
	SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
	return;
}

//----------------------------------------------------------
/**
 * @brief	ファクトリーワークの初期化
 *
 * @param	heapID		メモリ確保をおこなうヒープ指定
 *
 * @return	wk			取得したワークへのポインタ
 */
//----------------------------------------------------------
#if 0
FACTORYDATA* FACTORYDATA_AllocWork( int heapID )
{
	FACTORYDATA* wk = sys_AllocMemory( heapID, sizeof(FACTORYDATA) );
	FACTORYDATA_Init( wk );
	FACTORYSCORE_Init( wk );
	return wk;
}
#endif

//----------------------------------------------------------
/**
 * @brief	ファクトリーセーブデータの取得(プレイデータ)
 *
 * @param	sv		セーブデータへのポインタ
 *
 * @return	FACTORYDATAへのポインタ
 */
//----------------------------------------------------------
FACTORYDATA * SaveData_GetFactoryData( SAVEDATA * sv )
{
	//return (FACTORYDATA*)SaveData_Get( sv, GMDATA_ID_FACTORY );
	FRONTIER_SAVEWORK* data = SaveData_Get( sv, GMDATA_ID_FRONTIER );
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
	SVLD_CheckCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
	return &data->play_factory;
}


//============================================================================================
//
//	メンバアクセス
//
//============================================================================================
BOOL FACTORYDATA_GetSaveFlag( FACTORYDATA* wk );
void FACTORYDATA_SetSaveFlag( FACTORYDATA* wk, BOOL flag );
void FACTORYDATA_PutPlayData( FACTORYDATA* wk, u8 id, u8 param, const void* buf );
u32 FACTORYDATA_GetPlayData( FACTORYDATA* wk, u8 id, u8 param, void* buf );

//----------------------------------------------------------
/**
 * @brief	プレイデータ　正しくセーブ済みかどうか？
 *
 * @param	wk		FACTORYDATA型のポインタ
 *
 * @retval	TRUE	正しくセーブされている
 * @retval	FALSE	セーブされていない
 */
//----------------------------------------------------------
BOOL FACTORYDATA_GetSaveFlag( FACTORYDATA* wk )
{
	return wk->save_flag;
}

//----------------------------------------------------------
/**
 * @brief	プレイデータ　セーブ状態フラグをセット
 *
 * @param	wk		FACTORYDATA型のポインタ
 * @param	flag	TRUE、FALSE
 *
 * @retval	none
 */
//----------------------------------------------------------
void FACTORYDATA_SetSaveFlag( FACTORYDATA* wk, BOOL flag )
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
 * @param	wk		FACTORYDATA型のポインタ
 * @param	id		取得するデータID
 * @param	buf		void*:データを格納したバッファへのポインタ
 *
 * @retval	none
 */
//----------------------------------------------------------
void FACTORYDATA_PutPlayData( FACTORYDATA* wk, u8 id, u8 param, const void* buf )
{
	u32	*buf32	= (u32 *)buf;
	u16	*buf16	= (u16 *)buf;
	u8	*buf8	= (u8 *)buf;

	switch( id ){

	//LV50、オープン
	case FACTORYDATA_ID_LEVEL:
		wk->level = buf8[0];
		break;

	//シングル、ダブル、マルチ、wifiマルチ
	case FACTORYDATA_ID_TYPE:
		wk->type = buf8[0];
		break;

	//今何人目？
	case FACTORYDATA_ID_ROUND:
		wk->round = buf8[0];
		//OS_Printf( "buf8[0] = %d\n", buf8[0] );
		//OS_Printf( "wk->round = %d\n", wk->round );
		break;

	//
	case FACTORYDATA_ID_TR_INDEX:
		wk->tr_index[param] = buf16[0];
		break;

	//手持ち
	case FACTORYDATA_ID_TEMOTI_POKE_INDEX:
		wk->temoti_poke_index[param] = buf16[0];
		break;

	case FACTORYDATA_ID_TEMOTI_POW_RND:
		wk->temoti_pow_rnd[param] = buf8[0];
		break;

	case FACTORYDATA_ID_TEMOTI_PERSONAL_RND:
		wk->temoti_personal_rnd[param] = buf32[0];
		break;

	//敵
	case FACTORYDATA_ID_ENEMY_POKE_INDEX:
		wk->enemy_poke_index[param] = buf16[0];
		break;

	case FACTORYDATA_ID_ENEMY_POW_RND:
		wk->enemy_pow_rnd[param] = buf8[0];
		break;

	case FACTORYDATA_ID_ENEMY_PERSONAL_RND:
		wk->enemy_personal_rnd[param] = buf32[0];
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
 * @param	wk		FACTORYDATA型のポインタ
 * @param	id		取得するデータID
 * @param	buf		void*:データ取得ポインタ
 *
 * @retval	none
 *
 * 戻り値を取得する時に型キャストを使用しているのでコピペ注意！
 */
//----------------------------------------------------------
u32 FACTORYDATA_GetPlayData( FACTORYDATA* wk, u8 id, u8 param, void* buf )
{
	switch( id ){

	case FACTORYDATA_ID_TYPE:
		return (u32)wk->type;

	case FACTORYDATA_ID_LEVEL:
		return (u32)wk->level;

	case FACTORYDATA_ID_ROUND:
		return (u32)wk->round;

	case FACTORYDATA_ID_TR_INDEX:
		return (u32)wk->tr_index[param];

	//手持ち
	case FACTORYDATA_ID_TEMOTI_POKE_INDEX:
		return (u32)wk->temoti_poke_index[param];

	case FACTORYDATA_ID_TEMOTI_POW_RND:
		return (u32)wk->temoti_pow_rnd[param];

	case FACTORYDATA_ID_TEMOTI_PERSONAL_RND:
		return (u32)wk->temoti_personal_rnd[param];

	//敵
	case FACTORYDATA_ID_ENEMY_POKE_INDEX:
		return (u32)wk->enemy_poke_index[param];

	case FACTORYDATA_ID_ENEMY_POW_RND:
		return (u32)wk->enemy_pow_rnd[param];

	case FACTORYDATA_ID_ENEMY_PERSONAL_RND:
		return (u32)wk->enemy_personal_rnd[param];

	};

	return 0;
}


//==============================================================================================
//
//	成績関連
//
//==============================================================================================

//----------------------------------------------------------
/**
 * @brief	(成績データ)ファクトリーセーブワークのサイズ取得
 *
 * @param	none
 *
 * @return	int		サイズ(バイト単位)
 */
//----------------------------------------------------------
int FACTORYSCORE_GetWorkSize( void )
{
	return sizeof(FACTORYSCORE);
}

//----------------------------------------------------------
/**
 * @brief	(成績データ)ファクトリーワークの初期化
 *
 * @param	wk		FACTORYDATAへのポインタ
 *
 * @return	none
 */
//----------------------------------------------------------
void FACTORYSCORE_Init( FACTORYSCORE* wk )
{
	//MI_CpuClear32( wk, sizeof(FACTORYSCORE) );
	MI_CpuClear8( wk, sizeof(FACTORYSCORE) );
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
	SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
	return;
}

//----------------------------------------------------------
/**
 * @brief	成績データ　セット
 *
 * @param	wk		FACTORYSCORE型のポインタ
 * @param	id		取得するデータID
 * @param	buf		void*:データを格納したバッファへのポインタ
 *
 * @retval	none
 */
//----------------------------------------------------------
void FACTORYSCORE_PutScoreData( FACTORYSCORE* wk, u8 id, u8 param, const void* buf )
{
	u32	*buf32	= (u32 *)buf;
	u16	*buf16	= (u16 *)buf;
	u8	*buf8	= (u8 *)buf;

	switch( id ){

	//7連勝(クリア)したかフラグ
	case FACTORYSCORE_ID_CLEAR_FLAG:
		//wk->clear_flag = buf8[0];
		OS_Printf( "before clear_flag = %d\n", wk->clear_flag );
		if( buf8[0] >= 1 ){
			wk->clear_flag |= (1 << param);					//セット
		}else{
			wk->clear_flag &= (0xff ^ (1 << param));		//リセット
		}
		OS_Printf( "after clear_flag = %d\n", wk->clear_flag );
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
 * @param	wk		FACTORYSCORE型のポインタ
 * @param	id		取得するデータID
 * @param	buf		void*:データ取得ポインタ
 *
 * @retval	none
 *
 * 戻り値を取得する時に型キャストを使用しているのでコピペ注意！
 */
//----------------------------------------------------------
u32 FACTORYSCORE_GetScoreData( FACTORYSCORE* wk, u8 id, u8 param, void* buf )
{
	//OS_Printf( "FACTORYDATA = %d\n", sizeof(FACTORYDATA) );
	//OS_Printf( "FACTORYSCORE = %d\n", sizeof(FACTORYSCORE) );

	switch( id ){

	case FACTORYSCORE_ID_CLEAR_FLAG:
		//return (u32)wk->clear_flag;
		OS_Printf( "get clear_flag = %d\n", wk->clear_flag );
		return (u32)((wk->clear_flag >> param) & 0x01);

	};

	return 0;
}

//----------------------------------------------------------
/**
 * @brief	ファクトリーセーブデータの取得(成績データ)
 *
 * @param	sv		セーブデータへのポインタ
 *
 * @return	FACTORYSCOREへのポインタ
 */
//----------------------------------------------------------
FACTORYSCORE * SaveData_GetFactoryScore( SAVEDATA * sv )
{
	//return (FACTORYDATA*)SaveData_Get( sv, GMDATA_ID_FACTORY );
	FRONTIER_SAVEWORK* data = SaveData_Get( sv, GMDATA_ID_FRONTIER );
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
	SVLD_CheckCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
	return &data->factory.score;
}



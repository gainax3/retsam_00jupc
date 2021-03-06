//============================================================================================
/**
 * @file	sp_ribbon.c
 * @brief	配布リボンのセーブ関連
 * @author	Hiroyuki Nakamura
 * @date	2006.04.17
 */
//============================================================================================
#include "common.h"
#include "system/savedata.h"

#define	SP_RIBBON_H_GLOBAL
#include "savedata/sp_ribbon.h"


//============================================================================================
//	定数定義
//============================================================================================


//============================================================================================
//	プロトタイプ宣言
//============================================================================================


//--------------------------------------------------------------------------------------------
/**
 * 配布リボン用ワークのサイズ取得
 *
 * @param	none
 *
 * @return	ワークサイズ
 */
//--------------------------------------------------------------------------------------------
int SP_RIBBON_GetWorkSize(void)
{
	return ( HAIFU_RIBBON_MAX );
}

//--------------------------------------------------------------------------------------------
/**
 * 配布リボン用ワーク初期化
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void SP_RIBBON_Init( u8 * dat )
{
	memset( dat, 0, HAIFU_RIBBON_MAX );
}


//--------------------------------------------------------------------------------------------
/**
 * 配布リボン用ワークのポインタを取得
 *
 * @param	sv		セーブデータ
 *
 * @return	配布リボンデータ
 */
//--------------------------------------------------------------------------------------------
u8 * SaveData_GetSpRibbon( SAVEDATA * sv )
{
	return SaveData_Get( sv, GMDATA_ID_SP_RIBBON );
}

//==============================================================================
/**
 * @file	gds_debug.c
 * @brief	GDS通信のデバッグ関連
 * @author	matsuda
 * @date	2007.12.11(火)
 */
//==============================================================================
#include "common.h"
#include "gds_types.h"
#include "gds_battle_rec.h"
#include "gds_ranking.h"
#include "gds_boxshot.h"
#include "gds_dressup.h"

#include "savedata/imageclip_data.h"
#include "poketool/boxdata.h"

#include "savedata/gds_profile.h"

#include "gds_data_conv.h"
#include "gds_debug.h"



//==============================================================================
//	データ
//==============================================================================
static const GT_BOX_SHOT DebugDummyBoxData[] = {
	{
		0,
	},
};



//==============================================================================
//
//	ボックスショット
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   GTボックスショットのダミーデータを取得する
 *
 * @param   box_no		参照するボックス番号
 *
 * @retval  
 *
 */
//--------------------------------------------------------------
void GDS_DEBUG_BOX_GET_DummyBoxData(GT_BOX_SHOT *dest)
{
	MI_CpuCopy8(DebugDummyBoxData, dest, sizeof(DebugDummyBoxData));
}


//==============================================================================
//
//	ドレスアップ
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   GTドレスアップショットのダミーデータとして自分のテレビデータを取得する
 *
 * @param   sv			セーブデータへのポインタ
 * 
 * @retval	テレビデータへのポインタ
 */
//--------------------------------------------------------------
IMC_TELEVISION_SAVEDATA * GDS_DEBUG_DRESSUP_GET_DummyDressData(SAVEDATA *sv)
{
	IMC_SAVEDATA *imc;
	
	imc =  SaveData_GetImcSaveData(sv);
	return ImcSaveData_GetTelevisionSaveData(imc, IMC_SAVEDATA_TELEVISION_MYDATA);
}


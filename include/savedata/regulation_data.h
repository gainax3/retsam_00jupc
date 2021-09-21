//============================================================================================
/**
 * @file	regulation.h
 * @brief	バトルレギュレーションデータアクセス用ヘッダ
 * @author	k.ohno
 * @date	2006.5.24
 */
//============================================================================================
#ifndef __REGULATION_DATA_H__
#define __REGULATION_DATA_H__

#include "regulation.h"


//==============================================================================
//	定数定義
//==============================================================================
///レギュレーションNo
enum{
	REGULATION_NO_STANDARD,		///<スタンダードカップ
	REGULATION_NO_FANCY,		///<ファンシーカップ
	REGULATION_NO_LITTLE,		///<リトルカップ
	REGULATION_NO_LIGHT,		///<ライトカップ
	REGULATION_NO_DOUBLE,		///<ダブルカップ
	REGULATION_NO_ETC,			///<カスタムカップ(その他)
	
	REGULATION_NO_NULL = 0xff,	///<レギュレーション無し
};


//==============================================================================
//	外部関数宣言
//==============================================================================
// * @brief	レギュレーションデータへのポインタ取得
extern const REGULATION* Data_GetRegulation(SAVEDATA* pSave, int regNo);
// * @brief	レギュレーションデータの名前取得
extern void Data_GetRegulationName(SAVEDATA* pSave, int regNo, STRBUF* pStrBuff, int HeapID);
//制限無しのレギュレーションデータへのポインタ取得
extern const REGULATION* Data_GetNoLimitRegulation(void);
//レギュレーションデータからレギュレーションNOを取得
extern int Data_GetRegulationNo(const REGULATION *reg);


#endif //__REGULATION_DATA_H__

//============================================================================================
/**
 * @brief	coin_local.h
 * @brief	コイン操作
 * @date	2006.03.09
 * @author	tamada
 */
//============================================================================================

#ifndef	__CONTEST_SAVEDATA_LOCAL_H__
#define	__CONTEST_SAVEDATA_LOCAL_H__

//============================================================================================
//============================================================================================
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
struct _CONTEST_DATA{
	u16	TuushinRecord[CONTYPE_MAX][BREEDER_MAX];	///<コンテスト通信対戦成績(CONTYPE, 順位)
};

#endif	__CONTEST_SAVEDATA_LOCAL_H__

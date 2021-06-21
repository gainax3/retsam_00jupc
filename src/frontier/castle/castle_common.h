//==============================================================================================
/**
 * @file	castle_common.h
 * @brief	「バトルキャッスル」共通処理
 * @author	Satoshi Nohara
 * @date	07.10.2
 */
//==============================================================================================
#ifndef _CASTLE_COMMON_H_
#define _CASTLE_COMMON_H_

#include "common.h"
#include "savedata/castle_savedata.h"


//==============================================================================================
//
//	extern宣言
//
//==============================================================================================
extern u8 GetCsrPokePos( u8 h_max, u8 csr_pos );
extern u8 GetCsrSelType( u8 h_max, u8 csr_pos );
extern u8 GetCommSelCsrPos( u8 type_offset, u8 decide_type );
extern u8 Castle_GetRank( SAVEDATA* sv, u8 type, u8 rank_type );
extern void Castle_ListSeCall( u32 param, u16 no );
extern void Castle_SetPairName( WORDSET* wordset, u32 id );


#endif //_CASTLE_COMMON_H_



//============================================================================================
/**
 * @file	scr_roulette_sub.h
 * @bfief	スクリプトコマンド：バトルルーレット
 * @author	Satoshi Nohara
 * @date	07.09.06
 *
 * スクリプト関連とは切り離したい処理を置く
 */
//============================================================================================
#ifndef SCR_ROULETTE_SUB_H
#define SCR_ROULETTE_SUB_H

#include "../frontier/roulette_def.h"

//============================================================================================
//
//	通信用ワーク
//
//============================================================================================
typedef struct{
	u8	seq;
	u8	recieve_count;
	u16 dummy;

	u16 mine_monsno[ROULETTE_COMM_POKE_NUM];			//自分のポケモンナンバー格納
	u16 pair_monsno[ROULETTE_COMM_POKE_NUM];			//相手のポケモンナンバー格納

	u16* ret_wk;
}FLDSCR_ROULETTE_COMM;


//============================================================================================
//
//	extern宣言
//
//============================================================================================
extern void CommFldRouletteRecvMonsNo(int id_no,int size,void *pData,void *work);


#endif	/* SCR_ROULETTE_SUB_H */




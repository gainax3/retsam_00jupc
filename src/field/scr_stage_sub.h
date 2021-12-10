//============================================================================================
/**
 * @file	scr_stage_sub.h
 * @bfief	スクリプトコマンド：バトルステージ
 * @author	Satoshi Nohara
 * @date	06.06.13
 *
 * スクリプト関連とは切り離したい処理を置く
 */
//============================================================================================
#ifndef SCR_STAGE_SUB_H
#define SCR_STAGE_SUB_H

#include "savedata/stage_savedata.h"

//============================================================================================
//
//	通信用ワーク
//
//============================================================================================
typedef struct{
	u8	seq;
	u8	recieve_count;
	u16 dummy;

	u16 mine_monsno;			//自分のポケモンナンバー格納
	u16 pair_monsno;			//相手のポケモンナンバー格納

	u16* ret_wk;
}FLDSCR_STAGE_COMM;


//============================================================================================
//
//	extern宣言
//
//============================================================================================
extern void CommFldStageRecvMonsNo(int id_no,int size,void *pData,void *work);
extern void BattleStageSetNewChallenge( SAVEDATA* sv, STAGESCORE* wk, u8 type );


#endif	/* SCR_STAGE_SUB_H */



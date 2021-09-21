//============================================================================================
/**
 * @file	frontier_scrcmd.h
 * @bfief	フロンティアシステムスクリプトコマンド
 * @author	Satoshi Nohara
 * @date	07.04.10
 */
//============================================================================================
#ifndef	__FRONTIER_SCRCMD_H__
#define	__FRONTIER_SCRCMD_H__

#include "application/wifi_2dmap/wf2dmap_obj.h"
#include "application/wifi_2dmap/wf2dmap_cmdq.h"


//==============================================================================
//	構造体定義
//==============================================================================
///アニメーションリストデータ構造体
typedef struct{
	u16 code;		///<実行するアニメーションコマンドコード
	u16 num;		///<code実行回数
}FSSC_ANIME_DATA;

///アニメーションTCB動作用ワーク構造体
typedef struct{
	u8 seq;
	u8 num;
	s16 wait;
	u16 playid;
	u8 *anm_count_ptr;
	const FSSC_ANIME_DATA *anm_list;
	WF2DMAP_ACTCMDQ *actcmd_q;
	FSS_ACTOR_WORK *fss_actor;
}FSSC_ANIME_MOVE_WORK;


//==============================================================================
//	外部関数宣言
//==============================================================================
//extern inline u16 * FSSTGetWork(FSS_TASK *core);
//extern inline u16 FSSTGetWorkValue(FSS_TASK * core);
extern u16 * FSSTGetWork(FSS_TASK *core);
extern u16 FSSTGetWorkValue(FSS_TASK * core);
extern u16 FSS_GetEventWorkValue(FSS_TASK *core, u16 work_id );
extern u16 * FSS_GetEventWorkAdrs(FSS_TASK *core, u16 wk_id);


#endif	//__FRONTIER_SCRCMD_H__



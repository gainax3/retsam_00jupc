//============================================================================================
/**
 * @file	snd_def.h
 * @bfief	サウンド定義ファイル
 * @author	Satoshi Nohara
 * @date	08.04.03
 */
//============================================================================================
#ifndef __TORNWORLD_SND_DEF_H__
#define __TORNWORLD_SND_DEF_H__

#include "system/snd_tool.h"

//--------------------------------------------------------------------------------------------
//
//	SE定義
//
//--------------------------------------------------------------------------------------------
//定義で飛び石ジャンプ音無効
#define SE_YABURETA_JUMP2_STOP

//飛び石をジャンプする音
#define SE_YABURETA_JUMP2				( SEQ_SE_PL_JUMP2 )
//#define SE_YABURETA_JUMP2				( SEQ_SE_DP_DANSA5 )

//階層移動用　飛び石の音(ループ音)
#define SE_YABURETA_MOVE				( SEQ_SE_PL_FW089 )
//#define SE_YABURETA_MOVE				( SEQ_SE_PL_FW089B )

//第３層：水平垂直移動する　飛び石の音(ループ音)
//#define SE_YABURETA_MOVE2				( SEQ_SE_PL_FW089 )
#define SE_YABURETA_MOVE2				( SEQ_SE_PL_FW089B )

//第４層：出現する地形
#define SE_YABURETA_GROUND_APPEAR		( SEQ_SE_PL_SYUWA3 )

//第４層：消失する地形
#define SE_YABURETA_GROUND_DELETE		( SEQ_SE_PL_SYUWA3 )

//第５層：突然現れる木
#define SE_YABURETA_TREE_APPEAR			( SEQ_SE_PL_MEKI )

//第５層：突然消える木
#define SE_YABURETA_TREE_DELETE			( SEQ_SE_PL_MEKI2 )

//第５層：突然現れる岩
#define SE_YABURETA_ROCK_APPEAR			( SEQ_SE_PL_FW089_2 )

//第５層：突然消える岩
#define SE_YABURETA_ROCK_DELETE			( SEQ_SE_PL_FW089_2 )

//ギラティナシルエット飛行音
#define SE_YABURETA_GIRATINA_FLIGHT		( SEQ_SE_DP_FW019 )

//滝登り(ループ音)
#define SE_YABURETA_TAKINOBORI			( SEQ_SE_PL_FW463 )

//八層深部 ギラティナ降臨
#define SE_YABURETA_GIRATINA_ADVENT		( SEQ_SE_PL_GIRA )

//七層岩落とし　落下後の振動音
#define SE_YABURETA_ROCK_SHAKE			( SEQ_SE_DP_UG_008 )

#endif		// __TORNWORLD_SND_DEF_H__



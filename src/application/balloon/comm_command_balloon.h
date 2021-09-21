//=============================================================================
/**
 * @file	comm_command_balloon.h
 * @brief	通信のコマンド一覧  風船ミニゲーム用
 * @author	matsuda
 * @date    2007.11.26(月)
 */
//=============================================================================
#ifndef __COMM_COMMAND_BALLOON_H__
#define __COMM_COMMAND_BALLOON_H__

#include "communication/comm_command.h"
#include "balloon_game.h"
#include "balloon_comm_types.h"


/// 風船ミニゲーム専用通信コマンドの定義。
enum CommCommandBalloon_e {
  CB_EXIT_BALLOON = CS_COMMAND_MAX, ///<風船ミニゲームを終了しフィールドに戻る事を送信

	CB_SERVER_VERSION,			///<サーバーバージョンを伝える
	CB_PLAY_PARAM,				///<ゲームプレイ中データ
	CB_GAME_END,				///<ゲーム終了を伝える
	
  //------------------------------------------------ここまで
  CB_COMMAND_MAX   // 終端--------------これは移動させないでください
};

extern void CommCommandBalloonInitialize(void* pWork);


//==============================================================================
//	外部関数宣言
//==============================================================================
extern BOOL Send_CommBalloonPlayData(BALLOON_GAME_PTR game, BALLOON_SIO_PLAY_WORK *send_data);
extern BOOL Send_CommGameEnd(BALLOON_GAME_PTR game);
extern BOOL Send_CommServerVersion(BALLOON_GAME_PTR game);


#endif// __COMM_COMMAND_BALLOON_H__


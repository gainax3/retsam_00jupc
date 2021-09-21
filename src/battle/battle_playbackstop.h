//==============================================================================
/**
 * @file	battle_playbackstop.h
 * @brief	戦闘録画再生中の停止ボタン制御のヘッダ
 * @author	matsuda
 * @date	2007.07.25(水)
 */
//==============================================================================
#ifndef __BATTLE_PLAYBACKSTOP_H__
#define __BATTLE_PLAYBACKSTOP_H__


//==============================================================================
//	外部関数宣言
//==============================================================================
extern TCB_PTR PlayBackStopButton_Create(BATTLE_WORK *bw);
extern void PlayBackStopButton_Free(TCB_PTR tcb);


#endif	//__BATTLE_PLAYBACKSTOP_H__


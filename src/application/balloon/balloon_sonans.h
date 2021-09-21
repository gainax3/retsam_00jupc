//==============================================================================
/**
 * @file	balloon_sonans.h
 * @brief	風船割り：ソーナンス動作制御のヘッダ
 * @author	matsuda
 * @date	2007.11.14(水)
 */
//==============================================================================
#ifndef __BALLOON_SONANS_H__
#define __BALLOON_SONANS_H__


//==============================================================================
//	外部関数宣言
//==============================================================================
extern SONANS_SYS_PTR Sonas_Init(BALLOON_GAME_PTR game);
extern void Sonans_Exit(BALLOON_GAME_PTR game, SONANS_SYS_PTR sns);
extern void Sonans_Update(BALLOON_GAME_PTR game, SONANS_SYS_PTR sns);
extern void Sonans_VBlank(BALLOON_GAME_PTR game, SONANS_SYS_PTR sns);


#endif	//__BALLOON_SONANS_H__

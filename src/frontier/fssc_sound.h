//============================================================================================
/**
 * @file	fssc_sound.h
 * @bfief	フロンティアシステムスクリプトコマンド：サウンド
 * @author	Satoshi Nohara
 * @date	07.04.10
 */
//============================================================================================
#ifndef	__FSSC_SOUND_H__
#define	__FSSC_SOUND_H__


//============================================================================================
//
//	extern宣言
//
//============================================================================================
extern BOOL FSSC_SePlay( FSS_TASK * core );
extern BOOL FSSC_SeStop( FSS_TASK * core );
extern BOOL FSSC_SeWait(FSS_TASK * core);
extern BOOL FSSC_MePlay( FSS_TASK * core );
extern BOOL FSSC_MeWait( FSS_TASK * core );
extern BOOL FSSC_BgmPlay( FSS_TASK * core );
extern BOOL FSSC_BgmStop( FSS_TASK * core );


#endif	//__FSSC_SOUND_H__



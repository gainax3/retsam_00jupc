//============================================================================================
/**
 * @file	misc.h
 * @brief	未分類セーブデータを扱うためのヘッダ
 * @author	tamada
 * @date	2006.06.02
 *
 * 分類されていないセーブデータはとりあえずここに追加される。
 *
 * セーブデータとしての性質がきちんと定義できるようになった時点で、
 * それらのセーブ構造体はきちんと別ソースに切り分けられることになる。
 *
 * 2006.06.02
 * 時間切れで分類がわからんものはここに追加することにしました。
 */
//============================================================================================
#ifndef	__MISC_H__
#define	__MISC_H__

#include "savedata/savedata.h"
#include "gflib/msg_print.h"
#include "system/pms_data.h"

//============================================================================================
//============================================================================================
//----------------------------------------------------------
/**
 * @brief	自分状態データ型定義
 */
//----------------------------------------------------------
typedef struct _MISC MISC;

//============================================================================================
//============================================================================================
//----------------------------------------------------------
//----------------------------------------------------------
extern MISC * SaveData_GetMisc(SAVEDATA * sv);
extern const MISC * SaveData_GetMiscReadOnly(const SAVEDATA * sv);

//----------------------------------------------------------
//----------------------------------------------------------
extern const STRCODE * MISC_GetRivalName(const MISC * misc);
extern void MISC_SetRivalName(MISC * misc, STRBUF * str);

extern const STRCODE * MISC_GetMonumentName(const MISC * misc);
extern void MISC_SetMonumentName(MISC * misc, STRBUF * str);

extern void MISC_SetExtraInitFlag(MISC * misc);
extern u32 MISC_GetExtraInitFlag(const MISC * misc);
#ifdef PM_DEBUG
extern void MISC_ClearExtraInitFlag(MISC * misc);
#endif	//PM_DEBUG

extern void MISC_SetFavoriteMonsno(MISC * misc, int monsno, int form_no, int egg_flag);
extern void MISC_GetFavoriteMonsno(const MISC * misc, int *monsno, int *form_no, int *egg_flg);

extern void MISC_GetGdsSelfIntroduction(const MISC *misc, PMS_DATA *pms);
extern void MISC_SetGdsSelfIntroduction(MISC *misc, const PMS_DATA *pms);

extern void MISC_SetBattleRecoderColor( MISC * misc, u8 color );
extern void MISC_GetBattleRecoderColor( const MISC * misc, u8* color );

extern void MISC_ExtraSaveKeyGet(const MISC *misc, EXDATA_ID exdata_id, 
	EX_CERTIFY_SAVE_KEY *key, EX_CERTIFY_SAVE_KEY *old_key, u8 *flag);
extern void MISC_ExtraSaveKeySet(MISC *misc, EXDATA_ID exdata_id, 
	EX_CERTIFY_SAVE_KEY key, EX_CERTIFY_SAVE_KEY old_key, u8 flag);

//----------------------------------------------------------
//----------------------------------------------------------



#endif	__MISC_H__


//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		d_taya2.h
 *	@brief		田谷さん　デバックメニュー	常駐部分
 *	@author		tomoya takahashi
 *	@data		2007.04.18
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __D_TAYA2_H__
#define __D_TAYA2_H__


#include "common.h"
#include "fieldsys.h"
#include "field_event.h"

#undef GLOBAL
#ifdef	__D_TAYA2_H_GLOBAL
#define	GLOBAL	/* */
#else
#define	GLOBAL	extern
#endif

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------
/**
 * たんにフェードアウトして何か別画面の初期化を呼ぶだけ
 */
//-----------------------------------------------------------------
typedef void (*ScheneChangeCallback)(FIELDSYS_WORK*, void* wk);

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

typedef struct {
	u32					seq;
	ScheneChangeCallback	start_func;
	ScheneChangeCallback	end_func;
	BOOL				fadeFlag;
	void*				gen_wk;
}DEBUG_CHANGESCHENE_WORK;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

GLOBAL BOOL GMEVENT_ChangeScheneDebug(GMEVENT_CONTROL * event);

#undef	GLOBAL
#endif		// __D_TAYA2_H__


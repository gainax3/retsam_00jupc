//==============================================================================
/**
 * @file	demo_tengan.h
 * @brief	ŠÈ’P‚Èà–¾‚ğ‘‚­
 * @author	goto
 * @date	2007.11.05(Œ)
 *
 * ‚±‚±‚ÉFX‚È‰ğà“™‚ğ‘‚¢‚Ä‚à‚æ‚¢
 *
 */
//==============================================================================

#ifndef __DEMO_TENGAN_H__
#define __DEMO_TENGAN_H__

#include "savedata/config.h"
#include "savedata/mystatus.h"
#include "field/field_common.h"

typedef struct {

	int				player_sex;	///< «•Ê
	CONFIG*			cfg;		///< config
	const MYSTATUS*	my_status;	///< status
	
} DEMO_TENGAN_PARAM;


extern PROC_RESULT DemoTengan_Proc_Init( PROC * proc, int * seq );
extern PROC_RESULT DemoTengan_Proc_Main( PROC * proc, int * seq );
extern PROC_RESULT DemoTengan_Proc_Exit( PROC * proc, int * seq );

extern void EventCmd_DemoTanganProc( GMEVENT_CONTROL * event, SAVEDATA* save );

#endif
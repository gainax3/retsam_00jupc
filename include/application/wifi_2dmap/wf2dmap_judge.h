//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wf2dmap_judge.h
 *	@brief		コマンド判断モジュール＆リクエストコマンドバッファ
 *	@author		tomoya takahashi
 *	@data		2007.03.15
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WF2DMAP_JUDGE_H__
#define __WF2DMAP_JUDGE_H__

#include "application/wifi_2dmap/wf2dmap_common.h"
#include "application/wifi_2dmap/wf2dmap_obj.h"
#include "application/wifi_2dmap/wf2dmap_map.h"

#undef GLOBAL
#ifdef	__WF2DMAP_JUDGE_H_GLOBAL
#define	GLOBAL	/* */
#else
#define	GLOBAL	extern
#endif



//-----------------------------------------------------------------------------
/**
 *			コマンドジャッジモジュール
 */
//-----------------------------------------------------------------------------
GLOBAL BOOL WF2DMAP_JUDGESysCmdJudge( const WF2DMAP_MAPSYS* cp_mapsys, const WF2DMAP_OBJSYS* cp_objsys, const WF2DMAP_REQCMD* cp_req, WF2DMAP_ACTCMD* p_act );


#undef	GLOBAL
#endif		// __WF2DMAP_JUDGE_H__


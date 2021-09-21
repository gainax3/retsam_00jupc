#ifdef PM_DEBUG
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		d_mori2.h
 *	@brief		森さんのデバックメニュー常駐部分
 *	@author		tomoya takahashi
 *	@data		2007.04.17
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __D_MORI2_H__
#define __D_MORI2_H__

#include "common.h"
#include "application/namein.h"
#include "fieldsys.h"

#undef GLOBAL
#ifdef	__D_MORI2_H_GLOBAL
#define	GLOBAL	/* */
#else
#define	GLOBAL	extern
#endif

//-----------------------------------------------------------------------------
/**
 *		名前入力呼び出しテスト
 *		交換リスト呼び出し
 */
//-----------------------------------------------------------------------------
typedef struct{
	int 			seq;
	int 			sub;
	int				flag;
	GF_BGL_INI    	*bgl;
	FIELDSYS_WORK 	*fsys;
	NAMEIN_PARAM   *NameInParam;
}TEST_PROC_WORK;
GLOBAL void TestNameIn_Call( TCB_PTR tcb, void* work );
GLOBAL void TestTradeList_Call( TCB_PTR tcb, void* work );
GLOBAL void TestWorldTrade_Call( TCB_PTR tcb, void* work );

#undef	GLOBAL
#endif		// __D_MORI2_H__

#endif	// PM_DEBUG

//==============================================================================
/**
 * @file	hakai_warp.h
 * @brief	破れた世界突入デモのヘッダ
 * @author	matsuda
 * @date	2008.04.15(火)
 */
//==============================================================================
#ifndef __HAKAI_WARP_H__
#define __HAKAI_WARP_H__


//==============================================================================
//	外部関数宣言
//==============================================================================
extern PROC_RESULT HakaiWarpProc_Init( PROC * proc, int * seq );
extern PROC_RESULT HakaiWarpProc_Main( PROC * proc, int * seq );
extern PROC_RESULT HakaiWarpProc_End( PROC * proc, int * seq );


#endif	//__HAKAI_WARP_H__

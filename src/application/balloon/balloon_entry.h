//==============================================================================
/**
 * @file	balloon_entry.h
 * @brief	風船割り：エントリー画面のヘッダ
 * @author	matsuda
 * @date	2007.11.06(火)
 */
//==============================================================================
#ifndef __BALLOON_ENTRY_H__
#define __BALLOON_ENTRY_H__


//==============================================================================
//	外部関数宣言
//==============================================================================
extern PROC_RESULT BalloonEntryProc_Init( PROC * proc, int * seq );
extern PROC_RESULT BalloonEntryProc_Main( PROC * proc, int * seq );
extern PROC_RESULT BalloonEntryProc_End(PROC *proc, int *seq);


#endif	//__BALLOON_ENTRY_H__


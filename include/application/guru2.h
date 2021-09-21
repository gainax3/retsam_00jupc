//==============================================================================
/**
 * @file	guru2.h
 * @brief	‚®‚é‚®‚éŒðŠ·
 * @author	kagaya
 * @data	05.07.13
 */
//==============================================================================
#ifndef GURU2_H_FILE
#define GURU2_H_FILE

#include "system\procsys.h"
#include "savedata\config.h"

#include "savedata/savedata_def.h"
#include "field/comm_union_view_common.h"
#include "savedata/config.h"
#include "savedata/record.h"
#include "savedata/fnote_mem.h"

#include "field/field_common.h"

//==============================================================================
//	define
//==============================================================================

//==============================================================================
//	typedef
//==============================================================================
typedef struct _TAG_GURU2PROC_WORK GURU2PROC_WORK;

//--------------------------------------------------------------
///	GURU2_PARAM
//--------------------------------------------------------------
typedef struct
{
	int max;
	WINTYPE win_type;
	
	SAVEDATA 			*sv;
	COMM_UNIONROOM_VIEW *union_view;
	CONFIG				*config;
	RECORD				*record;
	FNOTE_DATA			*fnote;	
	
	FIELDSYS_WORK *fsys;
}GURU2_PARAM;

//==============================================================================
//	extern
//==============================================================================
extern GURU2PROC_WORK * Guru2_ProcInit( GURU2_PARAM *param, u32 heap_id );
extern void Guru2_ProcDelete( GURU2PROC_WORK *g2p );
extern BOOL Guru2_ReceiptRetCheck( GURU2PROC_WORK *g2p );

extern void * EventCmd_Guru2ProcWorkAlloc( FIELDSYS_WORK *fsys );
extern BOOL EventCmd_Guru2Proc( void *wk );

extern PROC_RESULT Guru2Receipt_Init( PROC *proc, int *seq );
extern PROC_RESULT Guru2Receipt_Main( PROC *proc, int *seq );
extern PROC_RESULT Guru2Receipt_End( PROC *proc, int *seq );

extern PROC_RESULT Guru2Main_Init( PROC *proc, int *seq );
extern PROC_RESULT Guru2Main_End( PROC *proc, int *seq );
extern PROC_RESULT Guru2Main_Main( PROC *proc, int *seq );

#endif //GURU2_H_FILE

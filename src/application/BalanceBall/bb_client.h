//==============================================================================
/**
 * @file	bb_client.h
 * @brief	ä»íPÇ»ê‡ñæÇèëÇ≠
 * @author	goto
 * @date	2007.09.25(âŒ)
 *
 * Ç±Ç±Ç…êFÅXÇ»âê‡ìôÇèëÇ¢ÇƒÇ‡ÇÊÇ¢
 *
 */
//==============================================================================

#ifndef __BB_CLIENT_H__
#define __BB_CLIENT_H__

#include "bb_common.h"

extern BB_CLIENT* BB_Client_AllocMemory( int comm_num, u32 netid, BB_SYS* sys );
extern void BB_Client_3D_PosInit( BB_CLIENT* wk );
extern void BB_Client_FreeMemory( BB_CLIENT* wk );
extern BOOL BB_Client_CountDown_Main( BB_CLIENT* wk );
extern BOOL BB_Client_IsSafety( BB_CLIENT* wk, int* state );
extern void BB_Manene_3D_Fall_TCB( TCB_PTR tcb, void* work );
extern void BB_Client_Manene_Action_TCB( TCB_PTR tcb, void* work );
extern void BB_Client_Manene_3D_Recover_Action_TCB( TCB_PTR tcb, void* work );

extern void BB_Client_GameCore( BB_CLIENT* wk );
extern void BB_Client_TouchPanel_Main( BB_CLIENT* wk );
extern void BB_Client_GameReset( BB_CLIENT* wk );
extern void BB_Client_GameFeverUpdate( BB_CLIENT* wk );
extern void BB_Client_GameUpdate( BB_CLIENT* wk );
extern BOOL BB_Client_JumpOnToBall( BB_CLIENT* wk );

#endif


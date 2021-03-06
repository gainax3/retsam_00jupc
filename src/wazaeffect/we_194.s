//==============================================================================
/**
 * @file	we_194.s
 * @brief	みちづれ			194
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_MITIDURE
	
// =============================================================================
//
//
//	■みちづれ			194
//
//
// =============================================================================
WEST_MITIDURE:

	LOAD_PARTICLE_DROP	0,W_194_SPA
	ADD_PARTICLE 	0,W_194_194_MICHI_BALL, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_194, OPERATOR_AXIS_194, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE

	SIDE_JP			0, SIDE_MINE, SIDE_ENEMY
/*
	SE_C			SEQ_SE_DP_W060
	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 0, 12, WE_PAL_BLACK,
	SE_FLOW_LR		SEQ_SE_DP_W109
	WAIT_FLAG
	
	WAIT			15
	
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_M1, 0, 1, WE_PAL_WHITE, 12,0
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, SHAKE_S_X, 0, SHAKE_S_WAIT, SHAKE_S_NUM,  WE_TOOL_M1 | WE_TOOL_SSP,
	
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_E1, 0, 1, WE_PAL_WHITE, 12,0
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_E2, 0, 1, WE_PAL_WHITE, 12,0
	
	SE_C			SEQ_SE_DP_W171
	WAIT_FLAG
	
	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 12, 0, WE_PAL_BLACK,
	WAIT_FLAG

	WAIT_PARTICLE
	EXIT_PARTICLE	0,
*/
	SEQEND


SIDE_MINE:

	SE_C			SEQ_SE_DP_W060
	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 0, 12, WE_PAL_BLACK,
	SE_FLOW_LR		SEQ_SE_DP_W109
	WAIT_FLAG
	
	WAIT			15
	
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_M1, 0, 1, WE_PAL_WHITE, 12,0
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, SHAKE_S_X, 0, SHAKE_S_WAIT, SHAKE_S_NUM,  WE_TOOL_M1 | WE_TOOL_SSP,
	
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_B, 0, 1, WE_PAL_WHITE, 12,0
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_D, 0, 1, WE_PAL_WHITE, 12,0
	
	SE_C			SEQ_SE_DP_W171
	WAIT_FLAG
	
	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 12, 0, WE_PAL_BLACK,
	WAIT_FLAG

	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	SEQEND

SIDE_ENEMY:

	SE_C			SEQ_SE_DP_W060
	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 0, 12, WE_PAL_BLACK,
	SE_FLOW_LR		SEQ_SE_DP_W109
	WAIT_FLAG
	
	WAIT			15
	
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_M1, 0, 1, WE_PAL_WHITE, 12,0
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, SHAKE_S_X, 0, SHAKE_S_WAIT, SHAKE_S_NUM,  WE_TOOL_M1 | WE_TOOL_SSP,
	
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_A, 0, 1, WE_PAL_WHITE, 12,0
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_C, 0, 1, WE_PAL_WHITE, 12,0
	
	SE_C			SEQ_SE_DP_W171
	WAIT_FLAG
	
	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 12, 0, WE_PAL_BLACK,
	WAIT_FLAG

	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	SEQEND

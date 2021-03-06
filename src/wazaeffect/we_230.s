//==============================================================================
/**
 * @file	we_230.s
 * @brief	あまいかおり			230
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_AMAIKAORI
	
// =============================================================================
//
//
//	■あまいかおり			230
//
//
// =============================================================================
WEST_AMAIKAORI:
	
	SIDE_JP		0, SIDE_MINE, SIDE_ENEMY
	
	SEQEND

SIDE_MINE:
	LOAD_PARTICLE_DROP	0,W_230_SPA
	
	SE_C			SEQ_SE_DP_W230

	ADD_PARTICLE 	0,W_230_230_AMAIKAORI_HANA, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_SET, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, -2000, 8000, 0
	
	ADD_PARTICLE 	0,W_230_230_AMAIKAORI_MIST, EMTFUNC_DUMMY
	
	ADD_PARTICLE 	0,W_230_230_AMAIKAORI_RING, EMTFUNC_DEFENCE_POS
	
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 5, WE_TOOL_E1, 0, 2, WE_PAL_PINK, 12
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 5, WE_TOOL_E2, 0, 2, WE_PAL_PINK, 12

	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	SEQEND
	
SIDE_ENEMY:

	LOAD_PARTICLE_DROP	0,W_230_SPA
	
	SE_C			SEQ_SE_DP_W230

	ADD_PARTICLE 	0,W_230_230_AMAIKAORI_HANA2, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_SET, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, -2000, 8000, 0
	
	ADD_PARTICLE 	0,W_230_230_AMAIKAORI_MIST2, EMTFUNC_DUMMY
	
	ADD_PARTICLE 	0,W_230_230_AMAIKAORI_RING, EMTFUNC_DEFENCE_POS
	
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 5, WE_TOOL_E1, 0, 2, WE_PAL_PINK, 12
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 5, WE_TOOL_E2, 0, 2, WE_PAL_PINK, 12

	WAIT_PARTICLE
	EXIT_PARTICLE	0,
		
	SEQEND

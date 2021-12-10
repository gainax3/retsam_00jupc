//==============================================================================
/**
 * @file	we_188.s
 * @brief	ヘドロばくだん			188
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_HEDOROBAKUDAN
	
// =============================================================================
//
//
//	■ヘドロばくだん			188
//
//
// =============================================================================
WEST_HEDOROBAKUDAN:

	LOAD_PARTICLE_DROP	0,W_188_SPA

	ADD_PARTICLE_EMIT_SET 	0,1,W_188_188_HEDOBOMB_KEMURI1, EMTFUNC_ATTACK_POS
	FUNC_CALL		WEST_SP_EMIT_PARABOLIC, 6, 1, 0, 0, 0, 14, 64,
	
	SE_REPEAT_C		SEQ_SE_DP_W145B,2,4

	WAIT			10
	
	//SE_REPEAT_R		SEQ_SE_DP_W091,2,8
	SE_REPEAT_R		SEQ_SE_DP_W092D,2,12

	ADD_PARTICLE 	0,W_188_188_HEDOBOMB_BUBBLE, EMTFUNC_DEFENCE_POS
	ADD_PARTICLE 	0,W_188_188_HEDOBOMB_BOMB,	  EMTFUNC_DEFENCE_POS
	
	WAIT			5
	POKE_FADE_EZ	WE_PAL_POISON, WE_TOOL_E1, 2


	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	SEQEND

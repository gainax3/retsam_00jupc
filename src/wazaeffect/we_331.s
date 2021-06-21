//==============================================================================
/**
 * @file	we_331.s
 * @brief	タネマシンガン			331
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_TANEMASINGAN
	
// =============================================================================
//
//
//	■タネマシンガン			331
//
//
// =============================================================================
#define W331_LIFE	(10)

WEST_TANEMASINGAN:
	
	LOAD_PARTICLE_DROP	0,W_331_SPA
	
	//SE_REPEAT_C		SEQ_SE_DP_W202B,2,8
	SE_REPEAT_C		SEQ_SE_DP_W202B,3,9
	
	ADD_PARTICLE_EMIT_SET 	0,0,W_331_331_TANE_BEAM,	EMTFUNC_ATTACK_POS
	FUNC_CALL				WEST_SP_EMIT_STRAIGHT, 6, 0, 0, 0, 0, 10, 64,
	WAIT					2
	
	ADD_PARTICLE_EMIT_SET 	0,1,W_331_331_TANE_BEAM,	EMTFUNC_ATTACK_POS
	FUNC_CALL				WEST_SP_EMIT_STRAIGHT, 6, 1, 0, 0, 0, 10, 64,
	WAIT					2

	ADD_PARTICLE 	0,W_331_331_TANE_BURN,	EMTFUNC_DEFENCE_POS
	ADD_PARTICLE 	0,W_331_331_TANE_HIT,	EMTFUNC_DEFENCE_POS
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, SHAKE_M_X, 0, SHAKE_M_WAIT, SHAKE_M_NUM*5,  WE_TOOL_E1 | WE_TOOL_SSP,
	
	ADD_PARTICLE_EMIT_SET 	0,2,W_331_331_TANE_BEAM,	EMTFUNC_ATTACK_POS
	FUNC_CALL				WEST_SP_EMIT_STRAIGHT, 6, 2, 0, 0, 0, 10, 64,
	WAIT					2
	
	ADD_PARTICLE_EMIT_SET 	0,3,W_331_331_TANE_BEAM,	EMTFUNC_ATTACK_POS
	FUNC_CALL				WEST_SP_EMIT_STRAIGHT, 6, 3, 0, 0, 0, 10, 64,
	SE_REPEAT_R				SEQ_SE_DP_W025B,5,6
	WAIT					2

	ADD_PARTICLE_EMIT_SET 	0,4,W_331_331_TANE_BEAM,	EMTFUNC_ATTACK_POS
	FUNC_CALL				WEST_SP_EMIT_STRAIGHT, 6, 4, 0, 0, 0, 10, 64,
	WAIT					2
	
	ADD_PARTICLE_EMIT_SET 	0,5,W_331_331_TANE_BEAM,	EMTFUNC_ATTACK_POS
	FUNC_CALL				WEST_SP_EMIT_STRAIGHT, 6, 5, 0, 0, 0, 10, 64,
	WAIT					2
	
	ADD_PARTICLE_EMIT_SET 	0,6,W_331_331_TANE_BEAM,	EMTFUNC_ATTACK_POS
	FUNC_CALL				WEST_SP_EMIT_STRAIGHT, 6, 6, 0, 0, 0, 10, 64,
	WAIT					2
	
	ADD_PARTICLE_EMIT_SET 	0,7,W_331_331_TANE_BEAM,	EMTFUNC_ATTACK_POS
	FUNC_CALL				WEST_SP_EMIT_STRAIGHT, 6, 7, 0, 0, 0, 10, 64,
	WAIT					2

	ADD_PARTICLE_EMIT_SET 	0,0,W_331_331_TANE_BEAM,	EMTFUNC_ATTACK_POS
	FUNC_CALL				WEST_SP_EMIT_STRAIGHT, 6, 0, 0, 0, 0, 10, 64,
	WAIT					2
	
	ADD_PARTICLE_EMIT_SET 	0,1,W_331_331_TANE_BEAM,	EMTFUNC_ATTACK_POS
	FUNC_CALL				WEST_SP_EMIT_STRAIGHT, 6, 1, 0, 0, 0, 10, 64,
	WAIT					2
	
	ADD_PARTICLE_EMIT_SET 	0,2,W_331_331_TANE_BEAM,	EMTFUNC_ATTACK_POS
	FUNC_CALL				WEST_SP_EMIT_STRAIGHT, 6, 2, 0, 0, 0, 10, 64,
	WAIT					2

	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	SEQEND

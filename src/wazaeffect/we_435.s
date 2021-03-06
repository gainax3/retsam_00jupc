//==============================================================================
/**
 * @file	we_001.s
 * @brief	はたく
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_Hataku
	
// =============================================================================
//
//
//	■はたく
//
//
// =============================================================================
WEST_Hataku:

	LOAD_PARTICLE_DROP	0,W_435_SPA

	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 0, 12, WE_PAL_BLACK,
	WAIT_FLAG
	
	ADD_PARTICLE 	0,W_435_435_HOUDEN_BALL,	 EMTFUNC_ATTACK_POS
	ADD_PARTICLE 	0,W_435_435_HOUDEN_THUNDER1, EMTFUNC_ATTACK_POS
	ADD_PARTICLE 	0,W_435_435_HOUDEN_THUNDER2, EMTFUNC_ATTACK_POS

	WAIT			2
	SE_REPEAT_L		SEQ_SE_DP_W085B,4,7
	WAIT			28
	
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, SHAKE_M_X, 0, SHAKE_M_WAIT, SHAKE_M_NUM,  WE_TOOL_OTHER | WE_TOOL_SSP
	
	ADD_PARTICLE 	0,W_435_435_HOUDEN_HIT,		 EMTFUNC_DEFENCE_POS
	SE_WAITPLAY_R	SEQ_SE_DP_W085,5
	POKE_FADE_EZ	WE_PAL_YELLOW, WE_TOOL_E1, 1
	POKE_FADE_EZ	WE_PAL_YELLOW, WE_TOOL_E2, 1
		
	WAIT_PARTICLE
	EXIT_PARTICLE	0,


	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 12, 0, WE_PAL_BLACK,
	WAIT_FLAG
	
	SEQEND


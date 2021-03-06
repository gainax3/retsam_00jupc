//==============================================================================
/**
 * @file	we_271.s
 * @brief	トリック			271
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_TORIKKU
	
// =============================================================================
//
//
//	■トリック			271
//
//
// =============================================================================

#define W271_OAM_MAX	(2)
#define W271_SE_OFS		( 15 + 10 )

WEST_TORIKKU:

	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 0, 12, WE_PAL_BLACK,
	
	SE_L			SEQ_SE_DP_166
	
	CATS_RES_INIT			0, W271_OAM_MAX, 1,1,1,1,0,0
	CATS_CAHR_RES_LOAD		0, EFFECT_271_NCGR_BIN
	CATS_PLTT_RES_LOAD		0, EFFECT_271_NCLR, 1
	CATS_CELL_RES_LOAD		0, EFFECT_271_NCER_BIN
	CATS_CELL_ANM_RES_LOAD	0, EFFECT_271_NANR_BIN
	CATS_ACT_ADD			0, WEST_CSP_WE_271,  EFFECT_271_NCGR_BIN, EFFECT_271_NCLR, EFFECT_271_NCER_BIN, EFFECT_271_NANR_BIN, 0, 0, 1, W271_OAM_MAX
	
	//SE_WAITPLAY_C			SEQ_SE_DP_W145B,70
	//SE_WAITPLAY_C			SEQ_SE_DP_W145B,75
	//SE_WAITPLAY_C			SEQ_SE_DP_W145B,73-W271_SE_OFS
	SE_WAITPLAY_C			SEQ_SE_DP_W104,80-W271_SE_OFS
	SE_WAITPLAY_C			SEQ_SE_DP_W104,95-W271_SE_OFS
	SE_WAITPLAY_C			SEQ_SE_DP_W104,108-W271_SE_OFS
	SE_WAITPLAY_C			SEQ_SE_DP_W104,122-W271_SE_OFS
	SE_WAITPLAY_C			SEQ_SE_DP_W104,130-W271_SE_OFS
	WAIT_FLAG
	
	CATS_RES_FREE			0

	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 12, 0, WE_PAL_BLACK,
	WAIT_FLAG
	
	SEQEND

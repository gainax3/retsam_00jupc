//==============================================================================
/**
 * @file	we_143.s
 * @brief	ゴッドバード			143
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_GODDOBAADO
	
// =============================================================================
//
//
//	■ゴッドバード			143
//
//
// =============================================================================

#define SCROLL_SPEED_X		(0)
#define SCROLL_SPEED_Y		(-20)//20)

WEST_GODDOBAADO:

	TURN_CHK	WEST_143_TURN_1, WEST_143_TURN_2

	SEQEND

///< turn1
WEST_143_TURN_1:

	LOAD_PARTICLE_DROP	0,W_143_SPA	
	ADD_PARTICLE 	0,W_143_143_GODBARD_CHARGE, EMTFUNC_ATTACK_POS
	
	///< パーティアタック
	PTAT_JP			PTAT
	
	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 0, 10, WE_PAL_BLACK
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_M1, 0, 1, WE_PAL_BLACK, 10,0
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_M2, 0, 1, WE_PAL_BLACK, 10,30
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_E2, 0, 1, WE_PAL_BLACK, 10,30
	
	SE_REPEAT_L		SEQ_SE_DP_W360,12,2		
	WAIT			25
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_M1, 0, 1, WE_PAL_WHITE, 12,0
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, 1, 0, 1, 6,  WE_TOOL_M1 | WE_TOOL_SSP,
	WAIT_FLAG
	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 10, 0, WE_PAL_BLACK
	WAIT_FLAG

	POKEOAM_RES_FREE
	POKEOAM_DROP_RESET	WEDEF_POKE_CAP_ID_0
	
	SEQEND

PTAT:

	SIDE_JP			0, SIDE_MINE, SIDE_ENEMY
	
	SEQEND

SIDE_MINE:
	
	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 0, 10, WE_PAL_BLACK
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_M1, 0, 1, WE_PAL_BLACK, 10,0
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_B, 0, 1, WE_PAL_BLACK, 10,30
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_D, 0, 1, WE_PAL_BLACK, 10,30
	
	SE_REPEAT_L		SEQ_SE_DP_W360,12,2		
	WAIT			25
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_M1, 0, 1, WE_PAL_WHITE, 12,0
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, 1, 0, 1, 6,  WE_TOOL_M1 | WE_TOOL_SSP,
	WAIT_FLAG
	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 10, 0, WE_PAL_BLACK
	WAIT_FLAG

	POKEOAM_RES_FREE
	POKEOAM_DROP_RESET	WEDEF_POKE_CAP_ID_0
	
	SEQEND

SIDE_ENEMY:
	
	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 0, 10, WE_PAL_BLACK
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_M1, 0, 1, WE_PAL_BLACK, 10,0
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_A, 0, 1, WE_PAL_BLACK, 10,30
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_C, 0, 1, WE_PAL_BLACK, 10,30
	
	SE_REPEAT_L		SEQ_SE_DP_W360,12,2		
	WAIT			25
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_M1, 0, 1, WE_PAL_WHITE, 12,0
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, 1, 0, 1, 6,  WE_TOOL_M1 | WE_TOOL_SSP,
	WAIT_FLAG
	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 10, 0, WE_PAL_BLACK
	WAIT_FLAG

	POKEOAM_RES_FREE
	POKEOAM_DROP_RESET	WEDEF_POKE_CAP_ID_0
	
	SEQEND

///< turn2
WEST_143_TURN_2:

	///< 背景
	LOAD_PARTICLE_DROP	0,W_143_SPA
	
	WORK_SET		WEDEF_GP_INDEX_SPEED_X,  WEDEF_HAIKEI_SPEED_X
	WORK_SET		WEDEF_GP_INDEX_SPEED_Y,  WEDEF_HAIKEI_SPEED_Y
	WORK_SET		WEDEF_GP_INDEX_SPEED_R,  1
	WORK_SET		WEDEF_GP_INDEX_SCREEN_R, 1
	WORK_SET		WEDEF_GP_INDEX_FADE_TYPE,  0
	HAIKEI_CHG		BG_ID_143, WEDEF_HAIKEI_MODE_FADE | WEDEF_HAIKEI_SBIT_MOVE
	//SE_L			SEQ_SE_DP_W327
	HAIKEI_CHG_WAIT

	///< 半透明フェードポケ
	POKEOAM_RES_INIT
	POKEOAM_RES_LOAD	0,
	POKEOAM_DROP	WEDEF_DROP_M1, WEDEF_POKE_AUTO_OFF, WEDEF_POKE_CAP_ID_0, WEDEF_POKE_RES_0
	PT_DROP_EX		WEDEF_DROP_M2, WEDEF_DROP_M2,
	
	FUNC_CALL		WEST_SP_WE_CAP_ALPHA_FADE, 6, 1, 16, 0, 0, 16, 8, 0, 0
	WAIT 1
	FUNC_CALL		WEST_SP_WE_SSP_POKE_VANISH, 2, WE_TOOL_M1, 1
	WAIT_FLAG
	POKE_OAM_ENABLE	WEDEF_POKE_CAP_ID_0, 0,
	POKE_OAM_ENABLE	WEDEF_POKE_CAP_ID_EX, 0,


	SE_FLOW_LR		SEQ_SE_DP_W360C

	CONTEST_JP		CONTEST

	SIDE_JP			0,WEST_143_SIDE_MINE, WEST_143_SIDE_ENEMY

	SEQEND

///< side mine
WEST_143_SIDE_MINE:
	
	ADD_PARTICLE 	0,W_143_143_GODBARD_FIRE1A, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE,OPERATOR_TARGET_DF, OPERATOR_POS_LSP, OPERATOR_AXIS_AT, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	
	ADD_PARTICLE 	0,W_143_143_GODBARD_BALL1,	EMTFUNC_ATTACK_POS
	ADD_PARTICLE 	0,W_143_143_GODBARD_BALL2,	EMTFUNC_ATTACK_POS

	WAIT			5
	SE_R			SEQ_SE_DP_186
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, 4, 0, 1, 6,  WE_TOOL_E1 | WE_TOOL_SSP,
	ADD_PARTICLE 	0,W_143_143_GODBARD_HIT,	EMTFUNC_DEFENCE_POS

	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	POKE_OAM_ENABLE	WEDEF_POKE_CAP_ID_0, 1,
	POKE_OAM_ENABLE	WEDEF_POKE_CAP_ID_EX, 1,
	FUNC_CALL		WEST_SP_WE_CAP_ALPHA_FADE, 6, 1, 0, 16, 16, 0, 8, 0, 0
	WAIT			7
	FUNC_CALL		WEST_SP_WE_SSP_POKE_VANISH, 2, WE_TOOL_M1, 0

	WORK_SET		WEDEF_GP_INDEX_SPEED_X,  WEDEF_HAIKEI_SPEED_X
	WORK_SET		WEDEF_GP_INDEX_SPEED_Y,  WEDEF_HAIKEI_SPEED_Y
	WORK_SET		WEDEF_GP_INDEX_SPEED_R,  1
	WORK_SET		WEDEF_GP_INDEX_SCREEN_R, 1
	WORK_SET		WEDEF_GP_INDEX_FADE_TYPE,  0
	HAIKEI_RECOVER	BG_ID_143, WEDEF_HAIKEI_RC_MODE_FADE | WEDEF_HAIKEI_SBIT_STOP
	HAIKEI_CHG_WAIT

	POKEOAM_RES_FREE
	POKEOAM_DROP_RESET	WEDEF_POKE_CAP_ID_0
	PT_DROP_RESET_EX
	
	SEQEND

CONTEST:

	ADD_PARTICLE 	0,W_143_143_GODBARD_FIRE1C, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE,OPERATOR_TARGET_DF, OPERATOR_POS_SP, OPERATOR_AXIS_AT, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	
	ADD_PARTICLE 	0,W_143_143_GODBARD_BALL1,	EMTFUNC_ATTACK_POS
	ADD_PARTICLE 	0,W_143_143_GODBARD_BALL2,	EMTFUNC_ATTACK_POS

	WAIT			5
	SE_R			SEQ_SE_DP_186
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, 4, 0, 1, 6,  WE_TOOL_E1 | WE_TOOL_SSP,
	ADD_PARTICLE 	0,W_143_143_GODBARD_HIT,	EMTFUNC_DEFENCE_POS

	WAIT_PARTICLE
	EXIT_PARTICLE	0,

	POKE_OAM_ENABLE	WEDEF_POKE_CAP_ID_0, 1,
	POKE_OAM_ENABLE	WEDEF_POKE_CAP_ID_EX, 1,
	FUNC_CALL		WEST_SP_WE_CAP_ALPHA_FADE, 6, 1, 0, 16, 16, 0, 8, 0, 0
	WAIT			7
	FUNC_CALL		WEST_SP_WE_SSP_POKE_VANISH, 2, WE_TOOL_M1, 0

	WORK_SET		WEDEF_GP_INDEX_SPEED_X,  WEDEF_HAIKEI_SPEED_X
	WORK_SET		WEDEF_GP_INDEX_SPEED_Y,  WEDEF_HAIKEI_SPEED_Y
	WORK_SET		WEDEF_GP_INDEX_SPEED_R,  1
	WORK_SET		WEDEF_GP_INDEX_SCREEN_R, 1
	WORK_SET		WEDEF_GP_INDEX_FADE_TYPE,  0
	HAIKEI_RECOVER	BG_ID_143, WEDEF_HAIKEI_RC_MODE_FADE | WEDEF_HAIKEI_SBIT_STOP
	HAIKEI_CHG_WAIT

	POKEOAM_RES_FREE
	POKEOAM_DROP_RESET	WEDEF_POKE_CAP_ID_0
	PT_DROP_RESET_EX
	
	SEQEND

///< side enemy
WEST_143_SIDE_ENEMY:

	ADD_PARTICLE 	0,W_143_143_GODBARD_FIRE1B, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE,OPERATOR_TARGET_DF, OPERATOR_POS_LSP, OPERATOR_AXIS_AT, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE

	ADD_PARTICLE 	0,W_143_143_GODBARD_BALL1,	EMTFUNC_ATTACK_POS
	ADD_PARTICLE 	0,W_143_143_GODBARD_BALL2,	EMTFUNC_ATTACK_POS
//	ADD_PARTICLE 	0,W_143_143_GODBARD_HIT,	EMTFUNC_ATTACK_POS
	
	WAIT			5
	SE_R			SEQ_SE_DP_186
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, 4, 0, 1, 6,  WE_TOOL_E1 | WE_TOOL_SSP,
	ADD_PARTICLE 	0,W_143_143_GODBARD_HIT,	EMTFUNC_DEFENCE_POS

	WAIT_PARTICLE
	EXIT_PARTICLE	0,

	POKE_OAM_ENABLE	WEDEF_POKE_CAP_ID_0, 1,
	POKE_OAM_ENABLE	WEDEF_POKE_CAP_ID_EX, 1,
	FUNC_CALL		WEST_SP_WE_CAP_ALPHA_FADE, 6, 1, 0, 16, 16, 0, 8, 0, 0
	WAIT			7
	FUNC_CALL		WEST_SP_WE_SSP_POKE_VANISH, 2, WE_TOOL_M1, 0

	WORK_SET		WEDEF_GP_INDEX_SPEED_X,  WEDEF_HAIKEI_SPEED_X
	WORK_SET		WEDEF_GP_INDEX_SPEED_Y,  WEDEF_HAIKEI_SPEED_Y
	WORK_SET		WEDEF_GP_INDEX_SPEED_R,  1
	WORK_SET		WEDEF_GP_INDEX_SCREEN_R, 1
	WORK_SET		WEDEF_GP_INDEX_FADE_TYPE,  0
	HAIKEI_RECOVER	BG_ID_143, WEDEF_HAIKEI_RC_MODE_FADE | WEDEF_HAIKEI_SBIT_STOP
	HAIKEI_CHG_WAIT

	POKEOAM_RES_FREE
	POKEOAM_DROP_RESET	WEDEF_POKE_CAP_ID_0
	PT_DROP_RESET_EX
	
	SEQEND


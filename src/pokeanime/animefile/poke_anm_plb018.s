//==============================================================================
/**
 * @file	poke_anm018.s
 * @brief	
 * @author	
 * @date	
 *
 */
//==============================================================================

	.text
	
	.include	"past.h"
	.include	"past_def.h"
	
	.global		PAST_ANIME_PLB018
	
// =============================================================================
//
//
//	揺れながら上下
//
//
// =============================================================================
#define ROT_1	( 0x3e8 )
#define OFS_Y	( 4 )
PAST_ANIME_PLB018:

	//50回の処理で左右0xcccの角度傾ける　左右２回ずつ行うので2周分(0x20000)
	CALL_MF_CURVE_DIVTIME APPLY_SET,0,CURVE_SIN_MINUS,TARGET_ROT,ROT_1,0x20000,0,50
	
	//傾きにあわせて縦移動
	CALL_MF_CURVE_DIVTIME APPLY_SET,0,CURVE_SIN,TARGET_DY,OFS_Y,0x10000,0,50
	HOLD_CMD

	END_ANM

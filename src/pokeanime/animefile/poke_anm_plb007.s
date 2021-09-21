//==============================================================================
/**
 * @file	poke_anm007.s
 * @brief	
 * @author	
 * @date	
 *
 */
//==============================================================================

	.text
	
	.include	"past.h"
	.include	"past_def.h"
	
	.global		PAST_ANIME_PLB007
	
// =============================================================================
//
//
//	ピョンピョン	
//
//
// =============================================================================
#define OFS_Y1	( 6 - 1 )	///< ふり幅
#define OFS_Y2	( 9 - 2 )	///< ふり幅 背面用なので、8以下に指定

PAST_ANIME_PLB007:
	START_LOOP	2
		CALL_MF_CURVE	APPLY_SET,0,CURVE_SIN_MINUS,TARGET_DY,OFS_Y1,0x2000,0,4
		HOLD_CMD
	END_LOOP
	
	CALL_MF_CURVE	APPLY_SET,0,CURVE_SIN_MINUS,TARGET_DY,OFS_Y2,0x2000,0,4
	HOLD_CMD
	END_ANM

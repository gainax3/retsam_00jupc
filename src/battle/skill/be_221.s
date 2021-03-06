//============================================================================
/**
 *
 *@file		be_221.s
 *@brief	í¬V[PXiBattleEffectj
 *			221@GÌgoª¼ªÈºÌêAZÌÐÍªQ{ÉÈéB
 *
 *@author	HisashiSogabe
 *@data		2006.02.10
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

BE_221:
	PSP_VALUE_WORK		VAL_GET,SIDE_DEFENCE,ID_PSP_hpmax,BUF_PARA_CALC_WORK
	VALUE				VAL_DIV,BUF_PARA_CALC_WORK,2
	IF_PSP_WORK			IF_FLAG_C,SIDE_DEFENCE,ID_PSP_hp,BUF_PARA_CALC_WORK,BE_221_NO_HALF
	VALUE				VAL_SET,BUF_PARA_DAMAGE_VALUE,20
	BRANCH				BE_221_NEXT
BE_221_NO_HALF:
	VALUE				VAL_SET,BUF_PARA_DAMAGE_VALUE,10
BE_221_NEXT:
	CRITICAL_CHECK
	DAMAGE_CALC
	SEQ_END

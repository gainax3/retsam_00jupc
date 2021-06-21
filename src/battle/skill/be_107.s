//============================================================================
/**
 *
 *@file		be_107.s
 *@brief	戦闘シーケンス（BattleEffect）
 *			107　敵が「ねむり」状態の時だけ効果のある技（「ねむり」状態でないと技は失敗する）、
 *				 敵ポケモンあくむ状態にする	
 *
 *@author	HisashiSogabe
 *@data		2006.01.27
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

BE_107:
#if AFTER_MASTER_070405_BT_FIX
	//みがわりがでていたら、うまくきまらん
	MIGAWARI_CHECK	SIDE_DEFENCE,Umakukimaran
	//すでにあくむにかかっていたら、うまくきまらん
	IF_PSP			IF_FLAG_BIT,SIDE_DEFENCE,ID_PSP_condition2,CONDITION2_AKUMU,Umakukimaran
	//ねむっていなかったら、うまくきまらん
	IF_PSP			IF_FLAG_NBIT,SIDE_DEFENCE,ID_PSP_condition,CONDITION_NEMURI,Umakukimaran
#endif //AFTER_MASTER_070405_BT_FIX
	VALUE		VAL_SET,BUF_PARA_ADD_STATUS_DIRECT,ADD_COND2_AKUMU|ADD_STATUS_WAZAKOUKA
	SEQ_END

#if AFTER_MASTER_070405_BT_FIX
Umakukimaran:
	VALUE		VAL_BIT,BUF_PARA_WAZA_STATUS_FLAG,WAZA_STATUS_FLAG_UMAKUKIMARAN
	SEQ_END
#endif //AFTER_MASTER_070405_BT_FIX

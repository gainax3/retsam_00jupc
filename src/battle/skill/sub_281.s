
//============================================================================
/**
 *
 *@file		sub_281.s
 *@brief	戦闘シーケンス（BattleEffect）
 *			攻撃対象がいなくて当たらない
 *
 *@author	HisashiSogabe
 *@data		2006.07.04
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

SUB_281:
	ATTACK_MESSAGE
	SERVER_WAIT
	WAIT			MSG_WAIT
	MESSAGE			NoPokemonMsg,TAG_NONE
	SERVER_WAIT
	WAIT			MSG_WAIT
	//ため系フラグを落とす
	KEEP_OFF		SIDE_ATTACK
	//いかり状態でいかりを出していないときはいかりフラグを落とす
	IF				IF_FLAG_EQ,BUF_PARA_WAZA_NO_NOW,WAZANO_IKARI,SUB_281_END
	IF_PSP			IF_FLAG_NBIT,SIDE_ATTACK,ID_PSP_condition2,CONDITION2_IKARI,SUB_281_END
	PSP_VALUE		VAL_NBIT,SIDE_ATTACK,ID_PSP_condition2,CONDITION2_IKARI
SUB_281_END:
	SEQ_END


//============================================================================
/**
 *
 *@file		sub_129.s
 *@brief	戦闘シーケンス
 *			おきみやげ追加効果シーケンス
 *@author	HisashiSogabe
 *@data		2006.02.06
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

SUB_129:
	ATTACK_MESSAGE
	SERVER_WAIT

	//攻撃力がMINまで下がっている時は、うまくきまらん
	IF_PSP			IF_FLAG_NE,SIDE_DEFENCE,ID_PSP_abiritycnt_pow,0,SUB_129_NEXT
	//特攻がMINまで下がっている時は、うまくきまらん
	IF_PSP			IF_FLAG_EQ,SIDE_DEFENCE,ID_PSP_abiritycnt_spepow,0,Umakukimaran

SUB_129_NEXT:
	WAZA_EFFECT		SIDE_ATTACK
	SERVER_WAIT

	//みがわりを出されている時は、うまくきまらん
	MIGAWARI_CHECK	SIDE_DEFENCE,MigawariUmakukimaran

	//これ以降は、ATTACK_MESSAGEとWAZA_EFFECTは出ないようにする
	VALUE			VAL_BIT,BUF_PARA_SERVER_STATUS_FLAG,SERVER_STATUS_FLAG_NO_ATTACK_MSG|SERVER_STATUS_FLAG_NO_WAZA_EFFECT

	//DefenceClientを追加対象に
	VALUE_WORK		VAL_SET,BUF_PARA_TSUIKA_CLIENT,BUF_PARA_DEFENCE_CLIENT
	VALUE			VAL_BIT,BUF_PARA_SERVER_STATUS_FLAG2,SERVER_STATUS_FLAG2_ABICNT_EFF_CHECK
	//攻撃力がMINまで下がっている時は、特攻へ
	IF_PSP			IF_FLAG_EQ,SIDE_DEFENCE,ID_PSP_abiritycnt_pow,0,SUB_129_SPEPOW
	//攻撃を2段階下げるをセット
	VALUE			VAL_SET,BUF_PARA_TSUIKA_PARA,ADD_COND2_POWDOWN2
	GOSUB			SUB_SEQ_ABICNT_CALC
SUB_129_SPEPOW:
	//アサートメッセージ非表示フラグをON
	VALUE			VAL_BIT,BUF_PARA_SERVER_STATUS_FLAG,SERVER_STATUS_FLAG_NO_DIRECT_MSG
	//特攻を2段階下げるをセット
	VALUE			VAL_SET,BUF_PARA_TSUIKA_PARA,ADD_COND2_SPEPOWDOWN2
	GOSUB			SUB_SEQ_ABICNT_CALC
SUB_129_END:
	VALUE			VAL_NBIT,BUF_PARA_SERVER_STATUS_FLAG2,SERVER_STATUS_FLAG2_ABICNT_EFF_FLAG
	VALUE			VAL_NBIT,BUF_PARA_SERVER_STATUS_FLAG2,SERVER_STATUS_FLAG2_ABICNT_EFF_CHECK
	SEQ_END

MigawariUmakukimaran:
	ATTACK_MESSAGE
	SERVER_WAIT
	WAIT			MSG_WAIT
	MESSAGE			ShikashiKoukanashiMsg,TAG_NONE
	SERVER_WAIT
	WAIT			MSG_WAIT
	//VALUE			VAL_BIT,BUF_PARA_WAZA_STATUS_FLAG,WAZA_STATUS_FLAG_SIPPAI_RENZOKU_CHECK
	VALUE			VAL_BIT,BUF_PARA_WAZA_STATUS_FLAG,WAZA_STATUS_FLAG_SIPPAI
	SEQ_END

Umakukimaran:
	VALUE			VAL_BIT,BUF_PARA_WAZA_STATUS_FLAG,WAZA_STATUS_FLAG_UMAKUKIMARAN
	SEQ_END


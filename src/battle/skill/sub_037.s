
//============================================================================
/**
 *
 *@file		sub_037.s
 *@brief	戦闘シーケンス
 *			こんらんの追加効果シーケンス
 *@author	HisashiSogabe
 *@data		2005.12.07
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

SUB_037:
#if AFTER_MASTER_070409_31_EUR_FIX
	//装備アイテムでの追加の時は専用のチェックをする
	IF				IF_FLAG_NE,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_SOUBIITEM,NormalCheck
SoubiItemCheck:
	//特性マイペースを持っているときは、失敗する
	TOKUSEI_CHECK	TOKUSEI_HAVE,SIDE_TSUIKA,TOKUSYU_MAIPEESU,SEQ_037_END
//すでにこんらんしている場合は、失敗する
	IF_PSP			IF_FLAG_BIT,SIDE_TSUIKA,ID_PSP_condition2,CONDITION2_KONRAN,SEQ_037_END
//しんぴのまもりで守られている場合は、失敗する
	IF				IF_FLAG_BIT,BUF_PARA_SIDE_CONDITION_TSUIKA,SIDE_CONDITION_SHINPI,SEQ_037_END
	BRANCH			NoWazaEffect
NormalCheck:
#endif //AFTER_MASTER_070409_31_EUR_FIX
	//特性マイペースを持っているときは、失敗する（かたやぶりチェックのために最上位）
	KATAYABURI_TOKUSEI_CHECK	TOKUSEI_HAVE,SIDE_TSUIKA,TOKUSYU_MAIPEESU,TokuseiNoKonran
	//技効果追加の場合、特性のチェックだけをする
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_WAZA_KOUKA,NoWazaEffect

	//間接追加の場合、りんぷんチェックをする
	IF				IF_FLAG_NE,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_INDIRECT,NoRinpun
	//特性りんぷんを持っているときは、失敗する（かたやぶりチェックのために最上位）
	KATAYABURI_TOKUSEI_CHECK	TOKUSEI_HAVE,SIDE_TSUIKA,TOKUSYU_RINPUN,Umakukimaran
NoRinpun:
//直接追加の場合、WAZAOUTシーケンスでメッセージを出さないので、ここで出す
	IF				IF_FLAG_NE,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_DIRECT,NoAttackMsg
	ATTACK_MESSAGE
	SERVER_WAIT
NoAttackMsg:

//みがわりを出されているときは、失敗する
	MIGAWARI_CHECK	SIDE_TSUIKA,Umakukimaran

//すでにこんらんしている場合は、失敗する
	IF_PSP			IF_FLAG_BIT,SIDE_TSUIKA,ID_PSP_condition2,CONDITION2_KONRAN,AlreadyKonran

//わざがはずれているときは、うまくきまらんにする
	IF				IF_FLAG_BIT,BUF_PARA_WAZA_STATUS_FLAG,WAZA_STATUS_FLAG_NOHIT_CHG,Umakukimaran

//しんぴのまもりで守られている場合は、失敗する
	IF				IF_FLAG_BIT,BUF_PARA_SIDE_CONDITION_TSUIKA,SIDE_CONDITION_SHINPI,ShinpiNoKonran

//直接追加の場合、WAZAOUTシーケンスで技エフェクトを出さないので、ここで出す
	IF				IF_FLAG_NE,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_DIRECT,NoWazaEffect
	WAZA_EFFECT		SIDE_ATTACK
	SERVER_WAIT
NoWazaEffect:
	STATUS_EFFECT	SIDE_TSUIKA,STATUS_KONRAN
	SERVER_WAIT
	//こんらんフラグを立てる（２～５ターン）
#if B1375_060817_FIX
	RANDOM_GET		3,2
#else //B1375_060817_FIX
	RANDOM_GET		3,3
#endif //B1375_060817_FIX
	PSP_VALUE_WORK	VAL_BIT,SIDE_TSUIKA,ID_PSP_condition2,BUF_PARA_CALC_WORK
//技効果追加の場合、メッセージは表示しない
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_WAZA_KOUKA,SUB_037_RET
	MESSAGE			KonranMineMsg,TAG_NICK,SIDE_TSUIKA
	SERVER_WAIT
	WAIT			MSG_WAIT
#if AFTER_MASTER_070409_31_EUR_FIX
SEQ_037_END:
#endif //AFTER_MASTER_070409_31_EUR_FIX
	SEQ_END

//うまくきまらないとき
Umakukimaran:
//間接追加の場合、メッセージを出さない
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_INDIRECT,SUB_037_RET
	WAIT			MSG_WAIT
	WAZA_PARAM_GET	ID_WTD_attackrange
	//複数選択技は、あたらなかったメッセージにする
	IF				IF_FLAG_EQ,BUF_PARA_CALC_WORK,RANGE_DOUBLE,Ataranakatta
	IF				IF_FLAG_EQ,BUF_PARA_CALC_WORK,RANGE_TRIPLE,Ataranakatta
	GOSUB			SUB_SEQ_UMAKUKIMARAN
	BRANCH			SUB_037_RET
Ataranakatta:
	GOSUB			SUB_SEQ_ATARANAKATTA
	BRANCH			SUB_037_RET

//すでにこんらんしているとき
AlreadyKonran:
//間接追加の場合、メッセージを出さない
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_INDIRECT,SUB_037_RET
	WAIT			MSG_WAIT
	MESSAGE			AlreadyKonranMineMsg,TAG_NICK,SIDE_TSUIKA
	BRANCH			SUB_037_END

//特性でこんらんを防ぐ
TokuseiNoKonran:
//間接追加の場合、メッセージを出さない
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_INDIRECT,SUB_037_RET
//装備アイテム追加の場合、メッセージを出さない
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_SOUBIITEM,SUB_037_RET
//技効果追加の場合、メッセージを出さない
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_WAZA_KOUKA,SUB_037_RET
	ATTACK_MESSAGE
	SERVER_WAIT
	WAIT			MSG_WAIT
NoAtkMsg:
	MESSAGE			MypaceMineMsg,TAG_NICK_TOKU,SIDE_TSUIKA,SIDE_TSUIKA
	BRANCH			SUB_037_END


//しんぴのまもりで防がれたとき
ShinpiNoKonran:
//間接追加の場合、メッセージを出さない
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_INDIRECT,SUB_037_RET
	WAIT			MSG_WAIT
	MESSAGE			ShinpiGuardMineMsg,TAG_NICK,SIDE_TSUIKA
SUB_037_END:
	SERVER_WAIT
	WAIT			MSG_WAIT
	//WazaStatusMessageを無効にするためにこのフラグを立てる
	//VALUE			VAL_BIT,BUF_PARA_WAZA_STATUS_FLAG,WAZA_STATUS_FLAG_SIPPAI_RENZOKU_CHECK
	VALUE			VAL_BIT,BUF_PARA_WAZA_STATUS_FLAG,WAZA_STATUS_FLAG_SIPPAI
SUB_037_RET:
	SEQ_END


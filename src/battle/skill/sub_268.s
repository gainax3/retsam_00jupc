
//============================================================================
/**
 *
 *@file		sub_268.s
 *@brief	戦闘シーケンス
 *			状態異常回復アイテムを使用したシーケンス（対戦トレーナー）
 *@author	HisashiSogabe
 *@data		2006.06.02
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

SUB_268:
	WAIT			MSG_WAIT/2
	SE_PLAY			SIDE_ATTACK,BSE_KAIHUKU
	MESSAGE			TrainerItemUseMsg,TAG_TRTYPE_TRNAME_ITEM,SIDE_ATTACK,SIDE_ATTACK,SIDE_WORK
	SERVER_WAIT
	WAIT			MSG_WAIT
	STATUS_RECOVER	SIDE_ATTACK
	IF				IF_FLAG_EQ,BUF_PARA_MSG_WORK,ITEM_USE_MSG_RECV_KONRAN,	ItemUseMsgRecvKonran
	IF				IF_FLAG_EQ,BUF_PARA_MSG_WORK,ITEM_USE_MSG_RECV_MAHI,	ItemUseMsgRecvMahi
	IF				IF_FLAG_EQ,BUF_PARA_MSG_WORK,ITEM_USE_MSG_RECV_KOORI,	ItemUseMsgRecvKoori
	IF				IF_FLAG_EQ,BUF_PARA_MSG_WORK,ITEM_USE_MSG_RECV_YAKEDO,	ItemUseMsgRecvYakedo
	IF				IF_FLAG_EQ,BUF_PARA_MSG_WORK,ITEM_USE_MSG_RECV_DOKU,	ItemUseMsgRecvDoku
	IF				IF_FLAG_EQ,BUF_PARA_MSG_WORK,ITEM_USE_MSG_RECV_NEMURI,	ItemUseMsgRecvNemuri
ItemUseMsgRecvMulti:
	MESSAGE			ItemNoHukusuuMineMsg,TAG_NICK_ITEM,SIDE_ATTACK,SIDE_WORK
	BRANCH			SUB_268_NEXT
ItemUseMsgRecvKonran:
	PSP_VALUE		VAL_NBIT,SIDE_ATTACK,ID_PSP_condition2,CONDITION2_KONRAN
	MESSAGE			ItemNoKonranMineMsg,TAG_NICK_ITEM,SIDE_ATTACK,SIDE_WORK
	BRANCH			SUB_268_NEXT2
ItemUseMsgRecvMahi:
	MESSAGE			ItemNoMahiMineMsg,TAG_NICK_ITEM,SIDE_ATTACK,SIDE_WORK
	BRANCH			SUB_268_NEXT
ItemUseMsgRecvKoori:
	MESSAGE			ItemNoKooriMineMsg,TAG_NICK_ITEM,SIDE_ATTACK,SIDE_WORK
	BRANCH			SUB_268_NEXT
ItemUseMsgRecvYakedo:
	MESSAGE			ItemNoYakedoMineMsg,TAG_NICK_ITEM,SIDE_ATTACK,SIDE_WORK
	BRANCH			SUB_268_NEXT
ItemUseMsgRecvDoku:
	MESSAGE			ItemNoDokuMineMsg,TAG_NICK_ITEM,SIDE_ATTACK,SIDE_WORK
	BRANCH			SUB_268_NEXT
ItemUseMsgRecvNemuri:
	MESSAGE			ItemNoNemuriMineMsg,TAG_NICK_ITEM,SIDE_ATTACK,SIDE_WORK
	BRANCH			SUB_268_NEXT
SUB_268_NEXT:
	PSP_VALUE		VAL_SET,SIDE_ATTACK,ID_PSP_condition,0
SUB_268_NEXT2:
	SERVER_WAIT
	STATUS_SET		SIDE_ATTACK,STATUS_NORMAL
	WAIT			MSG_WAIT
	SEQ_END

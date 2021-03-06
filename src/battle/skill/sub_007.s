
//============================================================================
/**
 *
 *@file		sub_007.s
 *@brief	戦闘シーケンス
 *			技ノーヒットシーケンス
 *@author	HisashiSogabe
 *@data		2005.07.29
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

SUB_007:
	//WAZA_OUTシーケンスに行く前にここにくるので、アタックメッセージを表示する
	ATTACK_MESSAGE
	SERVER_WAIT
	WAIT			MSG_WAIT
WazaKoyuuNoHit:
	IF				IF_FLAG_NBIT,BUF_PARA_WAZA_STATUS_FLAG,WAZA_STATUS_FLAG_WAZA_KOYUU_NOHIT,KieNoHit
	MESSAGE_WORK
	BRANCH			MoreNoHitMsg
KieNoHit:
	IF				IF_FLAG_NBIT,BUF_PARA_WAZA_STATUS_FLAG,WAZA_STATUS_FLAG_KIE_NOHIT,MamoruNoHit
	BRANCH			NoHit
MamoruNoHit:
	IF				IF_FLAG_NBIT,BUF_PARA_WAZA_STATUS_FLAG,WAZA_STATUS_FLAG_MAMORU_NOHIT,JimenNoHit
#if AFTER_MASTER_070405_BT_FIX
	IF				IF_FLAG_EQ,BUF_PARA_WAZA_NO_NOW,WAZANO_IBIKI,UmakukimaranCheck
	IF				IF_FLAG_EQ,BUF_PARA_WAZA_NO_NOW,WAZANO_AKUMU,UmakukimaranCheck
	IF				IF_FLAG_EQ,BUF_PARA_WAZA_NO_NOW,WAZANO_NAGETUKERU,UmakukimaranCheck
	IF				IF_FLAG_EQ,BUF_PARA_WAZA_NO_NOW,WAZANO_TOTTEOKI,UmakukimaranCheck
	IF				IF_FLAG_EQ,BUF_PARA_WAZA_NO_NOW,WAZANO_SAIKOSIHUTO,UmakukimaranCheck
	IF				IF_FLAG_EQ,BUF_PARA_WAZA_NO_NOW,WAZANO_NEKODAMASI,UmakukimaranCheck
	IF				IF_FLAG_NE,BUF_PARA_WAZA_NO_NOW,WAZANO_SIZENNOMEGUMI,MamoruNoHitMsg
UmakukimaranCheck:
	IF				IF_FLAG_BIT,BUF_PARA_WAZA_STATUS_FLAG,WAZA_STATUS_FLAG_UMAKUKIMARAN,Umakukimaran
MamoruNoHitMsg:
#endif //AFTER_MASTER_070405_BT_FIX
	MESSAGE			MamoruNoHitMineMsg,TAG_NICK,SIDE_DEFENCE
	BRANCH			MoreNoHitMsg
JimenNoHit:
	IF				IF_FLAG_NBIT,BUF_PARA_WAZA_STATUS_FLAG,WAZA_STATUS_FLAG_JIMEN_NOHIT,DenzihuyuuNoHit
	MESSAGE			JimenNoHitMineMsg,TAG_NICK_TOKU,SIDE_DEFENCE,SIDE_DEFENCE
	BRANCH			MoreNoHitMsg
DenzihuyuuNoHit:
	IF				IF_FLAG_NBIT,BUF_PARA_WAZA_STATUS_FLAG,WAZA_STATUS_FLAG_DENZIHUYUU_NOHIT,Umakukimaran
	VALUE			VAL_SET,BUF_PARA_WAZA_WORK,WAZANO_DENZIHUYUU
	MESSAGE			DenzihuyuuNoHitMineMsg,TAG_NICK_WAZA,SIDE_DEFENCE,SIDE_WORK
	BRANCH			MoreNoHitMsg
Umakukimaran:
	IF				IF_FLAG_NBIT,BUF_PARA_WAZA_STATUS_FLAG,WAZA_STATUS_FLAG_UMAKUKIMARAN,BatsugunNoHit
	MESSAGE			UmakukimaranMsg,TAG_NONE
	BRANCH			MoreNoHitMsg
BatsugunNoHit:
	IF				IF_FLAG_NBIT,BUF_PARA_WAZA_STATUS_FLAG,WAZA_STATUS_FLAG_BATSUGUN_NOHIT,Koukanai
	MESSAGE			TokuseiNoHitMineMsg,TAG_NICK_TOKU,SIDE_DEFENCE,SIDE_DEFENCE
	BRANCH			MoreNoHitMsg
Koukanai:
	IF				IF_FLAG_NBIT,BUF_PARA_WAZA_STATUS_FLAG,WAZA_STATUS_FLAG_KOUKANAI,GanzyouNoHit
	INC_RECORD		SIDE_ATTACK,CLIENT_BOOT_TYPE_MINE,RECID_WAZA_MUKOU
	MESSAGE			KoukanaiMineMsg,TAG_NICK,SIDE_DEFENCE
	BRANCH			MoreNoHitMsg
GanzyouNoHit:
	IF				IF_FLAG_NBIT,BUF_PARA_WAZA_STATUS_FLAG,WAZA_STATUS_FLAG_GANZYOU_NOHIT,IchigekiNoHit
	MESSAGE			GanzyouMineMsg,TAG_NICK_TOKU,SIDE_DEFENCE,SIDE_DEFENCE
	BRANCH			MoreNoHitMsg
IchigekiNoHit:
	IF				IF_FLAG_NBIT,BUF_PARA_WAZA_STATUS_FLAG,WAZA_STATUS_FLAG_ICHIGEKI_NOHIT,NoHit
	MESSAGE			ZenzenMineMsg,TAG_NICK,SIDE_DEFENCE
	BRANCH			MoreNoHitMsg
NoHit:
	WAZA_PARAM_GET	ID_WTD_attackrange
	//複数選択技は、あたらなかったメッセージにする
	IF				IF_FLAG_EQ,BUF_PARA_CALC_WORK,RANGE_DOUBLE,Ataranakatta
	IF				IF_FLAG_EQ,BUF_PARA_CALC_WORK,RANGE_TRIPLE,Ataranakatta
	MESSAGE			NoHitMineMsg,TAG_NICK,SIDE_ATTACK
	BRANCH			MoreNoHitMsg
Ataranakatta:
	MESSAGE			DoubleNoHitMineMsg,TAG_NICK,SIDE_DEFENCE
MoreNoHitMsg:
	SERVER_WAIT
	WAIT			MSG_WAIT
	IF				IF_FLAG_NBIT,BUF_PARA_SERVER_STATUS_FLAG,SERVER_STATUS_FLAG_MORE_NOHIT_MSG,SUB_007_NEXT
	GOSUB			SUB_SEQ_MESSAGE_WORK
SUB_007_NEXT:
	//ため系フラグを落とす
	KEEP_OFF		SIDE_ATTACK
	//技が外れた時にペナルティを課すルーチンへジャンプ
	GOSUB			SUB_SEQ_NOHIT_PENALTY
	SEQ_END


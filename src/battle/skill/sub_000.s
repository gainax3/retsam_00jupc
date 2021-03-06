//============================================================================
/**
 *
 *@file		sub_000.s
 *@brief	戦闘シーケンス
 *			ポケモンエンカウント
 *@author	HisashiSogabe
 *@data		2005.07.20
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

SUB_000:
	//トレーナー戦ならトレーナーエンカウントへ
	IF						IF_FLAG_BIT,BUF_PARA_FIGHT_TYPE,FIGHT_TYPE_TRAINER,TrainerEncount
	//サファリゾーンならサファリエンカウントへ
	IF						IF_FLAG_BIT,BUF_PARA_FIGHT_TYPE,FIGHT_TYPE_SAFARI,SafariEncount
	//ポケパークならポケパークエンカウントへ
	IF						IF_FLAG_BIT,BUF_PARA_FIGHT_TYPE,FIGHT_TYPE_POKE_PARK,PokeparkEncount
	POKEMON_ENCOUNT			SIDE_ENEMY
	TRAINER_ENCOUNT			SIDE_MINE
	ENCOUNT_EFFECT
	WAIT_NO_SKIP			ENCOUNT_WAIT+ENCOUNT_WAIT
	HP_GAUGE_IN_WAIT_SET	SIDE_ENEMY
	SERVER_WAIT
//	BG_CHG
	IF						IF_FLAG_BIT,BUF_PARA_BATTLE_STATUS_FLAG,BATTLE_STATUS_FLAG_YABURETA_SEKAI,YaburetaBattle
	IF						IF_FLAG_BIT,BUF_PARA_BATTLE_STATUS_FLAG,BATTLE_STATUS_FLAG_DENSETSU_BATTLE,DensetsuBattle
	IF						IF_FLAG_BIT,BUF_PARA_BATTLE_STATUS_FLAG,BATTLE_STATUS_FLAG_FIRST_BATTLE,FirstBattle
	IF						IF_FLAG_BIT,BUF_PARA_BATTLE_STATUS_FLAG,BATTLE_STATUS_FLAG_MITSUNURI_BATTLE,MitsuBattle
	IF						IF_FLAG_BIT,BUF_PARA_FIGHT_TYPE,FIGHT_TYPE_2vs2,Yasei2vs2Battle
NormalBattle:
	MESSAGE_NO_DIR			EncountMsg,TAG_NICK,SIDE_ENEMY
	BRANCH					Next
FirstBattle:
	MESSAGE_NO_DIR			FirstEncountMsg,TAG_NICK,SIDE_ENEMY
	BRANCH					Next
MitsuBattle:
	MESSAGE_NO_DIR			EncountMituMsg,TAG_NICK,SIDE_ENEMY
	BRANCH					Next
DensetsuBattle:
	MESSAGE_NO_DIR			SpecialPokeEncountMsg,TAG_NICK,SIDE_ENEMY
	BRANCH					Next
YaburetaBattle:
	MESSAGE_NO_DIR			GiratinaEncountMsg,TAG_NICK,SIDE_ENEMY
	BRANCH					Next
Yasei2vs2Battle:
	MESSAGE_NO_DIR			Encount2vs2Msg,TAG_NICK_NICK,SIDE_ENEMY_1,SIDE_ENEMY_2
Next:
	SERVER_WAIT
	WAIT					MSG_WAIT
	IF						IF_FLAG_BIT,BUF_PARA_FIGHT_TYPE,FIGHT_TYPE_MULTI,MultiMsg
	MESSAGE					AppearMsg_0,TAG_NICK,SIDE_MINE
	BRANCH					MsgNext
MultiMsg:
	ENCOUNT_KURIDASU_MESSAGE	SIDE_MINE
MsgNext:
	SS_TO_OAM_CALL			SIDE_ENEMY
	SERVER_WAIT
	TRAINER_THROW			SIDE_MINE,TT_TYPE_ENCOUNT
	POKEMON_ENCOUNT_APPEAR	SIDE_MINE
	WAIT_NO_SKIP			ENCOUNT_WAIT_MINE
	HP_GAUGE_IN_WAIT_SET	SIDE_MINE
	SERVER_WAIT
	OAM_TO_SS_CALL			SIDE_ENEMY
	SERVER_WAIT
	BRANCH					SUB_000_END

//トレーナーエンカウント
TrainerEncount:
	IF							IF_FLAG_BIT,BUF_PARA_BATTLE_STATUS_FLAG,BATTLE_STATUS_FLAG_REC_CHILD,TrainerEncountReverse
TrainerEncountNormal:
	ENCOUNT_EFFECT
	TRAINER_ENCOUNT				SIDE_ALL
	WAIT_NO_SKIP				BALL_GAUGE_ENCOUNT_WAIT
	BALL_GAUGE_RESOURCE_LOAD
	BALL_GAUGE_ENCOUNT_SET		SIDE_ENEMY
	BALL_GAUGE_ENCOUNT_SET		SIDE_MINE
	ENCOUNT_MESSAGE				SIDE_ENEMY
	SERVER_WAIT
//	BG_CHG
	WAIT						MSG_WAIT
	ENCOUNT_KURIDASU_MESSAGE	SIDE_ENEMY
TrainerOutEnemy1:
	BALL_GAUGE_ENCOUNT_OUT		SIDE_ENEMY
	TRAINER_THROW				SIDE_ENEMY,TT_TYPE_ENCOUNT
	POKEMON_ENCOUNT_APPEAR		SIDE_ENEMY
	WAIT_NO_SKIP				ENCOUNT_WAIT_ENEMY
	HP_GAUGE_IN_WAIT_SET		SIDE_ENEMY
	SERVER_WAIT
	ENCOUNT_KURIDASU_MESSAGE	SIDE_MINE
TrainerOutMine1:
	BALL_GAUGE_ENCOUNT_OUT		SIDE_MINE
	TRAINER_THROW				SIDE_MINE,TT_TYPE_ENCOUNT
	POKEMON_ENCOUNT_APPEAR		SIDE_MINE
	WAIT_NO_SKIP				ENCOUNT_WAIT_MINE
	HP_GAUGE_IN_WAIT_SET		SIDE_MINE
	SERVER_WAIT
	BALL_GAUGE_RESOURCE_DELETE
	BRANCH					SUB_000_END

//録画再生時に子機側だった方のエンカウントエフェクト
TrainerEncountReverse:
	ENCOUNT_EFFECT
	TRAINER_ENCOUNT				SIDE_ALL
	WAIT_NO_SKIP				BALL_GAUGE_ENCOUNT_WAIT
	BALL_GAUGE_RESOURCE_LOAD
	BALL_GAUGE_ENCOUNT_SET		SIDE_ENEMY
	BALL_GAUGE_ENCOUNT_SET		SIDE_MINE
	ENCOUNT_MESSAGE				SIDE_ENEMY
	SERVER_WAIT
//	BG_CHG
	WAIT						MSG_WAIT
	ENCOUNT_KURIDASU_MESSAGE	SIDE_MINE
TrainerOutEnemy2:
	BALL_GAUGE_ENCOUNT_OUT		SIDE_MINE
	TRAINER_THROW				SIDE_MINE,TT_TYPE_ENCOUNT
	POKEMON_ENCOUNT_APPEAR		SIDE_MINE
	WAIT_NO_SKIP				ENCOUNT_WAIT_MINE
	HP_GAUGE_IN_WAIT_SET		SIDE_MINE
	SERVER_WAIT
	ENCOUNT_KURIDASU_MESSAGE	SIDE_ENEMY
TrainerOutMine2:
	BALL_GAUGE_ENCOUNT_OUT		SIDE_ENEMY
	TRAINER_THROW				SIDE_ENEMY,TT_TYPE_ENCOUNT
	POKEMON_ENCOUNT_APPEAR		SIDE_ENEMY
	WAIT_NO_SKIP				ENCOUNT_WAIT_ENEMY
	HP_GAUGE_IN_WAIT_SET		SIDE_ENEMY
	SERVER_WAIT
	BALL_GAUGE_RESOURCE_DELETE
	BRANCH					SUB_000_END

//サファリエンカウント
SafariEncount:
	POKEMON_ENCOUNT			SIDE_ENEMY
	TRAINER_ENCOUNT			SIDE_MINE
	ENCOUNT_EFFECT
	WAIT_NO_SKIP			ENCOUNT_WAIT+ENCOUNT_WAIT
	HP_GAUGE_IN_WAIT_SET	SIDE_ENEMY
	SERVER_WAIT
//	BG_CHG
	MESSAGE_NO_DIR			EncountMsg,TAG_NICK,SIDE_ENEMY
	SERVER_WAIT
	HP_GAUGE_IN				SIDE_MINE
	WAIT					MSG_WAIT/4
	SERVER_WAIT
	BRANCH					SUB_000_END

//ポケパークエンカウント
PokeparkEncount:
	POKEMON_ENCOUNT			SIDE_ENEMY
	TRAINER_ENCOUNT			SIDE_MINE
	ENCOUNT_EFFECT
	WAIT_NO_SKIP			ENCOUNT_WAIT+ENCOUNT_WAIT
	HP_GAUGE_IN_WAIT_SET	SIDE_ENEMY
	SERVER_WAIT
//	BG_CHG
	MESSAGE_NO_DIR			ParkEncountMsg,TAG_TRNAME_NICK,SIDE_ENEMY,SIDE_ENEMY
	SERVER_WAIT
	HP_GAUGE_IN				SIDE_MINE
	WAIT					MSG_WAIT/4
	SERVER_WAIT

SUB_000_END:
	BG_CHG
	SEQ_END


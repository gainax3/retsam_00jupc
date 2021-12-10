//==============================================================================
/**
 * @file	frontier_scrcmd.c
 * @brief	ƒtƒƒ“ƒeƒBƒA—pƒXƒNƒŠƒvƒgFƒRƒ}ƒ“ƒhŠÖ”
 * @author	matsuda
 * @date	2007.03.28(…)
 */
//==============================================================================
#include "common.h"

#include "system/arc_tool.h"
#include "system/bmp_menu.h"
#include "system/brightness.h"
#include "system/fontproc.h"
#include "system/fontoam.h"
#include "system/font_arc.h"
#include "system/snd_def.h"
#include "system/snd_perap.h"
#include "system/snd_tool.h"
#include "system/window.h"
#include "system/msg_ds_icon.h"
#include "system/wipe.h"
#include "poketool/status_rcv.h"		//PokeParty_RecoverAll
#include "system/procsys.h"

//#include "battle/fight_def.h"		///<BattleWork‚Ö‚ÌˆË‘¶“x‚ª‚‚¢ƒ/[ƒX‚È‚Ì‚ÅA‹–‰Â
//#include "battle/server_def.h"		///<ServerParam‚Ö‚ÌˆË‘¶“x‚ª‚‚¢ƒ/[ƒX‚È‚Ì‚ÅA‹–‰Â
//#include "battle/client_def.h"		///<ClientParam‚Ö‚ÌˆË‘¶“x‚ª‚‚¢ƒ/[ƒX‚È‚Ì‚ÅA‹–‰Â

#include "field/eventflag.h"
#include "field/evwkdef.h"
#include "../field/scr_tool.h"

//MAKE‚ÌˆË‘¶ŠÖŒW‚ÉŠÜ‚ß‚é‚½‚ß‚ÉIncludei–{—ˆ‚Í•K—v‚È‚¢j
#include "battle/skill/waza_seq.naix"
#include "battle/skill/be_seq.naix"
#include "battle/skill/sub_seq.naix"

#include "frontier_types.h"
#include "frontier_main.h"
#include "frontier_scr.h"
#include "fss_task.h"
#include "fs_usescript.h"
#include "frontier_map.h"
#include "frontier_scrcmd.h"
#include "frontier_scrcmd_sub.h"
#include "frontier_tcb_pri.h"
#include "frontier_actor.h"
#include "frontier_objmove.h"
#include "fss_scene.h"
#include "frontier_def.h"
#include "frontier_tool.h"
//ƒTƒEƒ“ƒh
#include "fssc_sound.h"
//ƒtƒ@ƒNƒgƒŠ[
#include "fssc_factory.h"
//ƒoƒgƒ‹ƒ^ƒ[
#include "fssc_tower.h"
//ƒXƒe[ƒW
#include "fssc_stage.h"
//ƒLƒƒƒbƒXƒ‹
#include "fssc_castle.h"
//ƒ‹[ƒŒƒbƒg
#include "fssc_roulette.h"
//WiFió•t
#include "fssc_wifi_counter.h"
//˜^‰æ
#include "savedata/battle_rec.h"
#include "../field/field_subproc.h"
//ƒ[ƒhƒZƒbƒg
#include "msgdata/msg.naix"							//NARC_msg_??_dat
//’ÊM
#include "communication/comm_info.h"
#include "communication/comm_system.h"
#include "communication/comm_tool.h"
#include "communication/comm_def.h"
#include "communication/comm_state.h"
#include "communication/communication.h"
#include "communication/comm_mp.h"
#include "../field/comm_field_state.h"

#include "system/laster.h"
#include "f_encount.h"
#include "frontier_enc_cutin.h"

#include "savedata/wifilist.h"

#include "savedata/tv_work.h"
#include "../field/tv_topic.h"						//ƒeƒŒƒrƒgƒsƒbƒN¶¬—p
#include "field/tvtopic_extern.h"


//==============================================================================
//	’è”’è‹`
//==============================================================================
//ƒŒƒWƒXƒ^”äŠr‚ÌŒ‹‰Ê’è‹`
enum {
	MINUS_RESULT = 0,	//”äŠrŒ‹‰Ê‚ªƒ}ƒCƒiƒX
	EQUAL_RESULT,		//”äŠrŒ‹‰Ê‚ªƒCƒR[ƒ‹
	PLUS_RESULT			//”äŠrŒ‹‰Ê‚ªƒvƒ‰ƒX
};

///ƒAƒNƒ^[¶¬A’†SÀ•W‚É‚·‚éˆ×‚É‘«‚µ‚±‚ŞƒIƒtƒZƒbƒgX
#define FSS_ACTOR_SET_OFFSET_X		(-8)
///ƒAƒNƒ^[¶¬A‘«Œ³À•W‚É‚·‚éˆ×‚É‘«‚µ‚±‚ŞƒIƒtƒZƒbƒgY
#define FSS_ACTOR_SET_OFFSET_Y		(-16)

///ƒGƒ“ƒJƒEƒ“ƒgƒGƒtƒFƒNƒg‚Å©—R‚Ég—p‚µ‚Ä‚à‚æ‚¢ƒtƒŒ[ƒ€–Ê
#define ENCOUNT_EFF_FRAME		(FRMAP_FRAME_WIN)

//==============================================================================
//	ƒf[ƒ^
//==============================================================================
static const BMPWIN_DAT YesNoBmpDat = {
	FRMAP_FRAME_WIN, FFD_YESNO_WIN_PX, FFD_YESNO_WIN_PY,
	FFD_YESNO_WIN_SX, FFD_YESNO_WIN_SY, FFD_YESNO_WIN_PAL, FFD_YESNO_WIN_CGX
};

//ğŒ•ªŠò—pƒe[ƒuƒ‹
static const u8 ConditionTable[6][3] =
{
//	  MINUS  EQUAL  PLUS
	{ TRUE,	 FALSE, FALSE },	// LT
	{ FALSE, TRUE,  FALSE },	// EQ
	{ FALSE, FALSE, TRUE  },	// GT
	{ TRUE,  TRUE,  FALSE },	// LE
	{ FALSE, TRUE,  TRUE  },	// GE
	{ TRUE,  FALSE, TRUE  }		// NE
};

//============================================================================================
//	BMPWINƒf[ƒ^
//============================================================================================
//static const BMPWIN_DAT YesNoBmpWin = {
//	GF_BGL_FRAME2_M, 23, 13, 7, 4, MENU_MSG_PAL_NO, MENU_MSG_START
//};

//==============================================================================
//
//	ƒvƒƒgƒ^ƒCƒvéŒ¾
//
//==============================================================================
//static u16 GetEventWorkValue(FSS_TASK *core, u16 work_no );
u16 FSS_GetEventWorkValue(FSS_TASK *core, u16 work_no );
//static u16 * GetEventWorkAdrs(FSS_TASK *core, u16 wk_id);
u16 * FSS_GetEventWorkAdrs(FSS_TASK *core, u16 wk_id);
static BOOL FSSC_Dummy( FSS_TASK * core );
static BOOL FSSC_End( FSS_TASK * core );
static BOOL FSSC_ScriptFinish(FSS_TASK *core);
static BOOL FSSC_TimeWait( FSS_TASK * core );
static BOOL EvWaitTime(FSS_TASK * core);
static BOOL FSSC_TalkMsgAllPut( FSS_TASK * core );
static BOOL FSSC_TalkMsgNoSkip( FSS_TASK * core );
static BOOL FSSC_TalkMsg( FSS_TASK * core );
static BOOL TalkMsgWait(FSS_TASK * core);
static BOOL FSSC_TalkWinClose( FSS_TASK * core );
static BOOL FSSC_WipeFadeStart( FSS_TASK * core );
static BOOL FSSC_WipeFadeCheck( FSS_TASK * core );
static BOOL WaitWipeFadeCheck(FSS_TASK * core);
static BOOL FSSC_BmpMenuInit( FSS_TASK * core );
static BOOL FSSC_BmpMenuInitEx( FSS_TASK * core );
static BOOL FSSC_BmpMenuMakeList( FSS_TASK * core );
static BOOL FSSC_BmpMenuMakeListTalkMsg( FSS_TASK * core );
static BOOL FSSC_BmpMenuStart( FSS_TASK * core );
static BOOL FSSC_BmpListInit( FSS_TASK * core );
static BOOL FSSC_BmpListInitEx( FSS_TASK * core );
static BOOL FSSC_BmpListMakeList( FSS_TASK * core );
static BOOL FSSC_BmpListStart( FSS_TASK * core );
static BOOL EvSelListWinWait(FSS_TASK * core);
static BOOL FSSC_BmpListDel( FSS_TASK * core );
static BOOL EvSelMenuWinWait(FSS_TASK * core);
static BOOL FSSC_CmpMain( u16 r1, u16 r2 );
static BOOL FSSC_CmpWkValue( FSS_TASK * core );
static BOOL FSSC_CmpWkWk( FSS_TASK * core );
static BOOL FSSC_LoadWkValue( FSS_TASK * core );
static BOOL FSSC_LoadWkWk( FSS_TASK * core );
static BOOL FSSC_WkAdd( FSS_TASK * core );
static BOOL FSSC_WkSub( FSS_TASK * core );
void FSSC_Jump( FSS_TASK * core, FSS_CODE * adrs );
static BOOL FSSC_GlobalJump( FSS_TASK * core );
static BOOL FSSC_IfJump( FSS_TASK * core );
static BOOL FSSC_NameInProc(FSS_TASK *core);
static BOOL FSSC_YesNoWin( FSS_TASK * core );
static BOOL EvYesNoSelect(FSS_TASK * core);
static BOOL FSS_CharResourceSet(FSS_TASK *core);
static BOOL FSS_CharResourceFree(FSS_TASK *core);
static BOOL FSS_ActorSet(FSS_TASK *core);
static BOOL FSS_ActorFree(FSS_TASK *core);
static BOOL FSSC_ObjAnime( FSS_TASK * core );
static void FSSC_AnimeListSet(u16 playid, FSS_ACTOR_WORK *fss_actor, const FSSC_ANIME_DATA *anmlist, WF2DMAP_ACTCMDQ *actcmd_q, u8 *anm_count_ptr, int heap_id);
static BOOL FSSC_ObjAnimeWait( FSS_TASK * core );
static BOOL ObjAnmWait(FSS_TASK * core);
static BOOL FSSC_BattleRecInit( FSS_TASK * core );
static BOOL FSSC_BattleRecSave( FSS_TASK * core );
static BOOL BattleRecSaveWait(FSS_TASK * core);
static int Frontier_GetRecModeNo( u16 fr_no, u16 type );
static BOOL FSSC_BattleRecLoad( FSS_TASK * core );
static BOOL FSSC_BattleRecExit( FSS_TASK * core );
static BOOL FSSC_BattleRecDataOccCheck( FSS_TASK * core );
static BOOL FSSC_ReportSave( FSS_TASK * core );
static BOOL EvWaitDivSave(FSS_TASK * core);
static BOOL FSSC_ReportDivSave( FSS_TASK * core );
static BOOL FSSC_ExtraSaveInit( FSS_TASK * core );
static BOOL FSSC_ExtraSaveInitCheck( FSS_TASK * core );
static BOOL FSSC_TimeWaitIconAdd( FSS_TASK * core);
static BOOL FSSC_TimeWaitIconDel( FSS_TASK * core);
static BOOL FSSC_ItemName( FSS_TASK * core );
static BOOL FSSC_NumberName( FSS_TASK * core );
static BOOL FSSC_PlayerName( FSS_TASK * core );
static BOOL FSSC_PairName( FSS_TASK * core );
static BOOL FSSC_WazaName( FSS_TASK * core );
static BOOL FSSC_PokemonNameExtra( FSS_TASK * core );
static BOOL FSSC_TypeName( FSS_TASK * core );
static BOOL FSSC_RivalName( FSS_TASK * core );
static STRBUF * PokeNameGetAlloc( u16 id, u32 heap );
static BOOL FSSC_CheckBtlPoint( FSS_TASK * core );
static BOOL FSSC_AddBtlPoint( FSS_TASK * core );
static BOOL FSSC_SubBtlPoint( FSS_TASK * core );
static u16 GetNumKeta(u32 num);
u16 * FSSTGetWork(FSS_TASK *core);
u16 FSSTGetWorkValue(FSS_TASK * core);
static BOOL FSSC_GlobalCall( FSS_TASK * core );
static BOOL FSSC_Ret( FSS_TASK * core );
static BOOL FSSC_IfCall( FSS_TASK * core );
static BOOL FSSC_MapChange(FSS_TASK *core);
static BOOL FSSC_MapChangeEX(FSS_TASK *core);
static BOOL MapChangeWait(FSS_TASK * core);
static BOOL FSS_ActorVisibleSet( FSS_TASK * core );
static BOOL FSS_ActorBgPriSet( FSS_TASK * core );
static BOOL FSSC_CommTimingSyncStart( FSS_TASK * core );
static BOOL EvWaitCommIsTimingSync(FSS_TASK * core);
static BOOL FSSC_CommTempDataReset( FSS_TASK * core );
static BOOL FSSC_CommDirectEnd( FSS_TASK * core );
static BOOL EvWaitCommDirectEnd(FSS_TASK * core);
static BOOL FSSC_GetRand( FSS_TASK * core );
static BOOL FSSC_PcKaifuku( FSS_TASK * core );
static BOOL FSSC_ABKeyWait( FSS_TASK * core );
static BOOL EvWaitABKey(FSS_TASK * core);
static BOOL FSSC_ABKeyTimeWait( FSS_TASK * core );
static BOOL EvWaitABKeyTime(FSS_TASK * core);
BOOL FSSC_TrainerLoseCheck( FSS_TASK * core );
static BOOL FSSC_SaveEventWorkSet( FSS_TASK * core );
static BOOL FSSC_SaveEventWorkGet( FSS_TASK * core );
static BOOL FSS_ArticleResourceSet(FSS_TASK *core);
static BOOL FSS_ArticleResourceFree(FSS_TASK *core);
static BOOL FSS_ArticleActorSet(FSS_TASK *core);
static BOOL FSS_ArticleActorFree(FSS_TASK *core);
static BOOL FSS_ArticleActorVisibleSet( FSS_TASK * core );
static BOOL FSS_ArticleActorFlipHSet( FSS_TASK * core );
static BOOL FSS_ArticleActorObjModeSet( FSS_TASK * core );
static BOOL FSS_ArticleActorAnimeStart( FSS_TASK * core );
static BOOL FSS_ArticleActorAnimeStop( FSS_TASK * core );
static BOOL FSS_ArticleActorAnimeWait( FSS_TASK * core );
static BOOL WaitArticleActorAnime(FSS_TASK *core);
static BOOL FSSC_EncountEffect( FSS_TASK * core );
static BOOL FSSC_GetMineObj( FSS_TASK * core );
static BOOL EncountEffectWait(FSS_TASK * core);
static BOOL FSS_ParticleSPALoad(FSS_TASK *core);
static BOOL FSS_ParticleSPAExit(FSS_TASK *core);
static BOOL FSS_ParticleAddEmitter(FSS_TASK *core);
static BOOL FSS_ParticleWait(FSS_TASK *core);
static BOOL ParticleEmitWait(FSS_TASK * core);
static BOOL FSS_ShakeSet(FSS_TASK *core);
static BOOL FSS_ShakeWait(FSS_TASK *core);
static BOOL ShakeEndWait(FSS_TASK * core);
static BOOL FSS_WndSet(FSS_TASK *core);
static BOOL FSS_WndWait(FSS_TASK *core);
static BOOL WndEndWait(FSS_TASK * core);
static BOOL FSSC_DebugPrint( FSS_TASK * core );
static void FSSC_ObjMoveCode_Core(FSS_TASK *core, int code, int work_num);
static BOOL FSSC_ObjMoveCode_Reset(FSS_TASK *core);
static BOOL FSSC_ObjMoveCode_Kyoro(FSS_TASK *core);
u32 GetLasterBuffMatrixData( u16 x, u16 y );
static BOOL FSSC_TemotiPokeActAdd( FSS_TASK * core );
static BOOL FSSC_TemotiPokeActDel( FSS_TASK * core );
static BOOL FSSC_BrainEncountEffect(FSS_TASK *core);
static BOOL BrainEncountEffectWait(FSS_TASK * core);
static BOOL FSSC_RecordInc(FSS_TASK *core);
static BOOL FSSC_RecordAdd(FSS_TASK *core);
static BOOL FSSC_ScoreAdd(FSS_TASK *core);
static BOOL FSSC_CheckDPRomCode(FSS_TASK *core);
static BOOL FSSC_BattleRecServerVersionCheck(FSS_TASK *core);
static BOOL FSSC_WifiCounterListSetLastPlayDate(FSS_TASK *core);
static BOOL FSSC_TVTempFriendSet(FSS_TASK *core);
static BOOL FSSC_CommSetWifiBothNet( FSS_TASK * core );
static BOOL ov104_2231F44( FSS_TASK * core );
static BOOL ov104_2231F5C( FSS_TASK * core );


//============================================================================================
/**
 *	–½—ßƒe[ƒuƒ‹
 *	frontier_seq_def.h‚ÌƒCƒ“ƒfƒbƒNƒX‚Æˆê‘Î‚É‚È‚é‚æ‚¤‚É‚·‚é
 */
//============================================================================================

const FRSCR_CMD FSSCmdTable[]={
	FSSC_Dummy,
	FSSC_End,
	FSSC_ScriptFinish,
	FSSC_MapChange,
	FSSC_MapChangeEX,
	FSSC_TimeWait,
	FSSC_LoadWkValue,
	FSSC_LoadWkWk,
	FSSC_WkAdd,
	FSSC_WkSub,
	FSSC_GlobalJump,
	FSSC_IfJump,
	FSSC_GlobalCall,
	FSSC_Ret,
	FSSC_IfCall,
	FSSC_TalkMsgAllPut,
	FSSC_TalkMsgNoSkip,
	FSSC_TalkMsg,
	FSSC_TalkWinClose,
	FSSC_WipeFadeStart,
	FSSC_WipeFadeCheck,
	FSSC_BmpMenuInit,
	FSSC_BmpMenuInitEx,
	FSSC_BmpMenuMakeList,
	FSSC_BmpMenuMakeListTalkMsg,
	FSSC_BmpMenuStart,
	FSSC_BmpListInit,
	FSSC_BmpListInitEx,
	FSSC_BmpListMakeList,
	FSSC_BmpListStart,
	FSSC_BmpListDel,
	FSSC_YesNoWin,
	FSSC_CmpWkValue,
	FSSC_CmpWkWk,
	FSS_CharResourceSet,
	FSS_CharResourceFree,
	FSS_ActorSet,
	FSS_ActorFree,
	FSS_ActorVisibleSet,
	FSS_ActorBgPriSet,
	FSSC_ObjAnime,
	FSSC_ObjAnimeWait,
	FSS_ArticleResourceSet,
	FSS_ArticleResourceFree,
	FSS_ArticleActorSet,
	FSS_ArticleActorFree,
	FSS_ArticleActorVisibleSet,
	FSS_ArticleActorFlipHSet,
	FSS_ArticleActorObjModeSet,
	FSS_ArticleActorAnimeStart,
	FSS_ArticleActorAnimeStop,
	FSS_ArticleActorAnimeWait,
	FSSC_NameInProc,
	FSSC_CommTimingSyncStart,
	FSSC_CommTempDataReset,
	FSSC_CommDirectEnd,
	FSSC_GetRand,
	FSSC_PcKaifuku,
	FSSC_ABKeyWait,
	FSSC_ABKeyTimeWait,
	FSSC_TrainerLoseCheck,
	FSSC_SaveEventWorkSet,
	FSSC_SaveEventWorkGet,
	FSSC_EncountEffect,
	FSSC_GetMineObj,
	FSS_ParticleSPALoad,
	FSS_ParticleSPAExit,
	FSS_ParticleAddEmitter,
	FSS_ParticleWait,
	FSSC_TemotiPokeActAdd,
	FSSC_TemotiPokeActDel,
	FSSC_BrainEncountEffect,
	FSSC_RecordInc,
	FSSC_RecordAdd,
	FSSC_ScoreAdd,
	FSSC_CheckDPRomCode,
	FSS_ShakeSet,
	FSS_ShakeWait,
	FSS_WndSet,
	FSS_WndWait,
	
	//CPƒEƒBƒ“ƒhƒE
	FSSC_CPWinWrite,
	FSSC_CPWinDel,
	FSSC_CPWrite,
	//OBJ“®ìƒR[ƒh
	FSSC_ObjMoveCode_Reset,
	FSSC_ObjMoveCode_Kyoro,
	//ƒTƒEƒ“ƒh
	FSSC_SePlay,
	FSSC_SeStop,
	FSSC_SeWait,
	FSSC_MePlay,
	FSSC_MeWait,
	FSSC_BgmPlay,
	FSSC_BgmStop,
	//ƒtƒ@ƒNƒgƒŠ[
	FSSC_FactoryWorkAlloc,
	FSSC_FactoryWorkInit,
	FSSC_FactoryWorkFree,
	FSSC_FactoryRentalCall,
	FSSC_FactoryCallGetResult,
	FSSC_FactoryBattleCall,
	FSSC_FactoryTradeCall,
	FSSC_FactoryRentalPokePartySet,
	FSSC_FactoryBtlAfterPokePartySet,
	FSSC_FactoryTradePokeChange,
	FSSC_FactoryTradeAfterPokePartySet,
	FSSC_FactoryScrWork,
	FSSC_FactoryLoseCheck,
	FSSC_FactorySendBuf,
	FSSC_FactoryRecvBuf,
	FSSC_FactoryTalkMsgAppear,
	//˜^‰æ
	FSSC_BattleRecInit,
	FSSC_BattleRecSave,
	FSSC_BattleRecLoad,
	FSSC_BattleRecExit,
	FSSC_BattleRecDataOccCheck,
	FSSC_BattleRecServerVersionCheck,
	//ƒZ[ƒu
	FSSC_ReportSave,
	FSSC_ReportDivSave,
	FSSC_ExtraSaveInit,
	FSSC_ExtraSaveInitCheck,
	FSSC_StageRenshouCopyExtra,
	FSSC_TimeWaitIconAdd,
	FSSC_TimeWaitIconDel,
	//ƒ[ƒhƒZƒbƒg
	FSSC_ItemName,
	FSSC_NumberName,
	FSSC_PlayerName,
	FSSC_PairName,
	FSSC_WazaName,
	FSSC_PokemonNameExtra,
	FSSC_TypeName,
	FSSC_RivalName,
	//ƒoƒgƒ‹ƒ|ƒCƒ“ƒg
	FSSC_CheckBtlPoint,
	FSSC_AddBtlPoint,
	FSSC_SubBtlPoint,
	//ƒoƒgƒ‹ƒ^ƒ[
	FSSC_TowerScrWork,
	FSSC_TowerTalkMsgAppear,
	FSSC_TowerWorkRelease,
	FSSC_TowerBattleCall,
	FSSC_TowerCallGetResult,
	FSSC_TowerSendBuf,
	FSSC_TowerRecvBuf,
	//ƒXƒe[ƒW
	FSSC_StageWorkAlloc,
	FSSC_StageWorkEnemySet,
	FSSC_StageWorkFree,
	FSSC_StagePokeTypeSelCall,
	FSSC_StageCallGetResult,
	FSSC_StageBattleCall,
	FSSC_StageScrWork,
	FSSC_StageLoseCheck,
	FSSC_StageSendBuf,
	FSSC_StageRecvBuf,
	FSSC_StageTalkMsgAppear,
	//
	FSSC_DebugPrint,
	//ƒLƒƒƒbƒXƒ‹
	FSSC_CastleWorkAlloc,
	FSSC_CastleWorkInit,
	FSSC_CastleWorkFree,
	FSSC_CastleMineCall,
	FSSC_CastleCallGetResult,
	FSSC_CastleBattleCall,
	FSSC_CastleEnemyCall,
	FSSC_CastleBtlBeforePokePartySet,
	FSSC_CastleBtlAfterPokePartySet,
	FSSC_CastleScrWork,
	FSSC_CastleLoseCheck,
	FSSC_CastleSendBuf,
	FSSC_CastleRecvBuf,
	FSSC_CastleRankUpCall,
	FSSC_CastleParentCheckWait,
	FSSC_CastleGetCP,
	FSSC_CastleSubCP,
	FSSC_CastleAddCP,
	FSSC_CastleTalkMsgAppear,
	//WiFió•t
	FSSC_WiFiCounterWorkAlloc,
	FSSC_WiFiCounterWorkFree,
	FSSC_WiFiCounterSendBuf,
	FSSC_WiFiCounterRecvBuf,
	FSSC_WiFiCounterBFNoCheck,
	FSSC_WiFiCounterPokeList,
	FSSC_WiFiCounterPokeListGetResult,
	FSSC_WiFiCounterPairYameruCheck,
	FSSC_WiFiCounterScrWork,
	FSSC_WiFiCounterTowerCallBefore,
	FSSC_WiFiCounterTowerSendTrainerData,
	FSSC_WiFiCounterTowerRecvTrainerData,
	FSSC_WiFiCounterTowerCallAfter,
	FSSC_WifiCounterListSetLastPlayDate,
	//ƒ‹[ƒŒƒbƒg
	FSSC_RouletteWorkAlloc,
	FSSC_RouletteWorkInit,
	FSSC_RouletteWorkFree,
	FSSC_RouletteCallGetResult,
	FSSC_RouletteBattleCall,
	FSSC_RouletteBtlBeforePokePartySet,
	FSSC_RouletteBtlAfterPokePartySet,
	FSSC_RouletteScrWork,
	FSSC_RouletteLoseCheck,
	FSSC_RouletteSendBuf,
	FSSC_RouletteRecvBuf,
	FSSC_RouletteRankUpCall,
	FSSC_RouletteTalkMsgAppear,
	FSSC_RouletteDecideEvNoFunc,
	FSSC_RouletteChgItemKeepVanish,
	//
	FSSC_TVTempStageSet,
	FSSC_TVTempFriendSet,
	//
	FSSC_CommSetWifiBothNet,
    // MatchComment: add two new script commands
    ov104_2231F44,
    ov104_2231F5C
    
};

const u32 FSSCmdTableMax = NELEMS(FSSCmdTable);


//==============================================================================
//
//	
//
//==============================================================================

//-----------------------------------------------------------------------------
/**
 * @brief	ƒCƒ“ƒ‰ƒCƒ“ŠÖ”Fƒ[ƒN‚ğæ“¾‚·‚é
 * @param	core	‰¼‘zƒ}ƒVƒ“§Œäƒ[ƒN‚Ö‚Ìƒ|ƒCƒ“ƒ^
 * @return	u16 *	ƒ[ƒN‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * Ÿ‚Ì2ƒoƒCƒg‚ğƒ[ƒN‚ğw’è‚·‚éID‚Æ‚İ‚È‚µ‚ÄAƒ[ƒN‚Ö‚Ìƒ|ƒCƒ“ƒ^‚ğæ“¾‚·‚é
 */
//-----------------------------------------------------------------------------
//static inline u16 * FSSTGetWork(FSS_TASK *core)
u16 * FSSTGetWork(FSS_TASK *core)
{
	u16 *work;
	u16 no = FSSTGetU16(core);
	
	work = FSS_GetEventWorkAdrs( core, no );
	if( work == NULL ){
		OS_Printf( "work‚ªNULL no = %d\n", no );
	}
	GF_ASSERT(work != NULL);
	return work;
}

//-----------------------------------------------------------------------------
/**
 * @brief	ƒCƒ“ƒ‰ƒCƒ“ŠÖ”Fƒ[ƒN‚©‚ç’l‚ğæ“¾‚·‚é
 * @param	core	‰¼‘zƒ}ƒVƒ“§Œäƒ[ƒN‚Ö‚Ìƒ|ƒCƒ“ƒ^
 * @return	u16		’l
 *
 * Ÿ‚Ì2ƒoƒCƒg‚ªFSW_LOCAL_STARTˆÈ‰º‚Å‚ ‚ê‚Î’l‚Æ‚µ‚Äó‚¯æ‚éB
 * ‚»‚êˆÈã‚Ìê‡‚Íƒ[ƒN‚ğw’è‚·‚éID‚Æ‚İ‚È‚µ‚ÄA‚»‚Ìƒ[ƒN‚©‚ç’l‚ğæ“¾‚·‚é
 */
//-----------------------------------------------------------------------------
//static inline u16 FSSTGetWorkValue(FSS_TASK * core)
u16 FSSTGetWorkValue(FSS_TASK * core)
{
	return FSS_GetEventWorkValue( core, FSSTGetU16(core) );
}

//--------------------------------------------------------------
/**
 * @brief   ƒ[ƒNID‚©‚çg—p‚·‚éƒ[ƒN‚ÌƒAƒhƒŒƒX‚ğæ“¾‚·‚é
 *
 * @param   core		
 * @param   wk_id		ƒ[ƒNID
 *
 * @retval  ƒ[ƒNƒAƒhƒŒƒX
 */
//--------------------------------------------------------------
//static u16 * GetEventWorkAdrs(FSS_TASK *core, u16 wk_id)
u16 * FSS_GetEventWorkAdrs(FSS_TASK *core, u16 wk_id)
{
	if(wk_id < FSW_LOCAL_START){
		return NULL;
	}
	else if(wk_id < FSW_LOCAL_MAX){
		return &core->local_work[wk_id - FSW_LOCAL_START];
	}
	else if(wk_id < FSW_WORK_MAX){

		return FSS_ParamWorkAdrsGet(core->fss, wk_id - FSW_PARAM_START);
	}
	else if(wk_id < FSW_REG_MAX){
		return &core->reg[wk_id - FSW_REG_START];
	}
	//ˆÈ‰ºA“Á•Êˆ—
	else if(wk_id == FSS_DATA_END_CODE){
		return NULL;
	}
	else if(wk_id == FSS_CODE_MYSELF){
		return NULL;
	}
	else if(wk_id == FSS_CODE_SIO_PLAYER){
		return NULL;
	}
	
	GF_ASSERT(0);	//‘¶İ‚µ‚È‚¢ƒ[ƒNID
	return NULL;
}

//------------------------------------------------------------------
/**
 * @brief	ƒCƒxƒ“ƒgƒ[ƒN‚Ì’l‚ğæ“¾
 *
 * @param	fsys		FIELDSYS_WORK‚Ö‚Ìƒ|ƒCƒ“ƒ^
 * @param	work_no		ƒ[ƒNƒiƒ“ƒo[
 *
 * @return	"ƒ[ƒN‚Ì’l"
 */
//------------------------------------------------------------------
//static u16 GetEventWorkValue(FSS_TASK *core, u16 work_id )
u16 FSS_GetEventWorkValue(FSS_TASK *core, u16 work_id )
{
	u16* res = FSS_GetEventWorkAdrs( core, work_id );
	if( res == NULL ){ return work_id; }
	return *res;
}


//==============================================================================
//
//	–½—ßŠÖ”ŒQ
//
//==============================================================================

//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------------------------------------
/**
 * ‰½‚à‚µ‚È‚¢iƒfƒoƒbƒK‚Åˆø‚ÁŠ|‚¯‚é‚½‚ß‚Ì–½—ßj
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_Dummy( FSS_TASK * core )
{
	OS_TPrintf("FSSC ƒ_ƒ~[\n");
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒXƒNƒŠƒvƒg‚ÌI—¹
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_End( FSS_TASK * core )
{
	OS_TPrintf("FSSC END\n");
	FSST_End( core );
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief   2Dƒ}ƒbƒvI—¹
 *
 * @param   core		
 */
//--------------------------------------------------------------
static BOOL FSSC_ScriptFinish(FSS_TASK *core)
{
	FSST_End(core);
	Frontier_FinishReq(core->fss->fmain);
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief   ƒ}ƒbƒvØ‚è‘Ö‚¦
 *
 * @param   core		
 *
 * @retval  
 */
//--------------------------------------------------------------
static BOOL FSSC_MapChange(FSS_TASK *core)
{
	FSS_PTR fss = core->fss;
	u16 scene_id;
	
	scene_id = FSSTGetWorkValue( core );
	Frontier_MapChangeReq(fss->fmain, scene_id, FS_MAPCHANGE_EVENTID_NULL);

	FSST_SetWait( core, MapChangeWait);
	return 1;
}

//return 1 = I—¹
static BOOL MapChangeWait(FSS_TASK * core)
{
	return 1;
}

//--------------------------------------------------------------
/**
 * @brief   ƒ}ƒbƒvØ‚è‘Ö‚¦(ƒXƒNƒŠƒvƒgƒtƒ@ƒCƒ‹‚à“Ç‚İ‚İ‚È‚¨‚·)
 *
 * @param   core		
 *
 * @retval  
 */
//--------------------------------------------------------------
static BOOL FSSC_MapChangeEX(FSS_TASK *core)
{
	FSS_PTR fss = core->fss;
	u16 scene_id, event_id;
	
	scene_id = FSSTGetWorkValue( core );
	event_id = FSSTGetWorkValue( core );
	OS_Printf( "ƒ}ƒbƒvØ‚è‘Ö‚¦ƒV[ƒ“ = %d\n", scene_id );
	Frontier_MapChangeReq(fss->fmain, scene_id, event_id);

	FSST_End( core );
	return 0;	//ƒXƒNƒŠƒvƒgƒtƒ@ƒCƒ‹‚ğØ‚è‘Ö‚¦‚é‚Ì‚ÅAFSST_End‚ª‚±‚Ìê‚Å”½‰f‚³‚ê‚é‚æ‚¤‚É0‚Å•Ô‚·
}

//--------------------------------------------------------------------------------------------
/**
 * ƒEƒFƒCƒgˆ—
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"1"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_TimeWait( FSS_TASK * core )
{
	u16 num				= FSSTGetU16( core );
	u16 wk_id			= FSSTGetU16( core );
	u16* ret_wk			= FSS_GetEventWorkAdrs( core, wk_id );

	*ret_wk = num;

	//‰¼‘zƒ}ƒVƒ“‚Ì”Ä—pƒŒƒWƒXƒ^‚Éƒ[ƒN‚ÌID‚ğŠi”[
	core->reg[0] = wk_id;

	FSST_SetWait( core, EvWaitTime );
	return 1;
}

//return 1 = I—¹
static BOOL EvWaitTime(FSS_TASK * core)
{
	u16* ret_wk = FSS_GetEventWorkAdrs( core, core->reg[0] );	//’ˆÓI

	(*ret_wk)--;
	if( *ret_wk == 0 ){ 
		return 1;
	}
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒ[ƒN‚É’l‚ğŠi”[
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_LoadWkValue( FSS_TASK * core )
{
	u16 * work;

	work = FSSTGetWork( core );
	*work = FSSTGetU16( core );
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒ[ƒN‚Éƒ[ƒN‚Ì’l‚ğŠi”[
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_LoadWkWk( FSS_TASK * core )
{
	u16 * wk1;
	u16 * wk2;

	wk1 = FSSTGetWork( core );
	wk2 = FSSTGetWork( core );
	*wk1 = *wk2;
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒ[ƒN‚É’l‚ğ‘«‚·
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_WkAdd( FSS_TASK * core )
{
	u16 * work;
	u16 num;
	work = FSSTGetWork( core );
	*work += FSSTGetWorkValue( core );
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒ[ƒN‚É’l‚ğˆø‚­
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_WkSub( FSS_TASK * core )
{
	u16 * work;
	u16 num;
	work = FSSTGetWork( core );
	*work -= FSSTGetWorkValue( core );
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ‚Q‚Â‚Ì’l‚ğ”äŠr
 *
 * @param	r1		’l‚P
 * @param	r2		’l‚Q
 *
 * @retval	"r1 < r2 : MISUS_RESULT"
 * @retval	"r1 = r2 : EQUAL_RESULT"
 * @retval	"r1 > r2 : PLUS_RESULT"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_CmpMain( u16 r1, u16 r2 )
{
	if( r1 < r2 ){
		return MINUS_RESULT;
	}else if( r1 == r2 ){
		return EQUAL_RESULT;
	}
	return PLUS_RESULT;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒ[ƒN‚Æ’l‚ğ”äŠr
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_CmpWkValue( FSS_TASK * core )
{
	u16 * wk;
	u16	r1, r2;

	r1 = FSSTGetWorkValue(core);
	r2 = FSSTGetU16( core );
	core->cmp_flag = FSSC_CmpMain( r1, r2 );

	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒ[ƒN‚Æƒ[ƒN‚ğ”äŠr
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_CmpWkWk( FSS_TASK * core )
{
	u16 * wk1;
	u16 * wk2;
	u16	r1, r2;

	wk1 = FSSTGetWork(core);
	wk2 = FSSTGetWork(core);
	core->cmp_flag = FSSC_CmpMain( *wk1, *wk2 );

	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ‰¼‘zƒ}ƒVƒ“ƒWƒƒƒ“ƒv–½—ß
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 * @param	adrs		ƒWƒƒƒ“ƒvæƒAƒhƒŒƒX
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FSSC_Jump( FSS_TASK * core, FSS_CODE * adrs )
{
	core->PC = adrs;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒXƒNƒŠƒvƒgƒWƒƒƒ“ƒv
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 *
 * @li	EVCMD_JUMP
 *
 *	•\‹LF	EVCMD_JUMP	JumpOffset(s16)
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_GlobalJump( FSS_TASK * core )
{
	s32	pos;
	pos = (s32)FSSTGetU32(core);
	FSSC_Jump( core, (FSS_CODE *)(core->PC+pos) );	//JUMP
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒXƒNƒŠƒvƒgğŒƒWƒƒƒ“ƒv
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_IfJump( FSS_TASK * core )
{
	u8	r;
	s32	pos;

	r   = FSSTGetU8(core);
	pos = (s32)FSSTGetU32(core);

	if( ConditionTable[r][core->cmp_flag] == TRUE ){
		FSSC_Jump( core, (FSS_CODE *)(core->PC+pos) );
	}
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒXƒNƒŠƒvƒgƒR[ƒ‹
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 *
 * @li	EVCMD_CALL
 *
 *	•\‹LF	EVCMD_CALL	CallOffset(s16)
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_GlobalCall( FSS_TASK * core )
{
	s32	pos = (s32)FSSTGetU32(core);
	FSSTCall( core, (FSS_CODE *)(core->PC+pos) );
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒXƒNƒŠƒvƒgƒŠƒ^[ƒ“
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_Ret( FSS_TASK * core )
{
	FSSTRet( core );
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒXƒNƒŠƒvƒgğŒƒR[ƒ‹
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_IfCall( FSS_TASK * core )
{
	u8	r;
	s32	pos;

	r   = FSSTGetU8(core);
	pos = (s32)FSSTGetU32(core);

	if( ConditionTable[r][core->cmp_flag] == TRUE ){
		FSSTCall( core, (FSS_CODE *)(core->PC+pos) );
	}
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * “o˜^‚³‚ê‚½’PŒê‚ğg‚Á‚Ä•¶š—ñ“WŠJ@ƒƒbƒZ[ƒW•\¦(ˆêŠ‡•\¦)
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	í‚É0
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_TalkMsgAllPut( FSS_TASK * core )
{
	FSCR_TALK_EX_PARAM talk_ex_param;
	u16 msg_id = FSSTGetU16(core);

	talk_ex_param.msg_speed = MSG_ALLPUT;		///<ƒƒbƒZ[ƒW‘¬“x
	talk_ex_param.auto_flag = 0;				///<©“®‘—‚èİ’è(MSG_AUTO_???)
	talk_ex_param.font		= FONT_TALK;		///<ƒtƒHƒ“ƒg
	talk_ex_param.dummy		= 0;

	FSSC_Sub_ScrTalkMsg(core->fss, core->msgman, msg_id, 0, &talk_ex_param);
	//FSSC_Sub_ScrTalkMsg(core->fss, core->msgman, msg_id, 0, NULL);
	//FSST_SetWait( core, TalkMsgWait );
	//OS_TPrintf("ˆêŠ‡‰ï˜bƒƒbƒZ[ƒWŠJn\n");
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * “o˜^‚³‚ê‚½’PŒê‚ğg‚Á‚Ä•¶š—ñ“WŠJ@ƒƒbƒZ[ƒW•\¦(ƒXƒLƒbƒv‚È‚µ)
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	í‚É1
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_TalkMsgNoSkip( FSS_TASK * core )
{
	u16 msg_id = FSSTGetU16(core);
	FSSC_Sub_ScrTalkMsg(core->fss, core->msgman, msg_id, 0, NULL);
	FSST_SetWait( core, TalkMsgWait );
	OS_TPrintf("‰ï˜bƒƒbƒZ[ƒWŠJn\n");
	return 1;
}

//--------------------------------------------------------------------------------------------
/**
 * “o˜^‚³‚ê‚½’PŒê‚ğg‚Á‚Ä•¶š—ñ“WŠJ@ƒƒbƒZ[ƒW•\¦
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	í‚É1
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_TalkMsg( FSS_TASK * core )
{
	u16 msg_id = FSSTGetU16(core);
	FSSC_Sub_ScrTalkMsg(core->fss, core->msgman, msg_id, 1, NULL);
	FSST_SetWait( core, TalkMsgWait );
	OS_TPrintf("‰ï˜bƒƒbƒZ[ƒWŠJn\n");
	return 1;
}

static BOOL TalkMsgWait(FSS_TASK * core)
{
	if(GF_MSG_PrintEndCheck(core->fss->msg_talk_index) == 0){
		return 1;
	}
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ‰ï˜bƒEƒBƒ“ƒhƒE‚ğ•Â‚¶‚é
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_TalkWinClose( FSS_TASK * core )
{
	FSSC_Sub_ScrTalkClose(core->fss);
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒƒCƒvƒtƒF[ƒhƒXƒ^[ƒg
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 *
 * ’ˆÓI@ƒtƒF[ƒh‚É‚©‚©‚éLCDABG–Ê‚Ìİ’è‚ÍŒÅ’è‚É‚µ‚Ä‚¢‚é
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_WipeFadeStart( FSS_TASK * core )
{
	u16 div				= FSSTGetU16(core);		//ŠeƒƒCƒvˆ—‚Ì•ªŠ„”
	u16 sync			= FSSTGetU16(core);		//ŠeƒƒCƒv‚Ìˆ—‚ğ•ªŠ„‚µ‚½‚P•Ğ‚ÌƒVƒ“ƒN”
	u16 type			= FSSTGetU16(core);		//ƒ^ƒCƒv
	u16 color			= FSSTGetU16(core);		//ƒJƒ‰[

	WIPE_SYS_Start( WIPE_PATTERN_WMS, type, type, color, div, sync, HEAPID_WORLD );

	//ƒEƒBƒ“ƒhƒEƒ}ƒXƒNó‘Ô‚ğ‰ğœ
	WIPE_ResetWndMask( WIPE_DISP_MAIN );
	WIPE_ResetWndMask( WIPE_DISP_SUB );

	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒƒCƒvƒtƒF[ƒhI—¹ƒ`ƒFƒbƒN
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"1"
 *
 * ’ˆÓI@ƒ`ƒFƒbƒN‚·‚éLCD‚Ìİ’è‚ÍŒÅ’è‚É‚µ‚Ä‚¢‚é
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_WipeFadeCheck( FSS_TASK * core )
{
	FSST_SetWait( core, WaitWipeFadeCheck );
	return 1;
}

//return 1 = I—¹
static BOOL WaitWipeFadeCheck(FSS_TASK * core)
{
	if( WIPE_SYS_EndCheck() == TRUE ){
		return 1;
	}

	return 0;
}

//==============================================================================
//	BMPƒƒjƒ…[
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief	BMPƒƒjƒ…[	‰Šú‰»
 *
 * @param	none
 *
 * @return	1
 */
//--------------------------------------------------------------
static BOOL FSSC_BmpMenuInit( FSS_TASK * core )
{
	FSS_PTR fss = core->fss;
	u8 x				= FSSTGetU8(core);
	u8 y				= FSSTGetU8(core);
	u8 cursor			= FSSTGetU8(core);
	u8 cancel			= FSSTGetU8(core);
	u16 wk_id			= FSSTGetU16( core );
	
	//‰Šú‰»
	fss->ev_win	= FSSC_Sub_BmpMenu_Init(fss, x, y, cursor, cancel, 
								FSS_GetEventWorkAdrs(core, wk_id), fss->wordset, 
								NULL );

	//‰¼‘zƒ}ƒVƒ“‚Ì”Ä—pƒŒƒWƒXƒ^‚Éƒ[ƒN‚ÌID‚ğŠi”[
	core->reg[0] = wk_id;

	return 1;
}

//--------------------------------------------------------------
/**
 * @brief	BMPƒƒjƒ…[	‰Šú‰»(“Ç‚İ‚ñ‚Å‚¢‚égmmƒtƒ@ƒCƒ‹‚ğg—p‚·‚é)
 *
 * @param	none
 *
 * @return	1
 */
//--------------------------------------------------------------
static BOOL FSSC_BmpMenuInitEx( FSS_TASK * core )
{
	FSS_PTR fss = core->fss;
	u8 x				= FSSTGetU8(core);
	u8 y				= FSSTGetU8(core);
	u8 cursor			= FSSTGetU8(core);
	u8 cancel			= FSSTGetU8(core);
	u16 wk_id			= FSSTGetU16( core );
	
	//‰Šú‰»
	fss->ev_win	= FSSC_Sub_BmpMenu_Init(fss, x, y, cursor, cancel, 
								FSS_GetEventWorkAdrs(core,wk_id), fss->wordset, 
								core->msgman );

	//‰¼‘zƒ}ƒVƒ“‚Ì”Ä—pƒŒƒWƒXƒ^‚Éƒ[ƒN‚ÌID‚ğŠi”[
	core->reg[0] = wk_id;

	return 1;
}

//--------------------------------------------------------------
/**
 * @brief	BMPƒƒjƒ…[	ƒŠƒXƒgì¬(u16ƒo[ƒWƒ‡ƒ“)
 *
 * @param	none
 *
 * @retval	0
 */
//--------------------------------------------------------------
static BOOL FSSC_BmpMenuMakeList( FSS_TASK * core )
{
	u16 msg_id,talk_msg_id,param;
	FSS_PTR fss = core->fss;

//	msg_id	= FSSTGetWorkValue(core);
//	param	= FSSTGetWorkValue(core);
	msg_id = FSSTGetU16(core);
	talk_msg_id = FSEV_WIN_TALK_MSG_NONE;		//ƒŠƒXƒgg—p‚Ì‰ï˜bƒƒbƒZ[ƒW‚È‚µ
	param = FSSTGetU16(core);

	FSSC_Sub_BmpMenu_MakeList(fss->ev_win, msg_id, talk_msg_id, param );
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	BMPƒƒjƒ…[	ƒŠƒXƒgì¬(u16ƒo[ƒWƒ‡ƒ“)(‰ï˜bƒEƒBƒ“ƒƒbƒZ[ƒWw’è‚ ‚è)
 *
 * @param	none
 *
 * @retval	0
 */
//--------------------------------------------------------------
static BOOL FSSC_BmpMenuMakeListTalkMsg( FSS_TASK * core )
{
	u16 msg_id,talk_msg_id,param;
	FSS_PTR fss = core->fss;

//	msg_id	= FSSTGetWorkValue(core);
//	param	= FSSTGetWorkValue(core);
	msg_id = FSSTGetU16(core);
	talk_msg_id = FSSTGetU16(core);
	param = FSSTGetU16(core);

	FSSC_Sub_BmpMenu_MakeList(fss->ev_win, msg_id, talk_msg_id, param );
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	BMPƒƒjƒ…[	ŠJn
 *
 * @param	none
 *
 * @retval	1
 */
//--------------------------------------------------------------
static BOOL FSSC_BmpMenuStart( FSS_TASK * core )
{
	FSS_PTR fss = core->fss;

	FSSC_Sub_BmpMenu_Start( fss->ev_win );
	
	FSST_SetWait( core, EvSelMenuWinWait );
	return 1;
}

//ƒEƒFƒCƒgŠÖ”
static BOOL EvSelMenuWinWait(FSS_TASK * core)
{
	FSS_PTR fss = core->fss;
	u16* ret_wk = FSS_GetEventWorkAdrs(core, core->reg[0] );	//’ˆÓI

	if( *ret_wk == FSEV_WIN_NOTHING ){
		return FALSE;											//Œp‘±
	}

	fss->ev_win = NULL;
	return TRUE;												//I—¹
}

//==============================================================================
//	BMPƒŠƒXƒg
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief	BMPƒŠƒXƒg	‰Šú‰»
 *
 * @param	none
 *
 * @return	1
 */
//--------------------------------------------------------------
static BOOL FSSC_BmpListInit( FSS_TASK * core )
{
	FSS_PTR fss = core->fss;
	u8 x				= FSSTGetU8(core);
	u8 y				= FSSTGetU8(core);
	u8 cursor			= FSSTGetU8(core);
	u8 cancel			= FSSTGetU8(core);
	u16 wk_id			= FSSTGetU16( core );
	
	//‰Šú‰»
	fss->ev_win	= FSSC_Sub_BmpList_Init(fss, x, y, cursor, cancel, 
								FSS_GetEventWorkAdrs(core, wk_id), fss->wordset, 
								NULL );

	//‰¼‘zƒ}ƒVƒ“‚Ì”Ä—pƒŒƒWƒXƒ^‚Éƒ[ƒN‚ÌID‚ğŠi”[
	core->reg[0] = wk_id;

	return 1;
}

//--------------------------------------------------------------
/**
 * @brief	BMPƒŠƒXƒg	‰Šú‰»(“Ç‚İ‚ñ‚Å‚¢‚égmmƒtƒ@ƒCƒ‹‚ğg—p‚·‚é)
 *
 * @param	none
 *
 * @return	1
 */
//--------------------------------------------------------------
static BOOL FSSC_BmpListInitEx( FSS_TASK * core )
{
	FSS_PTR fss = core->fss;
	u8 x				= FSSTGetU8(core);
	u8 y				= FSSTGetU8(core);
	u8 cursor			= FSSTGetU8(core);
	u8 cancel			= FSSTGetU8(core);
	u16 wk_id			= FSSTGetU16( core );
	
	//‰Šú‰»
	fss->ev_win	= FSSC_Sub_BmpList_Init(fss, x, y, cursor, cancel, 
								FSS_GetEventWorkAdrs(core,wk_id), fss->wordset, 
								core->msgman );

	//‰¼‘zƒ}ƒVƒ“‚Ì”Ä—pƒŒƒWƒXƒ^‚Éƒ[ƒN‚ÌID‚ğŠi”[
	core->reg[0] = wk_id;

	return 1;
}

//--------------------------------------------------------------
/**
 * @brief	BMPƒŠƒXƒg	ƒŠƒXƒgì¬(u16ƒo[ƒWƒ‡ƒ“)
 *
 * @param	none
 *
 * @retval	0
 */
//--------------------------------------------------------------
static BOOL FSSC_BmpListMakeList( FSS_TASK * core )
{
	u16 msg_id,talk_msg_id,param;
	FSS_PTR fss = core->fss;

//	msg_id	= FSSTGetWorkValue(core);
//	param	= FSSTGetWorkValue(core);
	msg_id = FSSTGetU16(core);
	talk_msg_id = FSSTGetU16(core);
	param = FSSTGetU16(core);

	FSSC_Sub_BmpList_MakeList(fss->ev_win, msg_id, talk_msg_id, param );
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	BMPƒŠƒXƒg	ŠJn
 *
 * @param	none
 *
 * @retval	1
 */
//--------------------------------------------------------------
static BOOL FSSC_BmpListStart( FSS_TASK * core )
{
	FSS_PTR fss = core->fss;

	FSSC_Sub_BmpList_Start( fss->ev_win );
	
	//FSST_SetWait( core, EvSelListWinWait );
	FSST_SetWait( core, EvSelMenuWinWait );
	return 1;
}

#if 0
//ƒEƒFƒCƒgŠÖ”
static BOOL EvSelListWinWait(FSS_TASK * core)
{
	FSS_PTR fss = core->fss;
	u16* ret_wk = FSS_GetEventWorkAdrs(core, core->reg[0] );	//’ˆÓI

	if( *ret_wk == FSEV_WIN_NOTHING ){
		return FALSE;											//Œp‘±

	}else if( *ret_wk == FSEV_WIN_COMPULSION_DEL ){
		FSSC_Sub_BmpList_Del( fss->ev_win );					//‹­§íœ
	}

	fss->ev_win = NULL;
	return TRUE;												//I—¹
}
#endif

//--------------------------------------------------------------
/**
 * @brief	BMPƒŠƒXƒg	‹­§íœ
 *
 * @param	none
 *
 * @retval	1
 *
 * ƒoƒgƒ‹ƒLƒƒƒbƒXƒ‹‚Ì‚İ‚Åg—p
 */
//--------------------------------------------------------------
static BOOL FSSC_BmpListDel( FSS_TASK * core )
{
	FSS_PTR fss = core->fss;
	FSSC_Sub_BmpList_Del( fss->ev_win );
	return 1;
}

//--------------------------------------------------------------------------------------------
/**
 * u‚Í‚¢E‚¢‚¢‚¦vˆ—
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	1
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_YesNoWin( FSS_TASK * core )
{
	FSS_PTR fss = core->fss;
	FMAP_PTR fmap = FSS_GetFMapAdrs(fss);
	u16 wk_id			= FSSTGetU16( core );
	u8 csr_pos			= FSSTGetU8( core );

	//fss->mw = BmpYesNoSelectInit(
			//fmap->bgl, &YesNoBmpDat, FR_MENU_WIN_CGX_NUM, FR_MENU_WIN_PAL, fss->heap_id );
	fss->mw = BmpYesNoSelectInitEx(
			fmap->bgl, &YesNoBmpDat, FR_MENU_WIN_CGX_NUM, FR_MENU_WIN_PAL, csr_pos, fss->heap_id );

	//‰¼‘zƒ}ƒVƒ“‚Ì”Ä—pƒŒƒWƒXƒ^‚Éƒ[ƒN‚ÌID‚ğŠi”[
	core->reg[0] = wk_id;

	FSST_SetWait( core, EvYesNoSelect );

	return 1;
}

static BOOL EvYesNoSelect(FSS_TASK * core)
{
	u32	ret;
	FSS_PTR fss = core->fss;
	u16* ret_wk			= FSS_GetEventWorkAdrs(core, core->reg[0] );	//’ˆÓI
	
	ret  = BmpYesNoSelectMain( fss->mw, fss->heap_id );

	if( ret == BMPMENU_NULL ){ return 0; }

	if( ret == 0 ){
		*ret_wk = 0;
	}else{
		*ret_wk = 1;
	}
	return 1;
}

//--------------------------------------------------------------
/**
 * @brief   ƒLƒƒƒ‰ƒNƒ^ƒŠƒ\[ƒX‚ğ“o˜^
 *
 * @param   core		
 */
//--------------------------------------------------------------
static BOOL FSS_CharResourceSet(FSS_TASK *core)
{
	FSS_PTR fss = core->fss;
	FMAP_PTR fmap = Frontier_FMapAdrsGet(fss->fmain);
	const FSS_CODE *pc_backup;
	FSS_CHAR_RESOURCE_DATA res;
	s32 pos;
	
	pos = FSSTGetU32(core);
	pc_backup = core->PC;
	core->PC = (FSS_CODE *)(core->PC+pos);
	
	while(1){
		res.charid = FSSTGetWorkValue(core);
		if(res.charid == FSS_DATA_END_CODE){
			break;
		}

		res.movetype = FSSTGetU8(core);
		if(res.charid == FSS_CODE_MYSELF){
			const MYSTATUS *my_status;
			FRONTIER_EX_PARAM *ex_param;

			ex_param = Frontier_ExParamGet(fss->fmain);
			my_status = SaveData_GetMyStatus(ex_param->savedata);
			res.charid = FrontierTool_MyObjCodeGet(my_status);
			
			OS_TPrintf("ålŒö“o˜^ charid = %d, movetype = %d\n", res.charid, res.movetype);
			FMap_CharResourceSet(fmap, &res);
		}
		else if(res.charid == FSS_CODE_SIO_PLAYER){
			if(CommIsInitialize() == TRUE){
				const MYSTATUS *my_status;
				int comm_num, i;
				
				comm_num = CommGetConnectNum();
				for(i = 0; i < comm_num; i++){
					my_status = CommInfoGetMyStatus(i);
					res.charid = FrontierTool_MyObjCodeGet(my_status);
					
					OS_TPrintf("’ÊMƒvƒŒƒCƒ„[“o˜^ charid = %d, movetype = %d\n", res.charid, res.movetype);
					FMap_CharResourceSet(fmap, &res);
				}
			}
			else{	//’ÊM‚µ‚Ä‚¢‚È‚¢
				;		//‰½‚à“o˜^‚µ‚È‚¢
			}
		}
		else{
			OS_TPrintf("charid = %d, movetype = %d\n", res.charid, res.movetype);
			FMap_CharResourceSet(fmap, &res);
		}
	}
	
	core->PC = pc_backup;
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief   ƒLƒƒƒ‰ƒNƒ^ƒŠƒ\[ƒX‚ğíœ
 *
 * @param   core		
 */
//--------------------------------------------------------------
static BOOL FSS_CharResourceFree(FSS_TASK *core)
{
	FSS_PTR fss = core->fss;
	FMAP_PTR fmap = Frontier_FMapAdrsGet(fss->fmain);
	u16 charid;
	
	charid = FSSTGetWorkValue(core);
	FMap_CharResourceFree(fmap, charid);

	return 0;
}

//--------------------------------------------------------------
/**
 * @brief   ƒAƒNƒ^[“o˜^
 *
 * @param   core		
 *
 * playid, charid‚¾‚¯ƒ[ƒNŒo—R‚Åó‚¯“n‚µ‰Â”\‚Å‚·
 * À•W‚Íƒ}ƒCƒiƒX‚ª‚ ‚è‚»‚¤‚È‚Ì‚Å‚â‚Á‚Ä‚¢‚Ü‚¹‚ñ
 */
//--------------------------------------------------------------
static BOOL FSS_ActorSet(FSS_TASK *core)
{
	FSS_PTR fss = core->fss;
	FMAP_PTR fmap = Frontier_FMapAdrsGet(fss->fmain);
	const FSS_CODE *pc_backup;
	s32 pos;
	FSS_CHAR_ACTOR_DATA res;
	int act_pos;
	
	pos = FSSTGetU32(core);
	pc_backup = core->PC;
	core->PC = (FSS_CODE *)(core->PC+pos);
	
	while(1){
		act_pos = FMAP_ACT_POS_AUTO;
		res.playid = FSSTGetWorkValue(core);	//objid=playid
		if(res.playid == FSS_DATA_END_CODE){
			break;
		}
		res.charid = FSSTGetWorkValue(core);
		res.way = FSSTGetU8(core);
		res.x = FSSTGetU16(core) + FSS_ACTOR_SET_OFFSET_X;
		res.y = FSSTGetU16(core) + FSS_ACTOR_SET_OFFSET_Y;
		res.visible = FSSTGetU8(core);
		res.event_id = FSSTGetU8(core);
		res.status = WF2DMAP_OBJST_C_NONE;	//ƒXƒe[ƒ^ƒX‚¾‚¯ŒÅ’è
		if(res.charid == FSS_CODE_MYSELF){
			const MYSTATUS *my_status;
			FRONTIER_EX_PARAM *ex_param;

			ex_param = Frontier_ExParamGet(fss->fmain);
			my_status = SaveData_GetMyStatus(ex_param->savedata);
			res.charid = FrontierTool_MyObjCodeGet(my_status);
			
			act_pos = FIELD_OBJ_PLAYER;
		}
		else if(res.charid == FSS_CODE_SIO_PLAYER){
			const MYSTATUS *my_status;
			int netid;
			
			netid = FSSTGetU8(core);
			my_status = CommInfoGetMyStatus(netid);
			res.charid = FrontierTool_MyObjCodeGet(my_status);
		}
		else{
			;
		}
		OS_TPrintf("charid = %d, way = %d, x = %d, y = %d, visible = %d, event_id = %d, playid = %d, status = %d\n", res.charid, res.way, res.x, res.y, res.visible, res.event_id, res.playid, res.status);
		
		FMap_ActorSet(fmap, &res, act_pos);
	}
	
	core->PC = pc_backup;
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief   ƒAƒNƒ^[íœ
 *
 * @param   core		
 */
//--------------------------------------------------------------
static BOOL FSS_ActorFree(FSS_TASK *core)
{
	FSS_PTR fss = core->fss;
	FMAP_PTR fmap = Frontier_FMapAdrsGet(fss->fmain);
	WF2DMAP_OBJWK *objwk;
	u16 playid;
	
	playid = FSSTGetWorkValue(core);	//objid=playid
	objwk = WF2DMAP_OBJWkGet(fmap->objsys, playid);
	FMap_ActorFree(fmap, objwk);

	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒAƒNƒ^[•\¦E”ñ•\¦İ’è
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSS_ActorVisibleSet( FSS_TASK * core )
{
	FSS_PTR fss = core->fss;
	FMAP_PTR fmap = Frontier_FMapAdrsGet(fss->fmain);
	u16 playid	= FSSTGetWorkValue(core);	//objid = playid
	u8 visible_flg = FSSTGetU8(core);
	WF2DMAP_OBJDRAWWK *drawwk = NULL;

	FMap_OBJWkGet(fmap, playid, NULL, &drawwk);
	GF_ASSERT(drawwk != NULL);
	WF2DMAP_OBJDrawWkDrawFlagSet(drawwk, visible_flg);

	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒAƒNƒ^[BG—Dæİ’è
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSS_ActorBgPriSet( FSS_TASK * core )
{
#if 0
	FSS_PTR fss = core->fss;
	FMAP_PTR fmap = Frontier_FMapAdrsGet(fss->fmain);
	u16 playid	= FSSTGetWorkValue(core);	//objid = playid
	u16 bg_pri	= FSSTGetU16(core);
	WF2DMAP_OBJDRAWWK *drawwk = NULL;

	FMap_OBJWkGet(fmap, playid, NULL, &drawwk);
	GF_ASSERT(drawwk != NULL);
	WF2DMAP_OBJDrawSysDefBgPriSet( drawwk, bg_pri );
#endif

	return 0;
}


//==============================================================================
//
//	ƒAƒjƒ[ƒVƒ‡ƒ“
//
//==============================================================================
//--------------------------------------------------------------------------------------------
/**
 * ƒAƒjƒ[ƒVƒ‡ƒ“
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_ObjAnime( FSS_TASK * core )
{
	FSS_PTR fss = core->fss;
	FMAP_PTR fmap = Frontier_FMapAdrsGet(fss->fmain);
	FSS_CODE* p;
	u16 playid	= FSSTGetWorkValue(core);	//objid = playid
	u32 pos		= (s32)FSSTGetU32(core);
	FSS_ACTOR_WORK *fss_actor;
	
	fss_actor = Frontier_ActorWorkSearch(fss->fmain, playid);

	if( fss_actor == NULL ){
		OS_Printf( "‘¶İ‚µ‚È‚¢ objid = %d\n", playid );
		GF_ASSERT(0);	//‘¶İ‚µ‚È‚¢
	}

	//ƒAƒjƒ[ƒVƒ‡ƒ“ƒRƒ}ƒ“ƒhƒŠƒXƒgƒZƒbƒg
	p = (FSS_CODE*)(core->PC+pos);
	FSSC_AnimeListSet(
		playid, fss_actor, (FSSC_ANIME_DATA*)p, fmap->actcmd_q, &fss->anm_count, fss->heap_id);

	//ƒAƒjƒ[ƒVƒ‡ƒ“‚Ì”‚ğ‘«‚·
	fss->anm_count++;
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief   OBJƒAƒjƒ[ƒVƒ‡ƒ“ƒ[ƒN•TCB¶¬
 *
 * @param   playid				OBJWK”F¯”Ô†
 * @param   objwk				ƒIƒuƒWƒFƒ[ƒN‚Ö‚Ìƒ|ƒCƒ“ƒ^
 * @param   anmlist				ƒAƒjƒ[ƒVƒ‡ƒ“ƒf[ƒ^‚Ö‚Ìƒ|ƒCƒ“ƒ^
 * @param   reqcmd_q			ƒŠƒNƒGƒXƒgƒRƒ}ƒ“ƒhƒLƒ…[‚Ö‚Ìƒ|ƒCƒ“ƒ^
 * @param   anm_count_ptr		ƒAƒjƒ[ƒVƒ‡ƒ“ƒJƒEƒ“ƒ^‚Ö‚Ìƒ|ƒCƒ“ƒ^
 */
//--------------------------------------------------------------
static void FSSC_AnimeListSet(u16 playid, FSS_ACTOR_WORK *fss_actor, const FSSC_ANIME_DATA *anmlist, WF2DMAP_ACTCMDQ *actcmd_q, u8 *anm_count_ptr, int heap_id)
{
	FSSC_ANIME_MOVE_WORK *move;
	
	move = sys_AllocMemory(heap_id, sizeof(FSSC_ANIME_MOVE_WORK));
	MI_CpuClear8(move, sizeof(FSSC_ANIME_MOVE_WORK));
	
	move->fss_actor = fss_actor;
	move->anm_list = anmlist;
	move->actcmd_q = actcmd_q;
	move->playid = playid;
	move->anm_count_ptr = anm_count_ptr;
	
	GF_ASSERT(fss_actor->anime_tcb == NULL);
	fss_actor->anime_tcb = TCB_Add(FSSC_Sub_AnimeListMain, move, TCBPRI_FIELDOBJ_ANIME);
}

//--------------------------------------------------------------------------------------------
/**
 * ƒtƒB[ƒ‹ƒhOBJ‘S‚Ä‚ÌƒAƒjƒ[ƒVƒ‡ƒ“I—¹‘Ò‚¿
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	1
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_ObjAnimeWait( FSS_TASK * core )
{
	FSST_SetWait( core, ObjAnmWait );
	return 1;
}

//--------------------------------------------------------------
/**
 * @brief   ƒtƒB[ƒ‹ƒhOBJ‘S‚Ä‚ÌƒAƒjƒ[ƒVƒ‡ƒ“I—¹‘Ò‚¿
 * @param   core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 * @retval 	1=I—¹
 */
//--------------------------------------------------------------
static BOOL ObjAnmWait(FSS_TASK * core)
{
	FSS_PTR fss = core->fss;
	
	if(fss->anm_count == 0){
		return 1;
	}
	return 0;
}

//==============================================================================
//	”z’u•¨ƒAƒNƒ^[
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   ”z’u•¨‚ÌƒLƒƒƒ‰ƒNƒ^ƒŠƒ\[ƒX‚ğ“o˜^
 *
 * @param   core		
 */
//--------------------------------------------------------------
static BOOL FSS_ArticleResourceSet(FSS_TASK *core)
{
	FSS_PTR fss = core->fss;
	FMAP_PTR fmap = Frontier_FMapAdrsGet(fss->fmain);
	const FSS_CODE *pc_backup;
	s32 pos;
	u16 act_id;
	ARCHANDLE *hdl_obj;
	
	pos = FSSTGetU32(core);
	pc_backup = core->PC;
	core->PC = (FSS_CODE *)(core->PC+pos);

	hdl_obj = ArchiveDataHandleOpen(ARC_FRONTIER_OBJ, fss->heap_id);

	while(1){
		act_id = FSSTGetWorkValue(core);
		if(act_id == FSS_DATA_END_CODE){
			break;
		}
		FAct_ResourceLoad(fmap->clactsys.csp, fmap->clactsys.crp, hdl_obj, fmap->pfd, act_id);
		FMap_ArticleActResourceIDSet(fmap, act_id);
	}

	ArchiveDataHandleClose(hdl_obj);
	
	core->PC = pc_backup;
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief   ”z’u•¨‚ÌƒLƒƒƒ‰ƒNƒ^ƒŠƒ\[ƒX‚ğíœ
 *
 * @param   core		
 */
//--------------------------------------------------------------
static BOOL FSS_ArticleResourceFree(FSS_TASK *core)
{
	FSS_PTR fss = core->fss;
	FMAP_PTR fmap = Frontier_FMapAdrsGet(fss->fmain);
	u16 act_id;
	
	act_id = FSSTGetWorkValue(core);
	FAct_ResourceFree(fmap->clactsys.crp, act_id);
	FMap_ArticleActResourceIDDel(fmap, act_id);

	return 0;
}

//--------------------------------------------------------------
/**
 * @brief   ”z’u•¨ƒAƒNƒ^[“o˜^
 *
 * @param   core		
 */
//--------------------------------------------------------------
static BOOL FSS_ArticleActorSet(FSS_TASK *core)
{
	FSS_PTR fss = core->fss;
	FMAP_PTR fmap = Frontier_FMapAdrsGet(fss->fmain);
	const FSS_CODE *pc_backup;
	s32 pos;
	u16 act_id, x, y, visible, anm_start, set_actwork_no;
	CATS_ACT_PTR cap;
	
	pos = FSSTGetU32(core);
	pc_backup = core->PC;
	core->PC = (FSS_CODE *)(core->PC+pos);
	
	while(1){
		act_id = FSSTGetWorkValue(core);
		if(act_id == FSS_DATA_END_CODE){
			break;
		}
		x = FSSTGetWorkValue(core);
		y = FSSTGetWorkValue(core);
		visible = FSSTGetWorkValue(core);
		anm_start = FSSTGetU8(core);
		set_actwork_no = FSSTGetU8(core);
		
		cap = FMap_ArticleActCreate(fmap, set_actwork_no, act_id);
		CATS_ObjectPosSetCap(cap, x, y);
		CATS_ObjectEnableCap(cap, visible);
		FMap_ArticleActAnmBitSet(fmap, set_actwork_no, anm_start);
	//	CATS_ObjectAutoAnimeSetCap(cap, anm_start);
	}
	
	core->PC = pc_backup;
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief   ”z’u•¨ƒAƒNƒ^[íœ
 *
 * @param   core		
 */
//--------------------------------------------------------------
static BOOL FSS_ArticleActorFree(FSS_TASK *core)
{
	FSS_PTR fss = core->fss;
	u16 actwork_no;
	FMAP_PTR fmap = Frontier_FMapAdrsGet(fss->fmain);

	actwork_no = FSSTGetWorkValue(core);
	FMap_ArticleActDel(fmap, actwork_no);

	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ”z’u•¨ƒAƒNƒ^[•\¦E”ñ•\¦İ’è
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSS_ArticleActorVisibleSet( FSS_TASK * core )
{
	FSS_PTR fss = core->fss;
	FMAP_PTR fmap = Frontier_FMapAdrsGet(fss->fmain);
	u16 actwork_no	= FSSTGetWorkValue(core);
	u8 visible_flg = FSSTGetU8(core);
	CATS_ACT_PTR cap;

	cap = FMap_ArticleActGet(fmap, actwork_no);
	GF_ASSERT(cap != NULL);
	CATS_ObjectEnableCap(cap, visible_flg);

	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ”z’u•¨ƒAƒNƒ^[ƒtƒŠƒbƒv…•½•ûŒüİ’è
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSS_ArticleActorFlipHSet( FSS_TASK * core )
{
	FSS_PTR fss = core->fss;
	FMAP_PTR fmap = Frontier_FMapAdrsGet(fss->fmain);
	u16 actwork_no	= FSSTGetWorkValue(core);
	CATS_ACT_PTR cap;

	cap = FMap_ArticleActGet(fmap, actwork_no);
	GF_ASSERT(cap != NULL);
	CATS_ObjectFlipSet(cap->act, CLACT_FLIP_H);

	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ”z’u•¨ƒAƒNƒ^[OBJƒ‚[ƒhİ’è(0=’ÊíA1=”¼“§–¾)
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSS_ArticleActorObjModeSet( FSS_TASK * core )
{
	CATS_ACT_PTR cap;
	FSS_PTR fss		= core->fss;
	FMAP_PTR fmap	= Frontier_FMapAdrsGet(fss->fmain);
	u16 actwork_no	= FSSTGetWorkValue(core);
	u16 flag		= FSSTGetWorkValue(core);

	cap = FMap_ArticleActGet(fmap, actwork_no);
	GF_ASSERT(cap != NULL);

	if( flag == 0 ){
		CATS_ObjectObjModeSet(cap->act, GX_OAM_MODE_NORMAL);		//ƒm[ƒ}ƒ‹
	}else{
		CATS_ObjectObjModeSet(cap->act, GX_OAM_MODE_XLU);			//”¼“§–¾
	}
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief   ”z’u•¨ƒAƒNƒ^[:ƒAƒjƒŠJn
 *
 * @param   core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @retval  "0"
 */
//--------------------------------------------------------------
static BOOL FSS_ArticleActorAnimeStart( FSS_TASK * core )
{
	u16 actwork_no	= FSSTGetWorkValue(core);
	u16 anm_seqno = FSSTGetWorkValue(core);
	CATS_ACT_PTR cap;
	FMAP_PTR fmap = Frontier_FMapAdrsGet(core->fss->fmain);
	
	cap = FMap_ArticleActGet(fmap, actwork_no);
	CATS_ObjectAnimeSeqSetCap(cap, anm_seqno);
	FMap_ArticleActAnmBitSet(fmap, actwork_no, ON);
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief   ”z’u•¨ƒAƒNƒ^[:ƒAƒjƒŠJn
 *
 * @param   core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @retval  "0"
 */
//--------------------------------------------------------------
static BOOL FSS_ArticleActorAnimeStop( FSS_TASK * core )
{
	u16 actwork_no	= FSSTGetWorkValue(core);
	FMAP_PTR fmap = Frontier_FMapAdrsGet(core->fss->fmain);

	FMap_ArticleActAnmBitSet(fmap, actwork_no, OFF);
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief   ”z’u•¨ƒAƒNƒ^[:ƒAƒjƒI—¹‘Ò‚¿
 *
 * @param   core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @retval  "1"
 */
//--------------------------------------------------------------
static BOOL FSS_ArticleActorAnimeWait( FSS_TASK * core )
{
	u16 actwork_no	= FSSTGetWorkValue(core);

	//‰¼‘zƒ}ƒVƒ“‚Ì”Ä—pƒŒƒWƒXƒ^‚Éƒ[ƒN‚ÌID‚ğŠi”[
	core->reg[0] = actwork_no;

	FSST_SetWait( core, WaitArticleActorAnime );
	return 1;
}

//return 1 = I—¹
static BOOL WaitArticleActorAnime(FSS_TASK *core)
{
	CATS_ACT_PTR cap;
	FMAP_PTR fmap = Frontier_FMapAdrsGet(core->fss->fmain);
	
	cap = FMap_ArticleActGet(fmap, core->reg[0]);
	if(FMap_ArticleActAnmBitGet(fmap, core->reg[0]) == 0
			|| CATS_ObjectAnimeActiveCheckCap(cap) == FALSE){
		return 1;
	}
	return 0;
}

//==============================================================================
//
//	OBJ“®ìƒR[ƒhİ’è
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   OBJ“®ìƒR[ƒhİ’èF‹¤’Êˆ—
 *
 * @param   core			
 * @param   code			OBJ“®ìƒR[ƒh
 * @param   work_num		work‚É“n‚·—v‘f‚Ì”
 */
//--------------------------------------------------------------
static void FSSC_ObjMoveCode_Core(FSS_TASK *core, int code, int work_num)
{
	FSS_PTR fss = core->fss;
	s16 *work;
	u16 playid;
	int i;
	
	GF_ASSERT(work_num <= FSS_OBJMOVE_WORK_MAX);
	
	work = sys_AllocMemory(fss->heap_id, sizeof(s16) * work_num);
	playid = FSSTGetWorkValue(core);
	for(i = 0; i < 3; i++){
		work[i] = FSSTGetWorkValue(core);
	}
	FSS_ObjMoveFuncIDSet(fss->fmain, playid, OBJMOVE_KYORO, work, 3);
	sys_FreeMemoryEz(work);
}

//--------------------------------------------------------------
/**
 * @brief   OBJ“®ìƒR[ƒhİ’èFƒŠƒZƒbƒg‚·‚é(OBJ“®ì‚È‚µ‚É‚·‚é)
 * @param   core		
 * @return	"0"
 */
//--------------------------------------------------------------
static BOOL FSSC_ObjMoveCode_Reset(FSS_TASK *core)
{
	u16 playid;
	
	playid = FSSTGetWorkValue(core);
	FSS_ObjMoveFuncIDSet(core->fss->fmain, playid, OBJMOVE_ID_NULL, NULL, 0);

	return 0;
}

//--------------------------------------------------------------
/**
 * @brief   OBJ“®ìƒR[ƒhİ’èF‚«‚å‚ë‚«‚å‚ë
 * @param   core		
 * @return	"0"
 */
//--------------------------------------------------------------
static BOOL FSSC_ObjMoveCode_Kyoro(FSS_TASK *core)
{
	FSSC_ObjMoveCode_Core(core, OBJMOVE_KYORO, 3);
	return 0;
}



//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   –¼‘O“ü—Í‰æ–ÊŒÄ‚Ño‚µ	¦ƒTƒuPROCŒÄ‚Ño‚µ‚ÌƒeƒXƒg
 *
 * @param   core		
 *
 * @retval  
 */
//--------------------------------------------------------------
#include "application/namein.h"
static void NameInFreeMemory(void *parent_work);

static BOOL FSSC_NameInProc(FSS_TASK *core)
{
	void *namein_param;
	FRONTIER_EX_PARAM *ex_param;
	
	ex_param = Frontier_ExParamGet(core->fss->fmain);
	namein_param = NameIn_ParamAllocMake(
		HEAPID_WORLD, NAMEIN_MYNAME, 0, 8, (void*)ex_param->config);
    
    Frontier_SubProcSet(core->fss->fmain, &NameInProcData, namein_param, FALSE, NameInFreeMemory);

	return 1;
}

///–¼‘O“ü—Í‰æ–Ê‚Å“n‚µ‚½ƒ[ƒN‚Ì‰ğ•úˆ—
static void NameInFreeMemory(void *parent_work)
{
	NameIn_ParamDelete(parent_work);
}

//==============================================================================
//	í“¬˜^‰æ
//==============================================================================
//--------------------------------------------------------------------------------------------
/**
 * ƒf[ƒ^‰Šú‰»
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_BattleRecInit( FSS_TASK * core )
{
	LOAD_RESULT ret;
	FRONTIER_EX_PARAM *ex_param = Frontier_ExParamGet(core->fss->fmain);

	//‘Îí˜^‰æƒf[ƒ^‚Ì‰Šú‰»
	BattleRec_Init( ex_param->savedata, HEAPID_WORLD, &ret );
#if 0
	LOAD_RESULT_NULL = 0,		///<ƒf[ƒ^‚È‚µ
	LOAD_RESULT_OK,				///<ƒf[ƒ^³í“Ç‚İ‚İ
	LOAD_RESULT_NG,				///<ƒf[ƒ^ˆÙí
	LOAD_RESULT_BREAK,			///<”j‰óA•œ‹Œ•s”\ 
#endif

	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒZ[ƒu
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"1"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_BattleRecSave( FSS_TASK * core )
{
	FSS_PTR fss = core->fss;
	u16 fr_no	= FSSTGetU16(core);
	u16 type	= FSSTGetWorkValue(core);		//ƒoƒgƒ‹ƒ^ƒCƒv
	u16 rensyou	= FSSTGetWorkValue(core);		//Œ»İ‚Ì˜AŸ”
	u16* ret_wk	= FSSTGetWork(core);			//Œ‹‰Êæ“¾

	OS_Printf( "{İƒiƒ“ƒo[ = %d\n", fr_no );
	OS_Printf( "ƒoƒgƒ‹ƒ^ƒCƒv = %d\n", type );
	OS_Printf( "‰½í–Ú‚©(˜AŸ”‚©‚çæ“¾) = %d\n", rensyou );

#if 0
	SAVE_RESULT_CONTINUE = 0,		///<ƒZ[ƒuˆ—Œp‘±’†
	SAVE_RESULT_LAST,				///<ƒZ[ƒuˆ—Œp‘±’†AÅŒã‚Ìˆê‚Â‘O
	SAVE_RESULT_OK,					///<ƒZ[ƒu³íI—¹
	SAVE_RESULT_NG,					///<ƒZ[ƒu¸”sI—¹
#endif

	//9999‚ğ‰z‚³‚È‚¢‚æ‚¤‚É§ŒÀ
	if( rensyou > REC_COUNTER_MAX ){
		rensyou = REC_COUNTER_MAX;
	}

	fss->save_seq0 = 0;
	fss->save_seq1 = 0;
	fss->save_rec_mode = Frontier_GetRecModeNo(fr_no,type);
	fss->save_rensyou = rensyou;
	fss->save_ret_wk = ret_wk;
	FSST_SetWait( core, BattleRecSaveWait );

	return 1;
}

static BOOL BattleRecSaveWait(FSS_TASK * core)
{
	FSS_PTR fss = core->fss;
	FRONTIER_EX_PARAM *ex_param = Frontier_ExParamGet(core->fss->fmain);
	SAVE_RESULT ret;
	
	//‘Îí˜^‰æƒf[ƒ^‚ÌƒZ[ƒu
	ret = BattleRec_Save(ex_param->savedata, fss->save_rec_mode, fss->save_rensyou, 
		LOADDATA_MYREC, &fss->save_seq0, &fss->save_seq1);
	OS_Printf( "rec save = %d\n", ret );
	if(ret == SAVE_RESULT_OK || ret == SAVE_RESULT_NG){
		if(ret == SAVE_RESULT_OK){
			*(fss->save_ret_wk) = 1;	//¬Œ÷
		}
		else{
			*(fss->save_ret_wk) = 0;	//¸”s
		}
		return 1;
	}
	return 0;
}

//‹L˜^ˆ—‚Ì{İNo‚ğæ“¾(‚±‚Ì’è‹`‚Í{İ‚Æƒoƒgƒ‹Œ`®‚ª‚í‚©‚é’è‹`)
static int Frontier_GetRecModeNo( u16 fr_no, u16 type )
{
	int no;

	//•ÛŒ¯
	no = RECMODE_TOWER_SINGLE;

	switch( fr_no ){

	////////////////////////////////////////////////////
	//ƒoƒgƒ‹ƒ^ƒ[	
	case FRONTIER_NO_TOWER:

		switch( type ){
		case BTWR_MODE_SINGLE:
			no = RECMODE_TOWER_SINGLE;
			break;
		case BTWR_MODE_DOUBLE:
			no = RECMODE_TOWER_DOUBLE;
			break;
		case BTWR_MODE_MULTI:					//AIƒ}ƒ‹ƒ`
		case BTWR_MODE_COMM_MULTI:				//’ÊMƒ}ƒ‹ƒ`
		case BTWR_MODE_WIFI_MULTI:				//wifi
			no = RECMODE_TOWER_MULTI;
			break;
		case BTWR_MODE_WIFI:					//wifiƒ_ƒEƒ“ƒ[ƒh‚ÌƒVƒ“ƒOƒ‹
		case BTWR_MODE_RETRY:					//wifiƒ_ƒEƒ“ƒ[ƒh‚ÌƒVƒ“ƒOƒ‹‚ÌƒŠƒgƒ‰ƒC
			no = RECMODE_TOWER_WIFI_DL;
			break;
		default:
			GF_ASSERT(0);
			break;
		};
		break;

	////////////////////////////////////////////////////
	//ƒtƒ@ƒNƒgƒŠ[50
	case FRONTIER_NO_FACTORY_LV50:

		switch( type ){
		case FACTORY_TYPE_SINGLE:
			no = RECMODE_FACTORY_SINGLE;
			break;
		case FACTORY_TYPE_DOUBLE:
			no = RECMODE_FACTORY_DOUBLE;
			break;
		case FACTORY_TYPE_MULTI:
		case FACTORY_TYPE_WIFI_MULTI:
			no = RECMODE_FACTORY_MULTI;
			break;
		default:
			GF_ASSERT(0);
			break;
		};
		break;

	////////////////////////////////////////////////////
	//ƒtƒ@ƒNƒgƒŠ[100
	case FRONTIER_NO_FACTORY_LV100:

		switch( type ){
		case FACTORY_TYPE_SINGLE:
			no = RECMODE_FACTORY_SINGLE100;
			break;
		case FACTORY_TYPE_DOUBLE:
			no = RECMODE_FACTORY_DOUBLE100;
			break;
		case FACTORY_TYPE_MULTI:
		case FACTORY_TYPE_WIFI_MULTI:
			no = RECMODE_FACTORY_MULTI100;
			break;
		default:
			GF_ASSERT(0);
			break;
		};
		break;

	////////////////////////////////////////////////////
	//ƒLƒƒƒbƒXƒ‹
	case FRONTIER_NO_CASTLE:

		switch( type ){
		case CASTLE_TYPE_SINGLE:
			no = RECMODE_CASTLE_SINGLE;
			break;
		case CASTLE_TYPE_DOUBLE:
			no = RECMODE_CASTLE_DOUBLE;
			break;
		case CASTLE_TYPE_MULTI:
		case CASTLE_TYPE_WIFI_MULTI:
			no = RECMODE_CASTLE_MULTI;
			break;
		default:
			GF_ASSERT(0);
			break;
		};
		break;

	////////////////////////////////////////////////////
	//ƒXƒe[ƒW
	case FRONTIER_NO_STAGE:

		switch( type ){
		case STAGE_TYPE_SINGLE:
			no = RECMODE_STAGE_SINGLE;
			break;
		case STAGE_TYPE_DOUBLE:
			no = RECMODE_STAGE_DOUBLE;
			break;
		case STAGE_TYPE_MULTI:
		case STAGE_TYPE_WIFI_MULTI:
			no = RECMODE_STAGE_MULTI;
			break;
		default:
			GF_ASSERT(0);
			break;
		};
		break;

	////////////////////////////////////////////////////
	//ƒ‹[ƒŒƒbƒg
	case FRONTIER_NO_ROULETTE:

		switch( type ){
		case ROULETTE_TYPE_SINGLE:
			no = RECMODE_ROULETTE_SINGLE;
			break;
		case ROULETTE_TYPE_DOUBLE:
			no = RECMODE_ROULETTE_DOUBLE;
			break;
		case ROULETTE_TYPE_MULTI:
		case ROULETTE_TYPE_WIFI_MULTI:
			no = RECMODE_ROULETTE_MULTI;
			break;
		default:
			GF_ASSERT(0);
			break;
		};
		break;

	};

#if 0

	//“Á‚Éì‹Æ‚µ‚Ä‚¢‚È‚¢‚Ì‚Å•Û—¯
#define RECMODE_COLOSSEUM_SINGLE		(12)		///<ƒRƒƒVƒAƒ€(’ÊM‘Îí) / ƒVƒ“ƒOƒ‹
#define RECMODE_COLOSSEUM_DOUBLE		(13)		///<ƒRƒƒVƒAƒ€(’ÊM‘Îí) / ƒ_ƒuƒ‹
#define RECMODE_COLOSSEUM_MIX			(14)		///<ƒRƒƒVƒAƒ€(’ÊM‘Îí) / ƒ~ƒbƒNƒX
#define RECMODE_COLOSSEUM_MULTI		(21)			///<ƒRƒƒVƒAƒ€(’ÊM‘Îí) / ƒ}ƒ‹ƒ`

#endif

	return no;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒ[ƒh
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_BattleRecLoad( FSS_TASK * core )
{
	LOAD_RESULT ret;
	BATTLE_PARAM* bp;
	FRONTIER_EX_PARAM *ex_param = Frontier_ExParamGet(core->fss->fmain);

	bp = sys_AllocMemory( HEAPID_WORLD, sizeof(BATTLE_PARAM) );
	MI_CpuClear8( bp, sizeof(BATTLE_PARAM) );

	//bp	ƒ[ƒh‚µ‚½ƒf[ƒ^‚©‚ç¶¬‚·‚éBATTLE_PARAM\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
	//num	ƒ[ƒh‚·‚éƒf[ƒ^ƒiƒ“ƒo[iLOADDATA_MYRECALOADDATA_DOWNLOAD1ALOADDATA_DOWNLOAD2cj

	//‘Îí˜^‰æƒf[ƒ^‚Ìƒ[ƒh
	BattleRec_Load( ex_param->savedata, HEAPID_WORLD, &ret, bp, LOADDATA_MYREC );
	OS_Printf( "battle_load ret = %d\n", ret );
#if 0
	LOAD_RESULT_NULL = 0,		///<ƒf[ƒ^‚È‚µ
	LOAD_RESULT_OK,				///<ƒf[ƒ^³í“Ç‚İ‚İ
	LOAD_RESULT_NG,				///<ƒf[ƒ^ˆÙí
	LOAD_RESULT_BREAK,			///<”j‰óA•œ‹Œ•s”\ 
#endif

#if 1	//ƒfƒoƒbƒN(í“¬Ø‚è‘Ö‚¦)
	Snd_DataSetByScene( SND_SCENE_BATTLE, SEQ_BA_TRAIN, 1 );	//ƒoƒgƒ‹‹ÈÄ¶
    Frontier_SubProcSet( core->fss->fmain, &TestBattleProcData, bp, TRUE, NULL );
	OS_Printf( "˜^‰æí“¬ŒÄ‚Ño‚µƒeƒXƒg\n" );
	return 1;
#endif

	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒf[ƒ^”jŠü
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_BattleRecExit( FSS_TASK * core )
{
	BattleRec_Exit();
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ˜^‰æƒf[ƒ^‚ª‚·‚Å‚É‚ ‚é‚©ƒ`ƒFƒbƒN
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_BattleRecDataOccCheck( FSS_TASK * core )
{
	LOAD_RESULT ret;
	FRONTIER_EX_PARAM *ex_param = Frontier_ExParamGet(core->fss->fmain);
	u16* ret_wk	= FSSTGetWork( core );

	*ret_wk = BattleRec_DataOccCheck( ex_param->savedata, HEAPID_WORLD, &ret, LOADDATA_MYREC );
	OS_Printf( "battle_load ret = %d\n", ret );
	return 0;
}


//============================================================================================
//	ƒŒƒ|[ƒgƒZ[ƒu
//============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * ƒŒƒ|[ƒg
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_ReportSave( FSS_TASK * core )
{
	//‰¼‚Å‚·I

	FRONTIER_EX_PARAM *ex_param = Frontier_ExParamGet(core->fss->fmain);
	u16* ret_wk	= FSSTGetWork( core );

	if (SaveData_Save(ex_param->savedata) == SAVE_RESULT_OK) {
		OS_Printf( "save TRUE\n" );
	} else {
		OS_Printf( "save FALSE\n" );
		//SAVE_RESULT_OKˆÈŠO‚Ìê‡‚Í‚È‚ñ‚É‚¹‚æ
		//“®ì‚ª‚¨‚©‚µ‚¢‚Ì‚Å¸”s‚Æ‚İ‚È‚·
	}

	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒŒƒ|[ƒg(•ªŠ„ƒZ[ƒu)
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"1"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_ReportDivSave( FSS_TASK * core )
{
	FSS_PTR fss = core->fss;
	FRONTIER_EX_PARAM *ex_param = Frontier_ExParamGet(core->fss->fmain);
	u16* ret_wk					= FSSTGetWork( core );

	sys_SoftResetNG(SOFTRESET_TYPE_SAVELOAD);
	sys_SioErrorNG_PtrSet(fss->heap_id);
	
	SaveData_DivSave_Init(ex_param->savedata, SVBLK_ID_MAX);
	FSST_SetWait( core, EvWaitDivSave );
	return 1;
}

//return 1 = I—¹
static BOOL EvWaitDivSave(FSS_TASK * core)
{
	SAVE_RESULT result;
	FRONTIER_EX_PARAM *ex_param = Frontier_ExParamGet(core->fss->fmain);

	result = SaveData_DivSave_Main(ex_param->savedata);

	//ƒZ[ƒuI—¹A¬Œ÷
	if( result == SAVE_RESULT_OK ){
		sys_SioErrorNG_PtrFree();
		sys_SoftResetOK(SOFTRESET_TYPE_SAVELOAD);
		return 1;
	}

	//ƒZ[ƒuI—¹A¸”s
	if( result == SAVE_RESULT_NG ){
		OS_Printf( "ƒZ[ƒu¸”s\n" );
		sys_SioErrorNG_PtrFree();
		sys_SoftResetOK(SOFTRESET_TYPE_SAVELOAD);
		return 1;
	}

	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ŠO•”ƒZ[ƒu‚Ì‰Šú‰»
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_ExtraSaveInit( FSS_TASK * core )
{
	FRONTIER_EX_PARAM *ex_param = Frontier_ExParamGet(core->fss->fmain);
	SVDT_ExtraInit( ex_param->savedata );
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ŠO•”ƒZ[ƒu‚ª‰Šú‰»Ï‚İ‚©ƒ`ƒFƒbƒN
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_ExtraSaveInitCheck( FSS_TASK * core )
{
	FRONTIER_EX_PARAM *ex_param = Frontier_ExParamGet(core->fss->fmain);
	u16* ret_wk	= FSSTGetWork( core );
	*ret_wk = SaveData_GetExtraInitFlag( ex_param->savedata );
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	‘Ò‹@ƒAƒCƒRƒ“•\¦
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 * @return	0
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_TimeWaitIconAdd( FSS_TASK * core)
{
	FSS_PTR fss = core->fss;
	//fss->waiticon = TimeWaitIconAdd(fss->bmpwin_talk, TALK_WIN_CGX_NUM);
	fss->waiticon = TimeWaitIconAdd(&fss->bmpwin_talk, FR_TALK_WIN_CGX_NUM);
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	‘Ò‹@ƒAƒCƒRƒ“Á‹
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 * @return	0
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_TimeWaitIconDel( FSS_TASK * core)
{
	FSS_PTR fss = core->fss;
	TimeWaitIconDel( fss->waiticon );
	return 0;
}

//============================================================================================
//	ƒ[ƒhƒZƒbƒg
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * ƒAƒCƒeƒ€–¼‚ğw’èƒoƒbƒtƒ@‚É“o˜^
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_ItemName( FSS_TASK * core ) 
{
	u8 idx = FSSTGetU8(core);
	u16 itemno = FSSTGetWorkValue(core);

	WORDSET_RegisterItemName( core->fss->wordset, idx, itemno );
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ”’l‚ğw’èƒoƒbƒtƒ@‚É“o˜^
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_NumberName( FSS_TASK * core ) 
{
	u8 idx = FSSTGetU8(core);
	u16 number = FSSTGetWorkValue(core);

	WORDSET_RegisterNumber( core->fss->wordset, idx, number, GetNumKeta(number), 
							NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT );
	return 0;
}

//--------------------------------------------------------------
/**
 * “n‚³‚ê‚½’l‚ÌŒ…”‚ğæ“¾
 *
 * @param   num			’l
 *
 * @retval  "Œ…”"
 */
//--------------------------------------------------------------
static u16 GetNumKeta(u32 num)
{
	if( num / 10 == 0 )	return 1;
	else if( num / 100 == 0 ) return 2;
	else if( num / 1000 == 0 ) return 3;
	else if( num / 10000 == 0 ) return 4;
	else if( num / 100000 == 0 ) return 5;
	else if( num / 1000000 == 0 ) return 6;
	else if( num / 10000000 == 0 ) return 7;
	else if( num / 100000000 == 0 ) return 8;

	return 1;	//“–‚Ä‚Í‚Ü‚ç‚È‚©‚Á‚½‚ç‚PŒ…‚ğ•Ô‚·
}

//--------------------------------------------------------------------------------------------
/**
 * ƒvƒŒƒCƒ„[–¼‚ğw’èƒoƒbƒtƒ@‚É“o˜^
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_PlayerName( FSS_TASK * core ) 
{
	FRONTIER_EX_PARAM *ex_param = Frontier_ExParamGet(core->fss->fmain);
	u8 idx = FSSTGetU8(core);
	WORDSET_RegisterPlayerName( core->fss->wordset, idx, 
								SaveData_GetMyStatus(ex_param->savedata) );
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒp[ƒgƒi[–¼‚ğw’èƒoƒbƒtƒ@‚É“o˜^
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_PairName( FSS_TASK * core )
{
	FRONTIER_EX_PARAM *ex_param = Frontier_ExParamGet(core->fss->fmain);
	u8 idx	= FSSTGetU8(core);
	WORDSET_RegisterPlayerName( core->fss->wordset, idx, 
								CommInfoGetMyStatus( (CommGetCurrentID() ^ 1) ) );
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 *@‹Z–¼‚ğƒoƒbƒtƒ@‚Ö
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
BOOL FSSC_WazaName( FSS_TASK * core )
{
	u8 idx		= FSSTGetU8(core);
	u16 wazano	= FSSTGetWorkValue(core);
	WORDSET_RegisterWazaName( core->fss->wordset, idx, wazano );
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒ‚ƒ“ƒXƒ^[ƒiƒ“ƒo[‚©‚çƒ|ƒPƒ‚ƒ“–¼‚ğw’èƒoƒbƒtƒ@‚É“o˜^
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
BOOL FSSC_PokemonNameExtra( FSS_TASK * core )
{
	u8 idx		= FSSTGetU8(core);
	u16 mons	= FSSTGetWorkValue(core);
	u16 sex		= FSSTGetU16(core);
	u8 flag		= FSSTGetU8(core);
	STRBUF* buf	= PokeNameGetAlloc( mons, HEAPID_WORLD );

	//’PŒêƒZƒbƒg
	//ƒoƒbƒtƒ@ID
	//•¶š—ñ
	//«•ÊƒR[ƒh
	//’P^•¡iTRUE‚Å’P”j
	//Œ¾ŒêƒR[ƒh
	WORDSET_RegisterWord( core->fss->wordset, idx, buf, sex, flag, PM_LANG );
	STRBUF_Delete( buf );
	return 0;
}

static STRBUF * PokeNameGetAlloc( u16 id, u32 heap )
{
	MSGDATA_MANAGER* man;
	STRBUF* str;
	
	man = MSGMAN_Create( MSGMAN_TYPE_DIRECT, ARC_MSG, NARC_msg_monsname_dat, heap );
	str = MSGMAN_AllocString( man, id );

	MSGMAN_Delete( man );
	return str;
}

//--------------------------------------------------------------------------------------------
/**
 *@‹Z–¼‚ğƒoƒbƒtƒ@‚Ö
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_TypeName( FSS_TASK * core )
{
	u8 idx		= FSSTGetU8(core);
	u16 typeID	= FSSTGetWorkValue(core);
	WORDSET_RegisterPokeTypeName( core->fss->wordset, idx, typeID );
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒ‰ƒCƒoƒ‹–¼‚ğw’èƒoƒbƒtƒ@‚É“o˜^
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_RivalName( FSS_TASK * core ) 
{
	FRONTIER_EX_PARAM *ex_param = Frontier_ExParamGet(core->fss->fmain);
	u8 idx = FSSTGetU8(core);
	WORDSET_RegisterRivalName( core->fss->wordset, idx, ex_param->savedata );
	return 0;
}

//============================================================================================
//	ƒoƒgƒ‹ƒ|ƒCƒ“ƒg
//============================================================================================
#include "savedata/b_tower.h"
//--------------------------------------------------------------------------------------------
/**
 * Šƒoƒgƒ‹ƒ|ƒCƒ“ƒg‚ğæ“¾
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_CheckBtlPoint( FSS_TASK * core )
{
	FRONTIER_EX_PARAM *ex_param = Frontier_ExParamGet(core->fss->fmain);
	u16* ret_wk	= FSSTGetWork( core );
	
	*ret_wk = TowerScoreData_SetBattlePoint(
				SaveData_GetTowerScoreData(ex_param->savedata),0,BTWR_DATA_get);
	return	0;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒoƒgƒ‹ƒ|ƒCƒ“ƒg‚ğ‰Á‚¦‚é
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_AddBtlPoint( FSS_TASK * core )
{
	TV_WORK * tvwk;
	FRONTIER_EX_PARAM *ex_param = Frontier_ExParamGet(core->fss->fmain);
	u16 value					= FSSTGetWorkValue(core);

	//šfssc_tower.c TowerScr_AddBtlPoint‚àƒ`ƒFƒbƒN‚·‚é

	//TV:ƒoƒgƒ‹ƒ|ƒCƒ“ƒg–¼l
	tvwk = SaveData_GetTvWork( ex_param->savedata );
	TVTOPIC_BPTemp_Set( tvwk, value );
	//TVTOPIC_Entry_Record_BP( ex_param->savedata );

	//ƒŒƒR[ƒhXV
	RECORD_Add( SaveData_GetRecord(ex_param->savedata), RECID_WIN_BP, value );

	TowerScoreData_SetBattlePoint(
			SaveData_GetTowerScoreData(ex_param->savedata),value,BTWR_DATA_add);

	return 0;
};

//--------------------------------------------------------------------------------------------
/**
 * ƒoƒgƒ‹ƒ|ƒCƒ“ƒg‚ğˆø‚­
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_SubBtlPoint( FSS_TASK * core )
{
	FRONTIER_EX_PARAM *ex_param = Frontier_ExParamGet(core->fss->fmain);
	u16 value = FSSTGetWorkValue(core);

	//ƒŒƒR[ƒhXV
	RECORD_Add( SaveData_GetRecord(ex_param->savedata), RECID_USE_BP, value );

	TowerScoreData_SetBattlePoint(
			SaveData_GetTowerScoreData(ex_param->savedata),value,BTWR_DATA_sub);

	return 0;
};

//============================================================================================
//
//	’ÊMŠÖ˜A
//
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * ƒ^ƒCƒ~ƒ“ƒOƒRƒ}ƒ“ƒh‚ğ”­s‚·‚é
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_CommTimingSyncStart( FSS_TASK * core )
{
	u16 no = FSSTGetWorkValue( core );

	//‰¼‘zƒ}ƒVƒ“‚Ì”Ä—pƒŒƒWƒXƒ^‚Éƒ[ƒN‚ÌID‚ğŠi”[
	core->reg[0] = no;

	CommTimingSyncStart( no );

	FSST_SetWait( core, EvWaitCommIsTimingSync );

	OS_Printf("“¯Šú‘Ò‚¿@”Ô†%d\n",no);

	return 1;
}

//return 1 = I—¹
static BOOL EvWaitCommIsTimingSync(FSS_TASK * core)
{
	int result;
	if(CommGetConnectNum()<2){
		result = 1;
	}else{
		result = CommIsTimingSync( core->reg[0] );
	}
	return result;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒŠƒZƒbƒgƒRƒ}ƒ“ƒh‚ğ”­s‚·‚é
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_CommTempDataReset( FSS_TASK * core )
{
	CommToolTempDataReset();
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ’ÊMƒ_ƒCƒŒƒNƒgƒR[ƒi[‚ÌI—¹ˆ—‚ğŠJn‚·‚é
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"1"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_CommDirectEnd( FSS_TASK * core )
{
    CommFieldStateExitBattle();
	FSST_SetWait( core, EvWaitCommDirectEnd );
	return 1;
}

//return 1 = I—¹
static BOOL EvWaitCommDirectEnd(FSS_TASK * core)
{
	if( CommStateIsInitialize() != TRUE ){
        if(CommMPIsInitialize() != TRUE ){
            return 1;
        }
	}
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * —”‚Ìæ“¾
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"1"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_GetRand( FSS_TASK * core )
{
	u16* ret_wk	= FSSTGetWork( core );
	u16	limit	= FSSTGetWorkValue( core );

	*ret_wk = ( gf_rand() % limit );
	//OS_Printf("GET_RND[%d] limit[%d]\n",*ret_wk,limit);
	return 1;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒ|ƒPƒZƒ“‰ñ•œˆ—
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_PcKaifuku( FSS_TASK * core )
{
	FRONTIER_EX_PARAM *ex_param;
	ex_param = Frontier_ExParamGet(core->fss->fmain);
	PokeParty_RecoverAll( SaveData_GetTemotiPokemon(ex_param->savedata) );
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒL[ƒEƒFƒCƒg
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	í‚É1
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_ABKeyWait( FSS_TASK * core )
{
	FSST_SetWait( core, EvWaitABKey );
	return 1;
}

//return 1 = I—¹
static BOOL EvWaitABKey(FSS_TASK * core)
{
	if( sys.trg & ( PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL ) ){
		return 1;
	}

	return 0;
}
//--------------------------------------------------------------------------------------------
/**
 * ƒL[ƒEƒFƒCƒg or ŠÔ‘Ò‚¿
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	í‚É1
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_ABKeyTimeWait( FSS_TASK * core )
{
	core->reg[0] = FSSTGetWorkValue(core);
	FSST_SetWait( core, EvWaitABKeyTime );
	return 1;
}

//ƒL[orŠÔ‘Ò‚¿
static BOOL EvWaitABKeyTime(FSS_TASK * core)
{
	//if (sys.trg & (PAD_BUTTON_A | PAD_BUTTON_B)) {
	if (sys.trg & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL)) {
		return 1;
	}

	core->reg[0] --;
	if (core->reg[0] == 0) {
		return 1;
	}
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒgƒŒ[ƒi[”s–kƒ`ƒFƒbƒN
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
BOOL FSSC_TrainerLoseCheck( FSS_TASK * core )
{
	u16* ret_wk		= FSSTGetWork( core );
#if 0
	BOOL* win_flag	= GetEvScriptWorkMemberAdrs( core->fsys, ID_EVSCR_WIN_FLAG );

	*ret_wk = BattleParam_IsWinResult(*win_flag);
#endif
	return 1;
}

//--------------------------------------------------------------
/**
 * @brief	ƒXƒNƒŠƒvƒgƒRƒ}ƒ“ƒhFƒZ[ƒuƒ[ƒN‚Ö’l‚ğƒZƒbƒg
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------
BOOL FSSC_SaveEventWorkSet( FSS_TASK * core )
{
	u16* p_work;
	u16 work_no	= FSSTGetU16( core );
	//u16 param1= FSSTGetU16( core );
	u16 param1	= FSSTGetWorkValue( core );

	FRONTIER_EX_PARAM* ex_param = Frontier_ExParamGet( core->fss->fmain );
	p_work = EventWork_GetEventWorkAdrs( SaveData_GetEventWork(ex_param->savedata), work_no );
	*p_work = param1;
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	ƒXƒNƒŠƒvƒgƒRƒ}ƒ“ƒhFƒZ[ƒuƒ[ƒN‚Ì’l‚ğæ“¾
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------
BOOL FSSC_SaveEventWorkGet( FSS_TASK * core )
{
	u16* p_work;
	u16 work_no	= FSSTGetU16( core );
	u16* work	= FSSTGetWork( core );

	FRONTIER_EX_PARAM* ex_param = Frontier_ExParamGet( core->fss->fmain );
	p_work = EventWork_GetEventWorkAdrs( SaveData_GetEventWork(ex_param->savedata), work_no );
	*work = *p_work;
	return 0;
}

#define HC_LASTER_VOFS	( 2 )
#define HC_LASTER_MAX	( 192 / HC_LASTER_VOFS )

enum{
	LASTER_MODE_004 = 1,
	LASTER_MODE_005,
};

typedef struct {
	
	s16 start;
	s16 end;
	
	s16	speed_x;
	s16	speed_y;
	s16 ofs_x;
	s16 ofs_y;
	int init;
	
} HC_LASTER_DATA;

typedef struct {
	
	HC_LASTER_DATA lst[ HC_LASTER_MAX ];

	FEFTOOL_DEFLASTER_PTR	dlp;
	
	int laster_mode;	///< LASTER_MODE_???
} HC_LASTER_SYS;

// \‘¢‘Ì(”Ä—p)
typedef struct {
	
	FMAP_PTR fmap;
	
	int		seq;
	int		ece_type;
	
	BOOL	state;
	
	BOOL	flag;
	
	void*	temp;
	
	FENCOUNT_LASTER_SCROLL	laster;
	HC_LASTER_SYS*			hls;
	
	GF_BGL_BMPWIN* p_bmp;
	FENC_BMP_FILL_BLOCK_MOVE* p_block;		
} FSS_ECE;

static BOOL EncountEffect_001(FSS_ECE* wk);
static BOOL EncountEffect_002(FSS_ECE* wk);
static BOOL EncountEffect_003(FSS_ECE* wk);
static BOOL EncountEffect_004(FSS_ECE* wk);
static BOOL EncountEffect_005(FSS_ECE* wk);
static BOOL EncountEffect_006(FSS_ECE* wk);

//¦FR_ENCOUNT_EFF_???‚Ì’è‹`‚Æ•À‚Ñ‚ğ“¯‚¶‚É‚µ‚Ä‚¨‚­‚±‚ÆI
static BOOL ( * const EncountEffect_Table[] )( FSS_ECE* wk ) = {
	EncountEffect_001,		///< ƒƒCƒv
	EncountEffect_002,		///< ƒ{ƒbƒNƒX‰º‚©‚çÜ‚è•Ô‚µ‚ÄÏ‚Ş
	EncountEffect_003,		///< ƒ{ƒbƒNƒX¶‰E‚©‚çÜ‚è•Ô‚µ
	EncountEffect_004,		///< ƒ‰ƒXƒ^[Ü
	EncountEffect_005,		///< ƒ‰ƒXƒ^[ƒWƒOƒUƒO
	EncountEffect_006,		///< ÀŒ±
};

///< ƒƒCƒvƒ[ƒ‹Œn
static BOOL EncountEffect_001(FSS_ECE* wk)
{
	switch ( wk->seq ){
	case 0:
		FENC_EncountFlashTask(MASK_MAIN_DISPLAY, 16, -16,  &wk->state, 2);
		wk->seq++;
		break;
	
	case 1:
		if ( wk->state == FALSE ){ break; }
		
		WIPE_ResetWndMask( WIPE_DISP_MAIN );
		WIPE_ResetWndMask( WIPE_DISP_SUB );
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_SCREWOUT, WIPE_TYPE_SCREWOUT, WIPE_FADE_BLACK, 12, 1, HEAPID_WORLD );
		wk->seq++;
		break;
			
	default:
		if( WIPE_SYS_EndCheck() == TRUE ){
			return FALSE;
		}
		break;
	}	
	
	return TRUE;
}

/// ƒuƒƒbƒNÏ‚İã‚°Œn
static BOOL EncountEffect_002(FSS_ECE* wk)
{
	switch ( wk->seq ){
	
	case 0:
		FENC_EncountFlashTask(MASK_MAIN_DISPLAY, 16, -16,  &wk->state, 2);
		wk->seq++;
		break;
	
	case 1:
		if ( wk->state == FALSE ){ break; }
		{
			wk->p_bmp = GF_BGL_BmpWinAllocGet( HEAPID_WORLD, 1 );
			GF_BGL_BmpWinAdd( wk->fmap->bgl, wk->p_bmp, ENCOUNT_EFF_FRAME, 0, 0, 32, 32, 0, 0 );
		}
		{
			PaletteWork_Clear(wk->fmap->pfd, FADE_MAIN_BG, FADEBUF_ALL, 0x0000, 0, 16);
		}
		GF_BGL_BmpWinDataFill( wk->p_bmp, 0 );
		GF_BGL_BmpWinOnVReq( wk->p_bmp );
		wk->p_block = FENC_BMP_FillBlockMoveAlloc( HEAPID_WORLD );
		
		wk->seq++;
		
	case 2:		
		FENC_BMP_FillBlockMoveStart( wk->p_block, 
				ENCOUNT_TR_DAN_HIGH_BLOCK_SYNC,
				ENCOUNT_TR_DAN_HIGH_BLOCK_START_SYNC,
				wk->p_bmp, ENCOUNT_TR_DAN_HIGH_BLOCK_FILLCOLOR );		
		wk->seq++;
		break;
	
	case 3:
		{
			BOOL bState = FENC_BMP_FillBlockMoveMain_LinesUP( wk->p_block );
			GF_BGL_BmpWinOnVReq( wk->p_bmp );
			
			if ( bState ){
				 wk->seq++;
			}
		}
		break;
			
	default:
		if( WIPE_SYS_EndCheck() == TRUE ){
			
			FENC_BMP_FillBlockMoveDelete( wk->p_block );
				
			GF_BGL_BmpWinOff( wk->p_bmp );
			GF_BGL_BmpWinDel( wk->p_bmp );
			GF_BGL_BmpWinFree( wk->p_bmp, 1 );
			
            // MatchComment: add these two calls to WIPE_SetBrightness
            WIPE_SetBrightness( WIPE_DISP_MAIN, WIPE_FADE_BLACK );
            WIPE_SetBrightness( WIPE_DISP_SUB, WIPE_FADE_BLACK );
            
			GF_BGL_ClearCharSet( ENCOUNT_EFF_FRAME, 32, 0, HEAPID_WORLD );
			GF_BGL_ScrClear( wk->fmap->bgl, ENCOUNT_EFF_FRAME );
			
			return FALSE;
		}
		break;
	}	
	
	return TRUE;
}

static BOOL EncountEffect_003(FSS_ECE* wk)
{
	switch ( wk->seq ){
	
	case 0:
		FENC_EncountFlashTask(MASK_MAIN_DISPLAY, 16, -16,  &wk->state, 2);
		wk->seq++;
		break;
	
	case 1:
		if ( wk->state == FALSE ){ break; }
		{
			wk->p_bmp = GF_BGL_BmpWinAllocGet( HEAPID_WORLD, 1 );
			GF_BGL_BmpWinAdd( wk->fmap->bgl, wk->p_bmp, ENCOUNT_EFF_FRAME, 0, 0, 32, 32, 0, 0 );
		}
		{
			PaletteWork_Clear(wk->fmap->pfd, FADE_MAIN_BG, FADEBUF_ALL, 0x0000, 0, 16);

		}
		GF_BGL_BmpWinDataFill( wk->p_bmp, 0 );
		GF_BGL_BmpWinOnVReq( wk->p_bmp );
		wk->p_block = FENC_BMP_FillBlockMoveAlloc( HEAPID_WORLD );
		
		wk->seq++;
		
	case 2:
		FENC_BMP_FillBlockMoveStart( wk->p_block, 
				ENCOUNT_TR_DAN_HIGH_BLOCK_SYNC,
				ENCOUNT_TR_DAN_HIGH_BLOCK_START_SYNC,
				wk->p_bmp, ENCOUNT_TR_DAN_HIGH_BLOCK_FILLCOLOR );		
		wk->seq++;
		break;
	
	case 3:
		{
			BOOL bState = FENC_BMP_FillBlockMoveMain_Place( wk->p_block );
			GF_BGL_BmpWinOnVReq( wk->p_bmp );
			
			if ( bState ){
				 wk->seq++;
			}
		}
		break;
			
	default:
		if( WIPE_SYS_EndCheck() == TRUE ){
			
			FENC_BMP_FillBlockMoveDelete( wk->p_block );
				
			GF_BGL_BmpWinOff( wk->p_bmp );
			GF_BGL_BmpWinDel( wk->p_bmp );
			GF_BGL_BmpWinFree( wk->p_bmp, 1 );
			
            // MatchComment: add these two calls to WIPE_SetBrightness
            WIPE_SetBrightness( WIPE_DISP_MAIN, WIPE_FADE_BLACK );
            WIPE_SetBrightness( WIPE_DISP_SUB, WIPE_FADE_BLACK );
            
			GF_BGL_ClearCharSet( ENCOUNT_EFF_FRAME, 32, 0, HEAPID_WORLD );
			GF_BGL_ScrClear( wk->fmap->bgl, ENCOUNT_EFF_FRAME );
			
			return FALSE;
		}
		break;
	}	
	
	return TRUE;
}

u32 GetLasterBuffMatrixData( u16 x, u16 y )
{
	return ( (y << 16) | x );
}

static void LasterScroll( TCB_PTR tcb, void* work )
{
	int	 i, j;
	ENC_LASTER_BUFFER* buff;
	FSS_ECE*		wk  = work;
	HC_LASTER_SYS*	hls = wk->hls;
	MtxFx22	mtx;
	int scr_bg2_x, scr_bg2_y, scr_bg3_x, scr_bg3_y;
	
	scr_bg2_x = GF_BGL_ScreenScrollXGet(wk->fmap->bgl, FRMAP_FRAME_EFF);
	scr_bg2_y = GF_BGL_ScreenScrollXGet(wk->fmap->bgl, FRMAP_FRAME_EFF);
	scr_bg3_x = GF_BGL_ScreenScrollXGet(wk->fmap->bgl, FRMAP_FRAME_MAP);
	scr_bg3_y = GF_BGL_ScreenScrollXGet(wk->fmap->bgl, FRMAP_FRAME_MAP);
	
	if (wk->flag == TRUE){
		
		buff = FEFTool_GetDefLasterWriteBuff( hls->dlp );

		for (i = 0; i < HC_LASTER_MAX; i++){
			
			hls->lst[i].ofs_x = 0;
			hls->lst[i].ofs_y = 0;
			
			for (j = hls->lst[i].start; j < hls->lst[i].end; j++){
				s16 now_x, now_y;
				s16 set_x, set_y;
				
				now_x = hls->lst[i].init & 0xffff;
				now_y = hls->lst[i].init >> 16;
			#if 0
				buff[j] = GetLasterBuffMatrixData( now_x + hls->lst[i].ofs_x, now_y + hls->lst[i].ofs_y );
			#else
				AffineMtxMake_2D( &mtx, 0, FX32_ONE, FX32_ONE, AFFINE_MAX_NORMAL );
				//set_x = now_x + hls->lst[i].ofs_x;
				set_x = scr_bg2_x + hls->lst[i].ofs_x;
				//set_y = now_y + hls->lst[i].ofs_y;
				set_y = scr_bg2_y + hls->lst[i].ofs_y;
				if(hls->laster_mode == LASTER_MODE_005){
					set_x %= 256;
					set_y %= 256;
				}
				else if(hls->laster_mode == LASTER_MODE_004){
					if(set_x < 0){
	//					set_x = -set_x;
						set_x = 256+set_x;
					}
					if(set_y < 0){
						set_y = -set_y;
					}
					set_x %= 256;
				}
				G2x_SetBGyAffine_((u32)&buff[j].bg2_pa, &mtx, 0, 0, set_x, set_y);

				//set_x = now_x + hls->lst[i].ofs_x;
				set_x = scr_bg3_x + hls->lst[i].ofs_x;
				//set_y = now_y + hls->lst[i].ofs_y;
				set_y = scr_bg3_y + hls->lst[i].ofs_y;
			//	set_x %= 256;
			//	set_y %= 256;
				if(hls->laster_mode == LASTER_MODE_005){
					set_x %= 256;
					set_y %= 256;
				}
				else if(hls->laster_mode == LASTER_MODE_004){
					if(set_x < 0){
	//					set_x = -set_x;
						set_x = 256+set_x;
					}
					if(set_y < 0){
						set_y = -set_y;
					}
					set_x %= 256;
				}
				G2x_SetBGyAffine_((u32)&buff[j].bg3_pa, &mtx, 0, 0, set_x, set_y);
			#endif
			}
		}

		FEFTool_DeleteDefLaster(hls->dlp);
		sys_FreeMemoryEz(hls);
		TCB_Delete(tcb);
		return;
	}
		
	buff = FEFTool_GetDefLasterWriteBuff( hls->dlp );

	for (i = 0; i < HC_LASTER_MAX; i++){
		
		hls->lst[i].ofs_x += hls->lst[i].speed_x;
		hls->lst[i].ofs_y += hls->lst[i].speed_y;
		
		for (j = hls->lst[i].start; j < hls->lst[i].end; j++){
			s16 now_x, now_y;
			s16 set_x, set_y;
			now_x = hls->lst[i].init & 0xffff;
			now_y = hls->lst[i].init >> 16;
		#if 0
			buff[j] = GetLasterBuffMatrixData( now_x + hls->lst[i].ofs_x, now_y + hls->lst[i].ofs_y );
		#else
			AffineMtxMake_2D( &mtx, 0, FX32_ONE, FX32_ONE, AFFINE_MAX_NORMAL );
			//set_x = now_x + hls->lst[i].ofs_x;
			//set_y = now_y + hls->lst[i].ofs_y;
			set_x = scr_bg2_x + hls->lst[i].ofs_x;
			set_y = scr_bg2_y + hls->lst[i].ofs_y;
			if(hls->laster_mode == LASTER_MODE_005){
				set_x %= 256;
				set_y %= 256;
			}
			else if(hls->laster_mode == LASTER_MODE_004){
				if(set_x < 0){
//					set_x = -set_x;
					set_x = 256+set_x;
				}
				if(set_y < 0){
					set_y = -set_y;
				}
				set_x %= 256;
			}
			G2x_SetBGyAffine_((u32)&buff[j].bg2_pa, &mtx, 0, 0, set_x, set_y);

			//set_x = now_x + hls->lst[i].ofs_x;
			//set_y = now_y + hls->lst[i].ofs_y;
			set_x = scr_bg3_x + hls->lst[i].ofs_x;
			set_y = scr_bg3_y + hls->lst[i].ofs_y;
			if(hls->laster_mode == LASTER_MODE_005){
				set_x %= 256;
				set_y %= 256;
			}
			else if(hls->laster_mode == LASTER_MODE_004){
				if(set_x < 0){
					set_x = 256+set_x;
				}
				if(set_y < 0){
					set_y = -set_y;
				}
				set_x %= 256;
			}
			G2x_SetBGyAffine_((u32)&buff[j].bg3_pa, &mtx, 0, 0, set_x, set_y);
		#endif
		}	
	}
}

static BOOL EncountEffect_004(FSS_ECE* wk)
{
	ENC_LASTER_BUFFER init_num;
	
	switch ( wk->seq ){
	
	case 0:
		FENC_EncountFlashTask(MASK_MAIN_DISPLAY, 16, -16,  &wk->state, 2);
		wk->seq++;
		break;	
		
	case 1:
		if ( wk->state == FALSE ){ break; }
		MI_CpuClear8(&init_num, sizeof(ENC_LASTER_BUFFER));
		wk->flag	 = FALSE;
		wk->hls		 = sys_AllocMemory( HEAPID_FRONTIERMAP, sizeof( HC_LASTER_SYS ) );
		wk->hls->laster_mode = LASTER_MODE_004;
		wk->hls->dlp = FEFTool_InitDefLaster(ENCOUNT_HDMA_DEST_ADDRS,
											 &init_num,
											 HEAPID_FRONTIERMAP );
		{
			int i;
			for (i = 0; i < HC_LASTER_MAX; i++){
				wk->hls->lst[i].start	 = ( i * HC_LASTER_VOFS );
				wk->hls->lst[i].end		 = wk->hls->lst[ i ].start + HC_LASTER_VOFS;
				wk->hls->lst[i].speed_x	 = ( ( ( HC_LASTER_MAX / 2 ) - i ) + 1 ) % 8;
				wk->hls->lst[i].speed_y	 = ( ( ( HC_LASTER_MAX / 2 ) - i ) + 1 ) / 4;
				wk->hls->lst[i].ofs_x	 = 0;
				wk->hls->lst[i].ofs_y	 = 0;
				wk->hls->lst[i].init	 = GetLasterBuffMatrixData( 0, 0 );
			}
		}
		ChangeBrightnessRequest( 40, BRIGHTNESS_BLACK, 0, PLANEMASK_BG1|PLANEMASK_BG2|PLANEMASK_BG3|PLANEMASK_OBJ, MASK_MAIN_DISPLAY );
		TCB_Add( LasterScroll, wk, 0x1000);	
		wk->seq++;
		break;
	case 2:
		if ( IsFinishedBrightnessChg( MASK_MAIN_DISPLAY ) == FALSE ){ break; }
		wk->flag = TRUE;
		GF_BGL_VisibleSet( FRMAP_FRAME_MAP, VISIBLE_OFF );
		GF_BGL_ScrollSet( wk->fmap->bgl, FRMAP_FRAME_MAP, GF_BGL_SCROLL_X_SET, 0);
		GF_BGL_ScrollSet( wk->fmap->bgl, FRMAP_FRAME_MAP, GF_BGL_SCROLL_Y_SET, 0);
		wk->seq++;
		break;
		
	default:
		return FALSE;
	}
	
	return TRUE;
}


static BOOL EncountEffect_005(FSS_ECE* wk)
{
	ENC_LASTER_BUFFER init_num;

	switch ( wk->seq ){
	
	case 0:
		FENC_EncountFlashTask(MASK_MAIN_DISPLAY, 16, -16,  &wk->state, 2);
		wk->seq++;
		break;	
		
	case 1:
		if ( wk->state == FALSE ){ break; }
		MI_CpuClear8(&init_num, sizeof(ENC_LASTER_BUFFER));
		wk->flag	 = FALSE;
		wk->hls		 = sys_AllocMemory( HEAPID_FRONTIERMAP, sizeof( HC_LASTER_SYS ) );
		wk->hls->laster_mode = LASTER_MODE_005;
		wk->hls->dlp = FEFTool_InitDefLaster(ENCOUNT_HDMA_DEST_ADDRS,
											 &init_num,
											 HEAPID_FRONTIERMAP );
		{
			int i;
			for (i = 0; i < HC_LASTER_MAX; i++){
				wk->hls->lst[i].start	 = ( i * HC_LASTER_VOFS );
				wk->hls->lst[i].end		 = wk->hls->lst[ i ].start + HC_LASTER_VOFS;
				
				wk->hls->lst[i].speed_x = ( ( ( HC_LASTER_MAX / 2 ) - i ) / 8 ) + 1;
				if ( i % 2 ){
					wk->hls->lst[i].speed_x *= -1;
				}
				if ( i < ( HC_LASTER_MAX / 2 ) ){
					wk->hls->lst[i].speed_y = i;
				}
				else {
					wk->hls->lst[i].speed_y = HC_LASTER_MAX - i;
				}
			//	wk->hls->lst[i].speed_y *= 2;
				wk->hls->lst[i].ofs_x	 = 0;
				wk->hls->lst[i].ofs_y	 = 0;
				wk->hls->lst[i].init	 = GetLasterBuffMatrixData( 0, 0 );
			}
		}
		ChangeBrightnessRequest( 40, BRIGHTNESS_BLACK, 0, PLANEMASK_BG2|PLANEMASK_BG3|PLANEMASK_OBJ, MASK_MAIN_DISPLAY );
		TCB_Add( LasterScroll, wk, 0x1000);	
		wk->seq++;
		break;
	case 2:
		if ( IsFinishedBrightnessChg( MASK_MAIN_DISPLAY ) == FALSE ){ break; }
		wk->flag = TRUE;
		GF_BGL_VisibleSet( FRMAP_FRAME_MAP, VISIBLE_OFF );
		GF_BGL_ScrollSet( wk->fmap->bgl, FRMAP_FRAME_MAP, GF_BGL_SCROLL_X_SET, 0);
		GF_BGL_ScrollSet( wk->fmap->bgl, FRMAP_FRAME_MAP, GF_BGL_SCROLL_Y_SET, 0);
		wk->seq++;
		break;
		
	default:
		return FALSE;
	}
	
	return TRUE;
}


static BOOL EncountEffect_006(FSS_ECE* wk)
{
	switch ( wk->seq ){
	case 0:
		FEF_Laster_Init( &wk->laster, HEAPID_FRONTIERMAP );
		wk->seq++;
		break;
	
	case 1:
		FEF_Laster_Start( &wk->laster, 0, 191, 
					ENCOUNT_LASTER_SIN_ADDR, 
					ENCOUNT_LASTER_SIN_R, ENCOUNT_LASTER_SP,
					LASTER_SCROLL_MBG3, 0, ENCOUNT_LASTER_TASK_PRI );
		wk->seq++;
		break;	
	default:	
		break;
	}
#if 0
	switch ( wk->seq ){
	
	case 0:
		FENC_EncountFlashTask(MASK_MAIN_DISPLAY, 16, -16,  &wk->state, 2);
		wk->seq++;
		break;	
		
	case 1:
		if ( wk->state == FALSE ){ break; }
		
		{
			///< BMPì¬
			GXRgb color = 0;
			GF_BGL_PaletteSet( ENCOUNT_EFF_FRAME, &color, sizeof(short), 2*15 );
			wk->p_bmp = GF_BGL_BmpWinAllocGet( HEAPID_WORLD, 1 );
			GF_BGL_BmpWinAdd( wk->fmap->bgl, wk->p_bmp, ENCOUNT_EFF_FRAME, 0, 0, 32, 32, 0, 0 );
			GF_BGL_BmpWinDataFill( wk->p_bmp, 0 );
			GF_BGL_BmpWinFill( wk->p_bmp, 15, 0, 0, 128, 192);
			GF_BGL_BmpWinOnVReq( wk->p_bmp );
			GF_BGL_VisibleSet( ENCOUNT_EFF_FRAME, VISIBLE_ON );
			
		}
		{
			///< ƒ‰ƒXƒ^[ˆ—
			wk->flag	 = FALSE;
			wk->hls		 = sys_AllocMemory( HEAPID_WORLD, sizeof( HC_LASTER_SYS ) );
			wk->hls->dlp = FEFTool_InitDefLaster(LASTER_SCROLL_MBG2,
												 GetLasterBuffMatrixData( 0, 0 ), HEAPID_WORLD );
			{
				int i;
				for (i = 0; i < HC_LASTER_MAX; i++){
					wk->hls->lst[i].start	 = ( i * HC_LASTER_VOFS );
					wk->hls->lst[i].end		 = wk->hls->lst[ i ].start + HC_LASTER_VOFS;
										
					if ( i % 2 ){
						wk->hls->lst[i].speed_x  = i % 4;
						wk->hls->lst[i].speed_y  = 0;
						wk->hls->lst[i].ofs_x	 = 0;
						wk->hls->lst[i].ofs_y	 = 0;
					}
					else {
						wk->hls->lst[i].speed_x  = -i % 4;
						wk->hls->lst[i].speed_y  = 0;
						wk->hls->lst[i].ofs_x	 = 256;
						wk->hls->lst[i].ofs_y	 = 0;
					}
					wk->hls->lst[i].init	 = GetLasterBuffMatrixData( 0, 0 );
				}
			}
		}
		ChangeBrightnessRequest( 40, BRIGHTNESS_BLACK, 0, PLANEMASK_BG1|PLANEMASK_BG2|PLANEMASK_BG3|PLANEMASK_OBJ, MASK_MAIN_DISPLAY );
		TCB_Add( LasterScroll, wk, 0x1000);	
		wk->seq++;
		break;
	case 2:
		if ( IsFinishedBrightnessChg( MASK_MAIN_DISPLAY ) == FALSE ){ break; }
		wk->flag = TRUE;
		GF_BGL_VisibleSet( ENCOUNT_EFF_FRAME, VISIBLE_OFF );
		GF_BGL_VisibleSet( FRMAP_FRAME_MAP, VISIBLE_OFF );
		GF_BGL_ScrollSet( wk->fmap->bgl, FRMAP_FRAME_MAP, GF_BGL_SCROLL_X_SET, 0);
		GF_BGL_ScrollSet( wk->fmap->bgl, FRMAP_FRAME_MAP, GF_BGL_SCROLL_Y_SET, 0);
		
			
		GF_BGL_BmpWinOff( wk->p_bmp );
		GF_BGL_BmpWinDel( wk->p_bmp );
		GF_BGL_BmpWinFree( wk->p_bmp, 1 );
		
		GF_BGL_ClearCharSet( ENCOUNT_EFF_FRAME, 32, 0, HEAPID_WORLD );
		GF_BGL_ScrClear( wk->fmap->bgl, ENCOUNT_EFF_FRAME );
		
		wk->seq++;
		break;
		
	default:
		return FALSE;
	}
#endif
	return TRUE;
}


//--------------------------------------------------------------------------------------------
/**
 * ƒGƒ“ƒJƒEƒ“ƒgƒGƒtƒFƒNƒgŒÄ‚Ño‚µ
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	í‚É1
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_EncountEffect( FSS_TASK * core )
{
	FRONTIER_EX_PARAM* ex_param;
	FSS_ECE* fss_ece;
		
	ex_param = Frontier_ExParamGet( core->fss->fmain );

	core->reg[0] = FSSTGetWorkValue(core);	
	
	Snd_DataSetByScene( SND_SCENE_BATTLE, SEQ_BA_TRAIN, 1 );		//ƒoƒgƒ‹‹ÈÄ¶
	OS_Printf( " EncountEffect No = %2d\n", core->reg[0] );
	
	fss_ece				= sys_AllocMemory( HEAPID_WORLD, sizeof( FSS_ECE ) );
	fss_ece->temp		= Frontier_SysWorkGet( core->fss->fmain );
	fss_ece->seq		= 0;
	fss_ece->ece_type	= core->reg[0];
	fss_ece->fmap		= FSS_GetFMapAdrs( core->fss );
	
	Frontier_SysWorkSet( core->fss->fmain, fss_ece );
	
	FSST_SetWait( core, EncountEffectWait );

	//ƒoƒbƒNƒOƒ‰ƒEƒ“ƒhF‚ğ•‚É‚µ‚Ä‚¨‚­
	PaletteWork_Clear(fss_ece->fmap->pfd, FADE_MAIN_BG, FADEBUF_ALL, 0x0000, 0, 1);

	return 1;
}

//return 1 = I—¹
static BOOL EncountEffectWait(FSS_TASK * core)
{
	BOOL active;
	
	FSS_ECE* fss_ece = Frontier_SysWorkGet( core->fss->fmain );
	
	active = EncountEffect_Table[ fss_ece->ece_type ]( fss_ece );
	
	if ( active == FALSE ){
		WIPE_SetBrightness( WIPE_DISP_MAIN, WIPE_FADE_BLACK );
		WIPE_SetBrightness( WIPE_DISP_SUB,	WIPE_FADE_BLACK );		
		Frontier_SysWorkSet( core->fss->fmain, fss_ece->temp );
		sys_FreeMemoryEz( fss_ece );
	}
	
	return !active;
}

//--------------------------------------------------------------------------------------------
/**
 * ålŒö‚Ìl•¨OBJƒR[ƒh‚ğæ“¾
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_GetMineObj( FSS_TASK * core )
{
	u16 code;
	FRONTIER_EX_PARAM* ex_param;
	u16* ret_wk	= FSSTGetWork( core );

	ex_param = Frontier_ExParamGet( core->fss->fmain );

	if( MyStatus_GetMySex(SaveData_GetMyStatus(ex_param->savedata)) == PM_MALE ){
		code = HERO;
	}else{
		code = HEROINE;
	}

	*ret_wk = code;
	return 0;
}

//==============================================================================
//
//	ƒp[ƒeƒBƒNƒ‹
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   ƒp[ƒeƒBƒNƒ‹ƒVƒXƒeƒ€‚ğì¬‚µAƒŠƒ\[ƒX“Ç‚İ‚İ•“o˜^
 *
 * @param   core		
 */
//--------------------------------------------------------------
static BOOL FSS_ParticleSPALoad(FSS_TASK *core)
{
	FSS_PTR fss = core->fss;
	FMAP_PTR fmap = Frontier_FMapAdrsGet(fss->fmain);
	u16 work_id, spa_no, camera_type;
	
	work_id = FSSTGetWorkValue(core);
	spa_no = FSSTGetWorkValue(core);
	camera_type = FSSTGetWorkValue(core);
	
	FRParticle_SystemCreate(fmap->frp, work_id, spa_no, camera_type);

	return 0;
}

//--------------------------------------------------------------
/**
 * @brief   ƒp[ƒeƒBƒNƒ‹ƒVƒXƒeƒ€‚ğ”jŠü‚·‚é
 *
 * @param   core		
 */
//--------------------------------------------------------------
static BOOL FSS_ParticleSPAExit(FSS_TASK *core)
{
	FSS_PTR fss = core->fss;
	FMAP_PTR fmap = Frontier_FMapAdrsGet(fss->fmain);
	u16 work_id;
	
	work_id = FSSTGetWorkValue(core);
	FRParticle_SystemExit(fmap->frp, work_id);

	return 0;
}

//--------------------------------------------------------------
/**
 * @brief   ƒGƒ~ƒbƒ^‚ğ¶¬‚·‚é
 *
 * @param   core		
 */
//--------------------------------------------------------------
static BOOL FSS_ParticleAddEmitter(FSS_TASK *core)
{
	FSS_PTR fss = core->fss;
	FMAP_PTR fmap = Frontier_FMapAdrsGet(fss->fmain);
	u16 work_id, emit_no;
	PTC_PTR ptc;
	
	work_id = FSSTGetWorkValue(core);
	emit_no = FSSTGetWorkValue(core);
	
	ptc = FRParticle_PTCPtrGet(fmap->frp, work_id);
	Particle_CreateEmitterCallback(ptc, emit_no, NULL, NULL);

	return 0;
}

//--------------------------------------------------------------
/**
 * @brief   ¶¬‚³‚ê‚Ä‚¢‚é‘S‚Ä‚Ìƒp[ƒeƒBƒNƒ‹‚ÌI—¹‚ğ‘Ò‚Â
 *
 * @param   core		
 */
//--------------------------------------------------------------
static BOOL FSS_ParticleWait(FSS_TASK *core)
{
	FSST_SetWait( core, ParticleEmitWait );
	return 1;
}

//return 1 = I—¹
static BOOL ParticleEmitWait(FSS_TASK * core)
{
	FSS_PTR fss = core->fss;
	FMAP_PTR fmap = Frontier_FMapAdrsGet(fss->fmain);

	if(FRParticle_EmitNumGet(fmap->frp) == TRUE){
		return 1;
	}
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief   ’nkƒZƒbƒg
 *
 * @param   core		
 */
//--------------------------------------------------------------
static BOOL FSS_ShakeSet(FSS_TASK *core)
{
	FSS_PTR fss = core->fss;
	FMAP_PTR fmap = Frontier_FMapAdrsGet(fss->fmain);
	u16 x, y, wait, loop;
	FMAP_SHAKE_WORK *shake;
	
	shake = &fmap->shake_work;
	if(shake->tcb != NULL){
		GF_ASSERT(0);
		TCB_Delete(shake->tcb);
	}

	x = FSSTGetWorkValue(core);
	y = FSSTGetWorkValue(core);
	wait = FSSTGetWorkValue(core);
	loop = FSSTGetWorkValue(core);
	
	MI_CpuClear8(shake, sizeof(FMAP_SHAKE_WORK));
	shake->shake_x = x;
	shake->shake_y = y;
	shake->wait_max = wait;
	shake->loop = loop;
	shake->tcb = TCB_Add(FSSC_Sub_ShakeMove, shake, TCBPRI_SHAKE);
	
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief   ’nk‚ÌI—¹‚ğ‘Ò‚Â
 *
 * @param   core		
 */
//--------------------------------------------------------------
static BOOL FSS_ShakeWait(FSS_TASK *core)
{
	FSST_SetWait( core, ShakeEndWait );
	return 1;
}

//return 1 = I—¹
static BOOL ShakeEndWait(FSS_TASK * core)
{
	FSS_PTR fss = core->fss;
	FMAP_PTR fmap = Frontier_FMapAdrsGet(fss->fmain);

	if(fmap->shake_work.tcb == NULL){
		return 1;
	}
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief   Window‹@”\ƒZƒbƒg
 *
 * @param   core		
 */
//--------------------------------------------------------------
static BOOL FSS_WndSet(FSS_TASK *core)
{
	FSS_PTR fss = core->fss;
	FMAP_PTR fmap = Frontier_FMapAdrsGet(fss->fmain);
	u16 x, y, wait, loop;
	FMAP_WND_WORK *wnd;
	
	wnd = &fmap->wnd_work;
	if(wnd->tcb != NULL){
		GF_ASSERT(0);
		TCB_Delete(wnd->tcb);
	}

	MI_CpuClear8(wnd, sizeof(FMAP_WND_WORK));
	wnd->x1 = FSSTGetWorkValue(core);
	wnd->y1 = FSSTGetWorkValue(core);
	wnd->x2 = FSSTGetWorkValue(core);
	wnd->y2 = FSSTGetWorkValue(core);
	wnd->wait = FSSTGetWorkValue(core);
	wnd->on_off = FSSTGetWorkValue(core);
	wnd->tcb = TCB_Add(FSSC_Sub_Window, wnd, TCBPRI_WND);
	
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief   Window‹@”\‚ÌI—¹‚ğ‘Ò‚Â
 *
 * @param   core		
 */
//--------------------------------------------------------------
static BOOL FSS_WndWait(FSS_TASK *core)
{
	FSST_SetWait( core, WndEndWait );
	return 1;
}

//return 1 = I—¹
static BOOL WndEndWait(FSS_TASK * core)
{
	FSS_PTR fss = core->fss;
	FMAP_PTR fmap = Frontier_FMapAdrsGet(fss->fmain);

	if(fmap->wnd_work.tcb == NULL){
		return 1;
	}
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒfƒoƒbƒN
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_DebugPrint( FSS_TASK * core )
{
	u16 wk_id = FSSTGetU16(core);
	OS_Printf( "wk_id = %d", wk_id );
	OS_Printf( "num = %d\n", FSS_GetEventWorkValue(core,wk_id) );
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒ|ƒPƒ‚ƒ“ƒAƒNƒ^[•\¦
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
//evy		ŒW”(0`16) (•ÏX‚Ì“x‡‚¢A0(Œ³‚ÌF)`2,3..(’†ŠÔF)`16(w’è‚µ‚½F)
//next_rgb	•ÏXŒã‚ÌF‚ğw’è
#define STAGE_EVY			( 8 ) //ˆÃ‚¢(14)//–¾‚é‚¢( 1 )
#define STAGE_COLOR_FADE	( GX_RGB( 0,0,0 ) )

static BOOL FSSC_TemotiPokeActAdd( FSS_TASK * core )
{
	POKEPARTY* party;
	POKEMON_PARAM* pp;
	FRONTIER_EX_PARAM* ex_param;
	FMAP_PTR fmap = FSS_GetFMapAdrs( core->fss );
	u16 pos = FSSTGetWorkValue( core );
	u16 x	= FSSTGetWorkValue( core );
	u16 y	= FSSTGetWorkValue( core );
	u16 id	= FSSTGetU16( core );
	u16 evy = FSSTGetU16( core );
	u16 next_rgb = FSSTGetU16( core );
	
	ex_param = Frontier_ExParamGet( core->fss->fmain );
	party = SaveData_GetTemotiPokemon( ex_param->savedata );	//è‚¿ƒp[ƒeƒBæ“¾
	pp = PokeParty_GetMemberPointer( party, pos );

	/***********************************/
	//ÀŒ±’†‚Ì‚½‚ßA’¼Ú’l‚ğ“ü‚ê‚Ä‚¢‚Ü‚·
	/***********************************/

	evy		= STAGE_EVY;
	next_rgb= STAGE_COLOR_FADE;

	FSSC_Sub_PokemonActorAdd( fmap, pp, HEAPID_WORLD, id, x, y, 0, 0, evy, next_rgb );
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒ|ƒPƒ‚ƒ“ƒAƒNƒ^[íœ
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_TemotiPokeActDel( FSS_TASK * core )
{
	FMAP_PTR fmap = FSS_GetFMapAdrs( core->fss );
	u16 id	= FSSTGetU16( core );

	FSSC_Sub_PokemonDispDelete( fmap, id );
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief   ƒtƒƒ“ƒeƒBƒAƒuƒŒ[ƒ“í‚ÌƒGƒ“ƒJƒEƒ“ƒgƒGƒtƒFƒNƒg‚ğ”­“®
 *
 * @param   core		
 *
 * @retval  "1"
 */
//--------------------------------------------------------------
static BOOL FSSC_BrainEncountEffect(FSS_TASK *core)
{
	FMAP_PTR fmap = FSS_GetFMapAdrs( core->fss );
	u16 effect_no	= FSSTGetU16( core );
	s16 map_x, map_y;
	
	FMap_ScrollPosGet(fmap, &map_x, &map_y);
	
	BrainCutin_EffectStart(effect_no, fmap->bgl, fmap->clactsys.csp, fmap->clactsys.crp, 
		fmap->pfd, &core->reg[0], map_x, map_y);

	Snd_DataSetByScene( SND_SCENE_BATTLE, SEQ_PL_BA_BRAIN, 1 );		//ƒoƒgƒ‹‹ÈÄ¶
	
	FSST_SetWait( core, BrainEncountEffectWait );
	return 1;
}

//return 1 = I—¹
static BOOL BrainEncountEffectWait(FSS_TASK * core)
{
	FSS_PTR fss = core->fss;
	FMAP_PTR fmap = Frontier_FMapAdrsGet(fss->fmain);

	if(core->reg[0] == TRUE){
		return 1;
	}
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	ƒŒƒR[ƒhƒf[ƒ^‚ÌƒCƒ“ƒNƒŠƒƒ“ƒg
 *
 * @param   core		
 *
 * @retval  "0"
 */
//--------------------------------------------------------------
static BOOL FSSC_RecordInc(FSS_TASK *core)
{
	FRONTIER_EX_PARAM *ex_param;
	u16 recid	= FSSTGetU16( core );

	ex_param	= Frontier_ExParamGet(core->fss->fmain);

	RECORD_Inc(SaveData_GetRecord(ex_param->savedata),recid);
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	ƒŒƒR[ƒhƒf[ƒ^‚Ì’Ç‰Á
 *
 * @param   core		
 *
 * @retval  "0"
 */
//--------------------------------------------------------------
static BOOL FSSC_RecordAdd(FSS_TASK *core)
{
	FRONTIER_EX_PARAM *ex_param;
	u16 recid	= FSSTGetU16( core );
	u16 val		= FSSTGetWorkValue( core );

	ex_param	= Frontier_ExParamGet(core->fss->fmain);

	RECORD_Add(SaveData_GetRecord(ex_param->savedata),recid,val);
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	ƒXƒRƒA‚Ì’Ç‰Á
 *
 * @param   core		
 *
 * @retval  "0"
 */
//--------------------------------------------------------------
static BOOL FSSC_ScoreAdd(FSS_TASK *core)
{
	FRONTIER_EX_PARAM *ex_param;
	u16 recid	= FSSTGetU16( core );

	ex_param	= Frontier_ExParamGet(core->fss->fmain);

	RECORD_Score_Add(SaveData_GetRecord(ex_param->savedata),recid);
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	©•ªA‘Šè‚Ì‚Ç‚¿‚ç‚©‚ªDP‚©ƒ`ƒFƒbƒN
 *
 * @param   core		
 *
 * @retval  "0"
 *
 * "1 = DP‚ ‚è"
 */
//--------------------------------------------------------------
static BOOL FSSC_CheckDPRomCode(FSS_TASK *core)
{
	FRONTIER_EX_PARAM *ex_param;
	u16* ret_wk	= FSSTGetWork(core);			//Œ‹‰Êæ“¾

	ex_param= Frontier_ExParamGet(core->fss->fmain);
	*ret_wk = Frontier_CheckDPRomCode( ex_param->savedata );
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	©•ª‚ÌROM‚æ‚è‚à‚‚¢ƒT[ƒo[ƒo[ƒWƒ‡ƒ“‚Å“®‚¢‚Ä‚¢‚½‚©ƒ`ƒFƒbƒN
 *
 * @param   core		
 *
 * @retval  "0"
 */
//--------------------------------------------------------------
static BOOL FSSC_BattleRecServerVersionCheck(FSS_TASK *core)
{
	u16* ret_wk	= FSSTGetWork(core);			//Œ‹‰Êæ“¾
	*ret_wk = BattleRec_ServerVersionCheck();
	OS_Printf( "server result = %d\n", *ret_wk );
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief   “ú‚Ì‹L˜^‚ğ‚Â‚¯‚é(ƒtƒƒ“ƒeƒBƒAê—p)
 *
 * @param   core		
 *
 * @retval  "0"
 */
//--------------------------------------------------------------
static BOOL FSSC_WifiCounterListSetLastPlayDate(FSS_TASK *core)
{
	WIFI_LIST* list;
	FRONTIER_EX_PARAM *ex_param;

	ex_param	= Frontier_ExParamGet(core->fss->fmain);
	list		= SaveData_GetWifiListData( ex_param->savedata );

	WifiList_SetLastPlayDate( list, mydwc_getFriendIndex() );
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * TV:ƒtƒƒ“ƒeƒBƒA‚Å’‡—Ç‚µTEMPƒZƒbƒg
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_TVTempFriendSet( FSS_TASK * core )
{
	u16 bf_no;
	TV_WORK * tvwk;
	MYSTATUS* my;												//ƒp[ƒgƒi[‚ÌMyStatus
	FRONTIER_EX_PARAM *ex_param;

	ex_param	= Frontier_ExParamGet(core->fss->fmain);
	tvwk		= SaveData_GetTvWork( ex_param->savedata );
	bf_no		= FSSTGetWorkValue( core );

	//ƒp[ƒgƒi[‚ÌMyStatus‚ğæ“¾
	my = CommInfoGetMyStatus( 1 - CommGetCurrentID() );

	//ƒƒCƒAƒŒƒX‚Ì‚İŒÄ‚Ô‚æ‚¤‚É‚µ‚È‚¢‚Æƒ_ƒI
	
	TVTOPIC_FriendTemp_Set( tvwk, bf_no, my );
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * WIFI‚Åˆê‹C‚Éƒf[ƒ^‚ğ‘—‚é‚Ì‘Î‰
 *
 * @param	core		‰¼‘zƒ}ƒVƒ“§Œä\‘¢‘Ì‚Ö‚Ìƒ|ƒCƒ“ƒ^
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static BOOL FSSC_CommSetWifiBothNet( FSS_TASK * core )
{
	u16 flag = FSSTGetWorkValue( core );
	CommSetWifiBothNet( flag );
	return 0;
}

// NONMATCHING
static asm BOOL ov104_2231F44( FSS_TASK * core )
{
	push {r3, lr}
	ldr r2, [r0, #0x1c]
	ldr r3, [r0, #0]
	add r1, r2, #1
	str r1, [r0, #0x1c]
	ldrb r1, [r2]
	ldr r0, [r3, #0x60]
	bl ov104_223327C
	mov r0, #1
	pop {r3, pc}
	// .align 2, 0
}

// NONMATCHING
static asm BOOL ov104_2231F5C( FSS_TASK * core )
{
	push {r3, lr}
	ldr r2, [r0, #0x1c]
	ldr r3, [r0, #0]
	add r1, r2, #1
	str r1, [r0, #0x1c]
	ldrb r1, [r2]
	ldr r0, [r3, #0x60]
	bl ov104_2233298
	mov r0, #1
	pop {r3, pc}
	// .align 2, 0
}

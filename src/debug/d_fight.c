//==============================================================================================
/**
 * @file	d_fight.c
 * @brief	デバッグファイト
 * @author	sogabe
 * @date	2006.01.06
 */
//==============================================================================================

#ifdef PM_DEBUG

#include "common.h"
#include "system/fontproc.h"
#include "system/pm_str.h"
#include "system/msgdata.h"						//MSGDATA_MANAGER
#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "system/softsprite.h"
#include "poke_test.h"

#include "msgdata/msg.naix"						//NARC_msg_??_dat
#include "msgdata/msg_debug_fight.h"
#include "battle/battle_common.h"
#include "battle/shinka.h"
#include "poketool/monsno.h"
#include "battle/attr_def.h"
#include "battle/wazano_def.h"
#include "battle/chr_def.h"
#include "battle/tokusyu_def.h"
#include "battle/fight.h"
#include "poketool/poke_tool.h"
#include "poketool/pokeparty.h"
#include "system/snd_tool.h"
#include "system/wordset.h"
#include "system/brightness.h"
#include "system/wipe.h"
#include "itemtool/itemsym.h"
#include "itemtool/myitem.h"
#include "savedata/battle_rec.h"

#include "communication/communication.h"
#include "communication/comm_state.h"

#include "system/main.h"
#include "demo/title.h"
#include "savedata/savedata.h"
#include "d_fight.h"

u32	ee_loop_count=0;		//フロンティアループチェック用カウンタ

//#include <nitro/sinit.h>
#include "system/pm_overlay.h"
FS_EXTERN_OVERLAY(debug_fight);

//==============================================================================================
//
//	プロトタイプ宣言
//
//==============================================================================================

static	void				InitDebugFight(DEBUG_FIGHT_PARAM *dfp);
static	void				MainDebugFight(DEBUG_FIGHT_PARAM *dfp);
static	void				DebugFight_SetProc(DEBUG_FIGHT_PARAM *dfp);
static	void				DebugFightSioConnect(DEBUG_FIGHT_PARAM *dfp);
static	void				DebugFightBRDSave(DEBUG_FIGHT_PARAM *dfp,int flag);
static	void				DebugFightWorkInit(DEBUG_FIGHT_PARAM *dfp);

int		DebugFightCommandBufferGet(void);

DEBUG_POKEMON_PARAM	*DebugFightDPPGet(DEBUG_FIGHT_PARAM *dfp,int mine,int enemy,int pos);

extern	BATTLE_PARAM * BattleParam_Create(int heapID, u32 fight_type);
extern	void	BattleParam_Delete(BATTLE_PARAM * bp);

DEBUG_SAVE_PARAM	*dsp_p=NULL;

//==================================================================================================
//
//	関数
//
//==================================================================================================

static	void	InitDebugFight(DEBUG_FIGHT_PARAM *dfp)
{
	MI_CpuClearFast(dfp,sizeof(DEBUG_FIGHT_PARAM));

	dfp->man_msg=MSGMAN_Create(MSGMAN_TYPE_NORMAL,ARC_MSG,NARC_msg_debug_fight_dat,HEAPID_DEBUG_APPLICATION);
	dfp->man_poke=MSGMAN_Create( MSGMAN_TYPE_NORMAL,ARC_MSG,NARC_msg_monsname_dat,HEAPID_DEBUG_APPLICATION);
	dfp->man_waza=MSGMAN_Create(MSGMAN_TYPE_NORMAL,ARC_MSG,NARC_msg_wazaname_dat,HEAPID_DEBUG_APPLICATION);
	dfp->man_chr=MSGMAN_Create(MSGMAN_TYPE_NORMAL,ARC_MSG,NARC_msg_chr_dat,HEAPID_DEBUG_APPLICATION);
	dfp->man_speabi=MSGMAN_Create(MSGMAN_TYPE_NORMAL,ARC_MSG,NARC_msg_tokusei_dat,HEAPID_DEBUG_APPLICATION);
	dfp->man_item=MSGMAN_Create(MSGMAN_TYPE_NORMAL,ARC_MSG,NARC_msg_itemname_dat,HEAPID_DEBUG_APPLICATION);

	dfp->end_flag = FALSE;
	dfp->bgl=GF_BGL_BglIniAlloc(HEAPID_DEBUG_APPLICATION);
	dfp->win_m=GF_BGL_BmpWinAllocGet(HEAPID_DEBUG_APPLICATION,1);
	dfp->win_s=GF_BGL_BmpWinAllocGet(HEAPID_DEBUG_APPLICATION,1);
	dfp->msg_buf=STRBUF_Create(0x100,HEAPID_DEBUG_APPLICATION);
	dfp->wordset=WORDSET_Create(HEAPID_DEBUG_APPLICATION);

	dfp->pp=PokemonParam_AllocWork(HEAPID_DEBUG_APPLICATION);

	DebugFightBGCreate(dfp,dfp->bgl);

	GF_BGL_BmpWinAdd(dfp->bgl,dfp->win_m,GF_BGL_FRAME1_M,0x00,0x00,32,24,0x00,0x01);
	GF_BGL_BmpWinAdd(dfp->bgl,dfp->win_s,GF_BGL_FRAME1_S,0x00,0x00,32,24,0x00,0x01);

	DebugFightWorkInit(dfp);
}

//============================================================================================
/**
 *	ワーク初期化
 */
//============================================================================================
static	void	DebugFightWorkInit(DEBUG_FIGHT_PARAM *dfp)
{
	int	i,j;

	MI_CpuClearFast(&dfp->dsp,sizeof(DEBUG_SAVE_PARAM));

	for(j=0;j<CLIENT_MAX;j++){
		for(i=0;i<TEMOTI_POKEMAX;i++){
			DebugFightPokemonParamInit(&dfp->dsp.dpp[j][i]);
			if(i==0){
				DebugFightDefaultPokeSet(dfp,&dfp->dsp.dpp[j][i]);
			}
		}
	}

	dfp->flag=0xff;
	dfp->dsp.msg_speed=2;
	dfp->mine=0;
	dfp->enemy=0;
	dfp->cur_pos_m=0;

	DebugFightMainScreenCreate(dfp);
	DebugFightSubScreenCreate(dfp,DebugFightDPPGet(dfp,dfp->mine,dfp->enemy,dfp->cur_pos_m),dfp->page);
	CursorMove(dfp,MOVE_MAIN,dfp->cur_pos_m,CUR_PUT);

	Snd_Stop();
}

//============================================================================================
/**
 *	メインループタスク
 */
//============================================================================================
static	void	MainDebugFight(DEBUG_FIGHT_PARAM *dfp)
{
	int	move=NO_MOVE;

	if(dfp->put_req){
		dfp->put_req=0;
		GF_BGL_BmpWinOn(dfp->win_m);
		GF_BGL_BmpWinOn(dfp->win_s);
	}

	if(sys.trg){
		dfp->auto_timer=0;
	}
	else{
		if(dfp->dsp.battle_status_flag&AUTO_BATTLE){
			dfp->auto_timer++;
		}
	}

	switch(dfp->seq_no){
	case SEQ_MAIN_MENU_MOVE:
		if(sys.trg==PAD_KEY_LEFT){
			move=mlp_m[dfp->cur_pos_m].move_left;
		}
		else if(sys.trg==PAD_KEY_RIGHT){
			move=mlp_m[dfp->cur_pos_m].move_right;
		}
		if(sys.trg==PAD_KEY_UP){
			move=mlp_m[dfp->cur_pos_m].move_up;
		}
		else if(sys.trg==PAD_KEY_DOWN){
			move=mlp_m[dfp->cur_pos_m].move_down;
		}
		else if(sys.trg==PAD_BUTTON_A){
			if(mlp_m[dfp->cur_pos_m].a_button_act!=NULL){
				dfp->sub_seq_no=0;
				dfp->seq_no=SEQ_A_BUTTON_ACT;
			}
		}
		else if((sys.trg==PAD_BUTTON_L)&&(dfp->page)&&(dfp->cur_pos_m<=MS_ENEMY_POKE_6)){
			dfp->page--;
			DebugFightSubScreenCreate(dfp,DebugFightDPPGet(dfp,dfp->mine,dfp->enemy,dfp->cur_pos_m),dfp->page);
		}
		else if((sys.trg==PAD_BUTTON_R)&&(dfp->page<SUB_PAGE_MAX)&&(dfp->cur_pos_m<=MS_ENEMY_POKE_6)){
			dfp->page++;
			DebugFightSubScreenCreate(dfp,DebugFightDPPGet(dfp,dfp->mine,dfp->enemy,dfp->cur_pos_m),dfp->page);
		}
		if(sys.cont==DEBUG_FIGHT_END_KEY){
#if 0
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
			GF_BGL_BGControlExit(dfp->bgl, GF_BGL_FRAME1_M );
			GF_BGL_BGControlExit(dfp->bgl, GF_BGL_FRAME1_S );
			GF_BGL_BmpWinFree(dfp->win_m,1);
			GF_BGL_BmpWinFree(dfp->win_s,1);
			sys_FreeMemoryEz(dfp->bgl);
			MSGMAN_Delete(dfp->man_msg);
			MSGMAN_Delete(dfp->man_poke);
			MSGMAN_Delete(dfp->man_waza);
			MSGMAN_Delete(dfp->man_chr);
			MSGMAN_Delete(dfp->man_speabi);
			MSGMAN_Delete(dfp->man_item);
			sys_FreeMemoryEz(dfp->pp);
			sys_FreeMemoryEz(dfp->msg_buf);
			WORDSET_Delete(dfp->wordset);
			FontProc_UnloadBitData(FONT_SYSTEM);
			dfp->end_flag = TRUE;
			return;
#endif
			DebugFightWorkInit(dfp);
		}
#if 0
		if((sys.trg==PAD_BUTTON_SELECT)&&(dfp->cur_pos_m==MS_LOAD)){
			if((dfp->dsp.dpp[0].mons_no)&&(dfp->dsp.dpp[1].mons_no)){
				WIPE_SYS_Start(WIPE_PATTERN_WMS,
							   WIPE_TYPE_FADEOUT,
							   WIPE_TYPE_FADEOUT,
							   WIPE_FADE_BLACK,
							   WIPE_DEF_DIV,
							   WIPE_DEF_SYNC,
							   HEAPID_DEBUG_APPLICATION);
				dfp->seq_no=SEQ_SHINKA_INIT;
				break;
			}
		}
#endif
		if((sys.trg==PAD_BUTTON_SELECT)&&(dfp->cur_pos_m==MS_LOAD)){
			dfp->seq_no=SEQ_AUTO_FLAG_INIT;
		}
		if((sys.trg==PAD_BUTTON_SELECT)&&(dfp->cur_pos_m==MS_SAVE)){
			dfp->seq_no=SEQ_AI_FLAG_INIT;
		}
		if((sys.trg==PAD_BUTTON_SELECT)&&(dfp->cur_pos_m==MS_BRD_SAVE)){
			dfp->seq_no=SEQ_COM_SELECT_EDIT_INIT;
		}
		if((sys.trg==PAD_BUTTON_SELECT)&&(dfp->cur_pos_m==MS_VERSION)){
			{
				LOAD_RESULT	result;

				BattleRec_Load(dfp->sv,HEAPID_WORLD,&result,NULL,LOADDATA_MYREC);
			}
			dfp->brdedit_client_no=0;
			dfp->seq_no=SEQ_REC_DATA_EDIT_INIT;
		}
		if((sys.trg==PAD_BUTTON_START)||((dfp->dsp.battle_status_flag&AUTO_BATTLE)&&(dfp->auto_timer>AUTO_BATTLE_WAIT))){
			if(DebugFightStart(dfp)==TRUE){
				Overlay_UnloadID(FS_OVERLAY_ID(debug_fight));
				DebugFightSioConnect(dfp);
				Snd_SceneSet(SND_SCENE_DUMMY);
				Snd_DataSetByScene( SND_SCENE_BATTLE, SEQ_FIGHT0101, 1 );	//バトル曲再生
				WIPE_SYS_Start(WIPE_PATTERN_WMS,
						   WIPE_TYPE_FADEOUT,
						   WIPE_TYPE_FADEOUT,
						   WIPE_FADE_BLACK,
						   WIPE_DEF_DIV,
						   WIPE_DEF_SYNC,
						   HEAPID_DEBUG_APPLICATION);
				dfp->seq_no=SEQ_BATTLE_INIT;
			}
			break;
		}
		if(move!=NO_MOVE){
			CursorMove(dfp,MOVE_MAIN,move,CUR_PUT);
			if(dfp->cur_pos_m<=MS_ENEMY_POKE_6){
				DebugFightSubScreenCreate(dfp,DebugFightDPPGet(dfp,dfp->mine,dfp->enemy,dfp->cur_pos_m),dfp->page);
			}
			else{
				DebugFightSubScreenCreate(dfp,DebugFightDPPGet(dfp,dfp->mine,dfp->enemy,dfp->cur_pos_m),0xff);
			}
		}
		break;
	case SEQ_A_BUTTON_ACT:
		if(dfp->cur_pos_m<MS_GROUND){
			if(mlp_m[dfp->cur_pos_m].a_button_act(dfp,DebugFightDPPGet(dfp,dfp->mine,dfp->enemy,dfp->cur_pos_m))==TRUE){
				dfp->seq_no=SEQ_MAIN_MENU_MOVE;
			}
		}else{
			if(mlp_m[dfp->cur_pos_m].a_button_act(dfp,NULL)==TRUE){
				dfp->seq_no=SEQ_MAIN_MENU_MOVE;
			}
		}
		break;
	case SEQ_BATTLE_INIT:
		if(WIPE_SYS_EndCheck()==FALSE){
			break;
		}
		if((dfp->param->fight_type&FIGHT_TYPE_SIO)&&
		  ((dfp->param->battle_status_flag&BATTLE_STATUS_FLAG_REC_BATTLE)==0)&&
		   (CommStateDBattleIsReady()!=TRUE)){
			break;
		}
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
		GF_BGL_BGControlExit(dfp->bgl, GF_BGL_FRAME1_M );
		GF_BGL_BGControlExit(dfp->bgl, GF_BGL_FRAME1_S );

		if(dfp->param->battle_status_flag&BATTLE_STATUS_FLAG_REC_BATTLE){
			{
				LOAD_RESULT	result;

				BattleRec_Load(dfp->sv,HEAPID_WORLD,&result,dfp->param,LOADDATA_MYREC);
			}
		}
		else{
			{
				int	i;

				for(i=0;i<CommGetConnectNum();i++){
					dfp->param->comm_stand_no[i]=CommGetStandNo(i);
				}
			}
			dfp->param->comm_id=CommGetCurrentID();
		}

		Overlay_UnloadID(FS_OVERLAY_ID(debug_fight));
		DebugFight_SetProc(dfp);
		dfp->dsp.cb_pos=0;
		dfp->seq_no=SEQ_BATTLE_MAIN;
		break;
	case SEQ_BATTLE_MAIN:
		if(ProcMain(dfp->proc)){
			PROC_Delete(dfp->proc);
			Overlay_Load(FS_OVERLAY_ID(debug_fight), OVERLAY_LOAD_NOT_SYNCHRONIZE);
			if((dfp->param->fight_type&FIGHT_TYPE_SIO)&&
			  ((dfp->param->battle_status_flag&BATTLE_STATUS_FLAG_REC_BATTLE)==0)){
				CommSendFixData(CS_AUTO_EXIT);
			}
			sys_FreeMemoryEz(dfp->param->bag_cursor);
			BattleParam_Delete(dfp->param);
			dfp->proc=NULL;
			DebugFightBGCreate(dfp,dfp->bgl);

			DebugFightMainScreenCreate(dfp);
			if(dfp->cur_pos_m<=MS_ENEMY_POKE_6){
				DebugFightSubScreenCreate(dfp,DebugFightDPPGet(dfp,dfp->mine,dfp->enemy,dfp->cur_pos_m),dfp->page);
			}
			CursorMove(dfp,MOVE_MAIN,dfp->cur_pos_m,CUR_PUT);
			dfp->seq_no=SEQ_MAIN_MENU_MOVE;
			Snd_Stop();
			dfp->auto_timer=0;
		}
		break;
	case SEQ_SHINKA_INIT:
#if 0
		if(WIPE_SYS_EndCheck()){
			{
				int		i;
				STRCODE	name[PERSON_NAME_SIZE+EOM_SIZE];

				GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
				GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
				GF_BGL_BGControlExit(dfp->bgl, GF_BGL_FRAME1_M );
				GF_BGL_BGControlExit(dfp->bgl, GF_BGL_FRAME1_S );
				sys_CreateHeap(HEAPID_BASE_APP,HEAPID_ITEMSHINKA,0x30000);
				dfp->param=BattleParam_Create(HEAPID_ITEMSHINKA,FIGHT_TYPE_1vs1);
				MSGMAN_GetStr(dfp->man_msg,DF_BOY_NAME+dfp->dsp.my_sex,name);
				Debug_MyStatus_Make(dfp->param->my_status[0],name,dfp->dsp.my_sex);
				PokeParaSet(dfp->pp,dfp->dsp.dpp[0].mons_no,dfp->dsp.dpp[0].level,0,RND_NO_SET,0,ID_NO_SET,0);
				PokeParaGetInfoSet(dfp->pp,dfp->param->my_status[0],ITEM_MONSUTAABOORU,NULL,NULL,HEAPID_ITEMSHINKA);
				for(i=0;i<4;i++){
					PokeWazaSetPos(dfp->pp,dfp->dsp.dpp[0].waza[i],i);
				}
				dfp->sw=ShinkaInit(dfp->pp,dfp->dsp.dpp[1].mons_no,dfp->param->config,NULL,
								   dfp->param->zw,SHINKA_STATUS_FLAG_CANCEL_ON|SHINKA_STATUS_FLAG_BATTLE_MODE,HEAPID_ITEMSHINKA);
				dfp->seq_no=SEQ_SHINKA_MAIN;
			}
		}
#endif
		break;
	case SEQ_SHINKA_MAIN:
#if 0
		if(ShinkaEndCheck(dfp->sw)==TRUE){
			ShinkaEnd(dfp->sw);
			BattleParam_Delete(dfp->param);
			sys_DeleteHeap(HEAPID_ITEMSHINKA);
			DebugFightBGCreate(dfp,dfp->bgl);
			DebugFightMainScreenCreate(dfp);
			if(dfp->cur_pos_m<=MS_ENEMY_POKE_6){
				DebugFightSubScreenCreate(dfp,&dfp->dsp.dpp[dfp->cur_pos_m],dfp->page);
			}
			CursorMove(dfp,MOVE_MAIN,dfp->cur_pos_m,CUR_PUT);
			dfp->seq_no=SEQ_MAIN_MENU_MOVE;
		}
#endif
		break;
	case SEQ_AI_FLAG_INIT:
		DebugFightAIScreenCreate(dfp);
		dfp->sub_seq_no=0;
		CursorMove(dfp,MOVE_AI,dfp->sub_seq_no,CUR_PUT);
		dfp->seq_no++;
		break;
	case SEQ_AI_FLAG_MAIN:
		if(sys.trg==PAD_KEY_UP){
			if(dfp->sub_seq_no>0){
				move=dfp->sub_seq_no-1;
			}
			else{
				move=DF_HARASS_AI-DF_BASIC_AI;
			}
		}
		else if(sys.trg==PAD_KEY_DOWN){
			if(dfp->sub_seq_no<(DF_HARASS_AI-DF_BASIC_AI)){
				move=dfp->sub_seq_no+1;
			}
			else{
				move=0;
			}
		}
		else if(sys.trg==PAD_BUTTON_A){
			dfp->dsp.ai_bit^=No2Bit(dfp->sub_seq_no);
			DebugFightAIScreenCreate(dfp);
			CursorMove(dfp,MOVE_AI,dfp->sub_seq_no,CUR_PUT);
		}
		else if(sys.trg==PAD_BUTTON_START){
			dfp->sub_seq_no=0;
			DebugFightMainScreenCreate(dfp);
			if(dfp->cur_pos_m<=MS_ENEMY_POKE_6){
				DebugFightSubScreenCreate(dfp,DebugFightDPPGet(dfp,dfp->mine,dfp->enemy,dfp->cur_pos_m),dfp->page);
			}
			CursorMove(dfp,MOVE_MAIN,dfp->cur_pos_m,CUR_PUT);
			dfp->seq_no=SEQ_MAIN_MENU_MOVE;
		}
		if(move!=NO_MOVE){
			CursorMove(dfp,MOVE_AI,move,CUR_PUT);
		}
		break;
	case SEQ_AUTO_FLAG_INIT:
		DebugFightAutoScreenCreate(dfp);
		dfp->sub_seq_no=0;
		CursorMove(dfp,MOVE_AI,dfp->sub_seq_no,CUR_PUT);
		dfp->seq_no++;
		break;
	case SEQ_AUTO_FLAG_MAIN:
		if(sys.trg==PAD_KEY_UP){
			if(dfp->sub_seq_no>0){
				move=dfp->sub_seq_no-1;
			}
			else{
				move=7;
			}
		}
		else if(sys.trg==PAD_KEY_DOWN){
			if(dfp->sub_seq_no<7){
				move=dfp->sub_seq_no+1;
			}
			else{
				move=0;
			}
		}
		else if(sys.trg==PAD_BUTTON_START){
			dfp->sub_seq_no=0;
			DebugFightMainScreenCreate(dfp);
			if(dfp->cur_pos_m<=MS_ENEMY_POKE_6){
				DebugFightSubScreenCreate(dfp,DebugFightDPPGet(dfp,dfp->mine,dfp->enemy,dfp->cur_pos_m),dfp->page);
			}
			CursorMove(dfp,MOVE_MAIN,dfp->cur_pos_m,CUR_PUT);
			dfp->seq_no=SEQ_MAIN_MENU_MOVE;
		}
		if(dfp->sub_seq_no<4){
			if(sys.trg==PAD_BUTTON_A){
				dfp->dsp.battle_status_flag^=No2Bit(dfp->sub_seq_no);
				DebugFightAutoScreenCreate(dfp);
				CursorMove(dfp,MOVE_AI,dfp->sub_seq_no,CUR_PUT);
			}
		}
		else{
			{
				int	*id;
				int	max;
				int	chg;

				chg=0;

				switch(dfp->sub_seq_no){
				case 4:
					id=&dfp->dsp.trainer1_id;
					max=927;
					break;
				case 5:
					id=&dfp->dsp.trainer2_id;
					max=927;
					break;
				case 6:
					id=&dfp->dsp.bt_trainer1_id;
					max=307;
					break;
				case 7:
					id=&dfp->dsp.bt_trainer2_id;
					max=307;
					break;
				}

				if(sys.trg==PAD_BUTTON_A){
					id[0]++;
					chg=1;
				}
				else if(sys.trg==PAD_BUTTON_B){
					id[0]--;
					chg=1;
				}
				else if(sys.trg==PAD_BUTTON_X){
					id[0]+=10;
					chg=1;
				}
				else if(sys.trg==PAD_BUTTON_Y){
					id[0]-=10;
					chg=1;
				}
				else if(sys.trg==PAD_BUTTON_R){
					id[0]+=100;
					chg=1;
				}
				else if(sys.trg==PAD_BUTTON_L){
					id[0]-=100;
					chg=1;
				}
				if(id[0]>max){
					id[0]=max;
				}
				if(id[0]<0){
					id[0]=0;
				}
				if(chg){
					DebugFightAutoScreenCreate(dfp);
					CursorMove(dfp,MOVE_AI,dfp->sub_seq_no,CUR_PUT);
				}
			}
		}
		if(move!=NO_MOVE){
			CursorMove(dfp,MOVE_AI,move,CUR_PUT);
		}
		break;
	case SEQ_REC_DATA_EDIT_INIT:
		DebugFightRecDataEditScreenCreate(dfp);
		dfp->seq_no++;
		break;
	case SEQ_REC_DATA_EDIT_MAIN:
		if((sys.trg==PAD_KEY_DOWN)&&(dfp->brdedit_client_no>0)){
			dfp->brdedit_client_no--;
			DebugFightRecDataEditScreenCreate(dfp);
		}
		if((sys.trg==PAD_KEY_UP)&&(dfp->brdedit_client_no<CLIENT_MAX-1)){
			dfp->brdedit_client_no++;
			DebugFightRecDataEditScreenCreate(dfp);
		}
		if(sys.trg==PAD_BUTTON_A){
			dfp->brdedit_pos=0;
			dfp->brdedit_page=0;
			DebugFightRecDataEdit2ScreenCreate(dfp,COL_ACTIVE);
			dfp->seq_no++;
		}
		if(sys.trg==PAD_BUTTON_SELECT){
			DebugFightBRDSave(dfp,1);
		}
		if(sys.trg==PAD_BUTTON_START){
			DebugFightBRDSave(dfp,0);
		}
		break;
	case SEQ_REC_DATA_EDIT_MAIN2:
		if((sys.trg==PAD_KEY_UP)&&(dfp->brdedit_pos>7)){
			dfp->brdedit_pos-=8;
			DebugFightRecDataEdit2ScreenCreate(dfp,COL_ACTIVE);
		}
		if((sys.trg==PAD_KEY_DOWN)&&(dfp->brdedit_pos<0x40-8-1)){
			dfp->brdedit_pos+=8;
			DebugFightRecDataEdit2ScreenCreate(dfp,COL_ACTIVE);
		}
		if((sys.trg==PAD_KEY_LEFT)&&(dfp->brdedit_pos>0)){
			dfp->brdedit_pos--;
			DebugFightRecDataEdit2ScreenCreate(dfp,COL_ACTIVE);
		}
		if((sys.trg==PAD_KEY_RIGHT)&&(dfp->brdedit_pos<0x40-1)){
			dfp->brdedit_pos++;
			DebugFightRecDataEdit2ScreenCreate(dfp,COL_ACTIVE);
		}
		if((sys.trg==PAD_BUTTON_L)&&(dfp->brdedit_page>0)){
			dfp->brdedit_page--;
			DebugFightRecDataEdit2ScreenCreate(dfp,COL_ACTIVE);
		}
		if((sys.trg==PAD_BUTTON_R)&&(dfp->brdedit_page<16-1)){
			dfp->brdedit_page++;
			DebugFightRecDataEdit2ScreenCreate(dfp,COL_ACTIVE);
		}
#if 0
		if(sys.trg==PAD_BUTTON_SELECT){
			dfp->brdedit_pos=0;
			dfp->brdedit_page=0;
			BattleRec_WorkInit(BattleRec_WorkAdrsGet());
			DebugFightRecDataEdit2ScreenCreate(dfp,COL_ACTIVE);
		}
#endif
		if(sys.trg==PAD_BUTTON_SELECT){
			DebugFightBRDSave(dfp,1);
		}
		if(sys.trg==PAD_BUTTON_START){
			DebugFightBRDSave(dfp,0);
		}
		if(sys.trg==PAD_BUTTON_B){
			dfp->seq_no=SEQ_REC_DATA_EDIT_INIT;
		}
		if(sys.trg==PAD_BUTTON_A){
			dfp->seq_no=SEQ_REC_DATA_EDIT_DATA;
			DebugFightRecDataEdit2ScreenCreate(dfp,COL_CURSOR);
		}
		break;
	case SEQ_REC_DATA_EDIT_DATA:
		{
			REC_DATA	data;

			data=BattleRec_DataPlay(dfp->brdedit_client_no,0x40*dfp->brdedit_page+dfp->brdedit_pos);

			if((sys.trg==PAD_KEY_DOWN)&&(data>0)){
				data--;
				BattleRec_DataRec(dfp->brdedit_client_no,0x40*dfp->brdedit_page+dfp->brdedit_pos,data);
				DebugFightRecDataEdit2ScreenCreate(dfp,COL_CURSOR);
			}
			if((sys.trg==PAD_KEY_UP)&&(data<99)){
				data++;
				BattleRec_DataRec(dfp->brdedit_client_no,0x40*dfp->brdedit_page+dfp->brdedit_pos,data);
				DebugFightRecDataEdit2ScreenCreate(dfp,COL_CURSOR);
			}
			if(sys.trg==PAD_KEY_LEFT){
				if(data>9){
					data-=10;
				}
				else{
					data=0;
				}
				BattleRec_DataRec(dfp->brdedit_client_no,0x40*dfp->brdedit_page+dfp->brdedit_pos,data);
				DebugFightRecDataEdit2ScreenCreate(dfp,COL_CURSOR);
			}
			if(sys.trg==PAD_KEY_RIGHT){
				if(data<90){
					data+=10;
				}
				else{
					data=99;
				}
				BattleRec_DataRec(dfp->brdedit_client_no,0x40*dfp->brdedit_page+dfp->brdedit_pos,data);
				DebugFightRecDataEdit2ScreenCreate(dfp,COL_CURSOR);
			}
			if(sys.trg==PAD_BUTTON_A){
				DebugFightRecDataEdit2ScreenCreate(dfp,COL_ACTIVE);
				dfp->seq_no=SEQ_REC_DATA_EDIT_MAIN2;
			}
		}
		break;
	case SEQ_COM_SELECT_EDIT_INIT:
		DebugFightComSelectEditScreenCreate(dfp,COL_ACTIVE);
		dfp->seq_no++;
		break;
	case SEQ_COM_SELECT_EDIT_MAIN:
		if((sys.trg==PAD_KEY_UP)&&(dfp->brdedit_pos>7)){
			dfp->brdedit_pos-=8;
			DebugFightComSelectEditScreenCreate(dfp,COL_ACTIVE);
		}
		if((sys.trg==PAD_KEY_DOWN)&&(dfp->brdedit_pos<0x40-8-1)){
			dfp->brdedit_pos+=8;
			DebugFightComSelectEditScreenCreate(dfp,COL_ACTIVE);
		}
		if((sys.trg==PAD_KEY_LEFT)&&(dfp->brdedit_pos>0)){
			dfp->brdedit_pos--;
			DebugFightComSelectEditScreenCreate(dfp,COL_ACTIVE);
		}
		if((sys.trg==PAD_KEY_RIGHT)&&(dfp->brdedit_pos<0x40-1)){
			dfp->brdedit_pos++;
			DebugFightComSelectEditScreenCreate(dfp,COL_ACTIVE);
		}
		if(sys.trg==PAD_BUTTON_B){
			DebugFightMainScreenCreate(dfp);
			if(dfp->cur_pos_m<=MS_ENEMY_POKE_6){
				DebugFightSubScreenCreate(dfp,DebugFightDPPGet(dfp,dfp->mine,dfp->enemy,dfp->cur_pos_m),dfp->page);
			}
			CursorMove(dfp,MOVE_MAIN,dfp->cur_pos_m,CUR_PUT);
			dfp->seq_no=SEQ_MAIN_MENU_MOVE;
		}
		if(sys.trg==PAD_BUTTON_A){
			dfp->seq_no=SEQ_COM_SELECT_EDIT_DATA;
			DebugFightComSelectEditScreenCreate(dfp,COL_CURSOR);
		}
		break;
	case SEQ_COM_SELECT_EDIT_DATA:
		{
			u8	*data=&dfp->dsp.command_buffer[dfp->brdedit_pos];

			if((sys.trg==PAD_KEY_DOWN)&&(data[0]>0)){
				data[0]--;
				DebugFightComSelectEditScreenCreate(dfp,COL_CURSOR);
			}
			if((sys.trg==PAD_KEY_UP)&&(data[0]<99)){
				data[0]++;
				DebugFightComSelectEditScreenCreate(dfp,COL_CURSOR);
			}
			if(sys.trg==PAD_KEY_LEFT){
				if(data[0]>9){
					data[0]-=10;
				}
				else{
					data[0]=0;
				}
				DebugFightComSelectEditScreenCreate(dfp,COL_CURSOR);
			}
			if(sys.trg==PAD_KEY_RIGHT){
				if(data[0]<90){
					data[0]+=10;
				}
				else{
					data[0]=99;
				}
				DebugFightComSelectEditScreenCreate(dfp,COL_CURSOR);
			}
			if(sys.trg==PAD_BUTTON_A){
				DebugFightComSelectEditScreenCreate(dfp,COL_ACTIVE);
				dfp->seq_no=SEQ_COM_SELECT_EDIT_MAIN;
			}
		}
		break;
	}
}

static	void	DebugFightBRDSave(DEBUG_FIGHT_PARAM *dfp,int flag)
{
	if(flag){
		DebugFightBRDSaveActMain(dfp);
	}
	else{
		DebugFightBRDSaveAct(dfp,NULL);
	}

	DebugFightMainScreenCreate(dfp);
	if(dfp->cur_pos_m<=MS_ENEMY_POKE_6){
		DebugFightSubScreenCreate(dfp,DebugFightDPPGet(dfp,dfp->mine,dfp->enemy,dfp->cur_pos_m),dfp->page);
	}
	CursorMove(dfp,MOVE_MAIN,dfp->cur_pos_m,CUR_PUT);
	dfp->seq_no=SEQ_MAIN_MENU_MOVE;
}

static	void	DebugFightSioConnect(DEBUG_FIGHT_PARAM *dfp)
{
	if(dfp->param->battle_status_flag&BATTLE_STATUS_FLAG_REC_BATTLE){
		return;
	}

	switch(dfp->dsp.fight_type){
	case FT_1vs1_SIO_P:
		CommStateDBattleConnect(TRUE,COMM_MODE_BATTLE_SINGLE,dfp->sv);
		break;
	case FT_2vs2_SIO_P:
		CommStateDBattleConnect(TRUE,COMM_MODE_BATTLE_DOUBLE,dfp->sv);
		break;
	case FT_MULTI_SIO_P:
		CommStateDBattleConnect(TRUE,COMM_MODE_BATTLE_MULTI,dfp->sv);
		break;
	case FT_1vs1_WIFI_P:
		CommStateDBattleConnect(TRUE,COMM_MODE_BATTLE_SINGLE_WIFI,dfp->sv);
		break;
	case FT_BT_MULTI_SIO_P:
		CommStateDBattleConnect(TRUE,COMM_MODE_TOWER_MULTI,dfp->sv);
		break;
	case FT_1vs1_SIO_C:
		CommStateDBattleConnect(FALSE,COMM_MODE_BATTLE_SINGLE,dfp->sv);
		break;
	case FT_2vs2_SIO_C:
		CommStateDBattleConnect(FALSE,COMM_MODE_BATTLE_DOUBLE,dfp->sv);
		break;
	case FT_MULTI_SIO_C:
		CommStateDBattleConnect(FALSE,COMM_MODE_BATTLE_MULTI,dfp->sv);
		break;
	case FT_1vs1_WIFI_C:
		CommStateDBattleConnect(FALSE,COMM_MODE_BATTLE_SINGLE_WIFI,dfp->sv);
		break;
	case FT_BT_MULTI_SIO_C:
		CommStateDBattleConnect(FALSE,COMM_MODE_TOWER_MULTI,dfp->sv);
		break;
	default:
		break;
	}
}

//==============================================================================================
//
//	DEBUG_POKEMON_PARAMを取得
//
//==============================================================================================
DEBUG_POKEMON_PARAM	*DebugFightDPPGet(DEBUG_FIGHT_PARAM *dfp,int mine,int enemy,int pos)
{
	int	no;

	if(pos<MS_ENEMY_POKE_1){
		no=mine*2;
	}
	else{
		no=enemy*2+1;
	}

	pos=pos%6;

	return &dfp->dsp.dpp[no][pos];
}

//==============================================================================================
//
//	タイトルデバックメニューから呼ばれるように追加
//
//==============================================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
static	PROC_RESULT DebugFightProc_Init(PROC * proc, int * seq)
{
	DEBUG_FIGHT_PARAM	*dfp;
	LOAD_RESULT			result;

	sys_CreateHeap(HEAPID_BASE_APP,HEAPID_DEBUG_APPLICATION,CREATE_HEAP_SIZE);
	sys_CreateHeap(HEAPID_BASE_APP,HEAPID_WORLD,0x1c000);
	sys_CreateHeap( HEAPID_BASE_SYSTEM, HEAPID_COMMICON, 0x300 );
	dfp=PROC_AllocWork(proc,sizeof(DEBUG_FIGHT_PARAM),HEAPID_DEBUG_APPLICATION);
	Overlay_Load(FS_OVERLAY_ID(debug_fight), OVERLAY_LOAD_NOT_SYNCHRONIZE);
	InitDebugFight(dfp);
	dfp->sv=((MAINWORK*)PROC_GetParentWork(proc))->savedata;
	dsp_p=&dfp->dsp;
	BattleRec_Init(dfp->sv,HEAPID_WORLD,&result);
//	GF_ASSERT(result != LOAD_RESULT_NG);

	return PROC_RES_FINISH;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static	PROC_RESULT DebugFightProc_Main(PROC * proc, int * seq)
{
	DEBUG_FIGHT_PARAM *dfp = PROC_GetWork(proc);

	MainDebugFight(dfp);

	if (dfp->end_flag) {
		return PROC_RES_FINISH;
	} else {
		return PROC_RES_CONTINUE;
	}
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static	PROC_RESULT DebugFightProc_End(PROC * proc, int * seq)
{
	PROC_FreeWork(proc);
	sys_DeleteHeap( HEAPID_COMMICON );
	sys_DeleteHeap(HEAPID_DEBUG_APPLICATION);
	Main_SetNextProc(NO_OVERLAY_ID, &TitleProcData);
	OS_ResetSystem(0);									//ソフトリセット
	return PROC_RES_FINISH;
}

//--------------------------------------------------------------
//--------------------------------------------------------------

const PROC_DATA DebugFightProcData = {
	DebugFightProc_Init,
	DebugFightProc_Main,
	DebugFightProc_End,
	NO_OVERLAY_ID,
};

//------------------------------------------------------------------
/**
 * @brief	プロセス関数：初期化：戦闘（テスト）
 */
//------------------------------------------------------------------
static PROC_RESULT TestBattleProc_Init(PROC * proc, int * seq)
{
	return PROC_RES_FINISH;
}
//------------------------------------------------------------------
/**
 * @brief	プロセス関数：メイン：戦闘（テスト）
 */
//------------------------------------------------------------------
static PROC_RESULT TestBattleProc_Main(PROC * proc, int * seq)
{
	if (BattleSystemProc(proc,seq)) {
		return PROC_RES_FINISH;
	} else {
		return PROC_RES_CONTINUE;
	}
}
//------------------------------------------------------------------
/**
 * @brief	プロセス関数：終了：戦闘（テスト）
 */
//------------------------------------------------------------------
static PROC_RESULT TestBattleProc_End(PROC * proc, int * seq)
{
	return PROC_RES_FINISH;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static	void DebugFight_SetProc(DEBUG_FIGHT_PARAM *dfp)
{
	FS_EXTERN_OVERLAY(battle);

static const PROC_DATA TestBattleProcData = {
	TestBattleProc_Init,
	TestBattleProc_Main,
	TestBattleProc_End,
	FS_OVERLAY_ID(battle),
};
	dfp->proc=PROC_Create(&TestBattleProcData, dfp->param, HEAPID_DEBUG_APPLICATION);
}

int		DebugFightCommandBufferGet(void)
{
	int	ret=0;

	if(dsp_p->cb_pos<64){
		ret=dsp_p->command_buffer[dsp_p->cb_pos++];
	}

	return ret;
}

#if 0
//--------------------------------------------------------------
/**
 * @brief   
 *
 * ProcDataで指定した以外のオーバーレイのロードを行う
 *
 */
//--------------------------------------------------------------
static void NitroStaticInit(void)
{
	Overlay_Load(FS_OVERLAY_ID(debug_fight), OVERLAY_LOAD_NOT_SYNCHRONIZE);
}
#endif

#endif	//PM_DEBUG

//==============================================================================
/**
 * @file	tower_tool.c
 * @brief	バトルタワー関連ツール類
 * @author	nohara
 * @date	2007.05.28
 *
 * b_tower_fld.cの処理を移動
 *
 * 関連ソース	field/scr_btower.c		常駐
 *				field/b_tower_scr.c		フィールドサブ
 *				field/b_tower_ev.c		常駐
 *				field/b_tower_wifi.c	常駐
 *				field/b_tower_fld.c		フィールドサブ
 */
//==============================================================================
#include "common.h"
#include "system/procsys.h"
#include "system/pm_str.h"
#include "poketool/poke_tool.h"
#include "poketool/status_rcv.h"
#include "savedata/savedata_def.h"
#include "savedata/savedata.h"
#include "savedata/zukanwork.h"
#include "savedata/perapvoice.h"
#include "savedata/record.h"
#include "system/procsys.h"
#include "system/pm_str.h"
#include "gflib/strbuf_family.h"
#include "poketool/poke_tool.h"
#include "application/p_status.h"
#include "application/pokelist.h"
#include "field/fieldsys.h"
#include "field/field_event.h"
#include "field/encount_set.h"
#include "field/ev_time.h"
#include "field/fieldobj_code.h"
#include "msgdata/msg.naix"
//通信
#include "communication/comm_tool.h"
#include "communication/comm_system.h"
#include "communication/comm_def.h"
#include "communication/comm_info.h"
//フロンティアシステム
#include "frontier_tool.h"
#include "frontier_def.h"
//バトルタワー
#include "../field/b_tower_scr_def.h"
#include "../field/b_tower_ev.h"
//#include "../field/b_tower_fld.h"
#include "savedata/b_tower_local.h"
#include "tower_tool.h"
//正式データが出来るまでインクルード
#include "battle/attr_def.h"
#include "itemtool/itemsym.h"
#include "battle/wazano_def.h"
#include "battle/battle_common.h"
#include "battle/trtype_def.h"
#include "battle/b_tower_data.h"
#include "battle/trno_def.h"
#include "../field/b_tower_fld.dat"


//==============================================================================
//	extern宣言
//==============================================================================
extern void	BattleParam_TrainerDataMake(BATTLE_PARAM *bp);


//==============================================================================
//	プロトタイプ宣言
//==============================================================================
BOOL FSRomBattleTowerTrainerDataMake(BTOWER_SCRWORK* wk,B_TOWER_PARTNER_DATA *tr_data,u16 tr_no,int cnt,u16 *set_poke_no,u16 *set_item_no,B_TOWER_PAREPOKE_PARAM* poke,int heapID);
void btltower_BtlPartnerSelectWifi(SAVEDATA* sv,B_TOWER_PARTNER_DATA* wk,const u8 round);
BATTLE_PARAM* BtlTower_CreateBattleParam( BTOWER_SCRWORK* wk, FRONTIER_EX_PARAM* ex_param );
//void BTowerComm_SendPlayerData(BTOWER_SCRWORK* wk,SAVEDATA *sv);
//void BTowerComm_SendTrainerData(BTOWER_SCRWORK* wk);
void FSBTowerComm_SendRetireSelect(BTOWER_SCRWORK* wk,u16 retire);
//u16 BTowerComm_RecvPlayerData(BTOWER_SCRWORK* wk,const u16* recv_buf);
//u16 BTowerComm_RecvTrainerData(BTOWER_SCRWORK* wk,const u16* recv_buf);
u16	FSBTowerComm_RecvRetireSelect(BTOWER_SCRWORK* wk,const u16* recv_buf);
int BtlTower_GetPokeArc( u8 play_mode );
int BtlTower_GetTrArc( u8 play_mode );
int BtlTower_GetTrMsgArc( u8 play_mode );

static BOOL BattleTowerPokemonSetAct(BTOWER_SCRWORK* wk,B_TOWER_TRAINER_ROM_DATA *trd,
		u16 tr_no,B_TOWER_POKEMON *pwd,u8 cnt,
		u16 *set_poke_no,u16 *set_item_no,B_TOWER_PAREPOKE_PARAM* poke,int heapID);
static void btltower_BtlPartnerSelectSD(B_TOWER_PARTNER_DATA* wk,const u8 round);
static u32 BattleTowerPokemonParamMake(BTOWER_SCRWORK* wk,B_TOWER_POKEMON* pwd,
		u16 poke_no,u32 poke_id,u32 poke_rnd,u8 pow_rnd,u8 mem_idx,BOOL itemfix,int heapID);
static u32 btower_GetFightType(u8 play_mode);
static void	BattleTowerTrainerDataMake(BATTLE_PARAM *bp,B_TOWER_PARTNER_DATA *tr_data,int cnt,int client_no,int heapID);
static int BtlTower_GetArcVersion( u8 play_mode );


//==============================================================================
//
//	frontier_tool.cに似た処理があるのであとで調整する
//	
//==============================================================================

//------------------------------------------------------------------------------
/**
 *	バトルタワートレーナーデータ生成（ロムデータをB_TOWER_PARTNER_DATA構造体に展開）
 *
 * @param[in/out]	tr_data		生成するB_TOWER_PARTNAER_DATA構造体
 * @param[in]		tr_no		生成元になるトレーナーID
 * @param[in]		cnt			トレーナーに持たせるポケモンの数
 * @param[in]		set_poke_no	ペアを組んでいるトレーナーの持ちポケモン（NULLだとチェックなし）
 * @param[in]		set_item_no	ペアを組んでいるトレーナーの持ちポケモンの装備アイテム
 *								（NULLだとチェックなし）
 * @param[in/out]	poke		抽選されたポケモンの二体のパラメータを
 *								格納して返す構造体型データへのポインタ(NULLだとチェックなし）
 * @param[in]		heapID		ヒープID
 *
 * @retval	FALSE	抽選ループが50回以内で終わった
 * @retval	TRUE	抽選ループが50回以内で終わらなかった
 *
 * ★似た処理 b_tower_fld.c RomBattleTowerTrainerDataMake
 */
//------------------------------------------------------------------------------
BOOL FSRomBattleTowerTrainerDataMake(BTOWER_SCRWORK* wk,B_TOWER_PARTNER_DATA *tr_data,u16 tr_no,int cnt,u16 *set_poke_no,u16 *set_item_no,B_TOWER_PAREPOKE_PARAM* poke,int heapID)
{
	BOOL			ret = 0;
	B_TOWER_TRAINER_ROM_DATA	*trd;
	
	//トレーナーデータセット
	trd = Frontier_TrainerDataGet(	&tr_data->bt_trd, tr_no, heapID, 
									BtlTower_GetTrArc(wk->play_mode) );

	//ポケモンデータをセット
	ret = BattleTowerPokemonSetAct(wk,trd,tr_no,&tr_data->btpwd[0],cnt,set_poke_no,set_item_no,poke,heapID);
	
	sys_FreeMemoryEz(trd);
	return ret;
}


//============================================================================================
/**
 *	バトルタワーのポケモンを決める
 *
 * @param[in]		trd			トレーナーデータ
 * @param[in]		tr_no		トレーナーナンバー
 * @param[in/out]	pwd			B_TOWER_POKEMON構造体
 * @param[in]		cnt			トレーナーに持たせるポケモンの数
 * @param[in]		set_poke_no	ペアを組んでいるトレーナーの持ちポケモン（NULLだとチェックなし）
 * @param[in]		set_item_no	ペアを組んでいるトレーナーの持ちポケモンの装備アイテム
 *								（NULLだとチェックなし）
 * @param[in/out]	poke		抽選されたポケモンの二体のパラメータを
 *								格納して返す構造体型データへのポインタ(NULLだとチェックなし）
 * @param[in]		heapID		ヒープID
 *
 * @retval	FALSE	抽選ループが50回以内で終わった
 * @retval	TRUE	抽選ループが50回以内で終わらなかった
 */
//============================================================================================
static BOOL BattleTowerPokemonSetAct(BTOWER_SCRWORK* wk,B_TOWER_TRAINER_ROM_DATA *trd,
		u16 tr_no,B_TOWER_POKEMON *pwd,u8 cnt,
		u16 *set_poke_no,u16 *set_item_no,B_TOWER_PAREPOKE_PARAM* poke,int heapID)
{
	int	i,j;
	u8	pow_rnd;
	u8	poke_index;
	u32	id;
	int	set_index;
	int	set_index_no[4];
	u32	set_rnd_no[4];
	int	set_count;
	int	loop_count;
	BOOL	ret = 0;
	B_TOWER_POKEMON_ROM_DATA	prd_s;
	B_TOWER_POKEMON_ROM_DATA	prd_d;

	//手持ちポケモンのMAXは4体まで
	GF_ASSERT(cnt<=4);

	set_count=0;
	loop_count=0;
	while(set_count!=cnt){
//		poke_index = gf_rand()%trd->use_poke_cnt;
		poke_index = btower_rand(wk)%trd->use_poke_cnt;
		set_index=trd->use_poke_table[poke_index];
		Frontier_PokemonRomDataGet(&prd_d,set_index,BtlTower_GetPokeArc(wk->play_mode));

		//モンスターナンバーのチェック（同一のポケモンは持たない）
		for(i=0;i<set_count;i++){
			Frontier_PokemonRomDataGet(&prd_s,set_index_no[i],BtlTower_GetPokeArc(wk->play_mode));
			if(prd_s.mons_no==prd_d.mons_no){
				break;
			}
		}
		if(i!=set_count){
			continue;
		}

		//ペアを組んでいるトレーナーの持ちポケモンとのチェック
		if(set_poke_no!=NULL){
			//モンスターナンバーのチェック（同一のポケモンは持たない）
			for(i=0;i<cnt;i++){
				if(set_poke_no[i]==prd_d.mons_no){
					break;
				}
			}
			if(i!=cnt){
				continue;
			}
		}

		//50回まわして、決まらないようなら、同一アイテムチェックはしない
		if(loop_count<50){
			//装備アイテムのチェック（同一のアイテムは持たない）
			for(i=0;i<set_count;i++){
				Frontier_PokemonRomDataGet(	&prd_s,set_index_no[i],
											BtlTower_GetPokeArc(wk->play_mode));
				if((prd_s.item_no)&&(prd_s.item_no==prd_d.item_no)){
					break;
				}
			}
			if(i!=set_count){
				loop_count++;
				continue;
			}
			//ペアを組んでいるトレーナーの持ちポケモンの装備アイテムとのチェック
			if(set_item_no!=NULL){
				//装備アイテムのチェック（同一のアイテムは持たない）
				for(i=0;i<cnt;i++){
					if((set_item_no[i]==prd_d.item_no) && (set_item_no[i]!=0)){
						break;
					}
				}
				if(i!=cnt){
					loop_count++;
					continue;
				}
			}
		}

		set_index_no[set_count]=set_index;
		set_count++;
	}

	pow_rnd=BattleTowerPowRndGet(tr_no);
//	id=(gf_rand()|(gf_rand()<<16));
	id=(btower_rand(wk)|(btower_rand(wk)<<16));

	if(loop_count >= 50){
		ret = TRUE;
	}
	for(i=0;i<set_count;i++){
		set_rnd_no[i] = BattleTowerPokemonParamMake(wk,&(pwd[i]),
			set_index_no[i],id,0,pow_rnd,i,ret,heapID);
	}
	if(poke == NULL){
		return ret;
	}
	//ポインタがNULLでなければ、抽選されたポケモンの必要なパラメータを返す
	poke->poke_id = id;
	for(i = 0;i< BTOWER_STOCK_PAREPOKE_MAX;i++){
		poke->poke_no[i] = set_index_no[i];
		poke->poke_rnd[i] = set_rnd_no[i];
	}
	return ret;
}

//--------------------------------------------------------------
/**
 *	@brief	Wifi トレーナー抽選
 */
//--------------------------------------------------------------
void btltower_BtlPartnerSelectWifi(SAVEDATA* sv,B_TOWER_PARTNER_DATA* wk,const u8 round)
{
	int i;
	
	BTLTOWER_WIFI_DATA*	wifiSave;
	const B_TOWER_TRAINER *tr_src;
	const B_TOWER_POKEMON *poke_src;

	MI_CpuClear8(wk,sizeof(B_TOWER_PARTNER_DATA));
	
	wifiSave = SaveData_GetTowerWifiData(sv);

	if(!TowerWifiData_IsPlayerDataEnable(wifiSave)){
		//データがない(ROMから無理矢理抽選)
		btltower_BtlPartnerSelectSD(wk,round);
		return;
	}

	//セーブデータから引っ張る
	TowerWifiData_GetBtlPlayerData(wifiSave,wk,round);
}

//--------------------------------------------------------------
/**
 *	@brief	シングルダブル トレーナー抽選結果取得
 *	
 * @param[in/out]	tr_data		生成するB_TOWER_PARTNAER_DATA構造体
 * @param[in]		tr_no		生成元になるトレーナーID
 * @param[in]		cnt			トレーナーに持たせるポケモンの数
 * @param[in]		heapID		ヒープID
 */
//--------------------------------------------------------------
static void btltower_BtlPartnerSelectSD(B_TOWER_PARTNER_DATA* wk,const u8 round)
{
	int i;

	const B_TOWER_TRAINER *tr_src;
	const B_TOWER_POKEMON *poke_src;
	const B_TOWER_DMYENEMY	*poke_idx;

	MI_CpuClear8(wk,sizeof(B_TOWER_PARTNER_DATA));

	//本当はROM/RAMデータから引っ張ってくる
	tr_src = &(b_tower_trainer_data[round]);
	poke_src = b_tower_poke_data;
	poke_idx = &(b_tower_dmy_enemy[round]);

	//トレーナーデータコピー
	MI_CpuCopy8(tr_src,&wk->bt_trd,sizeof(B_TOWER_TRAINER));
	wk->bt_trd.dummy = poke_idx->obj_code;

	//ポケモンデータコピー
	for(i = 0;i < 4;i++){
		MI_CpuCopy8(&(poke_src[poke_idx->pokeno[i]]),&(wk->btpwd[i]),sizeof(B_TOWER_POKEMON));
	}
}

//持ちポケモン決定はランダムでしているが無限ループ防止のため、
//ある程度まわしたら、ポケモンの不一致のみをチェックして、
//アイテムを固定で持たせるためのアイテムテーブル
static const u16 BattleTowerPokemonItem[]={
	ITEM_HIKARINOKONA,
	ITEM_RAMUNOMI,
	ITEM_TABENOKOSI,
	ITEM_SENSEINOTUME,
};
//============================================================================================
/**
 *	@brief	バトルタワーのポケモンパラメータ生成
 *
 *	@param[in/out]	pwd	ポケモンパラメータの展開場所
 *	@param[in]		poke_no	タワーromデータポケモンナンバー
 *	@param[in]		poke_id	ポケモンにセットするid
 *	@param[in]		poke_rnd	ポケモンにセットする個性乱数(0が引き渡されたら関数内で生成)
 *	@param[in]		pow_rnd	ポケモンにセットするpow_rnd値
 *	@param[in]		mem_idx	メンバーindex。一体目or二体目
 *	@param[in]		itemfix	TRUEなら固定アイテム。FALSEならromデータのアイテム
 *	@param[in]		heapID	テンポラリメモリを確保するヒープID
 *
 *	@return	personal_rnd:生成されたポケモンの個性乱数値
 *
 */
//============================================================================================
static u32 BattleTowerPokemonParamMake(BTOWER_SCRWORK* wk,B_TOWER_POKEMON* pwd,
		u16 poke_no,u32 poke_id,u32 poke_rnd,u8 pow_rnd,u8 mem_idx,BOOL itemfix,int heapID)
{
	int i;
	int	exp;
	u32	personal_rnd;
	u8	friend;
	B_TOWER_POKEMON_ROM_DATA	prd_s;
	
	MI_CpuClear8(pwd,sizeof(B_TOWER_POKEMON));
	
	//ロムデータロード
	Frontier_PokemonRomDataGet(&prd_s,poke_no,BtlTower_GetPokeArc(wk->play_mode));
	
	//モンスターナンバー
	pwd->mons_no=prd_s.mons_no;

	//フォルムナンバー
	pwd->form_no=prd_s.form_no;

	//装備道具
	if(itemfix){
		//50回以上まわしていたフラグがTUREなら、装備アイテムは固定のものを持たせる
		pwd->item_no=BattleTowerPokemonItem[mem_idx];
	}else{
		//romデータに登録されたアイテムを持たせる
		pwd->item_no=prd_s.item_no;
	}

	//なつき度は255がデフォルト
	friend=255;
	for(i=0;i<WAZA_TEMOTI_MAX;i++){
		pwd->waza[i]=prd_s.waza[i];
		//やつあたりを持っているときは、なつき度を0にする
		if(prd_s.waza[i]==WAZANO_YATUATARI){
			friend=0;
		}
	}

	//IDNo
	pwd->id_no=poke_id;

	if(poke_rnd == 0){
		//個性乱数
		do{
//			personal_rnd=(gf_rand()|gf_rand()<<16);
			personal_rnd=(btower_rand(wk)|btower_rand(wk)<<16);
#if 0
		}while((prd_s.chr!=PokeSeikakuGetRnd(personal_rnd))&&(PokeRareGetPara(poke_id,personal_rnd)==TRUE));
#else
		//プラチナはタワーも修正する(08.03.17)(似た処理がfrontier_tool.cにもあるので注意！)
		//データの性格と一致していない"もしくは"レアの時は、ループを回す
		}while((prd_s.chr!=PokeSeikakuGetRnd(personal_rnd))||(PokeRareGetPara(poke_id,personal_rnd)==TRUE));
#endif

		//OS_Printf( "決定したpersonal_rnd = %d\n", personal_rnd );
		//OS_Printf( "PokeSeikakuGetRnd = %d\n", PokeSeikakuGetRnd(personal_rnd) );
		//OS_Printf( "レアじゃないか = %d\n", PokeRareGetPara(poke_id,personal_rnd) );
		pwd->personal_rnd=personal_rnd;
	}else{
		pwd->personal_rnd = poke_rnd;	//0でなければ引数の値を使用
		personal_rnd = poke_rnd;
	}
	
		
	//パワー乱数
	pwd->hp_rnd=pow_rnd;
	pwd->pow_rnd=pow_rnd;
	pwd->def_rnd=pow_rnd;
	pwd->agi_rnd=pow_rnd;
	pwd->spepow_rnd=pow_rnd;
	pwd->spedef_rnd=pow_rnd;

	//努力値
	exp=0;
	for(i=0;i<6;i++){
		if(prd_s.exp_bit&No2Bit(i)){
			exp++;
		}
	}
	if((PARA_EXP_TOTAL_MAX/exp)>255){
		exp=255;
	}else{
		exp=PARA_EXP_TOTAL_MAX/exp;
	}
	for(i = 0;i < 6;i++){
		if(prd_s.exp_bit&No2Bit(i)){
			pwd->exp[i]=exp;
		}
	}

	//技ポイント
	pwd->pp_count=0;

	//国コード
	pwd->country_code=CasetteLanguage;

	//特性
	i=PokePersonalParaGet(pwd->mons_no,ID_PER_speabi2);
	if(i){
		if(pwd->personal_rnd&1){
			pwd->tokusei=i;
		}else{
			pwd->tokusei=PokePersonalParaGet(pwd->mons_no,ID_PER_speabi1);
		}
	}else{
		pwd->tokusei=PokePersonalParaGet(pwd->mons_no,ID_PER_speabi1);
	}

	//なつき度
	pwd->natuki=friend;

	//ニックネーム
	MSGDAT_MonsNameGet(pwd->mons_no,heapID,&(pwd->nickname[0]));

	return personal_rnd;
}

//--------------------------------------------------------------
/**
 * @brief	バトルタワー用　バトルパラメータ生成
 */
//--------------------------------------------------------------
BATTLE_PARAM* BtlTower_CreateBattleParam( BTOWER_SCRWORK* wk, FRONTIER_EX_PARAM* ex_param )
{
	int i;
	u8	val8;
	u32	val32;
	BATTLE_PARAM *bp;
	SAVEDATA* sv;
	POKEPARTY* party;
	POKEMON_PARAM *pp;

	bp = BattleParam_Create(wk->heapID,btower_GetFightType(wk->play_mode));
	
	sv = ex_param->savedata;
	party = SaveData_GetTemotiPokemon(sv);

	BattleParam_SetParamByGameDataCore( bp, NULL, 
										ex_param->savedata,
										ex_param->zone_id,
										ex_param->fnote_data,
										ex_param->bag_cursor,
										ex_param->battle_cursor);

	bp->bg_id = BG_ID_TOWER;			//基本背景指定
	bp->ground_id = GROUND_ID_TOWER;	//基本地面指定

	//ポケモンデータセット
	pp = PokemonParam_AllocWork(wk->heapID);
	
	//選んだ手持ちポケモンをセット
	val8 = 50;
	PokeParty_Init(bp->poke_party[POKEPARTY_MINE],wk->member_num);
	for(i = 0;i < wk->member_num;i++){
		PokeCopyPPtoPP(PokeParty_GetMemberPointer(party,wk->member[i]),pp);

		//レベル調整
		if(PokeParaGet(pp,ID_PARA_level,NULL) > val8){
			val32 = PokeLevelExpGet(PokeParaGet(pp,ID_PARA_monsno,NULL),val8);

			PokeParaPut(pp,ID_PARA_exp,&val32);
			PokeParaCalc(pp);
		}
		BattleParam_AddPokemon(bp,pp,POKEPARTY_MINE);
	}
	sys_FreeMemoryEz(pp);

	//トレーナーデータ生成（自分側）
	BattleParam_TrainerDataMake(bp);

	//トレーナーデータ(enemy1)をセット
	BattleTowerTrainerDataMake(bp,&(wk->tr_data[0]),wk->member_num,CLIENT_NO_ENEMY,wk->heapID);

	//AIセット
	for( i=0; i < CLIENT_MAX ;i++ ){
		bp->trainer_data[i].aibit = FR_AI_EXPERT;
	}
	OS_Printf( "トレーナーのAI = %d\n", FR_AI_EXPERT );

	switch(wk->play_mode){
	case BTWR_MODE_MULTI:
		//ペアデータをセット
		BattleTowerTrainerDataMake(bp,&(wk->five_data[wk->partner]),wk->member_num,CLIENT_NO_MINE2,wk->heapID);
		//↓ここは共通処理で流れていい
	case BTWR_MODE_COMM_MULTI:
	case BTWR_MODE_WIFI_MULTI:
		//トレーナーデータ(enemy2)をセット
		BattleTowerTrainerDataMake(bp,&(wk->tr_data[1]),wk->member_num,CLIENT_NO_ENEMY2,wk->heapID);
		break;
	default:
		break;
	}
	return bp;
}

//============================================================================================
/**
 *	バトルタワートレーナーデータ生成
 *
 * @param[in/out]	bp			生成するBATTLE_PARAM構造体
 * @param[in]		tr_data		生成元になるトレーナーデータ
 * @param[in]		cnt			トレーナーの持ちポケモン数
 * @param[in]		client_no	生成するclient_no
 * @param[in]		heapID		ヒープID（POKEMON_PARAMの生成に必要）
 */
//============================================================================================
static void	BattleTowerTrainerDataMake(BATTLE_PARAM *bp,B_TOWER_PARTNER_DATA *tr_data,int cnt,int client_no,int heapID)
{
	int				i,j;
	//PMS_DATA		*pd;
	POKEMON_PARAM	*pp;

#if 0
	//トレーナーIDをセット
	bp->trainer_id[client_no]=tr_data->bt_trd.player_id;

	//トレーナーデータをセット
	bp->trainer_data[client_no].tr_type=tr_data->bt_trd.tr_type;
	PM_strcpy(&bp->trainer_data[client_no].name[0],&tr_data->bt_trd.name[0]);
	pd=(PMS_DATA *)&tr_data->bt_trd.win_word[0];
	bp->trainer_data[client_no].win_word=*pd;
	pd=(PMS_DATA *)&tr_data->bt_trd.lose_word[0];
	bp->trainer_data[client_no].lose_word=*pd;
#else
	BattleParamTrainerDataSet( bp, &tr_data->bt_trd, cnt, client_no, heapID );
#endif

	//ポケモンデータをセット
	pp=PokemonParam_AllocWork(heapID);
	for(i=0;i<cnt;i++){
		Frontier_PokeParaMake(&tr_data->btpwd[i],pp,FRONTIER_LEVEL_50);
		PokeParty_Add(bp->poke_party[client_no],pp);
	}

	sys_FreeMemoryEz(pp);
}

//--------------------------------------------------------------
/**
 * @brief	バトルタワー　プレイモードからFIGHT_TYPEを返す
 */
//--------------------------------------------------------------
static u32 btower_GetFightType(u8 play_mode)
{
	switch(play_mode){
	case BTWR_MODE_SINGLE:
	case BTWR_MODE_WIFI:
		return FIGHT_TYPE_BATTLE_TOWER_1vs1;
	case BTWR_MODE_DOUBLE:
		return FIGHT_TYPE_BATTLE_TOWER_2vs2;
	case BTWR_MODE_MULTI:
		return FIGHT_TYPE_BATTLE_TOWER_AI_MULTI;	
	case BTWR_MODE_COMM_MULTI:
	case BTWR_MODE_WIFI_MULTI:
		return FIGHT_TYPE_BATTLE_TOWER_SIO_MULTI;
	}
	return FIGHT_TYPE_BATTLE_TOWER_1vs1;
}

#if 0
//--------------------------------------------------------------
/**
 *	@brief	バトルタワー　自機性別とモンスターNoを送信
 */
//--------------------------------------------------------------
void BTowerComm_SendPlayerData(BTOWER_SCRWORK* wk,SAVEDATA *sv)
{
	int i;
	POKEPARTY* party;
	POKEMON_PARAM *pp;
	MYSTATUS	*my = SaveData_GetMyStatus(sv);
	
	wk->send_buf[0] = MyStatus_GetMySex(my);
	party = SaveData_GetTemotiPokemon(sv);
	for(i = 0;i < 2;i++){
		wk->send_buf[1+i] =
			PokeParaGet(PokeParty_GetMemberPointer(party,wk->member[i]),
						ID_PARA_monsno,NULL);
	}
	wk->send_buf[3] = 
		TowerScoreData_SetStage(wk->scoreSave,BTWR_MODE_COMM_MULTI,BTWR_DATA_get);
}

//--------------------------------------------------------------
/**
 *	@brief	バトルタワー　通信マルチ　抽選したトレーナーNoを子機に送信
 */
//--------------------------------------------------------------
void BTowerComm_SendTrainerData(BTOWER_SCRWORK* wk)
{
	MI_CpuCopy8(wk->trainer,wk->send_buf,BTOWER_STOCK_TRAINER_MAX*2);
}
#endif

//--------------------------------------------------------------
/**
 *	@brief	バトルタワー　通信マルチ　リタイアするかどうかを互いに送信
 *
 *	@param	retire	TRUEならリタイア
 */
//--------------------------------------------------------------
void FSBTowerComm_SendRetireSelect(BTOWER_SCRWORK* wk,u16 retire)
{
	//自分の選択結果をワークに保存
	wk->retire_f = retire;
	wk->send_buf[0] = retire;
}

#if 0
/**
 *	@brief	バトルタワー　送られてきたプレイヤーデータを受け取る
 */
u16 BTowerComm_RecvPlayerData(BTOWER_SCRWORK* wk,const u16* recv_buf)
{
	u16	ret = 0;

	wk->pare_sex = (u8)recv_buf[0];
	wk->pare_poke[0] = recv_buf[1];
	wk->pare_poke[1] = recv_buf[2];
	wk->pare_stage = recv_buf[3];

	wk->partner = 5+wk->pare_sex;

	OS_Printf("sio multi mem = %d,%d:%d,%d\n",wk->mem_poke[0],wk->mem_poke[1],wk->pare_poke[0],wk->pare_poke[1]);
	if(	wk->mem_poke[0] == wk->pare_poke[0] ||
		wk->mem_poke[0] == wk->pare_poke[1]){
		ret += 1;
	}
	if(	wk->mem_poke[1] == wk->pare_poke[0] ||
		wk->mem_poke[1] == wk->pare_poke[1]){
		ret += 2;
	}
	return ret;
}

/**
 *	@brief	バトルタワー　送られてきたトレーナーデータを受け取る
 */
u16	BTowerComm_RecvTrainerData(BTOWER_SCRWORK* wk,const u16* recv_buf)
{
	int i;

	if(CommGetCurrentID() == COMM_PARENT_ID){
		return 0;	//親は送信するだけなので受け取らない
	}

	MI_CpuCopy8(recv_buf,wk->trainer,BTOWER_STOCK_TRAINER_MAX*2);
	OS_Printf("sio multi trainer01 = %d,%d:%d,%d\n",
			wk->trainer[0],wk->trainer[1],wk->trainer[2],wk->trainer[3]);
	OS_Printf("sio multi trainer02 = %d,%d:%d,%d\n",
			wk->trainer[4],wk->trainer[5],wk->trainer[6],wk->trainer[7]);
	OS_Printf("sio multi trainer03 = %d,%d:%d,%d\n",
			wk->trainer[8],wk->trainer[9],wk->trainer[10],wk->trainer[11]);
	OS_Printf("sio multi trainer04 = %d,%d\n",
			wk->trainer[12],wk->trainer[13]);
	return 1;
}
#endif

/**
 *	@brief	バトルタワー　送られてきたリタイアするかどうかの結果を受け取る
 *
 *	@retval	0	リタイアしない
 *	@retval	1	リタイアする
 */
u16	FSBTowerComm_RecvRetireSelect(BTOWER_SCRWORK* wk,const u16* recv_buf)
{
	int i;

	OS_Printf("sio multi retire = %d,%d\n",wk->retire_f,recv_buf[0]);
	if(wk->retire_f || recv_buf[0]){
		return 1;
	}
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	対戦ポケモンのアーカイブを取得
 */
//--------------------------------------------------------------
int BtlTower_GetPokeArc( u8 play_mode )
{
	if( BtlTower_GetArcVersion(play_mode) == 0 ){
		return ARC_BTD_PM;			//ＤＰ
	}

	return ARC_PL_BTD_PM;			//プラチナ
}

//--------------------------------------------------------------
/**
 * @brief	対戦トレーナーのアーカイブを取得
 */
//--------------------------------------------------------------
int BtlTower_GetTrArc( u8 play_mode )
{
	if( BtlTower_GetArcVersion(play_mode) == 0 ){
		return ARC_BTD_TR;			//ＤＰ
	}

	return ARC_PL_BTD_TR;			//プラチナ
}

//--------------------------------------------------------------
/**
 * @brief	対戦トレーナーのメッセージアーカイブを取得
 */
//--------------------------------------------------------------
int BtlTower_GetTrMsgArc( u8 play_mode )
{
	if( BtlTower_GetArcVersion(play_mode) == 0 ){
		return NARC_msg_tower_trainer_dat;			//ＤＰ
	}

	return NARC_msg_tower_trainerpl_dat;			//プラチナ
}

//--------------------------------------------------------------
/**
 * @brief	バージョンをチェックして、どちらのアーカイブを取得かチェック
 *
 * @retval	"0=DP、1=プラチナ"
 */
//--------------------------------------------------------------
static int BtlTower_GetArcVersion( u8 play_mode )
{
	MYSTATUS* my1;
	MYSTATUS* my2;
	u8 rom_code1,rom_code2;

	switch( play_mode ){

	//通信
	case BTWR_MODE_COMM_MULTI:
	case BTWR_MODE_WIFI_MULTI:

		my1 = CommInfoGetMyStatus( 0 );
		if( my1 == NULL ){
			OS_Printf( "CommInfoGetMyStatus(0)取得出来ませんでした\n" );
			GF_ASSERT(0);
		}

		my2 = CommInfoGetMyStatus( 1 );
		if( my2 == NULL ){
			OS_Printf( "CommInfoGetMyStatus(1)取得出来ませんでした\n" );
			GF_ASSERT(0);
		}

		//ロムコード取得
		rom_code1 = MyStatus_GetRomCode( my1 );
		rom_code2 = MyStatus_GetRomCode( my2 );

#if 0
		//DP含まれていたら
		if( (rom_code1 == VERSION_DIAMOND) || (rom_code1 == VERSION_PEARL) ){
			return 0;			//ＤＰ
		}

		//DP含まれていたら
		if( (rom_code2 == VERSION_DIAMOND) || (rom_code2 == VERSION_PEARL) ){
			return 0;			//ＤＰ
		}
#else
		//DP含まれていたら
		if( (rom_code1 == POKEMON_DP_ROM_CODE) || (rom_code2 == POKEMON_DP_ROM_CODE) ){
			return 0;			//ＤＰ
		}
#endif

		return 1;			//プラチナ(★金銀とプラチナ★)
	};

	return 1;				//プラチナ(★金銀とプラチナ★)
}



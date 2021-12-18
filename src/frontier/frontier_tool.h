//==============================================================================
/**
 * @file	frontier_tool.h
 * @brief	バトルフロンティア施設で共通で使えるようなツール
 * @author	matsuda
 * @date	2007.03.27(火)
 */
//==============================================================================
#ifndef __FRONTIER_TOOL_H__
#define __FRONTIER_TOOL_H__

#include "gflib/msg_print.h"	// STRCODE 定義のため(b_tower_data.h用)
#include "battle/battle_common.h"	//(b_tower_data.h用)
#include "system/wordset.h"		//
#include "system/buflen.h"		//(b_tower_data.h用)
#include "savedata/b_tower.h"	//(b_tower_data.h用)
#include "battle/b_tower_data.h"
#include "battle/tr_ai_def.h"

#define FRONTIER_LEVEL_50		(120)		//LV50
#define FRONTIER_LEVEL_OPEN		(121)		//オープン(LV100)

//==============================================================================
//	外部関数宣言
//==============================================================================
extern B_TOWER_TRAINER_ROM_DATA * Frontier_TrainerRomDataGet(u16 trainer_index,int heap_id,int arcTR);
extern void Frontier_PokemonRomDataGet(B_TOWER_POKEMON_ROM_DATA *prd,int index, int arcID);
extern B_TOWER_TRAINER_ROM_DATA * Frontier_TrainerDataGet(B_TOWER_TRAINER *bt_trd, 
	int trainer_index, int heap_id, int arcTR);
extern u32 Frontier_PokemonParamMake(B_TOWER_POKEMON* pwd,
	u16 poke_index,u32 poke_id,u32 poke_rnd,u8 pow_rnd,u8 mem_idx,BOOL itemfix,int heapID, int arcID);
extern void Frontier_PokeParaMake(const B_TOWER_POKEMON* src,POKEMON_PARAM* dest, u8 level);
extern u16 Frontier_TrType2ObjCode(u8 tr_type);

extern void Frontier_BirthInfoSet( SAVEDATA* sv, POKEMON_PARAM* poke );
extern void Frontier_PokePartyAdd( SAVEDATA* sv, POKEPARTY* party, POKEMON_PARAM* poke );
//extern int Frontier_GetFriendIndex( u32 record_no );

extern void Frontier_PokeIconAnmChg( CLACT_WORK_PTR awp, u8 anm );
extern void Frontier_PokeIconPosSet( CLACT_WORK_PTR awp, s16 init_x, s16 init_y, u8 flag );
extern u8 Frontier_PokeIconAnmNoGet( u16 hp, u16 hp_max );

extern void FSRomBattleTowerTrainerDataMake2( B_TOWER_PARTNER_DATA *tr_data, u16 tr_no, int heapID, int arcTR );
//extern B_TOWER_TRAINER_ROM_DATA* RomTrainerDataAlloc(B_TOWER_PARTNER_DATA *tr_data,u16 tr_no,int heapID);
extern void BattleParamTrainerDataSet( BATTLE_PARAM* bp, B_TOWER_TRAINER* bt_trd, int cnt, int client_no, int heapID );
extern u32 Frontier_PokemonParamCreate( B_TOWER_POKEMON *pwd, u16 poke_index, int position, u8 pow_rnd, u32 personal_rnd, int heap_id, int arcPM );
extern void Frontier_PokemonParamCreateAll(B_TOWER_POKEMON *pwd, u16 poke_index[], 
	u8 pow_rnd[], u32 personal_rnd[], u32 ret_personal_rnd[], int set_num, int heap_id, int arcPM);
extern u8 Frontier_PowRndGet(u16 tr_no);
extern BOOL Frontier_PokemonIndexCreate( B_TOWER_TRAINER_ROM_DATA* trd, const u16 check_pokeno[], const u16 check_itemno[], int check_num, int get_count, u16 get_pokeindex[], int heap_id );
extern void Frontier_EnemyPokeMake( u8 set_num, u16 tr_index, u16 tr_index2, u16* enemy_poke_index, B_TOWER_POKEMON* enemy_poke, u8* pow_rnd, u32* personal_rnd, u8 flag );
//extern void Frontier_PairNameSet( WORDSET* wordset, u16 bufID );
extern void Frontier_PairNameWordSet( WORDSET* wordset, u32 bufID );
extern int FrontierTool_MyObjCodeGet(const MYSTATUS *my_status);
extern void FrontierTool_WifiHistory( SAVEDATA* savedata );


#endif	//__FRONTIER_TOOL_H__



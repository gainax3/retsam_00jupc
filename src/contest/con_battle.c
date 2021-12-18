//==============================================================================
/**
 * @file	con_battle.c
 * @brief	コンテスト・戦闘共通処理
 * @author	matsuda
 * @date	2006.06.14(水)
 *
 * オーバーレイ領域はbc_commonに配置されます
 */
//==============================================================================
#include "common.h"
#include "contest/contest.h"
#include "poketool/monsno.h"
#include "battle/battle_server.h"



//--------------------------------------------------------------
/**
 * @brief   コンテストで反転させないポケモンかチェック
 *
 * @param   monsno		ポケモン番号
 * @param   form_no		フォルム番号
 *
 * @retval  TRUE：反転する
 * @retval  FALSE：反転しない
 */
//--------------------------------------------------------------
BOOL ContestPokeFlipCheck(u32 monsno, u32 form_no)
{
	switch(monsno){
	case MONSNO_HITODEMAN:
	case MONSNO_EREBUU:
	case MONSNO_ARIGEITU:
	case MONSNO_ANNOON:
	case MONSNO_NYUURA:
	case MONSNO_ROZERIA:
	case MONSNO_ZANGUUSU:
	case MONSNO_ABUSORU:
	case MONSNO_SUBOMII:
	case MONSNO_ROZUREIDO:
	case MONSNO_BUUBAAN:
	case MONSNO_MORIGAME:
	case MONSNO_TOGEON:
	case MONSNO_KINGURAA:
		return FALSE;
	case MONSNO_EURISU:
		if(form_no == FORMNO_SHEIMI_FLOWER){
			return FALSE;
		}
		break;
	}
	return TRUE;
}


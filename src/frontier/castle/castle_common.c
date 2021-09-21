//==============================================================================================
/**
 * @file	castle_common.c
 * @brief	「バトルキャッスル」共通処理
 * @author	Satoshi Nohara
 * @date	07.10.22
 */
//==============================================================================================
#include "system/main.h"
#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "system/clact_tool.h"
#include "system/snd_tool.h"
#include "system/pm_str.h"
#include "system/buflen.h"
#include "system/wordset.h"
#include "system/bmp_list.h"
#include "poketool/pokeparty.h"
#include "savedata/frontier_savedata.h"

#include "communication/comm_system.h"
#include "communication/comm_tool.h"
#include "communication/comm_info.h"
#include "communication/comm_def.h"

#include "gflib/blact.h"
#include "../../field/fieldobj.h"
#include "../../field/scr_tool.h"

#include "castle_common.h"

#include "system/pm_overlay.h"
FS_EXTERN_OVERLAY(frontier_common);


//==============================================================================================
//
//	プロトタイプ宣言
//
//==============================================================================================
u8 GetCsrPokePos( u8 h_max, u8 csr_pos );
u8 GetCsrSelType( u8 h_max, u8 csr_pos );
u8 GetCommSelCsrPos( u8 type_offset, u8 decide_type );
u8 Castle_GetRank( SAVEDATA* sv, u8 type, u8 rank_type );
void Castle_ListSeCall( u32 param, u16 no );
void Castle_SetPairName( WORDSET* wordset, u32 id );

//--------------------------------------------------------------
/**
 * @brief	エントリーポケモンの何匹目か取得
 *
 * @param	h_max	横の数
 * @param	csr_pos	カーソル位置
 *
 * @return	"何匹目か"
 */
//--------------------------------------------------------------
u8 GetCsrPokePos( u8 h_max, u8 csr_pos )
{
	//return (csr_pos % h_max);
	return csr_pos;
}

//--------------------------------------------------------------
/**
 * @brief	カーソル位置から「かいふく」「どうぐ」「つよさ」「わざ」のどれを選択したか取得
 * @brief	カーソル位置から「てもち」「レベル」「つよさ」「わざ」のどれを選択したか取得
 * @brief	カーソル位置から「かいふく」「どうぐ」「じょうほう」のどれを選択したか取得
 *
 * @param	h_max	横の数
 * @param	csr_pos	カーソル位置
 *
 * @return	"種類"
 */
//--------------------------------------------------------------
u8 GetCsrSelType( u8 h_max, u8 csr_pos )
{
	return (csr_pos / h_max);
}

//--------------------------------------------------------------
/**
 * @brief	通信の選んだカーソル位置を取得
 *
 * @param	hv_max	縦横の数
 * @param	csr_pos	カーソル位置
 *
 * @return	"カーソル位置"
 */
//--------------------------------------------------------------
u8 GetCommSelCsrPos( u8 type_offset, u8 decide_type )
{
	//オフセットを外す
	if( decide_type < type_offset ){
		return decide_type;
	}

	return (decide_type - type_offset);
}

//--------------------------------------------------------------
/**
 * @brief	現在のランク取得
 *
 * @param	score_sv	CASTLESCORE型のポインタ
 * @param	type		バトルタイプ
 * @param	rank_type	どのランクを取得するか(かいふく、レンタル、じょうほう)
 *
 * @return	"ランク"
 */
//--------------------------------------------------------------
u8 Castle_GetRank( SAVEDATA* sv, u8 type, u8 rank_type )
{
	return FrontierRecord_Get(	SaveData_GetFrontier(sv), 
						CastleScr_GetRankRecordID(type,rank_type),
						Frontier_GetFriendIndex(CastleScr_GetRankRecordID(type,rank_type)) );
}

//--------------------------------------------------------------
/**
 * @brief	BMPLIST_NULLじゃない時は共通の効果音を鳴らす
 *
 * @param	param		
 * @param	no			SEナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
void Castle_ListSeCall( u32 param, u16 no )
{
	if( param != BMPLIST_NULL ){
		Snd_SePlay( no );
	}
	return;
}

//--------------------------------------------------------------
/**
 * @brief	パートナー名をセット
 *
 * @param	wordset		WORDSET型のポインタ
 * @param	id			bufID
 *
 * @return	none
 */
//--------------------------------------------------------------
void Castle_SetPairName( WORDSET* wordset, u32 id )
{
	MYSTATUS* my;
	u32 col;

	//パートナーのMyStatus取得
	my = CommInfoGetMyStatus( (CommGetCurrentID() ^ 1) );

	WORDSET_RegisterPlayerName( wordset, id, my );
	return;
}



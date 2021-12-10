//============================================================================================
/**
 * @file	syswork.c
 * @bfief	プログラム、スクリプトからアクセスされるシステムワーク操作
 * @author	Satoshi Nohara
 * @date	06.03.12
 */
//============================================================================================
#include "common.h"
#include "fieldsys.h"
#include "script.h"
#include "field/evwkdef.h"
#include "syswork.h"
#include "sysflag.h"				//SysFlag_
#include "poketool/monsno.h"		//ポケモンナンバー

#include "savedata/randomgroup.h"

//============================================================================================
//
//	プロトタイプ宣言
//
//============================================================================================
static BOOL SysWork_Set( EVENTWORK* ev, u16 work, u16 no );
static u16 SysWork_Get( EVENTWORK* ev, u16 work );
BOOL SysWork_FirstPokeNoSet( EVENTWORK* ev, u16 no );
u16 SysWork_FirstPokeNoGet( EVENTWORK* ev );
u16 SysWork_RivalPokeNoGet( EVENTWORK* ev );
u16 SysWork_SupportPokeNoGet( EVENTWORK* ev );


//============================================================================================
//
//	関数
//
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * システムワークセット
 *
 * @param	ev			EVENTWORKへのポインタ
 * @param	work		ワークナンバー
 * @param	no			セットする値
 *
 * @return	"TRUE=セット出来た、FALSE=セット出来なかった"
 */
//--------------------------------------------------------------------------------------------
static BOOL SysWork_Set( EVENTWORK* ev, u16 work, u16 no )
{
	u16* p	= EventWork_GetEventWorkAdrs( ev, work );

	if( (work < SVWK_START) || (work > SCWK_START ) ){
		GF_ASSERT( (0) && "不正なワークIDが渡されました！" );
		return FALSE;
	}

	if( p == NULL ){
		return FALSE;
	}

	*p = no;
	return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * システムワークゲット
 *
 * @param	ev			EVENTWORKのポインタ
 * @param	work		ワークナンバー
 *
 * @return	"ワークの値"
 */
//--------------------------------------------------------------------------------------------
static u16 SysWork_Get( EVENTWORK* ev, u16 work )
{
	u16* p	= EventWork_GetEventWorkAdrs( ev, work );

	if( p == NULL ){
		return 0;
	}

	return *p;
}


//============================================================================================
//
//	連れ歩きトレーナーID格納ワーク関連
//
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 連れ歩きトレーナーID格納ワークセット
 *
 * @param	ev			EVENTWORKへのポインタ
 * @param	no			トレーナーID
 *
 * @return	"TRUE=セット出来た、FALSE=セット出来なかった"
 */
//--------------------------------------------------------------------------------------------
BOOL SysWork_PairTrainerIDSet( EVENTWORK* ev, u16 no )
{
	return SysWork_Set( ev, SYS_WORK_PAIR_TRAINER_ID, no );
}

//--------------------------------------------------------------------------------------------
/**
 * 連れ歩きトレーナーID格納ワークゲット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @return	"トレーナーID"
 */
//--------------------------------------------------------------------------------------------
u16 SysWork_PairTrainerIDGet( EVENTWORK* ev )
{
	return SysWork_Get( ev, SYS_WORK_PAIR_TRAINER_ID );
}


//============================================================================================
//
//	最初のポケモンナンバー関連
//
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 最初のポケモンナンバーセット
 *
 * @param	ev			EVENTWORKへのポインタ
 * @param	no			ポケモンナンバー
 *
 * @return	"TRUE=セット出来た、FALSE=セット出来なかった"
 */
//--------------------------------------------------------------------------------------------
BOOL SysWork_FirstPokeNoSet( EVENTWORK* ev, u16 no )
{
	return SysWork_Set( ev, SYS_WORK_FIRST_POKE_NO, no );
}

//--------------------------------------------------------------------------------------------
/**
 * 最初のポケモンナンバーゲット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @return	"最初のポケモンナンバー"
 */
//--------------------------------------------------------------------------------------------
u16 SysWork_FirstPokeNoGet( EVENTWORK* ev )
{
	return SysWork_Get( ev, SYS_WORK_FIRST_POKE_NO );
}

//--------------------------------------------------------------------------------------------
/**
 * ライバルのポケモンナンバーゲット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @return	"ライバルのポケモンナンバー"
 */
//--------------------------------------------------------------------------------------------
u16 SysWork_RivalPokeNoGet( EVENTWORK* ev )
{
	u16 rival;
	u16 first = SysWork_Get( ev, SYS_WORK_FIRST_POKE_NO );

#if 1
	if( first == MONSNO_NAETORU ){
		rival = MONSNO_HIKOZARU;
	}else if( first == MONSNO_HIKOZARU ){
		rival = MONSNO_POTTYAMA;
	}else{
		rival = MONSNO_NAETORU;
	}
#endif

	return rival;
}

//--------------------------------------------------------------------------------------------
/**
 * サポートのポケモンナンバーゲット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @return	"サポートのポケモンナンバー"
 */
//--------------------------------------------------------------------------------------------
u16 SysWork_SupportPokeNoGet( EVENTWORK* ev )
{
	u16 support;
	u16 first = SysWork_Get( ev, SYS_WORK_FIRST_POKE_NO );

#if 1
	if( first == MONSNO_NAETORU ){
		support = MONSNO_POTTYAMA;
	}else if( first == MONSNO_HIKOZARU ){
		support = MONSNO_NAETORU;
	}else{
		support = MONSNO_HIKOZARU;
	}
#endif

	return support;
}


//============================================================================================
//
//	おおきさくらべ関連
//
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 現在の記録を取得
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @return	"個体値"
 */
//--------------------------------------------------------------------------------------------
u16 SysWork_OokisaRecordGet( EVENTWORK* ev )
{
	return SysWork_Get( ev, SYS_WORK_OOKISA_RECORD );
}

//--------------------------------------------------------------------------------------------
/**
 * 記録を書き換える
 *
 * @param	ev		EVENTWORKへのポインタ
 * @param	no			個体値
 *
 * @return	"TRUE=セット出来た、FALSE=セット出来なかった"
 */
//--------------------------------------------------------------------------------------------
BOOL SysWork_OokisaRecordSet( EVENTWORK* ev, u16 no )
{
	return SysWork_Set( ev, SYS_WORK_OOKISA_RECORD, no );
}


//============================================================================================
//
//	バトルサーチャー関連
//
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * マップ遷移した時のリセット処理
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void Sys_BtlSearcherReset( EVENTWORK* ev )
{
	SysFlag_BtlSearcherUseReset( ev );		//使用中を解除
	SysWork_BtlSearcherClearSet( ev, 0 );	//クリアカウント初期化
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * バッテリーカウントを取得
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @return	"カウント"
 */
//--------------------------------------------------------------------------------------------
u16 SysWork_BtlSearcherBatteryGet( EVENTWORK* ev )
{
	return SysWork_Get( ev, SYS_WORK_BTL_SEARCHER_BATTERY );
}

//--------------------------------------------------------------------------------------------
/**
 * バッテリーカウントをセット
 *
 * @param	ev		EVENTWORKへのポインタ
 * @param	no		セットカウント
 *
 * @return	"TRUE=セット出来た、FALSE=セット出来なかった"
 */
//--------------------------------------------------------------------------------------------
BOOL SysWork_BtlSearcherBatterySet( EVENTWORK* ev, u16 no )
{
	return SysWork_Set( ev, SYS_WORK_BTL_SEARCHER_BATTERY, no );
}

//--------------------------------------------------------------------------------------------
/**
 * クリアカウントを取得
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @return	"カウント"
 */
//--------------------------------------------------------------------------------------------
u16 SysWork_BtlSearcherClearGet( EVENTWORK* ev )
{
	return SysWork_Get( ev, SYS_WORK_BTL_SEARCHER_CLEAR );
}

//--------------------------------------------------------------------------------------------
/**
 * クリアカウントをセット
 *
 * @param	ev		EVENTWORKへのポインタ
 * @param	no		セットカウント
 *
 * @return	"TRUE=セット出来た、FALSE=セット出来なかった"
 */
//--------------------------------------------------------------------------------------------
BOOL SysWork_BtlSearcherClearSet( EVENTWORK* ev, u16 no )
{
	return SysWork_Set( ev, SYS_WORK_BTL_SEARCHER_CLEAR, no );
}




//============================================================================================
//
//
//			配布イベント関連
//
//
//============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * @brief	それぞれの配布イベントに対応したマジックナンバーを返す
 */
//--------------------------------------------------------------------------------------------
static get_haihu_magicnumber(int ex_event_id)
{
	static const u16 magic_number[] = {
		0x1209,
		0x1112,
		0x1123,
		0x1103,
	};
	GF_ASSERT(0 <= ex_event_id && ex_event_id < NELEMS(magic_number));
	return magic_number[ex_event_id];
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
void SysWork_HaihuEventWorkSet(EVENTWORK * ev, int haihu_id)
{
	SysWork_Set(ev, SYS_WORK_HAIHU_EVENT01 + haihu_id, get_haihu_magicnumber(haihu_id));
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
void SysWork_HaihuEventWorkClear(EVENTWORK * ev, int haihu_id)
{
	SysWork_Set(ev, SYS_WORK_HAIHU_EVENT01 + haihu_id, 0);
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
BOOL SysWork_HaihuEventWorkCheck(EVENTWORK * ev, int haihu_id)
{
	if (SysWork_Get(ev, SYS_WORK_HAIHU_EVENT01 + haihu_id) == get_haihu_magicnumber(haihu_id)) {
		return TRUE;
	} else {
		return FALSE;
	}
}

//============================================================================================
//
//
//			隠しマップ関連
//
//
//============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * @brief	それぞれに対応したマジックナンバーを返す
 */
//--------------------------------------------------------------------------------------------
static get_hidemap_magicnumber(int hidemap_id)
{
	static const u16 magic_number[] = {
		0x0208,
		0x0229,
		0x0312,
		0x1028,
	};
	GF_ASSERT(0 <= hidemap_id && hidemap_id < 4);
	return magic_number[hidemap_id];

}
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
void SysWork_HideMapWorkSet(EVENTWORK * ev, int hidemap_id)
{
	SysWork_Set(ev, SYS_WORK_HIDEMAP_01 + hidemap_id, get_hidemap_magicnumber(hidemap_id));
}
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
void SysWork_HideMapWorkClear(EVENTWORK * ev, int hidemap_id)
{
	SysWork_Set(ev, SYS_WORK_HIDEMAP_01 + hidemap_id, 0);
}
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
BOOL SysWork_HideMapWorkCheck(EVENTWORK * ev, int hidemap_id)
{
	if(SysWork_Get(ev, SYS_WORK_HIDEMAP_01 + hidemap_id) == get_hidemap_magicnumber(hidemap_id)) {
		return TRUE;
	} else {
		return FALSE;
	}
}


//--------------------------------------------------------------------------------------------
/**
 * 自然公園の総歩数クリア
 *
 * @param	ev			EVENTWORKへのポインタ
 *
 * @return	"TRUE=セット出来た、FALSE=セット出来なかった"
 */
//--------------------------------------------------------------------------------------------
BOOL SysWork_ParkWalkCountClear( EVENTWORK* ev)
{
	return SysWork_Set( ev, SYS_WORK_PARK_WALK_COUNT, 0 );
}

//--------------------------------------------------------------------------------------------
/**
 * 自然公園の総歩数ゲット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @return	"最初のポケモンナンバー"
 */
//--------------------------------------------------------------------------------------------
u16 SysWork_ParkWalkCountGet( EVENTWORK* ev )
{
	return SysWork_Get( ev, SYS_WORK_PARK_WALK_COUNT );
}

//--------------------------------------------------------------------------------------------
/**
 * 自然公園の総歩数を＋１
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @return	"最初のポケモンナンバー"
 */
//--------------------------------------------------------------------------------------------
BOOL SysWork_ParkWalkCountPlus( EVENTWORK* ev )
{
	u16 count;
	count = SysWork_Get( ev, SYS_WORK_PARK_WALK_COUNT );
	if(count < 10000) {		//※他での使用を考慮して多めに（自然公園のみなら２００でOK）
		count++;
	}else{
		count = 10000;
	}
	return SysWork_Set( ev, SYS_WORK_PARK_WALK_COUNT, count );
}

//--------------------------------------------------------------------------------------------
/**
 * 新聞社締め切りカウンタを取得
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @return	"カウント"
 */
//--------------------------------------------------------------------------------------------
u16 SysWork_NewsDaysCountGet( EVENTWORK* ev )
{
	return SysWork_Get( ev, SYS_WORK_NEWS_DAYS_COUNT );
}

//--------------------------------------------------------------------------------------------
/**
 * 新聞社締め切りカウンタをセット
 *
 * @param	ev		EVENTWORKへのポインタ
 * @param	no		セットカウント
 *
 * @return	"TRUE=セット出来た、FALSE=セット出来なかった"
 */
//--------------------------------------------------------------------------------------------
BOOL SysWork_NewsDaysCountSet( EVENTWORK* ev, u16 no )
{
	return SysWork_Set( ev, SYS_WORK_NEWS_DAYS_COUNT, no );
}

//--------------------------------------------------------------------------------------------
//			ポケモンクジ関連
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
/**
 * ポケモンクジ乱数に値をセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void SysWork_pokelot_value_set( EVENTWORK* ev ,u32 val)
{
	u16 L, H;

	H = (val >> 16) & 0xffff;
	L = val & 0xffff;
	SysWork_Set( ev, SYS_WORK_POKELOT_RND1, L );
	SysWork_Set( ev, SYS_WORK_POKELOT_RND1, H );

}

//--------------------------------------------------------------------------------------------
/**
 * ポケモンクジ乱数の値をゲット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
u32 SysWork_pokelot_value_get(EVENTWORK* ev)
{
	u16 L, H;
	L = SysWork_Get( ev, SYS_WORK_POKELOT_RND1 );
	H = SysWork_Get( ev, SYS_WORK_POKELOT_RND2 );
	return ((H<<16)|L);
}

//--------------------------------------------------------------------------------------------
/**
 * ポケモンクジ乱数の値を初期化
 *  ゲーム開始時の初期化処理
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void SysWorkInitPokeLot(EVENTWORK* ev)
{
	u16  L, H;

	L = gf_rand();
	H = gf_rand();
	OS_Printf("PokeLot L[%d] H[%d]\n",L,H);
	SysWork_pokelot_value_set(ev,(H<<16) | L);
}

//--------------------------------------------------------------------------------------------
/**
 * ポケモンクジ乱数の値を初期化
 * 日数経過による更新処理
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void SysWorkUpdatePokeLot(SAVEDATA *sv,u16 days)
{
	EVENTWORK * ev = SaveData_GetEventWork(sv);
	u32  value;

	value = RandomGroup_GetDefaultRandom(SaveData_GetRandomGroup(sv));

	value = value * 1103515245L + 12345;

	SysWork_pokelot_value_set(ev,value);
}


//--------------------------------------------------------------------------------------------
/**
 * １日１回、ポケモンのレベルと同じ数のとき、アイテムをくれるおじさん
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @return	"カウント"
 */
//--------------------------------------------------------------------------------------------
u16 SysWork_PokeLevelNoGet( EVENTWORK* ev )
{
	return SysWork_Get( ev, SYS_WORK_POKE_LEVEL_NO );
}

//--------------------------------------------------------------------------------------------
/**
 * １日１回、ポケモンのレベルと同じ数のとき、アイテムをくれるおじさん
 *
 * @param	ev		EVENTWORKへのポインタ
 * @param	no		セットカウント
 *
 * @return	"TRUE=セット出来た、FALSE=セット出来なかった"
 */
//--------------------------------------------------------------------------------------------
BOOL SysWork_PokeLevelNoSet( EVENTWORK* ev ,u16 no)
{
	return SysWork_Set( ev, SYS_WORK_POKE_LEVEL_NO, no );
}

//--------------------------------------------------------------------------------------------
/**
 * １日１回、ポケモンのレベルと同じ数のとき、アイテムをくれるおじさん
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void SysWorkUpdatePokeLevelNo(SAVEDATA *sv)
{
	EVENTWORK * ev = SaveData_GetEventWork(sv);
	u32  value;

	value = (gf_rand() % 98) + 2;		//2-99
	SysWork_PokeLevelNoSet(ev,value);
}

//--------------------------------------------------------------------------------------------
/**
 * 地下会話カウントを取得(ミカゲ専用)
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @return	"カウント"
 */
//--------------------------------------------------------------------------------------------
u16 SysWork_UGTalkCountGet( EVENTWORK* ev )
{
	u16		count;
	count = SysWork_Get( ev, SYS_WORK_UG_TALK_COUNT );
	OS_Printf("SYS_WORK_UG_TALK_COUNT [%d]\n",count);
	return count;
}

//--------------------------------------------------------------------------------------------
/**
 * 地下会話カウントをセット(ミカゲ専用)
 *
 * @param	ev		EVENTWORKへのポインタ
 * @param	no		セットカウント
 *
 * @return	"TRUE=セット出来た、FALSE=セット出来なかった"
 */
//--------------------------------------------------------------------------------------------
BOOL SysWork_UGTalkCountSet( EVENTWORK* ev, u16 no )
{
	return SysWork_Set( ev, SYS_WORK_UG_TALK_COUNT , no );
}



//--------------------------------------------------------------------------------------------
/**
 * 地下会話カウントを取得
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @return	"カウント"
 */
//--------------------------------------------------------------------------------------------
u16 SysWork_UGTalkCount2Get( EVENTWORK* ev )
{
	u16		count;
	count = SysWork_Get( ev, SYS_WORK_UG_TALK_COUNT2 );
	OS_Printf("SYS_WORK_UG_TALK_COUNT2 [%d]\n",count);
	return count;
}

//--------------------------------------------------------------------------------------------
/**
 * 地下会話カウントをセット
 *
 * @param	ev		EVENTWORKへのポインタ
 * @param	no		セットカウント
 *
 * @return	"TRUE=セット出来た、FALSE=セット出来なかった"
 */
//--------------------------------------------------------------------------------------------
BOOL SysWork_UGTalkCount2Set( EVENTWORK* ev, u16 no )
{
	return SysWork_Set( ev, SYS_WORK_UG_TALK_COUNT2 , no );
}

//--------------------------------------------------------------------------------------------
/**
 * レンチャン数をゲット
 *
 * @param	ev		EVENTWORKへのポインタ
 * @param	no		セットカウント
 *
 * @return	カウント
 */
//--------------------------------------------------------------------------------------------
u16 SysWork_RensyouCountGet( EVENTWORK* ev)
{
	u16		count;
	count = SysWork_Get( ev, SYS_WORK_RENSYOU_COUNT );
	return count;
}

//--------------------------------------------------------------------------------------------
/**
 * レンチャン数をセット
 *
 * @param	ev		EVENTWORKへのポインタ
 * @param	no		セットカウント
 *
 * @return	"TRUE=セット出来た、FALSE=セット出来なかった"
 */
//--------------------------------------------------------------------------------------------
BOOL SysWork_RensyouCountSet( EVENTWORK* ev, u16 no )
{
	OS_Printf("SYS_WORK_RENSYOU_COUNT <- [%d]\n",no);
	return SysWork_Set( ev, SYS_WORK_RENSYOU_COUNT , no );
}

//--------------------------------------------------------------------------------------------
/**
 *	デパート購入回数
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @return	"最初のポケモンナンバー"
 */
//--------------------------------------------------------------------------------------------
BOOL SysWork_RegularCountPlus( EVENTWORK* ev )
{
	u16 count;
	count = SysWork_Get( ev, SYS_WORK_DEPART_COUNT );
	if(count < 10000) {		//※ループしないようにリミットつき
		count++;
	}else{
		count = 10000;
	}
	return SysWork_Set( ev, SYS_WORK_DEPART_COUNT, count );
}

//--------------------------------------------------------------------------------------------
/**
 *	デパート購入回数
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @return	"カウント"
 */
//--------------------------------------------------------------------------------------------
u16 SysWork_RegularCountGet( EVENTWORK* ev )
{
	return SysWork_Get( ev, SYS_WORK_DEPART_COUNT );
}

//--------------------------------------------------------------------------------------------
/**
 *	地下で道具をあげた人数の累計
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @return	"カウント"
 */
//--------------------------------------------------------------------------------------------
u16 SysWork_UGToolGiveCountGet( EVENTWORK* ev )
{
	return SysWork_Get( ev, SYS_WORK_UG_TOOL_GIVE_COUNT	 );
}

//--------------------------------------------------------------------------------------------
/**
 *	地下で道具をあげた人数をセット
 *
 * @param	ev		EVENTWORKへのポインタ
 * @param	no		セットカウント
 *
 * @return	"TRUE=セット出来た、FALSE=セット出来なかった"
 */
//--------------------------------------------------------------------------------------------
BOOL SysWork_UGToolGiveCountSet( EVENTWORK* ev, u16 no )
{
	return SysWork_Set( ev, SYS_WORK_UG_TOOL_GIVE_COUNT	 , no );
}

//--------------------------------------------------------------------------------------------
/**
 *	地下でかせきを掘った回数の累計
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @return	"カウント"
 */
//--------------------------------------------------------------------------------------------
u16 SysWork_UGKasekiDigCountGet( EVENTWORK* ev )
{
	return SysWork_Get( ev, SYS_WORK_UG_KASEKI_DIG_COUNT	 );
}

//--------------------------------------------------------------------------------------------
/**
 *	地下でかせきを掘った回数をセット
 *
 * @param	ev		EVENTWORKへのポインタ
 * @param	no		セットカウント
 *
 * @return	"TRUE=セット出来た、FALSE=セット出来なかった"
 */
//--------------------------------------------------------------------------------------------
BOOL SysWork_UGKasekiDigCountSet( EVENTWORK* ev, u16 no )
{
	return SysWork_Set( ev, SYS_WORK_UG_KASEKI_DIG_COUNT , no );
}

//--------------------------------------------------------------------------------------------
/**
 *	地下でトラップにかけた回数の累計
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @return	"カウント"
 */
//--------------------------------------------------------------------------------------------
u16 SysWork_UGTrapHitCountGet( EVENTWORK* ev )
{
	return SysWork_Get( ev, SYS_WORK_UG_TRAP_HIT_COUNT	 );
}

//--------------------------------------------------------------------------------------------
/**
 *	地下でトラップにかけた回数をセット
 *
 * @param	ev		EVENTWORKへのポインタ
 * @param	no		セットカウント
 *
 * @return	"TRUE=セット出来た、FALSE=セット出来なかった"
 */
//--------------------------------------------------------------------------------------------
BOOL SysWork_UGTrapHitCountSet( EVENTWORK* ev, u16 no )
{
	return SysWork_Set( ev, SYS_WORK_UG_TRAP_HIT_COUNT , no );
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
u16 SysWork_FriendlyStepCountGet(EVENTWORK * ev)
{
	return SysWork_Get(ev, SYS_WORK_FRIENDLY_COUNT);
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
void SysWork_FriendlyStepCountSet(EVENTWORK * ev, u16 step)
{
	SysWork_Set(ev, SYS_WORK_FRIENDLY_COUNT, step);
}

//--------------------------------------------------------------
/**
 * PL 草ジム時刻セット
 * @param	ev	EVENTWORK *
 * @param	seq	時刻シーケンス
 * @retval	nothing
 */
//--------------------------------------------------------------
void SysWork_PLGrassGymTimeSeqSet( EVENTWORK *ev, u16 seq )
{
	SysWork_Set( ev, SYS_WORK_PL_GRASS_GYM_TIME, seq );
}

//--------------------------------------------------------------
/**
 * PL 草ジム時刻取得
 * @param	ev	EVENTWORK *
 * @retval	u16 時刻シーケンス
 */
//--------------------------------------------------------------
u16 SysWork_PLGrassGymTimeSeqGet( EVENTWORK *ev )
{
	return( SysWork_Get(ev,SYS_WORK_PL_GRASS_GYM_TIME) );
}

//--------------------------------------------------------------
/**
 * メイド用勝ち抜きターンセット
 *
 * @param	ev		EVENTWORK *
 * @param	turn	ターン数
 *
 * @retval	nothing
 */
//--------------------------------------------------------------
void SysWork_MaidWinTurnSet( EVENTWORK *ev, u16 turn )
{
	//ターン数は0オリジン？のようなので+1している
	OS_Printf( "btl turn = %d\n", (turn+1) );
	SysWork_Set( ev, SYS_WORK_MAID_WIN_TURN, (turn+1) );
}

//--------------------------------------------------------------
/**
 * メイド用勝ち抜きターン取得
 *
 * @param	ev	EVENTWORK *
 *
 * @retval	"ターン数"
 */
//--------------------------------------------------------------
u16 SysWork_MaidWinTurnGet( EVENTWORK *ev )
{
	return( SysWork_Get(ev,SYS_WORK_MAID_WIN_TURN) );
}

//============================================================================================
//
//	別荘
//
//============================================================================================
#include "mapdefine.h"				//ZONE_ID_C01
//#include "zonedata.h"				//ZoneData_GetBGMID
#include "field/situation_local.h"

u16 SysWork_EventT07R0201Get( EVENTWORK *ev );
void SysWork_T07R0201Set( EVENTWORK *ev, u16 no );
void SysWorkUpdateEventT07R0201( SAVEDATA *sv );
static u8 GetEventNoT07R0201( EVENTWORK* ev );

//ランクによって必要な家具の数
#define FURNITURE_RANK_2	(8)			//第二段階
#define FURNITURE_RANK_3	(12)		//第三段階

//イベントが起こる確率
static const u8 t07r0201_event_rand[] = { 25, 75, 90 };

//ランクによって起こるイベントの総数
static const u8 t07r0201_event_num[] = { 4, 12, 15 };

//--------------------------------------------------------------
/**
 * @brief	発生するイベントを決定(0xffは発生しない)
 *
 * @param	none
 *
 * @retval	0
 */
//--------------------------------------------------------------
static u8 GetEventNoT07R0201( EVENTWORK* ev )
{
	int i;
	u16 count,r,pos;

	//家具の数を取得
	count = 0;
	for( i=0; i < VILLA_FTURE_MAX ;i++ ){
		if( SysFlag_VillaFurniture(ev,SYSFLAG_MODE_CHECK,i) == TRUE ){
			count++;
		}
	}

	if( count >= FURNITURE_RANK_3 ){
		pos = 2;
	}else if( count >= FURNITURE_RANK_2 ){
		pos = 1;
	}else{
		pos = 0;
	}

	//イベント発生するか
	r = ( gf_rand() % 100 );
	if( r > t07r0201_event_rand[pos] ){
		return 0xff;
	}

	r = ( gf_rand() % t07r0201_event_num[pos] );
	OS_Printf( "別荘イベントナンバー = %d\n", r );
	return r;
}

//--------------------------------------------------------------------------------------------
/**
 * １日１回、別荘のイベント決定
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void SysWorkUpdateEventT07R0201( SAVEDATA *sv )
{
	u32 value;
	EVENTWORK* ev			= SaveData_GetEventWork(sv);
	SITUATION* sit			= SaveData_GetSituation(sv);
	LOCATION_WORK* now		= Situation_GetNowLocation(sit);

	//更新出来るゾーンかチェック
	if( (now->zone_id != ZONE_ID_T07) && (now->zone_id != ZONE_ID_T07R0201) ){
		SysFlag_T07ObjInReset( ev );				//別荘外のOBJが中に入ったフラグoff
		SysFlag_T07R0201PosAppearReset( ev );		//POSでOBJが表示されたフラグoff
		SysWork_T07R0201Set( ev, GetEventNoT07R0201(ev) );
	}
	return;
}

#define T07R0201_TALK_RAND_MAX		(5)				//ランダム会話の種類
//--------------------------------------------------------------
/**
 * 別荘イベントナンバーセット
 *
 * @param	ev		EVENTWORK *
 * @param	no		イベントナンバー
 *
 * @retval	nothing
 */
//--------------------------------------------------------------
void SysWork_T07R0201Set( EVENTWORK *ev, u16 no )
{
	SysWork_Set( ev, SYS_WORK_EVENT_T07R0201, no );

	//会話ナンバーをセット
	SysWork_Set( ev, SYS_WORK_T07R0201_TALK, (gf_rand() % T07R0201_TALK_RAND_MAX) );
}

//--------------------------------------------------------------
/**
 * 別荘イベントナンバー取得
 *
 * @param	ev	EVENTWORK *
 *
 * @retval	"イベントナンバー"
 */
//--------------------------------------------------------------
u16 SysWork_EventT07R0201Get( EVENTWORK *ev )
{
	return( SysWork_Get(ev,SYS_WORK_EVENT_T07R0201) );
}

//--------------------------------------------------------------
/**
 * 記念プリントワークの状態を取得(ファクトリー)
 *
 * @param	ev	EVENTWORK *
 *
 * @retval	"イベントナンバー"
 */
//--------------------------------------------------------------
u16 SysWork_MemoryPrintFactory( EVENTWORK *ev )
{
	return( SysWork_Get(ev,SYS_WORK_MEMORY_PRINT_FACTORY) );
}

//--------------------------------------------------------------
/**
 * 記念プリントワークの状態を取得(ステージ)
 *
 * @param	ev	EVENTWORK *
 *
 * @retval	"イベントナンバー"
 */
//--------------------------------------------------------------
u16 SysWork_MemoryPrintStage( EVENTWORK *ev )
{
	return( SysWork_Get(ev,SYS_WORK_MEMORY_PRINT_STAGE) );
}

//--------------------------------------------------------------
/**
 * 記念プリントワークの状態を取得(キャッスル)
 *
 * @param	ev	EVENTWORK *
 *
 * @retval	"イベントナンバー"
 */
//--------------------------------------------------------------
u16 SysWork_MemoryPrintCastle( EVENTWORK *ev )
{
	return( SysWork_Get(ev,SYS_WORK_MEMORY_PRINT_CASTLE) );
}

//--------------------------------------------------------------
/**
 * 記念プリントワークの状態を取得(ルーレット)
 *
 * @param	ev	EVENTWORK *
 *
 * @retval	"イベントナンバー"
 */
//--------------------------------------------------------------
u16 SysWork_MemoryPrintRoulette( EVENTWORK *ev )
{
	return( SysWork_Get(ev,SYS_WORK_MEMORY_PRINT_ROULETTE) );
}

//--------------------------------------------------------------
/**
 * 記念プリントワークの状態を取得(タワー)
 *
 * @param	ev	EVENTWORK *
 *
 * @retval	"イベントナンバー"
 */
//--------------------------------------------------------------
u16 SysWork_MemoryPrintTower( EVENTWORK *ev )
{
	return( SysWork_Get(ev,SYS_WORK_MEMORY_PRINT_TOWER) );
}

//--------------------------------------------------------------
/**
 * 破れた世界　イベント進行状況を取得
 *
 * @param	ev	EVENTWORK *
 *
 * @retval	"イベントナンバー"
 */
//--------------------------------------------------------------
u16 SysWork_TornWorldEventSeqNoGet( EVENTWORK *ev )
{
	return( SysWork_Get(ev,SYS_WORK_TW_EVSEQ) );
}

//--------------------------------------------------------------
/**
 * 破れた世界　イベント進行状況をセット
 * @param	ev	EVENTWORK *
 * @param	no	"イベントナンバー"
 * @retval	non
 */
//--------------------------------------------------------------
void SysWork_TornWorldEventSeqNoSet( EVENTWORK *ev, u16 no )
{
	SysWork_Set( ev, SYS_WORK_TW_EVSEQ, no );
}

//--------------------------------------------------------------
/**
 * アルセウス　イベント進行状況を取得
 *
 * @param	ev	EVENTWORK *
 *
 * @retval	"イベントナンバー"
 */
//--------------------------------------------------------------
u16 SysWork_AruseusuEventGet( EVENTWORK *ev )
{
	return( SysWork_Get(ev,SYS_WORK_ARUSEUSU) );
}

//--------------------------------------------------------------
/**
 * アルセウス　イベント進行状況をセット
 * @param	ev	EVENTWORK *
 * @param	no	"イベントナンバー"
 * @retval	non
 */
//--------------------------------------------------------------
void SysWork_AruseusuEventSet( EVENTWORK *ev, u16 no )
{
	SysWork_Set( ev, SYS_WORK_ARUSEUSU, no );
}

//--------------------------------------------------------------
/**
 * シェイミ　イベント進行状況を取得
 *
 * @param	ev	EVENTWORK *
 *
 * @retval	"イベントナンバー"
 */
//--------------------------------------------------------------
u16 SysWork_SyeimiEventGet( EVENTWORK *ev )
{
	return( SysWork_Get(ev,SYS_WORK_SYEIMI) );
}

//--------------------------------------------------------------
/**
 * シェイミ　イベント進行状況をセット
 * @param	ev	EVENTWORK *
 * @param	no	"イベントナンバー"
 * @retval	non
 */
//--------------------------------------------------------------
void SysWork_SyeimiEventSet( EVENTWORK *ev, u16 no )
{
	SysWork_Set( ev, SYS_WORK_SYEIMI, no );
}

//--------------------------------------------------------------
/**
 * ムーバス　捕獲したかを取得
 *
 * @param	ev	EVENTWORK *
 *
 * @retval	"捕獲したか"
 */
//--------------------------------------------------------------
u16 SysWork_MuubasuGet( EVENTWORK *ev )
{
	return( SysWork_Get(ev,SYS_WORK_MUUBASU_GET) );
}

//--------------------------------------------------------------
/**
 * ムーバス　捕獲したかをセット
 * @param	ev	EVENTWORK *
 * @param	no	"イベントナンバー"
 * @retval	non
 */
//--------------------------------------------------------------
void SysWork_MuubasuSet( EVENTWORK *ev, u16 no )
{
	SysWork_Set( ev, SYS_WORK_MUUBASU_GET, no );
}

//--------------------------------------------------------------
/**
 * アイ　捕獲したかを取得
 *
 * @param	ev	EVENTWORK *
 *
 * @retval	"捕獲したか"
 */
//--------------------------------------------------------------
u16 SysWork_AiGet( EVENTWORK *ev )
{
	return( SysWork_Get(ev,SYS_WORK_AI_GET) );
}

//--------------------------------------------------------------
/**
 * アイ　捕獲したかをセット
 * @param	ev	EVENTWORK *
 * @param	no	"イベントナンバー"
 * @retval	non
 */
//--------------------------------------------------------------
void SysWork_AiSet( EVENTWORK *ev, u16 no )
{
	SysWork_Set( ev, SYS_WORK_AI_GET, no );
}

//--------------------------------------------------------------
/**
 * ファイヤー　捕獲したかを取得
 *
 * @param	ev	EVENTWORK *
 *
 * @retval	"捕獲したか"
 */
//--------------------------------------------------------------
u16 SysWork_FaiyaaGet( EVENTWORK *ev )
{
	return( SysWork_Get(ev,SYS_WORK_FAIYAA_GET) );
}

//--------------------------------------------------------------
/**
 * ファイヤー　捕獲したかをセット
 * @param	ev	EVENTWORK *
 * @param	no	"イベントナンバー"
 * @retval	non
 */
//--------------------------------------------------------------
void SysWork_FaiyaaSet( EVENTWORK *ev, u16 no )
{
	SysWork_Set( ev, SYS_WORK_FAIYAA_GET, no );
}

//--------------------------------------------------------------
/**
 * サンダー　捕獲したかを取得
 *
 * @param	ev	EVENTWORK *
 *
 * @retval	"捕獲したか"
 */
//--------------------------------------------------------------
u16 SysWork_SandaaGet( EVENTWORK *ev )
{
	return( SysWork_Get(ev,SYS_WORK_SANDAA_GET) );
}

//--------------------------------------------------------------
/**
 * サンダー　捕獲したかをセット
 * @param	ev	EVENTWORK *
 * @param	no	"イベントナンバー"
 * @retval	non
 */
//--------------------------------------------------------------
void SysWork_SandaaSet( EVENTWORK *ev, u16 no )
{
	SysWork_Set( ev, SYS_WORK_SANDAA_GET, no );
}

//--------------------------------------------------------------
/**
 * フリーザー　捕獲したかを取得
 *
 * @param	ev	EVENTWORK *
 *
 * @retval	"捕獲したか"
 */
//--------------------------------------------------------------
u16 SysWork_HuriizaaGet( EVENTWORK *ev )
{
	return( SysWork_Get(ev,SYS_WORK_HURIIZAA_GET) );
}

//--------------------------------------------------------------
/**
 * フリーザー　捕獲したかをセット
 * @param	ev	EVENTWORK *
 * @param	no	"イベントナンバー"
 * @retval	non
 */
//--------------------------------------------------------------
void SysWork_HuriizaaSet( EVENTWORK *ev, u16 no )
{
	SysWork_Set( ev, SYS_WORK_HURIIZAA_GET, no );
}

//--------------------------------------------------------------
/**
 * 移動ポケモン　ワークセット
 *
 * @param	ev	EVENTWORK *
 * @param	monsno	モンスターナンバー
 * @param	num		ワークにセットする値
 *
 * @retval	non
 *
 * num = 2 倒した
 * num = 1 捕獲した
 */
//--------------------------------------------------------------
void SysWork_MovePokeWorkSet( EVENTWORK* ev, u16 monsno, u16 num )
{
	switch( monsno ){

	case MONSNO_AI:
		SysWork_AiSet( ev, num );
		break;

	case MONSNO_MUUBASU:
		SysWork_MuubasuSet( ev, num );
		break;

	case MONSNO_FAIYAA:
		SysWork_FaiyaaSet( ev, num );
		break;

	case MONSNO_SANDAA:
		SysWork_SandaaSet( ev, num );
		break;

	case MONSNO_HURIIZAA:
		SysWork_HuriizaaSet( ev, num );
		break;
	};

	return;
}

//--------------------------------------------------------------
/**
 * 破れた世界 アカギ出現フラグ取得
 * @param	ev	EVENTWORK *
 * @retval	u16	フラグ内容
 */
//--------------------------------------------------------------
u16 SysWork_TwAkagiAppearFlagGet( EVENTWORK *ev )
{
	return( SysWork_Get(ev,SYS_WORK_TW_AKAGI_APPEAR) );
}

//--------------------------------------------------------------
/**
 * 破れた世界 アカギ出現フラグセット
 * @param	ev	EVENTWORK *
 * @param	set	セット番号
 * @retval	non
 */
//--------------------------------------------------------------
void SysWork_TwAkagiAppearFlagSet( EVENTWORK *ev, u16 set )
{
	SysWork_Set( ev, SYS_WORK_TW_AKAGI_APPEAR, set );
}

//--------------------------------------------------------------------------------------------
/**
 * WIFIフロンティアのTEMPクリアフラグ取得
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @return	"0=クリアしていない、1=クリアしている"
 */
//--------------------------------------------------------------------------------------------
u16 SysWork_WifiFrClearFlagGet( EVENTWORK* ev )
{
	return SysWork_Get( ev, SYS_WORK_WIFI_FR_CLEAR_FLAG );
}



//============================================================================================
/**
 * @file	scr_wazaoshie.c
 * @bfief	スクリプトコマンド：技教え関連(非常中フィールド)
 * @author	Satoshi Nohara
 * @date	07.12.17
 *
 * リスト処理の似たものがscr_field.cにも存在している
 */
//============================================================================================
#include "common.h"
#include "system/lib_pack.h"
#include "fieldsys.h"

#include "script.h"
#include "scrcmd_def.h"
#include "field/eventflag.h"
#include "field/evwkdef.h"
#include "sysflag.h"
#include "syswork.h"
#include "scrcmd.h"
#include "ev_pokemon.h"
#include "itemtool/myitem.h"
#include "itemtool/itemsym.h"

#include "battle/wazano_def.h"
#include "battle/battle_server.h"		//フォルム定義

#include "system/wipe.h"
#include "system/fontproc.h"
#include "system/window.h"
#include "system/snd_tool.h"
#include "system/bmp_list.h"
#include "scr_wazaoshie.h"
#include "msgdata\msg.naix"
#include "msgdata\msg_ev_win.h"
#include "ev_win.h"
#include "fieldmap.h"
#include "fld_bmp.h"


//==============================================================================================
//
//	技教え構造体
//
//==============================================================================================
#define WAZA_NUM_MAX			(5)
typedef struct{
	u8 waza_1[WAZA_NUM_MAX];	//ビット単位で技教えられるかを持っている
}WAZA_OSHIE_DATA;
#include "../fielddata/wazaoshie/waza_oshie.dat"

#if 0
	フォルム違い一覧(07.12.17 西野さんより)	

	ポケモン名	パターン	パーソナル
	アンノーン	２８種		×			
	デオキシス	４種		○			
	エウリス	２種		○			
	プラズマ	６種		○			
	キマイラン	２種		○
	ミノメス	３種		○
	ミノムッチ	３種		×
	シーウシ	２種		×
	シードルゴ	２種		×
	ポワルン	４種		×
	チェリシュ	２種		×
	アウス		１７種		×

	パーソナルが複数あるもののみ、技覚えの対応も複数ある

	データリストの一番下に、別フォルム(といっていいのか微妙だが)はまとまっている
#endif

typedef struct{
	u16 waza;
	u8 aka;
	u8 ao;
	u8 ki;
	u8 midori;
	u32 color;				//青＝技、赤＝力、黄＝心
}WAZA_OSHIE_LIST;

//教えてくれる技のリスト(★ポケモン別技習得リストにあわせる)
static const WAZA_OSHIE_LIST waza_oshie_tbl[] = {
	{ WAZANO_DAIBINGU,		2,	4,	2,	0, WAZA_OSHIE_COL_BLUE },
	{ WAZANO_DOROKAKE,		4,	4,	0,	0, WAZA_OSHIE_COL_RED },
	{ WAZANO_RENZOKUGIRI,	0,	8,	0,	0, WAZA_OSHIE_COL_BLUE },
	{ WAZANO_KOGOERUKAZE,	0,	6,	0,	2, WAZA_OSHIE_COL_BLUE },
	{ WAZANO_KOROGARU,		4,	2,	0,	2, WAZA_OSHIE_COL_RED },
	{ WAZANO_KAMINARIPANTI,	2,	6,	0,	0, WAZA_OSHIE_COL_BLUE },
	{ WAZANO_HONOONOPANTI,	2,	6,	0,	0, WAZA_OSHIE_COL_BLUE },

	{ WAZANO_BAKADIKARA,	8,	0,	0,	0, WAZA_OSHIE_COL_RED },

	{ WAZANO_REITOUPANTI,	2,	6,	0,	0, WAZA_OSHIE_COL_BLUE },
	{ WAZANO_AIANHEDDO,		6,	0,	2,	0, WAZA_OSHIE_COL_RED },
	{ WAZANO_AKUATEERU,		6,	0,	0,	2, WAZA_OSHIE_COL_RED },
	{ WAZANO_AYASIIKAZE,	0,	6,	0,	2, WAZA_OSHIE_COL_BLUE },
	{ WAZANO_IEKI,			4,	0,	2,	2, WAZA_OSHIE_COL_RED },
	{ WAZANO_IBIKI,			2,	0,	4,	2, WAZA_OSHIE_COL_YELLOW },
	{ WAZANO_URAMI,			0,	0,	8,	0, WAZA_OSHIE_COL_YELLOW },
	{ WAZANO_EAKATTAA,		2,	4,	0,	2, WAZA_OSHIE_COL_BLUE },

	//{ WAZANO_GAMAN,		0,	0,	6,	2, WAZA_OSHIE_COL_YELLOW },
	{ WAZANO_TEDASUKE,		2,	0,	4,	2, WAZA_OSHIE_COL_YELLOW },

	{ WAZANO_GAMUSYARA,		4,	0,	4,	0, WAZA_OSHIE_COL_RED },
	{ WAZANO_GEKIRIN,		6,	0,	2,	0, WAZA_OSHIE_COL_RED },
	{ WAZANO_GENSINOTIKARA,	6,	0,	0,	2, WAZA_OSHIE_COL_RED },
	{ WAZANO_KOUGOUSEI,		0,	0,	2,	6, WAZA_OSHIE_COL_YELLOW },
	{ WAZANO_SIGUNARUBIIMU,	2,	2,	2,	2, WAZA_OSHIE_COL_RED },
	{ WAZANO_SINENNOZUTUKI,	0,	4,	4,	0, WAZA_OSHIE_COL_BLUE },
	{ WAZANO_SINKUUHA,		2,	4,	0,	2, WAZA_OSHIE_COL_BLUE },
	{ WAZANO_DAITINOTIKARA,	6,	0,	0,	2, WAZA_OSHIE_COL_RED },
	{ WAZANO_DASUTOSYUUTO,	4,	2,	0,	2, WAZA_OSHIE_COL_RED },
	{ WAZANO_TATUMAKI,		6,	0,	0,	2, WAZA_OSHIE_COL_RED },
	{ WAZANO_TANEBAKUDAN,	4,	0,	0,	4, WAZA_OSHIE_COL_RED },
	{ WAZANO_TEPPEKI,		4,	2,	2,	0, WAZA_OSHIE_COL_RED },
	{ WAZANO_DENZIHUYUU,	0,	2,	4,	2, WAZA_OSHIE_COL_YELLOW },
	{ WAZANO_TOTTEOKI,		0,	0,	0,	8, WAZA_OSHIE_COL_YELLOW },
	{ WAZANO_TOBIHANERU,	4,	0,	2,	2, WAZA_OSHIE_COL_RED },
	{ WAZANO_TORIKKU,		0,	4,	4,	0, WAZA_OSHIE_COL_BLUE },
	{ WAZANO_NEPPUU,		4,	2,	0,	2, WAZA_OSHIE_COL_RED },
	{ WAZANO_HATAKIOTOSU,	4,	4,	0,	0, WAZA_OSHIE_COL_BLUE },
	{ WAZANO_HUIUTI,		0,	6,	2,	0, WAZA_OSHIE_COL_BLUE },
	{ WAZANO_SUPIIDOSUTAA,	0,	2,	2,	4, WAZA_OSHIE_COL_YELLOW },
	{ WAZANO_SAWAGU,		0,	0,	6,	2, WAZA_OSHIE_COL_YELLOW },
};
#define WAZA_OSHIE_TBL_MAX		( NELEMS(waza_oshie_tbl) )


//============================================================================================
//
//	プロロトタイプ宣言
//
//============================================================================================
BOOL EvCmdWazaOshieDataCount(VM_MACHINE * core );
BOOL EvCmdWazaOshieChgPokeWaza( VM_MACHINE * core );
//BOOL EvCmdWazaOshiePokeStatusSetProc(VM_MACHINE * core);
//BOOL EvCmdWazaOshiePokeStatusGetResult(VM_MACHINE * core);
BOOL EvCmdWazaOshieItemCheck( VM_MACHINE * core );
BOOL EvCmdWazaOshieItemSub( VM_MACHINE * core );
BOOL EvCmdWazaOshieBmpListStart( VM_MACHINE * core );
static BOOL EvWazaOshieSelWinWait(VM_MACHINE * core);

static u16 GetWazaOshieWazaNo( u16 index );
static u16 GetWazaOshieWazaNoByWazaNo( u16 waza );
static u16 GetWazaOshieMax( void );
static u8 GetWazaOshieDataAdrs( POKEMON_PARAM* poke, u8 no );
static u16 GetWazaOshieDataCount( POKEMON_PARAM* poke, u16 color );


//============================================================================================
//
//	コマンド
//
//============================================================================================

void ov05_21F77A8(void);

// NONMATCHING
asm void ov05_21F77A8(void)
{
    push {r4, r5, r6, lr}
	add r5, r0, #0
	bl VMGetU16
	add r1, r0, #0
	add r0, r5, #0
	add r0, #0x80
	ldr r0, [r0, #0]
	bl GetEventWorkValue
	add r6, r0, #0
	add r0, r5, #0
	bl VMGetU16
	add r1, r0, #0
	add r0, r5, #0
	add r0, #0x80
	ldr r0, [r0, #0]
	bl GetEventWorkAdrs
	add r5, #0x80
	add r4, r0, #0
	ldr r0, [r5, #0]
	ldr r0, [r0, #0xc]
	bl SaveData_GetZukanWork
	add r1, r6, #0
	bl ZukanWork_GetPokeSeeFlag
	strh r0, [r4]
	mov r0, #0
	pop {r4, r5, r6, pc}
}

//--------------------------------------------------------------------------------------------
/**
 * 覚えられる技があるか
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
BOOL EvCmdWazaOshieDataCount( VM_MACHINE * core )
{
	POKEMON_PARAM* poke;
	u16 pos		= VMGetWorkValue(core);
	u16 color	= VMGetWorkValue(core);
	u16* ret_wk	= VMGetWork( core );

	//ポケモンへのポインタ取得
	poke = PokeParty_GetMemberPointer( SaveData_GetTemotiPokemon(core->fsys->savedata), pos );

	*ret_wk = GetWazaOshieDataCount( poke, color );
	return 0;
}

#if 0
//--------------------------------------------------------------------------------------------
/**
 * @brief	ポケモンステータス画面呼び出し
 *
 * @param	core	仮想マシン制御構造体へのポインタ
 *
 * @return	1		スクリプトからメイン制御に戻る
 *
 * @li EvCmdPokeStatusGetResultで、選択した位置を取得して、ワーク開放が必要
 */
//--------------------------------------------------------------------------------------------
BOOL EvCmdWazaOshiePokeStatusSetProc(VM_MACHINE * core)
{
	void** buf	= GetEvScriptWorkMemberAdrs(core->fsys, ID_EVSCR_SUBPROC_WORK);
	u16 pos		= VMGetWorkValue(core);
	u16 waza	= VMGetWorkValue(core);

	//*buf = WazaOshiePokeStatusEvent_SetProc( HEAPID_EVENT, core->fsys, pos, waza );
	*buf = WazaOshiePokeStatusEvent_SetProc( HEAPID_WORLD, core->fsys, pos, waza );

	VM_SetWait( core, EvCmdWaitSubProcEnd );
	return 1;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	ポケモンステータス画面呼び出し後の結果取得とワーク解放
 * @param	core		仮想マシン制御構造体へのポインタ
 * @return	1		スクリプトからメイン制御に戻る
 */
//--------------------------------------------------------------------------------------------
BOOL EvCmdWazaOshiePokeStatusGetResult(VM_MACHINE * core)
{
	void ** buf;
	u16 * ret_wk;
	PSTATUS_DATA* psd;

	ret_wk = VMGetWork(core);
	buf = GetEvScriptWorkMemberAdrs(core->fsys, ID_EVSCR_SUBPROC_WORK);

	GF_ASSERT(*buf != 0);

	psd = *buf;
	*ret_wk = psd->ret_sel;				//選択された技位置
	OS_Printf( "*ret_wk = %d\n", *ret_wk );
//	if (*ret_wk == PL_SEL_POS_EXIT) {
//		*ret_wk = 0xff;
//	}

	sys_FreeMemoryEz(*buf);
	*buf = NULL;

	return 0;
}
#endif

//--------------------------------------------------------------------------------------------
/**
 * 手持ちポケモンの技を置き換える(ロトムでも使用)
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
BOOL EvCmdWazaOshieChgPokeWaza( VM_MACHINE * core )
{
	u16 poke_pos= VMGetWorkValue(core);
	u16 waza_pos= VMGetWorkValue(core);
	u16 waza_no	= VMGetWorkValue(core);

	EvPoke_ChangeWaza(	SaveData_GetTemotiPokemon(core->fsys->savedata),
						poke_pos, waza_pos, waza_no );
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * 指定した技を覚えるアイテムがあるかチェック
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
BOOL EvCmdWazaOshieItemCheck( VM_MACHINE * core )
{
	int i;
	u8 aka,ao,ki,midori;
	MYITEM* my_item;
	u16 waza	= VMGetWorkValue(core);
	u16* ret_wk	= VMGetWork( core );

	OS_Printf( "waza = %d\n", waza );
	my_item = SaveData_GetMyItem( core->fsys->savedata );

	for( i=0; i < WAZA_OSHIE_TBL_MAX ;i++ ){
		if( waza == waza_oshie_tbl[i].waza ){
			aka		= waza_oshie_tbl[i].aka;
			ao		= waza_oshie_tbl[i].ao;
			ki		= waza_oshie_tbl[i].ki;
			midori	= waza_oshie_tbl[i].midori;
			break;
		}
	}

	if( i == WAZA_OSHIE_TBL_MAX ){
		OS_Printf( "技ナンバーが不正です！\n" );
		GF_ASSERT( 0 );
		*ret_wk = 0;
		return 0;
	}

	*ret_wk = 1;

	//アイテムの個数チェック
    if (aka) {
        if( MyItem_CheckItem(my_item,ITEM_AKAIKAKERA,aka,HEAPID_EVENT) == FALSE ){
            *ret_wk = 0;
        }
    }
    if (ao) {
        if( MyItem_CheckItem(my_item,ITEM_AOIKAKERA,ao,HEAPID_EVENT) == FALSE ){
            *ret_wk = 0;
        }
    }
	if (ki) {
        if( MyItem_CheckItem(my_item,ITEM_KIIROIKAKERA,ki,HEAPID_EVENT) == FALSE ){
            *ret_wk = 0;
        }
    }
    if (midori) {
        if( MyItem_CheckItem(my_item,ITEM_MIDORINOKAKERA,midori,HEAPID_EVENT) == FALSE ){
            *ret_wk = 0;
        }
    }

	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * 指定した技を覚えるアイテムを減らす
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
BOOL EvCmdWazaOshieItemSub( VM_MACHINE * core )
{
	int i;
	u8 aka,ao,ki,midori;
	MYITEM* my_item;
	u16 waza	= VMGetWorkValue(core);

	OS_Printf( "waza = %d\n", waza );
	my_item = SaveData_GetMyItem( core->fsys->savedata );

	for( i=0; i < WAZA_OSHIE_TBL_MAX ;i++ ){
		if( waza == waza_oshie_tbl[i].waza ){
			aka		= waza_oshie_tbl[i].aka;
			ao		= waza_oshie_tbl[i].ao;
			ki		= waza_oshie_tbl[i].ki;
			midori	= waza_oshie_tbl[i].midori;
			break;
		}
	}

	if( i == WAZA_OSHIE_TBL_MAX ){
		OS_Printf( "技ナンバーが不正です！\n" );
		GF_ASSERT( 0 );
	}

	//アイテム減らす
	MyItem_SubItem( my_item, ITEM_AKAIKAKERA, aka, HEAPID_EVENT );
	MyItem_SubItem( my_item, ITEM_AOIKAKERA, ao, HEAPID_EVENT );
	MyItem_SubItem( my_item, ITEM_KIIROIKAKERA, ki, HEAPID_EVENT );
	MyItem_SubItem( my_item, ITEM_MIDORINOKAKERA, midori, HEAPID_EVENT );
	return 0;
}


//==============================================================================================
//
//	static
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * 渡されたインデックスから技教えの技ナンバーを取得
 *
 * @param   index	リストの何番目か
 *
 * @retval  "技ナンバー"
 */
//--------------------------------------------------------------
static u16 GetWazaOshieWazaNo( u16 index )
{
	return waza_oshie_tbl[index].waza;
}

//--------------------------------------------------------------
/**
 * 渡された技ナンバーから技教えの技ナンバーを取得
 *
 * @param   waza	技ナンバー
 *
 * @retval  "技ナンバー"
 */
//--------------------------------------------------------------
static u16 GetWazaOshieWazaNoByWazaNo( u16 waza )
{
	int i;

	for( i=0; i < WAZA_OSHIE_TBL_MAX ;i++ ){
		if( waza_oshie_tbl[i].waza == waza ){
			return i;
		}
	}

	GF_ASSERT( (0) && "技ナンバーが不正です！" );
	return 0;
}

//--------------------------------------------------------------
/**
 * 技教えのリストの最大数を取得
 *
 * @param   none
 *
 * @retval  "最大数"
 */
//--------------------------------------------------------------
static u16 GetWazaOshieMax( void )
{
	return WAZA_OSHIE_TBL_MAX;
}

//★エクセルデータの並びがずれたら対応しないとダメ！(参照する時に-1している)
enum{
	WAZAOSHIE_DEO_A		= 494,
	WAZAOSHIE_DEO_D		= 495,
	WAZAOSHIE_DEO_S		= 496,

	WAZAOSHIE_MINO_G	= 497,
	WAZAOSHIE_MINO_M	= 498,

	WAZAOSHIE_KIMA		= 499,

	WAZAOSHIE_EU		= 500,

	WAZAOSHIE_PURA_1	= 501,
	WAZAOSHIE_PURA_2	= 502,
	WAZAOSHIE_PURA_3	= 503,
	WAZAOSHIE_PURA_4	= 504,
	WAZAOSHIE_PURA_5	= 505,
};

//--------------------------------------------------------------
/**
 * エクセルをコンバートしたデータリストから、データのアドレスを取得
 *
 * @param   monsno	取得したいモンスターナンバー
 *
 * @retval  "データ"
 */
//--------------------------------------------------------------
static u8 GetWazaOshieDataAdrs( POKEMON_PARAM* poke, u8 no )
{
	u16 index;
	u32 form,monsno;

	monsno	= PokeParaGet( poke, ID_PARA_monsno, NULL );
	form	= PokeParaGet( poke, ID_PARA_form_no, NULL );
	index	= monsno;

	//フォルムチェックして必要なインデックスを取得
		
	switch( monsno ){

	//デオキシス
	case MONSNO_DEOKISISU:
		if( form == FORMNO_DEOKISISU_ATTACK ){
			index = WAZAOSHIE_DEO_A;
		}else if( form == FORMNO_DEOKISISU_DEFENCE ){
			index = WAZAOSHIE_DEO_D;
		}else if( form == FORMNO_DEOKISISU_SPEED ){
			index = WAZAOSHIE_DEO_S;
		}
		break;

	//ミノメス
	case MONSNO_MINOMESU:
		if( form == FORMNO_MINOMUTTI_SUNA ){
			index = WAZAOSHIE_MINO_G;
		}else if( form == FORMNO_MINOMUTTI_TETSU ){
			index = WAZAOSHIE_MINO_M;
		}
		break;

	//キマイラン
	case MONSNO_KIMAIRAN:
		if( form == FORMNO_GIRATINA_ORIGIN ){
			index = WAZAOSHIE_KIMA;
		}
		break;

	//エウリス
	case MONSNO_EURISU:
		if( form == FORMNO_SHEIMI_FLOWER ){
			index = WAZAOSHIE_EU;
		}
		break;

	//プラズマ
	case MONSNO_PURAZUMA:
		if( form == FORMNO_ROTOMU_HOT ){
			index = WAZAOSHIE_PURA_1;
		}else if( form == FORMNO_ROTOMU_WASH ){
			index = WAZAOSHIE_PURA_2;
		}else if( form == FORMNO_ROTOMU_COLD ){
			index = WAZAOSHIE_PURA_3;
		}else if( form == FORMNO_ROTOMU_FAN ){
			index = WAZAOSHIE_PURA_4;
		}else if( form == FORMNO_ROTOMU_CUT ){
			index = WAZAOSHIE_PURA_5;
		}
		break;

	};

	//return &waza_oshie_data[index-1].waza_1;
	return waza_oshie_data[index-1].waza_1[no];
}

//--------------------------------------------------------------
/**
 * 技教えで覚えられる技があるかチェック
 *
 * @param   monsno	モンスターナンバー
 *
 * @retval  "TRUE = ある、FALSE = ない"
 */
//--------------------------------------------------------------
static u16 GetWazaOshieDataCount( POKEMON_PARAM* poke, u16 color )
{
	int i,bit,waza_i;
	u8 data,num;
	u32 monsno;
	u16 poke_waza[4];

	monsno = PokeParaGet( poke, ID_PARA_monsno, NULL );

	//ポケモンが覚えてる技を取得
	for( waza_i=0; waza_i < 4 ;waza_i++ ){
		poke_waza[waza_i] = PokeParaGet( poke, (ID_PARA_waza1 + waza_i), NULL );
	}

	//データ取得

	for( i=0; i < WAZA_NUM_MAX ;i++ ){
		data = GetWazaOshieDataAdrs( poke, i );
		OS_Printf( "data = %d\n", data );
		for( bit=0; bit < 8 ;bit++ ){

			num = ( (data >> bit) & 0x01 );

			//覚えられるビットが立っていて、渡されたカラーと同じかチェック
			if( (num == 1) && (color == waza_oshie_tbl[i*8+bit].color) ){

				//すでにポケモンが覚えてる技は除外
				for( waza_i=0; waza_i < 4 ;waza_i++ ){
					if( poke_waza[waza_i] == waza_oshie_tbl[i*8+bit].waza ){
						break;
					}
				}

				if( waza_i == 4 ){
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}


//==============================================================================================
//
//	ev_winのリスト処理を元にしている
//
//==============================================================================================
//有効で、表示数より項目数が多いリスト処理は、上下にスクロールカーソルを表示
//グラフィックデータがないので、その辺は仮です。
//#define EV_WIN_SCROLL_CURSOR

#ifdef EV_WIN_SCROLL_CURSOR	//070228
#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "system/clact_tool.h"
#include "field_clact.h"
#include "field/ranking.naix"			//グラフィックデータがないので仮
#endif


//==============================================================================================
//
//	プロトタイプ宣言
//
//==============================================================================================
static void EvWin_MsgManSet( WAZA_OSHIE_WORK* wk, MSGDATA_MANAGER* msgman );
static void EvWin_Init( FIELDSYS_WORK* fsys, WAZA_OSHIE_WORK* wk, u8 x, u8 y, u8 cursor, u8 cancel, u16* work, WORDSET* wordset, GF_BGL_BMPWIN* talk_bmpwin, MSGDATA_MANAGER* msgman );

//BMPリスト
WAZA_OSHIE_WORK * CmdWazaOshieBmpList_Init( FIELDSYS_WORK* fsys, u8 x, u8 y, u8 cursor, u8 cancel, u16* work, WORDSET* wordset, GF_BGL_BMPWIN* talk_bmpwin, MSGDATA_MANAGER* msgman );
void CmdWazaOshieBmpList_MakeList( WAZA_OSHIE_WORK* wk, u32 msg_id, u32 talk_msg_id, u32 param  );
static void CmdWazaOshieBmpList_Start( WAZA_OSHIE_WORK* wk );

static void BmpList_list_make( WAZA_OSHIE_WORK* wk, u32 msg_id, u32 talk_msg_id, u32 param  );
static u32 BmpList_length_get( WAZA_OSHIE_WORK* wk );
static void BmpList_h_default_set(WAZA_OSHIE_WORK* wk);
static void BmpList_CursorMoveCallBack( BMPLIST_WORK* wk, u32 param, u8 mode );
static void EvBmpList_MainTCB( TCB_PTR tcb, void* wk );
static void EvBmpList_Del( WAZA_OSHIE_WORK* wk );

#ifdef EV_WIN_SCROLL_CURSOR	//070228
static void EvWin_ActorInit( WAZA_OSHIE_WORK* wk );
static void EvWin_ActorRelease( WAZA_OSHIE_WORK* wk );
#endif


//--------------------------------------------------------------
/**
 * @brief	BMPリスト	初期化、リスト作成、開始
 *
 * @param	none
 *
 * @retval	1
 *
 * pos = 0xffの時は、ボード用に全ての技をセット
 */
//--------------------------------------------------------------
BOOL EvCmdWazaOshieBmpListStart( VM_MACHINE * core )
{
	u8 data,num;
	int i,set_pos,bit,waza_i;
	POKEMON_PARAM* poke;
	MSGDATA_MANAGER* man;
	MSGDATA_MANAGER* ev_win_man;
	FIELDSYS_WORK* fsys	= core->fsys;
	WAZA_OSHIE_WORK* waza_oshie_win;
	u16 poke_waza[4];
	u16 waza[WAZA_OSHIE_TBL_MAX];
	WORDSET** wordset			= GetEvScriptWorkMemberAdrs( fsys, ID_EVSCR_WORDSET );
	u16 pos						= VMGetWorkValue( core );
	u16 color					= VMGetWorkValue( core );
	u16 wk_id					= VMGetU16( core );

	//仮想マシンの汎用レジスタにワークのIDを格納
	core->reg[0] = wk_id;

	//ポケモンへのポインタ取得
	if( pos != 0xff ){
		poke = PokeParty_GetMemberPointer( SaveData_GetTemotiPokemon(core->fsys->savedata), pos );
	}

	//技名
	man = MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, NARC_msg_wazaname_dat, HEAPID_EVENT );

	//evwin初期化
	waza_oshie_win	= CmdWazaOshieBmpList_Init(	fsys, 20, 1, 0, 1, 
									GetEventWorkAdrs(fsys,wk_id), *wordset, 
									GetEvScriptWorkMemberAdrs(core->fsys,ID_EVSCR_MSGWINDAT),
									man );

	//クリア
	for( i=0; i < WAZA_OSHIE_TBL_MAX ;i++ ){
		waza[i] = 0;
	}

	set_pos = 0;

	//覚えられる技をセット
	if( pos != 0xff ){

		//ポケモンが覚えてる技を取得
		for( waza_i=0; waza_i < 4 ;waza_i++ ){
			poke_waza[waza_i] = PokeParaGet( poke, (ID_PARA_waza1 + waza_i), NULL );
		}

		for( i=0; i < WAZA_NUM_MAX ;i++ ){
			data = GetWazaOshieDataAdrs( poke, i );
			OS_Printf( "data = %d\n", data );
			for( bit=0; bit < 8 ;bit++ ){
				//OS_Printf( "要素 = %d\n", i*8+bit );
				//OS_Printf( "wazano = %d\n", waza_oshie_tbl[i*8+bit] );
				//OS_Printf( "data = %d\n", data );
				num = ( (data >> bit) & 0x01 );

				//覚えられるビットが立っていて、渡されたカラーと同じかチェック
				if( (num == 1) && (color == waza_oshie_tbl[i*8+bit].color) ){
				//if( num == 1 ){
				
					//すでにポケモンが覚えてる技は除外
					for( waza_i=0; waza_i < 4 ;waza_i++ ){
						if( poke_waza[waza_i] == waza_oshie_tbl[i*8+bit].waza ){
							break;
						}
					}

					if( waza_i == 4 ){
						waza[set_pos] = waza_oshie_tbl[i*8+bit].waza;
						set_pos++;
					}
				}
			}
		}
	}else{
		//ボード用に全てセット
		for( i=0; i < WAZA_OSHIE_TBL_MAX ;i++ ){
			//waza[i] = GetWazaOshieWazaNo( i );
			//渡されたカラーと同じかチェック
			if( color == waza_oshie_tbl[i].color ){
				waza[set_pos] = GetWazaOshieWazaNo( i );
				set_pos++;
			}
		}
		//set_pos = WAZA_OSHIE_TBL_MAX;
	}

	//リスト作成
	for( i=0; i < set_pos ;i++ ){
		CmdWazaOshieBmpList_MakeList( waza_oshie_win, waza[i], EV_WIN_TALK_MSG_NONE, waza[i] );
	}

#if 1
	//ev_win.gmmから「やめる」を使用している
	
	ev_win_man = MSGMAN_Create( MSGMAN_TYPE_DIRECT, ARC_MSG, NARC_msg_ev_win_dat, HEAPID_EVENT );
	EvWin_MsgManSet( waza_oshie_win, ev_win_man );
	CmdWazaOshieBmpList_MakeList(	waza_oshie_win, msg_ev_win_006, 
									EV_WIN_TALK_MSG_NONE, EV_WIN_B_CANCEL );
	MSGMAN_Delete( ev_win_man );

	//一応戻す
	EvWin_MsgManSet( waza_oshie_win, man );
#endif

	//リスト開始
	CmdWazaOshieBmpList_Start( waza_oshie_win );
	
	VM_SetWait( core, EvWazaOshieSelWinWait );

	MSGMAN_Delete( man );
	return 1;
}

//ウェイト関数
static BOOL EvWazaOshieSelWinWait(VM_MACHINE * core)
{
	FIELDSYS_WORK* fsys = core->fsys;
	u16* ret_wk = GetEventWorkAdrs( fsys, core->reg[0] );	//注意！

	if( *ret_wk == EV_WIN_NOTHING ){
		return FALSE;	//継続
	}

	return TRUE;		//終了
}


//==============================================================================================
//
//	定義
//
//==============================================================================================
#define EV_WIN_FONT				(FONT_SYSTEM)	//フォント指定

#define EVWIN_MSG_BUF_SIZE		(40*2)			//メッセージバッファサイズ

#define EV_WIN_LIST_MAX			(WAZA_OSHIE_TBL_MAX+1)			//BMPリスト項目の最大数
#define	EV_WIN_MSG_MAX			(WAZA_OSHIE_TBL_MAX+1)			//MSGバッファの最大数

//BMPリストヘッダー定義
#define EV_LIST_LINE			(8)				//表示最大項目数
#define EV_LIST_RABEL_X			(1)				//ラベル表示Ｘ座標
#define EV_LIST_DATA_X			(12)			//項目表示Ｘ座標
#define EV_LIST_CURSOR_X		(2)				//カーソル表示Ｘ座標
#define EV_LIST_LINE_Y			(1)				//表示Ｙ座標

//カーソル幅
#define EV_WIN_CURSOR_WIDTH		(12)			//カーソル幅

#define EV_WIN_DEFAULT_WAIT		(3)				//キー操作がすぐに入らないように基本ウェイト

// ↓ここから使って下さい！！
#define	EVWIN_FREE_CGX			( 1 )			//フリーのキャラ位置

#ifdef EV_WIN_SCROLL_CURSOR	//070228
#define CURSOR_ANMNO				(0)
#define CURSOR_SPRI					(0)
#define CURSOR_PALT					(1)

#define EVWIN_CHR_H_ID_CURSOR		(13528)
#define EVWIN_PAL_H_ID				(13528)
#define EVWIN_CEL_H_ID_CURSOR		(13528)
#define EVWIN_ANM_H_ID_CURSOR		(13528)

#define EVWIN_ACTMAX				(2)			//アクター数(上向きと下向きの２つ)

#define ACT_RES_PAL_NUM				(3)

//グラフィックデータがないので置き換え
enum{
	ARC_EVWIN_GRA	= ARC_RANKING_GRA,
	NARC_evwin_nclr = NARC_ranking_ranking_nclr,
	NARC_evwin_ncer = NARC_ranking_ranking_ncer,
	NARC_evwin_nanr = NARC_ranking_ranking_nanr,
	NARC_evwin_ncgr = NARC_ranking_ranking_ncgr,
};
#endif


//==============================================================================================
//
//	構造体
//
//==============================================================================================
struct _WAZA_OSHIE_WORK{
	FIELDSYS_WORK * fsys;						//FIELDSYS_WORKのポインタ
	TCB_PTR	tcb;

 	GF_BGL_BMPWIN bmpwin;						//BMPウィンドウデータ
 	GF_BGL_BMPWIN* talk_bmpwin;					//BMP会話ウィンドウデータ
	
	STRBUF* msg_buf[EV_WIN_MSG_MAX];			//メッセージデータのポインタ
	MSGDATA_MANAGER* msgman;					//メッセージマネージャー
	WORDSET* wordset;							//単語セット

	u8  wait;									//ウェイト
	u8  menu_id;								//BMPメニューID
	u8  cursor_pos;								//カーソル位置
	u8  cancel:1;								//キャンセル
	u8  msgman_del_flag:1;						//メッセージマネージャー削除フラグ
	u8  dmy:6;									//

	u8  x;										//ウィンドウ位置X
	u8  y;										//ウィンドウ位置Y
	u8  dmyy;									//メニューナンバー
	u8  list_no;								//メニュー項目の何番目か

	u16* pMsg;									//メッセージデータ
	u16* work;									//結果取得ワーク
 
	//BMPリスト
	BMPLIST_HEADER ListH;						//BMPリストヘッダー
	BMPLIST_WORK* lw;							//BMPリストデータ
	u16 list_bak;								//リスト位置バックアップ
	u16 cursor_bak;								//カーソル位置バックアップ
	BMPLIST_DATA list_Data[EV_WIN_LIST_MAX];	//リストデータ
	u16 talk_msg_id[EV_WIN_LIST_MAX];			//リストデータに対になる会話メッセージID

	u16 pos_bak;

#ifdef EV_WIN_SCROLL_CURSOR	//070228
	FIELD_CLACT		fcat;						//フィールド用セルアクター設定
	CATS_ACT_PTR	act[EVWIN_ACTMAX];			//CATSを使う時の専用アクター構造体
#endif
};


//==============================================================================================
//
//	関数
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	イベントウィンドウ　メッセージマネージャーセット
 *
 * @param	wk			WAZA_OSHIE_WORK型のポインタ
 * @param	msgman		MSGDATA_MANAGER型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void EvWin_MsgManSet( WAZA_OSHIE_WORK* wk, MSGDATA_MANAGER* msgman )
{
	wk->msgman = msgman;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	イベントウィンドウ　ワーク初期化
 *
 * @param	wk			WAZA_OSHIE_WORK型のポインタ
 * @param	x			ウィンドウ表示X座標
 * @param	y			ウィンドウ表示X座標
 * @param	cursor		カーソル位置
 * @param	cancel		Bキャンセルフラグ(TRUE=有効、FALSE=無効)
 * @param	work		結果を代入するワークのポインタ
 * @param	wordset		WORDSET型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void EvWin_Init( FIELDSYS_WORK* fsys, WAZA_OSHIE_WORK* wk, u8 x, u8 y, u8 cursor, u8 cancel, u16* work, WORDSET* wordset, GF_BGL_BMPWIN* talk_bmpwin, MSGDATA_MANAGER* msgman )
{
	int i;

	wk->msgman		= msgman;		//引継ぎ
	wk->msgman_del_flag = 0;
	wk->wordset		= wordset;		//スクリプトを引き継ぐ

	wk->fsys		= fsys;
	wk->work		= work;
	wk->cancel		= cancel;
	wk->cursor_pos	= cursor;
	wk->x			= x;
	wk->y			= y;
	wk->list_no		= 0;
	wk->talk_bmpwin	= talk_bmpwin;
	wk->wait		= EV_WIN_DEFAULT_WAIT;
	wk->pos_bak		= cursor;

	for( i=0; i < EV_WIN_LIST_MAX ;i++ ){
		wk->list_Data[i].str	= NULL;
		wk->list_Data[i].param	= 0;
		wk->talk_msg_id[i] = EV_WIN_TALK_MSG_NONE;
	}

	//MSGMAN_Createの後に処理
	for( i=0; i < EV_WIN_MSG_MAX ;i++ ){
		wk->msg_buf[i] = STRBUF_Create( EVWIN_MSG_BUF_SIZE, HEAPID_FIELD );
	}

	//選択した値を取得するワークを初期化
	*wk->work = EV_WIN_NOTHING;

	return;
}


//==============================================================================================
//
//	BMPリスト関連
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	BMPリスト　初期化
 *
 * @param	x			ウィンドウ表示X座標
 * @param	y			ウィンドウ表示X座標
 * @param	cursor		カーソル位置
 * @param	cancel		Bキャンセルフラグ(TRUE=有効、FALSE=無効)
 * @param	work		結果を代入するワークのポインタ
 * @param	wordset		WORDSET型のポインタ
 *
 * @retval	"WAZA_OSHIE_WORK型のアドレス、NULLは失敗"
 */
//--------------------------------------------------------------
WAZA_OSHIE_WORK * CmdWazaOshieBmpList_Init( FIELDSYS_WORK* fsys, u8 x, u8 y, u8 cursor, u8 cancel, u16* work, WORDSET* wordset, GF_BGL_BMPWIN* talk_bmpwin, MSGDATA_MANAGER* msgman )
{
	WAZA_OSHIE_WORK* wk;
	int i;

	wk = sys_AllocMemory( HEAPID_FIELD, sizeof(WAZA_OSHIE_WORK) );
	if( wk == NULL ){
		OS_Printf( "ev_win.c Alloc ERROR!" );
		return NULL;
	}
	memset( wk, 0, sizeof(WAZA_OSHIE_WORK) );

	//ワーク初期化	
	EvWin_Init( fsys, wk, x, y, cursor, cancel, work, wordset, talk_bmpwin, msgman );

	return wk;
}

//--------------------------------------------------------------
/**
 * @brief	BMPリスト　リスト作成
 *
 * @param	wk			WAZA_OSHIE_WORK型のポインタ
 * @param	msg_id		メッセージID
 * @param	talk_msg_id	会話メッセージID
 * @param	param		BMPLISTパラメータ
 *
 * @retval	none
 */
//--------------------------------------------------------------
void CmdWazaOshieBmpList_MakeList( WAZA_OSHIE_WORK* wk, u32 msg_id, u32 talk_msg_id, u32 param  )
{
	BmpList_list_make( wk, msg_id, talk_msg_id, param  );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	BMPリスト　開始
 *
 * @param	wk			WAZA_OSHIE_WORK型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void CmdWazaOshieBmpList_Start( WAZA_OSHIE_WORK* wk )
{
	//表示最大項目数チェック
	if( wk->list_no > EV_LIST_LINE ){
		GF_BGL_BmpWinAdd( wk->fsys->bgl, &wk->bmpwin, FLD_MBGFRM_FONT, wk->x, wk->y, 
									11, EV_LIST_LINE*2, FLD_SYSFONT_PAL, EVWIN_FREE_CGX );
	}else{
		GF_BGL_BmpWinAdd( wk->fsys->bgl, &wk->bmpwin, FLD_MBGFRM_FONT, wk->x, wk->y, 
									11, wk->list_no*2, FLD_SYSFONT_PAL, EVWIN_FREE_CGX );
	}

	//メニューウィンドウのグラフィックをセット
	MenuWinGraphicSet(
		wk->fsys->bgl, FLD_MBGFRM_FONT, MENU_WIN_CGX_NUM, MENU_WIN_PAL, 0, HEAPID_FIELD );

	//メニューウィンドウを描画
	BmpMenuWinWrite(&wk->bmpwin, WINDOW_TRANS_OFF, MENU_WIN_CGX_NUM, MENU_WIN_PAL);

#ifdef EV_WIN_SCROLL_CURSOR	//070228
	EvWin_ActorInit( wk );
#endif

	//リストヘッダー初期設定
	BmpList_h_default_set(wk);

	wk->lw = BmpListSet( (const BMPLIST_HEADER*)&wk->ListH, 0, wk->cursor_pos, HEAPID_FIELD );

	//TCB追加
	wk->tcb	= TCB_Add( EvBmpList_MainTCB, wk, 0 );

	return;
}

//--------------------------------------------------------------
/**
 * @brief	BMPリスト	リスト作成
 *
 * @param	wk			WAZA_OSHIE_WORK型のポインタ
 * @param	msg_id		メッセージID
 * @param	talk_msg_id	会話メッセージID
 * @param	param		BMPLISTパラメータ
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void BmpList_list_make( WAZA_OSHIE_WORK* wk, u32 msg_id, u32 talk_msg_id, u32 param  )
{
	int i;
	void* msg;

	SDK_ASSERTMSG( wk->list_no < EV_WIN_LIST_MAX, "リスト項目数オーバー！" );

	{
		//展開込み
		
		STRBUF* tmp_buf2= STRBUF_Create( EVWIN_MSG_BUF_SIZE, HEAPID_FIELD );	//コピー用バッファ

		MSGMAN_GetString( wk->msgman, msg_id, tmp_buf2 );
		WORDSET_ExpandStr( wk->wordset, wk->msg_buf[wk->list_no], tmp_buf2 );	//展開
		wk->list_Data[ wk->list_no ].str = (const void *)wk->msg_buf[wk->list_no];

		STRBUF_Delete( tmp_buf2 );
	}

	if( param == EV_WIN_LIST_LABEL ){
		wk->list_Data[ wk->list_no ].param = BMPLIST_RABEL;
		//*pa = BMPLIST_RABEL;
	}else{
		wk->list_Data[ wk->list_no ].param = param;
		//*pa = param;
	}

	wk->talk_msg_id[ wk->list_no ] = talk_msg_id;
	wk->list_no++;

	return;
}

//--------------------------------------------------------------
/**
 * @brief	BMPリスト	項目の中から最大文字数を取得
 *
 * @param	wk			WAZA_OSHIE_WORK型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
static u32 BmpList_length_get( WAZA_OSHIE_WORK* wk )
{
	int i;
	u32 ret,tmp_ret;

	ret		= 0;
	tmp_ret = 0;

	for( i=0; i < wk->list_no ;i++ ){
		if( wk->list_Data[i].str == NULL ){
			break;
		}

		ret = FontProc_GetPrintStrWidth( EV_WIN_FONT, (STRBUF*)wk->list_Data[i].str, 0 );
		
		if( tmp_ret < ret ){
			tmp_ret = ret;
		}
	}

	return tmp_ret + EV_WIN_CURSOR_WIDTH;
}

//--------------------------------------------------------------
/**
 * @brief	BMPリスト	ヘッダー初期設定
 *
 * @param	wk			WAZA_OSHIE_WORK型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void BmpList_h_default_set(WAZA_OSHIE_WORK* wk)
{
	wk->ListH.list		= wk->list_Data;
	wk->ListH.call_back = BmpList_CursorMoveCallBack;
	wk->ListH.icon		= NULL;
	wk->ListH.win		= &wk->bmpwin;

	wk->ListH.count		= wk->list_no;
	wk->ListH.line		= EV_LIST_LINE;

	wk->ListH.rabel_x	= EV_LIST_RABEL_X;
	wk->ListH.data_x	= EV_LIST_DATA_X;
	wk->ListH.cursor_x	= EV_LIST_CURSOR_X;
	wk->ListH.line_y	= EV_LIST_LINE_Y;

	wk->ListH.f_col		= FBMP_COL_BLACK;
	wk->ListH.b_col		= FBMP_COL_WHITE;		
	wk->ListH.s_col		= FBMP_COL_BLK_SDW;

	wk->ListH.msg_spc	= 0;
	wk->ListH.line_spc	= 16;
	wk->ListH.page_skip	= BMPLIST_LRKEY_SKIP;
	wk->ListH.font		= EV_WIN_FONT;
	wk->ListH.c_disp_f	= 0;

	wk->ListH.work		= (void*)wk;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	BMPリスト	カーソル移動ごとのコールバック
 *
 * @param	
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void BmpList_CursorMoveCallBack( BMPLIST_WORK* wk, u32 param, u8 mode )
{
	u32 count,line;
	u16 list_bak = 0;
	u16 cursor_bak = 0;
	WAZA_OSHIE_WORK* evwin_wk = (WAZA_OSHIE_WORK*)BmpListParamGet(wk,BMPLIST_ID_WORK);

#ifdef EV_WIN_SCROLL_CURSOR	//070228

	//初期化時
	if( mode == 1 ){
		//
	}

	count = BmpListParamGet( wk, BMPLIST_ID_COUNT );	//リスト項目数
	line  = BmpListParamGet( wk, BMPLIST_ID_LINE );		//表示最大項目数

	//全ての項目を表示していない時
	if( count > line ){

		BmpListPosGet( wk, &list_bak, &cursor_bak );

		if( list_bak == 0 ){
			//OS_Printf( "ウィンドウの上のスクロールカーソル非表示\n" );
			//OS_Printf( "ウィンドウの下のスクロールカーソル表示\n" );
			CATS_ObjectEnableCap( evwin_wk->act[0], FALSE );	//OBJの非表示設定
			CATS_ObjectEnableCap( evwin_wk->act[1], TRUE );		//OBJの表示設定
		}else if( list_bak == (count-line) ){
			//OS_Printf( "ウィンドウの上のスクロールカーソル表示\n" );
			//OS_Printf( "ウィンドウの下のスクロールカーソル非表示\n" );
			CATS_ObjectEnableCap( evwin_wk->act[0], TRUE );		//OBJの表示設定
			CATS_ObjectEnableCap( evwin_wk->act[1], FALSE );	//OBJの非表示設定
		}else{
			//OS_Printf( "ウィンドウの上のスクロールカーソル表示\n" );
			//OS_Printf( "ウィンドウの下のスクロールカーソル表示\n" );
			CATS_ObjectEnableCap( evwin_wk->act[0], TRUE );		//OBJの表示設定
			CATS_ObjectEnableCap( evwin_wk->act[1], TRUE );		//OBJの表示設定
		}
	}

#endif

	return;
}

//--------------------------------------------------------------
/**
 * @brief	BMPリスト　リストメイン
 *
 * @param	tcb			TCB_PTR
 * @param	wk			ワークのアドレス
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void EvBmpList_MainTCB( TCB_PTR tcb, void* wk )
{
	u16 tmp_pos_bak;
	u32	ret;
	WAZA_OSHIE_WORK* swk;
	swk = (WAZA_OSHIE_WORK*)wk;

	//ウェイト
	if( swk->wait != 0 ){
		swk->wait--;
		return;
	}

	//ワイプ中は処理しない
	if( WIPE_SYS_EndCheck() == FALSE ){
		return;
	}

	ret = BmpListMain( swk->lw );

	//BMPリストの全体位置を取得
	tmp_pos_bak = swk->pos_bak;		//退避
	BmpListDirectPosGet( swk->lw, &swk->pos_bak );

	//カーソルが動いたかチェック
	if( tmp_pos_bak != swk->pos_bak ){
		Snd_SePlay( SEQ_SE_DP_SELECT );
	}

#ifdef EV_WIN_SCROLL_CURSOR	//070228
	CLACT_Draw( swk->fcat.cas );
#endif

	switch( ret ){
	case BMPLIST_NULL:
		break;
	case BMPLIST_CANCEL:
		if( swk->cancel == TRUE ){			//TRUE = Bキャンセル有効
			Snd_SePlay( SEQ_SE_DP_SELECT );
			*swk->work = EV_WIN_B_CANCEL;	//選択した値をワークに代入
#ifdef EV_WIN_SCROLL_CURSOR	//070228
			EvWin_ActorRelease( wk );		//削除
#endif
			EvBmpList_Del(wk);
		}
		break;
	default:
		Snd_SePlay( SEQ_SE_DP_SELECT );
		*swk->work = ret;					//選択した値をワークに代入
#ifdef EV_WIN_SCROLL_CURSOR	//070228
		EvWin_ActorRelease( wk );			//削除
#endif
		EvBmpList_Del(wk);
		break;
	};

	return;
};

//--------------------------------------------------------------
/**
 * @brief	BMPリスト　リスト終了
 *
 * @param	wk			WAZA_OSHIE_WORK型のポインタ
 *
 * @retval	none
 *
 * SEQ_SE_DP_SELECTを鳴らしている！
 */
//--------------------------------------------------------------
static void EvBmpList_Del( WAZA_OSHIE_WORK* wk )
{
	int i;

	Snd_SePlay(SEQ_SE_DP_SELECT);	//注意！

	BmpListExit( wk->lw, NULL, NULL );
	BmpMenuWinClear( wk->ListH.win, WINDOW_TRANS_ON );
	GF_BGL_BmpWinDel( &wk->bmpwin );

	for( i=0; i < EV_WIN_MSG_MAX ;i++ ){
		STRBUF_Delete( wk->msg_buf[i] );
	}

	if( wk->msgman_del_flag == 1 ){
		//WORDSET_Delete( wk->wordset );
		MSGMAN_Delete( wk->msgman );
	}

	TCB_Delete( wk->tcb );
	sys_FreeMemoryEz( wk );
	return;
}


//==============================================================================================
//
//	スクロールカーソル
//
//==============================================================================================
#ifdef EV_WIN_SCROLL_CURSOR	//070228

//アクター初期化
static void EvWin_ActorInit( WAZA_OSHIE_WORK* wk )
{
	int i;
	s16 x = (wk->x * 8) + (BmpList_length_get(wk) / 2);

	//リソース登録最大数定義構造体
	TCATS_RESOURCE_NUM_LIST	crnl = { 1, 1, 1, 1 };

	//登録用構造体 単体登録用(座標は後で設定し直す)
	static const TCATS_OBJECT_ADD_PARAM_S ActAddParam_S[] =
	{
		//上向き
		{
			0, 0, 0,
			CURSOR_ANMNO, CURSOR_SPRI, CURSOR_PALT, NNS_G2D_VRAM_TYPE_2DMAIN,
			{
				EVWIN_CHR_H_ID_CURSOR, EVWIN_PAL_H_ID,
				EVWIN_CEL_H_ID_CURSOR, EVWIN_ANM_H_ID_CURSOR, 0, 0,
			},
			0, 0
		},

		//下向き
		{
			0, 0, 0,
			CURSOR_ANMNO, CURSOR_SPRI, CURSOR_PALT, NNS_G2D_VRAM_TYPE_2DMAIN,
			{
				EVWIN_CHR_H_ID_CURSOR, EVWIN_PAL_H_ID,
				EVWIN_CEL_H_ID_CURSOR, EVWIN_ANM_H_ID_CURSOR, 0, 0,
			},
			0, 0
		},
	};
	
	//フィールドセルアクター初期化
	FieldCellActSet_S( &wk->fcat, &crnl, EVWIN_ACTMAX, HEAPID_FIELD );
	
	{
		ARCHANDLE* hdl;
		
		hdl = ArchiveDataHandleOpen( ARC_EVWIN_GRA, HEAPID_FIELD );

		//リソース設定
		FldClact_LoadResPlttArcH(	&wk->fcat, hdl, NARC_evwin_nclr,
									0, ACT_RES_PAL_NUM, NNS_G2D_VRAM_TYPE_2DMAIN, 
									EVWIN_PAL_H_ID );

		FldClact_LoadResCellArcH(	&wk->fcat, hdl, NARC_evwin_ncer,
									0, EVWIN_CEL_H_ID_CURSOR );

		FldClact_LoadResCellAnmArcH(&wk->fcat, hdl, NARC_evwin_nanr,
									0, EVWIN_ANM_H_ID_CURSOR );

		FldClact_LoadResourceCharArcH(	&wk->fcat, hdl, NARC_evwin_ncgr,
										0, NNS_G2D_VRAM_TYPE_2DMAIN, EVWIN_CHR_H_ID_CURSOR );
		
		ArchiveDataHandleClose( hdl );
	}

	//セルアクター追加(単発用)
	for( i=0; i < EVWIN_ACTMAX; i++ ){
		wk->act[i] = FieldCellActAdd_S( &wk->fcat, &ActAddParam_S[i] );
		CLACT_SetAnmFlag( wk->act[i]->act, 1 );				//オートアニメ
		CATS_ObjectEnableCap( wk->act[i], FALSE );			//OBJの非表示設定
	}

	//座標の設定
	//OS_Printf( "x = %d\n", x );
	//OS_Printf( "wk->y = %d\n", (wk->y*8) );
	//OS_Printf( "y = %d\n", (wk->list_no*8) );
	CATS_ObjectPosSetCap( wk->act[0], x, (wk->y*8) );
	CATS_ObjectPosSetCap( wk->act[1], x, (wk->list_no*8) );

	//オートアニメで対応じゃなくて、座標を動かしてアニメでもいいのかも。。保留
	
	//選択カーソルの色変更
	//CATS_ObjectPaletteSetCap(wk->act[ACT_CURSOR],CURSOR_PALF);
	
	return;
}

//アクター削除
static void EvWin_ActorRelease( WAZA_OSHIE_WORK* wk )
{
	int i;

	//アクターポインタの削除
	for( i=0; i < EVWIN_ACTMAX; i++ ){
		if( wk->act[i] != NULL ){
			CATS_ActorPointerDelete_S( wk->act[i] );
		}
	}

	//セルアクター削除（単発用）
	FieldCellActDelete_S( &wk->fcat );
	return;
}
#endif

//--------------------------------------------------------------------------------------------
/**
 * 技教えボード表示
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
BOOL EvCmdWazaOshieBoardWrite( VM_MACHINE * core )
{
	FIELDSYS_WORK* fsys = core->fsys;
	WORDSET** wordset	= GetEvScriptWorkMemberAdrs( fsys, ID_EVSCR_WORDSET );
	EV_WIN_WORK** ev_win= GetEvScriptWorkMemberAdrs( fsys, ID_EVSCR_EVWIN );	//EV_WIN_WORK取得
	u8 x				= VMGetU8(core);
	u8 y				= VMGetU8(core);
	u16 no				= VMGetWorkValue(core);
	u16* wk				= VMGetWork(core);

	//OS_Printf( "no = %d\n", no );
	no = GetWazaOshieWazaNoByWazaNo( no );
	//OS_Printf( "no = %d\n", no );
	*ev_win = WazaOshieBoardWrite(	fsys, x, y, wk, *wordset, 
									waza_oshie_tbl[no].aka,
									waza_oshie_tbl[no].ao,
									waza_oshie_tbl[no].ki,
									waza_oshie_tbl[no].midori );
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * 技教えボード削除
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
BOOL EvCmdWazaOshieBoardDel( VM_MACHINE * core )
{
	FIELDSYS_WORK* fsys = core->fsys;
	EV_WIN_WORK** ev_win= GetEvScriptWorkMemberAdrs( fsys, ID_EVSCR_EVWIN );	//EV_WIN_WORK取得
	WazaOshieBoardDel( *ev_win );
	return 0;
}



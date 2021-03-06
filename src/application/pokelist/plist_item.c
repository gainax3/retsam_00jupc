//============================================================================================
/**
 * @file	plist_item.c
 * @brief	|PXgæÊACegp
 * @author	Hiroyuki Nakamura
 * @date	05.12.12
 */
//============================================================================================
#include "common.h"

#include "system/procsys.h"
#include "system/clact_tool.h"
#include "system/bmp_menu.h"
#include "system/fontproc.h"
#include "system/msgdata.h"
#include "system/wordset.h"
#include "system/numfont.h"
#include "system/window.h"
#include "system/snd_tool.h"
#include "battle/battle_common.h"
#include "poketool/pokeparty.h"
#include "poketool/status_rcv.h"
#include "poketool/waza_tool.h"
#include "poketool/poke_regulation.h"
#include "itemtool/item.h"
#include "itemtool/myitem.h"
#include "msgdata/msg.naix"
#include "msgdata/msg_pokelist.h"
#include "application/app_tool.h"
#include "application/p_status.h"
#include "../../field/zonedata.h"
#include "../../field/fieldsys.h"

#define	PLIST_ITEM_H_GLOBAL
#include "application/pokelist.h"
#include "plist_sys.h"
#include "plist_bmp.h"
#include "plist_obj.h"
#include "plist_item.h"
#include "plist_snd_def.h"


//============================================================================================
//	èè`
//============================================================================================
#define	RCV_FLG_SLEEP		( 0x01 )	// °èñ
#define	RCV_FLG_POISON		( 0x02 )	// Åñ
#define	RCV_FLG_BURN		( 0x04 )	// Îñ
#define	RCV_FLG_ICE			( 0x08 )	// Xñ
#define	RCV_FLG_PARALYZE	( 0x10 )	// áñ
#define	RCV_FLG_PANIC		( 0x20 )	// ¬ñ
#define	RCV_FLG_ALL			( 0x3f )	// Sõ

enum {
	ITEM_TYPE_BTL_ST_UP = 0,	// í¬pXe[^XAbvn
	ITEM_TYPE_ALLDETH_RCV,		// Sõmñ
	ITEM_TYPE_LV_UP,			// LvUpn
	ITEM_TYPE_NEMURI_RCV,		// °èñ
	ITEM_TYPE_DOKU_RCV,			// Åñ
	ITEM_TYPE_YAKEDO_RCV,		// Îñ
	ITEM_TYPE_KOORI_RCV,		// Xñ
	ITEM_TYPE_MAHI_RCV,			// áñ
	ITEM_TYPE_KONRAN_RCV,		// ¬ñ
	ITEM_TYPE_ALL_ST_RCV,		// Sõ
	ITEM_TYPE_MEROMERO_RCV,		// ñ
	ITEM_TYPE_HP_RCV,			// HPñ ( mà )
	ITEM_TYPE_HP_UP,			// HPwÍlUP
	ITEM_TYPE_ATC_UP,			// UwÍlUP
	ITEM_TYPE_DEF_UP,			// häwÍlUP
	ITEM_TYPE_AGL_UP,			// f³wÍlUP
	ITEM_TYPE_SPA_UP,			// ÁUwÍlUP
	ITEM_TYPE_SPD_UP,			// ÁhwÍlUP

	ITEM_TYPE_HP_DOWN,			// HPwÍlDOWN
	ITEM_TYPE_ATC_DOWN,			// UwÍlDOWN
	ITEM_TYPE_DEF_DOWN,			// häwÍlDOWN
	ITEM_TYPE_AGL_DOWN,			// f³wÍlDOWN
	ITEM_TYPE_SPA_DOWN,			// ÁUwÍlDOWN
	ITEM_TYPE_SPD_DOWN,			// ÁhwÍlDOWN

	ITEM_TYPE_EVO,				// i»n
	ITEM_TYPE_PP_UP,			// ppUpn
	ITEM_TYPE_PP_3UP,			// pp3Upn
	ITEM_TYPE_PP_RCV,			// ppñn
	ITEM_TYPE_ETC,				// »Ì¼
};


//============================================================================================
//	vg^Cvé¾
//============================================================================================
static int NormalRecover( void * work );
static int PrmExpDown_Recover( void * work );
static int HP_RecoverFuncInit( void * work );
static int HP_RecoverFuncMain( void * work );
static int LvUp_Recover( void * work );
static int LvUp_RecoverMain( void * work );

static void PL_WazaSet( PLIST_WORK * wk, POKEMON_PARAM * pp, u32 pos );
static int PL_NewWazaSetYes( void * work );
static int PL_NewWazaSetNo( void * work );
static int PL_NewWazaSetCancelYes( void * work );
static int PL_NewWazaSetCancelNo( void * work );

static u16 PL_PlaceIDGet( PLIST_WORK * wk );


//--------------------------------------------------------------------------------------------
/**
 * ñ^Cv`FbN
 *
 * @param	item	ACeÔ
 *
 * @return	ñ^Cv
 */
//--------------------------------------------------------------------------------------------
static u8 ItemRecoverCheck( u16 item )
{
	ITEMDATA * dat;
	s32	prm;

	dat = GetItemArcData( item, ITEM_GET_DATA, HEAPID_POKELIST );

	// p[^ðÁÄ¢È¢ACeÍ»Ì¼
	if( ItemBufParamGet( dat, ITEM_PRM_W_TYPE ) != ITEM_WKTYPE_POKEUSE ){
		sys_FreeMemoryEz( dat );
		return ITEM_TYPE_ETC;
	}

	// í¬pXe[^XAbvn
	if( ItemBufParamGet( dat, ITEM_PRM_ATTACK_UP ) != 0 ||
		ItemBufParamGet( dat, ITEM_PRM_DEFENCE_UP ) != 0 ||
		ItemBufParamGet( dat, ITEM_PRM_SP_ATTACK_UP ) != 0 ||
		ItemBufParamGet( dat, ITEM_PRM_SP_DEFENCE_UP ) != 0 ||
		ItemBufParamGet( dat, ITEM_PRM_AGILITY_UP ) != 0 ||
		ItemBufParamGet( dat, ITEM_PRM_HIT_UP ) != 0 ||
		ItemBufParamGet( dat, ITEM_PRM_CRITICAL_UP ) != 0 ){

		sys_FreeMemoryEz( dat );
		return ITEM_TYPE_BTL_ST_UP;
	}

	// Sõmñ
	if( ItemBufParamGet( dat, ITEM_PRM_ALL_DEATH_RCV ) != 0 ){
		sys_FreeMemoryEz( dat );
		return ITEM_TYPE_ALLDETH_RCV;
	}

	// LvUpn
	if( ItemBufParamGet( dat, ITEM_PRM_LV_UP ) != 0 ){
		sys_FreeMemoryEz( dat );
		return ITEM_TYPE_LV_UP;
	}

	// Statusñn
	prm = ItemBufParamGet( dat, ITEM_PRM_SLEEP_RCV );				// °è
	prm += ( ItemBufParamGet( dat, ITEM_PRM_POISON_RCV ) << 1 );	// Å
	prm += ( ItemBufParamGet( dat, ITEM_PRM_BURN_RCV ) << 2 );		// Î
	prm += ( ItemBufParamGet( dat, ITEM_PRM_ICE_RCV ) << 3 );		// X
	prm += ( ItemBufParamGet( dat, ITEM_PRM_PARALYZE_RCV ) << 4 );	// á
	prm += ( ItemBufParamGet( dat, ITEM_PRM_PANIC_RCV ) << 5 );		// ¬
	switch( prm ){
	case RCV_FLG_SLEEP:		// °è
		sys_FreeMemoryEz( dat );
		return ITEM_TYPE_NEMURI_RCV;
	case RCV_FLG_POISON:	// Å
		sys_FreeMemoryEz( dat );
		return ITEM_TYPE_DOKU_RCV;
	case RCV_FLG_BURN:		// Î
		sys_FreeMemoryEz( dat );
		return ITEM_TYPE_YAKEDO_RCV;
	case RCV_FLG_ICE:		// X
		sys_FreeMemoryEz( dat );
		return ITEM_TYPE_KOORI_RCV;
	case RCV_FLG_PARALYZE:	// á
		sys_FreeMemoryEz( dat );
		return ITEM_TYPE_MAHI_RCV;
	case RCV_FLG_PANIC:		// ¬
		sys_FreeMemoryEz( dat );
		return ITEM_TYPE_KONRAN_RCV;
	case RCV_FLG_ALL:		// Sõ
		if( ItemBufParamGet( dat, ITEM_PRM_HP_RCV ) != 0 ){
			sys_FreeMemoryEz( dat );
			return ITEM_TYPE_HP_RCV;	// HPñ ( mà )
		}else{
			sys_FreeMemoryEz( dat );
			return ITEM_TYPE_ALL_ST_RCV;
		}
	}
	// ñ
	if( ItemBufParamGet( dat, ITEM_PRM_MEROMERO_RCV ) != 0 ){
		sys_FreeMemoryEz( dat );
		return ITEM_TYPE_MEROMERO_RCV;
	}

	// HPñ ( mà )
	if( ItemBufParamGet( dat, ITEM_PRM_HP_RCV ) != 0 ){
		sys_FreeMemoryEz( dat );
		return ITEM_TYPE_HP_RCV;
	}

	// h[sOn
	// HPwÍl
	prm = ItemBufParamGet( dat, ITEM_PRM_HP_EXP_POINT );
	if( prm > 0 ){
		sys_FreeMemoryEz( dat );
		return ITEM_TYPE_HP_UP;
	}
	if( prm < 0 ){
		sys_FreeMemoryEz( dat );
		return ITEM_TYPE_HP_DOWN;
	}
	// UwÍl
	prm = ItemBufParamGet( dat, ITEM_PRM_POWER_EXP_POINT );
	if( prm > 0 ){
		sys_FreeMemoryEz( dat );
		return ITEM_TYPE_ATC_UP;
	}
	if( prm < 0 ){
		sys_FreeMemoryEz( dat );
		return ITEM_TYPE_ATC_DOWN;
	}
	// häwÍl
	prm = ItemBufParamGet( dat, ITEM_PRM_DEFENCE_EXP_POINT );
	if( prm > 0 ){
		sys_FreeMemoryEz( dat );
		return ITEM_TYPE_DEF_UP;
	}
	if( prm < 0 ){
		sys_FreeMemoryEz( dat );
		return ITEM_TYPE_DEF_DOWN;
	}
	// f³wÍl
	prm = ItemBufParamGet( dat, ITEM_PRM_AGILITY_EXP_POINT );
	if( prm > 0 ){
		sys_FreeMemoryEz( dat );
		return ITEM_TYPE_AGL_UP;
	}
	if( prm < 0 ){
		sys_FreeMemoryEz( dat );
		return ITEM_TYPE_AGL_DOWN;
	}
	// ÁUwÍl
	prm = ItemBufParamGet( dat, ITEM_PRM_SP_ATTACK_EXP_POINT );
	if( prm > 0 ){
		sys_FreeMemoryEz( dat );
		return ITEM_TYPE_SPA_UP;
	}
	if( prm < 0 ){
		sys_FreeMemoryEz( dat );
		return ITEM_TYPE_SPA_DOWN;
	}
	// ÁhwÍl
	prm = ItemBufParamGet( dat, ITEM_PRM_SP_DEFENCE_EXP_POINT );
	if( prm > 0 ){
		sys_FreeMemoryEz( dat );
		return ITEM_TYPE_SPD_UP;
	}
	if( prm < 0 ){
		sys_FreeMemoryEz( dat );
		return ITEM_TYPE_SPD_DOWN;
	}

	// i»n
	if( ItemBufParamGet( dat, ITEM_PRM_EVOLUTION ) != 0 ){
		sys_FreeMemoryEz( dat );
		return ITEM_TYPE_EVO;
	}

	// ppUpn
	if( ItemBufParamGet( dat, ITEM_PRM_PP_UP ) != 0 ){
		sys_FreeMemoryEz( dat );
		return ITEM_TYPE_PP_UP;
	}

	// pp3Upn
	if( ItemBufParamGet( dat, ITEM_PRM_PP_3UP ) != 0 ){
		sys_FreeMemoryEz( dat );
		return ITEM_TYPE_PP_3UP;
	}

	// ppñn
	if( ItemBufParamGet( dat, ITEM_PRM_PP_RCV ) != 0 ||
		ItemBufParamGet( dat, ITEM_PRM_ALL_PP_RCV ) != 0 ){

		sys_FreeMemoryEz( dat );
		return ITEM_TYPE_PP_RCV;
	}

	sys_FreeMemoryEz( dat );
	return ITEM_TYPE_ETC;
}

//--------------------------------------------------------------------------------------------
/**
 * ñbZ[WZbg
 *
 * @param	wk		|PXgÌ[N
 * @param	itemno	ACeÔ
 * @param	tmp		Äpp[^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void SetStatusRecoverMsg( PLIST_WORK * wk, u16 itemno, u32 tmp )
{
	POKEMON_PARAM * pp;
	STRBUF * str;

	pp  = PokeParty_GetMemberPointer( wk->dat->pp, wk->pos );
	WORDSET_RegisterPokeNickName( wk->wset, 0, PPPPointerGet(pp) );

	switch( ItemRecoverCheck( itemno ) ){
	case ITEM_TYPE_DOKU_RCV:	// Åñ
		str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_15 );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		STRBUF_Delete( str );
		break;
	case ITEM_TYPE_NEMURI_RCV:	// °èñ
		str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_35 );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		STRBUF_Delete( str );
		break;
	case ITEM_TYPE_YAKEDO_RCV:	// Îñ
		str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_17 );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		STRBUF_Delete( str );
		break;
	case ITEM_TYPE_KOORI_RCV:	// Xñ
		str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_18 );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		STRBUF_Delete( str );
		break;
	case ITEM_TYPE_MAHI_RCV:	// áñ
		str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_16 );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		STRBUF_Delete( str );
		break;
	case ITEM_TYPE_KONRAN_RCV:	// ¬ñ
		str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_23 );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		STRBUF_Delete( str );
		break;
	case ITEM_TYPE_MEROMERO_RCV:	// ñ
		str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_24 );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		STRBUF_Delete( str );
		break;
	case ITEM_TYPE_ALL_ST_RCV:	// Xe[^XÙíSõ
		str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_21 );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		STRBUF_Delete( str );
		break;
	case ITEM_TYPE_HP_UP:		// h[sOACe HPwÍlUP
		str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_25 );
		WORDSET_RegisterPokeStatusName( wk->wset, 1, 0 );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		STRBUF_Delete( str );
		break;
	case ITEM_TYPE_ATC_UP:		// h[sOACe UwÍlUP
		str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_25 );
		WORDSET_RegisterPokeStatusName( wk->wset, 1, 1 );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		STRBUF_Delete( str );
		break;
	case ITEM_TYPE_DEF_UP:		// h[sOACe häwÍlUP
		str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_25 );
		WORDSET_RegisterPokeStatusName( wk->wset, 1, 2 );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		STRBUF_Delete( str );
		break;
	case ITEM_TYPE_AGL_UP:		// h[sOACe f³wÍlUP
		str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_25 );
		WORDSET_RegisterPokeStatusName( wk->wset, 1, 3 );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		STRBUF_Delete( str );
		break;
	case ITEM_TYPE_SPA_UP:		// h[sOACe ÁUwÍlUP
		str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_25 );
		WORDSET_RegisterPokeStatusName( wk->wset, 1, 4 );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		STRBUF_Delete( str );
		break;
	case ITEM_TYPE_SPD_UP:		// h[sOACe ÁhwÍlUP
		str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_25 );
		WORDSET_RegisterPokeStatusName( wk->wset, 1, 5 );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		STRBUF_Delete( str );
		break;
	case ITEM_TYPE_HP_DOWN:		// h[sOACe HPwÍl
		str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_51+tmp );
		WORDSET_RegisterPokeStatusName( wk->wset, 1, 0 );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		STRBUF_Delete( str );
		break;
	case ITEM_TYPE_ATC_DOWN:	// h[sOACe UwÍlDOWN
		str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_51+tmp );
		WORDSET_RegisterPokeStatusName( wk->wset, 1, 1 );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		STRBUF_Delete( str );
		break;
	case ITEM_TYPE_DEF_DOWN:	// h[sOACe häwÍlDOWN
		str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_51+tmp );
		WORDSET_RegisterPokeStatusName( wk->wset, 1, 2 );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		STRBUF_Delete( str );
		break;
	case ITEM_TYPE_AGL_DOWN:	// h[sOACe f³wÍlDOWN
		str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_51+tmp );
		WORDSET_RegisterPokeStatusName( wk->wset, 1, 3 );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		STRBUF_Delete( str );
		break;
	case ITEM_TYPE_SPA_DOWN:	// h[sOACe ÁUwÍlDOWN
		str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_51+tmp );
		WORDSET_RegisterPokeStatusName( wk->wset, 1, 4 );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		STRBUF_Delete( str );
		break;
	case ITEM_TYPE_SPD_DOWN:	// h[sOACe ÁhwÍlDOWN
		str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_51+tmp );
		WORDSET_RegisterPokeStatusName( wk->wset, 1, 5 );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		STRBUF_Delete( str );
		break;
	case ITEM_TYPE_PP_UP:		// PPUp
	case ITEM_TYPE_PP_3UP:		// PP3Up
		str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_22 );
		WORDSET_RegisterWazaName( wk->wset, 0, tmp );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		STRBUF_Delete( str );
		break;
	case ITEM_TYPE_PP_RCV:		// PPñ
		MSGMAN_GetString( wk->msg_man, mes_pokelist_04_19, wk->msg_buf );
		break;
	default:
		MSGMAN_GetString( wk->msg_man, mes_pokelist_04_45, wk->msg_buf );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * ACegpC
 *
 * @param	wk		|PXgÌ[N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PokeList_ItemUseMain( PLIST_WORK * wk )
{
	switch( ItemRecoverCheck( wk->dat->item ) ){
	case ITEM_TYPE_BTL_ST_UP:		// í¬pXe[^XAbvn
	case ITEM_TYPE_ETC:				// »Ì¼
		// bZ[W\¦µÄI¹
		break;

	case ITEM_TYPE_ALLDETH_RCV:		// Sõmñ
		// SÌñ
		break;

	case ITEM_TYPE_LV_UP:			// LvUpn
		// bZ[W\¦ -> p[^\¦
		wk->strcv_func = LvUp_Recover;
		break;

	case ITEM_TYPE_NEMURI_RCV:		// °èñ
	case ITEM_TYPE_DOKU_RCV:		// Åñ
	case ITEM_TYPE_YAKEDO_RCV:		// Îñ
	case ITEM_TYPE_KOORI_RCV:		// Xñ
	case ITEM_TYPE_MAHI_RCV:		// áñ
	case ITEM_TYPE_KONRAN_RCV:		// ¬ñ
	case ITEM_TYPE_ALL_ST_RCV:		// Sõ
	case ITEM_TYPE_MEROMERO_RCV:	// ñ
	case ITEM_TYPE_HP_UP:			// HPwÍlUP
	case ITEM_TYPE_ATC_UP:			// UwÍlUP
	case ITEM_TYPE_DEF_UP:			// häwÍlUP
	case ITEM_TYPE_AGL_UP:			// f³wÍlUP
	case ITEM_TYPE_SPA_UP:			// ÁUwÍlUP
	case ITEM_TYPE_SPD_UP:			// ÁhwÍlUP
	case ITEM_TYPE_PP_UP:			// ppUpn
	case ITEM_TYPE_PP_3UP:			// pp3Upn
	case ITEM_TYPE_PP_RCV:			// ppñn
		wk->strcv_func = NormalRecover;
		break;

	case ITEM_TYPE_HP_DOWN:			// HPwÍlDOWN
	case ITEM_TYPE_ATC_DOWN:		// UwÍlDOWN
	case ITEM_TYPE_DEF_DOWN:		// häwÍlDOWN
	case ITEM_TYPE_AGL_DOWN:		// f³wÍlDOWN
	case ITEM_TYPE_SPA_DOWN:		// ÁUwÍlDOWN
	case ITEM_TYPE_SPD_DOWN:		// ÁhwÍlDOWN
		wk->strcv_func = PrmExpDown_Recover;
		break;

	case ITEM_TYPE_HP_RCV:			// HPñ ( mà )
		wk->strcv_func = HP_RecoverFuncInit;
		break;
/*
	case ITEM_TYPE_EVO:				// i»n
		wk->strcv_func = ShinkaRecover;
		break;
*/
	}
}

//--------------------------------------------------------------------------------------------
/**
 * ACegpbZ[WEFCg
 *
 * @param	work		|PXgÌ[N
 *
 * @return	ÌV[PX
 */
//--------------------------------------------------------------------------------------------
int PokeList_ItemUseMsgWait( void * work )
{
	PLIST_WORK * wk = (PLIST_WORK *)work;

	if( GF_MSG_PrintEndCheck( wk->msg_index ) != 0 ){
		return SEQ_ITEMUSE_WAIT;
	}

	if( sys.trg & ( PAD_BUTTON_A|PAD_BUTTON_B ) ){
/*
		MyItem_SubItem( wk->dat->myitem, wk->dat->item, 1, HEAPID_POKELIST );
		if( ItemKanpouyakuCheck( wk->dat->item ) == TRUE && wk->dat->tvwk != NULL ){
			POKEMON_PARAM * pp = PokeParty_GetMemberPointer( wk->dat->pp, wk->pos );
			TVTOPIC_Entry_Watch_UseKanpo( wk->dat->tvwk, pp, wk->dat->item );
		}
*/
		wk->dat->ret_mode = PL_RET_NORMAL;
		return SEQ_OUT_SET;
	}

	return SEQ_ITEMUSE_WAIT;
}


//--------------------------------------------------------------------------------------------
/**
 * Êíñ
 *
 * @param	work		|PXgÌ[N
 *
 * @return	ÌV[PX
 */
//--------------------------------------------------------------------------------------------
static int NormalRecover( void * work )
{
	PLIST_WORK * wk = (PLIST_WORK *)work;

	PokeParty_StatusRecover(
		wk->dat->pp, wk->dat->item, wk->pos, 0, PL_PlaceIDGet(wk), HEAPID_POKELIST );

	PokeList_PanelPPDataMake( wk, wk->pos );
	PokeListParamPut( wk, wk->pos );
	PokeListParamBmpCgxOn( wk, wk->pos );
	PokeList_StatusIconChg( wk, wk->pos, wk->panel[wk->pos].st );	// óÔÙíACR

	SetStatusRecoverMsg( wk, wk->dat->item, 0 );

	PokeListLargeMsgPut( wk, PL_MSG_COMP_EXPAND, 1 );

	Snd_SePlay( SEQ_SE_DP_KAIFUKU );

	wk->strcv_func = PokeList_ItemUseMsgWait;

	return SEQ_ITEMUSE_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * wÍl_E
 *
 * @param	work		|PXgÌ[N
 *
 * @return	ÌV[PX
 */
//--------------------------------------------------------------------------------------------
static int PrmExpDown_Recover( void * work )
{
	PLIST_WORK * wk;
	POKEMON_PARAM * pp;
	u8	prm[7];

	wk = (PLIST_WORK *)work;
	pp = PokeParty_GetMemberPointer( wk->dat->pp, wk->pos );

	prm[0] = PokeParaGet( pp, ID_PARA_hp_exp, NULL );
	prm[1] = PokeParaGet( pp, ID_PARA_pow_exp, NULL );
	prm[2] = PokeParaGet( pp, ID_PARA_def_exp, NULL );
	prm[3] = PokeParaGet( pp, ID_PARA_agi_exp, NULL );
	prm[4] = PokeParaGet( pp, ID_PARA_spepow_exp, NULL );
	prm[5] = PokeParaGet( pp, ID_PARA_spedef_exp, NULL );
	prm[6] = PokeParaGet( pp, ID_PARA_friend, NULL );

	PokeParty_StatusRecover(
		wk->dat->pp, wk->dat->item, wk->pos, 0, PL_PlaceIDGet(wk), HEAPID_POKELIST );

	PokeList_PanelPPDataMake( wk, wk->pos );
	PokeListParamPut( wk, wk->pos );
	PokeListParamBmpCgxOn( wk, wk->pos );
	PokeList_StatusIconChg( wk, wk->pos, wk->panel[wk->pos].st );	// óÔÙíACR

	if( prm[0] != PokeParaGet( pp, ID_PARA_hp_exp, NULL ) ||
		prm[1] != PokeParaGet( pp, ID_PARA_pow_exp, NULL ) ||
		prm[2] != PokeParaGet( pp, ID_PARA_def_exp, NULL ) ||
		prm[3] != PokeParaGet( pp, ID_PARA_agi_exp, NULL ) ||
		prm[4] != PokeParaGet( pp, ID_PARA_spepow_exp, NULL ) ||
		prm[5] != PokeParaGet( pp, ID_PARA_spedef_exp, NULL ) ){
		if( prm[6] != PokeParaGet( pp, ID_PARA_friend, NULL ) ){
			SetStatusRecoverMsg( wk, wk->dat->item, 0 );
		}else{
			SetStatusRecoverMsg( wk, wk->dat->item, 1 );
		}
	}else{
		SetStatusRecoverMsg( wk, wk->dat->item, 2 );
	}

	PokeListLargeMsgPut( wk, PL_MSG_COMP_EXPAND, 1 );

	wk->strcv_func = PokeList_ItemUseMsgWait;

	return SEQ_ITEMUSE_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * HPñ
 *
 * @param	work		|PXgÌ[N
 *
 * @return	ÌV[PX
 */
//--------------------------------------------------------------------------------------------
static int HP_RecoverFuncInit( void * work )
{
	PLIST_WORK * wk;
	POKEMON_PARAM * pp;
	STRBUF * str;
	u32	now_hp;
	u32	st;

	wk = (PLIST_WORK *)work;

	PokeParty_StatusRecover(
		wk->dat->pp, wk->dat->item, wk->pos, 0, PL_PlaceIDGet(wk), HEAPID_POKELIST );

	pp     = PokeParty_GetMemberPointer( wk->dat->pp, wk->pos );
	now_hp = PokeParaGet( pp, ID_PARA_hp, NULL );
	if( wk->panel[wk->pos].hp == 0 ){
		str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_20 );
	}else if( wk->panel[wk->pos].hp == now_hp ){
		str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_21 );
	}else{
		str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_14 );
	}
	WORDSET_RegisterPokeNickName( wk->wset, 0, PPPPointerGet(pp) );
	WORDSET_RegisterNumber(
		wk->wset,1,now_hp-wk->panel[wk->pos].hp,3,NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);
	WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
	STRBUF_Delete( str );

	st = BadStatusIconAnmGet( pp );
	PokeList_StatusIconChg( wk, wk->pos, st );	// óÔÙíACR
	if( st == ST_ICON_NONE ){
		wk->panel[wk->pos].st = PL_ST_NONE;
		PokeListLvPut( wk, wk->pos );
	}
	PokeListPanelPaletteSet( wk, wk->pos );

	wk->strcv_func = HP_RecoverFuncMain;
//	if( wk->panel[wk->pos].hp != now_hp ){
		Snd_SePlay( SEQ_SE_DP_KAIFUKU );
//	}

	return SEQ_ITEMUSE_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * HPñC
 *
 * @param	work		|PXgÌ[N
 *
 * @return	ÌV[PX
 */
//--------------------------------------------------------------------------------------------
static int HP_RecoverFuncMain( void * work )
{
	PLIST_WORK * wk = (PLIST_WORK *)work;
	POKEMON_PARAM * pp;
	u32	now_hp;

	pp     = PokeParty_GetMemberPointer( wk->dat->pp, wk->pos );
	now_hp = PokeParaGet( pp, ID_PARA_hp, NULL );

	if( wk->panel[wk->pos].hp != now_hp ){
		wk->panel[wk->pos].hp++;
	}

	PokeListHPClear( wk, wk->pos );
	GF_BGL_BmpWinDataFill( &wk->win[WIN_POKE1_HPBAR+wk->pos*5], 0 );

	PokeListHPPut( wk, wk->pos );
	PokeListHPBerPut( wk, wk->pos );

	if( wk->panel[wk->pos].hp == now_hp ){
		PokeListLargeMsgPut( wk, PL_MSG_COMP_EXPAND, 1 );
		wk->strcv_func = PokeList_ItemUseMsgWait;
	}

	return SEQ_ITEMUSE_WAIT;
}


//--------------------------------------------------------------------------------------------
/**
 * SÌmñACe`FbN
 *
 * @param	item	ACeÔ
 *
 * @retval	"TRUE = SÌmñACe"
 * @retval	"FALSE = »êÈO"
 */
//--------------------------------------------------------------------------------------------
BOOL PL_ALLDeathRcvItemCheck( u16 item )
{
	if( ItemParamGet( item, ITEM_PRM_ALL_DEATH_RCV, HEAPID_POKELIST ) != 0 ){
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * m|Pª¢é©
 *
 * @param	wk		|PXgÌ[N
 * @param	start	õJnÊu
 *
 * @retval	"0xff = ¢È¢"
 * @retval	"0xff != õÊu"
 */
//--------------------------------------------------------------------------------------------
static u8 PL_AllDeathRcvCheck( PLIST_WORK * wk, u8 start )
{
	u8	i;

	if( start >= 6 ){ return 0xff; }

	for( i=start; i<6; i++ ){
		if( wk->panel[i].flg != 0 && wk->panel[i].hp == 0 ){
			return i;
		}
	}
	return 0xff;
}

//--------------------------------------------------------------------------------------------
/**
 * SÌmñC
 *
 * @param	work		|PXgÌ[N
 *
 * @return	ÌV[PX
 */
//--------------------------------------------------------------------------------------------
int PokeList_AllDeathRcvMain( PLIST_WORK * wk )
{
	POKEMON_PARAM * pp;
	STRBUF * str;
	u32	now_hp;
	u8	pos;

	switch( wk->next_seq ){
	case 0:			// gp`FbN
		wk->pos = PL_AllDeathRcvCheck( wk, 0 );
		if( wk->pos == 0xff ){
			MSGMAN_GetString( wk->msg_man, mes_pokelist_04_45, wk->msg_buf );
			PokeListLargeMsgPut( wk, PL_MSG_COMP_EXPAND, 1 );
			wk->strcv_func = PokeList_ItemUseMsgWait;
			PokeList_SelCursorChgDirect( wk, 0, 1 );
			wk->pos = PL_SEL_POS_EXIT;
			return SEQ_ITEMUSE_WAIT;
		}
		if( wk->pos != 0 ){
			PokeListPanelPaletteSet( wk, 0 );
		}
	case 1:			// ú»
/*
		pos = wk->pos;
		wk->pos = PL_AllDeathRcvCheck( wk, pos );
		PokeListPanelPaletteSet( wk, pos );
*/
		pp = PokeParty_GetMemberPointer( wk->dat->pp, wk->pos );
		StatusRecover( pp, wk->dat->item, 0, PL_PlaceIDGet(wk), HEAPID_POKELIST );

		now_hp = PokeParaGet( pp, ID_PARA_hp, NULL );
		str    = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_20 );
		WORDSET_RegisterPokeNickName( wk->wset, 0, PPPPointerGet(pp) );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		STRBUF_Delete( str );

		wk->panel[wk->pos].st = PL_ST_NONE;
		PokeList_StatusIconChg( wk, wk->pos, wk->panel[wk->pos].st );
		PokeListLvPut( wk, wk->pos );
		PokeListPanelPaletteSet( wk, wk->pos );
		PokeList_SelCursorChgDirect( wk, wk->pos, 1 );
		Snd_SePlay( SEQ_SE_DP_KAIFUKU );
		wk->next_seq++;
		break;

	case 2:			// HPQ[Wñ
		pp     = PokeParty_GetMemberPointer( wk->dat->pp, wk->pos );
		now_hp = PokeParaGet( pp, ID_PARA_hp, NULL );

		wk->panel[wk->pos].hp++;
		PokeListHPClear( wk, wk->pos );
		GF_BGL_BmpWinDataFill( &wk->win[WIN_POKE1_HPBAR+wk->pos*5], 0 );
		PokeListHPPut( wk, wk->pos );
		PokeListHPBerPut( wk, wk->pos );

		if( wk->panel[wk->pos].hp == now_hp ){
			PokeListLargeMsgPut( wk, PL_MSG_COMP_EXPAND, 1 );
			wk->next_seq++;
		}
		break;

	case 3:			// bZ[WI¹Ò¿
		if( GF_MSG_PrintEndCheck( wk->msg_index ) != 0 ){ break; }

		if( sys.trg & ( PAD_BUTTON_A|PAD_BUTTON_B ) ){
			Snd_SePlay( PLIST_SE_ENTER );
			pos = wk->pos;
			wk->pos = PL_AllDeathRcvCheck( wk, wk->pos+1 );
			if( wk->pos != 0xff ){
				BmpTalkWinClear( &wk->win[WIN_L_MSG], WINDOW_TRANS_ON );
				PokeListPanelPaletteSet( wk, pos );
				wk->next_seq = 1;
			}else{
				MyItem_SubItem( wk->dat->myitem, wk->dat->item, 1, HEAPID_POKELIST );
				wk->dat->ret_mode = PL_RET_NORMAL;
				wk->pos = PL_SEL_POS_EXIT;
				return SEQ_OUT_SET;
			}
		}
		break;
	}

	return SEQ_ITEMUSE_ALLDEATHRCV;
}


//============================================================================================
//	svcÈA
//============================================================================================
enum {
	LVUP_SEQ_PLUSPUT = 0,		// bZ[WI¹Ò¿ -> EBhE\¦
	LVUP_SEQ_PARMPUT,			// {^Ò¿ -> EBhE«·¦
	LVUP_SEQ_WINEXIT,			// {^Ò¿ -> EBhEñ\¦
	LVUP_SEQ_WAZACHECK,			// Zo¦`FbN

	LVUP_SEQ_WAZASET,			// Zo¦F{^Ò¿io¦½j
	LVUP_SEQ_WAZAADD,			// Zo¦F{^Ò¿iYêÄo¦½j

	LVUP_SEQ_SHINKACHECK,		// i»`FbN
};

static int PL_LvUpNewWazaSetYes( void * work );
static int PL_LvUpNewWazaSetNo( void * work );
static int PL_LvUpNewWazaSetCancelYes( void * work );
static int PL_LvUpNewWazaSetCancelNo( void * work );

//--------------------------------------------------------------------------------------------
/**
 * svcÈA
 *
 * @param	work		|PXgÌ[N
 *
 * @return	ÌV[PX
 */
//--------------------------------------------------------------------------------------------
static int LvUp_Recover( void * work )
{
	PLIST_WORK * wk;
	POKEMON_PARAM * pp;
	STRBUF * str;
	u32	now_hp;
	u32	st;

	wk = (PLIST_WORK *)work;

	pp = PokeParty_GetMemberPointer( wk->dat->pp, wk->pos );

	wk->tmp[0] = (u16)PokeParaGet( pp, ID_PARA_hpmax, NULL );
	wk->tmp[1] = (u16)PokeParaGet( pp, ID_PARA_pow, NULL );
	wk->tmp[2] = (u16)PokeParaGet( pp, ID_PARA_def, NULL );
	wk->tmp[3] = (u16)PokeParaGet( pp, ID_PARA_spepow, NULL );
	wk->tmp[4] = (u16)PokeParaGet( pp, ID_PARA_spedef, NULL );
	wk->tmp[5] = (u16)PokeParaGet( pp, ID_PARA_agi, NULL );

	PokeParty_StatusRecover(
		wk->dat->pp, wk->dat->item, wk->pos, 0, PL_PlaceIDGet(wk), HEAPID_POKELIST );

	wk->panel[wk->pos].lv  = PokeParaGet( pp, ID_PARA_level, NULL );
	wk->panel[wk->pos].hp  = PokeParaGet( pp, ID_PARA_hp, NULL );
	wk->panel[wk->pos].mhp = PokeParaGet( pp, ID_PARA_hpmax, NULL );

	str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_08_09 );
	WORDSET_RegisterPokeNickName( wk->wset, 0, PPPPointerGet(pp) );
	WORDSET_RegisterNumber(
		wk->wset, 1, wk->panel[wk->pos].lv, 3, NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT );
	WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
	STRBUF_Delete( str );

	st = BadStatusIconAnmGet( pp );
	PokeList_StatusIconChg( wk, wk->pos, st );	// óÔÙíACR
	if( st == ST_ICON_NONE ){
		wk->panel[wk->pos].st = PL_ST_NONE;
		PokeListLvPut( wk, wk->pos );
	}
	PokeListPanelPaletteSet( wk, wk->pos );

	wk->strcv_func = HP_RecoverFuncMain;

	PokeListParamPut( wk, wk->pos );
	PokeListParamBmpCgxOn( wk, wk->pos );

	PokeListLargeMsgPut( wk, PL_MSG_COMP_EXPAND, 1 );
	wk->strcv_func = LvUp_RecoverMain;
	wk->lv_seq = 0;

	return SEQ_ITEMUSE_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * svcÈAC
 *
 * @param	work		|PXgÌ[N
 *
 * @return	ÌV[PX
 */
//--------------------------------------------------------------------------------------------
static int LvUp_RecoverMain( void * work )
{
	PLIST_WORK * wk;
	POKEMON_PARAM * pp;
	STRBUF * str;

	wk = work;

	switch( wk->lv_seq ){
	case LVUP_SEQ_PLUSPUT:		// bZ[WI¹Ò¿ -> EBhE\¦
		if( GF_MSG_PrintEndCheck( wk->msg_index ) == 0 ){
			if( sys.trg & (PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL) ){
				Snd_SePlay( PLIST_SE_ENTER );
				PokeList_LvWinPut( wk );
				wk->lv_seq = LVUP_SEQ_PARMPUT;
			}
		}
		break;

	case LVUP_SEQ_PARMPUT:		// {^Ò¿ -> EBhE«·¦
		if( sys.trg & (PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL) ){
			Snd_SePlay( PLIST_SE_ENTER );
			PokeList_LvUpParamPut( wk );
			wk->lv_seq = LVUP_SEQ_WINEXIT;
		}
		break;

	case LVUP_SEQ_WINEXIT:		// {^Ò¿ -> EBhEñ\¦
		if( sys.trg & (PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL) ){
			Snd_SePlay( PLIST_SE_ENTER );
			PokeList_LvUpWinExit( wk );
			wk->lv_seq = LVUP_SEQ_WAZACHECK;
			wk->dat->lv_cnt = 0;
		}
		break;

	case LVUP_SEQ_WAZACHECK:	// Zo¦`FbN
		pp = PokeParty_GetMemberPointer( wk->dat->pp, wk->pos );

		switch( PokeWazaOboeCheck( pp, &wk->dat->lv_cnt, &wk->dat->waza ) ){
		case NO_WAZA_OBOE:		// o¦çêéZÈµ
			wk->lv_seq = LVUP_SEQ_SHINKACHECK;
			break;

		case NO_WAZA_SET:		// ó«ªÈ¢
			WORDSET_RegisterPokeNickName( wk->wset, 0, PPPPointerGet(pp) );
			WORDSET_RegisterWazaName( wk->wset, 1, wk->dat->waza );
			str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_06 );
			WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
			STRBUF_Delete( str );
			PokeListLargeMsgPut( wk, PL_MSG_COMP_EXPAND, 0 );
			wk->yesno.yes = PL_LvUpNewWazaSetYes;
			wk->yesno.no  = PL_LvUpNewWazaSetNo;
			wk->next_seq = SEQ_YESNO_INIT;
			return SEQ_MSG_WAIT;

		case SAME_WAZA_SET:		// ·ÅÉo¦Ä¢é
			break;

		default:				// o¦½
			WORDSET_RegisterPokeNickName( wk->wset, 0, PPPPointerGet(pp) );
			WORDSET_RegisterWazaName( wk->wset, 1, wk->dat->waza );
			str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_08_10 );
			WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
			STRBUF_Delete( str );
			PokeListLargeMsgPut( wk, PL_MSG_COMP_EXPAND, 0 );
			wk->lv_seq = LVUP_SEQ_WAZASET;
			break;
		}
		break;

	case LVUP_SEQ_WAZASET:			// Zo¦F{^Ò¿io¦½j
		if( GF_MSG_PrintEndCheck( wk->msg_index ) == 0 ){
			if( sys.trg & (PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL) ){
				Snd_SePlay( PLIST_SE_ENTER );
				wk->lv_seq = LVUP_SEQ_WAZACHECK;
			}
		}
		break;

	case LVUP_SEQ_WAZAADD:			// Zo¦F{^Ò¿iYêÄo¦½j
		if( sys.trg & (PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL) ){
			pp  = PokeParty_GetMemberPointer( wk->dat->pp, wk->pos );

			PL_WazaSet( wk, pp, wk->dat->waza_pos );
			str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_11 );
			WORDSET_RegisterWazaName( wk->wset, 1, wk->dat->waza );
			WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
			STRBUF_Delete( str );
			PokeListLargeMsgPut( wk, PL_MSG_COMP_EXPAND, 0 );
			wk->lv_seq = LVUP_SEQ_WAZASET;
		}
		break;

	case LVUP_SEQ_SHINKACHECK:	// i»`FbN
		{
			POKEMON_PARAM * pp;
			FIELDSYS_WORK * fsys;
			int	place;

			pp    = PokeParty_GetMemberPointer( wk->dat->pp, wk->pos );
			fsys  = wk->dat->fsys;
			place = ZoneData_GetShinkaPlaceID( fsys->location->zone_id );
			wk->dat->after_mons = PokeShinkaCheck(
				wk->dat->pp, pp, LEVELUP_SHINKA, place, &wk->dat->shinka_cond );

			if( wk->dat->after_mons != 0 ){
				wk->dat->ret_mode = PL_RET_LVUPSHINKA;
			}else{
				wk->dat->ret_mode = PL_RET_NORMAL;
			}
		}
		return SEQ_OUT_SET;
	}

	return SEQ_ITEMUSE_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * xAbvZo¦
 *
 * @param	wk		|PXgÌ[N
 *
 * @return	ÌV[PX
 */
//--------------------------------------------------------------------------------------------
int PokeList_LvUpWazaSetRetInit( PLIST_WORK * wk )
{
	POKEMON_PARAM * pp;
	STRBUF * str;

	wk->strcv_func = LvUp_RecoverMain;
	wk->lv_seq = LVUP_SEQ_WAZACHECK;

	pp  = PokeParty_GetMemberPointer( wk->dat->pp, wk->pos );

	WORDSET_RegisterPokeNickName( wk->wset, 0, PPPPointerGet(pp) );

	if( wk->dat->waza_pos == 4 ){
		WORDSET_RegisterWazaName( wk->wset, 1, wk->dat->waza );
		return PL_LvUpNewWazaSetNo( wk );
	}

	WORDSET_RegisterWazaName(
		wk->wset, 1, PokeParaGet( pp, ID_PARA_waza1+wk->dat->waza_pos, NULL ) );
	str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_10 );
	WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
	STRBUF_Delete( str );
	PokeListLargeMsgPut( wk, PL_MSG_COMP_EXPAND, 1 );

	wk->next_seq = SEQ_ITEMUSE_WAIT;
	wk->lv_seq = LVUP_SEQ_WAZAADD;

	return SEQ_MSG_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * xAbvZo¦uZðo¦éHv@Í¢
 *
 * @param	wk		|PXgÌ[N
 *
 * @return	ÌV[PX
 */
//--------------------------------------------------------------------------------------------
static int PL_LvUpNewWazaSetYes( void * work )
{
	PLIST_WORK * wk;
	STRBUF * str;

	wk  = work;
	str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_09 );
	WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
	STRBUF_Delete( str );
	PokeListLargeMsgPut( wk, PL_MSG_COMP_EXPAND, 0 );
	wk->dat->ret_mode = PL_RET_LVUP_WAZASET;
	wk->next_seq = SEQ_ENDTRG_WAIT;
	return SEQ_MSG_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * xAbvZo¦uZðo¦éHv@¢¢¦
 *
 * @param	wk		|PXgÌ[N
 *
 * @return	ÌV[PX
 */
//--------------------------------------------------------------------------------------------
static int PL_LvUpNewWazaSetNo( void * work )
{
	PLIST_WORK * wk;
	STRBUF * str;

	wk  = work;
	str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_07 );
	WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
	STRBUF_Delete( str );
	PokeListLargeMsgPut( wk, PL_MSG_COMP_EXPAND, 1 );
	wk->yesno.yes = PL_LvUpNewWazaSetCancelYes;
	wk->yesno.no  = PL_LvUpNewWazaSetCancelNo;
	wk->next_seq = SEQ_YESNO_INIT;
	return SEQ_MSG_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * xAbvZo¦uZðo¦éÌðúßéHv@Í¢
 *
 * @param	wk		|PXgÌ[N
 *
 * @return	ÌV[PX
 */
//--------------------------------------------------------------------------------------------
static int PL_LvUpNewWazaSetCancelYes( void * work )
{
	PLIST_WORK * wk;
	STRBUF * str;

	wk  = work;
	str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_08 );
	WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
	STRBUF_Delete( str );
	PokeListLargeMsgPut( wk, PL_MSG_COMP_EXPAND, 0 );
	wk->next_seq = SEQ_ITEMUSE_WAIT;
	wk->lv_seq = LVUP_SEQ_WAZASET;
	return SEQ_MSG_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * xAbvZo¦uZðo¦éÌðúßéHv@¢¢¦
 *
 * @param	wk		|PXgÌ[N
 *
 * @return	ÌV[PX
 */
//--------------------------------------------------------------------------------------------
static int PL_LvUpNewWazaSetCancelNo( void * work )
{
	PLIST_WORK * wk;
	STRBUF * str;

	wk  = work;
	str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_06 );
	WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
	STRBUF_Delete( str );
	PokeListLargeMsgPut( wk, PL_MSG_COMP_EXPAND, 0 );
	wk->yesno.yes = PL_LvUpNewWazaSetYes;
	wk->yesno.no  = PL_LvUpNewWazaSetNo;
	wk->next_seq = SEQ_YESNO_INIT;
	return SEQ_MSG_WAIT;
}





//--------------------------------------------------------------------------------------------
/**
 * Z}Vgp`FbN
 *
 * @param	wk		|PXgÌ[N
 * @param	pp		POKEMON_PARAM
 *
 * @retval	"0-3 = ZÊu"
 * @retval	"0xfd = ·ÅÉo¦Ä¢é"
 * @retval	"0xfe = Zª¢ÁÏ¢"
 * @retval	"0xff = «ª«¢"
 */
//--------------------------------------------------------------------------------------------
u8 PokeList_WazaOboeCheck( PLIST_WORK * wk, POKEMON_PARAM * pp )
{
	u16	waza;
	u8	i;

	// ·ÅÉo¦Ä¢é©
	for( i=0; i<4; i++ ){
		waza = (u16)PokeParaGet( pp, ID_PARA_waza1+i, NULL );
		if( waza == wk->dat->waza ){
			return 0xfd;
		}
		if( waza == 0 ){ break; }
	}

	// o¦çêé©
	if( PokeParaWazaMachineCheck( pp, WazaMashineNoGet(wk->dat->item) ) == FALSE ){
		return 0xff;
	}

	// ó«ª é©
	if( i == 4 ){
		return 0xfe;
	}

	return i;
}


//--------------------------------------------------------------------------------------------
/**
 * Z}V
 *
 * @param	wk		|PXgÌ[N
 *
 * @return	ÌV[PX
 */
//--------------------------------------------------------------------------------------------
int PokeList_WazaOboeInit( PLIST_WORK * wk )
{
	POKEMON_PARAM * pp;
	STRBUF * str;
	u32	ret;

	pp  = PokeParty_GetMemberPointer( wk->dat->pp, wk->pos );
	ret = PokeList_WazaOboeCheck( wk, pp );

	WORDSET_RegisterPokeNickName( wk->wset, 0, PPPPointerGet(pp) );
	WORDSET_RegisterWazaName( wk->wset, 1, wk->dat->waza );

	switch( ret ){
	case 0:
	case 1:
	case 2:
	case 3:
		PL_WazaSet( wk, pp, ret );
		str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_11 );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		STRBUF_Delete( str );
		PokeListLargeMsgPut( wk, PL_MSG_COMP_EXPAND, 1 );
		wk->dat->ret_mode = PL_RET_NORMAL;
		wk->next_seq = SEQ_ENDTRG_WAIT;
		break;

	case 0xfd:		// o¦Ä¢é
		str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_13 );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		STRBUF_Delete( str );
		PokeListLargeMsgPut( wk, PL_MSG_COMP_EXPAND, 1 );
		wk->dat->ret_mode = PL_RET_NORMAL;
		wk->next_seq = SEQ_ENDTRG_WAIT;
		break;

	case 0xfe:		// ó«Èµ
		str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_06 );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		STRBUF_Delete( str );
		PokeListLargeMsgPut( wk, PL_MSG_COMP_EXPAND, 1 );
		wk->yesno.yes = PL_NewWazaSetYes;
		wk->yesno.no  = PL_NewWazaSetNo;
		wk->next_seq = SEQ_YESNO_INIT;
		break;

	case 0xff:		// «ª«¢
		str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_12 );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		STRBUF_Delete( str );
		PokeListLargeMsgPut( wk, PL_MSG_COMP_EXPAND, 1 );
		wk->dat->ret_mode = PL_RET_NORMAL;
		wk->next_seq = SEQ_ENDTRG_WAIT;
		break;
	}

	return SEQ_MSG_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * ZYê->o¦
 *
 * @param	wk		|PXgÌ[N
 *
 * @return	ÌV[PX
 */
//--------------------------------------------------------------------------------------------
int PokeList_WazaSetRetInit( PLIST_WORK * wk )
{
	POKEMON_PARAM * pp;
	STRBUF * str;

	pp  = PokeParty_GetMemberPointer( wk->dat->pp, wk->pos );

	WORDSET_RegisterPokeNickName( wk->wset, 0, PPPPointerGet(pp) );

	if( wk->dat->waza_pos == 4 ){
		WORDSET_RegisterWazaName( wk->wset, 1, wk->dat->waza );
		return PL_NewWazaSetNo( wk );
	}

	WORDSET_RegisterWazaName(
		wk->wset, 1, PokeParaGet( pp, ID_PARA_waza1+wk->dat->waza_pos, NULL ) );
	str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_10 );
	WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
	STRBUF_Delete( str );
	PokeListLargeMsgPut( wk, PL_MSG_COMP_EXPAND, 1 );

	wk->next_seq = SEQ_WAZASET_RET_SET;
	return SEQ_MSG_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * Zo¦bZ[WI¹Ò¿
 *
 * @param	wk		|PXgÌ[N
 *
 * @return	ÌV[PX
 */
//--------------------------------------------------------------------------------------------
int PokeList_WazaSetRetSet( PLIST_WORK * wk )
{
	POKEMON_PARAM * pp;
	STRBUF * str;

	if( sys.trg & (PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL) ){
		pp  = PokeParty_GetMemberPointer( wk->dat->pp, wk->pos );

		PL_WazaSet( wk, pp, wk->dat->waza_pos );
		str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_11 );
		WORDSET_RegisterWazaName( wk->wset, 1, wk->dat->waza );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		STRBUF_Delete( str );
		PokeListLargeMsgPut( wk, PL_MSG_COMP_EXPAND, 0 );
		wk->dat->ret_mode = PL_RET_NORMAL;
		wk->next_seq = SEQ_ENDTRG_WAIT;
		return SEQ_MSG_WAIT;
	}
	return SEQ_WAZASET_RET_SET;
}

//--------------------------------------------------------------------------------------------
/**
 * Z}VZo¦uZðo¦éHv@Í¢
 *
 * @param	wk		|PXgÌ[N
 *
 * @return	ÌV[PX
 */
//--------------------------------------------------------------------------------------------
static int PL_NewWazaSetYes( void * work )
{
	PLIST_WORK * wk;
	STRBUF * str;

	wk  = work;
	str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_09 );
	WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
	STRBUF_Delete( str );
	PokeListLargeMsgPut( wk, PL_MSG_COMP_EXPAND, 0 );
	wk->dat->ret_mode = PL_RET_WAZASET;
	wk->next_seq = SEQ_ENDTRG_WAIT;
	return SEQ_MSG_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * Z}VZo¦uZðo¦éHv@¢¢¦
 *
 * @param	wk		|PXgÌ[N
 *
 * @return	ÌV[PX
 */
//--------------------------------------------------------------------------------------------
static int PL_NewWazaSetNo( void * work )
{
	PLIST_WORK * wk;
	STRBUF * str;

	wk  = work;
	str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_07 );
	WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
	STRBUF_Delete( str );
	PokeListLargeMsgPut( wk, PL_MSG_COMP_EXPAND, 1 );
	wk->yesno.yes = PL_NewWazaSetCancelYes;
	wk->yesno.no  = PL_NewWazaSetCancelNo;
	wk->next_seq = SEQ_YESNO_INIT;
	return SEQ_MSG_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * Z}VZo¦uZðo¦éÌðúßéHv@Í¢
 *
 * @param	wk		|PXgÌ[N
 *
 * @return	ÌV[PX
 */
//--------------------------------------------------------------------------------------------
static int PL_NewWazaSetCancelYes( void * work )
{
	PLIST_WORK * wk;
	STRBUF * str;

	wk  = work;
	str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_08 );
	WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
	STRBUF_Delete( str );
	PokeListLargeMsgPut( wk, PL_MSG_COMP_EXPAND, 0 );
	wk->dat->ret_mode = PL_RET_NORMAL;
	wk->next_seq = SEQ_ENDTRG_WAIT;
	return SEQ_MSG_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * Z}VZo¦uZðo¦éÌðúßéHv@¢¢¦
 *
 * @param	wk		|PXgÌ[N
 *
 * @return	ÌV[PX
 */
//--------------------------------------------------------------------------------------------
static int PL_NewWazaSetCancelNo( void * work )
{
	PLIST_WORK * wk;
	STRBUF * str;

	wk  = work;
	str = MSGMAN_AllocString( wk->msg_man, mes_pokelist_04_06 );
	WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
	STRBUF_Delete( str );
	PokeListLargeMsgPut( wk, PL_MSG_COMP_EXPAND, 0 );
	wk->yesno.yes = PL_NewWazaSetYes;
	wk->yesno.no  = PL_NewWazaSetNo;
	wk->next_seq = SEQ_YESNO_INIT;
	return SEQ_MSG_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * ZZbg
 *
 * @param	wk		|PXgÌ[N
 * @param	pp		POKEMON_PARAM
 * @param	pos		ZÊu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PL_WazaSet( PLIST_WORK * wk, POKEMON_PARAM * pp, u32 pos )
{
	u32	buf;

	buf = wk->dat->waza;
	PokeParaPut( pp, ID_PARA_waza1+pos, &buf );
	buf = 0;
	PokeParaPut( pp, ID_PARA_pp_count1+pos, &buf );
	buf = WT_PPMaxGet( wk->dat->waza, 0 );
	PokeParaPut( pp, ID_PARA_pp1+pos, &buf );

	if( wk->dat->item != ITEM_DUMMY_DATA ){
		if( HidenWazaCheck( wk->dat->waza ) == FALSE ){
			MyItem_SubItem( wk->dat->myitem, wk->dat->item, 1, HEAPID_POKELIST );
		}
		FriendCalc( pp, FRIEND_WAZA_MACHINE, (u16)PL_PlaceIDGet(wk) );
	}
}



//============================================================================================
//	Zñ
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * ZIðj[Úì¬
 *
 * @param	wk		|PXgÌ[N
 * @param	pos		½ÔÚ©
 *
 * @retval	"0 = ZÈµ"
 * @retval	"1 = ZZbg"
 */
//--------------------------------------------------------------------------------------------
static u8 PL_WazaMenuParamSet( PLIST_WORK * wk, u8 pos )
{
	POKEMON_PARAM * pp;
	STRBUF * str;
	u16	waza;

	pp   = PokeParty_GetMemberPointer( wk->dat->pp, wk->pos );
	waza = (u16)PokeParaGet( pp, ID_PARA_waza1+pos, NULL );
	str  = MSGMAN_AllocString( wk->msg_man, mes_pokelist_05_18+pos );
	WORDSET_RegisterWazaName( wk->wset, 0, waza );
	WORDSET_ExpandStr( wk->wset, wk->str_buf, str );
	STRBUF_Delete( str );
	if( waza == 0 ){
		BMP_MENULIST_AddString( wk->md, wk->str_buf, BMPMENU_DUMMY );
		return 0;
	}
	BMP_MENULIST_AddString( wk->md, wk->str_buf, pos );
	return 1;
}

//--------------------------------------------------------------------------------------------
/**
 * ZIðj[ì¬
 *
 * @param	wk		|PXgÌ[N
 * @param	type	0 = PP Up, 1 = PP Rcv
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PokeList_WazaMenuInit( PLIST_WORK * wk, u8 type )
{
	BMPMENU_HEADER	mh;
	u8	max;

	if( type == 0 ){
		PokeListMiddleMsgPut( wk, mes_pokelist_03_05, 1 );
	}else{
		PokeListMiddleMsgPut( wk, mes_pokelist_03_04, 1 );
	}

	wk->md = BMP_MENULIST_Create( 4, HEAPID_POKELIST );
	max = PL_WazaMenuParamSet( wk, 0 );
	max += PL_WazaMenuParamSet( wk, 1 );
	max += PL_WazaMenuParamSet( wk, 2 );
	max += PL_WazaMenuParamSet( wk, 3 );

	mh.menu  = wk->md;
	mh.win   = &wk->win[WIN_WAZA_MENU];
	mh.font  = FONT_SYSTEM;
	mh.x_max = 1;
	mh.y_max = 4;
	mh.line_spc = 0;
	mh.c_disp_f = 0;
	if( max == 4 ){
		mh.loop_f = 1;
	}else{
		mh.loop_f = 0;
	}

	BmpMenuWinWrite( &wk->win[WIN_WAZA_MENU], WINDOW_TRANS_OFF, PL_MENU_WIN_CGX, PAL_MENU_WIN );
	wk->mw = BmpMenuAddEx( &mh, 8, 0, 0, HEAPID_POKELIST, PAD_BUTTON_CANCEL );
}

//--------------------------------------------------------------------------------------------
/**
 * ZIðj[C
 *
 * @param	wk		|PXgÌ[N
 *
 * @return	ÌV[PX
 */
//--------------------------------------------------------------------------------------------
int PokeList_WazaMenuMain( PLIST_WORK * wk )
{
	u32	ret = BmpMenuMain( wk->mw );

	switch( ret ){
	case BMPMENU_NULL:
		break;

	case BMPMENU_CANCEL:
		// |PIðÖ
		BmpTalkWinClear( &wk->win[WIN_M_MSG], WINDOW_TRANS_OFF );
		BmpMenuWinClear( &wk->win[WIN_WAZA_MENU], WINDOW_TRANS_OFF );
		BmpMenuExit( wk->mw, NULL );
		BMP_MENULIST_Delete( wk->md );
		PokeListSmallMsgPut( wk, mes_pokelist_02_04, 1 );
		return SEQ_ITEMUSE_SEL;

	default:
		BmpTalkWinClear( &wk->win[WIN_M_MSG], WINDOW_TRANS_OFF );
		BmpMenuWinClear( &wk->win[WIN_WAZA_MENU], WINDOW_TRANS_OFF );
		BmpMenuExit( wk->mw, NULL );
		BMP_MENULIST_Delete( wk->md );
		if( PokeParty_StatusRecover( wk->dat->pp, wk->dat->item, wk->pos, (u8)ret, PL_PlaceIDGet(wk), HEAPID_POKELIST ) == TRUE ){
			POKEMON_PARAM * pp = PokeParty_GetMemberPointer( wk->dat->pp, wk->pos );
			SetStatusRecoverMsg(
				wk, wk->dat->item, PokeParaGet( pp, ID_PARA_waza1+ret, NULL ) );
			MyItem_SubItem( wk->dat->myitem, wk->dat->item, 1, HEAPID_POKELIST );
			Snd_SePlay( SEQ_SE_DP_KAIFUKU );
		}else{
			MSGMAN_GetString( wk->msg_man, mes_pokelist_04_45, wk->msg_buf );
		}
		PokeListLargeMsgPut( wk, PL_MSG_COMP_EXPAND, 1 );
		wk->dat->ret_mode = PL_RET_NORMAL;
		wk->next_seq = SEQ_ENDTRG_WAIT;
		return SEQ_MSG_WAIT;
	}

	return SEQ_WAZAMENU_SEL;
}


//============================================================================================
//	[{bNX©ç[ð½¹é
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * [{bNX©ç[ð½¹é
 *
 * @param	wk		|PXgÌ[N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PokeList_MailBoxAdd( PLIST_WORK * wk )
{
	BmpTalkWinClear( &wk->win[WIN_S_MSG], WINDOW_TRANS_OFF );

	if( wk->panel[wk->pos].item == 0 ){
		MSGMAN_GetString( wk->msg_man, mes_pokelist_04_64, wk->msg_buf );
		PokeList_MailIconChg( wk, wk->pos );
	}else{
		MSGMAN_GetString( wk->msg_man, mes_pokelist_04_65, wk->msg_buf );
		wk->pos = PL_SEL_POS_EXIT;
	}

	PokeListLargeMsgPut( wk, PL_MSG_COMP_EXPAND, 1 );
	wk->dat->ret_mode = PL_RET_NORMAL;
	wk->next_seq = SEQ_ENDTRG_WAIT;
}


//--------------------------------------------------------------------------------------------
/**
 * »Ýnæ¾
 *
 * @param	wk		|PXgÌ[N
 *
 * @return	»Ýn
 */
//--------------------------------------------------------------------------------------------
static u16 PL_PlaceIDGet( PLIST_WORK * wk )
{
	FIELDSYS_WORK * fsys = wk->dat->fsys;
	return (u16)ZoneData_GetPlaceNameID( fsys->location->zone_id );
}

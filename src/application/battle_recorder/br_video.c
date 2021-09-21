//==============================================================================
/**
 * @file	br_video.c
 * @brief	バトルビデオ
 * @author	goto
 * @date	2008.02.16(土)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
#include "common.h"
#include "system/procsys.h"
#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "system/pm_str.h"
#include "system/clact_tool.h"
#include "system/msgdata_util.h"
#include "system/wordset.h"
#include "msgdata/msg.naix"
#include "system/brightness.h"
#include "system/fontproc.h"
#include "system/lib_pack.h"
#include "gflib/msg_print.h"
#include "gflib/touchpanel.h"
#include "poketool/poke_tool.h"
#include "poketool/pokeicon.h"
#include "poketool/boxdata.h"
#include "system/snd_tool.h"
#include "gflib/strbuf_family.h"
#include "system/pm_overlay.h"
#include "system/wipe.h"

#include "br_private.h"
#include "br_codein.h"

/*
	上画面をメイン・下画面をサブに　入れ替えて使用しています。
*/

//	TAG_CODE_INPUT
//	TAG_VIDEO_SEND


//==============================================================
// Prototype
//==============================================================
static void ListPokeIcon_PosSet( BR_WORK* wk );
static void ListPokeIcon_Add( BR_WORK* wk );
static void ListPokeIcon_Enable( BR_WORK* wk, BOOL flag );
static void ListPokeIcon_Del( BR_WORK* wk );
static void ListPokeIcon_ReAdd( BR_WORK* wk );
static BOOL IsSearchParamSet( BR_WORK* wk );
static void BRV_WinAdd_Main( BR_WORK* wk, int mode );
static void BRV_Word_Shisetu( BR_WORK* wk, int no );
static void BRV_Word_Pokemon( BR_WORK* wk, int no );
static void BRV_Word_Profile( BR_WORK* wk, int country, int area );
static void BRV_WinAdd_Sub( BR_WORK* wk, int mode );
static void BRV_WinDel_Main( BR_WORK* wk );
static void BRV_WinDel_Sub( BR_WORK* wk );
static void BRV_WinAdd_VideoList( BR_WORK* wk );
static BOOL CS_TopView_Init( BR_WORK* wk );
static BOOL CS_TopView_Main( BR_WORK* wk );
static BOOL CS_TopView_Exit( BR_WORK* wk );
static BOOL CS_SearchCommonInit( BR_WORK* wk );
static BOOL CS_Shisetu( BR_WORK* wk );
static BOOL CS_Pokemon( BR_WORK* wk );
static void WinAdd_AIUEO( BR_WORK* wk );
static void WinDel_AIUEO( BR_WORK* wk );
static BOOL CS_PokemonAIUEO( BR_WORK* wk );
static BOOL CS_PokemonSelect( BR_WORK* wk );
static BOOL IsPokeNameList( BR_WORK* wk, int word );
static void PokemonNameList_DataMake( BR_WORK* wk, int word );
static void PokemonNameList_DataFree( BR_WORK* wk );
static BOOL CS_Profile( BR_WORK* wk );
static BOOL CS_SearchCommonExit( BR_WORK* wk );
static BOOL CS_Search_Main( BR_WORK* wk );
static BOOL CS_Search_Exit( BR_WORK* wk );
static BOOL CS_ListInit( BR_WORK* wk );
static BOOL CS_ListMain( BR_WORK* wk );
static BOOL CS_TopViewRecover( BR_WORK* wk );
static BOOL CS_ProfIn( BR_WORK* wk );
static BOOL CS_ProfMain( BR_WORK* wk );
static BOOL CS_ProfChange( BR_WORK* wk );
static BOOL CS_ProfPlay( BR_WORK* wk );
static BOOL CS_ProfExit( BR_WORK* wk );
static BOOL CS_SaveIn( BR_WORK* wk );
static BOOL CS_SaveMain( BR_WORK* wk );
static BOOL CS_SaveExit( BR_WORK* wk );
static BOOL CS_PlayRecover( BR_WORK* wk );
static BOOL CS_CheckIn( BR_WORK* wk );
static BOOL CS_CheckMain( BR_WORK* wk );
static BOOL CS_CheckExit( BR_WORK* wk );
static BOOL CS_CheckFinish( BR_WORK* wk );
static BOOL CS_BrsDataGet( BR_WORK* wk );
static BOOL CS_RecoverIn( BR_WORK* wk );
static BOOL Video_Init( BR_WORK* wk );
static BOOL Video_Fade_Out( BR_WORK* wk );
static BOOL Video_CodeIn_Init( BR_WORK* wk );
static BOOL Video_CodeIn_Main( BR_WORK* wk );
static BOOL Video_CodeIn_Exit( BR_WORK* wk );
static BOOL Video_CodeIn_End( BR_WORK* wk );
static BOOL Video_CodeIn_DataSearch( BR_WORK* wk );
static void Video_GppDataCreate( BR_WORK* wk );
static void Video_GppDataCreate_from_OutLine( BR_WORK* wk );
static void Video_GppDataDelete( BR_WORK* wk );
static BOOL Video_ProfileIn( BR_WORK* wk );
static BOOL Video_ProfileMain( BR_WORK* wk );
static BOOL Video_ProfileChange( BR_WORK* wk );
static BOOL Video_ProfilePlay( BR_WORK* wk );
static BOOL Video_ProfileExit( BR_WORK* wk );
static void FontOam_SaveData_Set( OAM_BUTTON* obtn, BR_WORK* wk, int no );
static void FontOam_Message_Set( OAM_BUTTON* obtn, BR_WORK* wk, int no );
static void InfoMessage( BR_WORK* wk, int no, BOOL flag );
static void InfoMessageSet( BR_WORK* wk, int no, int data_no );
static void SaveFontButton_Create( BR_WORK* wk );
static void SaveFontButton_Delete( BR_WORK* wk );
static BOOL Video_ProfileSaveIn( BR_WORK* wk );
static BOOL Video_ProfileSaveMain( BR_WORK* wk );
static BOOL Video_ProfileSaveExit( BR_WORK* wk );
static BOOL Video_ProfileCheckIn( BR_WORK* wk );
static BOOL Video_ProfileCheckMain( BR_WORK* wk );
static BOOL Video_ProfileCheckExit( BR_WORK* wk );
static BOOL Video_ProfileCheckFinish( BR_WORK* wk );
static BOOL Video_ProfileRecoverIn( BR_WORK* wk );
static BOOL Video_Send_Init( BR_WORK* wk );
static BOOL Video_Send_Main( BR_WORK* wk );
static BOOL Video_Send_Exit( BR_WORK* wk );
static void ProfWin_Del( BR_WORK* wk );
static void ProfWin_MsgSet( BR_WORK* wk, int id );
static void ProfWin_Add( BR_WORK* wk );
static void SaveWin_Add( BR_WORK* wk );
static void SaveWin_Del( BR_WORK* wk );
static void Btn_CallBack_000( u32 button, u32 event, void* work );
static void Btn_CallBack_001( u32 button, u32 event, void* work );
static void Btn_CallBack_002( u32 button, u32 event, void* work );
static void Btn_CallBack_100( u32 button, u32 event, void* work );
static void Btn_CallBack_AIUEO( u32 button, u32 event, void* work );
static void Btn_CallBack_200( u32 button, u32 event, void* work );
static void Btn_CallBack_201( u32 button, u32 event, void* work );
static void Btn_CallBack_202( u32 button, u32 event, void* work );
static void ButtonOAM_PosSet( BR_WORK* wk, int mode );
static void FontButton_Create( BR_WORK* wk );
static void FontButton_Delete( BR_WORK* wk );
static void ButtonOAM_PosSetSub( BR_WORK* wk, int mode );
static void FontButton_CreateSub( BR_WORK* wk );
static void FontButton_DeleteSub( BR_WORK* wk );
static BOOL Common_ProfChange( BR_WORK* wk );
static BOOL Common_ProfSaveIn( BR_WORK* wk );
static BOOL Common_ProfCheckIn( BR_WORK* wk );
static BOOL Common_ProfCheckExit( BR_WORK* wk );
static BOOL Common_ProfCheckFinish( BR_WORK* wk );
static BOOL Common_ProfSaveExit( BR_WORK* wk );
static BOOL Common_ProfPlayRecover( BR_WORK* wk );
static BOOL Common_ProfPlay( BR_WORK* wk );
extern void BR_Response_VideoRegist( void *work, const GDS_RAP_ERROR_INFO *error_info );
extern void BR_Response_VideoGet( void *work, const GDS_RAP_ERROR_INFO* error_info );
extern void BR_Response_VideoDL( void *work, const GDS_RAP_ERROR_INFO* error_info );

static BOOL Video_ProfileSaveIn( BR_WORK* wk );
static BOOL Video_ProfileSaveMain( BR_WORK* wk );
static BOOL Video_ProfileSaveExit( BR_WORK* wk );
static BOOL Video_PlayRecover( BR_WORK* wk );

static BOOL Video_ProfileCheckIn( BR_WORK* wk );
static BOOL Video_ProfileCheckMain( BR_WORK* wk );
static BOOL Video_ProfileCheckExit( BR_WORK* wk );
static BOOL Video_ProfileCheckFinish( BR_WORK* wk );
static BOOL Video_ProfileRecoverIn( BR_WORK* wk );

static BOOL Video_Send_Init( BR_WORK* wk );
static BOOL Video_Send_Main( BR_WORK* wk );
static BOOL Video_Send_Exit( BR_WORK* wk );

static u16 NameHeadTable[]={
	ZKN_AKSTNHMYRW_IDX_A,
	ZKN_AKSTNHMYRW_IDX_K,
	ZKN_AKSTNHMYRW_IDX_S,
	ZKN_AKSTNHMYRW_IDX_T,
	ZKN_AKSTNHMYRW_IDX_N,
	ZKN_AKSTNHMYRW_IDX_H,
	ZKN_AKSTNHMYRW_IDX_M,
	ZKN_AKSTNHMYRW_IDX_Y,
	ZKN_AKSTNHMYRW_IDX_R,
    // MatchComment: remove ZKN_AKSTNHMYRW_IDX_W,
	ZKN_AKSTNHMYRW_IDX_END,
};

typedef struct {
	
	int					eva;
	int					evb;
	int					color;
	int					tmp_seq;
	
	OAM_BUTTON			obtn[ 4 ];			///< もどる　ＯＫ！

	GPP_WORK			gpp_work;	
	VIDEO_WORK			vtr_work;

	int					save_no;
	int					view_page;			///< 0 = プロフィール 1 = 戻る	
	GDS_PROFILE_PTR		gds_profile_ptr;	///< プロフィールポインタ
	
	///< コード検索で使う
	BR_CODEIN_WORK		codein;
	BR_CODEIN_PARAM*	codein_param;
	u64					data_number;
	BOOL				bData;
	GT_BATTLE_REC_RECV*	rec_data;
	
	///< くわしくさがす　で使う
	u16					monsno;
	u8					battle_mode;
	u8					country_code;
	u8					local_code;
	u8					search_mode;
	u8					dmy[ 2 ];
	POKE_SEL_WORK		poke_select;
	TL_INFO*			tl_poke_list_info;	///< どーしても可変になるので ポケモンの個数Alloc
	TOUCH_LIST_HEADER	tl_poke_list_head;	///< どーしても可変になるので
	
	POKE_ICON_WORK		icon;

	BUTTON_MAN*			btn_man[ 3 ];
	
	///< BMP WIN
	GF_BGL_BMPWIN		win_m[ 12 ];
	GF_BGL_BMPWIN		win_s[ 12 ];
	int					win_m_num;
	int					win_s_num;
	
	u8					secure_flag;

} VIDEO_SEARCH_WORK;

static void BR_SendVideoNo( VIDEO_SEARCH_WORK* vwk, BR_WORK* wk );

///< プロフィール画面
static const RECT_HIT_TBL hit_table_000[] = {
	{ DTC(  2 ), DTC(  6 ), DTC(  8 ), DTC( 25 ) },	///< バトルビデオ	
	{ DTC(  8 ), DTC( 14 ), DTC( 11 ), DTC( 21 ) },	///< 再生	
	{ 152, 152 + 32, 128, 128 + 96 },	///< 戻る
	{ 152, 152 + 32,  32,  32 + 96 },	///< 保存
};

///< 保存先
static const RECT_HIT_TBL hit_table_001[] = {	
	{ TAG_HIT_Y( 0 ) - 16, TAG_HIT_Y( 0 ) + 16, TAG_HIT_X, TAG_HIT_X + 160 },
	{ TAG_HIT_Y( 1 ) - 16, TAG_HIT_Y( 1 ) + 16, TAG_HIT_X, TAG_HIT_X + 160 },
	{ TAG_HIT_Y( 2 ) - 16, TAG_HIT_Y( 2 ) + 16, TAG_HIT_X, TAG_HIT_X + 160 },
	{ TAG_HIT_Y( 3 ) - 16, TAG_HIT_Y( 3 ) + 16, TAG_HIT_X, TAG_HIT_X + 160 },	
};

///< 上書きしますか？はい・いいえ スクリーン
static const RECT_HIT_TBL hit_table_002[] = {
	{ DTC(  5 ), DTC( 9 ), DTC(  4 ), DTC( 15 ) },	///< はい
	{ DTC(  5 ), DTC( 9 ), DTC( 18 ), DTC( 29 ) },	///< いいえ	
};

static const RECT_HIT_TBL hit_table_back[] = {	
	{ 152, 184, 80, 176 },	///< 戻る
};

static const RECT_HIT_TBL hit_table_view[] = {
	{ DTC( 4 ), DTC( 12 ), DTC( 10 ), DTC( 22 ) },	///< ビデオを見る
};

// ----------

///< くわしく探すTOP
static const RECT_HIT_TBL hit_table_100[] = {
	{ DTC(  2 ), DTC(  6 ), DTC(  5 ), DTC( 27 ) },	///< しせつでさがす
	{ DTC(  7 ), DTC( 11 ), DTC(  5 ), DTC( 27 ) },	///< ポケモンでさがす
	{ DTC( 12 ), DTC( 16 ), DTC(  5 ), DTC( 27 ) },	///< プロフィールでさがす
	{ 152, 152 + 32,  32,  32 + 96 },	///< 
	{ 152, 152 + 32, 128, 128 + 96 },	///< もどる
};

///< ポケモンで探す　あいうえお
// MatchComment: change entries to match US data (also means changing length)
const RECT_HIT_TBL hit_rect_AIUEO[ 9 ] = {	
	{  3 * 8,  6 * 8,  5 * 8,  9 * 8},
	{  3 * 8,  6 * 8, 14 * 8, 18 * 8},
	{  3 * 8,  6 * 8, 23 * 8, 27 * 8},

	{  7 * 8, 10 * 8,  5 * 8,  9 * 8},
	{  7 * 8, 10 * 8, 14 * 8, 18 * 8},
	{  7 * 8, 10 * 8, 23 * 8, 27 * 8},

	{ 11 * 8, 14 * 8,  5 * 8,  9 * 8},
	{ 11 * 8, 14 * 8, 14 * 8, 18 * 8},
	{ 11 * 8, 14 * 8, 23 * 8, 27 * 8},

};

// ----------

#define PRINT_COL_VIDEO		( PRINT_COL_NORMAL )
#define PRINT_COL_VIDEO2	( GF_PRINTCOLOR_MAKE( 12, 11,  0 ) )

static void ListPokeIcon_PosSet( BR_WORK* wk )
{
	const s16 icon_pos[][ 2 ] = {
		{ 13 * 8, 9 * 8 },{ 16 * 8, 9 * 8 },{ 19 * 8, 9 * 8 },{ 22 * 8, 9 * 8 },{ 25 * 8, 9 * 8 },{ 28 * 8, 9 * 8 },
		{ 13 * 8,12 * 8 },{ 16 * 8,12 * 8 },{ 19 * 8,12 * 8 },{ 22 * 8,12 * 8 },{ 25 * 8,12 * 8 },{ 28 * 8,12 * 8 },
		{ 13 * 8,15 * 8 },{ 16 * 8,15 * 8 },{ 19 * 8,15 * 8 },{ 22 * 8,15 * 8 },{ 25 * 8,15 * 8 },{ 28 * 8,15 * 8 },
		{ 13 * 8,18 * 8 },{ 16 * 8,18 * 8 },{ 19 * 8,18 * 8 },{ 22 * 8,18 * 8 },{ 25 * 8,18 * 8 },{ 28 * 8,18 * 8 },
		{ 13 * 8,21 * 8 },{ 16 * 8,21 * 8 },{ 19 * 8,21 * 8 },{ 22 * 8,21 * 8 },{ 25 * 8,21 * 8 },{ 28 * 8,21 * 8 },
	};
	int i,j;
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	for ( i = 0; i < 5; i++ ){
		for ( j = 0; j < 6; j++ ){			
			if ( vwk->icon.poke[ i ][ j ].cap ){
				CATS_ObjectPosSetCap( vwk->icon.poke[ i ][ j ].cap, icon_pos[ ( i * 6 ) + j ][ 0 ], icon_pos[ ( i * 6 ) + j ][ 1 ] );
			}			
		}
	}
}

static void ListPokeIcon_Add( BR_WORK* wk )
{
	int i,j;
	int index = 0;
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	int tbl[][ 6 ] = {
		{ 0,1,2,3,4,5 },	///< 1vs1
		{ 0,1,2,3,4,5 },	///< 2vs2  --味味敵敵
	//	{ 0,1,2,6,7,8 },
	};
	int monsno_tbl[ 6 ] = { 0,0,0,0,0,0 };
	int form_tbl[ 6 ]	= { 0,0,0,0,0,0 };
	int tbl_index = 0;	
	int monsno;
	int form;
	int egg;
	int now = wk->sys.touch_list.now;
		
	OS_Printf( "now = %d\n", now );
	
	for ( i = 0; i < 5; i++ ){
		if ( wk->recv_data.outline_data_tmp[ now + i ] == NULL ){ continue; }
		
		if ( VideoCheck2vs2_Head( ( BATTLE_REC_HEADER_PTR )&wk->recv_data.outline_data_tmp[ now + i ]->head ) ){
			tbl_index = 1;
		}
		else {
			tbl_index = 0;
		}
		
		///< たまごを抜いたソートをする 05.14
		{
			int n,m;
			int idx = 0;
			
			for ( n = 0; n < 6; n++ ){
				monsno_tbl[ n ] = 0;
				form_tbl[ n ]	= 0;				
			}
			
			for ( n = 0; n < 3; n++ ){
				monsno = wk->recv_data.outline_data_tmp[ now + i ]->head.monsno[ tbl[ tbl_index ][ n ] ];				
				if ( monsno == 0 ){ continue; }
				monsno_tbl[ idx ] = monsno;
				form_tbl[ idx ] = wk->recv_data.outline_data_tmp[ now + i ]->head.form_no[ tbl[ tbl_index ][ n ] ];
				idx++;
			}
			if ( VideoCheck2vs2_Head( ( BATTLE_REC_HEADER_PTR )&wk->recv_data.outline_data_tmp[ now + i ]->head ) ){
				idx = 3;			
			}
			for ( n = 3; n < 6; n++ ){
				monsno = wk->recv_data.outline_data_tmp[ now + i ]->head.monsno[ tbl[ tbl_index ][ n ] ];				
				if ( monsno == 0 ){ continue; }
				monsno_tbl[ idx ] = monsno;
				form_tbl[ idx ] = wk->recv_data.outline_data_tmp[ now + i ]->head.form_no[ tbl[ tbl_index ][ n ] ];
				idx++;
			}
		}
		
		for ( j = 0; j < 6; j++ ){
			monsno = monsno_tbl[ j ];//wk->recv_data.outline_data_tmp[ now + i ]->head.monsno[ tbl[ tbl_index ][ j ] ];
			form   = form_tbl[ j ];//wk->recv_data.outline_data_tmp[ now + i ]->head.monsno[ tbl[ tbl_index ][ j ] ];
			egg	   = 0;
			vwk->icon.poke[ i ][ j ].gra_id = vwk->icon.global_gra_id;
			vwk->icon.poke[ i ][ j ].cap	= PokemonIcon_AddHDL( wk, vwk->icon.handle, vwk->icon.poke[ i ][ j ].gra_id, monsno, form, egg );
			if ( vwk->icon.poke[ i ][ j ].cap ){
				CATS_ObjectPriSetCap( vwk->icon.poke[ i ][ j ].cap, 6 - j );
			}
			vwk->icon.global_gra_id++;
		}
	}
	ListPokeIcon_PosSet( wk );
}

static void ListPokeIcon_Enable( BR_WORK* wk, BOOL flag )
{
	int i,j;
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	for ( i = 0; i < 5; i++ ){
		for ( j = 0; j < 6; j++ ){			
			if ( vwk->icon.poke[ i ][ j ].cap ){
				CATS_ObjectEnableCap( vwk->icon.poke[ i ][ j ].cap, flag );
			}			
		}
	}
}

static void ListPokeIcon_Del( BR_WORK* wk )
{
	int i,j;
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	CATS_SystemActiveSet( wk, TRUE );
	for ( i = 0; i < 5; i++ ){
		for ( j = 0; j < 6; j++ ){			
			if ( vwk->icon.poke[ i ][ j ].cap ){
				PokemonIcon_Del( wk, vwk->icon.poke[ i ][ j ].gra_id, vwk->icon.poke[ i ][ j ].cap );
				vwk->icon.poke[ i ][ j ].cap = NULL;
			}			
		}
	}
	CATS_SystemActiveSet( wk, FALSE );
}

static void ListPokeIcon_ReAdd( BR_WORK* wk )
{
	int i,j;
	int index = 0;
	int old_index;
	int gra_id[ 6 ] = { 0, 0, 0, 0, 0, 0 };
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	if ( vwk->icon.view_start == wk->sys.touch_list.now ){ return; }

	CATS_SystemActiveSet( wk, TRUE );
	
	///< どっちへリストが動いたか
	if ( vwk->icon.view_start > wk->sys.touch_list.now ){
		///< ↑ = 最下段のアイコンの破棄
		index = 4;
	}
	
	///< 最上段か、最下段のアイコンを削除
	for ( i = 0; i < 6; i++ ){
		if ( vwk->icon.poke[ index ][ i ].cap ){
			PokemonIcon_DelActor( wk, vwk->icon.poke[ index ][ i ].gra_id, vwk->icon.poke[ index ][ i ].cap );
			gra_id[ i ] = vwk->icon.poke[ index ][ i ].gra_id;
			vwk->icon.poke[ index ][ i ].cap = NULL;
		}
	}

	if ( index ){
		///< 下へ詰めて、最上段をフリーにする
		for ( i = 4; i >= 1; i-- ){
			for ( j = 0; j < 6; j++ ){
				vwk->icon.poke[ i ][ j ].cap	= vwk->icon.poke[ i - 1 ][ j ].cap;
				vwk->icon.poke[ i ][ j ].gra_id = vwk->icon.poke[ i - 1 ][ j ].gra_id;
			}
		}
		index = 0;	///< index を 空けた箇所へ
	}
	else {
		///< 上へ詰めて、最下段をフリーにする
		for ( i = 1; i < 5; i++ ){
			for ( j = 0; j < 6; j++ ){
				vwk->icon.poke[ i - 1 ][ j ].cap	= vwk->icon.poke[ i ][ j ].cap;
				vwk->icon.poke[ i - 1 ][ j ].gra_id = vwk->icon.poke[ i ][ j ].gra_id;
			}
		}
		index = 4;	///< index を 空けた箇所へ
	}

	{
		int monsno;
		int form;
		int	egg;
		int pos = wk->sys.touch_list.now + index;
		int tbl[][ 6 ] = {
			{ 0,1,2,3,4,5 },
			{ 0,1,2,3,4,5 },	///< 2vs2  --味味敵敵
		//	{ 0,1,2,6,7,8 },
		};
		int monsno_tbl[ 6 ] = { 0,0,0,0,0,0 };
		int form_tbl[ 6 ]	= { 0,0,0,0,0,0 };
		int tbl_index = 0;

		if ( VideoCheck2vs2_Head( ( BATTLE_REC_HEADER_PTR )&wk->recv_data.outline_data_tmp[ pos ]->head ) ){
			tbl_index = 1;
		}
						
		///< たまごを抜いたソートをする 05.14
		{
			int n,m;
			int idx = 0;
			
			for ( n = 0; n < 6; n++ ){
				monsno_tbl[ n ] = 0;
				form_tbl[ n ]	= 0;				
			}
			
			for ( n = 0; n < 3; n++ ){
				monsno = wk->recv_data.outline_data_tmp[ pos ]->head.monsno[ tbl[ tbl_index ][ n ] ];				
				if ( monsno == 0 ){ continue; }
				monsno_tbl[ idx ] = monsno;
				form_tbl[ idx ] = wk->recv_data.outline_data_tmp[ pos ]->head.form_no[ tbl[ tbl_index ][ n ] ];
				idx++;
			}
			if ( VideoCheck2vs2_Head( ( BATTLE_REC_HEADER_PTR )&wk->recv_data.outline_data_tmp[ pos ]->head ) ){
				idx = 3;
			}
			for ( n = 3; n < 6; n++ ){
				monsno = wk->recv_data.outline_data_tmp[ pos ]->head.monsno[ tbl[ tbl_index ][ n ] ];				
				if ( monsno == 0 ){ continue; }
				monsno_tbl[ idx ] = monsno;
				form_tbl[ idx ] = wk->recv_data.outline_data_tmp[ pos ]->head.form_no[ tbl[ tbl_index ][ n ] ];
				idx++;
			}
		}
		
		for ( j = 0; j < 6; j++ ){
			monsno = monsno_tbl[ j ];//wk->recv_data.outline_data_tmp[ pos ]->head.monsno[ tbl[ tbl_index ][ j ] ];
			form   = form_tbl[ j ];//wk->recv_data.outline_data_tmp[ pos ]->head.form_no[ tbl[ tbl_index ][ j ] ];
			egg	   = 0;
			vwk->icon.poke[ index ][ j ].gra_id = vwk->icon.global_gra_id;
			vwk->icon.poke[ index ][ j ].cap	= PokemonIcon_AddHDL( wk, vwk->icon.handle, vwk->icon.poke[ index ][ j ].gra_id, monsno, form, egg );
			if ( vwk->icon.poke[ index ][ j ].cap ){
				CATS_ObjectPriSetCap( vwk->icon.poke[ index ][ j ].cap, 6 - j );
			}
			vwk->icon.global_gra_id++;
		}
	}
	ListPokeIcon_PosSet( wk );	
	vwk->icon.view_start = wk->sys.touch_list.now;

	for ( i = 0; i < 6; i++ ){
		if ( gra_id[ i ] == 0 ){ continue; }
		CATS_FreeResourceChar( wk->sys.crp, eID_OAM_POKE_ICON + gra_id[ i ] );
	}

	CATS_SystemActiveSet( wk, FALSE );
}

//--------------------------------------------------------------
/**
 * @brief	検索条件を入れたか？
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL IsSearchParamSet( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	if ( vwk->monsno != GT_BATTLE_REC_SEARCH_MONSNO_NONE ){ return TRUE; }
	
	if ( vwk->battle_mode != GT_BATTLE_REC_SEARCH_BATTLE_MODE_NONE ){ return TRUE; }
	
	if ( vwk->country_code != GT_BATTLE_REC_SEARCH_COUNTRY_CODE_NONE
	||	 vwk->local_code != GT_BATTLE_REC_SEARCH_LOCAL_CODE_NONE ){ return TRUE; }
	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	メイン表示
 *
 * @param	wk	
 * @param	mode	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BRV_WinAdd_Main( BR_WORK* wk, int mode )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	int num;
	int	center;
	const int (*bmp)[4];
	const int *msg; 

	const int bmp_win_m0[][4] = {
		{ 4, 3, 15, 2 },
		{ 4, 9, 15, 2 },
		{ 4,13, 15, 2 },
		{ 5, 5, 20, 4 },
		{ 5,11, 20, 2 },
		{ 5,15, 20, 2 },		
	};
	const int msg_m0[] = { msg_205,msg_206,msg_207,msg_722,msg_722,msg_722 };
	
	switch ( mode ){
	case 0:
		vwk->win_m_num = 6;
		center = 0;
		bmp = bmp_win_m0;
		msg = msg_m0;
		break;

	case 1:
		break;

	default:
		break;
	};

	{
		int i;
		int x = 0;
		int ofs = 256;
		STRBUF* str1;
		GF_BGL_BMPWIN* win;
		
		for ( i = 0; i < vwk->win_m_num; i++ ){
			win = &vwk->win_m[ i ];
			GF_BGL_BmpWinInit( win );
			GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_M, bmp[ i ][ 0 ], bmp[ i ][ 1 ], bmp[ i ][ 2 ], bmp[ i ][ 3 ], eBG_PAL_FONT, ofs );
			GF_BGL_BmpWinDataFill( win, 0x00 );
			str1 = MSGMAN_AllocString( wk->sys.man, msg[ i ] );
			ofs += bmp[ i ][ 2 ] * bmp[ i ][ 3 ];			
			if ( center ){
				x = BR_print_x_Get( win, str1 );
			}
			GF_STR_PrintColor( win, FONT_SYSTEM, str1, 0, 0, MSG_NO_PUT, PRINT_COL_VIDEO, NULL );
			GF_BGL_BmpWinOnVReq( win );
			STRBUF_Delete( str1 );
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief	施設　検索
 *
 * @param	wk	
 * @param	no	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BRV_Word_Shisetu( BR_WORK* wk, int no )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	GF_BGL_BMPWIN* win;
	STRBUF* str1;
	int index;
	
	win = &vwk->win_m[ 3 ];
	GF_BGL_BmpWinDataFill( win, 0x00 );
	if ( no == GT_BATTLE_REC_SEARCH_BATTLE_MODE_NONE ){
		str1 = MSGMAN_AllocString( wk->sys.man, msg_722 );	
	}
	else {
		if ( no == GT_BATTLE_MODE_COLOSSEUM_SINGLE_NO_REGULATION
		||	 no == GT_BATTLE_MODE_COLOSSEUM_SINGLE_REGULATION
		||	 no == GT_BATTLE_MODE_COLOSSEUM_DOUBLE_NO_REGULATION
		||	 no == GT_BATTLE_MODE_COLOSSEUM_DOUBLE_REGULATION ){
			index = msg_rule_021 + ( no - GT_BATTLE_MODE_COLOSSEUM_SINGLE_NO_REGULATION );
			str1 = MSGMAN_AllocString( wk->sys.man, index );
		}
		else {
			index = msg_rule_000 + no;
			str1 = MSGMAN_AllocString( wk->sys.man, index );
		}
	}
	GF_STR_PrintColor( win, FONT_SYSTEM, str1, 0, 0, MSG_NO_PUT, PRINT_COL_VIDEO, NULL );
	GF_BGL_BmpWinOnVReq( win );
	STRBUF_Delete( str1 );
}


//--------------------------------------------------------------
/**
 * @brief	ポケモン　検索
 *
 * @param	wk	
 * @param	no	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BRV_Word_Pokemon( BR_WORK* wk, int no )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	GF_BGL_BMPWIN* win;
	STRBUF* str1;
	STRCODE code[ 255 ];
	
	win = &vwk->win_m[ 4 ];
	GF_BGL_BmpWinDataFill( win, 0x00 );
	if ( no == GT_BATTLE_REC_SEARCH_MONSNO_NONE ){
		str1 = MSGMAN_AllocString( wk->sys.man, msg_722 );	
	}
	else {
		str1 = STRBUF_Create( 255, HEAPID_BR );		
		MSGDAT_MonsNameGet( no, HEAPID_BR, code );
		STRBUF_SetStringCode( str1, code );
	}
	GF_STR_PrintColor( win, FONT_SYSTEM, str1, 0, 0, MSG_NO_PUT, PRINT_COL_VIDEO, NULL );
	GF_BGL_BmpWinOnVReq( win );
	STRBUF_Delete( str1 );
}


//--------------------------------------------------------------
/**
 * @brief	プロフィール
 *
 * @param	wk	
 * @param	no	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BRV_Word_Profile( BR_WORK* wk, int country, int area )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	GF_BGL_BMPWIN* win;
	STRBUF*		str1;
	STRBUF*		tmp;
	WORDSET*	wset;
	
	win = &vwk->win_m[ 5 ];
	GF_BGL_BmpWinDataFill( win, 0x00 );
	if ( country == GT_BATTLE_REC_SEARCH_COUNTRY_CODE_NONE && area == GT_BATTLE_REC_SEARCH_LOCAL_CODE_NONE ){
		str1 = MSGMAN_AllocString( wk->sys.man, msg_722 );	
	}
	else {
		
		wset = BR_WORDSET_Create( HEAPID_BR );
		// MatchComment: GT_BATTLE_REC_SEARCH_LOCAL_CODE_NONE -> 0
        // TODO__fix_me: what does the 0 mean here?
		if ( area != 0){
			str1  = STRBUF_Create( 255, HEAPID_BR );
			tmp = MSGMAN_AllocString( wk->sys.man, msg_23 );
			WORDSET_RegisterLocalPlaceName( wset, 0, country, area );
			WORDSET_ExpandStr( wset, str1, tmp );
		}
		else {
			str1  = STRBUF_Create( 255, HEAPID_BR );
			tmp = MSGMAN_AllocString( wk->sys.man, msg_22 );
			WORDSET_RegisterCountryName( wset, 0, country );
			WORDSET_ExpandStr( wset, str1, tmp );			
		}
		STRBUF_Delete( tmp );
		WORDSET_Delete( wset );				
	}
	GF_STR_PrintColor( win, FONT_SYSTEM, str1, 0, 0, MSG_NO_PUT, PRINT_COL_VIDEO, NULL );
	GF_BGL_BmpWinOnVReq( win );
	STRBUF_Delete( str1 );
}


//--------------------------------------------------------------
/**
 * @brief	サブ表示
 *
 * @param	wk	
 * @param	mode	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BRV_WinAdd_Sub( BR_WORK* wk, int mode )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	int num;
	int	center;
	const int (*bmp)[4];
	const int *msg; 

	const int bmp_win_s0[][4] = {
		{ 7, 3, 20, 2 },{ 7, 8, 20, 2 },{ 7,13, 20, 2 },
	};
	const int msg_s0[] = { msg_205,msg_206,msg_207 };
	
	switch ( mode ){
	case 0:
		vwk->win_s_num = 3;
		center = 0;
		bmp = bmp_win_s0;
		msg = msg_s0;
		break;

	case 1:
		break;

	default:
		break;
	};
	
	{
		int i;
		int x = 0;
		int ofs = 256;
		STRBUF* str1;
		GF_BGL_BMPWIN* win;
		
		for ( i = 0; i < vwk->win_s_num; i++ ){
			win = &vwk->win_s[ i ];
			GF_BGL_BmpWinInit( win );
			GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_S, bmp[ i ][ 0 ], bmp[ i ][ 1 ], bmp[ i ][ 2 ], bmp[ i ][ 3 ], eBG_PAL_FONT, ofs );
			GF_BGL_BmpWinDataFill( win, 0x00 );
			str1 = MSGMAN_AllocString( wk->sys.man, msg[ i ] );
			ofs += bmp[ i ][ 2 ] * bmp[ i ][ 3 ];			
			if ( center ){
				x = BR_print_x_Get( win, str1 );
			}
			GF_STR_PrintColor( win, FONT_SYSTEM, str1, 0, 0, MSG_NO_PUT, PRINT_COL_VIDEO, NULL );
			GF_BGL_BmpWinOnVReq( win );
			STRBUF_Delete( str1 );
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief	メイン削除
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BRV_WinDel_Main( BR_WORK* wk )
{
	int i;
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	for ( i = 0; i < vwk->win_m_num; i++ ){
		GF_BGL_BmpWinOff( &vwk->win_m[ i ] );
		GF_BGL_BmpWinDel( &vwk->win_m[ i ] );			
	}
	vwk->win_m_num = 0;
}

//--------------------------------------------------------------
/**
 * @brief	サブ削除
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BRV_WinDel_Sub( BR_WORK* wk )
{
	int i;
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	for ( i = 0; i < vwk->win_s_num; i++ ){
		GF_BGL_BmpWinOff( &vwk->win_s[ i ] );
		GF_BGL_BmpWinDel( &vwk->win_s[ i ] );			
	}
	vwk->win_s_num = 0;
}


//--------------------------------------------------------------
/**
 * @brief	ビデオ検索結果用
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BRV_WinAdd_VideoList( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;


	int x = 0;
	int ofs = 32;
	STRBUF* str1;
	GF_BGL_BMPWIN* win;
	
	///< 見出し
	win = &vwk->win_m[ 0 ];
	GF_BGL_BmpWinInit( win );
	GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_M, 3, 3, 20, 4, eBG_PAL_FONT, ofs );
	GF_BGL_BmpWinDataFill( win, 0x00 );
	str1 = MSGMAN_AllocString( wk->sys.man, msg_723 );
	GF_STR_PrintColor( win, FONT_SYSTEM, str1, 0, 0, MSG_NO_PUT, PRINT_COL_VIDEO, NULL );
	GF_BGL_BmpWinOnVReq( win );
	STRBUF_Delete( str1 );

	///< ビデオを見る
	ofs = 256;
	win = &vwk->win_s[ 0 ];
	GF_BGL_BmpWinInit( win );
	GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_S, 11, 6, 10, 4, eBG_PAL_FONT, ofs );
	GF_BGL_BmpWinDataFill( win, 0x00 );
	str1 = MSGMAN_AllocString( wk->sys.man, msg_603 );
    // MatchComment: new change in plat US
    {
        u32 width = (80 - FontProc_GetPrintMaxLineWidth(FONT_SYSTEM, str1, 0)) / 2;
        GF_STR_PrintColor( win, FONT_SYSTEM, str1, width, 0, MSG_NO_PUT, PRINT_COL_VIDEO, NULL );
    }
	GF_BGL_BmpWinOnVReq( win );
	STRBUF_Delete( str1 );
	
	vwk->win_s_num = 1;
	vwk->win_m_num = 1;
}


//--------------------------------------------------------------
/**
 * @brief	リストの初期化
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL CS_TopView_Init( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;	
	
	switch ( wk->sub_seq ){
	case 0:
		PrioritySet_Common();
		Plate_AlphaInit_Default( &vwk->eva, &vwk->evb, ePLANE_ALL );
		ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR,	 wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
		ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR,	 wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_find_u_NSCR,  GF_BGL_FRAME3_M );
		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_find_d3_NSCR, GF_BGL_FRAME3_S );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG3, VISIBLE_ON );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
		FontButton_CreateSub( wk );
		ButtonOAM_PosSetSub( wk, 0 );
		vwk->icon.global_gra_id = GLOBAL_POKE_ICON_ID;	///< 気持ちずらす
		vwk->btn_man[ 0 ] = BMN_Create( hit_table_100,  NELEMS( hit_table_100 ),  Btn_CallBack_100,   wk, HEAPID_BR );
		vwk->btn_man[ 1 ] = BMN_Create( hit_rect_AIUEO, NELEMS( hit_rect_AIUEO ), Btn_CallBack_AIUEO, wk, HEAPID_BR );
//		vwk->btn_man[ 2 ] = BMN_Create( hit_table_102, NELEMS( hit_table_102 ), Btn_CallBack_002, wk, HEAPID_BR );
		wk->sub_seq++;
		break;
	
	case 1:
		BR_PaletteFade( &vwk->color, eFADE_MODE_IN );
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_IN, ePLANE_ALL ) ){
			Tool_InfoMessageMain( wk, msg_721 );	///< メッセージとの兼ね合いがアル
			BRV_WinAdd_Main( wk, 0 );				///< カテゴリ出す
			BRV_WinAdd_Sub( wk, 0 );				///< カテゴリ出す
			vwk->monsno = GT_BATTLE_REC_SEARCH_MONSNO_NONE;
			vwk->battle_mode = GT_BATTLE_REC_SEARCH_BATTLE_MODE_NONE;
			vwk->country_code = GT_BATTLE_REC_SEARCH_COUNTRY_CODE_NONE;
			vwk->local_code = GT_BATTLE_REC_SEARCH_LOCAL_CODE_NONE;
			BRV_Word_Pokemon( wk, vwk->monsno );
			BRV_Word_Shisetu( wk, vwk->battle_mode );
			BRV_Word_Profile( wk, vwk->country_code, vwk->local_code );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );	
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );
		break;
	
	default:
		BR_Main_SeqChange( wk, eCS_TopViewMain );
		break;
	}
		
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	リストのメイン処理
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL CS_TopView_Main( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	BMN_Main( vwk->btn_man[ 0 ] );

#ifdef BR_AUTO_SERCH
	if ( sys.trg & PAD_BUTTON_SELECT ){
		g_debug_auto ^= 1;		
		g_debug_count = 0;
	}
	if ( g_debug_auto ){
		Btn_CallBack_100( 4, BMN_EVENT_TOUCH, wk );
		g_debug_count++;
		OS_Printf( "---------------loop debug count = %d\n", g_debug_count );
	}
#endif
	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	リストの終了処理
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL CS_TopView_Exit( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
		///< OAM とか 文字とか消す
		BRV_WinDel_Main( wk );
		BRV_WinDel_Sub( wk );
		Tool_InfoMessageMainDel( wk );
		
		BMN_Delete( vwk->btn_man[ 0 ] );
		BMN_Delete( vwk->btn_man[ 1 ] );
//		BMN_Delete( vwk->btn_man[ 2 ] );		
		wk->sub_seq++;
		break;
	
	case 1:
		///< フェードアウト
		BR_PaletteFade( &vwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){			
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );
		break;
	
	case 2:
		///< 復帰
		CATS_SystemActiveSet( wk, TRUE );
		FontButton_DeleteSub( wk );
		NormalTag_RecoverAllOp( wk );
		CATS_SystemActiveSet( wk, FALSE );
		BR_ChangeDisplayVIntr( wk, DISP_3D_TO_SUB );				///< main = top
		wk->sub_seq++;
		break;
	
	default:
		if ( BR_IsChangeDisplay( wk ) == FALSE ){ break; }
		if ( BR_PaletteFade( &vwk->color, eFADE_MODE_IN ) ){					
			PaletteFadeClear( wk );
			BR_PaletteFadeIn_Init( &vwk->color );
			ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, 16, wk->sys.logo_color );	///< main	oam			
			BR_Main_ProcSeqChange( wk, eMAIN_Input, TRUE, wk->proc_seq );
			BR_Main_SeqChange( wk, eINPUT_MENU_IN );		
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_S );
			sys_FreeMemoryEz( vwk );
		}
		else {
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG,	 LOGO_PALETTE, vwk->color, wk->sys.logo_color );	///< main	oam
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );	///< main	oam
		}
		break;
	}
	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	各画面を終了させる
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL CS_SearchCommonInit( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;

	switch ( wk->sub_seq ){
	case 0:
		BRV_WinDel_Sub( wk );
		wk->sub_seq++;
	
	case 1:
		BR_PaletteFade( &vwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_OUT, ePLANE_SUB ) ){			
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );		
		break;
	
	default:
	//	BR_Main_SeqChange( wk, eCS_Shisetu + vwk->search_mode );
		OS_Printf( " 検索モード = %d\n", vwk->search_mode );
		switch ( vwk->search_mode ){
		case 0:	BR_Main_SeqChange( wk, eCS_Shisetu ); break;
		case 1:	BR_Main_SeqChange( wk, eCS_Pokemon ); break;
		case 2:	BR_Main_SeqChange( wk, eCS_Profile ); break;
		}
		break;
	}
	
	return FALSE;
}

// -----------------------------------------
//
//	施設
//
// -----------------------------------------
static const TL_INFO tl_info_shisetu_bclear[] = {
	{ msg_rule_no_000,		GT_BATTLE_MODE_COLOSSEUM_SINGLE_NO_REGULATION , 0 },
	{ msg_rule_no_000_01,	GT_BATTLE_MODE_COLOSSEUM_SINGLE_REGULATION, 0 },
	{ msg_rule_no_001,		GT_BATTLE_MODE_COLOSSEUM_DOUBLE_NO_REGULATION, 0 },
	{ msg_rule_no_001_01,	GT_BATTLE_MODE_COLOSSEUM_DOUBLE_REGULATION, 0 },
	{ msg_rule_no_002, 		GT_BATTLE_MODE_COLOSSEUM_MULTI, 0 },
};

static const TL_INFO tl_info_shisetu_aclear[] = {
	{ msg_rule_no_000,		GT_BATTLE_MODE_COLOSSEUM_SINGLE_NO_REGULATION , 0 },
	{ msg_rule_no_000_01,	GT_BATTLE_MODE_COLOSSEUM_SINGLE_REGULATION, 0 },
	{ msg_rule_no_001,		GT_BATTLE_MODE_COLOSSEUM_DOUBLE_NO_REGULATION, 0 },
	{ msg_rule_no_001_01,	GT_BATTLE_MODE_COLOSSEUM_DOUBLE_REGULATION, 0 },
	{ msg_rule_no_002, 		GT_BATTLE_MODE_COLOSSEUM_MULTI, 0 },
	{ msg_rule_no_003, 		GT_BATTLE_MODE_TOWER_SINGLE, 0 },
	{ msg_rule_no_004, 		GT_BATTLE_MODE_TOWER_DOUBLE, 0 },
	{ msg_rule_no_005, 		GT_BATTLE_MODE_TOWER_MULTI, 0 },
	{ msg_rule_no_006, 		GT_BATTLE_MODE_FACTORY50_SINGLE, 0 },
	{ msg_rule_no_007,		GT_BATTLE_MODE_FACTORY50_DOUBLE, 0 },
	{ msg_rule_no_008, 		GT_BATTLE_MODE_FACTORY50_MULTI, 0 },
	{ msg_rule_no_009, 		GT_BATTLE_MODE_FACTORY100_SINGLE, 0 },
	{ msg_rule_no_010, 		GT_BATTLE_MODE_FACTORY100_DOUBLE, 0 },
	{ msg_rule_no_011, 		GT_BATTLE_MODE_FACTORY100_MULTI, 0 },
	{ msg_rule_no_012, 		GT_BATTLE_MODE_STAGE_SINGLE, 0 },
	{ msg_rule_no_013, 		GT_BATTLE_MODE_STAGE_DOUBLE, 0 },
	{ msg_rule_no_014, 		GT_BATTLE_MODE_STAGE_MULTI, 0 },
	{ msg_rule_no_015, 		GT_BATTLE_MODE_CASTLE_SINGLE, 0 },
	{ msg_rule_no_016, 		GT_BATTLE_MODE_CASTLE_DOUBLE, 0 },
	{ msg_rule_no_017, 		GT_BATTLE_MODE_CASTLE_MULTI, 0 },
	{ msg_rule_no_018, 		GT_BATTLE_MODE_ROULETTE_SINGLE, 0 },
	{ msg_rule_no_019, 		GT_BATTLE_MODE_ROULETTE_DOUBLE, 0 },
	{ msg_rule_no_020, 		GT_BATTLE_MODE_ROULETTE_MULTI, 0 },
};

static const RECT_HIT_TBL hit_tbl_shisetu[] = {
	{ DTC(  3 ), DTC(  7 ), DTC( 6 ), DTC( 26 ) },
	{ DTC(  7 ), DTC( 11 ), DTC( 6 ), DTC( 26 ) },
	{ DTC( 11 ), DTC( 15 ), DTC( 6 ), DTC( 26 ) },
};

static const TOUCH_LIST_HEADER tl_head_shisetu_before = {
	tl_info_shisetu_bclear,
	NELEMS( tl_info_shisetu_bclear ),
	2,					///< 2行幅
	
	7,3,
	18,12,
	32,
	GF_BGL_FRAME2_S,
	
	3,					///< 表示件数
	hit_tbl_shisetu,	///< 当たり判定
};

static const TOUCH_LIST_HEADER tl_head_shisetu_after = {
	tl_info_shisetu_aclear,
	NELEMS( tl_info_shisetu_aclear ),
	2,					///< 2行幅
	
	7,3,
	18,12,
	32,
	GF_BGL_FRAME2_S,
	
	3,					///< 表示件数
	hit_tbl_shisetu,
};

static BOOL CS_Shisetu( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
		Tool_InfoMessageMainSet( wk, msg_710 );
		vwk->battle_mode = GT_BATTLE_REC_SEARCH_BATTLE_MODE_NONE;
		BRV_Word_Shisetu( wk, vwk->battle_mode );
		ButtonOAM_PosSetSub( wk, 1 );
		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_find_d1_NSCR, GF_BGL_FRAME3_S );
		wk->sub_seq++;
		break;
	
	case 1:
		BR_PaletteFade( &vwk->color, eFADE_MODE_IN );
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_IN, ePLANE_SUB ) ){
			CATS_SystemActiveSet( wk, TRUE );
			Slider_Init( &wk->sys.slider, wk );										///< スライダーの初期化
			TouchList_WorkClear( &wk->sys.touch_list );
			if ( BR_IsGameClear( wk ) == TRUE ){
				TouchList_Init( &wk->sys.touch_list, wk, &tl_head_shisetu_after );	///< リストの初期化
			}
			else {
				TouchList_Init( &wk->sys.touch_list, wk, &tl_head_shisetu_before );	///< リストの初期化
			}
			Slider_AnimeCheck( &wk->sys.slider, &wk->sys.touch_list );			///< アニメチェック
			TouchList_CursorAdd( &wk->sys.touch_list, wk, CATS_D_AREA_SUB );		///< リストのカーソル登録
			CATS_SystemActiveSet( wk, FALSE );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );		
		break;
	
	case 2:
		Slider_HitCheck_Main( &wk->sys.slider );		
		TouchList_ValueSet( &wk->sys.touch_list, wk->sys.slider.value );
		{		
			int param = TouchList_Main( &wk->sys.touch_list, wk );			
			if ( param ){
				vwk->battle_mode = param;
				BRV_Word_Shisetu( wk, param );
				wk->sub_seq++;
			}
		}
		if ( GF_TP_SingleHitTrg( &hit_table_back[ 0 ] ) ){
			TouchAction( wk );
			wk->sub_seq++;
		}
		break;
	
	case 3:
		Slider_Exit( &wk->sys.slider );					///< スライダーの破棄
		TouchList_Exit( &wk->sys.touch_list );			///< リストの破棄
		TouchList_CursorDel( &wk->sys.touch_list, wk );	///< リストのカーソル破棄

		///< 値の保持はしなくてよい
		Slider_WorkClear( &wk->sys.slider );			
		TouchList_WorkClear( &wk->sys.touch_list );
		wk->sub_seq++;
	
	case 4:
		BR_PaletteFade( &vwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_OUT, ePLANE_SUB ) ){
			ButtonOAM_PosSetSub( wk, 0 );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );	
		break;

	default:
		BR_Main_SeqChange( wk, eCS_SearchCommonExit );
		break;
	}
	
	return FALSE;
}

// -----------------------------------------
//
//	ポケモン指定
//
// -----------------------------------------
static const RECT_HIT_TBL hit_tbl_pokemon[] = {
	{ DTC(  3 ), DTC(  5 ), DTC( 7 ), DTC( 24 ) },
	{ DTC(  5 ), DTC(  7 ), DTC( 7 ), DTC( 24 ) },
	{ DTC(  7 ), DTC(  9 ), DTC( 7 ), DTC( 24 ) },
	{ DTC(  9 ), DTC( 11 ), DTC( 7 ), DTC( 24 ) },
	{ DTC( 11 ), DTC( 13 ), DTC( 7 ), DTC( 24 ) },
	{ DTC( 13 ), DTC( 15 ), DTC( 7 ), DTC( 24 ) },
};

static const TOUCH_LIST_HEADER tl_head_pokemon_dmy = {
	NULL,
	0,
	1,					///< 2行幅
		
	9,3,
	14,12,
	32,
	GF_BGL_FRAME2_S,
	
	6,					///< 表示件数
	hit_tbl_pokemon,	///< 当たり判定
};
static BOOL CS_Pokemon( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;

	switch ( wk->sub_seq ){
	case 0:
		Tool_InfoMessageMainSet( wk, msg_711 );
		vwk->monsno = GT_BATTLE_REC_SEARCH_MONSNO_NONE;
		BRV_Word_Pokemon( wk, vwk->monsno );
		ButtonOAM_PosSetSub( wk, 1 );
		wk->sub_seq++;

	default:
		BR_Main_SeqChange( wk, eCS_PokemonAIUEO );
		break;
	}	
	return FALSE;
}

#ifdef NONEQUIVALENT
static void WinAdd_AIUEO( BR_WORK* wk )
{
	STRBUF* 			str1;
	GF_BGL_BMPWIN*		win;
	VIDEO_SEARCH_WORK*	vwk = wk->sub_work;
	
	{
		int i;
		int ofs = 1;
		u8  x, y;
		int	px;
		
		for ( i = 0; i < 10; i++ ){		
			win = &vwk->win_s[ i ];			
			x = hit_rect_AIUEO[ i ].rect.left / 8;
			y = hit_rect_AIUEO[ i ].rect.top  / 8;		
			GF_BGL_BmpWinInit( win );
			GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_S, x, y, 2, 2, eBG_PAL_FONT, ofs );
			GF_BGL_BmpWinDataFill( win, 0x00 );
			str1 = MSGMAN_AllocString( wk->sys.man, msg_900 + i );
			px = BR_print_x_Get( win, str1 );
			
			if ( IsPokeNameList( wk, i ) == TRUE ){
				GF_STR_PrintColor( win, FONT_SYSTEM, str1, px, 0, MSG_NO_PUT, PRINT_COL_VIDEO, NULL );
			}
			else {
				GF_STR_PrintColor( win, FONT_SYSTEM, str1, px, 0, MSG_NO_PUT, PRINT_COL_VIDEO2, NULL );
			}
			GF_BGL_BmpWinOnVReq( win );			
			STRBUF_Delete( str1 );
			ofs += 4;
		}
	}
}
#else
asm static void WinAdd_AIUEO( BR_WORK* wk )
{
    push {r4, r5, r6, r7, lr}
	sub sp, #0x34
	str r0, [sp, #0x14]
	mov r0, #1
	str r0, [sp, #0x28]
	mov r1, #0x86
	ldr r0, [sp, #0x14]
	lsl r1, r1, #4
	ldr r1, [r0, r1]
	ldr r0, =0x000006D4 // _022423C8
	ldr r5, =hit_rect_AIUEO // _022423CC
	mov r7, #0
	add r4, r1, r0
_022422DA:
	ldrb r0, [r5, #2]
	lsl r0, r0, #0x15
	lsr r0, r0, #0x18
	str r0, [sp, #0x24]
	ldrb r0, [r5]
	lsl r0, r0, #0x15
	lsr r0, r0, #0x18
	str r0, [sp, #0x20]
	ldrb r0, [r5, #3]
	lsr r1, r0, #3
	ldr r0, [sp, #0x24]
	sub r0, r1, r0
	lsl r0, r0, #0x18
	lsr r0, r0, #0x18
	str r0, [sp, #0x1c]
	ldrb r0, [r5, #1]
	lsr r1, r0, #3
	ldr r0, [sp, #0x20]
	sub r0, r1, r0
	lsl r0, r0, #0x18
	lsr r6, r0, #0x18
	add r0, r4, #0
	bl GF_BGL_BmpWinInit
	ldr r0, [sp, #0x20]
	add r1, r4, #0
	str r0, [sp]
	ldr r0, [sp, #0x1c]
	mov r2, #6
	str r0, [sp, #4]
	str r6, [sp, #8]
	mov r0, #0xe
	str r0, [sp, #0xc]
	ldr r0, [sp, #0x28]
	lsl r0, r0, #0x10
	lsr r0, r0, #0x10
	str r0, [sp, #0x10]
	ldr r0, [sp, #0x14]
	ldr r3, [sp, #0x24]
	ldr r0, [r0, #0x24]
	bl GF_BGL_BmpWinAdd
	add r0, r4, #0
	mov r1, #0
	bl GF_BGL_BmpWinDataFill
	ldr r0, [sp, #0x14]
	add r1, r7, #0
	ldr r0, [r0, #0x48]
	add r1, #0x44
	bl MSGMAN_AllocString
	str r0, [sp, #0x2c]
	ldr r1, [sp, #0x2c]
	add r0, r4, #0
	bl BR_print_x_Get
	lsl r1, r6, #3
	sub r1, #0x10
	str r0, [sp, #0x30]
	lsr r0, r1, #0x1f
	add r0, r1, r0
	asr r0, r0, #1
	str r0, [sp, #0x18]
	ldr r0, [sp, #0x14]
	add r1, r7, #0
	bl IsPokeNameList
	cmp r0, #1
	bne _02242384
	ldr r0, [sp, #0x18]
	ldr r2, [sp, #0x2c]
	str r0, [sp]
	mov r0, #0xff
	str r0, [sp, #4]
	ldr r0, =0x000F0D00 // _022423D0
	ldr r3, [sp, #0x30]
	str r0, [sp, #8]
	mov r0, #0
	str r0, [sp, #0xc]
	add r0, r4, #0
	mov r1, #0
	bl GF_STR_PrintColor
	b _022423A0
_02242384:
	ldr r0, [sp, #0x18]
	ldr r2, [sp, #0x2c]
	str r0, [sp]
	mov r0, #0xff
	str r0, [sp, #4]
	ldr r0, =0x000C0B00 // _022423D4
	ldr r3, [sp, #0x30]
	str r0, [sp, #8]
	mov r0, #0
	str r0, [sp, #0xc]
	add r0, r4, #0
	mov r1, #0
	bl GF_STR_PrintColor
_022423A0:
	add r0, r4, #0
	bl GF_BGL_BmpWinOnVReq
	ldr r0, [sp, #0x2c]
	bl STRBUF_Delete
	ldr r0, [sp, #0x1c]
	add r7, r7, #1
	add r1, r0, #0
	ldr r0, [sp, #0x28]
	mul r1, r6
	add r0, r0, r1
	str r0, [sp, #0x28]
	add r4, #0x10
	add r5, r5, #4
	cmp r7, #9
	blo _022422DA
	add sp, #0x34
	pop {r4, r5, r6, r7, pc}
	nop
// _022423C8: .4byte 0x000006D4
// _022423CC: .4byte hit_rect_AIUEO
// _022423D0: .4byte 0x000F0D00
// _022423D4: .4byte 0x000C0B00
}
#endif

static void WinDel_AIUEO( BR_WORK* wk )
{
	u32 i; // MatchComment: change to u32
	GF_BGL_BMPWIN*		win;
	VIDEO_SEARCH_WORK*	vwk = wk->sub_work;

    // MatchComment: 10 -> 9
	for ( i = 0; i < 9; i++ ){		
		win = &vwk->win_s[ i ];					
		GF_BGL_BmpWinOff( win );
		GF_BGL_BmpWinDel( win );	
	}
}

static BOOL CS_PokemonAIUEO( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;

	switch ( wk->sub_seq ){
	case 0:
		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_find_d2_NSCR, GF_BGL_FRAME3_S );
		wk->sub_seq++;
		break;

	case 1:
		BR_PaletteFade( &vwk->color, eFADE_MODE_IN );
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_IN, ePLANE_SUB ) ){
			WinAdd_AIUEO( wk );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );
		break;

	case 2:
		BMN_Main( vwk->btn_man[ 1 ] );
		if ( GF_TP_SingleHitTrg( &hit_table_back[ 0 ] ) 
		||	 vwk->poke_select.bInit == TRUE ){
			TouchAction( wk );
			wk->sub_seq++;
		}
		break;
	
	case 3:
		WinDel_AIUEO( wk );				///< あいうえお　消す
		wk->sub_seq++;
	
	case 4:
		if ( vwk->poke_select.bInit == TRUE ){
			if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_OUT, ePLANE_SUB ) ){
				wk->sub_seq++;
			}
		}
		else {				
			BR_PaletteFade( &vwk->color, eFADE_MODE_OUT );
			if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_OUT, ePLANE_SUB ) ){
				ButtonOAM_PosSetSub( wk, 0 );
				wk->sub_seq++;
			}
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );
		}
		break;

	default:
		if ( vwk->poke_select.bInit == TRUE ){
			BR_Main_SeqChange( wk, eCS_PokemonSelect );
		}
		else {
			BR_Main_SeqChange( wk, eCS_SearchCommonExit );
		}
		break;
	}	
	return FALSE;
}

static BOOL CS_PokemonSelect( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;

	switch ( wk->sub_seq ){
	case 0:
		Tool_InfoMessageMainSet( wk, msg_711 );
		ButtonOAM_PosSetSub( wk, 1 );
		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_find_d4_NSCR, GF_BGL_FRAME3_S );
		wk->sub_seq++;
		break;
	
	case 1:
	//	BR_PaletteFade( &vwk->color, eFADE_MODE_IN );
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_IN, ePLANE_SUB ) ){
			CATS_SystemActiveSet( wk, TRUE );
			Slider_Init( &wk->sys.slider, wk );															///< スライダーの初期化
			
			vwk->tl_poke_list_head = tl_head_pokemon_dmy;												///< リストヘッダーのベースはコピーする
			vwk->tl_poke_list_head.info = vwk->tl_poke_list_info;										///< infoは書き換え
			vwk->tl_poke_list_head.data_max = vwk->poke_select.poke_list_num;							///< データ個数も書き換え
			TouchList_WorkClear( &wk->sys.touch_list );
			TouchList_InitEx( &wk->sys.touch_list, wk, vwk->poke_select.man, &vwk->tl_poke_list_head );	///< リストの初期化
			wk->sys.touch_list.bView = FALSE;
			Slider_AnimeCheck( &wk->sys.slider, &wk->sys.touch_list );			///< アニメチェック
			TouchList_CursorAdd( &wk->sys.touch_list, wk, CATS_D_AREA_SUB );							///< リストのカーソル登録
			CATS_SystemActiveSet( wk, FALSE );
			wk->sub_seq++;
		}
	//	ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );		
		break;
	
	case 2:
		Slider_HitCheck_Main( &wk->sys.slider );		
		TouchList_ValueSet( &wk->sys.touch_list, wk->sys.slider.value );
		{		
			int param = TouchList_Main( &wk->sys.touch_list, wk );
			if ( param ){
				vwk->monsno = param;
				BRV_Word_Pokemon( wk, param );
				wk->sub_seq++;
			}
		}
		if ( GF_TP_SingleHitTrg( &hit_table_back[ 0 ] ) ){
			vwk->monsno = GT_BATTLE_REC_SEARCH_MONSNO_NONE;
			TouchAction( wk );
			wk->sub_seq++;
		}
		break;
	
	case 3:
		Slider_Exit( &wk->sys.slider );					///< スライダーの破棄
		TouchList_Exit( &wk->sys.touch_list );			///< リストの破棄
		TouchList_CursorDel( &wk->sys.touch_list, wk );	///< リストのカーソル破棄
		
		///< カーソル位置の記憶は必要ない
		Slider_WorkClear( &wk->sys.slider );			
		TouchList_WorkClear( &wk->sys.touch_list );
		wk->sub_seq++;
	
	case 4:
		if ( vwk->monsno != GT_BATTLE_REC_SEARCH_MONSNO_NONE ){
			BR_PaletteFade( &vwk->color, eFADE_MODE_OUT );
			if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_OUT, ePLANE_SUB ) ){
				ButtonOAM_PosSetSub( wk, 0 );
				wk->sub_seq++;
			}
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );	
		}
		else {
			if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_OUT, ePLANE_SUB ) ){
				wk->sub_seq++;
			}
		}
		break;

	default:
		PokemonNameList_DataFree( wk );
		if ( vwk->monsno != GT_BATTLE_REC_SEARCH_MONSNO_NONE ){
			BR_Main_SeqChange( wk, eCS_SearchCommonExit );
		}
		else {
			BR_Main_SeqChange( wk, eCS_PokemonAIUEO );
		}
		break;
	}
	return FALSE;
}

static BOOL IsPokeNameList( BR_WORK* wk, int word )
{
	BOOL bOK = FALSE;
	
	
	u16* pdata;
	int  num;
	int  min;
	int  max;
	int  poke_num;
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	max = NameHeadTable[ word + 1 ];
	min = NameHeadTable[ word ];
	num = max - min;	
	pdata = ZukanSortDataGet_50on( HEAPID_BR, 0, &poke_num );
	
	{
		int  i;
		int  no = 0;
		BOOL bFound;		
		ZUKAN_WORK* zw = SaveData_GetZukanWork( wk->save );

		for ( i = min; i < max; i++ ){			
			bFound = ZukanWork_GetPokeSeeFlag( zw, pdata[ i ] );			
			if ( bFound == FALSE ){
				pdata[ i ] = NOTFOUND_MONSNO;
			}
		}
		
		no = 0;		
		for ( i = min; i < max; i++ ){			
			if ( pdata[ i ] != NOTFOUND_MONSNO ){
				bOK = TRUE;
				break;
			}			
		}
		sys_FreeMemoryEz( pdata );
	}
	
	return bOK;
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 * @param	word	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void PokemonNameList_DataMake( BR_WORK* wk, int word )
{
	u16* pdata;
	int  num;
	int  min;
	int  max;
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	max = NameHeadTable[ word + 1 ];
	min = NameHeadTable[ word ];
	num = max - min;
	
	vwk->poke_select.poke_data = ZukanSortDataGet_50on( HEAPID_BR, 0, &vwk->poke_select.poke_num );
	pdata = ZukanSortDataGet_50on( HEAPID_BR, 0, &vwk->poke_select.poke_num );	
	{
		int  i;
		int  no = 0;
		BOOL bFound;		
		ZUKAN_WORK* zw = SaveData_GetZukanWork( wk->save );
		
		vwk->poke_select.poke_list_num = 0;
		for ( i = min; i < max; i++ ){			
			bFound = ZukanWork_GetPokeSeeFlag( zw, pdata[ i ] );
			
			if ( bFound == FALSE ){
				pdata[ i ] = NOTFOUND_MONSNO;
			}
			else {
				vwk->poke_select.poke_list_num++;
			}
		}
		
		no = 0;		
		for ( i = min; i < max; i++ ){			
			if ( pdata[ i ] == NOTFOUND_MONSNO ){ continue; }			
			vwk->poke_select.poke_data[ no ] = pdata[ i ];
			no++;
		}
		for ( i = no; i < vwk->poke_select.poke_num; i++ ){
			vwk->poke_select.poke_data[ i ] = pdata[ i ];
		}
		sys_FreeMemoryEz( pdata );
		
		vwk->tl_poke_list_info = sys_AllocMemory( HEAPID_BR, sizeof( TL_INFO ) * vwk->poke_select.poke_list_num );
		MI_CpuFill8( vwk->tl_poke_list_info, 0, sizeof( TL_INFO ) * vwk->poke_select.poke_list_num );
		for ( i = 0; i < vwk->poke_select.poke_list_num; i++ ){
			vwk->tl_poke_list_info[ i ].msg   = vwk->poke_select.poke_data[ i ];
			vwk->tl_poke_list_info[ i ].param = vwk->poke_select.poke_data[ i ];
		}
	}
	vwk->poke_select.man = MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG,NARC_msg_monsname_dat, HEAPID_BR );
	vwk->poke_select.bInit = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void PokemonNameList_DataFree( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;

	MSGMAN_Delete( vwk->poke_select.man );
	sys_FreeMemoryEz( vwk->tl_poke_list_info );
	sys_FreeMemoryEz( vwk->poke_select.poke_data );
	
	vwk->poke_select.bInit = FALSE;
}



// -----------------------------------------
//
//	プロフィール
//
// -----------------------------------------
static const TL_INFO tl_info_profile[] = {
	{ msg_16, 13, 0 },
};

static const RECT_HIT_TBL hit_tbl_profile[] = {
	{ DTC(  3 ), DTC(  7 ), DTC( 7 ), DTC( 25 ) },
//	{ DTC(  7 ), DTC( 11 ), DTC( 6 ), DTC( 20 ) },
//	{ DTC( 11 ), DTC( 15 ), DTC( 6 ), DTC( 20 ) },
};

static const TOUCH_LIST_HEADER tl_head_profile = {
	tl_info_profile,
	NELEMS( tl_info_profile ),
	2,					///< 2行幅
	 8, 2,
	16,12,
	32,
	GF_BGL_FRAME2_S,
	
	1,					///< 表示件数
	hit_tbl_profile,	///< 当たり判定
};

static BOOL CS_Profile( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;

	switch ( wk->sub_seq ){
	case 0:
		Tool_InfoMessageMainSet( wk, msg_712 );
		vwk->country_code = GT_BATTLE_REC_SEARCH_COUNTRY_CODE_NONE;
		vwk->local_code	  = GT_BATTLE_REC_SEARCH_LOCAL_CODE_NONE;
		BRV_Word_Profile( wk, vwk->country_code, vwk->local_code );
		ButtonOAM_PosSetSub( wk, 1 );
		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_find_d5_NSCR, GF_BGL_FRAME3_S );
		wk->sub_seq++;
		break;
	
	case 1:
		BR_PaletteFade( &vwk->color, eFADE_MODE_IN );
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_IN, ePLANE_SUB ) ){
			CATS_SystemActiveSet( wk, TRUE );
			Slider_Init( &wk->sys.slider, wk );										///< スライダーの初期化			
			TouchList_Init( &wk->sys.touch_list, wk, &tl_head_profile );			///< リストの初期化
			Slider_AnimeCheck( &wk->sys.slider, &wk->sys.touch_list );			///< アニメチェック
			TouchList_CursorAdd( &wk->sys.touch_list, wk, CATS_D_AREA_SUB );		///< リストのカーソル登録
			CATS_SystemActiveSet( wk, FALSE );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );		
		break;
	
	case 2:
		Slider_HitCheck_Main( &wk->sys.slider );
		TouchList_ValueSet( &wk->sys.touch_list, wk->sys.slider.value );
		{		
			int param = TouchList_Main( &wk->sys.touch_list, wk );	
			if ( param ){
				vwk->country_code = GDS_Profile_GetNation( wk->br_gpp[ 0 ] );
				vwk->local_code	  = GDS_Profile_GetArea( wk->br_gpp[ 0 ] );
				BRV_Word_Profile( wk, vwk->country_code, vwk->local_code );
				wk->sub_seq++;
			}
		}
		if ( GF_TP_SingleHitTrg( &hit_table_back[ 0 ] ) ){
			TouchAction( wk );
			wk->sub_seq++;
		}
		break;
	
	case 3:
		Slider_Exit( &wk->sys.slider );					///< スライダーの破棄
		TouchList_Exit( &wk->sys.touch_list );			///< リストの破棄
		TouchList_CursorDel( &wk->sys.touch_list, wk );	///< リストのカーソル破棄
		wk->sub_seq++;
	
	case 4:
		BR_PaletteFade( &vwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_OUT, ePLANE_SUB ) ){
			ButtonOAM_PosSetSub( wk, 0 );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );	
		break;

	default:
		BR_Main_SeqChange( wk, eCS_SearchCommonExit );
		break;
	}
	
	return FALSE;
}

static BOOL CS_SearchCommonExit( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;

	switch ( wk->sub_seq ){
	case 0:
		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_find_u_NSCR,  GF_BGL_FRAME3_M );
		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_find_d3_NSCR, GF_BGL_FRAME3_S );
		wk->sub_seq++;
		break;
	
	case 1:
		BR_PaletteFade( &vwk->color, eFADE_MODE_IN );
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_IN, ePLANE_SUB ) ){
			Tool_InfoMessageMainSet( wk, msg_721 );	///< メッセージとの兼ね合いがアル
			BRV_WinAdd_Sub( wk, 0 );				///< カテゴリ出す
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );
		break;

	default:
		BR_Main_SeqChange( wk, eCS_TopViewMain );
		break;
	}	
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL CS_Search_Main( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	GDS_RAP_ERROR_INFO *error_info;
		
	if ( BR_GDSRAP_MoveStatusAllCheck( wk ) == FALSE ){ return FALSE; }

	switch ( wk->sub_seq ){
	case 0:
		///< メッセージ
		Tool_InfoMessageMainSet( wk, msg_714 );
		Snd_SePlay( eBR_SND_SEARCH );
		wk->sub_seq++;
		break;
	
	case 1:
		///< 検索条件を送信
		{
			int battle_mode = vwk->battle_mode;
			
			if ( battle_mode == GT_BATTLE_REC_SEARCH_BATTLE_MODE_NONE ){			
				if ( BR_IsGameClear( wk ) == TRUE ){
					battle_mode = GT_BATTLE_REC_SEARCH_BATTLE_MODE_NONE;
				}
				else {
					battle_mode = GT_BATTLE_MODE_EXCLUSION_FRONTIER;
				}
			}
			if ( GDSRAP_Tool_Send_BattleVideoSearchDownload( BR_GDSRAP_WorkGet( wk ), vwk->monsno, battle_mode, vwk->country_code, vwk->local_code ) == TRUE ){
				wk->sub_seq++;
			}
			OS_Printf( " ----- 検索条件 -----\n" );
			OS_Printf( " monsno       = %d\n", vwk->monsno );
			OS_Printf( " battle_mode  = %d\n", vwk->battle_mode );
			OS_Printf( " country_code = %d\n", vwk->country_code );
			OS_Printf( " local_code   = %d\n", vwk->local_code );
			OS_Printf( " ----- 　　　　 -----\n" );
		}
		break;
	
	case 2:
		if( GDSRAP_ErrorInfoGet( BR_GDSRAP_WorkGet( wk ), &error_info ) == TRUE ){			
			Snd_SeStopBySeqNo( eBR_SND_SEARCH, 0 );
			BR_ErrorMsgGet( wk, eERR_VIDEO_SEARCH, error_info->result, error_info->type );
			wk->recv_data.outline_data_num = 0;		///< データが見つからなかったら０個にする。()
			wk->sub_seq++;
		}
		else {
			Snd_SeStopBySeqNo( eBR_SND_SEARCH, 0 );
			wk->sys.comm_err_wk.err_msg_id = msg_715;
			wk->sub_seq++;
		}
		break;
	
	case 3:
		if ( wk->recv_data.outline_data_num == 0 ){
			Tool_InfoMessageMainSet( wk, wk->sys.comm_err_wk.err_msg_id );
			Snd_SePlay( eBR_SND_NG );
		}
		else {
			Snd_SePlay( eBR_SND_RECV_OK );
		}			
		wk->sub_seq++;
		break;
	
	default:
//		if ( !( sys.trg || GF_TP_GetTrg() ) ){ break; }
		TouchList_WorkClear( &wk->sys.touch_list );
		if ( wk->recv_data.outline_data_num == 0 ){
//			Tool_InfoMessageMainSet( wk, msg_721 );
			BR_Main_SeqChange( wk, eCS_TopViewMain );		///< 検索に戻る
		}
		else {
			BR_Main_SeqChange( wk, eCS_SearchExit );
		}
		break;
	}
	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	検索画面終了
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static const TOUCH_LIST_HEADER tl_head_video_list_dmy = {
	tl_info_shisetu_aclear,
	6,
	3,					///< 2行幅

	2,8,
	10,15,
	128,
	GF_BGL_FRAME2_M,

	5,					///< 表示件数
	NULL,				///< 当たり判定
};

static BOOL CS_Search_Exit( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
		BRV_WinDel_Main( wk );
		BRV_WinDel_Sub( wk );
		Tool_InfoMessageMainDel( wk );	
		BMN_Delete( vwk->btn_man[ 0 ] );
		BMN_Delete( vwk->btn_man[ 1 ] );
		wk->sub_seq++;
		break;
	
	case 1:
		///< フェードアウト
		BR_PaletteFade( &vwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){			
			ButtonOAM_PosSetSub( wk, 1 );			///< OAMボタン1個
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );
		break;

	default:		
		BR_Main_SeqChange( wk, eCS_ListInit );
		break;
	}
	
	return FALSE;
}

static BOOL CS_ListInit( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
		///< データ一覧の準備
		if ( BattleRec_DataExistCheck() ){	///< 読み込んだ録画データはいったん破棄
			BattleRec_Exit();
		}
		vwk->tl_poke_list_head = tl_head_video_list_dmy;								///< リストヘッダーのベースはコピーする
		vwk->tl_poke_list_head.info = NULL;												///< infoは書き換え
		vwk->tl_poke_list_head.data_max = wk->recv_data.outline_data_num;				///< データ個数も書き換え
		PokemonIcon_ResLoad( wk );														///< ポケモンアイコンのリソース読み込み
		vwk->icon.handle = PokemonGraphic_ArcOpen( HEAPID_BR );							///< アイコンのアークを開く
		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_video_u_NSCR,  GF_BGL_FRAME3_M );
		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_video_d1_NSCR, GF_BGL_FRAME3_S );
		
		///< 先読み非表示
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );	
		BRV_WinAdd_VideoList( wk );
		CATS_SystemActiveSet( wk, TRUE );
		Slider_Init( &wk->sys.slider, wk );											///< スライダーの初期化			
		TouchList_InitEx( &wk->sys.touch_list, wk, NULL, &vwk->tl_poke_list_head );	///< リストの初期化
		wk->sys.touch_list.bView = TRUE;
		Slider_AnimeCheck( &wk->sys.slider, &wk->sys.touch_list );			///< アニメチェック
		CATS_SystemActiveSet( wk, FALSE );

		CATS_SystemActiveSet( wk, TRUE );
		TouchList_CursorAdd( &wk->sys.touch_list, wk, CATS_D_AREA_MAIN );			///< リストのカーソル登録
		TouchList_CursorPosSet( &wk->sys.touch_list, 64, 184 );						///< カーソルの位置変更
		TouchList_ViewMode_Main( &wk->sys.touch_list, wk );							///< リスト更新を呼ぶ
		CATS_SystemActiveSet( wk, FALSE );
		
		CATS_SystemActiveSet( wk, TRUE );
		ListPokeIcon_Add( wk );														///< ポケモンアイコン登録
		ListPokeIcon_Enable( wk, FALSE );
		CATS_SystemActiveSet( wk, FALSE );
		wk->sub_seq++;
		break;
	
	case 1:
		///< 画面の表示
		BR_PaletteFade( &vwk->color, eFADE_MODE_IN );
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_IN, ePLANE_ALL ) ){
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );	
			ListPokeIcon_Enable( wk, TRUE );
			HormingCursor_Init( wk, wk->cur_wk, 16, 76 + ( wk->sys.touch_list.view.l_pos * 24 ) );
			Cursor_Visible( wk->cur_wk, TRUE );
			Cursor_R_Set( wk->cur_wk, LIST_HORMING_R, LIST_HORMING_R );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,  LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );
		ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, vwk->color, wk->sys.logo_color );		///< 上下カーソルがタグパレットなので
		break;
	
	default:
		BR_Main_SeqChange( wk, eCS_ListMain );
		break;
	}	
		
	return FALSE;
}

static BOOL CS_ListMain( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
		///< リスト処理
		Slider_HitCheck_Main( &wk->sys.slider );
		TouchList_ViewMode_ValueSet( &wk->sys.touch_list, wk->sys.slider.value );
		TouchList_ViewMode_Main( &wk->sys.touch_list, wk );
		Cursor_PosSet_Check( wk->cur_wk, 16, 76 + ( wk->sys.touch_list.view.l_pos * 24 ) );
		ListPokeIcon_ReAdd( wk );														///< ポケモンアイコン再登録

		if ( GF_TP_SingleHitTrg( &hit_table_view[ 0 ] ) ){	///< ビデオを見る
			TouchAction( wk );
			vwk->tmp_seq = eCS_ProfIn;
			wk->sub_seq++;
		}
		if ( GF_TP_SingleHitTrg( &hit_table_back[ 0 ] ) ){	///< 終わる
			TouchAction( wk );
			vwk->tmp_seq = eCS_TopViewRecover;
			wk->sub_seq++;
		}
#ifdef BR_AUTO_SERCH
	if ( g_debug_auto ){
		TouchAction( wk );
		vwk->tmp_seq = eCS_TopViewRecover;
		wk->sub_seq++;
	}
#endif
		break;
		
	case 1:
		///< 終了処理
		Slider_Exit( &wk->sys.slider );					///< スライダーの破棄
		TouchList_Exit( &wk->sys.touch_list );			///< リストの破棄
		TouchList_CursorDel( &wk->sys.touch_list, wk );	///< リストのカーソル破棄
		PokemonGraphic_ArcClose( vwk->icon.handle );	///< ポケモンアイコンのアーカイブ閉じる
		ListPokeIcon_Del( wk );							///< ポケモンアイコン破棄
		PokemonIcon_ResFree( wk );						///< リソース解放
		BRV_WinDel_Main( wk );
		BRV_WinDel_Sub( wk );
		Cursor_Visible( wk->cur_wk, FALSE );
		Cursor_R_Set( wk->cur_wk, 0, 0 );

		if ( vwk->tmp_seq == eCS_TopViewRecover ){		///< リストの位置は記憶しなくていい
			Slider_WorkClear( &wk->sys.slider );
			TouchList_WorkClear( &wk->sys.touch_list );
			vwk->icon.view_start = 0;
		}
		wk->sub_seq++;
	
	case 2:
		BR_PaletteFade( &vwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
			ButtonOAM_PosSetSub( wk, 0 );
			if ( vwk->tmp_seq == eCS_ProfIn ){	///< 文字を書き換える
				FontOam_MsgSet( &vwk->obtn[ 0 ], &wk->sys, msg_05 );
				FontOam_MsgSet( &vwk->obtn[ 1 ], &wk->sys, msg_602 );
				CATS_ObjectAnimeSeqSetCap( vwk->obtn[ 0 ].cap, eTAG_EX_BACK );
				CATS_ObjectAnimeSeqSetCap( vwk->obtn[ 1 ].cap, eTAG_EX_SAVE );	
			}
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );	
		ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, vwk->color, wk->sys.logo_color );		///< 上下カーソルがタグパレットなので
		break;
	
	default:
		BR_Main_SeqChange( wk, vwk->tmp_seq );
		break;
	}	
		
	return FALSE;
}

static BOOL CS_TopViewRecover( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_find_u_NSCR,  GF_BGL_FRAME3_M );
		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_find_d3_NSCR, GF_BGL_FRAME3_S );
		ButtonOAM_PosSetSub( wk, 0 );
		vwk->btn_man[ 0 ] = BMN_Create( hit_table_100,  NELEMS( hit_table_100 ),  Btn_CallBack_100,   wk, HEAPID_BR );
		vwk->btn_man[ 1 ] = BMN_Create( hit_rect_AIUEO, NELEMS( hit_rect_AIUEO ), Btn_CallBack_AIUEO, wk, HEAPID_BR );
		
		///< 先読み非表示
		BRV_WinAdd_Sub( wk, 0 );				///< カテゴリ出す
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );	
		wk->sub_seq++;
		break;

	case 1:
		BR_PaletteFade( &vwk->color, eFADE_MODE_IN );
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_IN, ePLANE_ALL ) ){
			Tool_InfoMessageMain( wk, msg_721 );	///< メッセージとの兼ね合いがアル
			BRV_WinAdd_Main( wk, 0 );				///< カテゴリ出す
			BRV_Word_Pokemon( wk, vwk->monsno );
			BRV_Word_Shisetu( wk, vwk->battle_mode );
			BRV_Word_Profile( wk, vwk->country_code, vwk->local_code );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );
		break;
			
	default:
		BR_Main_SeqChange( wk, eCS_TopViewMain );
		break;
	}
	return FALSE;
}

static BOOL CS_ProfIn( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;

	switch ( wk->sub_seq ){
	case 0:
//		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_bg1ua_data_NSCR, GF_BGL_FRAME3_M );
		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_video_d2_NSCR,   GF_BGL_FRAME3_S );
		wk->sub_seq++;
		break;
	
	case 1:
		Video_GppDataCreate_from_OutLine( wk );
		vwk->btn_man[ 0 ] = BMN_Create( hit_table_000, NELEMS( hit_table_000 ), Btn_CallBack_200, wk, HEAPID_BR );
		vwk->btn_man[ 1 ] = BMN_Create( hit_table_001, NELEMS( hit_table_001 ), Btn_CallBack_201, wk, HEAPID_BR );
		vwk->btn_man[ 2 ] = BMN_Create( hit_table_002, NELEMS( hit_table_002 ), Btn_CallBack_202, wk, HEAPID_BR );
		wk->sub_seq++;
		break;

	case 2:
		vwk->view_page = 0;
		CATS_SystemActiveSet( wk, TRUE );
		VideoDataMake( &vwk->vtr_work, wk );
		BR_ScrSet( wk, ProfileBGGet( &vwk->vtr_work ), GF_BGL_FRAME3_M );
		VideoData_IconEnable( &vwk->vtr_work, FALSE );
		CATS_SystemActiveSet( wk, FALSE );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG3, VISIBLE_ON );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
		wk->sub_seq++;
		break;

	case 3:
		BR_PaletteFade( &vwk->color, eFADE_MODE_IN );
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_IN, ePLANE_ALL ) ){
			VideoData_IconEnable( &vwk->vtr_work, TRUE );
			ProfWin_Add( wk );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );
		break;

	default:
		BR_Main_SeqChange( wk, eCS_ProfMain );
		break;
	}
	
	return FALSE;
}

///< プロフィールメイン
static BOOL CS_ProfMain( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;

	BMN_Main( vwk->btn_man[ 0 ] );
	
	return FALSE;
}

///< プロフィール切り替え
static BOOL CS_ProfChange( BR_WORK* wk )
{	
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	if ( Common_ProfChange( wk ) ){
		BR_Main_SeqChange( wk, eCS_ProfMain );
	}
	
	return FALSE;
}

///< 録画再生シーン
static BOOL CS_ProfPlay( BR_WORK* wk )
{	
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	if ( Common_ProfPlay( wk ) ){		
		sys_FreeMemoryEz( vwk );
		OS_Printf( " 終了～録画再生\n" );
		return TRUE;
	}
	return FALSE;
}

///< プロフィールからリストへ
static BOOL CS_ProfExit( BR_WORK* wk )
{	
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	switch ( wk->sub_seq ){
	case 0:
		BMN_Delete( vwk->btn_man[ 0 ] );
		BMN_Delete( vwk->btn_man[ 1 ] );
		BMN_Delete( vwk->btn_man[ 2 ] );
		GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		Video_GppDataDelete( wk );
		
		CATS_SystemActiveSet( wk, TRUE );
		if ( vwk->view_page == 0 ){
			VideoDataFree( &vwk->vtr_work, wk );
		}
		else {			
			GppDataFree( &vwk->gpp_work, wk );
		}
		CATS_SystemActiveSet( wk, FALSE );
		ProfWin_Del( wk );
		wk->sub_seq++;
		break;

	case 1:	
		BR_PaletteFade( &vwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
		//	FontButton_DeleteSub( wk );				
			FontOam_MsgSet( &vwk->obtn[ 0 ], &wk->sys, msg_05 );
			FontOam_MsgSet( &vwk->obtn[ 1 ], &wk->sys, msg_708 );
			CATS_ObjectAnimeSeqSetCap( vwk->obtn[ 0 ].cap, eTAG_EX_BACK );
			CATS_ObjectAnimeSeqSetCap( vwk->obtn[ 1 ].cap, eTAG_EX_OK );		
			ButtonOAM_PosSetSub( wk, 1 );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );
		break;
	
	default:
		BR_Main_SeqChange( wk, eCS_ListInit );
		break;
	}
	
	return FALSE;
}

///< セーブ
static BOOL CS_SaveIn( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	if ( BattleVideo_Preserved( wk, RecHeader_ParamGet( BattleRec_HeaderPtrGet(), RECHEAD_IDX_DATA_NUMBER, 0 ) ) ){			
		switch ( wk->sub_seq ){
		case 0:
			CATS_SystemActiveSet( wk, TRUE );
			if ( vwk->view_page == 0 ){
				///< 概要削除 プロフィール作成
				VideoDataFree( &vwk->vtr_work, wk );
			}
			else {
				///< プロフィール削除 概要作成
				GppDataFree( &vwk->gpp_work, wk );						
			}
			ProfWin_Del( wk );
			CATS_SystemActiveSet( wk, FALSE );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
			GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
			wk->sub_seq++;
			break;
		
		case 1:
			BR_PaletteFade( &vwk->color, eFADE_MODE_OUT );
			if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
				CATS_SystemActiveSet( wk, TRUE );
				FontButton_DeleteSub( wk );
				SaveFontButton_Create( wk );
				CATS_SystemActiveSet( wk, FALSE );
				GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
				GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_S );
				wk->sub_seq++;
			}
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );
			break;
		
		case 2:		
			Tool_InfoMessageMain( wk, msg_info_008 );			
			wk->sub_seq++;
			break;
		
		case 3:
			if ( !GF_TP_GetTrg() ){ break; }
			SaveFontButton_Delete( wk );
			Tool_InfoMessageMainDel( wk );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
			GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
			wk->sub_seq++;
			break;
		
		
		case 4:
			CATS_SystemActiveSet( wk, TRUE );
			FontButton_CreateSub( wk );
			if ( wk->ex_param1 == BR_EX_DATA_KUWASIKU ){	
				///< この時点だとボタンのタグ情報が変わってるので入れ替える
				FontOam_MsgSet( &vwk->obtn[ 0 ], &wk->sys, msg_05 );
				FontOam_MsgSet( &vwk->obtn[ 1 ], &wk->sys, msg_602 );
				CATS_ObjectAnimeSeqSetCap( vwk->obtn[ 0 ].cap, eTAG_EX_BACK );
				CATS_ObjectAnimeSeqSetCap( vwk->obtn[ 1 ].cap, eTAG_EX_SAVE );
			}
			CATS_SystemActiveSet( wk, FALSE );
			ProfWin_Add( wk );
			ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR, wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
			ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR, wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
			CATS_SystemActiveSet( wk, TRUE );
			if ( vwk->view_page == 0 ){
				///< プロフィール削除 概要作成
				VideoDataMake( &vwk->vtr_work, wk );
				VideoData_IconEnable( &vwk->vtr_work, FALSE );
			//	ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_bg1ua_data_NSCR, wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
				BR_ScrSet( wk, ProfileBGGet( &vwk->vtr_work ), GF_BGL_FRAME3_M );
			}
			else {
				///< プロフィール作成
				GppDataMake( &vwk->gpp_work, wk );
				GppData_IconEnable( &vwk->gpp_work, FALSE );
				ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_bg1uc_data_NSCR, wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
			}
			CATS_SystemActiveSet( wk, FALSE );
			ArcUtil_HDL_ScrnSet( wk->sys.p_handle,	 NARC_batt_rec_gra_batt_rec_video_d2_NSCR, 	 wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
			wk->sub_seq++;
			break;
		
		case 5:
			BR_PaletteFade( &vwk->color, eFADE_MODE_IN );
			if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_IN, ePLANE_ALL ) ){
				if ( vwk->view_page == 0 ){
					VideoData_IconEnable( &vwk->vtr_work, TRUE );
				}
				else {
					GppData_IconEnable( &vwk->gpp_work, TRUE );
				}
				GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
				GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
				wk->sub_seq++;
			}
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );		
			break;

		default:	
			{
				int i;
				for ( i = 0; i < wk->recv_data.outline_data_num; i++ ){
					wk->recv_data.outline_data_tmp[ i ] = &wk->recv_data.outline_data[ i ];	
				}
			}
			BR_Main_SeqChange( wk, eCS_ProfMain );	
			break;
		}	
	}
	else {
		if ( Common_ProfSaveIn( wk ) ){
			BR_Main_SeqChange( wk, eCS_SaveMain );
		}
	}
	return FALSE;
}

///< セーブメイン
static BOOL CS_SaveMain( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	BMN_Main( vwk->btn_man[ 1 ] );

	return FALSE;
}

///< セーブメインからプロフィールへ
static BOOL CS_SaveExit( BR_WORK* wk )
{	
	if ( Common_ProfSaveExit( wk ) ){
		{
			int i;
			for ( i = 0; i < wk->recv_data.outline_data_num; i++ ){
				wk->recv_data.outline_data_tmp[ i ] = &wk->recv_data.outline_data[ i ];	
			}
		}
		BR_Main_SeqChange( wk, eCS_ProfMain );
	}
	return FALSE;
}


static BOOL CS_PlayRecover( BR_WORK* wk )
{	
	if ( Common_ProfPlayRecover( wk ) ){
		{
			int i;
			for ( i = 0; i < wk->recv_data.outline_data_num; i++ ){
				wk->recv_data.outline_data_tmp[ i ] = &wk->recv_data.outline_data[ i ];	
			}
		}
		BR_Main_SeqChange( wk, eCS_ProfMain );
	}
	return FALSE;
}

///< セーブ
static BOOL CS_CheckIn( BR_WORK* wk )
{	
	if ( Common_ProfCheckIn( wk ) ){
		BR_Main_SeqChange( wk, eCS_CheckMain );
	}
	
	return FALSE;
}

///< 保存の確認
static BOOL CS_CheckMain( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	BMN_Main( vwk->btn_man[ 2 ] );
		
	return FALSE;
}

///< 保存の確認終了
static BOOL CS_CheckExit( BR_WORK* wk )
{	
	if ( Common_ProfCheckExit( wk ) ){
		BR_Main_SeqChange( wk, eCS_SaveMain );
	}
	return FALSE;
}

///< お気に入り登録
static BOOL CS_CheckFinish( BR_WORK* wk )
{
	Common_ProfCheckFinish( wk );
	
	return FALSE;
}

///< BRSが無い場合に生成する
static BOOL CS_BrsDataGet( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	GDS_RAP_ERROR_INFO* error_info;
	
	if ( BR_GDSRAP_MoveStatusAllCheck( wk ) == FALSE ){ return FALSE; }

	switch ( wk->sub_seq ){
	case 0:
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
		if ( vwk->view_page == 0 ){
			VideoData_IconEnable( &vwk->vtr_work, FALSE );
		}
		else {
			GppData_IconEnable( &vwk->gpp_work, FALSE );
		}
		wk->sub_seq++;
		
	case 1:		
		BR_PaletteFade( &vwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );
		break;
		
	case 2:
		///< ビデオを探しています。
		Tool_InfoMessageMain( wk, msg_714 );
		HormingCursor_Init( wk, wk->cur_wk, COMM_HORMING_CUR_CX, COMM_HORMING_CUR_CY );
		Cursor_Visible( wk->cur_wk, TRUE );
		Cursor_R_Set( wk->cur_wk, COMM_HORMING_R, COMM_HORMING_R );
		vwk->bData = FALSE;
		Snd_SePlay( eBR_SND_SEARCH );
		wk->sub_seq++;
		break;
	
	case 3:
		
		if ( GDSRAP_Tool_Send_BattleVideo_DataDownload( BR_GDSRAP_WorkGet( wk ), 
			 wk->recv_data.outline_data_tmp[ wk->sys.touch_list.view.this ]->head.data_number ) == TRUE ){
			wk->sub_seq++;
		}				
		break;
		
	case 4:		
		if ( GDSRAP_ErrorInfoGet( BR_GDSRAP_WorkGet( wk ), &error_info ) == TRUE ){
			Snd_SeStopBySeqNo( eBR_SND_SEARCH, 0 );
			Tool_InfoMessageMainDel( wk );
			BR_ErrorSet( wk, eERR_VIDEO_DATA_GET, error_info->result, error_info->type );
			wk->sub_seq++;
			vwk->bData = FALSE;
		#if 0
			if ( error_info->result == POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_ERROR_ILLEGALCODE ){
				wk->sub_seq++;
				vwk->bData = FALSE;
				Tool_InfoMessageMainSet( wk, msg_715 );
			}
		#endif
		}
		else {
			Snd_SeStopBySeqNo( eBR_SND_SEARCH, 0 );
			wk->sub_seq = 0xFF;
			vwk->bData  = TRUE;
			wk->secure_addrs  = &wk->recv_data.outline_secure_flag[ wk->sys.touch_list.view.this ];
			///< その後再生するかもしれないので、テンポラリにコピー
			vwk->rec_data = wk->recv_data.rec_data_tmp;
			MI_CpuCopy8( vwk->rec_data, &wk->recv_data.rec_data, sizeof( GT_BATTLE_REC_RECV ) );
		}
		Cursor_Visible( wk->cur_wk, FALSE );
		Cursor_R_Set( wk->cur_wk, 0, 0 );
		break;

	default:
	//	vwk->bData = NULL;
		if ( vwk->bData ){
			///< ビデオが見つかった場合は、先へ進む
			Snd_SePlay( eBR_SND_RECV_OK );
			Tool_InfoMessageMainDel( wk );
			BR_Main_SeqChange( wk, vwk->tmp_seq );
			RecData_RecStopFlagSet( wk, TRUE );
		}
		else {
			///< 見つからない場合は、プロフィールへ復帰　たぶんセーブからの復帰で問題ないはず･･･
		//	if ( !GF_TP_GetTrg() ){ break; }
			Tool_InfoMessageMainDel( wk );
			BR_Main_SeqChange( wk, eCS_PlayRecover );
		}
		break;
	}

	return FALSE;
}

static BOOL CS_RecoverIn( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;

	switch ( wk->sub_seq ){
	case 0:
		PrioritySet_Common();
		Plate_AlphaInit_Default( &vwk->eva, &vwk->evb, ePLANE_ALL );
		{
			int i;
			for ( i = 0; i < wk->recv_data.outline_data_num; i++ ){
				wk->recv_data.outline_data_tmp[ i ] = &wk->recv_data.outline_data[ i ];	
			}
		}
		vwk->secure_flag = wk->recv_data.outline_secure_flag[ wk->sys.touch_list.view.this ];
		OS_Printf( "recover secure flag = %d\n", vwk->secure_flag );
		ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR,   wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
		ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR,   wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
//		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_bg1ua_data_NSCR, GF_BGL_FRAME3_M );
		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_video_d2_NSCR,   GF_BGL_FRAME3_S );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG3, VISIBLE_ON );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
		FontButton_CreateSub( wk );
		ButtonOAM_PosSetSub( wk, 0 );
		FontOam_MsgSet( &vwk->obtn[ 0 ], &wk->sys, msg_05 );
		FontOam_MsgSet( &vwk->obtn[ 1 ], &wk->sys, msg_602 );
		CATS_ObjectAnimeSeqSetCap( vwk->obtn[ 0 ].cap, eTAG_EX_BACK );
		CATS_ObjectAnimeSeqSetCap( vwk->obtn[ 1 ].cap, eTAG_EX_SAVE );
		vwk->monsno = GT_BATTLE_REC_SEARCH_MONSNO_NONE;
		vwk->battle_mode = GT_BATTLE_REC_SEARCH_BATTLE_MODE_NONE;
		vwk->country_code = GT_BATTLE_REC_SEARCH_COUNTRY_CODE_NONE;
		vwk->local_code = GT_BATTLE_REC_SEARCH_LOCAL_CODE_NONE;		
		vwk->icon.global_gra_id = GLOBAL_POKE_ICON_ID;	///< 気持ちずらす
		vwk->icon.view_start = wk->sys.touch_list.now;	///< リストのアイコンずれ対応
		wk->sub_seq++;
		break;
	
	case 1:
		Video_GppDataCreate_from_OutLine( wk );
		vwk->btn_man[ 0 ] = BMN_Create( hit_table_000, NELEMS( hit_table_000 ), Btn_CallBack_200, wk, HEAPID_BR );
		vwk->btn_man[ 1 ] = BMN_Create( hit_table_001, NELEMS( hit_table_001 ), Btn_CallBack_201, wk, HEAPID_BR );
		vwk->btn_man[ 2 ] = BMN_Create( hit_table_002, NELEMS( hit_table_002 ), Btn_CallBack_202, wk, HEAPID_BR );
		wk->sub_seq++;
		break;

	case 2:
		CATS_SystemActiveSet( wk, TRUE );
		VideoDataMake( &vwk->vtr_work, wk );
		BR_ScrSet( wk, ProfileBGGet( &vwk->vtr_work ), GF_BGL_FRAME3_M );
		VideoData_IconEnable( &vwk->vtr_work, FALSE );		
		CATS_SystemActiveSet( wk, FALSE );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG3, VISIBLE_ON );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
		wk->sub_seq++;
		break;

	case 3:
		BR_PaletteFade( &vwk->color, eFADE_MODE_IN );
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_IN, ePLANE_ALL ) ){
			VideoData_IconEnable( &vwk->vtr_work, TRUE );
			ProfWin_Add( wk );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );
		break;

	case 4:
		WIPE_SYS_Start( WIPE_PATTERN_WMS,
			    WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN,
			    WIPE_FADE_BLACK,  WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_BR );
		wk->sub_seq++;
		break;

	case 5:		
		if ( WIPE_SYS_EndCheck() == TRUE ){	
			wk->sub_seq++;
		}
		break;

	default:
		wk->tag_man.stack_s += 1;				///< proc init で Menu_Backが呼ばれ1段階下がってしまうので応急処置
		BR_RecoverWorkClear( wk );		
		BR_Main_SeqChange( wk, eCS_ProfMain );
		break;
	}
	
	return FALSE;
}

BOOL (* const BR_Search_MainTable[])( BR_WORK* wk ) = {
	Video_Init,
	Video_Fade_Out,
	
	CS_TopView_Init,
	CS_TopView_Main,
	CS_TopView_Exit,
	
	CS_SearchCommonInit,
	CS_Shisetu,
	CS_Pokemon,
	CS_PokemonAIUEO,
	CS_PokemonSelect,
	CS_Profile,
	CS_SearchCommonExit,

	CS_Search_Main,
	CS_Search_Exit,	
	CS_ListInit,
	CS_ListMain,
	CS_TopViewRecover,

	CS_ProfIn,
	CS_ProfMain,
	CS_ProfChange,
	CS_ProfPlay,
	CS_ProfExit,
	
	CS_SaveIn,
	CS_SaveMain,
	CS_SaveExit,
	
	CS_CheckIn,
	CS_CheckMain,
	CS_CheckExit,
	CS_CheckFinish,
	
	CS_BrsDataGet,
	
	CS_RecoverIn,
	
	CS_PlayRecover,
};

// ---------------------------------
//
//	コード検索
//
// ---------------------------------
BOOL (* const BR_Video_MainTable[])( BR_WORK* wk ) = {
	Video_Init,
	Video_Fade_Out,
	Video_CodeIn_Init,
	Video_CodeIn_Main,
	Video_CodeIn_Exit,
	Video_CodeIn_End,
	Video_CodeIn_DataSearch,
	Video_ProfileIn,
	Video_ProfileMain,
	Video_ProfileChange,
	Video_ProfilePlay,
	Video_ProfileExit,
	Video_ProfileSaveIn,
	Video_ProfileSaveMain,
	Video_ProfileSaveExit,
	Video_ProfileCheckIn,
	Video_ProfileCheckMain,
	Video_ProfileCheckExit,
	Video_ProfileCheckFinish,
	Video_ProfileRecoverIn,
	
	Video_PlayRecover,
};


// ---------------------------------
//
//	データ送受信
//
// ---------------------------------
BOOL (* const BR_Video_Send_MainTable[])( BR_WORK* wk ) = {
	Video_Init,
	Video_Fade_Out,
	Video_Send_Init,
	Video_Send_Main,
	Video_Send_Exit,
};


//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL Video_Init( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = sys_AllocMemory( HEAPID_BR, sizeof( VIDEO_SEARCH_WORK ) );
	
	memset( vwk, 0, sizeof( VIDEO_SEARCH_WORK ) );
	
	wk->sub_work = vwk;
	
	GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
	GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
	GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
	GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_S );
	Plate_AlphaInit_Default( &vwk->eva, &vwk->evb, ePLANE_ALL );
	
	BR_Main_SeqChange( wk, eVIDEO_Fade_Out );
	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	画面フェードアウト
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL Video_Fade_Out( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
		if ( BR_PaletteFade( &vwk->color, eFADE_MODE_OUT ) ){ wk->sub_seq++; }	
		ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, 			 vwk->color, wk->sys.logo_color );	///< main	oam
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	 LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );	///< sub	oam
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG,	 LOGO_PALETTE,			 vwk->color, wk->sys.logo_color );	///< sub	bg
		break;
	
	case 1:
		CATS_SystemActiveSet( wk, TRUE );
		NormalTag_DeleteAllOp( wk );						///< tag delete
		CATS_SystemActiveSet( wk, FALSE );
		
	//	FontButton_Create( wk );
		GF_BGL_PrioritySet( GF_BGL_FRAME0_M, 0 );
		GF_BGL_PrioritySet( GF_BGL_FRAME1_M, 3 );
		GF_BGL_PrioritySet( GF_BGL_FRAME2_M, 0 );
		GF_BGL_PrioritySet( GF_BGL_FRAME3_M, 2 );		
		GF_BGL_PrioritySet( GF_BGL_FRAME0_S, 0 );
		GF_BGL_PrioritySet( GF_BGL_FRAME1_S, 3 );
		GF_BGL_PrioritySet( GF_BGL_FRAME2_S, 1 );
		GF_BGL_PrioritySet( GF_BGL_FRAME3_S, 2 );
		wk->sub_seq++;
		break;

	default:	
		switch ( wk->ex_param1 ){
		case BR_EX_DATA_NEW30:
			///< 最新30件
			BR_ChangeDisplayVIntr( wk, DISP_3D_TO_MAIN );
			Cursor_Visible( wk->cur_wk, FALSE );
			if ( BR_RecoverCheck( wk ) == TRUE ){
				BR_Main_SeqChange( wk, eN30_RecoverIn );
			}
			else {
				BR_Main_SeqChange( wk, eN30_DownLoad );
			}
			break;

		case BR_EX_DATA_KUWASIKU:
			///< 詳しく検索する
			BR_ChangeDisplayVIntr( wk, DISP_3D_TO_MAIN );
			Cursor_Visible( wk->cur_wk, FALSE );
			if ( BR_RecoverCheck( wk ) == TRUE ){
				BR_Main_SeqChange( wk, eCS_RecoverIn );
			}
			else {
				BR_Main_SeqChange( wk, eCS_TopViewInit );
			}
			break;

		case BR_EX_DATA_NO_SEARCH:
			///< データナンバー検索
			if ( BR_RecoverCheck( wk ) == TRUE ){
				BR_Main_SeqChange( wk, eVIDEO_CodeIn_RecoveIn );
			}
			else {
				BR_Main_SeqChange( wk, eVIDEO_CodeIn_Init );
			}
			Cursor_Visible( wk->cur_wk, FALSE );
			break;

		default:
			BR_ChangeDisplayVIntr( wk, DISP_3D_TO_MAIN );
			Cursor_Visible( wk->cur_wk, FALSE );
			BR_Main_SeqChange( wk, eVIDEO_Send_Init );
			break;
		}
		break;
	}
	
	return FALSE;	
}


// =============================================================================
//
//
//	■コード入力	TAG_CODE_INPUT
//
//
// =============================================================================
//--------------------------------------------------------------
/**
 * @brief	コード入力初期化
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL Video_CodeIn_Init( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	BR_CODEIN_WORK* ci = &vwk->codein;
	
	switch ( wk->sub_seq ){
	case 0:
		{
			int block[ CODE_BLOCK_MAX ];	
			CodeIn_BlockDataMake_2_5_5( block );
			vwk->codein_param = BRCodeInput_ParamCreate( HEAPID_BR, BRCODE_MAXLEN, block, SaveData_GetConfig( wk->save ) );
			ci->sys.p_handle = wk->sys.p_handle;
			ci->sys.csp = wk->sys.csp;
			ci->sys.crp = wk->sys.crp;
			ci->sys.bgl = wk->sys.bgl;
			ci->sys.pfd = wk->sys.pfd;	
			ci->param = *(vwk->codein_param);			
			ci->br_work = wk;
			FontButton_Create( wk );
		}
		wk->sub_seq++;
	
	case 1:
		Plate_AlphaInit( &vwk->eva, &vwk->evb, DEF_MP1, DEF_MP2, DEF_SP1, DEF_SP2, ePLANE_ALL );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG3, VISIBLE_ON );
		wk->sub_seq++;
		break;
	
	case 2:
		BR_PaletteFade( &vwk->color, eFADE_MODE_IN );	
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_IN, ePLANE_ALL ) ){
			Tool_InfoMessage( wk, msg_709 );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, vwk->color, wk->sys.logo_color );
		break;
	
	default:
		BR_Main_SeqChange( wk, eVIDEO_CodeIn_Main );
		break;
	}
	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	コード入力
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL Video_CodeIn_Main( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	BR_CODEIN_WORK* ci = &vwk->codein;
	BOOL bActive;
	
//	BR_Main_SeqChange( wk, eVIDEO_CodeIn_Exit );
//	return FALSE;
	
	bActive = BRCI_pv_MainUpdate( ci );

	if ( bActive == TRUE ){
		BR_Main_SeqChange( wk, eVIDEO_CodeIn_Exit );
	}

	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	コード入力終了
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL Video_CodeIn_Exit( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	BR_CODEIN_WORK* ci = &vwk->codein;
	
	switch ( wk->sub_seq ){
	case 0:	
		///< 削除して告ぎふぇーど
	//	BRCI_pv_disp_CodeRes_Delete( ci );
		Tool_InfoMessageDel( wk );
		Plate_AlphaSetPlane( &vwk->eva, &vwk->evb, DEF_MP1, DEF_MP2, DEF_SP1, DEF_SP2, ePLANE_ALL );
		BRCodeInput_ParamDelete( vwk->codein_param );
		wk->sub_seq++;

	case 1:
		BR_PaletteFade( &vwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG3, VISIBLE_OFF );
			FontButton_Delete( wk );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, vwk->color, wk->sys.logo_color );
		break;

	default:
		if ( ci->push_ok ){
			BR_Main_SeqChange( wk, eVIDEO_CodeIn_DataSearch );
		}
		else {
			BR_Main_SeqChange( wk, eVIDEO_CodeIn_End );
		}
		break;
	}
	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	終了
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL Video_CodeIn_End( BR_WORK* wk )
{	
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
		BR_PaletteFadeIn_Init( &vwk->color );
		CATS_SystemActiveSet( wk, TRUE );
		NormalTag_RecoverAllOp( wk );
		CATS_SystemActiveSet( wk, FALSE );
		wk->sub_seq++;
		break;
	
	default:
		if ( BR_PaletteFade( &vwk->color, eFADE_MODE_IN ) ){					
			PaletteFadeClear( wk );
			BR_PaletteFadeIn_Init( &vwk->color );
			ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, 16, wk->sys.logo_color );	///< main	oam
			
			BR_Main_ProcSeqChange( wk, eMAIN_Input, TRUE, wk->proc_seq );
			BR_Main_SeqChange( wk, eINPUT_MENU_IN );		
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_S );
			sys_FreeMemoryEz( vwk );
		}
		else {
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );	///< main	oam
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG,  LOGO_PALETTE, vwk->color, wk->sys.logo_color );	///< main	oam
		}
		break;
	}
	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL Video_CodeIn_DataSearch( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	BR_CODEIN_WORK* ci = &vwk->codein;
	GDS_RAP_ERROR_INFO* error_info;
	
	switch ( wk->sub_seq ){
	case 0:
		///< ビデオを探しています。
		Tool_InfoMessage( wk, msg_714 );
		HormingCursor_Init( wk, wk->cur_wk_top, COMM_HORMING_CUR_CX, COMM_HORMING_CUR_CY );
		Cursor_Visible( wk->cur_wk_top, TRUE );
		Cursor_R_Set( wk->cur_wk_top, COMM_HORMING_R, COMM_HORMING_R );
		vwk->bData = FALSE;
		Snd_SePlay( eBR_SND_SEARCH );
		wk->sub_seq++;
		break;
	
	case 1:
		if ( BR_GDSRAP_MoveStatusAllCheck( wk ) == FALSE ){ return FALSE; }
		if ( GDSRAP_Tool_Send_BattleVideo_DataDownload( BR_GDSRAP_WorkGet( wk ), ci->data_number ) == TRUE ){
			wk->sub_seq++;
		}				
		break;
		
	case 2:
		if ( BR_GDSRAP_MoveStatusAllCheck( wk ) == FALSE ){ return FALSE; }		
		Tool_InfoMessageDel( wk );
		if ( GDSRAP_ErrorInfoGet( BR_GDSRAP_WorkGet( wk ), &error_info ) == TRUE ){
			Snd_SeStopBySeqNo( eBR_SND_SEARCH, 0 );
			G2_BlendNone();
			BR_ErrorSet( wk, eERR_VIDEO_DATA_GET, error_info->result, error_info->type );
			BR_ChangeDisplayVIntr( wk, DISP_3D_TO_MAIN );
			wk->sub_seq++;
			vwk->bData = FALSE;
		#if 0
			if ( error_info->result == POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_ERROR_ILLEGALCODE ){
				wk->sub_seq++;
				vwk->bData = FALSE;
			}
		#endif
		}
		else {
			Snd_SeStopBySeqNo( eBR_SND_SEARCH, 0 );
			wk->sub_seq = 0xFF;
			vwk->bData  = TRUE;			
			Snd_SePlay( eBR_SND_RECV_OK );
			
			wk->secure_addrs  = &wk->recv_data.rec_secure_flag;
			///< その後再生するかもしれないので、テンポラリにコピー
			vwk->rec_data = wk->recv_data.rec_data_tmp;
			MI_CpuCopy8( vwk->rec_data, &wk->recv_data.rec_data, sizeof( GT_BATTLE_REC_RECV ) );
		}
		Cursor_Visible( wk->cur_wk_top, FALSE );
		Cursor_R_Set( wk->cur_wk_top, 0, 0 );
		break;
	
	case 3:
		///< 見つからなかった場合
	//	Tool_InfoMessage( wk, msg_715 );
		BR_ChangeDisplayVIntr( wk, DISP_3D_TO_SUB );
		wk->sub_seq++;
		break;
	
	case 4:
	//	if ( !GF_TP_GetTrg() ){ break; }
	//	Tool_InfoMessageDel( wk );
		wk->sub_seq++;
		break;

	default:
	//	vwk->bData = NULL;
		if ( vwk->bData ){			
			BR_Main_SeqChange( wk, eVIDEO_CodeIn_ProfIn );
		}
		else {
			BR_Main_SeqChange( wk, eVIDEO_CodeIn_End );
		}
		break;
	}

	return FALSE;
}


// =============================================================================
//
//
//	■プロフィール
//
//
// =============================================================================
static void Video_GppDataCreate( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	vwk->gpp_work.gpp	  = GDS_Profile_AllocMemory( HEAPID_BR );						///< 確保して
	GDS_CONV_GTGDSProfile_to_GDSProfile( &vwk->rec_data->profile, vwk->gpp_work.gpp );	///< ダウンロードデータコピーして
	vwk->vtr_work.gpp	  = vwk->gpp_work.gpp;											///< ビデオデータのプロフにもＧＰＰのポインタ当てる
	
	vwk->vtr_work.br_head = BattleRec_Header_AllocMemory( HEAPID_BR );
	MI_CpuCopy8( &vwk->rec_data->head, vwk->vtr_work.br_head, sizeof( GT_BATTLE_REC_HEADER ) );
}

static void Video_GppDataCreate_from_OutLine( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	vwk->gpp_work.gpp	  = GDS_Profile_AllocMemory( HEAPID_BR );															///< 確保して
	GDS_CONV_GTGDSProfile_to_GDSProfile( &wk->recv_data.outline_data_tmp[ wk->sys.touch_list.view.this ]->profile, vwk->gpp_work.gpp );	///< ダウンロードデータコピーして
	vwk->vtr_work.gpp	  = vwk->gpp_work.gpp;																				///< ビデオデータのプロフにもＧＰＰのポインタ当てる
	
	vwk->vtr_work.br_head = BattleRec_Header_AllocMemory( HEAPID_BR );
	MI_CpuCopy8( &wk->recv_data.outline_data_tmp[ wk->sys.touch_list.view.this ]->head, vwk->vtr_work.br_head, sizeof( GT_BATTLE_REC_HEADER ) );	
}

static void Video_GppDataDelete( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	BattleRec_Header_FreeMemory( vwk->vtr_work.br_head );
	GDS_Profile_FreeMemory( vwk->gpp_work.gpp );	
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL Video_ProfileIn( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
		BR_ChangeDisplayVIntr( wk, DISP_3D_TO_MAIN );
		wk->sub_seq++;
		break;
	
	case 1:
		if ( BR_IsChangeDisplay( wk ) == FALSE ){ break; }
		Plate_AlphaSetPlane( &vwk->eva, &vwk->evb, DEF_MP1, DEF_MP2, DEF_SP1, DEF_SP2, ePLANE_ALL );
		ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR,   wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
		ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR,   wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
//		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_bg1ua_data_NSCR, GF_BGL_FRAME3_M );
		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_video_d2_NSCR,   GF_BGL_FRAME3_S );
		GF_BGL_PrioritySet( GF_BGL_FRAME0_M, 1 );			///< priority
		GF_BGL_PrioritySet( GF_BGL_FRAME1_M, 3 );
		GF_BGL_PrioritySet( GF_BGL_FRAME2_M, 0 );
		GF_BGL_PrioritySet( GF_BGL_FRAME3_M, 1 );		
		GF_BGL_PrioritySet( GF_BGL_FRAME0_S, 1 );
		GF_BGL_PrioritySet( GF_BGL_FRAME1_S, 3 );
		GF_BGL_PrioritySet( GF_BGL_FRAME2_S, 0 );
		GF_BGL_PrioritySet( GF_BGL_FRAME3_S, 2 );
		Video_GppDataCreate( wk );
		FontButton_CreateSub( wk );
		vwk->btn_man[ 0 ] = BMN_Create( hit_table_000, NELEMS( hit_table_000 ), Btn_CallBack_000, wk, HEAPID_BR );
		vwk->btn_man[ 1 ] = BMN_Create( hit_table_001, NELEMS( hit_table_001 ), Btn_CallBack_001, wk, HEAPID_BR );
		vwk->btn_man[ 2 ] = BMN_Create( hit_table_002, NELEMS( hit_table_002 ), Btn_CallBack_002, wk, HEAPID_BR );
		wk->sub_seq++;
		break;

	case 2:
		CATS_SystemActiveSet( wk, TRUE );
		vwk->view_page = 0;
		VideoDataMake( &vwk->vtr_work, wk );
		BR_ScrSet( wk, ProfileBGGet( &vwk->vtr_work ), GF_BGL_FRAME3_M );
		VideoData_IconEnable( &vwk->vtr_work, FALSE );
		CATS_SystemActiveSet( wk, FALSE );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG3, VISIBLE_ON );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
		wk->sub_seq++;
		break;

	case 3:
		BR_PaletteFade( &vwk->color, eFADE_MODE_IN );
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_IN, ePLANE_ALL ) ){
			VideoData_IconEnable( &vwk->vtr_work, TRUE );
			ProfWin_Add( wk );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );
		break;

	default:
		BR_Main_SeqChange( wk, eVIDEO_CodeIn_ProfMain );
		break;
	}
	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL Video_ProfileMain( BR_WORK* wk )
{	
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;	

	BMN_Main( vwk->btn_man[ 0 ] );
	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL Video_ProfileChange( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	if ( Common_ProfChange( wk ) ){
		BR_Main_SeqChange( wk, eVIDEO_CodeIn_ProfMain );
	}
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL Video_ProfilePlay( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	if ( Common_ProfPlay( wk ) ){		
		sys_FreeMemoryEz( vwk );
		return TRUE;
	}	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL Video_ProfileExit( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
		BMN_Delete( vwk->btn_man[ 0 ] );
		BMN_Delete( vwk->btn_man[ 1 ] );
		BMN_Delete( vwk->btn_man[ 2 ] );
		GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		Video_GppDataDelete( wk );
		CATS_SystemActiveSet( wk, TRUE );
		if ( vwk->view_page == 0 ){
			VideoDataFree( &vwk->vtr_work, wk );
		}
		else {			
			GppDataFree( &vwk->gpp_work, wk );
		}
		CATS_SystemActiveSet( wk, FALSE );
		ProfWin_Del( wk );
		wk->sub_seq++;
		break;

	case 1:	
		BR_PaletteFade( &vwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
			FontButton_DeleteSub( wk );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );
		break;

	case 2:
		BR_PaletteFadeIn_Init( &vwk->color );
		CATS_SystemActiveSet( wk, TRUE );
		NormalTag_RecoverAllOp( wk );
		CATS_SystemActiveSet( wk, FALSE );
		BR_ChangeDisplayVIntr( wk, DISP_3D_TO_SUB );				///< main = top
		wk->sub_seq++;
		break;

	case 3:
		if ( BR_IsChangeDisplay( wk ) == FALSE ){ break; }
		if ( BR_PaletteFade( &vwk->color, eFADE_MODE_IN ) ){					
			PaletteFadeClear( wk );
			BR_PaletteFadeIn_Init( &vwk->color );
			ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, 16, wk->sys.logo_color );	///< main	oam
			
			BR_Main_ProcSeqChange( wk, eMAIN_Input, TRUE, wk->proc_seq );
			BR_Main_SeqChange( wk, eINPUT_MENU_IN );		
			sys_FreeMemoryEz( vwk );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_S );
			if ( BattleRec_DataExistCheck() == TRUE ){
				BattleRec_Exit();
			}
		}
		else {
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG,	 LOGO_PALETTE, vwk->color, wk->sys.logo_color );	///< main	oam
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );	///< main	oam
		}
		break;

	default:
		break;
	}

	return FALSE;
}


static void FontOam_SaveData_Set( OAM_BUTTON* obtn, BR_WORK* wk, int no )
{
	STRBUF*				str;
	MSGDATA_MANAGER*	man;
	GF_BGL_BMPWIN		bmpwin;	
	
	WORDSET*			wset;
	STRBUF*				tmp1;
	STRBUF*				tmp2;
	
	man = wk->sys.man;
	
	if ( wk->br_gpp[ no ] == NULL ){
		str	= MSGMAN_AllocString( man, msg_10 );
	}
	else {
		wset = BR_WORDSET_Create( HEAPID_BR );
		tmp1 = MSGMAN_AllocString( wk->sys.man, msg_09 );
		tmp2 = GDS_Profile_CreateNameString( wk->br_gpp[ no ], HEAPID_BR );
		BR_ErrorStrChange( wk, tmp2 );
		str	 = STRBUF_Create( 255, HEAPID_BR );

		WORDSET_RegisterWord( wset, 0, tmp2, 0, TRUE, PM_LANG );	
		WORDSET_ExpandStr( wset, str, tmp1 );
		
		STRBUF_Delete( tmp1 );
		STRBUF_Delete( tmp2 );
		WORDSET_Delete( wset );	
	}
	
	GF_BGL_BmpWinInit( &bmpwin );
	GF_BGL_BmpWinObjAdd( wk->sys.bgl, &bmpwin, BR_TAG_BMP_WIX_SX, BR_TAG_BMP_WIX_SY, 0, 0 );
	GF_STR_PrintColor( &bmpwin, FONT_SYSTEM, str, 0, 0, MSG_NO_PUT, PRINT_COL_BLACK, NULL );
	
	FONTOAM_OAMDATA_ResetBmp( obtn->font_obj, obtn->font_data, &bmpwin, HEAPID_BR );
		
	STRBUF_Delete(str);
	GF_BGL_BmpWinDel(&bmpwin);	
}

static void FontOam_Message_Set( OAM_BUTTON* obtn, BR_WORK* wk, int no )
{
	STRBUF*				str;
	MSGDATA_MANAGER*	man;
	GF_BGL_BMPWIN		bmpwin;	
	
	WORDSET*			wset;
	STRBUF*				tmp1;
	STRBUF*				tmp2;
	
	man = wk->sys.man;
	
	str	= MSGMAN_AllocString( man, no );
	
	GF_BGL_BmpWinInit( &bmpwin );
	GF_BGL_BmpWinObjAdd( wk->sys.bgl, &bmpwin, BR_TAG_BMP_WIX_SX, BR_TAG_BMP_WIX_SY, 0, 0 );
	GF_STR_PrintColor( &bmpwin, FONT_SYSTEM, str, 0, 0, MSG_NO_PUT, PRINT_COL_BLACK, NULL );
	
	FONTOAM_OAMDATA_ResetBmp( obtn->font_obj, obtn->font_data, &bmpwin, HEAPID_BR );
		
	STRBUF_Delete(str);
	GF_BGL_BmpWinDel(&bmpwin);	
}

static void InfoMessage( BR_WORK* wk, int no, BOOL flag )
{
	STRBUF* str1;
	GF_BGL_BMPWIN* win = &wk->info_win;
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;

	BR_disp_BG_InfoWindowLoad( wk, GF_BGL_FRAME2_M, flag );
	GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
	
	GF_BGL_BmpWinInit( win );
	GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_M, 1, 19, 30, 4, eBG_PAL_FONT, 300 );

	if ( no == msg_608 ){
		
		WORDSET*	wset;
		STRBUF*		str2;
		STRBUF*		str3;
		GDS_PROFILE_PTR	gpp = vwk->gpp_work.gpp;
		
		wset = BR_WORDSET_Create( HEAPID_BR );
		str1 = STRBUF_Create( 255, HEAPID_BR );
		str2 = GDS_Profile_CreateNameString( gpp, HEAPID_BR );
		BR_ErrorStrChange( wk, str2 );
		str3 = MSGMAN_AllocString( wk->sys.man, no );

		WORDSET_RegisterWord( wset, 0, str2, 0, TRUE, PM_LANG );	
		WORDSET_ExpandStr( wset, str1, str3 );
		
		STRBUF_Delete( str2 );
		STRBUF_Delete( str3 );
		WORDSET_Delete( wset );
	}
	else {
		str1 = MSGMAN_AllocString( wk->sys.man, no );	
	}

	GF_BGL_BmpWinDataFill( win, BR_MSG_CLEAR_CODE );	
		
	GF_STR_PrintColor( win, FONT_SYSTEM, str1, 0, 0, MSG_NO_PUT, PRINT_COL_INFO, NULL );
	
	GF_BGL_BmpWinOnVReq( win );
	
	STRBUF_Delete( str1 );
}

static void InfoMessageSet( BR_WORK* wk, int no, int data_no )
{
	STRBUF* str1;
	GF_BGL_BMPWIN* win = &wk->info_win;
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;

	if ( no == msg_608 ){
		
		WORDSET*	wset;
		STRBUF*		str2;
		STRBUF*		str3;
		GDS_PROFILE_PTR	gpp = vwk->gpp_work.gpp;
		
		wset = BR_WORDSET_Create( HEAPID_BR );
		str1 = STRBUF_Create( 255, HEAPID_BR );
		str2 = GDS_Profile_CreateNameString( gpp, HEAPID_BR );
		BR_ErrorStrChange( wk, str2 );
		str3 = MSGMAN_AllocString( wk->sys.man, no );

		WORDSET_RegisterWord( wset, 0, str2, 0, TRUE, PM_LANG );	
		WORDSET_ExpandStr( wset, str1, str3 );
		
		STRBUF_Delete( str2 );
		STRBUF_Delete( str3 );
		WORDSET_Delete( wset );
	}
	else if ( no == msg_610 ){
		
		WORDSET*	wset;
		STRBUF*		str2;
		STRBUF*		str3;
		GDS_PROFILE_PTR	gpp = wk->br_gpp[ data_no ];
		
		wset = BR_WORDSET_Create( HEAPID_BR );
		str1 = STRBUF_Create( 255, HEAPID_BR );
		str2 = GDS_Profile_CreateNameString( gpp, HEAPID_BR );
		BR_ErrorStrChange( wk, str2 );
		str3 = MSGMAN_AllocString( wk->sys.man, no );

		WORDSET_RegisterWord( wset, 0, str2, 0, TRUE, PM_LANG );	
		WORDSET_ExpandStr( wset, str1, str3 );
		
		STRBUF_Delete( str2 );
		STRBUF_Delete( str3 );
		WORDSET_Delete( wset );
	}
	else {
		str1 = MSGMAN_AllocString( wk->sys.man, no );	
	}

	GF_BGL_BmpWinDataFill( win, BR_MSG_CLEAR_CODE );	
		
	GF_STR_PrintColor( win, FONT_SYSTEM, str1, 0, 0, MSG_NO_PUT, PRINT_COL_INFO, NULL );
	
	GF_BGL_BmpWinOnVReq( win );
	
	STRBUF_Delete( str1 );
}

static void SaveFontButton_Create( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	int i;
	int mes[] = { msg_10, msg_10, msg_10, msg_05, };
	int anime[] = { eTAG_GDS_SAVE_DATA, eTAG_GDS_SAVE_DATA, eTAG_GDS_SAVE_DATA, eTAG_GDS_SAVE_BACK };

	ExTag_ResourceLoad_Save( wk, NNS_G2D_VRAM_TYPE_2DSUB );		
	
	for ( i = 0; i < 4; i++ ){		
		vwk->obtn[ i ].cap = ExTag_Add( wk, NNS_G2D_VRAM_TYPE_2DSUB );
		FontOam_Create( &vwk->obtn[ i ], &wk->sys, NNS_G2D_VRAM_TYPE_2DSUB );
		FontOam_MsgSet( &vwk->obtn[ i ], &wk->sys, mes[ i ] );
		if ( i != 3 ){
			FontOam_SaveData_Set( &vwk->obtn[ i ], wk, i + 1 );
		}
		CATS_ObjectPosSetCap( vwk->obtn[ i ].cap,  TAG_MAIN_POS + 8, TAG_INIT_POS( i, 4 ) + 64 );
		CATS_ObjectAnimeSeqSetCap( vwk->obtn[ i ].cap, anime[ i ] );
		FONTOAM_SetMat( vwk->obtn[ i ].font_obj, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY );
		FONTOAM_SetDrawFlag( vwk->obtn[ i ].font_obj, TRUE );
	}
}

static void SaveFontButton_Delete( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	int i;
	
	for ( i = 0; i < 4; i++ ){
		FontOam_Delete( &vwk->obtn[ i ] );
		CATS_ActorPointerDelete_S( vwk->obtn[ i ].cap );
	}
	
	ExTag_ResourceDelete( wk );
}

static BOOL Video_ProfileSaveIn( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	if ( BattleVideo_Preserved( wk, RecHeader_ParamGet( BattleRec_HeaderPtrGet(), RECHEAD_IDX_DATA_NUMBER, 0 ) ) ){
		switch ( wk->sub_seq ){
		case 0:
			CATS_SystemActiveSet( wk, TRUE );
			if ( vwk->view_page == 0 ){
				///< 概要削除 プロフィール作成
				VideoDataFree( &vwk->vtr_work, wk );
			}
			else {
				///< プロフィール削除 概要作成
				GppDataFree( &vwk->gpp_work, wk );						
			}
			CATS_SystemActiveSet( wk, FALSE );
			ProfWin_Del( wk );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
			GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
			wk->sub_seq++;
			break;
		
		case 1:
			BR_PaletteFade( &vwk->color, eFADE_MODE_OUT );
			if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
				CATS_SystemActiveSet( wk, TRUE );
				FontButton_DeleteSub( wk );
				SaveFontButton_Create( wk );
				CATS_SystemActiveSet( wk, FALSE );
				GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
				GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_S );
				wk->sub_seq++;
			}
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );
			break;
			
		case 2:		
			Tool_InfoMessageMain( wk, msg_info_008 );			
			wk->sub_seq++;
			break;
		
		case 3:
			if ( !GF_TP_GetTrg() ){ break; }
			Tool_InfoMessageMainDel( wk );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
			GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
			wk->sub_seq++;
			break;

		case 4:
			BR_PaletteFade( &vwk->color, eFADE_MODE_OUT );
			if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
				SaveFontButton_Delete( wk );
				wk->sub_seq++;
			}
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );
			break;
		
		case 5:
			BMN_Delete( vwk->btn_man[ 0 ] );
			BMN_Delete( vwk->btn_man[ 1 ] );
			BMN_Delete( vwk->btn_man[ 2 ] );
			BR_PaletteFadeIn_Init( &vwk->color );
			CATS_SystemActiveSet( wk, TRUE );
			Video_GppDataDelete( wk );
			NormalTag_RecoverAllOp( wk );	
			CATS_SystemActiveSet( wk, FALSE );	
			BR_ChangeDisplayVIntr( wk, DISP_3D_TO_SUB );				///< main = top
			wk->sub_seq++;
			break;
		
		default:
			if ( BR_IsChangeDisplay( wk ) == FALSE ){ break; }
			if ( BR_PaletteFade( &vwk->color, eFADE_MODE_IN ) ){					
				PaletteFadeClear( wk );
				BR_PaletteFadeIn_Init( &vwk->color );
				ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, 16, wk->sys.logo_color );	///< main	oam
				
				BR_Main_ProcSeqChange( wk, eMAIN_Input, TRUE, wk->proc_seq );
				BR_Main_SeqChange( wk, eINPUT_MENU_IN );		
				sys_FreeMemoryEz( vwk );
				GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
				GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
				GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
				GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_S );
				if ( BattleRec_DataExistCheck() == TRUE ){
					BattleRec_Exit();
				}
			}
			else {
				ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG,	 LOGO_PALETTE, vwk->color, wk->sys.logo_color );	///< main	oam
				ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );	///< main	oam
			}
			break;
		}
	}
	else {
		if ( Common_ProfSaveIn( wk ) ){
			BR_Main_SeqChange( wk, eVIDEO_CodeIn_SaveMain );
		}
	}
	return FALSE;
}

static BOOL Video_ProfileSaveMain( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	BMN_Main( vwk->btn_man[ 1 ] );

	return FALSE;
}

static BOOL Video_ProfileSaveExit( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	if ( Common_ProfSaveExit( wk ) ){
		BR_Main_SeqChange( wk, eVIDEO_CodeIn_ProfMain );
	}
	return FALSE;
}


static BOOL Video_PlayRecover( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	if ( Common_ProfPlayRecover( wk ) ){
		BR_Main_SeqChange( wk, eVIDEO_CodeIn_ProfMain );
	}
	return FALSE;
}

static BOOL Video_ProfileCheckIn( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	if ( Common_ProfCheckIn( wk ) ){
		BR_Main_SeqChange( wk, eVIDEO_CodeIn_CheckMain );
	}
	return FALSE;
}

static BOOL Video_ProfileCheckMain( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;

	BMN_Main( vwk->btn_man[ 2 ] );

	return FALSE;
}

static BOOL Video_ProfileCheckExit( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	if ( Common_ProfCheckExit( wk ) ){
		BR_Main_SeqChange( wk, eVIDEO_CodeIn_SaveMain );
	}
	return FALSE;
}

static BOOL Video_ProfileCheckFinish( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	GDS_RAP_ERROR_INFO* error_info;
	
	Common_ProfCheckFinish( wk );
	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	復帰
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL Video_ProfileRecoverIn( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
		if ( BR_IsChangeDisplay( wk ) == FALSE ){ break; }

		Plate_AlphaInit_Default( &vwk->eva, &vwk->evb, ePLANE_ALL );		
		vwk->secure_flag = wk->recv_data.rec_secure_flag;				///< secure flag だけ復帰
		
		vwk->rec_data = &wk->recv_data.rec_data;
		wk->recv_data.rec_data_tmp = vwk->rec_data;
		RecData_To_brs( wk );
		
		Video_GppDataCreate( wk );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
		GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR,   wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
		ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR,   wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
		BR_ChangeDisplayVIntr( wk, DISP_3D_TO_MAIN );
		wk->sub_seq++;
		break;

	case 1:		
		if ( BR_IsChangeDisplay( wk ) == FALSE ){ break; }
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_IN, ePLANE_MAIN ) ){
			GF_BGL_PrioritySet( GF_BGL_FRAME0_M, 1 );			///< priority
			GF_BGL_PrioritySet( GF_BGL_FRAME1_M, 3 );
			GF_BGL_PrioritySet( GF_BGL_FRAME2_M, 0 );
			GF_BGL_PrioritySet( GF_BGL_FRAME3_M, 1 );		
			GF_BGL_PrioritySet( GF_BGL_FRAME0_S, 1 );
			GF_BGL_PrioritySet( GF_BGL_FRAME1_S, 3 );
			GF_BGL_PrioritySet( GF_BGL_FRAME2_S, 0 );
			GF_BGL_PrioritySet( GF_BGL_FRAME3_S, 2 );
			vwk->btn_man[ 0 ] = BMN_Create( hit_table_000, NELEMS( hit_table_000 ), Btn_CallBack_000, wk, HEAPID_BR );
			vwk->btn_man[ 1 ] = BMN_Create( hit_table_001, NELEMS( hit_table_001 ), Btn_CallBack_001, wk, HEAPID_BR );
			vwk->btn_man[ 2 ] = BMN_Create( hit_table_002, NELEMS( hit_table_002 ), Btn_CallBack_002, wk, HEAPID_BR );
			wk->sub_seq++;
		}
		break;
	
	case 2:
		FontButton_CreateSub( wk );
		ProfWin_Add( wk );
		ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR, wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
		ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR, wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
		CATS_SystemActiveSet( wk, TRUE );
		if ( vwk->view_page == 0 ){
			///< プロフィール削除 概要作成
			VideoDataMake( &vwk->vtr_work, wk );
			VideoData_IconEnable( &vwk->vtr_work, FALSE );
		//	ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_bg1ua_data_NSCR, wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
			BR_ScrSet( wk, ProfileBGGet( &vwk->vtr_work ), GF_BGL_FRAME3_M );
		}
		else {
			///< プロフィール作成
			GppDataMake( &vwk->gpp_work, wk );
			GppData_IconEnable( &vwk->gpp_work, FALSE );
			ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_bg1uc_data_NSCR, wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
		}
		CATS_SystemActiveSet( wk, FALSE );
		ArcUtil_HDL_ScrnSet( wk->sys.p_handle,	 NARC_batt_rec_gra_batt_rec_video_d2_NSCR, 	 wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
		wk->sub_seq++;
		break;
	
	case 3:
		vwk->color = 0;
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_IN, ePLANE_ALL ) ){
			if ( vwk->view_page == 0 ){
				VideoData_IconEnable( &vwk->vtr_work, TRUE );
			}
			else {
				GppData_IconEnable( &vwk->gpp_work, TRUE );
			}
			GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG3, VISIBLE_ON );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG3, VISIBLE_ON );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );		
		break;
	
	case 4:
		WIPE_SYS_Start( WIPE_PATTERN_WMS,
			    WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN,
			    WIPE_FADE_BLACK,  WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_BR );
		wk->sub_seq++;
		break;

	case 5:		
		if ( WIPE_SYS_EndCheck() == TRUE ){	
			wk->sub_seq++;
		}
		break;
	
	default:	
		wk->tag_man.stack_s += 1;				///< proc init で Menu_Backが呼ばれ1段階下がってしまうので応急処置
		BR_RecoverWorkClear( wk );
		BR_Main_SeqChange( wk, eVIDEO_CodeIn_ProfMain );
		break;

	}
	
	return FALSE;
}


// =============================================================================
//
//
//	■ビデオ送信	TAG_VIDEO_SEND
//
//
// =============================================================================
//--------------------------------------------------------------
/**
 * @brief	送信	初期化
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL Video_Send_Init( BR_WORK* wk )
{
	HormingCursor_Init( wk, wk->cur_wk, COMM_HORMING_CUR_CX, COMM_HORMING_CUR_CY );
	Cursor_Visible( wk->cur_wk, TRUE );
	Cursor_R_Set( wk->cur_wk, COMM_HORMING_R, COMM_HORMING_R );
	
	BR_Main_SeqChange( wk, eVIDEO_Send_Main );
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	送信	メイン
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL Video_Send_Main( BR_WORK* wk )
{	
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	GDS_RAP_ERROR_INFO* error_info;
	
	switch ( wk->sub_seq ){
	case 0:
		Tool_InfoMessageMain( wk, msg_716 );
		Snd_SePlay( eBR_SND_SEARCH );
		wk->sub_seq++;
		break;
	
	case 1:
		///< 録画データ送信
		if ( BR_GDSRAP_MoveStatusAllCheck( wk ) == FALSE ){ return FALSE; }		
		{
			LOAD_RESULT result;
			
			BattleRec_Load( wk->save, HEAPID_BR, &result, NULL, LOADDATA_MYREC );
			
			vwk->gds_profile_ptr = GDS_Profile_AllocMemory( HEAPID_BR );
			GDS_Profile_MyDataSet( vwk->gds_profile_ptr, wk->save );
			
			if( GDSRAP_Tool_Send_BattleVideoUpload(	BR_GDSRAP_WorkGet( wk ), vwk->gds_profile_ptr ) == TRUE ){
#ifdef PM_DEBUG
			if ( sys.cont & PAD_BUTTON_L
			&&	 sys.cont & PAD_KEY_DOWN )
			{
				u16 def_msg[] = {
					n2_,n0_,n0_,n8_,ne_,n_,SA_,MA_,bou_,MA_,RI_,O_,KU_,RA_,BU_,KA_,TTU_,PU_,CR_,
					ke_,ttu_,si_,yyo_,u_,M__,C__,ko_,ba_,ya_,si_,v__,s__,G__,F__,ni_,si_,no_,EOM_
				};
				DEBUG_GDSRAP_SendVideoProfileFreeWordSet( BR_GDSRAP_WorkGet( wk ), def_msg );
			}
#endif
				GDS_Profile_FreeMemory( vwk->gds_profile_ptr );
				wk->sub_seq++;
			}
		}
		break;
	
	case 2:
		///< 録画データ送信完了
		if ( BR_GDSRAP_MoveStatusAllCheck( wk ) == FALSE ){ return FALSE; }
		
		BattleRec_Exit();
		Tool_InfoMessageMainDel( wk );
//		Tool_InfoMessageMain( wk, msg_717 );	

		if( GDSRAP_ErrorInfoGet( BR_GDSRAP_WorkGet( wk ), &error_info ) == TRUE ){
			Snd_SeStopBySeqNo( eBR_SND_SEARCH, 0 );
			BR_ErrorSet( wk, eERR_VIDEO_SEND, error_info->result, error_info->type );
			Cursor_Visible( wk->cur_wk, FALSE );
			Cursor_R_Set( wk->cur_wk, 0, 0 );
			//エラー発生時の処理
			//エラーメッセージの表示等はコールバックで行われるので、
			//ここでは表示後の処理。(アプリ終了とか特定のメニューに戻すとか)
			wk->sub_seq = 0xFF;
		}
		else{
			Tool_InfoMessageMain( wk, msg_716 );
			BR_SendVideoNo( vwk, wk );
			Snd_SeStopBySeqNo( eBR_SND_SEARCH, 0 );
			Snd_SePlay( eBR_SND_SEND_OK );
			wk->sub_seq++;
		}
		break;
		
	case 3:
		if ( !GF_TP_GetTrg() ){ break; }
		Tool_InfoMessageMainDel( wk );
		wk->sub_seq++;
		break;
	
	default:
		Cursor_Visible( wk->cur_wk, FALSE );
		Cursor_R_Set( wk->cur_wk, 0, 0 );
		BR_AddScore( wk, SCORE_ID_GDS_VIDEO_UPLOAD );
		BR_Main_SeqChange( wk, eVIDEO_Send_Exit );
		break;
	}

	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	送信	終了
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL Video_Send_Exit( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
		BR_PaletteFadeIn_Init( &vwk->color );
		BR_Stack_Menu_Back( wk );
		CATS_SystemActiveSet( wk, TRUE );
		NormalTag_RecoverAllOp( wk );
		CATS_SystemActiveSet( wk, FALSE );
		BR_ChangeDisplayVIntr( wk, DISP_3D_TO_SUB );			///< main = top
		wk->sub_seq++;
		break;
	
	default:
		if ( BR_IsChangeDisplay( wk ) == FALSE ){ break; }
		if ( BR_PaletteFade( &vwk->color, eFADE_MODE_IN ) ){					
			PaletteFadeClear( wk );
			BR_PaletteFadeIn_Init( &vwk->color );
			ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, 16, wk->sys.logo_color );	///< main	oam
			
			BR_Main_ProcSeqChange( wk, eMAIN_Input, TRUE, wk->proc_seq );
			BR_Main_SeqChange( wk, eINPUT_MENU_IN );		
			sys_FreeMemoryEz( vwk );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_S );
		}
		else {
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG,	 LOGO_PALETTE, vwk->color, wk->sys.logo_color );	///< main	oam
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );	///< main	oam
		}
		break;
	}
	
	return FALSE;
}


// =============================================================================
//
//
//	■ツール
//
//
// =============================================================================
//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void ProfWin_Del( BR_WORK* wk )
{	
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;

	GF_BGL_BmpWinOff( &vwk->win_s[ 0 ] );
	GF_BGL_BmpWinDel( &vwk->win_s[ 0 ] );	
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 * @param	id	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void ProfWin_MsgSet( BR_WORK* wk, int id )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	GF_BGL_BMPWIN*	win;
	STRBUF*			str1;
	int x;
	
	win = &vwk->win_s[ 0 ];
	
	GF_BGL_BmpWinDataFill( win, 0x00 );
	str1 = MSGMAN_AllocString( wk->sys.man, id );
	x = BR_print_x_Get( win, str1 );
	GF_STR_PrintColor( win, FONT_SYSTEM, str1, x, 0, MSG_NO_PUT, PRINT_COL_VIDEO, NULL );		
	STRBUF_Delete( str1 );		
	
	GF_BGL_BmpWinOnVReq( win );
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void ProfWin_Add( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	GF_BGL_BMPWIN*	win;
	int				ofs = 1;
	
	win = &vwk->win_s[ 0 ];	
	GF_BGL_BmpWinInit( win );	
	GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_S, 8, 3, 16, 2, eBG_PAL_FONT, ofs );
	
	ProfWin_MsgSet( wk, msg_718 + vwk->view_page );
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void SaveWin_Add( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	GF_BGL_BMPWIN*	win;
	STRBUF*			str1;
	int				ofs = 1;
	int x;
	
	win = &vwk->win_s[ 0 ];
	GF_BGL_BmpWinInit( win );
	GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_S, 4, 6, 10, 2, eBG_PAL_FONT, ofs );	
	GF_BGL_BmpWinDataFill( win, 0x00 );
	str1 = MSGMAN_AllocString( wk->sys.man, msg_1000 );
	x = BR_print_x_Get( win, str1 );
	GF_STR_PrintColor( win, FONT_SYSTEM, str1, x, 0, MSG_NO_PUT, PRINT_COL_VIDEO, NULL );		
	STRBUF_Delete( str1 );	
	GF_BGL_BmpWinOnVReq( win );
	
	ofs += ( 10 * 2 );
	
	win = &vwk->win_s[ 1 ];
	GF_BGL_BmpWinInit( win );
	GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_S, 18, 6, 10, 2, eBG_PAL_FONT, ofs );	
	GF_BGL_BmpWinDataFill( win, 0x00 );
	str1 = MSGMAN_AllocString( wk->sys.man, msg_1001 );
	x = BR_print_x_Get( win, str1 );
	GF_STR_PrintColor( win, FONT_SYSTEM, str1, x, 0, MSG_NO_PUT, PRINT_COL_VIDEO, NULL );		
	STRBUF_Delete( str1 );	
	GF_BGL_BmpWinOnVReq( win );	
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void SaveWin_Del( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;

	GF_BGL_BmpWinOff( &vwk->win_s[ 0 ] );
	GF_BGL_BmpWinDel( &vwk->win_s[ 0 ] );
	GF_BGL_BmpWinOff( &vwk->win_s[ 1 ] );
	GF_BGL_BmpWinDel( &vwk->win_s[ 1 ] );
}


//--------------------------------------------------------------
/**
 * @brief	ボタンコールバック
 *
 * @param	button	
 * @param	event	
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void Btn_CallBack_000( u32 button, u32 event, void* work )
{
	BR_WORK*  wk = work;
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	if ( event != BMN_EVENT_TOUCH ){ return; }
	
	OS_Printf( "button = %d\n", button );
	
	switch ( button ){
	case 0:
		TouchAction( wk );
		BR_Main_SeqChange( wk, eVIDEO_CodeIn_ProfChange );
		break;

	case 1:
		TouchAction( wk );
		BR_Main_SeqChange( wk, eVIDEO_CodeIn_Play );
		break;

	case 2:
		TouchAction( wk );
		BR_Main_SeqChange( wk, eVIDEO_CodeIn_SaveIn );
		break;

	case 3:
		TouchAction( wk );
		BR_Main_SeqChange( wk, eVIDEO_CodeIn_ProfExit );
		break;
	}
}


//--------------------------------------------------------------
/**
 * @brief	ボタンコールバック
 *
 * @param	button	
 * @param	event	
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void Btn_CallBack_001( u32 button, u32 event, void* work )
{
	BR_WORK*  wk = work;
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	if ( event != BMN_EVENT_TOUCH ){ return; }
	
	OS_Printf( "button = %d\n", button );
	
	switch ( button ){
	case 0:
	case 1:
	case 2:
		vwk->save_no = button + 1;
		if ( wk->br_gpp[ vwk->save_no ] != NULL ){
			BR_Main_SeqChange( wk, eVIDEO_CodeIn_CheckIn );
		}
		else {
			BR_Main_SeqChange( wk, eVIDEO_CodeIn_CheckFinish );
		}
		TouchAction( wk );
		break;

	case 3:
		TouchAction( wk );
		BR_Main_SeqChange( wk, eVIDEO_CodeIn_SaveExit );
		break;
	}
}


//--------------------------------------------------------------
/**
 * @brief	ボタンコールバック
 *
 * @param	button	
 * @param	event	
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void Btn_CallBack_002( u32 button, u32 event, void* work )
{
	BR_WORK*  wk = work;
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	if ( event != BMN_EVENT_TOUCH ){ return; }
	
	OS_Printf( "button = %d\n", button );
	
	switch ( button ){
	case 0:
		TouchAction( wk );
		SaveWin_Del( wk );
		BR_Main_SeqChange( wk, eVIDEO_CodeIn_CheckFinish );
		break;

	case 1:
		TouchAction( wk );
		BR_Main_SeqChange( wk, eVIDEO_CodeIn_CheckExit );
		break;
	}
}


static void Btn_CallBack_100( u32 button, u32 event, void* work )
{
	BR_WORK*  wk = work;
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	if ( event != BMN_EVENT_TOUCH ){ return; }
	
	OS_Printf( "button = %d\n", button );
	
	switch ( button ){
	case 0:
		TouchAction( wk );
		vwk->search_mode = button;
		BR_Main_SeqChange( wk, eCS_SearchCommonInit );
		break;

	case 1:
		TouchAction( wk );
		vwk->search_mode = button;
		BR_Main_SeqChange( wk, eCS_SearchCommonInit );
		break;

	case 2:
		{					
			int country = GDS_Profile_GetNation( wk->br_gpp[ 0 ] );
			TouchAction( wk );
			if ( country == 0 ){
				Snd_SePlay( eBR_SND_NG );
				Tool_InfoMessageMainSet( wk, msg_725 );	///< プロフィールが無い
			}
			else {
				vwk->search_mode = button;
				BR_Main_SeqChange( wk, eCS_SearchCommonInit );
			}
		}
		break;

	case 3:
		TouchAction( wk );
		BR_Main_SeqChange( wk, eCS_TopViewExit );
		break;

	case 4:
		TouchAction( wk );
#ifdef BR_AUTO_SERCH
		vwk->monsno = 20;
#endif
		if ( IsSearchParamSet( wk ) == FALSE ){
			Snd_SePlay( eBR_SND_NG );
			Tool_InfoMessageMainSet( wk, msg_713 );	///< 条件が入ってません！
		}
		else {
			BR_Main_SeqChange( wk, eCS_SearchMain );
		}
		break;
	}
}

static void Btn_CallBack_AIUEO( u32 button, u32 event, void* work )
{
	BR_WORK*  wk = work;
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	if ( event != BMN_EVENT_TOUCH ){ return; }

	if ( IsPokeNameList( wk, button ) == FALSE ){
		Snd_SePlay( eBR_SND_NG );
		return;
	}
	
	OS_Printf( "button = %d\n", button );
	
	PokemonNameList_DataMake( wk, button );
}

//--------------------------------------------------------------
/**
 * @brief	ボタンコールバック
 *
 * @param	button	
 * @param	event	
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void Btn_CallBack_200( u32 button, u32 event, void* work )
{
	BR_WORK*  wk = work;
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	if ( event != BMN_EVENT_TOUCH ){ return; }
	
	OS_Printf( "button = %d\n", button );
	
	switch ( button ){
	case 0:
		TouchAction( wk );
		BR_Main_SeqChange( wk, eCS_ProfChange );
		break;

	case 1:
		TouchAction( wk );
		if ( BattleRec_DataExistCheck() ){
			BR_Main_SeqChange( wk, eCS_Play );
		}
		else {
			vwk->tmp_seq = eCS_Play;
			BR_Main_SeqChange( wk, eCS_BrsDataGet );
		}
		break;

	case 2:
		TouchAction( wk );
		if ( BattleRec_DataExistCheck() ){
			BR_Main_SeqChange( wk, eCS_SaveIn );
		}
		else {
			vwk->tmp_seq = eCS_SaveIn;
			BR_Main_SeqChange( wk, eCS_BrsDataGet );
		}
		break;

	case 3:
		TouchAction( wk );
		BR_Main_SeqChange( wk, eCS_ProfExit );
		break;
	}
}


//--------------------------------------------------------------
/**
 * @brief	ボタンコールバック
 *
 * @param	button	
 * @param	event	
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void Btn_CallBack_201( u32 button, u32 event, void* work )
{
	BR_WORK*  wk = work;
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	if ( event != BMN_EVENT_TOUCH ){ return; }
	
	OS_Printf( "button = %d\n", button );
	
	switch ( button ){
	case 0:
	case 1:
	case 2:
		vwk->save_no = button + 1;
		if ( wk->br_gpp[ vwk->save_no ] != NULL ){
			BR_Main_SeqChange( wk, eCS_CheckIn );
		}
		else {
			BR_Main_SeqChange( wk, eCS_CheckFinish );
		}
		TouchAction( wk );
		break;

	case 3:
		TouchAction( wk );
		BR_Main_SeqChange( wk, eCS_SaveExit );
		break;
	}
}


//--------------------------------------------------------------
/**
 * @brief	ボタンコールバック
 *
 * @param	button	
 * @param	event	
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void Btn_CallBack_202( u32 button, u32 event, void* work )
{
	BR_WORK*  wk = work;
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	if ( event != BMN_EVENT_TOUCH ){ return; }
	
	OS_Printf( "button = %d\n", button );
	
	switch ( button ){
	case 0:
		TouchAction( wk );
		SaveWin_Del( wk );
		BR_Main_SeqChange( wk, eCS_CheckFinish );
		break;

	case 1:
		TouchAction( wk );
		BR_Main_SeqChange( wk, eCS_CheckExit );
		break;
	}
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 * @param	mode	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void ButtonOAM_PosSet( BR_WORK* wk, int mode )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	if ( mode == 0 ){
		///< 2個
		CATS_ObjectEnableCap( vwk->obtn[ 0 ].cap, TRUE );
		FONTOAM_SetDrawFlag( vwk->obtn[ 0 ].font_obj, TRUE );
		CATS_ObjectPosSetCap( vwk->obtn[ 1 ].cap, 128, 232 - 64 );
	}
	else {
		///< 1個
		CATS_ObjectEnableCap( vwk->obtn[ 0 ].cap, FALSE );
		FONTOAM_SetDrawFlag( vwk->obtn[ 0 ].font_obj, FALSE );
		CATS_ObjectPosSetCap( vwk->obtn[ 1 ].cap,  80, 232 - 64 );
	}
	FONTOAM_SetMat( vwk->obtn[ 0 ].font_obj, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY );
	FONTOAM_SetMat( vwk->obtn[ 1 ].font_obj, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY );
}


//--------------------------------------------------------------
/**
 * @brief	やめる・ＯＫ！
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void FontButton_Create( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;

	ExTag_ResourceLoad( wk, NNS_G2D_VRAM_TYPE_2DMAIN );		
	vwk->obtn[ 0 ].cap = ExTag_Add( wk, NNS_G2D_VRAM_TYPE_2DMAIN );
	vwk->obtn[ 1 ].cap = ExTag_Add( wk, NNS_G2D_VRAM_TYPE_2DMAIN );
	FontOam_Create( &vwk->obtn[ 0 ], &wk->sys, NNS_G2D_VRAM_TYPE_2DMAIN );
	FontOam_Create( &vwk->obtn[ 1 ], &wk->sys, NNS_G2D_VRAM_TYPE_2DMAIN );
	FontOam_MsgSet( &vwk->obtn[ 0 ], &wk->sys, msg_05 );
	FontOam_MsgSet( &vwk->obtn[ 1 ], &wk->sys, msg_708 );

	CATS_ObjectPosSetCap( vwk->obtn[ 0 ].cap,  32, 232 - 64 );
	CATS_ObjectPosSetCap( vwk->obtn[ 1 ].cap, 128, 232 - 64 );
	FONTOAM_SetMat( vwk->obtn[ 0 ].font_obj, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY );
	FONTOAM_SetMat( vwk->obtn[ 1 ].font_obj, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY );
	FONTOAM_SetDrawFlag( vwk->obtn[ 0 ].font_obj, TRUE );
	FONTOAM_SetDrawFlag( vwk->obtn[ 1 ].font_obj, TRUE );
	CATS_ObjectAnimeSeqSetCap( vwk->obtn[ 0 ].cap, eTAG_EX_BACK );
	CATS_ObjectAnimeSeqSetCap( vwk->obtn[ 1 ].cap, eTAG_EX_OK );
		
	ButtonOAM_PosSet( wk, 0 );	
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void FontButton_Delete( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	FontOam_Delete( &vwk->obtn[ 0 ] );
	FontOam_Delete( &vwk->obtn[ 1 ] );
	CATS_ActorPointerDelete_S( vwk->obtn[ 0 ].cap );
	CATS_ActorPointerDelete_S( vwk->obtn[ 1 ].cap );
	
	ExTag_ResourceDelete( wk );			
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 * @param	mode	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void ButtonOAM_PosSetSub( BR_WORK* wk, int mode )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	if ( mode == 0 ){
		///< 2個
		CATS_ObjectEnableCap( vwk->obtn[ 1 ].cap, TRUE );
		FONTOAM_SetDrawFlag( vwk->obtn[ 1 ].font_obj, TRUE );
		CATS_ObjectPosSetCap( vwk->obtn[ 0 ].cap, 32, 232 );
	}
	else {
		///< 1個
		CATS_ObjectEnableCap( vwk->obtn[ 1 ].cap, FALSE );
		FONTOAM_SetDrawFlag( vwk->obtn[ 1 ].font_obj, FALSE );
		CATS_ObjectPosSetCap( vwk->obtn[ 0 ].cap,  80, 232 );
	}
	FONTOAM_SetMat( vwk->obtn[ 0 ].font_obj, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY );
	FONTOAM_SetMat( vwk->obtn[ 1 ].font_obj, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY );
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void FontButton_CreateSub( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;

	ExTag_ResourceLoad( wk, NNS_G2D_VRAM_TYPE_2DSUB );		
	vwk->obtn[ 0 ].cap = ExTag_Add( wk, NNS_G2D_VRAM_TYPE_2DSUB );
	vwk->obtn[ 1 ].cap = ExTag_Add( wk, NNS_G2D_VRAM_TYPE_2DSUB );
	FontOam_Create( &vwk->obtn[ 0 ], &wk->sys, NNS_G2D_VRAM_TYPE_2DSUB );
	FontOam_Create( &vwk->obtn[ 1 ], &wk->sys, NNS_G2D_VRAM_TYPE_2DSUB );
	
	if ( wk->ex_param1 == BR_EX_DATA_KUWASIKU ){
		FontOam_MsgSet( &vwk->obtn[ 0 ], &wk->sys, msg_05 );
		FontOam_MsgSet( &vwk->obtn[ 1 ], &wk->sys, msg_708 );
		CATS_ObjectAnimeSeqSetCap( vwk->obtn[ 0 ].cap, eTAG_EX_BACK );
		CATS_ObjectAnimeSeqSetCap( vwk->obtn[ 1 ].cap, eTAG_EX_OK );
	}
	else {
		FontOam_MsgSet( &vwk->obtn[ 0 ], &wk->sys, msg_05 );
		FontOam_MsgSet( &vwk->obtn[ 1 ], &wk->sys, msg_602 );
		CATS_ObjectAnimeSeqSetCap( vwk->obtn[ 0 ].cap, eTAG_EX_BACK );
		CATS_ObjectAnimeSeqSetCap( vwk->obtn[ 1 ].cap, eTAG_EX_SAVE );
	}

	CATS_ObjectPosSetCap( vwk->obtn[ 0 ].cap,  32, 232 );
	CATS_ObjectPosSetCap( vwk->obtn[ 1 ].cap, 128, 232 );
	FONTOAM_SetMat( vwk->obtn[ 0 ].font_obj, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY );
	FONTOAM_SetMat( vwk->obtn[ 1 ].font_obj, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY );
	FONTOAM_SetDrawFlag( vwk->obtn[ 0 ].font_obj, TRUE );
	FONTOAM_SetDrawFlag( vwk->obtn[ 1 ].font_obj, TRUE );
		
	ButtonOAM_PosSetSub( wk, 0 );	
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void FontButton_DeleteSub( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	FontOam_Delete( &vwk->obtn[ 0 ] );
	FontOam_Delete( &vwk->obtn[ 1 ] );
	CATS_ActorPointerDelete_S( vwk->obtn[ 0 ].cap );
	CATS_ActorPointerDelete_S( vwk->obtn[ 1 ].cap );
	ExTag_ResourceDelete( wk );
}

// =============================================================================
//
//
//	■共通のシーケンス
//
//
// =============================================================================
static BOOL Common_ProfChange( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
		CATS_SystemActiveSet( wk, TRUE );
		if ( vwk->view_page == 0 ){
			///< 概要削除 プロフィール作成
			VideoDataFree( &vwk->vtr_work, wk );
		}
		else {
			///< プロフィール削除 概要作成
			GppDataFree( &vwk->gpp_work, wk );						
		}
		CATS_SystemActiveSet( wk, FALSE );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		wk->sub_seq++;
		break;
	
	case 1:
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_OUT, ePLANE_MAIN ) ){
			wk->sub_seq++;
		}
		break;
	
	case 2:
		CATS_SystemActiveSet( wk, TRUE );
		if ( vwk->view_page == 0 ){
			///< プロフィール作成
			GppDataMake( &vwk->gpp_work, wk );
			GppData_IconEnable( &vwk->gpp_work, FALSE );
			BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_bg1uc_data_NSCR, GF_BGL_FRAME3_M );
		}
		else {
			///< プロフィール削除 概要作成
			VideoDataMake( &vwk->vtr_work, wk );
			VideoData_IconEnable( &vwk->vtr_work, FALSE );
		//	BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_bg1ua_data_NSCR, GF_BGL_FRAME3_M );
			BR_ScrSet( wk, ProfileBGGet( &vwk->vtr_work ), GF_BGL_FRAME3_M );
		}
		CATS_SystemActiveSet( wk, FALSE );
		wk->sub_seq++;
		break;
	
	case 3:
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_IN, ePLANE_MAIN ) ){
			if ( vwk->view_page == 0 ){
				GppData_IconEnable( &vwk->gpp_work, TRUE );
			}
			else {
				VideoData_IconEnable( &vwk->vtr_work, TRUE );
			}
			GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			wk->sub_seq++;
		}
		break;

	default:
		vwk->view_page ^= 1;
		ProfWin_MsgSet( wk, msg_718 + vwk->view_page );
		return TRUE;
	}
	
	return FALSE;	
}

static BOOL Common_ProfSaveIn( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;

	switch ( wk->sub_seq ){
	case 0:
		CATS_SystemActiveSet( wk, TRUE );
		if ( vwk->view_page == 0 ){
			///< 概要削除 プロフィール作成
			VideoDataFree( &vwk->vtr_work, wk );
		}
		else {
			///< プロフィール削除 概要作成
			GppDataFree( &vwk->gpp_work, wk );						
		}
		CATS_SystemActiveSet( wk, FALSE );
		ProfWin_Del( wk );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		wk->sub_seq++;
		break;
	
	case 1:
		BR_PaletteFade( &vwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
			CATS_SystemActiveSet( wk, TRUE );
			FontButton_DeleteSub( wk );
			SaveFontButton_Create( wk );
			CATS_SystemActiveSet( wk, FALSE );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_S );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );
		break;
	
	case 2:
		BR_PaletteFade( &vwk->color, eFADE_MODE_IN );
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_IN, ePLANE_MAIN ) ){
			InfoMessage( wk, msg_608, FALSE );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );		
		break;

	default:
		return TRUE;
	//	BR_Main_SeqChange( wk, eVIDEO_CodeIn_SaveMain );
		break;
	}
	
	return FALSE;	
}

static BOOL Common_ProfCheckIn( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	switch( wk->sub_seq ){
	case 0:
		BR_PaletteFade( &vwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_OUT, ePLANE_SUB ) ){
			ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_answer_NSCR, wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
			InfoMessageSet( wk, msg_610, vwk->save_no );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );
		break;
	
	case 1:
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_IN, ePLANE_SUB ) ){
			SaveWin_Add( wk );
			GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );			
			wk->sub_seq++;
		}
		break;

	default:
		return TRUE;
	}	
	
	return FALSE;
}

static BOOL Common_ProfCheckExit( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;

	switch( wk->sub_seq ){
	case 0:
		GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		wk->sub_seq++;
		break;
	
	case 1:
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_OUT, ePLANE_SUB ) ){
			SaveWin_Del( wk );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_S );
			wk->sub_seq++;
		}
		break;
	
	case 2:
		BR_PaletteFade( &vwk->color, eFADE_MODE_IN );
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_IN, ePLANE_SUB ) ){
			InfoMessageSet( wk, msg_608, 0 );
			GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );			
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );
		break;

	default:
		return TRUE;
	}

	return FALSE;
}

static BOOL Common_ProfCheckFinish( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	GDS_RAP_ERROR_INFO* error_info;

	switch( wk->sub_seq ){
	case 0:
		GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		wk->sub_seq++;
		break;
	
	case 1:
		BR_PaletteFade( &vwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_OUT, ePLANE_SUB ) ){
		//	SaveWin_Del( wk );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
			InfoMessageSet( wk, msg_info_009, 0 );
			BR_SaveWork_Clear( wk );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );
		break;

	case 2:
		OS_Printf( "rec secure = %d\n", vwk->secure_flag );
		if ( RecData_Save( wk, vwk->save_no, vwk->secure_flag ) ){
			vwk->secure_flag = 0;
			wk->sub_seq++;
		}
		break;
	
	case 3:
	//	InfoMessageSet( wk, msg_1003, 0 );
		HormingCursor_Init( wk, wk->cur_wk, COMM_HORMING_CUR_CX, COMM_HORMING_CUR_CY );
		Cursor_Visible( wk->cur_wk, TRUE );
		Cursor_R_Set( wk->cur_wk, COMM_HORMING_R, COMM_HORMING_R );
		Snd_SePlay( eBR_SND_SEARCH );
		wk->sub_seq++;
		break;
	
	case 4:
		if ( BR_GDSRAP_MoveStatusAllCheck( wk ) == FALSE ){ return FALSE; }
		InfoMessageSet( wk, msg_info_011, 0 );	
		{
			u64 data_no = RecHeader_ParamGet( BattleRec_HeaderPtrGet(), RECHEAD_IDX_DATA_NUMBER, 0 );
				
			if ( GDSRAP_Tool_Send_BattleVideo_FavoriteUpload( BR_GDSRAP_WorkGet( wk ), data_no ) == TRUE ){
				wk->sub_seq++;
			}
		}
		break;
	
	case 5:
		if ( BR_GDSRAP_MoveStatusAllCheck( wk ) == FALSE ){ return FALSE; }
		
		if ( GDSRAP_ErrorInfoGet( BR_GDSRAP_WorkGet( wk ), &error_info ) == TRUE ){
			Snd_SeStopBySeqNo( eBR_SND_SEARCH, 0 );
			//エラーは無視しても問題ない
			wk->sub_seq++;
		}
		else{
			Snd_SeStopBySeqNo( eBR_SND_SEARCH, 0 );
			Snd_SePlay( eBR_SND_RECV_OK );
			wk->sub_seq++;
		}
		InfoMessageSet( wk, msg_info_012, 0 );	
		break;
	
	case 6:
		///< ここから終了へ向かう
		if ( !GF_TP_GetTrg() ){ break; }
		InfoMessageDel( wk );
		SaveFontButton_Delete( wk );		
		Cursor_Visible( wk->cur_wk, FALSE );
		Cursor_R_Set( wk->cur_wk, 0, 0 );
		
		BR_SaveData_GppHead_Delete( wk );						///< GPP データ破棄
		BR_SaveData_GppHead_Load( wk );							///< GPP データの再読み込み					
		TouchList_WorkClear( &wk->sys.touch_list );	

		wk->sub_seq++;
		break;
	
	case 7:
		BMN_Delete( vwk->btn_man[ 0 ] );
		BMN_Delete( vwk->btn_man[ 1 ] );
		BMN_Delete( vwk->btn_man[ 2 ] );
		BR_PaletteFadeIn_Init( &vwk->color );
		CATS_SystemActiveSet( wk, TRUE );
		Video_GppDataDelete( wk );
		NormalTag_RecoverAllOp( wk );
		CATS_SystemActiveSet( wk, FALSE );
		WirelessIconEasy_HoldLCD( TRUE, HEAPID_BR );		
		BR_ChangeDisplayVIntr( wk, DISP_3D_TO_SUB );				///< main = top
		wk->sub_seq++;
		break;
	
	default:
		if ( BR_IsChangeDisplay( wk ) == FALSE ){ break; }
		if ( BR_PaletteFade( &vwk->color, eFADE_MODE_IN ) ){					
			PaletteFadeClear( wk );
			BR_PaletteFadeIn_Init( &vwk->color );
			ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, 16, wk->sys.logo_color );	///< main	oam
			
			BR_Main_ProcSeqChange( wk, eMAIN_Input, TRUE, wk->proc_seq );
			BR_Main_SeqChange( wk, eINPUT_MENU_IN );		
			sys_FreeMemoryEz( vwk );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_S );
			if ( BattleRec_DataExistCheck() == TRUE ){
				BattleRec_Exit();
			}
		}
		else {
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG,	 LOGO_PALETTE, vwk->color, wk->sys.logo_color );	///< main	oam
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );	///< main	oam
		}
		break;
	}

	return FALSE;
}

static BOOL Common_ProfPlayRecover( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
		GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		wk->sub_seq++;
		break;

	case 1:
		BR_PaletteFade( &vwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
		//	SaveFontButton_Delete( wk );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );
		break;
	
	case 2:
	//	FontButton_CreateSub( wk );
		if ( wk->ex_param1 == BR_EX_DATA_KUWASIKU ){	
			///< この時点だとボタンのタグ情報が変わってるので入れ替える
			FontOam_MsgSet( &vwk->obtn[ 0 ], &wk->sys, msg_05 );
			FontOam_MsgSet( &vwk->obtn[ 1 ], &wk->sys, msg_602 );
			CATS_ObjectAnimeSeqSetCap( vwk->obtn[ 0 ].cap, eTAG_EX_BACK );
			CATS_ObjectAnimeSeqSetCap( vwk->obtn[ 1 ].cap, eTAG_EX_SAVE );
		}
		ProfWin_Del( wk );
		ProfWin_Add( wk );
		ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR, wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
		ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR, wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
		CATS_SystemActiveSet( wk, TRUE );
		if ( vwk->view_page == 0 ){
			///< プロフィール削除 概要作成
			VideoDataFree( &vwk->vtr_work, wk );
			VideoDataMake( &vwk->vtr_work, wk );
			VideoData_IconEnable( &vwk->vtr_work, FALSE );
		//	ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_bg1ua_data_NSCR, wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
			BR_ScrSet( wk, ProfileBGGet( &vwk->vtr_work ), GF_BGL_FRAME3_M );
		}
		else {
			///< プロフィール作成
			GppDataFree( &vwk->gpp_work, wk );
			GppDataMake( &vwk->gpp_work, wk );
			GppData_IconEnable( &vwk->gpp_work, FALSE );
			ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_bg1uc_data_NSCR, wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
		}
		CATS_SystemActiveSet( wk, FALSE );
		ArcUtil_HDL_ScrnSet( wk->sys.p_handle,	 NARC_batt_rec_gra_batt_rec_video_d2_NSCR, 	 wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
		wk->sub_seq++;
		break;
	
	case 3:
		BR_PaletteFade( &vwk->color, eFADE_MODE_IN );
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_IN, ePLANE_ALL ) ){
			if ( vwk->view_page == 0 ){
				VideoData_IconEnable( &vwk->vtr_work, TRUE );
			}
			else {
				GppData_IconEnable( &vwk->gpp_work, TRUE );
			}
			GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );		
		break;

	default:		
		return TRUE;
	}
	return FALSE;
}

static BOOL Common_ProfSaveExit( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
		InfoMessageDel( wk );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
		GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		wk->sub_seq++;
		break;

	case 1:
		BR_PaletteFade( &vwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
			SaveFontButton_Delete( wk );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );
		break;
	
	case 2:
		FontButton_CreateSub( wk );
		if ( wk->ex_param1 == BR_EX_DATA_KUWASIKU ){	
			///< この時点だとボタンのタグ情報が変わってるので入れ替える
			FontOam_MsgSet( &vwk->obtn[ 0 ], &wk->sys, msg_05 );
			FontOam_MsgSet( &vwk->obtn[ 1 ], &wk->sys, msg_602 );
			CATS_ObjectAnimeSeqSetCap( vwk->obtn[ 0 ].cap, eTAG_EX_BACK );
			CATS_ObjectAnimeSeqSetCap( vwk->obtn[ 1 ].cap, eTAG_EX_SAVE );
		}
		ProfWin_Add( wk );
		ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR, wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
		ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR, wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
		CATS_SystemActiveSet( wk, TRUE );
		if ( vwk->view_page == 0 ){
			///< プロフィール削除 概要作成
			VideoDataMake( &vwk->vtr_work, wk );
			VideoData_IconEnable( &vwk->vtr_work, FALSE );
		//	ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_bg1ua_data_NSCR, wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
			BR_ScrSet( wk, ProfileBGGet( &vwk->vtr_work ), GF_BGL_FRAME3_M );
		}
		else {
			///< プロフィール作成
			GppDataMake( &vwk->gpp_work, wk );
			GppData_IconEnable( &vwk->gpp_work, FALSE );
			ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_bg1uc_data_NSCR, wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
		}
		CATS_SystemActiveSet( wk, FALSE );
		ArcUtil_HDL_ScrnSet( wk->sys.p_handle,	 NARC_batt_rec_gra_batt_rec_video_d2_NSCR, 	 wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
		wk->sub_seq++;
		break;
	
	case 3:
		BR_PaletteFade( &vwk->color, eFADE_MODE_IN );
		if ( Plate_AlphaFade( &vwk->eva, &vwk->evb, eFADE_MODE_IN, ePLANE_ALL ) ){
			if ( vwk->view_page == 0 ){
				VideoData_IconEnable( &vwk->vtr_work, TRUE );
			}
			else {
				GppData_IconEnable( &vwk->gpp_work, TRUE );
			}
			GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, vwk->color, wk->sys.logo_color );		
		break;

	default:		
		return TRUE;
	}
	return FALSE;
}

static BOOL Common_ProfPlay( BR_WORK* wk )
{
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:		
		WIPE_SYS_Start( WIPE_PATTERN_WMS,
			    WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT,
			    WIPE_FADE_BLACK,  WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_BR );
		wk->sub_seq++;
		break;

	case 1:		
		if ( WIPE_SYS_EndCheck() == TRUE ){	
			wk->sub_seq++;
		}
		break;
	
	case 2:		
		CATS_SystemActiveSet( wk, TRUE );
		if ( vwk->view_page == 0 ){
			///< 概要削除
			VideoDataFree( &vwk->vtr_work, wk );
		}
		else {
			///< プロフィール削除
			GppDataFree( &vwk->gpp_work, wk );						
		}
		CATS_SystemActiveSet( wk, FALSE );
		BMN_Delete( vwk->btn_man[ 0 ] );
		BMN_Delete( vwk->btn_man[ 1 ] );
		BMN_Delete( vwk->btn_man[ 2 ] );
		
		CATS_SystemActiveSet( wk, TRUE );
		Video_GppDataDelete( wk );
		ProfWin_Del( wk );
		FontButton_DeleteSub( wk );
		NormalTag_RecoverAllOp_Simple( wk );
		CATS_SystemActiveSet( wk, FALSE );
		BR_RecoverSeqSet( wk, *(wk->proc_seq) );
		BR_PostMessage( wk->message, TRUE, 0 );
		
		wk->sub_seq++;
		break;
	
	default:
		return TRUE;
	}
	return FALSE;	
}

// =============================================================================
//
//
//	■送受信レスポンスコールバック
//
//
// =============================================================================
//--------------------------------------------------------------
/**
 * @brief	ビデオ送信
 *
 * @param	work	
 * @param	error_info	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_Response_VideoRegist( void *work, const GDS_RAP_ERROR_INFO *error_info )
{
	BR_WORK* wk = work;
	VIDEO_SEARCH_WORK* vwk = wk->sub_work;

	if( error_info->occ == TRUE ){
		//TRUEならばエラー発生しているので、ここでメニューを戻すとかアプリ終了モードへ移行とかする
	}
	else{
		//正常時ならば受信バッファからデータ取得などを行う
		//アップロードの場合は特に必要なし
		u64 data_number;
		data_number = GDS_RAP_RESPONSE_BattleVideo_Upload_DataGet( BR_GDSRAP_WorkGet( wk ) );
		vwk->data_number = data_number;
	}			
}


//--------------------------------------------------------------
/**
 * @brief	ビデオ受信
 *
 * @param	work	
 * @param	error_info	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_Response_VideoGet( void *work, const GDS_RAP_ERROR_INFO* error_info )
{
	BR_WORK* wk = work;
//	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	
	OS_TPrintf("バトルビデオデータ取得のダウンロードレスポンス取得\n");
	if( error_info->occ == TRUE ){
		//TRUEならばエラー発生しているので、ここでメニューを戻すとかアプリ終了モードへ移行とかする
	}
	else{
		//正常時ならば受信バッファからデータ取得などを行う
		//アップロードの場合は特に必要なし
		
		OS_Printf( "ビデオダウンロード\n");
		GDS_RAP_RESPONSE_BattleVideoData_Download_RecvPtr_Set( BR_GDSRAP_WorkGet( wk ), &wk->recv_data.rec_data_tmp );
//		GDS_RAP_RESPONSE_BattleVideoData_Download_RecvPtr_Set( BR_GDSRAP_WorkGet( wk ), &vwk->rec_data );
		RecData_To_brs( wk );
		wk->recv_data.rec_secure_flag = wk->recv_data.rec_data_tmp->head.secure;
		OS_Printf( " secure flag = %d\n", wk->recv_data.rec_secure_flag );

	//	int GDS_RAP_RESPONSE_Boxshot_Download_RecvPtr_Set(GDS_RAP_WORK *gdsrap, GT_BOX_SHOT_RECV **box_array, int array_max);
	}
}


//--------------------------------------------------------------
/**
 * @brief	お気に入り
 *
 * @param	work	
 * @param	error_info	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_Response_VideoDL( void *work, const GDS_RAP_ERROR_INFO* error_info )
{
	BR_WORK* wk = work;
	
	if( error_info->occ == TRUE ){
		//TRUEならばエラー発生しているので、ここでメニューを戻すとかアプリ終了モードへ移行とかする
	}
	else{
		//正常時ならば受信バッファからデータ取得などを行う
		//アップロードの場合は特に必要なし
	}	
}

static void BR_SendVideoNo( VIDEO_SEARCH_WORK* vwk, BR_WORK* wk )
{
	STRBUF* 	str1;
	STRBUF* 	str2;
	STRBUF*		tmp;
	WORDSET*	wset;
	GF_BGL_BMPWIN* win;
	
	wset = BR_WORDSET_Create( HEAPID_BR );
		
	///< データナンバー	
	{
		u64 data = vwk->data_number;
		u64	dtmp1 = data;
		u32	dtmp2[ 3 ];
		STRBUF* num1 = STRBUF_Create( 255, HEAPID_BR );
		STRBUF* num2 = STRBUF_Create( 255, HEAPID_BR );
		STRBUF* num3 = STRBUF_Create( 255, HEAPID_BR );

		win = &wk->info_win;

		dtmp2[ 0 ] = dtmp1 % 100000;	///< 3block
		dtmp1 /= 100000;
		dtmp2[ 1 ] = dtmp1 % 100000;	///< 2block
		dtmp1 /= 100000;
		dtmp2[ 2 ] = dtmp1;				///< 1block


		str2 = MSGMAN_AllocString( wk->sys.man, msg_info_026 );		///<
		tmp  = STRBUF_Create( 255, HEAPID_BR );						///< テンポラリ
		
		STRBUF_SetNumber( num1, dtmp2[ 0 ], 5, NUMBER_DISPTYPE_ZERO, NUMBER_CODETYPE_DEFAULT);
		STRBUF_SetNumber( num2, dtmp2[ 1 ], 5, NUMBER_DISPTYPE_ZERO, NUMBER_CODETYPE_DEFAULT);
		STRBUF_SetNumber( num3, dtmp2[ 2 ], 2, NUMBER_DISPTYPE_ZERO, NUMBER_CODETYPE_DEFAULT);
					
		WORDSET_RegisterWord( wset, 2, num1, 0, TRUE, PM_LANG );
		WORDSET_RegisterWord( wset, 1, num2, 0, TRUE, PM_LANG );
		WORDSET_RegisterWord( wset, 0, num3, 0, TRUE, PM_LANG );	
		WORDSET_ExpandStr( wset, tmp, str2 );
				
		GF_BGL_BmpWinDataFill( win, BR_MSG_CLEAR_CODE );
		GF_STR_PrintColor( win, FONT_SYSTEM, tmp, 0, 0, MSG_NO_PUT, PRINT_COL_NORMAL, NULL );
		GF_BGL_BmpWinOnVReq( win );

		STRBUF_Delete( num1 );
		STRBUF_Delete( num2 );
		STRBUF_Delete( num3 );
		STRBUF_Delete( str2 );
		STRBUF_Delete( tmp );
		WORDSET_ClearAllBuffer( wset );
	}
	
	WORDSET_Delete( wset );
}

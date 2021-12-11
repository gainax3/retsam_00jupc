//==============================================================================
/**
 * @file	codein_disp.c
 * @brief	文字入力インターフェース
 * @author	goto
 * @date	2007.07.11(水)
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
#include "system/window.h"
#include "msgdata/msg.naix"
#include "msgdata/msg_fightmsg_dp.h"
#include "system/wipe.h"
#include "system/brightness.h"
#include "system/fontproc.h"
#include "system/lib_pack.h"
#include "system/fontoam.h"
#include "system/msg_ds_icon.h"
#include "gflib/msg_print.h"
#include "gflib/touchpanel.h"
#include "poketool/poke_tool.h"
#include "poketool/monsno.h"
#include "poketool/pokeicon.h"
#include "poketool/boxdata.h"
#include "system/bmp_menu.h"
#include "system/snd_tool.h"
#include "gflib/strbuf_family.h"

#include "codein_pv.h"

//--------------------------------------------------------------
/**
 * @brief	CATS の 初期化
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_disp_CATS_Init( CODEIN_WORK* wk )
{
	wk->sys.csp = CATS_AllocMemory( HEAPID_CODEIN );
	{
		const TCATS_OAM_INIT OamInit = {
			0, 128, 0, 32,
			0, 128, 0, 32,
		};
		const TCATS_CHAR_MANAGER_MAKE Ccmm = {
			48 + 48,			///< ID管理 main + sub
			1024 * 0x40,		///< 64k
			 512 * 0x20,		///< 32k
			GX_OBJVRAMMODE_CHAR_1D_64K,
			GX_OBJVRAMMODE_CHAR_1D_32K
		};
		CATS_SystemInit( wk->sys.csp, &OamInit, &Ccmm, 16 + 16 );
	}
	
	{
		BOOL active;
		const TCATS_RESOURCE_NUM_LIST ResourceList = {
			48 + 48,	///< キャラ登録数 	main + sub
			16 + 16,	///< パレット登録数	main + sub
			64,			///< セル
			64,			///< セルアニメ
			16,			///< マルチセル
			16,			///< マルチセルアニメ
		};
		
		wk->sys.crp = CATS_ResourceCreate( wk->sys.csp );

		active   = CATS_ClactSetInit( wk->sys.csp, wk->sys.crp, 64 + 64 );
		GF_ASSERT( active );
		
		active	  = CATS_ResourceManagerInit( wk->sys.csp, wk->sys.crp, &ResourceList );
		GF_ASSERT( active );
	}	
}

//--------------------------------------------------------------
/**
 * @brief	コード用データ読み込み
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_disp_CodeRes_Load( CODEIN_WORK* wk )
{
	ARCHANDLE*		 hdl;
	GF_BGL_INI*		 bgl;
	CATS_SYS_PTR	 csp;
	CATS_RES_PTR	 crp;
	PALETTE_FADE_PTR pfd;

	csp = wk->sys.csp;
	crp = wk->sys.crp;
	pfd = wk->sys.pfd;	
	bgl = wk->sys.bgl;
	hdl = wk->sys.p_handle;

	///< bg
	ArcUtil_HDL_BgCharSet( hdl, NARC_codein_gra_code_bg_NCGR, bgl, GF_BGL_FRAME1_M, 0, 0, 0, HEAPID_CODEIN );
	ArcUtil_HDL_ScrnSet( hdl, NARC_codein_gra_code_bg_NSCR, bgl, GF_BGL_FRAME1_M, 0, 0, 0, HEAPID_CODEIN );
	PaletteWorkSet_Arc( pfd, ARC_CODE_GRA, NARC_codein_gra_code_bg_NCLR, HEAPID_CODEIN, FADE_MAIN_BG, 0x20 * 1, 0 );
	
	///< bg-top
	ArcUtil_HDL_BgCharSet( hdl, NARC_codein_gra_code_bg_t_NCGR, bgl, GF_BGL_FRAME1_S, 0, 0, 0, HEAPID_CODEIN );
	ArcUtil_HDL_ScrnSet( hdl, NARC_codein_gra_code_bg_t_NSCR, bgl, GF_BGL_FRAME1_S, 0, 0, 0, HEAPID_CODEIN );
	PaletteWorkSet_Arc( pfd, ARC_CODE_GRA, NARC_codein_gra_code_bg_t_NCLR, HEAPID_CODEIN, FADE_SUB_BG, 0x20 * 1, 0 );
	
	///< code
	CATS_LoadResourcePlttWorkArcH( pfd, FADE_MAIN_OBJ, csp, crp, hdl, NARC_codein_gra_code_NCLR, FALSE, 1, NNS_G2D_VRAM_TYPE_2DMAIN, eID_CODE_OAM );
	CATS_LoadResourceCharArcH( csp, crp, hdl, NARC_codein_gra_code_NCGR, FALSE, NNS_G2D_VRAM_TYPE_2DMAIN, eID_CODE_OAM );
	CATS_LoadResourceCellArcH( csp, crp, hdl, NARC_codein_gra_code_NCER, FALSE, eID_CODE_OAM );
	CATS_LoadResourceCellAnmArcH( csp, crp, hdl, NARC_codein_gra_code_NANR, FALSE, eID_CODE_OAM );	
	
	///< cur
	CATS_LoadResourcePlttWorkArcH( pfd, FADE_MAIN_OBJ, csp, crp, hdl, NARC_codein_gra_code_cur_NCLR, FALSE, 1, NNS_G2D_VRAM_TYPE_2DMAIN, eID_CUR_OAM );
	CATS_LoadResourceCharArcH( csp, crp, hdl, NARC_codein_gra_code_cur_NCGR, FALSE, NNS_G2D_VRAM_TYPE_2DMAIN, eID_CUR_OAM );
	CATS_LoadResourceCellArcH( csp, crp, hdl, NARC_codein_gra_code_cur_NCER, FALSE, eID_CUR_OAM );
	CATS_LoadResourceCellAnmArcH( csp, crp, hdl, NARC_codein_gra_code_cur_NANR, FALSE, eID_CUR_OAM );	

	///< button
	CATS_LoadResourcePlttWorkArcH( pfd, FADE_MAIN_OBJ, csp, crp, hdl, NARC_codein_gra_code_button_NCLR, FALSE, 2, NNS_G2D_VRAM_TYPE_2DMAIN, eID_BTN_OAM );
	CATS_LoadResourceCharArcH( csp, crp, hdl, NARC_codein_gra_code_button_NCGR, FALSE, NNS_G2D_VRAM_TYPE_2DMAIN, eID_BTN_OAM );
	CATS_LoadResourceCellArcH( csp, crp, hdl, NARC_codein_gra_code_button_NCER, FALSE, eID_BTN_OAM );
	CATS_LoadResourceCellAnmArcH( csp, crp, hdl, NARC_codein_gra_code_button_NANR, FALSE, eID_BTN_OAM );	
	
	///< font
	{
		int wintype = CONFIG_GetWindowType( wk->param.cfg );
		TalkWinGraphicSet( bgl, GF_BGL_FRAME0_S, 1, ePAL_WIN, wintype, HEAPID_CODEIN );
		PaletteWorkSet_Arc( pfd, ARC_WINFRAME, TalkWinPalArcGet( wintype ), HEAPID_CODEIN, FADE_SUB_BG, 0x20, ePAL_FRAME * 16 );

		///< フォント
		PaletteWorkSet_Arc( pfd, ARC_FONT, NARC_font_talk_ncrl, HEAPID_CODEIN, FADE_SUB_BG, 0x20, ePAL_FONT * 16 );
	}
}


//--------------------------------------------------------------
/**
 * @brief	もろもろ解放
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_disp_CodeRes_Delete( CODEIN_WORK* wk )
{
	int i;
	
	for ( i = 0; i < wk->code_max; i++ ){
		CATS_ActorPointerDelete_S( wk->code[ i ].cap );
	}
    
    for ( i = 0; i < wk->unk3ec; i++) {
		CATS_ActorPointerDelete_S( wk->bar[ i ].cap );
    }

	for ( i = 0; i < 2; i++ ){
		CATS_ActorPointerDelete_S( wk->btn[ i ].cap );
	}
	
	for ( i = 0; i < 3; i++ ){
		CATS_ActorPointerDelete_S( wk->cur[ i ].cap );
	}
	
	CI_pv_FontOam_SysDelete( wk );
	
	GF_BGL_BmpWinDel( &wk->sys.win );
	
//	CATS_ResourceDestructor_S( wk->sys.csp, wk->sys.crp );
}

//--------------------------------------------------------------
/**
 * @brief	コードOAM作成
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_disp_CodeOAM_Create( CODEIN_WORK* wk )
{
	int i;
	int i_c = 0;
	int i_b = 0;
	
	TCATS_OBJECT_ADD_PARAM_S coap;
	CATS_SYS_PTR csp;
	CATS_RES_PTR crp;
	PALETTE_FADE_PTR pfd;

	csp = wk->sys.csp;
	crp = wk->sys.crp;
	pfd = wk->sys.pfd;
	
	coap.x			= 0;
	coap.y			= 0;
	coap.z			= 0;		
	coap.anm		= 0;
	coap.pri		= 10;
	coap.d_area		= CATS_D_AREA_MAIN;
	coap.bg_pri		= 0;
	coap.vram_trans = 0;
	coap.pal		= 0;
	coap.id[0]		= eID_CODE_OAM;
	coap.id[1]		= eID_CODE_OAM;
	coap.id[2]		= eID_CODE_OAM;
	coap.id[3]		= eID_CODE_OAM;
	coap.id[4]		= CLACT_U_HEADER_DATA_NONE;
	coap.id[5]		= CLACT_U_HEADER_DATA_NONE;

    {
    unsigned r4;
    for ( r4 = wk->param.unk28, i = wk->unk3f0 - 1; i >= 0; i-- ) {
        wk->code[i].state = (r4 % 10) + 1;
        r4 /= 10;
    }
    }

	for ( i = 0; i < wk->code_max + wk->unk3ec; i++ ){
		if (wk->unk3ec != 0 && i == ( wk->bar[ i_b ].state + i_b + 1 ) ){
			
			wk->bar[ i_b ].cap = CATS_ObjectAdd_S( csp, crp, &coap );

			CATS_ObjectPosSetCap( wk->bar[ i_b ].cap, 72 + ( i * 8 ) + 4, POS_CODE_Y );
			CATS_ObjectAnimeSeqSetCap( wk->bar[ i_b ].cap, eANM_CODE_BAR );
			CATS_ObjectUpdateCap( wk->bar[ i_b ].cap );
			i_b++;
		}
		else {			
			wk->code[ i_c ].cap = CATS_ObjectAdd_S( csp, crp, &coap );

			CATS_ObjectPosSetCap( wk->code[ i_c ].cap, 72 + ( i * 8 ) + 4, POS_CODE_Y );
			CATS_ObjectAnimeSeqSetCap( wk->code[ i_c ].cap, CI_pv_disp_CodeAnimeGet( wk->code[ i_c ].state, wk->code[ i_c ].size ) );
			CATS_ObjectAffineSetCap( wk->code[ i_c ].cap, CLACT_AFFINE_DOUBLE );
			CATS_ObjectUpdateCap( wk->code[ i_c ].cap );
			i_c++;
		}
	}
}
//--------------------------------------------------------------
/**
 * @brief	カーソルOAMの作成
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_disp_CurOAM_Create( CODEIN_WORK* wk )
{	
	TCATS_OBJECT_ADD_PARAM_S coap;
	CATS_SYS_PTR csp;
	CATS_RES_PTR crp;
	PALETTE_FADE_PTR pfd;

	csp = wk->sys.csp;
	crp = wk->sys.crp;
	pfd = wk->sys.pfd;
	
	coap.x			= 0;
	coap.y			= 0;
	coap.z			= 0;		
	coap.anm		= 0;
	coap.pri		= 0;
	coap.d_area		= CATS_D_AREA_MAIN;
	coap.bg_pri		= 0;
	coap.vram_trans = 0;
	coap.pal		= 0;
	coap.id[0]		= eID_CUR_OAM;
	coap.id[1]		= eID_CUR_OAM;
	coap.id[2]		= eID_CUR_OAM;
	coap.id[3]		= eID_CUR_OAM;
	coap.id[4]		= CLACT_U_HEADER_DATA_NONE;
	coap.id[5]		= CLACT_U_HEADER_DATA_NONE;	
	
	wk->cur[ 0 ].cap = CATS_ObjectAdd_S( csp, crp, &coap );
	wk->cur[ 1 ].cap = CATS_ObjectAdd_S( csp, crp, &coap );
	wk->cur[ 2 ].cap = CATS_ObjectAdd_S( csp, crp, &coap );
	
	CI_pv_disp_CurBar_PosSet( wk, wk->unk3f0 );
	CATS_ObjectAnimeSeqSetCap( wk->cur[ 0 ].cap, 0 );
	CATS_ObjectUpdateCap( wk->cur[ 0 ].cap );
	
	wk->cur[ 1 ].move_wk.x = 0;
	wk->cur[ 1 ].move_wk.y = 0;
	wk->cur[ 1 ].state = eANM_CUR_SQ;
	CI_pv_disp_CurSQ_PosSet( wk, 0 );
	CATS_ObjectAnimeSeqSetCap( wk->cur[ 1 ].cap, wk->cur[ 1 ].state );
	CATS_ObjectUpdateCap( wk->cur[ 1 ].cap );
	CATS_ObjectObjModeSetCap( wk->cur[ 1 ].cap, GX_OAM_MODE_XLU );
	
	wk->cur[ 2 ].move_wk.x = 0;
	wk->cur[ 2 ].move_wk.y = 0;
	wk->cur[ 2 ].state = eANM_CUR_SQ;
	CI_pv_disp_CurSQ_PosSet( wk, 0 );
	CATS_ObjectAnimeSeqSetCap( wk->cur[ 2 ].cap, wk->cur[ 2 ].state );
	CATS_ObjectUpdateCap( wk->cur[ 2 ].cap );
	CATS_ObjectObjModeSetCap( wk->cur[ 2 ].cap, GX_OAM_MODE_XLU );
	
	CI_pv_disp_CurOAM_Visible( wk, 1, FALSE );
	CI_pv_disp_CurOAM_Visible( wk, 2, FALSE );
}


//--------------------------------------------------------------
/**
 * @brief	Button OAM 作成
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_disp_BtnOAM_Create( CODEIN_WORK* wk )
{	
	TCATS_OBJECT_ADD_PARAM_S coap;
	CATS_SYS_PTR csp;
	CATS_RES_PTR crp;
	PALETTE_FADE_PTR pfd;

	csp = wk->sys.csp;
	crp = wk->sys.crp;
	pfd = wk->sys.pfd;
	
	coap.x			= 0;
	coap.y			= 0;
	coap.z			= 0;		
	coap.anm		= 0;
	coap.pri		= 10;
	coap.d_area		= CATS_D_AREA_MAIN;
	coap.bg_pri		= 0;
	coap.vram_trans = 0;
	coap.pal		= 0;
	coap.id[0]		= eID_BTN_OAM;
	coap.id[1]		= eID_BTN_OAM;
	coap.id[2]		= eID_BTN_OAM;
	coap.id[3]		= eID_BTN_OAM;
	coap.id[4]		= CLACT_U_HEADER_DATA_NONE;
	coap.id[5]		= CLACT_U_HEADER_DATA_NONE;	
	
	coap.pal		= 0;
	wk->btn[ 0 ].cap = CATS_ObjectAdd_S( csp, crp, &coap );
	
	
	coap.pal		= 1;
	wk->btn[ 1 ].cap = CATS_ObjectAdd_S( csp, crp, &coap );
	{
		s16 x, y;
		
		x = ( wk->sys.rht[ eHRT_BACK ].rect.left + wk->sys.rht[ eHRT_BACK ].rect.right ) / 2;
		y = ( wk->sys.rht[ eHRT_BACK ].rect.top  + wk->sys.rht[ eHRT_BACK ].rect.bottom ) / 2;	

		CATS_ObjectPosSetCap( wk->btn[ 0 ].cap, x, y );
		CATS_ObjectAnimeSeqSetCap( wk->btn[ 0 ].cap, 0 );
		CATS_ObjectUpdateCap( wk->btn[ 0 ].cap );

		x = ( wk->sys.rht[ eHRT_END ].rect.left + wk->sys.rht[ eHRT_END ].rect.right ) / 2;
		y = ( wk->sys.rht[ eHRT_END ].rect.top  + wk->sys.rht[ eHRT_END ].rect.bottom ) / 2;	
		
		CATS_ObjectPosSetCap( wk->btn[ 1 ].cap, x, y );
		CATS_ObjectAnimeSeqSetCap( wk->btn[ 1 ].cap, 0 );
		CATS_ObjectUpdateCap( wk->btn[ 1 ].cap );
	}
}


//--------------------------------------------------------------
/**
 * @brief	カーソルOAMのONOFF
 *
 * @param	wk	
 * @param	no	
 * @param	flag	TRUE = ON
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_disp_CurOAM_Visible( CODEIN_WORK* wk, int no, BOOL flag )
{
	if ( flag == TRUE ){
		
		CATS_ObjectEnableCap( wk->cur[ no ].cap, CATS_ENABLE_TRUE );
	}
	else {
		
		CATS_ObjectEnableCap( wk->cur[ no ].cap, CATS_ENABLE_FALSE );		
	}
}


//--------------------------------------------------------------
/**
 * @brief	カーソルのONOFF取得
 *
 * @param	wk	
 * @param	no	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL CI_pv_disp_CurOAM_VisibleGet( CODEIN_WORK* wk, int no )
{
	int enable;
	
	enable = CATS_ObjectEnableGetCap( wk->cur[ no ].cap );
	
	return enable == CATS_ENABLE_TRUE ? TRUE : FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	アニメ切り替え
 *
 * @param	wk	
 * @param	no	
 * @param	anime	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_disp_CurOAM_AnimeChange( CODEIN_WORK* wk, int no, int anime )
{
	CATS_ObjectAnimeSeqSetCap( wk->cur[ no ].cap, anime );
}


//--------------------------------------------------------------
/**
 * @brief	カーソルの位置を入力位置に
 *
 * @param	wk	
 * @param	id	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_disp_CurBar_PosSet( CODEIN_WORK* wk, int id )
{
	s16 x, y;
	CATS_ACT_PTR cap;
	
    if (id < wk->unk3f0) {
        return;
    }

	cap = wk->code[ id ].cap;
	
	wk->cur[ 0 ].state = id;
	
	CATS_ObjectPosGetCap( cap, &x, &y );
	CATS_ObjectPosSetCap( wk->cur[ 0 ].cap, x, y + CUR_BAR_OY );
}

//--------------------------------------------------------------
/**
 * @brief	カーソル位置をパネルに
 *
 * @param	wk	
 * @param	id	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_disp_CurSQ_PosSet( CODEIN_WORK* wk, int id )
{
	s16 x, y;
	
	x = ( wk->sys.rht[ id + eHRT_NUM_0 ].rect.left + wk->sys.rht[ id + eHRT_NUM_0 ].rect.right ) / 2;
	y = ( wk->sys.rht[ id + eHRT_NUM_0 ].rect.top  + wk->sys.rht[ id + eHRT_NUM_0 ].rect.bottom ) / 2;
	
	CATS_ObjectPosSetCap( wk->cur[ 1 ].cap, x, y );
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
void CI_pv_disp_CurSQ_PosSetEx( CODEIN_WORK* wk, int id, int cur_id )
{
	s16 x, y;
	
	x = ( wk->sys.rht[ id + eHRT_NUM_0 ].rect.left + wk->sys.rht[ id + eHRT_NUM_0 ].rect.right ) / 2;
	y = ( wk->sys.rht[ id + eHRT_NUM_0 ].rect.top  + wk->sys.rht[ id + eHRT_NUM_0 ].rect.bottom ) / 2;
	
	CATS_ObjectPosSetCap( wk->cur[ cur_id ].cap, x, y );
}


//--------------------------------------------------------------
/**
 * @brief	カーソル更新
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_disp_CurUpdate( CODEIN_WORK* wk )
{
	CATS_ObjectUpdateCap( wk->cur[ 0 ].cap );
	CATS_ObjectUpdateCap( wk->cur[ 1 ].cap );
	CATS_ObjectUpdateCap( wk->cur[ 2 ].cap );
	
	{
		
		int id = 2;
		int anime;
		BOOL bAnime;
		
		for ( id = 1; id < 3; id++ ){
			
			anime = CATS_ObjectAnimeSeqGetCap( wk->cur[ id ].cap );
			
			if ( anime == eANM_CUR_TOUCH ){
				
				bAnime = CATS_ObjectAnimeActiveCheckCap( wk->cur[ id ].cap );
				
				if ( bAnime == FALSE ){
					
					CATS_ObjectAnimeSeqSetCap( wk->cur[ id ].cap, wk->cur[ id ].state );
			
					if ( wk->sys.touch == TRUE ){
							CI_pv_disp_CurOAM_Visible( wk, 1, FALSE );
					}
					else {
							CI_pv_disp_CurOAM_Visible( wk, 1, TRUE );
					}
					CI_pv_disp_CurOAM_Visible( wk, 2, FALSE );
				}
			}
			else {
				
				if ( anime != wk->cur[ id ].state ){
					CATS_ObjectAnimeSeqSetCap( wk->cur[ id ].cap, wk->cur[ id ].state );
				}
				{
					anime = CATS_ObjectAnimeSeqGetCap( wk->cur[ 2 ].cap );
					if ( anime != eANM_CUR_TOUCH ){
						if ( wk->sys.touch == TRUE ){
							CI_pv_disp_CurOAM_Visible( wk, 1, FALSE );
						}
						else {
							CI_pv_disp_CurOAM_Visible( wk, 1, TRUE );
						}	
					}
				}
			}
		}
	}
}


//--------------------------------------------------------------
/**
 * @brief	FONTOAM pos set
 *
 * @param	obj	
 * @param	ox	
 * @param	oy	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void FontOamPosSet(FONTOAM_OBJ_PTR obj, int ox, int oy)
{
	int x;
	int y;
	if (obj != NULL){
//		FONTOAM_GetMat(obj, &x, &y);
		FONTOAM_SetMat(obj,  ox,  oy);
	}	
}

// MatchComment: 24 -> 40
#define BTN_FONT_OAM_OX		( 40 )
#define BTN_FONT_OAM_OY		( 7 )
//--------------------------------------------------------------
/**
 * @brief	ボタンのアニメ
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_disp_BtnUpdate( CODEIN_WORK* wk )
{
	int i;
	s16 ox, oy;
	
	for ( i = 0; i < 2; i++ ){
	
		ox = ( wk->sys.rht[ i + eHRT_BACK ].rect.left + wk->sys.rht[ i + eHRT_BACK ].rect.right ) / 2;
		ox -= BTN_FONT_OAM_OX;
		oy = ( wk->sys.rht[ i + eHRT_BACK ].rect.top  + wk->sys.rht[ i + eHRT_BACK ].rect.bottom ) / 2;
		oy -= BTN_FONT_OAM_OY;
		
		switch ( wk->btn[ i ].state ){
		case 0:
			wk->btn[ i ].move_wk.wait = 0;
			break;
			
		case 1:
			wk->btn[ i ].move_wk.wait++;
			if ( wk->btn[ i ].move_wk.wait == 1 ){
				CATS_ObjectAnimeSeqSetCap( wk->btn[ i ].cap, 1 );
				FontOamPosSet( wk->sys.font_obj[ i ], ox, oy-0);
			}
			else if ( wk->btn[ i ].move_wk.wait == 2 ){
				CATS_ObjectAnimeSeqSetCap( wk->btn[ i ].cap, 2 );
				FontOamPosSet( wk->sys.font_obj[ i ], ox, oy-1);
			}
			else if ( wk->btn[ i ].move_wk.wait == 10 ){
				CATS_ObjectAnimeSeqSetCap( wk->btn[ i ].cap, 0 );
				FontOamPosSet( wk->sys.font_obj[ i ], ox, oy+0 );				
				wk->btn[ i ].state++;
			}		
			break;
		
		default:
			CATS_ObjectAnimeSeqSetCap( wk->btn[ i ].cap, 0 );
			FontOamPosSet( wk->sys.font_obj[ i ], ox, oy );
			wk->btn[ i ].state = 0;
			break;
		}
	}
}


//--------------------------------------------------------------
/**
 * @brief	アニメコードを取得
 *
 * @param	state	
 * @param	size	TRUE = Large
 *
 * @retval	int	
 *
 */
//--------------------------------------------------------------
int CI_pv_disp_CodeAnimeGet( int state, BOOL size )
{
	int anime = eANM_CODE_LN;
	
	if ( size == FALSE ){
		
		 anime = eANM_CODE_SN;
	}
	
	anime += state;
	
	return anime;
}

//--------------------------------------------------------------
/**
 * @brief	サイズフラグの設定
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_disp_SizeFlagSet( CODEIN_WORK* wk )
{
	int i;
	
	for ( i = 0; i < wk->code_max; i++ ){
		
		if ( i >= wk->ls && i < wk->le ){
			
			wk->code[ i ].size = TRUE;
		}
		else {
			
			wk->code[ i ].size = FALSE;
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief	移動先の位置を設定する
 *
 * @param	wk	
 * @param	mode	0 = set 1 = move_wk set
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_disp_MovePosSet( CODEIN_WORK* wk, int mode )
{
	int i;
	int bp;
	s16 base;
	s16 x, y;
	
	OS_Printf( "focus %d\n", wk->focus_now );
	base = wk->x_tbl[ wk->focus_now ];
	bp	 = 0;
	
	for ( i = 0; i < wk->code_max; i++ ){
				
		if ( i >= wk->ls && i < wk->le ){
			
			if ( i == wk->ls ){
				base += M_SIZE;
			}
			else {
				base += L_SIZE;
			}
		}
		else {
			if ( i == 0 ){
				base += M_SIZE;
			}
			else {
				base += S_SIZE;
			}			
		}
		
		CATS_ObjectPosGetCap( wk->code[ i ].cap, &x, &y );
		if ( mode == eMPS_SET ){
			
			CATS_ObjectPosSetCap( wk->code[ i ].cap, base, y );										
		}
		else {
			wk->code[ i ].move_wk.x		= ( base - x ) / MOVE_WAIT;
			wk->code[ i ].move_wk.y		= 0;
			wk->code[ i ].move_wk.wait	= MOVE_WAIT;
			wk->code[ i ].move_wk.scale	= 0;
		}
		
		if ( i == wk->bar[ bp ].state && bp != wk->unk3ec ){
			
			CATS_ObjectPosGetCap( wk->bar[ bp ].cap, &x, &y );
			
			if ( wk->ls == wk->le ){
				base += S_SIZE;
			}
			else {
				if ( i > wk->ls && i < wk->le ){
					
					base += M_SIZE;
				}
				else {
					
					base += S_SIZE;
				}
			}
			
			if ( mode == eMPS_SET ){
				
				CATS_ObjectPosSetCap( wk->bar[ bp ].cap, base, y );
			}
			else {
				
				wk->bar[ bp ].move_wk.x		= ( base - x ) / MOVE_WAIT;
				wk->bar[ bp ].move_wk.y		= 0;
				wk->bar[ bp ].move_wk.wait	= MOVE_WAIT;
			}
			bp++;				
		}		
	}
}


//--------------------------------------------------------------
/**
 * @brief	当たり判定の作成
 *
 * @param	wk	
 * @param	no	
 * @param	sx	
 * @param	sy	
 *
 * @retval	static inline void	
 *
 */
//--------------------------------------------------------------
static inline void CODE_HitRectSet( CODEIN_WORK* wk, int no, s16 sx, s16 sy )
{
	s16 x, y;	
	CODE_OAM* code;	
	
	code = &wk->code[ no ];
	
	CATS_ObjectPosGetCap( code->cap, &x, &y );
	
	code->hit->rect.top		= y - sy;
	code->hit->rect.left	= x - sx;
	code->hit->rect.bottom	= y + sy;
	code->hit->rect.right	= x + sx;
}


//--------------------------------------------------------------
/**
 * @brief	当たり判定の設定
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_disp_HitTableSet( CODEIN_WORK* wk )
{
	int i;
	s16 sx;
	s16 sy;
	
	for ( i = 0; i < wk->code_max; i++ ){
				
		if ( i >= wk->ls && i < wk->le ){
			
			sx = L_SIZE / 2;
			sy = L_SIZE / 2;
		}
		else {
			
			sx = S_SIZE / 2;
			sy = S_SIZE;
		}
		
		CODE_HitRectSet( wk, i, sx, sy );
	}
}


#include "include/msgdata/msg_codein.h"

#define	PRINT_COL_BLACK		(GF_PRINTCOLOR_MAKE(15, 13, 2))		// フォントカラー：白
#define	PRINT_COL_NUM		(GF_PRINTCOLOR_MAKE( 1,  2, 0))		// フォントカラー：黒
#define PRINT_NUM_CLEAR		(0x00)
#define PRINT_NUM_CLEAR_2	(0xEE)

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
void CI_pv_FontOam_SysInit(CODEIN_WORK* wk)
{
	wk->sys.fontoam_sys = FONTOAM_SysInit( BTN_OAM_MAX, HEAPID_CODEIN );
	
	FontProc_LoadFont( FONT_BUTTON, HEAPID_CODEIN );
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
void CI_pv_FontOam_SysDelete(CODEIN_WORK* wk)
{
	FontProc_UnloadFont( FONT_BUTTON );
	
	FONTOAM_Delete(wk->sys.font_obj[ 0 ]);
	CharVramAreaFree(&wk->sys.font_vram[ 0 ]);
	
	FONTOAM_Delete(wk->sys.font_obj[ 1 ]);
	CharVramAreaFree(&wk->sys.font_vram[ 1 ]);
	
	FONTOAM_SysDelete(wk->sys.fontoam_sys);			
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
void CI_pv_FontOam_ResourceLoad(CODEIN_WORK* wk)
{
	CATS_LoadResourcePlttWorkArc(wk->sys.pfd, FADE_MAIN_OBJ,
								 wk->sys.csp, wk->sys.crp,
								 ARC_FONT, NARC_font_talk_ncrl, FALSE,
								 1, NNS_G2D_VRAM_TYPE_2DMAIN, eID_FNT_OAM );	
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
void CI_pv_FontOam_Add( CODEIN_WORK* wk )
{
	CI_pv_FontOam_ResourceLoad(wk);
	CI_pv_FontOam_Create(wk, 0,  78, 165, 0);
	CI_pv_FontOam_Create(wk, 1, 172, 165, 0);
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 * @param	flag	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_FontOam_Enable(CODEIN_WORK* wk, BOOL flag)
{
	FONTOAM_SetDrawFlag(wk->sys.font_obj[ 0 ], flag);
	FONTOAM_SetDrawFlag(wk->sys.font_obj[ 1 ], flag);
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 * @param	no	
 * @param	x	
 * @param	y	
 * @param	pal_offset	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_FontOam_Create(CODEIN_WORK* wk, int no, int x, int y, int pal_offset)
{
	s16 ox, oy;
	FONTOAM_INIT	finit;
	STRBUF*			str;
	int				pal_id;
	int				vram_size;
	MSGDATA_MANAGER*		man;
	GF_BGL_BMPWIN 			bmpwin;
	
	
	
	man = MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, NARC_msg_codein_dat, HEAPID_CODEIN );
	str	= MSGMAN_AllocString( man, msg_03 + no );
		
	///< BMP
	{
		///< FONT_BUTTON は 2dotでかい
		GF_BGL_BmpWinInit(&bmpwin);
		GF_BGL_BmpWinObjAdd(wk->sys.bgl, &bmpwin, 10, 2, 0, 0);
		GF_STR_PrintColor(&bmpwin, FONT_BUTTON, str, FontProc_GetPrintCenteredPositionX(FONT_BUTTON, str, 0, 80), 0,
						  MSG_NO_PUT, PRINT_COL_BLACK, NULL);
	}

	pal_id = eID_FNT_OAM;

	vram_size = FONTOAM_NeedCharSize(&bmpwin, NNS_G2D_VRAM_TYPE_2DMAIN, HEAPID_CODEIN);
	CharVramAreaAlloc(vram_size, CHARM_CONT_AREACONT, NNS_G2D_VRAM_TYPE_2DMAIN, &wk->sys.font_vram[ no ]);
	
	ox = ( wk->sys.rht[ no + eHRT_BACK ].rect.left + wk->sys.rht[ no + eHRT_BACK ].rect.right ) / 2;
	ox -= BTN_FONT_OAM_OX;
	oy = ( wk->sys.rht[ no + eHRT_BACK ].rect.top  + wk->sys.rht[ no + eHRT_BACK ].rect.bottom ) / 2;
	oy -= BTN_FONT_OAM_OY;

	finit.fontoam_sys	= wk->sys.fontoam_sys;
	finit.bmp			= &bmpwin;
	finit.clact_set		= CATS_GetClactSetPtr(wk->sys.crp);
	finit.pltt			= CATS_PlttProxy(wk->sys.crp, pal_id);
	finit.parent		= NULL;
	finit.char_ofs		= wk->sys.font_vram[ no ].alloc_ofs;
	finit.x				= ox;
	finit.y				= oy;
	finit.bg_pri		= 0;
	finit.soft_pri		= 0;
	finit.draw_area		= NNS_G2D_VRAM_TYPE_2DMAIN;
	finit.heap			= HEAPID_CODEIN;
	
	wk->sys.font_obj[ no ] = FONTOAM_Init(&finit);
	FONTOAM_SetPaletteOffsetAddTransPlttNo(wk->sys.font_obj[ no ], pal_offset);
//	FONTOAM_SetMat(wk->sys.font_obj[ no ], x, y + );

	STRBUF_Delete(str);
	MSGMAN_Delete(man);
	GF_BGL_BmpWinDel(&bmpwin);
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	bgl	
 * @param	win	
 * @param	frm	
 * @param	x	
 * @param	y	
 * @param	sx	
 * @param	sy	
 * @param	ofs	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
// MatchComment: add argument mes_id
void CI_pv_disp_BMP_WindowAdd( GF_BGL_INI* bgl, GF_BGL_BMPWIN* win, int frm, int x, int y, int sx, int sy, int ofs, int mes_id )
{
	GF_BGL_BmpWinInit( win );
	GF_BGL_BmpWinAdd( bgl, win, frm, x, y, sx, sy, ePAL_FONT, ofs );
					 
	BmpTalkWinWrite( win, WINDOW_TRANS_OFF, 1, ePAL_FRAME );
	
	GF_BGL_BmpWinDataFill( win, FBMP_COL_WHITE );
	GF_BGL_BmpWinOn( win );
	
    // MatchComment: use argument mes_id
	CI_pv_BMP_MsgSet( win, mes_id );
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	win	
 * @param	mes_id	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_BMP_MsgSet( GF_BGL_BMPWIN * win, int mes_id )
{
	MSGDATA_MANAGER* man;
	STRBUF* str;
	
	GF_BGL_BmpWinDataFill(win, FBMP_COL_WHITE);

	man = MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, NARC_msg_codein_dat, HEAPID_CODEIN );
	str	= MSGMAN_AllocString( man, mes_id );
	
	GF_BGL_BmpWinDataFill( win, FBMP_COL_WHITE );
	GF_STR_PrintSimple( win, FONT_TALK, str, 0, 0, 0, NULL );
	GF_BGL_BmpWinOn( win );

	STRBUF_Delete( str );
	MSGMAN_Delete( man );	
}
//==============================================================================
/**
 * @file	email.c
 * @brief	簡単な説明を書く
 * @author	goto
 * @date	2007.10.19(金)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
#include "common.h"
#include "system/procsys.h"
#include "system/main.h"
#include "system/wipe.h"
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
#include "gflib/strbuf.h"
#include "savedata/config.h"
#include "gflib/button_man.h"
#include "system/font_arc.h"

#include "demo/title.h"
#include "include/application/email.h"
#include "email_gra.naix"
#include "email.dat"
#include "msgdata/msg_email.h"

#include "email_snd.h"
#include "communication/wm_icon.h"


//#define HEAPID_EMAIL	( HEAPID_BASE_APP )
#define EMAIL_PRINT_COL	( GF_PRINTCOLOR_MAKE( 1,  2, 0 ) )


///< システム
typedef struct {
	
	ARCHANDLE*			p_handle;			///< アーカイブハンドル
	
	CATS_SYS_PTR		csp;				///< OAMシステム
	CATS_RES_PTR		crp;				///< リソース一括管理

	GF_BGL_INI*			bgl;				///< BGL
	PALETTE_FADE_PTR	pfd;				///< パレットフェード
	
	BUTTON_MAN*			btn;				///< ボタン	
	RECT_HIT_TBL		rht[ EMAIL_BTN_MAX ];	///< 当たり判定(managerに登録する)
	
	BOOL				touch;				///< モード
	
	GF_BGL_BMPWIN		win;				///< 上
	GF_BGL_BMPWIN		info_win;			///< 下
	GF_BGL_BMPWIN		win1[ 2 ];			///< email num
	GF_BGL_BMPWIN		win2[ 5 ];			///< etc a-z
	
	BOOL				info_flag;
	
	PROC_RESULT			proc_return;		
	
} EMAIL_SYS;

///< EMAIL
typedef struct {
	

	EMAIL_SYS 		sys;	
	EMAIL_PARAM*	param;
	
	u16				now;
	u16				max;
	u16				min;
	
	u16				input_num_limit;			///< 追加入力の回数 
	s16				cur_px_limit;				///< カーソル位置
	s16				cur_py_limit;
	
	u16				address[ EMAIL_CODE_MAX ];
	u16				temp[ EMAIL_CODE_MAX ];
	
	int				input_num;					///< 0 = 初回入力 1 = 確認入力
	
	CATS_ACT_PTR	btn_cap[ 2 ];
	CATS_ACT_PTR	cur_cap[ 2 ];
	
	int				cx;
	int				cy;
	
	BOOL			mail_view;
	
} EMAIL_WORK;

static void InfoWinAdd( EMAIL_WORK* wk, int msg_no )
{
	GF_BGL_BMPWIN* win;		
	STRBUF* str;
	MSGDATA_MANAGER* man;
	
	if ( wk->sys.info_flag ){ return; }
	
	wk->sys.info_flag = TRUE;
	man = MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, NARC_msg_email_dat, HEAPID_EMAIL );
	
	win = &wk->sys.info_win;
	GF_BGL_BmpWinInit( win );
	GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME0_M, 2, 10, 28, 4, 14, 1 + MENU_WIN_CGX_SIZ );
	BmpMenuWinWrite( win, WINDOW_TRANS_ON, 1, 13 );	
	
	GF_BGL_BmpWinDataFill( win, 0xFF );
	
	if ( msg_no == eINFO_MAIL_ADDRESS ){
		int num;
		int i;
		int index = 0;
		int half = 25;
		STRCODE code[ 50 + 1 ];
		STRCODE moji[  1 + 1 ];
		STRBUF* temp;
		str	= wk->param->now_str;
		num = STRBUF_GetLen( str );
		OS_Printf( "num = %d\n", num );
		
		STRBUF_GetStringCode( str, code, num + 1 );

		temp = STRBUF_Create( 2, HEAPID_EMAIL );
		
		if ( num < 25 ){
			half = num;
		}
		index = 0;
		for ( i = 0; i < half; i++ ){
			moji[ 0 ] = code[ i ];
			moji[ 1 ] = EOM_;
			STRBUF_Clear( temp );
			STRBUF_SetStringCode( temp, moji );
			GF_STR_PrintExpand( win, FONT_SYSTEM, temp, ( index * 9 ) + 0,  0, 0, EMAIL_PRINT_COL, 0, 0, NULL );
			index++;
		//	GF_STR_PrintExpand( win, FONT_SYSTEM, str, i * 12,  0, 0, EMAIL_PRINT_COL, 0, 0, NULL );
		}
		index = 0;
		for ( ; i < num; i++ ){
			moji[ 0 ] = code[ i ];
			moji[ 1 ] = EOM_;
			STRBUF_Clear( temp );
			STRBUF_SetStringCode( temp, moji );
			GF_STR_PrintExpand( win, FONT_SYSTEM, temp, ( index * 9 ) + 0,  16, 0, EMAIL_PRINT_COL, 0, 0, NULL );
			index++;
		//	GF_STR_PrintExpand( win, FONT_SYSTEM, str, i * 12, 16, 0, EMAIL_PRINT_COL, 0, 0, NULL );
		}
		STRBUF_Delete( temp );
	}
	else {	
		str	= MSGMAN_AllocString( man, msg_email_900 + msg_no );	
		GF_STR_PrintExpand( win, FONT_SYSTEM, str, 0, 0, 0, EMAIL_PRINT_COL, 0, 0, NULL );
		STRBUF_Delete( str );
	}	
	
	GF_BGL_BmpWinOn( win );
	MSGMAN_Delete( man );

	G2_SetBlendBrightness( GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 |GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD, -10 );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
}

static void InfoWinDel( EMAIL_WORK* wk )
{
	if ( wk->sys.info_flag == FALSE ){ return; }
	
	wk->sys.info_flag = FALSE;
	
	BmpMenuWinClear( &wk->sys.info_win, WINDOW_TRANS_ON );
	GF_BGL_BmpWinOff( &wk->sys.info_win );	
	GF_BGL_BmpWinDel( &wk->sys.info_win );	
	
	G2_SetBlendBrightness( GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 |GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD, 0 );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
	
	G2_SetBlendAlpha( GX_BLEND_PLANEMASK_NONE,
					  GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2, 15, 7 );
					 
	G2S_SetBlendAlpha( GX_BLEND_PLANEMASK_NONE,
					   GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 |
					   GX_BLEND_PLANEMASK_BG3, 7, 8 );
}

static void EMail_Ascii_To_Code( char* adrs, u16* code )
{
	STRBUF* str = STRBUF_Create( EMAIL_CODE_MAX, HEAPID_EMAIL );
	
	Email_Ascii_to_Strcode( adrs, str, HEAPID_EMAIL );
	
	STRBUF_GetStringCode( str, code, EMAIL_CODE_MAX );
	
	STRBUF_Delete( str );
}

static void EMail_InfoDraw( EMAIL_WORK* wk, int no )
{
	GF_BGL_BMPWIN* win = &wk->sys.win;
	MSGDATA_MANAGER* man = MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, NARC_msg_email_dat, HEAPID_EMAIL );
	
	STRBUF* str;

	GF_BGL_BmpWinDataFill( win, 0xFF );
	
	if ( no == eINFO_MAIL_ADDRESS ){
		str	= wk->param->now_str;
		GF_STR_PrintExpand( win, FONT_SYSTEM, str, 0, 0, 0, EMAIL_PRINT_COL, 0, 0, NULL );
	}
	else {
		int msg_data[] = {
			msg_email_list_002,
			msg_email_090,
			msg_email_091,
			msg_email_list_001,		
		};
		str	= MSGMAN_AllocString( man, msg_data[ no ] );	
		GF_STR_PrintExpand( win, FONT_SYSTEM, str, 0, 0, 0, EMAIL_PRINT_COL, 0, 0, NULL );
		STRBUF_Delete( str );
	}

	GF_BGL_BmpWinOn( win );
	MSGMAN_Delete( man );
}

static void EMail_Copy_Base( EMAIL_WORK* wk )
{
	STRBUF* str = STRBUF_Create( EMAIL_CODE_MAX, HEAPID_EMAIL );
	
	STRBUF_SetStringCode( str, wk->temp );
	STRBUF_GetStringCode( str, wk->address, EMAIL_CODE_MAX );
	
	STRBUF_Delete( str );
	
	wk->temp[ 0 ] = EOM_;

	GF_BGL_BmpWinDataFill( &wk->sys.win1[ 0 ], 0xFF );
	
	{
		int i = 0;
		s16 x,y;
		u16 code[2];
		
		str = STRBUF_Create( 2, HEAPID_EMAIL );

		while ( wk->address[ i ] != EOM_ ) {
			
			code[ 0 ] = wk->address[ i ];
			code[ 1 ] = EOM_;
		
			x = ( i % EMAIL_CODE_1LINE ) * 8;
			y = ( i / EMAIL_CODE_1LINE ) * 16;
			
			STRBUF_SetStringCode( str, code );
			GF_STR_PrintExpand( &wk->sys.win1[ 0 ], FONT_SYSTEM, str, x, y, 0, EMAIL_PRINT_COL, 0, 1, NULL );
			
			i++;

		};
		
		wk->now = i;
		
		STRBUF_Delete( str );
	}

	GF_BGL_BmpWinOn( &wk->sys.win1[ 0 ] );
}

static void EMail_Copy_Temp( EMAIL_WORK* wk )
{
	STRBUF* str = STRBUF_Create( EMAIL_CODE_MAX, HEAPID_EMAIL );
	
	STRBUF_SetStringCode( str, wk->address );
	STRBUF_GetStringCode( str, wk->temp, EMAIL_CODE_MAX );
	
	STRBUF_Delete( str );
	
	wk->address[ 0 ] = EOM_;
	
	GF_BGL_BmpWinDataFill( &wk->sys.win1[ 0 ], 0xFF );
	GF_BGL_BmpWinOn( &wk->sys.win1[ 0 ] );

	wk->now = 0;
	wk->cx = 0;
	wk->cy = 0;	
}

static void EMail_Code_To_StringPut( EMAIL_WORK* wk, u16* code_src )
{
	STRBUF* str;// = STRBUF_Create( EMAIL_CODE_MAX, HEAPID_EMAIL );
	
	GF_BGL_BmpWinDataFill( &wk->sys.win1[ 0 ], 0xFF );

	{
		int i = 0;
		s16 x,y;
		u16 code[2];
		
		str = STRBUF_Create( 2, HEAPID_EMAIL );

		while ( code_src[ i ] != EOM_ ) {
			
			code[ 0 ] = code_src[ i ];
			code[ 1 ] = EOM_;
		
			x = ( i % EMAIL_CODE_1LINE ) * 8;
			y = ( i / EMAIL_CODE_1LINE ) * 16;
			
			STRBUF_SetStringCode( str, code );
			GF_STR_PrintExpand( &wk->sys.win1[ 0 ], FONT_SYSTEM, str, x, y, 0, EMAIL_PRINT_COL, 0, 1, NULL );
			
			i++;

		};
		
		wk->now = i;
		
		STRBUF_Delete( str );
	}
	
	GF_BGL_BmpWinOn( &wk->sys.win1[ 0 ] );
}

//==============================================================
// Prototype
//==============================================================
static void EMail_Oam_Delete( EMAIL_WORK* wk );
static void EMail_Oam_Update( EMAIL_WORK* wk );
static void EMail_Oam_Add( EMAIL_WORK* wk );
static void EM_VBlank( void* work );
static void EM_VramBankSet( GF_BGL_INI* bgl );
static u16  EMail_CodeGet( int button );
static int  EMail_PosMove_CodeGet( EMAIL_WORK* wk );
static void EMail_CodeTo_PosGet( int code, int* x, int* y );
static BOOL EMail_IsData( int mode, s16 px, s16 py );
static void EMail_KeyInput( EMAIL_WORK* wk );
static void EMail_ButtonCallBack( u32 button, u32 event, void* work );
static BOOL EMail_EndCheck( EMAIL_WORK* wk, int end_set );
static void EMail_Code_Print( GF_BGL_BMPWIN *win, const u16 *code );
extern void EM_CATS_Init( EMAIL_WORK* wk );
extern PROC_RESULT EMail_Proc_Init( PROC* proc, int* seq );
extern PROC_RESULT EMail_Proc_Main( PROC* proc, int* seq );
extern PROC_RESULT EMail_Proc_Exit( PROC* proc, int* seq );

static void EMail_Oam_Delete( EMAIL_WORK* wk )
{
	int i;
	
	for ( i = 0; i < 2; i++ ){
		CATS_ActorPointerDelete_S( wk->btn_cap[ i ] );
		CATS_ActorPointerDelete_S( wk->cur_cap[ i ] );
	}
}

static void EMail_Oam_Update( EMAIL_WORK* wk )
{
	int i;
	
	for ( i = 0; i < 2; i++ ){
		CATS_ObjectUpdateCapEx( wk->btn_cap[ i ] );
		CATS_ObjectUpdateCapEx( wk->cur_cap[ i ] );
	}
	
	///< bar
	{
		s16 x, y;
		int now = wk->now;
		
		if ( now >= wk->max ){
			 now--;
		}		
		x = wk->cur_px_limit + ( now % EMAIL_CODE_1LINE ) * 8;
		y = wk->cur_py_limit + ( now / EMAIL_CODE_1LINE ) * 16;		
		CATS_ObjectPosSetCap( wk->cur_cap[ 1 ], x, y );
	}
	
	///< anime
	{
		if ( CATS_ObjectAnimeSeqGetCap( wk->btn_cap[ 0 ] ) == eEM_ANM_BACK_2 ){
			if ( CATS_ObjectAnimeActiveCheckCap( wk->btn_cap[ 0 ] ) == FALSE ){
				CATS_ObjectAnimeSeqSetCap( wk->btn_cap[ 0 ], 0 );
			}
		}
		if ( CATS_ObjectAnimeSeqGetCap( wk->btn_cap[ 1 ] ) == eEM_ANM_END_2 ){
			if ( CATS_ObjectAnimeActiveCheckCap( wk->btn_cap[ 1 ] ) == FALSE ){
				CATS_ObjectAnimeSeqSetCap( wk->btn_cap[ 1 ], 2 );
			}
		}
		if ( CATS_ObjectAnimeSeqGetCap( wk->cur_cap[ 0 ] ) == eEM_ANM_CUR_2 ){
			
			CATS_ObjectObjModeSetCap( wk->cur_cap[ 0 ], GX_OAM_MODE_XLU );
			
			if ( CATS_ObjectAnimeActiveCheckCap( wk->cur_cap[ 0 ] ) == FALSE ){
				CATS_ObjectAnimeSeqSetCap( wk->cur_cap[ 0 ], eEM_ANM_CUR_1 );
				
				CATS_ObjectObjModeSetCap( wk->cur_cap[ 0 ], GX_OAM_MODE_NORMAL );
				
				if ( wk->sys.touch == TRUE ){
					CATS_ObjectEnableCap( wk->cur_cap[ 0 ], FALSE );
				}
				
				///< bug231
				if ( wk->now == wk->max ){
					wk->cx = EMAIL_CUR_END_PX;
					wk->cy = EMAIL_CUR_END_PY;
					if ( wk->sys.touch == TRUE ){
						 CATS_ObjectEnableCap( wk->cur_cap[0], FALSE );
					}	
				}
			}
		}
	}
}

static void EMail_Oam_Add( EMAIL_WORK* wk )
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
	
	CATS_LoadResourcePlttWorkArcH( pfd, FADE_MAIN_OBJ, csp, crp, hdl, NARC_email_gra_email_oam_NCLR, FALSE, 1, NNS_G2D_VRAM_TYPE_2DMAIN, 0 );
	CATS_LoadResourceCharArcH(csp, crp, hdl, NARC_email_gra_email_oam_NCGR, FALSE, NNS_G2D_VRAM_TYPE_2DMAIN, 0 );
	CATS_LoadResourceCellArcH( csp, crp, hdl, NARC_email_gra_email_oam_NCER, FALSE, 0 );
	CATS_LoadResourceCellAnmArcH( csp, crp, hdl, NARC_email_gra_email_oam_NANR, FALSE, 0 );
	
	{
		int i;		
		TCATS_OBJECT_ADD_PARAM_S coap;
		s16 btn_pos[][2] = { 
			{  72, 172 }, 
			{ 184, 172 },
		};
		s16 cur_pos[][2] = {
			{  24,  63 },  
			{  23,  22 }, 
		};
		
		coap.x		= 0;
		coap.y		= 0;
		coap.z		= 0;		
		coap.anm	= 0;
		coap.pri	= 0;
		coap.pal	= 0;
		coap.d_area = CATS_D_AREA_MAIN;
		coap.bg_pri = 1;
		coap.vram_trans = 0;
		
		coap.id[4] = CLACT_U_HEADER_DATA_NONE;
		coap.id[5] = CLACT_U_HEADER_DATA_NONE;
		
		coap.id[0] = 0;
		coap.id[1] = 0;
		coap.id[2] = 0;
		coap.id[3] = 0;

		for ( i = 0; i < 2; i++ ){
			coap.x = btn_pos[ i ][ 0 ];
			coap.y = btn_pos[ i ][ 1 ];
			coap.pri = 10;
			wk->btn_cap[ i ] = CATS_ObjectAdd_S( csp, crp, &coap );
		}
		CATS_ObjectAnimeSeqSetCap( wk->btn_cap[ 0 ], eEM_ANM_BACK_1 );
		CATS_ObjectAnimeSeqSetCap( wk->btn_cap[ 1 ], eEM_ANM_END_1 );
		
		for ( i = 0; i < 2; i++ ){
			coap.x = cur_pos[ i ][ 0 ];
			coap.y = cur_pos[ i ][ 1 ];
			coap.pri = 0;
			wk->cur_cap[ i ] = CATS_ObjectAdd_S( csp, crp, &coap );
		}
		CATS_ObjectAnimeSeqSetCap( wk->cur_cap[ 0 ], eEM_ANM_CUR_1 );
		CATS_ObjectAnimeSeqSetCap( wk->cur_cap[ 1 ], eEM_ANM_BAR_2 );
		
		CATS_ObjectEnableCap( wk->cur_cap[ 0 ], FALSE );
	//	CATS_ObjectObjModeSetCap( wk->cur_cap[ 0 ], GX_OAM_MODE_XLU );
	}
}

static void EM_VBlank( void* work )
{
	EMAIL_WORK* wk = work;

	DoVramTransferManager();			///< Vram転送マネージャー実行
	
	CATS_RenderOamTrans();

	PaletteFadeTrans( wk->sys.pfd );
	
	GF_BGL_VBlankFunc( wk->sys.bgl );
	
	OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}

static void EM_VramBankSet( GF_BGL_INI* bgl )
{
	GF_Disp_GX_VisibleControlInit();
	
	//VRAM設定
	{
		GF_BGL_DISPVRAM vramSetTable = {
			GX_VRAM_BG_128_A,				// メイン2DエンジンのBG
			GX_VRAM_BGEXTPLTT_NONE,			// メイン2DエンジンのBG拡張パレット
			GX_VRAM_SUB_BG_32_H,			// サブ2DエンジンのBG
			GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット
			GX_VRAM_OBJ_64_E,				// メイン2DエンジンのOBJ
			GX_VRAM_OBJEXTPLTT_NONE,		// メイン2DエンジンのOBJ拡張パレット
			GX_VRAM_SUB_OBJ_16_I,			// サブ2DエンジンのOBJ
			GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット
			GX_VRAM_TEX_01_BC,				// テクスチャイメージスロット
			GX_VRAM_TEXPLTT_01_FG			// テクスチャパレットスロット
		};
		GF_Disp_SetBank( &vramSetTable );

		//VRAMクリア
		MI_CpuClear32((void*)HW_BG_VRAM, HW_BG_VRAM_SIZE);
		MI_CpuClear32((void*)HW_DB_BG_VRAM, HW_DB_BG_VRAM_SIZE);
		MI_CpuClear32((void*)HW_OBJ_VRAM, HW_OBJ_VRAM_SIZE);
		MI_CpuClear32((void*)HW_DB_OBJ_VRAM, HW_DB_OBJ_VRAM_SIZE);
	}
	
		//メイン画面フレーム設定
	{
		GF_BGL_BGCNT_HEADER TextBgCntDat[] = {
			///<FRAME1_M
			{
				0, 0, 0x0800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01,
				0x01, 0, 0, FALSE
			},
			///<FRAME2_M
			{
				0, 0, 0x0800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x08000, GX_BG_EXTPLTT_01,
				0x02, 0, 0, FALSE
			},
			///<FRAME3_M
			{
				0, 0, 0x0800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
				0x03, 0, 0, FALSE
			},
			{
				0, 0, 0x0800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x1800, GX_BG_CHARBASE_0x18000, GX_BG_EXTPLTT_01,
				0x00, 0, 0, FALSE
			},
		};
		GF_BGL_BGControlSet( bgl, GF_BGL_FRAME1_M, &TextBgCntDat[0], GF_BGL_MODE_TEXT );
		GF_BGL_BGControlSet( bgl, GF_BGL_FRAME2_M, &TextBgCntDat[1], GF_BGL_MODE_TEXT );
		GF_BGL_BGControlSet( bgl, GF_BGL_FRAME3_M, &TextBgCntDat[2], GF_BGL_MODE_TEXT );
		GF_BGL_BGControlSet( bgl, GF_BGL_FRAME0_M, &TextBgCntDat[3], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear( bgl, GF_BGL_FRAME1_M );
		GF_BGL_ScrClear( bgl, GF_BGL_FRAME2_M );
		GF_BGL_ScrClear( bgl, GF_BGL_FRAME3_M );
		GF_BGL_ScrClear( bgl, GF_BGL_FRAME0_M );
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
	}
	
	///< サブ画面フレーム設定
	{
		GF_BGL_BGCNT_HEADER TextBgCntDat[] = {
			{	/// font
				0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
				0x00, 0, 0, FALSE
			},
			{	/// bg
				0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01,
				0x01, 0, 0, FALSE
			},
			{
				0
			},
			{
				0
			},
		#if 0
			{	/// list
				0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
				CB_BG_PRI_SCR_LIST, 0, 0, FALSE
			},
			{	/// edit
				0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01,
				CB_BG_PRI_SCR_EDIT, 0, 0, FALSE
			},
		#endif
		};
		GF_BGL_BGControlSet( bgl, GF_BGL_FRAME0_S, &TextBgCntDat[0], GF_BGL_MODE_TEXT );
		GF_BGL_BGControlSet( bgl, GF_BGL_FRAME1_S, &TextBgCntDat[1], GF_BGL_MODE_TEXT );
		GF_BGL_BGControlSet( bgl, GF_BGL_FRAME2_S, &TextBgCntDat[2], GF_BGL_MODE_TEXT );
		GF_BGL_BGControlSet( bgl, GF_BGL_FRAME3_S, &TextBgCntDat[3], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear( bgl, GF_BGL_FRAME0_S );
		GF_BGL_ScrClear( bgl, GF_BGL_FRAME1_S );
		GF_BGL_ScrClear( bgl, GF_BGL_FRAME2_S );
		GF_BGL_ScrClear( bgl, GF_BGL_FRAME3_S );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );
	}
	
	///< SUB画面をメイン画面にするため
	sys.disp3DSW = DISP_3D_TO_SUB;
	GF_Disp_DispSelect();
	
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
}

void EM_CATS_Init( EMAIL_WORK* wk )
{
	wk->sys.csp = CATS_AllocMemory( HEAPID_EMAIL );
	{
		const TCATS_OAM_INIT OamInit = {
			4, 128 - 4, 0, 32,
			4, 128 - 4, 0, 32,
		};
		const TCATS_CHAR_MANAGER_MAKE Ccmm = {
			48 + 48,			///< ID管理 main + sub
			1024 * 0x40,		///< 64k
			 512 * 0x20,		///< 32k
			GX_OBJVRAMMODE_CHAR_1D_32K,
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
 * @brief	
 *
 * @param	proc	
 * @param	seq	
 *
 * @retval	PROC_RESULT	
 *
 */
//--------------------------------------------------------------
PROC_RESULT EMail_Proc_Init( PROC* proc, int* seq )
{
	EMAIL_WORK* wk;
	
	sys_CreateHeap( HEAPID_BASE_APP, HEAPID_EMAIL, 0x30000 );
	
	wk = PROC_AllocWork( proc, sizeof( EMAIL_WORK ), HEAPID_EMAIL );
	memset( wk, 0, sizeof( EMAIL_WORK ) );
	wk->param = ( ( EMAIL_PARAM* )PROC_GetParentWork( proc ) );
	
	wk->address[ 0 ] = EOM_;
	wk->temp[ 0 ] = EOM_;
	
	///< 初期化
	sys_VBlankFuncChange( NULL, NULL );		///< VBlankセット
	sys_HBlankIntrStop();					///< HBlank割り込み停止
	GF_Disp_GX_VisibleControlInit();
	GF_Disp_GXS_VisibleControlInit();
	GX_SetVisiblePlane( 0 );
	GXS_SetVisiblePlane( 0 );
	
	///< システムワークの作成
	wk->sys.p_handle = ArchiveDataHandleOpen( ARC_EMAIL_GRA, HEAPID_EMAIL );	
	wk->sys.bgl		 = GF_BGL_BglIniAlloc( HEAPID_EMAIL );	
	wk->sys.pfd		 = PaletteFadeInit( HEAPID_EMAIL );
	
	PaletteTrans_AutoSet( wk->sys.pfd, TRUE );
	PaletteFadeWorkAllocSet( wk->sys.pfd, FADE_MAIN_BG,	 0x200, HEAPID_EMAIL );
	PaletteFadeWorkAllocSet( wk->sys.pfd, FADE_SUB_BG,	 0x200, HEAPID_EMAIL );
	PaletteFadeWorkAllocSet( wk->sys.pfd, FADE_MAIN_OBJ, 0x200, HEAPID_EMAIL );
	PaletteFadeWorkAllocSet( wk->sys.pfd, FADE_SUB_OBJ,	 0x200, HEAPID_EMAIL );	

	///< Vram設定
	EM_VramBankSet( wk->sys.bgl );	
	
	///< OAM設定
	EM_CATS_Init( wk );
	
	if ( wk->param->mode == EMAIL_INPUT_NUMBER )
	{
	    CLACT_U_WmIcon_SetReserveAreaCharManager( NNS_G2D_VRAM_TYPE_2DMAIN, GX_OBJVRAMMODE_CHAR_1D_128K );
	    CLACT_U_WmIcon_SetReserveAreaPlttManager( NNS_G2D_VRAM_TYPE_2DMAIN );
	    CLACT_U_WmIcon_SetReserveAreaCharManager( NNS_G2D_VRAM_TYPE_2DSUB, GX_OBJVRAMMODE_CHAR_1D_128K );
	    CLACT_U_WmIcon_SetReserveAreaPlttManager( NNS_G2D_VRAM_TYPE_2DSUB );
		WirelessIconEasy();
		WirelessIconEasy_HoldLCD( TRUE, HEAPID_EMAIL );
		{
			NNSG2dPaletteData *palData;
			void* dat = WirelessIconPlttResGet( HEAPID_EMAIL );

			NNS_G2dGetUnpackedPaletteData( dat, &palData );
			PaletteWorkSet( wk->sys.pfd, palData->pRawData, FADE_MAIN_OBJ, 14 * 16, 32 );	/// last = palofs, palsize
			PaletteWorkSet( wk->sys.pfd, palData->pRawData, FADE_SUB_OBJ,  14 * 16, 32 );	/// last = palofs, palsize

			sys_FreeMemoryEz( dat );
		}
	}
	
	///< 設定初期化
//	CI_pv_ParamInit( wk );
	
	///< Touch Panel設定
	{
		InitTPSystem();
		InitTPNoBuff( 4 );		
//		CI_pv_ButtonManagerInit( wk );
	}
	
	G2_SetBlendAlpha( GX_BLEND_PLANEMASK_NONE,
					  GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2, 15, 7 );
					 
	G2S_SetBlendAlpha( GX_BLEND_PLANEMASK_NONE,
					   GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 |
					   GX_BLEND_PLANEMASK_BG3, 7, 8 );
	
	sys_VBlankFuncChange( EM_VBlank, wk );


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
		
		
		ArcUtil_HDL_BgCharSet( hdl, NARC_email_gra_email_bg_NCGR, bgl, GF_BGL_FRAME2_M, 0, 0, 0, HEAPID_EMAIL );
		switch ( wk->param->mode ){
		case EMAIL_INPUT_ADDRESS:
			ArcUtil_HDL_ScrnSet( hdl, NARC_email_gra_email_bg1_NSCR, bgl, GF_BGL_FRAME2_M,  0, 0, 0, HEAPID_EMAIL );
			break;
		case EMAIL_INPUT_NUMBER:
			ArcUtil_HDL_ScrnSet( hdl, NARC_email_gra_email_bg2_NSCR, bgl, GF_BGL_FRAME2_M,  0, 0, 0, HEAPID_EMAIL );
			break;
		case EMAIL_INPUT_PASSWORD:
		case EMAIL_INPUT_VIEW:
			ArcUtil_HDL_ScrnSet( hdl, NARC_email_gra_email_bg3_NSCR, bgl, GF_BGL_FRAME2_M,  0, 0, 0, HEAPID_EMAIL );
			break;
		default:
			break;
		}
		
		ArcUtil_HDL_BgCharSet( hdl, NARC_email_gra_email_bg_NCGR, bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_EMAIL );
		ArcUtil_HDL_ScrnSet( hdl, NARC_email_gra_email_bg0_NSCR, bgl, GF_BGL_FRAME3_M,  0, 0, 0, HEAPID_EMAIL );
		PaletteWorkSet_Arc( pfd, ARC_EMAIL_GRA, NARC_email_gra_email_bg_NCLR, HEAPID_EMAIL, FADE_MAIN_BG, 0x20 * 2, 0 );

		ArcUtil_HDL_BgCharSet( hdl, NARC_email_gra_email_bg_NCGR, bgl, GF_BGL_FRAME1_S, 0, 0, 0, HEAPID_EMAIL );
		ArcUtil_HDL_ScrnSet( hdl, NARC_email_gra_email_bg0_NSCR, bgl, GF_BGL_FRAME1_S, 0, 0, 0, HEAPID_EMAIL );
		PaletteWorkSet_Arc( pfd, ARC_EMAIL_GRA, NARC_email_gra_email_bg_NCLR, HEAPID_EMAIL, FADE_SUB_BG, 0x20 * 2, 0 );
		
		PaletteWorkSet_Arc( pfd, ARC_FONT, NARC_font_talk_ncrl, HEAPID_EMAIL, FADE_MAIN_BG, 0x20, 14 * 16);
		PaletteWorkSet_Arc( pfd, ARC_FONT, NARC_font_talk_ncrl, HEAPID_EMAIL, FADE_SUB_BG,  0x20, 14 * 16);
					
		MenuWinGraphicSet( bgl, GF_BGL_FRAME0_S, 1, 13, 0, HEAPID_EMAIL );
		PaletteWorkSet_Arc( pfd, ARC_WINFRAME, MenuWinPalArcGet(), HEAPID_EMAIL, FADE_SUB_BG, 0x20, 13 * 16);

		MenuWinGraphicSet( bgl, GF_BGL_FRAME0_M, 1, 13, 0, HEAPID_EMAIL );
		PaletteWorkSet_Arc( pfd, ARC_WINFRAME, MenuWinPalArcGet(), HEAPID_EMAIL, FADE_MAIN_BG, 0x20, 13 * 16);
		
		FontProc_LoadFont( FONT_BUTTON, HEAPID_EMAIL );
		
		{
			int i;
			int sx;
			int sy;
			int ofs = 1;
			int win1[][ 4 ] = {
				{ 3,1,25,4 },
				{ 2,7,28,2 },
			};			
			int win2[][ 5 ] = {
				{ 2, 9,28,2 },
				{ 2,11,28,2 },
				{ 2,13,28,2 },
				{ 2,15,28,2 },
				{ 2,17,28,2 },
				{ 2,19,28,2 },
			};
			
			GF_BGL_BMPWIN* win;		
			
			win = &wk->sys.win;
			GF_BGL_BmpWinInit( win );
			GF_BGL_BmpWinAdd( bgl, win, GF_BGL_FRAME0_S, 2, 19, 28, 4, 14, 1 + MENU_WIN_CGX_SIZ );
			BmpMenuWinWrite( win, WINDOW_TRANS_ON, 1, 13 );
			
			if ( wk->param->mode == EMAIL_INPUT_VIEW ){
			//	EMail_InfoDraw( wk, eINFO_PASS_INPUT );
				EMail_InfoDraw( wk, wk->param->mode );
			}
			else {
				EMail_InfoDraw( wk, wk->param->mode );
			}
			
			
			switch ( wk->param->mode ){
			case EMAIL_INPUT_NUMBER:
				win1[0][0] = 12;
				win1[0][1] = 2;
				win1[0][2] = 8;
				win1[0][3] = 2;
				break;
			case EMAIL_INPUT_PASSWORD:
			case EMAIL_INPUT_VIEW:
				win1[0][0] = 14;
				win1[0][1] = 2;
				win1[0][2] = 4;
				win1[0][3] = 2;
				break;
			}

			ofs = 1;
			for ( i = 0; i < 2; i++ ){	
				win = &wk->sys.win1[ i ];				
				GF_BGL_BmpWinInit( win );
				GF_BGL_BmpWinAdd( bgl, win, GF_BGL_FRAME1_M, win1[i][0], win1[i][1], win1[i][2], win1[i][3], 14, ofs );
				ofs += ( win1[i][2] * win1[i][3] );
				
				if ( i == 0 ){
					GF_BGL_BmpWinDataFill( win, 0xFF );
				}
				else {
					GF_BGL_BmpWinDataFill( win, 0x00 );
				}
				GF_BGL_BmpWinOn( win );
			}
			
			EMail_Code_Print( &wk->sys.win1[ 1 ], code_num );
			
		//	ofs = 1;
			for ( i = 0; i < 5; i++ ){				
				win = &wk->sys.win2[ i ];
				GF_BGL_BmpWinInit( win );
				GF_BGL_BmpWinAdd( bgl, win, GF_BGL_FRAME1_M, win2[i][0], win2[i][1], win2[i][2], win2[i][3], 14, ofs );
				ofs += ( win2[i][2] * win2[i][3] );
				GF_BGL_BmpWinDataFill( win, 0x00 );
				GF_BGL_BmpWinOn( win );
			}
			
			if ( wk->param->mode == EMAIL_INPUT_ADDRESS ){
				EMail_Code_Print( &wk->sys.win2[ 0 ], code_etc );
				EMail_Code_Print( &wk->sys.win2[ 1 ], code_A_M );
				EMail_Code_Print( &wk->sys.win2[ 2 ], code_N_Z );
				EMail_Code_Print( &wk->sys.win2[ 3 ], code_a_m );
				EMail_Code_Print( &wk->sys.win2[ 4 ], code_n_z );
			}
		}
	}
	
	///< 当たり判定作成
	{
		int i, n, j;
		int len[] = { CODE_num_MAX, CODE_etc_MAX, CODE_A_M_MAX, CODE_N_Z_MAX, CODE_a_m_MAX, CODE_n_z_MAX, CODE_btn_MAX };
		
		i = j = n = 0;
		do {			
			if ( len[ i ] == 0 ){ i++; j = 0; }
			
			wk->sys.rht[ n ].rect.top	 = ( 7 * 8 ) + ( i * 16 );
			wk->sys.rht[ n ].rect.bottom = wk->sys.rht[ n ].rect.top + 16;
			wk->sys.rht[ n ].rect.left	 = ( 2 * 8 ) + ( j * 16 );
			wk->sys.rht[ n ].rect.right  = wk->sys.rht[ n ].rect.left + 16;
			
			n++;
			j++;
			len[ i ]--;
			
		} while ( i < 6 );
		
		n = EMAIL_BACK;
			
		wk->sys.rht[ n ].rect.top	 = 172 - 12;
		wk->sys.rht[ n ].rect.bottom = wk->sys.rht[ n ].rect.top + 20;
		wk->sys.rht[ n ].rect.left	 = 72 - 30;
		wk->sys.rht[ n ].rect.right  = wk->sys.rht[ n ].rect.left + 60;
		n++;
		
		wk->sys.rht[ n ].rect.top	 = 172 - 12;
		wk->sys.rht[ n ].rect.bottom = wk->sys.rht[ n ].rect.top  + 20;
		wk->sys.rht[ n ].rect.left	 = 184 - 30;
		wk->sys.rht[ n ].rect.right  = wk->sys.rht[ n ].rect.left + 60;
		n++;
		
		wk->sys.btn = BMN_Create( wk->sys.rht, EMAIL_BTN_MAX, EMail_ButtonCallBack, wk, HEAPID_EMAIL );
		
		wk->now = 0;
		
		switch ( wk->param->mode ){
		case EMAIL_INPUT_ADDRESS:
			wk->max = EMAIL_CODE_MAX - 1;
			wk->min = 0;
			wk->cur_px_limit = 23;
			wk->cur_py_limit = 22;
			wk->input_num_limit = 1;
			break;
		case EMAIL_INPUT_NUMBER:
			wk->max = EMAIL_NUM_MAX - 1;
			wk->min = 4;
			wk->cur_px_limit = 128 - 32;
			wk->cur_py_limit = 30;
			wk->input_num_limit = 0;
			break;
		case EMAIL_INPUT_PASSWORD:
			wk->max = EMAIL_PASS_MAX - 1;
			wk->min = 0;
			wk->cur_px_limit = 112;
			wk->cur_py_limit = 30;
			wk->input_num_limit = 0;
			break;
		case EMAIL_INPUT_VIEW:
			wk->max = EMAIL_PASS_MAX - 1;
			wk->min = 0;
			wk->cur_px_limit = 112;
			wk->cur_py_limit = 30;
			wk->input_num_limit = 0;
			break;
		}
	}
	
	EMail_Oam_Add( wk );
	
	WIPE_SYS_Start( WIPE_PATTERN_WMS,
				    WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN,
				    WIPE_FADE_BLACK,  WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_EMAIL );

	wk->sys.touch = TRUE;
	
	///< param の 確認
	{
		if ( wk->param->mode == EMAIL_INPUT_NUMBER ){
			int i;
			u16 code[ 3 ];		
			int num = wk->param->authenticate_rand_code;
			
			code[ 0 ] = num / 100;
			num %= 100;
			code[ 1 ] = num / 10;
			num %= 10;
			code[ 2 ] = num;
			
			for ( i = 0; i < 3; i++ ){
				wk->address[ i ] = code_num[ code[ i ] ];
			}
			wk->address[ 3 ] = bou_;
			wk->address[ 4 ] = EOM_;
			wk->now = wk->min;
			
			EMail_Code_To_StringPut( wk, wk->address );
		}
	}

	if ( wk->param->mode == EMAIL_INPUT_VIEW ){
		InfoWinAdd( wk, eINFO_PASS_INPUT );
	}
	else {
		InfoWinAdd( wk, wk->param->mode );
	}
				    	
	return PROC_RES_FINISH;
}

static u16  EMail_CodeGet( int button )
{
	int i;
	int no = button;
	u16 code;
	int len[] = { CODE_num_MAX, CODE_etc_MAX, CODE_A_M_MAX, CODE_N_Z_MAX, CODE_a_m_MAX, CODE_n_z_MAX, CODE_btn_MAX };
	
	for ( i = 0; i < 6; i++ ){
		
		if ( no < len[ i ] ){ break; }
		
		no -= len[ i ];
	}
	
	if ( button < 10 ){
		code = code_num[ no ];
	}
	else if ( button < 15 ){
		code = code_etc[ no ];
	}
	else if ( button < 28 ){
		code = code_A_M[ no ];
	}
	else if ( button < 41 ){
		code = code_N_Z[ no ];
	}
	else if ( button < 54 ){
		code = code_a_m[ no ];
	}
	else if ( button < 67 ){
		code = code_n_z[ no ];
	}
	
	return code;
}

static int  EMail_PosMove_CodeGet( EMAIL_WORK* wk )
{
	int len[] = { CODE_num_MAX, CODE_etc_MAX, CODE_A_M_MAX, CODE_N_Z_MAX, CODE_a_m_MAX, CODE_n_z_MAX, CODE_btn_MAX };
	int code = 0;
	int i;
	
	if ( wk->cy == NELEMS( len ) - 1 ){
		
		if ( wk->cx >= 0 && wk->cx <= 5 ){
			return EMAIL_BACK;
		}
		if ( wk->cx >= 7 && wk->cx <= 12 ){
			return EMAIL_END;
		}		
	}
	
	for ( i = 0; i < NELEMS( len ); i++ ){
		
		if ( i == wk->cy ){ break; }
		
		code += len[ i ];
	}

	code += wk->cx;

	return code;
}

static void EMail_CodeTo_PosGet( int code, int* x, int* y )
{
	int i;
	int num;
	int len[] = { CODE_num_MAX, CODE_etc_MAX, CODE_A_M_MAX, CODE_N_Z_MAX, CODE_a_m_MAX, CODE_n_z_MAX, CODE_btn_MAX };
	
	num = len[ 0 ];
	for ( i = 1; i < NELEMS( len ); i++ ){
		if ( code < num ){ break; }
		num += len[ i ];
	}
	
	*x = -( num - code - len[ i - 1 ] );
	*y = i - 1;
	
//	OS_Printf( " x y = %d, %d\n", *x, *y );
}

static BOOL EMail_IsData( int mode, s16 px, s16 py )
{
#define o ( TRUE )
#define x ( FALSE )	
	
	BOOL tbl[][ 13 ] = {
		{ o,o,o,o,o,o,o,o,o,o,x,x,x,},
		{ o,o,o,o,o,x,x,x,x,x,x,x,x,},
		{ o,o,o,o,o,o,o,o,o,o,o,o,o,},
		{ o,o,o,o,o,o,o,o,o,o,o,o,o,},
		{ o,o,o,o,o,o,o,o,o,o,o,o,o,},
		{ o,o,o,o,o,o,o,o,o,o,o,o,o,},
		{ o,o,o,o,o,o,x,o,o,o,o,o,o,},
	};
	BOOL tbl2[][ 13 ] = {
		{ o,o,o,o,o,o,o,o,o,o,x,x,x,},
		{ x,x,x,x,x,x,x,x,x,x,x,x,x,},
		{ x,x,x,x,x,x,x,x,x,x,x,x,x,},
		{ x,x,x,x,x,x,x,x,x,x,x,x,x,},
		{ x,x,x,x,x,x,x,x,x,x,x,x,x,},
		{ x,x,x,x,x,x,x,x,x,x,x,x,x,},
		{ o,o,o,o,o,o,x,o,o,o,o,o,o,},
	};

	if ( mode == EMAIL_INPUT_ADDRESS ){
		return tbl[ py ][ px ];
	}
	return tbl2[ py ][ px ];
	
#undef o
#undef x
}

#define MAIL_INPUT_KEY	( PAD_BUTTON_A | PAD_BUTTON_B | PAD_KEY_UP | PAD_KEY_DOWN | PAD_KEY_LEFT | PAD_KEY_RIGHT )

static void StartButton_Func( EMAIL_WORK* wk )
{
	int button;
	s16 x,y;
	wk->cx = 7;
	wk->cy = 6;
	button = EMail_PosMove_CodeGet( wk );		
	if ( button == EMAIL_BACK || button == EMAIL_END ){
		x = ( wk->sys.rht[ button ].rect.left + wk->sys.rht[ button ].rect.right ) / 2 + EMAIL_OAM_BTN_OX;
		y = ( wk->sys.rht[ button ].rect.top  + wk->sys.rht[ button ].rect.bottom )/ 2 + EMAIL_OAM_BTN_OY;
		CATS_ObjectAnimeSeqCheckSetCap( wk->cur_cap[ 0 ], eEM_ANM_CUR_BIG );	
	}
	CATS_ObjectPosSetCap( wk->cur_cap[ 0 ], x, y );	

	wk->sys.touch = FALSE;
	CATS_ObjectEnableCap( wk->cur_cap[0], TRUE );
}

static void EMail_KeyInput( EMAIL_WORK* wk )
{
	int	 len[] = { CODE_num_MAX, CODE_etc_MAX, CODE_A_M_MAX, CODE_N_Z_MAX, CODE_a_m_MAX, CODE_n_z_MAX, CODE_btn_MAX };
	
	s16 ox = wk->cx;
	s16 oy = wk->cy;
	
	
	if ( sys.trg & MAIL_INPUT_KEY ){
		if ( wk->sys.touch == TRUE ){
			 wk->sys.touch = FALSE;
			 CATS_ObjectObjModeSetCap( wk->cur_cap[ 0 ], GX_OAM_MODE_NORMAL );
			 CATS_ObjectEnableCap( wk->cur_cap[0], TRUE );
			 return;
		}
	}
	if ( sys.trg & PAD_BUTTON_START ){
		CATS_ObjectObjModeSetCap( wk->cur_cap[ 0 ], GX_OAM_MODE_NORMAL );
		StartButton_Func( wk );
		return;
	}
	
	///< 上下の移動
	{
		int i,s,e;
		BOOL bM;
		if ( sys.repeat & PAD_KEY_UP ){
			bM = FALSE;
			s = wk->cy - 1;
			e = 0;
			for ( i = s; i >= e; i-- ){
				bM = EMail_IsData( wk->param->mode, wk->cx, i );
				if ( bM == TRUE ){ break; }
			}
			if ( bM == FALSE ){
				s = CODE_TBL_Y_MAX;
				e = wk->cy;
				for ( i = s; i > e; i-- ){
					bM = EMail_IsData( wk->param->mode, wk->cx, i );
					if ( bM == TRUE ){ break; }
				}				
			}
			wk->cy = i;
		}
		if ( sys.repeat & PAD_KEY_DOWN ){
			bM = FALSE;
			s = wk->cy + 1;
			e = CODE_TBL_Y_MAX + 1;
			for ( i = s; i < e; i++ ){
				bM = EMail_IsData( wk->param->mode, wk->cx, i );
				if ( bM == TRUE ){ break; }
			}
			if ( bM == FALSE ){
				s = 0;
				e = wk->cy;
				for ( i = s; i < e; i++ ){
					bM = EMail_IsData( wk->param->mode, wk->cx, i );
					if ( bM == TRUE ){ break; }
				}				
			}
			wk->cy = i;
		}
	}
	
	///< 左右の移動
	if ( sys.repeat & PAD_KEY_LEFT ){
		if ( len[ wk->cy ] == CODE_btn_MAX ){
			if ( wk->cx <= EMAIL_CUR_BACK_CPX ){
				wk->cx = EMAIL_CUR_END_PX;
			}
			else if ( wk->cx >= EMAIL_CUR_END_CPX ){
				wk->cx = EMAIL_CUR_BACK_PX;
			}
		}
		else {
			( wk->cx - 1 ) >= 0 ? wk->cx-- : ( wk->cx = ( len[ wk->cy ] - 1 ) );
		}
	}
	if ( sys.repeat & PAD_KEY_RIGHT ){
		if ( len[ wk->cy ] == CODE_btn_MAX ){
			if ( wk->cx <= EMAIL_CUR_BACK_CPX ){
				wk->cx = EMAIL_CUR_END_PX;
			}
			else if ( wk->cx >= EMAIL_CUR_END_CPX ){
				wk->cx = EMAIL_CUR_BACK_PX;
			}
		}
		else {
			wk->cx++;
			wk->cx %= len[ wk->cy ];
		}
	}
	
	if ( ox != wk->cx || oy != wk->cy ){
		Snd_SePlay( EM_SE_MOVE );
	}
	
	///< 座標更新
	{
		int button;
		s16 x,y;
		
		button = EMail_PosMove_CodeGet( wk );
		
		if ( button == EMAIL_BACK || button == EMAIL_END ){
			x = ( wk->sys.rht[ button ].rect.left + wk->sys.rht[ button ].rect.right ) / 2 + EMAIL_OAM_BTN_OX;
			y = ( wk->sys.rht[ button ].rect.top  + wk->sys.rht[ button ].rect.bottom )/ 2 + EMAIL_OAM_BTN_OY;
			CATS_ObjectAnimeSeqCheckSetCap( wk->cur_cap[ 0 ], eEM_ANM_CUR_BIG );	
		}
		else {
			x = ( wk->sys.rht[ button ].rect.left + wk->sys.rht[ button ].rect.right ) / 2;
			y = ( wk->sys.rht[ button ].rect.top  + wk->sys.rht[ button ].rect.bottom )/ 2;
			if ( CATS_ObjectAnimeSeqGetCap( wk->cur_cap[ 0 ] ) != eEM_ANM_CUR_2 ){
				CATS_ObjectAnimeSeqCheckSetCap( wk->cur_cap[ 0 ], eEM_ANM_CUR_1 );
			}
		}
		CATS_ObjectPosSetCap( wk->cur_cap[ 0 ], x, y );
	}
	
	///< 決定・キャンセル
	if ( sys.trg & PAD_BUTTON_A ){
		EMail_ButtonCallBack( EMail_PosMove_CodeGet( wk ), BMN_EVENT_TOUCH, wk );
	}
	else if ( sys.trg & PAD_BUTTON_B ){		
		EMail_ButtonCallBack( EMAIL_BACK, BMN_EVENT_TOUCH, wk );
	}
	else if ( sys.trg & PAD_BUTTON_START ){
		StartButton_Func( wk );
	}
}

static void EMail_ButtonCallBack( u32 button, u32 event, void* work )
{
	int touch_btn;
	EMAIL_WORK* wk = work;
	
	if ( !( sys.cont & PAD_BUTTON_L || sys.cont & PAD_BUTTON_R ) ){
		if ( event != BMN_EVENT_TOUCH ){ return; }
	}
	
	if ( GF_TP_GetCont() ){
		if ( wk->sys.touch == FALSE ){
			 wk->sys.touch = TRUE;
			 CATS_ObjectEnableCap( wk->cur_cap[0], FALSE );
		}	
	}
	
	if ( wk->param->mode == EMAIL_INPUT_ADDRESS ){
		touch_btn = EMAIL_BACK;
	}
	else {
		touch_btn = EMAIL_CODE_ETC;
	}

	if ( button < touch_btn )
	{
		int i = 0;
		int w, x, y;
		u16 code[ 2 ];
		STRBUF* str;
		
		if ( wk->now >= ( wk->max ) ){ return; }
		
		CATS_ObjectAnimeSeqSetCap( wk->cur_cap[ 0 ], eEM_ANM_CUR_2 );
		CATS_ObjectEnableCap( wk->cur_cap[0], TRUE );
		
		EMail_CodeTo_PosGet( button, &wk->cx, &wk->cy );
		{			
			x = ( wk->sys.rht[ button ].rect.left + wk->sys.rht[ button ].rect.right ) / 2;
			y = ( wk->sys.rht[ button ].rect.top  + wk->sys.rht[ button ].rect.bottom )/ 2;
			CATS_ObjectPosSetCap( wk->cur_cap[ 0 ], x, y );
		}
		
		str = STRBUF_Create( 2, HEAPID_EMAIL );
		
		wk->address[ wk->now + 0 ] = EMail_CodeGet( button );
		wk->address[ wk->now + 1 ] = EOM_;
		
		code[ 0 ] = wk->address[ wk->now + 0 ];
		code[ 1 ] = wk->address[ wk->now + 1 ];
		
		do {
			x = ( wk->now % EMAIL_CODE_1LINE ) * 8;
			y = ( wk->now / EMAIL_CODE_1LINE ) * 16;
			
			STRBUF_SetStringCode( str, code );
		//	GF_STR_PrintSimple(	&wk->sys.win1[ 0 ], FONT_SYSTEM, str, x, y, 0, NULL );
			GF_STR_PrintExpand( &wk->sys.win1[ 0 ], FONT_SYSTEM, str, x, y, 0, EMAIL_PRINT_COL, 0, 1, NULL );
			
			i++;

		} while ( code[ i ] != EOM_ );
		
		STRBUF_Delete( str );
		
		if ( wk->now < ( wk->max ) ){
			wk->now++;			
			#if 0
			if ( wk->now == wk->max ){
				wk->cx = EMAIL_CUR_END_PX;
				wk->cy = EMAIL_CUR_END_PY;
				if ( wk->sys.touch == TRUE ){
					 CATS_ObjectEnableCap( wk->cur_cap[0], FALSE );
				}	
			}
			#endif
		}
		Snd_SePlay( EM_SE_DEC );
	}
	else {
		if ( button == EMAIL_BACK ){

			int x, y;
			
			if ( wk->now != wk->min ){
				wk->address[ wk->now ] = EOM_;
				wk->now--;
				wk->address[ wk->now ] = EOM_;
			}
			else {
				if ( event != BMN_EVENT_TOUCH ){ return; }			////< 仮
				
				if ( wk->param->mode != EMAIL_INPUT_PASSWORD ){
					if ( wk->input_num == 0 ){					
						EMail_EndCheck( wk, EMAIL_BACK );
						wk->sys.proc_return = PROC_RES_FINISH;
					}
					else {
						wk->input_num--;
						EMail_Copy_Base( wk );					
					//	EMail_InfoDraw( wk, wk->param->mode );
						InfoWinAdd( wk, wk->param->mode );
					}
				}
			}
			
			x = ( wk->now % EMAIL_CODE_1LINE ) * 8;
			y = ( wk->now / EMAIL_CODE_1LINE ) * 16;			
			
			GF_BGL_BmpWinFill( &wk->sys.win1[ 0 ], 0xFF, x, y, 9, 16 );
			GF_BGL_BmpWinOn( &wk->sys.win1[ 0 ] );
			
			CATS_ObjectAnimeSeqSetCap( wk->btn_cap[ 0 ], eEM_ANM_BACK_2 );
			
			Snd_SeStopBySeqNo( EM_SE_NG, 0 );
			Snd_SePlay( EM_SE_BS );
			
			if ( wk->sys.touch ){
				EMail_CodeTo_PosGet( button, &wk->cx, &wk->cy );
			}
		}
		if ( button == EMAIL_END ){
			if ( wk->now == 0 ){
				Snd_SePlay( EM_SE_NG );
			}
			else {		
				if ( wk->input_num != wk->input_num_limit ){
					 wk->input_num++;
					 EMail_Copy_Temp( wk );
				//	 EMail_InfoDraw( wk, eINFO_ADRS_RE_INPUT );
					 InfoWinAdd( wk, eINFO_ADRS_RE_INPUT );
					 Snd_SePlay( EM_SE_END );
				}
				else {							
					if ( EMail_EndCheck( wk, EMAIL_END ) == FALSE ){					
					}
					else {
						Snd_SePlay( EM_SE_END );
						wk->sys.proc_return = PROC_RES_FINISH;
					}
				}
			}			
			CATS_ObjectAnimeSeqSetCap( wk->btn_cap[ 1 ], eEM_ANM_END_2 );
		}		
	}
}

static BOOL EMail_EndCheck( EMAIL_WORK* wk, int end_set )
{
	BOOL bOK = FALSE;
	int comp;
	STRBUF* str1 = STRBUF_Create( EMAIL_CODE_MAX, HEAPID_EMAIL );
	STRBUF* str2 = STRBUF_Create( EMAIL_CODE_MAX, HEAPID_EMAIL );
	int len1;
	int len2;
	
	///< メールアドレス入力
	if ( wk->param->mode == EMAIL_INPUT_ADDRESS ){
		
		STRBUF_SetStringCode( str1, wk->address );
		STRBUF_SetStringCode( str2, wk->temp );
		len1 = STRBUF_GetLen( str1 );
		len2 = STRBUF_GetLen( str2 );
			
		comp = STRBUF_Compare( str1, str2 );
		
		if ( comp == 0 ){			
			if ( wk->input_num == wk->input_num_limit ){
				OS_Printf( "１回目 と ２回目 が 一致\n");
				wk->param->ret_address_flag = EMAIL_ADDRESS_RET_SET;
				bOK = TRUE;
			}
			else {
				wk->param->ret_address_flag = EMAIL_ADDRESS_RET_CANCEL;
			}
		}
		else {
			Snd_SePlay( EM_SE_NG );
			InfoWinAdd( wk, eINFO_ADRS_ERR );
			wk->param->ret_address_flag = EMAIL_ADDRESS_RET_CANCEL;
		}
		
		///< 文字列コピー
		if ( wk->param->ret_address_flag == EMAIL_ADDRESS_RET_SET ){
			STRBUF_Copy( wk->param->str, str1 );
		}
	}		
	STRBUF_Delete( str1 );
	STRBUF_Delete( str2 );

	///< 数字入力
	if ( wk->param->mode == EMAIL_INPUT_NUMBER ){
		if ( wk->now == wk->max ){
			wk->param->ret_address_flag = EMAIL_ADDRESS_RET_SET;
			bOK = TRUE;			
			{
				int i;
				int num  = 0;
				int keta = 1000;
				for ( i = wk->min; i < wk->max; i++ ){
					num += ( wk->address[ i ] - n0_ ) * keta;
					keta /= 10;
				}
				wk->param->ret_authenticate_code = num;
				OS_Printf( " 認証コード = %d\n", wk->param->ret_authenticate_code );
			}
		//	Snd_SePlay( EM_SE_DEC );
		}
		else {
			if ( wk->now >= 4 ){
				Snd_SePlay( EM_SE_NG );
			}
			wk->param->ret_authenticate_code = EMAIL_AUTHENTICATE_CODE_CANCEL;
		}
	}
	
	///< パスワード入力
	if ( wk->param->mode == EMAIL_INPUT_PASSWORD ){
		OS_Printf( "%d %d\n",wk->now, wk->max );
		if ( wk->now == wk->max ){
			wk->param->ret_address_flag = EMAIL_ADDRESS_RET_SET;
			bOK = TRUE;		
			{
				int i;
				int num  = 0;
				int keta = 1000;
				for ( i = wk->min; i < wk->max; i++ ){
					num += ( wk->address[ i ] - n0_ ) * keta;
					keta /= 10;
				}
				wk->param->ret_password = num;
				OS_Printf( " パスワード = %d\n", wk->param->ret_password  );
			}
		//	Snd_SePlay( EM_SE_DEC );
		}
		else {
		//	if ( wk->now != 0 ){
				Snd_SePlay( EM_SE_NG );
		//	}
			wk->param->ret_address_flag = EMAIL_AUTHENTICATE_CODE_CANCEL;
		}
	}
	
	///< メール確認 ちょっと特殊
	if ( wk->param->mode == EMAIL_INPUT_VIEW ){
		OS_Printf( "%d %d\n",wk->now, wk->max );
		{
			int i;
			int num  = 0;
			int keta = 1000;
			for ( i = wk->min; i < wk->max; i++ ){
				num += ( wk->address[ i ] - n0_ ) * keta;
				keta /= 10;
			}
			OS_Printf( " パスワード = %d\n = %d", num, wk->param->password );
			
			if ( num == wk->param->password ){
				wk->mail_view = TRUE;
				CATS_ObjectEnableCap( wk->cur_cap[ 0 ], FALSE );
			//	EMail_InfoDraw( wk, eINFO_MAIL_ADDRESS );
				InfoWinAdd( wk, eINFO_MAIL_ADDRESS );
			//	ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_BG,  0xFFFF, 12, 0x0000 );
			//	ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, 0xFFFF, 12, 0x0000 );
				Snd_SePlay( EM_SE_END );
			//	Snd_SePlay( EM_SE_DEC );
			}
			else {
				Snd_SePlay( EM_SE_NG );
				if ( wk->now != 0 ){
				//	EMail_InfoDraw( wk, eINFO_PASS_ERR );
					InfoWinAdd( wk, eINFO_PASS_ERR );
				}
			}
		}
	}
	
	return bOK;
}

static void EMail_Code_Print( GF_BGL_BMPWIN *win, const u16 *code )
{
	int i = 0;
	int w, x;
	u16 word[ 2 ];
	STRBUF* str;
	
	str = STRBUF_Create( 2, HEAPID_EMAIL );
	
	do {
		word[ 0 ] = code[ i ];
		word[ 1 ] = EOM_;
		
		w = FontProc_GetStrWidth( FONT_SYSTEM, word, 0 );
		x = ( i * 16 ) + ( ( 16 - w ) / 2 );
		STRBUF_SetStringCode( str, word );
	//	GF_STR_PrintSimple(	win, FONT_SYSTEM, str, x, 0, 0, NULL );
		GF_STR_PrintExpand( win, FONT_SYSTEM, str, x, 0, 0, EMAIL_PRINT_COL, 0, 1, NULL );
		
		i++;
				
	} while ( code[ i ] != EOM_ );
	
	STRBUF_Delete( str );
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	proc	
 * @param	seq	
 *
 * @retval	PROC_RESULT	
 *
 */
 
//#define EMAIL_DEBUG_ADDRS_SET
#ifdef EMAIL_DEBUG_ADDRS_SET

	static int g_adrs_pos = 0;

#endif

//--------------------------------------------------------------
PROC_RESULT EMail_Proc_Main( PROC* proc, int* seq )
{
	EMAIL_WORK* wk = PROC_GetWork( proc );
	
	wk->sys.proc_return = PROC_RES_CONTINUE;
	
	if ( WIPE_SYS_EndCheck() == FALSE ){ return wk->sys.proc_return; }
		
	if ( wk->mail_view ){		
		if ( sys.trg & PAD_BUTTON_A || GF_TP_GetTrg() == TRUE ){
			Snd_SePlay( EM_SE_BS );
			InfoWinDel( wk );
			wk->sys.proc_return = PROC_RES_FINISH;
		}
		EMail_Oam_Update( wk );
		CATS_Draw( wk->sys.crp );
	}
	else {
		if ( wk->sys.info_flag ){
			if ( sys.trg || GF_TP_GetTrg() == TRUE ){
				Snd_SePlay( EM_SE_BS );
				InfoWinDel( wk );			
			}
			EMail_Oam_Update( wk );
			CATS_Draw( wk->sys.crp );
			return 	PROC_RES_CONTINUE;
		}
	
#ifdef PM_DEBUG	
#ifdef EMAIL_DEBUG_ADDRS_SET
		if ( sys.trg & PAD_BUTTON_L ){
			char gf_adres[][ 51 ] = {
				{ "kawachimaru@gamefreak.co.jp" },
				{ "hitomi_sato@gamefreak.co.jp" },
				{ "matsuda@gamefreak.co.jp" },
				{ "daisuke_gotou@gamefreak.co.jp" },
			};

			EMail_Ascii_To_Code( gf_adres[ g_adrs_pos ], wk->address );

			wk->now = strlen( gf_adres[ g_adrs_pos ] );
			
			EMail_Code_To_StringPut( wk, wk->address );
			
			g_adrs_pos++;
			g_adrs_pos %= NELEMS( gf_adres );
		}
#endif
#endif
		
		EMail_KeyInput( wk );
		BMN_Main( wk->sys.btn );
	}

	EMail_Oam_Update( wk );
	CATS_Draw( wk->sys.crp );
	
	if ( wk->sys.proc_return != PROC_RES_CONTINUE ){			
		WIPE_SYS_Start( WIPE_PATTERN_WMS,
					    WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT,
					    WIPE_FADE_BLACK,  WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_EMAIL );
	}

	return wk->sys.proc_return;
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	proc	
 * @param	seq	
 *
 * @retval	PROC_RESULT	
 *
 */
//--------------------------------------------------------------
PROC_RESULT EMail_Proc_Exit( PROC* proc, int* seq )
{	
	EMAIL_WORK* wk;
	
	wk = PROC_GetWork( proc );
	
	if ( WIPE_SYS_EndCheck() == FALSE ){ return PROC_RES_CONTINUE; }
	
//	CI_pv_disp_CodeRes_Delete( wk );
	
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0,  VISIBLE_OFF );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1,  VISIBLE_OFF );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2,  VISIBLE_OFF );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3,  VISIBLE_OFF );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );
	GF_BGL_BGControlExit( wk->sys.bgl, GF_BGL_FRAME0_M );
	GF_BGL_BGControlExit( wk->sys.bgl, GF_BGL_FRAME1_M );
	GF_BGL_BGControlExit( wk->sys.bgl, GF_BGL_FRAME2_M );
	GF_BGL_BGControlExit( wk->sys.bgl, GF_BGL_FRAME3_M );
	GF_BGL_BGControlExit( wk->sys.bgl, GF_BGL_FRAME0_S );
	GF_BGL_BGControlExit( wk->sys.bgl, GF_BGL_FRAME1_S );
	GF_BGL_BGControlExit( wk->sys.bgl, GF_BGL_FRAME2_S );
	GF_BGL_BGControlExit( wk->sys.bgl, GF_BGL_FRAME3_S );
	sys_FreeMemoryEz( wk->sys.bgl );
	
	PaletteFadeWorkAllocFree( wk->sys.pfd, FADE_MAIN_BG );
	PaletteFadeWorkAllocFree( wk->sys.pfd, FADE_SUB_BG );
	PaletteFadeWorkAllocFree( wk->sys.pfd, FADE_MAIN_OBJ );
	PaletteFadeWorkAllocFree( wk->sys.pfd, FADE_SUB_OBJ );
	PaletteFadeFree( wk->sys.pfd );
	
	ArchiveDataHandleClose( wk->sys.p_handle );
	
	if ( wk->param->mode == EMAIL_INPUT_NUMBER ){
		WirelessIconEasyEnd();
	}
	
	{
		u32 active;
		active = StopTP();
	}
	
	EMail_Oam_Delete( wk );
	
	{
		int i;
		
		for ( i = 0; i < 2; i++ ){
			GF_BGL_BmpWinDel( &wk->sys.win1[ i ] );
		}
		
		for ( i = 0; i < 5; i++ ){
			GF_BGL_BmpWinDel( &wk->sys.win2[ i ] );
		}
		GF_BGL_BmpWinDel( &wk->sys.win );
	}
	
	CATS_ResourceDestructor_S( wk->sys.csp, wk->sys.crp );
	CATS_FreeMemory( wk->sys.csp );
	
	FontProc_UnloadFont( FONT_BUTTON );
	
	BMN_Delete( wk->sys.btn );
	
	PROC_FreeWork( proc );
		
	sys_DeleteHeap( HEAPID_EMAIL );
	
	return PROC_RES_FINISH;
}




//--------------------------------------------------------------
/**
 * @brief	プロセスデータ
 */
//--------------------------------------------------------------
const PROC_DATA EMail_ProcData = {
	EMail_Proc_Init,
	EMail_Proc_Main,
	EMail_Proc_Exit,
	NO_OVERLAY_ID,
};


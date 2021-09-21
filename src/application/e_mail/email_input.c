//==============================================================================
/**
 * @file	email_input.c
 * @brief	簡単な説明を書く
 * @author	matsuda
 * @date	2007.10.19(金)
 */
//==============================================================================
#include "common.h"
#include <dwc.h>
#include "libdpw/dpw_tr.h"
#include "system/procsys.h"
#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "system/clact_tool.h"
#include "system/msgdata_util.h"
#include "system/wordset.h"
#include "msgdata/msg.naix"
#include "system/wipe.h"
#include "system/fontproc.h"
#include "system/lib_pack.h"
#include "system/lib_pack.h"
#include "system/fontoam.h"
#include "system/window.h"
#include "gflib/touchpanel.h"
#include "system/bmp_menu.h"
#include "system/snd_tool.h"
#include "gflib/strbuf_family.h"
#include "savedata/wifilist.h"
#include "savedata/zukanwork.h"
#include "communication/communication.h"


//#include "application/worldtrade.h"
//#include "worldtrade_local.h"

#include "msgdata/msg_wifi_lobby.h"
#include "msgdata/msg_wifi_gtc.h"
#include "msgdata/msg_wifi_system.h"

#include "application/email_main.h"
#include "msgdata/msg_email.h"
#include "system/bmp_list.h"
#include "system/pmfprint.h"
#include "savedata/config.h"
#include "savedata\system_data.h"

#include "../wifi_p2pmatch/wifip2pmatch.naix"			// グラフィックアーカイブ定義


//==============================================================================
//	定数定義
//==============================================================================
enum{
	FUNCSEQ_CONTINUE,		///<処理継続中
	FUNCSEQ_FINISH,			///<入力完了
	FUNCSEQ_CANCEL,			///<キャンセルして終了された
};

#define INPUT_KETA_MAX		(7)

// 会話ウインドウ文字列バッファ長
#define TALK_MESSAGE_BUF_NUM	( 90*2 )

#define EMAIL_MESFRAME_PAL	 ( 10 )
#define EMAIL_MENUFRAME_PAL ( 11 )
#define EMAIL_MESFRAME_CHR	 (  1 )
#define EMAIL_MENUFRAME_CHR ( EMAIL_MESFRAME_CHR + TALK_WIN_CGX_SIZ )
#define EMAIL_TALKFONT_PAL	 ( 13 )

//==============================================================================
//	構造体定義
//==============================================================================
typedef struct{
	EMAIL_SYSWORK *esys;
	int num_code;
	int number[INPUT_KETA_MAX];
	int num_pos;
	int keta_max;
	int num_msgid;
	
	GF_BGL_INI		*bgl;								// GF_BGL_INI
	WORDSET			*WordSet;							// メッセージ展開用ワークマネージャー
	MSGDATA_MANAGER *MsgManager;						// 名前入力メッセージデータマネージャー
	STRBUF			*TalkString;						// 会話メッセージ用
	STRBUF			*ErrorString;
	int				MsgIndex;							// 終了検出用ワーク
	
	// BMPWIN描画周り
	GF_BGL_BMPWIN			MsgWin;					// 会話ウインドウ
	GF_BGL_BMPWIN			SubWin;					// 「レコードコーナー　ぼしゅうちゅう！」など
}EMAIL_INPUT_WORK;


//==============================================================================
//	プロトタイプ宣言
//==============================================================================
PROC_RESULT EmailInput_Init( PROC * proc, int * seq );
PROC_RESULT EmailInput_Main( PROC * proc, int * seq );
PROC_RESULT EmailInput_End(PROC *proc, int *seq);
static void VramBankSet(GF_BGL_INI *bgl);
static void BgExit( GF_BGL_INI * ini );
static void BgGraphicSet( EMAIL_INPUT_WORK * wk );
static void BmpWinInit( EMAIL_INPUT_WORK *wk );
static void BmpWinDelete( EMAIL_INPUT_WORK *wk );
static void Input_MessagePrint( EMAIL_INPUT_WORK *wk, MSGDATA_MANAGER *msgman, int msgno, int wait, u16 dat );
static int NumInputFunc(EMAIL_INPUT_WORK *wk);




//============================================================================================
//	プロセス関数
//============================================================================================
//==============================================================================
/**
 * $brief   世界交換入り口画面初期化
 *
 * @param   wk		
 * @param   seq		
 *
 * @retval  int		
 */
//==============================================================================
PROC_RESULT EmailInput_Init( PROC * proc, int * seq )
{
	EMAIL_INPUT_WORK *wk;

	sys_VBlankFuncChange(NULL, NULL);	// VBlankセット
	sys_HBlankIntrStop();	//HBlank割り込み停止

	GF_Disp_GX_VisibleControlInit();
	GF_Disp_GXS_VisibleControlInit();
	GX_SetVisiblePlane(0);
	GXS_SetVisiblePlane(0);
	GX_SetVisibleWnd(GX_WNDMASK_NONE);
	GXS_SetVisibleWnd(GX_WNDMASK_NONE);
	G2_BlendNone();
	G2S_BlendNone();

	//Eメール画面用ヒープ作成
	sys_CreateHeap( HEAPID_BASE_APP, HEAPID_EMAIL, 0x70000 );
	
	wk = PROC_AllocWork(proc, sizeof(EMAIL_INPUT_WORK), HEAPID_EMAIL );
	MI_CpuClear8(wk, sizeof(EMAIL_INPUT_WORK));

	wk->esys = PROC_GetParentWork(proc);

	wk->bgl = GF_BGL_BglIniAlloc( HEAPID_EMAIL );

	initVramTransferManagerHeap(64, HEAPID_EMAIL);

	sys_KeyRepeatSpeedSet( SYS_KEYREPEAT_SPEED_DEF, SYS_KEYREPEAT_WAIT_DEF );

	//VRAM割り当て設定
	VramBankSet(wk->bgl);

	// タッチパネルシステム初期化
	InitTPSystem();
	InitTPNoBuff(4);
	
	//メッセージマネージャ作成
	wk->WordSet    		 = WORDSET_Create( HEAPID_EMAIL );
	wk->MsgManager       = MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, NARC_msg_email_dat, HEAPID_EMAIL );

	// 文字列バッファ作成
	wk->ErrorString  = STRBUF_Create( TALK_MESSAGE_BUF_NUM, HEAPID_EMAIL );
	wk->TalkString  = STRBUF_Create( TALK_MESSAGE_BUF_NUM, HEAPID_EMAIL );

	// BGグラフィック転送
	BgGraphicSet( wk );

	// BMPWIN確保
	BmpWinInit( wk );

	// Wifi通信アイコン
    WirelessIconEasy();

	// ワイプフェード開始
	WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK, 
		WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_EMAIL );

	// BG面表示ON
	GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG0, VISIBLE_ON );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );

	MsgPrintSkipFlagSet(MSG_SKIP_ON);
	MsgPrintAutoFlagSet(MSG_AUTO_OFF);
	MsgPrintTouchPanelFlagSet(MSG_TP_OFF);

//	sys_VBlankFuncChange(ActinVBlank, apw);

	return PROC_RES_FINISH;
}


//==============================================================================
/**
 * $brief   世界交換入り口画面メイン
 *
 * @param   wk		
 * @param   seq		
 *
 * @retval  int		
 */
//==============================================================================
PROC_RESULT EmailInput_Main( PROC * proc, int * seq )
{
	EMAIL_INPUT_WORK * wk  = PROC_GetWork( proc );
	int ret;
	enum{
		SEQ_INIT,
		SEQ_MAIN,
		SEQ_END,
	};
	
	switch(*seq){
	case SEQ_INIT:
		if(WIPE_SYS_EndCheck() == TRUE){
			switch(wk->esys->sub_process_mode){
			case EMAIL_MODE_INPUT_AUTHENTICATE:
				wk->keta_max = 4;
				wk->num_msgid = msg_email_200;
				break;
			case EMAIL_MODE_INPUT_PASSWORD:
			default:
				wk->keta_max = 4;
				wk->num_msgid = msg_email_201;
				break;
			}
			WORDSET_RegisterNumber(wk->WordSet, 1, Email_AuthenticateRandCodeGet(wk->esys), 3, 
				NUMBER_DISPTYPE_ZERO, NUMBER_CODETYPE_DEFAULT);
			Input_MessagePrint(wk, wk->MsgManager, wk->num_msgid, MSG_ALLPUT, 0);
			*seq = SEQ_MAIN;
		}
		break;
	case SEQ_MAIN:
		// シーケンス遷移で実行
		ret = NumInputFunc(wk);
		switch(ret){
		case FUNCSEQ_CONTINUE:
			break;
		case FUNCSEQ_FINISH:
			{
				u32 code;
				
				code = Email_AuthenticateRandCodeGet(wk->esys) * 10000 + wk->num_code;
				Email_AuthenticateCodeSet(wk->esys, code);
				OS_TPrintf("最終code = %d\n", code);
			}
			*seq= SEQ_END;
			break;
		case FUNCSEQ_CANCEL:
			Email_AuthenticateCodeSet(wk->esys, EMAIL_AUTHENTICATE_CODE_CANCEL);
			OS_TPrintf("キャンセル終了\n");
			*seq= SEQ_END;
			break;
		}
		break;
	case SEQ_END:
		return PROC_RES_FINISH;
	}
	return PROC_RES_CONTINUE;
}


//==============================================================================
/**
 * $brief   世界交換入り口画面終了
 *
 * @param   wk		
 * @param   seq		
 *
 * @retval  int		
 */
//==============================================================================
PROC_RESULT EmailInput_End(PROC *proc, int *seq)
{
	EMAIL_INPUT_WORK * wk  = PROC_GetWork( proc );


	//サブPROCをリストに戻すためにセット
	Email_SubProcessChange( wk->esys, EMAIL_SUBPROC_MENU, 0 );


	// メッセージマネージャー・ワードセットマネージャー解放
	MSGMAN_Delete( wk->MsgManager );
	WORDSET_Delete( wk->WordSet );

	STRBUF_Delete( wk->ErrorString ); 
	STRBUF_Delete( wk->TalkString ); 
	
	BmpWinDelete( wk );
	
	// BG_SYSTEM解放
	sys_FreeMemoryEz( wk->bgl );
	BgExit( wk->bgl );

	sys_VBlankFuncChange( NULL, NULL );		// VBlankセット
	sys_HBlankIntrStop();	//HBlank割り込み停止

	//Vram転送マネージャー削除
	DellVramTransferManager();

	StopTP();		//タッチパネルの終了

	MsgPrintSkipFlagSet(MSG_SKIP_OFF);
	MsgPrintAutoFlagSet(MSG_AUTO_OFF);
	MsgPrintTouchPanelFlagSet(MSG_TP_OFF);

	WirelessIconEasyEnd();

	PROC_FreeWork( proc );				// PROCワーク開放
	sys_DeleteHeap( HEAPID_EMAIL );

	return PROC_RES_FINISH;
}


//--------------------------------------------------------------
/**
 * @brief   Vramバンク設定を行う
 *
 * @param   bgl		BGLデータへのポインタ
 */
//--------------------------------------------------------------
static void VramBankSet(GF_BGL_INI *bgl)
{
	GF_Disp_GX_VisibleControlInit();
	GF_Disp_GXS_VisibleControlInit();
	
	//VRAM設定
	{
		GF_BGL_DISPVRAM vramSetTable = {
			GX_VRAM_BG_128_A,				// メイン2DエンジンのBG
			GX_VRAM_BGEXTPLTT_NONE,			// メイン2DエンジンのBG拡張パレット

			GX_VRAM_SUB_BG_128_C,			// サブ2DエンジンのBG
			GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット

			GX_VRAM_OBJ_64_E,				// メイン2DエンジンのOBJ
			GX_VRAM_OBJEXTPLTT_NONE,		// メイン2DエンジンのOBJ拡張パレット

			GX_VRAM_SUB_OBJ_16_I,			// サブ2DエンジンのOBJ
			GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット

			GX_VRAM_TEX_0_B,				// テクスチャイメージスロット
			GX_VRAM_TEXPLTT_01_FG			// テクスチャパレットスロット
		};
		GF_Disp_SetBank( &vramSetTable );

		//VRAMクリア
		MI_CpuClear32((void*)HW_BG_VRAM, HW_BG_VRAM_SIZE);
		MI_CpuClear32((void*)HW_DB_BG_VRAM, HW_DB_BG_VRAM_SIZE);
		MI_CpuClear32((void*)HW_OBJ_VRAM, HW_OBJ_VRAM_SIZE);
		MI_CpuClear32((void*)HW_DB_OBJ_VRAM, HW_DB_OBJ_VRAM_SIZE);
	}

	// BG SYSTEM
	{
		GF_BGL_SYS_HEADER BGsys_data = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
		};
		GF_BGL_InitBG( &BGsys_data );
	}

	//メイン画面フレーム設定
	{
		GF_BGL_BGCNT_HEADER TextBgCntDat[] = {
			///<FRAME0_M	テキスト面
			{
				0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
				0, 0, 0, FALSE
			},
		};
		GF_BGL_BGControlSet(bgl, GF_BGL_FRAME0_M, &TextBgCntDat[0], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, GF_BGL_FRAME0_M );
		GF_BGL_ScrollSet(bgl, GF_BGL_FRAME0_M, GF_BGL_SCROLL_X_SET, 0);
		GF_BGL_ScrollSet(bgl, GF_BGL_FRAME0_M, GF_BGL_SCROLL_Y_SET, 0);
	}
	//サブ画面フレーム設定
	{
		GF_BGL_BGCNT_HEADER TextBgCntDat[] = {
			///<FRAME0_S	テキスト面
			{
				0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
				0, 0, 0, FALSE
			},
		};
		GF_BGL_BGControlSet(bgl, GF_BGL_FRAME0_S, &TextBgCntDat[0], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, GF_BGL_FRAME0_S );
		GF_BGL_ScrollSet(bgl, GF_BGL_FRAME0_S, GF_BGL_SCROLL_X_SET, 0);
		GF_BGL_ScrollSet(bgl, GF_BGL_FRAME0_S, GF_BGL_SCROLL_Y_SET, 0);
	}

	GF_BGL_ClearCharSet( GF_BGL_FRAME0_M, 32, 0, HEAPID_EMAIL );
	GF_BGL_ClearCharSet( GF_BGL_FRAME0_S, 32, 0, HEAPID_EMAIL );
}

//--------------------------------------------------------------------------------------------
/**
 * BG解放
 *
 * @param	ini		BGLデータ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BgExit( GF_BGL_INI * ini )
{
	GF_BGL_BGControlExit( ini, GF_BGL_FRAME0_S );
	GF_BGL_BGControlExit( ini, GF_BGL_FRAME0_M );

}


//--------------------------------------------------------------------------------------------
/**
 * グラフィックデータセット
 *
 * @param	wk		ポケモンリスト画面のワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BgGraphicSet( EMAIL_INPUT_WORK * wk )
{
	GF_BGL_INI *bgl = wk->bgl;
	ARCHANDLE* p_handle;

	p_handle = ArchiveDataHandleOpen( ARC_WIFIP2PMATCH_GRA, HEAPID_EMAIL );

	// 上下画面ＢＧパレット転送
	ArcUtil_HDL_PalSet(    p_handle, NARC_wifip2pmatch_conect_NCLR, PALTYPE_MAIN_BG, 0, 0,  HEAPID_EMAIL);
	ArcUtil_HDL_PalSet(    p_handle, NARC_wifip2pmatch_conect_NCLR, PALTYPE_SUB_BG,  0, 0,  HEAPID_EMAIL);
	
	// 会話フォントパレット転送
	TalkFontPaletteLoad( PALTYPE_MAIN_BG, EMAIL_TALKFONT_PAL*0x20, HEAPID_EMAIL );
	TalkFontPaletteLoad( PALTYPE_SUB_BG,  EMAIL_TALKFONT_PAL*0x20, HEAPID_EMAIL );

	// 会話ウインドウグラフィック転送
	TalkWinGraphicSet(	bgl, GF_BGL_FRAME0_M, EMAIL_MESFRAME_CHR, 
						EMAIL_MESFRAME_PAL,  CONFIG_GetWindowType(wk->esys->config), HEAPID_EMAIL );

	MenuWinGraphicSet(	bgl, GF_BGL_FRAME0_M, EMAIL_MENUFRAME_CHR,
						EMAIL_MENUFRAME_PAL, 0, HEAPID_EMAIL );



	GF_BGL_BackGroundColorSet( GF_BGL_FRAME0_M, 0 );
	GF_BGL_BackGroundColorSet( GF_BGL_FRAME0_S, 0 );

	ArchiveDataHandleClose( p_handle );
}

#define MENULSIT_X		( 5 )
#define MENULIST_Y		( 5 )
#define MENULIST_SX		( 10 )
#define MENULIST_SY		( 10 )

#define SUB_TEXT_X		(  4 )
#define SUB_TEXT_Y		(  4 )
#define SUB_TEXT_SX		( 23 )
#define SUB_TEXT_SY		( 16 )

#define CONNECT_TEXT_X	(  4 )
#define CONNECT_TEXT_Y	(  1 )
#define CONNECT_TEXT_SX	( 24 )
#define CONNECT_TEXT_SY	(  2 )

// 会話ウインドウ表示位置定義
#define TALK_WIN_X		(  2 )
#define TALK_WIN_Y		( 19 )
#define	TALK_WIN_SX		( 27 )
#define	TALK_WIN_SY		(  4 )

#define TALK_MESSAGE_OFFSET	 ( EMAIL_MENUFRAME_CHR + MENU_WIN_CGX_SIZ )
#define ERROR_MESSAGE_OFFSET ( TALK_MESSAGE_OFFSET   + TALK_WIN_SX*TALK_WIN_SY )
#define TITLE_MESSAGE_OFFSET ( ERROR_MESSAGE_OFFSET  + SUB_TEXT_SX*SUB_TEXT_SY )
#define YESNO_OFFSET 		 ( TITLE_MESSAGE_OFFSET  + CONNECT_TEXT_SX*CONNECT_TEXT_SY )
#define MENULIST_MESSAGE_OFFSET	(ERROR_MESSAGE_OFFSET)	//エラーメッセージと一緒には出ないので ※check YESNO_OFFSETの値を調べて、充分な空きがあるなら、その後ろにする

//------------------------------------------------------------------
/**
 * BMPWIN処理（文字パネルにフォント描画）
 *
 * @param   wk		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void BmpWinInit( EMAIL_INPUT_WORK *wk )
{
	// ---------- メイン画面 ------------------

	// BG0面BMPWIN(エラー説明)ウインドウ確保・描画
	GF_BGL_BmpWinAdd(wk->bgl, &wk->SubWin, GF_BGL_FRAME0_M,
	SUB_TEXT_X, SUB_TEXT_Y, SUB_TEXT_SX, SUB_TEXT_SY, EMAIL_TALKFONT_PAL,  ERROR_MESSAGE_OFFSET );

	GF_BGL_BmpWinDataFill( &wk->SubWin, 0x0000 );

	// BG0面BMP（会話ウインドウ）確保
	GF_BGL_BmpWinAdd(wk->bgl, &wk->MsgWin, GF_BGL_FRAME0_M,
		TALK_WIN_X, 
		TALK_WIN_Y, 
		TALK_WIN_SX, 
		TALK_WIN_SY, EMAIL_TALKFONT_PAL,  TALK_MESSAGE_OFFSET );
	GF_BGL_BmpWinDataFill( &wk->MsgWin, 0x0000 );
}

//------------------------------------------------------------------
/**
 * $brief   確保したBMPWINを解放
 *
 * @param   wk		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void BmpWinDelete( EMAIL_INPUT_WORK *wk )
{
	GF_BGL_BmpWinDel( &wk->MsgWin );
	GF_BGL_BmpWinDel( &wk->SubWin );
}

//------------------------------------------------------------------
/**
 * $brief   会話ウインドウ表示
 *
 * @param   wk		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void Input_MessagePrint( EMAIL_INPUT_WORK *wk, MSGDATA_MANAGER *msgman, int msgno, int wait, u16 dat )
{
	// 文字列取得
	STRBUF *tempbuf;

	// 文字列取得
	tempbuf = MSGMAN_AllocString(  msgman, msgno );

	// WORDSET展開
	WORDSET_ExpandStr( wk->WordSet, wk->TalkString, tempbuf );

	STRBUF_Delete(tempbuf);

	// 会話ウインドウ枠描画
	GF_BGL_BmpWinDataFill( &wk->MsgWin,  0x0f0f );
	BmpTalkWinWrite( &wk->MsgWin, WINDOW_TRANS_ON, EMAIL_MESFRAME_CHR, EMAIL_MESFRAME_PAL );

	// 文字列描画開始
	wk->MsgIndex = GF_STR_PrintSimple( &wk->MsgWin, FONT_TALK, wk->TalkString, 0, 0, wait, NULL);
}


//==============================================================================
//	
//==============================================================================
static int NumInputFunc(EMAIL_INPUT_WORK *wk)
{
	int update = 0;
	int i, k;
	
	if(sys.trg & PAD_BUTTON_A){
		OS_TPrintf("決定した 下4桁 = %d\n", wk->num_code);
		return FUNCSEQ_FINISH;
	}
	if(sys.trg & PAD_BUTTON_B){
		return FUNCSEQ_CANCEL;
	}
	
	if(sys.trg & PAD_KEY_RIGHT){
		wk->num_pos--;
		if(wk->num_pos < 0){
			wk->num_pos = wk->keta_max - 1;
		}
	}
	if(sys.trg & PAD_KEY_LEFT){
		wk->num_pos++;
		if(wk->num_pos >= wk->keta_max){
			wk->num_pos = 0;
		}
	}
	
	if(sys.trg & PAD_KEY_DOWN){
		wk->number[wk->num_pos]--;
		if(wk->number[wk->num_pos] < 0){
			wk->number[wk->num_pos] = 9;
		}
		update = 1;
	}
	if(sys.trg & PAD_KEY_UP){
		wk->number[wk->num_pos]++;
		if(wk->number[wk->num_pos] > 9){
			wk->number[wk->num_pos] = 0;
		}
		update = 1;
	}
	
	if(update){
		wk->num_code = 0;
		k = 1;
		for(i = 0; i < wk->keta_max; i++){
			wk->num_code += wk->number[i] * k;
			k *= 10;
		}

		WORDSET_RegisterNumber(wk->WordSet, 0, wk->num_code, 4, 
				NUMBER_DISPTYPE_ZERO, NUMBER_CODETYPE_DEFAULT);
		Input_MessagePrint(wk, wk->MsgManager, wk->num_msgid, MSG_ALLPUT, 0);
	}
	
	return FUNCSEQ_CONTINUE;
}


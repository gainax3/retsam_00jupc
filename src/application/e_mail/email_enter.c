//==============================================================================
/**
 * @file	email_enter.c
 * @brief	Eメール登録入り口処理
 * @author	matsuda
 * @date	2007.10.17(水)
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



//#include "worldtrade.naix"			// グラフィックアーカイブ定義
#include "../wifi_p2pmatch/wifip2pmatch.naix"			// グラフィックアーカイブ定義

#include "application/email_main.h"
#include "msgdata/msg_email.h"
#include "system/bmp_list.h"
#include "system/pmfprint.h"
#include "savedata/config.h"
#include "savedata/system_data.h"
#include "savedata/email_savedata.h"
#include "email_snd_def.h"

#include "application/connect_anm.h"

#include "system/bmp_cursor.h"
#include "email_gra.naix"

//==============================================================================
//	定数定義
//==============================================================================
///世界交換と同じにしておく(WORLDTRADE_WORDSET_BUFLEN)
#define EMAIL_WORDSET_BUFLEN	( 64 )
// 会話ウインドウ文字列バッファ長
#define TALK_MESSAGE_BUF_NUM	( 90*2 )
#define DWC_ERROR_BUF_NUM		(16*8*2)

#define EMAIL_BG_CUR_PAL	( 9 )
#define EMAIL_MESFRAME_PAL	 ( 10 )
#define EMAIL_MENUFRAME_PAL ( 11 )
#define EMAIL_MESFRAME_CHR	 (  1 )
#define EMAIL_MENUFRAME_CHR ( EMAIL_MESFRAME_CHR + TALK_WIN_CGX_SIZ )
#define EMAIL_TALKFONT_PAL	 ( 13 )

// １秒待つ用の定義
#define WAIT_ONE_SECONDE_NUM	( 30 )

//メッセージ一括描画の場合にセットするメッセージindex
#define EMAIL_MSG_NO_WAIT		(0xff)


///Eメール認証エラーが発生した際のエラーメッセージコード
enum{
	//個人情報登録時のエラー
	EMAIL_ENTRY_ERROR_SEND = -5000,
	EMAIL_ENTRY_ERROR_SENDFAILURE = -5001,
	EMAIL_ENTRY_ERROR_SUCCESS = -5002,
	EMAIL_ENTRY_ERROR_FAILURE = -5003,
	EMAIL_ENTRY_ERROR_INVALIDPARAM = -5004,
	EMAIL_ENTRY_ERROR_SERVERSTATE = -5005,
	
	//登録コード認証時のエラー
	EMAIL_PASS_ERROR_SEND = -5006,
	EMAIL_PASS_ERROR_SENDFAILURE = -5007,
	EMAIL_PASS_ERROR_SUCCESS = -5008,
	EMAIL_PASS_ERROR_FAILURE = -5009,
	EMAIL_PASS_ERROR_INVALIDPARAM = -5010,
	EMAIL_PASS_ERROR_SERVERSTATE = -5011,
};

///強制タイムアウトまでの時間
#define TIMEOUT_TIME			(30*60*2)	//2分


//==============================================================================
//	構造体定義
//==============================================================================
typedef struct{
	EMAIL_SYSWORK *esys;

	GF_BGL_INI		*bgl;								// GF_BGL_INI
	
	int				subprocess_seq;						// サブプログラムシーケンスNO
	int				subprocess_nextseq;					// サブプログラムNEXTシーケンスNO

	int				ConnectErrorNo;						// DWC・またはサーバーからのエラー
	int				ErrorRet;
	int				ErrorCode;
	int				ErrorType;

	// 描画まわりのワーク（主にBMP用の文字列周り）
	WORDSET			*WordSet;							// メッセージ展開用ワークマネージャー
	MSGDATA_MANAGER *MsgManager;						// 名前入力メッセージデータマネージャー
	MSGDATA_MANAGER *MonsNameManager;					// ポケモン名メッセージデータマネージャー
	MSGDATA_MANAGER *LobbyMsgManager;					// 名前入力メッセージデータマネージャー
	MSGDATA_MANAGER *SystemMsgManager;					// Wifiシステムメッセージデータ
	MSGDATA_MANAGER *EmailMsgManager;					// Eメールメッセージデータマネージャー
	STRBUF			*TalkString;						// 会話メッセージ用
	STRBUF			*TitleString;						// タイトルメッセージ用
	STRBUF			*ErrorString;
	int				MsgIndex;							// 終了検出用ワーク


	// BMPWIN描画周り
	GF_BGL_BMPWIN			MsgWin;					// 会話ウインドウ
	GF_BGL_BMPWIN			TitleWin;				// 「レコードコーナー　ぼしゅうちゅう！」など
	GF_BGL_BMPWIN			SubWin;					// 「レコードコーナー　ぼしゅうちゅう！」など
	GF_BGL_BMPWIN			list_bmpwin;			///<メニューリスト作成用BMPWIN

	BMPMENU_WORK			*YesNoMenuWork;
	void*					timeWaitWork;			// 会話ウインドウ横アイコンワーク

	int						wait;

	
	int local_seq;
	int local_wait;
	int local_work;
	BMP_MENULIST_DATA *listmenu;					///<Eメールメニュー
	BMPLIST_WORK *lw;
	
	// 利用規約用
	int 				sub_seq;
	int					info_pos;
	int					info_cur_pos;
	int					info_end;
	int					info_param;
	int					info_wait;
	int					info_count;
	GF_BGL_BMPWIN		info_win;
	GF_BGL_BMPWIN		info_win2;
	BMPCURSOR*			info_cur;
	
	s32 timeout_count;				///<タイムアウトカウンタ
	
	//WIFI接続BGパレットアニメ制御構造体へのポインタ
	CONNECT_BG_PALANM cbp;
}EMAIL_MENU_WORK;


//============================================================================================
//	プロトタイプ宣言
//============================================================================================
PROC_RESULT EmailMenu_Enter_Init( PROC * proc, int * seq );
PROC_RESULT EmailMenu_Enter_Main( PROC * proc, int * seq );
PROC_RESULT EmailMenu_Enter_End(PROC *proc, int *seq);
static void VBlankFunc( void * work );
static void EmailMenu_VramBankSet(GF_BGL_INI *bgl);
static void BgExit( GF_BGL_INI * ini );
static void BgGraphicSet( EMAIL_MENU_WORK * wk );
static void BmpWinInit( EMAIL_MENU_WORK *wk );
static void BmpWinDelete( EMAIL_MENU_WORK *wk );
static void Email_SetNextSeq( EMAIL_MENU_WORK *wk, int to_seq, int next_seq );
static void Email_TimeIconAdd( EMAIL_MENU_WORK *wk );
static void Email_TimeIconDel( EMAIL_MENU_WORK *wk );
static BMPMENU_WORK *Email_BmpWinYesNoMake( GF_BGL_INI *bgl, int y, int yesno_bmp_cgx );
static int Enter_MenuList( EMAIL_MENU_WORK *wk );
static int Enter_AddressCheckProcChange(EMAIL_MENU_WORK *wk);
static int Enter_RecvSelectYesNo(EMAIL_MENU_WORK *wk);
static int Enter_EmailDataInitializeYesNo(EMAIL_MENU_WORK *wk);
static int Enter_AddressEntryStart(EMAIL_MENU_WORK *wk);
static int Enter_AddressInputProcChange(EMAIL_MENU_WORK *wk);
static int Enter_AddressReturn(EMAIL_MENU_WORK *wk);
static int Enter_Start( EMAIL_MENU_WORK *wk);
static int Enter_ConnectYesNoSelect( EMAIL_MENU_WORK *wk );
static int Enter_ForceEndStart( EMAIL_MENU_WORK *wk ) ;
static int Enter_ForceEnd( EMAIL_MENU_WORK *wk );
static int Enter_ForceEndMessage( EMAIL_MENU_WORK *wk );
static int Enter_InternetConnect( EMAIL_MENU_WORK *wk );
static int Enter_InternetConnectWait( EMAIL_MENU_WORK *wk );
static int Enter_WifiConnectionLogin( EMAIL_MENU_WORK *wk );
static int Enter_WifiConnectionLoginWait( EMAIL_MENU_WORK *wk );
static int Enter_DpwTrInit( EMAIL_MENU_WORK *wk );
static int Enter_ServerStart( EMAIL_MENU_WORK *wk );
static int Enter_ServerResult( EMAIL_MENU_WORK *wk );
static int Enter_AuthMailWait(EMAIL_MENU_WORK *wk);
static int Enter_AuthInputProcChange(EMAIL_MENU_WORK *wk);
static int Enter_AuthMailEndYesNo(EMAIL_MENU_WORK *wk);
static int Enter_AuthenticateReturn(EMAIL_MENU_WORK *wk);
static int Enter_PasswordSetup(EMAIL_MENU_WORK *wk);
static int Enter_PasswordSetupCheck(EMAIL_MENU_WORK *wk);
static int Enter_Save(EMAIL_MENU_WORK *wk);
static int Enter_CleanupInet( EMAIL_MENU_WORK *wk );
static int Enter_DwcErrorPrint( EMAIL_MENU_WORK *wk );
static int Enter_ErrorPadWait( EMAIL_MENU_WORK *wk );
static int Enter_End( EMAIL_MENU_WORK *wk);
static int Enter_YesNo( EMAIL_MENU_WORK *wk);
static int Enter_ServerServiceError( EMAIL_MENU_WORK *wk );
static int Enter_ServerServiceEnd( EMAIL_MENU_WORK *wk );
static int Enter_MessageWait( EMAIL_MENU_WORK *wk );
static int Enter_MessageWait1Second( EMAIL_MENU_WORK *wk );
static int Enter_MessageWaitYesNoStart(EMAIL_MENU_WORK *wk);
static int ov98_2249798( EMAIL_MENU_WORK *wk );
static int ov98_22497F8( EMAIL_MENU_WORK *wk );

static void Enter_MessagePrint( EMAIL_MENU_WORK *wk, MSGDATA_MANAGER *msgman, int msgno, int wait, u16 dat );
static int Enter_MessagePrintEndCheck(int msg_index);
static int printCommonFunc( GF_BGL_BMPWIN *win, STRBUF *strbuf, int x, int flag, GF_PRINTCOLOR color, int font );
void Email_TalkPrint( GF_BGL_BMPWIN *win, STRBUF *strbuf, int x, int y, int flag, GF_PRINTCOLOR color );
static void _systemMessagePrint( EMAIL_MENU_WORK *wk, int msgno );
static void errorDisp(EMAIL_MENU_WORK* wk, int type, int code);
static void EmailMenuListAllHeader_CursorCallback(BMPLIST_WORK * wk, u32 param, u8 mode);
static void EmailMenuListNoDataHeader_CursorCallback(BMPLIST_WORK * wk, u32 param, u8 mode);


enum{
	ENTER_MENU_LIST,
	ENTER_ADDRESS_CHECK_PROC_CHANGE,
	ENTER_RECV_SELECT_YESNO,
	ENTER_EMAIL_DATA_INITIALIZE_YESNO,
	ENTER_ADDRESS_ENTRY_START,
	ENTER_ADDRESS_INPUT_PROC_CHANGE,
	ENTER_ADDRESS_RETURN,
	ENTER_START,
	ENTER_CONNECT_YESNO_SELECT,
	ENTER_FORCE_END_START,
	ENTER_FORCE_END,
	ENTER_FORCE_END_MESSAGE,
	ENTER_INTERNET_CONNECT,
	ENTER_INTERNET_CONNECT_WAIT,
	ENTER_WIFI_CONNECTION_LOGIN,
	ENTER_WIFI_CONNECTION_LOGIN_WAIT,
	ENTER_DPWTR_INIT,
	ENTER_SERVER_START,
	ENTER_SERVER_RESULT,
	ENTER_AUTH_MAIL_WAIT,
	ENTER_AUTH_INPUT_PROC_CHANGE,
	ENTER_AUTH_MAIL_END_YESNO,
	ENTER_AUTHENTICATE_RETURN,
	ENTER_PASSWORD_SETUP,
	ENTER_PASSWORD_SETUP_CHECK,
	ENTER_SAVE,
	ENTER_CLEANUP_INET,
	ENTER_DWC_ERROR_PRINT,
	ENTER_ERROR_PAD_WAIT,
	ENTER_END,
	ENTER_CONNECT_END,		//通信したまま終了
	ENTER_YESNO,
	ENTER_SERVER_SERVICE_ERROR,
	ENTER_SERVER_SERVICE_END,
	ENTER_MES_WAIT,
	ENTER_MES_WAIT_1_SECOND,
	ENTER_MES_WAIT_YESNO_START,
	ENTER_UNK_0x25,
	ENTER_UNK_0x26,
};

static int (*Functable[])( EMAIL_MENU_WORK *wk ) = {
	Enter_MenuList,						//ENTER_MENU_LIST,
	Enter_AddressCheckProcChange,		//ENTER_ADDRESS_CHECK_PROC_CHANGE
	Enter_RecvSelectYesNo,				//ENTER_RECV_SELECT_YESNO
	Enter_EmailDataInitializeYesNo,		//ENTER_EMAIL_DATA_INITIALIZE_YESNO
	Enter_AddressEntryStart,			//ENTER_ADDRESS_ENTRY_START
	Enter_AddressInputProcChange,		//ENTER_ADDRESS_INPUT_PROC_CHANGE
	Enter_AddressReturn,				//ENTER_ADDRESS_RETURN
	Enter_Start,                        //ENTER_START,
	Enter_ConnectYesNoSelect,           //ENTER_CONNECT_YESNO_SELECT,
	Enter_ForceEndStart,                //ENTER_FORCE_END_START,
	Enter_ForceEnd,                     //ENTER_FORCE_END,
	Enter_ForceEndMessage,              //ENTER_FORCE_END_MESSAGE,
	Enter_InternetConnect,              //ENTER_INTERNET_CONNECT,
	Enter_InternetConnectWait,          //ENTER_INTERNET_CONNECT_WAIT,
	Enter_WifiConnectionLogin,          //ENTER_WIFI_CONNECTION_LOGIN,
	Enter_WifiConnectionLoginWait,      //ENTER_WIFI_CONNECTION_LOGIN_WAIT,
	Enter_DpwTrInit,                    //ENTER_DPWTR_INIT,
	Enter_ServerStart,                  //ENTER_SERVER_START,
	Enter_ServerResult,                 //ENTER_SERVER_RESULT,
	Enter_AuthMailWait,                 //ENTER_AUTH_MAIL_WAIT,
	Enter_AuthInputProcChange,          //ENTER_AUTH_INPUT_PROC_CHANGE,
	Enter_AuthMailEndYesNo,             //ENTER_AUTH_MAIL_END_YESNO,
	Enter_AuthenticateReturn,           //ENTER_AUTHENTICATE_RETURN,
	Enter_PasswordSetup,                //ENTER_PASSWORD_SETUP,
	Enter_PasswordSetupCheck,           //ENTER_PASSWORD_SETUP_CHECK,
	Enter_Save,                         //ENTER_SAVE,
	Enter_CleanupInet,                  //ENTER_CLEANUP_INET,
	Enter_DwcErrorPrint,                //ENTER_DWC_ERROR_PRINT,
	Enter_ErrorPadWait,                 //ENTER_ERROR_PAD_WAIT,
	Enter_End,                          //ENTER_END,
	Enter_End,                          //ENTER_CONNECT_END,
	Enter_YesNo,                        //ENTER_YESNO,
	Enter_ServerServiceError,           //ENTER_SERVER_SERVICE_ERROR,
	Enter_ServerServiceEnd,             //ENTER_SERVER_SERVICE_END,
	Enter_MessageWait,                  //ENTER_MES_WAIT,
	Enter_MessageWait1Second,           //ENTER_MES_WAIT_1_SECOND,
	Enter_MessageWaitYesNoStart,        //ENTER_MES_WAIT_YESNO_START,
    ov98_2249798,                       //ENTER_UNK_0x25
    ov98_22497F8,                       //ENTER_UNK_0x26
};


//--------------------------------------------------------------
//	メニューリスト：全オープン
//--------------------------------------------------------------
typedef struct{
	u32 str_id;
	u32 param;
}EMAIL_BMPMENULIST;

///Eメールのトップメニューの項目
static const EMAIL_BMPMENULIST EmailMenuList_All[] = {
	{ msg_email_list_001, ENTER_ADDRESS_CHECK_PROC_CHANGE },		//確認
//	{ msg_email_list_002, ENTER_ADDRESS_ENTRY_START },		//アドレス設定
	{ msg_email_list_003, ENTER_RECV_SELECT_YESNO },		//受信設定
	{ msg_email_list_004, ENTER_EMAIL_DATA_INITIALIZE_YESNO },		//消す
	{ msg_email_list_005, ENTER_END },			//もどる
};

#define LIST_MENU_ALL_MAX		(NELEMS(EmailMenuList_All))

///Eメール画面：トップメニューのリスト
static const BMPLIST_HEADER EmailMenuListAllHeader = {
	NULL,			// 表示文字データポインタ
	EmailMenuListAllHeader_CursorCallback,		// カーソル移動ごとのコールバック関数
	NULL,					// 一列表示ごとのコールバック関数
	NULL,
	LIST_MENU_ALL_MAX,	// リスト項目数
	LIST_MENU_ALL_MAX,						// 表示最大項目数
	0,						// ラベル表示Ｘ座標
	8,						// 項目表示Ｘ座標
	0,						// カーソル表示Ｘ座標
	0,						// 表示Ｙ座標
	FBMP_COL_BLACK,			// 文字色
	FBMP_COL_WHITE,			// 背景色
	FBMP_COL_BLK_SDW,		// 文字影色
	0,						// 文字間隔Ｘ
	16,						// 文字間隔Ｙ
	BMPLIST_NO_SKIP,		// ページスキップタイプ
	FONT_SYSTEM,				// 文字指定
	0,						// ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)
};

static const u8 EmailMenuBmpSize[4] = {
	11, 7, 20, 10-2,	//X, Y, SX, SY
};

//--------------------------------------------------------------
//	メニューリスト：アドレス登録していない場合
//--------------------------------------------------------------
///Eメールのトップメニューの項目
static const EMAIL_BMPMENULIST EmailMenuList_NoData[] = {
	{ msg_email_list_002, ENTER_ADDRESS_ENTRY_START },		//アドレス設定
	{ msg_email_list_005, ENTER_END },			//もどる
};

#define LIST_MENU_NODATA_MAX		(NELEMS(EmailMenuList_NoData))

///Eメール画面：トップメニューのリスト
static const BMPLIST_HEADER EmailMenuListNoDataHeader = {
	NULL,			// 表示文字データポインタ
	EmailMenuListNoDataHeader_CursorCallback,	// カーソル移動ごとのコールバック関数
	NULL,					// 一列表示ごとのコールバック関数
	NULL,
	LIST_MENU_NODATA_MAX,	// リスト項目数
	LIST_MENU_NODATA_MAX,						// 表示最大項目数
	0,						// ラベル表示Ｘ座標
	8,						// 項目表示Ｘ座標
	0,						// カーソル表示Ｘ座標
	0,						// 表示Ｙ座標
	FBMP_COL_BLACK,			// 文字色
	FBMP_COL_WHITE,			// 背景色
	FBMP_COL_BLK_SDW,		// 文字影色
	0,						// 文字間隔Ｘ
	16,						// 文字間隔Ｙ
	BMPLIST_NO_SKIP,		// ページスキップタイプ
	FONT_SYSTEM,				// 文字指定
	0,						// ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)
};

static const u8 EmailMenuNoDataBmpSize[4] = {
    // MatchComment: use plat US data
	11, 13, 20, 4,	//X, Y, SX, SY
};

//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------
//	はい・いいえ　ウィンドウ
//--------------------------------------------------------------
// はい・いいえ
#define	BMP_YESNO_PX	( 23 )
#define	BMP_YESNO_PY	( 13 )
#define	BMP_YESNO_SX	( 7 )
#define	BMP_YESNO_SY	( 4 )
#define	BMP_YESNO_PAL	( 13 )

// はい・いいえ(ウインドウ用）
static const BMPWIN_DAT YesNoBmpWin = {
	GF_BGL_FRAME0_M, BMP_YESNO_PX, BMP_YESNO_PY,
	BMP_YESNO_SX, BMP_YESNO_SY, BMP_YESNO_PAL, 
	0, //後で指定する
};

// はい・いいえウインドウのY座標
#define	EMAIL_YESNO_PY2	( 13 )		// 会話ウインドウが２行の時
#define	EMAIL_YESNO_PY1	( 15 )		// 会話ウインドウが１行の時


//==============================================================================
//	定数定義
//==============================================================================
///サブシーケンスの戻り値
enum{
	SUBSEQ_CONTINUE,	///<継続
	SUBSEQ_END,			///<終了
};


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
PROC_RESULT EmailMenu_Enter_Init( PROC * proc, int * seq )
{
	EMAIL_MENU_WORK *wk;

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
	
	wk = PROC_AllocWork(proc, sizeof(EMAIL_MENU_WORK), HEAPID_EMAIL );
	MI_CpuClear8(wk, sizeof(EMAIL_MENU_WORK));

	wk->esys = PROC_GetParentWork(proc);

	wk->bgl = GF_BGL_BglIniAlloc( HEAPID_EMAIL );

	initVramTransferManagerHeap(64, HEAPID_EMAIL);

	sys_KeyRepeatSpeedSet( SYS_KEYREPEAT_SPEED_DEF, SYS_KEYREPEAT_WAIT_DEF );

	//VRAM割り当て設定
	EmailMenu_VramBankSet(wk->bgl);

	// タッチパネルシステム初期化
	InitTPSystem();
	InitTPNoBuff(4);
	
	//メッセージマネージャ作成
	wk->WordSet    		 = WORDSET_CreateEx( 11, EMAIL_WORDSET_BUFLEN, HEAPID_EMAIL );
	wk->MsgManager       = MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, NARC_msg_wifi_gtc_dat, HEAPID_EMAIL );
	wk->LobbyMsgManager  = MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, NARC_msg_wifi_lobby_dat, HEAPID_EMAIL );
	wk->SystemMsgManager = MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, NARC_msg_wifi_system_dat, HEAPID_EMAIL );
	wk->MonsNameManager  = MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, NARC_msg_monsname_dat, HEAPID_EMAIL );
	wk->EmailMsgManager  = MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, NARC_msg_email_dat, HEAPID_EMAIL );

	// 文字列バッファ作成
	wk->TalkString  = STRBUF_Create( TALK_MESSAGE_BUF_NUM, HEAPID_EMAIL );
	wk->ErrorString = STRBUF_Create( DWC_ERROR_BUF_NUM,    HEAPID_EMAIL );
	wk->TitleString = MSGMAN_AllocString( wk->MsgManager, msg_gtc_01_032 );

	// BGグラフィック転送
	BgGraphicSet( wk );

	// BMPWIN確保
	BmpWinInit( wk );

	switch(Email_RecoveryMenuModeGet(wk->esys)){
	case ENTER_INTERNET_CONNECT:	//GSIDを取得してきた
		if(!DWC_CheckInet() && mydwc_checkMyGSID(wk->esys->savedata) == TRUE){
			// 初回wifi接続の際は無条件で接続に
			//if(mydwc_checkMyGSID(wk->esys->savedata) == FALSE){
			// WIFIせつぞくを開始
			//Enter_MessagePrint( wk, wk->LobbyMsgManager, msg_wifilobby_002, 1, 0x0f0f );
			//Email_SetNextSeq( wk, ENTER_MES_WAIT, ENTER_INTERNET_CONNECT );
			//Email_TimeIconAdd(wk);
			wk->subprocess_seq = ENTER_INTERNET_CONNECT;
			// 通信エラー管理のために通信ルーチンをON
			CommStateWifiEMailStart( wk->esys->savedata );	// エラー後タイトルに戻るように専用の関数を作成 080603tomoya 
//			CommStateWifiDPWStart( wk->esys->savedata );
			// Wifi通信アイコン
		    WirelessIconEasy();
		}else{
			wk->subprocess_seq = ENTER_MENU_LIST;
		}
		break;
	case ENTER_AUTHENTICATE_RETURN:
		// Wifi通信アイコン
		wk->timeout_count = 0;
	    WirelessIconEasy();
	    wk->subprocess_seq = Email_RecoveryMenuModeGet(wk->esys);
		break;
	default:
		wk->subprocess_seq = Email_RecoveryMenuModeGet(wk->esys);
		break;
	}
	Email_RecoveryMenuModeSet(wk->esys, 0);	//値クリア

	// ワイプフェード開始
	WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK, 
		WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_EMAIL );

	// BG面表示ON
	GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG0, VISIBLE_ON );
	GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG1, VISIBLE_ON );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
	GF_Disp_GX_VisibleControl(  GX_PLANEMASK_OBJ, VISIBLE_ON );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

	//メイン画面設定
	sys.disp3DSW = DISP_3D_TO_MAIN;
	GF_Disp_DispSelect();

	MsgPrintSkipFlagSet(MSG_SKIP_ON);
	MsgPrintAutoFlagSet(MSG_AUTO_OFF);
	MsgPrintTouchPanelFlagSet(MSG_TP_OFF);

	sys_VBlankFuncChange(VBlankFunc, wk);

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
PROC_RESULT EmailMenu_Enter_Main( PROC * proc, int * seq )
{
	EMAIL_MENU_WORK * wk  = PROC_GetWork( proc );
	int ret, temp_subprocess_seq;
	enum{
		SEQ_INIT,
		SEQ_MAIN,
		SEQ_END,
	};
	
	switch(*seq){
	case SEQ_INIT:
		if(WIPE_SYS_EndCheck() == TRUE){
			*seq = SEQ_MAIN;
		}
		break;
	case SEQ_MAIN:
		// シーケンス遷移で実行
		temp_subprocess_seq = wk->subprocess_seq;
		ret = (*Functable[wk->subprocess_seq])( wk );
		if(temp_subprocess_seq != wk->subprocess_seq){
			wk->local_seq = 0;
			wk->local_work = 0;
			wk->local_wait = 0;
		}
		if(ret == SUBSEQ_END){
			*seq = SEQ_END;
		}
		break;
	case SEQ_END:
		if(WIPE_SYS_EndCheck() == TRUE){
			return PROC_RES_FINISH;
		}
		break;
	}
	
	ConnectBGPalAnm_Main(&wk->cbp);
	
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
PROC_RESULT EmailMenu_Enter_End(PROC *proc, int *seq)
{
	EMAIL_MENU_WORK * wk  = PROC_GetWork( proc );

	ConnectBGPalAnm_End(&wk->cbp);
	
	// メッセージマネージャー・ワードセットマネージャー解放
	MSGMAN_Delete( wk->EmailMsgManager );
	MSGMAN_Delete( wk->MonsNameManager );
	MSGMAN_Delete( wk->SystemMsgManager );
	MSGMAN_Delete( wk->LobbyMsgManager );
	MSGMAN_Delete( wk->MsgManager );
	WORDSET_Delete( wk->WordSet );

	STRBUF_Delete( wk->TitleString ); 
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

//--------------------------------------------------------------------------------------------
/**
 * VBlank関数
 *
 * @param	none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void VBlankFunc( void * work )
{
	EMAIL_MENU_WORK *wk = work;

	// セルアクターVram転送マネージャー実行
	DoVramTransferManager();

	// レンダラ共有OAMマネージャVram転送
	REND_OAMTrans();
	
	GF_BGL_VBlankFunc(wk->bgl);
	
	ConnectBGPalAnm_VBlank(&wk->cbp);

	OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}

//--------------------------------------------------------------
/**
 * @brief   Vramバンク設定を行う
 *
 * @param   bgl		BGLデータへのポインタ
 */
//--------------------------------------------------------------
static void EmailMenu_VramBankSet(GF_BGL_INI *bgl)
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
				2, 0, 0, FALSE
			},
			///<FRAME1_M	背景
			{
				0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x08000, GX_BG_EXTPLTT_01,
				3, 0, 0, FALSE
			},
			///<FRAME2_M	同意文面
			{
				0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
				1, 0, 0, FALSE
			},
			///<FRAME3_M	カーソル
			{
				0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x18000, GX_BG_EXTPLTT_01,
				0, 0, 0, FALSE
			},
		};
		GF_BGL_BGControlSet(bgl, GF_BGL_FRAME0_M, &TextBgCntDat[0], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, GF_BGL_FRAME0_M );
		GF_BGL_ScrollSet(bgl, GF_BGL_FRAME0_M, GF_BGL_SCROLL_X_SET, 0);
		GF_BGL_ScrollSet(bgl, GF_BGL_FRAME0_M, GF_BGL_SCROLL_Y_SET, 0);
		
		GF_BGL_BGControlSet(bgl, GF_BGL_FRAME1_M, &TextBgCntDat[1], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, GF_BGL_FRAME1_M );
		GF_BGL_ScrollSet(bgl, GF_BGL_FRAME1_M, GF_BGL_SCROLL_X_SET, 0);
		GF_BGL_ScrollSet(bgl, GF_BGL_FRAME1_M, GF_BGL_SCROLL_Y_SET, 0);

		GF_BGL_BGControlSet(bgl, GF_BGL_FRAME2_M, &TextBgCntDat[2], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, GF_BGL_FRAME2_M );
		GF_BGL_ScrollSet(bgl, GF_BGL_FRAME2_M, GF_BGL_SCROLL_X_SET, 0);
		GF_BGL_ScrollSet(bgl, GF_BGL_FRAME2_M, GF_BGL_SCROLL_Y_SET, 0);
		
		GF_BGL_BGControlSet(bgl, GF_BGL_FRAME3_M, &TextBgCntDat[3], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, GF_BGL_FRAME3_M );
		GF_BGL_ScrollSet(bgl, GF_BGL_FRAME3_M, GF_BGL_SCROLL_X_SET, 0);
		GF_BGL_ScrollSet(bgl, GF_BGL_FRAME3_M, GF_BGL_SCROLL_Y_SET, 0);
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
			///<FRAME1_S	背景
			{
				0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0xd800, GX_BG_CHARBASE_0x08000, GX_BG_EXTPLTT_01,
				2, 0, 0, FALSE
			},
		};
		GF_BGL_BGControlSet(bgl, GF_BGL_FRAME0_S, &TextBgCntDat[0], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, GF_BGL_FRAME0_S );
		GF_BGL_ScrollSet(bgl, GF_BGL_FRAME0_S, GF_BGL_SCROLL_X_SET, 0);
		GF_BGL_ScrollSet(bgl, GF_BGL_FRAME0_S, GF_BGL_SCROLL_Y_SET, 0);
		
		GF_BGL_BGControlSet(bgl, GF_BGL_FRAME1_S, &TextBgCntDat[1], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, GF_BGL_FRAME1_S );
		GF_BGL_ScrollSet(bgl, GF_BGL_FRAME1_S, GF_BGL_SCROLL_X_SET, 0);
		GF_BGL_ScrollSet(bgl, GF_BGL_FRAME1_S, GF_BGL_SCROLL_Y_SET, 0);
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
	GF_BGL_BGControlExit( ini, GF_BGL_FRAME1_S );
	GF_BGL_BGControlExit( ini, GF_BGL_FRAME0_S );
	GF_BGL_BGControlExit( ini, GF_BGL_FRAME3_M );
	GF_BGL_BGControlExit( ini, GF_BGL_FRAME2_M );
	GF_BGL_BGControlExit( ini, GF_BGL_FRAME1_M );
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
static void BgGraphicSet( EMAIL_MENU_WORK * wk )
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
						
	MenuWinGraphicSet(	bgl, GF_BGL_FRAME2_M, EMAIL_MENUFRAME_CHR,
						EMAIL_MENUFRAME_PAL, 0, HEAPID_EMAIL );




	// メイン画面BG1キャラ転送
	ArcUtil_HDL_BgCharSet( p_handle, NARC_wifip2pmatch_conect_NCGR, bgl, GF_BGL_FRAME1_M, 0, 0, 0, HEAPID_EMAIL);

	// メイン画面BG1スクリーン転送
	ArcUtil_HDL_ScrnSet(   p_handle, NARC_wifip2pmatch_conect_01_NSCR, bgl, GF_BGL_FRAME1_M, 0, 32*24*2, 0, HEAPID_EMAIL);



	// サブ画面BG1キャラ転送
	ArcUtil_HDL_BgCharSet( p_handle, NARC_wifip2pmatch_conect_sub_NCGR, bgl, GF_BGL_FRAME1_S, 0, 0, 0, HEAPID_EMAIL);

	// サブ画面BG1スクリーン転送
	ArcUtil_HDL_ScrnSet(   p_handle, NARC_wifip2pmatch_conect_sub_NSCR, bgl, GF_BGL_FRAME1_S, 0, 32*24*2, 0, HEAPID_EMAIL);

	GF_BGL_BackGroundColorSet( GF_BGL_FRAME0_M, 0 );
	GF_BGL_BackGroundColorSet( GF_BGL_FRAME0_S, 0 );
	
	//Wifi接続BGパレットアニメシステム初期化
	ConnectBGPalAnm_Init(&wk->cbp, p_handle, NARC_wifip2pmatch_conect_anm_NCLR, HEAPID_EMAIL);
	
	ArchiveDataHandleClose( p_handle );
		
	// カーソル用BG
	{
		p_handle = ArchiveDataHandleOpen( ARC_EMAIL_GRA, HEAPID_EMAIL );		

		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );

		ArcUtil_HDL_BgCharSet( p_handle, NARC_email_gra_mail_cur_NCGR, bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_EMAIL );
		ArcUtil_HDL_ScrnSet( p_handle, NARC_email_gra_mail_cur_NSCR, bgl, GF_BGL_FRAME3_M,  0, 0, 0, HEAPID_EMAIL );
		ArcUtil_HDL_PalSet( p_handle, NARC_email_gra_mail_cur_NCLR, PALTYPE_MAIN_BG, EMAIL_BG_CUR_PAL*0x20, 0x20, HEAPID_EMAIL);

		ArchiveDataHandleClose( p_handle );
	}
}

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
static void BmpWinInit( EMAIL_MENU_WORK *wk )
{
	// ---------- メイン画面 ------------------

	// BG0面BMPWIN(エラー説明)ウインドウ確保・描画
	GF_BGL_BmpWinAdd(wk->bgl, &wk->SubWin, GF_BGL_FRAME0_M,
	SUB_TEXT_X, SUB_TEXT_Y, SUB_TEXT_SX, SUB_TEXT_SY, EMAIL_TALKFONT_PAL,  ERROR_MESSAGE_OFFSET );

	GF_BGL_BmpWinDataFill( &wk->SubWin, 0x0000 );

	// BG0面BMPWIN(タイトル)ウインドウ確保・描画
	GF_BGL_BmpWinAdd(wk->bgl, &wk->TitleWin, GF_BGL_FRAME0_M,
	CONNECT_TEXT_X, CONNECT_TEXT_Y, CONNECT_TEXT_SX, CONNECT_TEXT_SY, EMAIL_TALKFONT_PAL, TITLE_MESSAGE_OFFSET );

	GF_BGL_BmpWinDataFill( &wk->TitleWin, 0x0000 );
	Email_TalkPrint( &wk->TitleWin, wk->TitleString, 0, 1, 1, GF_PRINTCOLOR_MAKE(15,14,0) );

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
static void BmpWinDelete( EMAIL_MENU_WORK *wk )
{
	GF_BGL_BmpWinDel( &wk->MsgWin );
	GF_BGL_BmpWinDel( &wk->TitleWin );
	GF_BGL_BmpWinDel( &wk->SubWin );
}


//------------------------------------------------------------------
/**
 * $brief   
 *
 * @param   wk		
 * @param   to_seq		
 * @param   next_seq		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void Email_SetNextSeq( EMAIL_MENU_WORK *wk, int to_seq, int next_seq )
{
	wk->subprocess_seq      = to_seq;
	wk->subprocess_nextseq  = next_seq;
}

//------------------------------------------------------------------
/**
 * @brief   時間アイコン追加
 *
 * @param   wk		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void Email_TimeIconAdd( EMAIL_MENU_WORK *wk )
{
	if(wk->timeWaitWork == NULL){
		wk->timeWaitWork = TimeWaitIconAdd( &wk->MsgWin, EMAIL_MESFRAME_CHR );
	}
}

//------------------------------------------------------------------
/**
 * @brief   時間アイコン消去（NULLチェックする）
 *
 * @param   wk		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void Email_TimeIconDel( EMAIL_MENU_WORK *wk )
{
	if(wk->timeWaitWork!=NULL){
		TimeWaitIconDel(wk->timeWaitWork);
		wk->timeWaitWork = NULL;
	}
}


//==============================================================================
/**
 * $brief   はい・いいえウインドウ登録
 *
 * @param   bgl		
 * @param   menuframe		
 * @param   y		
 * @param   yesno_bmp_cgx		
 *
 * @retval  BMPMENU_WORK *		
 */
//==============================================================================
static BMPMENU_WORK *Email_BmpWinYesNoMake( GF_BGL_INI *bgl, int y, int yesno_bmp_cgx )
{
	BMPWIN_DAT yesnowin;

	yesnowin        = YesNoBmpWin;
	yesnowin.pos_y  = y;
	yesnowin.chrnum = yesno_bmp_cgx;

	return BmpYesNoSelectInit( bgl, &yesnowin, EMAIL_MENUFRAME_CHR, EMAIL_MENUFRAME_PAL, HEAPID_EMAIL );
}

static BMPMENU_WORK *Email_BmpWinYesNoMakeEx( GF_BGL_INI *bgl, int y, int yesno_bmp_cgx, int pos )
{
	BMPWIN_DAT yesnowin;

	yesnowin        = YesNoBmpWin;
	yesnowin.pos_y  = y;
	yesnowin.chrnum = yesno_bmp_cgx;

	return BmpYesNoSelectInitEx( bgl, &yesnowin, EMAIL_MENUFRAME_CHR, EMAIL_MENUFRAME_PAL, pos, HEAPID_EMAIL );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
static int Enter_MenuList( EMAIL_MENU_WORK *wk )
{
	u32	ret;

	switch(wk->local_seq){
	case 0:
		{
			int i;
			BMPLIST_HEADER list_h;
			const u8 *bmp_size;
			const EMAIL_BMPMENULIST *bmp_menulist;
			int menu_max;
			
			if(EMAILSAVE_UseCheck(wk->esys->savedata) == TRUE){
				bmp_size = EmailMenuBmpSize;
				bmp_menulist = EmailMenuList_All;
				menu_max = LIST_MENU_ALL_MAX;
				list_h = EmailMenuListAllHeader;
			}
			else{
				bmp_size = EmailMenuNoDataBmpSize;
				bmp_menulist = EmailMenuList_NoData;
				menu_max = LIST_MENU_NODATA_MAX;
				list_h = EmailMenuListNoDataHeader;
			}

			//メニューリスト用のBMPウィンドウ生成
			GF_BGL_BmpWinAdd(wk->bgl, &wk->list_bmpwin, 
				GF_BGL_FRAME0_M, bmp_size[0], bmp_size[1], bmp_size[2], bmp_size[3], 
				EMAIL_TALKFONT_PAL, MENULIST_MESSAGE_OFFSET );

			wk->listmenu = BMP_MENULIST_Create(menu_max, HEAPID_EMAIL);
			for(i = 0; i < menu_max; i++){
				BMP_MENULIST_AddArchiveString(wk->listmenu, wk->EmailMsgManager, 
					bmp_menulist[i].str_id, bmp_menulist[i].param);
			}

			list_h.win = &wk->list_bmpwin;
			list_h.list = wk->listmenu;
			wk->lw = BmpListSet(&list_h, 0, 0, HEAPID_EMAIL);
			
			BmpMenuWinWrite(&wk->list_bmpwin, WINDOW_TRANS_OFF, 
				EMAIL_MENUFRAME_CHR, EMAIL_MENUFRAME_PAL);

			BmpTalkWinClear( &wk->MsgWin, WINDOW_TRANS_OFF );
			
			GF_BGL_BmpWinOn(&wk->list_bmpwin);
		}
		
		wk->local_seq++;
		break;
	case 1:
		ret = BmpListMain(wk->lw);
		switch(ret){
		case BMPLIST_NULL:
			break;
		case BMPLIST_CANCEL:
			Snd_SePlay(EMAIL_SE_CANCEL);
			Email_SubProcessChange( wk->esys, EMAIL_SUBPROC_END, 0 );
			wk->local_work  = ENTER_END;
			wk->local_seq++;
			break;
		default:
			Snd_SePlay(EMAIL_SE_DECIDE);
			wk->local_work = ret;
			wk->local_seq++;
			break;
		}
		break;
		
	default:
		BMP_MENULIST_Delete(wk->listmenu);
		BmpListExit(wk->lw, NULL, NULL);
		BmpMenuWinClear( &wk->list_bmpwin, WINDOW_TRANS_OFF);
		GF_BGL_BmpWinOff( &wk->list_bmpwin );
		GF_BGL_BmpWinDel( &wk->list_bmpwin );
		
		wk->subprocess_seq = wk->local_work;
		break;
	}
	return SUBSEQ_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   Eメール確認サブPROC呼び出し
 *
 * @param   wk		
 *
 * @retval  
 */
//--------------------------------------------------------------
static int Enter_AddressCheckProcChange(EMAIL_MENU_WORK *wk)
{
	//登録コード入力画面へ移行
	Email_SubProcessChange( wk->esys, 
			EMAIL_SUBPROC_ADDRESS_CHECK, EMAIL_MODE_INPUT_EMAIL_CHECK);
    // MatchComment: ENTER_MENU_LIST -> ENTER_UNK_0x26
	Email_RecoveryMenuModeSet( wk->esys, ENTER_UNK_0x26);
	wk->subprocess_seq = ENTER_END;
	return SUBSEQ_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   Eメールを受け取るか決める
 *
 * @param   wk		
 *
 * @retval  
 */
//--------------------------------------------------------------
static int Enter_RecvSelectYesNo(EMAIL_MENU_WORK *wk)
{
	if( Enter_MessagePrintEndCheck( wk->MsgIndex )==TRUE){
		return SUBSEQ_CONTINUE;
	}

	switch(wk->local_seq){
	case 0:
		Enter_MessagePrint( wk, wk->EmailMsgManager, msg_email_111, 1, 0x0f0f );
		wk->local_seq++;
		break;
	case 1:	//はい・いいえウィンドウ表示
		wk->YesNoMenuWork = Email_BmpWinYesNoMake(wk->bgl, EMAIL_YESNO_PY2, YESNO_OFFSET );
		wk->local_seq++;
		break;
	case 2:
		{
			int ret = BmpYesNoSelectMain( wk->YesNoMenuWork, HEAPID_EMAIL );

			if(ret!=BMPMENU_NULL){
				if(ret==BMPMENU_CANCEL){	//受け取らない
					Enter_MessagePrint( wk, wk->EmailMsgManager, msg_email_113, 1, 0x0f0f );
					EMAILSAVE_ParamSet(wk->esys->savedata, EMAIL_PARAM_RECV_FLAG, 0);
				}
				else{	//受け取る
					Enter_MessagePrint( wk, wk->EmailMsgManager, msg_email_112, 1, 0x0f0f );
					EMAILSAVE_ParamSet(wk->esys->savedata, EMAIL_PARAM_RECV_FLAG, 
						DPW_PROFILE_MAILRECVFLAG_EXCHANGE);
				}
				wk->local_seq++;
			}
		}
		break;
	case 3:
		Email_SetNextSeq( wk, ENTER_SAVE, ENTER_MENU_LIST );
		break;
	}
	return SUBSEQ_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   Eメールを初期化しますか？
 *
 * @param   wk		
 *
 * @retval  
 */
//--------------------------------------------------------------
static int Enter_EmailDataInitializeYesNo(EMAIL_MENU_WORK *wk)
{
	if( Enter_MessagePrintEndCheck( wk->MsgIndex )==TRUE){
		return SUBSEQ_CONTINUE;
	}

	switch(wk->local_seq){
	case 0:
		Enter_MessagePrint( wk, wk->EmailMsgManager, msg_email_114, 1, 0x0f0f );
		wk->local_seq++;
		break;
	case 1:	//はい・いいえウィンドウ表示
		wk->YesNoMenuWork = Email_BmpWinYesNoMakeEx(wk->bgl, EMAIL_YESNO_PY2, YESNO_OFFSET, 1 );
		wk->local_seq++;
		break;
	case 2:
		{
			int ret = BmpYesNoSelectMain( wk->YesNoMenuWork, HEAPID_EMAIL );

			if(ret!=BMPMENU_NULL){
				if(ret==BMPMENU_CANCEL){
					wk->subprocess_seq = ENTER_MENU_LIST;
				}
				else{
					Enter_MessagePrint( wk, wk->EmailMsgManager, msg_email_115, 1, 0x0f0f );
					EMAILSAVE_DataInitialize(wk->esys->savedata);
					wk->local_seq++;
				}
			}
		}
		break;
	case 3:
		Email_SetNextSeq( wk, ENTER_SAVE, ENTER_MENU_LIST );
		break;
	}
	return SUBSEQ_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   Eメール設定：開始確認
 *
 * @param   wk		
 *
 * @retval  
 */
//--------------------------------------------------------------
static const int Agreement_Str[] = {
	msg_email_info_000,
	msg_email_info_spc,
	msg_email_info_001,
	msg_email_info_002,
	msg_email_info_003,
	msg_email_info_004,
	msg_email_info_spc,
	msg_email_info_005,
	msg_email_info_006,
	msg_email_info_spc,
	msg_email_info_007,
//	msg_email_info_008,
//	msg_email_info_009,
//	msg_email_info_spc,
//	msg_email_info_010,
//	msg_email_info_011,
};
#define INFO_MESSAGE_LINE	( 6 )

static u32 AgreePos[][ 2 ] = {
	{   4, 8 },
	{ 128, 8 },
};

static void AgreeCurPut( EMAIL_MENU_WORK* wk )
{
	STRBUF* str;
	GF_BGL_BmpWinDataFill( &wk->info_win2,  0x0f0f );
	
	str = MSGMAN_AllocString( wk->EmailMsgManager, msg_email_907 );
	GF_STR_PrintSimple( &wk->info_win2, FONT_SYSTEM, str, AgreePos[ 0 ][ 0 ] + 12, AgreePos[ 0 ][ 1 ], MSG_NO_PUT, NULL );
	STRBUF_Delete( str );
	
	str = MSGMAN_AllocString( wk->EmailMsgManager, msg_email_908 );
	GF_STR_PrintSimple( &wk->info_win2, FONT_SYSTEM, str, AgreePos[ 1 ][ 0 ] + 12, AgreePos[ 1 ][ 1 ], MSG_NO_PUT, NULL );
	STRBUF_Delete( str );

	BMPCURSOR_Print( wk->info_cur, &wk->info_win2, AgreePos[ wk->info_cur_pos ][ 0 ], AgreePos[ wk->info_cur_pos ][ 1 ] );
	
	GF_BGL_BmpWinOn( &wk->info_win2 );
}

static void AgreeCurClear( EMAIL_MENU_WORK* wk )
{
	GF_BGL_BmpWinDataFill( &wk->info_win2,  0x0f0f );	
	GF_BGL_BmpWinOn( &wk->info_win2 );
}

static void AgreeListCurMove( EMAIL_MENU_WORK* wk )
{
	if ( (++wk->info_wait) == 8 ){
		wk->info_count ^= 1;
		wk->info_wait = 0;		
		///< 下
		if ( wk->info_pos + INFO_MESSAGE_LINE != wk->info_end ){
			GF_BGL_ScrFill( wk->bgl, GF_BGL_FRAME3_M, 1 + ( wk->info_count * 20 ), 14, 17, 1, 1, 9 );
			GF_BGL_ScrFill( wk->bgl, GF_BGL_FRAME3_M, 2 + ( wk->info_count * 20 ), 15, 17, 1, 1, 9 );
			GF_BGL_ScrFill( wk->bgl, GF_BGL_FRAME3_M, 3 + ( wk->info_count * 20 ), 16, 17, 1, 1, 9 );
			GF_BGL_ScrFill( wk->bgl, GF_BGL_FRAME3_M, 4 + ( wk->info_count * 20 ), 17, 17, 1, 1, 9 );
			GF_BGL_ScrFill( wk->bgl, GF_BGL_FRAME3_M,11 + ( wk->info_count * 20 ), 14, 18, 1, 1, 9 );
			GF_BGL_ScrFill( wk->bgl, GF_BGL_FRAME3_M,12 + ( wk->info_count * 20 ), 15, 18, 1, 1, 9 );
			GF_BGL_ScrFill( wk->bgl, GF_BGL_FRAME3_M,13 + ( wk->info_count * 20 ), 16, 18, 1, 1, 9 );
			GF_BGL_ScrFill( wk->bgl, GF_BGL_FRAME3_M,14 + ( wk->info_count * 20 ), 17, 18, 1, 1, 9 );
		}
		else {
			GF_BGL_ScrFill( wk->bgl, GF_BGL_FRAME3_M, 0, 14, 17, 4, 2, 9 );
		}

		///< 上
		if ( wk->info_pos != 0 ){
			GF_BGL_ScrFill( wk->bgl, GF_BGL_FRAME3_M, 5 + ( wk->info_count * 20 ), 14,  3, 1, 1, 9 );
			GF_BGL_ScrFill( wk->bgl, GF_BGL_FRAME3_M, 6 + ( wk->info_count * 20 ), 15,  3, 1, 1, 9 );
			GF_BGL_ScrFill( wk->bgl, GF_BGL_FRAME3_M, 7 + ( wk->info_count * 20 ), 16,  3, 1, 1, 9 );
			GF_BGL_ScrFill( wk->bgl, GF_BGL_FRAME3_M, 8 + ( wk->info_count * 20 ), 17,  3, 1, 1, 9 );
			GF_BGL_ScrFill( wk->bgl, GF_BGL_FRAME3_M,15 + ( wk->info_count * 20 ), 14,  4, 1, 1, 9 );
			GF_BGL_ScrFill( wk->bgl, GF_BGL_FRAME3_M,16 + ( wk->info_count * 20 ), 15,  4, 1, 1, 9 );
			GF_BGL_ScrFill( wk->bgl, GF_BGL_FRAME3_M,17 + ( wk->info_count * 20 ), 16,  4, 1, 1, 9 );
			GF_BGL_ScrFill( wk->bgl, GF_BGL_FRAME3_M,18 + ( wk->info_count * 20 ), 17,  4, 1, 1, 9 );
		}
		else {
			GF_BGL_ScrFill( wk->bgl, GF_BGL_FRAME3_M, 0, 14, 3, 4, 2, 9 );
		}
		GF_BGL_LoadScreenV_Req( wk->bgl, GF_BGL_FRAME3_M );
	}
}

#ifdef NONEQUIVALENT
static void Agreement_TxtRW( EMAIL_MENU_WORK* wk )
{
	int i;
	int no = 0;
						
	GF_BGL_BmpWinDataFill( &wk->info_win,  0x0f0f );
	for ( i = wk->info_pos; i < wk->info_pos + INFO_MESSAGE_LINE; i++ ){
		STRBUF* str = MSGMAN_AllocString( wk->EmailMsgManager, Agreement_Str[ i ] );
		GF_STR_PrintSimple( &wk->info_win, FONT_SYSTEM, str, 4, no * 16, MSG_NO_PUT, NULL );
		STRBUF_Delete( str );
		no++;
	}
	GF_BGL_BmpWinOn( &wk->info_win );
}
#else
asm static void Agreement_TxtRW( EMAIL_MENU_WORK* wk )
{
	push {r4, r5, r6, r7, lr}
	sub sp, #0x14
	add r7, r0, #0
	add r0, #0xc4
	mov r1, #0xf
	bl GF_BGL_BmpWinDataFill
	ldr r0, [r7, #0x34]
	mov r1, #0x2c
	bl MSGMAN_AllocString
	str r0, [sp, #0xc]
	bl STRBUF_GetLen
	mov r1, #0x6d
	bl STRBUF_Create
	add r6, r0, #0
	add r0, r7, #0
	add r0, #0xac
	ldr r4, [r0, #0]
	add r0, r4, #6
	cmp r4, r0
	bge _02248336
	add r0, r7, #0
	str r0, [sp, #0x10]
	add r0, #0xc4
	mov r5, #0
	str r0, [sp, #0x10]
_02248306:
	ldr r1, [sp, #0xc]
	add r0, r6, #0
	add r2, r4, #0
	bl STRBUF_CopyLine
	str r5, [sp]
	mov r0, #0xff
	str r0, [sp, #4]
	mov r0, #0
	str r0, [sp, #8]
	ldr r0, [sp, #0x10]
	mov r1, #0
	add r2, r6, #0
	mov r3, #4
	bl GF_STR_PrintSimple
	add r0, r7, #0
	add r0, #0xac
	ldr r0, [r0, #0]
	add r4, r4, #1
	add r0, r0, #6
	add r5, #0x10
	cmp r4, r0
	blt _02248306
_02248336:
	ldr r0, [sp, #0xc]
	bl STRBUF_Delete
	add r0, r6, #0
	bl STRBUF_Delete
	add r7, #0xc4
	add r0, r7, #0
	bl GF_BGL_BmpWinOn
	add sp, #0x14
	pop {r4, r5, r6, r7, pc}
}
#endif

#ifdef NONEQUIVALENT
static int Enter_Agreement( EMAIL_MENU_WORK *wk )
{
	switch ( wk->sub_seq ){
	case 0:
		{
			int i;
			int list_max = NELEMS( Agreement_Str );
			BMPLIST_HEADER list_h;
			
			wk->info_pos = 0;
			wk->info_cur_pos = 0;
			wk->info_param = 0;
			wk->info_end = NELEMS( Agreement_Str );
			GF_BGL_BmpWinAdd( wk->bgl, &wk->info_win,  GF_BGL_FRAME2_M, 1,  5, 30, 12, EMAIL_TALKFONT_PAL, MENULIST_MESSAGE_OFFSET );
			GF_BGL_BmpWinAdd( wk->bgl, &wk->info_win2, GF_BGL_FRAME2_M, 1, 19, 30,  4, EMAIL_TALKFONT_PAL, MENULIST_MESSAGE_OFFSET + ( 30 * 12 ) );

			GF_BGL_BmpWinDataFill( &wk->info_win,  0x0f0f );
			for ( i = 0; i < INFO_MESSAGE_LINE; i++ ){
				STRBUF* str = MSGMAN_AllocString( wk->EmailMsgManager, Agreement_Str[ i ] );
				GF_STR_PrintSimple( &wk->info_win, FONT_SYSTEM, str, 4, i * 16, MSG_NO_PUT, NULL );
				STRBUF_Delete( str );
			}
			BmpMenuWinWrite( &wk->info_win, WINDOW_TRANS_OFF, EMAIL_MENUFRAME_CHR, EMAIL_MENUFRAME_PAL );			
			GF_BGL_BmpWinOn( &wk->info_win );

			GF_BGL_BmpWinDataFill( &wk->info_win2,  0x0f0f );
			BmpMenuWinWrite( &wk->info_win2, WINDOW_TRANS_OFF, EMAIL_MENUFRAME_CHR, EMAIL_MENUFRAME_PAL );			
			GF_BGL_BmpWinOn( &wk->info_win2 );
			wk->info_cur = BMPCURSOR_Create( HEAPID_EMAIL );
			
			GF_BGL_BmpWinDataFill( &wk->MsgWin,  0x0f0f );
			BmpTalkWinClear( &wk->MsgWin, WINDOW_TRANS_OFF );
			GF_BGL_BmpWinOff( &wk->MsgWin );
			
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
			
			wk->info_wait  = 0;
			wk->info_count = 0;
			
			wk->sub_seq++;
		}
		break;

	case 1:
		{
			int i;
			int old = wk->info_pos;

			if ( sys.repeat & PAD_KEY_UP ){
				if ( wk->info_pos != 0 ){
					wk->info_pos--;
					Snd_SePlay( SEQ_SE_DP_SELECT );
				}
			}
			else if ( sys.repeat & PAD_KEY_DOWN ){
				if ( wk->info_pos + INFO_MESSAGE_LINE < wk->info_end ){
					wk->info_pos++;
					Snd_SePlay( SEQ_SE_DP_SELECT );
				}	
				if ( wk->info_pos + INFO_MESSAGE_LINE == wk->info_end ){	///< 最後まで行ったので同意チェック
					AgreeCurPut( wk );
					wk->sub_seq++;
					Snd_SePlay( SEQ_SE_DP_SELECT );
				}
			}
			else if ( sys.trg & PAD_BUTTON_B ){
				Snd_SePlay( SEQ_SE_DP_SELECT );
				wk->info_param = 2;
				wk->sub_seq = 0xFF;
			}
			if ( old != wk->info_pos ){
				Agreement_TxtRW( wk );
			}
		}
		break;

	case 2:
		{
			if ( sys.trg & PAD_KEY_RIGHT || sys.trg & PAD_KEY_LEFT ){
				wk->info_cur_pos ^= 1;
				Snd_SePlay( SEQ_SE_DP_SELECT );
				AgreeCurPut( wk );
			}
			else if ( sys.trg & PAD_BUTTON_A ){
				wk->info_param = 2 - wk->info_cur_pos;
				Snd_SePlay( SEQ_SE_DP_SELECT );
				wk->sub_seq++;
			}
			else if ( sys.trg & PAD_BUTTON_B ){
				wk->info_param = 2;
				Snd_SePlay( SEQ_SE_DP_SELECT );
				wk->sub_seq++;
			}
			else if ( sys.trg & PAD_KEY_UP ){
				AgreeCurClear( wk );
				Snd_SePlay( SEQ_SE_DP_SELECT );
				wk->sub_seq--;
				wk->info_pos--;
				Agreement_TxtRW( wk );
			}		
		}
		break;

	default:
		{
			BmpMenuWinClear( &wk->info_win, WINDOW_TRANS_ON );
			GF_BGL_BmpWinOff( &wk->info_win );
			GF_BGL_BmpWinDel( &wk->info_win );
			
			BmpMenuWinClear( &wk->info_win2, WINDOW_TRANS_ON );
			GF_BGL_BmpWinOff( &wk->info_win2 );
			GF_BGL_BmpWinDel( &wk->info_win2 );
			
			BMPCURSOR_Delete( wk->info_cur );
			
			GF_BGL_ScrClear( wk->bgl, GF_BGL_FRAME3_M );
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );
			
			wk->sub_seq = 0;
		}	
		return wk->info_param;
	}	
	AgreeListCurMove( wk );

	return 0;
}
#else
asm static int Enter_Agreement( EMAIL_MENU_WORK *wk )
{
	push {r4, r5, r6, r7, lr}
	sub sp, #0x1c
	add r5, r0, #0
	add r1, r5, #0
	add r1, #0xa8
	ldr r1, [r1, #0]
	cmp r1, #0
	beq _0224836E
	cmp r1, #1
	bne _02248366
	b _022484B8
_02248366:
	cmp r1, #2
	bne _0224836C
	b _02248566
_0224836C:
	b _02248614
_0224836E:
	ldr r0, [r5, #0x34]
	mov r1, #0x2c
	bl MSGMAN_AllocString
	str r0, [sp, #0x14]
	bl STRBUF_GetLines
	add r1, r5, #0
	mov r2, #0
	add r1, #0xac
	str r2, [r1, #0]
	add r1, r5, #0
	add r1, #0xb0
	str r2, [r1, #0]
	add r1, r5, #0
	add r1, #0xb8
	str r2, [r1, #0]
	add r1, r5, #0
	add r1, #0xb4
	str r0, [r1, #0]
	mov r0, #5
	str r0, [sp]
	mov r0, #0x1e
	str r0, [sp, #4]
	mov r0, #0xc
	str r0, [sp, #8]
	mov r0, #0xd
	str r0, [sp, #0xc]
	mov r0, #0x94
	str r0, [sp, #0x10]
	add r1, r5, #0
	ldr r0, [r5, #4]
	add r1, #0xc4
	mov r2, #2
	mov r3, #1
	bl GF_BGL_BmpWinAdd
	mov r0, #0x13
	str r0, [sp]
	mov r0, #0x1e
	str r0, [sp, #4]
	mov r0, #4
	str r0, [sp, #8]
	mov r0, #0xd
	str r0, [sp, #0xc]
	mov r0, #0x7f
	lsl r0, r0, #2
	str r0, [sp, #0x10]
	add r1, r5, #0
	ldr r0, [r5, #4]
	add r1, #0xd4
	mov r2, #2
	mov r3, #1
	bl GF_BGL_BmpWinAdd
	add r0, r5, #0
	add r0, #0xc4
	mov r1, #0xf
	bl GF_BGL_BmpWinDataFill
	ldr r0, [sp, #0x14]
	bl STRBUF_GetLen
	mov r1, #0x6d
	bl STRBUF_Create
	add r7, r0, #0
	add r0, r5, #0
	mov r6, #0
	str r0, [sp, #0x18]
	add r0, #0xc4
	add r4, r6, #0
	str r0, [sp, #0x18]
_02248400:
	ldr r1, [sp, #0x14]
	add r0, r7, #0
	add r2, r6, #0
	bl STRBUF_CopyLine
	str r4, [sp]
	mov r0, #0xff
	str r0, [sp, #4]
	mov r0, #0
	str r0, [sp, #8]
	ldr r0, [sp, #0x18]
	mov r1, #0
	add r2, r7, #0
	mov r3, #4
	bl GF_STR_PrintSimple
	add r6, r6, #1
	add r4, #0x10
	cmp r6, #6
	blt _02248400
	ldr r0, [sp, #0x14]
	bl STRBUF_Delete
	add r0, r7, #0
	bl STRBUF_Delete
	add r0, r5, #0
	add r0, #0xc4
	mov r1, #1
	mov r2, #0x1f
	mov r3, #0xb
	bl BmpMenuWinWrite
	add r0, r5, #0
	add r0, #0xc4
	bl GF_BGL_BmpWinOn
	add r0, r5, #0
	add r0, #0xd4
	mov r1, #0xf
	bl GF_BGL_BmpWinDataFill
	add r0, r5, #0
	add r0, #0xd4
	mov r1, #1
	mov r2, #0x1f
	mov r3, #0xb
	bl BmpMenuWinWrite
	add r0, r5, #0
	add r0, #0xd4
	bl GF_BGL_BmpWinOn
	mov r0, #0x6d
	bl BMPCURSOR_Create
	add r1, r5, #0
	add r1, #0xe4
	str r0, [r1, #0]
	add r0, r5, #0
	add r0, #0x48
	mov r1, #0xf
	bl GF_BGL_BmpWinDataFill
	add r0, r5, #0
	add r0, #0x48
	mov r1, #1
	bl BmpTalkWinClear
	add r0, r5, #0
	add r0, #0x48
	bl GF_BGL_BmpWinOff
	mov r0, #8
	mov r1, #1
	bl GF_Disp_GX_VisibleControl
	add r0, r5, #0
	mov r1, #0
	add r0, #0xbc
	str r1, [r0, #0]
	add r0, r5, #0
	add r0, #0xc0
	str r1, [r0, #0]
	add r0, r5, #0
	add r0, #0xa8
	ldr r0, [r0, #0]
	add r1, r0, #1
	add r0, r5, #0
	add r0, #0xa8
	str r1, [r0, #0]
	b _02248670
_022484B8:
	ldr r1, =sys // _0224867C
	add r0, #0xac
	ldr r2, [r1, #0x4c]
	ldr r4, [r0, #0]
	mov r0, #0x40
	tst r0, r2
	beq _022484E0
	cmp r4, #0
	beq _02248552
	add r0, r5, #0
	add r0, #0xac
	ldr r0, [r0, #0]
	sub r1, r0, #1
	add r0, r5, #0
	add r0, #0xac
	str r1, [r0, #0]
	ldr r0, =0x000005DC // _02248680
	bl Snd_SePlay
	b _02248552
_022484E0:
	mov r0, #0x80
	tst r0, r2
	beq _02248534
	add r0, r5, #0
	add r0, #0xb4
	ldr r0, [r0, #0]
	add r1, r4, #6
	cmp r1, r0
	bge _02248506
	add r0, r5, #0
	add r0, #0xac
	ldr r0, [r0, #0]
	add r1, r0, #1
	add r0, r5, #0
	add r0, #0xac
	str r1, [r0, #0]
	ldr r0, =0x000005DC // _02248680
	bl Snd_SePlay
_02248506:
	add r0, r5, #0
	add r0, #0xac
	ldr r0, [r0, #0]
	add r1, r0, #6
	add r0, r5, #0
	add r0, #0xb4
	ldr r0, [r0, #0]
	cmp r1, r0
	bne _02248552
	add r0, r5, #0
	bl AgreeCurPut
	add r0, r5, #0
	add r0, #0xa8
	ldr r0, [r0, #0]
	add r1, r0, #1
	add r0, r5, #0
	add r0, #0xa8
	str r1, [r0, #0]
	ldr r0, =0x000005DC // _02248680
	bl Snd_SePlay
	b _02248552
_02248534:
	ldr r1, [r1, #0x48]
	mov r0, #2
	tst r0, r1
	beq _02248552
	ldr r0, =0x000005DC // _02248680
	bl Snd_SePlay
	add r0, r5, #0
	mov r1, #2
	add r0, #0xb8
	str r1, [r0, #0]
	add r0, r5, #0
	mov r1, #0xff
	add r0, #0xa8
	str r1, [r0, #0]
_02248552:
	add r0, r5, #0
	add r0, #0xac
	ldr r0, [r0, #0]
	cmp r4, r0
	bne _0224855E
	b _02248670
_0224855E:
	add r0, r5, #0
	bl Agreement_TxtRW
	b _02248670
_02248566:
	ldr r1, =sys // _0224867C
	mov r2, #0x10
	ldr r1, [r1, #0x48]
	tst r2, r1
	bne _02248576
	mov r2, #0x20
	tst r2, r1
	beq _02248594
_02248576:
	add r0, r5, #0
	add r0, #0xb0
	ldr r1, [r0, #0]
	mov r0, #1
	eor r1, r0
	add r0, r5, #0
	add r0, #0xb0
	str r1, [r0, #0]
	ldr r0, =0x000005DC // _02248680
	bl Snd_SePlay
	add r0, r5, #0
	bl AgreeCurPut
	b _02248670
_02248594:
	mov r2, #1
	tst r2, r1
	beq _022485BE
	add r0, #0xb0
	ldr r1, [r0, #0]
	mov r0, #2
	sub r1, r0, r1
	add r0, r5, #0
	add r0, #0xb8
	str r1, [r0, #0]
	ldr r0, =0x000005DC // _02248680
	bl Snd_SePlay
	add r0, r5, #0
	add r0, #0xa8
	ldr r0, [r0, #0]
	add r1, r0, #1
	add r0, r5, #0
	add r0, #0xa8
	str r1, [r0, #0]
	b _02248670
_022485BE:
	mov r2, #2
	add r3, r1, #0
	tst r3, r2
	beq _022485E0
	add r0, #0xb8
	str r2, [r0, #0]
	ldr r0, =0x000005DC // _02248680
	bl Snd_SePlay
	add r0, r5, #0
	add r0, #0xa8
	ldr r0, [r0, #0]
	add r1, r0, #1
	add r0, r5, #0
	add r0, #0xa8
	str r1, [r0, #0]
	b _02248670
_022485E0:
	mov r2, #0x40
	tst r1, r2
	beq _02248670
	bl AgreeCurClear
	ldr r0, =0x000005DC // _02248680
	bl Snd_SePlay
	add r0, r5, #0
	add r0, #0xa8
	ldr r0, [r0, #0]
	sub r1, r0, #1
	add r0, r5, #0
	add r0, #0xa8
	str r1, [r0, #0]
	add r0, r5, #0
	add r0, #0xac
	ldr r0, [r0, #0]
	sub r1, r0, #1
	add r0, r5, #0
	add r0, #0xac
	str r1, [r0, #0]
	add r0, r5, #0
	bl Agreement_TxtRW
	b _02248670
_02248614:
	add r0, #0xc4
	mov r1, #0
	bl BmpMenuWinClear
	add r0, r5, #0
	add r0, #0xc4
	bl GF_BGL_BmpWinOff
	add r0, r5, #0
	add r0, #0xc4
	bl GF_BGL_BmpWinDel
	add r0, r5, #0
	add r0, #0xd4
	mov r1, #0
	bl BmpMenuWinClear
	add r0, r5, #0
	add r0, #0xd4
	bl GF_BGL_BmpWinOff
	add r0, r5, #0
	add r0, #0xd4
	bl GF_BGL_BmpWinDel
	add r0, r5, #0
	add r0, #0xe4
	ldr r0, [r0, #0]
	bl BMPCURSOR_Delete
	ldr r0, [r5, #4]
	mov r1, #3
	bl GF_BGL_ScrClear
	mov r0, #8
	mov r1, #0
	bl GF_Disp_GX_VisibleControl
	add r0, r5, #0
	mov r1, #0
	add r0, #0xa8
	str r1, [r0, #0]
	add r5, #0xb8
	add sp, #0x1c
	ldr r0, [r5, #0]
	pop {r4, r5, r6, r7, pc}
_02248670:
	add r0, r5, #0
	bl AgreeListCurMove
	mov r0, #0
	add sp, #0x1c
	pop {r4, r5, r6, r7, pc}
	// .align 2, 0
// _0224867C: .4byte sys
// _02248680: .4byte 0x000005DC
}
#endif

static int Enter_AddressEntryStart(EMAIL_MENU_WORK *wk)
{
	if( Enter_MessagePrintEndCheck( wk->MsgIndex )==TRUE){
		return SUBSEQ_CONTINUE;
	}

	switch(wk->local_seq){
	case 0: //20歳以上？
	//	Enter_MessagePrint( wk, wk->EmailMsgManager, msg_email_101, 1, 0x0f0f );
		wk->local_seq = 3;
		break;
	case 1:	//はい・いいえウィンドウ表示
	//	wk->YesNoMenuWork = Email_BmpWinYesNoMakeEx(wk->bgl, EMAIL_YESNO_PY2, YESNO_OFFSET, 1 );
		wk->local_seq++;
		break;
	
	case 2:
		{
			int ret = BmpYesNoSelectMain( wk->YesNoMenuWork, HEAPID_EMAIL );
			if(ret!=BMPMENU_NULL){
				if(ret==BMPMENU_CANCEL){
					wk->local_seq++;
				}
				else{
					///< wi-fiつながるー？
					Enter_MessagePrint( wk, wk->EmailMsgManager, msg_email_101_01, 1, 0x0f0f );
					wk->local_seq = 4;
				}
			}
		}
		break;
		
	case 3:
		{
			int ret = Enter_Agreement( wk );
			
			if ( ret == 1 ){
				Enter_MessagePrint( wk, wk->EmailMsgManager, msg_email_101_01, 1, 0x0f0f );
				wk->local_seq++;
			}
			else if ( ret == 2 ){
				Email_SetNextSeq( wk, ENTER_MES_WAIT, ENTER_MENU_LIST );
			}
		}
		break;
	
	case 4:	///< つながるよー
		wk->YesNoMenuWork = Email_BmpWinYesNoMake(wk->bgl, EMAIL_YESNO_PY2, YESNO_OFFSET );
		wk->local_seq++;
		break;
		
	default:
		{
			int ret = BmpYesNoSelectMain( wk->YesNoMenuWork, HEAPID_EMAIL );

			if(ret!=BMPMENU_NULL){
				if(ret==BMPMENU_CANCEL){
					Enter_MessagePrint( wk, wk->EmailMsgManager, msg_email_102, 1, 0x0f0f );
					Email_SetNextSeq( wk, ENTER_MES_WAIT, ENTER_MENU_LIST );
				}
				else{
				//	Enter_MessagePrint( wk, wk->EmailMsgManager, msg_email_102, 1, 0x0f0f );
				//	Email_SetNextSeq( wk, ENTER_MES_WAIT, ENTER_ADDRESS_INPUT_PROC_CHANGE );
					wk->subprocess_seq = ENTER_ADDRESS_INPUT_PROC_CHANGE;
				}
			}
		}
		break;
	}
	
	return SUBSEQ_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   登録コード入力画面呼び出し
 *
 * @param   wk		
 *
 * @retval  
 */
//--------------------------------------------------------------
static int Enter_AddressInputProcChange(EMAIL_MENU_WORK *wk)
{
	//登録コード入力画面へ移行
	Email_SubProcessChange( wk->esys, 
			EMAIL_SUBPROC_ADDRESS_INPUT, EMAIL_MODE_INPUT_EMAIL );
	Email_RecoveryMenuModeSet( wk->esys, ENTER_ADDRESS_RETURN);
	wk->subprocess_seq = ENTER_END;
	return SUBSEQ_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   アドレス入力画面から戻ってきたときに起動するシーケンス
 *
 * @param   wk		
 *
 * @retval  
 */
//--------------------------------------------------------------
#ifdef NONEQUIVALENT
static int Enter_AddressReturn(EMAIL_MENU_WORK *wk)
{
	STRCODE *code;
	
	if(Email_AddressReturnFlagGet(wk->esys) == EMAIL_ADDRESS_RET_CANCEL){
		wk->subprocess_seq = ENTER_MENU_LIST;
		return SUBSEQ_CONTINUE;
	}
	
	switch(wk->local_seq){
	case 0:
		wk->subprocess_seq = ENTER_START;
		break;
	}
	return SUBSEQ_CONTINUE;
}
#else
asm static int Enter_AddressReturn(EMAIL_MENU_WORK *wk)
{
	push {r4, lr}
	add r4, r0, #0
	ldr r0, [r4, #0]
	bl Email_AddressReturnFlagGet
	cmp r0, #1
	beq _022487E4
	cmp r0, #2
	beq _022487EA
	cmp r0, #3
	beq _022487EA
	b _022487F2
_022487E4:
	mov r0, #0
	str r0, [r4, #8]
	pop {r4, pc}
_022487EA:
	mov r0, #0x25
	str r0, [r4, #8]
	mov r0, #0
	pop {r4, pc}
_022487F2:
	add r0, r4, #0
	add r0, #0x94
	ldr r0, [r0, #0]
	cmp r0, #0
	bne _02248800
	mov r0, #7
	str r0, [r4, #8]
_02248800:
	mov r0, #0
	pop {r4, pc}
}
#endif

//------------------------------------------------------------------
/**
 * $brief   サブプロセスシーケンススタート処理
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Enter_Start( EMAIL_MENU_WORK *wk)
{
	if( Enter_MessagePrintEndCheck( wk->MsgIndex )==TRUE){
		return SUBSEQ_CONTINUE;
	}

	switch(wk->local_seq){
	case 0:
		OS_TPrintf("Enter 開始\n");

		wk->local_seq++;
		break;
	case 1:
		//通信します。準備はいいですか？
		Enter_MessagePrint( wk, wk->EmailMsgManager, msg_email_103, 1, 0x0f0f );
		wk->local_seq++;
		break;

	case 2:	//はい・いいえウィンドウ表示
		wk->YesNoMenuWork = Email_BmpWinYesNoMake(wk->bgl, EMAIL_YESNO_PY2, YESNO_OFFSET );
		wk->local_seq++;
		break;
	case 3:
		{
			int ret = BmpYesNoSelectMain( wk->YesNoMenuWork, HEAPID_EMAIL );

			if(ret!=BMPMENU_NULL){
				if(ret==BMPMENU_CANCEL){
					Email_SetNextSeq( wk, ENTER_MES_WAIT, ENTER_MENU_LIST );
				}
				else{
					wk->local_seq++;
				}
			}
		}
		break;
	
	case 4:	//WiFi接続開始
		if(mydwc_checkMyGSID(wk->esys->savedata) == FALSE){
			//自分のGSプロファイルIDを取得していないので、取得プロセスに移行
			Email_SubProcessChange( wk->esys, EMAIL_SUBPROC_GSPROFILE_GET, 0 );
			Email_RecoveryMenuModeSet(wk->esys, ENTER_INTERNET_CONNECT);
			wk->subprocess_seq = ENTER_END;
			return SUBSEQ_CONTINUE;
		}
		
		Email_SetNextSeq( wk, ENTER_MES_WAIT, ENTER_INTERNET_CONNECT );
		break;
	}
	
	return SUBSEQ_CONTINUE;
}



//------------------------------------------------------------------
/**
 * $brief   接続を開始しますか？
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Enter_ConnectYesNoSelect( EMAIL_MENU_WORK *wk )
{
	int ret = BmpYesNoSelectMain( wk->YesNoMenuWork, HEAPID_EMAIL );

	if(ret!=BMPMENU_NULL){
		if(ret==BMPMENU_CANCEL){
			// 接続を終了しますか？
//			Enter_MessagePrint( wk, wk->MsgManager, msg_gtc_01_008, 1, 0, 0x0f0f );
//			Email_SetNextSeq( wk, ENTER_MES_WAIT_YESNO_START, ENTER_END_YESNO_SELECT );
//			wk->subprocess_seq = ENTER_END_START;

			// 終了
			CommStateWifiEMailEnd();	// 通信エラー後タイトルに戻るように専用の関数を作成 080603 tomoya
//			CommStateWifiDPWEnd();
			Email_SubProcessChange( wk->esys, EMAIL_SUBPROC_END, 0 );
			wk->subprocess_seq  = ENTER_END;

		}else{
			// WIFIせつぞくを開始
			Enter_MessagePrint( wk, wk->LobbyMsgManager, msg_wifilobby_002, 1, 0x0f0f );
			Email_SetNextSeq( wk, ENTER_MES_WAIT, ENTER_INTERNET_CONNECT );
			Email_TimeIconAdd(wk);

		}
	}

	return SUBSEQ_CONTINUE;
	
}


//------------------------------------------------------------------
/**
 * $brief   既に選択は終わっているのでWIFIから接続する
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Enter_ForceEndStart( EMAIL_MENU_WORK *wk ) 
{
	// 接続を終了します
	Enter_MessagePrint( wk, wk->SystemMsgManager, dwc_message_0011, 1, 0x0f0f );
	Email_SetNextSeq( wk, ENTER_MES_WAIT, ENTER_FORCE_END );

	return SUBSEQ_CONTINUE;
}

//------------------------------------------------------------------
/**
 * $brief   接続終了
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Enter_ForceEnd( EMAIL_MENU_WORK *wk )
{
    // 通信エラー管理のために通信ルーチンをOFF
	CommStateWifiEMailEnd();	// 通信エラー後タイトルに戻るように専用の関数を作成 080603 tomoya
//    CommStateWifiDPWEnd();
	// WIFIせつぞくを終了
    DWC_CleanupInet();
    WirelessIconEasyEnd();
	sys_SleepOK(SLEEPTYPE_COMM);

    //画面を抜けずに2度連続で「Eメール設定」をしてWifiに繋げようとすると
    //"dpw_tr.c:150 Panic:dpw tr is already initialized."
    //のエラーが出るのできちんとこの終了関数を呼ぶようにする 2007.10.26(金) matsuda
	if(wk->esys->dpw_tr_init == TRUE){
	    Dpw_Tr_End();
	    wk->esys->dpw_tr_init = 0;
	}

//	Email_SubProcessChange( wk, WORLDTRADE_ENTER, 0 );
	wk->subprocess_seq  = ENTER_FORCE_END_MESSAGE;

	return SUBSEQ_CONTINUE;
	
}

//------------------------------------------------------------------
/**
 * @brief   接続終了メッセージ
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Enter_ForceEndMessage( EMAIL_MENU_WORK *wk )
{
	Enter_MessagePrint( wk, wk->SystemMsgManager, dwc_message_0012, 1, 0x0f0f );
	Email_SetNextSeq( wk, ENTER_MES_WAIT_1_SECOND, ENTER_END );
		
	return SUBSEQ_CONTINUE;
}


//------------------------------------------------------------------
/**
 * $brief   インターネット接続開始
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Enter_InternetConnect( EMAIL_MENU_WORK *wk )
{
	switch(wk->local_seq){
	case 0:
		// 通信エラー管理のために通信ルーチンをON
		CommStateWifiEMailStart( wk->esys->savedata );	// エラー後タイトルに戻るように専用の関数を作成 080603tomoya 
//		CommStateWifiDPWStart( wk->esys->savedata );
		// Wifi通信アイコン
	    WirelessIconEasy();
		// WIFIせつぞくを開始
		Enter_MessagePrint( wk, wk->LobbyMsgManager, msg_wifilobby_002, 1, 0x0f0f );
		Email_TimeIconAdd(wk);
		wk->local_seq++;
		break;
	case 1:
		if( Enter_MessagePrintEndCheck( wk->MsgIndex )==FALSE){
			wk->local_seq++;
		}
		break;
	case 2:
		sys_SleepNG(SLEEPTYPE_COMM);
		DWC_InitInetEx(&wk->esys->stConnCtrl,COMM_DMA_NO,COMM_POWERMODE,COMM_SSL_PRIORITY);
	    DWC_SetAuthServer(GF_DWC_CONNECTINET_AUTH_TYPE);
		DWC_ConnectInetAsync();
		
		wk->subprocess_seq = ENTER_INTERNET_CONNECT_WAIT;
		OS_TPrintf("InternetConnet Start\n");
		break;
	}
	return SUBSEQ_CONTINUE;
}

//------------------------------------------------------------------
/**
 * $brief   ネット接続待ち
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Enter_InternetConnectWait( EMAIL_MENU_WORK *wk )
{
	DWC_ProcessInet();

	if (DWC_CheckInet())
	{
		switch (DWC_GetInetStatus())
		{
		case DWC_CONNECTINET_STATE_ERROR:
			{
				// エラー表示
				DWCError err;
				int errcode;
                DWCErrorType errtype;
				err = DWC_GetLastErrorEx(&errcode,&errtype);
				wk->ErrorRet  = err;
				wk->ErrorCode = errcode;
				wk->ErrorType = errtype;

				OS_TPrintf("   Error occurred %d %d.\n", err, errcode);
			}
			DWC_ClearError();
			DWC_CleanupInet();
		    WirelessIconEasyEnd();
		    CommStateWifiEMailEnd();
			sys_SleepOK(SLEEPTYPE_COMM);

		    //画面を抜けずに2度連続で「Eメール設定」をしてWifiに繋げようとすると
		    //"dpw_tr.c:150 Panic:dpw tr is already initialized."
		    //のエラーが出るのできちんとこの終了関数を呼ぶようにする 2007.10.26(金) matsuda
			if(wk->esys->dpw_tr_init == TRUE){
			    Dpw_Tr_End();
			    wk->esys->dpw_tr_init = 0;
			}

			Email_TimeIconDel( wk );
			wk->subprocess_seq = ENTER_DWC_ERROR_PRINT;

			break;
		case DWC_CONNECTINET_STATE_NOT_INITIALIZED:
		case DWC_CONNECTINET_STATE_IDLE:
		case DWC_CONNECTINET_STATE_OPERATING:
		case DWC_CONNECTINET_STATE_OPERATED:
		case DWC_CONNECTINET_STATE_DISCONNECTING:
		case DWC_CONNECTINET_STATE_DISCONNECTED:
		default:
			OS_TPrintf("DWC_CONNECTINET_STATE_DISCONNECTED!\n");
			//break;
		case DWC_CONNECTINET_STATE_FATAL_ERROR:
			{
				// エラー表示
				DWCError err;
				int errcode;
				err = DWC_GetLastError(&errcode);

				OS_TPrintf("   Error occurred %d %d.\n", err, errcode);
				Email_TimeIconDel(wk);
				wk->subprocess_seq = ENTER_SERVER_SERVICE_ERROR;
				wk->ConnectErrorNo = DPW_TR_ERROR_SERVER_TIMEOUT;
			}
			break;

		case DWC_CONNECTINET_STATE_CONNECTED:
	        {	// 接続先を表示する。店舗の場合は店舗情報も表示する。
				DWCApInfo apinfo;
	
				DWC_GetApInfo(&apinfo);
	
	            OS_TPrintf("   Connected to AP type %d.\n", apinfo.aptype);

	            if (apinfo.aptype == DWC_APINFO_TYPE_SHOP)
	            {
					OS_TPrintf("<Shop AP Data>\n");
	                OS_TPrintf("area code: %d.\n", apinfo.area);
	                OS_TPrintf("spotinfo : %s.\n", apinfo.spotinfo);
	            }
	        }
	        // コネクト成功？
			wk->subprocess_seq = ENTER_WIFI_CONNECTION_LOGIN;
			break;
		}
		
		// 時間アイコン消去

	}
	
	return SUBSEQ_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief   GameSpyサーバーログイン開始
 *
 * @param   wk		
 *
 * @retval  int
 */
//------------------------------------------------------------------
static int Enter_WifiConnectionLogin( EMAIL_MENU_WORK *wk )
{
	DWC_NASLoginAsync();
	wk->subprocess_seq = ENTER_WIFI_CONNECTION_LOGIN_WAIT;
	OS_Printf("GameSpyサーバーログイン開始\n");

	return SUBSEQ_CONTINUE;
}
//------------------------------------------------------------------
/**
 * @brief   GameSpyサーバーログイン処理待ち
 *
 * @param   wk
 *
 * @retval  int
 */
//------------------------------------------------------------------
static int Enter_WifiConnectionLoginWait( EMAIL_MENU_WORK *wk )
{
	switch(DWC_NASLoginProcess()){
	case DWC_NASLOGIN_STATE_SUCCESS:
		OS_Printf("GameSpyサーバーログイン成功\n");
		wk->subprocess_seq = ENTER_DPWTR_INIT;
		break;
	case DWC_NASLOGIN_STATE_ERROR:
	case DWC_NASLOGIN_STATE_CANCELED:
	case DWC_NASLOGIN_STATE_DIRTY:
		Email_TimeIconDel(wk);
		OS_Printf("GameSpyサーバーログイン失敗\n");
		{
			int errCode;
			DWCErrorType errType;
			DWCError dwcError;
			dwcError = DWC_GetLastErrorEx( &errCode, &errType );
			wk->ErrorRet  = dwcError;
			wk->ErrorCode = errCode;

			DWC_ClearError();
			DWC_CleanupInet();
		    WirelessIconEasyEnd();
		    CommStateWifiEMailEnd();
			sys_SleepOK(SLEEPTYPE_COMM);

		    //画面を抜けずに2度連続で「Eメール設定」をしてWifiに繋げようとすると
		    //"dpw_tr.c:150 Panic:dpw tr is already initialized."
		    //のエラーが出るのできちんとこの終了関数を呼ぶようにする 2007.10.26(金) matsuda
			if(wk->esys->dpw_tr_init == TRUE){
			    Dpw_Tr_End();
			    wk->esys->dpw_tr_init = 0;
			}

			//ありえないはずだが、どのエラーにも引っかからない可能性を考慮し、初期値として次のシーケンスを先に設定しておく
			wk->subprocess_seq = ENTER_DWC_ERROR_PRINT;

			switch(errType){
			case DWC_ETYPE_LIGHT:
			case DWC_ETYPE_SHOW_ERROR:
				wk->subprocess_seq = ENTER_DWC_ERROR_PRINT;
				break;
			case DWC_ETYPE_SHUTDOWN_GHTTP:
				DWC_ShutdownGHTTP();
				wk->subprocess_seq = ENTER_DWC_ERROR_PRINT;
				break;
			case DWC_ETYPE_DISCONNECT:
				wk->subprocess_seq = ENTER_DWC_ERROR_PRINT;
				break;
			case DWC_ETYPE_SHUTDOWN_FM:
				OS_TPrintf("DWC_ETYPE_SHUTDOWN_FM!\n");
				DWC_ShutdownFriendsMatch();
				wk->subprocess_seq = ENTER_DWC_ERROR_PRINT;
				break;
			case DWC_ETYPE_SHUTDOWN_ND:	//このシーケンスではありえないので一応強制ふっとばしにする
				OS_TPrintf("DWC_ETYPE_SHUTDOWN_ND!\n");
				//break;
			case DWC_ETYPE_FATAL:
				// 強制ふっとばし
				CommFatalErrorFunc_NoNumber();
				break;
			}

			// 20000番台をキャッチしたらerrTypeが何であろうとリセットエラーへ
			if(errCode<-20000 && errCode >=-29999){
//				CommSetErrorReset(COMM_ERROR_RESET_TITLE);
				OS_Printf("dwcError = %d  errCode = %d, errType = %d\n", dwcError, errCode, errType);
				wk->subprocess_seq = ENTER_DWC_ERROR_PRINT;
			}
		}
		break;
	}
	
	
	return SUBSEQ_CONTINUE;
}



//------------------------------------------------------------------
/**
 * $brief   世界交換サーバー接続ライブラリ初期化
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Enter_DpwTrInit( EMAIL_MENU_WORK *wk )
{
	// 世界交換接続初期化
	DWCUserData		*MyUserData;		// 認証済みのDWCUSERデータしかこないはず
	s32 profileId;
	SYSTEMDATA *systemdata;
	WIFI_LIST *wifilist;
	
	wifilist = SaveData_GetWifiListData(wk->esys->savedata);
	systemdata = SaveData_GetSystemData(wk->esys->savedata);

	// DWCUser構造体取得
	MyUserData = WifiList_GetMyUserInfo(wifilist);

	// このFriendKeyはプレイヤーが始めて取得したものか？
	profileId = SYSTEMDATA_GetDpwInfo( systemdata );
	if( profileId==0 ){
		OS_TPrintf("初回取得profileIdなのでDpwInfoとして登録した %08x \n", mydwc_getMyGSID(SaveData_GetWifiListData(wk->esys->savedata)));

		// 初回取得FriendKeyなので、DpwIdとして保存する
		SYSTEMDATA_SetDpwInfo( systemdata, mydwc_getMyGSID(wifilist) );
	}

	
	// 正式なデータを取得
	profileId = SYSTEMDATA_GetDpwInfo( systemdata );
	OS_Printf("Dpwサーバーログイン情報 profileId=%08x\n", profileId);

	// DPW_TR初期化
	Dpw_Tr_Init( profileId, DWC_CreateFriendKey( MyUserData ) );
	wk->esys->dpw_tr_init = TRUE;

	OS_TPrintf("Dpw Trade 初期化\n");

	wk->subprocess_seq = ENTER_SERVER_START;
	
	return SUBSEQ_CONTINUE;
}

//------------------------------------------------------------------
/**
 * $brief   Eメール認証のサーバーとのやり取り開始
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Enter_ServerStart( EMAIL_MENU_WORK *wk )
{
	Email_DCProfileCreate(wk->esys);
	Email_DCProfileSet_Address(wk->esys);
	Dpw_Tr_SetProfileAsync(&wk->esys->dc_profile, &wk->esys->dc_profile_result);
//	Dpw_Tr_GetServerStateAsync();

	OS_TPrintf("プロフィール送信\n");

	// サーバー状態確認待ちへ
	wk->subprocess_seq = ENTER_SERVER_RESULT;
	wk->timeout_count = 0;

	return SUBSEQ_CONTINUE;
}

//------------------------------------------------------------------
/**
 * $brief   サーバー状態確認待ち
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Enter_ServerResult( EMAIL_MENU_WORK *wk )
{
	if (Dpw_Tr_IsAsyncEnd()){
		s32 result = Dpw_Tr_GetAsyncResult();
		wk->timeout_count = 0;
		switch (result){
		case DPW_TR_STATUS_SERVER_OK:		// 正常に動作している
			OS_TPrintf(" profile is up!\n");

			//プロフィールの結果ワークを確認
			switch(wk->esys->dc_profile_result.code){
			case DPW_PROFILE_RESULTCODE_SUCCESS:	//情報の登録に成功
				switch(wk->esys->dc_profile_result.mailAddrAuthResult){
				case DPW_PROFILE_AUTHRESULT_SEND:	//認証メール送信した
					wk->subprocess_seq = ENTER_AUTH_MAIL_WAIT;
					break;
				case DPW_PROFILE_AUTHRESULT_SENDFAILURE:	//認証メールの送信に失敗
					OS_TPrintf(" 認証メール送信失敗\n");
					Email_TimeIconDel(wk);
					wk->ConnectErrorNo = EMAIL_ENTRY_ERROR_SENDFAILURE;
					wk->subprocess_seq = ENTER_SERVER_SERVICE_ERROR;
					break;
				//以下のエラー処理はこのシーンでは想定していないメール認証の結果が返った場合
				// (自分の友達コードが変化したときに以前と同じメールアドレスとパスワードを送
				// 信したときにこのようになる可能性があります。通常はあり得ません。)マニュアル引用
				case DPW_PROFILE_AUTHRESULT_SUCCESS:	//認証成功
					OS_TPrintf(" mail service error\n");
					Email_TimeIconDel(wk);
					wk->ConnectErrorNo = EMAIL_ENTRY_ERROR_SUCCESS;
					wk->subprocess_seq = ENTER_SERVER_SERVICE_ERROR;
					break;
				case DPW_PROFILE_AUTHRESULT_FAILURE:	//認証に失敗
					OS_TPrintf(" mail service error\n");
					Email_TimeIconDel(wk);
					wk->ConnectErrorNo = EMAIL_ENTRY_ERROR_FAILURE;
					wk->subprocess_seq = ENTER_SERVER_SERVICE_ERROR;
					break;
				default:	//ありえないけど一応。強制ふっとばし
					CommFatalErrorFunc_NoNumber();
					break;
				}
				break;
			case DPW_PROFILE_RESULTCODE_ERROR_INVALIDPARAM:	//プロフィールの送信パラメータ不正
				OS_TPrintf(" server stop service.\n");
				Email_TimeIconDel(wk);
				wk->ConnectErrorNo = EMAIL_ENTRY_ERROR_INVALIDPARAM;
				wk->subprocess_seq = ENTER_SERVER_SERVICE_ERROR;
				break;
			case DPW_PROFILE_RESULTCODE_ERROR_SERVERSTATE:	//サーバメンテナンスor一時停止中
				OS_TPrintf(" server stop service.\n");
				Email_TimeIconDel(wk);
				wk->ConnectErrorNo = EMAIL_ENTRY_ERROR_SERVERSTATE;
				wk->subprocess_seq = ENTER_SERVER_SERVICE_ERROR;
				break;
			default:
				// 即ふっとばし
				OS_TPrintf("default error!\n");
				Email_TimeIconDel(wk);
				CommFatalErrorFunc_NoNumber();
				break;
			}
			break;
		case DPW_TR_STATUS_SERVER_STOP_SERVICE:	// サービス停止中
			OS_TPrintf(" server stop service.\n");
			Email_TimeIconDel(wk);
			wk->ConnectErrorNo = result;
			wk->subprocess_seq = ENTER_SERVER_SERVICE_ERROR;
			break;
		case DPW_TR_STATUS_SERVER_FULL:			// サーバーが満杯
		case DPW_TR_ERROR_SERVER_FULL:
			OS_TPrintf(" server full.\n");
			Email_TimeIconDel(wk);
			wk->ConnectErrorNo = result;
			wk->subprocess_seq = ENTER_SERVER_SERVICE_ERROR;
			break;

		case DPW_TR_ERROR_CANCEL :
		case DPW_TR_ERROR_FAILURE :
			// 「GTSのかくにんにしっぱいしました」→タイトルへ
			Email_TimeIconDel(wk);
			wk->ConnectErrorNo = result;
			wk->subprocess_seq = ENTER_SERVER_SERVICE_ERROR;
			break;

		case DPW_TR_ERROR_SERVER_TIMEOUT :
		case DPW_TR_ERROR_DISCONNECTED:	
			// サーバーと通信できません→終了
			OS_TPrintf(" upload error. %d \n", result);
			Email_TimeIconDel(wk);
			wk->ConnectErrorNo = result;
			wk->subprocess_seq = ENTER_SERVER_SERVICE_ERROR;
			break;
		case DPW_TR_ERROR_FATAL:			//!< 通信致命的エラー。電源の再投入が必要です
		default:
			// 即ふっとばし
			Email_TimeIconDel(wk);
			CommFatalErrorFunc_NoNumber();
			break;

		}
		
	}
	else{
		wk->timeout_count++;
		if(wk->timeout_count == TIMEOUT_TIME){
			CommFatalErrorFunc_NoNumber();	//強制ふっとばし
		}
	}
//	Email_TimeIconDel(wk);
	return SUBSEQ_CONTINUE;
}


//--------------------------------------------------------------
/**
 * @brief   プレイヤー自身の認証メール受信待ち
 *
 * @param   wk		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
static int Enter_AuthMailWait(EMAIL_MENU_WORK *wk)
{
	if( Enter_MessagePrintEndCheck( wk->MsgIndex )==TRUE){
		return SUBSEQ_CONTINUE;
	}

	switch(wk->local_seq){
	case 0:
		Email_TimeIconDel(wk);
		Enter_MessagePrint( wk, wk->EmailMsgManager, msg_email_105, 1, 0x0f0f );
		wk->local_seq++;
		break;
	case 1:	//はい・いいえウィンドウ表示
		wk->YesNoMenuWork = Email_BmpWinYesNoMake(wk->bgl, EMAIL_YESNO_PY2, YESNO_OFFSET );
		wk->local_seq++;
		break;
	case 2:
		{
			int ret = BmpYesNoSelectMain( wk->YesNoMenuWork, HEAPID_EMAIL );

			if(ret!=BMPMENU_NULL){
				if(ret==BMPMENU_CANCEL){
					//Eメール登録を中断しますか？
					wk->subprocess_seq = ENTER_AUTH_MAIL_END_YESNO;
				}
				else{
					wk->local_seq++;
				}
			}
		}
		break;
	case 3:
		//しも4桁の登録コードを入力してください
		Enter_MessagePrint( wk, wk->EmailMsgManager, msg_email_105_04, 1, 0x0f0f );
		Email_SetNextSeq( wk, ENTER_MES_WAIT, ENTER_AUTH_INPUT_PROC_CHANGE );
		break;
	}
	return SUBSEQ_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   登録コード入力画面呼び出し
 *
 * @param   wk		
 *
 * @retval  
 */
//--------------------------------------------------------------
static int Enter_AuthInputProcChange(EMAIL_MENU_WORK *wk)
{
	//登録コード入力画面へ移行
	Email_SubProcessChange( wk->esys, 
			EMAIL_SUBPROC_AUTHENTICATE_INPUT, EMAIL_MODE_INPUT_AUTHENTICATE );
	Email_RecoveryMenuModeSet( wk->esys, ENTER_AUTHENTICATE_RETURN);
	wk->subprocess_seq = ENTER_CONNECT_END;
	return SUBSEQ_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   認証メール受信後の、「Eメール登録を中断しますか？」選択
 *
 * @param   wk		
 *
 * @retval  
 */
//--------------------------------------------------------------
static int Enter_AuthMailEndYesNo(EMAIL_MENU_WORK *wk)
{
	if( Enter_MessagePrintEndCheck( wk->MsgIndex )==TRUE){
		return SUBSEQ_CONTINUE;
	}
	
	switch(wk->local_seq){
	case 0:
		//Eメール登録を中断しますか？
		Enter_MessagePrint( wk, wk->EmailMsgManager, msg_email_105_01, 1, 0x0f0f );
		wk->local_seq++;
		break;
	case 1:
		wk->YesNoMenuWork = Email_BmpWinYesNoMake(wk->bgl, EMAIL_YESNO_PY2, YESNO_OFFSET );
		wk->local_seq++;
		break;
	case 2:
		{
			int ret = BmpYesNoSelectMain( wk->YesNoMenuWork, HEAPID_EMAIL );

			if(ret!=BMPMENU_NULL){
				if(ret==BMPMENU_CANCEL){
					wk->subprocess_seq = ENTER_AUTH_MAIL_WAIT;
				}
				else{
					Email_SetNextSeq( wk, ENTER_CLEANUP_INET, ENTER_MENU_LIST );
				}
			}
		}
		break;
	}
	return SUBSEQ_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   登録コード入力画面から戻ってきたときに起動するシーケンス
 *
 * @param   wk		
 *
 * @retval  
 */
//--------------------------------------------------------------
static int Enter_AuthenticateReturn(EMAIL_MENU_WORK *wk)
{
	if(Email_AuthenticateCodeGet(wk->esys) == EMAIL_AUTHENTICATE_CODE_CANCEL){
		wk->subprocess_seq = ENTER_AUTH_MAIL_END_YESNO;
		return SUBSEQ_CONTINUE;
	}
	
	switch(wk->local_seq){
	case 0:
		//登録コードの確認中です
		Enter_MessagePrint( wk, wk->EmailMsgManager, msg_email_105_05, MSG_ALLPUT, 0x0f0f );
		Email_TimeIconAdd(wk);
		
		//Email_DCProfileCreate(wk->esys, &wk->dc_profile, wk->esys->savedata);
		Email_DCProfileSet_Authenticate(wk->esys);
		//登録コードをセットしたプロフィールを送信
		Dpw_Tr_SetProfileAsync(&wk->esys->dc_profile, &wk->esys->dc_profile_result);
		OS_TPrintf("送信した認証コード = %d\n", wk->esys->dc_profile.mailAddrAuthPass);
		wk->local_seq++;
		break;
	case 1:
		if(Dpw_Tr_IsAsyncEnd()){
			s32 result = Dpw_Tr_GetAsyncResult();

			wk->timeout_count = 0;
			Email_TimeIconDel(wk);
			switch (result){
			case DPW_TR_STATUS_SERVER_OK:		// 正常に動作している
				OS_TPrintf(" profile is up!\n");
				//プロフィールの結果ワークを確認
				switch(wk->esys->dc_profile_result.code){
				case DPW_PROFILE_RESULTCODE_SUCCESS:	//情報の登録に成功
					OS_TPrintf("mailAddrAuthResult = %d\n", wk->esys->dc_profile_result.mailAddrAuthResult);
					
					switch(wk->esys->dc_profile_result.mailAddrAuthResult){
					case DPW_PROFILE_AUTHRESULT_SUCCESS:	//認証成功
						OS_TPrintf("認証成功\n");
						wk->local_seq++;
						break;
					case DPW_PROFILE_AUTHRESULT_FAILURE:	//認証に失敗
						OS_TPrintf(" 認証失敗\n");
						//再入力へ
						Enter_MessagePrint( wk, wk->EmailMsgManager, msg_email_105_06, 1, 0x0f0f );
                        // MatchComment: ENTER_AUTH_INPUT_PROC_CHANGE -> ENTER_AUTH_MAIL_END_YESNO
						Email_SetNextSeq( wk, ENTER_MES_WAIT, ENTER_AUTH_MAIL_END_YESNO );
						break;
					//以下のエラー処理はこのシーンでは想定していないメール認証の結果が返った場合
					// (自分の友達コードが変化したときに以前と同じメールアドレスとパスワードを送
					// 信したときにこのようになる可能性があります。通常はあり得ません。)
					// マニュアル引用
					case DPW_PROFILE_AUTHRESULT_SEND:	//認証メール送信した
						OS_TPrintf(" mail service error\n");
						wk->ConnectErrorNo = EMAIL_PASS_ERROR_SEND;
						wk->subprocess_seq = ENTER_SERVER_SERVICE_ERROR;
						break;
					case DPW_PROFILE_AUTHRESULT_SENDFAILURE:	//認証メールの送信に失敗
						OS_TPrintf(" mail service error\n");
						wk->ConnectErrorNo = EMAIL_PASS_ERROR_SENDFAILURE;
						wk->subprocess_seq = ENTER_SERVER_SERVICE_ERROR;
						break;
					default:	//ありえないけど一応。強制ふっとばし
						CommFatalErrorFunc_NoNumber();
						break;
					}
					break;
				case DPW_PROFILE_RESULTCODE_ERROR_INVALIDPARAM:	//プロフィールの送信パラメータ不正
					OS_TPrintf(" server stop service.\n");
					wk->ConnectErrorNo = EMAIL_PASS_ERROR_INVALIDPARAM;
					wk->subprocess_seq = ENTER_SERVER_SERVICE_ERROR;
					break;
				case DPW_PROFILE_RESULTCODE_ERROR_SERVERSTATE:	//サーバメンテナンスor一時停止中
					OS_TPrintf(" server stop service.\n");
					wk->ConnectErrorNo = EMAIL_PASS_ERROR_SERVERSTATE;
					wk->subprocess_seq = ENTER_SERVER_SERVICE_ERROR;
					break;
				default:
					GF_ASSERT(0);
					CommFatalErrorFunc_NoNumber();
					break;
				}
				break;
			case DPW_TR_STATUS_SERVER_STOP_SERVICE:	// サービス停止中
				OS_TPrintf(" server stop service.\n");
				wk->ConnectErrorNo = result;
				wk->subprocess_seq = ENTER_SERVER_SERVICE_ERROR;
				break;
			case DPW_TR_STATUS_SERVER_FULL:			// サーバーが満杯
			case DPW_TR_ERROR_SERVER_FULL:
				OS_TPrintf(" server full.\n");
				wk->ConnectErrorNo = result;
				wk->subprocess_seq = ENTER_SERVER_SERVICE_ERROR;
				break;

			case DPW_TR_ERROR_CANCEL :
			case DPW_TR_ERROR_FAILURE :
				// 「GTSのかくにんにしっぱいしました」→タイトルへ
				wk->ConnectErrorNo = result;
				wk->subprocess_seq = ENTER_SERVER_SERVICE_ERROR;
				break;

			case DPW_TR_ERROR_SERVER_TIMEOUT :
			case DPW_TR_ERROR_DISCONNECTED:	
				// サーバーと通信できません→終了
				OS_TPrintf(" upload error. %d \n", result);
				wk->ConnectErrorNo = result;
				wk->subprocess_seq = ENTER_SERVER_SERVICE_ERROR;
				break;
			case DPW_TR_ERROR_FATAL:			//!< 通信致命的エラー。電源の再投入が必要です
			default:
				// 即ふっとばし
				CommFatalErrorFunc_NoNumber();
				break;

			}
		}
		else{
			wk->timeout_count++;
			if(wk->timeout_count == TIMEOUT_TIME){
				CommFatalErrorFunc_NoNumber();	//強制ふっとばし
			}
		}
		break;
	case 2:	//認証成功後の処理
		Enter_MessagePrint( wk, wk->EmailMsgManager, msg_email_106, 1, 0x0f0f );
		wk->local_seq++;
		break;
	case 3:
		if( Enter_MessagePrintEndCheck( wk->MsgIndex )==FALSE){
			Email_SetNextSeq( wk, ENTER_CLEANUP_INET, ENTER_PASSWORD_SETUP );
		}
		break;
	}
	return SUBSEQ_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   パスワード登録画面呼び出し
 *
 * @param   wk		
 *
 * @retval  
 */
//--------------------------------------------------------------
static int Enter_PasswordSetup(EMAIL_MENU_WORK *wk)
{
	if( Enter_MessagePrintEndCheck( wk->MsgIndex )==TRUE){
		return SUBSEQ_CONTINUE;
	}

	switch(wk->local_seq){
	case 0:
		Enter_MessagePrint( wk, wk->EmailMsgManager, msg_email_107, 1, 0x0f0f );
		wk->local_seq++;
		break;
	case 1:
		Email_SubProcessChange( wk->esys, EMAIL_SUBPROC_PASSWORD_INPUT, EMAIL_MODE_INPUT_PASSWORD);
		Email_RecoveryMenuModeSet(wk->esys, ENTER_PASSWORD_SETUP_CHECK);
		wk->subprocess_seq = ENTER_CONNECT_END;
		break;
	}
	return SUBSEQ_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   パスワード登録画面呼び出し
 *
 * @param   wk		
 *
 * @retval  
 */
//--------------------------------------------------------------
static int Enter_PasswordSetupCheck(EMAIL_MENU_WORK *wk)
{
	if( Enter_MessagePrintEndCheck( wk->MsgIndex )==TRUE){
		return SUBSEQ_CONTINUE;
	}

	switch(wk->local_seq){
	case 0:
		Enter_MessagePrint( wk, wk->EmailMsgManager, msg_email_110, 1, 0x0f0f );
		wk->local_seq++;
		break;
	case 1:
		Email_AddressSaveWorkSet(wk->esys);
		Email_AuthenticateCodeSaveWorkSet(wk->esys);
		Email_PasswordSaveWorkSet(wk->esys);
		Email_SetNextSeq( wk, ENTER_SAVE, ENTER_MENU_LIST );
		break;
	}
	return SUBSEQ_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   セーブ実行
 *
 * @param   wk		
 *
 * @retval  
 *
 * ・セーブ後の飛び先はEmail_SetNextSeqで設定しておいてください
 */
//--------------------------------------------------------------
static int Enter_Save(EMAIL_MENU_WORK *wk)
{
	switch(wk->local_seq){
	case 0:
		//セーブしています。電源切らないでください
		Enter_MessagePrint( wk, wk->EmailMsgManager, msg_email_300, MSG_ALLPUT, 0x0f0f );
		Email_TimeIconAdd(wk);
		wk->local_seq++;
		break;
	case 1:
		{
			SAVE_RESULT result;
			
			result = SaveData_Save(wk->esys->savedata);
			if(result == SAVE_RESULT_OK){
				WORDSET_RegisterPlayerName(wk->WordSet,0,SaveData_GetMyStatus(wk->esys->savedata));
				Enter_MessagePrint( wk, wk->EmailMsgManager, msg_email_301, MSG_ALLPUT, 0x0f0f );
				Snd_SePlay(SEQ_SE_DP_SAVE);
			}
			else{
				Enter_MessagePrint( wk, wk->EmailMsgManager, msg_email_302, MSG_ALLPUT, 0x0f0f );
			}
			Email_TimeIconDel(wk);
			wk->local_seq++;
		}
		break;
	case 2:
		if( Enter_MessagePrintEndCheck( wk->MsgIndex )==FALSE){
			wk->local_seq++;
		}
		break;
	case 3:
		wk->local_wait++;
		if(wk->local_wait > 60){
			wk->local_wait = 0;
			wk->subprocess_seq = wk->subprocess_nextseq;
		}
		break;
	}
	return SUBSEQ_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   Wi-Fiコネクションから切断
 *
 * @param   wk		
 *
 * @retval  
 *
 * ・切断メッセージを表示します。
 * ・切断後の飛び先はEmail_SetNextSeqで設定しておいてください
 */
//--------------------------------------------------------------
static int Enter_CleanupInet( EMAIL_MENU_WORK *wk )
{
	if( Enter_MessagePrintEndCheck( wk->MsgIndex )==TRUE){
		return SUBSEQ_CONTINUE;
	}

	switch(wk->local_seq){
	case 0:
		Enter_MessagePrint( wk, wk->EmailMsgManager, msg_email_105_02, 1, 0x0f0f );
		wk->local_seq++;
		break;
	case 1:
		// WIFIせつぞくを終了
//		if(DWC_CheckInet()){
		    DWC_CleanupInet();
		    WirelessIconEasyEnd();
//		}
	    // 通信エラー管理のために通信ルーチンをOFF
	    CommStateWifiEMailEnd();	// 通信エラー後タイトルに戻るように専用の関数を作成 080603 tomoya
//	    CommStateWifiDPWEnd();
		sys_SleepOK(SLEEPTYPE_COMM);
	    
	    //画面を抜けずに2度連続で「Eメール設定」をしてWifiに繋げようとすると
	    //"dpw_tr.c:150 Panic:dpw tr is already initialized."
	    //のエラーが出るのできちんとこの終了関数を呼ぶようにする 2007.10.26(金) matsuda
		if(wk->esys->dpw_tr_init == TRUE){
		    Dpw_Tr_End();
		    wk->esys->dpw_tr_init = 0;
		}
	    wk->local_seq++;
	    break;
	case 2:
		if(Enter_MessagePrintEndCheck( wk->MsgIndex )==FALSE){
			Enter_MessagePrint( wk, wk->EmailMsgManager, msg_email_105_03, 1, 0x0f0f );
			wk->local_seq++;
		}
		break;
	case 3:
		if(Enter_MessagePrintEndCheck( wk->MsgIndex )==FALSE){
			wk->local_seq++;
		}
		break;
	case 4:
		wk->wait++;
		if(wk->wait > WAIT_ONE_SECONDE_NUM){
			wk->subprocess_seq  = wk->subprocess_nextseq;
		}
		break;
	}
	return SUBSEQ_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief   Wifi接続エラーを表示
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Enter_DwcErrorPrint( EMAIL_MENU_WORK *wk )
{
	int type;
	
    type =  mydwc_errorType(-wk->ErrorCode,wk->ErrorType);


	OS_Printf("error code = %d, type = %d\n", wk->ErrorCode, type);

    errorDisp(wk, type, -wk->ErrorCode);


	wk->subprocess_seq = ENTER_ERROR_PAD_WAIT;
	
	return SUBSEQ_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief   プリント後キー待ち
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Enter_ErrorPadWait( EMAIL_MENU_WORK *wk )
{
	if(sys.trg & PAD_BUTTON_DECIDE || sys.trg & PAD_BUTTON_CANCEL){
		BmpMenuWinClear( &wk->SubWin, WINDOW_TRANS_ON );
		wk->subprocess_seq = ENTER_MENU_LIST;//ENTER_START;
	}
	return SUBSEQ_CONTINUE;
}


//------------------------------------------------------------------
/**
 * $brief   サブプロセスシーケンス終了処理
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Enter_End( EMAIL_MENU_WORK *wk)
{
	if(wk->subprocess_seq != ENTER_CONNECT_END){
	    CommStateWifiEMailEnd();	// 通信エラー後タイトルに戻るように専用の関数を作成 080603 tomoya
//		CommStateWifiDPWEnd();
	}

    WirelessIconEasyEnd();

	// 時間アイコン消去２重解放にならないようにNULLチェックしつつ
	Email_TimeIconDel( wk );

	
	WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, 
		WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_EMAIL );
	
	wk->subprocess_seq = 0;
//	wk->sub_out_flg = 1;
	
	return SUBSEQ_END;
}

//------------------------------------------------------------------
/**
 * $brief   はい・いいえ
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Enter_YesNo( EMAIL_MENU_WORK *wk)
{
	wk->YesNoMenuWork = Email_BmpWinYesNoMake(wk->bgl, EMAIL_YESNO_PY2, YESNO_OFFSET );
	wk->subprocess_seq = wk->subprocess_nextseq;

	return SUBSEQ_CONTINUE;
	
}


//==============================================================================
/**
 * $brief   ネットには繋がったけどサーバーエラーだった表示
 *
 * @param   wk		
 *
 * @retval  none		
 */
//==============================================================================
static int Enter_ServerServiceError( EMAIL_MENU_WORK *wk )
{
	int msgno =0;
	int msgman_select = 0;
	
	switch(wk->ConnectErrorNo){
	case DPW_TR_STATUS_SERVER_STOP_SERVICE:
		msgno = msg_email_error_001;
		break;
	case DPW_TR_STATUS_SERVER_FULL:
	case DPW_TR_ERROR_SERVER_FULL:
		msgno = msg_email_error_002;
		break;
	case DPW_TR_ERROR_SERVER_TIMEOUT:
	case DPW_TR_ERROR_DISCONNECTED:
		// ＧＴＳとのせつぞくがきれました。
		msgno = msg_email_error_006;
		break;
	case DPW_TR_ERROR_CANCEL  :
	case DPW_TR_ERROR_FAILURE :
	case DPW_TR_ERROR_NO_DATA:
	case DPW_TR_ERROR_ILLIGAL_REQUEST :
	default:
		//　つうしんエラーが発生しました。
		msgno = msg_email_error_005;
		break;
	case EMAIL_ENTRY_ERROR_SENDFAILURE:
		msgno = msg_gtc_email_error_000;
		msgman_select++;
		break;
	case EMAIL_ENTRY_ERROR_SUCCESS:
		msgno = msg_gtc_email_error_001;
		msgman_select++;
		break;
	case EMAIL_ENTRY_ERROR_FAILURE:
		msgno = msg_gtc_email_error_001;
		msgman_select++;
		break;
	case EMAIL_ENTRY_ERROR_INVALIDPARAM:
		msgno = msg_gtc_email_error_002;
		msgman_select++;
		break;
	case EMAIL_ENTRY_ERROR_SERVERSTATE:
		msgno = msg_gtc_email_error_003;
		msgman_select++;
		break;
	case EMAIL_PASS_ERROR_SEND:
		msgno = msg_gtc_email_error_001;
		msgman_select++;
		break;
	case EMAIL_PASS_ERROR_SENDFAILURE:
		msgno = msg_gtc_email_error_001;
		msgman_select++;
		break;
	case EMAIL_PASS_ERROR_INVALIDPARAM:
		msgno = msg_gtc_email_error_002;
		msgman_select++;
		break;
	case EMAIL_PASS_ERROR_SERVERSTATE:
		msgno = msg_gtc_email_error_003;
		msgman_select++;
		break;
	}
	// エラー表示
	if(msgman_select == 0){
		Enter_MessagePrint( wk, wk->EmailMsgManager, msgno, 1, 0x0f0f );
	}
	else{
		Enter_MessagePrint( wk, wk->MsgManager, msgno, 1, 0x0f0f );
	}
	Email_SetNextSeq( wk, ENTER_MES_WAIT, ENTER_SERVER_SERVICE_END );

	OS_TPrintf("Error発生\n");

	return SUBSEQ_CONTINUE;
}

//==============================================================================
/**
 * $brief   サーバーサービスの問題で終了
 *
 * @param   wk		
 *
 * @retval  none		
 */
//==============================================================================
static int Enter_ServerServiceEnd( EMAIL_MENU_WORK *wk )
{
	switch(wk->local_seq){
	case 0:
		Enter_MessagePrint( wk, wk->EmailMsgManager, msg_email_105_02, 1, 0x0f0f );
		wk->local_seq++;
		break;
	case 1:
		if(Enter_MessagePrintEndCheck(wk->MsgIndex) == FALSE){
		    // 通信エラー管理のために通信ルーチンをOFF
			CommStateWifiEMailEnd();	// 通信エラー後タイトルに戻るように専用の関数を作成 080603 tomoya
		//    CommStateWifiDPWEnd();
		    DWC_CleanupInet();
		    WirelessIconEasyEnd();
			sys_SleepOK(SLEEPTYPE_COMM);

		    //画面を抜けずに2度連続で「Eメール設定」をしてWifiに繋げようとすると
		    //"dpw_tr.c:150 Panic:dpw tr is already initialized."
		    //のエラーが出るのできちんとこの終了関数を呼ぶようにする 2007.10.26(金) matsuda
			if(wk->esys->dpw_tr_init == TRUE){
			    Dpw_Tr_End();
			    wk->esys->dpw_tr_init = 0;
			}
			wk->local_seq++;
		}
		break;
	case 2:
		Enter_MessagePrint( wk, wk->EmailMsgManager, msg_email_105_03, 1, 0x0f0f );
		wk->local_seq++;
		break;
	case 3:
		if(Enter_MessagePrintEndCheck( wk->MsgIndex )==FALSE){
			wk->local_seq++;
		}
		break;
	default:
		wk->wait++;
		if(wk->wait > WAIT_ONE_SECONDE_NUM){
			Email_SubProcessEndSet(wk->esys);
			wk->subprocess_seq  = ENTER_END;
		}
		break;
	}
	
	return SUBSEQ_CONTINUE;
}

//------------------------------------------------------------------
/**
 * $brief   会話終了を待って次のシーケンスへ
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Enter_MessageWait( EMAIL_MENU_WORK *wk )
{
	if( Enter_MessagePrintEndCheck( wk->MsgIndex )==FALSE){
		wk->subprocess_seq = wk->subprocess_nextseq;
	}
	return SUBSEQ_CONTINUE;

}

//------------------------------------------------------------------
/**
 * @brief   会話表示後1秒待つ
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Enter_MessageWait1Second( EMAIL_MENU_WORK *wk )
{
	if( Enter_MessagePrintEndCheck( wk->MsgIndex )==FALSE){
		if(wk->wait > WAIT_ONE_SECONDE_NUM){
			wk->subprocess_seq = wk->subprocess_nextseq;
		}
		wk->wait++;
	}
	return SUBSEQ_CONTINUE;
	
}

//------------------------------------------------------------------
/**
 * $brief   会話表示を待った上で「はい・いいえ」を開始する
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Enter_MessageWaitYesNoStart(EMAIL_MENU_WORK *wk)
{
	if( Enter_MessagePrintEndCheck( wk->MsgIndex )==FALSE){
		wk->YesNoMenuWork = Email_BmpWinYesNoMake(wk->bgl, EMAIL_YESNO_PY2, YESNO_OFFSET );
		wk->subprocess_seq = wk->subprocess_nextseq;
	}
	return SUBSEQ_CONTINUE;
	
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
static void Enter_MessagePrint( EMAIL_MENU_WORK *wk, MSGDATA_MANAGER *msgman, int msgno, int wait, u16 dat )
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

	wk->wait = 0;
	
	if(wait == MSG_NO_PUT || wait == MSG_ALLPUT){
		wk->MsgIndex = EMAIL_MSG_NO_WAIT;
	}
}

//--------------------------------------------------------------
/**
 * @brief   メッセージの終了処理チェック
 *
 * @param   msg_index		メッセージIndex
 *
 * @retval  TRUE:処理中。　　FALSE:終了している
 */
//--------------------------------------------------------------
static int Enter_MessagePrintEndCheck(int msg_index)
{
	if(msg_index == EMAIL_MSG_NO_WAIT || GF_MSG_PrintEndCheck( msg_index )==0){
		return FALSE;	//描画終了している
	}
	return TRUE;	//メッセージ処理続行中
}

// NONMATCHING
asm static int ov98_2249798(EMAIL_MENU_WORK *wk)
{
	push {r3, r4, lr}
	sub sp, #4
	add r4, r0, #0
	ldr r0, [r4, #0x44]
	bl Enter_MessagePrintEndCheck
	cmp r0, #1
	bne _022497AE
	add sp, #4
	mov r0, #0
	pop {r3, r4, pc}
_022497AE:
	add r0, r4, #0
	add r0, #0x94
	ldr r0, [r0, #0]
	cmp r0, #0
	beq _022497BE
	cmp r0, #1
	beq _022497EA
	b _022497EE
_022497BE:
	ldr r0, [r4, #0]
	bl Email_AddressReturnFlagGet
	cmp r0, #2
	bne _022497CC
	mov r2, #0x26
	b _022497CE
_022497CC:
	mov r2, #0x27
_022497CE:
	ldr r0, =0x00000F0F // _022497F4
	mov r3, #1
	str r0, [sp]
	ldr r1, [r4, #0x34]
	add r0, r4, #0
	bl Enter_MessagePrint
	add r0, r4, #0
	add r0, #0x94
	ldr r0, [r0, #0]
	add r4, #0x94
	add r0, r0, #1
	str r0, [r4, #0]
	b _022497EE
_022497EA:
	mov r0, #5
	str r0, [r4, #8]
_022497EE:
	mov r0, #0
	add sp, #4
	pop {r3, r4, pc}
	// .align 2, 0
// _022497F4: .4byte 0x00000F0F
}

// NONMATCHING
asm static int ov98_22497F8(EMAIL_MENU_WORK *wk)
{
	push {r3, r4, r5, r6, r7, lr}
	sub sp, #0x18
	add r6, r0, #0
	ldr r0, [r6, #0x44]
	ldr r4, [r6, #0]
	bl Enter_MessagePrintEndCheck
	cmp r0, #1
	bne _02249810
	add sp, #0x18
	mov r0, #0
	pop {r3, r4, r5, r6, r7, pc}
_02249810:
	add r0, r6, #0
	add r0, #0x94
	ldr r0, [r0, #0]
	cmp r0, #0
	beq _02249820
	cmp r0, #1
	beq _02249884
	b _02249888
_02249820:
	add r0, r4, #0
	bl Email_PasswordNumberGet
	add r5, r0, #0
	ldr r0, [r4, #4]
	mov r1, #3
	bl EMAILSAVE_ParamGet
	cmp r5, r0
	bne _02249866
	ldr r0, [r4, #4]
	bl EMAILSAVE_AddressGet
	add r1, sp, #8
	mov r2, #0x6c
	bl ov98_2249ACC
	mov r4, #0
	add r5, sp, #8
	mov r7, #2
_02249848:
	str r7, [sp]
	mov r0, #1
	str r0, [sp, #4]
	ldr r0, [r6, #0x20]
	ldr r2, [r5, #0]
	add r1, r4, #0
	mov r3, #4
	bl WORDSET_RegisterNumber
	add r4, r4, #1
	add r5, r5, #4
	cmp r4, #4
	blo _02249848
	mov r2, #0x29
	b _02249868
_02249866:
	mov r2, #0x28
_02249868:
	ldr r0, =0x00000F0F // _02249890
	mov r3, #1
	str r0, [sp]
	ldr r1, [r6, #0x34]
	add r0, r6, #0
	bl Enter_MessagePrint
	add r0, r6, #0
	add r0, #0x94
	ldr r0, [r0, #0]
	add r6, #0x94
	add r0, r0, #1
	str r0, [r6, #0]
	b _02249888
_02249884:
	mov r0, #0
	str r0, [r6, #8]
_02249888:
	mov r0, #0
	add sp, #0x18
	pop {r3, r4, r5, r6, r7, pc}
	nop
// _02249890: .4byte 0x00000F0F
}

//------------------------------------------------------------------
/**
 * @brief   
 *
 * @param   win		
 * @param   strbuf		
 * @param   flag		1だとセンタリング、２だと右よせ
 * @param   color		
 * @param   font		フォント指定（FONT_TALKかFONT_SYSTEM
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int printCommonFunc( GF_BGL_BMPWIN *win, STRBUF *strbuf, int x, int flag, GF_PRINTCOLOR color, int font )
{
	int length=0,ground;
	switch(flag){
	// センタリング
	case 1:
		length = FontProc_GetPrintStrWidth( font, strbuf, 0 );
		x          = ((win->sizx*8)-length)/2;
		break;

	// 右寄せ
	case 2:
		length = FontProc_GetPrintStrWidth( font, strbuf, 0 );
		x          = (win->sizx*8)-length;
		break;
	}
	return x;
}

//------------------------------------------------------------------
/**
 * $brief   BMPWIN内の表示位置を指定してFONT_TALKでプリント(描画のみ）
 *
 * @param   win		GF_BGL_BMPWIN
 * @param   strbuf	
 * @param   x		X座標ずらす値
 * @param   y		Y座標ずらす値
 * @param   flag	0だと左寄せ、1だとセンタリング、2だと右寄せ
 * @param   color	文字色指定（背景色でBMPを塗りつぶします）
 *
 * @retval  none
 */
//------------------------------------------------------------------
void Email_TalkPrint( GF_BGL_BMPWIN *win, STRBUF *strbuf, int x, int y, int flag, GF_PRINTCOLOR color )
{
	x = printCommonFunc( win, strbuf, x, flag, color, FONT_TALK );

	GF_STR_PrintColor( win, FONT_TALK, strbuf, x, y, MSG_ALLPUT, color,NULL);
	
}

//------------------------------------------------------------------
/**
 * @brief   ｗｉｆｉエラー表示
 *
 * @param   wk		
 * @param   msgno		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void _systemMessagePrint( EMAIL_MENU_WORK *wk, int msgno )
{
	STRBUF *tmpString = STRBUF_Create( DWC_ERROR_BUF_NUM, HEAPID_EMAIL );
    MSGMAN_GetString(  wk->SystemMsgManager, msgno, tmpString );
    WORDSET_ExpandStr( wk->WordSet, wk->ErrorString, tmpString );

    // 会話ウインドウ枠描画
    GF_BGL_BmpWinDataFill(&wk->SubWin, 15 );
    BmpMenuWinWrite(&wk->SubWin, WINDOW_TRANS_OFF, EMAIL_MENUFRAME_CHR, EMAIL_MENUFRAME_PAL );
    // 文字列描画開始
    wk->MsgIndex = GF_STR_PrintSimple( &wk->SubWin, FONT_TALK,
                                       wk->ErrorString, 0, 0, MSG_ALLPUT, NULL);
	wk->MsgIndex = EMAIL_MSG_NO_WAIT;	//一括描画なので
	
	STRBUF_Delete(tmpString);
}

//------------------------------------------------------------------
/**
 * @brief   Wifiコネクションエラーの表示
 *
 * @param   wk		
 * @param   type	
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void errorDisp(EMAIL_MENU_WORK* wk, int type, int code)
{
    int msgno;

    if(type != -1){
        msgno = dwc_error_0001 + type;
    }
    else{
        msgno = dwc_error_0012;
    }
//    EndMessageWindowOff(wk);
    WORDSET_RegisterNumber(wk->WordSet, 0, code,
                           5, NUMBER_DISPTYPE_ZERO, NUMBER_CODETYPE_DEFAULT);

	BmpTalkWinClear( &wk->MsgWin, WINDOW_TRANS_OFF );
    _systemMessagePrint(wk, msgno);

#if 0
    switch(type){
      case 1:
      case 4:
      case 5:
        wk->seq = WIFIP2PMATCH_RETRY_INIT;  // 再接続かフィールドか
        break;
      case 6:
      case 7:
      case 8:
      case 9:
        wk->seq = WIFIP2PMATCH_RETRY_INIT;//WIFIP2PMATCH_POWEROFF_INIT;  // 電源を切るかフィールド
        break;
      case 10:
        wk->seq = WIFIP2PMATCH_RETRY_INIT;  // メニュー一覧へ
        break;
      case 0:
      case 2:
      case 3:
      case 11:
      default:
        wk->seq = WIFIP2PMATCH_MODE_CHECK_AND_END;  // フィールド
        break;
    }
#endif
}




//--------------------------------------------------------------
/**
 * @brief   Eメール画面：トップメニューのリストのカーソル移動ごとのコールバック関数
 *
 * @param   wk		
 * @param   param		
 * @param   mode		
 */
//--------------------------------------------------------------
static void EmailMenuListAllHeader_CursorCallback(BMPLIST_WORK * wk, u32 param, u8 mode)
{
	if(mode == 0){	//初期化時はSEを鳴らさない
		Snd_SePlay(EMAIL_SE_MOVE);
	}
}

//--------------------------------------------------------------
/**
 * @brief   Eメール画面：トップメニューのリストのカーソル移動ごとのコールバック関数
 *
 * @param   wk		
 * @param   param		
 * @param   mode		
 */
//--------------------------------------------------------------
static void EmailMenuListNoDataHeader_CursorCallback(BMPLIST_WORK * wk, u32 param, u8 mode)
{
	if(mode == 0){	//初期化時はSEを鳴らさない
		Snd_SePlay(EMAIL_SE_MOVE);
	}
}

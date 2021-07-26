//==============================================================================
/**
 * @file	br_define.h
 * @brief	数値まとめてるとこ
 * @author	goto
 * @date	2008.03.12(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
#ifndef __BR_DEFINE_H__
#define __BR_DEFINE_H__

//	----------------------------------------
//
//		デバッグ
//
//	----------------------------------------
#ifdef PM_DEBUG

//#define BR_AUTO_SERCH
#ifdef BR_AUTO_SERCH
	extern int g_debug_auto;
	extern int g_debug_wait;
	extern u64 g_debug_count;	
#endif

#endif


// -----------------------------------------
//
//	タスクのプライオリティ
//
// -----------------------------------------
enum {
	eVINTR_TCB_PRI_VIEW		= 3,
	eVINTR_TCB_PRI_CHANGE,
	eVINTR_TCB_PRI_BAR,
	
	eTCB_PRI_TOUCH_ACTION	= 0x1000,	
	eTCB_PRI_TAG_MOVE		= 0x2000,	
};


#define BB_INFO_WAIT			( 30 * 2 )					///< 自動で消える時間


#define HEAPID_BR				( HEAPID_BATTLE_RECORDER )	///< HEAPID
#define HEAP_SIZE_BR			( 0x55000 )					///< 60000以内にしときたい。

#define BR_MSG_CLEAR_CODE		( 0xCC )

#define SUB_SURFACE_FX_Y_BR		( 256 * FX32_ONE )			///< サーフェイス
#define SUB_SURFACE_Y_BR		( 256 )
#define SUB_SURFACE_FX_Y_N		( 192 * FX32_ONE )
#define SUB_SURFACE_Y_N			( 192 )

///< タグの座標管理
#define TAG_INIT_POS( n,m )		( 25 + ( ( 36 ) * ( n ) ) + ( 16 * ( 5 - ( m ) ) ) )
#define TAG_INIT_POS_2( n,m )	( 25 + ( ( 36 ) * ( n ) ) + ( 16 * ( 4 - ( m ) ) ) )
#define TAG_INIT_POS_U( n )		( 25 + ( ( 36 ) * ( n ) ) )
#define TAG_MAIN_POS			( 42 )
#define TAG_HIT_X				( TAG_MAIN_POS + 8 )
#define TAG_HIT_Y( n )			( TAG_INIT_POS( n, 4 ) + 0 )


#define BR_TAG_BMP_WIX_SX	( 20 )
#define BR_TAG_BMP_WIX_SY	( 2 )
#define BMPWIN_CHAR_OFS		( 256 )
#define BR_DRESS_VIEW_MAX	( 5 )		///< ドレスアップの最大数
#define BR_UNION_CHAR_MAX	( 16 )		///< ユニオンキャラの最大数

#define PRINT_COL_BOOT		( GF_PRINTCOLOR_MAKE(  1, 14,  0 ) )	// 起動時の文字色
#define PRINT_COL_NORMAL	( GF_PRINTCOLOR_MAKE( 15, 13,  0 ) )	// よく使う文字色
#define	PRINT_COL_BLACK		( GF_PRINTCOLOR_MAKE( 14, 13,  0 ) )	// フォントカラー
#define	PRINT_COL_INFO		( GF_PRINTCOLOR_MAKE( 15, 13, 12 ) )	// フォントカラー
#define	PRINT_COL_PHOTO		( PRINT_COL_NORMAL )
#define	PRINT_COL_BOX_SHOT	( PRINT_COL_NORMAL )
#define	PRINT_COL_BOX_NAME	( GF_PRINTCOLOR_MAKE( 15,  1,  0 ) )
#define	PRINT_COL_FREC		( PRINT_COL_NORMAL )
#define	PRINT_COL_PROF		( PRINT_COL_NORMAL )

#define INPUT_MODE_TOUCH	( TRUE )
#define INPUT_MODE_KEY		( FALSE )
#define INPUT_MODE_DEF		( INPUT_MODE_TOUCH )

#define SIDE_BAR_WAIT		( 3 )
#define SIDE_BAR_SIN_MOVE_F	( 4 )
#define SIDE_BAR_OAM		( 2 )
#define SIDE_BAR_L			( 3 )
#define SIDE_BAR_R			( SIDE_BAR_L * 2 )
#define SIDE_BAR_TOTAL		( SIDE_BAR_R )

#define WEEKLY_RANK_DATA_MAX	( 9999999999999999999 )

///< フェード対象のカラービット
#define LOGO_PALETTE			( 0x0002 )
#define TAG_SUB_PALETTE			( 0x000C )		///< 
#define LINE_OTHER_SUB_PALETTE	( 0x000C )		///< 
#define TAG_PALETTE				( 0x000C )		///< 001-1100
#define POKE_ICON_PALETTE		( 0x00F0 )		///< 111-1100

///< ロゴの色 パレット１の１色目
#define LOGO_COLOR_B			( 0x73fa )		///< ブラウズモード 通常
#define LOGO_COLOR_B2			( 0x771f )
#define LOGO_COLOR_B3			( 0x6f7b )
#define LOGO_COLOR_B4			( 0x6fff )
#define LOGO_COLOR_B5			( 0x4ebf )
#define LOGO_COLOR_B6			( 0x575e )
#define LOGO_COLOR_B7			( 0x5b9f )
#define LOGO_COLOR_B8			( 0xFFFF )
#define LOGO_COLOR_B9			( 0xFFFF )
#define LOGO_COLOR_G			( 0x7fdd )		///< グローバルモード

///< ラインの色 パレット０の１色目
#define LINE_COLOR_B			( 0x1642 )		///< ブラウズモード 通常
#define LINE_COLOR_B2			( 0x357f )
#define LINE_COLOR_B3			( 0x3def )
#define LINE_COLOR_B4			( 0x031f )
#define LINE_COLOR_B5			( 0x00bc )
#define LINE_COLOR_B6			( 0x0131 )
#define LINE_COLOR_B7			( 0x023f )
#define LINE_COLOR_B8			( 0xFFFF )
#define LINE_COLOR_B9			( 0xFFFF )
#define LINE_COLOR_G			( 0x7e05 )		///< グローバルモード

#define CHANGE_COLOR_MAX		( 7 )			///< いろがえできる数

#define SIDE_BAR_MOVE_F			( 10 )			///< サイドバーの移動フレーム

#define	TAG_MAX					( 5 )			///< タグの個数
#define TAG_CIRCLE_CX			( 15 + 1 )		///< くるくるカーソルのオフセット
#define TAG_INIT_RAD			( 90 )			///< タグの角度
#define TAG_VANISH_TOP			( -48 )			///< 見えない位置
#define TAG_VANISH_BOTTOM		( 48 )			///< 下の見えない位置
#define TAG_VANISH_BOTTOM_INIT	( 32 )			///< 数が少ないときの初期値
#define TAG_UP_MOVE_F			( 8 )			///< 上にタグが来るスピード
#define TAG_UP_SUB_MOVE_F		( 4 )			///< サブ画面のタグのスピード
#define TAG_DOWN_MOVE_F			( 8 )			///< 下に下がるときのタグのスピード

#define MENU_IN_SPEED			( 1 )					///< メニューがでるスピード ( 初期 = x1 )
#define TAG_FADE_SPPED			( 1 * MENU_IN_SPEED )	///< タグのフェードスピード
#define TAG_IN_WAIT				( 4 / MENU_IN_SPEED )	///< タグのインサート待ち
#define TAG_MOVE_VALUE_U		( 8 * MENU_IN_SPEED )	///< タグの移動量
#define TAG_RAD_VALUE_U			( -4 * MENU_IN_SPEED )	///< タグの回転角度

#define MENU_OUT_SPEED			( 1 )					///< メニューが戻るスピード ( 初期 = x1 )
#define TAG_MOVE_VALUE_D		( 8 * MENU_OUT_SPEED )	///< タグの戻るスピード
#define TAG_RAD_VALUE_D			( 4 * MENU_OUT_SPEED )	///< タグの回転角度

#define FONT_OAM_TAG_OX			( 36 )			///< 文字OAMのオフセット
#define FONT_OAM_TAG_OY			( -8 )			///< 文字OAMのオフセット

#define TAG_HIT_HEIGHT			( 16 )
#define TAG_HIT_RIGHT			( 160 )

#define COMM_HORMING_CUR_CX		( 128 )			///< ホーミングカーソル
#define COMM_HORMING_CUR_CY		(  96 )
#define COMM_HORMING_R			(  24 )			///< 送受信中の半径
#define LIST_HORMING_R			(  12 )			///< リストカーソルの半径

#define GLOBAL_POKE_ICON_ID		( 500 )			///< グローバルなグラフィックＩＤ


///< エラーがらみのまとめ
#define ERR_WIN_X				( 1 )
#define ERR_WIN_Y				( 1 )
#define ERR_WIN_SX				( 20 )
#define ERR_WIN_SY				( 12 )
#define ERR_WIN_PAL				( 14 )
#define ERR_WIN_OFS				(  1 )


#define RGBtoY(r,g,b)	(((r)*76 + (g)*151 + (b)*29) >> 8)


#define DTC( n ) ( n * 8 )
#define DEF_MP1		( GX_BLEND_PLANEMASK_BG3 )
#define DEF_MP2		( GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_OBJ )
#define DEF_SP1		( GX_BLEND_PLANEMASK_BG3 )
#define DEF_SP2		( GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_OBJ )

///< スライダー処理
#define BB_SLIDER_HIT_FRAME		( 2 )
#define BB_SLIDER_MOVE_VALUE	( 16 )

///< タッチリスト　今週ならparam_exのスコアは参照しない
#define TL_THIS_WEEK_TR	( 10000 )	///< 今週トレーナー
#define TL_LAST_WEEK_TR	( 20000 )
#define TL_THIS_WEEK_YM	( 30000 )	///< 今週うまれつき
#define TL_LAST_WEEK_YM	( 40000 )
#define TL_THIS_WEEK_MN	( 50000 )	///< 今週ポケモン
#define TL_LAST_WEEK_MN	( 60000 )

#define NOTFOUND_MONSNO	( 0xFFFF )	///< ポケ無し

///< タッチアクション
#define TOUCH_ACTION_DEF_R		( 6 )
#define TOUCH_ACTION_ADD_R		( 2 )
#define TOUCH_ACTION_CUR_R( n )	( TOUCH_ACTION_DEF_R + ( n * TOUCH_ACTION_ADD_R ) )

enum {	
	eBR_BG0_PRI	= 1,
	eBR_BG1_PRI	= 3,
	eBR_BG2_PRI	= 2,
	eBR_BG3_PRI	= 1,
};

enum {	
	eHERO_TOP	= 0,
	eHERO_BOTTOM,
	eHERO_LEFT,
	eHERO_RIGHT,	
};

///< OAM
enum {	
	eOAM_PTI_FONT		= 3,
	eOAM_PRI_TAG_OFS	= 10,
};

///< シーケンス	※メニューデータも増やせ
enum {
	
	eMAIN_Start			= 0,
	eMAIN_Input,
	eMAIN_Profile,
	eMAIN_Frontier,
	eMAIN_PhotoSend,
	eMAIN_PhotoRecv,
	eMAIN_BoxSend,
	eMAIN_BoxRecv,
	
	eMAIN_ThisWeek,
	eMAIN_LastWeek,
	
	eMAIN_CommBattleRanking,
	eMAIN_BattleFrontierRanking,
	
	eMAIN_VideoNew30,
	eMAIN_VideoKuwasiku,
	eMAIN_VideoNoSearch,
	eMAIN_VideoSend,
	
	eMAIN_Blank,	
	eMAIN_Delete,
	
	eMAIN_Error,		///< 特殊な処理
	eMAIN_End			= 0xFF,	
};

///< メニューデータ
#define SEAM_LESS_ON	( TRUE )
#define SEAM_LESS_OFF	( TRUE )
enum {
	
	eMENU_DATA_SEQ_NONE	= 0,
	eMENU_DATA_SEQ_SELECT,
	
	eMENU_DATA_SEQ_END,
	eMENU_DATA_SEQ_BACK,
	
	eMENU_DATA_PROF_INIT,	
	eMENU_DATA_FREC_INIT,
	
	eMENU_DATA_SEQ_BLANK,	
	eMENU_DATA_REC_DELETE,
	
	eMENU_DATA_PHOTO_SEND,
	eMENU_DATA_PHOTO_RECV,
	
	eMENU_DATA_BOX_SEND,
	eMENU_DATA_BOX_RECV,
	
	eMENU_DATA_THIS_WEEK,
	eMENU_DATA_LAST_WEEK,
	
	eMENU_DATA_COMM_BATTLE_RANKING,
	eMENU_DATA_BATTLE_FRONTIER_RANKING,
	
	eMENU_DATA_VIDE_NEW30,
	eMENU_DATA_VIDE_KUWASIKU,
	eMENU_DATA_VIDE_NO_SEARCH,
	eMENU_DATA_VIDE_SEND,
	
	eMUENU_DATA_DUMMY,
};

///< エラーコード
enum {
	
	eERR_DRESS_SEND = 0,
	eERR_DRESS_RECV,
	
	eERR_BOX_SEND,
	eERR_BOX_RECV,
	
	eERR_RANKING_TYPE_RECV,
	eERR_RANKING_SEND,
	
	eERR_VIDEO_SEND,
	eERR_VIDEO_SEARCH,		///< 最新とかランキングも
	eERR_VIDEO_DATA_GET,	///< コード検索
};

///< キャラデータ
enum {
	
	eID_OAM_SIDE_BAR_M	= 9999,
	eID_OAM_SIDE_BAR_S,
	eID_OAM_TAG_M,
	eID_OAM_TAG_S,
	
	eID_OAM_POKE_ICON	= 100000,
	eID_OAM_HERO_ICON	= 100000 + 65535,
	
	eID_OAM_HUM_ICON	= 18181,	///< フロンティアブレイン
	eID_OAM_BG_PHOTO	= 28282,	///< BG
	eID_OAM_BP_NUM		= 29292,	///< BP
	
	eID_OAM_EX_TAG		= 11924,	///< 拡張タグ
	
	eID_LIST_CUR		= 200000,	///< リスト用のカーソル
};

///< Side Bar シーケンス
enum {
	eSB_TCB_STATE_WAIT		= 0,
	eSB_TCB_STATE_MOVE_1,
	eSB_TCB_STATE_MOVE_2,
	eSB_TCB_STATE_STAY,
};

///< OAMパレット
enum {
	
	eOAM_PAL_CUR			= 0,	
	eOAM_PAL_LINE			= 1,
	eOAM_PAL_TAG_1,
	eOAM_PAL_TAG_2,	
//	eOAM_PAL_FONT,
	
	///< プロフィール
	eOAM_PAL_POKE_ICON		= 4,			///<
	eOAM_PAL_POKE_ICON_2,
	eOAM_PAL_POKE_ICON_3,
	eOAM_PAL_PLAYER,
	
	///< BP画面
	eOMA_PAL_BP_NUM			= 4,			///< 下
	eOMA_PAL_BG_OAM			= 4,			///< 上 BGOAMx6
	eOMA_PAL_HUM			= 10,			///< 上 ブレーンx5
	eOAM_OAL_PLAYER			= 15,			///< 上 プレイヤー もうパレットねぇよ！
	
	eOAM_PAL_MAX,
};

#define OAM_TAG_PAL_MAX	( 3 )
#define dOAM_PAL_TAG_1	( eOAM_PAL_TAG_1 - eOAM_PAL_LINE )
#define dOAM_PAL_TAG_2	( eOAM_PAL_TAG_2 - eOAM_PAL_LINE )

///< BG パレット
enum {
	
	eBG_PAL_BASE_0		= 0,
	eBG_PAL_BASE_END	= 8,
	
	eBG_PAL_BOX		= 12,///13も
	eBG_PAL_DRESS	= 13,
	eBG_PAL_FONT	= 14,	
};
#define BG_PAL_BASE_END		( eBG_PAL_BASE_END + 1 )

///< タグコード
enum {
	
	eTAG_FIGHT		= 0,
	eTAG_DELETE,
	eTAG_NONE,
	eTAG_FRONTIER,
	eTAG_END		= 4,
	eTAG_BACK,
	eTAG_MYDATA,
	eTAG_ATHORDATA,	
};

enum {
	
	eTAG_GDS_DETAIL_S		 = 0,		///< くわしくさがす
	eTAG_GDS_FACILITIES_S,				///< しせつでさがす
	eTAG_GDS_LOOK,						///< みる
	eTAG_GDS_VS,						///< VS
	eTAG_GDS_ANOTHER_REC,				///< 誰かの記録
	eTAG_GDS_BACK,						///< もどる
	eTAG_GDS_OK,						///< ＯＫ
	eTAG_GDS_FACTORY_OPEN,				///< ファクトリーオープン
	eTAG_GDS_SAVE,						///< 保存
	eTAG_GDS_SEND,						///< 送信
	eTAG_GDS_NEW30,						///< さいしん３０
	eTAG_GDS_DATANO_S,					///< データナンバーでさがす
	eTAG_GDS_PREV,						///< 後
	eTAG_GDS_NEXT,						///< 前
	eTAG_GDS_END,						///< おわる
};

enum {	
	eTAG_GDS_PHOTO = 0,
	eTAG_GDS_BOX_SHOT,
	eTAG_GDS_BOX_DRESS_END,
	eTAG_GDS_BOX_DRESS_BACK,
	eTAG_GDS_BOX_DRESS_OK,
};

///< セーブ
enum {
	eTAG_GDS_SAVE_DATA = 0,
	eTAG_GDS_SAVE_BACK
};

enum {	
	eTAG_GDS_THIS_WEEK = 0,
	eTAG_GDS_LAST_WEEK,
	eTAG_GDS_WEEKLY_RANK_END,
	eTAG_GDS_RANK_SAVE_DATA,
	eTAG_GDS_RANK_BACK,
	eTAG_GDS_RANK_SAVE_OK,
};

///< ビデオ
enum {	
	eTAG_GDS_VIDEO_KUWASIKU = 0,
	eTAG_GDS_VIDEO_SISETU,
	eTAG_GDS_VIDEO_PLAY,
	eTAG_GDS_VIDEO_BACK,
	eTAG_GDS_VIDEO_OK,
	eTAG_GDS_VIDEO_SAVE,
	eTAG_GDS_VIDEO_SEND,
	eTAG_GDS_VIDEO_NEW30,
	eTAG_GDS_VIDEO_NO_SEARCH,
	eTAG_GDS_VIDEO_END,	
};

///< ビデオランキング
enum {	
	eTAG_GDS_VR_TUUSHIN = 0,
	eTAG_GDS_VR_FRONTIER,
	eTAG_GDS_VR_END,
	eTAG_GDS_VR_1,
	eTAG_GDS_VR_2,	///< もどる
	eTAG_GDS_VR_3,	///< はい
};

///< 拡張タグ
enum {	
	eTAG_EX_BACK = 0,
	eTAG_EX_OK,
	eTAG_EX_END,
	eTAG_EX_SAVE,
	eTAG_EX_SEND,
	
	eTAG_SLIDER,
};

///< ボックスの場合
enum {	
	eTAG_EX_BOX_BACK = 0,
	eTAG_EX_BOX_OK,	
	eTAG_EX_BOX_SLIDER,
};


///< 当たり判定
enum {	
	eHRT_TAG_0	= 0,		///< タグの当たり判定
	eHRT_TAG_1,
	eHRT_TAG_2,
	eHRT_TAG_3,
	eHRT_TAG_4,	
	
	eHRT_MAX,				///< 当たり判定テーブルの最大値
};


// -----------------------------------------
//
//	メニューデータ構造
//
// -----------------------------------------
#define BR_EX_DATA_NONE				( 0xFBFBFBFB )
#define BR_EX_DATA_BOX_SHOT_SEND	( 100 )
#define BR_EX_DATA_VIDEO_SEND		( 100 )
#define BR_EX_DATA_THIS_WEEK_RANK	( 0 )
#define BR_EX_DATA_LAST_WEEK_RANK	( 1 )
#define BR_EX_DATA_BATTLE_RANK		( 300 )
#define BR_EX_DATA_FRONTIER_RANK	( 301 )

#define BR_EX_DATA_NEW30			( 200 )
#define BR_EX_DATA_KUWASIKU			( 210 )
#define BR_EX_DATA_NO_SEARCH		( 220 )

#define BR_EX_WIFI_END				( 999 )
#define BR_EX_SEND_CHECK			( 9999 )	///< 送信確認



#define BR_RECOVER_WORK_MAX			( 8 )		///< 復帰用のワーク


// =============================================================================
//
//
//	■シーケンス定義
//
//
// =============================================================================

// br_start.c
enum {
	eSTART_SEQ_BOOT = 0,
	eSTART_SEQ_FADE,
	eSTART_SEQ_ON,
	eSTART_SEQ_MENU_IN,
};


// br_input.c
enum {
	
	eINPUT_TOUCH	= 0,
	eINPUT_KEY,
	eINPUT_END,
	eINPUT_TAG_SELECT,
	eINPUT_TAG_BACK,
	eINPUT_MENU_IN,
	eINPUT_EXECUTED,		///< ダイレクトに実行する
	eINPUT_COLOR_CHANGE_L,
	eINPUT_COLOR_CHANGE_R,
	
	eINPUT_NONE,
};


// br_profile.c
enum {
	
	ePROF_INIT		= 0,
	ePROF_MAIN,
	ePROF_END,
};


// br_frontier.c
enum {
	
	eFREC_INIT		= 0,
	eFREC_MAIN,
	eFREC_END,
};


// br_photo

///< 基本シーケンス
enum {
	eDRESS_Init = 0,
	eDRESS_Setup,	
	eDRESS_Sep,
};

#define DRESS_SepSeq	( eDRESS_Sep )

///< 送信シーケンス
enum {
	eDRESS_SendIn	= DRESS_SepSeq,
	eDRESS_SendMain,
	eDRESS_SendOut,
	eDRESS_SendMainOut,
};

///< ポケモン指定受信シーケンス
enum {
	eDRESS_PokemonInit	= DRESS_SepSeq,
	eDRESS_PokemonAIUEO,
	eDRESS_PokemonSelect,
	eDRESS_PokemonOut,

	eDRESS_RecvMain,		///< 受信
	
	eDRESS_MainIn,			///< ドレスIN
	eDRESS_Main,			///< メイン
	eDRESS_MainOut,			///< ドレス　→　ポケモン選択
	eDRESS_ProfChange,		///< プロフ　ドレス切り替え
	eDRESS_DataChange,		///< 次のデータ
};

enum {
	ePHOTO_INIT = 0,
	ePHOTO_SETUP,
	ePHOTO_MAIN,
	ePHOTO_FADE_R,
	ePHOTO_END,
	ePHOTO_SEND_FADE_R,
};
enum {
	eWORD_NONE	= 0,	
	eWORD_A		= 1,
	eWORD_KA,
	eWORD_SA,
	eWORD_TA,
	eWORD_NA,
	eWORD_HA,
	eWORD_MA,
	eWORD_YA,
	eWORD_RA,
	eWORD_WA,	
};

enum {
	ePHOTO_SEND_DATA = 0,
	ePHOTO_SEND_BACK,
};

enum {
	ePHOTO_PREVIEW_50ON_SELECT = 0,
	ePHOTO_PREVIEW_50ON_FADE,
	ePHOTO_PREVIEW_50ON_END,
	
	ePHOTO_PREVIEW_POKE_INIT,
	ePHOTO_PREVIEW_POKE_FADE,
	
	ePHOTO_PREVIEW_POKE_SELECT,
	ePHOTO_PREVIEW_POKE_END,
	
	ePHOTO_PREVIEW_RECV_INIT,
	ePHOTO_PREVIEW_RECV,
	
	ePHOTO_PREVIEW_RECV_PHOTO_INIT,
	ePHOTO_PREVIEW_RECV_MAIN,
	ePHOTO_PREVIEW_RECV_END,
	ePHOTO_PREVIEW_RECV_PROF_INIT,
	ePHOTO_PREVIEW_RECV_PROF_RESET,
	
	ePHOTO_PREVIEW_BACK,
};

// br_box.c
enum {	
	eBOX_COMMON_INIT = 0,
	eBOX_SETUP,
	eBOX_INIT,
	eBOX_MAIN,
	eBOX_TRAY_CHANGE,
	eBOX_CATEGORY_IN,
	eBOX_CATEGORY_MAIN,
	eBOX_CATEGORY_OUT,
	eBOX_SEND_OUT,
	eBOX_END_FADE,
	eBOX_EXIT,	
};

enum {	
	eBOX2_COMMON_INIT = 0,
	eBOX2_SETUP,
	eBOX2_INIT,
	eBOX2_MAIN,		
	eBOX2_SELECT_IN,
	eBOX2_SELECT,
	eBOX2_SELECT_OUT,	
	eBOX2_PROF_IN,
	eBOX2_PROF_OUT,	
	eBOX2_BOX_IN,
	eBOX2_BOX,	
	eBOX2_BOX_OUT,
	eBOX2_END_FADE,
	eBOX2_EXIT,	
};

// br_weekly_ranking.c
enum {	
	eRANK_Init = 0,
	eRANK_Setting,
	eRANK_SendRecv,
	eRANK_RankChange,
	eRANK_GroupChange,
	eRANK_MainIn,
	eRANK_Main,
	eRANK_Fade,
	eRANK_Exit,
};

// br_video_ranking.c
enum {
	eVRANK_Init = 0,
	eVRANK_Setting,
	eVRANK_SendRecv,
	eVRANK_MainIn,
	eVRANK_Main,
	eVRANK_Exit,

	eVRANK_ProfIn,
	eVRANK_ProfMain,
	eVRANK_ProfChange,
	eVRANK_Play,
	eVRANK_ProfExit,
	eVRANK_SaveIn,
	eVRANK_SaveMain,
	eVRANK_SaveExit,

	eVRANK_CheckIn,				///< 確認画面いく
	eVRANK_CheckMain,			///< 確認
	eVRANK_CheckExit,			///< 終了～セーブ画面？
	eVRANK_CheckFinish,			///< 終了～セーブ画面？
	
	eVRANK_BrsDataGet,			///< データ取得
	eVRANK_RecoverIn,
	eVRANK_PlayRecover,
};

// br_video.c
enum {
	///< 送信用	
	eVIDEO_Init = 0,
	eVIDEO_Fade_Out,
	eVIDEO_Send_Init,
	eVIDEO_Send_Main,
	eVIDEO_Send_Exit,
};

enum {
	///< データナンバー指定
	eVIDEO_CodeIn_Init = 2,
	eVIDEO_CodeIn_Main,
	eVIDEO_CodeIn_Exit,
	eVIDEO_CodeIn_End,
	eVIDEO_CodeIn_DataSearch,
	
	eVIDEO_CodeIn_ProfIn,
	eVIDEO_CodeIn_ProfMain,
	eVIDEO_CodeIn_ProfChange,	///< 概要・プロフの切り替え
	eVIDEO_CodeIn_Play,
	eVIDEO_CodeIn_ProfExit,		///< もどる

	eVIDEO_CodeIn_SaveIn,		///< セーブ画面行く
	eVIDEO_CodeIn_SaveMain,		///< セーブ画面
	eVIDEO_CodeIn_SaveExit,		///< セーブ画面～プロフィール

	eVIDEO_CodeIn_CheckIn,		///< 確認画面いく
	eVIDEO_CodeIn_CheckMain,	///< 確認
	eVIDEO_CodeIn_CheckExit,	///< 終了～セーブ画面？
	eVIDEO_CodeIn_CheckFinish,	///< 終了～セーブ画面？
	
	eVIDEO_CodeIn_RecoveIn,
	eVIDEO_PlayRecover,
};

enum {
	///< 最新30件
	eN30_DownLoad = 2,			///< 一覧取得
	eN30_ListInit,				///< リスト初期化
	eN30_ListMain,				///< リストメイン
	eN30_ListExit,				///< リスト抜ける	
	eN30_RecoverIn,
};

enum {
	///< 検索	Conditional Search
	eCS_TopViewInit = 2,		///< 検索TOP
	eCS_TopViewMain,
	eCS_TopViewExit,
	
	///< 各設定
	eCS_SearchCommonInit,		///< 下面終了各検索分岐
	eCS_Shisetu,
	eCS_Pokemon,
	eCS_PokemonAIUEO,
	eCS_PokemonSelect,
	eCS_Profile,
	eCS_SearchCommonExit,	
	
	eCS_SearchMain,				///< main
	eCS_SearchExit,				///< exit
	eCS_ListInit,				///< リストの初期化
	eCS_ListMain,				///< リストの初期化	
	eCS_TopViewRecover,			///< 検索画面に戻る

	eCS_ProfIn,					///< プロフィールイン
	eCS_ProfMain,				///< プロフィールメイン
	eCS_ProfChange,				///< 概要・プロフの切り替え
	eCS_Play,					///< 再生
	eCS_ProfExit,				///< もどる
	
	eCS_SaveIn,					///< 保存
	eCS_SaveMain,				///< 保存メイン
	eCS_SaveExit,				///< 終了
	
	eCS_CheckIn,				///< 確認画面いく
	eCS_CheckMain,				///< 確認
	eCS_CheckExit,				///< 終了～セーブ画面？
	eCS_CheckFinish,			///< 終了～セーブ画面？
	
	eCS_BrsDataGet,				///< データ取得
	
	eCS_RecoverIn,				///< 戦闘からの復帰
	
	eCS_PlayRecover,
};


// いろがえ
enum {	
	GET_PAL_FONT = 0,
	GET_PAL_PHOTO,
	GET_PAL_OBJ,
	GET_PAL_BG,	
};

// MatchComment: 10 -> 9 (only 9 entries in US)
extern const RECT_HIT_TBL hit_rect_AIUEO[ 9 ];

#endif

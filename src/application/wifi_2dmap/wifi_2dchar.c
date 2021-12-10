//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifi_2dchar.c
 *	@brief		wifi２Dキャラクタ読み込みシステム
 *	@author		tomoya takahshi
 *	@data		2007.02.07
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"
#include "include/system/arc_util.h"
#include "include/system/clact_util.h"
#include "src/graphic/wifi2dchar.naix"
#include "src/graphic/wifi_unionobj.naix"
#include "src/field/fieldobj_code.h"

#define __WIFI_2DCHAR_H_GLOBAL
#include "application/wifi_2dmap/wifi_2dchar.h"

//-----------------------------------------------------------------------------
/**
 *					コーディング規約
 *		●関数名
 *				１文字目は大文字それ以降は小文字にする
 *		●変数名
 *				・変数共通
 *						constには c_ を付ける
 *						staticには s_ を付ける
 *						ポインタには p_ を付ける
 *						全て合わさると csp_ となる
 *				・グローバル変数
 *						１文字目は大文字
 *				・関数内変数
 *						小文字と”＿”と数字を使用する 関数の引数もこれと同じ
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	キャラクターインデックス
enum {
	WF_2DC_HERO,
	WF_2DC_HEROINE,
	WF_2DC_BOY1,
	WF_2DC_BOY3,
	WF_2DC_MAN3,
	WF_2DC_BADMAN,
	WF_2DC_EXPLORE,
	WF_2DC_FIGHTER,
	WF_2DC_GORGGEOUSM,
	WF_2DC_MYSTERY,
	WF_2DC_GIRL1,
	WF_2DC_GIRL2,
	WF_2DC_WOMAN2,
	WF_2DC_WOMAN3,
	WF_2DC_IDOL,
	WF_2DC_LADY,
	WF_2DC_COWGIRL,
	WF_2DC_GORGGEOUSW,

	WF_2DC_BOY2,
	WF_2DC_GIRL3,
	WF_2DC_CAMPBOY,
	WF_2DC_PICNICGIRL,
	WF_2DC_BABYGIRL1,
	WF_2DC_MIDDLEMAN1,
	WF_2DC_MIDDLEWOMAN1,
	WF_2DC_WAITER,
	WF_2DC_WAITRESS,
	WF_2DC_MAN1,
	WF_2DC_WOMAN1,
	WF_2DC_CAMERAMAN,
	WF_2DC_REPORTER,
	WF_2DC_FARMER,
	WF_2DC_CYCLEM,
	WF_2DC_CYCLEW,
	WF_2DC_OLDMAN1,
	WF_2DC_MAN5,
	WF_2DC_WOMAN5,
	WF_2DC_BABYBOY1,
	WF_2DC_SPORTSMAN,
	WF_2DC_FISHING,
	WF_2DC_SEAMAN,
	WF_2DC_MOUNT,
	WF_2DC_MAN2,
	WF_2DC_BIGMAN,
	WF_2DC_ASSISTANTM,
	WF_2DC_GENTLEMAN,
	WF_2DC_WORKMAN,
	WF_2DC_CLOWN,
	WF_2DC_POLICEMAN,
	WF_2DC_AMBRELLA,
	WF_2DC_PIKACHU,
	WF_2DC_SEVEN1,
	WF_2DC_SEVEN2,
	WF_2DC_SEVEN3,
	WF_2DC_SEVEN4,
	WF_2DC_SEVEN5,
	WF_2DC_TOWERBOSS,

	WF_2DC_ACHAMO,
	WF_2DC_ARTIST,
	WF_2DC_ASSISTANTW,
	WF_2DC_BABY,
	WF_2DC_BAG,
	WF_2DC_BALLOON,
	WF_2DC_BEACHBOY,
	WF_2DC_BEACHGIRL,
	WF_2DC_BIGFOUR1,
	WF_2DC_BIGFOUR2,
	WF_2DC_BIGFOUR3,
	WF_2DC_BIGFOUR4,
	WF_2DC_BIRD,
	WF_2DC_BOY4,
	WF_2DC_CHAMPION,
	WF_2DC_DOCTOR,
	WF_2DC_DSBOY,
	WF_2DC_ENECO,
	WF_2DC_FREEZES,
	WF_2DC_GINGABOSS,
	WF_2DC_GINGAM,
	WF_2DC_GINGAW,
	WF_2DC_GIRL4,
	WF_2DC_GKANBU1,
	WF_2DC_GKANBU2,
	WF_2DC_GKANBU3,
	WF_2DC_KINOCOCO,
	WF_2DC_KODUCK,
	WF_2DC_KOIKING,
	WF_2DC_LEADER1,
	WF_2DC_LEADER2,
	WF_2DC_LEADER3,
	WF_2DC_LEADER4,
	WF_2DC_LEADER5,
	WF_2DC_LEADER6,
	WF_2DC_LEADER7,
	WF_2DC_LEADER8,
	WF_2DC_MAID,
	WF_2DC_MAMA,
	WF_2DC_MIDDLEMAN2,
	WF_2DC_MIDDLEWOMAN2,
	WF_2DC_MIKAN,
	WF_2DC_MIMITUTO,
	WF_2DC_MINUN,
	WF_2DC_MOSS,
	WF_2DC_OLDMAN2,
	WF_2DC_OLDWOMAN1,
	WF_2DC_OLDWOMAN2,
	WF_2DC_OOKIDO,
	WF_2DC_PATIRITUSU,
	WF_2DC_PCWOMAN1,
	WF_2DC_PCWOMAN2,
	WF_2DC_PCWOMAN3,
	WF_2DC_PIPPI,
	WF_2DC_POLE,
	WF_2DC_PRASLE,
	WF_2DC_PURIN,
	WF_2DC_RIVEL,
	WF_2DC_ROCK,
	WF_2DC_SHOPM1,
	WF_2DC_SHOPW1,
	WF_2DC_SKIERM,
	WF_2DC_SKIERW,
	WF_2DC_SPPOKE1,
	WF_2DC_SPPOKE10,
	WF_2DC_SPPOKE11,
	WF_2DC_SPPOKE12,
	WF_2DC_SPPOKE2,
	WF_2DC_SPPOKE3,
	WF_2DC_SPPOKE4,
	WF_2DC_SPPOKE5,
	WF_2DC_SPPOKE6,
	WF_2DC_SPPOKE7,
	WF_2DC_SPPOKE8,
	WF_2DC_SPPOKE9,
	WF_2DC_SUNGLASSES,
	WF_2DC_SWIMMERM,
	WF_2DC_SWIMMERW,
	WF_2DC_TREE,
	WF_2DC_VENTHOLE,
	WF_2DC_WANRIKY,
	WF_2DC_WOMAN6,
	
	// 本素材
	WF_2DC_BRAINS1,
	WF_2DC_BRAINS2,
	WF_2DC_BRAINS3,
	WF_2DC_BRAINS4,
	WF_2DC_PRINCESS,
	WF_2DC_BFSM,
	WF_2DC_BFSW1,
	WF_2DC_BFSW2,
	WF_2DC_WIFISM,
	WF_2DC_WIFISW,
	WF_2DC_DPHERO,
	WF_2DC_DPHEROINE,

	// データとして組み込んでいないもの
/*
	WF_2DC_ICPO,
	WF_2DC_GKANBU4,
	WF_2DC_NAETLE,
	WF_2DC_HAYASHIGAME,
	WF_2DC_DODAITOSE,
	WF_2DC_HIKOZARU,
	WF_2DC_MOUKAZARU,
	WF_2DC_GOUKAZARU,
	WF_2DC_POCHAMA,
	WF_2DC_POTTAISHI,
	WF_2DC_EMPERTE,
	WF_2DC_GUREGGRU,
	WF_2DC_GIRATINAORIGIN,
	WF_2DC_ROTOM,
	WF_2DC_ROTOMF,
	WF_2DC_ROTOMS,
	WF_2DC_ROTOMI,
	WF_2DC_ROTOMW,
	WF_2DC_ROTOMG,
	WF_2DC_DIRECTOR,
	WF_2DC_FIREBOX,
	WF_2DC_SKYBOX,
	WF_2DC_ICEBOX,
	WF_2DC_WATERBOX,
	WF_2DC_GRASSBOX,
	WF_2DC_GSET1,
	WF_2DC_GSET2,
//*/	
	
	WF_2DC_CHARNUM,
} ;
#define WF_2DC_RESM_OBJ_NUM	(WF_2DC_CHARNUM+1)	// リソース数	+1は陰


// リソースタイプ
typedef enum {
	WF_2DC_RES_NML,	// 通常
	WF_2DC_RES_UNI,	// ユニオン
} WF_2DC_RESTYPE;

// アークハンドルタイプ
enum{
	WF_2DC_ARCHANDLE_NML,	// 通常
	WF_2DC_ARCHANDLE_UNI,	// ユニオン
	WF_2DC_ARCHANDLE_NUM,
};



//
#define WF_2DC_UNICHAR_NUM		(16)	// ユニオンキャラクタの数



#define WF_2DC_RESMAN_NUM	(CLACT_U_MULTI_RES)	// 作成するリソースマネージュ数
#define WF_2DC_ANMRESMAN_OBJNUM	( (WF_2DC_MOVENUM*2)+WF_2DC_UNICHAR_NUM+1 )	// セルとアニメのリソースマネージャオブジェクト数	+1は陰
enum{
	WF_2DC_ANMRES_ANM_CELL,
	WF_2DC_ANMRES_ANM_ANM,
	WF_2DC_ANMRES_ANM_NOFLIP_CELL,
	WF_2DC_ANMRES_ANM_NOFLIP_ANM,
	WF_2DC_ANMRES_ANM_NUM,
};
static const u8 sc_WF_2DC_ANMRES_ANMCONTID[ WF_2DC_ANMRES_ANM_NUM ] = {
	0,0,
	1,1,
};
 
// 通常キャラクター
#define WF_2DC_ARC_CHARIDX	( NARC_wifi2dchar_pl_boy01_NCLR )	// キャラクタグラフィックの開始
#define WF_2DC_ARC_GETNCL(x)	(((x)*2)+WF_2DC_ARC_CHARIDX)	// パレット取得
#define WF_2DC_ARC_GETNCG(x)	(WF_2DC_ARC_GETNCL(x) + 1)		// キャラクタ取得

// ユニオンキャラクター
#define WF_2DC_ARC_UNICHARIDX	( NARC_wifi_unionobj_wf_match_top_trainer_boy1_NCGR )	// キャラクタグラフィックの開始
#define WF_2DC_ARC_GETUNINCL	(NARC_wifi_unionobj_wf_match_top_trainer_NCLR)	// パレット取得
#define WF_2DC_ARC_GETUNINCG(x)	(WF_2DC_ARC_UNICHARIDX + ((x) - WF_2DC_BOY1))		// キャラクタ取得
#define WF_2DC_UNIPLTT_NUM		( 8 )	// ユニオンキャラクタが使用するパレット本数
#define WF_2DC_ARC_UNIANMIDX	( NARC_wifi_unionobj_normal00_NCER )	// ユニオンアニメデータ
#define WF_2DC_ARC_GETUNICEL(x)	(WF_2DC_ARC_UNIANMIDX+((x)*2))	// セルdataidゲット
#define WF_2DC_ARC_GETUNIANM(x)	(WF_2DC_ARC_GETUNICEL(x)+1)		// アニメdataidゲット


// リソース管理ID
#define WF_2DC_ARC_CONTANMID	( 0x100 )	// アニメリソース管理ID
#define WF_2DC_ARC_CONTSHADOWID	( 0x120 )	// 陰リソース管理ID
#define WF_2DC_ARC_CONTCHARID	( 0x200 )	// キャラクタリソース管理ID
#define WF_2DC_ARC_CONTUNIONPLTTID	( 0x402 )	// ユニオンパレットリソース管理ID
												// ユニオンで出てくるキャラクタの管理IDの場所なので大丈夫
#define WF_2DC_ARC_CONTUNIONANMID	(0x180)	// ユニオンアニメリソース管理ID

#define WF_2DC_ARC_TURNANMCHARSIZ			(0x800)	// 振り向きのみ時のキャラクタサイズ
#define WF_2DC_ARC_TURNANMCHARSIZ_NOFLIP	(0x800)	// 振り向きのみ時のキャラクタサイズ
#define WF_2DC_ARC_NORMALANMCHARSIZ			(0x1400)// 歩き＋振り向きのみ時のキャラクタサイズ
#define WF_2DC_ARC_NORMALANMCHARSIZ_NOTFLIP	(0x1800)// 歩き＋振り向きのみ時のキャラクタサイズ	フリップなしのキャラのとき


// 各アニメのフレーム数
#define WF_2DC_ANM_WAY_FRAME	(1)		// 向き変えアニメ
#define WF_2DC_ANM_WALK_FRAME	(WF_COMMON_WALK_FRAME)		// 歩きアニメ
#define WF_2DC_ANM_TURN_FRAME	(WF_COMMON_TURN_FRAME)		// 振り向きアニメ
#define WF_2DC_ANM_RUN_FRAME	(WF_COMMON_RUN_FRAME)		// 走りアニメ
#define WF_2DC_ANM_WALLWALK_FRAME	(WF_COMMON_WALLWALK_FRAME)	// 壁歩きアニメ
#define WF_2DC_ANM_SLOWWALK_FRAME	(WF_COMMON_SLOWWALK_FRAME)	// 壁歩きアニメ
#define WF_2DC_ANM_HIGHWALK2_FRAME	(WF_COMMON_HIGHWALK2_FRAME)	// 高速歩きアニメ
#define WF_2DC_ANM_HIGHWALK4_FRAME	(WF_COMMON_HIGHWALK4_FRAME)	// 高速歩きアニメ
#define WF_2DC_ANM_ROTA_FRAME	(0xff)	// 回転アニメ

// アニメのシーケンス数
#define WF_2DC_ANM_ROTA			(4)
#define WF_2DC_ANM_WALK			(5)		// 歩きアニメのシーケンス数
#define WF_2DC_ANM_RUN			(9)		// 走りアニメのシーケンス数
#define WF_2DC_ANM_FRAME		(2*FX32_ONE)	// 16 fure
#define WF_2DC_ANM_SLOWFRAME (FX32_ONE)
#define WF_2DC_ANM_HIGH2 (8*FX32_ONE)			// 
#define WF_2DC_ANM_HIGH4 (6*FX32_ONE)
	
// ユニオンキャラクタテーブル
static const u8 WF_2DC_UnionChar[WF_2DC_UNICHAR_NUM] = {
	WF_2DC_BOY1,
	WF_2DC_BOY3,
	WF_2DC_MAN3,
	WF_2DC_BADMAN,
	WF_2DC_EXPLORE,
	WF_2DC_FIGHTER,
	WF_2DC_GORGGEOUSM,
	WF_2DC_MYSTERY,
	WF_2DC_GIRL1,
	WF_2DC_GIRL2,
	WF_2DC_WOMAN2,
	WF_2DC_WOMAN3,
	WF_2DC_IDOL,
	WF_2DC_LADY,
	WF_2DC_COWGIRL,
	WF_2DC_GORGGEOUSW,
};


// かげ
#define WF_2DC_SHADOW_MAT_OFS_X		( 8 )
#define WF_2DC_SHADOW_MAT_OFS_Y		( 14 )

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	キャラクタリソースデータ
//=====================================
typedef struct {
	WF_2DC_RESTYPE type;

	CLACT_U_RES_OBJ_PTR resobj[2];	//CG PLリソースオブジェ
	u32					drawtype;	// 転送先
	WF_2DC_MOVETYPE		movetype;	// 動作タイプ
	CLACT_HEADER		header;		// アクターヘッダー
} WF_2DCRES;

/*
typedef struct {
	CLACT_U_RES_OBJ_PTR resobj[2];	//CG PLリソースオブジェ
	u32					drawtype;	// 転送先
	WF_2DC_MOVETYPE		movetype;	// 動作タイプ
	CLACT_HEADER		header;		// アクターヘッダー
} WF_2DCRES;
//*/

//-------------------------------------
///	アニメリソースデータ
//=====================================
typedef struct {
	CLACT_U_RES_OBJ_PTR resobj[WF_2DC_ANMRES_ANM_NUM];	// CE ANリソースオブジェ
} WF_2DCANMRES;


//-------------------------------------
/// 2Dキャラクタワーク
//=====================================
typedef struct _WF_2DCWK {
	const WF_2DCRES* cp_res;	// リソースデータ
	CLACT_WORK_PTR	p_clwk;		// 人物
	CLACT_WORK_PTR	p_shadow;	// 陰
	WF_2DC_ANMTYPE	patanmtype;
	WF_COMMON_WAY	patanmway;
	s16				patanmframe;
	s16				pad_0;
} WF_2DCWK;

//-------------------------------------
///	陰リソース
//=====================================
typedef struct {
	CLACT_U_RES_OBJ_PTR resobj[4];	//リソースオブジェ
	CLACT_HEADER		header;		// アクターヘッダー
} WF_2DCSH_RES;


//-------------------------------------
///	2Dキャラクタ管理システム
//=====================================
typedef struct _WF_2DCSYS {
	CLACT_SET_PTR p_clset;
	PALETTE_FADE_PTR p_pfd;
	WF_2DCWK*		p_wk;
	u32 objnum;
	WF_2DCRES		res[ WF_2DC_CHARNUM ];
	WF_2DCANMRES	anmres[ WF_2DC_MOVENUM ];
	WF_2DCANMRES	unionres[ WF_2DC_UNICHAR_NUM ];
	WF_2DCSH_RES	shadowres;
	u32				shadow_pri;		// 陰のソフト優先順位開始位置
	CLACT_U_RES_MANAGER_PTR p_res_man[ WF_2DC_RESMAN_NUM ];
	CLACT_U_RES_OBJ_PTR p_unionpltt;
	ARCHANDLE*	p_handle[WF_2DC_ARCHANDLE_NUM];
} WF_2DCSYS;


//-------------------------------------
///	グラフィックファイルとキャラクタコードをリンク
//するデータの構造体
//=====================================
typedef struct {
	u16 code;
	u16 wf2dc_char;
} WF_2DC_CHARDATA;


//-----------------------------------------------------------------------------
/**
 *					データ部
 */
//-----------------------------------------------------------------------------

///	グラフィックファイルとキャラクタコードをリンク
static const WF_2DC_CHARDATA FIELDOBJCODE_CharIdx[WF_2DC_CHARNUM] = {
	{HERO,		WF_2DC_HERO},
	{BOY1,		WF_2DC_BOY1},
	{BOY3,      WF_2DC_BOY3},
	{MAN3,      WF_2DC_MAN3},
	{BADMAN,    WF_2DC_BADMAN},
	{EXPLORE,   WF_2DC_EXPLORE},
	{FIGHTER,   WF_2DC_FIGHTER},
	{GORGGEOUSM,WF_2DC_GORGGEOUSM},
	{MYSTERY,   WF_2DC_MYSTERY},
	{HEROINE,	WF_2DC_HEROINE},
	{GIRL1,     WF_2DC_GIRL1},
	{GIRL2,     WF_2DC_GIRL2},
	{WOMAN2,    WF_2DC_WOMAN2},
	{WOMAN3,    WF_2DC_WOMAN3},
	{IDOL,      WF_2DC_IDOL},
	{LADY,      WF_2DC_LADY},
	{COWGIRL,   WF_2DC_COWGIRL},
	{GORGGEOUSW,WF_2DC_GORGGEOUSW},
	{BOY2,			WF_2DC_BOY2},
	{GIRL3,			WF_2DC_GIRL3},
	{CAMPBOY,		WF_2DC_CAMPBOY},
	{PICNICGIRL,	WF_2DC_PICNICGIRL},
	{BABYGIRL1,		WF_2DC_BABYGIRL1},
	{MIDDLEMAN1,	WF_2DC_MIDDLEMAN1},
	{MIDDLEWOMAN1,	WF_2DC_MIDDLEWOMAN1},
	{WAITER,		WF_2DC_WAITER},
	{WAITRESS,		WF_2DC_WAITRESS},
	{MAN1,			WF_2DC_MAN1},
	{WOMAN1,		WF_2DC_WOMAN1},
	{CAMERAMAN,		WF_2DC_CAMERAMAN},
	{REPORTER,		WF_2DC_REPORTER},
	{FARMER,		WF_2DC_FARMER},
	{CYCLEM,		WF_2DC_CYCLEM},
	{CYCLEW,		WF_2DC_CYCLEW},
	{OLDMAN1,		WF_2DC_OLDMAN1},
	{MAN5,			WF_2DC_MAN5},
	{WOMAN5,		WF_2DC_WOMAN5},
	{BABYBOY1,		WF_2DC_BABYBOY1},
	{SPORTSMAN,		WF_2DC_SPORTSMAN},
	{FISHING,		WF_2DC_FISHING},
	{SEAMAN,		WF_2DC_SEAMAN},
	{MOUNT,			WF_2DC_MOUNT},
	{MAN2,			WF_2DC_MAN2},
	{BIGMAN,		WF_2DC_BIGMAN},
	{ASSISTANTM,	WF_2DC_ASSISTANTM},
	{GENTLEMAN,		WF_2DC_GENTLEMAN},
	{WORKMAN,		WF_2DC_WORKMAN},
	{CLOWN,			WF_2DC_CLOWN},
	{POLICEMAN,		WF_2DC_POLICEMAN},
	{AMBRELLA,		WF_2DC_AMBRELLA},
	{PIKACHU,		WF_2DC_PIKACHU},
	{SEVEN1,		WF_2DC_SEVEN1},
	{SEVEN2,		WF_2DC_SEVEN2},
	{SEVEN3,		WF_2DC_SEVEN3},
	{SEVEN4,		WF_2DC_SEVEN4},
	{SEVEN5,		WF_2DC_SEVEN5},
	{TOWERBOSS,		WF_2DC_TOWERBOSS},
	{ACHAMO,		WF_2DC_ACHAMO},
	{ARTIST,		WF_2DC_ARTIST},
	{ASSISTANTW,	WF_2DC_ASSISTANTW},
	{BABY,			WF_2DC_BABY},
	{BAG,			WF_2DC_BAG},
	{BALLOON,		WF_2DC_BALLOON},
	{BEACHBOY,		WF_2DC_BEACHBOY},
	{BEACHGIRL,		WF_2DC_BEACHGIRL},
	{BIGFOUR1,		WF_2DC_BIGFOUR1},
	{BIGFOUR2,		WF_2DC_BIGFOUR2},
	{BIGFOUR3,		WF_2DC_BIGFOUR3},
	{BIGFOUR4,		WF_2DC_BIGFOUR4},
	{BIRD,			WF_2DC_BIRD},
	{BOY4,			WF_2DC_BOY4},
	{CHAMPION,		WF_2DC_CHAMPION},
	{DOCTOR,		WF_2DC_DOCTOR},
	{DSBOY,			WF_2DC_DSBOY},
	{ENECO,			WF_2DC_ENECO},
	{FREEZES,		WF_2DC_FREEZES},
	{GINGABOSS,		WF_2DC_GINGABOSS},
	{GINGAM,		WF_2DC_GINGAM},
	{GINGAW,		WF_2DC_GINGAW},
	{GIRL4,			WF_2DC_GIRL4},
	{GKANBU1,		WF_2DC_GKANBU1},
	{GKANBU2,		WF_2DC_GKANBU2},
	{GKANBU3,		WF_2DC_GKANBU3},
	{KINOCOCO,		WF_2DC_KINOCOCO},
	{KODUCK,		WF_2DC_KODUCK},
	{KOIKING,		WF_2DC_KOIKING},
	{LEADER1,		WF_2DC_LEADER1},
	{LEADER2,		WF_2DC_LEADER2},
	{LEADER3,		WF_2DC_LEADER3},
	{LEADER4,		WF_2DC_LEADER4},
	{LEADER5,		WF_2DC_LEADER5},
	{LEADER6,		WF_2DC_LEADER6},
	{LEADER7,		WF_2DC_LEADER7},
	{LEADER8,		WF_2DC_LEADER8},
	{MAID,			WF_2DC_MAID},
	{MAMA,			WF_2DC_MAMA},
	{MIDDLEMAN2,	WF_2DC_MIDDLEMAN2},
	{MIDDLEWOMAN2,	WF_2DC_MIDDLEWOMAN2},
	{MIKAN,			WF_2DC_MIKAN},
	{MIMITUTO,		WF_2DC_MIMITUTO},
	{MINUN,			WF_2DC_MINUN},
	{MOSS,			WF_2DC_MOSS},
	{OLDMAN2,		WF_2DC_OLDMAN2},
	{OLDWOMAN1,		WF_2DC_OLDWOMAN1},
	{OLDWOMAN2,		WF_2DC_OLDWOMAN2},
	{OOKIDO,		WF_2DC_OOKIDO},
	{PATIRITUSU,	WF_2DC_PATIRITUSU},
	{PCWOMAN1,		WF_2DC_PCWOMAN1},
	{PCWOMAN2,		WF_2DC_PCWOMAN2},
	{PCWOMAN3,		WF_2DC_PCWOMAN3},
	{PIPPI,			WF_2DC_PIPPI},
	{POLE,			WF_2DC_POLE},
	{PRASLE,		WF_2DC_PRASLE},
	{PURIN,			WF_2DC_PURIN},
	{RIVEL,			WF_2DC_RIVEL},
	{ROCK,			WF_2DC_ROCK},
	{SHOPM1,		WF_2DC_SHOPM1},
	{SHOPW1,		WF_2DC_SHOPW1},
	{SKIERM,		WF_2DC_SKIERM},
	{SKIERW,		WF_2DC_SKIERW},
	{SPPOKE1,		WF_2DC_SPPOKE1},
	{SPPOKE10,		WF_2DC_SPPOKE10},
	{SPPOKE11,		WF_2DC_SPPOKE11},
	{SPPOKE12,		WF_2DC_SPPOKE12},
	{SPPOKE2,		WF_2DC_SPPOKE2},
	{SPPOKE3,		WF_2DC_SPPOKE3},
	{SPPOKE4,		WF_2DC_SPPOKE4},
	{SPPOKE5,		WF_2DC_SPPOKE5},
	{SPPOKE6,		WF_2DC_SPPOKE6},
	{SPPOKE7,		WF_2DC_SPPOKE7},
	{SPPOKE8,		WF_2DC_SPPOKE8},
	{SPPOKE9,		WF_2DC_SPPOKE9},
	{SUNGLASSES,	WF_2DC_SUNGLASSES},
	{SWIMMERM,		WF_2DC_SWIMMERM},
	{SWIMMERW,		WF_2DC_SWIMMERW},
	{TREE,			WF_2DC_TREE},
	{VENTHOLE,		WF_2DC_VENTHOLE},
	{WANRIKY,		WF_2DC_WANRIKY},
	{WOMAN6,		WF_2DC_WOMAN6},
	{BRAINS1,		WF_2DC_BRAINS1},
	{BRAINS2,		WF_2DC_BRAINS2},
	{BRAINS3,		WF_2DC_BRAINS3},
	{BRAINS4,		WF_2DC_BRAINS4},
	{PRINCESS,		WF_2DC_PRINCESS},
	{BFSM,			WF_2DC_BFSM},
	{BFSW1,			WF_2DC_BFSW1},
	{BFSW2,			WF_2DC_BFSW2},
	{WIFISM,		WF_2DC_WIFISM},
	{WIFISW,		WF_2DC_WIFISW},
	{DPHERO,		WF_2DC_DPHERO},		// DP主人公男
	{DPHEROINE,		WF_2DC_DPHEROINE},	// DP主人公女

/*
	// データとして組み込んでいないもの
	{ICPO,			WF_2DC_ICPO},
	{GKANBU4,		WF_2DC_GKANBU4},
	{NAETLE,		WF_2DC_NAETLE},
	{HAYASHIGAME,	WF_2DC_HAYASHIGAME},
	{DODAITOSE,		WF_2DC_DODAITOSE},
	{HIKOZARU,		WF_2DC_HIKOZARU},
	{MOUKAZARU,		WF_2DC_MOUKAZARU},
	{GOUKAZARU,		WF_2DC_GOUKAZARU},
	{POCHAMA,		WF_2DC_POCHAMA},
	{POTTAISHI,		WF_2DC_POTTAISHI},
	{EMPERTE,		WF_2DC_EMPERTE},
	{GUREGGRU,		WF_2DC_GUREGGRU},
	{GIRATINAORIGIN,WF_2DC_GIRATINAORIGIN},
	{ROTOM,			WF_2DC_ROTOM},
	{ROTOMF,		WF_2DC_ROTOMF},
	{ROTOMS,		WF_2DC_ROTOMS},
	{ROTOMI,		WF_2DC_ROTOMI},
	{ROTOMW,		WF_2DC_ROTOMW},
	{ROTOMG,		WF_2DC_ROTOMG},
	{DIRECTOR,		WF_2DC_DIRECTOR},
	{FIREBOX,		WF_2DC_FIREBOX},
	{SKYBOX,		WF_2DC_SKYBOX},
	{ICEBOX,		WF_2DC_ICEBOX},
	{WATERBOX,		WF_2DC_WATERBOX},
	{GRASSBOX,		WF_2DC_GRASSBOX},
	{GSET1,			WF_2DC_GSET1},
	{GSET2,			WF_2DC_GSET2},
	*/
};

/// 各アニメの終了フレーム数
static const u8 WF_2DC_AnmFrame[WF_2DC_ANMNUM] = {
	WF_2DC_ANM_WAY_FRAME, 
	WF_2DC_ANM_ROTA_FRAME, 
	WF_2DC_ANM_WALK_FRAME,
	WF_2DC_ANM_TURN_FRAME,
	WF_2DC_ANM_RUN_FRAME,
	WF_2DC_ANM_WALLWALK_FRAME,
	WF_2DC_ANM_SLOWWALK_FRAME,
	WF_2DC_ANM_HIGHWALK2_FRAME,
	WF_2DC_ANM_HIGHWALK4_FRAME,
	0,
};

//----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	パレットフェードシステムにパレットカラーを設定
//=====================================
static void WF_2DC_PFDPalSet( WF_2DCSYS* p_sys, CLACT_U_RES_OBJ_PTR p_pal, u32 num );

//-------------------------------------
/// アニメリソース関連
//=====================================
static void WF_2DC_AnmResLoad( WF_2DCSYS* p_sys, WF_2DC_MOVETYPE movetype, u32 heap );
static void WF_2DC_AnmResDel( WF_2DCSYS* p_sys, WF_2DC_MOVETYPE movetype );
static BOOL WF_2DC_AnmResCheck( const WF_2DCSYS* cp_sys, WF_2DC_MOVETYPE movetype );
static u32 WF_2DC_CharNoGet( u32 view_type );
static BOOL WF_2DC_AnmModeLinkCheck( u32 view_type, WF_2DC_MOVETYPE movetype );
static u32 WF_2DC_AnmResContIdGet( WF_2DC_MOVETYPE movetype, u32 res_type, BOOL flip );

//-------------------------------------
/// キャラ・パレットリソース関連
//=====================================
static void WF_2DC_CharResLoad( WF_2DCSYS* p_sys, u32 char_no, u32 draw_type, WF_2DC_MOVETYPE movetype, u32 heap, WF_2DC_RESTYPE restype );
static void WF_2DC_CharResLoadNml( WF_2DCSYS* p_sys, u32 char_no, u32 draw_type, WF_2DC_MOVETYPE movetype, u32 heap );
static void WF_2DC_CharResLoadUni( WF_2DCSYS* p_sys, u32 char_no, u32 draw_type, WF_2DC_MOVETYPE movetype, u32 heap );
static void WF_2DC_CharResDel( WF_2DCSYS* p_sys, u32 char_no );
static void WF_2DC_CharResDelNml( WF_2DCSYS* p_sys, u32 char_no );
static void WF_2DC_CharResDelUni( WF_2DCSYS* p_sys, u32 char_no );
static BOOL WF_2DC_CharResCheck( const WF_2DCSYS* cp_sys, u32 char_no );
static u32 WF_2DC_CharResDrawTypeGet( const WF_2DCRES* cp_res );
static WF_2DC_MOVETYPE WF_2DC_CharResMoveTypeGet( const WF_2DCRES* cp_res );


//-------------------------------------
///	陰リソース関連
//=====================================
static void WF_2DC_ShResLoad( WF_2DCSYS* p_sys, u32 draw_type, u32 heap );
static void WF_2DC_ShResDel( WF_2DCSYS* p_sys );
static BOOL WF_2DC_ShResCheck( const WF_2DCSH_RES* cp_shadowres );



//-------------------------------------
/// ユニオンリソース関連
//=====================================
static void WF_2DC_UniCharPlttResLoad( WF_2DCSYS* p_sys, u32 draw_type, u32 heap );
static void WF_2DC_UniCharPlttResDel( WF_2DCSYS* p_sys );
static void WF_2DC_UniCharAnmResLoad( WF_2DCSYS* p_sys, u32 heap );
static void WF_2DC_UniCharAnmResDel( WF_2DCSYS* p_sys );

//-------------------------------------
/// オブジェワーク操作関連
//=====================================
static WF_2DCWK* WF_2DC_WkCleanGet( WF_2DCSYS* p_sys );
static BOOL WF_2DC_WkCleanCheck( const WF_2DCWK* cp_wk );
static void WF_2DC_WkPatAnmUpdate( WF_2DCWK* p_wk );


//-------------------------------------
/// アニメ管理関連
//=====================================
static void WF_2DC_WkPatAnmWayInit( WF_2DCWK* p_wk, WF_COMMON_WAY way );
static void WF_2DC_WkPatAnmRotaInit( WF_2DCWK* p_wk, WF_COMMON_WAY way );
static void WF_2DC_WkPatAnmWalkInit( WF_2DCWK* p_wk, WF_COMMON_WAY way );
static void WF_2DC_WkPatAnmTurnInit( WF_2DCWK* p_wk, WF_COMMON_WAY way );
static void WF_2DC_WkPatAnmRunInit( WF_2DCWK* p_wk, WF_COMMON_WAY way );
static void WF_2DC_WkPatAnmWallWalkInit( WF_2DCWK* p_wk, WF_COMMON_WAY way );
static void WF_2DC_WkPatAnmSlowWalkInit( WF_2DCWK* p_wk, WF_COMMON_WAY way );
static void WF_2DC_WkPatAnmHighWalk2Init( WF_2DCWK* p_wk, WF_COMMON_WAY way );
static void WF_2DC_WkPatAnmHighWalk4Init( WF_2DCWK* p_wk, WF_COMMON_WAY way );

static void WF_2DC_WkPatAnmRotaMain( WF_2DCWK* p_wk );
static void WF_2DC_WkPatAnmWalkMain( WF_2DCWK* p_wk );
static void WF_2DC_WkPatAnmTurnMain( WF_2DCWK* p_wk );
static void WF_2DC_WkPatAnmRunMain( WF_2DCWK* p_wk );
static void WF_2DC_WkPatAnmWallWalkMain( WF_2DCWK* p_wk );
static void WF_2DC_WkPatAnmSlowWalkMain( WF_2DCWK* p_wk );
static void WF_2DC_WkPatAnmHighWalk2Main( WF_2DCWK* p_wk );
static void WF_2DC_WkPatAnmHighWalk4Main( WF_2DCWK* p_wk );

//----------------------------------------------------------------------------
/**
 *	@brief	WIFI　2D描画システム作成
 *
 *	@param	p_clset		使用するアクターセット
 *	@param	pfd			パレットフェードポインタ
 *	@param	objnum		オブジェクト数
 *	@param	heap		ヒープ
 *
 *	@return	作成したシステムポインタ
 */
//-----------------------------------------------------------------------------
WF_2DCSYS* WF_2DC_SysInit( CLACT_SET_PTR p_clset, PALETTE_FADE_PTR p_pfd, u32 objnum, u32 heap )
{
	WF_2DCSYS* p_sys;
	int i;

	// メモリ確保
	p_sys = sys_AllocMemory( heap, sizeof(WF_2DCSYS) );
	memset( p_sys, 0, sizeof(WF_2DCSYS) );

	// アクターセットは上からもらう
	p_sys->p_clset = p_clset;

	// パレットフェードシステム
	p_sys->p_pfd = p_pfd;

	// アーカイブハンドルオープン
	p_sys->p_handle[WF_2DC_ARCHANDLE_NML] = ArchiveDataHandleOpen( ARC_WIFI2DCHAR, heap );
	p_sys->p_handle[WF_2DC_ARCHANDLE_UNI] = ArchiveDataHandleOpen( ARC_WIFIUNIONCHAR, heap );

	// リソースマネージャ初期化
	for( i=0; i<2; i++ ){	// CG、PLは、キャラクタ分
		p_sys->p_res_man[i] = CLACT_U_ResManagerInit( WF_2DC_RESM_OBJ_NUM, i, heap );
	}
	for( i=0; i<2; i++ ){	// CE,ANはアニメタイプ分
		p_sys->p_res_man[i+2] = CLACT_U_ResManagerInit( WF_2DC_ANMRESMAN_OBJNUM, i+2, heap );
	}

	// アニメーションリソース読み込み
	for( i=0; i<WF_2DC_MOVENUM; i++ ){
		WF_2DC_AnmResLoad( p_sys, i, heap );
	}

	// アクターワーク作成
	p_sys->objnum = objnum;
	p_sys->p_wk = sys_AllocMemory( heap, sizeof(WF_2DCWK)*p_sys->objnum );
	memset( p_sys->p_wk, 0, sizeof(WF_2DCWK)*p_sys->objnum );
 
	return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WIFI　2D描画システム破棄
 *
 *	@param	p_sys	システムワーク
 */
//-----------------------------------------------------------------------------
void WF_2DC_SysExit( WF_2DCSYS* p_sys )
{
	int i;
	
	// ワーク破棄
	for( i=0; i<p_sys->objnum; i++ ){
		if( WF_2DC_WkCleanCheck( &p_sys->p_wk[i] ) == FALSE ){
			WF_2DC_WkDel( &p_sys->p_wk[i] );
		} 
	}
	
	// アニメーションリソース破棄
	for( i=0; i<WF_2DC_MOVENUM; i++ ){
		WF_2DC_AnmResDel( p_sys, i );
	}

	// キャラクタリソース破棄
	WF_2DC_AllResDel( p_sys );

	// アーカイブハンドルクローズ
	ArchiveDataHandleClose( p_sys->p_handle[WF_2DC_ARCHANDLE_NML] );
	ArchiveDataHandleClose( p_sys->p_handle[WF_2DC_ARCHANDLE_UNI] );
	
	// リソースマネージャ破棄
	for( i=0; i<WF_2DC_RESMAN_NUM; i++ ){
		CLACT_U_ResManagerDelete( p_sys->p_res_man[i] );
	}

	// ワーク破棄
	sys_FreeMemoryEz( p_sys->p_wk );
	sys_FreeMemoryEz( p_sys );
}


//----------------------------------------------------------------------------
/**
 *	@brief	リソース登録
 *
 *	@param	p_sys			システムワーク
 *	@param	view_type		見た目（FIELDOBJCODE）
 *	@param	draw_type		転送先
 *	@param	movetype		動作タイプ
 *	@param	heap			ヒープ
 *
 *	draw_type
	mainに登録：NNS_G2D_VRAM_TYPE_2DMAIN
	subに登録 ：NNS_G2D_VRAM_TYPE_2DSUB
	両方に登録：NNS_G2D_VRAM_TYPE_2DMAX
 */
//-----------------------------------------------------------------------------
void WF_2DC_ResSet( WF_2DCSYS* p_sys, u32 view_type, u32 draw_type, WF_2DC_MOVETYPE movetype, u32 heap )
{
	u32 char_no;
	BOOL result;

	// 読み込み可能かチェック
	result = WF_2DC_AnmModeLinkCheck( view_type, movetype );
	GF_ASSERT( result );

	// 見た目と性別からキャラクター番号取得
	char_no = WF_2DC_CharNoGet( view_type );
	
	// 読み込み
	WF_2DC_CharResLoad( p_sys, char_no, draw_type, movetype, heap, WF_2DC_RES_NML );
}

//----------------------------------------------------------------------------
/**
 *	@brief	キャラクタリソース破棄
 *
 *	@param	p_sys		システムワーク
 *	@param	view_type	見た目FIELDOBJCODE
 *
 */
//-----------------------------------------------------------------------------
void WF_2DC_ResDel( WF_2DCSYS* p_sys, u32 view_type )
{
	u32 char_no;

	char_no = WF_2DC_CharNoGet( view_type );

	// リソース破棄
	WF_2DC_CharResDel( p_sys, char_no );
}

//----------------------------------------------------------------------------
/**
 *	@brief	読み込んでいるかチェック
 *
 *	@param	cp_sys		システムワーク
 *	@param	view_type	見た目（FIELDOBJCODE）
 *
 *	@retval	TRUE	読み込んである
 *	@retval	FALSE	読み込んでない
 */
//-----------------------------------------------------------------------------
BOOL WF_2DC_ResCheck( const WF_2DCSYS* cp_sys, u32 view_type )
{
	u32 char_no;

	// 見た目と性別からキャラクター番号取得
	char_no = WF_2DC_CharNoGet( view_type );
	return WF_2DC_CharResCheck( cp_sys, char_no );	
}

//----------------------------------------------------------------------------
/**
 *	@brief	全リソースの破棄
 *
 *	@param	p_sys	システムワーク
 */
//-----------------------------------------------------------------------------
void WF_2DC_AllResDel( WF_2DCSYS* p_sys )
{
	int i;

	// ユニオンリソースの破棄
	if( p_sys->p_unionpltt != NULL ){
		WF_2DC_UnionResDel( p_sys );
	}
	
	// その他のリソースはき
	for( i=0; i<WF_2DC_CHARNUM; i++ ){
		if( WF_2DC_CharResCheck( p_sys, i ) == TRUE ){
			// リソース破棄
			WF_2DC_CharResDel( p_sys, i );
		}
	}

}

//----------------------------------------------------------------------------
/**
 *	@brief	ユニオンキャラクタの登録
 *
 *	@param	p_sys			システムワーク
 *	@param	draw_type		転送先
 *	@param	movetype		動作タイプ
 *	@param	heap			ヒープ
 *
 *	draw_type
	mainに登録：NNS_G2D_VRAM_TYPE_2DMAIN
	subに登録 ：NNS_G2D_VRAM_TYPE_2DSUB
	両方に登録：NNS_G2D_VRAM_TYPE_2DMAX
 */
//-----------------------------------------------------------------------------
void WF_2DC_UnionResSet( WF_2DCSYS* p_sys, u32 draw_type, WF_2DC_MOVETYPE movetype, u32 heap )
{
	int i;

	// 動作可能かチェック
	// 主人公以外は、走り以外受け付けていない
	GF_ASSERT( movetype != WF_2DC_MOVERUN );


	// ユニオンアニメリソース読込み
	WF_2DC_UniCharPlttResLoad( p_sys, draw_type, heap );
	
	// アニメリソース登録
	WF_2DC_UniCharAnmResLoad( p_sys, heap );

	for( i=0; i<WF_2DC_UNICHAR_NUM; i++ ){
		// 読み込み
		WF_2DC_CharResLoad( p_sys, WF_2DC_UnionChar[i], draw_type, movetype, heap, WF_2DC_RES_UNI );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ユニオンキャラクタのリソースはき
 *
 *	@param	p_sys			システムワーク
 */
//-----------------------------------------------------------------------------
void WF_2DC_UnionResDel( WF_2DCSYS* p_sys )
{
	int i;

	// ユニオンパレット破棄
	WF_2DC_UniCharPlttResDel( p_sys );

	// アニメリソース破棄
	WF_2DC_UniCharAnmResDel( p_sys );

	for( i=0; i<WF_2DC_UNICHAR_NUM; i++ ){
		// キャラクタはき
		WF_2DC_CharResDel( p_sys, WF_2DC_UnionChar[i] );
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief	陰のリソースを読み込む
 *
 *	@param	p_sys			システムワーク
 *	@param	draw_type		転送先
 *	@param	shadow_pri		陰の表示優先順位
 *	@param	heap			ヒープID
 *
 *	draw_type
	mainに登録：NNS_G2D_VRAM_TYPE_2DMAIN
	subに登録 ：NNS_G2D_VRAM_TYPE_2DSUB
	両方に登録：NNS_G2D_VRAM_TYPE_2DMAX
 */
//-----------------------------------------------------------------------------
void WF_2DC_ShadowResSet( WF_2DCSYS* p_sys, u32 draw_type, u32 shadow_pri, u32 heap )
{
	WF_2DC_ShResLoad( p_sys, draw_type, heap );
	p_sys->shadow_pri = shadow_pri;
}

//----------------------------------------------------------------------------
/**
 *	@brief	陰のリソースを破棄する
 *	
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
void WF_2DC_ShadowResDel( WF_2DCSYS* p_sys )
{
	WF_2DC_ShResDel( p_sys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	キャラクタアクター登録
 *
 *	@param	p_sys		システムワーク
 *	@param	cp_data		登録データ
 *	@param	view_type	FIELDOBJCODE
 *	@param	movetype	動作タイプ
 *	@param	heap		ヒープ
 *
 *	@return	ワーク
 */
//-----------------------------------------------------------------------------
WF_2DCWK* WF_2DC_WkAdd( WF_2DCSYS* p_sys, const WF_2DC_WKDATA* cp_data, u32 view_type, u32 heap )
{
	WF_2DCWK* p_wk;
	CLACT_ADD add;
	u32 char_no;
	
	// 空のワーク取得
	p_wk = WF_2DC_WkCleanGet( p_sys );

	// キャラクタナンバー取得
	char_no = WF_2DC_CharNoGet( view_type );

	// 読み込み済みチェック
	GF_ASSERT( WF_2DC_CharResCheck( p_sys, char_no ) == TRUE );
	
	memset( &add, 0, sizeof(CLACT_ADD) );
	add.ClActSet = p_sys->p_clset;
	add.ClActHeader = &p_sys->res[ char_no ].header;
	add.mat.x	= cp_data->x << FX32_SHIFT;
	add.mat.y	= cp_data->y << FX32_SHIFT;
	add.sca.x	= FX32_ONE;
	add.sca.y	= FX32_ONE;
	add.sca.z	= FX32_ONE;
	add.pri		= cp_data->pri;
	add.heap	= heap;

	// 表示エリア
	if( (p_sys->res[ char_no ].drawtype == NNS_G2D_VRAM_TYPE_MAX) || 
		(p_sys->res[ char_no ].drawtype == NNS_G2D_VRAM_TYPE_2DMAIN) ){
		add.DrawArea = NNS_G2D_VRAM_TYPE_2DMAIN;
	}else{
		add.DrawArea = NNS_G2D_VRAM_TYPE_2DSUB;
	}
	
	// アクター登録
	p_wk->p_clwk = CLACT_Add( &add );
	CLACT_BGPriorityChg( p_wk->p_clwk, cp_data->bgpri );

	// 動作モードにより初期アニメを変更
	if( p_sys->res[ char_no ].movetype == WF_2DC_MOVENORMAL ){
		// した歩き
		CLACT_AnmChg( p_wk->p_clwk, WF_2DC_ANM_WALK+WF_COMMON_BOTTOM );
	}else{
		// 下向き
		CLACT_AnmChg( p_wk->p_clwk, WF_COMMON_BOTTOM );
	}

	// アニメデータ初期化
	p_wk->patanmtype = WF_2DC_ANMNONE;
	p_wk->patanmframe = 0;
	p_wk->patanmway = WF_COMMON_BOTTOM;

	// リソース保存
	p_wk->cp_res = &p_sys->res[ char_no ];

	// 陰ワークの作成
	if( WF_2DC_ShResCheck( &p_sys->shadowres ) ){

		memset( &add, 0, sizeof(CLACT_ADD) );
		add.ClActSet = p_sys->p_clset;
		add.ClActHeader = &p_sys->shadowres.header;
		add.mat.x	= cp_data->x << FX32_SHIFT;
		add.mat.y	= cp_data->y << FX32_SHIFT;
		add.sca.x	= FX32_ONE;
		add.sca.y	= FX32_ONE;
		add.sca.z	= FX32_ONE;
		add.pri		= p_sys->shadow_pri;
		add.heap	= heap;

		// 表示エリア
		if( (p_sys->res[ char_no ].drawtype == NNS_G2D_VRAM_TYPE_MAX) || 
			(p_sys->res[ char_no ].drawtype == NNS_G2D_VRAM_TYPE_2DMAIN) ){
			add.DrawArea = NNS_G2D_VRAM_TYPE_2DMAIN;
		}else{
			add.DrawArea = NNS_G2D_VRAM_TYPE_2DSUB;
		}

		p_wk->p_shadow = CLACT_Add( &add );

		CLACT_BGPriorityChg( p_wk->p_shadow, cp_data->bgpri );
	}else{
		p_wk->p_shadow = NULL;
	}

	return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	アクター破棄
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void WF_2DC_WkDel( WF_2DCWK* p_wk )
{
	if( p_wk->p_shadow != NULL ){
		CLACT_Delete( p_wk->p_shadow );
	}
	CLACT_Delete( p_wk->p_clwk );
	memset( p_wk, 0, sizeof(WF_2DCWK) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	セルアクターワークポインタの取得
 *
 *	@param	p_wk	ワーク
 *	
 *	@return	セルアクターワーク
 */
//-----------------------------------------------------------------------------
CLACT_WORK_PTR WF_2DC_WkClWkGet( WF_2DCWK* p_wk )
{
	return p_wk->p_clwk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	セルアクターワークポインタの取得
 *
 *	@param	p_wk	ワーク
 *	
 *	@return	セルアクターワーク
 */
//-----------------------------------------------------------------------------
CONST_CLACT_WORK_PTR WF_2DC_WkConstClWkGet( const WF_2DCWK* cp_wk )
{
	return cp_wk->p_clwk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	座標の設定
 *
 *	@param	p_wk	ワーク
 *	@param	x		ｘ座標
 *	@param	y		ｙ座標
 */
//-----------------------------------------------------------------------------
void WF_2DC_WkMatrixSet( WF_2DCWK* p_wk, s16 x, s16 y )
{
	VecFx32	mat;

	mat.x = x<<FX32_SHIFT;
	mat.y = y<<FX32_SHIFT;
	mat.z = 0;
	CLACT_SetMatrix( p_wk->p_clwk, &mat );

	if( p_wk->p_shadow ){
		mat.x += WF_2DC_SHADOW_MAT_OFS_X << FX32_SHIFT;
		mat.y += WF_2DC_SHADOW_MAT_OFS_Y << FX32_SHIFT;
		CLACT_SetMatrix( p_wk->p_shadow, &mat );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	座標取得
 *
 *	@param	p_wk	ワーク
 *	@param	x_y		フラグ
 *
 *	x_y
	WF_2DC_GET_X,
	WF_2DC_GET_Y
 *
 *
 *	@return	フラグの座標の値
 */
//-----------------------------------------------------------------------------
s16 WF_2DC_WkMatrixGet( WF_2DCWK* p_wk, WF_2DC_MAT x_y )
{
	const VecFx32* cp_mat;

	cp_mat = CLACT_GetMatrix( p_wk->p_clwk );

	if( x_y == WF_2DC_GET_X ){
		return cp_mat->x >> FX32_SHIFT;
	}
	return cp_mat->y >> FX32_SHIFT;
}

//----------------------------------------------------------------------------
/**
 *	@brief	表示優先順位を設定
 *
 *	@param	p_wk	ワーク
 *	@param	pri		表示優先順位
 */
//-----------------------------------------------------------------------------
void WF_2DC_WkDrawPriSet( WF_2DCWK* p_wk, u16 pri )
{
	CLACT_DrawPriorityChg( p_wk->p_clwk, pri );
}

//----------------------------------------------------------------------------
/**
 *	@brief	表示優先順位を取得
 *
 *	@param	cp_wk	ワーク
 *
 *	@return	表示優先順位
 */
//-----------------------------------------------------------------------------
u16 WF_2DC_WkDrawPriGet( const WF_2DCWK* cp_wk )
{
	return CLACT_DrawPriorityGet( cp_wk->p_clwk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメーションを進める
 *
 *	@param	p_wk	ワーク
 *	@param	frame	フレーム
 */
//-----------------------------------------------------------------------------
void WF_2DC_WkAnmAddFrame( WF_2DCWK* p_wk, fx32 frame )
{
	CLACT_AnmFrameChg( p_wk->p_clwk, frame );
}

//----------------------------------------------------------------------------
/**
 *	@brie	表示非表示設定
 *
 *	@param	p_wk	ワーク
 *	@param	flag	フラグ
 */
//-----------------------------------------------------------------------------
void WF_2DC_WkDrawFlagSet( WF_2DCWK* p_wk, BOOL flag )
{
	CLACT_SetDrawFlag( p_wk->p_clwk, flag );
	if( p_wk->p_shadow ){
		CLACT_SetDrawFlag( p_wk->p_shadow, flag );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	表示非表示取得
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	TRUE	表示
 *	@retval	FALSE	非表示
 */
//-----------------------------------------------------------------------------
BOOL WF_2DC_WkDrawFlagGet( const WF_2DCWK* cp_wk )
{
	return CLACT_GetDrawFlag( cp_wk->p_clwk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	動作タイプを取得する
 *
 *	@param	cp_wk	ワーク
 *
 *	@return	動作タイプ
 */
//-----------------------------------------------------------------------------
WF_2DC_MOVETYPE WF_2DC_WkMoveTypeGet( const WF_2DCWK* cp_wk )
{
	return cp_wk->cp_res->movetype;
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメーションタイプを取得する
 *
 *	@param	cp_wk	ワーク
 *
 *	@return	アニメーションタイプ
 */
//-----------------------------------------------------------------------------
WF_2DC_ANMTYPE WF_2DC_WkAnmTypeGet( const WF_2DCWK* cp_wk )
{
	return cp_wk->patanmtype;
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメーションフレーム設定
 *
 *	@param	p_wk		ワーク
 *	@param	frame		フレーム
 */
//-----------------------------------------------------------------------------
void WF_2DC_WkAnmFrameSet( WF_2DCWK* p_wk, u16 frame )
{
	CLACT_AnmFrameSet( p_wk->p_clwk, frame );
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメーションフレーム取得
 *
 *	@param	cp_wk		ワーク
 *
 *	@return	フレームインデックス
 */
//-----------------------------------------------------------------------------
u16 WF_2DC_WkAnmFrameGet( const WF_2DCWK* cp_wk )
{
	return CLACT_AnmFrameGet( cp_wk->p_clwk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	陰の位置を操作する
 *
 *	@param	p_wk	ワーク
 *	@param	x		ｘ座標
 *	@param	y		ｙ座標
 */
//-----------------------------------------------------------------------------
void WF_2DC_WkShadowMatrixSet( WF_2DCWK* p_wk, s16 x, s16 y )
{
	VecFx32	mat;
	if( p_wk->p_shadow != NULL ){

		mat.x = x+WF_2DC_SHADOW_MAT_OFS_X<<FX32_SHIFT;
		mat.y = y+WF_2DC_SHADOW_MAT_OFS_Y<<FX32_SHIFT;
		mat.z = 0;
		CLACT_SetMatrix( p_wk->p_shadow, &mat );
	}
}




//----------------------------------------------------------------------------
/**
 *	@brief	Patternアニメ開始
 *
 *	@param	p_wk		ワーク
 *	@param	anmtype		アニメタイプ
 *	@param	anmway		アニメ方向
 */
//-----------------------------------------------------------------------------
void WF_2DC_WkPatAnmStart( WF_2DCWK* p_wk, WF_2DC_ANMTYPE anmtype, WF_COMMON_WAY anmway )
{
	static void (* const pFunc[])( WF_2DCWK*, WF_COMMON_WAY ) = {
		WF_2DC_WkPatAnmWayInit,
		WF_2DC_WkPatAnmRotaInit,
		WF_2DC_WkPatAnmWalkInit,
		WF_2DC_WkPatAnmTurnInit,
		WF_2DC_WkPatAnmRunInit,
		WF_2DC_WkPatAnmWallWalkInit,
		WF_2DC_WkPatAnmSlowWalkInit,
		WF_2DC_WkPatAnmHighWalk2Init,
		WF_2DC_WkPatAnmHighWalk4Init,
		NULL
	};

	GF_ASSERT( anmtype < WF_2DC_ANMNUM );
	GF_ASSERT( anmway < WF_COMMON_WAYNUM );

	p_wk->patanmtype = anmtype;
	p_wk->patanmframe = 0;
	
	if( pFunc[ anmtype ] != NULL ){
		pFunc[ anmtype ]( p_wk, anmway );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	Patternアニメ終了チェック
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
BOOL WF_2DC_WkPatAnmEndCheck( const WF_2DCWK* cp_wk )
{
	if( WF_2DC_AnmFrame[ cp_wk->patanmtype ] == 0xff ){	// ループ設定
		return FALSE;
	}

	//終了チェック
	if( WF_2DC_AnmFrame[ cp_wk->patanmtype ] <= cp_wk->patanmframe ){
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	Patternアニメを進める
 *
 *	@param	p_wk		ワーク
 *	@param	speed		スピード
 */
//-----------------------------------------------------------------------------
void WF_2DC_WkPatAnmAddFrame( WF_2DCWK* p_wk )
{
	BOOL updata = FALSE;
	
	if( WF_2DC_AnmFrame[ p_wk->patanmtype ] == 0xff ){	// ループ設定
		updata = TRUE;	
	}else{	
	
		if( WF_2DC_AnmFrame[ p_wk->patanmtype ] >= p_wk->patanmframe + 1 ){
			updata = TRUE;
		}
	}

	if( updata == TRUE ){
		WF_2DC_WkPatAnmUpdate( p_wk );	// アニメ更新
		p_wk->patanmframe ++;
	}
}


//-----------------------------------------------------------------------------
/**
 *				プライベート関数
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	アニメーションリソース読み込み
 *
 *	@param	p_sys			システムワーク
 *	@param	movetype		動作タイプ
 *	@param	heap			ヒープ
 */
//-----------------------------------------------------------------------------
static void WF_2DC_AnmResLoad( WF_2DCSYS* p_sys, WF_2DC_MOVETYPE movetype, u32 heap )
{
	int i;
	u32 res_type;
	u32 anm_contid;
	static const u32 sc_WF_2DC_ANMRES_ANM_IDX[ WF_2DC_MOVENUM ][ WF_2DC_ANMRES_ANM_NUM ] = {
		{ // WF_2DC_MOVERUN
			NARC_wifi2dchar_run_NCER,
			NARC_wifi2dchar_run_NANR,
			NARC_wifi2dchar_run_NCER,
			NARC_wifi2dchar_run_NANR,
		},
		{ // WF_2DC_MOVENORMAL
			NARC_wifi2dchar_normal_NCER,
			NARC_wifi2dchar_normal_NANR,
			NARC_wifi2dchar_normal01_NCER,
			NARC_wifi2dchar_normal01_NANR,
		},
		{ // WF_2DC_MOVETURN
			NARC_wifi2dchar_npc_NCER,
			NARC_wifi2dchar_npc_NANR,
			NARC_wifi2dchar_npc01_NCER,
			NARC_wifi2dchar_npc01_NANR,
		},
	};


	GF_ASSERT( p_sys->anmres[ movetype ].resobj[0] == NULL );
	
	for( i=0; i<WF_2DC_ANMRES_ANM_NUM; i++ ){

		res_type	= CLACT_U_CELL_RES+(i%2);
		anm_contid	= WF_2DC_ARC_CONTANMID + (movetype * 2) + sc_WF_2DC_ANMRES_ANMCONTID[i];
		
		p_sys->anmres[movetype].resobj[i] = 
			CLACT_U_ResManagerResAddArcKindCell_ArcHandle(
				p_sys->p_res_man[res_type], p_sys->p_handle[WF_2DC_ARCHANDLE_NML],
				sc_WF_2DC_ANMRES_ANM_IDX[movetype][i], FALSE,
				anm_contid, res_type, heap );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメーションリソース破棄
 *
 *	@param	p_sys			システムワーク
 *	@param	movetype		動作タイプ
 */
//-----------------------------------------------------------------------------
static void WF_2DC_AnmResDel( WF_2DCSYS* p_sys, WF_2DC_MOVETYPE movetype )
{
	int i;
	u32 res_type;
	
	GF_ASSERT( p_sys->anmres[ movetype ].resobj[0] != NULL );
	
	for( i=0; i<WF_2DC_ANMRES_ANM_NUM; i++ ){
		res_type = CLACT_U_CELL_RES+(i%2);

		CLACT_U_ResManagerResDelete( p_sys->p_res_man[res_type],
					p_sys->anmres[movetype].resobj[i] );
		p_sys->anmres[ movetype ].resobj[i] = NULL;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメーションリソース　読み込み済みかチェック
 *
 *	@param	cp_sys		システムワーク
 *	@param	movetype	動作タイプ
 *
 *	@retval	TRUE	読み込んである
 *	@retval	FALSE	読み込んでない
 */
//-----------------------------------------------------------------------------
static BOOL WF_2DC_AnmResCheck( const WF_2DCSYS* cp_sys, WF_2DC_MOVETYPE movetype )
{
	if( cp_sys->anmres[ movetype ].resobj[0] == NULL ){
		return FALSE;
	}
	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	見た目からキャラクタNOを取得する
 *
 *	@param	view_type	見た目（FIELDOBJCODE）
 *
 *	@return	キャラクタNO
 */
//-----------------------------------------------------------------------------
static u32 WF_2DC_CharNoGet( u32 view_type )
{
	int i;

	for( i=0; i<WF_2DC_CHARNUM; i++ ){
		if( view_type == FIELDOBJCODE_CharIdx[i].code ){
			return FIELDOBJCODE_CharIdx[i].wf2dc_char;
		}
	}
	GF_ASSERT( 0 );
	return 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	指定された人物に動作タイプを指定できるかチェック
 *
 *	@param	view_type		見た目
 *	@param	movetype		動作タイプ
 *
 *	@retval	TRUE	指定できる
 *	@retval	FALSE	指定できない
 */
//-----------------------------------------------------------------------------
static BOOL WF_2DC_AnmModeLinkCheck( u32 view_type, WF_2DC_MOVETYPE movetype )
{
	if( movetype == WF_2DC_MOVERUN ){
		if( (view_type == HERO) || (view_type == HEROINE) ||
			(view_type == DPHERO) || (view_type == DPHEROINE) ){
			return TRUE;
		}else{
			return FALSE;
		}
	}
	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	リソースの管理IDを取得する
 *
 *	@param	movetype		動作タイプ
 *	@param	res_type		リソースタイプ
 *	@param	flip			キャラクタのフリップの有無
 *
 *	@return	リソース管理ID
 */
//-----------------------------------------------------------------------------
u32 WF_2DC_AnmResContIdGet( WF_2DC_MOVETYPE movetype, u32 res_type, BOOL flip )
{
	u32 idx;
	
	GF_ASSERT( (res_type == CLACT_U_CELL_RES) || (res_type == CLACT_U_CELLANM_RES) );
	
	idx = res_type-CLACT_U_CELL_RES;
	if( flip == FALSE ){
		idx += WF_2DC_ANMRES_ANM_NOFLIP_CELL;
	}
	
	return (WF_2DC_ARC_CONTANMID + (movetype * 2) + sc_WF_2DC_ANMRES_ANMCONTID[idx]);
}

//----------------------------------------------------------------------------
/**
 *	@brief	キャラクタリソース読み込み
 *
 *	@param	p_sys		システムワーク
 *	@param	char_no		キャラクタ番号
 *	@param	draw_type	転送先
 *	@param	movetype	動作タイプ
 *	@param	heap		ヒープ
 *	@param	restype		リソースタイプ
 */
//-----------------------------------------------------------------------------
static void WF_2DC_CharResLoad( WF_2DCSYS* p_sys, u32 char_no, u32 draw_type, WF_2DC_MOVETYPE movetype, u32 heap, WF_2DC_RESTYPE restype )
{
	// 読み込み済みでないことをチェック
	GF_ASSERT( p_sys->res[ char_no ].resobj[0] == NULL );

	if( restype == WF_2DC_RES_NML ){
		WF_2DC_CharResLoadNml( p_sys, char_no, draw_type, movetype, heap );
	}else{
		WF_2DC_CharResLoadUni( p_sys, char_no, draw_type, movetype, heap );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	キャラクターリソース読み込み通常時
 *
 *	@param	p_sys		システムワーク
 *	@param	char_no		キャラクタ番号
 *	@param	draw_type	転送先
 *	@param	movetype	動作タイプ
 *	@param	heap		ヒープ
 */
//-----------------------------------------------------------------------------
static void WF_2DC_CharResLoadNml( WF_2DCSYS* p_sys, u32 char_no, u32 draw_type, WF_2DC_MOVETYPE movetype, u32 heap )
{
	NNSG2dCharacterData* p_char;
	BOOL result;
	BOOL flip;	// フリップを使用するキャラクタか
	u32 cell_contid, anm_contid;

//	OS_Printf( "char_no %d\n", char_no );

	// 読み込み
	p_sys->res[ char_no ].resobj[ 0 ] = 
		CLACT_U_ResManagerResAddArcChar_ArcHandle(
				p_sys->p_res_man[ 0 ], p_sys->p_handle[WF_2DC_ARCHANDLE_NML],
				WF_2DC_ARC_GETNCG( char_no ),
				FALSE, WF_2DC_ARC_CONTCHARID+char_no, draw_type, heap );

	p_sys->res[ char_no ].resobj[ 1 ] = 
		CLACT_U_ResManagerResAddArcPltt_ArcHandle(
				p_sys->p_res_man[ 1 ], p_sys->p_handle[WF_2DC_ARCHANDLE_NML],
				WF_2DC_ARC_GETNCL( char_no ),
				FALSE, WF_2DC_ARC_CONTCHARID+char_no, draw_type, 1, heap );

//	OS_TPrintf( "pltt contid=0x%x  ncl=%d\n", WF_2DC_ARC_CONTCHARID+char_no, WF_2DC_ARC_GETNCL( char_no ) );

	//  キャラクタデータ取得
	p_char = CLACT_U_ResManagerGetResObjResChar( p_sys->res[ char_no ].resobj[ 0 ] );

	// フリップを使用するキャラクタかサイズを見て調べる
	if( p_char->szByte == WF_2DC_ARC_NORMALANMCHARSIZ_NOTFLIP ){
		// フリップなしのキャラクタサイズ
		flip = FALSE;
	}else{
		// その他の場合はフリップを使用している
		flip = TRUE;	
	}

	// 転送サイズ調整
	if( movetype == WF_2DC_MOVETURN ){
		// キャラクタサイズを削減できる
		if( flip ){	// フリップのあるなしでサイズが変わる
			p_char->szByte = WF_2DC_ARC_TURNANMCHARSIZ;
		}else{
			p_char->szByte = WF_2DC_ARC_TURNANMCHARSIZ_NOFLIP;
		}
	}else if( movetype == WF_2DC_MOVENORMAL ){
		// キャラクタサイズを削減できる
		if( flip ){	// フリップのあるなしでサイズが変わる
			p_char->szByte = WF_2DC_ARC_NORMALANMCHARSIZ;
		}else{
			p_char->szByte = WF_2DC_ARC_NORMALANMCHARSIZ_NOTFLIP;
		}
	}
	
	// 転送
	result = CLACT_U_CharManagerSetCharModeAdjustAreaCont( p_sys->res[ char_no ].resobj[0] );
	GF_ASSERT( result );
	result = CLACT_U_PlttManagerSetCleanArea( p_sys->res[ char_no ].resobj[1] );
	GF_ASSERT( result );

	// パレットフェード設定
	if( p_sys->p_pfd ){
		WF_2DC_PFDPalSet( p_sys, p_sys->res[ char_no ].resobj[1], 1 );
	}

	// リソースだけ破棄
	CLACT_U_ResManagerResOnlyDelete( p_sys->res[ char_no ].resobj[0] );
	CLACT_U_ResManagerResOnlyDelete( p_sys->res[ char_no ].resobj[1] );

	// 管理パラメータ設定
	p_sys->res[ char_no ].drawtype = draw_type;
	p_sys->res[ char_no ].movetype = movetype;
	p_sys->res[ char_no ].type = WF_2DC_RES_NML;

	// セルアクターヘッダー作成
	cell_contid	= WF_2DC_AnmResContIdGet( movetype, CLACT_U_CELL_RES, flip );
	anm_contid	= WF_2DC_AnmResContIdGet( movetype, CLACT_U_CELLANM_RES, flip );
	CLACT_U_MakeHeader( &p_sys->res[ char_no ].header, 
			WF_2DC_ARC_CONTCHARID+char_no, WF_2DC_ARC_CONTCHARID+char_no,
			cell_contid, anm_contid,
			CLACT_U_HEADER_DATA_NONE, CLACT_U_HEADER_DATA_NONE,0,0,
			p_sys->p_res_man[0], p_sys->p_res_man[1], 
			p_sys->p_res_man[2], p_sys->p_res_man[3],
			NULL, NULL );
}

//----------------------------------------------------------------------------
/**
 *	@brief	キャラクターリソース読み込み	ユニオン時
 *
 *	@param	p_sys		システムワーク
 *	@param	char_no		キャラクタ番号
 *	@param	draw_type	転送先
 *	@param	movetype	動作タイプ
 *	@param	heap		ヒープ
 */
//-----------------------------------------------------------------------------
static void WF_2DC_CharResLoadUni( WF_2DCSYS* p_sys, u32 char_no, u32 draw_type, WF_2DC_MOVETYPE movetype, u32 heap )
{
	NNSG2dCharacterData* p_char;
	BOOL result;
	u32 union_idx;

	// ユニオンキャラクタとしてのナンバーを取得
	union_idx = char_no - WF_2DC_BOY1;

//	OS_Printf( "char_no %d\n", char_no );

	// 読み込み
	p_sys->res[ char_no ].resobj[ 0 ] = 
		CLACT_U_ResManagerResAddArcChar_ArcHandle(
				p_sys->p_res_man[ 0 ], p_sys->p_handle[WF_2DC_ARCHANDLE_UNI],
				WF_2DC_ARC_GETUNINCG( char_no ),
				FALSE, WF_2DC_ARC_CONTCHARID+char_no, draw_type, heap );


	// 転送サイズ調整
	if( movetype == WF_2DC_MOVETURN ){
		// キャラクタサイズを削減できる
		p_char = CLACT_U_ResManagerGetResObjResChar( p_sys->res[ char_no ].resobj[ 0 ] );
		p_char->szByte = WF_2DC_ARC_TURNANMCHARSIZ;
	}else if( movetype == WF_2DC_MOVENORMAL ){
		// キャラクタサイズを削減できる
		p_char = CLACT_U_ResManagerGetResObjResChar( p_sys->res[ char_no ].resobj[ 0 ] );
		p_char->szByte = WF_2DC_ARC_NORMALANMCHARSIZ;
	}
	
	// 転送
	result = CLACT_U_CharManagerSetCharModeAdjustAreaCont( p_sys->res[ char_no ].resobj[0] );
	GF_ASSERT( result );

	// リソースだけ破棄
	CLACT_U_ResManagerResOnlyDelete( p_sys->res[ char_no ].resobj[0] );

	// パレットデータ部分にはNULLを入れておく
	p_sys->res[ char_no ].resobj[1] = NULL;

	// 管理パラメータ設定
	p_sys->res[ char_no ].drawtype = draw_type;
	p_sys->res[ char_no ].movetype = movetype;
	p_sys->res[ char_no ].type = WF_2DC_RES_UNI;

	// セルアクターヘッダー作成
	CLACT_U_MakeHeader( &p_sys->res[ char_no ].header, 
			WF_2DC_ARC_CONTCHARID+char_no, WF_2DC_ARC_CONTUNIONPLTTID,
			WF_2DC_ARC_CONTUNIONANMID+union_idx, 
			WF_2DC_ARC_CONTUNIONANMID+union_idx,
			CLACT_U_HEADER_DATA_NONE, CLACT_U_HEADER_DATA_NONE,0,0,
			p_sys->p_res_man[0], p_sys->p_res_man[1], 
			p_sys->p_res_man[2], p_sys->p_res_man[3],
			NULL, NULL );
}

//----------------------------------------------------------------------------
/**
 *	@brief	キャラクタリソース破棄
 *
 *	@param	p_sys		システムワーク
 *	@param	char_no		キャラクタ番号
 */
//-----------------------------------------------------------------------------
static void WF_2DC_CharResDel( WF_2DCSYS* p_sys, u32 char_no )
{
	// 読み込み済みであることをチェック
	GF_ASSERT( p_sys->res[ char_no ].resobj[0] != NULL );

	if( p_sys->res[ char_no ].type == WF_2DC_RES_NML ){
		WF_2DC_CharResDelNml( p_sys, char_no );
	}else{
		WF_2DC_CharResDelUni( p_sys, char_no );
	}

}

//----------------------------------------------------------------------------
/**
 *	@brief	キャラクタリソース破棄	通常時
 *
 *	@param	p_sys		システムワーク
 *	@param	char_no		キャラクタ番号
 */
//-----------------------------------------------------------------------------
static void WF_2DC_CharResDelNml( WF_2DCSYS* p_sys, u32 char_no )
{
	// VRAM管理から破棄
	CLACT_U_CharManagerDelete( p_sys->res[ char_no ].resobj[0] );
	CLACT_U_PlttManagerDelete( p_sys->res[ char_no ].resobj[1] );
	
	// リソース破棄
	CLACT_U_ResManagerResDelete( p_sys->p_res_man[0], p_sys->res[ char_no ].resobj[0] );
	CLACT_U_ResManagerResDelete( p_sys->p_res_man[1], p_sys->res[ char_no ].resobj[1] );

	p_sys->res[ char_no ].resobj[0] = NULL;
	p_sys->res[ char_no ].resobj[1] = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	キャラクタリソース破棄	ユニオン時
 *
 *	@param	p_sys		システムワーク
 *	@param	char_no		キャラクタ番号
 */
//-----------------------------------------------------------------------------
static void WF_2DC_CharResDelUni( WF_2DCSYS* p_sys, u32 char_no )
{
	// VRAM管理から破棄
	CLACT_U_CharManagerDelete( p_sys->res[ char_no ].resobj[0] );
	
	// リソース破棄
	CLACT_U_ResManagerResDelete( p_sys->p_res_man[0], p_sys->res[ char_no ].resobj[0] );

	p_sys->res[ char_no ].resobj[0] = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	キャラクタリソースが読み込まれているかチェック
 *
 *	@param	cp_sys		システムワーク
 *	@param	char_no		キャラクタ番号
 *
 *	@retval	TRUE	読み込み完了
 *	@retval	FALSE	読み込んでいない
 */
//-----------------------------------------------------------------------------
static BOOL WF_2DC_CharResCheck( const WF_2DCSYS* cp_sys, u32 char_no )
{
	if( cp_sys->res[ char_no ].resobj[0] == NULL ){
		return FALSE;
	}
	return TRUE;	
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画タイプの取得
 *
 *	@param	cp_res	リソースワーク
 *
 *	@return	描画タイプ取得
 */
//-----------------------------------------------------------------------------
static u32 WF_2DC_CharResDrawTypeGet( const WF_2DCRES* cp_res )
{
	return cp_res->drawtype;
}

//----------------------------------------------------------------------------
/**
 *	@brief	動作タイプ取得
 *
 *	@param	cp_res	リソースワーク
 *	
 *	@return	動作タイプ
 */
//-----------------------------------------------------------------------------
static WF_2DC_MOVETYPE WF_2DC_CharResMoveTypeGet( const WF_2DCRES* cp_res )
{
	return cp_res->movetype;
}

//----------------------------------------------------------------------------
/**
 *	@brief	陰リソースを読み込む
 *
 *	@param	p_sys		システムワーク
 *	@param	draw_type	表示画面
 *	@param	heap		ヒープID
 */
//-----------------------------------------------------------------------------
static void WF_2DC_ShResLoad( WF_2DCSYS* p_sys, u32 draw_type, u32 heap )
{
	int i;
	BOOL result;
	BOOL hero_load;
	u32 hero_no;
	GF_ASSERT( p_sys->shadowres.resobj[0] == NULL );

	// 主人公が読み込まれているかチェックする

	hero_load = FALSE;
	if( WF_2DC_CharResCheck( p_sys, WF_2DC_HERO ) == TRUE ){
		hero_no		= WF_2DC_HERO;
		hero_load	= TRUE;
	}
	else if( WF_2DC_CharResCheck( p_sys, WF_2DC_HEROINE ) == TRUE ){
		hero_no		= WF_2DC_HEROINE;
		hero_load	= TRUE;
	}
	else if( WF_2DC_CharResCheck( p_sys, WF_2DC_DPHERO ) == TRUE ){
		hero_no		= WF_2DC_DPHERO;
		hero_load	= TRUE;
	}
	else if( WF_2DC_CharResCheck( p_sys, WF_2DC_DPHEROINE ) == TRUE ){
		hero_no		= WF_2DC_DPHEROINE;
		hero_load	= TRUE;
	}

	GF_ASSERT_MSG( hero_load == TRUE, "shodow load miss  hero or heroine not load" );

	// アニメリソース
	for( i=0; i<2; i++ ){
		p_sys->shadowres.resobj[i+2] = 
			CLACT_U_ResManagerResAddArcKindCell_ArcHandle(
				p_sys->p_res_man[i+2], p_sys->p_handle[WF_2DC_ARCHANDLE_NML],
				NARC_wifi2dchar_hero_ine_kage_NCER+i, FALSE,
				WF_2DC_ARC_CONTSHADOWID, CLACT_U_CELL_RES+i, heap );
	}

	// キャラクタリソース
	// 読み込み
	p_sys->shadowres.resobj[0] = 
		CLACT_U_ResManagerResAddArcChar_ArcHandle(
				p_sys->p_res_man[0], p_sys->p_handle[WF_2DC_ARCHANDLE_NML],
				NARC_wifi2dchar_hero_ine_kage_NCGR,
				FALSE, WF_2DC_ARC_CONTSHADOWID, draw_type, heap );

	
	// 転送
	result = CLACT_U_CharManagerSetCharModeAdjustAreaCont( p_sys->shadowres.resobj[0] );
	GF_ASSERT( result );

	// リソースだけ破棄
	CLACT_U_ResManagerResOnlyDelete( p_sys->shadowres.resobj[0] );


	// セルアクターヘッダー作成
	CLACT_U_MakeHeader( &p_sys->shadowres.header, 
			WF_2DC_ARC_CONTSHADOWID, WF_2DC_ARC_CONTCHARID+hero_no,
			WF_2DC_ARC_CONTSHADOWID, WF_2DC_ARC_CONTSHADOWID,
			CLACT_U_HEADER_DATA_NONE, CLACT_U_HEADER_DATA_NONE,0,0,
			p_sys->p_res_man[0], p_sys->p_res_man[1], 
			p_sys->p_res_man[2], p_sys->p_res_man[3],
			NULL, NULL );
}

//----------------------------------------------------------------------------
/**
 *	@brief	陰リソースの破棄
 *	
 *	@param	p_sys	システムワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_ShResDel( WF_2DCSYS* p_sys )
{
	int i;
	
	if( WF_2DC_ShResCheck( &p_sys->shadowres ) == TRUE ){
		
		// キャラクタリソース破棄
		{
			// VRAM管理から破棄
			CLACT_U_CharManagerDelete( p_sys->shadowres.resobj[0] );
			
			// リソース破棄
			CLACT_U_ResManagerResDelete( p_sys->p_res_man[0], p_sys->shadowres.resobj[0] );
			p_sys->shadowres.resobj[0] = NULL;
		}

		// アニメリソース破棄
		for( i=0; i<2; i++ ){
			CLACT_U_ResManagerResDelete( p_sys->p_res_man[i+2],
						p_sys->shadowres.resobj[i+2] );
			p_sys->shadowres.resobj[i+2] = NULL;
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	陰リソースが読み込まれているかチェック
 *
 *	@param	cp_shadowres	ワーク
 *
 *	@retval	TRUE	リソース読み込み済み
 *	@retval	FALSE	リソースが読み込まれていない
 */
//-----------------------------------------------------------------------------
static BOOL WF_2DC_ShResCheck( const WF_2DCSH_RES* cp_shadowres )
{

	if( cp_shadowres->resobj[0] != NULL ){
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ユニオンキャラクタ用パレット読込み
 *
 *	@param	p_sys		システムワーク
 *	@param	draw_type	描画面
 *	@param	heap
 */
//-----------------------------------------------------------------------------
static void WF_2DC_UniCharPlttResLoad( WF_2DCSYS* p_sys, u32 draw_type, u32 heap )
{
	BOOL result;

	GF_ASSERT( p_sys->p_unionpltt == NULL );
	
	// ユニオンパレット読み込み
	p_sys->p_unionpltt = 
		CLACT_U_ResManagerResAddArcPltt_ArcHandle(
				p_sys->p_res_man[ 1 ], p_sys->p_handle[WF_2DC_ARCHANDLE_UNI],
				WF_2DC_ARC_GETUNINCL,
				FALSE, WF_2DC_ARC_CONTUNIONPLTTID, draw_type, WF_2DC_UNIPLTT_NUM,
				heap );

	// 転送
	result = CLACT_U_PlttManagerSetCleanArea( p_sys->p_unionpltt );
	GF_ASSERT( result );

	// パレットフェード設定
	if( p_sys->p_pfd ){
		WF_2DC_PFDPalSet( p_sys, p_sys->p_unionpltt, WF_2DC_UNIPLTT_NUM );
	}

	// リソースだけ破棄
	CLACT_U_ResManagerResOnlyDelete( p_sys->p_unionpltt );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ユニオンキャラクタ用パレット破棄
 *
 *	@param	p_sys	システムワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_UniCharPlttResDel( WF_2DCSYS* p_sys )
{
	GF_ASSERT( p_sys->p_unionpltt != NULL );
	
	// ユニオンパレット破棄
	CLACT_U_PlttManagerDelete( p_sys->p_unionpltt );
	CLACT_U_ResManagerResDelete( p_sys->p_res_man[1], p_sys->p_unionpltt );
	p_sys->p_unionpltt = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ユニオンキャラクタ用アニメリソース読込み
 *
 *	@param	p_sys	システムワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_UniCharAnmResLoad( WF_2DCSYS* p_sys, u32 heap )
{
	int i;

	GF_ASSERT( p_sys->unionres[ 0 ].resobj[0] == NULL );

	for( i=0; i<WF_2DC_UNICHAR_NUM; i++ ){

		p_sys->unionres[i].resobj[ 0 ] = 
			CLACT_U_ResManagerResAddArcKindCell_ArcHandle(
				p_sys->p_res_man[2], p_sys->p_handle[WF_2DC_ARCHANDLE_UNI],
				WF_2DC_ARC_GETUNICEL(i), FALSE,
				WF_2DC_ARC_CONTUNIONANMID+i, CLACT_U_CELL_RES, heap );

		p_sys->unionres[i].resobj[ 1 ] = 
			CLACT_U_ResManagerResAddArcKindCell_ArcHandle(
				p_sys->p_res_man[3], p_sys->p_handle[WF_2DC_ARCHANDLE_UNI],
				WF_2DC_ARC_GETUNIANM(i), FALSE,
				WF_2DC_ARC_CONTUNIONANMID+i, CLACT_U_CELLANM_RES, heap );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ユニオンキャラクタ用アニメリソース破棄
 *
 *	@param	p_sys	システムワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_UniCharAnmResDel( WF_2DCSYS* p_sys )
{
	int i;

	GF_ASSERT( p_sys->unionres[ 0 ].resobj[0] != NULL );

	for( i=0; i<WF_2DC_UNICHAR_NUM; i++ ){

		CLACT_U_ResManagerResDelete( p_sys->p_res_man[2],
					p_sys->unionres[i].resobj[ 0 ] );

		CLACT_U_ResManagerResDelete( p_sys->p_res_man[3],
					p_sys->unionres[i].resobj[ 1 ] );

		p_sys->unionres[ i ].resobj[0] = NULL;
		p_sys->unionres[ i ].resobj[1] = NULL;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	空のワークを取得
 *
 *	@param	p_sys	ワーク
 *
 *	@return	空のワーク
 */
//-----------------------------------------------------------------------------
static WF_2DCWK* WF_2DC_WkCleanGet( WF_2DCSYS* p_sys )
{
	int i;

	for( i=0; i<p_sys->objnum; i++ ){
		if( WF_2DC_WkCleanCheck( &p_sys->p_wk[i] ) == TRUE ){
			return &p_sys->p_wk[i];
		}
	}
	GF_ASSERT( 0 );
	return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	Patternアニメ更新
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmUpdate( WF_2DCWK* p_wk )
{
	static void (* const pFunc[])( WF_2DCWK*  ) = {
		NULL,
		WF_2DC_WkPatAnmRotaMain,
		WF_2DC_WkPatAnmWalkMain,
		WF_2DC_WkPatAnmTurnMain,
		WF_2DC_WkPatAnmRunMain,
		WF_2DC_WkPatAnmWallWalkMain,
		WF_2DC_WkPatAnmSlowWalkMain,
		WF_2DC_WkPatAnmHighWalk2Main,
		WF_2DC_WkPatAnmHighWalk4Main,
		NULL
	};
	if( pFunc[p_wk->patanmtype] != NULL ){
		pFunc[p_wk->patanmtype]( p_wk );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	方向変えアニメ初期化
 *
 *	@param	p_wk	ワーク
 *	@param	way		方向
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmWayInit( WF_2DCWK* p_wk, WF_COMMON_WAY way )
{
	p_wk->patanmway = way;
	CLACT_AnmChgCheck( p_wk->p_clwk, p_wk->patanmway );
}

//----------------------------------------------------------------------------
/**
 *	@brief	回転アニメ初期化
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmRotaInit( WF_2DCWK* p_wk, WF_COMMON_WAY way )
{
	p_wk->patanmway = way;
	CLACT_AnmChgCheck( p_wk->p_clwk, WF_2DC_ANM_ROTA );
}

//----------------------------------------------------------------------------
/**
 *	@brief	歩きアニメ初期化
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmWalkInit( WF_2DCWK* p_wk, WF_COMMON_WAY way )
{
	p_wk->patanmway = way;
	CLACT_AnmChgCheck( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );
}

//----------------------------------------------------------------------------
/**
 *	@brief	振り向きアニメ初期化
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmTurnInit( WF_2DCWK* p_wk, WF_COMMON_WAY way )
{
	CLACT_AnmChgCheck( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );

	// 後で設定することで２フレーム目で方向が変わる
	p_wk->patanmway = way;
}

//----------------------------------------------------------------------------
/**
 *	@brief	走りアニメ初期化
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmRunInit( WF_2DCWK* p_wk, WF_COMMON_WAY way )
{
	p_wk->patanmway = way;
	CLACT_AnmChgCheck( p_wk->p_clwk, WF_2DC_ANM_RUN+p_wk->patanmway );
}

//----------------------------------------------------------------------------
/**
 *	@brief	壁方向歩きアニメ初期化
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmWallWalkInit( WF_2DCWK* p_wk, WF_COMMON_WAY way )
{
	p_wk->patanmway = way;
	CLACT_AnmChgCheck( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ゆっくり歩きアニメ初期化
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmSlowWalkInit( WF_2DCWK* p_wk, WF_COMMON_WAY way )
{
	p_wk->patanmway = way;
	CLACT_AnmChgCheck( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );
}

//----------------------------------------------------------------------------
/**
 *	@brief	高速アニメ２フレーム
 *
 *	@param	p_wk		ワーク
 *	@param	way			方向
 */		
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmHighWalk2Init( WF_2DCWK* p_wk, WF_COMMON_WAY way )
{
	p_wk->patanmway = way;
	CLACT_AnmChgCheck( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );
}

//----------------------------------------------------------------------------
/**
 *	@brief	高速アニメ４フレーム
 *
 *	@param	p_wk		ワーク
 *	@param	way			方向
 */		
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmHighWalk4Init( WF_2DCWK* p_wk, WF_COMMON_WAY way )
{
	p_wk->patanmway = way;
	CLACT_AnmChgCheck( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );
}

//----------------------------------------------------------------------------
/**
 *	@brief	回転アニメ
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmRotaMain( WF_2DCWK* p_wk )
{
	CLACT_AnmFrameChg( p_wk->p_clwk, WF_2DC_ANM_FRAME );
}

//----------------------------------------------------------------------------
/**
 *	@brief	歩きアニメメイン
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmWalkMain( WF_2DCWK* p_wk )
{
	CLACT_AnmFrameChg( p_wk->p_clwk, WF_2DC_ANM_FRAME );
}

//----------------------------------------------------------------------------
/**
 *	@brief	振り向きアニメ
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmTurnMain( WF_2DCWK* p_wk )
{
	if( p_wk->patanmframe == 0 ){
		CLACT_AnmFrameSet( p_wk->p_clwk, 1 );
	}else{
		CLACT_AnmChg( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	走りアニメメイン
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmRunMain( WF_2DCWK* p_wk )
{
	CLACT_AnmFrameChg( p_wk->p_clwk, WF_2DC_ANM_FRAME );
}

//----------------------------------------------------------------------------
/**
 *	@brief	壁方向歩きアニメメイン
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmWallWalkMain( WF_2DCWK* p_wk )
{
	CLACT_AnmFrameChg( p_wk->p_clwk, WF_2DC_ANM_SLOWFRAME );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ゆっくり歩きアニメメイン
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmSlowWalkMain( WF_2DCWK* p_wk )
{
	CLACT_AnmFrameChg( p_wk->p_clwk, WF_2DC_ANM_SLOWFRAME );
}

//----------------------------------------------------------------------------
/**
 *	@brief	高速２フレアニメメイン
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmHighWalk2Main( WF_2DCWK* p_wk )
{
	CLACT_AnmFrameChg( p_wk->p_clwk, WF_2DC_ANM_HIGH2 );
}

//----------------------------------------------------------------------------
/**
 *	@brief	高速４フレアニメメイン
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmHighWalk4Main( WF_2DCWK* p_wk )
{
	CLACT_AnmFrameChg( p_wk->p_clwk, WF_2DC_ANM_HIGH4 );
}


//----------------------------------------------------------------------------
/**
 *	@brief	ワークが使われているかチェック
 *
 *	@param cp_wk	ワーク
 *
 *	@retval		TRUE　		未使用
 *	@retval		FALSE		使用中
 */
//-----------------------------------------------------------------------------
static BOOL WF_2DC_WkCleanCheck( const WF_2DCWK* cp_wk )
{
	if( cp_wk->p_clwk == NULL ){
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	パレットデータを設定
 *
 *	@param	p_sys		システムワーク
 *	@param	p_pal		パレットリソースデータ
 *	@param	num			数
 */
//-----------------------------------------------------------------------------
static void WF_2DC_PFDPalSet( WF_2DCSYS* p_sys, CLACT_U_RES_OBJ_PTR p_pal, u32 num )
{
	NNSG2dPaletteData* p_paldata;
	u32 pal_no[2];

	p_paldata = CLACT_U_ResManagerGetResObjResPltt( p_pal );
	pal_no[0] = CLACT_U_PlttManagerGetPlttNo( p_pal, NNS_G2D_VRAM_TYPE_2DMAIN );
	pal_no[1] = CLACT_U_PlttManagerGetPlttNo( p_pal, NNS_G2D_VRAM_TYPE_2DSUB );
	
	if( pal_no[0] != CLACT_U_PLTT_NO_NONE ){
		PaletteWorkSet( p_sys->p_pfd, p_paldata->pRawData, FADE_MAIN_OBJ, pal_no[0]*16, num*32 );
	}
	if( pal_no[1] != CLACT_U_PLTT_NO_NONE ){
		PaletteWorkSet( p_sys->p_pfd, p_paldata->pRawData, FADE_SUB_OBJ, pal_no[1]*16, num*32 );
	}
}

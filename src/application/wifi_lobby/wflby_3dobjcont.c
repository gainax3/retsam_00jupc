//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_3dobjcont.c
 *	@brief		登場する人物をすべて管理するシステム	
 *				人物に対するリクエストもすべてここを当して行う
 *	@author		tomoya takahashi
 *	@data		2007.11.13
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"

#include "application/wifi_2dmap/wf2dmap_obj.h"
#include "application/wifi_2dmap/wf2dmap_cmdq.h"
#include "application/wifi_2dmap/wf2dmap_judge.h"

#include "wflby_def.h"
#include "wflby_3dobjcont.h"
#include "wflby_3dobj.h"
#include "wflby_3dmatrix.h"

#include "wflby_snd.h"

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
#define WFLBY_3DOBJCONT_QBUF_NUM	( 32 )	// コマンドキューバッファ数



//-------------------------------------
///	NPC動作定数
//=====================================
enum{
	WFLBY_3DOBJCONT_NPC_SEQ_WAITSET,
	WFLBY_3DOBJCONT_NPC_SEQ_WAIT,
	WFLBY_3DOBJCONT_NPC_SEQ_WAYSET,
	WFLBY_3DOBJCONT_NPC_SEQ_WAYMOVEWALKTURN,
	WFLBY_3DOBJCONT_NPC_SEQ_WAYMOVEWAIT,
};
static const u8 sc_WFLBY_3DOBJCONT_NPC_WAIT[] = {
	32, 64, 128, 144
};
static const u8 sc_WFLBY_3DOBJCONT_NPC_WAY[] = {
	WF2DMAP_WAY_UP, WF2DMAP_WAY_DOWN, WF2DMAP_WAY_LEFT, WF2DMAP_WAY_RIGHT
};


//-------------------------------------
///	のぼり　くだり　動作
//=====================================
// くだり
enum{
	WFLBY_3DOBJCONT_FLYDOWN_SEQ_INIT,
	WFLBY_3DOBJCONT_FLYDOWN_SEQ_MAIN,
	WFLBY_3DOBJCONT_FLYDOWN_SEQ_ONEROTA,	// さらに１回転
	WFLBY_3DOBJCONT_FLYDOWN_SEQ_END,
};
// 上り
enum{
	WFLBY_3DOBJCONT_FLYUP_SEQ_INIT,
	WFLBY_3DOBJCONT_FLYUP_SEQ_ONEROTA,	// １回転
	WFLBY_3DOBJCONT_FLYUP_SEQ_MAIN,
	WFLBY_3DOBJCONT_FLYUP_SEQ_END,
};
#define WFLBY_3DOBJCONT_FLY_Y			( FX32_CONST(500) )	// 飛んでいく距離
#define WFLBY_3DOBJCONT_FLY_COUNT		( 24 )		// 動作シンク数
#define WFLBY_3DOBJCONT_ROTA_ONE_COUNT	( 32 )

//-------------------------------------
///	回転吹き飛び動作
//=====================================
enum {
	WFLBY_3DOBJCONT_ROTA_SEQ_INIT,
	WFLBY_3DOBJCONT_ROTA_SEQ_MAIN,
	WFLBY_3DOBJCONT_ROTA_SEQ_ONEROTA,	// さらに１回転
	WFLBY_3DOBJCONT_ROTA_SEQ_END,
} ;
#define WFLBY_3DOBJCONT_ROTA_DIST	( WFLBY_3DMATRIX_GRID_GRIDSIZ )	// 移動する値
#define WFLBY_3DOBJCONT_ROTA_COUNT	( 4 )							// 移動するシンク数
#define WFLBY_3DOBJCONT_ROTA_ONEROTA_COUNT	( 4 )
#define WFLBY_3DOBJCONT_ROTA_ONESIDEROTA	( 2 )			// 回転１セルに必要なシンク数
#define WFLBY_3DOBJCONT_ROTA_ANMSPEED		( 2 )			// 回転アニメスピード


//-------------------------------------
///	ジャンプ動作
//=====================================
enum {
	WFLBY_3DOBJCONT_JUMP_SEQ_INIT,
	WFLBY_3DOBJCONT_JUMP_SEQ_MAIN,
} ;

#define WFLBY_3DOBJCONT_MOVEOBJ_JUMP_EFFCOUNT	( 6 )// ジャンプしている時間
#define WFLBY_3DOBJCONT_MOVEOBJ_JUMP_WAITCOUNT	( 16 )// 次のジャンプまでのウエイト
#define WFLBY_3DOBJCONT_MOVEOBJ_JUMP_COUNT		( (WFLBY_3DOBJCONT_MOVEOBJ_JUMP_EFFCOUNT*2)+WFLBY_3DOBJCONT_MOVEOBJ_JUMP_WAITCOUNT )// １ジャンプカウント
#define WFLBY_3DOBJCONT_MOVEOBJ_JUMP_DIS	( 10 )// ジャンプの高さ
#define WFLBY_3DOBJCONT_MOVEOBJ_JUMP_ZDIS	( -2 )// ジャンプの高さ
#define WFLBY_3DOBJCONT_MOVEOBJ_JUMP_RMAX	(180)// 回転弧MAX



//-------------------------------------
///	影の色を変更するシステム
//=====================================
#define WFLBY_3DOBJCONT_SHADOW_ALHA_FADE_COUNT	( 60 )


//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	線形動作ワーク
//=====================================
typedef struct {
	fx32 x;
	fx32 s_x;
	fx32 dis_x;
	u32  count_max;
} WFLBY_3DOBJCONT_MOVE_WK;


//-------------------------------------
///	影のアルファ値変動システム
//=====================================
typedef struct {
	u8	move;		// 動作中か
	u8	now;		// 今の値
	s8  start;		// 開始のアルファ値
	s8	diff;		// 変化量
	s16 count;		// カウント値
	s16 count_max;	// 最大カウント値
} WFLBY_SHADOW_ALPHA;


//-------------------------------------
///	動作ワーク構造体
//=====================================
typedef union{
	// MOVENPCワーク
	struct{
		s16 wait;
		s16 way;
	}npc;

	// MOVEFLY系ワーク
	struct{
		WFLBY_3DOBJCONT_MOVE_WK	y;
		u32						count;
	}fly;

	// rota系ワーク
	struct{
		WFLBY_3DOBJCONT_MOVE_WK	dist;
		u32						count;
		fx32					def;
	}rota;

	// jump系ワーク
	struct{
		s32						count;
		VecFx32					def_mat;
	}jump;
} WFLBY_3DPARSON_MOVEWK;

//-------------------------------------
///	ワーク構造体
//=====================================
typedef struct _WFLBY_3DPERSON{
	WF2DMAP_OBJWK* p_obj;
	WFLBY_3DOBJWK* p_draw;

	// 動作関数ワーク
	u16						seq;
	u8						moveend;
	u8						movetype;
	WFLBY_3DPARSON_MOVEWK	work;

	// 基本動作関数
	void (*p_func)( struct _WFLBY_3DPERSON* p_wk, WFLBY_3DOBJCONT* p_sys );
}WFLBY_3DPERSON;
typedef void (*pWFLBY_3DPARSON_FUNC)( struct _WFLBY_3DPERSON* p_wk, WFLBY_3DOBJCONT* p_sys );

//-------------------------------------
///	システム構造体
//=====================================
typedef struct _WFLBY_3DOBJCONT{
	WF2DMAP_OBJSYS*		p_objsys;
	WFLBY_3DOBJSYS*		p_drawsys;
	WFLBY_3DPERSON*		p_objbuf;
	u16					objnum;
	u16					hero_sex;
	WF2DMAP_REQCMDQ*	p_reqQ;
	const WFLBY_MAPCONT* cp_mapsys;
	WFLBY_SHADOW_ALPHA	shadow_alpha;

	WFLBY_3DPERSON*		p_player;
	WFLBY_3DPERSON*		p_pierrot;
}WFLBY_3DOBJCONT;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	マップ配置人物初期化
//=====================================
static void WFLBY_3DOBJCONT_MAPOBJ_Init( WFLBY_3DOBJCONT* p_sys );


//-------------------------------------
///	コマンド設定
//=====================================
static void WFLBY_3DOBJCONT_REQCMD_Set( WFLBY_3DOBJCONT* p_sys, s32 cmd, s32 way, s32 playid );


//-------------------------------------
///	ユーティリティ関数
//=====================================
// 乱数でテーブルの中の１つの要素の値を返す
static u8 WFLBY_3DOBJCONT_GetRndTbl( const u8* cp_tbl, u32 tblnum );	
// 人物のway方向のグリッドのOBJIDをチェックする
static BOOL WFLBY_3DOBJCONT_CheckWayObjID( const WFLBY_3DOBJCONT* cp_sys, const WFLBY_3DPERSON* cp_obj, WF2DMAP_WAY way, u32 objid );
// 線形動作
static void WFLBY_3DOBJCONT_MOVEWK_Init( WFLBY_3DOBJCONT_MOVE_WK* p_wk, fx32 s_x, fx32 e_x, u32 count_max );
static BOOL WFLBY_3DOBJCONT_MOVEWK_Main( WFLBY_3DOBJCONT_MOVE_WK* p_wk, u32 count );
static fx32 WFLBY_3DOBJCONT_MOVEWK_GetNum( const WFLBY_3DOBJCONT_MOVE_WK* cp_wk );



//-------------------------------------
///	人物ワーク操作
//=====================================
static WFLBY_3DPERSON* WFLBY_3DOBJCONT_GetCleanWk( WFLBY_3DOBJCONT* p_sys );
static BOOL WFLBY_3DOBJCONT_CheckCleanWk( const WFLBY_3DPERSON* cp_wk );
// 人物動作関数
static void WFLBY_3DOBJCONT_MoveNone(  WFLBY_3DPERSON* p_wk, WFLBY_3DOBJCONT* p_sys );
static void WFLBY_3DOBJCONT_PlayerMove(  WFLBY_3DPERSON* p_wk, WFLBY_3DOBJCONT* p_sys );
static void WFLBY_3DOBJCONT_NpcMove( WFLBY_3DPERSON* p_wk, WFLBY_3DOBJCONT* p_sys );
static void WFLBY_3DOBJCONT_FlyUpMove( WFLBY_3DPERSON* p_wk, WFLBY_3DOBJCONT* p_sys );
static void WFLBY_3DOBJCONT_FlyDownMove( WFLBY_3DPERSON* p_wk, WFLBY_3DOBJCONT* p_sys );
static void WFLBY_3DOBJCONT_RotaLeftMove( WFLBY_3DPERSON* p_wk, WFLBY_3DOBJCONT* p_sys );
static void WFLBY_3DOBJCONT_RotaRightMove( WFLBY_3DPERSON* p_wk, WFLBY_3DOBJCONT* p_sys );
static void WFLBY_3DOBJCONT_RotaUpMove( WFLBY_3DPERSON* p_wk, WFLBY_3DOBJCONT* p_sys );
static void WFLBY_3DOBJCONT_RotaDownMove( WFLBY_3DPERSON* p_wk, WFLBY_3DOBJCONT* p_sys );
static void WFLBY_3DOBJCONT_RotaMove( WFLBY_3DPERSON* p_wk, WFLBY_3DOBJCONT* p_sys, WF2DMAP_WAY way );
static void WFLBY_3DOBJCONT_Jump( WFLBY_3DPERSON* p_wk, WFLBY_3DOBJCONT* p_sys );
static void WFLBY_3DOBJCONT_JumpEnd( WFLBY_3DPERSON* p_wk, WFLBY_3DOBJCONT* p_sys );


//-------------------------------------
///	影のアルファ値変更システム
//=====================================
static void WFLBY_3DOBJCONT_ShadowAlpha_Init( WFLBY_SHADOW_ALPHA* p_wk, WFLBY_3DOBJSYS* p_sys, u32 alpha );
static void WFLBY_3DOBJCONT_ShadowAlpha_Start( WFLBY_SHADOW_ALPHA* p_wk, u32 alpha );
static BOOL WFLBY_3DOBJCONT_ShadowAlpha_Main( WFLBY_SHADOW_ALPHA* p_wk, WFLBY_3DOBJSYS* p_sys );




//-----------------------------------------------------------------------------
/**
 *			マップ配置人物データ
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	
//=====================================
typedef struct {
	u8	mapobjid;
	u8	way;
	u8 trtype;
	u8	playid;
} WFLBY_MAPOBJDATA;
static const WFLBY_MAPOBJDATA sc_WFLBY_3DPARSON_MAPOBJ[] = {
	{ WFLBY_MAPOBJID_NPC_SW_IN,		WF2DMAP_WAY_LEFT,	WIFISW, WFLBY_MAPPERSON_ID },
	{ WFLBY_MAPOBJID_NPC_SW_TOPIC,	WF2DMAP_WAY_DOWN,	WIFISW, WFLBY_MAPPERSON_ID },
	{ WFLBY_MAPOBJID_NPC_SW_TOPIC2,	WF2DMAP_WAY_DOWN,	WIFISM, WFLBY_MAPPERSON_ID },
	{ WFLBY_MAPOBJID_ANKETO_MAN,	WF2DMAP_WAY_DOWN,	WIFISM, WFLBY_MAPPERSON_PIERROT_ID },
};


//-----------------------------------------------------------------------------
/**
 *				動作テーブル
 */
//-----------------------------------------------------------------------------
static const pWFLBY_3DPARSON_FUNC sc_WFLBY_3DPARSON_FUNC[WFLBY_3DOBJCONT_MOVENUM] = {
	WFLBY_3DOBJCONT_MoveNone,
	WFLBY_3DOBJCONT_PlayerMove,
	WFLBY_3DOBJCONT_NpcMove,
	WFLBY_3DOBJCONT_FlyUpMove,
	WFLBY_3DOBJCONT_FlyDownMove,
	WFLBY_3DOBJCONT_RotaUpMove,
	WFLBY_3DOBJCONT_RotaDownMove,
	WFLBY_3DOBJCONT_RotaLeftMove,
	WFLBY_3DOBJCONT_RotaRightMove,
	WFLBY_3DOBJCONT_Jump,
};

static const pWFLBY_3DPARSON_FUNC sc_WFLBY_3DPARSON_END_FUNC[WFLBY_3DOBJCONT_MOVENUM] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	WFLBY_3DOBJCONT_JumpEnd,
};




//-----------------------------------------------------------------------------
/**
 *		影のアルファ値テーブル
 */
//-----------------------------------------------------------------------------
static const u32 sc_WFLBY_3DOBJCONT_SHADOW_ALPHA[ WFLBY_LIGHT_NEON_ROOMNUM ] = {
	// WFLBY_LIGHT_NEON_ROOM0
	{ 16, },

	// WFLBY_LIGHT_NEON_ROOM1
	{ 12, },

	// WFLBY_LIGHT_NEON_ROOM2
	{ 8, },

	// WFLBY_LIGHT_NEON_ROOM3
	{ 6, },

	// WFLBY_LIGHT_NEON_ROOM4
	{ 0, },

	// WFLBY_LIGHT_NEON_ROOM5
	{ 0, },
};



// システム管理
//----------------------------------------------------------------------------
/**
 *	@brief	人物管理システム	初期化
 *
 *	@param	objnum		オブジェクト数
 *	@param	hero_sex	性別
 *	@param	cp_map		マップシステム
 *	@param	heapID		ヒープID
 *	@param	gheapID		グラフィックヒープID
 *	
 *	@return	システムワーク
 */
//-----------------------------------------------------------------------------
WFLBY_3DOBJCONT* WFLBY_3DOBJCONT_Init( u32 objnum, u32 hero_sex, const WFLBY_MAPCONT* cp_map, u32 heapID, u32 gheapID )
{
	WFLBY_3DOBJCONT* p_sys;

	p_sys = sys_AllocMemory( heapID, sizeof(WFLBY_3DOBJCONT) );
	memset( p_sys, 0, sizeof(WFLBY_3DOBJCONT) );

	// システム作成
	p_sys->p_objsys		= WF2DMAP_OBJSysInit( objnum, heapID );
	p_sys->p_drawsys	= WFLBY_3DOBJSYS_Init( objnum, hero_sex, heapID, gheapID );
	p_sys->p_reqQ		= WF2DMAP_REQCMDQSysInit( WFLBY_3DOBJCONT_QBUF_NUM, heapID );

	// オブジェクトワーク作成
	p_sys->objnum		= objnum;
	p_sys->p_objbuf		= sys_AllocMemory( heapID, sizeof(WFLBY_3DPERSON)*objnum );
	memset( p_sys->p_objbuf, 0, sizeof(WFLBY_3DPERSON)*objnum );

	// マップシステムを保存しておく
	p_sys->cp_mapsys = cp_map;

	// 主人公の性別を保存しておく
	p_sys->hero_sex = hero_sex;

	// 受付のおねえさんなどを追加
	WFLBY_3DOBJCONT_MAPOBJ_Init( p_sys );
	
	return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief	人物管理システム	破棄
 *
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_3DOBJCONT_Exit( WFLBY_3DOBJCONT* p_sys )
{
	
	//  オブジェクトワーク破棄
	sys_FreeMemoryEz( p_sys->p_objbuf );
	
	// システム破棄
	WF2DMAP_REQCMDQSysExit( p_sys->p_reqQ );
	WFLBY_3DOBJSYS_Exit( p_sys->p_drawsys );
	WF2DMAP_OBJSysExit( p_sys->p_objsys );

	// システム自体を破棄
	sys_FreeMemoryEz( p_sys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	人物システム	動作処理
 *		
 *	@param	p_sys	システムワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_3DOBJCONT_Move( WFLBY_3DOBJCONT* p_sys )
{
	// オブジェクトシステム
	WF2DMAP_OBJSysMain( p_sys->p_objsys );

	// 影の色を変更
	WFLBY_3DOBJCONT_ShadowAlpha_Main( &p_sys->shadow_alpha, p_sys->p_drawsys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	人物システム	リクエスト反映処理
 *
 *	@param	p_sys	システムワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_3DOBJCONT_ReqMove( WFLBY_3DOBJCONT* p_sys )
{

	// プレイヤー動作
	{
		int i;
		for( i=0; i<p_sys->objnum; i++ ){
			if( WFLBY_3DOBJCONT_CheckCleanWk( &p_sys->p_objbuf[i] ) == FALSE ){
				p_sys->p_objbuf[i].p_func( &p_sys->p_objbuf[i], p_sys );
			}
		}
	}
	
	// 動作チェック処理
	{
		WF2DMAP_REQCMD req;
		WF2DMAP_ACTCMD act;
		BOOL result;
		const WF2DMAP_MAPSYS* cp_map_sys;

		// マップコアシステム
		cp_map_sys = WFLBY_MAPCONT_GetMapCoreSys( p_sys->cp_mapsys );

		// コマンドが間コマンドを実行
		while( WF2DMAP_REQCMDQSysCmdPop( p_sys->p_reqQ, &req ) == TRUE ){
			result = WF2DMAP_JUDGESysCmdJudge( cp_map_sys, p_sys->p_objsys, &req, &act );
			if( result == TRUE ){
				WF2DMAP_OBJSysCmdSet( p_sys->p_objsys, &act );
			}
		}
	}

	// 描画アップデート
	WFLBY_3DOBJSYS_Updata( p_sys->p_drawsys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	人物管理システム	描画処理
 *
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_3DOBJCONT_Draw( WFLBY_3DOBJCONT* p_sys )
{
	WFLBY_3DOBJSYS_Draw( p_sys->p_drawsys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	人物管理システム	VBlank処理
 *
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_3DOBJCONT_VBlank( WFLBY_3DOBJCONT* p_sys )
{
	WFLBY_3DOBJSYS_VBlank( p_sys->p_drawsys );
}


//----------------------------------------------------------------------------
/**
 *	@brief	影の色を初期化する
 *
 *	@param	p_sys		システムワーク
 *	@param	roomtype	部屋の明るさタイプ
 */
//-----------------------------------------------------------------------------
void WFLBY_3DOBJCONT_InitShadowAlpha( WFLBY_3DOBJCONT* p_sys, WFLBY_LIGHT_NEON_ROOMTYPE roomtype )
{
	WFLBY_3DOBJCONT_ShadowAlpha_Init( &p_sys->shadow_alpha, p_sys->p_drawsys, sc_WFLBY_3DOBJCONT_SHADOW_ALPHA[ roomtype ] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	人物の影の色を設定する
 *
 *	@param	p_sys		システムワーク
 *	@param	roomtype	部屋タイプ
 */
//-----------------------------------------------------------------------------
void WFLBY_3DOBJCONT_SetShadowAlpha( WFLBY_3DOBJCONT* p_sys, WFLBY_LIGHT_NEON_ROOMTYPE roomtype )
{
	WFLBY_3DOBJCONT_ShadowAlpha_Start( &p_sys->shadow_alpha, sc_WFLBY_3DOBJCONT_SHADOW_ALPHA[ roomtype ] );
}


// リクエストコマンド設定
//----------------------------------------------------------------------------
/**
 *	@brief	リクエストコマンドを設定
 *
 *	@param	p_sys		システムワーク
 *	@param	cp_cmd		コマンドデータ
 */
//-----------------------------------------------------------------------------
void WFLBY_3DOBJCONT_SetReqCmd( WFLBY_3DOBJCONT* p_sys, const WF2DMAP_REQCMD* cp_cmd )
{
	WF2DMAP_REQCMDQSysCmdPush( p_sys->p_reqQ, cp_cmd );
}

//  ワーク操作
//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤーを登録
 *
 *	@param	p_sys		システムワーク
 *	@param	plid		プレイヤーID
 *
 *	@return	ワーク
 */
//-----------------------------------------------------------------------------
WFLBY_3DPERSON* WFLBY_3DOBJCONT_AddPlayer( WFLBY_3DOBJCONT* p_sys, u32 plid )
{
	u16 addx, addy;
	BOOL result;

	// 登録位置を求める
	result = WFLBY_MAPCONT_SarchObjID( p_sys->cp_mapsys, 
			WFLBY_MAPOBJID_PLAYER_IN, &addx, &addy, 0 );
	GF_ASSERT( result );	// 登録位置がない
	return WFLBY_3DOBJCONT_AddPlayerEx( p_sys, plid, addx, addy );
}

//----------------------------------------------------------------------------
/**
 *	@brief	主人公の登録	出現位置指定
 *
 *	@param	p_sys		システムワーク
 *	@param	plid		プレイヤーID
 *	@param	gridx		グリッドX
 *	@param	gridy		グリッドY
 *
 *	@return	ワーク
 */
//-----------------------------------------------------------------------------
WFLBY_3DPERSON* WFLBY_3DOBJCONT_AddPlayerEx( WFLBY_3DOBJCONT* p_sys, u32 plid, u32 gridx,  u32 gridy )
{
	WFLBY_3DPERSON* p_wk;
	
	// 空のワークを取得
	p_wk = WFLBY_3DOBJCONT_GetCleanWk( p_sys );
	
	// OBJの登録
	{
		WF2DMAP_OBJDATA	objdata;
		objdata.x		= WF2DMAP_GRID2POS(gridx);
		objdata.y		= WF2DMAP_GRID2POS(gridy);
		objdata.playid	= plid;
		objdata.status	= WF2DMAP_OBJST_NONE;
		objdata.way		= WF2DMAP_WAY_UP;
		if( p_sys->hero_sex == PM_MALE ){
			objdata.charaid	= HERO;
		}else{
			objdata.charaid	= HEROINE;
		}
		p_wk->p_obj = WF2DMAP_OBJWkNew( p_sys->p_objsys, &objdata );
	}

	// drawobjの登録
	{
		p_wk->p_draw = WFLBY_3DOBJWK_New( p_sys->p_drawsys, p_wk->p_obj );
	}

	// プレイヤーデータは保存しておく
	p_sys->p_player = p_wk;

	// 動作関数設定
	WFLBY_3DOBJCONT_SetWkMove( p_sys, p_wk, WFLBY_3DOBJCONT_MOVENONE );

	return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	NPCを登録
 *
 *	@param	p_sys	システムワーク
 *	@param	plid	プレイヤーID
 *	@param	trtype	trainerタイプ
 *
 *	@return	ワーク	NULL	今は登録できません
 */
//-----------------------------------------------------------------------------
WFLBY_3DPERSON* WFLBY_3DOBJCONT_AddNpc( WFLBY_3DOBJCONT* p_sys, u32 plid, u32 trtype )
{
	WFLBY_3DPERSON* p_wk;
	
	// 空のワークを取得
	p_wk = WFLBY_3DOBJCONT_GetCleanWk( p_sys );
	
	// OBJの登録
	{
		WF2DMAP_OBJDATA	objdata;
		u16 addx, addy, point_idx;
		BOOL result;
		BOOL add_ok = FALSE;
		WFLBY_3DPERSON* p_player;

		// 登録位置を求める
		// 主人公がいなくて、その人が出ることの出来る場所
		point_idx = 0;
		do{
			result = WFLBY_MAPCONT_SarchObjID( p_sys->cp_mapsys, 
					WFLBY_MAPOBJID_NPC_00+plid, &addx, &addy, point_idx );
			
			if( result == FALSE ){
				OS_TPrintf( "npc 登録不可能\n" );
				return NULL;	// 現在登録不可能
			}else{
				// その位置に主人公がいないかチェック
				p_player = WFLBY_3DOBJCONT_GetPlayer( p_sys );
				if( p_player == NULL ){
					add_ok = TRUE;
				}else{
					if( WFLBY_3DOBJCONT_CheckGridHit( p_player, addx, addy ) == FALSE ){
						add_ok = TRUE;
					}
				}
			}
			
			// 次のポイントをチェック
			point_idx ++;
		}while( add_ok == FALSE );

		objdata.x		= WF2DMAP_GRID2POS(addx);
		objdata.y		= WF2DMAP_GRID2POS(addy);
		objdata.playid	= plid;
		objdata.status	= WF2DMAP_OBJST_NONE;
		objdata.way		= WF2DMAP_WAY_DOWN;
		objdata.charaid	= trtype;
		p_wk->p_obj = WF2DMAP_OBJWkNew( p_sys->p_objsys, &objdata );
	}

	// drawobjの登録
	{
		p_wk->p_draw = WFLBY_3DOBJWK_New( p_sys->p_drawsys, p_wk->p_obj );
	}

	// 動作関数設定
	WFLBY_3DOBJCONT_SetWkMove( p_sys, p_wk, WFLBY_3DOBJCONT_MOVENONE );

	return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	人物を破棄
 *	
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_3DOBJCONT_Delete( WFLBY_3DPERSON* p_wk )
{
	WFLBY_3DOBJWK_Del( p_wk->p_draw );
	WF2DMAP_OBJWkDel( p_wk->p_obj );

	memset( p_wk, 0, sizeof(WFLBY_3DPERSON) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤーIDで人物を取得
 *
 *	@param	p_sys	システムワーク
 *	@param	plid	プレイヤーID
 *
 *	@return	ワーク
 */
//-----------------------------------------------------------------------------
WFLBY_3DPERSON* WFLBY_3DOBJCONT_GetPlIDWk( WFLBY_3DOBJCONT* p_sys, u32 plid )
{
	int i;
	u32 obj_plid;

	for( i=0; i<p_sys->objnum; i++ ){
		if( p_sys->p_objbuf[i].p_obj ){
			obj_plid = WF2DMAP_OBJWkDataGet( p_sys->p_objbuf[i].p_obj, WF2DMAP_OBJPM_PLID );
			if( obj_plid == plid ){
				return &p_sys->p_objbuf[i];
			}
		}
	}

	return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	主人公を取得する
 *
 *	@param	p_sys	ワーク
 *
 *	@return	主人公
 */
//-----------------------------------------------------------------------------
WFLBY_3DPERSON* WFLBY_3DOBJCONT_GetPlayer( WFLBY_3DOBJCONT* p_sys )
{
	return p_sys->p_player;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ピエロを取得
 *
 *	@param	p_sys	ワーク
 *
 *	@return	ピエロオブジェデータ
 */
//-----------------------------------------------------------------------------
WFLBY_3DPERSON* WFLBY_3DOBJCONT_GetPierrot( WFLBY_3DOBJCONT* p_sys )
{
	return p_sys->p_pierrot;
}

//----------------------------------------------------------------------------
/**
 *	@brief	座標の設定
 *
 *	@param	p_wk		ワーク
 *	@param	pos			位置
 */
//-----------------------------------------------------------------------------
void WFLBY_3DOBJCONT_SetWkPos( WFLBY_3DPERSON* p_wk, WF2DMAP_POS pos )
{
	// 現在位置、過去位置両方に入れる
	GF_ASSERT( p_wk->p_obj );
	WF2DMAP_OBJWkMatrixSet( p_wk->p_obj, pos );
	WF2DMAP_OBJWkLastMatrixSet( p_wk->p_obj, pos );
	WF2DMAP_OBJWkDataSet( p_wk->p_obj, WF2DMAP_OBJPM_ST, WF2DMAP_OBJST_NONE );
	
}

//----------------------------------------------------------------------------
/**
 *	@brief	座標設定＋方向
 *
 *	@param	p_wk	ワーク
 *	@param	pos		位置
 *	@param	way		方向
 */
//-----------------------------------------------------------------------------
void WFLBY_3DOBJCONT_SetWkPosAndWay( WFLBY_3DPERSON* p_wk, WF2DMAP_POS pos, WF2DMAP_WAY way )
{
	WFLBY_3DOBJCONT_SetWkPos( p_wk, pos );
	WF2DMAP_OBJWkDataSet( p_wk->p_obj, WF2DMAP_OBJPM_WAY, way );

}

//----------------------------------------------------------------------------
/**
 *	@brief	ポジションを取得する
 *
 *	@param	cp_wk	ワーク
 */
//-----------------------------------------------------------------------------
WF2DMAP_POS WFLBY_3DOBJCONT_GetWkPos( const WFLBY_3DPERSON* cp_wk )
{
	GF_ASSERT( cp_wk->p_obj );
	return WF2DMAP_OBJWkFrameMatrixGet( cp_wk->p_obj );
}

//----------------------------------------------------------------------------
/**
 *	@brief	wf2dmap_objのデータを取得する
 *
 *	@param	cp_wk	ワーク
 *	@param	pm		取得するデータの定数
 *
 *	@return	データ
 */
//-----------------------------------------------------------------------------
u32 WFLBY_3DOBJCONT_GetWkObjData( const WFLBY_3DPERSON* cp_wk, WF2DMAP_OBJPARAM pm )
{
	GF_ASSERT( cp_wk->p_obj );
	return WF2DMAP_OBJWkDataGet( cp_wk->p_obj, pm );
}

//----------------------------------------------------------------------------
/**
 *	@brief	wf2dmap_objのワークを取得する
 *
 *	@param	p_wk	ワーク
 *
 *	@return	WF2DMAP_OBJWKポインタ
 */
//-----------------------------------------------------------------------------
WF2DMAP_OBJWK* WFLBY_3DOBJCONT_GetWkObjWk( WFLBY_3DPERSON* p_wk )
{
	GF_ASSERT( p_wk->p_obj );
	return p_wk->p_obj;
}

//----------------------------------------------------------------------------
/**
 *	@brief	人物のリクエストコマンドを発行する
 *
 *	@param	p_sys		オブジェ管理システム
 *	@param	cp_wk		人物ワーク
 *	@param	cmd			コマンド
 *	@param	way			方向
 */
//-----------------------------------------------------------------------------
void WFLBY_3DOBJCONT_SetWkReqCmd( WFLBY_3DOBJCONT* p_sys, const WFLBY_3DPERSON* cp_wk, WF2DMAP_CMD cmd, WF2DMAP_WAY way )
{
	u32 status;
	u32 playid;

	status = WF2DMAP_OBJWkDataGet( cp_wk->p_obj, WF2DMAP_OBJPM_ST );
	GF_ASSERT_MSG( status == WF2DMAP_OBJST_NONE, "cmd req status not none\n" );
	playid = WF2DMAP_OBJWkDataGet( cp_wk->p_obj, WF2DMAP_OBJPM_PLID );
	WFLBY_3DOBJCONT_REQCMD_Set( p_sys, cmd, way, playid );
}

//----------------------------------------------------------------------------
/**
 *	@brief	アクションコマンドで強制的に動かす
 *
 *	@param	p_sys		オブジェ管理システム
 *	@param	cp_wk		人物ワーク
 *	@param	cmd			コマンド
 *	@param	way			方向
 */
//-----------------------------------------------------------------------------
void WFLBY_3DOBJCONT_SetWkActCmd( WFLBY_3DOBJCONT* p_sys, const WFLBY_3DPERSON* cp_wk, WF2DMAP_CMD cmd, WF2DMAP_WAY way )
{
	WF2DMAP_ACTCMD act;
	act.pos		= WF2DMAP_OBJWkMatrixGet( cp_wk->p_obj );
	act.cmd		= cmd;
	act.way		= way;
	act.playid	= WF2DMAP_OBJWkDataGet( cp_wk->p_obj, WF2DMAP_OBJPM_PLID );
	WF2DMAP_OBJSysCmdSet( p_sys->p_objsys, &act );
}

//----------------------------------------------------------------------------
/**
 *	@brief	人物の動作を設定
 *
 *	@param	p_wk		ワーク
 *	@param	movetype	動作タイプ
 */
//-----------------------------------------------------------------------------
void WFLBY_3DOBJCONT_SetWkMove( WFLBY_3DOBJCONT* p_sys, WFLBY_3DPERSON* p_wk, WFLBY_3DOBJCONT_MOVETYPE movetype )
{
	GF_ASSERT( movetype < WFLBY_3DOBJCONT_MOVENUM );

	// 動作の終了処理
	if( sc_WFLBY_3DPARSON_END_FUNC[ p_wk->movetype ] != NULL ){
		sc_WFLBY_3DPARSON_END_FUNC[ p_wk->movetype ]( p_wk, p_sys );
	}
	
	p_wk->seq		= 0;
	p_wk->moveend	= FALSE;
	p_wk->movetype	= movetype;
	memset( &p_wk->work, 0, sizeof(WFLBY_3DPARSON_MOVEWK) );
	p_wk->p_func	= sc_WFLBY_3DPARSON_FUNC[ movetype ];
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤー動作が終了しているかチェック
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_3DOBJCONT_CheckWkMoveEnd( const WFLBY_3DPERSON* cp_wk )
{
	return cp_wk->moveend;
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤー動作タイプを取得する
 *
 *	@param	cp_wk	ワーク
 *
 *	@return	動作タイプ
 */
//-----------------------------------------------------------------------------
WFLBY_3DOBJCONT_MOVETYPE WFLBY_3DOBJCONT_GetWkMove( const WFLBY_3DPERSON* cp_wk )
{
	return cp_wk->movetype;
}

//----------------------------------------------------------------------------
/**
 *	@brief	人物の目の前の人物を取得する
 *
 *	@param	p_sys		システムワーク
 *	@param	cp_wk		ワーク
 *
 *	@retval			目の前の人物
 *	@retval	NULL	目の前には誰もいない
 */
//-----------------------------------------------------------------------------
WFLBY_3DPERSON* WFLBY_3DOBJCONT_GetFrontPerson( WFLBY_3DOBJCONT* p_sys, const WFLBY_3DPERSON* cp_wk )
{
	const WF2DMAP_OBJWK* cp_hitobj;
	u32 way;
	u32 hitobj_userid;

	way = WF2DMAP_OBJWkDataGet( cp_wk->p_obj, WF2DMAP_OBJPM_WAY );
	cp_hitobj = WF2DMAP_OBJSysHitCheck( cp_wk->p_obj, p_sys->p_objsys, way );

	// いたか？
	if( cp_hitobj == NULL ){
		return NULL;
	}

	hitobj_userid = WF2DMAP_OBJWkDataGet( cp_hitobj, WF2DMAP_OBJPM_PLID );
	return WFLBY_3DOBJCONT_GetPlIDWk( p_sys, hitobj_userid );
}

//----------------------------------------------------------------------------
/**
 *	@brief	カリングフラグ設定
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	TRUE	カリングして非表示中
 *	@retval	FALSE	カリングしてない表示中
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_3DOBJCONT_GetCullingFlag( const WFLBY_3DPERSON* cp_wk )
{
	return WFLBY_3DOBJWK_GetCullingFlag( cp_wk->p_draw );
}

//----------------------------------------------------------------------------
/**
 *	@brief	指定したグリッドに人物がいるかチェック
 *
 *	@param	cp_sys		システム
 *	@param	gridx		グリッドｘ
 *	@param	gridy		グリッドｙ
 *
 *	@retval	NULL以外	自分物がいる
 *	@retval	NULL		人物はいない
 */
//-----------------------------------------------------------------------------
const WFLBY_3DPERSON* WFLBY_3DOBJCONT_CheckSysGridHit( const WFLBY_3DOBJCONT* cp_sys, u16 gridx, u16 gridy )
{
	const WF2DMAP_OBJWK* cp_obj;
	int i;
	WF2DMAP_POS pos;

	pos.x = WF2DMAP_GRID2POS( gridx );
	pos.y = WF2DMAP_GRID2POS( gridy );
	
	cp_obj = WF2DMAP_OBJSysPosHitCheck( cp_sys->p_objsys, pos );

	if( cp_obj == NULL ){	// いなかった
		return NULL;
	}

	// そのオブジェの自分物データを返す
	for( i=0; i<cp_sys->objnum; i++ ){

		if( cp_sys->p_objbuf[i].p_obj == cp_obj ){
			return &cp_sys->p_objbuf[i];
		}
	}

	GF_ASSERT( 0 );	// おかしい
	return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	グリッド位置にこのオブジェがいるかチェックする
 *
 *	@param	cp_wk		ワーク
 *	@param	gridx		グリッドｘ座標
 *	@param	gridy		グリッドｙ座標
 *
 *	@retval	TRUE	その位置にいる
 *	@retval	FALSE	その位置にいない
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_3DOBJCONT_CheckGridHit( const WFLBY_3DPERSON* cp_wk, u16 gridx, u16 gridy )
{
	WF2DMAP_POS pos;
	WF2DMAP_POS last_pos;
	
	pos			= WF2DMAP_OBJWkMatrixGet( cp_wk->p_obj );
	last_pos	= WF2DMAP_OBJWkLastMatrixGet( cp_wk->p_obj );

	if( (pos.x == WF2DMAP_GRID2POS(gridx)) && (pos.y == WF2DMAP_GRID2POS(gridy)) ){
		return TRUE;
	}
	if( (last_pos.x == WF2DMAP_GRID2POS(gridx)) && (last_pos.y == WF2DMAP_GRID2POS(gridy)) ){
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	人物外４方向に空いている場所があるかチェックする
 *
 *	@param	p_sys		システム
 *	@param	cp_wk		ワーク
 *	@param	p_way		方向
 *	@param	p_pos		位置
 *
 *	@retval	TRUE	空いているところがあった
 *	@retval	FALSE	空いているところがない
 *
 *	優先順位は、右＞左＞下＞上
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_3DOBJCONT_GetOpenGird4Way( WFLBY_3DOBJCONT* p_sys, const WFLBY_3DPERSON* cp_wk, u32* p_way, WF2DMAP_POS* p_pos )
{
	static const u8 sc_WAY[ WF2DMAP_WAY_NUM ] = {
		WF2DMAP_WAY_RIGHT,	
		WF2DMAP_WAY_LEFT,	
		WF2DMAP_WAY_DOWN,	
		WF2DMAP_WAY_UP,	
	};
	int i;
	WF2DMAP_POS pos;
	WF2DMAP_POS waypos;
	BOOL hit;
	u32 eventid;
	const WF2DMAP_OBJWK* cp_hitobj;

	// 人物の座標を取得
	pos = WF2DMAP_OBJWkMatrixGet( cp_wk->p_obj );

	
	for( i=0; i<WF2DMAP_WAY_NUM; i++ ){
		waypos = WF2DMAP_OBJToolWayPosGet( pos, sc_WAY[i] );	

		// 地形と判定
		// 当たり判定
		hit = WFLBY_MAPCONT_HitGet( p_sys->cp_mapsys, 
				WF2DMAP_POS2GRID(waypos.x), WF2DMAP_POS2GRID(waypos.y) );
		if( hit == TRUE ){
			continue;	// 次へ
		}
		// その位置に何かのイベントがないか
		eventid = WFLBY_MAPCONT_EventGet( p_sys->cp_mapsys, 
				WF2DMAP_POS2GRID(waypos.x), WF2DMAP_POS2GRID(waypos.y) );
		if( (eventid != 0) && (eventid != WFLBY_MAPEVID_EV_ROOF_MAT) ){
			continue;	// 次へ
		}

		// 自分物がいないかチェック
		cp_hitobj = WF2DMAP_OBJSysPosHitCheck( p_sys->p_objsys, waypos );
		if( cp_hitobj != NULL ){
			continue;	// 次へ
		}

		// この位置空いてます
		*p_way = sc_WAY[i];
		*p_pos = waypos;
		return TRUE;
	}
	return FALSE;
}

// 外側から表示部分のみ変更する
//----------------------------------------------------------------------------
/**
 *	@brief	描画更新フラグを設定
 *
 *	@param	p_wk		ワーク
 *	@param	updata		アップデートフラグ
 */
//-----------------------------------------------------------------------------
void WFLBY_3DOBJCONT_DRAW_SetUpdata( WFLBY_3DPERSON* p_wk, BOOL updata )
{
	WFLBY_3DOBJWK_SetUpdata( p_wk->p_draw, updata );
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画アップデートフラグを取得
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	TRUE	アップデート中
 *	@retval	FALSE	アップデートしない
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_3DOBJCONT_DRAW_GetUpdata( const WFLBY_3DPERSON* cp_wk )
{
	return WFLBY_3DOBJWK_GetUpdata( cp_wk->p_draw );
}

//----------------------------------------------------------------------------
/**
 *	@brief	座標を設定する
 *
 *	@param	p_wk		ワーク
 *	@param	cp_pos		位置
 */
//-----------------------------------------------------------------------------
void WFLBY_3DOBJCONT_DRAW_SetMatrix( WFLBY_3DPERSON* p_wk, const WF2DMAP_POS* cp_pos )
{
	WFLBY_3DOBJWK_SetMatrix( p_wk->p_draw, cp_pos );
}

//----------------------------------------------------------------------------
/**
 *	@brief	３D座標を設定
 *
 *	@param	p_wk		ワーク
 *	@param	cp_vec		ベクトル
 */
//-----------------------------------------------------------------------------
void WFLBY_3DOBJCONT_DRAW_Set3DMatrix( WFLBY_3DPERSON* p_wk, const VecFx32* cp_vec )
{
	WFLBY_3DOBJWK_Set3DMatrix( p_wk->p_draw, cp_vec );
}

//----------------------------------------------------------------------------
/**
 *	@brief	3D座標を取得する
 *
 *	@param	cp_wk	ワーク
 *	@param	p_vec	ベクトル格納先
 */
//-----------------------------------------------------------------------------
void WFLBY_3DOBJCONT_DRAW_Get3DMatrix( const WFLBY_3DPERSON* cp_wk, VecFx32* p_vec )
{
	WFLBY_3DOBJWK_Get3DMatrix( cp_wk->p_draw, p_vec );
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画する方向を変更
 *
 *	@param	p_wk	ワーク
 *	@param	way		方向
 */
//-----------------------------------------------------------------------------
void WFLBY_3DOBJCONT_DRAW_SetWay( WFLBY_3DPERSON* p_wk, WF2DMAP_WAY way )
{
	WFLBY_3DOBJWK_SetWay( p_wk->p_draw, way );
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画	ジャンプシーンの描画
 *
 *	@param	p_wk		ワーク
 *	@param	flag		ＴＲＵＥ：再生	ＦＡＬＳＥ：オワリ
 */
//-----------------------------------------------------------------------------
void WFLBY_3DOBJCONT_DRAW_SetAnmJump( WFLBY_3DPERSON* p_wk, BOOL flag )
{
	if( flag ){
		WFLBY_3DOBJWK_StartAnm( p_wk->p_draw, WFLBY_3DOBJ_ANM_JUMP );
	}else{
		WFLBY_3DOBJWK_EndAnm( p_wk->p_draw );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画	回転アニメ描画
 *
 *	@param	p_wk		ワーク
 *	@param	flag		ＴＲＵＥ：再生	ＦＡＬＳＥ：オワリ
 */
//-----------------------------------------------------------------------------
void WFLBY_3DOBJCONT_DRAW_SetAnmRota( WFLBY_3DPERSON* p_wk, BOOL flag )
{
	if( flag ){
		WFLBY_3DOBJWK_StartAnm( p_wk->p_draw, WFLBY_3DOBJ_ANM_ROTA );
	}else{
		WFLBY_3DOBJWK_EndAnm( p_wk->p_draw );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画	バタバタアニメ描画
 *
 *	@param	p_wk		ワーク
 *	@param	flag		ＴＲＵＥ：再生	ＦＡＬＳＥ：オワリ
 */
//-----------------------------------------------------------------------------
void WFLBY_3DOBJCONT_DRAW_SetAnmBata( WFLBY_3DPERSON* p_wk, BOOL flag )
{
	if( flag ){
		WFLBY_3DOBJWK_StartAnm( p_wk->p_draw, WFLBY_3DOBJ_ANM_BATABATA );
	}else{
		WFLBY_3DOBJWK_EndAnm( p_wk->p_draw );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画	ライトフラグをON
 *
 *	@param	p_wk		ワーク
 *	@param	light_msk	ライトマスク
 */
//-----------------------------------------------------------------------------
void WFLBY_3DOBJCONT_DRAW_SetLight( WFLBY_3DPERSON* p_wk, u32 light_msk )
{
	WFLBY_3DOBJWK_SetLight( p_wk->p_draw, light_msk );
}


//----------------------------------------------------------------------------
/**
 *	@brief	描画	描画フラグを設定
 *
 *	@param	p_wk		ワーク
 *	@param	flag		フラグ
 */
//-----------------------------------------------------------------------------
void WFLBY_3DOBJCONT_DRAW_SetDraw( WFLBY_3DPERSON* p_wk,  BOOL flag )
{
	WFLBY_3DOBJWK_SetDrawFlag( p_wk->p_draw, flag );
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画	描画フラグを取得
 *
 *	@param	cp_wk		ワーク
 *
 *	@retval	TRUE	描画中
 *	@retval	FALSE	非表示中
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_3DOBJCONT_DRAW_GetDraw( const WFLBY_3DPERSON* cp_wk )
{
	return WFLBY_3DOBJWK_GetDrawFlag( cp_wk->p_draw );
}








//-----------------------------------------------------------------------------
/**
 *				プライベート関数
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	マップ配置人物の配置
 *
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DOBJCONT_MAPOBJ_Init( WFLBY_3DOBJCONT* p_sys )
{
	int i;
	WFLBY_3DPERSON* p_wk;
	u16 addx, addy;
	BOOL result;
	u16 idx;
	
	// マップの中に配置オブジェがいないかチェック
	for( i=0; i<NELEMS(sc_WFLBY_3DPARSON_MAPOBJ); i++ ){
		idx = 0;
		while( WFLBY_MAPCONT_SarchObjID( p_sys->cp_mapsys, 
					sc_WFLBY_3DPARSON_MAPOBJ[i].mapobjid, &addx, &addy, idx ) == TRUE ){

			// 空のワークを取得
			p_wk = WFLBY_3DOBJCONT_GetCleanWk( p_sys );

			// 登録
			{
				WF2DMAP_OBJDATA	objdata;

				objdata.x		= WF2DMAP_GRID2POS(addx);
				objdata.y		= WF2DMAP_GRID2POS(addy);
				objdata.playid	= sc_WFLBY_3DPARSON_MAPOBJ[i].playid;
				objdata.status	= WF2DMAP_OBJST_NONE;
				objdata.way		= sc_WFLBY_3DPARSON_MAPOBJ[i].way;
				objdata.charaid	= sc_WFLBY_3DPARSON_MAPOBJ[i].trtype;
				p_wk->p_obj = WF2DMAP_OBJWkNew( p_sys->p_objsys, &objdata );
			}

			// drawobjの登録
			{
				p_wk->p_draw = WFLBY_3DOBJWK_New( p_sys->p_drawsys, p_wk->p_obj );
			}

			// 動作関数設定
			WFLBY_3DOBJCONT_SetWkMove( p_sys, p_wk, WFLBY_3DOBJCONT_MOVENONE );

			// ピエロだけ保存する
			if( sc_WFLBY_3DPARSON_MAPOBJ[i].mapobjid == WFLBY_MAPOBJID_ANKETO_MAN ){
				p_sys->p_pierrot = p_wk;
			}

			idx ++;
		}
	}
}



//----------------------------------------------------------------------------
/**
 *	@brief	キューにコマンドを設定
 *
 *	@param	p_sys		システムワーク
 *	@param	cmd			コマンド
 *	@param	way			方向
 *	@param	playid		プレイヤーデータ
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DOBJCONT_REQCMD_Set( WFLBY_3DOBJCONT* p_sys, s32 cmd, s32 way, s32 playid )
{
	WF2DMAP_REQCMD reqcmd;
	
	reqcmd.cmd = cmd;
	reqcmd.way = way;
	reqcmd.playid = playid;
	WFLBY_3DOBJCONT_SetReqCmd( p_sys, &reqcmd );
}





//----------------------------------------------------------------------------
/**
 *	@brief	テーブルの中から好きなデータを乱数で取得する
 *
 *	@param	cp_tbl		テーブル
 *	@param	tblnum		テーブル数
 *
 *	@return	選択された数字
 */
//-----------------------------------------------------------------------------
static u8 WFLBY_3DOBJCONT_GetRndTbl( const u8* cp_tbl, u32 tblnum )
{
	int idx;
	idx = gf_mtRand() % tblnum;
	return cp_tbl[idx];
}

//----------------------------------------------------------------------------
/**
 *	@brief	人物のwayの方向にあるのがobjidのマップかチェック
 *
 *	@param	cp_sys		システム
 *	@param	cp_obj		人物ワーク
 *	@param	way			方向
 *	@param	objid		オブジェID
 *
 *	@retval	TRUE	wayの方向にあるのはobjid
 *	@retval	FALSE	wayの方向にあるのはobjidではない
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_3DOBJCONT_CheckWayObjID( const WFLBY_3DOBJCONT* cp_sys, const WFLBY_3DPERSON* cp_obj, WF2DMAP_WAY way, u32 objid )
{
	WF2DMAP_POS pos;
	u32 mapobjid;

	// way方向の座標を求める
	pos = WF2DMAP_OBJWkMatrixGet( cp_obj->p_obj );
	pos = WF2DMAP_OBJToolWayPosGet( pos, way );

	// そこのOBJIDを取得
	mapobjid = WFLBY_MAPCONT_ObjIDGet( cp_sys->cp_mapsys, WF2DMAP_POS2GRID(pos.x), WF2DMAP_POS2GRID(pos.y) );
	
	if( mapobjid == objid ){
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	線形動作初期化
 *
 *	@param	p_wk		ワーク
 *	@param	s_x			開始座標
 *	@param	e_x			終了座標
 *	@param	count_max	カウント最大値
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DOBJCONT_MOVEWK_Init( WFLBY_3DOBJCONT_MOVE_WK* p_wk, fx32 s_x, fx32 e_x, u32 count_max )
{
	p_wk->x			= s_x;
	p_wk->s_x		= s_x;
	p_wk->dis_x		= e_x - s_x;
	p_wk->count_max	= count_max;
}

//----------------------------------------------------------------------------
/**
 *	@brief	線形動作メイン
 *
 *	@param	p_wk		ワーク
 *	@param	count		カウンタ
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_3DOBJCONT_MOVEWK_Main( WFLBY_3DOBJCONT_MOVE_WK* p_wk, u32 count )
{
	fx32 w_x;

	// 現在座標取得
	w_x = FX_Mul( p_wk->dis_x, FX32_CONST(count) );
	w_x = FX_Div( w_x, FX32_CONST(p_wk->count_max) );

	
	p_wk->x = w_x + p_wk->s_x;

	if( count >= p_wk->count_max ){
		return TRUE;
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	値を取得する
 *
 *	@param	cp_wk	ワーク
 *
 *	@return	値
 */
//-----------------------------------------------------------------------------
static fx32 WFLBY_3DOBJCONT_MOVEWK_GetNum( const WFLBY_3DOBJCONT_MOVE_WK* cp_wk )
{
	return cp_wk->x;
}




//----------------------------------------------------------------------------
/**
 *	@brief	空いているワークを取得する
 *
 *	@param	p_sys		システム 
 *
 *	@return	空いているワーク
 */
//-----------------------------------------------------------------------------
static WFLBY_3DPERSON* WFLBY_3DOBJCONT_GetCleanWk( WFLBY_3DOBJCONT* p_sys )
{
	int i;

	for( i=0; i<p_sys->objnum; i++ ){
		if( WFLBY_3DOBJCONT_CheckCleanWk( &p_sys->p_objbuf[i] ) ){
			return &p_sys->p_objbuf[i];
		}
	}

	GF_ASSERT(0);
	return &p_sys->p_objbuf[i];
}

//----------------------------------------------------------------------------
/**
 *	@brief	空いているワークかチェック
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	TRUE	空いてる
 *	@retval	FALSE	空いてない
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_3DOBJCONT_CheckCleanWk( const WFLBY_3DPERSON* cp_wk )
{
	if( cp_wk->p_obj == NULL ){
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	動作なし
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DOBJCONT_MoveNone(  WFLBY_3DPERSON* p_wk, WFLBY_3DOBJCONT* p_sys )
{
}

//----------------------------------------------------------------------------
/**
 *	@brief	主人公の動作
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DOBJCONT_PlayerMove( WFLBY_3DPERSON* p_wk, WFLBY_3DOBJCONT* p_sys  )
{
	s32 way;
	s32 playid;
	s32 status;
	s32 cmd;
	const WF2DMAP_OBJWK* cp_wk;

	way = WF2DMAP_OBJWkDataGet( p_wk->p_obj, WF2DMAP_OBJPM_WAY );
	playid = WF2DMAP_OBJWkDataGet( p_wk->p_obj, WF2DMAP_OBJPM_PLID );

	// 待機状態でなければならない
	status = WF2DMAP_OBJWkDataGet( p_wk->p_obj, WF2DMAP_OBJPM_ST );
	if( status != WF2DMAP_OBJST_NONE ){
		return;
	}


	// 歩くか走るか
	if( sys.cont & PAD_BUTTON_B ){
		cmd = WF2DMAP_OBJST_RUN;
	}else{
		cmd = WF2DMAP_OBJST_WALK;
	}
	
	// 移動関係
	if( sys.cont & PAD_KEY_UP ){
		if( way == WF2DMAP_WAY_UP ){
			WFLBY_3DOBJCONT_REQCMD_Set( p_sys, cmd, way, playid );
		}else{
			WFLBY_3DOBJCONT_REQCMD_Set( p_sys, WF2DMAP_OBJST_TURN, WF2DMAP_WAY_UP, playid );
		}
	}else if( sys.cont & PAD_KEY_DOWN ){
		if( way == WF2DMAP_WAY_DOWN ){
			WFLBY_3DOBJCONT_REQCMD_Set( p_sys, cmd, way, playid );
		}else{
			WFLBY_3DOBJCONT_REQCMD_Set( p_sys, WF2DMAP_OBJST_TURN, WF2DMAP_WAY_DOWN, playid );
		}
	}else if( sys.cont & PAD_KEY_LEFT ){
		if( way == WF2DMAP_WAY_LEFT ){
			WFLBY_3DOBJCONT_REQCMD_Set( p_sys, cmd, way, playid );
		}else{
			WFLBY_3DOBJCONT_REQCMD_Set( p_sys, WF2DMAP_OBJST_TURN, WF2DMAP_WAY_LEFT, playid );
		}
	}else if( sys.cont & PAD_KEY_RIGHT ){
		if( way == WF2DMAP_WAY_RIGHT ){
			WFLBY_3DOBJCONT_REQCMD_Set( p_sys, cmd, way, playid );
		}else{
			WFLBY_3DOBJCONT_REQCMD_Set( p_sys, WF2DMAP_OBJST_TURN, WF2DMAP_WAY_RIGHT, playid );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	NPCの動作
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DOBJCONT_NpcMove( WFLBY_3DPERSON* p_wk, WFLBY_3DOBJCONT* p_sys )
{
	switch( p_wk->seq ){
	// ウエイトを乱数で決定
	case WFLBY_3DOBJCONT_NPC_SEQ_WAITSET:
		p_wk->work.npc.wait = WFLBY_3DOBJCONT_GetRndTbl( sc_WFLBY_3DOBJCONT_NPC_WAIT, NELEMS( sc_WFLBY_3DOBJCONT_NPC_WAIT ) );
		p_wk->seq++;
		break;
	// ウエイト
	case WFLBY_3DOBJCONT_NPC_SEQ_WAIT:
		p_wk->work.npc.wait --;
		if( p_wk->work.npc.wait == 0 ){
			p_wk->seq++;
		}
		break;
	// 歩く又は振り向く方向を乱数で決定
	case WFLBY_3DOBJCONT_NPC_SEQ_WAYSET:
		{
			u32 plno;
			u32 way;

			// ユーザNOと方向を取得
			plno = WF2DMAP_OBJWkDataGet( p_wk->p_obj, WF2DMAP_OBJPM_PLID );
			way	 = WF2DMAP_OBJWkDataGet( p_wk->p_obj, WF2DMAP_OBJPM_WAY );
			
			// 乱数から方向を求める
			p_wk->work.npc.way = WFLBY_3DOBJCONT_GetRndTbl( sc_WFLBY_3DOBJCONT_NPC_WAY, NELEMS( sc_WFLBY_3DOBJCONT_NPC_WAY ) );

			// その方向に進んで大丈夫かチェック
			// 大丈夫ー＞歩く
			// だめ　－＞振り向くだけ
			if( WFLBY_3DOBJCONT_CheckWayObjID( p_sys, p_wk, p_wk->work.npc.way, WFLBY_MAPOBJID_NPC_00+plno ) == TRUE ){

				// もうその方向を向いているのか？
				if( way == p_wk->work.npc.way ){
					WFLBY_3DOBJCONT_SetWkReqCmd( p_sys, p_wk, WF2DMAP_CMD_WALK, p_wk->work.npc.way );
					p_wk->seq = WFLBY_3DOBJCONT_NPC_SEQ_WAYMOVEWAIT;
				}else{
					WFLBY_3DOBJCONT_SetWkReqCmd( p_sys, p_wk, WF2DMAP_CMD_TURN, p_wk->work.npc.way );
					p_wk->seq = WFLBY_3DOBJCONT_NPC_SEQ_WAYMOVEWALKTURN;
				}
			}else{
				WFLBY_3DOBJCONT_SetWkReqCmd( p_sys, p_wk, WF2DMAP_CMD_TURN, p_wk->work.npc.way );
				p_wk->seq = WFLBY_3DOBJCONT_NPC_SEQ_WAYMOVEWAIT;
			}
			
		}
		break;
	// 歩く動作で振り向く必要がある場合
	case WFLBY_3DOBJCONT_NPC_SEQ_WAYMOVEWALKTURN:
		{
			u32 st;
			st = WF2DMAP_OBJWkDataGet( p_wk->p_obj, WF2DMAP_OBJPM_ST );
			if( st == WF2DMAP_OBJST_NONE ){
				WFLBY_3DOBJCONT_SetWkReqCmd( p_sys, p_wk, WF2DMAP_CMD_WALK, p_wk->work.npc.way );
				p_wk->seq ++;
			}
		}
		break;
	// 歩く・振り向く動作の終了待ち
	case WFLBY_3DOBJCONT_NPC_SEQ_WAYMOVEWAIT:
		{
			u32 st;
			st = WF2DMAP_OBJWkDataGet( p_wk->p_obj, WF2DMAP_OBJPM_ST );
			if( st == WF2DMAP_OBJST_NONE ){
				p_wk->seq = WFLBY_3DOBJCONT_NPC_SEQ_WAITSET;
			}
		}
		break;
	}
}



//----------------------------------------------------------------------------
/**
 *	@brief	真上に飛んでいく
 *
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DOBJCONT_FlyUpMove( WFLBY_3DPERSON* p_wk, WFLBY_3DOBJCONT* p_sys )
{
	switch( p_wk->seq ){
	// パラメータ初期化	そのままMAINへ
	case WFLBY_3DOBJCONT_FLYUP_SEQ_INIT:
		// グラフィック更新をOFF
		WFLBY_3DOBJWK_SetUpdata( p_wk->p_draw, FALSE );
		WFLBY_3DOBJWK_StartAnm( p_wk->p_draw, WFLBY_3DOBJ_ANM_ROTA );
		
		// 線形動作初期化
		WFLBY_3DOBJCONT_MOVEWK_Init( &p_wk->work.fly.y, 
				WFLBY_3DMATRIX_FLOOR_MAT, WFLBY_3DOBJCONT_FLY_Y, WFLBY_3DOBJCONT_FLY_COUNT );
		p_wk->work.fly.count = WFLBY_3DOBJCONT_ROTA_ONE_COUNT;
		p_wk->seq ++;

	// さらに１回転
	case WFLBY_3DOBJCONT_FLYUP_SEQ_ONEROTA:
		p_wk->work.fly.count--;
		if( p_wk->work.fly.count == 0 ){
			p_wk->seq ++;

			p_wk->work.fly.count = 0;


			// 上っていく音
			Snd_SePlay( WFLBY_SND_PLOUT );

		}
		break;
		
	// 動作メイン
	case WFLBY_3DOBJCONT_FLYUP_SEQ_MAIN:
		{
			BOOL result;
			VecFx32 vec;

			result = WFLBY_3DOBJCONT_MOVEWK_Main( &p_wk->work.fly.y, p_wk->work.fly.count );

			p_wk->work.fly.count ++;

			// 座標を設定
			WFLBY_3DOBJWK_Get3DMatrix( p_wk->p_draw, &vec );
			vec.y = WFLBY_3DOBJCONT_MOVEWK_GetNum( &p_wk->work.fly.y );
			WFLBY_3DOBJWK_Set3DMatrix( p_wk->p_draw, &vec );

			if( result == TRUE ){
				p_wk->seq ++;

				WFLBY_3DOBJWK_EndAnm( p_wk->p_draw );
				p_wk->moveend = TRUE;
			}
		}
		break;
	// 終了待ち
	case WFLBY_3DOBJCONT_FLYDOWN_SEQ_END:
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	下に下りてくる
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DOBJCONT_FlyDownMove( WFLBY_3DPERSON* p_wk, WFLBY_3DOBJCONT* p_sys )
{
	switch( p_wk->seq ){
	// パラメータ初期化	そのままMAINへ
	case WFLBY_3DOBJCONT_FLYDOWN_SEQ_INIT:
		// グラフィック更新をOFF
		WFLBY_3DOBJWK_SetUpdata( p_wk->p_draw, FALSE );
		WFLBY_3DOBJWK_StartAnm( p_wk->p_draw, WFLBY_3DOBJ_ANM_ROTA );
		
		// 線形動作初期化
		WFLBY_3DOBJCONT_MOVEWK_Init( &p_wk->work.fly.y, 
				WFLBY_3DOBJCONT_FLY_Y, WFLBY_3DMATRIX_FLOOR_MAT, WFLBY_3DOBJCONT_FLY_COUNT );
		p_wk->work.fly.count = 0;
		p_wk->seq ++;

		// 落ちてくる音
		Snd_SePlay( WFLBY_SND_PLOUT );

	// 動作メイン
	case WFLBY_3DOBJCONT_FLYDOWN_SEQ_MAIN:
		{
			BOOL result;
			VecFx32 vec;

			result = WFLBY_3DOBJCONT_MOVEWK_Main( &p_wk->work.fly.y, p_wk->work.fly.count );

			p_wk->work.fly.count ++;

			// 座標を設定
			WFLBY_3DOBJWK_Get3DMatrix( p_wk->p_draw, &vec );
			vec.y = WFLBY_3DOBJCONT_MOVEWK_GetNum( &p_wk->work.fly.y );
			WFLBY_3DOBJWK_Set3DMatrix( p_wk->p_draw, &vec );

			if( result == TRUE ){
				p_wk->work.fly.count = WFLBY_3DOBJCONT_ROTA_ONE_COUNT;
				p_wk->seq ++;
			}
		}
		break;
	// さらに１回転
	case WFLBY_3DOBJCONT_FLYDOWN_SEQ_ONEROTA:
		p_wk->work.fly.count--;
		if( p_wk->work.fly.count == 0 ){
			p_wk->seq ++;

			// 描画更新をON
			WFLBY_3DOBJWK_EndAnm( p_wk->p_draw );
			WFLBY_3DOBJWK_SetUpdata( p_wk->p_draw, TRUE );
			p_wk->moveend = TRUE;
		}
		break;
	// 終了待ち
	case WFLBY_3DOBJCONT_FLYDOWN_SEQ_END:
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ある方向に回転しながら１グリッド吹き飛ぶ
 *
 *	@param	p_wk	ワーク
 *	@param	p_sys	システム
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DOBJCONT_RotaLeftMove( WFLBY_3DPERSON* p_wk, WFLBY_3DOBJCONT* p_sys )
{
	WFLBY_3DOBJCONT_RotaMove( p_wk, p_sys, WF2DMAP_WAY_LEFT );
}
static void WFLBY_3DOBJCONT_RotaRightMove( WFLBY_3DPERSON* p_wk, WFLBY_3DOBJCONT* p_sys )
{
	WFLBY_3DOBJCONT_RotaMove( p_wk, p_sys, WF2DMAP_WAY_RIGHT );
}

static void WFLBY_3DOBJCONT_RotaUpMove( WFLBY_3DPERSON* p_wk, WFLBY_3DOBJCONT* p_sys )
{
	WFLBY_3DOBJCONT_RotaMove( p_wk, p_sys, WF2DMAP_WAY_UP );
}
static void WFLBY_3DOBJCONT_RotaDownMove( WFLBY_3DPERSON* p_wk, WFLBY_3DOBJCONT* p_sys )
{
	WFLBY_3DOBJCONT_RotaMove( p_wk, p_sys, WF2DMAP_WAY_DOWN );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ある方向に回転しながら吹き飛ぶ
 *
 *	@param	p_wk		ワーク
 *	@param	p_sys		システムワーク
 *	@param	way			方向
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DOBJCONT_RotaMove( WFLBY_3DPERSON* p_wk, WFLBY_3DOBJCONT* p_sys, WF2DMAP_WAY way )
{
	switch( p_wk->seq ){
	// 動作初期化
	case WFLBY_3DOBJCONT_ROTA_SEQ_INIT:
		// グラフィック更新をOFF
		WFLBY_3DOBJWK_SetUpdata( p_wk->p_draw, FALSE );
		WFLBY_3DOBJWK_StartAnm( p_wk->p_draw, WFLBY_3DOBJ_ANM_ROTA );
		WFLBY_3DOBJWK_SetAnmSpeed( p_wk->p_draw, WFLBY_3DOBJCONT_ROTA_ANMSPEED );

		// 線形動作初期化
		WFLBY_3DOBJCONT_MOVEWK_Init( &p_wk->work.rota.dist, 
				0, WFLBY_3DOBJCONT_ROTA_DIST, WFLBY_3DOBJCONT_ROTA_COUNT );

		// 基本となるその位置を取得
		{
			VecFx32 vec;
			WFLBY_3DOBJWK_Get3DMatrix( p_wk->p_draw, &vec );
			switch( way ){
			case WF2DMAP_WAY_UP:
			case WF2DMAP_WAY_DOWN:
				p_wk->work.rota.def = vec.z;
				break;

			case WF2DMAP_WAY_LEFT:
			case WF2DMAP_WAY_RIGHT:
				p_wk->work.rota.def = vec.x;
				break;
			}
		}
		p_wk->work.rota.count = 0;
		p_wk->seq ++;
		
	// 動作メイン
	case WFLBY_3DOBJCONT_ROTA_SEQ_MAIN:
		{
			BOOL result;
			VecFx32 vec;

			result = WFLBY_3DOBJCONT_MOVEWK_Main( &p_wk->work.rota.dist, p_wk->work.rota.count );

			p_wk->work.rota.count ++;

			// 座標を設定
			WFLBY_3DOBJWK_Get3DMatrix( p_wk->p_draw, &vec );
			switch( way ){
			case WF2DMAP_WAY_UP:
				vec.z = p_wk->work.rota.def - WFLBY_3DOBJCONT_MOVEWK_GetNum( &p_wk->work.rota.dist );
				break;
				
			case WF2DMAP_WAY_DOWN:
				vec.z = p_wk->work.rota.def + WFLBY_3DOBJCONT_MOVEWK_GetNum( &p_wk->work.rota.dist );
				break;

			case WF2DMAP_WAY_LEFT:
				vec.x = p_wk->work.rota.def - WFLBY_3DOBJCONT_MOVEWK_GetNum( &p_wk->work.rota.dist );
				break;
				
			case WF2DMAP_WAY_RIGHT:
				vec.x = p_wk->work.rota.def + WFLBY_3DOBJCONT_MOVEWK_GetNum( &p_wk->work.rota.dist );
			
				break;
			}
			WFLBY_3DOBJWK_Set3DMatrix( p_wk->p_draw, &vec );

			if( result == TRUE ){
				u32 retway;
				retway	= WF2DMPA_OBJToolRetWayGet( way );
				// 方向によって１回転の時間を変更する
				p_wk->work.rota.count = WFLBY_3DOBJCONT_ROTA_ONEROTA_COUNT;
				switch( retway ){
				case WF2DMAP_WAY_RIGHT:
					p_wk->work.rota.count += WFLBY_3DOBJCONT_ROTA_ONESIDEROTA;
				case WF2DMAP_WAY_UP:
					p_wk->work.rota.count += WFLBY_3DOBJCONT_ROTA_ONESIDEROTA;
				case WF2DMAP_WAY_LEFT:
					p_wk->work.rota.count += WFLBY_3DOBJCONT_ROTA_ONESIDEROTA;
				case WF2DMAP_WAY_DOWN:
					break;
				}
				p_wk->seq ++;
			}
		}
		break;
		
	case WFLBY_3DOBJCONT_ROTA_SEQ_ONEROTA:	// さらに１回転
		p_wk->work.rota.count--;
		if( p_wk->work.rota.count == 0 ){
			p_wk->seq ++;

			// 座標を吹き飛んだ先に設定
			{
				WF2DMAP_POS pos;
				u32			retway;
				pos		= WF2DMAP_OBJWkMatrixGet( p_wk->p_obj );
				pos		= WF2DMAP_OBJToolWayPosGet( pos, way );
				retway	= WF2DMPA_OBJToolRetWayGet( way );
				WFLBY_3DOBJCONT_SetWkPosAndWay( p_wk, pos, retway );
			}
			
			// 描画更新をON
			WFLBY_3DOBJWK_EndAnm( p_wk->p_draw );
			WFLBY_3DOBJWK_SetUpdata( p_wk->p_draw, TRUE );
			p_wk->moveend = TRUE;
		}
		break;
		
	case WFLBY_3DOBJCONT_ROTA_SEQ_END:

		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ジャンプ処理
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DOBJCONT_Jump( WFLBY_3DPERSON* p_wk, WFLBY_3DOBJCONT* p_sys )
{
	switch( p_wk->seq ){
	case WFLBY_3DOBJCONT_JUMP_SEQ_INIT:
		WFLBY_3DOBJWK_SetUpdata( p_wk->p_draw, FALSE );
		WFLBY_3DOBJWK_Get3DMatrix( p_wk->p_draw, &p_wk->work.jump.def_mat );
		p_wk->work.jump.count = WFLBY_3DOBJCONT_MOVEOBJ_JUMP_COUNT;
		p_wk->seq ++;
		break;

	case WFLBY_3DOBJCONT_JUMP_SEQ_MAIN:

		// カウント処理
		p_wk->work.jump.count --;
		if( p_wk->work.jump.count < 0 ){
			p_wk->work.jump.count = WFLBY_3DOBJCONT_MOVEOBJ_JUMP_COUNT;
		}
		
		
		// 描画反映処理
		{
			u32 r;
			VecFx32 matrix;
			s16 count;

			// 座標取得
			matrix = p_wk->work.jump.def_mat;

			// カウント値取得
			count = p_wk->work.jump.count - WFLBY_3DOBJCONT_MOVEOBJ_JUMP_WAITCOUNT;

			// count > 0　だとジャンプ中	それ以外は次のジャンプまでのウエイト
			if( count > 0 ){

				count = count % WFLBY_3DOBJCONT_MOVEOBJ_JUMP_EFFCOUNT;

				// SINカーブの角度を取得
				r = (WFLBY_3DOBJCONT_MOVEOBJ_JUMP_RMAX*count) / WFLBY_3DOBJCONT_MOVEOBJ_JUMP_EFFCOUNT;

				// ジャンプ
				matrix.z += FX_Mul( Sin360(r), WFLBY_3DOBJCONT_MOVEOBJ_JUMP_ZDIS*FX32_ONE );
				matrix.y += FX_Mul( Sin360(r), WFLBY_3DOBJCONT_MOVEOBJ_JUMP_DIS*FX32_ONE );
			}

			WFLBY_3DOBJWK_Set3DMatrix( p_wk->p_draw, &matrix );

			// 下を向かせる
			WFLBY_3DOBJWK_SetWay( p_wk->p_draw, WF2DMAP_WAY_DOWN );
		}
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ジャンプ終了処理
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DOBJCONT_JumpEnd( WFLBY_3DPERSON* p_wk, WFLBY_3DOBJCONT* p_sys )
{
	VecFx32 matrix;
	
	WFLBY_3DOBJWK_SetUpdata( p_wk->p_draw, TRUE );

	// 座標取得
	WFLBY_3DOBJWK_Get3DMatrix( p_wk->p_draw, &matrix );
	matrix.y = WFLBY_3DMATRIX_FLOOR_MAT;
	WFLBY_3DOBJWK_Set3DMatrix( p_wk->p_draw, &matrix );
}


//----------------------------------------------------------------------------
/**
 *	@brief	影アルファ値変更システム	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_sys		３Dオブジェ描画システム
 *	@param	alpha		アルファ値
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DOBJCONT_ShadowAlpha_Init( WFLBY_SHADOW_ALPHA* p_wk, WFLBY_3DOBJSYS* p_sys, u32 alpha )
{
	p_wk->now		= alpha;
	p_wk->start		= alpha;
	p_wk->diff		= 0;
	p_wk->count		= 0;
	p_wk->count_max	= 0;
	p_wk->move		= FALSE;
	WFLBY_3DOBJSYS_SetShadowAlpha( p_sys, alpha );
}

//----------------------------------------------------------------------------
/**
 *	@brief	影アルファ値変更システム	開始
 *
 *	@param	p_wk		ワーク
 *	@param	alpha		アルファ値
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DOBJCONT_ShadowAlpha_Start( WFLBY_SHADOW_ALPHA* p_wk, u32 alpha )
{
	p_wk->move		= TRUE;
	p_wk->start		= p_wk->now;
	p_wk->diff		= alpha - p_wk->now;
	p_wk->count		= 0;
	p_wk->count_max	= WFLBY_3DOBJCONT_SHADOW_ALHA_FADE_COUNT;
}

//----------------------------------------------------------------------------
/**
 *	@brief	影アルファ値変更システム	メイン
 *
 *	@param	p_wk		ワーク
 *	@param	p_sys		３Dオブジェ描画システム
 *
 *	@retval	TRUE	アルファ変更完了
 *	@retval	FALSE	アルファ変更中
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_3DOBJCONT_ShadowAlpha_Main( WFLBY_SHADOW_ALPHA* p_wk, WFLBY_3DOBJSYS* p_sys )
{
	if( p_wk->move == FALSE ){
		return TRUE;
	}

	p_wk->count ++;
	if( p_wk->count >= p_wk->count_max ){
		p_wk->move = FALSE;
	}

	p_wk->now = (p_wk->count * p_wk->diff) / p_wk->count_max;
	p_wk->now += p_wk->start;

	WFLBY_3DOBJSYS_SetShadowAlpha( p_sys, p_wk->now );

	return FALSE;
}


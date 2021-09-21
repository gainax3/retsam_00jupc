//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file       bct_cllient.c
 *  @brief		玉投げ	クライアントシステム
 *  @author		tomoya takahashi
 *  @data       2007.06.19
 *
 *
 *  試作の連続なのでめっちゃ定数多いです。
 *  ごめんなさい。
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"
#include "bct_client.h"
#include "bct_snd.h"

#include "gflib/g3d_system.h"

#include "system/wordset.h"
#include "system/msgdata.h"
#include "system/clact_util.h"
#include "system/render_oam.h"
#include "system/window.h"
#include "system/fontproc.h"
#include "system/brightness.h"
#include "system/fontoam.h"
#include "system/font_arc.h"

#include "communication/communication.h"

#include "application/wifi_lobby/minigame_tool.h"

#include "src/graphic/bucket.naix"
#include "src/graphic/wlmngm_tool.naix"
#include "system/d3dobj.h"

#include "gflib/calctool.h"

#include "msgdata/msg.naix"
#include "msgdata/msg_debug_tomoya.h"
#include "msgdata/msg_lobby_minigame1.h"
#include "system/snd_tool.h"

//-----------------------------------------------------------------------------
/**
 *                  コーディング規約
 *      ●関数名
 *              １文字目は大文字それ以降は小文字にする
 *      ●変数名
 *              ・変数共通
 *                      constには c_ を付ける
 *                      staticには s_ を付ける
 *                      ポインタには p_ を付ける
 *                      全て合わさると csp_ となる
 *              ・グローバル変数
 *                      １文字目は大文字
 *              ・関数内変数
 *                      小文字と”＿”と数字を使用する 関数の引数もこれと同じ
*/
//-----------------------------------------------------------------------------

#ifdef BCT_DEBUG
static s16  BCT_DEBUG_NutsSetNum = 0;
static s16  BCT_DEBUG_NutsDrawNum = 0;
#endif


//-----------------------------------------------------------------------------
/**
 *                  定数宣言
*/
//-----------------------------------------------------------------------------
#define BCT_NUTSBUFF_NUM    ( 96 )  // 送信用木の実データバッファ数
#define BCT_NUTSBUFFOAM_NUM ( 96 )  // 実描画バッファ数
#define BCT_MAINOAM_CONTID  ( 200 )
#define BCT_MAINOAM_BGPRI  ( 0 )
#define BCT_STRBUF_NUM      ( 128 ) // STRBUFを作るときのバッファ数
#define BCT_FONTOAM_WKNUM	( 8 )	// FONTOAMワーク数

#define BCT_COL_N_BLACK ( GF_PRINTCOLOR_MAKE( 1, 2, 15 ) )      // フォントカラー：黒
#define BCT_COL_N_WHITE ( GF_PRINTCOLOR_MAKE( 15, 14, 15 ) )        // フォントカラー：白
#define BCT_COL_N_RED       ( GF_PRINTCOLOR_MAKE( 3, 4, 15 ) )      // フォントカラー：青
#define BCT_COL_N_BLUE      ( GF_PRINTCOLOR_MAKE( 5, 6, 15 ) )      // フォントカラー：赤
#define BCT_COL_N_GRAY      ( GF_PRINTCOLOR_MAKE( 2, 14, 15 ) )     // フォントカラー：灰

#define BCT_COL_OAM_BLACK     ( GF_PRINTCOLOR_MAKE( 15, 14, 0 ) )     // フォントカラー：黒


//-------------------------------------
/// 木の実動作シーケンス
//=====================================
enum {
    BCT_NUTSSEQ_NONE,
    BCT_NUTSSEQ_MOVE,       // 動作
    BCT_NUTSSEQ_MOVEOTHER,  // 他人の木の実として動作
	BCT_NUTSSEQ_INMOVE,		// 入ってからの動作
    BCT_NUTSSEQ_END,        // 送信待ち
    BCT_NUTSSEQ_MOVEAWAY,   // 場外へ
    BCT_NUTSSEQ_NUM,
} ;

//-------------------------------------
/// 開始時
//=====================================
enum{
    BCT_STARTSEQ_TOUCHPEN_START,   // タッチペン開始
    BCT_STARTSEQ_TOUCHPEN_MAIN,   // タッチペンメイン
    BCT_STARTSEQ_MARUNOMU_SND,   // マルノームが上ってくる待ち
    BCT_STARTSEQ_MARUNOMU_OPEN, // マルノームの口が開く待ち
    BCT_STARTSEQ_WAIT,          // カウントダウン開始待ち
    BCT_STARTSEQ_TEXTINIT,      // テキストの準備
    BCT_STARTSEQ_COUNTDOWNWAIT, // カウントダウン待ち
    BCT_STARTSEQ_GAMESTARTWAIT, // ゲームが出来る状態になるまで待つ
    BCT_STARTSEQ_END,           // 開始終わり
};
#define BCT_START_MARU_MOUTH_OPEN_WAIT	( 65 )
  

//-------------------------------------
///	ゲーム内状態
//=====================================
typedef enum {
    BCT_MARUNOMU_MOVE_EASY,			// 簡単動作
    BCT_MARUNOMU_MOVE_FEVER_EFF,	// FEVER前エフェクト
    BCT_MARUNOMU_MOVE_FEVER,		// FEVER
    BCT_MARUNOMU_MOVE_NUM,			// タイプ数
} BCT_MARUNOMU_MOVE_TYPE;
// 動作開始時間
static const u32 sc_BCT_MARUNOMU_MOVE_STARTTIME[ BCT_MARUNOMU_MOVE_NUM ] = {
	0 * 30,
	(22 * 30),
	(25 * 30),
};



//-------------------------------------
/// マルノーム
//=====================================
enum{   // マルノームモデルオブジェ
	BCT_MARUNOMU_MDL_NORMAL,	// 通常
	BCT_MARUNOMU_MDL_OPEN,		// オープンアニメ用
	BCT_MARUNOMU_MDL_FEVER,		// フィーバーアニメ用
    BCT_MARUNOMU_MDL_NUM,
};
enum{   // マルノームアニメオブジェ
    BCT_MARUNOMU_ANM_WALK,  // 歩きアニメ
    BCT_MARUNOMU_ANM_ROTA,  // 回転アニメ
    BCT_MARUNOMU_ANM_PURU,  // 口プルプルアニメ
    BCT_MARUNOMU_ANM_FEV,	// FEVER前アニメ
    BCT_MARUNOMU_ANM_OPEN,  // 口開きアニメ
    BCT_MARUNOMU_ANM_CLOSE, // 口閉じアニメ
    BCT_MARUNOMU_ANM_NUM,
};
enum{
	BCT_MARUNOMU_ANM_COLANM_NORMAL,		//　通常モデル用カラーアニメ（BCT_MARUNOMU_MDL_NORMAL用）
//	BCT_MARUNOMU_ANM_COLANM_FEVER,		//　フィーバーエフェクトモデル用カラーアニメ（BCT_MARUNOMU_MDL_FEVER用）
	BCT_MARUNOMU_ANM_COLANM_NUM,
};
enum{	// 色香絵アニメフレーム管理
	BCT_MARUNOMU_ANM_COL_NORMAL,
	BCT_MARUNOMU_ANM_COL_1P,
	BCT_MARUNOMU_ANM_COL_2P,
	BCT_MARUNOMU_ANM_COL_3P,
	BCT_MARUNOMU_ANM_COL_4P,
	BCT_MARUNOMU_ANM_COL_NUM,

	BCT_MARUNOMU_ANM_COL_DATA_START = 0,	// 開始フレーム
	BCT_MARUNOMU_ANM_COL_DATA_END,			// 終了フレーム
	BCT_MARUNOMU_ANM_COL_DATA_NUM,
};
// カラーフレーム　開始終了フレーム
static const u8 sc_BCT_MARUNOMU_ANM_COL_DATA[BCT_MARUNOMU_ANM_COL_NUM][BCT_MARUNOMU_ANM_COL_DATA_NUM] = {
	// BCT_MARUNOMU_ANM_COL_NORMAL,
	{  0,  0},
	// BCT_MARUNOMU_ANM_COL_1P,
	{  1, 10 },
	// BCT_MARUNOMU_ANM_COL_2P,
	{ 11, 20 },
	// BCT_MARUNOMU_ANM_COL_3P,
	{ 21, 30 },
	// BCT_MARUNOMU_ANM_COL_4P,
	{ 31, 40 },
};
#define BCT_MARUNOMU_ANM_COL_1PSTART	( sc_BCT_MARUNOMU_ANM_COL_DATA[BCT_MARUNOMU_ANM_COL_1P][BCT_MARUNOMU_ANM_COL_DATA_START]*FX32_ONE )
#define BCT_MARUNOMU_ANM_COL_END		( sc_BCT_MARUNOMU_ANM_COL_DATA[BCT_MARUNOMU_ANM_COL_4P][BCT_MARUNOMU_ANM_COL_DATA_END]*FX32_ONE )

// アニメのノード名
#define BCT_MARUNOMU_ANM_NODE_NUM	( 15 )
static const NNSG3dResName sc_AnmNodeName[ BCT_MARUNOMU_ANM_NUM ][ BCT_MARUNOMU_ANM_NODE_NUM ] = {
	{	// BCT_MARUNOMU_ANM_WALK
		"polySurface2",
		"polySurface3",
		"pTorus2",
		"null1",
	},

	{	// BCT_MARUNOMU_ANM_ROTA
		"polySurface7",
		"polySurface13",
		"pSphere2",
		"pSphere3",
		"pSphere6",
		"pSphere7",
	},

	{	// BCT_MARUNOMU_ANM_PURU
		"polySurface13",
		"pSphere7",
	},

	{	// BCT_MARUNOMU_ANM_FEV
		"null1",
		"pCylinder4",
		"pCylinder5",
		"pSphere2",
		"pSphere3",
		"pSphere6",
		"pSphere7",
		"pTorus1",
		"pTorus2",
		"polySurface13",
		"polySurface8", 
		"polySurface2",
		"polySurface3",
		"polySurface7",
		"polySurface12",
	},

	{	// BCT_MARUNOMU_ANM_OPEN
		"polySurface7",
		"polySurface13",
		"pSphere2",
		"pSphere3",
		"pSphere6",
		"pSphere7",
		"null1",
	},

	{	// BCT_MARUNOMU_ANM_CLOSE
		"polySurface7",
		"polySurface13",
		"pSphere2",
		"pSphere3",
		"pSphere6",
		"pSphere7",
	},
};
// アニメとモデルの対応表 
static const u8 sc_BCT_MARUNOMU_ANM_MDL[ BCT_MARUNOMU_ANM_NUM ] = {
	BCT_MARUNOMU_MDL_NORMAL,
	BCT_MARUNOMU_MDL_NORMAL,
	BCT_MARUNOMU_MDL_NORMAL,
	BCT_MARUNOMU_MDL_FEVER,
	BCT_MARUNOMU_MDL_OPEN,
	BCT_MARUNOMU_MDL_NORMAL,
};

// カラーアニメとモデルの対応表 
static const u8 sc_BCT_MARUNOMU_COLANM_MDL[ BCT_MARUNOMU_ANM_COLANM_NUM ] = {
	BCT_MARUNOMU_MDL_NORMAL,
//	BCT_MARUNOMU_MDL_FEVER,
};
#define BCT_MARUNOMU_ANM_FRAME_MAX		( FX32_CONST(180) )	// マルノームアニメ最大フレーム
#define BCT_MARUNOMU_ANM_SPEED		( FX32_CONST(2) )	// アニメスピード
#define BCT_MARUNOMU_ANM_STARTSPEED	( FX32_CONST(1.0) )	// アニメスピード
#define BCT_MARUNOMU_ANM_FEVERSPEED	( FX32_CONST(1.0) )	// アニメスピード
#define BCT_MARUNOMU_ANM_SLOWSPEED	( FX32_CONST(1) )	// アニメスピード
// マルノーム当たり判定データ
#define BCT_FIELD_MARUNOMU_X    ( FX32_CONST(0) )   // マルノームがいる場所
#define BCT_FIELD_MARUNOMU_Z    ( FX32_CONST(0) )  // マルノームがいる場所
#define BCT_FIELD_MARUNOMU_Y    ( FX32_CONST(-90) )   // マルノームがいる場所
#define BCT_FIELD_MIN			( FX32_CONST(-50) )	// 床の位置
#define BCT_FIELD_Y_DEL			( FX32_CONST(-100) )	// 破棄する床の位置

// 床の描画位置
#define BCT_FIELD_YUKA_DRAW_Y	( BCT_FIELD_MARUNOMU_Y - FX32_CONST(8) )

#define BCT_SHADOW_Y_FLOOR		( BCT_FIELD_MIN - FX32_CONST( 1 ) )
#define BCT_SHADOW_Y_DEL		( FX32_CONST( -300 ) )
#define BCT_SHADOW_Y_SIZE_DIF	( FX32_CONST( 300 ) )	// 木の実の高さ最大
#define BCT_SHADOW_SIZE_MIN		( FX32_CONST( 1 ) )		// 陰のサイズ
#define BCT_SHADOW_SIZE_DIV		( FX32_CONST( 1 ) )		// 陰のサイズ
#define BCT_SHADOW_ALPHA_MIN	( 8 )	// 最小
#define BCT_SHADOW_ALPHA_DIV	( 16 )	// 差

static const VecFx32 sc_MARUNOMU_MAT = { BCT_FIELD_MARUNOMU_X, BCT_FIELD_MARUNOMU_Y, BCT_FIELD_MARUNOMU_Z };

#define BCT_MARUNOMU_HIT_DIS_DIV    ( FX32_CONST(30) )  // マルノームの口の大きさ変動値
#define BCT_MARUNOMU_HIT_DIS_MIN    ( FX32_CONST(50) )   // マルノームの口の大きさ最小
#define BCT_MARUNOMU_HIT_DIS_MAX    ( BCT_MARUNOMU_HIT_DIS_MIN+BCT_MARUNOMU_HIT_DIS_DIV )  // マルノームの口の大きさ最大
#define BCT_MARUNOMU_SCALE_INIT		( 2500 )	// マルノームの大きさ開始
#define BCT_MARUNOMU_SCALE_END		( 4516 )	// マルノームの大きさ終了
#define BCT_MARUNOMU_INRYOKU_DIST   ( FX32_CONST(200) ) // マルノームに向かって働く引力
#define BCT_MARUNOMU_INRYOKU_POWER_PAR  ( FX32_CONST(0.03) )    // 距離のこれ分引力を働かせる
static const VecFx32 BCT_MARUNOMU_HIT_PLANE_N = { 0, FX32_CONST(2500), 0 }; // マルノームの口当たり判定の法線ベクトル
#define BCT_MARUNOMU_HIT_X	( 0 )	// 当たり判定ｘ座標
#define BCT_MARUNOMU_HIT_Y	( FX32_CONST(55) )	// 当たり判定ｙ座標
#define BCT_MARUNOMU_HITBODY_Y_MIN	( FX32_CONST(150) )	// マルノーム体あたり判定開始
#define BCT_MARUNOMU_HITBODY_Y_DIF	( FX32_CONST(60) )	// 角度が０～９０になる間に変わる高さ
#define BCT_MARUNOMU_HITBODY_R		( FX32_CONST(80) )	// 当たり判定半径
#define BCT_MARUNOMU_HITBODY_R_MAX	( FX32_CONST(120) )	// 当たり判定半径
#define BCT_MARUNOMU_HITBODY_R_DIS	( BCT_MARUNOMU_HITBODY_R_MAX - BCT_MARUNOMU_HITBODY_R )
enum{
    BCT_MARUNOMU_MOUTH_HIT_NONE,    // まだ口までいっていない
    BCT_MARUNOMU_MOUTH_HIT_OK,  // 入った！
    BCT_MARUNOMU_MOUTH_HIT_NG,  // はずれた！
    BCT_MARUNOMU_MOUTH_HIT_NEAR,// 引力発生！
} ;
#define BCT_MARUNOMU_SCALE      (FX32_CONST(1.50f))
#define BCT_MARUNOMU_ROTA_X_MIN			( FX_GET_ROTA_NUM( 20 ) )	// 傾斜最小値
#define BCT_MARUNOMU_ROTA_X_INIT        ( FX_GET_ROTA_NUM( 20 ) )    // マルノームの口の傾き
#define BCT_MARUNOMU_ROTA_X_ADD         ( FX_GET_ROTA_NUM( 30 ) )   // マルノームの口の傾き
#define BCT_MARUNOMU_ROTA_X_MAX_360		( 90 )	// 最大傾斜
#define BCT_MARUNOMU_ROTA_X_MAX			( FX_GET_ROTA_NUM( BCT_MARUNOMU_ROTA_X_MAX_360 ) )	// 最大傾斜
#define BCT_MARUNOMU_ROTA_X_DIF			( BCT_MARUNOMU_ROTA_X_MAX - BCT_MARUNOMU_ROTA_X_MIN )	// MINとMAXの差
#define BCT_MARUNOMU_ROTA_X_ADD_SUB ( FX_GET_ROTA_NUM( 8 ) )    // 補正地
#define BCT_MARUNOMU_ROTA_INIT			( FX_GET_ROTA_NUM( 180 ) )	// Y軸回転初期値
#define BCT_MARUNOMU_ROTA_SPEED_INIT    ( 128 )
#define BCT_MARUNOMU_ROTA_SPEED_ADD     ( 360 ) // INITに足す最大値
#define BCT_MARUNOMU_ROTA_SPEED_ADD_SUB ( 96 )  // 補正地
#define BCT_MARUNOMU_DRAWROTA_ADD       ( FX_GET_ROTA_NUM( 0 ) )  // 表示で補正する回転角
#define BCT_MARUNOMU_ROTACOUNT_ONE		( 1000 )
#define BCT_MARUNOMU_ROTACOUNT_GETNUM(x) ((x)*BCT_MARUNOMU_ROTACOUNT_ONE)

// マルノームバクバク動作
#define BCT_MARUNOMU_BAKUBAKU_ONE_SYNC	( 50 )	// １回開いて閉じる間隔

// 得点データ
#define BCT_MARUNOMU_SCORE_TYPENUM  (3) // スコアのタイプ
static const u32 BCT_SCORE_NUM[ BCT_MARUNOMU_SCORE_TYPENUM ] = {
    100, 200, 300
};
#define BCT_MARUNOMU_SCORE_FEVER_MUL	(3)	// FEVERタイムの時にかける値

// フィールドデータ
#define BCT_FIELD_ZDIS  ( FX32_CONST(600) )     // フィールドの大きさ
#define BCT_FIELD_XDIS  ( FX32_CONST(600) )     // フィールドの大きさ
#define BCT_FIELD_ZDISHALF  ( BCT_FIELD_ZDIS/2 )    // フィールドの大きさ
#define BCT_FIELD_XDISHALF  ( BCT_FIELD_XDIS/2 )    // フィールドの大きさ
#define BCT_FIELD_PLAYER_X  ( FX32_CONST(0) )   // プレイヤーのいる場所
#define BCT_FIELD_PLAYER_Z  ( FX32_CONST(-300) )    // プレイヤーのいる場所
#define BCT_FIELD_PLAYER_Y  ( FX32_CONST(0) )   // プレイヤーのいる場所
#define BCT_FIELD_DOT256TOMAT   ( FX32_CONST(150) ) // DOT座標の256を３Dの～とするか
#define BCT_FIELD_DOT256MATHALF (BCT_FIELD_DOT256TOMAT / 2)
#define BCT_FIELD_DOT2MAT(x)        ( ((x)*(BCT_FIELD_DOT256TOMAT/256))-BCT_FIELD_DOT256MATHALF )   // DS1dotの３D座標
#define BCT_FIELD_MAT2DOT(x)        ( ((x)+BCT_FIELD_DOT256MATHALF)/(BCT_FIELD_DOT256TOMAT/256) )   // ３D座標のDS1dot座標
#define BCT_NUTS_COMM_SCALE     ( FX32_CONST(0.16) )    // 通信データにするときの数値の圧縮率
static const u16 BCT_NUTS_FIELD_NetIDRota[ BCT_PLAYER_NUM ][ BCT_PLAYER_NUM ] = {
    {   // 1人
        0,
        0,0,0
    },
    {   // 2人
        0,
        FX_GET_ROTA_NUM( 180 ),
        0,0
    },
    {   // 3人
        0,
        FX_GET_ROTA_NUM( 120 ),
        FX_GET_ROTA_NUM( 240 ),
        0
    },
    {   // 4人
        0,
        FX_GET_ROTA_NUM( 180 ),
        FX_GET_ROTA_NUM( 90 ),
        FX_GET_ROTA_NUM( 270 ),
    }
};

// カメラデータ
#define BCT_CAMERA_TARGET_X ( 0 )
#define BCT_CAMERA_TARGET_Y ( 0 )
#define BCT_CAMERA_TARGET_Z ( 0 )
#define BCT_CAMERA_ANGLE_X  ( 0xf112 )
#define BCT_CAMERA_ANGLE_Z  ( FX_GET_ROTA_NUM( 0 ) )
#define BCT_CAMERA_DISTANCE ( 0x1d9000 )
#define BCT_CAMERA_PEARCE   ( FX_GET_ROTA_NUM( 22 ) )
#define BCT_CAMERA_UP       ( FX32_ONE )
#define BCT_CAMERA_NEAR		( FX32_CONST(200) )
#define BCT_CAMERA_FAR		( FX32_CONST(1000) )
static const u16 BCT_CAMERA_ANGLEY_NetID[ BCT_PLAYER_NUM ][ BCT_PLAYER_NUM ] = {
    {   // 1人
        0,0,0,0,
	},
    {   // 2人
        FX_GET_ROTA_NUM( 180 ),
        0,
        0,0,
    },
    {   // 3人
        FX_GET_ROTA_NUM( 180 ),
        FX_GET_ROTA_NUM( 300 ),
        FX_GET_ROTA_NUM( 60 ),
        0,
    },
    {   // 4人
        FX_GET_ROTA_NUM( 180 ),
        0,
        FX_GET_ROTA_NUM( 270 ),
        FX_GET_ROTA_NUM( 90 ),
    },
};

// 木の実動作データ
#define BCT_NUTS_POWER_MIN  ( 0x8000 )    // パワー最小
#define BCT_NUTS_POWER_DIS  ( 0x16200 )    // パワー最大と最小の差
#define BCT_NUTS_POWER_MAX  ( BCT_NUTS_POWER_MIN+BCT_NUTS_POWER_DIS )// パワー最大

#define BCT_NUTS_POWER_RES  ( 0x670 )   // 空気抵抗
#define BCT_NUTS_POWER_G    ( 0xffffeccc )  // 重力
#define BCT_NUTS_POWRT_YPOWER ( 0x26000 )   // Y方向の力はXZ方向の何倍か
#define BCT_NUTS_POWER_MUL  ( FX32_CONST(0.6) )     // パワーを大きくする倍率
#define BCT_NUTS_AWAYPOWER_MUL	( FX32_CONST( 0.850f ) )	// 吹っ飛びパワーにかける値
#define BCT_NUTS_AWAYPOWERFIELD_MUL	( FX32_CONST( 0.80f ) )	// 地面吹っ飛びパワーにかける値
#define BCT_NUTS_AWAYWAY_MUL	( FX32_CONST( 1.0f ) )	// 吹っ飛ぶ方向を派手にするためにかけるあたい
#define BCT_NUTS_AWAYPOWER_DITCHMUL	( FX32_CONST( 0.250f ) )	// 吹っ飛ぶ方向を派手にするためにかけるあたい
#define BCT_NUTS_AWAYDEL_POWER	( 0x1000 )	// ボールを破棄するパワー
#define BCT_NUTS_AWAYDEL_POWER2	( 0x2000 )	// ボールを破棄するパワー

#define BCT_NUTS_AWAY_DITCH_MIN_R	( FX32_CONST( 188 ) )		// 溝の半径エリアデータ
#define BCT_NUTS_AWAY_DITCH_MAX_R	( FX32_CONST( 240 ) )
#define BCT_NUTS_AWAY_DITCH_DIV_R	( BCT_NUTS_AWAY_DITCH_MAX_R - BCT_NUTS_AWAY_DITCH_MIN_R )
#define BCT_NUTS_AWAY_BLOCK_R		( FX32_CONST( 240 ) )
#define BCT_NUTS_AWAY_MARUNOMU_R	( FX32_CONST( 120 ) )
#define BCT_NUTS_INMOVE_COUNT	( 8 )	// 入った後に動かすかウント数

#define BCT_NUTSDRAW_ROTA_MIN	( 512 )	// 木の実回転スピード値最低
#define BCT_NUTSDRAW_ROTA_MAX	( 1024 )	// 木の実回転スピード値最大
#define BCT_NUTSDRAW_ROTA_DIS	( BCT_NUTSDRAW_ROTA_MAX - BCT_NUTSDRAW_ROTA_MIN )
#define BCT_NUTSDRAW_ROTA_CHG_TIMING	( 8 )	// 回転スピードを変えるタイミング
#define BCT_NUTS_BONUS_COUNT_MAX	( 99 )	// ボーナスカウントの最大


// 木の実リソース
typedef enum{
	BCT_NUTSRES_MDLPLNO00,
	BCT_NUTSRES_MDLPLNO01,
	BCT_NUTSRES_MDLPLNO02,
	BCT_NUTSRES_MDLPLNO03,
	BCT_NUTSRES_MDLSPECIAL,
	BCT_NUTSRES_MDLNUM,
} BCT_NUTSRES_MDLTYPE;

// スロー
#define BCT_NUTS_SLOW_Q ( 8 )   // 投げ軌道キューのキューの数
#define BCT_NUTS_SLOW_USE_VECNUM    ( 2 )   // 必要な頂点の数
#define BCT_NUTS_SLOW_TOUCH_AREA_CENTER_X	( 128 )	// 木の実を拾えるエリアの中心座標
#define BCT_NUTS_SLOW_TOUCH_AREA_CENTER_Y	( -96 )	// 中心
#define BCT_NUTS_SLOW_TOUCH_AREA_R			( 190-BCT_NUTS_SLOW_TOUCH_AREA_CENTER_Y )	// 半径
#define BCT_NUTS_SLOW_RENSYA_TIME	( 2 )	// 最小次発射待ち時間

// OAM木の実画面外エフェクト
#define BCT_OAMAWAYNUTS_BUFFNUM	( 8 )	// OAM木の実画面外エフェクトバッファ数
#define BCT_OAMAWAYNUTS_DEL_XMIN (-FX32_CONST(32))	// エフェクトを破棄する座標
#define BCT_OAMAWAYNUTS_DEL_XMAX (FX32_CONST(288))	// エフェクトを破棄する座標
#define BCT_OAMAWAYNUTS_DEL_YMIN (-FX32_CONST(32))	// エフェクトを破棄する座標
#define BCT_OAMAWAYNUTS_DEL_YMAX (FX32_CONST(224))	// エフェクトを破棄する座標
#define BCT_OAMAWAYNUTS_MOVE0_COUNT	( 16 )	// 1バウンド目のカウント
#define BCT_OAMAWAYNUTS_MOVE0_SPEED	( 8 )	// 1バウンド目のスピード
#define BCT_OAMAWAYNUTS_MOVE0_SROTA	( 190 )	// 左基準の開始回転角度
#define BCT_OAMAWAYNUTS_MOVE0_EROTA	( 125 )	// 左基準の開始回転角度
#define BCT_OAMAWAYNUTS_MOVE1_COUNT	( 10 )	// 1バウンド目のカウント
#define BCT_OAMAWAYNUTS_MOVE1_SPEED	( 4 )	// 1バウンド目のスピード
#define BCT_OAMAWAYNUTS_MOVE1_SROTA	( 182 )	// 左基準の開始回転角度
#define BCT_OAMAWAYNUTS_MOVE1_EROTA	( 140 )	// 左基準の開始回転角度
#define BCT_OAMAWAYNUTS_MOVE2_COUNT	( 6 )	// 1バウンド目のカウント
#define BCT_OAMAWAYNUTS_MOVE2_SPEED	( 3 )	// 1バウンド目のスピード
#define BCT_OAMAWAYNUTS_MOVE2_SROTA	( 180 )	// 左基準の開始回転角度
#define BCT_OAMAWAYNUTS_MOVE2_EROTA	( 160 )	// 左基準の開始回転角度
#define BCT_OAMAWAYNUTS_SCALE	( 9 )	// OAMちょっと小さくする
#define BCT_OAMAWAYNUTS_SCALE_DIV ( 3 )	// OAM鶴首区変化地
#define BCT_OAMAWATNUTS_MOVE_COUNT	( BCT_OAMAWAYNUTS_MOVE0_COUNT+BCT_OAMAWAYNUTS_MOVE1_COUNT+BCT_OAMAWAYNUTS_MOVE2_COUNT )	// 全部のカウンタ



// 描画データ
#define BCT_GRA_CHARMAN_NUM ( 16 )  // キャラクタマネージャ管理数
#define BCT_GRA_PLTTMAN_NUM ( 16 )  // パレットマネージャ管理数
#define BCT_GRA_OAMVRAM_MAIN    ( 128*1024 )    // メインOAMVRAM
#define BCT_GRA_OAMVRAM_SUB     ( 16*1024 ) // サブOAMVRAM
#define BCT_GRA_CLACTNUM        ( 128 ) // セルアクターワーク数
#define BCT_GRA_RESMAN_NUM      ( 4 )   // 管理するリソース種類数
#define BCT_GRA_RESMAN_LOADNUM  ( 16 )  // リソース管理数
#define BCT_GRA_OAMSUBSURFACE_Y ( 512<<FX32_SHIFT )

#define BCT_GRA_NUTS_OAM_RESID  ( 100 ) // 木の実OAM管理ID
#define BCT_GRA_NUTS_OAM_PRI    ( 32 )
#define BCT_GRA_NUTS_OAM_BGPRI  ( 0 )

#define BCT_GRA_SUBBRIGHT_OFFNUM    (-14) // サブ面をOFFしているときのブライトネス値

enum{   // MAINパレット
    BCT_GRA_BGMAIN_PAL_NAME_PL00,
    BCT_GRA_BGMAIN_PAL_NAME_PL01,
    BCT_GRA_BGMAIN_PAL_NAME_PL02,
    BCT_GRA_BGMAIN_PAL_NAME_PL03,
    BCT_GRA_BGMAIN_PAL_SYSWND,
    BCT_GRA_BGMAIN_PAL_FONT,
};
enum{
	BCT_GRA_BGSUB_PAL_BACK_NETID0,
	BCT_GRA_BGSUB_PAL_NETID0_TOP,
	BCT_GRA_BGSUB_PAL_NETID0_BACK,
	BCT_GRA_BGSUB_PAL_NETID1_TOP,
	BCT_GRA_BGSUB_PAL_NETID1_BACK,
	BCT_GRA_BGSUB_PAL_NETID2_TOP,
	BCT_GRA_BGSUB_PAL_NETID2_BACK,
	BCT_GRA_BGSUB_PAL_NETID3_TOP,
	BCT_GRA_BGSUB_PAL_NETID3_BACK,
	BCT_GRA_BGSUB_PAL_BACK_NETID1,
	BCT_GRA_BGSUB_PAL_BACK_NETID2,
	BCT_GRA_BGSUB_PAL_BACK_NETID3,
	BCT_GRA_BGSUB_PAL_NUM,
};
enum{   // MAIN面OAMアニメシーケンス
    BCT_GRA_OAMMAIN_ANM_COUNTDOWN,  // カウントアニメ
    BCT_GRA_OAMMAIN_ANM_END,        // 終了 アニメ
    BCT_GRA_OAMMAIN_ANM_100,        // 100 アニメ
    BCT_GRA_OAMMAIN_ANM_200,        // 200 アニメ
    BCT_GRA_OAMMAIN_ANM_300,        // 300 アニメ
    BCT_GRA_OAMMAIN_ANM_FE100,        // FEVER300 アニメ
    BCT_GRA_OAMMAIN_ANM_FE200,        // FEVER600 アニメ
    BCT_GRA_OAMMAIN_ANM_FE300,        // FEVER900 アニメ

    BCT_GRA_OAMMAIN_ANM_100_OTHER,		// 相手用100 アニメ
    BCT_GRA_OAMMAIN_ANM_200_OTHER,		// 相手用200 アニメ
    BCT_GRA_OAMMAIN_ANM_300_OTHER,		// 相手用300 アニメ
    BCT_GRA_OAMMAIN_ANM_FE100_OTHER,	// 相手用FEVER300 アニメ
    BCT_GRA_OAMMAIN_ANM_FE200_OTHER,	// 相手用FEVER600 アニメ
    BCT_GRA_OAMMAIN_ANM_FE300_OTHER,	// 相手用FEVER900 アニメ
};
enum{	// MAIN面OAMパレット
	BCT_GRA_OAMMAIN_PAL_BACK,
	BCT_GRA_OAMMAIN_PAL_BACK00,
	BCT_GRA_OAMMAIN_PAL_BACK01,
	BCT_GRA_OAMMAIN_PAL_SCORE0,
	BCT_GRA_OAMMAIN_PAL_SCORE1,
	BCT_GRA_OAMMAIN_PAL_SCORE2,
	BCT_GRA_OAMMAIN_PAL_SCORE3,
	BCT_GRA_OAMMAIN_PAL_SCOREEX,
	BCT_GRA_OAMMAIN_PAL_NUM,
};
// メイン画面メニューウィンドウ
#define BCT_GRA_SYSWND_CGX      ( 1 )
#define BCT_GRA_SYSWND_CGXEND   ( BCT_GRA_SYSWND_CGX+MENU_WIN_CGX_SIZ )

// 開始画面
// 開始画面ウィンドウ
#define BCT_GRA_STARTWIN_X      ( 2 )
#define BCT_GRA_STARTWIN_Y      ( 19 )
#define BCT_GRA_STARTWIN_SIZX   ( 28 )
#define BCT_GRA_STARTWIN_SIZY   ( 4 )
#define BCT_GRA_STARTWIN_CGX    ( BCT_GRA_SYSWND_CGXEND )
#define BCT_GRA_STARTWIN_MSGX   ( 0 )
#define BCT_GRA_STARTWIN_MSGY   ( 0 )
// マルノーム
#define BCT_START_MARUNOMUROTA  ( 120 )
#define BCT_START_MARUNOMU_OPEN_COUNT   ( 24 )  // 開始時口をあけるのに掛ける時間
// OAM
#define BCT_START_COUNTDOWN_X   ( 128 ) // 配置X座標
#define BCT_START_COUNTDOWN_Y   ( 96 )  // 配置Y座標
#define BCT_START_COUNTDOWN_STARTFRAME	( 4 )
// スクロール
#define BCT_START_SCRLL3D_COUNT	( 64 )	// 動作カウンタ
#define BCT_START_SCRLL3D_Y_S		( -FX32_CONST( 200 ) )	// マルノームを動かす距離
#define BCT_START_SCRLL3D_Y_E		( FX32_CONST( 0 ) )		// マルノームを動かす距離
#define BCT_START_SCRLL3D_Y_A		( FX32_CONST( 2 ) )	// マルノームを動かす距離
//ブライトネス
#define BCT_START_BRIGHTNESS_COUNT	( 8 )	// 動作カウンタ
// 名前表示
// 名前フレームサイズ
#define BCT_START_NAME_FRAMESIZ_X	( 10 )
#define BCT_START_NAME_FRAMESIZ_Y	( 4 )
// 名前ビットマップサイズ
#define BCT_START_NAME_BMP_WINSIZ_X			( 8 )
#define BCT_START_NAME_BMP_WINSIZ_Y			( 2 )
#define BCT_START_NAME_BMP_WINCGX_START		( 256 )
#define BCT_START_NAME_BMP_WINCGX_ONENUM	( BCT_START_NAME_BMP_WINSIZ_X*BCT_START_NAME_BMP_WINSIZ_Y )
#define BCT_START_NAME_STRBUF_NUM			( 128 )
// キャラクタ単位　位置
//											// plno				commnum			 draw plno
static const Vec2DS32 sc_BCT_START_NAME_TBL[ BCT_PLAYER_NUM ][ BCT_PLAYER_NUM ][ BCT_PLAYER_NUM ] = {
	// 自分のPLNOが0
	{	
		// 自分だけのときはない
		{0},
		// 2人対戦
		{
			{0,0},
			{12,2},
		},
		// 3人対戦
		{
			{0,0},
			{23,4},
			{1,4},
		},
		// 4人対戦
		{
			{0,0},
			{12,2},
			{23,6},
			{1,6},
		},
	},

	// 自分のPLNOが1
	{	
		// 自分だけのときはない
		{0},
		// 2人対戦
		{
			{12,2},
			{0,0},
		},
		// 3人対戦
		{
			{1,4},
			{0,0},
			{23,4},
		},
		// 4人対戦
		{
			{12,2},
			{0,0},
			{1,6},
			{23,6},
		},
	},

	// 自分のPLNOが2
	{	
		// 自分だけのときはない
		{0},
		// 2人なし
		{
			{0},
		},
		// 3人対戦
		{
			{23,4},
			{1,4},
			{0,0},
		},
		// 4人対戦
		{
			{1,6},
			{23,6},
			{0,0},
			{12,2},
		},
	},

	// 自分のPLNOが3
	{	
		// 自分だけのときはない
		{0},
		// 2人なし
		{
			{0},
		},
		// 3人なし
		{
			{0,0},
		},
		// 4人対戦
		{
			{23,6},
			{1,6},
			{12,2},
			{0,0},
		},
	},
};

// 結果画面
enum{
    BCT_RESULT_SEQ_ENDINIT,
    BCT_RESULT_SEQ_ENDDRAW,
    BCT_RESULT_SEQ_ENDWAIT,
    BCT_RESULT_SEQ_END,
};
enum{
    BCT_RESULT_OAM_TIMEUP,
    BCT_RESULT_OAM_NUM,
};
#define BCT_RESULT_ENDOAM_X ( FX32_CONST(128) ) // オワリOAM表示座標
#define BCT_RESULT_ENDOAM_Y ( FX32_CONST(128) )
#define BCT_RESULT_MARUNOMU_MOVE_TIMING ( 60 )
#define BCT_RESULT_MARUNOMU_CLOSE_TIME  ( 24 )
#define BCT_RESULT_ENDBRIGHTNESS_SYNC	( 24 )
#define BCT_RESULT_ENDWAIT	( 64 )

// 背景
enum{
	BCT_MAINBACK_MDL_WALL_N,
	BCT_MAINBACK_MDL_WALL_F,
	BCT_MAINBACK_MDL_STAGE,
	BCT_MAINBACK_MDL_NUM,
};
#define BCT_MAINBACK_SCALE	( FX32_CONST(1.50f) )
#define BCT_MAINBACK_ROT	( RotKey( 180 ) )
enum{
	BCT_MAINBACK_ANM_WALL_N,
	BCT_MAINBACK_ANM_WALL_F,
	BCT_MAINBACK_ANM_WALL_F_TP,
	BCT_MAINBACK_ANM_NUM,
};
#define BCT_MAINBACK_FEVER_ANM_FRAME_NUM	(FX32_CONST(100))
#define BCT_MAINBACK_FEVER_ANM_FRAME_LOOPS	(FX32_CONST(51))
#define BCT_MARUNOMU_ANM_FEVERWALLSPEED_S	( FX32_CONST(1.0) )		// アニメスピード
#define BCT_MARUNOMU_ANM_FEVERWALLSPEED_E	( FX32_CONST(2.30) )		// アニメスピード
#define BCT_MARUNOMU_ANM_FEVERWALLSPEED_ADD	( FX32_CONST(0.10) )	// アニメスピード
#define BCT_MARUNOMU_ANM_FEVERWALLSPEED_SPEED	( FX32_CONST(1.0) )		// アニメスピード


// スコアエフェクト
#define BCT_SCORE_EFFECT_BUF		( 3 )	// スコアエフェクトバッファ
#define BCT_SCORE_EFFECT_MOVE_COUTN	( 16 )	// 動作カウンタ
#define BCT_SCORE_EFFECT_MOVE_Y		( -FX32_CONST(16) )	// 動作値
#define BCT_SCORE_EFFECT_PRI_START	(0)


//-------------------------------------
///	タッチパネルデータ
//=====================================
#define BCT_TOUCH_SYNC		( 4 )
#define BCT_TOUCH_BUFFNUM	( 8 )
static TPData BCT_TOUCH_BUFF[ BCT_TOUCH_BUFFNUM ];



//-------------------------------------
/// BG優先順位スクロール
//=====================================
enum {
	BCT_BGPRI_SCRL_SEQ_NORMAL,
	BCT_BGPRI_SCRL_SEQ_START,
	BCT_BGPRI_SCRL_SEQ_TOPFADE,
	BCT_BGPRI_SCRL_SEQ_BOTTOMFADE,
} ;
#define BCT_BGPRI_SCRL_START_MOSTBACK	( 2 )
#define BCT_BGPRI_SCRL_COUNT_MAX		( 16 )
#define BCT_BGPRI_SCRL_BOTTOMCOUNT_MAX	( 8 )
#define BCT_BGPRI_SCRL_MOSTBACK_MAX		( 3 )
#define BCT_BGPRI_SCRL_PRI_MAX			( 2 )
#define BCT_BGPRI_SCRL_REQ_START_NUM	( 10 )




//-------------------------------------
///	ゲームデータ反映システム
//=====================================
#define BCT_CONT_GAMEDATA_COUNT_MAX			( 16 )
#define BCT_CONT_GAMEDATA_RAREGAME_BIGSTART	( 6 )	// 大小逆転のテーブルを参照する開始位置



//-------------------------------------
///	タッチペンシステム
//=====================================
#define BCT_TOUCHPEN_OAM_RESID		( 120 )		// リソースID[
#define BCT_TOUCHPEN_OAM_BGPRI		( 0 )		// BG優先順位
#define BCT_TOUCHPEN_OAM_SOFPRI		( 0 )		// ソフト優先順位
#define BCT_TOUCHPEN_OAM_X			( 128 )		// 表示座標
#define BCT_TOUCHPEN_OAM_Y			( 120 )		
#define BCT_TOUCHPEN_WAIT			( 16 )
// タッチOAMアニメ定数
enum{
	BCT_TOUCHPEN_ANM_FRAME_START,
	BCT_TOUCHPEN_ANM_FRAME_MOVE00,	// 移動中
	BCT_TOUCHPEN_ANM_FRAME_MOVE01,
	BCT_TOUCHPEN_ANM_FRAME_MOVE02,
	BCT_TOUCHPEN_ANM_FRAME_MOVE03,
	BCT_TOUCHPEN_ANM_FRAME_MOVE04,
	BCT_TOUCHPEN_ANM_FRAME_MOVE05,
	BCT_TOUCHPEN_ANM_FRAME_MOVE06,
	BCT_TOUCHPEN_ANM_FRAME_MOVE07,
	BCT_TOUCHPEN_ANM_FRAME_MOVEOK,	// 上にいきった
	BCT_TOUCHPEN_ANM_FRAME_FADEOUT,	// ここで発射させる
	BCT_TOUCHPEN_ANM_FRAME_END,		// 消えた
	BCT_TOUCHPEN_ANM_FRAME_NUM,		// 消えた
};
// 動作のシーケンス
enum{
	BCT_TOUCHPEN_ANM_SEQ_MOVE_START,	// 開始
	BCT_TOUCHPEN_ANM_SEQ_MOVE_MAIN,		// 動作メイン
	BCT_TOUCHPEN_ANM_SEQ_MOVE_WAIT,		// 次の開始までのウエイト
	BCT_TOUCHPEN_ANM_SEQ_END_WAIT,		// 終了待ち（木の実が全部動作終わるのを待つ）
};
#define BCT_TOUCHPEN_ANM_ROOP_NUM	( 2 )
// タッチ動作パラメータ
static const s16 sc_BCT_TOUCHPEN_ANM_FRAME_Y[ BCT_TOUCHPEN_ANM_FRAME_NUM ] = {
	BCT_TOUCHPEN_OAM_Y+57, 
	BCT_TOUCHPEN_OAM_Y+47, 
	BCT_TOUCHPEN_OAM_Y+27, 
	BCT_TOUCHPEN_OAM_Y+17, 
	BCT_TOUCHPEN_OAM_Y+7, 
	BCT_TOUCHPEN_OAM_Y-3, 
	BCT_TOUCHPEN_OAM_Y-13, 
	BCT_TOUCHPEN_OAM_Y-23, 
	BCT_TOUCHPEN_OAM_Y-33, 
	BCT_TOUCHPEN_OAM_Y-43, 
	BCT_TOUCHPEN_OAM_Y-43, 
	BCT_TOUCHPEN_OAM_Y-43, 
};



//-------------------------------------
///	途中得点管理システム
//=====================================
#define BCT_MDLSCR_TOP_DEF	( 0xffff )	// 無効な一位のインデックス



//-------------------------------------
///	木の実カウンターシステム
//=====================================
// ビットマップサイズ
#define BCT_NUTS_COUNT_BMP_SIZX		( 3 )
#define BCT_NUTS_COUNT_BMP_SIZY		( 2 )
// ビットマップパレット管理ID
#define BCT_NUTS_COUNT_FONTOAM_PLTT_ID	( 150 )
// FONTOAMの表示パラメータ
#define BCT_NUTS_COUNT_FONTOAM_X		( -7 )
#define BCT_NUTS_COUNT_FONTOAM_Y		( -5 )
#define BCT_NUTS_COUNT_FONTOAM_BG_PRI	( 0 )
#define BCT_NUTS_COUNT_FONTOAM_SF_PRI	( 0 )
// 動作定数
enum{
	BCT_NUTS_COUNT_SEQ_WAIT,
	BCT_NUTS_COUNT_SEQ_IN,
	BCT_NUTS_COUNT_SEQ_MAIN,
	BCT_NUTS_COUNT_SEQ_OUT,
};
// セル表示パラメータ
#define BCT_NUTS_COUNT_DRAW_X		( 32 )
#define BCT_NUTS_COUNT_DRAW_Y		( 48 )
#define BCT_NUTS_COUNT_DRAW_RESID	( 140 )
#define BCT_NUTS_COUNT_DRAW_BGPRI	( BCT_NUTS_COUNT_FONTOAM_BG_PRI )
#define BCT_NUTS_COUNT_DRAW_SFPRI	( BCT_NUTS_COUNT_FONTOAM_SF_PRI+1 )
// 入出動作パラメータ
#define BCT_NUTS_COUNT_INOUT_SX			( -FX32_CONST(64) )
#define BCT_NUTS_COUNT_INOUT_EX			( FX32_CONST(BCT_NUTS_COUNT_DRAW_X) )
#define BCT_NUTS_COUNT_INOUT_SS			( FX32_CONST(36) )
#define	BCT_NUTS_COUNT_INOUT_COUNTMAX	( 8 )
#define BCT_NUTS_COUNT_OUT_WAIT			( 12 )
// ゆれ動作パラメータ
#define BCT_NUTS_COUNT_YURE_SY			( FX32_CONST(BCT_NUTS_COUNT_DRAW_Y) )
#define BCT_NUTS_COUNT_YURE_EY			( FX32_CONST(BCT_NUTS_COUNT_DRAW_Y) )
#define BCT_NUTS_COUNT_YURE_SS			( FX32_CONST(12) )
#define	BCT_NUTS_COUNT_YURE_COUNTMAX	( 4 )

 
//-------------------------------------
///	FEVERエフェクト
//=====================================
enum{
	BCT_FEVER_EFF_SEQ_BGM_WAIT,
	BCT_FEVER_EFF_SEQ_BGM_FLASH_OUT,
	BCT_FEVER_EFF_SEQ_BGM_FLASH_OUTWAIT,
	BCT_FEVER_EFF_SEQ_BGM_FLASH_IN,
	BCT_FEVER_EFF_SEQ_BGM_FLASH_INWAIT,
	BCT_FEVER_EFF_SEQ_BGM_FLASH_END,
	BCT_FEVER_EFF_SEQ_NUM,
};
#define BCT_FEVER_EFF_FLASHOUT_FLASHOUT_WAIT	( 58 )
#define BCT_FEVER_EFF_SE_MARUNOMUMOVE			( 12 )
#define BCT_FEVER_EFF_FLASHOUT_FLASHOUT_SYNC	( 4 )
#define BCT_FEVER_EFF_FLASHOUT_FLASHIN_SYNC		( 14 )
#define BCT_FEVER_EFF_BGM_TEMPO_START			( 256 )
#define BCT_FEVER_EFF_BGM_TEMPO_DIF				( 150 )
#define BCT_FEVER_BACK_ANM_SPEED_START			( FX32_CONST(1) )
#define BCT_FEVER_BACK_ANM_SPEED_DIF			( FX32_CONST(25) )



//-----------------------------------------------------------------------------
/**
 *                  構造体宣言
*/
//-----------------------------------------------------------------------------


//-------------------------------------
//	汎用加速動作
//=====================================
typedef struct {
	fx32 x;
	fx32 s_x;
	fx32 s_s;	// 初速度
	fx32 s_a;	// 加速度
	int count;
	int count_max;
} BCT_ADDMOVE_WORK;

//-------------------------------------
/// 開始カウントダウンワーク
//=====================================
typedef struct {
    u32 seq;
    s32 count;
	s32 wait;
    GF_BGL_BMPWIN   helpwin;    // ゲーム説明ウィンドウ
} BCT_COUNTDOWN_DRAW;


//-------------------------------------
/// マルノームワーク
//=====================================
typedef struct {
	u8 hit;				// 当たり判定処理の有無
	u8 eat_flag;		// 食べる許可フラグ
    u16 rota;           // y軸の回転角度
    u16 rotax;          // x軸の回転角度
    u16 speed;          // 回転スピード
    fx32 mouth_size;    // 口のサイズ
    MtxFx33 rotax_mtx;  // X回転のマトリクス
    MtxFx33 rotay_mtx; // Y回転のマトリクス
    MtxFx33 rotaxy_mtx; // XY回転のマトリクス
    VecFx32 hit_plane_n;    // 平面の法線
    VecFx32 center_mat; // 平面の中心
    fx32    d;          // 平面の方程式 D
    fx32    inryoku_area;   // 引力の働くエリアのサイズ
    fx32    inryoku_power;  // 引力の力

	// 基本となるマルノーム座標
	VecFx32 marunomu_mat;	

	// 拡大縮小
	fx32	scale;				// 全体拡大率
	fx32	mouth_div;			// 口の大きさ変動値
	fx32	mouth_min;			// 口の大きさ最小
	fx32	mouth_max;			// 口の大きさ最大
	fx32	hit_x;				// 当たり判定ｘ座標
	fx32	hit_y;				// 当たり判定ｙ座標
	fx32	hitbody_y_min;		// 体の当たり判定高さ最小
	fx32	hitbody_y_dif;		// 体の当たり判定高さ距離
	fx32	hitbody_r;			// 体の当たり判定半径
	fx32	hitbody_r_max;		// 体の当たり判定半径最大
	fx32	hitbody_r_dis;		// 体の当たり判定半径変動値
	fx32	draw_scale;			// 描画拡大率


	// 胴体あたり判定
	VecFx32 matrix;
	fx32 height;
} BCT_MARUNOMU;

//-------------------------------------
/// マルノーム描画データ
//=====================================
typedef struct {
    D3DOBJ      obj[BCT_MARUNOMU_MDL_NUM];
    D3DOBJ_MDL  mdl[BCT_MARUNOMU_MDL_NUM];
    D3DOBJ_ANM  anm[BCT_MARUNOMU_ANM_NUM];

	u16 set_mouthanm;	// 現在設定している口の動きのアニメデータ
	u16	walk_anm_flag;

	// カラーアニメ
	D3DOBJ_ANM	colanm[BCT_MARUNOMU_ANM_COLANM_NUM];// カラーアニメ
	fx32 colanm_frame;	// カラーアニメフレーム
	u16 col_top;		// カラー表示するトップのplno	BCT_PLAYER_NUMなら黒
	u16 col_rand;		// ランダムカラー表示するのか
} BCT_MARUNOMU_DRAW;


//-------------------------------------
/// 木の実データ
//=====================================
typedef struct {
    u16 plno;      // ネットID
    u16 in_flag;    // 木の実がマルノームに入ったか 
    VecFx32 way;    // 方位                         
    fx32    power;      // パワー                   
    VecFx32 mat;    // 座標
    s32 bonus;		// ボーナスカウンタ	（連続で入った木の実の数）
	BOOL special;	// 特殊ボールデータ
} BCT_NUT_DATA;

//-------------------------------------
/// 木の実データ
//=====================================
typedef struct {
    u8 seq;
    u8 end_flag;
    u16 count;
    BCT_NUT_DATA data;
    VecFx32 mat;
	s16 inmove_count;
	u16 inmove_bonus;
	BOOL blockhit;	//　壁当たり判定が必要なのかのフラグ
	fx32 xz_dist;
} BCT_CLIENT_NUTS;

//-------------------------------------
/// 投げデータ
//=====================================
typedef struct {
    u32 hand;           // もっているか
    NNSG2dSVec2 tp_q[ BCT_NUTS_SLOW_Q ];// 投げデータキュー
    u16 top;        // 先頭
    u16 tail;       // 末尾
	u32 time;
} BCT_CLIENT_SLOW;

//-------------------------------------
/// 木の実描画データ
//=====================================
typedef struct {
    D3DOBJ_MDL  mdl[ BCT_NUTSRES_MDLNUM ];
    D3DOBJ_MDL  shadowmdl;	// 影

    CLACT_U_RES_OBJ_PTR     resobj[4];      // 読み込んだりソースのオブジェクト
    CLACT_HEADER            header;         // アクター作成用ヘッダー
} BCT_CLIENT_NUTS_RES;


//-------------------------------------
/// OAM木の実描画
//=====================================
typedef struct {
    CLACT_WORK_PTR p_clwk;
    D3DOBJ      obj;
    D3DOBJ      shadow;	// かげよう
    BOOL draw2d;                    // 2d描画させるか
    const BCT_CLIENT_NUTS* cp_data;
	u16 rota_x;
	u16 rota_z;
	u16 rota_speed_x;
	u16 rota_speed_z;
	s16 rota_chg_count;
} BCT_CLIENT_NUTS_DRAW;

//-------------------------------------
/// 持っている木の実描画用
//=====================================
typedef struct {
    BOOL draw;
    CLACT_WORK_PTR p_clwk;
} BCT_CLIENT_HANDNUTS_DRAW;

//-------------------------------------
///	持っている木の実がこぼれ落ちましたエフェクト
//=====================================
typedef struct {
    BOOL draw;
    CLACT_WORK_PTR p_clwk;
	VecFx32 mat;
	s32 count;
	BOOL left;
} BCT_CLIENT_OAMAWAYNUTS;
typedef struct {
	BCT_CLIENT_OAMAWAYNUTS nutsbuff[BCT_OAMAWAYNUTS_BUFFNUM];
} BCT_CLIENT_OAMAWAYNUTS_DRAW;

//-------------------------------------
///	SCOREエフェクトワーク
//=====================================
typedef struct {
	CLACT_WORK_PTR  p_clwk[ BCT_SCORE_EFFECT_BUF ];
	u32 count[ BCT_SCORE_EFFECT_BUF ];
	VecFx32 mat[ BCT_SCORE_EFFECT_BUF ];
	BOOL  mydata;
} BCT_CLIENT_SCORE_EFFECT_WK;

//-------------------------------------
/// SCOREエフェクト
//=====================================
typedef struct {
    BCT_CLIENT_SCORE_EFFECT_WK wk[ BCT_PLAYER_NUM ];
} BCT_CLIENT_SCORE_EFFECT;

//-------------------------------------
/// 結果発表表示ワーク
//=====================================
typedef struct {
    u32 seq;
	u32 count;
	BCT_ADDMOVE_WORK mouthmove;
} BCT_CLIENT_RESULT;

//-------------------------------------
///	背景表示グラフィック
//=====================================
typedef struct {
    D3DOBJ      obj[BCT_MAINBACK_MDL_NUM];
    D3DOBJ_MDL  mdl[BCT_MAINBACK_MDL_NUM];
	D3DOBJ_ANM	anm[BCT_MAINBACK_ANM_NUM];
	fx32 anm_speed;
	u8 fever;
	u8	fever_anm_seq;
	s16	fever_anm_wait;
	fx32 fever_anm_frame;
	fx32 fever_anm_speed;
} BCT_CLIENT_MAINBACK;


//-------------------------------------
/// BG優先順位スクロール
//=====================================
typedef struct {
    u16 seq;		// シーケンス
	s16 count;		// エフェクトカウンタ
	s16 most_back;	// 一番下のBG面
	u8 plno;		// プレイヤーナンバー
	u8 req_num;		// リクエスト数
} BCT_CLIENT_BGPRI_SCRL;

//-------------------------------------
///	ゲームデータ設定システム
//=====================================
typedef struct {
	BCT_GAMEDATA_ONE		set;		// 設定値
	BCT_GAMEDATA_ONE		last;	// 前のデータ
	BCT_GAMEDATA_ONE		dif;		// 変動値
	s32						count;		// カウンタ
} BCT_CLIENT_CONT_GAMEDATA;


//-------------------------------------
///	タッチペンワーク
//=====================================
typedef struct {
    CLACT_U_RES_OBJ_PTR     resobj[4];      // 読み込んだりソースのオブジェクト
    CLACT_HEADER            header;         // アクター作成用ヘッダー
    CLACT_WORK_PTR p_clwk;

	u8					move;			// 動作フラグ
	u8					roop;			// ループ数
	u8					seq;			// シーケンス
	u8					nut_set;		// 木の実設定したか
	s32					wait;			// ウエイト
} BCT_CLIENT_TOUCHPEN_MOVE;

//-------------------------------------
///	Middleスコア
//=====================================
typedef struct {
	u32				score[ BCT_PLAYER_NUM ];	// 得点バッファ
	u32				score_msk;					// 受け取ったマスク
	u32				top_idx;					// 今のところの１位
} BCT_CLIENT_MIDDLE_SCORE;


//-------------------------------------
///	木の実カウンター
//=====================================
typedef struct {

    CLACT_U_RES_OBJ_PTR     resobj[4];			// 読み込んだりソースのオブジェクト
    CLACT_HEADER            header;				// アクター作成用ヘッダー
	CLACT_WORK_PTR			p_tblwk;			// テーブルワーク
	
	GF_BGL_BMPWIN			objbmp;				// 文字列ビットマップデータ
	FONTOAM_OBJ_PTR			p_fontoam;			// フォントOAMワーク
	FONTOAM_OAM_DATA_PTR	p_fontoam_data;		// フォントOAM構成データ
	CHAR_MANAGER_ALLOCDATA	fontoam_chardata;	// キャラクタ確保データ
	STRBUF*					p_str;				// 文字列データ
	CLACT_U_RES_OBJ_PTR		p_fontoam_pltt;		// パレットリソース

	BCT_ADDMOVE_WORK		inout_data;		// 入出移動データ
	BCT_ADDMOVE_WORK		yure_data;		// ゆれ移動データ
	s16						inout_count;	// 動作カウンタ
	s16						yure_count;		// ゆれカウンタ
	u16						seq;			// 動作シーケンス
	s16						out_wait;		// 出口ウエイト


	u32 heapID;
	
} BCT_CLIENT_NUTS_COUNT;


//-------------------------------------
///	FEVERエフェクト
//=====================================
typedef struct {
	u16 seq;
	u8 move;
	s8 wait;
} BCT_CLIENT_FEVER_EFF_WK;



//-------------------------------------
/// 描画システム
//=====================================
typedef struct {
    GF_BGL_INI*         p_bgl;              // GF_BGL_INI
    WORDSET*            p_wordset;          // メッセージ展開用ワークマネージャー
    MSGDATA_MANAGER*    p_msgman;           // メッセージデータマネージャー
    STRBUF*             p_msgstr;           // メッセージ用文字列バッファ
    STRBUF*             p_msgtmp;           // メッセージ用文字列バッファ
    u16                 msg_no;             // メッセージ完了検査ナンバー
    u16                 msg_speed;          // メッセージスピード

	GF_G3DMAN* p_3dman;

    CLACT_SET_PTR           clactSet;                       // セルアクターセット
    CLACT_U_EASYRENDER_DATA renddata;                       // 簡易レンダーデータ
    CLACT_U_RES_MANAGER_PTR resMan[BCT_GRA_RESMAN_NUM]; // キャラ・パレットリソースマネージャ
    CLACT_HEADER            mainoamheader;      // アクター作成用ヘッダー
    CLACT_U_RES_OBJ_PTR     mainoamresobj[BCT_GRA_RESMAN_NUM];

	FONTOAM_SYS_PTR	p_fontoam_sys;	// FONTOAMのシステム

    GF_CAMERA_PTR p_camera;
    VecFx32 target;

	// メイン画面背景
	BCT_CLIENT_MAINBACK mainback;

    // スコアエフェクト
    BCT_CLIENT_SCORE_EFFECT score_effect;

    // 開始アニメ
    BCT_COUNTDOWN_DRAW start;

    // 結果発表 
    BCT_CLIENT_RESULT result;

    // マルノーム
    BCT_MARUNOMU_DRAW marunomu; // マルノーム描画データ

	// START	TIMEUP	エフェクト
	MNGM_COUNTWK*  p_countwk;

	// BG優先順位　エフェクト
	BCT_CLIENT_BGPRI_SCRL bgpri_scrl;	

    // 木の実リソース
    BCT_CLIENT_NUTS_RES nutsres;
    BCT_CLIENT_NUTS_DRAW nuts[BCT_NUTSBUFFOAM_NUM];
    BCT_CLIENT_HANDNUTS_DRAW handnuts;  // 持っている木の実用
	BCT_CLIENT_OAMAWAYNUTS_DRAW oamnutsaway;	//  持っていた木の実を落としたときのエフェクト


	// 木の実カウンター
	BCT_CLIENT_NUTS_COUNT nutscount;


    // アロケータ
    NNSFndAllocator allocator;


} BCT_CLIENT_GRAPHIC;




//-------------------------------------
/// クライアントワーク
//=====================================
typedef struct _BCT_CLIENT{
	u32	move_type;	// 動作タイプ		時間で遷移する動作のタイプ	BCT_MARUNOMU_MOVE_TYPE
	BOOL time_count_flag;
    s32 time;
    s32 time_max;
    u32 comm_num;   // 通信人数
	u32 plno;	// NETID
    s32 score;  // 得点
    s32 bonus;  // ボーナスカウンタ
    BCT_CLIENT_SLOW slow;                       // 投げデータ
    BCT_CLIENT_NUTS nuts[BCT_NUTSBUFF_NUM];     // 木の実データ
    BCT_MARUNOMU marunomu;                      // マルノーム
    BCT_CLIENT_GRAPHIC graphic;                 // グラフィックデータ
    BCT_SCORE_COMM  all_score;                      // みんなのスコア
	TP_ONE_DATA tp_one;							// タッチパネル１つデータ
	BCT_CLIENT_CONT_GAMEDATA gamedata_cont;		// ゲームデータコントロール
	const BCT_GAMEDATA* cp_gamedata;			// ゲームデータ
	u16	gamedata_now;							// 今のゲームデータインデックス
	u16	gamedata_last;							// 前のゲームデータインデックス
	BCT_GAME_TYPE_WK gametype;					// ゲームタイプ
	BOOL gametype_flag;							// ゲームタイプ受信チェックフラグ

	BCT_CLIENT_TOUCHPEN_MOVE touchpen_wk;		// タッチペンシステム

	BCT_CLIENT_MIDDLE_SCORE	middle_score;		// 途中の得点管理システム

	
	// FEVERエフェクト
	BCT_CLIENT_FEVER_EFF_WK fever_eff;

} ;



//-----------------------------------------------------------------------------
/**
 *                  パーセンテージに対応した回転スピード
 */
//-----------------------------------------------------------------------------

#ifdef BCT_DEBUG_HITZONE
//-------------------------------------
/// 当たり判定位置表示オブジェクト
//=====================================
typedef struct {
    D3DOBJ_MDL  mdl;
    D3DOBJ      obj[4];
} BCT_DEBUG_POSITION;

static BCT_DEBUG_POSITION BCT_DPD;
static void BCT_DEBUG_PositionInit( ARCHANDLE* p_handle, u32 heapID );
static void BCT_DEBUG_PositionDraw( const BCT_CLIENT* cp_client );
static void BCT_DEBUG_PositionExit( void );

#endif



#ifdef BCT_DEBUG_AUTOSLOW
static void BCT_DEBUG_AutoSlow( BCT_CLIENT* p_wk );
#endif

#ifdef PM_DEBUG
static int BCT_DEBUG_in_num = 0;
#endif


//-----------------------------------------------------------------------------
/**
 *                  プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
static void BCT_AddMoveReqFx( BCT_ADDMOVE_WORK* p_work, fx32 s_x, fx32 e_x, fx32 s_s, int count_max );
static BOOL	BCT_AddMoveMainFx( BCT_ADDMOVE_WORK* p_work );

// 2D座標系の表示位置を取得する
static void BCT_netID2DMatrixGet( u32 comm_num, u32 myplno, u32 targetplno, s32* p_x,  s32* p_y );


// 簡単木の実発射関数
static void BCT_EasyNutsSet( BCT_CLIENT* p_wk, u16 t0_x, u16 t0_y, u16 t1_x, u16 t1_y, u32 moveseq );

// ボーナスナンバーのスコア取得
static u32 BCT_AddScoreGet( BCT_CLIENT* cp_wk );

// スコアエフェクト
static void BCT_CLIENT_ScoreEffectInit( BCT_CLIENT_SCORE_EFFECT* p_wk, BCT_CLIENT_GRAPHIC* p_drawsys, u32 comm_num, u32 plno, u32 heapID );
static void BCT_CLIENT_ScoreEffectExit( BCT_CLIENT_SCORE_EFFECT* p_wk, BCT_CLIENT_GRAPHIC* p_drawsys);
static void BCT_CLIENT_ScoreEffectStart( BCT_CLIENT_SCORE_EFFECT* p_wk, u32 plno, u32 bonus, BCT_MARUNOMU_MOVE_TYPE movetype );
static void BCT_CLIENT_ScoreEffectMain( BCT_CLIENT_SCORE_EFFECT* p_wk );
static void BCT_CLIENT_ScoreEffectWkInit( BCT_CLIENT_SCORE_EFFECT_WK* p_wk, CLACT_ADD* p_ad, BCT_CLIENT_GRAPHIC* p_drawsys, u32 comm_num, u32 plno, u32 myplno, u32 heapID );
static void BCT_CLIENT_ScoreEffectWkExit( BCT_CLIENT_SCORE_EFFECT_WK* p_wk );
static void BCT_CLIENT_ScoreEffectWkStart( BCT_CLIENT_SCORE_EFFECT_WK* p_wk, u32 plno, u32 bonus, BCT_MARUNOMU_MOVE_TYPE movetype );
static void BCT_CLIENT_ScoreEffectWkMain( BCT_CLIENT_SCORE_EFFECT_WK* p_wk );
static void BCT_CLIENT_ScoreEffectWkSetMatrix( BCT_CLIENT_SCORE_EFFECT_WK* p_wk, u32 idx );
static void BCT_CLIENT_ScoreEffectWkEnd( BCT_CLIENT_SCORE_EFFECT_WK* p_wk, u32 idx );

// 開始ワーク
static void BCT_CLIENT_StartSysInit( BCT_COUNTDOWN_DRAW* p_graphic, BCT_CLIENT_GRAPHIC* p_drawsys, const BCT_GAMEDATA* cp_param, u32 commnum, u32 myplno, ARCHANDLE* p_handle, u32 heapID );
static void BCT_CLIENT_StartSysExit( BCT_COUNTDOWN_DRAW* p_graphic, BCT_CLIENT_GRAPHIC* p_drawsys );
static void BCT_CLIENT_StartSysCountDownInit( BCT_COUNTDOWN_DRAW* p_graphic, BCT_CLIENT_GRAPHIC* p_drawsys );
static BOOL BCT_CLIENT_StartSysCountDown( BCT_COUNTDOWN_DRAW* p_graphic, BCT_CLIENT_GRAPHIC* p_drawsys );
static void BCT_CLIENT_StartSysDrawOff( BCT_COUNTDOWN_DRAW* p_graphic, BCT_CLIENT_GRAPHIC* p_drawsys );
static void BCT_CLIENT_StartSysMarunomuChange( BCT_COUNTDOWN_DRAW* p_graphic, BCT_CLIENT_GRAPHIC* p_drawsys );
static BOOL BCT_CLIENT_StartSysGameStart( BCT_COUNTDOWN_DRAW* p_graphic, BCT_CLIENT_GRAPHIC* p_drawsys );
static s32 BCT_CLIENT_StartSysScrllYGet( BCT_COUNTDOWN_DRAW* p_graphic );
static void BCT_CLIENT_StartSysMarunomuMatrixSet( BCT_CLIENT_GRAPHIC* p_drawsys, const BCT_MARUNOMU* cp_data );

// 結果発表ワーク
static void BCT_CLIENT_EndSysInit( BCT_CLIENT_GRAPHIC* p_graphic, u32 comm_num, u32 plno, u32 heapID );
static void BCT_CLIENT_EndSysExit( BCT_CLIENT_GRAPHIC* p_graphic, u32 comm_num );
static void BCT_CLIENT_EndSysMarunomuAnmChg( BCT_CLIENT_GRAPHIC* p_graphic );
static BOOL BCT_CLIENT_EndSysMarunomuAnm( BCT_CLIENT_GRAPHIC* p_graphic );
static BOOL BCT_CLIENT_EndSysTimeUpAnm( BCT_CLIENT_GRAPHIC* p_graphic );

// マルノーム
static void BCT_CLIENT_MarunomuInit( const BCT_CLIENT* cp_wk, BCT_MARUNOMU* p_maru );
static void BCT_CLIENT_MarunomuMain( const BCT_CLIENT* cp_wk, BCT_MARUNOMU* p_maru );
static void BCT_CLIENT_MarunomuSetHitFlag( BCT_MARUNOMU* p_maru, BOOL flag );
static void BCT_CLIENT_MarunomuRotaXSet( BCT_MARUNOMU* p_maru, u16 rota_x );
static void BCT_CLIENT_MarunomuRotaYSet( BCT_MARUNOMU* p_maru, u16 rota_y );
static u32 BCT_CLIENT_MarunomuMouthHitCheck( const BCT_MARUNOMU* cp_maru, const VecFx32* cp_mat, const VecFx32* cp_move, VecFx32* p_inryoku );
static BOOL BCT_CLIENT_MarunomuBodyHitCheck( const BCT_MARUNOMU* cp_maru, const VecFx32* cp_mat );
static fx32 BCT_CLIENT_MarunomuMouthSizeGet( const BCT_MARUNOMU* cp_maru, u16 rota_x );
static fx32 BCT_CLIENT_MarunomuBodyHeightGet( const BCT_MARUNOMU* cp_maru,  u16 rota_x );
static void BCT_CLIENT_MaruScaleSet( BCT_MARUNOMU* p_maru, fx32 scale );
static void BCT_CLIENT_MaruRotSpeedSet( BCT_MARUNOMU* p_maru, u16 speed );
static void BCT_CLIENT_MaruMatZSet( BCT_MARUNOMU* p_maru, fx32 z );
static u16 BCT_CLIENT_MarunomuGetRotaX_Easy( const BCT_MARUNOMU* cp_maru, s32 time, s32 max_time );
static u16 BCT_CLIENT_MarunomuGetRotaX_FeverEff( const BCT_MARUNOMU* cp_maru, s32 time, s32 max_time );
static u16 BCT_CLIENT_MarunomuGetRotaX_Fever( const BCT_MARUNOMU* cp_maru, s32 time, s32 max_time );

//////// 動作させない関数
#if 0
static u16 BCT_CLIENT_MarunomuParRotaGet( s32 par, s32 max_par );
static u16 BCT_CLIENT_MarunomuParRotaGetEx( s32 par, s32 max_par, u16 start, u16 div );
static u16 BCT_CLIENT_MarunomuRotaSpeedGet( s32 time, s32 max_time );
static u16 BCT_CLIENT_MarunomuXRotaGet( s32 time, s32 max_time );
static u16 BCT_CLIENT_MarunomuRotaSpeedGet2( s32 time, s32 max_time );
static u16 BCT_CLIENT_MarunomuXRotaGet2( s32 time, s32 max_time );
static u16 BCT_CLIENT_MarunomuRotaSpeedGet3( s32 time, s32 max_time );
static u16 BCT_CLIENT_MarunomuXRotaGet3( s32 time, s32 max_time );
#endif
//////////////////////////


// 通信木の実データと木の実
static void BCT_CLIENT_NutsDataToComm( const BCT_NUT_DATA* cp_data, BCT_NUT_COMM* p_comm );
static void BCT_CLIENT_NutsCommToData( const BCT_NUT_COMM* cp_comm, BCT_NUT_DATA* p_data );

// 木の実
static void BCT_CLIENT_NutsSet( BCT_CLIENT* p_wk, const BCT_NUT_DATA* cp_data, u32 seq );
static void BCT_CLIENT_NutsEnd( BCT_CLIENT* p_wk, BCT_CLIENT_NUTS* p_data, BOOL in_flag, u32 bonus );
static void BCT_CLIENT_NutsDel( BCT_CLIENT_NUTS* p_data );
static void BCT_CLIENT_AllNutsMove( BCT_CLIENT* p_wk );
static void BCT_CLIENT_NutsMove( BCT_CLIENT* p_wk, BCT_CLIENT_NUTS* p_nuts );
static void BCT_CLIENT_AllNutsEndCont( BCT_CLIENT* p_wk );
static void BCT_CLIENT_AllNutsEndContAllDel( BCT_CLIENT* p_wk );
static void BCT_CLIENT_NutsEndCont( BCT_CLIENT_NUTS* p_nuts, BCT_CLIENT* p_wk );
static void BCT_CLIENT_NutsSeqOtherChange( BCT_CLIENT* p_wk );
static BOOL BCT_CLIENT_NutsMoveCheck( const BCT_CLIENT* cp_wk );
static fx32 BCT_CLIENT_NutsNowPowerGet( const BCT_CLIENT_NUTS* cp_nuts );
static void BCT_CLIENT_NutsMoveGet( const BCT_CLIENT_NUTS* cp_nuts, VecFx32* p_move );
static void BCT_CLIENT_NutsAwayStart( BCT_CLIENT_NUTS* p_nuts, BCT_CLIENT* p_wk, BOOL maru_hit );
static void BCT_CLIENT_NutsAwayStartPowerReq( BCT_CLIENT_NUTS* p_nuts, BCT_CLIENT* p_wk, BOOL maru_hit, fx32 mul_power, BOOL move_side );
static void BCT_CLIENT_NutsAwayMove( BCT_CLIENT_NUTS* p_nuts, BCT_CLIENT* p_wk );
static BOOL BCT_CLIENT_NutsAwayDitchHitCheck( const BCT_CLIENT_NUTS* cp_nuts );
static BOOL BCT_CLIENT_NutsAwayMarunomuHitCheck( const BCT_CLIENT_NUTS* cp_nuts );
static BOOL BCT_CLIENT_NutsAwayBlockHitCheck( BCT_CLIENT_NUTS* p_nuts );
static void BCT_CLIENT_NutsAwayRetWayMove( BCT_CLIENT_NUTS* p_nuts, fx32 dist );
static void BCT_CLIENT_NutsInMoveStart( BCT_CLIENT_NUTS* p_nuts, BCT_CLIENT* p_wk );
static void BCT_CLIENT_NutsInMove( BCT_CLIENT_NUTS* p_nuts, BCT_CLIENT* p_wk );
static void BCT_CLIENT_NutsMatrixMove( BCT_CLIENT_NUTS* p_nuts, const VecFx32* cp_add );


// タッチによる動作
static void BCT_CLIENT_NutsSlow( BCT_CLIENT* p_wk );
static void BCT_CLIENT_NutsSlowEnd( BCT_CLIENT* p_wk );
static void BCT_CLIENT_NutsSlowMain( BCT_CLIENT* p_wk );
static BOOL BCT_CLIENT_NutsSlowStartCheck( BCT_CLIENT* p_wk );
static void BCT_CLIENT_NutsSlowQInit( BCT_CLIENT_SLOW* p_wk );
static void BCT_CLIENT_NutsSlowQPush( BCT_CLIENT_SLOW* p_wk, NNSG2dSVec2 data );
static BOOL BCT_CLIENT_NutsSlowQPop( BCT_CLIENT_SLOW* p_wk, NNSG2dSVec2* p_data );
static BOOL BCT_CLIENT_NutsSlowQGetTop( BCT_CLIENT_SLOW* p_wk, NNSG2dSVec2* p_data );
static BOOL BCT_CLIENT_NutsSlowQGetTail( BCT_CLIENT_SLOW* p_wk, NNSG2dSVec2* p_data );
static u32 BCT_CLIENT_NutsSlowQGetInNum( BCT_CLIENT_SLOW* p_wk );
static void BCT_CLIENT_NutsSlowClear( BCT_CLIENT* p_wk );


// ゲームデータ管理システム
static void BCT_CLIENT_ContGameData_Init( BCT_CLIENT_CONT_GAMEDATA* p_wk, const BCT_GAMEDATA* cp_gamedata, const BCT_GAME_TYPE_WK* cp_gametype );
static void BCT_CLIENT_ContGameData_Start( BCT_CLIENT_CONT_GAMEDATA* p_wk, const BCT_GAMEDATA* cp_gamedata, const BCT_GAME_TYPE_WK* cp_gametype, u16 last, u16 next );
static BOOL BCT_CLIENT_ContGameData_Main( BCT_CLIENT_CONT_GAMEDATA* p_wk );
static void BCT_CLIENT_ContGameData_Reflect( BCT_CLIENT* p_wk, const BCT_CLIENT_CONT_GAMEDATA* cp_data );
static void BCT_CLIENT_GameDataOne_Reflect( BCT_CLIENT* p_wk, const BCT_GAMEDATA_ONE* cp_data );
static void BCT_CLIENT_GameData_GetGameTypeOneData( const BCT_GAMEDATA* cp_gamedata, const BCT_GAME_TYPE_WK* cp_gametype, u32 idx, BCT_GAMEDATA_ONE* p_buff );


// タッチペン動作処理
static void BCT_CLIENT_TOUCHPEN_Init( BCT_CLIENT_TOUCHPEN_MOVE* p_wk, BCT_CLIENT_GRAPHIC* p_graphic, u32 heapID );
static void BCT_CLIENT_TOUCHPEN_Exit( BCT_CLIENT_TOUCHPEN_MOVE* p_wk, BCT_CLIENT_GRAPHIC* p_graphic );
static void BCT_CLIENT_TOUCHPEN_Start( BCT_CLIENT_TOUCHPEN_MOVE* p_wk );
static BOOL BCT_CLIENT_TOUCHPEN_Main( BCT_CLIENT_TOUCHPEN_MOVE* p_wk, BCT_CLIENT* p_sys );


// 途中経過管理システム
static void BCT_CLIENT_MDLSCR_Init( BCT_CLIENT_MIDDLE_SCORE* p_wk );
static void BCT_CLIENT_MDLSCR_Set( BCT_CLIENT_MIDDLE_SCORE* p_wk, u32 score, u32 idx );
static void BCT_CLIENT_MDLSCR_Reset( BCT_CLIENT_MIDDLE_SCORE* p_wk );
static u32 BCT_CLIENT_MDLSCR_GetTop( const BCT_CLIENT_MIDDLE_SCORE* cp_wk );
static BOOL BCT_CLIENT_MDLSCR_CheckInNum( const BCT_CLIENT_MIDDLE_SCORE* cp_wk, u32 num );



// グラフィック初期化
static void BCT_CLIENT_GraphicInit( BCT_CLIENT* p_wk, u32 heapID );
static void BCT_CLIENT_GraphicDelete( BCT_CLIENT* p_wk );
static void BCT_CLIENT_GraphicMain( const BCT_CLIENT* cp_wk, BCT_CLIENT_GRAPHIC* p_wk );
static void BCT_CLIENT_GraphicDrawCore( const BCT_CLIENT* cp_wk, BCT_CLIENT_GRAPHIC* p_wk );
static void BCT_CLIENT_BankSet( void );
static void BCT_CLIENT_3DSetUp( void );
static void BCT_CLIENT_3DInit( BCT_CLIENT_GRAPHIC* p_wk, u32 comm_num, u32 plno, u32 heapID );
static void BCT_CLIENT_3DExit( BCT_CLIENT_GRAPHIC* p_wk );
static void BCT_CLIENT_BgInit( BCT_CLIENT_GRAPHIC* p_wk, u32 heapID );
static void BCT_CLIENT_BgExit( BCT_CLIENT_GRAPHIC* p_wk );
static void BCT_CLIENT_OamInit( BCT_CLIENT_GRAPHIC* p_wk, u32 heapID );
static void BCT_CLIENT_OamExit( BCT_CLIENT_GRAPHIC* p_wk );
static void BCT_CLIENT_MsgInit( BCT_CLIENT_GRAPHIC* p_wk, u32 heapID );
static void BCT_CLIENT_MsgExit( BCT_CLIENT_GRAPHIC* p_wk );
static void BCT_CLIENT_CameraInit( BCT_CLIENT_GRAPHIC* p_wk, u32 comm_num, u32 plno, u32 heapID );
static void BCT_CLIENT_CameraExit( BCT_CLIENT_GRAPHIC* p_wk );
static void BCT_CLIENT_CameraMain( BCT_CLIENT_GRAPHIC* p_wk );
static void BCT_CLIENT_BgResLoad( BCT_CLIENT_GRAPHIC* p_wk, ARCHANDLE* p_handle, u32 plno, u32 heapID );
static void BCT_CLIENT_BgResRelease( BCT_CLIENT_GRAPHIC* p_wk );

static void BCT_CLIENT_MainOamInit( BCT_CLIENT_GRAPHIC* p_wk, ARCHANDLE* p_handle, u32 heapID );
static void BCT_CLIENT_MainOamExit( BCT_CLIENT_GRAPHIC* p_wk );

static void BCT_CLIENT_MarunomuDrawInit( BCT_MARUNOMU_DRAW* p_wk, ARCHANDLE* p_handle, u32 heapID, NNSFndAllocator* p_allocator );
static void BCT_CLIENT_MarunomuDrawExit( BCT_MARUNOMU_DRAW* p_wk, NNSFndAllocator* p_allocator );
static void BCT_CLIENT_MarunomuDrawMain( BCT_MARUNOMU_DRAW* p_wk, const BCT_MARUNOMU* cp_data, BCT_MARUNOMU_MOVE_TYPE movetype );
static void BCT_CLIENT_MarunomuDrawAnmMain( BCT_MARUNOMU_DRAW* p_wk, const BCT_MARUNOMU* cp_data, BCT_MARUNOMU_MOVE_TYPE movetype );
static void BCT_CLIENT_MarunomuDrawMatrixSet( BCT_MARUNOMU_DRAW* p_wk, const BCT_MARUNOMU* cp_data );
static void BCT_CLIENT_MarunomuDrawRotaSet( BCT_MARUNOMU_DRAW* p_wk, const BCT_MARUNOMU* cp_data );
static void BCT_CLIENT_MarunomuDrawScaleSet( BCT_MARUNOMU_DRAW* p_wk, const BCT_MARUNOMU* cp_data );
static void BCT_CLIENT_MarunomuDrawDraw( BCT_MARUNOMU_DRAW* p_wk, const BCT_MARUNOMU* cp_data );
static void BCT_CLIENT_MarunomuDrawAnmRotaSet( BCT_MARUNOMU_DRAW* p_wk, u32 rotax );
static void BCT_CLIENT_MarunomuDrawSetMouthAnm( BCT_MARUNOMU_DRAW* p_wk, u32 anmno );
static void BCT_CLIENT_MarunomuDrawLoopMouthAnm( BCT_MARUNOMU_DRAW* p_wk );
static BOOL BCT_CLIENT_MarunomuDrawNoLoopMouthAnm( BCT_MARUNOMU_DRAW* p_wk );
static BOOL BCT_CLIENT_MarunomuDrawNoLoopMouthAnm_Speed( BCT_MARUNOMU_DRAW* p_wk, fx32 speed );
static void BCT_CLIENT_MarunomuDrawSetWalkAnm( BCT_MARUNOMU_DRAW* p_wk, BOOL flag );
static void BCT_CLIENT_MarunomuDrawSetColAnmRand( BCT_MARUNOMU_DRAW* p_wk, BOOL flag );
static void BCT_CLIENT_MarunomuDrawSetColAnmTop( BCT_MARUNOMU_DRAW* p_wk, u32 plno );
static void BCT_CLIENT_MarunomuDrawColAnmMain( BCT_MARUNOMU_DRAW* p_wk );

static void BCT_CLIENT_2DMATto3DMAT( s16 x, s16 y, VecFx32* p_mat );
static void BCT_CLIENT_3DMATto2DMAT( const VecFx32* cp_mat, NNSG2dSVec2* p_2dmat );
static void BCT_CLIENT_VecNetIDRot( const VecFx32* cp_vec, VecFx32* p_invec, u32 plno, u32 comm_num );
static void BCT_CLIENT_VecNetIDRetRot( const VecFx32* cp_vec, VecFx32* p_invec, u32 plno, u32 comm_num );

static void BCT_CLIENT_NutsDrawSysInit( BCT_CLIENT_GRAPHIC* p_wk, ARCHANDLE* p_handle, u32 heapID  );
static void BCT_CLIENT_NutsDrawSysExit( BCT_CLIENT_GRAPHIC* p_wk );
static void BCT_CLIENT_NutsDrawSysStart( BCT_CLIENT_GRAPHIC* p_wk, const BCT_CLIENT_NUTS* cp_data, u32 comm_num, u32 plno );
static void BCT_CLIENT_NutsDrawSysEnd( BCT_CLIENT_GRAPHIC* p_wk, const BCT_CLIENT_NUTS* cp_data );
static void BCT_CLIENT_NutsDrawSysMain( BCT_CLIENT_GRAPHIC* p_wk, u32 comm_num );

static void BCT_CLIENT_NutsDrawInit( BCT_CLIENT_GRAPHIC* p_wk, BCT_CLIENT_NUTS_DRAW* p_data, u32 heapID );
static void BCT_CLIENT_NutsDrawExit( BCT_CLIENT_GRAPHIC* p_wk, BCT_CLIENT_NUTS_DRAW* p_data );
static void BCT_CLIENT_NutsDrawStart( BCT_CLIENT_GRAPHIC* p_wk, BCT_CLIENT_NUTS_DRAW* p_data, const BCT_CLIENT_NUTS* cp_data, u32 comm_num );
static void BCT_CLIENT_NutsDrawStartNoOam( BCT_CLIENT_GRAPHIC* p_wk, BCT_CLIENT_NUTS_DRAW* p_data, const BCT_CLIENT_NUTS* cp_data );
static void BCT_CLIENT_NutsDrawMain( BCT_CLIENT_NUTS_DRAW* p_data, BCT_CLIENT_GRAPHIC* p_wk, u32 comm_num );
static void BCT_CLIENT_NutsDrawShadowMatrixSet( BCT_CLIENT_NUTS_DRAW* p_data );
static void BCT_CLIENT_NutsDrawShadowAlpahReset( BCT_CLIENT_NUTS_DRAW* p_data );
static void BCT_CLIENT_NutsDrawEnd( BCT_CLIENT_NUTS_DRAW* p_data );
static BCT_CLIENT_NUTS_DRAW* BCT_CLIENT_NutsDrawWkGet( BCT_CLIENT_GRAPHIC* p_wk );
static BOOL BCT_CLIENT_NutsDrawMatrixSet( BCT_CLIENT_NUTS_DRAW* p_data, const BCT_CLIENT_NUTS* cp_data, u32 comm_num );
static void BCT_CLIENT_Nuts3DDrawOn( BCT_CLIENT_NUTS_DRAW* p_data, BCT_CLIENT_GRAPHIC* p_wk );
static D3DOBJ_MDL* BCT_CLIENT_Nuts3DMdlGet( const BCT_CLIENT_NUTS* cp_data, BCT_CLIENT_NUTS_RES* p_nutsres );
static void BCT_CLIENT_NutsDrawRotaSet( BCT_CLIENT_NUTS_DRAW* p_data );

static void BCT_CLIENT_HandNutsDrawInit( BCT_CLIENT_GRAPHIC* p_wk, BCT_CLIENT_HANDNUTS_DRAW* p_nuts, u32 plno, u32 heapID );
static void BCT_CLIENT_HandNutsDrawExit( BCT_CLIENT_HANDNUTS_DRAW* p_nuts );
static void BCT_CLIENT_HandNutsDrawStart( BCT_CLIENT_HANDNUTS_DRAW* p_nuts );
static void BCT_CLIENT_HandNutsDrawMain( BCT_CLIENT_HANDNUTS_DRAW* p_nuts );
static void BCT_CLIENT_HandNutsDrawEnd( BCT_CLIENT_HANDNUTS_DRAW* p_nuts );
static void BCT_CLIENT_HandNutsDrawSetMatrix( BCT_CLIENT_HANDNUTS_DRAW* p_nuts, s32 x, s32 y );

static void BCT_CLIENT_OamAwayNutsInit( BCT_CLIENT_GRAPHIC* p_drawsys, BCT_CLIENT_OAMAWAYNUTS_DRAW* p_wk, u32 plno, u32 heapID );
static void BCT_CLIENT_OamAwayNutsExit( BCT_CLIENT_GRAPHIC* p_drawsys, BCT_CLIENT_OAMAWAYNUTS_DRAW* p_wk );
static void BCT_CLIENT_OamAwayNutsMain( BCT_CLIENT_OAMAWAYNUTS_DRAW* p_wk );
static void BCT_CLIENT_OamAwayNutsStart( BCT_CLIENT_OAMAWAYNUTS_DRAW* p_wk, s32 x, s32 y );
static void BCT_CLIENT_OamAwayNutsMoveXY( int count, int countmax, int speed, int srota, int erota, s32* p_x, s32* p_y );

static void BCT_CLIENT_MainBackInit( BCT_CLIENT_MAINBACK* p_wk, BCT_CLIENT_GRAPHIC* p_graphic, ARCHANDLE* p_handle, int comm_num, u32 plno, u32 heapID, NNSFndAllocator* p_allocator );
static void BCT_CLIENT_MainBackExit( BCT_CLIENT_MAINBACK* p_wk, BCT_CLIENT_GRAPHIC* p_graphic, NNSFndAllocator* p_allocator );
static void BCT_CLIENT_MainBackDraw( BCT_CLIENT_MAINBACK* p_wk );
static void BCT_CLIENT_MainBackSetDrawFever( BCT_CLIENT_MAINBACK* p_wk );
static void BCT_CLIENT_MainBackSetAnmSpeed( BCT_CLIENT_MAINBACK* p_wk, fx32 speed );

static void BCT_CLIENT_CalcPlaneVecHitCheck( const VecFx32* cp_mat, const VecFx32* cp_move, const VecFx32* cp_n, fx32 d, VecFx32* p_hitmat, fx32* p_time );

static void BCT_CLIENT_CameraTargetYSet( BCT_CLIENT_GRAPHIC* p_gra, fx32 y );
static void BCT_CLIENT_CameraAngleXSet( BCT_CLIENT_GRAPHIC* p_gra, u16 angle );


// BG優先順位スクロール
static void BCT_CLIENT_BGPRISCRL_Init( BCT_CLIENT_BGPRI_SCRL* p_wk, u8 plno );
static void BCT_CLIENT_BGPRISCRL_Main( BCT_CLIENT_BGPRI_SCRL* p_wk, BCT_CLIENT_GRAPHIC* p_gra );
static void BCT_CLIENT_BGPRISCRL_Req( BCT_CLIENT_BGPRI_SCRL* p_wk );
static void BCT_CLIENT_BGPRISCRL_SetPri( BCT_CLIENT_GRAPHIC* p_gra, s16 most_back, u16 plno );


// 木の実カウンター
static void BCT_CLIENT_NUTS_COUNT_Init( BCT_CLIENT_NUTS_COUNT* p_wk, BCT_CLIENT_GRAPHIC* p_gra, ARCHANDLE* p_handle, u32 heapID );
static void BCT_CLIENT_NUTS_COUNT_Exit( BCT_CLIENT_NUTS_COUNT* p_wk, BCT_CLIENT_GRAPHIC* p_gra );
static void BCT_CLIENT_NUTS_COUNT_Start( BCT_CLIENT_NUTS_COUNT* p_wk, BCT_CLIENT_GRAPHIC* p_gra, u32 count );
static void BCT_CLIENT_NUTS_COUNT_End( BCT_CLIENT_NUTS_COUNT* p_wk );
static void BCT_CLIENT_NUTS_COUNT_SetData( BCT_CLIENT_NUTS_COUNT* p_wk, BCT_CLIENT_GRAPHIC* p_gra, u32 count );
static void BCT_CLIENT_NUTS_COUNT_Main( BCT_CLIENT_NUTS_COUNT* p_wk );


// FEVERエフェクト
static void BCT_CLIENT_FEVER_EFF_Start( BCT_CLIENT_FEVER_EFF_WK* p_wk );
static void BCT_CLIENT_FEVER_EFF_Main( BCT_CLIENT_FEVER_EFF_WK* p_wk, BCT_CLIENT_MAINBACK* p_mainback, BCT_MARUNOMU_DRAW* p_marunomudraw );
static void BCT_CLIENT_FEVER_EFF_Reset( BCT_CLIENT_FEVER_EFF_WK* p_wk );

//----------------------------------------------------------------------------
/**
 *  @brief  クライアント初期化
 *
 *  @param  heapID      ヒープID
 *  @param  timeover    タイムリミット
 *  @param  comm_num    通信人数
 *  @param	cp_gamedata	ゲームデータ
 *
 *  @return 作成したワーク
 */
//-----------------------------------------------------------------------------
BCT_CLIENT* BCT_CLIENT_Init( u32 heapID, u32 timeover, u32 comm_num, u32 plno, BCT_GAMEDATA* cp_gamedata )
{
    BCT_CLIENT* p_wk;
	u32 check;

    p_wk = sys_AllocMemory( heapID, sizeof(BCT_CLIENT) );
    memset( p_wk, 0, sizeof(BCT_CLIENT) );

	p_wk->time_count_flag	= TRUE;
    p_wk->time				= 0;
    p_wk->time_max			= timeover;
    p_wk->comm_num			= comm_num;
	p_wk->plno				= plno;
	p_wk->cp_gamedata		= cp_gamedata;
    

    // マルノーム初期化
    BCT_CLIENT_MarunomuInit( p_wk, &p_wk->marunomu );

    // グラフィック初期化
    BCT_CLIENT_GraphicInit( p_wk, heapID );

	// タッチペンシステム初期化
	BCT_CLIENT_TOUCHPEN_Init( &p_wk->touchpen_wk, &p_wk->graphic, heapID );

	// 途中経過データ	初期化
	BCT_CLIENT_MDLSCR_Init( &p_wk->middle_score );

	// タッチパネル
	check = InitTP( BCT_TOUCH_BUFF, BCT_TOUCH_BUFFNUM, 4 );
	GF_ASSERT( check == TP_OK );

    
    return p_wk;
}

//----------------------------------------------------------------------------
/**
 *  @brief  クライアント　破棄
 *
 *  @param  p_wk    ワーク
 */
//-----------------------------------------------------------------------------
void BCT_CLIENT_Delete( BCT_CLIENT* p_wk )
{
	u32 check;
	// タッチパネルサンプリング終了
	check = StopTP();
	GF_ASSERT( check == TP_OK );

	// タッチペンシステム破棄
	BCT_CLIENT_TOUCHPEN_Exit( &p_wk->touchpen_wk, &p_wk->graphic );

    // グラフィック破棄
    BCT_CLIENT_GraphicDelete( p_wk );
    
    sys_FreeMemoryEz( p_wk );

#ifdef PM_DEBUG
	// 食った数を表示
	OS_Printf( "今回の食べた数	%d\n", BCT_DEBUG_in_num );
#endif
}

//----------------------------------------------------------------------------
/**
 *  @brief  クライアント　開始処理
 *
 *  @param  p_wk    ワーク
 *  @param  event   使う人から受ける指示
 *
 *  @retval TRUE    途中
 *  @retval FALSE   終わり
 *
 *  event
 *      BCT_CLIENT_STARTEVENT_COUNT_START   カウントを開始するときに渡してください・
 *
 */
//-----------------------------------------------------------------------------
BOOL BCT_CLIENT_StartMain( BCT_CLIENT* p_wk, u32 event )
{
    BOOL result;
    BOOL ret = TRUE;
	s32 speed;

    // マルノームを常にゆっくり動かす
    if(  p_wk->graphic.start.seq >= BCT_STARTSEQ_WAIT ){
		if( p_wk->gametype.rota_rev ){
			speed = -BCT_START_MARUNOMUROTA;
		}else{
			speed = BCT_START_MARUNOMUROTA;
		}
        BCT_CLIENT_MarunomuRotaYSet( &p_wk->marunomu, p_wk->marunomu.rota + speed );
	}

	// 座標と角度を設定
	BCT_CLIENT_StartSysMarunomuMatrixSet( &p_wk->graphic, &p_wk->marunomu );
	BCT_CLIENT_MarunomuDrawRotaSet( &p_wk->graphic.marunomu, &p_wk->marunomu );
	BCT_CLIENT_MarunomuDrawScaleSet( &p_wk->graphic.marunomu, &p_wk->marunomu );

    switch( p_wk->graphic.start.seq ){
    case BCT_STARTSEQ_TOUCHPEN_START:   // タッチペン開始
		BCT_CLIENT_TOUCHPEN_Start( &p_wk->touchpen_wk );
		p_wk->graphic.start.seq ++;
		break;
		
    case BCT_STARTSEQ_TOUCHPEN_MAIN:   // タッチペンメイン
		result = BCT_CLIENT_TOUCHPEN_Main( &p_wk->touchpen_wk, p_wk );
		if( result == TRUE ){
			p_wk->graphic.start.seq ++;

			// 同期開始
			CommTimingSyncStart(BCT_SYNCID_CLIENT_TOUCHPEN_END);
		}
		break;

	case BCT_STARTSEQ_MARUNOMU_SND:   // 音を鳴らす

		// 同期が完了するまで待つ
		if(!CommIsTimingSync(BCT_SYNCID_CLIENT_TOUCHPEN_END)){
			break;
		}

		p_wk->graphic.start.seq ++;
		p_wk->graphic.start.wait = BCT_START_MARU_MOUTH_OPEN_WAIT;
		break;
		
	case BCT_STARTSEQ_MARUNOMU_OPEN: // マルノームの口が開く待ち

		p_wk->graphic.start.wait--;
		if( p_wk->graphic.start.wait == 50 ){
			Snd_SePlay( BCT_SND_MARUIN3 );	// はまる
		}
		if( p_wk->graphic.start.wait == 20 ){
			Snd_SePlay( BCT_SND_MARUIN2 );	// 口開き
		}
		
		// １回アニメ
		BCT_CLIENT_MarunomuDrawNoLoopMouthAnm_Speed( &p_wk->graphic.marunomu, BCT_MARUNOMU_ANM_STARTSPEED );
		if( p_wk->graphic.start.wait == 0 ){
			BCT_CLIENT_StartSysMarunomuChange( &p_wk->graphic.start, &p_wk->graphic );
			p_wk->graphic.start.seq = BCT_STARTSEQ_WAIT;
		}
		break;

    case BCT_STARTSEQ_WAIT:         // カウントダウン開始待ち
        if( event == BCT_CLIENT_STARTEVENT_COUNT_START ){
            p_wk->graphic.start.seq = BCT_STARTSEQ_TEXTINIT;
        }
        break;

    case BCT_STARTSEQ_TEXTINIT:     // テキストの準備
        BCT_CLIENT_StartSysCountDownInit( &p_wk->graphic.start, &p_wk->graphic );
		Snd_SePlay( BCT_SND_COUNT );
        p_wk->graphic.start.seq = BCT_STARTSEQ_COUNTDOWNWAIT;
        break;
        
    case BCT_STARTSEQ_COUNTDOWNWAIT:    // カウントダウン待ち
        result = BCT_CLIENT_StartSysCountDown( &p_wk->graphic.start, &p_wk->graphic );
        if( result == FALSE ){
            p_wk->graphic.start.seq = BCT_STARTSEQ_GAMESTARTWAIT;
        }
        break;
        
    case BCT_STARTSEQ_GAMESTARTWAIT:    // ゲームが出来る状態になるまで待つ
        result = BCT_CLIENT_StartSysGameStart( &p_wk->graphic.start, &p_wk->graphic );
        if( result == FALSE ){
            p_wk->graphic.start.seq = BCT_STARTSEQ_END;
        }
        break;
        
    case BCT_STARTSEQ_END:          // 開始終わり
        BCT_CLIENT_StartSysDrawOff( &p_wk->graphic.start, &p_wk->graphic ); // 説明とか消す
		// マルノームあたり判定会氏
		BCT_CLIENT_MarunomuSetHitFlag( &p_wk->marunomu, TRUE );
        ret = FALSE;
        break;
        
    }
	// 表示
	BCT_CLIENT_MarunomuDrawAnmMain( &p_wk->graphic.marunomu, &p_wk->marunomu, p_wk->move_type );
	BCT_CLIENT_GraphicDrawCore( p_wk, &p_wk->graphic );
    return ret;
}

//----------------------------------------------------------------------------
/**
 *  @brief  結果発表
 *  
 *  @param  p_wk    ワーク
 *  @param  event   イベント    外から何か指示を受け取るとき
 *
 *  @retval TRUE    途中
 *  @retval FALSE   完了
 */
//-----------------------------------------------------------------------------
BOOL BCT_CLIENT_EndMain( BCT_CLIENT* p_wk, u32 event )
{
    BOOL result;
    BOOL ret = TRUE;
	s32 speed;

    switch( p_wk->graphic.result.seq ){
    case BCT_RESULT_SEQ_ENDINIT:
        // 投げている途中の木の実をすべて人の投げたものにする
        BCT_CLIENT_NutsSeqOtherChange( p_wk );
        // 持っている途中のものも破棄する
        BCT_CLIENT_NutsSlowClear( p_wk );

		// した画面ブライトネスチェンジ
        ChangeBrightnessRequest( BCT_RESULT_ENDBRIGHTNESS_SYNC, 
                BCT_GRA_SUBBRIGHT_OFFNUM, BRIGHTNESS_NORMAL, PLANEMASK_ALL, MASK_SUB_DISPLAY );

		// マルノームのアニメを変更
		BCT_CLIENT_EndSysMarunomuAnmChg( &p_wk->graphic );

		// タイムアップ
		MNGM_COUNT_StartTimeUp( p_wk->graphic.p_countwk );

		// BGMを元に戻す
		BCT_CLIENT_FEVER_EFF_Reset( &p_wk->fever_eff );

		// 木の実カウンターを閉じる
		BCT_CLIENT_NUTS_COUNT_End( &p_wk->graphic.nutscount );

        p_wk->graphic.result.seq ++;
        break;
        
    case BCT_RESULT_SEQ_ENDDRAW:
        result = TRUE;
        // マルノームをROTA９０に戻す
        if( BCT_CLIENT_EndSysMarunomuAnm( &p_wk->graphic ) == FALSE ){
            result = FALSE;
        }

        // ボールを全部動かす（全部はずれ） 
        BCT_CLIENT_AllNutsMove( p_wk );
		BCT_CLIENT_AllNutsEndContAllDel( p_wk );
        if( BCT_CLIENT_NutsMoveCheck( p_wk ) == TRUE ){
            result = FALSE;
        }

        // タイムアップを表示
        if( BCT_CLIENT_EndSysTimeUpAnm( &p_wk->graphic ) == FALSE ){
            result = FALSE;
        }

		// サブ画面ブライトネス
		if( IsFinishedBrightnessChg( MASK_SUB_DISPLAY ) == FALSE ){
			result = FALSE;
		}
        
        // すべての動きが終わって、eventがスコア表示になったら次へ進む
        if( (result == TRUE) && (event == BCT_CLIENT_ENDEVENT_RESULTON) ){
			p_wk->graphic.result.count = 0;
            p_wk->graphic.result.seq++;
        }
        break;

	case BCT_RESULT_SEQ_ENDWAIT:
		// ウエイト
		p_wk->graphic.result.count++;
		if( p_wk->graphic.result.count >= BCT_RESULT_ENDWAIT ){
            p_wk->graphic.result.seq++;
		}
		break;
		
    case BCT_RESULT_SEQ_END:
        ret = FALSE;
        break;
    }

	// 回転
	if( p_wk->gametype.rota_rev ){
		speed = -BCT_START_MARUNOMUROTA;
	}else{
		speed = BCT_START_MARUNOMUROTA;
	}
	BCT_CLIENT_MarunomuRotaYSet( &p_wk->marunomu, p_wk->marunomu.rota + speed );
	// 座標と角度を設定
	BCT_CLIENT_MarunomuDrawMatrixSet( &p_wk->graphic.marunomu, &p_wk->marunomu );
	BCT_CLIENT_MarunomuDrawRotaSet( &p_wk->graphic.marunomu, &p_wk->marunomu );

	// マルノームアニメ
	BCT_CLIENT_MarunomuDrawAnmMain( &p_wk->graphic.marunomu, &p_wk->marunomu, p_wk->move_type );

    //３Ｄ描画開始
	BCT_CLIENT_GraphicDrawCore( p_wk, &p_wk->graphic );

	// 木の実カウンター
	BCT_CLIENT_NUTS_COUNT_Main( &p_wk->graphic.nutscount );
	
    return ret;
}

//----------------------------------------------------------------------------
/**
 *  @brief  メイン関数
 *
 *  @param  p_wk 
 *
 *  @retval TRUE    途中
 *  @retval FALSE   終わり
 */
//-----------------------------------------------------------------------------
BOOL BCT_CLIENT_Main( BCT_CLIENT* p_wk )
{
	BOOL result;
		
	// ゲーム状態チェック
	if( p_wk->move_type < BCT_MARUNOMU_MOVE_NUM-1 ){
		if( sc_BCT_MARUNOMU_MOVE_STARTTIME[ p_wk->move_type+1 ] <= p_wk->time ){
			p_wk->move_type ++;	// 時間が来たら次のゲームタイプへ

			// FEVERチェック
			if( p_wk->move_type == BCT_MARUNOMU_MOVE_FEVER_EFF ){

				// FEVERエフェクト発動
				BCT_CLIENT_FEVER_EFF_Start( &p_wk->fever_eff );
			}
		}
	}
	
	
	// タッチメイン
	MainTP( &p_wk->tp_one, TP_BUFFERING, 0 );
	
    // タッチパネル動作
    BCT_CLIENT_NutsSlow( p_wk );

	// ゲームデータ管理
	{
		result = BCT_CLIENT_ContGameData_Main( &p_wk->gamedata_cont );
		if( result == FALSE ){
			BCT_CLIENT_ContGameData_Reflect( p_wk, &p_wk->gamedata_cont );
		}
	}

	// 途中の得点管理
	{
		u32 top_idx;
		
		result = BCT_CLIENT_MDLSCR_CheckInNum( &p_wk->middle_score, p_wk->comm_num );
		if( result == TRUE ){

			// マルノームの下の絵を変える
			top_idx = BCT_CLIENT_MDLSCR_GetTop( &p_wk->middle_score );
//			OS_TPrintf( "now top %d\n", top_idx );
			if( top_idx != BCT_MDLSCR_TOP_DEF ){
				BCT_CLIENT_MarunomuDrawSetColAnmTop( &p_wk->graphic.marunomu, top_idx );
			}

			// 途中経過を再取得
			BCT_CLIENT_MDLSCR_Reset( &p_wk->middle_score );
		}
	}
    
    // 木の実動作
    BCT_CLIENT_AllNutsMove( p_wk );

    // マルノームを動作させる
    BCT_CLIENT_MarunomuMain( p_wk, &p_wk->marunomu );

    // 描画
    BCT_CLIENT_GraphicMain( p_wk, &p_wk->graphic ); 

    // 木の実終了管理
    BCT_CLIENT_AllNutsEndCont( p_wk );

	// FEVERエフェクト
	BCT_CLIENT_FEVER_EFF_Main( &p_wk->fever_eff, &p_wk->graphic.mainback, &p_wk->graphic.marunomu );
	
#ifndef BCT_DEBUG_NOT_TIMECOUNT
	if( p_wk->time_count_flag == TRUE ){
		
	    p_wk->time ++;
	}
#endif
    if( p_wk->time >= p_wk->time_max ){
		p_wk->time = p_wk->time_max;
        return TRUE;
    }
    return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  VBlank関数
 *
 *  @param  p_wk    ワーク
 */
//-----------------------------------------------------------------------------
void BCT_CLIENT_VBlank( BCT_CLIENT* p_wk )
{
    
    // BG書き換え
    GF_BGL_VBlankFunc( p_wk->graphic.p_bgl );

    // Vram転送マネージャー実行
    DoVramTransferManager();

    // レンダラ共有OAMマネージャVram転送
    REND_OAMTrans();
}

//----------------------------------------------------------------------------
/**
 *	@brief	今の経過時間を取得する
 *
 *	@param	cp_wk	ワーク
 *
 *	@return	時間
 */
//-----------------------------------------------------------------------------
s32	BCT_CLIENT_GetTime( const BCT_CLIENT* cp_wk )
{
	return cp_wk->time;
}

//----------------------------------------------------------------------------
/**
 *  @brief  木の実データの受信
 *
 *  @param  p_wk        ワーク
 *  @param  cp_data     受信データ
 */
//-----------------------------------------------------------------------------
void BCT_CLIENT_NutsDataSet( BCT_CLIENT* p_wk, const BCT_NUT_COMM* cp_data )
{
    BCT_NUT_DATA data;
    
    // 自分のデータではないかチェック
    if( p_wk->plno != cp_data->pl_no ){


        // クライアント木の実データに変換して設定
        BCT_CLIENT_NutsCommToData( cp_data, &data );
        BCT_CLIENT_NutsSet( p_wk, &data, BCT_NUTSSEQ_MOVEOTHER );

        // 食べたならマルノームを食べた動作にする
        if( cp_data->in_flag ){
            // スコアエフェクトを出す
            BCT_CLIENT_ScoreEffectStart( &p_wk->graphic.score_effect, cp_data->pl_no, cp_data->bonus, p_wk->move_type );
        }
    }
}

//----------------------------------------------------------------------------
/**
 *  @brief  木の実データ取得
 *
 *  @param  p_wk        ワーク
 *  @param  p_data      データ
 *
 *  @retval TRUE    まだある
 *  @retval FALSE   もうない
 */
//-----------------------------------------------------------------------------
BOOL BCT_CLIENT_NutsDataGet( BCT_CLIENT* p_wk, BCT_NUT_COMM* p_data )
{
    int i;

    for( i=0; i<BCT_NUTSBUFF_NUM; i++ ){
        if( p_wk->nuts[ i ].seq == BCT_NUTSSEQ_END ){
            // 通信データに変換して設定
            BCT_CLIENT_NutsDataToComm( &p_wk->nuts[i].data, p_data );
            BCT_CLIENT_NutsDel( &p_wk->nuts[ i ] );
            return TRUE;
        }
    }
    return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  みんなのスコアを設定する
 *
 *  @param  p_wk        ワーク
 *  @param  cp_data     データ
 */
//-----------------------------------------------------------------------------
void BCT_CLIENT_AllScoreSet( BCT_CLIENT* p_wk, const BCT_SCORE_COMM* cp_data )
{
    p_wk->all_score = *cp_data;
}

//----------------------------------------------------------------------------
/**
 *  @brief  自分のスコアを返す
 *
 *  @param  cp_wk       ワーク
 *
 *  @return スコア
 */
//-----------------------------------------------------------------------------
u32 BCT_CLIENT_ScoreGet( const BCT_CLIENT* cp_wk )
{
    return cp_wk->score;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ゲームデータインデックス	設定
 *
 *	@param	p_wk	ワーク
 *	@param	idx		インデックス
 */
//-----------------------------------------------------------------------------
void BCT_CLIENT_GameDataIdxSet( BCT_CLIENT* p_wk, u32 idx )
{
	p_wk->gamedata_last	= p_wk->gamedata_now;
	p_wk->gamedata_now	= idx;

	// データ変更をリクエスト
	BCT_CLIENT_ContGameData_Start( &p_wk->gamedata_cont, p_wk->cp_gamedata, &p_wk->gametype, p_wk->gamedata_last, p_wk->gamedata_now );
}

//----------------------------------------------------------------------------
/**
 *	@brief	途中経過スコアを設定する
 *
 *	@param	p_wk		ワーク
 *	@param	score		スコア
 *	@param	plno		プレイヤーナンバー
 */
//-----------------------------------------------------------------------------
void BCT_CLIENT_MiddleScoreSet( BCT_CLIENT* p_wk, u32 score, u32 plno )
{
	BCT_CLIENT_MDLSCR_Set( &p_wk->middle_score, score, plno );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ゲームタイプを取得する
 *
 *	@param	p_wk		ワーク
 *	@param	cp_data		ゲームタイプ
 */
//-----------------------------------------------------------------------------
void BCT_CLIENT_GameTypeSet( BCT_CLIENT* p_wk, const BCT_GAME_TYPE_WK* cp_data )
{
	p_wk->gametype			= *cp_data;
	p_wk->gametype_flag		= TRUE;


	//  ゲームデータ変更システム初期化
	BCT_CLIENT_ContGameData_Init( &p_wk->gamedata_cont, p_wk->cp_gamedata, &p_wk->gametype );
	// ゲーム初期データ設定
	BCT_CLIENT_ContGameData_Reflect( p_wk, &p_wk->gamedata_cont );

}

//----------------------------------------------------------------------------
/**
 *	@brief	ゲームタイプ設定済みかチェック
 *
 *	@param	p_wk		ワーク
 *		
 *	@retval	TRUE	設定済み
 *	@retval	FALSE	設定してない
 */
//-----------------------------------------------------------------------------
BOOL BCT_CLIENT_GameTypeSetCheck( const BCT_CLIENT* cp_wk )
{
	return cp_wk->gametype_flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief	タイムカウントフラグを設定する
 *
 *	@param	p_wk		ワーク
 *	@param	flag		フラグ
 */
//-----------------------------------------------------------------------------
void BCT_CLIENT_TimeCountFlagSet( BCT_CLIENT* p_wk, BOOL flag )
{
	p_wk->time_count_flag = flag;
}






//-----------------------------------------------------------------------------
/**
 *          プライベート関数
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	加速動作汎用　初期化
 *
 *	@param	p_work	ワーク
 *	@param	s_x		開始座標
 *	@param	e_x		終了座標
 *	@param	s_s		開始速度
 *	@param	count_max ｶｳﾝﾀ値
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void BCT_AddMoveReqFx( BCT_ADDMOVE_WORK* p_work, fx32 s_x, fx32 e_x, fx32 s_s, int count_max )
{
	fx32 t_x_t;	// タイムの２乗
	fx32 vot;	// 初速度＊タイム
	fx32 dis;
	fx32 a;

	dis = e_x - s_x;
	
	// 加速値を求める
	// a = 2(x - vot)/(t*t)
	t_x_t = (count_max * count_max) << FX32_SHIFT;
	vot = FX_Mul( s_s, count_max * FX32_ONE );
	vot = dis - vot;
	vot = FX_Mul( vot, 2*FX32_ONE );
	a = FX_Div( vot, t_x_t );

	p_work->x = s_x;
	p_work->s_x = s_x;
	p_work->s_s = s_s;
	p_work->s_a = a;
	p_work->count = 0;
	p_work->count_max = count_max;
}
//----------------------------------------------------------------------------
/**
 *	@brief	加速動作メイン
 *
 *	@param	p_work	ワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
static BOOL	BCT_AddMoveMainFx( BCT_ADDMOVE_WORK* p_work )
{
	fx32 dis;
	fx32 t_x_t;
	fx32 calc_work;
	fx32 vot;
	
	// 等加速度運動
	// dis = vot + 1/2( a*(t*t) )
	vot = FX_Mul( p_work->s_s, p_work->count << FX32_SHIFT );
	t_x_t = (p_work->count * p_work->count) << FX32_SHIFT;
	calc_work = FX_Mul( p_work->s_a, t_x_t );
	calc_work = FX_Div( calc_work, 2*FX32_ONE );	// 1/2(a*(t*t))
	dis = vot + calc_work;	///<移動距離

	p_work->x = p_work->s_x + dis;


	if( (p_work->count + 1) <= p_work->count_max ){
		p_work->count++;
		return FALSE;
	}

	p_work->count = p_work->count_max;
	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ネットIDに対応した２Dスコア表示位置を取得する
 *
 *  @param  comm_num        通信人数
 *  @param  myplno         自分のNETID
 *  @param  targetplno     座標がほしい人のNETID
 *  @param  p_x             ｘ座標格納先
 *  @param  p_y             ｙ座標格納先
 */
//-----------------------------------------------------------------------------
static void BCT_netID2DMatrixGet( u32 comm_num, u32 myplno, u32 targetplno, s32* p_x,  s32* p_y )
{

	//											// plno				commnum			 draw plno
	static const Vec2DS32 sc_BCT_2DMAT_TBL[ BCT_PLAYER_NUM ][ BCT_PLAYER_NUM ][ BCT_PLAYER_NUM ] = {
		// 自分のPLNOが0
		{	
			// 自分だけのときはない
			{0},
			// 2人対戦
			{
				{128,144},
				{128,48},
			},
			// 3人対戦
			{
				{128,144},
				{208,64},
				{48,64},
			},
			// 4人対戦
			{
				{128,144},
				{128,48},
				{208,96},
				{48,96},
			},
		},

		// 自分のPLNOが1
		{	
			// 自分だけのときはない
			{0},
			// 2人対戦
			{
				{128,48},
				{128,144},
			},
			// 3人対戦
			{
				{48,64},
				{128,144},
				{208,64},
			},
			// 4人対戦
			{
				{128,48},
				{128,144},
				{48,96},
				{208,96},
			},
		},

		// 自分のPLNOが2
		{	
			// 自分だけのときはない
			{0},
			// 2人なし
			{
				{0},
			},
			// 3人対戦
			{
				{208,64},
				{48,64},
				{128,144},
			},
			// 4人対戦
			{
				{48,96},
				{208,96},
				{128,144},
				{128,48},
			},
		},

		// 自分のPLNOが3
		{	
			// 自分だけのときはない
			{0},
			// 2人なし
			{
				{0},
			},
			// 3人なし
			{
				{0,0},
			},
			// 4人対戦
			{
				{208,96},
				{48,96},
				{128,48},
				{128,144},
			},
		},
	};

    *p_x = sc_BCT_2DMAT_TBL[ myplno ][ comm_num-1 ][ targetplno ].x;
    *p_y = sc_BCT_2DMAT_TBL[ myplno ][ comm_num-1 ][ targetplno ].y;
}


//----------------------------------------------------------------------------
/**
 *	@brief	簡単木の実発射システム
 *
 *	@param	p_wk		クライアントワーク
 *	@param	t0_x		タッチ始点	座標
 *	@param	t0_y
 *	@param	t1_x		タッチ終点　座標
 *	@param	t1_y
 *	@pram	moveseq		動作シーケンス
 */
//-----------------------------------------------------------------------------
static void BCT_EasyNutsSet( BCT_CLIENT* p_wk, u16 t0_x, u16 t0_y, u16 t1_x, u16 t1_y, u32 moveseq )
{
    BCT_NUT_DATA data;
    NNSG2dSVec2 vec;
    BOOL result;
    s32 vec_num;
	BOOL miss_slow = FALSE;
	fx32 xz_power;

	
	//  投げデータを作成する
	data.plno = p_wk->plno;
	data.in_flag = FALSE;

	// 投げた方向を求める
	vec.x = (t0_x - t1_x) / 2;
	vec.y = (t0_y - t1_y) / 2;

        
	data.way.x = vec.x << FX32_SHIFT;
	data.way.z = vec.y << FX32_SHIFT;
	data.way.y = 0;
	xz_power = VEC_Mag( &data.way );	// いったん平面状のパワーを計算


	// 力の値調節
	if( xz_power > BCT_NUTS_POWER_MAX ){
		// 力を調節して木の実設定
		VEC_Normalize( &data.way, &data.way );
		data.way.x = FX_Mul( data.way.x, BCT_NUTS_POWER_MAX );
		data.way.z = FX_Mul( data.way.z, BCT_NUTS_POWER_MAX );
	}

	data.way.y = BCT_NUTS_POWRT_YPOWER;
	data.power = VEC_Mag( &data.way );
	data.power = FX_Mul( data.power, BCT_NUTS_POWER_MUL );  // パワー増幅
	VEC_Normalize( &data.way, &data.way );

	// 座標位置、投げる方向をNETIDと関連づける
	// 座標はタッチしている位置によって変わる
	BCT_CLIENT_2DMATto3DMAT( t1_x, t1_y, &data.mat );
	data.mat.y = BCT_FIELD_PLAYER_Y;


	// 方向と座標をNETIDに関連付ける
	BCT_CLIENT_VecNetIDRot( &data.mat, &data.mat, data.plno, p_wk->comm_num );
	BCT_CLIENT_VecNetIDRot( &data.way, &data.way, data.plno, p_wk->comm_num );

	data.special = FALSE;
	
	// 登録
	BCT_CLIENT_NutsSet( p_wk, &data, moveseq );
}

//----------------------------------------------------------------------------
/**
 *  @brief  スコアに足す値を取得する
 *
 *  @param  cp_wk   ワーク
 *
 *  @return スコアに足す値
 */
//-----------------------------------------------------------------------------
static u32 BCT_AddScoreGet( BCT_CLIENT* cp_wk )
{
    u8 idx;
	u32 score;

    if( cp_wk->bonus >= BCT_MARUNOMU_SCORE_TYPENUM ){
        idx = BCT_MARUNOMU_SCORE_TYPENUM - 1;
    }else{
        idx = cp_wk->bonus;
    }
//	OS_Printf( "BCT_SCORE_NUM[ idx ] %d\n", BCT_SCORE_NUM[ idx ] );
	score = BCT_SCORE_NUM[ idx ];

	// FEVERチェック
	if( cp_wk->move_type == BCT_MARUNOMU_MOVE_FEVER ){
		score *= BCT_MARUNOMU_SCORE_FEVER_MUL;
	}
    return score;
}

//----------------------------------------------------------------------------
/**
 *  @brief  スコアエフェクトシステム    初期化
 *
 *  @param  p_wk            ワーク
 *  @param  p_drawsys       描画システム
 *  @param  comm_num        通信人数
 *  @param	plno			通信ID
 *  @param  heapID          ヒープID
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_ScoreEffectInit( BCT_CLIENT_SCORE_EFFECT* p_wk, BCT_CLIENT_GRAPHIC* p_drawsys, u32 comm_num, u32 plno, u32 heapID )
{
    CLACT_ADD add;
    int i, j;
    u32 myplno;

    add.ClActSet    = p_drawsys->clactSet;  // セルアクターセット
    add.ClActHeader = &p_drawsys->mainoamheader;// ヘッダーデータ
    add.sca.x       = FX32_ONE;     // 拡大値
    add.sca.y       = FX32_ONE;     // 拡大値
    add.sca.z       = FX32_ONE;     // 拡大値
    add.rot         = 0;        // 回転     (0～65535)
    add.pri         = 0;        // 優先順位
    add.DrawArea    = NNS_G2D_VRAM_TYPE_2DMAIN; // 描画面
    add.heap        = heapID;       // 使用するヒープ

    // 通信人数分追加
    myplno = plno;
    for( i=0; i<BCT_PLAYER_NUM; i++ ){
		BCT_CLIENT_ScoreEffectWkInit( &p_wk->wk[i], &add, p_drawsys, comm_num, i, myplno, heapID );
    }
}

//----------------------------------------------------------------------------
/**
 *  @brief  スコアエフェクトシステム    破棄
 *
 *  @param  p_wk        ワーク
 *  @param  p_drawsys   描画システム
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_ScoreEffectExit( BCT_CLIENT_SCORE_EFFECT* p_wk, BCT_CLIENT_GRAPHIC* p_drawsys)
{
    int i, j;

    for( i=0; i<BCT_PLAYER_NUM; i++ ){
		BCT_CLIENT_ScoreEffectWkExit( &p_wk->wk[i] );
    }
}

//----------------------------------------------------------------------------
/**
 *  @brief  スコアエフェクト開始
 *
 *  @param  p_wk    ワーク
 *  @param  plno   ネットID
 *  @param  bonus   ボーナスナンバー
 *  @param	movetype動作タイプ
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_ScoreEffectStart( BCT_CLIENT_SCORE_EFFECT* p_wk, u32 plno, u32 bonus, BCT_MARUNOMU_MOVE_TYPE movetype )
{
	BCT_CLIENT_ScoreEffectWkStart( &p_wk->wk[plno], plno, bonus, movetype );
}

//----------------------------------------------------------------------------
/**
 *	@brief	スコアエフェクトメイン
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_ScoreEffectMain( BCT_CLIENT_SCORE_EFFECT* p_wk )
{
	int i;

    for( i=0; i<BCT_PLAYER_NUM; i++ ){
		BCT_CLIENT_ScoreEffectWkMain( &p_wk->wk[i] );
    }
}

//----------------------------------------------------------------------------
/**
 *	@brief	スコアエフェクトワーク	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_ad		アッドデータ
 *	@param	p_drawsys	描画システム
 *	@param	comm_num	通信人数
 *	@param	plno		ネットID
 *	@param	myplno		自分おNETID
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_ScoreEffectWkInit( BCT_CLIENT_SCORE_EFFECT_WK* p_wk, CLACT_ADD* p_ad, BCT_CLIENT_GRAPHIC* p_drawsys, u32 comm_num, u32 plno, u32 myplno, u32 heapID )
{
	int i;
    s32 x, y;

	for( i=0; i<BCT_SCORE_EFFECT_BUF; i++ ){
		BCT_netID2DMatrixGet( comm_num, myplno, plno, &x, &y );
		p_ad->mat.x = x << FX32_SHIFT; 
		p_ad->mat.y = y << FX32_SHIFT; 
		p_wk->mat[i] = p_ad->mat;
		p_wk->p_clwk[i] = CLACT_Add( p_ad );

		// アニメ設定
		CLACT_AnmChg( p_wk->p_clwk[i], BCT_GRA_OAMMAIN_ANM_100 );

		// 表示OFF
		CLACT_SetDrawFlag( p_wk->p_clwk[i], FALSE );

		// オートアニメ設定
		CLACT_SetAnmFlag( p_wk->p_clwk[i], TRUE );
		CLACT_SetAnmFrame( p_wk->p_clwk[i], FX32_CONST(1.5) );

		// 動作カウンタ
		p_wk->count[i] = 0;
	}

	// 自分のエフェクトかチェック
	if( myplno == plno ){
		p_wk->mydata = TRUE;
	}else{
		p_wk->mydata = FALSE;
	}

}

//----------------------------------------------------------------------------
/**
 *	@brief	破棄処理
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_ScoreEffectWkExit( BCT_CLIENT_SCORE_EFFECT_WK* p_wk )
{
	int i;

	for( i=0; i<BCT_SCORE_EFFECT_BUF; i++ ){
		CLACT_Delete( p_wk->p_clwk[i] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメ開始
 *
 *	@param	p_wk	ワーク
 *	@param	plno	プレイヤーナンバー
 *	@param	bonus	ボーナス
 *	@param	movetype動作タイプ
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_ScoreEffectWkStart( BCT_CLIENT_SCORE_EFFECT_WK* p_wk, u32 plno, u32 bonus, BCT_MARUNOMU_MOVE_TYPE movetype )
{
	int i;
	int idx;
	BOOL clean_in;
	u32 pri;
	u32 max_pri;
	u32 anm_seq;
	u32 palno;

	// 空いてるところを探しながら
	// みんなのプライオリティを１つ下げる
	// 空きが無いならプライオリティが最大のワークを使う
	clean_in = FALSE;
	max_pri = 0;
	idx = -1;
	for( i=0; i<BCT_SCORE_EFFECT_BUF; i++ ){
		if( CLACT_GetDrawFlag( p_wk->p_clwk[i] ) == FALSE ){
			idx = i;	// これを使う
			clean_in = TRUE;
		}else{
			// 優先順位を１つ下げる
			pri = CLACT_DrawPriorityGet( p_wk->p_clwk[i] );
			pri ++;
			CLACT_DrawPriorityChg( p_wk->p_clwk[i], pri );

			// 空きワークが見つかってないなら
			// 最大プライオリティワークを設定
			if( (clean_in == FALSE) && (max_pri <= pri) ){
				max_pri = pri;
				idx = i;
			}
		}
	}
	GF_ASSERT( idx >= 0 );	// 発見できない

	if( bonus >= BCT_MARUNOMU_SCORE_TYPENUM ){
		bonus = BCT_MARUNOMU_SCORE_TYPENUM - 1;
	}

	// データ設定
	if( movetype != BCT_MARUNOMU_MOVE_FEVER ){
		if( p_wk->mydata == TRUE ){
			anm_seq = bonus + BCT_GRA_OAMMAIN_ANM_100;
		}else{
			anm_seq = bonus + BCT_GRA_OAMMAIN_ANM_100_OTHER;
		}
		palno	= BCT_GRA_OAMMAIN_PAL_SCORE0+plno;
	}else{
		if( p_wk->mydata == TRUE ){
			anm_seq = bonus + BCT_GRA_OAMMAIN_ANM_FE100;
		}else{
			anm_seq = bonus + BCT_GRA_OAMMAIN_ANM_FE100_OTHER;
		}
		palno	= BCT_GRA_OAMMAIN_PAL_SCOREEX;
	}
	CLACT_AnmChg( p_wk->p_clwk[idx], anm_seq );
	p_wk->count[idx] = 0;
	CLACT_SetDrawFlag( p_wk->p_clwk[idx], TRUE );	// 描画開始
	BCT_CLIENT_ScoreEffectWkSetMatrix( p_wk, idx );	// 座標
	CLACT_DrawPriorityChg( p_wk->p_clwk[idx], BCT_SCORE_EFFECT_PRI_START );	// 優先順位
	CLACT_PaletteNoChg( p_wk->p_clwk[idx], palno );	// パレット

}

//----------------------------------------------------------------------------
/**
 *	@brief	動作メイン
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_ScoreEffectWkMain( BCT_CLIENT_SCORE_EFFECT_WK* p_wk )
{
	int i;

	for( i=0; i<BCT_SCORE_EFFECT_BUF; i++ ){
		if( CLACT_GetDrawFlag( p_wk->p_clwk[i] ) == TRUE ){
			p_wk->count[i] ++;
			if( p_wk->count[i] > BCT_SCORE_EFFECT_MOVE_COUTN ){
				// 破棄
				BCT_CLIENT_ScoreEffectWkEnd( p_wk, i );
			}else{
				BCT_CLIENT_ScoreEffectWkSetMatrix( p_wk, i );
			}
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	座標設定
 *
 *	@param	p_wk		ワーク
 *	@param	idx			インデックス
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_ScoreEffectWkSetMatrix( BCT_CLIENT_SCORE_EFFECT_WK* p_wk, u32 idx )
{
	fx32 move_y;
	VecFx32 mat;

	move_y = FX_Div( FX_Mul( FX32_ONE * p_wk->count[idx], BCT_SCORE_EFFECT_MOVE_Y ), FX32_CONST(BCT_SCORE_EFFECT_MOVE_COUTN) );

	mat = p_wk->mat[idx];
	mat.y += move_y;
	CLACT_SetMatrix( p_wk->p_clwk[idx], &mat );
}

//----------------------------------------------------------------------------
/**
 *	@brief	エフェクトを消す
 *
 *	@param	p_wk	ワーク
 *	@param	idx		インデックス
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_ScoreEffectWkEnd( BCT_CLIENT_SCORE_EFFECT_WK* p_wk, u32 idx )
{
	CLACT_SetDrawFlag( p_wk->p_clwk[idx], FALSE );
}




//----------------------------------------------------------------------------
/**
 *  @brief  開始アニメ  初期化
 *
 *  @param  p_graphic       グラフィックワーク
 *  @param  p_drawsys       描画システム
 *  @param	p_handle		ハンドル
 *  @param  heapID          ヒープID
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_StartSysInit( BCT_COUNTDOWN_DRAW* p_graphic, BCT_CLIENT_GRAPHIC* p_drawsys, const BCT_GAMEDATA* cp_param, u32 commnum, u32 myplno, ARCHANDLE* p_handle, u32 heapID )
{
    STRBUF* p_str;
    CLACT_ADD add;

    memset( p_graphic, 0, sizeof(BCT_COUNTDOWN_DRAW) );

    // メッセージウィンドウ作成
    GF_BGL_BmpWinAdd( p_drawsys->p_bgl , &p_graphic->helpwin, GF_BGL_FRAME1_M,
            BCT_GRA_STARTWIN_X, BCT_GRA_STARTWIN_Y,
            BCT_GRA_STARTWIN_SIZX, BCT_GRA_STARTWIN_SIZY, 
            BCT_GRA_BGMAIN_PAL_FONT, BCT_GRA_STARTWIN_CGX );

    GF_BGL_BmpWinFill( &p_graphic->helpwin, 15, 0, 0, 
            BCT_GRA_STARTWIN_SIZX*8, BCT_GRA_STARTWIN_SIZY*8 );

    // メッセージを書き込む
    p_str = STRBUF_Create( BCT_STRBUF_NUM, heapID );
    MSGMAN_GetString( p_drawsys->p_msgman, msg_a_001, p_str );
    GF_STR_PrintColor( &p_graphic->helpwin, FONT_SYSTEM, p_str, 
            BCT_GRA_STARTWIN_MSGX, BCT_GRA_STARTWIN_MSGY,
            MSG_NO_PUT, BCT_COL_N_BLACK, NULL);
    STRBUF_Delete( p_str );


	// 名前スクリーン読み込み
	ArcUtil_HDL_BgCharSet( p_handle, NARC_bucket_ent_win_bg_NCGR, 
			p_drawsys->p_bgl, GF_BGL_FRAME2_M, 0, 0, FALSE, heapID );
	ArcUtil_HDL_ScrnSet( p_handle, NARC_bucket_ent_win_bg02_NSCR+(commnum-2),
			p_drawsys->p_bgl, GF_BGL_FRAME2_M, 0, 0, FALSE, heapID);
	ArcUtil_HDL_PalSet( p_handle, NARC_bucket_ent_win_bg_NCLR,
			PALTYPE_MAIN_BG, BCT_GRA_BGMAIN_PAL_NAME_PL00*32, (BCT_GRA_BGMAIN_PAL_NAME_PL03+1)*32,
			heapID );


	// 名前書き込み用ビットマップ作成
	{
		int i;
		s32 name_x, name_y;
		GF_BGL_BMPWIN namebmpwin;
		u32 namebmp_cgx;
		STRBUF* p_namestr;
		u32 col;
		u32 namestrsize;
		u32 draw_x;

		GF_BGL_BmpWinInit( &namebmpwin );
		GF_BGL_BmpWinAdd( p_drawsys->p_bgl, &namebmpwin, GF_BGL_FRAME2_M,
						0, 0, BCT_START_NAME_BMP_WINSIZ_X, BCT_START_NAME_BMP_WINSIZ_Y, 
						BCT_GRA_BGMAIN_PAL_FONT, BCT_START_NAME_BMP_WINCGX_START );
		namebmp_cgx = BCT_START_NAME_BMP_WINCGX_START;

		p_namestr = STRBUF_Create( BCT_START_NAME_STRBUF_NUM, heapID );

		for( i=0; i<commnum; i++ ){
			// 自分の名前はださない
			if( i != myplno ){
				name_x = sc_BCT_START_NAME_TBL[ myplno ][ commnum-1 ][ i ].x;
				name_y = sc_BCT_START_NAME_TBL[ myplno ][ commnum-1 ][ i ].y;
//				OS_TPrintf( "my_plno=%d comm_num=%d plno=%d name_x=%d name_y=%d\n", myplno, commnum, i, name_x, name_y );
				// 名前の書き込みとフレームカラー変更
				GF_BGL_ScrPalChange( p_drawsys->p_bgl, GF_BGL_FRAME2_M, name_x-1, name_y-1,
						BCT_START_NAME_FRAMESIZ_X, BCT_START_NAME_FRAMESIZ_Y, BCT_GRA_BGMAIN_PAL_NAME_PL00+i );

				// 名前書き込み
				GF_BGL_BmpWinDataFill( &namebmpwin, 15 );
				if( cp_param->vip[i] == TRUE ){	// 文字列カラー決定
					col = BCT_COL_N_BLUE;
				}else{
					col = BCT_COL_N_BLACK;
				}
				MyStatus_CopyNameString( cp_param->cp_status[i], p_namestr );	// 名前取得
				GF_BGL_BmpWinSet_PosX( &namebmpwin, name_x );	// 位置設定
				GF_BGL_BmpWinSet_PosY( &namebmpwin, name_y );
				namebmpwin.chrofs = namebmp_cgx;				// cgx設定
				namestrsize = FontProc_GetPrintStrWidth( FONT_SYSTEM, p_namestr, 0 );	// 表示位置設定
				draw_x		= ((BCT_START_NAME_BMP_WINSIZ_X*8) - namestrsize) / 2;	// 中央表示
				GF_STR_PrintColor( &namebmpwin, FONT_SYSTEM, p_namestr, 
						draw_x, 0,
						MSG_ALLPUT, col, NULL);

				namebmp_cgx += BCT_START_NAME_BMP_WINCGX_ONENUM;
				
			}
		}

		STRBUF_Delete( p_namestr );

		GF_BGL_BmpWinDel( &namebmpwin );
	}


    // した画面を薄暗くしておく
//    SetBrightness( BCT_GRA_SUBBRIGHT_OFFNUM, PLANEMASK_ALL, MASK_SUB_DISPLAY );


	// カウンタ郡
    p_graphic->count = 0;


    GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
    GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
}

//----------------------------------------------------------------------------
/**
 *  @brief  開始画面グラフィック破棄
 *
 *  @param  p_graphic       グラフィックワーク
 *  @param  p_drawsys       表示システムワーク
 */ 
//-----------------------------------------------------------------------------
static void BCT_CLIENT_StartSysExit( BCT_COUNTDOWN_DRAW* p_graphic, BCT_CLIENT_GRAPHIC* p_drawsys )
{
    // BMP破棄
    GF_BGL_BmpWinDel( &p_graphic->helpwin );
}

//----------------------------------------------------------------------------
/**
 *  @brief  カウントダウン処理初期化
 *
 *  @param  p_graphic       ワーク
 *  @param  p_drawsys       描画システム
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_StartSysCountDownInit( BCT_COUNTDOWN_DRAW* p_graphic, BCT_CLIENT_GRAPHIC* p_drawsys )
{
    // グラフィックの表示ON
    BmpMenuWinWrite(&p_graphic->helpwin, WINDOW_TRANS_ON,
        BCT_GRA_SYSWND_CGX, BCT_GRA_BGMAIN_PAL_SYSWND );

    GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
    GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );

	//  カウントダウン開始
	MNGM_COUNT_StartStart( p_drawsys->p_countwk );
}

//----------------------------------------------------------------------------
/**
 *  @brief  カウントダウン
 *
 *  @param  p_graphic       ワーク
 *  @param  p_drawsys       表示システム
 *
 *  @retval TRUE    途中
 *  @retval FALSE   完了
 */
//-----------------------------------------------------------------------------
static BOOL BCT_CLIENT_StartSysCountDown( BCT_COUNTDOWN_DRAW* p_graphic, BCT_CLIENT_GRAPHIC* p_drawsys )
{
    BOOL result;
	
    result = MNGM_COUNT_Wait( p_drawsys->p_countwk );
    if( result == TRUE ){
        return FALSE;
    }
    return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  開始画面表示をOFFする
 *
 *  @param  p_graphic       ワーク
 *  @param  p_drawsys       描画システム
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_StartSysDrawOff( BCT_COUNTDOWN_DRAW* p_graphic, BCT_CLIENT_GRAPHIC* p_drawsys )
{
    BmpMenuWinClear( &p_graphic->helpwin, WINDOW_TRANS_OFF );
    GF_BGL_BmpWinOffVReq( &p_graphic->helpwin );

    GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );

}

//----------------------------------------------------------------------------
/**
 *	@brief	マルノーム変更処理
 *
 *	@param	p_graphic		
 *	@param	p_drawsys 
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_StartSysMarunomuChange( BCT_COUNTDOWN_DRAW* p_graphic, BCT_CLIENT_GRAPHIC* p_drawsys )
{

	// 通常アニメ
	BCT_CLIENT_MarunomuDrawSetMouthAnm( &p_drawsys->marunomu, BCT_MARUNOMU_ANM_ROTA );

	// 角度は全快
	D3DOBJ_AnmSet( &p_drawsys->marunomu.anm[ BCT_MARUNOMU_ANM_ROTA ], BCT_MARUNOMU_ANM_FRAME_MAX );

	// 歩きアニメ設定
	BCT_CLIENT_MarunomuDrawSetWalkAnm( &p_drawsys->marunomu, TRUE );
}

//----------------------------------------------------------------------------
/**
 *  @brief  開始画面    マルノームの口をあけたり　した画面の半透明を解除
 *
 *  @param  p_graphic   ワーク
 *  @param  p_drawsys   描画システム
 *
 *  @retval TRUE    途中
 *  @retval FALSE   完了
 */
//-----------------------------------------------------------------------------
static BOOL BCT_CLIENT_StartSysGameStart( BCT_COUNTDOWN_DRAW* p_graphic, BCT_CLIENT_GRAPHIC* p_drawsys )
{
/*
    // ブライトネスフェード開始
    if( p_graphic->count == 0 ){
        ChangeBrightnessRequest( BCT_START_BRIGHTNESS_COUNT, 
                BRIGHTNESS_NORMAL, BCT_GRA_SUBBRIGHT_OFFNUM, PLANEMASK_ALL, MASK_SUB_DISPLAY );
    }
*/
    
    p_graphic->count ++;

	if( (p_graphic->count >= BCT_START_BRIGHTNESS_COUNT) ){
		return FALSE;
	}

    return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	開始画面３Dマルノーム位置設定
 *
 *	@param	p_drawsys
 *	@param	cp_data 
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_StartSysMarunomuMatrixSet( BCT_CLIENT_GRAPHIC* p_drawsys, const BCT_MARUNOMU* cp_data )
{

    // 座標を設定
	BCT_CLIENT_MarunomuDrawMatrixSet( &p_drawsys->marunomu, cp_data );
}

//----------------------------------------------------------------------------
/**
 *  @brief  結果発表    初期化
 *
 *  @param  p_graphic   グラフィック
 *  @param  comm_num    通信人数
 *  @param	plno		通信ID
 *  @param  heapID      ヒープID
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_EndSysInit( BCT_CLIENT_GRAPHIC* p_graphic, u32 comm_num, u32 plno, u32 heapID )
{
    p_graphic->result.seq = 0;
    p_graphic->result.count = 0;

}

//----------------------------------------------------------------------------
/**
 *  @brief  結果発表    破棄
 *
 *  @param  p_graphic   グラフィック
 *  @param  comm_num    通信人数
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_EndSysExit( BCT_CLIENT_GRAPHIC* p_graphic, u32 comm_num )
{
}

//----------------------------------------------------------------------------
/**
 *	@brief	マルノームのアニメを変更する
 *
 *	@param	p_graphic	グラフィックワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_EndSysMarunomuAnmChg( BCT_CLIENT_GRAPHIC* p_graphic )
{
	// マルノームのアニメを変更
	BCT_CLIENT_MarunomuDrawSetMouthAnm( &p_graphic->marunomu, BCT_MARUNOMU_ANM_CLOSE );
}

//----------------------------------------------------------------------------
/**
 *  @brief  マルノーム口とじアニメ
 *
 *  @param  p_graphic   ワーク
 *
 *  @retval TRUE    完了
 *  @retval FALSE   途中
 */
//-----------------------------------------------------------------------------
static BOOL BCT_CLIENT_EndSysMarunomuAnm( BCT_CLIENT_GRAPHIC* p_graphic )
{
	BOOL result;
	
    p_graphic->result.count ++;
    // BCT_RESULT_MARUNOMU_MOVE_TIMINGまで待機
    // anmtimeの値でアニメさせる
    if( p_graphic->result.count >= BCT_RESULT_MARUNOMU_MOVE_TIMING ){
		result = D3DOBJ_AnmNoLoop( &p_graphic->marunomu.anm[BCT_MARUNOMU_ANM_CLOSE],
				BCT_MARUNOMU_ANM_SPEED );	
        if( result == TRUE ){
            return TRUE;
        }
    }
    return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  マルノーム  タイムアップアニメ
 *
 *  @param  p_graphic   ワーク
 *
 *  @retval TRUE    完了
 *  @retval FALSE   途中
 */
//-----------------------------------------------------------------------------
static BOOL BCT_CLIENT_EndSysTimeUpAnm( BCT_CLIENT_GRAPHIC* p_graphic )
{
    BOOL result;

    result = MNGM_COUNT_Wait( p_graphic->p_countwk );
    if( result == TRUE ){
        return TRUE;
    }
    return FALSE;
}


//----------------------------------------------------------------------------
/**
 *  @brief  マルノームワークを初期化
 *  
 *  @param  cp_wk   ワーク
 *  @param  p_maru  マルノームワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MarunomuInit( const BCT_CLIENT* cp_wk, BCT_MARUNOMU* p_maru )
{
    // スピード設定
    p_maru->speed = 0;

    // 回転確度
    BCT_CLIENT_MarunomuRotaYSet( p_maru, BCT_MARUNOMU_ROTA_INIT );

    // X軸の回転角度設定
    BCT_CLIENT_MarunomuRotaXSet( p_maru, BCT_MARUNOMU_ROTA_X_INIT );

	// 当たり判定関係フラグ
	p_maru->hit			= FALSE;	// 当たり判定なし
	p_maru->eat_flag	= TRUE;		// 食べるフラグON

    // 引力の力
    p_maru->inryoku_area = BCT_MARUNOMU_INRYOKU_DIST;
    p_maru->inryoku_power = BCT_MARUNOMU_INRYOKU_POWER_PAR;

	// 口の大きさ
    p_maru->mouth_size = BCT_MARUNOMU_HIT_DIS_MAX;

	// 基本データ
	p_maru->marunomu_mat	= sc_MARUNOMU_MAT;

	// 拡大縮小
	BCT_CLIENT_MaruScaleSet( p_maru, BCT_MARUNOMU_SCALE_INIT );

	// 座標
	p_maru->matrix = sc_MARUNOMU_MAT;
	p_maru->height = p_maru->hitbody_y_min;
}

//----------------------------------------------------------------------------
/**
 *  @brief  マルノーム動作メイン
 *
 *  @param  cp_wk       ワーク
 *  @param  p_maru      マルノームワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MarunomuMain( const BCT_CLIENT* cp_wk, BCT_MARUNOMU* p_maru )
{
	static u16 (* const cp_func[ BCT_MARUNOMU_MOVE_NUM ])( const BCT_MARUNOMU* cp_maru, s32 time, s32 max_time ) = {
		BCT_CLIENT_MarunomuGetRotaX_Easy,
		BCT_CLIENT_MarunomuGetRotaX_FeverEff,
		BCT_CLIENT_MarunomuGetRotaX_Fever,
	};
    u16 rota_x; // x軸回転角度

	// 食べるフラグ設定
	switch( cp_wk->move_type ){
	// FEVER前エフェクト
	case BCT_MARUNOMU_MOVE_FEVER_EFF:	
		p_maru->eat_flag = FALSE;
		break;
	
	// その他
	default:
		p_maru->eat_flag = TRUE;
		break;
	}
	
	
    // 回転スピード取得
	rota_x = cp_func[ cp_wk->move_type ]( p_maru, cp_wk->time, cp_wk->time_max );
    BCT_CLIENT_MarunomuRotaXSet( p_maru, rota_x );

	// マウスサイズ設定
	p_maru->mouth_size = BCT_CLIENT_MarunomuMouthSizeGet( p_maru, p_maru->rotax );


	// 体当たり判定の高さを設定する
	p_maru->height = BCT_CLIENT_MarunomuBodyHeightGet( p_maru, p_maru->rotax );

    // 回転
	BCT_CLIENT_MarunomuRotaYSet( p_maru, p_maru->rota + p_maru->speed );
}

//----------------------------------------------------------------------------
/**
 *	@brief	マルノームあたり判定フラグを設定
 *
 *	@param	p_maru		マルノームワーク
 *	@param	flag		フラグ
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MarunomuSetHitFlag( BCT_MARUNOMU* p_maru, BOOL flag )
{
	p_maru->hit = flag;
}


//----------------------------------------------------------------------------
/**
 *  @brief  マルノーム回転X設定
 *
 *  @param  p_maru      マルノームワーク
 *  @param  rota_x      回転X
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MarunomuRotaXSet( BCT_MARUNOMU* p_maru, u16 rota_x )
{
    p_maru->rotax = rota_x;

    // 法線ベクトルを回転させる
    MTX_Identity33( &p_maru->rotax_mtx );
    MTX_RotX33( &p_maru->rotax_mtx, FX_SinIdx(rota_x), 
            FX_CosIdx(rota_x) );
}

//----------------------------------------------------------------------------
/**
 *  @brief  マルノームの回転Y設定
 *
 *  @param  p_maru  マルノーム
 *  @param  rota_y  回転Y
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MarunomuRotaYSet( BCT_MARUNOMU* p_maru, u16 rota_y )
{
	VecFx32 center_mat;
	
    p_maru->rota = rota_y;

    // 法線ベクトルを回転させる
    MTX_Identity33( &p_maru->rotaxy_mtx );
    MTX_Identity33( &p_maru->rotay_mtx );
    MTX_RotY33( &p_maru->rotay_mtx, FX_SinIdx(rota_y), 
            FX_CosIdx(rota_y) );

    // マトリクスを掛け合わせる
    MTX_Concat33( &p_maru->rotax_mtx, &p_maru->rotay_mtx, &p_maru->rotaxy_mtx );
    MTX_MultVec33( &BCT_MARUNOMU_HIT_PLANE_N, &p_maru->rotaxy_mtx, &p_maru->hit_plane_n );

	
	// 中心の計算
	// 当たり判定エリアの下を中心に回転するように、判定エリアのした部分を原点にした
	// ざひょうでXY軸回転をかける
	// その後、Y軸回転させたあたり判定の座標を足しこむ
	center_mat.x = 0;
	center_mat.y = 0;
	center_mat.z = -p_maru->mouth_size;
    MTX_MultVec33( &center_mat, &p_maru->rotaxy_mtx, &p_maru->center_mat );
	center_mat.x = p_maru->hit_x + p_maru->marunomu_mat.x;
	center_mat.y = p_maru->hit_y;
	center_mat.z = p_maru->marunomu_mat.z + BCT_MARUNOMU_HIT_DIS_MAX;
    MTX_MultVec33( &center_mat, &p_maru->rotay_mtx, &center_mat );
	VEC_Add( &center_mat, &p_maru->center_mat, &p_maru->center_mat  );

    // 平面の方程式Dの値を求める
    p_maru->d = FX_Mul( p_maru->hit_plane_n.x, p_maru->center_mat.x ) + FX_Mul( p_maru->hit_plane_n.y, p_maru->center_mat.y ) + FX_Mul( p_maru->hit_plane_n.z, p_maru->center_mat.z );

	// 表示座標を移動させる
    MTX_MultVec33( &p_maru->marunomu_mat, &p_maru->rotay_mtx, &p_maru->matrix );
	

//  OS_Printf( "rotaY %d rotaX %d N x=0x%x y=0x%x z=0x%x\n", p_maru->rota, p_maru->rotax, p_maru->hit_plane_n.x, p_maru->hit_plane_n.y, p_maru->hit_plane_n.z );
}

//----------------------------------------------------------------------------
/**
 *  @brief  マルノームの口に入ったかチェック
 *
 *  @param  cp_maru     マルノームワーク
 *  @param  cp_mat      当たり判定するものの現在位置
 *  @param  cp_move     当たり判定するものの今の移動値
 *  @param  p_inryoku   引力
 *
 *  @retval BCT_MARUNOMU_MOUTH_HIT_NONE,    // まだ口までいっていない
 *  @retval BCT_MARUNOMU_MOUTH_HIT_OK,  // 入った！
 *  @retval BCT_MARUNOMU_MOUTH_HIT_NG,  // はずれた！
 *  @retval BCT_MARUNOMU_MOUTH_HIT_NEAR,// 引力発生！
 */
//-----------------------------------------------------------------------------
static u32 BCT_CLIENT_MarunomuMouthHitCheck( const BCT_MARUNOMU* cp_maru, const VecFx32* cp_mat, const VecFx32* cp_move, VecFx32* p_inryoku )
{
    fx32 time;
    VecFx32 hitmat;
    fx32 dist;
    fx32 in_power;
    VecFx32 sub_mat;
    VecFx32 inryoku_way;
    u32 ret = BCT_MARUNOMU_MOUTH_HIT_NONE;

	// 判定スキップ
	if( cp_maru->hit == FALSE ){
		return BCT_MARUNOMU_MOUTH_HIT_NONE;
	}

    // 当たるまでの時間と位置を取得
    // 平面と平行に玉が動いているとき正確な値を
    // 返さない可能性がある
    BCT_CLIENT_CalcPlaneVecHitCheck( cp_mat, cp_move, 
            &cp_maru->hit_plane_n,
            cp_maru->d, &hitmat, &time );


    // 交点との距離を求める
//  VEC_Subtract( &cp_maru->center_mat, &hitmat, &sub_mat );

    // 次は今の位置と中心との距離を求める
    VEC_Subtract( &cp_maru->center_mat, cp_mat, &sub_mat );
    
    dist = VEC_Mag( &sub_mat );

    // timeの方向がどっちになればよいのかわからないのでとりあえず＋方向で考える
    if( (time >= 0) && (time <= FX32_ONE) ){

        // 時間を表示
//      OS_Printf( "time 0x%x\n", time );
//      OS_Printf( "dist = 0x%x limit = 0x%x \n", dist, cp_maru->mouth_size );

        // 平面とは当たっているので、中心からの距離を取得する
        if( dist <= cp_maru->mouth_size ){
			if( cp_maru->eat_flag == TRUE ){
	            ret = BCT_MARUNOMU_MOUTH_HIT_OK;
			}else{
	            ret = BCT_MARUNOMU_MOUTH_HIT_NG;
			}
        }else{
            // NGになったらこの後の処理を行う必要はない
            return BCT_MARUNOMU_MOUTH_HIT_NG;
        }
    }

    // time が近づいてきて、距離も近かったら引力発生
    if( (time > 0) && (dist < cp_maru->inryoku_area) ){

        // 最初は少し逆方向に反発し、その後一気に吸い込む
        in_power = FX_Mul( (cp_maru->inryoku_area - dist), cp_maru->inryoku_power );
        VEC_Normalize( &sub_mat, &inryoku_way );
        p_inryoku->x = FX_Mul( inryoku_way.x, in_power );
        p_inryoku->y = FX_Mul( inryoku_way.y, in_power );
        p_inryoku->z = FX_Mul( inryoku_way.z, in_power );

//      OS_Printf( "dist = 0x%x in_power = 0x%x dist - in_power 0x%x \n", dist, in_power, (dist - in_power) );
//      OS_Printf( "in x 0x%x y 0x%x z 0x%x\n", p_inryoku->x, p_inryoku->y, p_inryoku->z );

        // 引力によって口までの距離を消化してしまうのであれば
        // 口に入ったことにする
        if( (dist - in_power) <= cp_maru->mouth_size ){
			if( cp_maru->eat_flag == TRUE ){
	            ret = BCT_MARUNOMU_MOUTH_HIT_OK;
			}else{
	            ret = BCT_MARUNOMU_MOUTH_HIT_NG;
			}
        }else{
            ret = BCT_MARUNOMU_MOUTH_HIT_NEAR;
        }
    }

    return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief	マルノームの体との当たり判定
 *
 *	@param	cp_maru		マルノームデータ
 *	@param	cp_mat		座標
 *
 *	@retval	TRUE	ヒット
 */
//-----------------------------------------------------------------------------
static BOOL BCT_CLIENT_MarunomuBodyHitCheck( const BCT_MARUNOMU* cp_maru, const VecFx32* cp_mat )
{
	VecFx32 sub_vec;
	VecFx32 vec0,  vec1;
	fx32 dist;
	fx32 r;
	s32 y_dis;

	// 判定スキップ
	if( cp_maru->hit == FALSE ){
		return FALSE;
	}

	// 円柱と点の当たり判定
	
	// 高さがあっているかチェック
	if( (cp_maru->matrix.y < cp_mat->y) && ((cp_maru->matrix.y+cp_maru->height) > cp_mat->y) ){
		
		// XZ平面の半径距離チェック
		vec0.x = cp_maru->matrix.x;
		vec0.z = cp_maru->matrix.z;
		vec0.y = 0;
		vec1.x = cp_mat->x;
		vec1.z = cp_mat->z;
		vec1.y = 0;
		VEC_Subtract( &vec0, &vec1, &sub_vec );
		dist = VEC_Mag( &sub_vec );

		// 当たり判定半径を求める
		y_dis = cp_mat->y - cp_maru->matrix.y;
		y_dis = y_dis>>FX32_SHIFT;
		r = ( y_dis*cp_maru->hitbody_r_dis ) / (cp_maru->height >> FX32_SHIFT);
		r = cp_maru->hitbody_r_max - r;
		if( dist < r ){
			return TRUE;
		}
	}
	return FALSE;
	
}

//----------------------------------------------------------------------------
/**
 *	@brief	口のサイズを取得する
 *
 *	@param	cp_maru	マルノームワーク
 *	@param	rota_x	ｘ回転角
 *
 *	@return	サイズ
 */
//-----------------------------------------------------------------------------
static fx32 BCT_CLIENT_MarunomuMouthSizeGet( const BCT_MARUNOMU* cp_maru, u16 rota_x )
{
	s32 rota_num;
	fx32 size;
	rota_num = (rota_x*360) / 0xffff;
	rota_num = BCT_MARUNOMU_ROTA_X_MAX_360 - rota_num;
	size = (rota_num*cp_maru->mouth_div) / BCT_MARUNOMU_ROTA_X_MAX_360;
	size += cp_maru->mouth_min;
	return size;
}

//----------------------------------------------------------------------------
/**
 *	@brief	体の高さを取得する
 *
 *	@param	cp_maru	マルノームワーク
 *	@param	rota_x	ｘ回転角
 *
 *	@return	サイズ
 */
//-----------------------------------------------------------------------------
static fx32 BCT_CLIENT_MarunomuBodyHeightGet( const BCT_MARUNOMU* cp_maru,  u16 rota_x )
{
	s32 rota_num;
	fx32 height;
	rota_num = (rota_x*360) / 0xffff;
	height = (rota_num*cp_maru->hitbody_y_dif) / BCT_MARUNOMU_ROTA_X_MAX_360;
	height += cp_maru->hitbody_y_min;
	return height;
}

#if 0
//----------------------------------------------------------------------------
/**
 *	@brief	パーセンテージにあわせて	BCT_MARUNOMU_ROTA_X_INIT->BCT_MARUNOMU_ROTA_X_ADD->BCT_MARUNOMU_ROTA_X_INIT	の角度を返す
 *
 *	@param	par			現在パーセント
 *	@param	max_par		マックスパーセント
 *
 *	@return	回転角度
 */
//-----------------------------------------------------------------------------
static u16 BCT_CLIENT_MarunomuParRotaGet( s32 par, s32 max_par )
{
	return BCT_CLIENT_MarunomuParRotaGetEx( par, max_par, BCT_MARUNOMU_ROTA_X_INIT, BCT_MARUNOMU_ROTA_X_ADD );
}

//----------------------------------------------------------------------------
/**
 *	@brief	パーセンテージにあわせて	start->start+div->start	の角度を返す
 *
 *	@param	par			現在パーセント
 *	@param	max_par		マックスパーセント
 *	@param	start		開始回転角度
 *	@param	div			変化値
 *
 *	@return	回転角度
 */
//-----------------------------------------------------------------------------
static u16 BCT_CLIENT_MarunomuParRotaGetEx( s32 par, s32 max_par, u16 start, u16 div )
{
    fx32 add;
    s32 rota;

	//  補正値を求める
	rota = (par*180) / max_par;
	add = FX_Mul( Sin360( rota ), FX32_CONST( BCT_MARUNOMU_ROTA_X_ADD_SUB ) ) >> FX32_SHIFT;
	rota = ((par*div) / max_par) + add;

	return (u16)(rota + start);
}

//----------------------------------------------------------------------------
/**
 *  @brief  今の時間の回転スピード取得
 *
 *  @param  time        今の時間
 *  @param  max_time    回転時間
 *
 *  @return 現在の時間の回転スピード
 */
//-----------------------------------------------------------------------------
static u16 BCT_CLIENT_MarunomuRotaSpeedGet( s32 time, s32 max_time )
{
    s32 par;
    fx32 add;
    s32 rota;

    par = (time*BCT_MARUNOMU_ROTACOUNT_GETNUM(2)) / max_time;
    

    // 0～99まで大きく
    // 100～199まで小さく
    if( par < BCT_MARUNOMU_ROTACOUNT_GETNUM(1) ){
        //  補正値を求める
        rota = (par*180) / BCT_MARUNOMU_ROTACOUNT_GETNUM(1);
        add = FX_Mul( Sin360( rota ), FX32_CONST( BCT_MARUNOMU_ROTA_SPEED_ADD_SUB ) ) >> FX32_SHIFT;
        rota = ((par*BCT_MARUNOMU_ROTA_SPEED_ADD) / BCT_MARUNOMU_ROTACOUNT_GETNUM(1)) + add;
    }else{
        par -= BCT_MARUNOMU_ROTACOUNT_GETNUM(1);
        //  補正値を求める
        rota = ((par)*180) / BCT_MARUNOMU_ROTACOUNT_GETNUM(1);
        add = FX_Mul( Sin360( rota ), FX32_CONST( BCT_MARUNOMU_ROTA_SPEED_ADD_SUB ) ) >> FX32_SHIFT;
        rota = ((par*BCT_MARUNOMU_ROTA_SPEED_ADD) / BCT_MARUNOMU_ROTACOUNT_GETNUM(1));
        rota = BCT_MARUNOMU_ROTA_SPEED_ADD - rota + add;
    }

    // とりあえず、序所に早くなって、小さくなっていく
    return (rota + BCT_MARUNOMU_ROTA_SPEED_INIT);
}

//----------------------------------------------------------------------------
/**
 *  @brief  マルノームのX軸回転角度取得
 *  
 *  @param  time        今の時間
 *  @param  max_time    回転時間
 *
 *  @return 現在の時間のX軸回転角
 */
//-----------------------------------------------------------------------------
static u16 BCT_CLIENT_MarunomuXRotaGet( s32 time, s32 max_time )
{
	s32 par;
    s32 rota;

    par = (time*BCT_MARUNOMU_ROTACOUNT_GETNUM(2)) / max_time;

    // 0～99まで大きく
    // 100～199まで小さく
    if( par < BCT_MARUNOMU_ROTACOUNT_GETNUM(1) ){
		rota = BCT_CLIENT_MarunomuParRotaGet( par, BCT_MARUNOMU_ROTACOUNT_GETNUM(1) );
    }else{
        par -= BCT_MARUNOMU_ROTACOUNT_GETNUM(1);
		rota = BCT_CLIENT_MarunomuParRotaGet( par, BCT_MARUNOMU_ROTACOUNT_GETNUM(1) );
		// 反転させる
		rota = (BCT_MARUNOMU_ROTA_X_INIT+BCT_MARUNOMU_ROTA_X_ADD) - rota;
		if( rota < BCT_MARUNOMU_ROTA_X_MIN ){
			rota = BCT_MARUNOMU_ROTA_X_MIN;
		}
    }

    // とりあえず、序所に早くなって、小さくなっていく
    return rota;
}

//----------------------------------------------------------------------------
/**
 *	@brief	マルノーム回転スピード取得	システム２
 *
 *	@param	time
 *	@param	max_time 
 *
 *	@return	回転スピード
 */
//-----------------------------------------------------------------------------
static u16 BCT_CLIENT_MarunomuRotaSpeedGet2( s32 time, s32 max_time )
{
    s32 par;
    fx32 add;
    s32 rota;

    par = (time*(BCT_MARUNOMU_ROTACOUNT_GETNUM(3))) / max_time;
    

    if( par < BCT_MARUNOMU_ROTACOUNT_GETNUM(1) ){
        //  補正値を求める
        rota = (par*180) / BCT_MARUNOMU_ROTACOUNT_GETNUM(1);
        add = FX_Mul( Sin360( rota ), FX32_CONST( BCT_MARUNOMU_ROTA_SPEED_ADD_SUB ) ) >> FX32_SHIFT;
        rota = ((par*BCT_MARUNOMU_ROTA_SPEED_ADD) / BCT_MARUNOMU_ROTACOUNT_GETNUM(1)) + add;
    }else if( par < BCT_MARUNOMU_ROTACOUNT_GETNUM(2) ){
		par -= BCT_MARUNOMU_ROTACOUNT_GETNUM(1);

		// 一定スピードで動かす
		rota = BCT_MARUNOMU_ROTA_SPEED_ADD;
	}else{

        par -= BCT_MARUNOMU_ROTACOUNT_GETNUM(2);
        //  補正値を求める
        rota = ((par)*180) / BCT_MARUNOMU_ROTACOUNT_GETNUM(1);
        add = FX_Mul( Sin360( rota ), FX32_CONST( BCT_MARUNOMU_ROTA_SPEED_ADD_SUB ) ) >> FX32_SHIFT;
        rota = ((par*BCT_MARUNOMU_ROTA_SPEED_ADD) / BCT_MARUNOMU_ROTACOUNT_GETNUM(1));
        rota = BCT_MARUNOMU_ROTA_SPEED_ADD - rota + add;
    }

    return (rota + BCT_MARUNOMU_ROTA_SPEED_INIT);
}

//----------------------------------------------------------------------------
/**
 *	@brief	マルノームのX軸回転角取得	システム２
 *
 *	@param	time		
 *	@param	max_time 
 *
 *	@return	X軸回転角
 */
//-----------------------------------------------------------------------------
#define BCT_CLIENT_MARUNOMU_ROTX_2_START	( FX_GET_ROTA_NUM( 20 ) )
#define BCT_CLIENT_MARUNOMU_ROTX_2_DIV	( FX_GET_ROTA_NUM( 40 ) )
#define BCT_CLIENT_MARUNOMU_ROTAX_EX_TIME	( 500 )
static u16 BCT_CLIENT_MarunomuXRotaGet2( s32 time, s32 max_time )
{
	s32 par;
    s32 rota;

    par = (time*BCT_MARUNOMU_ROTACOUNT_GETNUM(40)) / max_time;

    if( par < BCT_MARUNOMU_ROTACOUNT_GETNUM(17) ){
        
		//  補正値を求める
		rota = BCT_CLIENT_MarunomuParRotaGet( par, BCT_MARUNOMU_ROTACOUNT_GETNUM(17) );
		
	}else if( par < BCT_MARUNOMU_ROTACOUNT_GETNUM(20) ){
		
		rota = BCT_MARUNOMU_ROTA_X_MAX;	// 閉じちゃう
		
    }else if( par < BCT_MARUNOMU_ROTACOUNT_GETNUM(30) ){
		
		par -= BCT_MARUNOMU_ROTACOUNT_GETNUM(20);
		
		//  ガコガコ顔を動かす	とりあえずBCT_CLIENT_MARUNOMU_ROTAX_EX_TIMEごとに動かしてみる
		rota = BCT_CLIENT_MarunomuParRotaGetEx( par%BCT_CLIENT_MARUNOMU_ROTAX_EX_TIME,
				BCT_CLIENT_MARUNOMU_ROTAX_EX_TIME, 0, BCT_CLIENT_MARUNOMU_ROTX_2_DIV );
		if( ((par/BCT_CLIENT_MARUNOMU_ROTAX_EX_TIME)%2)==0 ){
			rota = BCT_CLIENT_MARUNOMU_ROTX_2_DIV - rota;
		}
		rota += BCT_CLIENT_MARUNOMU_ROTX_2_START;
		
	}else{
		
        par -= BCT_MARUNOMU_ROTACOUNT_GETNUM(30);
        //  補正値を求める
		rota = BCT_CLIENT_MarunomuParRotaGet( par, BCT_MARUNOMU_ROTACOUNT_GETNUM(10) );
		// 反転させる
		rota = (BCT_MARUNOMU_ROTA_X_INIT+BCT_MARUNOMU_ROTA_X_ADD) - rota;
		if( rota < BCT_MARUNOMU_ROTA_X_MIN ){
			rota = BCT_MARUNOMU_ROTA_X_MIN;
		}

    }

    return rota;
}

//----------------------------------------------------------------------------
/**
 *  @brief  今の時間の回転スピード取得		タイプ３
 *
 *  @param  time        今の時間
 *  @param  max_time    回転時間
 *
 *  @return 現在の時間の回転スピード
 */
//-----------------------------------------------------------------------------
#define BCT_MARUNOMU_ROTASPEED3_MIN	( 160 )
#define BCT_MARUNOMU_ROTASPEED3_MAX	( 720 )
#define BCT_MARUNOMU_ROTASPEED3_MIN_COUNT	( 500 )
#define BCT_MARUNOMU_ROTASPEED3_MAX_COUNT	( 400 )	// 本当は５００だが４００にする
													// 後の１００はMAXスピードで回す
static u16 BCT_CLIENT_MarunomuRotaSpeedGet3( s32 time, s32 max_time )
{
    s32 par;
    fx32 add;
    s32 rota;

	// 前半は徐々に早くなっていく、
	// 後半は、まず遅くなり、その後一気に加速、減速を繰り返し、
	// 最後はゆっくりとまる
	
    par = (time*(BCT_MARUNOMU_ROTACOUNT_GETNUM(4))) / max_time;
    

    if( par < BCT_MARUNOMU_ROTACOUNT_GETNUM(2) ){
        //  補正値を求める
        rota = (par*180) / BCT_MARUNOMU_ROTACOUNT_GETNUM(2);
        add = FX_Mul( Sin360( rota ), FX32_CONST( BCT_MARUNOMU_ROTA_SPEED_ADD_SUB ) ) >> FX32_SHIFT;
        rota = ((par*BCT_MARUNOMU_ROTA_SPEED_ADD) / BCT_MARUNOMU_ROTACOUNT_GETNUM(2)) + add;
    }else if( par < BCT_MARUNOMU_ROTACOUNT_GETNUM(3) ){
		par -= BCT_MARUNOMU_ROTACOUNT_GETNUM(2);

		if( par < BCT_MARUNOMU_ROTASPEED3_MIN_COUNT ){
			rota = ((par*(BCT_MARUNOMU_ROTA_SPEED_ADD-BCT_MARUNOMU_ROTASPEED3_MIN)) / BCT_MARUNOMU_ROTASPEED3_MIN_COUNT);
			rota = BCT_MARUNOMU_ROTA_SPEED_ADD - rota;
		}else{
			par -= BCT_MARUNOMU_ROTASPEED3_MIN_COUNT; 
			if( par > BCT_MARUNOMU_ROTASPEED3_MAX_COUNT ){
				par = BCT_MARUNOMU_ROTASPEED3_MAX_COUNT;
			}
			rota = ((par*(BCT_MARUNOMU_ROTASPEED3_MAX - BCT_MARUNOMU_ROTASPEED3_MIN)) / BCT_MARUNOMU_ROTASPEED3_MAX_COUNT);
			rota += BCT_MARUNOMU_ROTASPEED3_MIN;
		}
	}else{

        par -= BCT_MARUNOMU_ROTACOUNT_GETNUM(3);
        //  補正値を求める
        rota = ((par)*180) / BCT_MARUNOMU_ROTACOUNT_GETNUM(1);
        add = FX_Mul( Sin360( rota ), FX32_CONST( BCT_MARUNOMU_ROTA_SPEED_ADD_SUB ) ) >> FX32_SHIFT;
        rota = ((par*BCT_MARUNOMU_ROTASPEED3_MAX) / BCT_MARUNOMU_ROTACOUNT_GETNUM(1));
        rota = BCT_MARUNOMU_ROTASPEED3_MAX - rota + add;
    }

    return (rota + BCT_MARUNOMU_ROTA_SPEED_INIT);
}

//----------------------------------------------------------------------------
/**
 *	@brief	マルノームのX軸回転角取得	タイプ３
 *
 *	@param	time		
 *	@param	max_time 
 *
 *	@return	X軸回転角
 */
//-----------------------------------------------------------------------------
static u16 BCT_CLIENT_MarunomuXRotaGet3( s32 time, s32 max_time )
{
	s32 par;
    s32 rota;

    par = (time*BCT_MARUNOMU_ROTACOUNT_GETNUM(2)) / max_time;

    // 0～99まで大きく
    // 100～199まで小さく
    if( par < BCT_MARUNOMU_ROTACOUNT_GETNUM(1) ){
		rota = BCT_CLIENT_MarunomuParRotaGet( par, BCT_MARUNOMU_ROTACOUNT_GETNUM(1) );
    }else{
        par -= BCT_MARUNOMU_ROTACOUNT_GETNUM(1);
		rota = BCT_CLIENT_MarunomuParRotaGet( par, BCT_MARUNOMU_ROTACOUNT_GETNUM(1) );
		// 反転させる
		rota = (BCT_MARUNOMU_ROTA_X_INIT+BCT_MARUNOMU_ROTA_X_ADD) - rota;
		if( rota < BCT_MARUNOMU_ROTA_X_MIN ){
			rota = BCT_MARUNOMU_ROTA_X_MIN;
		}
    }

    // とりあえず、序所に早くなって、小さくなっていく
    return rota;
}
#endif

//----------------------------------------------------------------------------
/**BCT_MARUNOMU_MOVE_NUM
 *	@brief	マルノームの拡大率を設定する
 *
 *	@param	p_maru		ワーク
 *	@param	scale		拡大率
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MaruScaleSet( BCT_MARUNOMU* p_maru, fx32 scale )
{
	p_maru->scale			= scale;
	p_maru->mouth_div		= FX_Mul( BCT_MARUNOMU_HIT_DIS_DIV, scale );  // マルノームの口の大きさ変化値
	p_maru->mouth_min		= FX_Mul( BCT_MARUNOMU_HIT_DIS_MIN, scale );   // マルノームの口の大きさ最小
	p_maru->mouth_max		= FX_Mul( BCT_MARUNOMU_HIT_DIS_MAX, scale );  // マルノームの口の大きさ最大
	p_maru->hit_x			= FX_Mul( BCT_MARUNOMU_HIT_X, scale );
	p_maru->hit_y			= FX_Mul( BCT_MARUNOMU_HIT_Y, scale );
	p_maru->hitbody_y_min	= FX_Mul( BCT_MARUNOMU_HITBODY_Y_MIN, scale );
	p_maru->hitbody_y_dif	= FX_Mul( BCT_MARUNOMU_HITBODY_Y_DIF, scale );
	p_maru->hitbody_r		= FX_Mul( BCT_MARUNOMU_HITBODY_R, scale );
	p_maru->hitbody_r_max	= FX_Mul( BCT_MARUNOMU_HITBODY_R_MAX, scale );
	p_maru->hitbody_r_dis	= p_maru->hitbody_r_max - p_maru->hitbody_r;
	p_maru->draw_scale		= FX_Mul( BCT_MARUNOMU_SCALE, scale );

}

//----------------------------------------------------------------------------
/**
 *	@brief	回転スピード設定
 *
 *	@param	p_maru	ワーク	
 *	@param	speed	スピード
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MaruRotSpeedSet( BCT_MARUNOMU* p_maru, u16 speed )
{
	p_maru->speed = speed;
}

//----------------------------------------------------------------------------
/**
 *	@brief	平面位置を設定
 *
 *	@param	p_maru	ワーク
 *	@param	z		平面位置
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MaruMatZSet( BCT_MARUNOMU* p_maru, fx32 z )
{
	p_maru->marunomu_mat.z = z;
}


//----------------------------------------------------------------------------
/**
 *	@brief	マルノーム	イージー動作時のX座標取得ルーチン
 *
 *	@param	cp_maru		マルノームデータ
 *	@param	time		今の時間
 *	@param	max_time	時間の最大値
 *
 *	@return	今の時間のX回転角度
 */
//-----------------------------------------------------------------------------
static u16 BCT_CLIENT_MarunomuGetRotaX_Easy( const BCT_MARUNOMU* cp_maru, s32 time, s32 max_time )
{
	return BCT_MARUNOMU_ROTA_X_MIN;
}

//----------------------------------------------------------------------------
/**
 *	@brief	マルノーム	フィーバーエフェクト時
 *
 *	@param	cp_maru		マルノームデータ
 *	@param	time		今の時間
 *	@param	max_time	時間の最大値
 *
 *	@return	今の時間のX回転角度
 */
//-----------------------------------------------------------------------------
static u16 BCT_CLIENT_MarunomuGetRotaX_FeverEff( const BCT_MARUNOMU* cp_maru, s32 time, s32 max_time )
{
	return BCT_MARUNOMU_ROTA_X_MAX;
}

//----------------------------------------------------------------------------
/**
 *	@brief	マルノーム	フィーバー動作時のX座標取得ルーチン
 *
 *	@param	cp_maru		マルノームデータ
 *	@param	time		今の時間
 *	@param	max_time	時間の最大値
 *
 *	@return	今の時間のX回転角度
 */
//-----------------------------------------------------------------------------
static u16 BCT_CLIENT_MarunomuGetRotaX_Fever( const BCT_MARUNOMU* cp_maru, s32 time, s32 max_time )
{
	s32 baku_time;	// バクバクしてる時間
	s32 rota;
	
	//  時間の経過から今の角度を割り出す
	baku_time = (time - sc_BCT_MARUNOMU_MOVE_STARTTIME[ BCT_MARUNOMU_MOVE_FEVER ]) % BCT_MARUNOMU_BAKUBAKU_ONE_SYNC;

	// 回転角度は１回max->min->maxとなる必要があるので
	// 移動距離は２倍なのでdif*2する
	rota = (baku_time * (BCT_MARUNOMU_ROTA_X_DIF*2)) / BCT_MARUNOMU_BAKUBAKU_ONE_SYNC;

	// difより大きいときは%difした値をdifから引く
	if( rota > BCT_MARUNOMU_ROTA_X_DIF ){
		rota = BCT_MARUNOMU_ROTA_X_DIF - (rota % BCT_MARUNOMU_ROTA_X_DIF);
	}

	/*
	// 今のままだとmin->max->minなのでBCT_MARUNOMU_ROTA_X_DIFからrotaを引いた値を
	// rotaとする
	rota = BCT_MARUNOMU_ROTA_X_DIF - rota;

	OS_TPrintf( "baku_time=%d  rota=%d\n",  baku_time, rota );
	//*/

	return  BCT_MARUNOMU_ROTA_X_MIN + rota;
}


//----------------------------------------------------------------------------
/**
 *  @brief  木の実データを通信データに変更
 *
 *  @param  cp_data     ワーク
 *  @param  p_comm      ツーシンデータ格納先
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsDataToComm( const BCT_NUT_DATA* cp_data, BCT_NUT_COMM* p_comm )
{
    p_comm->pl_no = cp_data->plno;
    p_comm->in_flag = cp_data->in_flag;
    p_comm->way.x = cp_data->way.x;
    p_comm->way.y = cp_data->way.y;
    p_comm->way.z = cp_data->way.z;
    p_comm->power = FX_Mul( cp_data->power, BCT_NUTS_COMM_SCALE );
    p_comm->mat.x = cp_data->mat.x >> FX32_SHIFT;
    p_comm->mat.y = cp_data->mat.y >> FX32_SHIFT;
    p_comm->mat.z = cp_data->mat.z >> FX32_SHIFT;
    p_comm->bonus = cp_data->bonus;
	p_comm->special = cp_data->special;


    /*
    OS_Printf( "前データ power=%x\n", cp_data->power );
    OS_Printf( "前データ x=%x\n", cp_data->mat.x );
    OS_Printf( "前データ y=%x\n", cp_data->mat.y );
    OS_Printf( "前データ z=%x\n", cp_data->mat.z );
    //*/
}

//----------------------------------------------------------------------------
/**
 *  @brief  木の実通信データをクライアント内のデータに変換
 *
 *  @param  cp_comm     通信データ
 *  @param  p_data      クライアント内データ
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsCommToData( const BCT_NUT_COMM* cp_comm, BCT_NUT_DATA* p_data )
{
    p_data->plno = cp_comm->pl_no;
    p_data->in_flag = cp_comm->in_flag;
    p_data->way.x = cp_comm->way.x;
    p_data->way.y = cp_comm->way.y;
    p_data->way.z = cp_comm->way.z;
    p_data->power = FX_Div( cp_comm->power, BCT_NUTS_COMM_SCALE );
    p_data->mat.x = cp_comm->mat.x << FX32_SHIFT;
    p_data->mat.y = cp_comm->mat.y << FX32_SHIFT;
    p_data->mat.z = cp_comm->mat.z << FX32_SHIFT;
	p_data->special = cp_comm->special;

    /*
    OS_Printf( "後データ power=%x\n", p_data->power );
    OS_Printf( "後データ x=%x\n", p_data->mat.x );
    OS_Printf( "後データ y=%x\n", p_data->mat.y );
    OS_Printf( "後データ z=%x\n", p_data->mat.z );
    //*/
}

//----------------------------------------------------------------------------
/**
 *  @brief  木の実設定
 *
 *  @param  p_wk        ワーク
 *  @param  cp_data     設定データ
 *  @param  seq         設定するシーケンス
 */ 
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsSet( BCT_CLIENT* p_wk, const BCT_NUT_DATA* cp_data, u32 seq )
{
    int i;

    // 空いているところに入れる
    for( i=0; i<BCT_NUTSBUFF_NUM; i++ ){
        
        if( p_wk->nuts[i].seq == BCT_NUTSSEQ_NONE ){
            p_wk->nuts[i].data = *cp_data;
            p_wk->nuts[i].seq = seq;
            p_wk->nuts[i].mat = cp_data->mat;
            p_wk->nuts[i].count = 0;
			p_wk->nuts[i].blockhit = FALSE;

            BCT_CLIENT_NutsDrawSysStart( &p_wk->graphic, &p_wk->nuts[i], p_wk->comm_num, p_wk->plno );

#ifdef BCT_DEBUG
            BCT_DEBUG_NutsSetNum++;
            OS_Printf( "NutsSetNum %d\n", BCT_DEBUG_NutsSetNum );
#endif
            return ;
        }
    }
    
    // バッファーオーバーした場合自分のデータなら強制的に入れる
    if( seq == BCT_NUTSSEQ_MOVE ){
        for( i=0; i<BCT_NUTSBUFF_NUM; i++ ){
            
            if( ( p_wk->nuts[i].seq == BCT_NUTSSEQ_MOVEOTHER ) ||
				( p_wk->nuts[i].seq == BCT_NUTSSEQ_MOVEAWAY ) ){

                // データを消す
                BCT_CLIENT_NutsDrawSysEnd( &p_wk->graphic, &p_wk->nuts[i] );
                BCT_CLIENT_NutsDel( &p_wk->nuts[i] );
                
                // 設定
                p_wk->nuts[i].data = *cp_data;
                p_wk->nuts[i].seq = seq;
                p_wk->nuts[i].mat = cp_data->mat;
                p_wk->nuts[i].count = 0;
				p_wk->nuts[i].blockhit = FALSE;

//				OS_Printf( "over write\n" );
                BCT_CLIENT_NutsDrawSysStart( &p_wk->graphic, &p_wk->nuts[i], p_wk->comm_num, p_wk->plno );

#ifdef BCT_DEBUG
                BCT_DEBUG_NutsSetNum++;
                OS_Printf( "NutsSetNum %d\n", BCT_DEBUG_NutsSetNum );
#endif
                return ;
            }
        }

        // それでもはいらないのか？
        GF_ASSERT(0);
    }
}

//----------------------------------------------------------------------------
/**
 *  @brief  木の実動作終了
 *
 *  @param  p_data      ワーク
 *  @param  in_flag     口にはいったか
 *  @param	bonus		ボーナス
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsEnd( BCT_CLIENT* p_wk, BCT_CLIENT_NUTS* p_data, BOOL in_flag, u32 bonus )
{
    // 終了フラグを立てる
    p_data->end_flag = TRUE;
    
    // 入ったフラグを設定
    p_data->data.in_flag = in_flag;

    // 入ったときのボーナス
    p_data->data.bonus = bonus;
}

//----------------------------------------------------------------------------
/**
 *  @brief  木の実データ破棄
 *  
 *  @param  p_data  ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsDel( BCT_CLIENT_NUTS* p_data )
{
#ifdef BCT_DEBUG
    BCT_DEBUG_NutsSetNum--;
    OS_Printf( "NutsSetNum %d\n", BCT_DEBUG_NutsSetNum );
#endif
    memset( p_data, 0, sizeof(BCT_CLIENT_NUTS) );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ワーク
 *
 *  @param  p_wk    木の実動作
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_AllNutsMove( BCT_CLIENT* p_wk )
{
    int i;

    for( i=0; i<BCT_NUTSBUFF_NUM; i++ ){
        // 動作させる
		if( (p_wk->nuts[i].seq == BCT_NUTSSEQ_MOVE) ||
	        (p_wk->nuts[i].seq == BCT_NUTSSEQ_MOVEOTHER) ){
	        BCT_CLIENT_NutsMove( p_wk, &p_wk->nuts[i] );
		}
		else if( (p_wk->nuts[i].seq == BCT_NUTSSEQ_MOVEAWAY) ){
	        BCT_CLIENT_NutsAwayMove( &p_wk->nuts[i], p_wk );
		}
		else if( (p_wk->nuts[i].seq == BCT_NUTSSEQ_INMOVE) ){
			BCT_CLIENT_NutsInMove( &p_wk->nuts[i], p_wk );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *  @brief  木の実ひとつひとつの動作
 *
 *  @param  p_wk        ワーク
 *  @param  p_nuts      木の実
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsMove( BCT_CLIENT* p_wk, BCT_CLIENT_NUTS* p_nuts )
{
    VecFx32 move;
    u32 result;
    VecFx32 inryoku;
	BOOL bounus_del;
	fx32 tmp_y;
	u32 last_seq;
    
    // 動作する必要があるか？
    if( (p_nuts->seq != BCT_NUTSSEQ_MOVE) &&
        (p_nuts->seq != BCT_NUTSSEQ_MOVEOTHER) ){
        return ;
    }

	// Y座標を保存
	tmp_y = p_nuts->mat.y;

	// シーケンス保存
	last_seq = p_nuts->seq;

    // 空気抵抗を加えた力を計算
	BCT_CLIENT_NutsMoveGet( p_nuts, &move );

    if( p_nuts->seq == BCT_NUTSSEQ_MOVE ){
        // 口との当たり判定
        result = BCT_CLIENT_MarunomuMouthHitCheck( &p_wk->marunomu, 
                &p_nuts->mat, &move, &inryoku );
    }else{
        result = BCT_MARUNOMU_MOUTH_HIT_NONE;
    }

#ifdef BCT_DEBUG
    if( result != 0 ){
        OS_Printf( "mouth hit = %d\n", result );
    }
#endif

    // 移動先計算
	BCT_CLIENT_NutsMatrixMove( p_nuts, &move );

    // 引力補正
    if( (result == BCT_MARUNOMU_MOUTH_HIT_NEAR) || (result == BCT_MARUNOMU_MOUTH_HIT_OK) ){
		BCT_CLIENT_NutsMatrixMove( p_nuts, &inryoku );
    }

    // カウント
    p_nuts->count ++;

//  OS_Printf( "nuts move   x=0x%x y=0x%x z=0x%x\n", p_nuts->mat.x, p_nuts->mat.y, p_nuts->mat.z );

    // 移動先が口に入ったかチェック
    if( result == BCT_MARUNOMU_MOUTH_HIT_OK ){
#ifdef PM_DEBUG
		BCT_DEBUG_in_num ++;
#endif

        p_wk->score += BCT_AddScoreGet( p_wk );

		if( p_wk->score > BCT_SCORE_MAX ){
			p_wk->score = BCT_SCORE_MAX;
		}

		switch( p_wk->bonus ){
		case 0:
			Snd_SePlay( BCT_SND_EAT );
			Snd_SePlay( BCT_SND_EAT100 );
			break;
		case 1:
			Snd_SePlay( BCT_SND_EAT );
			Snd_SePlay( BCT_SND_EAT200 );
			break;
		default:
			Snd_SePlay( BCT_SND_EAT );
			Snd_SePlay( BCT_SND_EAT300 );
			break;
		}

		// 口はいり動作開始
		BCT_CLIENT_NutsInMoveStart( p_nuts, p_wk );

        // スコアエフェクトを出す
        BCT_CLIENT_ScoreEffectStart( &p_wk->graphic.score_effect, p_wk->plno, p_wk->bonus, p_wk->move_type );

		// ボーナスカウンタ
		if( p_wk->bonus < BCT_NUTS_BONUS_COUNT_MAX ){
			p_wk->bonus ++; 

		}

		// 木の実カウンタを出すかチェック
		if( p_wk->bonus == BCT_NUTS_COUNT_START_BONUS_NUM ){
			BCT_CLIENT_NUTS_COUNT_Start( &p_wk->graphic.nutscount, &p_wk->graphic, p_wk->bonus );
		}else{
			// 木の実カウンタ値設定
			BCT_CLIENT_NUTS_COUNT_SetData( &p_wk->graphic.nutscount, &p_wk->graphic, p_wk->bonus );
		}

    }else{
		bounus_del = FALSE;

		// マルノームとの当たり判定
		result = BCT_CLIENT_MarunomuBodyHitCheck( &p_wk->marunomu, &p_nuts->mat );
		if( result == TRUE ){
			// ボーナスカウンタを破棄
			bounus_del = TRUE;


			if( last_seq == BCT_NUTSSEQ_MOVE ){
				Snd_SePlay( BCT_SND_BOUND );
			}
			
			BCT_CLIENT_NutsAwayStart( p_nuts, p_wk, TRUE );
		}
		//	地面におちたかチェック
		else if( p_nuts->mat.y <= BCT_FIELD_MIN ){
			
			// 落ちた位置が溝なら破棄
			result = BCT_CLIENT_NutsAwayDitchHitCheck( p_nuts );
			if( result == FALSE ){
				// ボーナスカウンタを破棄
				bounus_del = TRUE;

				if( tmp_y <= BCT_FIELD_MIN ){	//　溝以外で、以前のY座標がBCT_FIELD_MIN以下なら破棄
					BCT_CLIENT_NutsEnd( p_wk, p_nuts, FALSE, p_wk->bonus );
				}else{
					BCT_CLIENT_NutsAwayStart( p_nuts, p_wk, FALSE );
				}
			}else if( p_nuts->mat.y <= BCT_NUTS_AWAYPOWER_DITCHMUL ){
				// ボーナスカウンタを破棄
				bounus_del = TRUE;
				BCT_CLIENT_NutsEnd( p_wk, p_nuts, FALSE, p_wk->bonus );
			}
		}else{
			// 外壁との当たり判定
			result = BCT_CLIENT_NutsAwayBlockHitCheck( p_nuts );
			if( result == TRUE ){

				bounus_del = TRUE;

				// 壁に当たる前に座標に移動する
				VEC_Subtract( &p_nuts->mat, &move, &p_nuts->mat );
				
				// 力をなくして落ちていく～
				BCT_CLIENT_NutsAwayStartPowerReq( p_nuts, p_wk, TRUE, BCT_NUTS_AWAYPOWER_DITCHMUL, FALSE );

			}
		}

		// ボーナスデータ破棄
		if( (bounus_del == TRUE) && (last_seq == BCT_NUTSSEQ_MOVE) ){
			p_wk->bonus = 0;

			// 木の実カウンタクリア
			BCT_CLIENT_NUTS_COUNT_End( &p_wk->graphic.nutscount );
		}
    }
}

//----------------------------------------------------------------------------
/**
 *  @brief  すべての木の実の終了管理
 *
 *  @param  p_wk    ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_AllNutsEndCont( BCT_CLIENT* p_wk )
{
    int i;

    for( i=0; i<BCT_NUTSBUFF_NUM; i++ ){
        BCT_CLIENT_NutsEndCont( &p_wk->nuts[i], p_wk );
    }
}

//----------------------------------------------------------------------------
/**
 *	@brief	すべての木の実の終了管理	送信データは必ずなし
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_AllNutsEndContAllDel( BCT_CLIENT* p_wk )
{
    int i;

    for( i=0; i<BCT_NUTSBUFF_NUM; i++ ){
		if( p_wk->nuts[i].end_flag == TRUE ){
			p_wk->nuts[i].seq = BCT_NUTSSEQ_MOVEOTHER;
		}
        BCT_CLIENT_NutsEndCont( &p_wk->nuts[i], p_wk );
    }
}

//----------------------------------------------------------------------------
/**
 *  @brief  木の実の終了管理
 *  
 *  @param  p_nuts  ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsEndCont( BCT_CLIENT_NUTS* p_nuts, BCT_CLIENT* p_wk )
{
    if( p_nuts->end_flag ){
        if( (p_nuts->seq == BCT_NUTSSEQ_INMOVE) && (p_nuts->data.in_flag == TRUE) ){
            p_nuts->seq = BCT_NUTSSEQ_END;  // 送信データにする
        }else{
            //データを消す
            BCT_CLIENT_NutsDel( p_nuts );
        }
        // 表示終了
        BCT_CLIENT_NutsDrawSysEnd( &p_wk->graphic, p_nuts );
    }
}

//----------------------------------------------------------------------------
/**
 *  @brief  木の実動作方法を他人用に変更する
 *
 *  @param  p_wk        ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsSeqOtherChange( BCT_CLIENT* p_wk )
{
    int i;

    // 動作シーケンスを変更する
    for( i=0; i<BCT_NUTSBUFF_NUM; i++ ){
        
        if( (p_wk->nuts[i].seq == BCT_NUTSSEQ_MOVE) ){
            p_wk->nuts[i].seq = BCT_NUTSSEQ_MOVEOTHER;
        }
    }
}

//----------------------------------------------------------------------------
/**
 *  @brief  動いている木の実があるかチェック
 *
 *  @param  cp_wk   ワーク
 *
 *  @retval TRUE    動いている木の実がある
 *  @retval FALSE   動いている木の実はない
 */
//-----------------------------------------------------------------------------
static BOOL BCT_CLIENT_NutsMoveCheck( const BCT_CLIENT* cp_wk )
{
    int i;

    for( i=0; i<BCT_NUTSBUFF_NUM; i++ ){
        
        if( (cp_wk->nuts[i].seq == BCT_NUTSSEQ_MOVE) ||
            (cp_wk->nuts[i].seq == BCT_NUTSSEQ_MOVEOTHER) ||
            (cp_wk->nuts[i].seq == BCT_NUTSSEQ_MOVEAWAY) ||
			(cp_wk->nuts[i].seq == BCT_NUTSSEQ_INMOVE) ){
            return TRUE;
        }
    }
    return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	今のパワーを取得する
 *
 *	@param	cp_nuts		木の実
 *
 *	@return	パワー
 */
//-----------------------------------------------------------------------------
static fx32 BCT_CLIENT_NutsNowPowerGet( const BCT_CLIENT_NUTS* cp_nuts )
{
    fx32 power_num;
    // 空気抵抗を加えた力を計算
    power_num = cp_nuts->data.power - FX_Mul( BCT_NUTS_POWER_RES, cp_nuts->count*FX32_ONE );
    if( power_num < 0 ){
        power_num = 0;
    }
	return power_num;
}

//----------------------------------------------------------------------------
/**
 *	@brief	今の木の実の移動する値を取得する
 *
 *	@param	cp_nuts		木の実ワーク
 *	@param	p_move		移動する値
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsMoveGet( const BCT_CLIENT_NUTS* cp_nuts, VecFx32* p_move )
{
    fx32 power_num;
    VecFx32 move;

    // 空気抵抗を加えた力を計算
    power_num = BCT_CLIENT_NutsNowPowerGet( cp_nuts );

    // 現在の座標を取得する
    p_move->x = FX_Mul( cp_nuts->data.way.x, power_num );   // 速度
    p_move->y = FX_Mul( cp_nuts->data.way.y, power_num );   // 速度
    p_move->y += FX_Mul( BCT_NUTS_POWER_G, cp_nuts->count*FX32_ONE );   // G
    p_move->z = FX_Mul( cp_nuts->data.way.z, power_num );   // 速度
}


//----------------------------------------------------------------------------
/**
 *	@brief	木の実画面外フットばし
 *
 *	@param	p_nuts		木の実ワーク
 *	@param	p_wk		クライアントワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsAwayStart( BCT_CLIENT_NUTS* p_nuts, BCT_CLIENT* p_wk, BOOL maru_hit )
{
	fx32 mul;
	
	if( maru_hit == TRUE ){
	
		mul = BCT_NUTS_AWAYPOWER_MUL;
	}else{
		mul = BCT_NUTS_AWAYPOWERFIELD_MUL;
	}
	
	BCT_CLIENT_NutsAwayStartPowerReq( p_nuts, p_wk, maru_hit, mul, TRUE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	力倍率指定バージョン
 *
 *	@param	p_nuts		木の実
 *	@param	p_wk		ワーク
 *	@param	maru_hit	マルノーム又は、障害物に当たったのか
 *	@param	mul_power	かけるパワー
 *	@param	move_size	サイドに切れていくか？
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsAwayStartPowerReq( BCT_CLIENT_NUTS* p_nuts, BCT_CLIENT* p_wk, BOOL maru_hit, fx32 mul_power, BOOL move_side )
{
	fx32 tmp;
	fx32 power;
    VecFx32 move;
	u32 rand;
	
	// マルノームにあたっていたらwayを反転させる
	if( maru_hit ){
		p_nuts->data.way.x = -p_nuts->data.way.x;
		p_nuts->data.way.z = -p_nuts->data.way.z;

		if( move_side ){

			rand  = gf_mtRand();
			
			// ちょっと方向をゆがます	出来るだけ横に切れていく	普通横は小さいので小さいほうを大きくしちゃう
			if( MATH_ABS( p_nuts->data.way.x ) <= MATH_ABS( p_nuts->data.way.z ) ){
				power = FX_Mul( p_nuts->data.way.z, BCT_NUTS_AWAYWAY_MUL );
				if( (rand % 2) == 1 ){	// 遇奇数で方向を変えちゃう　この辺は適当でOK
					p_nuts->data.way.x = -power;
				}else{
					p_nuts->data.way.x = power;
				}
			}else{
				power = FX_Mul( p_nuts->data.way.x, BCT_NUTS_AWAYWAY_MUL );
				if( (rand % 2) == 1 ){	// 遇奇数で方向を変えちゃう　この辺は適当でOK
					p_nuts->data.way.z = -power;
				}else{
					p_nuts->data.way.z = power;
				}
			}
		}
	}

	// パワーもかえる
	p_nuts->data.power = FX_Mul( p_nuts->data.power, mul_power );

	// 動作カウンタを１に戻す	０だと移動した値が小さすぎて、跳ね返りきらない場合があるので１にする
	p_nuts->count = 0;

	// シーケンスを変更
	p_nuts->seq = BCT_NUTSSEQ_MOVEAWAY;


	// 1回動かす
    // 空気抵抗を加えた力を計算
	BCT_CLIENT_NutsMoveGet( p_nuts, &move );

    // 移動先計算
	BCT_CLIENT_NutsMatrixMove( p_nuts, &move );

    p_nuts->count ++;
}

//----------------------------------------------------------------------------
/**
 *	@brief	木の実画面がフットばし　メイン
 *
 *	@param	p_nuts		木の実ワーク	
 *	@param	p_wk		クライアントワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsAwayMove( BCT_CLIENT_NUTS* p_nuts, BCT_CLIENT* p_wk )
{
    VecFx32 move;
	BOOL result;
	BOOL nutsdel = FALSE;
	fx32 tmp_y;

    // 動作する必要があるか？
    if( (p_nuts->seq != BCT_NUTSSEQ_MOVEAWAY) ){
        return ;
    }

	// y座標
	tmp_y = p_nuts->mat.y;

    // 空気抵抗を加えた力を計算
	BCT_CLIENT_NutsMoveGet( p_nuts, &move );

    // 移動先計算
	BCT_CLIENT_NutsMatrixMove( p_nuts, &move );

    p_nuts->count ++;

	// マルノームと当たり判定
	result = BCT_CLIENT_MarunomuBodyHitCheck( &p_wk->marunomu, &p_nuts->mat );
	if( result == TRUE ){
		// 跳ね返り
		BCT_CLIENT_NutsAwayStart( p_nuts, p_wk, TRUE );
	}else{

		// 地面に落ちたらさらに跳ね返り
		if( p_nuts->mat.y <= BCT_FIELD_MIN ){

			// 床抜けチェック
			result = BCT_CLIENT_NutsAwayDitchHitCheck( p_nuts );

			if( result == FALSE ){	// 床があるなら跳ね返る
				if( tmp_y <= BCT_FIELD_MIN ){
					// 破棄処理へ
					nutsdel = TRUE;
				}else{
					BCT_CLIENT_NutsAwayStart( p_nuts, p_wk, FALSE );
				}
			}else if( p_nuts->mat.y <= BCT_FIELD_Y_DEL ){
				// 破棄処理へ
				nutsdel = TRUE;
			}

			// 力がなくなったらオワリ
			if( p_nuts->data.power <= BCT_NUTS_AWAYDEL_POWER2 ){
				nutsdel = TRUE;
			}

		}else{
			// 外壁との当たり判定
			result = BCT_CLIENT_NutsAwayBlockHitCheck( p_nuts );
			if( result == TRUE ){
				// 壁に当たる前に座標に移動する
				VEC_Subtract( &p_nuts->mat, &move, &p_nuts->mat );
				
				// 力をなくして落ちていく～
				BCT_CLIENT_NutsAwayStartPowerReq( p_nuts, p_wk, TRUE, BCT_NUTS_AWAYPOWER_DITCHMUL, FALSE );

			}
		}
	}

	// 力がなくなったらオワリ
	if( p_nuts->data.power <= BCT_NUTS_AWAYDEL_POWER ){
		nutsdel = TRUE;
	}

	// おわりチェック
	if( nutsdel == TRUE ){
		BCT_CLIENT_NutsEnd( p_wk, p_nuts, FALSE, p_wk->bonus );
	}
	
}

//----------------------------------------------------------------------------
/**
 *	@brief	溝にボールがはいったかチェック
 *
 *	@param	cp_nuts		木の実
 *
 *	@retval	TRUE	入った
 *	@retval	FALSE	入らない
 */
//-----------------------------------------------------------------------------
static BOOL BCT_CLIENT_NutsAwayDitchHitCheck( const BCT_CLIENT_NUTS* cp_nuts )
{
	if( (cp_nuts->xz_dist >= BCT_NUTS_AWAY_DITCH_MIN_R) &&
		(cp_nuts->xz_dist <= BCT_NUTS_AWAY_DITCH_MAX_R) ){
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	マルノームのエリアにボールがはいったかチェック
 *
 *	@param	cp_nuts			木の実
 *
 *	@retval	TRUE	入った
 *	@retval	FALSE	入らない
 */
//-----------------------------------------------------------------------------
static BOOL BCT_CLIENT_NutsAwayMarunomuHitCheck( const BCT_CLIENT_NUTS* cp_nuts )
{
	if( (cp_nuts->xz_dist <= BCT_NUTS_AWAY_MARUNOMU_R) ){
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	外壁との当たり判定
 *
 *	@param	p_nuts		木の実
 *
 *	@retval	TRUE	当たる
 *	@retval	FALSE	当たらない
 */
//-----------------------------------------------------------------------------
static BOOL BCT_CLIENT_NutsAwayBlockHitCheck( BCT_CLIENT_NUTS* p_nuts )
{
	if( (p_nuts->xz_dist >= BCT_NUTS_AWAY_BLOCK_R) ){
		if( p_nuts->blockhit == TRUE ){
			return TRUE;
		}
	}else if( p_nuts->blockhit == FALSE ){
		p_nuts->blockhit = TRUE;	// 1回エリア内に入ったら次からはちゃんと当たり判定
		
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	逆方向に移動させる
 *
 *	@param	p_nuts	木の実
 *	@param	dist	距離
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsAwayRetWayMove( BCT_CLIENT_NUTS* p_nuts, fx32 dist )
{
	VecFx32 move;

	move.x = FX_Mul( p_nuts->data.way.x, -dist );
	move.y = 0;
	move.z = FX_Mul( p_nuts->data.way.z, -dist );

    // 移動先計算
	BCT_CLIENT_NutsMatrixMove( p_nuts, &move );
}

//----------------------------------------------------------------------------
/**
 *	@brief	木の実　口にはいった後動作	開始
 *
 *	@param	p_nuts		木の実
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsInMoveStart( BCT_CLIENT_NUTS* p_nuts, BCT_CLIENT* p_wk )
{
	p_nuts->seq = BCT_NUTSSEQ_INMOVE;
	p_nuts->inmove_count = BCT_NUTS_INMOVE_COUNT;
	p_nuts->inmove_bonus = p_wk->bonus;
}

//----------------------------------------------------------------------------
/**
 *	@brief	木の実　口に入った後動作　メイン
 *
 *	@param	p_nuts		木の実
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsInMove( BCT_CLIENT_NUTS* p_nuts, BCT_CLIENT* p_wk )
{
    VecFx32 move;
    u32 result;
    VecFx32 inryoku;
    
    // 空気抵抗を加えた力を計算
	BCT_CLIENT_NutsMoveGet( p_nuts, &move );

	// 口との当たり判定
	result = BCT_CLIENT_MarunomuMouthHitCheck( &p_wk->marunomu, 
			&p_nuts->mat, &move, &inryoku );

    // 移動先計算
	BCT_CLIENT_NutsMatrixMove( p_nuts, &move );

    // 引力補正
    if( (result == BCT_MARUNOMU_MOUTH_HIT_NEAR) || (result == BCT_MARUNOMU_MOUTH_HIT_OK) ){
		BCT_CLIENT_NutsMatrixMove( p_nuts, &inryoku );
    }

    // カウント
    p_nuts->count ++;

	// 破棄カウント
	p_nuts->inmove_count --;
	if( p_nuts->inmove_count <= 0 ){
        BCT_CLIENT_NutsEnd( p_wk, p_nuts, TRUE, p_nuts->inmove_bonus );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	木の実の座標を動かす
 *
 *	@param	p_nuts		木の実	
 *	@param	cp_add		移動
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsMatrixMove( BCT_CLIENT_NUTS* p_nuts, const VecFx32* cp_add )
{
	VecFx32 sub_mat;

    VEC_Add( &p_nuts->mat, cp_add, &p_nuts->mat );

	
	// XZ平面の中心からの距離を求める
	sub_mat.y = 0;
	sub_mat.x = p_nuts->mat.x;
	sub_mat.z = p_nuts->mat.z;
	p_nuts->xz_dist = VEC_Mag( &sub_mat );
}


//----------------------------------------------------------------------------
/**
 *  @brief  投げる処理
 *
 *  @param  p_wk    ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsSlow( BCT_CLIENT* p_wk )
{
#ifdef BCT_DEBUG_AUTOSLOW
	BCT_DEBUG_AutoSlow( p_wk );
	return ;
#endif
    // タッチしているか
    if( sys.tp_cont == FALSE ){
        // 離したときの処理
        BCT_CLIENT_NutsSlowEnd( p_wk );
    }else{
        // タッチ時の処理
        BCT_CLIENT_NutsSlowMain( p_wk );
    }
}

//----------------------------------------------------------------------------
/**
 *  @brief  投げるの完了時
 *
 *  @param  p_wk    ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsSlowEnd( BCT_CLIENT* p_wk )
{
    BCT_NUT_DATA data;
    NNSG2dSVec2 vec;
    BOOL result;
    NNSG2dSVec2 vec0, vec1;
    s32 vec_num;
	BOOL miss_slow = FALSE;
	fx32 xz_power;


    // タッチ開始していたら投げる
    if( p_wk->slow.hand == FALSE ){
		// 玉発射間隔カウント
		p_wk->slow.time ++;
        return ;
    }

    // 最新の座標と昔の座標を取得
    result = BCT_CLIENT_NutsSlowQGetTop( &p_wk->slow, &vec0 );
    GF_ASSERT( result );
    result = BCT_CLIENT_NutsSlowQGetTail( &p_wk->slow, &vec1 );
    GF_ASSERT( result );
    vec_num = BCT_CLIENT_NutsSlowQGetInNum( &p_wk->slow );

    // 頂点がBCT_NUTS_SLOW_USE_VECNUM以上ないとだめ
    if( vec_num >= BCT_NUTS_SLOW_USE_VECNUM ){

        //  投げデータを作成する
        data.plno = p_wk->plno;
        data.in_flag = FALSE;

#ifdef BCT_DEBUG
//        OS_Printf( "sx=%d sy=%d  ex=%d ex=%d", vec0.x, vec0.y, vec1.x, vec1.y );
#endif

        // 投げた方向を求める
        vec.x = (vec0.x - vec1.x) / vec_num;
        vec.y = (vec0.y - vec1.y) / vec_num;

#ifdef BCT_DEBUG
//        OS_Printf( "vec.x %d vec.y %d\n", vec.x, vec.y );
#endif
        
        data.way.x = vec.x << FX32_SHIFT;
        data.way.z = vec.y << FX32_SHIFT;
        data.way.y = 0;
        xz_power = VEC_Mag( &data.way );	// いったん平面状のパワーを計算


        // 力の値調節
        if( xz_power < BCT_NUTS_POWER_MIN ){	// これだとぱわーが足りないのでミス投げ
			// 投げるの失敗
/*
			miss_slow = TRUE;
//*/
			if( xz_power != 0 ){
				// 力を調節して木の実設定
				VEC_Normalize( &data.way, &data.way );
				data.way.x = FX_Mul( data.way.x, BCT_NUTS_POWER_MIN );
				data.way.z = FX_Mul( data.way.z, BCT_NUTS_POWER_MIN );
//				OS_Printf( "slow power low\n" );
			}else{
				// 投げるの失敗
				miss_slow = TRUE;
//				OS_Printf( "slow point none\n" );
			}
        }else if( xz_power > BCT_NUTS_POWER_MAX ){
			// 力を調節して木の実設定
			VEC_Normalize( &data.way, &data.way );
			data.way.x = FX_Mul( data.way.x, BCT_NUTS_POWER_MAX );
			data.way.z = FX_Mul( data.way.z, BCT_NUTS_POWER_MAX );
        }

		if( miss_slow == FALSE ){
			data.way.y = BCT_NUTS_POWRT_YPOWER;
			data.power = VEC_Mag( &data.way );
			data.power = FX_Mul( data.power, BCT_NUTS_POWER_MUL );  // パワー増幅
			VEC_Normalize( &data.way, &data.way );

			// 座標位置、投げる方向をNETIDと関連づける
			// 座標はタッチしている位置によって変わる
			BCT_CLIENT_2DMATto3DMAT( vec1.x, vec1.y, &data.mat );
			data.mat.y = BCT_FIELD_PLAYER_Y;


			// 木の実発射パワー
#ifdef BCT_DEBUG
//			OS_Printf( "---xz_power--- 0x%x\n", xz_power );
//			OS_Printf( "---power--- 0x%x\n", data.power );
#endif

			// 方向と座標をNETIDに関連付ける
			BCT_CLIENT_VecNetIDRot( &data.mat, &data.mat, data.plno, p_wk->comm_num );
			BCT_CLIENT_VecNetIDRot( &data.way, &data.way, data.plno, p_wk->comm_num );

			// ボーナスを見て、スペシャルボールにする
			if( p_wk->bonus >= BCT_SPECIAL_BALL_COUNT ){
				data.special = TRUE;
			}else{
				data.special = FALSE;
			}
			
			// 登録
			BCT_CLIENT_NutsSet( p_wk, &data, BCT_NUTSSEQ_MOVE );
			Snd_SePlay( BCT_SND_SLOW );

		}
    }else{
        // 投げるの失敗
		miss_slow = TRUE;
//		OS_Printf( "slow point none\n" );
    }

	// MISSスローエフェクト開始
	if( miss_slow == TRUE ){
		BCT_CLIENT_OamAwayNutsStart( &p_wk->graphic.oamnutsaway, vec1.x, vec1.y );
	}

    // クリアと表示OFF
    BCT_CLIENT_NutsSlowClear( p_wk );

	// 次の球発射間隔タイマーリセット
	p_wk->slow.time = 0;
}


//----------------------------------------------------------------------------
/**
 *  @brief  木の実投げメイン
 *
 *  @param  p_wk 
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsSlowMain( BCT_CLIENT* p_wk )
{
    NNSG2dSVec2 data;
	BOOL result;
    
    if( p_wk->slow.hand == FALSE ){

		// タッチ開始OKチェック
		result = BCT_CLIENT_NutsSlowStartCheck( p_wk );
		if( result == TRUE ){
			
			p_wk->slow.hand = TRUE;
			// 持っている木の実描画開始
			BCT_CLIENT_HandNutsDrawStart( &p_wk->graphic.handnuts );

			// した画面のBG優先順位スクロール
			BCT_CLIENT_BGPRISCRL_Req( &p_wk->graphic.bgpri_scrl );
		}
    }

	if( p_wk->slow.hand == TRUE ){
		// タッチデータをキューに格納
		if( p_wk->tp_one.Size == 1 ){ // サンプリングできた頂点が１つの場合
			data.x = p_wk->tp_one.TPDataTbl[0].x;
			data.y = p_wk->tp_one.TPDataTbl[0].y;
			BCT_CLIENT_NutsSlowQPush( &p_wk->slow, data );
		}else if( p_wk->tp_one.Size >= 2 ){	// サンプリングが２以上
			data.x = p_wk->tp_one.TPDataTbl[0].x;
			data.y = p_wk->tp_one.TPDataTbl[0].y;
			BCT_CLIENT_NutsSlowQPush( &p_wk->slow, data );
			
			data.x = p_wk->tp_one.TPDataTbl[p_wk->tp_one.Size-1].x;
			data.y = p_wk->tp_one.TPDataTbl[p_wk->tp_one.Size-1].y;
			BCT_CLIENT_NutsSlowQPush( &p_wk->slow, data );
		}else{
			// １つも無いならtp_x tp_yをしよう
			data.x = sys.tp_x;
			data.y = sys.tp_y;
			BCT_CLIENT_NutsSlowQPush( &p_wk->slow, data );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	木の実を投げれる常態かチェック
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static BOOL BCT_CLIENT_NutsSlowStartCheck( BCT_CLIENT* p_wk )
{
	s32 x;
	s32 y;
	s32 dist;

	// ボールを離してから一定時間たたないと出せない
	if( p_wk->slow.time < BCT_NUTS_SLOW_RENSYA_TIME ){
		return FALSE;
	}
	
	// 中心からの距離を求める
	x = BCT_NUTS_SLOW_TOUCH_AREA_CENTER_X - sys.tp_x;
	y = BCT_NUTS_SLOW_TOUCH_AREA_CENTER_Y - sys.tp_y;
	dist = FX_Sqrt( ((x*x)+(y*y))<<FX32_SHIFT ) >> FX32_SHIFT;
	
	// 範囲の当たり判定
	if( dist <= BCT_NUTS_SLOW_TOUCH_AREA_R ){
		return TRUE;
	}
	return FALSE;
}


//----------------------------------------------------------------------------
/**
 *  @brief  木の実投げQ初期化
 *
 *  @param  p_wk    ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsSlowQInit( BCT_CLIENT_SLOW* p_wk )
{
    memset( p_wk->tp_q, 0, sizeof(NNSG2dSVec2)*BCT_NUTS_SLOW_Q);
    p_wk->top = 0;
    p_wk->tail = 0;
}

//----------------------------------------------------------------------------
/**
 *  @brief  投げデータ格納
 *
 *  @param  p_wk    ワーク
 *  @param  data    データ
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsSlowQPush( BCT_CLIENT_SLOW* p_wk, NNSG2dSVec2 data )
{
    // 末尾+1が先頭の時満杯
    if( ((p_wk->tail + 1)%BCT_NUTS_SLOW_Q) == p_wk->top ){
        NNSG2dSVec2 tmp;    // 捨てるコマンド
         
        // 満杯
        // 1つデータをポップしてデータ格納
        BCT_CLIENT_NutsSlowQPop( p_wk, &tmp );
    }

    // データ設定
    p_wk->tp_q[ p_wk->tail ] = data;

    // 末尾位置を動かす
    p_wk->tail = (p_wk->tail + 1)%BCT_NUTS_SLOW_Q;
}

//----------------------------------------------------------------------------
/**
 *  @brief  格納データを取得
 *
 *  @param  p_wk    ワーク
 *  @param  p_data  格納先
 */
//-----------------------------------------------------------------------------
static BOOL BCT_CLIENT_NutsSlowQPop( BCT_CLIENT_SLOW* p_wk, NNSG2dSVec2* p_data )
{
    // 先頭=末尾    データがない
    if( p_wk->tail == p_wk->top ){
        return FALSE;
    }

    *p_data = p_wk->tp_q[ p_wk->top ];

    // 先頭位置を動かす
    p_wk->top = (p_wk->top + 1)%BCT_NUTS_SLOW_Q;

    return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  先頭のデータを取得する  （一番過去のデータ）
 *  
 *  @param  p_wk    ワーク
 *  @param  p_data  データ格納先
 *
 *  @retval TRUE    データが格納できました
 *  @retval FALSE   データが格納出来ませんでした
 */
//-----------------------------------------------------------------------------
static BOOL BCT_CLIENT_NutsSlowQGetTop( BCT_CLIENT_SLOW* p_wk, NNSG2dSVec2* p_data )
{
    // 先頭=末尾    データがない
    if( p_wk->tail == p_wk->top ){
        return FALSE;
    }

    *p_data = p_wk->tp_q[ p_wk->top ];

    return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  終端のデータを取得する  （一番最新のデータ）
 *
 *  @param  p_wk        ワーク
 *  @param  p_data      データ格納先
 *
 *  @retval TRUE    データが格納できました
 *  @retval FALSE   データが格納出来ませんでした
 */
//-----------------------------------------------------------------------------
static BOOL BCT_CLIENT_NutsSlowQGetTail( BCT_CLIENT_SLOW* p_wk, NNSG2dSVec2* p_data )
{
    s32 idx;
    
    // 先頭=末尾    データがない
    if( p_wk->tail == p_wk->top ){
        return FALSE;
    }

    idx = p_wk->tail - 1;
    if( idx < 0 ){
        idx += BCT_NUTS_SLOW_Q;
    }
    
    *p_data = p_wk->tp_q[ idx ];

    return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  今格納されているデータの数を取得する
 *
 *  @param  p_wk    ワーク
 *
 *  @return 格納されているデータの数
 */
//-----------------------------------------------------------------------------
static u32 BCT_CLIENT_NutsSlowQGetInNum( BCT_CLIENT_SLOW* p_wk )
{
    if( p_wk->top > p_wk->tail ){
        return (BCT_NUTS_SLOW_Q - p_wk->top) + p_wk->tail;
    }
    return p_wk->tail - p_wk->top;
}

//----------------------------------------------------------------------------
/**
 *  @brief  木の実スローワークのクリア  表示もOFF
 *
 *  @param  p_wk    ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsSlowClear( BCT_CLIENT* p_wk )
{
    // ワークのクリア
    p_wk->slow.hand = FALSE;
    BCT_CLIENT_NutsSlowQInit( &p_wk->slow );

    // 持っている木の実描画OFF
    BCT_CLIENT_HandNutsDrawEnd( &p_wk->graphic.handnuts );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ゲームデータ変更システム	初期化
 *
 *	@param	p_wk
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_ContGameData_Init( BCT_CLIENT_CONT_GAMEDATA* p_wk, const BCT_GAMEDATA* cp_gamedata, const BCT_GAME_TYPE_WK* cp_gametype )
{
	p_wk->count = BCT_CONT_GAMEDATA_COUNT_MAX;

	// 初期データを設定
	BCT_CLIENT_GameData_GetGameTypeOneData( cp_gamedata, cp_gametype, 0, &p_wk->set );
}


//----------------------------------------------------------------------------
/**
 *	@brief	ゲームデータ変更システム	変更処理開始
 *
 *	@param	p_wk			ワーク
 *	@param	cp_gamedata		ゲームデータ
 *	@param	cp_gametype		ゲームタイプデータ
 *	@param	last			前のデータインデックス
 *	@param	next			次のデータインデックス
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_ContGameData_Start( BCT_CLIENT_CONT_GAMEDATA* p_wk, const BCT_GAMEDATA* cp_gamedata, const BCT_GAME_TYPE_WK* cp_gametype, u16 last, u16 next )
{
	BCT_GAMEDATA_ONE	last_one, next_one;

	
	// ゲームタイプの反映
	BCT_CLIENT_GameData_GetGameTypeOneData( cp_gamedata, cp_gametype, last, &last_one );
	BCT_CLIENT_GameData_GetGameTypeOneData( cp_gamedata, cp_gametype, next, &next_one );
	
	// 開始値
	p_wk->last				= last_one;
	p_wk->set				= last_one;

	// 変動値取得
	p_wk->dif.nuts_num		= next_one.nuts_num - last_one.nuts_num;
	p_wk->dif.camera_ta_y	= next_one.camera_ta_y - last_one.camera_ta_y;
	p_wk->dif.pos_z			= next_one.pos_z - last_one.pos_z;
	p_wk->dif.size			= next_one.size - last_one.size;
	
	// アングル、回転スピードは差分が距離が小さいほうを選ぶ
	if( (next_one.rota_speed - last_one.rota_speed) < ((0xffff + next_one.rota_speed) - last_one.rota_speed) ){	
		p_wk->dif.rota_speed	= next_one.rota_speed - last_one.rota_speed;
	}else{
        p_wk->dif.rota_speed	= ((0xffff + next_one.rota_speed) - last_one.rota_speed);
	}
	if( (next_one.camera_an_x - last_one.camera_an_x) < ((0xffff + next_one.camera_an_x) - last_one.camera_an_x) ){	
		p_wk->dif.camera_an_x	= next_one.camera_an_x - last_one.camera_an_x;
	}else{
        p_wk->dif.camera_an_x	= ((0xffff + next_one.camera_an_x) - last_one.camera_an_x);
	}

	p_wk->count = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ゲームデータ変更システム	メイン処理
 *		
 *	@param	p_wk	ワーク
 *
 *	@retval	TRUE	変更終了
 *	@retval	FALSE	変更途中
 */
//-----------------------------------------------------------------------------
static BOOL BCT_CLIENT_ContGameData_Main( BCT_CLIENT_CONT_GAMEDATA* p_wk )
{
	if( p_wk->count >= BCT_CONT_GAMEDATA_COUNT_MAX ){
		return TRUE;
	}

	// 計算処理
	p_wk->count ++;

	p_wk->set.nuts_num		= p_wk->last.nuts_num + ( (p_wk->dif.nuts_num * p_wk->count) / BCT_CONT_GAMEDATA_COUNT_MAX );
	p_wk->set.rota_speed	= p_wk->last.rota_speed + ( (p_wk->dif.rota_speed * p_wk->count) / BCT_CONT_GAMEDATA_COUNT_MAX );
	p_wk->set.camera_ta_y	= p_wk->last.camera_ta_y + ( (p_wk->dif.camera_ta_y * p_wk->count) / BCT_CONT_GAMEDATA_COUNT_MAX );
	p_wk->set.camera_an_x	= p_wk->last.camera_an_x + ( (p_wk->dif.camera_an_x * p_wk->count) / BCT_CONT_GAMEDATA_COUNT_MAX );
	p_wk->set.pos_z			= p_wk->last.pos_z + ( (p_wk->dif.pos_z * p_wk->count) / BCT_CONT_GAMEDATA_COUNT_MAX );
	p_wk->set.size			= p_wk->last.size + ( (p_wk->dif.size * p_wk->count) / BCT_CONT_GAMEDATA_COUNT_MAX );
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ゲームデータ変更システム	データ反映
 *
 *	@param	p_wk		ワーク
 *	@param	cp_data		データ
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_ContGameData_Reflect( BCT_CLIENT* p_wk, const BCT_CLIENT_CONT_GAMEDATA* cp_data )
{
	BCT_CLIENT_GameDataOne_Reflect( p_wk, &cp_data->set );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ゲームデータの１つを設定する
 *
 *	@param	p_wk		ワーク
 *	@param	cp_data		データ
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_GameDataOne_Reflect( BCT_CLIENT* p_wk, const BCT_GAMEDATA_ONE* cp_data )
{
	// 設定データ表示
#if 0
	OS_TPrintf( "speed %d\n", cp_data->rota_speed );
	OS_TPrintf( "ta_y %d\n", cp_data->camera_ta_y );
	OS_TPrintf( "an_x %d\n", cp_data->camera_an_x );
	OS_TPrintf( "pos_z %d\n", cp_data->pos_z );
	OS_TPrintf( "size %d\n", cp_data->size );
#endif
	
	//　回転スピード
	BCT_CLIENT_MaruRotSpeedSet( &p_wk->marunomu, cp_data->rota_speed );

	// カメラターゲットY座標
	BCT_CLIENT_CameraTargetYSet( &p_wk->graphic, cp_data->camera_ta_y );

	// カメラアングルX
	BCT_CLIENT_CameraAngleXSet( &p_wk->graphic, cp_data->camera_an_x );

	// 平面位置
	BCT_CLIENT_MaruMatZSet( &p_wk->marunomu, cp_data->pos_z );

	// スケール
	BCT_CLIENT_MaruScaleSet( &p_wk->marunomu, cp_data->size );
}

//----------------------------------------------------------------------------
/**
 *	@brief	gamedataからgametypeにあわせたゲーム構成１データを取得する
 *
 *	@param	cp_gamedata		ゲームデータ
 *	@param	cp_gametype		ゲームタイプ
 *	@param	idx				インデックス
 *	@param	p_buff			データ格納先
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_GameData_GetGameTypeOneData( const BCT_GAMEDATA* cp_gamedata, const BCT_GAME_TYPE_WK* cp_gametype, u32 idx, BCT_GAMEDATA_ONE* p_buff )
{
	u16 rota;
	s32 scale_tbl;
	
	GF_ASSERT( idx < cp_gamedata->tblnum );
	
	*p_buff = cp_gamedata->p_tbl[ idx ];

	// 回転系パラメータをu16内で動く値に変更
	rota				= p_buff->rota_speed;
	p_buff->rota_speed	= rota;

	rota				= p_buff->camera_an_x;
	p_buff->camera_an_x	= rota;


	// 拡大率のテーブル逆参照
	if( cp_gametype->scale_rev ){
		scale_tbl = BCT_CONT_GAMEDATA_RAREGAME_BIGSTART - (idx+1);
		if( scale_tbl < 0 ){
			scale_tbl = 0;
		}
		p_buff->size	= cp_gamedata->p_tbl[ scale_tbl ].size;
//		OS_TPrintf( "cp_gamedata->tblnum=%d  size=%d\n", cp_gamedata->tblnum, p_buff->size );
	}

	// 回転スピードの反転
	if( cp_gametype->rota_rev ){
		p_buff->rota_speed	= -p_buff->rota_speed;
	}
}




//----------------------------------------------------------------------------
/**
 *	@brief	タッチペン	初期化	
 *
 *	@param	p_wk			ワーク
 *	@param	p_graphic		グラフィック
 *	@param	heapID			ヒープID
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_TOUCHPEN_Init( BCT_CLIENT_TOUCHPEN_MOVE* p_wk, BCT_CLIENT_GRAPHIC* p_graphic, u32 heapID )
{
	ARCHANDLE* p_handle;
	BOOL result;

	memset( p_wk, 0, sizeof(BCT_CLIENT_TOUCHPEN_MOVE) );

	p_handle = ArchiveDataHandleOpen( ARC_WLMNGM_TOOL_GRA, heapID );
	

	// グラフィックの読み込み
	{
        // OAMリソース読込み
        p_wk->resobj[ 0 ] = CLACT_U_ResManagerResAddArcChar_ArcHandle(
                    p_graphic->resMan[ 0 ], p_handle,
                    NARC_wlmngm_tool_touchpen_NCGR,
                    FALSE, BCT_TOUCHPEN_OAM_RESID, NNS_G2D_VRAM_TYPE_2DSUB, heapID );

        p_wk->resobj[ 1 ] = CLACT_U_ResManagerResAddArcPltt_ArcHandle(
                    p_graphic->resMan[ 1 ], p_handle,
                    NARC_wlmngm_tool_touchpen_NCLR,
                    FALSE, BCT_TOUCHPEN_OAM_RESID, NNS_G2D_VRAM_TYPE_2DSUB, 2, heapID );

        p_wk->resobj[ 2 ] = CLACT_U_ResManagerResAddArcKindCell_ArcHandle(
                p_graphic->resMan[ 2 ], p_handle,
                NARC_wlmngm_tool_touchpen_NCER, FALSE,
                BCT_TOUCHPEN_OAM_RESID, CLACT_U_CELL_RES, heapID );

        p_wk->resobj[ 3 ] = CLACT_U_ResManagerResAddArcKindCell_ArcHandle(
                p_graphic->resMan[ 3 ], p_handle,
                NARC_wlmngm_tool_touchpen_NANR, FALSE,
                BCT_TOUCHPEN_OAM_RESID, CLACT_U_CELLANM_RES, heapID );

        // 転送
        result = CLACT_U_CharManagerSetCharModeAdjustAreaCont( p_wk->resobj[ 0 ] );
        GF_ASSERT( result );
        result = CLACT_U_PlttManagerSetCleanArea( p_wk->resobj[ 1 ] );
        GF_ASSERT( result );

        // リソースだけ破棄
        CLACT_U_ResManagerResOnlyDelete( p_wk->resobj[ 0 ] );
        CLACT_U_ResManagerResOnlyDelete( p_wk->resobj[ 1 ] );

		// セルアクターヘッダー作成
		CLACT_U_MakeHeader( &p_wk->header, 
				BCT_TOUCHPEN_OAM_RESID, BCT_TOUCHPEN_OAM_RESID,
				BCT_TOUCHPEN_OAM_RESID, BCT_TOUCHPEN_OAM_RESID,
				CLACT_U_HEADER_DATA_NONE, CLACT_U_HEADER_DATA_NONE,0,
				BCT_TOUCHPEN_OAM_BGPRI,
				p_graphic->resMan[0], p_graphic->resMan[1], 
				p_graphic->resMan[2], p_graphic->resMan[3],
				NULL, NULL );

	}

	// タッチペンアクター登録
	{
		CLACT_ADD_SIMPLE add;

		add.ClActSet		= p_graphic->clactSet;
		add.ClActHeader		= &p_wk->header;
		add.mat.x			= FX32_CONST( BCT_TOUCHPEN_OAM_X );
		add.mat.y			= FX32_CONST( BCT_TOUCHPEN_OAM_Y ) + BCT_GRA_OAMSUBSURFACE_Y;
		add.pri				= BCT_TOUCHPEN_OAM_SOFPRI;
		add.DrawArea		= NNS_G2D_VRAM_TYPE_2DSUB;
		add.heap			= heapID;

		p_wk->p_clwk = CLACT_AddSimple( &add );
		CLACT_SetDrawFlag( p_wk->p_clwk, FALSE );
	}

    ArchiveDataHandleClose( p_handle );
}

//----------------------------------------------------------------------------
/**
 *	@brief	タッチペン	破棄
 *
 *	@param	p_wk			ワーク
 *	@param	p_graphic		グラフィック
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_TOUCHPEN_Exit( BCT_CLIENT_TOUCHPEN_MOVE* p_wk, BCT_CLIENT_GRAPHIC* p_graphic )
{
	// アクターワークの破棄
	{
		CLACT_Delete( p_wk->p_clwk );
	}

	// リソース破棄
	{
        // VRAM管理から破棄
        CLACT_U_CharManagerDelete( p_wk->resobj[0] );
        CLACT_U_PlttManagerDelete( p_wk->resobj[1] );
        
        // リソース破棄
        CLACT_U_ResManagerResDelete( p_graphic->resMan[0], p_wk->resobj[0] );
        CLACT_U_ResManagerResDelete( p_graphic->resMan[1], p_wk->resobj[1] );
        CLACT_U_ResManagerResDelete( p_graphic->resMan[2], p_wk->resobj[2] );
        CLACT_U_ResManagerResDelete( p_graphic->resMan[3], p_wk->resobj[3] );
	}

	//クリア
	memset( p_wk, 0, sizeof(BCT_CLIENT_TOUCHPEN_MOVE) );
}
 
//----------------------------------------------------------------------------
/**
 *	@brief	タッチペン	開始
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_TOUCHPEN_Start( BCT_CLIENT_TOUCHPEN_MOVE* p_wk )
{
	//  動作開始
	{
		p_wk->move		= TRUE;
		p_wk->seq		= 0;
		p_wk->roop		= 0;
	}

	// 描画開始
	CLACT_SetDrawFlag( p_wk->p_clwk, TRUE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	タッチペン	メイン
 *
 *	@param	p_wk		ワーク
 *	@param	p_sys		クライアントシステムワーク
 */
//-----------------------------------------------------------------------------
static BOOL BCT_CLIENT_TOUCHPEN_Main( BCT_CLIENT_TOUCHPEN_MOVE* p_wk, BCT_CLIENT* p_sys )
{
	switch( p_wk->seq ){
	// 動作開始
	case BCT_TOUCHPEN_ANM_SEQ_MOVE_START:
		// タッチペンの動きをなぞって持っている木の実を描画する
		CLACT_AnmFrameSet( p_wk->p_clwk, BCT_TOUCHPEN_ANM_FRAME_START );

		// アニメフレームにあわせて持っている木の実を出す
		BCT_CLIENT_HandNutsDrawStart( &p_sys->graphic.handnuts );
		BCT_CLIENT_HandNutsDrawSetMatrix( &p_sys->graphic.handnuts, 
				BCT_TOUCHPEN_OAM_X,
				sc_BCT_TOUCHPEN_ANM_FRAME_Y[ BCT_TOUCHPEN_ANM_FRAME_START ] );

		// 木の実発射前
		p_wk->nut_set = FALSE;
		p_wk->seq++;
		break;

	// メイン
	case BCT_TOUCHPEN_ANM_SEQ_MOVE_MAIN:
		CLACT_AnmFrameChg( p_wk->p_clwk, FX32_CONST(2) );

		// アニメフレームにあわせて持っている木の実を出す
		{
			u32 frame;

			frame = CLACT_AnmFrameGet( p_wk->p_clwk );

			// 動作チェック
			switch( frame ){
			// ここで発射させる
			case BCT_TOUCHPEN_ANM_FRAME_FADEOUT:
				if( p_wk->nut_set == FALSE ){
					BCT_EasyNutsSet( p_sys, 128, 96, 128, 64, BCT_NUTSSEQ_MOVEOTHER );
					Snd_SePlay( BCT_SND_SLOW );
					p_wk->nut_set = TRUE;
				}
				break;
				
			// 消えた
			case BCT_TOUCHPEN_ANM_FRAME_END:
				p_wk->roop ++ ;
				if( p_wk->roop >= BCT_TOUCHPEN_ANM_ROOP_NUM ){
					// オワリ
					p_wk->seq = BCT_TOUCHPEN_ANM_SEQ_END_WAIT;
				}else{
					// もう一回
					p_wk->seq	= BCT_TOUCHPEN_ANM_SEQ_MOVE_WAIT;
					p_wk->wait	= BCT_TOUCHPEN_WAIT;
				}
				BCT_CLIENT_HandNutsDrawEnd( &p_sys->graphic.handnuts );
				break;

			default:
				break;
			}
			// 木の実座標を設定
			BCT_CLIENT_HandNutsDrawSetMatrix( &p_sys->graphic.handnuts, 
					BCT_TOUCHPEN_OAM_X,
					sc_BCT_TOUCHPEN_ANM_FRAME_Y[ frame ] );
		}
		break;

	// 次の開始までのウエイト
	case BCT_TOUCHPEN_ANM_SEQ_MOVE_WAIT:		
		p_wk->wait--;
		if( p_wk->wait <= 0 ){
			p_wk->seq = BCT_TOUCHPEN_ANM_SEQ_MOVE_START;
		}
		break;

	// 終了待ち
	case BCT_TOUCHPEN_ANM_SEQ_END_WAIT:
		if( BCT_CLIENT_NutsMoveCheck( p_sys ) == FALSE ){

			// 描画開始
			CLACT_SetDrawFlag( p_wk->p_clwk, FALSE );
			return TRUE;
		}
		break;
	}

	// 木の実動作処理
	BCT_CLIENT_AllNutsMove( p_sys );

	// 木の実の終了管理
    BCT_CLIENT_AllNutsEndCont( p_sys );

	return FALSE;
}



//----------------------------------------------------------------------------
/**
 *	@brief	途中経過	スコア	初期化
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MDLSCR_Init( BCT_CLIENT_MIDDLE_SCORE* p_wk )
{
	memset( p_wk, 0, sizeof(BCT_CLIENT_MIDDLE_SCORE) );
	p_wk->top_idx = BCT_MDLSCR_TOP_DEF;
}

//----------------------------------------------------------------------------
/**
 *	@brief	途中経過	スコア	設定
 *
 *	@param	p_wk		ワーク
 *	@param	score		得点
 *	@param	idx			インデックス
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MDLSCR_Set( BCT_CLIENT_MIDDLE_SCORE* p_wk, u32 score, u32 idx )
{
	p_wk->score[ idx ]	= score;
	p_wk->score_msk		|= 1<<idx; 

	if( score == 0 ){
		return ;
	}
	
	
	if( p_wk->top_idx != BCT_MDLSCR_TOP_DEF ){

		// 1位と得点を比較
		// 1位は早いもんがちにする
		// scoreは1以上
		if( p_wk->score[ p_wk->top_idx ] < score ){
			p_wk->top_idx = idx;
		}
	}else{

		//  まだ誰の点も入ってないのでその人が一位
		p_wk->top_idx = idx;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	途中経過	途中経過再取得開始
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MDLSCR_Reset( BCT_CLIENT_MIDDLE_SCORE* p_wk )
{
	p_wk->score_msk = 0;
	p_wk->top_idx	= BCT_MDLSCR_TOP_DEF;
}

//----------------------------------------------------------------------------
/**
 *	@brief	途中経過	今のところの１位を返す
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	BCT_MDLSCR_TOP_DEF	いない
 *	@retval	今の位置の人のインデックス
 */
//-----------------------------------------------------------------------------
static u32 BCT_CLIENT_MDLSCR_GetTop( const BCT_CLIENT_MIDDLE_SCORE* cp_wk )
{
	return cp_wk->top_idx;
}

//----------------------------------------------------------------------------
/**
 *	@brief	人数分の得点を取得してるかチェック
 *
 *	@param	cp_wk		ワーク
 *	@param	num			数
 *
 *	@retval	TRUE	全員の点を取得した
 *	@retval	FALSE	まだ全員分は持ってない
 */
//-----------------------------------------------------------------------------
static BOOL BCT_CLIENT_MDLSCR_CheckInNum( const BCT_CLIENT_MIDDLE_SCORE* cp_wk, u32 num )
{
	int i;
	int in_num;
	
	in_num = 0;
	for( i=0; i<BCT_PLAYER_NUM; i++ ){
		if( (cp_wk->score_msk & (1<<i)) != 0 ){
			in_num ++;
		}
	}
	if( num <= in_num ){
		return TRUE;
	}
	return FALSE;
}



//----------------------------------------------------------------------------
/**
 *  @brief  バンク設定
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_BankSet( void )
{
    GF_BGL_DISPVRAM vramSetTable = {
        GX_VRAM_BG_32_FG,               // メイン2DエンジンのBG
        GX_VRAM_BGEXTPLTT_NONE,         // メイン2DエンジンのBG拡張パレット
        GX_VRAM_SUB_BG_128_C,           // サブ2DエンジンのBG
        GX_VRAM_SUB_BGEXTPLTT_NONE,     // サブ2DエンジンのBG拡張パレット
        GX_VRAM_OBJ_128_B,              // メイン2DエンジンのOBJ
        GX_VRAM_OBJEXTPLTT_NONE,        // メイン2DエンジンのOBJ拡張パレット
        GX_VRAM_SUB_OBJ_16_I,           // サブ2DエンジンのOBJ
        GX_VRAM_SUB_OBJEXTPLTT_NONE,    // サブ2DエンジンのOBJ拡張パレット
        GX_VRAM_TEX_0_A,                // テクスチャイメージスロット
        GX_VRAM_TEXPLTT_0123_E          // テクスチャパレットスロット
    };
    GF_Disp_SetBank( &vramSetTable );
}

//----------------------------------------------------------------------------
/**
 *	@brief	バケットゲームの３Dセットアップ
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_3DSetUp( void )
{
	// ３Ｄ使用面の設定(表示＆プライオリティー)
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
    G2_SetBG0Priority(2);

	// 各種描画モードの設定(シェード＆アンチエイリアス＆半透明)
    G3X_SetShading( GX_SHADING_TOON );
    G3X_AntiAlias( TRUE );
	G3X_AlphaTest( FALSE, 0 );	// アルファテスト　　オフ
	G3X_AlphaBlend( TRUE );		// アルファブレンド　オン
	G3X_EdgeMarking( TRUE );
	G3X_SetFog( FALSE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x8000, 0 );

	// クリアカラーの設定
    G3X_SetClearColor(GX_RGB(26,26,26),0,0x7fff,63,FALSE);	//color,alpha,depth,polygonID,fog
	// ビューポートの設定
    G3_ViewPort(0, 0, 255, 191);
}

//----------------------------------------------------------------------------
/**
 *  @brief  ３D初期化
 *
 *  @param  p_wk        ワーク
 *  @param  comm_num    通信人数
 *  @param	plno		通信ID
 *  @param  heapID      ヒープID
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_3DInit( BCT_CLIENT_GRAPHIC* p_wk, u32 comm_num, u32 plno, u32 heapID )
{
	p_wk->p_3dman = GF_G3DMAN_Init( heapID, GF_G3DMAN_LNK, GF_G3DTEX_128K, 
			GF_G3DMAN_LNK, GF_G3DPLT_64K, BCT_CLIENT_3DSetUp );
	
    // カメラ設定
    BCT_CLIENT_CameraInit( p_wk, comm_num, plno, heapID );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ３D破棄
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_3DExit( BCT_CLIENT_GRAPHIC* p_wk )
{
    // カメラ破棄
    BCT_CLIENT_CameraExit( p_wk );
    
    GF_G3D_Exit( p_wk->p_3dman );
}

//----------------------------------------------------------------------------
/**
 *  @brief  グラフィックデータ初期化
 *
 *  @param  p_wk        ワーク
 *  @param  heapID      ヒープID
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_GraphicInit( BCT_CLIENT* p_wk, u32 heapID )
{
    ARCHANDLE* p_handle;

    // アロケータ作成
    sys_InitAllocator( &p_wk->graphic.allocator, heapID, 32 );
    
    // バンク設定
    BCT_CLIENT_BankSet();

    // ３D初期化
    BCT_CLIENT_3DInit( &p_wk->graphic, p_wk->comm_num, p_wk->plno, heapID );

    // BG初期化
    BCT_CLIENT_BgInit( &p_wk->graphic, heapID );

    // OAM初期化
    BCT_CLIENT_OamInit( &p_wk->graphic, heapID );

    // MSG初期化
    BCT_CLIENT_MsgInit( &p_wk->graphic, heapID );

    // アーカイブハンドルオープン
    p_handle = ArchiveDataHandleOpen( ARC_BUCKET_GRAPHIC, heapID );

    // メイン面OAMリソース初期化
    BCT_CLIENT_MainOamInit( &p_wk->graphic, p_handle, heapID );

	// メイン面背景初期化
	BCT_CLIENT_MainBackInit( &p_wk->graphic.mainback, &p_wk->graphic, p_handle, p_wk->comm_num, p_wk->plno, heapID, &p_wk->graphic.allocator );

    // マルノーム初期化
    BCT_CLIENT_MarunomuDrawInit( &p_wk->graphic.marunomu, p_handle, heapID, &p_wk->graphic.allocator );

    // 木の実初期化
    BCT_CLIENT_NutsDrawSysInit( &p_wk->graphic, p_handle, heapID );

    // 持っている木の実初期化
    BCT_CLIENT_HandNutsDrawInit( &p_wk->graphic, &p_wk->graphic.handnuts, p_wk->plno, heapID );

	// OAM木の実画面外移動エフェクト初期化
	BCT_CLIENT_OamAwayNutsInit( &p_wk->graphic, &p_wk->graphic.oamnutsaway, p_wk->plno, heapID );

    // BG面の描画
    BCT_CLIENT_BgResLoad( &p_wk->graphic, p_handle, p_wk->plno, heapID );

    // 開始画面ワーク初期化
    BCT_CLIENT_StartSysInit( &p_wk->graphic.start, &p_wk->graphic, p_wk->cp_gamedata, p_wk->comm_num, p_wk->plno, p_handle, heapID );

    // 終了ワーク初期化
    BCT_CLIENT_EndSysInit( &p_wk->graphic, p_wk->comm_num, p_wk->plno, heapID );

    // スコアエフェクト初期化
    BCT_CLIENT_ScoreEffectInit( &p_wk->graphic.score_effect, &p_wk->graphic, p_wk->comm_num, p_wk->plno, heapID );

	// BG優先順位スクロールの初期化
	BCT_CLIENT_BGPRISCRL_Init( &p_wk->graphic.bgpri_scrl, p_wk->plno );	

	// 木の実カウンター
	BCT_CLIENT_NUTS_COUNT_Init( &p_wk->graphic.nutscount, &p_wk->graphic, p_handle, heapID );

#ifdef BCT_DEBUG_HITZONE
    BCT_DEBUG_PositionInit( p_handle, heapID );
#endif
    
    ArchiveDataHandleClose( p_handle );


	// カウントワーク初期化
	p_wk->graphic.p_countwk = MNGM_COUNT_Init( p_wk->graphic.clactSet, heapID );

}

//----------------------------------------------------------------------------
/**
 *  @brief  グラフィックデータ破棄
 *
 *  @param  p_wk        ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_GraphicDelete( BCT_CLIENT* p_wk )
{

#ifdef BCT_DEBUG_HITZONE
    BCT_DEBUG_PositionExit();
#endif

	// 木の実カウンター
	BCT_CLIENT_NUTS_COUNT_Exit( &p_wk->graphic.nutscount, &p_wk->graphic );

    // スコアエフェクト破棄
    BCT_CLIENT_ScoreEffectExit( &p_wk->graphic.score_effect, &p_wk->graphic );

    // 終了ワーク破棄
    BCT_CLIENT_EndSysExit( &p_wk->graphic, p_wk->comm_num );

    // 開始画面ワーク破棄
    BCT_CLIENT_StartSysExit( &p_wk->graphic.start, &p_wk->graphic );
    
    // BG面破棄
    BCT_CLIENT_BgResRelease( &p_wk->graphic );

	// OAM木の実画面外移動エフェクト破棄
	BCT_CLIENT_OamAwayNutsExit( &p_wk->graphic, &p_wk->graphic.oamnutsaway );

    // 持っている木の実破棄
    BCT_CLIENT_HandNutsDrawExit( &p_wk->graphic.handnuts );

    // 木の実破棄
    BCT_CLIENT_NutsDrawSysExit( &p_wk->graphic );
    
    // マルノーム破棄
    BCT_CLIENT_MarunomuDrawExit( &p_wk->graphic.marunomu, &p_wk->graphic.allocator );

	// メイン面背景破棄
	BCT_CLIENT_MainBackExit( &p_wk->graphic.mainback, &p_wk->graphic, &p_wk->graphic.allocator );

    // メイン面OAMリソース破棄
    BCT_CLIENT_MainOamExit( &p_wk->graphic );

	// カウントワーク破棄
	MNGM_COUNT_Exit( p_wk->graphic.p_countwk );
    
    // BG破棄
    BCT_CLIENT_BgExit( &p_wk->graphic );

    // OAM破棄
    BCT_CLIENT_OamExit( &p_wk->graphic );

    // メッセージ破棄
    BCT_CLIENT_MsgExit( &p_wk->graphic );

    // ３D破棄
    BCT_CLIENT_3DExit( &p_wk->graphic );

	// ブライトネス破棄
	G2_BlendNone();
	G2S_BlendNone();
}

//----------------------------------------------------------------------------
/**
 *  @brief  描画メイン
 *
 *  @param  cp_wk   データワーク
 *  @param  p_wk    描画ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_GraphicMain( const BCT_CLIENT* cp_wk, BCT_CLIENT_GRAPHIC* p_wk )
{
    // マルノーム描画データ更新
    BCT_CLIENT_MarunomuDrawMain( &p_wk->marunomu, &cp_wk->marunomu, cp_wk->move_type );
	BCT_CLIENT_MarunomuDrawAnmMain( &p_wk->marunomu, &cp_wk->marunomu, cp_wk->move_type );

    // 持っている木の実描画メイン
    BCT_CLIENT_HandNutsDrawMain( &p_wk->handnuts );

	// 表示
	BCT_CLIENT_GraphicDrawCore( cp_wk, p_wk );

	// BG優先順位スクロールメイン
	BCT_CLIENT_BGPRISCRL_Main( &p_wk->bgpri_scrl, p_wk );

	// 木の実カウンター
	BCT_CLIENT_NUTS_COUNT_Main( &p_wk->nutscount );
}

//----------------------------------------------------------------------------
/**
 *	@brief	実際に表示のみ行う場所
 *
 *	@param	cp_wk		ワーク
 *	@param	p_wk		グラフィックワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_GraphicDrawCore( const BCT_CLIENT* cp_wk, BCT_CLIENT_GRAPHIC* p_wk )
{

	// OAM木の実画面外エフェクト描画メイン
	BCT_CLIENT_OamAwayNutsMain( &p_wk->oamnutsaway );
	
	// スコアエフェクトメイン
	BCT_CLIENT_ScoreEffectMain( &p_wk->score_effect );
	
    //３Ｄ描画開始
    GF_G3X_Reset();

    // カメラ設定
    BCT_CLIENT_CameraMain( p_wk );

    // ライトとアンビエント
    NNS_G3dGlbLightVector( 0, 0, -FX32_ONE, 0 );
    NNS_G3dGlbLightColor( 0, GX_RGB( 31,31,31 ) );
    NNS_G3dGlbMaterialColorDiffAmb( GX_RGB( 31,31,31 ), GX_RGB( 31,31,31 ), FALSE );
    NNS_G3dGlbMaterialColorSpecEmi( GX_RGB( 31,31,31 ), GX_RGB( 31,31,31 ), FALSE );

    // 描画
    NNS_G3dGePushMtx();
    {
		// メイン面背景
		BCT_CLIENT_MainBackDraw( &p_wk->mainback );
    }
    NNS_G3dGePopMtx(1);
	
    NNS_G3dGePushMtx();
    {
        // マルノーム描画
        BCT_CLIENT_MarunomuDrawDraw( &p_wk->marunomu, &cp_wk->marunomu );
    }
    NNS_G3dGePopMtx(1);

    NNS_G3dGePushMtx();
    {
        // 木の実描画
        BCT_CLIENT_NutsDrawSysMain( p_wk, cp_wk->comm_num );
    }
    NNS_G3dGePopMtx(1);

#ifdef BCT_DEBUG_HITZONE
    NNS_G3dGePushMtx();
    {
        BCT_DEBUG_PositionDraw( cp_wk );
    }
    NNS_G3dGePopMtx(1);
#endif
    

    /* ジオメトリ＆レンダリングエンジン関連メモリのスワップ */
    GF_G3_RequestSwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_Z);

    // セルアクター描画
    CLACT_Draw( p_wk->clactSet );
}

//----------------------------------------------------------------------------
/**
 *  @brief  BG面の初期化
 *
 *  @param  p_wk    ワーク
 *  @param  heapID  ヒープID
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_BgInit( BCT_CLIENT_GRAPHIC* p_wk, u32 heapID )
{
    p_wk->p_bgl = GF_BGL_BglIniAlloc( heapID );

    // BG SYSTEM
    {
        GF_BGL_SYS_HEADER BGsys_data = {
            GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_3D,
        };
        GF_BGL_InitBG( &BGsys_data );
    }

    // メイン画面1
    {   // ウィンドウ
        GF_BGL_BGCNT_HEADER TextBgCntDat = {
            0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
            GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
            0, 0, 0, FALSE
            };
        GF_BGL_BGControlSet( p_wk->p_bgl, GF_BGL_FRAME1_M, &TextBgCntDat, GF_BGL_MODE_TEXT );
        GF_BGL_ClearCharSet( GF_BGL_FRAME1_M, 32, 0, heapID);
        GF_BGL_ScrClear( p_wk->p_bgl, GF_BGL_FRAME1_M );
    }

    // メイン画面2
    {   // ウィンドウ
        GF_BGL_BGCNT_HEADER TextBgCntDat = {
            0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
            GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01,
            1, 0, 0, FALSE
            };
        GF_BGL_BGControlSet( p_wk->p_bgl, GF_BGL_FRAME2_M, &TextBgCntDat, GF_BGL_MODE_TEXT );
        GF_BGL_ClearCharSet( GF_BGL_FRAME2_M, 32, 0, heapID);
        GF_BGL_ScrClear( p_wk->p_bgl, GF_BGL_FRAME2_M );
    }

    // サブ画面0
    {
        GF_BGL_BGCNT_HEADER TextBgCntDat = {
            0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
            GX_BG_SCRBASE_0xd000, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
            0, 0, 0, FALSE
        };
        GF_BGL_BGControlSet( p_wk->p_bgl, GF_BGL_FRAME0_S, &TextBgCntDat, GF_BGL_MODE_TEXT );
        GF_BGL_ClearCharSet( GF_BGL_FRAME0_S, 32, 0, heapID);
        GF_BGL_ScrClear( p_wk->p_bgl, GF_BGL_FRAME0_S );
    }

    // サブ画面1   
    {
        GF_BGL_BGCNT_HEADER TextBgCntDat = {
            0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
            GX_BG_SCRBASE_0xd800, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
            1, 0, 0, FALSE
        };
        GF_BGL_BGControlSet( p_wk->p_bgl, GF_BGL_FRAME1_S, &TextBgCntDat, GF_BGL_MODE_TEXT );
        GF_BGL_ClearCharSet( GF_BGL_FRAME1_S, 32, 0, heapID);
        GF_BGL_ScrClear( p_wk->p_bgl, GF_BGL_FRAME1_S );
    }

	// サブ画面2	
	{
        GF_BGL_BGCNT_HEADER TextBgCntDat = {
            0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
            GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
            2, 0, 0, FALSE
        };
        GF_BGL_BGControlSet( p_wk->p_bgl, GF_BGL_FRAME2_S, &TextBgCntDat, GF_BGL_MODE_TEXT );
        GF_BGL_ClearCharSet( GF_BGL_FRAME2_S, 32, 0, heapID);
        GF_BGL_ScrClear( p_wk->p_bgl, GF_BGL_FRAME2_S );
	}

	// サブ画面3	
	{
        GF_BGL_BGCNT_HEADER TextBgCntDat = {
            0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
            GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
            3, 0, 0, FALSE
        };
        GF_BGL_BGControlSet( p_wk->p_bgl, GF_BGL_FRAME3_S, &TextBgCntDat, GF_BGL_MODE_TEXT );
        GF_BGL_ClearCharSet( GF_BGL_FRAME3_S, 32, 0, heapID);
        GF_BGL_ScrClear( p_wk->p_bgl, GF_BGL_FRAME3_S );
	}


    // メイン画面設定
    GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
    GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
    GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
    GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );
    GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

    // サブ面は表示OFF
    GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
    GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
    GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
    GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
    GF_Disp_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

    // メイン面にウィンドウグラフィックを設定
    TalkFontPaletteLoad( PALTYPE_MAIN_BG, BCT_GRA_BGMAIN_PAL_FONT*0x20, heapID );
    MenuWinGraphicSet(
        p_wk->p_bgl, GF_BGL_FRAME1_M, BCT_GRA_SYSWND_CGX, BCT_GRA_BGMAIN_PAL_SYSWND, 0, heapID );


}

//----------------------------------------------------------------------------
/**
 *  @brief  BG面の破棄
 *
 *  @param  p_wk    ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_BgExit( BCT_CLIENT_GRAPHIC* p_wk )
{
    GF_BGL_BGControlExit( p_wk->p_bgl, GF_BGL_FRAME1_M );
    GF_BGL_BGControlExit( p_wk->p_bgl, GF_BGL_FRAME2_M );
    GF_BGL_BGControlExit( p_wk->p_bgl, GF_BGL_FRAME0_S );
    GF_BGL_BGControlExit( p_wk->p_bgl, GF_BGL_FRAME1_S );
    GF_BGL_BGControlExit( p_wk->p_bgl, GF_BGL_FRAME2_S );
    GF_BGL_BGControlExit( p_wk->p_bgl, GF_BGL_FRAME3_S );

    sys_FreeMemoryEz( p_wk->p_bgl );
}

//----------------------------------------------------------------------------
/**
 *  @brief  OAM面の初期化
 *
 *  @param  p_wk        ワーク
 *  @param  heapID      ヒープID
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_OamInit( BCT_CLIENT_GRAPHIC* p_wk, u32 heapID )
{
    int i;

    // OAMマネージャーの初期化
    NNS_G2dInitOamManagerModule();

    // 共有OAMマネージャ作成
    // レンダラ用OAMマネージャ作成
    // ここで作成したOAMマネージャをみんなで共有する
    REND_OAMInit(
        0, 126,     // メイン画面OAM管理領域
        0, 31,      // メイン画面アフィン管理領域
        0, 126,     // サブ画面OAM管理領域
        0, 31,      // サブ画面アフィン管理領域
        heapID);


    // キャラクタマネージャー初期化
    {
        CHAR_MANAGER_MAKE cm = {
            BCT_GRA_CHARMAN_NUM,
            BCT_GRA_OAMVRAM_MAIN,
            BCT_GRA_OAMVRAM_SUB,
            0
        };
        cm.heap = heapID;
        InitCharManagerReg(&cm, GX_OBJVRAMMODE_CHAR_1D_128K, GX_OBJVRAMMODE_CHAR_1D_128K );
    }
    // パレットマネージャー初期化
    InitPlttManager(BCT_GRA_PLTTMAN_NUM, heapID);

    // 読み込み開始位置を初期化
    CharLoadStartAll();
    PlttLoadStartAll();

    //通信アイコン用にキャラ＆パレット制限
    CLACT_U_WmIcon_SetReserveAreaCharManager(NNS_G2D_VRAM_TYPE_2DMAIN, GX_OBJVRAMMODE_CHAR_1D_128K);
    CLACT_U_WmIcon_SetReserveAreaPlttManager(NNS_G2D_VRAM_TYPE_2DMAIN);
    

    // セルアクターセット作成
    p_wk->clactSet = CLACT_U_SetEasyInit( BCT_GRA_CLACTNUM, &p_wk->renddata, heapID );

    // サーフェース位置を移動させる
    CLACT_U_SetSubSurfaceMatrix( &p_wk->renddata, 0, BCT_GRA_OAMSUBSURFACE_Y );
    
    // キャラとパレットのリソースマネージャ作成
    for( i=0; i<BCT_GRA_RESMAN_NUM; i++ ){
        p_wk->resMan[i] = CLACT_U_ResManagerInit(BCT_GRA_RESMAN_LOADNUM, i, heapID);
    }


	// フォントOAM
	p_wk->p_fontoam_sys = FONTOAM_SysInit( BCT_FONTOAM_WKNUM, heapID );
}

//----------------------------------------------------------------------------
/**
 *  @brief  OAM面の破棄
 *
 *  @param  p_wk        ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_OamExit( BCT_CLIENT_GRAPHIC* p_wk )
{
    int i;

	// フォントOAM
	FONTOAM_SysDelete( p_wk->p_fontoam_sys );

    // アクターの破棄
    CLACT_DestSet( p_wk->clactSet );


    for( i=0; i<BCT_GRA_RESMAN_NUM; i++ ){
        CLACT_U_ResManagerDelete( p_wk->resMan[i] );
    }

    // リソース解放
    DeleteCharManager();
    DeletePlttManager();

    //OAMレンダラー破棄
    REND_OAM_Delete();
}

//----------------------------------------------------------------------------
/**
 *  @brief  メイン名OAMリソース 初期化
 *
 *  @param  p_wk        ワーク
 *  @param  p_handle    アークハンドル
 *  @param  heapID      ヒープID
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MainOamInit( BCT_CLIENT_GRAPHIC* p_wk, ARCHANDLE* p_handle, u32 heapID )
{
    BOOL result;
    // メイン画面OAMリソース読み込み
    {
        // OAMリソース読込み
        p_wk->mainoamresobj[ 0 ] = CLACT_U_ResManagerResAddArcChar_ArcHandle(
                    p_wk->resMan[ 0 ], p_handle,
                    NARC_bucket_font_boad_NCGR,
                    FALSE, BCT_MAINOAM_CONTID, NNS_G2D_VRAM_TYPE_2DMAIN, heapID );

        p_wk->mainoamresobj[ 1 ] = CLACT_U_ResManagerResAddArcPltt_ArcHandle(
                    p_wk->resMan[ 1 ], p_handle,
                    NARC_bucket_font_boad_NCLR,
                    FALSE, BCT_MAINOAM_CONTID, NNS_G2D_VRAM_TYPE_2DMAIN, 
					BCT_GRA_OAMMAIN_PAL_NUM, heapID );

        p_wk->mainoamresobj[ 2 ] = CLACT_U_ResManagerResAddArcKindCell_ArcHandle(
                p_wk->resMan[ 2 ], p_handle,
                NARC_bucket_font_boad_NCER, FALSE,
                BCT_MAINOAM_CONTID, CLACT_U_CELL_RES, heapID );

        p_wk->mainoamresobj[ 3 ] = CLACT_U_ResManagerResAddArcKindCell_ArcHandle(
                p_wk->resMan[ 3 ], p_handle,
                NARC_bucket_font_boad_NANR, FALSE,
                BCT_MAINOAM_CONTID, CLACT_U_CELLANM_RES, heapID );

        // 転送
        result = CLACT_U_CharManagerSetCharModeAdjustAreaCont( p_wk->mainoamresobj[ 0 ] );
        GF_ASSERT( result );
        result = CLACT_U_PlttManagerSetCleanArea( p_wk->mainoamresobj[ 1 ] );
        GF_ASSERT( result );

        // リソースだけ破棄
        CLACT_U_ResManagerResOnlyDelete( p_wk->mainoamresobj[ 0 ] );
        CLACT_U_ResManagerResOnlyDelete( p_wk->mainoamresobj[ 1 ] );

        // セルアクターヘッダー作成
        CLACT_U_MakeHeader( &p_wk->mainoamheader, 
                BCT_MAINOAM_CONTID, BCT_MAINOAM_CONTID,
                BCT_MAINOAM_CONTID, BCT_MAINOAM_CONTID,
                CLACT_U_HEADER_DATA_NONE, CLACT_U_HEADER_DATA_NONE,0,
                BCT_MAINOAM_BGPRI,
                p_wk->resMan[0], p_wk->resMan[1], 
                p_wk->resMan[2], p_wk->resMan[3],
                NULL, NULL );
    }
}

//----------------------------------------------------------------------------
/**
 *  @brief  メイン面　OAMリソース破棄
 *
 *  @param  p_wk    ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MainOamExit( BCT_CLIENT_GRAPHIC* p_wk )
{
    // メイン画面OAMリソース破棄
    {
        // VRAM管理から破棄
        CLACT_U_CharManagerDelete( p_wk->mainoamresobj[0] );
        CLACT_U_PlttManagerDelete( p_wk->mainoamresobj[1] );
        
        // リソース破棄
        CLACT_U_ResManagerResDelete( p_wk->resMan[0], p_wk->mainoamresobj[0] );
        CLACT_U_ResManagerResDelete( p_wk->resMan[1], p_wk->mainoamresobj[1] );
        CLACT_U_ResManagerResDelete( p_wk->resMan[2], p_wk->mainoamresobj[2] );
        CLACT_U_ResManagerResDelete( p_wk->resMan[3], p_wk->mainoamresobj[3] );
    }
    
}

//----------------------------------------------------------------------------
/**
 *  @brief  メッセージデータ初期化
 *
 *  @param  p_wk        ワーク
 *  @param  heapID      ヒープID
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MsgInit( BCT_CLIENT_GRAPHIC* p_wk, u32 heapID )
{
    p_wk->p_wordset = WORDSET_Create( heapID );
    p_wk->p_msgman = MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, NARC_msg_lobby_minigame1_dat, heapID );

    p_wk->p_msgstr = STRBUF_Create( BCT_STRBUF_NUM, heapID );
    p_wk->p_msgtmp = STRBUF_Create( BCT_STRBUF_NUM, heapID );
}

//----------------------------------------------------------------------------
/**
 *  @brief  メッセージデータ破棄
 *
 *  @param  p_wk    ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MsgExit( BCT_CLIENT_GRAPHIC* p_wk )
{
    STRBUF_Delete( p_wk->p_msgtmp );
    STRBUF_Delete( p_wk->p_msgstr );
    MSGMAN_Delete( p_wk->p_msgman );
    WORDSET_Delete( p_wk->p_wordset );
}

//----------------------------------------------------------------------------
/**
 *  @brief  カメラ初期化
 *
 *  @param  p_wk        ワーク
 *  @param  comm_num    通信人数
 *  @param	plno		通史ID
 *  @param  heapID      ヒープ
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_CameraInit( BCT_CLIENT_GRAPHIC* p_wk, u32 comm_num, u32 plno, u32 heapID )
{
    CAMERA_ANGLE angle;
    VecFx32 up;
    MtxFx33 rot;
    
    // カメラアロック
    p_wk->p_camera = GFC_AllocCamera( heapID );

    p_wk->target.x = BCT_CAMERA_TARGET_X;
    p_wk->target.y = BCT_CAMERA_TARGET_Y;
    p_wk->target.z = BCT_CAMERA_TARGET_Z;

    angle.x = BCT_CAMERA_ANGLE_X;
    angle.y = BCT_CAMERA_ANGLEY_NetID[comm_num-1][ plno ];
    angle.z = BCT_CAMERA_ANGLE_Z;

    GFC_InitCameraTDA( 
            &p_wk->target, BCT_CAMERA_DISTANCE,
            &angle, BCT_CAMERA_PEARCE,
            GF_CAMERA_PERSPECTIV,
            TRUE,   
            p_wk->p_camera
            );

    up.x = 0;
    up.y = BCT_CAMERA_UP;
    up.z = 0;
    GFC_SetCamUp( &up, p_wk->p_camera );
    
    GFC_AttachCamera( p_wk->p_camera );

	// Near Far設定
	GFC_SetCameraClip( BCT_CAMERA_NEAR, BCT_CAMERA_FAR, p_wk->p_camera );
}

//----------------------------------------------------------------------------
/**
 *  @brief  カメラ破棄
 *
 *  @param  p_wk        ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_CameraExit( BCT_CLIENT_GRAPHIC* p_wk )
{
    GFC_FreeCamera( p_wk->p_camera );
}

//----------------------------------------------------------------------------
/**
 *  @brief  カメラメイン
 *
 *  @param  p_wk        ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_CameraMain( BCT_CLIENT_GRAPHIC* p_wk )
{
    // カメラ設定
    GFC_CameraLookAt();
}

//----------------------------------------------------------------------------
/**
 *  @brief  BG画面の設定
 *
 *  @param  p_wk        ワーク
 *  @param  p_handle    ハンドル
 *  @param	plno		通信ID
 *  @param  heapID      ヒープID
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_BgResLoad( BCT_CLIENT_GRAPHIC* p_wk, ARCHANDLE* p_handle, u32 plno, u32 heapID )
{
	static const u8 sc_SubPal[ BCT_PLAYER_NUM ] = {
		BCT_GRA_BGSUB_PAL_BACK_NETID0,
		BCT_GRA_BGSUB_PAL_BACK_NETID1,
		BCT_GRA_BGSUB_PAL_BACK_NETID2,
		BCT_GRA_BGSUB_PAL_BACK_NETID3,
	};
	
    // サブ画面のBG
    ArcUtil_HDL_BgCharSet( p_handle, NARC_bucket_tamaire_bg_NCGR, p_wk->p_bgl, GF_BGL_FRAME0_S, 0, 0, FALSE, heapID );
    ArcUtil_HDL_ScrnSet( p_handle, NARC_bucket_tamaire_bg0_NSCR, p_wk->p_bgl,GF_BGL_FRAME3_S, 0, 0, FALSE, heapID );
    ArcUtil_HDL_ScrnSet( p_handle, NARC_bucket_tamaire_bg1_NSCR, p_wk->p_bgl,GF_BGL_FRAME2_S, 0, 0, FALSE, heapID );
    ArcUtil_HDL_ScrnSet( p_handle, NARC_bucket_tamaire_bg2_NSCR, p_wk->p_bgl,GF_BGL_FRAME1_S, 0, 0, FALSE, heapID );
    ArcUtil_HDL_ScrnSet( p_handle, NARC_bucket_tamaire_bg3_NSCR, p_wk->p_bgl,GF_BGL_FRAME0_S, 0, 0, FALSE, heapID );
    ArcUtil_HDL_PalSet( p_handle, NARC_bucket_tamaire_bg_NCLR, PALTYPE_SUB_BG, 0, BCT_GRA_BGSUB_PAL_NUM*32, heapID );

	// パレットを合わせる
	GF_BGL_ScrPalChange( p_wk->p_bgl, GF_BGL_FRAME3_S, 0, 0, 32, 32, sc_SubPal[plno] );	
	GF_BGL_ScrPalChange( p_wk->p_bgl, GF_BGL_FRAME2_S, 0, 0, 32, 32, BCT_GRA_BGSUB_PAL_NETID0_BACK+(plno*2) );	
	GF_BGL_ScrPalChange( p_wk->p_bgl, GF_BGL_FRAME1_S, 0, 0, 32, 32, BCT_GRA_BGSUB_PAL_NETID0_TOP+(plno*2) );	
	GF_BGL_ScrPalChange( p_wk->p_bgl, GF_BGL_FRAME0_S, 0, 0, 32, 32, BCT_GRA_BGSUB_PAL_NETID0_TOP+(plno*2) );

	GF_BGL_LoadScreenReq( p_wk->p_bgl, GF_BGL_FRAME0_S );
	GF_BGL_LoadScreenReq( p_wk->p_bgl, GF_BGL_FRAME1_S );
	GF_BGL_LoadScreenReq( p_wk->p_bgl, GF_BGL_FRAME2_S );
	GF_BGL_LoadScreenReq( p_wk->p_bgl, GF_BGL_FRAME3_S );
}

//----------------------------------------------------------------------------
/**
 *  @brief  BG画面の破棄
 *
 *  @param  p_wk    ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_BgResRelease( BCT_CLIENT_GRAPHIC* p_wk )
{
}

//----------------------------------------------------------------------------
/**
 *  @brief  マルノーム描画オブジェクト初期化
 *
 *  @param  p_wk        ワーク
 *  @param  p_handle    ハンドル
 *  @param  heapID      ヒープID
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MarunomuDrawInit( BCT_MARUNOMU_DRAW* p_wk, ARCHANDLE* p_handle, u32 heapID, NNSFndAllocator* p_allocator )
{
    static const u16 Anm[ BCT_MARUNOMU_ANM_NUM ] = {
        NARC_bucket_maru_robo_ani01_nsbca,
        NARC_bucket_maru_robo_ani02_nsbca,
        NARC_bucket_maru_robo_ani03_nsbca,
        NARC_bucket_maru_robo_ani04_nsbca,
        NARC_bucket_maru_robo_ani05_nsbca,
        NARC_bucket_maru_robo_ani06_nsbca,
    };
	static const u16 Mdl[ BCT_MARUNOMU_MDL_NUM ] = {
		NARC_bucket_maru_robo_col_nsbmd,
		NARC_bucket_maru_robo_ani05_nsbmd,
		NARC_bucket_maru_robo_ani04_nsbmd,
	};
    int i, j, k, idx;
	const NNSG3dResName* cp_node_name;
	const NNSG3dResNodeInfo* cp_nodeinfo;

	memset( p_wk, 0, sizeof(BCT_MARUNOMU_DRAW) );
    
    // モデルﾃﾞｰﾀ読み込み
	for( i=0; i<BCT_MARUNOMU_MDL_NUM; i++ ){
	    D3DOBJ_MdlLoadH( &p_wk->mdl[i], p_handle, Mdl[i], heapID );

		// レンダーオブジェクトに登録
		D3DOBJ_Init( &p_wk->obj[i], &p_wk->mdl[i] );

		// 描画OFF
		D3DOBJ_SetDraw( &p_wk->obj[i], FALSE );

		// 座標、拡大率の設定
	    D3DOBJ_SetMatrix( &p_wk->obj[i], sc_MARUNOMU_MAT.x, sc_MARUNOMU_MAT.y + BCT_START_SCRLL3D_Y_S, sc_MARUNOMU_MAT.z );
		D3DOBJ_SetScale( &p_wk->obj[i], BCT_MARUNOMU_SCALE, BCT_MARUNOMU_SCALE, BCT_MARUNOMU_SCALE );

	}
        

    
    // アニメﾃﾞｰﾀ読み込み
    for( i=0; i<BCT_MARUNOMU_ANM_NUM; i++ ){
        D3DOBJ_AnmLoadH( &p_wk->anm[i], &p_wk->mdl[ sc_BCT_MARUNOMU_ANM_MDL[i] ], p_handle, 
                Anm[i], heapID, p_allocator );
		// アニメ状態を設定
		D3DOBJ_AnmSet( &p_wk->anm[i], 0 );

		// アニメの反映ノード設定
		// まず全ノードへのアニメの影響をOFF
		j = 0;
		cp_nodeinfo = NNS_G3dGetNodeInfo( p_wk->mdl[ sc_BCT_MARUNOMU_ANM_MDL[i] ].pModel );
		while( (cp_node_name = NNS_G3dGetNodeNameByIdx( cp_nodeinfo, j )) != NULL ){
			NNS_G3dAnmObjDisableID( p_wk->anm[i].pAnmObj, j );
			j++;
		}

		// 反映すべきノードを設定
		for( j=0; j<BCT_MARUNOMU_ANM_NODE_NUM; j++ ){
			idx = NNS_G3dGetNodeIdxByName( cp_nodeinfo, &sc_AnmNodeName[i][j] );
			if( idx != -1 ){
				NNS_G3dAnmObjEnableID( p_wk->anm[i].pAnmObj, idx );
			}
//			OS_TPrintf( "node name %s  idx %d\n", sc_AnmNodeName[i][j].name, idx );
		}
    }

	// カラーアニメ読み込み
	for( i=0; i<BCT_MARUNOMU_ANM_COLANM_NUM; i++ ){
		D3DOBJ_AnmLoadH( &p_wk->colanm[i], &p_wk->mdl[ sc_BCT_MARUNOMU_COLANM_MDL[i] ], p_handle, 
				NARC_bucket_maru_robo_col_nsbtp, heapID, p_allocator );

		// カラーアニメ設定
		D3DOBJ_AddAnm( &p_wk->obj[ sc_BCT_MARUNOMU_COLANM_MDL[i] ], &p_wk->colanm[i] );
	}


	// カラーアニメパラメータ初期化
	p_wk->col_top	= BCT_PLAYER_NUM;
	p_wk->col_rand	= FALSE;


	// 歩きアニメ設定
	BCT_CLIENT_MarunomuDrawSetWalkAnm( p_wk, FALSE );

	// オープンアニメを登録
	D3DOBJ_AddAnm( &p_wk->obj[ BCT_MARUNOMU_MDL_OPEN ], &p_wk->anm[ BCT_MARUNOMU_ANM_OPEN ] );
	D3DOBJ_AnmSet( &p_wk->anm[ BCT_MARUNOMU_ANM_OPEN ], 0 );
	D3DOBJ_SetDraw( &p_wk->obj[ BCT_MARUNOMU_MDL_OPEN ], TRUE );	
	p_wk->set_mouthanm = BCT_MARUNOMU_ANM_OPEN;
}

//----------------------------------------------------------------------------
/**
 *  @brief  マルノーム描画オブジェクト破棄
 *
 *  @param  p_wk    ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MarunomuDrawExit( BCT_MARUNOMU_DRAW* p_wk, NNSFndAllocator* p_allocator )
{
    int i;
	
    
    // 全リソース破棄
	for( i=0; i<BCT_MARUNOMU_MDL_NUM; i++ ){
	    D3DOBJ_MdlDelete( &p_wk->mdl[i] ); 
	}

    for( i=0; i<BCT_MARUNOMU_ANM_NUM; i++ ){
        D3DOBJ_AnmDelete( &p_wk->anm[i], p_allocator );
    }

	for( i=0; i<BCT_MARUNOMU_ANM_COLANM_NUM; i++ ){
	    D3DOBJ_AnmDelete( &p_wk->colanm[i], p_allocator );
	}


    memset( p_wk, 0, sizeof(BCT_MARUNOMU_DRAW) );   
}

//----------------------------------------------------------------------------
/**
 *  @brief  マルノーム描画オブジェクトメイン動作
 *
 *  @param  p_wk		ワーク
 *  @param  cp_data		データ
 *  @param	movetype	時間で遷移する動作タイプ
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MarunomuDrawMain( BCT_MARUNOMU_DRAW* p_wk, const BCT_MARUNOMU* cp_data, BCT_MARUNOMU_MOVE_TYPE movetype )
{
    // 座標と角度と拡大率を設定
    BCT_CLIENT_MarunomuDrawMatrixSet( p_wk, cp_data );
	BCT_CLIENT_MarunomuDrawRotaSet( p_wk, cp_data );
	BCT_CLIENT_MarunomuDrawScaleSet( p_wk, cp_data );

	// アニメ設定
	switch( movetype ){
	// rotaxの値を設定
    case BCT_MARUNOMU_MOVE_EASY:			// 簡単動作
    case BCT_MARUNOMU_MOVE_FEVER:		// FEVER
		BCT_CLIENT_MarunomuDrawSetMouthAnm( p_wk, BCT_MARUNOMU_ANM_ROTA );
		BCT_CLIENT_MarunomuDrawAnmRotaSet( p_wk, cp_data->rotax );
		BCT_CLIENT_MarunomuDrawSetWalkAnm( p_wk, TRUE );
		break;

	/*
	// 独自アニメ
    case BCT_MARUNOMU_MOVE_BAKUBAKU_EFF:	// ばくばく前エフェクト
		BCT_CLIENT_MarunomuDrawSetMouthAnm( p_wk, BCT_MARUNOMU_ANM_PURU );
		BCT_CLIENT_MarunomuDrawLoopMouthAnm( p_wk );
		break;
	//*/
    case BCT_MARUNOMU_MOVE_FEVER_EFF:	// FEVER前エフェクト
		BCT_CLIENT_MarunomuDrawSetMouthAnm( p_wk, BCT_MARUNOMU_ANM_FEV );
		BCT_CLIENT_MarunomuDrawSetWalkAnm( p_wk, FALSE );
		BCT_CLIENT_MarunomuDrawNoLoopMouthAnm_Speed( p_wk, BCT_MARUNOMU_ANM_FEVERSPEED );
		break;
	}

}

//----------------------------------------------------------------------------
/**
 *	@brief	描画アニメメイン
 *
 *	@param	p_wk
 *	@param	cp_data
 *	@param	movetype 
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MarunomuDrawAnmMain( BCT_MARUNOMU_DRAW* p_wk, const BCT_MARUNOMU* cp_data, BCT_MARUNOMU_MOVE_TYPE movetype )
{
	// 常に歩きアニメ
	D3DOBJ_AnmLoop( &p_wk->anm[ BCT_MARUNOMU_ANM_WALK ], BCT_MARUNOMU_ANM_SLOWSPEED );

	BCT_CLIENT_MarunomuDrawColAnmMain( p_wk );
}

//----------------------------------------------------------------------------
/**
 *  @brief  マルノームに回転角度　座標を設定
 *
 *  @param  p_wk        ワーク
 *  @param  cp_data     マルノームデータ
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MarunomuDrawMatrixSet( BCT_MARUNOMU_DRAW* p_wk, const BCT_MARUNOMU* cp_data )
{
	int i;

    // 座標を設定
	for( i=0; i<BCT_MARUNOMU_MDL_NUM; i++ ){
	    D3DOBJ_SetMatrix( &p_wk->obj[i], cp_data->matrix.x, cp_data->matrix.y, cp_data->matrix.z );
	}

}

//----------------------------------------------------------------------------
/**
 *	@brief	角度を設定
 *
 *	@param	p_wk
 *	@param	cp_data 
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MarunomuDrawRotaSet( BCT_MARUNOMU_DRAW* p_wk, const BCT_MARUNOMU* cp_data )
{
	int i;
    fx32 scale;
    u32 setrota;
	
    // 平面角度設定
    setrota = cp_data->rota + BCT_MARUNOMU_DRAWROTA_ADD;

	for( i=0; i<BCT_MARUNOMU_MDL_NUM; i++ ){
	    D3DOBJ_SetRota( &p_wk->obj[i], (u16)setrota, D3DOBJ_ROTA_WAY_Y );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	拡大率を設定
 *
 *	@param	p_wk
 *	@param	cp_data 
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MarunomuDrawScaleSet( BCT_MARUNOMU_DRAW* p_wk, const BCT_MARUNOMU* cp_data )
{
	int i;
	
	// マルノームの描画に反映
	for( i=0; i<BCT_MARUNOMU_MDL_NUM; i++ ){
	    D3DOBJ_SetScale( &p_wk->obj[i], cp_data->draw_scale, cp_data->draw_scale, cp_data->draw_scale );
	}
}

//----------------------------------------------------------------------------
/**
 *  @brief  マルノーム描画
 *
 *  @param  p_wk    ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MarunomuDrawDraw( BCT_MARUNOMU_DRAW* p_wk, const BCT_MARUNOMU* cp_data )
{
	int i;
/*
	MtxFx33 mtx;
	MtxFx33 tmp;

    MTX_Identity33( &mtx );
    MTX_RotY33( &mtx, 
			FX_SinIdx( cp_data->rota + BCT_MARUNOMU_DRAWROTA_ADD ), 
			FX_CosIdx( cp_data->rota + BCT_MARUNOMU_DRAWROTA_ADD ) );

    MTX_Identity33( &tmp );
    MTX_RotX33( &tmp, 
			FX_SinIdx( FX_GET_ROTA_NUM(40) ), 
			FX_CosIdx( FX_GET_ROTA_NUM(40) ) );

    MTX_Concat33( &tmp, &mtx, &mtx );

	D3DOBJ_DrawRMtx( &p_wk->obj, &mtx );
//*/	
	for( i=0; i<BCT_MARUNOMU_MDL_NUM; i++ ){
		D3DOBJ_Draw( &p_wk->obj[i] );	
	}
}

//----------------------------------------------------------------------------
/**
 *  @brief  回転角度に対応したアニメーションフレームを設定
 *
 *  @param  p_wk        ワーク
 *  @param  rotax       X軸回転角度
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MarunomuDrawAnmRotaSet( BCT_MARUNOMU_DRAW* p_wk, u32 rotax )
{
    fx32 frame;


    // X軸回転角度のアニメフレームを設定
	// 90がBCT_MARUNOMU_ANM_FRAME_MAX
	rotax -= BCT_MARUNOMU_ROTA_X_MIN;
    frame = (rotax * 90) / RotKeyR( BCT_MARUNOMU_ROTA_X_MAX_360-BCT_MARUNOMU_ROTA_X_MIN );	// 0～90の値にする
	frame = (frame * BCT_MARUNOMU_ANM_FRAME_MAX) / 90;
	frame = BCT_MARUNOMU_ANM_FRAME_MAX - frame;
    D3DOBJ_AnmSet( &p_wk->anm[BCT_MARUNOMU_ANM_ROTA], frame );
}

//----------------------------------------------------------------------------
/**
 *	@brief	口のアニメを変更する
 *
 *	@param	p_wk		ワーク
 *	@param	anmno		アニメナンバー
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MarunomuDrawSetMouthAnm( BCT_MARUNOMU_DRAW* p_wk, u32 anmno )
{
	GF_ASSERT( anmno < BCT_MARUNOMU_ANM_NUM );
	
	if( p_wk->set_mouthanm != anmno ){
		
		D3DOBJ_DelAnm( &p_wk->obj[ sc_BCT_MARUNOMU_ANM_MDL[p_wk->set_mouthanm] ], &p_wk->anm[ p_wk->set_mouthanm ] );
		D3DOBJ_AddAnm( &p_wk->obj[ sc_BCT_MARUNOMU_ANM_MDL[anmno] ], &p_wk->anm[ anmno ] );

		D3DOBJ_AnmSet( &p_wk->anm[ anmno ], 0 );

		// オブジェの表示ONOFF
		D3DOBJ_SetDraw( &p_wk->obj[ sc_BCT_MARUNOMU_ANM_MDL[p_wk->set_mouthanm] ], FALSE );	
		D3DOBJ_SetDraw( &p_wk->obj[ sc_BCT_MARUNOMU_ANM_MDL[anmno] ], TRUE );	

		p_wk->set_mouthanm = anmno;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	口アニメのループ再生
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MarunomuDrawLoopMouthAnm( BCT_MARUNOMU_DRAW* p_wk )
{
	D3DOBJ_AnmLoop( &p_wk->anm[ p_wk->set_mouthanm ], BCT_MARUNOMU_ANM_SPEED );
}

//----------------------------------------------------------------------------
/**
 *	@brief	口アニメのループなし再生
 *
 *	@param	p_wk	ワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
static BOOL BCT_CLIENT_MarunomuDrawNoLoopMouthAnm( BCT_MARUNOMU_DRAW* p_wk )
{
	return D3DOBJ_AnmNoLoop( &p_wk->anm[ p_wk->set_mouthanm ], BCT_MARUNOMU_ANM_SPEED );
}

//----------------------------------------------------------------------------
/**
 *	@brief	口アニメのループなし再生	アニメスピード指定
 *
 *	@param	p_wk		ワーク
 *	@param	speed		スピード
 */
//-----------------------------------------------------------------------------
static BOOL BCT_CLIENT_MarunomuDrawNoLoopMouthAnm_Speed( BCT_MARUNOMU_DRAW* p_wk, fx32 speed )
{
	return D3DOBJ_AnmNoLoop( &p_wk->anm[ p_wk->set_mouthanm ], speed );
}

//----------------------------------------------------------------------------
/**
 *	@brief	歩きアニメ設定
 *
 *	@param	p_wk		ワーク
 *	@param	flag		フラグ
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MarunomuDrawSetWalkAnm( BCT_MARUNOMU_DRAW* p_wk, BOOL flag )
{
	if( p_wk->walk_anm_flag != flag ){

		if( flag == TRUE ){
			D3DOBJ_AddAnm( &p_wk->obj[ sc_BCT_MARUNOMU_ANM_MDL[BCT_MARUNOMU_ANM_WALK] ], &p_wk->anm[ BCT_MARUNOMU_ANM_WALK ] );
		}else{
			D3DOBJ_DelAnm( &p_wk->obj[ sc_BCT_MARUNOMU_ANM_MDL[BCT_MARUNOMU_ANM_WALK] ], &p_wk->anm[ BCT_MARUNOMU_ANM_WALK ] );
		}

		p_wk->walk_anm_flag = flag;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	カラーアニメ	ランダムカラーアニメを設定するか
 *
 *	@param	p_wk		ワーク
 *	@param	flag		フラグ	TRUE：ランダムアニメ再生
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MarunomuDrawSetColAnmRand( BCT_MARUNOMU_DRAW* p_wk, BOOL flag )
{
	p_wk->col_rand = flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief	カラーアニメ	１位の色変更
 *
 *	@param	p_wk		ワーク
 *	@param	plno		プレイヤーナンバー
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MarunomuDrawSetColAnmTop( BCT_MARUNOMU_DRAW* p_wk, u32 plno )
{
	p_wk->col_top = plno;
}

//----------------------------------------------------------------------------
/**
 *	@brief	カラーアニメメイン
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MarunomuDrawColAnmMain( BCT_MARUNOMU_DRAW* p_wk )
{
	int i;

	if( p_wk->col_rand == TRUE ){
		// 色をループ再生
		if( (p_wk->colanm_frame + BCT_MARUNOMU_ANM_SPEED) < BCT_MARUNOMU_ANM_COL_END ){
			p_wk->colanm_frame += BCT_MARUNOMU_ANM_SPEED;
		}else{
			p_wk->colanm_frame = BCT_MARUNOMU_ANM_COL_1PSTART;
		}
	}else{
		u32 anm_idx;

		if( p_wk->col_top == BCT_PLAYER_NUM ){
			anm_idx = BCT_MARUNOMU_ANM_COL_NORMAL;
		}else{
			anm_idx = BCT_MARUNOMU_ANM_COL_1P + p_wk->col_top;
		}
		// トップカラーのところでループ再生
		// 開始フレームになもなってないとき
		if( p_wk->colanm_frame < (sc_BCT_MARUNOMU_ANM_COL_DATA[anm_idx][BCT_MARUNOMU_ANM_COL_DATA_START] * FX32_ONE) ){
			p_wk->colanm_frame = sc_BCT_MARUNOMU_ANM_COL_DATA[anm_idx][BCT_MARUNOMU_ANM_COL_DATA_START] * FX32_ONE;
		}
		// 終了フレームになるまで再生
		else if( (p_wk->colanm_frame + BCT_MARUNOMU_ANM_SPEED) < (sc_BCT_MARUNOMU_ANM_COL_DATA[anm_idx][BCT_MARUNOMU_ANM_COL_DATA_END] * FX32_ONE) ){
			p_wk->colanm_frame += BCT_MARUNOMU_ANM_SPEED;
		}
		// 最初のところにループ
		else{
			p_wk->colanm_frame = sc_BCT_MARUNOMU_ANM_COL_DATA[anm_idx][BCT_MARUNOMU_ANM_COL_DATA_START] * FX32_ONE;
		}
	}

	for( i=0; i<BCT_MARUNOMU_ANM_COLANM_NUM; i++ ){
		D3DOBJ_AnmSet( &p_wk->colanm[i], p_wk->colanm_frame );
	}
}

//----------------------------------------------------------------------------
/**
 *  @brief  タッチパネル2D座標を３D座標に変更する
 *
 *  @param  x           ｘ座標
 *  @param  y           ｙ座標
 *  @param  p_mat       3D座標格納先
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_2DMATto3DMAT( s16 x, s16 y, VecFx32* p_mat )
{
    p_mat->x = BCT_FIELD_DOT2MAT( (255 - x) );
    p_mat->z = BCT_FIELD_PLAYER_Z - BCT_FIELD_DOT2MAT( y );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ３D座標をタッチパネル座標に変更
 *
 *  @param  cp_mat      ３D座標
 *  @param  p_2dmat     ２D座標格納先 
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_3DMATto2DMAT( const VecFx32* cp_mat, NNSG2dSVec2* p_2dmat )
{
    p_2dmat->x = 255 - BCT_FIELD_MAT2DOT( cp_mat->x );
    p_2dmat->y = BCT_FIELD_MAT2DOT( (-cp_mat->z) + BCT_FIELD_PLAYER_Z );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ベクトルをNETIDに関連付けて回転させる
 *
 *  @param  p_vec   ベクトル
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_VecNetIDRot( const VecFx32* cp_vec, VecFx32* p_invec, u32 plno, u32 comm_num )
{
    MtxFx33 rot;

    MTX_Identity33( &rot );
    MTX_RotY33( &rot, FX_SinIdx(BCT_NUTS_FIELD_NetIDRota[comm_num-1][ plno ]), 
            FX_CosIdx(BCT_NUTS_FIELD_NetIDRota[comm_num-1][ plno ]) );
    MTX_MultVec33( cp_vec, &rot, p_invec );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ベクトルをNETIDに関連付けて逆回転させる
 *          回転した座標を元に戻すときなどに使用
 *
 *  @param  cp_vec
 *  @param  p_invec
 *  @param  plno 
 *  @param  comm_num
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_VecNetIDRetRot( const VecFx32* cp_vec, VecFx32* p_invec, u32 plno, u32 comm_num )
{
    MtxFx33 rot;

    MTX_Identity33( &rot );
    MTX_RotY33( &rot, FX_SinIdx(BCT_NUTS_FIELD_NetIDRota[ comm_num-1 ][ plno ]), 
            FX_CosIdx(BCT_NUTS_FIELD_NetIDRota[ comm_num-1 ][ plno ]) );
    MTX_Inverse33( &rot, &rot );
    MTX_MultVec33( cp_vec, &rot, p_invec );
}

//----------------------------------------------------------------------------
/**
 *  @brief  木の実描画システム　初期化
 *
 *  @param  p_wk        ワーク
 *  @param  p_handle    ハンドル
 *  @param  heapID      ヒープ
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsDrawSysInit( BCT_CLIENT_GRAPHIC* p_wk, ARCHANDLE* p_handle, u32 heapID  )
{
    BOOL result;
    int i;
	static const u8 sc_BCT_NUTSRES_MDL_TBL[ BCT_NUTSRES_MDLNUM ] = {
		NARC_bucket_kinomi_1p_nsbmd,
		NARC_bucket_kinomi_2p_nsbmd,
		NARC_bucket_kinomi_3p_nsbmd,
		NARC_bucket_kinomi_4p_nsbmd,
		NARC_bucket_kinomi_ex_nsbmd,
	};
    
    // 描画データ作成
	for( i=0; i<BCT_NUTSRES_MDLNUM; i++ ){
	    D3DOBJ_MdlLoadH( &p_wk->nutsres.mdl[i], p_handle, sc_BCT_NUTSRES_MDL_TBL[i], heapID );
	}

	// 陰読み込み
	D3DOBJ_MdlLoadH( &p_wk->nutsres.shadowmdl, p_handle, NARC_bucket_maru_kage_nsbmd, heapID );
	NNS_G3dMdlUseGlbAlpha( p_wk->nutsres.shadowmdl.pModel );	// アルファ値はプログラムのものを参照

    {
        // OAMリソース読込み
        p_wk->nutsres.resobj[ 0 ] = CLACT_U_ResManagerResAddArcChar_ArcHandle(
                    p_wk->resMan[ 0 ], p_handle,
                    NARC_bucket_kinomi_01_NCGR,
                    FALSE, BCT_GRA_NUTS_OAM_RESID, NNS_G2D_VRAM_TYPE_2DSUB, heapID );

        p_wk->nutsres.resobj[ 1 ] = CLACT_U_ResManagerResAddArcPltt_ArcHandle(
                    p_wk->resMan[ 1 ], p_handle,
                    NARC_bucket_kinomi_NCLR,
                    FALSE, BCT_GRA_NUTS_OAM_RESID, NNS_G2D_VRAM_TYPE_2DSUB, 4, heapID );

        p_wk->nutsres.resobj[ 2 ] = CLACT_U_ResManagerResAddArcKindCell_ArcHandle(
                p_wk->resMan[ 2 ], p_handle,
                NARC_bucket_kinomi_01_NCER, FALSE,
                BCT_GRA_NUTS_OAM_RESID, CLACT_U_CELL_RES, heapID );

        p_wk->nutsres.resobj[ 3 ] = CLACT_U_ResManagerResAddArcKindCell_ArcHandle(
                p_wk->resMan[ 3 ], p_handle,
                NARC_bucket_kinomi_01_NANR, FALSE,
                BCT_GRA_NUTS_OAM_RESID, CLACT_U_CELLANM_RES, heapID );

        // 転送
        result = CLACT_U_CharManagerSetCharModeAdjustAreaCont( p_wk->nutsres.resobj[ 0 ] );
        GF_ASSERT( result );
        result = CLACT_U_PlttManagerSetCleanArea( p_wk->nutsres.resobj[ 1 ] );
        GF_ASSERT( result );

        // リソースだけ破棄
        CLACT_U_ResManagerResOnlyDelete( p_wk->nutsres.resobj[ 0 ] );
        CLACT_U_ResManagerResOnlyDelete( p_wk->nutsres.resobj[ 1 ] );
    }

    // セルアクターヘッダー作成
    CLACT_U_MakeHeader( &p_wk->nutsres.header, 
            BCT_GRA_NUTS_OAM_RESID, BCT_GRA_NUTS_OAM_RESID,
            BCT_GRA_NUTS_OAM_RESID, BCT_GRA_NUTS_OAM_RESID,
            CLACT_U_HEADER_DATA_NONE, CLACT_U_HEADER_DATA_NONE,0,
            BCT_GRA_NUTS_OAM_BGPRI,
            p_wk->resMan[0], p_wk->resMan[1], 
            p_wk->resMan[2], p_wk->resMan[3],
            NULL, NULL );

    // 各ワークの初期化
    for( i=0; i<BCT_NUTSBUFFOAM_NUM; i++ ){
        BCT_CLIENT_NutsDrawInit( p_wk, &p_wk->nuts[i], heapID );
    }
}

//----------------------------------------------------------------------------
/**
 *  @brief  木の実描画システム破棄
 *
 *  @param  p_wk    ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsDrawSysExit( BCT_CLIENT_GRAPHIC* p_wk )
{
    int i;

    // 各木の実破棄
    for( i=0; i<BCT_NUTSBUFFOAM_NUM; i++ ){
        BCT_CLIENT_NutsDrawExit( p_wk, &p_wk->nuts[i] );
    }

    // リソース破棄
    {

        // VRAM管理から破棄
        CLACT_U_CharManagerDelete( p_wk->nutsres.resobj[0] );
        CLACT_U_PlttManagerDelete( p_wk->nutsres.resobj[1] );
        
        // リソース破棄
        CLACT_U_ResManagerResDelete( p_wk->resMan[0], p_wk->nutsres.resobj[0] );
        CLACT_U_ResManagerResDelete( p_wk->resMan[1], p_wk->nutsres.resobj[1] );
        CLACT_U_ResManagerResDelete( p_wk->resMan[2], p_wk->nutsres.resobj[2] );
        CLACT_U_ResManagerResDelete( p_wk->resMan[3], p_wk->nutsres.resobj[3] );
    }
    for( i=0; i<BCT_NUTSRES_MDLNUM; i++ ){
        D3DOBJ_MdlDelete( &p_wk->nutsres.mdl[i] );
    }
    D3DOBJ_MdlDelete( &p_wk->nutsres.shadowmdl );

    // 全データ破棄
    memset( &p_wk->nutsres, 0, sizeof(BCT_CLIENT_NUTS_RES) );
    memset( &p_wk->nuts, 0, sizeof(BCT_CLIENT_NUTS_DRAW)*BCT_NUTSBUFFOAM_NUM );
}

//----------------------------------------------------------------------------
/**
 *  @brief  木の実描画設定
 *
 *  @param  p_wk        ワーク
 *  @param  cp_data     表示する木の実データ
 *  @param  comm_num    通信人数
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsDrawSysStart( BCT_CLIENT_GRAPHIC* p_wk, const BCT_CLIENT_NUTS* cp_data, u32 comm_num, u32 plno )
{
    BCT_CLIENT_NUTS_DRAW* p_drawwk;
    p_drawwk = BCT_CLIENT_NutsDrawWkGet( p_wk );

    if( cp_data->data.plno == plno ){
        BCT_CLIENT_NutsDrawStart( p_wk, p_drawwk, cp_data, comm_num );
//      BCT_CLIENT_NutsDrawStartNoOam( p_wk, p_drawwk, cp_data );
    }else{
        BCT_CLIENT_NutsDrawStartNoOam( p_wk, p_drawwk, cp_data );
    }
}

//----------------------------------------------------------------------------
/**
 *  @brief  描画停止
 *  
 *  @param  p_wk        ワーク
 *  @param  cp_data     木の実データ
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsDrawSysEnd( BCT_CLIENT_GRAPHIC* p_wk, const BCT_CLIENT_NUTS* cp_data )
{
    int i;

    for( i=0; i<BCT_NUTSBUFFOAM_NUM; i++ ){
        if( p_wk->nuts[i].cp_data == cp_data ){
            // 停止
            BCT_CLIENT_NutsDrawEnd( &p_wk->nuts[i] );
            return ;
        }
    }
    GF_ASSERT( 0 );
}


//----------------------------------------------------------------------------
/**
 *  @brief  木の実描画メイン
 *
 *  @param  p_wk        ワーク
 *  @param  comm_num    通信人数
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsDrawSysMain( BCT_CLIENT_GRAPHIC* p_wk, u32 comm_num )
{
    int i;

    for( i=0; i<BCT_NUTSBUFFOAM_NUM; i++ ){
        if( p_wk->nuts[i].cp_data != NULL ){
            BCT_CLIENT_NutsDrawMain( &p_wk->nuts[i], p_wk, comm_num );
        }
    }
}


//----------------------------------------------------------------------------
/**
 *  @brief  描画作成
 *
 *  @param  p_wk        グラフィックデータ
 *  @param  p_data      ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsDrawInit( BCT_CLIENT_GRAPHIC* p_wk, BCT_CLIENT_NUTS_DRAW* p_data, u32 heapID )
{
    // OAM作成
    {
        CLACT_ADD add = {0};

        add.ClActSet = p_wk->clactSet;
        add.ClActHeader = &p_wk->nutsres.header;
        add.sca.x = FX32_ONE;
        add.sca.y = FX32_ONE;
        add.sca.z = FX32_ONE;
        add.pri   = BCT_GRA_NUTS_OAM_PRI;
        add.DrawArea = NNS_G2D_VRAM_TYPE_2DSUB;
        add.heap  = heapID;
        p_data->p_clwk = CLACT_Add( &add );
        CLACT_SetDrawFlag( p_data->p_clwk, FALSE );
    }

	// 陰グラフィック設定
    D3DOBJ_Init( &p_data->shadow, &p_wk->nutsres.shadowmdl );
	D3DOBJ_SetDraw( &p_data->shadow, FALSE );
}

//----------------------------------------------------------------------------
/**
 *  @brief  木の実描画データ　破棄
 *
 *  @param  p_wk        グラフィックワーク
 *  @param  p_data      木の実グラフィックワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsDrawExit( BCT_CLIENT_GRAPHIC* p_wk, BCT_CLIENT_NUTS_DRAW* p_data )
{
    CLACT_Delete( p_data->p_clwk );
    memset( p_data, 0, sizeof(BCT_CLIENT_NUTS_DRAW) );
}

//----------------------------------------------------------------------------
/**
 *  @brief  木の実描画開始
 *
 *  @param  p_wk    ワーク
 *  @param  p_data  木の実グラフィックワーク
 *  @param  cp_data 木の実データ
 *  @param  comm_num通信人数
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsDrawStart( BCT_CLIENT_GRAPHIC* p_wk, BCT_CLIENT_NUTS_DRAW* p_data, const BCT_CLIENT_NUTS* cp_data, u32 comm_num )
{
    // 関連付ける木の実データ保存
    p_data->cp_data = cp_data;
    p_data->draw2d = TRUE;

	p_data->rota_chg_count = 0;

	// パレットオフセット設定
	CLACT_PaletteNoChg( p_data->p_clwk, cp_data->data.plno );
    
    // 座標設定
    BCT_CLIENT_NutsDrawMatrixSet( p_data, cp_data, comm_num );

    // 表示開始
    CLACT_SetDrawFlag( p_data->p_clwk, TRUE );


#ifdef BCT_DEBUG
    BCT_DEBUG_NutsDrawNum++;
    OS_Printf( "nuts draw %d\n", BCT_DEBUG_NutsDrawNum );
#endif
}

//----------------------------------------------------------------------------
/**
 *  @brief  いきなり３D描画モード
 *
 *  @param  p_wk    ワーク
 *  @param  p_data  木の実グラフィックワーク
 *  @param  cp_data 木の実データ
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsDrawStartNoOam( BCT_CLIENT_GRAPHIC* p_wk, BCT_CLIENT_NUTS_DRAW* p_data, const BCT_CLIENT_NUTS* cp_data )
{
    // 関連付ける木の実データ保存
    p_data->cp_data = cp_data;

	p_data->rota_chg_count = 0;

    // 表示開始
    CLACT_SetDrawFlag( p_data->p_clwk, FALSE );
    BCT_CLIENT_Nuts3DDrawOn( p_data, p_wk );

#ifdef BCT_DEBUG
    BCT_DEBUG_NutsDrawNum++;
    OS_Printf( "nuts draw %d\n", BCT_DEBUG_NutsDrawNum );
#endif
}

//----------------------------------------------------------------------------
/**
 *  @brief  木の実描画メイン
 *
 *  @param  p_data  ワーク
 *  @param  comm_num通信人数
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsDrawMain( BCT_CLIENT_NUTS_DRAW* p_data, BCT_CLIENT_GRAPHIC* p_wk, u32 comm_num )
{
    BOOL result;
	D3DOBJ_MDL* p_mdl;

    // 2D表示か３d表示か
    if( p_data->draw2d == TRUE ){
        // 座標をあわせる
        result = BCT_CLIENT_NutsDrawMatrixSet( p_data, p_data->cp_data, comm_num );

        //  2D座標が０以下になったら３D表示に切り替える
        if( result == FALSE ){
            BCT_CLIENT_Nuts3DDrawOn( p_data, p_wk );
        }
    }

	if( p_data->draw2d == FALSE ){
		p_mdl = BCT_CLIENT_Nuts3DMdlGet( p_data->cp_data, &p_wk->nutsres );

		// 木の実が場外動作なら暗く表示、そうでなければ明るく表示
		if( p_data->cp_data->seq == BCT_NUTSSEQ_MOVEAWAY ){
			
			// 暗くする
			NNS_G3dMdlSetMdlAmbAll( p_mdl->pModel, GX_RGB( 31,31,31 ) );
//			NNS_G3dMdlSetMdlAmbAll( p_mdl->pModel, GX_RGB( 24,24,24 ) );
		}else{
			// 明るくする
			NNS_G3dMdlSetMdlAmbAll( p_mdl->pModel, GX_RGB( 18,18,18 ) );
		}
		

		// 描画
		// 進んでいる方向に回転をかける
		BCT_CLIENT_NutsDrawRotaSet( p_data );
		// 座標の設定
		D3DOBJ_SetMatrix( &p_data->obj, 
				p_data->cp_data->mat.x, p_data->cp_data->mat.y, p_data->cp_data->mat.z  );
		D3DOBJ_Draw( &p_data->obj );

		// 陰の表示
		if( D3DOBJ_GetDraw( &p_data->shadow ) == TRUE ){
			BCT_CLIENT_NutsDrawShadowMatrixSet( p_data );
			D3DOBJ_Draw( &p_data->shadow );
			BCT_CLIENT_NutsDrawShadowAlpahReset( p_data );

		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	陰の座標を設定する
 *
 *	@param	p_data	ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsDrawShadowMatrixSet( BCT_CLIENT_NUTS_DRAW* p_data )
{
	BOOL result;
	fx32 y_dif;
	fx32 y;
	fx32 scale;
	int alpha;

	// マルノーム範囲内かチェック
	result = BCT_CLIENT_NutsAwayMarunomuHitCheck( p_data->cp_data );
	if( result == TRUE ){
		// マルノームの陰の位置に移動
		y = BCT_SHADOW_Y_DEL;
	}else{

		// XZ座標はそのままあわせて、Y座標を床にする
		result = BCT_CLIENT_NutsAwayDitchHitCheck( p_data->cp_data );
		if( result == FALSE ){
			// 床の位置に陰を出す
			y = BCT_SHADOW_Y_FLOOR;
		}else{
			// 溝の位置に陰を出す
			y = BCT_SHADOW_Y_DEL;
		}
	}

	D3DOBJ_SetMatrix( &p_data->shadow, 
			p_data->cp_data->mat.x, y, p_data->cp_data->mat.z  );

	// 床からの高さで大きさを決める
	y_dif = p_data->cp_data->mat.y - y;
	scale = FX32_ONE + (FX_Div(FX_Mul( y_dif, BCT_SHADOW_SIZE_DIV ), BCT_SHADOW_Y_SIZE_DIF));
	D3DOBJ_SetScale( &p_data->shadow, 
			scale, scale, scale );

	alpha = (FX_Div(FX_Mul( y_dif, FX32_CONST(BCT_SHADOW_ALPHA_DIV) ), BCT_SHADOW_Y_SIZE_DIF)) >> FX32_SHIFT;
	alpha = BCT_SHADOW_ALPHA_MIN + BCT_SHADOW_ALPHA_DIV - alpha;

	// ALPHAもかえる
	NNS_G3dGlbPolygonAttr(
		0,
		0,
		0,
		0,
		alpha,
		0 );
}

//----------------------------------------------------------------------------
/**
 *	@brief	陰で設定したアルファ値をリセット
 *
 *	@param	p_data		データ
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsDrawShadowAlpahReset( BCT_CLIENT_NUTS_DRAW* p_data )
{
	NNS_G3dGlbPolygonAttr(
		0,
		0,
		0,
		0,
		31,
		0 );
}

//----------------------------------------------------------------------------
/**
 *  @brief  表示の停止
 *
 *  @param  p_data  ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsDrawEnd( BCT_CLIENT_NUTS_DRAW* p_data )
{
    p_data->cp_data = NULL;
    p_data->draw2d = FALSE;
    CLACT_SetDrawFlag( p_data->p_clwk, FALSE );

	D3DOBJ_SetDraw( &p_data->shadow, FALSE );

#ifdef BCT_DEBUG
    BCT_DEBUG_NutsDrawNum--;
    OS_Printf( "nuts draw %d\n", BCT_DEBUG_NutsDrawNum );
#endif
}


//----------------------------------------------------------------------------
/**
 *  @brief  空いているワークを取得する
 *
 *  @param  p_wk    ワーク
 *
 *  @return 木の実ワーク
 */
//-----------------------------------------------------------------------------
static BCT_CLIENT_NUTS_DRAW* BCT_CLIENT_NutsDrawWkGet( BCT_CLIENT_GRAPHIC* p_wk )
{
    int i;  

    for( i=0; i<BCT_NUTSBUFFOAM_NUM; i++ ){
        if( p_wk->nuts[i].cp_data == NULL ){
            return &p_wk->nuts[i];
        }
    }
    GF_ASSERT( 0 );
    return NULL;
}

//----------------------------------------------------------------------------
/**
 *  @brief  描画座標を設定
 *
 *  @param  p_data  ワーク
 *  @param  cp_data 木の実データ
 *
 *  @retval TRUE    画面範囲内
 *  @retval FALSE   画面範囲外
 */
//-----------------------------------------------------------------------------
static BOOL BCT_CLIENT_NutsDrawMatrixSet( BCT_CLIENT_NUTS_DRAW* p_data, const BCT_CLIENT_NUTS* cp_data, u32 comm_num )
{
    VecFx32 mat;
    NNSG2dSVec2 vec2d;

    mat = p_data->cp_data->mat;
    BCT_CLIENT_VecNetIDRetRot( &mat, &mat, p_data->cp_data->data.plno, comm_num ); // 回転しているのを元に戻す
    BCT_CLIENT_3DMATto2DMAT( &mat, &vec2d );

    mat.x = vec2d.x << FX32_SHIFT;
    mat.y = (vec2d.y << FX32_SHIFT) + BCT_GRA_OAMSUBSURFACE_Y;
    mat.z = 0;

    CLACT_SetMatrix( p_data->p_clwk, &mat );

    if( (vec2d.x < 0) || (vec2d.y < 0) ){
        return FALSE;
    }
    return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  3D描画開始
 *
 *  @param  p_data  ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_Nuts3DDrawOn( BCT_CLIENT_NUTS_DRAW* p_data, BCT_CLIENT_GRAPHIC* p_wk )
{
	D3DOBJ_MDL* p_mdl;
	
    CLACT_SetDrawFlag( p_data->p_clwk, FALSE );
    p_data->draw2d = FALSE;

	p_mdl = BCT_CLIENT_Nuts3DMdlGet( p_data->cp_data, &p_wk->nutsres );

	D3DOBJ_Init( &p_data->obj, p_mdl );
	D3DOBJ_SetDraw( &p_data->obj, TRUE );

	// 自分の木の実なら影を出す
	if( p_data->cp_data->seq == BCT_NUTSSEQ_MOVE ){
		D3DOBJ_SetDraw( &p_data->shadow, TRUE );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	木の実データから	表示するモデルのリソースを取得する
 *	
 *	@param	cp_data		データ
 *	@param	p_nutsres	木の実リソースデータ
 *
 *	@return	モデルワーク
 */
//-----------------------------------------------------------------------------
static D3DOBJ_MDL* BCT_CLIENT_Nuts3DMdlGet( const BCT_CLIENT_NUTS* cp_data, BCT_CLIENT_NUTS_RES* p_nutsres )
{
	u32 mdlno;

    // 3D作成
	// レンダーオブジェクトに登録
	if( cp_data->data.special == TRUE ){	// 特殊木の実
		mdlno = BCT_NUTSRES_MDLSPECIAL;
	}else{
		mdlno = cp_data->data.plno;
	}

	return &p_nutsres->mdl[ mdlno ];
}

//----------------------------------------------------------------------------
/**
 *	@brief	木の実描画　回転角度設定
 *
 *	@param	p_data	データ
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NutsDrawRotaSet( BCT_CLIENT_NUTS_DRAW* p_data )
{
	fx32 power;
	u16 move_rota;
	u16 rota_speed;

	// 毎回回転角度を変えてると、処理が重いのでたまに行う
	p_data->rota_chg_count --;
	if( p_data->rota_chg_count < 0 ){
		power = BCT_CLIENT_NutsNowPowerGet( p_data->cp_data );
		rota_speed = ((power*BCT_NUTSDRAW_ROTA_DIS) / BCT_NUTS_POWER_MAX);

		// 移動方向の割合で回転方向を変える
		move_rota = FX_Atan2Idx( p_data->cp_data->data.way.z, p_data->cp_data->data.way.x );
		p_data->rota_speed_x = BCT_NUTSDRAW_ROTA_MIN+(FX_Mul( FX_SinIdx( move_rota ), rota_speed<<FX32_SHIFT ) >> FX32_SHIFT);
		p_data->rota_speed_z = BCT_NUTSDRAW_ROTA_MIN+(FX_Mul( FX_CosIdx( move_rota ), rota_speed<<FX32_SHIFT ) >> FX32_SHIFT);

		p_data->rota_chg_count = BCT_NUTSDRAW_ROTA_CHG_TIMING;
	}

	
	p_data->rota_x += p_data->rota_speed_x;
	p_data->rota_z += p_data->rota_speed_z;
	D3DOBJ_SetRota( &p_data->obj, p_data->rota_x, D3DOBJ_ROTA_WAY_X );
	D3DOBJ_SetRota( &p_data->obj, p_data->rota_z, D3DOBJ_ROTA_WAY_Z );
}

//----------------------------------------------------------------------------
/**
 *  @brief  持っている木の実　初期化
 *
 *  @param  p_wk        ワーク
 *  @param  p_nuts      木の実ワーク
 *  @param	plno		通信ID
 *  @param  heapID      ヒープID
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_HandNutsDrawInit( BCT_CLIENT_GRAPHIC* p_wk, BCT_CLIENT_HANDNUTS_DRAW* p_nuts, u32 plno, u32 heapID )
{
    CLACT_ADD add = {0};

    add.ClActSet = p_wk->clactSet;
    add.ClActHeader = &p_wk->nutsres.header;
    add.sca.x = FX32_ONE;
    add.sca.y = FX32_ONE;
    add.sca.z = FX32_ONE;
    add.pri   = BCT_GRA_NUTS_OAM_PRI;
    add.DrawArea = NNS_G2D_VRAM_TYPE_2DSUB;
    add.heap  = heapID;
    p_nuts->p_clwk = CLACT_Add( &add );
    CLACT_SetDrawFlag( p_nuts->p_clwk, FALSE );
	CLACT_PaletteNoChg( p_nuts->p_clwk, plno );
    p_nuts->draw = FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  持っている木の実描画データ　破棄
 *
 *  @param  p_nuts  木の実ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_HandNutsDrawExit( BCT_CLIENT_HANDNUTS_DRAW* p_nuts )
{
    CLACT_Delete( p_nuts->p_clwk );
}

//----------------------------------------------------------------------------
/**
 *  @brief  持っている木の実描画開始
 *
 *  @param   p_nuts     木の実ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_HandNutsDrawStart( BCT_CLIENT_HANDNUTS_DRAW* p_nuts )
{
    CLACT_SetDrawFlag( p_nuts->p_clwk, TRUE );
    p_nuts->draw = TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  持っている木の実描画　メイン
 *
 *  @param  p_nuts      木の実ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_HandNutsDrawMain( BCT_CLIENT_HANDNUTS_DRAW* p_nuts )
{   
    VecFx32 mat;
    
    // 表示中しか動かない
    if( p_nuts->draw == FALSE ){
        return ;
    }

    // 今のタッチパネル座標に設定する
	BCT_CLIENT_HandNutsDrawSetMatrix( p_nuts, sys.tp_x, sys.tp_y );
}

//----------------------------------------------------------------------------
/**
 *	@brief	座標の設定
 *
 *	@param	p_nuts		ワーク
 *	@param	x			ｘ座標
 *	@param	y			ｙ座標
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_HandNutsDrawSetMatrix( BCT_CLIENT_HANDNUTS_DRAW* p_nuts, s32 x, s32 y )
{
    VecFx32 mat;

    mat.x = x << FX32_SHIFT;
    mat.y = (y << FX32_SHIFT) + BCT_GRA_OAMSUBSURFACE_Y;
    CLACT_SetMatrix( p_nuts->p_clwk, &mat );
}

//----------------------------------------------------------------------------
/**
 *  @brief  持っている木の実描画　終了
 *
 *  @param  p_nuts      木の実ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_HandNutsDrawEnd( BCT_CLIENT_HANDNUTS_DRAW* p_nuts )
{
    CLACT_SetDrawFlag( p_nuts->p_clwk, FALSE );
    p_nuts->draw = FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	OAMの木の実を画面外へ出すエフェクト	システム初期化
 *
 *	@param	p_drawsys		描画システム
 *	@param	p_wk			このシステムのワーク
 *	@param	heapID			ヒープID
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_OamAwayNutsInit( BCT_CLIENT_GRAPHIC* p_drawsys, BCT_CLIENT_OAMAWAYNUTS_DRAW* p_wk, u32 plno, u32 heapID )
{
	int i;
    CLACT_ADD add = {0};

    add.ClActSet = p_drawsys->clactSet;
    add.ClActHeader = &p_drawsys->nutsres.header;
    add.sca.x = BCT_OAMAWAYNUTS_SCALE;
    add.sca.y = BCT_OAMAWAYNUTS_SCALE;
    add.sca.z = BCT_OAMAWAYNUTS_SCALE;
    add.pri   = BCT_GRA_NUTS_OAM_PRI;
    add.DrawArea = NNS_G2D_VRAM_TYPE_2DSUB;
    add.heap  = heapID;

	for( i=0; i<BCT_OAMAWAYNUTS_BUFFNUM; i++ ){
	    p_wk->nutsbuff[i].p_clwk = CLACT_Add( &add );
		CLACT_SetDrawFlag( p_wk->nutsbuff[i].p_clwk, FALSE );
		CLACT_SetAffineParam( p_wk->nutsbuff[i].p_clwk, CLACT_AFFINE_NORMAL );
		CLACT_PaletteNoChg( p_wk->nutsbuff[i].p_clwk, plno );
	    p_wk->nutsbuff[i].draw = FALSE;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	OAMの木の実を画面外へ出すエフェクト	システム破棄
 *
 *	@param	p_drawsys		描画システム
 *	@param	p_wk			このシステムのワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_OamAwayNutsExit( BCT_CLIENT_GRAPHIC* p_drawsys, BCT_CLIENT_OAMAWAYNUTS_DRAW* p_wk )
{
	int i;

	for( i=0; i<BCT_OAMAWAYNUTS_BUFFNUM; i++ ){
		CLACT_Delete( p_wk->nutsbuff[i].p_clwk );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	画面外へ移動していく処理
 *
 *	@param	p_wk	このシステムのワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_OamAwayNutsMain( BCT_CLIENT_OAMAWAYNUTS_DRAW* p_wk )
{
	int i;
	s32 count_sub;
	s32 x, y;
	fx32 scale;
	VecFx32 scale_vec;
	
	// power分毎回座標を足して、画面外へ言ったら終わり	
	for( i=0; i<BCT_OAMAWAYNUTS_BUFFNUM; i++ ){
		if( p_wk->nutsbuff[i].draw == TRUE ){

			// ３バウンドしながら落ちていく
			// 1バウンド
			if( p_wk->nutsbuff[i].count < BCT_OAMAWAYNUTS_MOVE0_COUNT ){
				count_sub = p_wk->nutsbuff[i].count;
				BCT_CLIENT_OamAwayNutsMoveXY( count_sub, BCT_OAMAWAYNUTS_MOVE0_COUNT, BCT_OAMAWAYNUTS_MOVE0_SPEED, BCT_OAMAWAYNUTS_MOVE0_SROTA, BCT_OAMAWAYNUTS_MOVE0_EROTA, &x, &y );
			}
			// 2バウンド
			else if( p_wk->nutsbuff[i].count < (BCT_OAMAWAYNUTS_MOVE0_COUNT+BCT_OAMAWAYNUTS_MOVE1_COUNT) ){
				count_sub = p_wk->nutsbuff[i].count - BCT_OAMAWAYNUTS_MOVE0_COUNT;
				
				BCT_CLIENT_OamAwayNutsMoveXY( count_sub, BCT_OAMAWAYNUTS_MOVE1_COUNT, BCT_OAMAWAYNUTS_MOVE1_SPEED, BCT_OAMAWAYNUTS_MOVE1_SROTA, BCT_OAMAWAYNUTS_MOVE1_EROTA, &x, &y );
			}
			// 3バウンド
			else{
				count_sub = p_wk->nutsbuff[i].count - (BCT_OAMAWAYNUTS_MOVE0_COUNT+BCT_OAMAWAYNUTS_MOVE1_COUNT);

				BCT_CLIENT_OamAwayNutsMoveXY( count_sub, BCT_OAMAWAYNUTS_MOVE2_COUNT, BCT_OAMAWAYNUTS_MOVE2_SPEED, BCT_OAMAWAYNUTS_MOVE2_SROTA, BCT_OAMAWAYNUTS_MOVE2_EROTA, &x, &y );
			}

			p_wk->nutsbuff[i].count++;

			// 小さくしていく
			scale = BCT_OAMAWAYNUTS_SCALE - ( (p_wk->nutsbuff[i].count * BCT_OAMAWAYNUTS_SCALE_DIV) / BCT_OAMAWATNUTS_MOVE_COUNT);
			scale = FX_Div( scale << FX32_SHIFT, FX32_CONST(10) ); 
			scale_vec.x = scale;
			scale_vec.y = scale;
			CLACT_SetScale( p_wk->nutsbuff[i].p_clwk, &scale_vec );	

			
			if( p_wk->nutsbuff[i].left ){
				p_wk->nutsbuff[i].mat.x += x << FX32_SHIFT;
			}else{
				p_wk->nutsbuff[i].mat.x -= x << FX32_SHIFT;
			}
			p_wk->nutsbuff[i].mat.y += y << FX32_SHIFT;
			CLACT_SetMatrix( p_wk->nutsbuff[i].p_clwk, &p_wk->nutsbuff[i].mat );

			if( (p_wk->nutsbuff[i].mat.x < BCT_OAMAWAYNUTS_DEL_XMIN) ||
				(p_wk->nutsbuff[i].mat.x > BCT_OAMAWAYNUTS_DEL_XMAX) ||
				(p_wk->nutsbuff[i].mat.y < BCT_OAMAWAYNUTS_DEL_YMIN + BCT_GRA_OAMSUBSURFACE_Y) ||
				(p_wk->nutsbuff[i].mat.y > BCT_OAMAWAYNUTS_DEL_YMAX + BCT_GRA_OAMSUBSURFACE_Y) ){
				p_wk->nutsbuff[i].draw = FALSE;
				CLACT_SetDrawFlag( p_wk->nutsbuff[i].p_clwk, FALSE );
			}
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	画面外へ移動していく処理	開始命令
 *
 *	@param	p_wk	このシステムのワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_OamAwayNutsStart( BCT_CLIENT_OAMAWAYNUTS_DRAW* p_wk, s32 x, s32 y )
{
	int i;
	BCT_CLIENT_OAMAWAYNUTS* p_obj = NULL;

	// 空いてるワーク取得
	for( i=0; i<BCT_OAMAWAYNUTS_BUFFNUM; i++ ){
		if( p_wk->nutsbuff[i].draw == FALSE ){
			p_obj = &p_wk->nutsbuff[i];
			break;
		}
	}
	if( p_obj == NULL ){
//		GF_ASSERT(0);
		return ;	// これ以上だせない
	}

	// 左右どっちにいるかでエフェクトを変えるくらい
	if( x < 128 ){
		p_obj->left = TRUE;
	}else{
		p_obj->left = FALSE;
	}
	
	// 座標を設定
	p_obj->mat.x = x << FX32_SHIFT;
	p_obj->mat.y = (y << FX32_SHIFT) + BCT_GRA_OAMSUBSURFACE_Y;
	CLACT_SetMatrix( p_obj->p_clwk, &p_obj->mat );

	// 表示、動作開始
	p_obj->draw = TRUE;
	CLACT_SetDrawFlag( p_obj->p_clwk, TRUE );

	// カウンタ初期化
	p_obj->count = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	画面外へ移動していく処理	移動値を求める
 *
 *	@param	count		カウント
 *	@param	countmax	カウント最大値
 *	@param	speed		スピード
 *	@param	srota		開始回転角度
 *	@param	erota		終了回転角度
 *	@param	p_x			ｘ移動値格納先
 *	@param	p_y			ｙ移動値格納先
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_OamAwayNutsMoveXY( int count, int countmax, int speed, int srota, int erota, s32* p_x, s32* p_y )
{
	s16 rota;
	s32 rota_div;

	// 角度の差を求める
	rota_div = erota - srota;

	// カウント力現在の回転角度を求める
	rota = srota + ( (count * rota_div) / countmax );

	// 角度から移動値を求める
	*p_x = FX_Mul( Cos360( rota ), speed << FX32_SHIFT ) >> FX32_SHIFT;
	*p_y = FX_Mul( Sin360( rota ), speed << FX32_SHIFT ) >> FX32_SHIFT;
}

//----------------------------------------------------------------------------
/**
 *	@brief	メイン画面背景　表示	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_graphic	描画システム
 *	@param	p_handle	ハンドル
 *	@param	comm_num	通信人数
 *	@param	plno		通信ID
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MainBackInit( BCT_CLIENT_MAINBACK* p_wk, BCT_CLIENT_GRAPHIC* p_graphic, ARCHANDLE* p_handle, int comm_num, u32 plno, u32 heapID, NNSFndAllocator* p_allocator )
{
	static const u16 CommNumNSBMD[ BCT_PLAYER_NUM ] = {
		NARC_bucket_maru_stage_2p_nsbmd,	// 1人用
		NARC_bucket_maru_stage_2p_nsbmd,	// 2人用
		NARC_bucket_maru_stage_3p_nsbmd,	// 3人用
		NARC_bucket_maru_stage_4p_nsbmd,	// 4人用
	};
	static const u16 sc_MainBackAnm[ BCT_MAINBACK_ANM_NUM ] = {
		NARC_bucket_maru_wall_n_nsbta,
		NARC_bucket_maru_wall_f_nsbta,
		NARC_bucket_maru_wall_f_nsbtp,
	};

	static const u16 sc_AnmSetModel[ BCT_MAINBACK_ANM_NUM ] = {
		BCT_MAINBACK_MDL_WALL_N,
		BCT_MAINBACK_MDL_WALL_F,
		BCT_MAINBACK_MDL_WALL_F,
	};
	u32 idx;
	int i;
	
	for( i=0; i<BCT_MAINBACK_MDL_NUM; i++ ){

		if( i==BCT_MAINBACK_MDL_WALL_N ){
			idx = NARC_bucket_maru_wall_n_nsbmd;
		}else if( i==BCT_MAINBACK_MDL_WALL_F ){
			idx = NARC_bucket_maru_wall_f_nsbmd;
		}else{
			idx = CommNumNSBMD[ comm_num-1 ];
		}
		
		// モデルﾃﾞｰﾀ読み込み
		D3DOBJ_MdlLoadH( &p_wk->mdl[ i ], p_handle, idx, heapID );
		// レンダーオブジェクトに登録
		D3DOBJ_Init( &p_wk->obj[ i ], &p_wk->mdl[ i ] );
		//　座標設定
		D3DOBJ_SetMatrix( &p_wk->obj[ i ], 0, BCT_FIELD_YUKA_DRAW_Y, 0 );
		D3DOBJ_SetScale( &p_wk->obj[ i ], BCT_MAINBACK_SCALE, BCT_MAINBACK_SCALE, BCT_MAINBACK_SCALE );
		
		// FEVER用の壁だけ非表示
		if( i == BCT_MAINBACK_MDL_WALL_F ){
			D3DOBJ_SetDraw( &p_wk->obj[ i ], FALSE );
		}

		// 壁はみんな共通の表示角度で写るようにする
		if( (i==BCT_MAINBACK_MDL_WALL_N) || (i==BCT_MAINBACK_MDL_WALL_F) ){
			D3DOBJ_SetRota( &p_wk->obj[ i ], BCT_CAMERA_ANGLEY_NetID[ comm_num-1 ][ plno ], D3DOBJ_ROTA_WAY_Y );	// １程の角度
		}else{
			D3DOBJ_SetRota( &p_wk->obj[ i ], BCT_MAINBACK_ROT, D3DOBJ_ROTA_WAY_Y );	// １程の角度
		}
	}

	// アニメ読み込み
	for( i=0; i<BCT_MAINBACK_ANM_NUM; i++ ){
		D3DOBJ_AnmLoadH( &p_wk->anm[i], &p_wk->mdl[sc_AnmSetModel[i]], p_handle, 
				sc_MainBackAnm[i], heapID, p_allocator );

		// 登録
		D3DOBJ_AddAnm( &p_wk->obj[sc_AnmSetModel[i]], &p_wk->anm[i] );
	}

	// アニメスピード設定
	p_wk->anm_speed = BCT_FEVER_BACK_ANM_SPEED_START;
}

//----------------------------------------------------------------------------
/**
 *	@brief	メイン画面背景　表示	破棄
 *
 *	@param	p_wk		ワーク
 *	@param	p_graphic	描画システム
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MainBackExit( BCT_CLIENT_MAINBACK* p_wk, BCT_CLIENT_GRAPHIC* p_graphic, NNSFndAllocator* p_allocator )
{
	int i;

	for( i=0; i<BCT_MAINBACK_ANM_NUM; i++ ){
		D3DOBJ_AnmDelete( &p_wk->anm[i], p_allocator );
	}

	for( i=0; i<BCT_MAINBACK_MDL_NUM; i++ ){
	    D3DOBJ_MdlDelete( &p_wk->mdl[i] ); 
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	メイン画面背景　表示処理
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MainBackDraw( BCT_CLIENT_MAINBACK* p_wk )
{
	int i;

	// 常にループアニメ
	D3DOBJ_AnmLoop( &p_wk->anm[BCT_MAINBACK_ANM_WALL_N], p_wk->anm_speed );

	// 今の状態に合わせてアニメ
	if( p_wk->fever ){
		
		switch( p_wk->fever_anm_seq ){
		case 0:
			// フィーバーアニメは、途中からアニメをループさせる
			if( (p_wk->fever_anm_frame + p_wk->fever_anm_speed) < BCT_MAINBACK_FEVER_ANM_FRAME_LOOPS ){
				p_wk->fever_anm_frame += p_wk->fever_anm_speed;
			}else{
				p_wk->fever_anm_frame = 0;
			}

			// アニメスピード加速
/*			if( sys.trg & PAD_BUTTON_A ){
				p_wk->fever_anm_speed += FX32_HALF;
				OS_TPrintf( "anm_speed 0x%x\n", p_wk->fever_anm_speed );
			}//*/
			if( (p_wk->fever_anm_speed + BCT_MARUNOMU_ANM_FEVERWALLSPEED_ADD) <= BCT_MARUNOMU_ANM_FEVERWALLSPEED_E ){
//				OS_TPrintf( "anm_speed 0x%x\n", p_wk->fever_anm_speed );
				p_wk->fever_anm_speed += BCT_MARUNOMU_ANM_FEVERWALLSPEED_ADD;
			}else{
				p_wk->fever_anm_speed = BCT_MARUNOMU_ANM_FEVERWALLSPEED_E;
				p_wk->fever_anm_wait = 24;
				p_wk->fever_anm_seq ++;
			}
			break;

		case 1:

			// フィーバーアニメは、途中からアニメをループさせる
			if( (p_wk->fever_anm_frame + p_wk->fever_anm_speed) < BCT_MAINBACK_FEVER_ANM_FRAME_LOOPS ){
				p_wk->fever_anm_frame += p_wk->fever_anm_speed;
			}else{
				p_wk->fever_anm_frame = 0;
			}

			p_wk->fever_anm_wait --;
			if( p_wk->fever_anm_wait <= 0 ){
				p_wk->fever_anm_seq ++;
				p_wk->fever_anm_frame = BCT_MAINBACK_FEVER_ANM_FRAME_LOOPS;
			}
			break;
			
		case 2:
			// フィーバーアニメは、途中からアニメをループさせる
			if( (p_wk->fever_anm_frame + BCT_MARUNOMU_ANM_FEVERWALLSPEED_SPEED) < BCT_MAINBACK_FEVER_ANM_FRAME_NUM ){
				p_wk->fever_anm_frame += BCT_MARUNOMU_ANM_FEVERWALLSPEED_SPEED;
			}else{
				p_wk->fever_anm_frame = BCT_MAINBACK_FEVER_ANM_FRAME_LOOPS;
			}
			break;

		default:
			GF_ASSERT(0);
			break;
		}

		D3DOBJ_AnmSet( &p_wk->anm[BCT_MAINBACK_ANM_WALL_F], p_wk->fever_anm_frame );
		D3DOBJ_AnmSet( &p_wk->anm[BCT_MAINBACK_ANM_WALL_F_TP], p_wk->fever_anm_frame );
	}

	for( i=0; i<BCT_MAINBACK_MDL_NUM; i++ ){
	    D3DOBJ_Draw( &p_wk->obj[i] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	どっちの壁を表示するのかを設定する
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MainBackSetDrawFever( BCT_CLIENT_MAINBACK* p_wk )
{
	// 壁を切り替える
	D3DOBJ_SetDraw( &p_wk->obj[BCT_MAINBACK_MDL_WALL_N], FALSE );
	D3DOBJ_SetDraw( &p_wk->obj[BCT_MAINBACK_MDL_WALL_F], TRUE );

//	p_wk->fever_anm_frame = BCT_MAINBACK_FEVER_ANM_FRAME_LOOPS;
	p_wk->fever_anm_frame = 0;
	p_wk->fever_anm_speed = BCT_MARUNOMU_ANM_FEVERWALLSPEED_S;
	p_wk->fever = TRUE;
	p_wk->fever_anm_seq = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメスピードを設定する
 *
 *	@param	p_wk		ワーク
 *	@param	speed		スピード
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_MainBackSetAnmSpeed( BCT_CLIENT_MAINBACK* p_wk, fx32 speed )
{
	p_wk->anm_speed = speed;
}


//----------------------------------------------------------------------------
/**
 *  @brief  平面と線分の当たり判定
 *
 *  @param  cp_mat          線分　開始座標
 *  @param  cp_move         線分　移動値
 *  @param  cp_n            平面　法線ベクトル
 *  @param  d               原点からの平面の距離
 *  @param  p_hitmat        ぶつかった座標
 *  @param  p_time          ぶつかる時間
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_CalcPlaneVecHitCheck( const VecFx32* cp_mat, const VecFx32* cp_move, const VecFx32* cp_n, fx32 d, VecFx32* p_hitmat, fx32* p_time )
{
    fx32 time;
    fx32 move_num;
    fx32 mat_num;

    // 法線         (a,b,c)
    // 線分始点 (x0,y0,z0)
    // 線分移動 (xv,yv,zv)
    // かかる時間 t
    // 平面の位置 d
    // t( axv + byv + czv ) = -ax0 - by0 - cz0 + d
    //    move_num              mat_num
    //  t = (mat_num + d) / move_num
    // を使用してtを求める
    move_num = FX_Mul( cp_n->x, cp_move->x ) + FX_Mul( cp_n->y, cp_move->y ) + FX_Mul( cp_n->z, cp_move->z );
    mat_num = -FX_Mul( cp_n->x, cp_mat->x ) - FX_Mul( cp_n->y, cp_mat->y ) - FX_Mul( cp_n->z, cp_mat->z );
    time = FX_Div( (mat_num + d), move_num );

//  OS_Printf( "move_num 0x%x\n", move_num );
//  OS_Printf( "mat_num 0x%x\n", mat_num );

    // tから座標を求める
    p_hitmat->x = cp_mat->x + FX_Mul( cp_move->x, time );
    p_hitmat->y = cp_mat->y + FX_Mul( cp_move->y, time );
    p_hitmat->x = cp_mat->z + FX_Mul( cp_move->z, time );

    *p_time = time;


#if 0

#ifdef BCT_DEBUG
    // 結果確認
    {
        fx32 test_d;

        test_d = FX_Mul( cp_n->x, p_hitmat->x ) + FX_Mul( cp_n->y, p_hitmat->y ) + FX_Mul( cp_n->z, p_hitmat->z );

        OS_Printf( "test_d = 0x%x d = 0x%x\n", test_d, d );
    }
#endif
    
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	カメラターゲットY座標を設定
 *
 *	@param	p_gra	グラフィックワーク
 *	@param	y		ｙ座標
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_CameraTargetYSet( BCT_CLIENT_GRAPHIC* p_gra, fx32 y )
{
	p_gra->target.y	= y;
}

//----------------------------------------------------------------------------
/**
 *	@brief	カメラアングルｘを設定
 *
 *	@param	p_gra		グラフィックワーク
 *	@param	angle		アングル
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_CameraAngleXSet( BCT_CLIENT_GRAPHIC* p_gra, u16 angle )
{
	CAMERA_ANGLE ca_angle;

	ca_angle	= GFC_GetCameraAngle( p_gra->p_camera );
	ca_angle.x	= angle;
	GFC_SetCameraAngleRev( &ca_angle, p_gra->p_camera );
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG優先順位	スクロールシステム	初期化
 *
 *	@param	p_wk	ワーク
 *	@param	plno	プレイヤーなんばー
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_BGPRISCRL_Init( BCT_CLIENT_BGPRI_SCRL* p_wk, u8 plno )
{
	memset( p_wk, 0, sizeof(BCT_CLIENT_BGPRI_SCRL) );
	p_wk->most_back = BCT_BGPRI_SCRL_START_MOSTBACK;
	p_wk->plno		= plno;
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG優先順位	スクロールシステム	メイン
 *
 *	@param	p_wk	ワーク
 *	@param	p_gra	グラフィック
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_BGPRISCRL_Main( BCT_CLIENT_BGPRI_SCRL* p_wk, BCT_CLIENT_GRAPHIC* p_gra )
{
	switch( p_wk->seq ){
	// 待機状態
	case BCT_BGPRI_SCRL_SEQ_NORMAL:
		break;

	// エフェクト開始
	case BCT_BGPRI_SCRL_SEQ_START:
		// エフェクトをかけて、most_backの状態にするので
		// most_backの床のもうひとつ前の状態でBG優先順位をつける
		BCT_CLIENT_BGPRISCRL_SetPri( p_gra, p_wk->most_back-1, p_wk->plno );

		// 
		p_wk->seq	++;
		p_wk->count	= 0;
		
	// エフェクトメイン
	case BCT_BGPRI_SCRL_SEQ_TOPFADE:
		if( p_wk->count >= BCT_BGPRI_SCRL_COUNT_MAX ){
			// 下に差し込む処理へ
			p_wk->seq	= BCT_BGPRI_SCRL_SEQ_BOTTOMFADE;
			p_wk->count = 0;

			// 優先順位を設定	本当はVブランクで行ったほうがきれいですよ
			BCT_CLIENT_BGPRISCRL_SetPri( p_gra, p_wk->most_back, p_wk->plno );
			G2S_SetBlendAlpha( 1<<p_wk->most_back,
					GX_BLEND_PLANEMASK_BG3,
					0, 16 );
		}else{
			p_wk->count ++;

			// アルファ値設定
			{
				s32 top, two, three;
				s32 alpha;

				top		= p_wk->most_back;
				two		= p_wk->most_back-2;
				three	= p_wk->most_back-1;
				if( top < 0 ){
					top += BCT_BGPRI_SCRL_MOSTBACK_MAX;
				}
				if( two < 0 ){
					two += BCT_BGPRI_SCRL_MOSTBACK_MAX;
				}
				if( three < 0 ){
					three += BCT_BGPRI_SCRL_MOSTBACK_MAX;
				}
				alpha	= 16 - ((p_wk->count * 16) / BCT_BGPRI_SCRL_COUNT_MAX);
				G2S_SetBlendAlpha( 1<<top,
						(1<<two) | (1<<three) | GX_BLEND_PLANEMASK_BG3,
						alpha, 16-alpha );
			}
		}
		break;

		
	// 下に差し込む処理
	case BCT_BGPRI_SCRL_SEQ_BOTTOMFADE:
		if( p_wk->count >= BCT_BGPRI_SCRL_BOTTOMCOUNT_MAX ){
			// 終了
			p_wk->seq	= BCT_BGPRI_SCRL_SEQ_NORMAL;

			G2S_BlendNone();

		}else{
			p_wk->count ++;

			// アルファ値設定
			{
				s32 alpha;
				alpha	= (p_wk->count * 16) / BCT_BGPRI_SCRL_BOTTOMCOUNT_MAX;
				G2S_SetBlendAlpha( 1<<p_wk->most_back,
						GX_BLEND_PLANEMASK_BG3,
						alpha, 16-alpha );
			}
		}
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG優先順位	スクロールシステム	リクエスト
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_BGPRISCRL_Req( BCT_CLIENT_BGPRI_SCRL* p_wk )
{
	p_wk->req_num ++;
	if( p_wk->req_num >= BCT_BGPRI_SCRL_REQ_START_NUM ){
		p_wk->seq		= BCT_BGPRI_SCRL_SEQ_START;
		p_wk->most_back = (p_wk->most_back + 1) % BCT_BGPRI_SCRL_MOSTBACK_MAX;
		p_wk->req_num	= 0;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	一番下のBG面を指定して、優先順位を設定する
 *
 *	@param	p_gra		グラフィックシステム
 *	@param	most_back	一番下のBG面
 *	@param	plno		プレイヤー優先順位
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_BGPRISCRL_SetPri( BCT_CLIENT_GRAPHIC* p_gra, s16 most_back, u16 plno )
{
	int i;
	s32 bgno;

	// マイナスのままmost_backがわたってくる可能性もあるでも大丈夫
	
	for( i=0; i<BCT_BGPRI_SCRL_MOSTBACK_MAX; i++ ){
		bgno = most_back - i;

		// BGフレーム計算
		if( bgno < 0 ){
			bgno += BCT_BGPRI_SCRL_MOSTBACK_MAX;
		}
		bgno += GF_BGL_FRAME0_S;

		
		// 優先順位設定
		GF_BGL_PrioritySet( bgno, (BCT_BGPRI_SCRL_PRI_MAX - i) );

		// パレット設定
		if( i==0 ){
			// 一番下
			GF_BGL_ScrPalChange( p_gra->p_bgl, bgno, 0, 0, 32, 32, BCT_GRA_BGSUB_PAL_NETID0_BACK+(plno*2) );
		}else{
			// それ以外
			GF_BGL_ScrPalChange( p_gra->p_bgl, bgno, 0, 0, 32, 32, BCT_GRA_BGSUB_PAL_NETID0_TOP+(plno*2) );
		}

		GF_BGL_LoadScreenReq( p_gra->p_bgl, bgno );
	}
}




//----------------------------------------------------------------------------
/**
 *	@brief	木の実カウンター	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_gra		グラフィックワーク
 *	@param	p_handle	ハンドル
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NUTS_COUNT_Init( BCT_CLIENT_NUTS_COUNT* p_wk, BCT_CLIENT_GRAPHIC* p_gra, ARCHANDLE* p_handle, u32 heapID )
{
	// ヒープID保存
	p_wk->heapID	= heapID;

	
	// テーブルとなるOAMの読み込み
	{
		BOOL result;

        // OAMリソース読込み
        p_wk->resobj[ 0 ] = CLACT_U_ResManagerResAddArcChar_ArcHandle(
                    p_gra->resMan[ 0 ], p_handle,
                    NARC_bucket_counter_NCGR,
                    FALSE, BCT_NUTS_COUNT_DRAW_RESID, NNS_G2D_VRAM_TYPE_2DMAIN, heapID );

        p_wk->resobj[ 1 ] = CLACT_U_ResManagerResAddArcPltt_ArcHandle(
                    p_gra->resMan[ 1 ], p_handle,
                    NARC_bucket_counter_NCLR,
                    FALSE, BCT_NUTS_COUNT_DRAW_RESID, NNS_G2D_VRAM_TYPE_2DMAIN, 1, heapID );

        p_wk->resobj[ 2 ] = CLACT_U_ResManagerResAddArcKindCell_ArcHandle(
                p_gra->resMan[ 2 ], p_handle,
                NARC_bucket_counter_NCER, FALSE,
                BCT_NUTS_COUNT_DRAW_RESID, CLACT_U_CELL_RES, heapID );

        p_wk->resobj[ 3 ] = CLACT_U_ResManagerResAddArcKindCell_ArcHandle(
                p_gra->resMan[ 3 ], p_handle,
                NARC_bucket_counter_NANR, FALSE,
                BCT_NUTS_COUNT_DRAW_RESID, CLACT_U_CELLANM_RES, heapID );

        // 転送
        result = CLACT_U_CharManagerSetCharModeAdjustAreaCont( p_wk->resobj[ 0 ] );
        GF_ASSERT( result );
        result = CLACT_U_PlttManagerSetCleanArea( p_wk->resobj[ 1 ] );
        GF_ASSERT( result );

        // リソースだけ破棄
        CLACT_U_ResManagerResOnlyDelete( p_wk->resobj[ 0 ] );
        CLACT_U_ResManagerResOnlyDelete( p_wk->resobj[ 1 ] );

		// セルアクターヘッダー作成
		CLACT_U_MakeHeader( &p_wk->header, 
				BCT_NUTS_COUNT_DRAW_RESID, BCT_NUTS_COUNT_DRAW_RESID,
				BCT_NUTS_COUNT_DRAW_RESID, BCT_NUTS_COUNT_DRAW_RESID,
				CLACT_U_HEADER_DATA_NONE, CLACT_U_HEADER_DATA_NONE,0,
				BCT_NUTS_COUNT_DRAW_BGPRI,
				p_gra->resMan[0], p_gra->resMan[1], 
				p_gra->resMan[2], p_gra->resMan[3],
				NULL, NULL );
	}

	// テーブル部分のOAM作成
	{
		CLACT_ADD_SIMPLE add;
		
		add.ClActSet		= p_gra->clactSet;
		add.ClActHeader		= &p_wk->header;
		add.mat.x			= BCT_NUTS_COUNT_INOUT_SX;
		add.mat.y			= BCT_NUTS_COUNT_YURE_SY;
		add.pri				= BCT_NUTS_COUNT_DRAW_SFPRI;
		add.DrawArea		= NNS_G2D_VRAM_TYPE_2DMAIN;
		add.heap			= heapID;

		p_wk->p_tblwk = CLACT_AddSimple( &add );
		CLACT_SetDrawFlag( p_wk->p_tblwk, FALSE );
	}
	
	// フォントOAMデータ作成
	{
		int char_size;
		BOOL result;
		FONTOAM_INIT fontoam_init;

		// 文字列バッファ作成
		p_wk->p_str = STRBUF_Create( 16, heapID );
		
		// ビットマップ
		GF_BGL_BmpWinObjAdd( p_gra->p_bgl, 
				&p_wk->objbmp, 
				BCT_NUTS_COUNT_BMP_SIZX, BCT_NUTS_COUNT_BMP_SIZY, 
				0, 0 );

		// OAM構成データを作成
		p_wk->p_fontoam_data	= FONTOAM_OAMDATA_Make( &p_wk->objbmp, heapID );
		char_size				= FONTOAM_OAMDATA_NeedCharSize( p_wk->p_fontoam_data, NNS_G2D_VRAM_TYPE_2DMAIN );

		// キャラクタ領域を確保
		result = CharVramAreaAlloc( char_size, CHARM_CONT_AREACONT,
				NNS_G2D_VRAM_TYPE_2DMAIN, &p_wk->fontoam_chardata );
		GF_ASSERT( result == TRUE );

		// フォント用パレット
		p_wk->p_fontoam_pltt = CLACT_U_ResManagerResAddArcPltt(
                    p_gra->resMan[1], ARC_FONT,
                    NARC_font_system_ncrl,
                    FALSE, BCT_NUTS_COUNT_FONTOAM_PLTT_ID, NNS_G2D_VRAM_TYPE_2DMAIN, 1, heapID ); 

        result = CLACT_U_PlttManagerSetCleanArea( p_wk->p_fontoam_pltt );
        GF_ASSERT( result );

        // リソースだけ破棄
        CLACT_U_ResManagerResOnlyDelete( p_wk->p_fontoam_pltt );


		// FONTOAMワークを作成
		fontoam_init.fontoam_sys	= p_gra->p_fontoam_sys;
		fontoam_init.bmp			= &p_wk->objbmp;
		fontoam_init.clact_set		= p_gra->clactSet;
		fontoam_init.pltt			= CLACT_U_PlttManagerGetProxy( p_wk->p_fontoam_pltt, NULL );
		fontoam_init.parent			= p_wk->p_tblwk;
		fontoam_init.char_ofs		= p_wk->fontoam_chardata.alloc_ofs;
		fontoam_init.x				= BCT_NUTS_COUNT_FONTOAM_X;
		fontoam_init.y				= BCT_NUTS_COUNT_FONTOAM_Y;
		fontoam_init.bg_pri			= BCT_NUTS_COUNT_FONTOAM_BG_PRI;
		fontoam_init.soft_pri		= BCT_NUTS_COUNT_FONTOAM_SF_PRI;
		fontoam_init.draw_area		= NNS_G2D_VRAM_TYPE_2DMAIN;
		fontoam_init.heap			= heapID;
		
		p_wk->p_fontoam				= FONTOAM_OAMDATA_Init( 
				&fontoam_init, p_wk->p_fontoam_data );

		// 表示OFF
		FONTOAM_SetDrawFlag( p_wk->p_fontoam, FALSE );

		// ビットマップ
		GF_BGL_BmpWinDel( &p_wk->objbmp );

	}

	// 動作パラメータ初期化
	BCT_AddMoveReqFx( &p_wk->inout_data, 
			BCT_NUTS_COUNT_INOUT_SX, BCT_NUTS_COUNT_INOUT_EX,
			BCT_NUTS_COUNT_INOUT_SS, BCT_NUTS_COUNT_INOUT_COUNTMAX );
	BCT_AddMoveReqFx( &p_wk->yure_data, 
			BCT_NUTS_COUNT_YURE_SY, BCT_NUTS_COUNT_YURE_EY,
			BCT_NUTS_COUNT_YURE_SS, BCT_NUTS_COUNT_YURE_COUNTMAX );

	// カウンタ初期化
	p_wk->inout_count	= 0;
	p_wk->yure_count	= BCT_NUTS_COUNT_YURE_COUNTMAX;
}

//----------------------------------------------------------------------------
/**
 *	@brief	木の実カウンター	破棄
 *
 *	@param	p_wk		ワーク
 *	@param	p_gra		グラフィックワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NUTS_COUNT_Exit( BCT_CLIENT_NUTS_COUNT* p_wk, BCT_CLIENT_GRAPHIC* p_gra )
{
	// フォントOAMデータ作成
	{
		
		// FONTOAMワークを破棄
		FONTOAM_OAMDATA_Delete( p_wk->p_fontoam ); 

		// パレット破棄
        CLACT_U_PlttManagerDelete( p_wk->p_fontoam_pltt );
        CLACT_U_ResManagerResDelete( p_gra->resMan[1], p_wk->p_fontoam_pltt );


		// キャラクタ領域を破棄
		CharVramAreaFree( &p_wk->fontoam_chardata );

		// OAM構成データを作成
		FONTOAM_OAMDATA_Free( p_wk->p_fontoam_data );
		
		// 文字列バッファ破棄
		STRBUF_Delete( p_wk->p_str );
	}

	// テーブル破棄
	CLACT_Delete( p_wk->p_tblwk );

	// テーブルとなるOAMのリソース破棄
	{
        // VRAM管理から破棄
        CLACT_U_CharManagerDelete( p_wk->resobj[0] );
        CLACT_U_PlttManagerDelete( p_wk->resobj[1] );
        
        // リソース破棄
        CLACT_U_ResManagerResDelete( p_gra->resMan[0], p_wk->resobj[0] );
        CLACT_U_ResManagerResDelete( p_gra->resMan[1], p_wk->resobj[1] );
        CLACT_U_ResManagerResDelete( p_gra->resMan[2], p_wk->resobj[2] );
        CLACT_U_ResManagerResDelete( p_gra->resMan[3], p_wk->resobj[3] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	木の実カウンター	開始
 *		
 *	@param	p_wk		ワーク
 *	@param	count		開始時のカウント値
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NUTS_COUNT_Start( BCT_CLIENT_NUTS_COUNT* p_wk, BCT_CLIENT_GRAPHIC* p_gra, u32 count )
{
	// 数字を書き込んで転送
	{
		STRBUF_SetNumber( p_wk->p_str, count, 2, 
				NUMBER_DISPTYPE_ZERO, NUMBER_CODETYPE_DEFAULT );

		// ビットマップ
		GF_BGL_BmpWinObjAdd( p_gra->p_bgl, 
				&p_wk->objbmp, 
				BCT_NUTS_COUNT_BMP_SIZX, BCT_NUTS_COUNT_BMP_SIZY, 
				0, 0 );

		// 書き込む
		GF_STR_PrintColor( &p_wk->objbmp, FONT_SYSTEM, p_wk->p_str,
				0, 0, MSG_NO_PUT, BCT_COL_OAM_BLACK, NULL );

		// 転送
		FONTOAM_OAMDATA_ResetBmp( p_wk->p_fontoam, p_wk->p_fontoam_data, &p_wk->objbmp, p_wk->heapID );


		// ビットマップ
		GF_BGL_BmpWinDel( &p_wk->objbmp );
	}

	// 描画開始
	FONTOAM_SetDrawFlag( p_wk->p_fontoam, TRUE );
	CLACT_SetDrawFlag( p_wk->p_tblwk, TRUE );

	// 
	p_wk->seq = BCT_NUTS_COUNT_SEQ_IN;	

	p_wk->out_wait	= 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	木の実カウンター	終了
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NUTS_COUNT_End( BCT_CLIENT_NUTS_COUNT* p_wk )
{
	// 戻らせる
	if( p_wk->seq != BCT_NUTS_COUNT_SEQ_OUT ){

		// メイン動作中のときのみ、ちょっと表示してから
		// 終了させる
		if( p_wk->seq == BCT_NUTS_COUNT_SEQ_MAIN ){
			p_wk->out_wait	= BCT_NUTS_COUNT_OUT_WAIT;
		}
		
		p_wk->seq		= BCT_NUTS_COUNT_SEQ_OUT;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	木の実カウンター	数字更新
 *	
 *	@param	p_wk		ワーク
 *	@param	count		カウント値
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NUTS_COUNT_SetData( BCT_CLIENT_NUTS_COUNT* p_wk, BCT_CLIENT_GRAPHIC* p_gra, u32 count )
{
	// 退室中でなければ設定
	if( p_wk->seq != BCT_NUTS_COUNT_SEQ_OUT ){
		
		// 数字を更新
		{
			STRBUF_SetNumber( p_wk->p_str, count, 2, 
					NUMBER_DISPTYPE_ZERO, NUMBER_CODETYPE_DEFAULT );

			// ビットマップ
			GF_BGL_BmpWinObjAdd( p_gra->p_bgl, 
					&p_wk->objbmp, 
					BCT_NUTS_COUNT_BMP_SIZX, BCT_NUTS_COUNT_BMP_SIZY, 
					0, 0 );

			// 書き込む
			GF_STR_PrintColor( &p_wk->objbmp, FONT_SYSTEM, p_wk->p_str,
					0, 0, MSG_NO_PUT, BCT_COL_OAM_BLACK, NULL );

			// 転送
			FONTOAM_OAMDATA_ResetBmp( p_wk->p_fontoam, p_wk->p_fontoam_data, &p_wk->objbmp, p_wk->heapID );

			// ビットマップ
			GF_BGL_BmpWinDel( &p_wk->objbmp );
		}

		// ゆれアニメ開始
		p_wk->yure_count = 0;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	木の実カウンター	メイン
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_NUTS_COUNT_Main( BCT_CLIENT_NUTS_COUNT* p_wk )
{
	switch( p_wk->seq ){
	// 待機状態
	case BCT_NUTS_COUNT_SEQ_WAIT:
		break;
	// 入ってくる動作
	case BCT_NUTS_COUNT_SEQ_IN:
		if( p_wk->inout_count < BCT_NUTS_COUNT_INOUT_COUNTMAX ){
			p_wk->inout_count ++;
		}else{
			p_wk->seq = BCT_NUTS_COUNT_SEQ_MAIN;
		}
		break;
	// メイン
	case BCT_NUTS_COUNT_SEQ_MAIN:
		break;
	// 退室動作
	case BCT_NUTS_COUNT_SEQ_OUT:
		if( p_wk->out_wait > 0 ){
			p_wk->out_wait --;
			break;
		}

		if( p_wk->inout_count > 0 ){
			p_wk->inout_count --;
		}else{
			p_wk->seq = BCT_NUTS_COUNT_SEQ_WAIT;
			// 表示OFF
			FONTOAM_SetDrawFlag( p_wk->p_fontoam, FALSE );
			CLACT_SetDrawFlag( p_wk->p_tblwk, FALSE );
		}
		break;
	}

	// 常に動くもの
	
	// たてゆれ動作
	if( p_wk->yure_count < BCT_NUTS_COUNT_YURE_COUNTMAX ){	
		p_wk->yure_count ++;
		p_wk->yure_data.count = p_wk->yure_count;
		// 計算
		BCT_AddMoveMainFx( &p_wk->yure_data );
	}

	// 横揺れ
	{
		p_wk->inout_data.count = p_wk->inout_count;
		// 計算
		BCT_AddMoveMainFx( &p_wk->inout_data );
	}


	// 座標設定
	{
		VecFx32 matrix;
		matrix.x = p_wk->inout_data.x;
		matrix.y = p_wk->yure_data.x;
		
		CLACT_SetMatrix( p_wk->p_tblwk, &matrix );
		FONTOAM_ReflectParentMat( p_wk->p_fontoam );
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief	フラッシュエフェクト開始
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_FEVER_EFF_Start( BCT_CLIENT_FEVER_EFF_WK* p_wk )
{
	p_wk->seq	= 0;
	p_wk->move	= TRUE;
	p_wk->wait	= 0; 
	Snd_SePlay( BCT_SND_FEVER_CHIME );// FEVERチャイム

}

//----------------------------------------------------------------------------
/**
 *	@brief	フラッシュエフェクトメイン
 *
 *	@param	p_wk			ワーク
 *	@param	p_mainback		背景グラフィック
 *	@param	p_marunomudraw	マルノーム描画ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_FEVER_EFF_Main( BCT_CLIENT_FEVER_EFF_WK* p_wk, BCT_CLIENT_MAINBACK* p_mainback, BCT_MARUNOMU_DRAW* p_marunomudraw )
{
	fx32 speed;
	u32 tempo;
	
	// 動作中じゃなかったらなんもしない
	if( p_wk->move == FALSE ){
		return ;
	}
	
	switch( p_wk->seq ){
	// エフェクト開始ウエイト
	case BCT_FEVER_EFF_SEQ_BGM_WAIT:
		p_wk->wait ++;

		// 背景アニメスピードを変更
		speed = (p_wk->wait*BCT_FEVER_BACK_ANM_SPEED_DIF) / BCT_FEVER_EFF_FLASHOUT_FLASHOUT_WAIT;
		speed += BCT_FEVER_BACK_ANM_SPEED_START;
		BCT_CLIENT_MainBackSetAnmSpeed( p_mainback, speed );

		// BGMのテンポアップ
		tempo = (p_wk->wait*BCT_FEVER_EFF_BGM_TEMPO_DIF) / BCT_FEVER_EFF_FLASHOUT_FLASHOUT_WAIT;
		tempo += BCT_FEVER_EFF_BGM_TEMPO_START;
		Snd_PlayerSetTempoRatio( SND_HANDLE_BGM, tempo );

		// マルノームが動く音
		if( BCT_FEVER_EFF_SE_MARUNOMUMOVE == p_wk->wait ){
			Snd_SePlay( BCT_SND_FEVER_MOVE );
		}

		
		if( p_wk->wait >= BCT_FEVER_EFF_FLASHOUT_FLASHOUT_WAIT ){
			p_wk->seq ++;
		}
		break;
	// フラッシュアウト
	case BCT_FEVER_EFF_SEQ_BGM_FLASH_OUT:
		ChangeBrightnessRequest( BCT_FEVER_EFF_FLASHOUT_FLASHOUT_SYNC,
				BRIGHTNESS_WHITE, BRIGHTNESS_NORMAL, 
				PLANEMASK_ALL, MASK_MAIN_DISPLAY );
		p_wk->seq ++;
		break;
	case BCT_FEVER_EFF_SEQ_BGM_FLASH_OUTWAIT:
		if( IsFinishedBrightnessChg( MASK_MAIN_DISPLAY ) == TRUE ){

			// 背景変更
			BCT_CLIENT_MainBackSetDrawFever( p_mainback );
			BCT_CLIENT_MainBackSetAnmSpeed( p_mainback, BCT_MARUNOMU_ANM_SPEED ); 
			// FEVERになったらランダム表示
			BCT_CLIENT_MarunomuDrawSetColAnmRand( p_marunomudraw, TRUE );
			p_wk->seq ++;
		}
		break;

	// フラッシュイン
	case BCT_FEVER_EFF_SEQ_BGM_FLASH_IN:
		ChangeBrightnessRequest( BCT_FEVER_EFF_FLASHOUT_FLASHIN_SYNC,
				BRIGHTNESS_NORMAL, BRIGHTNESS_WHITE, 
				PLANEMASK_ALL, MASK_MAIN_DISPLAY );
		p_wk->seq ++;
		break;
	case BCT_FEVER_EFF_SEQ_BGM_FLASH_INWAIT:
		if( IsFinishedBrightnessChg( MASK_MAIN_DISPLAY ) == TRUE ){
			p_wk->seq ++;
		}
		break;

	// 終了
	case BCT_FEVER_EFF_SEQ_BGM_FLASH_END:
		p_wk->move	= FALSE;
		p_wk->seq	= 0;
		break;

	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	FEVERエフェクト	のかかっている状態をリセット	（BGMのテンポ）
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void BCT_CLIENT_FEVER_EFF_Reset( BCT_CLIENT_FEVER_EFF_WK* p_wk )
{
	Snd_PlayerSetTempoRatio( SND_HANDLE_BGM, 256 );
}





#ifdef BCT_DEBUG_HITZONE

static void BCT_DEBUG_PositionInit( ARCHANDLE* p_handle, u32 heapID )
{
    int i;
    
    D3DOBJ_MdlLoadH( &BCT_DPD.mdl, p_handle, NARC_bucket_kinomi_1p_nsbmd, heapID );

    for( i=0; i<4; i++ ){
        D3DOBJ_Init( &BCT_DPD.obj[i], &BCT_DPD.mdl );
    }
}

#define BCT_DEBUG_HITZONE_MOUTH		// 口のエリア表示
#define BCT_DEBUG_HITZONE_BODY		// ボディエリア表示
//#define BCT_DEBUG_HITZONE_DITCH		// 溝エリア表示

static void BCT_DEBUG_PositionDraw( const BCT_CLIENT* cp_client )
{
    int i, j;
    MtxFx43 mtx;
    MtxFx43 tmp;
    MtxFx33 workmtx;
    VecFx32 matrix = { 0,0,0 };
	VecFx32 setmat;
	VecFx32 testmat;
	fx32 r;
	s32 y_dis;

    MTX_Identity33( &workmtx );


#ifdef BCT_DEBUG_HITZONE_MOUTH
    for( i=0; i<4; i++ ){
        NNS_G3dGePushMtx();

        // 位置設定
        NNS_G3dGlbSetBaseTrans(&matrix);
        // 角度設定
        NNS_G3dGlbSetBaseRot(&workmtx);
        // スケール設定
        NNS_G3dGlbSetBaseScale(&BCT_DPD.obj[i].scale);

        NNS_G3dGlbFlush();

        MTX_Identity43( &mtx );

		// 4頂点を作成する
		switch( i ){
		case 0:
			setmat.x = -cp_client->marunomu.mouth_size;
			setmat.z = -FX_Mul( cp_client->marunomu.mouth_size, FX32_CONST( 2 ) );
			setmat.y = 0;
			break;
		case 1:
			setmat.x = -cp_client->marunomu.mouth_size;
			setmat.z = 0;
			setmat.y = 0;
			break;
		case 2:
			setmat.x = cp_client->marunomu.mouth_size;
			setmat.z = -FX_Mul( cp_client->marunomu.mouth_size, FX32_CONST( 2 ) );
			setmat.y = 0;
			break;
		case 3:
			setmat.x = cp_client->marunomu.mouth_size;
			setmat.z = 0;
			setmat.y = 0;
			break;
		}
		

		// 足元で回転させる
		MTX_MultVec33( &setmat, &cp_client->marunomu.rotaxy_mtx, &setmat );

		// 表示位置に移動する
		testmat.x = BCT_FIELD_MARUNOMU_X;
		testmat.y = cp_client->marunomu.hit_y;
		testmat.z = cp_client->marunomu.marunomu_mat.z + BCT_MARUNOMU_HIT_DIS_MAX;
		MTX_MultVec33( &testmat, &cp_client->marunomu.rotay_mtx, &testmat );

		setmat.x += testmat.x;
		setmat.y += testmat.y;
		setmat.z += testmat.z;
		

        // 移動
        NNS_G3dGeTranslateVec( &setmat );
        NNS_G3dDraw(&BCT_DPD.obj[i].render);
    
        NNS_G3dGePopMtx(1);
    }

	// センター座標を表示
	{
        NNS_G3dGePushMtx();

        // 位置設定
        NNS_G3dGlbSetBaseTrans(&matrix);
        // 角度設定
        NNS_G3dGlbSetBaseRot(&workmtx);
        // スケール設定
        NNS_G3dGlbSetBaseScale(&BCT_DPD.obj[0].scale);

        NNS_G3dGlbFlush();

        MTX_Identity43( &mtx );

        // 移動
        NNS_G3dGeTranslateVec( &cp_client->marunomu.center_mat );
        NNS_G3dDraw(&BCT_DPD.obj[0].render);
    
        NNS_G3dGePopMtx(1);
	}
#endif

#ifdef BCT_DEBUG_HITZONE_BODY
	// ボディあたり判定も表示
	for( i=0; i<2; i++ ){

		if( i==0 ){
			setmat.y = cp_client->marunomu.matrix.y;
		}else{
			setmat.y = cp_client->marunomu.matrix.y + cp_client->marunomu.height;
		}

		for( j=0; j<4; j++ ){

			NNS_G3dGePushMtx();

			// 位置設定
			NNS_G3dGlbSetBaseTrans(&matrix);
			// 角度設定
			NNS_G3dGlbSetBaseRot(&workmtx);
			// スケール設定
			NNS_G3dGlbSetBaseScale(&BCT_DPD.obj[i].scale);

			NNS_G3dGlbFlush();

			MTX_Identity43( &mtx );

			// 当たり判定半径を求める
			y_dis = setmat.y - cp_client->marunomu.marunomu_mat.y;
			y_dis = y_dis>>FX32_SHIFT;
			r = ( y_dis*cp_client->marunomu.hitbody_r_dis ) / (cp_client->marunomu.height >> FX32_SHIFT);
			r = cp_client->marunomu.hitbody_r_max - r;

			// 4頂点を作成する
			switch( j ){
			case 0:
				setmat.x = cp_client->marunomu.marunomu_mat.x - r;
				setmat.z = cp_client->marunomu.marunomu_mat.z - r;
				break;
			case 1:
				setmat.x = cp_client->marunomu.marunomu_mat.x - r;
				setmat.z = cp_client->marunomu.marunomu_mat.z + r;
				break;
			case 2:
				setmat.x = cp_client->marunomu.marunomu_mat.x + r;
				setmat.z = cp_client->marunomu.marunomu_mat.z - r;
				break;
			case 3:
				setmat.x = cp_client->marunomu.marunomu_mat.x + r;
				setmat.z = cp_client->marunomu.marunomu_mat.z + r;
				break;
			}
			
			// 表示位置に移動する
			MTX_MultVec33( &setmat, &cp_client->marunomu.rotay_mtx, &setmat );

			// 移動
			NNS_G3dGeTranslateVec( &setmat );
			NNS_G3dDraw(&BCT_DPD.obj[i].render);
		
			NNS_G3dGePopMtx(1);

		}
	}
#endif

#ifdef BCT_DEBUG_HITZONE_DITCH
	// 溝の位置も表示
    for( i=0; i<8; i++ ){
        NNS_G3dGePushMtx();

        // 位置設定
        NNS_G3dGlbSetBaseTrans(&matrix);
        // 角度設定
        NNS_G3dGlbSetBaseRot(&workmtx);
        // スケール設定
        NNS_G3dGlbSetBaseScale(&BCT_DPD.obj[i%4].scale);

        NNS_G3dGlbFlush();

        MTX_Identity43( &mtx );

		// 4頂点を作成する
		switch( i ){
		case 0:
			setmat.x = BCT_CAMERA_TARGET_X - BCT_NUTS_AWAY_DITCH_MIN_R;
			setmat.z = BCT_CAMERA_TARGET_Z - BCT_NUTS_AWAY_DITCH_MIN_R;
			setmat.y = BCT_FIELD_MIN;
			break;
		case 1:
			setmat.x = BCT_CAMERA_TARGET_X - BCT_NUTS_AWAY_DITCH_MIN_R;
			setmat.z = BCT_CAMERA_TARGET_Z + BCT_NUTS_AWAY_DITCH_MIN_R;
			setmat.y = BCT_FIELD_MIN;
			break;
		case 2:
			setmat.x = BCT_CAMERA_TARGET_X + BCT_NUTS_AWAY_DITCH_MIN_R;
			setmat.z = BCT_CAMERA_TARGET_Z + BCT_NUTS_AWAY_DITCH_MIN_R;
			setmat.y = BCT_FIELD_MIN;
			break;
		case 3:
			setmat.x = BCT_CAMERA_TARGET_X + BCT_NUTS_AWAY_DITCH_MIN_R;
			setmat.z = BCT_CAMERA_TARGET_Z - BCT_NUTS_AWAY_DITCH_MIN_R;
			setmat.y = BCT_FIELD_MIN;
			break;

		case 4:
			setmat.x = BCT_CAMERA_TARGET_X - BCT_NUTS_AWAY_DITCH_MAX_R;
			setmat.z = BCT_CAMERA_TARGET_Z - BCT_NUTS_AWAY_DITCH_MAX_R;
			setmat.y = BCT_FIELD_MIN;
			break;
		case 5:
			setmat.x = BCT_CAMERA_TARGET_X - BCT_NUTS_AWAY_DITCH_MAX_R;
			setmat.z = BCT_CAMERA_TARGET_Z + BCT_NUTS_AWAY_DITCH_MAX_R;
			setmat.y = BCT_FIELD_MIN;
			break;
		case 6:
			setmat.x = BCT_CAMERA_TARGET_X + BCT_NUTS_AWAY_DITCH_MAX_R;
			setmat.z = BCT_CAMERA_TARGET_Z + BCT_NUTS_AWAY_DITCH_MAX_R;
			setmat.y = BCT_FIELD_MIN;
			break;
		case 7:
			setmat.x = BCT_CAMERA_TARGET_X + BCT_NUTS_AWAY_DITCH_MAX_R;
			setmat.z = BCT_CAMERA_TARGET_Z - BCT_NUTS_AWAY_DITCH_MAX_R;
			setmat.y = BCT_FIELD_MIN;
			break;
		}
		
        // 移動
        NNS_G3dGeTranslateVec( &setmat );
        NNS_G3dDraw(&BCT_DPD.obj[i%4].render);
    
        NNS_G3dGePopMtx(1);
    }
#endif
}

static void BCT_DEBUG_PositionExit( void )
{
    D3DOBJ_MdlDelete( &BCT_DPD.mdl );
}

#endif

#ifdef BCT_DEBUG_AUTOSLOW

//----------------------------------------------------------------------------
/**
 *	@brief	自動投げ
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
#define BCT_NUTS_SLOW_RENSYA_TIME_AUTOSLOW	( 18 )	// 最小次発射待ち時間
static void BCT_DEBUG_AutoSlow( BCT_CLIENT* p_wk )
{
	// 玉発射間隔カウント
	p_wk->slow.time += 1+(gf_mtRand()%4);

	// 発射間隔
	if( p_wk->slow.time < BCT_NUTS_SLOW_RENSYA_TIME_AUTOSLOW ){
		return ;
	}

	// 木の実発射
	BCT_EasyNutsSet( p_wk, 128, 96, 128, 66, BCT_NUTSSEQ_MOVE );

	// 次の球発射間隔タイマーリセット
	p_wk->slow.time = 0;
}

#endif

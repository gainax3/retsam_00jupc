//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_gadget.c
 *	@brief		ガジェットアニメーションシステム
 *	@author		tomoya takahashi
 *	@data		2008.01.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"

#include "system/arc_util.h"
#include "system/d3dobj.h"

#include "graphic/wifi_lobby_other.naix"

#include "wflby_gadget.h"
#include "wflby_3dmapobj.h"
#include "wflby_snd.h"
#include "wflby_3dmatrix.h"

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
#ifdef PM_DEBUG
//#define WFLBY_DEBUG_GADGET_ALL_PUT	// Rを押すと全員がタッチトイを鳴らす
#endif

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	ガジェットオブジェデータ
//=====================================
enum {
	WFLBY_GADGET_OBJ_RIPPLE00,	// リップル赤
	WFLBY_GADGET_OBJ_RIPPLE01,	// リップル黄
	WFLBY_GADGET_OBJ_RIPPLE02,	// リップル青
	WFLBY_GADGET_OBJ_SIGNAL00,	// シグナル
	WFLBY_GADGET_OBJ_SIGNAL01,	// シグナル
	WFLBY_GADGET_OBJ_SIGNAL02,	// シグナル
	WFLBY_GADGET_OBJ_SWING00,	// スウィング
	WFLBY_GADGET_OBJ_SWING01,	// スウィング
	WFLBY_GADGET_OBJ_SWING02,	// スウィング
	WFLBY_GADGET_OBJ_CRACLCER00,// クラッカー
	WFLBY_GADGET_OBJ_CRACLCER01,// クラッカー
	WFLBY_GADGET_OBJ_CRACLCER02,// クラッカー
	WFLBY_GADGET_OBJ_FLASH00,	// フラッシュ
	WFLBY_GADGET_OBJ_SPARKLE00,	// スパークル赤
	WFLBY_GADGET_OBJ_SPARKLE01,	// スパークル黄
	WFLBY_GADGET_OBJ_SPARKLE02,	// スパークル青
	WFLBY_GADGET_OBJ_BALLOON00,	// 風船赤
	WFLBY_GADGET_OBJ_BALLOON01,	// 風船黄
	WFLBY_GADGET_OBJ_BALLOON02,	// 風船青
	WFLBY_GADGET_OBJ_ONPU00_R,	// 音符A
	WFLBY_GADGET_OBJ_ONPU01_R,	// 音符B
	WFLBY_GADGET_OBJ_ONPU02_R,	// 音符C
	WFLBY_GADGET_OBJ_ONPU03_R,	// 音符D
	WFLBY_GADGET_OBJ_ONPU04_R,	// 音符E
	WFLBY_GADGET_OBJ_ONPU05_R,	// 音符F
	WFLBY_GADGET_OBJ_ONPU00_Y,	// 音符A
	WFLBY_GADGET_OBJ_ONPU01_Y,	// 音符B
	WFLBY_GADGET_OBJ_ONPU02_Y,	// 音符C
	WFLBY_GADGET_OBJ_ONPU03_Y,	// 音符D
	WFLBY_GADGET_OBJ_ONPU04_Y,	// 音符E
	WFLBY_GADGET_OBJ_ONPU05_Y,	// 音符F
	WFLBY_GADGET_OBJ_ONPU00_B,	// 音符A
	WFLBY_GADGET_OBJ_ONPU01_B,	// 音符B
	WFLBY_GADGET_OBJ_ONPU02_B,	// 音符C
	WFLBY_GADGET_OBJ_ONPU03_B,	// 音符D
	WFLBY_GADGET_OBJ_ONPU04_B,	// 音符E
	WFLBY_GADGET_OBJ_ONPU05_B,	// 音符F
	WFLBY_GADGET_OBJ_SPARK00,	// いなずま左
	WFLBY_GADGET_OBJ_SPARK01,	// いなずま右
	WFLBY_GADGET_OBJ_NUM,
} ;


//-------------------------------------
///	ガジェットモデル数
//=====================================
enum {
	WFLBY_GADGET_MDL_RIPPLER64x64,
	WFLBY_GADGET_MDL_SWING64x64,
	WFLBY_GADGET_MDL_16x16,
	WFLBY_GADGET_MDL_CRACKER32x32,
	WFLBY_GADGET_MDL_CRACKER0132x32,
	WFLBY_GADGET_MDL_CRACKER0232x32,
	WFLBY_GADGET_MDL_FLASH32x32,
	WFLBY_GADGET_MDL_SPARKLE_R32x32,
	WFLBY_GADGET_MDL_SPARKLE_Y32x32,
	WFLBY_GADGET_MDL_SPARKLE_B32x32,
	WFLBY_GADGET_MDL_BALLOON_R32x32,
	WFLBY_GADGET_MDL_BALLOON_Y32x32,
	WFLBY_GADGET_MDL_BALLOON_B32x32,
	WFLBY_GADGET_MDL_SPARKA16x16,
	WFLBY_GADGET_MDL_SPARKB16x16,
	WFLBY_GADGET_MDL_NUM,
} ;
#define WFLBY_GADGET_MDL_FILE_START		(NARC_wifi_lobby_other_ripple_r_nsbmd)

//-------------------------------------
///	ガジェットテクスチャ数
//=====================================
enum {
	WFLBY_GADGET_TEX_RIPPLE0,
	WFLBY_GADGET_TEX_RIPPLE1,
	WFLBY_GADGET_TEX_RIPPLE2,
	WFLBY_GADGET_TEX_SIGNAL_R,
	WFLBY_GADGET_TEX_SIGNAL_Y,
	WFLBY_GADGET_TEX_SIGNAL_B,
	WFLBY_GADGET_TEX_SWING_R,
	WFLBY_GADGET_TEX_SWING_Y,
	WFLBY_GADGET_TEX_SWING_B,
	WFLBY_GADGET_TEX_CRACLCER_X,
	WFLBY_GADGET_TEX_CRACLCER_B,
	WFLBY_GADGET_TEX_CRACLCER_R,
	WFLBY_GADGET_TEX_FLASH,
	WFLBY_GADGET_TEX_SPARKLE_R,
	WFLBY_GADGET_TEX_SPARKLE_Y,
	WFLBY_GADGET_TEX_SPARKLE_B,
	WFLBY_GADGET_TEX_BALLOON_R,
	WFLBY_GADGET_TEX_BALLOON_Y,
	WFLBY_GADGET_TEX_BALLOON_B,
	WFLBY_GADGET_TEX_ONPU_A_R,
	WFLBY_GADGET_TEX_ONPU_B_R,
	WFLBY_GADGET_TEX_ONPU_C_R,
	WFLBY_GADGET_TEX_ONPU_D_R,
	WFLBY_GADGET_TEX_ONPU_E_R,
	WFLBY_GADGET_TEX_ONPU_F_R,
	WFLBY_GADGET_TEX_ONPU_A_Y,
	WFLBY_GADGET_TEX_ONPU_B_Y,
	WFLBY_GADGET_TEX_ONPU_C_Y,
	WFLBY_GADGET_TEX_ONPU_D_Y,
	WFLBY_GADGET_TEX_ONPU_E_Y,
	WFLBY_GADGET_TEX_ONPU_F_Y,
	WFLBY_GADGET_TEX_ONPU_A_B,
	WFLBY_GADGET_TEX_ONPU_B_B,
	WFLBY_GADGET_TEX_ONPU_C_B,
	WFLBY_GADGET_TEX_ONPU_D_B,
	WFLBY_GADGET_TEX_ONPU_E_B,
	WFLBY_GADGET_TEX_ONPU_F_B,
	WFLBY_GADGET_TEX_SPARK_A,
	WFLBY_GADGET_TEX_SPARK_B,
	WFLBY_GADGET_TEX_NUM,
} ;
#define WFLBY_GADGET_TEX_FILE_START		(NARC_wifi_lobby_other_ripple_r_nsbtx)

//-------------------------------------
///	ガジェットアニメ数
//=====================================
enum {
	WFLBY_GADGET_ANM_RIPPLE1_ICA,
	WFLBY_GADGET_ANM_SIGNAL_ICA,
	WFLBY_GADGET_ANM_SIGNAL_IMA,
	WFLBY_GADGET_ANM_SWING_ICA,
	WFLBY_GADGET_ANM_CRACLCER_X,
	WFLBY_GADGET_ANM_CRACLCER_B,
	WFLBY_GADGET_ANM_CRACLCER_R,
	WFLBY_GADGET_ANM_FLASH,
	WFLBY_GADGET_ANM_SPARKLE_R,
	WFLBY_GADGET_ANM_SPARKLE_Y,
	WFLBY_GADGET_ANM_SPARKLE_B,
	WFLBY_GADGET_ANM_BALLOON_R,
	WFLBY_GADGET_ANM_BALLOON_Y,
	WFLBY_GADGET_ANM_BALLOON_B,
	WFLBY_GADGET_ANM_SPARK_A,
	WFLBY_GADGET_ANM_SPARK_B,
	WFLBY_GADGET_ANM_NUM,
	WFLBY_GADGET_ANM_NONE,
} ;
#define WFLBY_GADGET_ANM_FILE_START		(NARC_wifi_lobby_other_ripple1_nsbca)



//-------------------------------------
///	３DMDLオブジェID開始番号
//=====================================
#define WFLBY_GADGET_3DMDL_OBJID	( 40 )






//-------------------------------------
///	ガジェットアニメ最大数
//=====================================
#define WFLBY_GADGET_ANM_MAX	( 2 )

#define WFLBY_GADGET_ANM_SPEED	( FX32_ONE*2 )

//-------------------------------------
///	１ガジェットで同時に表示するオブジェクトの最大値
//=====================================
#define WFLBY_GADGET_OBJ_MAX	( 18 )

//-------------------------------------
///	ガジェットのY高さ
//=====================================
#define WFLBY_GADGET_FLOOR_Y	( FX32_CONST(8) )	// 床
#define WFLBY_GADGET_AIR_Y		( FX32_CONST(16) )	// 空中



//-------------------------------------
///	クラッカー
//=====================================
enum{
	WFLBY_GADGET_CRACKEROBJ_FLASH,
	WFLBY_GADGET_CRACKEROBJ_CRACKER,
	WFLBY_GADGET_CRACKEROBJ_CRACKER01,
	WFLBY_GADGET_CRACKEROBJ_CRACKER02,

	WFLBY_GADGET_CRACKEROBJ_CRACKER_MAX = 3,
};
#define WFLBY_GADGET_CRACKER_DISX	( FX32_CONST(23) )
#define WFLBY_GADGET_CRACKER_DISZ	( FX32_CONST(28) )
#define WFLBY_GADGET_CRACKER_DISZUN	( FX32_CONST(24) )
#define WFLBY_GADGET_FLASH_DIS		( FX32_CONST(14) )
#define WFLBY_GADGET_FLASH_DISUN	( FX32_CONST(11) )
#define WFLBY_GADGET_FLASH_DISX		( FX32_CONST(7) )
#define WFLBY_GADGET_FLASH_Z		( FX32_CONST(8) )
#define WFLBY_GADGET_FLASH_Y		( FX32_CONST(16) )
#define WFLBY_GADGET_CRACKER_CENTER_X	( FX32_CONST(8) )
#define WFLBY_GADGET_CRACKER_ROT_ADD	( 40 )
#define WFLBY_GADGET_CRACKER_ADD_MOV	( FX32_CONST(8) )
#define WFLBY_GADGET_CRACKER_START_WAIT	( 3 )
#define WFLBY_GADGET_CRACKER_ALPHA_OUT_S	( 20 )
#define WFLBY_GADGET_CRACKER_ALPHA_OUT_SYNC	( 8 )



//-------------------------------------
///	風船
//=====================================
enum{
	WFLBY_GADGET_BALLOON_INIT,
	WFLBY_GADGET_BALLOON_UPMOVE,
	WFLBY_GADGET_BALLOON_DOWNMOVE,
};
enum{
	WFLBY_GADGET_BALLOON_OBJ_BL00,
	WFLBY_GADGET_BALLOON_OBJ_BL01,
	WFLBY_GADGET_BALLOON_OBJ_BL02,
	WFLBY_GADGET_BALLOON_OBJ_NUM,
};
enum{
	WFLBY_GADGET_BALLOON_DOWNSEQ_BATA_WAIT,		// バタバタ開始まち
	WFLBY_GADGET_BALLOON_DOWNSEQ_BATA_START,	// バタバタ開始
	WFLBY_GADGET_BALLOON_DOWNSEQ_BATA_MAIN,		// バタバタメイン
	WFLBY_GADGET_BALLOON_DOWNSEQ_DOWN,			// 落ちる
	WFLBY_GADGET_BALLOON_DOWNSEQ_DON_MAIN,		// ドンメイン
};
enum{
	WFLBY_GADGET_BALLOON_SEQ_UP,		// 上り
	WFLBY_GADGET_BALLOON_SEQ_DOWN,		// くだり
};

// 高さの移動距離
static const u8	sc_WFLBY_GADGET_BALLOON_OBJ_UPDIS[ WFLBY_GADGET_BALLOON_OBJ_NUM ] = {
	48, 84,	128, 
};
#define WFLBY_GADGET_BALLOON_OBJ_UPSIDEDIS		( 8 )	// 揺れ幅
#define WFLBY_GADGET_BALLOON_OBJ_UPROTSPEED		( 10*182 )// 回転スピード
#define WFLBY_GADGET_BALLOON_OBJ_UP_SYNC		( 108 )	// 移動シンク数
#define WFLBY_GADGET_BALLOON_OBJ_DOWN_SYNC		( 10 )	// 移動シンク数
#define WFLBY_GADGET_BALLOON_OBJ_BATA_SWAIT		( 8 )	// バタバタするのが始まるまでのシンク数
#define WFLBY_GADGET_BALLOON_OBJ_BATA_WAIT		( 16 )	// バタバタするシンク数
#define WFLBY_GADGET_BALLOON_OBJ_DON_DIS		( FX32_CONST(8) )	//　DONと移動する距離 
#define WFLBY_GADGET_BALLOON_OBJ_DON_SYNC		( 10 )	// バタバタするシンク数
static const WF2DMAP_POS sc_WFLBY_GADGET_BALLOON_OBJ_POS[ WFLBY_GADGET_BALLOON_OBJ_NUM ] = {
	{  2, 56 },		// 左
	{  14, 56 },	// 右
	{   8, 60 },	// 真ん中
};
#define WFLBY_GADGET_BALLOON_OBJ_POS_Z		( FX32_CONST(1) )
// 高さの移動距離	屋根ありの場合
#define WFLBY_GADGET_BALLOON_OBJ_ROOF_UPDIS			( 2 )	// 上る距離
#define WFLBY_GADGET_BALLOON_OBJ_ROOF_UPSIDEDIS		( 0 )	// 揺れ幅
#define WFLBY_GADGET_BALLOON_OBJ_ROOF_UPROTSPEED	( 10*182 )// 回転スピード
#define WFLBY_GADGET_BALLOON_OBJ_ROOF_UP_SYNC		( 2 )	// 移動シンク数
#define WFLBY_GADGET_BALLOON_OBJ_ROOF_DOWN_SYNC		( 1 )	// 移動シンク数
#define WFLBY_GADGET_BALLOON_OBJ_ROOF_BATA_WAIT		( 8 )	// バタバタするシンク数

static const WF2DMAP_POS sc_WFLBY_GADGET_BALLOON_OBJ_ROOF_POS[ WFLBY_GADGET_BALLOON_OBJ_NUM ] = {
	{  2, 44 },		// 左
	{  14, 44 },	// 右
	{   8, 48 },	// 真ん中
};
#define WFLBY_GADGET_BALLOON_OBJ_ROOF_POS_Z		( -FX32_CONST(8) )
// ガジェットアニメ定数
enum {
	WFLBY_GADGET_BALLOON_ANM_CENTER,
	WFLBY_GADGET_BALLOON_ANM_RIGHT,
	WFLBY_GADGET_BALLOON_ANM_LEFT,
	WFLBY_GADGET_BALLOON_ANM_OTHER,
	WFLBY_GADGET_BALLOON_ANM_CRASH,
	WFLBY_GADGET_BALLOON_ANM_NUM,
} ;
#define WFLBY_GADGET_BALLOON_ANM_WAIT	( 8 )
#define WFLBY_GADGET_BALLOON_ANM_ONEFR	( FX32_ONE*2 )

// 風船飛んでいくときの定数
#define WFLBY_GADGET_BALLOON_OBJ_WAIT	( WFLBY_GADGET_BALLOON_OBJ_DOWN_SYNC + WFLBY_GADGET_BALLOON_OBJ_BATA_WAIT )
#define WFLBY_GADGET_BALLOON_OBJ_MOVE_Y	( FX32_CONST( 64 ) )
#define WFLBY_GADGET_BALLOON_OBJ_MOVE_X	( FX32_CONST( 64 ) )
#define WFLBY_GADGET_BALLOON_OBJ_ROT_S	( 15*182 )
#define WFLBY_GADGET_BALLOON_OBJ_ROT_DIS ( FX32_CONST( 6 ) )



//-------------------------------------
///	スパークル
//=====================================
#define WFLBY_GADGET_SPARKLE_1ROOP_OBJNUM	(4)	// １回の回転で出すオブジェの数
#define WFLBY_GADGET_SPARKLE_1ROOP_MAX		(4)	// 回転の最大数
#define WFLBY_GADGET_SPARKLE_OBJ_MAX		( WFLBY_GADGET_SPARKLE_1ROOP_MAX*WFLBY_GADGET_SPARKLE_1ROOP_OBJNUM )
#define WFLBY_GADGET_SPARKLE_1ROOP_SYNC		(8)
#define WFLBY_GADGET_SPARKLE_OFFS			( 10*FX32_ONE )	// 紙ふぶきが飛ぶ位置
#define WFLBY_GADGET_SPARKLE_CENTER_X		( 8*FX32_ONE )	// 紙ふぶきが飛ぶ位置
#define WFLBY_GADGET_SPARKLE_CENTER_Z		( -8*FX32_ONE )	// 紙ふぶきが飛ぶ位置
#define WFLBY_GADGET_SPARKLE_ANM_SYNC		( 14 )			// １紙ふぶきアニメシンク数
//  主人公からの初期位置
static const VecFx32 sc_WFLBY_GADGET_SPARKLE_OFFS[ WFLBY_GADGET_SPARKLE_1ROOP_OBJNUM ] = {
	{ WFLBY_GADGET_SPARKLE_CENTER_X, WFLBY_GADGET_AIR_Y, WFLBY_GADGET_SPARKLE_CENTER_Z+WFLBY_GADGET_SPARKLE_OFFS },
	{ WFLBY_GADGET_SPARKLE_CENTER_X+WFLBY_GADGET_SPARKLE_OFFS, WFLBY_GADGET_AIR_Y, WFLBY_GADGET_SPARKLE_CENTER_Z },
	{ WFLBY_GADGET_SPARKLE_CENTER_X, WFLBY_GADGET_AIR_Y, WFLBY_GADGET_SPARKLE_CENTER_Z-WFLBY_GADGET_SPARKLE_OFFS },
	{ WFLBY_GADGET_SPARKLE_CENTER_X-WFLBY_GADGET_SPARKLE_OFFS, WFLBY_GADGET_AIR_Y, WFLBY_GADGET_SPARKLE_CENTER_Z },
};
//  主人公からの移動値
#define WFLBY_GADGET_SPARKLE_MOVE_Y		( -FX32_CONST( 8 ) )	// Y方向移動値
#define WFLBY_GADGET_SPARKLE_MOVE_DIS	( FX32_CONST( 8 ) )	// 平面移動値
static const VecFx32 sc_WFLBY_GADGET_SPARKLE_MOVE[ WFLBY_GADGET_SPARKLE_1ROOP_OBJNUM ] = {
	{ 0, WFLBY_GADGET_SPARKLE_MOVE_Y, WFLBY_GADGET_SPARKLE_MOVE_DIS },
	{ WFLBY_GADGET_SPARKLE_MOVE_DIS, WFLBY_GADGET_SPARKLE_MOVE_Y, 0 },
	{ 0, WFLBY_GADGET_SPARKLE_MOVE_Y, -WFLBY_GADGET_SPARKLE_MOVE_DIS },
	{ -WFLBY_GADGET_SPARKLE_MOVE_DIS, WFLBY_GADGET_SPARKLE_MOVE_Y, 0 },
};



//-------------------------------------
///	リップル
//=====================================
#define WFLBY_GADGET_RIPPLE_NUM				( 3 )
#define WFLBY_GADGET_RIPPLE_ROOP_NUM		( 2 )
#define WFLBY_GADGET_RIPPLE_ROOP_START		( 6 )	// そう発射数
#define WFLBY_GADGET_RIPPLE_ROOP_SYNC		( 54 )	// 発射完了シンク数
#define WFLBY_GADGET_RIPPLE_MAT_Z			( FX32_CONST(6) )	// Zオフセット座標



//-------------------------------------
///	シグナル
//=====================================
//1つのアニメの定数
#define WFLBY_GADGET_SIGNAL_ANM_MOVE_X			(FX32_CONST(0))
#define WFLBY_GADGET_SIGNAL_ANM_MOVE_Y			(FX32_CONST(0))
#define WFLBY_GADGET_SIGNAL_ANM_MOVE_Z			(FX32_CONST(0))
#define WFLBY_GADGET_SIGNAL_ANM_FRAME_NUM		(12)
#define WFLBY_GADGET_SIGNAL_ANM_SYNC			(48)
#define WFLBY_GADGET_SIGNAL_OBJ_NUM				(3)
enum{
	WFLBY_GADGET_SINGNAL_ANM_OFF,
	WFLBY_GADGET_SINGNAL_ANM_DRAW00,
	WFLBY_GADGET_SINGNAL_ANM_DRAW01,
};
typedef struct{
	u16 objidx;
	u16 anm_data;
} WFLBY_GADGET_SINGNAL_ANMDATA;
static const WFLBY_GADGET_SINGNAL_ANMDATA sc_WFLBY_GADGET_SIGNAL_ANMSEQ[3][WFLBY_GADGET_SIGNAL_ANM_FRAME_NUM] = {
	{
		{ 0, WFLBY_GADGET_SINGNAL_ANM_DRAW00, },
		{ 0, WFLBY_GADGET_SINGNAL_ANM_OFF,    },
		{ 0, WFLBY_GADGET_SINGNAL_ANM_DRAW01, },
		{ 0, WFLBY_GADGET_SINGNAL_ANM_OFF,    },
		{ 0, WFLBY_GADGET_SINGNAL_ANM_DRAW00, },
		{ 0, WFLBY_GADGET_SINGNAL_ANM_OFF,    },
		{ 0, WFLBY_GADGET_SINGNAL_ANM_DRAW01, },
		{ 0, WFLBY_GADGET_SINGNAL_ANM_OFF,    },
		{ 0, WFLBY_GADGET_SINGNAL_ANM_DRAW00, },
		{ 0, WFLBY_GADGET_SINGNAL_ANM_OFF,    },
		{ 0, WFLBY_GADGET_SINGNAL_ANM_DRAW01, },
		{ 0, WFLBY_GADGET_SINGNAL_ANM_OFF,    },
	},
	{
		{ 1, WFLBY_GADGET_SINGNAL_ANM_DRAW00, },
		{ 1, WFLBY_GADGET_SINGNAL_ANM_OFF,    },
		{ 0, WFLBY_GADGET_SINGNAL_ANM_DRAW01, },
		{ 0, WFLBY_GADGET_SINGNAL_ANM_OFF,    },
		{ 1, WFLBY_GADGET_SINGNAL_ANM_DRAW00, },
		{ 1, WFLBY_GADGET_SINGNAL_ANM_OFF,    },
		{ 0, WFLBY_GADGET_SINGNAL_ANM_DRAW01, },
		{ 0, WFLBY_GADGET_SINGNAL_ANM_OFF,    },
		{ 1, WFLBY_GADGET_SINGNAL_ANM_DRAW00, },
		{ 1, WFLBY_GADGET_SINGNAL_ANM_OFF,    },
		{ 0, WFLBY_GADGET_SINGNAL_ANM_DRAW01, },
		{ 0, WFLBY_GADGET_SINGNAL_ANM_OFF,    },
	},
	{
		{ 0, WFLBY_GADGET_SINGNAL_ANM_DRAW00, },
		{ 0, WFLBY_GADGET_SINGNAL_ANM_OFF,    },
		{ 1, WFLBY_GADGET_SINGNAL_ANM_DRAW01, },
		{ 1, WFLBY_GADGET_SINGNAL_ANM_OFF,    },
		{ 2, WFLBY_GADGET_SINGNAL_ANM_DRAW00, },
		{ 2, WFLBY_GADGET_SINGNAL_ANM_OFF,    },
		{ 0, WFLBY_GADGET_SINGNAL_ANM_DRAW01, },
		{ 1, WFLBY_GADGET_SINGNAL_ANM_DRAW00, },
		{ 2, WFLBY_GADGET_SINGNAL_ANM_DRAW01, },
		{ 0, WFLBY_GADGET_SINGNAL_ANM_DRAW00, },
		{ 1, WFLBY_GADGET_SINGNAL_ANM_DRAW01, },
		{ 2, WFLBY_GADGET_SINGNAL_ANM_DRAW00, },
	},
};



//-------------------------------------
///	スウィング
//=====================================
#define WFLBY_GADGET_SWING_ANM_SYNC		( 32 )
#define	WFLBY_GADGET_SWING_OBJ_NUM		( 3 )



//-------------------------------------
///	音符動作
//=====================================
typedef enum{
	WFLBY_GADGET_ONPU_MAIN_PLAY,		// 再生中
	WFLBY_GADGET_ONPU_MAIN_START,		// 再生中で音符を発射した
	WFLBY_GADGET_ONPU_MAIN_ALLEND,		// 全部終わった
} WFLBY_GADGET_ONPU_MAIN_RET;
enum{
	WFLBY_GADGET_ONPU_MOVE_BELL,	// 音符ベル動作
	WFLBY_GADGET_ONPU_MOVE_DRUM,	// 音符ドラム動作
	WFLBY_GADGET_ONPU_MOVE_CYMBALS,	// 音符シンバル動作
	WFLBY_GADGET_ONPU_MOVE_NUM,		//	音符動作タイプ数
};
#define WFLBY_GADGET_ONPU_OBJWK_NUM	( 16 )
// 音符構成データ
static const u8 sc_WFLBY_LEVEL_00ADDOBJ[ WFLBY_GADGET_ONPU_OBJWK_NUM ] = {
	WFLBY_GADGET_OBJ_ONPU00_R,
	WFLBY_GADGET_OBJ_ONPU01_R,
	WFLBY_GADGET_OBJ_ONPU00_R,
	WFLBY_GADGET_OBJ_ONPU01_R,
	WFLBY_GADGET_OBJ_ONPU00_R,
	WFLBY_GADGET_OBJ_ONPU01_R,
	WFLBY_GADGET_OBJ_ONPU00_R,
	WFLBY_GADGET_OBJ_ONPU01_R,
	WFLBY_GADGET_OBJ_ONPU00_R,
	WFLBY_GADGET_OBJ_ONPU01_R,
	WFLBY_GADGET_OBJ_ONPU00_R,
	WFLBY_GADGET_OBJ_ONPU01_R,
	WFLBY_GADGET_OBJ_ONPU00_R,
	WFLBY_GADGET_OBJ_ONPU01_R,
	WFLBY_GADGET_OBJ_ONPU00_R,
	WFLBY_GADGET_OBJ_ONPU01_R,
};
static const u8 sc_WFLBY_LEVEL_01ADDOBJ[ WFLBY_GADGET_ONPU_OBJWK_NUM ] = {
	WFLBY_GADGET_OBJ_ONPU00_R,
	WFLBY_GADGET_OBJ_ONPU01_R,
	WFLBY_GADGET_OBJ_ONPU02_R,
	WFLBY_GADGET_OBJ_ONPU00_R,
	WFLBY_GADGET_OBJ_ONPU03_Y,
	WFLBY_GADGET_OBJ_ONPU04_Y,
	WFLBY_GADGET_OBJ_ONPU02_Y,
	WFLBY_GADGET_OBJ_ONPU00_Y,
	WFLBY_GADGET_OBJ_ONPU00_Y,
	WFLBY_GADGET_OBJ_ONPU04_Y,
	WFLBY_GADGET_OBJ_ONPU01_R,
	WFLBY_GADGET_OBJ_ONPU00_R,
	WFLBY_GADGET_OBJ_ONPU04_Y,
	WFLBY_GADGET_OBJ_ONPU02_Y,
	WFLBY_GADGET_OBJ_ONPU04_R,
	WFLBY_GADGET_OBJ_ONPU00_R,
};
static const u8 sc_WFLBY_LEVEL_02ADDOBJ[ WFLBY_GADGET_ONPU_OBJWK_NUM ] = {
	WFLBY_GADGET_OBJ_ONPU00_R,
	WFLBY_GADGET_OBJ_ONPU01_R,
	WFLBY_GADGET_OBJ_ONPU00_B,
	WFLBY_GADGET_OBJ_ONPU04_B,
	WFLBY_GADGET_OBJ_ONPU01_B,
	WFLBY_GADGET_OBJ_ONPU03_B,
	WFLBY_GADGET_OBJ_ONPU02_Y,
	WFLBY_GADGET_OBJ_ONPU05_Y,
	WFLBY_GADGET_OBJ_ONPU05_B,
	WFLBY_GADGET_OBJ_ONPU04_Y,
	WFLBY_GADGET_OBJ_ONPU05_R,
	WFLBY_GADGET_OBJ_ONPU02_B,
	WFLBY_GADGET_OBJ_ONPU05_Y,
	WFLBY_GADGET_OBJ_ONPU02_R,
	WFLBY_GADGET_OBJ_ONPU03_B,
	WFLBY_GADGET_OBJ_ONPU00_B,
};
// 動作データ
#define WFLBY_GADGET_ONPU_OBJ_NUM		( 4 )
#define WFLBY_GADGET_ONPU_X_OFS			( FX32_CONST(8) )
#define WFLBY_GADGET_ONPU_Z_OFS			( -FX32_CONST(8) )
//  ベル動作
#define WFLBY_GADGET_ONPU_MOVE_BELL_SYNC	( 18 )
#define WFLBY_GADGET_ONPU_MOVE_BELL_Y		( FX32_CONST(16) )
#define WFLBY_GADGET_ONPU_MOVE_BELL_X0		( FX32_CONST(8) )
#define WFLBY_GADGET_ONPU_MOVE_BELL_X1		( FX32_CONST(12) )
#define WFLBY_GADGET_ONPU_MOVE_BELL_Z		( FX32_CONST(6) )
#define WFLBY_GADGET_ONPU_MOVE_BELL_CVX		( FX32_CONST(6) )
#define WFLBY_GADGET_ONPU_MOVE_BELL_CVSP	( FX_GET_ROTA_NUM(18) )
// ドラム動作
#define WFLBY_GADGET_ONPU_MOVE_DRUM_DOWN_SYNC	( 14 )
#define WFLBY_GADGET_ONPU_MOVE_DRUM_UP_SYNC		( 8 )
#define WFLBY_GADGET_ONPU_MOVE_DRUM_XZ_SYNC		( 19 )
#define WFLBY_GADGET_ONPU_MOVE_DRUM_SYNC		( WFLBY_GADGET_ONPU_MOVE_DRUM_DOWN_SYNC+WFLBY_GADGET_ONPU_MOVE_DRUM_UP_SYNC )
#define WFLBY_GADGET_ONPU_MOVE_DRUM_DOWN_Y		( FX32_CONST(32) )
#define WFLBY_GADGET_ONPU_MOVE_DRUM_UP_Y		( FX32_CONST(12) )
#define WFLBY_GADGET_ONPU_MOVE_DRUM_DOWN_ROT	( FX_GET_ROTA_NUM(30) )
#define WFLBY_GADGET_ONPU_MOVE_DRUM_DOWN_SP		( FX_GET_ROTA_NUM(10) )
#define WFLBY_GADGET_ONPU_MOVE_DRUM_UP_ROT		( FX_GET_ROTA_NUM(0) )
#define WFLBY_GADGET_ONPU_MOVE_DRUM_UP_SP		( FX_GET_ROTA_NUM(20) )
#define WFLBY_GADGET_ONPU_MOVE_DRUM_X			( FX32_CONST(16) )
#define WFLBY_GADGET_ONPU_MOVE_DRUM_Z			( FX32_CONST(16) )
// シンバル
#define WFLBY_GADGET_ONPU_MOVE_CYMBALS_SEQ_NUM			(3)
#define WFLBY_GADGET_ONPU_MOVE_CYMBALS_START_Y			( FX32_CONST(4) )

#define WFLBY_GADGET_ONPU_MOVE_CYMBALS_SEQ00_SYNC		( 8 )
#define WFLBY_GADGET_ONPU_MOVE_CYMBALS_SEQ00_Y00		( FX32_CONST(5) )
#define WFLBY_GADGET_ONPU_MOVE_CYMBALS_SEQ00_Y01		( FX32_CONST(14) )
#define WFLBY_GADGET_ONPU_MOVE_CYMBALS_SEQ00_X00		( FX32_CONST(13) )
#define WFLBY_GADGET_ONPU_MOVE_CYMBALS_SEQ00_X01		( FX32_CONST(7) )

#define WFLBY_GADGET_ONPU_MOVE_CYMBALS_SEQ01_SYNC		( 3 )
#define WFLBY_GADGET_ONPU_MOVE_CYMBALS_SEQ01_Y00		( FX32_CONST(4) )
#define WFLBY_GADGET_ONPU_MOVE_CYMBALS_SEQ01_Y01		( FX32_CONST(3) )
#define WFLBY_GADGET_ONPU_MOVE_CYMBALS_SEQ01_X00		( -FX32_CONST(3) )
#define WFLBY_GADGET_ONPU_MOVE_CYMBALS_SEQ01_X01		( -FX32_CONST(3) )

#define WFLBY_GADGET_ONPU_MOVE_CYMBALS_SEQ02_SYNC		( 4 )
#define WFLBY_GADGET_ONPU_MOVE_CYMBALS_SEQ02_Y00		( FX32_CONST(5) )
#define WFLBY_GADGET_ONPU_MOVE_CYMBALS_SEQ02_Y01		( FX32_CONST(10) )
#define WFLBY_GADGET_ONPU_MOVE_CYMBALS_SEQ02_X00		( FX32_CONST(10) )
#define WFLBY_GADGET_ONPU_MOVE_CYMBALS_SEQ02_X01		( FX32_CONST(6) )



//-------------------------------------
///	音符動作
//=====================================
#define WFLBY_GADGET_MOVE_ONPU_WK_NUM		(4)
typedef struct {
	u16 count;
	u8	anmidx;
	u8	mvnum;
} WFLBY_GADGET_MOVE_ONPU_DATA;
// ベル
#define WFLBY_GADGET_MOVE_ONPU_BELL00_SYSNC		(33)
static const WFLBY_GADGET_MOVE_ONPU_DATA sc_WFLBY_GADGET_MOVE_ONPU_DATA_BELL00[]={
	{ 0, 0, 2 },
	{ 24, 2, 2 },
};
#define WFLBY_GADGET_MOVE_ONPU_BELL01_SYSNC		(33)
static const WFLBY_GADGET_MOVE_ONPU_DATA sc_WFLBY_GADGET_MOVE_ONPU_DATA_BELL01[]={
	{ 0, 0, 3 },
//	{ 11, 1, 3 },
	{ 24, 2, 3 },
//	{ 32, 3, 3 },
};
#define WFLBY_GADGET_MOVE_ONPU_BELL02_SYSNC		(33)
static const WFLBY_GADGET_MOVE_ONPU_DATA sc_WFLBY_GADGET_MOVE_ONPU_DATA_BELL02[]={
	{ 0, 0, 4 },
//	{ 8, 1, 4 },
	{ 16, 2, 4 },
//	{ 24, 3, 4 },
	{ 32, 0, 4 },
};
// ドラム
#define WFLBY_GADGET_MOVE_ONPU_DRUM00_SYSNC		(33)
static const WFLBY_GADGET_MOVE_ONPU_DATA sc_WFLBY_GADGET_MOVE_ONPU_DATA_DRUM00[]={
	{ 0, 0, 2 },
//	{ 8, 1, 2 },
	{ 24, 2, 2 },
//	{ 32, 3, 2 },
};
#define WFLBY_GADGET_MOVE_ONPU_DRUM01_SYSNC		(41)
static const WFLBY_GADGET_MOVE_ONPU_DATA sc_WFLBY_GADGET_MOVE_ONPU_DATA_DRUM01[]={
	{ 0, 0, 3 },
//	{ 8, 1, 3 },
	{ 16, 2, 3 },
//	{ 24, 3, 3 },
	{ 40, 0, 3 },
};
#define WFLBY_GADGET_MOVE_ONPU_DRUM02_SYSNC		(33)
static const WFLBY_GADGET_MOVE_ONPU_DATA sc_WFLBY_GADGET_MOVE_ONPU_DATA_DRUM02[]={
	{ 0, 0, 4 },
//	{ 8, 1, 4 },
	{ 16, 2, 4 },
//	{ 26, 3, 4 },
	{ 35, 0, 4 },
};

// シンバル
#define WFLBY_GADGET_MOVE_ONPU_CYMBALS00_SYSNC		(33)
static const WFLBY_GADGET_MOVE_ONPU_DATA sc_WFLBY_GADGET_MOVE_ONPU_DATA_CYMBALS00[]={
	{ 0, 0, 2 },
	{ 24, 1, 2 },
//	{ 32, 2, 2 },
};
#define WFLBY_GADGET_MOVE_ONPU_CYMBALS01_SYSNC		(33)
static const WFLBY_GADGET_MOVE_ONPU_DATA sc_WFLBY_GADGET_MOVE_ONPU_DATA_CYMBALS01[]={
	{ 0, 0, 2 },
//	{ 8, 1, 2 },
	{ 24, 2, 2 },
//	{ 32, 3, 2 },
};
#define WFLBY_GADGET_MOVE_ONPU_CYMBALS02_SYSNC		(33)
static const WFLBY_GADGET_MOVE_ONPU_DATA sc_WFLBY_GADGET_MOVE_ONPU_DATA_CYMBALS02[]={
	{ 0, 0, 4 },
//	{ 4, 1, 4 },
	{ 20, 2, 4 },
//	{ 30, 3, 4 },
};


//-------------------------------------
///	シンバル稲妻動作
//=====================================
enum{
	WFLBY_GADGET_CYMBALS_INAZUMA_00 = 16,
	WFLBY_GADGET_CYMBALS_INAZUMA_01,
	WFLBY_GADGET_CYMBALS_INAZUMA_NUM = 2,
};
#define WFLBY_GADGET_CYMBALS_INAZUMA_OFS_X		(-FX32_CONST(4))
#define WFLBY_GADGET_CYMBALS_INAZUMA_DIS_X		(FX32_CONST(24))
#define WFLBY_GADGET_CYMBALS_INAZUMA_OFS_Z		(-FX32_CONST(10))
#define WFLBY_GADGET_CYMBALS_INAZUMA_OFS_Y		(-FX32_CONST(0))
#define WFLBY_GADGET_CYMBALS_INAZUMA_ANM_SYNC	(13)


//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------


//-------------------------------------
///	汎用動作システム
//	直線移動
//=====================================
typedef struct {
	s32	count_max;
	fx32 x;
	fx32 y;
	fx32 z;
	fx32 dis_x;
	fx32 start_x;
	fx32 dis_y;
	fx32 start_y;
	fx32 dis_z;
	fx32 start_z;
} WFLBY_GADGET_MV_STRAIGHT;


//-------------------------------------
///	汎用動作システム
//	Sinカーブ移動
//=====================================
typedef struct {
	u16		rota_num;
	u16		speed;
	fx32	dis;
	fx32	num;
} WFLBY_GADGET_MV_SINCURVE;



//-------------------------------------
///	ガジェットリソース構成データ
//	4byte
//=====================================
typedef struct {
	u8 mdl;
	u8 tex;
	u8 anm[ WFLBY_GADGET_ANM_MAX ];
} WFLBY_GADGET_RES;


//-------------------------------------
///	音符動作ワーク
//	4つ音符がグループをくんで動作する
//=====================================
typedef struct {
	s16 count;
	u16	type;
	u16 mvnum;
	u8	move;
	u8	seq;
	WFLBY_GADGET_MV_STRAIGHT	st[WFLBY_GADGET_ONPU_OBJ_NUM];
	WFLBY_GADGET_MV_SINCURVE	cv[WFLBY_GADGET_ONPU_OBJ_NUM];
	D3DOBJ*	p_obj[WFLBY_GADGET_ONPU_OBJ_NUM];
	const WFLBY_3DPERSON* cp_person;
} WFLBY_GADGET_ONPU;




//-------------------------------------
///	描画オブジェ動作ワーク
//=====================================
typedef union {
	struct{
		s8 bl_seq;
		// 風船のかず
		s8	balloon_num;
		s16 count;
		// DONアニメ用
		fx32 don_start_y;
		// アニメウエイト
		u8 anm_wait[ WFLBY_GADGET_BALLOON_OBJ_NUM ];
		u8 pad0;
		u8 anmidx[ WFLBY_GADGET_BALLOON_OBJ_NUM ];
		u8 pad1;
		// 風船飛行カウンタ
		u8 balloon_fly_count;
		// 上に屋根のようなものがあるのかフラグ
		u8 roof_flag;
		u8 pad2[2];
		// 風船動作
		WFLBY_GADGET_MV_SINCURVE	sincurve[WFLBY_GADGET_BALLOON_OBJ_NUM];
		WFLBY_GADGET_MV_STRAIGHT	straight[WFLBY_GADGET_BALLOON_OBJ_NUM];
		// 主人公動作
		WFLBY_GADGET_MV_STRAIGHT	straightobj;
		WFLBY_GADGET_MV_SINCURVE	sincurveobj;
	} balloon;

	struct{
		s32 wait;	
		s32 alpha_count[ WFLBY_GADGET_CRACKEROBJ_CRACKER_MAX ];
	} cracker;

	struct {
		s8 count;			// 動作カウンタ
		s8 roop_count;		// ループ数カウンタ
		u8 roop_num;		// 全体ループ数
		u8 buff_num;		// オブジェ動作数

		// 直線動作
		s8							objcount[WFLBY_GADGET_SPARKLE_OBJ_MAX];
		WFLBY_GADGET_MV_STRAIGHT	objstraight[WFLBY_GADGET_SPARKLE_OBJ_MAX];
	} sparkle;

	struct {
		s8 ripple_num;
		s8 move_num;
		s8 pad[2];
	} ripple;

	struct{
		s32	lastnum;
	} signal;

	struct{
		s16	lastnum;
		s16	objnum;
	} swing;

	struct{
		WFLBY_GADGET_ONPU move[WFLBY_GADGET_MOVE_ONPU_WK_NUM];
		const WFLBY_GADGET_MOVE_ONPU_DATA*	cp_data;
		u16									data_num;
		u16									count_max;

		// ｲﾅｽﾞﾏ用
		u16 inazuma_move;
		u16	inazuma_count;
	} onpu;
} WFLBY_GADGET_OBJWK;


//-------------------------------------
///	描画オブジェ
//=====================================
typedef struct {
	u8			gadget_type;				// ガジェットタイプ
	u8			seq;						// シーケンス
	s16			count;						// カウンタ
	u32			mdl_objid;					// モデルに設定するオブジェID

	// オブジェデータ
	WFLBY_3DPERSON* p_person;

	
	//-------------------------------------
	///	それぞれの初期化関数で設定するデータ
	//=====================================
	// 描画オブジェ
	D3DOBJ		obj[ WFLBY_GADGET_OBJ_MAX ];
	// 動作中のオブジェデータ
	const WFLBY_GADGET_RES*	cp_objres[ WFLBY_GADGET_OBJ_MAX ];
	// アニメーションフレーム
	fx32		anm_frame[ WFLBY_GADGET_OBJ_MAX ][ WFLBY_GADGET_ANM_MAX ];	
	// それぞれの動作用のワーク
	WFLBY_GADGET_OBJWK	mvwk;
} WFLBY_GADGET_OBJ;


//-------------------------------------
///	ガジェットシステム
//=====================================
typedef struct _WFLBY_GADGET{
	WFLBY_SYSTEM*		p_system;	// システム
	WFLBY_3DOBJCONT*	p_objcont;	// オブジェ
	WFLBY_CAMERA*		p_camera;	// カメラオブジェ
	WFLBY_MAPCONT*		p_mapcont;	// マップ管理システム
	
	// ガジェット動作オブジェ
	WFLBY_GADGET_OBJ	obj[ WFLBY_PLAYER_MAX ];	


	// 各リソース
	D3DOBJ_MDL	mdl[ WFLBY_GADGET_MDL_NUM ];		// モデル
	void*		p_texres[ WFLBY_GADGET_TEX_NUM ];	// テクスチャ
	D3DOBJ_ANM	anm[ WFLBY_GADGET_ANM_NUM ];		// アニメ

	// アロケータ
	NNSFndAllocator		allocator;
	
} WFLBY_GADGET;



//-----------------------------------------------------------------------------
/**
 *			ガジェット構成データ
 */
//-----------------------------------------------------------------------------
static const WFLBY_GADGET_RES sc_WFLBY_GADGET_RES[ WFLBY_GADGET_OBJ_NUM ] = {
	// リップル
	{
		WFLBY_GADGET_MDL_RIPPLER64x64,
		WFLBY_GADGET_TEX_RIPPLE0,
		{
			WFLBY_GADGET_ANM_RIPPLE1_ICA,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	// リップル
	{
		WFLBY_GADGET_MDL_RIPPLER64x64,
		WFLBY_GADGET_TEX_RIPPLE1,
		{
			WFLBY_GADGET_ANM_RIPPLE1_ICA,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	// リップル
	{
		WFLBY_GADGET_MDL_RIPPLER64x64,
		WFLBY_GADGET_TEX_RIPPLE2,
		{
			WFLBY_GADGET_ANM_RIPPLE1_ICA,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	// シグナル
	{
		WFLBY_GADGET_MDL_SWING64x64,
		WFLBY_GADGET_TEX_SIGNAL_R,
		{
			WFLBY_GADGET_ANM_SIGNAL_ICA,
			WFLBY_GADGET_ANM_SIGNAL_IMA,
		},
	},

	// シグナル
	{
		WFLBY_GADGET_MDL_SWING64x64,
		WFLBY_GADGET_TEX_SIGNAL_Y,
		{
			WFLBY_GADGET_ANM_SIGNAL_ICA,
			WFLBY_GADGET_ANM_SIGNAL_IMA,
		},
	},

	// シグナル
	{
		WFLBY_GADGET_MDL_SWING64x64,
		WFLBY_GADGET_TEX_SIGNAL_B,
		{
			WFLBY_GADGET_ANM_SIGNAL_ICA,
			WFLBY_GADGET_ANM_SIGNAL_IMA,
		},
	},

	// スイング
	{
		WFLBY_GADGET_MDL_SWING64x64,
		WFLBY_GADGET_TEX_SWING_R,
		{
			WFLBY_GADGET_ANM_SWING_ICA,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	// スイング
	{
		WFLBY_GADGET_MDL_SWING64x64,
		WFLBY_GADGET_TEX_SWING_Y,
		{
			WFLBY_GADGET_ANM_SWING_ICA,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	// スイング
	{
		WFLBY_GADGET_MDL_SWING64x64,
		WFLBY_GADGET_TEX_SWING_B,
		{
			WFLBY_GADGET_ANM_SWING_ICA,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	//  クラッカー00
	{
		WFLBY_GADGET_MDL_CRACKER0232x32,
		WFLBY_GADGET_TEX_CRACLCER_R,
		{
			WFLBY_GADGET_ANM_CRACLCER_R,
			WFLBY_GADGET_ANM_NONE,
		},
	},


	//  クラッカー01
	{
		WFLBY_GADGET_MDL_CRACKER32x32,
		WFLBY_GADGET_TEX_CRACLCER_X,
		{
			WFLBY_GADGET_ANM_CRACLCER_X,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	//  クラッカー02
	{
		WFLBY_GADGET_MDL_CRACKER0132x32,
		WFLBY_GADGET_TEX_CRACLCER_B,
		{
			WFLBY_GADGET_ANM_CRACLCER_B,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	//  フラッシュ
	{
		WFLBY_GADGET_MDL_FLASH32x32,
		WFLBY_GADGET_TEX_FLASH,
		{
			WFLBY_GADGET_ANM_FLASH,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	//  スパークルWFLBY_GADGET_OBJ_SPARKLE00
	{
		WFLBY_GADGET_MDL_SPARKLE_R32x32,
		WFLBY_GADGET_TEX_SPARKLE_R,
		{
			WFLBY_GADGET_ANM_SPARKLE_R,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	//  スパークルWFLBY_GADGET_OBJ_SPARKLE01
	{
		WFLBY_GADGET_MDL_SPARKLE_Y32x32,
		WFLBY_GADGET_TEX_SPARKLE_Y,
		{
			WFLBY_GADGET_ANM_SPARKLE_Y,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	//  スパークルWFLBY_GADGET_OBJ_SPARKLE02
	{
		WFLBY_GADGET_MDL_SPARKLE_B32x32,
		WFLBY_GADGET_TEX_SPARKLE_B,
		{
			WFLBY_GADGET_ANM_SPARKLE_B,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	//  バルーン赤
	{
		WFLBY_GADGET_MDL_BALLOON_R32x32,
		WFLBY_GADGET_TEX_BALLOON_R,
		{
			WFLBY_GADGET_ANM_BALLOON_R,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	//  バルーン黄色
	{
		WFLBY_GADGET_MDL_BALLOON_Y32x32,
		WFLBY_GADGET_TEX_BALLOON_Y,
		{
			WFLBY_GADGET_ANM_BALLOON_Y,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	//  バルーン青
	{
		WFLBY_GADGET_MDL_BALLOON_B32x32,
		WFLBY_GADGET_TEX_BALLOON_B,
		{
			WFLBY_GADGET_ANM_BALLOON_B,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	//  音符A	赤
	{
		WFLBY_GADGET_MDL_16x16,
		WFLBY_GADGET_TEX_ONPU_A_R,
		{
			WFLBY_GADGET_ANM_NONE,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	//  音符B	赤
	{
		WFLBY_GADGET_MDL_16x16,
		WFLBY_GADGET_TEX_ONPU_B_R,
		{
			WFLBY_GADGET_ANM_NONE,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	//  音符C	赤
	{
		WFLBY_GADGET_MDL_16x16,
		WFLBY_GADGET_TEX_ONPU_C_R,
		{
			WFLBY_GADGET_ANM_NONE,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	//  音符D	赤
	{
		WFLBY_GADGET_MDL_16x16,
		WFLBY_GADGET_TEX_ONPU_D_R,
		{
			WFLBY_GADGET_ANM_NONE,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	//  音符E	赤
	{
		WFLBY_GADGET_MDL_16x16,
		WFLBY_GADGET_TEX_ONPU_E_R,
		{
			WFLBY_GADGET_ANM_NONE,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	//  音符F	赤
	{
		WFLBY_GADGET_MDL_16x16,
		WFLBY_GADGET_TEX_ONPU_F_R,
		{
			WFLBY_GADGET_ANM_NONE,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	//  音符A	黄
	{
		WFLBY_GADGET_MDL_16x16,
		WFLBY_GADGET_TEX_ONPU_A_Y,
		{
			WFLBY_GADGET_ANM_NONE,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	//  音符B	黄
	{
		WFLBY_GADGET_MDL_16x16,
		WFLBY_GADGET_TEX_ONPU_B_Y,
		{
			WFLBY_GADGET_ANM_NONE,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	//  音符C	黄
	{
		WFLBY_GADGET_MDL_16x16,
		WFLBY_GADGET_TEX_ONPU_C_Y,
		{
			WFLBY_GADGET_ANM_NONE,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	//  音符D	黄
	{
		WFLBY_GADGET_MDL_16x16,
		WFLBY_GADGET_TEX_ONPU_D_Y,
		{
			WFLBY_GADGET_ANM_NONE,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	//  音符E	黄
	{
		WFLBY_GADGET_MDL_16x16,
		WFLBY_GADGET_TEX_ONPU_E_Y,
		{
			WFLBY_GADGET_ANM_NONE,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	//  音符F	黄
	{
		WFLBY_GADGET_MDL_16x16,
		WFLBY_GADGET_TEX_ONPU_F_Y,
		{
			WFLBY_GADGET_ANM_NONE,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	//  音符A	青
	{
		WFLBY_GADGET_MDL_16x16,
		WFLBY_GADGET_TEX_ONPU_A_B,
		{
			WFLBY_GADGET_ANM_NONE,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	//  音符B	青
	{
		WFLBY_GADGET_MDL_16x16,
		WFLBY_GADGET_TEX_ONPU_B_B,
		{
			WFLBY_GADGET_ANM_NONE,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	//  音符C	青
	{
		WFLBY_GADGET_MDL_16x16,
		WFLBY_GADGET_TEX_ONPU_C_B,
		{
			WFLBY_GADGET_ANM_NONE,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	//  音符D	青
	{
		WFLBY_GADGET_MDL_16x16,
		WFLBY_GADGET_TEX_ONPU_D_B,
		{
			WFLBY_GADGET_ANM_NONE,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	//  音符E	青
	{
		WFLBY_GADGET_MDL_16x16,
		WFLBY_GADGET_TEX_ONPU_E_B,
		{
			WFLBY_GADGET_ANM_NONE,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	//  音符F	青
	{
		WFLBY_GADGET_MDL_16x16,
		WFLBY_GADGET_TEX_ONPU_F_B,
		{
			WFLBY_GADGET_ANM_NONE,
			WFLBY_GADGET_ANM_NONE,
		},
	},


	// いなずま左
	{
		WFLBY_GADGET_MDL_SPARKA16x16,
		WFLBY_GADGET_TEX_SPARK_A,
		{
			WFLBY_GADGET_ANM_SPARK_A,
			WFLBY_GADGET_ANM_NONE,
		},
	},

	// いなずま右
	{
		WFLBY_GADGET_MDL_SPARKB16x16,
		WFLBY_GADGET_TEX_SPARK_B,
		{
			WFLBY_GADGET_ANM_SPARK_B,
			WFLBY_GADGET_ANM_NONE,
		},
	},
	
};


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

// 汎用動作関数
// 直線移動
static void WFLBY_GADGET_MV_Straight_Init( WFLBY_GADGET_MV_STRAIGHT* p_wk, fx32 s_x, fx32 e_x, fx32 s_y, fx32 e_y, fx32 s_z, fx32 e_z, s32 count_max );
static BOOL WFLBY_GADGET_MV_Straight_Main( WFLBY_GADGET_MV_STRAIGHT* p_wk, s32 count );
static void WFLBY_GADGET_MV_Straight_GetNum( const WFLBY_GADGET_MV_STRAIGHT* cp_wk, fx32* p_x, fx32* p_y, fx32* p_z );
// Sinカーブ移動
static void WFLBY_GADGET_MV_SinCurve_Init( WFLBY_GADGET_MV_SINCURVE* p_wk, u16 s_rota, u16 speed, fx32 dis );
static void WFLBY_GADGET_MV_SinCurve_Main( WFLBY_GADGET_MV_SINCURVE* p_wk );
static void WFLBY_GADGET_MV_SinCurve_GetNum( const WFLBY_GADGET_MV_SINCURVE* cp_wk, fx32* p_num );

// SE再生
static void WFLBY_GADGET_SePlay( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk, u32 seno );


// モデルデータ
static void WFLBY_GADGET_LoadMdl( WFLBY_GADGET* p_sys, ARCHANDLE* p_handle, u32 gheapID );
static void WFLBY_GADGET_DeleteMdl( WFLBY_GADGET* p_sys );
// テクスチャデータ
static void WFLBY_GADGET_LoadTex( WFLBY_GADGET* p_sys, ARCHANDLE* p_handle, u32 gheapID );
static void WFLBY_GADGET_DeleteTex( WFLBY_GADGET* p_sys );
// アニメデータ
static void WFLBY_GADGET_LoadAnm( WFLBY_GADGET* p_sys, ARCHANDLE* p_handle, u32 gheapID );
static void WFLBY_GADGET_DeleteAnm( WFLBY_GADGET* p_sys );


// ガジェットオブジェ
static void WFLBY_GADGET_OBJ_Start( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk, WFLBY_3DPERSON* p_person, WFLBY_ITEMTYPE gadget );
static void WFLBY_GADGET_OBJ_Main( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static BOOL WFLBY_GADGET_OBJ_Draw( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static void WFLBY_GADGET_OBJ_End( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static BOOL WFLBY_GADGET_OBJ_CheckMove( const WFLBY_GADGET_OBJ* cp_wk );

static BOOL WFLBY_GADGET_OBJ_CheckRes( const WFLBY_GADGET_OBJ* cp_wk, u32 idx );
static void WFLBY_GADGET_OBJ_SetRes( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk, u32 idx, const WFLBY_GADGET_RES* cp_resdata );
static void WFLBY_GADGET_OBJ_DrawRes( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk, u32 idx );
static void WFLBY_GADGET_OBJ_LoopAnm( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk, u32 objidx, u32 anmidx );
static BOOL WFLBY_GADGET_OBJ_NoLoopAnm( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk, u32 objidx, u32 anmidx );
static void WFLBY_GADGET_OBJ_LoopAnm_Sp( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk, u32 objidx, u32 anmidx, fx32 speed );
static BOOL WFLBY_GADGET_OBJ_NoLoopAnm_Sp( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk, u32 objidx, u32 anmidx, fx32 speed );
static void WFLBY_GADGET_OBJ_SetFrame( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk, u32 objidx, u32 anmidx, fx32 frame );
static fx32 WFLBY_GADGET_OBJ_GetFrame( const WFLBY_GADGET* cp_sys, const WFLBY_GADGET_OBJ* cp_wk, u32 objidx, u32 anmidx );


// 各ガジェットタイプのアニメデータ
static void WFLBY_GADGET_ANM_Init_Rippru00( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static void WFLBY_GADGET_ANM_Init_Rippru01( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static void WFLBY_GADGET_ANM_Init_Rippru02( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static void WFLBY_GADGET_ANM_Init_Swing00( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static void WFLBY_GADGET_ANM_Init_Swing01( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static void WFLBY_GADGET_ANM_Init_Swing02( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static void WFLBY_GADGET_ANM_Init_Signal00( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static void WFLBY_GADGET_ANM_Init_Cracker00( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static void WFLBY_GADGET_ANM_Init_Cracker01( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static void WFLBY_GADGET_ANM_Init_Cracker02( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static void WFLBY_GADGET_ANM_Init_Balloon00( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static void WFLBY_GADGET_ANM_Init_Balloon01( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static void WFLBY_GADGET_ANM_Init_Balloon02( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static void WFLBY_GADGET_ANM_Init_Sparkle00( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static void WFLBY_GADGET_ANM_Init_Sparkle01( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static void WFLBY_GADGET_ANM_Init_Sparkle02( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static void WFLBY_GADGET_ANM_Init_Bell00( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static void WFLBY_GADGET_ANM_Init_Bell01( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static void WFLBY_GADGET_ANM_Init_Bell02( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static void WFLBY_GADGET_ANM_Init_Drum00( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static void WFLBY_GADGET_ANM_Init_Drum01( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static void WFLBY_GADGET_ANM_Init_Drum02( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static void WFLBY_GADGET_ANM_Init_Cymbals00( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static void WFLBY_GADGET_ANM_Init_Cymbals01( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static void WFLBY_GADGET_ANM_Init_Cymbals02( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );

static BOOL WFLBY_GADGET_ANM_Main_AnmTwo( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static BOOL WFLBY_GADGET_ANM_Main_AnmOne( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static BOOL WFLBY_GADGET_ANM_Main_Rippru00( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static BOOL WFLBY_GADGET_ANM_Main_Signal00( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static BOOL WFLBY_GADGET_ANM_Main_Signal01( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static BOOL WFLBY_GADGET_ANM_Main_Signal02( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static BOOL WFLBY_GADGET_ANM_Main_Swing00( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static BOOL WFLBY_GADGET_ANM_Main_Cracker00( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static BOOL WFLBY_GADGET_ANM_Main_Balloon00( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static BOOL WFLBY_GADGET_ANM_Main_Sparkle00( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static BOOL WFLBY_GADGET_ANM_Main_Onpu00( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static BOOL WFLBY_GADGET_ANM_Main_Cymbals( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );


static BOOL WFLBY_GADGET_ANM_Draw_Obj( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );




// 風船動作
static void WFLBY_GADGET_Balloon_InitUp( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ*  p_wk, u32 balloon_num );
static void WFLBY_GADGET_Balloon_InitUp_Normal( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ*  p_wk );
static void WFLBY_GADGET_Balloon_InitUp_Roof( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ*  p_wk );
static BOOL WFLBY_GADGET_Balloon_MainUp( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ*  p_wk );
static BOOL WFLBY_GADGET_Balloon_MainUp_Normal( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ*  p_wk );
static BOOL WFLBY_GADGET_Balloon_MainUp_Roof( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ*  p_wk );
static void WFLBY_GADGET_Balloon_InitDown( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ*  p_wk );
static void WFLBY_GADGET_Balloon_InitDown_Normal( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ*  p_wk );
static void WFLBY_GADGET_Balloon_InitDown_Roof( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ*  p_wk );
static BOOL WFLBY_GADGET_Balloon_MainDown( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ*  p_wk );
static BOOL WFLBY_GADGET_Balloon_MainDown_Normal( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ*  p_wk );
static BOOL WFLBY_GADGET_Balloon_MainDown_Roof( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ*  p_wk );
static void WFLBY_GADGET_Balloon_SetObjPos( WFLBY_GADGET_OBJ*  p_wk );
static void WFLBY_GADGET_Balloon_SetObjPos_Roof( WFLBY_GADGET_OBJ*  p_wk );
static BOOL WFLBY_GADGET_Balloon_SetAnm( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ*  p_wk, u32 balloon_idx, u32 anmidx );
static void WFLBY_GADGET_Balloon_Anm( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ*  p_wk, u32 balloon_idx );

// すぱーくる
static void WFLBY_GADGET_Sparkle_Init( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk, u32 roop_num );
static BOOL WFLBY_GADGET_Sparkle_OneObj_Main( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk, u32 objidx );


// リップル
static void WFLBY_GADGET_Ripple_Init( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk, u32 ripple_num );


// シグナル
static BOOL WFLBY_GADGET_Signal_Main( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk, u32 num );

// スウィング
static void WFLBY_GADGET_Swing_Init( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk, u32 num );

// ｲﾅｽﾞﾏ
static void WFLBY_GADGET_Inazuma_Init( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static void WFLBY_GADGET_Inazuma_Start( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );
static void WFLBY_GADGET_Inazuma_Main( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk );

// 音符発射システム
static void WFLBY_GADGET_ONPU_Cont_Init( WFLBY_GADGET_OBJWK* p_wk, const WFLBY_GADGET_MOVE_ONPU_DATA* cp_data, u32 data_num, u32 count_max, u32 anm_type, WFLBY_GADGET_OBJ* p_gadget );
static WFLBY_GADGET_ONPU_MAIN_RET WFLBY_GADGET_ONPU_Cont_Main( WFLBY_GADGET_OBJWK* p_wk, u32 count );

// 音符ここの動き
static void WFLBY_GADGET_OnpuMove_Init( WFLBY_GADGET_ONPU* p_wk, D3DOBJ* p_obj0, D3DOBJ* p_obj1, D3DOBJ* p_obj2, D3DOBJ* p_obj3, const WFLBY_3DPERSON* cp_person, u32 anm_type );
static void WFLBY_GADGET_OnpuMove_Start( WFLBY_GADGET_ONPU* p_wk, u32 num );
static BOOL WFLBY_GADGET_OnpuMove_Main( WFLBY_GADGET_ONPU* p_wk );
//ベル
static void WFLBY_GADGET_OnpuMove_InitBell( WFLBY_GADGET_ONPU* p_wk, const WFLBY_3DPERSON* cp_person );
static BOOL WFLBY_GADGET_OnpuMove_MainBell( WFLBY_GADGET_ONPU* p_wk );
//ドラム
static void WFLBY_GADGET_OnpuMove_InitDram( WFLBY_GADGET_ONPU* p_wk, const WFLBY_3DPERSON* cp_person );
static BOOL WFLBY_GADGET_OnpuMove_MainDram( WFLBY_GADGET_ONPU* p_wk );
//シンバル
static void WFLBY_GADGET_OnpuMove_InitCymbals( WFLBY_GADGET_ONPU* p_wk, const WFLBY_3DPERSON* cp_person );
static BOOL WFLBY_GADGET_OnpuMove_MainCymbals( WFLBY_GADGET_ONPU* p_wk );
static void WFLBY_GADGET_OnpuMove_SetCymbalsStParam( WFLBY_GADGET_ONPU* p_wk, u32 idx, const VecFx32* cp_def_matrix, fx32 ofs_x0, fx32 ofs_y0, fx32 ofs_x1, fx32 ofs_y1, u32 sync );

//----------------------------------------------------------------------------
/**
 *	@brief	ガジェットシステム初期化
 *
 *	@param	p_system		ロビーシステム
 *	@param	p_mapcont		マップ管理システム
 *	@param	p_camera		カメラ管理システム
 *	@param	p_objcont		オブジェ管理システム	
 *	@param	heapID			ヒープID
 *	@param	gheapID			グラフィックヒープID
 */
//-----------------------------------------------------------------------------
WFLBY_GADGET* WFLBY_GADGET_Init( WFLBY_SYSTEM* p_system, WFLBY_MAPCONT* p_mapcont, WFLBY_CAMERA* p_camera, WFLBY_3DOBJCONT* p_objcont, u32 heapID, u32 gheapID )
{
	WFLBY_GADGET* p_sys;

	// システムワーク作成
	p_sys = sys_AllocMemory( heapID, sizeof(WFLBY_GADGET) );
	memset( p_sys, 0, sizeof(WFLBY_GADGET) );

	// オブジェ管理システム保存
	p_sys->p_system		= p_system;
	p_sys->p_objcont	= p_objcont;
	p_sys->p_camera		= p_camera;
	p_sys->p_mapcont	= p_mapcont;

	// リソース読み込み
	{
		ARCHANDLE* p_handle;
		p_handle = ArchiveDataHandleOpen( ARC_WIFILOBBY_OTHER_GRA, heapID );

		// アロケータ作成
		sys_InitAllocator( &p_sys->allocator, gheapID, 4 );

		// モデル読み込み
		WFLBY_GADGET_LoadMdl( p_sys, p_handle, gheapID );

		// テクスチャ読み込み
		WFLBY_GADGET_LoadTex( p_sys, p_handle, gheapID );

		// アニメ読み込み
		WFLBY_GADGET_LoadAnm( p_sys, p_handle, gheapID );

		ArchiveDataHandleClose( p_handle );
	}

	return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ガジェットシステム破棄
 *
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_GADGET_Exit( WFLBY_GADGET* p_sys )
{
	// アニメデータ破棄
	{
		// モデル読み込み
		WFLBY_GADGET_DeleteMdl( p_sys );

		// テクスチャ読み込み
		WFLBY_GADGET_DeleteTex( p_sys );

		// アニメ読み込み
		WFLBY_GADGET_DeleteAnm( p_sys );
	}

	// 実態の破棄
	sys_FreeMemoryEz( p_sys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ガジェットメイン
 *
 *	@param	p_sys	ガジェットシステム
 */
//-----------------------------------------------------------------------------
void WFLBY_GADGET_Main( WFLBY_GADGET* p_sys )
{
	int i;

#ifdef WFLBY_DEBUG_GADGET_ALL_PUT
	if( sys.cont & PAD_BUTTON_R ){
		const WFLBY_USER_PROFILE* cp_profile;
		cp_profile = WFLBY_SYSTEM_GetMyProfileLocal( p_sys->p_system );
		for( i=0; i<WFLBY_PLAYER_MAX; i++ ){

			// その人がいるかチェック
			if( WFLBY_3DOBJCONT_GetPlIDWk( p_sys->p_objcont, i ) != NULL ){
				if( WFLBY_GADGET_EndWait( p_sys, i ) == TRUE ){
//					WFLBY_GADGET_Start( p_sys, i, (gf_mtRand() % WFLBY_ITEM_NUM) );
					WFLBY_GADGET_Start( p_sys, i, WFLBY_SYSTEM_GetProfileItem( cp_profile ) );
				}
			}
		}
	}
#endif
	
	
	// 動作中のオブジェを動かす
	for( i=0; i<WFLBY_PLAYER_MAX; i++ ){
		WFLBY_GADGET_OBJ_Main( p_sys, &p_sys->obj[i] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ガジェット	描画
 *
 *	@param	p_sys	ガジェットシステム
 */
//-----------------------------------------------------------------------------
void WFLBY_GADGET_Draw( WFLBY_GADGET* p_sys )
{
	int i;
	BOOL result;
	WFLBY_3DPERSON* p_player;

	p_player = WFLBY_3DOBJCONT_GetPlayer( p_sys->p_objcont );

	// 動作中のオブジェを描画
	for( i=0; i<WFLBY_PLAYER_MAX; i++ ){
		WFLBY_GADGET_OBJ_Draw( p_sys, &p_sys->obj[i] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ガジェット	開始
 *
 *	@param	p_sys	ガジェットシステム
 *	@param	idx		ユーザID
 *	@param	gadget	ガジェットタイプ
 */
//-----------------------------------------------------------------------------
void WFLBY_GADGET_Start( WFLBY_GADGET* p_sys, u32 idx, WFLBY_ITEMTYPE gadget )
{	
	WFLBY_3DPERSON* p_person;
	
	GF_ASSERT( idx < WFLBY_PLAYER_MAX );
	
	// 開始
	p_person = WFLBY_3DOBJCONT_GetPlIDWk( p_sys->p_objcont, idx );
	if( p_person == NULL ){
		OS_TPrintf( "gadget start err person none\n" );
		return ; 
	}
	WFLBY_GADGET_OBJ_Start( p_sys, &p_sys->obj[idx], p_person, gadget );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ガジェット	終了待ち
 *
 *	@param	cp_sys	ガジェットシステム
 *	@param	idx		ユーザID
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_GADGET_EndWait( const WFLBY_GADGET* cp_sys, u32 idx )
{
	if( WFLBY_GADGET_OBJ_CheckMove( &cp_sys->obj[idx] ) == TRUE ){
		return FALSE;
	}
	return TRUE;
}





//-----------------------------------------------------------------------------
/**
 *			プライベート関数
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	直進移動
 *
 *	@param	p_wk		ワーク
 *	@param	s_x			開始ｘ座標	
 *	@param	e_x			終了ｘ座標
 *	@param	s_y			開始ｙ座標
 *	@param	e_y			終了ｙ座標
 *	@param	s_z			開始ｚ座標
 *	@param	e_z			終了ｚ座標
 *	@param	count_max	最大カウント数
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_MV_Straight_Init( WFLBY_GADGET_MV_STRAIGHT* p_wk, fx32 s_x, fx32 e_x, fx32 s_y, fx32 e_y, fx32 s_z, fx32 e_z, s32 count_max )
{
	p_wk->count_max		= count_max;
	p_wk->x				= s_x;
	p_wk->dis_x			= e_x - s_x;
	p_wk->start_x		= s_x;
	p_wk->y				= s_y;
	p_wk->dis_y			= e_y - s_y;
	p_wk->start_y		= s_y;
	p_wk->z				= s_z;
	p_wk->dis_z			= e_z - s_z;
	p_wk->start_z		= s_z;
}

//----------------------------------------------------------------------------
/**
 *	@brief	直進移動	メイン
 *
 *	@param	p_wk		ワーク
 *	@param	count		カウント値
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_GADGET_MV_Straight_Main( WFLBY_GADGET_MV_STRAIGHT* p_wk, s32 count )
{
	s32 count_num;	
	BOOL ret = FALSE;

	// カウント値の最大値オーバーチェック
	if( count > p_wk->count_max ){
		count_num = p_wk->count_max;
		ret = TRUE;
	}else{
		count_num = count;
	}
	if( p_wk->dis_x != 0 ){
		p_wk->x = p_wk->start_x + FX_Div(FX_Mul( FX32_CONST(count_num), p_wk->dis_x ), FX32_CONST(p_wk->count_max));
	}
	if( p_wk->dis_y != 0 ){
		p_wk->y = p_wk->start_y + FX_Div(FX_Mul( FX32_CONST(count_num), p_wk->dis_y ), FX32_CONST(p_wk->count_max));
	}
	if( p_wk->dis_z != 0 ){
		p_wk->z = p_wk->start_z + FX_Div(FX_Mul( FX32_CONST(count_num), p_wk->dis_z ), FX32_CONST(p_wk->count_max));
	}

	return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief	直進移動	値を取得
 *
 *	@param	cp_wk		ワーク
 *	@param	p_x			ｘ座標
 *	@param	p_y			ｙ座標
 *	@param	p_z			ｚ座標
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_MV_Straight_GetNum( const WFLBY_GADGET_MV_STRAIGHT* cp_wk, fx32* p_x, fx32* p_y, fx32* p_z )
{
	*p_x = cp_wk->x;
	*p_y = cp_wk->y;
	*p_z = cp_wk->z;
}


//----------------------------------------------------------------------------
/**
 *	@brief	サインカーブ動作	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	s_rota		開始回転角度
 *	@param	speed		スピード
 *	@param	dis			距離
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_MV_SinCurve_Init( WFLBY_GADGET_MV_SINCURVE* p_wk, u16 s_rota, u16 speed, fx32 dis )
{
	p_wk->rota_num	= s_rota;
	p_wk->speed		= speed;
	p_wk->dis		= dis;
	p_wk->num		= FX_Mul( FX_SinIdx( s_rota ), p_wk->dis );
}

//----------------------------------------------------------------------------
/**
 *	@brief	サインカーブ動作	メイン
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_MV_SinCurve_Main( WFLBY_GADGET_MV_SINCURVE* p_wk )
{
	p_wk->rota_num	+= p_wk->speed;
	p_wk->num		= FX_Mul( FX_SinIdx( p_wk->rota_num ), p_wk->dis );
}

//----------------------------------------------------------------------------
/**
 *	@brief	サインカーブ動作	値取得
 *
 *	@param	cp_wk	ワーク
 *	@param	p_num	移動値
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_MV_SinCurve_GetNum( const WFLBY_GADGET_MV_SINCURVE* cp_wk, fx32* p_num )
{
	*p_num = cp_wk->num;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ガジェットSE再生
 *
 *	@param	p_sys		ガジェットシステム
 *	@param	p_wk		ワーク
 *	@param	seno		SEナンバー
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_SePlay( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk, u32 seno )
{
	// 自分と他人で再生方法を変更
	if( p_wk->p_person == WFLBY_3DOBJCONT_GetPlayer( p_sys->p_objcont ) ){
		Snd_SePlayEx( seno, SND_PLAYER_NO_WIFI_HIROBA );	//自分専用のプレイヤーナンバーで再生
	}else{
		if( WFLBY_3DOBJCONT_GetCullingFlag( p_wk->p_person ) == FALSE ){
			Snd_SePlay( seno );
		}
	}
}



//----------------------------------------------------------------------------
/**
 *	@brief	モデルデータ読み込み
 *
 *	@param	p_sys		システムワーク
 *	@param	gheapID		グラフィックヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_LoadMdl( WFLBY_GADGET* p_sys, ARCHANDLE* p_handle, u32 gheapID )
{
	int i;

	for( i=0; i<WFLBY_GADGET_MDL_NUM; i++ ){
		p_sys->mdl[i].pResMdl	= ArcUtil_HDL_Load( p_handle, 
					WFLBY_GADGET_MDL_FILE_START + i,
					FALSE, gheapID, ALLOC_TOP );
		p_sys->mdl[i].pModelSet	= NNS_G3dGetMdlSet( p_sys->mdl[i].pResMdl );
		p_sys->mdl[i].pModel	= NNS_G3dGetMdlByIdx( p_sys->mdl[i].pModelSet, 0 );

		// テクスチャは後でバインドする
		p_sys->mdl[i].pMdlTex = NULL;

		// エミッションを明るくする
		NNS_G3dMdlSetMdlEmiAll( p_sys->mdl[i].pModel, GX_RGB( 31,31,31 ) );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	モデルデータ破棄
 *
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_DeleteMdl( WFLBY_GADGET* p_sys )
{
	int i;

	for( i=0; i<WFLBY_GADGET_MDL_NUM; i++ ){
		sys_FreeMemoryEz( p_sys->mdl[i].pResMdl );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	テクスチャ読み込み
 *
 *	@param	p_sys		システムワーク
 *	@param	p_handle	ハンドル
 *	@param	gheapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_LoadTex( WFLBY_GADGET* p_sys, ARCHANDLE* p_handle, u32 gheapID )
{
	int i;

	for( i=0; i<WFLBY_GADGET_TEX_NUM; i++ ){
		WFLBY_3DMAPOBJ_TEX_LoatCutTex( &p_sys->p_texres[i], p_handle,
							WFLBY_GADGET_TEX_FILE_START+i, gheapID );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	テクスチャ破棄
 *
 *	@param	p_sys	システムワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_DeleteTex( WFLBY_GADGET* p_sys )
{
	int i;
	NNSG3dTexKey texKey;
	NNSG3dTexKey tex4x4Key;
	NNSG3dPlttKey plttKey;
	NNSG3dResTex* p_tex;

	for( i=0; i<WFLBY_GADGET_TEX_NUM; i++ ){

		p_tex = NNS_G3dGetTex( p_sys->p_texres[i] );
		//VramKey破棄
		NNS_G3dTexReleaseTexKey( p_tex, &texKey, &tex4x4Key );
		NNS_GfdFreeTexVram( texKey );	
		NNS_GfdFreeTexVram( tex4x4Key );	

		plttKey = NNS_G3dPlttReleasePlttKey( p_tex );
		NNS_GfdFreePlttVram( plttKey );

		sys_FreeMemoryEz( p_sys->p_texres[i] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメデータ読み込み
 *
 *	@param	p_sys		システムワーク
 *	@param	p_handle	ハンドル
 *	@param	gheapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_LoadAnm( WFLBY_GADGET* p_sys, ARCHANDLE* p_handle, u32 gheapID )
{
	int i, j;

	// アニメはモデルデータが必要なためsc_WFLBY_GADGET_RESをしようして初期化する。
	// sc_WFLBY_GADGET_RESに重複してアニメ定数を指定したときは、
	// 最初の１回だけメモリを確保する
	for( i=0; i<WFLBY_GADGET_OBJ_NUM; i++ ){
		for( j=0; j<WFLBY_GADGET_ANM_MAX; j++ ){
			if( sc_WFLBY_GADGET_RES[i].anm[j] != WFLBY_GADGET_ANM_NONE ){
				if( p_sys->anm[ sc_WFLBY_GADGET_RES[i].anm[j] ].pResAnm == NULL ){

					// テクスチャアニメ用に対応するテクスチャをモデルに設定する
					p_sys->mdl[ sc_WFLBY_GADGET_RES[i].mdl ].pMdlTex = NNS_G3dGetTex( p_sys->p_texres[sc_WFLBY_GADGET_RES[i].tex] );
					
					D3DOBJ_AnmLoadH( &p_sys->anm[ sc_WFLBY_GADGET_RES[i].anm[j] ], 
							&p_sys->mdl[ sc_WFLBY_GADGET_RES[i].mdl ], p_handle, 
							WFLBY_GADGET_ANM_FILE_START + sc_WFLBY_GADGET_RES[i].anm[j], 
							gheapID, &p_sys->allocator );
				}
			}
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメリソースの破棄
 *
 *	@param	p_sys	システムワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_DeleteAnm( WFLBY_GADGET* p_sys )
{
	int i;

	for( i=0; i<WFLBY_GADGET_ANM_NUM; i++ ){
		if( p_sys->anm[i].pResAnm != NULL ){
			D3DOBJ_AnmDelete( &p_sys->anm[i], &p_sys->allocator );
			p_sys->anm[i].pResAnm = NULL;
		}
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief	オブジェアニメ開始
 *
 *	@param	p_sys		システムワーク
 *	@param	p_wk		ワーク
 *	@param	p_person	人物ワーク
 *	@param	gadget		ガジェット
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_OBJ_Start( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk, WFLBY_3DPERSON* p_person, WFLBY_ITEMTYPE gadget )
{
	static void (* const pFunc[ WFLBY_ITEM_NUM ])( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk ) = {
		WFLBY_GADGET_ANM_Init_Bell00,		// ベル小
		WFLBY_GADGET_ANM_Init_Bell01,		// ベル中
		WFLBY_GADGET_ANM_Init_Bell02,		// ベル大
		WFLBY_GADGET_ANM_Init_Drum00,       // ドラム小
		WFLBY_GADGET_ANM_Init_Drum01,       // ドラム中
		WFLBY_GADGET_ANM_Init_Drum02,       // ドラム大
		WFLBY_GADGET_ANM_Init_Cymbals00,	// シンバル小
		WFLBY_GADGET_ANM_Init_Cymbals01,	// シンバル中
		WFLBY_GADGET_ANM_Init_Cymbals02,	// シンバル大
		WFLBY_GADGET_ANM_Init_Rippru00,     // リップル小
		WFLBY_GADGET_ANM_Init_Rippru01,     // リップル中
		WFLBY_GADGET_ANM_Init_Rippru02,     // リップル大
		WFLBY_GADGET_ANM_Init_Signal00,     // シグナル小
		WFLBY_GADGET_ANM_Init_Signal00,     // シグナル中
		WFLBY_GADGET_ANM_Init_Signal00,     // シグナル大
		WFLBY_GADGET_ANM_Init_Swing00,   	// スウィング小
		WFLBY_GADGET_ANM_Init_Swing01,       // スウィング中
		WFLBY_GADGET_ANM_Init_Swing02,       // スウィング大
		WFLBY_GADGET_ANM_Init_Cracker00,     // クラッカー小
		WFLBY_GADGET_ANM_Init_Cracker01,     // クラッカー中
		WFLBY_GADGET_ANM_Init_Cracker02,     // クラッカー大
		WFLBY_GADGET_ANM_Init_Sparkle00,     // スパークル小
		WFLBY_GADGET_ANM_Init_Sparkle01,     // スパークル中
		WFLBY_GADGET_ANM_Init_Sparkle02,     // スパークル大
		WFLBY_GADGET_ANM_Init_Balloon00,   // バルーン小
		WFLBY_GADGET_ANM_Init_Balloon01,   // バルーン中
		WFLBY_GADGET_ANM_Init_Balloon02,   // バルーン大
	};
	static const u32 sc_SndData[ WFLBY_ITEM_NUM ] = {
		WFLBY_SND_TOUCH_TOY01_1,
		WFLBY_SND_TOUCH_TOY01_2,
		WFLBY_SND_TOUCH_TOY01_3,
		WFLBY_SND_TOUCH_TOY02_1,
		WFLBY_SND_TOUCH_TOY02_2,
		WFLBY_SND_TOUCH_TOY02_3,
		WFLBY_SND_TOUCH_TOY03_1,
		WFLBY_SND_TOUCH_TOY03_2,
		WFLBY_SND_TOUCH_TOY03_3,
		WFLBY_SND_TOUCH_TOY04_1,
		WFLBY_SND_TOUCH_TOY04_2,
		WFLBY_SND_TOUCH_TOY04_3,
		WFLBY_SND_TOUCH_TOY05_1,
		WFLBY_SND_TOUCH_TOY05_2,
		WFLBY_SND_TOUCH_TOY05_3,
		WFLBY_SND_TOUCH_TOY06_1,
		WFLBY_SND_TOUCH_TOY06_2,
		WFLBY_SND_TOUCH_TOY06_3,
		WFLBY_SND_TOUCH_TOY07_1,
		WFLBY_SND_TOUCH_TOY07_2,
		WFLBY_SND_TOUCH_TOY07_3,
		WFLBY_SND_TOUCH_TOY08_1,
		WFLBY_SND_TOUCH_TOY08_2,
		WFLBY_SND_TOUCH_TOY08_3,
		WFLBY_SND_TOUCH_TOY09_1,
		WFLBY_SND_TOUCH_TOY09_2,
		WFLBY_SND_TOUCH_TOY09_3,
	};
	GF_ASSERT( gadget < WFLBY_ITEM_NUM );

	// 動作中ならそのデータ破棄
	WFLBY_GADGET_OBJ_End( p_sys, p_wk );
	
	// 人物ワーク設定
	p_wk->p_person		= p_person;

	// ガジェットタイプ設定
	p_wk->gadget_type	= gadget;

	// モデルに設定するオブジェID
	p_wk->mdl_objid		= WFLBY_GADGET_3DMDL_OBJID + WFLBY_3DOBJCONT_GetWkObjData( p_person, WF2DMAP_OBJPM_ST );

	// 描画データ設定
	pFunc[ p_wk->gadget_type ]( p_sys, p_wk );

	// 画面内に人がいたら音を鳴らす
	WFLBY_GADGET_SePlay( p_sys, p_wk, sc_SndData[ p_wk->gadget_type ] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ガジェットオブジェ動作メイン
 *
 *	@param	p_sys		システムワーク
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_OBJ_Main( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	static BOOL (* const pFunc[ WFLBY_ITEM_NUM ])( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk ) = {
		WFLBY_GADGET_ANM_Main_Onpu00,		// ベル小
		WFLBY_GADGET_ANM_Main_Onpu00,		// ベル中
		WFLBY_GADGET_ANM_Main_Onpu00,		// ベル大
		WFLBY_GADGET_ANM_Main_Onpu00,		// ドラム小
		WFLBY_GADGET_ANM_Main_Onpu00,		// ドラム中
		WFLBY_GADGET_ANM_Main_Onpu00,		// ドラム大
		WFLBY_GADGET_ANM_Main_Cymbals,		// シンバル小
		WFLBY_GADGET_ANM_Main_Cymbals,		// シンバル中
		WFLBY_GADGET_ANM_Main_Cymbals,		// シンバル大
		WFLBY_GADGET_ANM_Main_Rippru00,		// リップル小
		WFLBY_GADGET_ANM_Main_Rippru00,		// リップル中
		WFLBY_GADGET_ANM_Main_Rippru00,		// リップル大
		WFLBY_GADGET_ANM_Main_Signal00,		// シグナル小
		WFLBY_GADGET_ANM_Main_Signal01,	    // シグナル中
		WFLBY_GADGET_ANM_Main_Signal02,	    // シグナル大
		WFLBY_GADGET_ANM_Main_Swing00,		// スウィング小
		WFLBY_GADGET_ANM_Main_Swing00,		// スウィング中
		WFLBY_GADGET_ANM_Main_Swing00,		// スウィング大
		WFLBY_GADGET_ANM_Main_Cracker00,	// クラッカー小
		WFLBY_GADGET_ANM_Main_Cracker00,	// クラッカー中
		WFLBY_GADGET_ANM_Main_Cracker00,	// クラッカー大
		WFLBY_GADGET_ANM_Main_Sparkle00,	// スパークル小
		WFLBY_GADGET_ANM_Main_Sparkle00,	// スパークル中
		WFLBY_GADGET_ANM_Main_Sparkle00,	// スパークル大
		WFLBY_GADGET_ANM_Main_Balloon00,	// バルーン小
		WFLBY_GADGET_ANM_Main_Balloon00,	// バルーン中
		WFLBY_GADGET_ANM_Main_Balloon00,	// バルーン大
	};

	// データがあるかチェック
	if( WFLBY_GADGET_OBJ_CheckMove( p_wk ) == FALSE ){
		return ;
	}
	
	GF_ASSERT( p_wk->gadget_type < WFLBY_ITEM_NUM );

	if( pFunc[ p_wk->gadget_type ]( p_sys, p_wk ) == TRUE ){
		WFLBY_GADGET_OBJ_End( p_sys, p_wk );	// 終了
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ガジェットオブジェ描画
 *
 *	@param	p_sys		システムワーク
 *	@param	p_wk		ワーク
 *
 *	@retval	TRUE	描画した
 *	@retval	FALSE	描画してない
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_GADGET_OBJ_Draw( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	/*
	static void (* const pFunc[ WFLBY_ITEM_NUM ])( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk ) = {
		WFLBY_GADGET_ANM_Draw_Obj,		// ベル小
		WFLBY_GADGET_ANM_Draw_Obj,		// ベル中
		WFLBY_GADGET_ANM_Draw_Obj,		// ベル大
		WFLBY_GADGET_ANM_Draw_Obj,		// ドラム小
		WFLBY_GADGET_ANM_Draw_Obj,		// ドラム中
		WFLBY_GADGET_ANM_Draw_Obj,		// ドラム大
		WFLBY_GADGET_ANM_Draw_Obj,		// シンバル小
		WFLBY_GADGET_ANM_Draw_Obj,		// シンバル中
		WFLBY_GADGET_ANM_Draw_Obj,		// シンバル大
		WFLBY_GADGET_ANM_Draw_Obj,		// リップル小
		WFLBY_GADGET_ANM_Draw_Obj,		// リップル中
		WFLBY_GADGET_ANM_Draw_Obj,		// リップル大
		WFLBY_GADGET_ANM_Draw_Obj,		// シグナル小
		WFLBY_GADGET_ANM_Draw_Obj,	    // シグナル中
		WFLBY_GADGET_ANM_Draw_Obj,	    // シグナル大
		WFLBY_GADGET_ANM_Draw_Obj,		// スウィング小
		WFLBY_GADGET_ANM_Draw_Obj,		// スウィング中
		WFLBY_GADGET_ANM_Draw_Obj,		// スウィング大
		WFLBY_GADGET_ANM_Draw_Obj,		// クラッカー小
		WFLBY_GADGET_ANM_Draw_Obj,		// クラッカー中
		WFLBY_GADGET_ANM_Draw_Obj,		// クラッカー大
		WFLBY_GADGET_ANM_Draw_Obj,		// スパークル小
		WFLBY_GADGET_ANM_Draw_Obj,		// スパークル中
		WFLBY_GADGET_ANM_Draw_Obj,		// スパークル大
		WFLBY_GADGET_ANM_Draw_Obj,		// バルーン小
		WFLBY_GADGET_ANM_Draw_Obj,		// バルーン中
		WFLBY_GADGET_ANM_Draw_Obj,		// バルーン大
	};
	*/

	// データがあるかチェック
	if( WFLBY_GADGET_OBJ_CheckMove( p_wk ) == FALSE ){
		return FALSE;
	}

	GF_ASSERT( p_wk->gadget_type < WFLBY_ITEM_NUM );

//	pFunc[ p_wk->gadget_type ]( p_sys, p_wk );
	return WFLBY_GADGET_ANM_Draw_Obj( p_sys, p_wk );		// 基本これで全部いける
}

//----------------------------------------------------------------------------
/**
 *	@brief	ガジェットオブジェ動作終了
 *
 *	@param	p_sys		システムワーク
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_OBJ_End( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	// 全ワークの初期化
	memset( p_wk, 0, sizeof(WFLBY_GADGET_OBJ) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ガジェットオブジェが動作中かチェックする
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	TRUE	動作中
 *	@retval	FALSE	停止中
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_GADGET_OBJ_CheckMove( const WFLBY_GADGET_OBJ* cp_wk )
{
	if( cp_wk->p_person == NULL ){
		return FALSE;
	}
	return TRUE;
}



//----------------------------------------------------------------------------
/**
 *	@brief	IDXの描画オブジェが動いてるかチェックする
 *
 *	@param	cp_wk		ワーク
 *	@param	idx			インデックス
 *
 *	@retval	TRUE	そのIDXの描画オブジェが動いている
 *	@retval	FALSE	うごいてない
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_GADGET_OBJ_CheckRes( const WFLBY_GADGET_OBJ* cp_wk, u32 idx )
{
	if( cp_wk->cp_objres[idx] == NULL ){
		return FALSE;
	}
	return TRUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief	ガジェットオブジェ	リソース設定
 *
 *	@param	p_sys		システムワーク
 *	@param	p_wk		ワーク
 *	@param	idx			D3DOBJ　インデックス
 *	@param	cp_resdata	リソース構成データ
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_OBJ_SetRes( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk, u32 idx, const WFLBY_GADGET_RES* cp_resdata )
{
	int i;
	
	GF_ASSERT( idx < WFLBY_GADGET_OBJ_MAX );
	// オブジェ設定ずみでないかチェック
	GF_ASSERT( p_wk->cp_objres[idx] == NULL );


	// 初期化情報出力
	{
//		OS_TPrintf( "init mdl %d  tex %d anm [%d] [%d]\n", cp_resdata->mdl, cp_resdata->tex, cp_resdata->anm[0], cp_resdata->anm[1] );
	}
	
	
	// オブジェ構成データを保存
	p_wk->cp_objres[idx] = cp_resdata;
	
	// モデルの設定
	D3DOBJ_Init( &p_wk->obj[idx], &p_sys->mdl[cp_resdata->mdl] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	１D3DOBJの描画処理
 *
 *	@param	p_sys	システムワーク
 *	@param	p_wk	ワーク
 *	@param	idx		インデックス
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_OBJ_DrawRes( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk, u32 idx )
{
	BOOL result;
	void* p_tex;
	D3DOBJ_MDL* p_mdl;
	D3DOBJ_ANM* p_anm;
	int i;

	GF_ASSERT( p_wk->cp_objres[idx] != NULL );

	if( D3DOBJ_GetDraw( &p_wk->obj[idx] ) == FALSE ){
		return ;
	}

	// テクスチャとモデル取得
	p_tex = p_sys->p_texres[ p_wk->cp_objres[idx]->tex ];
	p_mdl = &p_sys->mdl[ p_wk->cp_objres[idx]->mdl ];
	
	// テクスチャバインド
	p_mdl->pMdlTex = NNS_G3dGetTex( p_tex );
	// 無理やり貼り付けてみる
	result = NNS_G3dForceBindMdlTex( p_mdl->pModel, p_mdl->pMdlTex, 0, 0 );
	GF_ASSERT( result );
	// 無理やり貼り付けてみる
	result = NNS_G3dForceBindMdlPltt( p_mdl->pModel, p_mdl->pMdlTex, 0, 0 );
	GF_ASSERT( result );

	// アニメの適用
	for( i=0; i<WFLBY_GADGET_ANM_MAX; i++ ){
		if( p_wk->cp_objres[idx]->anm[i] != WFLBY_GADGET_ANM_NONE ){
			p_anm = &p_sys->anm[ p_wk->cp_objres[idx]->anm[i] ];
			D3DOBJ_AddAnm( &p_wk->obj[idx], p_anm );
			// フレームをあわせる
			D3DOBJ_AnmSet( p_anm, p_wk->anm_frame[idx][i] );
		}else{
			break;
		}
	}

	// OBJIDの設定
	NNS_G3dMdlSetMdlPolygonIDAll( p_mdl->pModel, p_wk->mdl_objid );

	// 描画
	D3DOBJ_Draw( &p_wk->obj[idx] );	

	// アニメを破棄
	for( i=0; i<WFLBY_GADGET_ANM_MAX; i++ ){
		if( p_wk->cp_objres[idx]->anm[i] != WFLBY_GADGET_ANM_NONE ){
			p_anm = &p_sys->anm[ p_wk->cp_objres[idx]->anm[i] ];
			D3DOBJ_DelAnm( &p_wk->obj[idx], p_anm );
		}else{
			break;
		}
	}

	// バインド解除
	NNS_G3dReleaseMdlTex( p_mdl->pModel );
	NNS_G3dReleaseMdlPltt( p_mdl->pModel );
//	NNS_G3dReleaseMdlSet(p_mdl->pModelSet );
	p_mdl->pMdlTex = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ループアニメ
 *
 *	@param	p_sys		システムワーク
 *	@param	p_wk		ワーク
 *	@param	objidx		オブジェIDX
 *	@param	anmidx		アニメIDX
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_OBJ_LoopAnm( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk, u32 objidx, u32 anmidx )
{
	WFLBY_GADGET_OBJ_LoopAnm_Sp( p_sys, p_wk, objidx, anmidx, WFLBY_GADGET_ANM_SPEED );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ルーぷしないアニメ
 *
 *	@param	p_sys		システムワーク
 *	@param	p_wk		ワーク
 *	@param	objidx		オブジェIDX
 *	@param	anmidx		アニメIDX
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_GADGET_OBJ_NoLoopAnm( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk, u32 objidx, u32 anmidx )
{
	return WFLBY_GADGET_OBJ_NoLoopAnm_Sp( p_sys, p_wk, objidx, anmidx, WFLBY_GADGET_ANM_SPEED );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ループ再生	スピード指定版
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_OBJ_LoopAnm_Sp( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk, u32 objidx, u32 anmidx, fx32 speed )
{
	fx32 end_frame;
	D3DOBJ_ANM* p_anm;

	// アニメオブジェ取得
	p_anm = &p_sys->anm[ p_wk->cp_objres[ objidx ]->anm[ anmidx ] ];

	end_frame = D3DOBJ_AnmGetFrameNum( p_anm );

	// アニメを進める
	if( (p_wk->anm_frame[objidx][anmidx] + speed) < end_frame ){
		p_wk->anm_frame[objidx][anmidx] += speed;
	}else{
		p_wk->anm_frame[objidx][anmidx] = (p_wk->anm_frame[objidx][anmidx] + WFLBY_GADGET_ANM_SPEED) % end_frame;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	NOループ再生	スピード指定版
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_GADGET_OBJ_NoLoopAnm_Sp( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk, u32 objidx, u32 anmidx, fx32 speed )
{
	fx32 end_frame;
	BOOL ret;
	D3DOBJ_ANM* p_anm;

	// アニメオブジェ取得
	p_anm = &p_sys->anm[ p_wk->cp_objres[ objidx ]->anm[ anmidx ] ];

	end_frame = D3DOBJ_AnmGetFrameNum( p_anm );

	// アニメを進める
	if( (p_wk->anm_frame[objidx][anmidx] + speed) < end_frame ){
		p_wk->anm_frame[objidx][anmidx] += speed;
		ret = FALSE;
	}else{
		p_wk->anm_frame[objidx][anmidx] =  end_frame - FX32_HALF;
		ret = TRUE;
	}

	return ret;
}

// フレーム数の設定
static void WFLBY_GADGET_OBJ_SetFrame( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk, u32 objidx, u32 anmidx, fx32 frame )
{
	fx32 end_frame;
	D3DOBJ_ANM* p_anm;

	// アニメオブジェ取得
	p_anm = &p_sys->anm[ p_wk->cp_objres[ objidx ]->anm[ anmidx ] ];

	end_frame = D3DOBJ_AnmGetFrameNum( p_anm );

	if( end_frame < frame ){
		p_wk->anm_frame[objidx][anmidx] =  end_frame - FX32_HALF;
	}else{
		p_wk->anm_frame[objidx][anmidx] =  frame;
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	アニメフレームを取得する
 *
 *	@param	cp_sys		システム
 *	@param	cp_wk		ワーク
 *	@param	objidx		オブジェIDX
 *	@param	anmidx		アニメIDX
 *
 *	@return	フレーム
 */
//-----------------------------------------------------------------------------
static fx32 WFLBY_GADGET_OBJ_GetFrame( const WFLBY_GADGET* cp_sys, const WFLBY_GADGET_OBJ* cp_wk, u32 objidx, u32 anmidx )
{
	fx32 end_frame;
	const D3DOBJ_ANM* cp_anm;

	// アニメオブジェ取得
	cp_anm = &cp_sys->anm[ cp_wk->cp_objres[ objidx ]->anm[ anmidx ] ];

	return D3DOBJ_AnmGet( cp_anm );
}


//----------------------------------------------------------------------------
/**
 *	@brief	アニメの初期化
 *
 *	@param	p_sys	システムワーク
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
// リップル
static void WFLBY_GADGET_ANM_Init_Rippru00( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	WFLBY_GADGET_Ripple_Init( p_sys, p_wk, 1 );
}
static void WFLBY_GADGET_ANM_Init_Rippru01( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	WFLBY_GADGET_Ripple_Init( p_sys, p_wk, 2 );
}
static void WFLBY_GADGET_ANM_Init_Rippru02( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	WFLBY_GADGET_Ripple_Init( p_sys, p_wk, 3 );
}
// ウインク
static void WFLBY_GADGET_ANM_Init_Swing00( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	WFLBY_GADGET_Swing_Init( p_sys, p_wk, 1 );
}
static void WFLBY_GADGET_ANM_Init_Swing01( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	WFLBY_GADGET_Swing_Init( p_sys, p_wk, 2 );
}
static void WFLBY_GADGET_ANM_Init_Swing02( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	WFLBY_GADGET_Swing_Init( p_sys, p_wk, 3 );
}
// シグナル
static void WFLBY_GADGET_ANM_Init_Signal00( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	int i;
	VecFx32 matrix;


	for( i=0; i<WFLBY_GADGET_SIGNAL_OBJ_NUM; i++ ){
		WFLBY_GADGET_OBJ_SetRes( p_sys, p_wk, i, &sc_WFLBY_GADGET_RES[WFLBY_GADGET_OBJ_SIGNAL00 + i] );

		// 座標をあわせる
		WFLBY_3DOBJCONT_DRAW_Get3DMatrix( p_wk->p_person, &matrix );
		// 設定
		matrix.x += WFLBY_GADGET_SIGNAL_ANM_MOVE_X;
		matrix.z += WFLBY_GADGET_SIGNAL_ANM_MOVE_Z;
		matrix.y += WFLBY_GADGET_FLOOR_Y + (FX_Mul( FX32_ONE*i, WFLBY_GADGET_SIGNAL_ANM_MOVE_Y ));
		D3DOBJ_SetMatrix( &p_wk->obj[i], matrix.x, matrix.y, matrix.z );

		// 非表示
		D3DOBJ_SetDraw( &p_wk->obj[i], FALSE );
	}

	p_wk->mvwk.signal.lastnum = 0xff;
}

// クラッカー
static void WFLBY_GADGET_ANM_Init_Cracker00( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	VecFx32 matrix, flash_matrix;
	u32	way;
	
	WFLBY_GADGET_OBJ_SetRes( p_sys, p_wk, WFLBY_GADGET_CRACKEROBJ_CRACKER, &sc_WFLBY_GADGET_RES[WFLBY_GADGET_OBJ_CRACLCER00] );
	WFLBY_GADGET_OBJ_SetRes( p_sys, p_wk, WFLBY_GADGET_CRACKEROBJ_FLASH, &sc_WFLBY_GADGET_RES[WFLBY_GADGET_OBJ_FLASH00] );

	// クラッカーの描画OFF
	D3DOBJ_SetDraw( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER], FALSE );

	// 座標をあわせる
	WFLBY_3DOBJCONT_DRAW_Get3DMatrix( p_wk->p_person, &matrix );
	matrix.y		+= WFLBY_GADGET_AIR_Y;
	matrix.x		+= WFLBY_GADGET_CRACKER_CENTER_X;
	flash_matrix	= matrix;
	flash_matrix.y	+= WFLBY_GADGET_FLASH_Y;


	// アルファカウンタ初期化
	p_wk->mvwk.cracker.alpha_count[0] = WFLBY_GADGET_CRACKER_ALPHA_OUT_SYNC;

	// 回転角度
	way = WFLBY_3DOBJCONT_GetWkObjData( p_wk->p_person, WF2DMAP_OBJPM_WAY );
	switch( way ){
	case WF2DMAP_WAY_UP:
		matrix.z		-= 	WFLBY_GADGET_CRACKER_DISZ;
		flash_matrix.z	-=	WFLBY_GADGET_FLASH_DIS;
		D3DOBJ_SetRota( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER], FX_GET_ROTA_NUM(270), D3DOBJ_ROTA_WAY_Y );
		break;
		
	case WF2DMAP_WAY_DOWN:
		matrix.z		+= 	WFLBY_GADGET_CRACKER_DISZUN;
		flash_matrix.z	+=	WFLBY_GADGET_FLASH_DISUN;
		D3DOBJ_SetRota( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER], FX_GET_ROTA_NUM(90), D3DOBJ_ROTA_WAY_Y );
		break;
		
	case WF2DMAP_WAY_LEFT:
		matrix.x		-= 	WFLBY_GADGET_CRACKER_DISX;
		flash_matrix.x	-=	WFLBY_GADGET_FLASH_DISX;
		flash_matrix.z	+=	WFLBY_GADGET_FLASH_Z;
		D3DOBJ_SetRota( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER], FX_GET_ROTA_NUM(0), D3DOBJ_ROTA_WAY_Y );
		break;
		
	case WF2DMAP_WAY_RIGHT:
		matrix.x		+= 	WFLBY_GADGET_CRACKER_DISX;
		flash_matrix.x	+=	WFLBY_GADGET_FLASH_DISX;
		flash_matrix.z	+=	WFLBY_GADGET_FLASH_Z;
		D3DOBJ_SetRota( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER], FX_GET_ROTA_NUM(180), D3DOBJ_ROTA_WAY_Y );
		break;

	default:
		break;
	}

	// 設定
	D3DOBJ_SetMatrix( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER], matrix.x, matrix.y, matrix.z );
	D3DOBJ_SetMatrix( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_FLASH], flash_matrix.x, flash_matrix.y, flash_matrix.z );
}
// クラッカー２こ
static void WFLBY_GADGET_ANM_Init_Cracker01( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	VecFx32 matrix, flash_matrix;
	u32	way;
	
	WFLBY_GADGET_OBJ_SetRes( p_sys, p_wk, WFLBY_GADGET_CRACKEROBJ_CRACKER, &sc_WFLBY_GADGET_RES[WFLBY_GADGET_OBJ_CRACLCER00] );
	WFLBY_GADGET_OBJ_SetRes( p_sys, p_wk, WFLBY_GADGET_CRACKEROBJ_FLASH, &sc_WFLBY_GADGET_RES[WFLBY_GADGET_OBJ_FLASH00] );
	WFLBY_GADGET_OBJ_SetRes( p_sys, p_wk, WFLBY_GADGET_CRACKEROBJ_CRACKER01, &sc_WFLBY_GADGET_RES[WFLBY_GADGET_OBJ_CRACLCER01] );

	// クラッカーの描画OFF
	D3DOBJ_SetDraw( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER], FALSE );

	// 座標をあわせる
	WFLBY_3DOBJCONT_DRAW_Get3DMatrix( p_wk->p_person, &matrix );
	matrix.y		+= WFLBY_GADGET_AIR_Y;
	matrix.x		+= WFLBY_GADGET_CRACKER_CENTER_X;
	flash_matrix	= matrix;
	flash_matrix.y	+= WFLBY_GADGET_FLASH_Y;

	// アルファカウンタ初期化
	p_wk->mvwk.cracker.alpha_count[0] = WFLBY_GADGET_CRACKER_ALPHA_OUT_SYNC;
	p_wk->mvwk.cracker.alpha_count[1] = WFLBY_GADGET_CRACKER_ALPHA_OUT_SYNC;

	// 回転角度
	way = WFLBY_3DOBJCONT_GetWkObjData( p_wk->p_person, WF2DMAP_OBJPM_WAY );
	switch( way ){
	case WF2DMAP_WAY_UP:
		matrix.z		-= 	WFLBY_GADGET_CRACKER_DISZ;
		flash_matrix.z	-=	WFLBY_GADGET_FLASH_DIS;
		D3DOBJ_SetRota( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER], 
				FX_GET_ROTA_NUM((270-WFLBY_GADGET_CRACKER_ROT_ADD)), D3DOBJ_ROTA_WAY_Y );
		D3DOBJ_SetRota( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER01], 
				FX_GET_ROTA_NUM((270+WFLBY_GADGET_CRACKER_ROT_ADD)), D3DOBJ_ROTA_WAY_Y );

		D3DOBJ_SetMatrix( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER], matrix.x + WFLBY_GADGET_CRACKER_ADD_MOV, 
				matrix.y, matrix.z );
		D3DOBJ_SetMatrix( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER01], matrix.x - WFLBY_GADGET_CRACKER_ADD_MOV, 
				matrix.y, matrix.z );
		break;
		
	case WF2DMAP_WAY_DOWN:
		matrix.z		+= 	WFLBY_GADGET_CRACKER_DISZUN;
		flash_matrix.z	+=	WFLBY_GADGET_FLASH_DISUN;
		D3DOBJ_SetRota( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER], 
				FX_GET_ROTA_NUM((90-WFLBY_GADGET_CRACKER_ROT_ADD)), D3DOBJ_ROTA_WAY_Y );
		D3DOBJ_SetRota( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER01], 
				FX_GET_ROTA_NUM((90+WFLBY_GADGET_CRACKER_ROT_ADD)), D3DOBJ_ROTA_WAY_Y );

		D3DOBJ_SetMatrix( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER], matrix.x - WFLBY_GADGET_CRACKER_ADD_MOV, 
				matrix.y, matrix.z );
		D3DOBJ_SetMatrix( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER01], matrix.x + WFLBY_GADGET_CRACKER_ADD_MOV, 
				matrix.y, matrix.z );
		break;
		
	case WF2DMAP_WAY_LEFT:
		matrix.x		-= 	WFLBY_GADGET_CRACKER_DISX;
		flash_matrix.x	-=	WFLBY_GADGET_FLASH_DISX;
		flash_matrix.z	+=	WFLBY_GADGET_FLASH_Z;
		D3DOBJ_SetRota( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER], 
				FX_GET_ROTA_NUM((0-WFLBY_GADGET_CRACKER_ROT_ADD)), D3DOBJ_ROTA_WAY_Y );
		D3DOBJ_SetRota( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER01], 
				FX_GET_ROTA_NUM((0+WFLBY_GADGET_CRACKER_ROT_ADD)), D3DOBJ_ROTA_WAY_Y );

		D3DOBJ_SetMatrix( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER], matrix.x, 
				matrix.y, matrix.z - WFLBY_GADGET_CRACKER_ADD_MOV );
		D3DOBJ_SetMatrix( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER01], matrix.x, 
				matrix.y, matrix.z + WFLBY_GADGET_CRACKER_ADD_MOV );
		break;
		
	case WF2DMAP_WAY_RIGHT:
		matrix.x		+= 	WFLBY_GADGET_CRACKER_DISX;
		flash_matrix.x	+=	WFLBY_GADGET_FLASH_DISX;
		flash_matrix.z	+=	WFLBY_GADGET_FLASH_Z;
		D3DOBJ_SetRota( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER], 
				FX_GET_ROTA_NUM((180-WFLBY_GADGET_CRACKER_ROT_ADD)), D3DOBJ_ROTA_WAY_Y );
		D3DOBJ_SetRota( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER01], 
				FX_GET_ROTA_NUM((180+WFLBY_GADGET_CRACKER_ROT_ADD)), D3DOBJ_ROTA_WAY_Y );

		D3DOBJ_SetMatrix( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER], matrix.x, 
				matrix.y, matrix.z + WFLBY_GADGET_CRACKER_ADD_MOV );
		D3DOBJ_SetMatrix( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER01], matrix.x, 
				matrix.y, matrix.z - WFLBY_GADGET_CRACKER_ADD_MOV );
		break;

	default:
		break;
	}

	// 設定
	D3DOBJ_SetMatrix( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_FLASH], flash_matrix.x, flash_matrix.y, flash_matrix.z );
}
// クラッカー３こ
static void WFLBY_GADGET_ANM_Init_Cracker02( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	VecFx32 matrix, flash_matrix;
	u32	way;
	
	WFLBY_GADGET_OBJ_SetRes( p_sys, p_wk, WFLBY_GADGET_CRACKEROBJ_CRACKER, &sc_WFLBY_GADGET_RES[WFLBY_GADGET_OBJ_CRACLCER00] );
	WFLBY_GADGET_OBJ_SetRes( p_sys, p_wk, WFLBY_GADGET_CRACKEROBJ_FLASH, &sc_WFLBY_GADGET_RES[WFLBY_GADGET_OBJ_FLASH00] );
	WFLBY_GADGET_OBJ_SetRes( p_sys, p_wk, WFLBY_GADGET_CRACKEROBJ_CRACKER01, &sc_WFLBY_GADGET_RES[WFLBY_GADGET_OBJ_CRACLCER01] );
	WFLBY_GADGET_OBJ_SetRes( p_sys, p_wk, WFLBY_GADGET_CRACKEROBJ_CRACKER02, &sc_WFLBY_GADGET_RES[WFLBY_GADGET_OBJ_CRACLCER02] );

	// クラッカーの描画OFF
	D3DOBJ_SetDraw( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER], FALSE );

	// アルファカウンタ初期化
	p_wk->mvwk.cracker.alpha_count[0] = WFLBY_GADGET_CRACKER_ALPHA_OUT_SYNC;
	p_wk->mvwk.cracker.alpha_count[1] = WFLBY_GADGET_CRACKER_ALPHA_OUT_SYNC;
	p_wk->mvwk.cracker.alpha_count[2] = WFLBY_GADGET_CRACKER_ALPHA_OUT_SYNC;

	// 座標をあわせる
	WFLBY_3DOBJCONT_DRAW_Get3DMatrix( p_wk->p_person, &matrix );
	matrix.y		+= WFLBY_GADGET_AIR_Y;
	matrix.x		+= WFLBY_GADGET_CRACKER_CENTER_X;
	flash_matrix	= matrix;
	flash_matrix.y	+= WFLBY_GADGET_FLASH_Y;

	// 回転角度
	way = WFLBY_3DOBJCONT_GetWkObjData( p_wk->p_person, WF2DMAP_OBJPM_WAY );
	switch( way ){
	case WF2DMAP_WAY_UP:
		matrix.z		-= 	WFLBY_GADGET_CRACKER_DISZ;
		flash_matrix.z	-=	WFLBY_GADGET_FLASH_DIS;
		D3DOBJ_SetRota( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER], 
				FX_GET_ROTA_NUM((270-WFLBY_GADGET_CRACKER_ROT_ADD)), D3DOBJ_ROTA_WAY_Y );
		D3DOBJ_SetRota( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER01], 
				FX_GET_ROTA_NUM((270+WFLBY_GADGET_CRACKER_ROT_ADD)), D3DOBJ_ROTA_WAY_Y );
		D3DOBJ_SetRota( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER02], 
				FX_GET_ROTA_NUM(270), D3DOBJ_ROTA_WAY_Y );

		D3DOBJ_SetMatrix( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER], matrix.x + WFLBY_GADGET_CRACKER_ADD_MOV, 
				matrix.y, matrix.z );
		D3DOBJ_SetMatrix( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER01], matrix.x - WFLBY_GADGET_CRACKER_ADD_MOV, 
				matrix.y, matrix.z );
		D3DOBJ_SetMatrix( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER02], matrix.x, matrix.y, matrix.z );
		break;
		
	case WF2DMAP_WAY_DOWN:
		matrix.z		+= 	WFLBY_GADGET_CRACKER_DISZUN;
		flash_matrix.z	+=	WFLBY_GADGET_FLASH_DISUN;
		D3DOBJ_SetRota( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER], 
				FX_GET_ROTA_NUM((90-WFLBY_GADGET_CRACKER_ROT_ADD)), D3DOBJ_ROTA_WAY_Y );
		D3DOBJ_SetRota( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER01], 
				FX_GET_ROTA_NUM((90+WFLBY_GADGET_CRACKER_ROT_ADD)), D3DOBJ_ROTA_WAY_Y );
		D3DOBJ_SetRota( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER02], 
				FX_GET_ROTA_NUM(90), D3DOBJ_ROTA_WAY_Y );

		D3DOBJ_SetMatrix( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER], matrix.x - WFLBY_GADGET_CRACKER_ADD_MOV, 
				matrix.y, matrix.z );
		D3DOBJ_SetMatrix( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER01], matrix.x + WFLBY_GADGET_CRACKER_ADD_MOV, 
				matrix.y, matrix.z );
		D3DOBJ_SetMatrix( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER02], matrix.x, matrix.y, matrix.z );
		break;
		
	case WF2DMAP_WAY_LEFT:
		matrix.x		-= 	WFLBY_GADGET_CRACKER_DISX;
		flash_matrix.x	-=	WFLBY_GADGET_FLASH_DISX;
		flash_matrix.z	+=	WFLBY_GADGET_FLASH_Z;
		D3DOBJ_SetRota( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER], 
				FX_GET_ROTA_NUM((0-WFLBY_GADGET_CRACKER_ROT_ADD)), D3DOBJ_ROTA_WAY_Y );
		D3DOBJ_SetRota( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER01], 
				FX_GET_ROTA_NUM((0+WFLBY_GADGET_CRACKER_ROT_ADD)), D3DOBJ_ROTA_WAY_Y );
		D3DOBJ_SetRota( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER02], 
				FX_GET_ROTA_NUM(0), D3DOBJ_ROTA_WAY_Y );

		D3DOBJ_SetMatrix( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER], matrix.x, 
				matrix.y, matrix.z - WFLBY_GADGET_CRACKER_ADD_MOV );
		D3DOBJ_SetMatrix( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER01], matrix.x, 
				matrix.y, matrix.z + WFLBY_GADGET_CRACKER_ADD_MOV );
		D3DOBJ_SetMatrix( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER02], matrix.x, matrix.y, matrix.z );
		break;
		
	case WF2DMAP_WAY_RIGHT:
		matrix.x		+= 	WFLBY_GADGET_CRACKER_DISX;
		flash_matrix.x	+=	WFLBY_GADGET_FLASH_DISX;
		flash_matrix.z	+=	WFLBY_GADGET_FLASH_Z;
		D3DOBJ_SetRota( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER], 
				FX_GET_ROTA_NUM((180-WFLBY_GADGET_CRACKER_ROT_ADD)), D3DOBJ_ROTA_WAY_Y );
		D3DOBJ_SetRota( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER01], 
				FX_GET_ROTA_NUM((180+WFLBY_GADGET_CRACKER_ROT_ADD)), D3DOBJ_ROTA_WAY_Y );
		D3DOBJ_SetRota( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER02], 
				FX_GET_ROTA_NUM(180), D3DOBJ_ROTA_WAY_Y );

		D3DOBJ_SetMatrix( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER], matrix.x, 
				matrix.y, matrix.z + WFLBY_GADGET_CRACKER_ADD_MOV );
		D3DOBJ_SetMatrix( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER01], matrix.x, 
				matrix.y, matrix.z - WFLBY_GADGET_CRACKER_ADD_MOV );
		D3DOBJ_SetMatrix( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_CRACKER02], matrix.x, matrix.y, matrix.z );
		break;

	default:
		break;
	}

	// 設定
	D3DOBJ_SetMatrix( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_FLASH], flash_matrix.x, flash_matrix.y, flash_matrix.z );
}


// 風船１こ
static void WFLBY_GADGET_ANM_Init_Balloon00( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	WFLBY_GADGET_Balloon_InitUp( p_sys, p_wk, 1 );
}

// 風船２こ
static void WFLBY_GADGET_ANM_Init_Balloon01( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	WFLBY_GADGET_Balloon_InitUp( p_sys, p_wk, 2 );
}

// 風船３こ
static void WFLBY_GADGET_ANM_Init_Balloon02( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	WFLBY_GADGET_Balloon_InitUp( p_sys, p_wk, 3 );
}

// 紙ふぶき1周
static void WFLBY_GADGET_ANM_Init_Sparkle00( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	WFLBY_GADGET_Sparkle_Init( p_sys, p_wk, 1 );
}
// 紙ふぶき2周
static void WFLBY_GADGET_ANM_Init_Sparkle01( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	WFLBY_GADGET_Sparkle_Init( p_sys, p_wk, 2 );
}
// 紙ふぶき4周
static void WFLBY_GADGET_ANM_Init_Sparkle02( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	WFLBY_GADGET_Sparkle_Init( p_sys, p_wk, 4 );
}
// ベル
static void WFLBY_GADGET_ANM_Init_Bell00( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	int i;
	
	// オブジェ作成
	for( i=0; i<WFLBY_GADGET_ONPU_OBJWK_NUM; i++ ){
		WFLBY_GADGET_OBJ_SetRes( p_sys, p_wk, i, &sc_WFLBY_GADGET_RES[ sc_WFLBY_LEVEL_00ADDOBJ[i] ] );
	}

	// 動作パラメータ作成
	WFLBY_GADGET_ONPU_Cont_Init( &p_wk->mvwk, sc_WFLBY_GADGET_MOVE_ONPU_DATA_BELL00, 
			NELEMS(sc_WFLBY_GADGET_MOVE_ONPU_DATA_BELL00), 
			WFLBY_GADGET_MOVE_ONPU_BELL00_SYSNC, 
			WFLBY_GADGET_ONPU_MOVE_BELL,
			p_wk );
}
static void WFLBY_GADGET_ANM_Init_Bell01( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	int i;
	
	// オブジェ作成
	for( i=0; i<WFLBY_GADGET_ONPU_OBJWK_NUM; i++ ){
		WFLBY_GADGET_OBJ_SetRes( p_sys, p_wk, i, &sc_WFLBY_GADGET_RES[ sc_WFLBY_LEVEL_01ADDOBJ[i] ] );
	}

	// 動作パラメータ作成
	WFLBY_GADGET_ONPU_Cont_Init( &p_wk->mvwk, sc_WFLBY_GADGET_MOVE_ONPU_DATA_BELL01, 
			NELEMS(sc_WFLBY_GADGET_MOVE_ONPU_DATA_BELL01), 
			WFLBY_GADGET_MOVE_ONPU_BELL01_SYSNC, 
			WFLBY_GADGET_ONPU_MOVE_BELL,
			p_wk );
}
static void WFLBY_GADGET_ANM_Init_Bell02( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	int i;
	
	// オブジェ作成
	for( i=0; i<WFLBY_GADGET_ONPU_OBJWK_NUM; i++ ){
		WFLBY_GADGET_OBJ_SetRes( p_sys, p_wk, i, &sc_WFLBY_GADGET_RES[ sc_WFLBY_LEVEL_02ADDOBJ[i] ] );
	}

	// 動作パラメータ作成
	WFLBY_GADGET_ONPU_Cont_Init( &p_wk->mvwk, sc_WFLBY_GADGET_MOVE_ONPU_DATA_BELL02, 
			NELEMS(sc_WFLBY_GADGET_MOVE_ONPU_DATA_BELL02), 
			WFLBY_GADGET_MOVE_ONPU_BELL02_SYSNC, 
			WFLBY_GADGET_ONPU_MOVE_BELL,
			p_wk );
}

// ドラム
static void WFLBY_GADGET_ANM_Init_Drum00( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	int i;
	
	// オブジェ作成
	for( i=0; i<WFLBY_GADGET_ONPU_OBJWK_NUM; i++ ){
		WFLBY_GADGET_OBJ_SetRes( p_sys, p_wk, i, &sc_WFLBY_GADGET_RES[ sc_WFLBY_LEVEL_00ADDOBJ[i] ] );
	}

	// 動作パラメータ作成
	WFLBY_GADGET_ONPU_Cont_Init( &p_wk->mvwk, sc_WFLBY_GADGET_MOVE_ONPU_DATA_DRUM00, 
			NELEMS(sc_WFLBY_GADGET_MOVE_ONPU_DATA_DRUM00), 
			WFLBY_GADGET_MOVE_ONPU_DRUM00_SYSNC, 
			WFLBY_GADGET_ONPU_MOVE_DRUM,
			p_wk );
}
static void WFLBY_GADGET_ANM_Init_Drum01( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	int i;
	
	// オブジェ作成
	for( i=0; i<WFLBY_GADGET_ONPU_OBJWK_NUM; i++ ){
		WFLBY_GADGET_OBJ_SetRes( p_sys, p_wk, i, &sc_WFLBY_GADGET_RES[ sc_WFLBY_LEVEL_01ADDOBJ[i] ] );
	}

	// 動作パラメータ作成
	WFLBY_GADGET_ONPU_Cont_Init( &p_wk->mvwk, sc_WFLBY_GADGET_MOVE_ONPU_DATA_DRUM01, 
			NELEMS(sc_WFLBY_GADGET_MOVE_ONPU_DATA_DRUM01), 
			WFLBY_GADGET_MOVE_ONPU_DRUM01_SYSNC, 
			WFLBY_GADGET_ONPU_MOVE_DRUM,
			p_wk );
}
static void WFLBY_GADGET_ANM_Init_Drum02( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	int i;
	
	// オブジェ作成
	for( i=0; i<WFLBY_GADGET_ONPU_OBJWK_NUM; i++ ){
		WFLBY_GADGET_OBJ_SetRes( p_sys, p_wk, i, &sc_WFLBY_GADGET_RES[ sc_WFLBY_LEVEL_02ADDOBJ[i] ] );
	}

	// 動作パラメータ作成
	WFLBY_GADGET_ONPU_Cont_Init( &p_wk->mvwk, sc_WFLBY_GADGET_MOVE_ONPU_DATA_DRUM02, 
			NELEMS(sc_WFLBY_GADGET_MOVE_ONPU_DATA_DRUM02), 
			WFLBY_GADGET_MOVE_ONPU_DRUM02_SYSNC, 
			WFLBY_GADGET_ONPU_MOVE_DRUM,
			p_wk );
}
// シンバル
static void WFLBY_GADGET_ANM_Init_Cymbals00( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	int i;
	VecFx32 matrix;
	
	// オブジェ作成
	for( i=0; i<WFLBY_GADGET_ONPU_OBJWK_NUM; i++ ){
		WFLBY_GADGET_OBJ_SetRes( p_sys, p_wk, i, &sc_WFLBY_GADGET_RES[ sc_WFLBY_LEVEL_00ADDOBJ[i] ] );
	}

	// 動作パラメータ作成
	WFLBY_GADGET_ONPU_Cont_Init( &p_wk->mvwk, sc_WFLBY_GADGET_MOVE_ONPU_DATA_CYMBALS00, 
			NELEMS(sc_WFLBY_GADGET_MOVE_ONPU_DATA_CYMBALS00), 
			WFLBY_GADGET_MOVE_ONPU_CYMBALS00_SYSNC, 
			WFLBY_GADGET_ONPU_MOVE_CYMBALS,
			p_wk );

	// ｲﾅｽﾞﾏ初期化
	WFLBY_GADGET_Inazuma_Init( p_sys, p_wk );
}
static void WFLBY_GADGET_ANM_Init_Cymbals01( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	int i;
	VecFx32 matrix;
	
	// オブジェ作成
	for( i=0; i<WFLBY_GADGET_ONPU_OBJWK_NUM; i++ ){
		WFLBY_GADGET_OBJ_SetRes( p_sys, p_wk, i, &sc_WFLBY_GADGET_RES[ sc_WFLBY_LEVEL_01ADDOBJ[i] ] );
	}

	// 動作パラメータ作成
	WFLBY_GADGET_ONPU_Cont_Init( &p_wk->mvwk, sc_WFLBY_GADGET_MOVE_ONPU_DATA_CYMBALS01, 
			NELEMS(sc_WFLBY_GADGET_MOVE_ONPU_DATA_CYMBALS01), 
			WFLBY_GADGET_MOVE_ONPU_CYMBALS01_SYSNC, 
			WFLBY_GADGET_ONPU_MOVE_CYMBALS,
			p_wk );


	// ｲﾅｽﾞﾏ初期化
	WFLBY_GADGET_Inazuma_Init( p_sys, p_wk );
}
static void WFLBY_GADGET_ANM_Init_Cymbals02( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	int i;
	
	// オブジェ作成
	for( i=0; i<WFLBY_GADGET_ONPU_OBJWK_NUM; i++ ){
		WFLBY_GADGET_OBJ_SetRes( p_sys, p_wk, i, &sc_WFLBY_GADGET_RES[ sc_WFLBY_LEVEL_02ADDOBJ[i] ] );
	}

	// 動作パラメータ作成
	WFLBY_GADGET_ONPU_Cont_Init( &p_wk->mvwk, sc_WFLBY_GADGET_MOVE_ONPU_DATA_CYMBALS02, 
			NELEMS(sc_WFLBY_GADGET_MOVE_ONPU_DATA_CYMBALS02), 
			WFLBY_GADGET_MOVE_ONPU_CYMBALS02_SYSNC, 
			WFLBY_GADGET_ONPU_MOVE_CYMBALS,
			p_wk );

	// ｲﾅｽﾞﾏ初期化
	WFLBY_GADGET_Inazuma_Init( p_sys, p_wk );
}




//----------------------------------------------------------------------------
/**
 *	@brief	アニメのメイン
 *
 *	@param	p_sys	システムワーク
 *	@param	p_wk	ワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
// 2つのアニメを動かす
static BOOL WFLBY_GADGET_ANM_Main_AnmTwo( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	int i;
	BOOL ret = TRUE;
	BOOL result;
	
	// 2つのアニメを動かす
	for( i=0; i<2; i++ ){
		// アニメがあるかチェック
		if( p_wk->cp_objres[0]->anm[i] != WFLBY_GADGET_ANM_NONE ){
			result = WFLBY_GADGET_OBJ_NoLoopAnm( p_sys, p_wk, 0, i );
			if( result == FALSE ){
				ret = FALSE;
			}
		}
	}

	return ret;
}
// 1つのアニメを動かす
static BOOL WFLBY_GADGET_ANM_Main_AnmOne( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	GF_ASSERT( p_wk->cp_objres[0]->anm[0] != WFLBY_GADGET_ANM_NONE );
	return WFLBY_GADGET_OBJ_NoLoopAnm( p_sys, p_wk, 0, 0 );
}
// リップル
static BOOL WFLBY_GADGET_ANM_Main_Rippru00( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	int i;
	BOOL result;
	u32 move_num;
	u32 objidx;

	if( (p_wk->count+1) <= WFLBY_GADGET_RIPPLE_ROOP_SYNC ){
		p_wk->count ++;
	}
	
	// オブジェ発射管理部分
	// 合計６かいリップルの再生を開始する
	move_num = (p_wk->count * WFLBY_GADGET_RIPPLE_ROOP_START) / WFLBY_GADGET_RIPPLE_ROOP_SYNC;
	for( i=p_wk->mvwk.ripple.move_num; i<move_num; i++ ){
		objidx = i % WFLBY_GADGET_RIPPLE_NUM;
		if( objidx < p_wk->mvwk.ripple.ripple_num ){	// そのオブジェは生成されているかチェック
			D3DOBJ_SetDraw( &p_wk->obj[objidx], TRUE );
		}
	}
	p_wk->mvwk.ripple.move_num = move_num;

	// アニメ再生部分
	result = TRUE;
	for( i=0; i<p_wk->mvwk.ripple.ripple_num; i++ ){
		if( D3DOBJ_GetDraw( &p_wk->obj[i] ) == TRUE ){
			result = WFLBY_GADGET_OBJ_NoLoopAnm( p_sys, p_wk, i, 0 );
			if( result ){
				D3DOBJ_SetDraw( &p_wk->obj[i], FALSE );
				WFLBY_GADGET_OBJ_SetFrame( p_sys, p_wk, i, 0, 0 );
			}
		}
	}


	// 終了チェック
	// 全リップルを発射していて、全アニメが終わっていたら終了
	if( (p_wk->mvwk.ripple.move_num >= WFLBY_GADGET_RIPPLE_ROOP_START) &&
		(result == TRUE) ){
		return TRUE;
	}
	return FALSE;
}
// しぐなる
static BOOL WFLBY_GADGET_ANM_Main_Signal00( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	return WFLBY_GADGET_Signal_Main( p_sys, p_wk, 0 );
}
static BOOL WFLBY_GADGET_ANM_Main_Signal01( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	return WFLBY_GADGET_Signal_Main( p_sys, p_wk, 1 );
}
static BOOL WFLBY_GADGET_ANM_Main_Signal02( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	return WFLBY_GADGET_Signal_Main( p_sys, p_wk, 2 );
}
// スウィング
static BOOL WFLBY_GADGET_ANM_Main_Swing00( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	u32 move_num;
	int	i;
	BOOL result;

	move_num = (p_wk->count * WFLBY_GADGET_SWING_OBJ_NUM) / WFLBY_GADGET_SWING_ANM_SYNC;

	if( move_num != p_wk->mvwk.swing.lastnum ){
		p_wk->mvwk.swing.lastnum = move_num;

		if( p_wk->mvwk.swing.objnum > move_num ){

			// 今まで動いてたやつを非表示
			if( move_num > 0 ){
				D3DOBJ_SetDraw( &p_wk->obj[move_num-1], FALSE );
			}
			// 動作開始
			D3DOBJ_SetDraw( &p_wk->obj[move_num], TRUE );
		}
	}

	if( p_wk->count < WFLBY_GADGET_SWING_ANM_SYNC ){
		p_wk->count ++;
	}

	// 表示中のオブジェのアニメを進める
	result = FALSE;
	for( i=0; i<p_wk->mvwk.swing.objnum; i++ ){
		if( D3DOBJ_GetDraw( &p_wk->obj[i] ) ){
			if( i==p_wk->mvwk.swing.objnum-1 ){
				result = WFLBY_GADGET_OBJ_NoLoopAnm( p_sys, p_wk, i, 0 );
			}else{
				WFLBY_GADGET_OBJ_LoopAnm( p_sys, p_wk, i, 0 );
			}
		}
	}

	return result;
}

// クラッカー
static BOOL WFLBY_GADGET_ANM_Main_Cracker00( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	BOOL result;
	BOOL ret;
	int i;
	GF_ASSERT( p_wk->cp_objres[0]->anm[0] != WFLBY_GADGET_ANM_NONE );

	// 光を出す
	// 光が出きったらクラッカーもなる
	if( p_wk->mvwk.cracker.wait > 0 ){
		p_wk->mvwk.cracker.wait --;
		D3DOBJ_SetDraw( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_FLASH], FALSE );			
	}else{
		if( p_wk->count < WFLBY_GADGET_CRACKEROBJ_CRACKER_MAX ){
			if( WFLBY_GADGET_OBJ_CheckRes( p_wk, p_wk->count+WFLBY_GADGET_CRACKEROBJ_CRACKER ) == TRUE ){
				D3DOBJ_SetDraw( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_FLASH], TRUE );
				result = WFLBY_GADGET_OBJ_NoLoopAnm( p_sys, p_wk, WFLBY_GADGET_CRACKEROBJ_FLASH, 0 );
				if( result == TRUE ){
					p_wk->count ++;
					p_wk->mvwk.cracker.wait = WFLBY_GADGET_CRACKER_START_WAIT;
					if( p_wk->count < WFLBY_GADGET_CRACKEROBJ_CRACKER_MAX ){
						// アニメフレームを元に戻す
						WFLBY_GADGET_OBJ_SetFrame( p_sys, p_wk, WFLBY_GADGET_CRACKEROBJ_FLASH, 0, 0 );
					}else{
						D3DOBJ_SetDraw( &p_wk->obj[WFLBY_GADGET_CRACKEROBJ_FLASH], FALSE );			
					}
				}
			}
		}
	}
	// 全部動かす
	ret = TRUE;
	for( i=WFLBY_GADGET_CRACKEROBJ_CRACKER; i<=WFLBY_GADGET_CRACKEROBJ_CRACKER02; i++ ){

		// そのオブジェがあるかチェック
		if( WFLBY_GADGET_OBJ_CheckRes( p_wk, i ) == TRUE ){
			//  発射タイミングチェック
			if( p_wk->count > (i-WFLBY_GADGET_CRACKEROBJ_CRACKER) ){
				D3DOBJ_SetDraw( &p_wk->obj[i], TRUE );
				result = WFLBY_GADGET_OBJ_NoLoopAnm_Sp( p_sys, p_wk, i, 0, FX32_CONST( 2.50f ) );
				if( result == FALSE ){

					// 動作中
					ret = FALSE;
				}else{
					s32 alpha;
					s32 alpha_idx;

					alpha_idx = i-WFLBY_GADGET_CRACKEROBJ_CRACKER;

					// アニメが終了したらアルファをOFFにする

					// アルファ変更
					if( p_wk->mvwk.cracker.alpha_count[alpha_idx] == 0 ){

						// 終了
						D3DOBJ_SetDraw( &p_wk->obj[i], FALSE );

						// アルファを元に戻す
						NNS_G3dMdlSetMdlAlphaAll( p_sys->mdl[ p_wk->cp_objres[i]->mdl ].pModel, 31 );

					}else{
						// アニメが終わったらアルファフェードさせる
						p_wk->mvwk.cracker.alpha_count[alpha_idx]	--;

						alpha = (p_wk->mvwk.cracker.alpha_count[alpha_idx] * WFLBY_GADGET_CRACKER_ALPHA_OUT_S) / WFLBY_GADGET_CRACKER_ALPHA_OUT_SYNC;

						// アルファを設定
						NNS_G3dMdlSetMdlAlphaAll( p_sys->mdl[p_wk->cp_objres[i]->mdl].pModel, alpha );

						// 動作中
						ret = FALSE;
					}
				}
			}else{
				ret = FALSE;
			}
		}
	}

	return ret;
}
// 風船
static BOOL WFLBY_GADGET_ANM_Main_Balloon00( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	BOOL result;
	
	switch( p_wk->seq ){
	case WFLBY_GADGET_BALLOON_SEQ_UP:		// 上り
		result = WFLBY_GADGET_Balloon_MainUp( p_sys, p_wk );
		if( result ){
			p_wk->seq = WFLBY_GADGET_BALLOON_SEQ_DOWN;
			WFLBY_GADGET_Balloon_InitDown( p_sys, p_wk );
		}
		break;
		
	case WFLBY_GADGET_BALLOON_SEQ_DOWN:		// くだり
		result = WFLBY_GADGET_Balloon_MainDown( p_sys, p_wk );
		if( result ){
			return TRUE;
		}
		break;
	}

	return FALSE;
}

static BOOL WFLBY_GADGET_ANM_Main_Sparkle00( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	BOOL result;
	u32 move_num;
	int i;

	
	// カウントと動作開始処理は、ループ数が最大値以下のときに行う
	if( (p_wk->mvwk.sparkle.roop_count < p_wk->mvwk.sparkle.roop_num) ){
		
		// カウント処理
		p_wk->mvwk.sparkle.count ++;
		if( p_wk->mvwk.sparkle.count >= WFLBY_GADGET_SPARKLE_1ROOP_SYNC ){
			p_wk->mvwk.sparkle.count = 0;
			p_wk->mvwk.sparkle.roop_count++;
		}

		
		// 動作開始設定
		// 現在のカウント値での動作数になるまで、オブジェのアニメを開始させる
		move_num = (p_wk->mvwk.sparkle.count * WFLBY_GADGET_SPARKLE_1ROOP_OBJNUM) / WFLBY_GADGET_SPARKLE_1ROOP_SYNC;
		move_num += p_wk->mvwk.sparkle.roop_count * WFLBY_GADGET_SPARKLE_1ROOP_OBJNUM;
		for( i=p_wk->mvwk.sparkle.buff_num; i<move_num; i++ ){
			D3DOBJ_SetDraw( &p_wk->obj[i], TRUE );	// 描画開始
		}
		p_wk->mvwk.sparkle.buff_num	= move_num; 
	}


	// 動作
	for( i=0; i<p_wk->mvwk.sparkle.buff_num; i++ ){
		if( D3DOBJ_GetDraw( &p_wk->obj[i] ) == TRUE ){
			result = WFLBY_GADGET_Sparkle_OneObj_Main( p_sys, p_wk, i );
			// アニメ終了後非表示
			if( result == TRUE ){
				D3DOBJ_SetDraw( &p_wk->obj[i], FALSE );
			}
		}
	}

	// 終了チェック
	// オブジェが最後まで出て、アニメも終わったら終了
	if( (p_wk->mvwk.sparkle.roop_count == p_wk->mvwk.sparkle.roop_num) &&
		(result == TRUE) ){
		return TRUE;
	}
	return FALSE;
}

// 音符全般
static BOOL WFLBY_GADGET_ANM_Main_Onpu00( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	u32 result;
	result =  WFLBY_GADGET_ONPU_Cont_Main( &p_wk->mvwk, p_wk->count );
	p_wk->count ++;
	if( result == WFLBY_GADGET_ONPU_MAIN_ALLEND ){
		return TRUE;
	}
	return FALSE;
}
// シンバルメイン
static BOOL WFLBY_GADGET_ANM_Main_Cymbals( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	u32 result;
	int i;
	
	result =  WFLBY_GADGET_ONPU_Cont_Main( &p_wk->mvwk, p_wk->count );
	p_wk->count ++;

	// アニメ開始チェック
	if( result == WFLBY_GADGET_ONPU_MAIN_START ){
		WFLBY_GADGET_Inazuma_Start( p_sys, p_wk );
	}

	// ｲﾅｽﾞﾏアニメメイン
	WFLBY_GADGET_Inazuma_Main( p_sys, p_wk );

	if( result == WFLBY_GADGET_ONPU_MAIN_ALLEND ){
		return TRUE;
	}
	return FALSE;
}


//----------------------------------------------------------------------------
/**
 *	@brief	アニメの描画
 *
 *	@param	p_sys		システムワーク
 *	@param	p_wk		ワーク
 *
 *	@retval	TRUE	成功
 *	@retval	FALSE	失敗
 */	
//-----------------------------------------------------------------------------
static BOOL WFLBY_GADGET_ANM_Draw_Obj( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	int i;

	// リソースはインデックス０からつめて入れられている
	if( WFLBY_3DOBJCONT_GetCullingFlag( p_wk->p_person ) == TRUE ){
		// 矩形内にいないなら、出さない
		return FALSE;
	}

	for( i=0; i<WFLBY_GADGET_OBJ_MAX; i++ ){
		if( WFLBY_GADGET_OBJ_CheckRes( p_wk, i ) == TRUE ){

			WFLBY_GADGET_OBJ_DrawRes( p_sys, p_wk, i );
		}
	}

	return TRUE;
}






//----------------------------------------------------------------------------
/**
 *	@brief	風船動作	上昇
 *
 *	@param	p_sys			システムワーク
 *	@param	p_wk			ワーク
 *	@param	balloon_num		風船の数
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_Balloon_InitUp( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ*  p_wk, u32 balloon_num )
{
	VecFx32		matrix;

	// 主人公かチェック
	{
		if( p_wk->p_person == WFLBY_3DOBJCONT_GetPlayer( p_sys->p_objcont ) ){
			// カメラをはがす
			WFLBY_CAMERA_ResetTargetPerson( p_sys->p_camera );
		}
	}

	// 上に屋根状のものがないかチェック
	{
		WF2DMAP_POS pos;
		u32 ev_data;
		pos		= WFLBY_3DOBJCONT_GetWkPos( p_wk->p_person );
		ev_data	= WFLBY_MAPCONT_EventGet( p_sys->p_mapcont, WF2DMAP_POS2GRID(pos.x), WF2DMAP_POS2GRID(pos.y) );
		if( ev_data == WFLBY_MAPEVID_EV_ROOF_MAT ){
			p_wk->mvwk.balloon.roof_flag = TRUE;
		}else{
			p_wk->mvwk.balloon.roof_flag = FALSE;
		}
	}

	// モデルを読み込む
	switch(balloon_num){
	case 1:
		WFLBY_GADGET_OBJ_SetRes( p_sys, p_wk, 0, &sc_WFLBY_GADGET_RES[WFLBY_GADGET_OBJ_BALLOON00] );
		WFLBY_GADGET_Balloon_SetAnm( p_sys, p_wk, 0, WFLBY_GADGET_BALLOON_ANM_CENTER );
		break;
	case 2:
		WFLBY_GADGET_OBJ_SetRes( p_sys, p_wk, 0, &sc_WFLBY_GADGET_RES[WFLBY_GADGET_OBJ_BALLOON00] );
		WFLBY_GADGET_OBJ_SetRes( p_sys, p_wk, 1, &sc_WFLBY_GADGET_RES[WFLBY_GADGET_OBJ_BALLOON01] );
		WFLBY_GADGET_Balloon_SetAnm( p_sys, p_wk, 0, WFLBY_GADGET_BALLOON_ANM_LEFT );
		WFLBY_GADGET_Balloon_SetAnm( p_sys, p_wk, 1, WFLBY_GADGET_BALLOON_ANM_RIGHT );
		break;

	default:
	case 3:
		WFLBY_GADGET_OBJ_SetRes( p_sys, p_wk, 0, &sc_WFLBY_GADGET_RES[WFLBY_GADGET_OBJ_BALLOON00] );
		WFLBY_GADGET_OBJ_SetRes( p_sys, p_wk, 1, &sc_WFLBY_GADGET_RES[WFLBY_GADGET_OBJ_BALLOON01] );
		WFLBY_GADGET_OBJ_SetRes( p_sys, p_wk, 2, &sc_WFLBY_GADGET_RES[WFLBY_GADGET_OBJ_BALLOON02] );
		WFLBY_GADGET_Balloon_SetAnm( p_sys, p_wk, 0, WFLBY_GADGET_BALLOON_ANM_CENTER );
		WFLBY_GADGET_Balloon_SetAnm( p_sys, p_wk, 1, WFLBY_GADGET_BALLOON_ANM_LEFT );
		WFLBY_GADGET_Balloon_SetAnm( p_sys, p_wk, 2, WFLBY_GADGET_BALLOON_ANM_RIGHT );
		break;

	}

	//
	p_wk->mvwk.balloon.balloon_num	= balloon_num;

	// 主人公動作更新なし
	WFLBY_3DOBJCONT_DRAW_SetUpdata( p_wk->p_person, FALSE );	
	
	// 屋根あるなしで動作を変える
	// ないー＞通常動作
	// あるー＞すぐに割れる
	if( p_wk->mvwk.balloon.roof_flag == FALSE ){
		WFLBY_GADGET_Balloon_InitUp_Normal( p_sys, p_wk );
	}else{
		WFLBY_GADGET_Balloon_InitUp_Roof( p_sys, p_wk );
	}
	
}
// 通常の初期化
static void WFLBY_GADGET_Balloon_InitUp_Normal( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ*  p_wk )
{
	VecFx32 matrix;

	// 座標を取得
	WFLBY_3DOBJCONT_DRAW_Get3DMatrix( p_wk->p_person, &matrix );
	
	// 主人公動作初期化
	WFLBY_GADGET_MV_Straight_Init( &p_wk->mvwk.balloon.straightobj,
			matrix.x, matrix.x, 
			matrix.y, 
			matrix.y + FX32_CONST(sc_WFLBY_GADGET_BALLOON_OBJ_UPDIS[ p_wk->mvwk.balloon.balloon_num-1 ]),
			matrix.z, matrix.z, 
			WFLBY_GADGET_BALLOON_OBJ_UP_SYNC );

	WFLBY_GADGET_MV_SinCurve_Init( &p_wk->mvwk.balloon.sincurveobj, 0, 
			WFLBY_GADGET_BALLOON_OBJ_UPROTSPEED, 
			FX32_CONST( WFLBY_GADGET_BALLOON_OBJ_UPSIDEDIS ) );


	// 座標を設定
	WFLBY_GADGET_Balloon_SetObjPos( p_wk );

	p_wk->mvwk.balloon.count = 0;
}
// 上に屋根があるときの初期化
static void WFLBY_GADGET_Balloon_InitUp_Roof( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ*  p_wk )
{
	VecFx32		matrix;

	// 座標を取得
	WFLBY_3DOBJCONT_DRAW_Get3DMatrix( p_wk->p_person, &matrix );
	
	// 主人公動作初期化
	WFLBY_GADGET_MV_Straight_Init( &p_wk->mvwk.balloon.straightobj,
			matrix.x, matrix.x, 
			matrix.y, 
			matrix.y + FX32_CONST(WFLBY_GADGET_BALLOON_OBJ_ROOF_UPDIS),
			matrix.z, matrix.z, 
			WFLBY_GADGET_BALLOON_OBJ_ROOF_UP_SYNC );

	WFLBY_GADGET_MV_SinCurve_Init( &p_wk->mvwk.balloon.sincurveobj, 0, 
			WFLBY_GADGET_BALLOON_OBJ_ROOF_UPROTSPEED, 
			FX32_CONST( WFLBY_GADGET_BALLOON_OBJ_ROOF_UPSIDEDIS ) );


	// 座標を設定
	WFLBY_GADGET_Balloon_SetObjPos_Roof( p_wk );

	p_wk->mvwk.balloon.count = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	風船動作	メイン
 *
 *	@param	p_wk	ワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_GADGET_Balloon_MainUp( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ*  p_wk )
{
	BOOL ret;
	
	if( p_wk->mvwk.balloon.roof_flag == FALSE ){
		ret = WFLBY_GADGET_Balloon_MainUp_Normal( p_sys, p_wk );
	}else{
		ret = WFLBY_GADGET_Balloon_MainUp_Roof( p_sys, p_wk );
	}
	return ret;
}
// 通常動作
static BOOL WFLBY_GADGET_Balloon_MainUp_Normal( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ*  p_wk )
{
	BOOL		ret;
	VecFx32		matrix;
	fx32		carve_x;
	int			i;
	
	ret = WFLBY_GADGET_MV_Straight_Main( &p_wk->mvwk.balloon.straightobj, 
			p_wk->mvwk.balloon.count );
	WFLBY_GADGET_MV_SinCurve_Main( &p_wk->mvwk.balloon.sincurveobj );
	p_wk->mvwk.balloon.count ++;
	
	//  値を設定
	WFLBY_3DOBJCONT_DRAW_Get3DMatrix( p_wk->p_person, &matrix );
	WFLBY_GADGET_MV_Straight_GetNum( &p_wk->mvwk.balloon.straightobj, &matrix.x, &matrix.y, &matrix.z );
	WFLBY_GADGET_MV_SinCurve_GetNum( &p_wk->mvwk.balloon.sincurveobj, &carve_x );

	if( ret == FALSE ){
		matrix.x += carve_x;
	}

	WFLBY_3DOBJCONT_DRAW_Set3DMatrix( p_wk->p_person, &matrix );


	// 座標を設定
	WFLBY_GADGET_Balloon_SetObjPos( p_wk );

	// バルーンアニメ
	for( i=0;  i<p_wk->mvwk.balloon.balloon_num; i++ ){

		WFLBY_GADGET_Balloon_Anm( p_sys, p_wk, i );
	}

	// キーで方向を変える
	if( p_wk->p_person == WFLBY_3DOBJCONT_GetPlayer( p_sys->p_objcont ) ){
		u32 way = WF2DMAP_WAY_NUM;
		if( sys.trg & PAD_KEY_UP ){
			way = WF2DMAP_WAY_UP;
		}
		if( sys.trg & PAD_KEY_DOWN ){
			way = WF2DMAP_WAY_DOWN;
		}
		if( sys.trg & PAD_KEY_RIGHT ){
			way = WF2DMAP_WAY_RIGHT;
		}
		if( sys.trg & PAD_KEY_LEFT ){
			way = WF2DMAP_WAY_LEFT;
		}
		if( way != WF2DMAP_WAY_NUM ){
			WFLBY_3DOBJCONT_DRAW_SetWay( p_wk->p_person, way );
			WFLBY_3DOBJCONT_SetWkPosAndWay( p_wk->p_person, 
					WFLBY_3DOBJCONT_GetWkPos( p_wk->p_person ), way );
		}
	}

	return ret;
}
// 屋根があるときの動作
static BOOL WFLBY_GADGET_Balloon_MainUp_Roof( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ*  p_wk )
{
	BOOL		ret;
	VecFx32		matrix;
	int			i;
	
	ret = WFLBY_GADGET_MV_Straight_Main( &p_wk->mvwk.balloon.straightobj, 
			p_wk->mvwk.balloon.count );
	WFLBY_GADGET_MV_SinCurve_Main( &p_wk->mvwk.balloon.sincurveobj );
	p_wk->mvwk.balloon.count ++;
	
	//  値を設定
	WFLBY_3DOBJCONT_DRAW_Get3DMatrix( p_wk->p_person, &matrix );
	WFLBY_GADGET_MV_Straight_GetNum( &p_wk->mvwk.balloon.straightobj, &matrix.x, &matrix.y, &matrix.z );

	WFLBY_3DOBJCONT_DRAW_Set3DMatrix( p_wk->p_person, &matrix );


	// 座標を設定
	WFLBY_GADGET_Balloon_SetObjPos_Roof( p_wk );

	// バルーンアニメ
	for( i=0;  i<p_wk->mvwk.balloon.balloon_num; i++ ){

		WFLBY_GADGET_Balloon_Anm( p_sys, p_wk, i );
	}

	return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief	下降動作	初期化
 *
 *	@param	p_sys		システムワーク
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_Balloon_InitDown( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ*  p_wk )
{
	if( p_wk->mvwk.balloon.roof_flag == FALSE ){
		WFLBY_GADGET_Balloon_InitDown_Normal( p_sys, p_wk );
	}else{
		WFLBY_GADGET_Balloon_InitDown_Roof( p_sys, p_wk );
	}
}
// 通常動作
static void WFLBY_GADGET_Balloon_InitDown_Normal( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ*  p_wk )
{
	VecFx32		matrix;
	VecFx32		move_matrix;
	int i;
	u32 anmidx;

	// 座標を取得
	WFLBY_3DOBJCONT_DRAW_Get3DMatrix( p_wk->p_person, &matrix );
	
	// 主人公動作初期化
	WFLBY_GADGET_MV_Straight_Init( &p_wk->mvwk.balloon.straightobj,
			matrix.x, matrix.x, 
			matrix.y, 
			matrix.y - FX32_CONST(sc_WFLBY_GADGET_BALLOON_OBJ_UPDIS[ p_wk->mvwk.balloon.balloon_num-1 ]),
			matrix.z, matrix.z, 
			WFLBY_GADGET_BALLOON_OBJ_DOWN_SYNC );
	p_wk->mvwk.balloon.count = 0;
	p_wk->mvwk.balloon.bl_seq	= 0;

	// 風船の動き設定
	for( i=0; i<p_wk->mvwk.balloon.balloon_num; i++ ){
		D3DOBJ_GetMatrix( &p_wk->obj[i], &matrix.x, &matrix.y, &matrix.z );

		anmidx = p_wk->mvwk.balloon.anmidx[i];
		switch( anmidx ){
		case WFLBY_GADGET_BALLOON_ANM_CENTER:
			move_matrix.x = matrix.x;
			move_matrix.y = matrix.y + WFLBY_GADGET_BALLOON_OBJ_MOVE_Y;
			move_matrix.z = matrix.z - WFLBY_GADGET_BALLOON_OBJ_MOVE_Y;
			break;
			
		case WFLBY_GADGET_BALLOON_ANM_RIGHT:
			move_matrix.x = matrix.x + WFLBY_GADGET_BALLOON_OBJ_MOVE_X;
			move_matrix.y = matrix.y + WFLBY_GADGET_BALLOON_OBJ_MOVE_Y;
			move_matrix.z = matrix.z;
			break;
			
		case WFLBY_GADGET_BALLOON_ANM_LEFT:
			move_matrix.x = matrix.x - WFLBY_GADGET_BALLOON_OBJ_MOVE_X;
			move_matrix.y = matrix.y + WFLBY_GADGET_BALLOON_OBJ_MOVE_Y;
			move_matrix.z = matrix.z;
			break;
		}
		
		WFLBY_GADGET_MV_Straight_Init( &p_wk->mvwk.balloon.straight[i],
				matrix.x, move_matrix.x, 
				matrix.y, move_matrix.y,
				matrix.z, move_matrix.z, 
				WFLBY_GADGET_BALLOON_OBJ_WAIT );

		// ゆれアニメ
		WFLBY_GADGET_MV_SinCurve_Init( &p_wk->mvwk.balloon.sincurve[i], 0, 
				WFLBY_GADGET_BALLOON_OBJ_ROT_S, 
				WFLBY_GADGET_BALLOON_OBJ_ROT_DIS );

		// アニメ変更
		WFLBY_GADGET_Balloon_SetAnm( p_sys, p_wk, i, WFLBY_GADGET_BALLOON_ANM_OTHER );
	}

	p_wk->mvwk.balloon.balloon_fly_count = 0;
	
}
// 屋根あり動作
static void WFLBY_GADGET_Balloon_InitDown_Roof( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ*  p_wk )
{
	VecFx32		matrix;
	VecFx32		move_matrix;

	// 座標を取得
	WFLBY_3DOBJCONT_DRAW_Get3DMatrix( p_wk->p_person, &matrix );
	
	// 主人公動作初期化
	WFLBY_GADGET_MV_Straight_Init( &p_wk->mvwk.balloon.straightobj,
			matrix.x, matrix.x, 
			matrix.y, 
			matrix.y - FX32_CONST(WFLBY_GADGET_BALLOON_OBJ_ROOF_UPDIS),
			matrix.z, matrix.z, 
			WFLBY_GADGET_BALLOON_OBJ_ROOF_DOWN_SYNC );
	p_wk->mvwk.balloon.count = 0;
	p_wk->mvwk.balloon.bl_seq	= 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	下降動作	メイン
 *
 *	@param	p_wk		ワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_GADGET_Balloon_MainDown( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ*  p_wk )
{
	BOOL ret;
	
	if( p_wk->mvwk.balloon.roof_flag == FALSE ){
		ret = WFLBY_GADGET_Balloon_MainDown_Normal( p_sys, p_wk );
	}else{
		ret = WFLBY_GADGET_Balloon_MainDown_Roof( p_sys, p_wk );
	}
	return ret;
}
// 通常動作
static BOOL WFLBY_GADGET_Balloon_MainDown_Normal( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ*  p_wk )
{
	int i;

	switch( p_wk->mvwk.balloon.bl_seq ){
	case WFLBY_GADGET_BALLOON_DOWNSEQ_BATA_WAIT:		// バタバタ開始まち
		p_wk->mvwk.balloon.count ++;
		if( p_wk->mvwk.balloon.count >= WFLBY_GADGET_BALLOON_OBJ_BATA_SWAIT ){
			p_wk->mvwk.balloon.bl_seq = WFLBY_GADGET_BALLOON_DOWNSEQ_BATA_START;
		}
		break;
		
	case WFLBY_GADGET_BALLOON_DOWNSEQ_BATA_START:
		WFLBY_3DOBJCONT_DRAW_SetAnmBata( p_wk->p_person, TRUE );
		p_wk->mvwk.balloon.count = WFLBY_GADGET_BALLOON_OBJ_BATA_WAIT;

		p_wk->mvwk.balloon.bl_seq = WFLBY_GADGET_BALLOON_DOWNSEQ_BATA_MAIN;
		break;
		
	case WFLBY_GADGET_BALLOON_DOWNSEQ_BATA_MAIN:
		p_wk->mvwk.balloon.count--;
		if( p_wk->mvwk.balloon.count <= 0 ){
			WFLBY_3DOBJCONT_DRAW_SetAnmBata( p_wk->p_person, FALSE );
			p_wk->mvwk.balloon.bl_seq = WFLBY_GADGET_BALLOON_DOWNSEQ_DOWN;
			p_wk->mvwk.balloon.count = 0;

			// 向いている方向を向かせる
			{
				u32 way;

				way = WFLBY_3DOBJCONT_GetWkObjData( p_wk->p_person, WF2DMAP_OBJPM_WAY );
				WFLBY_3DOBJCONT_DRAW_SetWay( p_wk->p_person, way );
			}
		}
		break;
		
	case WFLBY_GADGET_BALLOON_DOWNSEQ_DOWN:
		{
			BOOL		result;
			VecFx32		matrix;
			
			result = WFLBY_GADGET_MV_Straight_Main( &p_wk->mvwk.balloon.straightobj, 
					p_wk->mvwk.balloon.count );
			p_wk->mvwk.balloon.count ++;
			
			//  値を設定
			WFLBY_3DOBJCONT_DRAW_Get3DMatrix( p_wk->p_person, &matrix );
			WFLBY_GADGET_MV_Straight_GetNum( &p_wk->mvwk.balloon.straightobj, &matrix.x, &matrix.y, &matrix.z );
			WFLBY_3DOBJCONT_DRAW_Set3DMatrix( p_wk->p_person, &matrix );

			if( result == TRUE ){
				p_wk->mvwk.balloon.bl_seq		= WFLBY_GADGET_BALLOON_DOWNSEQ_DON_MAIN;
				p_wk->mvwk.balloon.count		= 0;
				p_wk->mvwk.balloon.don_start_y	= matrix.y;

				// 着地音
				WFLBY_GADGET_SePlay( p_sys, p_wk, WFLBY_SND_BALLOON_02 );
			}
		}
		break;
		
	case WFLBY_GADGET_BALLOON_DOWNSEQ_DON_MAIN:
		{
			u16 rot;
			fx32 dis;
			VecFx32	matrix;
			
			rot = ( 0x7fff * p_wk->mvwk.balloon.count ) / WFLBY_GADGET_BALLOON_OBJ_DON_SYNC;
			dis = FX_Mul( FX_SinIdx( rot ), WFLBY_GADGET_BALLOON_OBJ_DON_DIS );

			WFLBY_3DOBJCONT_DRAW_Get3DMatrix( p_wk->p_person, &matrix );
			matrix.y = p_wk->mvwk.balloon.don_start_y+dis;
			WFLBY_3DOBJCONT_DRAW_Set3DMatrix( p_wk->p_person, &matrix );

			if( (p_wk->mvwk.balloon.count + 1) > WFLBY_GADGET_BALLOON_OBJ_DON_SYNC ){

				// 主人公動作更新あり
				WFLBY_3DOBJCONT_DRAW_SetUpdata( p_wk->p_person, TRUE );	


				// 主人公かチェック
				{
					if( p_wk->p_person == WFLBY_3DOBJCONT_GetPlayer( p_sys->p_objcont ) ){
						// カメラをせっと
						WFLBY_CAMERA_SetTargetPerson( p_sys->p_camera, p_wk->p_person );
					}
				}
				
				return TRUE;
			}else{
				p_wk->mvwk.balloon.count ++;
			}
		}
		break;
	}

	// バルーンアニメ
	p_wk->mvwk.balloon.balloon_fly_count++;
	for( i=0;  i<p_wk->mvwk.balloon.balloon_num; i++ ){

		// 座標設定
		{
			BOOL		ret;
			VecFx32		matrix;
			fx32		carve_x;
			BOOL		result;
			
			ret = WFLBY_GADGET_MV_Straight_Main( &p_wk->mvwk.balloon.straight[i], 
					p_wk->mvwk.balloon.balloon_fly_count );
			if( ret == FALSE ){
				WFLBY_GADGET_MV_SinCurve_Main( &p_wk->mvwk.balloon.sincurve[i] );

				//  値を設定
				D3DOBJ_GetMatrix( &p_wk->obj[i], &matrix.x, &matrix.y, &matrix.z );
				WFLBY_GADGET_MV_Straight_GetNum( &p_wk->mvwk.balloon.straight[i], &matrix.x, &matrix.y, &matrix.z );
				WFLBY_GADGET_MV_SinCurve_GetNum( &p_wk->mvwk.balloon.sincurve[i], &carve_x );

				matrix.x += carve_x;

				D3DOBJ_SetMatrix( &p_wk->obj[i], matrix.x, matrix.y,  matrix.z );
			}else{
				// 破裂
				result = WFLBY_GADGET_Balloon_SetAnm( p_sys, p_wk, i, WFLBY_GADGET_BALLOON_ANM_CRASH );

				// 破裂音
				if( result == TRUE ){
					WFLBY_GADGET_SePlay( p_sys, p_wk, WFLBY_SND_BALLOON_01 );
				}
			}
		}
		
		WFLBY_GADGET_Balloon_Anm( p_sys, p_wk, i );
	}

	return FALSE;
}
// 屋根があるときの動作
static BOOL WFLBY_GADGET_Balloon_MainDown_Roof( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ*  p_wk )
{
	int i;
	
	// 風船アニメは常に動かす
	for( i=0;  i<p_wk->mvwk.balloon.balloon_num; i++ ){
		WFLBY_GADGET_Balloon_Anm( p_sys, p_wk, i );
	}
	
	switch( p_wk->mvwk.balloon.bl_seq ){
	case WFLBY_GADGET_BALLOON_DOWNSEQ_BATA_WAIT:		// バタバタ開始まちはなし
		WFLBY_GADGET_SePlay( p_sys, p_wk, WFLBY_SND_BALLOON_01 );	// 風船が割れた音と風船を割る
		for( i=0; i<p_wk->mvwk.balloon.balloon_num; i++ ){
			WFLBY_GADGET_Balloon_SetAnm( p_sys, p_wk, i, WFLBY_GADGET_BALLOON_ANM_CRASH );
		}
		p_wk->mvwk.balloon.bl_seq = WFLBY_GADGET_BALLOON_DOWNSEQ_BATA_START;
		break;
		
	case WFLBY_GADGET_BALLOON_DOWNSEQ_BATA_START:
		WFLBY_3DOBJCONT_DRAW_SetAnmBata( p_wk->p_person, TRUE );
		p_wk->mvwk.balloon.count = WFLBY_GADGET_BALLOON_OBJ_ROOF_BATA_WAIT;

		p_wk->mvwk.balloon.bl_seq = WFLBY_GADGET_BALLOON_DOWNSEQ_BATA_MAIN;
		break;
		
	case WFLBY_GADGET_BALLOON_DOWNSEQ_BATA_MAIN:
		p_wk->mvwk.balloon.count--;
		if( p_wk->mvwk.balloon.count <= 0 ){
			WFLBY_3DOBJCONT_DRAW_SetAnmBata( p_wk->p_person, FALSE );
			p_wk->mvwk.balloon.bl_seq = WFLBY_GADGET_BALLOON_DOWNSEQ_DOWN;
			p_wk->mvwk.balloon.count = 0;

			// 向いている方向を向かせる
			{
				u32 way;

				way = WFLBY_3DOBJCONT_GetWkObjData( p_wk->p_person, WF2DMAP_OBJPM_WAY );
				WFLBY_3DOBJCONT_DRAW_SetWay( p_wk->p_person, way );
			}
		}
		break;
		
	case WFLBY_GADGET_BALLOON_DOWNSEQ_DOWN:
		{
			BOOL		result;
			VecFx32		matrix;
			
			result = WFLBY_GADGET_MV_Straight_Main( &p_wk->mvwk.balloon.straightobj, 
					p_wk->mvwk.balloon.count );
			p_wk->mvwk.balloon.count ++;
			
			//  値を設定
			WFLBY_3DOBJCONT_DRAW_Get3DMatrix( p_wk->p_person, &matrix );
			WFLBY_GADGET_MV_Straight_GetNum( &p_wk->mvwk.balloon.straightobj, &matrix.x, &matrix.y, &matrix.z );
			WFLBY_3DOBJCONT_DRAW_Set3DMatrix( p_wk->p_person, &matrix );

			if( result == TRUE ){
				p_wk->mvwk.balloon.count		= 0;
				p_wk->mvwk.balloon.don_start_y	= matrix.y;

				// 着地音
				WFLBY_GADGET_SePlay( p_sys, p_wk, WFLBY_SND_BALLOON_02 );

				// 主人公動作更新あり
				WFLBY_3DOBJCONT_DRAW_SetUpdata( p_wk->p_person, TRUE );	


				// 主人公かチェック
				{
					if( p_wk->p_person == WFLBY_3DOBJCONT_GetPlayer( p_sys->p_objcont ) ){
						// カメラをせっと
						WFLBY_CAMERA_SetTargetPerson( p_sys->p_camera, p_wk->p_person );
					}
				}
				
				return TRUE;
			}
		}
		break;
		
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	主人公の位置に風船を描画
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_Balloon_SetObjPos( WFLBY_GADGET_OBJ*  p_wk )
{
	VecFx32 matrix;

	// 座標を取得
	WFLBY_3DOBJCONT_DRAW_Get3DMatrix( p_wk->p_person, &matrix );
	
	switch( p_wk->mvwk.balloon.balloon_num ){
	case 1:
		D3DOBJ_SetMatrix( &p_wk->obj[0], 
				matrix.x + FX32_CONST(sc_WFLBY_GADGET_BALLOON_OBJ_POS[2].x),
				matrix.y + FX32_CONST(sc_WFLBY_GADGET_BALLOON_OBJ_POS[2].y),
				matrix.z + WFLBY_GADGET_BALLOON_OBJ_POS_Z );
		break;
		
	case 2:
		D3DOBJ_SetMatrix( &p_wk->obj[0], 
				matrix.x + FX32_CONST(sc_WFLBY_GADGET_BALLOON_OBJ_POS[0].x),
				matrix.y + FX32_CONST(sc_WFLBY_GADGET_BALLOON_OBJ_POS[0].y),
				matrix.z + WFLBY_GADGET_BALLOON_OBJ_POS_Z );

		D3DOBJ_SetMatrix( &p_wk->obj[1], 
				matrix.x + FX32_CONST(sc_WFLBY_GADGET_BALLOON_OBJ_POS[1].x),
				matrix.y + FX32_CONST(sc_WFLBY_GADGET_BALLOON_OBJ_POS[1].y),
				matrix.z + WFLBY_GADGET_BALLOON_OBJ_POS_Z );
		break;
		
	default:
	case 3:
		D3DOBJ_SetMatrix( &p_wk->obj[0], 
				matrix.x + FX32_CONST(sc_WFLBY_GADGET_BALLOON_OBJ_POS[2].x),
				matrix.y + FX32_CONST(sc_WFLBY_GADGET_BALLOON_OBJ_POS[2].y),
				matrix.z + WFLBY_GADGET_BALLOON_OBJ_POS_Z );

		D3DOBJ_SetMatrix( &p_wk->obj[1], 
				matrix.x + FX32_CONST(sc_WFLBY_GADGET_BALLOON_OBJ_POS[0].x),
				matrix.y + FX32_CONST(sc_WFLBY_GADGET_BALLOON_OBJ_POS[0].y),
				matrix.z + WFLBY_GADGET_BALLOON_OBJ_POS_Z );

		D3DOBJ_SetMatrix( &p_wk->obj[2], 
				matrix.x + FX32_CONST(sc_WFLBY_GADGET_BALLOON_OBJ_POS[1].x),
				matrix.y + FX32_CONST(sc_WFLBY_GADGET_BALLOON_OBJ_POS[1].y),
				matrix.z + WFLBY_GADGET_BALLOON_OBJ_POS_Z );


		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	主人公の位置に風船を描画	屋根ありバージョン
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_Balloon_SetObjPos_Roof( WFLBY_GADGET_OBJ*  p_wk )
{
	VecFx32 matrix;

	// 座標を取得
	WFLBY_3DOBJCONT_DRAW_Get3DMatrix( p_wk->p_person, &matrix );
	
	switch( p_wk->mvwk.balloon.balloon_num ){
	case 1:
		D3DOBJ_SetMatrix( &p_wk->obj[0], 
				matrix.x + FX32_CONST(sc_WFLBY_GADGET_BALLOON_OBJ_ROOF_POS[2].x),
				matrix.y + FX32_CONST(sc_WFLBY_GADGET_BALLOON_OBJ_ROOF_POS[2].y),
				matrix.z + WFLBY_GADGET_BALLOON_OBJ_ROOF_POS_Z );
		break;
		
	case 2:
		D3DOBJ_SetMatrix( &p_wk->obj[0], 
				matrix.x + FX32_CONST(sc_WFLBY_GADGET_BALLOON_OBJ_ROOF_POS[0].x),
				matrix.y + FX32_CONST(sc_WFLBY_GADGET_BALLOON_OBJ_ROOF_POS[0].y),
				matrix.z + WFLBY_GADGET_BALLOON_OBJ_ROOF_POS_Z );

		D3DOBJ_SetMatrix( &p_wk->obj[1], 
				matrix.x + FX32_CONST(sc_WFLBY_GADGET_BALLOON_OBJ_ROOF_POS[1].x),
				matrix.y + FX32_CONST(sc_WFLBY_GADGET_BALLOON_OBJ_ROOF_POS[1].y),
				matrix.z + WFLBY_GADGET_BALLOON_OBJ_ROOF_POS_Z );
		break;
		
	default:
	case 3:
		D3DOBJ_SetMatrix( &p_wk->obj[0], 
				matrix.x + FX32_CONST(sc_WFLBY_GADGET_BALLOON_OBJ_ROOF_POS[2].x),
				matrix.y + FX32_CONST(sc_WFLBY_GADGET_BALLOON_OBJ_ROOF_POS[2].y),
				matrix.z + WFLBY_GADGET_BALLOON_OBJ_ROOF_POS_Z );

		D3DOBJ_SetMatrix( &p_wk->obj[1], 
				matrix.x + FX32_CONST(sc_WFLBY_GADGET_BALLOON_OBJ_ROOF_POS[0].x),
				matrix.y + FX32_CONST(sc_WFLBY_GADGET_BALLOON_OBJ_ROOF_POS[0].y),
				matrix.z + WFLBY_GADGET_BALLOON_OBJ_ROOF_POS_Z );

		D3DOBJ_SetMatrix( &p_wk->obj[2], 
				matrix.x + FX32_CONST(sc_WFLBY_GADGET_BALLOON_OBJ_ROOF_POS[1].x),
				matrix.y + FX32_CONST(sc_WFLBY_GADGET_BALLOON_OBJ_ROOF_POS[1].y),
				matrix.z + WFLBY_GADGET_BALLOON_OBJ_ROOF_POS_Z );
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメ設定
 *
 *	@param	p_sys			ガジェットシステム
 *	@param	p_wk			ワーク
 *	@param	balloon_idx		風船インデックス
 *	@param	anmidx			アニメインデックス
 *
 *	@retval	TRUE	設定した
 *	@retval	FALSE	設定しなかった
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_GADGET_Balloon_SetAnm( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ*  p_wk, u32 balloon_idx, u32 anmidx )
{
	if( p_wk->mvwk.balloon.anmidx[ balloon_idx ] != anmidx ){
		p_wk->mvwk.balloon.anmidx[ balloon_idx ] = anmidx;
		p_wk->mvwk.balloon.anm_wait[ balloon_idx ] = 0;
		WFLBY_GADGET_OBJ_SetFrame( p_sys, p_wk, balloon_idx, 0, (anmidx*WFLBY_GADGET_BALLOON_ANM_ONEFR) );
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	風船アニメ
 *
 *	@param	p_sys		システムワーク
 *	@param	p_wk		ワーク
 *	@param	balloon_idx	風船インデックス
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_Balloon_Anm( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ*  p_wk, u32 balloon_idx )
{
	fx32 frame;
	u32 anmidx;

	anmidx = p_wk->mvwk.balloon.anmidx[balloon_idx];
	
	if( p_wk->mvwk.balloon.anm_wait[balloon_idx] <= 0 ){
		frame = WFLBY_GADGET_OBJ_GetFrame( p_sys, p_wk, balloon_idx, 0 );
		if( frame == (anmidx*WFLBY_GADGET_BALLOON_ANM_ONEFR) ){
			frame = (anmidx*WFLBY_GADGET_BALLOON_ANM_ONEFR) + FX32_ONE;
		}else{

			// 破裂アニメは、終わったら消す
			if( anmidx != WFLBY_GADGET_BALLOON_ANM_CRASH ){
			
				frame = (anmidx*WFLBY_GADGET_BALLOON_ANM_ONEFR);
			}else{
				D3DOBJ_SetDraw( &p_wk->obj[balloon_idx], FALSE );
			}
		}

		WFLBY_GADGET_OBJ_SetFrame( p_sys, p_wk, balloon_idx, 0, frame );
		 
		p_wk->mvwk.balloon.anm_wait[balloon_idx] = WFLBY_GADGET_BALLOON_ANM_WAIT;
	}

	p_wk->mvwk.balloon.anm_wait[balloon_idx]--;
}

//----------------------------------------------------------------------------
/**
 *	@brief	すぱーくる	初期化
 *
 *	@param	p_sys		システムワーク
 *	@param	p_wk		ワーク
 *	@param	roop_num	ループ数
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_Sparkle_Init( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk, u32 roop_num )
{
	int i, j;
	VecFx32 matrix;
	VecFx32 set_matrix;
	VecFx32 end_matrix;
	u32 idx;
	u32 objidx;
	
	// 主人公座標を取得
	WFLBY_3DOBJCONT_DRAW_Get3DMatrix( p_wk->p_person, &matrix );


	for( i=0; i<roop_num; i++ ){
		objidx = WFLBY_GADGET_OBJ_SPARKLE00+i;
		if( objidx > WFLBY_GADGET_OBJ_SPARKLE02 ){
			objidx = WFLBY_GADGET_OBJ_SPARKLE02;
		}
		
		for( j=0; j<WFLBY_GADGET_SPARKLE_1ROOP_OBJNUM; j++ ){
			
			idx = (i*WFLBY_GADGET_SPARKLE_1ROOP_OBJNUM)+j;
			WFLBY_GADGET_OBJ_SetRes( p_sys, p_wk, 
					idx,
					&sc_WFLBY_GADGET_RES[objidx] );
			VEC_Add( &matrix, 
					&sc_WFLBY_GADGET_SPARKLE_OFFS[ j ], &set_matrix );
			VEC_Add( &set_matrix, 
					&sc_WFLBY_GADGET_SPARKLE_MOVE[ j ], &end_matrix );
			D3DOBJ_SetMatrix( &p_wk->obj[idx], 
					set_matrix.x, set_matrix.y, set_matrix.z );

			D3DOBJ_SetDraw( &p_wk->obj[idx], FALSE );

			// 移動パラメータ設定
			p_wk->mvwk.sparkle.objcount[idx] = 0;
			WFLBY_GADGET_MV_Straight_Init( &p_wk->mvwk.sparkle.objstraight[idx],
					set_matrix.x, end_matrix.x, 
					set_matrix.y, end_matrix.y, 
					set_matrix.z, end_matrix.z, 
					WFLBY_GADGET_SPARKLE_ANM_SYNC );
		}	
	}
	
	p_wk->mvwk.sparkle.roop_num		= roop_num;
	p_wk->mvwk.sparkle.roop_count	= 0;
	p_wk->mvwk.sparkle.count		= 0;
	p_wk->mvwk.sparkle.buff_num		= 0;
}


//----------------------------------------------------------------------------
/**
 *	@brief	スパークル	１オブジェ動作
 *
 *	@param	p_sys		システム
 *	@param	p_wk		ワーク
 *	@param	objidx		オブジェインデックス
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_GADGET_Sparkle_OneObj_Main( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk, u32 objidx )
{
	BOOL result;
	VecFx32 move;

	p_wk->mvwk.sparkle.objcount[objidx]++;
		
	// 移動動作
	result = WFLBY_GADGET_MV_Straight_Main( &p_wk->mvwk.sparkle.objstraight[objidx],
			p_wk->mvwk.sparkle.objcount[objidx]	);

	// 移動値設定
	WFLBY_GADGET_MV_Straight_GetNum( &p_wk->mvwk.sparkle.objstraight[objidx], 
			&move.x, &move.y, &move.z );

	D3DOBJ_SetMatrix( &p_wk->obj[objidx], move.x, move.y, move.z );

	//  アニメループ
	WFLBY_GADGET_OBJ_LoopAnm( p_sys, p_wk, objidx, 0 );

	return result;
}


//----------------------------------------------------------------------------
/**
 *	@brief	リップル動作の初期化
 *
 *	@param	p_sys			システム
 *	@param	p_wk			ワーク
 *	@param	ripple_num		リップル数
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_Ripple_Init( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk, u32 ripple_num )
{
	int i;
	VecFx32 matrix;


	for( i=0; i<ripple_num; i++ ){
		WFLBY_GADGET_OBJ_SetRes( p_sys, p_wk, i, &sc_WFLBY_GADGET_RES[WFLBY_GADGET_OBJ_RIPPLE00 + i] );

		// 座標をあわせる
		WFLBY_3DOBJCONT_DRAW_Get3DMatrix( p_wk->p_person, &matrix );
		// 設定
		matrix.y += WFLBY_GADGET_FLOOR_Y;
		matrix.z += WFLBY_GADGET_RIPPLE_MAT_Z;
		D3DOBJ_SetMatrix( &p_wk->obj[i], matrix.x, matrix.y, matrix.z );

		// 非表示
		D3DOBJ_SetDraw( &p_wk->obj[i], FALSE );
	}

	p_wk->mvwk.ripple.ripple_num	= ripple_num;
}


//----------------------------------------------------------------------------
/**
 *	@brief	シグナルオブジェ	メイン動作
 *
 *	@param	p_sys		システム
 *	@param	p_wk		ワーク
 *	@param	num			表示するオブジェの種類
 *
 *	@retval	TRUE	終了
 *	@retval	FALS	途中
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_GADGET_Signal_Main( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk, u32 num )
{
	BOOL result;
	u32	move_num;
	WFLBY_GADGET_SINGNAL_ANMDATA	data;

	// 発射
	move_num = (p_wk->count * WFLBY_GADGET_SIGNAL_ANM_FRAME_NUM) / WFLBY_GADGET_SIGNAL_ANM_SYNC;
	if( move_num != p_wk->mvwk.signal.lastnum ){
		p_wk->mvwk.signal.lastnum = move_num;

		// 今まで表示してたのを非表示にする
		if( move_num > 0 ){
			data = sc_WFLBY_GADGET_SIGNAL_ANMSEQ[ num ][ move_num-1 ];
			D3DOBJ_SetDraw( &p_wk->obj[data.objidx], FALSE );
		}

		data = sc_WFLBY_GADGET_SIGNAL_ANMSEQ[ num ][ move_num ];

		// アニメ再生
		if( WFLBY_GADGET_SINGNAL_ANM_OFF != data.anm_data ){
			WFLBY_GADGET_OBJ_SetFrame( p_sys, p_wk, data.objidx, 0, 
					FX32_CONST( data.anm_data ) );
			WFLBY_GADGET_OBJ_SetFrame( p_sys, p_wk, data.objidx, 1, 
					FX32_CONST( data.anm_data ) );

			D3DOBJ_SetDraw( &p_wk->obj[data.objidx], TRUE );
		}else{
			D3DOBJ_SetDraw( &p_wk->obj[data.objidx], FALSE );
		}
	}
	
	// カウント
	if( p_wk->count < WFLBY_GADGET_SIGNAL_ANM_SYNC ){
		p_wk->count ++;
	}

	// 終了チェック
	if( (p_wk->count >= WFLBY_GADGET_SIGNAL_ANM_SYNC) ){
		return TRUE;
	}
	return FALSE;
}


//----------------------------------------------------------------------------
/**
 *	@brief	スウィング	初期化
 *
 *	@param	p_sys		システム
 *	@param	p_wk		ワーク
 *	@param	num			オブジェ数
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_Swing_Init( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk, u32 num )
{
	int i;
	VecFx32 matrix;


	for( i=0; i<num; i++ ){
		WFLBY_GADGET_OBJ_SetRes( p_sys, p_wk, i, &sc_WFLBY_GADGET_RES[WFLBY_GADGET_OBJ_SWING00 + i] );

		// 座標をあわせる
		WFLBY_3DOBJCONT_DRAW_Get3DMatrix( p_wk->p_person, &matrix );
		// 設定
		matrix.y += WFLBY_GADGET_FLOOR_Y;
		D3DOBJ_SetMatrix( &p_wk->obj[i], matrix.x, matrix.y, matrix.z );

		// 非表示
		D3DOBJ_SetDraw( &p_wk->obj[i], FALSE );
	}

	p_wk->mvwk.swing.lastnum	= 0xff;
	p_wk->mvwk.swing.objnum		= num;
}


//----------------------------------------------------------------------------
/**
 *	@brief	シンバルｲﾅｽﾞﾏ	初期化
 *
 *	@param	p_sys		システム
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_Inazuma_Init( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	VecFx32 matrix;
	int i;

	// 稲妻作成
	WFLBY_3DOBJCONT_DRAW_Get3DMatrix( p_wk->p_person, &matrix );
	matrix.z += WFLBY_GADGET_CYMBALS_INAZUMA_OFS_Z;
	matrix.x += WFLBY_GADGET_CYMBALS_INAZUMA_OFS_X;
	matrix.y += WFLBY_GADGET_AIR_Y + WFLBY_GADGET_CYMBALS_INAZUMA_OFS_Y;
	for( i=0; i<WFLBY_GADGET_CYMBALS_INAZUMA_NUM; i++ ){
		WFLBY_GADGET_OBJ_SetRes( p_sys, p_wk, 
				WFLBY_GADGET_CYMBALS_INAZUMA_00+i, &sc_WFLBY_GADGET_RES[ WFLBY_GADGET_OBJ_SPARK00+i ] );

		// 座標設定
		D3DOBJ_SetMatrix( &p_wk->obj[i+WFLBY_GADGET_CYMBALS_INAZUMA_00], 
				matrix.x + (i*WFLBY_GADGET_CYMBALS_INAZUMA_DIS_X),
				matrix.y, matrix.z );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ｲﾅｽﾞﾏアニメ	開始
 *
 *	@param	p_sys		システムワーク
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_Inazuma_Start( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	int i;
	
	p_wk->mvwk.onpu.inazuma_move	= TRUE;
	p_wk->mvwk.onpu.inazuma_count	= 0;

	for( i=0; i<WFLBY_GADGET_CYMBALS_INAZUMA_NUM; i++ ){
		D3DOBJ_SetDraw( &p_wk->obj[i+WFLBY_GADGET_CYMBALS_INAZUMA_00], TRUE );
	}

}

//----------------------------------------------------------------------------
/**
 *	@brief	ｲﾅｽﾞﾏアニメ	メイン
 *
 *	@param	p_sys		システムワーク
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_Inazuma_Main( WFLBY_GADGET* p_sys, WFLBY_GADGET_OBJ* p_wk )
{
	int i;

	if( p_wk->mvwk.onpu.inazuma_move == FALSE ){
		return ;
	}

	// カウント処理
	if( p_wk->mvwk.onpu.inazuma_count < WFLBY_GADGET_CYMBALS_INAZUMA_ANM_SYNC ){
		p_wk->mvwk.onpu.inazuma_count++;
	}else{
		// 動作停止
		p_wk->mvwk.onpu.inazuma_move = FALSE;
	}

	// ループアニメ
	for( i=0; i<WFLBY_GADGET_CYMBALS_INAZUMA_NUM; i++ ){
		if( p_wk->mvwk.onpu.inazuma_count < WFLBY_GADGET_CYMBALS_INAZUMA_ANM_SYNC ){
			WFLBY_GADGET_OBJ_LoopAnm( p_sys, p_wk, i+WFLBY_GADGET_CYMBALS_INAZUMA_00, 0 );
		}else{
			D3DOBJ_SetDraw( &p_wk->obj[i+WFLBY_GADGET_CYMBALS_INAZUMA_00], FALSE );
		}
	}

	
}

//----------------------------------------------------------------------------
/**
 *	@brief	音符発射システム	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	cp_data		データ
 *	@param	data_num	データ数
 *	@param	count_max	カウント最大値
 *	@param	anm_type	アニメタイプ
 *	@param	p_gadget	ガジェットデータ
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_ONPU_Cont_Init( WFLBY_GADGET_OBJWK* p_wk, const WFLBY_GADGET_MOVE_ONPU_DATA* cp_data, u32 data_num, u32 count_max, u32 anm_type, WFLBY_GADGET_OBJ* p_gadget )
{
	int i;
	
	p_wk->onpu.cp_data		= cp_data;
	p_wk->onpu.data_num		= data_num;
	p_wk->onpu.count_max	= count_max;

	// 動作パラメータの初期化
	for( i=0; i<WFLBY_GADGET_MOVE_ONPU_WK_NUM; i++ ){
		WFLBY_GADGET_OnpuMove_Init( &p_wk->onpu.move[i], 
				&p_gadget->obj[(i*WFLBY_GADGET_ONPU_OBJ_NUM)+0],
				&p_gadget->obj[(i*WFLBY_GADGET_ONPU_OBJ_NUM)+1],
				&p_gadget->obj[(i*WFLBY_GADGET_ONPU_OBJ_NUM)+2],
				&p_gadget->obj[(i*WFLBY_GADGET_ONPU_OBJ_NUM)+3],
				p_gadget->p_person, anm_type );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	音符発射システム	メイン
 *
 *	@param	p_wk		ワーク
 *	@param	count		カウント
 *
 *	@retval	WFLBY_GADGET_ONPU_MAIN_PLAY,		// 再生中
 *	@retval	WFLBY_GADGET_ONPU_MAIN_START,		// 再生中で音符を発射した
 *	@retval	WFLBY_GADGET_ONPU_MAIN_ALLEND,		// 全部終わった
 */
//-----------------------------------------------------------------------------
static WFLBY_GADGET_ONPU_MAIN_RET WFLBY_GADGET_ONPU_Cont_Main( WFLBY_GADGET_OBJWK* p_wk, u32 count )
{
	int i;
	WFLBY_GADGET_MOVE_ONPU_DATA data;
	BOOL result;
	BOOL all_end;
	BOOL ret = WFLBY_GADGET_ONPU_MAIN_PLAY;
	
	// countのカウント値で動作を開始するデータがあったら動かす
	for( i=0; i<p_wk->onpu.data_num; i++ ){
		data = p_wk->onpu.cp_data[i];
		if( data.count == count ){
			GF_ASSERT( data.anmidx < WFLBY_GADGET_MOVE_ONPU_WK_NUM );
			GF_ASSERT( data.mvnum <= WFLBY_GADGET_ONPU_OBJ_NUM );
			WFLBY_GADGET_OnpuMove_Start( &p_wk->onpu.move[ data.anmidx ], data.mvnum );
			ret = WFLBY_GADGET_ONPU_MAIN_START;
		}
	}

	// 動作メイン
	all_end = TRUE;
	for( i=0; i<WFLBY_GADGET_MOVE_ONPU_WK_NUM; i++ ){
		result = WFLBY_GADGET_OnpuMove_Main( &p_wk->onpu.move[ i ] );
		if( result == FALSE ){	// 動いているのがいるのでまだ終わっちゃいいけない
			all_end = FALSE;
			
		}
	}


	// 終了チェック
	if( (p_wk->onpu.count_max <= count) && (all_end == TRUE) ){
		ret = WFLBY_GADGET_ONPU_MAIN_ALLEND;
	}
	return ret;
}



//----------------------------------------------------------------------------
/**
 *	@brief	動作の開始関数
 *
 *	@param	p_wk		ワーク
 *	@param	p_obj0		描画オブジェ０
 *	@param	p_obj1		描画オブジェ１
 *	@param	p_obj2		描画オブジェ２
 *	@param	p_obj3		描画オブジェ３
 *	@param	cp_person	主人公
 *	@param	anm_type	アニメタイプ
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_OnpuMove_Init( WFLBY_GADGET_ONPU* p_wk, D3DOBJ* p_obj0, D3DOBJ* p_obj1, D3DOBJ* p_obj2, D3DOBJ* p_obj3, const WFLBY_3DPERSON* cp_person, u32 anm_type )
{
	GF_ASSERT( anm_type < WFLBY_GADGET_ONPU_MOVE_NUM );

	// 表示するオブジェの表示フラグを設定
	p_wk->p_obj[0] = p_obj0;
	p_wk->p_obj[1] = p_obj1;
	p_wk->p_obj[2] = p_obj2;
	p_wk->p_obj[3] = p_obj3;

	p_wk->cp_person = cp_person;

	
	// ベル動作初期化
	p_wk->type	= anm_type;
}

//----------------------------------------------------------------------------
/**
 *	@brief	音符動作	開始
 *
 *	@param	p_wk		ワーク
 *	@param	num			数
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_OnpuMove_Start( WFLBY_GADGET_ONPU* p_wk, u32 num )
{
	int i;
	static void (* const p_Func[WFLBY_GADGET_ONPU_MOVE_NUM])( WFLBY_GADGET_ONPU* p_wk, const WFLBY_3DPERSON* cp_person ) = {
		WFLBY_GADGET_OnpuMove_InitBell,
		WFLBY_GADGET_OnpuMove_InitDram,
		WFLBY_GADGET_OnpuMove_InitCymbals,
	};

	// 表示開始
	for( i=0; i<num; i++ ){
		D3DOBJ_SetDraw( p_wk->p_obj[i], TRUE );
	}

	p_wk->count	= 0;
	p_wk->move	= TRUE;
	p_wk->mvnum	= num;

	p_Func[ p_wk->type ]( p_wk, p_wk->cp_person );
}

//----------------------------------------------------------------------------
/**
 *	@brief	音符動作メイン関数
 *
 *	@param	p_wk	ワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_GADGET_OnpuMove_Main( WFLBY_GADGET_ONPU* p_wk )
{
	int i;
	BOOL result;
	static BOOL (* const p_Func[WFLBY_GADGET_ONPU_MOVE_NUM])( WFLBY_GADGET_ONPU* p_wk ) = {
		WFLBY_GADGET_OnpuMove_MainBell,
		WFLBY_GADGET_OnpuMove_MainDram,
		WFLBY_GADGET_OnpuMove_MainCymbals,
	};

	if( p_wk->move == FALSE ){
		return TRUE;
	}

	// メイン動作
	result = p_Func[ p_wk->type ]( p_wk );

	// カウント
	p_wk->count ++;

	// 終了になったら処理を終了
	if( result == TRUE ){
		// 表示OFF
		for( i=0; i<p_wk->mvnum; i++ ){
			D3DOBJ_SetDraw( p_wk->p_obj[i], FALSE );
		}
		p_wk->move = FALSE;
	}

	return result;
}


//----------------------------------------------------------------------------
/**
 *	@brief	ベル動作の開始
 *	
 *	@param	p_wk		ワーク
 *	@param	cp_person	主人公
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_OnpuMove_InitBell( WFLBY_GADGET_ONPU* p_wk, const WFLBY_3DPERSON* cp_person )
{
	int i;
	VecFx32 matrix;
	VecFx32 end_matrix;
	u16	start_rot;

	// 主人公の座標を取得
	WFLBY_3DOBJCONT_DRAW_Get3DMatrix( cp_person, &matrix );
	matrix.x += WFLBY_GADGET_ONPU_X_OFS;
	matrix.y += WFLBY_GADGET_AIR_Y;
	matrix.z += WFLBY_GADGET_ONPU_Z_OFS;
		
	for( i=0; i<p_wk->mvnum; i++ ){
		switch( i ){
		case 0:
			end_matrix.x = matrix.x - WFLBY_GADGET_ONPU_MOVE_BELL_X1;
			end_matrix.y = matrix.y + WFLBY_GADGET_ONPU_MOVE_BELL_Y;
			end_matrix.z = matrix.z + 0;
			start_rot	 = FX_GET_ROTA_NUM(0);
			break;
		case 1:
			end_matrix.x = matrix.x + WFLBY_GADGET_ONPU_MOVE_BELL_X1;
			end_matrix.y = matrix.y + WFLBY_GADGET_ONPU_MOVE_BELL_Y;
			end_matrix.z = matrix.z + 0;
			start_rot	 = FX_GET_ROTA_NUM(180);
			break;
		case 2:
			end_matrix.x = matrix.x - WFLBY_GADGET_ONPU_MOVE_BELL_X0;
			end_matrix.y = matrix.y + WFLBY_GADGET_ONPU_MOVE_BELL_Y;
			end_matrix.z = matrix.z - WFLBY_GADGET_ONPU_MOVE_BELL_Z;
			start_rot	 = FX_GET_ROTA_NUM(180);
			break;
		case 3:
			end_matrix.x = matrix.x + WFLBY_GADGET_ONPU_MOVE_BELL_X0;
			end_matrix.y = matrix.y + WFLBY_GADGET_ONPU_MOVE_BELL_Y;
			end_matrix.z = matrix.z - WFLBY_GADGET_ONPU_MOVE_BELL_Z;
			start_rot	 = FX_GET_ROTA_NUM(0);
			break;
		}
		WFLBY_GADGET_MV_Straight_Init( &p_wk->st[i], 
				matrix.x, end_matrix.x,
				matrix.y, end_matrix.y,
				matrix.z, end_matrix.z,
				WFLBY_GADGET_ONPU_MOVE_BELL_SYNC );
		WFLBY_GADGET_MV_SinCurve_Init( &p_wk->cv[i], 
				start_rot, WFLBY_GADGET_ONPU_MOVE_BELL_CVSP,
				WFLBY_GADGET_ONPU_MOVE_BELL_CVX );
		D3DOBJ_SetMatrix( p_wk->p_obj[i], matrix.x, matrix.y, matrix.z );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ベル動作メイン関数
 *
 *	@param	p_wk		ワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_GADGET_OnpuMove_MainBell( WFLBY_GADGET_ONPU* p_wk )
{
	int		i;
	BOOL	result;
	VecFx32	matrix;
	fx32	cv_num;

	for( i=0; i<p_wk->mvnum; i++ ){
		// カーブ移動
		WFLBY_GADGET_MV_SinCurve_Main( &p_wk->cv[i] );
		
		// 直線移動
		result = WFLBY_GADGET_MV_Straight_Main( &p_wk->st[i], p_wk->count );

		// 座標の設定
		WFLBY_GADGET_MV_SinCurve_GetNum( &p_wk->cv[i], &cv_num );
		WFLBY_GADGET_MV_Straight_GetNum( &p_wk->st[i], &matrix.x, &matrix.y, &matrix.z );
		matrix.x += cv_num;
		D3DOBJ_SetMatrix( p_wk->p_obj[i], matrix.x, matrix.y, matrix.z );
	}

	return result;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ドラム動作開始
 *
 *	@param	p_wk		ワーク
 *	@param	cp_person	主人公
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_OnpuMove_InitDram( WFLBY_GADGET_ONPU* p_wk, const WFLBY_3DPERSON* cp_person )
{
	int i;
	VecFx32 matrix;
	VecFx32 end_matrix;

	// 主人公の座標を取得
	WFLBY_3DOBJCONT_DRAW_Get3DMatrix( cp_person, &matrix );
	matrix.x += WFLBY_GADGET_ONPU_X_OFS;
	matrix.y += WFLBY_GADGET_FLOOR_Y;
	matrix.z += WFLBY_GADGET_ONPU_Z_OFS;
		
	for( i=0; i<p_wk->mvnum; i++ ){
		switch( i ){
		case 0:
			end_matrix.x = matrix.x - WFLBY_GADGET_ONPU_MOVE_DRUM_X;
			end_matrix.y = matrix.y;
			end_matrix.z = matrix.z + WFLBY_GADGET_ONPU_MOVE_DRUM_Z;
			break;
		case 1:
			end_matrix.x = matrix.x + WFLBY_GADGET_ONPU_MOVE_DRUM_X;
			end_matrix.y = matrix.y;
			end_matrix.z = matrix.z + WFLBY_GADGET_ONPU_MOVE_DRUM_Z;
			break;
		case 2:
			if( p_wk->mvnum == 3 ){
				end_matrix.x = matrix.x;
				end_matrix.y = matrix.y;
				end_matrix.z = matrix.z - (WFLBY_GADGET_ONPU_MOVE_DRUM_Z + WFLBY_GADGET_ONPU_MOVE_DRUM_X);
			}else{
				end_matrix.x = matrix.x - WFLBY_GADGET_ONPU_MOVE_DRUM_X;
				end_matrix.y = matrix.y;
				end_matrix.z = matrix.z - WFLBY_GADGET_ONPU_MOVE_DRUM_Z;
			}
			break;
		case 3:
			end_matrix.x = matrix.x + WFLBY_GADGET_ONPU_MOVE_DRUM_X;
			end_matrix.y = matrix.y;
			end_matrix.z = matrix.z - WFLBY_GADGET_ONPU_MOVE_DRUM_Z;
			break;
		}
		WFLBY_GADGET_MV_Straight_Init( &p_wk->st[i], 
				matrix.x, end_matrix.x,
				matrix.y, end_matrix.y,
				matrix.z, end_matrix.z,
				WFLBY_GADGET_ONPU_MOVE_DRUM_XZ_SYNC );
		WFLBY_GADGET_MV_SinCurve_Init( &p_wk->cv[i], 
				WFLBY_GADGET_ONPU_MOVE_DRUM_DOWN_ROT, 
				WFLBY_GADGET_ONPU_MOVE_DRUM_DOWN_SP,
				WFLBY_GADGET_ONPU_MOVE_DRUM_DOWN_Y );
	}

	// 座標設定	
	WFLBY_GADGET_OnpuMove_MainDram( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ドラム動作メイン関数
 *
 *	@param	p_wk		ワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_GADGET_OnpuMove_MainDram( WFLBY_GADGET_ONPU* p_wk )
{
	int		i;
	BOOL	result;
	VecFx32	matrix;
	fx32	cv_num;

	for( i=0; i<p_wk->mvnum; i++ ){

		// カウンタがWFLBY_GADGET_ONPU_MOVE_DRUM_DOWN_SYNCになったら回転移動のパラメータ変更
		if( p_wk->count == WFLBY_GADGET_ONPU_MOVE_DRUM_DOWN_SYNC ){
			WFLBY_GADGET_MV_SinCurve_Init( &p_wk->cv[i], 
					WFLBY_GADGET_ONPU_MOVE_DRUM_UP_ROT, 
					WFLBY_GADGET_ONPU_MOVE_DRUM_UP_SP,
					WFLBY_GADGET_ONPU_MOVE_DRUM_UP_Y );
		}else{

			// カーブ移動
			WFLBY_GADGET_MV_SinCurve_Main( &p_wk->cv[i] );
		}
		
		// 直線移動
		WFLBY_GADGET_MV_Straight_Main( &p_wk->st[i], p_wk->count );

		// 座標の設定
		WFLBY_GADGET_MV_SinCurve_GetNum( &p_wk->cv[i], &cv_num );
		WFLBY_GADGET_MV_Straight_GetNum( &p_wk->st[i], &matrix.x, &matrix.y, &matrix.z );
		matrix.y += cv_num;
		D3DOBJ_SetMatrix( p_wk->p_obj[i], matrix.x, matrix.y, matrix.z );

	}

	if( p_wk->count >= WFLBY_GADGET_ONPU_MOVE_DRUM_SYNC ){
		return TRUE;
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	シンバル動作開始
 *
 *	@param	p_wk		ワーク
 *	@param	cp_person	主人公
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_OnpuMove_InitCymbals( WFLBY_GADGET_ONPU* p_wk, const WFLBY_3DPERSON* cp_person )
{
	int i;
	VecFx32 matrix;
	VecFx32 end_matrix;

	// 主人公の座標を取得
	WFLBY_3DOBJCONT_DRAW_Get3DMatrix( cp_person, &matrix );
	matrix.x += WFLBY_GADGET_ONPU_X_OFS;
	matrix.y += WFLBY_GADGET_AIR_Y + WFLBY_GADGET_ONPU_MOVE_CYMBALS_START_Y;
	matrix.z += WFLBY_GADGET_ONPU_Z_OFS;
		
	for( i=0; i<p_wk->mvnum; i++ ){
		WFLBY_GADGET_OnpuMove_SetCymbalsStParam( p_wk, i, &matrix,
				WFLBY_GADGET_ONPU_MOVE_CYMBALS_SEQ00_X00,
				WFLBY_GADGET_ONPU_MOVE_CYMBALS_SEQ00_Y00,
				WFLBY_GADGET_ONPU_MOVE_CYMBALS_SEQ00_X01,
				WFLBY_GADGET_ONPU_MOVE_CYMBALS_SEQ00_Y01,
				WFLBY_GADGET_ONPU_MOVE_CYMBALS_SEQ00_SYNC );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	シンバル動作メイン関数
 *
 *	@param	p_wk		ワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_GADGET_OnpuMove_MainCymbals( WFLBY_GADGET_ONPU* p_wk )
{
	int		i;
	BOOL	result;
	VecFx32	matrix;

	for( i=0; i<p_wk->mvnum; i++ ){
		
		// 直線移動
		result = WFLBY_GADGET_MV_Straight_Main( &p_wk->st[i], p_wk->count );

		// 座標の設定
		WFLBY_GADGET_MV_Straight_GetNum( &p_wk->st[i], &matrix.x, &matrix.y, &matrix.z );
		D3DOBJ_SetMatrix( p_wk->p_obj[i], matrix.x, matrix.y, matrix.z );

		// 終了したら次の処理
		if( result == TRUE ){

			if( p_wk->seq == 0 ){
				WFLBY_GADGET_OnpuMove_SetCymbalsStParam( p_wk, i, &matrix,
						WFLBY_GADGET_ONPU_MOVE_CYMBALS_SEQ01_X00,
						WFLBY_GADGET_ONPU_MOVE_CYMBALS_SEQ01_Y00,
						WFLBY_GADGET_ONPU_MOVE_CYMBALS_SEQ01_X01,
						WFLBY_GADGET_ONPU_MOVE_CYMBALS_SEQ01_Y01,
						WFLBY_GADGET_ONPU_MOVE_CYMBALS_SEQ01_SYNC );
			}else{
				WFLBY_GADGET_OnpuMove_SetCymbalsStParam( p_wk, i, &matrix,
						WFLBY_GADGET_ONPU_MOVE_CYMBALS_SEQ02_X00,
						WFLBY_GADGET_ONPU_MOVE_CYMBALS_SEQ02_Y00,
						WFLBY_GADGET_ONPU_MOVE_CYMBALS_SEQ02_X01,
						WFLBY_GADGET_ONPU_MOVE_CYMBALS_SEQ02_Y01,
						WFLBY_GADGET_ONPU_MOVE_CYMBALS_SEQ02_SYNC );
			}
		}
	}

	if( result == TRUE ){
		if( (p_wk->seq + 1) < WFLBY_GADGET_ONPU_MOVE_CYMBALS_SEQ_NUM ){
			p_wk->seq ++;
			p_wk->count = 0;	// カウンタ初期化
		}else{
			// 終了
			return TRUE;
		}
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	CYMBALS動作値設定
 *
 *	@param	p_wk			ワーク
 *	@param	idx				インデックス
 *	@param	cp_def_matrix	基本座標
 *	@param	ofs_x0			オフセットX
 *	@param	ofs_y0			オフセットY
 *	@param	ofs_x1			オフセットX
 *	@param	ofs_y1			オフセットY
 *	@param	sync			シンク数
 */
//-----------------------------------------------------------------------------
static void WFLBY_GADGET_OnpuMove_SetCymbalsStParam( WFLBY_GADGET_ONPU* p_wk, u32 idx, const VecFx32* cp_def_matrix, fx32 ofs_x0, fx32 ofs_y0, fx32 ofs_x1, fx32 ofs_y1, u32 sync )
{
	VecFx32 end_matrix;
	
	switch( idx ){
	case 0:
		end_matrix.x = cp_def_matrix->x - ofs_x0;
		end_matrix.y = cp_def_matrix->y + ofs_y0;
		end_matrix.z = cp_def_matrix->z;
		break;
	case 1:
		end_matrix.x = cp_def_matrix->x + ofs_x0;
		end_matrix.y = cp_def_matrix->y + ofs_y0;
		end_matrix.z = cp_def_matrix->z;
		break;
	case 2:
		end_matrix.x = cp_def_matrix->x - ofs_x1;
		end_matrix.y = cp_def_matrix->y + ofs_y1;
		end_matrix.z = cp_def_matrix->z;
		break;
	case 3:
		end_matrix.x = cp_def_matrix->x + ofs_x1;
		end_matrix.y = cp_def_matrix->y + ofs_y1;
		end_matrix.z = cp_def_matrix->z;
		break;
	}
	WFLBY_GADGET_MV_Straight_Init( &p_wk->st[idx], 
			cp_def_matrix->x, end_matrix.x,
			cp_def_matrix->y, end_matrix.y,
			cp_def_matrix->z, end_matrix.z,
			sync );

	D3DOBJ_SetMatrix( p_wk->p_obj[idx], cp_def_matrix->x, cp_def_matrix->y, cp_def_matrix->z );
}

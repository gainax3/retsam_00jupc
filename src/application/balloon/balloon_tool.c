//==============================================================================
/**
 * @file	balloon_tool.c
 * @brief	風船割り：ツール類
 * @author	matsuda
 * @date	2007.11.25(日)
 */
//==============================================================================
#include "common.h"
#include "system/clact_tool.h"
#include "system/palanm.h"
#include "system/pmfprint.h"
#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "system/fontproc.h"
#include "system/msgdata.h"
#include "system/particle.h"
#include "system/brightness.h"
#include "system/snd_tool.h"
#include "communication/communication.h"
#include "communication/wm_icon.h"
#include "msgdata/msg.naix"
#include "system/wipe.h"
#include "communication/wm_icon.h"
#include "system/msgdata_util.h"
#include "system/procsys.h"
#include "system/d3dobj.h"
#include "system/fontoam.h"

#include "balloon_common.h"
#include "balloon_comm_types.h"
#include "balloon_game_types.h"
#include "application/balloon.h"
#include "balloon_game.h"
#include "balloon_tcb_pri.h"
#include "balloon_sonans.h"
#include "balloon_id.h"
#include "balloon_tool.h"
#include "balloon_send_recv.h"
#include "balloon_control.h"
#include "balloon_snd_def.h"

#include "graphic/balloon_gra_def.h"
#include "msgdata/msg_balloon.h"


//==============================================================================
//	風船の耐久力
//==============================================================================
///風船出現位置X
#define BALLOON_APPEAR_POS_X	(128)
///風船出現位置Y
#define BALLOON_APPEAR_POS_Y	(96)

///風船の最低拡縮率(耐久力が0でも、最低この大きさは維持する)
#define BALLOON_SCALE_MIN	(0x0300)
///このスケール値より大きくなったらXを歪ませ始める
#define BALLOON_SCALE_DISTORTION_START	(FX32_ONE)
///風船を歪ませる為に乗算する
#define BALLOON_SCALE_DISTORTION_X		(FX32_CONST(1.1))

///自分の空気の場合、下画面の空気が上に到着してから出すので、その分登場までウェイトを入れる
#define MY_AIR_2D_APPEAR_WAIT		(25)
///空気アクターの移動速度(fx32)
#define AIR_MOVE_SPEED		(FX32_CONST(5.0))

///上移動の空気アクターが消える位置：Y
#define AIR_MOVE_UP_DELETE_Y	(98+16)// + 16/2)
///右移動の空気アクターが消える位置：Y
#define AIR_MOVE_RIGHT_DELETE_X	(128-24)// - 16/2)
///下移動の空気アクターが消える位置：Y
#define AIR_MOVE_DOWN_DELETE_Y	(98-24)// - 16/2)
///左移動の空気アクターが消える位置：X
#define AIR_MOVE_LEFT_DELETE_X	(128+24)// + 16/2)

///ジョイントの何ドット手前から縮小を始めるか
#define AIR_REDUCE_DOT_OFFSET		(24/2 + 2)	//空気OBJが直径約24ドットなので。+2=微調整

///風船アイコンの表示座標X(一番右端に位置した時のX座標)
#define ICON_BALLOON_POS_RIGHT_X	(9 * 8)
///風船アイコンの表示座標Y
#define ICON_BALLOON_POS_Y			(4 * 8)
///風船アイコンの配置間隔：X
#define ICON_BALLOON_POS_OFFSET_X	(16)

///風船アイコンの移動速度X
#define ICON_BALLOON_MOVE_SP_X		(2)
///風船の耐久力のこのパーセンテージ以上(100%換算)になると風船アイコンはピンチ状態になる
#define ICON_BALLOON_PINCH_PARCENT	(75)

///風船のごちゃ混ぜセルに入っているアニメシーケンス
enum{
	MIXOBJ_ANMSEQ_ICON_BALLOON_1,	//風船アイコン：レベル１（ノーマル)
	MIXOBJ_ANMSEQ_ICON_BALLOON_2,	//風船アイコン：レベル２（ノーマル)
	MIXOBJ_ANMSEQ_ICON_BALLOON_3,	//風船アイコン：レベル３（ノーマル)
	MIXOBJ_ANMSEQ_ICON_BALLOON_1_PINCH,		//風船アイコン：レベル1（破裂しそう)
	MIXOBJ_ANMSEQ_ICON_BALLOON_2_PINCH,		//風船アイコン：レベル2（破裂しそう)
	MIXOBJ_ANMSEQ_ICON_BALLOON_3_PINCH,		//風船アイコン：レベル3（破裂しそう)
	MIXOBJ_ANMSEQ_ICON_BALLOON_EXPLODED,	//風船アイコン：破裂アニメ
	MIXOBJ_ANMSEQ_AIR_0,			//空気1人目
	MIXOBJ_ANMSEQ_AIR_1,			//空気2人目
	MIXOBJ_ANMSEQ_AIR_2,			//空気3人目
	MIXOBJ_ANMSEQ_AIR_3,			//空気4人目
	
	MIXOBJ_ANMSEQ_BALLOON_EXPLODED = 11,	//風船の破裂アニメ

	MIXOBJ_ANMSEQ_BOOSTER_1P_JUMP = 12,			//ブースター(青)：ジャンプ
	MIXOBJ_ANMSEQ_BOOSTER_1P_SWELLING,			//ブースター(青)：膨らむ
	MIXOBJ_ANMSEQ_BOOSTER_1P_SHRIVEL,			//ブースター(青)：しぼむ
	MIXOBJ_ANMSEQ_BOOSTER_1P_WEAK_JUMP,			//ブースター(青:弱)：ジャンプ
	MIXOBJ_ANMSEQ_BOOSTER_1P_WEAK_SWELLING,		//ブースター(青:弱)：膨らむ
	MIXOBJ_ANMSEQ_BOOSTER_1P_WEAK_SHRIVEL,		//ブースター(青:弱)：しぼむ
	MIXOBJ_ANMSEQ_BOOSTER_1P_HARD_JUMP,			//ブースター(青:強)：ジャンプ
	MIXOBJ_ANMSEQ_BOOSTER_1P_HARD_SWELLING,		//ブースター(青:強)：膨らむ
	MIXOBJ_ANMSEQ_BOOSTER_1P_HARD_SHRIVEL,		//ブースター(青:強)：しぼむ

	MIXOBJ_ANMSEQ_BOOSTER_HIT_EFF = 33,			//ブースターヒットエフェクト
	MIXOBJ_ANMSEQ_BOOSTER_SHADOW = 34,			//ブースター影
	MIXOBJ_ANMSEQ_BOOSTER_SHADOW_SMALL,
	MIXOBJ_ANMSEQ_BOOSTER_SHADOW_BIG,
	MIXOBJ_ANMSEQ_BOOSTER_LAND_SMOKE = 27,			//ブースターヒットエフェクト

	MIXOBJ_ANMSEQ_KAMI_STORM_GREEN = 21,		//紙ふぶき(緑)
	MIXOBJ_ANMSEQ_KAMI_STORM_RED,				//紙ふぶき(赤)
	MIXOBJ_ANMSEQ_KAMI_STORM_BLUE,				//紙ふぶき(青)
	MIXOBJ_ANMSEQ_KAMI_STORM_YELLOW,			//紙ふぶき(黄)

	MIXOBJ_ANMSEQ_EXPLODED_SMOKE_1 = 28,		//爆発の煙(小)
	MIXOBJ_ANMSEQ_EXPLODED_SMOKE_2,				//爆発の煙(中)
	MIXOBJ_ANMSEQ_EXPLODED_SMOKE_3,				//爆発の煙(大)

	MIXOBJ_ANMSEQ_JOINT_UD = 31,	//ジョイント上下
	MIXOBJ_ANMSEQ_JOINT_LR,			//ジョイント左右
};

///ブースターのアニメシーケンス数
#define BOOSTER_ANMSEQ_PATERN		(3)
///ブースターのアニメシーケンス開始位置からのオフセットID
enum{
	OFFSET_ANMID_BOOSTER_JUMP,			//ジャンプ
	OFFSET_ANMID_BOOSTER_SWELLING,		//膨らむ
	OFFSET_ANMID_BOOSTER_SHRIVEL,		//しぼむ
};

//--------------------------------------------------------------
//	パイプ蓋
//--------------------------------------------------------------
///パイプの蓋：NCG上の上蓋キャラクタ開始位置
#define PIPE_FUTA_UP_NCG_NO			(0x60)
///パイプの蓋：X幅
#define PIPE_FUTA_UP_LEN_X			(4)
///パイプの蓋：Y幅
#define PIPE_FUTA_UP_LEN_Y			(2)
///パイプの蓋：スクリーン開始位置
#define PIPE_FUTA_UP_SCRN_POS		(8*32 + 14)	//Y+X

#define PIPE_FUTA_LEFT_NCG_NO		(0x10)
#define PIPE_FUTA_LEFT_LEN_X		(2)
#define PIPE_FUTA_LEFT_LEN_Y		(4)
#define PIPE_FUTA_LEFT_SCRN_POS		(10*32 + 11)

#define PIPE_FUTA_RIGHT_NCG_NO		(0xb)
#define PIPE_FUTA_RIGHT_LEN_X		(2)
#define PIPE_FUTA_RIGHT_LEN_Y		(4)
#define PIPE_FUTA_RIGHT_SCRN_POS	(10*32 + 19)

//--------------------------------------------------------------
//	ブースター
//--------------------------------------------------------------
///ブースター：移動ふり幅X
#define BOOSTER_FURIHABA_X		(76)
///ブースター：移動ふり幅Y
#define BOOSTER_FURIHABA_Y		(64+4)
///ブースター：オフセットY
#define BOOSTER_OFFSET_Y		(-24)
///ブースター：ジャンプ移動ふり幅
#define BOOSTER_JUMP_FURIHABA_Y	(12 << FX32_SHIFT)

///ブースター：停止箇所の個数
#define BOOSTER_STOP_PART_NUM		(12)

///ブースター：ジャンプアニメの合計フレーム数(NITRO-CHARACTERで設定しているジャンプアニメの
///フレーム数の合計を書く)
#define BOOSTER_ANIME_FRAME_JUMP_TOTAL		(8+8+12+8+8)

//ブースターの登場時の、登場座標
#define BOOSTER_APPEAR_START_X_0		(128)
#define BOOSTER_APPEAR_START_Y_0		(-32)
#define BOOSTER_APPEAR_START_X_90		(256+32)
#define BOOSTER_APPEAR_START_Y_90		(96)
#define BOOSTER_APPEAR_START_X_180		(128)
#define BOOSTER_APPEAR_START_Y_180		(196+32)
#define BOOSTER_APPEAR_START_X_270		(-32)
#define BOOSTER_APPEAR_START_Y_270		(96)

///ブースターヒットエフェクト：本体からのオフセットY
#define BOOSTER_HIT_OFFSET_Y		(-32)
///ブースターヒットエフェクト：次のアニメパターンまでのフレーム数
#define BOOSTER_HITEFF_NEXT_ANM_FRAME	(4 * FX32_ONE)

///ブースター影：本体からのオフセットY
#define BOOSTER_SHADOW_OFFSET_Y		(24)
///ブースター着地の煙：本体からのオフセットY
#define BOOSTER_LAND_SMOKE_OFFSET_Y		(28)

//--------------------------------------------------------------
//	SIOブースター
//--------------------------------------------------------------
///SIOブースター登場時のオフセットY
#define SIO_BOOSTER_APPEAR_OFFSET_Y		(-96)
///SIOブースター登場時のIN速度(fx32)
#define SIO_BOOSTER_IN_SPEED		(0x08000)
///SIOブースター退場時のOUT速度(fx32)
#define SIO_BOOSTER_OUT_SPEED		(SIO_BOOSTER_IN_SPEED)

//--------------------------------------------------------------
//	タッチペンデモ
//--------------------------------------------------------------
///タッチペンデモ：ペンアクターの座標X
#define DEMO_PEN_X				(128)
///タッチペンデモ：ペンアクターの座標Y
#define DEMO_PEN_Y				(16)
///タッチペンデモ：ペンの速度
#define DEMO_PEN_SPEED			(6)
///タッチペンデモ：ペンが動き出す前のウェイト
#define DEMO_PEN_START_WAIT		(15)
///タッチペンデモ：ペンが動いた後のウェイト
#define DEMO_PEN_AFTER_WAIT		(30)

//==============================================================================
//	構造体定義
//==============================================================================
///風船レベル毎に定まっているデータ
typedef struct{
	u16 ncgr_id;		///<キャラクタアーカイブID
	u16 nscr_id;		///<スクリーンアーカイブID
	fx32 max_scale;		///<最大スケール
}BALLOON_LEVEL_DATA;

///参加人数毎に定まっているデータ
typedef struct{
	s32 max_air[BALLOON_LEVEL_MAX];	///<風船のレベル毎の耐久力
}BALLOON_NUM_PARTICIPATION_DATA;

//--------------------------------------------------------------
//	ブースター
//--------------------------------------------------------------
//何フレームで次の時刻へ移動させるか
//時刻へ到達した後、何フレーム待たせるか
//加速力(一週ごとにアップする速度
typedef struct{
	u8 move_frame;		///<次の停止箇所へ移動する時、何フレームかけて移動するか
	u8 wait_frame;		///<着地後、待つフレーム時間
	u8 hit_anime_time;	///<ヒットした時のアニメにかけるフレーム数(※必ずmove_frameよりも小さい必要がある！)
	
	u8 padding;
}BOOSTER_MOVE_DATA;

//--------------------------------------------------------------
//	破裂
//--------------------------------------------------------------
///破裂紙ふぶき(サブ画面)：プレイヤー位置毎の紙ふぶきのアニメシーケンス番号
static const u16 PlayerNoAnmSeq_Storm[][WFLBY_MINIGAME_MAX] = {
	{//0人参加(ダミー)
		MIXOBJ_ANMSEQ_KAMI_STORM_RED,
		MIXOBJ_ANMSEQ_KAMI_STORM_GREEN,
		MIXOBJ_ANMSEQ_KAMI_STORM_BLUE,
		MIXOBJ_ANMSEQ_KAMI_STORM_YELLOW,
	},
	{//1人参加	(下、左、上、右)：デバッグ用
		MIXOBJ_ANMSEQ_KAMI_STORM_RED,
		MIXOBJ_ANMSEQ_KAMI_STORM_GREEN,
		MIXOBJ_ANMSEQ_KAMI_STORM_BLUE,
		MIXOBJ_ANMSEQ_KAMI_STORM_YELLOW,
	},
	{//2人参加	(下、上)
		MIXOBJ_ANMSEQ_KAMI_STORM_RED,
		MIXOBJ_ANMSEQ_KAMI_STORM_BLUE,
	},
	{//3人参加	(下、左、右)
		MIXOBJ_ANMSEQ_KAMI_STORM_RED,
		MIXOBJ_ANMSEQ_KAMI_STORM_YELLOW,
		MIXOBJ_ANMSEQ_KAMI_STORM_BLUE,
	},
	{//4人参加	(下、左、上、右)
		MIXOBJ_ANMSEQ_KAMI_STORM_RED,
		MIXOBJ_ANMSEQ_KAMI_STORM_GREEN,
		MIXOBJ_ANMSEQ_KAMI_STORM_BLUE,
		MIXOBJ_ANMSEQ_KAMI_STORM_YELLOW,
	},
};

///破片のパレット番号	※風船レベル順
ALIGN4 static const u16 ExplodedChipPalNo[] = {
	PALOFS_SUB_EXPLODED_CHIP_BLUE,
	PALOFS_SUB_EXPLODED_CHIP_YELLOW,
	PALOFS_SUB_EXPLODED_CHIP_RED,
};

//==============================================================================
//	データ
//==============================================================================
//--------------------------------------------------------------
//	空気
//--------------------------------------------------------------
///空気アクターの出現座標(上画面)
///BalloonPlayerSortと同じ並び順(下を開始位置にして左回り)
static const AIR_POSITION_DATA AirPositionDataTbl[][WFLBY_MINIGAME_MAX] = {
	{//player_maxそのままでアクセスできるように1origin化の為のダミー
		{128, 192+16,	MIXOBJ_ANMSEQ_AIR_0, PALOFS_SUB_AIR_RED,	DIR_UP},		//下
	},
	
	{//参加人数1人(デバッグ用)
		{128, 192+16,	MIXOBJ_ANMSEQ_AIR_0, PALOFS_SUB_AIR_RED,	DIR_UP},		//下
	},
	{//参加人数2人
		{128, 192+16,	MIXOBJ_ANMSEQ_AIR_0, PALOFS_SUB_AIR_RED,	DIR_UP},		//下
		{128, 0-16,	MIXOBJ_ANMSEQ_AIR_3, PALOFS_SUB_AIR_BLUE,	DIR_DOWN},		//上
	},
	{//参加人数3人
		{128, 192+16,	MIXOBJ_ANMSEQ_AIR_0, PALOFS_SUB_AIR_RED,	DIR_UP},		//下
		{0-16, 192/2,	MIXOBJ_ANMSEQ_AIR_2, PALOFS_SUB_AIR_YELLOW,	DIR_RIGHT},		//左
		{256+16, 192/2,	MIXOBJ_ANMSEQ_AIR_1, PALOFS_SUB_AIR_BLUE,	DIR_LEFT},	//右
	},
	{//参加人数4人
		{128, 192+16,	MIXOBJ_ANMSEQ_AIR_0, PALOFS_SUB_AIR_RED,	DIR_UP},		//下
		{0-16, 192/2,	MIXOBJ_ANMSEQ_AIR_2, PALOFS_SUB_AIR_GREEN,	DIR_RIGHT},		//左
		{128, 0-16,	MIXOBJ_ANMSEQ_AIR_3, PALOFS_SUB_AIR_BLUE,	DIR_DOWN},		//上
		{256, 192/2,	MIXOBJ_ANMSEQ_AIR_1, PALOFS_SUB_AIR_YELLOW,	DIR_LEFT},	//右
	},
};

///空気のアクターヘッダ(サブ画面用)
static const TCATS_OBJECT_ADD_PARAM_S AirObjParam = {
	0, 0, 0,		//x, y, z
	0, BALLOONSUB_SOFTPRI_AIR, PALOFS_SUB_AIR_BLUE,	//アニメ番号、優先順位、パレット番号
	NNS_G2D_VRAM_TYPE_2DSUB,		//描画エリア
	{	//使用リソースIDテーブル
		CHARID_SUB_BALLOON_MIX,			//キャラ
		PLTTID_SUB_OBJ_COMMON,			//パレット
		CELLID_SUB_BALLOON_MIX,			//セル
		CELLANMID_SUB_BALLOON_MIX,		//セルアニメ
		CLACT_U_HEADER_DATA_NONE,		//マルチセル
		CLACT_U_HEADER_DATA_NONE,		//マルチセルアニメ
	},
	BALLOONSUB_BGPRI_AIR,				//BGプライオリティ
	0,									//Vram転送フラグ
};

///紙ふぶきのアクターヘッダ(サブ画面用)
static const TCATS_OBJECT_ADD_PARAM_S ExplodedStormObjParam = {
	BALLOON_APPEAR_POS_X, BALLOON_APPEAR_POS_Y, 0,		//x, y, z
	0, BALLOONSUB_SOFTPRI_EXPLODED_STORM, PALOFS_SUB_EXPLODED,//アニメ番号、優先順位、パレット番号
	NNS_G2D_VRAM_TYPE_2DSUB,		//描画エリア
	{	//使用リソースIDテーブル
		CHARID_SUB_BALLOON_MIX,			//キャラ
		PLTTID_SUB_OBJ_COMMON,			//パレット
		CELLID_SUB_BALLOON_MIX,			//セル
		CELLANMID_SUB_BALLOON_MIX,		//セルアニメ
		CLACT_U_HEADER_DATA_NONE,		//マルチセル
		CLACT_U_HEADER_DATA_NONE,		//マルチセルアニメ
	},
	BALLOONSUB_BGPRI_EXPLODED,				//BGプライオリティ
	0,									//Vram転送フラグ
};

///風船アイコンのアクターヘッダ(サブ画面用)
static const TCATS_OBJECT_ADD_PARAM_S IconBalloonObjParam = {
	0, 0, 0,		//x, y, z
	0, BALLOONSUB_SOFTPRI_ICON_BALLOON,PALOFS_SUB_ICON_BALLOON,//アニメ番号、優先順位、パレット番号
	NNS_G2D_VRAM_TYPE_2DSUB,		//描画エリア
	{	//使用リソースIDテーブル
		CHARID_SUB_BALLOON_MIX,			//キャラ
		PLTTID_SUB_OBJ_COMMON,			//パレット
		CELLID_SUB_BALLOON_MIX,			//セル
		CELLANMID_SUB_BALLOON_MIX,		//セルアニメ
		CLACT_U_HEADER_DATA_NONE,		//マルチセル
		CLACT_U_HEADER_DATA_NONE,		//マルチセルアニメ
	},
	BALLOONSUB_BGPRI_ICON_BALLOON,				//BGプライオリティ
	0,									//Vram転送フラグ
};

///ジョイントのアクターヘッダ(サブ画面用)
static const TCATS_OBJECT_ADD_PARAM_S JointObjParam = {
	0, 0, 0,		//x, y, z
	0, BALLOONSUB_SOFTPRI_JOINT, PALOFS_SUB_JOINT,//アニメ番号、優先順位、パレット番号
	NNS_G2D_VRAM_TYPE_2DSUB,		//描画エリア
	{	//使用リソースIDテーブル
		CHARID_SUB_BALLOON_MIX,			//キャラ
		PLTTID_SUB_OBJ_COMMON,			//パレット
		CELLID_SUB_BALLOON_MIX,			//セル
		CELLANMID_SUB_BALLOON_MIX,		//セルアニメ
		CLACT_U_HEADER_DATA_NONE,		//マルチセル
		CLACT_U_HEADER_DATA_NONE,		//マルチセルアニメ
	},
	BALLOONSUB_BGPRI_JOINT,				//BGプライオリティ
	0,									//Vram転送フラグ
};

///ブースター(ソーナノ)のアクターヘッダ(サブ画面用)
static const TCATS_OBJECT_ADD_PARAM_S BoosterObjParam = {
	0, 0, 0,		//x, y, z
	0, BALLOONSUB_SOFTPRI_BOOSTER, PALOFS_SUB_BOOSTER_RED,//アニメ番号、優先順位、パレット番号
	NNS_G2D_VRAM_TYPE_2DSUB,		//描画エリア
	{	//使用リソースIDテーブル
		CHARID_SUB_BALLOON_MIX,			//キャラ
		PLTTID_SUB_OBJ_COMMON,			//パレット
		CELLID_SUB_BALLOON_MIX,			//セル
		CELLANMID_SUB_BALLOON_MIX,		//セルアニメ
		CLACT_U_HEADER_DATA_NONE,		//マルチセル
		CLACT_U_HEADER_DATA_NONE,		//マルチセルアニメ
	},
	BALLOONSUB_BGPRI_BOOSTER,				//BGプライオリティ
	0,									//Vram転送フラグ
};

///ブースターヒット時に出るエフェクトのアクターヘッダ(サブ画面用)
static const TCATS_OBJECT_ADD_PARAM_S BoosterHitObjParam = {
	0, 0, 0,		//x, y, z
	0, BALLOONSUB_SOFTPRI_BOOSTER_HIT, PALOFS_SUB_BOOSTER_HIT,//アニメ番号、優先順位、パレット番号
	NNS_G2D_VRAM_TYPE_2DSUB,		//描画エリア
	{	//使用リソースIDテーブル
		CHARID_SUB_BALLOON_MIX,			//キャラ
		PLTTID_SUB_OBJ_COMMON,			//パレット
		CELLID_SUB_BALLOON_MIX,			//セル
		CELLANMID_SUB_BALLOON_MIX,		//セルアニメ
		CLACT_U_HEADER_DATA_NONE,		//マルチセル
		CLACT_U_HEADER_DATA_NONE,		//マルチセルアニメ
	},
	BALLOONSUB_BGPRI_BOOSTER,				//BGプライオリティ
	0,									//Vram転送フラグ
};

///ブースターの影のアクターヘッダ(サブ画面用)
static const TCATS_OBJECT_ADD_PARAM_S BoosterShadowObjParam = {
	0, 0, 0,		//x, y, z
	0, BALLOONSUB_SOFTPRI_BOOSTER_SHADOW, PALOFS_SUB_BOOSTER_SHADOW,//アニメ番号、優先順位、パレット番号
	NNS_G2D_VRAM_TYPE_2DSUB,		//描画エリア
	{	//使用リソースIDテーブル
		CHARID_SUB_BALLOON_MIX,			//キャラ
		PLTTID_SUB_OBJ_COMMON,			//パレット
		CELLID_SUB_BALLOON_MIX,			//セル
		CELLANMID_SUB_BALLOON_MIX,		//セルアニメ
		CLACT_U_HEADER_DATA_NONE,		//マルチセル
		CLACT_U_HEADER_DATA_NONE,		//マルチセルアニメ
	},
	BALLOONSUB_BGPRI_BOOSTER,				//BGプライオリティ
	0,									//Vram転送フラグ
};

///ブースターの着地の煙のアクターヘッダ(サブ画面用)
static const TCATS_OBJECT_ADD_PARAM_S BoosterLandSmokeObjParam = {
	0, 0, 0,		//x, y, z
	0, BALLOONSUB_SOFTPRI_BOOSTER_LAND_SMOKE, PALOFS_SUB_BOOSTER_LAND_SMOKE,//アニメ番号、優先順位、パレット番号
	NNS_G2D_VRAM_TYPE_2DSUB,		//描画エリア
	{	//使用リソースIDテーブル
		CHARID_SUB_BALLOON_MIX,			//キャラ
		PLTTID_SUB_OBJ_COMMON,			//パレット
		CELLID_SUB_BALLOON_MIX,			//セル
		CELLANMID_SUB_BALLOON_MIX,		//セルアニメ
		CLACT_U_HEADER_DATA_NONE,		//マルチセル
		CLACT_U_HEADER_DATA_NONE,		//マルチセルアニメ
	},
	BALLOONSUB_BGPRI_BOOSTER,				//BGプライオリティ
	0,									//Vram転送フラグ
};

///カウンターウィンドウのアクターヘッダ(メイン画面用)
static const TCATS_OBJECT_ADD_PARAM_S CounterWindowObjParam = {
	8*5, 192-16, 0,		//x, y, z
	0, BALLOON_SOFTPRI_COUNTER_WIN, 0,//アニメ番号、優先順位、パレット番号
	NNS_G2D_VRAM_TYPE_2DMAIN,		//描画エリア
	{	//使用リソースIDテーブル
		CHARID_COUNTER_WIN,			//キャラ
		PLTTID_COUNTER_WIN,			//パレット
		CELLID_COUNTER_WIN,			//セル
		CELLANMID_COUNTER_WIN,		//セルアニメ
		CLACT_U_HEADER_DATA_NONE,		//マルチセル
		CLACT_U_HEADER_DATA_NONE,		//マルチセルアニメ
	},
	BALLOON_BGPRI_COUNTER,				//BGプライオリティ
	0,									//Vram転送フラグ
};

///タッチペンのアクターヘッダ(メイン画面用)
static const TCATS_OBJECT_ADD_PARAM_S TouchPenObjParam = {
	0, 0, 0,		//x, y, z
	0, BALLOON_SOFTPRI_TOUCH_PEN, 0,//アニメ番号、優先順位、パレット番号
	NNS_G2D_VRAM_TYPE_2DMAIN,		//描画エリア
	{	//使用リソースIDテーブル
		CHARID_TOUCH_PEN,			//キャラ
		PLTTID_TOUCH_PEN,			//パレット
		CELLID_TOUCH_PEN,			//セル
		CELLANMID_TOUCH_PEN,		//セルアニメ
		CLACT_U_HEADER_DATA_NONE,		//マルチセル
		CLACT_U_HEADER_DATA_NONE,		//マルチセルアニメ
	},
	BALLOON_BGPRI_TOUCH_PEN,				//BGプライオリティ
	0,									//Vram転送フラグ
};

//--------------------------------------------------------------
//	カラースワップ
//--------------------------------------------------------------
///名前ウィンドウのパレット番号
enum{
	NAME_WIN_PALNO_RED = 1,
	NAME_WIN_PALNO_BLUE = 0,
	NAME_WIN_PALNO_YELLOW = 3,
	NAME_WIN_PALNO_GREEN = 2,
};

//デフォルトパレットでの2Pのカラー開始位置
#define PIPE_BG_COLOR_START_NO_RED		(1*16 + 0xd)
//デフォルトパレットでの1Pのカラー開始位置
#define PIPE_BG_COLOR_START_NO_BLUE		(0*16 + 0xd)
//デフォルトパレットでの4Pのカラー開始位置
#define PIPE_BG_COLOR_START_NO_YELLOW		(3*16 + 0xd)
//デフォルトパレットでの3Pのカラー開始位置
#define PIPE_BG_COLOR_START_NO_GREEN		(2*16 + 0xd)

///パイプBG：プレイヤー位置毎のパレットカラー開始位置
static const u16 PlayerNoPaletteSwapTbl_Pipe[][WFLBY_MINIGAME_MAX] = {
	{//0人参加(ダミー)
		PIPE_BG_COLOR_START_NO_RED,
		PIPE_BG_COLOR_START_NO_GREEN,
		PIPE_BG_COLOR_START_NO_BLUE,
		PIPE_BG_COLOR_START_NO_YELLOW,
	},
	{//1人参加	(下、左、上、右)：デバッグ用
		PIPE_BG_COLOR_START_NO_RED,
		PIPE_BG_COLOR_START_NO_GREEN,
		PIPE_BG_COLOR_START_NO_BLUE,
		PIPE_BG_COLOR_START_NO_YELLOW,
	},
	{//2人参加	(下、上)
		PIPE_BG_COLOR_START_NO_RED,
		PIPE_BG_COLOR_START_NO_BLUE,
	},
	{//3人参加	(下、左、右)
		PIPE_BG_COLOR_START_NO_RED,
		PIPE_BG_COLOR_START_NO_YELLOW,
		PIPE_BG_COLOR_START_NO_BLUE,
	},
	{//4人参加	(下、左、上、右)
		PIPE_BG_COLOR_START_NO_RED,
		PIPE_BG_COLOR_START_NO_GREEN,
		PIPE_BG_COLOR_START_NO_BLUE,
		PIPE_BG_COLOR_START_NO_YELLOW,
	},
};
///パイプBG：プレイヤーカラーがパレット開始位置からいくつ入っているか
#define PLAYER_NO_PALETTE_COLOR_NUM_PIPE		(3)

///パイプBG：プレイヤー位置毎のスワップしたパレット書き込み位置
static const u16 PlayerNoPaletteWritePosTbl_Pipe[WFLBY_MINIGAME_MAX] = {
	PIPE_BG_COLOR_START_NO_RED,
	PIPE_BG_COLOR_START_NO_BLUE,
	PIPE_BG_COLOR_START_NO_YELLOW,
	PIPE_BG_COLOR_START_NO_GREEN,
};

//デフォルトパレットでの2Pのカラー開始位置
#define OBJ_COLOR_START_NO_RED		(2*16 + 0)
//デフォルトパレットでの1Pのカラー開始位置
#define OBJ_COLOR_START_NO_BLUE		(1*16 + 0)
//デフォルトパレットでの4Pのカラー開始位置
#define OBJ_COLOR_START_NO_YELLOW		(3*16 + 0)
//デフォルトパレットでの3Pのカラー開始位置
#define OBJ_COLOR_START_NO_GREEN		(4*16 + 0)

///プレイヤーOBJ(サブ画面)：プレイヤー位置毎のパレットカラー開始位置(PLTTID_SUB_OBJ_COMMONからのoffset)
static const u16 PlayerNoPaletteSwapTbl_Obj[][WFLBY_MINIGAME_MAX] = {
	{//0人参加(ダミー)
		OBJ_COLOR_START_NO_RED,
		OBJ_COLOR_START_NO_BLUE,
		OBJ_COLOR_START_NO_YELLOW,
		OBJ_COLOR_START_NO_GREEN,
	},
	{//1人参加	(下、左、上、右)：デバッグ用
		OBJ_COLOR_START_NO_RED,
		OBJ_COLOR_START_NO_BLUE,
		OBJ_COLOR_START_NO_YELLOW,
		OBJ_COLOR_START_NO_GREEN,
	},
	{//2人参加	(下、上)
		OBJ_COLOR_START_NO_RED,
		OBJ_COLOR_START_NO_BLUE,
	},
	{//3人参加	(下、左、右)
		OBJ_COLOR_START_NO_RED,
		OBJ_COLOR_START_NO_YELLOW,
		OBJ_COLOR_START_NO_BLUE,
	},
	{//4人参加	(下、左、上、右)
		OBJ_COLOR_START_NO_RED,
		OBJ_COLOR_START_NO_GREEN,
		OBJ_COLOR_START_NO_BLUE,
		OBJ_COLOR_START_NO_YELLOW,
	},
};
///プレイヤーOBJ：プレイヤーカラーがパレット開始位置からいくつ入っているか
#define PLAYER_NO_PALETTE_COLOR_NUM_AIR		(16)

///プレイヤーOBJ：プレイヤー位置毎のスワップしたパレット書き込み位置
static const u16 PlayerNoPaletteWritePosTbl_Obj[WFLBY_MINIGAME_MAX] = {
	OBJ_COLOR_START_NO_RED,
	OBJ_COLOR_START_NO_BLUE,
	OBJ_COLOR_START_NO_YELLOW,
	OBJ_COLOR_START_NO_GREEN,
};


///エアーOBJのスケール値(ブースタータイプ順)
static const fx32 Air_BoosterScale[] = {
	FX32_ONE, 			//BOOSTER_TYPE_NONE
	0x1800,				//BOOSTER_TYPE_NORMAL
	0xa00,				//BOOSTER_TYPE_WEAK
	0x2000,				//BOOSTER_TYPE_HARD
};

//デフォルトパレットでの2Pのカラー開始位置
#define STORM_COLOR_START_NO_RED		(5*16 + 0x2)
//デフォルトパレットでの1Pのカラー開始位置
#define STORM_COLOR_START_NO_BLUE		(5*16 + 0x1)
//デフォルトパレットでの4Pのカラー開始位置
#define STORM_COLOR_START_NO_YELLOW		(5*16 + 0x3)
//デフォルトパレットでの3Pのカラー開始位置
#define STORM_COLOR_START_NO_GREEN		(5*16 + 0x4)

///紙ふぶき：プレイヤー位置毎のパレットカラー開始位置
static const u16 PlayerNoPaletteSwapTbl_Storm[][WFLBY_MINIGAME_MAX] = {
	{//0人参加(ダミー)
		STORM_COLOR_START_NO_RED,
		STORM_COLOR_START_NO_BLUE,
		STORM_COLOR_START_NO_YELLOW,
		STORM_COLOR_START_NO_GREEN,
	},
	{//1人参加	(下、左、上、右)：デバッグ用
		STORM_COLOR_START_NO_RED,
		STORM_COLOR_START_NO_BLUE,
		STORM_COLOR_START_NO_YELLOW,
		STORM_COLOR_START_NO_GREEN,
	},
	{//2人参加	(下、上)
		STORM_COLOR_START_NO_RED,
		STORM_COLOR_START_NO_BLUE,
	},
	{//3人参加	(下、左、右)
		STORM_COLOR_START_NO_RED,
		STORM_COLOR_START_NO_YELLOW,
		STORM_COLOR_START_NO_BLUE,
	},
	{//4人参加	(下、左、上、右)
		STORM_COLOR_START_NO_RED,
		STORM_COLOR_START_NO_GREEN,
		STORM_COLOR_START_NO_BLUE,
		STORM_COLOR_START_NO_YELLOW,
	},
};
///パイプBG：プレイヤーカラーがパレット開始位置からいくつ入っているか
#define PLAYER_NO_PALETTE_COLOR_NUM_STORM		(1)

///紙ふぶき：プレイヤー位置毎のスワップしたパレット書き込み位置
static const u16 PlayerNoPaletteWritePosTbl_Storm[WFLBY_MINIGAME_MAX] = {
	STORM_COLOR_START_NO_RED,
	STORM_COLOR_START_NO_BLUE,
	STORM_COLOR_START_NO_YELLOW,
	STORM_COLOR_START_NO_GREEN,
};

//--------------------------------------------------------------
//	ジョイント
//--------------------------------------------------------------
///ジョイントのアクター座標
static const struct{
	s16 x;
	s16 y;
}JointActorPosTbl[] = {
	{128, 164},		//JOINT_ACTOR_D
	{128, 28},		//JOINT_ACTOR_U
	{52, 96},		//JOINT_ACTOR_L
	{204, 96},		//JOINT_ACTOR_R
};

//--------------------------------------------------------------
//	ブースター
//--------------------------------------------------------------
///ブースタータイプ毎のアニメシーケンス開始番号
static const u16 BoosterType_StartSeqAnimeNo[] = {
	MIXOBJ_ANMSEQ_BOOSTER_1P_JUMP,		//NONE
	MIXOBJ_ANMSEQ_BOOSTER_1P_JUMP,
	MIXOBJ_ANMSEQ_BOOSTER_1P_WEAK_JUMP,
	MIXOBJ_ANMSEQ_BOOSTER_1P_HARD_JUMP,
};

///ブースターOBJ(サブ画面)：プレイヤー位置毎のブースターパレットオフセット番号
static const u16 PlayerNoPalOfs_Booster[][WFLBY_MINIGAME_MAX] = {
	{//0人参加(ダミー)
		PALOFS_SUB_BOOSTER_RED,
		PALOFS_SUB_BOOSTER_GREEN,
		PALOFS_SUB_BOOSTER_BLUE,
		PALOFS_SUB_BOOSTER_YELLOW,
	},
	{//1人参加	(下、左、上、右)：デバッグ用
		PALOFS_SUB_BOOSTER_RED,
		PALOFS_SUB_BOOSTER_GREEN,
		PALOFS_SUB_BOOSTER_BLUE,
		PALOFS_SUB_BOOSTER_YELLOW,
	},
	{//2人参加	(下、上)
		PALOFS_SUB_BOOSTER_RED,
		PALOFS_SUB_BOOSTER_BLUE,
	},
	{//3人参加	(下、左、右)
		PALOFS_SUB_BOOSTER_RED,
		PALOFS_SUB_BOOSTER_YELLOW,
		PALOFS_SUB_BOOSTER_BLUE,
	},
	{//4人参加	(下、左、上、右)
		PALOFS_SUB_BOOSTER_RED,
		PALOFS_SUB_BOOSTER_GREEN,
		PALOFS_SUB_BOOSTER_BLUE,
		PALOFS_SUB_BOOSTER_YELLOW,
	},
};

///SIOブースターOBJ(サブ画面)：プレイヤー位置毎のジョイント対応番号
static const u8 PlayerNoJointNo_SioBooster[][WFLBY_MINIGAME_MAX] = {
	{//player_maxそのままでアクセスできるように1origin化の為のダミー
		JOINT_ACTOR_D,
	},
	{//1人参加	(下、左、上、右)：デバッグ用
		JOINT_ACTOR_D,
		JOINT_ACTOR_L,
		JOINT_ACTOR_U,
		JOINT_ACTOR_R,
	},
	{//2人参加	(下、上)
		JOINT_ACTOR_D,
		JOINT_ACTOR_U,
	},
	{//3人参加	(下、左、右)
		JOINT_ACTOR_D,
		JOINT_ACTOR_L,
		JOINT_ACTOR_R,
	},
	{//4人参加	(下、左、上、右)
		JOINT_ACTOR_D,
		JOINT_ACTOR_L,
		JOINT_ACTOR_U,
		JOINT_ACTOR_R,
	},
};


//==============================================================================
//	外部データ
//==============================================================================
#include "balloon_control.dat"


//==============================================================================
//	プロトタイプ宣言
//==============================================================================
static void BalloonTool_BalloonAffineScaleUpdate(
	GF_BGL_INI *bgl, BALLOON_STATUS *bst, int player_max);
int BalloonTool_AirParcentGet(BALLOON_GAME_PTR game);
static void BalloonTool_BalloonAffineParamGet(
	int player_max, int balloon_lv, int air, fx32 *scale_x, fx32 *scale_y);
static int BalloonTool_AirStackAdd(BALLOON_GAME_PTR game, BALLOON_STATUS *bst, 
	int air, int net_id, int booster_type);
static int Air_ParamCreate(BALLOON_GAME_PTR game, const BALLOON_AIR_DATA * air_data, PLAYER_AIR_PARAM *air_param);
static BOOL Air_Move(BALLOON_GAME_PTR game, PLAYER_AIR_PARAM *air_param);
static CATS_ACT_PTR Air_ActorCreate(BALLOON_GAME_PTR game, const AIR_POSITION_DATA *air_posdata);
static void Exploded_ActorCreate(BALLOON_GAME_PTR game, EXPLODED_PARAM *exploded, BALLOON_STATUS *bst);
static void BalloonTool_BalloonBGErase(GF_BGL_INI *bgl, BALLOON_STATUS *bst);
static CATS_ACT_PTR IconBalloon_ActorCreate(BALLOON_GAME_PTR game, int icon_type, int pos);
void IconBalloon_AllCreate(BALLOON_GAME_PTR game);
BOOL IconBalloon_Update(BALLOON_GAME_PTR game);
static BOOL BalloonTool_IconBalloonParamAdd(BALLOON_GAME_PTR game, ICONBALLOON_PARAM *ibp, int pos, int balloon_no);
static void IconBalloon_PosMove(ICONBALLOON_PARAM *ibp);
static void IconBalloon_StatusUpdate(BALLOON_GAME_PTR game, ICONBALLOON_PARAM *ibp);
static int IconBalloon_TypeGet(int balloon_no);
static void IconBalloon_ExplodedReq(BALLOON_GAME_PTR game);
static void Booster_Move(BALLOON_GAME_PTR game, BOOSTER_WORK *booster, BOOSTER_MOVE *move);
static void BoosterMoveModeChange(BOOSTER_MOVE *move, int mode);
static int BoosterMove_Wait(BALLOON_GAME_PTR game, BOOSTER_WORK *booster, BOOSTER_MOVE *move);
static int BoosterMove_Appear(BALLOON_GAME_PTR game, BOOSTER_WORK *booster, BOOSTER_MOVE *move);
static int BoosterMove_Normal(BALLOON_GAME_PTR game, BOOSTER_WORK *booster, BOOSTER_MOVE *move);
static int BoosterMove_Hit(BALLOON_GAME_PTR game, BOOSTER_WORK *booster, BOOSTER_MOVE *move);
static int Booster_HitCheckNow(BALLOON_GAME_PTR game, BOOSTER_MOVE **ret_move);
static BOOL SioBoosterMove_Appear(SIO_BOOSTER_WORK *sio_booster, SIO_BOOSTER_MOVE *sio_move);
static void Balloon_CounterLastNumberSet(BALLOON_COUNTER *counter);
static BOOL Balloon_CounterAgreeCheck(BALLOON_COUNTER *counter);
static BOOL Balloon_CounterRotate(BALLOON_COUNTER *counter);
static void BoosterLandSmoke_ActorCreate(BALLOON_GAME_PTR game, BOOSTER_LAND_SMOKE *land_smoke);
static void BoosterLandSmoke_ActorDelete(BALLOON_GAME_PTR game, BOOSTER_LAND_SMOKE *land_smoke);
static void BoosterLandSmoke_SmokeReq(BALLOON_GAME_PTR game, BOOSTER_MOVE *move, BOOSTER_LAND_SMOKE *land_smoke);
static void BoosterLandSmoke_Update(BALLOON_GAME_PTR game, BOOSTER_MOVE *move, BOOSTER_LAND_SMOKE *land_smoke);


//==============================================================================
//	シーケンステーブル
//==============================================================================
///ブースターの動作モード	※BoosterMoveSeqTblと並びを同じにしておくこと！
enum{
	BOOSTER_MODE_WAIT,			///<待機中
	BOOSTER_MODE_APPEAR,		///<登場移動
	BOOSTER_MODE_NORMAL,		///<通常
	BOOSTER_MODE_HIT,			///<ヒット
};

///ブースター動作関数のシーケンステーブル	※BOOSTER_MODE_???と並びを同じにしておくこと！
static int (* const BoosterMoveSeqTbl[])(BALLOON_GAME_PTR, BOOSTER_WORK *, BOOSTER_MOVE *) = {
	BoosterMove_Wait,
	BoosterMove_Appear,
	BoosterMove_Normal,
	BoosterMove_Hit,
};



//--------------------------------------------------------------
/**
 * @brief   風船BGを展開する
 *
 * @param   bgl				BGシステムへのポインタ
 * @param   player_max		参加人数
 * @param   level			風船のレベル(BALLOON_LEVEL_???)
 * @param   bst				展開した風船のパラメータセット先
 */
//--------------------------------------------------------------
void BalloonTool_BalloonBGSet(GF_BGL_INI *bgl, int player_max, int level, BALLOON_STATUS *bst)
{
	ARCHANDLE* hdl;

	OS_TPrintf("balloon level = %d\n", level);
	
	//ハンドルオープン
	hdl  = ArchiveDataHandleOpen(ARC_BALLOON_GRA,  HEAPID_BALLOON); 

	ArcUtil_HDL_BgCharSet(hdl, BalloonLevelData[level].ncgr_id, bgl, 
		BALLOON_SUBFRAME_BALLOON, 0, 0, 0, HEAPID_BALLOON);
	ArcUtil_HDL_ScrnSet(hdl, BalloonLevelData[level].nscr_id, bgl, 
		BALLOON_SUBFRAME_BALLOON, 0, 0, 0, HEAPID_BALLOON);

	//ハンドル閉じる
	ArchiveDataHandleClose( hdl );
	
	
	//ステータスパラメータセット
	MI_CpuClear8(bst, sizeof(BALLOON_STATUS));
	bst->max_air = BalloonParicipationData[player_max].max_air[level];
	bst->level = level;
	bst->occ = TRUE;
	bst->bg_on_req = TRUE;
	
	//拡縮率設定
	BalloonTool_BalloonAffineScaleUpdate(bgl, bst, player_max);
	
	Snd_SePlay(SE_BALLOON_APPEAR);
}

//--------------------------------------------------------------
/**
 * @brief   風船BGを消去する
 *
 * @param   bgl		BGシステムへのポインタ
 * @param   bst		風船パラメータへのポインタ
 */
//--------------------------------------------------------------
static void BalloonTool_BalloonBGErase(GF_BGL_INI *bgl, BALLOON_STATUS *bst)
{
	GF_BGL_ScrClear(bgl, BALLOON_SUBFRAME_BALLOON);
	bst->occ = FALSE;
	bst->bg_off_req = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   風船の拡縮率を現在の空気量で更新
 *
 * @param   bgl			BGシステムへのポインタ
 * @param   bst			風船パラメータへのポインタ
 * @param   player_max	参加人数
 */
//--------------------------------------------------------------
static void BalloonTool_BalloonAffineScaleUpdate(GF_BGL_INI *bgl, BALLOON_STATUS *bst, int player_max)
{
	fx32 scale_x, scale_y;
	int x_offset, y_offset;
	
	if(bst->occ == FALSE){
		return;
	}
	
	BalloonTool_BalloonAffineParamGet(player_max, bst->level, bst->air, &scale_x, &scale_y);
	
	//BGの表示を中心を維持する為、スケール値に応じてX,Y座標のオフセットを足しこむ
	x_offset = 0;
	y_offset = 0;
	if(scale_x == FX32_ONE){
		x_offset = 0;
	}
	else if(scale_x < FX32_ONE){
		x_offset = 128 - (128 * scale_x / FX32_ONE);
	}
	else{// if(scale_x > FX32_ONE){
		x_offset = -(128 * (scale_x - FX32_ONE) / FX32_ONE);
	}

	if(scale_y == FX32_ONE){
		y_offset = 0;
	}
	else if(scale_y < FX32_ONE){
		y_offset = 128 - (128 * scale_y / FX32_ONE);
	}
	else{// if(scale_y > Fy32_ONE){
		y_offset = -(128 * (scale_y - FX32_ONE) / FX32_ONE);
	}
	
#if 1
	{
		MtxFx22 mtx22;
	    fx32 rScale_x,rScale_y;
		int shake_x;
		
		if(bst->air_stack == 0){
			shake_x = 0;
		}
		else{
			//shake_x = bst->air % 8;
			shake_x = (gf_rand() & 7) + 1;
			if(bst->shake_flag & 1){
				shake_x = -shake_x;
			}
			bst->shake_flag ^= 1;
		}
		
		rScale_x = FX_Inv(scale_x);
		rScale_y = FX_Inv(scale_y);

		GF_BGL_ScaleSetReq(bgl, BALLOON_SUBFRAME_BALLOON, GF_BGL_SCALE_X_SET, rScale_x);
		GF_BGL_ScaleSetReq(bgl, BALLOON_SUBFRAME_BALLOON, GF_BGL_SCALE_Y_SET, rScale_y);

		GF_BGL_ScrollReq(bgl, BALLOON_SUBFRAME_BALLOON, GF_BGL_SCROLL_X_SET, 0-x_offset + shake_x);
		GF_BGL_ScrollReq(bgl, BALLOON_SUBFRAME_BALLOON, GF_BGL_SCROLL_Y_SET, (256-192)/2 + 7 - y_offset);
	}
#else
	{
		MtxFx22 mtx22;
	    fx32 rScale_x,rScale_y;
		
		rScale_x = FX_Inv(scale_x);
		rScale_y = FX_Inv(scale_y);
	    mtx22._00 = rScale_x;
	    mtx22._01 = 0;
	    mtx22._10 = 0;
	    mtx22._11 = rScale_y;
		
		GF_BGL_AffineScrollSet(bgl, BALLOON_SUBFRAME_BALLOON, GF_BGL_SCROLL_X_SET, 0 - x_offset, &mtx22, 0, 0);
		GF_BGL_AffineScrollSet(bgl, BALLOON_SUBFRAME_BALLOON, GF_BGL_SCROLL_Y_SET, (256-192)/2 - y_offset, &mtx22, 0, 0);
	}
#endif
}

//--------------------------------------------------------------
/**
 * @brief   風船の現在の空気量から風船BGの拡縮率を算出する
 * @param   air		風船の現在空気量
 * @retval  拡縮率
 */
//--------------------------------------------------------------
static void BalloonTool_BalloonAffineParamGet(int player_max, int balloon_lv, int air, fx32 *scale_x, fx32 *scale_y)
{
	fx32 scale;
	
	scale = BalloonLevelData[balloon_lv].max_scale * air / BalloonParicipationData[player_max].max_air[balloon_lv];
	scale += BALLOON_SCALE_MIN;
	
	*scale_x = scale;
	*scale_y = scale;
	
	//ある程度大きくなった所で風船を歪ませる為、X方向のスケールを大きくする
	if(scale > BALLOON_SCALE_DISTORTION_START){
		*scale_x += FX_Mul((scale - BALLOON_SCALE_DISTORTION_START), BALLOON_SCALE_DISTORTION_X);
	}
	
//	scale = BALLOON_SCALE_MAX - scale;	//数字が小さい方が大きくなるので反転
//	return scale;
}

//--------------------------------------------------------------
/**
 * @brief   風船番号から風船のレベルを取得する
 * @param   balloon_no		風船番号
 * @retval  風船のレベル(BALLOON_LEVEL_???)
 */
//--------------------------------------------------------------
int BalloonTool_BalloonLevelGet(int balloon_no)
{
	if(BALLOON_LEVEL3_NUM <= balloon_no){
		return BALLOON_LEVEL_3;
	}
	else if(BALLOON_LEVEL2_NUM <= balloon_no){
		return BALLOON_LEVEL_2;
	}
	return BALLOON_LEVEL_1;
}

//--------------------------------------------------------------
/**
 * @brief   風船に注入する空気をスタックする
 *
 * @param   bst		風船パラメータへのポインタ
 * @param   air		加算する空気量
 * @param   net_id	ネットID
 * @param   booster_type	ブースタータイプ
 *
 * @retval  加算後のスタック空気量
 */
//--------------------------------------------------------------
static int BalloonTool_AirStackAdd(BALLOON_GAME_PTR game, BALLOON_STATUS *bst, int air, int net_id, int booster_type)
{
	bst->air_stack += air;
	bst->add_air = bst->air_stack / AIR_ADD_FRAME;
	bst->player_air[Balloon_NetID_to_PlayerPos(game, net_id)] += air;
	OS_TPrintf("追加されたair = %d, add_air = %d, stack = %d, net_id = %d, booster_type = %d, main_frame = %d(%dsec)\n", air, bst->add_air, bst->air_stack, net_id, booster_type, game->main_frame, game->main_frame/30);
	
	if(net_id == CommGetCurrentID()){
		game->my_total_air += air;
		Balloon_CounterNextNumberSet(&game->counter, game->my_total_air);
		OS_TPrintf("my_total_air = %d\n", game->my_total_air);
	}
	return bst->air_stack;
}

//--------------------------------------------------------------
/**
 * @brief   風船にスタックされている空気を注入する
 *
 * @param   bst		風船パラメータへのポインタ
 *
 * @retval  TRUE:注入された。　FALSE:されていない
 */
//--------------------------------------------------------------
static int BalloonTool_AirUpdate(BALLOON_STATUS *bst)
{
	int add_air;
	
	if(bst->air_stack == 0){
		return FALSE;
	}
	
	if(bst->air_stack >= bst->add_air){
		add_air = bst->add_air;
	}
	else{
		add_air = bst->air_stack;
	}
	bst->air_stack -= add_air;
	bst->air += add_air;
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   風船更新処理
 *
 * @param   bgl		BGシステムへのポインタ
 * @param   bst		風船パラメータへのポインタ
 */
//--------------------------------------------------------------
void BalloonTool_BalloonUpdate(BALLOON_GAME_PTR game, GF_BGL_INI *bgl, BALLOON_STATUS *bst)
{
	if(game->balloon_occ == FALSE){
		return;
	}
	BalloonTool_AirUpdate(bst);
	BalloonTool_BalloonAffineScaleUpdate(bgl, bst, game->bsw->player_max);
}

//--------------------------------------------------------------
/**
 * @brief   現在の風船耐久力をパーセンテージで取得する
 *
 * @param   game		
 *
 * @retval  風船耐久力をパーセンテージ換算したもの(100%MAX)
 */
//--------------------------------------------------------------
int BalloonTool_AirParcentGet(BALLOON_GAME_PTR game)
{
	BALLOON_STATUS *bst = &game->bst;
	
	return 100 * bst->air / bst->max_air;
}

//--------------------------------------------------------------
/**
 * @brief   プレイヤーが生成した空気情報をパラメータ登録する
 *
 * @param   air_data		空気データ
 *
 * @retval  TRUE:登録成功。　FALSE:失敗
 */
//--------------------------------------------------------------
BOOL BalloonTool_PlayerAirParamAdd(BALLOON_GAME_PTR game, const BALLOON_AIR_DATA *air_data)
{
	int i, arrival_frame;
	
	if(game->balloon_no != air_data->no){
//		OS_TPrintf("違う風船番号のエアーの為、無効\n");
//		return FALSE;	//違う風船番号のエアーの為、無効
	}
	
	for(i = 0; i < PLAYER_AIR_PARAM_MAX; i++){
		if(game->air_param[i].occ == FALSE){
			arrival_frame = Air_ParamCreate(game, air_data, &game->air_param[i]);
			SioBooster_Appear(game, &game->sio_booster, 
				air_data->booster_type, air_data->net_id, arrival_frame);
			return TRUE;
		}
	}
	
	//製品版ではワークがいっぱいだった時は、そのデータは無効と見なす
	//デバッグ中はパラメータがどの程度の頻度でオーバーフローを起こすか検証する為
	//assertで止める
	GF_ASSERT(0);
	return FALSE;	//パラメータワークがいっぱいで新規追加が出来ない
}

//--------------------------------------------------------------
/**
 * @brief   空気パラメータワークを生成
 *
 * @param   game			
 * @param   air_data		空気データ
 * @param   air_param		パラメータセット先
 * 
 * @retval  ジョイント到着までのフレーム数
 */
//--------------------------------------------------------------
static int Air_ParamCreate(BALLOON_GAME_PTR game, const BALLOON_AIR_DATA * air_data, PLAYER_AIR_PARAM *air_param)
{
	int player_pos;
	const AIR_POSITION_DATA *air_posdata;
	int arrival_frame;
	
	GF_ASSERT(air_param->occ == FALSE);
	
	MI_CpuClear8(air_param, sizeof(PLAYER_AIR_PARAM));
	
	player_pos = Balloon_NetID_to_PlayerPos(game, air_data->net_id);
	air_posdata = &AirPositionDataTbl[game->bsw->player_max][player_pos];
	
	air_param->net_id = air_data->net_id;
	air_param->balloon_no = air_data->no;
	air_param->air = air_data->last_air;
	air_param->apd = air_posdata;
	air_param->booster_type = air_data->booster_type;
	air_param->cap = Air_ActorCreate(game, air_posdata);

	if(air_data->net_id == CommGetCurrentID()){
		Air3D_EntryAdd(game, air_data->air);
		air_param->wait = MY_AIR_2D_APPEAR_WAIT;
		CATS_ObjectEnableCap(air_param->cap, CATS_ENABLE_FALSE);
	}
	
	air_param->occ = TRUE;

	{//ジョイント到着までのフレーム数を返す
		s16 before_x, before_y;
		s32 before_joint_offset = 0;
		
		CATS_ObjectPosGetCap_SubSurface(
			air_param->cap, &before_x, &before_y, BALLOON_SUB_ACTOR_DISTANCE);
		switch(air_posdata->move_dir){
		case DIR_DOWN:
			before_joint_offset = JointActorPosTbl[JOINT_ACTOR_U].y - before_y;
			break;
		case DIR_LEFT:
			before_joint_offset = before_x - JointActorPosTbl[JOINT_ACTOR_R].x;
			break;
		case DIR_UP:
			before_joint_offset = before_y - JointActorPosTbl[JOINT_ACTOR_D].y;
			break;
		case DIR_RIGHT:
			before_joint_offset = JointActorPosTbl[JOINT_ACTOR_L].x - before_x;
			break;
		}
		arrival_frame = (before_joint_offset * FX32_ONE) / AIR_MOVE_SPEED;
	}
	return arrival_frame;
}

//--------------------------------------------------------------
/**
 * @brief   空気アクター更新処理
 *
 * @param   game		
 */
//--------------------------------------------------------------
void Air_Update(BALLOON_GAME_PTR game)
{
	int air_no;
	PLAYER_AIR_PARAM *air_param;
	
	air_param = game->air_param;
	for(air_no = 0; air_no < PLAYER_AIR_PARAM_MAX; air_no++){
		if(air_param->occ == TRUE){
			if(air_param->wait > 0){
				air_param->wait--;
				if(air_param->wait == 0){
					CATS_ObjectEnableCap(air_param->cap, CATS_ENABLE_TRUE);
				}
			}
			else if(Air_Move(game, air_param) == TRUE){
				if(game->balloon_occ == TRUE //&& game->balloon_no == air_param->balloon_no
						&& game->game_finish == FALSE){
					//風船番号が一致しているならば空気注入
					BalloonTool_AirStackAdd(game, 
						&game->bst, air_param->air, air_param->net_id, air_param->booster_type);
				}
				CATS_ActorPointerDelete_S(air_param->cap);
				air_param->occ = FALSE;
			}
		}
		air_param++;
	}
}

//--------------------------------------------------------------
/**
 * @brief   空気アクター移動
 *
 * @param   air_param		空気パラメータ
 *
 * @retval  TRUE:目標地点に到達
 */
//--------------------------------------------------------------
static BOOL Air_Move(BALLOON_GAME_PTR game, PLAYER_AIR_PARAM *air_param)
{
	s16 x, y, before_x, before_y;
	int del_flag = FALSE;
	s32 joint_offset, before_joint_offset;	//+はジョイント手前、-はジョイント通過後
	f32 scale_xy;
	fx32 check_scale_xy;
	
	CATS_ObjectPosGetCap_SubSurface(
		air_param->cap, &before_x, &before_y, BALLOON_SUB_ACTOR_DISTANCE);
	
	switch(air_param->apd->move_dir){
	case DIR_DOWN:
		before_joint_offset = JointActorPosTbl[JOINT_ACTOR_U].y - before_y;
		
		CATS_ObjectPosMoveCapFx32(air_param->cap, 0, AIR_MOVE_SPEED);
		CATS_ObjectPosGetCap_SubSurface(air_param->cap, &x, &y, BALLOON_SUB_ACTOR_DISTANCE);
		joint_offset = JointActorPosTbl[JOINT_ACTOR_U].y - y;
		if(y > AIR_MOVE_DOWN_DELETE_Y){
			del_flag = TRUE;
		}
		break;
	case DIR_LEFT:
		before_joint_offset = before_x - JointActorPosTbl[JOINT_ACTOR_R].x;

		CATS_ObjectPosMoveCapFx32(air_param->cap, -AIR_MOVE_SPEED, 0);
		CATS_ObjectPosGetCap_SubSurface(air_param->cap, &x, &y, BALLOON_SUB_ACTOR_DISTANCE);
		joint_offset = x - JointActorPosTbl[JOINT_ACTOR_R].x;
		if(x < AIR_MOVE_LEFT_DELETE_X){
			del_flag = TRUE;
		}
		break;
	case DIR_UP:
		before_joint_offset = before_y - JointActorPosTbl[JOINT_ACTOR_D].y;

		CATS_ObjectPosMoveCapFx32(air_param->cap, 0, -AIR_MOVE_SPEED);
		CATS_ObjectPosGetCap_SubSurface(air_param->cap, &x, &y, BALLOON_SUB_ACTOR_DISTANCE);
		joint_offset = y - JointActorPosTbl[JOINT_ACTOR_D].y;
		if(y < AIR_MOVE_UP_DELETE_Y){
			del_flag = TRUE;
		}
		break;
	case DIR_RIGHT:
		before_joint_offset = JointActorPosTbl[JOINT_ACTOR_L].x - before_x;

		CATS_ObjectPosMoveCapFx32(air_param->cap, AIR_MOVE_SPEED, 0);
		CATS_ObjectPosGetCap_SubSurface(air_param->cap, &x, &y, BALLOON_SUB_ACTOR_DISTANCE);
		joint_offset = JointActorPosTbl[JOINT_ACTOR_L].x - x;
		if(x > AIR_MOVE_RIGHT_DELETE_X){
			del_flag = TRUE;
		}
		break;
	}
	
	if(joint_offset >= 0){	//ジョイント手前
		if(joint_offset < AIR_REDUCE_DOT_OFFSET){
			check_scale_xy = FX32_ONE * joint_offset / AIR_REDUCE_DOT_OFFSET;
			if(check_scale_xy < 0x400){
				check_scale_xy = 0x400;
			}
			scale_xy = FX_FX32_TO_F32(check_scale_xy);
			CATS_ObjectScaleSetCap(air_param->cap, scale_xy, scale_xy);
		}
	}
	else{	//ジョイント通過後　ブースターでの大きさを元に拡縮率を設定する
		if(joint_offset > -AIR_REDUCE_DOT_OFFSET){
			check_scale_xy = Air_BoosterScale[air_param->booster_type] 
				* (-joint_offset) / AIR_REDUCE_DOT_OFFSET;
			if(check_scale_xy < 0x400){
				check_scale_xy = 0x400;
			}
			scale_xy = FX_FX32_TO_F32(check_scale_xy);
			CATS_ObjectScaleSetCap(air_param->cap, scale_xy, scale_xy);
		}
	}
	
	//ジョイントのセンターを通過した時のブースター発生チェック
	if(air_param->net_id == CommGetCurrentID() && before_joint_offset >= 0 && joint_offset <= 0){
		BOOSTER_MOVE *hit_move;
		air_param->booster_type = Booster_HitCheckNow(game, &hit_move);
		switch(air_param->booster_type){
		case BOOSTER_TYPE_NORMAL:
			air_param->air *= 2;
			break;
		case BOOSTER_TYPE_WEAK:
			air_param->air /= 2;
			break;
		case BOOSTER_TYPE_HARD:
			air_param->air *= 3;
			break;
		}
		BalloonTool_SendAirBoosterSet(game, air_param->booster_type);
		if(air_param->booster_type != BOOSTER_TYPE_NONE){
			BoosterMoveModeChange(hit_move, BOOSTER_MODE_HIT);
		}

		//ブースター通過タイミングでスタックされている空気データを送信バッファへ送りこむ
		{
			BALLOON_AIR_DATA *air_data;
			
			air_data = BalloonTool_SendAirPop(game);
			if(air_data != NULL){
				SendBalloon_Air(game, air_data);
			}
			else{
				OS_TPrintf("Airスタックからもれた空気がブースター通過\n");
			}
		}
	}
	
	if(del_flag == TRUE){
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   空気アクター生成
 *
 * @param   game			
 * @param   air_posdata		空気データ
 *
 * @retval  生成したアクターへのポインタ
 */
//--------------------------------------------------------------
static CATS_ACT_PTR Air_ActorCreate(BALLOON_GAME_PTR game, const AIR_POSITION_DATA *air_posdata)
{
	CATS_ACT_PTR cap;
	TCATS_OBJECT_ADD_PARAM_S act_head;
	
	//-- アクター生成 --//
	act_head = AirObjParam;
	act_head.x = air_posdata->x;
	act_head.y = air_posdata->y;
	act_head.pal = air_posdata->pal_ofs;
	cap = CATS_ObjectAdd_S(game->csp, game->crp, &act_head);
	
	CATS_ObjectPosSetCap_SubSurface(
		cap, air_posdata->x, air_posdata->y, BALLOON_SUB_ACTOR_DISTANCE);
	CATS_ObjectAnimeSeqSetCap(cap, air_posdata->anmseq);
	
	CATS_ObjectAffineSetCap(cap, CLACT_AFFINE_DOUBLE);

	CATS_ObjectUpdate(cap->act);
	return cap;
}

//--------------------------------------------------------------
/**
 * @brief   風船アクターを全て削除
 *
 * @param   game		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
void Air_ActorAllDelete(BALLOON_GAME_PTR game)
{
	int i;
	
	for(i = 0; i < PLAYER_AIR_PARAM_MAX; i++){
		if(game->air_param[i].occ == TRUE){
			CATS_ActorPointerDelete_S(game->air_param[i].cap);
			game->air_param[i].occ = FALSE;
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   破裂アクター生成
 *
 * @param   game		
 *
 * @retval  生成した破裂アクターへのポインタ
 */
//--------------------------------------------------------------
static void Exploded_ActorCreate(BALLOON_GAME_PTR game, EXPLODED_PARAM *exploded, BALLOON_STATUS *bst)
{
	TCATS_OBJECT_ADD_PARAM_S act_head;
	CATS_ACT_PTR cap;
	int player_pos;
	int act_num[WFLBY_MINIGAME_MAX];
	int total_air, total_act, anmseq;
	int act_index = 0;
	int i, s;
	
	act_head = ExplodedStormObjParam;
	total_air = bst->air + bst->air_stack;	//スタックの残りも込みで割合を出す
	for(i = 0; i < game->bsw->player_max; i++){
		player_pos = Balloon_NetID_to_PlayerPos(game, game->bsw->player_netid[i]);
		act_num[i] = bst->player_air[player_pos] * EXPLODED_STORM_ACTOR_MAX / total_air;
		anmseq = PlayerNoAnmSeq_Storm[game->bsw->player_max][player_pos];
		OS_TPrintf("紙ふぶき　player_pos = %d, num = %d, anmseq = %d\n", player_pos, act_num[i], anmseq);
		for(s = 0; s < act_num[i]; s++){
			STORM_WORK *storm = &exploded->storm[act_index];
			
			cap = CATS_ObjectAdd_S(game->csp, game->crp, &act_head);
			if(cap == NULL){
				break;
			}
			CATS_ObjectPosSetCap_SubSurface(
				cap, BALLOON_APPEAR_POS_X, BALLOON_APPEAR_POS_Y, BALLOON_SUB_ACTOR_DISTANCE);
			CATS_ObjectAnimeSeqSetCap(cap, anmseq);
			CATS_ObjectUpdate(cap->act);
			
			storm->furihaba = gf_rand() % 0x2000 + 0x2000;
			storm->add_furihaba = gf_rand() % 0x4000 + 0x2000;
			storm->sec = (gf_rand() % 360) << FX32_SHIFT;
			storm->add_sec = gf_rand() % 0x14000 + 0xa000;
			storm->delete_wait = gf_rand() % 15 + 20;
			exploded->storm[act_index].cap = cap;
			act_index++;
		}
	}
	
	//-- 風圧 --//
	act_head.pri = BALLOONSUB_SOFTPRI_EXPLODED_SMOKE;
	act_head.pal = PALOFS_SUB_EXPLODED_SMOKE;
	for(i = 0; i < EXPLODED_SMOKE_ACTOR_MAX; i++){
		STORM_WORK *smoke = &exploded->smoke[i];
		
		cap = CATS_ObjectAdd_S(game->csp, game->crp, &act_head);
		if(cap == NULL){
			break;
		}
		CATS_ObjectPosSetCap_SubSurface(
			cap, BALLOON_APPEAR_POS_X, BALLOON_APPEAR_POS_Y, BALLOON_SUB_ACTOR_DISTANCE);
		CATS_ObjectAnimeSeqSetCap(cap, MIXOBJ_ANMSEQ_EXPLODED_SMOKE_1 + gf_rand() % 3);
		CATS_ObjectUpdate(cap->act);
		
		smoke->furihaba = gf_rand() % 0x2000 + 0x2000;
		smoke->add_furihaba = gf_rand() % 0x4000 + 0x2000;
		smoke->sec = (gf_rand() % 360) << FX32_SHIFT;
		smoke->add_sec = gf_rand() % 0x14000 + 0x10000;
		smoke->delete_wait = gf_rand() % 15 + 20;
		exploded->smoke[i].cap = cap;
	}
	
	//-- 破片 --//
	act_head.pri = BALLOONSUB_SOFTPRI_EXPLODED_CHIP;
	act_head.pal = ExplodedChipPalNo[bst->level];
	for(i = 0; i < EXPLODED_CHIP_ACTOR_MAX; i++){
		STORM_WORK *chip = &exploded->chip[i];
		
		cap = CATS_ObjectAdd_S(game->csp, game->crp, &act_head);
		if(cap == NULL){
			break;
		}
		CATS_ObjectPosSetCap_SubSurface(
			cap, BALLOON_APPEAR_POS_X, BALLOON_APPEAR_POS_Y, BALLOON_SUB_ACTOR_DISTANCE);
		CATS_ObjectAnimeSeqSetCap(cap, MIXOBJ_ANMSEQ_BALLOON_EXPLODED);
		CATS_ObjectUpdate(cap->act);
		
		chip->furihaba = gf_rand() % 0x2000 + 0x2000;
		chip->add_furihaba = gf_rand() % 0x4000 + 0x2000;
		chip->sec = (gf_rand() % 360) << FX32_SHIFT;
		chip->add_sec = gf_rand() % 0x14000 + 0x10000;
		chip->delete_wait = gf_rand() % 15 + 20;
		exploded->chip[i].cap = cap;
	}
}

//--------------------------------------------------------------
/**
 * @brief   風船破裂アクターを削除する
 *
 * @param   game		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
void Exploded_AllDelete(BALLOON_GAME_PTR game)
{
	int i;
	STORM_WORK *storm, *smoke, *chip;
	
	for(i = 0; i < EXPLODED_STORM_ACTOR_MAX; i++){
		storm = &game->exploded_param.storm[i];
		if(storm->cap != NULL){
			CATS_ActorPointerDelete_S(storm->cap);
		}
	}

	for(i = 0; i < EXPLODED_SMOKE_ACTOR_MAX; i++){
		smoke = &game->exploded_param.smoke[i];
		if(smoke->cap != NULL){
			CATS_ActorPointerDelete_S(smoke->cap);
		}
	}

	for(i = 0; i < EXPLODED_CHIP_ACTOR_MAX; i++){
		chip = &game->exploded_param.chip[i];
		if(chip->cap != NULL){
			CATS_ActorPointerDelete_S(chip->cap);
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   風船破裂アクター更新処理
 *
 * @param   game		
 *
 * @retval  TRUE:アクター動作中。　FALSE:何も動作無し
 */
//--------------------------------------------------------------
BOOL Exploded_Update(BALLOON_GAME_PTR game)
{
	EXPLODED_PARAM *exploded = &game->exploded_param;
	STORM_WORK *storm, *smoke, *chip;
	s16 x, y;
	int eff_count = 0;
	int i;
	
	if(exploded->occ == FALSE){
		return FALSE;
	}
	
	//-- 紙ふぶき --//
	for(i = 0; i < EXPLODED_STORM_ACTOR_MAX; i++){
		storm = &exploded->storm[i];
		if(storm->cap != NULL){
			if(storm->delete_wait == 0){
				CATS_ActorPointerDelete_S(storm->cap);
				storm->cap = NULL;
				continue;
			}
			storm->sec += storm->add_sec;
			storm->furihaba += storm->add_furihaba;
			if(storm->furihaba >= (128+32) << FX32_SHIFT){
				storm->furihaba = (128+32) << FX32_SHIFT;
			}
			x = BALLOON_APPEAR_POS_X + (FX_Mul(Sin360FX(storm->sec), storm->furihaba)) / FX32_ONE;
			y = BALLOON_APPEAR_POS_Y + (-FX_Mul(Cos360FX(storm->sec), storm->furihaba)) / FX32_ONE;
			CATS_ObjectPosSetCap_SubSurface(storm->cap, x, y, BALLOON_SUB_ACTOR_DISTANCE);
			storm->delete_wait--;
			eff_count++;
		}
	}

	//-- 煙 --//
	for(i = 0; i < EXPLODED_SMOKE_ACTOR_MAX; i++){
		smoke = &exploded->smoke[i];
		if(smoke->cap != NULL){
			if(smoke->delete_wait == 0){
				CATS_ActorPointerDelete_S(smoke->cap);
				smoke->cap = NULL;
				continue;
			}
			smoke->sec += smoke->add_sec;
			smoke->furihaba += smoke->add_furihaba;
			if(smoke->furihaba >= (128+32) << FX32_SHIFT){
				smoke->furihaba = (128+32) << FX32_SHIFT;
			}
			x = BALLOON_APPEAR_POS_X + (FX_Mul(Sin360FX(smoke->sec), smoke->furihaba)) / FX32_ONE;
			y = BALLOON_APPEAR_POS_Y + (-FX_Mul(Cos360FX(smoke->sec), smoke->furihaba)) / FX32_ONE;
			CATS_ObjectPosSetCap_SubSurface(smoke->cap, x, y, BALLOON_SUB_ACTOR_DISTANCE);
			smoke->delete_wait--;
			eff_count++;
		}
	}
	
	//-- 破片 --//
	for(i = 0; i < EXPLODED_CHIP_ACTOR_MAX; i++){
		chip = &exploded->chip[i];
		if(chip->cap != NULL){
			if(chip->delete_wait == 0){
				CATS_ActorPointerDelete_S(chip->cap);
				chip->cap = NULL;
				continue;
			}
			chip->sec += chip->add_sec;
			chip->furihaba += chip->add_furihaba;
			if(chip->furihaba >= (128+32) << FX32_SHIFT){
				chip->furihaba = (128+32) << FX32_SHIFT;
			}
			x = BALLOON_APPEAR_POS_X + (FX_Mul(Sin360FX(chip->sec), chip->furihaba)) / FX32_ONE;
			y = BALLOON_APPEAR_POS_Y + (-FX_Mul(Cos360FX(chip->sec), chip->furihaba)) / FX32_ONE;
			CATS_ObjectPosSetCap_SubSurface(chip->cap, x, y, BALLOON_SUB_ACTOR_DISTANCE);
			chip->delete_wait--;
			eff_count++;
		}
	}
	
	if(eff_count == 0){
		exploded->occ = FALSE;
	}
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   破裂パラメータセット
 *
 * @param   game		
 * @param   exploded		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
BOOL BalloonTool_ExplodedParamAdd(BALLOON_GAME_PTR game)
{
	EXPLODED_PARAM *exploded = &game->exploded_param;
	int i;
	
	if(exploded->occ == TRUE){
		return FALSE;
	}
	
	MI_CpuClear8(exploded, sizeof(EXPLODED_PARAM));
	
	//爆発アクター生成　自分の画面状況を反映させる為bstは個々のローカルのを使用
	Exploded_ActorCreate(game, &game->exploded_param, &game->bst);
	
	//風船BG消去
	BalloonTool_BalloonBGErase(game->bgl, &game->bst);
	
	//風船アイコン爆発アニメ
	IconBalloon_ExplodedReq(game);
	
	
	//-- bstクリア --//
	game->balloon_occ = FALSE;
	game->bst.air = 0;
	game->bst.air_stack = 0;
	for(i = 0; i < WFLBY_MINIGAME_MAX; i++){
		game->bst.player_air[i] = 0;
	}

	Snd_SePlay(SE_BALLOON_EXPLODED);
	exploded->occ = TRUE;
	return TRUE;
}


//--------------------------------------------------------------
/**
 * @brief   風船アイコンアクター生成
 *
 * @param   game		
 *
 * @retval  生成した破裂アクターへのポインタ
 */
//--------------------------------------------------------------
static CATS_ACT_PTR IconBalloon_ActorCreate(BALLOON_GAME_PTR game, int icon_type, int pos)
{
	CATS_ACT_PTR cap;
	TCATS_OBJECT_ADD_PARAM_S act_head;

	//-- アクター生成 --//
	act_head = IconBalloonObjParam;
	cap = CATS_ObjectAdd_S(game->csp, game->crp, &act_head);
	
	CATS_ObjectPosSetCap_SubSurface(
		cap, ICON_BALLOON_POS_RIGHT_X - pos * ICON_BALLOON_POS_OFFSET_X, 
		ICON_BALLOON_POS_Y, BALLOON_SUB_ACTOR_DISTANCE);
	CATS_ObjectAnimeSeqSetCap(cap, MIXOBJ_ANMSEQ_ICON_BALLOON_1 + icon_type);
	
	CATS_ObjectUpdate(cap->act);
	CATS_ObjectAutoAnimeSetCap(cap, TRUE);	//オートアニメ設定
	return cap;
}

//--------------------------------------------------------------
/**
 * @brief   風船アイコンを全て作成する
 *
 * @param   game		
 */
//--------------------------------------------------------------
void IconBalloon_AllCreate(BALLOON_GAME_PTR game)
{
	int i;
	ICONBALLOON_PARAM *ibp;
	
	for(i = 0; i < ICON_BALLOON_PARAM_MAX; i++){
		ibp = &game->iconballoon_param[i];
		BalloonTool_IconBalloonParamAdd(game, ibp, i, i);
		CATS_ObjectUpdateNumCap(ibp->cap, FX32_ONE*4 * i);	//最初のアニメをバラけさせる
	}
}

//--------------------------------------------------------------
/**
 * @brief   風船アイコンを全て削除する
 *
 * @param   game		
 */
//--------------------------------------------------------------
void IconBalloon_AllDelete(BALLOON_GAME_PTR game)
{
	int i;
	ICONBALLOON_PARAM *ibp;
	
	for(i = 0; i < ICON_BALLOON_PARAM_MAX; i++){
		ibp = &game->iconballoon_param[i];
		CATS_ActorPointerDelete_S(ibp->cap);
	}
}

//--------------------------------------------------------------
/**
 * @brief   風船破裂アクター更新処理
 *
 * @param   game		
 *
 * @retval  TRUE:アクター動作中。　FALSE:何も動作無し
 */
//--------------------------------------------------------------
BOOL IconBalloon_Update(BALLOON_GAME_PTR game)
{
	ICONBALLOON_PARAM *ibp;
	int i;
	
	for(i = 0; i < ICON_BALLOON_PARAM_MAX; i++){
		ibp = &game->iconballoon_param[i];
		IconBalloon_PosMove(ibp);
		IconBalloon_StatusUpdate(game, ibp);
	}

	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   風船アイコンパラメータセット
 *
 * @param   game		
 * @param   ibp				代入先
 * @param   pos				位置
 * @param   balloon_no		担当する風船番号
 *
 * @retval  TRUE:セット成功
 */
//--------------------------------------------------------------
static BOOL BalloonTool_IconBalloonParamAdd(BALLOON_GAME_PTR game, ICONBALLOON_PARAM *ibp, int pos, int balloon_no)
{
	MI_CpuClear8(ibp, sizeof(ICONBALLOON_PARAM));
	
	ibp->type = IconBalloon_TypeGet(balloon_no);
	ibp->status = ICON_BALLOON_STATUS_NORMAL;
	ibp->cap = IconBalloon_ActorCreate(game, ibp->type, pos);
	ibp->pos = pos;
	ibp->balloon_no = balloon_no;
	
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   風船アイコンの座標更新処理
 *
 * @param   ibp		風船アイコンパラメータへのポインタ
 */
//--------------------------------------------------------------
static void IconBalloon_PosMove(ICONBALLOON_PARAM *ibp)
{
	s16 x, now_x, now_y;
	
	x = ICON_BALLOON_POS_RIGHT_X - ibp->pos * ICON_BALLOON_POS_OFFSET_X;
	CATS_ObjectPosGetCap_SubSurface(ibp->cap, &now_x, &now_y, BALLOON_SUB_ACTOR_DISTANCE);
	
	if(x > now_x){
		now_x += ICON_BALLOON_MOVE_SP_X;
		if(now_x > x){
			now_x = x;
		}
		CATS_ObjectPosSetCap_SubSurface(ibp->cap, now_x, now_y, BALLOON_SUB_ACTOR_DISTANCE);
	}
}

//--------------------------------------------------------------
/**
 * @brief   風船アイコンのステータス更新
 *
 * @param   game		
 * @param   ibp		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
static void IconBalloon_StatusUpdate(BALLOON_GAME_PTR game, ICONBALLOON_PARAM *ibp)
{
	int parcent, i;
	ICONBALLOON_PARAM *get_ibp;
	
	if(ibp->pos != 0){
		return;	//一番右端以外は関係なし
	}
	
	switch(ibp->status){
	//爆発アニメ終了チェック
	case ICON_BALLOON_STATUS_EXPLODED:
		if(CATS_ObjectAnimeActiveCheckCap(ibp->cap) == FALSE){
			//爆発した風船アイコンを左から新しく登場するアイコンとして再利用する
			ibp->balloon_no += ICON_BALLOON_PARAM_MAX;	//風船番号更新
			ibp->type = IconBalloon_TypeGet(ibp->balloon_no);
			ibp->status = ICON_BALLOON_STATUS_NORMAL;
			CATS_ObjectAnimeSeqSetCap(ibp->cap, MIXOBJ_ANMSEQ_ICON_BALLOON_1 + ibp->type);
			CATS_ObjectPosSetCap_SubSurface(ibp->cap, 
				ICON_BALLOON_POS_RIGHT_X - ICON_BALLOON_PARAM_MAX * ICON_BALLOON_POS_OFFSET_X, 
				ICON_BALLOON_POS_Y, BALLOON_SUB_ACTOR_DISTANCE);
			//全ての風船アイコンのposを詰める
			for(i = 0; i < ICON_BALLOON_PARAM_MAX; i++){
				get_ibp = &game->iconballoon_param[i];
				if(get_ibp->pos == 0){
					get_ibp->pos = ICON_BALLOON_PARAM_MAX - 1;
				}
				else{
					get_ibp->pos--;
				}
			}
		}
		break;
	//ピンチ状態チェック
	case ICON_BALLOON_STATUS_NORMAL:
		parcent = BalloonTool_AirParcentGet(game);
		if(ibp->status == ICON_BALLOON_STATUS_NORMAL && parcent > ICON_BALLOON_PINCH_PARCENT){
			ibp->status = ICON_BALLOON_STATUS_PINCH;
			CATS_ObjectAnimeSeqSetCap(ibp->cap, MIXOBJ_ANMSEQ_ICON_BALLOON_1_PINCH + ibp->type);
			Snd_SePlay(SE_BALLOON_PINCH);
		}
		break;
	}
}

//--------------------------------------------------------------
/**
 * @brief   風船番号から風船のレベルタイプを取得する
 * @param   balloon_no		風船番号
 * @retval  レベルタイプ
 */
//--------------------------------------------------------------
static int IconBalloon_TypeGet(int balloon_no)
{
	if(balloon_no >= BALLOON_LEVEL3_NUM){
		return BALLOON_LEVEL_3;
	}
	if(balloon_no >= BALLOON_LEVEL2_NUM){
		return BALLOON_LEVEL_2;
	}
	return BALLOON_LEVEL_1;
}

//--------------------------------------------------------------
/**
 * @brief   風船アイコンに爆発アニメリクエストを設定する
 *
 * @param   game		
 */
//--------------------------------------------------------------
static void IconBalloon_ExplodedReq(BALLOON_GAME_PTR game)
{
	int i;
	ICONBALLOON_PARAM *ibp;
	
	for(i = 0; i < ICON_BALLOON_PARAM_MAX; i++){
		ibp = &game->iconballoon_param[i];
		if(ibp->pos == 0){
			ibp->status = ICON_BALLOON_STATUS_EXPLODED;
			CATS_ObjectAnimeSeqSetCap(ibp->cap, MIXOBJ_ANMSEQ_ICON_BALLOON_EXPLODED);
			return;
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   参加人数に応じて名前ウィンドウのパレット番号を書き換える
 *
 * @param   game		
 */
//--------------------------------------------------------------
void BalloonTool_NameWindowPalNoSwap(BALLOON_GAME_PTR game)
{
	int i;
	
	switch(game->bsw->player_max){
	case 3:	//問題があるのは3人の時だけ。(2人、4人は元データのままでOK)
		GF_BGL_ScrPalChange(game->bgl, BALLOON_SUBFRAME_WIN, 0, 13, 12, 4, NAME_WIN_PALNO_YELLOW);
		GF_BGL_ScrPalChange(game->bgl, BALLOON_SUBFRAME_WIN, 0x14, 13, 12, 4, NAME_WIN_PALNO_BLUE);
		break;
	}
}

//--------------------------------------------------------------
/**
 * @brief   プレイヤー位置に従ってパイプのカラーのスワップを行う
 *
 * @param   game		
 */
//--------------------------------------------------------------
void BalloonTool_PaletteSwap_Pipe(BALLOON_GAME_PTR game)
{
	u16 color[WFLBY_MINIGAME_MAX][PLAYER_NO_PALETTE_COLOR_NUM_PIPE];
	int i, player;
	int my_player = 0;
	int read_pos, write_pos, k, current_id;
	u16 *plttbuf, *transbuf;
	
	//各プレイヤーのカラーデータをテンポラリバッファへコピー
	for(player = 0; player < game->bsw->player_max; player++){
		read_pos = PlayerNoPaletteWritePosTbl_Pipe[player];
		for(i = 0; i < PLAYER_NO_PALETTE_COLOR_NUM_PIPE; i++){
			color[player][i] = PaletteWork_ColorGet(game->pfd, FADE_SUB_BG, FADEBUF_SRC, 
				read_pos + i);
		}
	}
	
	//自分のプレイヤー位置を取得
	current_id = CommGetCurrentID();
	for(my_player = 0; my_player < game->bsw->player_max; my_player++){
		if(game->bsw->player_netid[my_player] == current_id){
			break;
		}
	}
	
	//自分のプレイヤー位置を基準にカラーをセットしなおす
	plttbuf = PaletteWorkDefaultWorkGet(game->pfd, FADE_SUB_BG);
	transbuf = PaletteWorkTransWorkGet(game->pfd, FADE_SUB_BG);
	for(i = 0; i < game->bsw->player_max; i++){
		read_pos = i;
		write_pos = PlayerNoPaletteSwapTbl_Pipe[game->bsw->player_max][Balloon_NetID_to_PlayerPos(game, game->bsw->player_netid[i])];
		for(k = 0; k < PLAYER_NO_PALETTE_COLOR_NUM_PIPE; k++){
			plttbuf[write_pos + k] = color[read_pos][k];
			transbuf[write_pos + k] = color[read_pos][k];
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   プレイヤー位置に従ってプレイヤー毎のOBJ(ブースターや空気)のカラーのスワップを行う
 *
 * @param   game		
 */
//--------------------------------------------------------------
void BalloonTool_PaletteSwap_PlayerOBJ(BALLOON_GAME_PTR game)
{
	u16 color[WFLBY_MINIGAME_MAX][PLAYER_NO_PALETTE_COLOR_NUM_AIR];
	int i, player;
	int my_player = 0;
	int read_pos, write_pos, k, current_id;
	u16 *plttbuf, *transbuf;
	int common_palno;
	
	common_palno = CATS_PlttID_NoGet(game->crp, PLTTID_SUB_OBJ_COMMON, NNS_G2D_VRAM_TYPE_2DSUB);
	
	//各プレイヤーのカラーデータをテンポラリバッファへコピー
	for(player = 0; player < game->bsw->player_max; player++){
		read_pos = PlayerNoPaletteWritePosTbl_Obj[player];
		for(i = 0; i < PLAYER_NO_PALETTE_COLOR_NUM_AIR; i++){
			color[player][i] = PaletteWork_ColorGet(game->pfd, FADE_SUB_OBJ, FADEBUF_SRC, 
				common_palno*16 + read_pos + i);
		}
	}
	
	//自分のプレイヤー位置を取得
	current_id = CommGetCurrentID();
	for(my_player = 0; my_player < game->bsw->player_max; my_player++){
		if(game->bsw->player_netid[my_player] == current_id){
			break;
		}
	}
	
	//自分のプレイヤー位置を基準にカラーをセットしなおす
	plttbuf = PaletteWorkDefaultWorkGet(game->pfd, FADE_SUB_OBJ);
	transbuf = PaletteWorkTransWorkGet(game->pfd, FADE_SUB_OBJ);
	for(i = 0; i < game->bsw->player_max; i++){
		read_pos = i;
		write_pos = PlayerNoPaletteSwapTbl_Obj[game->bsw->player_max][Balloon_NetID_to_PlayerPos(game, game->bsw->player_netid[i])] + common_palno*16;
		for(k = 0; k < PLAYER_NO_PALETTE_COLOR_NUM_AIR; k++){
			plttbuf[write_pos + k] = color[read_pos][k];
			transbuf[write_pos + k] = color[read_pos][k];
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   プレイヤー位置に従って紙ふぶきのカラーのスワップを行う
 *
 * @param   game		
 */
//--------------------------------------------------------------
void BalloonTool_PaletteSwap_Storm(BALLOON_GAME_PTR game)
{
	u16 color[WFLBY_MINIGAME_MAX][PLAYER_NO_PALETTE_COLOR_NUM_STORM];
	int i, player;
	int my_player = 0;
	int read_pos, write_pos, k, current_id;
	u16 *plttbuf, *transbuf;
	int common_palno;
	
	common_palno = CATS_PlttID_NoGet(game->crp, PLTTID_SUB_OBJ_COMMON, NNS_G2D_VRAM_TYPE_2DSUB);
	
	//各プレイヤーのカラーデータをテンポラリバッファへコピー
	for(player = 0; player < game->bsw->player_max; player++){
		read_pos = PlayerNoPaletteWritePosTbl_Storm[player];
		for(i = 0; i < PLAYER_NO_PALETTE_COLOR_NUM_STORM; i++){
			color[player][i] = PaletteWork_ColorGet(game->pfd, FADE_SUB_OBJ, FADEBUF_SRC, 
				common_palno*16 + read_pos + i);
		}
	}
	
	//自分のプレイヤー位置を取得
	current_id = CommGetCurrentID();
	for(my_player = 0; my_player < game->bsw->player_max; my_player++){
		if(game->bsw->player_netid[my_player] == current_id){
			break;
		}
	}
	
	//自分のプレイヤー位置を基準にカラーをセットしなおす
	plttbuf = PaletteWorkDefaultWorkGet(game->pfd, FADE_SUB_OBJ);
	transbuf = PaletteWorkTransWorkGet(game->pfd, FADE_SUB_OBJ);
	for(i = 0; i < game->bsw->player_max; i++){
		read_pos = i;
		write_pos = PlayerNoPaletteSwapTbl_Storm[game->bsw->player_max][Balloon_NetID_to_PlayerPos(game, game->bsw->player_netid[i])] + common_palno*16;
		for(k = 0; k < PLAYER_NO_PALETTE_COLOR_NUM_STORM; k++){
			plttbuf[write_pos + k] = color[read_pos][k];
			transbuf[write_pos + k] = color[read_pos][k];
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   エアーデータを作成する
 *
 * @param   game			
 * @param   balloon_no		風船番号
 * @param   air				空気量(補正やブースター前の純粋な空気量)
 * @param   air_data		セット先
 */
//--------------------------------------------------------------
void BalloonTool_AirDataCreate(BALLOON_GAME_PTR game, int balloon_no, s32 air, BALLOON_AIR_DATA *air_data)
{
	MI_CpuClear8(air_data, sizeof(BALLOON_AIR_DATA));
	air_data->net_id = CommGetCurrentID();
	air_data->no = balloon_no;
	air_data->air = air;
	air_data->last_air = air;
	air_data->booster_type = BOOSTER_TYPE_NONE;
	air_data->booster_occ = 0;
}

//--------------------------------------------------------------
/**
 * @brief   空気データを送信バッファにスタックする
 *
 * @param   game		
 * @param   air_data	空気データ
 *
 * @retval  TRUE:送信バッファにスタックされた
 * @retval  FALSE:送信バッファがいっぱいだった為受け取れなかった
 *
 * FALSEが返ってきても基本、演出データなので無視してしまってよい
 */
//--------------------------------------------------------------
BOOL BalloonTool_SendAirPush(BALLOON_GAME_PTR game, BALLOON_AIR_DATA *air_data)
{
	if(game->send_airdata_write_no - game->send_airdata_read_no >= PIPE_AIR_AIR_MAX){
		return FALSE;
	}
	game->send_airdata_stack[game->send_airdata_write_no % PIPE_AIR_AIR_MAX] = *air_data;
	game->send_airdata_write_no++;
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   空気データを送信バッファから取り出す
 *
 * @param   game		
 *
 * @retval  空気データへのポインタ(データが無い場合はNULL)
 */
//--------------------------------------------------------------
BALLOON_AIR_DATA * BalloonTool_SendAirPop(BALLOON_GAME_PTR game)
{
	BALLOON_AIR_DATA *air_data;
	
	if(game->send_airdata_read_no >= game->send_airdata_write_no){
		return NULL;
	}
	air_data = &game->send_airdata_stack[game->send_airdata_read_no % PIPE_AIR_AIR_MAX];
	game->send_airdata_read_no++;
	return air_data;
}

//--------------------------------------------------------------
/**
 * @brief   先頭にスタックされている空気データにブースタータイプを設定する
 *
 * @param   game		
 *
 * @retval  TRUE:セットした
 * @retval  FALSE:セット出来なかった(スタックされている空気が無かった)
 */
//--------------------------------------------------------------
BOOL BalloonTool_SendAirBoosterSet(BALLOON_GAME_PTR game, int booster_type)
{
	BALLOON_AIR_DATA *air_data = NULL;
	int i, div_read, div_write;
	
	if(game->send_airdata_read_no == game->send_airdata_write_no
			|| booster_type == BOOSTER_TYPE_NONE){
		return FALSE;
	}
	
	div_read = game->send_airdata_read_no % PIPE_AIR_AIR_MAX;
	div_write = game->send_airdata_write_no % PIPE_AIR_AIR_MAX;
	if(div_read < div_write){
		for(i = div_read; i < div_write; i++){
			if(game->send_airdata_stack[i].booster_occ == 0){
				air_data = &game->send_airdata_stack[i];
				break;
			}
		}
	}
	else{
		for(i = div_read; i < PIPE_AIR_AIR_MAX; i++){
			if(game->send_airdata_stack[i].booster_occ == 0){
				air_data = &game->send_airdata_stack[i];
				break;
			}
		}
		if(air_data == NULL){
			for(i = 0; i < div_write; i++){
				if(game->send_airdata_stack[i].booster_occ == 0){
					air_data = &game->send_airdata_stack[i];
					break;
				}
			}
		}
	}
	if(air_data == NULL){
		GF_ASSERT(0);		//ブースター未設定のエアーデータが1つも無い
		return FALSE;
	}
	
	GF_ASSERT(air_data->booster_type == 0);
	air_data->booster_type = booster_type;
	air_data->booster_occ = TRUE;
	switch(booster_type){
	case BOOSTER_TYPE_NORMAL:
		air_data->last_air = air_data->air * 2;
		break;
	case BOOSTER_TYPE_WEAK:
		air_data->last_air = air_data->air / 2;
		break;
	case BOOSTER_TYPE_HARD:
		air_data->last_air = air_data->air * 3;
		break;
	}
	
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   ジョイントアクター生成
 *
 * @param   game		
 */
//--------------------------------------------------------------
void Joint_ActorCreateAll(BALLOON_GAME_PTR game, JOINT_WORK *joint)
{
	CATS_ACT_PTR cap;
	TCATS_OBJECT_ADD_PARAM_S act_head;
	int i;

	act_head = JointObjParam;
	for(i = 0; i < JOINT_ACTOR_MAX; i++){
		joint->cap[i] = CATS_ObjectAdd_S(game->csp, game->crp, &act_head);
		CATS_ObjectPosSetCap_SubSurface(joint->cap[i], 
			JointActorPosTbl[i].x, JointActorPosTbl[i].y, BALLOON_SUB_ACTOR_DISTANCE);
		CATS_ObjectAnimeSeqSetCap(joint->cap[i], MIXOBJ_ANMSEQ_JOINT_UD + i/2);
		CATS_ObjectUpdate(joint->cap[i]->act);
	}
	
	switch(game->bsw->player_max){
	case 2:
		CATS_ObjectEnableCap(joint->cap[JOINT_ACTOR_L], CATS_ENABLE_FALSE);
		CATS_ObjectEnableCap(joint->cap[JOINT_ACTOR_R], CATS_ENABLE_FALSE);
		break;
	case 3:
		CATS_ObjectEnableCap(joint->cap[JOINT_ACTOR_U], CATS_ENABLE_FALSE);
		break;
	}
}

//--------------------------------------------------------------
/**
 * @brief   ジョイントアクターを全て削除する
 *
 * @param   game		
 */
//--------------------------------------------------------------
void Joint_ActorDeleteAll(BALLOON_GAME_PTR game, JOINT_WORK *joint)
{
	int i;
	
	for(i = 0; i < JOINT_ACTOR_MAX; i++){
		CATS_ActorPointerDelete_S(joint->cap[i]);
	}
}

//--------------------------------------------------------------
/**
 * @brief   ブースターアクター生成
 *
 * @param   game		
 * @param   booster		ブースターワークへのポインタ
 */
//--------------------------------------------------------------
void Booster_ActorCreateAll(BALLOON_GAME_PTR game, BOOSTER_WORK *booster)
{
	TCATS_OBJECT_ADD_PARAM_S act_head;
	int i, pal_ofs;
	fx32 base_x, base_y;
	
//	pal_ofs = PlayerNoPalOfs_Booster[game->bsw->player_max][Balloon_NetID_to_PlayerPos(game, CommGetCurrentID())];

	act_head = BoosterObjParam;
	for(i = 0; i < BOOSTER_ACTOR_MAX; i++){
		base_x = 256/2 + FX_Mul(Sin360FX(
			booster->theta + booster->move[i].start_theta), BOOSTER_FURIHABA_X);
		base_y = 196/2 + (-FX_Mul(
			Cos360FX(booster->theta + booster->move[i].start_theta), BOOSTER_FURIHABA_Y));
		
//		act_head.pal = pal_ofs;
		booster->move[i].cap = CATS_ObjectAdd_S(game->csp, game->crp, &act_head);
		CATS_ObjectPosSetCap_SubSurface(booster->move[i].cap, 
			base_x, base_y + BOOSTER_OFFSET_Y, BALLOON_SUB_ACTOR_DISTANCE);
		CATS_ObjectAnimeSeqSetCap(booster->move[i].cap, 
			BoosterType_StartSeqAnimeNo[i + BOOSTER_TYPE_NORMAL]);
		CATS_ObjectUpdate(booster->move[i].cap->act);
		CATS_ObjectEnableCap(booster->move[i].cap, CATS_ENABLE_FALSE);
		
		//ヒットエフェクト
		booster->move[i].hit_cap = CATS_ObjectAdd_S(game->csp, game->crp, &BoosterHitObjParam);
		CATS_ObjectPosSetCap_SubSurface(booster->move[i].hit_cap, 
			0, 0, BALLOON_SUB_ACTOR_DISTANCE);
		CATS_ObjectAnimeSeqSetCap(booster->move[i].hit_cap, MIXOBJ_ANMSEQ_BOOSTER_HIT_EFF);
		CATS_ObjectUpdate(booster->move[i].hit_cap->act);
		CATS_ObjectEnableCap(booster->move[i].hit_cap, CATS_ENABLE_FALSE);
		
		//影
		booster->move[i].shadow_cap 
			= CATS_ObjectAdd_S(game->csp, game->crp, &BoosterShadowObjParam);
		CATS_ObjectPosSetCap_SubSurface(booster->move[i].shadow_cap, 
			base_x, base_y + BOOSTER_OFFSET_Y + BOOSTER_SHADOW_OFFSET_Y, 
			BALLOON_SUB_ACTOR_DISTANCE);
		CATS_ObjectObjModeSetCap(booster->move[i].shadow_cap, GX_OAM_MODE_XLU);	//半透明ON
		CATS_ObjectAffineSetCap(booster->move[i].shadow_cap, CLACT_AFFINE_NORMAL);	//拡縮ON
		CATS_ObjectScaleSetCap(booster->move[i].shadow_cap, 1.0f, 1.0f);
		CATS_ObjectAnimeSeqSetCap(booster->move[i].shadow_cap, MIXOBJ_ANMSEQ_BOOSTER_SHADOW + i);
		CATS_ObjectUpdate(booster->move[i].shadow_cap->act);
		CATS_ObjectEnableCap(booster->move[i].shadow_cap, CATS_ENABLE_FALSE);
		
		//着地の煙
		BoosterLandSmoke_ActorCreate(game, &booster->move[i].land_smoke);
	}
	
	CATS_ObjectEnableCap(booster->move[BOOSTER_ACTOR_NORMAL].cap, CATS_ENABLE_TRUE);
	CATS_ObjectEnableCap(booster->move[BOOSTER_ACTOR_NORMAL].shadow_cap, CATS_ENABLE_TRUE);
}

//--------------------------------------------------------------
/**
 * @brief   ブースターアクターを全て削除する
 *
 * @param   game		
 * @param   booster		ブースターワークへのポインタ
 */
//--------------------------------------------------------------
void Booster_ActorDeleteAll(BALLOON_GAME_PTR game, BOOSTER_WORK *booster)
{
	int i;
	
	for(i = 0; i < BOOSTER_ACTOR_MAX; i++){
		CATS_ActorPointerDelete_S(booster->move[i].cap);
		CATS_ActorPointerDelete_S(booster->move[i].hit_cap);
		CATS_ActorPointerDelete_S(booster->move[i].shadow_cap);
		BoosterLandSmoke_ActorDelete(game, &booster->move[i].land_smoke);
	}
}

//--------------------------------------------------------------
/**
 * @brief   ブースター動作制御ワーク初期化
 *
 * @param   game		
 * @param   booster		ブースター動作制御ワークへのポインタ
 */
//--------------------------------------------------------------
void Booster_Init(BALLOON_GAME_PTR game, BOOSTER_WORK *booster)
{
	int i;
	
	booster->add_theta = (360 << FX32_SHIFT) / BOOSTER_STOP_PART_NUM / BoosterMoveDataTbl[booster->lap].move_frame;
	for(i = 0; i < BOOSTER_ACTOR_MAX; i++){
		booster->move[i].start_theta = (i * 90) << FX32_SHIFT;
		booster->move[i].booster_type = BOOSTER_TYPE_NORMAL + i;
	}
	booster->move[BOOSTER_ACTOR_NORMAL].mode = BOOSTER_MODE_NORMAL;
	
	booster->stop = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   ブースター更新処理
 *
 * @param   game		
 * @param   booster		ブースター動作制御ワークへのポインタ
 */
//--------------------------------------------------------------
void Booster_Update(BALLOON_GAME_PTR game, BOOSTER_WORK *booster)
{
	int i, world_frame, end_frame = 0;
	
	if(booster->stop == TRUE){
		return;
	}
	
	booster->world_frame++;
	
	if(booster->wait == 0){
		//秒針を進める
		booster->theta += booster->add_theta;
		booster->frame++;
		if(booster->frame >= BoosterMoveDataTbl[booster->lap].move_frame){
			booster->frame = 0;
			booster->byousin_pos++;
			booster->theta = (360 << FX32_SHIFT) / BOOSTER_STOP_PART_NUM * booster->byousin_pos;
			booster->wait = BoosterMoveDataTbl[booster->lap].wait_frame;
		}
	}
	else{	//待ち時間
		booster->wait--;
		if(booster->wait == 0){
			if(booster->byousin_pos >= BOOSTER_STOP_PART_NUM){	//1周した
				booster->byousin_pos = 0;
				booster->lap++;
				if(booster->lap >= NELEMS(BoosterMoveDataTbl)){
					booster->lap = NELEMS(BoosterMoveDataTbl) - 1;
				}
				booster->add_theta = (360 << FX32_SHIFT) / BOOSTER_STOP_PART_NUM / BoosterMoveDataTbl[booster->lap].move_frame;
			}
		}
	}

	//新しいブースターの登場チェック
	if(booster->lap == BOOSTER_2ND_APPEAR_LAP - 1){
		for(i = 0; i < BOOSTER_2ND_APPEAR_LAP; i++){
			end_frame += BoosterMoveDataTbl[i].move_frame * BOOSTER_STOP_PART_NUM
				+ BoosterMoveDataTbl[i].wait_frame * BOOSTER_STOP_PART_NUM;
		}
		if(end_frame - BOOSTER_APPEAR_FRAME == booster->world_frame){
			BoosterMoveModeChange(&booster->move[1], BOOSTER_MODE_APPEAR);
		}
	}
	else if(booster->lap == BOOSTER_3RD_APPEAR_LAP - 1){
		for(i = 0; i < BOOSTER_3RD_APPEAR_LAP; i++){
			end_frame += BoosterMoveDataTbl[i].move_frame * BOOSTER_STOP_PART_NUM
				+ BoosterMoveDataTbl[i].wait_frame * BOOSTER_STOP_PART_NUM;
		}
		if(end_frame - BOOSTER_APPEAR_FRAME == booster->world_frame){
			BoosterMoveModeChange(&booster->move[2], BOOSTER_MODE_APPEAR);
		}
	}
	
	//各ブースターの動作
	for(i = 0; i < BOOSTER_ACTOR_MAX; i++){
		Booster_Move(game, booster, &booster->move[i]);
		BoosterLandSmoke_Update(game, &booster->move[i], &booster->move[i].land_smoke);
	}
}

//--------------------------------------------------------------
/**
 * @brief   今のタイミングでブースターとヒット可能かチェック
 *
 * @param   game					
 * @param   ret_move		ヒットしたブースターへのポインタが代入される
 * 
 * @retval  ブースタータイプ
 */
//--------------------------------------------------------------
static int Booster_HitCheckNow(BALLOON_GAME_PTR game, BOOSTER_MOVE **ret_move)
{
	BOOSTER_WORK *booster = &game->booster;
	int i;
	
	for(i = 0; i < BOOSTER_ACTOR_MAX; i++){
		if(booster->move[i].hit_ok == TRUE){
			OS_TPrintf("hit booster_type = %d\n", booster->move[i].booster_type);
			*ret_move = &booster->move[i];
			return booster->move[i].booster_type;
		}
	}
	*ret_move = NULL;
	return BOOSTER_TYPE_NONE;
}

#if 0
static int Booster_HitCheck(BALLOON_GAME_PTR game, PLAYER_AIR_PARAM)
{
	BOOSTER_WORK *booster = &game->booster;
	int i;
	int arrival_frame;
	int player_pos;
	const AIR_POSITION_DATA *air_posdata;
	fx32 joint_len_y;
	
	player_pos = Balloon_NetID_to_PlayerPos(game, CommGetCurrentID());
	air_posdata = &AirPositionDataTbl[game->bsw->player_max][player_pos];
	
	joint_len_y = (air_posdata->y - JointActorPosTbl[JOINT_ACTOR_D].y) << FX32_SHIFT;
	arrival_frame = FX_Div(joint_len_y, AIR_MOVE_SPEED);
	if(FX_Mod(joint_len_y, AIR_MOVE_SPEED)){
		arrival_frame++;
	}
	arrival_frame += MY_AIR_2D_APPEAR_WAIT;		//何フレーム後に空気がジョイントを通過するか
	
	//arrival_frameに達したとき、ブースターがHIT可、常態かチェック
	for(i = 0; i < 
}
#endif

//--------------------------------------------------------------
/**
 * @brief   ブースターのモード切替
 *
 * @param   move		
 * @param   mode		BOOSTER_MODE_???
 */
//--------------------------------------------------------------
static void BoosterMoveModeChange(BOOSTER_MOVE *move, int mode)
{
	move->mode = mode;
	move->seq = 0;
	move->local_frame = 0;
	move->local_work = 0;
	move->hit_ok = FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   ブースターの移動処理
 *
 * @param   game		
 * @param   booster		ブースター動作制御ワークへのポインタ
 * @param   move		ブースター動作ワークへのポインタ
 */
//--------------------------------------------------------------
static void Booster_Move(BALLOON_GAME_PTR game, BOOSTER_WORK *booster, BOOSTER_MOVE *move)
{
	BoosterMoveSeqTbl[move->mode](game, booster, move);
}

//--------------------------------------------------------------
/**
 * @brief   ブースター動作：待機中
 *
 * @param   game		
 * @param   booster		ブースター動作制御ワークH
 * @param   move		ブースター動作ワークへのポインタ
 *
 * @retval  
 */
//--------------------------------------------------------------
static int BoosterMove_Wait(BALLOON_GAME_PTR game, BOOSTER_WORK *booster, BOOSTER_MOVE *move)
{
	move->hit_ok = FALSE;
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   ブースター動作：登場
 *
 * @param   game		
 * @param   booster		ブースター動作制御ワークH
 * @param   move		ブースター動作ワークへのポインタ
 *
 * @retval  
 */
//--------------------------------------------------------------
static int BoosterMove_Appear(BALLOON_GAME_PTR game, BOOSTER_WORK *booster, BOOSTER_MOVE *move)
{
	s16 x, y, add_x, add_y, offset_y, offset_x;
	fx32 jump_theta;
	int jump_y = 0;
	fx32 check_scale_xy;
	f32 scale_xy;
	
	add_x = 0;
	add_y = 0;
	x = move->local_x;
	y = move->local_y;
	
	switch(move->seq){
	case 0:
		CATS_ObjectEnableCap(move->cap, CATS_ENABLE_TRUE);
		CATS_ObjectEnableCap(move->shadow_cap, CATS_ENABLE_TRUE);
		CATS_ObjectPosGetCap_SubSurface(move->cap, &x, &y, BALLOON_SUB_ACTOR_DISTANCE);
		move->local_x = x;
		move->local_y = y;
		move->seq++;
		//break;
	case 1:
		switch(move->start_theta){
		case 0:
			offset_y = move->local_y - BOOSTER_APPEAR_START_Y_0;
			add_y = move->local_frame * offset_y / BOOSTER_APPEAR_FRAME;
			y = BOOSTER_APPEAR_START_Y_0 + add_y;
			break;
		case 90<<FX32_SHIFT:
			offset_x = move->local_x - BOOSTER_APPEAR_START_X_90;
			add_x = move->local_frame * offset_x / BOOSTER_APPEAR_FRAME;
			x = BOOSTER_APPEAR_START_X_90 + add_x;
			break;
		case 180<<FX32_SHIFT:
			offset_y = move->local_y - BOOSTER_APPEAR_START_Y_180;
			add_y = move->local_frame * offset_y / BOOSTER_APPEAR_FRAME;
			y = BOOSTER_APPEAR_START_Y_180 + add_y;
			break;
		case 270<<FX32_SHIFT:
			offset_x = move->local_x - BOOSTER_APPEAR_START_X_270;
			add_x = move->local_frame * offset_x / BOOSTER_APPEAR_FRAME;
			x = BOOSTER_APPEAR_START_X_270 + add_x;
			break;
		default:
			GF_ASSERT(0);
			break;
		}
		
		if(move->local_frame >= BOOSTER_APPEAR_FRAME){
			CATS_ObjectPosSetCap_SubSurface(
				move->cap, move->local_x, move->local_y, BALLOON_SUB_ACTOR_DISTANCE);
			CATS_ObjectPosSetCap_SubSurface(
				move->shadow_cap, move->local_x, move->local_y + BOOSTER_SHADOW_OFFSET_Y, 
				BALLOON_SUB_ACTOR_DISTANCE);
			BoosterMoveModeChange(move, BOOSTER_MODE_NORMAL);
			BoosterLandSmoke_SmokeReq(game, move, &move->land_smoke);
			return TRUE;
		}

		jump_theta = ((180 * move->local_frame) << FX32_SHIFT) / BOOSTER_APPEAR_FRAME;
		jump_y = -(FX_Mul(Sin360FX(jump_theta), BOOSTER_APPEAR_JUMP_FURIHABA_Y)) / FX32_ONE;
		CATS_ObjectPosSetCap_SubSurface(move->cap, x, y + jump_y, BALLOON_SUB_ACTOR_DISTANCE);
		CATS_ObjectPosSetCap_SubSurface(move->shadow_cap, x, y + BOOSTER_SHADOW_OFFSET_Y, 
			BALLOON_SUB_ACTOR_DISTANCE);
		check_scale_xy 
			= FX32_ONE - (FX32_ONE * (-jump_y/3) / (BOOSTER_APPEAR_JUMP_FURIHABA_Y >> FX32_SHIFT));
		scale_xy = FX_FX32_TO_F32(check_scale_xy);
		CATS_ObjectScaleSetCap(move->shadow_cap, scale_xy, scale_xy);
		break;
	}
	move->local_frame++;
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   ブースター動作：通常
 *
 * @param   game		
 * @param   booster		ブースター動作制御ワークH
 * @param   move		ブースター動作ワークへのポインタ
 *
 * @retval  
 */
//--------------------------------------------------------------
static int BoosterMove_Normal(BALLOON_GAME_PTR game, BOOSTER_WORK *booster, BOOSTER_MOVE *move)
{
	fx32 base_x, base_y;
	fx32 jump_theta;
	int offset_y = 0;
	fx32 check_scale_xy;
	f32 scale_xy;
	
	if(booster->frame > 0){
		CATS_ObjectUpdateNumCap(move->cap, (BOOSTER_ANIME_FRAME_JUMP_TOTAL << FX32_SHIFT) / BoosterMoveDataTbl[booster->lap].move_frame);
		OS_TPrintf("seq_num = %d\n", CATS_ObjectAnimeSeqGetCap( move->cap ));
		OS_TPrintf("booster anime frame = %d, lap = %d, frame = %d\n", (BOOSTER_ANIME_FRAME_JUMP_TOTAL << FX32_SHIFT) / BoosterMoveDataTbl[booster->lap].move_frame, booster->lap, booster->frame);
	}
	else{
		OS_TPrintf("0 seq_num = %d\n", CATS_ObjectAnimeSeqGetCap( move->cap ));
		OS_TPrintf("0 booster anime frame = %d, lap = %d, frame = %d\n", (BOOSTER_ANIME_FRAME_JUMP_TOTAL << FX32_SHIFT) / BoosterMoveDataTbl[booster->lap].move_frame, booster->lap, booster->frame);
		CATS_ObjectAnimeFrameSetCap(move->cap, 0);
	}
	
	jump_theta = ((180 * booster->frame) << FX32_SHIFT) / BoosterMoveDataTbl[booster->lap].move_frame;
	offset_y = -(FX_Mul(Sin360FX(jump_theta), BOOSTER_JUMP_FURIHABA_Y)) / FX32_ONE;

	base_x = 256/2 + FX_Mul(Sin360FX(booster->theta + move->start_theta), BOOSTER_FURIHABA_X);
	base_y = 196/2 + (-FX_Mul(Cos360FX(booster->theta + move->start_theta), BOOSTER_FURIHABA_Y));
	CATS_ObjectPosSetCap_SubSurface(move->cap, base_x, base_y + BOOSTER_OFFSET_Y + offset_y, 
		BALLOON_SUB_ACTOR_DISTANCE);
	
	//影
	CATS_ObjectPosSetCap_SubSurface(move->shadow_cap, 
		base_x, base_y + BOOSTER_OFFSET_Y + BOOSTER_SHADOW_OFFSET_Y, 
		BALLOON_SUB_ACTOR_DISTANCE);
	check_scale_xy 
		= FX32_ONE - (FX32_ONE * (-offset_y/3) / (BOOSTER_JUMP_FURIHABA_Y >> FX32_SHIFT));
	scale_xy = FX_FX32_TO_F32(check_scale_xy);
	CATS_ObjectScaleSetCap(move->shadow_cap, scale_xy, scale_xy);

	if(booster->frame == BoosterMoveDataTbl[booster->lap].move_frame - 1){
		BoosterLandSmoke_SmokeReq(game, move, &move->land_smoke);
	}
	
	if(((booster->theta + move->start_theta) >> FX32_SHIFT) % 360 == 180){
		move->hit_ok = TRUE;
	}
	else{
		move->hit_ok = FALSE;
	}
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   ブースター動作：ヒット
 *
 * @param   game		
 * @param   booster		ブースター動作制御ワークH
 * @param   move		ブースター動作ワークへのポインタ
 *
 * @retval  
 */
//--------------------------------------------------------------
static int BoosterMove_Hit(BALLOON_GAME_PTR game, BOOSTER_WORK *booster, BOOSTER_MOVE *move)
{
	s16 x, y;
	int next_frame, next_byousin;
	s16 end_x, end_y, start_x, start_y;
	fx32 jump_theta;
	int jump_y = 0;
	
	switch(move->seq){
	case 0:
		Snd_SePlay(SE_BALLOON_BOOSTER_HIT);
		CATS_ObjectPosGetCap_SubSurface(move->cap, &x, &y, BALLOON_SUB_ACTOR_DISTANCE);
		CATS_ObjectPosSetCap_SubSurface(move->hit_cap, x, y + BOOSTER_HIT_OFFSET_Y, 
			BALLOON_SUB_ACTOR_DISTANCE);
		CATS_ObjectAnimeSeqSetCap(move->hit_cap, MIXOBJ_ANMSEQ_BOOSTER_HIT_EFF);//アニメリセット
		CATS_ObjectEnableCap(move->hit_cap, CATS_ENABLE_TRUE);
		
		//本体のアニメを変更
		CATS_ObjectAnimeSeqSetCap(move->cap, 	//1枚目が膨らんでいる絵なので
			BoosterType_StartSeqAnimeNo[move->booster_type] + OFFSET_ANMID_BOOSTER_SHRIVEL);
		
		move->local_frame = BoosterMoveDataTbl[booster->lap].hit_anime_time;
		move->local_wait = BoosterMoveDataTbl[booster->lap].hit_anime_time / 2;
		move->seq++;
		break;
	case 1:
		if(move->local_frame == move->local_wait){
			CATS_ObjectAnimeSeqSetCap(move->cap, 	//1枚目が凹んでいる絵なので
				BoosterType_StartSeqAnimeNo[move->booster_type] + OFFSET_ANMID_BOOSTER_SWELLING);
			CATS_ObjectEnableCap(move->hit_cap, CATS_ENABLE_FALSE);
			CATS_ObjectUpdateNumCap(move->hit_cap, BOOSTER_HITEFF_NEXT_ANM_FRAME);
		}
		if(move->local_frame == 0){
			CATS_ObjectEnableCap(move->hit_cap, CATS_ENABLE_FALSE);	//一応ここでも
			//1枚目がしぼんでいる絵なので
			CATS_ObjectAnimeSeqSetCap(move->cap, 	//1枚目が膨らんでいる絵なので
//				BoosterType_StartSeqAnimeNo[move->booster_type] + OFFSET_ANMID_BOOSTER_SWELLING);
				BoosterType_StartSeqAnimeNo[move->booster_type] + OFFSET_ANMID_BOOSTER_JUMP);
			
			//BoosterMoveModeChange(move, BOOSTER_MODE_NORMAL);
			//return TRUE;
			move->seq++;
			break;
		}
		move->local_frame--;
		break;
	case 2:
		next_frame = 0;
		if(booster->wait == 0){
			next_frame += BoosterMoveDataTbl[booster->lap].move_frame - booster->frame;
			next_frame += BoosterMoveDataTbl[booster->lap].wait_frame;
			next_frame += BoosterMoveDataTbl[booster->lap].move_frame;
			next_byousin = booster->byousin_pos + 2;
		}
		else{
			next_frame += booster->wait;
			if(booster->byousin_pos >= BOOSTER_STOP_PART_NUM){
				next_frame += BoosterMoveDataTbl[booster->lap + 1].move_frame;
				next_byousin = 1;
			}
			else{
				next_frame += BoosterMoveDataTbl[booster->lap].move_frame;
				next_byousin = booster->byousin_pos + 1;
				if(next_byousin >= BOOSTER_STOP_PART_NUM){
//					next_byousin = 0;
				}
			}
		}
		next_byousin += move->start_theta / ((360 / BOOSTER_STOP_PART_NUM) << FX32_SHIFT);
		next_byousin %= BOOSTER_STOP_PART_NUM;
		
		end_x = 256/2 + FX_Mul(Sin360FX((360<<FX32_SHIFT) / BOOSTER_STOP_PART_NUM * next_byousin), BOOSTER_FURIHABA_X);
		end_y = 196/2 + (-FX_Mul(Cos360FX((360<<FX32_SHIFT) / BOOSTER_STOP_PART_NUM * next_byousin), BOOSTER_FURIHABA_Y));
		CATS_ObjectPosGetCap_SubSurface(move->cap, &start_x, &start_y, BALLOON_SUB_ACTOR_DISTANCE);
		start_y -= BOOSTER_OFFSET_Y;
		move->local_add_x = (end_x - start_x) * FX32_ONE / next_frame;
		move->local_add_y = (end_y - start_y) * FX32_ONE / next_frame;
		move->local_fx_x = start_x * FX32_ONE;
		move->local_fx_y = start_y * FX32_ONE;
		move->local_frame = next_frame;
		move->local_work = next_frame;
		move->seq++;
//		OS_TPrintf("move add_x = %d, add_y = %d, fx_x = %d, fx_y = %d, frame = %d, next_byousin = %d\n", move->local_add_x, move->local_add_y, move->local_fx_x, move->local_fx_y, move->local_frame, next_byousin);
		//break;
	case 3:
		move->local_fx_x += move->local_add_x;
		move->local_fx_y += move->local_add_y;
		
		jump_theta = ((180 * move->local_frame) << FX32_SHIFT) / move->local_work;
		jump_y = -(FX_Mul(Sin360FX(jump_theta), BOOSTER_JUMP_FURIHABA_Y)) / FX32_ONE;
		
		CATS_ObjectPosSetCap_SubSurface(move->cap, 
			move->local_fx_x / FX32_ONE, 
			move->local_fx_y / FX32_ONE + BOOSTER_OFFSET_Y + jump_y, 
			BALLOON_SUB_ACTOR_DISTANCE);
		CATS_ObjectUpdateNumCap(move->cap, (BOOSTER_ANIME_FRAME_JUMP_TOTAL << FX32_SHIFT) / move->local_work);

		//影
		CATS_ObjectPosSetCap_SubSurface(move->shadow_cap, 
			move->local_fx_x / FX32_ONE, 
			move->local_fx_y / FX32_ONE + BOOSTER_OFFSET_Y + BOOSTER_SHADOW_OFFSET_Y, 
			BALLOON_SUB_ACTOR_DISTANCE);
		
		move->local_frame--;
		if(move->local_frame <= 0){
			CATS_ObjectAnimeSeqSetCap(move->cap, 
				BoosterType_StartSeqAnimeNo[move->booster_type] + OFFSET_ANMID_BOOSTER_JUMP);
			CATS_ObjectAnimeFrameSetCap(move->cap, 0);
			BoosterMoveModeChange(move, BOOSTER_MODE_NORMAL);
			return TRUE;
		}
		break;
	}
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   SIOブースターアクター生成
 *
 * @param   game		
 * @param   sio_booster		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
void SioBooster_ActorCreateAll(BALLOON_GAME_PTR game, SIO_BOOSTER_WORK *sio_booster)
{
	TCATS_OBJECT_ADD_PARAM_S act_head;
	int i;
	
	act_head = BoosterObjParam;
	for(i = 0; i < SIO_BOOSTER_ACTOR_MAX; i++){
		sio_booster->move[i].cap = CATS_ObjectAdd_S(game->csp, game->crp, &act_head);
		CATS_ObjectPosSetCap_SubSurface(sio_booster->move[i].cap, 
			0, 0, BALLOON_SUB_ACTOR_DISTANCE);
		CATS_ObjectEnableCap(sio_booster->move[i].cap, CATS_ENABLE_FALSE);
		
		//ヒットエフェクト
		sio_booster->move[i].hit_cap = CATS_ObjectAdd_S(game->csp, game->crp, &BoosterHitObjParam);
		CATS_ObjectPosSetCap_SubSurface(sio_booster->move[i].hit_cap, 
			0, 0, BALLOON_SUB_ACTOR_DISTANCE);
		CATS_ObjectAnimeSeqSetCap(sio_booster->move[i].hit_cap, MIXOBJ_ANMSEQ_BOOSTER_HIT_EFF);
		CATS_ObjectUpdate(sio_booster->move[i].hit_cap->act);
		CATS_ObjectEnableCap(sio_booster->move[i].hit_cap, CATS_ENABLE_FALSE);
	}
}

//--------------------------------------------------------------
/**
 * @brief   SIOブースターアクターを全て削除する
 *
 * @param   game		
 * @param   sio_booster		SIOブースターワークへのポインタ
 */
//--------------------------------------------------------------
void SioBooster_ActorDeleteAll(BALLOON_GAME_PTR game, SIO_BOOSTER_WORK *sio_booster)
{
	int i;
	
	for(i = 0; i < SIO_BOOSTER_ACTOR_MAX; i++){
		CATS_ActorPointerDelete_S(sio_booster->move[i].cap);
		CATS_ActorPointerDelete_S(sio_booster->move[i].hit_cap);
	}
}

//--------------------------------------------------------------
/**
 * @brief   SIOブースター登場
 *
 * @param   game		
 * @param   sio_booster		SIOブースターワークへのポインタ
 * @param   booster_type	ブースタータイプ
 * @param   net_id			対象のネットID
 * @param   arrival_frame	空気アクターがジョイント到着までにかかるフレーム数
 */
//--------------------------------------------------------------
void SioBooster_Appear(BALLOON_GAME_PTR game, SIO_BOOSTER_WORK *sio_booster, int booster_type, int net_id, int arrival_frame)
{
	int i, pal_ofs, player_pos, joint_no;
	SIO_BOOSTER_MOVE *sio_move = NULL;
	
	if(booster_type == BOOSTER_TYPE_NONE || net_id == CommGetCurrentID()){
		return;
	}
	for(i = 0; i < SIO_BOOSTER_ACTOR_MAX; i++){
		if(sio_booster->move[i].occ == FALSE){
			sio_move = &sio_booster->move[i];
			break;
		}
	}
	if(sio_move == NULL){
		return;
	}
	
	//パレット設定
	player_pos = Balloon_NetID_to_PlayerPos(game, net_id);
	pal_ofs = PlayerNoPalOfs_Booster[game->bsw->player_max][player_pos];
//	CATS_ObjectPaletteOffsetSetCap(sio_move->cap, pal_ofs);
	CATS_ObjectPaletteSetCap(sio_move->cap, pal_ofs);
	
	//アニメ設定
	CATS_ObjectAnimeSeqSetCap(sio_move->cap, 
		BoosterType_StartSeqAnimeNo[booster_type] + OFFSET_ANMID_BOOSTER_JUMP);
	
	//座標設定
	joint_no = PlayerNoJointNo_SioBooster[game->bsw->player_max][player_pos];
	sio_move->end_y = JointActorPosTbl[joint_no].y + BOOSTER_OFFSET_Y;
	CATS_ObjectPosSetCap_SubSurface(sio_move->cap, 
		JointActorPosTbl[joint_no].x, sio_move->end_y + SIO_BOOSTER_APPEAR_OFFSET_Y,
		BALLOON_SUB_ACTOR_DISTANCE);

	CATS_ObjectUpdate(sio_move->cap->act);
	sio_move->booster_type = booster_type;
	sio_move->start_wait = arrival_frame - (MATH_ABS(SIO_BOOSTER_APPEAR_OFFSET_Y) * FX32_ONE) / SIO_BOOSTER_IN_SPEED;
	sio_move->seq = 0;
	sio_move->occ = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   SIOブースター更新処理
 *
 * @param   game		
 * @param   sio_booster		SIOブースターワークへのポインタ
 */
//--------------------------------------------------------------
void SioBooster_Update(BALLOON_GAME_PTR game, SIO_BOOSTER_WORK *sio_booster)
{
	int i;
	SIO_BOOSTER_MOVE *sio_move;
	
	for(i = 0; i < SIO_BOOSTER_ACTOR_MAX; i++){
		if(sio_booster->move[i].occ == TRUE){
			sio_move = &sio_booster->move[i];
			SioBoosterMove_Appear(sio_booster, sio_move);
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   SIOブースター動作：登場
 *
 * @param   sio_booster		SIOブースターワークへのポインタ
 * @param   sio_move		SIOブースター動作ワークへのポインタ
 *
 * @retval  TRUE:処理継続中、　FALSE:処理終了
 */
//--------------------------------------------------------------
static BOOL SioBoosterMove_Appear(SIO_BOOSTER_WORK *sio_booster, SIO_BOOSTER_MOVE *sio_move)
{
	s16 x, y;
	
	switch(sio_move->seq){
	case 0:		//ワーク初期化
		CATS_ObjectPosGetCap_SubSurface(sio_move->cap, &x, &y, BALLOON_SUB_ACTOR_DISTANCE);
		sio_move->local_fx_x = x * FX32_ONE;
		sio_move->local_fx_y = y * FX32_ONE;
		sio_move->seq++;
		//break;
	case 1:		//落下
		if(sio_move->start_wait > 0){
			sio_move->start_wait--;
			break;
		}
		CATS_ObjectEnableCap(sio_move->cap, CATS_ENABLE_TRUE);
		sio_move->local_fx_y += SIO_BOOSTER_IN_SPEED;
		if((sio_move->local_fx_y / FX32_ONE) >= sio_move->end_y){
			sio_move->local_fx_y = sio_move->end_y * FX32_ONE;
			sio_move->seq++;
		}
		break;
	case 2:		//アニメ
		CATS_ObjectPosGetCap_SubSurface(sio_move->cap, &x, &y, BALLOON_SUB_ACTOR_DISTANCE);
		CATS_ObjectPosSetCap_SubSurface(sio_move->hit_cap, x, y + BOOSTER_HIT_OFFSET_Y, 
			BALLOON_SUB_ACTOR_DISTANCE);
		CATS_ObjectAnimeSeqSetCap(sio_move->hit_cap, MIXOBJ_ANMSEQ_BOOSTER_HIT_EFF);
		CATS_ObjectEnableCap(sio_move->hit_cap, CATS_ENABLE_TRUE);
		Snd_SePlay(SE_BALLOON_BOOSTER_HIT);

		//本体のアニメを変更
		CATS_ObjectAnimeSeqSetCap(sio_move->cap, 	//1枚目が膨らんでいる絵なので
			BoosterType_StartSeqAnimeNo[sio_move->booster_type] + OFFSET_ANMID_BOOSTER_SHRIVEL);
		sio_move->local_frame = 8;
		sio_move->seq++;
		break;
	case 3:
		sio_move->local_frame--;
		if(sio_move->local_frame == 3){
			CATS_ObjectUpdateNumCap(sio_move->hit_cap, BOOSTER_HITEFF_NEXT_ANM_FRAME);
		}
		if(sio_move->local_frame == 0){
			CATS_ObjectEnableCap(sio_move->hit_cap, CATS_ENABLE_FALSE);
			CATS_ObjectAnimeSeqSetCap(sio_move->cap,	//1枚目がしぼんでいる絵なので
				BoosterType_StartSeqAnimeNo[sio_move->booster_type] + OFFSET_ANMID_BOOSTER_JUMP);
			sio_move->seq++;
		}
		break;
	case 4:		//上昇
		sio_move->local_fx_y -= SIO_BOOSTER_OUT_SPEED;
		if((sio_move->local_fx_y / FX32_ONE) <= sio_move->end_y + SIO_BOOSTER_APPEAR_OFFSET_Y){
			CATS_ObjectEnableCap(sio_move->cap, CATS_ENABLE_FALSE);
			sio_move->seq = 0;
			sio_move->occ = FALSE;
			return FALSE;
		}
		break;
	}
	
	CATS_ObjectPosSetCap_SubSurface(sio_move->cap, sio_move->local_fx_x / FX32_ONE, 
		sio_move->local_fx_y / FX32_ONE, BALLOON_SUB_ACTOR_DISTANCE);
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   メイン画面のフォントOAMを作成する
 *
 * @param   bgl				BGLへのポインタ
 * @param   crp				crpへのポインタ
 * @param   fontoam_sys		フォントシステムへのポインタ
 * @param   ret_fontoam		生成したフォントOAM代入先
 * @param   ret_cma			生成したフォントOAMのキャラ領域情報代入先
 * @param   str				文字列
 * @param   font_type		フォントタイプ(FONT_SYSTEM等)
 * @param   color			フォントカラー構成
 * @param   pal_offset		パレット番号オフセット
 * @param   pal_id			登録開始パレットID
 * @param   x				座標X
 * @param   y				座標Y
 * @param   pos_center  	FALSE(X左端座標) or TRUE(X中心座標)
 * @param   bg_pri			BGプライオリティ
 * @param   soft_pri		ソフトプライオリティ
 * @param   y_char_len		BMPのYサイズ(キャラ単位)
 */
//--------------------------------------------------------------
void BalloonTool_FontOamCreate(GF_BGL_INI *bgl, CATS_RES_PTR crp, 
	FONTOAM_SYS_PTR fontoam_sys, BALLOON_FONTACT *fontact, const STRBUF *str, 
	FONT_TYPE font_type, GF_PRINTCOLOR color, int pal_offset, int pal_id, 
	int x, int y, int pos_center, int bg_pri, int soft_pri, int y_char_len)
{
	FONTOAM_INIT finit;
	GF_BGL_BMPWIN bmpwin;
	CHAR_MANAGER_ALLOCDATA cma;
	int vram_size;
	FONTOAM_OBJ_PTR fontoam;
	int font_len, char_len;
	int margin = 0;
	
	//文字列のドット幅から、使用するキャラ数を算出する
	{
		font_len = FontProc_GetPrintStrWidth(font_type, str, margin);
		char_len = font_len / 8;
		if(FX_ModS32(font_len, 8) != 0){
			char_len++;
		}
	}

	//BMP作成
	{
		GF_BGL_BmpWinInit(&bmpwin);
		GF_BGL_BmpWinObjAdd(bgl, &bmpwin, char_len, y_char_len, 0, 0);
		GF_STR_PrintExpand(&bmpwin, font_type, str, 0, 0, MSG_NO_PUT, color, 
			margin, 0, NULL);
//		GF_STR_PrintColor(&bmpwin, font_type, str, 0, 0, MSG_NO_PUT, color, NULL );
	}

	vram_size = FONTOAM_NeedCharSize(&bmpwin, NNS_G2D_VRAM_TYPE_2DMAIN,  HEAPID_BALLOON);
	CharVramAreaAlloc(vram_size, CHARM_CONT_AREACONT, NNS_G2D_VRAM_TYPE_2DMAIN, &cma);
	
	//座標位置修正
	if(pos_center == TRUE){
		x -= font_len / 2;
	}
	y += MAIN_SURFACE_Y_INTEGER;// - 8;
	
	finit.fontoam_sys = fontoam_sys;
	finit.bmp = &bmpwin;
	finit.clact_set = CATS_GetClactSetPtr(crp);
	finit.pltt = CATS_PlttProxy(crp, pal_id);
	finit.parent = NULL;
	finit.char_ofs = cma.alloc_ofs;
	finit.x = x;
	finit.y = y;
	finit.bg_pri = bg_pri;
	finit.soft_pri = soft_pri;
	finit.draw_area = NNS_G2D_VRAM_TYPE_2DMAIN;
	finit.heap = HEAPID_BALLOON;
	
	fontoam = FONTOAM_Init(&finit);
	if(pal_offset != 0){
		FONTOAM_SetPaletteOffset(fontoam, pal_offset);
	}
	FONTOAM_SetMat(fontoam, x, y);
	
	//解放処理
	GF_BGL_BmpWinDel(&bmpwin);
	
	fontact->fontoam = fontoam;
	fontact->cma = cma;
	fontact->len = font_len;
}

//--------------------------------------------------------------
/**
 * @brief   フォントOAMを削除する
 * @param   fontact		フォントアクターへのポインタ
 */
//--------------------------------------------------------------
void Balloon_FontOamDelete(BALLOON_FONTACT *fontact)
{
	FONTOAM_Delete(fontact->fontoam);
	CharVramAreaFree(&fontact->cma);
	
	fontact->fontoam = NULL;
}

//--------------------------------------------------------------
/**
 * @brief   カウンターの座標更新
 *
 * @param   counter		カウンター動作制御ワークへのポインタ
 */
//--------------------------------------------------------------
void Balloon_CounterPosUpdate(BALLOON_COUNTER *counter)
{
	int i, dot_pos;
	int act_len = 16*5;	//1つのアクターの長さ
	int x, y, y0, y1;
	
	for(i = 0; i < BALLOON_COUNTER_KETA_MAX; i++){
		dot_pos = counter->number[i];
		if(dot_pos < act_len - 16){
			y0 = act_len - 16 - dot_pos;
			y1 = y0 - act_len;
		}
//		else if(dot_pos > act_len*2 - 32){
//			y0 = -32 - dot_pos;
//			y1 = y0 - act_len;
//		}
		else if(dot_pos < 160-32){
			y1 = act_len - 16 - (dot_pos - act_len);
			y0 = y1 - act_len;
		}
		else{
			y0 = act_len - 16 - (dot_pos - 160);
			y1 = y0 - act_len;
		}
		FONTOAM_GetMat(counter->fontact[i][BALLOON_COUNTER_0].fontoam, &x, &y);
		FONTOAM_SetMat(counter->fontact[i][BALLOON_COUNTER_0].fontoam, x, COUNTER_Y - y0);
		FONTOAM_SetMat(counter->fontact[i][BALLOON_COUNTER_1].fontoam, x, COUNTER_Y - y1);
	}
}

//--------------------------------------------------------------
/**
 * @brief   カウンター更新処理
 *
 * @param   game		
 * @param   counter		カウンター動作制御ワークへのポインタ
 */
//--------------------------------------------------------------
void Balloon_CounterUpdate(BALLOON_GAME_PTR game, BALLOON_COUNTER *counter)
{
	int i;
	BOOL rotate_ret;
	
	if(counter->wait > 0){
		counter->wait--;
		return;
	}
	
	switch(counter->seq){
	case 0:
		if(Balloon_CounterAgreeCheck(counter) == TRUE){
			Balloon_CounterLastNumberSet(counter);
			break;
		}
		else{
			counter->seq++;
		}
		//break;
	case 1:
		rotate_ret = Balloon_CounterRotate(counter);
		Balloon_CounterPosUpdate(counter);
		if(rotate_ret == TRUE){
			counter->wait = COUNTER_AGREE_AFTER_WAIT;
			counter->seq--;
		}
		break;
	}
}

//--------------------------------------------------------------
/**
 * @brief   カウンターの次に表示させる数値をリクエストしておく
 *
 * @param   counter		
 * @param   next_number		数値
 */
//--------------------------------------------------------------
void Balloon_CounterNextNumberSet(BALLOON_COUNTER *counter, int next_number)
{
	int i, div_num, number;
	
	div_num = 100000;
	GF_ASSERT(BALLOON_COUNTER_KETA_MAX == 6);	//ketaが変わっていたらdiv_numの初期値も変更
	
	for(i = 0; i < BALLOON_COUNTER_KETA_MAX; i++){
		number = next_number / div_num;
		counter->next_number[i] = number * 16;
		next_number -= number * div_num;
		div_num /= 10;
	}
}

//--------------------------------------------------------------
/**
 * @brief   カウンターの表示させる数値をリクエストがかかっている数値で更新する
 *
 * @param   counter		
 */
//--------------------------------------------------------------
static void Balloon_CounterLastNumberSet(BALLOON_COUNTER *counter)
{
	int i;
	
	for(i = 0; i < BALLOON_COUNTER_KETA_MAX; i++){
		counter->last_number[i] = counter->next_number[i];
		counter->move_wait[i] = COUNTER_MOVE_START_WAIT * (BALLOON_COUNTER_KETA_MAX - i - 1);
		counter->rotate_count[i] = 0;
	}
	counter->move_keta = 0;
}

//--------------------------------------------------------------
/**
 * @brief   カウンターの数値が表示数値と一致しているか確認
 *
 * @param   counter		
 *
 * @retval  TRUE:一致。　FALSE：不一致
 */
//--------------------------------------------------------------
static BOOL Balloon_CounterAgreeCheck(BALLOON_COUNTER *counter)
{
	int i;
	
	for(i = 0; i < BALLOON_COUNTER_KETA_MAX; i++){
		if(counter->number[i] != counter->last_number[i]){
			return FALSE;
		}
	}
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   カウンター回転処理
 *
 * @param   counter		
 *
 * @retval  TRUE：回転動作終了
 * @retval  FALSE：回転動作継続中
 */
//--------------------------------------------------------------
static BOOL Balloon_CounterRotate(BALLOON_COUNTER *counter)
{
	int i;
	int pos = BALLOON_COUNTER_KETA_MAX - 1;
	int before_number, mae_stop, stop, over_last_number;
	
	if(counter->move_keta > BALLOON_COUNTER_KETA_MAX){
		return TRUE;
	}
	
	mae_stop = FALSE;
	stop = FALSE;
	for(i = 0; i < BALLOON_COUNTER_KETA_MAX; i++, pos--){
		mae_stop = stop;

		if(counter->move_wait[pos] > 0){
			counter->move_wait[pos]--;
			stop = FALSE;
			continue;
		}
		
		if(counter->rotate_count[pos] > COUNTER_ROTATE_NUM 
				&& counter->number[pos] == counter->last_number[pos]){
			stop = TRUE;
			continue;
		}
		stop = FALSE;
		
		before_number = counter->number[pos];
		counter->number[pos] += COUNTER_SPEED;
		over_last_number = counter->last_number[pos] + 16*10;	//桁上がりチェック用

		if((before_number <= counter->last_number[pos] 
				&& counter->number[pos] >= counter->last_number[pos])
				|| (before_number <= over_last_number 
				&& counter->number[pos] >= over_last_number)){
			//一周した
			if(counter->rotate_count[pos] < COUNTER_ROTATE_NUM){
				counter->rotate_count[pos]++;
			}
			else if(pos == BALLOON_COUNTER_KETA_MAX - 1 || mae_stop == TRUE){
				counter->rotate_count[pos]++;
				counter->number[pos] = counter->last_number[pos];
				counter->move_keta++;
				if(counter->move_keta >= BALLOON_COUNTER_KETA_MAX){
					return TRUE;
				}
			}
		}
		counter->number[pos] %= 10*16;
	}
	
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   カウンターウィンドウアクター生成
 *
 * @param   game		
 *
 * @retval  生成した破裂アクターへのポインタ
 */
//--------------------------------------------------------------
CATS_ACT_PTR CounterWindow_ActorCreate(BALLOON_GAME_PTR game)
{
	CATS_ACT_PTR cap;

	//-- アクター生成 --//
	cap = CATS_ObjectAdd_S(game->csp, game->crp, &CounterWindowObjParam);
	CATS_ObjectUpdate(cap->act);
	return cap;
}

//--------------------------------------------------------------
/**
 * @brief   カウンターダミーアクター生成
 *
 * @param   game		
 */
//--------------------------------------------------------------
void CounterDummyNumber_ActorCreate(BALLOON_GAME_PTR game)
{
	//-- カウンター --//
	STRBUF *str0;
	int i;
	u32 number;
	
	number = game->my_total_air;
	for(i = 0; i < BALLOON_COUNTER_KETA_MAX; i++){
		GF_ASSERT(game->counter.fontact_dummy[i].fontoam == NULL);
		str0 = MSGMAN_AllocString(game->msgman, msg_balloon_num0 + (number % 10));
		number /= 10;
		BalloonTool_FontOamCreate(game->bgl, game->crp, game->fontoam_sys,
			&game->counter.fontact_dummy[i], str0, FONT_SYSTEM, 
			COUNTER_FONT_COLOR, 0, PLTTID_COUNTER, 
			(COUNTER_BASE_X + COUNTER_X_SPACE * (BALLOON_COUNTER_KETA_MAX-1)) - i*COUNTER_X_SPACE,
			COUNTER_Y, FALSE, BALLOON_BGPRI_DUMMY_COUNTER, BALLOON_SOFTPRI_COUNTER, 2*1);
		STRBUF_Delete(str0);
	}
}

//--------------------------------------------------------------
/**
 * @brief   カウンターダミーアクター削除
 *
 * @param   game		
 */
//--------------------------------------------------------------
void CounterDummyNumber_ActorDelete(BALLOON_GAME_PTR game)
{
	int i;
	
	for(i = 0; i < BALLOON_COUNTER_KETA_MAX; i++){
		if(game->counter.fontact_dummy[i].fontoam != NULL){
			Balloon_FontOamDelete(&game->counter.fontact_dummy[i]);
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   風船アイコンを全て削除する
 *
 * @param   game		
 */
//--------------------------------------------------------------
void CounterWindow_ActorDelete(BALLOON_GAME_PTR game, CATS_ACT_PTR cap)
{
	CATS_ActorPointerDelete_S(cap);
}

//--------------------------------------------------------------
/**
 * @brief   タッチペンアクター生成
 *
 * @param   game		
 */
//--------------------------------------------------------------
CATS_ACT_PTR TouchPen_ActorCreate(BALLOON_GAME_PTR game)
{
	CATS_ACT_PTR cap;

	cap = CATS_ObjectAdd_S(game->csp, game->crp, &TouchPenObjParam);
//	CATS_ObjectPosSetCap_SubSurface(cap, x, y, BALLOON_SUB_ACTOR_DISTANCE);
	CATS_ObjectEnableCap(cap, CATS_ENABLE_FALSE);
	CATS_ObjectUpdate(cap->act);
	return cap;
}

//--------------------------------------------------------------
/**
 * @brief   タッチペンアクターを全て削除する
 *
 * @param   game		
 */
//--------------------------------------------------------------
void TouchPen_ActorDelete(BALLOON_GAME_PTR game, CATS_ACT_PTR cap)
{
	CATS_ActorPointerDelete_S(cap);
}

//--------------------------------------------------------------
/**
 * @brief   タッチペンデモ動作
 *
 * @param   game		
 * @param   pen			タッチペン動作制御ワークへのポインタ
 *
 * @retval  TRUE:デモ終了
 * @retval  FALSE:デモ継続中
 */
//--------------------------------------------------------------
BOOL BalloonTool_TouchPenDemoMove(BALLOON_GAME_PTR game, BALLOON_PEN *pen)
{
	switch(pen->seq){
	case 0:
	case 5:
		CATS_ObjectPosSetCap_SubSurface(pen->cap, 
			DEMO_PEN_X, DEMO_PEN_Y, BALLOON_SUB_ACTOR_DISTANCE);
		CATS_ObjectEnableCap(pen->cap, CATS_ENABLE_TRUE);
		pen->seq++;
		break;
	case 1:
	case 6:
		pen->wait++;
		if(pen->wait > DEMO_PEN_START_WAIT){
			pen->wait = 0;
			pen->tp_cont = TRUE;
			pen->tp_trg = TRUE;
			pen->tp_x = 128;
			pen->tp_y = 32;
			pen->seq++;
		}
		break;
	case 2:
	case 7:
		pen->tp_trg = FALSE;
		pen->tp_y += DEMO_PEN_SPEED;
		CATS_ObjectPosMoveCap(pen->cap, 0, DEMO_PEN_SPEED);
		if(pen->tp_y > 192 - 96){
			pen->seq++;
		}
		break;
	case 3:
	case 8:
		pen->tp_cont = FALSE;
		CATS_ObjectEnableCap(pen->cap, CATS_ENABLE_FALSE);
		pen->seq++;
		//break;
	case 4:
	case 9:
		pen->wait++;
		if(pen->wait > DEMO_PEN_AFTER_WAIT){
			pen->wait = 0;
			pen->seq++;
		}
		break;
	case 10:
		return TRUE;
	}
	
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   ブースターの着地時の煙アクター生成
 *
 * @param   game		
 */
//--------------------------------------------------------------
static void BoosterLandSmoke_ActorCreate(BALLOON_GAME_PTR game, BOOSTER_LAND_SMOKE *land_smoke)
{
	CATS_ACT_PTR cap;
	int i;
	
	for(i = 0; i < BOOSTER_LAND_SMOKE_NUM; i++){
		cap = CATS_ObjectAdd_S(game->csp, game->crp, &BoosterLandSmokeObjParam);
	//	CATS_ObjectPosSetCap_SubSurface(cap, x, y, BALLOON_SUB_ACTOR_DISTANCE);
		CATS_ObjectAnimeSeqSetCap(cap, MIXOBJ_ANMSEQ_BOOSTER_LAND_SMOKE);
		CATS_ObjectObjModeSetCap(cap, GX_OAM_MODE_XLU);	//半透明ON
		CATS_ObjectEnableCap(cap, CATS_ENABLE_FALSE);
		CATS_ObjectUpdate(cap->act);
		
		land_smoke->cap[i] = cap;
	}
}

//--------------------------------------------------------------
/**
 * @brief   ブースターの着地時の煙アクター削除
 *
 * @param   game		
 */
//--------------------------------------------------------------
static void BoosterLandSmoke_ActorDelete(BALLOON_GAME_PTR game, BOOSTER_LAND_SMOKE *land_smoke)
{
	int i;
	
	for(i = 0; i < BOOSTER_LAND_SMOKE_NUM; i++){
		CATS_ActorPointerDelete_S(land_smoke->cap[i]);
	}
}

//--------------------------------------------------------------
/**
 * @brief   ブースター着地煙の動作をリクエスト
 *
 * @param   game		
 * @param   move			ブースター動作制御ワークへのポインタ
 * @param   land_smoke		着地煙動作制御ワークへのポインタ
 */
//--------------------------------------------------------------
static void BoosterLandSmoke_SmokeReq(BALLOON_GAME_PTR game, BOOSTER_MOVE *move, BOOSTER_LAND_SMOKE *land_smoke)
{
	s16 x, y;
	int i;
	
	CATS_ObjectPosGetCap_SubSurface(move->cap, &x, &y, BALLOON_SUB_ACTOR_DISTANCE);
	
	for(i = 0; i < BOOSTER_LAND_SMOKE_NUM; i++){
		CATS_ObjectPosSetCap_SubSurface(land_smoke->cap[i], x, y + BOOSTER_LAND_SMOKE_OFFSET_Y, 
			BALLOON_SUB_ACTOR_DISTANCE);
		CATS_ObjectAnimeSeqSetCap(land_smoke->cap[i], 
			//MIXOBJ_ANMSEQ_BOOSTER_LAND_SMOKE + gf_rand() % BOOSTER_LAND_SMOKE_ANM_PATERN);
			MIXOBJ_ANMSEQ_BOOSTER_LAND_SMOKE);
		CATS_ObjectObjModeSetCap(land_smoke->cap[i], GX_OAM_MODE_XLU);	//半透明ON
		CATS_ObjectEnableCap(land_smoke->cap[i], CATS_ENABLE_TRUE);
		//CATS_ObjectUpdate(land_smoke->cap[i]->act);

		land_smoke->add_x[i] = gf_rand() % 0x1000 + 0x1000;
		land_smoke->add_y[i] = gf_rand() % 0x400 + 0x400;
		if(i & 1){
			land_smoke->add_x[i] *= -1;
		}
	}
	land_smoke->time = 0;
	land_smoke->seq = 1;
}

//--------------------------------------------------------------
/**
 * @brief   ブースター着地煙の更新処理
 *
 * @param   game			
 * @param   move			ブースター動作制御ワークへのポインタ
 * @param   land_smoke		着地煙動作制御ワークへのポインタ
 */
//--------------------------------------------------------------
static void BoosterLandSmoke_Update(BALLOON_GAME_PTR game, BOOSTER_MOVE *move, BOOSTER_LAND_SMOKE *land_smoke)
{
	int i;
	
	switch(land_smoke->seq){
	case 0:
		break;
	case 1:
		for(i = 0; i < BOOSTER_LAND_SMOKE_NUM; i++){
			CATS_ObjectPosMoveCapFx32(land_smoke->cap[i], 
				land_smoke->add_x[i], -land_smoke->add_y[i]);
			CATS_ObjectUpdate(land_smoke->cap[i]->act);
		}
		land_smoke->time++;
		if(land_smoke->time > BOOSTER_LAND_SMOKE_DELETE_TIME){
			for(i = 0; i < BOOSTER_LAND_SMOKE_NUM; i++){
				CATS_ObjectEnableCap(land_smoke->cap[i], CATS_ENABLE_FALSE);
			}
			land_smoke->seq = 0;
		}
		break;
	}
}

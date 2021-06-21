//==============================================================================
/**
 * @file	balloon_sonans.c
 * @brief	風船割りゲームのソーナンス制御ルーチン
 * @author	matsuda
 * @date	2007.11.13(火)
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
#include "balloon_send_recv.h"
#include "balloon_control.h"
#include "balloon_tool.h"
#include "balloon_snd_def.h"

#include "graphic/balloon_gra_def.h"
#include "balloon_id.h"
#include "../../particledata/pl_etc/pl_etc_particle_def.h"
#include "../../particledata/pl_etc/pl_etc_particle_lst.h"


//==============================================================================
//	定数定義
//==============================================================================
///ソーナンスのY方向に連なる連結四角形ポリゴンの数
#define SONANS_QUAD_STRIP_NUM		(8)
///ソーナンスに連なっている、いくつまでのポリゴンが可変対象になるか(残りは動かなくなる)
#define SONANS_VARIABLE_VERTEX_NUM	(SONANS_QUAD_STRIP_NUM - 1)

///ソーナンスの頂点座標リストの長さ(fx32)
#define SONANS_VERTEX_LEN			(128 * FX32_ONE)
///ソーナンスの頂点座標リストの長さ(fx32)
#define SONANS_VERTEX_LEN_HALF		(SONANS_VERTEX_LEN/2)
///ソーナンスの可変対象ポリゴンの頂点座標リスト長
#define SONANS_VARIABLE_VERTEX_LEN	(SONANS_VERTEX_LEN / SONANS_QUAD_STRIP_NUM * SONANS_VARIABLE_VERTEX_NUM)
///ソーナンスの潰れる長さ
#define SONANS_VERTEX_PUSH_LEN		(SONANS_VARIABLE_VERTEX_LEN - 12*FX32_ONE)

///ソーナンスの左端頂点座標X
#define SONANS_VERTEX_LEFT			(0)
///ソーナンスの右端頂点座標X
#define SONANS_VERTEX_RIGHT			(SONANS_VERTEX_LEN)
///ソーナンスの下端頂点座標X
#define SONANS_VERTEX_TOP			(0)
///ソーナンスの上端頂点座標X
#define SONANS_VERTEX_BOTTOM		(-SONANS_VERTEX_LEN)

///ソーナンスのテクスチャサイズ:X
#define SONANS_TEX_SIZE_X			(128)
///ソーナンスのテクスチャサイズ:Y
#define SONANS_TEX_SIZE_Y			(128)

///ソーナンスのスケールサイズX　※頂点がfx16の為、頂点はFX16_ONEにしておきスケールでその分拡大する
#define SONANS_SCALE_SIZE_X			(6 * 16*FX32_ONE)	//6grid(1grid=16unit)
///ソーナンスのスケールサイズY
#define SONANS_SCALE_SIZE_Y			(6 * 16*FX32_ONE)
///ソーナンスのスケールサイズZ
#define SONANS_SCALE_SIZE_Z			(FX32_ONE)

///ソーナンスのスケールサイズX(レア：細)
#define SONANS_SCALE_SIZE_FINE_X		(SONANS_SCALE_SIZE_X - 2*16*FX32_ONE)
///ソーナンスのスケールサイズY(レア：細)
#define SONANS_SCALE_SIZE_FINE_Y		(SONANS_SCALE_SIZE_Y + 1*16*FX32_ONE/2)
///ソーナンスのスケールサイズX(レア：太)
#define SONANS_SCALE_SIZE_THICK_X		(SONANS_SCALE_SIZE_X + 3*16*FX32_ONE)
///ソーナンスのスケールサイズY(レア：太)
#define SONANS_SCALE_SIZE_THICK_Y		(SONANS_SCALE_SIZE_Y + 1*16*FX32_ONE)

///ソーナンスの照準の初期座標X(2D的なドット位置)
#define SONANS_AIM_INIT_X		(128)
///ソーナンスの照準の初期座標Y(2D的なドット位置)
#define SONANS_AIM_INIT_Y		(40)
///ソーナンスの照準の終端座標Y(2D的なドット位置)
#define SONANS_AIM_END_Y		(256-48)

///ソーナンスのテクスチャグラフィックの足元の空白サイズ(ユニット単位)
#define SONANS_TEX_SPACE_SIZE	(9)
///台座の縦サイズ(ユニット単位)
#define DAIZA_UNIT_SIZE			(10)

///照準の当たり判定X(中心からの半径)
#define AIM_RANGE_HALF_X		(56)
///照準の当たり判定Y(中心からの半径)
#define AIM_RANGE_HALF_Y		(16)
///照準の中心を押している、という判定をする半径
#define AIM_CENTER_RANGE_HALF_X		(16)	//(AIM_RANGE_HALF_X / 2)

///照準にヒットした時の位置
enum{
	AIM_HIT_CENTER,			///<照準の中心にヒット
	AIM_HIT_LEFT,			///<照準の左側にヒット
	AIM_HIT_RIGHT,			///<照準の右側にヒット
};

///ソーナンス最大時の面積
#define SONANS_MAX_AREA		(((((SONANS_VERTEX_RIGHT-SONANS_VERTEX_LEFT) + (SONANS_VERTEX_RIGHT-SONANS_VERTEX_LEFT)) * (fx64)(SONANS_VERTEX_PUSH_LEN)) >> FX32_SHIFT) / 2)

///ボルテージが最大の時、ソーナンスを最大何％まで大きくするか(100で2倍):X
#define SONANS_VOLTAGE_SCALE_X_MAX	(30)
///ボルテージが最大の時、ソーナンスを1フレームごとに何％ずつ大きくするか:X
#define SONANS_VOLTAGE_SCALE_ADD_X	(8)


///ボルテージが最大の時の点滅アニメのウェイト間隔
#define VOLTAGE_ANM_FLASH_WAIT			(4)

//==============================================================================
//	構造体定義
//==============================================================================
///fx16のPOINT型
typedef struct{
	fx16 x;
	fx16 y;
}POINT_FX16;

///fx32のPOINT型
typedef struct{
	fx32 x;
	fx32 y;
}POINT_FX32;

///s32のPOINT型
typedef struct{
	s32 x;
	s32 y;
}POINT_S32;

///頂点座標リスト
typedef struct{
	POINT_FX32 lu;			///<頂点座標：左上
	POINT_FX32 ld;			///<頂点座標：左下
	POINT_FX32 ru;			///<頂点座標：右上
	POINT_FX32 rd;			///<頂点座標：右下
}VERTEX_LIST;

///テクスチャ頂点座標リスト
typedef struct{
	POINT_FX32 lu;			///<頂点座標：左上
	POINT_FX32 ld;			///<頂点座標：左下
	POINT_FX32 ru;			///<頂点座標：右上
	POINT_FX32 rd;			///<頂点座標：右下
}TEX_COORD_LIST;

//--------------------------------------------------------------
/**
 * @brief   ソーナンス動作制御用システムワーク
 */
//--------------------------------------------------------------
typedef struct _SONANS_SYS{
	NNSG3dResFileHeader *tex_file;			///<読み込んだテクスチャーファイルへのポインタ
	NNSG3dResTex	*texture;
	u16 rotate;
	VERTEX_LIST vertex_list[SONANS_QUAD_STRIP_NUM];
	TEX_COORD_LIST tex_coord_list[SONANS_QUAD_STRIP_NUM];
	
	int tp_x;
	int tp_y;
	int tp_cont;
	int tp_trg;
	
	int aim_x;
	int aim_y;
	int aim_hit_x;
	int aim_hit_y;
	int aim_hit_y_zure;				///<照準の中心からいくつずれてタッチしたかを保持(Y座標)
	fx32 aim_hit_center_y_fx;
	CATS_ACT_PTR aim_cap;			///<照準アクターへのポインタ
	
	int push_mode;
	int backup_tp_y;
	
	int before_area;				///<タッチ開始時の面積
	int pomp_area;					///<タッチを離した時に得た、最終的に押し込んだ面積
	
	fx32 return_power;				///<ソーナンスを戻す時のパワー値

	u16 *palette_work_src;			///<ソーナンスのパレットワーク(元データ)
	u16 *palette_work_dest;			///<ソーナンスのパレットワーク(転送データ)
	u32 palette_size;				///<ソーナンスのカラーパレットサイズ(byte)
	u32 palette_trans_adrs;			///<パレット転送先のアドレス
	
	s32 voltage;					///<ソーナンスのボルテージ値
	
	fx32 scale_x;
	fx32 scale_y;
	s16 scale_voltage_x;
	s16 scale_voltage_y;
	
	s16 rare_scale_x;				///<レアゲームによるスケール値:X
	s16 rare_scale_y;				///<レアゲームによるスケール値:Y
	
	MNGM_RAREGAME_TYPE rare_type;	///<レアタイプ
	
	u8 snd_push_se;					///<TRUE:ソーナンスを押しているSE再生中
	u8 voltage_anm_wait;			///<ボルテージ最大の時のアニメウェイト
	u8 color_trans_req;				///<TRUE:Vブランクでパレット転送リクエスト
	
#ifdef PM_DEBUG
	int debug_aim_on_off;			///<TRUE：照準表示　　FALSE:照準非表示
#endif
}SONANS_SYS;


//==============================================================================
//	型定義
//==============================================================================
///Sonans_VertexCalc関数の戻り値
typedef enum{
	VERTEX_CALC_RET_TRUE,			///<正常終了
	VERTEX_CALC_RET_RELEASE,		///<ペンのタッチ状態を解放させる必要がある
	VERTEX_CALC_RET_CARRY_OVER,		///<上限、下限計算で補正が発生した
}VERTEX_CALC_RET;


//==============================================================================
//	データ
//==============================================================================
static const fx16	 gCubeGeometryFx[] = {
	-FX16_HALF, FX16_HALF, 0,
	-FX16_HALF, -FX16_HALF, 0,
	FX16_HALF, FX16_HALF, 0,
	FX16_HALF, -FX16_HALF, 0,

	(FX16_ONE + FX16_ONE*2), FX16_ONE, 0,
	(FX16_ONE + FX16_ONE*2), -FX16_ONE, 0,

	(FX16_ONE + FX16_ONE*4), FX16_ONE, 0,
	(FX16_ONE + FX16_ONE*4), -FX16_ONE, 0,


	FX16_ONE, FX16_ONE, 0,
	FX16_ONE, FX16_ONE*2, 0,
	-FX16_ONE, FX16_ONE, 0,
	-FX16_ONE, FX16_ONE*2, 0,
};

static const fx16 gCubeNormalFx[3 * 6] = {
	0, 0, FX16_ONE - 1,
	0, FX16_ONE - 1, 0,
	FX16_ONE - 1, 0, 0,
	0, 0, -FX16_ONE + 1,
	0, -FX16_ONE + 1, 0,
	-FX16_ONE + 1, 0, 0,
};

static const fx32 gCubeTexCoordFx[] = {	//s, t
	0, 0,
	0, 128*FX32_ONE,
	128*FX32_ONE, 0,
	128*FX32_ONE, 128*FX32_ONE,

	64*FX32_ONE, 0,
	64*FX32_ONE, 128*FX32_ONE,
	96*FX32_ONE, 0,
	96*FX32_ONE, 128*FX32_ONE,
};


//--------------------------------------------------------------
//	
//--------------------------------------------------------------
///照準のアクターヘッダ(メイン画面用)
static const TCATS_OBJECT_ADD_PARAM_S AimObjParam = {
	0, 0, 0,		//x, y, z
	0, BALLOON_SOFTPRI_AIM, 0,	//アニメ番号、優先順位、パレット番号
	NNS_G2D_VRAM_TYPE_2DMAIN,		//描画エリア
	{	//使用リソースIDテーブル
		CHARID_BALLOON_AIM,			//キャラ
		PLTTID_OBJ_AIM,			//パレット
		CELLID_BALLOON_AIM,			//セル
		CELLANMID_BALLOON_AIM,		//セルアニメ
		CLACT_U_HEADER_DATA_NONE,		//マルチセル
		CLACT_U_HEADER_DATA_NONE,		//マルチセルアニメ
	},
	BALLOON_BGPRI_AIM,				//BGプライオリティ
	0,									//Vram転送フラグ
};


//--------------------------------------------------------------
//	
//--------------------------------------------------------------
///ソーナンスのあたり判定
static const struct{
	u8 left;
	u8 right;
	u8 top;
	u8 bottom;
}SonansHitRect[] = {	//ノーマル、レア１、レア２
	{64, 192, 16, 168},		//MNGM_RAREGAME_BALLOON_NORMAL
	{40, 256-40, 0, 168},		//MNGM_RAREGAME_BALLOON_THICK
	{84, 256-84, 8, 168},		//MNGM_RAREGAME_BALLOON_FINE
};

//==============================================================================
//	プロトタイプ宣言
//==============================================================================
static void Sonas_TexLoad(SONANS_SYS *sns);
static void Sonans_VertexListInit(SONANS_SYS_PTR sns);
static void Sonans_TexCoordListInit(SONANS_SYS_PTR sns);
static void Sonans_VertexListUpdate(SONANS_SYS_PTR sns);
static void Sonans_PolygonCreate(SONANS_SYS_PTR sns);
static BOOL Sonans_AimHitCheck(SONANS_SYS_PTR sns);
static int Sonans_VertexUpdate(SONANS_SYS_PTR sns);
static VERTEX_CALC_RET Sonans_VertexCalc(SONANS_SYS_PTR sns, int air_back_mode, fx32 dot_offset);
static int Sonans_AimHitPosGet(SONANS_SYS_PTR sns, int air_back_mode);
static int VertexCalc_TouchPower(SONANS_SYS_PTR sns, fx32 dot_offset, 
	fx32 now_y, fx32 now_reverse_y, fx32 *new_y, fx32 *new_reverse_y);
static fx32 Sonans_AreaGet(SONANS_SYS_PTR sns);
static int Sonans_NowAreaParcentGet(SONANS_SYS_PTR sns);
static void Sonans_Vertex_FX32_to_Conv(fx32 x, fx32 y, fx16 *ret_x, fx16 *ret_y);
static void Sonans_PushModeReset(SONANS_SYS_PTR sns);
static int Sonans_PompArea_to_AirPower(SONANS_SYS_PTR sns, int pomp_area);
static void Sonans_ReturnPowerCalc(SONANS_SYS_PTR sns, int pomp_area);
static void Sonans_PaletteTrans(SONANS_SYS_PTR sns);
static void Sonans_VoltagePaletteUpdate(SONANS_SYS_PTR sns, int voltage);
static BOOL Sonans_VoltageAdd(SONANS_SYS_PTR sns, int pomp_area);
static void Sonans_VoltageReset(SONANS_SYS_PTR sns);
static void Sonans_VoltageMaxAnime(SONANS_SYS_PTR sns);
static void Sonans_ParticleSet(BALLOON_GAME_PTR game, SONANS_SYS_PTR sns, int pomp_area, int voltage_max);

#define DEBUG_MAT
#ifdef DEBUG_MAT
void DEMO_Set3DDefaultMaterial(BOOL bUsediffuseAsVtxCol, BOOL bUseShininessTbl);
void DEMO_Set3DDefaultShininessTable();
#endif

static void Aim_ResourceLoad(BALLOON_GAME_PTR game);
static void Aim_ResourceFree(BALLOON_GAME_PTR game);
static CATS_ACT_PTR Aim_ActorCreate(BALLOON_GAME_PTR game);
static void Aim_Update(SONANS_SYS_PTR sns, CATS_ACT_PTR cap);
static void Aim_ActorDelete(CATS_ACT_PTR cap);


//==============================================================================
//	外部データ
//==============================================================================
extern const s32 AirRareRevision[];


//==============================================================================
//
//	インライン関数
//
//==============================================================================
inline void vtx(int idx)
{
	NNS_G3dGeVtx(gCubeGeometryFx[idx * 3], 
		gCubeGeometryFx[idx * 3 + 1], gCubeGeometryFx[idx * 3 + 2]);
}

inline void normal(int idx)
{
	NNS_G3dGeNormal(gCubeNormalFx[idx * 3], gCubeNormalFx[idx * 3 + 1],gCubeNormalFx[idx * 3 + 2]);
}

inline void tex_coord(int idx)
{
	NNS_G3dGeTexCoord(gCubeTexCoordFx[idx * 2], gCubeTexCoordFx[idx * 2 + 1]);
}


//==============================================================================
//
//	メイン
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief	ソーナンス関連初期化
 *
 * @param	sns		ソーナンスワークへのポインタ
 */
//--------------------------------------------------------------
SONANS_SYS_PTR Sonas_Init(BALLOON_GAME_PTR game)
{
	SONANS_SYS_PTR sns;
	
	sns = sys_AllocMemory(HEAPID_BALLOON, sizeof(SONANS_SYS));
	MI_CpuClear8(sns, sizeof(SONANS_SYS));
	sns->rare_type = game->bsw->raregame_type;
	
	//テクスチャロード
	Sonas_TexLoad(sns);
	
	//頂点座標リストの初期値設定
	Sonans_VertexListInit(sns);
	Sonans_TexCoordListInit(sns);
	
	switch(game->bsw->raregame_type){
	case MNGM_RAREGAME_BALLOON_FINE:
		sns->scale_x = SONANS_SCALE_SIZE_FINE_X;		//レアかどうかでデフォルトを変える
		sns->scale_y = SONANS_SCALE_SIZE_FINE_Y;
		break;
	case MNGM_RAREGAME_BALLOON_THICK:
		sns->scale_x = SONANS_SCALE_SIZE_THICK_X;		//レアかどうかでデフォルトを変える
		sns->scale_y = SONANS_SCALE_SIZE_THICK_Y;
		break;
	default:
		sns->scale_x = SONANS_SCALE_SIZE_X;		//レアかどうかでデフォルトを変える
		sns->scale_y = SONANS_SCALE_SIZE_Y;
		break;
	}
	
	sns->aim_x = SONANS_AIM_INIT_X;
	sns->aim_y = SONANS_AIM_INIT_Y;

	Aim_ResourceLoad(game);
	sns->aim_cap = Aim_ActorCreate(game);
	Aim_Update(sns, sns->aim_cap);
	
	sns->return_power = SONANS_RETURN_POWER_INIT;
	
	return sns;
}

//--------------------------------------------------------------
/**
 * @brief	ソーナンス関連の終了
 *
 * @param	sns		ソーナンスワークへのポインタ
 */
//--------------------------------------------------------------
void Sonans_Exit(BALLOON_GAME_PTR game, SONANS_SYS_PTR sns)
{
	Aim_ActorDelete(sns->aim_cap);
	Aim_ResourceFree(game);

	sys_FreeMemoryEz(sns->tex_file);
	sys_FreeMemoryEz(sns->palette_work_src);
	sys_FreeMemoryEz(sns->palette_work_dest);
	
	sys_FreeMemoryEz(sns);
}

//--------------------------------------------------------------
/**
 * @brief	ソーナンス動作更新処理
 *
 * @param	sns		ソーナンスワークへのポインタ
 */
//--------------------------------------------------------------
void Sonans_Update(BALLOON_GAME_PTR game, SONANS_SYS_PTR sns)
{
	sns->pomp_area = 0;
	
	if(game->countdown_eff == BALLOON_COUNTDOWN_TOUCHPEN_DEMO){
		sns->tp_x = game->pen.tp_x;
		sns->tp_y = game->pen.tp_y;
		sns->tp_cont = game->pen.tp_cont;
		sns->tp_trg = game->pen.tp_trg;
	}
	else if(game->game_start == FALSE || game->game_finish == TRUE){
		sns->tp_x = 0;
		sns->tp_y = 0;
		sns->tp_cont = 0;
		sns->tp_trg = 0;
	}
	else{
		sns->tp_x = sys.tp_x;
		sns->tp_y = sys.tp_y;
		sns->tp_cont = sys.tp_cont;
		sns->tp_trg = sys.tp_trg;
	}
	
#ifdef PM_DEBUG
	if(sys.trg & PAD_BUTTON_L){
//		sns->debug_aim_on_off ^= 1;
	}
#endif
	
	Sonans_AimHitCheck(sns);
	if(sns->tp_cont == 0){
		Sonans_PushModeReset(sns);
		
		//ソーナンスが完全に回復したら押し込んだSEを止める
		if(sns->vertex_list[0].ru.y == SONANS_VERTEX_TOP 
				&& sns->vertex_list[0].ru.y == SONANS_VERTEX_TOP){
			sns->snd_push_se = 0;
			//Snd_SeStopBySeqNo( SE_SONANS_PUSH, 0 );
		}
	}
	Sonans_VertexUpdate(sns);
	
	Sonans_PolygonCreate(sns);
	
	
	Aim_Update(sns, sns->aim_cap);
	
	//怒りMAX状態による拡大
	if(sns->scale_voltage_x > 0){
		sns->scale_voltage_x += SONANS_VOLTAGE_SCALE_ADD_X;
		if(sns->scale_voltage_x > SONANS_VOLTAGE_SCALE_X_MAX){
			sns->scale_voltage_x = SONANS_VOLTAGE_SCALE_X_MAX;
		}
		Sonans_VoltageMaxAnime(sns);
	}
	
	//エアー放出
	if(game->game_finish == FALSE && game->game_start == TRUE){
		if(sns->pomp_area > 0){
			BALLOON_AIR_DATA air_data;
			int voltage_max = FALSE, voltage_reset = FALSE;
			s32 air;
			
			//ボルテージ
			if(sns->voltage < SONANS_VOLTAGE_MAX){
				voltage_max = Sonans_VoltageAdd(sns, sns->pomp_area);
				if(voltage_max == TRUE){
					sns->scale_voltage_x = SONANS_VOLTAGE_SCALE_ADD_X;
					sns->voltage_anm_wait = 0;
					Snd_SePlay(SE_SONANS_BIG);
				}
			}
			else{
				Sonans_VoltageReset(sns);
				sns->scale_voltage_x = 0;
				voltage_reset = TRUE;
			}

			air = Sonans_PompArea_to_AirPower(sns, sns->pomp_area);
			if(voltage_reset == TRUE){
				air += air * VOLTAGE_MAX_REVISE / 100;
			}
			BalloonTool_AirDataCreate(game, game->balloon_no, air, &air_data);
			BalloonTool_SendAirPush(game, &air_data);
			BalloonTool_PlayerAirParamAdd(game, &air_data);
			
			//GREAT or GOOD状態なら戻る速度アップ
			Sonans_ReturnPowerCalc(sns, sns->pomp_area);
			Sonans_ParticleSet(game, sns, sns->pomp_area, voltage_reset);
			
			sns->pomp_area = 0;
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   ソーナンス：Vブランク更新処理
 *
 * @param   game		
 * @param   sns			
 */
//--------------------------------------------------------------
void Sonans_VBlank(BALLOON_GAME_PTR game, SONANS_SYS_PTR sns)
{
	if(sns->color_trans_req == TRUE){
		sns->color_trans_req = 0;
		Sonans_PaletteTrans(sns);
	}
}

//--------------------------------------------------------------
/**
 * @brief   押し込んだ面積を空気力に変換
 *
 * @param   sns				ソーナンスワークへのポインタ
 * @param   pomp_area		押し込んだ面積(100% MAX)
 *
 * @retval  空気力
 */
//--------------------------------------------------------------
static int Sonans_PompArea_to_AirPower(SONANS_SYS_PTR sns, int pomp_area)
{
	int air;
	
	if(pomp_area >= SONANS_POMP_AREA_MAX_LINE){
		air = SONANS_POMP_MAX_AIR;
	}
	else{
		air = pomp_area * 10;	//1% = 10cc
	}
	
	//レアゲーム補正
	air = air * AirRareRevision[sns->rare_type] / 100;
	return air;
}

//--------------------------------------------------------------
/**
 * @brief   ソーナンスの戻る力を、押し込んだ面積によって更新する
 *
 * @param   sns		
 * @param   pomp_area		押し込んだ面積(100%MAX)
 */
//--------------------------------------------------------------
static void Sonans_ReturnPowerCalc(SONANS_SYS_PTR sns, int pomp_area)
{
	if(sns->pomp_area >= SONANS_POMP_AREA_GREAT_LINE){
		sns->return_power += SONANS_RETURN_POWER_ADD_GREAT;
	}
	else if(sns->pomp_area >= SONANS_POMP_AREA_GOOD_LINE){
		sns->return_power += SONANS_RETURN_POWER_ADD_GOOD;
	}
	else{	//失敗なら初期値に戻す
		sns->return_power = SONANS_RETURN_POWER_INIT;
	}
	
	if(sns->return_power > SONANS_RETURN_POWER_MAX){
		sns->return_power = SONANS_RETURN_POWER_MAX;
	}
}

//--------------------------------------------------------------
/**
 * @brief   ソーナンスを押し込んだ時のパーティクルをセットする
 *
 * @param   game
 * @param   pomp_area		押し込んだ面積(100%MAX)
 * @param   voltage_max		TRUE:ボルテージが最大の時に押し込んだ
 */
//--------------------------------------------------------------
static void Sonans_ParticleSet(BALLOON_GAME_PTR game, SONANS_SYS_PTR sns, int pomp_area, int voltage_max)
{
	if(voltage_max == TRUE && sns->pomp_area >= SONANS_POMP_AREA_GREAT_LINE){
		BalloonParticle_EmitAdd(game, BALLOON_HINOMARU);
		Snd_SePlay(SE_GREAT);
	}
	else if(sns->pomp_area >= SONANS_POMP_AREA_GOOD_LINE){
		;
	}
	else{	//失敗
		if(sns->vertex_list[0].lu.y < sns->vertex_list[0].ru.y){
			BalloonParticle_EmitAdd(game, BALLOON_PUSYU2);
			Snd_SePlay(SE_SONANS_AIR);
		}
		else if(sns->vertex_list[0].lu.y > sns->vertex_list[0].ru.y){
			BalloonParticle_EmitAdd(game, BALLOON_PUSYU);
			Snd_SePlay(SE_SONANS_AIR);
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   Push状態をリセットする
 *
 * @param   sns		ソーナンスワークへのポインタ
 */
//--------------------------------------------------------------
static void Sonans_PushModeReset(SONANS_SYS_PTR sns)
{
	if(sns->push_mode == FALSE){
		return;
	}
	sns->push_mode = FALSE;
	sns->backup_tp_y = -1;
	
	//タッチを離した後の面積
	{
		int after_area;
	
		after_area = Sonans_NowAreaParcentGet(sns);
		sns->pomp_area = sns->before_area - after_area;
	#ifdef OSP_BALLOON_ON
		OS_TPrintf("稼いだ面積 = %d, before = %d, after = %d\n", sns->pomp_area, sns->before_area, after_area);
	#endif
		if(sns->pomp_area < SONANS_POMP_AREA_IGNORE_LINE){
			sns->pomp_area = 0;
		#ifdef OSP_BALLOON_ON
			OS_TPrintf("少ない空気なので無視\n");
		#endif
		}
	}
	
//	sns->snd_push_se = 0;
//	Snd_SeStopBySeqNo( SE_SONANS_PUSH, 0 );
}

//--------------------------------------------------------------
/**
 * @brief   ソーナンスの頂点リストを作成し、ポリゴン描画
 *
 * @param   sns		ソーナンスワークへのポインタ
 *
 *　毎フレーム呼ぶ必要があります。
 */
//--------------------------------------------------------------
static void Sonans_PolygonCreate(SONANS_SYS_PTR sns)
{
	u32 tex_address, pltt_address;

//	GF_G3X_Reset();
	
#if 0
	if(sys.cont & PAD_BUTTON_Y){
		sns->rotate = 0;
	}
	if(sys.cont & PAD_BUTTON_X){
		sns->rotate += 256;
	}
#endif

#if 0
	//---------------------------------------------------------------------------
	// Set up a camera matrix
	//---------------------------------------------------------------------------
	{
		VecFx32 Eye = { 0, 0, FX32_ONE };	// Eye position
		VecFx32 at = { 0, 0, 0 };  // Viewpoint
		VecFx32 vUp = { 0, FX32_ONE, 0 };	// Up

		G3_LookAt(&Eye, &vUp, &at, NULL);
	}
#endif

	//---------------------------------------------------------------------------
	// Set up light colors and direction.
	// Notice that light vector is transformed by the current vector matrix
	// immediately after LightVector command is issued.
	//
	// GX_LIGHTID_0: white, downward
	//---------------------------------------------------------------------------
	NNS_G3dGeLightVector(GX_LIGHTID_0, FX16_SQRT1_3, -FX16_SQRT1_3, -FX16_SQRT1_3);
	NNS_G3dGeLightColor(GX_LIGHTID_0, GX_RGB(31, 31, 31));

	NNS_G3dGePushMtx();

	{
		fx16	s = FX_SinIdx(sns->rotate);
		fx16	c = FX_CosIdx(sns->rotate);

		NNS_G3dGeTranslate(0, (SONANS_TEX_SPACE_SIZE + DAIZA_UNIT_SIZE) * FX32_ONE, -5 * FX32_ONE);

		//NNS_G3dGeMultMtx33
		
//		G3_RotX(s, c);
//		G3_RotY(s, c);
//		G3_RotZ(s, c);

		NNS_G3dGeScale(sns->scale_x 
			+ sns->scale_x * (sns->scale_voltage_x + sns->rare_scale_x) / 100,
			sns->scale_y + sns->scale_y * sns->scale_voltage_y / 100, SONANS_SCALE_SIZE_Z);
	}

	{
		NNS_G3dGeMtxMode(GX_MTXMODE_TEXTURE);
		NNS_G3dGeIdentity();
		// Use an identity matrix for the texture matrix for simplicity
		NNS_G3dGeMtxMode(GX_MTXMODE_POSITION_VECTOR);
	}

	// Set the material color( diffuse, ambient , specular ) as basic white 
#ifdef DEBUG_MAT
	DEMO_Set3DDefaultMaterial(TRUE, TRUE);
	DEMO_Set3DDefaultShininessTable();
#endif

	tex_address = NNS_GfdGetTexKeyAddr(sns->texture->texInfo.vramKey);
	pltt_address = NNS_GfdGetPlttKeyAddr(sns->texture->plttInfo.vramKey);

	NNS_G3dGeTexImageParam(GX_TEXFMT_PLTT256,		// use 16 colors palette texture
					 GX_TEXGEN_TEXCOORD,	// use texcoord
					 GX_TEXSIZE_S128,		// 64 pixels
					 GX_TEXSIZE_T128,		// 64 pixels
					 GX_TEXREPEAT_NONE, 	// no repeat
					 GX_TEXFLIP_NONE,		// no flip
					 GX_TEXPLTTCOLOR0_TRNS,//USE	// use color 0 of the palette
					 tex_address	   // the offset of the texture image
		);

	NNS_G3dGeTexPlttBase(pltt_address,  // the offset of the texture palette
				   GX_TEXFMT_PLTT256 // 16 colors palette texture
		);

	NNS_G3dGePolygonAttr(
			GX_LIGHTMASK_0,				// ライトを反映しない
			GX_POLYGONMODE_MODULATE,	// モジュレーションモード
			GX_CULL_NONE,//GX_CULL_BACK,				// カルバックを行う
			0,							// ポリゴンＩＤ
			31,							// アルファ値
			GX_POLYGON_ATTR_MISC_NONE
			);

	Sonans_VertexListUpdate(sns);

	NNS_G3dGePopMtx(1);

	NNS_G3dGeFlushBuffer();

	// swapping the polygon list RAM, the vertex RAM, etc.
//	G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
}


//==============================================================================
//
//	ローカルツール
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief	ソーナンスのテクスチャーロード
 *
 * @param	sns		ソーナンスワークへのポインタ
 */
//--------------------------------------------------------------
static void Sonas_TexLoad(SONANS_SYS *sns)
{
	//テクスチャファイル読み込み
	sns->tex_file = ArchiveDataLoadMalloc(
		ARC_BALLOON_GRA, SONANS_NSBTX, HEAPID_BALLOON);
	sns->texture = NNS_G3dGetTex(sns->tex_file);	// テクスチャリソース取得
	
	//VRAMへ展開
	LoadVRAMTexture(sns->texture);

	//パレットをワークへコピーする(ソーナンスを赤くしていくため)
	{
		u32 sz;
		void* pData;
		u32 from;

		sz = (u32)sns->texture->plttInfo.sizePltt << 3;
		pData = (u8*)sns->texture + sns->texture->plttInfo.ofsPlttData;
		from = (sns->texture->plttInfo.vramKey & 0xffff) << 3;
		
		GF_ASSERT(sns->palette_work_src == NULL);
		sns->palette_work_src = sys_AllocMemory(HEAPID_BALLOON, sz);
		sns->palette_work_dest = sys_AllocMemory(HEAPID_BALLOON, sz);
		MI_CpuCopy8(pData, sns->palette_work_src, sz);
		MI_CpuCopy8(pData, sns->palette_work_dest, sz);
		DC_FlushRange(sns->palette_work_dest, sz);
		OS_TPrintf("ソーナンスのパレットサイズ＝%d\n", sz);
		sns->palette_trans_adrs = from;
		sns->palette_size = sz;
	}
	
	//テクスチャイメージをVRAMへ展開し終わったので、実体を破棄
	{
		u8* texImgStartAddr;
		u32 newSize;

		GF_ASSERT(sns->texture->texInfo.ofsTex != 0);
		texImgStartAddr = (u8*)sns->texture + sns->texture->texInfo.ofsTex;
		// ヒープの先頭からテクスチャイメージまでのサイズ
		newSize = (u32)(texImgStartAddr - (u8*)sns->tex_file);
		sys_CutMemoryBlockSize( sns->tex_file, newSize );
	}
}

//--------------------------------------------------------------
/**
 * @brief   ソーナンスのパレット転送
 *
 * @param   sns		
 */
//--------------------------------------------------------------
static void Sonans_PaletteTrans(SONANS_SYS_PTR sns)
{
    GX_BeginLoadTexPltt();
	
	GX_LoadTexPltt(sns->palette_work_dest, sns->palette_trans_adrs, sns->palette_size);
	
    GX_EndLoadTexPltt();
}

//--------------------------------------------------------------
/**
 * @brief   ソーナンスのパレット更新処理
 *
 * @param   sns		
 */
//--------------------------------------------------------------
static void Sonans_VoltagePaletteUpdate(SONANS_SYS_PTR sns, int voltage)
{
	int evy;
	
	evy = (SONANS_VOLTAGE_EVY_MAX << 8) * voltage / SONANS_VOLTAGE_MAX;
	evy = (evy + 0x80) >> 8;	//四捨五入
	if(evy > 16){	//ありえないけど念のため
		evy = 16;
	}
#ifdef OSP_BALLOON_ON
	OS_TPrintf("sonans evy = %d\n", evy);
#endif
	SoftFade(sns->palette_work_src, sns->palette_work_dest, 
		sns->palette_size / 2, evy, SONANS_VOLTAGE_COLOR);
	
	DC_FlushRange(sns->palette_work_dest, sns->palette_size);
//	Sonans_PaletteTrans(sns);	//ちらつくようならVブランク転送に変える
	sns->color_trans_req = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   ソーナンスのボルテージを加算する
 *
 * @param   sns		
 * @param   pomp_area		押し込んだ面積(100%MAX)
 *
 * @retval  TRUE:ボルテージMAX
 */
//--------------------------------------------------------------
static BOOL Sonans_VoltageAdd(SONANS_SYS_PTR sns, int pomp_area)
{
	BOOL ret = FALSE;
	
	sns->voltage += pomp_area;
	if(sns->voltage >= SONANS_VOLTAGE_MAX){
		sns->voltage = SONANS_VOLTAGE_MAX;
		ret = TRUE;
	}

	//ボルテージによってカラーの加減算をおこなう
	Sonans_VoltagePaletteUpdate(sns, sns->voltage);

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   ボルテージをリセットする
 * @param   sns		
 */
//--------------------------------------------------------------
static void Sonans_VoltageReset(SONANS_SYS_PTR sns)
{
	sns->voltage = 0;
	Sonans_VoltagePaletteUpdate(sns, sns->voltage);
}

//--------------------------------------------------------------
/**
 * @brief   ボルテージが最大の時の点滅アニメ
 *
 * @param   sns		
 */
//--------------------------------------------------------------
static void Sonans_VoltageMaxAnime(SONANS_SYS_PTR sns)
{
	if(sns->scale_voltage_x == 0){
		return;
	}
	
	sns->voltage_anm_wait++;
	if(sns->voltage_anm_wait == VOLTAGE_ANM_FLASH_WAIT){
		Sonans_VoltagePaletteUpdate(sns, sns->voltage/2);
	}
	else if(sns->voltage_anm_wait == VOLTAGE_ANM_FLASH_WAIT * 2){
		Sonans_VoltagePaletteUpdate(sns, sns->voltage);
		sns->voltage_anm_wait = 0;
	}
}


//==============================================================================
//==============================================================================
#ifdef DEBUG_MAT

#define DEMO_INTENSITY_DF   23
#define DEMO_INTENSITY_AM   8
#define DEMO_INTENSITY_SP   31

static const GXRgb DEMO_DIFFUSE_COL =
GX_RGB(DEMO_INTENSITY_DF, DEMO_INTENSITY_DF, DEMO_INTENSITY_DF);
static const GXRgb DEMO_AMBIENT_COL =
GX_RGB(DEMO_INTENSITY_AM, DEMO_INTENSITY_AM, DEMO_INTENSITY_AM);
static const GXRgb DEMO_SPECULAR_COL =
GX_RGB(DEMO_INTENSITY_SP, DEMO_INTENSITY_SP, DEMO_INTENSITY_SP);
static const GXRgb DEMO_EMISSION_COL = GX_RGB(0, 0, 0);

static const u32 LIGHTING_L_DOT_S_SHIFT = 8;


void DEMO_Set3DDefaultMaterial(BOOL bUsediffuseAsVtxCol, BOOL bUseShininessTbl)
{
    NNS_G3dGeMaterialColorDiffAmb(DEMO_DIFFUSE_COL,   // diffuse
                            DEMO_AMBIENT_COL,   // ambient
                            bUsediffuseAsVtxCol // use diffuse as vtx color if TRUE
        );

    NNS_G3dGeMaterialColorSpecEmi(DEMO_SPECULAR_COL,  // specular
                            DEMO_EMISSION_COL,  // emission
                            bUseShininessTbl    // use shininess table if TRUE
        );
}


#include <nitro/code32.h>              // avoid byte access problems
void DEMO_Set3DDefaultShininessTable()
{
    u8      i;
    u32     shininess_table[32];
    u8     *pShininess = (u8 *)&shininess_table[0];
    const u8 tableLength = 32 * sizeof(u32);

    for (i = 0; i < tableLength - 1; i++)
    {
        // ShininessTable is ( 0.8 ) fixed point foramt, so we have to right-shift 8 bit for One multiply.
        // pShininess = (i*2+1)^4
        pShininess[i] = (u8)(((s64)(i * 2 + 1) * (i * 2 + 1) * (i * 2 + 1) * (i * 2 + 1)) >> (LIGHTING_L_DOT_S_SHIFT * (4 - 1)));       // byte access
    }

    // set the max value specialy   
    pShininess[tableLength - 1] = 0xFF;

    NNS_G3dGeShininess(&shininess_table[0]);
}

#include <nitro/codereset.h>


#endif






//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   ソーナンスの頂点座標リストを初期値に設定する
 *
 * @param   sns		
 */
//--------------------------------------------------------------
static void Sonans_VertexListInit(SONANS_SYS_PTR sns)
{
	int y;
	fx32 set_y;
	fx32 add_y;
	
	add_y = (SONANS_VERTEX_BOTTOM - SONANS_VERTEX_TOP) / SONANS_QUAD_STRIP_NUM;
//	OS_TPrintf("vertex add_y = %x\n", add_y);
	
	for(y = 0; y < SONANS_QUAD_STRIP_NUM; y++){
		set_y = SONANS_VERTEX_TOP + y * add_y;
		
		sns->vertex_list[y].lu.x = SONANS_VERTEX_LEFT;
		sns->vertex_list[y].lu.y = set_y;
		sns->vertex_list[y].ld.x = SONANS_VERTEX_LEFT;
		sns->vertex_list[y].ld.y = set_y + add_y;
		sns->vertex_list[y].ru.x = SONANS_VERTEX_RIGHT;
		sns->vertex_list[y].ru.y = set_y;
		sns->vertex_list[y].rd.x = SONANS_VERTEX_RIGHT;
		sns->vertex_list[y].rd.y = set_y + add_y;
	}
}

//--------------------------------------------------------------
/**
 * @brief   ソーナンスのテクスチャ座標リストを初期値に設定する
 *
 * @param   sns		
 */
//--------------------------------------------------------------
static void Sonans_TexCoordListInit(SONANS_SYS_PTR sns)
{
	int y;
	fx32 set_y;
	fx32 add_y;
	
	add_y = SONANS_TEX_SIZE_Y * FX32_ONE / SONANS_QUAD_STRIP_NUM;

	for(y = 0; y < SONANS_QUAD_STRIP_NUM; y++){
		set_y = y * add_y;
		
		sns->tex_coord_list[y].lu.x = 0;
		sns->tex_coord_list[y].lu.y = set_y;
		sns->tex_coord_list[y].ld.x = 0;
		sns->tex_coord_list[y].ld.y = set_y + add_y;
		sns->tex_coord_list[y].ru.x = SONANS_TEX_SIZE_X * FX32_ONE;
		sns->tex_coord_list[y].ru.y = set_y;
		sns->tex_coord_list[y].rd.x = SONANS_TEX_SIZE_X * FX32_ONE;
		sns->tex_coord_list[y].rd.y = set_y + add_y;
	}
}

//--------------------------------------------------------------
/**
 * @brief   頂点座標設定
 *
 * @param   sns		
 */
//--------------------------------------------------------------
#define SONANS_VERTEX_BASE		(FX16_ONE)
#define SONANS_VERTEX_BASE_HALF	(SONANS_VERTEX_BASE / 2)
static void Sonans_VertexListUpdate(SONANS_SYS_PTR sns)
{
	int y;
	fx64 calc_x, calc_y;
	fx16 set_x, set_y;	//vertexは頂点の傾きが計算しやすいようにfx32でドット座標で持っているので
						//頂点リストをセットする前にSONANS_VERTEX_BASEをMAXにした値に変換してやる
	
//	OS_TPrintf("頂点描画開始\n");
	for(y = 0; y < SONANS_QUAD_STRIP_NUM; y++){
		// 描画開始
//		NNS_G3dGeBegin(GX_BEGIN_QUAD_STRIP);// 頂点リストの開始(連結四角形ポリゴンでの描画宣言)
		NNS_G3dGeBegin(GX_BEGIN_QUADS);// 頂点リストの開始(連結四角形ポリゴンでの描画宣言)
		
		//左上
		Sonans_Vertex_FX32_to_Conv(
			sns->vertex_list[y].lu.x, sns->vertex_list[y].lu.y, &set_x, &set_y);
		NNS_G3dGeTexCoord(sns->tex_coord_list[y].lu.x, sns->tex_coord_list[y].lu.y);
		normal(0);
		NNS_G3dGeVtx(set_x, set_y, 0);

		//左下
		Sonans_Vertex_FX32_to_Conv(
			sns->vertex_list[y].ld.x, sns->vertex_list[y].ld.y, &set_x, &set_y);
		NNS_G3dGeTexCoord(sns->tex_coord_list[y].ld.x, sns->tex_coord_list[y].ld.y);
		normal(0);
		NNS_G3dGeVtx(set_x, set_y, 0);

		//右下
		Sonans_Vertex_FX32_to_Conv(
			sns->vertex_list[y].rd.x, sns->vertex_list[y].rd.y, &set_x, &set_y);
		NNS_G3dGeTexCoord(sns->tex_coord_list[y].rd.x, sns->tex_coord_list[y].rd.y);
		normal(0);
		NNS_G3dGeVtx(set_x, set_y, 0);

		//右上
		Sonans_Vertex_FX32_to_Conv(
			sns->vertex_list[y].ru.x, sns->vertex_list[y].ru.y, &set_x, &set_y);
		NNS_G3dGeTexCoord(sns->tex_coord_list[y].ru.x, sns->tex_coord_list[y].ru.y);
		normal(0);
		NNS_G3dGeVtx(set_x, set_y, 0);

		//描画終了
		NNS_G3dGeEnd();
	}
//	OS_TPrintf("頂点描画終了\n");
}

static void Sonans_Vertex_FX32_to_Conv(fx32 x, fx32 y, fx16 *ret_x, fx16 *ret_y)
{
	fx32 set_x, set_y;
	
	set_x = SONANS_VERTEX_BASE * (fx64)x / SONANS_VERTEX_LEN;
	set_y = SONANS_VERTEX_BASE * (fx64)y / SONANS_VERTEX_LEN;
	GF_ASSERT(set_x <= FX16_MAX);
	GF_ASSERT(set_x >= FX16_MIN);
	GF_ASSERT(set_y <= FX16_MAX);
	GF_ASSERT(set_y >= FX16_MIN);

//	OS_TPrintf("set_x0 = %x, set_y0 = %x, tex_coord_x = %x, tex_coord_y = %x\n", set_x, set_y, sns->tex_coord_list[y].lu.x, sns->tex_coord_list[y].lu.y);

	set_x -= SONANS_VERTEX_BASE_HALF;	//ソーナンスの中心を原点にする為
	set_y += SONANS_VERTEX_BASE_HALF;
	GF_ASSERT(set_x <= FX16_MAX);
	GF_ASSERT(set_x >= FX16_MIN);
	GF_ASSERT(set_y <= FX16_MAX);
	GF_ASSERT(set_y >= FX16_MIN);
	
	*ret_x = set_x;
	*ret_y = set_y;
}

//--------------------------------------------------------------
/**
 * @brief   照準にポイントしたかチェック
 * @param   sns		ソーナンスワークへのポインタ
 * @retval  TRUE:ヒット
 */
//--------------------------------------------------------------
static BOOL Sonans_AimHitCheck(SONANS_SYS_PTR sns)
{
	int aim_y;
	
	if(sns->push_mode == TRUE || 		//既に押されている状態
			sns->tp_trg == 0){		//押したフレームでないと判定しない
		return FALSE;
	}
	
	aim_y = (sns->aim_hit_center_y_fx >> FX32_SHIFT) + SonansHitRect[sns->rare_type].top;
#if 0
	OS_TPrintf("bbb aim_y = %d, aim_hit_center_y_fx = %d\n", aim_y, sns->aim_hit_center_y_fx);
	if(sns->tp_x < sns->aim_x - AIM_RANGE_HALF_X 
			|| sns->tp_x > sns->aim_x + AIM_RANGE_HALF_X
			|| sns->tp_y < aim_y - AIM_RANGE_HALF_Y 
			|| sns->tp_y > aim_y + AIM_RANGE_HALF_Y){
		return FALSE;	//照準の範囲外
	}
	
	sns->push_mode = TRUE;
	sns->aim_hit_x = sns->tp_x;
	sns->aim_hit_y = sns->tp_y;
	sns->aim_hit_y_zure = sns->tp_y - aim_y;	//中心からいくつずれてタッチしたかを保持
	sns->aim_hit_center_y_fx = 
		-(sns->vertex_list[0].ru.y + (sns->vertex_list[0].lu.y - sns->vertex_list[0].ru.y) / 2);
	OS_TPrintf("aim hit : x = %d, y = %d, center_y = %d\n", sns->aim_hit_x, sns->aim_hit_y, sns->aim_hit_center_y_fx);
	
	//タッチ開始時の面積
	sns->before_area = Sonans_NowAreaParcentGet(sns);
	OS_TPrintf("タッチ開始時の面積 = %d\n", sns->before_area);
#else
	if(sns->tp_x < SonansHitRect[sns->rare_type].left
			|| sns->tp_x > SonansHitRect[sns->rare_type].right
			|| sns->tp_y < aim_y	//SonansHitRect[sns->rare_type].top
			|| sns->tp_y > SonansHitRect[sns->rare_type].bottom){
		return FALSE;	//照準の範囲外
	}
	
	sns->push_mode = TRUE;
	sns->aim_hit_x = sns->tp_x;
	sns->aim_hit_y = sns->tp_y;
	sns->aim_hit_y_zure = sns->tp_y - aim_y;	//中心からいくつずれてタッチしたかを保持
	sns->aim_hit_center_y_fx = 
		-(sns->vertex_list[0].ru.y + (sns->vertex_list[0].lu.y - sns->vertex_list[0].ru.y) / 2);
#ifdef OSP_BALLOON_ON
	OS_TPrintf("aim hit : x = %d, y = %d, center_y = %d\n", sns->aim_hit_x, sns->aim_hit_y, sns->aim_hit_center_y_fx);
#endif

	//タッチ開始時の面積
	sns->before_area = Sonans_NowAreaParcentGet(sns);
#ifdef OSP_BALLOON_ON
	OS_TPrintf("タッチ開始時の面積 = %d\n", sns->before_area);
#endif
#endif
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   ソーナンスの頂点座標更新処理
 *
 * @param   sns		ソーナンスワークへのポインタ
 *
 * @retval  
 */
//--------------------------------------------------------------
static int Sonans_VertexUpdate(SONANS_SYS_PTR sns)
{
	int mode = 0;
	fx32 dot_offset;
	VERTEX_CALC_RET ret;
	int tp_offset;
	
	if(sns->push_mode == TRUE){
		if(sns->tp_y < sns->backup_tp_y){
		//	Sonans_PushModeReset(sns);	慎重にタッチしないと画面にあてた時のブレで上行くのでやめた
		//	Snd_SeStopBySeqNo( SE_SONANS_PUSH, 0 );
			sns->snd_push_se = 0;
			return FALSE;	//上方向へペンを動かした
		}
		else if(sns->tp_y == sns->backup_tp_y){
		//	Snd_SeStopBySeqNo( SE_SONANS_PUSH, 0 );
			sns->snd_push_se = 0;
			return TRUE;	//位置が変わっていないので計算処理する必要なし
		}
		
		//if(sns->snd_push_se == 0){
		if(sns->backup_tp_y != -1 && sns->tp_y > sns->backup_tp_y 
				&& Snd_SePlayCheck(SE_SONANS_PUSH) == FALSE){
			Snd_SePlay(SE_SONANS_PUSH);
			sns->snd_push_se = TRUE;
		}

		tp_offset = sns->tp_y - sns->aim_hit_y_zure;
		//OS_TPrintf("before tp_offset = %d, tp_y = %d, zure = %d\n", tp_offset, sns->tp_y, sns->aim_hit_y_zure);
//		tp_offset -= sns->backup_offset_tp_y;
//		OS_TPrintf("after tp_offset = %d, tp_y = %d, zure = %d\n", tp_offset, sns->tp_y, sns->aim_hit_y_zure);
		//dot_offset = tp_y - now_center_y;
		dot_offset = (tp_offset << FX32_SHIFT) 
			- (sns->aim_hit_center_y_fx + (SonansHitRect[sns->rare_type].top << FX32_SHIFT));
//		dot_offset = ((sns->aim_hit_y + tp_offset) << FX32_SHIFT) - sns->aim_hit_center_y_fx;
//		dot_offset = (sns->tp_y - sns->backup_tp_y) << FX32_SHIFT;
		//dot_offset = tp_y - (sns->aim_hit_y << FX32_SHIFT);
		
		//このままだとペンをずらすのよりも早くソーナンスが潰れていくのでいい感じに補正をかける
		//dot_offset = dot_offset * 10 / 100;
		
	#ifdef OSP_BALLOON_ON
		OS_TPrintf("tp_y = %d, tp_offset = %d, dot_offset = %d, aim_hit_y = %d, center_y_fx = %d\n", sns->tp_y, tp_offset, dot_offset, sns->aim_hit_y, sns->aim_hit_center_y_fx);
	#endif
	
		sns->backup_tp_y = sns->tp_y;
	}
	else{	//タッチしていない状態(空気を戻す)
		mode = TRUE;
		dot_offset = -sns->return_power;
	}
	
	ret = Sonans_VertexCalc(sns, mode, dot_offset);
	switch(ret){
	case VERTEX_CALC_RET_TRUE:
	default:
		break;
	case VERTEX_CALC_RET_RELEASE:
	case VERTEX_CALC_RET_CARRY_OVER:
		Sonans_PushModeReset(sns);
		break;
	}
	sns->aim_hit_center_y_fx = -(sns->vertex_list[0].ru.y + (sns->vertex_list[0].lu.y - sns->vertex_list[0].ru.y) / 2);

	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   タッチ位置により頂点座標の再計算
 *
 * @param   sns		ソーナンスワークへのポインタ
 * @param   air_back_mode		TRUE:空気戻りモード、　FALSE:タッチペンで押し込みモード
 * @param   dot_offset			頂点に加算する数値
 *
 * @retval  TRUE:正常終了
 * @retval  FALSE:座標の上限、下限補正が発生した。(タッチした状態ならタッチ解除させる)
 */
//--------------------------------------------------------------
static VERTEX_CALC_RET Sonans_VertexCalc(SONANS_SYS_PTR sns, int air_back_mode, fx32 dot_offset)
{
	fx32 tp_y, left_y, right_y, now_left_y, now_right_y, new_left_y, new_right_y;
	fx32 now_center_y, new_center_y;
	fx32 rad_x, rad_y;
	u16 right_rad, left_rad;
	int carry_over = FALSE;
	
//	OS_TPrintf("air_back_mode = %d, dot_offset = %d\n", air_back_mode, dot_offset);
	if(air_back_mode == FALSE && dot_offset < 0){
//		return VERTEX_CALC_RET_RELEASE;	//上方向へペンを動かしているので終了
	}

	tp_y = sns->tp_y << FX32_SHIFT;				//固定少数化
//	left_y = sns->hit_vertex_lu_y;		//タッチ判定開始時の左上頂点Y位置
//	right_y = sns->hit_vertex_ru_y;		//タッチ判定開始時の右上頂点Y位置
	now_left_y = -sns->vertex_list[0].lu.y;
	now_right_y = -sns->vertex_list[0].ru.y;
	now_center_y = now_right_y + (now_left_y - now_right_y) / 2;
	
	//OS_TPrintf("now_left_y = %d, now_right_y = %d, now_center_y = %d\n", now_left_y, now_right_y, now_center_y);
	//OS_TPrintf("dot_offset = %d, aim_hit_y = %d, aim_hit_center_y = %d\n", dot_offset, sns->aim_hit_y, sns->aim_hit_center_y_fx);
	
	switch(Sonans_AimHitPosGet(sns, air_back_mode)){
	case AIM_HIT_CENTER:
		new_left_y = now_left_y + dot_offset;
		new_right_y = now_right_y + dot_offset;
		break;
	case AIM_HIT_LEFT:
		carry_over = VertexCalc_TouchPower(sns, dot_offset, 
			now_left_y, now_right_y, &new_left_y, &new_right_y);
		break;
	case AIM_HIT_RIGHT:
		carry_over = VertexCalc_TouchPower(sns, dot_offset, 
			now_right_y, now_left_y, &new_right_y, &new_left_y);
		break;
	}
	//OS_TPrintf("aaa tp_y = %d, now_left_y = %d, new_left_y = %d, now_right_y = %d, new_right_y = %d\n", tp_y, now_left_y, new_left_y, now_right_y, new_right_y);
	new_center_y = new_right_y + (new_left_y - new_right_y) / 2;
	//OS_TPrintf("aaa now_center_y = %d, new_center_y = %d\n", now_center_y, new_center_y);
	
	if(now_center_y == new_center_y && now_left_y == new_left_y && now_right_y == new_right_y){
	#ifdef OSP_BALLOON_ON
		OS_TPrintf("計算CANCEL\n");
	#endif
		return VERTEX_CALC_RET_TRUE;	//何も値が変わっていないのでここで終了
	}
	
	//ソーナンスの上限、下限チェック
	if(new_center_y < 0){
		new_center_y = 0;
//		carry_over = TRUE;	上限にキャリーオーバーRETURNはいらない
		//OS_TPrintf("上限補正\n");
	}
	if(new_left_y < 0){
		new_left_y = 0;
//		carry_over = TRUE;
	}
	if(new_right_y < 0){
		new_right_y = 0;
//		carry_over = TRUE;
	}
	
	if(new_center_y > SONANS_VERTEX_PUSH_LEN){
		new_center_y = SONANS_VERTEX_PUSH_LEN;
		carry_over = TRUE;
		//OS_TPrintf("下限補正\n");
	}
	if(new_left_y > SONANS_VERTEX_PUSH_LEN){
		new_left_y = SONANS_VERTEX_PUSH_LEN;
		carry_over = TRUE;
	}
	if(new_right_y > SONANS_VERTEX_PUSH_LEN){
		new_right_y = SONANS_VERTEX_PUSH_LEN;
		carry_over = TRUE;
	}
	
//	sns->aim_hit_center_y_fx = new_center_y;	//照準位置更新
	
	//角度
	{
		fx32 rad_x, rad_y;
		int point_x;
		
	#if 0	//aim_hit_xでやった方がXの位置で角度が変わるのでそれっぽいんだけど
			//調整が大変な為、固定値にする
		point_x = sns->aim_hit_x - 128;
	#else
		point_x = 64;	//X角度固定値(この数値を小さくするほどソーナンスの倒れる角度が急になる)
	#endif
	
		switch(Sonans_AimHitPosGet(sns, air_back_mode)){
		case AIM_HIT_CENTER:
		default:
			if(new_left_y < new_right_y){
				rad_x = point_x * FX32_ONE;
			}
			else if(new_left_y > new_right_y){
				rad_x = -point_x * FX32_ONE;
			}
			else{
				rad_x = 0;
			}
			rad_x = point_x * FX32_ONE;
			rad_y = new_right_y - new_center_y;
			right_rad = FX_Atan2Idx(rad_y, rad_x);
			left_rad = right_rad + 0x8000;	//180度反転 (u16でオーバーフローを起こす事前提)
			break;
		case AIM_HIT_RIGHT:
			rad_x = point_x * FX32_ONE;//sns->aim_hit_x * FX32_ONE;
			rad_y = (new_right_y - new_center_y);
			right_rad = FX_Atan2Idx(rad_y, rad_x);
			left_rad = right_rad + 0x8000;	//180度反転 (u16でオーバーフローを起こす事前提)
			break;
		case AIM_HIT_LEFT:
			rad_x = -point_x * FX32_ONE;//sns->aim_hit_x * FX32_ONE;
			rad_y = new_left_y - new_center_y;
			left_rad = FX_Atan2Idx(rad_y, rad_x);
			right_rad = left_rad + 0x8000;	//180度反転 (u16でオーバーフローを起こす事前提)
			break;
		}
		//OS_TPrintf("aim_hit_x = %x, dot_offset = %x\n", sns->aim_hit_x, dot_offset);
		//OS_TPrintf("rad_x = %x, rad_y = %x, left_rad = %x, right_rad = %x, new_right_y = %x, new_center_y = %x, new_left_y = %x\n", rad_x, rad_y, left_rad, right_rad, new_right_y, new_center_y, new_left_y);

		//テスト
//		OS_TPrintf("rad 45,45 = %x\n", FX_Atan2Idx(45*FX32_ONE,45*FX32_ONE));
//		OS_TPrintf("rad -45,-45 = %x\n", FX_Atan2Idx(-45*FX32_ONE,-45*FX32_ONE));
//		OS_TPrintf("rad -45,45 = %x\n", FX_Atan2Idx(-45*FX32_ONE,45*FX32_ONE));
//		OS_TPrintf("rad 45,-45 = %x\n", FX_Atan2Idx(45*FX32_ONE,-45*FX32_ONE));
//		OS_TPrintf("rad 0,45 = %x\n", FX_Atan2Idx(0*FX32_ONE,45*FX32_ONE));
//		OS_TPrintf("rad 0,-45 = %x\n", FX_Atan2Idx(0*FX32_ONE,-45*FX32_ONE));
//		OS_TPrintf("rad 45,0 = %x\n", FX_Atan2Idx(45*FX32_ONE,0*FX32_ONE));
//		OS_TPrintf("rad -45,0 = %x\n", FX_Atan2Idx(-45*FX32_ONE,0*FX32_ONE));
	}
	
	//頂点座標更新
	{
		fx32 furihaba = SONANS_VERTEX_LEN_HALF / FX32_ONE;

		sns->vertex_list[0].lu.x = furihaba * FX_CosIdx(left_rad) + SONANS_VERTEX_LEN_HALF;
		sns->vertex_list[0].lu.y = -(furihaba * FX_SinIdx(left_rad) + new_center_y);
		sns->vertex_list[0].ru.x = furihaba * FX_CosIdx(right_rad) + SONANS_VERTEX_LEN_HALF;
		sns->vertex_list[0].ru.y = -(furihaba * FX_SinIdx(right_rad) + new_center_y);
		
		//OS_TPrintf("furihaba = %x, left_rad = %x, right_rad = %x, new_center_y = %x\n", furihaba, left_rad, right_rad, new_center_y);
		//OS_TPrintf("lu.x = %x, SinIdx = %x, lu.y = %x, CosIdx = %x\n", sns->vertex_list[0].lu.x, FX_SinIdx(left_rad), sns->vertex_list[0].lu.y, FX_CosIdx(left_rad));
		//OS_TPrintf("ru.x = %x, SinIdx = %x, ru.y = %x, CosIdx = %x\n", sns->vertex_list[0].ru.x, FX_SinIdx(right_rad), sns->vertex_list[0].ru.y, FX_CosIdx(right_rad));
		
		//OS_TPrintf("ld.x = %x, ld.y = %x, rd.x = %x, rd.y = %x\n", sns->vertex_list[0].ld.x, sns->vertex_list[0].ld.y, sns->vertex_list[0].rd.x, sns->vertex_list[0].rd.y);
		
		{
			int i;
			fx32 add_y_line;
			s32 add_rad;
			
			add_y_line = (SONANS_VARIABLE_VERTEX_LEN - new_center_y) / SONANS_VARIABLE_VERTEX_NUM;
			if(right_rad > 0x8000){
				add_rad = -(0x10000 - right_rad) / SONANS_VARIABLE_VERTEX_NUM;
			}
			else{
				add_rad = right_rad / SONANS_VARIABLE_VERTEX_NUM;
			}
		
			for(i = 1; i < SONANS_VARIABLE_VERTEX_NUM; i++){
				sns->vertex_list[i].lu.x = furihaba * FX_CosIdx(left_rad - add_rad * i) + SONANS_VERTEX_LEN_HALF;
				sns->vertex_list[i].lu.y = -(furihaba * FX_SinIdx(left_rad - add_rad * i) + new_center_y + add_y_line * i);
				sns->vertex_list[i].ru.x = furihaba * FX_CosIdx(right_rad - add_rad * i) + SONANS_VERTEX_LEN_HALF;
				sns->vertex_list[i].ru.y = -(furihaba * FX_SinIdx(right_rad - add_rad * i) + new_center_y + add_y_line * i);
			
				sns->vertex_list[i-1].ld.x = sns->vertex_list[i].lu.x;
				sns->vertex_list[i-1].ld.y = sns->vertex_list[i].lu.y;
				sns->vertex_list[i-1].rd.x = sns->vertex_list[i].ru.x;
				sns->vertex_list[i-1].rd.y = sns->vertex_list[i].ru.y;
				
				//OS_TPrintf("ver %dlu.x = %x, ver %dlu.y = %x\n", i, sns->vertex_list[i].lu.x, i, sns->vertex_list[i].lu.y);
				//OS_TPrintf("ver %dru.x = %x, ver %dru.y = %x\n", i, sns->vertex_list[i].ru.x, i, sns->vertex_list[i].ru.y);
			}
		}
	}

	if(carry_over == TRUE){
		return VERTEX_CALC_RET_CARRY_OVER;
	}
	return VERTEX_CALC_RET_TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   照準にヒットした時の位置判定を得る
 * @param   sns		ソーナンスワークへのポインタ
 * @retval  位置フラグ(AIM_HIT_???)
 */
//--------------------------------------------------------------
static int Sonans_AimHitPosGet(SONANS_SYS_PTR sns, int air_back_mode)
{
	if(air_back_mode == TRUE){
		return AIM_HIT_CENTER;
	}
	
	if(sns->aim_hit_x < sns->aim_x - AIM_CENTER_RANGE_HALF_X){
	#ifdef OSP_BALLOON_ON
		OS_TPrintf("AIM_HIT_LEFT\n");
	#endif
		return AIM_HIT_LEFT;
	}
	if(sns->aim_hit_x > sns->aim_x + AIM_CENTER_RANGE_HALF_X){
	#ifdef OSP_BALLOON_ON
		OS_TPrintf("AIM_HIT_RIGHT\n");
	#endif
		return AIM_HIT_RIGHT;
	}
#ifdef OSP_BALLOON_ON
	OS_TPrintf("AIM_HIT_CENTER\n");
#endif
	return AIM_HIT_CENTER;
}

//--------------------------------------------------------------
/**
 * @brief   タッチペンの位置、左右の現在頂点から、新しい左右頂点座標を取得する
 *
 * @param   sns				ソーナンスワークへのポインタ
 * @param   tp_y			タッチペンのY値
 * @param   now_y			タッチペンの値を反映する対象の頂点座標(左右どちらか)
 * @param   now_reverse_y	now_yの逆側の頂点座標
 * @param   new_y			now_y側の頂点の新しいY座標
 * @param   new_reverse_y	逆側の頂点の新しいY座標
 *
 * @retval  TRUE:上限、下限補正発生　FALSE:正常終了。
 */
//--------------------------------------------------------------
static int VertexCalc_TouchPower(SONANS_SYS_PTR sns, fx32 dot_offset, 
	fx32 now_y, fx32 now_reverse_y, fx32 *new_y, fx32 *new_reverse_y)
{
	int ret = FALSE;
	
	*new_y = now_y;
	*new_reverse_y = now_reverse_y;
	
	//上限、下限を超えないように補正
	if(now_y + dot_offset > SONANS_VERTEX_PUSH_LEN){
		dot_offset -= (now_y + dot_offset) - SONANS_VERTEX_PUSH_LEN;
	#ifdef OSP_BALLOON_ON
		OS_TPrintf("補正発生\n");
	#endif
		ret = TRUE;
	}
	
	*new_y = now_y + dot_offset;
#ifdef OSP_BALLOON_ON
	OS_TPrintf("ccc now_y = %d, now_reverse_y = %d, new_y = %d, new_reverse_y = %d\n", now_y, now_reverse_y, *new_y, *new_reverse_y);
#endif
	if(now_reverse_y <= now_y){	//変更前の逆側の頂点が同じか、上の位置にあるならば25%上昇
		*new_reverse_y = now_reverse_y - (dot_offset * 25 / 100);
	#ifdef OSP_BALLOON_ON
		OS_TPrintf("25\%UP now_reverse_y = %d, new = %d, dot = %d\n", now_reverse_y, new_reverse_y, dot_offset);
	#endif
	}
	else{	//低い位置にある場合は現状維持
		*new_reverse_y = now_reverse_y;
	#ifdef OSP_BALLOON_ON
		OS_TPrintf("現状維持 now_reverse_y = %d, new = %d, dot = %d\n", now_reverse_y, new_reverse_y, dot_offset);
	#endif
	}
	
	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   ソーナンスの面積を求める
 * @param   sns		ソーナンスワークへのポインタ
 * @retval  面積
 */
//--------------------------------------------------------------
static fx32 Sonans_AreaGet(SONANS_SYS_PTR sns)
{
	fx32 jyoutei, katei, takasa;
	fx32 jyoutei_y, jyoutei_x, katei_x;
	fx32 now_area;
	int max_pos = SONANS_VARIABLE_VERTEX_NUM - 1;
	
	//一番上の頂点と、一番下の頂点の台形の面積で算出する
	
	//台形面積で求められるように上底の高さは左右のYの中心にする。
	//(左右の高さがバラバラだと台形の公式では求められない)
	jyoutei_y = sns->vertex_list[0].ru.y + (sns->vertex_list[0].lu.y - sns->vertex_list[0].ru.y) / 2;
	takasa = -(sns->vertex_list[max_pos].ld.y - jyoutei_y);
	
	jyoutei_x = sns->vertex_list[0].ru.x - sns->vertex_list[0].lu.x;
	katei_x = sns->vertex_list[max_pos].rd.x 
		- sns->vertex_list[max_pos].ld.x;
		
	//now_area = (jyoutei_x + katei_x) * takasa / 2;
	now_area = FX_Mul(jyoutei_x + katei_x, takasa) / 2;
	
#ifdef OSP_BALLOON_ON
	OS_TPrintf("jyoutei_y = %d, takasa = %d, jyoutei_x = %d, katei_x = %d, ver0lu = %d, verMaxlu = %d, now_area = %d\n", jyoutei_y, takasa, jyoutei_x, katei_x, sns->vertex_list[0].lu.y, sns->vertex_list[max_pos].lu.y, now_area);
#endif
	
	return now_area;
}

//--------------------------------------------------------------
/**
 * @brief   現在のソーナンスの面積が最大時に比べて何%の状態か取得する
 *
 * @param   sns		ソーナンスワークへのポインタ
 *
 * @retval  現在の面積パーセンテージ(100％が最大)
 */
//--------------------------------------------------------------
static int Sonans_NowAreaParcentGet(SONANS_SYS_PTR sns)
{
	fx32 now_area;
	int now_parcent;
	
	now_area = Sonans_AreaGet(sns);
	now_parcent = 100 * (fx64)now_area / SONANS_MAX_AREA;
#ifdef OSP_BALLOON_ON
	OS_TPrintf("現在の面積　＝　%d％, now_area = %d, max = %d, 16now = %x, 16max = %x\n", now_parcent, now_area, SONANS_MAX_AREA, now_area, SONANS_MAX_AREA);
#endif

	return now_parcent;
}


//--------------------------------------------------------------
/**
 * @brief   照準リソース登録
 *
 * @param   game		
 */
//--------------------------------------------------------------
static void Aim_ResourceLoad(BALLOON_GAME_PTR game)
{
	ARCHANDLE* hdl;

	//ハンドルオープン
	hdl  = ArchiveDataHandleOpen(ARC_BALLOON_GRA,  HEAPID_BALLOON); 

	CATS_LoadResourcePlttWorkArcH(game->pfd, FADE_MAIN_OBJ, game->csp, game->crp, 
		hdl, AIM_NCLR, 0, 
		1, NNS_G2D_VRAM_TYPE_2DMAIN, PLTTID_OBJ_AIM);

	CATS_LoadResourceCharArcH(game->csp, game->crp, hdl, 
		AIM_NCGR, 0, NNS_G2D_VRAM_TYPE_2DMAIN, CHARID_BALLOON_AIM);
	CATS_LoadResourceCellArcH(game->csp, game->crp, hdl, AIM_NCER, 0, CELLID_BALLOON_AIM);
	CATS_LoadResourceCellAnmArcH(game->csp, game->crp, hdl, AIM_NANR, 
		0, CELLANMID_BALLOON_AIM);

	//ハンドル閉じる
	ArchiveDataHandleClose( hdl );
}

//--------------------------------------------------------------
/**
 * @brief   照準リソース解放
 *
 * @param   game		
 */
//--------------------------------------------------------------
static void Aim_ResourceFree(BALLOON_GAME_PTR game)
{
	CATS_FreeResourceChar(game->crp, CHARID_BALLOON_AIM);
	CATS_FreeResourceCell(game->crp, CELLID_BALLOON_AIM);
	CATS_FreeResourceCellAnm(game->crp, CELLANMID_BALLOON_AIM);
	CATS_FreeResourcePltt(game->crp, PLTTID_OBJ_AIM);
}

//--------------------------------------------------------------
/**
 * @brief   照準アクター生成
 *
 * @param   game		
 *
 * @retval  生成した照準アクターへのポインタ
 */
//--------------------------------------------------------------
static CATS_ACT_PTR Aim_ActorCreate(BALLOON_GAME_PTR game)
{
	CATS_ACT_PTR cap;
	TCATS_OBJECT_ADD_PARAM_S act_head;
	
	//-- アクター生成 --//
	act_head = AimObjParam;
	cap = CATS_ObjectAdd_S(game->csp, game->crp, &act_head);
	CATS_ObjectEnableCap(cap, CATS_ENABLE_FALSE);
	
	CATS_ObjectUpdate(cap->act);
	return cap;
}

//--------------------------------------------------------------
/**
 * @brief   照準アクター更新処理
 *
 * @param   cap		照準アクターへのポインタ
 * @param   x		座標X
 * @param   y		座標Y
 * @param   anmseq	アニメシーケンス番号
 */
//--------------------------------------------------------------
static void Aim_Update(SONANS_SYS_PTR sns, CATS_ACT_PTR cap)
{
	int anmseq;
	
	if(sns->push_mode == FALSE){
		anmseq = 0;
	}
	else{
		anmseq = Sonans_AimHitPosGet(sns, 0) + 1;
	}
	
	CATS_ObjectPosSetCap_SubSurface(cap, sns->aim_x, 
		(sns->aim_hit_center_y_fx >> FX32_SHIFT) + SonansHitRect[sns->rare_type].top, 
		BALLOON_SUB_ACTOR_DISTANCE);
	CATS_ObjectAnimeSeqSetCap(cap, anmseq );
	CATS_ObjectUpdate(cap->act);
#ifdef PM_DEBUG
	CATS_ObjectEnableCap(cap, sns->debug_aim_on_off);
#endif
}

//--------------------------------------------------------------
/**
 * @brief   照準アクター削除
 *
 * @param   cap		照準アクターへのポインタ
 */
//--------------------------------------------------------------
static void Aim_ActorDelete(CATS_ACT_PTR cap)
{
	CATS_ActorPointerDelete_S(cap);
}


//=============================================================================
/**
 * @file	gym.c
 * @bfief	ジム仕掛けとか
 * @author	Nozomu Saito
 *
 */
//=============================================================================
#include "common.h"
#include "fieldsys.h"
#include "fieldmap_work.h"
#include "system/snd_tool.h"
#include "include/savedata/gimmickwork.h"
#include "../fielddata/build_model/build_model_id.h"

#include "system/render_oam.h"
#include "system/clact_util.h"

#include "field_gimmick_def.h"
#include "syswork.h"
#include "sysflag_def.h" 

#include "gym_def.h"

#include "gym_local.h"
#include "gym_init.h"

#include "gym.h"

#include "script.h"
#include "..\fielddata\script\trainer_def.h"	//SCRID_TRAINER_MOVE_BATTLE
#include "ev_trainer.h"
//#include "arc/plgym_ghost.naix"	//未使用

#include "fielddata/maptable/zone_id.h"
#include "eventdata.h"

#include "talk_msg.h"
#include "system/window.h"
#include "fieldmap.h"
#include	"../../include/msgdata/msg_c04gym0101.h"
	
#include "..\fielddata\eventdata\zone_c05gym0101evd.h" //PLゴーストジム

//定義でDP格闘ジムを無効化
#define DP_OLDGYM_NULL_FIGHT

//ジム共通
#define ONE_GRID	(FX32_ONE*16)
#define ONE_HEIGHT	(FX32_ONE*16)

//水ジム-----------------
//水ジムの水位定義
#define WATER_LV1	(FX32_ONE*16*0)		//高さ１段階
#define WATER_LV2	(FX32_ONE*16*2)		//高さ２段階
#define WATER_LV3	(FX32_ONE*16*4)		//高さ３段階
#define MODEL_WATER_LV1	(FX32_ONE*0)	//モデル高さ１段階
#define MODEL_WATER_LV2	(FX32_ONE*16*2)	//モデル高さ２段階
#define MODEL_WATER_LV3	(FX32_ONE*16*4)	//モデル高さ３段階
#define WATER_DIF	(FX32_ONE)		//高さ変動値（モデル高さ用）

#define WTAER_GYM_DATA_HRIGHT_ID	(0)	//拡張高さ参照インデックス（水ジム水面用）

//アニメ制御コード
#define WATER_GYM_BTN_ANM1		(1)
#define WATER_GYM_BTN_ANM2		(2)
#define WATER_GYM_BTN_ANM3		(3)

#define WATER_GYM_EX_HEIGHT_X	(1)
#define WATER_GYM_EX_HEIGHT_Z	(2)
#define WATER_GYM_EX_HEIGHT_W	(25)
#define WATER_GYM_EX_HEIGHT_H	(38)


//ゴーストジム-----------------
//リフト位置定義
#define GHOST_LIFT_LV1	(FX32_ONE*16*0)		//高さ１段階
#define GHOST_LIFT_LV2	(FX32_ONE*16*10)		//高さ２段階
#define MODEL_GHOST_LIFT_LV1	(FX32_ONE*0)	//モデル高さ１段階
#define MODEL_GHOST_LIFT_LV2	(FX32_ONE*16*10)	//モデル高さ２段階
#define GHOST_LIFT_DIF	(FX32_ONE)		//高さ変動値（モデル高さ用）

#define GHOST_GYM_DATA_HRIGHT_ID	(0)	//拡張高さ参照インデックス（ゴーストジムリフト用）

#define GHOST_GYM_EX_HEIGHT_X	(8)
#define GHOST_GYM_EX_HEIGHT_Z	(13)
#define GHOST_GYM_EX_HEIGHT_W	(3)
#define GHOST_GYM_EX_HEIGHT_H	(2)

//鋼ジム-------------------
//フロア間距離
#define FLOOR_MARGIN	(10)
#define FLOOR_DISP_START_VAL	(9)
#define STEEL_UD_LIFT_DIF	(FX32_ONE*8)		//高さ変動値（モデル高さ用）
#define STEEL_FRLR_LIFT_DIF	(FX32_ONE*8)		//手前奥左右変動値（モデル高さ用）
#define STEEL_CENTER_OFS	(FX32_ONE*8)

#define STEEL_FOG_OFS	(0x76a0)
#define STEEL_FOG_MAX	(72)

//格闘ジム-------------------
#define DEBUG_FOOK_DP_COMBAT_GYM

#define COMBAT_WALL_POS_MAX	(15)
#define COMBAT_CENTER_OFS_X	(FX32_ONE*16*2)
#define COMBAT_CENTER_OFS_Z	(FX32_ONE*8)
enum{
	WALLNO_1 = 0,
	WALLNO_2,
	WALLNO_3,
	WALLNO_4,
	WALLNO_5,
	WALLNO_6,
	WALLNO_7,
	WALLNO_8,
	WALLNO_9,
	WALLNO_10,
	WALLNO_11,
	WALLNO_12,
};

//電気ジム------------------
#define ELEC_GEAR_NUM1_YROT	(3)
#define ELEC_GEAR_NUM2_YROT	(4)
#define ELEC_GEAR_NUM2_XROT	(2)
#define ELEC_GEAR_NUM3_YROT	(9)
#define ELEC_GEAR_NUM3_XROT	(4)

#define ELEC_GEAR_MAX1	(ELEC_GEAR_NUM1_YROT)
#define ELEC_GEAR_MAX2	(ELEC_GEAR_NUM2_YROT+ELEC_GEAR_NUM2_XROT)
#define ELEC_GEAR_MAX3	(ELEC_GEAR_NUM3_YROT+ELEC_GEAR_NUM3_XROT)
#define ELEC_GEAR_MAX	(ELEC_GEAR_MAX3)

#define ELEC_GEAR_CENTER_OFS_X	(FX32_ONE*8)
#define ELEC_GEAR_CENTER_OFS_Y	(FX32_ONE*8)
#define ELEC_GEAR_CENTER_OFS_Z	(FX32_ONE*8)


#define GEAR_ROT_DIR_LEFT	(0)		//回転が与えられたときその方向に回転する
#define GEAR_ROT_DIR_RIGHT	(1)		//回転が与えられたとき逆方向に回転する

#define GEAR_ROT_TYPE_Y	(0)			//Ｙ軸回転
#define GEAR_ROT_TYPE_X	(1)			//Ｘ軸回転

//下記3つの定義は、反時計回り回転を与えたとき、反時計回りをするギアの定義です
//2Dエディタを使用する際、
//指定回転方向と逆回転するギアの場合はROTATE_GEAR_PLUS90とROTATE_GEAR_MINUS90の使用方法を逆にします


#define GEAR_ROT_SPEED		(0x400)



typedef struct {
	int seq;							///<シーケンス保持ワーク
}EVENT_GYM_WORK;

static u8 GetBit(const u8 inBit, const int inData);

static void ChangeBit(const u8 inBit, int *outData);

//水ジム
static BOOL GMEVENT_SetGymWaterLv1(GMEVENT_CONTROL * event);
static BOOL GMEVENT_SetGymWaterLv2(GMEVENT_CONTROL * event);
static BOOL GMEVENT_SetGymWaterLv3(GMEVENT_CONTROL * event);

static void SetWaterGymButton(const u8 inWaterLv, FLD_3D_ANM_MNG_PTR field_3d_anime_ptr);

//ゴーストジム
static BOOL GMEVENT_MoveUpGymGhost(GMEVENT_CONTROL * event);
static BOOL GMEVENT_MoveDownGymGhost(GMEVENT_CONTROL * event);

//鋼ジム
static BOOL GMEVENT_MoveUDGymSteel(GMEVENT_CONTROL * event);
static BOOL GMEVENT_MoveLRGymSteel(GMEVENT_CONTROL * event);
static BOOL GMEVENT_MoveFRGymSteel(GMEVENT_CONTROL * event);

//格闘ジム
#ifndef DP_OLDGYM_NULL_FIGHT
static BOOL GMEVENT_MoveWallGymCombat(GMEVENT_CONTROL * event);
#endif

//電気ジム
static BOOL GMEVENT_RotateElecGymGear(GMEVENT_CONTROL * event);

//---------------------------------------------------------------------------
/**
 * @brief	制御ビット取得
 * 
 * @param	inBit			対象ビット
 * @param	inData			データ
 * 
 * @return	u8				bit
 */
//---------------------------------------------------------------------------
static u8 GetBit(const u8 inBit, const int inData)
{
	u8 bit;
	bit = (inData>>inBit)&(0x01);
	return bit;
}

//---------------------------------------------------------------------------
/**
 * @brief	制御ビット変更
 * 
 * @param	inBit		対象ビット
 * @param	outData		データ
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
static void ChangeBit(const u8 inBit, int *outData)
{
	(*outData)^=(0x1<<inBit);
}

//---------------------------------------------------------------------------
/**
 * @brief	水ジム:ボタンチェック
 * 
 * @param	fsys	フィールドシステムポインタ
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
void GYM_CheckWaterGymButton(FIELDSYS_WORK *fsys)
{
	TARGET_RECT rect;
	int x,z;
	BOOL rc;
	int id;
	int list[] = {BMID_R04_B1,BMID_R04_B2,BMID_R04_B3};	//ボタン1、ボタン2、ボタン3

	//*outScrID = SCRID_HONEY_TREE;
	//自機座標取得(グリッド)
	x = Player_NowGPosXGet( fsys->player );
	z = Player_NowGPosZGet( fsys->player );
	

	//自機のいるグリッドの矩形を作成
	MPTL_MakeTargetRect( x, z, 0, 0, 1, 1, &rect);

	rc = MPTL_CheckPluralMap3DObjInRect( fsys, list, NELEMS(list), &rect, NULL, &id);
	if(rc){
		EVENT_GYM_WORK * egw;
		GIMMICKWORK *work;
		WATER_GYM_WORK *water_gym_work;
		work = SaveData_GetGimmickWork(GameSystem_GetSaveData(fsys));
		water_gym_work = (WATER_GYM_WORK*)GIMMICKWORK_Get(work, FLD_GIMMICK_WATER_GYM);
		
		//イベントコール
		egw = sys_AllocMemoryLo(HEAPID_WORLD, sizeof(EVENT_GYM_WORK));
		egw->seq = 0;
		if (id == BMID_R04_B1){	//ボタン1
			FieldEvent_Call(fsys->event, GMEVENT_SetGymWaterLv3, egw);
			water_gym_work->Water = GYM_WATER_LV3;
		}else if(id == BMID_R04_B2){	//ボタン2
			FieldEvent_Call(fsys->event, GMEVENT_SetGymWaterLv2, egw);
			water_gym_work->Water = GYM_WATER_LV2;
		}else if(id == BMID_R04_B3){	//ボタン3
			FieldEvent_Call(fsys->event, GMEVENT_SetGymWaterLv1, egw);
			water_gym_work->Water = GYM_WATER_LV1;
		}else{
			GF_ASSERT(0&&"水面上昇下降エラー：OBJID不正");
		}
	}
}

//---------------------------------------------------------------------------
/**
 * @brief	水ジム:高さ当たり判定
 * 
 * @param	fsys		フィールドシステムポインタ
 * @param	inGridX		グリッドX座標
 * @param	inGridZ		グリッドZ座標
 * @param	inHeight	高さ
 * @param	outHit		判定結果	TRUE:ヒット	FALSE:ヒットしてない
 * 
 * @return	BOOL		TRUE:このあと通常当たり判定を行わない	FALSE:通常当たり判定を行う
 */
//---------------------------------------------------------------------------
BOOL GYM_HitCheckWaterGym(	FIELDSYS_WORK *fsys,
							const int inGridX, const int inGridZ,
							const fx32 inHeight, BOOL *outHit	)
{
	u8 attr;
	//アトリビュートを調べる
	attr = GetAttributeLSB(fsys, inGridX, inGridZ);
	if ( MATR_IsWaterHeightLv1(attr) ){
		if (inHeight != 0){
			(*outHit) = TRUE;
			return TRUE;
		}
	}else if ( MATR_IsWaterHeightLv2(attr) ){
		if (inHeight != FX32_ONE*16*2){
			(*outHit) = TRUE;
			return TRUE;
		}
	}else if ( MATR_IsWaterHeightLv3(attr) ){
		if (inHeight != FX32_ONE*16*4){
			(*outHit) = TRUE;
			return TRUE;
		}
	}
	(*outHit) = FALSE;
	return FALSE;	//通常当たり判定を実行
}

//---------------------------------------------------------------------------
/**
 * @brief	水ジム:水位をLV1にする
 * 
 * @param	event	イベントポインタ
 * 
 * @return	BOOL	TRUE:イベント終了	FALSE:イベント継続
 */
//---------------------------------------------------------------------------
static BOOL GMEVENT_SetGymWaterLv1(GMEVENT_CONTROL * event)
{
	M3DO_PTR ptr;
	FIELDSYS_WORK * fsys = FieldEvent_GetFieldSysWork(event);
	EVENT_GYM_WORK * egw = FieldEvent_GetSpecialWork(event);

	switch (egw->seq) {
	case 0:
		//ボタンアニメ（ボタン3を押下）
		SetWaterGymButton(GYM_WATER_LV1, fsys->field_3d_anime);
		(egw->seq) ++;
		break;
	case 1:			//水位を下降
		{
			FLD_3D_ANM_DAT_PTR anime1,anime2,anime3;
			anime1 = F3DA_GetFld3DAnmPtrByCode(WATER_GYM_BTN_ANM1, fsys->field_3d_anime);
			anime2 = F3DA_GetFld3DAnmPtrByCode(WATER_GYM_BTN_ANM2, fsys->field_3d_anime);
			anime3 = F3DA_GetFld3DAnmPtrByCode(WATER_GYM_BTN_ANM3, fsys->field_3d_anime);
			if ( F3DA_IsAnimeEnd(anime1) &&
				 F3DA_IsAnimeEnd(anime2) &&
				 F3DA_IsAnimeEnd(anime3) ){
				//水面OBJを取得
				ptr = M3DO_GetMap3DObjDataFromID(fsys->Map3DObjExp, BMID_R04_W);
				//y座標変更
				{
					VecFx32 vec;
					vec = M3DO_GetGlobalVec(ptr);
					vec.y -= WATER_DIF;	
					if (vec.y <= MODEL_WATER_LV1){	//セット終了
						vec.y = MODEL_WATER_LV1;
						(egw->seq) ++;
					}
					M3DO_SetGlobalVec(ptr, &vec);
				}
			}
		}
		break;
	case 2:	//高さセット
		EXH_SetExHeight( WTAER_GYM_DATA_HRIGHT_ID, WATER_LV1, fsys->ExHeightList );
		//水位変更音停止
		Snd_SeStopBySeqNo( SE_NOMOSE_GYM_BUTTON, 0 );
		(egw->seq) ++;
		break;
	case 3:
		sys_FreeMemoryEz(egw);
		return TRUE;
	}
	return FALSE;
}

//---------------------------------------------------------------------------
/**
 * @brief	水ジム:水位をLV2にする
 * 
 * @param	event	イベントポインタ
 * 
 * @return	BOOL	TRUE:イベント終了	FALSE:イベント継続
 */
//---------------------------------------------------------------------------
static BOOL GMEVENT_SetGymWaterLv2(GMEVENT_CONTROL * event)
{
	M3DO_PTR ptr;
	FIELDSYS_WORK * fsys = FieldEvent_GetFieldSysWork(event);
	EVENT_GYM_WORK * egw = FieldEvent_GetSpecialWork(event);

	switch (egw->seq) {
	case 0:	//上昇されるか、下降させるかを決定
		//ボタンアニメ（ボタン2を押下）
		SetWaterGymButton(GYM_WATER_LV2, fsys->field_3d_anime);
		(egw->seq) ++;
		break;
	case 1:
		{
			FLD_3D_ANM_DAT_PTR anime1,anime2,anime3;
			anime1 = F3DA_GetFld3DAnmPtrByCode(WATER_GYM_BTN_ANM1, fsys->field_3d_anime);
			anime2 = F3DA_GetFld3DAnmPtrByCode(WATER_GYM_BTN_ANM2, fsys->field_3d_anime);
			anime3 = F3DA_GetFld3DAnmPtrByCode(WATER_GYM_BTN_ANM3, fsys->field_3d_anime);
			if ( F3DA_IsAnimeEnd(anime1) &&
				 F3DA_IsAnimeEnd(anime2) &&
				 F3DA_IsAnimeEnd(anime3) ){
				//現在の水位を取得
				//水面OBJを取得
				ptr = M3DO_GetMap3DObjDataFromID(fsys->Map3DObjExp, BMID_R04_W);
				{
					VecFx32 vec;
					vec = M3DO_GetGlobalVec(ptr);
					if (vec.y == MODEL_WATER_LV1){
						(egw->seq) = 2;
					}else if(vec.y == MODEL_WATER_LV2){
						sys_FreeMemoryEz(egw);
						return TRUE;;
					}else if(vec.y == MODEL_WATER_LV3){
						(egw->seq) = 3;
					}else{
						GF_ASSERT(0);
					}
				}
			}
		}	
		break;
	case 2:			//水位を上昇
		//水面OBJを取得
		ptr = M3DO_GetMap3DObjDataFromID(fsys->Map3DObjExp, BMID_R04_W);
		//y座標変更
		{
			VecFx32 vec;
			vec = M3DO_GetGlobalVec(ptr);
			vec.y += WATER_DIF;	
			if (vec.y >= MODEL_WATER_LV2){	//セット終了
				vec.y = MODEL_WATER_LV2;
				(egw->seq) = 4;
			}
			M3DO_SetGlobalVec(ptr, &vec);
		}
		break;
	case 3:		//水位を降下
		//水面OBJを取得
		ptr = M3DO_GetMap3DObjDataFromID(fsys->Map3DObjExp, BMID_R04_W);
		//y座標変更
		{
			VecFx32 vec;
			vec = M3DO_GetGlobalVec(ptr);
			vec.y -= WATER_DIF;	
			if (vec.y <= MODEL_WATER_LV2){	//セット終了
				vec.y = MODEL_WATER_LV2;
				(egw->seq) = 4;
			}
			M3DO_SetGlobalVec(ptr, &vec);
		}
		break;
	case 4:	//高さセット
		EXH_SetExHeight( WTAER_GYM_DATA_HRIGHT_ID, WATER_LV2, fsys->ExHeightList );
		//水位変更音停止
		Snd_SeStopBySeqNo( SE_NOMOSE_GYM_BUTTON, 0 );
		(egw->seq) ++;
		break;
	case 5:
		sys_FreeMemoryEz(egw);
		return TRUE;
	}
	return FALSE;
}

//---------------------------------------------------------------------------
/**
 * @brief	水ジム:水位をLV3にする
 * 
 * @param	event	イベントポインタ
 * 
 * @return	BOOL	TRUE:イベント終了	FALSE:イベント継続
 */
//---------------------------------------------------------------------------
static BOOL GMEVENT_SetGymWaterLv3(GMEVENT_CONTROL * event)
{
	M3DO_PTR ptr;
	FIELDSYS_WORK * fsys = FieldEvent_GetFieldSysWork(event);
	EVENT_GYM_WORK * egw = FieldEvent_GetSpecialWork(event);

	switch (egw->seq) {
	case 0:
		//ボタンアニメ（ボタン1を押下）
		SetWaterGymButton(GYM_WATER_LV3, fsys->field_3d_anime);
		(egw->seq) ++;
		break;	
	case 1:			//水位を上昇
		{
			FLD_3D_ANM_DAT_PTR anime1,anime2,anime3;
			anime1 = F3DA_GetFld3DAnmPtrByCode(WATER_GYM_BTN_ANM1, fsys->field_3d_anime);
			anime2 = F3DA_GetFld3DAnmPtrByCode(WATER_GYM_BTN_ANM2, fsys->field_3d_anime);
			anime3 = F3DA_GetFld3DAnmPtrByCode(WATER_GYM_BTN_ANM3, fsys->field_3d_anime);
			if ( F3DA_IsAnimeEnd(anime1) &&
				 F3DA_IsAnimeEnd(anime2) &&
				 F3DA_IsAnimeEnd(anime3) ){
				//水面OBJを取得
				ptr = M3DO_GetMap3DObjDataFromID(fsys->Map3DObjExp, BMID_R04_W);
				//y座標変更
				{
					VecFx32 vec;
					vec = M3DO_GetGlobalVec(ptr);
					vec.y += WATER_DIF;	
					if (vec.y >= MODEL_WATER_LV3){	//セット終了
						vec.y = MODEL_WATER_LV3;
						(egw->seq) ++;
					}
					M3DO_SetGlobalVec(ptr, &vec);
				}
			}
		}
		break;
	case 2:	//高さセット
		EXH_SetExHeight( WTAER_GYM_DATA_HRIGHT_ID, WATER_LV3, fsys->ExHeightList );
		//水位変更音停止
		Snd_SeStopBySeqNo( SE_NOMOSE_GYM_BUTTON, 0 );
		(egw->seq) ++;
		break;
	case 3:
		sys_FreeMemoryEz(egw);
		return TRUE;
	}
	return FALSE;
}

//---------------------------------------------------------------------------
/**
 * @brief	水ジム:セットアップ
 * 
 * @param	fsys	フィールドシステムポインタ
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
void GYM_SetupWaterGym(FIELDSYS_WORK *fsys)
{
	u8 water;
	M3DO_PTR ptr;
	fx32 model_water_height;
	fx32 data_height;
	BOOL reverse1,reverse2,reverse3;
	GIMMICKWORK *work;
	WATER_GYM_WORK *water_gym_work;
	
	//ここでギミックワークの中身を見る
	work = SaveData_GetGimmickWork(GameSystem_GetSaveData(fsys));
	water_gym_work = (WATER_GYM_WORK*)GIMMICKWORK_Get(work, FLD_GIMMICK_WATER_GYM);

	//水面の作成
	{
		VecFx32 vec = {FX32_ONE*16*16,0,FX32_ONE*16*16};
		M3DO_SetMap3DObjExp(fsys->Map3DObjExp,
							fsys->MapResource,
							BMID_R04_W,
							&vec, NULL,
							fsys->field_3d_anime );//<水ジム水面
	}

	//拡張高さの作成
	EXH_SetUpExHeightData(	WTAER_GYM_DATA_HRIGHT_ID,
							WATER_GYM_EX_HEIGHT_X,
							WATER_GYM_EX_HEIGHT_Z,
							WATER_GYM_EX_HEIGHT_W,
							WATER_GYM_EX_HEIGHT_H,
							WATER_LV1,fsys->ExHeightList );

	//水位を取得
	water = water_gym_work->Water;
	//取得した水位を元に、
	//水の高さ、データの高さ、ボタンの押下状況をセット
	
	switch(water){
	case GYM_WATER_LV1:		//ボタン3が押されている(１段階)
		model_water_height = MODEL_WATER_LV1;
		data_height = WATER_LV1;
		reverse1 = TRUE;
		reverse2 = TRUE;
		reverse3 = FALSE;
		break;
	case GYM_WATER_LV2:		//ボタン2が押されている（２段階）
		model_water_height = MODEL_WATER_LV2;
		data_height = WATER_LV2;
		reverse1 = TRUE;
		reverse2 = FALSE;
		reverse3 = TRUE;
		break;
	case GYM_WATER_LV3:		//ボタン1が押されている（３段階）
		model_water_height = MODEL_WATER_LV3;
		data_height = WATER_LV3;
		reverse1 = FALSE;
		reverse2 = TRUE;
		reverse3 = TRUE;
		break;
	default:	//データが壊れている可能性がある（ボタン1が押されていることにしておく）
		GF_ASSERT(0&&"データが不正です");
		model_water_height = MODEL_WATER_LV1;
		data_height = WATER_LV1;
		reverse1 = TRUE;
		reverse2 = TRUE;
		reverse3 = FALSE;
	}
	
	//水面OBJを取得
	ptr = M3DO_GetMap3DObjDataFromID(fsys->Map3DObjExp, BMID_R04_W);
	//モデルのy座標変更
	{
		VecFx32 vec;
		vec = M3DO_GetGlobalVec(ptr);
		vec.y = model_water_height;
		M3DO_SetGlobalVec(ptr, &vec);
	}
	//
	//高さデータの変更
	EXH_SetExHeight( WTAER_GYM_DATA_HRIGHT_ID, data_height, fsys->ExHeightList );
	
	//３種類のボタン状況セット
	{
		FLD_3D_ANM_DAT_PTR anime;
		//ボタン1
		//アニメ検索
		anime = F3DA_GetFld3DAnmPtr( BMID_R04_B1, 0, fsys->field_3d_anime );
		//制御コードセット
		F3DA_SetControlCode( anime, WATER_GYM_BTN_ANM1 );
		//リバースセット
		F3DA_SetAnimeReverseDirect( anime, reverse1 );
		//ループ回数セット
		F3DA_SetAnimeLoopMax( anime, 1 );
		//ストップフラグセット
		F3DA_SetStopFlg( anime, TRUE );	//停止
		//アニメ終端フレームにセット
		F3DA_SetLastFrame( anime );

		//ボタン2
		//アニメ検索
		anime = F3DA_GetFld3DAnmPtr( BMID_R04_B2, 0, fsys->field_3d_anime );
		//制御コードセット
		F3DA_SetControlCode( anime, WATER_GYM_BTN_ANM2 );
		//リバースセット
		F3DA_SetAnimeReverseDirect( anime, reverse2 );
		//ループ回数セット
		F3DA_SetAnimeLoopMax( anime, 1 );
		//ストップフラグセット
		F3DA_SetStopFlg( anime, TRUE );	//停止
		//アニメ終端フレームにセット
		F3DA_SetLastFrame( anime );

		//ボタン3
		//アニメ検索
		anime = F3DA_GetFld3DAnmPtr( BMID_R04_B3, 0, fsys->field_3d_anime );
		//制御コードセット
		F3DA_SetControlCode( anime, WATER_GYM_BTN_ANM3 );
		//リバースセット
		F3DA_SetAnimeReverseDirect( anime, reverse3 );
		//ループ回数セット
		F3DA_SetAnimeLoopMax( anime, 1 );
		//ストップフラグセット
		F3DA_SetStopFlg( anime, TRUE );	//停止
		//アニメ終端フレームにセット
		F3DA_SetLastFrame( anime );
	}
}

//---------------------------------------------------------------------------
/**
 * @brief	水ジム:ボタンアニメのセット
 * 
 * @param	inWaterLv			水位レベル
 * @param	field_3d_anime_ptr	アニメ管理ポインタ
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
static void SetWaterGymButton(const u8 inWaterLv, FLD_3D_ANM_MNG_PTR field_3d_anime_ptr)
{
	FLD_3D_ANM_DAT_PTR anime;
	int anime_a,anime_b,anime_c;
	
	if (inWaterLv == GYM_WATER_LV3){	//ボタン１
		anime_a = WATER_GYM_BTN_ANM1;
		anime_b = WATER_GYM_BTN_ANM2;
		anime_c = WATER_GYM_BTN_ANM3;
	}else if(inWaterLv == GYM_WATER_LV2){			//ボタン２
		anime_a = WATER_GYM_BTN_ANM2;
		anime_b = WATER_GYM_BTN_ANM1;
		anime_c = WATER_GYM_BTN_ANM3;
	}else if(inWaterLv == GYM_WATER_LV1){			//ボタン３
		anime_a = WATER_GYM_BTN_ANM3;
		anime_b = WATER_GYM_BTN_ANM1;
		anime_c = WATER_GYM_BTN_ANM2;
	}else{
		GF_ASSERT(0);
	}
	anime = F3DA_GetFld3DAnmPtrByCode(anime_a, field_3d_anime_ptr);
	F3DA_SetAnimeReverseDirect(anime, FALSE);
	F3DA_SetStartFrame(anime);
	F3DA_SetStopFlg(anime, FALSE);
	F3DA_MoveAnime(anime);
		
	anime = F3DA_GetFld3DAnmPtrByCode(anime_b, field_3d_anime_ptr);
	F3DA_SetAnimeReverseDirect(anime, TRUE);
	F3DA_SetStartFrame(anime);
	F3DA_SetStopFlg(anime, FALSE);
	F3DA_MoveAnime(anime);

	anime = F3DA_GetFld3DAnmPtrByCode(anime_c, field_3d_anime_ptr);
	F3DA_SetAnimeReverseDirect(anime, TRUE);
	F3DA_SetStartFrame(anime);
	F3DA_SetStopFlg(anime, FALSE);
	F3DA_MoveAnime(anime);

	//水量変更音
	Snd_SePlay( SE_NOMOSE_GYM_BUTTON );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

//ゴーストジム

//---------------------------------------------------------------------------
/**
 * @brief	ゴーストジム:セットアップ
 * 
 * @param	fsys	フィールドシステムポインタ
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
void GYM_SetupGhostGym(FIELDSYS_WORK *fsys)
{
	u8 lift;
	M3DO_PTR ptr;
	fx32 model_lift_height;
	fx32 data_height;
	GIMMICKWORK *work;
	GHOST_GYM_WORK *ghost_gym_work;
	
	//ここでギミックワークの中身を見る
	work = SaveData_GetGimmickWork(GameSystem_GetSaveData(fsys));
	ghost_gym_work = (GHOST_GYM_WORK*)GIMMICKWORK_Get(work, FLD_GIMMICK_GHOST_GYM);
	
	//リフトの作成
	{
		VecFx32 vec = {FX32_ONE*(16*9+8),0,FX32_ONE*(16*13+8)};
		M3DO_SetMap3DObjExp(fsys->Map3DObjExp,
							fsys->MapResource,
							BMID_LIFT_BASE01,
							&vec, NULL,
							fsys->field_3d_anime );//<リフト
	}
	
	//拡張高さの作成
	EXH_SetUpExHeightData(	GHOST_GYM_DATA_HRIGHT_ID,
							GHOST_GYM_EX_HEIGHT_X,
							GHOST_GYM_EX_HEIGHT_Z,
							GHOST_GYM_EX_HEIGHT_W,
							GHOST_GYM_EX_HEIGHT_H,
							GHOST_LIFT_LV1,fsys->ExHeightList );

	//リフトの位置を取得
	lift = ghost_gym_work->Lift;
	
	switch(lift){
	case GYM_GHOST_LIFT_LV1:		//下フロア
		model_lift_height = MODEL_GHOST_LIFT_LV1;
		data_height = GHOST_LIFT_LV1;
		break;
	case GYM_GHOST_LIFT_LV2:		//上フロア
		model_lift_height = MODEL_GHOST_LIFT_LV2;
		data_height = GHOST_LIFT_LV2;
		break;
	default:	//データが壊れている可能性がある
		GF_ASSERT(0 && "データが不正です");
		model_lift_height = MODEL_GHOST_LIFT_LV1;
		data_height = GHOST_LIFT_LV1;
	}
	
	//リフトOBJを取得
	ptr = M3DO_GetMap3DObjDataFromID(fsys->Map3DObjExp, BMID_LIFT_BASE01);
	//モデルのy座標変更
	{
		VecFx32 vec;
		vec = M3DO_GetGlobalVec(ptr);
		vec.y = model_lift_height;
		M3DO_SetGlobalVec(ptr, &vec);
	}
	//
	//高さデータの変更
	EXH_SetExHeight( GHOST_GYM_DATA_HRIGHT_ID, data_height, fsys->ExHeightList );
}

//---------------------------------------------------------------------------
/**
 * @brief	ゴーストジム:リフト移動
 * 
 * @param	fsys	フィールドシステムポインタ
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
void GYM_MoveGhostGymLift(FIELDSYS_WORK *fsys)
{
	EVENT_GYM_WORK * egw;
	
	//イベントコール
	egw = sys_AllocMemoryLo( HEAPID_WORLD, sizeof(EVENT_GYM_WORK) );
	egw->seq = 0;
	
	//自機のy座標からコールするイベントを切り替える
	{
		VecFx32 vec;
		GIMMICKWORK *work;
		GHOST_GYM_WORK *ghost_gym_work;
		work = SaveData_GetGimmickWork(GameSystem_GetSaveData(fsys));
		ghost_gym_work = (GHOST_GYM_WORK*)GIMMICKWORK_Get(work, FLD_GIMMICK_GHOST_GYM);
		Player_VecPosGet( fsys->player, &vec );
		if (vec.y == GHOST_LIFT_LV1){
			FieldEvent_Call(fsys->event, GMEVENT_MoveUpGymGhost, egw);
			ghost_gym_work->Lift = GYM_GHOST_LIFT_LV2;
		}else{
			FieldEvent_Call(fsys->event, GMEVENT_MoveDownGymGhost, egw);
			ghost_gym_work->Lift = GYM_GHOST_LIFT_LV1;
		}
	}
}

//---------------------------------------------------------------------------
/**
 * @brief	ゴーストジム:リフトを上げる
 * 
 * @param	event	イベントポインタ
 * 
 * @return	BOOL TRUE:イベント終了	FALSE:イベント継続
 */
//---------------------------------------------------------------------------
static BOOL GMEVENT_MoveUpGymGhost(GMEVENT_CONTROL * event)
{
	M3DO_PTR ptr;
	FIELDSYS_WORK * fsys = FieldEvent_GetFieldSysWork(event);
	EVENT_GYM_WORK * egw = FieldEvent_GetSpecialWork(event);

	switch (egw->seq) {
	case 0:
		Player_HeightGet_ON_OFF( fsys->player, FALSE );//自機の高さ自動取得をオフ

		//リフト音再生
		Snd_SePlay( SEQ_SE_DP_ELEBETA );
		(egw->seq) ++;
		break;
	case 1:	//リフトを上昇
		{
			//リフトOBJを取得
			ptr = M3DO_GetMap3DObjDataFromID(fsys->Map3DObjExp, BMID_LIFT_BASE01);
			//y座標変更
			{
				VecFx32 vec;
				vec = M3DO_GetGlobalVec(ptr);
				vec.y += GHOST_LIFT_DIF;

				if (vec.y >= MODEL_GHOST_LIFT_LV2){	//セット終了
					vec.y = MODEL_GHOST_LIFT_LV2;
					//リフト音停止
					Snd_SeStopBySeqNo( SEQ_SE_DP_ELEBETA, 0 );
					(egw->seq) ++;
				}
				Player_VecPosYSet( fsys->player, vec.y );	//自機高さ変更
				M3DO_SetGlobalVec(ptr, &vec);
			}
		}
		break;
	case 2:	//高さセット
		EXH_SetExHeight( GHOST_GYM_DATA_HRIGHT_ID, GHOST_LIFT_LV2, fsys->ExHeightList );
		//自機の高さ自動取得をオン
		Player_HeightGetSet_ON_OFF( fsys->player, TRUE );

		//リフト移動終了音
		Snd_SePlay( SE_YOSUGA_GYM_LIFT );
		(egw->seq) ++;
		break;
	case 3:
		sys_FreeMemoryEz(egw);
		return TRUE;
	}
	return FALSE;
}

//---------------------------------------------------------------------------
/**
 * @brief	ゴーストジム:リフトを下げる
 * 
 * @param	event	イベントポインタ
 * 
 * @return	BOOL TRUE:イベント終了	FALSE:イベント継続
 */
//---------------------------------------------------------------------------
static BOOL GMEVENT_MoveDownGymGhost(GMEVENT_CONTROL * event)
{
	M3DO_PTR ptr;
	FIELDSYS_WORK * fsys = FieldEvent_GetFieldSysWork(event);
	EVENT_GYM_WORK * egw = FieldEvent_GetSpecialWork(event);

	switch (egw->seq) {
	case 0:
		Player_HeightGet_ON_OFF( fsys->player, FALSE );//自機の高さ自動取得をオフ

		//リフト音再生
		Snd_SePlay( SEQ_SE_DP_ELEBETA );
		(egw->seq)	++;
		break;
	case 1:	//リフトを下降
		{
			//リフトOBJを取得
			ptr = M3DO_GetMap3DObjDataFromID(fsys->Map3DObjExp, BMID_LIFT_BASE01);
			//y座標変更
			{
				VecFx32 vec;
				vec = M3DO_GetGlobalVec(ptr);
				vec.y -= GHOST_LIFT_DIF;
				
				if (vec.y <= MODEL_GHOST_LIFT_LV1){	//セット終了
					vec.y = MODEL_GHOST_LIFT_LV1;
					//リフト音停止
					Snd_SeStopBySeqNo( SEQ_SE_DP_ELEBETA, 0 );
					(egw->seq) ++;
				}
				Player_VecPosYSet( fsys->player, vec.y );	//自機高さ変更
				M3DO_SetGlobalVec(ptr, &vec);
			}
		}
		break;
	case 2:	//高さセット
		EXH_SetExHeight( GHOST_GYM_DATA_HRIGHT_ID, GHOST_LIFT_LV1, fsys->ExHeightList );
		//自機の高さ自動取得をオン
		Player_HeightGetSet_ON_OFF( fsys->player, TRUE );

		//リフト移動終了音
		Snd_SePlay( SE_YOSUGA_GYM_LIFT );
		(egw->seq) ++;
		break;
	case 3:
		sys_FreeMemoryEz(egw);
		return TRUE;
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////

//鋼ジム
static const u8 SteelGymHitTbl[4][1024] = 
{
	//#gym02_attr_01
	{
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,
1,1,1,1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,
1,1,1,1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,
1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,
1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,
1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,
1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,
1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,
1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,
1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,
1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,1,1,1,1,
1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,1,1,1,1,1,
1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,
1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,
1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,
1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,1,1,1,1,0,0,0,1,
1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1,
1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,
1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	},
	//#gym02_attr_02
	{
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
1,1,1,1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
1,1,1,1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
1,1,1,1,1,1,0,0,0,1,0,0,0,1,1,1,1,1,1,1,0,0,1,0,1,1,1,1,0,0,0,1,
1,1,1,1,1,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,1,0,0,0,1,
1,1,1,1,1,1,1,1,1,1,0,0,0,1,0,1,1,1,0,1,0,0,1,0,0,0,0,1,0,0,0,1,
1,1,1,1,1,1,1,1,1,1,0,0,0,1,0,1,1,1,0,1,0,0,1,0,0,0,0,1,0,0,0,1,
1,1,1,1,1,1,1,1,1,1,0,0,0,1,0,1,1,1,0,1,0,0,1,0,0,0,0,1,0,0,0,1,
1,1,1,1,1,1,0,0,0,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
1,1,1,1,1,1,1,1,1,1,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
1,1,1,1,1,1,1,1,1,1,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,0,0,0,0,0,0,0,0,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,0,0,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,1,0,0,0,1,0,0,0,1,
1,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,1,0,0,0,1,0,0,0,1,
1,0,0,0,0,0,0,1,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,1,0,0,0,1,0,0,0,1,
1,0,0,0,0,0,0,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,0,0,0,0,0,0,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,0,0,0,0,0,0,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,0,0,0,0,0,0,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	},
	//#gym02_attr_03
	{
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,0,0,1,0,0,0,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,1,
1,0,0,1,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,
1,0,0,1,0,0,0,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,1,
1,0,0,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,1,
1,0,0,1,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,0,1,1,1,1,1,
1,0,0,1,0,0,1,1,1,1,1,1,1,1,0,1,1,1,0,0,1,1,1,1,0,1,0,1,0,0,0,1,
1,0,0,1,0,0,1,1,1,1,1,1,1,1,0,1,1,1,0,0,1,1,1,1,0,1,0,1,0,0,0,1,
1,0,0,1,0,0,1,1,1,1,1,1,1,1,0,1,1,1,0,0,1,1,1,1,0,1,0,1,0,0,0,1,
1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,0,0,0,1,
1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,0,1,0,0,0,1,
1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,0,0,0,1,
1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,0,0,0,1,
1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,
1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,
1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,
1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,
1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,0,0,0,0,0,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	},
	//#gym02_attr_04
	{
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,1,1,
1,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,
1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,
1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,
1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,
1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	},
};

typedef enum {
	STEEL_FLOOR_1F = 0,
	STEEL_FLOOR_2F,
	STEEL_FLOOR_3F,
	STEEL_FLOOR_4F,
}STEEL_FLOOR;

typedef enum { 
	MOVE_DIR_UP,
	MOVE_DIR_DOWN,
	MOVE_DIR_REAR,
	MOVE_DIR_FRONT,
	MOVE_DIR_RIGHT,
	MOVE_DIR_LEFT,
}MOVE_DIR;

typedef struct LIFT_POSITION_tag
{
	u8 GridX;
	u8 Height;
	u8 GridZ;
}LIFT_POSITION;

typedef struct STEEL_GYM_TEMP_WORK_tag
{
	FIELDSYS_WORK *fsys;
	int Floor2F;
	int Floor3F;
	int Floor4F;
	u8 LiftHeight[STEEL_LIFT_MAX];	//フロア識別用バッファ　0:1F 1:2F 2:3F 3:4F
	u8 LiftEntryIdx[STEEL_LIFT_MAX];		//拡張配置モデル登録インデックス
	LIFT_POSITION	LiftPos[STEEL_LIFT_MAX];
	int ObjID[STEEL_LIFT_MAX];
	u8 ListIdx;
	fx32 Dst;
	MOVE_DIR MoveDir;
}STEEL_GYM_TEMP_WORK;

typedef struct STEEL_LIFT_INFO_tag
{
	int ObjID;
	LIFT_POSITION	BeforePos;
	LIFT_POSITION	AffterPos;
	u8 BeforeHeight;
	u8 AffterHeight;
	u16 dummy;	
}STEEL_LIFT_INFO;

static const STEEL_LIFT_INFO LiftInfo[STEEL_LIFT_MAX] = {
	{ BMID_LIFT_BASE05, {16,  0,  9},{16, 30,  9}, STEEL_FLOOR_1F, STEEL_FLOOR_4F, 0 },
	{ BMID_LIFT_BASE04, {11,  0, 13},{11, 10, 13}, STEEL_FLOOR_1F, STEEL_FLOOR_2F, 0 },
	{ BMID_LIFT_BASE04, {15,  0, 13},{15, 10, 13}, STEEL_FLOOR_1F, STEEL_FLOOR_2F, 0 },
	{ BMID_LIFT_BASE04, {19,  0, 13},{19, 10, 13}, STEEL_FLOOR_1F, STEEL_FLOOR_2F, 0 },
	{ BMID_LIFT_BASE04, {24,  0, 13},{24, 10, 13}, STEEL_FLOOR_1F, STEEL_FLOOR_2F, 0 },
	{ BMID_LIFT_BASE04, {21,  0, 22},{21, 10, 22}, STEEL_FLOOR_1F, STEEL_FLOOR_2F, 0 },
	
	{ BMID_LIFT_BASE04, {25,  0,  9},{25, 10,  9}, STEEL_FLOOR_1F, STEEL_FLOOR_2F, 1 },
	{ BMID_LIFT_BASE04, {25,  0, 22},{25, 10, 22}, STEEL_FLOOR_1F, STEEL_FLOOR_2F, 1 },
	{ BMID_LIFT_BASE04, {29,  0, 22},{29, 10, 22}, STEEL_FLOOR_1F, STEEL_FLOOR_2F, 1 },
	{ BMID_LIFT_BASE04, { 5, 10, 26},{ 5, 20, 26}, STEEL_FLOOR_2F, STEEL_FLOOR_3F, 0 },
	{ BMID_LIFT_BASE03, {11, 10, 22},{18, 10, 22}, STEEL_FLOOR_2F, STEEL_FLOOR_2F, 1 } ,
	
	{ BMID_LIFT_BASE04, {29, 10,  9},{29, 20,  9}, STEEL_FLOOR_2F, STEEL_FLOOR_3F, 1 },
	{ BMID_LIFT_BASE03, {10, 20,  4},{14, 20,  4}, STEEL_FLOOR_3F, STEEL_FLOOR_3F, 0 },
	{ BMID_LIFT_BASE03, {19, 20,  4},{22, 20,  4}, STEEL_FLOOR_3F, STEEL_FLOOR_3F, 0 },
	{ BMID_LIFT_BASE03, { 7, 20, 12},{22, 20, 12}, STEEL_FLOOR_3F, STEEL_FLOOR_3F, 0 },
	{ BMID_LIFT_BASE03, { 9, 20, 26},{21, 20, 26}, STEEL_FLOOR_3F, STEEL_FLOOR_3F, 0 },
	{ BMID_LIFT_BASE02, { 2, 20, 19},{ 2, 20, 22}, STEEL_FLOOR_3F, STEEL_FLOOR_3F, 1 },
	{ BMID_LIFT_BASE02, {26, 20, 16},{26, 20, 22}, STEEL_FLOOR_3F, STEEL_FLOOR_3F, 0 },
	{ BMID_LIFT_BASE02, {29, 20, 16},{29, 20, 22}, STEEL_FLOOR_3F, STEEL_FLOOR_3F, 1 },

	{ BMID_LIFT_BASE05, { 7,  0,  9},{ 7, 30,  9}, STEEL_FLOOR_1F, STEEL_FLOOR_4F, 1 },
	{ BMID_LIFT_BASE03, {19, 30,  4},{26, 30,  4}, STEEL_FLOOR_4F, STEEL_FLOOR_4F, 0 },
	{ BMID_LIFT_BASE03, { 5, 30, 26},{26, 30, 26}, STEEL_FLOOR_4F, STEEL_FLOOR_4F, 1 },
	{ BMID_LIFT_BASE02, {29, 30,  7},{29, 30, 23}, STEEL_FLOOR_4F, STEEL_FLOOR_4F, 0 },
	{ BMID_LIFT_BASE02, { 2, 30, 12},{ 2, 30, 23}, STEEL_FLOOR_4F, STEEL_FLOOR_4F, 1 },
};

//---------------------------------------------------------------------------
/**
 * @brief	鋼ジム:リフトのバニッシュ
 * 
 * @param	inSteelTemp		鋼ジムテンポラリワーク
 * @param	inInvisible			バニッシュフラグ
 * @param	inFloor				対象フロア
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
static void InvisibleLift(const STEEL_GYM_TEMP_WORK *inSteelTemp,
						const BOOL inInvisible,
						const STEEL_FLOOR inFloor)
{
	M3DO_PTR obj_ptr;
	int i;
	for(i=0;i<STEEL_LIFT_MAX;i++){
		if (inSteelTemp->LiftHeight[i] == inFloor){
			obj_ptr = M3DO_GetMap3DObjData(	inSteelTemp->fsys->Map3DObjExp,
											inSteelTemp->LiftEntryIdx[i] );
			M3DO_SetVanish(obj_ptr, inInvisible);
		}
	}
}

//---------------------------------------------------------------------------
/**
 * @brief	鋼ジム:リフトヒット判定
 * 
 * @param	inSteelTemp		鋼ジムテンポラリワーク
 * 
 * @return	u8					ヒットしたリフトインデックス
 */
//---------------------------------------------------------------------------
static u8 CheckLiftHit(STEEL_GYM_TEMP_WORK *inSteelTemp)
{
	u8 i;
	u8 height;
	u8 grid_x;
	u8 grid_z;
	VecFx32 vec;
	Player_VecPosGet( inSteelTemp->fsys->player, &vec );
	//自機の高さ取得
	height = vec.y/ONE_HEIGHT;
	//自機のグリッド座標を取得
	grid_x = vec.x/ONE_GRID;
	grid_z = vec.z/ONE_GRID;
	
	for(i=0;i<STEEL_LIFT_MAX;i++){
		//高さ比較
		if (inSteelTemp->LiftPos[i].Height == height){
			//XZ比較
			if ( (inSteelTemp->LiftPos[i].GridX == grid_x) &&
					(inSteelTemp->LiftPos[i].GridZ == grid_z) ){
				return i;
			}
		}
	}
	return STEEL_LIFT_MAX;
}
#if 0
//---------------------------------------------------------------------------
/**
 * @brief	鋼ジム:リフト制御ビット取得
 * 
 * @param	inSteelGumWork		鋼ジムワーク
 * @param	inBit				対象ビット
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
static u8 GetLiftPosBit(const STEEL_GYM_WORK *inSteelGymWork, const u8 inBit)
{
	u8 bit;
	bit = (inSteelGymWork->LiftPosBit>>inBit)&(0x01);
	return bit;
}

//---------------------------------------------------------------------------
/**
 * @brief	鋼ジム:リフト制御ビットセット
 * 
 * @param	outSteelGymWork		鋼ジムワーク
 * @param	inBit				対象ビット
 * @param	inVal				値
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
static void SetLiftPosBit(STEEL_GYM_WORK *outSteelGymWork, const u8 inBit, const u8 inVal)
{
	int u,m,d;
	GF_ASSERT((inVal<=1)&&"2以上の値がセットされています");
	
	u = outSteelGymWork->LiftPosBit & (0xffffffff<<(inBit+1));
	d = outSteelGymWork->LiftPosBit & (0xffffffff>>(32-inBit));
	m = inVal<<inBit;
	outSteelGymWork->LiftPosBit = (u|m|d);
}

//---------------------------------------------------------------------------
/**
 * @brief	鋼ジム:リフト制御ビット変更
 * 
 * @param	outSteelGymWork		鋼ジムワーク
 * @param	inBit				対象ビット
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
static void ChangeLiftPosBit(STEEL_GYM_WORK *outSteelGymWork, const u8 inBit)
{
	outSteelGymWork->LiftPosBit^=(0x1<<inBit);
}
#endif
//---------------------------------------------------------------------------
/**
 * @brief	鋼ジム:リフト移動イベントコール
 * 
 * @param	ioSteelTemp		鋼ジムテンポラリワーク
 * @param	inIdx			リフトインデックス
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
static void CallLiftMoveEvent(STEEL_GYM_TEMP_WORK *ioSteelTemp, const u8 inIdx)
{
	int lift_type;
	u8 bit;
	EVENT_GYM_WORK * egw;
	
	GIMMICKWORK *work;
	STEEL_GYM_WORK *steel_gym_work;
	work = SaveData_GetGimmickWork(GameSystem_GetSaveData(ioSteelTemp->fsys));
	steel_gym_work = (STEEL_GYM_WORK*)GIMMICKWORK_Get(work, FLD_GIMMICK_STEEL_GYM);
	
	egw = sys_AllocMemoryLo(HEAPID_WORLD, sizeof(EVENT_GYM_WORK));
	egw->seq = 0;

	ioSteelTemp->ListIdx = inIdx;	
	
	//対象リフトのタイプを取得
	lift_type = ioSteelTemp->ObjID[inIdx];
	//対象リフトの位置を取得
	bit = GetBit(inIdx, steel_gym_work->LiftPosBit);
	///bit = GetLiftPosBit(steel_gym_work, inIdx);
	//位置更新
	///ChangeLiftPosBit(steel_gym_work, inIdx);
	ChangeBit(inIdx,&steel_gym_work->LiftPosBit);
	//移動イベントをコール
	if (lift_type == BMID_LIFT_BASE03){	//左右
		if (bit){		//左へ
			ioSteelTemp->MoveDir = MOVE_DIR_LEFT;
			ioSteelTemp->Dst = LiftInfo[inIdx].BeforePos.GridX*ONE_GRID+STEEL_CENTER_OFS;
			ioSteelTemp->LiftPos[inIdx] = LiftInfo[inIdx].BeforePos;		
		}else{			//右へ
			ioSteelTemp->MoveDir = MOVE_DIR_RIGHT;
			ioSteelTemp->Dst = LiftInfo[inIdx].AffterPos.GridX*ONE_GRID+STEEL_CENTER_OFS;
			ioSteelTemp->LiftPos[inIdx] = LiftInfo[inIdx].AffterPos;
		}
		FieldEvent_Set(ioSteelTemp->fsys, GMEVENT_MoveLRGymSteel, egw);
	}else if (lift_type == BMID_LIFT_BASE02) {			//手前奥
		if (bit){		//奥へ
			ioSteelTemp->MoveDir = MOVE_DIR_REAR;
			ioSteelTemp->Dst = LiftInfo[inIdx].BeforePos.GridZ*ONE_GRID+STEEL_CENTER_OFS;
			ioSteelTemp->LiftPos[inIdx] = LiftInfo[inIdx].BeforePos;
		}else{			//手前へ
			ioSteelTemp->MoveDir = MOVE_DIR_FRONT;
			ioSteelTemp->Dst = LiftInfo[inIdx].AffterPos.GridZ*ONE_GRID+STEEL_CENTER_OFS;
			ioSteelTemp->LiftPos[inIdx] = LiftInfo[inIdx].AffterPos;
		}
		FieldEvent_Set(ioSteelTemp->fsys, GMEVENT_MoveFRGymSteel, egw);
	}else{				//上下
		if (bit){		//下へ
			ioSteelTemp->MoveDir = MOVE_DIR_DOWN;
			ioSteelTemp->Dst = LiftInfo[inIdx].BeforePos.Height*ONE_HEIGHT;
			ioSteelTemp->LiftPos[inIdx] = LiftInfo[inIdx].BeforePos;
			//高さコード更新	(下り時は先に行ってしまう)
			ioSteelTemp->LiftHeight[inIdx] =
							LiftInfo[inIdx].BeforeHeight;
		}else{			//上へ
			ioSteelTemp->MoveDir = MOVE_DIR_UP;
			ioSteelTemp->Dst = LiftInfo[inIdx].AffterPos.Height*ONE_HEIGHT;
			ioSteelTemp->LiftPos[inIdx] = LiftInfo[inIdx].AffterPos;
		}
		FieldEvent_Set(ioSteelTemp->fsys, GMEVENT_MoveUDGymSteel, egw);	
	}
}

//---------------------------------------------------------------------------
/**
 * @brief	鋼ジム:床のバニッシュ
 * 
 * @param	steel_temp		鋼ジムテンポラリワーク
 * @param	inHeight		高さ
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
static void SetLiftFloorVanish(STEEL_GYM_TEMP_WORK *steel_temp, const fx32 inHeight)
{
	M3DO_PTR obj_ptr2F;
	M3DO_PTR obj_ptr3F;
	M3DO_PTR obj_ptr4F;
	
	//高さに応じて床をバニッシュ
	obj_ptr2F = M3DO_GetMap3DObjData(steel_temp->fsys->Map3DObjExp, steel_temp->Floor2F);
	obj_ptr3F = M3DO_GetMap3DObjData(steel_temp->fsys->Map3DObjExp, steel_temp->Floor3F);
	obj_ptr4F = M3DO_GetMap3DObjData(steel_temp->fsys->Map3DObjExp, steel_temp->Floor4F);

	if (inHeight>=ONE_HEIGHT*(FLOOR_MARGIN-FLOOR_DISP_START_VAL)){	//2F
		M3DO_SetVanish(obj_ptr2F, FALSE);//表示
		//STEEL_FLOOR_2Fのリフトを表示
		InvisibleLift(steel_temp, FALSE, STEEL_FLOOR_2F);
	}else{
		M3DO_SetVanish(obj_ptr2F, TRUE);//非表示
		//STEEL_FLOOR_2Fのリフトを非表示
		InvisibleLift(steel_temp, TRUE, STEEL_FLOOR_2F);
	}
	
	if (inHeight>=ONE_HEIGHT*(FLOOR_MARGIN*2-FLOOR_DISP_START_VAL)){	//3F
		M3DO_SetVanish(obj_ptr3F, FALSE);//表示
		//STEEL_FLOOR_3Fのリフトを表示
		InvisibleLift(steel_temp, FALSE, STEEL_FLOOR_3F);
	}else{
		M3DO_SetVanish(obj_ptr3F, TRUE);//非表示
		//STEEL_FLOOR_3Fのリフトを表示
		InvisibleLift(steel_temp, TRUE, STEEL_FLOOR_3F);
	}
	
	if (inHeight>=ONE_HEIGHT*(FLOOR_MARGIN*3-FLOOR_DISP_START_VAL)){	//4F
		M3DO_SetVanish(obj_ptr4F, FALSE);//表示
		//STEEL_FLOOR_4Fのリフトを表示
		InvisibleLift(steel_temp, FALSE, STEEL_FLOOR_4F);
	}else{
		M3DO_SetVanish(obj_ptr4F, TRUE);//非表示
		//STEEL_FLOOR_4Fのリフトを表示
		InvisibleLift(steel_temp, TRUE, STEEL_FLOOR_4F);
	}
}

//---------------------------------------------------------------------------
/**
 * @brief	鋼ジム:リフトの上下移動
 * 
 * @param	event	イベントポインタ
 * 
 * @return	BOOL TRUE:イベント終了	FALSE:イベント継続
 */
//---------------------------------------------------------------------------
static BOOL GMEVENT_MoveUDGymSteel(GMEVENT_CONTROL * event)
{
	M3DO_PTR ptr;
	FIELDSYS_WORK * fsys = FieldEvent_GetFieldSysWork(event);
	EVENT_GYM_WORK * egw = FieldEvent_GetSpecialWork(event);
	STEEL_GYM_TEMP_WORK *steel_temp;

	steel_temp = (STEEL_GYM_TEMP_WORK *)fsys->fldmap->Work;
	
	switch (egw->seq) {
	case 0:
		Player_HeightGet_ON_OFF( fsys->player, FALSE );//自機の高さ自動取得をオフ

		//リフト音再生
		Snd_SePlay( SEQ_SE_DP_ELEBETA );
		(egw->seq)	++;
		break;
	case 1:	//リフトを移動
		{
			int entry;
			//リフトOBJを取得
			entry = steel_temp->LiftEntryIdx[steel_temp->ListIdx];
			ptr = M3DO_GetMap3DObjData(fsys->Map3DObjExp, entry);
			//y座標変更
			{
				VecFx32 vec;
				vec = M3DO_GetGlobalVec(ptr);
				if (steel_temp->MoveDir == MOVE_DIR_UP){
					vec.y += STEEL_UD_LIFT_DIF;
					if (vec.y >= steel_temp->Dst){	//セット終了
						vec.y = steel_temp->Dst;
						//高さコード更新
						steel_temp->LiftHeight[steel_temp->ListIdx] =
							LiftInfo[steel_temp->ListIdx].AffterHeight;
						//リフト音停止
						Snd_SeStopBySeqNo( SEQ_SE_DP_ELEBETA, 0 );
						(egw->seq) ++;
					}
				}else if (steel_temp->MoveDir == MOVE_DIR_DOWN){
					vec.y -= STEEL_UD_LIFT_DIF;
					if (vec.y <= steel_temp->Dst){	//セット終了
						vec.y = steel_temp->Dst;
						//リフト音停止
						Snd_SeStopBySeqNo( SEQ_SE_DP_ELEBETA, 0 );
						(egw->seq) ++;
					}
				}else{
					GF_ASSERT(0&&"方向指定エラー");
				}
				
				Player_VecPosYSet( fsys->player, vec.y );	//自機高さ変更
				M3DO_SetGlobalVec(ptr, &vec);

				SetLiftFloorVanish(steel_temp, vec.y);
			}
		}
		break;
	case 2:
		//自機の高さ自動取得をオン
		Player_HeightGetSet_ON_OFF( fsys->player, TRUE );
		//リフト移動終了音
		Snd_SePlay( SE_YOSUGA_GYM_LIFT );
		(egw->seq) ++;
		break;
	case 3:
		sys_FreeMemoryEz(egw);
		return TRUE;
	}
	return FALSE;
}

//---------------------------------------------------------------------------
/**
 * @brief	鋼ジム:リフトを左右に移動
 * 
 * @param	event	イベントポインタ
 * 
 * @return	BOOL TRUE:イベント終了	FALSE:イベント継続
 */
//---------------------------------------------------------------------------
static BOOL GMEVENT_MoveLRGymSteel(GMEVENT_CONTROL * event)
{
	M3DO_PTR ptr;
	FIELD_OBJ_PTR fop;
	FIELDSYS_WORK * fsys = FieldEvent_GetFieldSysWork(event);
	EVENT_GYM_WORK * egw = FieldEvent_GetSpecialWork(event);
	STEEL_GYM_TEMP_WORK *steel_temp;

	steel_temp = (STEEL_GYM_TEMP_WORK *)fsys->fldmap->Work;
	fop = Player_FieldOBJGet( fsys->player );	
	switch (egw->seq) {
	case 0:
		if( FieldOBJ_AcmdSetCheck(fop) == TRUE ){	//アニメセットできるか?
			FieldOBJ_AcmdSet(fop,AC_ANM_PAUSE_ON);	//出来る。動きをポーズ
			//リフト音再生
			Snd_SePlay( SEQ_SE_DP_ELEBETA );
			(egw->seq)	++;
		}
		break;
	case 1:	//リフトを移動
		{
			int entry;
			//リフトOBJを取得
			entry = steel_temp->LiftEntryIdx[steel_temp->ListIdx];
			ptr = M3DO_GetMap3DObjData(fsys->Map3DObjExp, entry);
			//x座標変更
			{
				VecFx32 vec,player_vec;
				vec = M3DO_GetGlobalVec(ptr);
				Player_VecPosGet( fsys->player, &player_vec );
				vec.x = player_vec.x;
				if (steel_temp->MoveDir == MOVE_DIR_RIGHT){
					if (vec.x >= steel_temp->Dst){	//セット終了
						vec.x = steel_temp->Dst;
						//リフト音停止
						Snd_SeStopBySeqNo( SEQ_SE_DP_ELEBETA, 0 );
						(egw->seq) ++;
					}else{
						if( FieldOBJ_AcmdSetCheck(fop) == TRUE ){	//アニメセットできるか?	
							FieldOBJ_AcmdSet(fop,AC_WALK_R_2F);	//自機右移動
						}
					}
				}else if (steel_temp->MoveDir == MOVE_DIR_LEFT){
					if (vec.x <= steel_temp->Dst){	//セット終了
						vec.x = steel_temp->Dst;
						//リフト音停止
						Snd_SeStopBySeqNo( SEQ_SE_DP_ELEBETA, 0 );
						(egw->seq) ++;
					}else{
						if( FieldOBJ_AcmdSetCheck(fop) == TRUE ){	//アニメセットできるか?	
							FieldOBJ_AcmdSet(fop,AC_WALK_L_2F);	//自機左移動
						}
					}
				}else{
					GF_ASSERT(0&&"方向指定エラー");
				}
				M3DO_SetGlobalVec(ptr, &vec);
			}
		}
		break;
	case 2:
		if( FieldOBJ_AcmdSetCheck(fop) == TRUE ){	//アニメセットできるか?
			FieldOBJ_AcmdSet(fop,AC_ANM_PAUSE_OFF);	//出来る。ポーズ解除
			(egw->seq) ++;
		}
	case 3:
		if( FieldOBJ_AcmdEndCheck(fop) == TRUE ){	//アニメ終了チェック
			FieldOBJ_AcmdEnd(fop); //アニメ終了
			//リフト移動終了音
			Snd_SePlay( SE_YOSUGA_GYM_LIFT );
			(egw->seq) ++;
		}
		break;
	case 4:
		sys_FreeMemoryEz(egw);
		return TRUE;
	}
	return FALSE;
}

//---------------------------------------------------------------------------
/**
 * @brief	鋼ジム:リフトを手前奥に移動
 * 
 * @param	event	イベントポインタ
 * 
 * @return	BOOL TRUE:イベント終了	FALSE:イベント継続
 */
//---------------------------------------------------------------------------
static BOOL GMEVENT_MoveFRGymSteel(GMEVENT_CONTROL * event)
{
	M3DO_PTR ptr;
	FIELD_OBJ_PTR fop;
	FIELDSYS_WORK * fsys = FieldEvent_GetFieldSysWork(event);
	EVENT_GYM_WORK * egw = FieldEvent_GetSpecialWork(event);
	STEEL_GYM_TEMP_WORK *steel_temp;

	steel_temp = (STEEL_GYM_TEMP_WORK *)fsys->fldmap->Work;
	fop = Player_FieldOBJGet( fsys->player );	
	switch (egw->seq) {
	case 0:
		if( FieldOBJ_AcmdSetCheck(fop) == TRUE ){	//アニメセットできるか?
			FieldOBJ_AcmdSet(fop,AC_ANM_PAUSE_ON);	//出来る。動きをポーズ
			//リフト音再生
			Snd_SePlay( SEQ_SE_DP_ELEBETA );
			(egw->seq)	++;
		}
		break;
	case 1:	//リフトを移動
		{
			int entry;
			//リフトOBJを取得
			entry = steel_temp->LiftEntryIdx[steel_temp->ListIdx];
			ptr = M3DO_GetMap3DObjData(fsys->Map3DObjExp, entry);
			//z座標変更
			{
				VecFx32 vec,player_vec;
				vec = M3DO_GetGlobalVec(ptr);
				Player_VecPosGet( fsys->player, &player_vec );
				vec.z = player_vec.z;
				if (steel_temp->MoveDir == MOVE_DIR_FRONT){
					if (vec.z >= steel_temp->Dst){	//セット終了
						vec.z = steel_temp->Dst;
						//リフト音停止
						Snd_SeStopBySeqNo( SEQ_SE_DP_ELEBETA, 0 );
						(egw->seq) ++;
					}else{
						if( FieldOBJ_AcmdSetCheck(fop) == TRUE ){	//アニメセットできるか?	
							FieldOBJ_AcmdSet(fop,AC_WALK_D_2F);	//自機手前移動
						}
					}
				}else if (steel_temp->MoveDir == MOVE_DIR_REAR){
					if (vec.z <= steel_temp->Dst){	//セット終了
						vec.z = steel_temp->Dst;
						//リフト音停止
						Snd_SeStopBySeqNo( SEQ_SE_DP_ELEBETA, 0 );
						(egw->seq) ++;
					}else{
						if( FieldOBJ_AcmdSetCheck(fop) == TRUE ){	//アニメセットできるか?	
							FieldOBJ_AcmdSet(fop,AC_WALK_U_2F);	//自機奥移動
						}
					}
				}else{
					GF_ASSERT(0&&"方向指定エラー");
				}
				M3DO_SetGlobalVec(ptr, &vec);
			}
		}
		break;
	case 2:
		if( FieldOBJ_AcmdSetCheck(fop) == TRUE ){	//アニメセットできるか?
			FieldOBJ_AcmdSet(fop,AC_ANM_PAUSE_OFF);	//出来る。ポーズ解除
			(egw->seq) ++;
		}
		break;
	case 3:
		{
			FIELD_OBJ_PTR fop;
			fop = Player_FieldOBJGet( fsys->player );
			if( FieldOBJ_AcmdEndCheck(fop) == TRUE ){	//アニメ終了チェック
				FieldOBJ_AcmdEnd(fop); //アニメ終了
				//リフト移動終了音
				Snd_SePlay( SE_YOSUGA_GYM_LIFT );
				(egw->seq) ++;
			}
		}
		break;
	case 4:
		sys_FreeMemoryEz(egw);
		return TRUE;
	}
	return FALSE;
}

//---------------------------------------------------------------------------
/**
 * @brief	鋼ジム:セットアップ
 * 
 * @param	fsys	フィールドシステムポインタ
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
void GYM_SetupSteelGym(FIELDSYS_WORK *fsys)
{
	STEEL_GYM_TEMP_WORK *steel_temp;
	GIMMICKWORK *work;
	STEEL_GYM_WORK *steel_gym_work;
	
	//ここでギミックワークの中身を見る
	work = SaveData_GetGimmickWork(GameSystem_GetSaveData(fsys));
	steel_gym_work = (STEEL_GYM_WORK*)GIMMICKWORK_Get(work, FLD_GIMMICK_STEEL_GYM);

	//鋼ジム専用ワークをアロケートして、フィールドマップ汎用ワークポインタにあてる
	fsys->fldmap->Work = sys_AllocMemory(HEAPID_FIELD, sizeof(STEEL_GYM_TEMP_WORK));
	steel_temp = fsys->fldmap->Work;

	steel_temp->fsys = fsys;
	//床（2F～4F）の作成
	{
		VecFx32 vec = {FX32_ONE*(16*16),0,FX32_ONE*(16*16)};
		vec.y = ONE_HEIGHT*FLOOR_MARGIN;
		steel_temp->Floor2F = M3DO_SetMap3DObjExp(fsys->Map3DObjExp,
							fsys->MapResource,
							BMID_GYM02_2F,
							&vec,NULL,
							fsys->field_3d_anime );
		vec.y = ONE_HEIGHT*FLOOR_MARGIN*2;
		steel_temp->Floor3F = M3DO_SetMap3DObjExp(fsys->Map3DObjExp,
							fsys->MapResource,
							BMID_GYM02_3F,
							&vec,NULL,
							fsys->field_3d_anime );
		vec.y = ONE_HEIGHT*FLOOR_MARGIN*3;
		steel_temp->Floor4F = M3DO_SetMap3DObjExp(fsys->Map3DObjExp,
							fsys->MapResource,
							BMID_GYM02_4F,
							&vec,NULL,
							fsys->field_3d_anime );
	}
	//リフトの作成(24個)
	{
		VecFx32 vec;
		int i;
		for(i=0;i<STEEL_LIFT_MAX;i++){
			if (/*GetLiftPosBit(steel_gym_work,i)*/GetBit(i, steel_gym_work->LiftPosBit)){
				vec.x = ONE_GRID*LiftInfo[i].AffterPos.GridX;
				vec.y = ONE_HEIGHT*LiftInfo[i].AffterPos.Height;
				vec.z = ONE_GRID*LiftInfo[i].AffterPos.GridZ;
				steel_temp->LiftPos[i] = LiftInfo[i].AffterPos;
				steel_temp->LiftHeight[i] = LiftInfo[i].AffterHeight;
			}else{
				vec.x = ONE_GRID*LiftInfo[i].BeforePos.GridX;
				vec.y = ONE_HEIGHT*LiftInfo[i].BeforePos.Height;
				vec.z = ONE_GRID*LiftInfo[i].BeforePos.GridZ;
				steel_temp->LiftPos[i] = LiftInfo[i].BeforePos;
				steel_temp->LiftHeight[i] = LiftInfo[i].BeforeHeight;
			}
			vec.x+=STEEL_CENTER_OFS;
			vec.z+=STEEL_CENTER_OFS;
			steel_temp->LiftEntryIdx[i] = 
				M3DO_SetMap3DObjExp(fsys->Map3DObjExp,
									fsys->MapResource,
									LiftInfo[i].ObjID,
									&vec, NULL,
									fsys->field_3d_anime );//<リフト

			steel_temp->ObjID[i] = LiftInfo[i].ObjID;
		}
	}

	//バニッシュセット
	{
		VecFx32 player_vec;
		//自機の高さを取得
		Player_VecPosGet( fsys->player, &player_vec );
		SetLiftFloorVanish(steel_temp, player_vec.y);
	}
	
	//クリップセット
	GFC_SetCameraClip(FX32_ONE * 100,FX32_ONE * 1700, fsys->camera_ptr);
	
	//フォグセット
	SetFogData(fsys->fog_data, FOG_SYS_ALL,
			TRUE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x0200, STEEL_FOG_OFS);
	
	SetFogColor(fsys->fog_data, FOG_SYS_ALL, GX_RGB(0,0,0), 0);
	{
		int i;
		char fog_tbl[32];
		for(i=0;i<32;i++){
			fog_tbl[i] = i*(STEEL_FOG_MAX/32);
		}
		SetFogTbl(fsys->fog_data, fog_tbl);
	}

}

//---------------------------------------------------------------------------
/**
 * @brief	鋼ジム:リフトチェック
 * 
 * @param	fsys	フィールドシステムポインタ
 * 
 * @return	BOOL	TRUE:リフトあり		FALSE:リフト無し
 */
//---------------------------------------------------------------------------
BOOL GYM_CheckSteelLift(FIELDSYS_WORK *fsys)
{
	//リフトヒットチェック
	u8 idx;
	STEEL_GYM_TEMP_WORK *steel_temp;

	//ギミックワークを見る
	{
		int id;
		GIMMICKWORK *work;
		
		//ギミックワーク取得
		work = SaveData_GetGimmickWork(GameSystem_GetSaveData(fsys));
		//ギミックコードを取得
		id = GIMMICKWORK_GetAssignID(work);

		if (id != FLD_GIMMICK_STEEL_GYM){
			return FALSE;					//鋼ジムギミック無し
		}
	}

	steel_temp = (STEEL_GYM_TEMP_WORK *)fsys->fldmap->Work;
	if (steel_temp == NULL){
		return FALSE;
	}
	idx = CheckLiftHit(steel_temp);
	if (idx != STEEL_LIFT_MAX){
		//ヒットしたので処理
		//リフトの位置とリフトタイプで、イベント分岐
		CallLiftMoveEvent(steel_temp, idx);
		return TRUE;
	}
	
	return FALSE;
}

//---------------------------------------------------------------------------
/**
 * @brief	鋼ジム:終了処理
 * 
 * @param	fsys	フィールドシステムポインタ
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
void GYM_EndSteelGym(FIELDSYS_WORK *fsys)
{
	STEEL_GYM_TEMP_WORK *steel_temp;
	steel_temp = (STEEL_GYM_TEMP_WORK *)fsys->fldmap->Work;
	sys_FreeMemoryEz(steel_temp);
	fsys->fldmap->Work = NULL;
}

//---------------------------------------------------------------------------
/**
 * @brief	鋼ジム:当たり判定
 * 
 * @param	fsys		フィールドシステムポインタ
 * @param	inGridX		グリッドX座標
 * @param	inGridZ		グリッドZ座標
 * @param	inHeight	高さ
 * @param	outHit		判定結果	TRUE:ヒット	FALSE:ヒットしてない
 * 
 * @return	BOOL		TRUE:このあと通常当たり判定を行わない	FALSE:通常当たり判定を行う
 */
//---------------------------------------------------------------------------
BOOL GYM_HitCheckSteelGym(	FIELDSYS_WORK *fsys,
							const int inGridX, const int inGridZ,
							const fx32 inHeight, BOOL *outHit	)
{
	u8 floor;
	const u8 *tbl;
	int index;
	//階層を決定
	floor = (inHeight/ONE_HEIGHT)/FLOOR_MARGIN;
	GF_ASSERT(floor<=3&&"階層エラー");
	
	tbl = SteelGymHitTbl[floor];
	index = inGridX+inGridZ*32;
	GF_ASSERT(index<1024&&"インデックスエラー");
	(*outHit) = tbl[index];

	return TRUE;	//通常当たり判定は行わない
}

//////////////////////////////////////////////////////////////////////////////////////////
//----
#ifndef DP_OLDGYM_NULL_FIGHT
//----

//格闘ジム
typedef struct COMBAT_HIT_RECT_tag
{
	u8 X;
	u8 W;
}COMBAT_HIT_RECT;

typedef struct COMBAT_GYM_TEMP_WORK_tag
{
	FIELDSYS_WORK *fsys;
	u8 WallEntryIdx[COMBAT_WALL_MAX];		//拡張配置モデル登録インデックス
///	int ObjID[COMBAT_WALL_MAX];
	u8 WallPos[COMBAT_WALL_MAX];
	u8 ListIdx;
	fx32 Dst;
	MOVE_DIR MoveDir;
	const COMBAT_HIT_RECT	* HitRect[COMBAT_WALL_MAX];
}COMBAT_GYM_TEMP_WORK;

typedef struct COMBAT_WALL_INFO_tag
{
	int ObjID;
	u8 BaseZ;		//配置されるZ座標
	u8 BeforeX;		//移動前X座標(左時)
	u8 AffterX;		//移動後X座標（右時）
	u8 dummy;	
}COMBAT_WALL_INFO;

typedef struct COMBAT_WALL_POS_INFO_tag
{
	u8 GridX;	//X座標
	u8 GridZ;	//Z座標
	u8 Bit;		//発動条件	0:左から押せる　1右から押せる
	u8 WallNo;	//しきり番号
}COMBAT_WALL_POS_INFO;

typedef struct COMBAT_WALL_HIT_tag
{
	COMBAT_HIT_RECT BeforeRect[2];
	COMBAT_HIT_RECT AffterRect[2];
}COMBAT_WALL_HIT;

//しきり情報
static const COMBAT_WALL_INFO WallInfo[COMBAT_WALL_MAX] = {
	{ BMID_GYM_WALL06,  9,  9, 10, 0 },
	{ BMID_GYM_WALL02, 10,  3,  4, 0 },
	{ BMID_GYM_WALL02, 10, 11, 12, 1 },
	{ BMID_GYM_WALL04, 10, 17, 18, 1 },
	{ BMID_GYM_WALL05, 12,  1,  5, 1 },
	{ BMID_GYM_WALL01, 13, 16, 20, 0 },
	{ BMID_GYM_WALL07, 14, 19, 20, 0 },
	{ BMID_GYM_WALL06, 16,  4,  7, 1 },
	{ BMID_GYM_WALL06, 16, 16, 17, 0 },
	{ BMID_GYM_WALL05, 18,  1,  5, 1 },
	{ BMID_GYM_WALL07, 19, 19, 20, 0 },
	{ BMID_GYM_WALL03, 20,  3,  8, 1 },
};

//しきり押し場所情報
static const COMBAT_WALL_POS_INFO WallPosInfo[COMBAT_WALL_POS_MAX] = {
	{12, 20, 1, WALLNO_12},
	{ 2, 20, 0, WALLNO_12},
	{18, 19, 0, WALLNO_11},
	{ 9, 18, 1, WALLNO_10},
	{21, 16, 1, WALLNO_9},
	{15, 16, 0, WALLNO_9},
	{11, 16, 1, WALLNO_8},
	{18, 14, 0, WALLNO_7},
	{15, 13, 0, WALLNO_6},
	{ 9, 12, 1, WALLNO_5},
	{22, 10, 1, WALLNO_4},
	{16, 10, 1, WALLNO_3},
	{ 2, 10, 0, WALLNO_2},
	{ 8,  9, 0, WALLNO_1},
};

//しきり当たり判定
static const COMBAT_WALL_HIT WallHitInfo[COMBAT_WALL_MAX] = {
	{ {{ 9, 2}, {12, 1}}, {{10, 2}, {13, 1}} },
	{ {{ 3, 1}, { 5, 2}}, {{ 4, 1}, { 6, 2}} },
	{ {{11, 1}, {13, 2}}, {{12, 1}, {14, 2}} },
	{ {{18, 1}, {20, 1}}, {{19, 1}, {21, 1}} },
	{ {{ 4, 1}, { 0, 0}}, {{ 5, 4}, { 0, 0}} },
	{ {{16, 4}, { 0, 0}}, {{20, 1}, { 0, 0}} },
	{ {{19, 4}, { 0, 0}}, {{22, 1}, { 0, 0}} },
	{ {{ 4, 2}, { 7, 1}}, {{ 7, 2}, {10, 1}} },
	{ {{16, 2}, {19, 1}}, {{17, 2}, {20, 1}} },
	{ {{ 4, 1}, { 0, 0}}, {{ 5, 4}, { 0, 0}} },
	{ {{19, 4}, { 0, 0}}, {{22, 1}, { 0, 0}} },
	{ {{ 3, 4}, { 0, 0}}, {{ 8, 4}, { 0, 0}} },
};

//---------------------------------------------------------------------------
/**
 * @brief	格闘ジム:しきりプッシュ判定
 * 
 * @param	inCombatTemp		格闘ジムテンポラリワーク
 * @param	inDir				方向
 * 
 * @return	u8					ヒットしたリフトインデックス
 */
//---------------------------------------------------------------------------
static u8 CheckWallPush(COMBAT_GYM_TEMP_WORK *inCombatTemp, const u8 inDir)
{
	u8 i;
	u8 dir;
	u8 check_x;
	u8 grid_x;
	u8 grid_z;
	VecFx32 vec;
	Player_VecPosGet( inCombatTemp->fsys->player, &vec );
	//自機のグリッド座標を取得
	grid_x = vec.x/ONE_GRID;
	grid_z = vec.z/ONE_GRID;
	
	for(i=0;i<COMBAT_WALL_POS_MAX;i++){
		//XZ比較
		if ( (WallPosInfo[i].GridX == grid_x) &&
				(WallPosInfo[i].GridZ == grid_z) ){
			u8 no;
			no = WallPosInfo[i].WallNo;
			if (WallPosInfo[i].Bit){	//右にある
				check_x = WallInfo[no].AffterX;
				dir = DIR_LEFT;
			}else{						//左にある
				check_x = WallInfo[no].BeforeX;
				dir = DIR_RIGHT;
			}

			if ( (check_x == inCombatTemp->WallPos[no])&&
					(dir == inDir) ){
				return no;
			}
		}
	}
	return COMBAT_WALL_POS_MAX;
}

//---------------------------------------------------------------------------
/**
 * @brief	格闘ジム:しきり移動イベントコール
 * 
 * @param	ioCombatTemp	格闘ジムテンポラリワーク
 * @param	inIdx			リフトインデックス
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
static void CallWallMoveEvent(COMBAT_GYM_TEMP_WORK *ioCombatTemp, const u8 inIdx)
{
	int lift_type;
	u8 bit;
	EVENT_GYM_WORK * egw;
	
	GIMMICKWORK *work;
	COMBAT_GYM_WORK *combat_gym_work;
	work = SaveData_GetGimmickWork(GameSystem_GetSaveData(ioCombatTemp->fsys));
	combat_gym_work = (COMBAT_GYM_WORK*)GIMMICKWORK_Get(work, FLD_GIMMICK_COMBAT_GYM);
	
	egw = sys_AllocMemoryLo(HEAPID_WORLD, sizeof(EVENT_GYM_WORK));
	egw->seq = 0;

	ioCombatTemp->ListIdx = inIdx;
	
	//対象リフトの位置を取得
	bit = GetBit(inIdx, combat_gym_work->WallPosBit);
	
	ChangeBit(inIdx,&combat_gym_work->WallPosBit);
	//移動イベントをコール
	
	if (bit){		//今右にある
		ioCombatTemp->MoveDir = MOVE_DIR_LEFT;
		ioCombatTemp->Dst = WallInfo[inIdx].BeforeX*ONE_GRID+COMBAT_CENTER_OFS_X;
		ioCombatTemp->WallPos[inIdx] = WallInfo[inIdx].BeforeX;
		ioCombatTemp->HitRect[inIdx] = &(WallHitInfo[inIdx].BeforeRect[0]);
	}else{			//今左にある
		ioCombatTemp->MoveDir = MOVE_DIR_RIGHT;
		ioCombatTemp->Dst = WallInfo[inIdx].AffterX*ONE_GRID+COMBAT_CENTER_OFS_X;
		ioCombatTemp->WallPos[inIdx] = WallInfo[inIdx].AffterX;
		ioCombatTemp->HitRect[inIdx] = &(WallHitInfo[inIdx].AffterRect[0]);
	}
	FieldEvent_Set(ioCombatTemp->fsys, GMEVENT_MoveWallGymCombat, egw);
}

//---------------------------------------------------------------------------
/**
 * @brief	格闘ジム:セットアップ
 * 
 * @param	fsys	フィールドシステムポインタ
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
void GYM_SetupCombatGym(FIELDSYS_WORK *fsys)
{
	COMBAT_GYM_TEMP_WORK *combat_temp;
	GIMMICKWORK *work;
	COMBAT_GYM_WORK *combat_gym_work;
	
	//ここでギミックワークの中身を見る
	work = SaveData_GetGimmickWork(GameSystem_GetSaveData(fsys));
	combat_gym_work = (COMBAT_GYM_WORK*)GIMMICKWORK_Get(work, FLD_GIMMICK_COMBAT_GYM);

	//格闘ジム専用ワークをアロケートして、フィールドマップ汎用ワークポインタにあてる
	fsys->fldmap->Work = sys_AllocMemory(HEAPID_FIELD, sizeof(COMBAT_GYM_TEMP_WORK));
	combat_temp = fsys->fldmap->Work;

	combat_temp->fsys = fsys;
	
	//しきりの作成(12個)
	{
		VecFx32 vec;
		int i;
		for(i=0;i<COMBAT_WALL_MAX;i++){
			if (GetBit(i,combat_gym_work->WallPosBit)){
				vec.x = ONE_GRID*WallInfo[i].AffterX;
				vec.y = -ONE_GRID;
				vec.z = ONE_GRID*WallInfo[i].BaseZ;
				combat_temp->WallPos[i] = WallInfo[i].AffterX;
				combat_temp->HitRect[i] = &(WallHitInfo[i].AffterRect[0]);
			}else{
				vec.x = ONE_GRID*WallInfo[i].BeforeX;
				vec.y = -ONE_GRID;
				vec.z = ONE_GRID*WallInfo[i].BaseZ;
				combat_temp->WallPos[i] = WallInfo[i].BeforeX;
				combat_temp->HitRect[i] = &(WallHitInfo[i].BeforeRect[0]);
			}
			vec.x+=COMBAT_CENTER_OFS_X;
			vec.z+=COMBAT_CENTER_OFS_Z;
			combat_temp->WallEntryIdx[i] = 
				M3DO_SetMap3DObjExp(fsys->Map3DObjExp,
									fsys->MapResource,
									WallInfo[i].ObjID,
									&vec, NULL,
									fsys->field_3d_anime );//<しきり

			///combat_temp->ObjID[i] = WallInfo[i].ObjID;
		}
	}
}

//---------------------------------------------------------------------------
/**
 * @brief	格闘ジム:終了処理
 * 
 * @param	fsys	フィールドシステムポインタ
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
void GYM_EndCombatGym(FIELDSYS_WORK *fsys)
{
	COMBAT_GYM_TEMP_WORK *combat_temp;
	combat_temp = (COMBAT_GYM_TEMP_WORK *)fsys->fldmap->Work;
	sys_FreeMemoryEz(combat_temp);
	fsys->fldmap->Work = NULL;
}

//---------------------------------------------------------------------------
/**
 * @brief	格闘ジム:しきりチェック
 * 
 * @param	fsys	フィールドシステムポインタ
 * 
 * @return	BOOL	TRUE:HIT		FALSE:NO_HIT
 */
//---------------------------------------------------------------------------
BOOL GYM_CheckCombatWall(FIELDSYS_WORK *fsys)
{
	u8 idx;
	u8 dir;
	COMBAT_GYM_TEMP_WORK *combat_temp;

	//ギミックワークを見る
	{
		int id;
		GIMMICKWORK *work;
		
		//ギミックワーク取得
		work = SaveData_GetGimmickWork(GameSystem_GetSaveData(fsys));
		//ギミックコードを取得
		id = GIMMICKWORK_GetAssignID(work);

		if (id != FLD_GIMMICK_COMBAT_GYM){
			return FALSE;					//格闘ジムギミック無し
		}
	}

	combat_temp = (COMBAT_GYM_TEMP_WORK *)fsys->fldmap->Work;
	if (combat_temp == NULL){
		return FALSE;
	}
	dir = Player_DirGet(fsys->player);
	idx = CheckWallPush(combat_temp, dir);
	if (idx != COMBAT_WALL_POS_MAX){
		//ヒットしたので処理
		CallWallMoveEvent(combat_temp, idx);
		return TRUE;
	}
	
	return FALSE;
}

//---------------------------------------------------------------------------
/**
 * @brief	格闘ジム:しきり当たり判定
 * 
 * @param	fsys		フィールドシステムポインタ
 * @param	inGridX		グリッドX座標
 * @param	inGridZ		グリッドZ座標
 * @param	inHeight	高さ
 * @param	outHit		判定結果	TRUE:ヒット	FALSE:ヒットしてない
 * 
 * @return	BOOL		TRUE:このあと通常当たり判定を行わない	FALSE:通常当たり判定を行う
 */
//---------------------------------------------------------------------------
BOOL GYM_HitCheckCombatGym(	FIELDSYS_WORK *fsys,
							const int inGridX, const int inGridZ,
							const fx32 inHeight, BOOL *outHit	)
{
	int i,j;
	COMBAT_GYM_TEMP_WORK *combat_temp;
	const COMBAT_HIT_RECT	*rect;
	combat_temp = (COMBAT_GYM_TEMP_WORK *)fsys->fldmap->Work;
	for(i=0;i<COMBAT_WALL_MAX;i++){
		if (WallInfo[i].BaseZ == inGridZ){
			rect = combat_temp->HitRect[i];
			for(j=0;j<2;j++){
				if (rect[j].W == 0){
					break;
				}
				if ( (rect[j].X<=inGridX)&&
						(inGridX < (rect[j].X+rect[j].W)) ){
					(*outHit) = TRUE;
					return TRUE;	//HITしたので、通常当たり判定はパス
				}
			}
		}
	}
	return FALSE;	//通常当たり判定を実行
}

//---------------------------------------------------------------------------
/**
 * @brief	格闘ジム:しきりの移動
 * 
 * @param	event	イベントポインタ
 * 
 * @return	BOOL TRUE:イベント終了	FALSE:イベント継続
 */
//---------------------------------------------------------------------------
static BOOL GMEVENT_MoveWallGymCombat(GMEVENT_CONTROL * event)
{
	M3DO_PTR ptr;
	FIELD_OBJ_PTR fop;
	FIELDSYS_WORK * fsys = FieldEvent_GetFieldSysWork(event);
	EVENT_GYM_WORK * egw = FieldEvent_GetSpecialWork(event);
	COMBAT_GYM_TEMP_WORK *combat_temp;

	combat_temp = (COMBAT_GYM_TEMP_WORK *)fsys->fldmap->Work;
	fop = Player_FieldOBJGet( fsys->player );
	switch (egw->seq) {
	case 0:
		//しきりを押す音
		Snd_SePlay( SE_TOBARI_GYM_MOVE );
		//自機の移動(壁になっていて、グリッドのズレが生じないため、自機の移動アニメ終了を待たずにアニメ登録)
		if (combat_temp->MoveDir == MOVE_DIR_RIGHT){
			FieldOBJ_AcmdSet(fop,AC_WALK_R_8F);	//自機右移動
		}else{
			FieldOBJ_AcmdSet(fop,AC_WALK_L_8F);	//自機左移動
		}
		(egw->seq)	++;		
		break;
	case 1:	//しきりを移動
		{
			int entry;
			//リフトOBJを取得
			entry = combat_temp->WallEntryIdx[combat_temp->ListIdx];
			ptr = M3DO_GetMap3DObjData(fsys->Map3DObjExp, entry);
			//x座標変更
			{
				VecFx32 vec,player_vec;
				vec = M3DO_GetGlobalVec(ptr);
				
				if (combat_temp->MoveDir == MOVE_DIR_RIGHT){
					vec.x += (FX32_ONE*2);
					if (vec.x >= combat_temp->Dst){	//セット終了
						vec.x = combat_temp->Dst;
						(egw->seq) ++;
					}
				}else if (combat_temp->MoveDir == MOVE_DIR_LEFT){
					vec.x -= (FX32_ONE*2);
					if (vec.x <= combat_temp->Dst){	//セット終了
						vec.x = combat_temp->Dst;
						(egw->seq) ++;
					}
				}else{
					GF_ASSERT(0&&"方向指定エラー");
				}
				M3DO_SetGlobalVec(ptr, &vec);
			}
		}
		break;
	case 2:
		{
			FIELD_OBJ_PTR fop;
			fop = Player_FieldOBJGet( fsys->player );
			if( FieldOBJ_AcmdEndCheck(fop) == TRUE ){	//アニメ終了チェック
				FieldOBJ_AcmdEnd(fop); //アニメ終了
				//しきり動く音止める
				Snd_SeStopBySeqNo( SE_TOBARI_GYM_MOVE, 0 );
				(egw->seq) ++;
			}
		}
		break;
	case 3:
		sys_FreeMemoryEz(egw);
		return TRUE;
	}
	return FALSE;
}
//----
#endif	//#ifndef DP_OLDGYM_NULL_FIGHT
//----

//////////////////////////////////////////////////////////////////////////////////////////

//電気ジム

typedef struct ELEC_GYM_TEMP_WORK_tag
{
	FIELDSYS_WORK *fsys;
	u8 ObjEntryIdx[ELEC_GEAR_MAX];
	u8 RoomNo;
	u8 GearNum;
	u8 RotateCode;
	
	u16 RotVal;			//回転度合いタイプ
	u16 NowRotate;		//現在回転値
	u16 DstRotate;		//目的回転値
}ELEC_GYM_TEMP_WORK;

typedef struct GEAR_tag
{
	int ObjID;		//ギアＯＢＪＩＤ
	u8 X;
	u8 Y;
	u8 Z;
	u8 RotateCode:2;	//回転状況
	u8 RotateDir:1;	//ギア回転方向
	u8 RotType:1;	//ギア回転タイプ
	u8 dummy:4;
}GEAR;

typedef struct GEAR_HIT_RECT_tag
{
	u8 X;
	u8 Z;
	u8 W;
	u8 H;
}GEAR_HIT_RECT;


static const GEAR	Room1Gear[ELEC_GEAR_MAX1] = 
{
	{ BMID_GYM_GEAR02,	  3,  0,  8, GEAR_ROT_ST_90,  GEAR_ROT_DIR_LEFT,  GEAR_ROT_TYPE_Y },
	{ BMID_GYM_GEAR02_2,  8,  0,  8, GEAR_ROT_ST_180, GEAR_ROT_DIR_RIGHT, GEAR_ROT_TYPE_Y },
	{ BMID_GYM_GEAR02,   13,  0,  8, GEAR_ROT_ST_0,	  GEAR_ROT_DIR_LEFT,  GEAR_ROT_TYPE_Y },
};

static const GEAR	Room2Gear[ELEC_GEAR_MAX2] = 
{
	{ BMID_GYM_GEAR02,	  6,  0,  8, GEAR_ROT_ST_90,  GEAR_ROT_DIR_LEFT,  GEAR_ROT_TYPE_Y },
	{ BMID_GYM_GEAR01_2, 11,  0,  8, GEAR_ROT_ST_270, GEAR_ROT_DIR_RIGHT, GEAR_ROT_TYPE_Y },
	{ BMID_GYM_GEAR04,   15,  3,  8, GEAR_ROT_ST_90,  GEAR_ROT_DIR_RIGHT,GEAR_ROT_TYPE_X },
	{ BMID_GYM_GEAR04,	  2,  3, 13, GEAR_ROT_ST_90,  GEAR_ROT_DIR_LEFT,  GEAR_ROT_TYPE_X },
	{ BMID_GYM_GEAR01_2,  6,  0, 13, GEAR_ROT_ST_180, GEAR_ROT_DIR_RIGHT, GEAR_ROT_TYPE_Y },
	{ BMID_GYM_GEAR03,   11,  0, 13, GEAR_ROT_ST_180, GEAR_ROT_DIR_LEFT,  GEAR_ROT_TYPE_Y },
};

static const GEAR	Room3Gear[ELEC_GEAR_MAX3] = 
{
	{ BMID_GYM_GEAR03,	  6,  6,  8, GEAR_ROT_ST_90,  GEAR_ROT_DIR_LEFT,  GEAR_ROT_TYPE_Y },
	{ BMID_GYM_GEAR02_2, 11,  6,  8, GEAR_ROT_ST_0,   GEAR_ROT_DIR_RIGHT, GEAR_ROT_TYPE_Y },
	{ BMID_GYM_GEAR03_2, 16,  6,  8, GEAR_ROT_ST_0,	  GEAR_ROT_DIR_LEFT,  GEAR_ROT_TYPE_Y },
	{ BMID_GYM_GEAR04,	  2,  3, 13, GEAR_ROT_ST_0,   GEAR_ROT_DIR_RIGHT, GEAR_ROT_TYPE_X },
	{ BMID_GYM_GEAR02_2,  6,  6, 13, GEAR_ROT_ST_90,  GEAR_ROT_DIR_RIGHT, GEAR_ROT_TYPE_Y },
	{ BMID_GYM_GEAR03,   11,  6, 13, GEAR_ROT_ST_270, GEAR_ROT_DIR_LEFT,  GEAR_ROT_TYPE_Y },
	{ BMID_GYM_GEAR03_2, 16,  6, 13, GEAR_ROT_ST_270, GEAR_ROT_DIR_RIGHT, GEAR_ROT_TYPE_Y },
	{ BMID_GYM_GEAR04,   20,  3, 13, GEAR_ROT_ST_90,  GEAR_ROT_DIR_LEFT,  GEAR_ROT_TYPE_X },
	{ BMID_GYM_GEAR04_2,  2,  3, 18, GEAR_ROT_ST_0,	  GEAR_ROT_DIR_LEFT,  GEAR_ROT_TYPE_X },
	{ BMID_GYM_GEAR02,	  6,  0, 18, GEAR_ROT_ST_90,  GEAR_ROT_DIR_RIGHT, GEAR_ROT_TYPE_Y },
	{ BMID_GYM_GEAR03_2, 11,  0, 18, GEAR_ROT_ST_270, GEAR_ROT_DIR_LEFT,  GEAR_ROT_TYPE_Y },
	{ BMID_GYM_GEAR03,   16,  0, 18, GEAR_ROT_ST_0,	  GEAR_ROT_DIR_RIGHT, GEAR_ROT_TYPE_Y },
	{ BMID_GYM_GEAR04_2, 20,  3, 18, GEAR_ROT_ST_90,  GEAR_ROT_DIR_RIGHT, GEAR_ROT_TYPE_X },
};

static const GEAR * const RoomGear[ELEC_ROOM_MAX] = {
	Room1Gear, Room2Gear, Room3Gear
};

static const GEAR_HIT_RECT Room1GearHit[4*ELEC_GEAR_NUM1_YROT] =
{
	{ 1,8,2,1}, { 3,6,1,2}, { 4,8,2,1}, { 3,9,1,2},	//左のギア
	{ 6,8,2,1}, { 8,6,1,2}, { 9,8,2,1}, { 8,9,1,2},	//真ん中のギア
	{11,8,2,1}, {13,6,1,2}, {14,8,2,1}, {13,9,1,2}	//右のギア
};
static const GEAR_HIT_RECT Room2GearHit[4*ELEC_GEAR_NUM2_YROT+ELEC_GEAR_NUM2_XROT] =
{
	{ 4, 8,2,1}, { 6, 6,1,2}, { 7, 8,2,1}, { 6, 9,1,2},	//奥左のギア
	{ 9, 8,2,1}, {11, 6,1,2}, {12, 8,2,1}, {11, 9,1,2},	//奥真ん中のギア
	{15, 6,1,5},										//奥右（縦）のギア
	{ 2,11,1,5},										//手前左（縦）のギア
	{ 4,13,2,1}, { 6,11,1,2}, { 7,13,2,1}, { 6,14,1,2},	//手前真ん中のギア
	{ 9,13,2,1}, {11,11,1,2}, {12,13,2,1}, {11,14,1,2}	//手前右のギア
};
static const GEAR_HIT_RECT Room3GearHit[4*ELEC_GEAR_NUM3_YROT+ELEC_GEAR_NUM3_XROT] =
{
	{ 4,8,2,1}, { 6, 6,1,2}, { 7,8,2,1}, { 6,9,1,2},	//上段奥左のギア
	{ 9,8,2,1}, {11, 6,1,2}, {12,8,2,1}, {11,9,1,2},	//上段奥真ん中のギア
	{14,8,2,1}, {16, 6,1,2}, {17,8,2,1}, {16,9,1,2},	//上段奥右のギア
	{ 2,11,1,5},
	{ 4,13,2,1}, { 6,11,1,2}, { 7,13,2,1}, { 6,14,1,2},	//上段手前左のギア
	{ 9,13,2,1}, {11,11,1,2}, {12,13,2,1}, {11,14,1,2},	//上段手前真ん中のギア
	{14,13,2,1}, {16,11,1,2}, {17,13,2,1}, {16,14,1,2},	//上段手前右のギア
	{20,11,1,5},
	{ 2,16,1,5},
	{ 4,18,2,1}, { 6,16,1,2}, { 7,18,2,1}, { 6,19,1,2},	//下段左のギア
	{ 9,18,2,1}, {11,16,1,2}, {12,18,2,1}, {11,19,1,2},	//下段真ん中のギア
	{14,18,2,1}, {16,16,1,2}, {17,18,2,1}, {16,19,1,2},	//下段右のギア
	{20,16,1,5},
};

static const GEAR_HIT_RECT * const RoomGearHit[ELEC_ROOM_MAX] = {
	Room1GearHit,Room2GearHit,Room3GearHit
};

//ギア当たり判定

//1部屋目
static const u8 Room1Rot0Hit[] = { 1,2,4,5,10,11 };
static const u8 Room1Rot90Hit[] = { 0,1,5,6, 9,10 };
static const u8 Room1Rot180Hit[] = { 0,3,6,7, 8, 9 };
static const u8 Room1Rot270Hit[] = {  2,3,4,7, 8,11 };
//2部屋目
static const u8 Room2Rot0Hit[] =  { 1,2,4,5,7,8,9,10,11,12,15 };
static const u8 Room2Rot90Hit[] = { 0,1,4,5,6,11,12,13,14,0,0 };//後ろ2つはダミー（すでにエントリされている番号で埋めとく）
static const u8 Room2Rot180Hit[] = { 0,3,5,6,7,8, 9,10,12,13,17 };
static const u8 Room2Rot270Hit[] = { 2,3,4,6,7,10,11,13,16,2,2 };//後ろ2つはダミー（すでにエントリされている番号で埋めとく）
//3部屋目
static const u8 Room3Rot0Hit[] =  { 2,6,7,11,14,15,17,21,25,28,29,31,38,39 };
static const u8 Room3Rot90Hit[] = { 1,4,7,10,12,15,16,20,22,26,29,30,34,35 };
static const u8 Room3Rot180Hit[] = { 0,4,5,9,13,16,19,23,25,27,30,33,36,39 };
static const u8 Room3Rot270Hit[] = { 3,5,6,8,12,13,14,18,24,26,27,28,32,37 };

typedef struct GEAR_HIT_INFO_tag
{
	int HitNum;	//当たり矩形数
	u8 const *HitList[4];	//0,90,180,270°のときの当たり判定テーブル
}GEAR_HIT_INFO;

GEAR_HIT_INFO GearHitInfo[ELEC_ROOM_MAX] = 
{
	{ 6,{Room1Rot0Hit, Room1Rot90Hit, Room1Rot180Hit, Room1Rot270Hit} },
	{ 11,{Room2Rot0Hit, Room2Rot90Hit, Room2Rot180Hit, Room2Rot270Hit} },
	{ 14,{Room3Rot0Hit, Room3Rot90Hit, Room3Rot180Hit, Room3Rot270Hit} },
};

//---------------------------------------------------------------------------
/**
 * @brief	電気ジム:部屋に応じたギア個数を返す
 * 
 * @param	inRoomNo	部屋ナンバー
 * 
 * @return	u8			ギア個数
 */
//---------------------------------------------------------------------------
static u8 GetGearNum(const u8 inRoomNo)
{
	u8 num;
	switch (inRoomNo){
	case ELEC_GYM_ROOM1:
		num = ELEC_GEAR_MAX1;
		break;
	case ELEC_GYM_ROOM2:
		num = ELEC_GEAR_MAX2;
		break;
	case ELEC_GYM_ROOM3:
		num = ELEC_GEAR_MAX3;
		break;
	default:
		GF_ASSERT(0);
		num = 0;
	}
	return num;
}

//---------------------------------------------------------------------------
/**
 * @brief	電気ジム:ギアの初期回転計算
 *
 * @param	inGear			ギアデータ
 * @param	inRoomRotate	ギミック発動による追加回転
 * @param	outRotate		回転ベクトル格納バッファ
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
static void GetStartRotate(const GEAR *inGear , const u8 inRoomRotate, VecFx32 *outRotate)
{
	fx32 *target = NULL;
	fx16 rot,room_rot;

	//足しこみ対象決定
	switch (inGear->RotType){
	case GEAR_ROT_TYPE_Y:
		target = &(outRotate->y);
		break;
	case GEAR_ROT_TYPE_X:
		target = &(outRotate->x);
		break;
	default:
		GF_ASSERT(0&&"ERROR:RotType unknown");
	}

	rot = 0;

	//ギアごとの初期回転をベクトルに足しこむ
	GF_ASSERT(inGear->RotateCode<=GEAR_ROT_ST_270&&"ERROR:RotState unknown");
	rot += (0x4000*inGear->RotateCode);

	GF_ASSERT(inRoomRotate<=GEAR_ROT_ST_270&&"ERROR:RotState unknown");
	//ボタンによる回転をギアに足し込む(回転方向によっては差し引く)
	room_rot = 0x4000*inRoomRotate;
	if (inGear->RotateDir == GEAR_ROT_DIR_LEFT){
		rot += room_rot;
	}else if(inGear->RotateDir == GEAR_ROT_DIR_RIGHT){
		rot -= room_rot;
	}else{
		GF_ASSERT(0&&"ERROR:RotDir unknown");;
	}

	(*target) = rot;
}

//---------------------------------------------------------------------------
/**
 * @brief	電気ジム:セットアップ
 * 
 * @param	fsys	フィールドシステムポインタ
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
void GYM_SetupElecGym(FIELDSYS_WORK *fsys)
{
	ELEC_GYM_TEMP_WORK *elec_temp;
	GIMMICKWORK *work;
	ELEC_GYM_WORK *elec_gym_work;
	
	//ここでギミックワークの中身を見る
	work = SaveData_GetGimmickWork(GameSystem_GetSaveData(fsys));
	elec_gym_work = (ELEC_GYM_WORK*)GIMMICKWORK_Get(work, FLD_GIMMICK_ELEC_GYM);

	//電気ジム専用ワークをアロケートして、フィールドマップ汎用ワークポインタにあてる
	fsys->fldmap->Work = sys_AllocMemory(HEAPID_FIELD, sizeof(ELEC_GYM_TEMP_WORK));
	elec_temp = fsys->fldmap->Work;

	elec_temp->RoomNo = elec_gym_work->RoomNo;
	elec_temp->RotateCode = elec_gym_work->GearRotateCode;
///	elec_temp->fsys = fsys;
	
	//部屋ごとにギアを作成
	{
		VecFx32 vec;
		VecFx32 rotate;
		int i;
		u8 gear_num;

		gear_num = GetGearNum(elec_gym_work->RoomNo);

		for(i=0;i<gear_num;i++){
			const GEAR *room_gear;
			room_gear = RoomGear[elec_gym_work->RoomNo];
			vec.x = room_gear[i].X * ONE_GRID;
			vec.y = room_gear[i].Y * ONE_GRID;
			vec.z = room_gear[i].Z * ONE_GRID;
			
			vec.x+=ELEC_GEAR_CENTER_OFS_X;
			vec.z+=ELEC_GEAR_CENTER_OFS_Z;

			//Ｘ軸回転なら補正をかける
			if (room_gear[i].RotType == GEAR_ROT_TYPE_X){
				vec.y+=ELEC_GEAR_CENTER_OFS_Y;
			}
			
			//回転計算
			{
				rotate.x = 0;
				rotate.y = 0;
				rotate.z = 0;
				//初期回転を取得
				GetStartRotate(&(room_gear[i]), elec_gym_work->GearRotateCode, &rotate);
			}
			
			
			elec_temp->ObjEntryIdx[i] = 
				M3DO_SetMap3DObjExp(fsys->Map3DObjExp,
									fsys->MapResource,
									room_gear[i].ObjID,
									&vec, &rotate,
									fsys->field_3d_anime );
		}
	}
}

//---------------------------------------------------------------------------
/**
 * @brief	電気ジム:終了処理
 * 
 * @param	fsys	フィールドシステムポインタ
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
void GYM_EndElecGym(FIELDSYS_WORK *fsys)
{
	ELEC_GYM_TEMP_WORK *elec_temp;
	elec_temp = (ELEC_GYM_TEMP_WORK *)fsys->fldmap->Work;
	sys_FreeMemoryEz(elec_temp);
	fsys->fldmap->Work = NULL;
}

//---------------------------------------------------------------------------
/**
 * @brief	電気ジム:当たり判定
 * 
 * @param	fsys		フィールドシステムポインタ
 * @param	inGridX		グリッドX座標
 * @param	inGridZ		グリッドZ座標
 * @param	inHeight	高さ
 * @param	outHit		判定結果	TRUE:ヒット	FALSE:ヒットしてない
 * 
 * @return	BOOL		TRUE:このあと通常当たり判定を行わない	FALSE:通常当たり判定を行う
 */
//---------------------------------------------------------------------------
BOOL GYM_HitCheckElecGym(	FIELDSYS_WORK *fsys,
							const int inGridX, const int inGridZ,
							const fx32 inHeight, BOOL *outHit	)
{
	
	int i, hit_num,rot;
	u8 const *hit_list;
	u8 idx;
	GEAR_HIT_RECT const *rect;
	ELEC_GYM_TEMP_WORK *elec_temp;
	elec_temp = (ELEC_GYM_TEMP_WORK *)fsys->fldmap->Work;
	
	hit_num = GearHitInfo[elec_temp->RoomNo].HitNum;
	rot = elec_temp->RotateCode;
	hit_list = GearHitInfo[elec_temp->RoomNo].HitList[rot];

	rect = RoomGearHit[elec_temp->RoomNo];

	for(i=0;i<hit_num;i++){
		idx = hit_list[i];
		if ( (rect[idx].X<=inGridX)&&(inGridX<rect[idx].X+rect[idx].W) &&
				(rect[idx].Z<=inGridZ)&&(inGridZ<rect[idx].Z+rect[idx].H) ){
			(*outHit) = TRUE;
			return TRUE;	//HITしたので、通常当たり判定はパス
		}
	}
		
	return FALSE;	//通常当たり判定を実行
}

//---------------------------------------------------------------------------
/**
 * @brief	電気ジム:ギア回転
 * 
 * @param	fsys	フィールドシステムポインタ
 * @param	inRotate	回転（0:+90°、1:-90°、2:180°）
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
void GYM_RotateElecGymGear(FIELDSYS_WORK *fsys, const u8 inRotate)
{
	EVENT_GYM_WORK * egw;
	ELEC_GYM_TEMP_WORK *elec_temp;
	GIMMICKWORK *work;
	ELEC_GYM_WORK *elec_gym_work;
	
	work = SaveData_GetGimmickWork(GameSystem_GetSaveData(fsys));
	elec_gym_work = (ELEC_GYM_WORK*)GIMMICKWORK_Get(work, FLD_GIMMICK_ELEC_GYM);
	
	elec_temp = (ELEC_GYM_TEMP_WORK *)fsys->fldmap->Work;
	
	//イベントコール
	egw = sys_AllocMemoryLo(HEAPID_WORLD, sizeof(EVENT_GYM_WORK));
	egw->seq = 0;

	{
		s8 rot_code;
		elec_temp->RotVal = inRotate;
		elec_temp->NowRotate = 0;
		elec_temp->GearNum = GetGearNum(elec_gym_work->RoomNo);
		rot_code = elec_gym_work->GearRotateCode;
		
		if ( inRotate == ROTATE_GEAR_PLUS90){
			elec_temp->DstRotate = 0x4000;
			rot_code = (rot_code+1)%GEAR_ROT_ST_MAX;
		}else if(inRotate == ROTATE_GEAR_MINUS90){
			elec_temp->DstRotate = 0x4000;
			rot_code--;
			if (rot_code<0){
				rot_code = GEAR_ROT_ST_270;
			}
		}else if (inRotate == ROTATE_GEAR_PLUS180){
			elec_temp->DstRotate = 0x8000;
			rot_code = (rot_code+2)%GEAR_ROT_ST_MAX;
		}else{
			GF_ASSERT(0&&"ERROR:GearRotate unknown");
			return;
		}

		elec_gym_work->GearRotateCode = rot_code;
		elec_temp->RotateCode = rot_code;

		//ギア回転音
		Snd_SePlay( SE_NAGISA_GYM_BUTTON );
		
		FieldEvent_Call(fsys->event, GMEVENT_RotateElecGymGear, egw);
	}
}

//---------------------------------------------------------------------------
/**
 * @brief	電気ジム:ギアの回転
 * 
 * @param	event	イベントポインタ
 * 
 * @return	BOOL TRUE:イベント終了	FALSE:イベント継続
 */
//---------------------------------------------------------------------------
static BOOL GMEVENT_RotateElecGymGear(GMEVENT_CONTROL * event)
{
	M3DO_PTR ptr;
	FIELDSYS_WORK * fsys = FieldEvent_GetFieldSysWork(event);
	EVENT_GYM_WORK * egw = FieldEvent_GetSpecialWork(event);
	ELEC_GYM_TEMP_WORK *elec_temp;

	elec_temp = (ELEC_GYM_TEMP_WORK *)fsys->fldmap->Work;
	switch (egw->seq) {
	case 0:	//ギア回転
		{
			u8 i;
			u16 rot;
			u16 speed;
			//回転形式ごとに、変化量を足しこむ
			if (elec_temp->NowRotate + GEAR_ROT_SPEED <= elec_temp->DstRotate){
				speed = GEAR_ROT_SPEED;
			}else{
				speed = elec_temp->NowRotate + GEAR_ROT_SPEED - elec_temp->DstRotate;
			}
			elec_temp->NowRotate += speed;

			rot = speed;

			if (elec_temp->RotVal == ROTATE_GEAR_MINUS90){
				rot = -rot;
			}

			for(i=0;i<elec_temp->GearNum;i++){
				int entry;
				//ギアOBJを取得
				entry = elec_temp->ObjEntryIdx[i];
				ptr = M3DO_GetMap3DObjData(fsys->Map3DObjExp, entry);

				//回転
				{
					VecFx32 *rot_vec;
					rot_vec = M3DO_GetRotateVecPtr(ptr);

					//時計回り、反時計回りで足すか引くか分岐する
					{
						const GEAR *room_gear;
						fx32 *target;
						u16 temp,apply_rot;
						room_gear = RoomGear[elec_temp->RoomNo];
						if (room_gear[i].RotateDir == GEAR_ROT_DIR_RIGHT){
							apply_rot = -rot;
						}else{
							apply_rot = rot;
						}
						//回転軸対象の決定
						switch (room_gear[i].RotType){
						case GEAR_ROT_TYPE_Y:
							target = &(rot_vec->y);
							break;
						case GEAR_ROT_TYPE_X:
							target = &(rot_vec->x);
							break;
						default:
							GF_ASSERT(0&&"ERROR:RotType unknown");
						}
						temp = (*target);
						temp += apply_rot;
						(*target) = temp;
					}
				}
			}	//end for

			if (elec_temp->NowRotate >= elec_temp->DstRotate){
				(egw->seq) ++;
			}
		}
		break;
	case 1:
		sys_FreeMemoryEz(egw);
		return TRUE;
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////

//PL 草ジム kaga

//#define DEBUG_PL_KUSAGYM_TEST_TASK	//定義でデバッグ用タスクセット

//--------------------------------------------------------------
//	define
//--------------------------------------------------------------
enum	///針種類
{
	HARITYPE_S = 0,	///<短針
	HARITYPE_L,		///<長針
	HARITYPE_MAX,	///<針種類最大
};

enum	///草ジム時刻進行フラグ
{
	HARISEQFLAG_0,	///<初期
	HARISEQFLAG_1,	///<トレーナー イを倒した
	HARISEQFLAG_2,	///<トレーナー ロを倒した
	HARISEQFLAG_3,	///<トレーナー ハを倒した
	HARISEQFLAG_4,	///<ジムリーダーを倒した
	HARISEQFLAG_MAX,///<最大
	HARISEQFLAG_END=HARISEQFLAG_4,///<終了番号
};

typedef enum //草ジムジャンプ種類
{
	HARIJUMP_H,	//横
	HARIJUMP_V,	//縦
	HARIJUMP_NON,
}HARIJUMPTYPE;

#define HARI_CX_GRID (11)
#define HARI_CZ_GRID (13)
#define HARI_CX_FX (GRID_SIZE_FX32(HARI_CX_GRID)+GRID_HALF_FX32)
#define HARI_CZ_FX (GRID_SIZE_FX32(HARI_CZ_GRID)+GRID_HALF_FX32)
#define HARI_CY_S (12)
//#define HARI_CY_L (20)
#define HARI_CY_L (13)
#define HARI_CY_S_FX (NUM_FX32(HARI_CY_S))
#define HARI_CY_L_FX (NUM_FX32(HARI_CY_L))

#define HARI_ROTATE_FX360(a) (FX_GET_ROTA_NUM((a))+1)	///<360->0x10000
#define HARI_ROTATE_MIN (360/60) ///<360->60
#define HARI_ROTATE_MIN_FX (HARI_ROTATE_FX360(HARI_ROTATE_MIN)) ///<fx
#define HARI_ROTATE_60M_FX(a) ((a)*HARI_ROTATE_MIN_FX) 	///<分 fx乗算

#define HARI_ROTATE_90_FX (0x4000) ///<90度 fx
#define HARI_ROTATE_15M_FX(a) ((a)*HARI_ROTATE_90_FX) ///<15分 fx

#define HARI_ROTATE_60M_1H_GAIN_FX(a) ((HARI_ROTATE_60M_FX(5)/60)*(a)) ///<分->短針の量

#define HARI_ROTATE_OFFS_S_FX (HARI_ROTATE_90_FX*2) //短針回転オフセット
#define HARI_ROTATE_OFFS_L_FX (-HARI_ROTATE_90_FX*1) //長針回転オフセット

#define HARI_HITC_START_GX (5) //時計台あたり判定座標
#define HARI_HITC_START_GZ (7)
#define HARI_HITC_SIZE_GX (13)
#define HARI_HITC_SIZE_GZ (13)
#define HARI_HITC_END_GX (HARI_HITC_START_GX+HARI_HITC_SIZE_GX-1)
#define HARI_HITC_END_GZ (HARI_HITC_START_GZ+HARI_HITC_SIZE_GZ-1)

#define HARI_SPEED_FX (FX32_ONE*2)

#define FUNSUI_HITC_START_GX (1)
#define FUNSUI_HITC_START_GZ (19)
#define FUNSUI_HITC_SIZE_GX (21)
#define FUNSUI_HITC_SIZE_GZ (1)
#define FUNSUI_HITC_END_GX (FUNSUI_HITC_START_GX+FUNSUI_HITC_SIZE_GX-1)
#define FUNSUI_HITC_END_GZ (FUNSUI_HITC_START_GZ+FUNSUI_HITC_SIZE_GZ-1)

#define AC_GTIME_SCR (AC_WALK_U_8F)

#define HARI_SEQ4_QUICK

//--------------------------------------------------------------
//	struct
//--------------------------------------------------------------
typedef struct	///<時刻
{
	s16 hour;
	s16 min;
}TIMEDATA;

typedef struct
{
	s16 gx;
	s16 gz;
	HARIJUMPTYPE jump_type;
}EXATTRPOS;

typedef struct	///<針OBJ
{
	VecFx32 rotate;
	u32 bmid;
	u16 entry_idx;	//拡張モデル登録インデックス
	M3DO_PTR m3do;
}HARIOBJ;

typedef struct PL_GRASS_GYM_LOCAL_WORK_tag	///<草ジム内部ワーク
{
	fx32 hour;
	fx32 min;
	int hari_hour_min_raise_flag;
	HARIOBJ hari[HARITYPE_MAX];	//針OBJ
	FIELDSYS_WORK *fsys;
	
#ifdef DEBUG_PL_KUSAGYM_TEST_TASK
	TCB_PTR tcb;
#endif
}PL_GRASS_GYM_LOCAL_WORK;

typedef struct ///<草ジムスクロールワーク
{
	int seq_no;
	int start_x;
	int target_gx;
	int target_gz;
	int before_gx;
	int before_gz;
	int dir_h;
	int dir_v;
	FIELD_OBJ_PTR fldobj;
}PLGRASSGYM_SCROLL_WORK;

typedef struct ///<草ジムイベントワーク
{
	int seq_no;
	int wait;
	fx32 next_hour;
	fx32 next_min;
	
	FIELDSYS_WORK *fsys;
	PL_GRASS_GYM_WORK *gym_work;
	PL_GRASS_GYM_LOCAL_WORK *gym_l_work;
	PLGRASSGYM_SCROLL_WORK scroll;
	
	u32 msgidx;
	GF_BGL_BMPWIN *msgwin;
	MSGDATA_MANAGER *msgman;
	STRBUF *strbuf;
}EV_PLGRASSGYM_TIME_WORK;

//--------------------------------------------------------------
//	data
//--------------------------------------------------------------
static const u32 DATA_HariBMID[HARITYPE_MAX] =	///<針配置モデルID
{
	BMID_GYM04_HARI_S,
	BMID_GYM04_HARI_L,
};

static const VecFx32 DATA_HariPos[HARITYPE_MAX] =	///<針座標
{
	{ HARI_CX_FX, HARI_CY_S_FX, HARI_CZ_FX },
	{ HARI_CX_FX, HARI_CY_L_FX, HARI_CZ_FX },
};

static const u16 DATA_HariRotateHour[12] = ///<時間別針角度
{
	HARI_ROTATE_60M_FX(0),	//0 or 12
	HARI_ROTATE_60M_FX(5),	//1
	HARI_ROTATE_60M_FX(10),	//2
	HARI_ROTATE_15M_FX(1),	//3
	HARI_ROTATE_60M_FX(20),	//4
	HARI_ROTATE_60M_FX(25),	//5
	HARI_ROTATE_15M_FX(2),	//6
	HARI_ROTATE_60M_FX(35),	//7
	HARI_ROTATE_60M_FX(40),	//8
	HARI_ROTATE_15M_FX(3),	//9
	HARI_ROTATE_60M_FX(50),	//10
	HARI_ROTATE_60M_FX(55),	//11
};

static const TIMEDATA DATA_HariTimeData[HARISEQFLAG_MAX] = ///<進行別時刻
{
	{ 7, 25 },
	{ 6, 15 },
	{ 9, 15 },
//	{ 9, 45 },
	{ 0, 45 },
	{ 0, 30 },
};

static const EXATTRPOS DATA_HariAttrPosEx[HARISEQFLAG_MAX] =
{
	{-1,-1,HARIJUMP_NON},
	{11,18,HARIJUMP_V},
	{6,13,HARIJUMP_H},
//	{6,13,HARIJUMP_H},
	{11,8,HARIJUMP_V},
	{11,8,HARIJUMP_V},
};

static ALIGN4 const u8 DATA_HariHitTbl[HARISEQFLAG_MAX][HARI_HITC_SIZE_GX*HARI_HITC_SIZE_GZ] = //時計台シーケンス別あたり判定
{
	{
		1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,
		0,1,1,0,0,0,0,0,0,0,1,1,0,
	},
	{
		1,1,1,1,1,1,1,1,1,1,1,1,0,
		1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,0,
		1,1,1,1,1,1,1,1,1,1,1,1,0,
		1,1,1,1,1,1,0,1,1,1,1,1,0,
		1,1,1,1,1,0,0,0,0,0,0,0,0,
		1,1,1,1,1,1,0,1,1,1,1,1,0,
		1,1,1,1,1,1,0,1,1,1,1,1,0,
		1,1,1,1,1,1,0,1,1,1,1,1,0,
		1,1,1,1,1,1,0,1,1,1,1,1,1,
		1,1,1,1,1,1,0,1,1,1,1,1,1,
		0,1,1,0,0,0,0,0,0,0,1,1,0,
	},
	{
		0,1,1,1,1,1,1,1,1,1,1,1,0,
		1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,
		0,1,1,1,1,1,1,1,1,1,1,1,0,
		0,1,1,1,1,1,1,1,1,1,1,1,0,
		0,1,1,1,1,1,0,1,1,1,1,1,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,1,1,1,1,1,0,1,1,1,1,1,0,
		0,1,1,1,1,1,1,1,1,1,1,1,0,
		0,1,1,1,1,1,1,1,1,1,1,1,0,
		1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,
		0,1,1,0,0,0,0,0,0,0,1,1,0,
	},
	{
		0,1,1,0,0,0,0,0,0,0,1,1,0,
		1,1,1,1,1,1,0,1,1,1,1,1,1,
		1,1,1,1,1,1,0,1,1,1,1,1,1,
		0,1,1,1,1,1,0,1,1,1,1,1,0,
		0,1,1,1,1,1,0,1,1,1,1,1,0,
		0,1,1,1,1,1,0,1,1,1,1,1,0,
		0,0,0,0,0,0,0,0,1,1,1,1,0,
		0,1,1,1,1,1,0,1,1,1,1,1,0,
		0,1,1,1,1,1,1,1,1,1,1,1,0,
		0,1,1,1,1,1,1,1,1,1,1,1,0,
		1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,
		0,1,1,0,0,0,0,0,0,0,1,1,0,
	},
	{
		0,1,1,0,0,0,0,0,0,0,1,1,0,
		1,1,1,1,1,1,0,1,1,1,1,1,1,
		1,1,1,1,1,1,0,1,1,1,1,1,1,
		0,1,1,1,1,1,0,1,1,1,1,1,0,
		0,1,1,1,1,1,0,1,1,1,1,1,0,
		0,1,1,1,1,1,0,1,1,1,1,1,0,
		0,1,1,1,1,0,0,0,1,1,1,1,0,
		0,1,1,1,1,1,0,1,1,1,1,1,0,
		0,1,1,1,1,1,0,1,1,1,1,1,0,
		0,1,1,1,1,1,0,1,1,1,1,1,0,
		1,1,1,1,1,1,0,1,1,1,1,1,1,
		1,1,1,1,1,1,0,1,1,1,1,1,1,
		0,1,1,0,0,0,0,0,0,0,1,1,0,
	},
};

static ALIGN4 const u8 DATA_FunsuiHitTbl[HARISEQFLAG_MAX][FUNSUI_HITC_SIZE_GX*
FUNSUI_HITC_SIZE_GZ] =
{
	{1,1,1,1,0,0, 0,0,0,0,0,0,0,0,0, 0,0,1,1,1,1},
	{1,1,1,1,0,0, 0,0,0,0,0,0,0,0,0, 0,0,1,1,1,1},
	{1,1,1,1,0,0, 0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0},
	{0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0},
	{0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0},
};

//--------------------------------------------------------------
/**
 * PL 草ジム　時刻シーケンスセット
 * @param	fsys	FIELDSYS_WORK
 * @param	seq		HARISEQFLAG_0等
 * @retval	nothing
 */
//--------------------------------------------------------------
static void gym_PLGrassTimeSeqSysSet( FIELDSYS_WORK *fsys, u16 seq )
{
	EVENTWORK *ev = SaveData_GetEventWork( fsys->savedata );
	SysWork_PLGrassGymTimeSeqSet( ev, seq );
}

//--------------------------------------------------------------
/**
 * PL 草ジム　時刻シーケンス取得
 * @param	fsys	FIELDSYS_WORK
 * @param	seq		HARISEQFLAG_0等
 * @retval	nothing
 */
//--------------------------------------------------------------
static u32 gym_PLGrassTimeSeqSysGet( FIELDSYS_WORK *fsys )
{
	EVENTWORK *ev = SaveData_GetEventWork( fsys->savedata );
	return( SysWork_PLGrassGymTimeSeqGet(ev) );
}

//--------------------------------------------------------------
/**
 * PL 草ジム　時間を進める
 * @param	work	PL_GRASS_GYM_LOCAL_WORK
 * @param	add		増減値
 * @retval	nothing
 */
//--------------------------------------------------------------
static void gym_PLGrassTimeGain( PL_GRASS_GYM_LOCAL_WORK *work, fx32 add )
{
	work->min += add;
	
	while( work->min >= NUM_FX32(60) ){
		work->min -= NUM_FX32(60);
		work->hour += NUM_FX32( 1 );
	}
	
	while( work->min < 0 ){
		work->min += NUM_FX32(60);
		work->hour += NUM_FX32( 1 );
	}
	
	work->hour %= NUM_FX32( 12 );
}

//--------------------------------------------------------------
/**
 * PL 草ジム 針回転軸セット
 * @param	hari	HARIOBJ
 * @return	nothing
 */
//--------------------------------------------------------------
static void gym_PLGrassHariRotateSet( HARIOBJ *hari )
{
	VecFx32 h_rot = hari->rotate;
	VecFx32 *rot = M3DO_GetRotateVecPtr( hari->m3do );
	
	rot->x = hari->rotate.x;
	rot->y = (0x00010000 - hari->rotate.y) % 0x00010000;	//反転
	rot->z = hari->rotate.z;
}

//--------------------------------------------------------------
/**
 * PL 草ジム 時間から針角度セット
 * @param	PL_GRASS_GYM_LOCAL_WORK *work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void gym_PLGrassTimeHariRotateSet( PL_GRASS_GYM_LOCAL_WORK *work )
{
	int hour,min;
	HARIOBJ *hari;
	
	hari = &work->hari[HARITYPE_L];
	min = FX32_NUM( work->min );
	
	if( (min % 15) == 0 ){	//丁度15分単位
		hari->rotate.y = DATA_HariRotateHour[min/5];
	}else{
		hari->rotate.y = HARI_ROTATE_60M_FX( min );
	}
	
	hari->rotate.y += HARI_ROTATE_OFFS_L_FX;
	hari->rotate.y %= 0x00010000;
	
	gym_PLGrassHariRotateSet( hari );
	
	hour = FX32_NUM( work->hour );
	hari = &work->hari[HARITYPE_S];
	hari->rotate.y = DATA_HariRotateHour[hour];
	
	if( work->hari_hour_min_raise_flag == TRUE ){	//分進行を時に反映
		hari->rotate.y += HARI_ROTATE_60M_1H_GAIN_FX( min ); 
	}
	
	hari->rotate.y += HARI_ROTATE_OFFS_S_FX;
	hari->rotate.y %= 0x00010000;
	
	gym_PLGrassHariRotateSet( hari );
}

//--------------------------------------------------------------
/**
 * 草ジムデバッグテスト
 */
//--------------------------------------------------------------
#ifdef DEBUG_PL_KUSAGYM_TEST_TASK
static void gym_test_tcb( TCB_PTR tcb, void *wk )
{
	FIELDSYS_WORK *fsys = wk;
	PL_GRASS_GYM_LOCAL_WORK *work = fsys->fldmap->Work;
	
#if 1
	if( (sys.cont & PAD_BUTTON_L) == 0 ){
		return;
	}
	
	/*
	if( work->min == 0 ){
		if( (sys.cont & PAD_BUTTON_A) == 0 ){
			return;
		}
	}
	*/
#endif
	
#if	1
	gym_PLGrassTimeGain( work, FX32_ONE );
	gym_PLGrassTimeHariRotateSet( work );
	
	if( work->min == 0 ){
		OS_Printf( "草ジム時刻　%02d:%02dをお知らせします\n",
			FX32_NUM(work->hour), FX32_NUM(work->min) );
	}
#endif
}
#endif

//---------------------------------------------------------------------------
/**
 * @brief	PL 草ジム:セットアップ
 * 
 * @param	fsys	フィールドシステムポインタ
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
void GYM_SetupPLGrassGym( FIELDSYS_WORK *fsys )
{
	GIMMICKWORK *gimwork;
	PL_GRASS_GYM_WORK *gymwork;
	PL_GRASS_GYM_LOCAL_WORK *work;
	
	gimwork = SaveData_GetGimmickWork( GameSystem_GetSaveData(fsys) );
	gymwork = GIMMICKWORK_Get( gimwork, FLD_GIMMICK_PL_GRASS_GYM );
	
	work = sys_AllocMemory(
			HEAPID_FIELD, sizeof(PL_GRASS_GYM_LOCAL_WORK) );
	memset( work, 0, sizeof(PL_GRASS_GYM_LOCAL_WORK) );
	fsys->fldmap->Work = work;
	
	{	//ワーク初期化
		work->fsys = fsys;
	}
	
	{	//針モデルセット
		int i;
		HARIOBJ *hari = work->hari;
		const u32 *bmid = DATA_HariBMID;
		const VecFx32 *pos = DATA_HariPos;
		const VecFx32 rot = {0,0,0};
		
		for( i = 0; i < HARITYPE_MAX; i++, hari++, bmid++, pos++ ){
			hari->bmid = *bmid;
			
			hari->entry_idx =
				M3DO_SetMap3DObjExp(
				fsys->Map3DObjExp, fsys->MapResource,
				*bmid, pos, &rot,
				fsys->field_3d_anime );
			
			hari->m3do =
				M3DO_GetMap3DObjDataFromID( fsys->Map3DObjExp, *bmid );
		}
	}
	
	{	//イベントフラグを元にタイムシーケンスセット
		int type[HARISEQFLAG_3] =
		{ GYM04EFF_FUNSUI_ALL, GYM04EFF_FUNSUI_ALL, GYM04EFF_FUNSUI_LEFT };
		
		gymwork->time_seqflag = gym_PLGrassTimeSeqSysGet( fsys );
		
		{
			const TIMEDATA *time =
				&DATA_HariTimeData[gymwork->time_seqflag];
			work->hour = NUM_FX32( time->hour );
			work->min = NUM_FX32( time->min );
			gym_PLGrassTimeHariRotateSet( work );
		}
		
		if( gymwork->time_seqflag < HARISEQFLAG_3 ){
			FE_FldOBJGym04EffSet( fsys, type[gymwork->time_seqflag] );
		}
	}
	
	#ifdef DEBUG_PL_KUSAGYM_TEST_TASK
	work->tcb = TCB_Add( gym_test_tcb, fsys, 0 );
	#endif
}

//---------------------------------------------------------------------------
/**
 * @brief	PL 草ジム:終了処理
 * 
 * @param	fsys	フィールドシステムポインタ
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
void GYM_EndPLGrassGym( FIELDSYS_WORK *fsys )
{
	PL_GRASS_GYM_LOCAL_WORK *work = fsys->fldmap->Work;
	#ifdef DEBUG_PL_KUSAGYM_TEST_TASK
	TCB_Delete( work->tcb );
	#endif
	sys_FreeMemoryEz( work );
	fsys->fldmap->Work = NULL;
}

//---------------------------------------------------------------------------
/**
 * @brief	PL 草ジム:草ジム用当たり判定
 * 
 * @param	fsys		フィールドシステムポインタ
 * @param	inGridX		グリッドX座標
 * @param	inGridZ		グリッドZ座標
 * @param	inHeight	高さ
 * @param	outHit		判定結果	TRUE:ヒット	FALSE:ヒットしてない
 * @return	BOOL		TRUE:このあと通常当たり判定を行わない	FALSE:通常当たり判定を行う
 */
//---------------------------------------------------------------------------
BOOL GYM_HitCheckPLGrassGym( FIELDSYS_WORK *fsys,
							const int inGridX, const int inGridZ,
							const fx32 inHeight, BOOL *outHit	)
{
	BOOL hit;
	int seq,x,z;
	GIMMICKWORK *gimwork;
	PL_GRASS_GYM_WORK *gymwork;
	
	gimwork = SaveData_GetGimmickWork( GameSystem_GetSaveData(fsys) );
	gymwork = GIMMICKWORK_Get( gimwork, FLD_GIMMICK_PL_GRASS_GYM );
	seq = gymwork->time_seqflag;
	
	hit = FALSE;
	*outHit = FALSE;
	
	//時計台
	if( (inGridZ >= HARI_HITC_START_GZ && inGridZ <= HARI_HITC_END_GZ) &&
		(inGridX >= HARI_HITC_START_GX && inGridX <= HARI_HITC_END_GX) ){
		x = inGridX - HARI_HITC_START_GX;
		z = (inGridZ - HARI_HITC_START_GZ) * HARI_HITC_SIZE_GX;
		hit = DATA_HariHitTbl[seq][z+x];
		*outHit = hit;
	}
	
	//噴水
	if( hit == FALSE &&
		(inGridZ >= FUNSUI_HITC_START_GZ && inGridZ <= FUNSUI_HITC_END_GZ) &&
		(inGridX >= FUNSUI_HITC_START_GX && inGridX <= FUNSUI_HITC_END_GX) ){
		x = inGridX - FUNSUI_HITC_START_GX;
		z = (inGridZ - FUNSUI_HITC_START_GZ) * FUNSUI_HITC_SIZE_GX;
		hit = DATA_FunsuiHitTbl[seq][z+x];
		*outHit = hit;
	}
	
	return( hit );
}

//--------------------------------------------------------------
/**
 * 時計中心へスクロール　初期化
 * @param	fsys	FIELDSYS_WORK
 * @param	work	PLGRASSGYM_SCROLL_WORK
 * @param	gx		指定GridX座標
 * @param	gZ		指定GridZ座標
 * @retval	nothing
 */
//--------------------------------------------------------------
static void gym_PLGrassCenterScrollInit(
	FIELDSYS_WORK *fsys, PLGRASSGYM_SCROLL_WORK *work, int gx, int gz )
{
	int x,z,cx,cz;
	const VecFx32 *pos;
	
	x = Player_NowGPosXGet( fsys->player );
	z = Player_NowGPosZGet( fsys->player );
	
	work->before_gx = x;
	work->before_gz = z;
	work->target_gx = gx;
	work->target_gz = gz;
	
	if( (work->target_gx - work->before_gx) < 0 ){
		work->dir_h = DIR_LEFT;
	}else{
		work->dir_h = DIR_RIGHT;
	}
	
	if( (work->target_gz - work->before_gz) < 0 ){
		work->dir_v = DIR_UP;
	}else{
		work->dir_v = DIR_DOWN;
	}
	
	cx = work->target_gx - work->before_gx;
	if( cx < 0 ){ cx = -cx; }
	cz = work->target_gz - work->before_gz;
	if( cz < 0 ){ cz = -cz; }
	
	if( cx < cz ){
		work->start_x = 1;
	}
	
	work->fldobj = FieldOBJ_AddHMake( fsys->fldobjsys, 
			x, z, DIR_UP, NONDRAW, MV_DMY, fsys->location->zone_id );
	
	FieldOBJ_VecPosNowHeightGetSet( work->fldobj );
	FieldOBJ_StatusBitSet_Vanish( work->fldobj, TRUE );
	FieldOBJ_StatusBitSet_FellowHit( work->fldobj, FALSE );
	FieldOBJ_StatusBitSet_HeightGetOFF( work->fldobj, TRUE );
	
	pos = FieldOBJ_VecPosPtrGet( work->fldobj );
	DivMapBindTarget( pos, fsys->map_cont_dat );
	GFC_BindCameraTarget( pos, fsys->camera_ptr );
}

//--------------------------------------------------------------
/**
 * 時計中心へスクロール　再初期化
 * @param	fsys	FIELDSYS_WORK
 * @param	work	PLGRASSGYM_SCROLL_WORK
 * @param	gx		指定GridX座標
 * @param	gZ		指定GridZ座標
 * @retval	nothing
 */
//--------------------------------------------------------------
static void gym_PLGrassCenterScrollReset(
	PLGRASSGYM_SCROLL_WORK *work, int gx, int gz )
{
	int x,z,cx,cz;
	const VecFx32 *pos;
	
	work->before_gx = work->target_gx;
	work->before_gz = work->target_gz;
	work->target_gx = gx;
	work->target_gz = gz;
	
	if( (work->target_gx - work->before_gx) < 0 ){
		work->dir_h = DIR_LEFT;
	}else{
		work->dir_h = DIR_RIGHT;
	}
	
	if( (work->target_gz - work->before_gz) < 0 ){
		work->dir_v = DIR_UP;
	}else{
		work->dir_v = DIR_DOWN;
	}
	
	cx = work->target_gx - work->before_gx;
	if( cx < 0 ){ cx = -cx; }
	cz = work->target_gz - work->before_gz;
	if( cz < 0 ){ cz = -cz; }
	
	if( cx < cz ){
		work->start_x = 1;
	}
}

//--------------------------------------------------------------
/**
 * 時計中心へスクロール　反転初期化
 * @param	work	PLGRASSGYM_SCROLL_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void gym_PLGrassCenterScrollFlipInit( PLGRASSGYM_SCROLL_WORK *work )
{
	work->start_x ^= 1;
	work->target_gx = work->before_gx;
	work->target_gz = work->before_gz;
	work->dir_h = FieldOBJTool_DirFlip( work->dir_h );
	work->dir_v = FieldOBJTool_DirFlip( work->dir_v );
}

//--------------------------------------------------------------
/**
 * 時計中心へスクロール　パラメタ指定
 * @param	work	PLGRASSGYM_SCROLL_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void gym_PLGrassCenterScrollParamInit(
	PLGRASSGYM_SCROLL_WORK *work,
	int start_x, int gx, int gz, int dir_h, int dir_v )
{
	work->start_x = start_x;
	work->target_gx = gx;
	work->target_gz = gz;
	work->dir_h = dir_h;
	work->dir_v = dir_v;
}

//--------------------------------------------------------------
/**
 * 時計中心へスクロール　終了
 * @param	fsys	FIELDSYS_WORK
 * @param	work	PLGRASSGYM_SCROLL_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void gym_PLGrassCenterScrollEnd(
		FIELDSYS_WORK *fsys, PLGRASSGYM_SCROLL_WORK *work )
{
	const VecFx32 *pos = Player_VecPosPtrGet( fsys->player );
	DivMapBindTarget( pos, fsys->map_cont_dat );
	GFC_BindCameraTarget( pos, fsys->camera_ptr );
	FieldOBJ_Delete( work->fldobj );
}

//--------------------------------------------------------------
/**
 * 時計中心へスクロール　スクロール
 * @param	work	PLGRASSGYM_SCROLL_WORK
 * @retval	BOOL	TRUE=終了
 */
//--------------------------------------------------------------
static BOOL gym_PLGrassCenterScroll( PLGRASSGYM_SCROLL_WORK *work )
{
	FIELD_OBJ_PTR fldobj = work->fldobj;
	
	if( FieldOBJ_AcmdSetCheck(work->fldobj) == TRUE ){
		int ac = ACMD_MAX;
		int x = FieldOBJ_NowPosGX_Get( fldobj );
		int z = FieldOBJ_NowPosGZ_Get( fldobj );
		
		if( x == work->target_gx && z == work->target_gz ){
			return( TRUE );
		}
		
		if( work->start_x ){
			if( x != work->target_gx ){
				ac = FieldOBJ_AcmdCodeDirChange( work->dir_h, AC_GTIME_SCR );
			}else if( z != work->target_gz ){
				ac = FieldOBJ_AcmdCodeDirChange( work->dir_v, AC_GTIME_SCR );
			}
		}else{
			if( z != work->target_gz ){
				ac = FieldOBJ_AcmdCodeDirChange( work->dir_v, AC_GTIME_SCR );
			}else if( x != work->target_gx ){
				ac = FieldOBJ_AcmdCodeDirChange( work->dir_h, AC_GTIME_SCR );
			}
		}
		
		if( ac != ACMD_MAX ){
			FieldOBJ_AcmdSet( fldobj, ac );
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * PL 草ジム：イベント処理　指定時刻へ進ませる
 * @param	gym_l_work PL_GRASS_GYM_LOCAL_WORK
 * @param	nh	目標時
 * @param	nm	目標分
 * @retval	nothing
 */
//--------------------------------------------------------------
static BOOL PLGrassGymTimeGain(
		PL_GRASS_GYM_LOCAL_WORK *gym_l_work, const fx32 nh, const fx32 nm )
{
	BOOL ret = TRUE;
	fx32 oh = gym_l_work->hour;
	fx32 om = gym_l_work->min;
	
	if( oh != nh || om != nm ){
		fx32 h,m;
		
		gym_PLGrassTimeGain( gym_l_work, HARI_SPEED_FX );
		
		h = gym_l_work->hour;
		m = gym_l_work->min;
		
		if( h == nh ){
			if( om > m ){ //pass 60 minute
				fx32 cm = m + NUM_FX32( 60 );
				fx32 cnm = nm + NUM_FX32( 60 );
				
				if( (om < cnm && cm > cnm) ){
					m = nm;
				}
			}else if( (om < nm && m > nm) ){
				m = nm;
			}
		}
		
		gym_l_work->hour = h;
		gym_l_work->min = m;
	}
	
	if( gym_l_work->hour != nh || gym_l_work->min != nm ){
		ret = FALSE;
	}
	
	return( ret );
}

//--------------------------------------------------------------
/**
 * PL 草ジム:イベント　指定時刻へ進む
 * @param	event	GMEVENT_CONTROL
 * @retval	BOOL	TRUE:終了 FALSE:継続
 */
//--------------------------------------------------------------
static BOOL GMEVENT_PLGrassGymTimeGain( GMEVENT_CONTROL *event )
{
	BOOL ret;
	FIELDSYS_WORK *fsys = FieldEvent_GetFieldSysWork( event );
	EV_PLGRASSGYM_TIME_WORK *work = FieldEvent_GetSpecialWork( event );
	PL_GRASS_GYM_WORK *gym_work = work->gym_work;
	PL_GRASS_GYM_LOCAL_WORK *gym_l_work = work->gym_l_work;
	
	switch( work->seq_no ){
	case 0:	//時計中心へ移動
		gym_PLGrassCenterScrollInit(
			fsys, &work->scroll, HARI_CX_GRID, HARI_CZ_GRID );
		work->seq_no++;
	case 1:								//中心移動待ち
		if( gym_PLGrassCenterScroll(&work->scroll) == TRUE ){
			work->seq_no++;
		}
		
		break;
	case 2:	//中心移動後の一寸待ち
		work->wait++;
		if( work->wait < 8 ){
			break;
		}
		
		work->wait = 0;
		Snd_SePlay( SEQ_SE_PL_TOKEI21 );			//時計回転SE再生
		work->seq_no++;
	case 3:	//時刻進行

#ifdef HARI_SEQ4_QUICK
		if( gym_work->time_seqflag == HARISEQFLAG_4 ){
			if( gym_l_work->hour == work->next_hour &&
				gym_l_work->min > work->next_min ){
				gym_l_work->hour = NUM_FX32( 11 );
			}
		}
#endif
		ret = PLGrassGymTimeGain(
			gym_l_work, work->next_hour, work->next_min );
		
#ifdef HARI_SEQ4_QUICK
		if( gym_work->time_seqflag == HARISEQFLAG_4 ){
			const TIMEDATA *time =
				&DATA_HariTimeData[gym_work->time_seqflag];
			gym_l_work->hour = NUM_FX32( time->hour );
		}
#endif
		
#ifndef HARI_SEQ4_QUICK	//正規時計ロジック
		if( gym_l_work->hour == work->next_hour &&
			gym_l_work->min <= work->next_min ){
			gym_l_work->hari_hour_min_raise_flag = FALSE;
		}else{
			gym_l_work->hari_hour_min_raise_flag = TRUE;
		}
#else	//SEQ4の待ち時間短縮
		if( gym_l_work->hour == work->next_hour ){
			if(	gym_l_work->min <= work->next_min ||
				gym_work->time_seqflag == HARISEQFLAG_4 ){
				gym_l_work->hari_hour_min_raise_flag = FALSE;
			}
		}else{
			gym_l_work->hari_hour_min_raise_flag = TRUE;
		}
#endif
		gym_PLGrassTimeHariRotateSet( gym_l_work );
		
		if( ret == TRUE ){
			Snd_SeStopBySeqNo( SEQ_SE_PL_TOKEI21, 0 );	//時計回転SE停止
			Snd_SePlay( SEQ_SE_DP_PIRORIRO2 );	//時計回転が終了したSE再生
			work->seq_no++;
		}
		
		break;
	case 4:	//時刻進行後の一寸待ち
		work->wait++;
		if( work->wait < 8 ){
			break;
		}
		
		work->wait = 0;
		
		if( gym_work->time_seqflag == HARISEQFLAG_2 ||
			gym_work->time_seqflag == HARISEQFLAG_3 ){
			gym_PLGrassCenterScrollParamInit(
				&work->scroll, 0, 11, 19, DIR_LEFT, DIR_DOWN );
			work->seq_no = 7;
		}else{
			gym_PLGrassCenterScrollFlipInit( &work->scroll );
			work->seq_no++;
		}
		
		break;
	case 5:	//復帰スクロール
		if( gym_PLGrassCenterScroll(&work->scroll) == TRUE ){
			work->seq_no++;
		}
		
		break;
	case 6:	//終了
		gym_PLGrassCenterScrollEnd( fsys, &work->scroll );
		sys_FreeMemoryEz( work );
		return( TRUE );
	case 7:	//噴水停止　噴水と水平位置へスクロール
		if( gym_PLGrassCenterScroll(&work->scroll) == TRUE ){
			int gx = 20, gz = 19, dir_h = DIR_RIGHT;
			if( gym_work->time_seqflag == HARISEQFLAG_3 ){
				gx = 2;
				dir_h = DIR_LEFT;
			}
			gym_PLGrassCenterScrollParamInit(
				&work->scroll, TRUE, gx, gz, dir_h, DIR_DOWN );
			work->seq_no++;
		}else{
			break;
		}
	case 8:	//噴水停止　噴水位置へスクロール
		if( gym_PLGrassCenterScroll(&work->scroll) == TRUE ){
			work->seq_no++;
		}
		
		break;
	case 9:	//噴水停止　一寸待ち
		work->wait++;
		if( work->wait >= 4 ){
			work->wait = 0;
			if( gym_work->time_seqflag == HARISEQFLAG_2 ){
				FE_FldOBJGym04EffEndSet( fsys, GYM04EFF_FUNSUI_RIGHT );
			}else if( gym_work->time_seqflag == HARISEQFLAG_3 ){
				FE_FldOBJGym04EffEndSet( fsys, GYM04EFF_FUNSUI_LEFT );
			}
			Snd_SePlay( SEQ_SE_DP_T_AME );					//噴水停止音(ループ)の再生
			work->seq_no++;
		}
		
		break;
	case 10: //噴水停止　噴水停止アニメ待ち
		work->wait++;
		
		if( work->wait >= 60 ){
			work->wait = 0;
			work->seq_no++;
			Snd_SeStopBySeqNo( SEQ_SE_DP_T_AME, 0 );		//噴水停止音(ループ)の停止
			
			MSGMAN_GetString( work->msgman,
				msg_c04gym0101_fountain_01, work->strbuf );
			FldTalkBmpAdd( fsys->bgl, work->msgwin, FLD_MBGFRM_FONT );
			BmpTalkWinClear( work->msgwin, WINDOW_TRANS_ON );
			FieldTalkWinPut( work->msgwin,
				SaveData_GetConfig(fsys->savedata) );
			work->msgidx = FieldTalkMsgStart(
				work->msgwin, work->strbuf, 
				SaveData_GetConfig(fsys->savedata), 1 );
		}
		
		break;
	case 11: //噴水停止　停止メッセージ終了待ち
		if( FldTalkMsgEndCheck(work->msgidx) != TRUE ){
			break;
		}
		
		work->seq_no++;
	case 12:
		if( sys.trg & (PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL) ){
			int gx = Player_NowGPosXGet( fsys->player );
			int gz = Player_NowGPosZGet( fsys->player );
			gym_PLGrassCenterScrollReset( &work->scroll, gx, gz );
			
			BmpTalkWinClear( work->msgwin, WINDOW_TRANS_ON );
			GF_BGL_BmpWinDel( work->msgwin );
			work->wait = 0;
			work->seq_no = 5;
			break;
		}
	}
	
	return( FALSE );
}

/*	
	case 2:
		work->wait++;
		
		if( work->wait >= 8 ){
			work->wait = 0;
#if 0		
			if( gym_work->time_seqflag == HARISEQFLAG_2 ){
				FE_FldOBJGym04EffEndSet( fsys, GYM04EFF_FUNSUI_RIGHT );
			}else if( gym_work->time_seqflag == HARISEQFLAG_3 ){
				FE_FldOBJGym04EffEndSet( fsys, GYM04EFF_FUNSUI_LEFT );
			}
			
			Snd_SePlay( SEQ_SE_PL_TOKEI21 );			//時計回転SE再生
			work->seq_no++;
		}
		
		break;
	case 3:
		ret = PLGrassGymTimeGain(
			gym_l_work, work->next_hour, work->next_min );
	
		if( gym_l_work->hour == work->next_hour &&
			gym_l_work->min <= work->next_min ){
			gym_l_work->hari_hour_min_raise_flag = FALSE;
		}else{
			gym_l_work->hari_hour_min_raise_flag = TRUE;
		}
	
		gym_PLGrassTimeHariRotateSet( gym_l_work );
		
		if( ret == TRUE ){
			Snd_SeStopBySeqNo( SEQ_SE_PL_TOKEI21, 0 );	//時計回転SE停止
			Snd_SePlay( SEQ_SE_DP_PIRORIRO2 );			//時計回転が終了したSE再生
			work->seq_no++;
		}
		
		break;
	case 4:
		work->wait++;
		
		if( work->wait >= 8 ){
			work->wait = 0;
			work->seq_no++;
		}
		
		break;
	case 5:
		if( gym_PLGrassCenterScroll(&work->scroll) == TRUE ){
			work->seq_no++;
		}
		
		break;
	case 6:
		gym_PLGrassCenterScrollEnd( fsys, &work->scroll );
		sys_FreeMemoryEz( work );
		return( TRUE );
	}
	
	return( FALSE );
}
*/

//--------------------------------------------------------------
/**
 * PL 草ジム：時刻を進めるイベント
 * @param	fsys	FIELDSYS_WORK
 * @retval	BOOL	TRUE=イベント発動,
 * 					FALSE=これ以上の時刻進行は無いため発動しない
 */
//--------------------------------------------------------------
BOOL GYM_PLGrass_TimeGainEventSet( FIELDSYS_WORK *fsys,
		GF_BGL_BMPWIN *win, MSGDATA_MANAGER *msgman, STRBUF *buf )
{
	GIMMICKWORK *gimwork;
	PL_GRASS_GYM_WORK *gymwork;
	
	gimwork = SaveData_GetGimmickWork( GameSystem_GetSaveData(fsys) );
	gymwork = GIMMICKWORK_Get( gimwork, FLD_GIMMICK_PL_GRASS_GYM );
	
	if( gymwork->time_seqflag >= HARISEQFLAG_END ){
		return( FALSE );
	}
	
	gymwork->time_seqflag++;
	gym_PLGrassTimeSeqSysSet( fsys, gymwork->time_seqflag );
	
	OS_Printf( "草ジム　時刻進行番号 %d\n", gymwork->time_seqflag );
	
	{
		EV_PLGRASSGYM_TIME_WORK *work;
		PL_GRASS_GYM_LOCAL_WORK *gym_l_work = fsys->fldmap->Work;
		const TIMEDATA *time = &DATA_HariTimeData[gymwork->time_seqflag];
		
		work = sys_AllocMemory(
				HEAPID_WORLD, sizeof(EV_PLGRASSGYM_TIME_WORK) );
		memset( work, 0, sizeof(EV_PLGRASSGYM_TIME_WORK) );
		
		work->msgwin = win;
		work->msgman = msgman;
		work->strbuf = buf;
		work->next_hour = NUM_FX32( time->hour );
		work->next_min = NUM_FX32( time->min );
		work->fsys = fsys;
		work->gym_work = gymwork;
		work->gym_l_work = gym_l_work;
		
		FieldEvent_Call( fsys->event, GMEVENT_PLGrassGymTimeGain, work );
	}
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * PL 草ジム：ジャンプ位置チェック
 * @param	fsys	FIELDSYS_WORK
 * @param	dir		進行方向 DIR_UP等
 * @retval	BOOL	TRUE=ジャンプ位置
 */
//--------------------------------------------------------------
BOOL GYM_PLGrass_ExJumpPosCheck(
	FIELDSYS_WORK *fsys, int gx, int gz, int dir )
{
	GIMMICKWORK *gimwork;
	PL_GRASS_GYM_WORK *gymwork;
	const EXATTRPOS *pos;
	
	gimwork = SaveData_GetGimmickWork( GameSystem_GetSaveData(fsys) );
	gymwork = GIMMICKWORK_Get( gimwork, FLD_GIMMICK_PL_GRASS_GYM );
	pos = &DATA_HariAttrPosEx[gymwork->time_seqflag];
	
	if( pos->gx == gx && pos->gz == gz ){
		if( (pos->jump_type == HARIJUMP_H &&
			(dir == DIR_LEFT || dir == DIR_RIGHT)) ||
			(pos->jump_type == HARIJUMP_V &&
			(dir == DIR_UP || dir == DIR_DOWN)) ){
			return( TRUE );
		}
	}
	
	return( FALSE );
}

//==============================================================================
//	PL	格闘ジム kaga	
//==============================================================================
#define FGGYM_BAG_MAX (9)
#define FGGYM_BOX_MAX (11)
#define FGGYM_ACT_MAX (FGGYM_BOX_MAX+FGGYM_BAG_MAX)

#define FGGYM_ACT_GRID_OFFS_X (0)
#define FGGYM_ACT_GRID_OFFS_Z (-2)

#define FGGYM_GRID_SIZE_X (32)
#define FGGYM_GRID_SIZE_Z (32)
#define FGGYM_GRID_SIZE_XZ (FGGYM_GRID_SIZE_X*FGGYM_GRID_SIZE_Z)

//#define SANDBAG_PAUSE_FRAME (58)	//揺れアニメポーズフレーム
#define SANDBAG_START_FRAME (30)	//サンドバッグ移動開始フレーム
#define SANDBAG_ACCEL_FRAME (36)	//サンドバッグ加速フレーム
#define SANDBAG_BREAK_FRAME (43)
#define SANDBAG_START_SPEED_FX ((FX32_ONE/2)/SANDBAG_START_FRAME)
#define SANDBAG_MOVE_ANM_FRAME (SANDBAG_BREAK_FRAME-SANDBAG_START_FRAME)

#define BOX_BREAK_START_FRAME (4)

#define BAGMOVE_SCRSTART_GRID_U (3)
#define BAGMOVE_SCRSTART_GRID_D (4)
#define BAGMOVE_SCRSTART_GRID_L (4)
#define BAGMOVE_SCRSTART_GRID_R (4)

//--------------------------------------------------------------
///	サンドバッグ移動イベント番号
//--------------------------------------------------------------
enum
{
	SEQNO_BAGMOVE_INIT = 0,
	SEQNO_BAGMOVE_ACMD_SET,
	SEQNO_BAGMOVE_SHAKE_START,
	SEQNO_BAGMOVE_MOVE,
	SEQNO_BAGMOVE_BREAK_BOX_START,
	SEQNO_BAGMOVE_BREAK_BOX,
	SEQNO_BAGMOVE_SCROLL_WAIT,
	SEQNO_BAGMOVE_FLIP_SCROLL_WAIT,
	SEQNO_BAGMOVE_SHAKE_WAIT,
	SEQNO_BAGMOVE_ACMD_WAIT,
	SEQNO_BAGMOVE_END,
	SEQNO_BAGMOVE_MAX,
};
	
//--------------------------------------------------------------
///	FGGYM_EVRET
//--------------------------------------------------------------
typedef enum
{
	FGGYM_EVRET_NON,
	FGGYM_EVRET_NEXT,
	FGGYM_EVRET_END,
}FGGYM_EVRET;

//--------------------------------------------------------------
///	FGGYM_ACTTYPE
//--------------------------------------------------------------
typedef enum
{
	ACTTYPE_BOX,
	ACTTYPE_BAG,
}FGGYM_ACTTYPE;

//--------------------------------------------------------------
///	SANDBAGHIT
//--------------------------------------------------------------
typedef enum
{
	SBAGHIT_NON = 0,	// 0 通行可能 
	SBAGHIT_HIT,		// 1 通行不可
	SBAGHIT_NOTV,		// 2 縦方向不可
	SBAGHIT_NOTH,		// 3 横方向不可
	SBAGHIT_JOINT,		// 4 ジョイント
	SBAGHIT_NOTL,		// 5 左移動不可
}SANDBAGHIT;

//--------------------------------------------------------------
///	BAGHITBIT
//--------------------------------------------------------------
#define BAGHITBIT_NON			(0)
#define BAGHITBIT_WALL			(1<<0)
#define BAGHITBIT_BOX			(1<<1)
#define BAGHITBIT_JOINT_NOW		(1<<2)

//--------------------------------------------------------------
///	FGGYM_ACTPOS
//--------------------------------------------------------------
typedef struct
{
	int gx;
	int gz;
}FGGYM_ACTPOS;

//--------------------------------------------------------------
///	FGGYM_ACTWORK
//--------------------------------------------------------------
typedef struct
{
	int gx;
	int gz;
	FIELD_OBJ_PTR fldobj;
	EOA_PTR eoa;
}FGGYM_ACTWORK;

//--------------------------------------------------------------
///	SANDBAG_WORK
//--------------------------------------------------------------
typedef struct
{
	int use_flag;
	int seq_no;
	FGGYM_ACTWORK act;
}SANDBAG_WORK;

//--------------------------------------------------------------
///	BOX_WORK
//--------------------------------------------------------------
typedef struct
{
	int use_flag;
	int seq_no;
	FGGYM_ACTWORK act;
}BOX_WORK;

//--------------------------------------------------------------
///	PLFIGHT_GYM_LOCAL_WORK
//--------------------------------------------------------------
typedef struct
{
	FIELDSYS_WORK *fsys;
	BOX_WORK box_work[FGGYM_BOX_MAX];
	SANDBAG_WORK bag_work[FGGYM_BAG_MAX];
}PLFIGHT_GYM_LOCAL_WORK;

//--------------------------------------------------------------
///	FGGYM_SCRWORK
//--------------------------------------------------------------
typedef struct
{
	int seq_no;
	int ac;
	int gx;
	int gz;
	int init_gx;
	int init_gz;
	int target_gx;
	int target_gz;
	int dir;
	int count_max;
	int count;
	int end;
	FIELD_OBJ_PTR fldobj;
	FIELDSYS_WORK *fsys;
}FGGYM_SCRWORK;

//--------------------------------------------------------------
///	EV_PLFIGHT_GYM_BAG_MOVE
//--------------------------------------------------------------
typedef struct
{
	int seq_no;
	int frame;
	int dir;
	int gx;
	int gz;
	int old_count;
	int scr_flag;
	int move_grid;
	VecFx32 pos;
	fx32 speed_fx;
	fx32 count_fx;
	SANDBAG_WORK *bag;
	BOX_WORK *break_box;
	PLFIGHT_GYM_LOCAL_WORK *gym_l;
	FGGYM_SCRWORK scrwork;
}EV_PLFIGHT_GYM_BAG_MOVE;

//--------------------------------------------------------------
///	サンドバッグ初期位置
//--------------------------------------------------------------
static const FGGYM_ACTPOS DATA_SandbagPosTbl[FGGYM_BAG_MAX] =
{
	{3,12},
	{4,24},
	{8,9},
	{8,22},
	{15,28},
	{16,12},
	{20,19},
	{21,26},
	{23,13},
};

//--------------------------------------------------------------
///	箱初期位置
//--------------------------------------------------------------
static const FGGYM_ACTPOS DATA_BoxPosTbl[FGGYM_BOX_MAX] =
{
	{3,23},
	{8,12},
	{8,14},
	{8,16},
	{12,9},
	{12,12},
	{19,12},
	{20,13},
	{20,28},
	{22,9},
	{23,30},
};

//--------------------------------------------------------------
///	サンドバッグ通路
//--------------------------------------------------------------
static ALIGN4 const u8 DATA_SandbagHitMap[FGGYM_GRID_SIZE_XZ] =
{ //0             7               15               23               31
	1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,//0
	1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1, 0,0,0,4,0,0,0,0, 0,0,0,0,0,0,1,1, 1,1,1,1,1,1,1,1,//7
	1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,//8
//	1,1,1,0,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
	1,1,1,0,1,1,1,1, 1,0,0,0,0,4,0,0, 4,0,0,1,1,1,1,1, 1,1,1,1,1,1,1,1,
	1,1,1,0,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0, 1,1,1,1,1,1,1,1,
	1,1,1,3,5,0,0,0, 1,1,1,1,1,1,1,1, 1,1,1,1,0,1,1,0, 1,1,1,1,1,1,1,1,
	1,1,1,3,0,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,0,1,1,0, 1,1,1,1,1,1,1,1,
	1,1,1,0,0,0,0,0, 1,1,1,1,1,1,1,1, 1,1,1,1,0,1,1,0, 1,1,1,1,1,1,1,1,
	1,1,1,1,0,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,0,1,1,0, 1,1,1,1,1,1,1,1,//15
	1,1,1,1,0,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,3,0,1,0, 1,1,1,1,1,1,1,1,//16
	1,1,1,0,0,0,0,0, 0,1,1,1,1,1,1,1, 1,1,1,1,3,0,1,0, 1,1,1,1,1,1,1,1,
	1,1,1,0,0,1,1,1, 0,1,1,1,1,1,1,1, 1,1,1,1,1,0,1,0, 1,1,1,1,1,1,1,1,
	1,1,1,0,0,1,1,1, 0,1,1,1,1,1,1,1, 1,1,1,1,1,0,1,0, 1,1,1,1,1,1,1,1,
	1,1,1,0,0,0,0,0, 0,1,1,1,1,1,1,1, 1,1,1,1,1,4,1,0, 1,1,1,1,1,1,1,1,
	1,1,1,1,0,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,0,1,0, 1,1,1,1,1,1,1,1,
	1,1,1,1,0,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,0,1,0, 1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,0,1,0, 1,1,1,1,1,1,1,1,//23
	1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,0,1,0, 1,1,1,1,1,1,1,1,//24
	1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0, 1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0, 0,0,0,0,1,1,1,0, 1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0, 1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,//31
};

//--------------------------------------------------------------
///	方向別スクロール適用外の値
//--------------------------------------------------------------
static const int DATA_PLFightGymScrollValue[DIR_4_MAX] =
{
	BAGMOVE_SCRSTART_GRID_U,
	BAGMOVE_SCRSTART_GRID_D,
	BAGMOVE_SCRSTART_GRID_L,
	BAGMOVE_SCRSTART_GRID_R,
};

//--------------------------------------------------------------
//	static
//--------------------------------------------------------------
static void PLFightGym_ActInit( PLFIGHT_GYM_LOCAL_WORK *work );
static void PLFightGym_ActFldOBJRecover( PLFIGHT_GYM_LOCAL_WORK *work );

static FGGYM_EVRET (* const DATA_EvBagMoveTbl[SEQNO_BAGMOVE_MAX])(
		EV_PLFIGHT_GYM_BAG_MOVE * );

static void PLFightGym_ScrollInit( FIELDSYS_WORK *fsys,
	EV_PLFIGHT_GYM_BAG_MOVE *ev, FGGYM_ACTWORK *act, int dir );
static void PLFightGym_ScrollEnd( EV_PLFIGHT_GYM_BAG_MOVE *ev );
static BOOL PLFightGym_ScrollEndCheck( EV_PLFIGHT_GYM_BAG_MOVE *ev );
static BOOL PLFightGym_ScrollSetCheck(
		EV_PLFIGHT_GYM_BAG_MOVE *ev, FGGYM_ACTWORK *act, int dir );
static void PLFightGym_ScrollStart( EV_PLFIGHT_GYM_BAG_MOVE *ev, int flip );
static void PLFightGym_Scroll( EV_PLFIGHT_GYM_BAG_MOVE *ev, int ac );
static void PLFightGym_ScrollSpeed( EV_PLFIGHT_GYM_BAG_MOVE *ev, fx32 s_fx );

//--------------------------------------------------------------
/**
 * PL 格闘ジム:セットアップ
 * @param	fsys	FIELDSYS_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
void GYM_SetupPLFightGym( FIELDSYS_WORK *fsys )
{
	GIMMICKWORK *gmc;
	PLFIGHT_GYM_WORK *gym;
	PLFIGHT_GYM_LOCAL_WORK *work;
	
	gmc = SaveData_GetGimmickWork( GameSystem_GetSaveData(fsys) );
	gym = GIMMICKWORK_Get( gmc, FLD_GIMMICK_COMBAT_GYM );
	
	work = sys_AllocMemory( HEAPID_FIELD, sizeof(PLFIGHT_GYM_LOCAL_WORK) );
	memset( work, 0, sizeof(PLFIGHT_GYM_LOCAL_WORK) );
	fsys->fldmap->Work = work;
	
	work->fsys = fsys;
	
	FE_EffectRegist( fsys->fes, FE_FLD_GYM07EFF );
	FE_Gym07Eff_BoxSandbagMaxSet( fsys->fes, FGGYM_BOX_MAX, FGGYM_BAG_MAX );
	
	if( gym->init_flag == TRUE ){	//初期化済み
		OS_Printf( "格闘ジム アクター復帰\n" );
		PLFightGym_ActFldOBJRecover( work );
	}else{							//未初期化
		OS_Printf( "格闘ジム アクター初期化\n" );
		PLFightGym_ActInit( work );
	}
	
	gym->init_flag = TRUE;
}

//--------------------------------------------------------------
/**
 * PL 格闘ジム:終了処理
 * @param	fsys	FIELDSYS_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
void GYM_EndPLFightGym( FIELDSYS_WORK *fsys )
{
	PLFIGHT_GYM_LOCAL_WORK *work = fsys->fldmap->Work;
	sys_FreeMemoryEz( work );
	fsys->fldmap->Work = NULL;
}

//--------------------------------------------------------------
/**
 * PL 格闘ジム:格闘ジム用あたり判定
 * @param	fsys		フィールドシステムポインタ
 * @param	inGridX		グリッドX座標
 * @param	inGridZ		グリッドZ座標
 * @param	inHeight	高さ
 * @param	outHit		判定結果	TRUE:ヒット	FALSE:ヒットしてない
 * @return	BOOL		TRUE:このあと通常当たり判定を行わない	FALSE:通常当たり判定を行う
 */
//--------------------------------------------------------------
BOOL GYM_HitCheckPLFightGym( FIELDSYS_WORK *fsys,
							const int inGridX, const int inGridZ,
							const fx32 inHeight, BOOL *outHit	)
{
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * ジム仕掛け用フィールドOBJを追加
 * @param	fsys	FIELDSYS_WORK
 * @param	gx		グリッドX
 * @param	gz		グリッドZ
 * @param	type	ACTTYPE_BOX等
 * @retval	FIELD_OBJ_PTR 追加したFIELD_OBJ_PTR
 */
//--------------------------------------------------------------
static FIELD_OBJ_PTR PLFightGym_ActFldOBJAdd(
		FIELDSYS_WORK *fsys, int gx, int gz, FGGYM_ACTTYPE type )
{
	FIELD_OBJ_PTR fldobj;
	
	fldobj = FieldOBJ_AddHMake( fsys->fldobjsys,
			gx, gz, DIR_UP, NONDRAW, MV_DMY, fsys->location->zone_id );
	FieldOBJ_OBJIDSet( fldobj, FLDOBJ_ID_GIMMICK );
	FieldOBJ_ParamSet( fldobj, type, FLDOBJ_PARAM_0 );
	return( fldobj );
}

//--------------------------------------------------------------
/**
 * FIELD_OBJ_PTRからFGGYM_ACTWORK初期化
 * @param	work	FGGYM_ACTWORK
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	fsys	FIELDSYS_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void PLFightGym_ActWorkInit(
		FGGYM_ACTWORK *act, FIELD_OBJ_PTR fldobj, FIELDSYS_WORK *fsys )
{
	act->fldobj = fldobj;
	act->gx = FieldOBJ_NowPosGX_Get( fldobj );
	act->gz = FieldOBJ_NowPosGZ_Get( fldobj );
	
	if( FieldOBJ_ParamGet(fldobj,FLDOBJ_PARAM_0) == ACTTYPE_BOX ){
		act->eoa = FE_Gym07Eff_BoxAdd( fsys, act->gx, act->gz, 0 );
	}else{
		act->eoa = FE_Gym07Eff_SandbagAdd( fsys, act->gx, act->gz, 0 );
	}
}

//--------------------------------------------------------------
/**
 * サンドバッグ座標検索
 * @param	work	PLFIGHT_GYM_LOCAL_WORK
 * @param	gx			グリッドX
 * @param	gz			グリッドZ
 * @retval	SANDBAG_WORK	NULL=一致ナシ
 */
//--------------------------------------------------------------
static SANDBAG_WORK * PLFightGym_SandbagPosSearch(
		PLFIGHT_GYM_LOCAL_WORK *work, int gx, int gz )
{
	int i = 0;
	SANDBAG_WORK *bag = work->bag_work;
	
	do{
		if( bag->act.gx == gx && bag->act.gz == gz ){
			return( bag );
		}
		
		bag++;
		i++;
	}while( i < FGGYM_BAG_MAX );
	
	return( NULL );
}

//--------------------------------------------------------------
/**
 * 箱座標検索
 * @param	work	PLFIGHT_GYM_LOCAL_WORK
 * @param	gx			グリッドX
 * @param	gz			グリッドZ
 * @retval	BOX_WORK	NULL=一致ナシ
 */
//--------------------------------------------------------------
static BOX_WORK * PLFightGym_BoxPosSearch(
		PLFIGHT_GYM_LOCAL_WORK *work, int gx, int gz )
{
	int i = 0;
	BOX_WORK *box = work->box_work;
	
	do{
		if( box->use_flag ){
			if( box->act.gx == gx && box->act.gz == gz ){
				return( box );
			}
		}
		
		box++;
		i++;
	}while( i < FGGYM_BOX_MAX );
	
	return( NULL );
}

//--------------------------------------------------------------
/**
 * 箱削除
 * @param	BOX_WORK	削除するBOX_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void PLFightGym_BoxDelete( BOX_WORK *box )
{
	GF_ASSERT( box );
	GF_ASSERT( box->act.eoa );
	GF_ASSERT( box->act.fldobj );
	
	EOA_Delete( box->act.eoa );
	FieldOBJ_Delete( box->act.fldobj );
	box->use_flag = FALSE;
}

//--------------------------------------------------------------
/**
 * 箱、サンドバッグ初期化
 * @param	work	PLFIGHT_GYM_LOCAL_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void PLFightGym_ActInit( PLFIGHT_GYM_LOCAL_WORK *work )
{
	int i;
	FIELD_OBJ_PTR fldobj;
	const FGGYM_ACTPOS *pos;
	FIELDSYS_WORK *fsys = work->fsys;
	BOX_WORK *box = work->box_work;
	SANDBAG_WORK *bag = work->bag_work;
	
	memset( (u8*)box, 0, sizeof(BOX_WORK)*FGGYM_BOX_MAX );
	memset( (u8*)bag, 0, sizeof(SANDBAG_WORK)*FGGYM_BAG_MAX );
	
	for( i = 0, pos = DATA_BoxPosTbl; i < FGGYM_BOX_MAX; i++, pos++, box++ ){
		fldobj = PLFightGym_ActFldOBJAdd( fsys,
			pos->gx + FGGYM_ACT_GRID_OFFS_X,
			pos->gz + FGGYM_ACT_GRID_OFFS_Z, ACTTYPE_BOX );
		PLFightGym_ActWorkInit( &box->act, fldobj, fsys );
		box->use_flag = TRUE;
	}
	
	for( i = 0, pos = DATA_SandbagPosTbl; i < FGGYM_BAG_MAX; i++, pos++, bag++  ){
		fldobj = PLFightGym_ActFldOBJAdd( fsys,
			pos->gx + FGGYM_ACT_GRID_OFFS_X,
			pos->gz + FGGYM_ACT_GRID_OFFS_Z, ACTTYPE_BAG );
		PLFightGym_ActWorkInit( &bag->act, fldobj, fsys );
		bag->use_flag = TRUE;
	}
}

//--------------------------------------------------------------
/**
 * ジム仕掛け用フィールドOBJから復元
 * @param	fsys	FIELDSYS_WORK
 * @param	type	ACTTYPE_BOX等
 * @retval	FIELD_OBJ_PTR 追加したFIELD_OBJ_PTR
 */
//--------------------------------------------------------------
static void PLFightGym_ActFldOBJRecover( PLFIGHT_GYM_LOCAL_WORK *work )
{
	int objno = 0,check = 0;
	FIELD_OBJ_PTR fldobj;
	FIELDSYS_WORK *fsys = work->fsys;
	FIELD_OBJ_SYS_PTR fos = fsys->fldobjsys;
	BOX_WORK *box = work->box_work;
	SANDBAG_WORK *bag = work->bag_work;
	
	while( FieldOBJSys_FieldOBJSearch(
		fos,&fldobj,&objno,FLDOBJ_STA_BIT_USE) == TRUE ){
		if( FieldOBJ_OBJIDGet(fldobj) == FLDOBJ_ID_GIMMICK ){
			if( FieldOBJ_ParamGet(fldobj,FLDOBJ_PARAM_0) == ACTTYPE_BOX ){
				PLFightGym_ActWorkInit( &box->act, fldobj, fsys );
				box->use_flag = TRUE;
				box++;
			}else{
				PLFightGym_ActWorkInit( &bag->act, fldobj, fsys );
				bag->use_flag = TRUE;
				bag++;
			}
			
			check++;
			GF_ASSERT( check <= FGGYM_ACT_MAX );
		}
	}
}

//--------------------------------------------------------------
/**
 * サンドバッグあたり判定取得
 * @param	x	グリッドX座標
 * @param	z	グリッドZ座標
 * @retval	SANDBAGHIT	SANDBAGHIT
 */
//--------------------------------------------------------------
static SANDBAGHIT SandbagHitFlagGet( int gx, int gz )
{
	GF_ASSERT( gx >= 0 );
	GF_ASSERT( gx < FGGYM_GRID_SIZE_X );
	GF_ASSERT( gz >= 0 );
	GF_ASSERT( gz < FGGYM_GRID_SIZE_Z );
	
	{
		SANDBAGHIT flag = DATA_SandbagHitMap[gx+(gz*FGGYM_GRID_SIZE_X)];
		return( flag );
	}
}

//--------------------------------------------------------------
/**
 * サンドバッグ移動可能チェック
 * @param	work	PLFIGHT_GYM_LOCAL_WORK
 * @param	gx	現グリッドX座標
 * @param	gz	現グリッドZ座標
 * @param	dir	方向
 * @retval	u32 	ヒットビット　BAGHITBIT
 */
//--------------------------------------------------------------
static u32 SandbagMoveHitCheck(
		PLFIGHT_GYM_LOCAL_WORK *work, int gx, int gz, int dir )
{
	u32 ret = BAGHITBIT_NON;
	SANDBAGHIT b_flag,a_flag;
	
	b_flag = SandbagHitFlagGet( gx, gz );		//現在位置
	
	switch( b_flag ){
	case SBAGHIT_JOINT:
		ret |= BAGHITBIT_JOINT_NOW;
		break;
	case SBAGHIT_NOTV:	//縦方向移動不可
		if( dir == DIR_UP || dir == DIR_DOWN ){
			ret |= BAGHITBIT_WALL;
		}
		break;
	case SBAGHIT_NOTH:	//横方向移動不可
		if( dir == DIR_LEFT || dir == DIR_RIGHT ){
			ret |= BAGHITBIT_WALL;
		}
		break;
	case SBAGHIT_NOTL:	//左方向移動不可
		if( dir == DIR_LEFT ){
			ret |= BAGHITBIT_WALL;
		}
		break;
	}
	
	gx += FieldOBJ_DirAddValueGX( dir );		//移動先
	gz += FieldOBJ_DirAddValueGZ( dir );
	a_flag = SandbagHitFlagGet( gx, gz );
	
	if( a_flag == SBAGHIT_HIT ){
		ret |= BAGHITBIT_WALL;
	}
	
	if( PLFightGym_BoxPosSearch(work,gx,gz) != NULL ){
		ret |= BAGHITBIT_BOX;
	}
	
	return( ret );
}

//--------------------------------------------------------------
/**
 * サンドバッググリッド移動数取得
 * @param	work	PLFIGHT_GYM_LOCAL_WORK
 * @param	gx	現グリッドX座標
 * @param	gz	現グリッドZ座標
 * @param	dir	方向
 * @param	count	移動数格納先
 * @retval	u32 ヒットビット	BAGHITBIT
 */
//--------------------------------------------------------------
static u32 SandbagMoveGridGet(
		PLFIGHT_GYM_LOCAL_WORK *work, int gx, int gz, int dir, int *count )
{
	u32 hit;
	
	*count = 0;
	hit = SandbagMoveHitCheck( work, gx, gz, dir );
	
	if( hit != BAGHITBIT_NON && hit != BAGHITBIT_JOINT_NOW ){	//初回のみ無視
		return( hit );
	}
	
	do{
		gx += FieldOBJ_DirAddValueGX( dir );
		gz += FieldOBJ_DirAddValueGZ( dir );
		(*count)++;
		hit = SandbagMoveHitCheck( work, gx, gz, dir );
	}while( hit == BAGHITBIT_NON );
	
	return( hit );
}

//--------------------------------------------------------------
/**
 * PL 格闘ジム:イベント　サンドバッグ揺れ
 * @param	event	GMEVENT_CONTROL
 * @retval	BOOL	TRUE=終了
 */
//--------------------------------------------------------------
static BOOL GMEVENT_PLFightGymSandbagShake( GMEVENT_CONTROL *event )
{
	FIELD_OBJ_PTR p_obj;
	FIELDSYS_WORK *fsys = FieldEvent_GetFieldSysWork( event );
	EV_PLFIGHT_GYM_BAG_MOVE *work = FieldEvent_GetSpecialWork( event );
	PLFIGHT_GYM_LOCAL_WORK *gym_l = work->gym_l;
	
	p_obj = Player_FieldOBJGet( fsys->player );
	
	switch( work->seq_no ){
	case 0:
		if( FieldOBJ_AcmdSetCheck(p_obj) == TRUE ){
			FieldOBJ_AcmdSet( p_obj,
				FieldOBJ_AcmdCodeDirChange(work->dir,AC_STAY_WALK_U_2F) );
			FE_Gym07Eff_SandbagShakeSet(
				work->bag->act.eoa, work->dir, FALSE );
			//Snd_SePlay( SEQ_SE_DP_UG_026 );	//サンドバッグ揺れSE
			Snd_SePlay( SEQ_SE_PL_SUTYA2 );		//サンドバッグ揺れSE
			work->seq_no++;
		}
		
		break;
	case 1:
		if( FieldOBJ_AcmdEndCheck(p_obj) == TRUE &&
			FE_Gym07Eff_SandbagAnmEndCheck(work->bag->act.eoa) == TRUE ){
			FieldOBJ_AcmdEnd( p_obj );
			sys_FreeMemoryEz( work );
			return( TRUE );
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * PL 格闘ジム:イベント　サンドバッグ移動
 * @param	event	GMEVENT_CONTROL
 * @retval	BOOL	TRUE=終了
 */
//--------------------------------------------------------------
static BOOL GMEVENT_PLFightGymSandbagMove( GMEVENT_CONTROL *event )
{
	FGGYM_EVRET ret;
	EV_PLFIGHT_GYM_BAG_MOVE *work;
	
	work = FieldEvent_GetSpecialWork( event );
	
	do{
		ret = DATA_EvBagMoveTbl[work->seq_no]( work );
	}while( ret == FGGYM_EVRET_NEXT );
	
	if( ret == FGGYM_EVRET_END ){
		return( TRUE );
	}
	
	return( FALSE );
}
		
//--------------------------------------------------------------
/**
 * PL 格闘ジム:イベント　サンドバッグ移動　初期化
 * @param	work	EV_PLFIGHT_GYM_BAG_MOVE
 * @retval	FGGYM_EVRET	FGGYM_EVRET
 */
//--------------------------------------------------------------
static FGGYM_EVRET EvBagMove_Init( EV_PLFIGHT_GYM_BAG_MOVE *work )
{
	u32 hit;
	FGGYM_ACTWORK *act = &work->bag->act;
	int gx = act->gx;
	int gz = act->gz;
	
	SandbagMoveGridGet( work->gym_l, gx, gz, work->dir, &work->move_grid );
	
	EOA_MatrixGet( act->eoa, &work->pos );
	work->speed_fx = SANDBAG_START_SPEED_FX;
	work->seq_no = SEQNO_BAGMOVE_ACMD_SET;
	return( FGGYM_EVRET_NON );	//div_map.cの自機移動完了待ち
}

//--------------------------------------------------------------
/**
 * PL 格闘ジム:イベント　サンドバッグ移動　自機アニメセット
 * @param	work	EV_PLFIGHT_GYM_BAG_MOVE
 * @retval	FGGYM_EVRET	FGGYM_EVRET
 */
//--------------------------------------------------------------
static FGGYM_EVRET EvBagMove_AcmdSet( EV_PLFIGHT_GYM_BAG_MOVE *work )
{
	FIELD_OBJ_PTR fldobj;
	
	fldobj = Player_FieldOBJGet( work->gym_l->fsys->player );
	
	if( FieldOBJ_AcmdSetCheck(fldobj) == FALSE ){
		return( FGGYM_EVRET_NON );
	}
	
	FieldOBJ_AcmdSet( fldobj,
		FieldOBJ_AcmdCodeDirChange(work->dir,AC_STAY_WALK_U_16F) );
	
	{
		SANDBAG_WORK *bag = work->bag;
		EOA_PTR eoa = bag->act.eoa;
		FE_Gym07Eff_SandbagShakeSet( eoa, work->dir, TRUE );
		
		if( PLFightGym_ScrollSetCheck(work,&bag->act,work->dir) == TRUE ){
			work->scr_flag = TRUE;
			PLFightGym_ScrollInit(
				work->gym_l->fsys, work, &bag->act, work->dir );
			PLFightGym_ScrollStart( work, FALSE );
		}
	}
	
	//Snd_SePlay( SEQ_SE_DP_UG_026 );	//サンドバッグ揺れSE
	Snd_SePlay( SEQ_SE_PL_SUTYA2 );		//サンドバッグ揺れSE
	
	work->seq_no = SEQNO_BAGMOVE_SHAKE_START;
	return( FGGYM_EVRET_NEXT );
}

//--------------------------------------------------------------
/**
 * PL 格闘ジム:イベント　サンドバッグ移動　最初のゆれ
 * @param	work	EV_PLFIGHT_GYM_BAG_MOVE
 * @retval	FGGYM_EVRET	FGGYM_EVRET
 */
//--------------------------------------------------------------
static FGGYM_EVRET EvBagMove_ShakeStart( EV_PLFIGHT_GYM_BAG_MOVE *work )
{
	FieldOBJTool_VecPosDirAdd( work->dir, &work->pos, work->speed_fx );
	EOA_MatrixSet( work->bag->act.eoa, &work->pos );
	
	work->speed_fx += SANDBAG_START_SPEED_FX;
	work->count_fx += work->speed_fx;
	work->frame++;
	
	if( work->frame >= SANDBAG_START_FRAME ){
		fx32 s = NUM_FX32( SANDBAG_MOVE_ANM_FRAME );
		s /= work->move_grid*GRID/2;
		FE_Gym07Eff_SandbagAnmSpeedSet( work->bag->act.eoa, s );
		work->seq_no = SEQNO_BAGMOVE_MOVE;
		Snd_SePlay( SEQ_SE_PL_GYM01 );		//バッグ　レール移動SE
		OS_Printf( "サンドバッグスロウ speed=0x%x\n", s );
	}
	
	return( FGGYM_EVRET_NON );
}

//--------------------------------------------------------------
/**
 * PL 格闘ジム:イベント　サンドバッグ移動　移動
 * @param	work	EV_PLFIGHT_GYM_BAG_MOVE
 * @retval	FGGYM_EVRET	FGGYM_EVRET
 */
//--------------------------------------------------------------
static FGGYM_EVRET EvBagMove_Move( EV_PLFIGHT_GYM_BAG_MOVE *work )
{
	SANDBAG_WORK *bag = work->bag;
	
	FieldOBJTool_VecPosDirAdd( work->dir, &work->pos, work->speed_fx );
	EOA_MatrixSet( bag->act.eoa, &work->pos );
	
	work->count_fx += work->speed_fx;
	work->frame++;
	
	if( work->frame < SANDBAG_ACCEL_FRAME ){
		if( work->speed_fx < FX32_ONE ){
			work->speed_fx += work->speed_fx;
		}
	}else if( work->frame == SANDBAG_ACCEL_FRAME ){
		if( work->speed_fx <= (FX32_ONE*2) ){
			work->speed_fx = FX32_ONE*2;
		}
	}
	
	PLFightGym_ScrollSpeed( work, work->speed_fx );
	
	if( SIZE_GRID_FX32(work->count_fx) > work->old_count ){	//1G超え
		u32 ret;
		
		work->old_count = SIZE_GRID_FX32( work->count_fx );
		bag->act.gx += FieldOBJ_DirAddValueGX( work->dir );
		bag->act.gz += FieldOBJ_DirAddValueGZ( work->dir );
		
		ret = SandbagMoveHitCheck(
				work->gym_l, bag->act.gx, bag->act.gz, work->dir );
		
		if( ret == BAGHITBIT_NON ){
#if 0
			if( work->old_count > 1 && work->speed_fx < NUM_FX32(4) ){
				work->speed_fx <<= 1;
			}
#endif
		}else{	//障害物ヒット
			FieldOBJTool_GridCenterPosGet(
				bag->act.gx, bag->act.gz, &work->pos );
			FieldOBJ_NowPosGX_Set( bag->act.fldobj, bag->act.gx );
			FieldOBJ_NowPosGZ_Set( bag->act.fldobj, bag->act.gz );
			FieldOBJ_GPosUpdate( bag->act.fldobj );
			FE_Gym07Eff_SandbagAnmSpeedSet( bag->act.eoa, FX32_ONE );
			
			Snd_SeStopBySeqNo( SEQ_SE_PL_GYM01, 0 );		//バッグ　レール移動SE停止
			//Snd_SePlay( SEQ_SE_DP_UG_026 );				//サンドバッグ揺れSE
			Snd_SePlay( SEQ_SE_PL_SUTYA2 );					//サンドバッグ揺れSE
			Snd_SePlay( SEQ_SE_PL_GYM02 );					//金属で止まるSE
			
			if( (ret&BAGHITBIT_BOX) == 0 ){
				if( work->scr_flag == TRUE ){
					work->seq_no = SEQNO_BAGMOVE_SCROLL_WAIT;
				}else{
					work->seq_no = SEQNO_BAGMOVE_SHAKE_WAIT;
				}
			}else{
				work->break_box = PLFightGym_BoxPosSearch( work->gym_l,
					bag->act.gx + FieldOBJ_DirAddValueGX(work->dir),
					bag->act.gz + FieldOBJ_DirAddValueGZ(work->dir) );
				GF_ASSERT( work->break_box != NULL );
				work->frame = 0;
				work->seq_no = SEQNO_BAGMOVE_BREAK_BOX_START;
			}
		}
	}
	
	return( FGGYM_EVRET_NON );
}

//--------------------------------------------------------------
/**
 * PL 格闘ジム:イベント　サンドバッグ移動　箱破壊開始
 * @param	work	EV_PLFIGHT_GYM_BAG_MOVE
 * @retval	FGGYM_EVRET	FGGYM_EVRET
 */
//--------------------------------------------------------------
static FGGYM_EVRET EvBagMove_BreakBoxStart( EV_PLFIGHT_GYM_BAG_MOVE *work )
{
	PLFightGym_ScrollSpeed( work, work->speed_fx );
	work->frame++;
	
	if( work->frame >= BOX_BREAK_START_FRAME ){
		work->frame = 0;
		work->seq_no = SEQNO_BAGMOVE_BREAK_BOX;
		FE_Gym07Eff_BoxBreakAnmSet( work->break_box->act.eoa );
		//Snd_SePlay( SEQ_SE_DP_W173C );		//破壊SE
		Snd_SePlay( SEQ_SE_DP_UG_026 );			//破壊SE
	}
	
	return( FGGYM_EVRET_NON );
}

//--------------------------------------------------------------
/**
 * PL 格闘ジム:イベント　サンドバッグ移動　箱破壊
 * @param	work	EV_PLFIGHT_GYM_BAG_MOVE
 * @retval	FGGYM_EVRET	FGGYM_EVRET
 */
//--------------------------------------------------------------
static FGGYM_EVRET EvBagMove_BreakBox( EV_PLFIGHT_GYM_BAG_MOVE *work )
{
	PLFightGym_ScrollSpeed( work, work->speed_fx );
	
	if( FE_Gym07Eff_BoxBreakAnmCheck(work->break_box->act.eoa) ){
		PLFightGym_BoxDelete( work->break_box );
		work->frame = 0;
		
		if( work->scr_flag == TRUE ){
			work->seq_no = SEQNO_BAGMOVE_SCROLL_WAIT;
		}else{
			work->seq_no = SEQNO_BAGMOVE_SHAKE_WAIT;
		}
	}
	
	return( FGGYM_EVRET_NON );
}

//--------------------------------------------------------------
/**
 * PL 格闘ジム:イベント　サンドバッグ移動　サンドバッグスクロール終了待ち
 * @param	work	EV_PLFIGHT_GYM_BAG_MOVE
 * @retval	FGGYM_EVRET	FGGYM_EVRET
 */
//--------------------------------------------------------------
static FGGYM_EVRET EvBagMove_ScrollWait( EV_PLFIGHT_GYM_BAG_MOVE *work )
{
	PLFightGym_ScrollSpeed( work, work->speed_fx );
	
	if( PLFightGym_ScrollEndCheck(work) == TRUE &&
		FE_Gym07Eff_SandbagAnmEndCheck(work->bag->act.eoa) == TRUE ){
		PLFightGym_ScrollStart( work, TRUE );
		work->seq_no = SEQNO_BAGMOVE_FLIP_SCROLL_WAIT;
	}
	
	return( FGGYM_EVRET_NON );
}

//--------------------------------------------------------------
/**
 * PL 格闘ジム:イベント　サンドバッグ移動　反転スクロール終了待ち
 * @param	work	EV_PLFIGHT_GYM_BAG_MOVE
 * @retval	FGGYM_EVRET	FGGYM_EVRET
 */
//--------------------------------------------------------------
static FGGYM_EVRET EvBagMove_FlipScrollWait( EV_PLFIGHT_GYM_BAG_MOVE *work )
{
	PLFightGym_Scroll( work, AC_WALK_U_4F );
		
	if( PLFightGym_ScrollEndCheck(work) == TRUE ){
		PLFightGym_ScrollEnd( work );
		work->seq_no = SEQNO_BAGMOVE_ACMD_WAIT;
	}
	
	return( FGGYM_EVRET_NON );
}

//--------------------------------------------------------------
/**
 * PL 格闘ジム:イベント　サンドバッグ移動　サンドバッグアニメ終了待ち
 * @param	work	EV_PLFIGHT_GYM_BAG_MOVE
 * @retval	FGGYM_EVRET	FGGYM_EVRET
 */
//--------------------------------------------------------------
static FGGYM_EVRET EvBagMove_ShakeWait( EV_PLFIGHT_GYM_BAG_MOVE *work )
{
	if(	FE_Gym07Eff_SandbagAnmEndCheck(work->bag->act.eoa) == TRUE ){
		work->seq_no = SEQNO_BAGMOVE_ACMD_WAIT;
	}
	
	return( FGGYM_EVRET_NON );
}

//--------------------------------------------------------------
/**
 * PL 格闘ジム:イベント　サンドバッグ移動　自機アニメ終了待ち
 * @param	work	EV_PLFIGHT_GYM_BAG_MOVE
 * @retval	FGGYM_EVRET	FGGYM_EVRET
 */
//--------------------------------------------------------------
static FGGYM_EVRET EvBagMove_AcmdWait( EV_PLFIGHT_GYM_BAG_MOVE *work )
{
	FIELD_OBJ_PTR fldobj;
	fldobj = Player_FieldOBJGet( work->gym_l->fsys->player );
	
	if( FieldOBJ_AcmdEndCheck(fldobj) == TRUE ){
		FieldOBJ_AcmdEnd( fldobj );
		work->seq_no = SEQNO_BAGMOVE_END;
	}
	
	return( FGGYM_EVRET_NON );
}

//--------------------------------------------------------------
/**
 * PL 格闘ジム:イベント　サンドバッグ移動　終了
 * @param	work	EV_PLFIGHT_GYM_BAG_MOVE
 * @retval	FGGYM_EVRET	FGGYM_EVRET
 */
//--------------------------------------------------------------
static FGGYM_EVRET EvBagMove_End( EV_PLFIGHT_GYM_BAG_MOVE *work )
{
	sys_FreeMemoryEz( work );
	return( FGGYM_EVRET_END );
}

//--------------------------------------------------------------
///	PL 格闘ジムサンドバッグ移動イベント処理テーブル
//--------------------------------------------------------------
static FGGYM_EVRET (* const DATA_EvBagMoveTbl[SEQNO_BAGMOVE_MAX])(
		EV_PLFIGHT_GYM_BAG_MOVE * ) =
{
	EvBagMove_Init,
	EvBagMove_AcmdSet,
	EvBagMove_ShakeStart,
	EvBagMove_Move,
	EvBagMove_BreakBoxStart,
	EvBagMove_BreakBox,
	EvBagMove_ScrollWait,
	EvBagMove_FlipScrollWait,
	EvBagMove_ShakeWait,
	EvBagMove_AcmdWait,
	EvBagMove_End,
};
		
//--------------------------------------------------------------
/**
 * PL 草ジム　イベントセット共通部分
 * @param	gym_l	PL_GRASS_GYM_LOCAL_WORK
 * @param	bag		SANDBAG_WORK
 * @param	dir		押した方向　DIR_UP等
 * @param	EV_PLFIGHT_GYM_BAG_MOVE* EV_PLFIGHT_GYM_BAG_MOVE
 */
//--------------------------------------------------------------
static EV_PLFIGHT_GYM_BAG_MOVE * PLFightGym_EvWorkInit(
		PLFIGHT_GYM_LOCAL_WORK *gym_l, SANDBAG_WORK *bag, int dir )
{
	EV_PLFIGHT_GYM_BAG_MOVE *work;
	
	work = sys_AllocMemoryLo( HEAPID_WORLD, sizeof(EV_PLFIGHT_GYM_BAG_MOVE) );
	GF_ASSERT( work != NULL );
	memset( work, 0, sizeof(EV_PLFIGHT_GYM_BAG_MOVE) );
	
	work->dir = dir;
	work->gx = bag->act.gx;
	work->gz = bag->act.gz;
	work->bag = bag;
	work->gym_l = gym_l;
	
	return( work );
}

//--------------------------------------------------------------
/**
 * PL 草ジム　サンドバッグ揺れイベントセット
 * @param	fsys	FIELDSYS_WORK
 * @param	gym_l	PL_GRASS_GYM_LOCAL_WORK
 * @param	bag		SANDBAG_WORK
 * @param	dir		押した方向　DIR_UP等
 * @retval	nothing
 */
//--------------------------------------------------------------
static void PLFightGym_SandbagShakeEventSet( FIELDSYS_WORK *fsys,
		PLFIGHT_GYM_LOCAL_WORK *gym_l, SANDBAG_WORK *bag, int dir )
{
	EV_PLFIGHT_GYM_BAG_MOVE *work = PLFightGym_EvWorkInit( gym_l, bag, dir );
	FieldEvent_Set( fsys, GMEVENT_PLFightGymSandbagShake, work );
}

//--------------------------------------------------------------
/**
 * PL 格闘ジム　サンドバッグ移動イベントセット
 * @param	fsys	FIELDSYS_WORK
 * @param	gym_l	PL_GRASS_GYM_LOCAL_WORK
 * @param	bag		SANDBAG_WORK
 * @param	dir		押した方向　DIR_UP等
 * @retval	nothing
 */
//--------------------------------------------------------------
static void PLFightGym_SandbagMoveEventSet( FIELDSYS_WORK *fsys,
		PLFIGHT_GYM_LOCAL_WORK *gym_l, SANDBAG_WORK *bag, int dir )
{
	EV_PLFIGHT_GYM_BAG_MOVE *work = PLFightGym_EvWorkInit( gym_l, bag, dir );
	FieldEvent_Set( fsys, GMEVENT_PLFightGymSandbagMove, work );
}

//--------------------------------------------------------------
/**
 * PL 格闘ジム:サンドバッグチェック
 * @param	fsys	FIELDSYS_WORK
 * @retval	BOOL	TRUE=イベント起動 FALSE=無し
 */
//--------------------------------------------------------------
BOOL GYM_PLFightGymEventHitCheck( FIELDSYS_WORK *fsys )
{
	GIMMICKWORK *gim;
	
	gim = SaveData_GetGimmickWork( GameSystem_GetSaveData(fsys) );
	
	if( GIMMICKWORK_GetAssignID(gim) != FLD_GIMMICK_COMBAT_GYM ){
		return( FALSE );
	}
	
	{
		int gx,gz,count;
		SANDBAG_WORK *bag;
		int dir = Player_DirGet( fsys->player );
		PLFIGHT_GYM_LOCAL_WORK *work = fsys->fldmap->Work;
		
		Player_DirNextGridPosGet( fsys->player, dir, &gx, &gz );
		bag = PLFightGym_SandbagPosSearch( work, gx, gz );
		
		if( bag == NULL ){
			return( FALSE );
		}
		
//		gx += FieldOBJ_DirAddValueGX( dir );
//		gz += FieldOBJ_DirAddValueGZ( dir );
		
		SandbagMoveGridGet( work, gx, gz, dir, &count );
		
		if( count == 0 ){
			PLFightGym_SandbagShakeEventSet( fsys, work, bag, dir );
		}else{											//障害物無し
			PLFightGym_SandbagMoveEventSet( fsys, work, bag, dir );
		}
	}
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * PL 格闘ジム:画面スクロールセット
 * @param	fsys	FIELDSYS_WORK
 * @param	ev	EV_PLFIGHT_GYM_BAG_MOVE
 * @param	act	スクロール対象となるFGGYM_ACTWORK
 * @param	dir	スクロールする方向
 * @retval	nothing
 */
//--------------------------------------------------------------
static void PLFightGym_ScrollInit( FIELDSYS_WORK *fsys,
	EV_PLFIGHT_GYM_BAG_MOVE *ev, FGGYM_ACTWORK *act, int dir )
{
	int gx,gz;
	const VecFx32 *pos;
	FGGYM_SCRWORK *work;
	
	work = &ev->scrwork;
	memset( work, 0, sizeof(FGGYM_SCRWORK) );
	
	work->fsys = fsys;
	gx = act->gx;
	gz = act->gz;
	work->gx = gx;
	work->gz = gz;
	work->dir = dir;
	
	SandbagMoveGridGet( ev->gym_l, gx, gz, dir, &work->count_max );
	GF_ASSERT( work->count_max );
	
	work->target_gx = gx + (FieldOBJ_DirAddValueGX(dir)*work->count_max);
	work->target_gz = gz + (FieldOBJ_DirAddValueGZ(dir)*work->count_max);
	
	OS_Printf( "格闘ジム　サンドバックスクロール%d\n", work->count_max );
	
	gx = Player_NowGPosXGet( fsys->player );
	gz = Player_NowGPosZGet( fsys->player );
	work->init_gx = gx;
	work->init_gz = gz;
	
	work->fldobj = FieldOBJ_AddHMake( fsys->fldobjsys,
		gx, gz, DIR_UP, NONDRAW, MV_DMY, fsys->location->zone_id );
	
	FieldOBJ_VecPosNowHeightGetSet( work->fldobj );
	FieldOBJ_StatusBitSet_Vanish( work->fldobj, TRUE );
	FieldOBJ_StatusBitSet_FellowHit( work->fldobj, FALSE );
	FieldOBJ_StatusBitSet_HeightGetOFF( work->fldobj, TRUE );
	
	pos = FieldOBJ_VecPosPtrGet( work->fldobj );
	DivMapBindTarget( pos, fsys->map_cont_dat );
	GFC_BindCameraTarget( pos, fsys->camera_ptr );
}

//--------------------------------------------------------------
/**
 * PL 格闘ジム:画面スクロール終了
 * @param	ev	EV_PLFIGHT_GYM_BAG_WORK
 * @retval	int	TRUE=終了
 */
//--------------------------------------------------------------
static void PLFightGym_ScrollEnd( EV_PLFIGHT_GYM_BAG_MOVE *ev )
{
	FGGYM_SCRWORK *work = &ev->scrwork;
	const VecFx32 *pos = Player_VecPosPtrGet( work->fsys->player );
	
	#ifdef PM_DEBUG
	{
		VecFx32 spos;
		FieldOBJ_VecPosGet( work->fldobj, &spos );
		OS_Printf( "格闘ジム スクロール終了" );
		OS_Printf( "SX=0x%x,PX=0x%x,", spos.x, pos->x );
		OS_Printf( "PZ=0x%x,PZ=0x%x\n", spos.z, pos->z );
	}
	#endif
	
	DivMapBindTarget( pos, work->fsys->map_cont_dat );
	GFC_BindCameraTarget( pos, work->fsys->camera_ptr );
	FieldOBJ_Delete( work->fldobj );
}

//--------------------------------------------------------------
/**
 * PL 格闘ジム:画面スクロール終了チェック
 * @param	ev	EV_PLFIGHT_GYM_BAG_WORK
 * @retval	int	TRUE=終了
 */
//--------------------------------------------------------------
static BOOL PLFightGym_ScrollEndCheck( EV_PLFIGHT_GYM_BAG_MOVE *ev )
{
	FGGYM_SCRWORK *work = &ev->scrwork;
	return( work->end );
}

//--------------------------------------------------------------
/**
 * PL 格闘ジム:画面スクロール適用チェック
 * @param	ev		PLFIGHT_GYM_BAG_MOVE
 * @param	act		スクロール対象となるFGGYM_ACTWORK
 * @param	dir		スクロールする方向
 * @retval	BOOL	TRUE=スクロール適用範囲
 */
//--------------------------------------------------------------
static BOOL PLFightGym_ScrollSetCheck(
		EV_PLFIGHT_GYM_BAG_MOVE *ev, FGGYM_ACTWORK *act, int dir )
{
	int count;
	int gx = act->gx;
	int gz = act->gz;
	
	SandbagMoveGridGet( ev->gym_l, gx, gz, dir, &count );
	
	if( count > DATA_PLFightGymScrollValue[dir] ){
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * PL 格闘ジム:画面スクロール開始
 * @param	ev	EV_PLFIGHT_GYM_BAG_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void PLFightGym_ScrollStart( EV_PLFIGHT_GYM_BAG_MOVE *ev, int flip )
{
	FGGYM_SCRWORK *work = &ev->scrwork;
	work->count = 0;
	work->seq_no = 1;
	work->end = FALSE;
	
	if( flip == TRUE ){
		work->dir = FieldOBJTool_DirFlip( work->dir );
	}
}

//--------------------------------------------------------------
/**
 * PL 格闘ジム:画面スクロール
 * @param	ev	EV_PLFIGHT_GYM_BAG_MOVE
 * @retval	nothing
 */
//--------------------------------------------------------------
static void PLFightGym_Scroll( EV_PLFIGHT_GYM_BAG_MOVE *ev, int ac )
{
	FGGYM_SCRWORK *work = &ev->scrwork;
	
	switch( work->seq_no ){
	case 0:		//待機
		break;
	case 1:
		if( FieldOBJ_AcmdSetCheck(work->fldobj) == TRUE ){
			if( work->count >= work->count_max ){
				work->seq_no++;
			}else{
				ac = FieldOBJ_AcmdCodeDirChange( work->dir, ac );
				FieldOBJ_AcmdSet( work->fldobj, ac );
				work->count++;
			}
		}
		break;
	case 2:		//div_mapへ反映される間
		work->end = TRUE;
		work->seq_no = 0;
		break;
	}
}

//--------------------------------------------------------------
/**
 * 速度からアニメコードを選びスクロール処理
 * @param	ev		EV_PLFIGHT_GYM_BAG_MOVE
 * @param	s_fx	移動速度
 * @retval	nothing
 */
//--------------------------------------------------------------
static void PLFightGym_ScrollSpeed( EV_PLFIGHT_GYM_BAG_MOVE *ev, fx32 s_fx )
{
	if( ev->scr_flag == TRUE ){
		int speed = FX32_NUM( s_fx );
#if 1
		if( speed ){
			int ac = AC_WALK_U_16F;
			if( speed <= 2 ){ ac = AC_WALK_U_8F; }
			else if( speed > 2 ){ ac = AC_WALK_U_4F; }
			PLFightGym_Scroll( ev, ac );
		}
#else
		PLFightGym_Scroll( ev, AC_WALK_U_32F );
#endif
	}
}

//==============================================================================
//	PL ゴーストジム　kaga
//==============================================================================
//--------------------------------------------------------------
//	debug
//--------------------------------------------------------------
//#define DEBUG_PLG_LIGHT_CHECK
//#define DEBUG_PLG_WINON		//定義でウィンドウON
//#define DEBUG_PLG_OBJWIN	//定義でOBJウィンドウON
//#define DEBUG_PLG_FOG_OFF		//定義でフォグOFF

//--------------------------------------------------------------
//	define
//--------------------------------------------------------------
#define PLG_OWND_OUT_MASK (GX_WND_PLANEMASK_BG1|GX_WND_PLANEMASK_BG2|GX_WND_PLANEMASK_BG3|GX_WND_PLANEMASK_OBJ)

#define PLG_OWND_IN_MASK (GX_WND_PLANEMASK_BG0|GX_WND_PLANEMASK_BG1|GX_WND_PLANEMASK_BG2|GX_WND_PLANEMASK_BG3|GX_WND_PLANEMASK_OBJ)

#define PLG_CLACT_TRANS_MAX (8)

#define PLG_LIGHT_RANGE_MAX (6)
#define PLG_LIGHT_MOVE_SPEED_FX (FX32_ONE)
#define PLG_LIGHT_SCALE_SPEED_FX (0x00c0)

#define PLG_LIGHT_RANGE_DEF (2) //ライトデフォルト距離

#define PLG_LOFF_TBL_MAX (0xffffffff)

#define PLGGYM_HINTM0_GRID_SIZE_X (15)
#define PLGGYM_HINTM0_GRID_SIZE_Z (8)
#define PLGGYM_HINTM0_GRID_SIZE_XZ (PLGGYM_HINTM0_GRID_SIZE_X*PLGGYM_HINTM0_GRID_SIZE_Z)
#define PLGGYM_HINTM1_GRID_SIZE_X (27)
#define PLGGYM_HINTM1_GRID_SIZE_Z (20)
#define PLGGYM_HINTM1_GRID_SIZE_XZ (PLGGYM_HINTM1_GRID_SIZE_X*PLGGYM_HINTM1_GRID_SIZE_Z)

//--------------------------------------------------------------
//	FOG
//--------------------------------------------------------------
#define PLG_FOG_TBL_MAX (32)
//#define PLG_FOG_TBL_DEF (126)
#define PLG_FOG_TBL_DEF_0 (109)
#define PLG_FOG_TBL_DEF_1 (119)
#define PLG_FOG_TBL_DEF_AFTER (91)
#define PLG_FOG_SLOPE (10)
#define PLG_FOG_COL_R (1)
#define PLG_FOG_COL_G (1)
#define PLG_FOG_COL_B (1)
#define PLG_FOG_ALPHA (1)

//--------------------------------------------------------------
///	セルアクター数
//--------------------------------------------------------------
#define	PLG_CLACT_MAX (16)
#define PLG_ACT_MAX (PLG_CLACT_MAX)

//--------------------------------------------------------------
///	セルキャラリソース数
//--------------------------------------------------------------
enum
{
	PLG_CLACT_RES_CHAR_LIGHT,
	PLG_CLACT_RES_CHAR_MAX,
};

//--------------------------------------------------------------
///	セルパレットリソース数
//--------------------------------------------------------------
enum
{
	PLG_CLACT_RES_PLTT_LIGHT,
	PLG_CLACT_RES_PLTT_MAX,
};

//--------------------------------------------------------------
///	セルリソース数
//--------------------------------------------------------------
enum
{
	PLG_CLACT_RES_CELL_LIGHT,
	PLG_CLACT_RES_CELL_MAX,
};

//--------------------------------------------------------------
///	アニメリソース数
//--------------------------------------------------------------
enum
{
	PLG_CLACT_RES_ANM_LIGHT,
	PLG_CLACT_RES_ANM_MAX,
};

//--------------------------------------------------------------
///	TCBプライオリティオフセット
//--------------------------------------------------------------
enum
{
	PLG_TCBOFFS_SYS = 1,
	PLG_TCBOFFS_ACT,
	PLG_TCBOFFS_EOA,
	PLG_TCBOFFS_DRAW,
};

//--------------------------------------------------------------
///	ステージ数	*STAGE04はリーダー部屋
//--------------------------------------------------------------
enum
{
	PLG_STAGE_01,
	PLG_STAGE_02,
	PLG_STAGE_03,
	PLG_STAGE_MAX,
};

//--------------------------------------------------------------
///	ヒントが置かれるステージ
//--------------------------------------------------------------
enum
{
	PLG_STAGE_HINT_0,
	PLG_STAGE_HINT_1,
	PLG_STAGE_HINT_MAX,
};

//--------------------------------------------------------------
///	ヒント番号
//--------------------------------------------------------------
enum
{
	PLG_HINT_0,
	PLG_HINT_1,
	PLG_HINT_2,
	PLG_HINT_3,
	PLG_HINT_4,
	PLG_HINT_5,
	PLG_HINT_6,
	PLG_HINT_7,
	PLG_HINT_MAX,
};

//--------------------------------------------------------------
///	PLG_HINTDATA
//--------------------------------------------------------------
typedef struct
{
	int zone_id;
	int start_no;
	int max_no;
	int start_gx;
	int start_gz;
	int size_gx;
	int size_gz;
}PLG_HINTDATA;

//--------------------------------------------------------------
///	PLG_FOGDATA
//--------------------------------------------------------------
typedef struct
{
	FOG_DATA_PTR fog;
	int slope;
	u8 col_r;
	u8 col_g;
	u8 col_b;
	u8 col_a;
	ALIGN4 char fog_tbl[PLG_FOG_TBL_MAX];
}PLG_FOGDATA;

//--------------------------------------------------------------
///	PLG_EXIT
//--------------------------------------------------------------
typedef struct
{
	int hint_no;
	s16 x;
	s16 z;
}PLG_EXIT;

//--------------------------------------------------------------
///	PLG_CLACT_SETDATA
//--------------------------------------------------------------
typedef struct
{
	CLACT_SET_PTR clact_set;
	CLACT_U_EASYRENDER_DATA clact_easydata;
	CLACT_U_RES_MANAGER_PTR clact_resm_char;
	CLACT_U_RES_MANAGER_PTR clact_resm_pltt;
	CLACT_U_RES_MANAGER_PTR clact_resm_cell;
	CLACT_U_RES_MANAGER_PTR clact_resm_anm;
	CLACT_U_RES_OBJ_PTR	clact_resobj_char[PLG_CLACT_RES_CHAR_MAX];
	CLACT_U_RES_OBJ_PTR	clact_resobj_pltt[PLG_CLACT_RES_PLTT_MAX];
	CLACT_U_RES_OBJ_PTR	clact_resobj_cell[PLG_CLACT_RES_CELL_MAX];
	CLACT_U_RES_OBJ_PTR	clact_resobj_anm[PLG_CLACT_RES_ANM_MAX];
//	NNSG2dCellTransferState *cellTransArray;
	TCB_PTR tcb_clact_draw;
}PLG_CLACT_SETDATA;

typedef struct _TAG_PLGHOST_GYM_LOCAL_WORK PLGHOST_GYM_LOCAL_WORK;

//--------------------------------------------------------------
///	PLG_LIGHT_ACT_WORK
//--------------------------------------------------------------
typedef struct
{
	int use;
	int dir;
	int range;
	TCB_PTR tcb;
	EOA_PTR eoa;
	FIELD_OBJ_PTR fldobj;
	PLGHOST_GYM_LOCAL_WORK *gyml;
	
#ifdef DEBUG_PLG_OBJWIN	
	VecFx32 offs;
	VecFx32 scale_fx;
	CLACT_WORK_PTR clact;
	#ifdef DEBUG_PLG_LIGHT_CHECK
	int debug_mode;
	#endif
#endif
}PLG_LIGHT_ACT_WORK;

//--------------------------------------------------------------
///	PLGHOST_GYM_LOCAL_WORK
//--------------------------------------------------------------
typedef struct _TAG_PLGHOST_GYM_LOCAL_WORK
{
	FIELDSYS_WORK *fsys;
	PLG_LIGHT_ACT_WORK act_light[PLG_ACT_MAX];
	PLG_FOGDATA fogdata;
	
	u32 jiki_fog_off;
	TCB_PTR tcb_proc;
	
#ifdef DEBUG_PLG_OBJWIN	
	PLG_CLACT_SETDATA clact_setdata;
#endif
};

//--------------------------------------------------------------
//	proto
//--------------------------------------------------------------
static void PlgProcTcb( TCB_PTR tcb, void *wk );

static void PlgFog_Init( PLGHOST_GYM_LOCAL_WORK *gyml );

#ifdef DEBUG_PLG_OBJWIN
static void PlgClActSetData_Init( PLGHOST_GYM_LOCAL_WORK *gyml );
static void PlgClActSetData_Delete( PLGHOST_GYM_LOCAL_WORK *gyml );
static void PlgClActSetData_DrawTcb( TCB_PTR tcb, void *wk );

//static void PlgClActSetData_VBlankTrans( TCB_PTR tcb, void *wk );
#endif

static void PlgLightAct_Init( PLGHOST_GYM_LOCAL_WORK *gyml );
static void PlgLightAct_Delete( PLGHOST_GYM_LOCAL_WORK *gyml );
static void PlgLightAct_Tcb( TCB_PTR tcb, void *wk );

#if 0
static int PlgJikiHitRangeGet(
	PLGHOST_GYM_LOCAL_WORK *gyml, FIELD_OBJ_PTR fldobj );
static void PlgTargetAdd( fx32 *val, fx32 tar, fx32 add );
static void PlgTargetAddVecFx32(
		VecFx32 *vec, const VecFx32 *tar, fx32 add );
#endif

#ifdef DEBUG_PLG_OBJWIN
static void PlgLightActPosSet( PLG_LIGHT_ACT_WORK *act );
#endif

#ifdef DEBUG_PLG_LIGHT_CHECK
static void DEBUG_PlgLightCheck( PLG_LIGHT_ACT_WORK *act );
#endif

#ifdef DEBUG_PLG_OBJWIN
static const VecFx32 DATA_PlgLightRangeTbl[DIR_4_MAX][PLG_LIGHT_RANGE_MAX];
static const u32 DATA_PlgLightRotateTbl[DIR_4_MAX];
static const VecFx32 DATA_PlgLightPosTbl[DIR_4_MAX][PLG_LIGHT_RANGE_MAX];
#endif

//--------------------------------------------------------------
///	PL ゴーストジム:ヒントデータ
//--------------------------------------------------------------
static const PLG_HINTDATA DATA_PlgHintDataTbl[PLG_STAGE_HINT_MAX] =
{
	{ZONE_ID_C05GYM0102,0,3,1,3,15,8},
	{ZONE_ID_C05GYM0103,3,5,1,3,27,20},
};

//--------------------------------------------------------------
///	PL ゴーストジム:ヒント配置マップ
//--------------------------------------------------------------
static ALIGN4 const u8 DATA_PlgHintMap0Tbl[PLGGYM_HINTM0_GRID_SIZE_XZ] =
{
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,
	0,1,0,0,0,1,1,1,1,1,0,0,1,0,0,
	0,1,0,0,0,1,1,1,1,1,0,0,1,0,0,
	0,1,1,1,0,1,0,0,0,0,0,1,1,1,0,
	0,1,1,1,0,0,1,1,1,0,0,1,1,1,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};

static ALIGN4 const u8 DATA_PlgHintMap1Tbl[PLGGYM_HINTM1_GRID_SIZE_XZ] =
{
	0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
	0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
	0,1,1,1,1,1,0,0,0,1,1,1,0,1,0,1,0,0,0,1,1,0,0,0,0,1,0,
	0,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,1,1,1,1,0,
	0,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,1,1,1,1,0,
	0,0,0,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,0,0,0,
	0,0,0,1,0,0,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,0,1,0,0,0,
	1,1,1,0,1,1,1,1,1,0,0,0,0,1,0,0,0,0,1,1,1,1,1,0,0,0,0,
	1,1,1,0,1,1,1,1,1,0,0,0,0,1,0,0,0,0,1,1,1,1,1,0,0,0,0,
	0,0,0,0,0,1,1,1,0,0,1,1,1,0,1,1,1,0,0,1,1,1,1,1,0,0,0,
	0,0,0,0,0,1,1,1,0,0,1,1,1,0,1,1,1,0,0,1,1,1,1,1,0,0,0,
	0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,1,1,1,1,0,1,1,1,1,1,1,
	0,0,0,0,0,0,0,1,1,1,1,0,0,1,0,0,1,1,1,1,0,1,1,1,1,1,1,
	0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,
	0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,0,0,0,
	1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,0,1,1,1,0,0,1,0,0,1,1,1,0,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,0,1,1,1,0,0,1,0,0,1,1,1,0,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,0,0,1,0,0,1,1,1,0,0,1,0,0,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};

static const u8 * const DATA_PlgHintMapTbl[PLG_STAGE_HINT_MAX] =
{
	DATA_PlgHintMap0Tbl,
	DATA_PlgHintMap1Tbl,
};

//--------------------------------------------------------------
///	ヒントNoとドア接続
//--------------------------------------------------------------
static const PLG_EXIT DATA_PlgExitData0[] =
{
	{ 0, 4, 2 },
	{ 1, 8, 2 },
	{ 2, 12, 2 },
	{ PLG_HINT_MAX, 0, 0 },//終端
};

static const PLG_EXIT DATA_PlgExitData1[] =
{
	{ 3, 4, 2 },
	{ 4, 9, 2 },
	{ 5, 14, 2 },
	{ 6, 19, 2 },
	{ 7, 24, 2 },
	{ PLG_HINT_MAX, 0, 0 },//終端
};

static const PLG_EXIT * const DATA_PlgExitDataTbl[PLG_STAGE_HINT_MAX] =
{
	DATA_PlgExitData0,
	DATA_PlgExitData1,
};

//--------------------------------------------------------------
/**
 * PL ゴーストジム:セットアップ
 * @param	fsys	FIELDSYS_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
void GYM_SetupPLGhostGym( FIELDSYS_WORK *fsys )
{
	GIMMICKWORK *gmc;
	PLGHOST_GYM_WORK *gym;
	PLGHOST_GYM_LOCAL_WORK *work;
	
	gmc = SaveData_GetGimmickWork( GameSystem_GetSaveData(fsys) );
	gym = GIMMICKWORK_Get( gmc, FLD_GIMMICK_GHOST_GYM );
	
	work = sys_AllocMemory( HEAPID_FIELD, sizeof(PLGHOST_GYM_LOCAL_WORK) );
	memset( work, 0, sizeof(PLGHOST_GYM_LOCAL_WORK) );
	work->fsys = fsys;
	fsys->fldmap->Work = work;
	
#ifdef DEBUG_PLG_WINON
	GX_SetVisibleWnd( GX_WNDMASK_OW );
	G2_SetWndOBJInsidePlane( PLG_OWND_IN_MASK, TRUE );
	G2_SetWndOutsidePlane( PLG_OWND_OUT_MASK, TRUE );
#endif
	
#ifndef DEBUG_PLG_FOG_OFF
	PlgFog_Init( work );
#endif
	
	FE_EffectRegist( fsys->fes, FE_FLD_GYM05EFF );
	
	//影をつけない
	FieldOBJSys_ShadowJoinSet( fsys->fldobjsys, FALSE );
	
	{	//主人公をFOG無効タイプに
		int code = GHOSTHERO;
		PLAYER_STATE_PTR jiki = fsys->player;
		FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( jiki );
		if( Player_SexGet(fsys->player) == PM_FEMALE ){
			code = GHOSTHEROINE;
		}
		FieldOBJ_DrawReset( fldobj, code );
	}
	
	{	//ギョエーをFOG無効に
		FE_Gyoe_NotFogSet( fsys, TRUE );
	}
	
#ifdef DEBUG_PLG_OBJWIN
	PlgClActSetData_Init( work );
#endif
	
	{	//ライトアクター初期化
		PlgLightAct_Init( work );
	}
	
	{	//ヒントパネル初期化
		int stage = 0;
		int zone_id = fsys->location->zone_id;
		const PLG_HINTDATA *hint = DATA_PlgHintDataTbl;
		
		do{
			if( hint->zone_id == zone_id ){ break; }
			hint++;
			stage++;
		}while(	stage < PLG_STAGE_HINT_MAX );
		
		if( gym->init_flag == FALSE ){
			gym->init_flag = TRUE;
			gym->hint_no = GYM_PLGHOST_HINT_MAX;
			
			if( stage != PLG_STAGE_HINT_MAX ){			//ヒント有り
				const u8 *hit = DATA_PlgHintMapTbl[stage];	//GX,GZ
				int rnd = gf_mtRand() % hint->max_no;		//ヒントNo
				gym->hint_no = rnd + hint->start_no;
				
				do{
					do{
						gym->hint_gx = gf_mtRand() % hint->size_gx;
						gym->hint_gz = gf_mtRand() % hint->size_gz;
					}while( hit[(gym->hint_gz*hint->size_gx)+gym->hint_gx] == 1 );
					gym->hint_gx += hint->start_gx;
					gym->hint_gz += hint->start_gz;
				}while( FieldOBJ_SearchGridPos(fsys->fldobjsys,gym->hint_gx,gym->hint_gz,FALSE) != NULL );
				
				OS_Printf("HINT GX=%d,GZ=%d\n",gym->hint_gx,gym->hint_gz);
				GF_ASSERT(gym->hint_no<(hint->start_no+hint->max_no) );
				GF_ASSERT(gym->hint_gx<(hint->start_gx+hint->size_gx));
				GF_ASSERT(gym->hint_gz<(hint->start_gz+hint->size_gz));
				GF_ASSERT(GetHitAttr(fsys,gym->hint_gx,gym->hint_gz)==0);
			}
		}
		
		if( gym->hint_no != GYM_PLGHOST_HINT_MAX ){		//ヒント有り
			int id;
			const PLG_EXIT *tbl = DATA_PlgExitDataTbl[stage];
			
			while( tbl->hint_no != PLG_HINT_MAX ){	//外れドア設定
				if( tbl->hint_no != gym->hint_no ){	//当たり以外のドア
					id = EventData_SearchNowConnectIDByXZ(
							fsys, tbl->x, tbl->z );
					EventData_ConnectDataZoneChange(
							fsys, id, ZONE_ID_C05GYM0101 );
					EventData_ConnectDataDoorChange(
							fsys, id, C05GYM0101_MAT02 );
				}
				tbl++;
			}
			
			FE_Gym05Eff_HintPanelAdd(
				fsys->fes, gym->hint_gx, gym->hint_gz, gym->hint_no );
		}
	}
	
	work->tcb_proc = TCB_Add( PlgProcTcb, work, 0 );
}

//--------------------------------------------------------------
/**
 * PL ゴーストジム:終了処理
 * @param	fsys
 * @retval	nothing
 */
//--------------------------------------------------------------
void GYM_EndPLGhostGym( FIELDSYS_WORK *fsys )
{
	PLGHOST_GYM_LOCAL_WORK *work = fsys->fldmap->Work;
	
	TCB_Delete( work->tcb_proc );
	
#ifdef DEBUG_PLG_OBJWIN	
	PlgClActSetData_Delete( work );
#endif
	
	PlgLightAct_Delete( work );
	sys_FreeMemoryEz( work );
	fsys->fldmap->Work = NULL;
}

//--------------------------------------------------------------
/**
 * PL ゴーストジム:常駐処理
 * @param	tcb		TCB_PTR
 * @param	wk		tcb work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void PlgProcTcb( TCB_PTR tcb, void *wk )
{
//FieldOBJ BlActレベルでの対応の為、不要となる
#if 0
	PLGHOST_GYM_LOCAL_WORK *work = wk;
	
	if( work->jiki_fog_off == FALSE ){
		FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( work->fsys->player );
		BLACT_WORK_PTR act = FieldOBJ_DrawBlAct00_BlActPtrGet( fldobj );
		if( act != NULL ){
			NNSG3dResMdl *mdl = BLACT_MdlResGet( act );
			NNS_G3dMdlSetMdlFogEnableFlagAll( mdl, 0 );
			work->jiki_fog_off = TRUE;
		}
	}
#endif
}

//--------------------------------------------------------------
/**
 * PL ゴーストジム:フォグ初期化
 * @param	gyml	PLGHOST_GYM_LOCAL_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void PlgFog_Init( PLGHOST_GYM_LOCAL_WORK *gyml )
{
	PLG_FOGDATA *fogdata = &gyml->fogdata;
	FOG_DATA_PTR fog = gyml->fsys->fog_data;
	
	fogdata->fog = fog;
	fogdata->slope = PLG_FOG_SLOPE;
	fogdata->col_r = PLG_FOG_COL_R;
	fogdata->col_g = PLG_FOG_COL_G;
	fogdata->col_b = PLG_FOG_COL_B;
	fogdata->col_a = PLG_FOG_ALPHA;
	
	{
		char fog = PLG_FOG_TBL_DEF_0;
		
		BOOL ret =  MyStatus_GetBadgeFlag(
			SaveData_GetMyStatus(gyml->fsys->savedata), BADGE_ID_C05 );
		
		if( ret == TRUE ){
			fog = PLG_FOG_TBL_DEF_AFTER;
		}else if( gyml->fsys->location->zone_id == ZONE_ID_C05GYM0103 ){
			fog = PLG_FOG_TBL_DEF_1;
		}
		
		memset( fogdata->fog_tbl, fog, PLG_FOG_TBL_MAX );
	}
	
	SetFogData( fog, FOG_SYS_ALL, TRUE, 0, fogdata->slope, 0 );
	
	SetFogColor( fog,
		FOG_SYS_ALL,
		GX_RGB(fogdata->col_r,fogdata->col_g,fogdata->col_b),
		fogdata->col_a );
	
	SetFogTbl( fog, fogdata->fog_tbl );
}

//--------------------------------------------------------------
//	DEBUG:フォグテスト
//--------------------------------------------------------------
#ifdef PM_DEBUG
u8 * DEBUG_GYM_PLGhostGym_FogRedGet( FIELDSYS_WORK *fsys )
{
	PLGHOST_GYM_LOCAL_WORK *gyml = fsys->fldmap->Work;
	PLG_FOGDATA *fogdata = &gyml->fogdata;
	return( &fogdata->col_r );
}

u8 * DEBUG_GYM_PLGhostGym_FogGreenGet( FIELDSYS_WORK *fsys )
{
	PLGHOST_GYM_LOCAL_WORK *gyml = fsys->fldmap->Work;
	PLG_FOGDATA *fogdata = &gyml->fogdata;
	return( &fogdata->col_g );
}

u8 * DEBUG_GYM_PLGhostGym_FogBlueGet( FIELDSYS_WORK *fsys )
{
	PLGHOST_GYM_LOCAL_WORK *gyml = fsys->fldmap->Work;
	PLG_FOGDATA *fogdata = &gyml->fogdata;
	return( &fogdata->col_b );
}

u8 * DEBUG_GYM_PLGhostGym_FogAlphaGet( FIELDSYS_WORK *fsys )
{
	PLGHOST_GYM_LOCAL_WORK *gyml = fsys->fldmap->Work;
	PLG_FOGDATA *fogdata = &gyml->fogdata;
	return( &fogdata->col_a );
}

char DEBUG_GYM_PLGhostGym_FogTblGet( FIELDSYS_WORK *fsys )
{
	PLGHOST_GYM_LOCAL_WORK *gyml = fsys->fldmap->Work;
	PLG_FOGDATA *fogdata = &gyml->fogdata;
	return( fogdata->fog_tbl[0] );
}

void DEBUG_GYM_PLGhostGym_FogTblSet( FIELDSYS_WORK *fsys, char param )
{
	int i;
	PLGHOST_GYM_LOCAL_WORK *gyml = fsys->fldmap->Work;
	PLG_FOGDATA *fogdata = &gyml->fogdata;
	char *tbl = fogdata->fog_tbl;
	for( i = 0; i < PLG_FOG_TBL_MAX; *tbl = param, tbl++, i++ );
}

void DEBUG_GYM_PLGhostGym_FogSet( FIELDSYS_WORK *fsys )
{
	PLGHOST_GYM_LOCAL_WORK *gyml = fsys->fldmap->Work;
	PLG_FOGDATA *fogdata = &gyml->fogdata;
	FOG_DATA_PTR fog = gyml->fsys->fog_data;
	
	SetFogColor( fog,
		FOG_SYS_ALL,
		GX_RGB(fogdata->col_r,fogdata->col_g,fogdata->col_b),
		fogdata->col_a );
	SetFogTbl( fog, fogdata->fog_tbl );
}
#endif

//--------------------------------------------------------------
/**
 * PL ゴーストジム:セルアクターセット初期化
 * @param	PLGHOST_GYM_LOCAL_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
#ifdef DEBUG_PLG_OBJWIN
static void PlgClActSetData_Init( PLGHOST_GYM_LOCAL_WORK *gyml )
{
	PLG_CLACT_SETDATA *set = &gyml->clact_setdata;
	
	set->clact_set = CLACT_U_SetEasyInit(
			PLG_CLACT_MAX, &set->clact_easydata, HEAPID_FIELD );
	
//	set->cellTransArray = InitCellTransfer(PLG_CLACT_TRANS_MAX, HEAPID_FIELD);
//	initVramTransferManagerHeap( PLG_CLACT_TRANS_MAX, HEAPID_FIELD );
	
	set->clact_resm_char = CLACT_U_ResManagerInit(
			PLG_CLACT_RES_CHAR_MAX, CLACT_U_CHAR_RES, HEAPID_FIELD );
	set->clact_resm_pltt = CLACT_U_ResManagerInit(
			PLG_CLACT_RES_PLTT_MAX, CLACT_U_PLTT_RES, HEAPID_FIELD );
	set->clact_resm_cell = CLACT_U_ResManagerInit(
			PLG_CLACT_RES_CELL_MAX, CLACT_U_CELL_RES, HEAPID_FIELD );
	set->clact_resm_anm = CLACT_U_ResManagerInit(
			PLG_CLACT_RES_ANM_MAX, CLACT_U_CELLANM_RES, HEAPID_FIELD );
	
	{	//リソースセット
		int i;
		ARCHANDLE *hd;
		hd = ArchiveDataHandleOpen( ARC_PLGYM_GHOST, HEAPID_FIELD );
		
		for( i = 0; i < PLG_CLACT_RES_CHAR_MAX; i++ ){
			set->clact_resobj_char[i] =
				CLACT_U_ResManagerResAddArcChar_ArcHandle(
				set->clact_resm_char, hd,
				NARC_plgym_ghost_plgym_ghost_light_NCGR, FALSE,
				i,
				NNS_G2D_VRAM_TYPE_2DMAIN, HEAPID_FIELD );
			CLACT_U_CharManagerSetAreaCont( set->clact_resobj_char[i] );
		}
		
		for( i = 0; i < PLG_CLACT_RES_PLTT_MAX; i++ ){
			set->clact_resobj_pltt[i] =
				CLACT_U_ResManagerResAddArcPltt_ArcHandle(
					set->clact_resm_pltt, hd,
					NARC_plgym_ghost_plgym_ghost_light_NCLR, FALSE,
					i,
					NNS_G2D_VRAM_TYPE_2DMAIN, 1, HEAPID_FIELD );
			CLACT_U_PlttManagerSetCleanArea( set->clact_resobj_pltt[i] );
		}
		
		CLACT_U_ResManagerResAddArcKindCell_ArcHandle(
			set->clact_resm_cell, hd,
			NARC_plgym_ghost_plgym_ghost_light_NCER, FALSE,
			PLG_CLACT_RES_CELL_LIGHT, 
			CLACT_U_CELL_RES, HEAPID_FIELD );
		
		CLACT_U_ResManagerResAddArcKindCell_ArcHandle(
			set->clact_resm_anm, hd,
			NARC_plgym_ghost_plgym_ghost_light_NANR, FALSE,
			PLG_CLACT_RES_ANM_LIGHT,
			CLACT_U_CELLANM_RES, HEAPID_FIELD );
			
		ArchiveDataHandleClose( hd );
	}

	{	//描画用TCB
		int pri;
		pri = FieldOBJSys_TCBStandardPriorityGet( gyml->fsys->fldobjsys ) + PLG_TCBOFFS_DRAW;
		set->tcb_clact_draw = TCB_Add( PlgClActSetData_DrawTcb, gyml, pri );
	}
}
#endif

//--------------------------------------------------------------
/**
 * PL ゴーストジム:セルアクターセット削除
 * @param	gyml	PLGHOST_GYM_LOCAL_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
#ifdef DEBUG_PLG_OBJWIN
static void PlgClActSetData_Delete( PLGHOST_GYM_LOCAL_WORK *gyml )
{
	int i;
	PLG_CLACT_SETDATA *set = &gyml->clact_setdata;
	
	TCB_Delete( set->tcb_clact_draw );
	
	for( i = 0; i < PLG_CLACT_RES_CHAR_MAX; i++ ){
		if( set->clact_resobj_char[i] != NULL ){
			CLACT_U_CharManagerDelete( set->clact_resobj_char[i] );
		}
	}
	
	for( i = 0; i < PLG_CLACT_RES_PLTT_MAX; i++ ){
		if( set->clact_resobj_pltt[i] != NULL ){
			CLACT_U_PlttManagerDelete( set->clact_resobj_pltt[i] );
		}
	}
	
	for( i = 0; i < PLG_CLACT_RES_CELL_MAX; i++ ){
		if( set->clact_resobj_cell[i] != NULL ){
			CLACT_U_ResManagerResOnlyDelete( set->clact_resobj_cell[i] );
			CLACT_U_ResManagerResDelete(
				set->clact_resm_cell, set->clact_resobj_cell[i] );
		}
	}
	
	for( i = 0; i < PLG_CLACT_RES_ANM_MAX; i++ ){
		if( set->clact_resobj_anm[i] != NULL ){
			CLACT_U_ResManagerResOnlyDelete( set->clact_resobj_anm[i] );
			CLACT_U_ResManagerResDelete(
				set->clact_resm_anm, set->clact_resobj_anm[i] );
		}
	}
	
	CLACT_U_ResManagerDelete( set->clact_resm_char );
	CLACT_U_ResManagerDelete( set->clact_resm_pltt );
	CLACT_U_ResManagerDelete( set->clact_resm_cell );
	CLACT_U_ResManagerDelete( set->clact_resm_anm );
	
//	DellVramTransferManager();
//	DeleteCellTransfer( set->cellTransArray );
	
	CLACT_DelAllSet( set->clact_set );
	CLACT_DestSet( set->clact_set );
}
#endif

//--------------------------------------------------------------
/**
 * PL ゴーストジム：セルアクター描画
 * @param	tcb	TCB_PTR
 * @param	wk	PLGHOST_GYM_LOCAL_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
#ifdef DEBUG_PLG_OBJWIN
static void PlgClActSetData_DrawTcb( TCB_PTR tcb, void *wk )
{
	PLGHOST_GYM_LOCAL_WORK *gyml = wk;
	PLG_CLACT_SETDATA *set = &gyml->clact_setdata;
	CLACT_Draw( set->clact_set );
}
#endif

#if 0
//--------------------------------------------------------------
/**
 * PL ゴーストジム:セルリソースVブランク転送
 * @param	tcb	TCB_PTR
 * @param	wk	PLGHOST_GYM_LOCAL_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void PlgClActSetData_VBlankTrans( TCB_PTR tcb, void *wk )
{
	int i;
	PLGHOST_GYM_LOCAL_WORK *gyml = wk;
	PLG_CLACT_SETDATA *set = &gyml->clact_setdata;
	
	for( i = 0; i < PLG_CLACT_RES_CHAR_MAX; i++ ){
		CLACT_U_CharManagerSetAreaCont( set->clact_resobj_char[i] );
	}
	
	for( i = 0; i < PLG_CLACT_RES_PLTT_MAX; i++ ){
		CLACT_U_PlttManagerSetCleanArea( set->clact_resobj_pltt[i] );
	}
	
	TCB_Delete( tcb );
}
#endif

//--------------------------------------------------------------
/**
 * PL ゴーストジム：セルアクター追加
 * @param	set	PLG_CLACT_SETDATA
 * @param	pos	位置
 * @param	char_id	キャラID
 * @param	pltt_id	パレットID
 * @param	cell_id	セルID
 * @param	anm_id	アニメID
 * @retval	CLACT_WORK_PTR	追加されたCLACT_WORK_PTR
 */
//--------------------------------------------------------------
#ifdef DEBUG_PLG_OBJWIN
static CLACT_WORK_PTR PlgClActAdd(
		PLG_CLACT_SETDATA *set, const VecFx32 *pos,
		u32 char_id, u32 pltt_id, u32 cell_id, u32 anm_id,
		int bg_pri, int act_pri )
{
	CLACT_HEADER head;
	CLACT_ADD_SIMPLE add;
	CLACT_WORK_PTR act;
	
	CLACT_U_MakeHeader(
		&head, char_id, pltt_id, cell_id, anm_id,
		CLACT_U_HEADER_DATA_NONE, CLACT_U_HEADER_DATA_NONE,
		FALSE,
		bg_pri,
		set->clact_resm_char,
		set->clact_resm_pltt,
		set->clact_resm_cell,
		set->clact_resm_anm,
		NULL, NULL );
	
	add.ClActSet = set->clact_set;
	add.ClActHeader = &head;
	add.mat = *pos;
	add.pri = act_pri;
	add.DrawArea = NNS_G2D_VRAM_TYPE_2DMAIN;
	add.heap = HEAPID_FIELD;
	
	act = CLACT_AddSimple( &add );
	GF_ASSERT( act != NULL );
	return( act );
}
#endif

//--------------------------------------------------------------
/**
 * PL ライトアクター初期化
 * @param	gyml	PLGHOST_GYM_LOCAL_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void PlgLightAct_Init( PLGHOST_GYM_LOCAL_WORK *gyml )
{
	int no,count,dir,pri,id;
	FIELD_OBJ_PTR fldobj;
	PLG_LIGHT_ACT_WORK *act = gyml->act_light;
	CONST_FIELD_OBJ_SYS_PTR fos = gyml->fsys->fldobjsys;
	
	no = 0;
	count = 0;
	pri = FieldOBJSys_TCBStandardPriorityGet( fos ) + PLG_TCBOFFS_ACT;
	
	while( FieldOBJSys_FieldOBJSearch(fos,&fldobj,&no,FLDOBJ_STA_BIT_USE) ){
		id = FieldOBJ_OBJIDGet( fldobj );
		act->range = FieldOBJ_ParamGet( fldobj, FLDOBJ_PARAM_0 );
#if 1	//formal
		if( id == FLDOBJ_ID_PLAYER ||
			(FieldOBJ_EventTypeGet(fldobj) ==
			 EV_TYPE_TRAINER && act->range) ){
#else	//test
		if( id == FLDOBJ_ID_PLAYER ){
#endif			
			act->use = TRUE;
			act->dir = FieldOBJ_DirDispGet( fldobj );
			act->fldobj = fldobj;
			act->gyml = gyml;
			
			if( id == FLDOBJ_ID_PLAYER ){
				act->range = PLG_LIGHT_RANGE_DEF;
			}
			
			#ifdef DEBUG_PLG_OBJWIN
			act->scale_fx = DATA_PlgLightRangeTbl[act->dir][act->range];
			act->offs = DATA_PlgLightPosTbl[act->dir][act->range];
		
			act->clact = PlgClActAdd( &gyml->clact_setdata, &act->offs,
					PLG_CLACT_RES_CHAR_LIGHT,PLG_CLACT_RES_PLTT_LIGHT,
					PLG_CLACT_RES_CELL_LIGHT,PLG_CLACT_RES_ANM_LIGHT,
					0, 0 );
	
			CLACT_ObjModeSet( act->clact, GX_OAM_MODE_OBJWND );
			CLACT_SetAnmFlag( act->clact, 1 );
			CLACT_SetScaleAffine(
				act->clact, &act->scale_fx, CLACT_AFFINE_DOUBLE );
			CLACT_SetRotation( act->clact, DATA_PlgLightRotateTbl[act->dir] );
			#endif
		
			act->eoa = FE_Gym05Eff_LightAdd(
				gyml->fsys->fes, fldobj, act->range, PLG_TCBOFFS_EOA );
		
			act->tcb = TCB_Add( PlgLightAct_Tcb, act, pri );
			GF_ASSERT( act->tcb );
		
			count++;
			GF_ASSERT( count < PLG_ACT_MAX );
		
			act++;
		}
	}
	
	OS_Printf( "ゴーストジム　ライトアクター数　%d\n", count );
}

//--------------------------------------------------------------
/**
 * PL ライトアクター削除
 * @param	gyml	PLGHOST_GYM_LOCAL_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void PlgLightAct_Delete( PLGHOST_GYM_LOCAL_WORK *gyml )
{
	int i;
	PLG_LIGHT_ACT_WORK *act = gyml->act_light;
	
	for( i = 0; i < PLG_ACT_MAX; i++, act++ ){
		if( act->use ){
			TCB_Delete( act->tcb );
			act->use = 0;
		}
	}
}

//--------------------------------------------------------------
/**
 * PL ライトアクタータスク
 * @param	tcb
 * @param	wk	tcb work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void PlgLightAct_Tcb( TCB_PTR tcb, void *wk )
{
#if 0	//test
	fx32 val;
	int range,dir;
	const VecFx32 *dat;
	PLG_LIGHT_ACT_WORK *act = wk;
	PLGHOST_GYM_LOCAL_WORK *gyml = act->gyml;
	FIELD_OBJ_PTR fldobj = act->fldobj;
	
	if( (sys.trg & PAD_BUTTON_SELECT) ){
		act->range++;
		act->range %= PLG_LIGHT_RANGE_MAX;
		OS_Printf( "ゴーストジム　ライト距離%d\n", act->range );
	}
	
	dir = FieldOBJ_DirDispGet( fldobj );
	range = PlgJikiHitRangeGet( gyml, fldobj );
	
	if( range > act->range ){
		range = act->range;
	}
	
	FE_Gym05Eff_LightRangeSet( act->eoa, range );
	
	#ifdef DEBUG_PLG_LIGHT_CHECK
	DEBUG_PlgLightCheck( act );
	return;
	#endif
	
	#ifdef DEBUG_PLG_OBJWIN
	dat = &DATA_PlgLightRangeTbl[dir][range];
	
	if( dir != act->dir ){
		act->scale_fx = *dat;
	}else{
		PlgTargetAddVecFx32( &act->scale_fx, dat, PLG_LIGHT_SCALE_SPEED_FX );
	}
	
	dat = &DATA_PlgLightPosTbl[dir][range];
	
	if( dir != act->dir ){
		act->offs = *dat;
	}else{
		PlgTargetAddVecFx32( &act->offs, dat, PLG_LIGHT_MOVE_SPEED_FX );
	}
	#endif
	
	act->dir = dir;
	
	#ifdef DEBUG_PLG_OBJWIN
	PlgLightActPosSet( act );
	#endif
#endif
}

#ifdef DEBUG_PLG_LIGHT_CHECK
static void DEBUG_PlgLightCheck( PLG_LIGHT_ACT_WORK *act )
{
	fx32 val;
	int dir;
	u32 trg = sys.trg;
	u32 cnt = sys.cont;
	PLGHOST_GYM_LOCAL_WORK *gyml = act->gyml;
	FIELD_OBJ_PTR fldobj = act->fldobj;
	
	dir = FieldOBJ_DirDispGet( fldobj );
	
	#ifdef DEBUG_PLG_OBJWIN
	if( act->dir != dir ){
		act->dir = dir;
		act->scale_fx = DATA_PlgLightRangeTbl[dir][act->range];
		act->offs = DATA_PlgLightPosTbl[dir][act->range];
	}
	
	if( cnt & PAD_BUTTON_L ){
		if( trg & PAD_BUTTON_B ){
			act->debug_mode++;
			act->debug_mode &= 0x01;
			if( act->debug_mode )OS_Printf( "モード　拡縮\n" );
			else OS_Printf( "モード 座標\n" );
		}
		
		if( act->debug_mode == 0 ){
			if( cnt & PAD_KEY_UP ){
				act->offs.y -= NUM_FX32( 1 );
			}else if( cnt & PAD_KEY_DOWN ){
				act->offs.y += NUM_FX32( 1 );
			}else if( cnt & PAD_KEY_LEFT ){
				act->offs.x -= NUM_FX32( 1 );
			}else if( cnt & PAD_KEY_RIGHT ){
				act->offs.x += NUM_FX32( 1 );
			}
			
			if( trg & PAD_BUTTON_START ){
				OS_Printf( "ライト座標　x=%d,y=%d\n",
					FX32_NUM(act->offs.x), FX32_NUM(act->offs.y) );
			}
		}else{
			if( trg & PAD_KEY_UP ){
				act->scale_fx.y -= 0x0100;
			}else if( trg & PAD_KEY_DOWN ){
				act->scale_fx.y += 0x0100;
			}else if( trg & PAD_KEY_LEFT ){
				act->scale_fx.x -= 0x0100;
			}else if( trg & PAD_KEY_RIGHT ){
				act->scale_fx.x += 0x0100;
			}
			
			if( trg & PAD_BUTTON_START ){
				OS_Printf( "ライト拡縮率 " );
				OS_Printf( "X = 0x%x ", act->scale_fx.x );
				OS_Printf( "Y = 0x%x\n", act->scale_fx.y );
			}
		}
	}
	
	PlgLightActPosSet( act );
	#endif
}
#endif

//--------------------------------------------------------------
/**
 * FOBJ移動先の障害物を判定する
 * @param	gyml	PLGHOST_GYM_LOCAL_WORK
 * @retval	int		障害物までのグリッド数
 */
//--------------------------------------------------------------
#if 0
static int PlgJikiHitRangeGet(
	PLGHOST_GYM_LOCAL_WORK *gyml, FIELD_OBJ_PTR fldobj )
{
	u32 hit;
	VecFx32 pos;
	int grid,dir,x,y,z;
	
	x = FieldOBJ_NowPosGX_Get( fldobj );
	y = FieldOBJ_NowPosGY_Get( fldobj );
	z = FieldOBJ_NowPosGZ_Get( fldobj );
	dir = FieldOBJ_DirDispGet( fldobj );
	FieldOBJ_VecPosGet( fldobj, &pos );
	
	for( grid = 0; grid < PLG_LIGHT_RANGE_MAX; grid++ ){
		x += FieldOBJ_DirAddValueGX( dir );
		z += FieldOBJ_DirAddValueGZ( dir );
		FieldOBJTool_GridCenterPosGet( x, z, &pos );
		
		hit = FieldOBJ_MoveHitCheck( fldobj, &pos, x, y, z, dir );
		
		if( hit != FLDOBJ_MOVE_HIT_BIT_NON &&
			hit != FLDOBJ_MOVE_HIT_BIT_LIM ){
			return( grid );
		}
	}
	
	return( grid );
}
#endif

//--------------------------------------------------------------
/**
 * 目標値への増減
 * @param	val	値
 * @param	tar	目標値
 * @param	add	増減値
 * @retval	BOOL	TRUE=目標値と一致
 */
//--------------------------------------------------------------
#if 0
static void PlgTargetAdd( fx32 *val, fx32 tar, fx32 add )
{
	GF_ASSERT( add >= 0 );
	
	if( (*val) < tar ){
		(*val) += add;
		if( (*val) > tar ){ (*val) = tar; }
	}else if( (*val) > tar ){
		(*val) -= add;
		if( (*val) < tar ){ (*val) = tar; }
	}
}

//--------------------------------------------------------------
/**
 * 目標値への増減 VecFx32
 * @param	vec	値
 * @param	tar	目標値
 * @param	add	増減値
 * @retval	BOOL	TRUE=目標値と一致
 */
//--------------------------------------------------------------
static void PlgTargetAddVecFx32(
		VecFx32 *vec, const VecFx32 *tar, fx32 add )
{
	PlgTargetAdd( &vec->x, tar->x, add );
	PlgTargetAdd( &vec->y, tar->y, add );
	PlgTargetAdd( &vec->z, tar->z, add );
}
#endif

//--------------------------------------------------------------
/**
 * PL ゴーストジム:自機ライト視線チェック
 * @param	fsys	FIELDSYS_WORK
 * @retval	BOOL	TRUE=ライトヒットによる視線イベント発動
 */
//--------------------------------------------------------------
BOOL GYM_EventCheckPLGhostGymLightTrainer( FIELDSYS_WORK *fsys )
{
	int i,dir,range,hit;
	GIMMICKWORK *gmc;
	PLAYER_STATE_PTR jiki;
	FIELD_OBJ_SYS_PTR fos;
	FIELD_OBJ_PTR fldobj,jobj;
	
	gmc = SaveData_GetGimmickWork( GameSystem_GetSaveData(fsys) );
	
	if( GIMMICKWORK_GetAssignID(gmc) != FLD_GIMMICK_GHOST_GYM ){
		return( FALSE );	//ゴーストジムではない
	}
	
	i = 0;
	fldobj = NULL;
	fos = fsys->fldobjsys;
	jiki = fsys->player;
	jobj = Player_FieldOBJGet( jiki );
	range = PLG_LIGHT_RANGE_DEF;
	dir = FieldOBJTool_DirFlip( FieldOBJ_DirDispGet(jobj) ); //光の方向を反転
	
	while( FieldOBJSys_FieldOBJSearch(fos,&fldobj,&i,FLDOBJ_STA_BIT_USE) ){
#if 1	//formal
		if( fldobj != jobj && EvTrainer_FightFlagCheck(fsys,fldobj) == TRUE ){
			hit = EvTrainer_EyeRangeHitCheck( fldobj, jiki, dir, range );
			if( hit != EYE_CHECK_NOHIT ){	//ヒット　イベント起動
				int scr_id = FieldOBJ_EventIDGet( fldobj );
				EventSet_Script( fsys, SCRID_TRAINER_MOVE_BATTLE, fldobj );
				EventSet_TrainerEyeData(
					fsys, fldobj, hit, dir, scr_id, 
					GetTrainerIdByScriptId(scr_id),
					SCR_EYE_TR_TYPE_SINGLE, SCR_EYE_TR_0 );
				return( TRUE );
			}
		}
#else	//debug
		if( fldobj != jobj ){
			hit = EvTrainer_EyeRangeHitCheck( fldobj, jiki, dir, range );
			if( hit != EYE_CHECK_NOHIT ){	//ヒット　イベント起動
				int id = FieldOBJ_OBJIDGet( fldobj );
				OS_Printf( "自機懐中電灯ヒット OBJ ID=%d\n" );
				return( FALSE );
			}
		}
#endif
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * PL ゴーストジム:戦闘後のトレーナーをランダム移動に
 * @param	fsys	FIELDSYS_WORK
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	BOOL	TRUE=変更した
 */
//--------------------------------------------------------------
BOOL GYM_PlGhostGymTrainerMoveCodeChange(
		FIELDSYS_WORK *fsys, void *obj )
{
	GIMMICKWORK *gmc;
	
	gmc = SaveData_GetGimmickWork( GameSystem_GetSaveData(fsys) );
	
	if( GIMMICKWORK_GetAssignID(gmc) != FLD_GIMMICK_GHOST_GYM ){
		return( FALSE );	//ゴーストジムではない
	}
	
	{
		FIELD_OBJ_PTR fldobj = obj;
		#if 0
		int code = FieldOBJ_MoveCodeGet( fldobj );
		
		switch( code ){
		case MV_DMY:  case MV_DIR_RND:
		case MV_UP:   case MV_DOWN:
		case MV_LEFT: case MV_RIGHT:
		case MV_SPIN_L: case MV_SPIN_R:
			break;
		default:
			FieldOBJ_InitPosGX_Set( fldobj, FieldOBJ_NowPosGX_Get(fldobj) );
			FieldOBJ_InitPosGZ_Set( fldobj, FieldOBJ_NowPosGZ_Get(fldobj) );
			FieldOBJ_MoveLimitXSet( fldobj, 2 );
			FieldOBJ_MoveLimitZSet( fldobj, 2 );
			FieldOBJ_MoveCodeChange( fldobj, MV_RND );
			return( TRUE );
		}
		#else
		FieldOBJ_InitPosGX_Set( fldobj, FieldOBJ_NowPosGX_Get(fldobj) );
		FieldOBJ_InitPosGZ_Set( fldobj, FieldOBJ_NowPosGZ_Get(fldobj) );
		FieldOBJ_MoveLimitXSet( fldobj, 2 );
		FieldOBJ_MoveLimitZSet( fldobj, 2 );
		FieldOBJ_MoveCodeChange( fldobj, MV_RND );
		return( TRUE );
		#endif	
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * PL ゴーストジム：外れドアチェック
 * @param	fsys	FIELDSYS_WORK *
 * @param	x		ドアグリッドX
 * @param	z		ドアグリッドZ
 * @param	dir		進入方向　外れの場合は方向が反転される。
 * @retval	BOOL	TRUE=ゴーストジムの外れドアである。
 */
//--------------------------------------------------------------
BOOL GYM_PlGhostGymHintMissDoorCheck(
		FIELDSYS_WORK *fsys, int gx, int gz, int *dir )
{
	if( (*dir) != DIR_UP ){	//上以外
		return( FALSE );
	}
	
	{
		GIMMICKWORK *gmc;
		gmc = SaveData_GetGimmickWork( GameSystem_GetSaveData(fsys) );
	
		if( GIMMICKWORK_GetAssignID(gmc) != FLD_GIMMICK_GHOST_GYM ){
			return( FALSE );	//ゴーストジムではない
		}
		
		{
			PLGHOST_GYM_WORK *gym;
			gym = GIMMICKWORK_Get( gmc, FLD_GIMMICK_GHOST_GYM );
	
			if( gym->hint_no == GYM_PLGHOST_HINT_MAX ){
				return( FALSE );	//ヒントがない
			}
			
			{
				int stage = 0;
				int zone_id = fsys->location->zone_id;
				const PLG_HINTDATA *hint = DATA_PlgHintDataTbl;
				const PLG_EXIT *tbl;
		
				do{
					if( hint->zone_id == zone_id ){ break; }
					hint++;
					stage++;
				}while(	stage < PLG_STAGE_HINT_MAX );
		
				GF_ASSERT( stage != PLG_STAGE_HINT_MAX );
		
				tbl = DATA_PlgExitDataTbl[stage];
		
				while( tbl->hint_no != PLG_HINT_MAX ){
					if( tbl->x == gx && tbl->z == gz &&
						tbl->hint_no == gym->hint_no ){
						return( FALSE );	//正解である
					}
					tbl++;
				}
			}
		}
	}
	
	(*dir) = DIR_DOWN;
	OS_Printf( "ゴーストジム　外れドアの為、進入方向を反転させました\n" );
	return( TRUE );	//外れドアである
}

//--------------------------------------------------------------
/**
 * ライトOBJ座標をセット	debug
 * @param	act	PLG_LIGHT_ACT_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
#ifdef DEBUG_PLG_OBJWIN
static void PlgLightActPosSet( PLG_LIGHT_ACT_WORK *act )
{
	int dir;
	VecFx32 pos,f_pos,j_pos;
	PLAYER_STATE_PTR jiki;
	
	dir = act->dir;
	jiki = act->gyml->fsys->player;
	
	FieldOBJ_VecPosGet( act->fldobj, &f_pos );
	Player_VecPosGet( act->gyml->fsys->player, &j_pos );
	pos.x = NUM_FX32(128) - (j_pos.x - f_pos.x);
	pos.y = NUM_FX32(92) - (j_pos.z - f_pos.z);
	
	{
		int x,y;
		
		x = FX32_NUM( j_pos.x - f_pos.x );
		
		if( x > 0 ){		//自機より左
			x /= 16;
			pos.x -= NUM_FX32( x );
		}else if( x < 0 ){	//右
			x /= 16;
			pos.x -= NUM_FX32( x );
		}
		
		y = FX32_NUM( j_pos.z - f_pos.z );
		
		if( y > 0 ){		//自機より上
			y /= 8;
			pos.y += NUM_FX32( y );
		}else if( y < 0 ){	//下
			y /= 64;
			pos.y += NUM_FX32( y );
		}
	}
	
	pos.x -= NUM_FX32(128) - act->offs.x;
	pos.y -= NUM_FX32(92) - act->offs.y;
	
	CLACT_SetMatrix( act->clact, &pos );
	CLACT_SetScale( act->clact, &act->scale_fx );
	CLACT_SetRotation( act->clact, DATA_PlgLightRotateTbl[dir] );
	
	if( sys.trg & PAD_BUTTON_L ){
		OS_Printf( "ライトアクター(ID=%d)座標 X=%d,Y=%d\n",
				FieldOBJ_OBJIDGet(act->fldobj),
				FX32_NUM(pos.x), FX32_NUM(pos.y) );
	}
}
#endif

//--------------------------------------------------------------
///	ライト方向距離別拡縮率
//--------------------------------------------------------------
#ifdef DEBUG_PLG_OBJWIN
static const VecFx32 DATA_PlgLightRangeTbl[DIR_4_MAX][PLG_LIGHT_RANGE_MAX] =
{
	{
		{ 0x1c00, 0x0600, 0 },	//ok
		{ 0x1500, 0x0900, 0 },	//ok
		{ 0x1a00, 0x0d00, 0 },	//ok
		{ 0x1100, 0x0f00, 0 },	//ok
		{ 0x0f00, 0x1300, 0 },	//ok
		{ 0x1200, 0x1700, 0 },	//ok
	},
	{
		{ 0x1c00, 0x0600, 0 },	//ok
		{ 0x1500, 0x0900, 0 },	//ok
		{ 0x1a00, 0x0d00, 0 },	//ok
		{ 0x1100, 0x0f00, 0 },	//ok
		{ 0x0f00, 0x1300, 0 },	//ok
		{ 0x1200, 0x1700, 0 },	//ok
	},
	{
		{ 0x0600, 0x1c00, 0 },
		{ 0x0900, 0x1500, 0 },
		{ 0x0d00, 0x1a00, 0 },
		{ 0x0f00, 0x1100, 0 },
		{ 0x1300, 0x0f00, 0 },
		{ 0x1700, 0x1200, 0 },
	},
	{
		{ 0x0600, 0x1c00, 0 },
		{ 0x0900, 0x1500, 0 },
		{ 0x0d00, 0x1a00, 0 },
		{ 0x0f00, 0x1100, 0 },
		{ 0x1300, 0x0f00, 0 },
		{ 0x1700, 0x1200, 0 },
	},
};
#endif

//--------------------------------------------------------------
///	ライト方向別回転角度
//--------------------------------------------------------------
#ifdef DEBUG_PLG_OBJWIN
static const u32 DATA_PlgLightRotateTbl[DIR_4_MAX] =
{
	0,
	128*256,
	192*256,
	64*256,
};
#endif

//--------------------------------------------------------------
///	ライト方向、距離別表示位置
//--------------------------------------------------------------
#ifdef DEBUG_PLG_OBJWIN
static const VecFx32 DATA_PlgLightPosTbl[DIR_4_MAX][PLG_LIGHT_RANGE_MAX] =
{
	{
		{NUM_FX32(127),NUM_FX32(72),0},
		{NUM_FX32(127),NUM_FX32(64),0},
		{NUM_FX32(127),NUM_FX32(56),0},
		{NUM_FX32(127),NUM_FX32(53),0},
		{NUM_FX32(127),NUM_FX32(49),0},
		{NUM_FX32(127),NUM_FX32(42),0},
	},
	{
		{NUM_FX32(127),NUM_FX32(107),0},
		{NUM_FX32(127),NUM_FX32(112),0},
		{NUM_FX32(127),NUM_FX32(119),0},
		{NUM_FX32(127),NUM_FX32(123),0},
		{NUM_FX32(127),NUM_FX32(128),0},
		{NUM_FX32(127),NUM_FX32(136),0},
	},
	{
		{NUM_FX32(115),NUM_FX32(92),0},
		{NUM_FX32(109),NUM_FX32(92),0},
		{NUM_FX32(100),NUM_FX32(92),0},
		{NUM_FX32(97),NUM_FX32(92),0},
		{NUM_FX32(85),NUM_FX32(92),0},
		{NUM_FX32(82),NUM_FX32(92),0},
	},
	{
		{NUM_FX32(128+128-115),NUM_FX32(92),0},
		{NUM_FX32(128+128-109),NUM_FX32(92),0},
		{NUM_FX32(128+128-100),NUM_FX32(92),0},
		{NUM_FX32(128+128-97),NUM_FX32(92),0},
		{NUM_FX32(128+128-85),NUM_FX32(92),0},
		{NUM_FX32(128+128-82),NUM_FX32(92),0},
	},
};
#endif

#ifdef DEBUG_PLGHOSTGYM_CAPTURE
//--------------------------------------------------------------
//	ライトON,OFF
//--------------------------------------------------------------
void DEBUG_PlGhostGym_LightVanish( FIELDSYS_WORK *fsys )
{
	int i;
	PLGHOST_GYM_LOCAL_WORK *gyml = fsys->fldmap->Work;
	PLG_LIGHT_ACT_WORK *act = gyml->act_light;
	
	for( i = 0; i < PLG_ACT_MAX; i++, act++ ){
		if( act->use ){
			DEBUG_FE_Gym05Eff_LightVanish( act->eoa );
		}
	}
}
#endif


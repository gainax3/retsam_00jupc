//=============================================================================
/**
 * @file	gym_local.h
 * @bfief	ジム仕掛けとか
 * @author	Nozomu Saito
 *
 */
//=============================================================================
#ifndef __GYM_LOCAL_H__
#define __GYM_LOCAL_H__

//水ジム
//swtich用
#define GYM_WATER_LV1	(0)
#define GYM_WATER_LV2	(1)
#define GYM_WATER_LV3	(2)

//ゴーストジム
//swtich用
#define GYM_GHOST_LIFT_LV1	(0)
#define GYM_GHOST_LIFT_LV2	(1)

#define GHOST_LIFT_ROOM_START_Z		(22)	//リフトの部屋に初めて入ったときの、自機Z座標

//鋼ジム
#define STEEL_LIFT_MAX	(24)

#if 0	//DP
//格闘ジム
#define COMBAT_WALL_MAX	(12)
#endif

//電気ジム
#define GEAR_ROT_ST_0	(0)			//0°
#define GEAR_ROT_ST_90	(1)			//90°
#define GEAR_ROT_ST_180	(2)			//180°
#define GEAR_ROT_ST_270	(3)			//270°
#define GEAR_ROT_ST_MAX	(4)

#define ELEC_ROOM1_START_Z	(14)
#define ELEC_ROOM2_START_Z	(21)
#define ELEC_ROOM3_START_Z	(25)

#define ELEC_ROOM_MAX	(3)


///////ジムワーク///////
//水ジムワーク---------------------
typedef struct WATER_GYM_WORK_tag
{
	u8 Water;//水ジムの水位
}WATER_GYM_WORK;

//ゴーストジムワーク-------------------
typedef struct GHOST_GYM_WORK_tag
{
	u8 Lift;	//リフトの位置（上か下か）
}GHOST_GYM_WORK;

//鋼ジムワーク---------------------
typedef struct STEEL_GYM_WORK_tag
{
	int LiftPosBit;	//リフトの位置を格納
}STEEL_GYM_WORK;

#if 0	//DP
//格闘ジムワーク--------------------
typedef struct COMBAT_GYM_WORK_tag
{
	int WallPosBit;	//しきりの位置を格納
}COMBAT_GYM_WORK;
#endif

//電気ジムワーク--------------------
typedef struct ELEC_GYM_WORK_tag
{
	int GearRotateCode;	//ギア回転状況
	int RoomNo;		//部屋番号0～2
}ELEC_GYM_WORK;

//PL 草ジムワーク-------------------
typedef struct PL_GRASS_GYM_WORK_tag
{
	u32 time_seqflag;
}PL_GRASS_GYM_WORK;

//PL 格闘ジムワーク-----------------
typedef struct PLFIGHT_GYM_WORK_tag
{
	int init_flag;	//初期化フラグ
}PLFIGHT_GYM_WORK;

//PL ゴーストジムワーク
typedef struct PLGHOST_GYM_WORK_tag
{
	s16 init_flag;
	s16 hint_no;
	s16 hint_gx;
	s16 hint_gz;
}PLGHOST_GYM_WORK;

//PL 別荘
typedef struct
{
	int dmy;
}FLD_VILLA_WORK;

//PL やぶれたせかい
#define TW_SP_GROUND_BIT (4) //0-15
#define TW_SP_GROUND_MAX (1<<TW_SP_GROUND_BIT)

typedef struct
{
	u32 init_flag:1;			//初期化フラグ
	u32 fstone_flag:24;			//飛び石出現フラグ
	u32 sp_ground_no:TW_SP_GROUND_BIT; //自機現在の特殊地形番号
	u32 dmy:3;					//4 ビット余り
	
	u16 cm_ox;
	u16 cm_oy;					//8
	u16 cm_oz;						
	u16 zlink_mflag;			//12
	
	u32 event_flag;				//16
	
	u8 free[16];				//空き
}FLD_TORNWORLD_WORK;

#endif	//__GYM_LOCAL_H__

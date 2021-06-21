/*****************************************************************************
/**
 *
 * @file	field_tornworld.c
 * @brief	フィールド　やぶれたせかい
 * @author	kagaya
 * @data	05.07.13
 *
 */
//******************************************************************************
#include "common.h"
#include "fieldsys.h"
#include "fieldmap_work.h"
#include "fieldmap.h"
#include "field_effect.h"
#include "fieldobj.h"
#include "player.h"
#include "system/clact_util.h"
#include "worldmap.h"

#include "../fielddata/script/hiden_def.h"

#include "script.h"

#include "mapdefine.h"	//zone_id
#include "map_attr.h"
#include "zonedata.h"
#include "syswork.h"
#include "sysflag.h"
//#include "eventdata.h"

#include "include/savedata/gimmickwork.h"
#include "field_gimmick_def.h"
#include "gym_local.h"

#include "field_tornworld.h"

#include "../../include/field/evwkdef.h"
#include "../fielddata/script/saveflag.h"
#include "../fielddata/script/fld_item_def.h"
#include "../fielddata/script/d34r0101_def.h"
#include "../fielddata/script/d34r0102_def.h"
#include "../fielddata/script/d34r0103_def.h"
#include "../fielddata/script/d34r0104_def.h"
#include "../fielddata/script/d34r0108_def.h"
#include "../fielddata/script/d34r0109_def.h"
#include "../fielddata/script/d34r0110_def.h"
#include "../fielddata/script/d34r0111_def.h"

#include "../fielddata/tornworld/tw_arc.naix"
#include "../fielddata/tornworld/tw_arc_attr.naix"
#include "../data/tw_arc_etc.naix"

#include "tornworld_snd_def.h"
#include "battle/battle_common.h"

#include "hiden_effect.naix"

//==============================================================================
//	define
//==============================================================================
//--------------------------------------------------------------
//	デバッグ
//--------------------------------------------------------------
//----
#ifdef PM_DEBUG
//----
#define TW_DEBUG					//特定のデバッグ機能有効
#define DEBUG_TW_PRINTF_ON			//デバッグ出力有効
//#define DEBUG_TW_FOG_OFF			//定義でフォグ無効

/*---------------- kaga */
#ifdef DEBUG_ONLY_FOR_kagaya

#ifdef DEBUG_TW_PRINTF_ON
#define DEBUG_TW_PRINTF					//プリント有効
#endif

//#define DEBUG_TAKI_EFF				//滝エフェクトチェック
#define DEBUG_SPLASH_EFF
#define DEBUG_TW_OBJ_COUNT				//OBJ数カウント
#define DEBUG_TW_BUF_OBJ_ASSERT			//OBJアサート有効
#define DEBUG_TAKI_KUDARI_PAUSE			//滝下り ポーズ
//#define DEBUG_GIRAS					//ギラティナ影デバッグ
//#define DEBUG_TW_GIRAS_FLAG_OFF		//ギラティナ影出現済みに
//#define DEBUG_TW_FALLROCK_FINISH		//岩落とし攻略済みに
//#define DEBUG_TW_FALLROCK_7			//岩全て７層に落とし済みに
//#define DEBUG_TW_FLAG_INIT			//デバッグフラグ初期化有効

#ifdef DEBUG_TW_FALLROCK_FINISH
//#define DEBUG_TW_FALLROCK_FINISH_EVENT	//岩落とし攻略完了イベントを見る
#endif

#ifdef DEBUG_TW_PRINTF_ON
#define DEBUG_TW_PRINTF_FORCE		//他の環境でも出したいPrint出力はこちら
#endif

#endif
/*----------------------*/

/*------------------ nohara */
#ifdef DEBUG_ONLY_FOR_nohara
//0110 ギラティナ戦直前
//#define DEBUG_TW_SCENE_NOHARA_CHECK
#endif
/*----------------------*/
//----
#endif
//----

//--------------------------------------------------------------
///	みきわめちゅう
//--------------------------------------------------------------
//定義で壁ジャンプ後の自機高さ取得設定をタイプ別に変更
#define TESTPROC_JIKI_HEIGHT_SWITCH
//定義で背景のパレットフェードをオフ
//#define TORNWORLD_PLTT_FADE_OFF
//定義でマップリソース独立化
//#define TWORLD_MAPRES_INIT

//定義で滝エフェクト発生
//#define TWTEST_TAKI_EFF

//--------------------------------------------------------------
///	パレットフェード
//--------------------------------------------------------------
#define FADE_CHANGE( before, after, evy )					\
			( before + ( ( after - before ) * evy >> 4 ) )

///ギラティナ降臨時のパレット輝度
#define TW_PLTT_EVY_GIRAS_APP (12)

//--------------------------------------------------------------
//	自機
//--------------------------------------------------------------
//#define TWJIKI_RX (292)

//--------------------------------------------------------------
//	BG
//--------------------------------------------------------------
#define TW_BGFRM_BG	(FLD_MBGFRM_EFFECT2)
#define TW_BGFRM_BG_PLTT_OFFS (0)

//--------------------------------------------------------------
///	3Dエッジ
//--------------------------------------------------------------
#define TW_EDGE_COLOR_MAX (8)
#define EDGE_COLOR_R (4)
#define EDGE_COLOR_G (4)
#define EDGE_COLOR_B (4)

#define GIRAS_EDGE_NO (1)
#define GIRAS_EDGE_COLOR_R (4)
#define GIRAS_EDGE_COLOR_G (3)
#define GIRAS_EDGE_COLOR_B (7)
#define GIRAS_POLID (GIRAS_EDGE_NO*8)

/* エッジ参照位置はID上位3bit
 BBB_NNN
 000_000	//no 0, id 0,1
 000_001
 000_010
 000_011
 000_100
 000_101
 000_111
 001_000	//no 1, id 2,3
*/

//--------------------------------------------------------------
//	マップ
//--------------------------------------------------------------
#define NARC_MAPIDX_BIN_START (NARC_tw_arc_tw_maptable_idx+1)

//--------------------------------------------------------------
//	カメラ
//--------------------------------------------------------------
//#define TWCM_DEF_RX (0xdc62)
//#define TWCM_DEF_RX (0xce62)
//#define TWCM_DEF_RX (0xdc62)
//#define TWCM_DEF_RY (0x0000)
//#define TWCM_DEF_RZ (0x0000)
#define TWCM_DEF_RX (-0x29fe)
#define TWCM_DEF_RY (0x0000)
#define TWCM_DEF_RZ (0x0000)

#define TWCM_CLIP_NEAR (FX32_ONE*150)
#define TWCM_CLIP_FAR (FX32_ONE*1700)

//有効でカメラ生成 無効の場合はフィールド標準を操作する
//#define TWCM_CAMERADATA_ALLOC

//--------------------------------------------------------------
//	TCB
//--------------------------------------------------------------
enum
{
	TW_TCBPRI_START = 0,
	TW_TCBPRI_ZONECHG,
	TW_TCBPRI_STONE,
	
	TW_TCBPRI_SPLASH = 0xfffe,
	TW_TCBPRI_CLACT_DRAW = 0xffff,
};

//--------------------------------------------------------------
///	セルアクター
//--------------------------------------------------------------
#define TW_CLACT_MAX (16)

#define TW_BGPRI_CLOUD (3)

#define CLOUD_SET_MAX (9)		//雲最大数

#define TW_CLOUD_OFFS_Y (32)
#define TW_CLOUD_OFFS_Y_BIG (12)

#define TW_CLACTID_ORG (0xff)
#define CLACTRESID(a) ((a)+TW_CLACTID_ORG)
#define CLACT_ID_RES(a) ((a)-TW_CLACTID_ORG)

#define TW_CLACT_SFACE_X_FX (NUM_FX32(0))
#define TW_CLACT_SFACE_Y_FX (NUM_FX32(-512))

//--------------------------------------------------------------
///	セルアクター　リソースID　キャラ
//--------------------------------------------------------------
enum
{
	TW_CLACT_RES_CHAR_CLOUD_0,
	TW_CLACT_RES_CHAR_CLOUD_1,
	TW_CLACT_RES_CHAR_CLOUD_2,
	TW_CLACT_RES_CHAR_CLOUD_3,
	TW_CLACT_RES_CHAR_CLOUD_4,
	TW_CLACT_RES_CHAR_CLOUD_5,
	TW_CLACT_RES_CHAR_CLOUD_6,
	TW_CLACT_RES_CHAR_MAX,
};

//--------------------------------------------------------------
///	セルアクター　リソースID　パレット
//--------------------------------------------------------------
//#define TW_CELL_PLTT_TEST

#ifdef TW_CELL_PLTT_TEST
enum
{
	TW_CLACT_RES_PLTT_CLOUD_0,
	TW_CLACT_RES_PLTT_CLOUD_1,
	TW_CLACT_RES_PLTT_CLOUD_2,
	TW_CLACT_RES_PLTT_CLOUD_3,
	TW_CLACT_RES_PLTT_CLOUD_4,
	TW_CLACT_RES_PLTT_MAX,
};
#else
enum
{
	TW_CLACT_RES_PLTT_CLOUD_0,
	TW_CLACT_RES_PLTT_MAX,
};
#endif

///雲セルアクターパレット数
#define TW_CLACT_RES_PLTT_CLOUD_NUM (5)

//--------------------------------------------------------------
///	セルアクター　リソースID　セル
//--------------------------------------------------------------
enum
{
	TW_CLACT_RES_CELL_CLOUD_0,
	TW_CLACT_RES_CELL_CLOUD_1,
	TW_CLACT_RES_CELL_CLOUD_2,
	TW_CLACT_RES_CELL_CLOUD_3,
	TW_CLACT_RES_CELL_CLOUD_4,
	TW_CLACT_RES_CELL_CLOUD_5,
	TW_CLACT_RES_CELL_CLOUD_6,
	TW_CLACT_RES_CELL_MAX,
};

//--------------------------------------------------------------
///	セルアクター　リソースID　アニメ
//--------------------------------------------------------------
enum
{
	TW_CLACT_RES_ANM_CLOUD_0,
	TW_CLACT_RES_ANM_CLOUD_1,
	TW_CLACT_RES_ANM_CLOUD_2,
	TW_CLACT_RES_ANM_CLOUD_3,
	TW_CLACT_RES_ANM_CLOUD_4,
	TW_CLACT_RES_ANM_CLOUD_5,
	TW_CLACT_RES_ANM_CLOUD_6,
	TW_CLACT_RES_ANM_MAX,
};

//--------------------------------------------------------------
///	雲種類
//--------------------------------------------------------------
enum
{
	CLOUD_0,
	CLOUD_1,
	CLOUD_2,
	CLOUD_3,
	CLOUD_4,
	CLOUD_5,
	CLOUD_6,
	CLOUD_MAX,
};

//--------------------------------------------------------------
///	雲速度種類
//--------------------------------------------------------------
enum
{
	CLOUD_SPEED_TYPE_0,	//中心	
	CLOUD_SPEED_TYPE_1,	
	CLOUD_SPEED_TYPE_2,
	CLOUD_SPEED_TYPE_3,	//外周
	CLOUD_SPEED_TYPE_MAX,
};

//--------------------------------------------------------------
///	雲速度レベル
//--------------------------------------------------------------
enum
{
	CLOUD_SPEED_LV_0,	//初期
	CLOUD_SPEED_LV_1,	//最下層イベント用
	CLOUD_SPEED_LV_2,	//最下層イベント用
	CLOUD_SPEED_LV_MAX,
};

//--------------------------------------------------------------
///	フォグ
//--------------------------------------------------------------
#define TW_FOG_SLOPE	(6)		 //掛かり具合 低い0～10濃い
#define TW_FOG_OFFS		(0x7600) //デブス値 オフセット 低い程FOG適用Y値が高く
#define TW_FOG_COLOR	(GX_RGB(0,0,0))	//フォグカラー
#define TW_FOG_ALPHA	(16)	 //透過率 0(2D面と融合)～31(2D面との融合無し)
//#define TW_FOGTBL_BLDMAX (72) //フォグテーブルフォグ濃度最大 0-127 高いほど濃く
#define TW_FOGTBL_BLDMAX (88) //フォグテーブルフォグ濃度最大 0-127 高いほど濃く

//--------------------------------------------------------------
///	イベント
//--------------------------------------------------------------
#define TW_EVENTWORK_SIZE (128)	///<イベントで使用するワークサイズ

//--------------------------------------------------------------
///	自機ジャンプオフセット
//--------------------------------------------------------------
typedef enum
{
	OFFS_VEC_X = 0,
	OFFS_VEC_Y,
	OFFS_VEC_Z,
}OFFS_VEC_TYPE;

#define OFFS_FRAME_MAX (16)
#define OFFS_FRAME_MAX_FX (NUM_FX32(OFFS_FRAME_MAX))

typedef enum
{
	OFFS_VEC_FLIP_NON = 0,
	OFFS_VEC_FLIP_ON,
}OFFS_VEC_FLIP;

//--------------------------------------------------------------
///	OBJ回転数
//--------------------------------------------------------------
#define TW_OBJROT_MAX (4+2)	///<OBJ回転数 +2=けむり

//--------------------------------------------------------------
///	マップ
//--------------------------------------------------------------
///アトリビュートデータ->アトリビュート
#define ATRD_ATTR(a) ((a)&0xff)
///アトリビュートデータ->移動フラグ
#define ATRD_MFLAG(a) (((a)&0x8000)>>15)	//最上位1bit=壁フラグ

///特殊地形　アトリビュート
enum
{
	TWATTR_NOT = 0xffff,
	TWATTR_OVER = 0xfffe,
};

//--------------------------------------------------------------
///	接続位置　動作タイプ
//--------------------------------------------------------------
enum
{
	LPOSMOVE_ZONE_UP = 0,		///<ゾーン上移動
	LPOSMOVE_ZONE_DOWN,			///<ゾーン下移動
	LPOSMOVE_LIFT,				///<ゾーン内リフト
	LPOSMOVE_MAX,				///<最大
};

//--------------------------------------------------------------
///	ゾーン別リンク数
//--------------------------------------------------------------
enum
{
	TW_ZONELINK_0101,
	TW_ZONELINK_0102,
	TW_ZONELINK_0103,
	TW_ZONELINK_0104,
	TW_ZONELINK_0105,
	TW_ZONELINK_0107,
	TW_ZONELINK_0108,
	TW_ZONELINK_0109,
	TW_ZONELINK_MAX,
	
//	TW_ZONELINK_0111,
};

//--------------------------------------------------------------
///	階層数
//--------------------------------------------------------------
enum
{
	TW_ZONE0101,		//1
	TW_ZONE0102,		//2
	TW_ZONE0103,		//3
	TW_ZONE0104,		//4
	TW_ZONE0105,		//5A
	TW_ZONE0107,		//6
	TW_ZONE0108,		//7
	TW_ZONE0109,		//8A
	TW_ZONE0110,		//8D
	TW_ZONE0111,		//8B
	TW_ZONECOUNT_MAX,
	
//	TW_ZONE0106,		//5B
};

//--------------------------------------------------------------
///	ゾーン変更動作テーブル
//--------------------------------------------------------------
enum
{
	ZCHG_MID_Z0101_1_2,
	ZCHG_MID_Z0102_2_1,
	ZCHG_MID_Z0102_2_3,	
	ZCHG_MID_Z0103_3_2,
	ZCHG_MID_Z0103_3_4,
	ZCHG_MID_Z0104_4_3,
	ZCHG_MID_Z0104_4A_5A,
	ZCHG_MID_Z0104_4B_5B,
	ZCHG_MID_Z0104_4_6_0,
	ZCHG_MID_Z0104_4_6_1,
	ZCHG_MID_Z0105_5A_4A,
	ZCHG_MID_Z0105_5B_4B,
	ZCHG_MID_Z0105_5_6,
	ZCHG_MID_Z0107_6_5,
	ZCHG_MID_Z0107_6_7,
	ZCHG_MID_Z0107_6_4_0,
	ZCHG_MID_Z0107_6_4_1,
	ZCHG_MID_Z0108_7_6,
	ZCHG_MID_Z0108_7_8,
	ZCHG_MID_Z0109_8_7,
	ZCHG_MID_Z0111_0,	//not use
	ZCHG_MID_Z0111_1,	//not use
	ZCHG_MID_MAX,
};

//--------------------------------------------------------------
///	PROCRET
//--------------------------------------------------------------
typedef enum
{
	RET_NON,			///<何もしない
	RET_CONTINUE,		///<続ける
	RET_END,			///<終了
}PROCRET;

//--------------------------------------------------------------
///	ゾーン接続移動フラグ
//--------------------------------------------------------------
enum
{
	ZLINKMFLAG_0102 = 0,	//1->2移動フラグ
	ZLINKMFLAG_0203,		//2->3移動フラグ
	ZLINKMFLAG_0304,		//3->4移動フラグ
	ZLINKMFLAG_0405A,		//4->5A移動フラグ
	ZLINKMFLAG_0405B,		//4->5B移動フラグ
	ZLINKMFLAG_0406A,		//4->6A移動フラグ
	ZLINKMFLAG_0504A,		//5->4A移動フラグ
	ZLINKMFLAG_0506B,		//5->6B移動フラグ
	ZLINKMFLAG_06B05,		//6B->5移動フラグ
	ZLINKMFLAG_0607,		//6->7移動フラグ
	ZLINKMFLAG_0708,		//7->8移動フラグ
	ZLINKMFLAG_MAX,			//フラグ最大
};

///初期化時にセットするゾーン接続移動フラグまとめ
#define ZLINKMFLAG_INITFLAG ( \
		(1<<ZLINKMFLAG_0405A) | \
		(1<<ZLINKMFLAG_0506B) \
		)

///八層初期化時にセットするゾーン接続移動フラグまとめ
#define ZLINKMFLAG_INITFLAG_08 ( \
		(1<<ZLINKMFLAG_0102) | \
		(1<<ZLINKMFLAG_0203) | \
		(1<<ZLINKMFLAG_0304) | \
		(1<<ZLINKMFLAG_0405A) | \
		(1<<ZLINKMFLAG_0405B) | \
		(1<<ZLINKMFLAG_0506B) | \
		(1<<ZLINKMFLAG_0607) | \
		(1<<ZLINKMFLAG_0708) \
		)

//==============================================================================
//	define スクリプト
//==============================================================================
//--------------------------------------------------------------
///	スクリプトタイプ
//--------------------------------------------------------------
typedef enum
{
	TWSCRCODE_ACMD = 0,		///<アニメーションコマンドセット
	TWSCRCODE_LIFT,			///<リフト移動
	TWSCRCODE_FLDOBJADD,	///<フィールドOBJ追加
	TWSCRCODE_FLDOBJDEL,	///<フィールドOBJ削除
	TWSCRCODE_TAKINOBORI,	///<滝登り
	TWSCRCODE_SCRCALL,		///<スクリプトコール
	TWSCRCODE_EVSEQSET,		///<TWEVSEQセット
	TWSCRCODE_GIRAS,		///<ギラティナ影
	TWSCRCODE_GIRASFLAGSET,	///<sysflag ギラティナ影フラグセット
	TWSCRCODE_TWEVFLAG_SET,	///<TWEVFLAG ローカルフラグセット
	TWSCRCODE_TAKIKUDARI,	///<滝下り
	TWSCRCODE_GIRA_APPEAR,	///<ギラティナ登場
	TWSCRCODE_ROCKPOKE_YUKUSHI,	///<岩ユクシー
	TWSCRCODE_ROCKPOKE_AGUNOMU,	///<岩アグノム
	TWSCRCODE_ROCKPOKE_EMURIT,	///<岩エムリット
	TWSCRCODE_STONE0110_ON,		///<最下層飛び石表示イベント
	TWSCRCODE_STONE0110_OFF,		///<最下層飛び石非表示イベント
	TWSCRCODE_TWEVFLAG_OFF,	///<TWEVFLAG ローカルフラグ落とし
	TWSCRCODE_MAX,
}TWSCRCODE;

#define TW_SCRPROCWORK_SIZE (160)	//スクリプトワークサイズ

//==============================================================================
//	define	tornworld外部データと共通
//==============================================================================
//--------------------------------------------------------------
///	地形タイプ
//--------------------------------------------------------------
enum
{
	TW_GNDTYPE_GROUND = 0,
	TW_GNDTYPE_SIDE_L,
	TW_GNDTYPE_SIDE_R,
	TW_GNDTYPE_ROOF,
	TW_GNDTYPE_MAX,
};

//--------------------------------------------------------------
///	座標発動自機イベント種類
//--------------------------------------------------------------
enum
{
	EVPOSTYPE_WALLJUMP,
	EVPOSTYPE_MAX,
};

//--------------------------------------------------------------
///	モデルID
//--------------------------------------------------------------
enum
{
	TW_MDL_STEP,		//0
	TW_MDL_PUT,
	TW_MDL_STEP3A,
	TW_MDL_STEP3B,
	TW_MDL_STEP3C,
	TW_MDL_STEP4,
	TW_MDL_STEP5,
	TW_MDL_STEP6,
	TW_MDL_STEP7,
	TW_MDL_STEP8,
	TW_MDL_STEP9,		//10
	TW_MDL_STEP10,
	TW_MDL_STEP11,
	TW_MDL_STEP12,
	TW_MDL_STEP13,
	TW_MDL_STEP14,
	TW_MDL_STEP15,
	TW_MDL_STEP16,
	TW_MDL_STEP17,
	TW_MDL_STEP18,
	TW_MDL_GIRAS,		//20
	TW_MDL_WATER_FALL,
	TW_MDL_LAND_TREE,
	TW_MDL_LAND_ROCK,
	TW_MDL_ANA_OUT,
	TW_MDL_MAX,
};

//--------------------------------------------------------------
///	アニメID
//--------------------------------------------------------------
enum
{
	TW_ANM_GIRAS,
	TW_ANM_LAND_TREE,
	TW_ANM_LAND_ROCK,
	TW_ANM_WATER_FALL,
	TW_ANM_ANA_OUT,
	TW_ANM_MAX,
};

//--------------------------------------------------------------
///	OBJ種類
//--------------------------------------------------------------
typedef enum
{
	TWOBJTYPE_STEP,			//0
	TWOBJTYPE_PUT,
	TWOBJTYPE_STEP3A,
	TWOBJTYPE_STEP3B,
	TWOBJTYPE_STEP3C,
	TWOBJTYPE_STEP4,
	TWOBJTYPE_STEP5,
	TWOBJTYPE_STEP6,
	TWOBJTYPE_STEP7,
	TWOBJTYPE_STEP8,
	TWOBJTYPE_STEP9,		//10
	TWOBJTYPE_STEP10,
	TWOBJTYPE_STEP11,
	TWOBJTYPE_STEP12,
	TWOBJTYPE_STEP13,
	TWOBJTYPE_STEP14,
	TWOBJTYPE_STEP15,
	TWOBJTYPE_STEP16,
	TWOBJTYPE_STEP17,
	TWOBJTYPE_STEP18,		//19
	TWOBJTYPE_GIRAS,		
	TWOBJTYPE_WATER_FALL,	//21
	TWOBJTYPE_LAND_TREE,	//22
	TWOBJTYPE_LAND_ROCK,	//23
	TWOBJTYPE_ANA_OUT,
	TWOBJTYPE_MAX,
}TWOBJTYPE;

//--------------------------------------------------------------
///	OBJ共用識別
//--------------------------------------------------------------
enum
{
	OBJRES_ONLY,
	OBJRES_SHARE,
};

//--------------------------------------------------------------
///	3DOBJ表示数
//--------------------------------------------------------------
//#define TW_OBJ_MAX (64)
#define TW_OBJ_MAX (34)

//--------------------------------------------------------------
///	飛び石
//--------------------------------------------------------------
#define FSTONE_FLAGNO_MAX (24)

//--------------------------------------------------------------
///	配置OBJ
//--------------------------------------------------------------
#define TW_PUTOBJ_MAX (8)

//--------------------------------------------------------------
///	配置フィールドOBJ
//--------------------------------------------------------------
//#define TW_FLDOBJ_MAX (10)
//OBJ増加に伴い最大数増加。七層NONDRAWx9なのでVRAMリソース増加は無い。
#define TW_FLDOBJ_MAX (19)

//--------------------------------------------------------------
///	破れた世界イベントフラグ MAX 32 
///	破れた世界の中でのみ保持されるイベントフラグ。
/// 破れた世界から出る事でクリアされる。
//--------------------------------------------------------------
enum
{
	TWEVFLAGNO_ERROR = -1,				///<エラー判定用
	
	TWEVFLAGNO_FALLROCK0_6 = 0,			///<00 岩落とし0 6層に存在
	TWEVFLAGNO_FALLROCK1_6,				///<01 岩落とし1 6層に存在
	TWEVFLAGNO_FALLROCK2_6,				///<02 岩落とし2 6層に存在
	TWEVFLAGNO_FALLROCK0_7,				///<03 岩落とし0 7層に存在
	TWEVFLAGNO_FALLROCK1_7,				///<04 岩落とし1 7層に存在
	TWEVFLAGNO_FALLROCK2_7,				///<05 岩落とし2 7層に存在
	TWEVFLAGNO_FALLROCK0_OK,			///<06 岩落とし 0 OK
	TWEVFLAGNO_FALLROCK1_OK,			///<07 岩落とし 1 OK
	TWEVFLAGNO_FALLROCK2_OK,			///<08 岩落とし 2 OK
	TWEVFLAGNO_GIRA_BARK,				///<09 ギラティナ吠える
	TWEVFLAGNO_0107YUKUSHIEV,		///<10 岩落としユクシー登場イベント済み
	TWEVFLAGNO_0107AGUNOMUEV,		///<11 岩落としアグノム登場イベント済み
	TWEVFLAGNO_0107EMURITEV,		///<12 岩落としエムリット登場イベント済み
	TWEVFLAGNO_0108YUKUSHI,				///<13 ７層ユクシーON
	TWEVFLAGNO_0108AGUNOMU,				///<14 ７層アグノムON
	TWEVFLAGNO_0108EMURIT,				///<15 ７層エムリットON
	TWEVFLAGNO_0110STONEEV_END,			///<16 八層深部飛び石出現イベント
	
	TWEVFLAGNO_MAX,						///<最大,無効判定用
};

///岩落とし 初期配置フラグまとめ
#define TWEVFLAG_FALLROCK_6_ALL \
	( (1<<TWEVFLAGNO_FALLROCK0_6)| \
	  (1<<TWEVFLAGNO_FALLROCK1_6)| \
	  (1<<TWEVFLAGNO_FALLROCK2_6) )

///岩落とし　攻略済み
#define TWEVFLAG_FALLROCK_FINISH \
	( (1<<TWEVFLAGNO_0107YUKUSHIEV) | \
	  (1<<TWEVFLAGNO_0107AGUNOMUEV) | \
	  (1<<TWEVFLAGNO_0107EMURITEV) | \
	  (1<<TWEVFLAGNO_FALLROCK0_OK) | \
	  (1<<TWEVFLAGNO_FALLROCK1_OK) | \
	  (1<<TWEVFLAGNO_FALLROCK2_OK) )
	
//--------------------------------------------------------------
//	落下石ID
//--------------------------------------------------------------
enum
{
	TWFLDOBJID_FALLROCK_0 = 0,
	TWFLDOBJID_FALLROCK_1,
	TWFLDOBJID_FALLROCK_2,
};

//--------------------------------------------------------------
///	ゾーン変更タイプ　タスクにて識別
//--------------------------------------------------------------
enum
{
	TWZONECHGTYPE_NON = 0,
	TWZONECHGTYPE_UP,
	TWZONECHGTYPE_DOWN,
};

//--------------------------------------------------------------
///	ギラティナ影専用方向
//--------------------------------------------------------------
enum
{
	GIRAS_DIR_UP = 0,	//上
	GIRAS_DIR_DOWN,		//下
	GIRAS_DIR_LEFT,		//左
	GIRAS_DIR_RIGHT,	//右
	GIRAS_DIR_RISE,		//上昇
	GIRAS_DIR_MAX,
};

//--------------------------------------------------------------
///	ギラティナ影SE識別
//--------------------------------------------------------------
enum
{
	GIRAS_SE_OFF,		///<SE無し
	GIRAS_SE_VOICE,		///<ポケモンボイス
	GIRAS_SE_FLIGHT,	///<飛行音
};

//--------------------------------------------------------------
///	ギラティナ影その他
//--------------------------------------------------------------
#define GIRAS_MOVE_VALUE (FX32_ONE*48)	///<移動速度

//==============================================================================
//	接続石
//==============================================================================
//--------------------------------------------------------------
///	三層　ID
//--------------------------------------------------------------
enum
{
	LPOSID0103_ZUP_3_2 = 0,
//	LPOSID0103_ZDOWN_3_4,
	LPOSID0103_LIFT00,
	LPOSID0103_LIFT01,
	LPOSID0103_LIFT02,
	LPOSID0103_LIFT03,
	LPOSID0103_LIFT04,
	LPOSID0103_LIFT05,
	LPOSID0103_LIFT06,
	LPOSID0103_LIFT07,
//	LPOSID0103_LIFT08,	仕掛け簡易化により廃止
	
	LPOSID0103_LIFT200,
	LPOSID0103_LIFT201,
	LPOSID0103_LIFT202,
	LPOSID0103_LIFT203,
	LPOSID0103_LIFT204,
	LPOSID0103_LIFT205,
	LPOSID0103_LIFT206,
#if 0					//仕掛け簡易化により廃止
	LPOSID0103_LIFT207,
	LPOSID0103_LIFT208,
#endif
	LPOSID0103_LIFT209,
	LPOSID0103_LIFT210,
};

//--------------------------------------------------------------
///	４層　ID
//--------------------------------------------------------------
enum
{
	LPOSID0104_ZUP_4_3 = 0,
	LPOSID0104_ZDOWN_4A_5A,
	LPOSID0104_ZDOWN_4B_5B,
	LPOSID0104_ZDOWN_4_6,
};

//--------------------------------------------------------------
///	５層　ID
//--------------------------------------------------------------
enum
{
	LPOSID0105_ZUP_5A_4A = 0,
	LPOSID0105_ZUP_5B_4B,
	LPOSID0105_ZDOWN_5_6,
};

//--------------------------------------------------------------
///	六層　ID
//--------------------------------------------------------------
enum
{
	LPOSID0107_ZUP_06_05 = 0,
	LPOSID0107_ZDOWN_06_07,
	LPOSID0107_ZUP_06_04,
};

//==============================================================================
//	typedef
//==============================================================================
typedef struct _TORNWORLD_WORK TORNWORLD_WORK;

//--------------------------------------------------------------
///	RECT_SIZE
//--------------------------------------------------------------
typedef struct
{
	s16 gx;
	s16 gy;
	s16 gz;
	s16 gx_size;
	s16 gy_size;
	s16 gz_size;
}TW_GRID_RECT_SIZE;

//--------------------------------------------------------------
///	CLACT_RESID
//--------------------------------------------------------------
typedef struct
{
	u8 char_id;
	u8 pltt_id;
	u8 cell_id;
	u8 anm_id;
}CLACT_RESID;

//--------------------------------------------------------------
///	TW_CLACT_SETDATA
//--------------------------------------------------------------
typedef struct
{
	CLACT_SET_PTR clact_set;
	CLACT_U_EASYRENDER_DATA clact_easydata;
	CLACT_U_RES_MANAGER_PTR clact_resm_char;
	CLACT_U_RES_MANAGER_PTR clact_resm_pltt;
	CLACT_U_RES_MANAGER_PTR clact_resm_cell;
	CLACT_U_RES_MANAGER_PTR clact_resm_anm;
	CLACT_U_RES_OBJ_PTR	clact_resobj_char[TW_CLACT_RES_CHAR_MAX];
	CLACT_U_RES_OBJ_PTR	clact_resobj_pltt[TW_CLACT_RES_PLTT_MAX];
	CLACT_U_RES_OBJ_PTR	clact_resobj_cell[TW_CLACT_RES_CELL_MAX];
	CLACT_U_RES_OBJ_PTR	clact_resobj_anm[TW_CLACT_RES_ANM_MAX];
//	NNSG2dCellTransferState *cellTransArray;
	TCB_PTR tcb_clact_draw;
}TW_CLACT_SETDATA;

//--------------------------------------------------------------
///	PLTT構造体
//--------------------------------------------------------------
typedef struct
{
	u16 red:5;
	u16 green:5;
	u16 blue:5;
	u16 dummy:1;
}PLTT;

//--------------------------------------------------------------
///	TW_PLTT_WORK
//--------------------------------------------------------------
typedef struct
{
	s16 evy;
	s16 old_evy;
	u16 pltt_trans_flag;
	u16 evy_layer_off;
	
	u16 bg3_plttbase[16];
	u16 bg3_plttbuf[16];
	u16 clact_cloud_plttbase[16*TW_CLACT_RES_PLTT_CLOUD_NUM];
	u16 clact_cloud_plttbuf[16*TW_CLACT_RES_PLTT_CLOUD_NUM];
}TW_PLTT_WORK;

//--------------------------------------------------------------
///	EOA_CLOUD_DATA
//--------------------------------------------------------------
typedef struct
{
	int type;
	int speed_type;
	fx32 angle;
	fx32 offs_angle;
	int radius;
	fx32 speed;
	VecFx32 c_pos;
	VecFx32 scale;
}CLOUD_INITDATA;

//--------------------------------------------------------------
///	EOA_CLOUD_H
//--------------------------------------------------------------
typedef struct
{
	TORNWORLD_WORK *tw;
	CLOUD_INITDATA data;
	TW_CLACT_SETDATA *set;
}EOA_CLOUD_H;

//--------------------------------------------------------------
///	EOA_CLOUD_WORK
//--------------------------------------------------------------
typedef struct
{
	fx32 angle;
	fx32 add_speed;
	EOA_CLOUD_H head;
	CLACT_WORK_PTR clact;
}EOA_CLOUD_WORK;

//--------------------------------------------------------------
///	TW_FOG_WORK
//--------------------------------------------------------------
typedef struct
{
	char fog_tbl[32];
}TW_FOG_WORK;

//--------------------------------------------------------------
///	TW_MDIDX tornworld外部データと共通 tw_maptable
//--------------------------------------------------------------
typedef struct
{
	u32 zone_id;
	u16 arc_idx;
	s16 x;
	s16 y;
	s16 z;
}TW_MDIDX;

//--------------------------------------------------------------
///	TW_MDIDX_IDX
//--------------------------------------------------------------
typedef struct
{
	int idx_max;
	TW_MDIDX *idx_p;
	void *idx_buf_p;
}TW_MDATA_IDX;

//--------------------------------------------------------------
///	TW_GNDDATA	tornworld外部データと共通
//--------------------------------------------------------------
typedef struct
{
	s16 type;
	u16 attr_idx;
	TW_GRID_RECT_SIZE rect_size;
	u16 attr_gx_size;
	u16 attr_gy_size;
}TW_GNDDATA;

//--------------------------------------------------------------
///	TW_EVJUMPDATA	tornworld外部データと共通
//--------------------------------------------------------------
typedef struct
{
	u16 type;						//イベントタイプ
	s16 dir;						//発動方向
	int param;						//パラメタ
	TW_GRID_RECT_SIZE rect_size;	//イベント発生矩形
	s16 gx_size;					//グリッド移動量X
	s16 gy_size;					//グリッド移動量Y
	s16 gz_size;					//グリッド移動量Z
	s16 rot_val;					//OBJ回転量 360
	s16 frame;						//グリッド移動フレーム
	u16 offs_vec_type;	//オフセットをかけるベクトルタイプ
	u16 offs_vec_flip;	//オフセットの反転
	s16 dir_after;					//移動後のOBJ方向
	s16 gnd_after;					//移動後の着地地面タイプ
	u16 gndno_after;				//移動後の着地地面データ番号
}TW_EVJUMPDATA;

//--------------------------------------------------------------
///	TW_CMACTDATA	tornworld外部データと共通
//--------------------------------------------------------------
typedef struct
{
	TW_GRID_RECT_SIZE rect_size;
	u16 cm_x;	//256
	u16 cm_y;
	u16 cm_z;
	s16 dir;
	s32 frame;
}TW_CMACTDATA;

///TW_CMACTDATA cm_*を0x10000単位に変換
#define CMAD_V16(a) ((a)*0x100)

//--------------------------------------------------------------
//	TW_FSTONE_HEADER	tornworld外部データと共通
//--------------------------------------------------------------
typedef struct
{
	int put_max;
	int flagpos_max;
	u32 init_flag;
}TW_FSTONE_HEADER;

//--------------------------------------------------------------
///	TW_FSTONE_PUTPOS	tornworld外部データと共通
//--------------------------------------------------------------
typedef struct
{
	u32 flag_no;
	u16 type;
	s16 gx;
	s16 gy;
	s16 gz;
}TW_FSTONE_PUTPOS;

//--------------------------------------------------------------
///	TW_FSTONE_FLAGPOS	tornworld外部データと共通
//--------------------------------------------------------------
typedef struct
{
	u32 flag_no;
	s16 dir;
	s16 on_off;	//draw flag
	TW_GRID_RECT_SIZE rect_size;
}TW_FSTONE_FLAGPOS;

//--------------------------------------------------------------
///	TW_GNDDATA_TBL
//--------------------------------------------------------------
typedef struct
{
	int gnd_num_now;
	int tbl_max;
	TW_GNDDATA *gnd_tbl_p;
	u32 attr_size;
	u16 *attr_buf_p;
}TW_GNDDATA_TBL;

//--------------------------------------------------------------
///	TW_EVJUMPDATA_TBL
//--------------------------------------------------------------
typedef struct
{
	int tbl_max;
	TW_EVJUMPDATA *evj_tbl_p;
}TW_EVJUMPDATA_TBL;

//--------------------------------------------------------------
///	TW_CMACTDATA_TBL
//--------------------------------------------------------------
typedef struct
{
	int tbl_max;
	TW_CMACTDATA *cma_tbl_p;
}TW_CMACTDATA_TBL;

//--------------------------------------------------------------
///	TW_FSTONEDATA_TBL
//--------------------------------------------------------------
typedef struct
{
	const TW_FSTONE_HEADER *head_p;
	const TW_FSTONE_PUTPOS *putpos_tbl_p;
	const TW_FSTONE_FLAGPOS *flagpos_tbl_p;
}TW_FSTONEDATA_TBL;

//--------------------------------------------------------------
///	TW_ZONEDATA_BUF_IDX
//--------------------------------------------------------------
typedef struct
{
	int bin_no;
	int gnd_size;
	int evj_size;
	int cma_size;
	int fstone_size;
}TW_ZONEDATA_BUF_IDX;

//--------------------------------------------------------------
///	TW_ZONEDATA_BUF
//--------------------------------------------------------------
typedef struct
{
	u32 zone_id;
	void *gnd_p;
	void *evj_p;
	void *cma_p;
	void *fstone_p;
	void *idx_p;
	void *zone_buf_p;
}TW_ZONEDATA_BUF;

//--------------------------------------------------------------
///	TW_MDATA
//--------------------------------------------------------------
typedef struct
{
	TW_MDATA_IDX		md_idx;
	TW_ZONEDATA_BUF		zone_buf;
	TW_GNDDATA_TBL		gnd_tbl;
	TW_EVJUMPDATA_TBL	evj_tbl;
	TW_CMACTDATA_TBL	cma_tbl;
	TW_FSTONEDATA_TBL	fstone_tbl;
	
	TW_ZONEDATA_BUF		zone_buf_lower;
	TW_FSTONEDATA_TBL	fstone_tbl_lower;
}TW_MDATA;

//--------------------------------------------------------------
///	TW_CMACT_WORK
//--------------------------------------------------------------
typedef struct
{
	int act_flag;
	int frame;
	VecFx32 offs_fx;
	VecFx32 add_val_fx;
}TW_CMACT_WORK;

//--------------------------------------------------------------
///	TW_CAMERA_WORK
//--------------------------------------------------------------
typedef struct
{
	GF_CAMERA_PTR gfc_camera;
	CAMERA_ANGLE angle;					//カメラの基本アングル
	CAMERA_ANGLE angle_offs;			//アングルオフセット
	CAMERA_ANGLE angle_offs_true;		//アングルオフセットの真
	
	TW_CMACT_WORK action_work;
	TCB_PTR tcb_action;
	
	#ifdef DEBUG_TWORLD_CAPTURE
	CAMERA_ANGLE debug_cap_angle;
	#endif
}TW_CAMERA_WORK;

//--------------------------------------------------------------
///	TW_JIKI_WALLJUMP_WORK
//--------------------------------------------------------------
typedef struct
{
	int seq_no;
	int frame;
	VecFx32 move_val;
	
	fx32 offs_frame;
	fx32 offs_frame_val;
	VecFx32 grid_count;
	VecFx32 grid_count_val;
	
	TW_EVJUMPDATA data;
}TW_JIKI_WALLJUMP_WORK;

//--------------------------------------------------------------
///	TW_EVENT_WORK
//--------------------------------------------------------------
typedef struct
{
	int dmy;
	ALIGN4 u8 work[TW_EVENTWORK_SIZE];
}TW_EVENT_WORK;

//--------------------------------------------------------------
//	DEBUG
//--------------------------------------------------------------
#ifdef TW_DEBUG
typedef struct
{
	int seq_no;
	fx32 frame;
	void *pResAnm;
	void *pResAnmIdx;
	NNSG3dAnmObj *pAnmObj;
	NNSFndAllocator Allocator;
	
	#ifdef TWTEST_TAKI_EFF
	VecFx32 splash_pos;
	EOA_PTR eoa_splash;
	#endif
	
#ifdef TW_DEBUG
	int zone_change;
#endif
}TW_DEBUG_WORK;
#endif

#if 0
//--------------------------------------------------------------
///	BLACT_ROT_WORK
//--------------------------------------------------------------
typedef struct
{
	NNSG3dAnmObj *pAnmObj;
	fx32 frame;
}TW_BLACT_ROT_WORK;
#endif

//--------------------------------------------------------------
/// TW_OBJ_ROT_ACT
//--------------------------------------------------------------
typedef struct
{
	s16 rot_org;	//0-359
	s16 add_frame;
	fx32 rot_fx;
	fx32 rot_add_fx;
	fx32 frame_fx;
	NNSG3dAnmObj *pAnmObj;
	
	u16 obj_code;
	u16 obj_id;
	FIELD_OBJ_PTR fldobj;	//登録されている場合は監視
	BLACT_WORK_PTR blact;	//登録されている場合は回転処理を行う
}TW_OBJ_ROT_ACT_WORK;

#if 0
//--------------------------------------------------------------
///	TW_OBJ_ROT_REG_WORK	
//--------------------------------------------------------------
typedef struct
{
	int init_rotate;	//0-359
	FIELD_OBJ_PTR fldobj;
}TW_OBJ_ROT_REG_WORK;
#endif

//--------------------------------------------------------------
///	TW_OBJ_ROT_PROC_WORK
//--------------------------------------------------------------
typedef struct
{
	int max;
	TW_OBJ_ROT_ACT_WORK *act_tbl;
	
	void *pResAnm;
	void *pResAnmIdx;
	NNSFndAllocator Allocator;
}TW_OBJ_ROT_PROC_WORK;

//--------------------------------------------------------------
///	TW_OBJRES_ID
//--------------------------------------------------------------
typedef struct
{
	u16 mdl_id;
	u16 anm_id;
	u32 share_type;
}TW_OBJRES_ID;

//--------------------------------------------------------------
///	TW_MDLRES
//--------------------------------------------------------------
typedef struct
{
	u32 id;
	FRO_MDL rmdl;
}TW_MDLRES;

//--------------------------------------------------------------
///	TW_ANMRES
//--------------------------------------------------------------
typedef struct
{
	u32 id;
	void *anm_res;
}TW_ANMRES;

//--------------------------------------------------------------
///	TW_OBJRES
//--------------------------------------------------------------
typedef struct
{
	u16 use;
	u16 type;
	FRO_OBJ robj;
	FRO_ANM ranm;
}TW_OBJRES;

//--------------------------------------------------------------
///	TW_OBJRESDATA_WORK
//--------------------------------------------------------------
typedef struct
{
	TW_MDLRES mdl_tbl[TW_MDL_MAX];
	TW_ANMRES anm_tbl[TW_ANM_MAX];
	TW_OBJRES obj_tbl[TW_OBJ_MAX];
}TW_OBJRESDATA_WORK;

//--------------------------------------------------------------
///	EOA_FSTONE_HEAD
//--------------------------------------------------------------
typedef struct
{
	s16 no;
	s16 zone_id;
	u16 main_lower_flag;
	u16 dmy;
	TW_FSTONE_PUTPOS putpos;
	TORNWORLD_WORK *tw;
}EOA_FSTONE_HEAD;

//--------------------------------------------------------------
///	TW_FSTONE_WORK
//--------------------------------------------------------------
typedef struct
{
	u32 vanish_flag;
	s32 put_max;
	EOA_PTR *eoa_tbl_p;
}TW_FSTONE_WORK;

//--------------------------------------------------------------
///	TW_DMYMAP
//--------------------------------------------------------------
typedef struct
{
	int zone_id;
	int init_flag;
	u32 draw_flag;
	ARCHANDLE *arc_handle;
	WORLD_MAP_PTR world;
	MAP_RESOURCE_PTR map_res;
	DMC_PTR map_cont;
}TW_DMYMAP;

//--------------------------------------------------------------
///	EOA_LINKOBJ_HEAD
//--------------------------------------------------------------
typedef struct
{
	s16 gx;
	s16 gy;
	s16 gz;
	u16 objtype;
	u32 flagcheck;
	TORNWORLD_WORK *tw;
}EOA_LINKOBJ_HEAD;

//--------------------------------------------------------------
///	EOA_LINKOBJ_WORK
//--------------------------------------------------------------
typedef struct
{
	u8 alpha_on;
	u8 vanish;
	u8 alpha;
	u8 alpha_frame;
	
	VecFx32 pos;
	EOA_LINKOBJ_HEAD head;
	TW_OBJRES *objres_p;
}EOA_LINKOBJ_WORK;

#define LPOS_NEXT_MOVENO_END (0xffff)

//--------------------------------------------------------------
///	LINKPOS_ZONECHGMOVEDATA
//--------------------------------------------------------------
typedef struct
{
	u16 id;				//動作ID
	u16 next_id;		//次の動作ID
	
	s16 end_gx;			//移動量終端GX
	s16 end_gy;			//移動量終端GY
	s16 end_gz;			//移動量終端GZ
	s16 chg_gx;			//切り替え移動量GX
	s16 chg_gy;			//切り替え移動量GY
	s16 chg_gz;			//切り替え移動量GZ
	
	VecFx32 move_val;	//移動量
	
	u16 zlink_mflag_on;	//移動後、ONにするフラグ
	u16 zlink_mflag_off;//移動後、OFFにするフラグ
}LINKPOS_ZONECHGMOVEDATA;

//--------------------------------------------------------------
///	LINKPOS_DATA ゾーン接続配置位置
//--------------------------------------------------------------
typedef struct
{
	u16 id;				//識別ID
	s16 gx;				//配置GX
	s16 gy;				//配置GY
	s16 gz;				//配置GZ
	u16 mdata_id;		//動作ID
	u16 move_type;		//動作タイプ
	u32 zone_chg_id;	//ゾーンチェンジ後のID
	u32 objtype;		//表示OBJタイプ TWOBJTYPE
	u32 zlink_mflag_off_stop; //立っていない場合は追加を停止するZLINKMFLAG
}LINKPOS_DATA;

//--------------------------------------------------------------
///	TW_LINKPOS_ZONEDATA ゾーン別リンクデータ
//--------------------------------------------------------------
typedef struct
{
	u32 zone_id;
	const LINKPOS_DATA **pos_tbl;
}TW_LINKPOS_ZONEDATA;

//--------------------------------------------------------------
///	TW_LINKPOS_ACTOR
//--------------------------------------------------------------
typedef struct
{
	u8 use;
	u8 zone_chg_deloff;
	u16 live_zone_id;
	
	LINKPOS_DATA ldata;
	FIELD_OBJ_PTR fldobj;
	EOA_PTR eoa;
}TW_LINKPOS_ACTOR;

#define TW_LINKPOS_ACTOR_MAX (32)

//--------------------------------------------------------------
///	TW_LINKPOS_WORK
//--------------------------------------------------------------
typedef struct
{
	TW_LINKPOS_ACTOR act_tbl[TW_LINKPOS_ACTOR_MAX];
}TW_LINKPOS_WORK;

//--------------------------------------------------------------
///	TW_PROC_WORK
//--------------------------------------------------------------
typedef struct
{
	TCB_PTR tcb_proc;
}TW_PROC_WORK;

//--------------------------------------------------------------
///	TW_ZONE_LINKDATA
//--------------------------------------------------------------
typedef struct
{
	u32 zone_id;					///<対象zone ID
	u32 upper_zone_id;				///<上層zone id
	u32 lower_zone_id;				///<下層zone_id
}TW_ZONE_LINKDATA;

//--------------------------------------------------------------
///	PUTOBJ_DATA
//--------------------------------------------------------------
typedef struct
{
	u32 obj_id;
	u16 obj_type;	//TWOBJTYPE_MAX==終端
	s16 gx;
	s16 gy;
	s16 gz;
	u16 check_flag_type;
	u16 check_flag;
}PUTOBJ_DATA;

//--------------------------------------------------------------
///	PUTOBJ_ZONEDATA
//--------------------------------------------------------------
typedef struct
{
	u32 zone_id;
	const PUTOBJ_DATA *pobj_tbl;
}PUTOBJ_ZONEDATA;

//--------------------------------------------------------------
///	TW_PUTOBJ_SET
//--------------------------------------------------------------
typedef struct
{
	u16 zone_id;
	EOA_PTR eoa;
	PUTOBJ_DATA data;
}TW_PUTOBJ_SET;

//--------------------------------------------------------------
///	TW_PUTOBJ_WORK
//--------------------------------------------------------------
typedef struct
{
	TW_PUTOBJ_SET objset_tbl[TW_PUTOBJ_MAX];
}TW_PUTOBJ_WORK;

//--------------------------------------------------------------
///	EOA_PUTOBJ_HEAD
//--------------------------------------------------------------
typedef struct
{
	TORNWORLD_WORK *tw;
	const TW_PUTOBJ_SET *poset_p;
}EOA_PUTOBJ_HEAD;

//--------------------------------------------------------------
///	ギラティナ影フラグ
//--------------------------------------------------------------
enum
{
	GIRASFLAG_0500 = 0,
	GIRASFLAG_0501,
};

//--------------------------------------------------------------
///	TWFLAGTYPE
//--------------------------------------------------------------
enum
{
	TWFLAGTYPE_NON,				//特に無し
	TWFLAGTYPE_EV_OFF,			//破れた世界ローカルフラグ OFF
	TWFLAGTYPE_EV_ON,			//破れた世界ローカルフラグ ON
	TWFLAGTYPE_TWEVSEQ_EQ,		//破れた世界TWEVSEQ イコール
	TWFLAGTYPE_TWEVSEQ_LOW,		//破れた世界TWEVSEQ 低い
	TWFLAGTYPE_TWEVSEQ_HI,		//破れた世界TWEVSEQ 高い
	TWFLAGTYPE_EVENT,			//スクリプトからの追加 FLDOBJ専用
	TWFLAGTYPE_GIRASFLAG_OFF,	//sysflag ギラティナ影OFF
	TWFLAGTYPE_AKAGI_FLAG_EQ,	//SYS_WORK_TW_AKAGI_APPEAR イコール
};

enum
{
	TWFLDOBJ_ROT_OFF = 0,
	TWFLDOBJ_ROT_ON,
};

//--------------------------------------------------------------
///	TW_FLDOBJ_HEAD
//--------------------------------------------------------------
typedef struct
{
	u16 check_flag_type;		///<TWFLAGTYPE_NON
	u16 check_flag;				///<check_flag_type参照値
	u16 obj_rotate_flag;		///<OBJRotate設定 TWFLDOBJ_ROT_OFF等
	u16 obj_rotate;				///<OBJRotate設定 角度0-359
	FIELD_OBJ_H head;			///<FIELD_OBJ_H
}TW_FLDOBJ_HEAD;

//--------------------------------------------------------------
///	TW_FLDOBJ_ZONEDATA
//--------------------------------------------------------------
typedef struct
{
	u32 zone_id;
	const TW_FLDOBJ_HEAD **head_tbl;
}TW_FLDOBJ_ZONEDATA;

//--------------------------------------------------------------
///	TW_FLDOBJ_WORK
//--------------------------------------------------------------
typedef struct
{
	FIELD_OBJ_PTR fldobj_tbl[TW_FLDOBJ_MAX];
}TW_FLDOBJ_WORK;

//--------------------------------------------------------------
///	SCRDATA_LIFTMOVE
//--------------------------------------------------------------
typedef struct
{
	u16 move_id;
	u16 jiki_move_flag;
	u16 after_jiki_type;
	s16 gx_val;
	s16 gy_val;
	s16 gz_val;
	VecFx32 move_val;
}SCRDATA_LIFTMOVE;

//--------------------------------------------------------------
///	SCRDATA_ACMD
//--------------------------------------------------------------
typedef struct
{
	u32 obj_id;
	u32 code;
}SCRDATA_ACMD;

//--------------------------------------------------------------
///	TW_SCRDATA
//--------------------------------------------------------------
typedef struct
{
	TWSCRCODE code;
	const void *cmddata_p;
}TW_SCRDATA;

//--------------------------------------------------------------
///	TW_SCRWORK
//--------------------------------------------------------------
typedef struct
{
	u32 flag;
	u16 scr_tbl_no;
	u16 scr_seq_no;
	const TW_SCRDATA *scrdata_p;
	ALIGN4 u8 proc_work[TW_SCRPROCWORK_SIZE];
}TW_SCRWORK;

//--------------------------------------------------------------
///	TW_SCR_PROC
///	tw	TORNWORLD_WORK
///	seq_no	処理シーケンス
///	data	処理データ
//--------------------------------------------------------------
typedef PROCRET (*TW_SCR_PROC)( TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data );

//--------------------------------------------------------------
///	SCRPOSDATA
//--------------------------------------------------------------
typedef struct
{
	s16 gx;
	s16 gy;
	s32 gz;
	u16 check_flag_type;		//TWFLAGTYPE
	u16 check_flag;				//type match flag
	const TW_SCRDATA *scr_p;
}SCRPOSDATA;

//--------------------------------------------------------------
///	SCRPOSDATA_ZONE
//--------------------------------------------------------------
typedef struct
{
	u32 zone_id;
	const SCRPOSDATA *scrpos;
}SCRPOSDATA_ZONE;

//--------------------------------------------------------------
///	SCRDATA_GIRAS
//--------------------------------------------------------------
typedef struct
{
	s16 gx;
	s16 gy;
	s16 gz;
	s8 dir;
	u8 se_play;
	VecFx32 scale;
	VecFx32 move_val;
	int frame;
}SCRDATA_GIRAS;

//--------------------------------------------------------------
///	GIRASACT_WORK
//--------------------------------------------------------------
typedef struct
{
	u32 gp_init;
	TW_OBJRES *objres_p;
	EOA_PTR eoa;
}GIRASACT_WORK;

//--------------------------------------------------------------
///	TW_ZONECHG	※旧
//--------------------------------------------------------------
typedef struct
{
	u8 zchg_type;
	u8 seq_no;
	u8 list_count;
	u8 dmy;
	
	int gx;
	int gz;
	int block_no_tbl[4];
	TW_LINKPOS_ACTOR *lact;
}TW_ZONECHG;

//--------------------------------------------------------------
///	TW_ZONECHG_WORK
//--------------------------------------------------------------
typedef struct
{
	u32 chg_type;	//TWZONECHGTYPE_NON等
	
	u16 seq_no;
	u16 list_count;
	s16 jiki_gx;
	s16 jiki_gz;
	int next_zone_id;
	int block_no_tbl[4];
	
	int vintr_trans_flag;
	MAP_RESOURCE_PTR map_res_trans_p;
	
	TW_LINKPOS_ACTOR *lact;
	TCB_PTR tcb;
}TW_ZONECHG_WORK;

//--------------------------------------------------------------
///	TORNWORLD_WORK
//--------------------------------------------------------------
struct _TORNWORLD_WORK
{
	FIELDSYS_WORK *fsys;
	FLD_TORNWORLD_WORK *gmc_tw;
	
	ARCHANDLE *arc_h_map;
	ARCHANDLE *arc_h_attr;
	ARCHANDLE *arc_h_etc;
	
	TW_CAMERA_WORK camera;
	TW_EVENT_WORK event;
	TW_SCRWORK script;
	TW_PROC_WORK proc_work;
	TW_OBJ_ROT_PROC_WORK objrot;
	
	TW_CLACT_SETDATA clact_set;
	TW_OBJRESDATA_WORK ores;
	
	TW_MDATA mdata;
	TW_FSTONE_WORK fstone;
	TW_FSTONE_WORK fstone_lower;
	TW_LINKPOS_WORK linkpos;
	TW_PUTOBJ_WORK putobj;
	TW_FLDOBJ_WORK twfldobj;
	
	TW_ZONECHG old_zone_chg;
	TW_ZONECHG_WORK zone_chg;
	TW_PLTT_WORK pltt;
	
	TW_DMYMAP dmy_map;
	GIRASACT_WORK giras_act;
	GXRgb edge_color_tbl[TW_EDGE_COLOR_MAX];
	
	#ifdef TW_DEBUG
	TW_DEBUG_WORK debug;
	#endif
	
	u16 draw_giratina_fade_flag;
	u16 cloud_speed_lv;
	TCB_PTR tcb_vintr_p;
};

//==============================================================================
//	proto
//==============================================================================
//debug
#ifdef DEBUG_TWORLD_CAPTURE
static int DebugTWorldCaptureFlag_StepVanish;
static int DebugTWorldCaptureFlag_CloudStop;
static int DebugTWorldCaptureFlag_BGVanish;
#endif

//main
//static void tw_ZoneChange( TORNWORLD_WORK *work );
static void tw_ZoneChangeDown( TORNWORLD_WORK *work );
static void tw_ZoneChangeUp( TORNWORLD_WORK *work );

//arc
static void tw_ArcHandleOpen( TORNWORLD_WORK *work );
static void tw_ArcHandleClose( TORNWORLD_WORK *work );

//save
static void tw_SaveDataEventFlagInit( TORNWORLD_WORK *tw );
static void tw_SaveDataCameraAngleSet(
		TORNWORLD_WORK *tw, u16 x, u16 y, u16 z );
static void tw_SaveDataCameraAngleGet(
		TORNWORLD_WORK *tw, u16 *x, u16 *y, u16 *z );
static BOOL tw_SaveDataInitFinishCheck( TORNWORLD_WORK *tw );
static void tw_SaveDataFStoneFlagSet( TORNWORLD_WORK *tw,u32 flag );
static u32 tw_SaveDataFStoneFlagGet( TORNWORLD_WORK *tw );
static void tw_SaveDataZLinkMoveFlagSet( TORNWORLD_WORK *tw, u32 flag );
static void tw_SaveDataZLinkMoveFlagON( TORNWORLD_WORK *tw, u32 flag );
static void tw_SaveDataZLinkMoveFlagOFF( TORNWORLD_WORK *tw, u32 flag );
static BOOL tw_SaveDataZLinkMoveFlagCheck( TORNWORLD_WORK *tw, u32 flag );
static void tw_SaveDataEventFlagON( TORNWORLD_WORK *tw, u32 flag_no );
static void tw_SaveDataEventFlagOFF( TORNWORLD_WORK *tw, u32 flag_no );
static BOOL tw_SaveDataEventFlagCheck( TORNWORLD_WORK *tw, u32 flag_no );
static void tw_SaveDataSpGroundNoSet( TORNWORLD_WORK *tw, u32 no );
static u32 tw_SaveDataSpGroundNoGet( TORNWORLD_WORK *tw );

//proc
static void tw_ProcInit( TORNWORLD_WORK *work );
static void tw_ProcDelete( TORNWORLD_WORK *work );
static void tw_ProcTcb( TCB_PTR tcb, void *wk );

//vintr tcb
static void tw_VIntrTCBInit( TORNWORLD_WORK *tw );
static void tw_VIntrTCBDelete( TORNWORLD_WORK *tw );
static void tw_VIntrTCB( TCB_PTR tcb, void *wk );

//graphic
static void tw_GraphicInit( TORNWORLD_WORK *tw );
static void tw_GraphicDelete( TORNWORLD_WORK *tw );
static void tw_GrapEdgeColorSet( TORNWORLD_WORK *tw, u32 no, u32 col );
static GXRgb tw_GrapEdgeColorGet( TORNWORLD_WORK *tw, u32 no );

//fieldobj
static void tw_FldOBJParamInit( TORNWORLD_WORK *tw );
static void tw_FldOBJParamDelete( TORNWORLD_WORK *tw );

//camera action
static void tw_CameraInit( TORNWORLD_WORK *tw );
static void tw_CameraDelete( TORNWORLD_WORK *tw );
static void tw_CameraActionTCB( TCB_PTR tcb, void *wk );
static BOOL tw_CameraActionSet(
	TORNWORLD_WORK *tw, const TW_CMACTDATA *data );

//obj rotate
static void tw_OBJRotateProc_Init( TORNWORLD_WORK *tw, int max );
static void tw_OBJRotateAct_BlActResSet( TW_OBJ_ROT_PROC_WORK *work,
		TW_OBJ_ROT_ACT_WORK *act, BLACT_WORK_PTR blact);
static void tw_OBJRotateAct_BlActResFreeOnly(
		TW_OBJ_ROT_PROC_WORK *work,	TW_OBJ_ROT_ACT_WORK *act );
static void tw_OBJRotateAct_BlActResFree(
		TW_OBJ_ROT_PROC_WORK *work,	TW_OBJ_ROT_ACT_WORK *act );
static void tw_OBJRotateProc_Delete( TORNWORLD_WORK *tw );
static void tw_OBJRotate_RegOBJ(
	TORNWORLD_WORK *tw, FIELD_OBJ_PTR fldobj, int rot );
static void tw_OBJRotate_RegBlAct(
	TORNWORLD_WORK *tw, BLACT_WORK_PTR blact, int rot );
static void tw_OBJRotate_BlActUseWorkFree(
		TORNWORLD_WORK *tw, CONST_BLACT_WORK_PTR blact );
static void tw_OBJRotateProcTcbCall( TORNWORLD_WORK *tw );
static void BlActOBJRotateDraw( BLACT_WORK_PTR blact, void *wk );
static void tw_OBJRotateAnmSet(
	TORNWORLD_WORK *tw, FIELD_OBJ_PTR fldobj, int rot, int frame );

#if 0
static void tw_OBJRotateProc_Init( TORNWORLD_WORK *tw, int max );
static void tw_OBJRotateProc_ActMdlInit(
		TORNWORLD_WORK *tw, BLACT_WORK_PTR act );
static void tw_OBJRotateProc_Delete( TORNWORLD_WORK *tw );
static void tw_OBJRotate_RegOBJ(
		TORNWORLD_WORK *tw, FIELD_OBJ_PTR fldobj, int rot );
static void tw_OBJRotateProc_RegOBJProc( TORNWORLD_WORK *tw );
static void tw_OBJRotate_ModelSet(
	TORNWORLD_WORK *tw, FIELD_OBJ_PTR fldobj, int rot );
static void BlActOBJRotateDraw( BLACT_WORK_PTR act, void *wk );
static void tw_OBJRotateProcTcbCall( TORNWORLD_WORK *tw );
static void tw_OBJRotateAnm( TW_OBJ_ROT_WORK *work );
static void tw_OBJRotateAnmSet(
	TORNWORLD_WORK *tw, FIELD_OBJ_PTR fldobj, int rot, int frame );
#endif

//event
static void tw_EventInit( TORNWORLD_WORK *tw );
static void tw_EventDelete( TORNWORLD_WORK *tw );
static void * tw_EventWorkInit( TORNWORLD_WORK *tw, int size );
static void * tw_EventWorkGet( TORNWORLD_WORK *tw );

//camera action event
static BOOL tw_EventMoveCheck_PosCameraAction(
	TORNWORLD_WORK *tw, int gx, int gy, int gz, int dir );

//jiki
static void tw_JikiInit( TORNWORLD_WORK *tw );
static void tw_JikiDelete( TORNWORLD_WORK *tw );

//jiki event
static BOOL tw_EventMoveCheck_PosJikiEvent(
		TORNWORLD_WORK *tw, int gx, int gy, int gz, int dir );

//jiki event wall jump
static void tw_JikiEventSet_WallJump(
		TORNWORLD_WORK *tw, const TW_EVJUMPDATA *data );
static BOOL tw_GMEVENT_JikiWallJump( GMEVENT_CONTROL *event );
static BOOL JikiJumpWall(
		TW_JIKI_WALLJUMP_WORK *work, FIELD_OBJ_PTR fldobj );

//bg
static void tw_BGInit( TORNWORLD_WORK *tw );
static void tw_BGDelete( TORNWORLD_WORK *tw );

//clact
static void tw_ClActInit(
		TORNWORLD_WORK *tw, TW_CLACT_SETDATA *set, ARCHANDLE *hd );
static void tw_ClActDelete( TW_CLACT_SETDATA *set );
static void tw_ClActDrawTcb( TCB_PTR tcb, void *wk );
static CLACT_WORK_PTR tw_ClActAdd(
		TW_CLACT_SETDATA *set, const VecFx32 *pos,
		u32 char_id, u32 pltt_id, u32 cell_id, u32 anm_id,
		int bg_pri, int act_pri );

//eoa cloud
static void tw_EoaCloudSet(
	TORNWORLD_WORK *tw, FE_SYS *fes, TW_CLACT_SETDATA *set );
static const EOA_H_NPP DATA_EoaCloudHeader;

//fog
static void tw_FogInit( TORNWORLD_WORK *tw );
static void tw_FogDelete( TORNWORLD_WORK *tw );

//fstone
static void tw_FStoneInitMain( TORNWORLD_WORK *tw,
	TW_FSTONE_WORK *fstone, const TW_FSTONE_HEADER *head,
	const TW_FSTONE_PUTPOS *putpos, int zone_id, u32 vanish_flag );
static void tw_FStoneInitNow( TORNWORLD_WORK *tw, int init_vns );
static void tw_FStoneInitLower( TORNWORLD_WORK *tw );
static void tw_FStoneInit( TORNWORLD_WORK *tw );
static void tw_FStoneDeleteMain( TW_FSTONE_WORK *fstone );
static void tw_FStoneDelete( TORNWORLD_WORK *tw );
static void tw_FStoneVanishFlagON( TORNWORLD_WORK *tw, u32 no );
static void tw_FStoneVanishFlagOFF( TORNWORLD_WORK *tw, u32 no );
static BOOL tw_FStoneVanishFlagCheck( TORNWORLD_WORK *tw, u32 no );
static BOOL tw_FStoneVanishFlagCheckLower( TORNWORLD_WORK *tw, u32 no );
static void tw_FStoneVanishFlagSaveDataSet( TORNWORLD_WORK *tw );
static void tw_FStonePut( TORNWORLD_WORK *tw,
	TW_FSTONE_WORK *fstone, const TW_FSTONE_PUTPOS *put, int zone_id );
static EOA_PTR tw_FStoneAdd(
		TORNWORLD_WORK *tw, int no, int zone_id,
		const TW_FSTONE_PUTPOS *pos );
static void tw_EventMoveCheck_FStoneAdvent(
	TORNWORLD_WORK *tw, int gx, int gy, int gz, int dir );
static BOOL tw_FStoneOBJResMdlIDSearch( TORNWORLD_WORK *tw, u32 mdl_id );
static BOOL tw_FStoneOBJResAnmIDSearch( TORNWORLD_WORK *tw, u32 anm_id );
static BOOL tw_FStoneOBJResTypeSearch( TORNWORLD_WORK *tw, TWOBJTYPE c_type );
static void tw_FStoneDeleteNow( TORNWORLD_WORK *tw );
static void tw_FStoneDeleteLower( TORNWORLD_WORK *tw );
static void tw_FStoneCleanNow( TORNWORLD_WORK *tw );
static void tw_FStoneCleanLower( TORNWORLD_WORK *tw );
static void tw_FStoneRemoveNowLower( TORNWORLD_WORK *tw );
static void tw_FStoneRemoveLowerNow( TORNWORLD_WORK *tw );

//fstone eoa
//static void tw_FStoneAlphaSet(
//	TORNWORLD_WORK *tw, int type, int alpha, int pid );
//static void tw_FStoneDraw( TORNWORLD_WORK *tw, int type, VecFx32 *pos );
static u16 tw_EoaFStoneTypeGet( EOA_PTR eoa );
//static void tw_FStoneTypeRegist( TORNWORLD_WORK *tw, u16 type );
//static void tw_FStoneTypeEras( TORNWORLD_WORK *tw, u16 type );

#if 0
//lift
static BOOL tw_EventMoveCheck_Lift(
		TORNWORLD_WORK *tw, int gx, int gy, int gz );
static void tw_EventSet_LiftMove(
		TORNWORLD_WORK *tw, TW_LINKPOS_ACTOR *lact );
static BOOL tw_GMEVENT_LiftMove( GMEVENT_CONTROL *event );
#endif

//map data
static void tw_MapDataInit( TORNWORLD_WORK *tw );
static void tw_MapDataReset( TORNWORLD_WORK *tw );
static void tw_MapDataDelete( TORNWORLD_WORK *tw );
static void tw_MapDataRemoveNowLower( TORNWORLD_WORK *tw, u32 zone_id );
static void tw_MapDataRemoveLowerNow( TORNWORLD_WORK *tw, u32 zone_id );
static void tw_MapDataIdxInitMain(
		ARCHANDLE *arc_h_map, TW_MDATA_IDX *md_idx );
static void tw_MapDataIdxInit( TORNWORLD_WORK *tw );
static void tw_MapDataIdxDeleteMain( TW_MDATA_IDX *md_idx );
static void tw_MapDataIdxDelete( TORNWORLD_WORK *tw );
static const TW_MDIDX * tw_MapDataIdxZoneIDMdIdxGetMain(
		const TW_MDATA_IDX *md_idx, int zone_id );
static const TW_MDIDX * tw_MapDataIdxZoneIDMdIdxGet(
		TORNWORLD_WORK *tw, int zone_id );
static u32 tw_MapDataIdxZoneIDIdxGet( TORNWORLD_WORK *tw, int zone_id );
static void tw_MapDataIdxZoneIDOriginGridPosGetMain(
		const TW_MDATA_IDX *md_idx, int zone_id, int *x, int *y, int *z );
static void tw_MapDataIdxZoneIDOriginGridPosGet(
		TORNWORLD_WORK *tw, int zone_id, int *x, int *y, int *z );
static void tw_MapDataIdxZoneIDOriginPosGet(
		TORNWORLD_WORK *tw, int zone_id, VecFx32 *pos );
static void tw_ZoneDataBufInitMain(
	TORNWORLD_WORK *tw, TW_ZONEDATA_BUF *buf, int zone_id );
static void tw_ZoneDataBufInit(
		TORNWORLD_WORK *tw, int zone_id, int l_zone_id );
static void tw_ZoneDataBufInitNow( TORNWORLD_WORK *tw, int zone_id );
static void tw_ZoneDataBufInitLower( TORNWORLD_WORK *tw, int zone_id );
static void tw_ZoneDataBufDeleteMain( TW_ZONEDATA_BUF *buf );
static void tw_ZoneDataBufDelete( TORNWORLD_WORK *tw );
static void tw_ZoneDataBufDeleteNow( TORNWORLD_WORK *tw );
static void tw_ZoneDataBufDeleteLower( TORNWORLD_WORK *tw );
static void tw_ZoneDataBufRemoveNowLower( TORNWORLD_WORK *tw );
static void tw_ZoneDataBufRemoveLowerNow( TORNWORLD_WORK *tw );
static BOOL tw_ZoneDataBufValidCheck( TW_ZONEDATA_BUF *buf );
static int tw_ZoneDataBufGndDataSizeGet( TW_ZONEDATA_BUF *buf );
static int tw_ZoneDataBufGndDataCountGet( TW_ZONEDATA_BUF *buf );
static TW_GNDDATA * tw_ZoneDataBufGndDataGet( TW_ZONEDATA_BUF *buf );
static int tw_ZoneDataBufEvJumpDataSizeGet( TW_ZONEDATA_BUF *buf );
static int tw_ZoneDataBufEvJumpDataCountGet( TW_ZONEDATA_BUF *buf );
static TW_EVJUMPDATA * tw_ZoneDataBufEvJumpDataGet( TW_ZONEDATA_BUF *buf );
static int tw_ZoneDataBufCmActDataSizeGet( TW_ZONEDATA_BUF *buf );
static int tw_ZoneDataBufCmActDataCountGet( TW_ZONEDATA_BUF *buf );
static TW_CMACTDATA * tw_ZoneDataBufCmActDataGet( TW_ZONEDATA_BUF *buf );
static int tw_ZoneDataBufFStoneDataSizeGet( TORNWORLD_WORK *tw );
static int tw_ZoneDataBufFStoneDataSizeGetLower( TORNWORLD_WORK *tw );
static const TW_FSTONE_HEADER * tw_ZoneDataBufFStoneHeaderGet(
		TW_ZONEDATA_BUF *buf );
static const TW_FSTONE_PUTPOS * tw_ZoneDataBufFStonePutPosDataGet(
		TW_ZONEDATA_BUF *buf );
static const TW_FSTONE_FLAGPOS * tw_ZoneDataBufFStoneFlagPosDataGet(
		TW_ZONEDATA_BUF *buf );
static void tw_MapDataGndDataInit( TORNWORLD_WORK *tw );
static void tw_MapDataGndDataDelete( TORNWORLD_WORK *tw );
static void tw_MapDataGndDataPosDataSet(
	TORNWORLD_WORK *tw, int gx, int gy, int gz, s16 type );
static void tw_MapDataGndDataNoSet( TORNWORLD_WORK *tw, u32 no );
static void tw_MapDataGndDataNoMaxSet( TORNWORLD_WORK *tw );
static u32 tw_MapDataGndDataGndTypeNowGet( TORNWORLD_WORK *tw );
static u32 tw_MapDataGndDataNowGndCheck( TORNWORLD_WORK *tw );
static u32 tw_MapDataNowGndTypeGet( TORNWORLD_WORK *tw );
static u32 tw_MapDataPosGndTypeGet(
		TORNWORLD_WORK *tw, int gx, int gy, int gz );
static void tw_MapDataGndDataAttrLoad( TORNWORLD_WORK *tw, u32 idx );
static u16 tw_MapDataGndDataAttrGetPos( TORNWORLD_WORK *tw, int gx, int gz );
static u16 tw_MapDataGndDataAttrGetCheckPos(
	TORNWORLD_WORK *tw, int gx, int gy, int gz );
static void tw_MapDataEvJumpDataInit( TORNWORLD_WORK *tw );
static void tw_MapDataEvJumpDataDelete( TORNWORLD_WORK *tw );
static const TW_EVJUMPDATA * tw_MapDataEvJumpDataPosCheck(
	TORNWORLD_WORK *tw, int gx, int gy, int gz, int dir );
static void tw_MapDataCmActDataInit( TORNWORLD_WORK *tw );
static void tw_MapDataCmActDataDelete( TORNWORLD_WORK *tw );
static const TW_CMACTDATA * tw_MapDataCmActDataPosCheck(
	TORNWORLD_WORK *tw, int gx, int gy, int gz, int dir );
static void tw_MapDataFStoneDataInitMain(
		TW_ZONEDATA_BUF *buf, TW_FSTONEDATA_TBL *fsdata);
static void tw_MapDataFStoneDataInitNow( TORNWORLD_WORK *tw );
static void tw_MapDataFStoneDataInitLower( TORNWORLD_WORK *tw );
static void tw_MapDataFStoneDataInit( TORNWORLD_WORK *tw );
static void tw_MapDataFStoneDataDeleteNow( TORNWORLD_WORK *tw );
static void tw_MapDataFStoneDataDelete( TORNWORLD_WORK *tw );
static void tw_MapDataFStoneDataDeleteLower( TORNWORLD_WORK *tw );
static void tw_MapDataFStoneDataRemoveNowLower( TORNWORLD_WORK *tw );
static void tw_MapDataFStoneDataRemoveLowerNow( TORNWORLD_WORK *tw );
static const TW_FSTONE_HEADER * tw_MapDataFStoneHeaderGet(
		TORNWORLD_WORK *tw );
static const TW_FSTONE_HEADER * tw_MapDataFStoneHeaderGetLower(
		TORNWORLD_WORK *tw );
static const TW_FSTONE_PUTPOS * tw_MapDataFStonePutPosTblGet(
		TORNWORLD_WORK *tw );
static const TW_FSTONE_PUTPOS * tw_MapDataFStonePutPosTblGetLower(
		TORNWORLD_WORK *tw );
static const TW_FSTONE_FLAGPOS * tw_MapDataFStoneFlagPutTblGet(
		TORNWORLD_WORK *tw );
static const TW_FSTONE_FLAGPOS * tw_MapDataFStoneFlagPutTblGetLower(
		TORNWORLD_WORK *tw );

//dummy map
static void tw_DmyMapInit( TORNWORLD_WORK *tw );
static void tw_DmyMapDelete( TORNWORLD_WORK *tw );
static void tw_DmyMapZoneChgDown(
		TORNWORLD_WORK *tw, TW_LINKPOS_ACTOR *lact );
static void tw_DmyMapZoneChgUp(
		TORNWORLD_WORK *tw, TW_LINKPOS_ACTOR *lact );
static void tw_DmyMapZoneChangeDownTask( TORNWORLD_WORK *tw );
static void tw_DmyMapZoneChangeUpTask( TORNWORLD_WORK *tw );
static void tw_DmyMapZoneChgUpTaskSet(
		TORNWORLD_WORK *tw, int gx, int gz, TW_LINKPOS_ACTOR *act );
static void tw_DmyMapZoneChgDownTaskSet(
		TORNWORLD_WORK *tw, int gx, int gz, TW_LINKPOS_ACTOR *act );

//zone change task
static void tw_ZoneChangeTaskInit( TORNWORLD_WORK *tw );
static void tw_ZoneChangeTaskDelete( TORNWORLD_WORK *tw );
static void tw_ZoneChangeTaskTCB( TCB_PTR tcb, void *wk );
static void tw_ZoneChangeTaskVIntr( TORNWORLD_WORK *tw );
static void tw_ZoneChangeTaskSet(
	TORNWORLD_WORK *tw, TW_LINKPOS_ACTOR *lact, u32 type );
static BOOL tw_ZoneChangeTaskCheck( TORNWORLD_WORK *tw );

//zone change event
static BOOL tw_EventMoveCheck_ZoneChange(
		TORNWORLD_WORK *tw, int gx, int gy, int gz );
static void tw_EventSet_ZoneChange(
		TORNWORLD_WORK *tw, TW_LINKPOS_ACTOR *lact );
static BOOL tw_GMEVENT_ZoneChange( GMEVENT_CONTROL *event );

//zone link
static const TW_ZONE_LINKDATA * tw_ZoneLinkDataGet( int zone_id );

//obj resource
static void tw_OBJResDataInit( TORNWORLD_WORK *tw );
static void tw_OBJResDataMdlRegist( TORNWORLD_WORK *tw, u32 id );
static void tw_OBJResDataMdlDelete( TORNWORLD_WORK *tw, u32 id );
static FRO_MDL * tw_OBJResDataMdlSearch( TORNWORLD_WORK *tw, u32 id );
static void tw_OBJResDataAnmRegist( TORNWORLD_WORK *tw, u32 id );
static void tw_OBJResDataAnmDelete( TORNWORLD_WORK *tw, u32 id );
static void tw_OBJResDataDelete( TORNWORLD_WORK *tw );
static void tw_OBJResDataOBJInit( TORNWORLD_WORK *tw,
	u32 mdl_id, u32 anm_id, FRO_OBJ *robj, FRO_ANM *ranm );
static void tw_OBJResDataOBJTypeInit(
	TORNWORLD_WORK *tw, TWOBJTYPE type, FRO_OBJ *robj, FRO_ANM *ranm );
static void tw_OBJResDataMdlAlphaSetMain(
	TORNWORLD_WORK *tw, TWOBJTYPE type, int alpha, int pid );
static void tw_OBJResDataMdlAlphaSet(
	TORNWORLD_WORK *tw, TWOBJTYPE type, int alpha );
static void tw_OBJResDataMdlPolIDSet(
	TORNWORLD_WORK *tw, TWOBJTYPE type, int pid );
static BOOL tw_OBJResDataTypeAnmCheck( TWOBJTYPE type );
static BOOL tw_OBJResDataTypeShareCheck( TWOBJTYPE type );
static void tw_OBJResDataClean( TORNWORLD_WORK *tw );
static TW_OBJRES * tw_OBJResDataOBJResGet(
		TORNWORLD_WORK *tw, u32 type, int *init_chk );
static void tw_OBJResDataOBJResRelease( TORNWORLD_WORK *tw, TW_OBJRES *obj );
static void tw_OBJResDataOBJResCheckRelease(
		TORNWORLD_WORK *tw, TW_OBJRES *obj );
static void tw_OBJResDataOBJResReleaseTypeAll(
		TORNWORLD_WORK *tw, TWOBJTYPE type );
#ifdef DEBUG_TW_OBJ_COUNT
static void DEBUG_tw_OBJResCountPrint( TORNWORLD_WORK *tw );
#endif
static BOOL tw_OBJResDataTypeCullingCheck(
	TORNWORLD_WORK *tw, TWOBJTYPE type, VecFx32 *pos );

//link pos
static void tw_LinkPosInit( TORNWORLD_WORK *tw );
static void tw_LinkPosDelete( TORNWORLD_WORK *tw );
static void tw_LinkPosZoneIDDeleteAll( TORNWORLD_WORK *tw, u32 zone_id );
static void tw_LinkPosLoad( TORNWORLD_WORK *tw );
static void tw_LinkPosLoadZoneID( TORNWORLD_WORK *tw, u32 zone_id );
static void tw_LinkPosZoneDataLoad(
	TORNWORLD_WORK *tw, const TW_LINKPOS_ZONEDATA *zlink, u32 zone_id );
static void tw_LinkPosAddZoneIDNo( TORNWORLD_WORK *tw, u32 zone_id, int no );
static TW_LINKPOS_ACTOR * tw_LinkPosSpaceActGet( TORNWORLD_WORK *tw );
static TW_LINKPOS_ACTOR * tw_LinkPosActIDGet(
		TORNWORLD_WORK *tw, u32 id, u32 zone_id );
static const TW_LINKPOS_ZONEDATA * tw_LinkPosZoneDataGet( u32 zone_id );
static const LINKPOS_DATA * tw_ZoneLinkPosIDDataGet( u32 zone_id, u32 id );
#if 0
static const LINKPOS_HITDATA * tw_ZoneLinkPosMoveDataGet(
		u32 zone_id, u32 no );
#endif
#if 0
static const LINKPOS_ZONECHGDATA * tw_ZoneLinkPosZoneChgDataGet(
		u32 zone_id, u32 no );
#endif
static const LINKPOS_ZONECHGMOVEDATA * tw_ZoneLinkPosZoneChgMoveDataGet(
		u32 id );
static FIELD_OBJ_PTR tw_LinkPosActFldOBJAdd(
	TORNWORLD_WORK *tw, int gx, int gy, int gz,
	int data_no, int move_no, u32 zone_id );
static void tw_LinkPosActFldOBJRecover( TORNWORLD_WORK *tw,
	TW_LINKPOS_ACTOR *lact, FIELD_OBJ_PTR fldobj );
static EOA_PTR tw_LinkPosActEoaAdd(
		TORNWORLD_WORK *tw, TW_LINKPOS_ACTOR *lact );
static void tw_LinkPosActAdd( TORNWORLD_WORK *tw,
	TW_LINKPOS_ACTOR *lact, const LINKPOS_DATA *ldata, u32 zone_id );
static void tw_LinkPosActDelete(
		TORNWORLD_WORK *tw, TW_LINKPOS_ACTOR *lact );
static void tw_LinkPosActDeleteAll(
		TORNWORLD_WORK *tw, TW_LINKPOS_ACTOR *lact );
static void tw_LinkPosActZoneChangePerm(
	TORNWORLD_WORK *tw, TW_LINKPOS_ACTOR *lact, u32 zone_id );
static void tw_LinkPosActZoneChgDelNotFlagSet(
		TW_LINKPOS_ACTOR *lact, u32 flag );
static BOOL tw_LinkPosOBJResMdlIDSearch( TORNWORLD_WORK *tw, u32 mdl_id );
static BOOL tw_LinkPosOBJResAnmIDSearch( TORNWORLD_WORK *tw, u32 anm_id );
static BOOL tw_LinkPosOBJResTypeSearch(TORNWORLD_WORK *tw, TWOBJTYPE type);
static TW_LINKPOS_ACTOR * tw_LinkPosActPosCheck(
		TORNWORLD_WORK *tw, int gx, int gy, int gz, u32 zone_id );
static void tw_LinkPosActUpdate( TW_LINKPOS_ACTOR *lact );
//link pos eoa
static VecFx32 * tw_EoaLinkOBJPosPtrGet( EOA_PTR eoa );

static const EOA_H_NPP DATA_EoaLinkOBJHead;

//script
static void tw_ScrInit( TORNWORLD_WORK *tw );
static void tw_ScrDelete( TORNWORLD_WORK *tw );
static void tw_ScrSet( TORNWORLD_WORK *tw, const TW_SCRDATA *scrdata );
static void * tw_ScrProcWorkInit( TORNWORLD_WORK *tw, u32 size );
static void * tw_ScrProcWorkGet( TORNWORLD_WORK *tw );
static BOOL tw_ScrMain( TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev );

//script pos
static const SCRPOSDATA * tw_ScrPosDataZoneGet( u32 zone_id );
static BOOL tw_EventPosCheck_Script(
		TORNWORLD_WORK *tw, int gx, int gy, int gz );
static void tw_EventPosScrSetMain(
		TORNWORLD_WORK *tw, const TW_SCRDATA *scr_p );
static void tw_EventPosScrSet(
		TORNWORLD_WORK *tw, const SCRPOSDATA *scrpos );
static BOOL tw_GMEVENT_ScrPosWait( GMEVENT_CONTROL *event );

//gratina shadow
static void tw_GirasGraphicInit( TORNWORLD_WORK *tw );
static void tw_GirasGraphicDelete( TORNWORLD_WORK *tw );
static void tw_GirasActAdd(
	TORNWORLD_WORK *tw, const SCRDATA_GIRAS *data_p );
static BOOL tw_GirasActMoveWait( TORNWORLD_WORK *tw );
static void tw_GirasActInit( TORNWORLD_WORK *tw );
static void tw_GirasActDelete( TORNWORLD_WORK *tw );
static BOOL tw_GirasActOBJResMdlIDSearch( TORNWORLD_WORK *tw, u32 mdl_id );
static BOOL tw_GirasActOBJResAnmIDSearch( TORNWORLD_WORK *tw, u32 anm_id );
static BOOL tw_GirasActOBJResTypeSearch( TORNWORLD_WORK *tw, TWOBJTYPE type );
static const EOA_H_NPP DATA_EoaGirasHeader;

//putobj
static void tw_PutOBJInit( TORNWORLD_WORK *tw );
static void tw_PutOBJDelete( TORNWORLD_WORK *tw );
static void tw_PutOBJDeleteSet( TW_PUTOBJ_SET *poset );
static void tw_PutOBJAddTbl(
		TORNWORLD_WORK *tw, const PUTOBJ_ZONEDATA *data );
static void tw_PutOBJLoadZoneID( TORNWORLD_WORK *tw, u32 zone_id );
static TW_PUTOBJ_SET * tw_PutOBJSpaceGet( TORNWORLD_WORK *tw );
static void tw_PutOBJDeleteZoneID( TORNWORLD_WORK *tw, u32 zone_id );
static BOOL tw_PutOBJResTypeSearch( TORNWORLD_WORK *tw, TWOBJTYPE type );
static BOOL tw_PutOBJResMdlIDSearch( TORNWORLD_WORK *tw, u32 mdl_id );
static BOOL tw_PutOBJResAnmIDSearch( TORNWORLD_WORK *tw, u32 anm_id );

//putobj eoa
static const EOA_H_NPP DATA_EoaPutOBJHeader;

//fldobj
static void tw_FldOBJInit( TORNWORLD_WORK *tw );
static void tw_FldOBJDeleteAll( TORNWORLD_WORK *tw );
static void tw_FldOBJDelete( TORNWORLD_WORK *tw, FIELD_OBJ_PTR fldobj );
static FIELD_OBJ_PTR * tw_FldOBJSpaceGet( TORNWORLD_WORK *tw );
static BOOL tw_FldOBJAddFlagCheck(
	TORNWORLD_WORK *tw, const TW_FLDOBJ_HEAD *tw_head, u16 ev_flag );
static BOOL tw_FldOBJAdd( TORNWORLD_WORK *tw,
	FIELD_OBJ_PTR *fldobj, const TW_FLDOBJ_HEAD *tw_head,
	u32 zone_id, u16 ev_flag );
static BOOL tw_FldOBJAddTbl( TORNWORLD_WORK *tw,
	const TW_FLDOBJ_HEAD **tw_head_tbl, u32 zone_id );
static void tw_FldOBJLoadZoneID( TORNWORLD_WORK *tw, u32 zone_id );
static void tw_FldOBJDeleteZoneID( TORNWORLD_WORK *tw, u32 zone_id );
static FIELD_OBJ_PTR tw_FldOBJEventAdd(
		TORNWORLD_WORK *tw, u32 zone_id, u16 id );
static BOOL tw_FldOBJRegWorkCheck(
		TORNWORLD_WORK *tw, FIELD_OBJ_PTR fldobj );

//palette
static void tw_PlttInit( TORNWORLD_WORK *tw );
static void tw_PlttDelete( TORNWORLD_WORK *tw );
static void tw_PlttProc( TORNWORLD_WORK *tw );
static void tw_PlttVIntrProc( TORNWORLD_WORK *tw );
static void tw_PlttFadeSystemChg( TORNWORLD_WORK *tw, u32 flag );
static void tw_PlttFadeEvySet( TORNWORLD_WORK *tw, s16 evy );
static void ColorTargetFade( u16 base, u16 target, u16 evy, u16 *dest );

//splash effect
#ifdef TWTEST_TAKI_EFF
static EOA_PTR EoaSplash_Set( TORNWORLD_WORK *tw, const VecFx32 *pos );
static VecFx32 * EoaSplash_OffsetPosPtrGet( EOA_PTR eoa );
#endif

//parts
static BOOL twGridRectCheck(
	int gx, int gy, int gz, const TW_GRID_RECT_SIZE *rect );
#if 0
static BOOL twGridRectCheckOrg0(
	int gx, int gy, int gz, const TW_GRID_RECT_SIZE *rect );
#endif
static int twAngle16ShortDiffGet( u16 a, u16 ma );
static void twAngleAdd360( s16 *rot, s16 val );
static void twAngleAdd360Fx( fx32 *rot, fx32 val );
static void twJikiGridPosGet(
	TORNWORLD_WORK *tw, int *gx, int *gy, int *gz );
static void twZoneOffsetPosGet( int zone_id, VecFx32 *offs );
static u32 twNowZoneID( TORNWORLD_WORK *tw );
static HEROTWTYPE twGndTypeHeroType( u32 type );
static BOOL twFlagCheck( TORNWORLD_WORK *tw, u32 type, u32 flag );
static void twSndCheckPlay( u16 no );
static void twFldOBJVecPosUpdate(
		FIELD_OBJ_PTR fldobj, const VecFx32 *pos );

//debug
#ifdef DEBUG_TW_PRINTF_ON
static void DEBUG_twZoneIDPrintf( u32 zone_id );
#endif

#ifdef DEBUG_TW_PRINTF
static void DEBUG_twFldOBJTblPrintf( TORNWORLD_WORK *tw );
static void DEBUG_twFldOBJTblCheck( TORNWORLD_WORK *tw );
static void DEBUG_twFldOBJStatusPrintf( FIELD_OBJ_PTR fldobj );
static void DEBUG_twFldHeapPrintf( void );
#else
#define DEBUG_twFldHeapPrintf();
#endif

//data jump table
static const fx32 DATA_JumpOffsTblType0[OFFS_FRAME_MAX];

static void (* const DATA_JikiPosEventProcWallJumpTbl[EVPOSTYPE_MAX])(
		TORNWORLD_WORK *tw, const TW_EVJUMPDATA *data );

//data clact
static const int DATA_TwClActDataArcIdxTbl_Char[TW_CLACT_RES_CHAR_MAX];
static const int DATA_TwClActDataArcIdxTbl_Cell[TW_CLACT_RES_CELL_MAX];
static const int DATA_TwClActDataArcIdxTbl_Anm[TW_CLACT_RES_ANM_MAX];
static const int DATA_TwClActDataArcIdxTbl_Pltt[TW_CLACT_RES_PLTT_MAX];

static const CLACT_RESID DATA_CloudResIDTbl[CLOUD_MAX];

//data eoa
static const CLOUD_INITDATA DATA_EoaCloudInitDataTbl[CLOUD_SET_MAX];
static const fx32 DATA_CloudAddSpeedTbl[CLOUD_SPEED_LV_MAX][CLOUD_SPEED_TYPE_MAX];


//data fstone
//static const u32 DATA_FStoneArcIdxTbl[FSTONETYPE_MAX];
//static const EOA_H_NPP * const DATA_EoaFStoneHeaderTbl[FSTONETYPE_MAX];

//data model
static const u32 DATA_MdlArcIdxTbl[TW_MDL_MAX];
static const u32 DATA_AnmArcIdxTbl[TW_ANM_MAX];
static const TW_OBJRES_ID DATA_OBJResIDTbl[TWOBJTYPE_MAX];
//static const VecFx32 DATA_OBJPosOffsTbl[TWOBJTYPE_MAX];
static const VecFx32 DATA_OBJTypePosOffsTbl[TWOBJTYPE_MAX];
static const VecFx32 DATA_OBJTypeScaleTbl[TWOBJTYPE_MAX];
static const EOA_H_NPP * const DATA_EoaOBJTypeHeaderTbl[TWOBJTYPE_MAX];

//data zone link
static const TW_ZONE_LINKDATA DATA_TwZoneLinkDataTbl[TW_ZONECOUNT_MAX];

//data link pos
static const TW_LINKPOS_ZONEDATA DATA_TwZoneLinkPosTbl[TW_ZONELINK_MAX];
static const LINKPOS_ZONECHGMOVEDATA DATA_LinkPosZoneChgMoveTbl[ZCHG_MID_MAX];

//data script
static const TW_SCR_PROC * DATA_ScrCmdProcTbl[TWSCRCODE_MAX];
const SCRPOSDATA_ZONE DATA_ScrPosDataZoneTbl[];

const TW_SCRDATA DATA_Scr0105_Taki[];
const TW_SCRDATA Data_Scr0107_Taki[];

//data putobj
const PUTOBJ_ZONEDATA DATA_PutOBJZoneDataTbl[];

//data fldobj
const TW_FLDOBJ_ZONEDATA DATA_FldOBJZoneDataTbl[];

//==============================================================================
//	やぶれたせかい　呼び出し
//==============================================================================
//--------------------------------------------------------------
/**
 * やぶれたせかい　セットアップ
 * @param	fsys	FIELDSYS_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
void FldTornWorld_Setup( FIELDSYS_WORK *fsys )
{
	GIMMICKWORK *gmc;
	FLD_TORNWORLD_WORK *gmc_tw;
	TORNWORLD_WORK *work;
	
	gmc = SaveData_GetGimmickWork( GameSystem_GetSaveData(fsys) );
	gmc_tw = GIMMICKWORK_Get( gmc, FLD_GIMMICK_TORNWORLD );
	
	work = sys_AllocMemory( HEAPID_FIELD, sizeof(TORNWORLD_WORK) );
	memset( work, 0, sizeof(TORNWORLD_WORK) );
	work->fsys = fsys;
	work->gmc_tw = gmc_tw;
	fsys->fldmap->Work = work;
	
	//デバッグ変数初期化
	#ifdef DEBUG_TWORLD_CAPTURE
	DebugTWorldCaptureFlag_StepVanish = FALSE;
	DebugTWorldCaptureFlag_CloudStop = FALSE;
	DebugTWorldCaptureFlag_BGVanish = FALSE;
	#endif
	
	//アーカイブハンドル開く
	tw_ArcHandleOpen( work );
	
	//フラグ初期化
	if( gmc_tw->init_flag == FALSE ){
		tw_SaveDataEventFlagInit( work );
	}
	
	//マップデータ初期化
	tw_MapDataInit( work );
	
	//初期化
	tw_GraphicInit( work );
	tw_FldOBJParamInit( work );
	tw_OBJRotateProc_Init( work, TW_OBJROT_MAX );
	tw_OBJResDataInit( work );
	tw_BGInit( work );
	tw_FogInit( work );
	tw_ClActInit( work, &work->clact_set, work->arc_h_etc );
	tw_PlttInit( work );
	tw_CameraInit( work );
	tw_EventInit( work );
	tw_JikiInit( work );
	tw_FldOBJInit( work );
	tw_PutOBJInit( work );
	tw_FStoneInit( work );
	tw_LinkPosInit( work );
	tw_ScrInit( work );
	tw_ProcInit( work );
	
	//ダミーマップ初期化
	tw_DmyMapInit( work );
	tw_ZoneChangeTaskInit( work );
	
	//リンク位置読み込み
	tw_LinkPosLoad( work );
	
	//その他
	tw_EoaCloudSet( work, work->fsys->fes, &work->clact_set );
	tw_GirasActInit( work );
	FE_Kemuri_TornWorldON( work->fsys->fes );
	
	//VブランクTCB初期化
	tw_VIntrTCBInit( work );
	
	//セーブデータ　初期化完了を示す
	gmc_tw->init_flag = TRUE;
	
	//デバッグ出力
	DEBUG_twFldHeapPrintf();
}

//--------------------------------------------------------------
/**
 * やぶれたせかい　終了処理
 * @param	fsys	FIELDSYS_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
void FldTornWorld_End( FIELDSYS_WORK *fsys )
{
	TORNWORLD_WORK *work = fsys->fldmap->Work;
	
	//VブランクTCB削除
	tw_VIntrTCBDelete( work );
	
	//その他削除
	tw_GirasActDelete( work );
	
	//ダミーマップ削除
	tw_ZoneChangeTaskDelete( work );
	tw_DmyMapDelete( work );
	
	//終了処理
	tw_ProcDelete( work );
	tw_ScrDelete( work );
	tw_LinkPosDelete( work );
	tw_FStoneDelete( work );
	tw_PutOBJDelete( work );
	tw_FldOBJDeleteAll( work );
	tw_JikiDelete( work );
	tw_EventDelete( work );
	tw_CameraDelete( work );
	tw_PlttDelete( work );
	tw_ClActDelete( &work->clact_set );
	tw_FogDelete( work );
	tw_BGDelete( work );
	tw_OBJResDataDelete( work );
	tw_OBJRotateProc_Delete( work );
	tw_FldOBJParamDelete( work );
	tw_GraphicDelete( work );
	
	//マップデータ削除
	tw_MapDataDelete( work );
	
	//アーカイブハンドル閉じる
	tw_ArcHandleClose( work );
	
	sys_FreeMemoryEz( work );
	fsys->fldmap->Work = NULL;
}

//--------------------------------------------------------------
/**
 * やぶれたせかい　ゾーン切り替え
 * @param	work	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
#if 0
static void tw_ZoneChange( TORNWORLD_WORK *work )
{
	//飛び石削除
	tw_FStoneDelete( work );
	tw_SaveDataFStoneFlagSet( work, 0 );
	
	//接続石削除
	tw_LinkPosZoneIDDeleteAll( work, ZONE_ID_MAX );
	
	//マップデータリセット
	tw_MapDataReset( work );
	
	//飛び石初期化
	tw_FStoneInit( work );
	
	//新規接続石設置
	tw_LinkPosLoadZoneID( work, ZONE_ID_MAX );
	
	//OBJリソースクリーン
	tw_OBJResDataClean( work );
	
	//デバッグ出力
	DEBUG_twFldHeapPrintf();
}
#endif

//--------------------------------------------------------------
/**
 * やぶれたせかい　ゾーン切り替え　下層移動
 * @param	work	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ZoneChangeDown( TORNWORLD_WORK *work )
{
	u32 zone_id = twNowZoneID( work );	//既に移動先
	const TW_ZONE_LINKDATA *link = tw_ZoneLinkDataGet( zone_id );
	
	//飛び石削除
	tw_FStoneDeleteNow( work );
	tw_SaveDataFStoneFlagSet( work, 0 );
	
	//PUTOBJ削除 *上位=旧ゾーンID
	tw_PutOBJDeleteZoneID( work, link->upper_zone_id );
	
	//接続石削除 *上位=旧ゾーンID
	tw_LinkPosZoneIDDeleteAll( work, link->upper_zone_id );
	
	//フィールドOBJ削除 *上位=旧ゾーンID
	tw_FldOBJDeleteZoneID( work, link->upper_zone_id );
	
	//マップデータ 下層ダミーを現在層へ移行
	tw_MapDataRemoveLowerNow( work, link->lower_zone_id );
	
#if 1	//転送負荷が辛いがこちらに移す
	//OBJリソースクリーン
	tw_OBJResDataClean( work );
#endif
	
	//下層飛び石初期化
	tw_FStoneInitLower( work );
	
	//PUTOBJ設置
	tw_PutOBJLoadZoneID( work, link->lower_zone_id );
	
	//新規接続石設置
	tw_LinkPosLoadZoneID( work, link->lower_zone_id );
	
	//新規フィールドOBJ設置
	tw_FldOBJLoadZoneID( work, link->lower_zone_id );
	
#if 0	//本来こちらが良いがサイズが足りなくなる。
	//OBJリソースクリーン
	tw_OBJResDataClean( work );
#endif
	
	//デバッグ出力
	DEBUG_twFldHeapPrintf();
}

//--------------------------------------------------------------
/**
 * やぶれたせかい　ゾーン切り替え　上層移動
 * @param	work	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ZoneChangeUp( TORNWORLD_WORK *work )
{
	u32 zone_id = twNowZoneID( work );	//既に移動先
	const TW_ZONE_LINKDATA *link = tw_ZoneLinkDataGet( zone_id );
	link = tw_ZoneLinkDataGet( link->lower_zone_id );	//旧現在層
	
	//飛び石削除
	tw_FStoneDeleteLower( work );
	tw_SaveDataFStoneFlagSet( work, 0 );
	
	//PUTOBJ削除 *下層=旧ゾーンID
	tw_PutOBJDeleteZoneID( work, link->lower_zone_id );
	
	//接続石削除 *下層=旧ゾーンID
	tw_LinkPosZoneIDDeleteAll( work, link->lower_zone_id );
	
	//フィールドOBJ削除 *下層=旧ゾーンID
	tw_FldOBJDeleteZoneID( work, link->lower_zone_id );
	
	//マップデータ　旧現在層を下層ダミーへ移行
	tw_MapDataRemoveNowLower( work, zone_id );
	
#if 1	//転送負荷が辛いがこちらに移す
	//OBJリソースクリーン
	tw_OBJResDataClean( work );
#endif
	
	//現在層飛び石設置
	tw_FStoneInitNow( work, TRUE );
	
	//新規PUTOBJ設置
	tw_PutOBJLoadZoneID( work, zone_id );
	
	//新規接続石設置
	tw_LinkPosLoadZoneID( work, zone_id );
	
	//新規フィールドOBJ追加
	tw_FldOBJLoadZoneID( work, zone_id );
	
#if 0	//本来こちらが良いがサイズが足りなくなる。
	//OBJリソースクリーン
	tw_OBJResDataClean( work );
#endif
	
	//デバッグ出力
	DEBUG_twFldHeapPrintf();
}

//--------------------------------------------------------------
/**
 * やぶれたせかい　ゾーン切り替えで発生する削除処理　下層移動用
 * @param	work	TORNWORLD_WORK
 * @param	zone_id	削除するzone_id
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ZoneChangeDeleteProcDown( TORNWORLD_WORK *work, u32 zone_id )
{
	//飛び石削除
	tw_FStoneDeleteNow( work );
	tw_SaveDataFStoneFlagSet( work, 0 );
	
	//PUTOBJ削除 *上位=旧ゾーンID
	tw_PutOBJDeleteZoneID( work, zone_id );
	
	//接続石削除 *上位=旧ゾーンID
	tw_LinkPosZoneIDDeleteAll( work, zone_id );
	
	//フィールドOBJ削除 *上位=旧ゾーンID
	tw_FldOBJDeleteZoneID( work, zone_id );
	
	#if 1	//転送負荷が辛いがこちらに移す
	//OBJリソースクリーン
	tw_OBJResDataClean( work );
	#endif
}

//--------------------------------------------------------------
/**
 * やぶれたせかい　ゾーン切り替えで発生する追加処理　下層移動用
 * @param	work	TORNWORLD_WORK
 * @param	zone_id	追加するzone_id(新規下層
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ZoneChangeAddProcDown( TORNWORLD_WORK *work, u32 zone_id )
{
	//下層飛び石初期化
	tw_FStoneInitLower( work );
	
	//PUTOBJ設置
	tw_PutOBJLoadZoneID( work, zone_id );
	
	//新規接続石設置
	tw_LinkPosLoadZoneID( work, zone_id );
	
	//新規フィールドOBJ設置
	tw_FldOBJLoadZoneID( work, zone_id );
	
	#if 0	//本来こちらが良いがサイズが足りなくなる。
	//OBJリソースクリーン
	tw_OBJResDataClean( work );
	#endif
	
	//デバッグ出力
	DEBUG_twFldHeapPrintf();
}

//--------------------------------------------------------------
/**
 * やぶれたせかい　ゾーン切り替えで発生する削除処理　上層移動用
 * @param	work	TORNWORLD_WORK
 * @param	zone_id	削除するzone_id(下層
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ZoneChangeDeleteProcUp( TORNWORLD_WORK *work, u32 zone_id )
{
	//飛び石削除
	tw_FStoneDeleteLower( work );
	tw_SaveDataFStoneFlagSet( work, 0 );
	
	//PUTOBJ削除
	tw_PutOBJDeleteZoneID( work, zone_id );
	
	//接続石削除 *下層=旧ゾーンID
	tw_LinkPosZoneIDDeleteAll( work, zone_id );
	
	//フィールドOBJ削除 *下層=旧ゾーンID
	tw_FldOBJDeleteZoneID( work, zone_id );
	
#if 1	//転送負荷が辛いがこちらに移す
	//OBJリソースクリーン
	tw_OBJResDataClean( work );
#endif
}

//--------------------------------------------------------------
/**
 * やぶれたせかい　ゾーン切り替えで発生する追加処理　上層移動用
 * @param	work	TORNWORLD_WORK
 * @param	zone_id	追加するzone_id(新規上層
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ZoneChangeAddProcUp( TORNWORLD_WORK *work, u32 zone_id )
{
	//現在層飛び石設置
	tw_FStoneInitNow( work, TRUE );
	
	//新規PUTOBJ設置
	tw_PutOBJLoadZoneID( work, zone_id );
	
	//新規接続石設置
	tw_LinkPosLoadZoneID( work, zone_id );
	
	//新規フィールドOBJ追加
	tw_FldOBJLoadZoneID( work, zone_id );
	
#if 0	//本来こちらが良いがサイズが足りなくなる。
	//OBJリソースクリーン
	tw_OBJResDataClean( work );
#endif
	
	//デバッグ出力
	DEBUG_twFldHeapPrintf();
}

//--------------------------------------------------------------
/**
 * やぶれたせかい　アーカイブハンドル開く
 * @param	work	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ArcHandleOpen( TORNWORLD_WORK *work )
{
	work->arc_h_map =
		ArchiveDataHandleOpen( ARC_TWORLD, HEAPID_FIELD );
	work->arc_h_attr =
		ArchiveDataHandleOpen( ARC_TWORLD_ATTR, HEAPID_FIELD );
	work->arc_h_etc =
		ArchiveDataHandleOpen( ARC_TWORLD_ETC, HEAPID_FIELD );
}

//--------------------------------------------------------------
/**
 * やぶれたせかい　アーカイブハンドル閉じる
 * @param	work	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ArcHandleClose( TORNWORLD_WORK *work )
{
	ArchiveDataHandleClose( work->arc_h_map );
	ArchiveDataHandleClose( work->arc_h_attr );
	ArchiveDataHandleClose( work->arc_h_etc );
}

//==============================================================================
//	やぶれたせかい　セーブデータ
//==============================================================================
//--------------------------------------------------------------
/**
 * イベントフラグ初期化　破れた世界初回初期化のみ処理
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_SaveDataEventFlagInit( TORNWORLD_WORK *tw )
{
	FLD_TORNWORLD_WORK *gmc_tw = tw->gmc_tw;
	EVENTWORK *ev = SaveData_GetEventWork( tw->fsys->savedata );
	gmc_tw->event_flag = 0;
	
	{	//ゾーン接続移動フラグ初期化
		u32 flag = ZLINKMFLAG_INITFLAG;
		if( twNowZoneID(tw) == ZONE_ID_D34R0109 ){
			flag = ZLINKMFLAG_INITFLAG_08;
		}
		tw_SaveDataZLinkMoveFlagSet( tw, flag );
	}
	
	#ifdef DEBUG_TW_FALLROCK_FINISH	//岩落とし攻略済みに
	SysFlag_TornWorldRockFinish( ev, SYSFLAG_MODE_SET );
	 #ifndef DEBUG_TW_FALLROCK_FINISH_EVENT //岩落とし攻略完了イベントを見る
	 SysFlag_TornWorldRockLiftFlagSet( ev );
	 #endif
	#endif
	
	//岩落としフラグ初期化
	if( SysFlag_TornWorldRockFinish(ev,SYSFLAG_MODE_CHECK) == FALSE ){
		gmc_tw->event_flag |= TWEVFLAG_FALLROCK_6_ALL;
	}else{
		gmc_tw->event_flag |= TWEVFLAG_FALLROCK_FINISH;
	}
	
	#ifndef DEBUG_TW_FALLROCK_FINISH
	#ifdef DEBUG_TW_FALLROCK_7		//岩全て７層に落とし済み
	gmc_tw->event_flag &= ~TWEVFLAG_FALLROCK_6_ALL;
	
	gmc_tw->event_flag |=
		(1<<TWEVFLAGNO_FALLROCK0_7) |
		(1<<TWEVFLAGNO_FALLROCK1_7) |
		(1<<TWEVFLAGNO_FALLROCK2_7) |
		(1<<TWEVFLAGNO_0107YUKUSHIEV) |
	 	(1<<TWEVFLAGNO_0107AGUNOMUEV) |
	 	(1<<TWEVFLAGNO_0107EMURITEV) | 
		(1<<TWEVFLAGNO_0108YUKUSHI)	|
		(1<<TWEVFLAGNO_0108AGUNOMU) |
		(1<<TWEVFLAGNO_0108EMURIT);
	#endif
	#endif
	
	#ifdef DEBUG_TW_GIRAS_FLAG_OFF		//ギラティナ影出現済み
	{
		EVENTWORK *ev = SaveData_GetEventWork( tw->fsys->savedata );
		SysFlag_TornWorldGirasFlag(ev,SYSFLAG_MODE_SET,GIRASFLAG_0500);
		SysFlag_TornWorldGirasFlag(ev,SYSFLAG_MODE_SET,GIRASFLAG_0501);
	}
	#endif
	
	#ifdef DEBUG_TW_FLAG_INIT			//デバッグ用初期化
	{
		//最初から
//		SysWork_TornWorldEventSeqNoSet( ev, TWEVSEQ_01_00 );
		//シロナ三層に存在
//		SysWork_TornWorldEventSeqNoSet( ev, TWEVSEQ_03_00 );
		//７層岩落とし開始
		SysWork_TornWorldEventSeqNoSet( ev, TWEVSEQ_04_00 );
		//４層アカギイベント見れる
//		SysWork_TornWorldEventSeqNoSet( ev, TWEVSEQ_03_01 );
		//岩落とし攻略後のシロナストッパー解除目前
//		SysWork_TornWorldEventSeqNoSet( ev, TWEVSEQ_07_00 );
		//最下層イベント目前
//		SysWork_TornWorldEventSeqNoSet( ev, TWEVSEQ_08_02 );
	}
	#endif
	
	#ifdef	DEBUG_TW_SCENE_NOHARA_CHECK		//for nohara
	{
		//最下層イベント目前
		SysWork_TornWorldEventSeqNoSet( ev, TWEVSEQ_08_02 );
	}
	#endif
}

//--------------------------------------------------------------
/**
 * カメラオフセット角度のセーブ
 * @param	tw	TORNWORLD_WORK
 * @param	x	カメラ角度
 * @param	y	カメラ角度
 * @param	z	カメラ角度
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_SaveDataCameraAngleSet(
		TORNWORLD_WORK *tw, u16 x, u16 y, u16 z )
{
	FLD_TORNWORLD_WORK *gmc_tw = tw->gmc_tw;
	gmc_tw->cm_ox = x;
	gmc_tw->cm_oy = y;
	gmc_tw->cm_oz = z;
}

//--------------------------------------------------------------
/**
 * カメラオフセット角度の取得
 * @param	tw	TORNWORLD_WORK
 * @param	x	カメラ角度
 * @param	y	カメラ角度
 * @param	z	カメラ角度
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_SaveDataCameraAngleGet(
		TORNWORLD_WORK *tw, u16 *x, u16 *y, u16 *z )
{
	FLD_TORNWORLD_WORK *gmc_tw = tw->gmc_tw;
	*x = gmc_tw->cm_ox;
	*y = gmc_tw->cm_oy;
	*z = gmc_tw->cm_oz;
}

//--------------------------------------------------------------
/**
 * 初期化済みかどうか
 * @param	tw	TORNWORLD_WORK
 * @retval	BOOL	TRUE=済み
 */
//--------------------------------------------------------------
static BOOL tw_SaveDataInitFinishCheck( TORNWORLD_WORK *tw )
{
	FLD_TORNWORLD_WORK *gmc_tw = tw->gmc_tw;
	return( gmc_tw->init_flag );
}

//--------------------------------------------------------------
/**
 * 飛び石フラグセット
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_SaveDataFStoneFlagSet( TORNWORLD_WORK *tw, u32 flag )
{
	FLD_TORNWORLD_WORK *gmc_tw = tw->gmc_tw;
	gmc_tw->fstone_flag = flag;
	
	#ifdef DEBUG_TW_PRINTF
//	OS_Printf( "TW FSTONE FLAG SET %xH\n", flag );
	#endif
}

//--------------------------------------------------------------
/**
 * 飛び石フラグ取得
 * @param	tw	TORNWORLD_WORK
 * @retval	u16	飛び石フラグ
 */
//--------------------------------------------------------------
static u32 tw_SaveDataFStoneFlagGet( TORNWORLD_WORK *tw )
{
	FLD_TORNWORLD_WORK *gmc_tw = tw->gmc_tw;
	#ifdef DEBUG_TW_PRINTF
//	OS_Printf( "TW FSTONE FLAG GET %xH\n", gmc_tw->fstone_flag );
	#endif
	return( gmc_tw->fstone_flag );
}

//--------------------------------------------------------------
/**
 * ゾーン移動フラグ立て　ビットセット
 * @param	tw	TORNWORLD_WORK
 * @param	flag	ZLINKMFLAG_0102等(No->フラグ化されている事
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_SaveDataZLinkMoveFlagSet( TORNWORLD_WORK *tw, u32 flag )
{
	FLD_TORNWORLD_WORK *gmc_tw = tw->gmc_tw;
	gmc_tw->zlink_mflag = flag;
}

//--------------------------------------------------------------
/**
 * ゾーン移動フラグ立て
 * @param	tw	TORNWORLD_WORK
 * @param	flag	ZLINKMFLAG_0102等
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_SaveDataZLinkMoveFlagON( TORNWORLD_WORK *tw, u32 flag )
{
	FLD_TORNWORLD_WORK *gmc_tw = tw->gmc_tw;
	GF_ASSERT( flag < ZLINKMFLAG_MAX );
	gmc_tw->zlink_mflag |= (1 << flag);
}

//--------------------------------------------------------------
/**
 * ゾーン移動フラグ落とし
 * @param	tw	TORNWORLD_WORK
 * @param	flag	ZLINKMFLAG_0102等
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_SaveDataZLinkMoveFlagOFF( TORNWORLD_WORK *tw, u32 flag )
{
	FLD_TORNWORLD_WORK *gmc_tw = tw->gmc_tw;
	GF_ASSERT( flag < ZLINKMFLAG_MAX );
	gmc_tw->zlink_mflag &= ~(1 << flag);
}

//--------------------------------------------------------------
/**
 * ゾーン移動フラグチェック
 * @param	tw	TORNWORLD_WORK
 * @param	flag	ZLINKMFLAG_0102等
 * @retval	BOOL	TRUE=flag立っている
 */
//--------------------------------------------------------------
static BOOL tw_SaveDataZLinkMoveFlagCheck( TORNWORLD_WORK *tw, u32 flag )
{
	FLD_TORNWORLD_WORK *gmc_tw = tw->gmc_tw;
	GF_ASSERT( flag < ZLINKMFLAG_MAX );
	if( (gmc_tw->zlink_mflag & (1<<flag)) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * 破れた世界イベントフラグ ON
 * @param	tw		TORNWORLD_WORK
 * @param	flag_no	TWEVFLAGNO_MAX
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_SaveDataEventFlagON( TORNWORLD_WORK *tw, u32 flag_no )
{
	FLD_TORNWORLD_WORK *gmc_tw = tw->gmc_tw;
	GF_ASSERT( flag_no < TWEVFLAGNO_MAX );
	gmc_tw->event_flag |= (1<<flag_no);
}

//--------------------------------------------------------------
/**
 * 破れた世界イベントフラグ OFF
 * @param	tw		TORNWORLD_WORK
 * @param	flag_no	TWEVFLAGNO_MAX
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_SaveDataEventFlagOFF( TORNWORLD_WORK *tw, u32 flag_no )
{
	FLD_TORNWORLD_WORK *gmc_tw = tw->gmc_tw;
	GF_ASSERT( flag_no < TWEVFLAGNO_MAX );
	gmc_tw->event_flag &= ~(1<<flag_no);
}

//--------------------------------------------------------------
/**
 * 破れた世界イベントフラグ Check
 * @param	tw		TORNWORLD_WORK
 * @param	flag_no	TWEVFLAGNO_MAX
 * @retval	BOOL	TRUE=hit
 */
//--------------------------------------------------------------
static BOOL tw_SaveDataEventFlagCheck( TORNWORLD_WORK *tw, u32 flag_no )
{
	FLD_TORNWORLD_WORK *gmc_tw = tw->gmc_tw;
	GF_ASSERT( flag_no < TWEVFLAGNO_MAX );
	if( (gmc_tw->event_flag & (1<<flag_no)) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * 破れた世界特殊地形番号セット
 * @param	tw	TORNWORLD_WORK
 * @param	no	特殊番号
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_SaveDataSpGroundNoSet( TORNWORLD_WORK *tw, u32 no )
{
	FLD_TORNWORLD_WORK *gmc_tw = tw->gmc_tw;
	GF_ASSERT( no < TW_SP_GROUND_MAX );
	gmc_tw->sp_ground_no = no;
}

//--------------------------------------------------------------
/**
 * 破れた世界特殊地形番号取得
 * @param	tw	TORNWORLD_WORK
 * @retval	u32 特殊地形番号
 */
//--------------------------------------------------------------
static u32 tw_SaveDataSpGroundNoGet( TORNWORLD_WORK *tw )
{
	FLD_TORNWORLD_WORK *gmc_tw = tw->gmc_tw;
	return( gmc_tw->sp_ground_no );
}

#if 0
//--------------------------------------------------------------
/**
 * ゾーン移動　特定階層到着時に強制操作するリフトフラグまとめ
 * @param	tw	TORNWORLD_WORK
 * @param	id	リフト移動動作ID ZCHG_MID_Z0101_1_2等
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_SaveDataZLinkMoveFlagZoneForceSet(
		TORNWORLD_WORK *tw, u16 id )
{
	switch( id ){
	case ZCHG_MID_Z0105_5A_4A:
	case ZCHG_MID_Z0105_5B_4B:
		tw_SaveDataZLinkMoveFlagOFF( tw, ZLINKMFLAG_0406A );
		break;
	}
}
#endif

//==============================================================================
//	やぶれたせかい　当たり判定
//==============================================================================
//---------------------------------------------------------------------------
/**
 * やぶれたせかい 当たり判定
 * @param	fsys		フィールドシステムポインタ
 * @param	inGridX		グリッドX座標
 * @param	inGridZ		グリッドZ座標
 * @param	inHeight	高さ
 * @param	outHit		判定結果	TRUE:ヒット	FALSE:ヒットしてない
 * @return	BOOL		TRUE:このあと通常当たり判定を行わない	FALSE:通常当たり判定を行う
 */
//---------------------------------------------------------------------------
BOOL FldTornWorld_HitCheck( FIELDSYS_WORK *fsys,
	const int gx, const int gz,
	const fx32 inHeight, BOOL *outHit )
{
	TORNWORLD_WORK *tw = fsys->fldmap->Work;
	u32 zone_id = twNowZoneID( tw );
	
	if( zone_id == ZONE_ID_D34R0110 ){			//ゾーン遷移位置
		if( gx == 15 && gz == 26 ){
			*outHit = TRUE;
			return( TRUE );
		}
	}else if( zone_id == ZONE_ID_D34R0109 ){	//ゾーン遷移位置
		if( gx == 89 && gz == 56 ){
			*outHit = TRUE;
			return( TRUE );
		}
	}
	
	*outHit = FALSE;
	return( FALSE );
}

//==============================================================================
//	やぶれたせかい　常駐処理
//==============================================================================
//--------------------------------------------------------------
/**
 * やぶれたせかい　常駐処理　初期化
 * @param	work	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ProcInit( TORNWORLD_WORK *work )
{
	int pri;
	TW_PROC_WORK *pwk = &work->proc_work;
	memset( pwk, 0, sizeof(TW_PROC_WORK) );
	pri = FieldOBJSys_TCBStandardPriorityGet( work->fsys->fldobjsys );
	pri += FLDOBJ_TCBPRI_OFFS_AFTER; //field objより後
	pwk->tcb_proc = TCB_Add( tw_ProcTcb, work, pri );
}

//--------------------------------------------------------------
/**
 * やぶれたせかい　常駐処理　削除
 * @param	work	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ProcDelete( TORNWORLD_WORK *work )
{
	TW_PROC_WORK *pwk = &work->proc_work;
	if( pwk->tcb_proc != NULL ){
		TCB_Delete( pwk->tcb_proc );
	}
}

//--------------------------------------------------------------
/**
 * やぶれたせかい　常駐処理　TCB
 * @param	tcb	TCB_PTR
 * @param	wk	tcb work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ProcTcb( TCB_PTR tcb, void *wk )
{
	TORNWORLD_WORK *tw = wk;
	TW_PROC_WORK *pwk = &tw->proc_work;
	
	{	//OBJ回転処理
		tw_OBJRotateProcTcbCall( tw );
	}
	
	{	//パレット
		#ifndef TORNWORLD_PLTT_FADE_OFF
		tw_PlttProc( tw );
		#endif
	}
	
	#ifdef TW_DEBUG
	{
		TW_DEBUG_WORK *debug = &tw->debug;
		
		#ifdef DEBUG_TAKI_EFF
		if( (sys.trg & PAD_BUTTON_SELECT) ){
			#ifdef TWTEST_TAKI_EFF
			if( debug->eoa_splash == NULL ){
				Player_VecPosGet( tw->fsys->player, &debug->splash_pos );
				debug->eoa_splash = EoaSplash_Set( tw, &debug->splash_pos );
			}else{
				EOA_Delete( debug->eoa_splash );
				debug->eoa_splash = NULL;
			}
			#endif
		}
		
		#ifdef TWTEST_TAKI_EFF
		if( debug->eoa_splash != NULL ){
			Player_VecPosGet( tw->fsys->player, &debug->splash_pos );
		}
		#endif
		#endif
		
		#ifdef DEBUG_TW_PRINTF_FORCE
		if( (sys.trg & PAD_BUTTON_SELECT) ){
			DEBUG_twZoneIDPrintf( twNowZoneID(tw) );
			DEBUG_twFldHeapPrintf();
			
			#ifdef DEBUG_TW_OBJ_COUNT
			DEBUG_tw_OBJResCountPrint( tw );
			#endif
			
			#ifdef DEBUG_TW_PRINTF
			{
				u32 attr;
				VecFx32 pos;
				PLAYER_STATE_PTR jiki = tw->fsys->player;
				FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( jiki );
				int x = FieldOBJ_NowPosGX_Get( fldobj );
				int y = H_GRID_G_GRID( FieldOBJ_NowPosGY_Get(fldobj) );
				int z = FieldOBJ_NowPosGZ_Get( fldobj );
				int dir = FieldOBJ_DirMoveGet( fldobj );
				
				OS_Printf("自機現在座標 GX=%d,GY=%d,GZ=%d, 方向 ",x,y,z);
				
				switch( dir ){
				case DIR_UP: OS_Printf( "UP\n" ); break;
				case DIR_DOWN: OS_Printf( "DOWN\n" ); break;
				case DIR_LEFT: OS_Printf( "LEFT\n" ); break;
				case DIR_RIGHT: OS_Printf( "RIGHT\n" ); break;
				}
				
				FieldOBJ_VecPosGet( fldobj, &pos );
				OS_Printf( "自機実座標 X=%xH(%d),Y=%xH(%d),Z=%xH(%d)\n",
					pos.x, SIZE_GRID_FX32(pos.x),
					pos.y, SIZE_GRID_FX32(pos.y),
					pos.z, SIZE_GRID_FX32(pos.z) );
				
				Player_TwThinkDirPosGet( jiki, dir, &x, &y, &z );
				OS_Printf( "前方座標 GX=%d,GY=%d,GZ=%d, ", x, y, z );
				
				attr = Player_TwThinkAttrGet( jiki, x, y, z );
				OS_Printf( "Attr 0x%02xH\n", attr );
				
			}
			#endif
		}
		#endif
	}
	#endif
}

#if 0	//旧
static void tw_ProcTcb( TCB_PTR tcb, void *wk )
{
	TORNWORLD_WORK *work = wk;
	
	tw_OBJRotateProcTcbCall( work );
	
	{
		TW_DEBUG_WORK *dg = &work->debug;
		FE_SYS * fes = work->fsys->fes;
		FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( work->fsys->player );
		BLACT_WORK_PTR act = FieldOBJ_DrawBlAct00_BlActPtrGet( fldobj );
		
		if( act != NULL ){
			void *buf;
			NNSG3dResMdl *pResMdl;
			NNSG3dResTex *pResTex;
			NNSG3dRenderObj *rndobj;
			
			switch( dg->seq_no ){
			case 0:
				sys_InitAllocator( &dg->Allocator, HEAPID_FIELD, 4 );
				
				pResMdl = BLACT_ResMdlGet( act );
				pResTex = BLACT_ResTexGet( act );
				
				dg->pResAnm = FE_ArcDataLoadAlloc(
					fes, NARC_fldeff_hero_rote_nsbta, ALLOC_LO );
				dg->pResAnmIdx = NNS_G3dGetAnmByIdx( dg->pResAnm, 0 );
				dg->pAnmObj = NNS_G3dAllocAnmObj(
						&dg->Allocator, dg->pResAnmIdx, pResMdl );
				
				NNS_G3dAnmObjInit(
						dg->pAnmObj, dg->pResAnmIdx, pResMdl, pResTex );
				
				rndobj = BLACT_RenderObjGet( act );
				
				NNS_G3dRenderObjAddAnmObj( rndobj, dg->pAnmObj );
				dg->seq_no++;
				break;
			case 1:
				{
					fx32 end = NNS_G3dAnmObjGetNumFrame( dg->pAnmObj );
					
					dg->frame += FX32_ONE;
					
					if( dg->frame >= end ){
						dg->frame -= end;
					}
					
					NNS_G3dAnmObjSetFrame( dg->pAnmObj, dg->frame );
				}
				
				break;
			}
		}
	}
}
#endif

//==============================================================================
//	破れた世界　VブランクTCB
//==============================================================================
//--------------------------------------------------------------
/**
 * 破れた世界　VブランクTCB初期化
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_VIntrTCBInit( TORNWORLD_WORK *tw )
{
	GF_ASSERT( tw->tcb_vintr_p == NULL );
	tw->tcb_vintr_p = VIntrTCB_Add( tw_VIntrTCB, tw, 0x80 );
}

//--------------------------------------------------------------
/**
 * 破れた世界　VブランクTCB削除
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_VIntrTCBDelete( TORNWORLD_WORK *tw )
{
	GF_ASSERT( tw->tcb_vintr_p != NULL );
	TCB_Delete( tw->tcb_vintr_p );
	tw->tcb_vintr_p = NULL;
}

//--------------------------------------------------------------
/**
 * 破れた世界　VブランクTCB
 * @param	tcb	TCB_PTR
 * @param	wk	tcb work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_VIntrTCB( TCB_PTR tcb, void *wk )
{
	TORNWORLD_WORK *tw = wk;
	tw_ZoneChangeTaskVIntr( tw );
	tw_PlttVIntrProc( tw );
}

//==============================================================================
//	破れた世界　グラフィック
//==============================================================================
//--------------------------------------------------------------
/**
 * やぶれたせかい　グラフィック初期化
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_GraphicInit( TORNWORLD_WORK *tw )
{
	{	//エッジカラー初期化
		int i;
		for( i = 0; i < TW_EDGE_COLOR_MAX; i++ ){
			tw->edge_color_tbl[i] =
				GX_RGB( EDGE_COLOR_R, EDGE_COLOR_G, EDGE_COLOR_B );
		}
		
		tw->edge_color_tbl[GIRAS_EDGE_NO] = GX_RGB(
			GIRAS_EDGE_COLOR_R, GIRAS_EDGE_COLOR_G, GIRAS_EDGE_COLOR_B );
		G3X_SetEdgeColorTable( tw->edge_color_tbl );
	}
}

//--------------------------------------------------------------
/**
 * やぶれたせかい　グラフィック削除
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_GraphicDelete( TORNWORLD_WORK *tw )
{
}

//--------------------------------------------------------------
/**
 * 破れたせかい　指定されたエッジ番号のカラー設定
 * @param	tw	TORNWORLD_WORK
 * @param	no 0-7
 * @param	col	GX_RGB
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_GrapEdgeColorSet( TORNWORLD_WORK *tw, u32 no, u32 col )
{
	tw->edge_color_tbl[no] = col;
	G3X_SetEdgeColorTable( tw->edge_color_tbl );
}

//--------------------------------------------------------------
/**
 * 破れたせかい　指定ポリゴンIDに対応したエッジカラ－取得
 * @param	tw	TORNWORLD_WORK
 * @param	no 0-7
 * @retval	GXRgb GX_RGB
 */
//--------------------------------------------------------------
static GXRgb tw_GrapEdgeColorGet( TORNWORLD_WORK *tw, u32 no )
{
	return( tw->edge_color_tbl[no] );
}

//==============================================================================
//	やぶれたせかい　フィールドOBJ
//==============================================================================
//--------------------------------------------------------------
/**
 * 破れた世界　フィールドOBJ　初期化
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_FldOBJParamInit( TORNWORLD_WORK *tw )
{
	FieldOBJSys_ShadowJoinSet( tw->fsys->fldobjsys, FALSE );
}

//--------------------------------------------------------------
/**
 * 破れた世界　フィールドOBJ　初期化
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_FldOBJParamDelete( TORNWORLD_WORK *tw )
{
}

//==============================================================================
//	やぶれたせかい　カメラ
//==============================================================================
//--------------------------------------------------------------
/*
 * やぶれたせかい　カメラ反映
 * @param	fsys	FIELDSYS_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
void FldTornWorld_CameraUpdate( FIELDSYS_WORK *fsys )
{
	CAMERA_ANGLE angle;
	TORNWORLD_WORK *tw = fsys->fldmap->Work;
	TW_CAMERA_WORK *work = &tw->camera;
	
	angle.x = work->angle.x + work->angle_offs.x;
	angle.y = work->angle.y + work->angle_offs.y;
	angle.z = work->angle.z + work->angle_offs.z;
	
	#ifdef DEBUG_TWORLD_CAPTURE
	angle.x += work->debug_cap_angle.x;
	angle.y += work->debug_cap_angle.y;
	angle.z += work->debug_cap_angle.z;
	#endif
	
	GFC_SetCameraAngleRev( &angle, work->gfc_camera );
}

//--------------------------------------------------------------
/**
 * やぶれたせかい　カメラ角度を基本に戻す
 * @param	fsys	FIELDSYS_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
void FldTornWorld_CameraDefaultSet( FIELDSYS_WORK *fsys )
{
	GIMMICKWORK *gmc;
	FLD_TORNWORLD_WORK *gmc_tw;
	
	GF_ASSERT( fsys != NULL );
	
	gmc = SaveData_GetGimmickWork( GameSystem_GetSaveData(fsys) );
	gmc_tw = GIMMICKWORK_Get( gmc, FLD_GIMMICK_TORNWORLD );
	gmc_tw->cm_ox = 0;
	gmc_tw->cm_oy = 0;
	gmc_tw->cm_oz = 0;
}

//--------------------------------------------------------------
/**
 * やぶれたせかい　カメラ初期化
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_CameraInit( TORNWORLD_WORK *tw )
{
	TW_CAMERA_WORK *work = &tw->camera;
	
	tw->fsys->camera_type = FIELD_CAMERA_LOOKAT_Z;
	
	#ifdef TWCM_CAMERADATA_ALLOC
	work->gfc_camera = GFC_AllocCamera( HEAPID_FIELD );
	GFC_CopyCamera( tw->fsys->camera_ptr, work->gfc_camera );
	GFC_AttachCamera( work->gfc_camera );
	#else
	work->gfc_camera = tw->fsys->camera_ptr;
	#endif
	
	{
		GF_CAMERA_PARAM cmp =
		{
			0x29aec1,
			{-0x29fe,0,0},
			GF_CAMERA_PERSPECTIV,
			0x05c1,
			0
		};
		
		GFC_SetCameraDistance( cmp.Distance, work->gfc_camera );
		GFC_SetCameraAngleRev( &cmp.Angle, work->gfc_camera );
		GFC_SetCameraPerspWay( cmp.PerspWay, work->gfc_camera );
		GFC_SetCameraView( cmp.View, work->gfc_camera );
	//	GFC_SetCameraView( 0, work->gfc_camera );	//透視射影
	}
	
	GFC_SetCameraClip( TWCM_CLIP_NEAR, TWCM_CLIP_FAR, work->gfc_camera );
	
	work->angle.x = TWCM_DEF_RX;
	work->angle.y = TWCM_DEF_RY;
	work->angle.z = TWCM_DEF_RZ;
	
	if( tw_SaveDataInitFinishCheck(tw) == TRUE ){
		u16 x,y,z;
		tw_SaveDataCameraAngleGet( tw, &x, &y, &z );
		work->angle_offs_true.x = CMAD_V16( x );
		work->angle_offs_true.y = CMAD_V16( y );
		work->angle_offs_true.z = CMAD_V16( z );
		work->angle_offs = work->angle_offs_true;
	}
	
	if( twNowZoneID(tw) != ZONE_ID_D34R0110 ){
		GFC_AddCameraPerspWay( 0x0681-0x05c1, work->gfc_camera );
	}
	
	work->tcb_action = TCB_Add( tw_CameraActionTCB, tw, 0 );
}

//--------------------------------------------------------------
/**
 * やぶれたせかい　カメラ削除
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_CameraDelete( TORNWORLD_WORK *tw )
{
	TW_CAMERA_WORK *work = &tw->camera;
	
	tw->fsys->camera_type = FIELD_CAMERA_NORMAL;
	
	if( work->tcb_action != NULL ){
		TCB_Delete( work->tcb_action );
	}
	
	#ifdef TWCM_CAMERADATA_ALLOC
	GFC_FreeCamera( work->gfc_camera );
	#endif
}

//--------------------------------------------------------------
/**
 * 座標起動カメラアクション　カメラ操作 TCB
 * @param	tcb	TCB_PTR
 * @param	wk	tcb work TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_CameraActionTCB( TCB_PTR tcb, void *wk )
{
	TORNWORLD_WORK *tw = wk;
	TW_CAMERA_WORK *cm = &tw->camera;
	TW_CMACT_WORK *work = &cm->action_work;
	
	if( work->act_flag == FALSE ){
		return;
	}
	
	work->frame--;
	
	if( work->frame <= 0 ){
		work->act_flag = FALSE;
		cm->angle_offs = cm->angle_offs_true;
		return;
	}
	
	work->offs_fx.x += work->add_val_fx.x;
	work->offs_fx.y += work->add_val_fx.y;
	work->offs_fx.z += work->add_val_fx.z;
	cm->angle_offs.x = FX32_NUM( work->offs_fx.x );
	cm->angle_offs.y = FX32_NUM( work->offs_fx.y );
	cm->angle_offs.z = FX32_NUM( work->offs_fx.z );
}

//--------------------------------------------------------------
/**
 * 座標起動カメラアクション　セット
 * @param	tw	TORNWORLD_WORK
 * @param	data	TW_CMACTDATA
 * @retval	BOOL	TRUE=セット FALSE=セットする必要なし
 */
//--------------------------------------------------------------
static BOOL tw_CameraActionSet(
	TORNWORLD_WORK *tw, const TW_CMACTDATA *data )
{
	TW_CAMERA_WORK *cm = &tw->camera;
	TW_CMACT_WORK *work = &cm->action_work;
	
	tw_SaveDataCameraAngleSet( tw, data->cm_x, data->cm_y, data->cm_z );
	cm->angle_offs_true.x = CMAD_V16( data->cm_x );	//真とする
	cm->angle_offs_true.y = CMAD_V16( data->cm_y );
	cm->angle_offs_true.z = CMAD_V16( data->cm_z );
	
	if( cm->angle_offs.x == cm->angle_offs_true.x &&	//既に一致
		cm->angle_offs.y == cm->angle_offs_true.y &&
		cm->angle_offs.z == cm->angle_offs_true.z ){
		work->act_flag = FALSE;
		return( FALSE );
	}
	
	work->act_flag = TRUE;
	work->frame = data->frame;
	
	work->offs_fx.x = NUM_FX32( cm->angle_offs.x );	//目標を保存
	work->offs_fx.y = NUM_FX32( cm->angle_offs.y );
	work->offs_fx.z = NUM_FX32( cm->angle_offs.z );
	
	{
		int diff;
		diff = twAngle16ShortDiffGet(
			cm->angle_offs.x, cm->angle_offs_true.x );
		work->add_val_fx.x = NUM_FX32( diff ) / data->frame;
		diff = twAngle16ShortDiffGet(
			cm->angle_offs.y, cm->angle_offs_true.y );
		work->add_val_fx.y = NUM_FX32( diff ) / data->frame;
		diff = twAngle16ShortDiffGet(
			cm->angle_offs.z, cm->angle_offs_true.z );
		work->add_val_fx.z = NUM_FX32( diff ) / data->frame;
	}
	
	return( TRUE );
}

//==============================================================================
//	やぶれたせかい　OBJテクスチャ回転
//==============================================================================
//--------------------------------------------------------------
/**
 * OBJ回転　初期化
 * @param	tw	TORNWORLD_WORK
 * @param	max	回転させるOBJ最大数
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJRotateProc_Init( TORNWORLD_WORK *tw, int max )
{
	TW_OBJ_ROT_PROC_WORK *work = &tw->objrot;
	
	work->max = max;
	max *= sizeof( TW_OBJ_ROT_ACT_WORK );
	work->act_tbl = sys_AllocMemory( HEAPID_FIELD, max );
	memset( work->act_tbl, 0, max );
	
	sys_InitAllocator( &work->Allocator, HEAPID_FIELD, 4 );
	work->pResAnm = FE_ArcDataLoadAlloc(
			tw->fsys->fes, NARC_fldeff_hero_rote_nsbta, ALLOC_LO );
	work->pResAnmIdx = NNS_G3dGetAnmByIdx( work->pResAnm, 0 );
}

//--------------------------------------------------------------
/**
 * OBJ回転　回転アクターリソースセット
 * @param	act	TW_OBJ_ROT_ACT_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJRotateAct_BlActResSet( TW_OBJ_ROT_PROC_WORK *work,
		TW_OBJ_ROT_ACT_WORK *act, BLACT_WORK_PTR blact )
{
	NNSG3dResMdl *pResMdl = BLACT_ResMdlGet( blact );
	NNSG3dResTex *pResTex = BLACT_ResTexGet( blact );
	NNSG3dRenderObj *pRndObj = BLACT_RenderObjGet( blact );
	GF_ASSERT( act->pAnmObj == NULL ); //初期化済み
	
	act->blact = blact;
	act->pAnmObj = NNS_G3dAllocAnmObj(
		&work->Allocator, work->pResAnmIdx, pResMdl );
	NNS_G3dAnmObjInit( act->pAnmObj, work->pResAnmIdx, pResMdl, pResTex );
	NNS_G3dRenderObjAddAnmObj( pRndObj, act->pAnmObj );
	BLACT_DrawBeforeProcSet( blact, BlActOBJRotateDraw, act );
	
	if( act->fldobj != NULL ){
		act->obj_code = FieldOBJ_OBJCodeGet( act->fldobj );
	}
}

//--------------------------------------------------------------
/**
 * OBJ回転　回転アクターリソース開放　開放のみ
 * @param	act	TW_OBJ_ROT_ACT_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJRotateAct_BlActResFreeOnly(
		TW_OBJ_ROT_PROC_WORK *work,	TW_OBJ_ROT_ACT_WORK *act )
{
	if( act->pAnmObj != NULL ){
		NNS_G3dFreeAnmObj( &work->Allocator, act->pAnmObj );
		act->pAnmObj = NULL;
	}
}

//--------------------------------------------------------------
/**
 * OBJ回転　回転アクターリソース開放
 * @param	act	TW_OBJ_ROT_ACT_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJRotateAct_BlActResFree(
		TW_OBJ_ROT_PROC_WORK *work,	TW_OBJ_ROT_ACT_WORK *act )
{
	tw_OBJRotateAct_BlActResFreeOnly( work, act );
	memset( act, 0, sizeof(TW_OBJ_ROT_ACT_WORK) );
}

//--------------------------------------------------------------
/**
 * OBJ回転　ビルボードが登録されたらリソースセット
 * @param	act	TW_OBJ_ROT_ACT_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJRotateAct_ResUpdate(
		TW_OBJ_ROT_PROC_WORK *work, TW_OBJ_ROT_ACT_WORK *act )
{
	if( act->fldobj != NULL ){			//field obj監視型
		if( act->pAnmObj == NULL ){		//登録がまだ
			BLACT_WORK_PTR blact =
				FieldOBJ_DrawBlAct00_BlActPtrGet( act->fldobj );
			
			if( blact == NULL ){		//初期化がまだ
				return;
			}
			
			tw_OBJRotateAct_BlActResSet( work, act, blact );
		}
	}
}

typedef enum
{
	OBJRTRES_NON,
	OBJRTRES_DEL,
	OBJRTRES_INIT,
}OBJRTRES;

//--------------------------------------------------------------
/**
 * OBJ回転　OBJ生存チェック
 * @param	act	TW_OBJ_ROT_ACT_WORK
 * @retval	int TRUE=問題なし
 */
//--------------------------------------------------------------
static OBJRTRES tw_OBJRotateAct_UpdateSame( TW_OBJ_ROT_ACT_WORK *act )
{
	if( act->fldobj != NULL ){
		if( FieldOBJ_StatusBitCheck_Use(act->fldobj) == FALSE ){
			return( OBJRTRES_DEL );
		}
		
		if( FieldOBJ_OBJIDGet(act->fldobj) != act->obj_id ){
			return( OBJRTRES_DEL ); //OBJ IDが一致しない
		}
		
		if( act->pAnmObj != NULL ){			//リソース初期化済み
			if( act->obj_code != FieldOBJ_OBJCodeGet(act->fldobj) ){
				return( OBJRTRES_INIT );	//再初期化が必要である
			}
		}
	}else if( BLACT_GetState(act->blact) == BLACT_MOVE_NONE ){ //blact only
		return( OBJRTRES_DEL );
	}
	
	return( OBJRTRES_NON );
}

//--------------------------------------------------------------
/**
 * OBJ回転　削除
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJRotateProc_Delete( TORNWORLD_WORK *tw )
{
	int i;
	TW_OBJ_ROT_PROC_WORK *work = &tw->objrot;
	TW_OBJ_ROT_ACT_WORK *act = work->act_tbl;
	
	for( i = 0; i < work->max; i++, act++ ){
		tw_OBJRotateAct_BlActResFree( work, act );
	}
	
	if( work->pResAnm != NULL ){
		sys_FreeMemoryEz( work->pResAnm );
		work->pResAnm = NULL;
	}
	
	sys_FreeMemoryEz( work->act_tbl );
	work->act_tbl = NULL;
}

//--------------------------------------------------------------
/**
 * OBJ回転　登録
 * @param	act	TW_OBJ_ROT_ACT_WORK
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	rot	初期角度0-359
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJRotateAct_OBJRegist(
	TW_OBJ_ROT_ACT_WORK *act, FIELD_OBJ_PTR fldobj, int rot )
{
	act->rot_org = rot;
	act->rot_fx = NUM_FX32( rot );
	act->frame_fx = act->rot_fx;
	
	if( fldobj != NULL ){
		act->fldobj = fldobj;
		act->obj_id = FieldOBJ_OBJIDGet( fldobj );
	}
}

//--------------------------------------------------------------
/**
 * OBJ回転　指定OBJに回転動作を付ける為に登録
 * @param	tw	TORNWORLD_WORK
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	rot	0-359
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJRotate_RegOBJ(
	TORNWORLD_WORK *tw, FIELD_OBJ_PTR fldobj, int rot )
{
	int i = 0;
	TW_OBJ_ROT_PROC_WORK *work = &tw->objrot;
	TW_OBJ_ROT_ACT_WORK *act = work->act_tbl;
	
	while( i < work->max ){
		if( act->fldobj == NULL && act->blact == NULL ){
			tw_OBJRotateAct_OBJRegist( act, fldobj, rot );
			return;
		}
		
		i++;
		act++;
	}
	
	GF_ASSERT( 0 );
}

//--------------------------------------------------------------
/**
 * OBJ回転　指定OBJに回転動作を付ける為に登録　ビルボードアクターのみ
 * @param	tw	TORNWORLD_WORK
 * @param	blact
 * @param	rot	0-359
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJRotate_RegBlAct(
	TORNWORLD_WORK *tw, BLACT_WORK_PTR blact, int rot )
{
	int i = 0;
	TW_OBJ_ROT_PROC_WORK *work = &tw->objrot;
	TW_OBJ_ROT_ACT_WORK *act = work->act_tbl;
	
	while( i < work->max ){
		if( act->fldobj == NULL && act->blact == NULL ){
			tw_OBJRotateAct_OBJRegist( act, NULL, rot ); // blact only
			tw_OBJRotateAct_BlActResSet( work, act, blact );
			return;
		}
		
		i++;
		act++;
	}
	
	GF_ASSERT( 0 );
}

//--------------------------------------------------------------
/**
 * OBJ回転　指定されたビルボードアクターが使用しているワークを削除
 * @param	tw	TORNWORLD_WORK
 * @param	blact
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJRotate_BlActUseWorkFree(
		TORNWORLD_WORK *tw, CONST_BLACT_WORK_PTR blact )
{
	int i = 0;
	TW_OBJ_ROT_PROC_WORK *work = &tw->objrot;
	TW_OBJ_ROT_ACT_WORK *act = work->act_tbl;
	
	while( i < work->max ){
		if( act->blact == blact ){
			tw_OBJRotateAct_BlActResFree( work, act );
			return;
		}
		
		i++;
		act++;
	}
	
	GF_ASSERT( 0 );	//一致なし
}

//--------------------------------------------------------------
/**
 * OBJ回転　アクター常駐処理
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJRotateProc_ActProc( TORNWORLD_WORK *tw,
		TW_OBJ_ROT_PROC_WORK *work, TW_OBJ_ROT_ACT_WORK *act )
{
	OBJRTRES ret;
	
	if( act->fldobj == NULL && act->blact == NULL ){
		return;
	}
	
	ret = tw_OBJRotateAct_UpdateSame( act );
		
	if( ret == OBJRTRES_DEL ){
		tw_OBJRotateAct_BlActResFree( work, act );
		return;
	}
	
	if( ret == OBJRTRES_INIT ){
		#ifdef DEBUG_TW_PRINTF
		OS_Printf( "TW OBJRotate 再初期化開始\n" );
		#endif
		tw_OBJRotateAct_BlActResFreeOnly( work, act );
	}
	
	tw_OBJRotateAct_ResUpdate( work, act );
	
	if( act->add_frame ){		//フレーム処理
		act->add_frame--;
		twAngleAdd360Fx( &act->rot_fx, act->rot_add_fx );
		
		if( act->add_frame == 0 ){
			act->rot_fx = NUM_FX32( act->rot_org );
			//OS_Printf( "UVAnime %dFrame\n", FX32_NUM(act->rot_fx) );
		}
		
		act->frame_fx = act->rot_fx;
	}
}

//--------------------------------------------------------------
/**
 * OBJ回転　常駐処理	TCB常駐処理から呼び出し
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJRotateProcTcbCall( TORNWORLD_WORK *tw )
{
	int i;
	TW_OBJ_ROT_PROC_WORK *work = &tw->objrot;
	TW_OBJ_ROT_ACT_WORK *act = work->act_tbl;
	
	for( i = 0; i < work->max; i++, act++ ){
		tw_OBJRotateProc_ActProc( tw, work, act );
	}
}

//--------------------------------------------------------------
/**
 * ビルボードアクターから呼ばれるOBJ回転
 * @param	blact	BLACT_WORK_PTR
 * @param	wk		TW_BLACT_ROT_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void BlActOBJRotateDraw( BLACT_WORK_PTR blact, void *wk )
{
	TW_OBJ_ROT_ACT_WORK *act = wk;
	NNS_G3dAnmObjSetFrame( act->pAnmObj, act->frame_fx );
}

//--------------------------------------------------------------
/**
 * OBJ回転　フレーム回転アニメをセット
 * @param	work	TW_OBJ_ROT_WORK
 * @param	fldobj	対象となるFIELD_OBJ_PTR
 * @param	rot		増減値
 * @param	frame	フレーム
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJRotateAnmSet(
	TORNWORLD_WORK *tw, FIELD_OBJ_PTR fldobj, int rot, int frame )
{
	int i = 0;
	TW_OBJ_ROT_PROC_WORK *work = &tw->objrot;
	TW_OBJ_ROT_ACT_WORK *act = work->act_tbl;
	
	while( i < work->max ){
		if( act->fldobj == fldobj ){
			act->rot_fx = NUM_FX32( act->rot_org );	//オリジナルで初期化
			act->rot_add_fx = NUM_FX32(rot) / frame;
			act->add_frame = frame;
			twAngleAdd360( &act->rot_org, rot );
			return;
		}
		
		act++;
		i++;
	}
	
	GF_ASSERT( 0 );
}

#if 0
//--------------------------------------------------------------
/**
 * OBJ回転　外部呼び出し
 * @param	fsys	FIELDSYS_WORK
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	rot		回転角度 0-359
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldTornWorld_OBJRotateRegist(
		FIELDSYS_WORK *fsys, FIELD_OBJ_PTR fldobj, int rot )
{
	TORNWORLD_WORK *tw;
	tw = fsys->fldmap->Work;
	tw_OBJRotate_Regist( tw, fldobj, rot );
}
#endif

//--------------------------------------------------------------
/**
 * OBJ回転　外部呼出し　指定OBJの回転角度を取得
 * @param	fsys	FIELDSYS_WORK
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	int		回転角度 0-359
 */
//--------------------------------------------------------------
int FldTornWorld_OBJRotateGet( FIELDSYS_WORK *fsys, FIELD_OBJ_PTR fldobj )
{
	int i = 0;
	TORNWORLD_WORK *tw = fsys->fldmap->Work;
	TW_OBJ_ROT_PROC_WORK *work = &tw->objrot;
	TW_OBJ_ROT_ACT_WORK *act=  work->act_tbl;
	
	while( i < work->max ){
		if( act->fldobj == fldobj ){
			int frame = FX32_NUM( act->frame_fx );
			return( frame );
		}
			
		act++;
		i++;
	}
	
	return( 0 );	//登録無し
}

//--------------------------------------------------------------
/**
 * OBJ回転　外部呼出し　指定されたビルボードアクターに回転を付ける
 * @param	fsys	FIELDSYS_WORK
 * @param	blact	BLACT_WORK_PTR
 * @param	rot		回転角度
 * @retval	nothing
 */
//--------------------------------------------------------------
void FldTornWorld_BlActRotateAdd(
	FIELDSYS_WORK *fsys, BLACT_WORK_PTR blact, int rot )
{
	TORNWORLD_WORK *tw = fsys->fldmap->Work;
	tw_OBJRotate_RegBlAct( tw, blact, rot );
}

//--------------------------------------------------------------
/**
 * OBJ回転　外部呼出し。
 * FldTornWorld_BlActRotateAdd()で追加した回転処理の削除
 * @param	fsys	FIELDSYS_WORK
 * @param	blact	BLACT_WORK_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FldTornWorld_BlActRotateDel(
		FIELDSYS_WORK *fsys, CONST_BLACT_WORK_PTR blact )
{
	TORNWORLD_WORK *tw = fsys->fldmap->Work;
	tw_OBJRotate_BlActUseWorkFree( tw, blact );
}

//==============================================================================
//	やぶれたせかい　イベント
//==============================================================================
//--------------------------------------------------------------
/**
 * やぶれたせかい　イベント初期化
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_EventInit( TORNWORLD_WORK *tw )
{
	TW_EVENT_WORK *work = &tw->event;
}

//--------------------------------------------------------------
/**
 * やぶれたせかい　イベント削除
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_EventDelete( TORNWORLD_WORK *tw )
{
	TW_EVENT_WORK *work = &tw->event;
}

//--------------------------------------------------------------
/**
 * 破れた世界　イベント用ワーク初期化
 * @param	tw	TORNWORLD_WORK
 * @param	size	使用するワークサイズ
 * @retval	void*	ワーク*
 */
//--------------------------------------------------------------
static void * tw_EventWorkInit( TORNWORLD_WORK *tw, int size )
{
	GF_ASSERT( size <= TW_EVENTWORK_SIZE );
	{
		TW_EVENT_WORK *ev = &tw->event;
		void *work = ev->work;
		memset( work, 0, size );
		return( work );
	}
}

//--------------------------------------------------------------
/**
 * 破れた世界　イベント用ワーク取得
 * @param	tw	TORNWORLD_WORK
 * @retval	void*	ワーク*
 */
//--------------------------------------------------------------
static void * tw_EventWorkGet( TORNWORLD_WORK *tw )
{
	TW_EVENT_WORK *ev = &tw->event;
	void *work = ev->work;
	return( work );
}

//--------------------------------------------------------------
/**
 * やぶれたせかい　自機移動イベントチェック　自機移動開始時に発動
 * @param	fsys	FIELDSYS_WORK
 * @param	dir		DIR_UP等
 * @retval	BOOL	TRUE=イベント起動
 */
//--------------------------------------------------------------
BOOL FldTornWorld_PlayerMoveEventCheck( FIELDSYS_WORK *fsys, int dir )
{
	GIMMICKWORK *gmc;
	gmc = SaveData_GetGimmickWork( GameSystem_GetSaveData(fsys) );
	
	if( GIMMICKWORK_GetAssignID(gmc) == FLD_GIMMICK_TORNWORLD ){
		int gx,gy,gz,val;
		TORNWORLD_WORK *tw = fsys->fldmap->Work;
		
		val = Player_MoveSelectValueCheck( fsys->player, dir );
		twJikiGridPosGet( tw, &gx, &gy, &gz );
		
		//飛び石出現イベント
		if( val == OBJ_MOVE_VALUE_WALK ){
			tw_EventMoveCheck_FStoneAdvent( tw, gx, gy, gz, dir );
		}
		
		//座標起動カメラアクション
		if( tw_EventMoveCheck_PosCameraAction(tw,gx,gy,gz,dir) == TRUE ){
			//起動しても何もしない？
		}
		
		//自機座標イベント
		if( tw_EventMoveCheck_PosJikiEvent(tw,gx,gy,gz,dir) == TRUE ){
			return( TRUE );
		}
		
		//滝登りチェック
		{
			u32 zone_id = twNowZoneID( tw );
			
			if( zone_id == ZONE_ID_D34R0105 ){
				if( dir == DIR_RIGHT &&
					gx == 104 && gy == 170 && gz >= 76 && gz <= 79 ){
					tw_EventPosScrSetMain( tw, DATA_Scr0105_Taki );
					return( TRUE );
				}
			}else if( zone_id == ZONE_ID_D34R0107 ){
				if( dir == DIR_RIGHT &&
					gx == 104 && gy == 128 && gz >= 76 && gz <= 79 ){
					tw_EventPosScrSetMain( tw, Data_Scr0107_Taki );
					return( TRUE );
				}
			}
		}
		
		//八層深部限定カメラアクション
		
		#if 0
		//マップ遷移チェック
		{
			u32 zone_id = twNowZoneID( tw );
			EVENTWORK *ev = SaveData_GetEventWork( tw->fsys->savedata );
			
			if( zone_id == ZONE_ID_D34R0109 &&
				SysWork_TornWorldEventSeqNoGet(ev) == TWEVSEQ_08_02 && 
				gx == 89 && gy == 65 && gz == 57 && dir == DIR_UP ){
				EventSet_Script( fsys, SCRID_D34R0109_GIRA_WARP, NULL );
				return( TRUE );
			}else if( zone_id == ZONE_ID_D34R0110 &&
				gx == 15 && gy == 1 && gz == 25 && dir == DIR_DOWN ){
				EventSet_Script( fsys, SCRID_D34R0110_08_WARP, NULL );
				return( TRUE );
			}
		}
		#endif
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * やぶれたせかい　イベントチェック　一歩移動開始のタイミング
 * @param	fsys	FIELDSYS_WORK
 * @param	dir		移動方向
 * @retval	BOOL	TRUE=イベント起動
 */
//--------------------------------------------------------------
BOOL FldTornWorld_StepEventCheck( FIELDSYS_WORK *fsys, int dir )
{
	TORNWORLD_WORK *tw = fsys->fldmap->Work;
	u32 zone_id = twNowZoneID( tw );
	
	//ゾーン別各種イベントチェック
	switch( zone_id ){
	#if 0	//自機イベントチェックへ移動
	case ZONE_ID_D34R0105:		//滝下りイベントチェック
		if( dir == DIR_RIGHT ){
			int gx,gy,gz;
			twJikiGridPosGet( tw, &gx, &gy, &gz );
			if( gx == 104 && gy == 170 && gz >= 75 && gz <= 79 ){
				tw_EventPosScrSetMain( tw, DATA_Scr0105_Taki );
				return( TRUE );
			}
		}
		break;
	case ZONE_ID_D34R0107:		//滝登りイベントチェック
		if( dir == DIR_RIGHT ){
			int gx,gy,gz;
			twJikiGridPosGet( tw, &gx, &gy, &gz );
			if( gx == 104 && gy == 128 && gz >= 75 && gz <= 79 ){
				tw_EventPosScrSetMain( tw, Data_Scr0107_Taki );
				return( TRUE );
			}
		}
		break;
	#endif
	case ZONE_ID_D34R0109:		//8->8Dイベント限定ゾーン遷移チェック
		if( dir == DIR_UP ){
			EVENTWORK *ev = SaveData_GetEventWork( tw->fsys->savedata );
			if( SysWork_TornWorldEventSeqNoGet(ev) >= TWEVSEQ_08_02 ){
				int gx,gy,gz;
				twJikiGridPosGet( tw, &gx, &gy, &gz );
				if( gx == 89 && gy == 65 && gz == 57 ){
					EventSet_Script( fsys, SCRID_D34R0109_GIRA_WARP, NULL );
					return( TRUE );
				}
			}
		}
		break;
	case ZONE_ID_D34R0110:		//8D->8イベント限定ゾーン遷移チェック
		if( dir == DIR_DOWN ){
			int gx,gy,gz;
			twJikiGridPosGet( tw, &gx, &gy, &gz );
			if( gx == 15 && gy == 1 && gz == 25 ){
				EventSet_Script( fsys, SCRID_D34R0110_08_WARP, NULL );
				return( TRUE );
			}
		}
		break;
	}
	
	return( FALSE );
}

#if 0	//old
BOOL FldTornWorld_StepEventCheck( FIELDSYS_WORK *fsys, int dir )
{
	GIMMICKWORK *gmc;
	gmc = SaveData_GetGimmickWork( GameSystem_GetSaveData(fsys) );
	TORNWORLD_WORK *tw = fsys->fldmap->Work;
	
	if( GIMMICKWORK_GetAssignID(gmc) != FLD_GIMMICK_TORNWORLD ){
		return( FALSE );	//破れた世界ではない
	}
	
#if 0
	{
		int gx,gy,gz;
		TORNWORLD_WORK *tw = fsys->fldmap->Work;
		twJikiGridPosGet( tw, &gx, &gy, &gz );
		
		//座標起動カメラアクション
		if( tw_EventMoveCheck_PosCameraAction(tw,gx,gy,gz,dir) == TRUE ){
			//起動しても何もしない？
		}
		
		//自機座標イベント
		if( tw_EventMoveCheck_PosJikiEvent(tw,gx,gy,gz,dir) == TRUE ){
			return( TRUE );
		}
	}
#endif
	
	{
		int gx,gy,gz;
		twJikiGridPosGet( tw, &gx, &gy, &gz );
		
		{
			u32 zone_id = twNowZoneID( tw );
			EVENTWORK *ev = SaveData_GetEventWork( tw->fsys->savedata );
			
			//8<->8Dイベント限定ゾーン遷移チェック
			if( zone_id == ZONE_ID_D34R0109 &&
				SysWork_TornWorldEventSeqNoGet(ev) >= TWEVSEQ_08_02 && 
				gx == 89 && gy == 65 && gz == 57 && dir == DIR_UP ){
				EventSet_Script( fsys, SCRID_D34R0109_GIRA_WARP, NULL );
				return( TRUE );
			}else if( zone_id == ZONE_ID_D34R0110 &&
				gx == 15 && gy == 1 && gz == 25 && dir == DIR_DOWN ){
				EventSet_Script( fsys, SCRID_D34R0110_08_WARP, NULL );
				return( TRUE );
			}
		}
	}
	
	return( FALSE );
}
#endif

//--------------------------------------------------------------
/**
 * やぶれたせかい　イベントチェック　一歩移動終了のタイミング
 * @param	fsys	FIELDSYS_WORK
 * @retval	BOOL	TRUE=イベント起動
 */
//--------------------------------------------------------------
BOOL FldTornWorld_MoveEventCheck( FIELDSYS_WORK *fsys )
{
	GIMMICKWORK *gmc;
	gmc = SaveData_GetGimmickWork( GameSystem_GetSaveData(fsys) );
	
	if( GIMMICKWORK_GetAssignID(gmc) != FLD_GIMMICK_TORNWORLD ){
		return( FALSE );	//破れた世界ではない
	}
	
	{
		int gx,gy,gz;
		int dir = Player_DirGet( fsys->player );
		TORNWORLD_WORK *tw = fsys->fldmap->Work;
		twJikiGridPosGet( tw, &gx, &gy, &gz );
		
		//ゾーン切り替えイベント
		if( tw_EventMoveCheck_ZoneChange(tw,gx,gy,gz) == TRUE ){
			return( TRUE );
		}
		
		//座標スクリプト起動チェック
		if( tw_EventPosCheck_Script(tw,gx,gy,gz) == TRUE ){
			return( TRUE );
		}
		
		//マップ遷移チェック
		{
			u32 zone_id = twNowZoneID( tw );
			EVENTWORK *ev = SaveData_GetEventWork( tw->fsys->savedata );
			
			if( zone_id == ZONE_ID_D34R0109 && dir == DIR_UP ){
				if( SysWork_TornWorldEventSeqNoGet(ev) >= TWEVSEQ_08_02 ){
					if( gx == 89 && gy == 65 && (gz == 56 || gz == 57) ){
						EventSet_Script(
							fsys, SCRID_D34R0109_GIRA_WARP, NULL );
						return( TRUE );
					}
				}
			}else if( zone_id == ZONE_ID_D34R0110 && dir == DIR_DOWN ){
				if( gx == 15 && gy == 1 && (gz == 25 || gz == 26) ){
					EventSet_Script(
						fsys, SCRID_D34R0110_08_WARP, NULL );
					return( TRUE );
				}
			}
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * やぶれたせかい　イベントチェック　障害物押し込みのタイミング
 * @param	fsys	FIELDSYS_WORK
 * @param	dir		移動方向
 * @retval	BOOL	TRUE=イベント起動
 */
//--------------------------------------------------------------
BOOL FldTornWorld_PushEventCheck( FIELDSYS_WORK *fsys, int dir )
{
	TORNWORLD_WORK *tw = fsys->fldmap->Work;
	u32 zone_id = twNowZoneID( tw );
	
	//ゾーン別各種イベントチェック
	switch( zone_id ){
	case ZONE_ID_D34R0109:		//8->8Dイベント限定ゾーン遷移チェック
		if( dir == DIR_UP ){
			EVENTWORK *ev = SaveData_GetEventWork( tw->fsys->savedata );
			if( SysWork_TornWorldEventSeqNoGet(ev) >= TWEVSEQ_08_02 ){
				int gx,gy,gz;
				twJikiGridPosGet( tw, &gx, &gy, &gz );
				if( gx == 89 && gy == 65 && gz == 57 ){
					EventSet_Script( fsys, SCRID_D34R0109_GIRA_WARP, NULL );
					return( TRUE );
				}
			}
		}
		break;
	case ZONE_ID_D34R0110:		//8D->8イベント限定ゾーン遷移チェック
		if( dir == DIR_DOWN ){
			int gx,gy,gz;
			twJikiGridPosGet( tw, &gx, &gy, &gz );
			if( gx == 15 && gy == 1 && gz == 25 ){
				EventSet_Script( fsys, SCRID_D34R0110_08_WARP, NULL );
				return( TRUE );
			}
		}
		break;
	}
	
	return( FALSE );
}

//==============================================================================
//	イベント　座標起動カメラアクション
//==============================================================================
//--------------------------------------------------------------
/**
 * 座標起動カメラアクションチェック
 * @param	tw	TORNWORLD_WORK
 * @param	gx	自機グリッドX
 * @param	gy	自機グリッドY
 * @param	gz	自機グリッドZ
 * @param	dir	自機進行方向 DIR_UP等
 * @retval	BOOL	TRUE=起動
 */
//--------------------------------------------------------------
static BOOL tw_EventMoveCheck_PosCameraAction(
	TORNWORLD_WORK *tw, int gx, int gy, int gz, int dir )
{
	const TW_CMACTDATA *data;
	
	data = tw_MapDataCmActDataPosCheck( tw, gx, gy, gz, dir );
	
	if( data == NULL ){
		return( FALSE );
	}
	
	tw_CameraActionSet( tw, data );
	return( TRUE );
}

//==============================================================================
//	自機操作
//==============================================================================
//--------------------------------------------------------------
/**
 * 自機を破れた世界専用に変更
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_JikiInit( TORNWORLD_WORK *tw )
{
	u32 type,code;
	int gx,gy,gz,form;
	int rot[HEROTWTYPE_MAX] = { 0, 0, 0, 90, 270, 180 };
	PLAYER_STATE_PTR jiki = tw->fsys->player;
	FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( jiki );
	
	//影の非表示化->フィールドOBJシステム側の設定で行う様にする
	//FieldOBJ_StatusBit_ON( fldobj, FLDOBJ_STA_BIT_SHADOW_VANISH );
	
	//自機現在座標の取得
	twJikiGridPosGet( tw, &gx, &gy, &gz );
	
	//座標から地形種類割り出し
	#if 0	//これでは混合地形に対処できない
	type = tw_MapDataPosGndTypeGet( tw, gx, gy, gz );
	#else	//地形初期化時にセットされた情報
	type = tw_MapDataNowGndTypeGet( tw );
	#endif
	type = twGndTypeHeroType( type );
	
	//地形別に高さ取得フラグを変更
	if( type == HEROTWTYPE_ON ){
		FieldOBJ_StatusBitSet_HeightGetOFF( fldobj, FALSE );
	}else{
		FieldOBJ_StatusBitSet_HeightGetOFF( fldobj, TRUE );
	}
	
	//地形別に自機動作変更
	Player_MoveBitSet_TWorld( jiki, type );
	
	//自機表示コード変更
	form = HERO_FORM_NORMAL;
	code = FieldOBJ_OBJCodeGet( fldobj );
	
	switch( code ){
	case HERO:
	case RTHERO:
		code = RTHERO;
		break;
	case HEROINE:
	case RTHEROINE:
		code = RTHEROINE;
		break;
	case SWIMHERO:
	case SWIMHERORT:
		code = SWIMHERORT;
		form = HERO_FORM_SWIM;
		break;
	case SWIMHEROINE:
	case SWIMHEROINERT:
		code = SWIMHEROINERT;
		form = HERO_FORM_SWIM;
		break;
	default:
		GF_ASSERT( 0 );
	}
	
	Player_FormSet( jiki, form );
	FieldOBJ_DrawReset( fldobj, code );
	
	if( type != HEROTWTYPE_ON ){
		GF_ASSERT( FieldOBJ_HeightOFFCheck(fldobj) == TRUE );
	}
	
	if( form == HERO_FORM_SWIM ){
		EOA_PTR eoa;
		int dir = Player_DirGet( jiki );
		eoa = FE_FldOBJNamiPokeRSet( jiki, gx, gy, gz, dir, TRUE, type );
		Player_JointEoaSet( jiki, eoa );
	}
	
	//地形別に表示角度セット
	tw_OBJRotate_RegOBJ( tw, fldobj, rot[type] );
	
	#ifdef DEBUG_TW_PRINTF
	{
		VecFx32 pos;
		
		twJikiGridPosGet( tw, &gx, &gy, &gz );
		OS_Printf( "TW JikiInit GX=%d,GY=%d,GZ=%d ", gx, gy, gz );
	
		switch( type ){
		case HEROTWTYPE_GROUND: OS_Printf( "TYPE_GROUND\n" ); break;
		case HEROTWTYPE_WLEFT:	OS_Printf( "TYPE_WLEFT\n" ); break;
		case HEROTWTYPE_WRIGHT:	OS_Printf( "TYPE_WRIGHT\n" ); break;
		case HEROTWTYPE_ROOF:	OS_Printf( "TYPE_ROOF\n" ); break;
		default:				OS_Printf( "TYPE_ON\n" ); break;
		}
	
		if( type != HEROTWTYPE_ON ){
			if( Player_MoveBitCheck_TWorldSp(jiki) == FALSE ){
				GF_ASSERT( 0 );
			}
		}
		
		FieldOBJ_VecPosGet( fldobj, &pos );
		
		OS_Printf( "TW JikiInit Pos X=%xH(%d),Y=%xH(%d),Z=%xH(%d)\n",
				pos.x, SIZE_GRID_FX32(pos.x),
				pos.y, SIZE_GRID_FX32(pos.y),
				pos.z, SIZE_GRID_FX32(pos.z) );
	}
	#endif
}

//--------------------------------------------------------------
/**
 * 破れた世界用の自機を削除
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void tw_JikiDelete( TORNWORLD_WORK *tw )
{
	PLAYER_STATE_PTR jiki = tw->fsys->player;
	Player_MoveBitSet_TWorld( jiki, HEROTWTYPE_NON );
	Player_MoveSpeedClear( jiki );
}	

//==============================================================================
//	自機イベント
//==============================================================================
//--------------------------------------------------------------
/**
 * 座標起動自機イベントチェック
 * @param	tw	TORNWORLD_WORK
 * @param	gx	自機グリッドX
 * @param	gy	自機グリッドY
 * @param	gz	自機グリッドZ
 * @param	dir	自機進行方向 DIR_UP等
 * @retval	BOOL	TRUE=起動
 */
//--------------------------------------------------------------
static BOOL tw_EventMoveCheck_PosJikiEvent(
		TORNWORLD_WORK *tw, int gx, int gy, int gz, int dir )
{
	{	//壁ジャンプイベント
		const TW_EVJUMPDATA *data;
		data = tw_MapDataEvJumpDataPosCheck( tw, gx, gy, gz, dir );
		
		if( data ){
			FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( tw->fsys->player );
	//		イベント側でセットする
	//		tw_OBJRotateAnmSet( tw, fldobj, data->rot_val, data->frame );
			DATA_JikiPosEventProcWallJumpTbl[data->type]( tw, data );
			return( TRUE );
		}
	}
	
	return( FALSE );
}

//==============================================================================
//	自機イベント　壁ジャンプ	
//==============================================================================
//--------------------------------------------------------------
/**
 * 自機平面→壁ジャンプイベントセット
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void tw_JikiEventSet_WallJump(
		TORNWORLD_WORK *tw, const TW_EVJUMPDATA *data )
{
	TW_JIKI_WALLJUMP_WORK *work;
	work = tw_EventWorkInit( tw, sizeof(TW_JIKI_WALLJUMP_WORK) );
	
	work->data = *data;
	work->frame = data->frame;
	work->move_val.x = GRID_SIZE_FX32( data->gx_size ) / data->frame;
	work->move_val.y = GRID_SIZE_FX32( data->gy_size ) / data->frame;
	work->move_val.z = GRID_SIZE_FX32( data->gz_size ) / data->frame;
	work->offs_frame_val = OFFS_FRAME_MAX_FX / data->frame;
	
	work->grid_count_val = work->move_val;
	if( work->grid_count_val.x < 0 ){
		work->grid_count_val.x = -work->grid_count_val.x;
	}
	if( work->grid_count_val.y < 0 ){
		work->grid_count_val.y = -work->grid_count_val.y;
	}
	if( work->grid_count_val.z < 0 ){
		work->grid_count_val.z = -work->grid_count_val.z;
	}
	
	FieldEvent_Set( tw->fsys, tw_GMEVENT_JikiWallJump, tw );
}

#if 0
//--------------------------------------------------------------
/**
 * 自機　壁ジャンプ用ワークセット
 * @param
 * @param	
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void JikiJumpWallParamSet(
		TW_JIKI_WALLJUMP_WORK *work,
		const EVPOSDATA_
		int gx_size, int gy_size, int gz_size, int frame,
		OFFS_VEC_TYPE offs_vec_type, OFFS_VEC_FLIP offs_vec_flip,
		int dir_after, int form_after )
{
	memset( work, 0, sizeof(TW_JIKI_WALLJUMP_WORK) );
	
	work->frame = frame;
	work->dir_after = dir_after;
	work->form_after = form_after;
	work->gx_size = gx_size;
	work->gy_size = gy_size;
	work->gz_size = gz_size;
	
	work->move_val.x = GRID_SIZE_FX32( gx_size ) / frame;
	work->move_val.y = GRID_SIZE_FX32( gy_size ) / frame;
	work->move_val.z = GRID_SIZE_FX32( gz_size ) / frame;
	
	work->offs_vec_type = offs_vec_type;
	work->offs_vec_flip = offs_vec_flip;
	work->offs_frame_val = OFFS_FRAME_MAX_FX / frame;
	
	work->grid_count_val = work->move_val;
	
	if( work->grid_count_val.x < 0 ){
		work->grid_count_val.x = -work->grid_count_val.x;
	}
	if( work->grid_count_val.y < 0 ){
		work->grid_count_val.y = -work->grid_count_val.y;
	}
	if( work->grid_count_val.z < 0 ){
		work->grid_count_val.z = -work->grid_count_val.z;
	}
}
#endif

//--------------------------------------------------------------
/**
 * 自機　壁ジャンプイベント
 * @param	event GMEVENT_CONTROL
 * @retval	BOOL	TRUE=終了
 */
//--------------------------------------------------------------
static BOOL tw_GMEVENT_JikiWallJump( GMEVENT_CONTROL *event )
{
	FIELDSYS_WORK *fsys = FieldEvent_GetFieldSysWork( event );
	TORNWORLD_WORK *tw = FieldEvent_GetSpecialWork( event );
	TW_JIKI_WALLJUMP_WORK *work = tw_EventWorkGet( tw );
	PLAYER_STATE_PTR jiki = fsys->player;
	FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( jiki );
	
	switch( work->seq_no ){
	case 0:
		{	//div_map.cの自機情報更新待ち
			work->seq_no++;
		}
		break;
	case 1:	//向きを壁へ向ける
		if( FieldOBJ_AcmdSetCheck(fldobj) == FALSE ){
			break;
		}else{
			int dir,code,type;
			dir = work->data.dir;
			type = tw_MapDataGndDataGndTypeNowGet( tw );
			
			switch( type ){
			case TW_GNDTYPE_MAX:
			case TW_GNDTYPE_GROUND:
				break;
			case TW_GNDTYPE_SIDE_L:
				{
					int tbl[] = {DIR_LEFT,DIR_RIGHT,DIR_UP,DIR_DOWN};
					dir = tbl[dir];
				}
				break;
			case TW_GNDTYPE_SIDE_R:
				{
					int tbl[] = {DIR_RIGHT,DIR_LEFT,DIR_UP,DIR_DOWN};
					dir = tbl[dir];
				}
				break;
			case TW_GNDTYPE_ROOF:
				{
					int tbl[] = {DIR_DOWN,DIR_UP,DIR_RIGHT,DIR_LEFT};
					dir = tbl[dir];
				}
				break;
			}
			
			code = FieldOBJ_AcmdCodeDirChange( dir, AC_DIR_U );
			FieldOBJ_AcmdSet( fldobj, code );
			FieldOBJ_DirDispCheckSet( fldobj, dir ); //完全強制で振り向かせる
			tw_OBJRotateAnmSet( tw, fldobj,
				work->data.rot_val, work->data.frame );
			work->seq_no++;
		}
	case 2:
		if( JikiJumpWall(work,fldobj) != TRUE ){
			break;
		}
		
		{
			HEROTWTYPE type = HEROTWTYPE_ON;
			FE_KEMURI_TYPE kemuri = FE_KEMURI_TYPE_FLAT;
			FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( jiki );
			
			//特殊地形データの更新
			#ifdef DEBUG_TW_PRINTF
			OS_Printf( "tw ground after %d\n", work->data.gndno_after );
			#endif
			tw_MapDataGndDataNoSet( tw, work->data.gndno_after );
			type = twGndTypeHeroType( work->data.gnd_after );
			Player_MoveBitSet_TWorld( jiki, type );
			Player_MoveSpeedClear( jiki );
			
			#ifdef DEBUG_TW_PRINTF
			switch( type ){
			case HEROTWTYPE_ON: OS_Printf("着地地面 平面\n"); break;
			case HEROTWTYPE_GROUND: OS_Printf("着地地面 特殊平面\n"); break;
			case HEROTWTYPE_WLEFT: OS_Printf("着地地面 左壁\n"); break;
			case HEROTWTYPE_WRIGHT: OS_Printf("着地地面 右壁\n"); break;
			case HEROTWTYPE_ROOF: OS_Printf( "着地地面 天井面\n" ); break;
			default: GF_ASSERT( 0 );
			}
			#endif
			
			#ifdef TESTPROC_JIKI_HEIGHT_SWITCH
			if( work->data.gndno_after < 0 ){	//着地地形番号が無効
				OS_Printf( "着地地面番号 無効　高さ取得ON\n" );
				FieldOBJ_StatusBitSet_HeightGetOFF( fldobj, FALSE );
			}else{
				OS_Printf( "着地地面番号 有効　高さ取得OFF\n" );
				FieldOBJ_StatusBitSet_HeightGetOFF( fldobj, TRUE );
			}	
			#endif
			
			//着地煙エフェクト
			switch( work->data.gnd_after ){
			case TW_GNDTYPE_SIDE_L: kemuri = FE_KEMURI_TYPE_RIGHTG; break;
			case TW_GNDTYPE_SIDE_R: kemuri = FE_KEMURI_TYPE_LEFTG; break;
			case TW_GNDTYPE_ROOF: kemuri = FE_KEMURI_TYPE_ROOFG; break;
			}
			FE_FldOBJKemuriTornWorld_Add( fldobj, kemuri );
			Snd_SePlay( SE_SUTYA2 );
			
			work->seq_no++;
		}
		break;
	case 3:
		if( FieldOBJ_AcmdSetCheck(fldobj) == FALSE ){
			break;
		}else{
			int code = FieldOBJ_AcmdCodeDirChange(
					work->data.dir_after, AC_DIR_U );
			FieldOBJ_AcmdSet( fldobj, code );
			return( TRUE );
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * 自機　壁ジャンプ移動
 * @param	
 * @param	
 * @param
 * @retval
 */
//--------------------------------------------------------------
static BOOL JikiJumpWall(
		TW_JIKI_WALLJUMP_WORK *work, FIELD_OBJ_PTR fldobj )
{
	const TW_EVJUMPDATA *data = &work->data;
	
	{
		VecFx32 pos;
		FieldOBJ_VecPosGet( fldobj, &pos );
		pos.x += work->move_val.x;
		pos.y += work->move_val.y;
		pos.z += work->move_val.z;
		FieldOBJ_VecPosSet( fldobj, &pos );
	}
	
	{
		int grid;
		work->grid_count.x += work->grid_count_val.x;
		
		if( work->grid_count.x >= GRID_FX32 ){
			work->grid_count.x -= GRID_FX32;
			
			grid = FieldOBJ_NowPosGX_Get( fldobj );
			if( data->gx_size > 0 ){ grid++; }
			else if( data->gx_size < 0 ){ grid--; }
			FieldOBJ_NowPosGX_Set( fldobj, grid );
		}
		
		work->grid_count.y += work->grid_count_val.y;
		
		if( work->grid_count.y >= H_GRID_FX32 ){
			work->grid_count.y -= H_GRID_FX32;
		
			grid = FieldOBJ_NowPosGY_Get( fldobj );
			if( data->gy_size > 0 ){ grid++; }
			else if( data->gy_size < 0 ){ grid--; }
			FieldOBJ_NowPosGY_Set( fldobj, grid );
		}
		
		work->grid_count.z += work->grid_count_val.z;
		
		if( work->grid_count.z >= GRID_FX32 ){
			work->grid_count.z -= GRID_FX32;
		
			grid = FieldOBJ_NowPosGZ_Get( fldobj );
			if( data->gz_size > 0 ){ grid++; }
			else if( data->gz_size < 0 ){ grid--; }
			FieldOBJ_NowPosGZ_Set( fldobj, grid );
		}
	}
	
	{
		int no;
		fx32 *offs;
		VecFx32 *vec = FieldOBJ_VecDrawOffsPtrGet( fldobj );
		const fx32 *tbl = DATA_JumpOffsTblType0;
		
		work->offs_frame += work->offs_frame_val;
		no = FX32_NUM( work->offs_frame );
		
		switch( data->offs_vec_type ){
		case OFFS_VEC_X: offs = &vec->x; break;
		case OFFS_VEC_Y: offs = &vec->y; break;
		case OFFS_VEC_Z: offs = &vec->z; break;
		default:
			GF_ASSERT( 0 ); break;
		}
		
		*offs = tbl[no];
		
		if( data->offs_vec_flip == TRUE ){
			*offs = -(*offs);
		}
		
		work->frame--;
		
		if( work->frame <= 0  ){
			*offs = 0;
			FieldOBJ_GPosUpdate( fldobj );
			FieldOBJ_DrawProcCall( fldobj );
			
			FieldOBJ_DirDispCheckSet( fldobj, data->dir_after );
			FieldOBJ_DrawStatusSet( fldobj, DRAW_STA_STOP );
			return( TRUE );
		}
	}
	
	return( FALSE );
}

//==============================================================================
//	背景BG
//==============================================================================
//--------------------------------------------------------------
/**
 * やぶれたせかい　背景BG初期化
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_BGInit( TORNWORLD_WORK *tw )
{
	char *scr_buf;
	
	GF_Disp_GX_VisibleControl( TW_BGFRM_BG, VISIBLE_OFF );
	
	{	//パレット バックグラウンドパレットを換える
		char *pltt_buf;
		NNSG2dPaletteData *pPltt;
	
		pltt_buf = ArchiveDataLoadAllocByHandle(
			tw->arc_h_etc, NARC_tw_arc_etc_yabu_worldbg_NCLR, HEAPID_FIELD );
		NNS_G2dGetUnpackedPaletteData( pltt_buf, &pPltt );
		
		GF_BGL_PaletteSet(
			TW_BGFRM_BG, pPltt->pRawData, 32, 32*(TW_BGFRM_BG_PLTT_OFFS) );
		
		{	//フェード用に元のパレットを保存
			int i;
			TW_PLTT_WORK *pltt = &tw->pltt;
			u16 *base = pPltt->pRawData;
			base = &base[TW_BGFRM_BG_PLTT_OFFS*16];
			
			for( i = 0; i < 16; i++ ){
				pltt->bg3_plttbase[i] = base[i];
				pltt->bg3_plttbuf[i] = base[i];
			}
		}
		
		sys_FreeMemoryEz( pltt_buf );
	}
	
	{	//キャラ
		char *char_buf;
		NNSG2dCharacterData *pChar;
		
		char_buf = ArchiveDataLoadAllocByHandle( 
			tw->arc_h_etc, NARC_tw_arc_etc_yabu_worldbg_NCGR, HEAPID_FIELD );
		NNS_G2dGetUnpackedCharacterData( char_buf, &pChar );
		GF_BGL_LoadCharacter( tw->fsys->bgl, TW_BGFRM_BG,
				pChar->pRawData,
				pChar->szByte,
				0 );
		sys_FreeMemoryEz( char_buf );
	}
	
	{	//スクリーン
		char *scr_buf;
		NNSG2dScreenData *pScr;
		
		scr_buf = ArchiveDataLoadAllocByHandle( 
			tw->arc_h_etc, NARC_tw_arc_etc_yaby_worldbg0_NSCR, HEAPID_FIELD );
		NNS_G2dGetUnpackedScreenData( scr_buf, &pScr );
		GF_BGL_LoadScreen( tw->fsys->bgl, TW_BGFRM_BG,
				(void*)pScr->rawData, pScr->szByte, 0 );
		GF_BGL_ScreenBufSet( tw->fsys->bgl,
			TW_BGFRM_BG, (void*)pScr->rawData, pScr->szByte );
		GF_BGL_LoadScreenReq( tw->fsys->bgl, TW_BGFRM_BG );
		sys_FreeMemoryEz( scr_buf );
	}
	
	{	//プライオリティ変更
		G2_SetBG0Priority( 2 );	//pol
		G2_SetBG1Priority( 1 );	//effect
		G2_SetBG2Priority( 3 );	//effect
		G2_SetBG3Priority( 0 );	//font
	}
	
	{	//アルファ変更
		G2_SetBlendAlpha(
			GX_BLEND_PLANEMASK_BG0,
			GX_BLEND_PLANEMASK_BG1|
			GX_BLEND_PLANEMASK_BG2|
			GX_BLEND_PLANEMASK_OBJ|
			GX_BLEND_PLANEMASK_BD,
			0, 16 );
	}
	
	GF_Disp_GX_VisibleControl( TW_BGFRM_BG, VISIBLE_ON );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
}

//--------------------------------------------------------------
/**
 * やぶれたせかい　背景BG削除
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_BGDelete( TORNWORLD_WORK *tw )
{
	G2_BlendNone();	//アルファ無効に
#if 0
	//プライオリティを戻す
	G2_SetBG0Priority( 1 );	//pol
	G2_SetBG1Priority( 2 );	//effect
	G2_SetBG2Priority( 3 );	//effect
	G2_SetBG3Priority( 0 );	//font
#endif
}

//==============================================================================
//	セルアクター
//==============================================================================
//--------------------------------------------------------------
/**
 * やぶれたせかい　セルアクターセット初期化
 * @param	tw	TORNWORLD_WORK
 * @param	set	TW_CLACT_SETDATA *
 * @param	hd	ARCHANDLE *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ClActInit(
		TORNWORLD_WORK *tw, TW_CLACT_SETDATA *set, ARCHANDLE *hd )
{
//	TW_CLACT_SETDATA *set = &tw->clact_setdata;
	
	set->clact_set = CLACT_U_SetEasyInit(
			TW_CLACT_MAX, &set->clact_easydata, HEAPID_FIELD );
	
	//サーフェース変更
	CLACT_U_SetMainSurfaceMatrix(
		&set->clact_easydata, TW_CLACT_SFACE_X_FX, TW_CLACT_SFACE_Y_FX );
	
	//リソースマネージャ初期化
	set->clact_resm_char = CLACT_U_ResManagerInit(
			TW_CLACT_RES_CHAR_MAX, CLACT_U_CHAR_RES, HEAPID_FIELD );
	set->clact_resm_pltt = CLACT_U_ResManagerInit(
			TW_CLACT_RES_PLTT_MAX, CLACT_U_PLTT_RES, HEAPID_FIELD );
	set->clact_resm_cell = CLACT_U_ResManagerInit(
			TW_CLACT_RES_CELL_MAX, CLACT_U_CELL_RES, HEAPID_FIELD );
	set->clact_resm_anm = CLACT_U_ResManagerInit(
			TW_CLACT_RES_ANM_MAX, CLACT_U_CELLANM_RES, HEAPID_FIELD );
	
	{	//リソースセット
		int i;
		
		for( i = 0; i < TW_CLACT_RES_CHAR_MAX; i++ ){
			set->clact_resobj_char[i] =
				CLACT_U_ResManagerResAddArcChar_ArcHandle(
					set->clact_resm_char, hd,
					DATA_TwClActDataArcIdxTbl_Char[i], FALSE,
					CLACTRESID(i),
					NNS_G2D_VRAM_TYPE_2DMAIN, HEAPID_FIELD );
			CLACT_U_CharManagerSetAreaCont( set->clact_resobj_char[i] );
		}
		
		#ifdef TW_CELL_PLTT_TEST
		{
			u32 vram;
			char *pltt_buf,*pltt_res;
			NNSG2dPaletteData *pPltt;
			NNSG2dImageProxy *proxy_image;
			const NNSG2dImagePaletteProxy *proxy_pltt;
			
			pltt_buf = ArchiveDataLoadAllocByHandle( hd,
					NARC_tw_arc_etc_cloud_all_NCLR, HEAPID_FIELD );
			NNS_G2dGetUnpackedPaletteData( pltt_buf, &pPltt );
			pltt_res = (char *)pPltt->pRawData;
			
			for( i = 0; i < TW_CLACT_RES_PLTT_MAX; i++ ){
				set->clact_resobj_pltt[i] =
					CLACT_U_ResManagerResAddArcPltt_ArcHandle(
						set->clact_resm_pltt, hd,
						DATA_TwClActDataArcIdxTbl_Pltt[0], FALSE,
						CLACTRESID(i),
						NNS_G2D_VRAM_TYPE_2DMAIN, 5, HEAPID_FIELD );
			}
			
			DC_FlushRange( pltt_buf, pPltt->szByte );
			
			for( i = 0; i < TW_CLACT_RES_PLTT_MAX; i++ ){
				proxy_pltt = GetPlttIDProxy( i );
				vram = NNS_G2dGetImagePaletteLocation(
					proxy_pltt, NNS_G2D_VRAM_TYPE_2DMAIN );
				GX_LoadOBJPltt( &pltt_buf[i*32], vram, 32 );
			}
			
			sys_FreeMemoryEz( pltt_buf );
		}
		#else
		set->clact_resobj_pltt[0] =
			CLACT_U_ResManagerResAddArcPltt_ArcHandle(
				set->clact_resm_pltt,				//pltt res
				hd,									// arc hd
				DATA_TwClActDataArcIdxTbl_Pltt[0], // arc index
				FALSE,
				CLACTRESID(0), //id
				NNS_G2D_VRAM_TYPE_2DMAIN,
				TW_CLACT_RES_PLTT_CLOUD_NUM, //pal count
				HEAPID_FIELD );
		
		{	//フェード用に元データを確保
			NNSG2dPaletteData *data;
			data = CLACT_U_ResManagerGetResObjResPltt(
					set->clact_resobj_pltt[0] );
			{
				int i = 0;
				TW_PLTT_WORK *tw_pltt = &tw->pltt;
				u16 *copy_p = tw_pltt->clact_cloud_plttbase;
				u16 *base_p = data->pRawData;
				
				do{
					*copy_p = *base_p;
					copy_p++;
					base_p++;
				}while( ++i < (16*TW_CLACT_RES_PLTT_CLOUD_NUM) );
			}
		}
		
		CLACT_U_PlttManagerSetCleanArea( set->clact_resobj_pltt[0] );
		#endif
		
		for( i = 0; i < TW_CLACT_RES_CELL_MAX; i++ ){
			set->clact_resobj_cell[i] =
				CLACT_U_ResManagerResAddArcKindCell_ArcHandle( 
					set->clact_resm_cell, hd,
					DATA_TwClActDataArcIdxTbl_Cell[i], FALSE,
					CLACTRESID(i),
					CLACT_U_CELL_RES, HEAPID_FIELD );
		}
		
		for( i = 0; i < TW_CLACT_RES_ANM_MAX; i++ ){
			set->clact_resobj_anm[i] =
				CLACT_U_ResManagerResAddArcKindCell_ArcHandle( 
					set->clact_resm_anm, hd,
					DATA_TwClActDataArcIdxTbl_Anm[i], FALSE,
					CLACTRESID(i),
					CLACT_U_CELLANM_RES, HEAPID_FIELD );
		}
	}
	
	{	//描画用TCB
		set->tcb_clact_draw = TCB_Add(
			tw_ClActDrawTcb, set, TW_TCBPRI_CLACT_DRAW );
	}
}

//--------------------------------------------------------------
/**
 * やぶれたせかい　セルアクターセット削除
 * @param		set	TW_CLACT_SETDATA 
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ClActDelete( TW_CLACT_SETDATA *set )
{
	int i;
	
	TCB_Delete( set->tcb_clact_draw );
	
	for( i = 0; i < TW_CLACT_RES_CHAR_MAX; i++ ){
		if( set->clact_resobj_char[i] != NULL ){
			CLACT_U_CharManagerDelete( set->clact_resobj_char[i] );
		}
	}
	
	for( i = 0; i < TW_CLACT_RES_PLTT_MAX; i++ ){
		if( set->clact_resobj_pltt[i] != NULL ){
			CLACT_U_PlttManagerDelete( set->clact_resobj_pltt[i] );
		}
	}
	
	for( i = 0; i < TW_CLACT_RES_CELL_MAX; i++ ){
		if( set->clact_resobj_cell[i] != NULL ){
			CLACT_U_ResManagerResOnlyDelete( set->clact_resobj_cell[i] );
			CLACT_U_ResManagerResDelete(
				set->clact_resm_cell, set->clact_resobj_cell[i] );
		}
	}
	
	for( i = 0; i < TW_CLACT_RES_ANM_MAX; i++ ){
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
	
	CLACT_DelAllSet( set->clact_set );
	CLACT_DestSet( set->clact_set );
}

//--------------------------------------------------------------
/**
 * やぶれたせかい　セルアクター描画
 * @param	tcb	TCB_PTR
 * @param	wk	TORNWORLD_WORK
 * @retval	mnothing
 */
//--------------------------------------------------------------
static void tw_ClActDrawTcb( TCB_PTR tcb, void *wk )
{
	TW_CLACT_SETDATA *set = wk;
	CLACT_Draw( set->clact_set );
}

//--------------------------------------------------------------
/**
 * やぶれたせかい　セルアクター追加
 * @param	set	TW_CLACT_SETDATA
 * @param	pos	VecFx32
 * @param	char_id
 * @param	pltt_id
 * @param	cell_id
 * @param	anm_id
 * @param	bg_pri
 * @param	act_pri
 * @retval	CLACT_WORK_PTR CLACT_WORK_PTR
 */
//--------------------------------------------------------------
static CLACT_WORK_PTR tw_ClActAdd(
		TW_CLACT_SETDATA *set, const VecFx32 *pos,
		u32 char_id, u32 pltt_id, u32 cell_id, u32 anm_id,
		int bg_pri, int act_pri )
{
	CLACT_HEADER head;
	CLACT_ADD_SIMPLE add;
	CLACT_WORK_PTR act;
	
	CLACT_U_MakeHeader(
		&head,
		CLACTRESID(char_id), CLACTRESID(pltt_id),
		CLACTRESID(cell_id), CLACTRESID(anm_id),
		CLACT_U_HEADER_DATA_NONE, CLACT_U_HEADER_DATA_NONE, FALSE, bg_pri,
		set->clact_resm_char, set->clact_resm_pltt,
		set->clact_resm_cell, set->clact_resm_anm,
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

//==============================================================================
//	EOA	セルアクター　雲
//==============================================================================
//--------------------------------------------------------------
/**
 * 雲セット
 * @param	fes	FE_SYS *fes
 * @param	set	TW_CLACT_SETDATA
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_EoaCloudSet(
	TORNWORLD_WORK *tw, FE_SYS *fes, TW_CLACT_SETDATA *set )
{
	int i;
	EOA_CLOUD_H head;
	EOA_PTR eoa;
	
	//雲速度決定
	tw->cloud_speed_lv = CLOUD_SPEED_LV_0;
	
	if( twNowZoneID(tw) == ZONE_ID_D34R0110 ){
		EVENTWORK *ev = SaveData_GetEventWork( tw->fsys->savedata );
		u32 ev_seq_no = SysWork_TornWorldEventSeqNoGet( ev );
		
		if( ev_seq_no >= TWEVSEQ_08_02 ){
			if( ev_seq_no <= TWEVSEQ_08D_01 ){
				tw->cloud_speed_lv = CLOUD_SPEED_LV_1;
			}else if( ev_seq_no <= TWEVSEQ_08D_02 ){
				tw->cloud_speed_lv = CLOUD_SPEED_LV_2;
			}
		}
	}
	
	head.tw = tw;
	head.set = set;
	
	for( i = 0; i < CLOUD_SET_MAX; i++ ){
		head.data = DATA_EoaCloudInitDataTbl[i];
		eoa = FE_EoaAddNpp( fes, &DATA_EoaCloudHeader,
			NULL, 0, &head, 0 );
	}
}

//--------------------------------------------------------------
/**
 * EOA 雲　初期化
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work
 * @retval	int TRUE=初期化完了
 */
//--------------------------------------------------------------
static int tw_EoaCloudInit( EOA_PTR eoa, void *wk )
{
	VecFx32 pos = {0,0,0};
	const CLACT_RESID *res_id;
	const EOA_CLOUD_H *head = EOA_AddPtrGet( eoa );
	EOA_CLOUD_WORK *work = wk;
	
	work->head = *head;
	work->angle = work->head.data.angle;
	work->add_speed = DATA_CloudAddSpeedTbl[
		head->tw->cloud_speed_lv][head->data.speed_type];
	
	res_id = &DATA_CloudResIDTbl[work->head.data.type];
	work->clact = tw_ClActAdd(
		work->head.set, &pos,
		res_id->char_id, res_id->pltt_id,
		res_id->cell_id, res_id->anm_id,
		TW_BGPRI_CLOUD, 0xffff );
	CLACT_SetScaleAffine( work->clact,
		&work->head.data.scale, CLACT_AFFINE_DOUBLE );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * EOA 雲　削除
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_EoaCloudDelete( EOA_PTR eoa, void *wk )
{
	EOA_CLOUD_WORK *work = wk;
	CLACT_Delete( work->clact );
}

//--------------------------------------------------------------
/**
 * EOA 雲　動作
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_EoaCloudMove( EOA_PTR eoa, void *wk )
{
	fx32 angle,speed,add_speed;
	VecFx32 pos;
	EOA_CLOUD_WORK *work = wk;
	
	add_speed = DATA_CloudAddSpeedTbl[
		work->head.tw->cloud_speed_lv][work->head.data.speed_type];
	
	if( add_speed < work->add_speed ){
		work->add_speed -= 0x200;
		if( add_speed > work->add_speed ){
			work->add_speed = add_speed;
		}
	}else if( add_speed > work->add_speed ){
		work->add_speed += 0x200;
		if( add_speed < work->add_speed ){
			work->add_speed = add_speed;
		}
	}
	
	#ifdef DEBUG_TWORLD_CAPTURE
	if( DebugTWorldCaptureFlag_CloudStop == FALSE ){
		speed = work->head.data.speed + work->add_speed;
		twAngleAdd360Fx( &work->angle, speed );
	}
	
	angle = work->angle;
	twAngleAdd360Fx( &angle, work->head.data.offs_angle );
	
	pos = work->head.data.c_pos;
	pos.x += TW_CLACT_SFACE_X_FX +
		(Cos360( FX32_NUM(work->angle) ) * work->head.data.radius);
	pos.y += TW_CLACT_SFACE_Y_FX +
		(Sin360( FX32_NUM(work->angle) ) * work->head.data.radius);
	EOA_MatrixSet( eoa, &pos );
	CLACT_SetMatrix( work->clact, &pos );
	CLACT_SetRotation( work->clact, RotKeyR(FX32_NUM(angle)) );
	
	{
		u8 flag = TRUE;
		if( DebugTWorldCaptureFlag_BGVanish ){ flag = FALSE; }
		CLACT_SetDrawFlag( work->clact, flag );
	}
	#else	//正規
	speed = work->head.data.speed + work->add_speed;
	twAngleAdd360Fx( &work->angle, speed );
	angle = work->angle;
	twAngleAdd360Fx( &angle, work->head.data.offs_angle );
	
	pos = work->head.data.c_pos;
	pos.x += TW_CLACT_SFACE_X_FX +
		(Cos360( FX32_NUM(work->angle) ) * work->head.data.radius);
	pos.y += TW_CLACT_SFACE_Y_FX +
		(Sin360( FX32_NUM(work->angle) ) * work->head.data.radius);
	EOA_MatrixSet( eoa, &pos );
	CLACT_SetMatrix( work->clact, &pos );
	CLACT_SetRotation( work->clact, RotKeyR(FX32_NUM(angle)) );
	#endif
}

//--------------------------------------------------------------
/**
 * EOA 雲　描画
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_EoaCloudDraw( EOA_PTR eoa, void *wk )
{
}

//--------------------------------------------------------------
///	雲 EOA_H
//--------------------------------------------------------------
static const EOA_H_NPP DATA_EoaCloudHeader =
{
	sizeof(EOA_CLOUD_WORK),
	tw_EoaCloudInit,
	tw_EoaCloudDelete,
	tw_EoaCloudMove,
	tw_EoaCloudDraw,
};

//==============================================================================
//	フォグ
//==============================================================================
//--------------------------------------------------------------
/**
 * フォグ初期化
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_FogInit( TORNWORLD_WORK *tw )
{
#ifndef DEBUG_TW_FOG_OFF
	FOG_DATA_PTR fog = tw->fsys->fog_data;
	
	SetFogData( fog,				//フォグデータセット
		FOG_SYS_ALL,				//全てに
		TRUE,						//FOG有効
		GX_FOGBLEND_COLOR_ALPHA,	//半透明タイプ
		TW_FOG_SLOPE,				//掛かり具合
		TW_FOG_OFFS );				//掛かる位置
	
	SetFogColor( fog, FOG_SYS_ALL, TW_FOG_COLOR, TW_FOG_ALPHA );
	
	{
		int i = 0;
		char fog_tbl[32];
		
		do{
			fog_tbl[i] = (TW_FOGTBL_BLDMAX/32) * i;
			i++;
		}while( i < 32 );
		
		SetFogTbl( fog, fog_tbl );
	}
#endif
}

//--------------------------------------------------------------
/**
 * フォグ削除
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_FogDelete( TORNWORLD_WORK *tw )
{
}

//==============================================================================
//	飛び石
//==============================================================================
//--------------------------------------------------------------
/**
 * 飛び石　初期化　メイン
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_FStoneInitMain( TORNWORLD_WORK *tw,
	TW_FSTONE_WORK *fstone, const TW_FSTONE_HEADER *head,
	const TW_FSTONE_PUTPOS *putpos, int zone_id, u32 vanish_flag )
{
	{
		GF_ASSERT( fstone->eoa_tbl_p == NULL );
	}
	
	{
		int size;
		memset( fstone, 0, sizeof(TW_FSTONE_WORK) );
		
		fstone->vanish_flag = vanish_flag;
		fstone->put_max = head->put_max;
		GF_ASSERT( fstone->put_max );
		
		size = fstone->put_max * sizeof(EOA_PTR);
		fstone->eoa_tbl_p = sys_AllocMemoryLo( HEAPID_FIELD, size );
		memset( (u8*)fstone->eoa_tbl_p, 0, size );
		
		tw_FStonePut( tw, fstone, putpos, zone_id );
	}
}

/*
init_flag   = 00000000100000111111111100000000
vanish_flag = 11111111011111000000000011111111
*/

//--------------------------------------------------------------
/**
 * 飛び石　初期化　現在層
 * @param	tw		TORNWORLD_WORK
 * @param	init_vns TRUE=フラグ初期値で初期化 FALSE=セーブデータチェック
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_FStoneInitNow( TORNWORLD_WORK *tw, int init_vns )
{
	int zone_id;
	const TW_ZONE_LINKDATA *link;
	
	zone_id = twNowZoneID( tw );
	link = tw_ZoneLinkDataGet( zone_id );
	
	if( tw_ZoneDataBufFStoneDataSizeGet(tw) ){			//メイン
		TW_FSTONE_WORK *fstone = &tw->fstone;
		const TW_FSTONE_HEADER *head = tw_MapDataFStoneHeaderGet( tw );
		const TW_FSTONE_PUTPOS *put = tw_MapDataFStonePutPosTblGet( tw );
		
		if( init_vns == TRUE || tw_SaveDataInitFinishCheck(tw) == FALSE ){
			fstone->vanish_flag = ~head->init_flag;
			tw_SaveDataFStoneFlagSet( tw, fstone->vanish_flag );
			#ifdef DEBUG_TW_PRINTF
			{
				int i;
				u32 flag = head->init_flag;
				OS_Printf( "head->init_flag   = " );
				for( i = 0; i < 32; i++, flag <<= 1 ){
					if( flag & 0x80000000 ){
						OS_Printf( "1" );
					}else{
						OS_Printf( "0" );
					}
				}
				OS_Printf( "\n" );
				flag = fstone->vanish_flag;
				OS_Printf( "fstone->vanish_flag = " );
				for( i = 0; i < 32; i++, flag <<= 1 ){
					if( flag & 0x80000000 ){
						OS_Printf( "1" );
					}else{
						OS_Printf( "0" );
					}
				}
				OS_Printf( "\n" );
			}
			#endif
		}else{
			fstone->vanish_flag = tw_SaveDataFStoneFlagGet( tw );
		}
		
		tw_FStoneInitMain(
			tw, fstone, head, put, zone_id, fstone->vanish_flag );
		
		#ifdef DEBUG_TW_PRINTF
		OS_Printf( "TW FStone main PUT MAX %d\n", head->put_max );
		#endif
	}
}

//--------------------------------------------------------------
/**
 * 飛び石　初期化　下層
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_FStoneInitLower( TORNWORLD_WORK *tw )
{
	int zone_id;
	const TW_ZONE_LINKDATA *link;
	
	zone_id = twNowZoneID( tw );
	link = tw_ZoneLinkDataGet( zone_id );
	
	zone_id = link->lower_zone_id;
	
	if( tw_ZoneDataBufFStoneDataSizeGetLower(tw) ){		//下層
		TW_FSTONE_WORK *fstone = &tw->fstone_lower;
		const TW_FSTONE_HEADER *head = tw_MapDataFStoneHeaderGetLower( tw );
		const TW_FSTONE_PUTPOS *put = tw_MapDataFStonePutPosTblGetLower( tw );
		tw_FStoneInitMain( tw, fstone, head, put, zone_id, ~head->init_flag );
		
		#ifdef DEBUG_TW_PRINTF
		OS_Printf( "TW FStone lower PUT MAX %d\n", head->put_max );
		#endif
	}
}


//--------------------------------------------------------------
/**
 * 飛び石　初期化
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_FStoneInit( TORNWORLD_WORK *tw )
{
	tw_FStoneInitNow( tw, FALSE );
	tw_FStoneInitLower( tw );
}

#if 0
static void tw_FStoneInit( TORNWORLD_WORK *tw )
{
	int zone_id;
	const TW_ZONE_LINKDATA *link;
	
	zone_id = twNowZoneID( tw );
	link = tw_ZoneLinkDataGet( zone_id );
	
	if( tw_ZoneDataBufFStoneDataSizeGet(tw) ){			//メイン
		TW_FSTONE_WORK *fstone = &tw->fstone;
		const TW_FSTONE_HEADER *head = tw_MapDataFStoneHeaderGet( tw );
		const TW_FSTONE_PUTPOS *put = tw_MapDataFStonePutPosTblGet( tw );
		
		if( tw_SaveDataInitFinishCheck(tw) == FALSE ){
			fstone->vanish_flag = ~head->init_flag;
			tw_SaveDataFStoneFlagSet( tw, fstone->vanish_flag );
		}else{
			fstone->vanish_flag = tw_SaveDataFStoneFlagGet( tw );
		}
		
		tw_FStoneInitMain( tw, fstone, head, put, zone_id );
		
		#ifdef DEBUG_TW_PRINTF
		OS_Printf( "TW FStone main PUT MAX %d\n", head->put_max );
		#endif
	}
	
	zone_id = link->lower_zone_id;
	
	if( tw_ZoneDataBufFStoneDataSizeGetLower(tw) ){		//下層
		TW_FSTONE_WORK *fstone = &tw->fstone_lower;
		const TW_FSTONE_HEADER *head = tw_MapDataFStoneHeaderGetLower( tw );
		const TW_FSTONE_PUTPOS *put = tw_MapDataFStonePutPosTblGetLower( tw );
		
		fstone->vanish_flag = ~head->init_flag;
		tw_FStoneInitMain( tw, fstone, head, put, zone_id );
		
		#ifdef DEBUG_TW_PRINTF
		OS_Printf( "TW FStone lower PUT MAX %d\n", head->put_max );
		#endif
	}
}
#endif

//--------------------------------------------------------------
/**
 * 飛び石　削除　メイン
 * @param	fstone	TW_FSTONE_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_FStoneDeleteMain( TW_FSTONE_WORK *fstone )
{
	int i;
	
	if( fstone->put_max ){
		for( i = 0; i < fstone->put_max; i++ ){
			if( fstone->eoa_tbl_p[i] != NULL ){
				EOA_Delete( fstone->eoa_tbl_p[i] );
			}
		}
		
		sys_FreeMemoryEz( fstone->eoa_tbl_p );
		fstone->eoa_tbl_p = NULL;
		fstone->put_max = 0;
	}
}

//--------------------------------------------------------------
/**
 * 飛び石　削除
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_FStoneDelete( TORNWORLD_WORK *tw )
{
	tw_FStoneDeleteNow( tw );
	tw_FStoneDeleteLower( tw );
}

//--------------------------------------------------------------
/**
 * 飛び石　現在のみ削除
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_FStoneDeleteNow( TORNWORLD_WORK *tw )
{
	#ifdef DEBUG_TW_PRINTF
	OS_Printf( "TW FSTONE DELETE NOW\n" );
	#endif
	tw_FStoneDeleteMain( &tw->fstone );
}

//--------------------------------------------------------------
/**
 * 飛び石　下層のみ削除
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_FStoneDeleteLower( TORNWORLD_WORK *tw )
{
	#ifdef DEBUG_TW_PRINTF
	OS_Printf( "TW FSTONE DELETE LOWER\n" );
	#endif
	tw_FStoneDeleteMain( &tw->fstone_lower );
}

//--------------------------------------------------------------
/**
 * 飛び石　現在層のみ消去
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_FStoneCleanNow( TORNWORLD_WORK *tw )
{
	tw->fstone.eoa_tbl_p = NULL;
	tw->fstone.put_max = 0;
}

//--------------------------------------------------------------
/**
 * 飛び石　下層のみ消去
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_FStoneCleanLower( TORNWORLD_WORK *tw )
{
	tw->fstone_lower.eoa_tbl_p = NULL;
	tw->fstone_lower.put_max = 0;
}

//--------------------------------------------------------------
/**
 * 飛び石　現在情報を下層へ
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_FStoneRemoveNowLower( TORNWORLD_WORK *tw )
{
	GF_ASSERT( tw->fstone_lower.eoa_tbl_p == NULL );
	GF_ASSERT( tw->fstone_lower.put_max == 0 );
	tw->fstone_lower = tw->fstone;
}

//--------------------------------------------------------------
/**
 * 飛び石　下層情報を現在に
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_FStoneRemoveLowerNow( TORNWORLD_WORK *tw )
{
	GF_ASSERT( tw->fstone.eoa_tbl_p == NULL );
	GF_ASSERT( tw->fstone.put_max == 0 );
	tw->fstone = tw->fstone_lower;
}

#if 0
static void tw_FStoneDelete( TORNWORLD_WORK *tw )
{
	int i;
	TW_FSTONE_WORK *fstone = &tw->fstone;
	
	tw_FStoneDeleteMain( tw->fstone );
	if( fstone->put_max ){
		for( i = 0; i < fstone->put_max; i++ ){
			if( fstone->eoa_tbl_p[i] != NULL ){
				EOA_Delete( fstone->eoa_tbl_p[i] );
			}
		}
		
		sys_FreeMemoryEz( fstone->eoa_tbl_p );
		fstone->eoa_tbl_p = NULL;
	}
	
#if 0	
	for( i = 0; i < FSTONETYPE_MAX; i++ ){
		tw_FStoneTypeEras( tw, i );
	}
#endif
}
#endif

//--------------------------------------------------------------
/**
 * 飛び石　バニッシュフラグセット
 * @param	tw	TORNWORLD_WORK
 * @param	no 0-15
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_FStoneVanishFlagON( TORNWORLD_WORK *tw, u32 no )
{
	TW_FSTONE_WORK *fstone = &tw->fstone;
	GF_ASSERT( no < FSTONE_FLAGNO_MAX );
	fstone->vanish_flag |= 1 << no;
	tw_SaveDataFStoneFlagSet( tw, fstone->vanish_flag );
}

//--------------------------------------------------------------
/**
 * 飛び石　バニッシュフラグOFF
 * @param	tw	TORNWORLD_WORK
 * @param	no
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_FStoneVanishFlagOFF( TORNWORLD_WORK *tw, u32 no )
{
//	u32 bit = 0xffffffff ^ (1 << no);
	u32 bit = ~(1 << no);
	TW_FSTONE_WORK *fstone = &tw->fstone;
	GF_ASSERT( no < FSTONE_FLAGNO_MAX );
	fstone->vanish_flag &= bit;
	tw_SaveDataFStoneFlagSet( tw, fstone->vanish_flag );
}

//--------------------------------------------------------------
/**
 * 飛び石　バニッシュフラグチェック
 * @param	tw	TORNWORLD_WORK
 * @param	no
 * @retval	nothing
 */
//--------------------------------------------------------------
static BOOL tw_FStoneVanishFlagCheck( TORNWORLD_WORK *tw, u32 no )
{
	TW_FSTONE_WORK *fstone = &tw->fstone;
	GF_ASSERT( no < FSTONE_FLAGNO_MAX );
	if( (fstone->vanish_flag & (1<<no)) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * 飛び石　バニッシュフラグチェック　下層
 * @param	tw	TORNWORLD_WORK
 * @param	no
 * @retval	nothing
 */
//--------------------------------------------------------------
static BOOL tw_FStoneVanishFlagCheckLower( TORNWORLD_WORK *tw, u32 no )
{
	TW_FSTONE_WORK *fstone = &tw->fstone_lower;
	GF_ASSERT( no < FSTONE_FLAGNO_MAX );
	if( (fstone->vanish_flag & (1<<no)) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * 飛び石　現在のバニッシュフラグをセーブデータに反映
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_FStoneVanishFlagSaveDataSet( TORNWORLD_WORK *tw )
{
	TW_FSTONE_WORK *fstone = &tw->fstone;
	tw_SaveDataFStoneFlagSet( tw, fstone->vanish_flag );
}

//--------------------------------------------------------------
/**
 * 飛び石　配置
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_FStonePut( TORNWORLD_WORK *tw,
	TW_FSTONE_WORK *fstone, const TW_FSTONE_PUTPOS *put, int zone_id )
{
	if( fstone->put_max ){
		int i;
		for( i = 0; i < fstone->put_max; i++, put++ ){
			#ifdef DEBUG_TW_PRINTF
			{
				/*
				u32 free = sys_GetHeapFreeSize( HEAPID_FIELD );
				OS_Printf( "飛石No.%d 配置開始 FldFreeSize %xH\n", i, free );
				*/
			}
			#endif
			
			fstone->eoa_tbl_p[i] = tw_FStoneAdd( tw, i, zone_id, put );
			
			#ifdef DEBUG_TW_PRINTF
			{
				/*
				u32 free = sys_GetHeapFreeSize( HEAPID_FIELD );
				OS_Printf( "配置完了 FldFreeSize %xH\n", free );
				*/
			}
			#endif
		}
	}
}

#if 0
static void tw_FStonePut( TORNWORLD_WORK *tw )
{
	TW_FSTONE_WORK *fstone = &tw->fstone;
	
	if( fstone->put_max ){
		int i;
		const TW_FSTONE_PUTPOS *put = tw_MapDataFStonePutPosTblGet( tw );
		for( i = 0; i < fstone->put_max; i++, put++ ){
			tw_FStoneAdd( tw, i, put );
		}
	}
}
#endif

//--------------------------------------------------------------
/**
 * 飛び石　追加
 * @param	tw		TORNWORLD_WORK
 * @param	no		飛び石番号
 * @param	flag	フラグ
 * @param	type	TWOBJTYPE
 * @param	gx		グリッド座標x
 * @param	gy		グリッド座標y
 * @param	gz		グリッド座標z
 * @retval	nothing
 */
//--------------------------------------------------------------
static EOA_PTR tw_FStoneAdd(
		TORNWORLD_WORK *tw, int no, int zone_id,
		const TW_FSTONE_PUTPOS *pos )
{
	EOA_PTR eoa;
	EOA_FSTONE_HEAD head;
	TW_FSTONE_WORK *fstone = &tw->fstone;
	const EOA_H_NPP *eoa_h = DATA_EoaOBJTypeHeaderTbl[pos->type];
	head.no = no;
	head.zone_id = zone_id;
	head.putpos = *pos;
	head.tw = tw;
	
	eoa = FE_EoaAddNpp(
		tw->fsys->fes, eoa_h, NULL, 0, &head, TW_TCBPRI_STONE );
	return( eoa );
}

#if 0
static void tw_FStoneAdd(
		TORNWORLD_WORK *tw, int no, const TW_FSTONE_PUTPOS *pos )
{
	EOA_FSTONE_HEAD head;
	TW_FSTONE_WORK *fstone = &tw->fstone;
	const EOA_H_NPP *eoa_h = DATA_EoaOBJTypeHeaderTbl[pos->type];
	head.no = no;
	head.zone_id = twNowZoneID( tw );
	head.putpos = *pos;
	head.tw = tw;
	fstone->eoa_tbl_p[no] = FE_EoaAddNpp( tw->fsys->fes,
		eoa_h, NULL, 0, &head, TW_TCBPRI_STONE );
}
#endif

//--------------------------------------------------------------
/**
 * 飛び石　出現イベントチェック
 * @param	tw	TORNWORLD_WORK
 * @param	gx	自機グリッドX
 * @param	gy	自機グリッドY
 * @param	gz	自機グリッドZ
 * @param	dir	自機進行方向 DIR_UP等
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_EventMoveCheck_FStoneAdvent(
	TORNWORLD_WORK *tw, int gx, int gy, int gz, int dir )
{
	if( tw_ZoneDataBufFStoneDataSizeGet(tw) ){
		const TW_FSTONE_HEADER *head = tw_MapDataFStoneHeaderGet( tw );
		
		if( head->flagpos_max ){
			int max = head->flagpos_max;
			const TW_FSTONE_FLAGPOS *flag;
			flag = tw_MapDataFStoneFlagPutTblGet( tw );
			
			do{
				if( dir == flag->dir ){
					if( twGridRectCheck(gx,gy,gz,&flag->rect_size) ){
						if( flag->on_off == FALSE ){ //draw flag FALSE=OFF
							tw_FStoneVanishFlagON( tw, flag->flag_no );
						}else{
							tw_FStoneVanishFlagOFF( tw, flag->flag_no );
						}
					}
				}
				flag++;
			}while( --max );
		}
	}
}

#if 0
static void tw_EventMoveCheck_FStoneAdvent(
	TORNWORLD_WORK *tw, int gx, int gy, int gz, int dir )
{
	if( tw_ZoneDataBufFStoneDataSizeGet(tw) ){
		const TW_FSTONE_HEADER *head = tw_MapDataFStoneHeaderGet( tw );
		if( head->flagpos_max ){
			int i;
			const TW_FSTONE_FLAGPOS *flag;
			flag = tw_MapDataFStoneFlagPutTblGet( tw );
			for( i = 0; i < head->flagpos_max; i++, flag++ ){
				if( dir == flag->dir ){
					if( twGridRectCheck(gx,gy,gz,&flag->rect_size) ){
						if( flag->on_off == FALSE ){ //draw flag FALSE=OFF
							tw_FStoneVanishFlagON( tw, flag->flag_no );
						}else{
							tw_FStoneVanishFlagOFF( tw, flag->flag_no );
						}
					}
				}
			}
		}
	}
}
#endif

#if 0
//--------------------------------------------------------------
/**
 * 飛び石　指定座標で描画
 * @param	tw	TORNWORLD_WORK
 * @param	type	FSTONETYPE_MAX
 * @param	pos		座標
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_FStoneDraw( TORNWORLD_WORK *tw, int type, VecFx32 *pos )
{
	TW_FSTONE_WORK *fstone = &tw->fstone;
	FRO_OBJ_DrawPos( &fstone->robj[type], pos );
}
#endif

//--------------------------------------------------------------
/**
 * 飛び石　存在している飛び石が使用しているモデルリソースIDを検索
 * @param	tw	TORNWORLD_WORK
 * @param	mdl_id	TW_MDL_MAX
 * @retval	BOOL	TRUE=存在している
 */
//--------------------------------------------------------------
static BOOL tw_FStoneOBJResMdlIDSearch( TORNWORLD_WORK *tw, u32 mdl_id )
{
	int i;
	u16 type;
	const TW_OBJRES_ID *res_id;
	TW_FSTONE_WORK *fstone = &tw->fstone;
	
	GF_ASSERT( mdl_id != TW_MDL_MAX );
	
	for( i = 0; i < fstone->put_max; i++ ){
		if( EOA_StatusBit_UseCheck(fstone->eoa_tbl_p[i]) == TRUE ){
			type = tw_EoaFStoneTypeGet( fstone->eoa_tbl_p[i] );
			res_id = &DATA_OBJResIDTbl[type];
			
			if( mdl_id == res_id->mdl_id ){
				return( TRUE );
			}
		}
	}
	
	fstone = &tw->fstone_lower;
	
	for( i = 0; i < fstone->put_max; i++ ){
		if( EOA_StatusBit_UseCheck(fstone->eoa_tbl_p[i]) == TRUE ){
			type = tw_EoaFStoneTypeGet( fstone->eoa_tbl_p[i] );
			res_id = &DATA_OBJResIDTbl[type];
			
			if( mdl_id == res_id->mdl_id ){
				return( TRUE );
			}
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * 飛び石　存在している飛び石が使用しているアニメリソースIDを検索
 * @param	tw	TORNWORLD_WORK
 * @param	anm_id	TW_ANM_MAX
 * @retval	BOOL	TRUE=存在している
 */
//--------------------------------------------------------------
static BOOL tw_FStoneOBJResAnmIDSearch( TORNWORLD_WORK *tw, u32 anm_id )
{
	int i;
	u16 type;
	const TW_OBJRES_ID *res_id;
	TW_FSTONE_WORK *fstone = &tw->fstone;
	GF_ASSERT( anm_id != TW_ANM_MAX );
	
	for( i = 0; i < fstone->put_max; i++ ){
		if( EOA_StatusBit_UseCheck(fstone->eoa_tbl_p[i]) == TRUE ){
			type = tw_EoaFStoneTypeGet( fstone->eoa_tbl_p[i] );
			res_id = &DATA_OBJResIDTbl[type];
			
			if( anm_id == res_id->anm_id ){
				return( TRUE );
			}
		}
	}
	
	fstone = &tw->fstone_lower;
	
	for( i = 0; i < fstone->put_max; i++ ){
		if( EOA_StatusBit_UseCheck(fstone->eoa_tbl_p[i]) == TRUE ){
			type = tw_EoaFStoneTypeGet( fstone->eoa_tbl_p[i] );
			res_id = &DATA_OBJResIDTbl[type];
			
			if( anm_id == res_id->anm_id ){
				return( TRUE );
			}
		}
	}
	
	return( FALSE );
}

#if 0
//--------------------------------------------------------------
/**
 * 飛び石　存在している飛び石が使用しているリソースIDを検索
 * @param	tw	TORNWORLD_WORK
 * @param	mdl_id	TW_MDL_MAX
 * @param	anm_id	TW_ANM_MAX
 * @retval	BOOL	TRUE=存在している
 */
//--------------------------------------------------------------
static BOOL tw_FStoneOBJResIDSearch(
	TORNWORLD_WORK *tw, u32 mdl_id, u32 anm_id )
{
	int i;
	u16 type;
	const TW_OBJRES_ID *res_id;
	TW_FSTONE_WORK *fstone = &tw->fstone;
	EOA_PTR *tbl = fstone->eoa_tbl_p;
	
	for( i = 0; i < fstone->put_max; i++, tbl++ ){
		if( EOA_StatusBit_UseCheck(*tbl) == TRUE ){
			type = tw_EoaFStoneTypeGet( *tbl );
			res_id = &DATA_OBJResIDTbl[type];
			
			if( mdl_id != TW_MDL_MAX && mdl_id == res_id->mdl_id ){
				return( TRUE );
			}
			
			if( anm_id != TW_ANM_MAX && anm_id == res_id->anm_id ){
				return( TRUE );
			}
		}
	}
	
	return( FALSE );
}
#endif

//--------------------------------------------------------------
/**
 * 飛び石　存在している飛び石のOBJTYPEを検索
 * @param	tw	TORNWORLD_WORK
 * @param	type	チェックするTWOBJTYPE
 * @retval	BOOL	TRUE=存在
 */
//--------------------------------------------------------------
static BOOL tw_FStoneOBJResTypeSearch( TORNWORLD_WORK *tw, TWOBJTYPE c_type )
{
	int i;
	u16 type;
	const TW_OBJRES_ID *res_id;
	TW_FSTONE_WORK *fstone = &tw->fstone;
	
	for( i = 0; i < fstone->put_max; i++ ){
		if( EOA_StatusBit_UseCheck(fstone->eoa_tbl_p[i]) == TRUE ){
			type = tw_EoaFStoneTypeGet( fstone->eoa_tbl_p[i] );
			if( type == c_type ){
				return( TRUE );
			}
		}
	}
	
	fstone = &tw->fstone_lower;
	
	for( i = 0; i < fstone->put_max; i++ ){
		if( EOA_StatusBit_UseCheck(fstone->eoa_tbl_p[i]) == TRUE ){
			type = tw_EoaFStoneTypeGet( fstone->eoa_tbl_p[i] );
			if( type == c_type ){
				return( TRUE );
			}
		}
	}
	
	return( FALSE );
}

//==============================================================================
//	飛び石　EOA 共通
//==============================================================================
//--------------------------------------------------------------
/**
 * EOA 飛び石　種類を取得
 * @param	eoa	EOA_PTR
 * @retval	u16 FSTONETYPE_MAX等
 */
//--------------------------------------------------------------
static u16 tw_EoaFStoneTypeGet( EOA_PTR eoa )
{
	return( (u16)EOA_AnswerGet(eoa) );
}

//==============================================================================
//	飛び石　EOA
//==============================================================================
#define FSTONE_OFFS_TBL_MAX (8)
#define FSTONE_OFFS_TBL_MAX_FX (NUM_FX32(FSTONE_OFFS_TBL_MAX))
#define FSTONE_OFFS_SPEED_FX (0x0800)

//--------------------------------------------------------------
///	EOA_FSTONE_WORK
//--------------------------------------------------------------
typedef struct
{
	s8 alpha;
	s8 alpha_value;
	u16 culling;
	fx32 offs_no;
	fx32 offs_val;
	fx32 offs_y_fx;
	VecFx32 pos;
	VecFx32 draw_pos;
	EOA_FSTONE_HEAD head;
	TW_OBJRES *objres_p;
}EOA_FSTONE_WORK;

//--------------------------------------------------------------
/**
 * EOA 飛び石　初期化
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work
 * @retval	int	FALSE=初期化失敗
 */
//--------------------------------------------------------------
static int tw_EoaFStoneInit( EOA_PTR eoa, void *wk )
{
	int init;
	TW_FSTONE_PUTPOS *put;
	EOA_FSTONE_WORK *work = wk;
	const EOA_FSTONE_HEAD *head = EOA_AddPtrGet( eoa );
	
	work->head = *head;
	put = &work->head.putpos;
	
	EOA_AnswerSet( eoa, put->type );
	
	work->objres_p = tw_OBJResDataOBJResGet( head->tw, put->type, &init );
	
	if( init == FALSE ){
		tw_OBJResDataOBJTypeInit(
			work->head.tw, put->type,
			&work->objres_p->robj, &work->objres_p->ranm );
	}
	
	{
		BOOL ret;
		
		if( twNowZoneID(head->tw) == head->zone_id ){
			ret = tw_FStoneVanishFlagCheck( head->tw, put->flag_no );
		}else{
			ret = tw_FStoneVanishFlagCheckLower( head->tw, put->flag_no );
		}
		
		if( ret == TRUE ){
			work->alpha = 0;
		}else{	//表示
			work->alpha = 31;
		}
	}
	
	FieldOBJTool_GridCenterPosGet( put->gx, put->gz, &work->pos );
	work->pos.y = GRID_SIZE_FX32( put->gy ) + GRID_HALF_FX32;
	
	{
		VecFx32 offs;
		const VecFx32 *d_offs = &DATA_OBJTypePosOffsTbl[put->type];
		work->pos.x += d_offs->x;
		work->pos.y += d_offs->y;
		work->pos.z += d_offs->z;
	}
	
	work->offs_no = gf_rand() % FSTONE_OFFS_TBL_MAX_FX;
	work->offs_val = FSTONE_OFFS_SPEED_FX;
	if( (work->offs_no&0x01) ){ work->offs_val = -work->offs_val; }
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * EOA 飛び石　削除
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work
 * @retval	void
 */
//--------------------------------------------------------------
static void tw_EoaFStoneDelete( EOA_PTR eoa, void *wk )
{
	EOA_FSTONE_WORK *work = wk;
	EOA_FSTONE_HEAD *head = &work->head;
	
	if( tw_OBJResDataTypeAnmCheck(head->putpos.type) == TRUE ){
		FRO_ANM_DeleteAll( &work->objres_p->ranm );
	}
	
	tw_OBJResDataOBJResCheckRelease( head->tw, work->objres_p );
}

//--------------------------------------------------------------
/**
 * EOA 飛び石　動作　ゆれなし
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work
 * @retval	void
 */
//--------------------------------------------------------------
static void tw_EoaFStoneMove( EOA_PTR eoa, void *wk )
{
	EOA_FSTONE_WORK *work = wk;
	EOA_FSTONE_HEAD *head = &work->head;
	TW_FSTONE_PUTPOS *put = &head->putpos;
	
	{
		BOOL ret;
		
		if( twNowZoneID(head->tw) == head->zone_id ){
			ret = tw_FStoneVanishFlagCheck( head->tw, put->flag_no );
		}else{
			ret = tw_FStoneVanishFlagCheckLower( head->tw, put->flag_no );
		}
		
		if( ret == TRUE ){
			if( work->alpha > 0 ){
				work->alpha--;
				if( work->alpha_value != -1 ){
					work->alpha_value = -1;
					twSndCheckPlay( SE_YABURETA_GROUND_DELETE );
				}
			}
		}else if( work->alpha < 31 ){
			work->alpha++;
			if( work->alpha_value != 1 ){
				work->alpha_value = 1;
				twSndCheckPlay( SE_YABURETA_GROUND_APPEAR );
			}
		}
	}
	
	{
		work->draw_pos = work->pos;
		work->draw_pos.y += work->offs_y_fx;
		work->culling = tw_OBJResDataTypeCullingCheck(
			head->tw, head->putpos.type, &work->draw_pos );
	}
}

//--------------------------------------------------------------
///	ゆれテーブル
//--------------------------------------------------------------
static const fx32 DATA_FStoneMoveShakeTbl[FSTONE_OFFS_TBL_MAX] =
{
	0x0000,-0x1000,-0x2000,-0x4000,-0x5000,-0x5800,-0x5c00,-0x6000,
};

//--------------------------------------------------------------
/**
 * EOA 飛び石　動作　ゆれあり
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work
 * @retval	void
 */
//--------------------------------------------------------------
static void tw_EoaFStoneMoveShake( EOA_PTR eoa, void *wk )
{
	const fx32 *tbl;
	EOA_FSTONE_WORK *work = wk;
	EOA_FSTONE_HEAD *head = &work->head;
	TW_FSTONE_PUTPOS *put = &head->putpos;
	u32 hit = FALSE, zone_id = twNowZoneID( head->tw );
	
	if( head->putpos.type == TWOBJTYPE_STEP && zone_id == head->zone_id ){
		int objno = 0;
		int gy = G_GRID_H_GRID( head->putpos.gy );
		CONST_FIELD_OBJ_SYS_PTR fos = head->tw->fsys->fldobjsys;
		FIELD_OBJ_PTR fldobj;
		
		while( FieldOBJSys_FieldOBJSearch(
			fos,&fldobj,&objno,FLDOBJ_STA_BIT_USE) == TRUE ){
			if( FieldOBJ_NowPosGY_Get(fldobj) == gy ){
				if( FieldOBJ_NowPosGZ_Get(fldobj) == head->putpos.gz ){
					if( FieldOBJ_NowPosGX_Get(fldobj) == head->putpos.gx ){
						hit = TRUE;
						break;
					}
				}
			}
		}
	}
	
	tbl = DATA_FStoneMoveShakeTbl;
	work->offs_y_fx = tbl[FX32_NUM(work->offs_no)%FSTONE_OFFS_TBL_MAX];
	if( hit == TRUE ){ work->offs_y_fx >>= 1; }
	work->offs_no += work->offs_val;
	
	if( work->offs_no < 0 ){
		work->offs_no = 0;
		work->offs_val = FSTONE_OFFS_SPEED_FX;
	}else if( work->offs_no >= FSTONE_OFFS_TBL_MAX_FX ){
		work->offs_no = FSTONE_OFFS_TBL_MAX_FX - FSTONE_OFFS_SPEED_FX;
		work->offs_val = -FSTONE_OFFS_SPEED_FX;
	}
	
	{
		BOOL ret;
		
		if( twNowZoneID(head->tw) == head->zone_id ){
			ret = tw_FStoneVanishFlagCheck( head->tw, put->flag_no );
		}else{
			ret = tw_FStoneVanishFlagCheckLower( head->tw, put->flag_no );
		}
		
		if( ret == TRUE ){
			if( work->alpha > 0 ){
				work->alpha--;
				if( work->alpha_value != -1 ){
					work->alpha_value = -1;
					twSndCheckPlay( SE_YABURETA_GROUND_DELETE );
				}
			}
		}else if( work->alpha < 31 ){
			work->alpha++;
			if( work->alpha_value != 1 ){
				work->alpha_value = 1;
				twSndCheckPlay( SE_YABURETA_GROUND_APPEAR );
			}
		}
	}
	
	{
		work->draw_pos = work->pos;
		work->draw_pos.y += work->offs_y_fx;
		work->culling = tw_OBJResDataTypeCullingCheck(
			head->tw, head->putpos.type, &work->draw_pos );
	}
}

//--------------------------------------------------------------
/**
 * EOA 飛び石　描画
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work
 * @retval	void
 */
//--------------------------------------------------------------
static void tw_EoaFStoneDraw( EOA_PTR eoa, void *wk )
{
	EOA_FSTONE_WORK *work = wk;
	
	if( work->culling == FALSE ){
		return;
	}
	
	#ifdef DEBUG_TWORLD_CAPTURE
	if( DebugTWorldCaptureFlag_StepVanish == TRUE &&
		work->head.putpos.type == TWOBJTYPE_PUT ){
		return;
	}
	#endif
	
	if( work->alpha > 0 ){
		EOA_FSTONE_HEAD *head = &work->head;
		tw_OBJResDataMdlAlphaSet(
			head->tw, head->putpos.type, work->alpha );
		FRO_OBJ_DrawPos( &work->objres_p->robj, &work->draw_pos );
	}
}

#if 0
static void tw_EoaFStoneDraw( EOA_PTR eoa, void *wk )
{
	EOA_FSTONE_WORK *work = wk;
	
	if( work->alpha > 0 ){
		EOA_FSTONE_HEAD *head = &work->head;
		VecFx32 pos = work->pos;
		pos.y += work->offs_y_fx;
		tw_OBJResDataMdlAlphaSet(
			head->tw, head->putpos.type, work->alpha );
		
		#if 0
		FRO_OBJ_DrawPos( &work->objres_p->robj, &pos );
		#endif
		
		#if 0
		{
            MtxFx33 Mat33;
            const MtxFx43* pCamInv;
			VecFx32 scale = {FX32_ONE,FX32_ONE,FX32_ONE};
			MtxFx33 rot	  = { FX32_ONE, 0,0,0, FX32_ONE, 0,0,0,FX32_ONE};
            
			pCamInv = NNS_G3dGlbGetInvCameraMtx();
            Rot360_to_MtxFx33( &rot, 0, 0, 0 );
            MTX_Copy43To33( pCamInv, &Mat33 );
            MTX_Concat33( &rot, &Mat33, &rot );
			
			FRO_OBJ_Draw1Mat1Shape(
				&work->objres_p->robj, &pos, &scale, &rot );
		}
		#endif
		
		#if 0
		{
			FRO_MDL *rmdl;
			MtxFx33 rot;
			const VecFx32 scale = {FX32_ONE, FX32_ONE, FX32_ONE};
			const TW_OBJRES_ID *tbl = &DATA_OBJResIDTbl[head->putpos.type];
			u32 id = tbl->mdl_id;
			
			MTX_Identity33(&rot);
			rmdl = tw_OBJResDataMdlSearch( head->tw, id );
			
			if( BB_CullingCheck3DModel(rmdl->pResMdl,&pos,&rot,&scale) ){
				FRO_OBJ_DrawPos( &work->objres_p->robj, &pos );
			}
		}
		#endif
		
		#if 1
		if( work->vanish_flag == FALSE ){
			FRO_OBJ_DrawPos( &work->objres_p->robj, &pos );
		}
		#endif
	}
}
#endif

//==============================================================================
//	飛び石　EOA アニメ有り　木
//==============================================================================
//--------------------------------------------------------------
///	EOA_FSANM_WORK
//--------------------------------------------------------------
typedef struct
{
	s8 alpha;
	s8 alpha_value;
	u16 culling;
	u16 se_app;
	u16 se_del;
	VecFx32 pos;
	VecFx32 draw_pos;
	EOA_FSTONE_HEAD head;
	TW_OBJRES *objres_p;
}EOA_FSANM_WORK;

//--------------------------------------------------------------
/**
 * EOA 飛び石木　初期化
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work
 * @retval	int	FALSE=初期化失敗
 */
//--------------------------------------------------------------
static int tw_EoaFStoneTreeInit( EOA_PTR eoa, void *wk )
{
	int init;
	TW_FSTONE_PUTPOS *put;
	EOA_FSANM_WORK *work = wk;
	const EOA_FSTONE_HEAD *head = EOA_AddPtrGet( eoa );
	
	work->head = *head;
	put = &work->head.putpos;
	EOA_AnswerSet( eoa, put->type );
	
	work->objres_p = tw_OBJResDataOBJResGet( head->tw, put->type, &init );
	
	if( init == FALSE ){
		tw_OBJResDataOBJTypeInit(
			work->head.tw, put->type,
			&work->objres_p->robj, &work->objres_p->ranm );
	}
	
	if( put->type == TWOBJTYPE_LAND_ROCK ){
		work->se_app = SE_YABURETA_ROCK_APPEAR;
		work->se_del = SE_YABURETA_ROCK_DELETE;
	}else{
		work->se_app = SE_YABURETA_TREE_APPEAR;
		work->se_del = SE_YABURETA_TREE_DELETE;
	}
	
	{
		BOOL ret;
		
		if( twNowZoneID(head->tw) == head->zone_id ){
			ret = tw_FStoneVanishFlagCheck( head->tw, put->flag_no );
		}else{
			ret = tw_FStoneVanishFlagCheckLower( head->tw, put->flag_no );
		}
		
		if( ret == TRUE ){
			work->alpha = 0;
		}else{	//表示
			work->alpha = 31;
			FRO_ANM_FrameSet( &work->objres_p->ranm,
				FRO_ANM_MaxFrameGet(&work->objres_p->ranm) );
		}
	}
	
	FieldOBJTool_GridCenterPosGet( put->gx, put->gz, &work->pos );
	work->pos.y = GRID_SIZE_FX32( put->gy ) + GRID_HALF_FX32;
	
	{
		VecFx32 offs;
		const VecFx32 *d_offs = &DATA_OBJTypePosOffsTbl[put->type];
		work->pos.x += d_offs->x;
		work->pos.y += d_offs->y;
		work->pos.z += d_offs->z;
	}
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * EOA 飛び石木　削除
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work
 * @retval	void
 */
//--------------------------------------------------------------
static void tw_EoaFStoneTreeDelete( EOA_PTR eoa, void *wk )
{
	EOA_FSANM_WORK *work = wk;
	EOA_FSTONE_HEAD *head = &work->head;
	
	if( tw_OBJResDataTypeAnmCheck(head->putpos.type) == TRUE ){
		FRO_ANM_DeleteAll( &work->objres_p->ranm );
	}
	
	tw_OBJResDataOBJResCheckRelease( head->tw, work->objres_p );
}

//--------------------------------------------------------------
/**
 * EOA 飛び石木　動作
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work
 * @retval	void
 */
//--------------------------------------------------------------
static void tw_EoaFStoneTreeMove( EOA_PTR eoa, void *wk )
{
	EOA_FSANM_WORK *work = wk;
	EOA_FSTONE_HEAD *head = &work->head;
	TW_FSTONE_PUTPOS *put = &head->putpos;
	
	{
		BOOL ret;
		
		if( twNowZoneID(head->tw) == head->zone_id ){
			ret = tw_FStoneVanishFlagCheck( head->tw, put->flag_no );
		}else{
			ret = tw_FStoneVanishFlagCheckLower( head->tw, put->flag_no );
		}
		
		if( ret == TRUE ){
			fx32 frame;
			FRO_ANM_Play( &work->objres_p->ranm, -FX32_ONE*2, ANMLOOP_OFF );
			frame = FRO_ANM_FrameGet( &work->objres_p->ranm );
			
			if( work->alpha >= FX32_NUM(frame) ){
				if( work->alpha > 0 ){
					work->alpha -= 2;
					
					if( work->alpha_value != -2 ){
						work->alpha_value = -2;
						twSndCheckPlay( work->se_del );
					}
					
					if( work->alpha < 0 ){
						work->alpha = 0;
					}
				}else{
					work->alpha = 0;
				}
			}
		}else{
			if( work->alpha < 31 ){
				work->alpha += 2;
				
				if( work->alpha_value != 2 ){
					work->alpha_value = 2;
					twSndCheckPlay( work->se_app );
				}
				
				if( work->alpha > 31 ){
					work->alpha = 31;
				}
			}else{
				work->alpha = 31;
			}
		
			FRO_ANM_Play( &work->objres_p->ranm, FX32_ONE*2, ANMLOOP_OFF );
		}
	}
	
	{
		work->culling = tw_OBJResDataTypeCullingCheck(
			head->tw, head->putpos.type, &work->pos );
	}
}

#if 0
static void tw_EoaFStoneAnimeMove( EOA_PTR eoa, void *wk )
{
	EOA_FSANM_WORK *work = wk;
	EOA_FSTONE_HEAD *head = &work->head;
	
	if( tw_FStoneVanishFlagCheck(head->tw,head->putpos.flag_no) == TRUE ){
		fx32 frame;
		FRO_ANM_Play( &work->objres_p->ranm, -FX32_ONE, ANMLOOP_OFF );
		frame = FRO_ANM_FrameGet( &work->objres_p->ranm );
		
		if( work->alpha >= FX32_NUM(frame) ){
			if( work->alpha > 0 ){ work->alpha--; }
		}
	}else{
		if( work->alpha < 31 ){ work->alpha++; }
		FRO_ANM_Play( &work->objres_p->ranm, FX32_ONE, ANMLOOP_OFF );
	}
}
#endif

//--------------------------------------------------------------
/**
 * EOA 飛び石木　描画
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work
 * @retval	void
 */
//--------------------------------------------------------------
static void tw_EoaFStoneTreeDraw( EOA_PTR eoa, void *wk )
{
	EOA_FSANM_WORK *work = wk;
	
	if( work->culling == FALSE ){
		return;
	}
	
	if( work->alpha > 0 ){
		EOA_FSTONE_HEAD *head = &work->head;
		tw_OBJResDataMdlAlphaSet(
			head->tw, head->putpos.type, work->alpha );
		FRO_OBJ_DrawPos( &work->objres_p->robj, &work->pos );
	}
}

//==============================================================================
//	マップデータ
//==============================================================================
//--------------------------------------------------------------
/**
 * マップデータ　初期化
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_MapDataInit( TORNWORLD_WORK *tw )
{
	int zone_id;
	const TW_ZONE_LINKDATA *link;
	
	//情報取得
	zone_id = twNowZoneID( tw );
	link = tw_ZoneLinkDataGet( zone_id );
	
	//マップデータバッファ初期化
	tw_MapDataIdxInit( tw );
	tw_ZoneDataBufInit( tw, zone_id, link->lower_zone_id );
	
	//各マップデータ初期化
	tw_MapDataGndDataInit( tw );
	tw_MapDataEvJumpDataInit( tw );
	tw_MapDataCmActDataInit( tw );
	tw_MapDataFStoneDataInit( tw );
	
	//マップデータセット
	if( tw_SaveDataInitFinishCheck(tw) == FALSE ){	//初期化まだ
		//自機現地形データセット
		int gx,gy,gz;
		twJikiGridPosGet( tw, &gx, &gy, &gz );
		tw_MapDataGndDataPosDataSet( tw, gx, gy, gz, TW_GNDTYPE_MAX );
	}else{											//初期化済み
		//セーブデータの地形番号で初期化
		u32 no = tw_SaveDataSpGroundNoGet( tw );
		tw_MapDataGndDataNoSet( tw, no );
	}
}

//--------------------------------------------------------------
/**
 * マップデータ　リセット
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_MapDataReset( TORNWORLD_WORK *tw )
{
	//各マップデータ削除
	tw_MapDataCmActDataDelete( tw );
	tw_MapDataEvJumpDataDelete( tw );
	tw_MapDataGndDataDelete( tw );
	tw_MapDataFStoneDataDelete( tw );
	
	//マップデータバッファ削除
	tw_ZoneDataBufDelete( tw );
	
	//マップデータバッファ初期化
	tw_ZoneDataBufInit( tw, twNowZoneID(tw), ZONE_ID_MAX );
	
	//各マップデータ初期化
	tw_MapDataGndDataInit( tw );
	tw_MapDataEvJumpDataInit( tw );
	tw_MapDataCmActDataInit( tw );
	tw_MapDataFStoneDataInit( tw );
	
	//マップデータセット
	{	//自機現地形データセット
		int gx,gy,gz;
		twJikiGridPosGet( tw, &gx, &gy, &gz );
		tw_MapDataGndDataPosDataSet( tw, gx, gy, gz, TW_GNDTYPE_MAX );
	}
}

//--------------------------------------------------------------
/**
 * マップデータ　削除
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_MapDataDelete( TORNWORLD_WORK *tw )
{
	//各マップデータ削除
	tw_MapDataCmActDataDelete( tw );
	tw_MapDataEvJumpDataDelete( tw );
	tw_MapDataGndDataDelete( tw );
	tw_MapDataFStoneDataDelete( tw );
	
	//マップデータバッファ削除
	tw_ZoneDataBufDelete( tw );
	tw_MapDataIdxDelete( tw );
}

//--------------------------------------------------------------
/**
 * マップデータ　現在層を下層に移動　(上階層移動
 * @param	tw	TORNWORLD_WORK
 * @param	zone_id	新規ゾーンID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_MapDataRemoveNowLower( TORNWORLD_WORK *tw, u32 zone_id )
{
	tw_MapDataFStoneDataDeleteLower( tw );	//下層データ削除
	tw_ZoneDataBufDeleteLower( tw );		//下層バッファ削除
	
	tw_MapDataCmActDataDelete( tw );		//現在層マップデータ削除
	tw_MapDataEvJumpDataDelete( tw );
	tw_MapDataGndDataDelete( tw );
	
	tw_ZoneDataBufRemoveNowLower( tw );		//ゾーンバッファを下層移動
	tw_MapDataFStoneDataRemoveNowLower( tw );
	tw_FStoneRemoveNowLower( tw );
	tw_FStoneCleanNow( tw );
	
	tw_ZoneDataBufInitNow( tw, zone_id );	//新規現在層の読み込み
	tw_MapDataGndDataInit( tw );
	tw_MapDataGndDataNoMaxSet( tw );
	tw_MapDataEvJumpDataInit( tw );
	tw_MapDataCmActDataInit( tw );
	tw_MapDataFStoneDataInitNow( tw );
}

//--------------------------------------------------------------
/**
 * マップデータ　下層を現在層に移動　(下階層移動
 * @param	tw		TORNWORLD_WORK
 * @param	zone_id	新規下階層ゾーンID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_MapDataRemoveLowerNow( TORNWORLD_WORK *tw, u32 zone_id )
{
	tw_MapDataCmActDataDelete( tw );			//現在層マップデータ削除
	tw_MapDataEvJumpDataDelete( tw );
	tw_MapDataGndDataDelete( tw );
	tw_MapDataFStoneDataDeleteNow( tw );
	
	tw_ZoneDataBufDeleteNow( tw );				//現階層バッファ削除
	
	tw_ZoneDataBufRemoveLowerNow( tw );			//下層バッファを現階層へ
	tw_MapDataFStoneDataRemoveLowerNow( tw );	//下層飛び石を現階層へ
	tw_FStoneRemoveLowerNow( tw );
	tw_FStoneVanishFlagSaveDataSet( tw );
	tw_FStoneCleanLower( tw );
	
	tw_MapDataGndDataInit( tw );				//引き継いだバッファから初期化
	tw_MapDataGndDataNoMaxSet( tw );
	tw_MapDataEvJumpDataInit( tw );
	tw_MapDataCmActDataInit( tw );
	
	if( zone_id != ZONE_ID_MAX ){				//下層の読み込み
		tw_ZoneDataBufInitLower( tw, zone_id );
		tw_MapDataFStoneDataInitLower( tw );
	}
}

//--------------------------------------------------------------
/**
 * マップデータインデックス初期化　メイン
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_MapDataIdxInitMain(
		ARCHANDLE *arc_h_map, TW_MDATA_IDX *md_idx )
{
	u32 size = ArchiveDataSizeGetByHandle(
		arc_h_map, NARC_tw_arc_tw_maptable_idx );
	md_idx->idx_buf_p = sys_AllocMemory( HEAPID_FIELD, size );
	ArchiveDataLoadByHandle(
		arc_h_map, NARC_tw_arc_tw_maptable_idx, md_idx->idx_buf_p );
	md_idx->idx_max = *(int*)md_idx->idx_buf_p;
	md_idx->idx_p = md_idx->idx_buf_p;
	(u8*)md_idx->idx_p += 4;	//table start pos
	#ifdef DEBUG_TW_PRINTF
	OS_Printf( "mdata idx load idx=%d\n", md_idx->idx_max );
	#endif
}

//--------------------------------------------------------------
/**
 * マップデータインデックス初期化
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_MapDataIdxInit( TORNWORLD_WORK *tw )
{
	GF_ASSERT( tw->mdata.md_idx.idx_buf_p == NULL );
	tw_MapDataIdxInitMain( tw->arc_h_map, &tw->mdata.md_idx );
}

//--------------------------------------------------------------
/**
 * マップデータインデックス削除　メイン
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_MapDataIdxDeleteMain( TW_MDATA_IDX *md_idx )
{
	GF_ASSERT( md_idx->idx_buf_p != NULL );
	sys_FreeMemoryEz( md_idx->idx_buf_p );
	md_idx->idx_buf_p = NULL;
}

//--------------------------------------------------------------
/**
 * マップデータインデックス削除
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_MapDataIdxDelete( TORNWORLD_WORK *tw )
{
	tw_MapDataIdxDeleteMain( &tw->mdata.md_idx );
}

//--------------------------------------------------------------
/**
 * マップデータインデックス　指定のゾーンIDに対応したTW_MDIDX取得　メイン
 * @param	md_idx	TW_MDATA_IDX
 * @param	zone_id		zone id
 * @retval	TW_MDIDX*	TW_MDIDX	
 */
//--------------------------------------------------------------
static const TW_MDIDX * tw_MapDataIdxZoneIDMdIdxGetMain(
		const TW_MDATA_IDX *md_idx, int zone_id )
{
	int i = 0;
	const TW_MDIDX *tbl = md_idx->idx_p;
	while( i < md_idx->idx_max ){
		if( zone_id == tbl->zone_id ){ return( tbl ); }
		tbl++;
		i++;
	}
	
	OS_Printf( "tworld nothing zone_id error, error id = %d\n", zone_id );
	GF_ASSERT( 0 );
	return( NULL );
}

//--------------------------------------------------------------
/**
 * マップデータインデックス　指定のゾーンIDに対応したTW_MDIDX取得
 * @param	tw			TORNWORLD_WORK
 * @param	zone_id		zone id
 * @retval	TW_MDIDX*	TW_MDIDX	
 */
//--------------------------------------------------------------
static const TW_MDIDX * tw_MapDataIdxZoneIDMdIdxGet(
		TORNWORLD_WORK *tw, int zone_id )
{
	return( tw_MapDataIdxZoneIDMdIdxGetMain(&tw->mdata.md_idx,zone_id) );
}

//--------------------------------------------------------------
/**
 * マップデータインデックス　指定のゾーンIDのデータインデックス取得
 * @param	tw	TORNWORLD_WORK
 * @param	zone	zone id
 * @retval	u32	インデックス番号
 */
//--------------------------------------------------------------
static u32 tw_MapDataIdxZoneIDIdxGet( TORNWORLD_WORK *tw, int zone_id )
{
	const TW_MDIDX *tbl = tw_MapDataIdxZoneIDMdIdxGet( tw, zone_id );
	return( tbl->arc_idx + NARC_MAPIDX_BIN_START );
}

//--------------------------------------------------------------
/**
 * マップデータインデックス　指定のゾーンIDの始点グリッド座標を取得　メイン
 * @param	zone_id	ZONE ID
 * @param	x	X座標格納先
 * @param	y	X座標格納先
 * @param	z	X座標格納先
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_MapDataIdxZoneIDOriginGridPosGetMain(
		const TW_MDATA_IDX *md_idx, int zone_id, int *x, int *y, int *z )
{
	const TW_MDIDX *tbl;
	tbl = tw_MapDataIdxZoneIDMdIdxGetMain( md_idx, zone_id );
	*x = tbl->x;
	*y = tbl->y;
	*z = tbl->z;
}

//--------------------------------------------------------------
/**
 * マップデータインデックス　指定のゾーンIDの始点グリッド座標を取得
 * @param	tw	TORNWORLD_WORK
 * @param	zone_id	ZONE ID
 * @param	x	X座標格納先
 * @param	y	X座標格納先
 * @param	z	X座標格納先
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_MapDataIdxZoneIDOriginGridPosGet(
		TORNWORLD_WORK *tw, int zone_id, int *x, int *y, int *z )
{
	tw_MapDataIdxZoneIDOriginGridPosGetMain(
		&tw->mdata.md_idx, zone_id, x, y, z );
}

//--------------------------------------------------------------
/**
 * マップデータインデックス　指定のゾーンIDの座標(fx32)を取得
 * @param	tw	TORNWORLD_WORK
 * @param	zone_id	ZONE ID
 * @param	pos	座標格納先
 * @param	x	X座標格納先
 * @param	y	X座標格納先
 * @param	z	X座標格納先
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_MapDataIdxZoneIDOriginPosGet(
		TORNWORLD_WORK *tw, int zone_id, VecFx32 *pos )
{
	const TW_MDIDX *tbl = tw_MapDataIdxZoneIDMdIdxGet( tw, zone_id );
	pos->x = GRID_SIZE_FX32( tbl->x );
	pos->y = GRID_SIZE_FX32( tbl->y );
	pos->z = GRID_SIZE_FX32( tbl->z );
}

//--------------------------------------------------------------
/**
 * ゾーンデータバッファ　初期化　メイン
 * @param	tw	TORNWORLD_WORK
 * @param	buf	TW_ZONEDATA_BUF
 * @param	zone	zone id
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ZoneDataBufInitMain(
	TORNWORLD_WORK *tw, TW_ZONEDATA_BUF *buf, int zone_id )
{
	GF_ASSERT( buf->zone_buf_p == NULL );
	
	memset( buf, 0, sizeof(TW_ZONEDATA_BUF) );
	buf->zone_id = zone_id;
	
	if( zone_id != ZONE_ID_MAX ){
		u8 *buf_p;
		u32 size,idx;
		TW_ZONEDATA_BUF_IDX *idx_p;
		
		idx = tw_MapDataIdxZoneIDIdxGet( tw, zone_id );
		size = ArchiveDataSizeGetByHandle( tw->arc_h_map, idx );
		buf->zone_buf_p = sys_AllocMemoryLo( HEAPID_FIELD, size );
		ArchiveDataLoadByHandle( tw->arc_h_map, idx, buf->zone_buf_p );
	
		idx_p = buf->zone_buf_p;
		buf->idx_p = idx_p;
	
		buf_p = buf->zone_buf_p;
		buf_p += sizeof( TW_ZONEDATA_BUF_IDX );
	
		buf->gnd_p = buf_p;
		buf->evj_p = (u8*)buf->gnd_p + idx_p->gnd_size;
		buf->cma_p = (u8*)buf->evj_p + idx_p->evj_size;
		buf->fstone_p = (u8*)buf->cma_p + idx_p->cma_size;
		
		#ifdef DEBUG_TW_PRINTF
		OS_Printf( "mdata zonedata load bin idx=%d\n", idx );
		#endif
	}
}

#if 0
static void tw_ZoneDataBufInitMain(
	TORNWORLD_WORK *tw, TW_ZONEDATA_BUF *buf, int zone_id )
{
	u8 *buf_p;
	u32 size,idx;
	TW_ZONEDATA_BUF_IDX *idx_p;
	
	idx = tw_MapDataIdxZoneIDIdxGet( tw, zone_id );
	size = ArchiveDataSizeGetByHandle( tw->arc_h_map, idx );
	buf->zone_buf_p = sys_AllocMemory( HEAPID_FIELD, size );
	ArchiveDataLoadByHandle( tw->arc_h_map, idx, buf->zone_buf_p );
	
	idx_p = buf->zone_buf_p;
	buf->idx_p = idx_p;
	
	buf_p = buf->zone_buf_p;
	buf_p += sizeof( TW_ZONEDATA_BUF_IDX );
	
	buf->gnd_p = buf_p;
	buf->evj_p = (u8*)buf->gnd_p + idx_p->gnd_size;
	buf->cma_p = (u8*)buf->evj_p + idx_p->evj_size;
	buf->fstone_p = (u8*)buf->cma_p + idx_p->cma_size;
	
	#ifdef DEBUG_TW_PRINTF
	OS_Printf( "mdata zonedata load bin idx=%d\n", idx );
	#endif
}
#endif

//--------------------------------------------------------------
/**
 * ゾーンデータバッファ　初期化
 * @param	tw	TORNWORLD_WORK
 * @param	zone	zone id
 * @param	l_zone	lower zone_id
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ZoneDataBufInit(
	TORNWORLD_WORK *tw, int zone_id, int l_zone_id )
{
	tw_ZoneDataBufInitNow( tw, zone_id );
	tw_ZoneDataBufInitLower( tw, l_zone_id );
}

//--------------------------------------------------------------
/**
 * ゾーンデータバッファ　初期化　現在のみ
 * @param	tw	TORNWORLD_WORK
 * @param	zone	zone id
 * @param	l_zone	lower zone_id
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ZoneDataBufInitNow( TORNWORLD_WORK *tw, int zone_id )
{
	#ifdef DEBUG_TW_PRINTF
	OS_Printf( "mdata zonedata main load start\n" );
	#endif
	tw_ZoneDataBufInitMain( tw, &tw->mdata.zone_buf, zone_id );
}

//--------------------------------------------------------------
/**
 * ゾーンデータバッファ　初期化　下層のみ
 * @param	tw	TORNWORLD_WORK
 * @param	zone	zone id
 * @param	l_zone	lower zone_id
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ZoneDataBufInitLower( TORNWORLD_WORK *tw, int zone_id )
{
	#ifdef DEBUG_TW_PRINTF
	OS_Printf( "mdata zonedata lower load start\n" );
	#endif
	tw_ZoneDataBufInitMain( tw, &tw->mdata.zone_buf_lower, zone_id );
}

//--------------------------------------------------------------
/**
 * ゾーンデータバッファ　クリア
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ZoneDataBufClean( TW_ZONEDATA_BUF *buf )
{
	memset( buf, 0, sizeof(TW_ZONEDATA_BUF) );
	buf->zone_id = ZONE_ID_MAX;
}	

//--------------------------------------------------------------
/**
 * ゾーンデータバッファ　削除　メイン
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ZoneDataBufDeleteMain( TW_ZONEDATA_BUF *buf )
{
	if( buf->zone_buf_p != NULL ){
		sys_FreeMemoryEz( buf->zone_buf_p );
	}
	tw_ZoneDataBufClean( buf );
}	

//--------------------------------------------------------------
/**
 * ゾーンデータバッファ　削除
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ZoneDataBufDelete( TORNWORLD_WORK *tw )
{
	tw_ZoneDataBufDeleteNow( tw );
	tw_ZoneDataBufDeleteLower( tw );
}

//--------------------------------------------------------------
/**
 * ゾーンデータバッファ　削除　現在のみ
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ZoneDataBufDeleteNow( TORNWORLD_WORK *tw )
{
	tw_ZoneDataBufDeleteMain( &tw->mdata.zone_buf );
}

//--------------------------------------------------------------
/**
 * ゾーンデータバッファ　削除　下層のみ
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ZoneDataBufDeleteLower( TORNWORLD_WORK *tw )
{
	tw_ZoneDataBufDeleteMain( &tw->mdata.zone_buf_lower );
}

//--------------------------------------------------------------
/**
 * ゾーンデータバッファ　現在層を下層に移動
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ZoneDataBufRemoveNowLower( TORNWORLD_WORK *tw )
{
	GF_ASSERT( tw->mdata.zone_buf_lower.zone_id == ZONE_ID_MAX );
	GF_ASSERT( tw->mdata.zone_buf_lower.zone_buf_p == NULL );
	tw->mdata.zone_buf_lower = tw->mdata.zone_buf;
	tw_ZoneDataBufClean( &tw->mdata.zone_buf );
}

//--------------------------------------------------------------
/**
 * ゾーンデータバッファ　下層を現在層に移動し下層バッファクリア
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ZoneDataBufRemoveLowerNow( TORNWORLD_WORK *tw )
{
	GF_ASSERT( tw->mdata.zone_buf.zone_id == ZONE_ID_MAX );
	GF_ASSERT( tw->mdata.zone_buf.zone_buf_p == NULL );
	tw->mdata.zone_buf = tw->mdata.zone_buf_lower;
	tw_ZoneDataBufClean( &tw->mdata.zone_buf_lower );
}

//--------------------------------------------------------------
/**
 * ゾーンデータバッファ　ゾーンバッファ存在チェック
 * @param	tw	TORNWORLD_WORK
 * @retval	u32	size
 */
//--------------------------------------------------------------
static BOOL tw_ZoneDataBufValidCheck( TW_ZONEDATA_BUF *buf )
{
	if( buf->zone_id == ZONE_ID_MAX ){
		return( FALSE );
	}
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * ゾーンデータバッファ　地形データサイズ取得
 * @param	tw	TORNWORLD_WORK
 * @retval	u32	size
 */
//--------------------------------------------------------------
static int tw_ZoneDataBufGndDataSizeGet( TW_ZONEDATA_BUF *buf )
{
	TW_ZONEDATA_BUF_IDX *idx = buf->idx_p;
	return( idx->gnd_size );
}

//--------------------------------------------------------------
/**
 * ゾーンデータバッファ　地形データ数　取得
 * @param	tw	TORNWORLD_WORK
 * @retval	int	データ要素数
 */
//--------------------------------------------------------------
static int tw_ZoneDataBufGndDataCountGet( TW_ZONEDATA_BUF *buf )
{
	int *p = buf->gnd_p;
	return( *p );
}

//--------------------------------------------------------------
/**
 * ゾーンデータバッファ　地形データ取得
 * @param	tw	TORNWORLD_WORK
 * @retval	TW_GNDDATA
 */
//--------------------------------------------------------------
static TW_GNDDATA * tw_ZoneDataBufGndDataGet( TW_ZONEDATA_BUF *buf )
{
	u8 *p = buf->gnd_p;
	p += 4;				//データ数飛ばし
	return( (TW_GNDDATA*)p );
}

//--------------------------------------------------------------
/**
 * ゾーンデータバッファ　ジャンプイベントデータサイズ取得
 * @param	tw	TORNWORLD_WORK
 * @retval	u32	size
 */
//--------------------------------------------------------------
static int tw_ZoneDataBufEvJumpDataSizeGet( TW_ZONEDATA_BUF *buf )
{
	TW_ZONEDATA_BUF_IDX *idx = buf->idx_p;
	return( idx->evj_size );
}

//--------------------------------------------------------------
/**
 * ゾーンデータバッファ　ジャンプイベントデータ数　取得
 * @param	tw	TORNWORLD_WORK
 * @retval	int	データ要素数
 */
//--------------------------------------------------------------
static int tw_ZoneDataBufEvJumpDataCountGet( TW_ZONEDATA_BUF *buf )
{
	int *p = buf->evj_p;
	return( *p );
}

//--------------------------------------------------------------
/**
 * ゾーンデータバッファ　ジャンプイベントデータ取得
 * @param	tw	TORNWORLD_WORK
 * @retval	TW_EVJUMPDATA
 */
//--------------------------------------------------------------
static TW_EVJUMPDATA * tw_ZoneDataBufEvJumpDataGet( TW_ZONEDATA_BUF *buf )
{
	u8 *p = buf->evj_p;
	p += 4;				//データ数飛ばし
	return( (TW_EVJUMPDATA*)p );
}

//--------------------------------------------------------------
/**
 * ゾーンデータバッファ　カメラアクションデータサイズ取得
 * @param	tw	TORNWORLD_WORK
 * @retval	u32	size
 */
//--------------------------------------------------------------
static int tw_ZoneDataBufCmActDataSizeGet( TW_ZONEDATA_BUF *buf )
{
	TW_ZONEDATA_BUF_IDX *idx = buf->idx_p;
	return( idx->cma_size );
}

//--------------------------------------------------------------
/**
 * ゾーンデータバッファ　カメラアクションデータ数取得
 * @param	tw	TORNWORLD_WORK
 * @retval	int	データ要素数
 */
//--------------------------------------------------------------
static int tw_ZoneDataBufCmActDataCountGet( TW_ZONEDATA_BUF *buf )
{
	int *p = buf->cma_p;
	return( *p );
}

//--------------------------------------------------------------
/**
 * ゾーンデータバッファ　カメラアクションデータ取得
 * @param	tw	TORNWORLD_WORK
 * @retval	TW_EVJUMPDATA
 */
//--------------------------------------------------------------
static TW_CMACTDATA * tw_ZoneDataBufCmActDataGet( TW_ZONEDATA_BUF *buf )
{
	u8 *p = buf->cma_p;
	p += 4;				//データ数飛ばし
	return( (TW_CMACTDATA*)p );
}

//--------------------------------------------------------------
/**
 * ゾーンデータバッファ　飛び石データサイズ取得
 * @param	tw	TORNWORLD_WORK
 * @retval	u32	size
 */
//--------------------------------------------------------------
static int tw_ZoneDataBufFStoneDataSizeGet( TORNWORLD_WORK *tw )
{
	TW_ZONEDATA_BUF *buf = &tw->mdata.zone_buf;
	TW_ZONEDATA_BUF_IDX *idx = buf->idx_p;
	return( idx->fstone_size );
}

//--------------------------------------------------------------
/**
 * ゾーンデータバッファ　飛び石データサイズ取得　下層
 * @param	tw	TORNWORLD_WORK
 * @retval	u32	size
 */
//--------------------------------------------------------------
static int tw_ZoneDataBufFStoneDataSizeGetLower( TORNWORLD_WORK *tw )
{
	int size = 0;
	TW_ZONEDATA_BUF *buf = &tw->mdata.zone_buf_lower;
	
	if( buf->zone_id != ZONE_ID_MAX ){
		TW_ZONEDATA_BUF_IDX *idx = buf->idx_p;
		size = idx->fstone_size;
	}
	
	return( size );
}

//--------------------------------------------------------------
/**
 * ゾーンデータバッファ　飛び石ヘッダ取得
 * @param	buf	TW_ZONEDATA_BUF
 * @retval	nothing
 */
//--------------------------------------------------------------
static const TW_FSTONE_HEADER * tw_ZoneDataBufFStoneHeaderGet(
		TW_ZONEDATA_BUF *buf )
{
	TW_FSTONE_HEADER *head = buf->fstone_p;
	return( head );
}

//--------------------------------------------------------------
/**
 * ゾーンデータバッファ　飛び石配置データ取得
 * @param	buf	TW_ZONEDATA_BUF *
 * @retval	TW_FSTONE_PUTPOS *
 */
//--------------------------------------------------------------
static const TW_FSTONE_PUTPOS * tw_ZoneDataBufFStonePutPosDataGet(
		TW_ZONEDATA_BUF *buf )
{
	const u8 *p = buf->fstone_p;
	p += sizeof( TW_FSTONE_HEADER );
	return( (TW_FSTONE_PUTPOS*)p );
}

//--------------------------------------------------------------
/**
 * ゾーンデータバッファ　飛び石フラグ操作データ取得
 * @param	tw	TORNWORLD_WORK
 * @retval	TW_FSTONE_FLAGPOS*
 */
//--------------------------------------------------------------
static const TW_FSTONE_FLAGPOS * tw_ZoneDataBufFStoneFlagPosDataGet(
		TW_ZONEDATA_BUF *buf )
{
	const TW_FSTONE_HEADER *head = buf->fstone_p;
	const u8 *p = buf->fstone_p;
	p += sizeof(TW_FSTONE_HEADER) + (sizeof(TW_FSTONE_PUTPOS)*head->put_max);
	return( (TW_FSTONE_FLAGPOS*)p );
}

//--------------------------------------------------------------
/**
 * 地形データ　初期化
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_MapDataGndDataInit( TORNWORLD_WORK *tw )
{
	TW_ZONEDATA_BUF *zone_buf = &tw->mdata.zone_buf;
	GF_ASSERT( tw->mdata.gnd_tbl.gnd_tbl_p == NULL );
	
	if( tw_ZoneDataBufGndDataSizeGet(zone_buf) ){
		TW_GNDDATA_TBL *gnd = &tw->mdata.gnd_tbl;
		gnd->tbl_max = tw_ZoneDataBufGndDataCountGet( zone_buf );
		gnd->gnd_tbl_p = tw_ZoneDataBufGndDataGet( zone_buf );
		#ifdef DEBUG_TW_PRINTF
		OS_Printf( "mdata gnddata init max=%d\n", gnd->tbl_max );
		#endif
	}
}

//--------------------------------------------------------------
/**
 * 地形データ　削除
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_MapDataGndDataDelete( TORNWORLD_WORK *tw )
{
	TW_GNDDATA_TBL *gnd = &tw->mdata.gnd_tbl;
	
	if( gnd->attr_buf_p != NULL ){
		sys_FreeMemoryEz( gnd->attr_buf_p );
	}
	
	memset( gnd, 0, sizeof(TW_GNDDATA_TBL) );
}

//--------------------------------------------------------------
/**
 * 地形データ　指定座標に存在する特殊地形データをチェック
 * @param	TORNWORLD_WORK
 * @param	gx	グリッドX
 * @param	gy	グリッドY
 * @param	gz	グリッドZ
 * @param	
 * @retval	nothing
 */
//--------------------------------------------------------------
static BOOL tw_MapDataGndDataPosExistCheck(
	TORNWORLD_WORK *tw, int gx, int gy, int gz, s16 type )
{
	TW_GNDDATA_TBL *gnd = &tw->mdata.gnd_tbl;
	const TW_GNDDATA *data = gnd->gnd_tbl_p;
	int i = 0, max = gnd->tbl_max;
	
	if( max == 0 ){				//特殊地形無し
		return( FALSE );
	}
	
	do{
		if( twGridRectCheck(gx,gy,gz,&data->rect_size) == TRUE ){
			if( type != TW_GNDTYPE_MAX || type == data->type ){
				return( TRUE );
			}
		}
		
		data++;
		i++;
	}while( i < max );
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * 地形データ　指定座標に一致する地形データセット
 * 初期化されるのは 現地形番号、アトリビュート。
 * @param	TORNWORLD_WORK
 * @param	gx	グリッドX
 * @param	gy	グリッドY
 * @param	gz	グリッドZ
 * @param	type TW_GNDTYPE_GROUND等。
 * MAXの場合、判定は座標一致のみ。
 * 指定がある場合は座標とtypeで設定された地形に一致するものを選ぶ。
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_MapDataGndDataPosDataSet(
	TORNWORLD_WORK *tw, int gx, int gy, int gz, s16 type )
{
	TW_GNDDATA_TBL *gnd = &tw->mdata.gnd_tbl;
	const TW_GNDDATA *data = gnd->gnd_tbl_p;
	int i = 0, max = gnd->tbl_max;
	
	gnd->gnd_num_now = max;		//無効ナンバーで初期化
	tw_SaveDataSpGroundNoSet( tw, max );
	
	if( gnd->attr_buf_p != NULL ){
		sys_FreeMemoryEz( gnd->attr_buf_p );
		gnd->attr_buf_p = NULL;
	}
	
	if( max == 0 ){				//特殊地形無し
		return;
	}
	
	do{
		if( twGridRectCheck(gx,gy,gz,&data->rect_size) == TRUE ){
			if( type == TW_GNDTYPE_MAX || type == data->type ){
				#ifdef DEBUG_TW_PRINTF
				OS_Printf( "tw sp ground %d\n", i );
				#endif
				gnd->gnd_num_now = i;
				tw_SaveDataSpGroundNoSet( tw, i );
				tw_MapDataGndDataAttrLoad( tw, data->attr_idx );
				return;
			}
		}
		
		data++;
		i++;
	}while( i < max );
	
	#ifdef DEBUG_TW_PRINTF
	OS_Printf( "tw normal ground\n" );
	#endif
}

//--------------------------------------------------------------
/**
 * 地形データ　指定番号の地形データセット
 * @param	TORNWORLD_WORK
 * @param	no	データ番号
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_MapDataGndDataNoSet( TORNWORLD_WORK *tw, u32 no )
{
	TW_GNDDATA_TBL *gnd = &tw->mdata.gnd_tbl;
	const TW_GNDDATA *data = gnd->gnd_tbl_p;
	int i = 0, max = gnd->tbl_max;
	
	gnd->gnd_num_now = max;		//無効ナンバーで初期化
	
	if( gnd->attr_buf_p != NULL ){
		sys_FreeMemoryEz( gnd->attr_buf_p );
		gnd->attr_buf_p = NULL;
	}
	
	if( max == 0 || no >= max || no < 0 ){		//特殊地形無し
		tw_SaveDataSpGroundNoSet( tw, max );
		return;
	}
	
	gnd->gnd_num_now = no;
	tw_SaveDataSpGroundNoSet( tw, no );
	tw_MapDataGndDataAttrLoad( tw, data[no].attr_idx );
}

//--------------------------------------------------------------
/**
 * 地形データ　地形番号を無効で初期化。
 * tw_MapDataGndDataInit()後である事が前提
 * @param	TORNWORLD_WORK
 * @param	no	データ番号
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_MapDataGndDataNoMaxSet( TORNWORLD_WORK *tw )
{
	TW_GNDDATA_TBL *gnd = &tw->mdata.gnd_tbl;
	gnd->gnd_num_now = gnd->tbl_max;
	tw_SaveDataSpGroundNoSet( tw, gnd->tbl_max );
	
	if( gnd->attr_buf_p != NULL ){
		sys_FreeMemoryEz( gnd->attr_buf_p );
		gnd->attr_buf_p = NULL;
	}
}	

//--------------------------------------------------------------
/**
 * 地形データ　現在の地形タイプを返す
 * @param	TORNWORLD_WORK
 * @param	no	データ番号
 * @retval	nothing
 */
//--------------------------------------------------------------
static u32 tw_MapDataGndDataGndTypeNowGet( TORNWORLD_WORK *tw )
{
	TW_GNDDATA_TBL *gnd = &tw->mdata.gnd_tbl;
	const TW_GNDDATA *data = gnd->gnd_tbl_p;
	int i = 0, max = gnd->tbl_max;
	
	if( gnd->tbl_max == 0 ||
		gnd->gnd_num_now >= gnd->tbl_max ){
		return( TW_GNDTYPE_MAX );
	}
	
	{
		TW_GNDDATA *data = &gnd->gnd_tbl_p[gnd->gnd_num_now];
		return( data->type );
	}
}

//--------------------------------------------------------------
/**
 * 地形データ　現在の地形が特殊地形かどうか
 * @param	TORNWORLD_WORK
 * @retval	BOOL	TRUE=特殊地形
 */
//--------------------------------------------------------------
static u32 tw_MapDataGndDataNowGndCheck( TORNWORLD_WORK *tw )
{
	TW_GNDDATA_TBL *gnd = &tw->mdata.gnd_tbl;
	
	if( gnd->tbl_max == 0 ||
		gnd->gnd_num_now >= gnd->tbl_max ){
		return( FALSE );
	}
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * 地形データ　現在読み込んでいる地形タイプを取得
 * @param	tw	TORNWORLD_WORK
 * @retval	u32	TW_GNDTYPE_GROUND等
 */
//--------------------------------------------------------------
static u32 tw_MapDataNowGndTypeGet( TORNWORLD_WORK *tw )
{
	TW_GNDDATA_TBL *gnd = &tw->mdata.gnd_tbl;
	int max = gnd->tbl_max;
	
	if( gnd->tbl_max == 0 ||
		gnd->gnd_num_now >= gnd->tbl_max ){
		return( TW_GNDTYPE_MAX );
	}
	
	{
		const TW_GNDDATA *data = &gnd->gnd_tbl_p[gnd->gnd_num_now];
		return( data->type );
	}
}

//--------------------------------------------------------------
/**
 * 地形データ　指定座標の地形タイプを取得
 * @param	tw	TORNWORLD_WORK
 * @param	gx	グリッド座標X
 * @param	gy	グリッド座標Y
 * @param	gz	グリッド座標Z
 * @retval	u32	TW_GNDTYPE_GROUND等
 */
//--------------------------------------------------------------
static u32 tw_MapDataPosGndTypeGet(
		TORNWORLD_WORK *tw, int gx, int gy, int gz )
{
	TW_GNDDATA_TBL *gnd = &tw->mdata.gnd_tbl;
	int max = gnd->tbl_max;
	
	if( gnd->tbl_max == 0 ||
		gnd->gnd_num_now >= gnd->tbl_max ){
		return( TW_GNDTYPE_MAX );
	}
	
	{
		const TW_GNDDATA *data = &gnd->gnd_tbl_p[gnd->gnd_num_now];
		if( twGridRectCheck(gx,gy,gz,&data->rect_size) == FALSE ){
			return( TW_GNDTYPE_GROUND );
		}
		
		return( data->type );
	}
}

//--------------------------------------------------------------
/**
 * 地形データ　指定のアトリビュート読み込み。
 * 既に読み込み済みの場合、開放してから読み込む。
 * @param	tw	TORNWORLD_WORK
 * @param	idx	アーカイブインデックス
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_MapDataGndDataAttrLoad( TORNWORLD_WORK *tw, u32 idx )
{
	TW_GNDDATA_TBL *gnd = &tw->mdata.gnd_tbl;
	
	if( gnd->attr_buf_p != NULL ){
		sys_FreeMemoryEz( gnd->attr_buf_p );
	}
	
	gnd->attr_size = ArchiveDataSizeGetByHandle( tw->arc_h_attr, idx );
	gnd->attr_buf_p = sys_AllocMemoryLo( HEAPID_FIELD, gnd->attr_size );
	ArchiveDataLoadByHandle( tw->arc_h_attr, idx, gnd->attr_buf_p );
}

//--------------------------------------------------------------
/**
 * 地形データ　指定座標のアトリビュートを取得　座標変換無し
 * @param	TORNWORLD_WORK
 * @param	tw	TORNWORLD_WORK
 * @param	gx	グリッドX
 * @param	gz	グリッドZ
 * @retval	nothing
 */
//--------------------------------------------------------------
static u16 tw_MapDataGndDataAttrGetPos( TORNWORLD_WORK *tw, int gx, int gz )
{
	u16 attr;
	TW_GNDDATA_TBL *gnd = &tw->mdata.gnd_tbl;
	const TW_GNDDATA *data = &gnd->gnd_tbl_p[gnd->gnd_num_now];
	GF_ASSERT( gnd->attr_buf_p != NULL );
	attr = gnd->attr_buf_p[gx+(gz*data->attr_gx_size)];
	return( attr );
}

#if 0 //old
static u16 tw_MapDataGndDataAttrGetPos(
	TORNWORLD_WORK *tw, int gx, int gy, int gz )
{
	u16 attr;
	TW_GNDDATA_TBL *gnd = &tw->mdata.gnd_tbl;
	const TW_GNDDATA *data = &gnd->gnd_tbl_p[gnd->gnd_num_now];
	
	GF_ASSERT( gnd->attr_buf_p != NULL );
	
	switch( data->type ){
	case TW_GNDTYPE_GROUND:				//そのまま
		attr = gnd->attr_buf_p[gx+(gz*data->attr_gx_size)];
		break;
	case TW_GNDTYPE_SIDE_L:
		attr = gnd->attr_buf_p[gz+(gy*data->attr_gx_size)];
		break;
	case TW_GNDTYPE_SIDE_R:
		attr = gnd->attr_buf_p[gz+(gy*data->attr_gx_size)];
		break;
	case TW_GNDTYPE_ROOF:
		attr = gnd->attr_buf_p[gx+(gz*data->attr_gx_size)];
		break;
	}
	
	return( attr );
}
#endif

//--------------------------------------------------------------
/**
 * 地形データ　アトリビュート取得　地形別に座標変換　座標チェック有り
 * @param	tw	TORNWORLD_WORK
 * @param	gx	グリッドX
 * @param	gy	グリッドY
 * @param	gz	グリッドZ
 * @retval	u16	取得したアトリビュート TWATTR_NOT等=エラー
 */
//--------------------------------------------------------------
static u16 tw_MapDataGndDataAttrGetCheckPos(
	TORNWORLD_WORK *tw, int gx, int gy, int gz )
{
	u16 attr;
	int c_gx = 0,c_gz = 0;
	TW_GNDDATA_TBL *gnd = &tw->mdata.gnd_tbl;
	
	if( gnd->tbl_max == 0 ||				//特地無しor特地に居ない場合は 
		gnd->gnd_num_now >= gnd->tbl_max ){
		#if 0	//通常アトリビュート取得
		attr = GetAttribute( tw->fsys, gx, gz );
		return( attr );
		#else	//通常移動の際はこの関数を呼ぶことは無い
		return( TWATTR_NOT );
		#endif
	}
	
	{
		TW_GNDDATA *data = &gnd->gnd_tbl_p[gnd->gnd_num_now];
		
		if( twGridRectCheck(gx,gy,gz,&data->rect_size) == FALSE ){
			#ifdef DEBUG_TW_PRINTF_FORCE
			OS_Printf( "乗っている地形の範囲外\n" );
			#endif
			return( TWATTR_OVER );
		}
		
		GF_ASSERT( gnd->attr_buf_p != NULL );
		
		switch( data->type ){				//地形別座標変換
		case TW_GNDTYPE_GROUND:
			c_gx = gx - data->rect_size.gx;
			c_gz = gz - data->rect_size.gz;
			break;
		case TW_GNDTYPE_SIDE_L:
			c_gx = data->rect_size.gy_size - (gy - data->rect_size.gy);
			c_gz = gz - data->rect_size.gz;
			break;
		case TW_GNDTYPE_SIDE_R:
			c_gx = gy - data->rect_size.gy;
			c_gz = gz - data->rect_size.gz;
			break;
		case TW_GNDTYPE_ROOF:
			c_gx = data->rect_size.gx_size - (gx - data->rect_size.gx);
			c_gz = gz - data->rect_size.gz;
			break;
		default:
			GF_ASSERT( 0 );
			break;
		}
		
		#ifdef DEBUG_TW_PRINTF
		OS_Printf( "CHECK_GX=%d,CHECK_GZ=%d\n", c_gx, c_gz );
		#endif
		
		attr = tw_MapDataGndDataAttrGetPos( tw, c_gx, c_gz );
		return( attr );
	}
}

//--------------------------------------------------------------
/**
 * ジャンプイベントデータ　初期化
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_MapDataEvJumpDataInit( TORNWORLD_WORK *tw )
{
	TW_ZONEDATA_BUF *zone_buf = &tw->mdata.zone_buf;
	GF_ASSERT( tw->mdata.evj_tbl.evj_tbl_p == NULL );
	
	if( tw_ZoneDataBufEvJumpDataSizeGet(zone_buf) ){
		TW_EVJUMPDATA_TBL *evj = &tw->mdata.evj_tbl;
		evj->tbl_max = tw_ZoneDataBufEvJumpDataCountGet( zone_buf );
		evj->evj_tbl_p = tw_ZoneDataBufEvJumpDataGet( zone_buf );
		#ifdef DEBUG_TW_PRINTF
		OS_Printf( "mdata jumpdata init max=%d\n", evj->tbl_max );
		#endif
	}
}

//--------------------------------------------------------------
/**
 * ジャンプイベントデータ　削除
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_MapDataEvJumpDataDelete( TORNWORLD_WORK *tw )
{
	TW_EVJUMPDATA_TBL *evj = &tw->mdata.evj_tbl;
	memset( evj, 0, sizeof(TW_EVJUMPDATA_TBL) );
}

//--------------------------------------------------------------
/**
 * ジャンプイベントデータ　座標チェック
 * @param	tw	TORNWORLD_WORK
 * @param	gx	グリッドX
 * @param	gy	グリッドY
 * @param	gz	グリッドZ
 * @param	dir	DIR_UP等
 * @retval	TW_EVJUMPDATA*	NULL=外れ
 */
//--------------------------------------------------------------
static const TW_EVJUMPDATA * tw_MapDataEvJumpDataPosCheck(
	TORNWORLD_WORK *tw, int gx, int gy, int gz, int dir )
{
	TW_EVJUMPDATA_TBL *evj = &tw->mdata.evj_tbl;
	TW_EVJUMPDATA *data = evj->evj_tbl_p;
	int i = evj->tbl_max;
	
	while( i ){
		if( dir == data->dir ){
			if( twGridRectCheck(gx,gy,gz,&data->rect_size) ){
				return( data );
			}
		}
		
		data++;
		i--;
	}
	
	return( NULL );
}

//--------------------------------------------------------------
/**
 * カメラアクションデータ　初期化
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_MapDataCmActDataInit( TORNWORLD_WORK *tw )
{
	TW_ZONEDATA_BUF *zone_buf = &tw->mdata.zone_buf;
	GF_ASSERT( tw->mdata.cma_tbl.cma_tbl_p == NULL );
	
	if( tw_ZoneDataBufCmActDataSizeGet(zone_buf) ){
		TW_CMACTDATA_TBL *cma = &tw->mdata.cma_tbl;
		cma->tbl_max = tw_ZoneDataBufCmActDataCountGet( zone_buf );
		cma->cma_tbl_p = tw_ZoneDataBufCmActDataGet( zone_buf );
		#ifdef DEBUG_TW_PRINTF
		OS_Printf( "mdata cmadata init max=%d\n", cma->tbl_max );
		#endif
	}
}

//--------------------------------------------------------------
/**
 * カメラアクションデータ　削除
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_MapDataCmActDataDelete( TORNWORLD_WORK *tw )
{
	TW_CMACTDATA_TBL *cma = &tw->mdata.cma_tbl;
	memset( cma, 0, sizeof(TW_CMACTDATA_TBL) );
}

//--------------------------------------------------------------
/**
 * カメラアクションデータ　座標チェック
 * @param	tw	TORNWORLD_WORK
 * @param	gx	グリッドX
 * @param	gy	グリッドY
 * @param	gz	グリッドZ
 * @param	dir	DIR_UP等
 * @retval	TW_CMACTDATA*	NULL=外れ
 */
//--------------------------------------------------------------
static const TW_CMACTDATA * tw_MapDataCmActDataPosCheck(
	TORNWORLD_WORK *tw, int gx, int gy, int gz, int dir )
{
	TW_CMACTDATA_TBL *cma = &tw->mdata.cma_tbl;
	TW_CMACTDATA *data = cma->cma_tbl_p;
	int i = cma->tbl_max;
	
	while( i ){
		if( dir == data->dir ){
			if( twGridRectCheck(gx,gy,gz,&data->rect_size) ){
				return( data );
			}
		}
		
		data++;
		i--;
	}
	
	return( NULL );
}

//--------------------------------------------------------------
/**
 * 飛び石データ　初期化 メイン
 * @param	buf	TW_ZONEDATA_BUF
 * @param	fsdata	TW_FSTONEDATA_TBL
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_MapDataFStoneDataInitMain(
		TW_ZONEDATA_BUF *buf, TW_FSTONEDATA_TBL *fsdata )
{
	GF_ASSERT( fsdata->head_p == NULL );
	GF_ASSERT( fsdata->putpos_tbl_p == NULL );
	GF_ASSERT( fsdata->flagpos_tbl_p == NULL );
	fsdata->head_p = tw_ZoneDataBufFStoneHeaderGet( buf );
	fsdata->putpos_tbl_p = tw_ZoneDataBufFStonePutPosDataGet( buf );
	fsdata->flagpos_tbl_p = tw_ZoneDataBufFStoneFlagPosDataGet( buf );
}	

//--------------------------------------------------------------
/**
 * 飛び石データ　初期化　現在層
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_MapDataFStoneDataInitNow( TORNWORLD_WORK *tw )
{
	if( tw_ZoneDataBufFStoneDataSizeGet(tw) ){			//メイン
		TW_ZONEDATA_BUF *buf = &tw->mdata.zone_buf;
		TW_FSTONEDATA_TBL *fsdata = &tw->mdata.fstone_tbl;
		tw_MapDataFStoneDataInitMain( buf, fsdata );
		
		#ifdef DEBUG_TW_PRINTF
		OS_Printf( "mdata main fstone flag init =%xH\n",
				fsdata->head_p->init_flag );
		OS_Printf( "mdata main fstone putpos init max =%d\n",
				fsdata->head_p->put_max );
		OS_Printf( "mdata main fstone flagpos init max=%d\n",
				fsdata->head_p->flagpos_max );
		#endif
	}
}
//--------------------------------------------------------------
/**
 * 飛び石データ　初期化　下層
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_MapDataFStoneDataInitLower( TORNWORLD_WORK *tw )
{
	if( tw_ZoneDataBufFStoneDataSizeGetLower(tw) ){		//下層
		TW_ZONEDATA_BUF *buf = &tw->mdata.zone_buf_lower;
		TW_FSTONEDATA_TBL *fsdata = &tw->mdata.fstone_tbl_lower;
		tw_MapDataFStoneDataInitMain( buf, fsdata );
		
		#ifdef DEBUG_TW_PRINTF
		OS_Printf( "mdata lower fstone flag init =%xH\n",
				fsdata->head_p->init_flag );
		OS_Printf( "mdata lower fstone putpos init max =%d\n",
				fsdata->head_p->put_max );
		OS_Printf( "mdata lower fstone flagpos init max=%d\n",
				fsdata->head_p->flagpos_max );
		#endif
	}
}

//--------------------------------------------------------------
/**
 * 飛び石データ　初期化
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_MapDataFStoneDataInit( TORNWORLD_WORK *tw )
{
	tw_MapDataFStoneDataInitNow( tw );
	tw_MapDataFStoneDataInitLower( tw );
}	

//--------------------------------------------------------------
/**
 * 飛び石データ　クリア
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_MapDataFStoneDataClean( TW_FSTONEDATA_TBL *fsdata )
{
	memset( fsdata, 0, sizeof(TW_FSTONEDATA_TBL) );
}

//--------------------------------------------------------------
/**
 * 飛び石データ　削除　現在層
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_MapDataFStoneDataDeleteNow( TORNWORLD_WORK *tw )
{
	TW_FSTONEDATA_TBL *fsdata = &tw->mdata.fstone_tbl;
	tw_MapDataFStoneDataClean( fsdata );
}

//--------------------------------------------------------------
/**
 * 飛び石データ　削除　下層
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_MapDataFStoneDataDeleteLower( TORNWORLD_WORK *tw )
{
	TW_FSTONEDATA_TBL *fsdata = &tw->mdata.fstone_tbl_lower;
	tw_MapDataFStoneDataClean( fsdata );
}

//--------------------------------------------------------------
/**
 * 飛び石データ　削除
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_MapDataFStoneDataDelete( TORNWORLD_WORK *tw )
{
	tw_MapDataFStoneDataDeleteNow( tw );
	tw_MapDataFStoneDataDeleteLower( tw );
}

//--------------------------------------------------------------
/**
 * 飛び石データ　現在層から下層へデータ移動
 * @param	buf	TW_ZONEDATA_BUF
 * @param	fsdata	TW_FSTONEDATA_TBL
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_MapDataFStoneDataRemoveNowLower( TORNWORLD_WORK *tw )
{
	#ifdef TW_DEBUG
	{
		TW_FSTONEDATA_TBL *fsdata = &tw->mdata.fstone_tbl_lower;
		GF_ASSERT( fsdata->head_p == NULL );
		GF_ASSERT( fsdata->putpos_tbl_p == NULL );
		GF_ASSERT( fsdata->flagpos_tbl_p == NULL );
	}
	#endif
	tw->mdata.fstone_tbl_lower = tw->mdata.fstone_tbl;
	tw_MapDataFStoneDataClean( &tw->mdata.fstone_tbl );
}

//--------------------------------------------------------------
/**
 * 飛び石データ　下層から現在層へデータ移動
 * @param	buf	TW_ZONEDATA_BUF
 * @param	fsdata	TW_FSTONEDATA_TBL
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_MapDataFStoneDataRemoveLowerNow( TORNWORLD_WORK *tw )
{
	#ifdef TW_DEBUG
	{
		TW_FSTONEDATA_TBL *fsdata = &tw->mdata.fstone_tbl;
		GF_ASSERT( fsdata->head_p == NULL );
		GF_ASSERT( fsdata->putpos_tbl_p == NULL );
		GF_ASSERT( fsdata->flagpos_tbl_p == NULL );
	}
	#endif
	tw->mdata.fstone_tbl = tw->mdata.fstone_tbl_lower;
	tw_MapDataFStoneDataClean( &tw->mdata.fstone_tbl_lower );
}

//--------------------------------------------------------------
/**
 * 飛び石データ　TW_FSTONEDATA_HEADER取得
 * @param	tw	TORNWORLD_WORK
 * @retval	TW_FSTONEDATA_TBL
 */
//--------------------------------------------------------------
static const TW_FSTONE_HEADER * tw_MapDataFStoneHeaderGet(
		TORNWORLD_WORK *tw )
{
	TW_FSTONEDATA_TBL *fsdata = &tw->mdata.fstone_tbl;
	return( fsdata->head_p );
}

//--------------------------------------------------------------
/**
 * 飛び石データ　TW_FSTONEDATA_HEADER取得　下層
 * @param	tw	TORNWORLD_WORK
 * @retval	TW_FSTONEDATA_TBL
 */
//--------------------------------------------------------------
static const TW_FSTONE_HEADER * tw_MapDataFStoneHeaderGetLower(
		TORNWORLD_WORK *tw )
{
	TW_FSTONEDATA_TBL *fsdata = &tw->mdata.fstone_tbl_lower;
	return( fsdata->head_p );
}

//--------------------------------------------------------------
/**
 * 飛び石データ　TW_FSTONE_PUTPOS取得
 * @param	tw	TORNWORLD_WORK
 * @retval	TW_FSTONE_PUTPOS
 */
//--------------------------------------------------------------
static const TW_FSTONE_PUTPOS * tw_MapDataFStonePutPosTblGet(
		TORNWORLD_WORK *tw )
{
	TW_FSTONEDATA_TBL *fsdata = &tw->mdata.fstone_tbl;
	return( fsdata->putpos_tbl_p );
}

//--------------------------------------------------------------
/**
 * 飛び石データ　TW_FSTONE_PUTPOS取得　下層
 * @param	tw	TORNWORLD_WORK
 * @retval	TW_FSTONE_PUTPOS
 */
//--------------------------------------------------------------
static const TW_FSTONE_PUTPOS * tw_MapDataFStonePutPosTblGetLower(
		TORNWORLD_WORK *tw )
{
	TW_FSTONEDATA_TBL *fsdata = &tw->mdata.fstone_tbl_lower;
	return( fsdata->putpos_tbl_p );
}

//--------------------------------------------------------------
/**
 * 飛び石データ　TW_FSTONE_FLAGPOS取得
 * @param	tw	TORNWORLD_WORK
 * @retval	TW_FSTONE_PUTPOS
 */
//--------------------------------------------------------------
static const TW_FSTONE_FLAGPOS * tw_MapDataFStoneFlagPutTblGet(
		TORNWORLD_WORK *tw )
{
	TW_FSTONEDATA_TBL *fsdata = &tw->mdata.fstone_tbl;
	return( fsdata->flagpos_tbl_p );
}

//--------------------------------------------------------------
/**
 * 飛び石データ　TW_FSTONE_FLAGPOS取得　下層
 * @param	tw	TORNWORLD_WORK
 * @retval	TW_FSTONE_PUTPOS
 */
//--------------------------------------------------------------
static const TW_FSTONE_FLAGPOS * tw_MapDataFStoneFlagPutTblGetLower(
		TORNWORLD_WORK *tw )
{
	TW_FSTONEDATA_TBL *fsdata = &tw->mdata.fstone_tbl_lower;
	return( fsdata->flagpos_tbl_p );
}

//==============================================================================
//	ダミーマップ
//==============================================================================
#if 0
//--------------------------------------------------------------
/**
 * ダミーマップ初期化　メイン
 * @param	tw	TORNWORLD_WORK
 * @param	dmy_zone_id	展開するZONE ID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_DmyMapInitMain( TORNWORLD_WORK *tw, int dmy_zone_id )
{
	TW_DMYMAP *dmap = &tw->dmy_map;
	FIELDSYS_WORK *fsys = tw->fsys;
	const TW_ZONE_LINKDATA *d_link;
	
	#ifdef DEBUG_TW_PRINTF
	{
		u32 a_free = sys_GetHeapFreeSize( HEAPID_FIELD );
		OS_Printf("TW DummyMapLoadStart FieldHeapMapMem %08xH\n",a_free);
		a_free = sys_GetHeapFreeSize( HEAPID_WORLD );
		OS_Printf("TW DummyMapLoadStart WorldHeapMapMem %08xH\n",a_free);
	}
	#endif
	
	GF_ASSERT( dmy_zone_id == ZONE_ID_MAX );
	
	d_link = tw_ZoneLinkDataGet( dmy_zone_id );
	dmap->zone_id = dmy_zone_id;
	
	/*
	#ifdef DEBUG_TW_PRINTF_FORCE
	OS_Printf( "tworld zone id = %04xH,", link->zone_id );
	OS_Printf( "lower zone id = %04xH,", link->lower_zone_id );
	OS_Printf( "dummy zone id = %04xH\n", d_link->zone_id );
	#endif
	*/
	
	{	//ワールドマップ初期化
		dmap->world = WorldMapInitHeap( HEAPID_FIELD );
		SetUpWorldMatrix( dmy_zone_id, dmap->world );
	}
	
	{	//マップリソース初期化
#ifdef TWORLD_MAPRES_INIT
		u16 area_id = ZoneData_GetAreaID( dmy_zone_id );
		dmap->map_res = AllocMapResource( area_id, fsys->field_3d_anime );
		SetupMapResource( dmap->map_res );
#else
		dmap->map_res = fsys->MapResource;
#endif
	}
	
	{	//マップ管理データ初期化
		ARCHANDLE *arc = GetDivMapArcHandle( fsys->map_cont_dat );
		dmap->map_cont = InitDivMapDummy( dmap->world, dmap->map_res, arc );
		DivMapBindTarget( Player_VecPosPtrGet(fsys->player), dmap->map_cont );
	}
	
	{	//ブロックモードを2x2,配置モデルをOFFに
		SetDivMapBlockMode( dmap->map_cont, BLOCKMODE_2x2 );
		SetDivMapM3dObjSetFlag( dmap->map_cont, DIVM3DO_OFF );
	}
	
	{	//始点設定
		int x,y,z;
		tw_MapDataIdxZoneIDOriginGridPosGet( tw, dmy_zone_id, &x, &y, &z );
		SetDivMapOriginGridPos( dmap->map_cont, x, y, z );
	}
	
	{	//ロード
		int gx = Player_NowGPosXGet( fsys->player );
		int gz = Player_NowGPosZGet( fsys->player );
		LoadDivMapDummy( dmap->map_cont, gx, gz );
		dmap->draw_flag = TRUE;
	}
	
	#ifdef DEBUG_TW_PRINTF
	{
		u32 a_free = sys_GetHeapFreeSize( HEAPID_FIELD );
		OS_Printf("TW DummyMapLoadEnd FieldHeapMapMem %08xH\n",a_free);
		a_free = sys_GetHeapFreeSize( HEAPID_WORLD );
		OS_Printf("TW DummyMapLoadEnd WorldHeapMapMem %08xH\n",a_free);
	}
	#endif
	
	dmap->init_flag = TRUE;
	
	{	//オフセット高さを自機に設定
		VecFx32 org,pos;
		FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( fsys->player );
		FieldOBJ_VecPosGet( fldobj, &pos );
		GetDivMapOriginPos( fsys->map_cont_dat, &org );
		pos.y = org.y + GRID_SIZE_FX32( 1 );
		FieldOBJ_VecPosSet( fldobj, &pos );
		FieldOBJ_OldPosGY_Set( fldobj, FieldOBJ_NowPosGY_Get(fldobj) );
		FieldOBJ_NowPosGY_Set( fldobj, SIZE_H_GRID_FX32(pos.y) );
	}
}
#endif

//--------------------------------------------------------------
/**
 * ダミーマップ初期化
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_DmyMapInit( TORNWORLD_WORK *tw )
{
	int now_zone_id,dmy_zone_id;
	TW_DMYMAP *dmap = &tw->dmy_map;
	FIELDSYS_WORK *fsys = tw->fsys;
	const TW_ZONE_LINKDATA *link,*d_link;
	
	#ifdef DEBUG_TW_PRINTF
	{
		u32 a_free = sys_GetHeapFreeSize( HEAPID_FIELD );
		OS_Printf("TW DummyMapLoadStart FieldHeapMapMem %08xH\n",a_free);
		a_free = sys_GetHeapFreeSize( HEAPID_WORLD );
		OS_Printf("TW DummyMapLoadStart WorldHeapMapMem %08xH\n",a_free);
	}
	#endif
	
	now_zone_id = twNowZoneID( tw );
	link = tw_ZoneLinkDataGet( now_zone_id );
	dmy_zone_id = link->lower_zone_id;
	
	if( dmy_zone_id == ZONE_ID_MAX ){	//下層が存在しない
		return;
	}
	
	d_link = tw_ZoneLinkDataGet( dmy_zone_id );
	dmap->zone_id = dmy_zone_id;
	
	#ifdef DEBUG_TW_PRINTF_FORCE
	OS_Printf( "tworld zone id = %04xH,", link->zone_id );
	OS_Printf( "lower zone id = %04xH,", link->lower_zone_id );
	OS_Printf( "dummy zone id = %04xH\n", d_link->zone_id );
	#endif
	
	{	//ワールドマップ初期化
		dmap->world = WorldMapInitHeap( HEAPID_FIELD );
		SetUpWorldMatrix( dmy_zone_id, dmap->world );
	}
	
	{	//マップリソース初期化
#ifdef TWORLD_MAPRES_INIT
		u16 area_id = ZoneData_GetAreaID( dmy_zone_id );
		dmap->map_res = AllocMapResource( area_id, fsys->field_3d_anime );
		SetupMapResource( dmap->map_res );
#else
		dmap->map_res = fsys->MapResource;
#endif
	}
	
	{	//マップ管理データ初期化
		ARCHANDLE *arc = GetDivMapArcHandle( fsys->map_cont_dat );
		dmap->map_cont = InitDivMapDummy( dmap->world, dmap->map_res, arc );
		DivMapBindTarget( Player_VecPosPtrGet(fsys->player), dmap->map_cont );
	}
	
	{	//ブロックモードを2x2,配置モデルをOFFに
		SetDivMapBlockMode( dmap->map_cont, BLOCKMODE_2x2 );
		SetDivMapM3dObjSetFlag( dmap->map_cont, DIVM3DO_OFF );
	}
	
	{	//始点設定
		int x,y,z;
		tw_MapDataIdxZoneIDOriginGridPosGet( tw, dmy_zone_id, &x, &y, &z );
		SetDivMapOriginGridPos( dmap->map_cont, x, y, z );
	}
	
	{	//ロード
		int gx = Player_NowGPosXGet( fsys->player );
		int gz = Player_NowGPosZGet( fsys->player );
		LoadDivMapDummy( dmap->map_cont, gx, gz );
		dmap->draw_flag = TRUE;
	}
	
	#ifdef DEBUG_TW_PRINTF
	{
		u32 a_free = sys_GetHeapFreeSize( HEAPID_FIELD );
		OS_Printf("TW DummyMapLoadEnd FieldHeapMapMem %08xH\n",a_free);
		a_free = sys_GetHeapFreeSize( HEAPID_WORLD );
		OS_Printf("TW DummyMapLoadEnd WorldHeapMapMem %08xH\n",a_free);
	}
	#endif
	
	dmap->init_flag = TRUE;
	
	//初回の場合はオフセット高さを自機に設定
	if( tw_SaveDataInitFinishCheck(tw) == FALSE ){
		VecFx32 org,pos;
		FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( fsys->player );
		FieldOBJ_VecPosGet( fldobj, &pos );
		GetDivMapOriginPos( fsys->map_cont_dat, &org );
		pos.y = org.y + GRID_SIZE_FX32( 1 );
		FieldOBJ_VecPosSet( fldobj, &pos );
		FieldOBJ_OldPosGY_Set( fldobj, FieldOBJ_NowPosGY_Get(fldobj) );
		FieldOBJ_NowPosGY_Set( fldobj, SIZE_H_GRID_FX32(pos.y) );
	}
}

//--------------------------------------------------------------
/**
 * ダミーマップ削除
 * @param	tw	TORNWORLD_WORK
 * @param	zone_id	ZONE ID
 * @param	gx	読み込む中心位置GX
 * @param	gz	読み込む中心位置GZ
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_DmyMapDelete( TORNWORLD_WORK *tw )
{
	TW_DMYMAP *dmap = &tw->dmy_map;
	
	if( dmap->init_flag ){
		DivMapPurgeTarget( dmap->map_cont );
		DivMapLoad_FreeMapDummy( dmap->map_cont );
		
		#ifdef TWORLD_MAPRES_INIT	
		if( dmap->map_res != NULL ){
			ReleaseMapResource( &dmap->map_res );
			dmap->map_res = NULL;
		}
		#else
		if( dmap->map_res != NULL ){
			dmap->map_res = NULL;
		}
		#endif
		
		if( dmap->map_cont != NULL ){
			FreeDivMapContDummy( dmap->map_cont );
			dmap->map_cont = NULL;
		}
		
		if( dmap->world != NULL ){
			WorldMapRelease( dmap->world );
			dmap->world = NULL;
		}
		
		dmap->init_flag = FALSE;
	}
}

//--------------------------------------------------------------
/**
 * ダミーマップ描画
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_DmyMapDraw( TORNWORLD_WORK *tw )
{
	TW_DMYMAP *dmap = &tw->dmy_map;
	
	if( dmap->draw_flag ){
//		VecFx32 offs = {0,GRID_SIZE_FX32(-32),0};
		DrawAroundBlockDummy( dmap->map_cont, tw->fsys->glst_data );
	}
}

//--------------------------------------------------------------
/**
 * ダミーマップ描画
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
void FldTornWorld_Draw( FIELDSYS_WORK *fsys )
{
	TORNWORLD_WORK *tw = fsys->fldmap->Work;
	tw_DmyMapDraw( tw );
}

//--------------------------------------------------------------
/**
 * ダミーマップ更新
 * @param
 * @retval
 */
//--------------------------------------------------------------
void FldTornWorld_MapUpdate( FIELDSYS_WORK *fsys )
{
	TORNWORLD_WORK *tw = fsys->fldmap->Work;
	TW_DMYMAP *dmap = &tw->dmy_map;
	
	{
		TW_ZONECHG *zchg = &tw->old_zone_chg;
		if( zchg->zchg_type == TWZONECHGTYPE_UP ){
			tw_DmyMapZoneChangeUpTask( tw );
		}else if( zchg->zchg_type == TWZONECHGTYPE_DOWN ){
			tw_DmyMapZoneChangeDownTask( tw );
		}
	}
	
	if( dmap->draw_flag == TRUE ){
		DivMapLoadMainDummy( fsys, dmap->map_cont );
	}
}

//--------------------------------------------------------------
/**
 * ダミーマップ　ゾーンチェンジ　下降タイプ
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_DmyMapZoneChgDown( TORNWORLD_WORK *tw, TW_LINKPOS_ACTOR *lact )
{
	FIELDSYS_WORK *fsys = tw->fsys;
	TW_DMYMAP *dmap = &tw->dmy_map;
	u32 zone_id = twNowZoneID( tw );
	const TW_ZONE_LINKDATA *link = tw_ZoneLinkDataGet( zone_id );
	GF_ASSERT( link->lower_zone_id != ZONE_ID_MAX );
	link = tw_ZoneLinkDataGet( link->lower_zone_id );	//下層へ
	
	#ifdef DEBUG_TW_PRINTF
	OS_Printf( "ダミーマップ 下降ゾーンチェンジ 開始\n" );
	#endif
	
	//描画停止
	dmap->draw_flag;
	
	//フィールドマップ　ゾーン切り替え
	FieldMap_TornWorldZoneChange( fsys, link->zone_id );
	
	//DMC_PTR 入れ替えの為のリセット
	DivMapLoad_RemoveReset( fsys->map_cont_dat );
	
	{	//ダミーマップから現マップへの入れ替え
		int gx = Player_NowGPosXGet( fsys->player );
		int gz = Player_NowGPosZGet( fsys->player );
		
		RemoveDivMapDummyMain(
			dmap->world,
			dmap->map_res,
			dmap->map_cont, fsys->map_cont_dat, gx, gz );
		dmap->draw_flag = FALSE;
	}
	
	//ダミーWORLD_MAP破棄
	WorldMapRelease( dmap->world );
	dmap->world = NULL;
	
	//現在のマップリソースを破棄してダミーマップから差し替え
	ReleaseMapResource( &fsys->MapResource );
	fsys->MapResource = dmap->map_res;
	dmap->map_res = NULL;
	
	//下層マップロード
	dmap->zone_id = link->lower_zone_id;
	
	if( link->lower_zone_id != ZONE_ID_MAX ){	//下マップ展開
		link = tw_ZoneLinkDataGet( link->lower_zone_id );
		dmap->world = WorldMapInitHeap( HEAPID_FIELD );
		SetUpWorldMatrix( link->zone_id, dmap->world );
		
		#ifdef DEBUG_TW_PRINTF
		OS_Printf( "ダミーマップ 下降 マップリソース再セット\n" );
		#endif
		
		{
			u16 area_id = ZoneData_GetAreaID( link->zone_id );
			dmap->map_res = AllocMapResource( area_id, fsys->field_3d_anime );
			SetupMapResource( dmap->map_res );
		}
		
		#ifdef DEBUG_TW_PRINTF
		OS_Printf( "ダミーマップ 下降 マップリソース再セット完了\n" );
		#endif
		
		{	//再初期化
			ARCHANDLE *arc = GetDivMapArcHandle( fsys->map_cont_dat );
			InitDivMapDummyContinue(
				dmap->map_cont, dmap->world, dmap->map_res, arc );
		}
		
		{	//始点、ブロック設定
			int x,y,z;
			tw_MapDataIdxZoneIDOriginGridPosGet(
						tw, dmap->zone_id, &x, &y, &z );
			SetDivMapOriginGridPos( dmap->map_cont, x, y, z );
			SetDivMapBlockMode( dmap->map_cont, BLOCKMODE_2x2 );
			SetDivMapM3dObjSetFlag( dmap->map_cont, DIVM3DO_OFF );
		}
		
		#if 0
		{
			int gx = Player_NowGPosXGet( fsys->player );
			int gz = Player_NowGPosZGet( fsys->player );
			ReloadDivMapDummy( dmap->map_cont, gx, gz );
			dmap->draw_flag = TRUE;
		}
		#else
		{
			int gx = Player_NowGPosXGet( fsys->player );
			int gz = Player_NowGPosZGet( fsys->player );
			tw_DmyMapZoneChgDownTaskSet( tw, gx, gz, lact );
		}
		#endif
	}
	
	DEBUG_twFldHeapPrintf();
	
	#ifdef DEBUG_TW_PRINTF
	OS_Printf( "ダミーマップ　下降ゾーンチェンジ セットアップ完了\n" );
	#endif
}

//--------------------------------------------------------------
/**
 * ダミーマップ　ゾーンチェンジ　上昇タイプ
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_DmyMapZoneChgUp(
		TORNWORLD_WORK *tw, TW_LINKPOS_ACTOR *lact )
{
	FIELDSYS_WORK *fsys = tw->fsys;
	TW_DMYMAP *dmap = &tw->dmy_map;
	u32 zone_id = twNowZoneID( tw );
	const TW_ZONE_LINKDATA *link = tw_ZoneLinkDataGet( zone_id );
	GF_ASSERT( link->upper_zone_id != ZONE_ID_MAX );
	link = tw_ZoneLinkDataGet( link->upper_zone_id );	//上層リンク
	
	#ifdef DEBUG_TW_PRINTF
	OS_Printf( "ダミーマップ　上昇ゾーンチェンジ セットアップ開始\n" );
	#endif
	
	//描画停止
	dmap->draw_flag = FALSE;
	
	//フィールドマップ　ゾーン切り替え
	FieldMap_TornWorldZoneChange( fsys, link->zone_id );
	
	{	//ダミーマップ初期化チェック
		if( dmap->init_flag == FALSE ){
			//初期化処理
		}
	}
	
	{	//ダミーマップ　配置モデルクリーン
		if( dmap->init_flag == TRUE ){
			DivMapLoad_CleanM3DObj( dmap->map_cont );
		}
	}
	
	{	//メインマップ　配置モデルクリーン
		DivMapLoad_CleanM3DObj( fsys->map_cont_dat );
	}
	
	{	//ワールド生成チェック
		if( dmap->world == NULL ){
			dmap->world = WorldMapInitHeap( HEAPID_FIELD );
		}
		
		if( dmap->map_cont == NULL ){
			ARCHANDLE *arc = GetDivMapArcHandle( fsys->map_cont_dat );
			dmap->map_cont = InitDivMapDummy(
				dmap->world, dmap->map_res, arc );
		}else{
			SetDivMapWorldMap( dmap->map_cont, dmap->world );
		}
	}
	
	{	//ダミーマップリソースを破棄してメインを利用
		if( dmap->map_res != NULL ){
			ReleaseMapResource( &dmap->map_res );
		}
		
		dmap->map_res = fsys->MapResource;
		fsys->MapResource = NULL;
	}
	
	{	//メインマップをダミーマップへ移行する
		RemoveDivMapMainDummy( dmap->map_cont, fsys->map_cont_dat );
	}
	
	//上層マップ用WORLD_MAP初期化
	SetUpWorldMatrix( link->zone_id, fsys->World );
	
	#ifdef DEBUG_TW_PRINTF
	OS_Printf( "ダミーマップ 上昇 マップリソース再セット\n" );
	#endif
	
	//上層マップ用リソース初期化
	{
		u16 area_id = ZoneData_GetAreaID( link->zone_id );
		fsys->MapResource = AllocMapResource( area_id, fsys->field_3d_anime );
		SetupMapResource( fsys->MapResource );
	}
	
	#ifdef DEBUG_TW_PRINTF
	OS_Printf( "ダミーマップ 上昇 マップリソース再セット完了\n" );
	#endif
	
	//上位マップロード
	{
		int x = 0,y = 0,z = 0;
		ResetDivMap( fsys->map_cont_dat, fsys->World, fsys->MapResource );
		FldTornWorld_ArcDataMapOriginGridPosGet( link->zone_id, &x, &y, &z );
		SetDivMapOriginGridPos( fsys->map_cont_dat, x, y, z );
		SetDivMapBlockMode( fsys->map_cont_dat, BLOCKMODE_2x2 );
		SetDivMapM3dObjSetFlag( fsys->map_cont_dat, DIVM3DO_OFF );
		
		#if 0
		LoadDivMapNonAlloc( fsys->map_cont_dat,
			fsys->location->grid_x, fsys->location->grid_z );
		#else
		tw_DmyMapZoneChgUpTaskSet( tw,
			fsys->location->grid_x, fsys->location->grid_z,
			lact );
		#endif
	}
	
	dmap->draw_flag = TRUE;
	
	DEBUG_twFldHeapPrintf();
	
	#ifdef DEBUG_TW_PRINTF
	OS_Printf( "ダミーマップ　上昇ゾーンチェンジ セットアップ完了\n" );
	#endif
}

//--------------------------------------------------------------
/**
 * ダミーマップ　ゾーンチェンジ　下　タスク処理
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void tw_DmyMapZoneChangeDownTask( TORNWORLD_WORK *tw )
{
	TW_ZONECHG *work = &tw->old_zone_chg;
	TW_DMYMAP *dmap = &tw->dmy_map;
	
	switch( work->seq_no ){
	case 0:
		#ifdef DEBUG_TW_PRINTF
		OS_Printf( "破界ゾーンチェンジ下分割タスク　開始\n" );
		#endif
		
		LoadBlockListSetUpForDummy(
			dmap->map_cont, work->gx, work->gz, work->block_no_tbl );
		work->seq_no++;
		break;
	case 1:
		#ifdef DEBUG_TW_PRINTF
		OS_Printf("破界ゾーンチェンジ下分割タスク No.%d",work->list_count);
		#endif
		
		LoadBlockListNoForDummy( dmap->map_cont,
			work->list_count, work->block_no_tbl[work->list_count] );
		
		#ifdef DEBUG_TW_PRINTF
		OS_Printf( "完了\n" );
		#endif
		
		work->list_count++;
		
		if( work->list_count >= 4 ){
			work->seq_no++;
		}
		
		break;
	case 2:
		UpdateDivMapNowPos( dmap->map_cont, work->gx, work->gz );
		
		{
			int i = 0;
			do{
				SetDivMapDrawFlag( dmap->map_cont, i, TRUE );
				i++;
			}while( i < 4 );
		}
		
		#ifdef DEBUG_TW_PRINTF
		OS_Printf( "破世界ゾーンチェンジ下分割タスク 全て完了\n" );
		#endif
		
		tw_ZoneChangeDown( tw );
		
		if( work->lact != NULL ){
			tw_LinkPosActZoneChgDelNotFlagSet( work->lact, FALSE );
		}
		
		dmap->draw_flag = TRUE;
		work->zchg_type = TWZONECHGTYPE_NON;
	}
}

//--------------------------------------------------------------
/**
 * ダミーマップ　ゾーンチェンジ　上　タスク処理
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void tw_DmyMapZoneChangeUpTask( TORNWORLD_WORK *tw )
{
	TW_ZONECHG *work = &tw->old_zone_chg;
	TW_DMYMAP *dmap = &tw->dmy_map;
	FIELDSYS_WORK *fsys = tw->fsys;
	
	switch( work->seq_no ){
	case 0:
		LoadBlockListSetUp(
			fsys->map_cont_dat, work->gx, work->gz, work->block_no_tbl );
		work->seq_no++;
		break;
	case 1:
		LoadBlockListNoNonAlloc( fsys->map_cont_dat,
			work->list_count, work->block_no_tbl[work->list_count] );
		
		work->list_count++;
		
		if( work->list_count >= 4 ){
			work->seq_no++;
		}
		
		break;
	case 2:	
		UpdateDivMapNowPos( fsys->map_cont_dat, work->gx, work->gz );
		
		{
			int i = 0;
			do{
				SetDivMapDrawFlag( fsys->map_cont_dat, i, TRUE );
				i++;
			}while( i < 4 );
		}
		
		tw_ZoneChangeUp( tw );
		
		if( work->lact != NULL ){
			tw_LinkPosActZoneChgDelNotFlagSet( work->lact, FALSE );
		}
		
		work->zchg_type = TWZONECHGTYPE_NON;
	}
}

//--------------------------------------------------------------
/**
 * 上移動　ゾーンチェンジタスクセット
 * @param	tw
 * @param	gx
 * @param	gz
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_DmyMapZoneChgUpTaskSet(
		TORNWORLD_WORK *tw, int gx, int gz, TW_LINKPOS_ACTOR *act )
{
	TW_ZONECHG *work = &tw->old_zone_chg;
	
	work->zchg_type = TWZONECHGTYPE_UP;
	work->gx = gx;
	work->gz = gz;
	work->seq_no = 0;
	work->list_count = 0;
	work->lact = act;
	
	tw_DmyMapZoneChangeUpTask( tw );
}

//--------------------------------------------------------------
/**
 * 下移動　ゾーンチェンジタスクセット
 * @param	tw
 * @param	gx
 * @param	gz
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_DmyMapZoneChgDownTaskSet(
		TORNWORLD_WORK *tw, int gx, int gz, TW_LINKPOS_ACTOR *act )
{
	TW_ZONECHG *work = &tw->old_zone_chg;
	
	work->zchg_type = TWZONECHGTYPE_DOWN;
	work->gx = gx;
	work->gz = gz;
	work->seq_no = 0;
	work->list_count = 0;
	work->lact = act;
	
	tw_DmyMapZoneChangeDownTask( tw );
}

//==============================================================================
//	ゾーンチェンジタスク
//==============================================================================
//--------------------------------------------------------------
///	ゾーンチェンジ下降処理番号
//--------------------------------------------------------------
enum
{
	SEQNO_ZONECHG_DOWN_0,
	SEQNO_ZONECHG_DOWN_1,
	SEQNO_ZONECHG_DOWN_2,
	SEQNO_ZONECHG_DOWN_3,
	SEQNO_ZONECHG_DOWN_4,
	SEQNO_ZONECHG_DOWN_MAX,
};

//--------------------------------------------------------------
///	ゾーンチェンジ上昇処理番号
//--------------------------------------------------------------
enum
{
	SEQNO_ZONECHG_UP_0,
	SEQNO_ZONECHG_UP_1,
	SEQNO_ZONECHG_UP_2,
	SEQNO_ZONECHG_UP_3,
	SEQNO_ZONECHG_UP_MAX,
};

static PROCRET (* const DATA_ZoneChangeDownTbl[SEQNO_ZONECHG_DOWN_MAX])( TORNWORLD_WORK *tw, TW_ZONECHG_WORK *work );
static PROCRET (* const DATA_ZoneChangeUpTbl[SEQNO_ZONECHG_UP_MAX])( TORNWORLD_WORK *tw, TW_ZONECHG_WORK *work );

//--------------------------------------------------------------
/**
 * DMC_PTR 描画フラグ全てセット
 * @param	dmc		DMC_PTR
 * @param	flag	TRUE=描画 FALSE=非表示
 * @retval	nothing
 */
//--------------------------------------------------------------
static void DmcDrawFlagSetAll( DMC_PTR dmc, BOOL flag )
{
	int i = 0;
	do{ SetDivMapDrawFlag(dmc,i++,flag); }while( i < 4 );
}

//--------------------------------------------------------------
/**
 * ゾーンチェンジタスク　初期化
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ZoneChangeTaskInit( TORNWORLD_WORK *tw )
{
	TW_ZONECHG_WORK *work = &tw->zone_chg;
	
	work->chg_type = TWZONECHGTYPE_NON;
	work->tcb = TCB_Add( tw_ZoneChangeTaskTCB, tw, TW_TCBPRI_ZONECHG );
}

//--------------------------------------------------------------
/**
 * ゾーンチェンジタスク　削除
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ZoneChangeTaskDelete( TORNWORLD_WORK *tw )
{
	TW_ZONECHG_WORK *work = &tw->zone_chg;
	TCB_Delete( work->tcb );
}

//--------------------------------------------------------------
/**
 * ゾーンチェンジタスク TCB
 * @param	tcb	TCB_PTR
 * @param	wk	tcb work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ZoneChangeTaskTCB( TCB_PTR tcb, void *wk )
{
	TORNWORLD_WORK *tw = wk;
	TW_ZONECHG_WORK *work = &tw->zone_chg;
	PROCRET (* const *tbl)(TORNWORLD_WORK*,TW_ZONECHG_WORK*) = NULL;
	
	if( work->chg_type == TWZONECHGTYPE_DOWN ){
		tbl = DATA_ZoneChangeDownTbl;
	}else if( work->chg_type == TWZONECHGTYPE_UP ){
		tbl = DATA_ZoneChangeUpTbl;
	}
	
	if( tbl != NULL ){
		PROCRET ret;
		
		do{
			ret = tbl[work->seq_no]( tw, work );
		}while( ret == RET_CONTINUE );
		
		if( ret == RET_END ){
			work->chg_type = TWZONECHGTYPE_NON;
		}
	}
}

//--------------------------------------------------------------
/**
 * ゾーンチェンジタスク　VIntr処理
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ZoneChangeTaskVIntr( TORNWORLD_WORK *tw )
{
	TW_ZONECHG_WORK *work = &tw->zone_chg;
	
	#ifdef TWORLD_MAPRES_INIT
	if( work->vintr_trans_flag == TRUE ){
		SetupMapResourceVram( work->map_res_trans_p );
		work->vintr_trans_flag = FALSE;
	}
	#endif
}

//--------------------------------------------------------------
/**
 * ゾーンチェンジタスク　起動
 * @param	tw		TORNWORLD_WORK
 * @param	lact	TW_LINKPOS_ACTOR
 * @param	type	TWZONECHGTYPE_DOWN等
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ZoneChangeTaskSet(
	TORNWORLD_WORK *tw, TW_LINKPOS_ACTOR *lact, u32 type )
{
	TW_ZONECHG_WORK *work = &tw->zone_chg;
	
	GF_ASSERT( work->chg_type == TWZONECHGTYPE_NON );
	GF_ASSERT( type != TWZONECHGTYPE_NON );
	
	work->chg_type = type;
	work->seq_no = 0;
	work->list_count = 0;
	work->lact = lact;
}

//--------------------------------------------------------------
/**
 * ゾーンチェンジタスク　起動中チェック
 * @param	tw	TORNWORLD_WORK
 * @retval	BOOL	TRUE=起動中
 */
//--------------------------------------------------------------
static BOOL tw_ZoneChangeTaskCheck( TORNWORLD_WORK *tw )
{
	TW_ZONECHG_WORK *work = &tw->zone_chg;
	
	if( work->chg_type != TWZONECHGTYPE_NON ){
		return( TRUE );
	}
	
	return( FALSE );
}

//==============================================================================
//	ゾーンチェンジ　下降処理
//==============================================================================
//--------------------------------------------------------------
/**
 * ゾーンチェンジ下降　開始		0F+1F
 * @param	tw	TORNWORLD_WORK
 * @param	work	TW_ZONECHG_WORK
 * @retval	PROCRET
 */
//--------------------------------------------------------------
static PROCRET tw_ZoneChgDownTask_0(
	TORNWORLD_WORK *tw, TW_ZONECHG_WORK *work )
{
	//消す対象を非表示に
	DmcDrawFlagSetAll( tw->fsys->map_cont_dat, FALSE );
	
	//ゾーン切り替えに伴う削除処理
	tw_ZoneChangeDeleteProcDown( tw, twNowZoneID(tw) );
	
	#ifdef DEBUG_TW_PRINTF
	OS_Printf( "ゾーンチェンジ下降タスク 開始\n" );
	#endif
	
	work->seq_no = SEQNO_ZONECHG_DOWN_1;
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ゾーンチェンジ下降　入れ替え		1F+1F
 * @param	tw	TORNWORLD_WORK
 * @param	work	TW_ZONECHG_WORK
 * @retval	PROCRET
 */
//--------------------------------------------------------------
static PROCRET tw_ZoneChgDownTask_1(
	TORNWORLD_WORK *tw, TW_ZONECHG_WORK *work )
{
	FIELDSYS_WORK *fsys = tw->fsys;
	TW_DMYMAP *dmap = &tw->dmy_map;
	const TW_ZONE_LINKDATA *link;
	
	//リンクデータ取得
	link = tw_ZoneLinkDataGet( twNowZoneID(tw) );
	GF_ASSERT( link->lower_zone_id != ZONE_ID_MAX );
	link = tw_ZoneLinkDataGet( link->lower_zone_id );	//下層へ
	
	//移動先のZONE ID
	work->next_zone_id = link->zone_id;
	
	//フィールドマップ　ゾーン切り替え
	FieldMap_TornWorldZoneChange( tw->fsys, work->next_zone_id );
	
	//マップデータ 下層ダミーを現在層へ移行
	tw_MapDataRemoveLowerNow( tw, link->lower_zone_id );
	
	//DMC_PTR 入れ替えの為のリセット
	DivMapLoad_RemoveReset( fsys->map_cont_dat );
	
	//ダミーマップから現マップへの入れ替え
	work->jiki_gx = Player_NowGPosXGet( fsys->player );
	work->jiki_gz = Player_NowGPosZGet( fsys->player );
	
	RemoveDivMapDummyMain( dmap->world,
		dmap->map_res, dmap->map_cont, fsys->map_cont_dat,
		work->jiki_gx, work->jiki_gz );
	
	//ダミーマップ非表示に
	dmap->draw_flag = FALSE;
	DmcDrawFlagSetAll( dmap->map_cont, FALSE );
	
	//ダミーWORLD_MAP破棄
	WorldMapRelease( dmap->world );
	dmap->world = NULL;
	
	//ダミー->メイン入れ替え完了。メインマップ表示
	DmcDrawFlagSetAll( tw->fsys->map_cont_dat, TRUE );
	
	//下層マップロード開始
	dmap->zone_id = link->lower_zone_id;
	
	if( dmap->zone_id != ZONE_ID_MAX ){	//下マップ展開
		link = tw_ZoneLinkDataGet( link->lower_zone_id );
		dmap->world = WorldMapInitHeap( HEAPID_FIELD );
		SetUpWorldMatrix( link->zone_id, dmap->world );
		work->seq_no = SEQNO_ZONECHG_DOWN_2;
	}else{										//下マップ無し
	//	tw_DmyMapDelete( tw );
		work->seq_no = SEQNO_ZONECHG_DOWN_4;
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ゾーンチェンジ下降　ロードブロックリストセットアップ	2F+0F
 * @param	tw	TORNWORLD_WORK
 * @param	work	TW_ZONECHG_WORK
 * @retval	PROCRET
 */
//--------------------------------------------------------------
static PROCRET tw_ZoneChgDownTask_2(
	TORNWORLD_WORK *tw, TW_ZONECHG_WORK *work )
{
	if( work->vintr_trans_flag == FALSE ){
		int x,y,z;
		ARCHANDLE *arc;
		TW_DMYMAP *dmap = &tw->dmy_map;
		
		//再初期化
		arc = GetDivMapArcHandle( tw->fsys->map_cont_dat );
		InitDivMapDummyContinue(
			dmap->map_cont, dmap->world, dmap->map_res, arc );
		
		//始点、ブロック設定
		tw_MapDataIdxZoneIDOriginGridPosGet(
					tw, dmap->zone_id, &x, &y, &z );
		SetDivMapOriginGridPos( dmap->map_cont, x, y, z );
		SetDivMapBlockMode( dmap->map_cont, BLOCKMODE_2x2 );
		SetDivMapM3dObjSetFlag( dmap->map_cont, DIVM3DO_OFF );
		
		//ロードブロックリストセットアップ
		LoadBlockListSetUpForDummy( dmap->map_cont,
			work->jiki_gx, work->jiki_gz, work->block_no_tbl );
		
		work->seq_no = SEQNO_ZONECHG_DOWN_3;
		return( RET_CONTINUE );
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ゾーンチェンジ下降　ダミーマップセット 2F+4F
 * @param	tw	TORNWORLD_WORK
 * @param	work	TW_ZONECHG_WORK
 * @retval	PROCRET
 */
//--------------------------------------------------------------
static PROCRET tw_ZoneChgDownTask_3(
	TORNWORLD_WORK *tw, TW_ZONECHG_WORK *work )
{
	TW_DMYMAP *dmap = &tw->dmy_map;
	
	LoadBlockListNoForDummy( dmap->map_cont,
		work->list_count, work->block_no_tbl[work->list_count] );
	
	work->list_count++;
		
	if( work->list_count >= 4 ){
		work->seq_no = SEQNO_ZONECHG_DOWN_4;
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ゾーンチェンジ下降　ダミーマップ展開完了 6F+1F total 7F(起動含み8Fになる
 * @param	tw	TORNWORLD_WORK
 * @param	work	TW_ZONECHG_WORK
 * @retval	PROCRET
 */
//--------------------------------------------------------------
static PROCRET tw_ZoneChgDownTask_4(
		TORNWORLD_WORK *tw, TW_ZONECHG_WORK *work )
{
	const TW_ZONE_LINKDATA *link;
	TW_DMYMAP *dmap = &tw->dmy_map;
	
	//ダミーマップ座標更新と表示
	if( dmap->zone_id != ZONE_ID_MAX ){
		dmap->draw_flag = TRUE;
		DmcDrawFlagSetAll( dmap->map_cont, TRUE );
		UpdateDivMapNowPos( dmap->map_cont, work->jiki_gx, work->jiki_gz );
	}
	
	//TORNWORLDゾーンチェンジによる追加処理
	link = tw_ZoneLinkDataGet( work->next_zone_id );
	tw_ZoneChangeAddProcDown( tw, link->lower_zone_id );
	
	//リンクアクター　ゾーン削除無効フラグを落とす
	if( work->lact != NULL ){
		tw_LinkPosActZoneChgDelNotFlagSet( work->lact, FALSE );
	}
	
	return( RET_END );
}

//--------------------------------------------------------------
///	ゾーンチェンジ下降処理テーブル
//--------------------------------------------------------------
static PROCRET (* const DATA_ZoneChangeDownTbl[SEQNO_ZONECHG_DOWN_MAX])( TORNWORLD_WORK *tw, TW_ZONECHG_WORK *work ) =
{
	tw_ZoneChgDownTask_0,
	tw_ZoneChgDownTask_1,
	tw_ZoneChgDownTask_2,
	tw_ZoneChgDownTask_3,
	tw_ZoneChgDownTask_4,
};

//==============================================================================
//	ゾーンチェンジ　上昇処理
//==============================================================================
//--------------------------------------------------------------
/**
 * ゾーンチェンジ　上昇開始		0F+1F
 * @param	tw	TORNWORLD_WORK
 * @param	work	TW_ZONECHG_WORK
 * @retval	PROCRET
 */
//--------------------------------------------------------------
static PROCRET tw_ZoneChgUpTask_0(
		TORNWORLD_WORK *tw, TW_ZONECHG_WORK *work )
{
	const TW_ZONE_LINKDATA *link;
	TW_DMYMAP *dmap = &tw->dmy_map;
	
	//消す対象を非表示に
	dmap->draw_flag = FALSE;
	
	if( dmap->init_flag == TRUE ){
		DmcDrawFlagSetAll( dmap->map_cont, FALSE );
	}
	
	//ゾーン切り替えに伴う削除処理
	link = tw_ZoneLinkDataGet( twNowZoneID(tw) );
	tw_ZoneChangeDeleteProcUp( tw, link->lower_zone_id );
	
	#ifdef DEBUG_TW_PRINTF
	OS_Printf( "ゾーンチェンジ下降タスク 開始\n" );
	#endif
	
	work->seq_no = SEQNO_ZONECHG_UP_1;
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ゾーンチェンジ　入れ替え		1F+1F
 * @param	tw	TORNWORLD_WORK
 * @param	work	TW_ZONECHG_WORK
 * @retval	PROCRET
 */
//--------------------------------------------------------------
static PROCRET tw_ZoneChgUpTask_1(
		TORNWORLD_WORK *tw, TW_ZONECHG_WORK *work )
{
	FIELDSYS_WORK *fsys = tw->fsys;
	TW_DMYMAP *dmap = &tw->dmy_map;
	const TW_ZONE_LINKDATA *link;
	
	//リンクデータ取得
	link = tw_ZoneLinkDataGet( twNowZoneID(tw) );
	GF_ASSERT( link->upper_zone_id != ZONE_ID_MAX );
	link = tw_ZoneLinkDataGet( link->upper_zone_id );	//上層リンク
	
	//移動先のZONE ID
	work->next_zone_id = link->zone_id;
	
	//フィールドマップ　ゾーン切り替え
	FieldMap_TornWorldZoneChange( tw->fsys, work->next_zone_id );
	
	//マップデータ　旧現在層を下層ダミーへ移行
	tw_MapDataRemoveNowLower( tw, work->next_zone_id );
	
	//ワールド生成チェック
	if( dmap->world == NULL ){
		dmap->world = WorldMapInitHeap( HEAPID_FIELD );
	}
	
	//マップリソース設定
	if( dmap->map_res == NULL ){
		dmap->map_res = fsys->MapResource;
	}
	
	//ダミーマップ初期化チェック
	if( dmap->init_flag == FALSE ){
		ARCHANDLE *arc = GetDivMapArcHandle( fsys->map_cont_dat );
		dmap->map_cont = InitDivMapDummy( NULL, dmap->map_res, arc );
		AllocOnlyDivMapBlockList( dmap->map_cont );
	}
	
	//ダミーマップ　配置モデルクリーン
	if( dmap->init_flag == TRUE ){
		DivMapLoad_CleanM3DObj( dmap->map_cont );
	}
	
	//メインマップ　配置モデルクリーン
	DivMapLoad_CleanM3DObj( fsys->map_cont_dat );
	
	//ワールドマップセット
	SetDivMapWorldMap( dmap->map_cont, dmap->world );
	
	//ダミーマップリソースを破棄してメインを利用
	#if 0
	if( dmap->map_res != NULL ){
		ReleaseMapResource( &dmap->map_res );
	}
		
	dmap->map_res = fsys->MapResource;
	fsys->MapResource = NULL;
	#endif
	
	//メインマップをダミーマップへ移行する
	RemoveDivMapMainDummy( dmap->map_cont, fsys->map_cont_dat );
	
	//入れ替え後のダミーマップ表示
	dmap->init_flag = TRUE;
	dmap->draw_flag = TRUE;
	DmcDrawFlagSetAll( dmap->map_cont, TRUE );
	
	//ロードする上層マップを非表示に
	DmcDrawFlagSetAll( tw->fsys->map_cont_dat, FALSE );
	
	//上層マップ用WORLD_MAP初期化
	SetUpWorldMatrix( link->zone_id, fsys->World );
	
	#if 0
	//上層マップ用リソース初期化
	{
		u16 area_id = ZoneData_GetAreaID( link->zone_id );
		fsys->MapResource = AllocMapResource( area_id, fsys->field_3d_anime );
		SetupMapResource( fsys->MapResource );
	}
	#endif
	
	//上位マップロードセットアップ
	{
		int x = 0,y = 0,z = 0;
		ResetDivMap( fsys->map_cont_dat, fsys->World, fsys->MapResource );
		FldTornWorld_ArcDataMapOriginGridPosGet( link->zone_id, &x, &y, &z );
		SetDivMapOriginGridPos( fsys->map_cont_dat, x, y, z );
		SetDivMapBlockMode( fsys->map_cont_dat, BLOCKMODE_2x2 );
		SetDivMapM3dObjSetFlag( fsys->map_cont_dat, DIVM3DO_OFF );
		work->jiki_gx = fsys->location->grid_x;
		work->jiki_gz = fsys->location->grid_z;
		
		LoadBlockListSetUp( fsys->map_cont_dat,
			work->jiki_gx, work->jiki_gz, work->block_no_tbl );
	}
	
	work->seq_no = SEQNO_ZONECHG_UP_2;
	return( RET_NON );
}

#if 0	//旧　dummyが存在していないケース未対処
static PROCRET tw_ZoneChgUpTask_1(
		TORNWORLD_WORK *tw, TW_ZONECHG_WORK *work )
{
	FIELDSYS_WORK *fsys = tw->fsys;
	TW_DMYMAP *dmap = &tw->dmy_map;
	const TW_ZONE_LINKDATA *link;
	
	//リンクデータ取得
	link = tw_ZoneLinkDataGet( twNowZoneID(tw) );
	GF_ASSERT( link->upper_zone_id != ZONE_ID_MAX );
	link = tw_ZoneLinkDataGet( link->upper_zone_id );	//上層リンク
	
	//移動先のZONE ID
	work->next_zone_id = link->zone_id;
	
	//フィールドマップ　ゾーン切り替え
	FieldMap_TornWorldZoneChange( tw->fsys, work->next_zone_id );
	
	//マップデータ　旧現在層を下層ダミーへ移行
	tw_MapDataRemoveNowLower( tw, work->next_zone_id );
	
	//ダミーマップ初期化チェック
	if( dmap->init_flag == FALSE ){
	}
	
	//ダミーマップ　配置モデルクリーン
	if( dmap->init_flag == TRUE ){
		DivMapLoad_CleanM3DObj( dmap->map_cont );
	}
	
	//メインマップ　配置モデルクリーン
	DivMapLoad_CleanM3DObj( fsys->map_cont_dat );
	
	//ワールド生成チェック
	if( dmap->world == NULL ){
		dmap->world = WorldMapInitHeap( HEAPID_FIELD );
	}
	
	if( dmap->map_res == NULL ){
		dmap->map_res = fsys->MapResource;
	}
	
	if( dmap->map_cont == NULL ){
		ARCHANDLE *arc = GetDivMapArcHandle( fsys->map_cont_dat );
		dmap->map_cont = InitDivMapDummy(
			dmap->world, dmap->map_res, arc );
		dmap->init_flag = TRUE;
	}else{
		SetDivMapWorldMap( dmap->map_cont, dmap->world );
	}
	
	//ダミーマップリソースを破棄してメインを利用
	#if 0
	if( dmap->map_res != NULL ){
		ReleaseMapResource( &dmap->map_res );
	}
		
	dmap->map_res = fsys->MapResource;
	fsys->MapResource = NULL;
	#endif
	
	//メインマップをダミーマップへ移行する
	RemoveDivMapMainDummy( dmap->map_cont, fsys->map_cont_dat );
	
	//入れ替え後のダミーマップ表示
	dmap->draw_flag = TRUE;
	DmcDrawFlagSetAll( dmap->map_cont, TRUE );
	
	//ロードする上層マップを非表示に
	DmcDrawFlagSetAll( tw->fsys->map_cont_dat, FALSE );
	
	//上層マップ用WORLD_MAP初期化
	SetUpWorldMatrix( link->zone_id, fsys->World );
	
	#if 0
	//上層マップ用リソース初期化
	{
		u16 area_id = ZoneData_GetAreaID( link->zone_id );
		fsys->MapResource = AllocMapResource( area_id, fsys->field_3d_anime );
		SetupMapResource( fsys->MapResource );
	}
	#endif
	
	//上位マップロードセットアップ
	{
		int x = 0,y = 0,z = 0;
		ResetDivMap( fsys->map_cont_dat, fsys->World, fsys->MapResource );
		FldTornWorld_ArcDataMapOriginGridPosGet( link->zone_id, &x, &y, &z );
		SetDivMapOriginGridPos( fsys->map_cont_dat, x, y, z );
		SetDivMapBlockMode( fsys->map_cont_dat, BLOCKMODE_2x2 );
		SetDivMapM3dObjSetFlag( fsys->map_cont_dat, DIVM3DO_OFF );
		work->jiki_gx = fsys->location->grid_x;
		work->jiki_gz = fsys->location->grid_z;
		
		LoadBlockListSetUp( fsys->map_cont_dat,
			work->jiki_gx, work->jiki_gz, work->block_no_tbl );
	}
	
	work->seq_no = SEQNO_ZONECHG_UP_2;
	return( RET_NON );
}
#endif

//--------------------------------------------------------------
/**
 * ゾーンチェンジ　上層マップ展開		2F+4F
 * @param	tw	TORNWORLD_WORK
 * @param	work	TW_ZONECHG_WORK
 * @retval	PROCRET
 */
//--------------------------------------------------------------
static PROCRET tw_ZoneChgUpTask_2(
		TORNWORLD_WORK *tw, TW_ZONECHG_WORK *work )
{
	FIELDSYS_WORK *fsys = tw->fsys;
	
	LoadBlockListNoNonAlloc( fsys->map_cont_dat,
		work->list_count, work->block_no_tbl[work->list_count] );
	
	work->list_count++;
	
	if( work->list_count >= 4 ){
		work->seq_no = SEQNO_ZONECHG_UP_3;
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ゾーンチェンジ　上層マップ展開完了		6F+1F (起動含みこちらも8F
 * @param	tw	TORNWORLD_WORK
 * @param	work	TW_ZONECHG_WORK
 * @retval	PROCRET
 */
//--------------------------------------------------------------
static PROCRET tw_ZoneChgUpTask_3(
		TORNWORLD_WORK *tw, TW_ZONECHG_WORK *work )
{
	FIELDSYS_WORK *fsys = tw->fsys;
	
	//座標更新
	UpdateDivMapNowPos( fsys->map_cont_dat, work->jiki_gx, work->jiki_gz );
	
	//TORNWORLDゾーンチェンジによる追加処理
	tw_ZoneChangeAddProcUp( tw, work->next_zone_id );
	
	//リンクアクター　ゾーン削除無効フラグを落とす
	if( work->lact != NULL ){
		tw_LinkPosActZoneChgDelNotFlagSet( work->lact, FALSE );
	}
	
	//上層マップを表示
	DmcDrawFlagSetAll( tw->fsys->map_cont_dat, TRUE );
	
	return( RET_END );
}

//--------------------------------------------------------------
///	ゾーンチェンジ上昇処理テーブル
//--------------------------------------------------------------
static PROCRET (* const DATA_ZoneChangeUpTbl[SEQNO_ZONECHG_UP_MAX])( TORNWORLD_WORK *tw, TW_ZONECHG_WORK *work ) =
{
	tw_ZoneChgUpTask_0,
	tw_ZoneChgUpTask_1,
	tw_ZoneChgUpTask_2,
	tw_ZoneChgUpTask_3,
};

//==============================================================================
//	ゾーン切り替えイベント
//==============================================================================
//--------------------------------------------------------------
/**
 * ゾーン切り替えイベントチェック
 * @param	tw	TORNWORLD_WORK
 * @param	gx	自機グリッドX
 * @param	gy	自機グリッドY
 * @param	gz	自機グリッドZ
 * @retval	BOOL	TRUE=起動
 */
//--------------------------------------------------------------
static BOOL tw_EventMoveCheck_ZoneChange(
		TORNWORLD_WORK *tw, int gx, int gy, int gz )
{
	TW_LINKPOS_ACTOR *lact;
	
	lact = tw_LinkPosActPosCheck( tw, gx, gy, gz, twNowZoneID(tw) );
	if( lact == NULL ){ return( FALSE ); }
	
	if( lact->ldata.move_type == LPOSMOVE_ZONE_DOWN ||
		lact->ldata.move_type == LPOSMOVE_ZONE_UP ){
		tw_EventSet_ZoneChange( tw, lact );
		return( TRUE );
	}
	
	return( FALSE );
}

typedef struct
{
	u8 zone_chg_type;
	u8 shake_finish;
	u16 zchg_mid;
	
	u16 seq_no;
	u16 next_zone_id;
	u16 next_move_id;
	s16 end_gx;
	s16 end_gy;
	s16 end_gz;
	u16 zlink_mflag_on;
	u16 zlink_mflag_off;
	
	VecFx32 jiki_pos;
	VecFx32 move_val;
	VecFx32 max_count;
	VecFx32 chg_count;
	VecFx32 count;
	
	fx32 shake_save_oy_jiki;
	fx32 shake_save_oy_pair;
	fx32 shake_save_y_lift;
	fx32 shake_offs_y;
	u32 shake_count;
	
	TW_LINKPOS_ACTOR *lact;
	
	FIELD_OBJ_PTR fldobj_pair;
	
	TCB_PTR akagi_tcb;
}EV_ZONECHG_WORK;

#if 0
typedef enum
{
	SCRTYPE_ACMD,
	SCRTYPE_LIFT_MOVE,
	SCRTYPE_MAX,
}SCRTYPE;
#endif

enum
{
	SEQNO_ZCHG_INIT = 0,
	SEQNO_ZCHG_START_SHAKE,
	SEQNO_ZCHG_MOVE_START,
	SEQNO_ZCHG_ZONE_CHANGE,
	SEQNO_ZCHG_MOVE,
	SEQNO_ZCHG_END,
	
	SEQNO_ZCHG_D0105_AKAGI_INIT,
	SEQNO_ZCHG_D0105_AKAGI_WAIT,
	SEQNO_ZCHG_MAX,
};

#define ZONE_CHANGE_GRID (14)
#define ZONE_CHANGE_END_GRID (32)

static PROCRET (* const DATA_EvZoneChgProcTbl[SEQNO_ZCHG_MAX])(
		TORNWORLD_WORK *tw, EV_ZONECHG_WORK *work );

//--------------------------------------------------------------
/**
 * ゾーン切り替えイベントセット
 * @param	tw	TORNWORLD_WORK
 * @param	lact	TW_LINKPOS_ACTOR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_EventSet_ZoneChange(
		TORNWORLD_WORK *tw, TW_LINKPOS_ACTOR *lact )
{
	EV_ZONECHG_WORK *work;
	
	work = tw_EventWorkInit( tw, sizeof(EV_ZONECHG_WORK) );
	work->lact = lact;
	
	FieldEvent_Set( tw->fsys, tw_GMEVENT_ZoneChange, tw );
}

//--------------------------------------------------------------
/**
 * ゾーン切り替え　イベント
 * @param	event GMEVENT_CONTROL
 * @retval	BOOL	TRUE=終了
 */
//--------------------------------------------------------------
static BOOL tw_GMEVENT_ZoneChange( GMEVENT_CONTROL *event )
{
	PROCRET ret;
	TORNWORLD_WORK *tw = FieldEvent_GetSpecialWork( event );
	EV_ZONECHG_WORK *work = tw_EventWorkGet( tw );
	
	do{
		ret = DATA_EvZoneChgProcTbl[work->seq_no]( tw, work );
	}while( ret == RET_CONTINUE );
	
	if( ret == RET_END ){
		return( TRUE );
	}
	
	return( FALSE );
}

#if 0
//--------------------------------------------------------------
/**
 * ゾーン切り替え　カウント
 * @param	work	EV_ZONECHG_WORK
 * @retval	int	カウントしたグリッド数
 */
//--------------------------------------------------------------
static int tw_ZoneChangeMoveCount( EV_ZONECHG_WORK *work )
{
	fx32 val = work->move_val.y;
	if( val < 0 ){ val = -val; }
	work->count += val;
	return( SIZE_GRID_FX32(work->count) );
}
#endif

//--------------------------------------------------------------
/**
 * ゾーン切り替え　初期化
 * @param	tw	TORNWORLD_WORK
 * @param	work	EV_ZONECHG_WORK
 * @retval	BOOL	TRUE=再帰
 */
//--------------------------------------------------------------
static PROCRET EvZoneChg_Init( TORNWORLD_WORK *tw, EV_ZONECHG_WORK *work )
{
	u32 zone_id;
	FIELD_OBJ_PTR fldobj;
	const TW_ZONE_LINKDATA *link;
	const LINKPOS_ZONECHGMOVEDATA *mdata_p;
	
	work->zone_chg_type = work->lact->ldata.move_type;
	work->zchg_mid = work->lact->ldata.mdata_id;
	
	#ifdef PM_DEBUG
	if( work->zone_chg_type != LPOSMOVE_ZONE_UP &&
		work->zone_chg_type != LPOSMOVE_ZONE_DOWN ){
		GF_ASSERT( 0 );
	}
	#endif
	
	zone_id = twNowZoneID( tw );
	link = tw_ZoneLinkDataGet( zone_id );
	
	if( work->zone_chg_type == LPOSMOVE_ZONE_DOWN ){	//下降
		work->next_zone_id = link->lower_zone_id;
		
		//下降イベント限定処理
		{
			EVENTWORK *ev = SaveData_GetEventWork( tw->fsys->savedata );
			u16 ev_seq_no = SysWork_TornWorldEventSeqNoGet( ev );
			
			//　シロナと共に
			if( zone_id == ZONE_ID_D34R0101 && ev_seq_no == TWEVSEQ_01_02 ){
				work->fldobj_pair = FieldOBJSys_OBJIDSearch(
						tw->fsys->fldobjsys, TW_OBJID_0101_SIRONA_01 );
				GF_ASSERT( work->fldobj_pair != NULL );
			}else if( zone_id == ZONE_ID_D34R0108 &&
				ev_seq_no == TWEVSEQ_07_00 ){
				work->fldobj_pair = FieldOBJSys_OBJIDSearch(
					tw->fsys->fldobjsys, TW_OBJID_0108_SIRONA );
				GF_ASSERT( work->fldobj_pair != NULL );
			}
			
			#if 0	//四層アカギイベントスクリプトで対応
			//ストッパーシロナと会話せず４層へ移動する
			if( zone_id == ZONE_ID_D34R0103 && ev_seq_no == TWEVSEQ_03_00 ){
				//ストッパーシロナ解除
				SysWork_TornWorldEventSeqNoSet( ev, TWEVSEQ_03_01 );
			}
			#endif
		}
	}else{											//上昇
		work->next_zone_id = link->upper_zone_id;
		
		//上昇時限定 動作ID別ゾーン移動フラグ強制セット
		switch( tw, work->zchg_mid ){
		case ZCHG_MID_Z0107_6_5:	//6->5
			tw_SaveDataZLinkMoveFlagON( tw, ZLINKMFLAG_0405A );
			tw_SaveDataZLinkMoveFlagOFF( tw, ZLINKMFLAG_0405B );
			break;
		case ZCHG_MID_Z0105_5A_4A:	//5->4
			tw_SaveDataZLinkMoveFlagOFF( tw, ZLINKMFLAG_0406A );
			break;
		case ZCHG_MID_Z0105_5B_4B:	//5->4
			tw_SaveDataZLinkMoveFlagON( tw, ZLINKMFLAG_0504A );
			tw_SaveDataZLinkMoveFlagOFF( tw, ZLINKMFLAG_0405A );
			tw_SaveDataZLinkMoveFlagOFF( tw, ZLINKMFLAG_0406A );
			break;
		}
	}
	
	mdata_p = tw_ZoneLinkPosZoneChgMoveDataGet( work->zchg_mid );
	
	fldobj = Player_FieldOBJGet( tw->fsys->player );
	FieldOBJ_VecPosGet( fldobj, &work->jiki_pos );
	FieldOBJ_StatusBitSet_HeightGetOFF( fldobj, TRUE );
	
	work->count.x = 0;
	work->count.y = 0;
	work->count.z = 0;
	work->end_gx = FieldOBJ_NowPosGX_Get( fldobj );
	work->end_gx += mdata_p->end_gx;
	work->end_gy = FieldOBJ_NowPosGY_Get( fldobj );
	work->end_gy += G_GRID_H_GRID( mdata_p->end_gy );
	work->end_gz = FieldOBJ_NowPosGZ_Get( fldobj );
	work->end_gz += mdata_p->end_gz;
	work->max_count.x = GRID_SIZE_FX32( mdata_p->end_gx );
	work->max_count.y = GRID_SIZE_FX32( mdata_p->end_gy );
	work->max_count.z = GRID_SIZE_FX32( mdata_p->end_gz );
	work->chg_count.x = GRID_SIZE_FX32( mdata_p->chg_gx );
	work->chg_count.y = GRID_SIZE_FX32( mdata_p->chg_gy );
	work->chg_count.z = GRID_SIZE_FX32( mdata_p->chg_gz );
	work->move_val = mdata_p->move_val;
	work->next_move_id = mdata_p->next_id;
	
	if( work->shake_finish == FALSE ){		//振動まだ
		VecFx32 pos,*eoa_pos_p;
		work->shake_offs_y = NUM_FX32( 6 );
		
		FieldOBJ_VecDrawOffsOutSideGet( fldobj, &pos );
		work->shake_save_oy_jiki = pos.y;
		
		if( work->fldobj_pair != NULL ){
			FieldOBJ_VecDrawOffsOutSideGet( work->fldobj_pair, &pos );
			work->shake_save_oy_pair = pos.y;
		}
		
		eoa_pos_p = tw_EoaLinkOBJPosPtrGet( work->lact->eoa );
		work->shake_save_y_lift = eoa_pos_p->y;
		
		work->shake_finish = TRUE;
		work->seq_no = SEQNO_ZCHG_START_SHAKE;
		
		Snd_SePlay( SE_YABURETA_MOVE );
	}else{									//振動済み
		work->seq_no = SEQNO_ZCHG_MOVE_START;
	}
	
#if 0	
	if( mdata_p->zlink_mflag_on != ZLINKMFLAG_MAX ){
		tw_SaveDataZLinkMoveFlagON( tw, mdata_p->zlink_mflag_on );
	}
	
	if( mdata_p->zlink_mflag_off != ZLINKMFLAG_MAX ){
		tw_SaveDataZLinkMoveFlagOFF( tw, mdata_p->zlink_mflag_off );
	}
#else
	work->zlink_mflag_on = mdata_p->zlink_mflag_on;
	work->zlink_mflag_off = mdata_p->zlink_mflag_off;
#endif
	
	#ifdef DEBUG_TW_PRINTF
	OS_Printf( "TW ZoneChange MDataID(%d)\n", work->zchg_mid );
	OS_Printf( "TW ZoneChange EndGX(%d),GY(%d),GZ(%d)\n",
			mdata_p->end_gx,mdata_p->end_gy,mdata_p->end_gz );
	OS_Printf( "TW ZoneChange ValX(%xH)\n", work->move_val.x );
	OS_Printf( "TW ZoneChange ValY(%xH)\n", work->move_val.y );
	OS_Printf( "TW ZoneChange ValZ(%xH)\n", work->move_val.z );
	#endif
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ゾーン切り替え　起動揺れ
 * @param	tw	TORNWORLD_WORK
 * @param	work	EV_ZONECHG_WORK
 * @retval	BOOL	TRUE=再帰
 */
//--------------------------------------------------------------
static PROCRET EvZoneChg_StartShake(
		TORNWORLD_WORK *tw, EV_ZONECHG_WORK *work )
{
	VecFx32 offs;
	PLAYER_STATE_PTR jiki = tw->fsys->player;
	FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( jiki );
	VecFx32 *eoa_pos = tw_EoaLinkOBJPosPtrGet( work->lact->eoa );
	
	FieldOBJ_VecDrawOffsOutSideGet( fldobj, &offs );
	offs.y = work->shake_save_oy_jiki + work->shake_offs_y;
	FieldOBJ_VecDrawOffsOutSideSet( fldobj, &offs );
	twFldOBJVecPosUpdate( fldobj, &work->jiki_pos );
	
	if( work->fldobj_pair != NULL ){
		VecFx32 pos;
		FieldOBJ_VecDrawOffsOutSideGet( work->fldobj_pair, &offs );
		offs.y = work->shake_save_oy_pair + work->shake_offs_y;
		FieldOBJ_VecDrawOffsOutSideSet( work->fldobj_pair, &offs );
		
		FieldOBJ_VecPosGet( work->fldobj_pair, &pos );
		pos.y = work->jiki_pos.y;
		twFldOBJVecPosUpdate( work->fldobj_pair, &pos );
	}
	
	eoa_pos->y = work->shake_save_y_lift + work->shake_offs_y;
	
	work->shake_offs_y = -work->shake_offs_y;
	
	if( work->shake_offs_y >= 0 ){
		if( work->shake_offs_y >= NUM_FX32(4) ){
			work->shake_offs_y -= NUM_FX32( 2 );
		}else if( work->shake_offs_y > NUM_FX32(1) ){
			work->shake_offs_y -= NUM_FX32( 1 );
		}else{
			work->shake_count++;
			
			if( work->shake_count >= 8 ){
				work->shake_offs_y -= NUM_FX32( 1 );
			}
		}
		
		if( work->shake_offs_y <= 0 ){
			FieldOBJ_VecDrawOffsOutSideGet( fldobj, &offs );
			offs.y = work->shake_save_oy_jiki;
			FieldOBJ_VecDrawOffsOutSideSet( fldobj, &offs );
			
			if( work->fldobj_pair != NULL ){
				FieldOBJ_VecDrawOffsOutSideGet( work->fldobj_pair, &offs );
				offs.y = work->shake_save_oy_pair;
				FieldOBJ_VecDrawOffsOutSideSet( work->fldobj_pair, &offs );
			}
			
			eoa_pos->y = work->shake_save_y_lift;
			
			work->seq_no = SEQNO_ZCHG_MOVE_START;
		}
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ゾーン切り替え　初期移動
 * @param	tw	TORNWORLD_WORK
 * @param	work	EV_ZONECHG_WORK
 * @retval	BOOL	TRUE=再帰
 */
//--------------------------------------------------------------
static PROCRET EvZoneChg_Start( TORNWORLD_WORK *tw, EV_ZONECHG_WORK *work )
{
	int grid;
	PLAYER_STATE_PTR jiki = tw->fsys->player;
	FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( jiki );
	VecFx32 *eoa_pos = tw_EoaLinkOBJPosPtrGet( work->lact->eoa );
	
	if( work->count.x != work->max_count.x ){
		work->count.x += work->move_val.x;
		work->jiki_pos.x += work->move_val.x;
		eoa_pos->x += work->move_val.x;
	}
	
	if( work->count.y != work->max_count.y ){
		work->count.y += work->move_val.y;
		work->jiki_pos.y += work->move_val.y;
		eoa_pos->y += work->move_val.y;
	}
	
	if( work->count.z != work->max_count.z ){
		work->count.z += work->move_val.z;
		work->jiki_pos.z += work->move_val.z;
		eoa_pos->z += work->move_val.z;
	}
	
	twFldOBJVecPosUpdate( fldobj, &work->jiki_pos );
	
	if( work->fldobj_pair != NULL ){
		VecFx32 pos;
		FieldOBJ_VecPosGet( work->fldobj_pair, &pos );
		pos.y = work->jiki_pos.y;
		twFldOBJVecPosUpdate( work->fldobj_pair, &pos );
	}
	
	if(	work->count.x == work->chg_count.x &&
		work->count.y == work->chg_count.y &&
		work->count.z == work->chg_count.z ){
		work->seq_no = SEQNO_ZCHG_ZONE_CHANGE;
		return( RET_CONTINUE );
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ゾーン切り替え　ゾーンチェンジ
 * @param	tw	TORNWORLD_WORK
 * @param	work	EV_ZONECHG_WORK
 * @retval	BOOL	TRUE=再帰
 */
//--------------------------------------------------------------
static PROCRET EvZoneChg_Change( TORNWORLD_WORK *tw, EV_ZONECHG_WORK *work )
{
	tw_LinkPosActZoneChgDelNotFlagSet( work->lact, TRUE );
	
	//現ターンで終了。行き先に置き換え
	if( work->next_move_id == ZCHG_MID_MAX ){
		if( work->zlink_mflag_on != ZLINKMFLAG_MAX ){
			tw_SaveDataZLinkMoveFlagON( tw, work->zlink_mflag_on );
		}
		
		if( work->zlink_mflag_off != ZLINKMFLAG_MAX ){
			tw_SaveDataZLinkMoveFlagOFF( tw, work->zlink_mflag_off );
		}
		
		tw_LinkPosActZoneChangePerm( tw, work->lact, work->next_zone_id );
	}
	
	if( work->fldobj_pair != NULL ){	//イベントシロナ限定置き換え
		if( work->next_zone_id == ZONE_ID_D34R0102 ){
			FieldOBJ_OBJIDSet( work->fldobj_pair, TW_OBJID_0102_SIRONA );
		}else{	//ZONE_ID_D34R0109
			FieldOBJ_OBJIDSet( work->fldobj_pair, TW_OBJID_0109_SIRONA );
			FieldOBJ_EventIDSet(
				work->fldobj_pair, SCRID_D34R0109_SIRONA_TALK00 );
		}
		
		FieldOBJ_ZoneIDSet( work->fldobj_pair, work->next_zone_id );
	}
	
	if( work->zone_chg_type == LPOSMOVE_ZONE_DOWN ){
		#if 0
		tw_DmyMapZoneChgDown( tw );
		tw_ZoneChangeDown( tw );
		#else
		//tw_DmyMapZoneChgDown( tw, work->lact );
		tw_ZoneChangeTaskSet( tw, work->lact, TWZONECHGTYPE_DOWN );
		#endif
	}else if( work->zone_chg_type == LPOSMOVE_ZONE_UP ){
		#if 0
		tw_DmyMapZoneChgUp( tw );
		tw_ZoneChangeUp( tw );
		#else
		//tw_DmyMapZoneChgUp( tw, work->lact );
		tw_ZoneChangeTaskSet( tw, work->lact, TWZONECHGTYPE_UP );
		#endif
	}else{
		GF_ASSERT( 0 );
	}
	
	#if 0
	tw_LinkPosActZoneChgDelNotFlagSet( work->lact, FALSE );
	#endif
	work->seq_no = SEQNO_ZCHG_MOVE;
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ゾーン切り替え　終点まで移動
 * @param	tw	TORNWORLD_WORK
 * @param	work	EV_ZONECHG_WORK
 * @retval	BOOL	TRUE=再帰
 */
//--------------------------------------------------------------
static PROCRET EvZoneChg_Move( TORNWORLD_WORK *tw, EV_ZONECHG_WORK *work )
{
	int grid;
	PLAYER_STATE_PTR jiki = tw->fsys->player;
	FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( jiki );
	VecFx32 *eoa_pos = tw_EoaLinkOBJPosPtrGet( work->lact->eoa );
	
	if( work->count.x != work->max_count.x ){
		work->count.x += work->move_val.x;
		work->jiki_pos.x += work->move_val.x;
		eoa_pos->x += work->move_val.x;
	}
	
	if( work->count.y != work->max_count.y ){
		work->count.y += work->move_val.y;
		work->jiki_pos.y += work->move_val.y;
		eoa_pos->y += work->move_val.y;
	}
	
	if( work->count.z != work->max_count.z ){
		work->count.z += work->move_val.z;
		work->jiki_pos.z += work->move_val.z;
		eoa_pos->z += work->move_val.z;
	}
	
	twFldOBJVecPosUpdate( fldobj, &work->jiki_pos );
	
	if( work->fldobj_pair != NULL ){
		VecFx32 pos;
		FieldOBJ_VecPosGet( work->fldobj_pair, &pos );
		pos.y = work->jiki_pos.y;
		twFldOBJVecPosUpdate( work->fldobj_pair, &pos );
	}
	
	if(	work->count.x == work->max_count.x &&
		work->count.y == work->max_count.y &&
		work->count.z == work->max_count.z ){
		
		if( work->next_move_id == ZCHG_MID_MAX ){
			Snd_SeStopBySeqNo( SE_YABURETA_MOVE, 0 );
			work->seq_no = SEQNO_ZCHG_END;
		}else{
			FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( tw->fsys->player );
			FieldOBJ_NowPosGX_Set( fldobj, work->end_gx );
			FieldOBJ_NowPosGY_Set( fldobj, work->end_gy );
			FieldOBJ_NowPosGZ_Set( fldobj, work->end_gz );
			FieldOBJ_GPosUpdate( fldobj );
			
			if( work->fldobj_pair != NULL ){
				FieldOBJ_NowPosGY_Set( work->fldobj_pair, work->end_gy );
				FieldOBJ_GPosUpdate( work->fldobj_pair );
			}
			
			{	//動作ID変更
				TW_LINKPOS_ACTOR *lact = work->lact;
				lact->ldata.mdata_id = work->next_move_id;
				FieldOBJ_ParamSet( lact->fldobj,
					lact->ldata.mdata_id, FLDOBJ_PARAM_2 );
			}
			
			work->seq_no = SEQNO_ZCHG_INIT;
		}
		
		return( RET_CONTINUE );
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ゾーン切り替え　終了
 * @param	tw	TORNWORLD_WORK
 * @param	work	EV_ZONECHG_WORK
 * @retval	BOOL	TRUE=再帰
 */
//--------------------------------------------------------------
static PROCRET EvZoneChg_End( TORNWORLD_WORK *tw, EV_ZONECHG_WORK *work )
{
	if( tw_ZoneChangeTaskCheck(tw) == FALSE ){
		FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( tw->fsys->player );
		FieldOBJ_NowPosGX_Set( fldobj, work->end_gx );
		FieldOBJ_NowPosGY_Set( fldobj, work->end_gy );
		FieldOBJ_NowPosGZ_Set( fldobj, work->end_gz );
		FieldOBJ_GPosUpdate( fldobj );
		Player_HeightGetSet_ON_OFF( tw->fsys->player, TRUE );
	
		if( work->fldobj_pair != NULL ){
			FieldOBJ_NowPosGY_Set( work->fldobj_pair, work->end_gy );
			FieldOBJ_GPosUpdate( work->fldobj_pair );
		}
		
		//下降時限定 リフト強制出現処理
		if( work->zone_chg_type == LPOSMOVE_ZONE_DOWN ){
			switch( work->zchg_mid ){
			case ZCHG_MID_Z0104_4_6_1:	//4->6
				tw_SaveDataZLinkMoveFlagON( tw, ZLINKMFLAG_0506B );
				tw_LinkPosAddZoneIDNo(
					tw, ZONE_ID_D34R0107, LPOSID0107_ZUP_06_05 );
				break;
			}
		}
		
		//第５層アカギイベントチェック
		if( work->next_zone_id == ZONE_ID_D34R0105 &&
			work->zone_chg_type == LPOSMOVE_ZONE_DOWN &&
			work->lact->ldata.id == LPOSID0105_ZUP_5B_4B ){
			EVENTWORK *ev = SaveData_GetEventWork( tw->fsys->savedata );
			
			if( SysWork_TwAkagiAppearFlagGet(ev) == 0 ){
				work->seq_no = SEQNO_ZCHG_D0105_AKAGI_INIT;
				return( RET_CONTINUE );
			}
		}
		
		return( RET_END );
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
///	アカギ　座標別アニメーションコマンドテーブル
//--------------------------------------------------------------
static const FIELD_OBJ_ACMD_LIST DATA_D0105AkagiAcmdList0[] =
{
	{AC_WALK_R_8F,2},
	{AC_WALK_U_8F,4},
	{ACMD_END,0},
};

static const FIELD_OBJ_ACMD_LIST DATA_D0105AkagiAcmdList1[] =
{
	{AC_WALK_R_8F,1},
	{AC_WALK_U_8F,4},
	{ACMD_END,0},
};

static const FIELD_OBJ_ACMD_LIST DATA_D0105AkagiAcmdList2[] =
{
	{AC_WALK_U_8F,4},
	{ACMD_END,0},
};

static const FIELD_OBJ_ACMD_LIST * const DATA_D0105AkagiAcmdListTbl[3] =
{
	DATA_D0105AkagiAcmdList0,
	DATA_D0105AkagiAcmdList1,
	DATA_D0105AkagiAcmdList2,
};

//--------------------------------------------------------------
/**
 * ゾーン切り替え　第五層限定アカギ移動イベント 初期化
 * @param	tw	TORNWORLD_WORK
 * @param	work	EV_ZONECHG_WORK
 * @retval	BOOL	TRUE=再帰
 */
//--------------------------------------------------------------
static PROCRET EvZoneChg_D0105AkagiEvent_Init(
		TORNWORLD_WORK *tw, EV_ZONECHG_WORK *work )
{
	int gx;
	FIELD_OBJ_PTR akagi;
	const FIELD_OBJ_ACMD_LIST *tbl;
	
	akagi = FieldOBJSys_OBJIDSearch(
			tw->fsys->fldobjsys, TW_OBJID_0105_AKAGI );
	GF_ASSERT( akagi != NULL );
	
	gx = FieldOBJ_NowPosGX_Get( akagi );
	
	#ifdef DEBUG_TW_PRINTF
	OS_Printf( "akagi gx = %d\n", gx );
	#endif
	GF_ASSERT( (u32)(gx-88) < 3 );
	
	tbl = DATA_D0105AkagiAcmdListTbl[gx-88];
	work->akagi_tcb = FieldOBJ_AcmdListSet( akagi, tbl );
	work->seq_no = SEQNO_ZCHG_D0105_AKAGI_WAIT;
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ゾーン切り替え　第五層限定アカギ移動イベント 終了待ち
 * @param	tw	TORNWORLD_WORK
 * @param	work	EV_ZONECHG_WORK
 * @retval	BOOL	TRUE=再帰
 */
//--------------------------------------------------------------
static PROCRET EvZoneChg_D0105AkagiEvent_Wait(
		TORNWORLD_WORK *tw, EV_ZONECHG_WORK *work )
{
	if( FieldOBJ_AcmdListEndCheck(work->akagi_tcb) == TRUE ){
		EVENTWORK *ev;
		FIELD_OBJ_PTR akagi;
		FieldOBJ_AcmdListEnd( work->akagi_tcb );
		
		akagi = FieldOBJSys_OBJIDSearch(
			tw->fsys->fldobjsys, TW_OBJID_0105_AKAGI );
		
		tw_FldOBJDelete( tw, akagi );
		
		ev = SaveData_GetEventWork( tw->fsys->savedata );
		SysWork_TwAkagiAppearFlagSet( ev, 1 );
		return( RET_END );
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
///	ゾーン切り替え処理テーブル
//--------------------------------------------------------------
static PROCRET (* const DATA_EvZoneChgProcTbl[SEQNO_ZCHG_MAX])( TORNWORLD_WORK *tw, EV_ZONECHG_WORK *work ) =
{
	EvZoneChg_Init,
	EvZoneChg_StartShake,
	EvZoneChg_Start,
	EvZoneChg_Change,
	EvZoneChg_Move,
	EvZoneChg_End,
	
	EvZoneChg_D0105AkagiEvent_Init,
	EvZoneChg_D0105AkagiEvent_Wait,
};

//==============================================================================
//	ゾーンリンク
//==============================================================================
//--------------------------------------------------------------
/**
 * 指定されたゾーンIDからZONE_LINK_DATAを返す
 * @param	zone_id	zone id
 * @retval	TW_ZONE_LINKDATA NULL=無し
 */
//--------------------------------------------------------------
static const TW_ZONE_LINKDATA * tw_ZoneLinkDataGet( int zone_id )
{
	int i = 0;
	const TW_ZONE_LINKDATA *link = DATA_TwZoneLinkDataTbl;
	
	while( i < TW_ZONECOUNT_MAX ){
		if( link->zone_id == zone_id ){
			return( link );
		}
		i++;
		link++;
	}
	
	GF_ASSERT( 0 );
	return( NULL );
}

//==============================================================================
//	オブジェリソース管理
//==============================================================================
//--------------------------------------------------------------
/**
 * モデルリソース管理 初期化
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJResDataInit( TORNWORLD_WORK *tw )
{
	int i;
	TW_OBJRESDATA_WORK *ores = &tw->ores;
	memset( (u8*)ores, 0, sizeof(TW_OBJRESDATA_WORK) );
	
	for( i = 0; i < TW_MDL_MAX; i++ ){
		ores->mdl_tbl[i].id = TW_MDL_MAX;
	}
	
	for( i = 0; i < TW_ANM_MAX; i++ ){
		ores->anm_tbl[i].id = TW_ANM_MAX;
	}
}

//--------------------------------------------------------------
/**
 * モデルリソース管理　モデル登録
 * @param	tw	TORNWORLD_WORK
 * @param	id	TW_MDL_MAX
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJResDataMdlRegist( TORNWORLD_WORK *tw, u32 id )
{
	TW_OBJRESDATA_WORK *ores = &tw->ores;
	GF_ASSERT( id < TW_MDL_MAX );
	
	if( ores->mdl_tbl[id].id == TW_MDL_MAX ){
		#ifdef DEBUG_TW_PRINTF
		DEBUG_twFldHeapPrintf();
		#endif
		ores->mdl_tbl[id].id = id;
		FE_FROMdl_ResSetArcLoadTexTrans(
			tw->fsys->fes, &ores->mdl_tbl[id].rmdl, 0,
			DATA_MdlArcIdxTbl[id], ALLOC_LO );
		#ifdef DEBUG_TW_PRINTF
		DEBUG_twFldHeapPrintf();
		OS_Printf( "****************OBJRes Mdl ID(%d) Reg*********\n", id );
		#endif
	}
}

//--------------------------------------------------------------
/**
 * モデルリソース管理　モデル破棄
 * @param	tw	TORNWORLD_WORK
 * @param	id	TW_MDL_MAX
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJResDataMdlDelete( TORNWORLD_WORK *tw, u32 id )
{
	TW_OBJRESDATA_WORK *ores = &tw->ores;
	GF_ASSERT( id < TW_MDL_MAX );
	if( ores->mdl_tbl[id].id != TW_MDL_MAX ){
		#ifdef DEBUG_TW_PRINTF
		OS_Printf( "==============OBJRes Mdl ID(%d) Del===========\n", id );
		#endif
		ores->mdl_tbl[id].id = TW_MDL_MAX;
		FRO_MDL_DeleteAll( &ores->mdl_tbl[id].rmdl );
	}
}

//--------------------------------------------------------------
/**
 * モデルリソース管理　モデル検索
 * @param	tw	TORNWORLD_WORK
 * @param	id	TW_MDL_MAX
 * @retval	nothing
 */
//--------------------------------------------------------------
static FRO_MDL * tw_OBJResDataMdlSearch( TORNWORLD_WORK *tw, u32 id )
{
	TW_OBJRESDATA_WORK *ores = &tw->ores;
	GF_ASSERT( id < TW_MDL_MAX );
	GF_ASSERT( ores->mdl_tbl[id].id != TW_MDL_MAX );
	
	return( &ores->mdl_tbl[id].rmdl );
}

//--------------------------------------------------------------
/**
 * モデルリソース管理　アニメ登録
 * @param	tw	TORNWORLD_WORK
 * @param	id	TW_ANM_MAX
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJResDataAnmRegist( TORNWORLD_WORK *tw, u32 id )
{
	TW_OBJRESDATA_WORK *ores = &tw->ores;
	GF_ASSERT( id < TW_ANM_MAX );
	if( ores->anm_tbl[id].id == TW_ANM_MAX ){
		u32 idx = DATA_AnmArcIdxTbl[id];
		u32 size = FE_ArcDataSizeGet( tw->fsys->fes, idx );
		ores->anm_tbl[id].anm_res = sys_AllocMemoryLo( HEAPID_FIELD, size );
		FE_ArcDataLoad( tw->fsys->fes, idx,ores->anm_tbl[id].anm_res );
		ores->anm_tbl[id].id = id;
		#ifdef DEBUG_TW_PRINTF
		OS_Printf( "tw OBJRes Anm ID(%d) Regist\n", id );
		#endif
	}
}

//--------------------------------------------------------------
/**
 * モデルリソース管理　アニメ破棄
 * @param	tw	TORNWORLD_WORK
 * @param	id	TW_Anm_MAX
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJResDataAnmDelete( TORNWORLD_WORK *tw, u32 id )
{
	TW_OBJRESDATA_WORK *ores = &tw->ores;
	GF_ASSERT( id < TW_ANM_MAX );
	if( ores->anm_tbl[id].id != TW_ANM_MAX ){
		ores->anm_tbl[id].id = TW_ANM_MAX;
		#ifdef DEBUG_TW_PRINTF
		OS_Printf( "tw OBJRes Anm ID(%d) Delete\n", id );
		#endif
		sys_FreeMemoryEz( ores->anm_tbl[id].anm_res );
	}
}

//--------------------------------------------------------------
/**
 * モデルリソース管理　OBJリソース取得
 * @param	tw	TORNWOLRD_WORK
 * @param	type	TWOBJTYPE
 * @param	init_chk TRUE=初期化済みである(他で同typeを使用中なので流用する
 * @retval	TW_OBJRES TW_OBJRES
 */
//--------------------------------------------------------------
static TW_OBJRES * tw_OBJResDataOBJResGet(
		TORNWORLD_WORK *tw, u32 type, int *init_chk )
{
	int i;
	BOOL share;
	TW_OBJRESDATA_WORK *ores = &tw->ores;
	
	share = tw_OBJResDataTypeShareCheck( type );
	
	if( share == TRUE ){	//共用型
		for( i = 0; i < TW_OBJ_MAX; i++ ){
			if( ores->obj_tbl[i].use == TRUE ){
				if( ores->obj_tbl[i].type == type ){
					*init_chk = TRUE;
					return( &ores->obj_tbl[i] );
				}
			}
		}
	}
	
	for( i = 0; i < TW_OBJ_MAX; i++ ){
		if( ores->obj_tbl[i].use == FALSE ){
			*init_chk = FALSE;
			ores->obj_tbl[i].use = TRUE;
			ores->obj_tbl[i].type = type;
			return( &ores->obj_tbl[i] );
		}
	}
	
	#ifdef DEBUG_TW_PRINTF_FORCE
	OS_Printf( "TW OBJ Resource OVER!! MAX=%d\n", TW_OBJ_MAX );
	#endif
	GF_ASSERT( 0 );
	return( NULL );
}

//--------------------------------------------------------------
/**
 * モデルリソース管理　OBJリソース破棄
 * @param	tw	TORNWOLRD_WORK
 * @param	obj	TW_OBJRES
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJResDataOBJResRelease( TORNWORLD_WORK *tw, TW_OBJRES *obj )
{
	GF_ASSERT( obj != NULL );
	obj->use = FALSE;
}

//--------------------------------------------------------------
/**
 * モデルリソース管理　OBJリソース破棄　共用型の場合は削除しない
 * @param	tw	TORNWOLRD_WORK
 * @param	obj	TW_OBJRES
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJResDataOBJResCheckRelease(
		TORNWORLD_WORK *tw, TW_OBJRES *obj )
{
	BOOL share;
	
	#ifdef DEBUG_TW_BUF_OBJ_ASSERT
	GF_ASSERT( obj != NULL );
	GF_ASSERT( obj->use == TRUE );
	#endif
	
	#ifdef PM_DEBUG
	if( obj->use != TRUE ){	//kari
		return;
	}
	#endif
	
	share = tw_OBJResDataTypeShareCheck( obj->type );
	
	if( share == FALSE ){
		tw_OBJResDataOBJResRelease( tw, obj );
	}
}

//--------------------------------------------------------------
/**
 * モデルリソース管理　OBJリソース破棄　指定タイプを全削除
 * @param	tw	TORNWOLRD_WORK
 * @param	obj	TW_OBJRES
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJResDataOBJResReleaseTypeAll(
		TORNWORLD_WORK *tw, TWOBJTYPE type )
{
	int i;
	TW_OBJRESDATA_WORK *ores = &tw->ores;
	
	for( i = 0; i < TW_OBJ_MAX; i++ ){
		if( ores->obj_tbl[i].use && ores->obj_tbl[i].type == type ){
			#ifdef DEBUG_TW_PRINTF
			OS_Printf( "OBJリソース破棄 type(%d)\n", type );
			#endif
			tw_OBJResDataOBJResRelease( tw, &ores->obj_tbl[i] );
		}
	}
}

//--------------------------------------------------------------
/**
 * モデルマネージャ　削除
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJResDataDelete( TORNWORLD_WORK *tw )
{
	int i;
	TW_OBJRESDATA_WORK *ores = &tw->ores;
	
	for( i = 0; i < TW_MDL_MAX; i++ ){
		tw_OBJResDataMdlDelete( tw, i );
	}
	
	for( i = 0; i < TW_ANM_MAX; i++ ){
		tw_OBJResDataAnmDelete( tw, i );
	}
}

//--------------------------------------------------------------
/**
 * レンダオブジェ,アニメ初期化。登録されていない場合は登録する
 * @param	tw	TORNWORLD_WORK
 * @param	id	TW_MDL_MAX
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJResDataOBJInit( TORNWORLD_WORK *tw,
	u32 mdl_id, u32 anm_id, FRO_OBJ *robj, FRO_ANM *ranm )
{
	TW_OBJRESDATA_WORK *ores = &tw->ores;
	
	if( mdl_id != TW_MDL_MAX ){
		if( ores->mdl_tbl[mdl_id].id == TW_MDL_MAX ){
			tw_OBJResDataMdlRegist( tw, mdl_id );
		}
		
		FRO_OBJ_InitInMdl( robj, &ores->mdl_tbl[mdl_id].rmdl );
	}
	
	if( anm_id != TW_ANM_MAX ){
		if( ores->anm_tbl[anm_id].id == TW_ANM_MAX ){
			tw_OBJResDataAnmRegist( tw, anm_id );
		}
		
		FRO_ANM_AnmResSetSub( ranm, ores->anm_tbl[anm_id].anm_res, 0 );
		FRO_ANM_AllocAnmObjInMdl(
			ranm, &ores->mdl_tbl[mdl_id].rmdl, HEAPID_FIELD );
		FRO_ANM_AnmObjInitInMdl( ranm, &ores->mdl_tbl[mdl_id].rmdl );
		FRO_OBJ_AddAnmInAnm( robj, ranm );
	}
}

//--------------------------------------------------------------
/**
 * レンダオブジェ、アニメ初期化。TWOBJTYPE_MAXタイプ
 * @param	tw	TORNWORLD_WORK
 * @param	id	TWOBJTYPE_MAX
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJResDataOBJTypeInit(
	TORNWORLD_WORK *tw, TWOBJTYPE type, FRO_OBJ *robj, FRO_ANM *ranm )
{
	const TW_OBJRES_ID *tbl = &DATA_OBJResIDTbl[type];
	#ifdef DEBUG_TW_PRINTF
	OS_Printf( "<<<<<<<<<<<<<<<<OBJTypeInit Type(%d)>>>>>>>>>>>\n", type );
	#endif
	tw_OBJResDataOBJInit( tw, tbl->mdl_id, tbl->anm_id, robj, ranm );
}

//--------------------------------------------------------------
/**
 * 指定タイプのアルファ値をセット　メイン
 * @param	tw	TORNWORLD_WORK
 * @param	type	TWOBJTYPE_MAX
 * @param	alpha	濃度 薄0-31濃
 * @param	pid		ポリゴンID設定値 -1=デフォルト値
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJResDataMdlAlphaSetMain(
	TORNWORLD_WORK *tw, TWOBJTYPE type, int alpha, int pid )
{
	const TW_OBJRES_ID *tbl = &DATA_OBJResIDTbl[type];
	int id = tbl->mdl_id;
	TW_OBJRESDATA_WORK *ores = &tw->ores;
	
	#ifdef PM_DEBUG
	if( ores->mdl_tbl[id].id >= TW_MDL_MAX ){
		OS_Printf( "登録されていないMDL IDです type=%d,id=%d\n", type, id );
		GF_ASSERT( 0 );
	}
	#endif
	
	NNS_G3dMdlUseMdlAlpha( ores->mdl_tbl[id].rmdl.pResMdl );
	NNS_G3dMdlSetMdlAlphaAll( ores->mdl_tbl[id].rmdl.pResMdl, alpha );
	
	//IDを別にする事でブレンドによるバッファ上書きを防ぐ
	if( pid != -1 ){
		#if 1
		pid &= 0x3f;
		NNS_G3dMdlSetMdlPolygonIDAll( ores->mdl_tbl[id].rmdl.pResMdl, pid );
		
		#else
		{
			u32 mid = 0;
			NNSG3dResMdl *pMdl = ores->mdl_tbl[id].rmdl.pResMdl;
			//pid = NNS_G3DMdlGetPolygonID( pMdl, mid );
			
			do{
				NNS_G3dMdlSetMdlPolygonID( pMdl, mid, pid );
				pid = (pid+1) & 0x3f;
				if( !pid ){ pid++; }
			}while( ++mid < pMdl->info.numMat );
		}
		#endif
	} 
}

//--------------------------------------------------------------
/**
 * 指定タイプのアルファ値をセット
 * @param	tw	TORNWORLD_WORK
 * @param	type	TWOBJTYPE_MAX
 * @param	alpha	濃度 薄0-31濃
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJResDataMdlAlphaSet(
	TORNWORLD_WORK *tw, TWOBJTYPE type, int alpha )
{
	tw_OBJResDataMdlAlphaSetMain( tw, type, alpha, -1 );
}

//--------------------------------------------------------------
/**
 * 指定タイプのポリゴンIDをセット
 * @param	tw	TORNWORLD_WORK
 * @param	type	TWOBJTYPE_MAX
 * @param	pid		ポリゴンID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJResDataMdlPolIDSet(
	TORNWORLD_WORK *tw, TWOBJTYPE type, int pid )
{
	const TW_OBJRES_ID *tbl = &DATA_OBJResIDTbl[type];
	int id = tbl->mdl_id;
	TW_OBJRESDATA_WORK *ores = &tw->ores;
	GF_ASSERT( ores->mdl_tbl[id].id < TW_MDL_MAX );
	pid &= 0x3f;
	NNS_G3dMdlSetMdlPolygonIDAll( ores->mdl_tbl[id].rmdl.pResMdl, pid );
}

//--------------------------------------------------------------
/**
 * 指定タイプがアニメ有りかチェック
 * @param	type	TWOBJTYPE
 * @retval	BOOL	TRUE=有り
 */
//--------------------------------------------------------------
static BOOL tw_OBJResDataTypeAnmCheck( TWOBJTYPE type )
{
	const TW_OBJRES_ID *tbl = &DATA_OBJResIDTbl[type];
	if( tbl->anm_id != TW_ANM_MAX ){ return( TRUE ); }
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * 指定タイプが共有型かチェック
 * @param	type	TWOBJTYPE
 * @retval	BOOL	TRUE=有り
 */
//--------------------------------------------------------------
static BOOL tw_OBJResDataTypeShareCheck( TWOBJTYPE type )
{
	const TW_OBJRES_ID *tbl = &DATA_OBJResIDTbl[type];
	if( tbl->share_type == OBJRES_SHARE ){ return( TRUE ); }
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * OBJリソースクリーン 使われていないリソースを消去する。
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJResDataClean( TORNWORLD_WORK *tw )
{
	int i;
	
	for( i = 0; i < TWOBJTYPE_MAX; i++ ){
		if( tw_FStoneOBJResTypeSearch(tw,i) == FALSE &&
			tw_LinkPosOBJResTypeSearch(tw,i) == FALSE &&
			tw_GirasActOBJResTypeSearch(tw,i) == FALSE &&
			tw_PutOBJResTypeSearch(tw,i) == FALSE ){
			tw_OBJResDataOBJResReleaseTypeAll( tw, i );
		}
	}
	
	for( i = 0; i < TW_MDL_MAX; i++ ){
		if( tw_FStoneOBJResMdlIDSearch(tw,i) == FALSE &&
			tw_LinkPosOBJResMdlIDSearch(tw,i) == FALSE &&
			tw_GirasActOBJResMdlIDSearch(tw,i) == FALSE &&
			tw_PutOBJResMdlIDSearch(tw,i) == FALSE ){
			tw_OBJResDataMdlDelete( tw, i );
		}
	}
	
	for( i = 0; i < TW_ANM_MAX; i++ ){
		if( tw_FStoneOBJResAnmIDSearch(tw,i) == FALSE &&
			tw_LinkPosOBJResAnmIDSearch(tw,i) == FALSE &&
			tw_GirasActOBJResAnmIDSearch(tw,i) == FALSE &&
			tw_PutOBJResAnmIDSearch(tw,i) == FALSE ){
			tw_OBJResDataAnmDelete( tw, i );
		}
	}
}

#ifdef DEBUG_TW_OBJ_COUNT
//--------------------------------------------------------------
/**
 * デバッグ　現在のOBJリソース数を出力
 * @param	TORNWORLD_WORK
 * @retval	
 */
//--------------------------------------------------------------
static void DEBUG_tw_OBJResCountPrint( TORNWORLD_WORK *tw )
{
	int i = 0;
	int count = 0;
	int share = 0;
	TW_OBJRESDATA_WORK *ores = &tw->ores;
	
	while( i < TW_OBJ_MAX ){
		if( ores->obj_tbl[i].use ){
			count++;
			if( tw_OBJResDataTypeShareCheck(ores->obj_tbl[i].type) ){
				share++;
			}
		}
		
		i++;
	}
	
	OS_Printf( "TW OBJRes Max=%d,Use=%d,Share=%d,Org=%d\n",
		TW_OBJ_MAX, count, share, count-share );
}
#endif


//--------------------------------------------------------------
/**
 * 指定タイプのカリングチェック
 * @param	tw	TORNWORLD_WORK
 * @param	type	TWOBJTYPE_MAX
 * @param	pos		座標
 * @retval	TRUE	画面内
 */
//--------------------------------------------------------------
static BOOL tw_OBJResDataTypeCullingCheck(
	TORNWORLD_WORK *tw, TWOBJTYPE type, VecFx32 *pos )
{
	const VecFx32 *scale = &DATA_OBJTypeScaleTbl[type];
	
	if( scale->x == 0 ){	//チェック無し
		return( TRUE );
	}
	
	{
		const TW_OBJRES_ID *tbl = &DATA_OBJResIDTbl[type];
		FRO_MDL *rmdl = tw_OBJResDataMdlSearch( tw, tbl->mdl_id );
		MtxFx33 rot;
		MTX_Identity33(&rot);
		
		if( BB_CullingCheck3DModel(rmdl->pResMdl,pos,&rot,scale) ){
			return( TRUE );
		}
	}
	
	return( FALSE );
}

//==============================================================================
//	接続石
//==============================================================================
//--------------------------------------------------------------
/**
 * 接続石　初期化
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_LinkPosInit( TORNWORLD_WORK *tw )
{
	TW_LINKPOS_WORK *lpos = &tw->linkpos;
	memset( lpos, 0, sizeof(TW_LINKPOS_WORK) );
}

//--------------------------------------------------------------
/**
 * 接続石　削除
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_LinkPosDelete( TORNWORLD_WORK *tw )
{
	int i;
	TW_LINKPOS_WORK *lpos = &tw->linkpos;
	
	for( i = 0; i < TW_LINKPOS_ACTOR_MAX; i++ ){
		if( lpos->act_tbl[i].use ){
			tw_LinkPosActDelete( tw, &lpos->act_tbl[i] );
		}
	}
}

//--------------------------------------------------------------
/**
 * 接続石	指定ゾーンIDを持つ石を全て削除
 * @param	tw		TORNWORLD_WORK
 * @param	zone_id		削除するゾーンID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_LinkPosZoneIDDeleteAll( TORNWORLD_WORK *tw, u32 zone_id )
{
	int i;
	TW_LINKPOS_WORK *lpos = &tw->linkpos;
	
	for( i = 0; i < TW_LINKPOS_ACTOR_MAX; i++ ){
		if( lpos->act_tbl[i].use ){
			if( lpos->act_tbl[i].live_zone_id == zone_id ){
				if( lpos->act_tbl[i].zone_chg_deloff == FALSE ){
					#ifdef DEBUG_TW_PRINTF
					{
					 TW_LINKPOS_ACTOR *lact = &lpos->act_tbl[i];
					 DEBUG_twZoneIDPrintf( zone_id );
					 OS_Printf( "LINKPOSACT ID =%d,",lact->ldata.id );
					 OS_Printf( "を削除しました\n" );
					}
					#endif
					tw_LinkPosActDeleteAll( tw, &lpos->act_tbl[i] );
				}else{
					#ifdef DEBUG_TW_PRINTF
					TW_LINKPOS_ACTOR *lact = &lpos->act_tbl[i];
					DEBUG_twZoneIDPrintf( zone_id );
					OS_Printf( "LINKPOSACT ID =%d,",lact->ldata.id );
					OS_Printf( "削除しません\n" );
					#endif
				}
			}
		}
	}
}

//--------------------------------------------------------------
/**
 * 接続石　展開
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_LinkPosLoad( TORNWORLD_WORK *tw )
{
	if( tw_SaveDataInitFinishCheck(tw) == TRUE ){	//初期化済み
		int objno = 0;
		TW_LINKPOS_ACTOR *lact;
		FIELD_OBJ_PTR fldobj;
		FIELD_OBJ_SYS_PTR fos = tw->fsys->fldobjsys;
		
		while( FieldOBJSys_FieldOBJSearch(
			fos,&fldobj,&objno,FLDOBJ_STA_BIT_USE) == TRUE ){
			if( FieldOBJ_OBJIDGet(fldobj) == FLDOBJ_ID_GIMMICK ){
				lact = tw_LinkPosSpaceActGet( tw );
				tw_LinkPosActFldOBJRecover( tw, lact, fldobj );
			}
		}
	}else{
		u32 zone_id = twNowZoneID( tw );
		const TW_ZONE_LINKDATA *zlink = tw_ZoneLinkDataGet( zone_id );
		tw_LinkPosLoadZoneID( tw, zone_id );
		tw_LinkPosLoadZoneID( tw, zlink->lower_zone_id );
	}
}

//--------------------------------------------------------------
/**
 * 接続石　指定ゾーンIDのものを展開
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_LinkPosLoadZoneID( TORNWORLD_WORK *tw, u32 zone_id )
{
	const TW_LINKPOS_ZONEDATA *zlink;
	zlink = tw_LinkPosZoneDataGet( zone_id );
	
	if( zlink != NULL ){	//設置有り
		tw_LinkPosZoneDataLoad( tw, zlink, zone_id );
	}
}

//--------------------------------------------------------------
/**
 * 接続石　指定のリンクデータ展開　既に存在しているものは追加しない
 * @param	tw	TORNWORLD_WORK
 * @param	zlink	TW_LINKPOS_ZONEDATA
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_LinkPosZoneDataLoad(
	TORNWORLD_WORK *tw, const TW_LINKPOS_ZONEDATA *zlink, u32 zone_id )
{
	u32 flag;
	int i = 0;
	TW_LINKPOS_ACTOR *lact;
	
	GF_ASSERT( zlink != NULL );
	
	while( zlink->pos_tbl[i] != NULL ){
		flag = zlink->pos_tbl[i]->zlink_mflag_off_stop;
		if( flag == ZLINKMFLAG_MAX ||
			tw_SaveDataZLinkMoveFlagCheck(tw,flag) == TRUE ){
			
			if( tw_LinkPosActIDGet(tw,zlink->pos_tbl[i]->id,zone_id)==NULL){
				lact = tw_LinkPosSpaceActGet( tw );
				tw_LinkPosActAdd( tw, lact, zlink->pos_tbl[i], zone_id );
				
				#ifdef DEBUG_TW_PRINTF
				DEBUG_twZoneIDPrintf( zone_id );
				OS_Printf( "LINKPOSACT ID =%d,", lact->ldata.id );
				OS_Printf( "を追加しました\n" );
				#endif
			}else{
				#ifdef DEBUG_TW_PRINTF
				DEBUG_twZoneIDPrintf( zone_id );
				OS_Printf( "LINKPOSACT ID = %d, ", zlink->pos_tbl[i]->id );
				OS_Printf( "既に存在している為、追加を中止しました\n" );
				#endif
			}
		}
		i++;
	}
}

//--------------------------------------------------------------
/**
 * 接続石　指定されたZONE IDと番号の接続石を追加。
 * 追加済みの場合は何もせず戻る。
 * @param	tw	TORNWORLD_WORK
 * @param	zone_id	zone id
 * @param	no	接続石番号
 * @retval	int TRUE=追加された FALSE=追加済みである。
 */
//--------------------------------------------------------------
static void tw_LinkPosAddZoneIDNo( TORNWORLD_WORK *tw, u32 zone_id, int no )
{
	const TW_LINKPOS_ZONEDATA *zlink;
	zlink = tw_LinkPosZoneDataGet( zone_id );
	
	#ifdef PM_DEBUG
	{
		u32 flag = zlink->pos_tbl[no]->zlink_mflag_off_stop;
		GF_ASSERT( zlink != NULL );
		GF_ASSERT( zlink->pos_tbl[no] != NULL );
		if( flag != ZLINKMFLAG_MAX ){
			if( tw_SaveDataZLinkMoveFlagCheck(tw,flag) != TRUE ){
				GF_ASSERT( 0 );
			}
		}
	}
	#endif
	
	if( tw_LinkPosActIDGet(tw,zlink->pos_tbl[no]->id,zone_id) == NULL ){
		TW_LINKPOS_ACTOR *lact;
		lact = tw_LinkPosSpaceActGet( tw );
		tw_LinkPosActAdd( tw, lact, zlink->pos_tbl[no], zone_id );
		
		#ifdef DEBUG_TW_PRINTF
		DEBUG_twZoneIDPrintf( zone_id );
		OS_Printf( "LINKPOSACT ID =%d,", lact->ldata.id );
		OS_Printf( "を追加しました\n" );
		#endif
	}
	
	#ifdef DEBUG_TW_PRINTF
	DEBUG_twZoneIDPrintf( zone_id );
	OS_Printf( "LINKPOSACT ID = %d, ", zlink->pos_tbl[no]->id );
	OS_Printf( "既に存在している為、追加を中止しました\n" );
	#endif
}

//--------------------------------------------------------------
/**
 * 接続石　空アクター検索
 * @param	tw	TORNWORLD_WORK
 * @retval	TW_LINKPOS_ACTOR*
 */
//--------------------------------------------------------------
static TW_LINKPOS_ACTOR * tw_LinkPosSpaceActGet( TORNWORLD_WORK *tw )
{
	int i = 0; TW_LINKPOS_WORK *lpos = &tw->linkpos;
	while( i < TW_LINKPOS_ACTOR_MAX ){
		if( lpos->act_tbl[i].use == FALSE ){
			return(&lpos->act_tbl[i]);
		}
		i++;
	}
	GF_ASSERT( 0 );
	return( NULL );
}

//--------------------------------------------------------------
/**
 * 接続石　指定IDのアクター取得
 * @param	tw	TORNWORLD_WORK
 * @param	id	data_no
 * @retval	TW_LINKPOS_ACTOR NULL=該当無し
 */
//--------------------------------------------------------------
static TW_LINKPOS_ACTOR * tw_LinkPosActIDGet(
		TORNWORLD_WORK *tw, u32 id, u32 zone_id )
{
	int i = 0;
	TW_LINKPOS_WORK *lpos = &tw->linkpos;
	
	while( i < TW_LINKPOS_ACTOR_MAX ){
		if( lpos->act_tbl[i].use == TRUE &&
			lpos->act_tbl[i].ldata.id == id &&
			lpos->act_tbl[i].live_zone_id == zone_id ){
			return( &lpos->act_tbl[i] );
		}
		i++;
	}
	
	return( NULL );
}

//--------------------------------------------------------------
/**
 * 指定されたゾーンに対応したリンクデータ取得
 * @param	zone_id	ZONE_ID
 * @retval	TW_LINKPOS_ZONEDATA
 */
//--------------------------------------------------------------
static const TW_LINKPOS_ZONEDATA * tw_LinkPosZoneDataGet( u32 zone_id )
{
	int i = 0;
	const TW_LINKPOS_ZONEDATA *tbl = DATA_TwZoneLinkPosTbl;
	while( i < TW_ZONELINK_MAX ){
		if( tbl->zone_id == zone_id ){ return( tbl ); }
		i++; tbl++;
	}
	return( NULL );
}

//--------------------------------------------------------------
/**
 * 指定されたゾーンとIDに対応したLINKPOS_DATA取得
 * @param	zone_id	ZONE_ID
 * @param	id	id
 * @retval	LINKPOS_DATA
 */
//--------------------------------------------------------------
static const LINKPOS_DATA * tw_ZoneLinkPosIDDataGet( u32 zone_id, u32 id )
{
	const TW_LINKPOS_ZONEDATA *zlink;
	zlink = tw_LinkPosZoneDataGet( zone_id );
	return( zlink->pos_tbl[id] );
}

#if 0
//--------------------------------------------------------------
/**
 * 指定されたゾーンと番号に対応したLINKPOS_HITDATA取得
 * @param	zone_id	ZONE_ID
 * @param	no	番号
 * @retval	LINKPOS_MOVEDATA 
 */
//--------------------------------------------------------------
static const LINKPOS_MOVEDATA * tw_ZoneLinkPosHitDataGet(
		u32 zone_id, u32 no )
{
	const TW_LINKPOS_ZONEDATA *zlink;
	zlink = tw_LinkPosZoneDataGet( zone_id );
	GF_ASSERT( zlink->move_tbl );
	return( &zlink->move_tbl[no] );
}
#endif

#if 0
//--------------------------------------------------------------
/**
 * 指定されたゾーンと番号に対応したLINKPOS_ZONECHGDATA取得
 * @param	zone_id	ZONE_ID
 * @param	no	番号
 * @retval	LINKPOS_ZONECHGDATA
 */
//--------------------------------------------------------------
static const LINKPOS_ZONECHGDATA * tw_ZoneLinkPosZoneChgDataGet(
		u32 zone_id, u32 no )
{
	const TW_LINKPOS_ZONEDATA *zlink;
	zlink = tw_LinkPosZoneDataGet( zone_id );
	GF_ASSERT( zlink->zone_chg_tbl );
	return( &zlink->zone_chg_tbl[no] );
}
#endif

//--------------------------------------------------------------
/**
 * 指定されたIDに対応したLINKPOS_ZONECHGMOVEDATA取得
 * @param	id	ZCHG_MID_Z0101_1_2等
 * @retval	LINKPOS_ZONECHGMOVEDATA
 */
//--------------------------------------------------------------
static const LINKPOS_ZONECHGMOVEDATA * tw_ZoneLinkPosZoneChgMoveDataGet(
		u32 id )
{
	u32 i = 0;
	const LINKPOS_ZONECHGMOVEDATA *tbl = DATA_LinkPosZoneChgMoveTbl;
	while( i < ZCHG_MID_MAX ){ if(tbl->id==id){return(tbl);}tbl++;i++;}
	GF_ASSERT(0);
	return( NULL );
}

//--------------------------------------------------------------
/**
 * 接続石　フィールドOBJ追加
 * @param	tw	TORNWORLD_WORK
 * @param	g	グリッド座標
 * @param	g	グリッド座標
 * @param	g	グリッド座標
 * @param	data_no	データNo
 * @param	move_no	動作No
 * @retval	FILED_OBJ_PTR	FIELD_OBJ_PTR
 */
//--------------------------------------------------------------
static FIELD_OBJ_PTR tw_LinkPosActFldOBJAdd(
		TORNWORLD_WORK *tw, int gx, int gy, int gz,
		int data_no, int move_no, u32 zone_id )
{
	FIELD_OBJ_PTR fldobj;
	
	fldobj = FieldOBJ_AddHMake( tw->fsys->fldobjsys,
			gx, gz, DIR_UP, NONDRAW, MV_DMY, twNowZoneID(tw) );
	FieldOBJ_NowPosGY_Set( fldobj, gy );
	FieldOBJ_OBJIDSet( fldobj, FLDOBJ_ID_GIMMICK );
	FieldOBJ_ParamSet( fldobj, data_no, FLDOBJ_PARAM_0 );
	FieldOBJ_ParamSet( fldobj, zone_id, FLDOBJ_PARAM_1 );
	FieldOBJ_ParamSet( fldobj, move_no, FLDOBJ_PARAM_2 );
	FieldOBJ_NotZoneDeleteSet( fldobj, TRUE );
	FieldOBJ_StatusBitSet_FellowHit( fldobj, FALSE );
	FieldOBJ_StatusBitSet_HeightGetOFF( fldobj, TRUE );
	FieldOBJ_MoveBitSet_AttrGetOFF( fldobj, TRUE );
	return( fldobj );
}

//--------------------------------------------------------------
/**
 * 接続石　フィールドOBJから復帰 eoa追加も
 * @param	zone_id	ZONE_ID
 * @param	lact	TW_LINKPOS_ACTOR
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_LinkPosActFldOBJRecover( TORNWORLD_WORK *tw,
	TW_LINKPOS_ACTOR *lact, FIELD_OBJ_PTR fldobj )
{
	u32 id,zone_id,no,mdata_id;
	const LINKPOS_DATA *ldata;
	
	id = FieldOBJ_ParamGet( fldobj, FLDOBJ_PARAM_0 );
	zone_id = FieldOBJ_ParamGet( fldobj, FLDOBJ_PARAM_1 );
	mdata_id = FieldOBJ_ParamGet( fldobj, FLDOBJ_PARAM_2 );
	ldata = tw_ZoneLinkPosIDDataGet( zone_id, id );
	
	lact->use = TRUE;
	lact->live_zone_id = zone_id;
	lact->ldata = *ldata;
	lact->ldata.gx = FieldOBJ_NowPosGX_Get( fldobj );
	lact->ldata.gy = FieldOBJ_NowPosGY_Get( fldobj );
	lact->ldata.gz = FieldOBJ_NowPosGZ_Get( fldobj );
	lact->ldata.mdata_id = mdata_id;
	lact->fldobj = fldobj;
	lact->eoa = tw_LinkPosActEoaAdd( tw, lact );
	
	FieldOBJ_NotZoneDeleteSet( fldobj, TRUE );
	FieldOBJ_StatusBitSet_FellowHit( fldobj, FALSE );
	FieldOBJ_StatusBitSet_HeightGetOFF( fldobj, TRUE );
	FieldOBJ_MoveBitSet_AttrGetOFF( fldobj, TRUE );
}

//--------------------------------------------------------------
/**
 * 接続石 EOA追加
 * @param	tw	TORNWORLD_WORK
 * @param	lact	TW_LINKPOS_ACTOR
 * @retval	EOA_PTR	EOA_PTR
 */
//--------------------------------------------------------------
static EOA_PTR tw_LinkPosActEoaAdd(
		TORNWORLD_WORK *tw, TW_LINKPOS_ACTOR *lact )
{
	EOA_PTR eoa;
	EOA_LINKOBJ_HEAD head;
	
	head.gx = lact->ldata.gx;
	head.gy = lact->ldata.gy;
	head.gz = lact->ldata.gz;
	head.objtype = lact->ldata.objtype;
	head.tw = tw;
	head.flagcheck = FALSE;
	
	if( lact->live_zone_id == ZONE_ID_D34R0108 && lact->ldata.id == 1 ){
		head.flagcheck = TRUE; //イベントによるフラグチェック有り
	}
	
	eoa = FE_EoaAddNpp( tw->fsys->fes,
		&DATA_EoaLinkOBJHead, NULL, 0, &head, 0 );
	return( eoa );
}

//--------------------------------------------------------------
/**
 * 接続石　アクター　追加
 * @param	tw	TORNWORLD_WORK
 * @param	lact	TW_LINKPOS_ACTOR
 * @param	ldata	LINKPOS_DATA
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_LinkPosActAdd( TORNWORLD_WORK *tw,
	TW_LINKPOS_ACTOR *lact, const LINKPOS_DATA *ldata, u32 zone_id )
{
	lact->use = TRUE;
	lact->live_zone_id = zone_id;
	lact->ldata = *ldata;
	lact->fldobj = tw_LinkPosActFldOBJAdd( tw,
		ldata->gx, ldata->gy, ldata->gz,
		ldata->id, ldata->mdata_id, zone_id );
	lact->eoa = tw_LinkPosActEoaAdd( tw, lact );
}

//--------------------------------------------------------------
/**
 * 接続石　アクター　削除 フィールドOBJは残す
 * @param	tw		TORNWORLD_WORK
 * @param	lact	TW_LINKPOS_ACTOR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_LinkPosActDelete(
		TORNWORLD_WORK *tw, TW_LINKPOS_ACTOR *lact )
{
	if( lact->eoa != NULL ){ EOA_Delete( lact->eoa ); }
	memset( lact, 0, sizeof(TW_LINKPOS_ACTOR) );
}

//--------------------------------------------------------------
/**
 * 接続石　アクター　削除
 * @param	tw		TORNWORLD_WORK
 * @param	lact	TW_LINKPOS_ACTOR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_LinkPosActDeleteAll(
		TORNWORLD_WORK *tw, TW_LINKPOS_ACTOR *lact )
{
	if( lact->fldobj != NULL ){ FieldOBJ_Delete( lact->fldobj ); }
	tw_LinkPosActDelete( tw, lact );
}

//--------------------------------------------------------------
/**
 * 接続石　アクター　ゾーン変更後のアクターに置換
 * @param	tw	TORNWORLD_WORK
 * @param	lact	TW_LINKPOS_ACTOR
 * @param	zone_id	変更後のゾーンID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_LinkPosActZoneChangePerm(
	TORNWORLD_WORK *tw, TW_LINKPOS_ACTOR *lact, u32 zone_id )
{
	lact->live_zone_id = zone_id;
	
	{
		const LINKPOS_DATA *ldata;
		ldata = tw_ZoneLinkPosIDDataGet( zone_id, lact->ldata.zone_chg_id );
		lact->ldata = *ldata;
	}
	
	{
		FIELD_OBJ_PTR fldobj = lact->fldobj;
		FieldOBJ_NowPosGX_Set( fldobj, lact->ldata.gx );
		FieldOBJ_NowPosGY_Set( fldobj, lact->ldata.gy );
		FieldOBJ_NowPosGZ_Set( fldobj, lact->ldata.gz );
		FieldOBJ_ParamSet( fldobj, lact->ldata.id, FLDOBJ_PARAM_0 );
		FieldOBJ_ParamSet( fldobj, zone_id, FLDOBJ_PARAM_1 );
		FieldOBJ_ParamSet( fldobj, lact->ldata.mdata_id, FLDOBJ_PARAM_2 );
	}
}

//--------------------------------------------------------------
/**
 * 接続石　アクター　ゾーン変更後の削除無効フラグセット
 * @param	lact	TW_LINKPOS_ACTOR
 * @param	flag	TRUE=無効
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_LinkPosActZoneChgDelNotFlagSet(
		TW_LINKPOS_ACTOR *lact, u32 flag )
{
	lact->zone_chg_deloff = flag;
}

//--------------------------------------------------------------
/**
 * 接続石　モデルリソースチェック
 * @param	tw	TORNWORLD_WORK
 * @param	mdl_id	TW_MDL_MAX
 * @retval	BOOL	TRUE=存在している
 */
//--------------------------------------------------------------
static BOOL tw_LinkPosOBJResMdlIDSearch( TORNWORLD_WORK *tw, u32 mdl_id )
{
	int i;
	u16 type;
	const TW_OBJRES_ID *res_id;
	TW_LINKPOS_WORK *lpos = &tw->linkpos;
	GF_ASSERT( mdl_id != TW_MDL_MAX );
	
	for( i = 0; i < TW_LINKPOS_ACTOR_MAX; i++ ){
		if( lpos->act_tbl[i].use ){
			type = lpos->act_tbl[i].ldata.objtype;
			res_id = &DATA_OBJResIDTbl[type];
			
			if( mdl_id == res_id->mdl_id ){
				return( TRUE );
			}
		}	
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * 接続石　アニメリソースチェック
 * @param	tw	TORNWORLD_WORK
 * @param	anm_id	TW_ANM_MAX
 * @retval	BOOL	TRUE=存在している
 */
//--------------------------------------------------------------
static BOOL tw_LinkPosOBJResAnmIDSearch( TORNWORLD_WORK *tw, u32 anm_id )
{
	int i;
	u16 type;
	const TW_OBJRES_ID *res_id;
	TW_LINKPOS_WORK *lpos = &tw->linkpos;
	GF_ASSERT( anm_id != TW_ANM_MAX );
	
	for( i = 0; i < TW_LINKPOS_ACTOR_MAX; i++ ){
		if( lpos->act_tbl[i].use ){
			type = lpos->act_tbl[i].ldata.objtype;
			res_id = &DATA_OBJResIDTbl[type];
			
			if( anm_id == res_id->anm_id ){
				return( TRUE );
			}
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * 接続石　OBJタイプチェック
 * @param	tw	TORNWORLD_WORK
 * @param	type	TWOBJTYPE
 * @retval	BOOL	TRUE=存在している
 */
//--------------------------------------------------------------
static BOOL tw_LinkPosOBJResTypeSearch( TORNWORLD_WORK *tw, TWOBJTYPE type )
{
	int i;
	const TW_OBJRES_ID *res_id;
	TW_LINKPOS_WORK *lpos = &tw->linkpos;
	
	for( i = 0; i < TW_LINKPOS_ACTOR_MAX; i++ ){
		if( lpos->act_tbl[i].use ){
			if( lpos->act_tbl[i].ldata.objtype == type ){
				return( TRUE );
			}
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * 接続石　指定位置のアクター検索
 * @param	tw	TORNWORLD_WORK
 * @param	gx	グリッドX
 * @param	gy	グリッドY
 * @param	gz	グリッドZ
 * @retval	TW_LINKPOS_ACTOR* NULL=無し
 */
//--------------------------------------------------------------
static TW_LINKPOS_ACTOR * tw_LinkPosActPosCheck(
		TORNWORLD_WORK *tw, int gx, int gy, int gz, u32 zone_id )
{
	int i = TW_LINKPOS_ACTOR_MAX;
	TW_LINKPOS_ACTOR *act_tbl = tw->linkpos.act_tbl;
	const LINKPOS_DATA *ldata;
	
	do{
		if( act_tbl->use && act_tbl->live_zone_id == zone_id ){
			ldata = &act_tbl->ldata;
			if( ldata->gy == gy && ldata->gz == gz && ldata->gx == gx ){
				return( act_tbl );
			}
		}
		act_tbl++;
	}while( --i );
	
	return( NULL );
}

#if 0
static TW_LINKPOS_ACTOR * tw_LinkPosActPosCheck(
		TORNWORLD_WORK *tw, int gx, int gy, int gz )
{
	int i = 0;
	const LINKPOS_DATA *ldata;
	TW_LINKPOS_WORK *lpos = &tw->linkpos;
	
	for( i = 0; i < TW_LINKPOS_ACTOR_MAX; i++ ){
		if( lpos->act_tbl[i].use ){
			ldata = &lpos->act_tbl[i].ldata;
			if( ldata->gy == gy && ldata->gz == gz && ldata->gx == gx ){
				return( &lpos->act_tbl[i] );
			}
		}
	}
	
	return( NULL );
}
#endif

//--------------------------------------------------------------
/**
 * 接続石　現在情報で更新
 * @param	lact	LINKPOS_ACTOR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_LinkPosActUpdate( TW_LINKPOS_ACTOR *lact )
{
	FieldOBJ_NowPosGX_Set( lact->fldobj, lact->ldata.gx );
	FieldOBJ_NowPosGY_Set( lact->fldobj, lact->ldata.gy );
	FieldOBJ_NowPosGZ_Set( lact->fldobj, lact->ldata.gz );
	FieldOBJ_ParamSet( lact->fldobj, lact->ldata.id, FLDOBJ_PARAM_0 );
	FieldOBJ_ParamSet( lact->fldobj, lact->live_zone_id, FLDOBJ_PARAM_1 );
	FieldOBJ_ParamSet( lact->fldobj, lact->ldata.mdata_id, FLDOBJ_PARAM_2 );
}

//==============================================================================
///	接続石 EOA
//==============================================================================
//--------------------------------------------------------------
/**
 * EOA 接続石　初期化
 * @param	eoa	EOA_PTR
 * @param	wk	eoa	work
 * @retval	int FALSE=初期化失敗
 */
//--------------------------------------------------------------
static int tw_EoaLinkOBJInit( EOA_PTR eoa, void *wk )
{
	int init;
	EOA_LINKOBJ_WORK *work = wk;
	const EOA_LINKOBJ_HEAD *head = EOA_AddPtrGet( eoa );
	
	work->head = *head;
	
	work->objres_p =
		tw_OBJResDataOBJResGet( head->tw, head->objtype, &init );
	
	if( init == FALSE ){
		tw_OBJResDataOBJTypeInit( head->tw, head->objtype,
			&work->objres_p->robj, &work->objres_p->ranm );
	}
	
	FieldOBJTool_GridCenterPosGet( head->gx, head->gz, &work->pos );
	work->pos.y = GRID_SIZE_FX32( head->gy ) + GRID_HALF_FX32;
	
	{
		VecFx32 offs;
		const VecFx32 *d_offs = &DATA_OBJTypePosOffsTbl[head->objtype];
		work->pos.x += d_offs->x;
		work->pos.y += d_offs->y;
		work->pos.z += d_offs->z;
	}
	
	work->alpha = 31;
	work->alpha_on = FALSE;
	
	if( head->flagcheck == TRUE ){ //イベントフラグによるON,OFFあり
		EVENTWORK *ev = SaveData_GetEventWork( head->tw->fsys->savedata );
		if( SysFlag_TornWorldRockLiftFlagCheck(ev) == FALSE ){
			work->alpha = 0;
			work->alpha_on = TRUE;
			work->vanish = TRUE;
		}
	}
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * EOA 接続石　削除
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_EoaLinkOBJDelete( EOA_PTR eoa, void *wk )
{
	EOA_LINKOBJ_WORK *work = wk;
	
	if( tw_OBJResDataTypeAnmCheck(work->head.objtype) == TRUE ){
		FRO_ANM_DeleteAll( &work->objres_p->ranm );
	}
	
	tw_OBJResDataOBJResCheckRelease( work->head.tw, work->objres_p );
}

//--------------------------------------------------------------
/**
 * EOA 接続石　動作
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_EoaLinkOBJMove( EOA_PTR eoa, void *wk )
{
	EOA_LINKOBJ_WORK *work = wk;
	
	if( work->alpha_on == TRUE ){
		EVENTWORK *ev;
		ev = SaveData_GetEventWork( work->head.tw->fsys->savedata );
		
		if( SysFlag_TornWorldRockLiftFlagCheck(ev) == TRUE ){
			work->vanish = FALSE;
			
			if( work->alpha < 31 ){
				if( work->alpha == 0 ){
					twSndCheckPlay( SE_YABURETA_GROUND_APPEAR );
				}
				
				work->alpha_frame++;
				work->alpha = work->alpha_frame >> 1;
			}
			
			if( work->alpha >= 31 ){
				work->alpha = 31;
				work->alpha_on = FALSE;
			}
		}
	}
}

//--------------------------------------------------------------
/**
 * EOA 接続石　描画
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_EoaLinkOBJDraw( EOA_PTR eoa, void *wk )
{
	EOA_LINKOBJ_WORK *work = wk;
	
	if( work->vanish == FALSE ){
		if( work->alpha_on == TRUE ){
			const EOA_LINKOBJ_HEAD *head = &work->head;
			tw_OBJResDataMdlAlphaSetMain(
					head->tw, head->objtype, work->alpha, 1 );
			FRO_OBJ_DrawPos( &work->objres_p->robj, &work->pos );
			
			//pop
			tw_OBJResDataMdlPolIDSet( head->tw, head->objtype, 0 );
			tw_OBJResDataMdlAlphaSet( head->tw, head->objtype, 31 );
		}else{
			FRO_OBJ_DrawPos( &work->objres_p->robj, &work->pos );
		}
	}
}

//--------------------------------------------------------------
///	EOA 接続石　ヘッダー
//--------------------------------------------------------------
static const EOA_H_NPP DATA_EoaLinkOBJHead =
{
	sizeof(EOA_LINKOBJ_WORK),
	tw_EoaLinkOBJInit,
	tw_EoaLinkOBJDelete,
	tw_EoaLinkOBJMove,
	tw_EoaLinkOBJDraw,
};

//--------------------------------------------------------------
/**
 * EOA 接続石　座標ポインタ取得
 * @param	eoa	EOA_PTR
 * @retval	VecFx32*
 */
//--------------------------------------------------------------
static VecFx32 * tw_EoaLinkOBJPosPtrGet( EOA_PTR eoa )
{
	EOA_LINKOBJ_WORK *work = EOA_LocalWorkGet( eoa );
	return( &work->pos );
}

//==============================================================================
//	リフト
//==============================================================================
#if 0
//--------------------------------------------------------------
/**
 * リフト移動イベントチェック
 * @param	tw	TORNWORLD_WORK
 * @param	gx	自機グリッドX
 * @param	gy	自機グリッドY
 * @param	gz	自機グリッドZ
 * @retval	BOOL	TRUE=起動
 */
//--------------------------------------------------------------
static BOOL tw_EventMoveCheck_Lift(
		TORNWORLD_WORK *tw, int gx, int gy, int gz )
{
//	LINKPOS_HITDATA *hit = ( tw_LinkPosActHitDataGet
	
	if( hit == NULL ){
		return( FALSE );
	}
	
	TW_LINKPOS_ACTOR *lact = tw_LinkPosActPosCheck( tw, gx, gy, gz );
	
	if( lact != NULL ){
		if( lact->ldata.move_type == LPOSMOVE_LIFT ){
			tw_EventSet_LiftMove( tw, lact );
			return( TRUE );
		}
	}
	return( FALSE );
}

enum
{
	SEQNO_LIFTM_INIT = 0,
	SEQNO_LIFTM_MOVE,
	SEQNO_LIFTM_END,
	SEQNO_LIFTM_MAX,
};

//--------------------------------------------------------------
///	TW_LIFTMOVE_WORK
//--------------------------------------------------------------
typedef struct
{
	u16 seq_no;
	s16 gx_end;
	s16 gy_end;
	s16 gz_end;
	VecFx32 count;
	VecFx32 max_count;
	VecFx32 move_val;
	TW_LINKPOS_ACTOR *lact;
}TW_LIFTMOVE_WORK;

static PROCRET (* const DATA_EvLiftMoveProcTbl[SEQNO_LIFTM_MAX])( TORNWORLD_WORK *tw, TW_LIFTMOVE_WORK *work );

//--------------------------------------------------------------
/**
 * リフト移動　イベントセット
 * @param	
 * @retval
 */
//--------------------------------------------------------------
static void tw_EventSet_LiftMove(
		TORNWORLD_WORK *tw, TW_LINKPOS_ACTOR *lact )
{
	TW_LIFTMOVE_WORK *work;
	work = tw_EventWorkInit( tw, sizeof(TW_LIFTMOVE_WORK) );
	work->lact = lact;
	FieldEvent_Set( tw->fsys, tw_GMEVENT_LiftMove, tw );
}

//--------------------------------------------------------------
/**
 * リフト移動　イベント
 * @param	event	GMEVENT_CONTROL
 * @retval	BOOL	TRUE=終了
 */
//--------------------------------------------------------------
static BOOL tw_GMEVENT_LiftMove( GMEVENT_CONTROL *event )
{
	PROCRET ret;
	TORNWORLD_WORK *tw = FieldEvent_GetSpecialWork( event );
	TW_LIFTMOVE_WORK *work = tw_EventWorkGet( tw );
	
	do{
		ret = DATA_EvLiftMoveProcTbl[work->seq_no]( tw, work );
	}while( ret == RET_CONTINUE );
	
	if( ret == RET_END ){
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * リフト移動イベント　初期化
 * @param	tw	TORNWORLD_WORK
 * @param	work	TW_LIFTMOVE_WORK
 * @retval	PROCRET PROCRET
 */
//--------------------------------------------------------------
static PROCRET EvLiftMove_Init( TORNWORLD_WORK *tw, TW_LIFTMOVE_WORK *work )
{
	FIELD_OBJ_PTR fldobj;
	const LINKPOS_HITDATA *mdata_p;
	TW_LINKPOS_ACTOR *lact = work->lact;
		
	GF_ASSERT( work->lact->ldata.move_type == LPOSMOVE_LIFT );
#if 0
	mdata_p = tw_ZoneLinkPosHitDataGet(
		twNowZoneID(tw), lact->ldata.mdata_no );
#endif
	
	fldobj = Player_FieldOBJGet( tw->fsys->player );
	FieldOBJ_StatusBitSet_HeightGetOFF( fldobj, TRUE );
	
	work->gx_end = FieldOBJ_NowPosGX_Get( fldobj ) + mdata_p->gx_val;
	work->gy_end = FieldOBJ_NowPosGY_Get( fldobj ) + mdata_p->gy_val;
	work->gz_end = FieldOBJ_NowPosGZ_Get( fldobj ) + mdata_p->gz_val;
	work->max_count.x = GRID_SIZE_FX32( mdata_p->gx_val );
	work->max_count.y = GRID_SIZE_FX32( mdata_p->gy_val );
	work->max_count.z = GRID_SIZE_FX32( mdata_p->gz_val );
	work->move_val = mdata_p->move_val;
	
	work->seq_no = SEQNO_LIFTM_MOVE;
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * リフト移動イベント	移動
 * @param	tw	TORNWORLD_WORK
 * @param	work	TW_LIFTMOVE_WORK
 * @retval	PROCRET PROCRET
 */
//--------------------------------------------------------------
static PROCRET EvLiftMove_Move( TORNWORLD_WORK *tw, TW_LIFTMOVE_WORK *work )
{
	VecFx32 j_pos;
	FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( tw->fsys->player );
	VecFx32 *e_pos = tw_EoaLinkOBJPosPtrGet( work->lact->eoa );
	
	FieldOBJ_VecPosGet( fldobj, &j_pos );
	
	if( work->count.x != work->max_count.x ){
		work->count.x += work->move_val.x;
		j_pos.x += work->move_val.x;
		e_pos->x += work->move_val.x;
	}
	
	if( work->count.y != work->max_count.y ){
		work->count.y += work->move_val.y;
		j_pos.y += work->move_val.y;
		e_pos->y += work->move_val.y;
	}
	
	if( work->count.z != work->max_count.z ){
		work->count.z += work->move_val.z;
		j_pos.z += work->move_val.z;
		e_pos->z += work->move_val.z;
	}
	
	twFldOBJVecPosUpdate( fldobj, &j_pos );
	
	if(	work->count.x == work->max_count.x &&
		work->count.y == work->max_count.y &&
		work->count.z == work->max_count.z ){
		work->seq_no = SEQNO_LIFTM_END;
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * リフト移動イベント	終了
 * @param	tw	TORNWORLD_WORK
 * @param	work	TW_LIFTMOVE_WORK
 * @retval	PROCRET PROCRET
 */
//--------------------------------------------------------------
static PROCRET EvLiftMove_End( TORNWORLD_WORK *tw, TW_LIFTMOVE_WORK *work )
{
	u32 type;
	const LINKPOS_HITDATA *mdata_p;
	FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( tw->fsys->player );
	FieldOBJ_NowPosGX_Set( fldobj, work->gx_end );
	FieldOBJ_NowPosGY_Set( fldobj, work->gy_end );
	FieldOBJ_NowPosGZ_Set( fldobj, work->gz_end );
	FieldOBJ_GPosUpdate( fldobj );
	
#if 0	
	mdata_p = tw_ZoneLinkPosMoveDataGet(
			twNowZoneID(tw), work->lact->ldata.mdata_no );
#else
	mdata_p = NULL;
#endif
	
	work->lact->ldata.gx += mdata_p->gx_val;
	work->lact->ldata.gy += mdata_p->gy_val;
	work->lact->ldata.gz += mdata_p->gz_val;
	work->lact->ldata.mdata_no = mdata_p->after_move_no;
	tw_LinkPosActUpdate( work->lact );
	
	tw_MapDataGndDataPosDataSet(
		tw, work->gx_end, work->gy_end, work->gz_end, TW_GNDTYPE_MAX );
	
	type = tw_MapDataGndDataGndTypeNowGet( tw );
	type = twGndTypeHeroType( type );
	Player_MoveBitSet_TWorld( tw->fsys->player, type );
	
	if( type != HEROTWTYPE_GROUND ){ //特殊地形ではない
		FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( tw->fsys->player );
		FieldOBJ_StatusBitSet_HeightGetOFF( fldobj, FALSE );
	}
	
	return( RET_END );
}

//--------------------------------------------------------------
///	リフト移動　処理テーブル
//--------------------------------------------------------------
static PROCRET (* const DATA_EvLiftMoveProcTbl[SEQNO_LIFTM_MAX])( TORNWORLD_WORK *tw, TW_LIFTMOVE_WORK *work ) =
{
	EvLiftMove_Init,
	EvLiftMove_Move,
	EvLiftMove_End
};
#endif

//==============================================================================
//	イベント座標データ
//==============================================================================
#if 0
//--------------------------------------------------------------
/**
 * ゾーン別座標イベントデータを取得
 * @param	zone_id	ゾーンID
 * @retval	TW_EVPOSDATA	NULL=イベント無し
 */
//--------------------------------------------------------------
static const TW_EVPOSDATA * DATA_TwEvPosDataGet( u32 zone_id )
{
	return( NULL );
}

//--------------------------------------------------------------
/**
 * 座標イベント発動チェック
 * @param	tw	TORNWORLD_WORK
 * @param	gx	グリッドX
 * @param	gy	グリッドY
 * @param	gz	グリッドZ
 * @retval	BOOL TRUE=起動
 */
//--------------------------------------------------------------
static BOOL tw_EventHitCheck_EvPos(
	TORNWORLD_WORK *tw, int gx, int gy, int gz )
{
	const TW_EVPOSDATA *evpos = DATA_TwEvPosDataGet( twNowZoneID(tw) );
	
	if( evpos != NULL ){
		while( evpos->scr_p != NULL ){
			if( evpos->gy == gy && evpos->gz == gz && evpos->gx == gx ){
				//スクリプト発動
				return( TRUE );
			}
			evpos++;
		}
	}
	
	return( FALSE );
}
#endif

//==============================================================================
//	スクリプト
//==============================================================================
//--------------------------------------------------------------
/**
 * スクリプト　初期化
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ScrInit( TORNWORLD_WORK *tw )
{
	TW_SCRWORK *work = &tw->script;
	memset( work, 0, sizeof(TW_SCRWORK) );
}

//--------------------------------------------------------------
/**
 * スクリプト　削除
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ScrDelete( TORNWORLD_WORK *tw )
{
}

//--------------------------------------------------------------
/**
 * スクリプト　イベント発動
 * @param	tw	TORNWORLD_WORK
 * @param	srdata	TW_SCRDATA
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ScrSet( TORNWORLD_WORK *tw, const TW_SCRDATA *scrdata )
{
	TW_SCRWORK *work = &tw->script;
	
	GF_ASSERT( work->flag == FALSE );
	GF_ASSERT( scrdata != NULL );
	
	work->flag = TRUE;
	work->scr_tbl_no = 0;
	work->scr_seq_no = 0;
	work->scrdata_p = scrdata;
}

//--------------------------------------------------------------
/**
 * スクリプト　スクリプトプロセスワーククリア
 * @param	tw	TORNWORLD_WORK
 * @param	size	クリアサイズ
 * @retval	work	スクリプトワーク
 */
//--------------------------------------------------------------
static void * tw_ScrProcWorkInit( TORNWORLD_WORK *tw, u32 size )
{
	#ifdef DEBUG_TW_PRINTF
	if( size > TW_SCRPROCWORK_SIZE ){
		OS_Printf("tw script work size over %d\n",size-TW_SCRPROCWORK_SIZE);
	}
	#endif
	GF_ASSERT( size < TW_SCRPROCWORK_SIZE );
	
	{
		u8 *work = tw->script.proc_work;
		memset( work, 0, size );
		return( work );
	}
}

//--------------------------------------------------------------
/**
 * スクリプト　スクリプトプロセスワーク取得
 * @param	tw	TORNWORLD_WORK
 * @retval	work	スクリプトワーク
 */
//--------------------------------------------------------------
static void * tw_ScrProcWorkGet( TORNWORLD_WORK *tw )
{
	u8 *work = tw->script.proc_work;
	return( work );
}

//--------------------------------------------------------------
/**
 * スクリプト	メイン
 * @param	tw	TORNWORLD_WORK
 * @retval	BOOL	TRUE=終了
 */
//--------------------------------------------------------------
static BOOL tw_ScrMain( TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev )
{
	PROCRET ret;
	TWSCRCODE code;
	TW_SCRWORK *work;
	const TW_SCR_PROC *proc;
	const TW_SCRDATA *data;
	
	work = &tw->script;
	data = work->scrdata_p;
	GF_ASSERT( data != NULL );
	
	while( data[work->scr_tbl_no].code != TWSCRCODE_MAX ){
		code = data[work->scr_tbl_no].code;
		proc = DATA_ScrCmdProcTbl[code];
		
		do{
			ret = proc[work->scr_seq_no]( tw, ev,
				&work->scr_seq_no, data[work->scr_tbl_no].cmddata_p );
		}while( ret == RET_CONTINUE );
		
		if( ret == RET_NON ){
			return( FALSE );
		}
		
		work->scr_seq_no = 0;
		work->scr_tbl_no++;
	}
	
	work->flag = FALSE;
	work->scr_tbl_no = 0;
	return( TRUE );
}

//==============================================================================
//	スクリプト　座標起動スクリプト
//==============================================================================
//--------------------------------------------------------------
/**
 * ゾーン別座標起動スクリプトデータテーブル取得
 * @param	zone_id	zone id
 * @retval	SCRPOSDATA	NULL=無し
 */
//--------------------------------------------------------------
static const SCRPOSDATA * tw_ScrPosDataZoneGet( u32 zone_id )
{
	const SCRPOSDATA_ZONE *tbl = DATA_ScrPosDataZoneTbl;
	
	while( tbl->zone_id != ZONE_ID_MAX ){
		if( tbl->zone_id == zone_id ){
			return( tbl->scrpos );
		}
		tbl++;
	}
	
	return( NULL );
}

//--------------------------------------------------------------
/**
 * イベント　座標起動スクリプトチェック
 * @param	tw	TORNWORLD_WORK
 * @param	gx	自機グリッドX
 * @param	gy	自機グリッドY
 * @param	gz	自機グリッドZ
 * @retval	BOOL	TRUE=起動
 */
//--------------------------------------------------------------
static BOOL tw_EventPosCheck_Script(
		TORNWORLD_WORK *tw, int gx, int gy, int gz )
{
	EVENTWORK *ev = SaveData_GetEventWork( tw->fsys->savedata );
	const SCRPOSDATA *data = tw_ScrPosDataZoneGet( twNowZoneID(tw) );
	
	if( data != NULL ){
		u16 type,flag;
		
		while( data->scr_p != NULL ){
			if( gy == data->gy && gz == data->gz && gx == data->gx ){
				type = data->check_flag_type;
				flag = data->check_flag;
				
				if( twFlagCheck(tw,type,flag) == TRUE ){
					tw_EventPosScrSet( tw, data );
					return( TRUE );
				}
			}
			data++;
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * イベント　座標起動スクリプトセット　メイン
 * @param	tw	TORNWORLD_WORK
 * @param	data	SCRPOSDATA
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_EventPosScrSetMain(
		TORNWORLD_WORK *tw, const TW_SCRDATA *scr_p )
{
	tw_ScrSet( tw, scr_p );
	FieldEvent_Set( tw->fsys, tw_GMEVENT_ScrPosWait, tw );
}

//--------------------------------------------------------------
/**
 * イベント　座標起動スクリプトセット
 * @param	tw	TORNWORLD_WORK
 * @param	data	SCRPOSDATA
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_EventPosScrSet(
		TORNWORLD_WORK *tw, const SCRPOSDATA *scrpos )
{
	tw_EventPosScrSetMain( tw, scrpos->scr_p );
	
	#ifdef DEBUG_TW_PRINTF
	OS_Printf( "座標起動スクリプト発動 START_SCRIPT_TYPE = " );
	OS_Printf( "%d\n", scrpos->scr_p->code );
	#endif
}

//--------------------------------------------------------------
/**
 * イベント　座標軌道スクリプト管理
 * @param	event	GMEVENT_CONTROL
 * @retval	BOOL	TRUE=終了
 */
//--------------------------------------------------------------
static BOOL tw_GMEVENT_ScrPosWait( GMEVENT_CONTROL *event )
{
	TORNWORLD_WORK *tw = FieldEvent_GetSpecialWork( event );
	
	if( tw_ScrMain(tw,event) == TRUE ){
		return( TRUE );
	}
	
	return( FALSE );
}

//==============================================================================
//	スクリプト　アニメーションコマンドセット
//==============================================================================
//--------------------------------------------------------------
///	スクリプト　アニメーションコマンド　処理番号
//--------------------------------------------------------------
enum
{
	SEQNO_ACMD_INIT = 0,
	SEQNO_ACMD_SET,
	SEQNO_ACMD_WAIT,
	SEQNO_ACMD_MAX,
};

//--------------------------------------------------------------
///	SCRWORK_ACMD
//--------------------------------------------------------------
typedef struct
{
	int dmy;
}SCRWORK_ACMD;

//--------------------------------------------------------------
/**
 * スクリプト　アニメーションコマンド　初期化
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrAcmdSet_Init(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	SCRWORK_ACMD *work = tw_ScrProcWorkInit( tw, sizeof(SCRWORK_ACMD) );
	*seq_no = SEQNO_ACMD_SET;
	return( RET_CONTINUE );
}

//--------------------------------------------------------------
/**
 * スクリプト　アニメーションコマンド　セット
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrAcmdSet_Set(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	FIELD_OBJ_PTR fldobj;
	const SCRDATA_ACMD *dat = data;
	SCRWORK_ACMD *work = tw_ScrProcWorkGet( tw );
	
	fldobj = FieldOBJSys_OBJIDSearch( tw->fsys->fldobjsys, dat->obj_id );
	GF_ASSERT( fldobj != NULL );
	
	if( FieldOBJ_AcmdSetCheck(fldobj) == TRUE ){
		FieldOBJ_AcmdSet( fldobj, dat->code );
		*seq_no = SEQNO_ACMD_WAIT;
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　アニメーションコマンド　待ち
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrAcmdSet_Wait(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	FIELD_OBJ_PTR fldobj;
	const SCRDATA_ACMD *dat = data;
	SCRWORK_ACMD *work = tw_ScrProcWorkGet( tw );
	
	fldobj = FieldOBJSys_OBJIDSearch( tw->fsys->fldobjsys, dat->obj_id );
	GF_ASSERT( fldobj != NULL );
	
	if( FieldOBJ_AcmdEnd(fldobj) == TRUE ){
		return( RET_END );
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
///	スクリプトテーブル　アニメーションコマンド
//--------------------------------------------------------------
static const TW_SCR_PROC DATA_ScrTbl_Acmd[SEQNO_ACMD_MAX] =
{
	ScrAcmdSet_Init,
	ScrAcmdSet_Set,
	ScrAcmdSet_Wait,
};

//==============================================================================
//	スクリプト　リフト
//==============================================================================
//--------------------------------------------------------------
///	スクリプト　リフト　処理番号
//--------------------------------------------------------------
enum
{
	SEQNO_LIFT_INIT = 0,
	SEQNO_LIFT_START_SHAKE,
	SEQNO_LIFT_MOVE,
	SEQNO_LIFT_END,
	SEQNO_LIFT_MAX,
};

//--------------------------------------------------------------
///	SCRWORK_LIFT
//--------------------------------------------------------------
typedef struct
{
	VecFx32 count;
	VecFx32 max_count;
	VecFx32 move_val;
	TW_LINKPOS_ACTOR *lact;
	
	fx32 shake_save_oy_jiki;
	fx32 shake_save_y_lift;
	fx32 shake_offs_y;
	u32 shake_count;
}SCRWORK_LIFT;

//--------------------------------------------------------------
/**
 * スクリプト　リフト　初期化
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrLift_Init(
		TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	const SCRDATA_LIFTMOVE *dat = data;
	SCRWORK_LIFT *work = tw_ScrProcWorkInit( tw, sizeof(SCRWORK_LIFT) );
	
	work->lact = tw_LinkPosActIDGet( tw, dat->move_id, twNowZoneID(tw) );
	GF_ASSERT( work->lact != NULL );
	
	work->max_count.x = GRID_SIZE_FX32( dat->gx_val );
	work->max_count.y = GRID_SIZE_FX32( dat->gy_val );
	work->max_count.z = GRID_SIZE_FX32( dat->gz_val );
	work->move_val = dat->move_val;
	
	work->shake_offs_y = NUM_FX32( 3 );
	
	{
		VecFx32 *pos = tw_EoaLinkOBJPosPtrGet( work->lact->eoa );
		work->shake_save_y_lift = pos->y;
	}
	
	if( dat->jiki_move_flag == TRUE ){
		VecFx32 offs;
		FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( tw->fsys->player );
		FieldOBJ_StatusBitSet_HeightGetOFF( fldobj, TRUE );
		
		FieldOBJ_VecDrawOffsOutSideGet( fldobj, &offs );
		work->shake_save_oy_jiki = offs.y;
	}
	
	Snd_SePlay( SE_YABURETA_MOVE2 );
	
	*seq_no = SEQNO_LIFT_START_SHAKE;
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　リフト　起動ゆれ
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrLift_StartShake(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	FIELD_OBJ_PTR fldobj = NULL;
	const SCRDATA_LIFTMOVE *dat = data;
	SCRWORK_LIFT *work = tw_ScrProcWorkGet( tw );
	VecFx32 *lpos_p = tw_EoaLinkOBJPosPtrGet( work->lact->eoa );
	
	if( dat->jiki_move_flag == TRUE ){
		fldobj = Player_FieldOBJGet( tw->fsys->player );
	}
	
	lpos_p->y = work->shake_save_y_lift + work->shake_offs_y;
	
	if( fldobj != NULL ){
		VecFx32 pos;
		FieldOBJ_VecDrawOffsOutSideGet( fldobj, &pos );
		pos.y = work->shake_save_oy_jiki + work->shake_offs_y;
		FieldOBJ_VecDrawOffsOutSideSet( fldobj, &pos );
		
		FieldOBJ_VecPosGet( fldobj, &pos );
		twFldOBJVecPosUpdate( fldobj, &pos );
	}
	
	work->shake_offs_y = -work->shake_offs_y;
	
	if( work->shake_offs_y >= 0 ){
		if( work->shake_offs_y > NUM_FX32(1) ){
			work->shake_offs_y -= NUM_FX32( 1 );
		}else{
			work->shake_count++;
			
			if( work->shake_count >= 4 ){
				work->shake_offs_y = 0;
			}
		}
	
		if( work->shake_offs_y == 0 ){
			lpos_p->y = work->shake_save_y_lift;
			
			if( fldobj != NULL ){
				VecFx32 pos;
				FieldOBJ_VecDrawOffsOutSideGet( fldobj, &pos );
				pos.y = work->shake_save_oy_jiki;
				FieldOBJ_VecDrawOffsOutSideSet( fldobj, &pos );
				
				FieldOBJ_VecPosGet( fldobj, &pos );
				twFldOBJVecPosUpdate( fldobj, &pos );
			}
			
			*seq_no = SEQNO_LIFT_MOVE;
		}
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　リフト　移動
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrLift_Move(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	VecFx32 j_val = {0,0,0};
	const SCRDATA_LIFTMOVE *dat = data;
	SCRWORK_LIFT *work = tw_ScrProcWorkGet( tw );
	VecFx32 *lpos_p = tw_EoaLinkOBJPosPtrGet( work->lact->eoa );
	
	if( work->count.x != work->max_count.x ){
		work->count.x += work->move_val.x;
		lpos_p->x += work->move_val.x;
		if( dat->jiki_move_flag == TRUE ){
			j_val.x += work->move_val.x;
		}
	}
	
	if( work->count.y != work->max_count.y ){
		work->count.y += work->move_val.y;
		lpos_p->y += work->move_val.y;
		if( dat->jiki_move_flag == TRUE ){
			j_val.y += work->move_val.y;
		}
	}
	
	if( work->count.z != work->max_count.z ){
		work->count.z += work->move_val.z;
		lpos_p->z += work->move_val.z;
		if( dat->jiki_move_flag == TRUE ){
			j_val.z += work->move_val.z;
		}
	}
	
	if( dat->jiki_move_flag == TRUE ){
		VecFx32 pos;
		FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( tw->fsys->player );
		FieldOBJ_VecPosGet( fldobj, &pos );
		pos.x += j_val.x;
		pos.y += j_val.y;
		pos.z += j_val.z;
		twFldOBJVecPosUpdate( fldobj, &pos );
	}
	
	if( work->count.x == work->max_count.x &&
		work->count.y == work->max_count.y &&
		work->count.z == work->max_count.z ){
		*seq_no = SEQNO_LIFT_END;
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　リフト　終了
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrLift_End(
		TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	const SCRDATA_LIFTMOVE *dat = data;
	
	if( dat->jiki_move_flag == TRUE ){
		FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( tw->fsys->player );
		int gx = FieldOBJ_NowPosGX_Get( fldobj ) + dat->gx_val;
		int gy = FieldOBJ_NowPosGY_Get( fldobj ) + G_GRID_H_GRID(dat->gy_val);
		int gz = FieldOBJ_NowPosGZ_Get( fldobj ) + dat->gz_val;
		FieldOBJ_NowPosGX_Set( fldobj, gx );
		FieldOBJ_NowPosGY_Set( fldobj, gy );
		FieldOBJ_NowPosGZ_Set( fldobj, gz );
		FieldOBJ_GPosUpdate( fldobj );
		
		{
			u32 type;
			PLAYER_STATE_PTR jiki = tw->fsys->player;
			gy = H_GRID_G_GRID( gy );
			tw_MapDataGndDataPosDataSet( tw, gx, gy, gz, TW_GNDTYPE_MAX );
			type = tw_MapDataPosGndTypeGet( tw, gx, gy, gz );
			type = twGndTypeHeroType( type );
			Player_MoveBitSet_TWorld( jiki, type );	//地形別に自機動作変更
			
			if( type == HEROTWTYPE_ON ){
				FieldOBJ_StatusBitSet_HeightGetOFF( fldobj, FALSE );
			}else{
				FieldOBJ_StatusBitSet_HeightGetOFF( fldobj, TRUE );
			}
		}
	}
	
	Snd_SeStopBySeqNo( SE_YABURETA_MOVE2, 0 );
	return( RET_END );
}

//--------------------------------------------------------------
///	スクリプトテーブル　リフト
//--------------------------------------------------------------
static const TW_SCR_PROC DATA_ScrTbl_Lift[SEQNO_LIFT_MAX] =
{
	ScrLift_Init,
	ScrLift_StartShake,
	ScrLift_Move,
	ScrLift_End,
};

//==============================================================================
//	スクリプト　フィールドOBJ 追加
//==============================================================================
//--------------------------------------------------------------
///	SCRDATA_FLDOBJADD
//--------------------------------------------------------------
typedef struct
{
	u16 zone_id;
	u16 obj_id;
}SCRDATA_FLDOBJADD;

//--------------------------------------------------------------
//	スクリプト　フィールドOBJ追加　処理番号
//--------------------------------------------------------------
enum
{
	SEQNO_FLDOBJADD_INIT = 0,
	SEQNO_FLDOBJADD_MAX,
};

//--------------------------------------------------------------
/**
 * スクリプト　フィールドOBJ追加　初期化
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrFldOBJAdd_Init(
		TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	const SCRDATA_FLDOBJADD *dat = data;
	tw_FldOBJEventAdd( tw, dat->zone_id, dat->obj_id );
	return( RET_END );
}

//--------------------------------------------------------------
///	スクリプトテーブル　フィールドOBJ追加
//--------------------------------------------------------------
static const TW_SCR_PROC DATA_ScrTbl_FldOBJAdd[SEQNO_FLDOBJADD_MAX] =
{
	ScrFldOBJAdd_Init,
};

//==============================================================================
//	スクリプト　フィールドOBJ　削除
//==============================================================================
//--------------------------------------------------------------
///	SCRDATA_FLDOBJDEL
//--------------------------------------------------------------
typedef struct
{
	u32 obj_id;
}SCRDATA_FLDOBJDEL;

//--------------------------------------------------------------
///	スクリプト　フィールドOBJ削除　処理番号
//--------------------------------------------------------------
enum
{
	SEQNO_FLDOBJDEL_INIT = 0,
	SEQNO_FLDOBJDEL_MAX,
};

//--------------------------------------------------------------
/**
 * スクリプト　フィールドOBJ削除　初期化
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrFldOBJDel_Init(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	const SCRDATA_FLDOBJDEL *dat = data;
	FldTornWorld_EvFldOBJDelete( tw->fsys, dat->obj_id );
	return( RET_END );
}

//--------------------------------------------------------------
///	スクリプトテーブル　フィールドOBJ削除
//--------------------------------------------------------------
static const TW_SCR_PROC DATA_ScrTbl_FldOBJDelete[SEQNO_FLDOBJDEL_MAX] =
{
	ScrFldOBJDel_Init,
};

#if 0
//==============================================================================
//	スクリプト　会話メッセージ表示
//==============================================================================
//--------------------------------------------------------------
///	SCRWORK_TALKMSG
//--------------------------------------------------------------
typedef struct
{
	int dmy;
	GF_BGL_BMPWIN *msgwin;
	MSGDATA_MANAGER *msgman;
	STRBUF *strbuf;
}SCRWORK_TALKMSG;

//--------------------------------------------------------------
/**
 * メッセージ表示　初期化
 * @param	
 * @retval
 */
//--------------------------------------------------------------
static PROCRET ScrTalkMsg_Init(
		TORNWORLD_WORK *tw, u16 *seq_no, const void *data )
{
	STRBUF **pBuf;
	SCRWORK_TALKMSG *work;
	FIELDSYS_WORK *fsys = tw->fsys;
	
	work = tw_ScrProcWorkInit( tw, sizeof(SCRWORK_TALKMSG) );
	pBuf = GetEvScriptWorkMemberAdrs( fsys, ID_EVSCR_MSGBUF );
	work->strbuf = *pBuf;
	work->msgwin = GetEvScriptWorkMemberAdrs( fsys, ID_EVSCR_MSGWINDAT );
}
#endif

//==============================================================================
//	ギラティナ影
//==============================================================================
//--------------------------------------------------------------
///	EOA_GIRAS_HEAD
//--------------------------------------------------------------
typedef struct
{
	TORNWORLD_WORK *tw;
	TW_OBJRES *objres_p;
	SCRDATA_GIRAS data;
}EOA_GIRAS_HEAD;

//--------------------------------------------------------------
///	EOA_GIRAS_WORK
//--------------------------------------------------------------
typedef struct
{
	EOA_GIRAS_HEAD head;
	u16 seq_no;
	u16 vanish;
	int frame;
	int end_flag;
	ROTATE rotate;
}EOA_GIRAS_WORK;

#if 0
typedef struct
{
	EOA_GIRAS_HEAD head;
	
	u16 seq_no;
	u16 vanish;
	s16 dir;
	s16 count_max;
	fx32 count;
	VecFx32 move;
	ROTATE rotate;
	
	u32 debug_count;
}EOA_GIRAS_WORK;
#endif

//--------------------------------------------------------------
///	ギラティナ影　スクリプトイベント用データ
//--------------------------------------------------------------
static const SCRDATA_GIRAS DATA_EvScrGirasTbl[TW_GIRASNO_MAX] =
{
	{	//TW_GIRASNO_0101_00
		54-48,289+8,42,GIRAS_DIR_RIGHT,GIRAS_SE_VOICE,
		{FX32_ONE,FX32_ONE,FX32_ONE},
		{GIRAS_MOVE_VALUE,0,0},64
	},
};

//--------------------------------------------------------------
/**
 * ギラティナ　影　追加
 * @param	fsys	FIELDSYS_WORK
 * @param	no		エフェクト番号
 * @retval	nothing
 */
//--------------------------------------------------------------
void FldTornWorld_GirasSet( FIELDSYS_WORK *fsys, u16 no )
{
	TORNWORLD_WORK *tw;
	GF_ASSERT( no < TW_GIRASNO_MAX );
	tw = fsys->fldmap->Work;
	tw_GirasActAdd( tw, &DATA_EvScrGirasTbl[no] );
}

//--------------------------------------------------------------
/**
 * ギラティナ　影　削除
 * @param	fsys	FIELDSYS_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
void FldTornWorld_GirasDelete( FIELDSYS_WORK *fsys )
{
	TORNWORLD_WORK *tw;
	tw = fsys->fldmap->Work;
	tw_GirasActDelete( tw );
}

//--------------------------------------------------------------
/**
 * ギラティナ　影　グラフィック初期化
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_GirasGraphicInit( TORNWORLD_WORK *tw )
{
	int init;
	GIRASACT_WORK *work = &tw->giras_act;
	
	if( work->gp_init == FALSE ){
		work->objres_p = tw_OBJResDataOBJResGet( tw, TWOBJTYPE_GIRAS, &init );
	
		if( init == FALSE ){
			tw_OBJResDataOBJTypeInit( tw, TWOBJTYPE_GIRAS,
				&work->objres_p->robj, &work->objres_p->ranm );
		}
		work->gp_init = TRUE;
	}
}

//--------------------------------------------------------------
/**
 * ギラティナ　影　グラフィック削除
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_GirasGraphicDelete( TORNWORLD_WORK *tw )
{
	int init;
	GIRASACT_WORK *work = &tw->giras_act;
	
	if( work->gp_init == TRUE ){
		tw_OBJResDataOBJResCheckRelease( tw, work->objres_p );
		tw_OBJResDataAnmDelete( tw, TW_ANM_GIRAS );
		tw_OBJResDataMdlDelete( tw, TW_MDL_GIRAS );
		work->gp_init = FALSE;
	}
}

//--------------------------------------------------------------
/**
 * ギラティナ　影　EOA追加
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_GirasActAdd( TORNWORLD_WORK *tw, const SCRDATA_GIRAS *data_p )
{
	EOA_GIRAS_HEAD head;
	GIRASACT_WORK *work = &tw->giras_act;
	
	tw_GirasGraphicInit( tw );
	
	head.tw = tw;
	head.data = *data_p;
	head.objres_p = work->objres_p;
	
	work->eoa = FE_EoaAddNpp(
		tw->fsys->fes, &DATA_EoaGirasHeader, NULL, 0, &head, 0 );
}

//--------------------------------------------------------------
/**
 * ギラティナ　影　動作終了チェック
 * @param	tw	TORNWORLD_WORK
 * @retval	BOOL	TRUE=終了
 */
//--------------------------------------------------------------
static BOOL tw_GirasActMoveWait( TORNWORLD_WORK *tw )
{
	EOA_GIRAS_WORK *eoa_work;
	GIRASACT_WORK *work = &tw->giras_act;
	GF_ASSERT( work->eoa != NULL );
	
	eoa_work = EOA_LocalWorkGet( work->eoa );
	return( eoa_work->end_flag );
}

//--------------------------------------------------------------
/**
 * ギラティナ　影　初期化
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_GirasActInit( TORNWORLD_WORK *tw )
{
	GIRASACT_WORK *work = &tw->giras_act;
}

//--------------------------------------------------------------
/**
 * ギラティナ　影　削除
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_GirasActDelete( TORNWORLD_WORK *tw )
{
	GIRASACT_WORK *work = &tw->giras_act;
	
	if( work->eoa != NULL ){
		EOA_Delete( work->eoa );
	}
	
	tw_GirasGraphicDelete( tw );
}

//--------------------------------------------------------------
/**
 * ギラティナ　使用モデルIDチェック
 * @param	tw	TORNWORLD_WORK
 * @param	id	TW_MDL_MAX
 * @retval	BOOL TRUE=使用中のidである
 */
//--------------------------------------------------------------
static BOOL tw_GirasActOBJResMdlIDSearch( TORNWORLD_WORK *tw, u32 mdl_id )
{
	GIRASACT_WORK *work = &tw->giras_act;
	
	GF_ASSERT( mdl_id != TW_MDL_MAX );
	
	if( work->gp_init == TRUE ){
		if( mdl_id == TW_MDL_GIRAS ){
			return( TRUE );
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * ギラティナ　使用モデルIDチェック
 * @param	tw	TORNWORLD_WORK
 * @param	id	TW_MDL_MAX
 * @retval	BOOL TRUE=使用中のidである
 */
//--------------------------------------------------------------
static BOOL tw_GirasActOBJResAnmIDSearch( TORNWORLD_WORK *tw, u32 anm_id )
{
	GIRASACT_WORK *work = &tw->giras_act;
	
	GF_ASSERT( anm_id != TW_ANM_MAX );
	
	if( work->gp_init == TRUE ){
		if( anm_id == TW_ANM_GIRAS ){
			return( TRUE );
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * ギラティナ　使用OBJタイプチェック
 * @param	tw	TORNWORLD_WORK
 * @param	type	TWOBJTYPE
 * @retval	BOOL TRUE=使用中のtypeである
 */
//--------------------------------------------------------------
static BOOL tw_GirasActOBJResTypeSearch( TORNWORLD_WORK *tw, TWOBJTYPE type )
{
	GIRASACT_WORK *work = &tw->giras_act;
	
	if( work->gp_init == TRUE ){
		if( type == TWOBJTYPE_GIRAS ){
			return( TRUE );
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
///	ギラティナ影　方向別自転角度
//--------------------------------------------------------------
static const ROTATE DATA_GirasRotateTbl[GIRAS_DIR_MAX] =
{
	{0,0,0},
	{0,180,0},
	{0,90,0},
	{0,270,0},
	{90,0,0},
};

//--------------------------------------------------------------
/**
 * EOA ギラティナ影　初期化
 * @param	tw	TORNWORLD_WORK
 * @param	wk	eoa work
 * @retval	int	TRUE=初期化完了
 */
//--------------------------------------------------------------
static int tw_EoaGirasActInit( EOA_PTR eoa, void *wk )
{
	VecFx32 pos;
	
	EOA_GIRAS_WORK *work = wk;
	const EOA_GIRAS_HEAD *head = EOA_AddPtrGet( eoa );
	const SCRDATA_GIRAS *data_p;
	
	work->head = *head;
	data_p = &work->head.data;
	
//	#ifdef DEBUG_TW_PRINTF
	#if 0
	OS_Printf( "ギラティナ影 初期化 DATA_P=%xH\n", data_p );
	#endif
	
	work->vanish = TRUE;
	work->rotate = DATA_GirasRotateTbl[data_p->dir];
	pos.x = GRID_SIZE_FX32( data_p->gx );
	pos.y = GRID_SIZE_FX32( data_p->gy );
	pos.z = GRID_SIZE_FX32( data_p->gz );
	
	if( data_p->dir == GIRAS_DIR_RISE ){
		pos.x += NUM_FX32( 8 );
	}
	
	EOA_MatrixSet( eoa, &pos );
	
//	#ifdef DEBUG_TW_PRINTF
	#if 0
	OS_Printf( "ギラティナ影 GX=%d,GY=%d,GZ=%d,DIR=%d\n",
		data_p->gx,data_p->gy,data_p->gz,data_p->dir );
	#endif
	
	work->vanish = FALSE;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * EOA ギラティナ影　削除
 * @param	tw	TORNWORLD_WORK
 * @param	wk	eoa work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_EoaGirasActDelete( EOA_PTR eoa, void *wk )
{
	EOA_GIRAS_WORK *work = wk;
	FRO_ANM_AnmObjFree( &work->head.objres_p->ranm );
}

//--------------------------------------------------------------
/**
 * EOA ギラティナ影　動作 kari
 * @param	tw	TORNWORLD_WORK
 * @param	wk	eoa work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_EoaGirasActMove( EOA_PTR eoa, void *wk )
{
	VecFx32 pos;
	EOA_GIRAS_WORK *work = wk;
	const SCRDATA_GIRAS *data_p = &work->head.data;
	
	switch( work->seq_no ){
	case 0:
		//#ifdef DEBUG_TW_PRINTF
		#if 0
		OS_Printf( "ギラティナ影 動作 DATA_P=%xH\n", data_p );
		#endif
		
		if( data_p->se_play == GIRAS_SE_VOICE ){
			Snd_PMVoicePlay( MONSNO_KIMAIRAN, 0 );
		}else if( data_p->se_play == GIRAS_SE_FLIGHT ){
			Snd_SePlay( SE_YABURETA_GIRATINA_FLIGHT );
		}
		
		work->seq_no++;
	case 1:
		{
			VecFx32 pos;
			const VecFx32 *move = &data_p->move_val;
			
			EOA_MatrixGet( eoa, &pos );
			pos.x += move->x;
			pos.y += move->y;
			pos.z += move->z;
			work->frame++;
			
			if( work->frame >= data_p->frame ){
				work->seq_no++;
				work->end_flag = TRUE;
				work->vanish = TRUE;
			}
			
			EOA_MatrixSet( eoa, &pos );
		}
		break;
	case 2:
		break;
	}
	
	if( work->vanish == FALSE ){
		FRO_ANM_Play( &work->head.objres_p->ranm, FX32_ONE, ANMLOOP_ON );
	}
}

#if 0	//test
static void tw_EoaGirasActMove( EOA_PTR eoa, void *wk )
{
	EOA_GIRAS_WORK *work = wk;
	
	switch( work->seq_no ){
	case 0:								//出現タイミング初期化
		#ifdef DEBUG_GIRAS
		if( sys.trg & PAD_BUTTON_SELECT ){
			work->seq_no = 4;
		}
		#endif
		
		if( work->debug_count == 0 ){
			work->debug_count = (30 * 2) + (30 * (gf_rand()%15));
		}
		
		work->debug_count--;
		if( work->debug_count > 1 ){
			break;
		}
		
		work->debug_count = 0;
		work->seq_no++;
	case 1:								//出現
		{
			u32 rand;
			VecFx32 pos;
			
			work->move.x = 0;
			work->move.y = 0;
			work->move.z = 0;
			work->count = 0;
			work->rotate.x = 0;
			work->rotate.y = 0;
			work->rotate.z = 0;
			work->count_max = 64;
			
			Player_VecPosGet( work->head.tw->fsys->player, &pos );
			rand = gf_rand();
			work->dir = rand & 0x03;
			#ifdef DEBUG_TW_PRINTF
//			OS_Printf( "TW GIRAS DEBUG No %d \n", work->dir );
			#endif
			switch( work->dir ){
			case DIR_UP:
				pos.z += GRID_SIZE_FX32( 24 );
				work->move.z = -GIRAS_MOVE_VALUE;
				work->rotate.y = 0;
				break;
			case DIR_DOWN:
				pos.z -= GRID_SIZE_FX32( 24 );
				work->move.z = GIRAS_MOVE_VALUE;
				work->rotate.y = 180;
				break;
			case DIR_LEFT:
				pos.x += GRID_SIZE_FX32( 24 );
				work->move.x = -GIRAS_MOVE_VALUE;
				work->rotate.y = 90;
				break;
			case DIR_RIGHT:
				pos.x -= GRID_SIZE_FX32( 24 );
				work->move.x = GIRAS_MOVE_VALUE;
				work->rotate.y = 270;
				break;
			}
			
			rand = gf_rand();
			rand &= 0x01;
			
			if( rand ){
				pos.y += GRID_SIZE_FX32( 4 );
			}else{
				pos.y -= GRID_SIZE_FX32( 50 );
				pos.z -= GRID_SIZE_FX32( 25 );
				work->count_max = 192;
			}
			
			EOA_MatrixSet( eoa, &pos );
			work->vanish = FALSE;
			work->seq_no++;
		}
	case 2:
		{
			int grid;
			VecFx32 pos;
			EOA_MatrixGet( eoa, &pos );
			
			pos.x += work->move.x;
			pos.y += work->move.y;
			pos.z += work->move.z;
			
			switch( work->dir ){
			case DIR_UP:
			case DIR_DOWN:
				work->count += work->move.z;
				break;
			case DIR_LEFT:
			case DIR_RIGHT:
				work->count += work->move.x;
				break;
			}
			
			grid = SIZE_GRID_FX32( work->count );
			if( grid < 0 ){ grid = -grid; }
			
			if( grid >= work->count_max ){
				work->seq_no++;
			}
			
			EOA_MatrixSet( eoa, &pos );
		}
		break;
	case 3:
		{
			work->seq_no = 0;
			work->vanish = TRUE;
		}
		break;
	case 4:	//debug
		{
			{
				VecFx32 pos;
				Player_VecPosGet( work->head.tw->fsys->player, &pos );
				pos.z -= FX32_ONE*2;
				EOA_MatrixSet( eoa, &pos );
				
				work->vanish = FALSE;
				work->rotate.x = 0;
				work->rotate.y = 0;
				work->rotate.z = 0;
				work->seq_no++;
			}
		}
	case 5:
		{
			u32 trg = sys.trg;
			u32 cont = sys.cont;
			
			if( trg & PAD_BUTTON_SELECT ){
				work->vanish = TRUE;
				work->seq_no = 0;
				break;
			}
			
			if( (cont & PAD_BUTTON_L) == 0 ){
				break;
			}
			
			if( cont & PAD_KEY_UP ){
				work->rotate.y += 0x10;
			}else if( cont & PAD_KEY_DOWN ){
				work->rotate.y -= 0x10;
			}
			
			work->rotate.y %= 256;
			
			if( cont & PAD_KEY_LEFT ){
				work->rotate.x += 0x10;
			}else if( cont & PAD_KEY_RIGHT ){
				work->rotate.x -= 0x10;
			}
			
			work->rotate.x %= 256;
			break;
		}
	}
	
	if( work->vanish == FALSE ){
		FRO_ANM_Play( &work->head.objres_p->ranm, FX32_ONE, ANMLOOP_ON );
	}
}
#endif

//--------------------------------------------------------------
/**
 * EOA ギラティナ影　描画
 * @param	tw	TORNWORLD_WORK
 * @param	wk	eoa work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_EoaGirasActDraw( EOA_PTR eoa, void *wk )
{
	EOA_GIRAS_WORK *work = wk;
	
	if( work->vanish == FALSE ){
		VecFx32 pos;
		const VecFx32 *scale_p;
		
		EOA_MatrixGet( eoa, &pos );
		scale_p = &work->head.data.scale;
		
		tw_OBJResDataMdlPolIDSet(
			work->head.tw, TWOBJTYPE_GIRAS, GIRAS_POLID );
		FRO_OBJ_DrawScaleRotate(
			&work->head.objres_p->robj, &pos,
			scale_p, &work->rotate );
	}
}

//--------------------------------------------------------------
///	EOA ギラティナ影　ヘッダー
//--------------------------------------------------------------
static const EOA_H_NPP DATA_EoaGirasHeader =
{
	sizeof(EOA_GIRAS_WORK),
	tw_EoaGirasActInit,
	tw_EoaGirasActDelete,
	tw_EoaGirasActMove,
	tw_EoaGirasActDraw,
};

//==============================================================================
//	PUTOBJ
//==============================================================================
//--------------------------------------------------------------
/**
 * PUTOBJ　初期化
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_PutOBJInit( TORNWORLD_WORK *tw )
{
	TW_PUTOBJ_WORK *pobj = &tw->putobj;
	u32 zone_id = twNowZoneID( tw );
	const TW_ZONE_LINKDATA *link = tw_ZoneLinkDataGet( zone_id );
		
	tw_PutOBJLoadZoneID( tw, zone_id );
	
	if( link->lower_zone_id != ZONE_ID_MAX ){
		tw_PutOBJLoadZoneID( tw, link->lower_zone_id );
	}
}

//--------------------------------------------------------------
/**
 * PUTOBJ　削除
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_PutOBJDelete( TORNWORLD_WORK *tw )
{
	int i = 0;
	TW_PUTOBJ_WORK *pobj = &tw->putobj;
	TW_PUTOBJ_SET *poset = pobj->objset_tbl;
	
	while( i < TW_PUTOBJ_MAX ){
		if( poset->eoa != NULL ){
			tw_PutOBJDeleteSet( poset );
		}
		poset++;
		i++;
	}
}

//--------------------------------------------------------------
/**
 * PUTOBJ　削除
 * @param	poset	TW_PUTOBJ_SET
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_PutOBJDeleteSet( TW_PUTOBJ_SET *poset )
{
	if( poset->eoa != NULL ){
		EOA_Delete( poset->eoa );
	}
	poset->eoa = NULL;
}

//--------------------------------------------------------------
/**
 * PUTOBJ PUTOBJ_DATAから追加
 * @param	tw	TORNWORLD_WORK
 * @param	poset	TW_PUTOBJ_SET
 * @param	data	PUTOBJ_DATA
 * @param	zone_id	zone id
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_PutOBJAdd( TORNWORLD_WORK *tw,
	TW_PUTOBJ_SET *poset, const PUTOBJ_DATA *data, u32 zone_id )
{
	poset->zone_id = zone_id;
	poset->data = *data;
	{
		EOA_PUTOBJ_HEAD add_h;
		const EOA_H_NPP *eoa_h;
		add_h.tw = tw;
		add_h.poset_p = poset;
		eoa_h = DATA_EoaOBJTypeHeaderTbl[data->obj_type];
		poset->eoa = FE_EoaAddNpp(
			tw->fsys->fes, eoa_h, NULL, 0, &add_h, TW_TCBPRI_STONE );
	}
}

//--------------------------------------------------------------
/**
 * PUTOBJ　テーブル追加
 * @param	tw	TORNWORLD_WORK
 * @param	tbl	PUTOBJ_ZONEDATA
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_PutOBJAddTbl(
		TORNWORLD_WORK *tw, const PUTOBJ_ZONEDATA *data )
{
	u16 type,flag;
	const PUTOBJ_DATA *objtbl = data->pobj_tbl;
	TW_PUTOBJ_SET *poset;
	
	while( objtbl->obj_type != TWOBJTYPE_MAX ){
		type = objtbl->check_flag_type;
		flag = objtbl->check_flag;
		
		if( twFlagCheck(tw,type,flag) == TRUE ){
			poset = tw_PutOBJSpaceGet( tw );
			tw_PutOBJAdd( tw, poset, objtbl, data->zone_id );
		}
		
		objtbl++;
	}
}

//--------------------------------------------------------------
/**
 * PUTOBJ　指定ゾーンのPUTOBJをロードする
 * @param	tw	TORNWORLD_WORK
 * @param	tbl	PUTOBJ_ZONEDATA
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_PutOBJLoadZoneID( TORNWORLD_WORK *tw, u32 zone_id )
{
	const PUTOBJ_ZONEDATA *data = DATA_PutOBJZoneDataTbl;
	
	while( data->zone_id != ZONE_ID_MAX ){
		if( data->zone_id == zone_id ){
			tw_PutOBJAddTbl( tw, data );
			#ifdef DEBUG_TW_PRINTF
			OS_Printf( "TW PUTOBJ LOAD ZONE=%xH\n", zone_id );
			#endif
			return;
		}
		data++;
	}
}

//--------------------------------------------------------------
/**
 * PUTOBJ　空き探し
 * @param	tw	TORNWORLD_WORK
 * @retval	TW_PUTOBJ_SET
 */
//--------------------------------------------------------------
static TW_PUTOBJ_SET * tw_PutOBJSpaceGet( TORNWORLD_WORK *tw )
{
	int i = 0;
	TW_PUTOBJ_WORK *pobj = &tw->putobj;
	TW_PUTOBJ_SET *poset = pobj->objset_tbl;
	
	do{
		if( poset->eoa == NULL ){ return( poset ); }
		poset++;
		i++;
	}while( i < TW_PUTOBJ_MAX );
	
	GF_ASSERT( 0 && "tw putobj max\n" );
	return( NULL );
}

//--------------------------------------------------------------
/**
 * PUTOBJ　指定ゾーンIDを持つOBJ削除
 * @param	tw	TORNWORLD_WORK
 * @retval	TW_PUTOBJ_SET
 */
//--------------------------------------------------------------
static void tw_PutOBJDeleteZoneID( TORNWORLD_WORK *tw, u32 zone_id )
{
	int i = 0;
	TW_PUTOBJ_WORK *pobj = &tw->putobj;
	TW_PUTOBJ_SET *poset = pobj->objset_tbl;
	
	while( i < TW_PUTOBJ_MAX ){
		if( poset->zone_id == zone_id && poset->eoa != NULL ){
			tw_PutOBJDeleteSet( poset );
		}
		poset++;
		i++;
	}
}

//--------------------------------------------------------------
/**
 * PUTOBJ OBJリソース検索
 * @param	tw	TORNWORLD_WORK
 * @param	type	TWOBJTYPE
 * @retval	BOOL	TRUE=使用している
 */
//--------------------------------------------------------------
static BOOL tw_PutOBJResTypeSearch( TORNWORLD_WORK *tw, TWOBJTYPE type )
{
	int i = 0;
	TW_PUTOBJ_WORK *pobj = &tw->putobj;
	TW_PUTOBJ_SET *poset = pobj->objset_tbl;
	
	while( i < TW_PUTOBJ_MAX ){
		if( poset->eoa != NULL ){
			if( poset->data.obj_type == type ){
				return( TRUE );
			}
		}
		poset++;
		i++;
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * PUTOBJ モデルリソース検索
 * @param	tw	TORNWORLD_WORK
 * @param	mdl_id	TW_MLD_MAX
 * @retval	BOOL	TRUE=使用している
 */
//--------------------------------------------------------------
static BOOL tw_PutOBJResMdlIDSearch( TORNWORLD_WORK *tw, u32 mdl_id )
{
	int i = 0;
	const TW_OBJRES_ID *res_id;
	TW_PUTOBJ_WORK *pobj = &tw->putobj;
	TW_PUTOBJ_SET *poset = pobj->objset_tbl;
	
	while( i < TW_PUTOBJ_MAX ){
		if( poset->eoa != NULL ){
			res_id = &DATA_OBJResIDTbl[poset->data.obj_type];
			if( res_id->mdl_id == mdl_id ){
				return( TRUE );
			}
		}
		poset++;
		i++;
	}
	
	return( FALSE );
}
  
//--------------------------------------------------------------
/**
 * PUTOBJ アニメリソース検索
 * @param	tw	TORNWORLD_WORK
 * @param	anm_id	TW_ANM_MAX
 * @retval	BOOL	TRUE=使用している
 */
//--------------------------------------------------------------
static BOOL tw_PutOBJResAnmIDSearch( TORNWORLD_WORK *tw, u32 anm_id )
{
	int i = 0;
	const TW_OBJRES_ID *res_id;
	TW_PUTOBJ_WORK *pobj = &tw->putobj;
	TW_PUTOBJ_SET *poset = pobj->objset_tbl;
	
	while( i < TW_PUTOBJ_MAX ){
		if( poset->eoa != NULL ){
			res_id = &DATA_OBJResIDTbl[poset->data.obj_type];
			if( res_id->anm_id == anm_id ){
				return( TRUE );
			}
		}
		poset++;
		i++;
	}
	
	return( FALSE );
}

//==============================================================================
//	PUTOBJ	EOA
//==============================================================================
//--------------------------------------------------------------
///	EOA_PUTOBJ_WORK
//--------------------------------------------------------------
typedef struct
{
	s16 alpha;
	u16 culling;
	VecFx32 pos;
	VecFx32 draw_pos;
	EOA_PUTOBJ_HEAD head;
	TW_OBJRES *objres_p;
}EOA_PUTOBJ_WORK;

//--------------------------------------------------------------
/**
 * EOA PUTOBJ PUTOBJ　初期化 
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work
 * @retval	int TRUE=初期化完了
 */
//--------------------------------------------------------------
static int tw_EoaPutOBJInit( EOA_PTR eoa, void *wk )
{
	int init;
	EOA_PUTOBJ_WORK *work = wk;
	const EOA_PUTOBJ_HEAD *head = EOA_AddPtrGet( eoa );
	const PUTOBJ_DATA *data = &head->poset_p->data;
	
	work->head = *head;
	work->objres_p = tw_OBJResDataOBJResGet( head->tw, data->obj_type, &init);
	
	if( init == FALSE ){
		tw_OBJResDataOBJTypeInit(
			work->head.tw, data->obj_type,
			&work->objres_p->robj, &work->objres_p->ranm );
	}
	
	FieldOBJTool_GridCenterPosGet( data->gx, data->gz, &work->pos );
	work->pos.y = GRID_SIZE_FX32( data->gy ) + GRID_HALF_FX32;
	
	{
		VecFx32 offs;
		const VecFx32 *d_offs = &DATA_OBJTypePosOffsTbl[data->obj_type];
		work->pos.x += d_offs->x;
		work->pos.y += d_offs->y;
		work->pos.z += d_offs->z;
	}
	
	#if 0
	if( data->obj_type == TWOBJTYPE_WATER_FALL ){ //滝専用
		FRO_MDL *rmdl_p;
		rmdl_p = tw_OBJResDataMdlSearch( head->tw, TW_MDL_WATER_FALL );
		NNS_G3dMdlUseMdlAlpha( rmdl_p->pResMdl );
		NNS_G3dMdlSetMdlAlphaAll( rmdl_p->pResMdl, 24 );
	}
	#endif
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * EOA PUTOBJ 削除
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_EoaPutOBJDelete( EOA_PTR eoa, void *wk )
{
	int init;
	EOA_PUTOBJ_WORK *work = wk;
	const PUTOBJ_DATA *data = &work->head.poset_p->data;
	
	if( tw_OBJResDataTypeAnmCheck(data->obj_type) == TRUE ){
		FRO_ANM_DeleteAll( &work->objres_p->ranm );
	}
	
	tw_OBJResDataOBJResCheckRelease( work->head.tw, work->objres_p );
}

//--------------------------------------------------------------
/**
 * EOA PUTOBJ 動作
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work
 * @retval	void
 */
//--------------------------------------------------------------
static void tw_EoaPutOBJMove( EOA_PTR eoa, void *wk )
{
	EOA_PUTOBJ_WORK *work = wk;
	TWOBJTYPE type = work->head.poset_p->data.obj_type;
	
	if( tw_OBJResDataTypeAnmCheck(type) == TRUE ){
		FRO_ANM_Play( &work->objres_p->ranm, FX32_ONE, ANMLOOP_ON );
	}
	
	work->culling =
		tw_OBJResDataTypeCullingCheck( work->head.tw, type, &work->pos );
}

//--------------------------------------------------------------
/**
 * EOA PUTOBJ 描画
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work
 * @retval	void
 */
//--------------------------------------------------------------
static void tw_EoaPutOBJDraw( EOA_PTR eoa, void *wk )
{
	EOA_PUTOBJ_WORK *work = wk;
	
	if( work->culling == TRUE ){
		FRO_OBJ_DrawPos( &work->objres_p->robj, &work->pos );
	}
}

//--------------------------------------------------------------
///	EOA PUTOBJ	
//--------------------------------------------------------------
static const EOA_H_NPP DATA_EoaPutOBJHeader =
{
	sizeof(EOA_PUTOBJ_WORK),
	tw_EoaPutOBJInit,
	tw_EoaPutOBJDelete,
	tw_EoaPutOBJMove,
	tw_EoaPutOBJDraw,
};

//==============================================================================
///	各ゾーン開始時限定初期化処理
//==============================================================================
#if 0
//--------------------------------------------------------------
/**
 * ゾーン開始処理　呼び出し
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_ZoneStartProcCall( TORNWORLD_WORK *tw )
{
}
#endif

//==============================================================================
//	フィールドOBJ配置
//==============================================================================
//--------------------------------------------------------------
/**
 * フィールドOBJ配置　初期化
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_FldOBJInit( TORNWORLD_WORK *tw )
{
	TW_FLDOBJ_WORK *twfldobj = &tw->twfldobj;
	u32 zone_id = twNowZoneID( tw );
	const TW_ZONE_LINKDATA *link = tw_ZoneLinkDataGet( zone_id );
	
	tw_FldOBJLoadZoneID( tw, zone_id );
	
	if( link->lower_zone_id != ZONE_ID_MAX ){
		tw_FldOBJLoadZoneID( tw, link->lower_zone_id );
	}
}

//--------------------------------------------------------------
/**
 * フィールドOBJ配置　削除
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_FldOBJDeleteAll( TORNWORLD_WORK *tw )
{
#if 0	//削除は上位の扱いに依存する
	int i = 0;
	TW_FLDOBJ_WORK *twfldobj = &tw->twfldobj;
	FIELD_OBJ_PTR *fldobj = twfldobj->fldobj_tbl;
	
	for( i = 0; i < TW_FLDOBJ_MAX; i++, fldobj++ ){
		if( (*fldobj) != NULL ){
			FieldOBJ_Delete( *fldobj );
			*fldobj = NULL;
		}
	}
#endif
}

//--------------------------------------------------------------
/**
 * フィールドOBJ配置　指定OBJの削除
 * @param	tw	TORNWORLD_WORK
 * @param	fldobj	削除FIELD_OBJ_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_FldOBJDelete( TORNWORLD_WORK *tw, FIELD_OBJ_PTR fldobj )
{
	int i = 0;
	TW_FLDOBJ_WORK *twfldobj = &tw->twfldobj;
	FIELD_OBJ_PTR *fldobj_tbl = twfldobj->fldobj_tbl;
	
	#ifdef DEBUG_TW_PRINTF
	OS_Printf( "TW FieldOBJ_Delete " );
	DEBUG_twFldOBJStatusPrintf( fldobj );
	#endif
	
	for( i = 0; i < TW_FLDOBJ_MAX; i++, fldobj_tbl++ ){
		if( *fldobj_tbl != NULL && *fldobj_tbl == fldobj ){
			FieldOBJ_Delete( *fldobj_tbl );
			*fldobj_tbl = NULL;
			return;
		}
	}
	GF_ASSERT( 0 );
}

//--------------------------------------------------------------
/**
 * フィールドOBJ配置　空き取得
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static FIELD_OBJ_PTR * tw_FldOBJSpaceGet( TORNWORLD_WORK *tw )
{
	int i;
	TW_FLDOBJ_WORK *twfldobj = &tw->twfldobj;
	
	for( i = 0; i < TW_FLDOBJ_MAX; i++ ){
		if( twfldobj->fldobj_tbl[i] == NULL ){
			return( &twfldobj->fldobj_tbl[i] );
		}
	}
	
	#ifdef DEBUG_TW_PRINTF_FORCE
	OS_Printf( "tw_FldOBJ()空きが無い\n" );
	#endif
	
	#ifdef DEBUG_TW_PRINTF
	DEBUG_twFldOBJTblPrintf( tw );
	#endif
	
	GF_ASSERT( 0 );
	return( NULL );
}

//--------------------------------------------------------------
/**
 * フィールドOBJ 指定されたheadが存在しているかチェック
 * @param	tw	TORNWORLD_WORK
 * @param	head	FIELD_OBJ_H
 * @param	zone_id	zone id
 * @retval	FIELD_OBJ_PTR NULL=存在していない
 */
//--------------------------------------------------------------
static FIELD_OBJ_PTR tw_FldOBJHeadSearch(
		TORNWORLD_WORK *tw, const FIELD_OBJ_H *head, u32 zone_id )
{
	int no = 0;
	FIELD_OBJ_PTR fldobj;
	CONST_FIELD_OBJ_SYS_PTR fos = tw->fsys->fldobjsys;
	
	while( FieldOBJSys_FieldOBJSearch(fos,&fldobj,&no,FLDOBJ_STA_BIT_USE) ){
		if( FieldOBJ_ZoneIDGet(fldobj) == zone_id ){
			if( FieldOBJ_OBJIDGet(fldobj) == head->id ){
				GF_ASSERT( head->obj_code == FieldOBJ_OBJCodeGet(fldobj) );
				return( fldobj );
			}
		}
	}
	
	return( NULL );
}

//--------------------------------------------------------------
/**
 * フィールドOBJ　追加時のフラグチェック
 * @param	tw	TORNWORLD_WORK
 * @param	tw_head	TW_FLDOBJ_HEAD
 * @param	ev	イベントからの追加か TRUE=正
 * @retval	BOOL TRUE=追加可能 FALSE=追加不可
 */
//--------------------------------------------------------------
static BOOL tw_FldOBJAddFlagCheck(
	TORNWORLD_WORK *tw, const TW_FLDOBJ_HEAD *tw_head, u16 ev_flag )
{
	u16 type = tw_head->check_flag_type;
	u16 flag = tw_head->check_flag;
	EVENTWORK *ev = SaveData_GetEventWork( tw->fsys->savedata );
	
	if( type == TWFLAGTYPE_EVENT ){	//イベントスクリプトから
		if( ev_flag == FALSE ){
			return( FALSE );
		}
	}else if( twFlagCheck(tw,type,flag) == FALSE ){
		return( FALSE );
	}
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * フィールドOBJ　追加
 * @param	tw	TORNWORLD_WORK
 * @param	fldobj	FIELD_OBJ_PTR *
 * @param	tw_head	TW_FLDOBJ_HEAD
 * @param	zone_id	ZONE_ID
 * @param	ev_flag	TRUE=イベント(スクリプト)からの追加である
 * @retval	BOOL TRUE=追加された FALSE=追加されない
 */
//--------------------------------------------------------------
static BOOL tw_FldOBJAdd( TORNWORLD_WORK *tw,
	FIELD_OBJ_PTR *fldobj,
	const TW_FLDOBJ_HEAD *tw_head,
	u32 zone_id, u16 ev_flag )
{
	FIELD_OBJ_PTR chkobj;
	
	//既に存在しているかどうかチェック
	chkobj = tw_FldOBJHeadSearch( tw, &tw_head->head, zone_id );
	
	if( chkobj != NULL ){	//追加済み
		if( tw_FldOBJRegWorkCheck(tw,chkobj) == TRUE ){
			return( FALSE );	//登録済み
		}
		
		*fldobj = chkobj;
		
		#ifdef DEBUG_TW_PRINTF
		OS_Printf( "TW 既存のFLDOBJです。OBJ ID=%xH, CODE=%xH,",
			FieldOBJ_OBJIDGet(*fldobj),FieldOBJ_OBJCodeGet(*fldobj) );
		#endif
	}else{					//新規追加
		if( tw_FldOBJAddFlagCheck(tw,tw_head,ev_flag) == FALSE ||
			CheckEventFlag(tw->fsys,tw_head->head.event_flag) != FALSE ){
			return( FALSE );	//追加フラグチェックにヒット
		}
		
		*fldobj = FieldOBJ_AddH(
			tw->fsys->fldobjsys, &tw_head->head, zone_id );
		GF_ASSERT( (*fldobj) != NULL );
	}
	
	FieldOBJ_StatusBitSet_Vanish( *fldobj, TRUE );
	
	if( tw_head->obj_rotate_flag == TWFLDOBJ_ROT_ON ){
		tw_OBJRotate_RegOBJ( tw, *fldobj, tw_head->obj_rotate );
	}
	
	FieldOBJ_MoveBitSet_AttrGetOFF( *fldobj, TRUE );
	FieldOBJ_NotZoneDeleteSet( *fldobj, TRUE );
	FieldOBJ_StatusBitSet_HeightGetOFF( *fldobj, TRUE );
	FieldOBJ_StatusBit_ON( *fldobj, FLDOBJ_STA_BIT_HEIGHT_VANISH_OFF );
	
	#if 0
	//新規である場合、システム上、初回高さを拾ってしまう為、再度初期化
	if( chkobj == NULL ){
		FieldOBJ_GPosDirInit(
			*fldobj,
			tw_head->head.gx,
			SIZE_H_GRID_FX32(tw_head->head.gy),
			tw_head->head.gz,
			tw_head->head.dir );
	}
	#else
	if( chkobj == NULL ){
		BLACT_WORK_PTR blact;
			
		FieldOBJ_GPosDirInit(
			*fldobj,
			tw_head->head.gx,
			SIZE_H_GRID_FX32(tw_head->head.gy),
			tw_head->head.gz,
			tw_head->head.dir );
		
		blact = FieldOBJ_DrawBlAct00_BlActPtrGet( *fldobj );
		
		if( blact != NULL ){	//描画にも反映
			FieldOBJ_BlActPosSet( *fldobj, blact );
		}
	}else{
		
	}
	#endif
	
	FieldOBJ_StatusBitSet_Vanish( *fldobj, FALSE );
	
	return( TRUE );
}

#if 0	//旧バグ有り版
static BOOL tw_FldOBJAdd( TORNWORLD_WORK *tw,
	FIELD_OBJ_PTR *fldobj,
	const TW_FLDOBJ_HEAD *tw_head,
	u32 zone_id, u16 ev_flag )
{
	FIELD_OBJ_PTR chkobj;
	
	//既に存在しているかどうかチェック
	chkobj = tw_FldOBJHeadSearch( tw, &tw_head->head, zone_id );
	
	if( chkobj != NULL ){
		*fldobj = chkobj;
		#ifdef DEBUG_TW_PRINTF
		OS_Printf( "既存のFLDOBJです。OBJ ID=%xH, CODE=%xH,",
			FieldOBJ_OBJIDGet(*fldobj),FieldOBJ_OBJCodeGet(*fldobj) );
		#endif
	}else{ //追加
		if( tw_FldOBJAddFlagCheck(tw,tw_head,ev_flag) == FALSE ||
			CheckEventFlag(tw->fsys,tw_head->head.event_flag) != FALSE ){
			return( FALSE );
		}
		
		*fldobj = FieldOBJ_AddH(
			tw->fsys->fldobjsys, &tw_head->head, zone_id );
		GF_ASSERT( (*fldobj) != NULL );
	}
	
	if( tw_head->obj_rotate_flag == TWFLDOBJ_ROT_ON ){
		tw_OBJRotate_RegOBJ( tw, *fldobj, tw_head->obj_rotate );
	}
	
	FieldOBJ_MoveBitSet_AttrGetOFF( *fldobj, TRUE );
	FieldOBJ_NotZoneDeleteSet( *fldobj, TRUE );
	FieldOBJ_StatusBitSet_HeightGetOFF( *fldobj, TRUE );
	FieldOBJ_StatusBit_ON( *fldobj, FLDOBJ_STA_BIT_HEIGHT_VANISH_OFF );
	
	//新規である場合、システム上、初回高さを拾ってしまう為、再度初期化
	if( chkobj == NULL ){
		FieldOBJ_GPosDirInit(
			*fldobj,
			tw_head->head.gx,
			SIZE_H_GRID_FX32(tw_head->head.gy),
			tw_head->head.gz,
			tw_head->head.dir );
	}
	
	return( TRUE );
}
#endif

//--------------------------------------------------------------
/**
 * フィールドOBJ　テーブル追加
 * @param	tw	TORNWORLD_WORK
 * @param	tw_head	TW_FLDOBJ_HEAD
 * @param	zone_id	ZONE_ID
 * @retval	BOOL TRUE=追加された FALSE=追加されない
 */
//--------------------------------------------------------------
static BOOL tw_FldOBJAddTbl( TORNWORLD_WORK *tw,
	const TW_FLDOBJ_HEAD **tw_head_tbl, u32 zone_id )
{
	FIELD_OBJ_PTR *fldobj;
	while( (*tw_head_tbl) != NULL ){
		fldobj = tw_FldOBJSpaceGet( tw );
		tw_FldOBJAdd( tw, fldobj, *tw_head_tbl, zone_id, FALSE );
		#ifdef DEBUG_TW_PRINTF
		DEBUG_twFldOBJTblCheck( tw );
		#endif
		tw_head_tbl++;
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * フィールドOBJ　指定ゾーンのフィールドOBJを配置する
 * @param	tw	TORNWORLD_WORK
 * @param	zone_id	zone id
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_FldOBJLoadZoneID( TORNWORLD_WORK *tw, u32 zone_id )
{
	const TW_FLDOBJ_ZONEDATA *data = DATA_FldOBJZoneDataTbl;
	
	while( data->zone_id != ZONE_ID_MAX ){
		if( data->zone_id == zone_id ){
			tw_FldOBJAddTbl( tw, data->head_tbl, zone_id );
			return;
		}
		data++;
	}
}

//--------------------------------------------------------------
/**
 * フィールドOBJ配置 指定ゾーンIDを持つOBJを削除
 * @param	tw	TORNWORLD_WORK
 * @param	zone_id	zone id
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_FldOBJDeleteZoneID( TORNWORLD_WORK *tw, u32 zone_id )
{
	int i = 0;
	TW_FLDOBJ_WORK *twfldobj = &tw->twfldobj;
	FIELD_OBJ_PTR *fldobj = twfldobj->fldobj_tbl;
	
	for( i = 0; i < TW_FLDOBJ_MAX; i++, fldobj++ ){
		if( (*fldobj) != NULL ){
			if( FieldOBJ_ZoneIDGet(*fldobj) == zone_id ){
				#ifdef DEBUG_TW_PRINTF
				OS_Printf( "TW FieldOBJ_Delete ZoneID " );
				DEBUG_twFldOBJStatusPrintf( *fldobj );
				#endif
				FieldOBJ_Delete( *fldobj );
				*fldobj = NULL;
			}
		}
	}
}

//--------------------------------------------------------------
/**
 * フィールドOBJ配置　指定IDのOBJを追加　メイン
 * @param	fsys	FIELDSYS_WORK
 * @param	id		OBJ_ID
 * @retval	FIELD_OBJ_PTR
 */
//--------------------------------------------------------------
static FIELD_OBJ_PTR tw_FldOBJEventAdd(
		TORNWORLD_WORK *tw, u32 zone_id, u16 id )
{
	FIELD_OBJ_PTR *fldobj = NULL;
	const TW_FLDOBJ_ZONEDATA *data = DATA_FldOBJZoneDataTbl;
	
	while( data->zone_id != ZONE_ID_MAX ){
		if( data->zone_id == zone_id ){
			const TW_FLDOBJ_HEAD **tw_head_tbl = data->head_tbl;
			while( (*tw_head_tbl) != NULL ){
				if( (*tw_head_tbl)->head.id == id ){
					fldobj = tw_FldOBJSpaceGet( tw );
					tw_FldOBJAdd( tw, fldobj, *tw_head_tbl, zone_id, TRUE );
					#ifdef DEBUG_TW_PRINTF
					DEBUG_twFldOBJTblCheck( tw );
					#endif
					
					if( zone_id == ZONE_ID_D34R0108 ){//第七層岩用Hit無効化
						if( id >= TW_OBJID_0108_HOLEMSG0 &&
							id <= TW_OBJID_0108_HOLEMSG8 ){
							FieldOBJ_StatusBitSet_FellowHit(
									*fldobj, FALSE );
						}
					}
					
					return( *fldobj );
				}
				tw_head_tbl++;
			}
			GF_ASSERT( 0 );	//指定zone内ID一致無し
		}
		data++;
	}
	GF_ASSERT( 0 );
	
	return( NULL );
}

//--------------------------------------------------------------
/**
 * フィールドOBJ配置　指定IDのOBJを追加
 * @param	fsys	FIELDSYS_WORK
 * @param	id		OBJ_ID
 * @retval	nothing
 */
//--------------------------------------------------------------
void FldTornWorld_EvFldOBJAdd( FIELDSYS_WORK *fsys, u16 id )
{
	u32 zone_id = fsys->location->zone_id;
	TORNWORLD_WORK *tw = fsys->fldmap->Work;
	tw_FldOBJEventAdd( tw, zone_id, id );
}	

//--------------------------------------------------------------
/**
 * フィールドOBJ配置　指定IDのOBJを削除
 * @param	fsys	FIELDSYS_WORK
 * @param	id		OBJ_ID
 * @retval	nothing
 */
//--------------------------------------------------------------
void FldTornWorld_EvFldOBJDelete( FIELDSYS_WORK *fsys, u16 id )
{
	int objno = 0;
	FIELD_OBJ_PTR fldobj;
	u32 zone_id = fsys->location->zone_id;
	FIELD_OBJ_SYS_PTR fos = fsys->fldobjsys;
	TORNWORLD_WORK *tw = fsys->fldmap->Work;
	
	while( FieldOBJSys_FieldOBJSearch(
		fos,&fldobj,&objno,FLDOBJ_STA_BIT_USE) == TRUE ){
		if( FieldOBJ_OBJIDGet(fldobj) == id &&
			FieldOBJ_ZoneIDGet(fldobj) == zone_id ){
			tw_FldOBJDelete( tw, fldobj );
			return;
		}
	}
	
	GF_ASSERT( 0 );
}

//--------------------------------------------------------------
/**
 * フィールドOBJ配置　指定OBJが登録済みかどうかチェック
 * @param	tw		TORNOWRLD_WORK
 * @param	fldobj	チェックするFIELD_OBJ_PTR
 * @retval	BOOL	TRUE==登録済み
 */
//--------------------------------------------------------------
static BOOL tw_FldOBJRegWorkCheck(
		TORNWORLD_WORK *tw, FIELD_OBJ_PTR fldobj )
{
	int i = 0;
	TW_FLDOBJ_WORK *twfldobj = &tw->twfldobj;
	FIELD_OBJ_PTR *fldobj_tbl = twfldobj->fldobj_tbl;
	
	GF_ASSERT( fldobj != NULL );
	
	do{
		if( (*fldobj_tbl) == fldobj ){
			return( TRUE );
		}
		
		fldobj_tbl++;
		i++;
	}while( i < TW_FLDOBJ_MAX );
	
	return( FALSE );
}

//==============================================================================
//	怪力岩
//==============================================================================
//--------------------------------------------------------------
///	FALLROCKPOSDATA
//--------------------------------------------------------------
typedef struct
{
	u32 zone_id;
	s16 gx;
	s16 gz;
	u32 match_flag_no;
}FALLROCKPOSDATA;

//--------------------------------------------------------------
///	岩落としポイント
//--------------------------------------------------------------
static const FALLROCKPOSDATA DATA_FallRockPosTbl[] =
{
	{ZONE_ID_D34R0107,76,68,TWEVFLAGNO_MAX},
	{ZONE_ID_D34R0107,97,67,TWEVFLAGNO_MAX},
	{ZONE_ID_D34R0107,86,57,TWEVFLAGNO_MAX},
	
	{ZONE_ID_D34R0108,68,67,TWEVFLAGNO_FALLROCK0_OK},
	{ZONE_ID_D34R0108,75,67,TWEVFLAGNO_ERROR},
	{ZONE_ID_D34R0108,79,67,TWEVFLAGNO_ERROR},
	
	{ZONE_ID_D34R0108,96,68,TWEVFLAGNO_FALLROCK1_OK},
	{ZONE_ID_D34R0108,100,68,TWEVFLAGNO_ERROR},
	{ZONE_ID_D34R0108,105,68,TWEVFLAGNO_ERROR},
	
	{ZONE_ID_D34R0108,85,60,TWEVFLAGNO_FALLROCK2_OK},
	{ZONE_ID_D34R0108,85,56,TWEVFLAGNO_ERROR},
	{ZONE_ID_D34R0108,85,57,TWEVFLAGNO_ERROR},
	
	{ZONE_ID_D34R0108,85,52,TWEVFLAGNO_ERROR},
	{ZONE_ID_D34R0108,85,53,TWEVFLAGNO_ERROR},
	
	{ZONE_ID_MAX,0,0,0},	//終端
};

//--------------------------------------------------------------
/**
 * 指定座標が落下場所かどうか
 * @param	gx		座標GX
 * @param	gy		座標GZ
 * @param	flag	落下場所に対応したTWEVFLAGNO
 * @retval	BOOL	TRUE=落下場所
 */
//--------------------------------------------------------------
static BOOL FallRockPosCheck( u32 zone_id, int gx, int gz, u32 *flag )
{
	const FALLROCKPOSDATA *data = DATA_FallRockPosTbl;
	
	while( data->zone_id != ZONE_ID_MAX ){
		if( data->zone_id == zone_id &&
			data->gx == gx && data->gz == gz ){
			*flag = data->match_flag_no;
			return( TRUE );
		}
		
		data++;
	}
	
	return( FALSE );
}

#if 0
	if( fsys->location->zone_id == ZONE_ID_D34R0107 &&
		FieldOBJ_ZoneIDGet(fldobj) == ZONE_ID_D34R0107 ){
		if( (gx == 76 && gz == 68) ||
			(gx == 97 && gz == 67) ||
			(gx == 86 && gz == 57) ){	//移動先が落ちる場所
			return( TRUE );
		}
	}
	
	return( FALSE );
#endif

//--------------------------------------------------------------
/**
 * 怪力落下岩　対象が下へ落ちる岩かどうか
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	dir	移動方向
 * @retval	BOOL	TRUE=落ちる岩である
 */
//--------------------------------------------------------------
BOOL FldTornWorld_FallRockDirCheck( CONST_FIELD_OBJ_PTR fldobj, int dir )
{
	int gx,gz;
	u32 dmy,zone_id;
	FIELDSYS_WORK *fsys;
	
	fsys = FieldOBJ_FieldSysWorkGet( fldobj );
	zone_id = fsys->location->zone_id;
	
	gx = FieldOBJ_NowPosGX_Get( fldobj );
	gz = FieldOBJ_NowPosGZ_Get( fldobj );
	gx += FieldOBJ_DirAddValueGX( dir );
	gz += FieldOBJ_DirAddValueGZ( dir );
	return( FallRockPosCheck(zone_id,gx,gz,&dmy) );
}

//--------------------------------------------------------------
/**
 * 怪力落下岩　対象が下へ落ちる岩かどうか
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	flag	match flag
 * @retval	BOOL	TRUE=落ちる岩である
 */
//--------------------------------------------------------------
static BOOL tw_FldOBJFallRockCheck( CONST_FIELD_OBJ_PTR fldobj, u32 *flag )
{
	int gx,gz;
	u32 zone_id;
	FIELDSYS_WORK *fsys;
	
	fsys = FieldOBJ_FieldSysWorkGet( fldobj );
	zone_id = fsys->location->zone_id;
	gx = FieldOBJ_NowPosGX_Get( fldobj );
	gz = FieldOBJ_NowPosGZ_Get( fldobj );
	return( FallRockPosCheck(zone_id,gx,gz,flag) );
}

//--------------------------------------------------------------
/**
 * 怪力落下岩　対象が下へ落ちる岩かどうか
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	BOOL	TRUE=落ちる岩である
 */
//--------------------------------------------------------------
BOOL FldTornWorld_FallRockCheck( CONST_FIELD_OBJ_PTR fldobj )
{
	u32 dmy;
	return( tw_FldOBJFallRockCheck(fldobj,&dmy) );
}

//--------------------------------------------------------------
///	EV_FALLROCK_WORK
//--------------------------------------------------------------
struct _EV_FALLROCK_WORK
{
	TORNWORLD_WORK *tw;
	FIELDSYS_WORK *fsys;
	GMEVENT_CONTROL *ev;
	FIELD_OBJ_PTR fldobj;
	
	u16 seq_no;
	u16 fall_type;
	u32 flag_no;
	int frame;
	
	fx32 save_y;
	fx32 shake_y;
};

//--------------------------------------------------------------
///	FALLROCKTYPE
//--------------------------------------------------------------
enum
{
	FALLROCKTYPE_7_8,
	FALLROCKTYPE_OK,
	FALLROCKTYPE_ERROR,
};

//--------------------------------------------------------------
/**
 * 怪力落下岩 落下イベント初期化
 * @param	fsys	FIELDSYS_WORK
 * @param	fldobj	対象となるFIELD_OBJ_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
EV_FALLROCK_WORK * FldTornWorld_FallRockEventInit(
		FIELDSYS_WORK *fsys, GMEVENT_CONTROL *ev, FIELD_OBJ_PTR fldobj )
{
	EV_FALLROCK_WORK *work;
	
	work = sys_AllocMemoryLo( HEAPID_FIELD, sizeof(EV_FALLROCK_WORK) );
	memset( work, 0, sizeof(EV_FALLROCK_WORK) );
	
	work->tw = fsys->fldmap->Work;
	work->fsys = fsys;
	work->ev = ev;
	work->fldobj = fldobj;
	
	tw_FldOBJFallRockCheck( fldobj, &work->flag_no );
	
	switch( work->flag_no ){
	case TWEVFLAGNO_MAX:	//7-8
		work->fall_type = FALLROCKTYPE_7_8;
		break;
	case TWEVFLAGNO_FALLROCK0_OK:
	case TWEVFLAGNO_FALLROCK1_OK:
	case TWEVFLAGNO_FALLROCK2_OK:
		if( tw_SaveDataEventFlagCheck(work->tw,work->flag_no) == FALSE ){
			work->fall_type = FALLROCKTYPE_OK;
			break;
		}
	default:				//ERROR
		work->fall_type = FALLROCKTYPE_ERROR;
		break;
	}
	
	return( work );
}

#define FROCK_FALL_START_FRAME (8)
#define FROCK_FALL_FRAME_OK (4)
#define FROCK_FALL_FRAME_NG (40)
#define FROCK_LAND_GY (115)		//8層地面
#define FROCK_LAND_Y_FX (GRID_SIZE_FX32(FROCK_LAND_GY))

//--------------------------------------------------------------
/**
 * 怪力落下岩 落下イベント動作　七層から八層へ
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	BOOL	TRUE=終了
 */
//--------------------------------------------------------------
static BOOL FallRockEvent_7_8( EV_FALLROCK_WORK *work )
{
	int y;
	VecFx32 pos;
	FIELD_OBJ_PTR fldobj = work->fldobj;
	
	FieldOBJ_VecPosGet( fldobj, &pos );
	
	pos.y -= FX32_ONE*8;
	FieldOBJ_VecPosSet( fldobj, &pos );
	
	if( pos.y > FROCK_LAND_Y_FX ){
		return( FALSE );
	}
	
	Snd_SePlay( SE_YABURETA_ROCK_SHAKE );
	
	pos.y = FROCK_LAND_Y_FX;
	FieldOBJ_VecPosDirInit( fldobj, &pos, FieldOBJ_DirDispGet(fldobj) );
	
	FieldOBJ_ZoneIDSet( fldobj, ZONE_ID_D34R0108 );	//OBJ置き換え
	
	{	//フラグ立て
		u32 flag_on,flag_off;
		
		switch( FieldOBJ_OBJIDGet(fldobj) ){
		case TW_OBJID_0107_ROCK_0:
			flag_on = TWEVFLAGNO_FALLROCK0_7;
			flag_off = TWEVFLAGNO_FALLROCK0_6;
			break;
		case TW_OBJID_0107_ROCK_1:
			flag_on = TWEVFLAGNO_FALLROCK1_7;
			flag_off = TWEVFLAGNO_FALLROCK1_6;
			break;
		case TW_OBJID_0107_ROCK_2:
			flag_on = TWEVFLAGNO_FALLROCK2_7;
			flag_off = TWEVFLAGNO_FALLROCK2_6;
			break;
		}
		
		tw_SaveDataEventFlagON( work->tw, flag_on );
		tw_SaveDataEventFlagOFF( work->tw, flag_off );
	}
	
	return( TRUE );
}

//--------------------------------------------------------------
/*
 * 怪力落下岩 落下イベント動作　正解。
 * 怪力の押し込みで既に1グリッド移動済みである
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	BOOL	TRUE=終了
 */
//--------------------------------------------------------------
static BOOL FallRockEvent_OK( EV_FALLROCK_WORK *work )
{
	switch( work->seq_no ){
	case 0:
		{
			VecFx32 pos;
			FIELD_OBJ_PTR fldobj = work->fldobj;
			
			FieldOBJ_VecPosGet( fldobj, &pos );
			pos.y -= FX32_ONE*2;
			FieldOBJ_VecPosSet( fldobj, &pos );
			
			FieldOBJ_VecPosDirAdd( fldobj,
				FieldOBJ_DirMoveGet(fldobj), NUM_FX32(2) );
		}
		
		work->frame++;
		
		if( work->frame < FROCK_FALL_START_FRAME ){
			return( FALSE );
		}
		
		work->frame = 0;
		work->seq_no++;
		break;
	case 1:
		{
			VecFx32 pos;
			FIELD_OBJ_PTR fldobj = work->fldobj;
			
			FieldOBJ_VecPosGet( fldobj, &pos );
			pos.y -= FX32_ONE*4;
			FieldOBJ_VecPosSet( fldobj, &pos );
		}
		
		work->frame++;
		
		if( work->frame < FROCK_FALL_FRAME_OK ){
			return( FALSE );
		}
		
		work->frame = 0;
		
		{
			u32 id = 0, num = 0;
			FIELD_OBJ_PTR fldobj = work->fldobj;
			
			//落下終了　正解フラグ立て
			tw_SaveDataEventFlagON( work->tw, work->flag_no );
			
			//OBJ書き換え
			switch( FieldOBJ_OBJIDGet(fldobj) ){
			case TW_OBJID_0108_ROCK_0:
				id = TW_OBJID_0108_ROCK_OK_0;
				num = TWEVFLAGNO_FALLROCK0_7;
				break;
			case TW_OBJID_0108_ROCK_1:
				id = TW_OBJID_0108_ROCK_OK_1;
				num = TWEVFLAGNO_FALLROCK1_7;
				break;
			case TW_OBJID_0108_ROCK_2:
				id = TW_OBJID_0108_ROCK_OK_2;
				num = TWEVFLAGNO_FALLROCK2_7;
				break;
			default:
				GF_ASSERT( 0 );
			}
			
			tw_SaveDataEventFlagOFF( work->tw, num );
			FieldOBJ_OBJIDSet( fldobj, id );
			
			{	//座標更新
				int dir = FieldOBJ_DirMoveGet( fldobj );
				int gx = FieldOBJ_NowPosGX_Get( fldobj );
				int gy = FieldOBJ_NowPosGY_Get( fldobj );
				int gz = FieldOBJ_NowPosGZ_Get( fldobj );
				gx += FieldOBJ_DirAddValueGX( dir );
				gy -= G_GRID_H_GRID( 2 );
				gz += FieldOBJ_DirAddValueGZ( dir );
				FieldOBJ_NowPosGX_Set( fldobj, gx );
				FieldOBJ_NowPosGY_Set( fldobj, gy );
				FieldOBJ_NowPosGZ_Set( fldobj, gz );
				FieldOBJ_GPosUpdate( fldobj );
			}
		}
		
		{	//振動ワーク初期化
			VecFx32 offs;
			FieldOBJ_VecDrawOffsOutSideGet( work->fldobj, &offs );
			work->save_y = offs.y;
			work->shake_y = NUM_FX32( 1 );
			
			Snd_SePlay( SE_YABURETA_ROCK_SHAKE );
		}
		
		work->seq_no++;
		break;
	case 2:	//振動
		{
			VecFx32 offs;
			FIELD_OBJ_PTR fldobj = work->fldobj;
			
			FieldOBJ_VecDrawOffsOutSideGet( fldobj, &offs );
			offs.y = work->save_y + work->shake_y;
			FieldOBJ_VecDrawOffsOutSideSet( fldobj, &offs );
			work->shake_y = -work->shake_y;
		}
		
		work->frame++;
		
		if( work->frame == 7 ){
			work->shake_y = 0;
		}
		
		if( work->frame < 32 ){
			return( FALSE );
		}
		
		{
			u32 num = 0,flag = 0;
			FIELD_OBJ_PTR fldobj = work->fldobj;
	
			//正解別フラグ操作とポケモンスクリプト起動
			switch( work->flag_no ){
			case TWEVFLAGNO_FALLROCK0_OK:
				flag = TWEVFLAGNO_0108EMURIT;
				num = SCRID_D34R0108_FALLROCK_EM;
				break;
			case TWEVFLAGNO_FALLROCK1_OK:
				flag = TWEVFLAGNO_0108AGUNOMU;
				num = SCRID_D34R0108_FALLROCK_AGU;
				break;
			case TWEVFLAGNO_FALLROCK2_OK:
				flag = TWEVFLAGNO_0108YUKUSHI;
				num = SCRID_D34R0108_FALLROCK_YUKU;
				break;
			default:
				GF_ASSERT( 0 );
			}
			
			tw_SaveDataEventFlagOFF( work->tw, flag );
			EventCall_Script( work->ev, num, NULL, NULL );
			work->seq_no++;
			break;
		}
	case 3:	//スクリプト終了
		{
			int count = 0;
			count += tw_SaveDataEventFlagCheck(
					work->tw, TWEVFLAGNO_FALLROCK0_OK );
			count += tw_SaveDataEventFlagCheck(
					work->tw, TWEVFLAGNO_FALLROCK1_OK );
			count += tw_SaveDataEventFlagCheck(
					work->tw, TWEVFLAGNO_FALLROCK2_OK );
			
			if( count >= 3 ){
				EVENTWORK *ev = SaveData_GetEventWork( work->fsys->savedata );
				SysFlag_TornWorldRockFinish( ev, SYSFLAG_MODE_SET );
			}
		}
		
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * 怪力落下岩 落下イベント動作　エラー。
 * 怪力の押し込みで既に1グリッド移動済みである
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	BOOL	TRUE=終了
 */
//--------------------------------------------------------------
static BOOL FallRockEvent_Error( EV_FALLROCK_WORK *work )
{
	switch( work->seq_no ){
	case 0:
		{
			VecFx32 pos;
			FIELD_OBJ_PTR fldobj = work->fldobj;
			
			FieldOBJ_VecPosGet( fldobj, &pos );
			pos.y -= FX32_ONE*2;
			FieldOBJ_VecPosSet( fldobj, &pos );
			
			FieldOBJ_VecPosDirAdd( fldobj,
				FieldOBJ_DirMoveGet(fldobj), NUM_FX32(2) );
		}
		
		work->frame++;
		
		if( work->frame < FROCK_FALL_START_FRAME ){
			return( FALSE );
		}
		
		work->frame = 0;
		work->seq_no++;
		break;
	case 1:
		{
			VecFx32 pos;
			FIELD_OBJ_PTR fldobj = work->fldobj;
			
			FieldOBJ_VecPosGet( fldobj, &pos );
			pos.y -= FX32_ONE*4;
			FieldOBJ_VecPosSet( fldobj, &pos );
		}
		
		work->frame++;
		
		if( work->frame < FROCK_FALL_FRAME_OK ){
			return( FALSE );
		}
		
		work->frame = 0;
		work->seq_no++;
		break;
	case 2:
		{
			VecFx32 pos;
			FIELD_OBJ_PTR fldobj = work->fldobj;
			
			FieldOBJ_VecPosGet( fldobj, &pos );
			pos.y -= FX32_ONE*8;
			FieldOBJ_VecPosSet( fldobj, &pos );
		}
		
		work->frame++;
		
		if( work->frame < FROCK_FALL_FRAME_NG ){
			return( FALSE );
		}
		
		{
			u32 on = 0,off = 0,num = 0,flag = 0;
			FIELD_OBJ_PTR fldobj = work->fldobj;
			
			//OBJ消し
			switch( FieldOBJ_OBJIDGet(fldobj) ){
			case TW_OBJID_0108_ROCK_0:
				on = TWEVFLAGNO_FALLROCK0_6;
				off = TWEVFLAGNO_FALLROCK0_7;
				break;
			case TW_OBJID_0108_ROCK_1:
				on = TWEVFLAGNO_FALLROCK1_6;
				off = TWEVFLAGNO_FALLROCK1_7;
				break;
			case TW_OBJID_0108_ROCK_2:
				on = TWEVFLAGNO_FALLROCK2_6;
				off = TWEVFLAGNO_FALLROCK2_7;
				break;
			default:
				GF_ASSERT( 0 );
			}
			
			tw_SaveDataEventFlagON( work->tw, on );
			tw_SaveDataEventFlagOFF( work->tw, off );
			tw_FldOBJDelete( work->tw, work->fldobj );
			
			return( TRUE );
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * 怪力落下岩 落下イベント動作
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	BOOL	TRUE=終了
 */
//--------------------------------------------------------------
BOOL FldTornWorld_FallRockEvent( EV_FALLROCK_WORK *work )
{
	BOOL ret;
	
	switch( work->fall_type ){
	case FALLROCKTYPE_7_8:
		ret = FallRockEvent_7_8( work );
		break;
	case FALLROCKTYPE_OK:
		ret = FallRockEvent_OK( work );
		break;
	case FALLROCKTYPE_ERROR:
		ret = FallRockEvent_Error( work );
		break;
	}
	
	if( ret == TRUE ){
		sys_FreeMemoryEz( work );
	}
	
	return ret;
}

//==============================================================================
//	パレットフェード
//==============================================================================

//--------------------------------------------------------------
/**
 * パレットフェード　初期化
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_PlttInit( TORNWORLD_WORK *tw )
{
	TW_PLTT_WORK *pltt = &tw->pltt;
	u32 zone_id = twNowZoneID( tw );
	
	if( zone_id == ZONE_ID_D34R0110 ){	//八層深部ギラティナ降臨中チェック
		EVENTWORK *ev = SaveData_GetEventWork( tw->fsys->savedata );
		
		//降臨中であれば降臨イベント後の状態と同一に
		if( SysWork_TornWorldEventSeqNoGet(ev) == TWEVSEQ_08D_02 ){
			pltt->evy_layer_off = TRUE;
			pltt->evy = TW_PLTT_EVY_GIRAS_APP;
		}
	}
	
	pltt->old_evy = -1;		//即更新
}

//--------------------------------------------------------------
/**
 * パレットフェード　削除
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_PlttDelete( TORNWORLD_WORK *tw )
{
}

#define RGB16(r,g,b) ((r) | ((g)<<5) | ((b)<<10))
#define TW_BG_BLACK (RGB16(4,4,8))
#define TW_CLOUD_BLACK (RGB16(6,6,8))

#define TW_TOP_LAYER_GY (289)
#define TW_TOP_LAYER_Y (GRID_SIZE_FX32(TW_TOP_LAYER_GY))

#define TW_BOTTOM_LAYER_GY (65)
#define TW_BOTTOM_LAYER_Y (GRID_SIZE_FX32(TW_BOTTOM_LAYER_GY))

#define TW_TOP_BOTTOM_GRID_SIZE (TW_TOP_LAYER_GY-TW_BOTTOM_LAYER_GY)
#define TW_TOP_BOTTOM_SIZE (GRID_SIZE_FX32(TW_TOP_BOTTOM_GRID_SIZE))

#define TW_TOP_BOTTOM_EVY_SIZE (TW_TOP_BOTTOM_SIZE/12)

//--------------------------------------------------------------
/**
 * パレットフェード　常駐処理から呼ばれるメイン処理
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_PlttProc( TORNWORLD_WORK *tw )
{
	TW_PLTT_WORK *pltt = &tw->pltt;
	PLAYER_STATE_PTR jiki = tw->fsys->player;
	const VecFx32 *pos = Player_VecPosPtrGet( jiki );
	
	if( pltt->evy_layer_off == FALSE ){
		pltt->evy = (pos->y-TW_BOTTOM_LAYER_Y) / TW_TOP_BOTTOM_EVY_SIZE;
	}
	
	if( pltt->evy < 0 ){
		pltt->evy = 0;
	}else if( pltt->evy > 12 ){
		pltt->evy = 12;
	}
	
	if( pltt->evy != pltt->old_evy ){
		int i = 0;
		
		do{
			ColorTargetFade(
				pltt->bg3_plttbase[i], TW_BG_BLACK, pltt->evy,
				&pltt->bg3_plttbuf[i] );
			i++;
		}while( i < 16 );
		
		i = 0;
		
		do{
			ColorTargetFade(
				pltt->clact_cloud_plttbase[i], TW_CLOUD_BLACK, pltt->evy,
				&pltt->clact_cloud_plttbuf[i] );
			i++;
		}while( i < (16*TW_CLACT_RES_PLTT_CLOUD_NUM) );
		
		#ifdef DEBUG_TW_PRINTF
		OS_Printf( "EVY UPDATE %d -> %d\n", pltt->old_evy, pltt->evy );
		#endif
		
		pltt->pltt_trans_flag = TRUE;
		pltt->old_evy = pltt->evy;
	}
}

//--------------------------------------------------------------
/**
 * パレットフェード　VIntr処理
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_PlttVIntrProc( TORNWORLD_WORK *tw )
{
	TW_PLTT_WORK *pltt = &tw->pltt;
	
	#ifdef DEBUG_TWORLD_CAPTURE
	if( DebugTWorldCaptureFlag_BGVanish ){
		return;
	}
	#endif
	
	if( pltt->pltt_trans_flag == TRUE ){
		{
			GF_BGL_PaletteSet(
				TW_BGFRM_BG, pltt->bg3_plttbuf,
				32, 32*(TW_BGFRM_BG_PLTT_OFFS) );
		}
		
		{
			u32 vram;
			TW_CLACT_SETDATA *setdata;
			CLACT_U_RES_OBJ_PTR reso_pltt;
			const NNSG2dImagePaletteProxy *proxy_pltt;
			
			setdata = &tw->clact_set;
			reso_pltt = setdata->clact_resobj_pltt[0];
			proxy_pltt = CLACT_U_PlttManagerGetProxy( reso_pltt, NULL );
			vram = NNS_G2dGetImagePaletteLocation(
					proxy_pltt, NNS_G2D_VRAM_TYPE_2DMAIN );
			DC_FlushRange( (void*)pltt->clact_cloud_plttbuf,
							32*TW_CLACT_RES_PLTT_CLOUD_NUM );
			GX_LoadOBJPltt( pltt->clact_cloud_plttbuf,
					vram, 32*TW_CLACT_RES_PLTT_CLOUD_NUM );
		}
		
		pltt->pltt_trans_flag = FALSE;
	}
}

//--------------------------------------------------------------
/**
 * パレットフェードEVY設定を変更
 * @param	tw		TORNWORLD_WORK
 * @param	flag	TRUE=手動 FALSE=階層
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_PlttFadeSystemChg( TORNWORLD_WORK *tw, u32 flag )
{
	TW_PLTT_WORK *pltt = &tw->pltt;
	pltt->evy_layer_off = flag;
}

//--------------------------------------------------------------
/**
 * パレットフェードEVY設定
 * @param	tw		TORNWORLD_WORK
 * @param	evy		0-16
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_PlttFadeEvySet( TORNWORLD_WORK *tw, s16 evy )
{
	TW_PLTT_WORK *pltt = &tw->pltt;
	pltt->evy = evy;
}

//--------------------------------------------------------------
/**
 * 指定色変更
 * @param	base		元パレット
 * @param	target		目標パレット
 * @param	evy			0-16  0=元 8=中間 16=目標
 * @param	dest		結果格納先
 * @retval	nothing
 */
//--------------------------------------------------------------
static void ColorTargetFade( u16 base, u16 target, u16 evy, u16 *dest )
{
	const PLTT *pbase = (PLTT *)&base;
	const PLTT *ptarget = (PLTT *)&target;
	
	if( evy > 16 ){
		evy = 16;
	}
	
	(*dest) = FADE_CHANGE( pbase->red, ptarget->red, evy ) |
		(FADE_CHANGE( pbase->green, ptarget->green, evy ) << 5) |
		(FADE_CHANGE( pbase->blue, ptarget->blue, evy ) << 10);
}

//==============================================================================
//	スクリプト　滝移動動作部分
//
//==============================================================================
//--------------------------------------------------------------
///	WFALLRET
//--------------------------------------------------------------
typedef enum
{
	WFALLRET_NON,			///<特に無し
	WFALLRET_ZONE_CHANGE,	///<ゾーン切り替えを行った
	WFALLRET_LAST,			///<最後の1グリッド移動である。
	WFALLRET_END,			///<終了
}WFALLRET;

//--------------------------------------------------------------
///	WFALLVEC
//--------------------------------------------------------------
typedef enum
{
	WFALLVEC_X = 0,
	WFALLVEC_Y,
	WFALLVEC_Z,
}WFALLVEC;

//--------------------------------------------------------------
//	WFALLDATA
//--------------------------------------------------------------
typedef struct
{
	u32 check_vec;		//終端をチェックするWFALLVEC_X等
	u16 zone_chg_type;	//ゾーン変更型 LPOSMOVE_ZONE_UP等
	s16 move_gx;		//移動量GX
	s16 move_gy;		//移動量GY(地面グリッド単位
	s16 move_gz;		//移動量GZ
	s16 move_gx_size;
	s16 move_gy_size;
	s16 move_gz_size;
	s16 chg_gx;			//ゾーン切り替えタイミングGX
	s16 chg_gy;			//ゾーン切り替えタイミングGY(地面グリッド単位
	s16 chg_gz;			//ゾーン切り替えタイミングGZ
	VecFx32 move_val;	//移動量
}WFALLDATA;

//--------------------------------------------------------------
//	WFALLWORK
//--------------------------------------------------------------
typedef struct
{
	u32 zone_chg_end:1;
	u32 zone_chg_type:1;	//LPOSMOVE_ZONE_UP or DONW
	u32 shake_on:1;
	u32 check_vec:2;		//WFALL_VEC_X等
	u32 dmy:27;
	
	VecFx32 move_val;
	VecFx32 move_count;
	VecFx32 max_count;
	VecFx32 chg_count;
	
	VecFx32 target_pos;
	
	VecFx32 shake_offs;
	fx32 *shake_offs_p;
	fx32 shake_value;
	fx32 shake_max;
}WFALLWORK;

//--------------------------------------------------------------
/**
 * 滝移動　初期化
 * @param	tw		TORNWORLD_WORK
 * @param	work	WFALLWORK
 * @param	data	WFALLDATA
 * @retval	nothing
 */
//--------------------------------------------------------------
static void WaterFallMove_Init( TORNWORLD_WORK *tw,
	WFALLWORK *work, const WFALLDATA *data, const VecFx32 *pos )
{
	work->check_vec = data->check_vec;
	work->zone_chg_type = data->zone_chg_type;
	work->max_count.x = NUM_FX32( data->move_gx_size );
	work->max_count.y = NUM_FX32( data->move_gy_size );
	work->max_count.z = NUM_FX32( data->move_gz_size );
	work->chg_count.x = GRID_SIZE_FX32( data->chg_gx );
	work->chg_count.y = GRID_SIZE_FX32( data->chg_gy );
	work->chg_count.z = GRID_SIZE_FX32( data->chg_gz );
	work->move_val = data->move_val;
	work->target_pos = *pos;
}

//--------------------------------------------------------------
/**
 * 滝移動　揺れ初期化
 * @param	work	WFALLWORK
 * @param	value	ゆれ速度
 * @param	max		揺れ最大
 * @retval	nothing
 */
//--------------------------------------------------------------
static void WaterFallMove_ShakeInit(
	WFALLWORK *work, WFALLVEC vec, fx32 value, fx32 max )
{
	switch( vec ){
	case WFALLVEC_X: work->shake_offs_p = &work->shake_offs.x; break;
	case WFALLVEC_Y: work->shake_offs_p = &work->shake_offs.y; break;
	case WFALLVEC_Z: work->shake_offs_p = &work->shake_offs.z; break;
	}
	
	*work->shake_offs_p = FX32_ONE;
	work->shake_value = value;
	work->shake_max = max;
	work->shake_on = 1;
}

//--------------------------------------------------------------
/**
 * 滝移動　移動
 * @param	tw		TORNWORLD_WORK
 * @param	work	WFALLWORK
 * @retval	BOOL	
 */
//--------------------------------------------------------------
static BOOL WaterFallMove_Move( TORNWORLD_WORK *tw, WFALLWORK *work )

{
	if( work->move_count.x != work->max_count.x ){
		work->move_count.x += work->move_val.x;
		work->target_pos.x += work->move_val.x;
	}
	
	if( work->move_count.y != work->max_count.y ){
		work->move_count.y += work->move_val.y;
		work->target_pos.y += work->move_val.y;
	}
	
	if( work->move_count.z != work->max_count.z ){
		work->move_count.z += work->move_val.z;
		work->target_pos.z += work->move_val.z;
	}
	
	#ifdef DEBUG_TW_PRINTF
	OS_Printf( "move_count.y=%xH(%d,GRID=%d)\n",
			work->move_count.y,
			FX32_NUM(work->move_count.y),
			SIZE_GRID_FX32(work->move_count.y) );
	OS_Printf( "max_count.y=%xH(%d,GRID=%d)\n",
			work->max_count.y,
			FX32_NUM(work->max_count.y),
			SIZE_GRID_FX32(work->max_count.y) );
	#endif
	
	if( work->shake_on ){
		*work->shake_offs_p += work->shake_value;
		
		if( *work->shake_offs_p >= work->shake_max ){
			*work->shake_offs_p = work->shake_max;
			work->shake_value = -work->shake_value;
		}else if( *work->shake_offs_p <= FX32_ONE ){
			*work->shake_offs_p = FX32_ONE;
			work->shake_value = -work->shake_value;
		}
	}
	
	if( work->zone_chg_end == 0 ){
		if(	work->move_count.x == work->chg_count.x &&
			work->move_count.y == work->chg_count.y &&
			work->move_count.z == work->chg_count.z ){
			if( work->zone_chg_type == LPOSMOVE_ZONE_UP ){
				#if 0
				tw_DmyMapZoneChgUp( tw );
				tw_ZoneChangeUp( tw );
				#else
				//tw_DmyMapZoneChgUp( tw, NULL );
				tw_ZoneChangeTaskSet( tw, NULL, TWZONECHGTYPE_UP );
				#endif
			}else{
				#if 0
				tw_DmyMapZoneChgDown( tw );
				tw_ZoneChangeDown( tw );
				#else
				//tw_DmyMapZoneChgDown( tw, NULL );
				tw_ZoneChangeTaskSet( tw, NULL, TWZONECHGTYPE_DOWN );
				#endif
			}
			
			work->zone_chg_end++;
		}
		
		return( WFALLRET_ZONE_CHANGE );
	}
	
	if( work->move_count.x == work->max_count.x &&
		work->move_count.y == work->max_count.y &&
		work->move_count.z == work->max_count.z ){
		return( WFALLRET_END );
	}
	
	{
		fx32 last;
		
		switch( work->check_vec ){
		case WFALLVEC_X:
			last = work->max_count.x - work->move_count.x; break;
		case WFALLVEC_Y:
			last = work->max_count.y - work->move_count.y; break;
		case WFALLVEC_Z:
			last = work->max_count.z - work->move_count.z; break;
		}
		
		if( last < 0 ){
			last = -last;
		}
		
		if( (last-GRID_FX32) == 0 ){ 	//最後の1Grid
			return( WFALLRET_LAST );
		}
	}
	
	return( WFALLRET_NON );
}

//==============================================================================
//	スクリプト　滝下り 5->6
//==============================================================================
//--------------------------------------------------------------
///	スクリプト　滝下り　処理番号
//--------------------------------------------------------------
enum
{
	SEQNO_TAKIKUDARI_INIT = 0,
	SEQNO_TAKIKUDARI_START_TURN,
	SEQNO_TAKIKUDARI_MOVE0,
	SEQNO_TAKIKUDARI_MOVE1,
	SEQNO_TAKIKUDARI_LAST,
	SEQNO_TAKIKUDARI_MAX,
};

//--------------------------------------------------------------
///	SCRDATA_TAKIKUDARI
//--------------------------------------------------------------
typedef struct
{
	WFALLDATA wfall_data;
}SCRDATA_TAKIKUDARI;

//--------------------------------------------------------------
///	SCRWORK_TAKIKUDARI
//--------------------------------------------------------------
typedef struct
{
	s16 frame;
	u16 acmd_count;
	u16 cm_count;
	s16 cm_frame;
	s16 np_frame0;
	s16 np_frame1;
	
	VecFx32 np_rot;
	VecFx32 np_rot_val;
	VecFx32 np_offs_val;
	fx32 np_rev;
	fx32 np_rev_val;
	fx32 shake_offs_org_x;
	fx32 shake_offs_org_x_val;
	
	WFALLWORK wfall_wk;
	#ifdef TWTEST_TAKI_EFF
	EOA_PTR eoa_splash_p;
	#endif
	
	#ifdef DEBUG_TAKI_KUDARI_PAUSE
	int pause_flag;
	#endif
}SCRWORK_TAKIKUDARI;

#if 0	//old
static const TW_CMACTDATA DATA_TakiDownCmAct00 =
{
	{0,0,0,0},
	240,
	240,
	0,
	DIR_UP,
	64,
};
#endif

static const TW_CMACTDATA DATA_TakiDownCmAct00 =
{
	{0,0,0,0},
	245,
	239,
	0,
	DIR_UP,
	72,
};

static const TW_CMACTDATA DATA_TakiDownCmAct01 =
{
	{0,0,0,0},
	0,
	0,
	0,
	DIR_UP,
	32,
};

#ifdef TWTEST_TAKI_EFF
static const VecFx32 DATA_TakiDownSplashOffsTbl[3] =	//cm_count
{
	{NUM_FX32(8),NUM_FX32(-4),NUM_FX32(16)},
	{NUM_FX32(8),NUM_FX32(22),NUM_FX32(0)},
	{NUM_FX32(11),NUM_FX32(9),NUM_FX32(6)},
};

static const VecFx32 DATA_TakiDownSplashOffsSpeedTbl[3] =	//cm_count
{
	{0x800,0x800,0x800},
	{0x800,0x200,0x800},
	{0x800,0x800,0x800},
};
#endif

//--------------------------------------------------------------
///	fx差分増減
//--------------------------------------------------------------
static void takikudari_offscalc( fx32 *offs, const fx32 tget, fx32 val )
{
	GF_ASSERT( val >= 0 );
	
	if( (*offs) > tget ){
		(*offs) -= val;
		if( (*offs) < tget ){
			(*offs) = tget;
		}
	}else if( (*offs) < tget ){
		(*offs) += val;
		if( (*offs) > tget ){
			(*offs) = tget;
		}
	}
}

//--------------------------------------------------------------
/**
 * 滝下り　アクター処理
 * @param	tw		TORNWORLD_WORK
 * @param	work	SCRWORK_TAKIKUDARI
 * @retval	nothing
 */
//--------------------------------------------------------------
static void TakiKudari_ActorProc(
	TORNWORLD_WORK *tw, SCRWORK_TAKIKUDARI *work )
{
	FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( tw->fsys->player );
	int grid = SIZE_GRID_FX32( work->wfall_wk.move_count.y );
	
	if( work->cm_count == 0 && grid == -20 ){
		tw_CameraActionSet( tw, &DATA_TakiDownCmAct00 );
//		tw_OBJRotateAnmSet( tw, fldobj, -28, 72 );	//滝の角度に併せる
		tw_OBJRotateAnmSet( tw, fldobj, -32, 72 );	//滝の角度に併せる
		work->cm_count++;
	}else if( work->cm_count == 1 && grid == -36 ){
		FieldOBJ_DirDispCheckSet( fldobj, DIR_LEFT );
		tw_CameraActionSet( tw, &DATA_TakiDownCmAct01 );
//		tw_OBJRotateAnmSet( tw, fldobj, 28, 31 );
		tw_OBJRotateAnmSet( tw, fldobj, 32, 31 );
		work->cm_count++;
	}
	
	//カメラ復帰と併せて向きを戻す
	if( work->cm_count == 1 && work->cm_frame >= 0 ){
		work->cm_frame++;
		if( work->cm_frame >= 32 ){
			FieldOBJ_DirDispCheckSet( fldobj, DIR_UP );
			work->cm_frame = -1;
		}
	}
	
	#ifdef TWTEST_TAKI_EFF
	if( work->eoa_splash_p != NULL && work->cm_count < 3 ){
		const VecFx32 *tget = &DATA_TakiDownSplashOffsTbl[work->cm_count];
		const VecFx32 *val = &DATA_TakiDownSplashOffsSpeedTbl[work->cm_count];
		VecFx32 *offs = EoaSplash_OffsetPosPtrGet( work->eoa_splash_p );
		takikudari_offscalc( &offs->x, tget->x, val->x );
		takikudari_offscalc( &offs->y, tget->y, val->y );
		takikudari_offscalc( &offs->z, tget->z, val->z );
	}
	#endif
}

#if 0	//old
static void TakiKudari_ActorProc(
	TORNWORLD_WORK *tw, SCRWORK_TAKIKUDARI *work )
{
	FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( tw->fsys->player );
	int grid = SIZE_GRID_FX32( work->wfall_wk.move_count.y );
	
	if( work->cm_count == 0 && grid == -20 ){
		tw_CameraActionSet( tw, &DATA_TakiDownCmAct00 );
		work->cm_count++;
	}else if( work->cm_count == 1 && grid == -36 ){
		FieldOBJ_DirDispCheckSet( fldobj, DIR_LEFT );
		tw_CameraActionSet( tw, &DATA_TakiDownCmAct01 );
		work->cm_count++;
	}
	
	if( work->cm_count == 1 && work->cm_frame >= 0 ){
		work->cm_frame++;
		if( work->cm_frame >= 32 ){
			FieldOBJ_DirDispCheckSet( fldobj, DIR_UP );
			work->cm_frame = -1;
		}
	}
	
	#ifdef TWTEST_TAKI_EFF
	if( work->eoa_splash_p != NULL && work->cm_count < 3 ){
		const VecFx32 *tget = &DATA_TakiDownSplashOffsTbl[work->cm_count];
		const VecFx32 *val = &DATA_TakiDownSplashOffsSpeedTbl[work->cm_count];
		VecFx32 *offs = EoaSplash_OffsetPosPtrGet( work->eoa_splash_p );
		takikudari_offscalc( &offs->x, tget->x, val->x );
		takikudari_offscalc( &offs->y, tget->y, val->y );
		takikudari_offscalc( &offs->z, tget->z, val->z );
	}
	#endif
}
#endif

//--------------------------------------------------------------
/**
 * スクリプト　滝下り　初期化
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrTakiKudari_Init(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	VecFx32 pos;
	WFALLWORK *wfall;
	FIELD_OBJ_PTR fldobj;
	PLAYER_STATE_PTR jiki;
	SCRWORK_TAKIKUDARI *work;
	const SCRDATA_TAKIKUDARI *dat;
	
	dat = data;
	work = tw_ScrProcWorkInit( tw, sizeof(SCRWORK_TAKIKUDARI) );
	wfall = &work->wfall_wk;
	
	jiki = tw->fsys->player;
	fldobj = Player_FieldOBJGet( jiki );
	FieldOBJ_VecPosGet( fldobj, &pos );
	FieldOBJ_DirDispCheckSet( fldobj, DIR_LEFT );
	
	WaterFallMove_Init( tw, wfall, &dat->wfall_data, &pos );
	wfall->move_val.x >>= 1;
	wfall->move_val.y >>= 1;
	wfall->move_val.z >>= 1;
	
	work->frame = 32;	//1grid(16) 0.5
	tw_OBJRotateAnmSet( tw, fldobj, 90, work->frame );
	
	{
		VecFx32 clear = {0,0,0};
		FieldOBJ_VecDrawOffsOutSideSet( fldobj, &clear );
	}
	
	{ 	//この時点での波ポケ状態 自転 X180,Y90,Z0, REV90
		VecFx32 *np_offs_p;
		EOA_PTR eoa = Player_JointEoaGet( jiki );
		ROTATE *rot = FE_FldOBJNamiPokeR_RotatePtrGet( eoa );
		
		FE_FldOBJNamiPokeR_RotateDirInit( eoa, DIR_LEFT, HEROTWTYPE_ROOF );
		
		FE_FldOBJNamiPokeR_StatusBitON(
			eoa,
			NPOKER_BIT_ROTATE_OFF		|
			NPOKER_BIT_ANGLE_REV_OFF	|
			NPOKER_BIT_POKE_OFFS_OFF 	|
			NPOKER_BIT_SHAKE_OUTSIDE );
		
		work->np_rot.x = NUM_FX32( rot->x );
		work->np_rot.y = NUM_FX32( rot->y );
		work->np_rot.z = NUM_FX32( rot->z );
		work->np_rot_val.x = NUM_FX32( -90 ) / 32;
		work->np_rot_val.y = NUM_FX32( 180 ) / 32;
		work->np_rot_val.z = 0;
		work->np_rev = NUM_FX32( 90 );
		work->np_rev_val = NUM_FX32( -70 ) / 32;
		
		np_offs_p = FE_FldOBJNamiPokeR_PokeOffsetPtrGet( eoa );
		np_offs_p->x = 0;
		np_offs_p->y = 0;
		np_offs_p->z = NUM_FX32( 6 );
		work->np_offs_val.x = 0;
		work->np_offs_val.y = 0;
		work->np_offs_val.z = NUM_FX32( 4 ) / 32;
	}
	
	Snd_SePlay( SE_YABURETA_TAKINOBORI );
	
	*seq_no = SEQNO_TAKIKUDARI_START_TURN;
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　滝下り　初回動作　回転に併せてゆっくり移動
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrTakiKudari_StartTurn(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( tw->fsys->player );
	SCRWORK_TAKIKUDARI *work = tw_ScrProcWorkGet( tw );
	WFALLWORK *wfall = &work->wfall_wk;
	
	WaterFallMove_Move( tw, wfall );
	FieldOBJ_VecPosSet( fldobj, &wfall->target_pos );
	TakiKudari_ActorProc( tw, work );
	
	{
		EOA_PTR eoa = Player_JointEoaGet( tw->fsys->player );
		
		{
			ROTATE *rot = FE_FldOBJNamiPokeR_RotatePtrGet( eoa );
			twAngleAdd360Fx( &work->np_rot.x, work->np_rot_val.x );
			twAngleAdd360Fx( &work->np_rot.y, work->np_rot_val.y );
			twAngleAdd360Fx( &work->np_rot.z, work->np_rot_val.z );
			rot->x = FX32_NUM( work->np_rot.x );
			rot->y = FX32_NUM( work->np_rot.y );
			rot->z = FX32_NUM( work->np_rot.z );
		}
		
		{
			twAngleAdd360Fx( &work->np_rev, work->np_rev_val );
			FE_FldOBJNamiPokeR_AngleRevSet( eoa, FX32_NUM(work->np_rev) );
		}
		
		{
			VecFx32 *offs = FE_FldOBJNamiPokeR_PokeOffsetPtrGet( eoa );
			offs->x += work->np_offs_val.x;
			offs->y += work->np_offs_val.y;
			offs->z += work->np_offs_val.z;
		}
	}
	
	work->frame--;
	
	if( work->frame == 0 ){
		wfall->move_val.x <<= 1;
		wfall->move_val.y <<= 1;
		wfall->move_val.z <<= 1;
		WaterFallMove_ShakeInit( wfall, WFALLVEC_X, 0x400, NUM_FX32(4) );
		
		{
			EOA_PTR eoa = Player_JointEoaGet( tw->fsys->player );
			VecFx32 *np_offs_p;
			np_offs_p = FE_FldOBJNamiPokeR_PokeOffsetPtrGet( eoa );
			np_offs_p->x = 0;
			np_offs_p->y = 0;
			np_offs_p->z = NUM_FX32( 10 );
			work->np_frame0 = 70;	//カメラ0のフレームよりやや早め
			work->np_frame1 = -1;
			work->np_offs_val.x = 0;
			work->np_offs_val.y = 0;
			work->np_offs_val.z = NUM_FX32( -16 ) / work->np_frame0;
			work->shake_offs_org_x_val = NUM_FX32( -4 ) / work->np_frame0;
		}
		
		#ifdef TWTEST_TAKI_EFF
		{	//水飛沫エフェクト追加
			VecFx32 *offs;
			work->eoa_splash_p = EoaSplash_Set(
					tw, Player_VecPosPtrGet(tw->fsys->player) );
			offs = EoaSplash_OffsetPosPtrGet( work->eoa_splash_p );
			*offs = DATA_TakiDownSplashOffsTbl[0];
		}
		#endif
		
		*seq_no = SEQNO_TAKIKUDARI_MOVE0;
		return( RET_NON );
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　滝下り　移動0
 * @param	tw	TORNWORLD_WORK
 * @retval	BOOL	TRUE=再帰
 */
//--------------------------------------------------------------
static PROCRET ScrTakiKudari_Move0(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	WFALLRET ret;
	EOA_PTR eoa = Player_JointEoaGet( tw->fsys->player );
	FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( tw->fsys->player );
	SCRWORK_TAKIKUDARI *work = tw_ScrProcWorkGet( tw );
	WFALLWORK *wfall = &work->wfall_wk;
	
	#ifdef DEBUG_TAKI_KUDARI_PAUSE
	if( (sys.trg & PAD_BUTTON_A) ){
		work->pause_flag = (work->pause_flag + 1) & 0x01;
	}
	
	if( work->pause_flag ){
		return( RET_NON );
	}
	#endif
	
	ret = WaterFallMove_Move( tw, wfall );
	FieldOBJ_VecPosSet( fldobj, &wfall->target_pos );
	
	{
		VecFx32 offs = wfall->shake_offs;
		offs.x += work->shake_offs_org_x;
		FieldOBJ_VecDrawOffsSet( fldobj, &offs );
	}
	
	TakiKudari_ActorProc( tw, work );
	
	{
		VecFx32 *offs = FE_FldOBJNamiPokeR_PokeOffsetPtrGet( eoa );
		VecFx32 *shake = FE_FldOBJNamiPokeR_ShakeOffsetPtrGet( eoa );
		*shake = wfall->shake_offs;
		
		if( work->cm_count == 1 ){	//カメラアクション 0発生
			if( work->np_frame0 > 0 ){
				work->np_frame0--;
				offs->x += work->np_offs_val.x;
				offs->y += work->np_offs_val.y;
				offs->z += work->np_offs_val.z;
				work->shake_offs_org_x += work->shake_offs_org_x_val;
			}
		}else if( work->cm_count == 2 ){	//カメラアクション 1発生
			if( work->np_frame1 < 0 ){		//- == 初期化
				work->np_frame1 = 30;		//カメラ2のフレームよりやや早め
				work->np_offs_val.x = 0;
				work->np_offs_val.y = 0;
				work->np_offs_val.z = NUM_FX32( 6 ) / work->np_frame1;
				work->shake_offs_org_x_val = NUM_FX32( 4 ) /work->np_frame1;
			}
			
			if( work->np_frame1 > 0 ){
				work->np_frame1--;
				offs->x += work->np_offs_val.x;
				offs->y += work->np_offs_val.y;
				offs->z += work->np_offs_val.z;
				work->shake_offs_org_x += work->shake_offs_org_x_val;
			}
		}
	}
	
	if( ret == WFALLRET_LAST ){				//最後の1グリッド
		work->frame = 16;	//1grid(16) 1.0
		tw_OBJRotateAnmSet( tw, fldobj, 90, work->frame );
		
		{
			work->np_rot_val.x = NUM_FX32(-90) / 16;
			work->np_rot_val.y = 0;
			work->np_rot_val.z = 0;
			
			work->np_offs_val.x = 0;
			work->np_offs_val.y = 0;
			work->np_offs_val.z = NUM_FX32(-7) / 16; //0->-7
			
			work->np_rev_val = NUM_FX32(-110) / 16;	//20->270
			FE_FldOBJNamiPokeR_StatusBitOFF( eoa, NPOKER_BIT_SHAKE_OUTSIDE );
		}
		
		*seq_no = SEQNO_TAKIKUDARI_MOVE1;
	}
	
	GF_ASSERT( ret != WFALLRET_END );
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　滝下り　移動1
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrTakiKudari_Move1(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	WFALLRET ret;
	EOA_PTR eoa = Player_JointEoaGet( tw->fsys->player );
	FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( tw->fsys->player );
	SCRWORK_TAKIKUDARI *work = tw_ScrProcWorkGet( tw );
	WFALLWORK *wfall = &work->wfall_wk;
	
	ret = WaterFallMove_Move( tw, wfall );
	FieldOBJ_VecPosSet( fldobj, &wfall->target_pos );
	FieldOBJ_VecDrawOffsSet( fldobj, &wfall->shake_offs );
	TakiKudari_ActorProc( tw, work );
	
	{
		VecFx32 *offs;
		ROTATE *rot = FE_FldOBJNamiPokeR_RotatePtrGet( eoa );
		twAngleAdd360Fx( &work->np_rot.x, work->np_rot_val.x );
		twAngleAdd360Fx( &work->np_rot.y, work->np_rot_val.y );
		twAngleAdd360Fx( &work->np_rot.z, work->np_rot_val.z );
		rot->x = FX32_NUM( work->np_rot.x );
		rot->y = FX32_NUM( work->np_rot.y );
		rot->z = FX32_NUM( work->np_rot.z );
		
		twAngleAdd360Fx( &work->np_rev, work->np_rev_val );
		FE_FldOBJNamiPokeR_AngleRevSet( eoa, FX32_NUM(work->np_rev) );
		
		{
			VecFx32 *offs = FE_FldOBJNamiPokeR_PokeOffsetPtrGet( eoa );
			offs->x += work->np_offs_val.x;
			offs->y += work->np_offs_val.y;
			offs->z += work->np_offs_val.z;
		}
	}
	
	if( ret == WFALLRET_END ){
		int gx,gy,gz;
		const SCRDATA_TAKIKUDARI *dat = data;
		const WFALLDATA *wfd = &dat->wfall_data;
		VecFx32 clear = {0,0,0};
		
		gx = FieldOBJ_NowPosGX_Get( fldobj ) + wfd->move_gx;
		gy = FieldOBJ_NowPosGY_Get( fldobj ) + G_GRID_H_GRID( wfd->move_gy );
		gz = FieldOBJ_NowPosGZ_Get( fldobj ) + wfd->move_gz;
		FieldOBJ_NowPosGX_Set( fldobj, gx );
		FieldOBJ_NowPosGY_Set( fldobj, gy );
		FieldOBJ_NowPosGZ_Set( fldobj, gz );
		FieldOBJ_GPosUpdate( fldobj );
		FieldOBJ_DirDispCheckSet( fldobj, DIR_LEFT );
		FieldOBJ_DirMoveSet( fldobj, DIR_LEFT );
		
		FieldOBJ_VecDrawOffsSet( fldobj, &clear );
		FieldOBJ_VecDrawOffsOutSideSet( fldobj, &clear );
		
		tw_MapDataGndDataPosDataSet(
			tw, gx, H_GRID_G_GRID(gy), gz, TW_GNDTYPE_MAX );
		Player_MoveBitSet_TWorld( tw->fsys->player, HEROTWTYPE_ON );
		FieldOBJ_StatusBitSet_HeightGetOFF( fldobj, FALSE );
		
		{	//波乗りパラメタ初期化
			{
				VecFx32 * np_offs_p =
					FE_FldOBJNamiPokeR_PokeOffsetPtrGet( eoa );
				*np_offs_p = clear;
				
				np_offs_p =
					FE_FldOBJNamiPokeR_ShakeOffsetPtrGet( eoa );
				*np_offs_p = clear;
			}
			
			FE_FldOBJNamiPokeR_AngleRevSet( eoa, 270 );
			FE_FldOBJNamiPokeR_RotateDirInit( eoa, DIR_LEFT, HEROTWTYPE_ON );
			
			FE_FldOBJNamiPokeR_StatusBitOFF(
				eoa,
				NPOKER_BIT_ROTATE_OFF		|
				NPOKER_BIT_ANGLE_REV_OFF	|
				NPOKER_BIT_POKE_OFFS_OFF 	|
				NPOKER_BIT_SHAKE_OUTSIDE );
		}
		
		Snd_SeStopBySeqNo( SE_YABURETA_TAKINOBORI, 0 );
		
		*seq_no = SEQNO_TAKIKUDARI_LAST;
		
		#ifdef TWTEST_TAKIEFF
		if( work->eoa_splash_p != NULL ){	//水飛沫削除
			EOA_Delete( work->eoa_splash_p );
			work->eoa_splash_p = NULL;
		}
		#endif
		
		{	//滝下り限定　強制リフト出現処理
			tw_SaveDataZLinkMoveFlagON( tw, ZLINKMFLAG_0506B );
			tw_LinkPosAddZoneIDNo(
				tw, ZONE_ID_D34R0107, LPOSID0107_ZUP_06_05 );
		}
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　滝下り　最後の惰性移動
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrTakiKudari_Last(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	u32 tbl[2] = { AC_WALK_L_16F, AC_WALK_L_32F };
	SCRWORK_TAKIKUDARI *work = tw_ScrProcWorkGet( tw );
	FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( tw->fsys->player );
	
	if( FieldOBJ_AcmdSetCheck(fldobj) == FALSE ){
		return( RET_NON );
	}
	
	if( work->acmd_count >= 2 ){
		return( RET_END );
	}
	
	FieldOBJ_AcmdSet( fldobj, tbl[work->acmd_count] );
	work->acmd_count++;
	return( RET_NON );
}

//--------------------------------------------------------------
///	スクリプトテーブル　滝下り
//--------------------------------------------------------------
static const TW_SCR_PROC DATA_ScrTbl_TakiKudari[SEQNO_TAKIKUDARI_MAX] =
{
	ScrTakiKudari_Init,
	ScrTakiKudari_StartTurn,
	ScrTakiKudari_Move0,
	ScrTakiKudari_Move1,
	ScrTakiKudari_Last,
};

//==============================================================================
//	スクリプト　滝登り 6->5
//==============================================================================
//--------------------------------------------------------------
///	スクリプト　滝登り　処理番号
//--------------------------------------------------------------
enum
{
	SEQNO_TAKINOBORI_INIT = 0,
	SEQNO_TAKINOBORI_START_TURN,
	SEQNO_TAKINOBORI_MOVE0,
	SEQNO_TAKINOBORI_MOVE1,
	SEQNO_TAKINOBORI_LAST,
	SEQNO_TAKINOBORI_MAX,
};

//--------------------------------------------------------------
///	SCRDATA_TAKINOBORI
//--------------------------------------------------------------
typedef struct
{
	WFALLDATA wfall_data;
}SCRDATA_TAKINOBORI;

//--------------------------------------------------------------
///	SCRWORK_TAKINOBORI
//--------------------------------------------------------------
typedef struct
{
	s16 frame;
	u16 acmd_count;
	u16 cm_count;
	s16 cm_frame;
	
	VecFx32 np_rot;
	VecFx32 np_rot_val;
	VecFx32 np_offs_val;
	fx32 np_rev;
	fx32 np_rev_val;
	
	WFALLWORK wfall_wk;
	
	#ifdef TWTEST_TAKIEFF
	EOA_PTR eoa_splash_p;
	#endif
	
	#ifdef DEBUG_TAKI_KUDARI_PAUSE
	int pause_flag;
	#endif
}SCRWORK_TAKINOBORI;

static const TW_CMACTDATA DATA_TakiUpCmAct00 =
{
	{0,0,0,0},
	55,
	240,
	0,
	DIR_UP,
	16,
};

#ifdef TWTEST_TAKIEFF
static const VecFx32 DATA_TakiUpSplashOffsTbl[2] =	//cm count
{
	{NUM_FX32(8),NUM_FX32(14),NUM_FX32(2)},
	{NUM_FX32(8),NUM_FX32(-1),NUM_FX32(17)},
};

static const VecFx32 DATA_TakiUpSplashOffsSpeedTbl[2] =	//cm count
{
	{0x800,0x800,0x800},
	{0x800,0x2000,0x2000},
};
#endif

//--------------------------------------------------------------
/**
 * 滝登り　アクター処理
 * @param	tw		TORNWORLD_WORK
 * @param	work	SCRWORK_TAKINOBORI
 * @retval	nothing
 */
//--------------------------------------------------------------
static void TakiNobori_ActorProc(
	TORNWORLD_WORK *tw, SCRWORK_TAKINOBORI *work )
{
	FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( tw->fsys->player );
	int grid = SIZE_GRID_FX32( work->wfall_wk.move_count.y );
	
	if( work->cm_count == 0 && grid == 20 ){
		tw_CameraActionSet( tw, &DATA_TakiUpCmAct00 );
		work->cm_count++;
	}
	
	#ifdef TWTEST_TAKIEFF
	if( work->eoa_splash_p != NULL && work->cm_count < 2 ){
		const VecFx32 *tget = &DATA_TakiUpSplashOffsTbl[work->cm_count];
		const VecFx32 *val = &DATA_TakiUpSplashOffsSpeedTbl[work->cm_count];
		VecFx32 *offs = EoaSplash_OffsetPosPtrGet( work->eoa_splash_p );
		takikudari_offscalc( &offs->x, tget->x, val->x );
		takikudari_offscalc( &offs->y, tget->y, val->y );
		takikudari_offscalc( &offs->z, tget->z, val->z );
	}
	#endif
}

//--------------------------------------------------------------
/**
 * スクリプト　滝登り　初期化
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrTakiNobori_Init(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	VecFx32 pos;
	WFALLWORK *wfall;
	FIELD_OBJ_PTR fldobj;
	PLAYER_STATE_PTR jiki;
	SCRWORK_TAKINOBORI *work;
	const SCRDATA_TAKINOBORI *dat;
	
	dat = data;
	work = tw_ScrProcWorkInit( tw, sizeof(SCRWORK_TAKINOBORI) );
	wfall = &work->wfall_wk;
	
	jiki = tw->fsys->player;
	fldobj = Player_FieldOBJGet( jiki );
	FieldOBJ_VecPosGet( fldobj, &pos );
	
	FieldOBJ_DirDispCheckSet( fldobj, DIR_RIGHT );
	
	WaterFallMove_Init( tw, wfall, &dat->wfall_data, &pos );
	wfall->move_val.x >>= 1;
	wfall->move_val.y >>= 1;
	wfall->move_val.z >>= 1;
	
	work->frame = 4;	//1grid(16) 8->4
	tw_OBJRotateAnmSet( tw, fldobj, -90, work->frame );
	
	{
		VecFx32 clear = {0,0,0};
		FieldOBJ_VecDrawOffsOutSideSet( fldobj, &clear );
	}
	
	{ 	//この時点での波ポケ状態 自転 X0,Y90,Z0, REV270
		VecFx32 *np_offs_p;
		EOA_PTR eoa = Player_JointEoaGet( jiki );
		ROTATE *rot = FE_FldOBJNamiPokeR_RotatePtrGet( eoa );
		
		FE_FldOBJNamiPokeR_RotateDirInit( eoa, DIR_RIGHT, HEROTWTYPE_ON );
		
		FE_FldOBJNamiPokeR_StatusBitON(
			eoa,
			NPOKER_BIT_ROTATE_OFF		|
			NPOKER_BIT_ANGLE_REV_OFF	|
			NPOKER_BIT_POKE_OFFS_OFF 	|
			NPOKER_BIT_SHAKE_OUTSIDE );
		
		work->np_rot.x = NUM_FX32( rot->x );
		work->np_rot.y = NUM_FX32( rot->y );
		work->np_rot.z = NUM_FX32( rot->z );
		work->np_rot_val.x = NUM_FX32( -90 ) / 4;
		work->np_rot_val.y = NUM_FX32( 180 ) / 4;
		work->np_rot_val.z = 0;
		work->np_rev = NUM_FX32( 270 );
		work->np_rev_val = NUM_FX32( 110 ) / 4;
		
		np_offs_p = FE_FldOBJNamiPokeR_PokeOffsetPtrGet( eoa );
		np_offs_p->x = 0;
		np_offs_p->y = 0;
		np_offs_p->z = NUM_FX32( -7 );
		work->np_offs_val.x = 0;
		work->np_offs_val.y = 0;
		work->np_offs_val.z = NUM_FX32( 0 ) / 4;
	}
	
	Snd_SePlay( SE_YABURETA_TAKINOBORI );
	
	*seq_no = SEQNO_TAKINOBORI_START_TURN;
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　滝登り　初回動作　回転に併せてゆっくり移動
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrTakiNobori_StartTurn(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( tw->fsys->player );
	SCRWORK_TAKINOBORI *work = tw_ScrProcWorkGet( tw );
	WFALLWORK *wfall = &work->wfall_wk;
	
	WaterFallMove_Move( tw, wfall );
	FieldOBJ_VecPosSet( fldobj, &wfall->target_pos );
	TakiNobori_ActorProc( tw, work );
	
	{
		EOA_PTR eoa = Player_JointEoaGet( tw->fsys->player );
		
		{
			ROTATE *rot = FE_FldOBJNamiPokeR_RotatePtrGet( eoa );
			twAngleAdd360Fx( &work->np_rot.x, work->np_rot_val.x );
			twAngleAdd360Fx( &work->np_rot.y, work->np_rot_val.y );
			twAngleAdd360Fx( &work->np_rot.z, work->np_rot_val.z );
			rot->x = FX32_NUM( work->np_rot.x );
			rot->y = FX32_NUM( work->np_rot.y );
			rot->z = FX32_NUM( work->np_rot.z );
		}
		
		{
			twAngleAdd360Fx( &work->np_rev, work->np_rev_val );
			FE_FldOBJNamiPokeR_AngleRevSet( eoa, FX32_NUM(work->np_rev) );
		}
		
		{
			VecFx32 *offs = FE_FldOBJNamiPokeR_PokeOffsetPtrGet( eoa );
			offs->x += work->np_offs_val.x;
			offs->y += work->np_offs_val.y;
			offs->z += work->np_offs_val.z;
		}
	}
	
	work->frame--;
	
	if( work->frame == 0 ){
		wfall->move_val.x <<= 1;
		wfall->move_val.y <<= 1;
		wfall->move_val.z <<= 1;
		WaterFallMove_ShakeInit( wfall, WFALLVEC_X, 0x200, NUM_FX32(4) );
		
		{
			EOA_PTR eoa = Player_JointEoaGet( tw->fsys->player );
			VecFx32 *np_offs_p;
			np_offs_p = FE_FldOBJNamiPokeR_PokeOffsetPtrGet( eoa );
			np_offs_p->x = 0;
			np_offs_p->y = 0;
			np_offs_p->z = NUM_FX32( -7 );
			work->np_offs_val.x = 0;
			work->np_offs_val.y = 0;
			work->np_offs_val.z = NUM_FX32( 17 ) / 16; //カメラ0と同一
		}
		
		#ifdef TWTEST_TAKIEFF
		{	//水飛沫絵エフェクト追加
			VecFx32 *offs;
			work->eoa_splash_p = EoaSplash_Set(
					tw, Player_VecPosPtrGet(tw->fsys->player) );
			offs = EoaSplash_OffsetPosPtrGet( work->eoa_splash_p );
			*offs = DATA_TakiUpSplashOffsTbl[0];
		}
		#endif
		
		*seq_no = SEQNO_TAKINOBORI_MOVE0;
		return( RET_NON );
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　滝登り　移動0
 * @param	tw	TORNWORLD_WORK
 * @retval	BOOL	TRUE=再帰
 */
//--------------------------------------------------------------
static PROCRET ScrTakiNobori_Move0(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	WFALLRET ret;
	EOA_PTR eoa = Player_JointEoaGet( tw->fsys->player );
	FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( tw->fsys->player );
	SCRWORK_TAKINOBORI *work = tw_ScrProcWorkGet( tw );
	WFALLWORK *wfall = &work->wfall_wk;
	
	#ifdef DEBUG_TAKI_KUDARI_PAUSE
	if( (sys.trg & PAD_BUTTON_A) ){
		work->pause_flag = (work->pause_flag + 1) & 0x01;
	}
	
	if( work->pause_flag ){
		return( RET_NON );
	}
	#endif
	
	ret = WaterFallMove_Move( tw, wfall );
	FieldOBJ_VecPosSet( fldobj, &wfall->target_pos );
	FieldOBJ_VecDrawOffsSet( fldobj, &wfall->shake_offs );
	TakiNobori_ActorProc( tw, work );
	
	{
		VecFx32 *shake = FE_FldOBJNamiPokeR_ShakeOffsetPtrGet( eoa );
		*shake = wfall->shake_offs;
		
		if( work->cm_count == 1 ){
			VecFx32 *offs = FE_FldOBJNamiPokeR_PokeOffsetPtrGet( eoa );
			
			if( offs->z < NUM_FX32(10) ){
				offs->x += work->np_offs_val.x;
				offs->y += work->np_offs_val.y;
				offs->z += work->np_offs_val.z;
			}
		}
	}
	
	if( ret == WFALLRET_LAST ){				//最後の1グリッド
		work->frame = 2;	//1grid(16) 8
		tw_OBJRotateAnmSet( tw, fldobj, -90, work->frame );
		
		{
			work->np_rot_val.x = NUM_FX32(90) / 2;
			work->np_rot_val.y = 0;
			work->np_rot_val.z = 0;
			
			work->np_offs_val.x = 0;
			work->np_offs_val.y = 0;
			work->np_offs_val.z = NUM_FX32(-4) / 2; //10->6
			
			work->np_rev_val = NUM_FX32(70) / 2;	//20->90
			FE_FldOBJNamiPokeR_StatusBitOFF( eoa, NPOKER_BIT_SHAKE_OUTSIDE );
		}
		
		*seq_no = SEQNO_TAKINOBORI_MOVE1;
	}
	
	GF_ASSERT( ret != WFALLRET_END );
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　滝登り　移動1
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrTakiNobori_Move1(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	WFALLRET ret;
	EOA_PTR eoa = Player_JointEoaGet( tw->fsys->player );
	FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( tw->fsys->player );
	SCRWORK_TAKINOBORI *work = tw_ScrProcWorkGet( tw );
	WFALLWORK *wfall = &work->wfall_wk;
		
	ret = WaterFallMove_Move( tw, wfall );
	FieldOBJ_VecPosSet( fldobj, &wfall->target_pos );
	FieldOBJ_VecDrawOffsSet( fldobj, &wfall->shake_offs );
	TakiNobori_ActorProc( tw, work );
	
	{
		VecFx32 *offs;
		ROTATE *rot = FE_FldOBJNamiPokeR_RotatePtrGet( eoa );
		twAngleAdd360Fx( &work->np_rot.x, work->np_rot_val.x );
		twAngleAdd360Fx( &work->np_rot.y, work->np_rot_val.y );
		twAngleAdd360Fx( &work->np_rot.z, work->np_rot_val.z );
		rot->x = FX32_NUM( work->np_rot.x );
		rot->y = FX32_NUM( work->np_rot.y );
		rot->z = FX32_NUM( work->np_rot.z );
		
		twAngleAdd360Fx( &work->np_rev, work->np_rev_val );
		FE_FldOBJNamiPokeR_AngleRevSet( eoa, FX32_NUM(work->np_rev) );
		
		{
			VecFx32 *offs = FE_FldOBJNamiPokeR_PokeOffsetPtrGet( eoa );
			offs->x += work->np_offs_val.x;
			offs->y += work->np_offs_val.y;
			offs->z += work->np_offs_val.z;
		}
	}
	
	if( ret == WFALLRET_END ){
		int gx,gy,gz;
		const SCRDATA_TAKINOBORI *dat = data;
		const WFALLDATA *wfd = &dat->wfall_data;
		VecFx32 clear = {0,0,0};
		
		gx = FieldOBJ_NowPosGX_Get( fldobj ) + wfd->move_gx;
		gy = FieldOBJ_NowPosGY_Get( fldobj ) + G_GRID_H_GRID( wfd->move_gy );
		gz = FieldOBJ_NowPosGZ_Get( fldobj ) + wfd->move_gz;
		FieldOBJ_NowPosGX_Set( fldobj, gx );
		FieldOBJ_NowPosGY_Set( fldobj, gy );
		FieldOBJ_NowPosGZ_Set( fldobj, gz );
		FieldOBJ_GPosUpdate( fldobj );
		FieldOBJ_DirDispCheckSet( fldobj, DIR_RIGHT );
		FieldOBJ_DirMoveSet( fldobj, DIR_RIGHT );
		
		FieldOBJ_VecDrawOffsSet( fldobj, &clear );
		FieldOBJ_VecDrawOffsOutSideSet( fldobj, &clear );
		
		tw_MapDataGndDataPosDataSet(
			tw, gx, H_GRID_G_GRID(gy), gz, TW_GNDTYPE_MAX );
		Player_MoveBitSet_TWorld( tw->fsys->player, HEROTWTYPE_ROOF );
		FieldOBJ_StatusBitSet_HeightGetOFF( fldobj, TRUE );
		
		{	//波乗りパラメタ初期化
			{
				VecFx32 * np_offs_p =
					FE_FldOBJNamiPokeR_PokeOffsetPtrGet( eoa );
				*np_offs_p = clear;
				
				np_offs_p =
					FE_FldOBJNamiPokeR_ShakeOffsetPtrGet( eoa );
				*np_offs_p = clear;
			}
			
			FE_FldOBJNamiPokeR_AngleRevSet( eoa, 90 );
			FE_FldOBJNamiPokeR_RotateDirInit(
					eoa, DIR_RIGHT, HEROTWTYPE_ROOF );
			
			FE_FldOBJNamiPokeR_StatusBitOFF(
				eoa,
				NPOKER_BIT_ROTATE_OFF		|
				NPOKER_BIT_ANGLE_REV_OFF	|
				NPOKER_BIT_POKE_OFFS_OFF 	|
				NPOKER_BIT_SHAKE_OUTSIDE );
		}
		
		#ifdef TWTEST_TAKIEFF
		if( work->eoa_splash_p != NULL ){	//水飛沫削除
			EOA_Delete( work->eoa_splash_p );
			work->eoa_splash_p = NULL;
		}
		#endif
		
		Snd_SeStopBySeqNo( SE_YABURETA_TAKINOBORI, 0 );
		
		*seq_no = SEQNO_TAKINOBORI_LAST;
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　滝登り　最後の惰性移動
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrTakiNobori_Last(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	u32 tbl[3] = { AC_WALKGU_L_2F, AC_WALKGU_L_4F, AC_WALKGU_L_8F };
	SCRWORK_TAKINOBORI *work = tw_ScrProcWorkGet( tw );
	FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( tw->fsys->player );
	
	if( FieldOBJ_AcmdSetCheck(fldobj) == FALSE ){
		return( RET_NON );
	}
	
	if( work->acmd_count >= 3 ){
		return( RET_END );
	}
	
	FieldOBJ_AcmdSet( fldobj, tbl[work->acmd_count] );
	work->acmd_count++;
	return( RET_NON );
}

//--------------------------------------------------------------
///	スクリプトテーブル　滝登り
//--------------------------------------------------------------
static const TW_SCR_PROC DATA_ScrTbl_TakiNobori[SEQNO_TAKINOBORI_MAX] =
{
	ScrTakiNobori_Init,
	ScrTakiNobori_StartTurn,
	ScrTakiNobori_Move0,
	ScrTakiNobori_Move1,
	ScrTakiNobori_Last,
};

//==============================================================================
//	スクリプト　スクリプトコール
//==============================================================================
//--------------------------------------------------------------
///	進行番号
//--------------------------------------------------------------
enum
{
	SEQNO_SCRCALL_INIT,
	SEQNO_SCRCALL_WAIT,
	SEQNO_SCRCALL_MAX,
};

//--------------------------------------------------------------
///	SCRDATA_SCRCALL
//--------------------------------------------------------------
typedef struct
{
	u32 scr_id;
}SCRDATA_SCRCALL;

//--------------------------------------------------------------
/**
 * スクリプト　スクリプトコール
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrScriptCall_Init(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	const SCRDATA_SCRCALL *scr = data;
	EventCall_Script( ev, scr->scr_id, NULL, NULL );
	(*seq_no)++;
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　スクリプトコール　ウェイト
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrScriptCall_Wait(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	return( RET_END );
}

//--------------------------------------------------------------
///	スクリプトテーブル　スクリプト呼び出し
//--------------------------------------------------------------
static const TW_SCR_PROC DATA_ScrTbl_ScriptCall[SEQNO_SCRCALL_MAX] =
{
	ScrScriptCall_Init,
	ScrScriptCall_Wait,
};

//==============================================================================
//	スクリプト　SYSWORKセット
//==============================================================================
//--------------------------------------------------------------
///	進行番号
//--------------------------------------------------------------
enum
{
	SEQNO_TWEVSEQ_INIT,
	SEQNO_TWEVSEQ_MAX,
};

//--------------------------------------------------------------
///	SCRDATA_SCRCALL
//--------------------------------------------------------------
typedef struct
{
	u32 seq_no;
}SCRDATA_EVSEQ;

//--------------------------------------------------------------
/**
 * スクリプト　sysworkセット
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrTwEvSeqSet_Init(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	const SCRDATA_EVSEQ *seq = data;
	EVENTWORK *evwork = SaveData_GetEventWork( tw->fsys->savedata );
	SysWork_TornWorldEventSeqNoSet( evwork, seq->seq_no );
	return( RET_END );
}

//--------------------------------------------------------------
///	スクリプトテーブル　イベントシーケンスセット
//--------------------------------------------------------------
static const TW_SCR_PROC DATA_ScrTbl_TwEvSeqSet[SEQNO_TWEVSEQ_MAX] =
{
	ScrTwEvSeqSet_Init,
};

//==============================================================================
//	スクリプト　ギラティナ影フラグセット
//==============================================================================
//--------------------------------------------------------------
///	進行番号
//--------------------------------------------------------------
enum
{
	SEQNO_GIRASFLAG_INIT,
	SEQNO_GIRASFLAG_MAX,
};

//--------------------------------------------------------------
///	SCRDATA_SCRCALL
//--------------------------------------------------------------
typedef struct
{
	u32 flag_no;
}SCRDATA_GIRASFLAG;

//--------------------------------------------------------------
/**
 * スクリプト　ギラティナ影フラグセット
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrGirasFlagSet_Init(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	const SCRDATA_GIRASFLAG *dat = data;
	EVENTWORK *evwork = SaveData_GetEventWork( tw->fsys->savedata );
	SysFlag_TornWorldGirasFlag( evwork, SYSFLAG_MODE_SET, dat->flag_no );
	return( RET_END );
}

//--------------------------------------------------------------
///	スクリプトテーブル　ギラティナ影フラグセット
//--------------------------------------------------------------
static const TW_SCR_PROC DATA_ScrTbl_GirasFlagSet[SEQNO_GIRASFLAG_MAX] =
{
	ScrGirasFlagSet_Init,
};

//==============================================================================
//	スクリプト　破れた世界イベントフラグセット
//==============================================================================
//--------------------------------------------------------------
///	進行番号
//--------------------------------------------------------------
enum
{
	SEQNO_TWEVFLAG_SET_INIT,
	SEQNO_TWEVFLAG_SET_MAX,
};

//--------------------------------------------------------------
///	進行番号
//--------------------------------------------------------------
enum
{
	SEQNO_TWEVFLAG_OFF_INIT,
	SEQNO_TWEVFLAG_OFF_MAX,
};

//--------------------------------------------------------------
///	SCRDATA_TWEVFLAG
//--------------------------------------------------------------
typedef struct
{
	u32 flag_no;	//TWEVFLAGNO_
}SCRDATA_TWEVFLAG;

//--------------------------------------------------------------
/**
 * スクリプト　破れた世界イベントフラグセット
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrTwEvFlagSet_Init(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	const SCRDATA_TWEVFLAG *dat = data;
	tw_SaveDataEventFlagON( tw, dat->flag_no );
	return( RET_END );
}

//--------------------------------------------------------------
///	スクリプトテーブル　破れた世界イベントフラグセット
//--------------------------------------------------------------
static const TW_SCR_PROC DATA_ScrTbl_TwEvFlagSet[SEQNO_TWEVFLAG_SET_MAX] =
{
	ScrTwEvFlagSet_Init,
};

//--------------------------------------------------------------
/**
 * スクリプト　破れた世界イベントフラグ落とし
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrTwEvFlagOFF_Init(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	const SCRDATA_TWEVFLAG *dat = data;
	tw_SaveDataEventFlagOFF( tw, dat->flag_no );
	return( RET_END );
}

//--------------------------------------------------------------
///	スクリプトテーブル　破れた世界イベントフラグセット
//--------------------------------------------------------------
static const TW_SCR_PROC DATA_ScrTbl_TwEvFlagOFF[SEQNO_TWEVFLAG_OFF_MAX] =
{
	ScrTwEvFlagOFF_Init,
};

//==============================================================================
//	スクリプト	ギラティナ影
//==============================================================================
//--------------------------------------------------------------
///	進行番号
//--------------------------------------------------------------
enum
{
	SEQNO_GIRAS_INIT,
	SEQNO_GIRAS_WAIT,
	SEQNO_GIRAS_MAX,
};

//--------------------------------------------------------------
/**
 * スクリプト　ギラティナ影セット
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrGirasMove_Init(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	const SCRDATA_GIRAS *dat = data;
	tw_GirasActAdd( tw, dat );
	*seq_no = SEQNO_GIRAS_WAIT;
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　ギラティナ影セット　待ち
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrGirasMove_Wait(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	if( tw_GirasActMoveWait(tw) == TRUE ){
		tw_GirasActDelete( tw );
		return( RET_END );
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
///	スクリプトテーブル　ギラティナ影セット
//--------------------------------------------------------------
static const TW_SCR_PROC DATA_ScrTbl_GirasSet[SEQNO_GIRAS_MAX] =
{
	ScrGirasMove_Init,
	ScrGirasMove_Wait,
};

//==============================================================================
//	スクリプト　八層深部　ギラティナ登場
//==============================================================================
//--------------------------------------------------------------
///	進行番号
//--------------------------------------------------------------
enum
{
	SEQNO_GIRA_APPEAR_INIT,
	SEQNO_GIRA_APPEAR_ADD_WAIT,
	SEQNO_GIRA_APPEAR_MOVE,
	SEQNO_GIRA_APPEAR_FADE_IN,
	SEQNO_GIRA_APPEAR_LAST_WAIT,
	SEQNO_GIRA_APPEAR_MAX,
};

//--------------------------------------------------------------
///	SCRWORK_GIRAAPP
//--------------------------------------------------------------
typedef struct
{
	fx32 evy;
	fx32 bg_evy;
	VecFx32 offs;
	int frame;
	FIELD_OBJ_PTR fldobj;
	u16 pltt_tbl[16];
}SCRWORK_GIRAAPP;

//--------------------------------------------------------------
///	ギラティナテクスチャパレット
//--------------------------------------------------------------
static const u16 DATA_GiraPlttTbl[16] =
{
	0x32c9, 0x20d5, 0x104f, 0x3dad, 0x4a10, 0x2929, 0x5672, 0x62d5,
	0x6bff, 0x0f5d, 0x092a, 0x0a55, 0x215e, 0x18c6, 0x0000, 0x7fff,
};

//--------------------------------------------------------------
/**
 * ギラティナパレットフェード
 * @param	work	SCRWORK_GIRAAPP
 * @retval	nothing
 */
//--------------------------------------------------------------
static void ScrGiraAppear_PlttFade( SCRWORK_GIRAAPP *work )
{
	u32 i,evy = FX32_NUM( work->evy );
	
	for( i = 0; i < 16; i++ ){
		ColorTargetFade( DATA_GiraPlttTbl[i], 0, evy, &work->pltt_tbl[i] );
	}
}

//--------------------------------------------------------------
/**
 * 破れた世界　ギラティナOBJテクスチャパレットフェード
 * @param	fsys	FIELDSYS_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
void FldTornWorld_GiratinaPlttFade( FIELDSYS_WORK *fsys )
{
	TORNWORLD_WORK *tw = fsys->fldmap->Work;
	
	if( tw->draw_giratina_fade_flag == TRUE ){
		SCRWORK_GIRAAPP *work = tw_ScrProcWorkGet( tw );
		FIELD_OBJ_BLACT_CONT *cont =
			FieldOBJSys_BlActContGet( tw->fsys->fldobjsys );
		TEXRES_MANAGER_PTR resm = FieldOBJ_BlActCont_TexResManageGet(cont);
		TEXRES_OBJ_PTR reso = TEXRESM_GetResObj( resm, GIRATINAORIGIN );
		NNSGfdPlttKey plttkey = TEXRESM_GetPlttKeyPTR( reso );
		u32 addr = NNS_GfdGetPlttKeyAddr( plttkey );
		AddVramTransferManager(
			NNS_GFD_DST_3D_TEX_PLTT, addr, work->pltt_tbl, 32 );
	}
}

//--------------------------------------------------------------
/**
 * スクリプト　ギラティナ登場　初期化
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrGiraAppear_Init(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	SCRWORK_GIRAAPP *work;
	
	work = tw_ScrProcWorkInit( tw, sizeof(SCRWORK_GIRAAPP) );
	
	work->fldobj = tw_FldOBJEventAdd(
			tw, ZONE_ID_D34R0110, TW_OBJID_0110_GIRA );
	work->offs.y = GRID_SIZE_FX32( 10 );
	FieldOBJ_VecDrawOffsOutSideSet( work->fldobj, &work->offs );
	*seq_no = SEQNO_GIRA_APPEAR_ADD_WAIT;
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　ギラティナ登場　追加ウェイト
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrGiraAppear_AddWait(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	BLACT_WORK_PTR blact;
	SCRWORK_GIRAAPP *work;
	
	work = tw_ScrProcWorkGet( tw );
	blact = FieldOBJ_DrawBlAct00_BlActPtrGet( work->fldobj );
	
	if( blact != NULL ){	//追加完了　移動開始
		work->evy = NUM_FX32( 16 );
		ScrGiraAppear_PlttFade( work );
		tw->draw_giratina_fade_flag = TRUE;
		
		work->bg_evy = NUM_FX32( 0 );
		tw_PlttFadeSystemChg( tw, TRUE );
		
		tw->cloud_speed_lv = CLOUD_SPEED_LV_2;
		
//		Snd_BgmPlay( SEQ_PL_EV_GIRA );      //ギラティナ登場
//		Snd_BgmPlay( SEQ_PL_EV_GIRA2 );      //ギラティア飛び立つ時
//		Snd_BgmPlay( SEQ_PL_D_GIRATINA );    //マップの曲
		Snd_SePlay( SE_YABURETA_GIRATINA_ADVENT	);
		
		*seq_no = SEQNO_GIRA_APPEAR_MOVE;
		return( TRUE );
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　ギラティナ登場　移動
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrGiraAppear_Move(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	SCRWORK_GIRAAPP *work;
	
	work = tw_ScrProcWorkGet( tw );
	
	work->bg_evy += NUM_FX32(8) / (3*30);
	
	if( work->bg_evy >= NUM_FX32(TW_PLTT_EVY_GIRAS_APP) ){
		work->bg_evy = NUM_FX32( TW_PLTT_EVY_GIRAS_APP );
	}
	
	tw_PlttFadeEvySet( tw, FX32_NUM(work->bg_evy));
	
	if( work->offs.y >= GRID_SIZE_FX32(1) ){
		work->offs.y -= 0x1000;
	}else{
		work->offs.y -= 0x0800;
	}
	
	if( work->offs.y < 0 &&
		work->bg_evy >= NUM_FX32(TW_PLTT_EVY_GIRAS_APP) ){
		work->offs.y = 0;
		*seq_no = SEQNO_GIRA_APPEAR_FADE_IN;
	}
	
	FieldOBJ_VecDrawOffsOutSideSet( work->fldobj, &work->offs );
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　ギラティナ登場　フェードイン
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrGiraAppear_FadeIn(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	BLACT_WORK_PTR blact;
	NNSG3dResMdl *pResMdl;
	SCRWORK_GIRAAPP *work;
	
	work = tw_ScrProcWorkGet( tw );
	blact = FieldOBJ_DrawBlAct00_BlActPtrGet( work->fldobj );
	pResMdl = BLACT_ResMdlGet( blact );
	
	work->evy -= NUM_FX32(16) / (3*30);	//3sec
	
	if( work->evy < 0 ){
		work->evy = 0;
		*seq_no = SEQNO_GIRA_APPEAR_LAST_WAIT;
	}
	
	ScrGiraAppear_PlttFade( work );
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　ギラティナ登場　最後ウェイト
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrGiraAppear_LastWait(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	SCRWORK_GIRAAPP *work;
	
	work = tw_ScrProcWorkGet( tw );
	
	work->frame++;
	
	if( work->frame >= 30 ){
		tw->draw_giratina_fade_flag = FALSE;
		return( RET_END );
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
///	スクリプトテーブル　ギラティナ登場
//--------------------------------------------------------------
static const TW_SCR_PROC DATA_ScrTbl_GiraAppear[SEQNO_GIRA_APPEAR_MAX] =
{
	ScrGiraAppear_Init,
	ScrGiraAppear_AddWait,
	ScrGiraAppear_Move,
	ScrGiraAppear_FadeIn,
	ScrGiraAppear_LastWait,
};

//==============================================================================
///	スクリプト	岩落としユクシー登場
//==============================================================================
//--------------------------------------------------------------
///	進行番号
//--------------------------------------------------------------
enum
{
	SEQNO_ROCKPOKE_UK_INIT,
	SEQNO_ROCKPOKE_UK_MOVE0,
	SEQNO_ROCKPOKE_UK_MOVE1,
	SEQNO_ROCKPOKE_UK_MOVE2,
	SEQNO_ROCKPOKE_UK_MOVE3,
	SEQNO_ROCKPOKE_UK_MOVE4,
	SEQNO_ROCKPOKE_UK_MAX,
};

//--------------------------------------------------------------
///	SCRWORK_ROCKEV_Y
//--------------------------------------------------------------
typedef struct
{
	int tbl_no;
	int tbl_speed;
	int count;
	fx32 def_y;
	fx32 add_y;
	VecFx32 offs;
	FIELD_OBJ_PTR fldobj;
}SCRWORK_ROCKEV_Y;

//--------------------------------------------------------------
/**
 * スクリプト　岩落としユクシー登場　初期化
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrRockPokeYukushi_Init(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	SCRWORK_ROCKEV_Y *work;
	
	work = tw_ScrProcWorkInit( tw, sizeof(SCRWORK_ROCKEV_Y) );
	
	work->fldobj = tw_FldOBJEventAdd(
			tw, ZONE_ID_D34R0107, TW_OBJID_0107_YUKUSHI );
	
	Snd_PMVoicePlay( MONSNO_REI, 0 );
	
	work->tbl_speed = 1;
	*seq_no = SEQNO_ROCKPOKE_UK_MOVE0;
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　岩落としユクシー　上昇
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrRockPokeYukushi_Move0(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	SCRWORK_ROCKEV_Y *work;
	fx32 speed = NUM_FX32( 2 );
	
	work = tw_ScrProcWorkGet( tw );
	
	if( SIZE_GRID_FX32(work->offs.y) < 16 ){
		speed <<= 1;	
	}
	
	work->offs.y += speed;
	FieldOBJ_VecDrawOffsOutSideSet( work->fldobj, &work->offs );
	
	if( SIZE_GRID_FX32(work->offs.y) >= 17 ){
		*seq_no = SEQNO_ROCKPOKE_UK_MOVE1;
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　岩落としユクシー　前進して岩の上に
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrRockPokeYukushi_Move1(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	SCRWORK_ROCKEV_Y *work;
	
	work = tw_ScrProcWorkGet( tw );
	work->offs.z -= NUM_FX32( 1 );
	FieldOBJ_VecDrawOffsOutSideSet( work->fldobj, &work->offs );
	
	if( SIZE_GRID_FX32(work->offs.z) <= -2 ){
		work->def_y = work->offs.y;
		*seq_no = SEQNO_ROCKPOKE_UK_MOVE2;
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　岩落としユクシー　岩の上でゆらゆら
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrRockPokeYukushi_Move2(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	SCRWORK_ROCKEV_Y *work;
	fx32 tbl[8] = { 0x0000,0x800,0x1000,0x2000,0x4000,0x6000,0x7000,0x8000};
	
	work = tw_ScrProcWorkGet( tw );
	work->offs.y = work->def_y + tbl[work->tbl_no>>1];
	FieldOBJ_VecDrawOffsOutSideSet( work->fldobj, &work->offs );
	
	work->tbl_no += work->tbl_speed;
	
	if( work->tbl_no >= 15 || work->tbl_no <= 0 ){
		work->tbl_speed = -work->tbl_speed;
		
		if( work->tbl_no == 0 ){
			work->count++;
			
			if( work->count >= 3 ){
				work->offs.y = work->def_y + tbl[work->tbl_no>>1];
				*seq_no = SEQNO_ROCKPOKE_UK_MOVE3;
			}
		}
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　岩落としユクシー　下がる
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrRockPokeYukushi_Move3(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	SCRWORK_ROCKEV_Y *work;
	
	work = tw_ScrProcWorkGet( tw );
	work->offs.z += NUM_FX32( 1 );
	FieldOBJ_VecDrawOffsOutSideSet( work->fldobj, &work->offs );
	
	if( SIZE_GRID_FX32(work->offs.z) == 1 ){
		*seq_no = SEQNO_ROCKPOKE_UK_MOVE4;
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　岩落としユクシー　降りる
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrRockPokeYukushi_Move4(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	SCRWORK_ROCKEV_Y *work;
	
	work = tw_ScrProcWorkGet( tw );
	
	if( work->add_y < NUM_FX32(2) ){
		work->add_y += 0x0200;
	}
	
	work->offs.y -= work->add_y;
	FieldOBJ_VecDrawOffsOutSideSet( work->fldobj, &work->offs );
	
	if( SIZE_GRID_FX32(work->offs.y) <= 0 ){
		tw_FldOBJDelete( tw, work->fldobj );
		tw_SaveDataEventFlagON( tw, TWEVFLAGNO_0107YUKUSHIEV );
		tw_SaveDataEventFlagON( tw, TWEVFLAGNO_0108YUKUSHI );
		work->fldobj = tw_FldOBJEventAdd(
			tw, ZONE_ID_D34R0108, TW_OBJID_0108_YUKU );
		return( RET_END );
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
///	スクリプトテーブル　岩ユクシー
//--------------------------------------------------------------
static const TW_SCR_PROC DATA_ScrTbl_RockPokeYukushi[SEQNO_ROCKPOKE_UK_MAX] =
{
	ScrRockPokeYukushi_Init,
	ScrRockPokeYukushi_Move0,
	ScrRockPokeYukushi_Move1,
	ScrRockPokeYukushi_Move2,
	ScrRockPokeYukushi_Move3,
	ScrRockPokeYukushi_Move4,
};

//==============================================================================
//	スクリプト　岩落としアグノム
//==============================================================================
//--------------------------------------------------------------
///	進行番号
//--------------------------------------------------------------
enum
{
	SEQNO_ROCKPOKE_AG_INIT,
	SEQNO_ROCKPOKE_AG_MOVE0,
	SEQNO_ROCKPOKE_AG_MOVE1,
	SEQNO_ROCKPOKE_AG_MOVE2,
	SEQNO_ROCKPOKE_AG_MAX,
};

//--------------------------------------------------------------
///	SCRWORK_ROCKEV_A
//--------------------------------------------------------------
typedef struct
{
	fx32 add_y;
	VecFx32 offs;
	TCB_PTR acmd_tcb;
	FIELD_OBJ_PTR fldobj;
}SCRWORK_ROCKEV_A;

//--------------------------------------------------------------
///	アグノムアニメ
//--------------------------------------------------------------
static const FIELD_OBJ_ACMD_LIST DATA_RockPokeAgunomuAcmdList[] =
{
	{AC_WAIT_32F,1},
	{AC_WALK_R_32F,1},
	{AC_WALK_R_32F,1},
	{AC_WALK_L_16F,1},
	{AC_WALK_L_32F,1},
	{AC_WAIT_16F,1},
	{ACMD_END,0},
};

//--------------------------------------------------------------
/**
 * スクリプト　岩落としアグノム登場　初期化
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrRockPokeAgunomu_Init(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	SCRWORK_ROCKEV_A *work;
	
	work = tw_ScrProcWorkInit( tw, sizeof(SCRWORK_ROCKEV_A) );
	
	work->fldobj = tw_FldOBJEventAdd(
			tw, ZONE_ID_D34R0107, TW_OBJID_0107_AGUNOMU );
	
	Snd_PMVoicePlay( MONSNO_HAI, 0 );
	
	*seq_no = SEQNO_ROCKPOKE_AG_MOVE0;
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　岩落としアグノム　上昇
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrRockPokeAgunomu_Move0(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	SCRWORK_ROCKEV_A *work;
	fx32 speed = NUM_FX32( 2 );
	
	work = tw_ScrProcWorkGet( tw );
	
	if( SIZE_GRID_FX32(work->offs.y) < 12 ){
		speed <<= 1;	
	}
	
	work->offs.y += speed;
	FieldOBJ_VecDrawOffsOutSideSet( work->fldobj, &work->offs );
	
	if( SIZE_GRID_FX32(work->offs.y) >= 13 ){
		work->acmd_tcb = FieldOBJ_AcmdListSet(
				work->fldobj, DATA_RockPokeAgunomuAcmdList );
		*seq_no = SEQNO_ROCKPOKE_AG_MOVE1;
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　岩落としアグノム　アニメ終了待ち
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrRockPokeAgunomu_Move1(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	SCRWORK_ROCKEV_A *work;
	
	work = tw_ScrProcWorkGet( tw );
	
	if( FieldOBJ_AcmdListEndCheck(work->acmd_tcb) == TRUE ){
		FieldOBJ_AcmdListEnd( work->acmd_tcb );
		*seq_no = SEQNO_ROCKPOKE_AG_MOVE2;
		return( RET_CONTINUE );
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　岩落としアグノム　降りる
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrRockPokeAgunomu_Move2(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	SCRWORK_ROCKEV_A *work;
	
	work = tw_ScrProcWorkGet( tw );
	
	if( work->add_y < NUM_FX32(2) ){
		work->add_y += 0x0200;
	}
	
	work->offs.y -= work->add_y;
	FieldOBJ_VecDrawOffsOutSideSet( work->fldobj, &work->offs );
	
	if( SIZE_GRID_FX32(work->offs.y) <= 0 ){
		tw_FldOBJDelete( tw, work->fldobj );
		tw_SaveDataEventFlagON( tw, TWEVFLAGNO_0107AGUNOMUEV );
		tw_SaveDataEventFlagON( tw, TWEVFLAGNO_0108AGUNOMU );
		work->fldobj = tw_FldOBJEventAdd(
			tw, ZONE_ID_D34R0108, TW_OBJID_0108_AGU );
		return( RET_END );
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
///	スクリプトテーブル　岩アグノム
//--------------------------------------------------------------
static const TW_SCR_PROC DATA_ScrTbl_RockPokeAgunomu[SEQNO_ROCKPOKE_AG_MAX] =
{
	ScrRockPokeAgunomu_Init,
	ScrRockPokeAgunomu_Move0,
	ScrRockPokeAgunomu_Move1,
	ScrRockPokeAgunomu_Move2,
};

//==============================================================================
///	スクリプト	岩落としエムリット
//==============================================================================
//--------------------------------------------------------------
///	進行番号
//--------------------------------------------------------------
enum
{
	SEQNO_ROCKPOKE_EM_INIT,
	SEQNO_ROCKPOKE_EM_MOVE0,
	SEQNO_ROCKPOKE_EM_MOVE1,
	SEQNO_ROCKPOKE_EM_MOVE2,
	SEQNO_ROCKPOKE_EM_MAX,
};

//--------------------------------------------------------------
///	SCRWORK_ROCKEV_E
//--------------------------------------------------------------
typedef struct
{
	fx32 add_y;
	VecFx32 offs;
	TCB_PTR acmd_tcb_poke;
	TCB_PTR acmd_tcb_jiki;
	FIELD_OBJ_PTR fldobj;
}SCRWORK_ROCKEV_E;

//--------------------------------------------------------------
///	エムリットアニメ z 68
//--------------------------------------------------------------
static const FIELD_OBJ_ACMD_LIST DATA_RockPokeEmuritAcmdListX80Z67[] =
{
	{AC_WAIT_32F,1},
	{AC_WALK_R_8F,2},
	
	{AC_WALK_U_4F,2},
	{AC_WALK_R_4F,2},
	{AC_WALK_D_4F,2},
	{AC_WALK_L_4F,2},
	
	{AC_WALK_U_4F,2},
	{AC_WALK_R_4F,2},
	{AC_WALK_D_4F,2},
	{AC_WALK_L_4F,2},
	
	{AC_WALK_L_4F,2},
	{AC_WALK_L_8F,1},
	{AC_WALK_L_16F,1},
	{AC_WAIT_32F,1},
	
	{AC_WALK_R_16F,2},
	{AC_WALK_R_32F,1},
	{AC_WAIT_16F,1},
	{ACMD_END,0},
};

static const FIELD_OBJ_ACMD_LIST DATA_RockPokeEmuritAcmdListX80Z68[] =
{
	{AC_WAIT_32F,1},
	{AC_WALK_R_8F,2},
	
	{AC_WALK_U_4F,1},
	{AC_WALK_R_4F,2},
	{AC_WALK_D_4F,2},
	{AC_WALK_L_4F,2},
	
	{AC_WALK_U_4F,2},
	{AC_WALK_R_4F,2},
	{AC_WALK_D_4F,2},
	{AC_WALK_L_4F,2},
	{AC_WALK_U_4F,1},
	
	{AC_WALK_L_4F,2},
	{AC_WALK_L_8F,1},
	{AC_WALK_L_16F,1},
	{AC_WAIT_32F,1},
	
	{AC_WALK_R_16F,2},
	{AC_WALK_R_32F,1},
	{AC_WAIT_16F,1},
	{ACMD_END,0},
};

//--------------------------------------------------------------
///	自機アニメ
//--------------------------------------------------------------
static const FIELD_OBJ_ACMD_LIST DATA_RockPokeJikiAcmdListX80Z67[] =
{
	{AC_WAIT_32F,1},
	{AC_WAIT_8F,2},
	
	{AC_DIR_L,1},
	{AC_WAIT_1F,3},
	{AC_DIR_U,1},
	{AC_WAIT_1F,3},
	{AC_DIR_R,1},
	{AC_WAIT_1F,3},
	{AC_DIR_D,1},
	{AC_WAIT_1F,3},
	
	{AC_DIR_L,1},
	{AC_WAIT_1F,3},
	{AC_DIR_U,1},
	{AC_WAIT_1F,3},
	{AC_DIR_R,1},
	{AC_WAIT_1F,3},
	{AC_DIR_D,1},
	{AC_WAIT_1F,3},
	
	{AC_DIR_L,1},
	{ACMD_END,0},
};

static const FIELD_OBJ_ACMD_LIST DATA_RockPokeJikiAcmdListX80Z68[] =
{
	{AC_WAIT_32F,1},
	{AC_WAIT_8F,2},
	
	{AC_DIR_L,1},
	{AC_WAIT_1F,3},
	{AC_DIR_U,1},
	{AC_WAIT_1F,3},
	{AC_DIR_R,1},
	{AC_WAIT_1F,3},
	{AC_DIR_D,1},
	{AC_WAIT_1F,3},
	
	{AC_DIR_L,1},
	{AC_WAIT_1F,3},
	{AC_DIR_U,1},
	{AC_WAIT_1F,3},
	{AC_DIR_R,1},
	{AC_WAIT_1F,3},
	{AC_DIR_D,1},
	{AC_WAIT_1F,3},
	
	{AC_DIR_L,1},
	{ACMD_END,0},
};

//--------------------------------------------------------------
/**
 * スクリプト　岩落としエムリット登場　初期化
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrRockPokeEmurit_Init(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	SCRWORK_ROCKEV_E *work;
	
	work = tw_ScrProcWorkInit( tw, sizeof(SCRWORK_ROCKEV_A) );
	
	work->fldobj = tw_FldOBJEventAdd(
			tw, ZONE_ID_D34R0107, TW_OBJID_0107_EMURIT );
	
	Snd_PMVoicePlay( MONSNO_AI, 0 );
	*seq_no = SEQNO_ROCKPOKE_EM_MOVE0;
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　岩落としエムリット　上昇
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrRockPokeEmurit_Move0(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	SCRWORK_ROCKEV_E *work;
	fx32 speed = NUM_FX32( 2 );
	
	work = tw_ScrProcWorkGet( tw );
	
	if( SIZE_GRID_FX32(work->offs.y) < 8 ){
		speed <<= 1;	
	}
	
	work->offs.y += speed;
	FieldOBJ_VecDrawOffsOutSideSet( work->fldobj, &work->offs );
	
	if( SIZE_GRID_FX32(work->offs.y) >= 9 ){
		int gx,gy,gz;
		const FIELD_OBJ_ACMD_LIST *j_list,*p_list;
		
		twJikiGridPosGet( tw, &gx, &gy, &gz );
		
		if( gz == 67 ){
			j_list = DATA_RockPokeJikiAcmdListX80Z67;
			p_list = DATA_RockPokeEmuritAcmdListX80Z67;
		}else{	//z68
			j_list = DATA_RockPokeJikiAcmdListX80Z68;
			p_list = DATA_RockPokeEmuritAcmdListX80Z68;
		}
		
		work->acmd_tcb_poke = FieldOBJ_AcmdListSet(
				work->fldobj, p_list );
		work->acmd_tcb_jiki = FieldOBJ_AcmdListSet(
				Player_FieldOBJGet(tw->fsys->player), j_list );
		
		*seq_no = SEQNO_ROCKPOKE_EM_MOVE1;
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　岩落としエムリット　アニメ終了待ち
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrRockPokeEmurit_Move1(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	SCRWORK_ROCKEV_E *work;
	
	work = tw_ScrProcWorkGet( tw );
	
	if( FieldOBJ_AcmdListEndCheck(work->acmd_tcb_poke) == TRUE &&
		FieldOBJ_AcmdListEndCheck(work->acmd_tcb_jiki) == TRUE ){
		FieldOBJ_AcmdListEnd( work->acmd_tcb_poke );
		FieldOBJ_AcmdListEnd( work->acmd_tcb_jiki );
		*seq_no = SEQNO_ROCKPOKE_EM_MOVE2;
		return( RET_CONTINUE );
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　岩落としエムリット　降りる
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrRockPokeEmurit_Move2(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	SCRWORK_ROCKEV_E *work;
	
	work = tw_ScrProcWorkGet( tw );
	
	if( work->add_y < NUM_FX32(2) ){
		work->add_y += 0x0200;
	}
	
	work->offs.y -= work->add_y;
	FieldOBJ_VecDrawOffsOutSideSet( work->fldobj, &work->offs );
	
	if( SIZE_GRID_FX32(work->offs.y) <= 0 ){
		tw_FldOBJDelete( tw, work->fldobj );
		tw_SaveDataEventFlagON( tw, TWEVFLAGNO_0107EMURITEV );
		tw_SaveDataEventFlagON( tw, TWEVFLAGNO_0108EMURIT );
		work->fldobj = tw_FldOBJEventAdd(
			tw, ZONE_ID_D34R0108, TW_OBJID_0108_EM );
		return( RET_END );
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
///	スクリプトテーブル　岩エムリット
//--------------------------------------------------------------
static const TW_SCR_PROC DATA_ScrTbl_RockPokeEmurit[SEQNO_ROCKPOKE_EM_MAX] =
{
	ScrRockPokeEmurit_Init,
	ScrRockPokeEmurit_Move0,
	ScrRockPokeEmurit_Move1,
	ScrRockPokeEmurit_Move2,
};

//==============================================================================
///	スクリプト	八層深部飛び石表示
//==============================================================================
//--------------------------------------------------------------
///	進行番号
//--------------------------------------------------------------
enum
{
	SEQNO_STONE0110_ON_0,
	SEQNO_STONE0110_ON_1,
	SEQNO_STONE0110_ON_MAX,
};

#define STONE0110WAIT_MAX (48)

//--------------------------------------------------------------
///	カメラデータ
//--------------------------------------------------------------
const TW_CMACTDATA DATA_CmActStone0110ON =
{
	{0,0,0,0},
	16,0,0,
	DIR_UP,
	20,
};

//--------------------------------------------------------------
///	SCRWORK_STONE0110_ON
//--------------------------------------------------------------
typedef struct
{
	s16 vanish_off_no;
	s16 frame;
}SCRWORK_STONE0110_ON;

//--------------------------------------------------------------
/**
 * 八層深部飛び石表示　初期化
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrStone0110_ON_0(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	SCRWORK_STONE0110_ON *work;
	work = tw_ScrProcWorkInit( tw, sizeof(SCRWORK_STONE0110_ON) );
	
	tw_CameraActionSet( tw, &DATA_CmActStone0110ON );
	
	work->frame = 20+16;	//cm wait + start wait
	work->vanish_off_no = 1;	//vanish No 1,2,3
	*seq_no = SEQNO_STONE0110_ON_1;
	return( RET_CONTINUE );
}

//--------------------------------------------------------------
/**
 * 八層深部飛び石表示　表示
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrStone0110_ON_1(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	SCRWORK_STONE0110_ON *work = tw_ScrProcWorkGet( tw );
	
	work->frame--;
	
	if( work->frame <= 0 ){
		work->frame = STONE0110WAIT_MAX;
		Snd_SeStopBySeqNo( SE_YABURETA_GROUND_APPEAR, 0 ); //確実にSEを鳴らす
		tw_FStoneVanishFlagOFF( tw, work->vanish_off_no );
		work->vanish_off_no++;
		
		if( work->vanish_off_no >= 4 ){
			return( RET_END );
		}
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
///	スクリプトテーブル　八層深部飛び石表示
//--------------------------------------------------------------
static const TW_SCR_PROC DATA_ScrTbl_Stone0110_ON[SEQNO_STONE0110_ON_MAX] =
{
	ScrStone0110_ON_0,
	ScrStone0110_ON_1,
};

//==============================================================================
///	スクリプト	八層深部飛び石非表示
//==============================================================================
//--------------------------------------------------------------
///	進行番号
//--------------------------------------------------------------
enum
{
	SEQNO_STONE0110_OFF_0,
	SEQNO_STONE0110_OFF_1,
	SEQNO_STONE0110_OFF_2,
	SEQNO_STONE0110_OFF_MAX,
};

#if 0
//--------------------------------------------------------------
///	カメラデータ
//--------------------------------------------------------------
const TW_CMACTDATA DATA_CmActStone0110OFF =
{
	{0,0,0,0},
	0,0,0,
	DIR_UP,
	12,
};
#endif

//--------------------------------------------------------------
///	SCRWORK_STONE0110_ON
//--------------------------------------------------------------
typedef struct
{
	s16 vanish_off_no;
	s16 frame;
}SCRWORK_STONE0110_OFF;

//--------------------------------------------------------------
/**
 * 八層深部飛び石非表示　初期化
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrStone0110_OFF_0(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	SCRWORK_STONE0110_OFF *work;
	work = tw_ScrProcWorkInit( tw, sizeof(SCRWORK_STONE0110_OFF) );
	
//	tw_CameraActionSet( tw, &DATA_CmActStone0110OFF );
	
	work->frame = 16;	//start wait
	work->vanish_off_no = 3;	//vanish No 3,2,1
	*seq_no = SEQNO_STONE0110_OFF_1;
	return( RET_CONTINUE );
}

//--------------------------------------------------------------
/**
 * 八層深部飛び石非表示　順番に消す
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrStone0110_OFF_1(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	SCRWORK_STONE0110_OFF *work = tw_ScrProcWorkGet( tw );
	
	work->frame--;
	
	if( work->frame <= 0 ){
		if( work->vanish_off_no >= 1 ){		//飛び石消してる最中
			work->frame = STONE0110WAIT_MAX;
			Snd_SeStopBySeqNo( SE_YABURETA_GROUND_APPEAR, 0 );
			tw_FStoneVanishFlagON( tw, work->vanish_off_no );
			work->vanish_off_no--;
		}else{								//全て消した後
			work->frame = 8;	//last wait
			*seq_no = SEQNO_STONE0110_OFF_2;
		}
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * 八層深部飛び石非表示　カメラ移動
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrStone0110_OFF_2(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	SCRWORK_STONE0110_OFF *work = tw_ScrProcWorkGet( tw );
	
	work->frame--;
	
	if( work->frame <= 0 ){
		return( RET_END );
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
///	スクリプトテーブル　八層深部飛び石非表示
//--------------------------------------------------------------
static const TW_SCR_PROC DATA_ScrTbl_Stone0110_OFF[SEQNO_STONE0110_OFF_MAX] =
{
	ScrStone0110_OFF_0,
	ScrStone0110_OFF_1,
	ScrStone0110_OFF_2,
};

//==============================================================================
//	エフェクト　滝登り　水しぶき
//==============================================================================
#ifdef TWTEST_TAKIEFF
//--------------------------------------------------------------
///	SPLASH_RES
//--------------------------------------------------------------
typedef struct
{
	FRO_ANM ranm_0;
	FRO_ANM ranm_1;
	FRO_OBJ robj;
	FRO_MDL rmdl;
}SPLASH_RES;

//--------------------------------------------------------------
///	EOA_SPLASH_HEAD
//--------------------------------------------------------------
typedef struct
{
	FE_SYS *fes;
	const VecFx32 *oya_pos_p;
}EOA_SPLASH_HEAD;

//--------------------------------------------------------------
///	EOA_SPLASH_WORK
//--------------------------------------------------------------
typedef struct
{
	VecFx32 offs;
	ROTATE rotate;
	SPLASH_RES *res_p;
	EOA_SPLASH_HEAD head;
	
	u32 debug;
}EOA_SPLASH_WORK;

static const EOA_H_NPP DATA_EoaH_Splash;

//--------------------------------------------------------------
/**
 * EOA 水飛沫　セット
 * @param	tw	TORNWORLD_WORK
 * @param	pos	追従するpos
 * @retval	EOA_PTR
 */
//--------------------------------------------------------------
static EOA_PTR EoaSplash_Set( TORNWORLD_WORK *tw, const VecFx32 *pos )
{
	EOA_PTR eoa;
	EOA_SPLASH_HEAD head;
	
	head.fes = tw->fsys->fes;
	head.oya_pos_p = pos;
	
	eoa = FE_EoaAddNpp( tw->fsys->fes,
		&DATA_EoaH_Splash, NULL, 0, &head, TW_TCBPRI_SPLASH );
	
	return( eoa );
}

//--------------------------------------------------------------
/**
 * EOA 水飛沫　オフセット座標取得
 * @param	EOA_PTR eoa
 * @retval	VecFx32* オフセット座標*
 */
//--------------------------------------------------------------
static VecFx32 * EoaSplash_OffsetPosPtrGet( EOA_PTR eoa )
{
	EOA_SPLASH_WORK *work;
	
	work = EOA_LocalWorkGet( eoa );
	return( &work->offs );
}	

//--------------------------------------------------------------
/**
 * EOA 水飛沫　初期化
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work
 * @retval	int TRUE=初期化完了
 */
//--------------------------------------------------------------
static int EoaSplash_Init( EOA_PTR eoa, void *wk )
{
	ARCHANDLE *arc_h;
	EOA_SPLASH_WORK *work = wk;
	const EOA_SPLASH_HEAD *head_p;
	
	head_p = EOA_AddPtrGet( eoa );
	work->head = *head_p;
	
	work->res_p = sys_AllocMemoryLo( HEAPID_FIELD, sizeof(SPLASH_RES) );
	
	arc_h = FE_ArcHandleGet( work->head.fes );
	
	FRO_MDL_ResSetArcLoad( &work->res_p->rmdl, 0,
		arc_h, NARC_fldeff_d33_wave_obj_nsbmd,
		HEAPID_FIELD, ALLOC_LO );
	FRO_MDL_TexTransBindVTaskAdd( &work->res_p->rmdl );
	
	FRO_ANM_AnmResSetArcLoad( &work->res_p->ranm_0, 0,
			arc_h, NARC_fldeff_d33_wave_obj_nsbtp,
			HEAPID_FIELD, ALLOC_LO );
	FRO_ANM_AllocAnmObjInMdl(
		&work->res_p->ranm_0, &work->res_p->rmdl, HEAPID_FIELD );
	FRO_ANM_AnmObjInitInMdl( &work->res_p->ranm_0, &work->res_p->rmdl );
	
	FRO_ANM_AnmResSetArcLoad( &work->res_p->ranm_1, 0,
			arc_h, NARC_fldeff_d33_wave_obj_nsbca,
			HEAPID_FIELD, ALLOC_LO );
	FRO_ANM_AllocAnmObjInMdl(
		&work->res_p->ranm_1, &work->res_p->rmdl, HEAPID_FIELD );
	FRO_ANM_AnmObjInitInMdl( &work->res_p->ranm_1, &work->res_p->rmdl );
	
	FRO_OBJ_InitAll( &work->res_p->robj,
		&work->res_p->rmdl, &work->res_p->ranm_0 );
	FRO_OBJ_AddAnmInAnm( &work->res_p->robj, &work->res_p->ranm_1 );
	
	NNS_G3dMdlUseMdlAlpha( work->res_p->rmdl.pResMdl );
	NNS_G3dMdlSetMdlAlphaAll( work->res_p->rmdl.pResMdl, 24 );
	
	//ArchiveDataHandleClose( arc_h );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * EOA 水飛沫　削除
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaSplash_Delete( EOA_PTR eoa, void *wk )
{
	EOA_SPLASH_WORK *work = wk;
	
	FRO_MDL_DeleteAll( &work->res_p->rmdl );
	FRO_ANM_DeleteAll( &work->res_p->ranm_0 );
	FRO_ANM_DeleteAll( &work->res_p->ranm_1 );
	sys_FreeMemoryEz( work->res_p );
}

//--------------------------------------------------------------
/**
 * EOA 水飛沫　動作
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaSplash_Move( EOA_PTR eoa, void *wk )
{
	EOA_SPLASH_WORK *work = wk;
	VecFx32 *offs = &work->offs;
	VecFx32 pos = *work->head.oya_pos_p;
	
	#ifdef DEBUG_SPLASH_EFF
	{
		u32 trg = sys.trg;
		u32 cont = sys.cont;
		ROTATE *rot = &work->rotate;
		
		if( trg & PAD_BUTTON_SELECT ){
			OS_Printf( "スプラッシュ　RX(%d),RY(%d),RZ(%d)\n",
				rot->x,rot->y,rot->z );
			OS_Printf( "OX(%d),OY(%d),OZ(%d)\n",
				FX32_NUM(offs->x), FX32_NUM(offs->y), FX32_NUM(offs->z) );
		}
		
		if( trg & PAD_BUTTON_B ){
			work->debug = (work->debug + 1) % 3;
			
			switch( work->debug ){
			#if 0
			case 0: OS_Printf( "スプラッシュ RX操作\n" ); break;
			case 1: OS_Printf( "スプラッシュ RY操作\n" ); break;
			case 2: OS_Printf( "スプラッシュ RZ操作\n" ); break;
			#endif
			case 0: OS_Printf( "スプラッシュ OX操作\n" ); break;
			case 1: OS_Printf( "スプラッシュ OY操作\n" ); break;
			case 2: OS_Printf( "スプラッシュ OZ操作\n" ); break;
			}
		}
		
		if( cont & PAD_BUTTON_L ){
			switch( work->debug ){
			#if 0
			case 0: twAngleAdd360( (s16*)&rot->x, -2 ); break;
			case 1: twAngleAdd360( (s16*)&rot->y, -2 ); break;
			case 2: twAngleAdd360( (s16*)&rot->z, -2 ); break;
			#endif
			case 0: offs->x += NUM_FX32( 1 ); break;
			case 1: offs->y += NUM_FX32( 1 ); break;
			case 2: offs->z += NUM_FX32( 1 ); break;
			}
		}else if( cont & PAD_BUTTON_R ){
			switch( work->debug ){
			#if 0
			case 0: twAngleAdd360( (s16*)&rot->x, 2 ); break;
			case 1: twAngleAdd360( (s16*)&rot->y, 2 ); break;
			case 2: twAngleAdd360( (s16*)&rot->z, 2 ); break;
			#endif
			case 0: offs->x += NUM_FX32( -1 ); break;
			case 1: offs->y += NUM_FX32( -1 ); break;
			case 2: offs->z += NUM_FX32( -1 ); break;
			}
		}
		
	}
	#endif
	
	pos.x += offs->x;
	pos.y += offs->y;
	pos.z += offs->z;
	EOA_MatrixSet( eoa, &pos );
	FRO_ANM_Play( &work->res_p->ranm_0, FX32_ONE, ANMLOOP_ON );
	FRO_ANM_Play( &work->res_p->ranm_1, FX32_ONE, ANMLOOP_ON );
}

//--------------------------------------------------------------
/**
 * EOA 水飛沫　描画
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaSplash_Draw( EOA_PTR eoa, void *wk )
{
	VecFx32 pos;
	EOA_SPLASH_WORK *work = wk;
	EOA_MatrixGet( eoa, &pos );
	FRO_OBJ_DrawRotate( &work->res_p->robj, &pos, &work->rotate );
}

//--------------------------------------------------------------
///	EOA 水飛沫 ヘッダー
//--------------------------------------------------------------
static const EOA_H_NPP DATA_EoaH_Splash =
{
	sizeof(EOA_SPLASH_WORK),
	EoaSplash_Init,
	EoaSplash_Delete,
	EoaSplash_Move,
	EoaSplash_Draw,
};
#endif

//==============================================================================
//	パーツ
//==============================================================================
//--------------------------------------------------------------
/**
 * TW_GRID_RECT_SIZE 範囲チェック
 * @param	gx	GRID X
 * @param	gy	GRID Y
 * @param	gz	GRID Z
 * @param	rect	TW_GRID_RECT_SIZE
 * @retval	BOOL TRUE=rectの内
 */
//--------------------------------------------------------------
static BOOL twGridRectCheck(
	int gx, int gy, int gz, const TW_GRID_RECT_SIZE *rect )
{
	if( (gy >= rect->gy) && (gy <= rect->gy+rect->gy_size) &&
		(gz >= rect->gz) && (gz <= rect->gz+rect->gz_size) &&
		(gx >= rect->gx) && (gx <= rect->gx+rect->gx_size) ){
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * TW_GRID_RECT_SIZE 範囲チェック 0origin
 * @param	gx	GRID X
 * @param	gy	GRID Y
 * @param	gz	GRID Z
 * @param	rect	TW_GRID_RECT_SIZE
 * @retval	BOOL TRUE=rectの内
 */
//--------------------------------------------------------------
#if 0
static BOOL twGridRectCheckOrg0(
	int gx, int gy, int gz, const TW_GRID_RECT_SIZE *rect )
{
	TW_GRID_RECT_SIZE d_rect = *rect;
	
	d_rect.gx_size--;
	if( d_rect.gx_size < 0 ){ d_rect.gx_size = 0; }
	d_rect.gy_size--;
	if( d_rect.gy_size < 0 ){ d_rect.gy_size = 0; }
	d_rect.gz_size--;
	if( d_rect.gz_size < 0 ){ d_rect.gz_size = 0; }
	
	if( gy >= d_rect.gy && gy <= (d_rect.gy+d_rect.gy_size) &&
		gz >= d_rect.gz && gz <= (d_rect.gz+d_rect.gz_size) &&
		gx >= d_rect.gx && gx <= (d_rect.gx+d_rect.gx_size) ){
		return( TRUE );
	}
	
	return( FALSE );
}
#endif

//--------------------------------------------------------------
/**
 * 現角度から目標角度への近距離差分を取得　0x10000
 * @param	a	現角度
 * @param	ma	目標角度
 * @retval	int	m_rotへの近距離差分
 */
//--------------------------------------------------------------
static int twAngle16ShortDiffGet( u16 a, u16 ma )
{
	int s0 = (s16)ma - (s16)a;
	int s1 = s0;
	
	if( s1 < 0 ){
		s1 = -s1;
	}
	
	if( s1 > 0x8000 ){ // r180 over
		s1 = (s1 + 0x8000) % 0x8000;
		
		if( a < ma ){
			s1 = -s1;
		}
		
		s0 = s1;
	}
	
	return( s0 );
}

//--------------------------------------------------------------
/**
 * 角度増減　360
 * @param	rot	fx32
 * @param	val	fx32
 * @retval	nothing
 */
//--------------------------------------------------------------
static void twAngleAdd360( s16 *rot, s16 val )
{
	(*rot) += val;
	if( (*rot) < 0 ){ do{(*rot)+=360;}while((*rot)<0); }
	else{ (*rot) %= 360; }
}

//--------------------------------------------------------------
/**
 * 角度増減　360 fx
 * @param	rot	fx32
 * @param	val	fx32
 * @retval	nothing
 */
//--------------------------------------------------------------
static void twAngleAdd360Fx( fx32 *rot, fx32 val )
{
	(*rot) += val;
	if( (*rot) < 0 ){ do{(*rot)+=NUM_FX32(360);}while((*rot)<0); }
	else{ (*rot) %= NUM_FX32( 360 ); }
}

//--------------------------------------------------------------
/**
 * 自機現グリッド座標読み込み　相対座標。Yはグリッドサイズに変換される
 * @param	tw	TORNWORLD_WORK
 * @param	gx	グリッド座標x
 * @param	gy	グリッド座標y
 * @param	gz	グリッド座標z
 * @retval	nothing
 */
//--------------------------------------------------------------
static void twJikiGridPosGet(
	TORNWORLD_WORK *tw, int *gx, int *gy, int *gz )
{
	FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( tw->fsys->player );
	*gx = FieldOBJ_NowPosGX_Get( fldobj );
	*gy = FieldOBJ_NowPosGY_Get( fldobj );
	*gy = H_GRID_G_GRID( *gy );	//H -> G グリッド変換
	*gz = FieldOBJ_NowPosGZ_Get( fldobj );
}

//--------------------------------------------------------------
/**
 * 現在、やぶれた世界かどうかをチェックします。
 * @param	fsys	FIELDSYS_WORK *fsys
 * @retval	BOOL	TRUE=やぶれたせかい
 */
//--------------------------------------------------------------
BOOL FldTornWorld_TornWorldCheck( FIELDSYS_WORK *fsys )
{
	GIMMICKWORK *gmc;
	gmc = SaveData_GetGimmickWork( GameSystem_GetSaveData(fsys) );
	
	if( GIMMICKWORK_GetAssignID(gmc) != FLD_GIMMICK_TORNWORLD ){
		return( FALSE );	//破れた世界ではない
	}
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * やぶれたせかい　壁判定
 * @param	fsys	FIELDSYS_WORK *fsys
 * @retval	BOOL	TRUE=壁
 */
//--------------------------------------------------------------
BOOL FldTornWorld_WallCheck( FIELDSYS_WORK *fsys, int gx, int gy, int gz )
{
	u16 attr;
	TORNWORLD_WORK *tw = fsys->fldmap->Work;
	
	//壁、方向別に切りかえ
	attr = tw_MapDataGndDataAttrGetCheckPos( tw, gx, gy, gz );
	
	if( attr == TWATTR_NOT || attr == TWATTR_OVER ){
		#ifdef DEBUG_TW_PRINTF
		{
		 if( attr == TWATTR_NOT ){
			OS_Printf("TW 移動方向 AttrNot GX=%d,GY=%d,GZ=%d\n",gx,gy,gz);
		 }else{
			OS_Printf("TW 移動方向 AttrOver GX=%d,GY=%d,GZ=%d\n",gx,gy,gz);
		 }
		}
		#endif
		return( TRUE );
	}
	
	attr = ATRD_MFLAG( attr );
	
	if( attr ){
		#ifdef DEBUG_TW_PRINTF
		{
			OS_Printf("TW 移動方向 AttrHit GX=%d,GY=%d,GZ=%d\n",gx,gy,gz);
		}
		#endif
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * やぶれたせかい　特殊地形の範囲内か
 * @param	fsys	FIELDSYS_WORK *fsys
 * @retval	BOOL	FALSE=範囲外
 */
//--------------------------------------------------------------
BOOL FldTornWorld_SpMapDataInsideCheck(
	FIELDSYS_WORK *fsys, int gx, int gy, int gz )
{
	u16 attr;
	TORNWORLD_WORK *tw = fsys->fldmap->Work;
		
	//壁、方向別に切りかえ
	attr = tw_MapDataGndDataAttrGetCheckPos( tw, gx, gy, gz );
	
	if( attr == TWATTR_NOT || attr == TWATTR_OVER ){
		return( FALSE );
	}
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * やぶれたせかい　特殊地形があるかどうか。
 * 現在地形と合わない地形は無視。
 * @param	fsys	FIELDSYS_WORK *fsys
 * @retval	BOOL	FALSE=範囲外
 */
//--------------------------------------------------------------
BOOL FldTornWorld_SpMapDataExistCheck(
	FIELDSYS_WORK *fsys, int gx, int gy, int gz )
{
	TORNWORLD_WORK *tw = fsys->fldmap->Work;
	s16 type = tw_MapDataNowGndTypeGet( tw );
	return( tw_MapDataGndDataPosExistCheck(tw,gx,gy,gz,type) );
}

//--------------------------------------------------------------
/**
 * やぶれたせかい　指定座標の特殊地形を読み込む。
 * 現在地形と合わない地形は無視。
 * @param	fsys	FIELDSYS_WORK *fsys
 * @retval	BOOL	FALSE=範囲外
 */
//--------------------------------------------------------------
void FldTornWorld_SpMapDataPosExistLoad(
	FIELDSYS_WORK *fsys, int gx, int gy, int gz )
{
	TORNWORLD_WORK *tw = fsys->fldmap->Work;
	s16 type = tw_MapDataNowGndTypeGet( tw );
	
	if( tw_MapDataGndDataPosExistCheck(tw,gx,gy,gz,type) == TRUE ){
		tw_MapDataGndDataPosDataSet( tw, gx, gy, gz, type );
		return;
	}
	
	GF_ASSERT( 0 );
}

//--------------------------------------------------------------
/**
 * やぶれたせかい　アトリビュート取得
 * @param	fsys	FIELDSYS_WORK *fsys
 * @param	gx		グリッドGX
 * @param	gy		グリッドGY(地面グリッド単位
 * @param	gz		グリッドGZ
 * @param	attr	アトリビュート格納先
 * @retval	BOOL	TRUE=特殊地形である FALSE=特殊地形ではない
 */
//--------------------------------------------------------------
BOOL FldTornWorld_SpAttrGet(
	FIELDSYS_WORK *fsys, int gx, int gy, int gz, u32 *attr )
{
	GIMMICKWORK *gmc;
	gmc = SaveData_GetGimmickWork( GameSystem_GetSaveData(fsys) );
	
	if( GIMMICKWORK_GetAssignID(gmc) != FLD_GIMMICK_TORNWORLD ){
		GF_ASSERT( 0 );
		return( FALSE );	//破れた世界ではない
	}
	
	{
		u16 ret;
		TORNWORLD_WORK *tw = fsys->fldmap->Work;
		
		ret = tw_MapDataGndDataAttrGetCheckPos( tw, gx, gy, gz );
		
		if( ret == TWATTR_NOT || ret == TWATTR_OVER ){
			*attr = MATR_IsNotAttrGet();	//無効アトリビュート
			return( FALSE );
		}
		
		*attr = ATRD_ATTR( ret );
		return( TRUE );
	}
}

#if 0
//--------------------------------------------------------------
/**
 * ゾーンIDのオフセット座標を取得
 * @param	zone	zone id
 * @param	offs	オフセット格納先
 * @retval	nothing
 */
//--------------------------------------------------------------
static void twZoneOffsetPosGet( int zone_id, VecFx32 *offs )
{
	const TW_ZONE_LINKDATA *link = tw_ZoneLinkDataGet( zone_id );
	offs->x = GRID_SIZE_FX32( link->offs_gx );
	offs->y = GRID_SIZE_FX32( link->offs_gy );
	offs->z = GRID_SIZE_FX32( link->offs_gz );
}
#endif

//--------------------------------------------------------------
/**
 * 各ゾーンの始点座標を取得　破れた世界のみ各値に始点をセット。
 * field_tornworld.cは展開されているが、
 * TORNWORLD_WORKが初期化されていない状況で呼ばれる関数である。
 * アーカイブデータを開いて座標を取得する為に遅い。連続コールは控える事。
 * @param	zone	zone id
 * @param	x		グリッドX座標格納先
 * @param	y		グリッドY座標格納先
 * @param	z		グリッドZ座標格納先
 * @retval	nothing
 */
//--------------------------------------------------------------
void FldTornWorld_ArcDataMapOriginGridPosGet(
			int zone_id, int *x, int *y, int *z )
{
	TW_MDATA_IDX md_idx;
	ARCHANDLE *arc_h_map = ArchiveDataHandleOpen( ARC_TWORLD, HEAPID_FIELD );
	tw_MapDataIdxInitMain( arc_h_map, &md_idx );
	tw_MapDataIdxZoneIDOriginGridPosGetMain( &md_idx, zone_id, x, y, z );
	tw_MapDataIdxDeleteMain( &md_idx );
	ArchiveDataHandleClose( arc_h_map );
}

//--------------------------------------------------------------
/**
 * 現在のゾーンIDを取得
 * @param	tw	TORNWORLD_WORK
 * @retval	u32	zone id
 */
//--------------------------------------------------------------
static u32 twNowZoneID( TORNWORLD_WORK *tw )
{
	return( tw->fsys->location->zone_id );
}

//--------------------------------------------------------------
/**
 * TW_GNDTYPEに対応したHEROTWTYPEを返す
 * @param	type	TW_GNDTYPE_GROUND等
 * @retval	HEROTWTYPE HEROTWTYPE
 */
//--------------------------------------------------------------
static HEROTWTYPE twGndTypeHeroType( u32 type )
{
	switch( type ){
	case TW_GNDTYPE_GROUND: return( HEROTWTYPE_GROUND );
	case TW_GNDTYPE_SIDE_L: return( HEROTWTYPE_WLEFT );
	case TW_GNDTYPE_SIDE_R: return( HEROTWTYPE_WRIGHT );
	case TW_GNDTYPE_ROOF: return( HEROTWTYPE_ROOF );
	}
	return( HEROTWTYPE_ON );	//max
}

//--------------------------------------------------------------
/**
 * TWFLAGTYPEチェック
 * @param	tw			TORNWORLD_WORK
 * @param	type	TWFLAGTYPE
 * @param	flag	typeでチェックするflag
 * @retval	BOOL	TRUE=typeが成立している
 */
//--------------------------------------------------------------
static BOOL twFlagCheck( TORNWORLD_WORK *tw, u32 type, u32 flag )
{
	EVENTWORK *ev = SaveData_GetEventWork( tw->fsys->savedata );
	
	switch( type ){
	case TWFLAGTYPE_NON:
		return( TRUE );
	case TWFLAGTYPE_EV_OFF:			//ローカルイベント OFF
		if( tw_SaveDataEventFlagCheck(tw,flag) != TRUE ){
			return( TRUE );
		}
		break;
	case TWFLAGTYPE_EV_ON:			//ローカルイベント ON
		if( tw_SaveDataEventFlagCheck(tw,flag) == TRUE ){
			return( TRUE );
		}
		break;
	case TWFLAGTYPE_TWEVSEQ_EQ:		//flag == TWEVSEQ
		if( SysWork_TornWorldEventSeqNoGet(ev) == flag ){
			return( TRUE );
		}
		break;
	case TWFLAGTYPE_TWEVSEQ_LOW:	//flag <= TWEVSEQ
		if( SysWork_TornWorldEventSeqNoGet(ev) <= flag ){
			return( TRUE );
		}
		break;
	case TWFLAGTYPE_TWEVSEQ_HI:		//flag >= TWEVSEQ
		if( SysWork_TornWorldEventSeqNoGet(ev) >= flag ){
			return( TRUE );
		}
		break;
	case TWFLAGTYPE_GIRASFLAG_OFF:	//sysflag GIRAS OFF
		if( SysFlag_TornWorldGirasFlag(ev,SYSFLAG_MODE_CHECK,flag) == FALSE ){
			return( TRUE );
		}
		break;
	case TWFLAGTYPE_AKAGI_FLAG_EQ: //flag == AKAGI FLAG
		if( SysWork_TwAkagiAppearFlagGet(ev) == flag ){
			return( TRUE );
		}
		break;
	#ifdef PM_DEBUG
	default:
		GF_ASSERT( 0 );
	#endif
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * 8層深部限定 3Gジャンプアトリビュート無効チェック
 * @param	fsys	FIELDSYS_WORK
 * @param	gx		チェックXグリッド
 * @param	gz		チェックZグリッド
 * @retval	BOOL	TRUE=3Gジャンプ無効
 */
//--------------------------------------------------------------
BOOL FldTornWorld_NotJumpAttrCheck(
	FIELDSYS_WORK *fsys, int gx, int gz, int dir )
{
	TORNWORLD_WORK *tw = fsys->fldmap->Work;
	
	if( twNowZoneID(tw) == ZONE_ID_D34R0110 ){
		if( gz == 15 && gx == 15 && dir == DIR_DOWN ){
			EVENTWORK *ev = SaveData_GetEventWork( tw->fsys->savedata );
			u32 seq_no = SysWork_TornWorldEventSeqNoGet( ev );
			if( seq_no == TWEVSEQ_08D_03 ){
				return( TRUE );
			}
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * SE再生中チェックし再生中でなければSE再生
 * @param	no	SEQ_SE...
 * @retval	nothing
 */
//--------------------------------------------------------------
static void twSndCheckPlay( u16 no )
{
	if( Snd_SePlayCheck(no) == FALSE ){
		Snd_SePlay( no );
	}
}

//--------------------------------------------------------------
/**
 * フィールドOBJの座標を更新　ビルボード、描画座標も更新
 * @param	
 * @retval
 */
//--------------------------------------------------------------
static void twFldOBJVecPosUpdate( FIELD_OBJ_PTR fldobj, const VecFx32 *pos )
{
	BLACT_WORK_PTR blact;
	
	FieldOBJ_VecPosSet( fldobj, pos );
	
	blact = FieldOBJ_DrawBlAct00_BlActPtrGet( fldobj );
	
	if( blact != NULL ){
		FieldOBJ_BlActPosSet( fldobj, blact );
	}
}

//==============================================================================
//	debug
//==============================================================================
//--------------------------------------------------------------
//	DEBUG_TW_PRINTF_ON
//--------------------------------------------------------------
#ifdef DEBUG_TW_PRINTF_ON
//指定IDをプリント
static void DEBUG_twZoneIDPrintf( u32 zone_id )
{
	switch( zone_id ){
	case ZONE_ID_D34R0101: OS_Printf( "ZONE ID D34R0101 " ); break;
	case ZONE_ID_D34R0102: OS_Printf( "ZONE ID D34R0102 " ); break;
	case ZONE_ID_D34R0103: OS_Printf( "ZONE ID D34R0103 " ); break;
	case ZONE_ID_D34R0104: OS_Printf( "ZONE ID D34R0104 " ); break;
	case ZONE_ID_D34R0105: OS_Printf( "ZONE ID D34R0105 " ); break;
	case ZONE_ID_D34R0107: OS_Printf( "ZONE ID D34R0107 " ); break;
	case ZONE_ID_D34R0108: OS_Printf( "ZONE ID D34R0108 " ); break;
	case ZONE_ID_D34R0109: OS_Printf( "ZONE ID D34R0109 " ); break;
	case ZONE_ID_D34R0110: OS_Printf( "ZONE ID D34R0110 " ); break;
	case ZONE_ID_D34R0111: OS_Printf( "ZONE ID D34R0111 " ); break;
	default: GF_ASSERT( 0 ); break;
	}
}
#endif	//DEBUG_TW_PRINTF_ON

//--------------------------------------------------------------
//	DEBUG_TW_PRINTF
//--------------------------------------------------------------
#ifdef DEBUG_TW_PRINTF
//フィールドOBJ追加テーブルの中身をプリント
static void DEBUG_twFldOBJTblPrintf( TORNWORLD_WORK *tw )
{
	int i;
	TW_FLDOBJ_WORK *twfldobj = &tw->twfldobj;
	FIELD_OBJ_PTR fldobj;
	
	for( i = 0; i < TW_FLDOBJ_MAX; i++ ){
		fldobj = twfldobj->fldobj_tbl[i];
		
		if( fldobj != NULL ){
			OS_Printf( "No.%d objid(%xH) objcode(%xH), ",
				i, FieldOBJ_OBJIDGet(fldobj), FieldOBJ_OBJCodeGet(fldobj) );
			DEBUG_twZoneIDPrintf( FieldOBJ_ZoneIDGet(fldobj) );
			OS_Printf( "\n" );
		}
	}
}

//フィールドOBJ追加テーブルの中身チェック
static void DEBUG_twFldOBJTblCheck( TORNWORLD_WORK *tw )
{
	int i,j,code,zone_id,obj_id;
	TW_FLDOBJ_WORK *twfldobj = &tw->twfldobj;
	FIELD_OBJ_PTR *objtbl = twfldobj->fldobj_tbl;
	
	for( i = 0; i < TW_FLDOBJ_MAX-1; i++ ){
		if( objtbl[i] != NULL ){
			code = FieldOBJ_OBJCodeGet( objtbl[i] );
			zone_id = FieldOBJ_ZoneIDGet( objtbl[i] );
			obj_id = FieldOBJ_OBJIDGet( objtbl[i] );
	
			for( j = i + 1; j < TW_FLDOBJ_MAX; j++ ){
				if( objtbl[j] != NULL ){
					if( code == FieldOBJ_OBJCodeGet(objtbl[j]) &&
						zone_id == FieldOBJ_ZoneIDGet(objtbl[j]) &&
						obj_id == FieldOBJ_OBJIDGet(objtbl[j]) ){
						OS_Printf( "フィールドOBJ重複している\n" );
						DEBUG_twFldOBJTblPrintf( tw );
						GF_ASSERT( 0 );
					}
				}
			}
		}
	}
}

//フィールドOBJのステータス出力
static void DEBUG_twFldOBJStatusPrintf( FIELD_OBJ_PTR fldobj )
{
	OS_Printf( "TW FldOBJSt " );
	DEBUG_twZoneIDPrintf( FieldOBJ_ZoneIDGet(fldobj) );
	OS_Printf( " OBJ ID(%xH) OBJ CODE(%xH) GX(%d) GY(%d) GZ(%d)\n",
			FieldOBJ_OBJIDGet(fldobj),FieldOBJ_OBJCodeGet(fldobj),
			FieldOBJ_NowPosGX_Get(fldobj),
			H_GRID_G_GRID(FieldOBJ_NowPosGY_Get(fldobj)),
			FieldOBJ_NowPosGZ_Get(fldobj) );
}

//HEAPID_FIELDの空きサイズ出力
static void DEBUG_twFldHeapPrintf( void )
{
	u32 free = sys_GetHeapFreeSize( HEAPID_FIELD );
	OS_Printf("HEAPID_FIELD FreeSize=%08xH\n",free);
}
#endif	//DEBUG_TW_PRINTF

//----
#ifdef DEBUG_TWORLD_CAPTURE
//----
void DEBUG_FldTornWorld_StepVanish( void )
{
	DebugTWorldCaptureFlag_StepVanish ^= 1;
}

void DEBUG_FldTornWorld_CloudStop( void )
{
	DebugTWorldCaptureFlag_CloudStop ^= 1;
}

void DEBUG_FldTornWorld_BGVanish( FIELDSYS_WORK *fsys )
{
	DebugTWorldCaptureFlag_BGVanish ^= 1;
	
	if( DebugTWorldCaptureFlag_BGVanish == 0 ){
		TORNWORLD_WORK *tw = fsys->fldmap->Work;
		TW_PLTT_WORK *pltt = &tw->pltt;
		pltt->pltt_trans_flag = TRUE;
	}else{
		static u16 pltt[16];
		memset( pltt, 0, sizeof(u16)*16 );
		GF_BGL_PaletteSet(
			TW_BGFRM_BG, pltt,
			32, 32*(TW_BGFRM_BG_PLTT_OFFS) );
	}
}

CAMERA_ANGLE * DEBUG_FldTornWorld_CameraAngleOffsetGet( FIELDSYS_WORK *fsys )
{
	TORNWORLD_WORK *tw = fsys->fldmap->Work;
	TW_CAMERA_WORK *cm = &tw->camera;
	return( &cm->debug_cap_angle );
}

void DEBUG_FldTornWorld_CameraReset( FIELDSYS_WORK *fsys )
{
	TORNWORLD_WORK *tw = fsys->fldmap->Work;
	TW_CAMERA_WORK *work = &tw->camera;
	GF_CAMERA_PARAM cmp =
	{
		0x29aec1,
		{-0x29fe,0,0},
		GF_CAMERA_PERSPECTIV,
		0x05c1,
		0
	};
	
	GFC_SetCameraDistance( cmp.Distance, work->gfc_camera );
	GFC_SetCameraAngleRev( &cmp.Angle, work->gfc_camera );
	GFC_SetCameraPerspWay( cmp.PerspWay, work->gfc_camera );
	GFC_SetCameraView( cmp.View, work->gfc_camera );
	GFC_SetCameraClip( TWCM_CLIP_NEAR, TWCM_CLIP_FAR, work->gfc_camera );
	
	if( twNowZoneID(tw) != ZONE_ID_D34R0110 ){
		GFC_AddCameraPerspWay( 0x0681-0x05c1, work->gfc_camera );
	}
	
	work->debug_cap_angle.x = 0;
	work->debug_cap_angle.y = 0;
	work->debug_cap_angle.z = 0;
}
//----
#endif //DEBUG_TWORLD_CAPTURE
//----

//==============================================================================
//	data
//==============================================================================
//--------------------------------------------------------------
///	AC_JUMP系　高さYオフセット その0
//--------------------------------------------------------------
static const fx32 DATA_JumpOffsTblType0[OFFS_FRAME_MAX] =			// 12
{
	4*FX32_ONE,6*FX32_ONE,8*FX32_ONE,10*FX32_ONE,
	11*FX32_ONE,12*FX32_ONE,12*FX32_ONE,12*FX32_ONE,
	11*FX32_ONE,10*FX32_ONE,9*FX32_ONE,8*FX32_ONE,
	6*FX32_ONE,4*FX32_ONE,0*FX32_ONE,0*FX32_ONE
};

//--------------------------------------------------------------
///	自機イベントセット関数まとめ 壁ジャンプ
//--------------------------------------------------------------
static void (* const DATA_JikiPosEventProcWallJumpTbl[EVPOSTYPE_MAX])(
		TORNWORLD_WORK *tw, const TW_EVJUMPDATA *data ) =
{
	tw_JikiEventSet_WallJump,
};

//--------------------------------------------------------------
///	セルリソース　アーカイブインデックス　キャラ
//--------------------------------------------------------------
static const int DATA_TwClActDataArcIdxTbl_Char[TW_CLACT_RES_CHAR_MAX] =
{
	NARC_tw_arc_etc_yabu_cloud_01_NCGR,
	NARC_tw_arc_etc_yabu_cloud_02_NCGR,
	NARC_tw_arc_etc_yabu_cloud_03_NCGR,
	NARC_tw_arc_etc_yabu_cloud_04_NCGR,
	NARC_tw_arc_etc_yabu_cloud_05_NCGR,
	NARC_tw_arc_etc_yabu_cloud_06_NCGR,
	NARC_tw_arc_etc_yabu_cloud_07_NCGR,
};

//--------------------------------------------------------------
///	セルリソース　アーカイブインデックス　セル
//--------------------------------------------------------------
static const int DATA_TwClActDataArcIdxTbl_Cell[TW_CLACT_RES_CELL_MAX] =
{
	NARC_tw_arc_etc_yabu_cloud_01_NCER,
	NARC_tw_arc_etc_yabu_cloud_02_NCER,
	NARC_tw_arc_etc_yabu_cloud_03_NCER,
	NARC_tw_arc_etc_yabu_cloud_04_NCER,
	NARC_tw_arc_etc_yabu_cloud_05_NCER,
	NARC_tw_arc_etc_yabu_cloud_06_NCER,
	NARC_tw_arc_etc_yabu_cloud_07_NCER,
};

//--------------------------------------------------------------
///	セルリソース　アーカイブインデックス　アニメ
//--------------------------------------------------------------
static const int DATA_TwClActDataArcIdxTbl_Anm[TW_CLACT_RES_ANM_MAX] =
{
	NARC_tw_arc_etc_yabu_cloud_01_NANR,
	NARC_tw_arc_etc_yabu_cloud_02_NANR,
	NARC_tw_arc_etc_yabu_cloud_03_NANR,
	NARC_tw_arc_etc_yabu_cloud_04_NANR,
	NARC_tw_arc_etc_yabu_cloud_05_NANR,
	NARC_tw_arc_etc_yabu_cloud_06_NANR,
	NARC_tw_arc_etc_yabu_cloud_07_NANR,
};

//--------------------------------------------------------------
///	セルリソース　アーカイブインデックス　パレット
//--------------------------------------------------------------
static const int DATA_TwClActDataArcIdxTbl_Pltt[TW_CLACT_RES_PLTT_MAX] =
{
	NARC_tw_arc_etc_cloud_all_NCLR,
};

//--------------------------------------------------------------
///	セルリソース　雲種類別ID
//--------------------------------------------------------------
#ifdef TW_CELL_PLTT_TEST
static const CLACT_RESID DATA_CloudResIDTbl[CLOUD_MAX] =
{
	{TW_CLACT_RES_CHAR_CLOUD_0,TW_CLACT_RES_PLTT_CLOUD_0,
	TW_CLACT_RES_CELL_CLOUD_0,TW_CLACT_RES_ANM_CLOUD_0},
	{TW_CLACT_RES_CHAR_CLOUD_1,TW_CLACT_RES_PLTT_CLOUD_1,
	TW_CLACT_RES_CELL_CLOUD_1,TW_CLACT_RES_ANM_CLOUD_1},
	{TW_CLACT_RES_CHAR_CLOUD_2,TW_CLACT_RES_PLTT_CLOUD_2,
	TW_CLACT_RES_CELL_CLOUD_2,TW_CLACT_RES_ANM_CLOUD_2},
	{TW_CLACT_RES_CHAR_CLOUD_3,TW_CLACT_RES_PLTT_CLOUD_0,
	TW_CLACT_RES_CELL_CLOUD_3,TW_CLACT_RES_ANM_CLOUD_3},
	{TW_CLACT_RES_CHAR_CLOUD_4,TW_CLACT_RES_PLTT_CLOUD_3,
	TW_CLACT_RES_CELL_CLOUD_4,TW_CLACT_RES_ANM_CLOUD_4},
	{TW_CLACT_RES_CHAR_CLOUD_5,TW_CLACT_RES_PLTT_CLOUD_4,
	TW_CLACT_RES_CELL_CLOUD_5,TW_CLACT_RES_ANM_CLOUD_5},
	{TW_CLACT_RES_CHAR_CLOUD_6,TW_CLACT_RES_PLTT_CLOUD_4,
	TW_CLACT_RES_CELL_CLOUD_6,TW_CLACT_RES_ANM_CLOUD_6},
};
#else
static const CLACT_RESID DATA_CloudResIDTbl[CLOUD_MAX] =
{
	{TW_CLACT_RES_CHAR_CLOUD_0,TW_CLACT_RES_PLTT_CLOUD_0,
	TW_CLACT_RES_CELL_CLOUD_0,TW_CLACT_RES_ANM_CLOUD_0},
	{TW_CLACT_RES_CHAR_CLOUD_1,TW_CLACT_RES_PLTT_CLOUD_0,
	TW_CLACT_RES_CELL_CLOUD_1,TW_CLACT_RES_ANM_CLOUD_1},
	{TW_CLACT_RES_CHAR_CLOUD_2,TW_CLACT_RES_PLTT_CLOUD_0,
	TW_CLACT_RES_CELL_CLOUD_2,TW_CLACT_RES_ANM_CLOUD_2},
	{TW_CLACT_RES_CHAR_CLOUD_3,TW_CLACT_RES_PLTT_CLOUD_0,
	TW_CLACT_RES_CELL_CLOUD_3,TW_CLACT_RES_ANM_CLOUD_3},
	{TW_CLACT_RES_CHAR_CLOUD_4,TW_CLACT_RES_PLTT_CLOUD_0,
	TW_CLACT_RES_CELL_CLOUD_4,TW_CLACT_RES_ANM_CLOUD_4},
	{TW_CLACT_RES_CHAR_CLOUD_5,TW_CLACT_RES_PLTT_CLOUD_0,
	TW_CLACT_RES_CELL_CLOUD_5,TW_CLACT_RES_ANM_CLOUD_5},
	{TW_CLACT_RES_CHAR_CLOUD_6,TW_CLACT_RES_PLTT_CLOUD_0,
	TW_CLACT_RES_CELL_CLOUD_6,TW_CLACT_RES_ANM_CLOUD_6},
};
#endif

//--------------------------------------------------------------
///	雲初期化データ
//--------------------------------------------------------------
static const CLOUD_INITDATA DATA_EoaCloudInitDataTbl[CLOUD_SET_MAX] =
{
	{
		CLOUD_6,CLOUD_SPEED_TYPE_3,
		NUM_FX32(0),NUM_FX32(135),104,0x1400,
		{NUM_FX32(128),NUM_FX32(96+TW_CLOUD_OFFS_Y_BIG),0},
		{FX32_ONE*2,FX32_ONE*2,FX32_ONE*2}
	},
	{
		CLOUD_6,CLOUD_SPEED_TYPE_3,
		NUM_FX32(180),NUM_FX32(135),104,0x1400,
		{NUM_FX32(128),NUM_FX32(96+TW_CLOUD_OFFS_Y_BIG),0},
		{FX32_ONE*2,FX32_ONE*2,FX32_ONE*2}
	},
	{
		CLOUD_6,CLOUD_SPEED_TYPE_3,
		NUM_FX32(90),NUM_FX32(135),104,0x1400,
		{NUM_FX32(128),NUM_FX32(96+TW_CLOUD_OFFS_Y_BIG),0},
		{FX32_ONE*2,FX32_ONE*2,FX32_ONE*2}
	},
	{
		CLOUD_6,CLOUD_SPEED_TYPE_3,
		NUM_FX32(270),NUM_FX32(135),104,0x1400,
		{NUM_FX32(128),NUM_FX32(96+TW_CLOUD_OFFS_Y_BIG),0},
		{FX32_ONE*2,FX32_ONE*2,FX32_ONE*2}
	},
	{
		CLOUD_1,CLOUD_SPEED_TYPE_2,
		NUM_FX32(0),NUM_FX32(90),60,0x1000,
		{NUM_FX32(128),NUM_FX32(96+TW_CLOUD_OFFS_Y),0},
		{FX32_ONE,FX32_ONE,FX32_ONE}
	},
	{
		CLOUD_2,CLOUD_SPEED_TYPE_2,
		NUM_FX32(180),NUM_FX32(90),60,0x1000,
		{NUM_FX32(128),NUM_FX32(96+TW_CLOUD_OFFS_Y),0},
		{FX32_ONE,FX32_ONE,FX32_ONE}
	},
	{
		CLOUD_3,CLOUD_SPEED_TYPE_1,
		NUM_FX32(315),NUM_FX32(90),42,0x0c00,
		{NUM_FX32(128),NUM_FX32(96+TW_CLOUD_OFFS_Y),0},
		{FX32_ONE,FX32_ONE,FX32_ONE}
	},
	{
		CLOUD_4,CLOUD_SPEED_TYPE_1,
		NUM_FX32(135),NUM_FX32(90),42,0x0c00,
		{NUM_FX32(128),NUM_FX32(96+TW_CLOUD_OFFS_Y),0},
		{FX32_ONE,FX32_ONE,FX32_ONE}
	},
 	{
		CLOUD_5,CLOUD_SPEED_TYPE_0,
		NUM_FX32(0),NUM_FX32(90),0,0x0800,
		{NUM_FX32(128),NUM_FX32(96+TW_CLOUD_OFFS_Y),0},
		{FX32_ONE,FX32_ONE,FX32_ONE}
	},
};

//--------------------------------------------------------------
///	雲速度
//--------------------------------------------------------------
static const fx32 DATA_CloudAddSpeedTbl[CLOUD_SPEED_LV_MAX][CLOUD_SPEED_TYPE_MAX] =
{
	{ 0x0000, 0x0000, 0x0000, 0x0000 },	//CLOUD_SPEED_LV_0
	{ 0x0800, 0x0c00*2, 0x1000*3, 0x1400*4 },	//CLOUD_SPEED_LV_1
	{ -0x0800*4, -0x0c00*6, -0x1000*7, -0x1400*10 },	//CLOUD_SPEED_LV_2
};

//==============================================================================
//	data	飛び石
//==============================================================================
#if 0
//--------------------------------------------------------------
///	飛び石種類別アーカイブインデックス
//--------------------------------------------------------------
static const u32 DATA_FStoneArcIdxTbl[FSTONETYPE_MAX] =
{
	NARC_fldeff_d33_step01_nsbmd,
	NARC_fldeff_d33_step02_nsbmd,	
	NARC_fldeff_d33_step03a_nsbmd,
	NARC_fldeff_d33_step03b_nsbmd,
	NARC_fldeff_d33_step03c_nsbmd,
	NARC_fldeff_d33_step04_nsbmd,
	NARC_fldeff_d33_step05_nsbmd,
	NARC_fldeff_d33_step06_nsbmd,
	NARC_fldeff_d33_step07_nsbmd,
	NARC_fldeff_d33_step08_nsbmd,
	NARC_fldeff_d33_step09_nsbmd,
	NARC_fldeff_d33_step10_nsbmd,
	NARC_fldeff_d33_step11_nsbmd,
	NARC_fldeff_d33_step12_nsbmd,
	NARC_fldeff_d33_step13_nsbmd,
	NARC_fldeff_d33_step14_nsbmd,
	NARC_fldeff_d33_step15_nsbmd,
	NARC_fldeff_d33_step16_nsbmd,
	NARC_fldeff_d33_step17_nsbmd,
	NARC_fldeff_d33_step18_nsbmd,
};
#endif

//--------------------------------------------------------------
///	モデルアーカイブインデックス
//--------------------------------------------------------------
static const u32 DATA_MdlArcIdxTbl[TW_MDL_MAX] =
{
	NARC_fldeff_d33_step01_nsbmd,
	NARC_fldeff_d33_step02_nsbmd,	
	NARC_fldeff_d33_step03a_nsbmd,
	NARC_fldeff_d33_step03b_nsbmd,
	NARC_fldeff_d33_step03c_nsbmd,
	NARC_fldeff_d33_step04_nsbmd,
	NARC_fldeff_d33_step05_nsbmd,
	NARC_fldeff_d33_step06_nsbmd,
	NARC_fldeff_d33_step07_nsbmd,
	NARC_fldeff_d33_step08_nsbmd,
	NARC_fldeff_d33_step09_nsbmd,
	NARC_fldeff_d33_step10_nsbmd,
	NARC_fldeff_d33_step11_nsbmd,
	NARC_fldeff_d33_step12_nsbmd,
	NARC_fldeff_d33_step13_nsbmd,
	NARC_fldeff_d33_step14_nsbmd,
	NARC_fldeff_d33_step15_nsbmd,
	NARC_fldeff_d33_step16_nsbmd,
	NARC_fldeff_d33_step17_nsbmd,
	NARC_fldeff_d33_step18_nsbmd,
	NARC_fldeff_gira_shadow01c_nsbmd,
	NARC_fldeff_d33_waterfall_nsbmd,
	NARC_fldeff_d33_land_tree_nsbmd,
	NARC_fldeff_d33_land_rock_nsbmd,
	NARC_fldeff_yabu_ana_out_nsbmd,
};

//--------------------------------------------------------------
///	アニメアーカイブインデックス
//--------------------------------------------------------------
static const u32 DATA_AnmArcIdxTbl[TW_ANM_MAX] =
{
	NARC_fldeff_gira_shadow01c_nsbta,
	NARC_fldeff_d33_land_tree_nsbca,
	NARC_fldeff_d33_land_rock_nsbca,
	NARC_fldeff_d33_waterfall_nsbta,
	NARC_fldeff_yabu_ana_out_nsbca,
};

//--------------------------------------------------------------
///	登録ID->オブジェ構成
//--------------------------------------------------------------
static const TW_OBJRES_ID DATA_OBJResIDTbl[TWOBJTYPE_MAX] =
{
	{TW_MDL_STEP,TW_ANM_MAX,OBJRES_SHARE},
	{TW_MDL_PUT,TW_ANM_MAX,OBJRES_SHARE},
	{TW_MDL_STEP3A,TW_ANM_MAX,OBJRES_SHARE},
	{TW_MDL_STEP3B,TW_ANM_MAX,OBJRES_SHARE},
	{TW_MDL_STEP3C,TW_ANM_MAX,OBJRES_SHARE},
	{TW_MDL_STEP4,TW_ANM_MAX,OBJRES_SHARE},
	{TW_MDL_STEP5,TW_ANM_MAX,OBJRES_SHARE},
	{TW_MDL_STEP6,TW_ANM_MAX,OBJRES_SHARE},
	{TW_MDL_STEP7,TW_ANM_MAX,OBJRES_SHARE},
	{TW_MDL_STEP8,TW_ANM_MAX,OBJRES_SHARE},
	{TW_MDL_STEP9,TW_ANM_MAX,OBJRES_SHARE},
	{TW_MDL_STEP10,TW_ANM_MAX,OBJRES_SHARE},
	{TW_MDL_STEP11,TW_ANM_MAX,OBJRES_SHARE},
	{TW_MDL_STEP12,TW_ANM_MAX,OBJRES_SHARE},
	{TW_MDL_STEP13,TW_ANM_MAX,OBJRES_SHARE},
	{TW_MDL_STEP14,TW_ANM_MAX,OBJRES_SHARE},
	{TW_MDL_STEP15,TW_ANM_MAX,OBJRES_SHARE},
	{TW_MDL_STEP16,TW_ANM_MAX,OBJRES_SHARE},
	{TW_MDL_STEP17,TW_ANM_MAX,OBJRES_SHARE},
	{TW_MDL_STEP18,TW_ANM_MAX,OBJRES_SHARE},
	{TW_MDL_GIRAS,TW_ANM_GIRAS,OBJRES_ONLY},
	{TW_MDL_WATER_FALL,TW_ANM_WATER_FALL,OBJRES_ONLY},
	{TW_MDL_LAND_TREE,TW_ANM_LAND_TREE,OBJRES_ONLY},
	{TW_MDL_LAND_ROCK,TW_ANM_LAND_ROCK,OBJRES_ONLY},
	{TW_MDL_ANA_OUT,TW_ANM_ANA_OUT,OBJRES_ONLY},
};

//--------------------------------------------------------------
///	飛び石種類別表示オフセット
//--------------------------------------------------------------
static const VecFx32 DATA_OBJTypePosOffsTbl[TWOBJTYPE_MAX] =
{
	{0,-FX32_ONE*25,-FX32_ONE*6},	//step01
	{0,-FX32_ONE*25,-FX32_ONE*6},	//step02
	{0,-FX32_ONE*25,-FX32_ONE*6},	//step03a
	{0,-FX32_ONE*25,-FX32_ONE*6},	//b
	{0,-FX32_ONE*25,-FX32_ONE*6},	//c
	{0,-FX32_ONE*25,-FX32_ONE*6},	//step04
	{0,-FX32_ONE*25,-FX32_ONE*6},	//
	{0,-FX32_ONE*25,-FX32_ONE*6},	//
	{0+(-FX32_ONE*8),-FX32_ONE*25,(-FX32_ONE*6)+(FX32_ONE*8)},	//
	{0,-FX32_ONE*25,-FX32_ONE*6},	//step08
	{0,-FX32_ONE*25,-FX32_ONE*6},	//
	{0,-FX32_ONE*25,-FX32_ONE*6},	//
	{0,-FX32_ONE*25,-FX32_ONE*6},	//
	{0,-FX32_ONE*25,-FX32_ONE*6},	//step12
	{0,-FX32_ONE*25,-FX32_ONE*6},	//
	{0,-FX32_ONE*25,-FX32_ONE*6},	//
	{0+(-FX32_ONE*8),-FX32_ONE*25,(-FX32_ONE*6)+(FX32_ONE*8)},	//
	{0,-FX32_ONE*25,-FX32_ONE*6},	//step16
	{0+(-FX32_ONE*8),-FX32_ONE*25,(-FX32_ONE*6)+(FX32_ONE*16)},	//
	{0,-FX32_ONE*25,-FX32_ONE*6},	//
	{0,-FX32_ONE*32,-FX32_ONE*6},	//giras
	{0,-FX32_ONE*32,-FX32_ONE*6},	//water fall
	{0,-FX32_ONE*32,-FX32_ONE*6},	//land tree
	{0,-FX32_ONE*32,-FX32_ONE*6},	//land rock
//	{0,-FX32_ONE*16,-FX32_ONE*0},	//yabu ana
	{0,-FX32_ONE*14,FX32_ONE*8},	//yabu ana
};

//--------------------------------------------------------------
///	飛び石種類別サイズ
//--------------------------------------------------------------
#define CINGSIZE_X (FX32_ONE+(0x100*4))
#define CINGSIZE_Y (FX32_ONE+(0x100*4))
#define CINGSIZE_Z (FX32_ONE+(0x100*4))
#define CINGSIZE_Z_TREE (FX32_ONE+(0x100*8))

static const VecFx32 DATA_OBJTypeScaleTbl[TWOBJTYPE_MAX] =
{
	{CINGSIZE_X,CINGSIZE_Y,CINGSIZE_Z},	//0 step01
	{CINGSIZE_X,CINGSIZE_Y,CINGSIZE_Z},
	{CINGSIZE_X,CINGSIZE_Y,CINGSIZE_Z},
	{CINGSIZE_X,CINGSIZE_Y,CINGSIZE_Z},
	{CINGSIZE_X,CINGSIZE_Y,CINGSIZE_Z},
	{CINGSIZE_X,CINGSIZE_Y,CINGSIZE_Z},
	{CINGSIZE_X,CINGSIZE_Y,CINGSIZE_Z},
	{CINGSIZE_X,CINGSIZE_Y,CINGSIZE_Z},
	{CINGSIZE_X,CINGSIZE_Y,CINGSIZE_Z},	//8
	{CINGSIZE_X,CINGSIZE_Y,CINGSIZE_Z},
	{CINGSIZE_X,CINGSIZE_Y,CINGSIZE_Z},
	{CINGSIZE_X,CINGSIZE_Y,CINGSIZE_Z},
	{CINGSIZE_X,CINGSIZE_Y,CINGSIZE_Z},
	{CINGSIZE_X,CINGSIZE_Y,CINGSIZE_Z},
	{CINGSIZE_X,CINGSIZE_Y,CINGSIZE_Z},
	{CINGSIZE_X,CINGSIZE_Y,CINGSIZE_Z},
	{CINGSIZE_X,CINGSIZE_Y,CINGSIZE_Z},	//16
	{CINGSIZE_X,CINGSIZE_Y,CINGSIZE_Z},
	{CINGSIZE_X,CINGSIZE_Y,CINGSIZE_Z},
	{CINGSIZE_X,CINGSIZE_Y,CINGSIZE_Z},
	{CINGSIZE_X,CINGSIZE_Y,CINGSIZE_Z},
	{CINGSIZE_X,CINGSIZE_Y,CINGSIZE_Z},
	{CINGSIZE_X,CINGSIZE_Y,CINGSIZE_Z_TREE},
	{0,0,0},
};

//--------------------------------------------------------------
///	EOA	飛び石
//--------------------------------------------------------------
static const EOA_H_NPP DATA_EoaFStoneHeader =
{
	sizeof(EOA_FSTONE_WORK),
	tw_EoaFStoneInit,
	tw_EoaFStoneDelete,
	tw_EoaFStoneMove,
	tw_EoaFStoneDraw,
};

//--------------------------------------------------------------
///	EOA	飛び石　揺れ有り
//--------------------------------------------------------------
static const EOA_H_NPP DATA_EoaFStoneHeaderShake =
{
	sizeof(EOA_FSTONE_WORK),
	tw_EoaFStoneInit,
	tw_EoaFStoneDelete,
	tw_EoaFStoneMoveShake,
	tw_EoaFStoneDraw,
};

//--------------------------------------------------------------
///	EOA	飛び石　木
//--------------------------------------------------------------
static const EOA_H_NPP DATA_EoaFStoneTreeHeader =
{
	sizeof(EOA_FSANM_WORK),
	tw_EoaFStoneTreeInit,
	tw_EoaFStoneTreeDelete,
	tw_EoaFStoneTreeMove,
	tw_EoaFStoneTreeDraw,
};

//--------------------------------------------------------------
///	OBJ種類別ヘッダーテーブル
//--------------------------------------------------------------
static const EOA_H_NPP * const DATA_EoaOBJTypeHeaderTbl[TWOBJTYPE_MAX] =
{
	&DATA_EoaFStoneHeaderShake,	//0
	&DATA_EoaFStoneHeaderShake,
	&DATA_EoaFStoneHeaderShake,
	&DATA_EoaFStoneHeaderShake,
	&DATA_EoaFStoneHeaderShake,
	&DATA_EoaFStoneHeaderShake,
	&DATA_EoaFStoneHeaderShake,
	&DATA_EoaFStoneHeaderShake,
	&DATA_EoaFStoneHeaderShake,
	&DATA_EoaFStoneHeaderShake,
	&DATA_EoaFStoneHeaderShake,	//10
	&DATA_EoaFStoneHeaderShake,
	&DATA_EoaFStoneHeaderShake,
	&DATA_EoaFStoneHeaderShake,
	&DATA_EoaFStoneHeaderShake,
	&DATA_EoaFStoneHeaderShake,
	&DATA_EoaFStoneHeaderShake,
	&DATA_EoaFStoneHeaderShake,
	&DATA_EoaFStoneHeaderShake,
	&DATA_EoaFStoneHeaderShake,
	&DATA_EoaGirasHeader,		//20	ギラティナ影
	&DATA_EoaPutOBJHeader,		//滝
	&DATA_EoaFStoneTreeHeader,
	&DATA_EoaFStoneTreeHeader,
	&DATA_EoaPutOBJHeader,		//yabu ana
};

//==============================================================================
//　data	ゾーン接続
//==============================================================================
//--------------------------------------------------------------
///	ゾーン接続データテーブル
//--------------------------------------------------------------
static const TW_ZONE_LINKDATA DATA_TwZoneLinkDataTbl[TW_ZONECOUNT_MAX] =
{
	{
		ZONE_ID_D34R0101, ZONE_ID_MAX, ZONE_ID_D34R0102,
	},
	{
		ZONE_ID_D34R0102, ZONE_ID_D34R0101, ZONE_ID_D34R0103,
	},
	{
		ZONE_ID_D34R0103, ZONE_ID_D34R0102, ZONE_ID_D34R0104,
	},
	{
		ZONE_ID_D34R0104, ZONE_ID_D34R0103, ZONE_ID_D34R0105,
	},
	{
		ZONE_ID_D34R0105, ZONE_ID_D34R0104, ZONE_ID_D34R0107,
	},
	{
		ZONE_ID_D34R0107, ZONE_ID_D34R0105, ZONE_ID_D34R0108,
	},
	{
		ZONE_ID_D34R0108, ZONE_ID_D34R0107, ZONE_ID_D34R0109,
	},
	{
		ZONE_ID_D34R0109, ZONE_ID_D34R0108, ZONE_ID_MAX,
	},
	{
		ZONE_ID_D34R0111, ZONE_ID_MAX, ZONE_ID_MAX,
	},
	{
		ZONE_ID_D34R0110, ZONE_ID_MAX, ZONE_ID_MAX,
	},
};

#if 0
static const TW_ZONE_LINKDATA DATA_TwZoneLinkDataTbl[TW_ZONECOUNT_MAX] =
{
	{
		ZONE_ID_D34R0101, ZONE_ID_MAX, ZONE_ID_D34R0102,
		21, 32*10, 10
	},
	{
		ZONE_ID_D34R0102, ZONE_ID_D34R0101, ZONE_ID_D34R0103,
		0, 32*9, 35
	},
	{
		ZONE_ID_D34R0103, ZONE_ID_D34R0102, ZONE_ID_D34R0104,
		15, 32*8, 0
	},
	{
		ZONE_ID_D34R0104, ZONE_ID_D34R0103, ZONE_ID_D34R0105,
		47, 32*7, 21
	},
	{
		ZONE_ID_D34R0105, ZONE_ID_D34R0104, ZONE_ID_D34R0106,
		51, 32*6, 50
	},
	{
		ZONE_ID_D34R0106, ZONE_ID_D34R0105, ZONE_ID_D34R0107,
		83, 32*5, 43
	},
	
	{
		ZONE_ID_D34R0107, ZONE_ID_D34R0106, ZONE_ID_D34R0108,
		52, 32*4, 37
	},
	{
		ZONE_ID_D34R0108, ZONE_ID_D34R0107, ZONE_ID_D34R0109,
		52, 32*3, 37
	},
	
	{
		ZONE_ID_D34R0109, ZONE_ID_D34R0108, ZONE_ID_D34R0110,
		0, 32*2, 0
	},
	{
		ZONE_ID_D34R0110, ZONE_ID_D34R0109, ZONE_ID_D34R0111,
		0, 32*1, 0
	},
	{
		ZONE_ID_D34R0111, ZONE_ID_D34R0110, ZONE_ID_MAX,
		52, 32*0, 37
	},
};
#endif

//==============================================================================
//	data link pos
//==============================================================================
//--------------------------------------------------------------
///	１階層　接続位置
//--------------------------------------------------------------
static const LINKPOS_DATA DATA_LinkPosZ0101_0 =		//->2
{
	0,						//id
	40, 289, 54,			//x,y,z
	ZCHG_MID_Z0101_1_2, LPOSMOVE_ZONE_DOWN,	//move no, move type
	0,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP3A,		//draw obj
	ZLINKMFLAG_MAX,
};

static const LINKPOS_DATA * DATA_LinkPosTbl_Z0101[] =
{
	&DATA_LinkPosZ0101_0,
	NULL,
};

//--------------------------------------------------------------
///	２階層　接続位置
//--------------------------------------------------------------
static const LINKPOS_DATA DATA_LinkPosZ0102_0 =		//->1
{
	0,						//id
	40, 257, 54,			//x,y,z
	ZCHG_MID_Z0102_2_1, LPOSMOVE_ZONE_UP,	//move no, move type
	0,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP3A,		//draw obj
	ZLINKMFLAG_0102,
};

static const LINKPOS_DATA DATA_LinkPosZ0102_1 =		//->3
{
	1,						//id
	33, 257, 45,			//x,y,z
	ZCHG_MID_Z0102_2_3, LPOSMOVE_ZONE_DOWN,	//move no, move type
	0,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP3B,		//draw obj
	ZLINKMFLAG_MAX,
};

static const LINKPOS_DATA * DATA_LinkPosTbl_Z0102[] =
{
	&DATA_LinkPosZ0102_0,
	&DATA_LinkPosZ0102_1,
	NULL,
};

//--------------------------------------------------------------
///	３階層　接続位置
//--------------------------------------------------------------
static const LINKPOS_DATA DATA_LinkPosZ0103_0 =		//->2
{
	LPOSID0103_ZUP_3_2,			//id
	33, 225, 45,			//x,y,z
	ZCHG_MID_Z0103_3_2, LPOSMOVE_ZONE_UP,	//move no, move type
	1,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP3B,		//draw obj
	ZLINKMFLAG_0203,
};

#if 0
static const LINKPOS_DATA DATA_LinkPosZ0103_1 =		//->4
{
	LPOSID0103_ZDOWN_3_4,		//id
	65, 225, 36,			//x,y,z
	1, LPOSMOVE_ZONE_DOWN,	//move no, move type
	0,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP3A		//draw obj
};
#endif

static const LINKPOS_DATA DATA_LinkPosZ0103_2 =		//lift 0
{
	LPOSID0103_LIFT00,						//id
	33, 225, 38,			//x,y,z
	0, LPOSMOVE_LIFT,	//move no, move type
	0,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP4,		//draw obj
	ZLINKMFLAG_MAX,
};

static const LINKPOS_DATA DATA_LinkPosZ0103_3 =		//lift 1
{
	LPOSID0103_LIFT01,		//id
	33, 225, 23,			//x,y,z
	0, LPOSMOVE_LIFT,		//move no, move type
	0,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP4,		//draw obj
	ZLINKMFLAG_MAX,
};

static const LINKPOS_DATA DATA_LinkPosZ0103_4 =		//lift 2
{
	LPOSID0103_LIFT02,		//id
	33, 225, 8,			//x,y,z
	0, LPOSMOVE_LIFT,		//move no, move type
	0,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP4,		//draw obj
	ZLINKMFLAG_MAX,
};

static const LINKPOS_DATA DATA_LinkPosZ0103_5 =		//lift 3
{
	LPOSID0103_LIFT03,		//id
	49, 225, 8,			//x,y,z
	0, LPOSMOVE_LIFT,		//move no, move type
	0,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP4,		//draw obj
	ZLINKMFLAG_MAX,
};

static const LINKPOS_DATA DATA_LinkPosZ0103_6 =		//lift 4
{
	LPOSID0103_LIFT04,		//id
	56, 225, 8,			//x,y,z
	0, LPOSMOVE_LIFT,		//move no, move type
	0,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP4,		//draw obj
	ZLINKMFLAG_MAX,
};

static const LINKPOS_DATA DATA_LinkPosZ0103_7 =		//lift 5
{
	LPOSID0103_LIFT05,		//id
	49, 225, 31,			//x,y,z
	0, LPOSMOVE_LIFT,		//move no, move type
	0,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP4,		//draw obj
	ZLINKMFLAG_MAX,
};

static const LINKPOS_DATA DATA_LinkPosZ0103_8 =		//lift 6
{
	LPOSID0103_LIFT06,		//id
	65, 225, 31,			//x,y,z
	ZCHG_MID_Z0103_3_4, LPOSMOVE_ZONE_DOWN,	//move no, move type
	0,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP3C,		//draw obj
	ZLINKMFLAG_MAX,
};

static const LINKPOS_DATA DATA_LinkPosZ0103_9 =		//lift 7
{
	LPOSID0103_LIFT07,		//id
	56, 225, 52,			//x,y,z
	0, LPOSMOVE_LIFT,		//move no, move type
	0,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP4,		//draw obj
	ZLINKMFLAG_MAX,
};

#if 0	//仕掛け簡易化により廃止
static const LINKPOS_DATA DATA_LinkPosZ0103_10 =		//lift 8
{
	LPOSID0103_LIFT08,			//id
	33, 225, 52,			//x,y,z
	0, LPOSMOVE_LIFT,	//move no, move type
	1,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP4,		//draw obj
	ZLINKMFLAG_MAX,
};
#endif

static const LINKPOS_DATA DATA_LinkPosZ0103_200 =		//lift 2-0
{
	LPOSID0103_LIFT200,		//id
	33, 233, 15,			//x,y,z
	0, LPOSMOVE_LIFT,		//move no, move type
	1,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP5,		//draw obj
	ZLINKMFLAG_MAX,
};

static const LINKPOS_DATA DATA_LinkPosZ0103_201 =		//lift 2-1
{
	LPOSID0103_LIFT201,			//id
	49, 233, 15,			//x,y,z
	0, LPOSMOVE_LIFT,	//move no, move type
	1,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP5,		//draw obj
	ZLINKMFLAG_MAX,
};

static const LINKPOS_DATA DATA_LinkPosZ0103_202 =		//lift 2-2
{
	LPOSID0103_LIFT202,			//id
	65, 233, 15,			//x,y,z
	0, LPOSMOVE_LIFT,	//move no, move type
	1,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP5,		//draw obj
	ZLINKMFLAG_MAX,
};

static const LINKPOS_DATA DATA_LinkPosZ0103_203 =		//lift 2-3
{
	LPOSID0103_LIFT203,			//id
	65, 233, 45,			//x,y,z
	0, LPOSMOVE_LIFT,	//move no, move type
	1,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP5,		//draw obj
	ZLINKMFLAG_MAX,
};

static const LINKPOS_DATA DATA_LinkPosZ0103_204 =		//lift 2-4
{
	LPOSID0103_LIFT204,			//id
	65, 233, 52,			//x,y,z
	0, LPOSMOVE_LIFT,	//move no, move type
	1,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP5,		//draw obj
	ZLINKMFLAG_MAX,
};

static const LINKPOS_DATA DATA_LinkPosZ0103_205 =		//lift 2-5
{
	LPOSID0103_LIFT205,			//id
	41, 233, 52,			//x,y,z
	0, LPOSMOVE_LIFT,	//move no, move type
	1,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP5,		//draw obj
	ZLINKMFLAG_MAX,
};

static const LINKPOS_DATA DATA_LinkPosZ0103_206 =		//lift 2-6
{
	LPOSID0103_LIFT206,			//id
	49, 233, 45,			//x,y,z
	0, LPOSMOVE_LIFT,	//move no, move type
	1,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP5,		//draw obj
	ZLINKMFLAG_MAX,
};

#if 0	//仕掛け簡易化により廃止
static const LINKPOS_DATA DATA_LinkPosZ0103_207 =		//lift 2-7
{
	LPOSID0103_LIFT207,			//id
	26, 233, 31,			//x,y,z
	0, LPOSMOVE_LIFT,	//move no, move type
	1,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP5,		//draw obj
	ZLINKMFLAG_MAX,
};

static const LINKPOS_DATA DATA_LinkPosZ0103_208 =		//lift 2-8
{
	LPOSID0103_LIFT208,			//id
	41, 233, 31,			//x,y,z
	0, LPOSMOVE_LIFT,	//move no, move type
	1,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP5,		//draw obj
	ZLINKMFLAG_MAX,
};
#endif

static const LINKPOS_DATA DATA_LinkPosZ0103_209 =		//lift 2-9
{
	LPOSID0103_LIFT209,			//id
	26, 233, 38,			//x,y,z
	0, LPOSMOVE_LIFT,	//move no, move type
	1,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP5,		//draw obj
	ZLINKMFLAG_MAX,
};

static const LINKPOS_DATA DATA_LinkPosZ0103_210 =		//lift 2-10
{
	LPOSID0103_LIFT210,			//id
	49, 233, 38,			//x,y,z
	0, LPOSMOVE_LIFT,	//move no, move type
	1,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP5,		//draw obj
	ZLINKMFLAG_MAX,
};

static const LINKPOS_DATA * DATA_LinkPosTbl_Z0103[] =
{
	&DATA_LinkPosZ0103_0,
	&DATA_LinkPosZ0103_2,
	&DATA_LinkPosZ0103_3,
	&DATA_LinkPosZ0103_4,
	&DATA_LinkPosZ0103_5,
	&DATA_LinkPosZ0103_6,
	&DATA_LinkPosZ0103_7,
	&DATA_LinkPosZ0103_8,
	&DATA_LinkPosZ0103_9,
//	&DATA_LinkPosZ0103_10,	仕掛け簡易化により廃止
	&DATA_LinkPosZ0103_200,
	&DATA_LinkPosZ0103_201,
	&DATA_LinkPosZ0103_202,
	&DATA_LinkPosZ0103_203,
	&DATA_LinkPosZ0103_204,
	&DATA_LinkPosZ0103_205,
	&DATA_LinkPosZ0103_206,
#if 0						//仕掛け簡易化により廃止
	&DATA_LinkPosZ0103_207,
	&DATA_LinkPosZ0103_208,
#endif
	&DATA_LinkPosZ0103_209,
	&DATA_LinkPosZ0103_210,
	NULL,
};

//--------------------------------------------------------------
///	４階層　接続位置
//--------------------------------------------------------------
static const LINKPOS_DATA DATA_LinkPosZ0104_0 =		//->3
{
	LPOSID0104_ZUP_4_3,		//id
	65, 193, 31,			//x,y,z
	ZCHG_MID_Z0104_4_3, LPOSMOVE_ZONE_UP,	//move no, move type
	LPOSID0103_LIFT06,		//ゾーンチェンジ後のID
	TWOBJTYPE_STEP3C,		//draw obj
	ZLINKMFLAG_0304,
};

static const LINKPOS_DATA DATA_LinkPosZ0104_1 =		//4A->5A
{
	LPOSID0104_ZDOWN_4A_5A,						//id
	79, 193, 62,			//x,y,z
	ZCHG_MID_Z0104_4A_5A, LPOSMOVE_ZONE_DOWN,	//move no, move type
	LPOSID0105_ZUP_5A_4A,					//ゾーンチェンジ後のID
	TWOBJTYPE_STEP8,		//draw obj
	ZLINKMFLAG_0504A,
};

static const LINKPOS_DATA DATA_LinkPosZ0104_2 =		//4B->5B
{
	LPOSID0104_ZDOWN_4B_5B,		//id
	95, 193, 70,			//x,y,z
	ZCHG_MID_Z0104_4B_5B, LPOSMOVE_ZONE_DOWN,	//move no, move type
	LPOSID0105_ZUP_5B_4B,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP8,		//draw obj
	ZLINKMFLAG_MAX,
};

static const LINKPOS_DATA DATA_LinkPosZ0104_3 =		//4A->6A
{
	LPOSID0104_ZDOWN_4_6,	//id
	96, 193, 43,			//x,y,z
	ZCHG_MID_Z0104_4_6_0, LPOSMOVE_ZONE_DOWN,	//move no, move type
	2,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP7,		//draw obj
	ZLINKMFLAG_MAX,
};

static const LINKPOS_DATA * DATA_LinkPosTbl_Z0104[] =
{
	&DATA_LinkPosZ0104_0,
	&DATA_LinkPosZ0104_1,
	&DATA_LinkPosZ0104_2,
	&DATA_LinkPosZ0104_3,
	NULL,
};

//--------------------------------------------------------------
///	5階層　接続位置
//--------------------------------------------------------------
static const LINKPOS_DATA DATA_LinkPosZ0105_0 =		//5A->4A
{
	LPOSID0105_ZUP_5A_4A,						//id
	79, 161, 62,			//x,y,z
	ZCHG_MID_Z0105_5A_4A, LPOSMOVE_ZONE_UP,	//move no, move type
	1,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP8,		//draw obj
	ZLINKMFLAG_0405A,
};

static const LINKPOS_DATA DATA_LinkPosZ0105_1 =		//5B->4A
{
	LPOSID0105_ZUP_5B_4B,						//id
	95, 161, 70,			//x,y,z
	ZCHG_MID_Z0105_5B_4B, LPOSMOVE_ZONE_UP,	//move no, move type
	2,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP8,		//draw obj
	ZLINKMFLAG_0405B,
};

static const LINKPOS_DATA DATA_LinkPosZ0105_2 =		//->6
{
	LPOSID0105_ZDOWN_5_6,					//id
	78, 161, 77,			//x,y,z
	ZCHG_MID_Z0105_5_6, LPOSMOVE_ZONE_DOWN,	//move no, move type
	0,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP15,		//draw obj
	ZLINKMFLAG_06B05,
};

static const LINKPOS_DATA * DATA_LinkPosTbl_Z0105[] =
{
	&DATA_LinkPosZ0105_0,
	&DATA_LinkPosZ0105_1,
	&DATA_LinkPosZ0105_2,
	NULL,
};

//--------------------------------------------------------------
///	6階層　接続位置
//--------------------------------------------------------------
static const LINKPOS_DATA DATA_LinkPosZ0107_0 =		//->5
{
	LPOSID0107_ZUP_06_05,					//id
	78, 129, 77,			//x,y,z
	ZCHG_MID_Z0107_6_5, LPOSMOVE_ZONE_UP,	//move no, move type
	2,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP15,		//draw obj
	ZLINKMFLAG_0506B,
};

static const LINKPOS_DATA DATA_LinkPosZ0107_1 =		//->7
{
	LPOSID0107_ZDOWN_06_07,						//id
	87, 129, 67,			//x,y,z
	ZCHG_MID_Z0107_6_7, LPOSMOVE_ZONE_DOWN,	//move no, move type
	0,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP17,		//draw obj
	ZLINKMFLAG_MAX,
};

static const LINKPOS_DATA DATA_LinkPosZ0107_2 =		//->4
{
	LPOSID0107_ZUP_06_04,						//id
	96, 129, 43,			//x,y,z
	ZCHG_MID_Z0107_6_4_0, LPOSMOVE_ZONE_UP,	//move no, move type
	3,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP7,		//draw obj
	ZLINKMFLAG_0406A,
};

static const LINKPOS_DATA * DATA_LinkPosTbl_Z0107[] =
{
	&DATA_LinkPosZ0107_0,
	&DATA_LinkPosZ0107_1,
	&DATA_LinkPosZ0107_2,
	NULL,
};

//--------------------------------------------------------------
///	7階層　接続位置
//--------------------------------------------------------------
static const LINKPOS_DATA DATA_LinkPosZ0108_0 =		//->6
{
	0,						//id
	87, 115, 67,			//x,y,z
	ZCHG_MID_Z0108_7_6, LPOSMOVE_ZONE_UP,	//move no, move type
	1,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP17,		//draw obj
	ZLINKMFLAG_0607,
};

static const LINKPOS_DATA DATA_LinkPosZ0108_1 =		//->8
{
	1,						//id
	85, 115, 86,			//x,y,z
	ZCHG_MID_Z0108_7_8, LPOSMOVE_ZONE_DOWN,	//move no, move type
	0,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP18,		//draw obj
	ZLINKMFLAG_MAX,
};

static const LINKPOS_DATA * DATA_LinkPosTbl_Z0108[] =
{
	&DATA_LinkPosZ0108_0,
	&DATA_LinkPosZ0108_1,
	NULL,
};

//--------------------------------------------------------------
///	8階層A　接続位置
//--------------------------------------------------------------
static const LINKPOS_DATA DATA_LinkPosZ0109_0 =		//->7
{
	0,					//id
	85, 65, 86,			//x,y,z
	ZCHG_MID_Z0109_8_7,LPOSMOVE_ZONE_UP,	//move no, move type
	1,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP18,		//draw obj
	ZLINKMFLAG_0708,
};

#if 0
static const LINKPOS_DATA DATA_LinkPosZ0109_1 =
{
	0,						//id
	35, 225, 47,			//x,y,z
	ZCHG_MID_Z0111_1, LPOSMOVE_ZONE_DOWN,	//move no, move type
	0,						//ゾーンチェンジ後のID
	TWOBJTYPE_STEP3A,		//draw obj
	ZLINKMFLAG_MAX,
};
#endif

static const LINKPOS_DATA * DATA_LinkPosTbl_Z0109[] =
{
	&DATA_LinkPosZ0109_0,
	NULL,
};

//--------------------------------------------------------------
///	ゾーン別リンクテーブル
//--------------------------------------------------------------
static const TW_LINKPOS_ZONEDATA DATA_TwZoneLinkPosTbl[TW_ZONELINK_MAX] =
{
	{ZONE_ID_D34R0101,DATA_LinkPosTbl_Z0101},
	{ZONE_ID_D34R0102,DATA_LinkPosTbl_Z0102},
	{ZONE_ID_D34R0103,DATA_LinkPosTbl_Z0103},
	{ZONE_ID_D34R0104,DATA_LinkPosTbl_Z0104},
	{ZONE_ID_D34R0105,DATA_LinkPosTbl_Z0105},
	{ZONE_ID_D34R0107,DATA_LinkPosTbl_Z0107},
	{ZONE_ID_D34R0108,DATA_LinkPosTbl_Z0108},
	{ZONE_ID_D34R0109,DATA_LinkPosTbl_Z0109},
};

//--------------------------------------------------------------
///	ゾーン変更動作テーブル
//--------------------------------------------------------------
static const LINKPOS_ZONECHGMOVEDATA
	DATA_LinkPosZoneChgMoveTbl[ZCHG_MID_MAX] =
{
 {	//1-2
	 ZCHG_MID_Z0101_1_2,ZCHG_MID_MAX,0,-32,0, 0,-16,0, {0,NUM_FX32(-4),0},
	 ZLINKMFLAG_0102,ZLINKMFLAG_MAX,
 },
 {	//2-1
	 ZCHG_MID_Z0102_2_1,ZCHG_MID_MAX,0,32,0, 0,18,0, {0,NUM_FX32(4),0},
	 ZLINKMFLAG_MAX,ZLINKMFLAG_0102,
 },
 {	//2-3
	 ZCHG_MID_Z0102_2_3,ZCHG_MID_MAX,0,-32,0, 0,-28,0, {0,NUM_FX32(-4),0},
	 ZLINKMFLAG_0203,ZLINKMFLAG_MAX,
 },
 {	//3-2
	 ZCHG_MID_Z0103_3_2,ZCHG_MID_MAX,0,32,0, 0,18,0, {0,NUM_FX32(4),0},
	 ZLINKMFLAG_MAX,ZLINKMFLAG_0203,
 },
 {	//3-4
	 ZCHG_MID_Z0103_3_4,ZCHG_MID_MAX,0,-32,0, 0,-14,0, {0,NUM_FX32(-4),0},
	 ZLINKMFLAG_0304,ZLINKMFLAG_MAX,
 },
 {	//4-3
	 ZCHG_MID_Z0104_4_3,ZCHG_MID_MAX,0,32,0, 0,18,0, {0,NUM_FX32(4),0},
	 ZLINKMFLAG_MAX,ZLINKMFLAG_0304,
 },
 {	//4A-5A
	 ZCHG_MID_Z0104_4A_5A,ZCHG_MID_MAX,0,-32,0, 0,-18,0, {0,NUM_FX32(-4),0},
	 ZLINKMFLAG_0405A,ZLINKMFLAG_0504A,
 },
 {	//4B-5B
	 ZCHG_MID_Z0104_4B_5B,ZCHG_MID_MAX,0,-32,0, 0,-14,0, {0,NUM_FX32(-4),0},
	 ZLINKMFLAG_0405B,ZLINKMFLAG_MAX,
 },
 {	//4->5(->6)
	ZCHG_MID_Z0104_4_6_0,ZCHG_MID_Z0104_4_6_1,0,-32,0, 0,-18,0, {0,NUM_FX32(-4),0},
	ZLINKMFLAG_0406A,ZLINKMFLAG_MAX,
 },
 {	//(4->)5->6
	ZCHG_MID_Z0104_4_6_1,ZCHG_MID_MAX,0,-32,0, 0,-14,0, {0,NUM_FX32(-4),0},
	ZLINKMFLAG_MAX,ZLINKMFLAG_MAX,
 },
 {	//5A->4A
	ZCHG_MID_Z0105_5A_4A,ZCHG_MID_MAX,0,32,0, 0,18,0, {0,NUM_FX32(4),0},
	ZLINKMFLAG_0504A,ZLINKMFLAG_0405A,
 },
 {	//5B->4B
	ZCHG_MID_Z0105_5B_4B,ZCHG_MID_MAX,0,32,0, 0,18,0, {0,NUM_FX32(4),0},
	ZLINKMFLAG_MAX,ZLINKMFLAG_0405B,
 },
 {	//5->6
	ZCHG_MID_Z0105_5_6,ZCHG_MID_MAX,0,-32,0, 0,-14,0, {0,NUM_FX32(-4),0},
	ZLINKMFLAG_0506B,ZLINKMFLAG_06B05,
 },
 {	//6->5
	ZCHG_MID_Z0107_6_5,ZCHG_MID_MAX,0,32,0, 0,18,0, {0,NUM_FX32(4),0},
	ZLINKMFLAG_06B05,ZLINKMFLAG_0506B,
 },
 {	//6->7
	ZCHG_MID_Z0107_6_7,ZCHG_MID_MAX,0,-14,0, 0,-12,0, {0,NUM_FX32(-4),0},
	ZLINKMFLAG_0607,ZLINKMFLAG_MAX,
 },
 {	//6->5(->4)
	ZCHG_MID_Z0107_6_4_0,ZCHG_MID_Z0107_6_4_1,0,32,0, 0,18,0, {0,NUM_FX32(4),0},
	ZLINKMFLAG_MAX,ZLINKMFLAG_0406A,
 },
 {	//(6->)5->4
	ZCHG_MID_Z0107_6_4_1,ZCHG_MID_MAX,0,32,0, 0,10,0, {0,NUM_FX32(4),0},
	ZLINKMFLAG_MAX,ZLINKMFLAG_MAX,
 },
 {	//7->6
	ZCHG_MID_Z0108_7_6,ZCHG_MID_MAX,0,14,0, 0,1,0, {0,NUM_FX32(4),0},
	ZLINKMFLAG_MAX,ZLINKMFLAG_0607,
 },
 {	//7->8
	ZCHG_MID_Z0108_7_8,ZCHG_MID_MAX,0,-50,0, 0,-17,0, {0,NUM_FX32(-4),0},
	ZLINKMFLAG_0708,ZLINKMFLAG_MAX,
 },
 {	//8->7
	ZCHG_MID_Z0109_8_7,ZCHG_MID_MAX,0,50,0, 0,18,0, {0,NUM_FX32(4),0},
	ZLINKMFLAG_MAX,ZLINKMFLAG_0708,
 },
 {	//not use
	ZCHG_MID_Z0111_0,ZCHG_MID_MAX,0,32,0, 0,18,0, {0,NUM_FX32(4),0},
	ZLINKMFLAG_MAX,ZLINKMFLAG_MAX,
 },
 {	//not use
	ZCHG_MID_Z0111_1,ZCHG_MID_MAX,0,-32,0, 0,-14,0, {0,NUM_FX32(4),0},
	ZLINKMFLAG_MAX,ZLINKMFLAG_MAX,
 },
};

//==============================================================================
///	スクリプト
//==============================================================================
//--------------------------------------------------------------
///	スクリプト処理テーブル
//--------------------------------------------------------------
static const TW_SCR_PROC * DATA_ScrCmdProcTbl[TWSCRCODE_MAX] =
{
	DATA_ScrTbl_Acmd,			//TWSCRCODE_ACMD
	DATA_ScrTbl_Lift,			//TWSCRCODE_LIFT
	DATA_ScrTbl_FldOBJAdd,		//TWSCRCODE_FLDOBJADD,
	DATA_ScrTbl_FldOBJDelete,	//TWSCRCODE_FLDOBJDEL
	DATA_ScrTbl_TakiNobori,		//TWSCRCODE_TAKINOBORI
	DATA_ScrTbl_ScriptCall,		//TWSCRCODE_SCRCALL
	DATA_ScrTbl_TwEvSeqSet,		//TWSCRCODE_EVSEQSET
	DATA_ScrTbl_GirasSet,		//TWSCRCODE_GIRAS
	DATA_ScrTbl_GirasFlagSet,	//TWSCRCODE_GIRASFLAGSET
	DATA_ScrTbl_TwEvFlagSet,	//TWSCRCODE_TWEVFLAG_SET
	DATA_ScrTbl_TakiKudari,		//TWSCRCODE_TAKIKUDARI
	DATA_ScrTbl_GiraAppear,		//TWSCRCODE_GIRA_APPEAR
	DATA_ScrTbl_RockPokeYukushi,//TWSCRCODE_ROCKPOKE_YUKUSHI
	DATA_ScrTbl_RockPokeAgunomu,//TWSCRCODE_ROCKPOKE_AGUNOMU
	DATA_ScrTbl_RockPokeEmurit,	//TWSCRCODE_ROCKPOKE_EMURIT
	DATA_ScrTbl_Stone0110_ON,	//TWSCRCODE_STONE0110_ON
	DATA_ScrTbl_Stone0110_OFF,	//TWSCRCODE_STONE0110_OFF
	DATA_ScrTbl_TwEvFlagOFF,	//TWSCRCODE_TWEVFLAG_OFF
};

//==============================================================================
///	座標スクリプト　0101
//==============================================================================
//--------------------------------------------------------------
///	座標スクリプト　シロナ会話
//--------------------------------------------------------------
static const SCRDATA_SCRCALL DataScr0101_00 =
{SCRID_D34R0101_SIRONA01};
static const SCRDATA_EVSEQ DataScr0101_01 =
{TWEVSEQ_01_02};
static const TW_SCRDATA DATA_Scr0101_00[] =
{
	{TWSCRCODE_SCRCALL,&DataScr0101_00},
	{TWSCRCODE_EVSEQSET,&DataScr0101_01},
	{TWSCRCODE_MAX,NULL},
};

static const SCRPOSDATA DATA_ScrPos0101[] =
{
	{40,289,52,TWFLAGTYPE_TWEVSEQ_EQ,TWEVSEQ_01_01,DATA_Scr0101_00},
	{0,0,0,0,NULL},
};

//==============================================================================
//	座標スクリプト	0102
//==============================================================================
//--------------------------------------------------------------
///	座標スクリプト　シロナ移動
//--------------------------------------------------------------
static const SCRDATA_SCRCALL DataScr0102_00 =
{SCRID_D34R0102_MES01};							//アグノム呼び出し
static const SCRDATA_EVSEQ DataScr0102_01 =
{TWEVSEQ_03_00};
static const SCRDATA_FLDOBJADD DataScr0102_02 =	//3層にシロナを追加
{ZONE_ID_D34R0103,TW_OBJID_0103_SIRONA};
static const TW_SCRDATA DATA_Scr0102_00[] =
{
	{TWSCRCODE_SCRCALL,&DataScr0102_00},
	{TWSCRCODE_EVSEQSET,&DataScr0102_01},
	{TWSCRCODE_FLDOBJADD,&DataScr0102_02},
	{TWSCRCODE_MAX,NULL},
};

static const SCRPOSDATA DATA_ScrPos0102[] =
{
	{15,257,58,TWFLAGTYPE_TWEVSEQ_EQ,TWEVSEQ_02_00,DATA_Scr0102_00},
	{0,0,0,0,NULL},
};

//==============================================================================
//	座標スクリプト 0103
//==============================================================================
//--------------------------------------------------------------
///	スクリプト　リフト移動その00 0-1-0
//--------------------------------------------------------------
static const SCRDATA_LIFTMOVE DataScr0103_00 =
{LPOSID0103_LIFT00,TRUE,HEROTWTYPE_ON,0,0,-8,{0,0,NUM_FX32(-4)}};
static const SCRDATA_ACMD DataScr0103_01 =
{FLDOBJ_ID_PLAYER,AC_JUMP_U_3G_24F};
static const SCRDATA_LIFTMOVE DataScr0103_02 =
{LPOSID0103_LIFT00,FALSE,HEROTWTYPE_NON,0,0,8,{0,0,NUM_FX32(4)}};
static const TW_SCRDATA DATA_Scr0103_Lift00[] =
{
	{TWSCRCODE_LIFT,&DataScr0103_00},
	{TWSCRCODE_ACMD,&DataScr0103_01},
	{TWSCRCODE_LIFT,&DataScr0103_02},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	スクリプト　リフト移動その01 1-0-0
//--------------------------------------------------------------
static const SCRDATA_LIFTMOVE DataScr0103_03 =
{LPOSID0103_LIFT00,FALSE,HEROTWTYPE_NON,0,0,-8,{0,0,NUM_FX32(-4)}};
static const SCRDATA_ACMD DataScr0103_04 =
{FLDOBJ_ID_PLAYER,AC_JUMP_D_3G_24F};
static const SCRDATA_LIFTMOVE DataScr0103_05 =
{LPOSID0103_LIFT00,TRUE,HEROTWTYPE_ON,0,0,8,{0,0,NUM_FX32(4)}};
static const TW_SCRDATA DATA_Scr0103_Lift01[] =
{
	{TWSCRCODE_LIFT,&DataScr0103_03},
	{TWSCRCODE_ACMD,&DataScr0103_04},
	{TWSCRCODE_LIFT,&DataScr0103_05},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	スクリプト　リフト移動その02 1-2-1
//--------------------------------------------------------------
static const SCRDATA_LIFTMOVE DataScr0103_06 =
{LPOSID0103_LIFT01,TRUE,HEROTWTYPE_ON,0,0,-8,{0,0,NUM_FX32(-4)}};
static const SCRDATA_ACMD DataScr0103_07 =
{FLDOBJ_ID_PLAYER,AC_JUMP_U_3G_24F};
static const SCRDATA_LIFTMOVE DataScr0103_08 =
{LPOSID0103_LIFT01,FALSE,HEROTWTYPE_NON,0,0,8,{0,0,NUM_FX32(4)}};
static const TW_SCRDATA DATA_Scr0103_Lift02[] =
{
	{TWSCRCODE_LIFT,&DataScr0103_06},
	{TWSCRCODE_ACMD,&DataScr0103_07},
	{TWSCRCODE_LIFT,&DataScr0103_08},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	スクリプト　リフト移動その03 2-1-1
//--------------------------------------------------------------
static const SCRDATA_LIFTMOVE DataScr0103_09 =
{LPOSID0103_LIFT01,FALSE,HEROTWTYPE_NON,0,0,-8,{0,0,NUM_FX32(-4)}};
static const SCRDATA_ACMD DataScr0103_10 =
{FLDOBJ_ID_PLAYER,AC_JUMP_D_3G_24F};
static const SCRDATA_LIFTMOVE DataScr0103_11 =
{LPOSID0103_LIFT01,TRUE,HEROTWTYPE_ON,0,0,8,{0,0,NUM_FX32(4)}};
static const TW_SCRDATA DATA_Scr0103_Lift03[] =
{
	{TWSCRCODE_LIFT,&DataScr0103_09},
	{TWSCRCODE_ACMD,&DataScr0103_10},
	{TWSCRCODE_LIFT,&DataScr0103_11},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	スクリプト　リフト移動その04 2-3-2
//--------------------------------------------------------------
static const SCRDATA_LIFTMOVE DataScr0103_12 =
{LPOSID0103_LIFT02,TRUE,HEROTWTYPE_ON,9,0,0,{NUM_FX32(4),0,0}};
static const SCRDATA_ACMD DataScr0103_13 =
{FLDOBJ_ID_PLAYER,AC_JUMP_R_3G_24F};
static const SCRDATA_LIFTMOVE DataScr0103_14 =
{LPOSID0103_LIFT02,FALSE,HEROTWTYPE_NON,-9,0,0,{NUM_FX32(-4),0,0}};
static const TW_SCRDATA DATA_Scr0103_Lift04[] =
{
	{TWSCRCODE_LIFT,&DataScr0103_12},
	{TWSCRCODE_ACMD,&DataScr0103_13},
	{TWSCRCODE_LIFT,&DataScr0103_14},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	スクリプト　リフト移動その05 3-2-2
//--------------------------------------------------------------
static const SCRDATA_LIFTMOVE DataScr0103_15 =
{LPOSID0103_LIFT02,FALSE,HEROTWTYPE_NON,9,0,0,{NUM_FX32(4),0,0}};
static const SCRDATA_ACMD DataScr0103_16 =
{FLDOBJ_ID_PLAYER,AC_JUMP_L_3G_24F};
static const SCRDATA_LIFTMOVE DataScr0103_17 =
{LPOSID0103_LIFT02,TRUE,HEROTWTYPE_ON,-9,0,0,{NUM_FX32(-4),0,0}};
static const TW_SCRDATA DATA_Scr0103_Lift05[] =
{
	{TWSCRCODE_LIFT,&DataScr0103_15},
	{TWSCRCODE_ACMD,&DataScr0103_16},
	{TWSCRCODE_LIFT,&DataScr0103_17},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	スクリプト　リフト移動その06 4-7-4
//--------------------------------------------------------------
static const SCRDATA_LIFTMOVE DataScr0103_18 =
{LPOSID0103_LIFT04,TRUE,HEROTWTYPE_ON,0,0,37,{0,0,NUM_FX32(4)}};
static const SCRDATA_ACMD DataScr0103_19 =
{FLDOBJ_ID_PLAYER,AC_JUMP_D_3G_24F};
static const SCRDATA_LIFTMOVE DataScr0103_20 =
{LPOSID0103_LIFT04,FALSE,HEROTWTYPE_NON,0,0,-37,{0,0,NUM_FX32(-4)}};
static const TW_SCRDATA DATA_Scr0103_Lift06[] =
{
	{TWSCRCODE_LIFT,&DataScr0103_18},
	{TWSCRCODE_ACMD,&DataScr0103_19},
	{TWSCRCODE_LIFT,&DataScr0103_20},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	スクリプト　リフト移動その07 7-4-4
//--------------------------------------------------------------
static const SCRDATA_LIFTMOVE DataScr0103_21 =
{LPOSID0103_LIFT04,FALSE,HEROTWTYPE_NON,0,0,37,{0,0,NUM_FX32(4)}};
static const SCRDATA_ACMD DataScr0103_22 =
{FLDOBJ_ID_PLAYER,AC_JUMP_U_3G_24F};
static const SCRDATA_LIFTMOVE DataScr0103_23 =
{LPOSID0103_LIFT04,TRUE,HEROTWTYPE_ON,0,0,-37,{0,0,NUM_FX32(-4)}};
static const TW_SCRDATA DATA_Scr0103_Lift07[] =
{
	{TWSCRCODE_LIFT,&DataScr0103_21},
	{TWSCRCODE_ACMD,&DataScr0103_22},
	{TWSCRCODE_LIFT,&DataScr0103_23},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	スクリプト　リフト移動その08 5-6-5
//--------------------------------------------------------------
static const SCRDATA_LIFTMOVE DataScr0103_0800 =
{LPOSID0103_LIFT05,TRUE,HEROTWTYPE_ON,9,0,0,{NUM_FX32(4),0,0}};
static const SCRDATA_ACMD DataScr0103_0801 =
{FLDOBJ_ID_PLAYER,AC_JUMP_R_3G_24F};
static const SCRDATA_LIFTMOVE DataScr0103_0802 =
{LPOSID0103_LIFT05,FALSE,HEROTWTYPE_NON,-9,0,0,{NUM_FX32(-4),0,0}};
static const TW_SCRDATA DATA_Scr0103_Lift08[] =
{
	{TWSCRCODE_LIFT,&DataScr0103_0800},
	{TWSCRCODE_ACMD,&DataScr0103_0801},
	{TWSCRCODE_LIFT,&DataScr0103_0802},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	スクリプト　リフト移動その09 6-5-5
//--------------------------------------------------------------
static const SCRDATA_LIFTMOVE DataScr0103_0901 =
{LPOSID0103_LIFT05,FALSE,HEROTWTYPE_NON,9,0,0,{NUM_FX32(4),0,0}};
static const SCRDATA_ACMD DataScr0103_0902 =
{FLDOBJ_ID_PLAYER,AC_JUMP_L_3G_24F};
static const SCRDATA_LIFTMOVE DataScr0103_0903 =
{LPOSID0103_LIFT05,TRUE,HEROTWTYPE_ON,-9,0,0,{NUM_FX32(-4),0,0}};
static const TW_SCRDATA DATA_Scr0103_Lift09[] =
{
	{TWSCRCODE_LIFT,&DataScr0103_0901},
	{TWSCRCODE_ACMD,&DataScr0103_0902},
	{TWSCRCODE_LIFT,&DataScr0103_0903},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	スクリプト　リフト移動その10 7-8-7
//--------------------------------------------------------------
#if 0	//仕掛け簡易化により廃止
static const SCRDATA_LIFTMOVE DataScr0103_1000 =
{LPOSID0103_LIFT07,TRUE,HEROTWTYPE_ON,-16,0,0,{NUM_FX32(-4),0,0}};
static const SCRDATA_ACMD DataScr0103_1001 =
{FLDOBJ_ID_PLAYER,AC_JUMP_L_3G_24F};
static const SCRDATA_LIFTMOVE DataScr0103_1002 =
{LPOSID0103_LIFT07,FALSE,HEROTWTYPE_NON,16,0,0,{NUM_FX32(4),0,0}};
static const TW_SCRDATA DATA_Scr0103_Lift10[] =
{
	{TWSCRCODE_LIFT,&DataScr0103_1000},
	{TWSCRCODE_ACMD,&DataScr0103_1001},
	{TWSCRCODE_LIFT,&DataScr0103_1002},
	{TWSCRCODE_MAX,NULL},
};
#endif

//--------------------------------------------------------------
///	スクリプト　リフト移動その11 8-7-7
//--------------------------------------------------------------
#if 0	//仕掛け簡易化により廃止
static const SCRDATA_LIFTMOVE DataScr0103_1100 =
{LPOSID0103_LIFT07,FALSE,HEROTWTYPE_NON,-16,0,0,{NUM_FX32(-4),0,0}};
static const SCRDATA_ACMD DataScr0103_1101 =
{FLDOBJ_ID_PLAYER,AC_JUMP_R_3G_24F};
static const SCRDATA_LIFTMOVE DataScr0103_1102 =
{LPOSID0103_LIFT07,TRUE,HEROTWTYPE_ON,16,0,0,{NUM_FX32(4),0,0}};
static const TW_SCRDATA DATA_Scr0103_Lift11[] =
{
	{TWSCRCODE_LIFT,&DataScr0103_1100},
	{TWSCRCODE_ACMD,&DataScr0103_1101},
	{TWSCRCODE_LIFT,&DataScr0103_1102},
	{TWSCRCODE_MAX,NULL},
};
#endif

//--------------------------------------------------------------
///	スクリプト　リフト移動その2-0 0-1-0
//--------------------------------------------------------------
static const SCRDATA_LIFTMOVE DataScr0103_2_0_0 =
{LPOSID0103_LIFT200,TRUE,HEROTWTYPE_GROUND,9,0,0,{NUM_FX32(4),0,0}};
static const SCRDATA_ACMD DataScr0103_2_0_1 =
{FLDOBJ_ID_PLAYER,AC_JUMP_R_3G_24F};
static const SCRDATA_LIFTMOVE DataScr0103_2_0_2 =
{LPOSID0103_LIFT200,FALSE,HEROTWTYPE_GROUND,-9,0,0,{NUM_FX32(-4),0,0}};
static const TW_SCRDATA Data_Scr0103_2_Lift00[] =
{
	{TWSCRCODE_LIFT,&DataScr0103_2_0_0},
	{TWSCRCODE_ACMD,&DataScr0103_2_0_1},
	{TWSCRCODE_LIFT,&DataScr0103_2_0_2},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	スクリプト　リフト移動その2-1 1-0-0
//--------------------------------------------------------------
static const SCRDATA_LIFTMOVE DataScr0103_2_1_0 =
{LPOSID0103_LIFT200,FALSE,HEROTWTYPE_GROUND,9,0,0,{NUM_FX32(4),0,0}};
static const SCRDATA_ACMD DataScr0103_2_1_1 =
{FLDOBJ_ID_PLAYER,AC_JUMP_L_3G_24F};
static const SCRDATA_LIFTMOVE DataScr0103_2_1_2 =
{LPOSID0103_LIFT200,TRUE,HEROTWTYPE_GROUND,-9,0,0,{NUM_FX32(-4),0,0}};
static const TW_SCRDATA Data_Scr0103_2_Lift01[] =
{
	{TWSCRCODE_LIFT,&DataScr0103_2_1_0},
	{TWSCRCODE_ACMD,&DataScr0103_2_1_1},
	{TWSCRCODE_LIFT,&DataScr0103_2_1_2},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	スクリプト　リフト移動その2-2 2-3-2
//--------------------------------------------------------------
static const SCRDATA_LIFTMOVE DataScr0103_2_2_0 =
{LPOSID0103_LIFT201,TRUE,HEROTWTYPE_GROUND,9,0,0,{NUM_FX32(4),0,0}};
static const SCRDATA_ACMD DataScr0103_2_2_1 =
{FLDOBJ_ID_PLAYER,AC_JUMP_R_3G_24F};
static const SCRDATA_LIFTMOVE DataScr0103_2_2_2 =
{LPOSID0103_LIFT201,FALSE,HEROTWTYPE_GROUND,-9,0,0,{NUM_FX32(-4),0,0}};
static const TW_SCRDATA Data_Scr0103_2_Lift02[] =
{
	{TWSCRCODE_LIFT,&DataScr0103_2_2_0},
	{TWSCRCODE_ACMD,&DataScr0103_2_2_1},
	{TWSCRCODE_LIFT,&DataScr0103_2_2_2},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	スクリプト　リフト移動その2-3 3-2-2
//--------------------------------------------------------------
static const SCRDATA_LIFTMOVE DataScr0103_2_3_0 =
{LPOSID0103_LIFT201,FALSE,HEROTWTYPE_GROUND,9,0,0,{NUM_FX32(4),0,0}};
static const SCRDATA_ACMD DataScr0103_2_3_1 =
{FLDOBJ_ID_PLAYER,AC_JUMP_L_3G_24F};
static const SCRDATA_LIFTMOVE DataScr0103_2_3_2 =
{LPOSID0103_LIFT201,TRUE,HEROTWTYPE_GROUND,-9,0,0,{NUM_FX32(-4),0,0}};
static const TW_SCRDATA Data_Scr0103_2_Lift03[] =
{
	{TWSCRCODE_LIFT,&DataScr0103_2_3_0},
	{TWSCRCODE_ACMD,&DataScr0103_2_3_1},
	{TWSCRCODE_LIFT,&DataScr0103_2_3_2},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	スクリプト　リフト移動その2-4 3-4-3
//--------------------------------------------------------------
static const SCRDATA_LIFTMOVE DataScr0103_2_4_0 =
{LPOSID0103_LIFT202,TRUE,HEROTWTYPE_GROUND,0,0,23,{0,0,NUM_FX32(4)}};
static const SCRDATA_ACMD DataScr0103_2_4_1 =
{FLDOBJ_ID_PLAYER,AC_JUMP_D_3G_24F};
static const SCRDATA_LIFTMOVE DataScr0103_2_4_2 =
{LPOSID0103_LIFT202,FALSE,HEROTWTYPE_GROUND,0,0,-23,{0,0,NUM_FX32(-4)}};
static const TW_SCRDATA Data_Scr0103_2_Lift04[] =
{
	{TWSCRCODE_LIFT,&DataScr0103_2_4_0},
	{TWSCRCODE_ACMD,&DataScr0103_2_4_1},
	{TWSCRCODE_LIFT,&DataScr0103_2_4_2},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	スクリプト　リフト移動その2-5 4-3-3
//--------------------------------------------------------------
static const SCRDATA_LIFTMOVE DataScr0103_2_5_0 =
{LPOSID0103_LIFT202,FALSE,HEROTWTYPE_GROUND,0,0,23,{0,0,NUM_FX32(4)}};
static const SCRDATA_ACMD DataScr0103_2_5_1 =
{FLDOBJ_ID_PLAYER,AC_JUMP_U_3G_24F};
static const SCRDATA_LIFTMOVE DataScr0103_2_5_2 =
{LPOSID0103_LIFT202,TRUE,HEROTWTYPE_GROUND,0,0,-23,{0,0,NUM_FX32(-4)}};
static const TW_SCRDATA Data_Scr0103_2_Lift05[] =
{
	{TWSCRCODE_LIFT,&DataScr0103_2_5_0},
	{TWSCRCODE_ACMD,&DataScr0103_2_5_1},
	{TWSCRCODE_LIFT,&DataScr0103_2_5_2},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	スクリプト　リフト移動その2-6 4-7-4
//--------------------------------------------------------------
static const SCRDATA_LIFTMOVE DataScr0103_2_6_0 =
{LPOSID0103_LIFT203,TRUE,HEROTWTYPE_GROUND,-9,0,0,{NUM_FX32(-4),0,0}};
static const SCRDATA_ACMD DataScr0103_2_6_1 =
{FLDOBJ_ID_PLAYER,AC_JUMP_L_3G_24F};
static const SCRDATA_LIFTMOVE DataScr0103_2_6_2 =
{LPOSID0103_LIFT203,FALSE,HEROTWTYPE_GROUND,9,0,0,{NUM_FX32(4),0,0}};
static const TW_SCRDATA Data_Scr0103_2_Lift06[] =
{
	{TWSCRCODE_LIFT,&DataScr0103_2_6_0},
	{TWSCRCODE_ACMD,&DataScr0103_2_6_1},
	{TWSCRCODE_LIFT,&DataScr0103_2_6_2},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	スクリプト　リフト移動その2-7 7-4-4
//--------------------------------------------------------------
static const SCRDATA_LIFTMOVE DataScr0103_2_7_0 =
{LPOSID0103_LIFT203,FALSE,HEROTWTYPE_GROUND,-9,0,0,{NUM_FX32(-4),0,0}};
static const SCRDATA_ACMD DataScr0103_2_7_1 =
{FLDOBJ_ID_PLAYER,AC_JUMP_R_3G_24F};
static const SCRDATA_LIFTMOVE DataScr0103_2_7_2 =
{LPOSID0103_LIFT203,TRUE,HEROTWTYPE_GROUND,9,0,0,{NUM_FX32(4),0,0}};
static const TW_SCRDATA Data_Scr0103_2_Lift07[] =
{
	{TWSCRCODE_LIFT,&DataScr0103_2_7_0},
	{TWSCRCODE_ACMD,&DataScr0103_2_7_1},
	{TWSCRCODE_LIFT,&DataScr0103_2_7_2},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	スクリプト　リフト移動その2-8 5-6-5
//--------------------------------------------------------------
static const SCRDATA_LIFTMOVE DataScr0103_2_8_0 =
{LPOSID0103_LIFT204,TRUE,HEROTWTYPE_GROUND,-17,0,0,{NUM_FX32(-4),0,0}};
static const SCRDATA_ACMD DataScr0103_2_8_1 =
{FLDOBJ_ID_PLAYER,AC_JUMP_L_3G_24F};
static const SCRDATA_LIFTMOVE DataScr0103_2_8_2 =
{LPOSID0103_LIFT204,FALSE,HEROTWTYPE_GROUND,17,0,0,{NUM_FX32(4),0,0}};
static const TW_SCRDATA Data_Scr0103_2_Lift08[] =
{
	{TWSCRCODE_LIFT,&DataScr0103_2_8_0},
	{TWSCRCODE_ACMD,&DataScr0103_2_8_1},
	{TWSCRCODE_LIFT,&DataScr0103_2_8_2},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	スクリプト　リフト移動その2-9 6-5-5
//--------------------------------------------------------------
static const SCRDATA_LIFTMOVE DataScr0103_2_9_0 =
{LPOSID0103_LIFT204,FALSE,HEROTWTYPE_GROUND,-17,0,0,{NUM_FX32(-4),0,0}};
static const SCRDATA_ACMD DataScr0103_2_9_1 =
{FLDOBJ_ID_PLAYER,AC_JUMP_R_3G_24F};
static const SCRDATA_LIFTMOVE DataScr0103_2_9_2 =
{LPOSID0103_LIFT204,TRUE,HEROTWTYPE_GROUND,17,0,0,{NUM_FX32(4),0,0}};
static const TW_SCRDATA Data_Scr0103_2_Lift09[] =
{
	{TWSCRCODE_LIFT,&DataScr0103_2_9_0},
	{TWSCRCODE_ACMD,&DataScr0103_2_9_1},
	{TWSCRCODE_LIFT,&DataScr0103_2_9_2},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	スクリプト　リフト移動その2-10 6-9-6
//--------------------------------------------------------------
#if 0	//仕掛け簡易化により廃止
static const SCRDATA_LIFTMOVE DataScr0103_2_10_0 =
{LPOSID0103_LIFT205,TRUE,HEROTWTYPE_GROUND,0,0,-14,{0,0,NUM_FX32(-4)}};
static const SCRDATA_ACMD DataScr0103_2_10_1 =
{FLDOBJ_ID_PLAYER,AC_JUMP_U_3G_24F};
static const SCRDATA_LIFTMOVE DataScr0103_2_10_2 =
{LPOSID0103_LIFT205,FALSE,HEROTWTYPE_GROUND,0,0,14,{0,0,NUM_FX32(4)}};
static const TW_SCRDATA Data_Scr0103_2_Lift10[] =
{
	{TWSCRCODE_LIFT,&DataScr0103_2_10_0},
	{TWSCRCODE_ACMD,&DataScr0103_2_10_1},
	{TWSCRCODE_LIFT,&DataScr0103_2_10_2},
	{TWSCRCODE_MAX,NULL},
};
#endif

//--------------------------------------------------------------
///	スクリプト　リフト移動その2-11 9-6-6
//--------------------------------------------------------------
#if 0	//仕掛け簡易化により廃止
static const SCRDATA_LIFTMOVE DataScr0103_2_11_0 =
{LPOSID0103_LIFT205,FALSE,HEROTWTYPE_GROUND,0,0,-14,{0,0,NUM_FX32(-4)}};
static const SCRDATA_ACMD DataScr0103_2_11_1 =
{FLDOBJ_ID_PLAYER,AC_JUMP_D_3G_24F};
static const SCRDATA_LIFTMOVE DataScr0103_2_11_2 =
{LPOSID0103_LIFT205,TRUE,HEROTWTYPE_GROUND,0,0,14,{0,0,NUM_FX32(4)}};
static const TW_SCRDATA Data_Scr0103_2_Lift11[] =
{
	{TWSCRCODE_LIFT,&DataScr0103_2_11_0},
	{TWSCRCODE_ACMD,&DataScr0103_2_11_1},
	{TWSCRCODE_LIFT,&DataScr0103_2_11_2},
	{TWSCRCODE_MAX,NULL},
};
#endif

//--------------------------------------------------------------
///	スクリプト　リフト移動その2-12 8-9-8
//--------------------------------------------------------------
#if 0	//仕掛け簡易化により廃止
static const SCRDATA_LIFTMOVE DataScr0103_2_12_0 =
{LPOSID0103_LIFT207,TRUE,HEROTWTYPE_GROUND,8,0,0,{NUM_FX32(4),0,0}};
static const SCRDATA_ACMD DataScr0103_2_12_1 =
{FLDOBJ_ID_PLAYER,AC_JUMP_R_3G_24F};
static const SCRDATA_LIFTMOVE DataScr0103_2_12_2 =
{LPOSID0103_LIFT207,FALSE,HEROTWTYPE_GROUND,-8,0,0,{NUM_FX32(-4),0,0}};
static const TW_SCRDATA Data_Scr0103_2_Lift12[] =
{
	{TWSCRCODE_LIFT,&DataScr0103_2_12_0},
	{TWSCRCODE_ACMD,&DataScr0103_2_12_1},
	{TWSCRCODE_LIFT,&DataScr0103_2_12_2},
	{TWSCRCODE_MAX,NULL},
};
#endif

//--------------------------------------------------------------
///	スクリプト　リフト移動その2-13 9-8-8
//--------------------------------------------------------------
#if 0	//仕掛け簡易化により廃止
static const SCRDATA_LIFTMOVE DataScr0103_2_13_0 =
{LPOSID0103_LIFT207,FALSE,HEROTWTYPE_GROUND,8,0,0,{NUM_FX32(4),0,0}};
static const SCRDATA_ACMD DataScr0103_2_13_1 =
{FLDOBJ_ID_PLAYER,AC_JUMP_L_3G_24F};
static const SCRDATA_LIFTMOVE DataScr0103_2_13_2 =
{LPOSID0103_LIFT207,TRUE,HEROTWTYPE_GROUND,-8,0,0,{NUM_FX32(-4),0,0}};
static const TW_SCRDATA Data_Scr0103_2_Lift13[] =
{
	{TWSCRCODE_LIFT,&DataScr0103_2_13_0},
	{TWSCRCODE_ACMD,&DataScr0103_2_13_1},
	{TWSCRCODE_LIFT,&DataScr0103_2_13_2},
	{TWSCRCODE_MAX,NULL},
};
#endif

//--------------------------------------------------------------
///	スクリプト　リフト移動その2-14 11下降->1階->11上昇
//--------------------------------------------------------------
static const SCRDATA_LIFTMOVE DataScr0103_2_14_0 =
{LPOSID0103_LIFT210,TRUE,HEROTWTYPE_ON,0,-8,0,{0,NUM_FX32(-2),0}};
static const SCRDATA_ACMD DataScr0103_2_14_1 =
{FLDOBJ_ID_PLAYER,AC_JUMP_U_3G_24F};
static const SCRDATA_LIFTMOVE DataScr0103_2_14_2 =
{LPOSID0103_LIFT210,FALSE,HEROTWTYPE_ON,0,8,0,{0,NUM_FX32(2),0}};

static const TW_SCRDATA Data_Scr0103_2_Lift14[] =
{
	{TWSCRCODE_LIFT,&DataScr0103_2_14_0},
	{TWSCRCODE_ACMD,&DataScr0103_2_14_1},
	{TWSCRCODE_LIFT,&DataScr0103_2_14_2},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	スクリプト　リフト移動その2-15 1階->11下降->11上昇
//--------------------------------------------------------------
static const SCRDATA_LIFTMOVE DataScr0103_2_15_0 =
{LPOSID0103_LIFT210,FALSE,HEROTWTYPE_ON,0,-8,0,{0,NUM_FX32(-2),0}};
static const SCRDATA_ACMD DataScr0103_2_15_1 =
{FLDOBJ_ID_PLAYER,AC_JUMP_D_3G_24F};
static const SCRDATA_LIFTMOVE DataScr0103_2_15_2 =
{LPOSID0103_LIFT210,TRUE,HEROTWTYPE_ON,0,8,0,{0,NUM_FX32(2),0}};

static const TW_SCRDATA Data_Scr0103_2_Lift15[] =
{
	{TWSCRCODE_LIFT,&DataScr0103_2_15_0},
	{TWSCRCODE_ACMD,&DataScr0103_2_15_1},
	{TWSCRCODE_LIFT,&DataScr0103_2_15_2},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	スクリプト　リフト移動その2-16 10下降->1階->10上昇
//--------------------------------------------------------------
static const SCRDATA_LIFTMOVE DataScr0103_2_16_0 =
{LPOSID0103_LIFT209,TRUE,HEROTWTYPE_ON,0,-8,0,{0,NUM_FX32(-2),0}};
static const SCRDATA_ACMD DataScr0103_2_16_1 =
{FLDOBJ_ID_PLAYER,AC_JUMP_R_3G_24F};
static const SCRDATA_LIFTMOVE DataScr0103_2_16_2 =
{LPOSID0103_LIFT209,FALSE,HEROTWTYPE_ON,0,8,0,{0,NUM_FX32(2),0}};

static const TW_SCRDATA Data_Scr0103_2_Lift16[] =
{
	{TWSCRCODE_LIFT,&DataScr0103_2_16_0},
	{TWSCRCODE_ACMD,&DataScr0103_2_16_1},
	{TWSCRCODE_LIFT,&DataScr0103_2_16_2},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	スクリプト　リフト移動その2-17 1階->10->下降->10上昇
//--------------------------------------------------------------
static const SCRDATA_LIFTMOVE DataScr0103_2_17_0 =
{LPOSID0103_LIFT209,FALSE,HEROTWTYPE_ON,0,-8,0,{0,NUM_FX32(-2),0}};
static const SCRDATA_ACMD DataScr0103_2_17_1 =
{FLDOBJ_ID_PLAYER,AC_JUMP_L_3G_24F};
static const SCRDATA_LIFTMOVE DataScr0103_2_17_2 =
{LPOSID0103_LIFT209,TRUE,HEROTWTYPE_ON,0,8,0,{0,NUM_FX32(2),0}};

static const TW_SCRDATA Data_Scr0103_2_Lift17[] =
{
	{TWSCRCODE_LIFT,&DataScr0103_2_17_0},
	{TWSCRCODE_ACMD,&DataScr0103_2_17_1},
	{TWSCRCODE_LIFT,&DataScr0103_2_17_2},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	スクリプト 0103 lift 1
//--------------------------------------------------------------
static const SCRPOSDATA DATA_ScrPos0103[] =
{
	{33,225,36,TWFLAGTYPE_NON,0,DATA_Scr0103_Lift00},	//0-1-0
	{33,225,25,TWFLAGTYPE_NON,0,DATA_Scr0103_Lift01},	//1-0-0
	{33,225,21,TWFLAGTYPE_NON,0,DATA_Scr0103_Lift02},	//1-2-1
	{33,225,10,TWFLAGTYPE_NON,0,DATA_Scr0103_Lift03},	//2-1-1
	{35,225,8,TWFLAGTYPE_NON,0,DATA_Scr0103_Lift04},	//2-3-2
	{47,225,8,TWFLAGTYPE_NON,0,DATA_Scr0103_Lift05},	//3-2-2
	{56,225,10,TWFLAGTYPE_NON,0,DATA_Scr0103_Lift06},	//4-7-4
	{56,225,50,TWFLAGTYPE_NON,0,DATA_Scr0103_Lift07},	//7-4-4
	{51,225,31,TWFLAGTYPE_NON,0,DATA_Scr0103_Lift08},	//5-6-5
	{63,225,31,TWFLAGTYPE_NON,0,DATA_Scr0103_Lift09},	//6-5-5
//	{54,225,52,TWFLAGTYPE_NON,0,DATA_Scr0103_Lift10},	//7-8-7
//	{35,225,52,TWFLAGTYPE_NON,0,DATA_Scr0103_Lift11},	//8-7-7
	
	{35,233,15,TWFLAGTYPE_NON,0,Data_Scr0103_2_Lift00},	//2 1-2-1
	{47,233,15,TWFLAGTYPE_NON,0,Data_Scr0103_2_Lift01},	//2 2-1-1 
	{51,233,15,TWFLAGTYPE_NON,0,Data_Scr0103_2_Lift02}, //2 2-3-2
	{63,233,15,TWFLAGTYPE_NON,0,Data_Scr0103_2_Lift03}, //2 3-2-2
	{65,233,17,TWFLAGTYPE_NON,0,Data_Scr0103_2_Lift04}, //2 3-4-3
	{65,233,43,TWFLAGTYPE_NON,0,Data_Scr0103_2_Lift05}, //2 4-3-3
	{63,233,45,TWFLAGTYPE_NON,0,Data_Scr0103_2_Lift06}, //2 4-7-4
	{51,233,45,TWFLAGTYPE_NON,0,Data_Scr0103_2_Lift07},	//2 7-4-4
	{63,233,52,TWFLAGTYPE_NON,0,Data_Scr0103_2_Lift08}, //2 5-6-5
	{43,233,52,TWFLAGTYPE_NON,0,Data_Scr0103_2_Lift09},	//2 6-5-5
	
#if 0	//仕掛け簡易化により廃止
	{41,233,50,TWFLAGTYPE_NON,0,Data_Scr0103_2_Lift10}, //2 6-9-6
	{41,233,33,TWFLAGTYPE_NON,0,Data_Scr0103_2_Lift11},	//2 9-6-6
	{28,233,31,TWFLAGTYPE_NON,0,Data_Scr0103_2_Lift12},	//2 8-9-8
	{39,233,31,TWFLAGTYPE_NON,0,Data_Scr0103_2_Lift13},	//2 9-8-8
#endif	
	
	{49,233,36,TWFLAGTYPE_NON,0,Data_Scr0103_2_Lift14},
	{49,225,33,TWFLAGTYPE_NON,0,Data_Scr0103_2_Lift15},
	{28,233,38,TWFLAGTYPE_NON,0,Data_Scr0103_2_Lift16},
	{31,225,38,TWFLAGTYPE_NON,0,Data_Scr0103_2_Lift17},
	
	{0,0,0,0,NULL}, //終端
};

//==============================================================================
//	座標スクリプト 0104
//==============================================================================
//--------------------------------------------------------------
///	アカギ登場
//--------------------------------------------------------------
static const SCRDATA_SCRCALL DataScr0104_0 =
{SCRID_D34R0104_AKAGI00};
static const SCRDATA_EVSEQ DataScr0104_01 =
{TWEVSEQ_04_00};
static const TW_SCRDATA DATA_Scr0104_0[] =
{
	{TWSCRCODE_SCRCALL,&DataScr0104_0},
	{TWSCRCODE_EVSEQSET,&DataScr0104_01},
	{TWSCRCODE_MAX,NULL},
};

static const SCRPOSDATA DATA_ScrPos0104[] =
{
	//ストッパーシロナと会話せず４層へ移動した状態
	{65,193,41,TWFLAGTYPE_TWEVSEQ_EQ,TWEVSEQ_03_00,DATA_Scr0104_0},
	//ストッパーシロナと会話し４層へ移動した状態
	{65,193,41,TWFLAGTYPE_TWEVSEQ_EQ,TWEVSEQ_03_01,DATA_Scr0104_0},
	{0,0,0,0,NULL},
};
		
//==============================================================================
//	座標スクリプト 0105
//==============================================================================
//--------------------------------------------------------------
///	スクリプト　滝下り
//--------------------------------------------------------------
static const SCRDATA_TAKIKUDARI DataScr0105_Taki0 =
{ 
	{
		WFALLVEC_Y,
		LPOSMOVE_ZONE_DOWN,
		0, -42, 0,
		0, -664, 0,
		0, -21, 0,
		{0,NUM_FX32(-1),0},
	},
};

static const TW_SCRDATA DATA_Scr0105_Taki[] =
{
	{TWSCRCODE_TAKIKUDARI,&DataScr0105_Taki0},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	スクリプト　ギラティナ影00
//--------------------------------------------------------------
static const SCRDATA_GIRAS DataScr0105_Giras00 =
{
	63,161+8,57-48,GIRAS_DIR_DOWN,GIRAS_SE_VOICE,
	{FX32_ONE,FX32_ONE,FX32_ONE},
	{0,0,GIRAS_MOVE_VALUE},64
};
static const SCRDATA_GIRASFLAG DataScr0105_GirasFlag00 =
{GIRASFLAG_0500};
static const TW_SCRDATA DATA_Scr0105_Giras00[] =
{
	{TWSCRCODE_GIRAS,&DataScr0105_Giras00},
	{TWSCRCODE_GIRASFLAGSET,&DataScr0105_GirasFlag00},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	スクリプト　ギラティナ影01
//--------------------------------------------------------------
static const SCRDATA_GIRAS DataScr0105_Giras01 =
{
	98-56,161-24,56-24,GIRAS_DIR_RIGHT,GIRAS_SE_VOICE,
	{FX32_ONE,FX32_ONE,FX32_ONE},
	{GIRAS_MOVE_VALUE,0,0},72
};
static const SCRDATA_GIRASFLAG DataScr0105_GirasFlag01 =
{GIRASFLAG_0501};
static const TW_SCRDATA DATA_Scr0105_Giras01[] =
{
	{TWSCRCODE_GIRAS,&DataScr0105_Giras01},
	{TWSCRCODE_GIRASFLAGSET,&DataScr0105_GirasFlag01},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///　スクリプト 0105
//--------------------------------------------------------------
static const SCRPOSDATA DATA_ScrPos0105[] =
{
	/*
	{104,170,75,TWFLAGTYPE_NON,0,DATA_Scr0105_Taki},
	{104,170,76,TWFLAGTYPE_NON,0,DATA_Scr0105_Taki},
	{104,170,77,TWFLAGTYPE_NON,0,DATA_Scr0105_Taki},
	{104,170,78,TWFLAGTYPE_NON,0,DATA_Scr0105_Taki},
	{104,170,79,TWFLAGTYPE_NON,0,DATA_Scr0105_Taki},
	*/
	
	{65,161,57,TWFLAGTYPE_GIRASFLAG_OFF,GIRASFLAG_0500,DATA_Scr0105_Giras00},
	{98,161,56,TWFLAGTYPE_GIRASFLAG_OFF,GIRASFLAG_0501,DATA_Scr0105_Giras01},
	{98,161,57,TWFLAGTYPE_GIRASFLAG_OFF,GIRASFLAG_0501,DATA_Scr0105_Giras01},
	{0,0,0,0,NULL}, //終端
};

//==============================================================================
//	座標スクリプト 0107
//==============================================================================
//--------------------------------------------------------------
///	スクリプト　滝登り
//--------------------------------------------------------------
static const SCRDATA_TAKINOBORI DATAScr0107_0 =
{
	{
		WFALLVEC_Y,
		LPOSMOVE_ZONE_UP,
		0, 42, 0,
		0, 664, 0,
		0, 20, 0,
		{0,NUM_FX32(8),0},
	},
};

static const TW_SCRDATA Data_Scr0107_Taki[] =
{
	{TWSCRCODE_TAKINOBORI,&DATAScr0107_0},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	スクリプト　ユクシー登場
//--------------------------------------------------------------
static const TW_SCRDATA Data_Scr0107_Yukushi[] =
{
	{TWSCRCODE_ROCKPOKE_YUKUSHI,NULL},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	スクリプト　アグノム登場
//--------------------------------------------------------------
static const TW_SCRDATA Data_Scr0107_Agunomu[] =
{
	{TWSCRCODE_ROCKPOKE_AGUNOMU,NULL},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	スクリプト　エムリット登場
//--------------------------------------------------------------
static const TW_SCRDATA Data_Scr0107_Emurit[] =
{
	{TWSCRCODE_ROCKPOKE_EMURIT,NULL},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///　スクリプト 0107
//--------------------------------------------------------------
static const SCRPOSDATA DATA_ScrPos0107[] =
{
	/*
	{104,128,75,TWFLAGTYPE_NON,0,Data_Scr0107_Taki},	//0
	{104,128,76,TWFLAGTYPE_NON,0,Data_Scr0107_Taki},
	{104,128,77,TWFLAGTYPE_NON,0,Data_Scr0107_Taki},
	{104,128,78,TWFLAGTYPE_NON,0,Data_Scr0107_Taki},
	{104,128,79,TWFLAGTYPE_NON,0,Data_Scr0107_Taki},
	*/
	
	{86,129,53,TWFLAGTYPE_EV_OFF,
	 TWEVFLAGNO_0107YUKUSHIEV,Data_Scr0107_Yukushi},
	{87,129,53,TWFLAGTYPE_EV_OFF,
	 TWEVFLAGNO_0107YUKUSHIEV,Data_Scr0107_Yukushi},
	 
	{100,129,67,TWFLAGTYPE_EV_OFF,
	 TWEVFLAGNO_0107AGUNOMUEV,Data_Scr0107_Agunomu},
	{100,129,68,TWFLAGTYPE_EV_OFF,
	 TWEVFLAGNO_0107AGUNOMUEV,Data_Scr0107_Agunomu},
	 
	{80,129,67,TWFLAGTYPE_EV_OFF,
	 TWEVFLAGNO_0107EMURITEV,Data_Scr0107_Emurit},
	{80,129,68,TWFLAGTYPE_EV_OFF,
	 TWEVFLAGNO_0107EMURITEV,Data_Scr0107_Emurit},
	 
	{0,0,0,0,NULL}, //終端
};

//==============================================================================
//	座標スクリプト 0109
//==============================================================================
//--------------------------------------------------------------
///	シロナ、アカギ会話
//--------------------------------------------------------------
static const SCRDATA_SCRCALL DataScr0109_00 =
{SCRID_D34R0109_SIROAKA00};
static const SCRDATA_EVSEQ DataScr0109_01 =
{TWEVSEQ_08_01};
static const TW_SCRDATA DATA_Scr0109_0[] =
{
	{TWSCRCODE_SCRCALL,&DataScr0109_00},
	{TWSCRCODE_EVSEQSET,&DataScr0109_01},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	八層Bへ
//--------------------------------------------------------------
#if 0
static const SCRDATA_SCRCALL DataScr0109_02 =
{SCRID_D34R0109_GIRA_WARP};
static const TW_SCRDATA DATA_Scr0109_1[] =
{
	{TWSCRCODE_SCRCALL,&DataScr0109_02},
	{TWSCRCODE_MAX,NULL},
};
#endif

static const SCRPOSDATA DATA_ScrPos0109[] =
{
	{84,65,76,TWFLAGTYPE_TWEVSEQ_EQ,TWEVSEQ_08_00,DATA_Scr0109_0},
	{85,65,76,TWFLAGTYPE_TWEVSEQ_EQ,TWEVSEQ_08_00,DATA_Scr0109_0},
	{86,65,76,TWFLAGTYPE_TWEVSEQ_EQ,TWEVSEQ_08_00,DATA_Scr0109_0},
//	{89,65,57,TWFLAGTYPE_TWEVSEQ_EQ,TWEVSEQ_08_02,DATA_Scr0109_1},
	{0,0,0,0,NULL},
};

//==============================================================================
//	座標スクリプト 0110
//==============================================================================
//--------------------------------------------------------------
///	スクリプト　飛び石表示
//--------------------------------------------------------------
static const SCRDATA_TWEVFLAG DATA_Scr0110_StoneON_00_00 =
{TWEVFLAGNO_0110STONEEV_END};

static const TW_SCRDATA Data_Scr0110_StoneON_00[] =
{
	{TWSCRCODE_STONE0110_ON,NULL},
	{TWSCRCODE_TWEVFLAG_SET,&DATA_Scr0110_StoneON_00_00},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	スクリプト　飛び石非表示
//--------------------------------------------------------------
static const SCRDATA_TWEVFLAG DATA_Scr0110_StoneOFF_00_00 =
{TWEVFLAGNO_0110STONEEV_END};

static const TW_SCRDATA Data_Scr0110_StoneOFF_00[] =
{
	{TWSCRCODE_STONE0110_OFF,NULL},
	{TWSCRCODE_TWEVFLAG_OFF,&DATA_Scr0110_StoneOFF_00_00},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	ギラティナ影00
//--------------------------------------------------------------
static const SCRDATA_SCRCALL DataScr0110_Giras00_00 =
{SCRID_D34R0110_GIRA_BARK};

static const SCRDATA_GIRAS DataScr0110_Giras00_01 =
{
	15-24,-4,22,GIRAS_DIR_RIGHT,GIRAS_SE_OFF,
	{FX32_ONE/4,FX32_ONE/4,FX32_ONE/4},
	{NUM_FX32(16),0,0},48
};

static const SCRDATA_EVSEQ DataScr0110_Giras00_02 =
{TWEVSEQ_08D_00};

static const TW_SCRDATA DATA_Scr0110_Giras00[] =
{
	{TWSCRCODE_SCRCALL,&DataScr0110_Giras00_00},
	{TWSCRCODE_GIRAS,&DataScr0110_Giras00_01},
	{TWSCRCODE_EVSEQSET,&DataScr0110_Giras00_02},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	ギラティナ影01
//--------------------------------------------------------------
static const SCRDATA_GIRAS DataScr0110_Giras01_00 =
{
	15,-34,8,GIRAS_DIR_RISE,GIRAS_SE_FLIGHT,
	{FX32_ONE/2,FX32_ONE/2,FX32_ONE/2},
	{0,NUM_FX32(32),NUM_FX32(2)},32
};
static const SCRDATA_EVSEQ DataScr0110_Giras01_01 =
{TWEVSEQ_08D_01};

static const TW_SCRDATA DATA_Scr0110_Giras01[] =
{
	{TWSCRCODE_GIRAS,&DataScr0110_Giras01_00},
	{TWSCRCODE_EVSEQSET,&DataScr0110_Giras01_01},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	ギラティナ登場
//--------------------------------------------------------------
static const SCRDATA_EVSEQ DataScr0110_GiraApp00 =
{TWEVSEQ_08D_02};

static const SCRDATA_SCRCALL DataScr0110_GiraApp01 =
{SCRID_D34R0110_GIRA_ENTER_FINISH_MSG};

static const TW_SCRDATA DATA_Scr0110_GiraApp[] =
{
	{TWSCRCODE_EVSEQSET,&DataScr0110_GiraApp00},
	{TWSCRCODE_GIRA_APPEAR,NULL},
	{TWSCRCODE_SCRCALL,&DataScr0110_GiraApp01},
	{TWSCRCODE_MAX,NULL},
};

//--------------------------------------------------------------
///	座標スクリプト0110 まとめ
//--------------------------------------------------------------
static const SCRPOSDATA DATA_ScrPos0110[] =
{
	{
		15,1,23,
		TWFLAGTYPE_EV_OFF,TWEVFLAGNO_0110STONEEV_END,
		Data_Scr0110_StoneON_00
	},
	{
		15,1,24,
		TWFLAGTYPE_EV_ON,TWEVFLAGNO_0110STONEEV_END,
		Data_Scr0110_StoneOFF_00
	},
	{15,1,24,TWFLAGTYPE_TWEVSEQ_EQ,TWEVSEQ_08_02,DATA_Scr0110_Giras00},
	{15,1,17,TWFLAGTYPE_TWEVSEQ_EQ,TWEVSEQ_08D_00,DATA_Scr0110_Giras01},
	{15,1,14,TWFLAGTYPE_TWEVSEQ_EQ,TWEVSEQ_08D_01,DATA_Scr0110_GiraApp},
	{0,0,0,0,NULL},
};

//==============================================================================
//	スクリプト　ゾーン別座標起動まとめ
//==============================================================================
//--------------------------------------------------------------
///	スクリプト　ゾーン別座標起動
//--------------------------------------------------------------
static const SCRPOSDATA_ZONE DATA_ScrPosDataZoneTbl[] =
{
	{ ZONE_ID_D34R0101, DATA_ScrPos0101 },
	{ ZONE_ID_D34R0102, DATA_ScrPos0102 },
	{ ZONE_ID_D34R0103, DATA_ScrPos0103 },
	{ ZONE_ID_D34R0104, DATA_ScrPos0104 },
	{ ZONE_ID_D34R0105, DATA_ScrPos0105 },
	{ ZONE_ID_D34R0107, DATA_ScrPos0107 },
	{ ZONE_ID_D34R0109, DATA_ScrPos0109 },
	{ ZONE_ID_D34R0110, DATA_ScrPos0110 },
	{ ZONE_ID_MAX, NULL },
};

//==============================================================================
//	PUTOBJ
//==============================================================================
//--------------------------------------------------------------
/// PUTOBJ 34r0101
//--------------------------------------------------------------
static const PUTOBJ_DATA DATA_TW_PutOBJ0101[] =
{
	{0,TWOBJTYPE_ANA_OUT,55,289,39,TWFLAGTYPE_NON,0},
	{0,TWOBJTYPE_MAX,0,0,0},
};

//--------------------------------------------------------------
/// PUTOBJ 34r0107
//--------------------------------------------------------------
static const PUTOBJ_DATA DATA_TW_PutOBJ0107[] =
{
	{0,TWOBJTYPE_WATER_FALL,106,153,78,TWFLAGTYPE_NON,0},
	{0,TWOBJTYPE_MAX,0,0,0,0,0},
};

//--------------------------------------------------------------
/// PUTOBJ 34r0110
//--------------------------------------------------------------
static const PUTOBJ_DATA DATA_TW_PutOBJ0110[] =
{
	{0,TWOBJTYPE_ANA_OUT,15,1,12,TWFLAGTYPE_TWEVSEQ_HI,TWEVSEQ_08D_03},
	{0,TWOBJTYPE_MAX,0,0,0,0,0},
};

//--------------------------------------------------------------
/// PUTOBJ 34r0111
//--------------------------------------------------------------
static const PUTOBJ_DATA DATA_TW_PutOBJ0111[] =
{
	{0,TWOBJTYPE_ANA_OUT,116,65,74,TWFLAGTYPE_NON,0},
	{0,TWOBJTYPE_MAX,0,0,0},
};

//--------------------------------------------------------------
///	PUTOBJ ZONEDATA
//--------------------------------------------------------------
static const PUTOBJ_ZONEDATA DATA_PutOBJZoneDataTbl[] =
{
	{ZONE_ID_D34R0101,DATA_TW_PutOBJ0101},
	{ZONE_ID_D34R0107,DATA_TW_PutOBJ0107},
	{ZONE_ID_D34R0110,DATA_TW_PutOBJ0110},
	{ZONE_ID_D34R0111,DATA_TW_PutOBJ0111},
	{ZONE_ID_MAX,NULL},
};

//==============================================================================
//	TW_FLDOBJ_HEAD
//==============================================================================
//--------------------------------------------------------------
///	第一階層
//--------------------------------------------------------------
static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0101_0 =
{
	TWFLAGTYPE_EVENT,0,
	TWFLDOBJ_ROT_OFF,0,
	{
		TW_OBJID_0101_SIRONA_00,
		CHAMPION,
		MV_DMY,
		0,
		0,
		0,
		DIR_DOWN,
		0,0,0,
		0,0,
		55,40,
		GRID_SIZE_FX32(289),
	},
};

static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0101_1 =
{
	TWFLAGTYPE_TWEVSEQ_LOW,TWEVSEQ_01_02,
	TWFLDOBJ_ROT_OFF,0,
	{
		TW_OBJID_0101_SIRONA_01,
		CHAMPION,
		MV_DMY,
		0,
		0,
		SCRID_D34R0101_SIRONA02,
		DIR_RIGHT,
		0,0,0,
		0,0,
		39,52,
		GRID_SIZE_FX32(289),
	},
};

static const TW_FLDOBJ_HEAD *DATA_FldOBJHead_0101[] =
{
	&DATA_FldOBJHead_0101_0,
	&DATA_FldOBJHead_0101_1,
	NULL,
};

//--------------------------------------------------------------
//	第２階層
//--------------------------------------------------------------
static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0102_0 =
{
	TWFLAGTYPE_EVENT,0,
	TWFLDOBJ_ROT_OFF,0,
	{
		TW_OBJID_0102_SPPOKE2,
		BLDAGUNOMU,
		MV_DMY,
		0,
		0,
		0,
		DIR_UP,
		0,0,0,
		0,0,
//		25,56,
		12,57,
		GRID_SIZE_FX32(257),
	},
};

static const TW_FLDOBJ_HEAD *DATA_FldOBJHead_0102[] =
{
	&DATA_FldOBJHead_0102_0,
	NULL,
};

//--------------------------------------------------------------
///	第三階層
//--------------------------------------------------------------
static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0103_0 =	//シロナその1
{
	TWFLAGTYPE_TWEVSEQ_EQ,TWEVSEQ_03_00,
	TWFLDOBJ_ROT_ON,90,
	{
		TW_OBJID_0103_SIRONA,
		CHAMPION,
		MV_DMY,
		0,
		0,
		SCRID_D34R0103_SIRONA00,
		DIR_DOWN,
		0,0,0,
		0,0,
		30,20,
		GRID_SIZE_FX32(233),
	},
};

//３層話しかけ後に一層に戻り、再度２，３層に移動後
static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0103_1 =	//シロナその2
{
	TWFLAGTYPE_TWEVSEQ_EQ,TWEVSEQ_03_01,
	TWFLDOBJ_ROT_ON,90,
	{
		TW_OBJID_0103_SIRONA,
		CHAMPION,
		MV_DMY,
		0,
		0,
		SCRID_D34R0103_SIRONA00,
		DIR_UP,
		0,0,0,
		0,0,
		30,20,
		GRID_SIZE_FX32(232),
	},
};

static const TW_FLDOBJ_HEAD *DATA_FldOBJHead_0103[] =
{
	&DATA_FldOBJHead_0103_0,
	&DATA_FldOBJHead_0103_1,
	NULL,
};

//--------------------------------------------------------------
//	第４階層
//--------------------------------------------------------------
static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0104_0 =
{
	TWFLAGTYPE_EVENT,0,
	TWFLDOBJ_ROT_OFF,0,
	{
		TW_OBJID_0104_AKAGI,
		GINGABOSS,
		MV_DMY,
		0,
		0,
		0,
		DIR_UP,
		0,0,0,
		0,0,
		65,49,
		GRID_SIZE_FX32(193),
	},
};

static const TW_FLDOBJ_HEAD *DATA_FldOBJHead_0104[] =
{
	&DATA_FldOBJHead_0104_0,
	NULL,
};

//--------------------------------------------------------------
//	第５階層
//--------------------------------------------------------------
static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0105_0 =
{
	TWFLAGTYPE_AKAGI_FLAG_EQ,0,
	TWFLDOBJ_ROT_OFF,0,
	{
		TW_OBJID_0105_AKAGI,
		GINGABOSS,
		MV_RND_H_LIM,
		0,
		0,
		0,
		DIR_LEFT,
		0,0,0,
		1,0,
		89,64,
		GRID_SIZE_FX32(161),
	},
};

static const TW_FLDOBJ_HEAD *DATA_FldOBJHead_0105[] =
{
	&DATA_FldOBJHead_0105_0,
	NULL,
};

//--------------------------------------------------------------
//	第六階層
//--------------------------------------------------------------
static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0107_0 =
{
	TWFLAGTYPE_EV_ON,
	TWEVFLAGNO_FALLROCK0_6,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0107_ROCK_0,
		ROCK,
		MV_DMY,
		0,
		0,
		SCRID_HIDEN_KAIRIKI,
		DIR_UP,
		0,0,0,
		0,0,
		75,68,
		GRID_SIZE_FX32(129),
	},
};

static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0107_1 =
{
	TWFLAGTYPE_EV_ON,
	TWEVFLAGNO_FALLROCK1_6,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0107_ROCK_1,
		ROCK,
		MV_DMY,
		0,
		0,
		SCRID_HIDEN_KAIRIKI,
		DIR_UP,
		0,0,0,
		0,0,
		98,67,
		GRID_SIZE_FX32(129),
	},
};

static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0107_2 =
{
	TWFLAGTYPE_EV_ON,
	TWEVFLAGNO_FALLROCK2_6,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0107_ROCK_2,
		ROCK,
		MV_DMY,
		0,
		0,
		SCRID_HIDEN_KAIRIKI,
		DIR_UP,
		0,0,0,
		0,0,
		86,56,
		GRID_SIZE_FX32(129),
	},
};

static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0107_3 =
{
	TWFLAGTYPE_EVENT,
	0,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0107_YUKUSHI,
		SPPOKE1,
		MV_DMY,
		0,
		0,
		0,
		DIR_UP,
		0,0,0,
		0,0,
		86,58,
		GRID_SIZE_FX32(129-16),
	},
};

static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0107_4 =
{
	TWFLAGTYPE_EVENT,
	0,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0107_AGUNOMU,
		SPPOKE3,
		MV_DMY,
		0,
		0,
		0,
		DIR_UP,
		0,0,0,
		0,0,
		96,67,
		GRID_SIZE_FX32(129-12),
	},
};

static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0107_5 =
{
	TWFLAGTYPE_EVENT,
	0,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0107_EMURIT,
		SPPOKE2,
		MV_DMY,
		0,
		0,
		0,
		DIR_UP,
		0,0,0,
		0,0,
		77,68,
		GRID_SIZE_FX32(129-8),
	},
};

static const TW_FLDOBJ_HEAD *DATA_FldOBJHead_0107[] =
{
	&DATA_FldOBJHead_0107_0,
	&DATA_FldOBJHead_0107_1,
	&DATA_FldOBJHead_0107_2,
	&DATA_FldOBJHead_0107_3,
	&DATA_FldOBJHead_0107_4,
	&DATA_FldOBJHead_0107_5,
	NULL,
};

//--------------------------------------------------------------
///	第七階層
//--------------------------------------------------------------
static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0108_0 =	//7層岩0
{
	TWFLAGTYPE_EV_ON,
	TWEVFLAGNO_FALLROCK0_7,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0108_ROCK_0,
		ROCK,
		MV_DMY,
		0,
		0,
		SCRID_HIDEN_KAIRIKI,
		DIR_UP,
		0,0,0,
		0,0,
		75,68,
		GRID_SIZE_FX32(115),
	},
};

static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0108_1 =	//7層岩1
{
	TWFLAGTYPE_EV_ON,
	TWEVFLAGNO_FALLROCK1_7,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0108_ROCK_1,
		ROCK,
		MV_DMY,
		0,
		0,
		SCRID_HIDEN_KAIRIKI,
		DIR_UP,
		0,0,0,
		0,0,
		98,67,
		GRID_SIZE_FX32(115),
	},
};

static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0108_2 =	//7層岩2
{
	TWFLAGTYPE_EV_ON,
	TWEVFLAGNO_FALLROCK2_7,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0108_ROCK_2,
		ROCK,
		MV_DMY,
		0,
		0,
		SCRID_HIDEN_KAIRIKI,
		DIR_UP,
		0,0,0,
		0,0,
		86,56,
		GRID_SIZE_FX32(115),
	},
};

static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0108_3 =	//エムリット
{
	TWFLAGTYPE_EV_ON,
	TWEVFLAGNO_0108EMURIT,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0108_EM,
		TW7EMULIT,
		MV_DMY,
		0,
		0,
		0,
		DIR_UP,
		0,0,0,
		0,0,
		68,64,
		GRID_SIZE_FX32(115),
	},
};

static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0108_4 =	//ユクシー
{
	TWFLAGTYPE_EV_ON,
	TWEVFLAGNO_0108YUKUSHI,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0108_YUKU,
		TW7YUKUSHI,
		MV_DMY,
		0,
		0,
		0,
		DIR_UP,
		0,0,0,
		0,0,
		82,60,
		GRID_SIZE_FX32(115),
	},
};

static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0108_5 =	//アグノム
{
	TWFLAGTYPE_EV_ON,
	TWEVFLAGNO_0108AGUNOMU,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0108_AGU,
		TW7AGUNOMU,
		MV_DMY,
		0,
		0,
		0,
		DIR_UP,
		0,0,0,
		0,0,
		96,71,
		GRID_SIZE_FX32(115),
	},
};

static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0108_6 =	//シロナ　ストッパー
{
	TWFLAGTYPE_TWEVSEQ_EQ,
	TWEVSEQ_04_00,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0108_SIRONA,
		CHAMPION,
		MV_DMY,
		0,
		0,
		SCRID_D34R0108_SIRONA00,
		DIR_UP,
		0,0,0,
		0,0,
		85,80,
		GRID_SIZE_FX32(115),
	},
};

static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0108_7 = //シロナ ストッパー解除
{
	TWFLAGTYPE_TWEVSEQ_EQ,
	TWEVSEQ_07_00,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0108_SIRONA,
		CHAMPION,
		MV_DMY,
		0,
		0,
		SCRID_D34R0108_SIRONA00,
		DIR_RIGHT,
		0,0,0,
		0,0,
		84,84,
		GRID_SIZE_FX32(115),
	},
};

static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0108_8 = //岩落とし穴MSG OBJ
{
	TWFLAGTYPE_NON,
	0,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0108_HOLEMSG0,
		NONDRAW,
		MV_DMY,
		0,
		0,
		SCRID_D34R0108_HOLE_MSG,
		DIR_UP,
		0,0,0,
		0,0,
		68,67,
		GRID_SIZE_FX32(115),
	},
};

static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0108_9 = //岩落とし穴MSG OBJ
{
	TWFLAGTYPE_NON,
	0,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0108_HOLEMSG1,
		NONDRAW,
		MV_DMY,
		0,
		0,
		SCRID_D34R0108_HOLE_MSG,
		DIR_UP,
		0,0,0,
		0,0,
		75,67,
		GRID_SIZE_FX32(115),
	},
};

static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0108_10 = //岩落とし穴MSG OBJ
{
	TWFLAGTYPE_NON,
	0,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0108_HOLEMSG2,
		NONDRAW,
		MV_DMY,
		0,
		0,
		SCRID_D34R0108_HOLE_MSG,
		DIR_UP,
		0,0,0,
		0,0,
		79,67,
		GRID_SIZE_FX32(115),
	},
};

static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0108_11 = //岩落とし穴MSG OBJ
{
	TWFLAGTYPE_NON,
	0,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0108_HOLEMSG3,
		NONDRAW,
		MV_DMY,
		0,
		0,
		SCRID_D34R0108_HOLE_MSG,
		DIR_UP,
		0,0,0,
		0,0,
		85,52,
		GRID_SIZE_FX32(115),
	},
};

static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0108_12 = //岩落とし穴MSG OBJ
{
	TWFLAGTYPE_NON,
	0,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0108_HOLEMSG4,
		NONDRAW,
		MV_DMY,
		0,
		0,
		SCRID_D34R0108_HOLE_MSG,
		DIR_UP,
		0,0,0,
		0,0,
		85,56,
		GRID_SIZE_FX32(115),
	},
};

static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0108_13 = //岩落とし穴MSG OBJ
{
	TWFLAGTYPE_NON,
	0,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0108_HOLEMSG5,
		NONDRAW,
		MV_DMY,
		0,
		0,
		SCRID_D34R0108_HOLE_MSG,
		DIR_UP,
		0,0,0,
		0,0,
		85,60,
		GRID_SIZE_FX32(115),
	},
};

static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0108_14 = //岩落とし穴MSG OBJ
{
	TWFLAGTYPE_NON,
	0,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0108_HOLEMSG6,
		NONDRAW,
		MV_DMY,
		0,
		0,
		SCRID_D34R0108_HOLE_MSG,
		DIR_UP,
		0,0,0,
		0,0,
		96,68,
		GRID_SIZE_FX32(115),
	},
};

static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0108_15 = //岩落とし穴MSG OBJ
{
	TWFLAGTYPE_NON,
	0,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0108_HOLEMSG7,
		NONDRAW,
		MV_DMY,
		0,
		0,
		SCRID_D34R0108_HOLE_MSG,
		DIR_UP,
		0,0,0,
		0,0,
		100,68,
		GRID_SIZE_FX32(115),
	},
};

static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0108_16 = //岩落とし穴MSG OBJ
{
	TWFLAGTYPE_NON,
	0,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0108_HOLEMSG8,
		NONDRAW,
		MV_DMY,
		0,
		0,
		SCRID_D34R0108_HOLE_MSG,
		DIR_UP,
		0,0,0,
		0,0,
		105,68,
		GRID_SIZE_FX32(115),
	},
};

static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0108_17 =	//7層岩落下後0
{
	TWFLAGTYPE_EV_ON,
	TWEVFLAGNO_FALLROCK0_OK,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0108_ROCK_OK_0,
		ROCK,
		MV_DMY,
		0,
		0,
		0,
		DIR_UP,
		0,0,0,
		0,0,
		68,66,
		GRID_SIZE_FX32(113),
	},
};

static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0108_18 =	//7層岩落下後1
{
	TWFLAGTYPE_EV_ON,
	TWEVFLAGNO_FALLROCK1_OK,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0108_ROCK_OK_1,
		ROCK,
		MV_DMY,
		0,
		0,
		0,
		DIR_UP,
		0,0,0,
		0,0,
		96,69,
		GRID_SIZE_FX32(113),
	},
};

static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0108_19 =	//7層岩落下後2
{
	TWFLAGTYPE_EV_ON,
	TWEVFLAGNO_FALLROCK2_OK,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0108_ROCK_OK_2,
		ROCK,
		MV_DMY,
		0,
		0,
		0,
		DIR_UP,
		0,0,0,
		0,0,
		84,60,
		GRID_SIZE_FX32(113),
	},
};

static const TW_FLDOBJ_HEAD *DATA_FldOBJHead_0108[] =
{
	&DATA_FldOBJHead_0108_0,
	&DATA_FldOBJHead_0108_1,
	&DATA_FldOBJHead_0108_2,
	&DATA_FldOBJHead_0108_3,
	&DATA_FldOBJHead_0108_4,
	&DATA_FldOBJHead_0108_5,
	&DATA_FldOBJHead_0108_6,
	&DATA_FldOBJHead_0108_7,
	&DATA_FldOBJHead_0108_8,
	&DATA_FldOBJHead_0108_9,
	&DATA_FldOBJHead_0108_10,
	&DATA_FldOBJHead_0108_11,
	&DATA_FldOBJHead_0108_12,
	&DATA_FldOBJHead_0108_13,
	&DATA_FldOBJHead_0108_14,
	&DATA_FldOBJHead_0108_15,
	&DATA_FldOBJHead_0108_16,
	&DATA_FldOBJHead_0108_17,
	&DATA_FldOBJHead_0108_18,
	&DATA_FldOBJHead_0108_19,
	NULL,
};

//--------------------------------------------------------------
///	第八階層
//--------------------------------------------------------------
//シロナその１　八層到達時　アカギ対話前
static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0109_0 =
{
	TWFLAGTYPE_TWEVSEQ_EQ,
	TWEVSEQ_08_00,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0109_SIRONA,
		CHAMPION,
		MV_DMY,
		0,
		0,
		SCRID_D34R0109_SIRONA_TALK00,
		DIR_UP,
		0,0,0,
		0,0,
		85,74,
		GRID_SIZE_FX32(65),
	},
};

//シロナその２　アカギ対話後
static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0109_1 =
{
	TWFLAGTYPE_TWEVSEQ_EQ,
	TWEVSEQ_08_01,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0109_SIRONA,
		CHAMPION,
		MV_DMY,
		0,
		0,
		SCRID_D34R0109_SIRONA_TALK00,
		DIR_RIGHT,
		0,0,0,
		0,0,
		84,74,
		GRID_SIZE_FX32(65),
	},
};

//シロナその３　アカギ決着後の連れ歩き後
static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0109_2 =
{
	TWFLAGTYPE_TWEVSEQ_HI,
	TWEVSEQ_08_02,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0109_SIRONA,
		CHAMPION,
		MV_DMY,
		0,
		0,
		SCRID_D34R0109_SIRONA_TALK00,
		DIR_UP,
		0,0,0,
		0,0,
		89,69,
		GRID_SIZE_FX32(65),
	},
};

//アカギその１　アカギ対話前
static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0109_3 =
{
	TWFLAGTYPE_TWEVSEQ_LOW,
	TWEVSEQ_08_00,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0109_AKAGI,
		GINGABOSS,
		MV_DMY,
		0,
		0,
		SCRID_D34R0109_AKAGI_TALK00,
		DIR_DOWN,
		0,0,0,
		0,0,
		85,70,
		GRID_SIZE_FX32(65),
	},
};

//アカギその１　アカギ対話後
static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0109_4 =
{
	TWFLAGTYPE_TWEVSEQ_EQ,
	TWEVSEQ_08_01,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0109_AKAGI,
		GINGABOSS,
		MV_DMY,
		0,
		0,
		SCRID_D34R0109_AKAGI_TALK00,
		DIR_LEFT,
		0,0,0,
		0,0,
		85,74,
		GRID_SIZE_FX32(65),
	},
};

static const TW_FLDOBJ_HEAD *DATA_FldOBJHead_0109[] =
{
	&DATA_FldOBJHead_0109_0,
	&DATA_FldOBJHead_0109_1,
	&DATA_FldOBJHead_0109_2,
	&DATA_FldOBJHead_0109_3,
	&DATA_FldOBJHead_0109_4,
	NULL,
};

//--------------------------------------------------------------
///	第八層深部
//--------------------------------------------------------------
static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0110_0 =	//ギラティナ
{
	TWFLAGTYPE_TWEVSEQ_EQ,
	TWEVSEQ_08D_02,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0110_GIRA,
		GIRATINAORIGIN,
		MV_DMY,
		0,
		0,
		SCRID_D34R0110_GIRA_TALK00,
		DIR_UP,
		0,0,0,
		0,0,
		15,13,
		GRID_SIZE_FX32(1),
	},
};

static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0110_1 =	//シロナ
{
	TWFLAGTYPE_EVENT,
	0,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0110_SIRONA,
		CHAMPION,
		MV_DMY,
		0,
		0,
		0,
		DIR_UP,
		0,0,0,
		0,0,
		15,23,
		GRID_SIZE_FX32(1),
	},
};

static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0110_2 =	//アカギ
{
	TWFLAGTYPE_EVENT,
	0,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0110_AKAGI,
		GINGABOSS,
		MV_DMY,
		0,
		0,
		0,
		DIR_UP,
		0,0,0,
		0,0,
		15,23,
		GRID_SIZE_FX32(1),
	},
};

static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0110_3 =	//穴
{
	TWFLAGTYPE_TWEVSEQ_EQ,
	TWEVSEQ_08D_03,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0110_HOLE,
		NONDRAW,
		MV_DMY,
		0,
		0,
		SCRID_D34R0110_HOLE_TALK,
		DIR_UP,
		0,0,0,
		0,0,
		15,13,
		GRID_SIZE_FX32(1),
	},
};

static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0110_4 =	//透明シロナ話しかけ
{
	TWFLAGTYPE_TWEVSEQ_EQ,
	TWEVSEQ_08D_03,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0110_NONSIRONA,
		NONDRAW,
		MV_DMY,
		0,
		0,
		SCRID_D34R0110_SIRONA_TALK00,
		DIR_UP,
		0,0,0,
		0,0,
		15,15,
		GRID_SIZE_FX32(1),
	},
};

static const TW_FLDOBJ_HEAD *DATA_FldOBJHead_0110[] =
{
	&DATA_FldOBJHead_0110_0,
	&DATA_FldOBJHead_0110_1,
	&DATA_FldOBJHead_0110_2,
	&DATA_FldOBJHead_0110_3,
	&DATA_FldOBJHead_0110_4,
	NULL,
};

//--------------------------------------------------------------
///	第八層　その２
//--------------------------------------------------------------
static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0111_0 =	//穴
{
	TWFLAGTYPE_NON,
	0,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0111_HOLE,
		NONDRAW,
		MV_DMY,
		0,
		0,
		SCRID_D34R0111_HOLE,
		DIR_UP,
		0,0,0,
		0,0,
		116,74,
		GRID_SIZE_FX32(65),
	},
};

static const TW_FLDOBJ_HEAD DATA_FldOBJHead_0111_1 =	//発禁球
{
	TWFLAGTYPE_NON,
	0,
	TWFLDOBJ_ROT_OFF,
	0,
	{
		TW_OBJID_0111_ITEM,
		MONATERBALL,
		MV_DMY,
		0,
		FV_FLD_ITEM_322,
		SCRID_FLD_ITEM_322,
		DIR_UP,
		0,0,0,
		0,0,
		89,
		55,
		GRID_SIZE_FX32(65),
	},
};

static const TW_FLDOBJ_HEAD *DATA_FldOBJHead_0111[] =
{
	&DATA_FldOBJHead_0111_0,
	&DATA_FldOBJHead_0111_1,
	NULL,
};

//--------------------------------------------------------------
///	ゾーン別TW_FLDOBJ_HEADまとめ
//--------------------------------------------------------------
static const TW_FLDOBJ_ZONEDATA DATA_FldOBJZoneDataTbl[] =
{
	{
		ZONE_ID_D34R0101,
		DATA_FldOBJHead_0101,
	},
	{
		ZONE_ID_D34R0102,
		DATA_FldOBJHead_0102,
	},
	{
		ZONE_ID_D34R0103,
		DATA_FldOBJHead_0103,
	},
	{
		ZONE_ID_D34R0104,
		DATA_FldOBJHead_0104,
	},
	{
		ZONE_ID_D34R0105,
		DATA_FldOBJHead_0105,
	},
	{
		ZONE_ID_D34R0107,
		DATA_FldOBJHead_0107,
	},
	{
		ZONE_ID_D34R0108,
		DATA_FldOBJHead_0108,
	},
	{
		ZONE_ID_D34R0109,
		DATA_FldOBJHead_0109,
	},
	{
		ZONE_ID_D34R0110,
		DATA_FldOBJHead_0110,
	},
	{
		ZONE_ID_D34R0111,
		DATA_FldOBJHead_0111,
	},
	{
		ZONE_ID_MAX,
		NULL,
	},
};

//==============================================================================
//	デバッグ
//==============================================================================
//----
#ifdef PM_DEBUG
//----

//----
#endif
//----

//==============================================================================
//	使わなくなったが参考に残す
//==============================================================================
//----
#if 0
//----
//--------------------------------------------------------------
///	座標発動カメラアクションイベント数
//--------------------------------------------------------------
/*
enum
{
	CMEVPOS_0,
	CMEVPOS_1,
	
	CMEVPOS_2,
	CMEVPOS_3,
	
	CMEVPOS_4,
	CMEVPOS_5,
	
	CMEVPOS_6,
	CMEVPOS_7,
	
	CMEVPOS_8,
	CMEVPOS_9,
	
	CMEVPOS_10,
	CMEVPOS_11,
	
	CMEVPOS_MAX,
};
*/

//--------------------------------------------------------------
///	座標発動自機イベント数
//--------------------------------------------------------------
/*
enum
{
	JIKIEVPOSDATA_WALLJUMP_0,
	JIKIEVPOSDATA_WALLJUMP_1,
	
	JIKIEVPOSDATA_WALLJUMP_2,
	JIKIEVPOSDATA_WALLJUMP_3,
	
	JIKIEVPOSDATA_WALLJUMP_4,
	JIKIEVPOSDATA_WALLJUMP_5,
	
	JIKIEVPOSDATA_WALLJUMP_6,
	JIKIEVPOSDATA_WALLJUMP_7,
	
	JIKIEVPOSDATA_WALLJUMP_MAX,
};
*/

//--------------------------------------------------------------
///	人物回転
//--------------------------------------------------------------
/*
typedef enum
{
	OBJROTDIR_L = 0,	///<左回り
	OBJROTDIR_R,		///<右回り
}OBJROTDIR;
*/

#if 0	//old -> TW_EVJUMPDATA
//--------------------------------------------------------------
///	EVPOSDATA_WALLJUMP
//--------------------------------------------------------------
typedef struct
{
	u16 type;						//イベントタイプ
	s16 dir;						//発動方向
	int param;						//パラメタ
	TW_GRID_RECT_SIZE rect;			//イベント発生矩形
	s16 gx_size;					//グリッド移動量X
	s16 gy_size;					//グリッド移動量Y
	s16 gz_size;					//グリッド移動量Z
	s16 rot_val;					//OBJ回転量 360
	int frame;						//グリッド移動フレーム
	OFFS_VEC_TYPE offs_vec_type;	//オフセットをかけるベクトルタイプ
	OFFS_VEC_FLIP offs_vec_flip;	//オフセットの反転
	s16 dir_after;					//移動後のOBJ方向
	s16 form_after;					//移動後の自機フォーム
}EVPOSDATA_WALLJUMP;
#endif

#if 0 //old
typedef struct
{
	TW_GRID_RECT_SIZE rect;
	s16 dir;
	s16 param;
	s16 type;
	
	s16 gx_size;
	s16 gy_size;
	s16 gz_size;
	s16 frame;
	s16 dir_after;
	OFFS_VEC_TYPE offs_vec_type;
	OFFS_VEC_FLIP offs_vec_flip;
	int form_after;
	
	int obj_rot_val;
}EVPOSDATA_WALLJUMP;
#endif

#if 0
//--------------------------------------------------------------
///	CMACT_EVPOS_DATA
//--------------------------------------------------------------
typedef struct
{
	TW_GRID_RECT_SIZE rect;
	s16 dir;
	s16 frame;
	CAMERA_ANGLE angle;
}CMACT_EVPOS_DATA;
#endif

#if 0	//目標角度指定型
//--------------------------------------------------------------
/**
 * OBJ回転　フレーム回転アニメをセット
 * @param	work	TW_OBJ_ROT_WORK
 * @param	fldobj	対象となるFIELD_OBJ_PTR
 * @param	rot		目標角度0-360
 * @param	frame	目標フレーム
 * @param	dir		OBJROTDIR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJRotateAnmSet( TORNWORLD_WORK *tw, FIELD_OBJ_PTR fldobj,
	int rot, int frame, OBJROTDIR dir )
{
	int i = 0;
	TW_OBJ_ROT_PROC_WORK *work = &tw->objrot;
	TW_OBJ_ROT_WORK *tbl = work->rot_tbl;
	
	/*
	n 60
	f 50
	r L
	n > f = OK
	n(60) - f(50) = 10 -> L = * -1 = -10 ok
		
	n 50
	f 60
	r L
	n < f = n += 360;
	n(410) - f(60) = 350 -> L = * -1 = -350 ok
	
	n 50
	f 60
	f R
	n < f OK
	n(50) - f(60) = -10 -> R = + = 10; OK
	
	n 60
	f 50
	r R
	n > f = n -= 360;
	n(-300) - f(50) = -350 -> R = + = 350; ok
	*/
	
	while( i < work->max ){
		if( tbl->fldobj == fldobj ){
			fx32 rotfx = NUM_FX32( rot );
			fx32 rotfx_now = tbl->rot_fx;
			
			if( dir == OBJROTDIR_L ){		//r-
				if( rotfx_now < rotfx ){
					rotfx_now += NUM_FX32( 360 );
				}
			}else if( rotfx_now > rotfx ){	//r+
				rotfx_now -= NUM_FX32( 360 );
			}
			
			tbl->rot_add = rotfx_now - rotfx;
			tbl->rot_add *= -1;
			tbl->rot_add /= frame;
			tbl->frame = frame;
			return;
		}
		
		tbl++;
		i++;
	}
	
	GF_ASSERT( 0 );
}
#endif

//--------------------------------------------------------------
///	座標発動カメラアクションデータ
//--------------------------------------------------------------
#if 0
static const CMACT_EVPOS_DATA DATA_CameraActionEventPosTbl[] =
{
	{
		{8,2,0,0,0,31}, DIR_LEFT, 16,
		{0,0x1000-TWCM_DEF_RY, 0 }
	},
	{
		{8,2,0,0,0,31}, DIR_RIGHT, 16,
		{0,0,0}
	},
	
	{
		{2,11,0,0,0,31}, DIR_UP, 16,
		{0xb62-TWCM_DEF_RX, 0x1000-TWCM_DEF_RY, 0 },
	},
	{
		{2,11,0,0,0,31}, DIR_DOWN, 16,
		{0,0x1000-TWCM_DEF_RY, 0 },
	},
	
	{
		{8,29,0,0,0,31}, DIR_RIGHT, 16,
		{0xb62-TWCM_DEF_RX,0,0}
	},
	{
		{8,29,0,0,0,31}, DIR_LEFT, 16,
		{0xb62-TWCM_DEF_RX, 0x1000-TWCM_DEF_RY, 0 },
	},
	
	{
		{23,29,0,0,0,31}, DIR_RIGHT, 16,
		{0xb62-TWCM_DEF_RX, -0x1000, 0 }
	},
	{
		{23,29,0,0,0,31}, DIR_LEFT, 16,
		{0xb62-TWCM_DEF_RX,0,0}
	},
	
	{
		{29,11,0,0,0,31}, DIR_DOWN, 16,
		{0, -0x1000, 0 },
	},
	{
		{29,11,0,0,0,31}, DIR_UP, 16,
		{0xb62-TWCM_DEF_RX, -0x1000, 0 }
	},
	
	{
		{19,2,0,0,0,31}, DIR_RIGHT, 16,
		{0,-0x1000, 0 }
	},
	{
		{19,2,0,0,0,31}, DIR_LEFT, 16,
		{0,0,0}
	},
	
	{	//終端
		{0,0,0,0,0,0},DIR_NOT
	},
};
#endif

//--------------------------------------------------------------
///	座標発動自機アクションデータ 壁ジャンプ
//--------------------------------------------------------------
#if 0
static const EVPOSDATA_WALLJUMP DATA_JikiEventPosWallJumpTbl[] =
{
	{
		EVPOSTYPE_WALLJUMP,DIR_LEFT,0,{4,2,4, 0,0,23}, 
		-2,2,0,90,32,OFFS_VEC_Y, OFFS_VEC_FLIP_NON,
		DIR_UP,HERO_FORM_GL,
	},
	{
		EVPOSTYPE_WALLJUMP,DIR_DOWN,0,{2,4,4,0,0,23},
		2,-2,0,-90,32,OFFS_VEC_Y,OFFS_VEC_FLIP_NON,
		DIR_RIGHT,HERO_FORM_NORMAL,
	},
	
	{
		EVPOSTYPE_WALLJUMP,DIR_UP,0,{2,27,4,0,0,23},
		2,2,0,90,32,OFFS_VEC_Y,OFFS_VEC_FLIP_NON,
		DIR_UP,HERO_FORM_GU,
	},
	{
		EVPOSTYPE_WALLJUMP,DIR_LEFT,0,{4,29,4,0,0,23},
		-2,-2,0,-90,32,OFFS_VEC_Y,OFFS_VEC_FLIP_NON,
		DIR_DOWN,HERO_FORM_GL,
	},
	
	{
		EVPOSTYPE_WALLJUMP,DIR_RIGHT,0,{27,29,4, 0,0,23},
		2,-2,0,90,32,OFFS_VEC_Y,OFFS_VEC_FLIP_NON,
		DIR_RIGHT,HERO_FORM_GR,
	},
	{
		EVPOSTYPE_WALLJUMP,DIR_UP,0,{29,27,4, 0,0,23},
		-2,2,0,-90,32,OFFS_VEC_Y,OFFS_VEC_FLIP_NON,
		DIR_RIGHT,HERO_FORM_GU,
	},
	
	{
		EVPOSTYPE_WALLJUMP,DIR_RIGHT,0,{27,2,4, 0,0,23}, 
		2,2,0,-90,32,OFFS_VEC_Y,OFFS_VEC_FLIP_NON,
		DIR_LEFT,HERO_FORM_GR
	},
	{
		EVPOSTYPE_WALLJUMP,DIR_DOWN,0,{29,4,4, 0,0,23},
		-2,-2,0,90,32,OFFS_VEC_Y,OFFS_VEC_FLIP_NON,
		DIR_RIGHT,HERO_FORM_NORMAL,
	},
	{	//終端
		EVPOSTYPE_MAX,
	},
};
#endif

#if 0
{
 {CLOUD_0,NUM_FX32(0),128,0x1000,{NUM_FX32(128),NUM_FX32(96),0}},
 {CLOUD_1,NUM_FX32(0),96,0x1000,{NUM_FX32(128),NUM_FX32(96),0}},
 {CLOUD_2,NUM_FX32(0),80,0x1000,{NUM_FX32(128),NUM_FX32(96),0}},
 {CLOUD_3,NUM_FX32(0),64,0x1000,{NUM_FX32(128),NUM_FX32(96),0}},
 {CLOUD_4,NUM_FX32(0),48,0x1000,{NUM_FX32(128),NUM_FX32(96),0}},
 {CLOUD_5,NUM_FX32(0),32,0x1000,{NUM_FX32(128),NUM_FX32(96),0}},
 {CLOUD_6,NUM_FX32(0),0,0x1000,{NUM_FX32(128),NUM_FX32(96),0}},
};
#endif

#if 0
//--------------------------------------------------------------
/**
 * 座標起動カメラアクション　カメラ操作 TCB
 * @param	tcb	TCB_PTR
 * @param	wk	tcb work TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_CameraActionTCB( TCB_PTR tcb, void *wk )
{
	TORNWORLD_WORK *tw = wk;
	TW_CAMERA_WORK *cm = &tw->camera;
	TW_CMACT_WORK *work = &cm->action_work;
	
	if( work->act_flag == FALSE ){
		return;
	}
	
	work->frame--;
	
	if( work->frame <= 0 ){
		work->act_flag = FALSE;
		cm->angle_offs = cm->angle_offs_true;
		return;
	}
	
	work->offs_fx.x += work->add_val_fx.x;
	work->offs_fx.y += work->add_val_fx.y;
	work->offs_fx.z += work->add_val_fx.z;
	cm->angle_offs.x = FX32_NUM( work->offs_fx.x );
	cm->angle_offs.y = FX32_NUM( work->offs_fx.y );
	cm->angle_offs.z = FX32_NUM( work->offs_fx.z );
}

//--------------------------------------------------------------
/**
 * 座標起動カメラアクション　セット
 * @param	tw	TORNWORLD_WORK
 * @param	data	TW_CMACTDATA
 * @retval	BOOL	TRUE=セット FALSE=セットする必要なし
 */
//--------------------------------------------------------------
static BOOL tw_CameraActionSet(
	TORNWORLD_WORK *tw, const TW_CMACTDATA *data )
{
	TW_CAMERA_WORK *cm = &tw->camera;
	TW_CMACT_WORK *work = &cm->action_work;
	
	cm->angle_offs_true.x = data->cm_x;			//真とする
	cm->angle_offs_true.y = data->cm_y;
	cm->angle_offs_true.z = data->cm_z;
	
	if( cm->angle_offs.x == data->cm_x &&		//既に一致
		cm->angle_offs.y == data->cm_y &&
		cm->angle_offs.z == data->cm_z ){
		work->act_flag = FALSE;
		return( FALSE );
	}
	
	work->act_flag = TRUE;
	work->frame = data->frame;
	
	work->offs_fx.x = NUM_FX32( cm->angle_offs.x );	//目標を保存
	work->offs_fx.y = NUM_FX32( cm->angle_offs.y );
	work->offs_fx.z = NUM_FX32( cm->angle_offs.z );
	
	{
		int diff;
		diff = twAngle16ShortDiffGet( cm->angle_offs.x, data->cm_x );
		work->add_val_fx.x = NUM_FX32( diff ) / data->frame;
		diff = twAngle16ShortDiffGet( cm->angle_offs.y, data->cm_y );
		work->add_val_fx.y = NUM_FX32( diff ) / data->frame;
		diff = twAngle16ShortDiffGet( cm->angle_offs.z, data->cm_z );
		work->add_val_fx.z = NUM_FX32( diff ) / data->frame;
	}
	
	return( TRUE );
}
#endif

#if 0
//--------------------------------------------------------------
///	飛び石
//--------------------------------------------------------------
static const STEPSTONE_PUTDATA DATA_FStonePutTbl[STEPSTONE_PUT_MAX] =
{
	{2,0,0,16,1,13},
	{1,0,0,16,1,16},
	{0,0,0,16,1,19},
	
	{3,1,0,24,1,43},
	{4,1,0,27,1,43},
	{5,1,0,30,1,43},
	
	{6,2,0,33,1,31},
	{7,2,0,33,1,28},
	
	{8,3,1,53,1,18},
	{9,3,1,37,1,21},
	
	{10,3,1,39,1,33},
	{11,3,1,27,1,34},
	{12,3,1,7,1,34},
	
	{13,3,1,3,1,42},
	{14,3,1,9,1,53},
	{15,3,1,17,1,48},
	{16,3,1,28,1,57},
	{17,3,1,41,1,52},
	{18,3,1,52,1,44},
};

//--------------------------------------------------------------
///	飛び石　イベントデータ
//--------------------------------------------------------------
static const TW_STEPSTONE_EVDATA DATA_FStoneEvTbl[STEPSTONE_EVMAX] =
{
	{ DIR_UP, 0, FALSE, 16, 1, 24, 0, 0, 0 },
	{ DIR_DOWN, 0, TRUE, 16, 1, 24, 0, 0, 0 },
	
	{ DIR_RIGHT, 1, FALSE, 20, 1, 43, 0, 0, 0 },
	{ DIR_LEFT, 1, TRUE, 20, 1, 43, 0, 0, 0 },
	{ DIR_DOWN, 1, FALSE, 33, 1, 40, 0, 0, 0 },
	{ DIR_UP, 1, TRUE, 33, 1, 40, 0, 0, 0 },
	
	{ DIR_UP, 2, FALSE, 33, 1, 36, 0, 0, 0 },
	{ DIR_DOWN, 2, TRUE, 33, 1, 36, 0, 0, 0 },
	{ DIR_LEFT, 2, FALSE, 39, 1, 28, 0, 0, 0 },
	{ DIR_RIGHT, 2, TRUE, 39, 1, 28, 0, 0, 0 },
};
#endif

#if 0
typedef struct
{
	int dir;
	int gx;
	int gz;
}TW_3GJUMP_POS;

static const TW_3GJUMP_POS DATA_3GJumpPosTbl[] =
{
	{ DIR_UP, 16, 21 },
	{ DIR_UP, 16, 18 },
	{ DIR_UP, 16, 15 },
	{ DIR_DOWN, 16, 14 },
	{ DIR_DOWN, 16, 17 },
	{ DIR_DOWN, 16, 20 },
	
	{ DIR_UP, 16, 29 },
	{ DIR_DOWN, 16, 28 },
	
	{ DIR_RIGHT, 22, 43 },
	{ DIR_RIGHT, 25, 43 },
	{ DIR_RIGHT, 28, 43 },
	{ DIR_RIGHT, 31, 43 },
	{ DIR_LEFT, 23, 43 },
	{ DIR_LEFT, 26, 43 },
	{ DIR_LEFT, 29, 43 },
	{ DIR_LEFT, 32, 43 },
	
	{ DIR_UP, 33, 33 },
	{ DIR_UP, 33, 30 },
	{ DIR_DOWN, 33, 32 },
	{ DIR_DOWN, 33, 29 },
	
	{ DIR_RIGHT, 34, 28 },
	{ DIR_LEFT, 35, 28 },
	
	{ DIR_NOT, 35, 28 },
};

int FldTornWorld_Attr3GJumpCheck(
	FIELDSYS_WORK *fsys, int gx, int gz, int dir )
{
	GIMMICKWORK *gmc;
	const TW_3GJUMP_POS *pos = DATA_3GJumpPosTbl;
	
	gmc = SaveData_GetGimmickWork( GameSystem_GetSaveData(fsys) );
	
	if( GIMMICKWORK_GetAssignID(gmc) != FLD_GIMMICK_TORNWORLD ){
		return( FALSE );	//破れた世界ではない
	}
	
	while( pos->dir != DIR_NOT ){
		if( pos->dir == dir && pos->gz == gz && pos->gx == gx ){
			return( TRUE );
		}
		pos++;
	}
	
	return( FALSE );
}
#endif

#if 0
//==============================================================================
//	リンク座標
//==============================================================================
//--------------------------------------------------------------
/**
 * 指定されたゾーンIDに対応するリンク座標データを取得
 * @param	zone_id
 * @retval	TW_ZONE_LINKPOS
 */
//--------------------------------------------------------------
static const TW_ZONE_LINKPOS * tw_LinkPosZoneDataGet( int zone_id )
{
	int i = 0;
	const TW_ZONE_LINKPOS *link = DATA_TwZoneLinkPosTbl;
	while( i < TW_ZONECOUNT_MAX){
		if( link->zone_id == zone_id ){ return(link); }
		link++; i++;
	}
	return( NULL );
}

//--------------------------------------------------------------
/**
 * 現ゾーンでのリンク座標該当チェック
 * @param	tw	TORNWORLD_WORK
 * @param	gx	グリッドX
 * @param	gy	グリッドY
 * @param	gz	グリッドZ
 * @retval	TW_LINKPOS	NULL=該当無し
 */
//--------------------------------------------------------------
static const TW_LINKPOS * tw_ZoneLinkPosDataCheck(
		TORNWORLD_WORK *tw, int gx, int gy, int gz )
{
	const TW_ZONE_LINKPOS *zl_pos;
	
	zl_pos = tw_LinkPosZoneDataGet( tw->fsys->location->zone_id );
	
	if( zl_pos ){
		u32 i = 0;
		const TW_LINKPOS *link;
		const TW_LINKPOS **tbl = zl_pos->pos_tbl;
		
		while( tbl[i] != NULL ){
			link = tbl[i];
			if( link->gz == gz && link->gx == gx && link->gy == gy ){
				OS_Printf( "ゾーンチェンジイベント起動 %d\n", i );
				return( link );
			}
			i++;
		}
	}
	
	return( NULL );
}
#endif

#if 0
		
			if( sys.trg & PAD_BUTTON_SELECT ){
				work->seq_no = 0;
				work->vanish = TRUE;
			}else{
				u8 r,g,b;
				GXRgb orgb, rgb = tw_GrapEdgeColorGet(
						work->head.tw, GIRAS_EDGE_NO );
				orgb = rgb;
				
	//			0-4(0,1,2,3,4)
				r = (rgb) & 0x1f;
				g = (rgb>>5) & 0x1f;
				b = (rgb>>10) & 0x1f;
				
				if( sys.trg & PAD_BUTTON_B ){
					work->debug++;
					work->debug %= 3;
					switch( work->debug ){
					case 0: OS_Printf( "青変更\n" ); break;
					case 1: OS_Printf( "赤変更\n" ); break;
					case 2: OS_Printf( "緑変更\n" ); break;
					}
				}
				
				if( sys.trg & PAD_BUTTON_R ){
					switch( work->debug ){
					case 0: b = (b+1) & 0x1f; break;
					case 1: r = (r+1) & 0x1f; break;
					case 2: g = (g+1) & 0x1f; break;
					}
				}else if( sys.trg & PAD_BUTTON_L ){
					switch( work->debug ){
					case 0: b = (b-1) & 0x1f; break;
					case 1: r = (r-1) & 0x1f; break;
					case 2: g = (g-1) & 0x1f; break;
					}
				}
				
				rgb = GX_RGB( r, g, b );
				tw_GrapEdgeColorSet(
					work->head.tw, GIRAS_EDGE_NO, rgb );
				
				if( orgb != rgb ){
					OS_Printf( "R=%d,G=%d,B=%d\n", r, g, b );
				}
#endif

#if 0
//--------------------------------------------------------------
/**
 * OBJ回転　初期化
 * @param	tw	TORNWORLD_WORK
 * @param	max	回転させるOBJ最大数
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJRotateProc_Init( TORNWORLD_WORK *tw, int max )
{
	TW_OBJ_ROT_PROC_WORK *work = &tw->objrot;
	
	work->max = max;
	max *= sizeof( TW_OBJ_ROT_WORK );
	work->rot_tbl = sys_AllocMemory( HEAPID_FIELD, max );
	memset( work->rot_tbl, 0, max );
	
	max = work->max * sizeof( TW_OBJ_ROT_REG_WORK );
	work->rot_reg_tbl = sys_AllocMemory( HEAPID_FIELD, max );
	memset( work->rot_reg_tbl, 0, max );
}

//--------------------------------------------------------------
/**
 * OBJ回転　回転パラメタ初期化
 * @param	tw		TORNWORLD_WORK
 * @param	act		回転させるモデルを持つBLACT_WORK_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJRotateProc_ActMdlInit(
		TORNWORLD_WORK *tw, BLACT_WORK_PTR act )
{
	TW_OBJ_ROT_PROC_WORK *work = &tw->objrot;
	NNSG3dResMdl *pResMdl = BLACT_ResMdlGet( act );
	NNSG3dResTex *pResTex = BLACT_ResTexGet( act );
	NNSG3dRenderObj *pRndObj = BLACT_RenderObjGet( act );
	
	GF_ASSERT( work->pResAnm == NULL );	//初期化済み
	
	sys_InitAllocator( &work->Allocator, HEAPID_FIELD, 4 );
	
	work->pResAnm = FE_ArcDataLoadAlloc(
			tw->fsys->fes, NARC_fldeff_hero_rote_nsbta, ALLOC_LO );
	work->pResAnmIdx = NNS_G3dGetAnmByIdx( work->pResAnm, 0 );
	work->pAnmObj = NNS_G3dAllocAnmObj(
		&work->Allocator, work->pResAnmIdx, pResMdl );
	NNS_G3dAnmObjInit( work->pAnmObj, work->pResAnmIdx, pResMdl, pResTex );
	
	NNS_G3dRenderObjAddAnmObj( pRndObj, work->pAnmObj );
}

//--------------------------------------------------------------
/**
 * OBJ回転　削除
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJRotateProc_Delete( TORNWORLD_WORK *tw )
{
	TW_OBJ_ROT_PROC_WORK *work = &tw->objrot;
	
	if( work->pAnmObj != NULL ){
		NNS_G3dFreeAnmObj( &work->Allocator, work->pAnmObj );
		sys_FreeMemoryEz( work->pResAnm );
		work->pResAnm = NULL;
	}
	
	sys_FreeMemoryEz( work->rot_tbl );
	work->rot_tbl = NULL;
	sys_FreeMemoryEz( work->rot_reg_tbl );
	work->rot_reg_tbl = NULL;
}

//--------------------------------------------------------------
/**
 * OBJ回転　指定OBJに回転動作を付ける為に登録
 * @param	tw	TORNWORLD_WORK
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	rot	0-359
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJRotate_RegOBJ(
		TORNWORLD_WORK *tw, FIELD_OBJ_PTR fldobj, int rot )
{
	int i = 0;
	TW_OBJ_ROT_PROC_WORK *work = &tw->objrot;
	TW_OBJ_ROT_REG_WORK *tbl = work->rot_reg_tbl;
	
	while( i < work->max ){
		if( tbl->fldobj == NULL ){
			tbl->fldobj = fldobj;
			tbl->init_rotate = rot;
			return;
		}
		
		i++;
		tbl++;
	}
	
	GF_ASSERT( 0 );
}

//--------------------------------------------------------------
/**
 * OBJ回転　指定OBJに回転動作を付ける為に登録したものを処理
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJRotateProc_RegOBJProc( TORNWORLD_WORK *tw )
{
	int i;
	TW_OBJ_ROT_PROC_WORK *work = &tw->objrot;
	TW_OBJ_ROT_REG_WORK *tbl = work->rot_reg_tbl;
	
	for( i = 0; i < work->max; i++, tbl++ ){
		if( tbl->fldobj != NULL ){
			BLACT_WORK_PTR act =
				FieldOBJ_DrawBlAct00_BlActPtrGet( tbl->fldobj );
			
			if( act != NULL ){
				tw_OBJRotateProc_ActMdlInit( tw, act );
				tw_OBJRotate_ModelSet( tw, tbl->fldobj, tbl->init_rotate );
				tbl->fldobj = NULL;
			}
		}
	}
}

//--------------------------------------------------------------
/**
 * OBJ回転　OBJに回転動作を付ける
 * @param	tw	TORNWORLD_WORK
 * @param	fldobj	対象となるFIELD_OBJ_PTR
 * @param	rot		角度 0-359
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJRotate_ModelSet(
	TORNWORLD_WORK *tw, FIELD_OBJ_PTR fldobj, int rot )
{
	int i = 0;
	TW_OBJ_ROT_PROC_WORK *work = &tw->objrot;
	TW_OBJ_ROT_WORK *tbl = work->rot_tbl;
	BLACT_WORK_PTR act = FieldOBJ_DrawBlAct00_BlActPtrGet( fldobj );
	
	if( work->pResAnm == NULL ){	//初期化がまだである
		tw_OBJRotateProc_ActMdlInit( tw, act );
	}
	
	while( i < work->max ){
		if( tbl->fldobj == NULL ){
			memset( tbl, 0, sizeof(TW_OBJ_ROT_WORK) );
			tbl->rot_org = rot;
			tbl->rot_fx = NUM_FX32( rot );
			tbl->fldobj = fldobj;
			tbl->act_rot.pAnmObj = work->pAnmObj;
			tbl->act_rot.frame = tbl->rot_fx;
			BLACT_DrawBeforeProcSet( act, BlActOBJRotateDraw, &tbl->act_rot );
			return;
		}
		
		tbl++;
		i++;
	}
	
	GF_ASSERT( 0 );
}

//--------------------------------------------------------------
/**
 * ビルボードアクターから呼ばれるOBJ回転
 * @param	blact	BLACT_WORK_PTR
 * @param	wk		TW_BLACT_ROT_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void BlActOBJRotateDraw( BLACT_WORK_PTR act, void *wk )
{
	TW_BLACT_ROT_WORK *work = wk;
	NNS_G3dAnmObjSetFrame( work->pAnmObj, work->frame );
}

//--------------------------------------------------------------
/**
 * OBJ回転　常駐処理	TCB常駐処理から呼び出し
 * @param	tw	TORNWORLD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJRotateProcTcbCall( TORNWORLD_WORK *tw )
{
	int i = 0;
	TW_OBJ_ROT_PROC_WORK *work = &tw->objrot;
	TW_OBJ_ROT_WORK *tbl = work->rot_tbl;
	
	while( i < work->max ){
		if( tbl->fldobj != NULL ){
			tw_OBJRotateAnm( tbl );
		}
		
		tbl++;
		i++;
	}
}

//--------------------------------------------------------------
/**
 * OBJ回転　フレーム回転アニメ
 * @param	work	TW_OBJ_ROT_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJRotateAnm( TW_OBJ_ROT_WORK *work )
{
	if( work->add_frame ){
		work->add_frame--;
		twAngleAdd360Fx( &work->rot_fx, work->rot_add_fx );
		
		if( work->add_frame == 0 ){
			work->rot_fx = NUM_FX32( work->rot_org );
//			OS_Printf( "UVアニメ %dフレーム\n", FX32_NUM(work->rot_fx) );
		}
		
		work->act_rot.frame = work->rot_fx;
	}
}

//--------------------------------------------------------------
/**
 * OBJ回転　フレーム回転アニメをセット
 * @param	work	TW_OBJ_ROT_WORK
 * @param	fldobj	対象となるFIELD_OBJ_PTR
 * @param	rot		増減値
 * @param	frame	フレーム
 * @retval	nothing
 */
//--------------------------------------------------------------
static void tw_OBJRotateAnmSet(
	TORNWORLD_WORK *tw, FIELD_OBJ_PTR fldobj, int rot, int frame )
{
	int i = 0;
	TW_OBJ_ROT_PROC_WORK *work = &tw->objrot;
	TW_OBJ_ROT_WORK *tbl = work->rot_tbl;
	
	while( i < work->max ){
		if( tbl->fldobj == fldobj ){
			tbl->rot_fx = NUM_FX32( tbl->rot_org );	//オリジナルで初期化
			tbl->rot_add = NUM_FX32(rot) / frame;
			tbl->add_frame = frame;
			twAngleAdd360( &tbl->rot_org, rot );
			return;
		}
		
		tbl++;
		i++;
	}
	
	GF_ASSERT( 0 );
}

#if 0
//--------------------------------------------------------------
/**
 * OBJ回転　外部呼び出し
 * @param	fsys	FIELDSYS_WORK
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	rot		回転角度 0-359
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldTornWorld_OBJRotateRegist(
		FIELDSYS_WORK *fsys, FIELD_OBJ_PTR fldobj, int rot )
{
	TORNWORLD_WORK *tw;
	tw = fsys->fldmap->Work;
	tw_OBJRotate_Regist( tw, fldobj, rot );
}
#endif

//--------------------------------------------------------------
/**
 * OBJ回転　外部呼出し　指定OBJの回転角度を取得
 * @param	fsys	FIELDSYS_WORK
 * @param	fldobj	FIELD_OBJ_PTR
 * @retval	int		回転角度 0-359
 */
//--------------------------------------------------------------
int FldTornWorld_OBJRotateGet( FIELDSYS_WORK *fsys, FIELD_OBJ_PTR fldobj )
{
	int i = 0;
	TORNWORLD_WORK *tw = fsys->fldmap->Work;
	TW_OBJ_ROT_PROC_WORK *work = &tw->objrot;
	TW_OBJ_ROT_WORK *tbl = work->rot_tbl;
	
	while( i < work->max ){
		if( tbl->fldobj == fldobj ){
			int frame = FX32_NUM( tbl->act_rot.frame );
			return( frame );
		}
			
		tbl++;
		i++;
	}
	
	return( 0 );	//登録無し
}
#endif

/*	
	_TRAINER_BTL_SET	GINGALBOSS_02, 0
	_TRAINER_LOSE_CHECK	SCWK_ANSWER
	_IFVAL_JUMP		SCWK_ANSWER,EQ,0,ev_d05r0114_boss_lose

ev_d05r0114_boss_lose:
    //
	_TRAINER_LOSE
	_EVENT_END
	_END
*/

#if 0
/********************************************************************/
/*						INIT_CHANGE_LABEL 							*/
/********************************************************************/
ev_d28r0103_init_change:

	_IF_FLAGON_CALL	FE_INIT_CHANGE_OBJ_DEL,ev_d28r0103_init_change_obj_del

	_END

ev_d28r0103_init_change_obj_del:
	_FLAG_SET		FV_D28R0103_HAI
	_OBJ_DEL		D28R0103_HAI
	_FLAG_RESET		FE_INIT_CHANGE_OBJ_DEL
	_RET


/********************************************************************/
/*								ハイ								*/
/********************************************************************/
ev_d28r0103_hai:
	_TALK_OBJ_START

	_VOICE_PLAY		MONSNO_HAI,0
	_TALK_KEYWAIT	msg_d28r0103_sppoke3_01
	_TALK_CLOSE

	_FLAG_SET		FE_INIT_CHANGE_OBJ_DEL
	_SP_WILD_BTL_SET	MONSNO_HAI,50
	_FLAG_RESET		FE_INIT_CHANGE_OBJ_DEL

	_LOSE_CHECK			SCWK_ANSWER
	_IFVAL_JUMP			SCWK_ANSWER,EQ,0,ev_d28r0103_legend_lose
	
	//4だったら、FIGHT_POKE_GET
	_IFVAL_JUMP			SCWK_ANSWER,EQ,0,ev_d28r0103_legend_lose

	//
	_SEACRET_POKE_RETRY_CHECK	SCWK_ANSWER
	_IFVAL_JUMP		SCWK_ANSWER,EQ,1,ev_d28r0103_legend_capture_ng

//ハイ捕まえたor倒した
ev_d28r0103_legend_event_clear:
	//クリアフラグon
	_FLAG_SET			FE_D28R0103_LEGEND_CLEAR
	_TALK_OBJ_END
	_END

//ハイ捕まえ損ねた
ev_d28r0103_legend_capture_ng:
	_TALKMSG			msg_d28r0103_sppoke3_02
	_LAST_KEYWAIT
	_TALK_CLOSE
	_TALK_OBJ_END
	_END

//全滅
ev_d28r0103_legend_lose:
	_NORMAL_LOSE	
	_TALK_OBJ_END
	_END
/********************************************************************/
/*							ギンガ団幹部							*/
/********************************************************************/
ev_d28r0103_gkanbu3:
	_TALK_OBJ_START_TURN_NOT	/*振り向きなし*/

	_TALK_KEYWAIT	msg_d28r0103_gkanbu3_01
	_TALK_CLOSE

	/*振り向き　ビックリ*/
	_TURN_HERO_SITE
	_OBJ_ANIME		D28R0103_GKANBU3,anm_d28r0103_gkanbu3_01
	_OBJ_ANIME_WAIT

	_TIME_WAIT		30,SCWK_ANSWER

	_TALK_KEYWAIT	msg_d28r0103_gkanbu3_02
	_TALK_CLOSE

	//戦闘呼び出し
	_TRAINER_BTL_SET	GINGALEADER3_01,0
	_TRAINER_LOSE_CHECK SCWK_ANSWER
	_IFVAL_JUMP			SCWK_ANSWER,EQ,0,ev_d28r0103_battle_lose

	_TALK_KEYWAIT	msg_d28r0103_gkanbu3_03
	_TALK_CLOSE

	_BLACK_OUT		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	//ギンガ団消す
	_OBJ_DEL		D28R0103_GKANBU3
	_FLAG_SET		FE_L02_GINGA_WIN

	/*L01の湖*/
	_FLAG_SET		SYS_FLAG_BGM_D28

	_BLACK_IN		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	_TALK_OBJ_END
	_END


/********************************************************************/
/*							共通敗北処理							*/
/********************************************************************/
ev_d28r0103_battle_lose:
	_TRAINER_LOSE
	_TALK_OBJ_END
	_END
#endif

#if 0 //旧滝登り
//--------------------------------------------------------------
///	スクリプト　滝登り　処理番号
//--------------------------------------------------------------
enum
{
	SEQNO_TAKINOBORI_INIT = 0,
	SEQNO_TAKINOBORI_START_TURN,
	SEQNO_TAKINOBORI_MOVE0,
	SEQNO_TAKINOBORI_ZONE_CHANGE,
	SEQNO_TAKINOBORI_MOVE1,
	SEQNO_TAKINOBORI_LAST_TURN,
	SEQNO_TAKINOBORI_MAX,
};

//--------------------------------------------------------------
///	SCRDATA_TAKIKU
//--------------------------------------------------------------
typedef struct
{
	u32 zone_chg_type;
	s16 end_gx;
	s16 end_gy;
	s16 end_gz;
	s16 chg_gx;
	s16 chg_gy;
	s16 chg_gz;
	VecFx32 move_val;
}SCRDATA_TAKINOBORI;

//--------------------------------------------------------------
///	SCRWORK_TAKINOBORI
//--------------------------------------------------------------
typedef struct
{
	s16 frame;
	s16 end_gx;
	s16 end_gy;
	s16 end_gz;
	u16 cm_count;
	s16 cm_frame;
	
	VecFx32 jiki_pos;
	VecFx32 move_val;
	VecFx32 max_count;
	VecFx32 chg_count;
	VecFx32 count;
	
	VecFx32 np_pos;
	VecFx32 np_move_val;
	VecFx32 np_rotate;
	VecFx32 np_rotate_val;
	
	fx32 shake_offs;
	fx32 shake_value;
}SCRWORK_TAKINOBORI;

static const TW_CMACTDATA DATA_TakiUpCmAct00 =
{
	{0,0,0,0},
	240,
	240,
	0,
	DIR_UP,
	48,
};

static const TW_CMACTDATA DATA_TakiUpCmAct01 =
{
	{0,0,0,0},
	55,
	240,
	0,
	DIR_UP,
	16,
};

//--------------------------------------------------------------
/**
 * 滝上り　自機移動
 * @param	tw	TORNWORLD_WORK
 * @param	work	SCRWORK_TAKINOBORI
 * @retval	nothing
 */
//--------------------------------------------------------------
static void ScrTakiNoboriMove( TORNWORLD_WORK *tw, SCRWORK_TAKINOBORI *work )
{
	if( work->count.x != work->max_count.x ){
		work->count.x += work->move_val.x;
		work->jiki_pos.x += work->move_val.x;
		work->np_pos.x += work->move_val.x;
	}
	
	if( work->count.y != work->max_count.y ){
		work->count.y += work->move_val.y;
		work->jiki_pos.y += work->move_val.y;
		work->np_pos.y += work->move_val.y;
	}
	
	if( work->count.z != work->max_count.z ){
		work->count.z += work->move_val.z;
		work->jiki_pos.z += work->move_val.z;
		work->np_pos.z += work->move_val.z;
	}
	
	{
		FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( tw->fsys->player );
		FieldOBJ_VecPosSet( fldobj, &work->jiki_pos );
	}
	
	{
		VecFx32 pos;
		FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( tw->fsys->player );
		EOA_PTR eoa = Player_JointEoaGet( tw->fsys->player );
		pos = work->np_pos;
		pos.x += work->shake_offs;
//		FE_FldOBJNamiPokeRPosSet( eoa, &pos );
		
		FieldOBJ_VecDrawOffsGet( fldobj, &pos );
		pos.x = work->shake_offs;
		FieldOBJ_VecDrawOffsSet( fldobj, &pos );
	}
}

//--------------------------------------------------------------
/**
 * 滝上り　カメラアクション
 * @param	tw	TORNWORLD_WORK
 * @param	work	SCRWORK_TAKINOBORI
 * @retval	nothing
 */
//--------------------------------------------------------------
static void ScrTakiNoboriCamera(
		TORNWORLD_WORK *tw, SCRWORK_TAKINOBORI *work )
{
	FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( tw->fsys->player );
	
	if( work->cm_count == 0 ){
		if( work->count.y == GRID_SIZE_FX32(16) ){
			tw_CameraActionSet( tw, &DATA_TakiUpCmAct00 );
			work->cm_count++;
		}
	}else if( work->cm_count == 1 ){
		if( work->count.y == GRID_SIZE_FX32(21) ){
			FieldOBJ_DirDispCheckSet( fldobj, DIR_RIGHT );
			tw_CameraActionSet( tw, &DATA_TakiUpCmAct01 );
			work->cm_count++;
		}
	}
	
	if( work->cm_count == 1 && work->cm_frame >= 0 ){
		work->cm_frame++;
		if( work->cm_frame >= 32 ){
			FieldOBJ_DirDispCheckSet( fldobj, DIR_UP );
			work->cm_frame = -1;
		}
	}
}

//--------------------------------------------------------------
/**
 * 滝上り　ゆれ
 * @param	tw	TORNWORLD_WORK
 * @param	work	SCRWORK_TAKINOBORI
 * @retval	nothing
 */
//--------------------------------------------------------------
static void ScrTakiNoboriShake(
		TORNWORLD_WORK *tw, SCRWORK_TAKINOBORI *work )
{
	VecFx32 offs;
	FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( tw->fsys->player );
	
	work->shake_offs += work->shake_value;
	
	if( work->shake_offs >= TK_SHAKE_MAX ){
		work->shake_offs = TK_SHAKE_MAX;
		work->shake_value = -work->shake_value;
	}else if( work->shake_offs <= FX32_ONE ){
		work->shake_offs = FX32_ONE;
		work->shake_value = -work->shake_value;
	}
}

//--------------------------------------------------------------
/**
 * スクリプト　滝登り　初期化
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrTakiNobori_Init(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	VecFx32 pos;
	EOA_PTR eoa;
	int gx,gy,gz;
	FIELD_OBJ_PTR fldobj;
	PLAYER_STATE_PTR jiki;
	SCRWORK_TAKINOBORI *work;
	const SCRDATA_TAKINOBORI *dat;
	
	work = tw_ScrProcWorkInit( tw, sizeof(SCRWORK_TAKINOBORI) );
	
	dat = data;
	jiki = tw->fsys->player;
	fldobj = Player_FieldOBJGet( jiki );
	gx = FieldOBJ_NowPosGX_Get( fldobj );
	gy = FieldOBJ_NowPosGY_Get( fldobj );	//h
	gz = FieldOBJ_NowPosGZ_Get( fldobj );
	
	work->end_gx = gx + dat->end_gx;
	work->end_gy = gy + G_GRID_H_GRID( dat->end_gy ); //h
	work->end_gz = gz + dat->end_gz;
	
	work->max_count.x = GRID_SIZE_FX32( dat->end_gx );
	work->max_count.y = GRID_SIZE_FX32( dat->end_gy ); //g
	work->max_count.z = GRID_SIZE_FX32( dat->end_gz );
	work->chg_count.x = GRID_SIZE_FX32( dat->chg_gx );
	work->chg_count.y = GRID_SIZE_FX32( dat->chg_gy );
	work->chg_count.z = GRID_SIZE_FX32( dat->chg_gz );
	work->move_val = dat->move_val;
	
	FieldOBJ_VecPosGet( fldobj, &work->jiki_pos );
	
	FieldOBJ_DirDispCheckSet( fldobj, DIR_RIGHT );
	eoa = Player_JointEoaGet( jiki );
	GF_ASSERT( eoa != NULL );
//	FE_FldOBJNamiPokeRDirRotateInit( eoa, DIR_RIGHT, HEROTWTYPE_ON );
//	FE_FldOBJNamiPokeRPosRotateNonFlagSet( eoa, TRUE, TRUE );
	
	{
		VecFx32 clear = {0,0,0};
		FieldOBJ_VecDrawOffsOutSideSet( fldobj, &clear );
	}
	
	{
		ROTATE *rot;
		VecFx32 t_pos;
		t_pos.x = GRID_SIZE_FX32( gx+1 ) + GRID_HALF_FX32;
		t_pos.y = GRID_SIZE_FX32(H_GRID_G_GRID(gy)+2) + GRID_HALF_FX32;
		t_pos.z = GRID_SIZE_FX32( gz ) + GRID_HALF_FX32;
		
		t_pos.y += NUM_FX32( -4 );	//調整
		
//		FE_FldOBJNamiPokeRPosGet( eoa, &work->np_pos );
		work->np_move_val.x = (t_pos.x - work->np_pos.x) / 32;
		work->np_move_val.y = (t_pos.y - work->np_pos.y) / 32;
		work->np_move_val.z = (t_pos.z - work->np_pos.z) / 32;
		
//		rot = FE_FldOBJNamiPokeRRotatePtrGet( eoa );
		work->np_rotate.x = NUM_FX32( rot->x );
		work->np_rotate.y = NUM_FX32( rot->y );
		work->np_rotate.z = NUM_FX32( rot->z );
		work->np_rotate_val.x = (NUM_FX32(-90)) / 32;
		work->np_rotate_val.y = 0;
		work->np_rotate_val.z = 0;
	}
	
	{
		work->shake_offs = FX32_ONE;
		work->shake_value = 0x400; 
	}
	
	work->frame = 32;
	tw_OBJRotateAnmSet( tw, fldobj, -90, work->frame );
	
	*seq_no = SEQNO_TAKINOBORI_START_TURN;
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　滝上り　初回動作　回転に併せてゆっくり移動
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrTakiNobori_StartTurn(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	VecFx32 pos;
	SCRWORK_TAKINOBORI *work = tw_ScrProcWorkGet( tw );
	PLAYER_STATE_PTR jiki = tw->fsys->player;
	FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( jiki );
	EOA_PTR eoa = Player_JointEoaGet( jiki );
	
	work->count.y += work->move_val.y / 8;
	work->jiki_pos.y += work->move_val.y / 8;
	FieldOBJ_VecPosSet( fldobj, &work->jiki_pos );
	
	{
		ROTATE *rot;
		work->np_pos.x += work->np_move_val.x;
		work->np_pos.y += work->np_move_val.y;
		work->np_pos.z += work->np_move_val.z;
//		FE_FldOBJNamiPokeRPosSet( eoa, &work->np_pos );
		
		twAngleAdd360Fx( &work->np_rotate.x, work->np_rotate_val.x );
		twAngleAdd360Fx( &work->np_rotate.y, work->np_rotate_val.y );
		twAngleAdd360Fx( &work->np_rotate.z, work->np_rotate_val.z );
//		rot = FE_FldOBJNamiPokeRRotatePtrGet( eoa );
		rot->x = FX32_NUM( work->np_rotate.x );
		rot->y = FX32_NUM( work->np_rotate.y );
		rot->z = FX32_NUM( work->np_rotate.z );
	}
	
	work->frame--;
	
	if( work->frame <= 0 ){
		work->frame = 0;
		*seq_no = SEQNO_TAKINOBORI_MOVE0;
		return( RET_NON );
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　滝登り　移動0
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrTakiNobori_Move0(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	SCRWORK_TAKINOBORI *work = tw_ScrProcWorkGet( tw );
	
	ScrTakiNoboriShake( tw, work );
	ScrTakiNoboriMove( tw, work );
	ScrTakiNoboriCamera( tw, work );
	
	if(	work->count.x == work->chg_count.x &&
		work->count.y == work->chg_count.y &&
		work->count.z == work->chg_count.z ){
		*seq_no = SEQNO_TAKINOBORI_ZONE_CHANGE;
		return( RET_CONTINUE );
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　滝登り　ゾーンチェンジ 
 * @param	tw	TORNWORLD_WORK
 * @retval	BOOL	TRUE=再帰
 */
//--------------------------------------------------------------
static PROCRET ScrTakiNobori_ZoneChange(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	const SCRDATA_TAKINOBORI *dat = data;
	SCRWORK_TAKINOBORI *work = tw_ScrProcWorkGet( tw );
	
	if( dat->zone_chg_type == LPOSMOVE_ZONE_DOWN ){
		tw_DmyMapZoneChgDown( tw );
		tw_ZoneChangeDown( tw );
	}else if( dat->zone_chg_type == LPOSMOVE_ZONE_UP ){
		tw_DmyMapZoneChgUp( tw );
		tw_ZoneChangeUp( tw );
	}else{
		GF_ASSERT( 0 );
	}
	
	*seq_no = SEQNO_TAKINOBORI_MOVE1;
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　滝登り　移動1
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrTakiNobori_Move1(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	SCRWORK_TAKINOBORI *work = tw_ScrProcWorkGet( tw );
	
	ScrTakiNoboriShake( tw, work );
	ScrTakiNoboriMove( tw, work );
	ScrTakiNoboriCamera( tw, work );
	
	if(	work->count.x == work->max_count.x &&
		work->count.y == work->max_count.y &&
		work->count.z == work->max_count.z ){
		EOA_PTR eoa;
		FIELD_OBJ_PTR fldobj;
		
		work->frame = 16;
		fldobj = Player_FieldOBJGet( tw->fsys->player );
		eoa = Player_JointEoaGet( tw->fsys->player );
		
		tw_OBJRotateAnmSet( tw, fldobj, -90, work->frame );
		
		{
			VecFx32 t_pos;
			
			t_pos.x = GRID_SIZE_FX32( work->end_gx ) + GRID_HALF_FX32;
			t_pos.y = GRID_SIZE_FX32( H_GRID_G_GRID(work->end_gy) );
			t_pos.z = GRID_SIZE_FX32( work->end_gz ) + GRID_HALF_FX32;
			work->np_move_val.x = (t_pos.x - work->np_pos.x) / 16;
			work->np_move_val.y = (t_pos.y - work->np_pos.y) / 16;
			work->np_move_val.z = (t_pos.z - work->np_pos.z) / 16;
			work->np_rotate_val.x = NUM_FX32(180) - work->np_rotate.x / 16;
			work->np_rotate_val.y = 0;
			work->np_rotate_val.z = 0;
		}
		
		{	
			VecFx32 clear = {0,0,0};
			FieldOBJ_VecDrawOffsOutSideSet( fldobj, &clear );
		}
		
		*seq_no = SEQNO_TAKINOBORI_LAST_TURN;
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * スクリプト　滝登り　移動後回転
 * @param	tw	TORNWORLD_WORK
 * @param	seq_no	処理番号
 * @param	data	コマンドデータ
 * @retval	PROCRET	PROCRET
 */
//--------------------------------------------------------------
static PROCRET ScrTakiNobori_LastTurn(
	TORNWORLD_WORK *tw, GMEVENT_CONTROL *ev, u16 *seq_no, const void *data )
{
	SCRWORK_TAKINOBORI *work = tw_ScrProcWorkGet( tw );
	EOA_PTR eoa = Player_JointEoaGet( tw->fsys->player );
	
	{
		ROTATE *rot;
		work->np_pos.x += work->np_move_val.x;
		work->np_pos.y += work->np_move_val.y;
		work->np_pos.z += work->np_move_val.z;
//		FE_FldOBJNamiPokeRPosSet( eoa, &work->np_pos );
		
		twAngleAdd360Fx( &work->np_rotate.x, work->np_rotate_val.x );
		twAngleAdd360Fx( &work->np_rotate.y, work->np_rotate_val.y );
		twAngleAdd360Fx( &work->np_rotate.z, work->np_rotate_val.z );
//		rot = FE_FldOBJNamiPokeRRotatePtrGet( eoa );
		rot->x = FX32_NUM( work->np_rotate.x );
		rot->y = FX32_NUM( work->np_rotate.y );
		rot->z = FX32_NUM( work->np_rotate.z );
	}
	
	work->frame--;
	
	if( work->frame <= 0 ){
		PLAYER_STATE_PTR jiki = tw->fsys->player;
		FIELD_OBJ_PTR fldobj = Player_FieldOBJGet( jiki );
		
		FieldOBJ_GPosDirInit( fldobj,
			work->end_gx, work->end_gy, work->end_gz, DIR_RIGHT );
		FieldOBJ_DirMoveSet( fldobj, DIR_RIGHT );
		Player_MoveBitSet_TWorld( jiki, HEROTWTYPE_ROOF );
		
		{	//自機現地形データセット
			int gx,gy,gz;
			twJikiGridPosGet( tw, &gx, &gy, &gz );
			tw_MapDataGndDataPosDataSet( tw, gx, gy, gz, TW_GNDTYPE_MAX );
		}
		
		{
			VecFx32 clear = {0,0,0};
			FieldOBJ_VecDrawOffsOutSideSet( fldobj, &clear );
		}
		
//		FE_FldOBJNamiPokeRDirRotateInit( eoa, DIR_RIGHT, HEROTWTYPE_ROOF );
//		FE_FldOBJNamiPokeRPosRotateNonFlagSet( eoa, FALSE, FALSE );
		return( RET_END );
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
///	スクリプトテーブル　滝登り
//--------------------------------------------------------------
static const TW_SCR_PROC DATA_ScrTbl_TakiNobori[SEQNO_TAKINOBORI_MAX] =
{
	ScrTakiNobori_Init,
	ScrTakiNobori_StartTurn,
	ScrTakiNobori_Move0,
	ScrTakiNobori_ZoneChange,
	ScrTakiNobori_Move1,
	ScrTakiNobori_LastTurn,
};
#endif

#endif	//#if 0

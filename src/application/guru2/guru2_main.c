//******************************************************************************
/**
 * 
 * @file	guru2_main.c
 * @brief	ぐるぐる交換　ゲームメイン
 * @author	kagaya
 * @data	05.07.13
 *
 */
//******************************************************************************
#include "guru2_local.h"
#include "guru2_snd.h"

#include "system/render_oam.h"
#include "../../system/clact_tool.h"
#include "system/arc_tool.h"
#include "system/snd_tool.h"
#include "system/wipe.h"

#include "system/bmp_list.h"
#include "system/fontproc.h"
#include "system/pm_str.h"
#include "system/wordset.h"
#include "system/fontproc.h"
#include "system/window.h"
#include "../../field/fld_bmp.h"
#include "../../field/field_rdobj.h"

#include "msgdata/msg.naix"
#include "msgdata/msg_guru2.h"

#include "poketool/pokeparty.h"
#include "itemtool/myitem.h"
#include "poketool/poke_memo.h"

#include "system/font_arc.h"	//フォントアーカイブインデックス
#include "../../demo/egg/data/egg_data_def.h" //卵アーカイブインデックス
#include "../nutmixer/nutmixer.naix"

//==============================================================================
//	define
//==============================================================================
//--------------------------------------------------------------
//	debug
//--------------------------------------------------------------
//----PM_DEBUG
#ifdef PM_DEBUG

//----GURU2_DEBUG_ON
#ifdef GURU2_DEBUG_ON
#define DEBUG_DISC_NO (3)
//#define DEBUG_DISP_CHECK_VANISH
//EGG_3D_USE
#endif
//----GURU2_DEBUG_ON END

//----DEBUG_ONLY_FOR_kagaya
#ifdef DEBUG_ONLY_FOR_kagaya
#define DEBUG_GURU2_PRINTF 			//定義でOS_Printf()有効
#endif
//----DEBUG_ONLY_FOR_kagaya END

#define DEBUG_GURU2_PRINTF_FORCE	//他の環境でも出すPrintf()

#endif
//----PM_DEBUG END

//--------------------------------------------------------------
//	ゲームシンボル
//--------------------------------------------------------------
#define FRAME_SEC (30)	///<秒


#define GURU2_GAME_FRAME (FRAME_SEC*20)
#define GURU2_GAME_FRAME_H (GURU2_GAME_FRAME/2)

#define COMM_WAIT_ERROR_FRAME (FRAME_SEC*30)

#define G2M_TEMPWORK_SIZE (32)

#define GURU2_MSG_WAIT_FRAME (FRAME_SEC*4)
#define GURU2_RESULT_NAME_WAIT_FRAME (FRAME_SEC*4)
#define GURU2_RESULT_MSG_WAIT_FRAME (FRAME_SEC*4)

#define GURU2_KINOMI_MAX (1)	//木の実最大

///aフレームでbに到達する速度にcフレームで達する為の加速度を割り出す
#define ACCEL_ARRIVAL_FX(a,b,c) (((NUM_FX32(b)/a)/c)
///aフレームでbに到達する速度を出す
#define ACCEL_FRAME_FX(a,b) ((NUM_FX32(b)/a))

//--------------------------------------------------------------
///	VRAMマネージャ
//--------------------------------------------------------------
#define GURU2_VRAMTRANSFER_MANAGER_NUM (8)	//<VRAM転送マネージャ最大数

#define CHAR_MANAGER_MAX (32)
#define PLTT_MANAGER_MAX (16) 
#define CHAR_VRAMTRANS_M_SIZE (0x4000)	//512
#define CHAR_VRAMTRANS_S_SIZE (0x4000)	//512

//--------------------------------------------------------------
///	3Dカラー
//--------------------------------------------------------------
#define EDGE_COLOR_MAX (8)	///<3D　エッジカラー
#define EDGE_COLOR_R (4)	///<3D　エッジカラーR
#define EDGE_COLOR_G (4)
#define EDGE_COLOR_B (4)

#define CLEAR_COLOR_R (31)	///<3D　クリアカラーR
#define CLEAR_COLOR_G (31)
#define CLEAR_COLOR_B (31)

//--------------------------------------------------------------
///	BG
//--------------------------------------------------------------
#define BGF_M_3D		(GF_BGL_FRAME0_M)
#define BGF_M_KAIWA 	(GF_BGL_FRAME1_M)
#define BGF_M_BG		(GF_BGL_FRAME3_M)
#define BGF_S_BG		(GF_BGL_FRAME3_S)

#define BGF_PLTT_M_SIZE (32*2)	///<BGパレット
#define BGF_PLTT_M_OFFS (32*0)
#define BGF_PLTT_S_SIZE (32*2)
#define BGF_PLTT_S_OFFS (32*0)

#define BGF_BG3_M_CHAR_OFFS (32*0)	///<BGキャラオフセット
#define BGF_BG2_S_CHAR_OFFS (32*0)
#define BGF_BG3_S_CHAR_OFFS (32*0)

#define BGF_PANO_MENU_WIN	(15)	///<ウィンドウパレット
#define BGF_PANO_TALK_WIN	(14)	///<会話ウィンドウパレット

#define BGF_CHSIZE_NAME (4*10)		///<名前入力オフセット

///<メニューウィンドウキャラ開始位置
#define BGF_CHARNO_MENU (1)	
///<会話ウィンドウキャラ開始位置
#define BGF_CHARNO_TALK (BGF_CHARNO_MENU+MENU_WIN_CGX_SIZ)
///<ビットマップキャラベース
#define BGF_BMPCHAR_BASE (BGF_CHARNO_TALK+TALK_WIN_CGX_SIZ)

///会話ウィンドウビットマップ
#define BGF_BMPCHAR_TALK (BGF_BMPCHAR_BASE)
///会話ウィンドウビットマップサイズ
#define BGF_BMPCHAR_SIZE (FLD_MSG_WIN_SX*FLD_MSG_WIN_SY)

///ネームウィンドウビットマップ
#define BGF_BMPCHAR_NAME_BASE (BGF_BMPCHAR_TALK+BGF_BMPCHAR_SIZE)

///ネームウィンドウ算出
#define BGF_BMPCHAR_NAME(a) (BGF_BMPCHAR_NAME_BASE+(BGF_CHSIZE_NAME*(a)))

#define DOTCHAR(a)	(((a)/8)*8)	///<ドットサイズ->キャラサイズ

//--------------------------------------------------------------
///	BMP
//--------------------------------------------------------------
#define BMPSIZE_NAME_X (8)
#define BMPSIZE_NAME_Y (2)

#define CCX (16)
#define CCY (12)

#define BMPPOS_NAME_X_2_0 (CCX-4)
#define BMPPOS_NAME_Y_2_0 (CCY+6)
#define BMPPOS_NAME_X_2_1 (CCX-4)
#define BMPPOS_NAME_Y_2_1 (CCY-6)

#define BMPPOS_NAME_X_3_0 (CCX-4)
#define BMPPOS_NAME_Y_3_0 (CCY+6)
#define BMPPOS_NAME_X_3_1 (CCX-11)
#define BMPPOS_NAME_Y_3_1 (CCY-2)
#define BMPPOS_NAME_X_3_2 (CCX+3)
#define BMPPOS_NAME_Y_3_2 (CCY-2)

#define BMPPOS_NAME_X_4_0 (CCX-4)	//ok
#define BMPPOS_NAME_Y_4_0 (CCY+6)
#define BMPPOS_NAME_X_4_1 (CCX-12)	//ok
#define BMPPOS_NAME_Y_4_1 (CCY+1)
#define BMPPOS_NAME_X_4_2 (CCX-4)	//ok
#define BMPPOS_NAME_Y_4_2 (CCY-6)
#define BMPPOS_NAME_X_4_3 (CCX+4)	//ok
#define BMPPOS_NAME_Y_4_3 (CCY+1)

#define BMPPOS_NAME_X_5_0 (CCX-4)
#define BMPPOS_NAME_Y_5_0 (CCY+7)

#define BMPPOS_NAME_X_5_1 (CCX-12)
#define BMPPOS_NAME_Y_5_1 (CCY+2)

#define BMPPOS_NAME_X_5_2 (CCX-10)
#define BMPPOS_NAME_Y_5_2 (CCY-3)

#define BMPPOS_NAME_X_5_3 (CCX+2)
#define BMPPOS_NAME_Y_5_3 (CCY-3)

#define BMPPOS_NAME_X_5_4 (CCX+4)
#define BMPPOS_NAME_Y_5_4 (CCY+2)

#define NAME_COL (GF_PRINTCOLOR_MAKE(FBMP_COL_BLACK,FBMP_COL_BLK_SDW,0))
#define NAME_COL_MINE (GF_PRINTCOLOR_MAKE(FBMP_COL_RED,FBMP_COL_RED_SDW,0))
	
//--------------------------------------------------------------
///	セルID
//--------------------------------------------------------------
enum
{
	EDID_EGG_NCGR = 0,
	EDID_EGG_NCLR,
	EDID_EGG_NCER,
	EDID_EGG_NANR,
	EDID_CDOWN_NCGR,
	EDID_CDOWN_NCLR,
	EDID_CDOWN_NCER,
	EDID_CDOWN_NANR,
};

//--------------------------------------------------------------
///	会話ウィンドウ
//--------------------------------------------------------------
enum							///<会話ウィンドウリスト
{
	GURU2TALK_WIN_TALK = 0,		///<会話
	GURU2TALKWIN_MAX,			///<最大数
};

enum							///<ネームウィンドウ
{
	GURU2NAME_WIN_NAME_0,
	GURU2NAME_WIN_NAME_1,
	GURU2NAME_WIN_NAME_2,
	GURU2NAME_WIN_NAME_3,
	GURU2NAME_WIN_NAME_4,
	GURU2NAME_WIN_MAX,
};

#define STR_BUF_SIZE (0x0100)	///<文字バッファサイズ

//--------------------------------------------------------------
///	カメラ
//--------------------------------------------------------------
#define CAMERA_TARGET_X (FX32_ONE*0)		///<カメラターゲット
#define CAMERA_TARGET_Y (FX32_ONE*0)
#define CAMERA_TARGET_Z (FX32_ONE*0)

#define CM_ANGLE_X (-32)					///<カメラアングル
#define CM_ANGLE_Y (0)
#define CM_ANGLE_Z (0)
//#define CM_PERSP (10)						///<カメラパース
#define CM_PERSP (6)						///<カメラパース
#define CM_DISTANCE (0x143)

#define CAMERA_ANGLE_X (FX_GET_ROTA_NUM(CM_ANGLE_X))
#define CAMERA_ANGLE_Y (FX_GET_ROTA_NUM(CM_ANGLE_Y))
#define CAMERA_ANGLE_Z (FX_GET_ROTA_NUM(CM_ANGLE_Z))
#define CAMERA_PERSP (FX_GET_ROTA_NUM(CM_PERSP))
#define CAMERA_DISTANCE (CM_DISTANCE<<FX32_SHIFT)

//--------------------------------------------------------------
///	タッチパネル
//--------------------------------------------------------------
#define TP_SYNC (4)	///<タッチパネルサンプリングバッファ数

#define TP_BTN_CX (128)		///<ボタン中心座標
#define TP_BTN_CY (96)		///<ボタン中心座標
#define TP_BTN_HSX (128/2)	///<ボタンサイズ1/2
#define TP_BTN_HSY (128/2)	///<ボタンサイズ1/2

//--------------------------------------------------------------
///	角度
//--------------------------------------------------------------
#define ROT16_ONE (0x10000/256)
#define ROT16_360 (ROT16_ONE*256)
#define ROT16_AG(a) (ROT16_ONE*(a))
#define ROT16_AG_FX(a) NUM_FX32(ROT16_AG(a))

//--------------------------------------------------------------
//	皿
//--------------------------------------------------------------
#if 0
#define DISC_POS_X_FX32 (FX32_ONE*0)		///<皿初期位置
#define DISC_POS_Y_FX32 (FX32_ONE*-10)
#define DISC_POS_Z_FX32 (FX32_ONE*0)
#else
#define DISC_POS_X_FX32 (NUM_FX32(0))		///<皿初期位置
#define DISC_POS_Y_FX32 (NUM_FX32(-36))
#define DISC_POS_Z_FX32 (NUM_FX32(0))
#endif

#define DISC_ROTATE_X (0)					///<皿回転角度X
#define DISC_ROTATE_Y (0)
#define DISC_ROTATE_Z (0)
#define DISC_ROTATE_DRAW_OFFS_FX_2 (NUM_FX32(-72))
#define DISC_ROTATE_DRAW_OFFS_FX_3 (NUM_FX32(0))
#define DISC_ROTATE_DRAW_OFFS_FX_4 (NUM_FX32(-181))
#define DISC_ROTATE_DRAW_OFFS_FX_5 (NUM_FX32(0))

#define DISC_ROTATE_SPEED_MAX_FX (NUM_FX32(-4))	///<回転速度
#define EGG_ROTATE_SPEED_MAX_FX (NUM_FX32(4))	///<回転速度
#define DISC_ROTATE_SPEED_LOW_FX (NUM_FX32(-1))	///<回転速度
#define EGG_ROTATE_SPEED_LOW_FX (NUM_FX32(1))	///<回転速度

#define EGG_ATARI_HABA_L_FX (NUM_FX32(-4))
#define EGG_ATARI_HABA_R_FX (NUM_FX32(5))

//#define DISC_TOP_SPEED (10)
#define DISC_TOP_SPEED (14)
#define DISC_TOP_SPEED_FX (NUM_FX32(DISC_TOP_SPEED))
#define DISC_ACCEL_FX (ACCEL_FRAME_FX(GURU2_GAME_FRAME_H,DISC_TOP_SPEED))
#define DISC_LOW_SPEED (4)
#define DISC_LOW_SPEED_FX (NUM_FX32(DISC_LOW_SPEED))
#define DISC_LAST_RANGE_FX (NUM_FX32(16))

//--------------------------------------------------------------
//	卵
//--------------------------------------------------------------
#if 0
#define EGG_DISC_CX_FX (NUM_FX32(128))			///<卵皿中心位置
#define EGG_DISC_CY_FX (NUM_FX32(100))
#define EGG_DISC_CXS (60)						///<卵皿サイズ
#define EGG_DISC_CYS (40)
#else
#define EGG_DISC_CX_FX (NUM_FX32(0))			///<卵皿中心位置
#define EGG_DISC_CY_FX (NUM_FX32(-6))
#define EGG_DISC_CZ_FX (NUM_FX32(0))
#define EGG_DISC_CXS (22)						///<卵皿半径サイズ
#define EGG_DISC_CZS (22)
#endif

#define EGG_START_OFFS_Y_FX (NUM_FX32(192))		///<卵登場オフセット
#define EGG_ADD_NEXT_FRAME (15)					///<卵追加時の待ち時間
#define EGG_ADD_END_WAIT (30)					///<卵追加後の待ち時間

#define EGG_JUMP_TOPSPEED 		(4)
#define EGG_JUMP_TOPSPEED_FX	NUM_FX32(EGG_JUMP_TOPSPEED)
#define EGG_JUMP_FLIGHT_FRAME		(30)
#define EGG_JUMP_FLIGHT_FRAME_HL	(EGG_JUMP_FLIGHT_FRAME/2)
#define EGG_JUMP_ACCLE_FX (EGG_JUMP_TOPSPEED_FX/EGG_JUMP_FLIGHT_FRAME_HL)
#define EGG_JUMP_ROTATE_ACCLE_FX (ROT16_AG_FX(32*4)/EGG_JUMP_FLIGHT_FRAME)

#define EGG_FRONT_ANGLE (90)

#define EGG_SPIN_TOPSPEED (24)
#define EGG_SPIN_TOPSPEED_FX NUM_FX32(EGG_SPIN_TOPSPEED)
#define EGG_SPIN_STOPSPEED (4)
#define EGG_SPIN_STOPSPEED_FX NUM_FX32(EGG_SPIN_STOPSPEED)
#define EGG_SPIN_DOWNSPEED (2)
#define EGG_SPIN_DOWNSPEED_FX NUM_FX32(EGG_SPIN_DOWNSPEED)

#define EGG_SHAKE_HABA (30)
#define EGG_SHAKE_HABA_FX (NUM_FX32(EGG_SHAKE_HABA))
#define EGG_SHAKE_TOPSPEED (20)
#define EGG_SHAKE_TOPSPEED_FX NUM_FX32(EGG_SHAKE_TOPSPEED)

//--------------------------------------------------------------
//	カウントダウンメッセージ
//--------------------------------------------------------------
#define CDOWN_CX (128)
#define CDOWN_CY (100)
#define CDOWN_WAIT_FRAME (FRAME_SEC*3+10)
#define CDOWN_SE_FRAME (30)

//--------------------------------------------------------------
//	ボタン
//--------------------------------------------------------------
#define BTN_ANM_FRAME_START (2)
#define BTN_ANM_FRAME_ON	(4)
#define BTN_ANM_FRAME_BACK	(2)

//--------------------------------------------------------------
//	メッセージ
//--------------------------------------------------------------
#define MSG_WAIT					(msg_guru2_00)
#define MSG_NICKNAME				(msg_guru2_01)
#define MSG_EGG_GET					(msg_guru2_02)
#define MSG_OMAKE_AREA				(msg_guru2_03)
#define MSG_ZANNEN					(msg_guru2_04)
#define MSG_COMM_TAIKI				(msg_guru2_05)
#define MSG_COMM_ERROR_MEMBER		(msg_guru2_06)
#define MSG_COMM_ERROR_CANCEL_OYA	(msg_guru2_07)
#define MSG_COMM_ERROR				(msg_guru2_08) 
#define MSG_COMM_ERROR_JOIN_CLOSE	(msg_guru2_09)
#define MSG_SAVE					(msg_guru2_11)
#define MSG_COMM_WAIT				(msg_guru2_10)
#define MSG_COMM_ERROR_DAME_TAMAGO	(msg_guru2_12)

//--------------------------------------------------------------
//	ボタン
//--------------------------------------------------------------
typedef enum
{
	BTN_OFF = 0,					///<ボタン押されていない
	BTN_ON,							///<ボタン押されている
}BTN;

///ボタンを押した際のウェイトフレーム
#define BTN_PUSH_WAIT_FRAME (FRAME_SEC*3)

//--------------------------------------------------------------
///	メイン関数戻り値
//--------------------------------------------------------------
typedef enum
{
	RET_NON = 0,	///<特になし
	RET_CONT,		///<継続
	RET_END,		///<終了
}RET;

//--------------------------------------------------------------
///	メイン処理シーケンス
//--------------------------------------------------------------
enum
{
	SEQNO_MAIN_INIT = 0,
	SEQNO_MAIN_FADEIN_WAIT,
	
	SEQNO_MAIN_OYA_SIGNAL_JOIN_WAIT,
	SEQNO_MAIN_OYA_SEND_JOIN_CLOSE,
	SEQNO_MAIN_OYA_CONNECT_NUM_CHECK,
	SEQNO_MAIN_OYA_SEND_PLAY_MAX,
	SEQNO_MAIN_OYA_SEND_PLAY_NO,
	SEQNO_MAIN_OYA_SIGNAL_EGG_ADD_START,
	
	SEQNO_MAIN_KO_SEND_SIGNAL_JOIN,
	SEQNO_MAIN_KO_EGG_ADD_START_WAIT,
	
	SEQNO_MAIN_EGG_DATA_SEND_INIT,
	SEQNO_MAIN_EGG_DATA_SEND_TIMING_WAIT,
	SEQNO_MAIN_EGG_DATA_TRADE_POS_SEND,
	SEQNO_MAIN_EGG_DATA_SEND,
	SEQNO_MAIN_EGG_DATA_RECV_WAIT,
	
	SEQNO_MAIN_EGG_DATA_CHECK_WAIT,
	
	SEQNO_MAIN_EGG_ADD_INIT,
	SEQNO_MAIN_EGG_ADD,
	SEQNO_MAIN_EGG_ADD_WAIT,
	SEQNO_MAIN_EGG_ADD_END_WAIT,
	
	SEQNO_MAIN_SEND_GAME_START_FLAG,
	SEQNO_MAIN_RECV_GAME_START_FLAG,
	
#ifdef DEBUG_DISP_CHECK
	SEQNO_MAIN_DEBUG_DISP_CHECK,
#endif
	
	SEQNO_MAIN_COUNTDOWN_BEFORE_TIMING_INIT,
	SEQNO_MAIN_COUNTDOWN_BEFORE_TIMING_WAIT,
	SEQNO_MAIN_COUNTDOWN_INIT,
	SEQNO_MAIN_COUNTDOWN,
	
	SEQNO_MAIN_GAME_INIT,
	SEQNO_MAIN_GAME_OYA,
	SEQNO_MAIN_GAME_KO,
	
	SEQNO_MAIN_GAME_END_INIT,
	SEQNO_MAIN_GAME_END_ERROR_ROTATE,
	SEQNO_MAIN_GAME_END_LAST_ROTATE,
	SEQNO_MAIN_GAME_END_TIMING_INIT,
	SEQNO_MAIN_GAME_END_TIMING_WAIT,
	SEQNO_MAIN_GAME_END_OYA_DATA_SEND,
	SEQNO_MAIN_GAME_END_KO_DATA_RECV,
	
	SEQNO_MAIN_RESULT_INIT,
	SEQNO_MAIN_RESULT_NAME_WAIT,
	SEQNO_MAIN_RESULT_MSG_WAIT,
	
	SEQNO_MAIN_OMAKE_CHECK,
	SEQNO_MAIN_OMAKE_MSG_WAIT,
	SEQNO_MAIN_OMAKE_ERROR_MSG_START_WAIT,
	SEQNO_MAIN_OMAKE_ERROR_MSG_WAIT,
	
	SEQNO_MAIN_SAVE_BEFORE_TIMING_INIT,
	SEQNO_MAIN_SAVE_BEFORE_TIMING_WAIT,
	SEQNO_MAIN_SAVE,
	
	#if 0
	SEQNO_MAIN_COMM_ERROR_MSG,
	SEQNO_MAIN_COMM_ERROR_MEMBER_MSG,
	SEQNO_MAIN_COMM_ERROR_OYA_CANCEL_MSG,
	SEQNO_MAIN_COMM_ERROR_JOIN_CLOSE_MSG,
	#endif
	SEQNO_MAIN_COMM_ERROR_DAME_TAMAGO_MSG,
	
	SEQNO_MAIN_MSG_WAIT_NEXT_END,
	
	SEQNO_MAIN_END_TIMING_SYNC_INIT,
	SEQNO_MAIN_END_TIMING_SYNC,
	
	SEQNO_MAIN_END_CONNECT_CHECK,
	
	SEQNO_MAIN_END_FADEOUT_START,
	SEQNO_MAIN_END_FADEOUT,
	SEQNO_MAIN_END,
	
	SEQNO_MAIN_MAX,
};

//--------------------------------------------------------------
///	卵登場処理シーケンス
//--------------------------------------------------------------
enum
{
	SEQNO_EGGSTART_INIT = 0,
	SEQNO_EGGSTART_FALL,
	SEQNO_EGGSTART_FALL_END,
	SEQNO_EGGSTART_MAX,
};

//--------------------------------------------------------------
///	タスクプライオリティ
//--------------------------------------------------------------
enum
{
	TCBPRI_EGG_DISC_JUMP = 0xff,
	TCBPRI_EGG_JUMP,
	TCBPRI_EGG_SPIN,
	TCBPRI_EGG_SHAKE,
	TCBPRI_BTN_ANM,
	TCBPRI_OMAKE_JUMP,
};

//==============================================================================
//	typedef
//==============================================================================
//--------------------------------------------------------------
//	DEBUG
//--------------------------------------------------------------
#ifdef GURU2_DEBUG_ON
typedef struct
{
	int type;
	int dummy;
	int angle_x;
	int angle_y;
	int angle_z;
	int persp;
	int distance;
}DEBUGWORK;
#endif

//--------------------------------------------------------------
///	DISCWORK
//--------------------------------------------------------------
typedef struct
{
	VecFx32 pos;
	VecFx32 draw_pos;
	VecFx32 scale;
	VecFx32 offs;		//皿にのみ影響を与えるオフセット
	VecFx32 offs_egg;	//卵にも影響を与えるオフセット
	fx32	rotate_fx;
	fx32	rotate_offs_fx;
	fx32	rotate_draw_offs_fx;
	fx32	speed_fx;
	ROTATE	rotate;
	FRO_MDL rmdl;
	FRO_OBJ robj;
}DISCWORK;

//--------------------------------------------------------------
///	EGGACTOR
//--------------------------------------------------------------
typedef struct
{
	int use_flag;
	int draw_flag;
	int set_flag;
	int comm_id;
	int play_no;
	int name_no;
	int shake_count;
	fx32 angle;
	fx32 offs_angle;
	ROTATE rotate;
	VecFx32 rotate_fx;
	VecFx32 scale;
	VecFx32 pos;
	VecFx32 offs;
//	CATS_ACT_PTR cap;	//EGG_3D_USE
	FRO_OBJ robj;
}EGGACTOR;

//--------------------------------------------------------------
///	EGGKAGE
//--------------------------------------------------------------
typedef struct
{
	int use_flag;
	ROTATE rotate;
	VecFx32 scale;
	VecFx32 pos;
	FRO_OBJ robj;
	EGGACTOR *eact;
}EGGKAGE;

//--------------------------------------------------------------
///	EGGCURSOR
//--------------------------------------------------------------
typedef struct
{
	int use_flag;
	VecFx32 pos;
	FRO_OBJ robj;
	EGGACTOR *eact;
}EGGCURSOR;

//--------------------------------------------------------------
///	EGGWORK
//--------------------------------------------------------------
typedef struct
{
	FRO_MDL m_rmdl;
	FRO_MDL m_rmdl_kage;
	FRO_MDL m_rmdl_cursor[G2MEMBER_MAX];
	FRO_ANM m_ranm_cursor[G2MEMBER_MAX];
	EGGACTOR eact[G2MEMBER_MAX];
	EGGKAGE ekage[G2MEMBER_MAX];
	EGGCURSOR ecursor[G2MEMBER_MAX];
}EGGWORK;

//--------------------------------------------------------------
///	EGGSTARTWORK
//--------------------------------------------------------------
typedef struct
{
	int seq_no;
	int frame;
	int fall_se_play;
	fx32 fall_speed;
	EGGACTOR *eact;
	GURU2MAIN_WORK *g2m;
}EGGSTARTWORK;

//--------------------------------------------------------------
///	MSGWORK
//--------------------------------------------------------------
typedef struct 
{
	int win_name_max;
	MSGDATA_MANAGER *msgman;
	WORDSET *wordset;
	GF_BGL_BMPWIN bmpwin_talk[GURU2TALKWIN_MAX];
	GF_BGL_BMPWIN bmpwin_name[GURU2NAME_WIN_MAX];
	void *strbuf;
}MSGWORK;

//--------------------------------------------------------------
///	EGGADDWORK
//--------------------------------------------------------------
typedef struct
{
	int count;
	int offs;
	int add;
	int id;
	int no;
	int max;
}EGGADDWORK;

//--------------------------------------------------------------
///	CAMERAWORK
//--------------------------------------------------------------
typedef struct
{
	fx32 distance;
	u32 persp;
	VecFx32 target_pos;
	CAMERA_ANGLE angle;
	GF_CAMERA_PTR gf_camera;
}CAMERAWORK;

//--------------------------------------------------------------
///	TPWORK
//--------------------------------------------------------------
typedef struct
{
	int x;
	int y;
	int trg;
	int prs;
	TP_ONE_DATA tp_now;
}TPWORK;

//--------------------------------------------------------------
///	BTNFADE_WORK
//--------------------------------------------------------------
typedef struct
{
	GURU2MAIN_WORK *g2m;
	int light_flag;
	u32 evy;
}BTNFADE_WORK;

//--------------------------------------------------------------
///	OMAKE_AREA
//--------------------------------------------------------------
typedef struct
{
	u16 start;
	u16 end;
}OMAKE_AREA;

//--------------------------------------------------------------
///	OMAKE_AREA_TBL
//--------------------------------------------------------------
typedef struct
{
	int max;
	const OMAKE_AREA *area;
}OMAKE_AREA_TBL;

//--------------------------------------------------------------
///	EGGJUMP_WORK
//--------------------------------------------------------------
typedef struct
{
	int use_flag;
	int seq_no;
	int frame;
	int turn_flag;
	fx32 add_y;
	fx32 accle_y;
	fx32 accle_r;
	EGGACTOR *eact;
}EGGJUMP_WORK;

//--------------------------------------------------------------
///	EGGJUMPTCB_WORK
//--------------------------------------------------------------
typedef struct
{
	EGGJUMP_WORK jump_work[G2MEMBER_MAX];
}EGGJUMPTCB_WORK;

//--------------------------------------------------------------
///	EGGSPIN_WORK
//--------------------------------------------------------------
typedef struct
{
	int use_flag;
	int seq_no;
	int count;
	fx32 add_fx;
	EGGACTOR *eact;
}EGGSPIN_WORK;

//--------------------------------------------------------------
///	EGGSPINTCB_WORK
//--------------------------------------------------------------
typedef struct
{
	EGGSPIN_WORK spin_work[G2MEMBER_MAX];
}EGGSPINTCB_WORK;

//--------------------------------------------------------------
///	EGGSHAKE_WORK
//--------------------------------------------------------------
typedef struct
{
	int use_flag;
	int seq_no;
	fx32 haba_fx;
	fx32 add_fx;
	fx32 rad;
	EGGACTOR *eact;
}EGGSHAKE_WORK;

//--------------------------------------------------------------
///	EGGSHAKETCB_WORK
//--------------------------------------------------------------
typedef struct
{
	EGGSHAKE_WORK shake_work[G2MEMBER_MAX];
}EGGSHAKETCB_WORK;

//--------------------------------------------------------------
///	EGGDISCJUMPTCB_WORK
//--------------------------------------------------------------
typedef struct
{
	int seq_no;
	int jump_flag;		//ジャンプ中
	int jump_proc_flag;	//ジャンプ処理中
	int shake_count;
	fx32 shake_fx;
}EGGDISCJUMPTCB_WORK;

//--------------------------------------------------------------
///	BTNANMTCB_WORK
//--------------------------------------------------------------
typedef struct
{
	u16 seq_no;
	u16 push_flag;
	int frame;
}BTNANMTCB_WORK;

//--------------------------------------------------------------
///	OMAKEJUMP_WORK
//--------------------------------------------------------------
typedef struct
{
	int seq_no;
	int count;
	int wait;
	EGGACTOR *eact;
}OMAKEJUMP_WORK;

//--------------------------------------------------------------
///	OMAKEJUMPTCB_WORK
//--------------------------------------------------------------
typedef struct
{
	OMAKEJUMP_WORK omake_jump_work[G2MEMBER_MAX];
}OMAKEJUMPTCB_WORK;

//--------------------------------------------------------------
///	COMMDATA
//--------------------------------------------------------------
typedef struct
{
	int my_play_no;
	int play_max;
	int push_btn;
	int send_btn;
	int game_data_send_flag;
	int game_result_get_flag;
	int trade_no_tbl[G2MEMBER_MAX];
	GURU2COMM_GAMEDATA game_data;
	GURU2COMM_GAMERESULT game_result;
	GURU2COMM_PLAYNO play_no_tbl[G2MEMBER_MAX];
	MYSTATUS *my_status[G2MEMBER_MAX];
	STRBUF *my_name_buf[G2MEMBER_MAX];
}COMMDATA;

//--------------------------------------------------------------
///	GURU2MAIN_WORK
//--------------------------------------------------------------
struct _TAG_GURU2MAIN_WORK
{
	int seq_no;
	int save_seq;
	int game_frame;
	int frame;
	int comm_wait_frame;
	int play_send_count;
	int force_end_flag;
	int before_bgm_no;
	u32 omake_bit;
	
	POKEPARTY *my_poke_party;
		
	EGGACTOR *front_eggact;
	
	COMMDATA comm;
	
	u8 temp_work[G2M_TEMPWORK_SIZE];
	
	GURU2PROC_WORK *g2p;
	GURU2COMM_WORK *g2c;
	
	DISCWORK disc;
	EGGWORK egg;
	
	EGGJUMPTCB_WORK egg_jump_tcb_work;
	EGGSPINTCB_WORK egg_spin_tcb_work;
	EGGSHAKETCB_WORK egg_shake_tcb_work;
	EGGDISCJUMPTCB_WORK egg_disc_jump_tcb_work;
	BTNANMTCB_WORK btn_anm_tcb_work;
	OMAKEJUMPTCB_WORK omake_jump_tcb_work;
	
	MSGWORK msgwork;
	CAMERAWORK camera;
	TPWORK touch;
	ARCHANDLE *arc_handle;
	
	GF_BGL_INI *bgl;
	NNSG2dScreenData *bg_pScr;
	NNSG2dCharacterData *bg_pChar;
	NNSG2dPaletteData *bg_pPltt;
	
	CATS_SYS_PTR csp;
	CATS_RES_PTR crp;
	PALETTE_FADE_PTR pfd;
	GXRgb edge_color_tbl[EDGE_COLOR_MAX];
	
	TCB_PTR tcb_egg_jump;
	TCB_PTR tcb_egg_spin;
	TCB_PTR tcb_egg_shake;
	TCB_PTR tcb_egg_disc_jump;
	TCB_PTR tcb_btn_anm;
	TCB_PTR tcb_omake_jump;
	
	void *time_wait_icon_p;
	
#ifdef GURU2_DEBUG_ON
	DEBUGWORK debug;
#endif
};

//==============================================================================
//	proto
//==============================================================================
static void guru2Main_FrameWorkClear( GURU2MAIN_WORK *g2m );
static void guru2Main_Delete( GURU2MAIN_WORK *g2m );

static RET (* const DATA_Guru2ProcTbl[SEQNO_MAIN_MAX])( GURU2MAIN_WORK *g2m );

static void guru2_VBlankFunc( void *wk );

static void guru2_DrawInit( GURU2MAIN_WORK *g2m );
static void guru2_DrawDelete( GURU2MAIN_WORK *g2m );
static void guru2_DrawProc( GURU2MAIN_WORK *g2m );

static void guru2_DispInit( GURU2MAIN_WORK *g2m );
static void guru2_DispON( GURU2MAIN_WORK *g2m );
static void guru2_DispOFF( GURU2MAIN_WORK *g2m );

static void guru2_3DDrawInit( GURU2MAIN_WORK *g2m );

static void guru2_BGInit( GURU2MAIN_WORK *g2m );
static void guru2_BGResLoad( GURU2MAIN_WORK *g2m );
static void guru2_BGDelete( GURU2MAIN_WORK *g2m );
static void Guru2BG_ButtonONSet( GURU2MAIN_WORK *g2m );
static void Guru2BG_ButtonOFFSet( GURU2MAIN_WORK *g2m );
static void Guru2BG_ButtonMiddleSet( GURU2MAIN_WORK *g2m );

static void guru2_PlttFadeInit( GURU2MAIN_WORK *g2m );
static void guru2_PlttFadeDelete( GURU2MAIN_WORK *g2m );
static void Guru2PlttFade_BtnFade( GURU2MAIN_WORK *g2m, u32 evy );

static void guru2_ClActInit( GURU2MAIN_WORK *g2m );
static void guru2_ClActResLoad( GURU2MAIN_WORK *g2m );
static void guru2_ClActDelete( GURU2MAIN_WORK *g2m );
static CATS_ACT_PTR Guru2ClAct_Add(
	GURU2MAIN_WORK *g2m, const TCATS_OBJECT_ADD_PARAM_S *param );

static void guru2_TalkWinFontInit( GURU2MAIN_WORK *g2m );
static void guru2_TalkWinFontDelete( GURU2MAIN_WORK *g2m );
static void Guru2TalkWin_Write( GURU2MAIN_WORK *g2m, u32 msgno );
static void Guru2TalkWin_WritePlayer(
	GURU2MAIN_WORK *g2m, u32 msgno, const MYSTATUS *status );
static void Guru2TalkWin_WriteItem( GURU2MAIN_WORK *g2m, u32 msgno, u32 id );
static void Guru2TalkWin_Clear( GURU2MAIN_WORK *g2m );
static void Guru2NameWin_Init( GURU2MAIN_WORK *g2m, int max );
static void Guru2NameWin_Delete( GURU2MAIN_WORK *g2m );
static void Guru2NameWin_Write(
	GURU2MAIN_WORK *g2m, STRBUF *name, int no, u32 col );
static void Guru2NameWin_WriteIDColor(
	GURU2MAIN_WORK *g2m, STRBUF *name, int no, int id );
static void Guru2NameWin_Clear( GURU2MAIN_WORK *g2m, int no );

static void guru2_CameraInit( GURU2MAIN_WORK *g2m );
static void guru2_CameraDelete( GURU2MAIN_WORK *g2m );
static void guru2_CameraSet( GURU2MAIN_WORK *g2m );

static void guru2_TPProc( GURU2MAIN_WORK *g2m );
static BOOL Guru2TP_ButtonHitTrgCheck( GURU2MAIN_WORK *g2m );
static BOOL Guru2TP_ButtonHitContCheck( GURU2MAIN_WORK *g2m );

static void Disc_Init( GURU2MAIN_WORK *g2m );
static void Disc_Delete( GURU2MAIN_WORK *g2m );
static void Disc_Update( GURU2MAIN_WORK *g2m );
static void Disc_Draw( GURU2MAIN_WORK *g2m );
static void Disc_Rotate( DISCWORK *disc, fx32 add );

static void Egg_Init( GURU2MAIN_WORK *g2m );
static void Egg_Delete( GURU2MAIN_WORK *g2m );
static void Egg_MdlInit( GURU2MAIN_WORK *g2m );
static void Egg_MdlDelete( GURU2MAIN_WORK *g2m );
static void Egg_MdlActInit( GURU2MAIN_WORK *g2m, EGGACTOR *act );

static void EggAct_Update( GURU2MAIN_WORK *g2m );
static void EggAct_Draw( GURU2MAIN_WORK *g2m );
static void EggAct_Rotate( GURU2MAIN_WORK *g2m, fx32 add );
static void EggAct_AnglePosSet( EGGACTOR *eact, const VecFx32 *offs );
static EGGACTOR * EggAct_FrontEggActGet( GURU2MAIN_WORK *g2m );

static void EggKage_Init( GURU2MAIN_WORK *g2m, EGGACTOR *act );
static void EggKage_Update( GURU2MAIN_WORK *g2m, EGGKAGE *ekage );

static void EggCursor_Init( GURU2MAIN_WORK *g2m, EGGACTOR *act );
static void EggCursor_Update( GURU2MAIN_WORK *g2m, EGGCURSOR *ecs );

static TCB_PTR EggAct_StartTcbSet(
	GURU2MAIN_WORK *g2m, int id, int no, int name_no,
	int angle, int offs, EGGACTOR *eact );
static void EggActStartTcb( TCB_PTR tcb, void *wk );
static RET (* const DATA_EggStartTbl[SEQNO_EGGSTART_MAX])( EGGSTARTWORK *work );

static void EggJumpTcb_Init( GURU2MAIN_WORK *g2m );
static void EggJumpTcb_Delete( GURU2MAIN_WORK *g2m );
static void EggJumpTcb_EggSetAll( GURU2MAIN_WORK *g2m );
static BOOL EggJumpTcb_JumpCheck( GURU2MAIN_WORK *g2m );

static void EggSpinTcb_Init( GURU2MAIN_WORK *g2m );
static void EggSpinTcb_Delete( GURU2MAIN_WORK *g2m );
static void EggSpinTcb_EggSet( GURU2MAIN_WORK *g2m, EGGACTOR *act );

static void EggShakeTcb_Init( GURU2MAIN_WORK *g2m );
static void EggShakeTcb_Delete( GURU2MAIN_WORK *g2m );
static void EggShakeTcb_EggSet( GURU2MAIN_WORK *g2m, EGGACTOR *act );

static void EggDiscJumpTcb_Init( GURU2MAIN_WORK *g2m );
static void EggDiscJumpTcb_Delete( GURU2MAIN_WORK *g2m );
static void EggDiscJumpTcb_JumpSet( GURU2MAIN_WORK *g2m );
static BOOL EggDiscJumpTcb_JumpCheck( GURU2MAIN_WORK *g2m );
static BOOL EggDiscJumpTcb_JumpProcCheck( GURU2MAIN_WORK *g2m );

static void BtnAnmTcb_Init( GURU2MAIN_WORK *g2m );
static void BtnAnmTcb_Delete( GURU2MAIN_WORK *g2m );
static BOOL BtnAnmTcb_PushCheck( GURU2MAIN_WORK *g2m );
static void BtnAnmTcb_PushSet( GURU2MAIN_WORK *g2m );

static void OmakeEggJumpTcb_Init( GURU2MAIN_WORK *g2m );
static void OmakeEggJumpTcb_Delete( GURU2MAIN_WORK *g2m );
static void OmakeEggJumpTcb_EggSet( GURU2MAIN_WORK *g2m, EGGACTOR *act );
static void OmakeEggJumpTcb_OmakeBitCheckSet( GURU2MAIN_WORK *g2m );

static TCB_PTR guru2_CountDownTcbSet( GURU2MAIN_WORK *g2m );

static void BtnFadeTcbAdd( GURU2MAIN_WORK *g2m, int light );

static void AngleAdd( fx32 *angle, fx32 add );
static void DiscRotateEggMove( GURU2MAIN_WORK *gm, fx32 speed );
static BOOL Guru2GameTimeCount( GURU2MAIN_WORK *g2m );
static void * Guru2Arc_DataLoad( GURU2MAIN_WORK *g2m, u32 idx, BOOL fb );

static BOOL Guru2Pad_TrgCheck( u32 pad );
static BOOL Guru2Pad_ContCheck( u32 pad );
static void * Guru2MainTempWorkInit( GURU2MAIN_WORK *g2m, u32 size );
static void * Guru2MainTempWorkGet( GURU2MAIN_WORK *g2m );
static BOOL Guru2MainCommSignalCheck(GURU2MAIN_WORK *g2m, u16 bit);
//static BOOL Guru2MainCommJoinCloseCheck( GURU2MAIN_WORK *g2m );
static BOOL Guru2MainCommJoinNumCheck( GURU2MAIN_WORK *g2m );
static int Guru2MainCommJoinNumGet( GURU2MAIN_WORK *g2m );
//static BOOL Guru2MainCommOyaCancelCheck( GURU2MAIN_WORK *g2m );
static BOOL Guru2MainCommIDPlayCheck( GURU2MAIN_WORK *g2m, int id );
static void Guru2MainFriendEggExchange( GURU2MAIN_WORK *g2m, int id );
static int Guru2MainCommEggDataNumGet( GURU2MAIN_WORK *g2m );
static BOOL Guru2MainOmakeZoneCheck( GURU2MAIN_WORK *g2m, fx32 pos, int max );
static BOOL Guru2MainPokePartyDameTamagoCheck( POKEPARTY *ppty );
static BOOL Guru2MainDameTamagoCheck( GURU2MAIN_WORK *g2m );
static int Guru2MainCommEggDataOKCountCheck( GURU2MAIN_WORK *g2m );
static BOOL Guru2MainCommEggDataNGCheck( GURU2MAIN_WORK *g2m );

static const BMPWIN_DAT DATA_Guru2BmpTalkWinList[GURU2TALKWIN_MAX];

static const BMPWIN_DAT * const DATA_Guru2BmpNameWinTbl[G2MEMBER_MAX+1];
static const u16 DATA_DiscOffsetAngle[G2MEMBER_MAX+1][G2MEMBER_MAX];
static const u16 DATA_EggStartAngle[G2MEMBER_MAX+1][G2MEMBER_MAX];
//static const u16 DATA_GameEndAngle[G2MEMBER_MAX+1];
static const OMAKE_AREA_TBL DATA_OmakeAreaTbl[G2MEMBER_MAX+1];
static const u32 DATA_KinomiTbl[G2MEMBER_MAX+1][2];
static const u32 DATA_SaraArcIdxTbl[G2MEMBER_MAX+1];
static const fx32 DATA_DiscRotateDrawOffset[G2MEMBER_MAX+1];


#ifdef GURU2_DEBUG_ON
static void DEBUG_WorkInit( GURU2MAIN_WORK *g2m );
static void DEBUG_Proc( GURU2MAIN_WORK *g2m );
#endif

#ifdef DEBUG_DISP_CHECK
extern void DEBUG_DiscTest( GURU2MAIN_WORK *g2m );
#endif

//==============================================================================
//	ぐるぐる交換　メイン処理
//==============================================================================
//--------------------------------------------------------------
/**
 * ぐるぐるメイン処理初期化
 * @param	proc	PROC *
 * @param	seq		seq
 * @retval	PROC_RESULT	PROC_RES_CONTINUE,PROC_RES_FINISH
 */
//--------------------------------------------------------------
PROC_RESULT Guru2Main_Init( PROC *proc, int *seq )
{
	GURU2MAIN_WORK *g2m;
	GURU2PROC_WORK *g2p = PROC_GetParentWork( proc );
	
	//切断禁止
	CommStateSetErrorCheck( TRUE, TRUE );
	
	//H,Vブランク停止
	sys_VBlankFuncChange( NULL, NULL );
	sys_HBlankIntrStop();
	
	//ソフトリセット禁止
	sys_SoftResetNG( SOFTRESET_TYPE_TRADE );
	
	//ヒープ作成
	sys_CreateHeap( HEAPID_BASE_APP, HEAPID_GURU2, GURU2_HEAPSIZE );
	
	//ワーク確保
	g2m = PROC_AllocWork( proc, sizeof(GURU2MAIN_WORK), HEAPID_GURU2 );
	memset( g2m, 0, sizeof(GURU2MAIN_WORK) );
	
	g2p->g2m = g2m;
	g2m->g2p = g2p;
	g2m->g2c = g2p->g2c;
	
	//ポケモンパーティ
	g2m->my_poke_party = SaveData_GetTemotiPokemon( g2m->g2p->param.sv );
	
	//アーカイブハンドルオープン
	g2m->arc_handle = ArchiveDataHandleOpen( ARC_GURU2, HEAPID_GURU2 );
	
	//VRAM転送マネージャ初期化
	initVramTransferManagerHeap(
			GURU2_VRAMTRANSFER_MANAGER_NUM, HEAPID_GURU2 );
	
	//タッチパネル初期化
	InitTPSystem();
//	while( InitTPNoBuff(4) == TP_OK );
//	if( InitTPNoBuff(TP_SYNC) != TP_OK ){ GF_ASSERT(0); }
	InitTPNoBuff( TP_SYNC );
	
	//グラフィック初期化
	guru2_DrawInit( g2m );
	
	//VBlankセット
	sys_VBlankFuncChange( guru2_VBlankFunc, g2m );
	
	//アクター初期化
	Disc_Init( g2m );
	Egg_Init( g2m );
	
	{ //現在のIDと参加人数でディスク角度セット
		int id = 0, count = 0;
		int my_id = CommGetCurrentID();
		DISCWORK *disc = &g2m->disc;
		
		do{
			if( (g2m->g2p->receipt_bit & (1 << id)) ){
			 	if( id == my_id ){
					break;
				}
				
				count++;
			}
			
			id++;
		}while( id < G2MEMBER_MAX );
		
		disc->rotate_offs_fx = NUM_FX32(
			DATA_DiscOffsetAngle[g2m->g2p->receipt_num][count] );
	}
	
	//現在再生中のBGM Noを保存
	g2m->before_bgm_no = Snd_NowBgmNoGet();
	
	//タスク初期化
	EggJumpTcb_Init( g2m );
	EggSpinTcb_Init( g2m );
	EggShakeTcb_Init( g2m );
	EggDiscJumpTcb_Init( g2m );
	BtnAnmTcb_Init( g2m );
	OmakeEggJumpTcb_Init( g2m );
	
	//デバッグ
	#ifdef GURU2_DEBUG_ON
	DEBUG_WorkInit( g2m );
	#endif
	
	//ワイプ開始
	WIPE_SYS_Start( WIPE_PATTERN_WMS,
		WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN,
		WIPE_FADE_BLACK, 8, 1, HEAPID_GURU2 );
	
	return( PROC_RES_FINISH );
}

//--------------------------------------------------------------
/**
 * ぐるぐるメイン処理　終了
 * @param	proc	PROC *
 * @param	seq		seq
 * @retval	PROC_RESULT	PROC_RES_CONTINUE,PROC_RES_FINISH
 */
//--------------------------------------------------------------
PROC_RESULT Guru2Main_End( PROC *proc, int *seq )
{
	GURU2MAIN_WORK *g2m = PROC_GetWork( proc );
	
	//ワーク反映
	
	//タッチパネル終了
	if( StopTP() != TP_OK ){ GF_ASSERT(0) };
	
	//タスク削除
	EggJumpTcb_Delete( g2m );
	EggSpinTcb_Delete( g2m );
	EggShakeTcb_Delete( g2m );
	EggDiscJumpTcb_Delete( g2m );
	BtnAnmTcb_Delete( g2m );
	OmakeEggJumpTcb_Delete( g2m );
	
	//アクター削除
	Disc_Delete( g2m );
	Egg_Delete( g2m );
	
	//グラフィック削除
	guru2_DrawDelete( g2m );
	sys_VBlankFuncChange( NULL, NULL );
	DellVramTransferManager();
	
	//アーカイブクローズ
	ArchiveDataHandleClose( g2m->arc_handle );
	
	//ワーク開放
	PROC_FreeWork( proc );
	sys_DeleteHeap( HEAPID_GURU2 );
	
	//ソフトリセット有効に戻す
	sys_SoftResetOK( SOFTRESET_TYPE_TRADE );
	
	return( PROC_RES_FINISH );
}

//--------------------------------------------------------------
/**
 * メイン処理
 * @param	proc	PROC *
 * @param	seq		seq
 * @retval	PROC_RESULT	PROC_RES_CONTINUE,PROC_RES_FINISH
 */
//--------------------------------------------------------------
PROC_RESULT Guru2Main_Main( PROC *proc, int *seq )
{
	RET ret;
	GURU2MAIN_WORK *g2m = PROC_GetWork( proc );
	
	guru2_TPProc( g2m );
	
	do{
		#ifdef GURU2_DEBUG_ON
		DEBUG_Proc( g2m );
		#endif
		ret = DATA_Guru2ProcTbl[g2m->seq_no]( g2m );
	}while( ret == RET_CONT );
	
	if( ret == RET_END ){
		return( PROC_RES_FINISH );
	}
	
	Disc_Update( g2m );
	EggAct_Update( g2m );
	
	guru2_DrawProc( g2m );
	guru2Main_FrameWorkClear( g2m );
	return( PROC_RES_CONTINUE );
}

//--------------------------------------------------------------
/**
 * 1フレーム毎にクリアするワーク
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void guru2Main_FrameWorkClear( GURU2MAIN_WORK *g2m )
{
	g2m->comm.push_btn = 0;
	g2m->comm.game_data_send_flag = 0;
}

//==============================================================================
//	ぐるぐる交換　メイン処理
//==============================================================================
//--------------------------------------------------------------
/**
 * メイン　初期化
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_Init( GURU2MAIN_WORK *g2m )
{
	Guru2TalkWin_Write( g2m, MSG_WAIT );
	g2m->seq_no = SEQNO_MAIN_FADEIN_WAIT;
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン 親子共通 フェードイン待ち
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_FadeInWait( GURU2MAIN_WORK *g2m )
{
	if( WIPE_SYS_EndCheck() ){
		if( CommGetCurrentID() == 0 ){
			g2m->seq_no = SEQNO_MAIN_OYA_SIGNAL_JOIN_WAIT;
		}else{
			g2m->seq_no = SEQNO_MAIN_KO_SEND_SIGNAL_JOIN;
		}
		
		return( RET_CONT );
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　親　参加待ち
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_OyaSignalJoinWait( GURU2MAIN_WORK *g2m )
{
	if( Guru2MainCommJoinNumCheck(g2m) ){		//参加数チェック
		#ifdef DEBUG_GURU2_PRINTF
		OS_Printf( "ぐるぐる 親 参加待ち 全員参加\n" );
		#endif
		g2m->seq_no = SEQNO_MAIN_OYA_SEND_JOIN_CLOSE;
		return( RET_CONT );
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　親　参加締め切り
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_OyaSendJoinClose( GURU2MAIN_WORK *g2m )
{
	u16 bit = G2COMM_GMSBIT_JOIN_CLOSE;
	
	if( Guru2Comm_SendData(g2m->g2c,G2COMM_GM_SIGNAL,&bit,2) == TRUE ){
		#ifdef DEBUG_GURU2_PRINTF
		OS_Printf( "ぐるぐる 親 参加締め切り\n" );
		#endif
		g2m->seq_no = SEQNO_MAIN_OYA_CONNECT_NUM_CHECK;
		return( RET_CONT );
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　親　接続人数一致待ち
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_OyaConnectNumCheck( GURU2MAIN_WORK *g2m )
{
	int count = Guru2MainCommJoinNumGet( g2m ) + 1; //+1=自身
	
	if( count != CommGetConnectNum() ){
		return( RET_NON );
	}
	
	g2m->seq_no = SEQNO_MAIN_OYA_SEND_PLAY_MAX;
	return( RET_CONT );
}

//--------------------------------------------------------------
/**
 * メイン　親　プレイ人数最大を転送
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_OyaSendPlayMax( GURU2MAIN_WORK *g2m )
{
	int ret;
	
	g2m->comm.play_max = Guru2MainCommJoinNumGet(g2m) + 1; //+1=自身
	
	ret = Guru2Comm_SendData(
		g2m->g2c, G2COMM_GM_PLAYMAX, &g2m->comm.play_max, 4 );
	
	if( ret == TRUE ){
		g2m->play_send_count = 0;
		g2m->seq_no = SEQNO_MAIN_OYA_SEND_PLAY_NO;
		#ifdef DEBUG_GURU2_PRINTF_FORCE
		OS_Printf( "ぐるぐる 参加数 max=%d\n", g2m->comm.play_max );
		#endif
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　親　ゲーム番号転送
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_OyaSendPlayNo( GURU2MAIN_WORK *g2m )
{
	int ret;
	GURU2COMM_PLAYNO play;
	
	if( g2m->play_send_count == 0 ){	//初回 親
		play.comm_id = 0;
		play.play_no = 0;
		
		ret = Guru2Comm_SendData(g2m->g2c,
				G2COMM_GM_PLAYNO,&play,sizeof(GURU2COMM_PLAYNO) );
		
		if( ret == TRUE ){
			g2m->play_send_count++;
			
			#ifdef DEBUG_GURU2_PRINTF_FORCE
			OS_Printf( "ぐるぐる 子へ親プレイ番号(%d)転送\n",
				play.comm_id, play.play_no );
			#endif
		}
		
		return( RET_NON );
	}
	
#if	0
	{	//子プレイデータ送信
		int id = 1, count = 1;
		u32 join = g2m->g2c->comm_game_join_bit;
		
		while( join ){
			if( (join&0x01) ){
				if( count >= g2m->play_send_count ){
					play.comm_id = id;
					play.play_no = g2m->play_send_count;
					
					ret = Guru2Comm_SendData(g2m->g2c,
						G2COMM_GM_PLAYNO,&play,sizeof(GURU2COMM_PLAYNO) );
					
					if( ret == TRUE ){
						g2m->play_send_count++;
						#ifdef DEBUG_GURU2_PRINTF_FORCE
						OS_Printf( "ぐるぐる 子(ID:%d)へ", id );
						OS_Printf( "プレイ番号転送 no=%d\n", count );
						#endif
					}
					
					return( RET_NON );
				}
				
				count++;
			}
			
			id++;
			join >>= 1;
		}
	}
#else
	{	//子プレイデータ送信
		int id = 1, count = 1;
		u32 join = g2m->g2c->comm_game_join_bit;
		
		do{
			if( (join & (1<<id)) ){
				if( count >= g2m->play_send_count ){
					play.comm_id = id;
					play.play_no = g2m->play_send_count;
					ret = Guru2Comm_SendData(g2m->g2c,
						G2COMM_GM_PLAYNO,&play,sizeof(GURU2COMM_PLAYNO) );
					
					if( ret == TRUE ){
						g2m->play_send_count++;
						#ifdef DEBUG_GURU2_PRINTF_FORCE
						OS_Printf( "ぐるぐる 子(ID:%d)へ", id );
						OS_Printf( "プレイ番号転送 no=%d\n", count );
						#endif
					}
					
					break;
				}
				
				count++;
			}
			
			id++;
		}while( id < G2MEMBER_MAX );
	}
#endif
	
	if( g2m->play_send_count >= g2m->comm.play_max ){
		g2m->seq_no = SEQNO_MAIN_OYA_SIGNAL_EGG_ADD_START;
	}
	
#if 0
	0 1 2 3 4 5
	1 2 3 4 5
#endif
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　親　タマゴ追加開始シグナル転送
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_OyaSignalEggAddStart( GURU2MAIN_WORK *g2m )
{
	u16 bit = G2COMM_GMSBIT_EGG_ADD_START;
	
	if( Guru2Comm_SendData(g2m->g2c,G2COMM_GM_SIGNAL,&bit,2) == TRUE ){
		g2m->seq_no = SEQNO_MAIN_EGG_DATA_SEND_INIT; //タマゴ追加へ
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　子　参加シグナル送信
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_KoSendSignalJoin( GURU2MAIN_WORK *g2m )
{
	int ret;
	
	ret = Guru2Comm_SendData( g2m->g2c, G2COMM_GM_JOIN, NULL, 0 );
	
	if( ret == TRUE ){
		g2m->comm_wait_frame = 0; //エラー時間を一旦クリア
		g2m->seq_no = SEQNO_MAIN_KO_EGG_ADD_START_WAIT;
		return( RET_NON );
	}
	
	#if 0
	{
		g2m->comm_wait_frame++;
		if( g2m->comm_wait_frame >= COMM_WAIT_ERROR_FRAME ){ //待ち時間超え
			g2m->force_end_flag = TRUE;
			g2m->seq_no = SEQNO_MAIN_KO_SEND_JOIN_TIME_OVER_CANCEL;
			#ifdef DEBUG_GURU2_PRINTF
			OS_Printf( "ぐるぐる　子　時間切れによるキャンセル\n" );
			#endif
			return( RET_CONT );
		}
	}
	#endif
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　子　タマゴ追加開始待ち
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_KoEggAddStartWait( GURU2MAIN_WORK *g2m )
{
	if( Guru2MainCommSignalCheck(g2m,G2COMM_GMSBIT_EGG_ADD_START) == TRUE ){
		GF_ASSERT( g2m->comm.play_max >= 2 );
		GF_ASSERT( g2m->comm.my_play_no != 0 );
		g2m->seq_no = SEQNO_MAIN_EGG_DATA_SEND_INIT;
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　たまごデータ転送　初期化
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_EggDataSendInit( GURU2MAIN_WORK *g2m )
{
	#ifdef DEBUG_GURU2_PRINTF
	OS_Printf( "ぐるぐる　たまごデータ転送開始\n" );
	#endif
	
	CommTimingSyncStart( COMM_GURU2_TIMINGSYNC_NO );
	g2m->seq_no = SEQNO_MAIN_EGG_DATA_SEND_TIMING_WAIT;
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　たまごデータ転送　同期待ち
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_EggDataSendTimingWait( GURU2MAIN_WORK *g2m )
{
	if( CommIsTimingSync(COMM_GURU2_TIMINGSYNC_NO) ){
		g2m->seq_no = SEQNO_MAIN_EGG_DATA_TRADE_POS_SEND;
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　タマゴ位置を送信
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_EggDataTradePosSend( GURU2MAIN_WORK *g2m )
{
	if( Guru2Comm_SendData(
			g2m->g2c,G2COMM_GM_TRADE_POS,&g2m->g2p->trade_no,4) ){
		g2m->seq_no = SEQNO_MAIN_EGG_DATA_SEND;
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　たまごデータ転送
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_EggDataSend( GURU2MAIN_WORK *g2m )
{
	int ret;
	u32 send;
	
	send = (u32)g2m->my_poke_party;
	
	ret = CommSendHugeData(
		CG_GURU2_EGG, (void*)send, POKEPARTY_SEND_ONCE_SIZE );
	
	if( ret ){
		g2m->seq_no = SEQNO_MAIN_EGG_DATA_RECV_WAIT;
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　たまごデータ受信待ち
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_EggDataRecvWait( GURU2MAIN_WORK *g2m )
{
	if( Guru2MainCommEggDataNumGet(g2m) == g2m->comm.play_max ){
		//タマゴデータ受信完了　ダメタマゴチェック
		if( Guru2MainDameTamagoCheck(g2m) == TRUE ){
			Guru2Comm_SendData( g2m->g2c, G2COMM_GM_EGG_DATA_NG, NULL, 0 );
			g2m->seq_no = SEQNO_MAIN_COMM_ERROR_DAME_TAMAGO_MSG;
			#ifdef DEBUG_GURU2_PRINTF
			OS_Printf( "ダメタマゴを発見\n" );
			#endif
		}else{
			Guru2Comm_SendData( g2m->g2c, G2COMM_GM_EGG_DATA_OK, NULL, 0 );
			g2m->seq_no = SEQNO_MAIN_EGG_DATA_CHECK_WAIT;
			#ifdef DEBUG_GURU2_PRINTF
			OS_Printf( "ぐるぐる　タマゴデータ受信完了\n" );
			#endif
		}
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　全員の卵チェック終了待ち
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_DameTamagoCheckWait( GURU2MAIN_WORK *g2m )
{
	if( Guru2MainCommEggDataNGCheck(g2m) == TRUE ){
		g2m->seq_no = SEQNO_MAIN_COMM_ERROR_DAME_TAMAGO_MSG;
	}else if( Guru2MainCommEggDataOKCountCheck(g2m) == g2m->comm.play_max ){
		g2m->seq_no = SEQNO_MAIN_EGG_ADD_INIT;
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　卵追加初期化
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_EggAddInit( GURU2MAIN_WORK *g2m )
{
	int i;
	EGGADDWORK *wk = Guru2MainTempWorkInit( g2m, sizeof(EGGADDWORK) );
	
	wk->no = g2m->comm.my_play_no;	//自身の卵から追加
	wk->max = g2m->comm.play_max;
	wk->offs = DATA_DiscOffsetAngle[wk->max][g2m->comm.my_play_no];
	
	//BTS:0187 まわす数が足りてなかったので通信最大値に変更
	for( i = 0; i < COMM_MACHINE_MAX; i++ ){
		if( Guru2MainCommIDPlayCheck(g2m,i) ){
			g2m->comm.my_status[i] = CommInfoGetMyStatus( i );
			MyStatus_CopyNameString(
				g2m->comm.my_status[i], g2m->comm.my_name_buf[i] );
		}
	}
	
	g2m->disc.rotate_offs_fx = NUM_FX32( wk->offs );
	g2m->seq_no = SEQNO_MAIN_EGG_ADD;
	
	//BGM切り替え
	Snd_DataSetByScene( SND_SCENE_GURUGURU, SEQ_PL_GURUGURU, 1 );
	
	Guru2TalkWin_Clear( g2m );
	return( RET_CONT );
}

//--------------------------------------------------------------
/**
 * メイン　卵追加
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_EggAdd( GURU2MAIN_WORK *g2m )
{
	g2m->frame--;
	
	if( g2m->frame > 0 ){
		return( RET_NON );
	}
	
	g2m->frame = EGG_ADD_NEXT_FRAME;
	
	{
		EGGADDWORK *wk = Guru2MainTempWorkGet( g2m );
		u16 angle = DATA_EggStartAngle[wk->max][wk->no];
		int id = g2m->comm.play_no_tbl[wk->no].comm_id;
		
		EggAct_StartTcbSet( g2m, id, wk->no, wk->count,
				angle, wk->offs, &g2m->egg.eact[wk->no] );
		
		wk->no++;
		wk->no %= wk->max; 
		wk->count++; 							//タマゴ追加カウント
		
		if( wk->count == wk->max ){
			g2m->frame = 0;
			g2m->seq_no = SEQNO_MAIN_EGG_ADD_WAIT;
		}
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　卵追加待ち
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_EggAddWait( GURU2MAIN_WORK *g2m )
{
	EGGWORK *egg = &g2m->egg;
	int i = 0,count = 0,max = g2m->comm.play_max;
	
	do{
		if( egg->eact[i].set_flag == TRUE ){
			count++;
		}
		
		i++;
	}while( i < max );
	
	if( count == max ){
		g2m->seq_no = SEQNO_MAIN_EGG_ADD_END_WAIT;
		return( RET_CONT );
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　卵追加後の一寸待ち
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_EggAddEndWait( GURU2MAIN_WORK *g2m )
{
	g2m->frame++;
	
	if( g2m->frame > EGG_ADD_END_WAIT ){
		g2m->frame = 0;
		
		if( CommGetCurrentID() == 0 ){
			g2m->seq_no = SEQNO_MAIN_SEND_GAME_START_FLAG;
		}else{
			g2m->seq_no = SEQNO_MAIN_RECV_GAME_START_FLAG;
		}
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　親　ゲーム開始フラグ転送
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_SendGameStartFlag( GURU2MAIN_WORK *g2m )
{
	u16 bit = G2COMM_GMSBIT_GAME_START;
	
	if( Guru2Comm_SendData(g2m->g2c,G2COMM_GM_SIGNAL,&bit,2) == TRUE ){
		#ifdef DEBUG_DISP_CHECK
		g2m->seq_no = SEQNO_MAIN_DEBUG_DISP_CHECK;
		#else
		g2m->seq_no = SEQNO_MAIN_COUNTDOWN_BEFORE_TIMING_INIT;
		#endif
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　子　ゲーム開始フラグ待ち
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_RecvGameStartFlag( GURU2MAIN_WORK *g2m )
{
	if( Guru2MainCommSignalCheck(g2m,G2COMM_GMSBIT_GAME_START) == TRUE ){
		#ifdef DEBUG_DISP_CHECK
		{
			int i;
			for( i = 0; i < g2m->comm.play_max; i++ ){
				Guru2NameWin_Clear( g2m, i );
			}
			BtnFadeTcbAdd( g2m, TRUE );
			g2m->seq_no = SEQNO_MAIN_DEBUG_DISP_CHECK;
		}
		#else
		g2m->seq_no = SEQNO_MAIN_COUNTDOWN_BEFORE_TIMING_INIT;
		#endif
		return( RET_CONT );
	}
	
	return( RET_NON );
}

#ifdef DEBUG_DISP_CHECK
//--------------------------------------------------------------
/**
 * メイン　子　デバッグ　画面テスト
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET GURU2RET
 */
//--------------------------------------------------------------
static RET DEBUG_Guru2Proc_DispCheck( GURU2MAIN_WORK *g2m )
{
	if( BtnAnmTcb_PushCheck(g2m) == FALSE ){
		if( Guru2TP_ButtonHitTrgCheck(g2m) == TRUE ){
			BtnAnmTcb_PushSet( g2m );
		}
	}
	
	if( Guru2Pad_ContCheck(PAD_BUTTON_START) ){
		g2m->seq_no = SEQNO_MAIN_COUNTDOWN_BEFORE_TIMING_INIT;
		return( RET_CONT );
	}
	
	return( RET_NON );
}
#endif

//--------------------------------------------------------------
/**
 * メイン　カウントダウン直前の同期開始
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_CountDownBeforeTimingInit( GURU2MAIN_WORK *g2m )
{
	CommTimingSyncStart( COMM_GURU2_TIMINGSYNC_NO );
	g2m->seq_no = SEQNO_MAIN_COUNTDOWN_BEFORE_TIMING_WAIT;
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　カウントダウン直前の同期待ち
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_CountDownBeforeTimingWait( GURU2MAIN_WORK *g2m )
{
	if( CommIsTimingSync(COMM_GURU2_TIMINGSYNC_NO) ){
		g2m->seq_no = SEQNO_MAIN_COUNTDOWN_INIT;
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　カウントダウン初期化
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_CountDownInit( GURU2MAIN_WORK *g2m )
{
	BtnFadeTcbAdd( g2m, TRUE );
	guru2_CountDownTcbSet( g2m );
	
	g2m->seq_no = SEQNO_MAIN_COUNTDOWN;
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　カウントダウン
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_CountDown( GURU2MAIN_WORK *g2m )
{
	int i;
	
	g2m->frame++;
	
	if( g2m->frame < (30*3+5) ){
		return( RET_NON );
	}
	
	for( i = 0; i < g2m->comm.play_max; i++ ){
		Guru2NameWin_Clear( g2m, i );
	}
	
	g2m->frame = 0;
	g2m->seq_no = SEQNO_MAIN_GAME_INIT;
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　ゲーム　初期化
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_GameInit( GURU2MAIN_WORK *g2m )
{
	if( CommGetCurrentID() == 0 ){
		g2m->seq_no = SEQNO_MAIN_GAME_OYA;
	}else{
		g2m->seq_no = SEQNO_MAIN_GAME_OYA;
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　親　ゲーム
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_GameOya( GURU2MAIN_WORK *g2m )
{
	{
		BOOL ret = Guru2GameTimeCount( g2m );
		
		if( ret == TRUE && EggDiscJumpTcb_JumpProcCheck(g2m) == FALSE ){
			g2m->seq_no = SEQNO_MAIN_GAME_END_INIT;
			return( RET_CONT );
		}
	}
	
	if( g2m->comm.push_btn ){
		if( EggDiscJumpTcb_JumpProcCheck(g2m) == FALSE ){
			EggDiscJumpTcb_JumpSet( g2m );
		}
	}
	
	if( BtnAnmTcb_PushCheck(g2m) == FALSE ){
		if( Guru2TP_ButtonHitTrgCheck(g2m) == TRUE ){
			if( EggDiscJumpTcb_JumpProcCheck(g2m) == FALSE ){
				EggDiscJumpTcb_JumpSet( g2m );
				g2m->comm.send_btn = TRUE;
			}
			
			BtnAnmTcb_PushSet( g2m );
		}
	}
	
	if( g2m->comm.send_btn ){
		if( Guru2Comm_SendData(g2m->g2p->g2c,
				G2COMM_GM_BTN,&g2m->comm.send_btn,1) == TRUE ){
			g2m->comm.send_btn = 0;
		}
	}
	
	{	//皿回し
		fx32 speed;
		DISCWORK *disc = &g2m->disc;
		
		if( g2m->game_frame < GURU2_GAME_FRAME_H ){
			disc->speed_fx += DISC_ACCEL_FX;
			if( disc->speed_fx >= DISC_TOP_SPEED_FX ){
				disc->speed_fx = DISC_TOP_SPEED_FX;
			}
		}else{
			disc->speed_fx -= DISC_ACCEL_FX;
			if( disc->speed_fx < DISC_LOW_SPEED_FX ){
				disc->speed_fx = DISC_LOW_SPEED_FX;
			}
		}
		
		speed = disc->speed_fx;
		
		if( EggDiscJumpTcb_JumpCheck(g2m) == TRUE ){
			Disc_Rotate( &g2m->disc, (speed)<<1 );
		}else{
			DiscRotateEggMove( g2m, speed );
		}
	}
	
	{
		GURU2COMM_GAMEDATA data;
		
		data.game_time = g2m->game_frame;
		data.disc_speed = FX32_NUM( g2m->disc.speed_fx );
		data.disc_angle = FX32_NUM( g2m->disc.rotate_fx );
		
		Guru2Comm_SendData( g2m->g2c,
			G2COMM_GM_GAMEDATA, &data, sizeof(GURU2COMM_GAMEDATA) );
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　子　ゲーム
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_GameKo( GURU2MAIN_WORK *g2m )
{
	if( g2m->comm.game_data_send_flag == TRUE ){	//ゲームデータ受信
		g2m->disc.rotate_fx = NUM_FX32( g2m->comm.game_data.disc_angle );
		g2m->disc.speed_fx = NUM_FX32( g2m->comm.game_data.disc_speed );
		g2m->game_frame = g2m->comm.game_data.game_time;
	}
	
	{
		BOOL ret = Guru2GameTimeCount( g2m );
		
		if( ret == TRUE && EggDiscJumpTcb_JumpProcCheck(g2m) == FALSE ){
			g2m->seq_no = SEQNO_MAIN_GAME_END_INIT;
			return( RET_CONT );
		}
	}
	
	if( g2m->comm.push_btn ){
		if( EggDiscJumpTcb_JumpProcCheck(g2m) == FALSE ){
			EggDiscJumpTcb_JumpSet( g2m );
		}
	}
	
	if( BtnAnmTcb_PushCheck(g2m) == FALSE ){
		if( Guru2TP_ButtonHitTrgCheck(g2m) == TRUE ){
			if( EggDiscJumpTcb_JumpProcCheck(g2m) == FALSE ){
				EggDiscJumpTcb_JumpSet( g2m );
				g2m->comm.send_btn = TRUE;
			}
			
			BtnAnmTcb_PushSet( g2m );
		}
	}
	
	if( g2m->comm.send_btn ){
		if( Guru2Comm_SendData(g2m->g2p->g2c,
				G2COMM_GM_BTN,&g2m->comm.send_btn,1) == TRUE ){
			g2m->comm.send_btn = 0;
		}
	}
	
	if( EggDiscJumpTcb_JumpCheck(g2m) == TRUE ){
		Disc_Rotate( &g2m->disc, (g2m->disc.speed_fx)<<1 );
	}else{
		DiscRotateEggMove( g2m, g2m->disc.speed_fx );
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　ゲーム終了　初期化
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_GameEndInit( GURU2MAIN_WORK *g2m )
{
	BtnFadeTcbAdd( g2m, FALSE );
	
	g2m->front_eggact = EggAct_FrontEggActGet( g2m );
	
	if( g2m->front_eggact->comm_id == CommGetCurrentID() ){
		g2m->seq_no = SEQNO_MAIN_GAME_END_ERROR_ROTATE;
	}else{
		g2m->seq_no = SEQNO_MAIN_GAME_END_LAST_ROTATE;
	}
	
	Snd_SePlay( GURU2_SE_TIMEUP	);	
	return( RET_CONT );
}

//--------------------------------------------------------------
/**
 * メイン　ゲーム終了　自タマによる回転
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_GameEndErrorRotate( GURU2MAIN_WORK *g2m )
{
	u32 front;
	EGGACTOR *eact = g2m->front_eggact;
	
	DiscRotateEggMove( g2m, DISC_LOW_SPEED_FX );
	g2m->front_eggact = EggAct_FrontEggActGet( g2m );
	
	if( g2m->front_eggact->comm_id != CommGetCurrentID() ){
		g2m->seq_no = SEQNO_MAIN_GAME_END_LAST_ROTATE;
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　ゲーム終了　最後の回転
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_GameEndLastRotate( GURU2MAIN_WORK *g2m )
{
	int max,no;
	fx32 angle,front,res,speed;
	EGGACTOR *eact = g2m->front_eggact;
	
	no = g2m->comm.my_play_no;
	max = g2m->comm.play_max;
	speed = DISC_LOW_SPEED_FX;
	
	angle = eact->angle;
	front = NUM_FX32( DATA_EggStartAngle[max][no] );
	
	res = front - angle;
	
	if( res < 0 ){
		res = (front + NUM_FX32(360)) - angle;
	}
	
	if( res <= DISC_LAST_RANGE_FX ){
		speed >>= 1;
	}
	
	if( res < speed ){
		speed = res;
	}
	
	Disc_Rotate( &g2m->disc, speed );
	EggAct_Rotate( g2m, speed );
	
	AngleAdd( &angle, speed );
	
	if( FX32_NUM(angle) == FX32_NUM(front) ){
		g2m->seq_no = SEQNO_MAIN_GAME_END_TIMING_INIT;
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　ゲーム終了　通信同期開始
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_GameEndTimingInit( GURU2MAIN_WORK *g2m )
{
	CommTimingSyncStart( COMM_GURU2_TIMINGSYNC_NO );
	g2m->seq_no = SEQNO_MAIN_GAME_END_TIMING_WAIT;
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　ゲーム終了　通信同期
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_GameEndTimingWait( GURU2MAIN_WORK *g2m )
{
	if( CommIsTimingSync(COMM_GURU2_TIMINGSYNC_NO) ){
		if( CommGetCurrentID() == 0 ){
			g2m->seq_no = SEQNO_MAIN_GAME_END_OYA_DATA_SEND;
		}else{
			g2m->seq_no = SEQNO_MAIN_GAME_END_KO_DATA_RECV;
		}
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　親　ゲーム終了　ゲーム結果を送信
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_OyaGameEndDataSend( GURU2MAIN_WORK *g2m )
{
	int i,ret;
	EGGACTOR *eact;
	GURU2COMM_GAMERESULT result;
	
	result.disc_angle = FX32_NUM( g2m->disc.rotate_fx );
	
	for( i = 0, g2m->omake_bit = 0; i < g2m->comm.play_max; i++ ){
		eact = &g2m->egg.eact[i];
		GF_ASSERT( eact->use_flag );
		result.egg_angle[eact->play_no] = (u16)FX32_NUM( eact->angle );
		
		#ifdef DEBUG_GURU2_PRINTF_FORCE
		OS_Printf( "Guru2Result OYA Egg No %d, play_no %d, comm_id %d, angle %d\n", i, eact->play_no, eact->comm_id, result.egg_angle[eact->play_no] );
		#endif
		
		if( Guru2MainOmakeZoneCheck(
			g2m,eact->angle,g2m->comm.play_max) == TRUE ){
			g2m->omake_bit |= (1 << eact->comm_id);
		}
	}
	
	result.omake_bit = g2m->omake_bit;
	
	ret = Guru2Comm_SendData( g2m->g2c,
		G2COMM_GM_GAMERESULT, &result, sizeof(GURU2COMM_GAMERESULT) );
	
	if( ret == TRUE ){
		g2m->seq_no = SEQNO_MAIN_RESULT_INIT;
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　子　ゲーム終了　親からゲーム結果受け取り
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_KoGameEndDataRecv( GURU2MAIN_WORK *g2m )
{
	if( g2m->comm.game_result_get_flag == TRUE ){
		int i;
		EGGACTOR *eact;
		GURU2COMM_GAMERESULT *res = &g2m->comm.game_result;
		
		g2m->disc.rotate_fx = NUM_FX32( res->disc_angle );
		
		for( i = 0; i < g2m->comm.play_max; i++ ){
			eact = &g2m->egg.eact[i];
			GF_ASSERT( eact->use_flag );
			eact->angle = NUM_FX32( res->egg_angle[eact->play_no] );
			
			#ifdef DEBUG_GURU2_PRINTF_FORCE
			OS_Printf( "Guru2Result KO Egg No %d, play_no %d, comm_id %d, angle %d\n", i, eact->play_no, eact->comm_id, res->egg_angle[eact->play_no] );
			#endif
		}
		
		g2m->omake_bit = res->omake_bit;
		
		Disc_Update( g2m );		//皿更新
		EggAct_Update( g2m );	//タマゴ更新
		g2m->front_eggact = EggAct_FrontEggActGet( g2m ); //正面タマゴ取得
		
		g2m->seq_no = SEQNO_MAIN_RESULT_INIT;
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　結果発表　名前表示
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_ResultInit( GURU2MAIN_WORK *g2m )
{
	int i,id;
	int no = g2m->front_eggact->play_no;
	
	for( i = 0; i < g2m->comm.play_max; i++ ){
		id = g2m->egg.eact[no].comm_id;
		Guru2NameWin_WriteIDColor( g2m, g2m->comm.my_name_buf[id], i, id );
		
		no++;
		no %= g2m->comm.play_max;
	}
	
	g2m->frame = 0;
	g2m->seq_no = SEQNO_MAIN_RESULT_NAME_WAIT;
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　結果発表　名前表示待ち
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_ResultNameWait( GURU2MAIN_WORK *g2m )
{
	g2m->frame++;
	
	if( g2m->frame < GURU2_RESULT_NAME_WAIT_FRAME ){
		return( RET_NON );
	}
	
	g2m->frame = 0;
	
	{
		int i;
		
		for( i = 0; i < g2m->comm.play_max; i++ ){
			Guru2NameWin_Clear( g2m, i );
		}
	}
	
	{
		int id = g2m->front_eggact->comm_id;
		Guru2TalkWin_WritePlayer(
			g2m, msg_guru2_02, g2m->comm.my_status[id] );
	}
	
	{	//卵取得ファンファーレ
		Snd_MePlay( SEQ_FANFA4 );
	}
	
	g2m->seq_no = SEQNO_MAIN_RESULT_MSG_WAIT;
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　結果発表　結果メッセージ表示待ち
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_ResultMsgWait( GURU2MAIN_WORK *g2m )
{
	if( g2m->frame < GURU2_RESULT_MSG_WAIT_FRAME ){
		g2m->frame++;
	}else if( Snd_MePlayCheckBgmPlay() == FALSE ){
		g2m->frame = 0;
		g2m->seq_no = SEQNO_MAIN_OMAKE_CHECK;
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　おまけエリア チェック
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_OmakeAreaCheck( GURU2MAIN_WORK *g2m )
{
	OmakeEggJumpTcb_OmakeBitCheckSet( g2m );
	
	if( (g2m->omake_bit & (1 << g2m->front_eggact->comm_id)) == 0 ){
		g2m->seq_no = SEQNO_MAIN_SAVE_BEFORE_TIMING_INIT;
		return( RET_NON );
	}
	
	{
		u32 id;
		BOOL ret;
		
		id = DATA_KinomiTbl[g2m->comm.play_max][0];
		id += gf_rand() % (DATA_KinomiTbl[g2m->comm.play_max][1] - id + 1);
		
		if( id < DATA_KinomiTbl[g2m->comm.play_max][0] ){	//念の為
			#ifdef DEBUG_GURU2_PRINTF 			
			OS_Printf( "木の実の値が変\n" );
			#endif
			id = DATA_KinomiTbl[g2m->comm.play_max][0];
		}else if( id > DATA_KinomiTbl[g2m->comm.play_max][1] ){
			#ifdef DEBUG_GURU2_PRINTF 			
			OS_Printf( "木の実の値が変\n" );
			#endif
			id = DATA_KinomiTbl[g2m->comm.play_max][1];
		}
		
		Guru2TalkWin_WriteItem( g2m, MSG_OMAKE_AREA, id );
		
		ret = MyItem_AddItem(
			SaveData_GetMyItem(g2m->g2p->param.sv), id, 1, HEAPID_GURU2 ); 
		
		Snd_MePlay( SEQ_FANFA4 );	//ファンファーレ
		
		if( ret == TRUE ){	//成功
			g2m->seq_no = SEQNO_MAIN_OMAKE_MSG_WAIT;
		}else{				//失敗
			g2m->seq_no = SEQNO_MAIN_OMAKE_ERROR_MSG_START_WAIT;
		}
	}
	
	return( RET_NON );
}

#if 0 // old
static RET Guru2Proc_OmakeAreaCheck( GURU2MAIN_WORK *g2m )
{
	OmakeEggJumpTcb_AllSet( g2m );
	
	{
		fx32 pos = g2m->front_eggact->angle;
		
		if( Guru2MainOmakeZoneCheck(g2m,pos,g2m->comm.play_max) == FALSE ){
			g2m->seq_no = SEQNO_MAIN_SAVE_BEFORE_TIMING_INIT;
			return( RET_NON );
		}
	}
	
	{
		u32 id;
		BOOL ret;
		
		id = DATA_KinomiTbl[g2m->comm.play_max][0];
		id += gf_rand() % (DATA_KinomiTbl[g2m->comm.play_max][1] - id + 1);
		
		if( id < DATA_KinomiTbl[g2m->comm.play_max][0] ){	//念の為
			#ifdef DEBUG_GURU2_PRINTF 			
			OS_Printf( "木の実の値が変\n" );
			#endif
			id = DATA_KinomiTbl[g2m->comm.play_max][0];
		}else if( id > DATA_KinomiTbl[g2m->comm.play_max][1] ){
			#ifdef DEBUG_GURU2_PRINTF 			
			OS_Printf( "木の実の値が変\n" );
			#endif
			id = DATA_KinomiTbl[g2m->comm.play_max][1];
		}
		
		Guru2TalkWin_WriteItem( g2m, MSG_OMAKE_AREA, id );
		
		ret = MyItem_AddItem(
			SaveData_GetMyItem(g2m->g2p->param.sv), id, 1, HEAPID_GURU2 ); 
		
		Snd_MePlay( SEQ_FANFA4 );	//ファンファーレ
		
		if( ret == TRUE ){	//成功
			g2m->seq_no = SEQNO_MAIN_OMAKE_MSG_WAIT;
		}else{				//失敗
			g2m->seq_no = SEQNO_MAIN_OMAKE_ERROR_MSG_START_WAIT;
		}
	}
	
	return( RET_NON );
}
#endif

//--------------------------------------------------------------
/**
 * メイン　おまけエリア　木の実取得後のメッセージ表示待ち
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_OmakeAreaMsgWait( GURU2MAIN_WORK *g2m )
{
	if( g2m->frame < GURU2_MSG_WAIT_FRAME ){
		g2m->frame++;
	}else if( Snd_MePlayCheckBgmPlay() == FALSE ){
		g2m->frame = 0;
		g2m->seq_no = SEQNO_MAIN_SAVE_BEFORE_TIMING_INIT;
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　おまけエリア　木の実取得失敗メッセージ表示開始
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_OmakeAreaErrorMsgStartWait( GURU2MAIN_WORK *g2m )
{
	if( g2m->frame < GURU2_MSG_WAIT_FRAME ){
		g2m->frame++;
	}else if( Snd_MePlayCheckBgmPlay() == FALSE ){
		g2m->frame = 0;
		g2m->seq_no = SEQNO_MAIN_OMAKE_MSG_WAIT;
		Guru2TalkWin_Write( g2m, MSG_ZANNEN );
	}	
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　おまけエリア　木の実取得失敗メッセージ表示待ち
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_OmakeAreaErrorMsgWait( GURU2MAIN_WORK *g2m )
{
	if( g2m->frame < GURU2_MSG_WAIT_FRAME ){
		g2m->frame++;
	}else{
		g2m->frame = 0;
		g2m->seq_no = SEQNO_MAIN_SAVE_BEFORE_TIMING_INIT;
	}	
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　セーブ　セーブ前の同期初期化
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_SaveBeforeTimingInit( GURU2MAIN_WORK *g2m )
{
	void *FnoteWork;
	
	FnoteWork = FNOTE_SioIDOnlyDataMake( HEAPID_GURU2, FNOTE_ID_PL_GURUGURU );
	FNOTE_DataSave( g2m->g2p->param.fnote, FnoteWork, FNOTE_TYPE_SIO );
	
	RECORD_Inc( g2m->g2p->param.record, RECID_GURUGURU_COUNT );
	RECORD_Score_Add( g2m->g2p->param.record,SCORE_ID_GURUGURU );
	
	Guru2TalkWin_Write( g2m, MSG_SAVE );
	CommTimingSyncStart( COMM_GURU2_TIMINGSYNC_NO );
	
	sys.DontSoftReset = 1; //ソフトリセット不可
	g2m->seq_no = SEQNO_MAIN_SAVE_BEFORE_TIMING_WAIT;
	
	#ifdef DEBUG_GURU2_PRINTF
	OS_Printf( "ぐるぐる交換　セーブ開始\n" );
	#endif
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　セーブ　セーブ前の同期
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_SaveBeforeTimingWait( GURU2MAIN_WORK *g2m )
{
	if( CommIsTimingSync(COMM_GURU2_TIMINGSYNC_NO) ){
		Guru2MainFriendEggExchange( g2m, g2m->front_eggact->comm_id );
		CommSyncronizeSaveInit( &g2m->save_seq );
		g2m->time_wait_icon_p = TimeWaitIconAdd(
			&g2m->msgwork.bmpwin_talk[GURU2TALK_WIN_TALK], BGF_CHARNO_TALK );
		g2m->seq_no = SEQNO_MAIN_SAVE;
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　セーブ　セーブデータ反映
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_Save( GURU2MAIN_WORK *g2m )
{
	int ret = CommSyncronizeSave(
		g2m->g2p->param.sv, SVBLK_ID_MAX, &g2m->save_seq );
	
	if( ret ){
		sys.DontSoftReset = 0;
		TimeWaitIconDel( g2m->time_wait_icon_p );
		g2m->seq_no = SEQNO_MAIN_END_TIMING_SYNC_INIT;
	}
	
	return( RET_NON );
}

#if 0
//--------------------------------------------------------------
/**
 * メイン　通信エラーメッセージ　通信エラー
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_CommErrorMsg( GURU2MAIN_WORK *g2m )
{
	Guru2TalkWin_Write( g2m, MSG_COMM_ERROR );
	g2m->seq_no = SEQNO_MAIN_MSG_WAIT_NEXT_END;
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　通信エラーメッセージ　メンバーの都合がつかない
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_CommErrorMember( GURU2MAIN_WORK *g2m )
{
	Guru2TalkWin_Write( g2m, MSG_COMM_ERROR_MEMBER );
	g2m->seq_no = SEQNO_MAIN_MSG_WAIT_NEXT_END;
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　通信エラーメッセージ　親キャンセル
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_CommErrorOyaCancelMsg( GURU2MAIN_WORK *g2m )
{
	Guru2TalkWin_Write( g2m, MSG_COMM_ERROR_CANCEL_OYA );
	g2m->seq_no = SEQNO_MAIN_MSG_WAIT_NEXT_END;
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　通信エラーメッセージ　参加締め切り
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_CommErrorJoinCloseMsg( GURU2MAIN_WORK *g2m )
{
	Guru2TalkWin_Write( g2m, MSG_COMM_ERROR_JOIN_CLOSE );
	g2m->seq_no = SEQNO_MAIN_MSG_WAIT_NEXT_END;
	return( RET_NON );
}
#endif

//--------------------------------------------------------------
/**
 * メイン　通信エラーメッセージ　ダメタマゴが存在
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_CommErrorDameTamagoMsg( GURU2MAIN_WORK *g2m )
{
	Guru2TalkWin_Write( g2m, MSG_COMM_ERROR_DAME_TAMAGO );
	g2m->seq_no = SEQNO_MAIN_MSG_WAIT_NEXT_END;
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　メッセージ終了待ち -> ゲーム終了へ
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_MsgWaitNextEnd( GURU2MAIN_WORK *g2m )
{
	g2m->frame++;
	
	if( g2m->frame >= GURU2_MSG_WAIT_FRAME ){
		g2m->frame = 0;
		g2m->seq_no = SEQNO_MAIN_END_TIMING_SYNC_INIT;
		return( RET_CONT );
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　終了　通信同期待ち開始
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_EndTimingSyncInit( GURU2MAIN_WORK *g2m )
{
	if( g2m->force_end_flag == FALSE ){
		CommTimingSyncStart( COMM_GURU2_TIMINGSYNC_NO );
	}
	
	Guru2TalkWin_Write( g2m, MSG_COMM_WAIT );
	g2m->seq_no = SEQNO_MAIN_END_TIMING_SYNC;
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　終了　通信同期待ち
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_EndTimingSync( GURU2MAIN_WORK *g2m )
{
	if( g2m->force_end_flag == FALSE ){
		if( CommIsTimingSync(COMM_GURU2_TIMINGSYNC_NO) ){
			CommStateSetErrorCheck( FALSE, FALSE ); //切断可能に
			CommStateSetLimitNum( 1 );
			CommStateUnionBconCollectionRestart();
			g2m->seq_no = SEQNO_MAIN_END_CONNECT_CHECK;
		}
	}else{
		g2m->frame++;
		
		if( g2m->frame >= GURU2_MSG_WAIT_FRAME ){
			g2m->frame = 0;
			
			if( g2m->g2c->comm_psel_oya_end_flag == G2C_OYA_END_FLAG_NON ){
				CommStateSetErrorCheck( FALSE, FALSE );
				CommStateSetLimitNum( 1 );
				CommStateUnionBconCollectionRestart();
			}
			
			g2m->seq_no = SEQNO_MAIN_END_CONNECT_CHECK;
		}
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　終了　通信人数確認
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_EndConnectCheck( GURU2MAIN_WORK *g2m )
{
	if( CommGetConnectNum() <= 1 ){
		g2m->seq_no = SEQNO_MAIN_END_FADEOUT_START;
		return( RET_CONT );
	}
	
	return( RET_NON );
}	

//--------------------------------------------------------------
/**
 * メイン　終了　フェードアウト開始
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_EndFadeOutStart( GURU2MAIN_WORK *g2m )
{
	WIPE_SYS_Start( WIPE_PATTERN_FSAM,
		WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT,
		WIPE_FADE_BLACK, 8, 1, HEAPID_GURU2 );
	
	if( g2m->before_bgm_no != Snd_NowBgmNoGet() ){
		Snd_DataSetByScene( SND_SCENE_FIELD, g2m->before_bgm_no, 1 );
	}
	
	g2m->seq_no = SEQNO_MAIN_END_FADEOUT;
	return( RET_NON );
}	

//--------------------------------------------------------------
/**
 * メイン　終了　フェードアウト
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_EndFadeOut( GURU2MAIN_WORK *g2m )
{
	if( WIPE_SYS_EndCheck() ){
		g2m->seq_no = SEQNO_MAIN_END;
		return( RET_CONT );
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * メイン　終了
 * @param	g2m	GURU2MAIN_WORK
 * @retval	GURU2RET	GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Proc_End( GURU2MAIN_WORK *g2m )
{
	return( RET_END );
}

//--------------------------------------------------------------
///	メイン処理テーブル
//--------------------------------------------------------------
static RET (* const DATA_Guru2ProcTbl[SEQNO_MAIN_MAX])( GURU2MAIN_WORK *g2m ) =
{
	Guru2Proc_Init,
	Guru2Proc_FadeInWait,
	
	Guru2Proc_OyaSignalJoinWait,
	Guru2Proc_OyaSendJoinClose,
	Guru2Proc_OyaConnectNumCheck,
	Guru2Proc_OyaSendPlayMax,
	Guru2Proc_OyaSendPlayNo,
	Guru2Proc_OyaSignalEggAddStart,

	Guru2Proc_KoSendSignalJoin,
	Guru2Proc_KoEggAddStartWait,
	
	Guru2Proc_EggDataSendInit,
	Guru2Proc_EggDataSendTimingWait,
	Guru2Proc_EggDataTradePosSend,
	Guru2Proc_EggDataSend,
	Guru2Proc_EggDataRecvWait,
	
	Guru2Proc_DameTamagoCheckWait,
	
	Guru2Proc_EggAddInit,
	Guru2Proc_EggAdd,
	Guru2Proc_EggAddWait,
	Guru2Proc_EggAddEndWait,
	
	Guru2Proc_SendGameStartFlag,
	Guru2Proc_RecvGameStartFlag,
	
	#ifdef DEBUG_DISP_CHECK
	DEBUG_Guru2Proc_DispCheck,
	#endif
	
	Guru2Proc_CountDownBeforeTimingInit,
	Guru2Proc_CountDownBeforeTimingWait,
	Guru2Proc_CountDownInit,
	Guru2Proc_CountDown,
	
	Guru2Proc_GameInit,
	Guru2Proc_GameOya,
	Guru2Proc_GameKo,
	
	Guru2Proc_GameEndInit,
	Guru2Proc_GameEndErrorRotate,
	Guru2Proc_GameEndLastRotate,
	Guru2Proc_GameEndTimingInit,
	Guru2Proc_GameEndTimingWait,
	Guru2Proc_OyaGameEndDataSend,
	Guru2Proc_KoGameEndDataRecv,
	
	Guru2Proc_ResultInit,
	Guru2Proc_ResultNameWait,
	Guru2Proc_ResultMsgWait,
	
	Guru2Proc_OmakeAreaCheck,
	Guru2Proc_OmakeAreaMsgWait,
	Guru2Proc_OmakeAreaErrorMsgStartWait,
	Guru2Proc_OmakeAreaErrorMsgWait,
	
	Guru2Proc_SaveBeforeTimingInit,
	Guru2Proc_SaveBeforeTimingWait,
	Guru2Proc_Save,
	
	#if 0
	Guru2Proc_CommErrorMsg,
	Guru2Proc_CommErrorMember,
	Guru2Proc_CommErrorOyaCancelMsg,
	Guru2Proc_CommErrorJoinCloseMsg,
	#endif
	Guru2Proc_CommErrorDameTamagoMsg,
	
	Guru2Proc_MsgWaitNextEnd,
	
	Guru2Proc_EndTimingSyncInit,
	Guru2Proc_EndTimingSync,
	Guru2Proc_EndConnectCheck,
	
	Guru2Proc_EndFadeOutStart,
	Guru2Proc_EndFadeOut,
	Guru2Proc_End,
};

//==============================================================================
//	VBlank
//==============================================================================
//--------------------------------------------------------------
/**
 * ぐるぐる交換　VBlank関数
 * @param	wk		work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void guru2_VBlankFunc( void *wk )
{
	GURU2MAIN_WORK *g2m = wk;
	
	DoVramTransferManager();
	CATS_RenderOamTrans();
	PaletteFadeTrans( g2m->pfd );
	GF_BGL_VBlankFunc( g2m->bgl );
}

//==============================================================================
//	グラフィック
//==============================================================================
//--------------------------------------------------------------
/**
 * 描画初期化
 * @param	g2m	GURU2MAIN_WORK 
 * @retval	nothing
 */
//--------------------------------------------------------------
static void guru2_DrawInit( GURU2MAIN_WORK *g2m )
{
	guru2_DispInit( g2m );			//画面初期化
	guru2_3DDrawInit( g2m );		//3D描画初期化
	guru2_CameraInit( g2m );		//カメラ初期化
	guru2_BGInit( g2m );			//BG初期化
	guru2_PlttFadeInit( g2m );		//パレットフェード初期化
	guru2_ClActInit( g2m );			//セルアクター初期化
	
	guru2_BGResLoad( g2m );			//BGリソースロード
	guru2_TalkWinFontInit( g2m );	//フォント初期化
	guru2_ClActResLoad( g2m );		//セルアクターリソースロード
	guru2_DispON( g2m );			//画面ON
}

//--------------------------------------------------------------
/**
 * 描画削除
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void guru2_DrawDelete( GURU2MAIN_WORK *g2m )
{
	guru2_DispOFF( g2m );
	
	guru2_ClActDelete( g2m );
	guru2_TalkWinFontDelete( g2m );
	guru2_BGDelete( g2m );
	guru2_PlttFadeDelete( g2m );
	guru2_CameraDelete( g2m );
}

//--------------------------------------------------------------
/**
 * 描画処理
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void guru2_DrawProc( GURU2MAIN_WORK *g2m )
{
	//----3D描画
	GF_G3X_Reset();
	
	//カメラ設定
	GFC_CameraLookAt();
	
	//ライト設定
	NNS_G3dGlbLightVector( 0, 0, -FX32_ONE, 0 );
	NNS_G3dGlbLightColor( 0, GX_RGB(31,31,31) );
	NNS_G3dGlbMaterialColorDiffAmb(
		GX_RGB(31,31,31), GX_RGB(31,31,31), FALSE );
	NNS_G3dGlbMaterialColorSpecEmi(
		GX_RGB(31,31,31), GX_RGB(31,31,31), FALSE );
	
	//皿描画
	Disc_Draw( g2m );
	//卵描画
	EggAct_Draw( g2m );
	
	//ジオメトリ＆レンダリングエンジン関連メモリのスワップ
	GF_G3_RequestSwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_Z);	
	
	//----2D描画
	CATS_Draw( g2m->crp );
}

//==============================================================================
//	画面
//==============================================================================
//--------------------------------------------------------------
/**
 * 画面初期化
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void guru2_DispInit( GURU2MAIN_WORK *g2m )
{
	GF_BGL_DISPVRAM tbl = {				//ディスプレイ　バンク初期化
		GX_VRAM_BG_128_B,				// メイン2DエンジンのBG
		GX_VRAM_BGEXTPLTT_NONE,			// メイン2DエンジンのBG拡張パレット
		GX_VRAM_SUB_BG_128_C,			// サブ2DエンジンのBG
		GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット
		GX_VRAM_OBJ_80_EF,				// メイン2DエンジンのOBJ
		GX_VRAM_OBJEXTPLTT_NONE,		// メイン2DエンジンのOBJ拡張パレット
		GX_VRAM_SUB_OBJ_16_I,			// サブ2DエンジンのOBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット
		GX_VRAM_TEX_0_A,				// テクスチャイメージスロット
		GX_VRAM_TEXPLTT_0_G				// テクスチャパレットスロット
	};
		
	GF_Disp_SetBank( &tbl );
}

//--------------------------------------------------------------
/**
 * 画面ON
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void guru2_DispON( GURU2MAIN_WORK *g2m )
{
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
	
	G2_SetBG0Priority( 1 );
	G2_SetBG1Priority( 0 );
	G2_SetBG2Priority( 2 );
	G2_SetBG3Priority( 3 );
	G2S_SetBG0Priority( 0 );
	G2S_SetBG1Priority( 1 );
	G2S_SetBG2Priority( 2 );
	G2S_SetBG3Priority( 3 );
	
	{	//アルファ変更
		G2_SetBlendAlpha(
			GX_BLEND_PLANEMASK_BG2,
			GX_BLEND_PLANEMASK_BG3|GX_BLEND_PLANEMASK_BD,
			11, 10 );
	}
}

//--------------------------------------------------------------
/**
 * 画面OFF
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void guru2_DispOFF( GURU2MAIN_WORK *g2m )
{
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_OFF );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_OFF );
}


//==============================================================================
//	3D描画
//==============================================================================
//--------------------------------------------------------------
/**
 * 3D描画初期化
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void guru2_3DDrawInit( GURU2MAIN_WORK *g2m )
{
	int i;
		
	NNS_G3dInit();						//3Dエンジン初期化
    G3X_InitMtxStack();					//マトリクス初期化
	
	G3X_SetShading( GX_SHADING_TOON );	//シェード
	G3X_AntiAlias( TRUE);				//アンチエイリアス
	G3X_AlphaTest( FALSE, 0 );			//アルファテスト　
	G3X_AlphaBlend( TRUE );				//アルファブレンド
	
	G3X_EdgeMarking( TRUE );
										//エッジ作成
	for( i = 0; i < EDGE_COLOR_MAX; i++ ){
		g2m->edge_color_tbl[i] =
			GX_RGB( EDGE_COLOR_R, EDGE_COLOR_G, EDGE_COLOR_B );
	}
	
	G3X_SetEdgeColorTable( g2m->edge_color_tbl );
	
    G3X_SetClearColor(		//クリアカラー
		GX_RGB(CLEAR_COLOR_R,CLEAR_COLOR_G,CLEAR_COLOR_B),
		0,					//クリアカラーアルファブレンド値
		0x7fff,				//クリアカラーデプス値
		63,					//アトリビュートバッファポリゴンID初期値
		FALSE );			//アトリビュートバッファフォグON,OFF
	
	//レンダリングエンジンへのスワップ
    G3_SwapBuffers( GX_SORTMODE_AUTO, GX_BUFFERMODE_W );
	
	//ビューポート
    G3_ViewPort( 0, 0, 255, 191 );
	
	//テクスチャVRAMマネージャ初期化
	//VRAMスロット1, VRAMマネージャをデフォルトで使用
	NNS_GfdInitFrmTexVramManager( 1, TRUE );
	
	//パレットVRAMマネージャ初期化
	//パレットRAM=0x4000=16kb,VRAMマネージャをデフォルトで使用
	NNS_GfdInitFrmPlttVramManager(0x4000, TRUE);
}

//==============================================================================
//	BG
//==============================================================================
//--------------------------------------------------------------
/**
 * BG初期化
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void guru2_BGInit( GURU2MAIN_WORK *g2m )
{
	g2m->bgl = GF_BGL_BglIniAlloc( HEAPID_GURU2 );
	GX_SetDispSelect( GX_DISP_SELECT_MAIN_SUB );
	
	{	//BG初期化
		GF_BGL_SYS_HEADER bg_head = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_3D };
		GF_BGL_InitBG( &bg_head );
	}
	
	{	//main BG0 ポリゴン面
	}
	
	{	//main BG1	会話ウィンドウ
		GF_BGL_BGCNT_HEADER bg_cnt_header = {
			0, 0, 0x800, 0,
			GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		GF_BGL_BGControlSet(
			g2m->bgl, GF_BGL_FRAME1_M, &bg_cnt_header, GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear( g2m->bgl, GF_BGL_FRAME1_M );
		GF_BGL_ClearCharSet( GF_BGL_FRAME1_M, 32, 0, HEAPID_GURU2 );
	}
		
	{	//main BG2	背景その1
		GF_BGL_BGCNT_HEADER bg_cnt_header = {
			0, 0, 0x800, 0,
			GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x0c000,
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
		};
		GF_BGL_BGControlSet(
			g2m->bgl, GF_BGL_FRAME2_M, &bg_cnt_header, GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear( g2m->bgl, GF_BGL_FRAME2_M );
	}
	
	{	//main BG3 背景その２
		GF_BGL_BGCNT_HEADER bg_cnt_header = {
			0, 0, 0x800, 0,
			GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x0c000,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
		};
		GF_BGL_BGControlSet(
			g2m->bgl, GF_BGL_FRAME3_M, &bg_cnt_header, GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear( g2m->bgl, GF_BGL_FRAME3_M );
	}
	
	{	//sub BG2　ボタン
		GF_BGL_BGCNT_HEADER bg_cnt_header = {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x14000, GX_BG_EXTPLTT_01,
			2, 0, 0, FALSE
		};
		GF_BGL_BGControlSet(
			g2m->bgl, GF_BGL_FRAME2_S, &bg_cnt_header, GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear( g2m->bgl, GF_BGL_FRAME2_S );
	}
	
	{	//sub BG3　ボタン背景
		GF_BGL_BGCNT_HEADER bg_cnt_header = {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x1800, GX_BG_CHARBASE_0x1c000, GX_BG_EXTPLTT_01,
			3, 0, 0, FALSE
		};
		GF_BGL_BGControlSet(
			g2m->bgl, GF_BGL_FRAME3_S, &bg_cnt_header, GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear( g2m->bgl, GF_BGL_FRAME3_S );
	}
}

//--------------------------------------------------------------
/**
 * BGグラフィックリソースロード
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void guru2_BGResLoad( GURU2MAIN_WORK *g2m )
{
	void *buf;
	
	//BGパレット main
	buf = Guru2Arc_DataLoad( g2m, NARC_guru2_guruguru_bg_NCLR, FALSE );
	NNS_G2dGetUnpackedPaletteData( buf, &g2m->bg_pPltt );
	PaletteWorkSet( g2m->pfd, g2m->bg_pPltt->pRawData,
		FADE_MAIN_BG, BGF_PLTT_M_OFFS, BGF_PLTT_M_SIZE );
	sys_FreeMemoryEz( buf );
	
	//BGパレット sub
	buf = Guru2Arc_DataLoad( g2m, NARC_guru2_guruguru_sub_NCLR, FALSE );
	NNS_G2dGetUnpackedPaletteData( buf, &g2m->bg_pPltt );
	PaletteWorkSet( g2m->pfd, g2m->bg_pPltt->pRawData,
		FADE_SUB_BG, BGF_PLTT_S_OFFS, BGF_PLTT_S_SIZE );
	sys_FreeMemoryEz( buf );
	
	//BGパレット sub 事前に暗くしておく
	ColorConceChangePfd( g2m->pfd, FADE_SUB_BG, 0xffff, 8, 0 );
	
	//BGキャラ MAIN BG3
	buf = Guru2Arc_DataLoad( g2m, NARC_guru2_guruguru_bg_NCGR, FALSE );
	NNS_G2dGetUnpackedCharacterData( buf, &g2m->bg_pChar );
	GF_BGL_LoadCharacter( g2m->bgl, GF_BGL_FRAME3_M,
		g2m->bg_pChar->pRawData, g2m->bg_pChar->szByte, BGF_BG3_M_CHAR_OFFS );
	sys_FreeMemoryEz( buf );
	
	//BGキャラ SUB BG2
	buf = Guru2Arc_DataLoad( g2m, NARC_guru2_guruguru_but_NCGR, FALSE );
	NNS_G2dGetUnpackedCharacterData( buf, &g2m->bg_pChar );
	GF_BGL_LoadCharacter( g2m->bgl, GF_BGL_FRAME2_S,
		g2m->bg_pChar->pRawData, g2m->bg_pChar->szByte, BGF_BG2_S_CHAR_OFFS );
	sys_FreeMemoryEz( buf );
	
	//BGキャラ SUB BG3
	buf = Guru2Arc_DataLoad( g2m, NARC_guru2_guruguru_sub_bg_NCGR, FALSE );
	NNS_G2dGetUnpackedCharacterData( buf, &g2m->bg_pChar );
	GF_BGL_LoadCharacter( g2m->bgl, GF_BGL_FRAME3_S,
		g2m->bg_pChar->pRawData, g2m->bg_pChar->szByte, BGF_BG3_S_CHAR_OFFS );
	sys_FreeMemoryEz( buf );
	
	//BGスクリーン MAIN BG2
	buf = Guru2Arc_DataLoad( g2m, NARC_guru2_guruguru_bg1_NSCR, FALSE );
	NNS_G2dGetUnpackedScreenData( buf, &g2m->bg_pScr );
	GF_BGL_ScreenBufSet( g2m->bgl, GF_BGL_FRAME2_M,
		(void*)g2m->bg_pScr->rawData, g2m->bg_pScr->szByte );
	GF_BGL_LoadScreenReq( g2m->bgl, GF_BGL_FRAME2_M );
	sys_FreeMemoryEz( buf );
	
	//BGスクリーン MAIN BG3
	buf = Guru2Arc_DataLoad( g2m, NARC_guru2_guruguru_bg0_NSCR, FALSE );
	NNS_G2dGetUnpackedScreenData( buf, &g2m->bg_pScr );
	GF_BGL_ScreenBufSet( g2m->bgl, GF_BGL_FRAME3_M,
		(void*)g2m->bg_pScr->rawData, g2m->bg_pScr->szByte );
	GF_BGL_LoadScreenReq( g2m->bgl, GF_BGL_FRAME3_M );
	sys_FreeMemoryEz( buf );
	
	//BGスクリーン SUB BG2
	buf = Guru2Arc_DataLoad( g2m, NARC_guru2_guruguru_but0_NSCR, FALSE );
	NNS_G2dGetUnpackedScreenData( buf, &g2m->bg_pScr );
	GF_BGL_ScreenBufSet( g2m->bgl, GF_BGL_FRAME2_S,
		(void*)g2m->bg_pScr->rawData, g2m->bg_pScr->szByte );
	GF_BGL_LoadScreenReq( g2m->bgl, GF_BGL_FRAME2_S );
	sys_FreeMemoryEz( buf );
	
	//BGスクリーン SUB BG3
	buf = Guru2Arc_DataLoad( g2m, NARC_guru2_guruguru_sub_bg_NSCR, FALSE );
	NNS_G2dGetUnpackedScreenData( buf, &g2m->bg_pScr );
	GF_BGL_ScreenBufSet( g2m->bgl, GF_BGL_FRAME3_S,
		(void*)g2m->bg_pScr->rawData, g2m->bg_pScr->szByte );
	GF_BGL_LoadScreenReq( g2m->bgl, GF_BGL_FRAME3_S );
	sys_FreeMemoryEz( buf );
}

//--------------------------------------------------------------
/**
 * BGコントロール削除
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void guru2_BGDelete( GURU2MAIN_WORK *g2m )
{
	GF_BGL_BGControlExit( g2m->bgl, GF_BGL_FRAME1_M );
	GF_BGL_BGControlExit( g2m->bgl, GF_BGL_FRAME2_M );
	GF_BGL_BGControlExit( g2m->bgl, GF_BGL_FRAME3_M );
	GF_BGL_BGControlExit( g2m->bgl, GF_BGL_FRAME2_S );
	GF_BGL_BGControlExit( g2m->bgl, GF_BGL_FRAME3_S );
	sys_FreeMemoryEz( g2m->bgl );
}

//--------------------------------------------------------------
/**
 * BG SUB ボタンスクリーンを転送
 * @param	g2m	GURU2MAIN_WORK
 * @param	idx	ボタンインデックス
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Guru2BG_ButtonScreenSet( GURU2MAIN_WORK *g2m, u32 idx )
{
	void *buf;
	
	buf = Guru2Arc_DataLoad( g2m, idx, FALSE );
	NNS_G2dGetUnpackedScreenData( buf, &g2m->bg_pScr );
	GF_BGL_ScreenBufSet( g2m->bgl, GF_BGL_FRAME2_S,
		(void*)g2m->bg_pScr->rawData, g2m->bg_pScr->szByte );
	GF_BGL_LoadScreenReq( g2m->bgl, GF_BGL_FRAME2_S );
	sys_FreeMemoryEz( buf );
}

//--------------------------------------------------------------
/**
 * BG　ボタンスクリーン ONを転送
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Guru2BG_ButtonONSet( GURU2MAIN_WORK *g2m )
{
	Guru2BG_ButtonScreenSet( g2m, NARC_guru2_guruguru_but2_NSCR );
}

//--------------------------------------------------------------
/**
 * BG　ボタンスクリーン OFFを転送
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Guru2BG_ButtonOFFSet( GURU2MAIN_WORK *g2m )
{
	Guru2BG_ButtonScreenSet( g2m, NARC_guru2_guruguru_but0_NSCR );
}

//--------------------------------------------------------------
/**
 * BG　ボタンスクリーン 押し始めを転送
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Guru2BG_ButtonMiddleSet( GURU2MAIN_WORK *g2m )
{
	Guru2BG_ButtonScreenSet( g2m, NARC_guru2_guruguru_but1_NSCR );
}

//==============================================================================
//	パレットフェード
//==============================================================================
//--------------------------------------------------------------
/**
 * パレットフェード初期化
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void guru2_PlttFadeInit( GURU2MAIN_WORK *g2m )
{
	g2m->pfd = PaletteFadeInit( HEAPID_GURU2 );
	PaletteTrans_AutoSet(g2m->pfd,TRUE);
	PaletteFadeWorkAllocSet(g2m->pfd,FADE_MAIN_BG,0x200,HEAPID_GURU2);
	PaletteFadeWorkAllocSet(g2m->pfd,FADE_MAIN_OBJ,0x200,HEAPID_GURU2);
	PaletteFadeWorkAllocSet(g2m->pfd,FADE_SUB_BG,0x200,HEAPID_GURU2);
	PaletteFadeWorkAllocSet(g2m->pfd,FADE_SUB_OBJ,0x200,HEAPID_GURU2);
}

//--------------------------------------------------------------
/**
 * パレットフェード削除
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void guru2_PlttFadeDelete( GURU2MAIN_WORK *g2m )
{
	PaletteFadeWorkAllocFree(g2m->pfd, FADE_MAIN_BG);
	PaletteFadeWorkAllocFree(g2m->pfd, FADE_MAIN_OBJ);
	PaletteFadeWorkAllocFree(g2m->pfd, FADE_SUB_BG);
	PaletteFadeWorkAllocFree(g2m->pfd, FADE_SUB_OBJ);
	PaletteFadeFree(g2m->pfd);	
}

//--------------------------------------------------------------
/**
 * 下画面ボタン　輝度セット
 * @param	g2m	GURU2MAIN_WORK
 * @param	evy	EVY
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Guru2PlttFade_BtnFade( GURU2MAIN_WORK *g2m, u32 evy )
{
	ColorConceChangePfd( g2m->pfd, FADE_SUB_BG, 0xffff, evy, 0 );
}

//==============================================================================
//	セルアクター
//==============================================================================
//--------------------------------------------------------------
/**
 * セルアクター初期化
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void guru2_ClActInit( GURU2MAIN_WORK *g2m )
{
	{	//セルアクター初期化
		const TCATS_OAM_INIT ED_OamInit = {
			0, 128, 0, 32,
			0, 128, 0, 32,
		};
		const TCATS_CHAR_MANAGER_MAKE ED_Ccmm = {
			48 + 48,			///< ID管理 main + sub
			1024 * 0x40,		///< 64k
			512 * 0x20,			///< 32k
			GX_OBJVRAMMODE_CHAR_1D_64K,
			GX_OBJVRAMMODE_CHAR_1D_32K
		};
		
		g2m->csp = CATS_AllocMemory( HEAPID_GURU2 );
		CATS_SystemInit( g2m->csp, &ED_OamInit, &ED_Ccmm, 16+16);
	}
	
	{	//セルリソースマネージャー初期化
		BOOL active;
		const TCATS_RESOURCE_NUM_LIST ED_ResList = {
			48 + 48,	///< キャラ登録数 	main + sub
			16 + 16,	///< パレット登録数	main + sub
			64,			///< セル
			64,			///< セルアニメ
			16,			///< マルチセル
			16,			///< マルチセルアニメ
		};
		
		g2m->crp = CATS_ResourceCreate( g2m->csp );
		active   = CATS_ClactSetInit( g2m->csp, g2m->crp, 255 );
		GF_ASSERT(active);
		active  = CATS_ResourceManagerInit( g2m->csp, g2m->crp, &ED_ResList );
		GF_ASSERT(active);
	}
}

//--------------------------------------------------------------
//
/**
 * セルアクター　リソースロード
 * @param	g2m
 * @retval	nothing
 */
//--------------------------------------------------------------
static void guru2_ClActResLoad( GURU2MAIN_WORK *g2m )
{
	CATS_SYS_PTR csp = g2m->csp;
	CATS_RES_PTR crp = g2m->crp;
	PALETTE_FADE_PTR pfd = g2m->pfd;
	ARCHANDLE *hdl = g2m->arc_handle;
	
	//通信アイコン用パレット領域を先に確保
	{
		CLACT_U_WmIcon_SetReserveAreaPlttManager( NNS_G2D_VRAM_TYPE_2DMAIN );
	}
	
	//卵
#if 0	//3Dモデル仕様
	CATS_LoadResourceCharArc( csp, crp,
		ARC_EGGDEMO_GRA, EGG_DATA_EGG_NORMAL_NCGR_BIN, TRUE,
		NNS_G2D_VRAM_TYPE_2DMAIN, EDID_EGG_NCGR );
	CATS_LoadResourcePlttWorkArc( pfd, FADE_MAIN_OBJ, csp, crp,
		ARC_EGGDEMO_GRA, EGG_DATA_EGG_NORMAL_NCLR,
		FALSE, 1, NNS_G2D_VRAM_TYPE_2DMAIN, EDID_EGG_NCLR);
	CATS_LoadResourceCellArc( csp, crp, ARC_EGGDEMO_GRA,
		EGG_DATA_EGG_NORMAL_NCER_BIN, TRUE, EDID_EGG_NCER);
	CATS_LoadResourceCellAnmArc( csp, crp, ARC_EGGDEMO_GRA,
		EGG_DATA_EGG_NORMAL_NANR_BIN, TRUE, EDID_EGG_NANR );
#endif
	
	//カウントダウン
#if 0	//仮絵
	CATS_LoadResourceCharArcH( csp, crp,
		hdl, NARC_guru2_guru2_obj_count_NCGR, FALSE,
		NNS_G2D_VRAM_TYPE_2DMAIN, EDID_CDOWN_NCGR );
	CATS_LoadResourcePlttWorkArcH( pfd, FADE_MAIN_OBJ, csp, crp,
		hdl, NARC_guru2_guru2_obj_count_NCLR,
		FALSE, 1, NNS_G2D_VRAM_TYPE_2DMAIN, EDID_CDOWN_NCLR);
	CATS_LoadResourceCellArcH( csp, crp, hdl,
		NARC_guru2_guru2_obj_count_NCER, FALSE, EDID_CDOWN_NCER );
	CATS_LoadResourceCellAnmArcH( csp, crp, hdl,
		NARC_guru2_guru2_obj_count_NANR, FALSE, EDID_CDOWN_NANR);	
#else
	{	//まぜまぜ流用
		ARCHANDLE *n_hdl;
		n_hdl = ArchiveDataHandleOpen( ARC_NUTMIXER_GRA, HEAPID_GURU2 );
		
		CATS_LoadResourceCharArcH( csp, crp,
			n_hdl, NARC_nutmixer_cook_s_obj0_NCGR, FALSE,
			NNS_G2D_VRAM_TYPE_2DMAIN, EDID_CDOWN_NCGR );
		CATS_LoadResourcePlttWorkArcH( pfd, FADE_MAIN_OBJ, csp, crp,
			n_hdl, NARC_nutmixer_cook_s_obj0_NCLR,
			FALSE, 1, NNS_G2D_VRAM_TYPE_2DMAIN, EDID_CDOWN_NCLR );
		CATS_LoadResourceCellArcH( csp, crp, n_hdl,
			NARC_nutmixer_cook_s_obj0_NCER, FALSE, EDID_CDOWN_NCER );
		CATS_LoadResourceCellAnmArcH( csp, crp, n_hdl,
			NARC_nutmixer_cook_s_obj0_NANR, FALSE, EDID_CDOWN_NANR);
		
		ArchiveDataHandleClose( n_hdl );
	}
	
	{	//アイコンセット
		WirelessIconEasy();
	}
	
	{	//パレットフェードに通信アイコンパレット転送
		NNSG2dPaletteData *palData;
		void *pal = WirelessIconPlttResGet( HEAPID_GURU2 );
		NNS_G2dGetUnpackedPaletteData( pal, &palData );
		PaletteWorkSet(	pfd, palData->pRawData,
			FADE_MAIN_OBJ, WM_ICON_PALETTE_NO*16, 32 );
		sys_FreeMemoryEz( pal );
	}
#endif
}

//--------------------------------------------------------------
/**
 * セルアクター　削除
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void guru2_ClActDelete( GURU2MAIN_WORK *g2m )
{
	CATS_ResourceDestructor_S( g2m->csp, g2m->crp );
	CATS_FreeMemory( g2m->csp );
}

//--------------------------------------------------------------
/**
 * セルアクター　アクター追加
 * @param	g2m	GURU2MAIN_WORK
 * @param	param	TCATS_OBJECT_ADD_PARAM_S
 * @retval	CATS_ACT_PTR CATS_ACT_PTR
 */
//--------------------------------------------------------------
static CATS_ACT_PTR Guru2ClAct_Add(
	GURU2MAIN_WORK *g2m, const TCATS_OBJECT_ADD_PARAM_S *param )
{
	CATS_ACT_PTR act = CATS_ObjectAdd_S( g2m->csp, g2m->crp, param );
	return( act );
}

//==============================================================================
//	フォント
//==============================================================================
//--------------------------------------------------------------
/**
 * フォント　初期化
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void guru2_TalkWinFontInit( GURU2MAIN_WORK *g2m )
{
	int i;
	MSGWORK *msg = &g2m->msgwork;
	
	MenuWinGraphicSet( g2m->bgl, BGF_M_KAIWA,
		BGF_CHARNO_MENU, BGF_PANO_MENU_WIN, 0, HEAPID_GURU2 );
	
	TalkWinGraphicSet(
		g2m->bgl, BGF_M_KAIWA, BGF_CHARNO_TALK,
		BGF_PANO_TALK_WIN, g2m->g2p->param.win_type, HEAPID_GURU2 );

#if 0
//	TalkFontPaletteLoad( PALTYPE_MAIN_BG, BGF_PANO_FONT*32, HEAPID_GURU2 );
	
//	TalkFontPaletteLoad(
//		PALTYPE_MAIN_BG, BGF_PANO_MENU_WIN*32, HEAPID_GURU2 );
#else
	PaletteWorkSet_Arc( g2m->pfd,
		ARC_WINFRAME, TalkWinPalArcGet(g2m->g2p->param.win_type),
		HEAPID_GURU2, FADE_MAIN_BG, 0x20, BGF_PANO_TALK_WIN * 16 );
	PaletteWorkSet_Arc( g2m->pfd,
		ARC_FONT, NARC_font_talk_ncrl, 
		HEAPID_GURU2, FADE_MAIN_BG, 0x20, BGF_PANO_MENU_WIN * 16 );
#endif
	
	msg->msgman = MSGMAN_Create(
		MSGMAN_TYPE_NORMAL, ARC_MSG, NARC_msg_guru2_dat, HEAPID_GURU2 );
	
	msg->wordset = WORDSET_Create( HEAPID_GURU2 );
	
	for( i = 0; i < GURU2TALKWIN_MAX; i++ ){
		GF_BGL_BmpWinAddEx(
			g2m->bgl, &msg->bmpwin_talk[i], &DATA_Guru2BmpTalkWinList[i] );
	}
	
	msg->strbuf = STRBUF_Create( STR_BUF_SIZE, HEAPID_GURU2 );
	
	for( i = 0; i < G2MEMBER_MAX; i++ ){
		g2m->comm.my_name_buf[i] =
			STRBUF_Create( PERSON_NAME_SIZE+EOM_SIZE, HEAPID_GURU2 ); 
	}
	
	Guru2NameWin_Init( g2m, g2m->g2p->receipt_num );
}

//--------------------------------------------------------------
/**
 * フォント　削除
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void guru2_TalkWinFontDelete( GURU2MAIN_WORK *g2m )
{
	int i;
	MSGWORK *msg = &g2m->msgwork;
	
	for( i = 0; i < GURU2TALKWIN_MAX; i++ ){
		GF_BGL_BmpWinOff( &msg->bmpwin_talk[i] );
		GF_BGL_BmpWinDel( &msg->bmpwin_talk[i] );
	}
	
	Guru2NameWin_Delete( g2m );
	
	MSGMAN_Delete( msg->msgman );
	WORDSET_Delete( msg->wordset );
	STRBUF_Delete( msg->strbuf );
	
	for( i = 0; i < G2MEMBER_MAX; i++ ){
		STRBUF_Delete( g2m->comm.my_name_buf[i] );
	}
}

//--------------------------------------------------------------
/**
 * 会話ウィンドウ表示
 * @param	g2m	GURU2MAIN_WORK
 * @param	msgno	メッセージ番号	
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Guru2TalkWin_Write( GURU2MAIN_WORK *g2m, u32 msgno )
{
	MSGWORK *msg = &g2m->msgwork;
	GF_BGL_BMPWIN *bmpwin = &msg->bmpwin_talk[GURU2TALK_WIN_TALK];
	
	GF_BGL_BmpWinDataFill( bmpwin, FBMP_COL_WHITE );
	
	BmpTalkWinWrite( bmpwin,
		WINDOW_TRANS_OFF, BGF_CHARNO_TALK, BGF_PANO_TALK_WIN );
	
	GF_BGL_BmpWinDataFill( bmpwin, FBMP_COL_WHITE );
	
	MSGMAN_GetString( msg->msgman, msgno, msg->strbuf );
	GF_STR_PrintSimple( bmpwin,
		FONT_TALK, msg->strbuf, 0, 0, MSG_NO_PUT, NULL );
	
	GF_BGL_BmpWinOnVReq( bmpwin );
}

//--------------------------------------------------------------
/**
 * 会話ウィンドウ表示　プレイヤーネーム指定
 * @param	g2m	GURU2MAIN_WORK
 * @param	msgno	メッセージ番号	
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Guru2TalkWin_WritePlayer(
	GURU2MAIN_WORK *g2m, u32 msgno, const MYSTATUS *status )
{
	STRBUF *str;
	MSGWORK *msg = &g2m->msgwork;
	GF_BGL_BMPWIN *bmpwin = &msg->bmpwin_talk[GURU2TALK_WIN_TALK];
	
	//文字列生成
	WORDSET_RegisterPlayerName( msg->wordset, 1,
			CommInfoGetMyStatus(CommGetCurrentID()) );
	WORDSET_RegisterPlayerName( msg->wordset, 2, status );
	
	str = STRBUF_Create( STR_BUF_SIZE, HEAPID_GURU2 );
	MSGMAN_GetString( msg->msgman, msgno, str );
	WORDSET_ExpandStr( msg->wordset, msg->strbuf, str );
	STRBUF_Delete( str );
	
	BmpTalkWinWrite( bmpwin,
		WINDOW_TRANS_OFF, BGF_CHARNO_TALK, BGF_PANO_TALK_WIN );
	
	GF_BGL_BmpWinDataFill( bmpwin, FBMP_COL_WHITE );
	
	GF_STR_PrintSimple( bmpwin,
		FONT_TALK, msg->strbuf, 0, 0, MSG_NO_PUT, NULL );
	
	GF_BGL_BmpWinOnVReq( bmpwin );
}

//--------------------------------------------------------------
/**
 * 会話ウィンドウ表示　アイテムID指定
 * @param	g2m	GURU2MAIN_WORK
 * @param	msgno	メッセージ番号	
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Guru2TalkWin_WriteItem(
		GURU2MAIN_WORK *g2m, u32 msgno, u32 id )
{
	STRBUF *str;
	MSGWORK *msg = &g2m->msgwork;
	GF_BGL_BMPWIN *bmpwin = &msg->bmpwin_talk[GURU2TALK_WIN_TALK];
	
	//文字列生成
	WORDSET_RegisterItemName( msg->wordset, 0, id );
	
	str = STRBUF_Create( STR_BUF_SIZE, HEAPID_GURU2 );
	MSGMAN_GetString( msg->msgman, msgno, str );
	WORDSET_ExpandStr( msg->wordset, msg->strbuf, str );
	STRBUF_Delete( str );
	
	BmpTalkWinWrite( bmpwin,
		WINDOW_TRANS_OFF, BGF_CHARNO_TALK, BGF_PANO_TALK_WIN );
	
	GF_BGL_BmpWinDataFill( bmpwin, FBMP_COL_WHITE );
	
	GF_STR_PrintSimple( bmpwin,
		FONT_TALK, msg->strbuf, 0, 0, MSG_NO_PUT, NULL );
	
	GF_BGL_BmpWinOnVReq( bmpwin );
}

//--------------------------------------------------------------
/**
 * 会話ウィンドウクリア
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Guru2TalkWin_Clear( GURU2MAIN_WORK *g2m )
{
	MSGWORK *msg = &g2m->msgwork;
	GF_BGL_BMPWIN *bmpwin = &msg->bmpwin_talk[GURU2TALK_WIN_TALK];
	
	BmpTalkWinClear( bmpwin, WINDOW_TRANS_OFF );
	GF_BGL_BmpWinDataFill( bmpwin, FBMP_COL_NULL );
	GF_BGL_BmpWinOnVReq( bmpwin );
}

//--------------------------------------------------------------
/**
 * ネームウィンドウ初期化
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Guru2NameWin_Init( GURU2MAIN_WORK *g2m, int max )
{
	int i;
	MSGWORK *msg = &g2m->msgwork;
	const BMPWIN_DAT *bmpdata = DATA_Guru2BmpNameWinTbl[max];
	
	msg->win_name_max = max;
	#ifdef DEBUG_GURU2_PRINTF
	OS_Printf( "win_name_max = %d\n", max );
	#endif
	
	for( i = 0; i < max; i++ ){
		GF_BGL_BmpWinAddEx( g2m->bgl, &msg->bmpwin_name[i], &bmpdata[i] );
	}
}

//--------------------------------------------------------------
/**
 * ネームウィンドウ削除
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Guru2NameWin_Delete( GURU2MAIN_WORK *g2m )
{
	int i;
	MSGWORK *msg = &g2m->msgwork;
	
	for( i = 0; i < msg->win_name_max; i++ ){
		GF_BGL_BmpWinOff( &msg->bmpwin_name[i] );
		GF_BGL_BmpWinDel( &msg->bmpwin_name[i] );
	}
}

//--------------------------------------------------------------
/**
 * ネームウィンドウ表示
 * @param	g2m	GURU2MAIN_WORK
 * @param	name	STRBUF
 * @param	no	ウィンドウ番号
 * @param	col	文字色
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Guru2NameWin_Write(
	GURU2MAIN_WORK *g2m, STRBUF *name, int no, u32 col )
{
	MSGWORK *msg = &g2m->msgwork;
	GF_BGL_BMPWIN *bmp = &msg->bmpwin_name[no];
	
	BmpMenuWinWrite(
		bmp, WINDOW_TRANS_OFF, BGF_CHARNO_MENU, BGF_PANO_TALK_WIN );
	GF_BGL_BmpWinDataFill( bmp, FBMP_COL_WHITE );
	GF_STR_PrintColor( bmp, FONT_SYSTEM, name, FontProc_GetPrintCenteredPositionX( FONT_SYSTEM, name, 0, BMPSIZE_NAME_X*8 ), 0, MSG_NO_PUT, col, NULL );
	GF_BGL_BmpWinOnVReq( bmp );
}

//--------------------------------------------------------------
/**
 * ネームウィンドウ表示　IDから色設定
 * @param	g2m	GURU2MAIN_WORK
 * @param	name	STRBUF
 * @param	no	ウィンドウ番号
 * @param	id	通信ID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Guru2NameWin_WriteIDColor(
	GURU2MAIN_WORK *g2m, STRBUF *name, int no, int id )
{
	u32 col = NAME_COL;
	if( id == CommGetCurrentID() ){ col = NAME_COL_MINE; }
	Guru2NameWin_Write( g2m, name, no, col );
}

//--------------------------------------------------------------
/**
 * ネームウィンドウクリア
 * @param	g2m	GURU2MAIN_WORK
 * @param	no	ウィンドウ番号
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Guru2NameWin_Clear( GURU2MAIN_WORK *g2m, int no )
{
	MSGWORK *msg = &g2m->msgwork;
	GF_BGL_BMPWIN *bmpwin = &msg->bmpwin_name[no];
	
	BmpTalkWinClear( bmpwin, WINDOW_TRANS_OFF );
	GF_BGL_BmpWinDataFill( bmpwin, FBMP_COL_NULL );
	GF_BGL_BmpWinOnVReq( bmpwin );
}

//==============================================================================
//	カメラ
//==============================================================================
//--------------------------------------------------------------
/**
 * カメラ初期化
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void guru2_CameraInit( GURU2MAIN_WORK *g2m )
{
	CAMERAWORK *cm = &g2m->camera;
	
	cm->gf_camera = GFC_AllocCamera( HEAPID_GURU2 );
	
	{
		VecFx32 up;
		
		cm->target_pos.x = CAMERA_TARGET_X;
		cm->target_pos.y = CAMERA_TARGET_Y;
		cm->target_pos.z = CAMERA_TARGET_Z;
		
		cm->angle.x = CAMERA_ANGLE_X;
		cm->angle.y = CAMERA_ANGLE_Y;
		cm->angle.z = CAMERA_ANGLE_Z;
		
		cm->distance = CAMERA_DISTANCE;
		cm->persp = CAMERA_PERSP;
		
		GFC_InitCameraTDA(
			&cm->target_pos,
			cm->distance,
			&cm->angle,
			cm->persp,
			GF_CAMERA_PERSPECTIV, //PERSPECTIV=透視射影 ORTHO=正射影
			TRUE,
			cm->gf_camera );	
		
		up.x = 0;
		up.y = FX32_ONE;
		up.z = 0;
		GFC_SetCamUp( &up, cm->gf_camera );
	
		GFC_AttachCamera( cm->gf_camera );
	}
}

//--------------------------------------------------------------
/**
 * カメラ　削除
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void guru2_CameraDelete( GURU2MAIN_WORK *g2m )
{
	CAMERAWORK *cm = &g2m->camera;
	GFC_FreeCamera( cm->gf_camera );
}

//--------------------------------------------------------------
/**
 * カメラ反映
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void guru2_CameraSet( GURU2MAIN_WORK *g2m )
{
	CAMERAWORK *cm = &g2m->camera;
	GFC_BindCameraTarget( &cm->target_pos, cm->gf_camera );
	GFC_SetCameraAngleRev( &cm->angle, cm->gf_camera );
	GFC_SetCameraDistance( cm->distance, cm->gf_camera );
	GFC_SetCameraPerspWay( cm->persp, cm->gf_camera );	
}

//==============================================================================
//	タッチパネル
//==============================================================================
//--------------------------------------------------------------
/**
 * タッチパネル処理 
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void guru2_TPProc( GURU2MAIN_WORK *g2m )
{
	int prs = 0, i = TP_SYNC - 1;
	TPWORK *tp = &g2m->touch;
	
	MainTP( &tp->tp_now, TP_NO_BUFF, 0 );
	
	while( i >= 0 ){
		if( tp->tp_now.TPDataTbl[i].touch == TRUE ){
			tp->x = tp->tp_now.TPDataTbl[i].x;
			tp->y = tp->tp_now.TPDataTbl[i].y;
			prs = 1;
		}
		
		i--;
	}
	
	tp->trg = prs & (prs ^ tp->prs );
	tp->prs = prs;
}

//--------------------------------------------------------------
/**
 * タッチパネル　ボタン範囲チェック
 * @param	g2m	GURU2MAIN_WORK
 * @retval	BOOL	TRUE=ヒット
 */
//--------------------------------------------------------------
static BOOL guru2_TPPosButtonCheck( TPWORK *tp )
{
	int sx,sy,ex,ey,x,y;
	
	x = tp->x;
	y = tp->y;
	sx = TP_BTN_CX - TP_BTN_HSX;
	sy = TP_BTN_CY - TP_BTN_HSY;
	ex = TP_BTN_CX + TP_BTN_HSX - 1;
	ey = TP_BTN_CY + TP_BTN_HSY - 1;
	
	if( (y >= sy && y <= ey) && (x >= sx && x <= ex) ){
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * タッチパネル　ボタン範囲内トリガチェック
 * @param	g2m	GURU2MAIN_WORK
 * @retval	BOOL	TRUE=ヒット
 */
//--------------------------------------------------------------
static BOOL Guru2TP_ButtonHitTrgCheck( GURU2MAIN_WORK *g2m )
{
	TPWORK *tp = &g2m->touch;
	
	if( tp->trg ){
		if( guru2_TPPosButtonCheck(tp) == TRUE ){
			return( TRUE );
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * タッチパネル　ボタン範囲内コンティニューチェック
 * @param	g2m	GURU2MAIN_WORK
 * @retval	BOOL	TRUE=ヒット
 */
//--------------------------------------------------------------
static BOOL Guru2TP_ButtonHitContCheck( GURU2MAIN_WORK *g2m )
{
	TPWORK *tp = &g2m->touch;
	
	if( tp->prs ){
		if( guru2_TPPosButtonCheck(tp) == TRUE ){
			return( TRUE );
		}
	}
	
	return( FALSE );
}

//==============================================================================
//	皿
//==============================================================================
//--------------------------------------------------------------
/**
 * 皿初期化
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Disc_Init( GURU2MAIN_WORK *g2m )
{
	int no;
	u32 idx;
	DISCWORK *disc = &g2m->disc;
	ARCHANDLE *handle = g2m->arc_handle;
	
	no = g2m->g2p->receipt_num;
	#ifdef DEBUG_DISC_NO
	no = DEBUG_DISC_NO;
	#endif	
	idx = DATA_SaraArcIdxTbl[no];
	
	FRO_MDL_ResSetArcLoad( &disc->rmdl, 0, handle, idx, HEAPID_GURU2, 0 );
	FRO_MDL_TexTransBindVTaskAdd( &disc->rmdl );
	FRO_OBJ_InitInMdl( &disc->robj, &disc->rmdl );
	
	disc->rotate_fx = NUM_FX32( DISC_ROTATE_Y );
	disc->rotate_draw_offs_fx =	DATA_DiscRotateDrawOffset[no];
	disc->scale.x = FX32_ONE;
	disc->scale.y = FX32_ONE;
	disc->scale.z = FX32_ONE;
	disc->rotate.x = DISC_ROTATE_X;
	disc->rotate.y = DISC_ROTATE_Y;
	disc->rotate.z = DISC_ROTATE_Z;
	disc->pos.x = DISC_POS_X_FX32;
	disc->pos.y = DISC_POS_Y_FX32;
	disc->pos.z = DISC_POS_Z_FX32;
	disc->draw_pos = disc->pos;
}

//--------------------------------------------------------------
/**
 * 皿削除
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Disc_Delete( GURU2MAIN_WORK *g2m )
{
	DISCWORK *disc = &g2m->disc;
	FRO_MDL_DeleteAll( &disc->rmdl );
}

//--------------------------------------------------------------
/**
 * 皿更新
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Disc_Update( GURU2MAIN_WORK *g2m )
{
	DISCWORK *disc = &g2m->disc;
	fx32 fa = disc->rotate_fx;
	AngleAdd( &fa, disc->rotate_offs_fx );
	AngleAdd( &fa, disc->rotate_draw_offs_fx );
	disc->rotate.y = (360 - FX32_NUM( fa )) % 360;		//反転させ時計回りに
	
	disc->draw_pos.x = disc->pos.x + disc->offs_egg.x + disc->offs.x;
	disc->draw_pos.y = disc->pos.y + disc->offs_egg.y + disc->offs.y;
	disc->draw_pos.z = disc->pos.z + disc->offs_egg.z + disc->offs.z;
}

//--------------------------------------------------------------
/**
 * 皿描画
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Disc_Draw( GURU2MAIN_WORK *g2m )
{
	DISCWORK *disc = &g2m->disc;
	
#ifndef DEBUG_DISP_CHECK_VANISH
	FRO_OBJ_DrawScaleRotate(
		&disc->robj, &disc->draw_pos, &disc->scale, &disc->rotate );
#endif
}

//--------------------------------------------------------------
/**
 * 皿　回転
 * @param	disc	DISCWORK
 * @param	add		回転角度
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Disc_Rotate( DISCWORK *disc, fx32 add )
{
	AngleAdd( &disc->rotate_fx, add );
}

//==============================================================================
//	卵
//==============================================================================
//--------------------------------------------------------------
/**
 * 卵　初期化
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Egg_Init( GURU2MAIN_WORK *g2m )
{
	EGGWORK *egg = &g2m->egg;
	Egg_MdlInit( g2m );
}

//--------------------------------------------------------------
/**
 * 卵　削除
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Egg_Delete( GURU2MAIN_WORK *g2m )
{
	int i;
	EGGACTOR *eact;
	EGGWORK *egg = &g2m->egg;
	
	for( i = 0; i < G2MEMBER_MAX; i++ ){
		eact = &egg->eact[i];
/*	EGG_3D_USE
		if( eact->cap != NULL ){
			CATS_ActorPointerDelete_S( eact->cap );
		}
*/
	}
	
	Egg_MdlDelete( g2m );
}

//==============================================================================
//	卵グラフィック
//==============================================================================
static const u32 DATA_EggCursorMdlArcIDTbl[G2MEMBER_MAX] =
{
	NARC_guru2_g_egg_cursor1_nsbmd,
	NARC_guru2_g_egg_cursor2_nsbmd,
	NARC_guru2_g_egg_cursor3_nsbmd,
	NARC_guru2_g_egg_cursor4_nsbmd,
	NARC_guru2_g_egg_cursor5_nsbmd,
};

static const u32 DATA_EggCursorAnmArcIDTbl[G2MEMBER_MAX] =
{
	NARC_guru2_g_egg_cursor1_nsbca,
	NARC_guru2_g_egg_cursor2_nsbca,
	NARC_guru2_g_egg_cursor3_nsbca,
	NARC_guru2_g_egg_cursor4_nsbca,
	NARC_guru2_g_egg_cursor5_nsbca,
};

//--------------------------------------------------------------
/**
 * 卵グラフィック初期化
 * @param	GURU2MAIN_WORK *g2m
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Egg_MdlInit( GURU2MAIN_WORK *g2m )
{
	int i;
	const u32 *mdl_id, *anm_id;
	
	EGGWORK *egg = &g2m->egg;
	
	FRO_MDL_ResSetArcLoad(	//メインリソースとして扱う
		&egg->m_rmdl, 0, g2m->arc_handle,
		NARC_guru2_g_egg_nsbmd, HEAPID_GURU2, 0 );
	FRO_MDL_TexTransBindVTaskAdd( &egg->m_rmdl );
	
	FRO_MDL_ResSetArcLoad(	//メインリソースとして扱う
		&egg->m_rmdl_kage, 0, g2m->arc_handle,
		NARC_guru2_g_egg_kage_nsbmd, HEAPID_GURU2, 0 );
	FRO_MDL_TexTransBindVTaskAdd( &egg->m_rmdl_kage );
	
	mdl_id = DATA_EggCursorMdlArcIDTbl;
	anm_id = DATA_EggCursorAnmArcIDTbl;
	
	for( i = 0; i < G2MEMBER_MAX; i++, mdl_id++, anm_id++ ){
		FRO_MDL_ResSetArcLoad(
			&egg->m_rmdl_cursor[i], 0, g2m->arc_handle,
			*mdl_id, HEAPID_GURU2, 0 );
		FRO_MDL_TexTransBindVTaskAdd( &egg->m_rmdl_cursor[i] );
		
		FRO_ANM_AnmResSetArcLoad(
			&egg->m_ranm_cursor[i], 0, g2m->arc_handle,
			*anm_id, HEAPID_GURU2, 0 );
		
		FRO_ANM_AllocAnmObjInMdl(
			&egg->m_ranm_cursor[i], &egg->m_rmdl_cursor[i], HEAPID_GURU2 );
		FRO_ANM_AnmObjInitInMdl(
			&egg->m_ranm_cursor[i], &egg->m_rmdl_cursor[i] );
	}
}

//--------------------------------------------------------------
/**
 * 卵グラフィック削除
 * @param	GURU2MAIN_WORK *g2m
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Egg_MdlDelete( GURU2MAIN_WORK *g2m )
{
	int i;
	
	EGGWORK *egg = &g2m->egg;
	FRO_MDL_DeleteAll( &egg->m_rmdl );
	FRO_MDL_DeleteAll( &egg->m_rmdl_kage );
	
	for( i = 0; i < G2MEMBER_MAX; i++ ){
		FRO_MDL_DeleteAll( &egg->m_rmdl_cursor[i] );
		FRO_ANM_AnmResFree( &egg->m_ranm_cursor[i] );
	}
}

//--------------------------------------------------------------
/**
 * 卵アクター　OBJ初期化
 * @param	GURU2MAIN_WORK *g2m
 * @param	act	EGGACTOR*
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Egg_MdlActInit( GURU2MAIN_WORK *g2m, EGGACTOR *act )
{
	EGGWORK *egg = &g2m->egg;
	FRO_OBJ_InitInMdl( &act->robj, &egg->m_rmdl );
	act->draw_flag = TRUE;
	
	EggKage_Init( g2m, act );
	EggCursor_Init( g2m, act );
}

//==============================================================================
//	卵アクター
//==============================================================================
//--------------------------------------------------------------
/**
 * 卵アクター更新
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EggAct_Update( GURU2MAIN_WORK *g2m )
{
	int i = 0,max = g2m->comm.play_max;
	EGGWORK *egg = &g2m->egg;
	
	while( i < max ){
		if( egg->eact[i].use_flag ){
			EggAct_AnglePosSet( &egg->eact[i], &g2m->disc.offs_egg );
		}
		
		if( egg->ekage[i].use_flag ){
			EggKage_Update( g2m, &egg->ekage[i] );
		}
		
		if( egg->ecursor[i].use_flag ){
			EggCursor_Update( g2m, &egg->ecursor[i] );
		}
		
		i++;
	}
}

//--------------------------------------------------------------
/**
 * 卵アクター　描画
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EggAct_Draw( GURU2MAIN_WORK *g2m )
{
	int i = 0,max = g2m->comm.play_max;
	EGGWORK *egg = &g2m->egg;
	EGGACTOR *eact = egg->eact;
	EGGKAGE *ekage = egg->ekage;
	EGGCURSOR *ecs = egg->ecursor;
	
	while( i < max ){
		if( eact->draw_flag ){
			FRO_OBJ_DrawScaleRotate(
				&eact->robj, &eact->pos, &eact->scale, &eact->rotate );
		}
		
		if( ekage->use_flag ){
			FRO_OBJ_DrawScaleRotate(
				&ekage->robj, &ekage->pos, &ekage->scale, &ekage->rotate );
		}
		
		if( ecs->use_flag ){
			FRO_OBJ_DrawPos( &ecs->robj, &ecs->pos );
		}
		
		eact++;
		ekage++;
		ecs++;
		i++;
	}
}

//--------------------------------------------------------------
/**
 * 卵アクター　回転
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EggAct_Rotate( GURU2MAIN_WORK *g2m, fx32 add )
{
	int i = 0,max = g2m->comm.play_max;
	EGGWORK *egg = &g2m->egg;
	
	while( i < max ){
		if( egg->eact[i].use_flag ){
			AngleAdd( &egg->eact[i].angle, add );
		}
		
		i++;
	}
}

//--------------------------------------------------------------
/**
 * 卵　角度から座標、拡縮率決定
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void EggAct_AnglePosSet( EGGACTOR *eact, const VecFx32 *offs )
{
	u16 r;
	fx32 fa;
	
	fa = eact->angle;
	AngleAdd( &fa, eact->offs_angle );
	r = (u16)FX32_NUM( fa );
	
	eact->pos.x =
		EGG_DISC_CX_FX + eact->offs.x + offs->x + (Cos360(r)*EGG_DISC_CXS);
	eact->pos.y =
		EGG_DISC_CY_FX + eact->offs.y + offs->y;
	eact->pos.z =
		EGG_DISC_CZ_FX + eact->offs.z + offs->z + (Sin360(r)*EGG_DISC_CZS);
	
	eact->rotate.x = FX32_NUM( eact->rotate_fx.x );
	eact->rotate.y = FX32_NUM( eact->rotate_fx.y );
	eact->rotate.z = FX32_NUM( eact->rotate_fx.z );
}

//--------------------------------------------------------------
/**
 * 画面手前の卵アクターを返す
 * @param	g2m	GURU2MAIN_WORK
 * @retval	int	アクター配列要素数
 */
//--------------------------------------------------------------
static EGGACTOR * EggAct_FrontEggActGet( GURU2MAIN_WORK *g2m )
{
	int i,j,a,n;
	fx32 front,angle,res;
	int max = g2m->comm.play_max;
	int buf[G2MEMBER_MAX],no[G2MEMBER_MAX];
	EGGWORK *egg = &g2m->egg;
	
	i = 0;
	front = NUM_FX32( DATA_EggStartAngle[max][g2m->comm.my_play_no] );
	
	do{no[i]=i,buf[i]=0xffff;}while(++i<max);
	
	for( i = 0; i < max; i++ ){
		angle = egg->eact[i].angle;
		res = front - angle;
		
		if( res < 0 ){
			res = (front + NUM_FX32(360)) - angle;
		}
		
		buf[i] = FX32_NUM( res );
	}
	
	for( i = 1; i < max; i++ ){
		n = no[i]; a = buf[n];
		for( j = i - 1; j >= 0 && buf[no[j]] > a; j-- ){
			no[j+1] = no[j];
		}
		no[j+1] = n;
	}
	
	return( &egg->eact[no[0]] );
}

//==============================================================================
//	卵影
//==============================================================================
#define EGGKAGE_OFFS_Y_FX (FX32_ONE*2)

//--------------------------------------------------------------
/**
 * 卵影　セット
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void EggKage_Init( GURU2MAIN_WORK *g2m, EGGACTOR *act )
{
	EGGWORK *egg = &g2m->egg;
	EGGKAGE *ekage = &egg->ekage[act->play_no];
	
	ekage->use_flag = TRUE;
	ekage->eact = act;
	FRO_OBJ_InitInMdl( &ekage->robj, &egg->m_rmdl_kage );
	EggKage_Update( g2m, ekage );
}

//--------------------------------------------------------------
/**
 * 卵影　更新
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void EggKage_Update( GURU2MAIN_WORK *g2m, EGGKAGE *ekage )
{
	fx32 offs;
	
	ekage->rotate.x = 0;
	ekage->rotate.y = 0;
	ekage->rotate.z = 0;
	ekage->pos = ekage->eact->pos;
	ekage->pos.x -= ekage->eact->offs.x;
	ekage->pos.y -= ekage->eact->offs.y;
	ekage->pos.z -= ekage->eact->offs.z;
	ekage->pos.y += EGGKAGE_OFFS_Y_FX;
	
	offs = ekage->eact->offs.y;
	
	if( offs < 0 ){
		offs = -offs;
	}
	
	offs /= 0x80;
	
	if( offs > 0x800 ){
		offs = 0x800;
	}
	
	ekage->scale.x = FX32_ONE - offs;
	ekage->scale.y = FX32_ONE - offs;
	ekage->scale.z = FX32_ONE - offs;
}

//==============================================================================
//	卵カーソル
//==============================================================================
#define EGGCURSOR_OFFS_Y_FX (FX32_ONE*20)

//--------------------------------------------------------------
/**
 * 卵カーソル　セット
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void EggCursor_Init( GURU2MAIN_WORK *g2m, EGGACTOR *act )
{
	EGGWORK *egg = &g2m->egg;
	EGGCURSOR *ecs = &egg->ecursor[act->play_no];
	
	ecs->use_flag = TRUE;
	ecs->eact = act;
	
	FRO_OBJ_InitAll( &ecs->robj,
		&egg->m_rmdl_cursor[act->play_no],
		&egg->m_ranm_cursor[act->play_no] );
}

//--------------------------------------------------------------
/**
 * 卵カーソル　更新
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void EggCursor_Update( GURU2MAIN_WORK *g2m, EGGCURSOR *ecs )
{
	EGGWORK *egg = &g2m->egg;
	ecs->pos = ecs->eact->pos;
	ecs->pos.y += EGGCURSOR_OFFS_Y_FX;
	FRO_ANM_Play(
		&egg->m_ranm_cursor[ecs->eact->play_no], FX32_ONE, ANMLOOP_ON);
}

//==============================================================================
//	卵登場タスク
//==============================================================================
//--------------------------------------------------------------
/**
 * 卵登場タスク追加
 * @param	g2m	GURU2MAIN_WORK
 * @param	id	卵ID
 * @param	no	卵番号
 * @param	name_no	名前ウィンドウ番号
 * @param	angle	角度
 * @param	offset	角度オフセット
 * @param	eact	EGGACTOR
 * @retval	TCB_PTR	TCB_PTR
 */
//--------------------------------------------------------------
static TCB_PTR EggAct_StartTcbSet(
	GURU2MAIN_WORK *g2m, int id, int no, int name_no,
	int angle, int offs, EGGACTOR *eact )
{
	TCB_PTR tcb;
	EGGSTARTWORK *work;
	
/*	EGG_3D_USE	
	GF_ASSERT( eact->cap == NULL );
*/
	eact->comm_id = id;
	eact->play_no = no;
	eact->name_no = name_no;
	eact->shake_count = no;
	eact->angle = NUM_FX32( angle );
	eact->offs_angle = NUM_FX32( offs );
	eact->use_flag = TRUE;
	eact->set_flag = FALSE;
	
	work = sys_AllocMemoryLo( HEAPID_GURU2, sizeof(EGGSTARTWORK) );
	memset( work, 0, sizeof(EGGSTARTWORK) );
	work->g2m = g2m;
	work->eact = eact;
	
	tcb = TCB_Add( EggActStartTcb, work, 0 );
	return( tcb );
}

//--------------------------------------------------------------
/**
 * 卵登場タスク
 * @param	tcb	TCB_PTR
 * @param	wk	tcb work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EggActStartTcb( TCB_PTR tcb, void *wk )
{
	RET ret;
	EGGSTARTWORK *work = wk;
	
	do{
		ret = DATA_EggStartTbl[work->seq_no]( work );
	}while( ret == RET_CONT );
	
	if( ret == RET_END ){
		work->eact->set_flag = TRUE;
		sys_FreeMemoryEz( work );
		TCB_Delete( tcb );
	}
}

//--------------------------------------------------------------
/**
 * 卵登場タスク　初期化
 * @param	work	EGGSTARTWORK
 * @retval	BOOL	TRUE=再起
 */
//--------------------------------------------------------------
static RET EggActStart_Init( EGGSTARTWORK *work )
{
	EGGACTOR *eact = work->eact;
	
/*	EGG_3D_USE
	TCATS_OBJECT_ADD_PARAM_S prm;
	
	prm.x = 0;
	prm.y = 0;
	prm.z = 0;
	prm.anm = 0;
	prm.pri = 0;
	prm.pal = 0;
	prm.d_area = CATS_D_AREA_MAIN;
	prm.bg_pri = 1;
	prm.vram_trans = 0;
	prm.id[0]	= EDID_EGG_NCGR;
	prm.id[1]	= EDID_EGG_NCLR;
	prm.id[2]	= EDID_EGG_NCER;
	prm.id[3]	= EDID_EGG_NANR;
	prm.id[4]	= CLACT_U_HEADER_DATA_NONE;
	prm.id[5]	= CLACT_U_HEADER_DATA_NONE;
	
	eact->cap = Guru2ClAct_Add( work->g2m, &prm );
	CATS_ObjectAffineSetCap( eact->cap, CLACT_AFFINE_DOUBLE );
	CATS_ObjectUpdateCap( eact->cap );
*/
	eact->scale.x = FX32_ONE;
	eact->scale.y = FX32_ONE;
	eact->scale.z = FX32_ONE;
	eact->offs.y = EGG_START_OFFS_Y_FX;
	Egg_MdlActInit( work->g2m, eact );
	
	work->fall_speed = NUM_FX32( 8 );
	work->seq_no = SEQNO_EGGSTART_FALL;
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * 卵登場タスク　落下
 * @param	work	EGGSTARTWORK
 * @retval	BOOL	TRUE=再帰
 */
//--------------------------------------------------------------
static RET EggStart_Fall( EGGSTARTWORK *work )
{
	EGGACTOR *eact = work->eact;
	
	eact->offs.y -= work->fall_speed;
	
	if( work->fall_se_play == FALSE ){		//落下SE鳴らすタイミング調整
		if( eact->offs.y <= NUM_FX32(104) ){
			work->fall_se_play = TRUE;
			Snd_SePlay( GURU2_SE_EGG_FALL );
		}
	}
	
	if( eact->offs.y <= NUM_FX32(32) ){
		eact->offs.y = NUM_FX32( 32 );
		work->seq_no = SEQNO_EGGSTART_FALL_END;
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
/**
 * 卵登場タスク　落下終了
 * @param	work	EGGSTARTWORK
 * @retval	BOOL	TRUE=再帰
 */
//--------------------------------------------------------------
static RET EggStart_FallEnd( EGGSTARTWORK *work )
{
	EGGACTOR *eact = work->eact;
	
	if( work->fall_speed > NUM_FX32(1) ){
		work->fall_speed -= 0x1000;
		
		if( work->fall_speed < NUM_FX32(1) ){
			work->fall_speed = NUM_FX32( 1 );
		}
	}
	
	eact->offs.y -= work->fall_speed;
	
	if( eact->offs.y <= 0 ){
		eact->offs.y = 0;
		
		Guru2NameWin_WriteIDColor(
			work->g2m,
			work->g2m->comm.my_name_buf[eact->comm_id],
			eact->name_no, eact->comm_id );
		
		return( RET_END );
	}
	
	return( RET_NON );
}

//--------------------------------------------------------------
///	卵登場タスク　処理テーブル
//--------------------------------------------------------------
static RET (* const DATA_EggStartTbl[SEQNO_EGGSTART_MAX])( EGGSTARTWORK *work ) =
{
	EggActStart_Init,
	EggStart_Fall,
	EggStart_FallEnd,
};

//==============================================================================
//	卵浮き上がりタスク
//==============================================================================
//--------------------------------------------------------------
/**
 * 卵浮き上がり動作
 * @param	g2m		GURU2MAIN_WORK
 * @param	work	EGGJUMP_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EggJump( GURU2MAIN_WORK *g2m, EGGJUMP_WORK *work )
{
	EGGACTOR *eact = work->eact;
	
	switch( work->seq_no ){
	case 0:
		EggSpinTcb_EggSet( g2m, eact );
		work->add_y = EGG_JUMP_TOPSPEED_FX;
		work->accle_y = -EGG_JUMP_ACCLE_FX;
		work->seq_no++;
	case 1:
		eact->offs.y += work->add_y;
		work->add_y += work->accle_y;
		work->frame++;
		
		if( work->frame < EGG_JUMP_FLIGHT_FRAME_HL	){
			break;
		}
		
		work->frame = 0;
		work->add_y = 0;
		
		if( work->turn_flag == FALSE ){
			work->turn_flag = TRUE;
			break;
		}
		
		eact->offs.y = 0;
		EggShakeTcb_EggSet( g2m, eact );
		memset( work, 0, sizeof(EGGJUMP_WORK) );
	}
}

//--------------------------------------------------------------
/**
 * 卵浮き上がりタスク
 * @param	tcb		TCB_PTR
 * @param	wk		tcb work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EggJumpTcb( TCB_PTR tcb, void *wk )
{
	int i;
	GURU2MAIN_WORK *g2m = wk;
	EGGJUMPTCB_WORK *work = &g2m->egg_jump_tcb_work;
	EGGJUMP_WORK *jump = work->jump_work;
	
	for( i = 0; i < G2MEMBER_MAX; i++, jump++ ){
		if( jump->use_flag ){
			EggJump( g2m, jump );
		}
	}
}

//--------------------------------------------------------------
/**
 * 卵浮き上がりタスク初期化
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EggJumpTcb_Init( GURU2MAIN_WORK *g2m )
{
	EGGJUMPTCB_WORK *work;
	
	work = &g2m->egg_jump_tcb_work;
	memset( work, 0, sizeof(EGGJUMPTCB_WORK) );
	
	g2m->tcb_egg_jump = TCB_Add( EggJumpTcb, g2m, TCBPRI_EGG_JUMP );
	GF_ASSERT( g2m->tcb_egg_jump );
}

//--------------------------------------------------------------
/**
 * 卵浮き上がりタスク削除
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EggJumpTcb_Delete( GURU2MAIN_WORK *g2m )
{
	TCB_Delete( g2m->tcb_egg_jump );
}

//--------------------------------------------------------------
/**
 * 卵浮き上がりタスクに卵アクターセット
 * @param	g2m	GURU2MAIN_WORK
 * @param	act	EGGACTOR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EggJumpTcb_EggSet( GURU2MAIN_WORK *g2m, EGGACTOR *act )
{
	int i;
	EGGJUMPTCB_WORK *work = &g2m->egg_jump_tcb_work;
	EGGJUMP_WORK *jump = work->jump_work;
	
	for( i = 0; i < G2MEMBER_MAX; i++, jump++ ){
		if( jump->use_flag == FALSE ){
			jump->use_flag = TRUE;
			jump->eact = act;
			return;
		}
	}
	
	GF_ASSERT( 0 );
}

//--------------------------------------------------------------
/**
 * 卵浮き上がりタスクに全ての卵アクターセット
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EggJumpTcb_EggSetAll( GURU2MAIN_WORK *g2m )
{
	EGGWORK *egg = &g2m->egg;
	int i = 0, max = g2m->comm.play_max;
	
	do{
		if( egg->eact[i].use_flag ){
			EggJumpTcb_EggSet( g2m, &egg->eact[i] );
		}
		i++;
	}while( i < max );
}

//--------------------------------------------------------------
/**
 * 卵浮き上がりタスクチェック
 * @param	g2m	GURU2MAIN_WORK
 * @retval	BOOL	TRUE=ジャンプ中
 */
//--------------------------------------------------------------
static BOOL EggJumpTcb_JumpCheck( GURU2MAIN_WORK *g2m )
{
	int i;
	EGGJUMPTCB_WORK *work = &g2m->egg_jump_tcb_work;
	EGGJUMP_WORK *jump = work->jump_work;
	
	for( i = 0; i < G2MEMBER_MAX; i++, jump++ ){
		if( jump->use_flag ){
			return( TRUE );
		}
	}
	
	return( FALSE );
}

//==============================================================================
///	卵回転タスク
//==============================================================================
//--------------------------------------------------------------
/**
 * 卵回転
 * @param	spin	EGGSPIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EggSpin( EGGSPIN_WORK *spin )
{
	fx32 r;
	EGGACTOR *eact = spin->eact;
	
	switch( spin->seq_no ){
	case 0:
		spin->add_fx = EGG_SPIN_TOPSPEED_FX;
		eact->rotate_fx.y = 0;
		spin->seq_no++;
	case 1:
		r = eact->rotate_fx.y;
		AngleAdd( &eact->rotate_fx.y, spin->add_fx );
		
		if( eact->rotate_fx.y < EGG_SPIN_STOPSPEED_FX ){
			spin->add_fx = (u32)(spin->add_fx) >> 1;
			
			if( spin->add_fx < EGG_SPIN_STOPSPEED_FX ){
				eact->rotate_fx.y = 0;
				memset( spin, 0, sizeof(EGGSPIN_WORK) );
			}
		}
	}
}

//--------------------------------------------------------------
/**
 * 卵回転タスク
 * @param	tcb		TCB_PTR
 * @param	wk		tcb work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EggSpinTcb( TCB_PTR tcb, void *wk )
{
	int i;
	GURU2MAIN_WORK *g2m = wk;
	EGGSPINTCB_WORK *work = &g2m->egg_spin_tcb_work;
	EGGSPIN_WORK *spin = work->spin_work;
	
	for( i = 0; i < G2MEMBER_MAX; i++, spin++ ){
		if( spin->use_flag ){
			EggSpin( spin );
		}
	}
}

//--------------------------------------------------------------
/**
 * 卵回転タスク初期化
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EggSpinTcb_Init( GURU2MAIN_WORK *g2m )
{
	EGGSPINTCB_WORK *work;
	
	work = &g2m->egg_spin_tcb_work;
	memset( work, 0, sizeof(EGGSPINTCB_WORK) );
	
	g2m->tcb_egg_spin = TCB_Add( EggSpinTcb, g2m, TCBPRI_EGG_SPIN );
	GF_ASSERT( g2m->tcb_egg_spin );
}

//--------------------------------------------------------------
/**
 * 卵回転タスク削除
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EggSpinTcb_Delete( GURU2MAIN_WORK *g2m )
{
	TCB_Delete( g2m->tcb_egg_spin );
}

//--------------------------------------------------------------
/**
 * 卵回転タスクに卵アクターセット
 * @param	g2m	GURU2MAIN_WORK
 * @param	act	EGGACTOR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EggSpinTcb_EggSet( GURU2MAIN_WORK *g2m, EGGACTOR *act )
{
	int i;
	EGGSPINTCB_WORK *work = &g2m->egg_spin_tcb_work;
	EGGSPIN_WORK *spin = work->spin_work;
	
	for( i = 0; i < G2MEMBER_MAX; i++, spin++ ){ //実行中であれば再起動
		if( spin->use_flag == TRUE && spin->eact == act ){
			spin->seq_no = 0;
			return;
		}
	}
	
	for( i = 0, spin = work->spin_work; i < G2MEMBER_MAX; i++, spin++ ){
		if( spin->use_flag == FALSE ){
			spin->use_flag = TRUE;
			spin->eact = act;
			return;
		}
	}
	
	GF_ASSERT( 0 );
}

//==============================================================================
//	卵揺れタスク
//==============================================================================
//--------------------------------------------------------------
/**
 * 卵揺れ
 * @param	tcb	TCB_PTR
 * @param	wk	tcb work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EggShake( EGGSHAKE_WORK *shake )
{
//	eact->pos.z = EGG_DISC_CZ_FX + eact->offs.z + (Sin360(r)*EGG_DISC_CZS);
	fx32 r;
	EGGACTOR *eact = shake->eact;
	
	switch( shake->seq_no ){
	case 0:
		eact->rotate_fx.z = 0;
		shake->rad = 0;
		shake->haba_fx = EGG_SHAKE_HABA_FX;
		shake->add_fx = EGG_SHAKE_TOPSPEED_FX;
		shake->seq_no++;
	case 1:
		r = Sin360(FX32_NUM(shake->rad)) * FX32_NUM(shake->haba_fx);
		eact->rotate_fx.z = 0;
		AngleAdd( &eact->rotate_fx.z, r );
		
		r = shake->rad;
		AngleAdd( &shake->rad, shake->add_fx );
		
		if( shake->rad < r ){
			shake->haba_fx = (u32)(shake->haba_fx) >> 1;
			
			if( shake->haba_fx == 0 ){
				eact->rotate_fx.z = 0;
				memset( shake, 0, sizeof(EGGSHAKE_WORK) );
			}
		}
	}
}

//--------------------------------------------------------------
/**
 * 卵揺れタスク
 * @param	tcb	TCB_PTR
 * @param	wk	tcb	work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EggShakeTcb( TCB_PTR tcb, void *wk )
{
	int i;
	GURU2MAIN_WORK *g2m = wk;
	EGGSHAKETCB_WORK *work = &g2m->egg_shake_tcb_work;
	EGGSHAKE_WORK *shake = work->shake_work;
	
	for( i = 0; i < G2MEMBER_MAX; i++, shake++ ){
		if( shake->use_flag ){
			EggShake( shake );
		}
	}
}

//--------------------------------------------------------------
/**
 * 卵揺れタスク初期化
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EggShakeTcb_Init( GURU2MAIN_WORK *g2m )
{
	EGGSHAKETCB_WORK *work;
	
	work = &g2m->egg_shake_tcb_work;
	memset( work, 0, sizeof(EGGSHAKETCB_WORK) );
	
	g2m->tcb_egg_shake = TCB_Add( EggShakeTcb, g2m, TCBPRI_EGG_SHAKE );
	GF_ASSERT( g2m->tcb_egg_shake );
}

//--------------------------------------------------------------
/**
 * 卵揺れタスク削除
 * @param	g2m	GURU2MAIN_WORK	
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EggShakeTcb_Delete( GURU2MAIN_WORK *g2m )
{
	TCB_Delete( g2m->tcb_egg_shake );
}

//--------------------------------------------------------------
/**
 * 卵揺れタスクに卵アクターセット
 * @param	g2m	GURU2MAIN_WORK
 * @param	act	EGGACTOR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EggShakeTcb_EggSet( GURU2MAIN_WORK *g2m, EGGACTOR *act )
{
	int i;
	EGGSHAKETCB_WORK *work = &g2m->egg_shake_tcb_work;
	EGGSHAKE_WORK *shake = work->shake_work;
	
	for( i = 0; i < G2MEMBER_MAX; i++, shake++ ){ //実行中であれば再起動
		if( shake->use_flag == TRUE && shake->eact == act ){
			shake->seq_no = 0;
			return;
		}
	}
	
	for( i = 0, shake = work->shake_work; i < G2MEMBER_MAX; i++, shake++ ){
		if( shake->use_flag == FALSE ){
			shake->use_flag = TRUE;
			shake->eact = act;
			return;
		}
	}
	
	GF_ASSERT( 0 );
}

//==============================================================================
//	卵皿ジャンプタスク
//==============================================================================
//--------------------------------------------------------------
/**
 * 卵皿浮き上がり処理
 * @param	g2m	GURU2MAIN_WORK
 * @param	work	EGGDISCJUMPTCB_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EggDiscJump( GURU2MAIN_WORK *g2m, EGGDISCJUMPTCB_WORK *work )
{
	DISCWORK *disc = &g2m->disc;
	
	switch( work->seq_no ){
	case 0:
		break;
	case 1:
		work->shake_fx = NUM_FX32( 4 );
		work->shake_count = 0;
		work->seq_no++;
	case 2:
		disc->offs_egg.y -= NUM_FX32( 1 );
		if( disc->offs_egg.y <= NUM_FX32(-8) ){
			work->seq_no++;
		}
		break;
	case 3:
		disc->offs_egg.y += NUM_FX32( 2 );
		if( disc->offs_egg.y >= NUM_FX32(0) ){
			disc->offs_egg.y = 0;
			EggJumpTcb_EggSetAll( g2m );
			work->jump_flag = TRUE;
			work->seq_no++;
			
			Snd_SePlay( GURU2_SE_EGG_JUMP );
		}
		break;
	case 4:
		disc->offs.y = work->shake_fx;
		work->shake_fx = -work->shake_fx;
		work->shake_count++;
		if( (work->shake_count&0x03) == 0 ){
			if( work->shake_fx <= NUM_FX32(1) ){
				disc->offs.y = 0;
				work->seq_no++;
			}else{
				work->shake_fx >>= 1;
			}
		}
		break;
	case 5:
		if( EggJumpTcb_JumpCheck(g2m) == FALSE ){
			work->jump_proc_flag = FALSE;
			work->jump_flag = FALSE;
			work->seq_no = 0;
			
			Snd_SePlay( GURU2_SE_EGG_LANDING );
		}
		break;
	}
}

//--------------------------------------------------------------
/**
 * 卵皿浮き上がりタスク
 * @param	tcb		TCB_PTR
 * @param	wk		tcb work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EggDiscJumpTcb( TCB_PTR tcb, void *wk )
{
	GURU2MAIN_WORK *g2m = wk;
	EGGDISCJUMPTCB_WORK *work = &g2m->egg_disc_jump_tcb_work;
	EggDiscJump( g2m, work );
}

//--------------------------------------------------------------
/**
 * 卵皿浮き上がりジャンプタスク初期化 
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EggDiscJumpTcb_Init( GURU2MAIN_WORK *g2m )
{
	EGGDISCJUMPTCB_WORK *work;
	
	work = &g2m->egg_disc_jump_tcb_work;
	memset( work, 0, sizeof(EGGDISCJUMPTCB_WORK) );
	
	g2m->tcb_egg_disc_jump = TCB_Add(
			EggDiscJumpTcb, g2m, TCBPRI_EGG_DISC_JUMP );
	GF_ASSERT( g2m->tcb_egg_disc_jump );
}

//--------------------------------------------------------------
/**
 * 卵皿浮き上がりタスク削除
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EggDiscJumpTcb_Delete( GURU2MAIN_WORK *g2m )
{
	TCB_Delete( g2m->tcb_egg_disc_jump );
}

//--------------------------------------------------------------
/**
 * 卵皿浮き上がりタスク起動
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EggDiscJumpTcb_JumpSet( GURU2MAIN_WORK *g2m )
{
	EGGDISCJUMPTCB_WORK *work;
	work = &g2m->egg_disc_jump_tcb_work;
	GF_ASSERT( work->jump_flag == 0 );
	
	work->jump_proc_flag = TRUE;
	work->seq_no = 1;
}

//--------------------------------------------------------------
/**
 * 卵皿浮き上がりタスク　ジャンプチェック
 * @param	g2m	GURU2MAIN_WORK
 * @retval	BOOL	TRUE=ジャンプ中
 */
//--------------------------------------------------------------
static BOOL EggDiscJumpTcb_JumpCheck( GURU2MAIN_WORK *g2m )
{
	EGGDISCJUMPTCB_WORK *work;
	work = &g2m->egg_disc_jump_tcb_work;
	return( work->jump_flag );
}

//--------------------------------------------------------------
/**
 * 卵皿浮き上がりタスク　処理中チェック
 * @param	g2m	GURU2MAIN_WORK
 * @retval	BOOL	TRUE=ジャンプ中
 */
//--------------------------------------------------------------
static BOOL EggDiscJumpTcb_JumpProcCheck( GURU2MAIN_WORK *g2m )
{
	EGGDISCJUMPTCB_WORK *work;
	work = &g2m->egg_disc_jump_tcb_work;
	return( work->jump_proc_flag );
}

//==============================================================================
//	カウントダウンタスク
//==============================================================================

//--------------------------------------------------------------
///	CDOWNWORK
//--------------------------------------------------------------
typedef struct
{
	int seq_no;
	int frame;
	int end;
	int count;
	GURU2MAIN_WORK *g2m;
	CATS_ACT_PTR cap;
}CDOWNWORK;

//--------------------------------------------------------------
/**
 * カウントダウンTCB
 * @param	tcb	TCB_PTR
 * @param	wk	tcb work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void CountDownTcb( TCB_PTR tcb, void *wk )
{
	CDOWNWORK *work = wk;
	
	switch( work->seq_no ){
	case 0:
		if( work->count < 4 ){	//3..2..1..START
			int tbl[4] = {GURU2_SE_COUNT_3,
				GURU2_SE_COUNT_2,GURU2_SE_COUNT_1,GURU2_SE_START};
			work->frame--;
			if( work->frame <= 0 ){
				work->frame = CDOWN_SE_FRAME;
				Snd_SePlay( tbl[work->count] );
				work->count++;
			}
		}
		
		CATS_ObjectUpdateNumCap( work->cap, FX32_ONE+(FX32_ONE/2) );
		
		if( CATS_ObjectAnimeActiveCheckCap(work->cap) == FALSE ){
			work->seq_no++;
		}
		break;
	case 1:
		CATS_ActorPointerDelete_S( work->cap );
		sys_FreeMemoryEz( work );
		TCB_Delete( tcb );
	}
}

//--------------------------------------------------------------
/**
 * カウントダウンタスク追加
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static TCB_PTR guru2_CountDownTcbSet( GURU2MAIN_WORK *g2m )
{
	TCATS_OBJECT_ADD_PARAM_S prm;
	CDOWNWORK *work = sys_AllocMemoryLo( HEAPID_GURU2, sizeof(CDOWNWORK) );
	memset( work, 0, sizeof(CDOWNWORK) );
	
	prm.x = CDOWN_CX;
	prm.y = CDOWN_CY;
	prm.z = 0;
	prm.anm = 0;
	prm.pri = 0;
	prm.pal = 0;
	prm.d_area = CATS_D_AREA_MAIN;
	prm.bg_pri = 0;
	prm.vram_trans = 0;
	prm.id[0]	= EDID_CDOWN_NCGR;
	prm.id[1]	= EDID_CDOWN_NCLR;
	prm.id[2]	= EDID_CDOWN_NCER;
	prm.id[3]	= EDID_CDOWN_NANR;
	prm.id[4]	= CLACT_U_HEADER_DATA_NONE;
	prm.id[5]	= CLACT_U_HEADER_DATA_NONE;
	
	work->cap = Guru2ClAct_Add( g2m,&prm );
	CATS_ObjectUpdateCap( work->cap );
	
	work->g2m = g2m;
	{
	 TCB_PTR tcb = TCB_Add(CountDownTcb,work,0);
	 GF_ASSERT( tcb != NULL );
	 return( tcb );
	}
}

//==============================================================================
//	ボタンアニメタスク
//==============================================================================
//--------------------------------------------------------------
/**
 * ボタンアニメ動作
 * @param	g2m	GURU2MAIN_WORK
 * @param	work	BTNANMTCB_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void BtnAnm( GURU2MAIN_WORK *g2m, BTNANMTCB_WORK *work )
{
	switch( work->seq_no ){
	case 0:	//待機
		break;
	case 1:	//押す開始
		Guru2BG_ButtonONSet( g2m );
		Snd_SePlay( GURU2_SE_BTN_PUSH );
		work->frame = 0;
		work->seq_no++;
	case 2:	//ボタン押し込み
		work->frame++;
		if( work->frame >= BTN_ANM_FRAME_START ){
			work->frame = 0;
			work->seq_no++;
			Guru2BG_ButtonMiddleSet( g2m );
		}
		break;
	case 3:	
		work->frame++;
		if( work->frame >= BTN_ANM_FRAME_ON ){
			Guru2BG_ButtonONSet( g2m );
			work->frame = 0;
			work->seq_no++;
		}
		break;
	case 4:
		work->frame++;
		if( work->frame >= BTN_ANM_FRAME_BACK ){
			Guru2BG_ButtonOFFSet( g2m );
			work->push_flag = FALSE;
			work->seq_no = 0;
		}
		break;
	}
}

//--------------------------------------------------------------
/**
 * ボタンアニメタスク
 * @param	tcb		TCB_PTR
 * @param	wk		tcb work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void BtnAnmTcb( TCB_PTR tcb, void *wk )
{
	GURU2MAIN_WORK *g2m = wk;
	BTNANMTCB_WORK *work = &g2m->btn_anm_tcb_work;
	BtnAnm( g2m, work );
}

//--------------------------------------------------------------
/**
 * ボタンアニメタスク初期化
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void BtnAnmTcb_Init( GURU2MAIN_WORK *g2m )
{
	BTNANMTCB_WORK *work;
	work = &g2m->btn_anm_tcb_work;
	memset( work, 0, sizeof(BTNANMTCB_WORK) );
	g2m->tcb_btn_anm = TCB_Add( BtnAnmTcb, g2m, TCBPRI_BTN_ANM );
	GF_ASSERT( g2m->tcb_btn_anm );
}

//--------------------------------------------------------------
/**
 * ボタンアニメタスク削除
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void BtnAnmTcb_Delete( GURU2MAIN_WORK *g2m )
{
	TCB_Delete( g2m->tcb_btn_anm );
}

//--------------------------------------------------------------
/**
 * ボタンアニメタスク　押し込みチェック
 * @param	g2m	GURU2MAIN_WORK
 * @retval	BOOL	TRUE=押し込み中
 */
//--------------------------------------------------------------
static BOOL BtnAnmTcb_PushCheck( GURU2MAIN_WORK *g2m )
{
	BTNANMTCB_WORK *work;
	work = &g2m->btn_anm_tcb_work;
	return( work->push_flag );
}

//--------------------------------------------------------------
/**
 * ボタンアニメタスク　押す
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void BtnAnmTcb_PushSet( GURU2MAIN_WORK *g2m )
{
	BTNANMTCB_WORK *work;
	work = &g2m->btn_anm_tcb_work;
	GF_ASSERT( work->push_flag == FALSE );
	work->push_flag = TRUE;
	work->seq_no = 1;
}

//==============================================================================
//	おまけゾーン卵ジャンプタスク
//==============================================================================
//--------------------------------------------------------------
/**
 * おまけ卵ジャンプ
 * @param	g2m	GURU2MAIN_WORK
 * @param	work	OMAKEJUMP_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void OmakeEggJump( GURU2MAIN_WORK *g2m, OMAKEJUMP_WORK *work )
{
	switch( work->seq_no ){
	case 0:
		break;
	case 1:
		{
			static int offs[16] = {
				4, 6, 8, 10, 11, 12, 12, 12,
				11, 10, 9, 8, 6, 4, 0, 0 };
			
			work->eact->offs.y = NUM_FX32( offs[work->count] );
			work->count++;
		
			if( work->count >= 16 ){
				work->count = 0;
				work->seq_no++;
			}
		}
		break;
	case 2:
		work->wait++;
		
		if( work->wait >= 15 ){
			work->wait = 0;
			work->seq_no = 1;
		}
		break;
	}
}

//--------------------------------------------------------------
/**
 * おまけ卵ジャンプタスク
 * @param	tcb		TCB_PTR
 * @param	wk		tcb work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void OmakeEggJumpTcb( TCB_PTR tcb, void *wk )
{
	int i;
	GURU2MAIN_WORK *g2m = wk;
	OMAKEJUMPTCB_WORK *work = &g2m->omake_jump_tcb_work;
	OMAKEJUMP_WORK *jump = work->omake_jump_work;
	
	for( i = 0; i < G2MEMBER_MAX; i++, jump++ ){
		OmakeEggJump( g2m, jump );
	}
}

//--------------------------------------------------------------
/**
 * おまけ卵ジャンプタスク初期化
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void OmakeEggJumpTcb_Init( GURU2MAIN_WORK *g2m )
{
	OMAKEJUMPTCB_WORK *work;
	work = &g2m->omake_jump_tcb_work;
	memset( work,0,sizeof(OMAKEJUMPTCB_WORK) );
	g2m->tcb_omake_jump = TCB_Add( OmakeEggJumpTcb, g2m, TCBPRI_OMAKE_JUMP );
	GF_ASSERT( g2m->tcb_omake_jump );
}

//--------------------------------------------------------------
/**
 * おまけ卵ジャンプタスク削除
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void OmakeEggJumpTcb_Delete( GURU2MAIN_WORK *g2m )
{
	TCB_Delete( g2m->tcb_omake_jump );
}

//--------------------------------------------------------------
/**
 * おまけ卵ジャンプタスクに卵登録
 * @param	g2m	GURU2MAIN_WORK
 * @param	act	EGGACTOR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void OmakeEggJumpTcb_EggSet( GURU2MAIN_WORK *g2m, EGGACTOR *act )
{
	int id = act->comm_id;
	OMAKEJUMPTCB_WORK *work = &g2m->omake_jump_tcb_work;
	OMAKEJUMP_WORK *ojump = &work->omake_jump_work[id];
	
	GF_ASSERT( id < G2MEMBER_MAX );
	GF_ASSERT( ojump->seq_no == 0 );
	
	ojump->seq_no = 1;
	ojump->eact = act;
}

//--------------------------------------------------------------
/**
 * omake_bit立ち卵にジャンプセット
 * @param	
 * @retval
 */
//--------------------------------------------------------------
static void OmakeEggJumpTcb_OmakeBitCheckSet( GURU2MAIN_WORK *g2m )
{
	u32 bit = g2m->omake_bit;
	int i = 0, max = g2m->comm.play_max;
	EGGWORK *egg = &g2m->egg;
	
	while( i < max ){
		if( egg->eact[i].use_flag && (bit & (1 << egg->eact[i].comm_id)) ){
			OmakeEggJumpTcb_EggSet( g2m, &egg->eact[i] );
		}
		
		i++;
	}
}

#if 0
static void OmakeEggJumpTcb_AllSet( GURU2MAIN_WORK *g2m )
{
	fx32 pos;
	int i = 0, max = g2m->comm.play_max;
	EGGWORK *egg = &g2m->egg;
	
	while( i < max ){
		if( egg->eact[i].use_flag ){
			pos = egg->eact[i].angle;
			
			if( Guru2MainOmakeZoneCheck(g2m,pos,max) == TRUE ){
				OmakeEggJumpTcb_EggSet( g2m, &egg->eact[i] );
			}
		}
		
		i++;
	}
}
#endif

//==============================================================================
//	ボタン明暗
//==============================================================================
//--------------------------------------------------------------
/**
 * ボタンを明暗するタスク
 * @param	g2m	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void BtnFadeTcb( TCB_PTR tcb, void *wk )
{
	int end = FALSE;
	BTNFADE_WORK *work = wk;
	
	if( work->light_flag == TRUE ){		//明るく
		work->evy--;
		
		if( (int)work->evy <= 0 ){
			work->evy = 0;
			end = TRUE;
		}
	}else{
		work->evy++;
		
		if( (int)work->evy >= 8 ){
			work->evy = 8;
			end = TRUE;
		}
	}
	
	Guru2PlttFade_BtnFade( work->g2m, work->evy );
	
	if( end == TRUE ){
		sys_FreeMemoryEz( work );
		TCB_Delete( tcb );
	}
}

//--------------------------------------------------------------
/**
 * ボタン明暗タスク追加
 * @param	g2m		GURU2MAIN_WORK
 * @param	light	TURE=明るく FALSE=暗く
 * @retval	nothing
 */
//--------------------------------------------------------------
static void BtnFadeTcbAdd( GURU2MAIN_WORK *g2m, int light )
{
	TCB_PTR tcb;
	u32 evy[2] = { 0, 8 };
	
	BTNFADE_WORK *work =
		sys_AllocMemoryLo( HEAPID_GURU2, sizeof(BTNFADE_WORK) );
	
	work->g2m = g2m;
	work->light_flag = light;
	work->evy = evy[light];
	
	tcb = TCB_Add( BtnFadeTcb, work, 0 );
	GF_ASSERT( tcb != NULL );
}

//==============================================================================
//	パーツ
//==============================================================================
//--------------------------------------------------------------
/**
 * 角度増減
 * @param	angle	角度
 * @param	add		増減値
 * @retval	nothing
 */
//--------------------------------------------------------------
static void AngleAdd( fx32 *angle, fx32 add )
{
	(*angle) += add;
	while( (*angle) < 0 ){ (*angle) += NUM_FX32(360); }
	(*angle) %= NUM_FX32( 360 );
}

//--------------------------------------------------------------
/**
 * 皿を回転させ、卵を動かす。
 * @param	gm	GURU2MAIN_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void DiscRotateEggMove( GURU2MAIN_WORK *g2m, fx32 speed )
{
	Disc_Rotate( &g2m->disc, speed );
	EggAct_Rotate( g2m, speed );
}

//--------------------------------------------------------------
/**
 * ゲーム時間の進行
 * @param	g2m	GURU2MAIN_WORK
 * @retval	BOOL	TRUE=終了
 */
//--------------------------------------------------------------
static BOOL Guru2GameTimeCount( GURU2MAIN_WORK *g2m )
{
	g2m->game_frame++;
	
	if( g2m->game_frame < GURU2_GAME_FRAME ){
		return( FALSE );
	}
	
	g2m->game_frame = GURU2_GAME_FRAME;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * アーカイブデータ取り出し
 * @param	g2m		GURU2MAIN_WORK
 * @param	idx		データインデックス
 * @param	fb		TRUE=前,FALSE=後ろから領域確保
 * @retval	void*	ロードデータ
 */
//--------------------------------------------------------------
static void * Guru2Arc_DataLoad( GURU2MAIN_WORK *g2m, u32 idx, BOOL fb )
{
	void *buf;
	u32 size = ArchiveDataSizeGetByHandle( g2m->arc_handle, idx );
	
	if( fb == TRUE ){
		buf = sys_AllocMemory( HEAPID_GURU2, size );
	}else{
		buf = sys_AllocMemoryLo( HEAPID_GURU2, size );
	}
	
	ArchiveDataLoadByHandle( g2m->arc_handle, idx, buf );
	return( buf );
}

//--------------------------------------------------------------
/**
 * キートリガチェック
 * @param	pad		PAD_BUTTON_A等
 * @retval	BOOL	TRUE=ヒット
 */
//--------------------------------------------------------------
static BOOL Guru2Pad_TrgCheck( u32 pad )
{
	u32 ret = sys.trg & pad;
	if( ret ){ return( TRUE ); }
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * キーコンテニューチェック
 * @param	pad		PAD_BUTTON_A等
 * @retval	BOOL	TRUE=ヒット
 */
//--------------------------------------------------------------
static BOOL Guru2Pad_ContCheck( u32 pad )
{
	u32 ret = sys.cont & pad;
	if( ret ){ return( TRUE ); }
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * 通信から送られてくるボタン情報を格納
 * @param	g2m		GURU2MAIN_WORK
 * @param	btn		ボタン情報
 * @retval	nothing
 */
//--------------------------------------------------------------
void Guru2Main_CommButtonSet( GURU2MAIN_WORK *g2m, u32 btn )
{
	if( btn != 0 ){
		g2m->comm.push_btn = btn;
	}
}

//--------------------------------------------------------------
/**
 * 通信から送られてくるゲーム情報を格納
 * @param	g2m	GURU2MAIN
 * @param	data	GURU2COMM_GAMEDATA
 * @retval	nothing
 */
//--------------------------------------------------------------
void Guru2Main_CommGameDataSet(
		GURU2MAIN_WORK *g2m, const GURU2COMM_GAMEDATA *data )
{
	g2m->comm.game_data_send_flag = TRUE;
	g2m->comm.game_data = *data;
}

//--------------------------------------------------------------
/**
 * テンポラリワーク初期化
 * @param	g2m	GURU2MAIN_WORK
 * @retval	void*
 */
//--------------------------------------------------------------
static void * Guru2MainTempWorkInit( GURU2MAIN_WORK *g2m, u32 size )
{
	GF_ASSERT( size < G2M_TEMPWORK_SIZE );
	memset( g2m->temp_work, 0, G2M_TEMPWORK_SIZE );
	return( g2m->temp_work );
}

//--------------------------------------------------------------
/**
 * テンポラリワーク取得
 * @param	g2m	GURU2MAIN_WORK
 * @retval	void*
 */
//--------------------------------------------------------------
static void * Guru2MainTempWorkGet( GURU2MAIN_WORK *g2m )
{
	return( g2m->temp_work );
}

//--------------------------------------------------------------
/**
 * 親が送るシグナルビットチェック
 * @param	g2m	GURU2MAIN_WORK
 * @retval	BOOL	TRUE=締め切り
 */
//--------------------------------------------------------------
static BOOL Guru2MainCommSignalCheck(GURU2MAIN_WORK *g2m, u16 bit)
{
	u32 signal = g2m->g2c->comm_game_signal_bit;
	if( (signal&bit) ){ return( TRUE ); }
	return( FALSE );
}

#if 0
//--------------------------------------------------------------
/**
 * ゲーム参加締め切りチェック
 * @param	g2m	GURU2MAIN_WORK
 * @retval	BOOL	TRUE=締め切り
 */
//--------------------------------------------------------------
static BOOL Guru2MainCommJoinCloseCheck( GURU2MAIN_WORK *g2m )
{
	u32 bit = g2m->g2c->comm_game_signal_bit;
	
	if( (bit&G2COMM_GMSBIT_JOIN_CLOSE) ){
		return( TRUE );
	}
	
	return( FALSE );
}
#endif

//--------------------------------------------------------------
/**
 * ゲーム参加人数チェック
 * @param	g2m	GURU2MAIN_WORK
 * @retval	BOOL	TRUE 参加人数一致
 */
//--------------------------------------------------------------
static BOOL Guru2MainCommJoinNumCheck( GURU2MAIN_WORK *g2m )
{
	int in = 0;
	int max = g2m->g2p->receipt_num - 1;	//-1=自身
	u32 bit = g2m->g2c->comm_game_join_bit;
	
	while( bit ){
		in += (bit&0x01);
		bit >>= 1;
	}
	
	if( in >= max ){
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * ゲーム参加人数取得
 * @param	g2m	GURU2MAIN_WORK
 * @retval	int TRUE 参加人数一致
 */
//--------------------------------------------------------------
static int Guru2MainCommJoinNumGet( GURU2MAIN_WORK *g2m )
{
	int in = 0;
	u32 bit = g2m->g2c->comm_game_join_bit;	//子の参加者
	
	while( bit ){
		in += (bit&0x01);
		bit >>= 1;
	}
	
	return( in );
}

#if 0
//--------------------------------------------------------------
/**
 * 親ゲームキャンセルチェック
 * @param	g2m	GURU2MAIN_WORK
 * @retval	BOOL	TRUE=親キャンセル
 */
//--------------------------------------------------------------
static BOOL Guru2MainCommOyaCancelCheck( GURU2MAIN_WORK *g2m )
{
	if( Guru2MainCommSignalCheck(g2m,G2COMM_GMSBIT_CANCEL) ){
		return( TRUE );
	}

	return( FALSE );
}
#endif

//--------------------------------------------------------------
/**
 * 送られてきたプレイ番号をセット
 * @param	g2m		GURU2MAIN_WORK
 * @param	no	GURU2COMM_PLAYNO
 * @retval	nothing
 */
//--------------------------------------------------------------
void Guru2Main_CommPlayNoDataSet(
		GURU2MAIN_WORK *g2m, const GURU2COMM_PLAYNO *no )
{
	g2m->comm.play_no_tbl[no->play_no] = *no;
	
	if( no->comm_id == CommGetCurrentID() ){
		g2m->comm.my_play_no = no->play_no;
		#ifdef DEBUG_GURU2_PRINTF
		OS_Printf( "ぐるぐる　自分のプレイ番号(%d)受信\n", no->play_no );
		#endif
	}
}

//--------------------------------------------------------------
/**
 * 送られてきたプレイ最大人数をセット
 * @param	g2m		GURU2MAIN_WORK
 * @param	max 	プレイ最大人数
 * @retval	nothing
 */
//--------------------------------------------------------------
void Guru2Main_CommPlayMaxSet( GURU2MAIN_WORK *g2m, int max )
{
	g2m->comm.play_max = max;
}

//--------------------------------------------------------------
/**
 * 指定IDがゲーム参加者かどうかチェック
 * @param	g2m
 * @param	id
 * @retval	nothing
 */
//--------------------------------------------------------------
static BOOL Guru2MainCommIDPlayCheck( GURU2MAIN_WORK *g2m, int id )
{
	if( id != 0 ){ //親は無条件で参加
		u32 bit = 1 << (u32)id;
		if( (g2m->g2c->comm_game_join_bit&bit) == 0 ){
			return( FALSE );
		}
	}
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * たまごデータ交換
 * @param	g2m		GURU2MAIN_WORK
 * @param	id		id=交換先のID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Guru2MainFriendEggExchange( GURU2MAIN_WORK *g2m, int id )
{
	int m_pos,f_pos;
	POKEPARTY *m_poke,*f_poke;
	POKEMON_PARAM *m_pp,*f_pp;
//	POKEMON_PARAM *mc_pp,*fc_pp;
	
	m_poke = g2m->my_poke_party;
	f_poke = (POKEPARTY *)Guru2Comm_FriendPokePartyGet( g2m->g2c, id );
	
	m_pos = g2m->g2p->trade_no;
	f_pos = g2m->comm.trade_no_tbl[id];
	
	m_pp = PokeParty_GetMemberPointer( m_poke, m_pos );
	f_pp = PokeParty_GetMemberPointer( f_poke, f_pos );
	
	TrainerMemoSetPP(
		f_pp,
		CommInfoGetMyStatus(CommGetCurrentID()),
		TRMEMO_ONTRADE_PLACESET,
		0,
		HEAPID_WORLD );
	
	PokeCopyPPtoPP( f_pp, m_pp );	//コピー元,コピー先
}

//--------------------------------------------------------------
/**
 * タマゴデータ受信数取得
 * @param	g2m	GURU2MAIN_WORK
 * @retval	int
 */
//--------------------------------------------------------------
static int Guru2MainCommEggDataNumGet( GURU2MAIN_WORK *g2m )
{
	int in = 0;
	u32 bit = g2m->g2c->comm_game_egg_recv_bit;
	
	while( bit ){
		in += (bit&0x01);
		bit >>= 1;
	}
	
	return( in );
}

//--------------------------------------------------------------
/**
 * 交換位置セット
 * @param	g2m	GURU2MAIN_WORK
 * @retval	int
 */
//--------------------------------------------------------------
void Guru2Main_TradeNoSet( GURU2MAIN_WORK *g2m, int id, int no )
{
	g2m->comm.trade_no_tbl[id] = no;
}

//--------------------------------------------------------------
/**
 * ゲーム結果セット
 * @param	g2m	GURU2MAIN_WORK
 * @param	result	GURU2COMM_GAMERESULT
 * @retval	int
 */
//--------------------------------------------------------------
void Guru2Main_GameResultSet(
	GURU2MAIN_WORK *g2m, const GURU2COMM_GAMERESULT *result )
{
	g2m->comm.game_result = *result;
	g2m->comm.game_result_get_flag = TRUE;
}

//--------------------------------------------------------------
/**
 * おまけゾーンチェック
 * @param	g2m		GURU2MAIN_WORK
 * @param	pos_fx	角度0-359
 * @param	max		プレイ人数最大
 * @retval	BOOL	TRUE=あたり
 */
//--------------------------------------------------------------
static BOOL Guru2MainOmakeZoneCheck(
		GURU2MAIN_WORK *g2m, fx32 pos_fx, int max )
{
	int count;
	fx32 d_pos;
	u32 pos0,pos1;
	const OMAKE_AREA_TBL *tbl;
	const OMAKE_AREA *area;
	
	#ifdef DEBUG_DISC_NO
	max = DEBUG_DISC_NO;
	#endif
	
	AngleAdd( &pos_fx, -g2m->disc.rotate_fx );		//ディスク移動分補正
	
	d_pos = pos_fx;
	AngleAdd( &d_pos, EGG_ATARI_HABA_L_FX );
	pos0 = FX32_NUM( d_pos );
	
	d_pos = pos_fx;
	AngleAdd( &d_pos, EGG_ATARI_HABA_R_FX );
	pos1 = FX32_NUM( d_pos );
	
	#ifndef DEBUG_DISP_CHECK
	OS_Printf("ぐるぐる おまけエリアチェック " );
	OS_Printf( "pos0=%d,pos1=%d,max=%d\n",pos0,max);
	#endif
	
	tbl = &DATA_OmakeAreaTbl[max];
	count = tbl->max;
	area = tbl->area;
	
	while( count ){
		if( (pos0 >= area->start && pos0 <= area->end) ||
			(pos1 >= area->start && pos1 <= area->end) ){
			return( TRUE );
		}
		
		area++;
		count--;
	}
	
	return( FALSE );
}

#if 0	//一点チェックのみ
static BOOL Guru2MainOmakeZoneCheck(
		GURU2MAIN_WORK *g2m, fx32 pos_fx, int max )
{
	u32 pos;
	int count;
	fx32 d_offs;
	const OMAKE_AREA_TBL *tbl;
	const OMAKE_AREA *area;
	
	#ifdef DEBUG_DISC_NO
	max = DEBUG_DISC_NO;
	#endif
	
	AngleAdd( &pos_fx, -g2m->disc.rotate_fx );		//ディスク移動分補正
	pos = FX32_NUM( pos_fx );
	
	#ifndef DEBUG_DISP_CHECK
	OS_Printf("ぐるぐる おまけエリアチェック pos=%d,max=%d\n",pos,max);
	#endif
	
	tbl = &DATA_OmakeAreaTbl[max];
	count = tbl->max;
	area = tbl->area;
	
	while( count ){
		if( area->start <= pos && area->end >= pos ){
			return( TRUE );
		}
		
		area++;
		count--;
	}
	
	return( FALSE );
}
#endif

//--------------------------------------------------------------
/**
 * ダメタマゴチェック 
 * @param	ppty	POKEPARTY 
 * @retval	BOOL	TRUE==ダメタマゴ有り
 */
//--------------------------------------------------------------
static BOOL Guru2MainPokePartyDameTamagoCheck( POKEPARTY *ppty )
{
	int i,max;
	POKEMON_PARAM *pp;
	
	max = PokeParty_GetPokeCount( ppty );
	
	for( i = 0; i < max; i++ ){
		pp = PokeParty_GetMemberPointer( ppty, i );
		
		if( PokeParaGet(pp,ID_PARA_tamago_flag,NULL) ){
			if( PokeParaGet(pp,ID_PARA_fusei_tamago_flag,NULL) ){
				return( TRUE );
			}
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * 手持ちと相手のダメタマゴチェック
 * @param	g2m	GURU2MAIN_WORK
 * @retval	BOOL TRUE==ダメタマゴ有り
 */
//--------------------------------------------------------------
static BOOL Guru2MainDameTamagoCheck( GURU2MAIN_WORK *g2m )
{
	//手持ち
	if( Guru2MainPokePartyDameTamagoCheck(g2m->my_poke_party) == TRUE ){
		return( TRUE );
	}
	
	{	//相手
		int id;
		POKEPARTY *pty;
		
		for( id = 0; id < G2MEMBER_MAX; id++ ){
			if( Guru2MainCommIDPlayCheck(g2m,id) ){
				pty = Guru2Comm_FriendPokePartyGet( g2m->g2c, id );
				if( Guru2MainPokePartyDameTamagoCheck(pty) == TRUE ){
					return( TRUE );
				}
			}
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * 卵データチェック　OK数を取得
 * @param	g2m	GURU2MAIN_WORK *
 * @retval	int OK受信数
 */
//--------------------------------------------------------------
static int Guru2MainCommEggDataOKCountCheck( GURU2MAIN_WORK *g2m )
{
	int count = 0;
	u32 bit = g2m->g2c->comm_game_egg_check_ok_bit;
	
	while( bit ){
		count += bit & 0x01;
		bit >>= 1;
	}
	
	return( count );
}

//--------------------------------------------------------------
/**
 * 卵データチェック　エラー判定
 * @param	g2m	GURU2MAIN_WORK *
 * @retval	BOOL TRUE=ダメタマゴ持ちが存在している
 */
//--------------------------------------------------------------
static BOOL Guru2MainCommEggDataNGCheck( GURU2MAIN_WORK *g2m )
{
	int count = 0;
	u32 bit = g2m->g2c->comm_game_egg_check_error_bit;
	
	if( bit ){
		return( TRUE );
	}
	
	return( FALSE );
}

//==============================================================================
//	data
//==============================================================================
//--------------------------------------------------------------
///	ビットマップウィンドウ　会話
//--------------------------------------------------------------
static const BMPWIN_DAT DATA_Guru2BmpTalkWinList[GURU2TALKWIN_MAX] =
{
	BGF_M_KAIWA, 2, 19, 26, 4,
	BGF_PANO_MENU_WIN, BGF_BMPCHAR_TALK,
};

//--------------------------------------------------------------
///	ビットマップウィンドウ　名前　人数 2
//--------------------------------------------------------------
static const BMPWIN_DAT DATA_Guru2BmpNameWin_2[2] =
{
	{
		BGF_M_KAIWA,
		BMPPOS_NAME_X_2_0,BMPPOS_NAME_Y_2_0,
		BMPSIZE_NAME_X,BMPSIZE_NAME_Y,
		BGF_PANO_MENU_WIN, BGF_BMPCHAR_NAME(0),
	},
	{
		BGF_M_KAIWA,
		BMPPOS_NAME_X_2_1,BMPPOS_NAME_Y_2_1,
		BMPSIZE_NAME_X,BMPSIZE_NAME_Y,
		BGF_PANO_MENU_WIN, BGF_BMPCHAR_NAME(1),
	},
};

//--------------------------------------------------------------
///	ビットマップウィンドウ　名前　人数 3
//--------------------------------------------------------------
static const BMPWIN_DAT DATA_Guru2BmpNameWin_3[3] =
{
	{
		BGF_M_KAIWA,
		BMPPOS_NAME_X_3_0,BMPPOS_NAME_Y_3_0,
		BMPSIZE_NAME_X,BMPSIZE_NAME_Y,
		BGF_PANO_MENU_WIN, BGF_BMPCHAR_NAME(0),
	},
	{
		BGF_M_KAIWA,
		BMPPOS_NAME_X_3_1,BMPPOS_NAME_Y_3_1,
		BMPSIZE_NAME_X,BMPSIZE_NAME_Y,
		BGF_PANO_MENU_WIN, BGF_BMPCHAR_NAME(1),
	},
	{
		BGF_M_KAIWA,
		BMPPOS_NAME_X_3_2,BMPPOS_NAME_Y_3_2,
		BMPSIZE_NAME_X,BMPSIZE_NAME_Y,
		BGF_PANO_MENU_WIN, BGF_BMPCHAR_NAME(2),
	},
};

//--------------------------------------------------------------
///	ビットマップウィンドウ　名前　人数 4
//--------------------------------------------------------------
static const BMPWIN_DAT DATA_Guru2BmpNameWin_4[4] =
{
	{
		BGF_M_KAIWA,
		BMPPOS_NAME_X_4_0,BMPPOS_NAME_Y_4_0,
		BMPSIZE_NAME_X,BMPSIZE_NAME_Y,
		BGF_PANO_MENU_WIN, BGF_BMPCHAR_NAME(0),
	},
	{
		BGF_M_KAIWA,
		BMPPOS_NAME_X_4_1,BMPPOS_NAME_Y_4_1,
		BMPSIZE_NAME_X,BMPSIZE_NAME_Y,
		BGF_PANO_MENU_WIN, BGF_BMPCHAR_NAME(1),
	},
	{
		BGF_M_KAIWA,
		BMPPOS_NAME_X_4_2,BMPPOS_NAME_Y_4_2,
		BMPSIZE_NAME_X,BMPSIZE_NAME_Y,
		BGF_PANO_MENU_WIN, BGF_BMPCHAR_NAME(2),
	},
	{
		BGF_M_KAIWA,
		BMPPOS_NAME_X_4_3,BMPPOS_NAME_Y_4_3,
		BMPSIZE_NAME_X,BMPSIZE_NAME_Y,
		BGF_PANO_MENU_WIN, BGF_BMPCHAR_NAME(3),
	},
};

//--------------------------------------------------------------
///	ビットマップウィンドウ　名前　人数 5
//--------------------------------------------------------------
static const BMPWIN_DAT DATA_Guru2BmpNameWin_5[5] =
{
	{
		BGF_M_KAIWA,
		BMPPOS_NAME_X_5_0,BMPPOS_NAME_Y_5_0,
		BMPSIZE_NAME_X,BMPSIZE_NAME_Y,
		BGF_PANO_MENU_WIN, BGF_BMPCHAR_NAME(0),
	},
	{
		BGF_M_KAIWA,
		BMPPOS_NAME_X_5_1,BMPPOS_NAME_Y_5_1,
		BMPSIZE_NAME_X,BMPSIZE_NAME_Y,
		BGF_PANO_MENU_WIN, BGF_BMPCHAR_NAME(1),
	},
	{
		BGF_M_KAIWA,
		BMPPOS_NAME_X_5_2,BMPPOS_NAME_Y_5_2,
		BMPSIZE_NAME_X,BMPSIZE_NAME_Y,
		BGF_PANO_MENU_WIN, BGF_BMPCHAR_NAME(2),
	},
	{
		BGF_M_KAIWA,
		BMPPOS_NAME_X_5_3,BMPPOS_NAME_Y_5_3,
		BMPSIZE_NAME_X,BMPSIZE_NAME_Y,
		BGF_PANO_MENU_WIN, BGF_BMPCHAR_NAME(3),
	},
	{
		BGF_M_KAIWA,
		BMPPOS_NAME_X_5_4,BMPPOS_NAME_Y_5_4,
		BMPSIZE_NAME_X,BMPSIZE_NAME_Y,
		BGF_PANO_MENU_WIN, BGF_BMPCHAR_NAME(4),
	},
};

//--------------------------------------------------------------
///	ビットマップウィンドウ　名前　人数別
//--------------------------------------------------------------
static const BMPWIN_DAT * const
	DATA_Guru2BmpNameWinTbl[G2MEMBER_MAX+1] =
{
	NULL,					//ダミーデータ
	NULL,					//ダミーデータ
	DATA_Guru2BmpNameWin_2,
	DATA_Guru2BmpNameWin_3,
	DATA_Guru2BmpNameWin_4,
	DATA_Guru2BmpNameWin_5,
};

//--------------------------------------------------------------
//	人数別皿初期角度
//--------------------------------------------------------------
static const u16 DATA_DiscOffsetAngle[G2MEMBER_MAX+1][G2MEMBER_MAX] =
{
	{0,0,0,0,0},		//0 ダミー
	{0,0,0,0,0},		//1 ダミー
	{0,180,0,0,0},		//2
	{0,240,120,0,0},	//3
	{0,270,180,90,0},	//4
	{0,288,216,144,72},	//5
};

//--------------------------------------------------------------
//	人数別卵初期位置
//--------------------------------------------------------------
static const u16 DATA_EggStartAngle[G2MEMBER_MAX+1][G2MEMBER_MAX] =
{
	{0,0,0,0,0},			//0 ダミー
	{0,0,0,0,0},			//1 ダミー
	{90,270,0,0,0},			//2
	{90,210,330,0,0},		//3
	{90,180,270,0,0},		//4 
	{90,162,234,306,18},	//5
};

//--------------------------------------------------------------
///	人数別名前表示オフセット
//--------------------------------------------------------------
static const int DATA_NameWinNoOffset[G2MEMBER_MAX+1][G2MEMBER_MAX] =
{
	{0,0,0,0,0},			//0 ダミー
	{0,0,0,0,0},			//1 ダミー
	{0,1,0,0,0},			//2
	{0,2,1,0,0},			//3
	{0,0,0,0,0},			//4
	{0,0,0,0,0},			//5
};

#if 0
//--------------------------------------------------------------
///	終了角度兼角度増大率
//--------------------------------------------------------------
static const u16 DATA_GameEndAngle[G2MEMBER_MAX+1] =
{
	0,		//0　ダミー
	0,		//1　ダミー
	180,	//2
	120,	//3
	90,		//4
	72,		//5
};
#endif

//--------------------------------------------------------------
///	おまけエリア
//--------------------------------------------------------------
static const OMAKE_AREA DATA_OmakeArea2 =	///<二人
{ 0, 180 };

static const OMAKE_AREA DATA_OmakeArea3[2] =	///<三人
{
	{ 0, 198 },
	{ 342, 359 },
};

static const OMAKE_AREA DATA_OmakeArea4[2] =	///<四人
{
	{ 37, 142 },
	{ 215, 323 },
};

static const OMAKE_AREA DATA_OmakeArea5[2] =	///<五人
{
	{ 0, 215 },
	{ 323, 359 },
};

///おまけエリアまとめ
static const OMAKE_AREA_TBL DATA_OmakeAreaTbl[G2MEMBER_MAX+1] =
{
	{ 0, NULL },			//dummy
	{ 0, NULL },			//dummy
	{ 1, &DATA_OmakeArea2 },	//2
	{ 2, DATA_OmakeArea3 },	//3
	{ 2, DATA_OmakeArea4 },	//4
	{ 2, DATA_OmakeArea5 },	//5
};

//--------------------------------------------------------------
///	木の実テーブル
//--------------------------------------------------------------
static const u32 DATA_KinomiTbl[G2MEMBER_MAX+1][2] =
{
	{0,0},	//0 dummy
	{0,0},	//1 dummy
	{159,163}, //2
	{159,163}, //3
	{164,168}, //4
	{175,183}, //5
};

//--------------------------------------------------------------
///	人数別皿モデルアーカイブID
//--------------------------------------------------------------
static const u32 DATA_SaraArcIdxTbl[G2MEMBER_MAX+1] =
{
	0,	//0 dummy
	0,	//1 dummy
	NARC_guru2_g_panel02_c_nsbmd,
	NARC_guru2_g_panel03_c_nsbmd,
	NARC_guru2_g_panel04_c_nsbmd,
	NARC_guru2_g_panel05_c_nsbmd,
};

//--------------------------------------------------------------
///	人数別皿描画角度オフセット
//--------------------------------------------------------------
static const fx32 DATA_DiscRotateDrawOffset[G2MEMBER_MAX+1] =
{
	0,	//0 dummy
	0,	//1 dummy
	DISC_ROTATE_DRAW_OFFS_FX_2,
	DISC_ROTATE_DRAW_OFFS_FX_3,
	DISC_ROTATE_DRAW_OFFS_FX_4,
	DISC_ROTATE_DRAW_OFFS_FX_5,
};

//==============================================================================
//	debug
//==============================================================================
#ifdef DEBUG_DISP_CHECK
static void DEBUG_CameraTest( GURU2MAIN_WORK *g2m );
static void DEBUG_DiscMove( GURU2MAIN_WORK *g2w );
static void DEBUG_OmakeCheck( GURU2MAIN_WORK *g2m );
#endif

#ifdef GURU2_DEBUG_ON
static void DEBUG_WorkInit( GURU2MAIN_WORK *g2m )
{
	DEBUGWORK *dw = &g2m->debug;
	memset( dw, 0, sizeof(DEBUGWORK) );
	
	dw->angle_x = CM_ANGLE_X;
	dw->angle_y = CM_ANGLE_Y;
	dw->angle_z = CM_ANGLE_Z;
	dw->persp = CM_PERSP;
	dw->distance = CAMERA_DISTANCE;
}

static void DEBUG_Proc( GURU2MAIN_WORK *g2m )
{
	DEBUGWORK *dw = &g2m->debug;
	
	#ifdef DEBUG_DISP_CHECK
	if( Guru2Pad_TrgCheck(PAD_BUTTON_SELECT) ){
		dw->type++;
		dw->type %=3;
		
		switch( dw->type ){
		case 0:	OS_Printf( "ぐるぐる　カメラテスト\n" ); break;
		case 1:	OS_Printf( "ぐるぐる　皿テスト\n" ); break;
		case 2: OS_Printf( "ぐるぐる　おまけエリアテスト\n" );
		}
	}
	
	if( dw->type == 0 ){
		DEBUG_CameraTest( g2m );
	}else if( dw->type == 1 ){
		DEBUG_DiscTest( g2m );
	}else if( dw->type == 2 ){
		DEBUG_OmakeCheck( g2m );
	}
	#endif	//DEBUG_DISP_CHECK
}
#endif //GURU2_DEBUG_ON

#ifdef DEBUG_DISP_CHECK
//--------------------------------------------------------------
//	カメラテスト
//--------------------------------------------------------------
static void DEBUG_CameraTest( GURU2MAIN_WORK *g2m )
{
	DEBUGWORK *dw = &g2m->debug;
	CAMERAWORK *cm = &g2m->camera;
	
	if( Guru2Pad_ContCheck(PAD_BUTTON_Y) ){		//カメラアングル
		if( Guru2Pad_ContCheck(PAD_BUTTON_R) ){
			if( Guru2Pad_ContCheck(PAD_KEY_UP) ){
				dw->angle_z -= 1;
			}else if( Guru2Pad_ContCheck(PAD_KEY_DOWN) ){
				dw->angle_z += 1;
			}
		}else{
			if( Guru2Pad_ContCheck(PAD_KEY_UP) ){
				dw->angle_y -= 1;
			}else if( Guru2Pad_ContCheck(PAD_KEY_DOWN) ){
				dw->angle_y += 1;
			}else if( Guru2Pad_ContCheck(PAD_KEY_LEFT) ){
				dw->angle_x -= 1;
			}else if( Guru2Pad_ContCheck(PAD_KEY_RIGHT) ){
				dw->angle_x += 1;
			}
		}
	}else if( Guru2Pad_ContCheck(PAD_BUTTON_A) ){		//カメラ距離
		if( Guru2Pad_ContCheck(PAD_KEY_UP) ){
			cm->distance -= FX32_ONE;
		}else if( Guru2Pad_ContCheck(PAD_KEY_DOWN) ){
			cm->distance += FX32_ONE;
		}
	}else if( Guru2Pad_ContCheck(PAD_BUTTON_B) ){		//カメラパース
		if( Guru2Pad_ContCheck(PAD_KEY_UP) ){
			dw->persp -= 1;
		}else if( Guru2Pad_ContCheck(PAD_KEY_DOWN) ){
			dw->persp += 1;
		}
	}
	
	dw->angle_x %= 360;
	dw->angle_y %= 360;
	dw->angle_z %= 360;
	dw->persp %= 360;
	
	if( Guru2Pad_TrgCheck(PAD_BUTTON_X) ){
		OS_Printf( "ぐるぐる　カメラ設定 ID=%d ", CommGetCurrentID() );
		OS_Printf( "angle X=%d, Y=%d, Z=%d, ",
			dw->angle_x,dw->angle_y,dw->angle_z );
		OS_Printf( "distance 0x%x : ", cm->distance );
		OS_Printf( "persp %d\n", dw->persp ); 
	}
	
	cm->angle.x = FX_GET_ROTA_NUM( dw->angle_x );
	cm->angle.y = FX_GET_ROTA_NUM( dw->angle_y );
	cm->angle.z = FX_GET_ROTA_NUM( dw->angle_z );
	cm->persp = FX_GET_ROTA_NUM( dw->persp );
	
	guru2_CameraSet( g2m );
}
#endif	//DEBUG_DISP_CHECK

#ifdef DEBUG_DISP_CHECK
void DEBUG_DiscTest( GURU2MAIN_WORK *g2m )
{
	DISCWORK *disc = &g2m->disc;
	
	if( Guru2Pad_ContCheck(PAD_BUTTON_Y) ){	//皿角度
		if( Guru2Pad_ContCheck(PAD_BUTTON_R) ){
			if( Guru2Pad_ContCheck(PAD_KEY_UP) ){
				disc->rotate.z -= 1;
			}else if( Guru2Pad_ContCheck(PAD_KEY_DOWN) ){
				disc->rotate.z += 1;
			}
		}else{
			if( Guru2Pad_ContCheck(PAD_KEY_UP) ){
				disc->rotate_fx -= NUM_FX32( 1 );
			}else if( Guru2Pad_ContCheck(PAD_KEY_DOWN) ){
				disc->rotate_fx += NUM_FX32( 1 );
			}else if( Guru2Pad_ContCheck(PAD_KEY_LEFT) ){
				disc->rotate.x -= 1;
			}else if( Guru2Pad_ContCheck(PAD_KEY_RIGHT) ){
				disc->rotate.x += 1;
			}
		}
	}else{										//皿座標
		if( Guru2Pad_ContCheck(PAD_BUTTON_R) ){
			if( Guru2Pad_ContCheck(PAD_KEY_UP) ){
				disc->pos.z -= FX32_ONE;
			}else if( Guru2Pad_ContCheck(PAD_KEY_DOWN) ){
				disc->pos.z += FX32_ONE;
			}
		}else{
			if( Guru2Pad_ContCheck(PAD_KEY_UP) ){
				disc->pos.y -= FX32_ONE;
			}else if( Guru2Pad_ContCheck(PAD_KEY_DOWN) ){
				disc->pos.y += FX32_ONE;
			}else if( Guru2Pad_ContCheck(PAD_KEY_LEFT) ){
				disc->pos.x -= FX32_ONE;
			}else if( Guru2Pad_ContCheck(PAD_KEY_RIGHT) ){
				disc->pos.x += FX32_ONE;
			}
		}
	}
	
	disc->rotate.x %= 360;
	disc->rotate.y %= 360;
	disc->rotate.z %= 360;
	
	if( Guru2Pad_TrgCheck(PAD_BUTTON_X) ){
		OS_Printf( "ぐるぐる　皿設定 ID=%d ", CommGetCurrentID() );
		OS_Printf( "disc X=%d, Y=%d, Z=%d, AX=%d, AY=%d, AZ=%d\n",
			disc->pos.x/FX32_ONE,disc->pos.y/FX32_ONE,disc->pos.z/FX32_ONE,
			disc->rotate.x,FX32_NUM(disc->rotate_fx),disc->rotate.z );
	}
}
#endif	//DEBUG_DISP_CHECK

#ifdef DEBUG_DISP_CHECK
static void DEBUG_OmakeCheck( GURU2MAIN_WORK *g2m )
{
	int hit = FALSE;
	fx32 pos;
	EGGACTOR *eact;
	DISCWORK *disc = &g2m->disc;
	
	if( Guru2Pad_ContCheck(PAD_BUTTON_Y) ){		//皿角度
		if( Guru2Pad_ContCheck(PAD_KEY_UP) ){
			Disc_Rotate( disc, NUM_FX32(1) );
		}else if( Guru2Pad_ContCheck(PAD_KEY_DOWN) ){
			Disc_Rotate( disc, NUM_FX32(-1) );
		}
	}
	
	eact = EggAct_FrontEggActGet( g2m );
	pos = eact->angle;
	hit = Guru2MainOmakeZoneCheck( g2m, pos, g2m->comm.play_max );
	
	if( Guru2Pad_TrgCheck(PAD_BUTTON_X) ){
		int d_pos,e_pos,h_pos;
		fx32 dmy;
		
		if( hit == TRUE ){
			OS_Printf( "ぐるぐる　おまけエリア　あたり！　" );
		}else{
			OS_Printf( "ぐるぐる　おまけエリア　はずれ…　" );
		}
		
		d_pos = FX32_NUM( disc->rotate_fx );
		e_pos = FX32_NUM( pos );
		
		dmy = pos;
		AngleAdd( &dmy, -disc->rotate_fx );
		h_pos = FX32_NUM( dmy );
		
		OS_Printf( "ID = %d, disc angle = %d(%d), egg pos = %d(%d,%d)\n",
			CommGetCurrentID(),
			d_pos, 360-d_pos, e_pos, 360-e_pos, h_pos );
	}
}
#endif	//DEBUG_DISP_CHECK

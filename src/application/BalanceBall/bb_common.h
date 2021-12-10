//==============================================================================
/**
 * @file	bb_common.h
 * @brief	簡単な説明を書く
 * @author	goto
 * @date	2007.09.25(火)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================

#ifndef __BB_COMMON_H__
#define __BB_COMMON_H__

#include "bb_local.h"
#include "bb_game.h" 


#define HEAPID_BB			( HEAPID_BALANCE_BALL )
#define HEAP_SIZE_BB		( 0x65000 )

#define ARCID_BB_RES		( ARC_BALANCE_BALL_GRA )
#define ARCID_COMMON_RES	( ARC_BUCKET_GRAPHIC )

#define BB_BG_CGX_OFS		( 180 )
#define BB_CGX_OFS			( 256 )

enum {
	
	BB_TCB_PRI_1 = 0x1000,
	BB_TCB_PRI_2,
	
};


#define BB_MODEL_OFS_X		( 0 * FX32_ONE )
#define BB_MODEL_OFS_Y		( -20 * FX32_ONE )
#define BB_MODEL_OFS_Y2		( -8 * FX32_ONE )
#define BB_MODEL_OFS_Z		( 0 * FX32_ONE )
#define BB_MODEL_OFS_SPOT_Z	( +30 * FX32_ONE )

#define BB_LIGHT_NO			( 0 )
#define BB_LIGHT_VX			( 0 )
#define BB_LIGHT_VY			( -FX32_ONE )
#define BB_LIGHT_VZ			( -FX32_ONE )

#define BB_CAMERA_PERSPWAY	( 0x0b60 )
#define BB_CAMERA_NEAR_CLIP	( FX32_ONE * -1000 )
#define BB_CAMERA_NEAR_FAR	( FX32_ONE * 1000 )
#define BB_CAMERA_TX		( 0 )		/// target
#define BB_CAMERA_TY		( 0 )
#define BB_CAMERA_TZ		( 0 )
#define BB_CAMERA_PX		( 0 )		/// pos
#define BB_CAMERA_PY		( 0 )
#define BB_CAMERA_PZ		( FX32_ONE * 160 )//( FX32_ONE * 144 )

#define BB_TOUCH_HIT_NUM	( 1 )		///< 当たり判定の数

#define BB_BALL_HIT_CX		( 122 )
#define BB_BALL_HIT_CY		( 143 )
#define BB_BALL_HIT_R		(  36 )
#define BB_BALL_HIT_D		( BB_BALL_HIT_R * 2 )

#define BB_SURFECE_OFS	( 64 )
#define BB_OAM_OX		( 0 )
#define BB_MANE_PY		( 176 + BB_SURFECE_OFS )
#define BB_MANE_OY		( 0 )
#define BB_BALL_PY		( BB_MANE_PY + BB_MANE_OY )

#define BB_OAM_CD_PX	( 128 )
#define BB_OAM_CD_PY	(  96 )

#define BB_KAMI_BLOCK	( 3 )
#define BB_KAMI_LINE	( 12 )
#define BB_KAMI_MAX		( BB_KAMI_BLOCK * BB_KAMI_LINE )
#define BB_HAND_MAX		( 4 )

#define STAR_DUST_EFF_MAX	( 8 )
#define STAR_DUST_MAX		( 3 )
#define STAR_DUST_EFF_WAIT	( 8 )

#define BB_KAMI_HUBUKI_MAX	( 8 )
#define BB_LIGHT_MAX		( 2 )
#define BB_COLOR_FADE_COL		( 0x0000 )
#define BB_COLOR_FADE_MAX		( 4 )
#define BB_COLOR_FADE_MIN		( 0 )
#define BB_COLOR_FADE_F_MIN		( 4 )
#define BB_COLOR_FADE_F_MAX		( 6 )
//#define BB_COLOR_FADE_MAX		( 10 )
#define BB_COLOR_FADE_PAL_OFS	( 0x0001 )


///< ルール
enum {	
	eBB_RULE_BALL_L		= 0,
	eBB_RULE_BALL_N,
	eBB_RULE_BALL_H,
};


///< 通信同期用
enum {	
	BB_SYNCID_NONE,
	BB_SYNCID_END,	
};

///< ゲームルール通信
typedef struct {
	
	u8		ball_type;		///< 1 = レアゲーム	
	u32		rand;
		
} BB_RULE_COMM;

///< ゲーム通信
enum {
	eBB_STATE_NONE = 0,
	eBB_STATE_FALL,
	eBB_STATE_RECOVER,
	eBB_STATE_RESET,
};
typedef struct {

//	u16		netid;
	u32		state;
//	BOOL	state;	///< TRUE = 落ちた

} BB_STATE_COMM;


///< スコア通信
typedef struct {
	
	u32 score;
	u8	rank;

} BB_RESULT_COMM;


///< メンバーのスコア通信
typedef struct {
	
	BB_RESULT_COMM	result[ BB_PLAYER_NUM ];

} BB_MENBER_RESULT_COMM;


///< 3D ball
typedef struct {	
    D3DOBJ				obj;
    D3DOBJ_MDL			mdl;
    D3DOBJ_ANM			anm[ 10 ];			///< とりあえず10個        
	QUATERNION_MTX44	rt;					///< 計算用マトリックス
	QUATERNION			tq;					///< クォータニオン
	QUATERNION			cq;					///< クォータニオン
	MtxFx43				tmp43;				///< 実際の表示部分マトリックス
	VecFx32				pos;
	BOOL				bAnime;
	BOOL				bInit;
	int					anime_no;	
	int					pow_count;
	f32					get_pow_x;
	f32					get_pow_y;
	f32					get_pow_tx;
	f32					get_pow_ty;
} BB_3D_MODEL;


///< コントローラー
typedef struct {
	
	u32 old_x;
	u32 old_y;
	
	int frame;
	
} BB_CONTROL;

typedef struct {
	
	fx32 x;
	fx32 s_x;
	fx32 s_s;	// 初速度
	fx32 s_a;	// 加速度
	int count;
	int count_max;
	
} BB_ADDMOVE_WORK_FX;

///< 落ちるとき、上るときのTCB用ワーク
typedef struct {
	
	int				seq;
	int				type;
	int				wait;
	CATS_ACT_PTR	cap_mane;		///< 影ワークにも使う
	CATS_ACT_PTR	cap_ball;

	BB_ADDMOVE_WORK_FX data;

	BB_SYS*	sys;
		
} BB_MANENE_ACTION;

typedef struct {
	
	int				seq;
	int				type;
	int				wait;
	CATS_ACT_PTR	cap_kage;		///< 影ワークにも使う

	BB_ADDMOVE_WORK_FX data;
	
	int				old_level;
	int*			level;
	BOOL			bStart;

	BB_3D_MODEL*	model_bs;
	BB_3D_MODEL*	model_up;
	BB_SYS*			sys;
	
	int				rare_game;

} BB_LEVEL_UP;

typedef struct {
	
	int				seq;
	int				type;
	int				wait;
	
	int*			anime_type;
	BB_3D_MODEL*	model;
	BB_3D_MODEL*	model_ball;
	
	s32 mx;
	s32 my;
	
	VecFx32			mane_pos;
	
	BB_ADDMOVE_WORK_FX dx;
	BB_ADDMOVE_WORK_FX dy;
	BB_ADDMOVE_WORK_FX dz;
	
	int				rare_game;

	BB_SYS*			sys;
	
} BB_MANENE_3D_ACTION;

///< ゲームシステム
typedef struct {
	
	int				lot_timer;					///< 抽選タイマー
	int 			game_timer;					///< タイマー(各クライアントが判断する為だけに使う)

	int				eff_level;					///< エフェクトレベル
	int				level;						///< レベル	
	int				point;						///< スコアの元
	int				fever_level;				///< フィーバーレベル
	int				fever_level_old;			///< フィーバーレベル１個前
	s16				recover_wait;				///< 復帰時間

	s16				fever_time;					///< シンクロ発動カウンタ
	s16				l_time[ BB_PLAYER_NUM ];	///< 各ユーザーの生存時間
	int				o_level[ BB_PLAYER_NUM ];	///< 各ユーザーのレベル
	
	int				result_time;				///< 実際に乗っている時間
	
	
	///< 移動用
	s16				pow_x;
	s16				pow_y;
	s16				frame;
	s16				roll_seq;	
	BB_ADDMOVE_WORK_FX	amx;
	BB_ADDMOVE_WORK_FX	amy;	
	BB_ADDMOVE_WORK_FX	ini_amx;
	BB_ADDMOVE_WORK_FX	ini_amy;
	
	BOOL			touch;
	BOOL			inertia;
	BOOL			init_ok;
	
	BOOL			boot_init;
	BOOL			bFirstTouch;
	
	s16				set_pow_x;
	s16				set_pow_y;
		
} BB_GAME_SYS;

#define BB_EFFECT_MAX	( 5 )

///< ライトのワーク
typedef struct {
	
	CATS_ACT_PTR	cap;
	
	int seq;
	s16 rad;
	int wait;
	int	num;

	s16 cx;
	s16 cy;
	int	rx;
	int	ry;
	
	BB_ADDMOVE_WORK_FX	mx;
	BB_ADDMOVE_WORK_FX	my;
	
} BB_LIGHT;

typedef struct {
	
	BB_LIGHT main[ BB_LIGHT_MAX ];
	BB_LIGHT sub[ BB_LIGHT_MAX ];
	
	int num;
	int speed;
	int speed_m;
	
	int cnt;

} BB_LIGHT_MOVE;

///< ライトのエフェクト
typedef struct {
	
	BB_LIGHT_MOVE	mvwk;

	GF_BGL_INI* bgl;

	TCB_PTR	 tcb;
	int*	 level;			///< Level
	int*	 col1;			///< 個人エフェクトの明度
	int*	 col2;			///< フィーバー時の明度
	
	int		 wait;			///< wait

	int		 eff_seq;		///< エフェクト用のseq
	int		 eff_wait;		///< エフェクト用のwait

	int		 seq;			///< Open Close に 使う
	BOOL	 bActive;		///< ふぇーバー始まったか？
	BOOL	 bOpenClose;	///< OpenClose task TRUE = OPEN

	BB_SYS*	 sys;
	BB_ADDMOVE_WORK_FX	scr_move[ 5 ];
	
} BB_FEVER;

///< 個人エフェクト
typedef struct {
	
	int		seq;
	BOOL 	active;
	void*	data[ BB_KAMI_HUBUKI_MAX ];
	int		temp[ 10 ];
	BOOL	flag[ 10 ];
	
} BB_EFF_WORK;

///< 個人エフェクト管理
typedef struct {	

	int			old_level;	///< 1フレ前のレベル
	int* 		level;		///< ゲームシステムが管理してるレベル

	BB_EFF_WORK work[ BB_EFFECT_MAX ];
	TCB_PTR		tcb;
	
	BB_SYS*		sys;

} BB_EFFECT;

///< 星くず
typedef struct {

	BOOL				active;
	int					seq;
	CATS_ACT_PTR		cap[ STAR_DUST_MAX ];
	BB_ADDMOVE_WORK_FX	data[ STAR_DUST_MAX ][ 2 ];
	
	BB_SYS*				sys;

} BB_STARDUST;


///< サーバー
typedef struct {
	
	int dummy;
	
	int comm_num;
	s16	timer;
	
	BB_SYS*	sys;
	
} BB_SERVER;


///< クライアント
typedef struct {
	
	u32 netid;	
	int comm_num;
	int seq;
	int seq2;
	int wait;
	
	BB_SYS*	sys;
	
	CATS_ACT_PTR	cap_cd;
	CATS_ACT_PTR	cap_mane[ BB_PLAYER_NUM - 1 ];
	CATS_ACT_PTR	cap_ball[ BB_PLAYER_NUM - 1 ];
	CATS_ACT_PTR	cap_kage[ BB_PLAYER_NUM - 1 ];

	CATS_ACT_PTR	cap_hanabi[ BB_KAMI_MAX ];
	CATS_ACT_PTR	cap_hand[ BB_HAND_MAX ];
	
	CATS_ACT_PTR	cap_light_s[ BB_LIGHT_MAX ];
	CATS_ACT_PTR	cap_light_m[ BB_LIGHT_MAX ];
	
	CATS_ACT_PTR	cap_pen;

	u16				netid_to_capid[ BB_PLAYER_NUM ];
	int				direc[ BB_PLAYER_NUM - 1 ];	
	int				color1;						///< 画面全体の明暗操作	
	int				color2;						///< 画面全体の明暗操作	
	int				anime_type;					///< アニメーションの種類
	
	BB_3D_MODEL		bb3d_ball;						///< ボール
	BB_3D_MODEL		bb3d_mane[ 3 ];					///< マネネ
	BB_3D_MODEL		bb3d_spot;						///< ライト
	BB_3D_MODEL		bb3d_kami[ BB_KAMI_HUBUKI_MAX ];///< 紙吹雪
	BB_3D_MODEL		bb3d_lvbs;						///< レベルアップ
	BB_3D_MODEL		bb3d_lvup;						///< レベルアップ

	VecFx32			mane_pos;					///< マネネのある１点
	
	BB_CONTROL		control;					///< コントロール（タッチパネル）
	
	GF_BGL_BMPWIN	win;						///< ウィンドウ
	GF_BGL_BMPWIN	win_name[ BB_PLAYER_NUM ];	///< ウィンドウ

	BB_FEVER		fever_sys;					///< シンクロ
	BB_EFFECT		eff_sys;					///< 個人エフェクト管理
	BB_STARDUST		star[ STAR_DUST_EFF_MAX ];	///< 星くずエフェクト
	
	BB_LEVEL_UP		level_sys[ BB_PLAYER_NUM ];	///< レベルアップ
	BB_GAME_SYS		game_sys;					///< ゲームシステム
	BB_STATE_COMM	state[ BB_PLAYER_NUM ];		///< ユーザーの状態
	
	u32				rand;	
	u32				seed_use;
	int				rare_game;
	
	BOOL			bAction;
	
} BB_CLIENT;


///< メインワーク
typedef struct {

	BB_SERVER*		p_server;
	BB_CLIENT*		p_client;

	BOOL			bStart;
	BOOL			bEnd;
	BOOL			bResult;
	
	BB_RULE_COMM	rule;
	BB_RESULT_COMM	result;
	BB_MENBER_RESULT_COMM mem_result;

	u32				netid;
	
	BB_SYS			sys;
	
	u32				seed_tmp;
	
	MNGM_COUNTWK*	count_down;
	
	BB_PROC_WORK*	parent_wk;
	
	MNGM_ENRES_PARAM 	entry_param;
	MNGM_ENTRYWK*	 	entry_work;
	
	MNGM_RESULT_PARAM	result_param;
	MNGM_RESULTWK*		result_work;
	
	int				table[ BB_PLAYER_NUM ];
	
	BOOL			bRePlay;
	
} BB_WORK;


extern void BB_SystemExit( BB_WORK* wk );
extern BOOL IsParentID( BB_WORK* wk );
extern void BB_cmd_StartSet( BB_WORK* wk );
extern void BB_cmd_EndSet( BB_WORK* wk );
extern int NetID_To_PlayerNo( BB_WORK* wk, int net_id );


/// bb_disp.c
extern BOOL Quaternion_Rotation( BB_3D_MODEL* wk, int x, int y, int ox, int oy, f32 pow, BOOL pow_get_set );
extern BOOL Quaternion_Rotation_Pow( BB_3D_MODEL* wk, f32 pow );

extern void BB_disp_3DModelDraw( D3DOBJ* obj, MtxFx43* mat43, VecFx32* pos );
extern void BB_disp_Model_Load( BB_3D_MODEL* wk, ARCHANDLE* p_handle, int id );
extern void BB_disp_Model_Init( BB_3D_MODEL* wk, int flag );
extern void BB_disp_Model_Delete( BB_3D_MODEL* wk );
extern void BB_disp_Draw( BB_WORK* wk );
extern void ManeneAnime_Set( BB_CLIENT* wk, int anime_no );
extern void BB_ManeneAnime_CheckSet( BB_CLIENT* wk );
extern int ManeneAnime_Get( BB_WORK* wk );
extern void ManeneAnime_FlagSet( BB_WORK* wk, BOOL flag );
extern void BB_disp_CameraSet( BB_WORK* wk );
extern void BB_disp_ResourceLoad( BB_WORK* wk );
extern void BB_disp_Hanabi_OAM_Add( BB_CLIENT* wk );
extern void BB_disp_Hanabi_OAM_Enable( BB_CLIENT* wk, BOOL flag, int mode );
extern void BB_disp_Hanabi_OAM_Update( BB_CLIENT* wk );
extern void BB_disp_Hanabi_OAM_Del( BB_CLIENT* wk );
extern void BB_disp_Manene_OAM_AnimeChangeCap( CATS_ACT_PTR cap, int type, int anime );
extern void BB_disp_Manene_OAM_AnimeChange( BB_CLIENT* wk, int netid, int anime );
extern int BB_disp_Manene_OAM_AnimeCheck( BB_CLIENT* wk, int netid );
extern void BB_disp_Manene_OAM_Update( BB_CLIENT* wk );
extern void BB_disp_Manene_OAM_Del( BB_CLIENT* wk );
extern void BB_disp_InfoWinAdd( BB_CLIENT* wk );
extern void BB_disp_InfoWinDel( BB_CLIENT* wk );
extern void BB_disp_NameWinAdd( BB_WORK* bwk, BB_CLIENT* wk );
extern void BB_disp_NameWinDel( BB_CLIENT* wk );

extern s16 Action_MoveValue_2Y( void );
extern s16 Action_MoveValue_3Y( void );
extern s16 Action_MoveValue_3Z( void );

extern BOOL Debug_GameSetup( BB_WORK* work );

///< 新規
extern void BB_disp_BG_Load( BB_WORK* wk );
extern void BB_disp_Manene_Add( BB_WORK* bb_wk, BB_CLIENT* wk );


extern void BB_AddMoveReqFx( BB_ADDMOVE_WORK_FX* p_work, fx32 s_x, fx32 e_x, fx32 s_s, int count_max );
extern BOOL BB_AddMoveMainFx( BB_ADDMOVE_WORK_FX* p_work );
extern void BB_MoveInit_FX( BB_ADDMOVE_WORK_FX* p_data, fx32 s_num, fx32 e_num, s32 count );
extern BOOL BB_MoveMain_FX( BB_ADDMOVE_WORK_FX* p_data );

extern CATS_ACT_PTR BB_disp_Stardust_Add( BB_CLIENT* wk, s16 x, s16 y );
extern void BB_Stardust_Call( BB_CLIENT* wk, s16 x, s16 y );

extern void BB_Effect_Call( BB_CLIENT* wk );
extern void BB_Fever_Call( BB_CLIENT* wk );

extern void BB_disp_Hand_Add( BB_CLIENT* wk );
extern void BB_disp_Hand_Del( BB_CLIENT* wk );

extern void BB_Client_EffectStart( BB_CLIENT* wk, BOOL* flag );
extern void BB_disp_Light_Add( BB_CLIENT* wk );
extern void BB_disp_Light_Del( BB_CLIENT* wk );

extern void BB_disp_Pen_Add( BB_CLIENT* wk );
extern void BB_disp_Pen_Del( BB_CLIENT* wk );

extern void BR_ColorChange( BB_WORK* wk, int id );

#endif	//


//==============================================================================
/**
 * @file	ending_common.h
 * @brief	エンディング共通ヘッダ
 * @author	matsuda
 * @date	2008.04.07(月)
 */
//==============================================================================
#ifndef __ENDING_COMMON_H__
#define __ENDING_COMMON_H__


//==============================================================================
//	デバッグ用定義
//==============================================================================
#ifdef PM_DEBUG
///有効にしているとシーン毎にYボタンでスキップ可能 
//#define DEBUG_ENDING_SCENE_SKIP
///有効にしているとカメラや配置物をいじれる
//#define DEBUG_ENDING_CAMERA
#endif


//--------------------------------------------------------------
//	システム
//--------------------------------------------------------------
///システムフォントパレットの展開位置
#define SYSTEM_FONT_PAL			(15)

///ブライトネスフェード対象面(スタッフロール面のみ除く)
#define ENDING_FADE_PLANE		(PLANEMASK_BG0 | PLANEMASK_BG2 | PLANEMASK_BG3 | PLANEMASK_OBJ | PLANEMASK_OBJ | PLANEMASK_BD)

///シーン切り替えのブライトネスフェード速度
#define ENDING_FADE_SYNC_SCENE		(24)
///キャプチャ画像切り替えのブライトネスフェード速度
#define ENDING_FADE_SYNC_CAPTURE	(32)

///BGフレーム設定
#define FRAME_M_STAFFROLL		(GF_BGL_FRAME1_M)	//スタッフロール面
#define FRAME_M_BG				(GF_BGL_FRAME2_M)	//背景
#define FRAME_M_CAPTURE			(GF_BGL_FRAME3_M)	//キャプチャ
#define FRAME_S_STAFFROLL		(GF_BGL_FRAME1_S)	//スタッフロール
#define FRAME_S_EFF				(GF_BGL_FRAME2_S)	//エフェクト(未使用)
#define FRAME_S_BG				(GF_BGL_FRAME3_S)	//背景

///BGフレーム設定(フィナーレ以降)
#define FRAME_FIN_M_BG			(GF_BGL_FRAME1_M)	//背景
#define FRAME_FIN_M_CAP_A		(GF_BGL_FRAME2_M)	//キャプチャA
#define FRAME_FIN_M_CAP_B		(GF_BGL_FRAME3_M)	//キャプチャB
#define FRAME_FIN_S_BG			(GF_BGL_FRAME3_S)	//背景

///BGプライオリティ
enum{
	FRAME_BGPRI_3D = 1,
	
	FRAME_BGPRI_M_STAFFROLL = 0,
	FRAME_BGPRI_M_BG = 3,
	FRAME_BGPRI_M_CAPTURE = 2,
	
	FRAME_BGPRI_S_STAFFROLL = 0,
	FRAME_BGPRI_S_EFF = 1,
	FRAME_BGPRI_S_BG = 3,
	
	FRAME_BGPRI_FIN_M_CAPTURE_TOP = 2,
	FRAME_BGPRI_FIN_M_CAPTURE_BOTTOM = 3,
};

///エンディングのシーン数
enum{
	ENDING_SCENE_MAP1,
	ENDING_SCENE_CAPTURE1,
	ENDING_SCENE_MAP2,
	ENDING_SCENE_CAPTURE2,
	ENDING_SCENE_MAP3,
	ENDING_SCENE_FIELD,
	ENDING_SCENE_COPYRIGHT,

	ENDING_SCENE_MAX,
};

///エンディング画面遷移フレーム値	※エンディング曲 4分37秒(8310フレーム)
enum{
	ENDING_COUNTER_SCENE1_FUWANTE_0 = 30 * 30,
	ENDING_COUNTER_SCENE1_FUWANTE_1 = 45 * 30,
	ENDING_COUNTER_SCENE1_FUWANTE_2 = 53 * 30,
	ENDING_COUNTER_SCENE1_END = 61 * 30,
	
	ENDING_COUNTER_CAP_SCENE1_END = 160 * 30 + 15,
	
	//-- フィールド：シーン２ --//
	ENDING_COUNTER_SCENE2_KYAMOME_ESCAPE = 199 * 30,
	ENDING_COUNTER_SCENE2_END = 200 * 30,
	
	ENDING_COUNTER_CAP_SCENE2_END = 232 * 30,
	
	//-- フィールド：シーン３ --//
	ENDING_COUNTER_SCENE3_STAR_0 = 240 * 30,
	ENDING_COUNTER_SCENE3_STAR_1 = 246 * 30,
	ENDING_COUNTER_SCENE3_STAR_2 = 248 * 30,
	ENDING_COUNTER_SCENE3_JIBA = 248 * 30,
	ENDING_COUNTER_SCENE3_PALANM_START = 254 * 30,
	ENDING_COUNTER_SCENE3_END = 266 * 30,
	
	ENDING_COUNTER_FIELD_END = ENDING_COUNTER_SCENE3_END + 30*10,
	ENDING_COUNTER_COPYRIGHT_END = ENDING_COUNTER_FIELD_END + 60*30,
};

///マップシーン２で使用するパレットアニメの本数
#define ENDING_SCENE2_PALANM_NUM		(4)

///マップシーン３で使用するパレットアニメの本数
#define ENDING_SCENE3_PALANM_NUM		(8)
///マップシーン３のパレットアニメ展開位置
#define ENDING_SCENE3_PALANM_PAL_POS	(1)

//--------------------------------------------------------------
//	アクター
//--------------------------------------------------------------
///サブサーフェースY(fx32)
#define ENDING_SUB_ACTOR_DISTANCE		((192 + 80) << FX32_SHIFT)
///サブサーフェースY(int)
#define ENDING_SUB_ACTOR_DISTANCE_INTEGER		(ENDING_SUB_ACTOR_DISTANCE >> FX32_SHIFT)

///アクターインデックス
enum{
	//シーンマップ１
	ACT_INDEX_SCENE1_HERO = 0,
	ACT_INDEX_SCENE1_HERO_SCARF,
	ACT_INDEX_SCENE1_FUWANTE_0,
	ACT_INDEX_SCENE1_FUWANTE_1,
	ACT_INDEX_SCENE1_FUWANTE_2,
	
	//シーンマップ２
	ACT_INDEX_SCENE2_HERO = 0,
	ACT_INDEX_SCENE2_HERO_SCARF,
	ACT_INDEX_SCENE2_KYAMOME_0,
	ACT_INDEX_SCENE2_KYAMOME_1,

	//シーンマップ３
	ACT_INDEX_SCENE3_HERO = 0,
	ACT_INDEX_SCENE3_HERO_SCARF,
	ACT_INDEX_SCENE3_JIBA_0,
	ACT_INDEX_SCENE3_STAR_MAIN,
	ACT_INDEX_SCENE3_STAR_SUB,
	
	ACT_INDEX_MAX = ACT_INDEX_SCENE1_FUWANTE_2 + 1,		//各シーンの中で一番大きいものを指定
};

//--------------------------------------------------------------
//	アクターBGプライオリティ
//--------------------------------------------------------------
enum{
	ACTBGPRI_HERO = 1,			///<主人公
	ACTBGPRI_STAR = 3,			///<流れ星
	
	//-- サブ画面 --//
	ACTBGPRI_SUB_FUWANTE = 2,		///<フワンテ
	ACTBGPRI_SUB_KYAMOME = 2,		///<キャモメ
	ACTBGPRI_SUB_JIBA = 2,			///<ジバコイル
	ACTBGPRI_SUB_STAR = 3,			///<流れ星
};

//--------------------------------------------------------------
//	アクターソフトプライオリティ
//--------------------------------------------------------------
enum{
	SOFTPRI_HERO = 51,			///<主人公
	SOFTPRI_HERO_SCARF = 60,	///<主人公のスカーフ
	
	SOFTPRI_STAR = 200,			///<流れ星
	
	//-- サブ画面 --//
	SOFTPRI_SUB_FUWANTE = 150,		///<フワンテ
	SOFTPRI_SUB_KYAMOME = 150,		///<キャモメ
	SOFTPRI_SUB_JIBA = 150,			///<ジバコイル
	SOFTPRI_SUB_STAR = 200,			///<流れ星
};

//--------------------------------------------------------------
//	3Dモデル
//--------------------------------------------------------------
///モデル最大使用数
#define ENDING_MODEL_MAX		(10)

///モデルデータIndex
enum{
	//シーン１
	ENDING_SCENE1_MDL_TREE_A = 0,
	ENDING_SCENE1_MDL_TREE_B,
	
	//シーン２
	ENDING_SCENE2_MDL_LIGHT = 0,
	
	//シーン３
	ENDING_SCENE3_MDL_TREE_A = 0,
	ENDING_SCENE3_MDL_TREE_A2,
	ENDING_SCENE3_MDL_TREE_B,
	ENDING_SCENE3_MDL_LIGHT,
	
	
	ENDING_SCENE_MDL_MAX = ENDING_SCENE3_MDL_LIGHT + 1,	//各シーンの中で一番大きいのを指定
};

///各シーンで使用する3DOBJの最大数(ライン単位)
#define ENDING_SCENE_LINE_3DOBJ_MAX		(16)

///ライン数
enum{
	MODEL_LINE_0,
	MODEL_LINE_1,
	MODEL_LINE_MAX,
};

//==============================================================================
//	構造体定義
//==============================================================================
///スタッフロール制御構造体への不定形ポインタ
typedef struct _PL_ENDING_LIST_WORK	PL_ENDING_LIST_WORK;

///配置物のライン配置データ
typedef struct{
	s16 mdl_num;		///<STARTとENDの間にいくつモデルを置くか
	s16 move_frame;		///<何フレームでSTARTからENDまで移動させるか
#if 0
	fx32 start_x;
	fx32 end_x;
	fx32 start_y;
	fx32 end_y;
	fx32 start_z;
	fx32 end_z;
#else
	fx32 end_x;
	fx32 space_x;
	fx32 end_y;
	fx32 space_y;
	fx32 end_z;
	fx32 space_z;
#endif
}MDL_LINE_ARRANGE_DATA;

///配置物の配置データ
typedef struct{
	MDL_LINE_ARRANGE_DATA arrange[MODEL_LINE_MAX];
}MDL_ARRANGE_DATA;

///フワンテ動作ワーク
typedef struct{
	fx32 x;
	fx32 y;
	fx32 theta;
	s32 start_wait;
	s16 scale_count;
	s8 scale_dir;
}FUWANTE_WORK;

///キャモメ動作ワーク
typedef struct{
	fx32 x;
	fx32 y;
	fx32 add_x;
	fx32 add_y;
	fx32 theta;
	fx32 theta_x;
	fx32 theta_y;
	fx32 add_theta;
	fx32 add_theta_x;
	fx32 add_theta_y;
	fx32 offset_x;
	fx32 offset_y;
	s32 move_frame;
	s16 wait;
	s16 rotate_count;
	s8 rotate_dir;
	u8 seq;
}KYAMOME_WORK;

///ジバコイル動作ワーク
typedef struct{
	fx32 x;
	fx32 y;
	fx32 theta;
	s16 rotate_count;
	s8 rotate_dir;
}JIBA_WORK;

///シーン：マップ1のワーク
typedef struct{
	fx32 bg_scr_x;
	fx32 bg_scr_y;
	FUWANTE_WORK fuwante_work[ACT_INDEX_SCENE1_FUWANTE_2 - ACT_INDEX_SCENE1_FUWANTE_0 + 1];
}SCENE_WORK_MAP1;

///シーン：マップ2のワーク
typedef struct{
	fx32 bg_scr_x;
	fx32 bg_scr_y;
	u16 palanm_buf[ENDING_SCENE2_PALANM_NUM][16];
	int anm_pos;
	int evy;							///<EVY値(下位8ビット小数)
	int evy_dir;						///<EVYの加減算の方向
	KYAMOME_WORK kyamome_work[ACT_INDEX_SCENE2_KYAMOME_1 - ACT_INDEX_SCENE2_KYAMOME_0 + 1];
}SCENE_WORK_MAP2;

///シーン：マップ3のワーク
typedef struct{
	fx32 bg_scr_x;
	fx32 bg_scr_y;
	u16 palanm_buf[ENDING_SCENE3_PALANM_NUM][16];
	int anm_pos;
	int evy;							///<EVY値(下位8ビット小数)
	int evy_dir;						///<EVYの加減算の方向
	int anm_seq;
	u32 anm_frame;

	u8 star_move;			///<TRUE:スター動作中
	u8 star_ud;				///<0:上画面のスター動作、　1:下画面のスター動作
	s8 ev1;					///<半透明係数
	s8 ev2;					///<半透明係数
	
	s32 mdl_sort_tblno;		///<表示モデル参照先
	
	JIBA_WORK jiba_work[ACT_INDEX_SCENE3_JIBA_0 - ACT_INDEX_SCENE3_JIBA_0 + 1];
}SCENE_WORK_MAP3;

///シーン：キャプチャ1のワーク
typedef struct{
	u32 change_frame;			///<キャプチャ画像を切り替えるタイミングをカウント
}SCENE_WORK_CAP1;

///シーン：フィールド1のワーク
typedef struct{
	u32 change_frame;			///<キャプチャ画像を切り替えるタイミングをカウント
	u8 change_num;				///<キャプチャ画像を切り替えた回数をカウント
	s8 ev1;
	s8 ev2;
	u32 plane1;
}SCENE_WORK_FIELD1;

///シーン：コピーライトのワーク
typedef struct{
	u32 work;
}SCENE_WORK_COPYRIGHT;

///各シーンのメインで自由に使えるワーク
typedef struct{
	int seq;
	s16 wait;
	s16 work;
	union{
		SCENE_WORK_MAP1 sw_map1;
		SCENE_WORK_MAP2 sw_map2;
		SCENE_WORK_MAP3 sw_map3;
		SCENE_WORK_CAP1 sw_cap1;
		SCENE_WORK_FIELD1 sw_field1;
		SCENE_WORK_COPYRIGHT sw_cp;
	};
}ENDING_SCENE_WORK;

///主人公アニメ用ワーク
typedef struct{
	s16 mabataki_frame;
	s16 mabataki_anmno;
	s16 run_anm_count;		///<走りアニメをしている回数
	s16 mabataki_count;
}ENDING_HERO_ANM_WORK;


//--------------------------------------------------------------
/**
 *	エンディング制御構造体
 */
//--------------------------------------------------------------
typedef struct{
	ENDING_PARENT_PARAM *parent_work;
	BOOL skipFlag;			///<TRUE：スキップ実行中

	GF_BGL_INI		*bgl;				///<BGシステムへのポインタ
	PALETTE_FADE_PTR pfd;				///<パレットシステム
	GF_G3DMAN *g3Dman;
	TCB_PTR update_tcb;					///<Update用TCBへのポインタ
	CATS_SYS_PTR		csp;
	CATS_RES_PTR		crp;
	
	// 描画まわりのワーク（主にBMP用の文字列周り）
	MSGDATA_MANAGER *msgman;						// 名前入力メッセージデータマネージャー

	// BMPWIN描画周り
	GF_BGL_BMPWIN		*win_fin;		///「おわり」のBMPWIN

	GF_CAMERA_PTR camera;				///<カメラへのポインタ
	
	//3Dモデル
	D3DOBJ_MDL mdl[ENDING_SCENE_MDL_MAX];		///<各シーンで使用するモデルの最大数
	D3DOBJ     obj[MODEL_LINE_MAX][ENDING_SCENE_LINE_3DOBJ_MAX];///<各シーンで使用する3DOBJ最大数
	MDL_ARRANGE_DATA mdl_arrange;				///<配置データ
	
	ENDING_SCENE_WORK scene_work;		///<シーンワーク
	CATS_ACT_PTR cap[ACT_INDEX_MAX];	///<アクターポインタ
	u8 *hero_eye_charbuf;				///<主人公の目キャラクタバッファ
	
	ARCHANDLE *ending_hdl;				///<グラフィックファイルへのハンドル
	u32 main_counter;					///<エンディング開始してからのフレーム数をカウント
	u8 skip_seq;						///<スキップ動作時のシーケンス
	u8 scene_no;						///<現在のシーン番号
	u8 scene_init_ok;					///<TRUE:シーンの初期化処理を通っている
	u8 draw_3d;							///<TRUE:3D描画OK
	u8 mdl_line_num;					///<3Dモデルの使用ライン数
	u8 cap_no;							///<キャプチャ画像を何番目まで表示したか
	
	PL_ENDING_LIST_WORK *list_work;		///<スタッフロール制御
	PL_ENDING_LIST_WORK *list_work_sub;	///<サブ画面用スタッフロール制御
	BOOL list_end_flag;					///<スタッフロール終了フラグ

	ENDING_HERO_ANM_WORK hero_anm_work;	///<主人公アニメ用ワーク
	
#ifdef PM_DEBUG
	int debug_mode;
#endif
}ENDING_MAIN_WORK;



#endif	//__ENDING_COMMON_H__

//==============================================================================
/**
 * @file	balloon_types.h
 * @brief	風船ミニゲームの汎用定義
 * @author	matsuda
 * @date	2007.11.27(火)
 */
//==============================================================================
#ifndef __BALLOON_GAME_TYPES_H__
#define __BALLOON_GAME_TYPES_H__


//==============================================================================
//	定数定義
//==============================================================================
enum{
	DIR_DOWN,
	DIR_LEFT,
	DIR_UP,
	DIR_RIGHT,
};

///同時に出せる空気アクター最大数(全プレイヤーの合計)
#define PLAYER_AIR_PARAM_MAX			(40)

///同時に出せる風船アイコンアクター最大数
#define ICON_BALLOON_PARAM_MAX			(5)

///破裂エフェクトを表示している時間
#define EXPLODED_ANM_TIME				(30)

///カウンターの桁数
enum{
	BALLOON_COUNTER_0,
	BALLOON_COUNTER_1,
	BALLOON_COUNTER_MAX,
};
///カウンターの桁数
#define BALLOON_COUNTER_KETA_MAX		(6)


///半透明第1対象面
#define BLD_PLANE_1		(GX_BLEND_PLANEMASK_BG3)
///半透明第2対象面
#define BLD_PLANE_2		(GX_BLEND_BGALL | GX_BLEND_PLANEMASK_OBJ)
///第1対象面に対するαブレンディング係数
#define BLD_ALPHA_1		13//(0x1b)	//(9)
///第2対象面に対するαブレンディング係数
#define BLD_ALPHA_2		(3)

//--------------------------------------------------------------
//	BMPウィンドウ
//--------------------------------------------------------------
enum{
	BALLOON_BMPWIN_TALK,		///<会話ウィンドウ

	BALLOON_BMPWIN_NAME_1,		///<プレイヤー名
	BALLOON_BMPWIN_NAME_2,		///<プレイヤー名
	BALLOON_BMPWIN_NAME_3,		///<プレイヤー名
	
	BALLOON_BMPWIN_SUB_TALK,	///<サブ画面用会話ウィンドウ

	BALLOON_BMPWIN_MAX,
};

//--------------------------------------------------------------
//	アクターソフトプライオリティ
//--------------------------------------------------------------
enum{
	BALLOON_SOFTPRI_START = 10,
	
	BALLOON_SOFTPRI_TOUCH_PEN,
	
	BALLOON_SOFTPRI_COUNTER,
	BALLOON_SOFTPRI_COUNTER_WIN,
	
	BALLOON_SOFTPRI_AIM,
};

//--------------------------------------------------------------
//	サブ画面のアクターソフトプライオリティ
//--------------------------------------------------------------
enum{
	BALLOONSUB_SOFTPRI_START = 10,
	
	BALLOONSUB_SOFTPRI_ICON_BALLOON,
	BALLOONSUB_SOFTPRI_EXPLODED_STORM,
	BALLOONSUB_SOFTPRI_EXPLODED_CHIP,
	BALLOONSUB_SOFTPRI_EXPLODED_SMOKE,

	BALLOONSUB_SOFTPRI_BOOSTER_HIT,
	BALLOONSUB_SOFTPRI_BOOSTER,
	BALLOONSUB_SOFTPRI_BOOSTER_LAND_SMOKE,
	BALLOONSUB_SOFTPRI_BOOSTER_SHADOW,
	BALLOONSUB_SOFTPRI_JOINT,
	BALLOONSUB_SOFTPRI_AIR,
};

//--------------------------------------------------------------
//	BGプライオリティ
//--------------------------------------------------------------
///タッチペンアクターのBGプライオリティ
#define BALLOON_BGPRI_TOUCH_PEN			(0)

///カウンターアクターのBGプライオリティ
#define BALLOON_BGPRI_COUNTER			(3)
///カウンターアクターWindowOFF時のダミーアクターBGプライオリティ
#define BALLOON_BGPRI_DUMMY_COUNTER		(1)

///照準アクターのBGプライオリティ
#define BALLOON_BGPRI_AIM				(1)

//--------------------------------------------------------------
//	サブ画面のBGプライオリティ
//--------------------------------------------------------------
///空気アクターのBGプライオリティ
#define BALLOONSUB_BGPRI_AIR			(3)
///風船破裂アクターのBGプライオリティ
#define BALLOONSUB_BGPRI_EXPLODED		(1)
///風船アイコンアクターのBGプライオリティ
#define BALLOONSUB_BGPRI_ICON_BALLOON	(1)
///ジョイントアクターのBGプライオリティ
#define BALLOONSUB_BGPRI_JOINT			(3)
///ブースターアクターのBGプライオリティ
#define BALLOONSUB_BGPRI_BOOSTER		(3)


//--------------------------------------------------------------
//	
//--------------------------------------------------------------
///同時に出せる空気の最大数
#define PIPE_AIR_AIR_MAX		(8)

///空気モデルのタイプ
enum{
	PIPE_3D_AIR_TYPE_SMALL,
	PIPE_3D_AIR_TYPE_MIDDLE,
	PIPE_3D_AIR_TYPE_BIG,
	PIPE_3D_AIR_TYPE_SPECIAL,	///<ソーナンスがでかくなったときに押し込んだとき
	
	PIPE_3D_AIR_TYPE_MAX,
};

//--------------------------------------------------------------
//	風船アイコン
//--------------------------------------------------------------
///風船アイコンの状態
enum{
	ICON_BALLOON_STATUS_NULL,			///<生成されていない
	ICON_BALLOON_STATUS_NORMAL,			///<通常状態
	ICON_BALLOON_STATUS_PINCH,			///<風船が破裂しそうな状態
	ICON_BALLOON_STATUS_EXPLODED,		///<爆発中
};

//--------------------------------------------------------------
//	カウントダウン
//--------------------------------------------------------------
enum{
	BALLOON_COUNTDOWN_NULL,				///<初期状態
	BALLOON_COUNTDOWN_TOUCHPEN_DEMO_INIT,	///<タッチペンデモ
	BALLOON_COUNTDOWN_TOUCHPEN_DEMO,	///<タッチペンデモ
	BALLOON_COUNTDOWN_START,			///<カウントダウン開始
	BALLOON_COUNTDOWN_START_WAIT,		///<終了待ち状態
	BALLOON_COUNTDOWN_START_END,		///<開始カウントダウン終了
	BALLOON_COUNTDOWN_TIMEUP,			///<タイムアップ開始
	BALLOON_COUNTDOWN_TIMEUP_WAIT,		///<タイムアップ待ち状態
	BALLOON_COUNTDOWN_TIMEUP_END,		///<タイムアップ終了
};

//--------------------------------------------------------------
//	ジョイント番号
//--------------------------------------------------------------
///ジョイントのアクター番号
enum{
	JOINT_ACTOR_D,		//下
	JOINT_ACTOR_U,		//上
	JOINT_ACTOR_L,		//左
	JOINT_ACTOR_R,		//右
	
	JOINT_ACTOR_MAX,
};

//--------------------------------------------------------------
//	ブースター番号
//--------------------------------------------------------------
enum{
	BOOSTER_ACTOR_NORMAL,	//通常
	BOOSTER_ACTOR_WEAK,		//半減ブースター
	BOOSTER_ACTOR_HARD,		//3倍ブースター
	
	BOOSTER_ACTOR_MAX,
};

///同時に出せるSIOブースターの数
#define SIO_BOOSTER_ACTOR_MAX		(3)

//--------------------------------------------------------------
//	ブースタータイプ
//--------------------------------------------------------------
enum{
	BOOSTER_TYPE_NONE,		//なし
	BOOSTER_TYPE_NORMAL,	//通常(2倍)
	BOOSTER_TYPE_WEAK,		//半減ブースター
	BOOSTER_TYPE_HARD,		//3倍ブースター
};

//--------------------------------------------------------------
//	ブースター着地の煙
//--------------------------------------------------------------
///ブースター着地で出る煙の数
#define BOOSTER_LAND_SMOKE_NUM				(2)
///ブースター着地で出る煙の寿命
#define BOOSTER_LAND_SMOKE_DELETE_TIME		(8)
///ブースター着地煙のアニメ種類
#define BOOSTER_LAND_SMOKE_ANM_PATERN		(2)

//--------------------------------------------------------------
//	爆発
//--------------------------------------------------------------
///爆発の紙ふぶきで使用するアクター数
#define EXPLODED_STORM_ACTOR_MAX		(36)	//(40)
///爆発の煙で使用するアクター数
#define EXPLODED_SMOKE_ACTOR_MAX		(8)//(12)
///爆発の風船の破片で使用するアクター数
#define EXPLODED_CHIP_ACTOR_MAX			(3)//(4)

//--------------------------------------------------------------
//	カウンター
//--------------------------------------------------------------
///カウンターのフォントカラー設定
#define COUNTER_FONT_COLOR		(GF_PRINTCOLOR_MAKE(0xe,0xf,0))
///カウンターX座標基準値
#define COUNTER_BASE_X			(8)
///カウンターX座標の配置間隔
#define COUNTER_X_SPACE			(8)
///カウンターY座標
#define COUNTER_Y				(168)

//==============================================================================
//	型定義
//==============================================================================
///ソーナンス動作制御システムワークの不定形ポインタ
typedef struct _SONANS_SYS * SONANS_SYS_PTR;


//==============================================================================
//	構造体定義
//==============================================================================
///空気アクターの位置毎のデータ
typedef struct{
	s16 x;				///<空気出現座標X
	s16 y;				///<空気出現座標Y
	u16 anmseq;			///<アニメシーケンス番号
	u8 pal_ofs;			///<パレットオフセット
	u8 move_dir;		///<移動方向
	
	u8 dummy[2];
}AIR_POSITION_DATA;

///プレイヤーそれぞれが発生させた空気パラメータ
typedef struct{
	BOOL occ;			///<TRUE:有効　FALSE:無効
	s16 net_id;
	s16 balloon_no;
	s32 air;
	
	s16 wait;
	u8 booster_type;	///<BOOSTER_TYPE_???
	u8 dummy[1];
	
	CATS_ACT_PTR cap;
	const AIR_POSITION_DATA *apd;
}PLAYER_AIR_PARAM; 

///紙ふぶきワーク
typedef struct{
	CATS_ACT_PTR cap;
	fx32 furihaba;
	fx32 add_furihaba;
	fx32 sec;
	fx32 add_sec;
	s16 delete_wait;
}STORM_WORK;

///破裂アクターパラメータ
typedef struct{
	BOOL occ;			///<TRUE:爆発アニメ中
	STORM_WORK storm[EXPLODED_STORM_ACTOR_MAX];
	STORM_WORK smoke[EXPLODED_SMOKE_ACTOR_MAX];
	STORM_WORK chip[EXPLODED_CHIP_ACTOR_MAX];
	
	u8 dummy[2];
}EXPLODED_PARAM; 

///風船アイコンアクターパラメータ
typedef struct{
	CATS_ACT_PTR cap;
	u8 type;			///<アイコンの種類(BALLOON_LEVEL_???)
	u8 status;			///<現在の状態(ICON_BALLOON_STATUS_???)
	u8 pos;				///<表示位置(一番右端からのオフセット番号)
	u8 dummy;
	
	s16 balloon_no;		///<担当する風船番号
	s16 dummy_2;
}ICONBALLOON_PARAM;

///ジョイント動作制御
typedef struct{
	CATS_ACT_PTR cap[JOINT_ACTOR_MAX];
}JOINT_WORK;

///ブースター着地時の煙アクター動作制御構造体
typedef struct{
	CATS_ACT_PTR cap[BOOSTER_LAND_SMOKE_NUM];
	fx32 add_x[BOOSTER_LAND_SMOKE_NUM];
	fx32 add_y[BOOSTER_LAND_SMOKE_NUM];
	u8 seq;
	u8 time;
}BOOSTER_LAND_SMOKE;

///ブースターの動作ワーク
typedef struct{
	CATS_ACT_PTR cap;
	CATS_ACT_PTR hit_cap;		///<ヒットした時に出すエフェクトアクター
	CATS_ACT_PTR shadow_cap;	///<影アクター
	fx32 start_theta;		///<スタート角度
	u8 mode;				///<BOOSTER_MODE_???
	u8 seq;
	u8 local_frame;
	u8 local_wait;
	u8 booster_type;		///<ブースタータイプ
	u8 hit_ok;				///<TRUE:空気との当たり判定発生可能
	s32 local_work;
	s16 local_x;
	s16 local_y;
	fx32 local_add_x;
	fx32 local_add_y;
	fx32 local_fx_x;
	fx32 local_fx_y;
	
	BOOSTER_LAND_SMOKE land_smoke;	///<ブースター着地時の煙アクター動作制御ワーク
}BOOSTER_MOVE;

///ブースター動作制御構造体
typedef struct{
	BOOSTER_MOVE move[BOOSTER_ACTOR_MAX];			///<ブースター動作ワーク
	fx32 theta;				///<現在の秒針角度
	fx32 add_theta;			///<秒針に加算する角度
	u32 world_frame;		///<ブースターが動作している最中、常にカウンタアップ
	u8 byousin_pos;			///<秒針が現在どこをさしているか
	u8 stop;				///<TRUE:ブースター全ての動作を停止する
	u8 wait;				///<ウェイト
	u8 frame;				///<秒針間の移動フレーム
	u8 lap;					///<現在の周回
}BOOSTER_WORK;

///SIOブースターの動作ワーク
typedef struct{
	CATS_ACT_PTR cap;
	CATS_ACT_PTR hit_cap;	///<ヒットした時に出すエフェクトアクター
	fx32 local_fx_x;
	fx32 local_fx_y;
	s16 end_y;
	u8 booster_type;
	u8 seq;
	u8 local_frame;
	u8 occ;
	s16 start_wait;
}SIO_BOOSTER_MOVE;

///SIOブースター動作制御構造体
typedef struct{
	SIO_BOOSTER_MOVE move[SIO_BOOSTER_ACTOR_MAX];			///<ブースター動作ワーク
}SIO_BOOSTER_WORK;

///フォントOAM用の構造体定義
typedef struct{
	FONTOAM_OBJ_PTR fontoam;		///<フォントOAM
	CHAR_MANAGER_ALLOCDATA cma;		///<フォントOAMキャラ領域データ
	u16 len;						///<フォントのドット長
}BALLOON_FONTACT;

///カウンター制御構造体定義
typedef struct{
	CATS_ACT_PTR win_cap;			///<ウィンドウアクターのポインタ
	BALLOON_FONTACT fontact[BALLOON_COUNTER_KETA_MAX][BALLOON_COUNTER_MAX];	///<フォントアクター(カウンター)
	BALLOON_FONTACT fontact_cc;		///<フォントアクター(CC)
	BALLOON_FONTACT fontact_dummy[BALLOON_COUNTER_KETA_MAX];	///<フォントアクター(カウンターダミー)
	
	u8 number[BALLOON_COUNTER_KETA_MAX];		///<スクロールドット単位(現在値)
	u8 last_number[BALLOON_COUNTER_KETA_MAX];	///<スクロールドット単位(停止値)
	u8 next_number[BALLOON_COUNTER_KETA_MAX];	///<スクロールドット単位(リクエスト値)
	u8 rotate_count[BALLOON_COUNTER_KETA_MAX];	///<何周したかカウント
	s8 move_wait[BALLOON_COUNTER_KETA_MAX];		///<動作開始前のウェイト
	s8 wait;
	u8 move_keta;								///<何桁まで回転動作しているか
	u8 seq;
}BALLOON_COUNTER;

///タッチペン制御構造体定義
typedef struct{
	CATS_ACT_PTR cap;				///<タッチペンアクターのポインタ
	s16 seq;
	s16 wait;
	s16 x;
	s16 y;
	int tp_x;						///<疑似タッチペン座標
	int tp_y;
	int tp_cont;
	int tp_trg;
	BOOL demo_end;					///<TRUE:デモ終了
}BALLOON_PEN;

//--------------------------------------------------------------
/**
 * @brief   ソーナンスに繋がっているパイプと空気の動作制御構造体
 */
//--------------------------------------------------------------
typedef struct{
	D3DOBJ_MDL	pipe_mdl;		///<パイプモデル
   	D3DOBJ		pipe_obj;		///<パイプOBJ
   	
	D3DOBJ_MDL	air_mdl[PIPE_3D_AIR_TYPE_MAX];	///<空気モデル
	struct{
		int occ;				///<TRUE:データが生成されている
		D3DOBJ obj;				///<空気OBJ
		D3DOBJ_ANM	anm;		///<空気アニメ
	}air[PIPE_AIR_AIR_MAX][PIPE_3D_AIR_TYPE_MAX];
}PIPE_AIR_WORK;

//--------------------------------------------------------------
/**
 * @brief   ソーナンスに繋がっている台座の動作制御構造体
 */
//--------------------------------------------------------------
typedef struct{
	D3DOBJ_MDL	mdl;		///<台座モデル
   	D3DOBJ		obj;		///<台座OBJ
}DAIZA_WORK;

//--------------------------------------------------------------
/**
 * @brief   サーバー用のシステムワーク
 */
//--------------------------------------------------------------
typedef struct{
	int seq;
	int work;
	int wait;

	int local_seq;
	int local_work;
	int local_wait;
	
	u8 req_timing_no;		///<同期取りように全員に発行した同期番号
	u8 dummy[3];
}BALLOON_SERVER_WORK;

//--------------------------------------------------------------
/**
 * @brief   風船割りゲーム制御メイン構造体
 */
//--------------------------------------------------------------
typedef struct _BALLOON_GAME_WORK{
	BALLOON_SYSTEM_WORK *bsw;		///<風船割りシステムワークへのポインタ
	int server_netid;				///<サーバー担当のネットID
	u32 recv_server_version_cnt;	///<サーバーバージョンの受信件数
	u32 recv_server_version[BALLOON_PLAYER_MAX];	///受信したサーバーバージョン
	
	MNGM_COUNTWK *mgcount;			///<ミニゲーム共通カウントダウンエフェクトシステムへのポインタ
	int countdown_eff;				///<カウントダウンエフェクトの現在の状態
	
	CATS_SYS_PTR		csp;
	CATS_RES_PTR		crp;
	GF_BGL_INI *bgl;
	GF_BGL_BMPWIN win[BALLOON_BMPWIN_MAX];
	MSGDATA_MANAGER *msgman;		///<メッセージデータマネージャのポインタ
	WORDSET *wordset;				///<Allocしたメッセージ用単語バッファへのポインタ
	STRBUF *msg_buf;				///<Allocした文字列バッファへのポインタ
	PALETTE_FADE_PTR pfd;			///<パレットフェードシステムへのポインタ
	FONTOAM_SYS_PTR fontoam_sys;	///<FONTOAMシステム

	TCB_PTR update_tcb;
	GF_G3DMAN *g3Dman;

	GF_CAMERA_PTR camera;			///<カメラへのポインタ
	GF_CAMERA_PTR camera_3d;		///<3Dモデル用カメラへのポインタ
	
	PTC_PTR ptc;					///<パーティクルシステムへのポインタ

    // アロケータ
    NNSFndAllocator allocator;
	
	BALLOON_SERVER_WORK server_work;	///<サーバー用のシステムワーク
	
	SONANS_SYS_PTR sns;				///<ソーナンス動作制御システムワークへのポインタ
	
	PIPE_AIR_WORK pipe_air;			///<ソーナンスに繋がるパイプと空気3D制御構造体
	DAIZA_WORK daiza;				///<台座制御構造体
	
	BALLOON_STATUS bst;				///<風船ステータス

	JOINT_WORK joint;				///<ジョイント制御ワーク
	BOOSTER_WORK booster;				///<ブースター動作制御ワーク
	SIO_BOOSTER_WORK sio_booster;		///<SIOブースター動作制御ワーク
	
	BALLOON_COUNTER counter;		///<カウンター動作制御ワーク
	BALLOON_PEN pen;				///<タッチペン動作制御ワーク
	
	BALLOON_SIO_BUFFER send_buf;	///<送信バッファ
	BALLOON_SIO_BUFFER recv_buf[BALLOON_PLAYER_MAX];	///<受信バッファ
	BALLOON_SIO_PLAY_WORK send_important;		///<重要データ専用送信バッファ(親のみが使用)
	BALLOON_SIO_PLAY_WORK recv_important;		///<重要データ専用受信バッファ
	int recv_write_ring_no[BALLOON_PLAYER_MAX];	///<受信用リングバッファの書き込み位置
	int recv_read_ring_no[BALLOON_PLAYER_MAX];	///<受信用リングバッファの読み込み位置
	int send_write_ring_no;						///<送信用リングバッファの書き込み位置
	int send_read_ring_no;						///<送信用リングバッファの読み込み位置
	BALLOON_AIR_DATA send_airdata_stack[PIPE_AIR_AIR_MAX];	///<空気データスタック
	int send_airdata_read_no;					///<空気データスタック参照番号(読み出し)
	int send_airdata_write_no;					///<空気データスタック参照番号(書き込み)
	
	u32 time;					///<ゲームプレイタイム
	int game_start;				///<TRUE:ゲーム開始
	int game_finish;			///<TRUE:ゲーム終了(終了演出へ移行)
	int game_end;				///<TRUE:ゲーム終了(画面暗転へ移行)
	int sio_wait;				///<Wifi負荷対策用に一度送信すると一定期間送信出来ないようにする
	int balloon_no;				///<現在の風船番号
	int balloon_occ;			///<TRUE：風船が生成されている
	int exploded_count;			///<風船を破裂させた回数
	
	int timing_req;				///<同期取りリクエスト番号
	int timing_no;				///<同期番号
	
	PLAYER_AIR_PARAM air_param[PLAYER_AIR_PARAM_MAX];	///<空気アクターパラメータ
	EXPLODED_PARAM exploded_param;		///<破裂アクターパラメータ
	ICONBALLOON_PARAM iconballoon_param[ICON_BALLOON_PARAM_MAX];	///<風船アイコンパラメータ
	
	s32 my_total_air;			///<自分が入れた空気量の合計値
	u32 main_frame;				///<ゲーム開始からのフレーム数
}BALLOON_GAME_WORK;


#endif	//__BALLOON_GAME_TYPES_H__

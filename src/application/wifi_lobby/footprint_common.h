//==============================================================================
/**
 * @file	footprint_common.h
 * @brief	足跡ボード共通ヘッダ
 * @author	matsuda
 * @date	2008.01.18(金)
 */
//==============================================================================
#ifndef __FOOTPRINT_COMMON_H__
#define __FOOTPRINT_COMMON_H__


//==============================================================================
//	定数定義
//==============================================================================

///現在のゲームの状態
enum{
	FOOTPRINT_GAME_STATUS_INIT,			///<初期化処理中
	FOOTPRINT_GAME_STATUS_NORMAL,			///<通常
	FOOTPRINT_GAME_STATUS_EXIT_SELECT,	///<終了選択
	FOOTPRINT_GAME_STATUS_FINISH,			///<終了処理中
};

//--------------------------------------------------------------
//	BGフレーム
//--------------------------------------------------------------
///フレーム指定
enum{
	//メイン画面
	FOOT_FRAME_WIN = GF_BGL_FRAME1_M,			//ウィンドウ面
	FOOT_FRAME_PANEL = GF_BGL_FRAME2_M,			//パネル
	FOOT_FRAME_BG = GF_BGL_FRAME3_M,			//背景
	
	//サブ画面
	FOOT_SUBFRAME_WIN = GF_BGL_FRAME1_S,		//ウィンドウ名
	FOOT_SUBFRAME_PLATE = GF_BGL_FRAME2_S,			//プレート
	FOOT_SUBFRAME_BG = GF_BGL_FRAME3_S,			//背景
};

//--------------------------------------------------------------
//	BGプライオリティ
//--------------------------------------------------------------
enum{
	FOOT_BGPRI_3D = 2,		//3D面BGプライオリティ
	
	FOOT_BGPRI_WIN = 0,		//パネル面BGプライオリティ
	FOOT_BGPRI_PANEL = 0,		//パネル面BGプライオリティ
	FOOT_BGPRI_BG = 3,			//背景面BGプライオリティ

	FOOT_SUBBGPRI_WIN = 2,		//パネル面BGプライオリティ
	FOOT_SUBBGPRI_PLATE = 3,	//プレート面BGプライオリティ
	FOOT_SUBBGPRI_BG = 3,		//背景面BGプライオリティ
};

//--------------------------------------------------------------
//	アクターBGプライオリティ
//--------------------------------------------------------------
enum{
	ACTBGPRI_INK = 1,				///<インク
	ACTBGPRI_INK_FOOT = 1,			///<インクの上に配置する足跡
	ACTBGPRI_INK_FOUNDATION = 1,	///<インクの下地
	
	ACTBGPRI_TOUCH_EFF = 0,			///<インクをタッチした時に出すエフェクト
	ACTBGPRI_EXIT_FONT = 0,			///<「やめる」
	
	//-- サブ画面 --//
	ACTBGPRI_SUB_NAME_FRAME = 1,	///<名前を囲む枠
	ACTBGPRI_SUB_NAME_FOOT = 1,		///<名前の横に配置する足跡
};

//--------------------------------------------------------------
//	アクターソフトプライオリティ
//--------------------------------------------------------------
enum{
	SOFTPRI_TOUCH_EFF = 50,
	SOFTPRI_EXIT_FONT,
	SOFTPRI_INK_FOOT,
	SOFTPRI_INK,
	SOFTPRI_INK_FOUNDATION,
	
	//-- サブ画面 --//
	SOFTPRI_SUB_NAME_FRAME = 50,
	SOFTPRI_SUB_NAME_FOOT,
};

//--------------------------------------------------------------
//	パレット展開位置
//--------------------------------------------------------------
///ボタンの「はい・いいえ」パレット展開位置(メイン画面)
#define FOOT_MAINBG_BUTTON_YESNO_PAL	(11)	//2本使用
///会話フォントパレット展開位置(メイン画面)
#define FOOT_MAINBG_TALKFONT_PAL	 (13)
///会話ウィンドウパレット展開位置(メイン画面)
#define FOOT_MAINBG_TALKWIN_PAL		(14)

///会話フォントパレット展開位置(サブ画面)
#define FOOT_SUBBG_TALKFONT_PAL		 (13)

//--------------------------------------------------------------
//	メッセージCGX展開位置
//--------------------------------------------------------------
///会話ウィンドウのキャラクタ展開開始位置
#define WINCGX_TALKWIN_START	(1)
///ボタンの「はい・いいえ」展開開始位置
#define WINCGX_BUTTON_YESNO_START	(WINCGX_TALKWIN_START + TALK_WIN_CGX_SIZ)
///会話ウィンドウ内のメッセージ展開開始位置
#define WINCGX_MESSAGE_START	(WINCGX_BUTTON_YESNO_START + TOUCH_SW_USE_CHAR_NUM)

//--------------------------------------------------------------
//	サブ画面：メッセージCGX展開位置
//--------------------------------------------------------------
///名前表示メッセージのキャラクタ展開開始位置
#define WINCGX_SUB_NAME_START		(0x4000 / 0x20)
///名前表示BMPWINのサイズX(キャラ単位)
#define WINCGX_SUB_NAME_SIZE_X		(8)
///名前表示BMPWINのサイズY(キャラ単位)
#define WINCGX_SUB_NAME_SIZE_Y		(2)
///名前表示のBMPWIN数
#define FOOTPRINT_BMPWIN_NAME_MAX		(8)

//--------------------------------------------------------------
//	アクター
//--------------------------------------------------------------
///サブサーフェースY(fx32)
#define FOOTPRINT_SUB_ACTOR_DISTANCE		((192 + 80) << FX32_SHIFT)
///サブサーフェースY(int)
#define FOOTPRINT_SUB_ACTOR_DISTANCE_INTEGER		(FOOTPRINT_SUB_ACTOR_DISTANCE >> FX32_SHIFT)

//--------------------------------------------------------------
//	システム
//--------------------------------------------------------------
///足跡ボードに参加できる最大人数
#define FOOTPRINT_ENTRY_MAX		(8)

///スタンプの出せる最大数
#define STAMP_MAX		(128)

///スタンプテクスチャのVWait転送リクエストバッファの最大数
#define LOAD_TEX_BUFFER_MAX		(32)


//==============================================================================
//	型定義
//==============================================================================
///FOOTPRINT_SYSの不定形ポインタ
typedef struct _FOOTPRINT_SYS * FOOTPRINT_SYS_PTR;

///STAMP_MOVE_WORKの不定形ポインタ
typedef struct _STAMP_MOVE_WORK * STAMP_MOVE_PTR;


//==============================================================================
//	構造体定義
//==============================================================================
//--------------------------------------------------------------
//	マイデータ
//--------------------------------------------------------------
///自分の通信ステータス
typedef struct{
	BOOL ready;							///<TRUE:通信受信受付準備が出来ている状態
	s32 user_id;						///<ユーザーID
}FOOTPRINT_MY_COMM_STATUS;

//--------------------------------------------------------------
//	足跡スタンプ
//--------------------------------------------------------------
///足跡スタンプのパラメータ(これが通信で送受信される)
typedef struct{
	u16 color;				///<スタンプのカラーコード
	u16 monsno;				///<ポケモン番号
	u32 personal_rnd;		///<個性乱数(性別、性格)
	u8 form_no;				///<フォルム番号
	u8 x;					///<足跡座標X(3D座標ではなくタッチパネル上の座標)
	u8 y;					///<足跡座標Y(3D座標ではなくタッチパネル上の座標)
	
	u8 padding;
}STAMP_PARAM;

//--------------------------------------------------------------
//	スタンプの連鎖回数を管理する構造体
//--------------------------------------------------------------
///スタンプの連鎖回数を管理する構造体
typedef struct{
	u8 chain;			///<チェイン回数
	u8 stamp_num;		///<いくつのスタンプがこのワークを参照しているか
	
	u16 padding;
}STAMP_CHAIN_WORK;

//--------------------------------------------------------------
//	スペシャルエフェクト動作構造体
//--------------------------------------------------------------
///スペシャルエフェクト：にじみ
typedef struct{
	fx32 default_len;
	s16 wait;
	u8 seq;			///<シーケンス番号
	
	u8 padding;
}SPECIAL_EFF_NIJIMI;

///スペシャルエフェクト：はじけ
typedef struct{
	VecFx32 up_vec;		///<カメラの上方向
	u8 seq;
	u8 count;
	u8 loop;
	
	u8 padding;
}SPECIAL_EFF_HAJIKE;

///スペシャルエフェクト：ジグザグ
typedef struct{
	CAMERA_ANGLE default_angle;
	s32 calc_angle_y;	///<現在地の計算に便利なようにマイナスも扱えるアングルを用意しておく
	s16 wait;
	u8 seq;
	
	u8 padding;
}SPECIAL_EFF_ZIGZAG;

///スペシャルエフェクト：蛇行
typedef struct{
	CAMERA_ANGLE default_angle;
	s32 calc_angle_y;	///<現在地の計算に便利なようにマイナスも扱えるアングルを用意しておく
	s16 wait;
	u8 seq;
	
	u8 padding;
}SPECIAL_EFF_DAKOU;

///スペシャルエフェクト：軌跡
typedef struct{
	CAMERA_ANGLE default_angle;
	s32 calc_angle_x;	///<現在地の計算に便利なようにマイナスも扱えるアングルを用意しておく
	s16 wait;
	u8 seq;
	
	u8 padding;
}SPECIAL_EFF_KISEKI;

///スペシャルエフェクト：揺れ
typedef struct{
	CAMERA_ANGLE default_angle;
	fx32 theta;
	u8 loop;
	u8 seq;
	
	u8 padding[2];
}SPECIAL_EFF_YURE;

///スペシャルエフェクト：拡大
typedef struct{
	fx32 default_len;
	s16 wait;
	u8 seq;			///<シーケンス番号
	
	u8 padding;
}SPECIAL_EFF_KAKUDAI;

///スペシャルエフェクト：ブラーX
typedef struct{
	VecFx32 default_target;
	VecFx32 default_pos;
	fx32 theta;
	u8 loop;
	u8 seq;
	
	u8 padding[2];
}SPECIAL_EFF_BRAR_X;

///スペシャルエフェクト：ブラーY
typedef struct{
	VecFx32 default_target;
	VecFx32 default_pos;
	fx32 theta;
	u8 loop;
	u8 seq;
	
	u8 padding[2];
}SPECIAL_EFF_BRAR_Y;

///スペシャルエフェクト：たれ
typedef struct{
	CAMERA_ANGLE default_angle;
	s32 calc_angle_x;	///<現在地の計算に便利なようにマイナスも扱えるアングルを用意しておく
	s16 wait;
	u8 seq;
	
	u8 padding;
}SPECIAL_EFF_TARE;

///スタンプのスペシャルエフェクト動作構造体
typedef struct{
	SPECIAL_EFF_NIJIMI sp_nijimi;
	SPECIAL_EFF_HAJIKE sp_hajike;
	SPECIAL_EFF_ZIGZAG sp_zigzag;
	SPECIAL_EFF_DAKOU sp_dakou;
	SPECIAL_EFF_KISEKI sp_kiseki;
	SPECIAL_EFF_YURE sp_yure;
	SPECIAL_EFF_KAKUDAI sp_kakudai;
	SPECIAL_EFF_BRAR_X sp_brar_x;
	SPECIAL_EFF_BRAR_Y sp_brar_y;
	SPECIAL_EFF_TARE sp_tare;
}STAMP_SPECIAL_WORK;

///スペシャルエフェクト発動前のフラッシュエフェクト動作構造体
typedef struct{
	u8 seq;			///<シーケンス番号
	u8 loop;		///<ループカウンタ
	s16 evy;		///<EVY値(下位8ビット小数)
}SPECIAL_FLASH_WORK;

//--------------------------------------------------------------
/**
 * @brief   足跡スタンプ全体を管理するシステムワーク
 */
//--------------------------------------------------------------
typedef struct{
	STAMP_SPECIAL_WORK special_work;			///<スペシャルエフェクト動作ワーク
	SPECIAL_FLASH_WORK flash_work;				///<スペシャル発動前のフラッシュ動作ワーク
	STAMP_MOVE_PTR move[STAMP_MAX];
	STAMP_MOVE_PTR hitcheck_move[STAMP_MAX];	///ヒットチェック対象のスタンプへのポインタ
	NNSG3dResTex *load_tex[LOAD_TEX_BUFFER_MAX];	///<テクスチャVwait転送バッファ
	u32 polygon_id_manage[2];		///<ポリゴンID管理ビット
	STAMP_CHAIN_WORK chain_work[STAMP_MAX];		///<スタンプの連鎖管理ワーク
	u8 special_effect_type;				///<発動中のスペシャルエフェクトタイプ
	u8 special_effect_req;				///<スペシャルエフェクトタイプの発動リクエスト
	u8 special_effect_start_effect_end;			///<TRUE:スペシャル前のフラッシュエフェクト終了
	u8 player_max;								///<現在の参加人数
	u8 v_wipe_req;								///<VブランクでのWIPE転送リクエスト
	s16 v_wipe_no;								///<VブランクでのWIPE転送時の値
}STAMP_SYSTEM_WORK;

///コンバータから出力されるデータ型
typedef struct{
	u8 disp;			///<足跡の有無。(TRUE=有り)
	u8 hit_size;		///<ヒット範囲の大きさ
	
	u16 padding;
}FOOTPRINT_CONV_DATA;

//==============================================================================
//	外部関数宣言
//==============================================================================
extern FOOTPRINT_MY_COMM_STATUS * Footprint_MyCommStatusGet(FOOTPRINT_SYS_PTR fps);
extern BOOL Footprint_StampAdd(FOOTPRINT_SYS_PTR fps, const STAMP_PARAM *param, s32 user_id);
extern void Footprint_NameWrite(FOOTPRINT_SYS_PTR fps, s32 user_id);
extern void Footprint_NameErase(FOOTPRINT_SYS_PTR fps, u32 user_index);
extern void Footprint_WorldWidthHeightGet(FOOTPRINT_SYS_PTR fps, fx32 *width, fx32 *height);


#endif	//__FOOTPRINT_COMMON_H__

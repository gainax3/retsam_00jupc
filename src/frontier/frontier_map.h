//==============================================================================
/**
 * @file	frontier_map.h
 * @brief	フロンティアマップのヘッダ
 * @author	matsuda
 * @date	2007.04.04(水)
 */
//==============================================================================
#ifndef __FRONTIER_MAP_H__
#define __FRONTIER_MAP_H__

#include "application/wifi_2dmap/wf2dmap_obj.h"
#include "application/wifi_2dmap/wf2dmap_judge.h"
#include "application/wifi_2dmap/wf2dmap_scrdraw.h"
#include "application/wifi_2dmap/wf2dmap_objdraw.h"
#include "application/wifi_2dmap/wf2dmap_cmdq.h"

#include "system/clact_tool.h"
#include "frontier_particle.h"		//FRP_PTR定義のため
#include "frontier_main.h"			//FSS_CHAR_RESOURCE_DATA定義のため

#include "fs_usescript.h"


//==============================================================================
//	定数定義
//==============================================================================
///フロンティアマップで確保するヒープサイズ
#define FRONTIERMAP_ALLOC_SIZE		(0x90000)

///フィールド上で出せるOBJ最大数
#define FIELD_OBJ_MAX				(32)
///キャラクタリソース、最大管理数
#define FIELD_OBJ_RESOURCE_MAX		(24)

///マップX方向グリッド数
#define MAP_GRID_MAX_X				(256 / 16)
///マップY方向グリッド数
#define MAP_GRID_MAX_Y				(256 / 16)

///無効なプレイID
#define FIELD_PLAYID_NULL			(0xffff)

///article_resource_idで使用していない場合の値
#define SCR_ACTWORK_RESOURCE_ID_NULL	(0xffff)

//--------------------------------------------------------------
//	OBJWKのシステム側であらかじめ確保する位置
//--------------------------------------------------------------
///プレイヤー(自機)のobjwk使用位置
#define FIELD_OBJ_PLAYER		(FIELD_OBJ_MAX - 1)
///通信相手のobjwk使用位置
#define FIELD_OBJ_SIO_PLAYER	(FIELD_OBJ_MAX - 2)

//--------------------------------------------------------------
//	フレーム番号
//--------------------------------------------------------------
///ウィンドウ・メッセージ面のフレーム
#define FRMAP_FRAME_WIN					(GF_BGL_FRAME1_M)
///エフェクト面(or 多重面)のフレーム
#define FRMAP_FRAME_EFF					(GF_BGL_FRAME2_M)
///背景面のフレーム
#define FRMAP_FRAME_MAP					(GF_BGL_FRAME3_M)

///サブ画面：観客面のフレーム
#define FRMAP_FRAME_SUB_AUDIENCE		(GF_BGL_FRAME0_S)

///BG番号：ウィンドウ
#define FRMAP_BGNO_WIN			(1)
///BG番号：エフェクト
#define FRMAP_BGNO_EFF			(2)
///BG番号：背景
#define FRMAP_BGNO_BACKGROUND	(3)

///GX_WND番号：エフェクト
#define FRMAP_GX_WND_EFF			(GX_WND_PLANEMASK_BG2)

///<3D面のBGプライオリティ
#define FRMAP_3DBG_PRIORITY		(0)
///ウィンドウ面のBGプライオリティ
#define FRMAP_BGPRI_WIN			(0)
///エフェクト面のBGプライオリティ
#define FRMAP_BGPRI_EFF			(1)
///背景面のBGプライオリティ
#define FRMAP_BGPRI_MAP			(3)

///OBJキャラクターのBGプライオリティ
#define FRMAP_BGPRI_CHARACTER	(2)

///<サブ画面：背景面のBGプライオリティ
///観客面のBGプライオリティ
#define FRMAP_BGPRI_SUB_AUDIENCE	(3)


//--------------------------------------------------------------
//	アクター
//--------------------------------------------------------------
///サブ画面のサーフェース位置
#define FMAP_SUB_ACTOR_DISTANCE		(512 * FX32_ONE)

///メイン	OAM管理領域・開始
#define FMAP_OAM_START_MAIN			(0)
///メイン	OAM管理領域・終了
#define FMAP_OAM_END_MAIN				(128)
///メイン	アフィン管理領域・開始
#define FMAP_OAM_AFFINE_START_MAIN		(0)
///メイン	アフィン管理領域・終了
#define FMAP_OAM_AFFINE_END_MAIN		(32)
///サブ	OAM管理領域・開始
#define FMAP_OAM_START_SUB				(0)
///サブ	OAM管理領域・終了
#define FMAP_OAM_END_SUB				(128)
///サブ アフィン管理領域・開始
#define FMAP_OAM_AFFINE_START_SUB		(0)
///サブ	アフィン管理領域・終了
#define FMAP_OAM_AFFINE_END_SUB		(32)

///キャラマネージャ：キャラクタID管理数(上画面＋下画面)
#define FMAP_CHAR_MAX					(96)
///キャラマネージャ：メイン画面サイズ(byte単位)
#define FMAP_CHAR_VRAMSIZE_MAIN			(0x10000)	//(1024 * 0x40)	//64K
///キャラマネージャ：サブ画面サイズ(byte単位)
#define FMAP_CHAR_VRAMSIZE_SUB			(512 * 0x20)	//32K

///メイン画面＋サブ画面で使用するアクター総数
#define FMAP_ACTOR_MAX					(64 + 64)	//メイン画面 + サブ画面
///OBJで使用するパレット本数(上画面＋下画面)
#define FMAP_OAM_PLTT_MAX				(16 + 16)

///OAMリソース：キャラ登録最大数(メイン画面 + サブ画面)
#define FMAP_OAMRESOURCE_CHAR_MAX		(FMAP_CHAR_MAX)
///OAMリソース：パレット登録最大数(メイン画面 + サブ画面)
#define FMAP_OAMRESOURCE_PLTT_MAX		(FMAP_OAM_PLTT_MAX)
///OAMリソース：セル登録最大数
#define FMAP_OAMRESOURCE_CELL_MAX		(64)
///OAMリソース：セルアニメ登録最大数
#define FMAP_OAMRESOURCE_CELLANM_MAX	(64)
///OAMリソース：マルチセル登録最大数
#define FMAP_OAMRESOURCE_MCELL_MAX		(8)
///OAMリソース：マルチセルアニメ登録最大数
#define FMAP_OAMRESOURCE_MCELLANM_MAX	(8)


//--------------------------------------------------------------
//	
//--------------------------------------------------------------
///FMap_ActorSet関数の引数：空き領域を検索して登録
#define FMAP_ACT_POS_AUTO		(-1)


//==============================================================================
//	構造体定義
//==============================================================================
///配置物アクター構造体
typedef struct{
	CATS_ACT_PTR act[ACTWORK_MAX];///<スクリプト上で生成して管理する場合のポインタ保存場所
	u16 act_id[ACTWORK_MAX];	///<スクリプト上で生成して管理する場合のact_id保存場所
	u32 anmbit;				///<アニメ動作の有無(1ビット単位で管理)
	u16 resource_id[ACTWORK_RESOURCE_MAX];	///<スクリプト上で生成したアクターリソースのID保存場所
}FMAP_ARTICLE_ACT;

///地震制御構造体
typedef struct{
	TCB_PTR tcb;				///<地震動作実行タスク
	s16 shake_x;				///<揺れX
	s16 shake_y;				///<揺れY
	s8 wait;					///<地震待ち
	s8 wait_max;				///<ウェイト最大値
	s8 loop;					///<地震回数
	
	u8 padding;
}FMAP_SHAKE_WORK;

///WINDOW制御構造体
typedef struct{
	TCB_PTR tcb;
	u8 x1;
	u8 y1;
	u8 x2;
	u8 y2;
	s16 wait;
	s8 on_off;
	u8 padding;
}FMAP_WND_WORK;

//--------------------------------------------------------------
/**
 * @brief   フロンティアマップシステムワーク
 */
//--------------------------------------------------------------
typedef struct _FRONTIERMAP_PROC_WORK{
	//-- システム系 --//
	GF_BGL_INI *bgl;
	PALETTE_FADE_PTR pfd;			///<パレットフェードシステムへのポインタ
	FMAIN_PTR fmain;
	GF_G3DMAN *g3Dman;
	FRP_PTR frp;					///<フロンティアパーティクルシステムワークへのポインタ
	
	//2Dマップシステムモジュール
	WF2DMAP_OBJSYS *objsys;		///<オブジェクト管理モジュール
//	WF2DMAP_OBJWK *objwk[FIELD_OBJ_MAX];	///<オブジェクトワーク
	//マップデータ管理モジュール
	WF2DMAP_MAPSYS *mapsys;		///<マップデータ管理モジュール
	//スクロールデータ管理モジュール
	WF2DMAP_SCROLL scrollsys;	///<スクロールデータ管理モジュール(※これだけ実体)
	//オブジェクトデータ描画モジュール
	WF2DMAP_OBJDRAWSYS *drawsys;	///<オブジェクトデータ描画モジュール
//	WF2DMAP_OBJDRAWWK *drawwk[FIELD_OBJ_MAX];	///<描画オブジェクトワーク
	//スクロールデータ描画モジュール
	WF2DMAP_SCRDRAW *scrdrawsys;		///<スクロールデータ描画モジュール
	WF2DMAP_SCRDRAW *multi_scrdrawsys;	///<スクロールデータ描画モジュール(多重面用)
	//コマンドキュー
	WF2DMAP_REQCMDQ *reqcmd_q;		///<リクエストコマンド
	WF2DMAP_ACTCMDQ *actcmd_q;		///<アクションコマンド
	
	//セルアクター
	struct{
		CATS_SYS_PTR		csp;
		CATS_RES_PTR		crp;
	}clactsys;

	//配置物アクター
	FMAP_ARTICLE_ACT article;		///<配置物アクター
	
	//ポケモンアクター
	CATS_ACT_PTR poke_cap[POKE_DISP_MANAGER_ID_NUM];
	
	//シーンデータの初期動作関数内でAllocした場合のポインタ保持領域
	void *scene_func_work_ptr;
	
	//-- ローカル --//
	TCB_PTR objupdate_tcb;			///<オブジェクトシステム更新処理
	TCB_PTR cmdjudgeupdate_tcb;		///<コマンド判断実行処理
	TCB_PTR update_tcb;				///<システム更新処理
	TCB_PTR vintr_tcb;				///<Vブランク割り込みタスク
	
	FMAP_SHAKE_WORK shake_work;		///<地震制御構造体
	FMAP_WND_WORK wnd_work;			///<Window制御構造体
	
	u16 seq;
	u16 tbl_seq_no;					///<メインシーケンステーブルの番号
	u8 proc_mode;					///<現在の動作状況
	u8 func_scene_id;				///<シーン毎に実行される初期(終了)動作関数のシーンID

	u8 main_end;					///<TRUE:メイン処理終了
}FRONTIERMAP_WORK;


//==============================================================================
//	外部関数宣言
//==============================================================================
extern FMAP_PTR FrontierMap_Init(FMAIN_PTR fmain);
extern BOOL FrontierMap_Main(FMAP_PTR fmap);
extern void FrontierMap_End(FMAP_PTR fmap);
extern void FrontierMap_Push(FMAP_PTR fmap);
extern void FrontierMap_Pop(FMAP_PTR fmap);

extern void FMap_CharResourceSet(FMAP_PTR fmap, const FSS_CHAR_RESOURCE_DATA *res);
extern void FMap_CharResourceFree(FMAP_PTR fmap, int charid);
extern WF2DMAP_OBJWK * FMap_ActorSet(FMAP_PTR fmap, const FSS_CHAR_ACTOR_DATA *res, int auto_pos);
extern void FMap_ActorFree(FMAP_PTR fmap, WF2DMAP_OBJWK *objwk);
extern void FMap_OBJWkGet(FMAP_PTR fmap, u16 playid, 
	WF2DMAP_OBJWK **objwk, WF2DMAP_OBJDRAWWK **drawwk);

extern void FMap_ArticleActResourceIDSet(FMAP_PTR fmap, u16 act_id);
extern void FMap_ArticleActResourceIDDel(FMAP_PTR fmap, u16 act_id);
extern CATS_ACT_PTR FMap_ArticleActCreate(FMAP_PTR fmap, u16 work_no, u16 act_id);
extern void FMap_ArticleActDel(FMAP_PTR fmap, u16 work_no);
extern CATS_ACT_PTR FMap_ArticleActGet(FMAP_PTR fmap, u16 work_no);
extern void FMap_ArticleActAnmBitSet(FMAP_PTR fmap, u16 work_no, int anm_start);
extern u32 FMap_ArticleActAnmBitGet(FMAP_PTR fmap, u16 work_no);
extern void FMap_ScrollPosGet(FMAP_PTR fmap, s16 *ret_x, s16 *ret_y);

extern FSS_ACTOR_WORK * Frontier_ActorWorkSearch(FMAIN_PTR fmain, int playid);


#endif	//__FRONTIER_MAP_H__

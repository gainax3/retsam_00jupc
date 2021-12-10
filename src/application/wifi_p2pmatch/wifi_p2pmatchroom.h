//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifi_p2pmatchroom.h
 *	@brief		wifi	マッチングルーム
 *	@author		tomoya takahashi
 *	@data		2007.01.31
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WIFI_P2PMATCHROOM_H__
#define __WIFI_P2PMATCHROOM_H__

#include "clact.h"
#include "bg_system.h"
#include "system/arc_util.h"
#include "application/wifi_2dmap/wf2dmap_map.h"
#include "application/wifi_2dmap/wf2dmap_obj.h"
#include "application/wifi_2dmap/wf2dmap_judge.h"
#include "application/wifi_2dmap/wf2dmap_objdraw.h"
#include "application/wifi_2dmap/wf2dmap_scroll.h"
#include "application/wifi_2dmap/wf2dmap_scrdraw.h"
#include "application/wifi_2dmap/wf2dmap_cmdq.h"

#include "wifi_p2pmatchroom_map.h"


#undef GLOBAL
#ifdef	__WIFI_P2PMATCHROOM_H_GLOBAL
#define	GLOBAL	/* */
#else
#define	GLOBAL	extern
#endif

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
enum{	// 管理リソース種類
	MCR_CLACT_RESCHAR,
	MCR_CLACT_RESPLTT,
	MCR_CLACT_RESCELL,
	MCR_CLACT_RESCELLANM,
	MCR_CLACT_RESNUM
};
#define MCR_CLACT_LOADRESNUM	(16)// 読み込むリソースの種類
#define MCR_CLACT_OBJNUM	(96)// 管理オブジェクト数

// 人物オブジェクト用
#define MCR_MOVEOBJNUM		(48)// 動作オブジェクト数

// レンダラー系
#define MCR_CLACTSUBSURFACE_Y	(800*FX32_ONE)	// サブさーふぇーす


// 外側への戻り値
enum{
	MCR_RET_NONE,		// なし
	MCR_RET_CANCEL,		// キャンセル
	MCR_RET_SELECT,		// 何か選択
	MCR_RET_MYSELECT,	// 自分選択
};

//-------------------------------------
///	NPC動作定数
//=====================================
typedef enum {
	MCR_NPC_MOVE_NORMAL,
	MCR_NPC_MOVE_JUMP,
	MCR_NPC_MOVE_NUM,
} MCR_NPC_MOVETYPE;



//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	人物ワーク
//=====================================
typedef struct _WIFI_MATCHROOM WIFI_MATCHROOM;

typedef struct _MCR_MOVEOBJ{

	WF2DMAP_OBJWK* p_obj;
	WF2DMAP_OBJDRAWWK* p_draw;

	s16	move_count;	// エフェクト用カウンタ
	u8 accesFriend;// アクセスしている友達No	friendNoと一緒なら誰ともaccesしていない
	u8 moveID;			// NPC現在の動作
	u16 move_st;	// 今の動作
	u16 move_st_tmp;// 状態保存先
	
	// コマンドリクエスト
	BOOL (*pMove)( WIFI_MATCHROOM* p_mcr, struct _MCR_MOVEOBJ* p_obj );
	BOOL (*pMoveTmp)( WIFI_MATCHROOM* p_mcr, struct _MCR_MOVEOBJ* p_obj );
	
	// 特殊表示
	void (*pDraw)( WIFI_MATCHROOM* p_mcr, struct _MCR_MOVEOBJ* p_obj );
} MCR_MOVEOBJ;

//-------------------------------------
///	エフェクトワーク
//=====================================
typedef struct {
	CLACT_U_RES_OBJ_PTR resobj[4];	// エフェクトリソースバッファ
	CLACT_HEADER header;
	CLACT_WORK_PTR exit_cursor;
	CLACT_WORK_PTR obj_waku;
} MCR_EFFECT;

//-------------------------------------
///	セルアクターワーク
//=====================================
typedef struct {
	CLACT_SET_PTR 			clactSet;						// セルアクターセット
	CLACT_U_EASYRENDER_DATA	renddata;						// 簡易レンダーデータ
	CLACT_U_RES_MANAGER_PTR	resMan[MCR_CLACT_RESNUM];		// キャラ・パレットリソースマネージャ
	MCR_EFFECT effect;	// エフェクト
} MCR_CLACT; 


//-------------------------------------
///	パソコンアニメ
//=====================================
typedef struct {
	void* p_plbuff;
	NNSG2dPaletteData* p_pltt;
	u8 all_pcbitmap;	// 待機アニメさせるPCのビットマップ
	u8 all_seq;			// 待機アニメシーケンス
	s16 all_count;		// 待機アニメカウンタ
	u8 use_pc;			// 使用PC
	u8 use_pc_seq;		// 使用PCアニメシーケンス
	s16 use_pc_count;	// 使用PCアニメカウンタ
	u8 use_pc_next;		// 次に進んでほしいときのフラグ
	u8 pad[3];
} MCR_PCANM;



//-------------------------------------
///	WIFIマッチングルーム構造体
//=====================================
typedef struct _WIFI_MATCHROOM{
	BOOL	init;			// 初期化ﾁｪｯｸ用
	u32 use_heap;			// 使用するヒープ
	u32 seq;				// シーケンス
	BOOL player_pause;		// プレイヤーの移動を停止させる
	GF_BGL_INI*	p_bgl;		// BGL
	MCR_CLACT				clact;		// アクター情報
	MCR_MOVEOBJ				moveObj[MCR_MOVEOBJNUM];		// 動作オブジェクト
	MCR_MOVEOBJ*			p_player;	// 主人公は特別に取っておく		
	MCR_PCANM				pc_anm;		// pcアニメーション

	WF2DMAP_MAPSYS*		p_mapsys;
	WF2DMAP_OBJSYS*		p_objsys;
	WF2DMAP_OBJDRAWSYS* p_objdraw;
	WF2DMAP_SCROLL		scroll;
	WF2DMAP_SCRDRAW*	p_scrdraw;
	WF2DMAP_REQCMDQ*	p_reqcmdQ;	// リクエストコマンドキュー

} WIFI_MATCHROOM;


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
GLOBAL void WIFI_MCR_Init( WIFI_MATCHROOM* p_mcr, u32 heapID, ARCHANDLE* p_handle, GF_BGL_INI* p_bgl, u32 hero_view, u32 friendNum );
GLOBAL void WIFI_MCR_Dest( WIFI_MATCHROOM* p_mcr );
GLOBAL BOOL WIFI_MCR_GetInitFlag( const WIFI_MATCHROOM* cp_mcr );

GLOBAL u32 WIFI_MCR_Main( WIFI_MATCHROOM* p_mcr );
GLOBAL void WIFI_MCR_Draw( WIFI_MATCHROOM* p_mcr );
GLOBAL u8 WIFI_MCR_PlayerSelect( const WIFI_MATCHROOM* cp_mcr );
GLOBAL u32 WIFI_MCR_GetPlayerOnMapParam( const WIFI_MATCHROOM* cp_mcr );
GLOBAL u32 WIFI_MCR_GetPlayerOnUnderMapParam( const WIFI_MATCHROOM* cp_mcr );
GLOBAL void WIFI_MCR_CursorOn( WIFI_MATCHROOM* p_mcr, const MCR_MOVEOBJ* cp_obj );
GLOBAL void WIFI_MCR_CursorOff( WIFI_MATCHROOM* p_mcr );
GLOBAL void WIFI_MCR_PlayerMovePause( WIFI_MATCHROOM* p_mcr, BOOL flag );
GLOBAL BOOL WIFI_MCR_PlayerMovePauseGet( const WIFI_MATCHROOM* cp_mcr );


// オブジェクト関連
GLOBAL MCR_MOVEOBJ* WIFI_MCR_SetPlayer( WIFI_MATCHROOM* p_mcr, u32 view );
GLOBAL MCR_MOVEOBJ* WIFI_MCR_SetNpc( WIFI_MATCHROOM* p_mcr, u32 view, u8 friendNo );
GLOBAL void WIFI_MCR_DelPeopleReq( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj );
GLOBAL void WIFI_MCR_DelPeople( MCR_MOVEOBJ* p_obj );

GLOBAL u8	WIFI_MCR_GetFriendNo( const MCR_MOVEOBJ* cp_obj );
GLOBAL u32	WIFI_MCR_GetView( const MCR_MOVEOBJ* cp_obj );
GLOBAL WF2DMAP_WAY	WIFI_MCR_GetWay( const MCR_MOVEOBJ* cp_obj );
GLOBAL WF2DMAP_WAY	WIFI_MCR_GetRetWay( const MCR_MOVEOBJ* cp_obj );

// NPC操作
GLOBAL void WIFI_MCR_NpcPauseOn( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj, WF2DMAP_WAY way );
GLOBAL void WIFI_MCR_NpcPauseOff( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj );

// NPC動作設定
GLOBAL void WIFI_MCR_NpcMoveSet( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj, MCR_NPC_MOVETYPE moveID );

// PCアニメ設定
GLOBAL void WIFI_MCR_PCAnmStart( WIFI_MATCHROOM* p_mcr );
GLOBAL void WIFI_MCR_PCAnmOff( WIFI_MATCHROOM* p_mcr );
GLOBAL void WIFI_MCR_PCAnmMain( WIFI_MATCHROOM* p_mcr );

#undef	GLOBAL
#endif		// __WIFI_P2PMATCHROOM_H__


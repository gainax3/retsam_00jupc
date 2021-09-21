//==============================================================================
/**
 * @file	bb_local.h
 * @brief	簡単な説明を書く
 * @author	goto
 * @date	2007.09.25(火)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================

#ifndef __BB_LOCAL_H__
#define __BB_LOCAL_H__

#include "application/balance_ball.h"

#include "system/clact_tool.h"
#include "system/snd_tool.h"
#include "system/d3dobj.h"
#include "system\font_arc.h"

#include "include/gflib/strbuf.h"
#include "include/gflib/msg_print.h"
#include "system/fontproc.h"
#include "system/window.h"
#include "include/system/fontproc.h"
#include "include/system/msgdata.h"
#include "include/msgdata/msg.naix"

#include "gflib/button_man.h"

#include "src/graphic/bucket.naix"
#include "balance_ball_gra.naix"
#include "manene.naix"

#include "system/quaternion.h"
#include "src/application/wifi_lobby/minigame_tool.h"

#include "system/wipe.h"

#include "application/wifi_lobby/minigame_tool.h"

#include "bb_snd.h"

//#include "br_snd_def.h"

/// palette
enum {
	eBB_OAM_PAL_TD_MANENE	= 0,		///< 上画面 マネネ
	eBB_OAM_PAL_TD_MANENE_2,
	eBB_OAM_PAL_TD_MANENE_3,
	eBB_OAM_PAL_TD_MANENE_4,
	eBB_OAM_PAL_TD_MANENE_5,
	eBB_OAM_PAL_TD_MANENE_6,
	eBB_OAM_PAL_TD_LIGHT_0,
	eBB_OAM_PAL_TD_LIGHT_1,
	eBB_OAM_PAL_TD_LIGHT_2,
	eBB_OAM_PAL_TD_LVUP_0,
	eBB_OAM_PAL_TD_LVUP_1,
	eBB_OAM_PAL_TD_LVUP_2,
	eBB_OAM_PAL_TD_LVUP_3,
	
	eBB_OAM_PAL_TD_MAX,
	
//	eBB_OAM_PAL_BD_CD		= 0,		///< 下画面 カウントダウン
	eBB_OAM_PAL_BD_STAR		= 0,
	eBB_OAM_PAL_BD_HAND,
	eBB_OAM_PAL_BD_KAMI,
	eBB_OAM_PAL_BD_HANABI,
	eBB_OAM_PAL_BD_LIGHT,
	eBB_OAM_PAL_BD_LIGHT_2,
	eBB_OAM_PAL_BD_LIGHT_3,	
	eBB_OAM_PAL_BD_PEN,
	eBB_OAM_PAL_BD_PEN_1,
};

enum {	
	eBB_ID_OAM_S	= 9000,
	eBB_ID_LIGHT_S,
	eBB_ID_LIGHT_M,
	eBB_ID_STAR,
	eBB_ID_HAND,
	eBB_ID_KAMI,
	eBB_ID_HANABI,	
	
	eBB_ID_PEN,
};

///< セルアニメの定義
enum {	
	eANM_MANENE_ARUKU = 0,
	eANM_MANENE_OCHIRU,
	eANM_BALL_STOP,
	eANM_BALL_ARUKU,
	eANM_BALL_ARUKU2,
	eANM_KAGE,
	eANM_SPOT_LIGHT,
	eANM_LEVEL_EFF,
	
	eANM_CODE_STOP = 0,
	eANM_CODE_ARUKU,
	eANM_CODE_ARUKU2,
	eANM_CODE_OCHIRU,
};

///< モデルアニメの定義
enum {
	eANM_CODE_DEFAULT = 0,	///< 通常
	eANM_CODE_FALL,			///< 落下
	eANM_CODE_RECOVER,		///< 復帰

	ePAT_3D_ARUKU	  = 0,	///< あるき
	ePAT_3D_ARARA,			///< あらら
	ePAT_3D_OTTOTTO,		///< おっと
};


///< 通信エラー用
typedef struct {
	
	u8	dis_err;
	u8	pad[ 3 ];
	
} BB_COMM_ERR_DATA;


typedef struct {
	
	ARCHANDLE*			p_handle_bb;		///< アーカイブハンドル ( 玉乗り用 )
	ARCHANDLE*			p_handle_cr;		///< アーカイブハンドル ( 共通リソース )
	
	CATS_SYS_PTR		csp;				///< OAMシステム
	CATS_RES_PTR		crp;				///< リソース一括管理

	GF_BGL_INI*			bgl;				///< BGL
	PALETTE_FADE_PTR	pfd;				///< パレットフェード
	
	GF_G3DMAN*			g3Dman;				///< 3D
	GF_CAMERA_PTR		camera_p;			///< カメラ
	
	BUTTON_MAN*			btn;				///< ボタン
	
//	BUTTON_MAN*			btn;				///< ボタン	
//	RECT_HIT_TBL		rht[ eHRT_MAX ];	///< 当たり判定(managerに登録する)
//	MSGDATA_MANAGER*	man;
//	FONTOAM_SYS_PTR		fontoam_sys;
//	int					logo_color;
	NNSFndAllocator		allocator;
	
	BB_COMM_ERR_DATA	comm_err_data;
	
} BB_SYS;

#endif

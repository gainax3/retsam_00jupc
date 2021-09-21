#ifndef __COMMON_H__
#define __COMMON_H__

//=========================================================================
/**
 * @file	common.h
 * @brief	共通定義ファイル
 * @author	GAME FREAK inc.
 *
 */
//=========================================================================

#include "gflib/system.h"					//グローバル変数定義
#include "gflib/standard.h"
#include "gflib/tcb.h"					//Task Control Block
#include "gflib/mmap_get.h"
// ↑↑↑重要そうなファイル↑↑↑
// ↓↓↓アルファベット順　↓↓↓
#include "gflib/bg_system.h"				//ＢＧコントロールシステム
#include "gflib/blact.h"
#include "gflib/calc2d.h"
#include "gflib/calc3d.h"
#include "gflib/calctool.h"
#include "gflib/camera.h"					//基本カメラシステム
#include "gflib/clact.h"
#include "gflib/char_manager.h"
#include "gflib/display.h"				//表示画面設定
#include "gflib/bounding_box.h"
#include "gflib/fntequ_agb_jp.h"
#include "gflib/fntsys.h"
#include "gflib/msg_print.h"
#include "gflib/pltt_manager.h"
#include "gflib/point_sel.h"
#include "gflib/simple_3dgraphics.h"		//基本３Ｄグラフィックシステム
#include "gflib/str_tool.h"
#include "gflib/touchpanel_system.h"
#include "gflib/vram_transfer_anm.h"
#include "gflib/vram_transfer_manager.h"
#include "gflib/sleep.h"
#include "gflib/res_manager.h"
#include "gflib/texanm_sys.h"
#include "gflib/gf_gx.h"
#include "gflib/g3d_system.h"

#include "src_os_print.h"			// OS_Printf()の有効/無効定義


#include "gflib\assert.h"
#include "gflib\sdkdef.h"




#define	GLOBAL_MSGLEN	( 2 )		///<文字列データ補正定義


//#define	PAD_BUTTON_DECIDE	( PAD_BUTTON_A|PAD_BUTTON_X )	// 決定ボタン定義
//#define	PAD_BUTTON_CANCEL	( PAD_BUTTON_B|PAD_BUTTON_Y )	// キャンセルボタン定義
#define	PAD_BUTTON_DECIDE	( PAD_BUTTON_A )	// 決定ボタン定義
#define	PAD_BUTTON_CANCEL	( PAD_BUTTON_B )	// キャンセルボタン定義


//==============================================================================
//	サーバーの製品版、開発版への接続切り替え
//==============================================================================
///認証サーバーの開発用サーバー、製品版サーバーの切り替え
// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ALL) imatake 2008/07/29
// PG5_WIFIRELEASE = yes なら、Wi-Fi関係がリリースサーバに接続

#ifdef PG5_WIFIRELEASE
//製品版用サーバー
#define GF_DWC_CONNECTINET_AUTH_TYPE	(DWC_CONNECTINET_AUTH_RELEASE)
//Wi-Fiロビーサーバ
#define GF_DWC_LOBBY_CHANNEL_PREFIX		(DWC_LOBBY_CHANNEL_PREFIX_RELEASE)
#else
//開発用サーバー
#define GF_DWC_CONNECTINET_AUTH_TYPE	(DWC_CONNECTINET_AUTH_TEST)
//Wi-Fiロビーサーバ
#define GF_DWC_LOBBY_CHANNEL_PREFIX		(PPW_LOBBY_CHANNEL_PREFIX_DEBUG)
#endif

// ----------------------------------------------------------------------------

#endif //__COMMON_H__

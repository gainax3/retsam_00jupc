#ifndef __COMMON_H__
#define __COMMON_H__

//=========================================================================
/**
 * @file	common.h
 * @brief	‹¤’Ê’è‹`ƒtƒ@ƒCƒ‹
 * @author	GAME FREAK inc.
 *
 */
//=========================================================================

#include "gflib/system.h"					//ƒOƒ[ƒoƒ‹•Ï”’è‹`
#include "gflib/standard.h"
#include "gflib/tcb.h"					//Task Control Block
#include "gflib/mmap_get.h"
// ªªªd—v‚»‚¤‚Èƒtƒ@ƒCƒ‹ªªª
// «««ƒAƒ‹ƒtƒ@ƒxƒbƒg‡@«««
#include "gflib/bg_system.h"				//‚a‚fƒRƒ“ƒgƒ[ƒ‹ƒVƒXƒeƒ€
#include "gflib/blact.h"
#include "gflib/calc2d.h"
#include "gflib/calc3d.h"
#include "gflib/calctool.h"
#include "gflib/camera.h"					//Šî–{ƒJƒƒ‰ƒVƒXƒeƒ€
#include "gflib/clact.h"
#include "gflib/char_manager.h"
#include "gflib/display.h"				//•/¦‰æ–Êİ’è
#include "gflib/bounding_box.h"
#include "gflib/fntequ_agb_jp.h"
#include "gflib/fntsys.h"
#include "gflib/msg_print.h"
#include "gflib/pltt_manager.h"
#include "gflib/point_sel.h"
#include "gflib/simple_3dgraphics.h"		//Šî–{‚R‚cƒOƒ‰ƒtƒBƒbƒNƒVƒXƒeƒ€
#include "gflib/str_tool.h"
#include "gflib/touchpanel_system.h"
#include "gflib/vram_transfer_anm.h"
#include "gflib/vram_transfer_manager.h"
#include "gflib/sleep.h"
#include "gflib/res_manager.h"
#include "gflib/texanm_sys.h"
#include "gflib/gf_gx.h"
#include "gflib/g3d_system.h"

#include "src_os_print.h"			// OS_Printf()‚Ì—LŒø/–³Œø’è‹`


#include "gflib/assert.h"
#include "gflib/sdkdef.h"




#define	GLOBAL_MSGLEN	( 2 )		///<•¶š—ñƒf[ƒ^•â³’è‹`


//#define	PAD_BUTTON_DECIDE	( PAD_BUTTON_A|PAD_BUTTON_X )	// Œˆ’èƒ{ƒ^ƒ“’è‹`
//#define	PAD_BUTTON_CANCEL	( PAD_BUTTON_B|PAD_BUTTON_Y )	// ƒLƒƒƒ“ƒZƒ‹ƒ{ƒ^ƒ“’è‹`
#define	PAD_BUTTON_DECIDE	( PAD_BUTTON_A )	// Œˆ’èƒ{ƒ^ƒ“’è‹`
#define	PAD_BUTTON_CANCEL	( PAD_BUTTON_B )	// ƒLƒƒƒ“ƒZƒ‹ƒ{ƒ^ƒ“’è‹`


//==============================================================================
//	ƒT[ƒo[‚Ì»•i”ÅAŠJ”­”Å‚Ö‚ÌÚ‘±Ø‚è‘Ö‚¦
//==============================================================================
///”FØƒT[ƒo[‚ÌŠJ”­—pƒT[ƒo[A»•i”ÅƒT[ƒo[‚ÌØ‚è‘Ö‚¦
// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ALL) imatake 2008/07/29
// PG5_WIFIRELEASE = yes ‚È‚çAWi-FiŠÖŒW‚ªƒŠƒŠ[ƒXƒT[ƒo‚ÉÚ‘±

#ifdef PG5_WIFIRELEASE
//»•i”Å—pƒT[ƒo[
#define GF_DWC_CONNECTINET_AUTH_TYPE	(DWC_CONNECTINET_AUTH_RELEASE)
//Wi-Fiƒƒr[ƒT[ƒo
#define GF_DWC_LOBBY_CHANNEL_PREFIX		(DWC_LOBBY_CHANNEL_PREFIX_RELEASE)
#else
//ŠJ”­—pƒT[ƒo[
#define GF_DWC_CONNECTINET_AUTH_TYPE	(DWC_CONNECTINET_AUTH_TEST)
//Wi-Fiƒƒr[ƒT[ƒo
#define GF_DWC_LOBBY_CHANNEL_PREFIX		(PPW_LOBBY_CHANNEL_PREFIX_DEBUG)
#endif

// ----------------------------------------------------------------------------

#endif //__COMMON_H__

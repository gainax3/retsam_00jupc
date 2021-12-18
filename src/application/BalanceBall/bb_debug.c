
#ifdef PM_DEBUG

#include "common.h"
#include "system/wipe.h"

#include "application/balance_ball.h"

#include "bb_common.h"
#include "bb_comm_cmd.h"

#include "bb_server.h"
#include "bb_client.h"

#include "bb_game.h"

typedef struct {
	
	GF_BGL_BMPWIN	win;
	BB_SYS*			sys;
	int				seq;
	
} DEBUG_WORK;

static DEBUG_WORK	dwk = { 0 };
static DEBUG_WORK*  wk = &dwk;

//--------------------------------------------------------------
/**
 * @brief	Game Setup
 *
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
BOOL Debug_GameSetup( BB_WORK* work )
{
	GF_BGL_BMPWIN* win = &wk->win;
		
	wk->sys = &work->sys;
	
	if ( wk->seq == 0 ) {
		
		GF_BGL_BmpWinInit( win );
		GF_BGL_BmpWinAdd( wk->sys->bgl, win, GF_BGL_FRAME3_M, 1, 1, 30, 22, 14, 1 );
		GF_BGL_BmpWinDataFill( win, 0xFF );
		GF_BGL_BmpWinOn( win );
		
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
		
		wk->seq++;
	}
	
	if ( sys.trg & PAD_BUTTON_B ){
		ArcUtil_HDL_BgCharSet( wk->sys->p_handle_bb, NARC_balance_ball_gra_manene_bottom_NCGR, wk->sys->bgl, GF_BGL_FRAME1_M, 0, 0, 0, HEAPID_BB );	
		ArcUtil_HDL_ScrnSet(   wk->sys->p_handle_bb, NARC_balance_ball_gra_manene_bottom_NSCR, wk->sys->bgl, GF_BGL_FRAME1_M, 0, 0, 0, HEAPID_BB );
		
	    GF_BGL_BmpWinOff( win );
		GF_BGL_BmpWinDel( win );		
		return TRUE;
	}
	
	return FALSE;
}


#endif

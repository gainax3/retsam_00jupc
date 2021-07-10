//============================================================================================
/**
 * @file	pmsiv_sub.c
 * @bfief	簡易会話入力画面（描画下請け：サブ画面）
 * @author	taya
 * @date	06.02.10
 */
//============================================================================================
#include "common.h"
#include "gflib\heapsys.h"
#include "gflib\strbuf.h"
#include "system\arc_util.h"
#include "system\window.h"
#include "system\pms_word.h"
#include "system\winframe.naix"
#include "system\msgdata.h"
#include "msgdata\msg_pms_input.h"
#include "msgdata\msg.naix"


#include "pms_input_prv.h"
#include "pms_input_view.h"


//======================================================================

enum {


	GROUP_WIN_XPOS = 4,
	GROUP_WIN_YPOS = 5,
	GROUP_WIN_PALNO = 0,

	INITIAL_WIN_XPOS = 19,
	INITIAL_WIN_YPOS = 5,
	INITIAL_WIN_PALNO = 1,

	BUTTON_WIN_WIDTH = 9,
	BUTTON_WIN_HEIGHT = 5,
	BUTTON_WIN_CHARSIZE = BUTTON_WIN_WIDTH*BUTTON_WIN_HEIGHT,

	PUSH_BUTTON_WRITE_OX = 5,
	PUSH_BUTTON_WRITE_OY = 5,
	RELEASE_BUTTON_WRITE_OX = 4,
	RELEASE_BUTTON_WRITE_OY = 4,

	WIN_COL_PUSH_GROUND = 0x0a,
	WIN_COL_RELEASE_GROUND = 0x09,
	WIN_COL1 = 0x01,
	WIN_COL2 = 0x02,
	WIN_COL3 = 0x03,

};

enum {
	SCREEN_PATTERN_PUSH_GROUP,
	SCREEN_PATTERN_PUSH_INITIAL,
};

enum {
	ANM_MODEBUTTON_OFF,
	ANM_MODEBUTTON_ON,
	ANM_MODEBUTTON_PUSH,
};


enum {
	MODEBUTTON_SCRN_SIZE = MODEBUTTON_SCRN_WIDTH * MODEBUTTON_SCRN_HEIGHT,

	SCRN_ID_GROUP_OFF = 0,
	SCRN_ID_GROUP_PUSH1,
	SCRN_ID_GROUP_PUSH2,
	SCRN_ID_GROUP_ON,
	SCRN_ID_INITIAL_OFF,
	SCRN_ID_INITIAL_PUSH1,
	SCRN_ID_INITIAL_PUSH2,
	SCRN_ID_INITIAL_ON,
	SCRN_ID_MAX,

};


enum {
	ACTANM_ARROW_UP_STOP,
	ACTANM_ARROW_UP_HOLD,
	ACTANM_ARROW_DOWN_STOP,
	ACTANM_ARROW_DOWN_HOLD,
};


//======================================================================


//======================================================================
typedef struct {
	void*					cellLoadPtr;
	NNSG2dCellDataBank*		cellData;
	void*					animLoadPtr;
	NNSG2dAnimBankData*		animData;
}CELL_ANIM_PACK;


//--------------------------------------------------------------
/**
 *	
 */
//--------------------------------------------------------------
struct _PMSIV_SUB {
	PMS_INPUT_VIEW*        vwk;
	const PMS_INPUT_WORK*  mwk;
	const PMS_INPUT_DATA*  dwk;

	GF_BGL_INI*        bgl;

	CLACT_SET_PTR      actsys;
	CLACT_WORK_PTR     up_button;
	CLACT_WORK_PTR     down_button;
	CELL_ANIM_PACK     clpack;

	u16   button_scrn[SCRN_ID_MAX][MODEBUTTON_SCRN_SIZE];

	TCB_PTR       changeButtonTask;

};


//==============================================================
// Prototype
//==============================================================
static void load_scrn_datas( PMSIV_SUB* wk, ARCHANDLE* p_handle );
static void setup_cgx_datas( PMSIV_SUB* wk, ARCHANDLE* p_handle );
static void print_mode_name( GF_BGL_BMPWIN* win, GF_BGL_INI* bgl, const STRBUF* str, int yofs );
static void setup_actors( PMSIV_SUB* wk, ARCHANDLE* p_handle );
static void cleanup_actors( PMSIV_SUB* wk );
static void load_clpack( CELL_ANIM_PACK* clpack, ARCHANDLE* p_handle, u32 cellDatID, u32 animDatID );
static void unload_clpack( CELL_ANIM_PACK* clpack );
static void ChangeButtonTask( TCB_PTR tcb, void* wk_adrs );


 static CLACT_WORK_PTR add_actor(
	PMSIV_SUB* wk, NNSG2dImageProxy* imgProxy, NNSG2dImagePaletteProxy* palProxy,
	CELL_ANIM_PACK* clpack, int xpos, int ypos, int bgpri, int actpri );



//------------------------------------------------------------------
/**
 * 
 *
 * @param   vwk		
 * @param   mwk		
 * @param   dwk		
 *
 * @retval  PMSIV_SUB*		
 */
//------------------------------------------------------------------
PMSIV_SUB*  PMSIV_SUB_Create( PMS_INPUT_VIEW* vwk, const PMS_INPUT_WORK* mwk, const PMS_INPUT_DATA* dwk )
{
	PMSIV_SUB*  wk = sys_AllocMemory( HEAPID_PMS_INPUT_VIEW, sizeof(PMSIV_SUB) );

	wk->vwk = vwk;
	wk->mwk = mwk;
	wk->dwk = dwk;
	wk->bgl = PMSIView_GetBGL(vwk);
	wk->actsys = PMSIView_GetActSys(vwk);

	wk->up_button = NULL;
	wk->down_button = NULL;
	wk->changeButtonTask = NULL;

	return wk;
}
//------------------------------------------------------------------
/**
 * 
 *
 * @param   wk		
 *
 */
//------------------------------------------------------------------
void PMSIV_SUB_Delete( PMSIV_SUB* wk )
{
	cleanup_actors( wk );

	sys_FreeMemoryEz( wk );
}



//------------------------------------------------------------------
/**
 * 
 *
 * @param   wk		
 *
 */
//------------------------------------------------------------------
void PMSIV_SUB_SetupGraphicDatas( PMSIV_SUB* wk, ARCHANDLE* p_handle )
{
	ArcUtil_HDL_PalSet( p_handle, NARC_pmsi_bg_sub_nclr, PALTYPE_SUB_BG, 0, 0x40, HEAPID_PMS_INPUT_VIEW );
	ArcUtil_HDL_ScrnSet( p_handle, NARC_pmsi_bg_sub_lz_nscr, wk->bgl, FRM_SUB_BG, 0, 0, TRUE, HEAPID_PMS_INPUT_VIEW );


	load_scrn_datas( wk, p_handle );
	setup_cgx_datas( wk, p_handle );

	if( PMSI_GetCategoryMode( wk->mwk ) == CATEGORY_MODE_GROUP )
	{
		GF_BGL_ScrWriteExpand( wk->bgl, FRM_SUB_BG, BUTTON_GROUP_SCRN_XPOS, BUTTON_GROUP_SCRN_YPOS,
					MODEBUTTON_SCRN_WIDTH, MODEBUTTON_SCRN_HEIGHT, wk->button_scrn[SCRN_ID_GROUP_ON],
					0, 0, MODEBUTTON_SCRN_WIDTH, MODEBUTTON_SCRN_HEIGHT );

		GF_BGL_ScrWriteExpand( wk->bgl, FRM_SUB_BG, BUTTON_INITIAL_SCRN_XPOS, BUTTON_INITIAL_SCRN_YPOS,
					MODEBUTTON_SCRN_WIDTH, MODEBUTTON_SCRN_HEIGHT, wk->button_scrn[SCRN_ID_INITIAL_OFF],
					0, 0, MODEBUTTON_SCRN_WIDTH, MODEBUTTON_SCRN_HEIGHT );
	}
	else
	{
		GF_BGL_ScrWriteExpand( wk->bgl, FRM_SUB_BG, BUTTON_GROUP_SCRN_XPOS, BUTTON_GROUP_SCRN_YPOS,
					MODEBUTTON_SCRN_WIDTH, MODEBUTTON_SCRN_HEIGHT, wk->button_scrn[SCRN_ID_GROUP_OFF],
					0, 0, MODEBUTTON_SCRN_WIDTH, MODEBUTTON_SCRN_HEIGHT );

		GF_BGL_ScrWriteExpand( wk->bgl, FRM_SUB_BG, BUTTON_INITIAL_SCRN_XPOS, BUTTON_INITIAL_SCRN_YPOS,
					MODEBUTTON_SCRN_WIDTH, MODEBUTTON_SCRN_HEIGHT, wk->button_scrn[SCRN_ID_INITIAL_ON],
					0, 0, MODEBUTTON_SCRN_WIDTH, MODEBUTTON_SCRN_HEIGHT );

	}

	setup_actors( wk, p_handle );

	GF_BGL_LoadScreenReq( wk->bgl, FRM_SUB_BG );

}
static void load_scrn_datas( PMSIV_SUB* wk, ARCHANDLE* p_handle )
{
	static const u16 dat_id[] = {
		NARC_pmsi_bg_sub_a1_lz_nscr,
		NARC_pmsi_bg_sub_a2_lz_nscr,
		NARC_pmsi_bg_sub_a3_lz_nscr,
		NARC_pmsi_bg_sub_a4_lz_nscr,
		NARC_pmsi_bg_sub_b1_lz_nscr,
		NARC_pmsi_bg_sub_b2_lz_nscr,
		NARC_pmsi_bg_sub_b3_lz_nscr,
		NARC_pmsi_bg_sub_b4_lz_nscr,
	};
	int i;
	void* loadPtr;
	NNSG2dScreenData* scrnData;

	for(i=0; i<NELEMS(dat_id); i++)
	{
		loadPtr = ArcUtil_HDL_ScrnDataGet(p_handle, dat_id[i], TRUE, &scrnData, HEAPID_PMS_INPUT_VIEW);
		if( loadPtr )
		{
			MI_CpuCopy16( scrnData->rawData, wk->button_scrn[i], MODEBUTTON_SCRN_SIZE*2 );
			DC_FlushRange( wk->button_scrn[i], MODEBUTTON_SCRN_SIZE*2 );
			sys_FreeMemoryEz( loadPtr );
		}
	}
}

#ifdef NONEQUIVALENT
static void setup_cgx_datas( PMSIV_SUB* wk, ARCHANDLE* p_handle )
{
	// ----------------------------------------------------------------------------
	// localize_spec_mark(LANG_ALL) imatake 2007/01/09
	// モードボタンの文字を中央寄せ

	enum {
		CGX_WIDTH  = 11,
		CGX_HEIGHT = 57,
		LINE_HEIGHT = 16,
	};

	STRBUF *str_group,   *str_group2;
	STRBUF *str_initial, *str_initial2;
	void* loadPtr;
	NNSG2dCharacterData* charData;

	FontProc_LoadFont( FONT_BUTTON, HEAPID_BASE_SYSTEM );
	str_group    = MSGDAT_GetStrDirectAlloc(ARC_MSG, NARC_msg_pms_input_dat, str_group_mode,    HEAPID_PMS_INPUT_VIEW );
	str_group2   = MSGDAT_GetStrDirectAlloc(ARC_MSG, NARC_msg_pms_input_dat, str_group_mode2,   HEAPID_PMS_INPUT_VIEW );
	str_initial  = MSGDAT_GetStrDirectAlloc(ARC_MSG, NARC_msg_pms_input_dat, str_initial_mode,  HEAPID_PMS_INPUT_VIEW );
	str_initial2 = MSGDAT_GetStrDirectAlloc(ARC_MSG, NARC_msg_pms_input_dat, str_initial_mode2, HEAPID_PMS_INPUT_VIEW );

	loadPtr = ArcUtil_CharDataGet(ARC_PMSI_GRAPHIC, NARC_pmsi_bg_sub_lz_ncgr, TRUE, &charData, HEAPID_PMS_INPUT_VIEW);
	if(loadPtr)
	{
		GF_BGL_BMPWIN   win;
		int i;

		GF_BGL_BmpWinInit(&win);
		win.ini = wk->bgl;
		win.sizx = CGX_WIDTH;
		win.sizy = CGX_HEIGHT;
		win.bitmode = GF_BGL_BMPWIN_BITMODE_4;
		win.chrbuf = charData->pRawData;
		print_mode_name( &win, wk->bgl, str_group,  0 );
		print_mode_name( &win, wk->bgl, str_group2, LINE_HEIGHT );

		win.chrbuf = (u8*)(charData->pRawData) + ((CGX_WIDTH * (MODEBUTTON_SCRN_HEIGHT*4))*0x20);
		print_mode_name( &win, wk->bgl, str_initial,  0 );
		print_mode_name( &win, wk->bgl, str_initial2, LINE_HEIGHT );

		DC_FlushRange( charData->pRawData, charData->szByte );
		GF_BGL_LoadCharacter(wk->bgl, FRM_SUB_BG, charData->pRawData, charData->szByte, 0);

		sys_FreeMemoryEz(loadPtr);
	}

	STRBUF_Delete(str_initial);
	STRBUF_Delete(str_initial2);
	STRBUF_Delete(str_group);
	STRBUF_Delete(str_group2);
	FontProc_UnloadFont( FONT_BUTTON );

	// ----------------------------------------------------------------------------
}
#else
asm static void setup_cgx_datas( PMSIV_SUB* wk, ARCHANDLE* p_handle )
{
	push {r3, r4, r5, r6, r7, lr}
	sub sp, #0x18
	add r5, r0, #0
	add r7, r1, #0
	mov r0, #2
	mov r1, #0
	bl FontProc_LoadFont
	ldr r1, =0x000001B5 // _021D4984
	mov r0, #0x1a
	mov r2, #0xb
	mov r3, #0x23
	bl MSGDAT_GetStrDirectAlloc
	add r6, r0, #0
	ldr r1, =0x000001B5 // _021D4984
	mov r0, #0x1a
	mov r2, #0xc
	mov r3, #0x23
	bl MSGDAT_GetStrDirectAlloc
	add r4, r0, #0
	mov r0, #0x23
	str r0, [sp]
	add r0, r7, #0
	mov r1, #0x14
	mov r2, #1
	add r3, sp, #4
	bl ArcUtil_HDL_CharDataGet
	add r7, r0, #0
	beq _021D496C
	add r0, sp, #8
	bl GF_BGL_BmpWinInit
	ldr r0, [r5, #0xc]
	add r1, sp, #4
	str r0, [sp, #8]
	mov r0, #0xb
	strb r0, [r1, #0xb]
	mov r0, #0x39
	strb r0, [r1, #0xc]
	ldrh r2, [r1, #0xe]
	ldr r0, =0xFFFF7FFF // _021D4988
	mov r3, #0
	and r0, r2
	strh r0, [r1, #0xe]
	ldr r0, [sp, #4]
	add r2, r6, #0
	ldr r0, [r0, #0x14]
	str r0, [sp, #0x14]
	ldr r1, [r5, #0xc]
	add r0, sp, #8
	bl print_mode_name
	ldr r0, [sp, #4]
	add r2, r4, #0
	ldr r1, [r0, #0x14]
	mov r0, #0x9a
	lsl r0, r0, #6
	add r0, r1, r0
	str r0, [sp, #0x14]
	ldr r1, [r5, #0xc]
	add r0, sp, #8
	mov r3, #0
	bl print_mode_name
	ldr r1, [sp, #4]
	ldr r0, [r1, #0x14]
	ldr r1, [r1, #0x10]
	bl DC_FlushRange
	ldr r3, [sp, #4]
	mov r0, #0
	str r0, [sp]
	ldr r2, [r3, #0x14]
	ldr r0, [r5, #0xc]
	ldr r3, [r3, #0x10]
	mov r1, #4
	bl GF_BGL_LoadCharacter
	add r0, r7, #0
	bl sys_FreeMemoryEz
_021D496C:
	add r0, r4, #0
	bl STRBUF_Delete
	add r0, r6, #0
	bl STRBUF_Delete
	mov r0, #2
	bl FontProc_UnloadFont
	add sp, #0x18
	pop {r3, r4, r5, r6, r7, pc}
	nop
// _021D4984: .4byte 0x000001B5
// _021D4988: .4byte 0xFFFF7FFF
}
#endif

#ifdef NONEQUIVALENT
static void print_mode_name( GF_BGL_BMPWIN* win, GF_BGL_INI* bgl, const STRBUF* str, int yofs )
{
	// ----------------------------------------------------------------------------
	// localize_spec_mark(LANG_ALL) imatake 2007/01/09
	// モードボタンの文字を中央寄せ

	enum {
		COL_1 = 0x01,
		COL_2 = 0x02,
		COL_3 = 0x03,

		WRITE_Y_ORG = 22,
		WRITE_Y_DIFF = 56,

		BUTTON_WIDTH = 88,		// setup_cgs_datas() の CGX_WIDTH * 8
	};

	static const s16 write_y[] = {
		 WRITE_Y_ORG,
		(WRITE_Y_ORG + WRITE_Y_DIFF*1)-1,
		(WRITE_Y_ORG + WRITE_Y_DIFF*2)-2,
		(WRITE_Y_ORG + WRITE_Y_DIFF*3)-1,
	};

	int i, x, y;

	x = (BUTTON_WIDTH - FontProc_GetPrintMaxLineWidth(FONT_BUTTON, str, 0)) / 2;	// 1行ずつに変更したが念のため複数行対応

	for(i=0; i<NELEMS(write_y); i++)
	{
		y = write_y[i] + yofs;
		GF_STR_PrintColor( win, FONT_BUTTON, str, x, y,
							MSG_NO_PUT, GF_PRINTCOLOR_MAKE(COL_1,COL_2,COL_3), NULL );
	}

	// ----------------------------------------------------------------------------
}
#else
enum {
    COL_1 = 0x01,
    COL_2 = 0x02,
    COL_3 = 0x03,

    WRITE_Y_ORG = 22,
    WRITE_Y_DIFF = 56,

    BUTTON_WIDTH = 88,		// setup_cgs_datas() の CGX_WIDTH * 8
};

static const s16 write_y[] = {
     WRITE_Y_ORG,
    (WRITE_Y_ORG + WRITE_Y_DIFF*1)-1,
    (WRITE_Y_ORG + WRITE_Y_DIFF*2)-2,
    (WRITE_Y_ORG + WRITE_Y_DIFF*3)-1,
};

asm static void print_mode_name( GF_BGL_BMPWIN* win, GF_BGL_INI* bgl, const STRBUF* str, int yofs )
{
	push {r4, r5, r6, r7, lr}
	sub sp, #0x24
	str r0, [sp, #0x10]
	add r0, r2, #0
	str r2, [sp, #0x14]
	add r7, r3, #0
	bl STRBUF_GetLines
	str r0, [sp, #0x1c]
	mov r0, #0x20
	mov r1, #0x23
	bl STRBUF_Create
	add r6, r0, #0
	ldr r0, [sp, #0x1c]
	mov r1, #2
	sub r0, r1, r0
	lsl r0, r0, #4
	lsr r0, r0, #1
	add r7, r7, r0
	mov r0, #0
	str r0, [sp, #0x20]
	ldr r0, [sp, #0x1c]
	cmp r0, #0
	bls _021D4A10
_021D49BE:
	ldr r1, [sp, #0x14]
	ldr r2, [sp, #0x20]
	add r0, r6, #0
	bl STRBUF_CopyLine
	mov r0, #2
	add r1, r6, #0
	mov r2, #0
	mov r3, #0x58
	bl FontProc_GetPrintCenteredPositionX
	ldr r5, =write_y // _021D4A1C
	str r0, [sp, #0x18]
	mov r4, #0
_021D49DA:
	mov r0, #0
	ldrsh r0, [r5, r0]
	ldr r3, [sp, #0x18]
	mov r1, #2
	add r0, r7, r0
	str r0, [sp]
	mov r0, #0xff
	str r0, [sp, #4]
	ldr r0, =0x00010203 // _021D4A20
	add r2, r6, #0
	str r0, [sp, #8]
	mov r0, #0
	str r0, [sp, #0xc]
	ldr r0, [sp, #0x10]
	bl GF_STR_PrintColor
	add r4, r4, #1
	add r5, r5, #2
	cmp r4, #4
	blo _021D49DA
	ldr r0, [sp, #0x20]
	add r7, #0x10
	add r1, r0, #1
	ldr r0, [sp, #0x1c]
	str r1, [sp, #0x20]
	cmp r1, r0
	blo _021D49BE
_021D4A10:
	add r0, r6, #0
	bl STRBUF_Delete
	add sp, #0x24
	pop {r4, r5, r6, r7, pc}
	nop
// _021D4A1C: .4byte write_y
// _021D4A20: .4byte 0x00010203
}
#endif

//==============================================================================================
//==============================================================================================

static void setup_actors( PMSIV_SUB* wk, ARCHANDLE* p_handle )
{
	NNSG2dImagePaletteProxy	palProxy;
	NNSG2dImageProxy		imgProxy;

	NNS_G2dInitImagePaletteProxy( &palProxy );
	NNS_G2dInitImageProxy( &imgProxy );
	ArcUtil_HDL_PalSysLoad( p_handle, NARC_pmsi_obj_sub_nclr, NNS_G2D_VRAM_TYPE_2DSUB, 0, HEAPID_PMS_INPUT_VIEW, &palProxy );
	ArcUtil_HDL_CharSysLoad( p_handle, NARC_pmsi_obj_sub_lz_ncgr, TRUE, CHAR_MAP_1D, 0, NNS_G2D_VRAM_TYPE_2DSUB, 0, 
							HEAPID_PMS_INPUT_VIEW, &imgProxy );

	load_clpack( &(wk->clpack), p_handle, NARC_pmsi_obj_sub_lz_ncer, NARC_pmsi_obj_sub_lz_nanr );

	wk->up_button = add_actor(wk, &imgProxy, &palProxy, &wk->clpack, BUTTON_UP_XPOS, BUTTON_UP_YPOS, 0, 0);
	CLACT_AnmChg( wk->up_button, ACTANM_ARROW_UP_STOP );
	CLACT_SetDrawFlag( wk->up_button, FALSE );

	wk->down_button = add_actor(wk, &imgProxy, &palProxy, &wk->clpack, BUTTON_DOWN_XPOS, BUTTON_DOWN_YPOS, 0, 0);
	CLACT_AnmChg( wk->down_button, ACTANM_ARROW_DOWN_STOP );
	CLACT_SetDrawFlag( wk->down_button, FALSE );

}

static void cleanup_actors( PMSIV_SUB* wk )
{
	if( wk->up_button )
	{
		CLACT_Delete( wk->up_button );
	}
	if( wk->down_button )
	{
		CLACT_Delete( wk->down_button );
	}

	unload_clpack(&wk->clpack);
}




static void load_clpack( CELL_ANIM_PACK* clpack, ARCHANDLE* p_handle, u32 cellDatID, u32 animDatID )
{
	clpack->cellLoadPtr = ArcUtil_HDL_CellBankDataGet(p_handle, cellDatID, TRUE, &(clpack->cellData), HEAPID_PMS_INPUT_VIEW);
	clpack->animLoadPtr = ArcUtil_HDL_AnimBankDataGet(p_handle, animDatID, TRUE, &(clpack->animData), HEAPID_PMS_INPUT_VIEW);
}
static void unload_clpack( CELL_ANIM_PACK* clpack )
{
	sys_FreeMemoryEz( clpack->cellLoadPtr );
	sys_FreeMemoryEz( clpack->animLoadPtr );
}


 static CLACT_WORK_PTR add_actor(
	PMSIV_SUB* wk, NNSG2dImageProxy* imgProxy, NNSG2dImagePaletteProxy* palProxy,
	CELL_ANIM_PACK* clpack, int xpos, int ypos, int bgpri, int actpri )
{
	CLACT_HEADER  header;
	CLACT_ADD_SIMPLE  add;
	CLACT_WORK_PTR   act;

	header.pImageProxy = imgProxy;
	header.pPaletteProxy = palProxy;
	header.pCellBank     = clpack->cellData;
	header.pAnimBank     = clpack->animData;
	header.priority      = bgpri;
	header.pCharData = NULL;
	header.pMCBank = NULL;
	header.pMCABank = NULL;
	header.flag = FALSE;

	add.ClActSet = wk->actsys;
	add.ClActHeader = &header;
	add.mat.x = xpos * FX32_ONE;
	add.mat.y = (ypos+192) * FX32_ONE;
	add.mat.z = 0;
	add.pri = actpri;
	add.DrawArea = NNS_G2D_VRAM_TYPE_2DSUB;
	add.heap = HEAPID_PMS_INPUT_VIEW;

	act = CLACT_AddSimple(&add);
	if( act )
	{
		CLACT_SetAnmFlag( act, TRUE );
		CLACT_SetAnmFrame( act, PMSI_ANM_SPEED );
	}
	return act;
}

//==============================================================================================
//==============================================================================================

typedef struct {
	PMSIV_SUB*  wk;
	int seq;
	u16 timer;

	u16 on_scrnID;
	u16 on_scrnID_end;
	u16 on_scrn_x;
	u16 on_scrn_y;

	u16 off_scrnID;
	u16 off_scrn_x;
	u16 off_scrn_y;

}CHANGE_BUTTON_WORK;

//------------------------------------------------------------------
/**
 * 
 *
 * @param   wk		
 *
 */
//------------------------------------------------------------------
void PMSIV_SUB_ChangeCategoryButton( PMSIV_SUB* wk )
{
	CHANGE_BUTTON_WORK* twk = sys_AllocMemory(HEAPID_PMS_INPUT_VIEW, sizeof(CHANGE_BUTTON_WORK));

	if( twk )
	{
		static const struct {
			u16 on_id_start;
			u16 on_id_end;
			u16 on_x;
			u16 on_y;
			u16 off_id;
			u16 off_x;
			u16 off_y;
		}TaskParam[] = {
			{
				SCRN_ID_GROUP_PUSH1, SCRN_ID_GROUP_ON, BUTTON_GROUP_SCRN_XPOS, BUTTON_GROUP_SCRN_YPOS,
				SCRN_ID_INITIAL_OFF, BUTTON_INITIAL_SCRN_XPOS, BUTTON_INITIAL_SCRN_YPOS
			},{
				SCRN_ID_INITIAL_PUSH1, SCRN_ID_INITIAL_ON, BUTTON_INITIAL_SCRN_XPOS, BUTTON_INITIAL_SCRN_YPOS,
				SCRN_ID_GROUP_OFF, BUTTON_GROUP_SCRN_XPOS, BUTTON_GROUP_SCRN_YPOS
			}
		};

		int mode = PMSI_GetCategoryMode( wk->mwk );

		twk->wk = wk;
		twk->seq = 0;
		twk->timer = 0;

		twk->on_scrnID		= TaskParam[mode].on_id_start;
		twk->on_scrnID_end	= TaskParam[mode].on_id_end;
		twk->on_scrn_x		= TaskParam[mode].on_x;
		twk->on_scrn_y		= TaskParam[mode].on_y;
		twk->off_scrnID		= TaskParam[mode].off_id;
		twk->off_scrn_x		= TaskParam[mode].off_x;
		twk->off_scrn_y		= TaskParam[mode].off_y;

		wk->changeButtonTask = TCB_Add( ChangeButtonTask, twk, TASKPRI_VIEW_COMMAND );

	}
	else
	{
		wk->changeButtonTask = NULL;
	}
}

BOOL PMSIV_SUB_WaitChangeCategoryButton( PMSIV_SUB* wk )
{
	return (wk->changeButtonTask == NULL);
}


static void ChangeButtonTask( TCB_PTR tcb, void* wk_adrs )
{
	enum {
		ANM_WAIT1 = 2,
		ANM_WAIT2 = 4,
	};

	CHANGE_BUTTON_WORK* twk = wk_adrs;

	switch( twk->seq ){
	case 0:
		GF_BGL_ScrWriteExpand(twk->wk->bgl, FRM_SUB_BG, twk->on_scrn_x, twk->on_scrn_y,
						MODEBUTTON_SCRN_WIDTH, MODEBUTTON_SCRN_HEIGHT, twk->wk->button_scrn[twk->on_scrnID],
						0, 0, MODEBUTTON_SCRN_WIDTH, MODEBUTTON_SCRN_HEIGHT );

		GF_BGL_ScrWriteExpand(twk->wk->bgl, FRM_SUB_BG, twk->off_scrn_x, twk->off_scrn_y,
						MODEBUTTON_SCRN_WIDTH, MODEBUTTON_SCRN_HEIGHT, twk->wk->button_scrn[twk->off_scrnID],
						0, 0, MODEBUTTON_SCRN_WIDTH, MODEBUTTON_SCRN_HEIGHT );

		GF_BGL_LoadScreenReq( twk->wk->bgl, FRM_SUB_BG );
		twk->on_scrnID++;
		twk->seq++;
		break;

	case 1:
		if( ++(twk->timer) >= ANM_WAIT1 )
		{
			GF_BGL_ScrWriteExpand(twk->wk->bgl, FRM_SUB_BG, twk->on_scrn_x, twk->on_scrn_y,
						MODEBUTTON_SCRN_WIDTH, MODEBUTTON_SCRN_HEIGHT, twk->wk->button_scrn[twk->on_scrnID],
						0, 0, MODEBUTTON_SCRN_WIDTH, MODEBUTTON_SCRN_HEIGHT );

			GF_BGL_LoadScreenReq( twk->wk->bgl, FRM_SUB_BG );
			twk->timer = 0;
			twk->on_scrnID++;
			twk->seq++;
		}
		break;

	case 2:
		if( ++(twk->timer) >= ANM_WAIT2 )
		{
			GF_BGL_ScrWriteExpand(twk->wk->bgl, FRM_SUB_BG, twk->on_scrn_x, twk->on_scrn_y,
						MODEBUTTON_SCRN_WIDTH, MODEBUTTON_SCRN_HEIGHT, twk->wk->button_scrn[twk->on_scrnID],
						0, 0, MODEBUTTON_SCRN_WIDTH, MODEBUTTON_SCRN_HEIGHT );

			GF_BGL_LoadScreenReq( twk->wk->bgl, FRM_SUB_BG );
			twk->seq++;
		}
		break;

	case 3:
		twk->wk->changeButtonTask = NULL;
		sys_FreeMemoryEz(wk_adrs);
		TCB_Delete(tcb);
	}
}




//------------------------------------------------------------------
/**
 * 矢印ボタン描画オン／オフ
 *
 * @param   wk		
 * @param   flag		
 *
 */
//------------------------------------------------------------------
void PMSIV_SUB_VisibleArrowButton( PMSIV_SUB* wk, BOOL flag )
{
	if( flag )
	{
		CLACT_SetDrawFlag( wk->up_button, PMSI_GetWordWinUpArrowVisibleFlag(wk->mwk) );
		CLACT_SetDrawFlag( wk->down_button, PMSI_GetWordWinDownArrowVisibleFlag(wk->mwk) );
	}
	else
	{
		CLACT_SetDrawFlag( wk->up_button, FALSE );
		CLACT_SetDrawFlag( wk->down_button, FALSE );
	}
}

//------------------------------------------------------------------
/**
 * 矢印ボタン状態切り替え
 *
 * @param   wk		
 * @param   pos		
 * @param   state		
 *
 */
//------------------------------------------------------------------
void PMSIV_SUB_ChangeArrowButton( PMSIV_SUB* wk, int pos, int state )
{
	switch(pos){
	case SUB_BUTTON_UP:
		switch( state ){
		case SUB_BUTTON_STATE_HOLD:
			CLACT_AnmChg( wk->up_button, ACTANM_ARROW_UP_HOLD );
			break;
		case SUB_BUTTON_STATE_RELEASE:
			CLACT_AnmChg( wk->up_button, ACTANM_ARROW_UP_STOP );
			break;
		}
		break;

	case SUB_BUTTON_DOWN:
		switch( state ){
		case SUB_BUTTON_STATE_HOLD:
			CLACT_AnmChg( wk->down_button, ACTANM_ARROW_DOWN_HOLD );
			break;
		case SUB_BUTTON_STATE_RELEASE:
			CLACT_AnmChg( wk->down_button, ACTANM_ARROW_DOWN_STOP );
			break;
		}
		break;
	}
}


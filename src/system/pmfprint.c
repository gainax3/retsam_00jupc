//=============================================================================================
/**
 * @file	pmfprint.c
 * @brief	•¶š•\¦ŠÖ”
 * @author	tetsu
 * @date	2003.04.08
 *
 * @data	2004.11.04 AGB -> NDS
 * @author	Hiroyuki Nakamura
 */
//=============================================================================================
#include "common.h"
#include "fntsys.h"

#define __PMFPRINT_H_GLOBAL__
#include	"system/pmfprint.h"

#include	"system/snd_tool.h"
#include	"system/fontproc.h"
#include "../field/fld_bmp.h"




#define	CURSOR_WAIT_TIME		(8)

#define WRITE_SKIP_CURSOR_W		( 8)
#define WRITE_SKIP_CURSOR_H		(16)


extern void	BmpPrintMsg(MSG_DATA_HEADER* mdh_p);


#include "skip_cur.dat"			//‘—‚èƒJ[ƒ/ƒ‹ƒf[ƒ^

static MSG_PRINT_MODE msg_print_flag;


#define	AFF_FONT_POS_CHG		// Šg‘åƒtƒHƒ“ƒg‚ÌˆÊ’u•ÏXiƒRƒƒ“ƒgƒAƒEƒg‚Åã‘µ‚¦j

#ifdef	AFF_FONT_POS_CHG
//#define	AFF_FONT_CENTER		// Šg‘åƒtƒHƒ“ƒg‚ğ’†‘µ‚¦‚Å•\¦iƒRƒƒ“ƒgƒAƒEƒg‚Å‰º‘µ‚¦j
#endif




//=============================================================================================
#define	PRINTSEQ_WRITE			(0)
#define	PRINTSEQ_TRGWAIT		(1)
#define	PRINTSEQ_TRGWAIT_CLEAR	(2)
#define	PRINTSEQ_TRGWAIT_SCROLL	(3)
#define	PRINTSEQ_SCROLL			(4)
#define	PRINTSEQ_SE_WAIT		(5)
#define	PRINTSEQ_FORCEWAIT		(6)

#define	SCR_SPEED_FAST		(4)
#define	SCR_SPEED_NORMAL	(2)
#define	SCR_SPEED_SLOW		(1)
static	const u8 scr_speed_value[] = {
	SCR_SPEED_SLOW,
	SCR_SPEED_NORMAL,
	SCR_SPEED_FAST,
};

// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ALL) imatake 2006/12/06
// ‚ ‚ç‚©‚¶‚ß“o˜^‚µ‚Ä‚¨‚¢‚½F‚ÉŒã‚Å•ÏX‚·‚éd‘g‚İ‚ğÀ‘•
// localize_spec_mark(LANG_ALL) imatake 2006/12/11
// F‚Ì“o˜^‚Ìd‘g‚İ‚ğ•ÏX
#define COLORSTACK_COLORNUM			(7)
#define COLORSTACK_COLOROFFSET		(100)
#define COLORSTACK_COLORMAX			(COLORSTACK_COLOROFFSET + COLORSTACK_COLORNUM)
#define COLORSTACK_SWAP				(255)
#define COLORSTACK_ISVALID(c)		(c >= COLORSTACK_COLOROFFSET && c < COLORSTACK_COLORMAX)
// ----------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------
/*
 *	•¶šƒf[ƒ^æ“¾
 *
 * @param	ini			BGLƒf[ƒ^
 * @param	mdh_p		ƒƒbƒZ[ƒWƒf[ƒ^\‘¢‘Ìƒ|ƒCƒ“ƒ^
 *
 * @retval	w_flag		•¶šæ“¾ƒpƒ‰ƒ[ƒ^
 *
 */
//---------------------------------------------------------------------------------------------
PRINT_RESULT PokeFontPrint( MSG_DATA_HEADER * mdh_p )
{
	const MSG_FONT_DATA* font_bmp;
	MSG_PRINT_USER*	mpu_p;
	int msg_speed, scr_sp;
	u16	fcode;

//	mdh_p->mph->bmpwin->ini

	mpu_p = (MSG_PRINT_USER *)&(mdh_p->userwork[0]);

	switch(mdh_p->seq){

	case PRINTSEQ_WRITE:		//’Êíˆ—

		//‰Ÿ‚µ‚Á‚Ï‚È‚µ‚Ìê‡‚Ì”»’è(Å‰‚ÌƒgƒŠƒK[‚ªŒŸo‚³‚ê‚Ä‚¢‚ê‚Î—LŒø)
		if( ( (sys.cont & (PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL)) && (mpu_p->skip_f) ) ||
			( (sys.tp_cont) && (msg_print_flag.tp_f) ) ){
			mdh_p->waitwork = 0;
			if( mdh_p->wait != MSG_ALLPUT ){
				msg_print_flag.btl_skip_f = 1;
			}
		}
		if((mdh_p->waitwork)&&(mdh_p->wait)){
			(mdh_p->waitwork)--;

			if(msg_print_flag.skip_f){//ƒXƒLƒbƒv‹@”\‚ª—LŒø‚Ìê‡‚Ìˆ—
				//Å‰‚ÌƒgƒŠƒK[‚ğŒŸo
				if( ( sys.trg & (PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL) ) ||
					( (sys.tp_trg) && (msg_print_flag.tp_f) ) ){
					mpu_p->skip_f = 1;
					mdh_p->waitwork = 0;
				}
			}
			return PRINT_RESULT_COMMAND;
		}

/*		Œ‹‹ÇAƒXƒLƒbƒvƒtƒ‰ƒO‚ğ—§‚Ä‚é‚Æƒ{ƒ^ƒ“‚Å‘—‚ç‚ê‚é‚Ì‚ÅAƒEƒFƒCƒg‚ÍƒvƒŠƒ“ƒg‘¬“x‚ÉˆË‘¶
		if(msg_print_flag.auto_f){
			mdh_p->waitwork = AUTO_MSGWAIT;		//ƒI[ƒg’†‚ÍƒEƒFƒCƒgˆê’è
		}else{
			mdh_p->waitwork = mdh_p->wait;
		}
*/
		mdh_p->waitwork = mdh_p->wait;


		//ƒtƒHƒ“ƒgƒf[ƒ^æ“¾
		fcode = *(u16*)(mdh_p->mph.msg);
		mdh_p->mph.msg++; 

		// ----------------------------------------------------------------------------
		// localize_spec_mark(LANG_ALL) imatake 2006/12/07
		// ˆ³k‚³‚ê‚½•¶š—ñ‚ğ‚»‚Ì‚Ü‚Ü•\¦‚µ‚æ‚¤‚Æ‚µ‚Ä‚¢‚½‚çƒXƒgƒbƒv
		GF_ASSERT_MSG(fcode != COMPRESSED_MARK, "ˆ³k•¶š—ñ‚ğ‚»‚Ì‚Ü‚Ü•\¦‚µ‚æ‚¤‚Æ‚µ‚Ü‚µ‚½B\n");
		// ----------------------------------------------------------------------------

		switch(fcode){
		/* I—¹ƒR[ƒh */
		case EOM_:	
			return PRINT_RESULT_END;

		/* ‰üsƒR[ƒh */
		case CR_:	
			mdh_p->mph.write_x = mdh_p->mph.start_x;
			mdh_p->mph.write_y +=( FontHeaderGet( mdh_p->mph.fnt_index, FONT_HEADER_SIZE_Y ) + mdh_p->mph.space_y ); 
			return PRINT_RESULT_LOOP;

		/* w’èINDEXƒoƒbƒtƒ@‚ÌƒƒbƒZ[ƒW“WŠJ	*/
		case I_MSG_:	
			//–¢ì¬(DATA = 1byte)
			mdh_p->mph.msg++; 
			return PRINT_RESULT_LOOP;


		/* ƒtƒHƒ“ƒg§ŒäƒRƒ}ƒ“ƒh */
		case _CTRL_TAG:
			// ƒ^ƒOƒR[ƒhoŒ»ˆÊ’u‚Ü‚Åƒ|ƒCƒ“ƒ^‚ğ–ß‚·
			mdh_p->mph.msg--;

			fcode = STRCODE_GetTagType( mdh_p->mph.msg );

			switch( fcode ){
			case NC_FONT_COL_:				// ƒtƒHƒ“ƒg§ŒäƒR[ƒh
				{
					u16	 param = STRCODE_GetTagParam( mdh_p->mph.msg, 0 );
					// ----------------------------------------------------------------------------
					// localize_spec_mark(LANG_ALL) imatake 2006/12/06
					// ‚ ‚ç‚©‚¶‚ß“o˜^‚µ‚Ä‚¨‚¢‚½F‚ÉŒã‚Å•ÏX‚·‚éd‘g‚İ‚ğÀ‘•
					// localize_spec_mark(LANG_ALL) imatake 2006/12/11
					// F‚Ì“o˜^‚Ìd‘g‚İ‚ğ•ÏX
					if (param == COLORSTACK_SWAP) {
						u8 color_stack = mdh_p->mph.color_stack;
						mdh_p->mph.color_stack = (mdh_p->mph.f_col - 1) / 2 + COLORSTACK_COLOROFFSET;
						if (!COLORSTACK_ISVALID(color_stack)) break;	// ³‚µ‚¢’l‚ª•Û‘¶‚³‚ê‚Ä‚¢‚È‚¯‚ê‚ÎŒ»óˆÛ
						param = color_stack - COLORSTACK_COLOROFFSET;
					} else if (param >= COLORSTACK_COLOROFFSET) {
						mdh_p->mph.color_stack = param;
						break;
					}
					// ----------------------------------------------------------------------------
					mdh_p->mph.f_col = 1 + (param*2);
					mdh_p->mph.s_col = 1 + (param*2) + 1;
					FntDataColorSet(mdh_p->mph.f_col, mdh_p->mph.b_col, mdh_p->mph.s_col);
				}
				break;

			case NC_NOTE_ICON:		// ‚±‚±‚É’–ÚƒAƒCƒRƒ“ˆ—‚ğ‹Lq‚·‚é
				{
					u16	 param = STRCODE_GetTagParam( mdh_p->mph.msg, 0 );
					GF_MSG_PrintDispIcon( mdh_p, mdh_p->mph.write_x, mdh_p->mph.write_y, param );
				}
				if( mdh_p->wait_flg != 0 ){
					GF_BGL_BmpWinOn( mdh_p->mph.bmpwin );
				}
				break;

			case NC_FORCE_WAIT:
				{
					mdh_p->waitwork = STRCODE_GetTagParam( mdh_p->mph.msg, 0 );
					mdh_p->mph.msg = STRCODE_SkipTag( mdh_p->mph.msg );
					mdh_p->seq = PRINTSEQ_FORCEWAIT;
					return PRINT_RESULT_COMMAND;
				}
				break;

			case NC_CALLBACK_ARG:
				mdh_p->callback_arg = STRCODE_GetTagParam( mdh_p->mph.msg, 0 );
				mdh_p->mph.msg = STRCODE_SkipTag( mdh_p->mph.msg );
				return PRINT_RESULT_COMMAND;

			case NC_WRITEPOS_CHANGE_X:
				mdh_p->mph.write_x = STRCODE_GetTagParam( mdh_p->mph.msg, 0 );
				break;

			case NC_WRITEPOS_CHANGE_Y:
				mdh_p->mph.write_y = STRCODE_GetTagParam( mdh_p->mph.msg, 0 );
				break;

			case NC_FONT_SIZE_:
				{
					u16 param = STRCODE_GetTagParam( mdh_p->mph.msg, 0 );

					switch( param ) {
					case NC_FAFF_100_:		// ƒTƒCƒYF100“
						mdh_p->mph.dot_tbl = 0;
						mdh_p->mph.dot_wy = 0;
						break;
					case NC_FAFF_200_:		// ƒTƒCƒYF200“
						mdh_p->mph.dot_tbl = 0xfffc;
						mdh_p->mph.dot_wy = 0;
						break;
					}
				}
				break;

			case NC_MOVE_:				// “®ì§ŒäƒR[ƒh
				{
					u16	 c_move = STRCODE_GetTagParam( mdh_p->mph.msg, 0 );

					switch( c_move ){
					case NC_MV_CLEAR_:		// ƒL[‘Ò‚¿ƒNƒŠƒA
						mdh_p->seq	= PRINTSEQ_TRGWAIT_CLEAR;
						PokeFontTriggerCursorAnimeInit(mdh_p);
						mdh_p->mph.msg = STRCODE_SkipTag( mdh_p->mph.msg );
						return PRINT_RESULT_COMMAND;
					case NC_MV_SCROLL_:		// ƒL[‘Ò‚¿ƒXƒNƒ[ƒ‹
						mdh_p->seq	= PRINTSEQ_TRGWAIT_SCROLL;
						PokeFontTriggerCursorAnimeInit(mdh_p);
						mdh_p->mph.msg = STRCODE_SkipTag( mdh_p->mph.msg );
						return PRINT_RESULT_COMMAND;
					}
				}
				break;
			}

			mdh_p->mph.msg = STRCODE_SkipTag( mdh_p->mph.msg );
			return PRINT_RESULT_LOOP;

		/* "¥"		ƒgƒŠƒK[‘Ò‚¿¨MSGƒGƒŠƒAƒNƒŠƒA({•`‰æŠJnƒŠƒZƒbƒg) */
		case NORMAL_WAIT_:
			mdh_p->seq	= PRINTSEQ_TRGWAIT_CLEAR;
			PokeFontTriggerCursorAnimeInit(mdh_p);
			return PRINT_RESULT_COMMAND;
	
		/* "¤"		ƒgƒŠƒK[‘Ò‚¿¨ˆêsƒXƒNƒ[ƒ‹({‰üs) */
		case SCROLL_WAIT_:
			mdh_p->seq	= PRINTSEQ_TRGWAIT_SCROLL;
			PokeFontTriggerCursorAnimeInit(mdh_p);
			return PRINT_RESULT_COMMAND;

		}
		font_bmp = FontDataGet( mpu_p->fmode, fcode );

		//ŠeíƒtƒHƒ“ƒg•`‰æ
		GF_BGL_BmpWinPrintMsgWide(
			mdh_p->mph.bmpwin, font_bmp->data,
			font_bmp->size_x, font_bmp->size_y,
			mdh_p->mph.write_x,mdh_p->mph.write_y, mdh_p->mph.dot_tbl );

		//Ÿ‚Ì•¶š‚Ì•`‰æˆÊ’u‚ğİ’è
		mdh_p->mph.write_x += (font_bmp->size_x + mdh_p->mph.space_x);
		return PRINT_RESULT_WRITE;

	case PRINTSEQ_TRGWAIT:		//ƒgƒŠƒK[‘Ò‚¿
		if(PokeFontTriggerWait2(mdh_p)){
			PokeFontTriggerCursorAnimeExit2(mdh_p);
			mdh_p->seq	= PRINTSEQ_WRITE;
		}
		return PRINT_RESULT_COMMAND;

	case PRINTSEQ_TRGWAIT_CLEAR:		//ƒgƒŠƒK[‘Ò‚¿¨MSGƒGƒŠƒAƒNƒŠƒA
		if(PokeFontTriggerWait(mdh_p)){
			//ƒGƒŠƒAƒNƒŠƒA
			PokeFontTriggerCursorAnimeExit2(mdh_p);	//‰º‚Å“¯‚ÉƒNƒŠƒA
			GF_BGL_BmpWinDataFill( mdh_p->mph.bmpwin, mdh_p->mph.b_col );
			mdh_p->mph.write_x = mdh_p->mph.start_x;
			mdh_p->mph.write_y = mdh_p->mph.start_y;
			mdh_p->seq	= PRINTSEQ_WRITE;
		}
		return PRINT_RESULT_COMMAND;

	case PRINTSEQ_TRGWAIT_SCROLL:		//ƒgƒŠƒK[‘Ò‚¿¨ƒXƒNƒ[ƒ‹
		if(PokeFontTriggerWait( mdh_p )){
			PokeFontTriggerCursorAnimeExit2(mdh_p);
			//ƒXƒNƒ[ƒ‹•İ’è(Y‚Ì‚‚³•ª + •\¦•)
			mdh_p->scrwork	=( FontHeaderGet( mdh_p->mph.fnt_index, FONT_HEADER_SIZE_Y ) + mdh_p->mph.space_y ); 
			mdh_p->mph.write_x = mdh_p->mph.start_x;
			mdh_p->seq	= PRINTSEQ_SCROLL;
		}
		return PRINT_RESULT_COMMAND;

	case PRINTSEQ_SCROLL:				//ƒXƒNƒ[ƒ‹ˆ—
		if( mdh_p->scrwork )
		{
			scr_sp = scr_speed_value[2];

			if(mdh_p->scrwork < scr_sp)
			{
				GF_BGL_BmpWinShift(
					mdh_p->mph.bmpwin, GF_BGL_BMPWIN_SHIFT_U,
					mdh_p->scrwork, (mdh_p->mph.b_col<<4)|mdh_p->mph.b_col );
				mdh_p->scrwork = 0;
			}
			else
			{
				GF_BGL_BmpWinShift(
					mdh_p->mph.bmpwin, GF_BGL_BMPWIN_SHIFT_U,
					scr_sp, (mdh_p->mph.b_col<<4)|mdh_p->mph.b_col );
				mdh_p->scrwork -= scr_sp;
			}

			GF_BGL_BmpWinOn( mdh_p->mph.bmpwin );

		}
		else
		{
			mdh_p->seq	= PRINTSEQ_WRITE;
		}
		return PRINT_RESULT_COMMAND;

	case PRINTSEQ_SE_WAIT:			//SEƒEƒFƒCƒg
		mdh_p->seq	= PRINTSEQ_WRITE;
		return PRINT_RESULT_COMMAND;

	case PRINTSEQ_FORCEWAIT:		//‹­§ƒEƒFƒCƒg
		if( mdh_p->waitwork )
		{
			mdh_p->waitwork--;
		}
		else
		{
			mdh_p->seq	= PRINTSEQ_WRITE;
		}
		return PRINT_RESULT_COMMAND;
	}
	return PRINT_RESULT_END;
}

//=============================================================================================
/**
 *		ƒgƒŠƒK[‘Ò‚¿ƒJ[ƒ\ƒ‹•\¦
 */
//=============================================================================================
static u16 TrgCursorCgxPos = 0;		// •\¦‚·‚éƒEƒBƒ“ƒhƒE˜g‚ÌƒLƒƒƒ‰ˆÊ’u

//--------------------------------------------------------------------------------------------
/**
 * ƒgƒŠƒK[‘Ò‚¿ƒJ[ƒ\ƒ‹‚ğ•\¦‚·‚éƒEƒBƒ“ƒhƒE˜g‚ÌƒLƒƒƒ‰ˆÊ’u‚ğ•Û‘¶
 *
 * @param	cgx		ƒEƒBƒ“ƒhƒE˜g‚ÌƒLƒƒƒ‰ˆÊ’u
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PokeFontTrgCursorCgxSet( u16 cgx )
{
	TrgCursorCgxPos = cgx;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒgƒŠƒK[‘Ò‚¿ƒJ[ƒ\ƒ‹ƒAƒjƒ‰Šú‰»
 *
 * @param	mdh_p	ƒƒbƒZ[ƒWƒf[ƒ^ƒwƒbƒ_
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void		PokeFontTriggerCursorAnimeInit(MSG_DATA_HEADER* mdh_p)
{
	MSG_PRINT_USER*	mpu_p;

	mpu_p = (MSG_PRINT_USER*)&(mdh_p->userwork[0]);

	if(msg_print_flag.auto_f){
		mpu_p->rep_wait = 0;	//ƒI[ƒg’†ƒEƒFƒCƒgƒ[ƒN‰Šú‰»
	}else{
		mpu_p->cur_anm  = 0;
		mpu_p->cur_wait = 0;
	}
}

static const u8 CursorAnmData[] = {0,1,2,1,};

//--------------------------------------------------------------------------------------------
/**
 * ƒgƒŠƒK[‘Ò‚¿ƒJ[ƒ\ƒ‹ƒAƒjƒ
 *
 * @param	mdh_p	ƒƒbƒZ[ƒWƒf[ƒ^ƒwƒbƒ_
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PokeFontTriggerCursorAnime2( MSG_DATA_HEADER * mdh_p )
{
	MSG_PRINT_USER * mpu_p;
	void * cgx;

	mpu_p = (MSG_PRINT_USER *)&(mdh_p->userwork[0]);

	if(msg_print_flag.auto_f){
		return;
	}
	if(mpu_p->cur_wait){
		mpu_p->cur_wait--;
		return;
	}

	{
		u16	cgx;
		u8	frm, px, py, sx;

		frm = GF_BGL_BmpWinGet_Frame( mdh_p->mph.bmpwin );
		px  = GF_BGL_BmpWinGet_PosX( mdh_p->mph.bmpwin );
		py  = GF_BGL_BmpWinGet_PosY( mdh_p->mph.bmpwin );
		sx  = GF_BGL_BmpWinGet_SizeX( mdh_p->mph.bmpwin );
		cgx = TrgCursorCgxPos;

		GF_BGL_ScrFill(
			mdh_p->mph.bmpwin->ini, frm, cgx+18+(CursorAnmData[mpu_p->cur_anm]*4),
			px+sx+1, py+2, 1, 1, GF_BGL_SCRWRT_PALNL );
		GF_BGL_ScrFill(
			mdh_p->mph.bmpwin->ini, frm, cgx+19+(CursorAnmData[mpu_p->cur_anm]*4),
			px+sx+2, py+2, 1, 1, GF_BGL_SCRWRT_PALNL );
		GF_BGL_ScrFill(
			mdh_p->mph.bmpwin->ini, frm, cgx+20+(CursorAnmData[mpu_p->cur_anm]*4),
			px+sx+1, py+3, 1, 1, GF_BGL_SCRWRT_PALNL );
		GF_BGL_ScrFill(
			mdh_p->mph.bmpwin->ini, frm, cgx+21+(CursorAnmData[mpu_p->cur_anm]*4),
			px+sx+2, py+3, 1, 1, GF_BGL_SCRWRT_PALNL );
		GF_BGL_LoadScreenReq( mdh_p->mph.bmpwin->ini, frm );

		mpu_p->cur_wait = CURSOR_WAIT_TIME;
		mpu_p->cur_anm++;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * ƒgƒŠƒK[‘Ò‚¿ƒJ[ƒ\ƒ‹ƒAƒjƒI—¹ˆ—
 *
 * @param	mdh_p	ƒƒbƒZ[ƒWƒf[ƒ^ƒwƒbƒ_
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PokeFontTriggerCursorAnimeExit2( MSG_DATA_HEADER* mdh_p )
{
	u16	cgx;
	u8	frm, px, py, sx;

	frm = GF_BGL_BmpWinGet_Frame( mdh_p->mph.bmpwin );
	px  = GF_BGL_BmpWinGet_PosX( mdh_p->mph.bmpwin );
	py  = GF_BGL_BmpWinGet_PosY( mdh_p->mph.bmpwin );
	sx  = GF_BGL_BmpWinGet_SizeX( mdh_p->mph.bmpwin );
	cgx = TrgCursorCgxPos;

	GF_BGL_ScrFill(
		mdh_p->mph.bmpwin->ini, frm, cgx+10, px+sx+1, py+2, 1, 2, GF_BGL_SCRWRT_PALNL );
	GF_BGL_ScrFill(
		mdh_p->mph.bmpwin->ini, frm, cgx+11, px+sx+2, py+2, 1, 2, GF_BGL_SCRWRT_PALNL );

	GF_BGL_LoadScreenReq( mdh_p->mph.bmpwin->ini, frm );
}


//============================================================================================
//	ƒgƒŠƒK[‘Ò‚¿
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * ƒL[“ü—ÍƒgƒŠƒK[‘Ò‚¿ŠÖ”
 *
 * @param	mdh_p	ƒƒbƒZ[ƒWƒf[ƒ^ƒwƒbƒ_
 *
 * @retval	"TRUE = Ÿ‚Ö"
 * @retval	"FALSE = wait"
 */
//--------------------------------------------------------------------------------------------
static BOOL PokeFontTriggerWaitCheck( MSG_DATA_HEADER * mdh_p )
{
	if( ( sys.trg & (PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL) ) ||
		( (sys.tp_trg) && (msg_print_flag.tp_f) ) ){
		Snd_SePlay( SE_MSG_SKIP );
		msg_print_flag.btl_wait_f = 1;
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒI[ƒg‘—‚èƒgƒŠƒK[‘Ò‚¿ŠÖ”
 *
 * @param	mdh_p	ƒƒbƒZ[ƒWƒf[ƒ^ƒwƒbƒ_
 *
 * @retval	"TRUE = Ÿ‚Ö"
 * @retval	"FALSE = wait"
 */
//--------------------------------------------------------------------------------------------
BOOL PokeFontTriggerWaitReplayCheck(MSG_DATA_HEADER* mdh_p)
{
	MSG_PRINT_USER*	mpu_p;
	u16	wait_len;

	mpu_p = (MSG_PRINT_USER*)&(mdh_p->userwork[0]);

	wait_len = AUTO_MSGTRGWAIT;

	if(mpu_p->rep_wait == wait_len){	//ƒŠƒvƒŒƒC’†ƒEƒFƒCƒgˆê’è
		return TRUE;
	}
	mpu_p->rep_wait++;

	if( msg_print_flag.auto_f_skip ){
		return PokeFontTriggerWaitCheck( mdh_p );
	}

	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒgƒŠƒK[‘Ò‚¿ŠÖ” ( ¥‚ ‚è )
 *
 * @param	mdh_p	ƒƒbƒZ[ƒWƒf[ƒ^ƒwƒbƒ_
 *
 * @retval	"TRUE = Ÿ‚Ö"
 * @retval	"FALSE = wait"
 */
//--------------------------------------------------------------------------------------------
BOOL PokeFontTriggerWait( MSG_DATA_HEADER* mdh_p )
{
	BOOL flg = FALSE;

	if(msg_print_flag.auto_f){
		//ƒI[ƒg‘—‚èƒEƒFƒCƒgƒ`ƒFƒbƒN
		flg = PokeFontTriggerWaitReplayCheck(mdh_p);
	}else{
		PokeFontTriggerCursorAnime2(mdh_p);
		flg = PokeFontTriggerWaitCheck( mdh_p );
	}
	return flg;
}

//--------------------------------------------------------------------------------------------
/**
 * ƒgƒŠƒK[‘Ò‚¿ŠÖ” ( ¥‚È‚µ )
 *
 * @param	mdh_p	ƒƒbƒZ[ƒWƒf[ƒ^ƒwƒbƒ_
 *
 * @retval	"TRUE = Ÿ‚Ö"
 * @retval	"FALSE = wait"
 */
//--------------------------------------------------------------------------------------------
BOOL PokeFontTriggerWait2(MSG_DATA_HEADER* mdh_p)
{
	u8	flg = FALSE;

	if(msg_print_flag.auto_f){
		//ƒI[ƒg‘—‚èƒEƒFƒCƒgƒ`ƒFƒbƒN
		flg = PokeFontTriggerWaitReplayCheck(mdh_p);
	}else{
		flg = PokeFontTriggerWaitCheck( mdh_p );
	}
	return flg;
}



//=============================================================================================
//	•`‰æ§Œä
//=============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * ƒXƒLƒbƒvİ’è
 *
 * @param	flg		ƒtƒ‰ƒO
 *
 * @return	none
 *
 *	flg = MSG_SKIP_OFF : ƒXƒLƒbƒv–³Œø
 *	flg = MSG_SKIP_ON  : ƒXƒLƒbƒv—LŒø
 */
//--------------------------------------------------------------------------------------------
void MsgPrintSkipFlagSet( int flg )
{
	msg_print_flag.skip_f = flg;
}

//--------------------------------------------------------------------------------------------
/**
 * ©“®‘—‚èİ’è
 *
 * @param	flg		ƒtƒ‰ƒO
 *
 * @return	none
 *
 *	flg = MSG_AUTO_OFF     : ©“®‘—‚è–³Œø
 *	flg = MSG_AUTO_ON      : ©“®‘—‚è—LŒø
 *	flg = MSG_AOTO_SKIP_ON : ©“®‘—‚è—LŒø/ƒ{ƒ^ƒ“ƒXƒLƒbƒv—LŒø
 */
//--------------------------------------------------------------------------------------------
void MsgPrintAutoFlagSet( int flg )
{
	msg_print_flag.auto_f      = (flg&1);
	msg_print_flag.auto_f_skip = ((flg>>1)&1);
}

//--------------------------------------------------------------------------------------------
/**
 * ƒ^ƒbƒ`ƒpƒlƒ‹‘—‚èİ’è
 *
 * @param	flg		ƒtƒ‰ƒO
 *
 * @return	none
 *
 *	flg = MSG_TP_OFF : ƒ^ƒbƒ`ƒpƒlƒ‹–³Œø
 *	flg = MSG_TP_ON  : ƒ^ƒbƒ`ƒpƒlƒ‹—LŒø
 */
//--------------------------------------------------------------------------------------------
void MsgPrintTouchPanelFlagSet( int flg )
{
	msg_print_flag.tp_f = flg;
}


//--------------------------------------------------------------------------------------------
/**
 * í“¬—pFƒXƒLƒbƒv’†‚©
 *
 * @param	none
 *
 * @retval	"1 = ‘—‚è’†"
 * @retval	"0 = ‚»‚êˆÈŠO"
 */
//--------------------------------------------------------------------------------------------
u8 MsgPrintBattleSkipPushFlagGet(void)
{
	return msg_print_flag.btl_skip_f;
}

//--------------------------------------------------------------------------------------------
/**
 * í“¬—pFƒXƒLƒbƒv’†‚©‚ğ”»’è‚·‚éƒtƒ‰ƒO‚ğƒNƒŠƒA
 *
 * @param	none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void MsgPrintBattleSkipPushFlagClear(void)
{
	msg_print_flag.btl_skip_f = 0;
}

//--------------------------------------------------------------------------------------------
/**
 * í“¬—pF¥‚ğ‘—‚Á‚½‚©
 *
 * @param	none
 *
 * @retval	"1 = ‘—‚è’†"
 * @retval	"0 = ‚»‚êˆÈŠO"
 */
//--------------------------------------------------------------------------------------------
u8 MsgPrintBattleWaitPushFlagGet(void)
{
	return msg_print_flag.btl_wait_f;
}

//--------------------------------------------------------------------------------------------
/**
 * í“¬—pF¥‚ğ‘—‚Á‚½‚©‚ğ”»’è‚·‚éƒtƒ‰ƒO‚ğƒNƒŠƒA
 *
 * @param	none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void MsgPrintBattleWaitPushFlagClear(void)
{
	msg_print_flag.btl_wait_f = 0;
}

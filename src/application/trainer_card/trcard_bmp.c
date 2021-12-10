//============================================================================================
/**
 * @file	trcard_bmp.c
 * @bfief	トレーナーカード画面BMP関連
 * @author	Nozomu Saito
 * @date	05.11.15
 */
//============================================================================================
#include "common.h"
#include "savedata/playtime.h"
#include "system/fontproc.h"
#include "system/pm_str.h"
#include "system/buflen.h"
// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ALL) imatake 2006/12/13
// 単位類を gmm のテキストに移行
#include "system/wordset.h"
// ----------------------------------------------------------------------------
#include "strbuf_family.h"

#include "trcard_bmp.h"
#include "trcard_cgx_def.h"

#include "msgdata/msg_trainerscard.h"
#include "msgdata/msg.naix"

#define FONT_BG	(GF_BGL_FRAME3_S)

#define HOUR_DISP_MAX	(999)


////////////////////////////////////////////////////////////////
//表面
////////////////////////////////////////////////////////////////
//トレーナーNO					:34
#define WIN_TR_NO_PX	(2)
#define WIN_TR_NO_PY	(4)
#define WIN_TR_NO_SX	(17)
#define WIN_TR_NO_SY	(2)
#define WIN_TR_NO_CGX	(TR_BG_BASE+TR_BG_SIZE)

//名前							:34
#define WIN_NAME_PX	(2)
#define WIN_NAME_PY	(6)
#define WIN_NAME_SX	(17)
#define WIN_NAME_SY	(2)
#define WIN_NAME_CGX	(WIN_TR_NO_CGX+WIN_TR_NO_SX*WIN_TR_NO_SY)

//おこづかい					:34
#define WIN_MONEY_PX	(2)
#define WIN_MONEY_PY	(9)
#define WIN_MONEY_SX	(17)
#define WIN_MONEY_SY	(2)
#define WIN_MONEY_CGX	(WIN_NAME_CGX+WIN_NAME_SX*WIN_NAME_SY)

//ずかん						:34
#define WIN_BOOK_PX	(2)
#define WIN_BOOK_PY	(12)
#define WIN_BOOK_SX	(17)
#define WIN_BOOK_SY	(2)
#define WIN_BOOK_CGX	(WIN_MONEY_CGX+WIN_MONEY_SX*WIN_MONEY_SY)

//スコア						:34
#define WIN_SCORE_PX	(2)
#define WIN_SCORE_PY	(15)
#define WIN_SCORE_SX	(17)
#define WIN_SCORE_SY	(2)
#define WIN_SCORE_CGX	(WIN_BOOK_CGX+WIN_BOOK_SX*WIN_BOOK_SY)

//プレイ時間					:56
#define WIN_TIME_PX	(2)
#define WIN_TIME_PY	(18)
#define WIN_TIME_SX	(28)
#define WIN_TIME_SY	(2)
#define WIN_TIME_CGX	(WIN_SCORE_CGX+WIN_SCORE_SX*WIN_SCORE_SY)

//スタート時間					:56
#define WIN_S_TIME_PX	(2)
#define WIN_S_TIME_PY	(20)
#define WIN_S_TIME_SX	(28)
#define WIN_S_TIME_SY	(2)
#define WIN_S_TIME_CGX	(WIN_TIME_CGX+WIN_TIME_SX*WIN_TIME_SY)
//////////////////////////////////////////////////////////////
//裏面
//////////////////////////////////////////////////////////////
//でんどういり					:112
#define WIN_CLEAR_PX	(2)
#define WIN_CLEAR_PY	(2)
#define WIN_CLEAR_SX	(28)
#define WIN_CLEAR_SY	(4)
#define WIN_CLEAR_CGX	(SIGN_CGX+TR_SIGN_SIZE)
//つうしん回数					:56
#define WIN_COMM_PX	(2)
#define WIN_COMM_PY	(7)
#define WIN_COMM_SX	(28)
#define WIN_COMM_SY	(2)
#define WIN_COMM_CGX	(WIN_CLEAR_CGX+WIN_CLEAR_SX*WIN_CLEAR_SY)

//通信対戦						:56
#define WIN_BATTLE_PX	(2)
#define WIN_BATTLE_PY	(9)
#define WIN_BATTLE_SX	(28)
#define WIN_BATTLE_SY	(2)
#define WIN_BATTLE_CGX	(WIN_COMM_CGX+WIN_COMM_SX*WIN_COMM_SY)

//通信交換						:56
#define WIN_TRADE_PX	(2)
#define WIN_TRADE_PY	(11)
#define WIN_TRADE_SX	(28)
#define WIN_TRADE_SY	(2)
#define WIN_TRADE_CGX	(WIN_BATTLE_CGX+WIN_BATTLE_SX*WIN_BATTLE_SY)

#define TR_STRING_LEN	(32)		//ローカライズを考え多めに。

#define TR_MSGCOLOR			(GF_PRINTCOLOR_MAKE( 1, 2, FBMP_COL_NULL ))
#define BMP_WIDTH_TYPE1	(8*17)
#define BMP_WIDTH_TYPE2	(8*28)

#define SEC_DISP_OFS	(2)		//適当。いい感じに見える値で。
// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ENGLISH) imatake 2006/12/13
// 海外版では円・ひきを削除
#define YEN_OFS			(0)		//「円」表示分右スペース
#define HIKI_OFS		(0)		//「ひき」表示分右スペース
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ENGLISH) imatake 2006/12/13
// 単位類を gmm のテキストに移行

#define TIME_OFS		(184)
#define TIME_WIDTH		(40)
#define COLON_OFS		(207 - 2)	// 幅5のスペースが用意できたらオフセットは取る
#define COLON_WIDTH		(5)

#define WIN_OFS		(104 + 8)
#define LOSE_OFS	(168 + 8)

// 数値等を入れる WORDSET のバッファID
enum {
	BUFID_HOUR,
	BUFID_MINUTE,
	BUFID_YEAR,
	BUFID_MONTH,
	BUFID_DAY,
	BUFID_MONEY,
	WORDMAX_ALL
};

#define WORDMAX_TIME	(BUFID_MINUTE + 1)
#define WORDMAX_DATE	(BUFID_DAY    + 1)

// ----------------------------------------------------------------------------

#define SEC_DISP_POS_X	(8*25)
#define MINITE_DISP_POS_X	(8*26)
#define HOUR_DISP_POS_X	(8*21)
#define MINITE_DISP_W	(8*2)
#define HOUR_DISP_W	(8*4)

static const BMPWIN_DAT TrCardBmpData[] =
{
	
	{	// 0:トレーナーNO
		FONT_BG, WIN_TR_NO_PX, WIN_TR_NO_PY,
		WIN_TR_NO_SX, WIN_TR_NO_SY, TR_FONT_PAL, WIN_TR_NO_CGX
	},	
	{	// 1:名前
		FONT_BG, WIN_NAME_PX, WIN_NAME_PY,
		WIN_NAME_SX, WIN_NAME_SY, TR_FONT_PAL, WIN_NAME_CGX
	},
	{	// 2:おこづかい
		FONT_BG, WIN_MONEY_PX, WIN_MONEY_PY,
		WIN_MONEY_SX, WIN_MONEY_SY, TR_FONT_PAL, WIN_MONEY_CGX
	},	
	{	// 3:ずかん
		FONT_BG, WIN_BOOK_PX, WIN_BOOK_PY,
		WIN_BOOK_SX, WIN_BOOK_SY, TR_FONT_PAL, WIN_BOOK_CGX
	},
	{	// 4:スコア
		FONT_BG, WIN_SCORE_PX, WIN_SCORE_PY,
		WIN_SCORE_SX, WIN_SCORE_SY, TR_FONT_PAL, WIN_SCORE_CGX
	},
	{	//　5:プレイ時間
		FONT_BG, WIN_TIME_PX, WIN_TIME_PY,
		WIN_TIME_SX, WIN_TIME_SY, TR_FONT_PAL, WIN_TIME_CGX
	},
	{	//　6:開始時間
		FONT_BG, WIN_S_TIME_PX, WIN_S_TIME_PY,
		WIN_S_TIME_SX, WIN_S_TIME_SY, TR_FONT_PAL, WIN_S_TIME_CGX
	},
	
	{	//　7:でんどういり
		FONT_BG, WIN_CLEAR_PX, WIN_CLEAR_PY,
		WIN_CLEAR_SX, WIN_CLEAR_SY, TR_FONT_PAL, WIN_CLEAR_CGX
	},

	{	//　8:通信回数
		FONT_BG, WIN_COMM_PX, WIN_COMM_PY,
		WIN_COMM_SX, WIN_COMM_SY, TR_FONT_PAL, WIN_COMM_CGX
	},
	{	//　9:通信対戦
		FONT_BG, WIN_BATTLE_PX, WIN_BATTLE_PY,
		WIN_BATTLE_SX, WIN_BATTLE_SY, TR_FONT_PAL, WIN_BATTLE_CGX
	},
	{	//　10:通信交換
		FONT_BG, WIN_TRADE_PX, WIN_TRADE_PY,
		WIN_TRADE_SX, WIN_TRADE_SY, TR_FONT_PAL, WIN_TRADE_CGX
	},
};

static void WriteNumData(	GF_BGL_BMPWIN *inWin,
							const u32 inBmpWidth,
							const u32 inRightSpace,
							const u32 inStartY,
							STRBUF *buff,
							const u32 inNum,
							const u8 inDigit,
							const NUMBER_DISPTYPE inDisptype);
static void WriteNumDataFill(	GF_BGL_BMPWIN *inWin,
								const u32 inBmpWidth,
								const u32 inRightSpace,
								const u32 inStartY,
								STRBUF *buff,
								const u32 inNum,
								const u8 inDigit,
								const NUMBER_DISPTYPE inDisptype,
								const u32 inFillSatrt,
								const u32 inFillWidth);
static void WriteStrData(	GF_BGL_BMPWIN *inWin,
							const u32 inBmpWidth,
							const u32 inRightSpace,
							const u32 inStartY,
							const STRBUF *buff);

//--------------------------------------------------------------------------------------------
/**
 * BMPウィンドウ追加
 *
 * @param	bgl		bgl
 * @param	win		BmpWin
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void TRCBmp_AddTrCardBmp( GF_BGL_INI * bgl, GF_BGL_BMPWIN	*win )
{
#if 1
	u8 i;
	const BMPWIN_DAT * dat =TrCardBmpData;
	for(i=0;i<TR_CARD_WIN_MAX;i++){
		GF_BGL_BmpWinAddEx( bgl, &win[i], &dat[i] );
	}
	//先頭キャラをクリア（スクリーンクリアされてるところは、このキャラで埋まる）
	GF_BGL_CharFill( bgl, FONT_BG, 0, 1, 0 );
#endif
}


//--------------------------------------------------------------------------------------------
/**
 * BMPウィンドウ破棄
 *
 * @param	win		BmpWin
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void TRCBmp_ExitTrCardBmpWin( GF_BGL_BMPWIN	*win )
{
	u16	i;

	for( i=0; i<TR_CARD_WIN_MAX; i++ ){
		GF_BGL_BmpWinDel( &win[i] );
	}
}

static const int MsgList[] = {
	MSG_TCARD_01,
	MSG_TCARD_02,
	MSG_TCARD_03,
	MSG_TCARD_04,
	MSG_TCARD_05,
	MSG_TCARD_06,
	MSG_TCARD_07,
	MSG_TCARD_08,
	MSG_TCARD_09,
	MSG_TCARD_10,
	MSG_TCARD_11,
};

//--------------------------------------------------------------------------------------------
/**
 * 情報表示（表面）
 *
 * @param	win				BmpWin
 * @param	inTrCardData	トレーナーカードデータ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void TRCBmp_WriteTrWinInfo( GF_BGL_BMPWIN	*win, const TR_CARD_DATA *inTrCardData )
{
	u8 i;
	MSGDATA_MANAGER* man;
	STRBUF *msg_buf;
	
	GF_BGL_BmpWinFill(
		&win[0], 0, 0, 0,  WIN_TR_NO_SX*8,  WIN_TR_NO_SY*8 );
	GF_BGL_BmpWinFill(
		&win[1], 0, 0, 0,  WIN_NAME_SX*8,  WIN_NAME_SY*8 );
	GF_BGL_BmpWinFill(
		&win[2], 0, 0, 0,  WIN_MONEY_SX*8,  WIN_MONEY_SY*8 );
	GF_BGL_BmpWinFill(
		&win[3], 0, 0, 0,  WIN_BOOK_SX*8,  WIN_BOOK_SY*8 );
	GF_BGL_BmpWinFill(
		&win[4], 0, 0, 0,  WIN_SCORE_SX*8,  WIN_SCORE_SY*8 );
	GF_BGL_BmpWinFill(
		&win[5], 0, 0, 0,  WIN_TIME_SX*8,  WIN_TIME_SY*8 );
	GF_BGL_BmpWinFill(
		&win[6], 0, 0, 0,  WIN_S_TIME_SX*8,  WIN_S_TIME_SY*8 );

	man = MSGMAN_Create(MSGMAN_TYPE_NORMAL, ARC_MSG, NARC_msg_trainerscard_dat, HEAPID_TR_CARD);
	msg_buf = STRBUF_Create(TR_STRING_LEN, HEAPID_TR_CARD);
	for(i=0;i<7;i++){
		if ( (i!=3) ||
				(i==3)&&(inTrCardData->PokeBookFlg) ){	//ずかんのみ持っているときだけ表示
			MSGMAN_GetString(man, MsgList[i] ,msg_buf);
			GF_STR_PrintColor(&win[i], FONT_SYSTEM, msg_buf, 0, 0, MSG_ALLPUT, TR_MSGCOLOR, NULL);
		}
	}
	
	{
		// ----------------------------------------------------------------------------
		// localize_spec_mark(LANG_ALL) imatake 2006/12/14
		// 単位類を gmm のテキストに移行

		STRBUF *tmp_buf = STRBUF_Create(TR_STRING_LEN, HEAPID_TR_CARD);
		WORDSET *wordset = WORDSET_CreateEx(WORDMAX_ALL, WORDSET_DEFAULT_BUFLEN, HEAPID_TR_CARD);
		
		//ID
		WriteNumData(	&win[TRC_BMPWIN_TR_ID],
						BMP_WIDTH_TYPE1, 0, 0, msg_buf, inTrCardData->TrainerID, TR_ID_DIGIT,
						NUMBER_DISPTYPE_ZERO);
		
		//なまえ
		STRBUF_SetStringCode( msg_buf, inTrCardData->TrainerName );
		WriteStrData(&win[TRC_BMPWIN_TR_NAME], BMP_WIDTH_TYPE1, 0, 0, msg_buf);

		//おこづかい
		{
			u32 xofs;
			WORDSET_RegisterNumber(wordset, BUFID_MONEY, inTrCardData->Money, MONEY_DIGIT, NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);
			MSGMAN_GetString(man, MSG_TCARD_15, tmp_buf);
			WORDSET_ExpandStr(wordset, msg_buf, tmp_buf);
			xofs = BMP_WIDTH_TYPE1 - FontProc_GetPrintStrWidth(FONT_SYSTEM, msg_buf, 0);
			GF_STR_PrintColor(&win[TRC_BMPWIN_MONEY], FONT_SYSTEM, msg_buf, xofs, 0, MSG_ALLPUT, TR_MSGCOLOR, NULL);
		}

		//ずかん
		if (inTrCardData->PokeBookFlg){	//表示フラグがたっているときのみ表示	
			// MatchComment: new change in plat US
            u32 xofs;
            WORDSET_RegisterNumber(wordset, BUFID_MONEY, inTrCardData->PokeBook, MONS_NUM_DIGIT, NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);
            MSGMAN_GetString(man, MSG_TCARD_22, tmp_buf);
            WORDSET_ExpandStr(wordset, msg_buf, tmp_buf);
			xofs = BMP_WIDTH_TYPE1 - FontProc_GetPrintStrWidth(FONT_SYSTEM, msg_buf, 0);
			GF_STR_PrintColor(&win[TRC_BMPWIN_BOOK], FONT_SYSTEM, msg_buf, xofs, 0, MSG_ALLPUT, TR_MSGCOLOR, NULL);
		}
		
		//スコア
		WriteNumData(	&win[TRC_BMPWIN_SCORE],
						BMP_WIDTH_TYPE1, 0, 0, msg_buf, inTrCardData->Score, SCORE_DIGIT,
						NUMBER_DISPTYPE_SPACE);
		
		//プレイ時間
		{
			u32 xofs;
			if ( inTrCardData->TimeUpdate) {	//通常
				WORDSET_RegisterNumber(wordset, BUFID_HOUR,   PLAYTIME_GetHour(inTrCardData->PlayTime),   TIME_H_DIGIT, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT);
				WORDSET_RegisterNumber(wordset, BUFID_MINUTE, PLAYTIME_GetMinute(inTrCardData->PlayTime), TIME_M_DIGIT, NUMBER_DISPTYPE_ZERO,  NUMBER_CODETYPE_DEFAULT);
				MSGMAN_GetString(man, MSG_TCARD_17, tmp_buf);
			} else {							//時間更新しない場合のみ、固定で「：」表示
				WORDSET_RegisterNumber(wordset, BUFID_HOUR,   inTrCardData->PlayTime_h, TIME_H_DIGIT, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT);
				WORDSET_RegisterNumber(wordset, BUFID_MINUTE, inTrCardData->PlayTime_m, TIME_M_DIGIT, NUMBER_DISPTYPE_ZERO,  NUMBER_CODETYPE_DEFAULT);
				MSGMAN_GetString(man, MSG_TCARD_16, tmp_buf);
			}
			WORDSET_ExpandStr(wordset, msg_buf, tmp_buf);
			xofs = BMP_WIDTH_TYPE2 - FontProc_GetPrintStrWidth(FONT_SYSTEM, msg_buf, 0);
			GF_STR_PrintColor(&win[TRC_BMPWIN_PLAY_TIME], FONT_SYSTEM, msg_buf, xofs, 0, MSG_ALLPUT, TR_MSGCOLOR, NULL);
		}
		
		//スタート時間
		{
			u32 xofs;
			WORDSET_RegisterNumber(wordset, BUFID_YEAR,  inTrCardData->Start_y, DAY_DIGIT, NUMBER_DISPTYPE_ZERO, NUMBER_CODETYPE_DEFAULT);
			// ----------------------------------------------------------------------------
			// localize_spec_mark(LANG_ALL) imatake 2007/01/26
			// 月の表示を単語表記に変更
			WORDSET_RegisterMonthName( wordset, BUFID_MONTH, inTrCardData->Start_m );
			// ----------------------------------------------------------------------------
			WORDSET_RegisterNumber(wordset, BUFID_DAY,   inTrCardData->Start_d, DAY_DIGIT, NUMBER_DISPTYPE_ZERO, NUMBER_CODETYPE_DEFAULT);
			MSGMAN_GetString(man, MSG_TCARD_18, tmp_buf);
			WORDSET_ExpandStr(wordset, msg_buf, tmp_buf);
			xofs = BMP_WIDTH_TYPE2 - FontProc_GetPrintStrWidth(FONT_SYSTEM, msg_buf, 0);
			GF_STR_PrintColor(&win[TRC_BMPWIN_START_TIME], FONT_SYSTEM, msg_buf, xofs, 0, MSG_ALLPUT, TR_MSGCOLOR, NULL);
		}

		STRBUF_Delete( tmp_buf );
		WORDSET_Delete( wordset );

		// ----------------------------------------------------------------------------

	}
	STRBUF_Delete( msg_buf );
	MSGMAN_Delete( man );
}

//--------------------------------------------------------------------------------------------
/**
 * 情報表示（裏面）
 *
 * @param	win				BmpWin
 * @param	inTrCardData	トレーナーカードデータ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
#ifdef NONEQUIVALENT
void TRCBmp_WriteTrWinInfoRev( GF_BGL_BMPWIN	*win, const TR_CARD_DATA *inTrCardData  )
{
	u8 i;
	MSGDATA_MANAGER* man;
	STRBUF *msg_buf;

	GF_BGL_BmpWinFill(
		&win[7], 0, 0, 0,  WIN_CLEAR_SX*8,  WIN_CLEAR_SY*8 );
	GF_BGL_BmpWinFill(
		&win[8], 0, 0, 0,  WIN_COMM_SX*8,  WIN_COMM_SY*8 );
	GF_BGL_BmpWinFill(
		&win[9], 0, 0, 0,  WIN_BATTLE_SX*8,  WIN_BATTLE_SY*8 );
	GF_BGL_BmpWinFill(
		&win[10], 0, 0, 0,  WIN_TRADE_SX*8,  WIN_TRADE_SY*8 );

	man = MSGMAN_Create(MSGMAN_TYPE_NORMAL, ARC_MSG, NARC_msg_trainerscard_dat, HEAPID_TR_CARD);
	msg_buf = STRBUF_Create(TR_STRING_LEN, HEAPID_TR_CARD);
	
	for(i=7;i<TR_CARD_WIN_MAX;i++){
		MSGMAN_GetString(man, MsgList[i] ,msg_buf);
		GF_STR_PrintColor(&win[i], FONT_SYSTEM, msg_buf, 0, 0, MSG_ALLPUT, TR_MSGCOLOR, NULL);
	}
	{
		// ----------------------------------------------------------------------------
		// localize_spec_mark(LANG_ALL) imatake 2006/12/14
		// 単位類を gmm のテキストに移行

		STRBUF *tmp_buf = STRBUF_Create(TR_STRING_LEN, HEAPID_TR_CARD);
		WORDSET *wordset = WORDSET_CreateEx(WORDMAX_DATE, WORDSET_DEFAULT_BUFLEN, HEAPID_TR_CARD);

		//殿堂入り
		{
			u32 xofs;

			// ----------------------------------------------------------------------------
			// localize_spec_mark(LANG_ALL) imatake 2007/02/13
			// でんどういり前のでんどういりの日付表示をベタテキストに置き換え

			if (inTrCardData->Clear_m != 0){	//月が0月でなければ、クリアしたとみなす
				WORDSET_RegisterNumber(wordset, BUFID_YEAR,   inTrCardData->Clear_y,     YEAR_DIGIT,   NUMBER_DISPTYPE_ZERO,  NUMBER_CODETYPE_DEFAULT);
				// ----------------------------------------------------------------------------
				// localize_spec_mark(LANG_ALL) imatake 2007/01/26
				// 月の表示を単語表記に変更
				WORDSET_RegisterMonthName( wordset, BUFID_MONTH, inTrCardData->Clear_m );
				// ----------------------------------------------------------------------------
				WORDSET_RegisterNumber(wordset, BUFID_DAY,    inTrCardData->Clear_d,     DAY_DIGIT,    NUMBER_DISPTYPE_ZERO,  NUMBER_CODETYPE_DEFAULT);
				WORDSET_RegisterNumber(wordset, BUFID_HOUR,   inTrCardData->ClearTime_h, TIME_H_DIGIT, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT);
				WORDSET_RegisterNumber(wordset, BUFID_MINUTE, inTrCardData->ClearTime_m, TIME_M_DIGIT, NUMBER_DISPTYPE_ZERO,  NUMBER_CODETYPE_DEFAULT);
				MSGMAN_GetString(man, MSG_TCARD_18, tmp_buf);
				WORDSET_ExpandStr(wordset, msg_buf, tmp_buf);
			}else{
				MSGMAN_GetString(man, MSG_TCARD_13, tmp_buf);
				WORDSET_RegisterWord(wordset, BUFID_HOUR,   tmp_buf, 0, FALSE, PM_LANG);
				WORDSET_RegisterWord(wordset, BUFID_MINUTE, tmp_buf, 0, FALSE, PM_LANG);
				MSGMAN_GetString(man, MSG_TCARD_18_2, msg_buf);
			}
			// 殿堂入りの日付
			xofs = BMP_WIDTH_TYPE2 - FontProc_GetPrintStrWidth(FONT_SYSTEM, msg_buf, 0);
			GF_STR_PrintColor(&win[TRC_BMPWIN_CLEAR_TIME], FONT_SYSTEM, msg_buf, xofs, 0, MSG_ALLPUT, TR_MSGCOLOR, NULL);

			// ----------------------------------------------------------------------------

			// 殿堂入りまでのプレイ時間
			MSGMAN_GetString(man, MSG_TCARD_16, tmp_buf);
			WORDSET_ExpandStr(wordset, msg_buf, tmp_buf);
			xofs = BMP_WIDTH_TYPE2 - FontProc_GetPrintStrWidth(FONT_SYSTEM, msg_buf, 0);
			GF_STR_PrintColor(&win[TRC_BMPWIN_CLEAR_TIME], FONT_SYSTEM, msg_buf, xofs, 16, MSG_ALLPUT, TR_MSGCOLOR, NULL);
		}
		
		//通信回数
		WriteNumData(	&win[TRC_BMPWIN_COMM_INFO],
						BMP_WIDTH_TYPE2, 0, 0, msg_buf, inTrCardData->CommNum, COMM_DIGIT,
						NUMBER_DISPTYPE_SPACE);
		
		//通信対戦
		MSGMAN_GetString(man, MSG_TCARD_19, msg_buf);
		GF_STR_PrintColor(&win[TRC_BMPWIN_BATTLE_INFO], FONT_SYSTEM, msg_buf, WIN_OFS,  0, MSG_ALLPUT, TR_MSGCOLOR, NULL);
		WriteNumData(	&win[TRC_BMPWIN_BATTLE_INFO],
						BMP_WIDTH_TYPE2, 0, 0, msg_buf, inTrCardData->CommBattleLose, BATTLE_DIGIT,
						NUMBER_DISPTYPE_SPACE);
		MSGMAN_GetString(man, MSG_TCARD_20, msg_buf);
		GF_STR_PrintColor(&win[TRC_BMPWIN_BATTLE_INFO], FONT_SYSTEM, msg_buf, LOSE_OFS, 0, MSG_ALLPUT, TR_MSGCOLOR, NULL);
		WriteNumData(	&win[TRC_BMPWIN_BATTLE_INFO],
						BMP_WIDTH_TYPE2, 8*8, 0, msg_buf, inTrCardData->CommBattleWin, BATTLE_DIGIT,
						NUMBER_DISPTYPE_SPACE);
		
		//通信交換
		WriteNumData(	&win[TRC_BMPWIN_TRADE_INFO],
						BMP_WIDTH_TYPE2, 0, 0, msg_buf, inTrCardData->CommTrade, TRADE_DIGIT,
						NUMBER_DISPTYPE_SPACE);

		STRBUF_Delete( tmp_buf );
		WORDSET_Delete( wordset );

		// ----------------------------------------------------------------------------
	}
	STRBUF_Delete( msg_buf );
	MSGMAN_Delete( man );
}
#else
asm void TRCBmp_WriteTrWinInfoRev( GF_BGL_BMPWIN	*win, const TR_CARD_DATA *inTrCardData  )
{
	push {r3, r4, r5, r6, r7, lr}
	sub sp, #0x18
	add r6, r0, #0
	mov r0, #0xe0
	str r0, [sp]
	mov r0, #0x20
	str r1, [sp, #0x10]
	mov r1, #0
	str r0, [sp, #4]
	add r0, r6, #0
	add r0, #0x70
	add r2, r1, #0
	add r3, r1, #0
	bl GF_BGL_BmpWinFill
	mov r1, #0
	mov r0, #0xe0
	str r0, [sp]
	mov r0, #0x10
	str r0, [sp, #4]
	add r0, r6, #0
	add r0, #0x80
	add r2, r1, #0
	add r3, r1, #0
	bl GF_BGL_BmpWinFill
	mov r1, #0
	mov r0, #0xe0
	str r0, [sp]
	mov r0, #0x10
	str r0, [sp, #4]
	add r0, r6, #0
	add r0, #0x90
	add r2, r1, #0
	add r3, r1, #0
	bl GF_BGL_BmpWinFill
	mov r1, #0
	mov r0, #0xe0
	str r0, [sp]
	mov r0, #0x10
	str r0, [sp, #4]
	add r0, r6, #0
	add r0, #0xa0
	add r2, r1, #0
	add r3, r1, #0
	bl GF_BGL_BmpWinFill
	mov r2, #0x9a
	mov r0, #0
	mov r1, #0x1a
	lsl r2, r2, #2
	mov r3, #0x19
	bl MSGMAN_Create
	add r7, r0, #0
	mov r0, #0x20
	mov r1, #0x19
	bl STRBUF_Create
	add r5, r0, #0
	mov r4, #7
_0223CAA4:
	ldr r1, =MsgList // _0223CD3C
	lsl r2, r4, #2
	ldr r1, [r1, r2]
	add r0, r7, #0
	add r2, r5, #0
	bl MSGMAN_GetString
	mov r0, #0
	str r0, [sp]
	str r0, [sp, #4]
	ldr r0, =0x00010200 // _0223CD40
	mov r1, #0
	str r0, [sp, #8]
	mov r0, #0
	str r0, [sp, #0xc]
	lsl r0, r4, #4
	add r0, r6, r0
	add r2, r5, #0
	add r3, r1, #0
	bl GF_STR_PrintColor
	add r0, r4, #1
	lsl r0, r0, #0x18
	lsr r4, r0, #0x18
	cmp r4, #0xb
	blo _0223CAA4
	mov r0, #0x20
	mov r1, #0x19
	bl STRBUF_Create
	str r0, [sp, #0x14]
	mov r0, #6
	mov r1, #0x20
	mov r2, #0x19
	bl WORDSET_CreateEx
	ldr r1, [sp, #0x10]
	add r4, r0, #0
	add r1, #0x33
	ldrb r1, [r1]
	cmp r1, #0
	beq _0223CB6E
	mov r1, #2
	str r1, [sp]
	mov r2, #1
	str r2, [sp, #4]
	ldr r2, [sp, #0x10]
	add r3, r1, #0
	add r2, #0x32
	ldrb r2, [r2]
	bl WORDSET_RegisterNumber
	ldr r2, [sp, #0x10]
	add r0, r4, #0
	add r2, #0x33
	ldrb r2, [r2]
	mov r1, #3
	bl WORDSET_RegisterMonthName
	mov r3, #2
	ldr r2, [sp, #0x10]
	str r3, [sp]
	mov r0, #1
	str r0, [sp, #4]
	add r2, #0x34
	ldrb r2, [r2]
	add r0, r4, #0
	mov r1, #4
	bl WORDSET_RegisterNumber
	mov r0, #1
	str r0, [sp]
	str r0, [sp, #4]
	ldr r2, [sp, #0x10]
	add r0, r4, #0
	ldrh r2, [r2, #0x2c]
	mov r1, #0
	mov r3, #3
	bl WORDSET_RegisterNumber
	mov r3, #2
	ldr r2, [sp, #0x10]
	str r3, [sp]
	mov r1, #1
	str r1, [sp, #4]
	add r2, #0x35
	ldrb r2, [r2]
	add r0, r4, #0
	bl WORDSET_RegisterNumber
	ldr r2, [sp, #0x14]
	add r0, r7, #0
	mov r1, #0x11
	bl MSGMAN_GetString
	ldr r2, [sp, #0x14]
	add r0, r4, #0
	add r1, r5, #0
	bl WORDSET_ExpandStr
	b _0223CBA6
_0223CB6E:
	ldr r2, [sp, #0x14]
	add r0, r7, #0
	mov r1, #0xc
	bl MSGMAN_GetString
	mov r1, #0
	str r1, [sp]
	mov r0, #2
	str r0, [sp, #4]
	ldr r2, [sp, #0x14]
	add r0, r4, #0
	add r3, r1, #0
	bl WORDSET_RegisterWord
	mov r3, #0
	str r3, [sp]
	mov r0, #2
	str r0, [sp, #4]
	ldr r2, [sp, #0x14]
	add r0, r4, #0
	mov r1, #1
	bl WORDSET_RegisterWord
	add r0, r7, #0
	mov r1, #0x14
	add r2, r5, #0
	bl MSGMAN_GetString
_0223CBA6:
	mov r0, #0
	add r1, r5, #0
	add r2, r0, #0
	bl FontProc_GetPrintStrWidth
	mov r1, #0xe0
	sub r3, r1, r0
	mov r1, #0
	str r1, [sp]
	ldr r0, =0x00010200 // _0223CD40
	str r1, [sp, #4]
	str r0, [sp, #8]
	add r0, r6, #0
	add r0, #0x70
	add r2, r5, #0
	str r1, [sp, #0xc]
	bl GF_STR_PrintColor
	ldr r2, [sp, #0x14]
	add r0, r7, #0
	mov r1, #0xf
	bl MSGMAN_GetString
	ldr r2, [sp, #0x14]
	add r0, r4, #0
	add r1, r5, #0
	bl WORDSET_ExpandStr
	mov r0, #0
	add r1, r5, #0
	add r2, r0, #0
	bl FontProc_GetPrintStrWidth
	mov r1, #0xe0
	sub r3, r1, r0
	mov r0, #0x10
	str r0, [sp]
	mov r1, #0
	ldr r0, =0x00010200 // _0223CD40
	str r1, [sp, #4]
	str r0, [sp, #8]
	add r0, r6, #0
	add r0, #0x70
	add r2, r5, #0
	str r1, [sp, #0xc]
	bl GF_STR_PrintColor
	mov r0, #0
	str r0, [sp]
	mov r0, #1
	str r0, [sp, #4]
	ldr r2, [sp, #0x10]
	add r0, r4, #0
	ldr r2, [r2, #0x38]
	mov r1, #5
	mov r3, #6
	bl WORDSET_RegisterNumber
	ldr r2, [sp, #0x14]
	add r0, r7, #0
	mov r1, #0x16
	bl MSGMAN_GetString
	ldr r2, [sp, #0x14]
	add r0, r4, #0
	add r1, r5, #0
	bl WORDSET_ExpandStr
	mov r0, #0
	add r1, r5, #0
	add r2, r0, #0
	bl FontProc_GetPrintStrWidth
	mov r1, #0xe0
	sub r3, r1, r0
	mov r1, #0
	str r1, [sp]
	ldr r0, =0x00010200 // _0223CD40
	str r1, [sp, #4]
	str r0, [sp, #8]
	add r0, r6, #0
	add r0, #0x80
	add r2, r5, #0
	str r1, [sp, #0xc]
	bl GF_STR_PrintColor
	add r0, r7, #0
	mov r1, #0x12
	add r2, r5, #0
	bl MSGMAN_GetString
	mov r1, #0
	str r1, [sp]
	ldr r0, =0x00010200 // _0223CD40
	str r1, [sp, #4]
	str r0, [sp, #8]
	add r0, r6, #0
	add r0, #0x90
	add r2, r5, #0
	mov r3, #0x70
	str r1, [sp, #0xc]
	bl GF_STR_PrintColor
	mov r2, #0
	ldr r0, [sp, #0x10]
	str r5, [sp]
	ldr r0, [r0, #0x40]
	mov r1, #0xe0
	str r0, [sp, #4]
	mov r0, #4
	str r0, [sp, #8]
	mov r0, #1
	str r0, [sp, #0xc]
	add r0, r6, #0
	add r0, #0x90
	add r3, r2, #0
	bl WriteNumData
	add r0, r7, #0
	mov r1, #0x13
	add r2, r5, #0
	bl MSGMAN_GetString
	mov r1, #0
	str r1, [sp]
	ldr r0, =0x00010200 // _0223CD40
	str r1, [sp, #4]
	str r0, [sp, #8]
	add r0, r6, #0
	add r0, #0x90
	add r2, r5, #0
	mov r3, #0xb0
	str r1, [sp, #0xc]
	bl GF_STR_PrintColor
	ldr r0, [sp, #0x10]
	str r5, [sp]
	ldr r0, [r0, #0x3c]
	mov r1, #0xe0
	str r0, [sp, #4]
	mov r0, #4
	str r0, [sp, #8]
	mov r0, #1
	str r0, [sp, #0xc]
	add r0, r6, #0
	add r0, #0x90
	mov r2, #0x40
	mov r3, #0
	bl WriteNumData
	mov r0, #0
	str r0, [sp]
	mov r0, #1
	str r0, [sp, #4]
	ldr r2, [sp, #0x10]
	add r0, r4, #0
	ldr r2, [r2, #0x44]
	mov r1, #5
	mov r3, #6
	bl WORDSET_RegisterNumber
	ldr r2, [sp, #0x14]
	add r0, r7, #0
	mov r1, #0x16
	bl MSGMAN_GetString
	ldr r2, [sp, #0x14]
	add r0, r4, #0
	add r1, r5, #0
	bl WORDSET_ExpandStr
	mov r0, #0
	add r1, r5, #0
	add r2, r0, #0
	bl FontProc_GetPrintStrWidth
	mov r1, #0xe0
	sub r3, r1, r0
	mov r1, #0
	str r1, [sp]
	ldr r0, =0x00010200 // _0223CD40
	str r1, [sp, #4]
	str r0, [sp, #8]
	add r6, #0xa0
	add r0, r6, #0
	add r2, r5, #0
	str r1, [sp, #0xc]
	bl GF_STR_PrintColor
	ldr r0, [sp, #0x14]
	bl STRBUF_Delete
	add r0, r4, #0
	bl WORDSET_Delete
	add r0, r5, #0
	bl STRBUF_Delete
	add r0, r7, #0
	bl MSGMAN_Delete
	add sp, #0x18
	pop {r3, r4, r5, r6, r7, pc}
	// .align 2, 0
// _0223CD3C: .4byte MsgList
// _0223CD40: .4byte 0x00010200
}
#endif

//--------------------------------------------------------------------------------------------
/**
 * BMPウィンドウ非表示
 *
 * @param	win				BmpWin
 * @param	start			対象ウィンドウ開始インデックス
 * @param	end				対象ウィンドウ最終インデックス
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void TRCBmp_NonDispWinInfo(GF_BGL_BMPWIN	*win, const u8 start, const u8 end)
{
	u8 i;
	for(i=start;i<=end;i++){
		GF_BGL_BmpWinOff( &win[i] );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * 数字表示
 *
 * @param	win				BmpWin
 * @param	inBmpWidth		幅
 * @param	inRightSpace	右空白
 * @param	inStartY		表示開始Y位置
 * @param	buff			バッファ
 * @param	inNum			数字
 * @param	inDigit			桁数
 * @param	inDispType		表示タイプ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void 
WriteNumData(	GF_BGL_BMPWIN *inWin,
							const u32 inBmpWidth,
							const u32 inRightSpace,
							const u32 inStartY,
							STRBUF *buff,
							const u32 inNum,
							const u8 inDigit,
							const NUMBER_DISPTYPE inDisptype)
{
	u32 len;

	STRBUF_SetNumber( buff, inNum, inDigit, inDisptype, NUMBER_CODETYPE_DEFAULT );
	len = FontProc_GetPrintStrWidth(FONT_SYSTEM,buff,0);

	GF_STR_PrintColor(	inWin, FONT_SYSTEM, buff,
						inBmpWidth-(len+inRightSpace), inStartY,
						MSG_ALLPUT, TR_MSGCOLOR, NULL);	
}

//--------------------------------------------------------------------------------------------
/**
 * 数字表示(上書き用)
 *
 * @param	win				BmpWin
 * @param	inBmpWidth		幅
 * @param	inRightSpace	右空白
 * @param	inStartY		表示開始Y位置
 * @param	buff			バッファ
 * @param	inNum			数字
 * @param	inDigit			桁数
 * @param	inDispType		表示タイプ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void 
WriteNumDataFill(	GF_BGL_BMPWIN *inWin,
					const u32 inBmpWidth,
					const u32 inRightSpace,
					const u32 inStartY,
					STRBUF *buff,
					const u32 inNum,
					const u8 inDigit,
					const NUMBER_DISPTYPE inDisptype,
					const u32 inFillSatrt,
					const u32 inFillWidth)
{
	u32 len;
	
	STRBUF_SetNumber( buff, inNum, inDigit, inDisptype, NUMBER_CODETYPE_DEFAULT );
	len = FontProc_GetPrintStrWidth(FONT_SYSTEM,buff,0);

	GF_BGL_BmpWinFill( inWin, 0, inFillSatrt, 0,  inFillWidth,  2*8 );

	GF_STR_PrintColor(	inWin, FONT_SYSTEM, buff,
						inBmpWidth-(len+inRightSpace), inStartY,
						MSG_ALLPUT, TR_MSGCOLOR, NULL);	
}

//--------------------------------------------------------------------------------------------
/**
 * 文字列表示
 *
 * @param	win				BmpWin
 * @param	inBmpWidth		幅
 * @param	inRightSpace	右空白
 * @param	inStartY		表示開始Y位置
 * @param	buff			バッファ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void WriteStrData(	GF_BGL_BMPWIN *inWin,
							const u32 inBmpWidth,
							const u32 inRightSpace,
							const u32 inStartY,
							const STRBUF *buff)
{
	u32 len;
	len = FontProc_GetPrintStrWidth(FONT_SYSTEM,buff,0);
	GF_STR_PrintColor(	inWin, FONT_SYSTEM, buff,
						inBmpWidth-(len+inRightSpace), inStartY,
						MSG_ALLPUT, TR_MSGCOLOR, NULL);
}

//--------------------------------------------------------------------------------------------
/**
 * プレイ時間描画
 *
 * @param	win				BmpWin
 * @param	inTrCardData	トレーナーカードデータ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void TRCBmp_WritePlayTime(GF_BGL_BMPWIN	*win, const TR_CARD_DATA *inTrCardData, STRBUF *str)
{
	int hour;
	
	GF_ASSERT(inTrCardData->PlayTime!=NULL&&"ERROR:PlayTimeData is NULL!!");

	hour = PLAYTIME_GetHour(inTrCardData->PlayTime);

	if (hour>HOUR_DISP_MAX){
		hour = HOUR_DISP_MAX;
	}

	// ----------------------------------------------------------------------------
	// localize_spec_mark(LANG_ALL) imatake 2006/12/14
	// 単位類を gmm のテキストに移行

	GF_BGL_BmpWinFill( &win[TRC_BMPWIN_PLAY_TIME], 0, TIME_OFS, 0, TIME_WIDTH, 2*8 );

	{
		u32 xofs;
		MSGDATA_MANAGER *man = MSGMAN_Create(MSGMAN_TYPE_NORMAL, ARC_MSG, NARC_msg_trainerscard_dat, HEAPID_TR_CARD);
		STRBUF *msg_buf = STRBUF_Create(TR_STRING_LEN, HEAPID_TR_CARD);
		STRBUF *tmp_buf = STRBUF_Create(TR_STRING_LEN, HEAPID_TR_CARD);
		WORDSET *wordset = WORDSET_CreateEx(WORDMAX_TIME, WORDSET_DEFAULT_BUFLEN, HEAPID_TR_CARD);

		WORDSET_RegisterNumber(wordset, BUFID_HOUR,   PLAYTIME_GetHour(inTrCardData->PlayTime),   TIME_H_DIGIT, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT);
		WORDSET_RegisterNumber(wordset, BUFID_MINUTE, PLAYTIME_GetMinute(inTrCardData->PlayTime), TIME_M_DIGIT, NUMBER_DISPTYPE_ZERO,  NUMBER_CODETYPE_DEFAULT);
		MSGMAN_GetString(man, MSG_TCARD_17, tmp_buf);
		WORDSET_ExpandStr(wordset, msg_buf, tmp_buf);
		xofs = BMP_WIDTH_TYPE2 - FontProc_GetPrintStrWidth(FONT_SYSTEM, msg_buf, 0);
		GF_STR_PrintColor(&win[TRC_BMPWIN_PLAY_TIME], FONT_SYSTEM, msg_buf, xofs, 0, MSG_ALLPUT, TR_MSGCOLOR, NULL);

		MSGMAN_Delete( man );
		STRBUF_Delete( msg_buf );
		STRBUF_Delete( tmp_buf );
		WORDSET_Delete( wordset );
	}

	// ----------------------------------------------------------------------------
}

//--------------------------------------------------------------------------------------------
/**
 * 秒表示
 *
 * @param	win				BmpWin
 * @param	inDisp			表示フラグ
 * @param	inSecBuf		文字列（コロン）
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void TRCBmp_WriteSec(GF_BGL_BMPWIN	*win, const BOOL inDisp, STRBUF *inSecBuf)
{
	// ----------------------------------------------------------------------------
	// localize_spec_mark(LANG_ALL) imatake 2006/12/14
	// 単位類を gmm のテキストに移行
	if (inDisp){
		GF_STR_PrintColor(win, FONT_SYSTEM, inSecBuf, COLON_OFS, 0, MSG_ALLPUT, TR_MSGCOLOR, NULL);
	}else{
		GF_BGL_BmpWinFill( win, 0, COLON_OFS, 0, COLON_WIDTH, WIN_S_TIME_SY*8 );
		GF_BGL_BmpWinOn( win );
	}
	// ----------------------------------------------------------------------------
}


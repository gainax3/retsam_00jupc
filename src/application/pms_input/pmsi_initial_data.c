//============================================================================================
/**
 * @file	pmsi_initial_data.c
 * @bfief	簡易会話入力画面（カテゴリイニシャルモード用のデータテーブル処理）
 * @author	taya
 * @date	06.02.14
 */
//============================================================================================
#include "common.h"
#include "gflib\heapsys.h"
#include "gflib\strbuf_family.h"
#include "system\arc_util.h"
#include "system\window.h"
#include "system\buflen.h"
#include "system\pms_word.h"
#include "system\winframe.naix"
#include "system\msgdata.h"
#include "msgdata\msg_pms_category.h"
#include "msgdata\msg.naix"


#include "pms_input_prv.h"
#include "pmsi_initial_data.h"

//======================================================================
// イニシャルモード時の選択文字テーブル（言語ごとに設定）
//======================================================================

//-------------------------------------------------------------
/**
 *  各文字の描画幅
 */
//-------------------------------------------------------------
enum {
	// ----------------------------------------------------------------------------
	// localize_spec_mark(LANG_ALL) imatake 2007/01/09
	// 頭文字パレットのレイアウト調整
	INITIAL_WRITE_X_MARGIN = 16,		// １文字ごとの描画幅
	INITIAL_WRITE_Y_MARGIN = 16,		// １文字ごとの描画高さ
	INITIAL_WRITE_X_OFFSET = 28,		// 左上の文字のX座標
	INITIAL_WRITE_Y_OFFSET = 12,		// 左上の文字のY座標
	// ----------------------------------------------------------------------------
};

// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ALL) imatake 2007/01/09
// 頭文字パレットのレイアウト調整
#define INITIAL_XPOS(n)	((n)*INITIAL_WRITE_X_MARGIN+INITIAL_WRITE_X_OFFSET)
#define INITIAL_YPOS(n)	((n)*INITIAL_WRITE_Y_MARGIN+INITIAL_WRITE_Y_OFFSET)
// ----------------------------------------------------------------------------

//-------------------------------------------------------------
/**
 *  各文字のインデックス
 */
//-------------------------------------------------------------
// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ALL) imatake 2007/01/09
// 頭文字パレットのレイアウト調整
enum {
	INI_A = 0,
	INI_B,
	INI_C,
	INI_D,
	INI_E,
	INI_F,
	INI_G,
	INI_H,
	INI_I,
	INI_J,
	INI_K,
	INI_L,
	INI_M,
	INI_N,
	INI_O,
	INI_P,
	INI_Q,
	INI_R,
	INI_S,
	INI_T,
	INI_U,
	INI_V,
	INI_W,
	INI_X,
	INI_Y,
	INI_Z,
	INI_OTHERS,

	INI_BACK = CATEGORY_POS_BACK,
	INI_DIS  = CATEGORY_POS_DISABLE,
};

static const struct {
	STRCODE		code;
	u8			xpos;
	u8			ypos;
	u8			up;
	u8			down;
	u8			left;
	u8			right;
	u8			col_top;
	u8			col_bottom;
}InitialParamTbl[] = {

	// localize_spec_mark(LANG_ALL) yamamoto 2006/12/28 仮対応。
	{ h_A__,  INITIAL_XPOS(0), INITIAL_YPOS(0), INI_BACK,   INI_K,      INI_J,      INI_B,      INI_A,      INI_U,      },
	{ h_B__,  INITIAL_XPOS(1), INITIAL_YPOS(0), INI_BACK,   INI_L,      INI_A,      INI_C,      INI_B,      INI_V,      },
	{ h_C__,  INITIAL_XPOS(2), INITIAL_YPOS(0), INI_BACK,   INI_M,      INI_B,      INI_D,      INI_C,      INI_W,      },
	{ h_D__,  INITIAL_XPOS(3), INITIAL_YPOS(0), INI_BACK,   INI_N,      INI_C,      INI_E,      INI_D,      INI_X,      },
	{ h_E__,  INITIAL_XPOS(4), INITIAL_YPOS(0), INI_BACK,   INI_O,      INI_D,      INI_F,      INI_E,      INI_Y,      },
	{ h_F__,  INITIAL_XPOS(5), INITIAL_YPOS(0), INI_BACK,   INI_P,      INI_E,      INI_G,      INI_F,      INI_Z,      },
	{ h_G__,  INITIAL_XPOS(6), INITIAL_YPOS(0), INI_BACK,   INI_Q,      INI_F,      INI_H,      INI_G,      INI_Q,      },
	{ h_H__,  INITIAL_XPOS(7), INITIAL_YPOS(0), INI_BACK,   INI_R,      INI_G,      INI_I,      INI_H,      INI_R,      },
	{ h_I__,  INITIAL_XPOS(8), INITIAL_YPOS(0), INI_BACK,   INI_S,      INI_H,      INI_J,      INI_I,      INI_S,      },
	{ h_J__,  INITIAL_XPOS(9), INITIAL_YPOS(0), INI_BACK,   INI_T,      INI_I,      INI_A,      INI_J,      INI_OTHERS, },
	{ h_K__,  INITIAL_XPOS(0), INITIAL_YPOS(1), INI_A,      INI_U,      INI_T,      INI_L,      INI_A,      INI_U,      },
	{ h_L__,  INITIAL_XPOS(1), INITIAL_YPOS(1), INI_B,      INI_V,      INI_K,      INI_M,      INI_B,      INI_V,      },
	{ h_M__,  INITIAL_XPOS(2), INITIAL_YPOS(1), INI_C,      INI_W,      INI_L,      INI_N,      INI_C,      INI_W,      },
	{ h_N__,  INITIAL_XPOS(3), INITIAL_YPOS(1), INI_D,      INI_X,      INI_M,      INI_O,      INI_D,      INI_X,      },
	{ h_O__,  INITIAL_XPOS(4), INITIAL_YPOS(1), INI_E,      INI_Y,      INI_N,      INI_P,      INI_E,      INI_Y,      },
	{ h_P__,  INITIAL_XPOS(5), INITIAL_YPOS(1), INI_F,      INI_Z,      INI_O,      INI_Q,      INI_F,      INI_Z,      },
	{ h_Q__,  INITIAL_XPOS(6), INITIAL_YPOS(1), INI_G,      INI_BACK,   INI_P,      INI_R,      INI_G,      INI_Q,      },
	{ h_R__,  INITIAL_XPOS(7), INITIAL_YPOS(1), INI_H,      INI_BACK,   INI_Q,      INI_S,      INI_H,      INI_R,      },
	{ h_S__,  INITIAL_XPOS(8), INITIAL_YPOS(1), INI_I,      INI_BACK,   INI_R,      INI_T,      INI_I,      INI_S,      },
	{ h_T__,  INITIAL_XPOS(9), INITIAL_YPOS(1), INI_J,      INI_OTHERS, INI_S,      INI_K,      INI_J,      INI_OTHERS, },
	{ h_U__,  INITIAL_XPOS(0), INITIAL_YPOS(2), INI_K,      INI_BACK,   INI_OTHERS, INI_V,      INI_A,      INI_U,      },
	{ h_V__,  INITIAL_XPOS(1), INITIAL_YPOS(2), INI_L,      INI_BACK,   INI_U,      INI_W,      INI_B,      INI_V,      },
	{ h_W__,  INITIAL_XPOS(2), INITIAL_YPOS(2), INI_M,      INI_BACK,   INI_V,      INI_X,      INI_C,      INI_W,      },
	{ h_X__,  INITIAL_XPOS(3), INITIAL_YPOS(2), INI_N,      INI_BACK,   INI_W,      INI_Y,      INI_D,      INI_X,      },
	{ h_Y__,  INITIAL_XPOS(4), INITIAL_YPOS(2), INI_O,      INI_BACK,   INI_X,      INI_Z,      INI_E,      INI_Y,      },
	{ h_Z__,  INITIAL_XPOS(5), INITIAL_YPOS(2), INI_P,      INI_BACK,   INI_Y,      INI_OTHERS, INI_F,      INI_Z,      },
	{ h_gyoe_,INITIAL_XPOS(9), INITIAL_YPOS(2), INI_T,      INI_BACK,   INI_Z,      INI_U,      INI_J,      INI_OTHERS, },


};
// ----------------------------------------------------------------------------




//------------------------------------------------------------------
/**
 * イニシャルナンバー最大を返す
 *
 * @retval  u32		
 */
//------------------------------------------------------------------
u32 PMSI_INITIAL_DAT_GetInitialMax( void )
{
	return NELEMS(InitialParamTbl);
}

//------------------------------------------------------------------
/**
 * １文字の文字コードをバッファにコピー
 *
 * @param   initial		イニシャルナンバー
 * @param   buf			コピー先バッファ
 *
 */
//------------------------------------------------------------------
void PMSI_INITIAL_DAT_GetStr( u32 initial, STRBUF* buf )
{
	GF_ASSERT(initial < NELEMS(InitialParamTbl));

	STRBUF_SetStringCodeOrderLength( buf, &(InitialParamTbl[initial].code), 2);
}

//------------------------------------------------------------------
/**
 * 描画用基準位置（ドット単位）を取得
 *
 * @param   initial		イニシャルナンバー
 * @param   x			描画位置Ｘ
 * @param   y			描画位置Ｙ
 *
 */
//------------------------------------------------------------------
void PMSI_INITIAL_DAT_GetPrintPos( u32 initial, u32* x, u32* y )
{
	GF_ASSERT_MSG((initial < NELEMS(InitialParamTbl)), "pos=%d", initial);

	*x = InitialParamTbl[initial].xpos;
	*y = InitialParamTbl[initial].ypos;
}

//------------------------------------------------------------------
/**
 * 上キー押下時の移動先イニシャルナンバーを返す
 *
 * @param   initial		イニシャルナンバー
 *
 * @retval  int		移動先イニシャルナンバー
 */
//------------------------------------------------------------------
int PMSI_INITIAL_DAT_GetUpCode( u32 initial )
{
	GF_ASSERT(initial < NELEMS(InitialParamTbl));
	return InitialParamTbl[initial].up;
}

//------------------------------------------------------------------
/**
 * 下キー押下時の移動先イニシャルナンバーを返す
 *
 * @param   initial		イニシャルナンバー
 *
 * @retval  int		移動先イニシャルナンバー
 */
//------------------------------------------------------------------
int PMSI_INITIAL_DAT_GetDownCode( u32 initial )
{
	GF_ASSERT(initial < NELEMS(InitialParamTbl));
	return InitialParamTbl[initial].down;
}

//------------------------------------------------------------------
/**
 * 左キー押下時の移動先イニシャルナンバーを返す
 *
 * @param   initial		イニシャルナンバー
 *
 * @retval  int		移動先イニシャルナンバー
 */
//------------------------------------------------------------------
int PMSI_INITIAL_DAT_GetLeftCode( u32 initial )
{
	GF_ASSERT(initial < NELEMS(InitialParamTbl));
	return InitialParamTbl[initial].left;
}

//------------------------------------------------------------------
/**
 * 右キー押下時の移動先イニシャルナンバーを返す
 *
 * @param   initial		イニシャルナンバー
 *
 * @retval  int		移動先イニシャルナンバー
 */
//------------------------------------------------------------------
int PMSI_INITIAL_DAT_GetRightCode( u32 initial )
{
	GF_ASSERT(initial < NELEMS(InitialParamTbl));
	return InitialParamTbl[initial].right;
}

//------------------------------------------------------------------
/**
 * 指定キーのある列の最上段ナンバーを返す
 *
 * @param   initial		イニシャルナンバー
 *
 * @retval  int		移動先イニシャルナンバー
 */
//------------------------------------------------------------------
int PMSI_INITIAL_DAT_GetColTopCode( u32 initial )
{
	GF_ASSERT(initial < NELEMS(InitialParamTbl));
	return InitialParamTbl[initial].col_top;
}

//------------------------------------------------------------------
/**
 * 指定キーのある列の最下段ナンバーを返す
 *
 * @param   initial		イニシャルナンバー
 *
 * @retval  int		移動先イニシャルナンバー
 */
//------------------------------------------------------------------
int PMSI_INITIAL_DAT_GetColBottomCode( u32 initial )
{
	GF_ASSERT(initial < NELEMS(InitialParamTbl));
	return InitialParamTbl[initial].col_bottom;
}



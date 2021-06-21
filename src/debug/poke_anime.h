//==============================================================================
/**
 * @file	poke_anim.h
 * @brief	
 * @author	goto
 * @date	2007.02.05(月)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================

enum {
	
	ePokeAnimeEdit_No = 0,		///< Ｎｏ．
	ePokeAnimeEdit_Sex,			///< 性別
	ePokeAnimeEdit_Col,			///< カラー
	ePokeAnimeEdit_Vo_wait,		///< ボイスウェイト			新
	ePokeAnimeEdit_Pat,			///< パターン
	ePokeAnimeEdit_Pat_ox,		///< パターン Offset X		新
	ePokeAnimeEdit_Pat_oy,		///< パターン Offset Y		新
	ePokeAnimeEdit_Flm,			///< フォルムＮｏ．
	
	ePokeAnimeEdit_oy,			///< Offset Y
	ePokeAnimeEdit_Shadow,		///< 影タイプ
	ePokeAnimeEdit_Shadow_ox,	///< 影 Offset X
	
	
	ePokeAnimeEdit_BG_R,		///< 背景色 R
	ePokeAnimeEdit_BG_G,		///< 背景色 G
	ePokeAnimeEdit_BG_B,		///< 背景色 B
};
	



//==============================================================================================
/**
 * @file	poke_test.h
 * @brief	ポケモングラフィックチェック
 * @author	sogabe
 * @date	2005.11.08
 */
//==============================================================================================
#if 0
//==============================================================================================
//
//	extern宣言
//
//==============================================================================================
extern void PokeGraAnimeProcGet(void);


//==============================================================================================
//
//	定義
//
//==============================================================================================
enum{
	SEQ_POKE_TEST_INIT=0,
	SEQ_POKE_TEST_MAIN,
	SEQ_POKE_TEST_END
};
#endif



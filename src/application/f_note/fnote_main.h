//============================================================================================
/**
 * @file	fnote_main.h
 * @brief	冒険ノート　表示メイン処理
 * @author	Hiroyuki Nakamura
 * @date	2006.03.06
 */
//============================================================================================
#ifndef FNOTE_MAIN_H
#define FNOTE_MAIN_H
#undef GLOBAL
#ifdef FNOTE_MAIN_H_GLOBAL
#define GLOBAL	/*	*/
#else
#define GLOBAL	extern
#endif


//============================================================================================
//	定数定義
//============================================================================================
// BMPウィンドウインデックス
enum {
	WIN_TIME1 = 0,		// 日付１
	WIN_STR1,			// 情報１
	WIN_TIME2,			// 日付２
	WIN_STR2,			// 情報２
	WIN_MAX
};

// 冒険ノートワーク
typedef struct {
	GF_BGL_INI * bgl;				// BGLデータ

	GF_BGL_BMPWIN	win[WIN_MAX];	// BMPウィンドウデータ

	FNOTE_DATA * dat;			// 冒険ノートデータ
	MYSTATUS * myst;			// プレーヤーデータ
	SAVEDATA * sv;				// ライバル名取得のため・・・

	MSGDATA_MANAGER * mman;		// メッセージデータマネージャ
	WORDSET * wset;				// 単語セット
	STRBUF * msg_buf;			// テンポラリメッセージバッファ

//	TCB_PTR		vtask;


	u16	chr_scrn[32*32];		// 背景BG保存場所
	u16	bmp_scrn[32*32];		// BMPのBG保存場所

//	BOOL	wipe_flg;

	s8	page;					// 現在のページ
	s8	page_chg_mv;			// ページ切り替え方向
	s8	page_chg_seq;			// ページ切り替えシーケンス
	s8	page_chg_cnt;			// ページ切り替えカウンタ
	s8	page_write_flg;			// ページ書き込みフラグ

	u8	now_chr_frm;			// 現在表示されているページのBGスクリーン
	u8	now_bmp_frm;			// 現在表示されているページの文字スクリーン
	u8	new_chr_frm;			// 新しく表示するページのBGスクリーン
	u8	new_bmp_frm;			// 新しく表示するページの文字スクリーン

}FNOTE_WORK;



//============================================================================================
//	プロトタイプ宣言
//============================================================================================




#undef GLOBAL
#endif	/* FNOTE_MAIN_H */

//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wf2dmap_scrdraw.c
 *	@brief		スクロール描画設定処理
 *	@author		tomoya takahshi
 *	@data		2007.03.19
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"

#define __WF2DMAP_SCRDRAW_H_GLOBAL
#include "application/wifi_2dmap/wf2dmap_scrdraw.h"

//-----------------------------------------------------------------------------
/**
 *					コーディング規約
 *		●関数名
 *				１文字目は大文字それ以降は小文字にする
 *		●変数名
 *				・変数共通
 *						constには c_ を付ける
 *						staticには s_ を付ける
 *						ポインタには p_ を付ける
 *						全て合わさると csp_ となる
 *				・グローバル変数
 *						１文字目は大文字
 *				・関数内変数
 *						小文字と”＿”と数字を使用する 関数の引数もこれと同じ
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
#define WF2DMAP_SCRDRAW_SCRNSIZE_X	(33)	// スクロール時書き込むスクリーンサイズ
#define WF2DMAP_SCRDRAW_SCRNSIZE_Y	(25)	// スクロール時書き込むスクリーンサイズ

//-------------------------------------
///	スクリーンタイプ
//=====================================
enum {
	WF2DMAP_SCRNTYPE_256x256,
	WF2DMAP_SCRNTYPE_512x256,
	WF2DMAP_SCRNTYPE_256x512,
	WF2DMAP_SCRNTYPE_512x512,
	WF2DMAP_SCRNTYPE_NUM,
} ;


//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	スクロール描画ワーク
//=====================================
typedef struct _WF2DMAP_SCRDRAW {
	CLACT_U_EASYRENDER_DATA* p_render;
	GF_BGL_INI* p_bgl;
	void* p_scrnbuff;
	NNSG2dScreenData* p_scrn;
	u32 surface_type;
	u32 bg_frame;
	u16 now_scrn_x;	// 現在のスクリーン書き出し先
	u16 now_scrn_y;	// 現在のスクリーン書き出し先
}WF2DMAP_SCRDRAW;


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
static void WF2DMAP_OBJDrawSysBGCNTSet( GF_BGL_INI* p_bgl, int bg_frame, const WF2DMAP_SCRDRAWINIT* cp_data );
static void WF2DMAP_OBJDrawSysBGScrSet( WF2DMAP_SCRDRAW* p_sys, s16 scr_x, s16 scr_y );
static void WF2DMAP_OBJDrawSysBGScrnWrite( GF_BGL_INI* p_bgl, int bg_frame, const NNSG2dScreenData* cp_scrn, s16 scrn_x, s16 scrn_y );

static void WF2DMAP_OBJDrawSysScrnDraw( GF_BGL_INI * ini, u8 frmnum, u8 write_px, u8 write_py, u8 write_sx, u8 write_sy, const void * buf, u8 buf_px, u8 buf_py, u8 buf_sx, u8 buf_sy );
static u8 WF2DMAP_OBJDrawSysScrnTypeGet( u8 scrn_siz_x, u8 scrn_siz_y );
static const void* WF2DMAP_OBJDrawSysScrnBuffPtrGet( const u8* p_buf, u8 scrnblck_x, u8 scrnblck_y, u8 scrntype, u8 scrn_siz_x, u8 scrn_siz_y, WF2DMAP_POS* p_siz );

//----------------------------------------------------------------------------
/**
 *	@brief	スクロール描画システム作成
 *
 *	@param	p_render			レンダラーデータ
 *	@param	p_bgl				ＧＦ＿ＢＧＬデータ
 *	@param	cp_scrn				スクリーンデータ
 *	@param	cp_init				初期化データ
 *	@param	heapID				ヒープＩＤ
 *
 *	@return	スクロール描画
 *
 */
//-----------------------------------------------------------------------------
WF2DMAP_SCRDRAW* WF2DMAP_SCRDrawSysInit( CLACT_U_EASYRENDER_DATA* p_render, GF_BGL_INI* p_bgl, const WF2DMAP_SCRDRAWINIT* cp_init, u32 heapID )
{
	WF2DMAP_SCRDRAW* p_sys;

	p_sys = sys_AllocMemory( heapID, sizeof(WF2DMAP_SCRDRAW) );
	memset( p_sys, 0, sizeof(WF2DMAP_SCRDRAW) );
	p_sys->p_render = p_render;
	p_sys->p_bgl = p_bgl;
	p_sys->surface_type = cp_init->surface_type;
	p_sys->bg_frame = cp_init->bg_frame;

	// BGCONT設定
	WF2DMAP_OBJDrawSysBGCNTSet( p_sys->p_bgl, p_sys->bg_frame, cp_init );

	// スクリーングラフィック読み込み
	p_sys->p_scrnbuff = ArcUtil_ScrnDataGet( cp_init->arcid_scrn, cp_init->dataid_scrn,
			cp_init->arcdata_comp, &p_sys->p_scrn, heapID );

	return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief	スクロール描画システム破棄
 *
 *	@param	p_sys	スクロール描画システム
 */
//-----------------------------------------------------------------------------
void WF2DMAP_SCRDrawSysExit( WF2DMAP_SCRDRAW* p_sys )
{
	sys_FreeMemoryEz( p_sys->p_scrnbuff );
	sys_FreeMemoryEz( p_sys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	スクロールメイン
 *
 *	@param	p_sys		スクロール描画システム
 *	@param	cp_scroll	スクロールデータ
 */
//-----------------------------------------------------------------------------
void WF2DMAP_SCRDrawSysMain( WF2DMAP_SCRDRAW* p_sys, const WF2DMAP_SCROLL* cp_scroll )
{
	s16 scr_x;
	s16 scr_y;
	
	// スクロール座標からスクロール座標を各表示システムに設定
	scr_y = WF2DMAP_SCROLLSysTopGet( cp_scroll );	
	scr_x = WF2DMAP_SCROLLSysLeftGet( cp_scroll );	

	// サーフェース
	if( p_sys->surface_type == CLACT_U_EASYRENDER_SURFACE_MAIN ){
		CLACT_U_SetMainSurfaceMatrix( p_sys->p_render, FX32_CONST(scr_x), FX32_CONST(scr_y) );
	}else{
		CLACT_U_SetSubSurfaceMatrix( p_sys->p_render, FX32_CONST(scr_x), FX32_CONST(scr_y)+SUB_SURFACE_Y );
	}

	// BGL
	// スクロール座標からスクリーンデータを書き込んで転送
	WF2DMAP_OBJDrawSysBGScrSet( p_sys,  scr_x, scr_y );
}


//-----------------------------------------------------------------------------
/**
 *			プライベート
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	スクロールするＢＧ面のＢＧコントロール設定
 *
 *	@param	p_bgl		ＢＧＬ
 *	@param	bg_frame	ＢＧフレーム
 *	@param	cp_data		初期化データ
 */
//-----------------------------------------------------------------------------
static void WF2DMAP_OBJDrawSysBGCNTSet( GF_BGL_INI* p_bgl, int bg_frame, const WF2DMAP_SCRDRAWINIT* cp_data )
{
	GF_BGL_BGCNT_HEADER TextBgCntDat = {
		0, 0, 0x1000, 0, GF_BGL_SCRSIZ_512x256,
	};

	TextBgCntDat.colorMode = cp_data->colorMode;
	TextBgCntDat.screenBase = cp_data->screenBase;
	TextBgCntDat.charBase = cp_data->charbase;
	TextBgCntDat.bgExtPltt = cp_data->bgExtPltt;
	TextBgCntDat.priority = cp_data->priority;
	TextBgCntDat.areaOver = 0;
	TextBgCntDat.mosaic = cp_data->mosaic;

	GF_BGL_BGControlExit( p_bgl, bg_frame );
	GF_BGL_BGControlSet( p_bgl, bg_frame, &TextBgCntDat, GF_BGL_MODE_TEXT );
//	GF_BGL_ScrClear( p_bgl, bg_frame );
}	

//----------------------------------------------------------------------------
/**
 *	@brief	BG面のスクロール設定
 *
 *	@param	p_sys		システム
 *	@param	scr_x		スクロールＸ座標
 *	@param	scr_y		スクロールＹ座標
 */
//-----------------------------------------------------------------------------
static void WF2DMAP_OBJDrawSysBGScrSet( WF2DMAP_SCRDRAW* p_sys, s16 scr_x, s16 scr_y )
{
	s16 scrn_x;	// スクリーン位置
	s16 scrn_y;	
	s16 ofs_x;	// スクロール座標
	s16 ofs_y;

	// スクリーン位置とスクロール座標を求める
	scrn_x = scr_x / 8;
	scrn_y = scr_y / 8;
	ofs_x = scr_x % 8;	
	ofs_y = scr_y % 8;

	// スクリーン位置が変わらないときはスクリーンの書き換えを行わない
	if( (scrn_x != p_sys->now_scrn_x) || (scrn_y != p_sys->now_scrn_y) ){
		p_sys->now_scrn_x = scrn_x;
		p_sys->now_scrn_y = scrn_y;
		WF2DMAP_OBJDrawSysBGScrnWrite( p_sys->p_bgl, p_sys->bg_frame, p_sys->p_scrn, -scrn_x, -scrn_y );
		GF_BGL_LoadScreenV_Req( p_sys->p_bgl, p_sys->bg_frame );
	}

	// スクロール座標を設定
	GF_BGL_ScrollReq( p_sys->p_bgl, p_sys->bg_frame, GF_BGL_SCROLL_X_SET, ofs_x );
	GF_BGL_ScrollReq( p_sys->p_bgl, p_sys->bg_frame, GF_BGL_SCROLL_Y_SET, ofs_y );
}

//----------------------------------------------------------------------------
/**
 *	@brief	スクリーンデータ書き込み
 *
 *	@param	p_bgl		ＢＧデータ
 *	@param	bg_frame	ＢＧフレーム
 *	@param	cp_scrn		スクリーンデータ
 *	@param	scrn_x		スクリーンデータの書き出し位置
 *	@param	scrn_y		スクリーンデータの書き出し位置
 */
//-----------------------------------------------------------------------------
static void WF2DMAP_OBJDrawSysBGScrnWrite( GF_BGL_INI* p_bgl, int bg_frame, const NNSG2dScreenData* cp_scrn, s16 scrn_x, s16 scrn_y )
{
	s16 write_siz_x, write_siz_y;
	s16 read_x, read_y;
	s16 write_x, write_y;
	s16 scrndata_sizx, scrndata_sizy;

	// スクリーンデータサイズ取得
	scrndata_sizx = (cp_scrn->screenWidth/8);
	scrndata_sizy = (cp_scrn->screenHeight/8);
	

	// 読み込み先設定
	if( scrn_x < 0 ){
		read_x = -scrn_x;	
	}else{
		read_x = 0;
	}
	if( scrn_y < 0 ){
		read_y = -scrn_y;	
	}else{
		read_y = 0;
	}

	// 書き込み先設定
	if( scrn_x > 0 ){
		write_x = scrn_x;
	}else{
		write_x = 0;
	}
	if( scrn_y > 0 ){
		write_y = scrn_y;
	}else{
		write_y = 0;
	}

	// スクリーンデータに書き込むほどのサイズがあるのかチェック
	write_siz_x = WF2DMAP_SCRDRAW_SCRNSIZE_X - write_x;
	write_siz_y = WF2DMAP_SCRDRAW_SCRNSIZE_Y - write_y;
	if( scrndata_sizx < (read_x+write_siz_x) ){
		write_siz_x -= (read_x+write_siz_x) - scrndata_sizx;
	}
	if( scrndata_sizy < (read_y+write_siz_y) ){
		write_siz_y -= (read_y+write_siz_y) - scrndata_sizy;
	}

	// スクリーンクリーン
	GF_BGL_ScrFill( p_bgl, bg_frame, 0, 
			0, 0, 
			WF2DMAP_SCRDRAW_SCRNSIZE_X, WF2DMAP_SCRDRAW_SCRNSIZE_Y, GF_BGL_SCRWRT_PALIN );
	
	// 立てに２スクリーンに分けなくてはいけないかチェック
	WF2DMAP_OBJDrawSysScrnDraw(
			p_bgl, bg_frame,
			write_x, write_y,				//  書き込み開始位置
			write_siz_x,					// 書き込みサイズ 
			write_siz_y,					// 書き込みサイズ
			cp_scrn->rawData,				// スクリーンデータ
			read_x, read_y,					// 書き込み開始位置
			scrndata_sizx, scrndata_sizy	// スクリーンサイズ
			);
}


//----------------------------------------------------------------------------
/**
 *	@brief	スクリーンデータ描画
 *
 *	@param	p_bgl			BGL
 *	@param	bg_frame		フレーム
 *	@param	write_x			書き込み開始　ｘキャラ
 *	@param	write_y			書き込み開始　ｙキャラ
 *	@param	write_siz_x		書き込みサイズ
 *	@param	write_siz_y		書き込みサイズ
 *	@param	buf				スクリーンデータ
 *	@param	read_x			読み込み開始　ｘキャラ
 *	@param	read_y			読み込み開始　ｙキャラ
 *	@param	buf_sx			スクリーンデータ　ｘキャラサイズ
 *	@param	buf_sy			スクリーンデータ　ｙキャラサイズ
 */
//-----------------------------------------------------------------------------
static void WF2DMAP_OBJDrawSysScrnDraw( GF_BGL_INI * p_bgl, u8 bg_frame, u8 write_x, u8 write_y, u8 write_siz_x, u8 write_siz_y, const void * buf, u8 read_x, u8 read_y, u8 buf_sx, u8 buf_sy )
{
	s8 ws_x, ws_y;	// 書き込んだサイズ
	s8 nws_x, nws_y;// 次の書き込むサイズ
	const u16* write_buf;
	WF2DMAP_POS siz;	// バッファサイズ
	s8 scrnblck_x, scrnblck_y;
	u8 scrn_type;
	s8 reedw_x, writew_x;	// 読み込み書き込み先

	scrn_type = WF2DMAP_OBJDrawSysScrnTypeGet( buf_sx, buf_sy );

//	OS_Printf( "BUFF_size x[%d] y[%d] scrntype[%d]\n", buf_sx, buf_sy, scrn_type );
	
	// バッファが３２＊３２内に入っているならそのまま書き込む
	if( scrn_type == WF2DMAP_SCRNTYPE_256x256 ){
		GF_BGL_ScrWriteExpand(
				p_bgl, bg_frame,
				write_x, write_y,				// 書き込み開始位置
				write_siz_x,					// 書き込みサイズ 
				write_siz_y,					// 書き込みサイズ
				buf,							// スクリーンデータ
				read_x, read_y,					// 書き込み開始位置
				buf_sx, buf_sy					// スクリーンサイズ
			);
		return ;
	}

	// 縦方向書き込みループ
	ws_y = write_siz_y;
	while( ws_y > 0 ){
		
		if( (((read_y)%32) + ws_y) <= 32 ){	// Y方向書き込みが３２以上になるかチェック
			// 以下なら１回でかける
			nws_y = ws_y;
			ws_y = 0;
		}else{
			// 大きいなら、今かけるだけ書く
			nws_y = 32 - (read_y%32);
			ws_y -= nws_y;
		}

		// スクリーンブロック取得
		scrnblck_y = read_y / 32;

		ws_x = write_siz_x;
		reedw_x = read_x;
		writew_x = write_x;
		// X方向書き込みループ
		while( ws_x > 0 ){
			if( (((reedw_x)%32) + ws_x) <= 32 ){	// Y方向書き込みが３２以上になるかチェック
				// 以下なら１回でかける
				nws_x = ws_x;
				ws_x = 0;
			}else{
				// 大きいなら、今かけるだけ書く
				nws_x = 32 - (reedw_x%32);
				ws_x -= nws_x;
			}

			// スクリーンブロック取得
			scrnblck_x = reedw_x / 32;

			// 書き込み
			write_buf = WF2DMAP_OBJDrawSysScrnBuffPtrGet( buf, scrnblck_x, scrnblck_y, scrn_type,  buf_sx, buf_sy, &siz );

			/*
			OS_Printf( "writew_x[%d] write_y[%d]\n", writew_x, write_y );
			OS_Printf( "nws_x[%d] nws_y[%d]\n", nws_x, nws_y );
			OS_Printf( "reedw_x[%d] read_y[%d]\n", reedw_x%32, read_y%32 );
			OS_Printf( "siz_x[%d] siz_y[%d]\n", siz.x, siz.y );
			//*/

			GF_BGL_ScrWriteExpand(
					p_bgl, bg_frame,
					writew_x, write_y,				//  書き込み開始位置
					nws_x,							// 書き込みサイズ 
					nws_y,							// 書き込みサイズ
					write_buf,						// スクリーンデータ
					reedw_x%32, read_y%32,			// 書き込み開始位置
					siz.x, siz.y					// スクリーンサイズ
				);

			// 読み込み先を動かす
			reedw_x += nws_x;

			// 書き込み先を動かす
			writew_x += nws_x;
		}

		// 読み込み先を動かす
		read_y += nws_y;

		// 書き込み先を動かす
		write_y += nws_y;
	}

	OS_Printf( "\n" );
}

//----------------------------------------------------------------------------
/**
 *	@brief	スクリーンタイプ取得
 *
 *	@param	scrn_siz_x		スクリーンサイズｘ
 *	@param	scrn_siz_y		スクリーンサイズｙ
 *
 *	@return	スクリーンタイプ
 */
//-----------------------------------------------------------------------------
static u8 WF2DMAP_OBJDrawSysScrnTypeGet( u8 scrn_siz_x, u8 scrn_siz_y )
{
	GF_ASSERT( scrn_siz_x <= 64 );
	GF_ASSERT( scrn_siz_y <= 64 );
	
	if( scrn_siz_x <= 32 ){
		if( scrn_siz_y <= 32 ){
			return WF2DMAP_SCRNTYPE_256x256;
		}else{
			return WF2DMAP_SCRNTYPE_256x512;
		}
	}else{
		if( scrn_siz_y <= 32 ){
			return WF2DMAP_SCRNTYPE_512x256;
		}else{
			return WF2DMAP_SCRNTYPE_512x512;
		}
	}
	GF_ASSERT(0);
	return 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	スクリーンブロックナンバーからスクリーンの先頭ポインタを取得
 *
 *	@param	p_buf			バッファ
 *	@param	scrnblck_x		スクリーンブロックナンバーｘ
 *	@param	scrnblck_y		スクリーンブロックナンバーｙ
 *	@param	scrntype		スクリーンタイプ
 *
 *	@return	そのブロックのバッファの先頭バッファ
 */
//-----------------------------------------------------------------------------
static const void* WF2DMAP_OBJDrawSysScrnBuffPtrGet( const u8* p_buf, u8 scrnblck_x, u8 scrnblck_y, u8 scrntype, u8 scrn_siz_x, u8 scrn_siz_y, WF2DMAP_POS* p_siz )
{
	s16 one_blck_siz;
	u16 idx;

	// 今のところ、いろんなスクリーンサイズに対応しています。
	// ただ分岐などが多くなってしまうため、もし速度的に問題があれば、
	// 絶対に256の倍数のスクリーンサイズにするという方法で高速化をする必要があると思います
	
	switch( scrntype ){
	case WF2DMAP_SCRNTYPE_256x256:
		p_siz->x = scrn_siz_x;
		p_siz->y = scrn_siz_y;
		return p_buf;
		
	case WF2DMAP_SCRNTYPE_512x256:
		// スクリーン書き込みサイズ
		if( ((scrnblck_x+1)*32) <= scrn_siz_x ){
			p_siz->x = 32;
		}else{
			p_siz->x = scrn_siz_x - (scrnblck_x*32);
		}
		p_siz->y = scrn_siz_y;

		// 1blckのサイズ
		one_blck_siz = scrn_siz_y * (2*32);
		return &p_buf[ (scrnblck_x * one_blck_siz) ];
		
	case WF2DMAP_SCRNTYPE_256x512:
		// スクリーン書き込みサイズ
		p_siz->x = scrn_siz_x;
		if( ((scrnblck_y+1)*32) <= scrn_siz_y ){
			p_siz->y = 32;
		}else{
			p_siz->y = scrn_siz_y - (scrnblck_y*32);
		}

		// 1blckのサイズ
		one_blck_siz = (scrn_siz_x*2) * 32;
		return &p_buf[ (scrnblck_y * one_blck_siz) ];
		
	case WF2DMAP_SCRNTYPE_512x512:
		// スクリーン書き込みサイズ
		if( ((scrnblck_x+1)*32) <= scrn_siz_x ){
			p_siz->x = 32;
		}else{
			p_siz->x = scrn_siz_x - (scrnblck_x*32);
		}
		if( ((scrnblck_y+1)*32) <= scrn_siz_y ){
			p_siz->y = 32;
		}else{
			p_siz->y = scrn_siz_y - (scrnblck_y*32);
		}

		// バッファの参照先を設定
		// 0 1
		// 2 3
		// とスクリーンは並んでいる
		if( scrnblck_y == 0 ){
			idx = 0;

			if( scrnblck_x > 0 ){
				idx += (32*2) * 32;
			}
		}else{
			idx = (scrn_siz_x*2)*32;

			if( scrnblck_x > 0 ){
				idx += (32*2) *scrn_siz_y;
			}
		}

		return &p_buf[ idx ];

	default:
		GF_ASSERT(0);
		break;
	}

	GF_ASSERT(0);
	return NULL;
}


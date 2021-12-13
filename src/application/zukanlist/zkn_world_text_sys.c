//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		zkn_world_text_sys.c
 *	@brief		図鑑の外国語テキストアクセス
 *	@author		tomoya takahashi 
 *	@data		2006.02.28
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "system.h"
#include "assert.h"
#include <string.h>
#include "include/system/fontproc.h"
#include "include/system/msgdata_util.h"
#include "include/msgdata/msg.naix"

#include "include/application/zukanlist/zkn_defain.h"
#include "include/application/zukanlist/zkn_world_text_data.h"
//#include "src/application/zukanlist/zkn_worldtext.h"

#include "include/application/zukanlist/zkn_version.h"

#define	__ZKN_WORLD_TEXT_SYS_H_GLOBAL
#include "include/application/zukanlist/zkn_world_text_sys.h"

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
#define ZKN_WT_ALLOC_STR_NUM		( 256 )		// 確保する文字列数


//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/**
 *					グローバルデータ
 */
//-----------------------------------------------------------------------------
//-------------------------------------
//	ポケモン外国語テキスト用国コードと
//	実際にGMMファイルに入っているデータの外国語項目数
//	対応データ
//	IDXは外国語テキスト用国コード
//
//	zukan_data.xlsには下の数字の順にﾃﾞｰﾀを格納しておく
//=====================================
// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ALL) imatake 2006/10/05
// 外国語ずかんのテキストを自国分も持った形式に変更

enum {
	TEXTCODE_JAPAN,
	TEXTCODE_ENGLISH,
	TEXTCODE_FRANCE,
	TEXTCODE_GERMANY,
	TEXTCODE_ITALY,
	TEXTCODE_SPAIN
};

#if (PM_LANG == LANG_JAPAN)
#define TEXTCODE_OWN	TEXTCODE_JAPAN
#elif (PM_LANG == LANG_ENGLISH)
#define TEXTCODE_OWN	TEXTCODE_ENGLISH
#elif (PM_LANG == LANG_FRANCE)
#define TEXTCODE_OWN	TEXTCODE_FRANCE
#elif (PM_LANG == LANG_GERMANY)
#define TEXTCODE_OWN	TEXTCODE_GERMANY
#elif (PM_LANG == LANG_ITALY)
#define TEXTCODE_OWN	TEXTCODE_ITALY
#elif (PM_LANG == LANG_SPAIN)
#define TEXTCODE_OWN	TEXTCODE_SPAIN
#endif

static const u8 ZKN_WORLD_TEXT_LangBtnOrder[ZKN_WORLD_TEXT_NUM] = {
#if (PM_LANG == LANG_JAPAN)
	TEXTCODE_JAPAN, TEXTCODE_ENGLISH, TEXTCODE_FRANCE, TEXTCODE_GERMANY, TEXTCODE_ITALY, TEXTCODE_SPAIN
#elif (PM_LANG == LANG_ENGLISH)
	TEXTCODE_ENGLISH, TEXTCODE_FRANCE, TEXTCODE_GERMANY, TEXTCODE_ITALY, TEXTCODE_SPAIN, TEXTCODE_JAPAN
#elif (PM_LANG == LANG_FRANCE)
	TEXTCODE_FRANCE, TEXTCODE_ENGLISH, TEXTCODE_GERMANY, TEXTCODE_ITALY, TEXTCODE_SPAIN, TEXTCODE_JAPAN
#elif (PM_LANG == LANG_GERMANY)
	TEXTCODE_GERMANY, TEXTCODE_ENGLISH, TEXTCODE_FRANCE, TEXTCODE_ITALY, TEXTCODE_SPAIN, TEXTCODE_JAPAN
#elif (PM_LANG == LANG_ITALY)
	TEXTCODE_ITALY, TEXTCODE_ENGLISH, TEXTCODE_FRANCE, TEXTCODE_GERMANY, TEXTCODE_SPAIN, TEXTCODE_JAPAN
#elif (PM_LANG == LANG_SPAIN)
	TEXTCODE_SPAIN, TEXTCODE_ENGLISH, TEXTCODE_FRANCE, TEXTCODE_GERMANY, TEXTCODE_ITALY, TEXTCODE_JAPAN
#endif
};

// ----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
static inline int ZknWt_GetGmmIdx( int zkn_text_code );
static inline BOOL ZknWt_CheckWorldData( int textverpokenum, int gmm_idx );
static STRBUF* ZknWt_GetSTRData( int file_idx, int data_idx, int heap );
static void ZknWt_GetCountryPokeData( int monsno, int country, int* p_country_poke_num, int* p_country_text_code, int* p_country_gmm_idx );

//----------------------------------------------------------------------------
/**
 *	@brief	文字列バッファを破棄する
 *
 *	@param	buf		バッファ
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
void ZKN_WT_DeleteStrBuf( STRBUF* buf )
{
	STRBUF_Delete( buf );
}

//----------------------------------------------------------------------------
/**
 *	@brief	国コードから対応する外国語GMMインデックスを取得
 *
 *	@param	country	国コード
 *
 *	@return	外国語番号
 */
//-----------------------------------------------------------------------------
int ZKN_WT_LANG_Code_WORLD_TEXT_GMM_Idx( int country )
{
	int country_text_code;
	int country_gmm_idx;
	
	// 外国語テキスト用国コードに変換
	country_text_code = ZKN_WT_GetLANG_Code_ZKN_WORLD_TEXT_Code( country );
	GF_ASSERT( country_text_code < ZKN_WORLD_TEXT_NUM );	// 無いということはデータも無い
	// 外国語テキスト用国コードからGMMデータ外国語項目数取得
	country_gmm_idx = ZknWt_GetGmmIdx( country_text_code );

	return country_gmm_idx;
}

//----------------------------------------------------------------------------
/**
 *	@brief	外国語番号から対応する国コードを取得する
 *
 *	@param	county	外国語番号	(gmmファイルに並んでいる番号)
 *
 *	@return	国コード		// 引っかからないときはホームタウンを返す
 */
//-----------------------------------------------------------------------------
int ZKN_WT_WORLD_TEXT_GMM_Idx_LANG_Code( int country )
{
	// ----------------------------------------------------------------------------
	// localize_spec_mark(LANG_ALL) imatake 2006/10/05
	// 外国語ずかんのテキストを自国分も持った形式に変更

	return ZKN_WT_GetZKN_WORLD_TEXT_Code_LANG_Code( ZKN_WORLD_TEXT_LangBtnOrder[country+1] );

	// ----------------------------------------------------------------------------
}


//----------------------------------------------------------------------------
/**
 *	@brief	国のポケモン名を返す
 *
 *	@param	monsno		モンスターナンバー
 *	@param	country		ゲーム内国コード
 *	@param	heap		ヒープ
 *
 *	@return	文字列データ
 */
//-----------------------------------------------------------------------------
STRBUF* ZKN_WT_GetPokeName( int monsno, int country, int heap )
{
	int country_poke_num;
	int country_text_code;
	int country_gmm_idx;
	int gmm_file_idx;

	// モンスターナンバーとゲーム内国コードから
	// 外国語テキストデータ項目数
	// 外国語テキストデータ用国コード
	// 外国語テキストデータGMM外国項目数取得
	ZknWt_GetCountryPokeData( monsno, country, &country_poke_num, &country_text_code, &country_gmm_idx );

	// 自国チェック
	if( country_gmm_idx == ZKN_WORLD_TEXT_NUM ){
		// 自国
		// GMMの中のデータidx＝monsno
		// ----------------------------------------------------------------------------
		// localize_spec_mark(LANG_ALL) imatake 2007/01/19
		// ポケモン名はすべて全大文字に
		return MSGDAT_UTIL_GetMonsName( monsno, heap );
		// ----------------------------------------------------------------------------
	}else{
		// 外国
		// GMMの中のデータインデックスを求める
        const int gmm_file_idx_tbl[] = {
            NARC_msg_zkn_worldnamejp_dat,
            NARC_msg_zkn_worldname_dat,
            NARC_msg_zkn_worldname01_dat,
            NARC_msg_zkn_worldname02_dat,
            NARC_msg_zkn_worldname03_dat,
            NARC_msg_zkn_worldname04_dat,
        };
		gmm_file_idx = gmm_file_idx_tbl[country_gmm_idx];
		country_gmm_idx = country_poke_num;
	}

	// gmm_file_idxとcountry_gmm_idxからSTRBUFを取得する
	return ZknWt_GetSTRData( gmm_file_idx, country_gmm_idx, heap );
}

//----------------------------------------------------------------------------
/**
 *	@brief	国のポケモンタイプを返す
 *
 *	@param	monsno		モンスターナンバー
 *	@param	country		ゲーム内国コード
 *	@param	heap		ヒープ
 *
 *	@return	文字列データ
 */
//-----------------------------------------------------------------------------
STRBUF* ZKN_WT_GetPokeType( int monsno, int country, int heap )
{
	int country_poke_num;
	int country_text_code;
	int country_gmm_idx;
	int gmm_file_idx;

	// モンスターナンバーとゲーム内国コードから
	// 外国語テキストデータ項目数
	// 外国語テキストデータ用国コード
	// 外国語テキストデータGMM外国項目数取得
	ZknWt_GetCountryPokeData( monsno, country, &country_poke_num, &country_text_code, &country_gmm_idx );

	// 自国チェック
	if( country_gmm_idx == ZKN_WORLD_TEXT_NUM ){
		// 自国
		// GMMの中のデータidx＝monsno
		country_gmm_idx = monsno;
		gmm_file_idx = NARC_msg_zkn_type_dat;
	}else{
		// 外国
		// GMMの中のデータインデックスを求める
        const int gmm_file_idx_tbl[] = {
            NARC_msg_zkn_worldtypejp_dat,
            NARC_msg_zkn_worldtype_dat,
            NARC_msg_zkn_worldtype01_dat,
            NARC_msg_zkn_worldtype02_dat,
            NARC_msg_zkn_worldtype03_dat,
            NARC_msg_zkn_worldtype04_dat,
        };
		gmm_file_idx = gmm_file_idx_tbl[country_gmm_idx];
		country_gmm_idx = country_poke_num;
	}

	// gmm_file_idxとcountry_gmm_idxからSTRBUFを取得する
	return ZknWt_GetSTRData( gmm_file_idx, country_gmm_idx, heap );
}

//----------------------------------------------------------------------------
/**
 *	@brief	国のテキストデータを返す
 *
 *	@param	monsno		モンスターナンバー
 *	@param	country		ゲーム内国コード
 *	@param	page		ページ
 *	@param	heap		ヒープ
 *	
 *	@return	文字列データ
 */
//-----------------------------------------------------------------------------
STRBUF* ZKN_WT_GetText( int monsno, int country, int page, int heap )
{
	int country_poke_num;
	int country_text_code;
	int country_gmm_idx;
	int gmm_file_idx;

	// モンスターナンバーとゲーム内国コードから
	// 外国語テキストデータ項目数
	// 外国語テキストデータ用国コード
	// 外国語テキストデータGMM外国項目数取得
	ZknWt_GetCountryPokeData( monsno, country, &country_poke_num, &country_text_code, &country_gmm_idx );

	// 自国チェック
	if( country_gmm_idx == ZKN_WORLD_TEXT_NUM ){

		// ページ数チェック
		GF_ASSERT( page < ZKN_WT_HOME_TEXT_PAGE_NUM );
		// 自国
		// GMMの中のデータidx＝monsno
		country_gmm_idx = (monsno * ZKN_WT_HOME_TEXT_PAGE_NUM) + page;
		gmm_file_idx = ZKN_TEXT_DAT;
	}else{
        const int gmm_file_idx_tbl[] = {
            NARC_msg_zkn_comment_jp_dat,
            NARC_msg_zkn_comment_02_dat,
            NARC_msg_zkn_comment_03_dat,
            NARC_msg_zkn_comment_04_dat,
            NARC_msg_zkn_comment_05_dat,
            NARC_msg_zkn_comment_06_dat,
        };
		// ページ数チェック
		GF_ASSERT( page < ZKN_WT_WORLD_TEXT_PAGE_NUM );
		// 外国
		// GMMの中のデータインデックスを求める
		gmm_file_idx = gmm_file_idx_tbl[country_gmm_idx];
		country_gmm_idx = (country_poke_num * ZKN_WT_WORLD_TEXT_PAGE_NUM) + page;
	}

	// gmm_file_idxとcountry_gmm_idxからSTRBUFを取得する
	return ZknWt_GetSTRData( gmm_file_idx, country_gmm_idx, heap );
}

//-----------------------------------------------------------------------------
/**
 *			プライベート関数
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief	関数での戻り値[外国語GMM内のデータ項目][GMMデータ外国語項目数]からそのデータがあるのかチェックする
 *
 *	@param	textverpokenum
 *	@param	gmm_idx 
 *
 *	@retval	TRUE	データがある
 *	@retval	FALSE	そのポケモンのその国のデータはない
 */
//-----------------------------------------------------------------------------
static inline BOOL ZknWt_CheckWorldData( int textverpokenum, int gmm_idx )
{
	// 自国のデータを求めていないのにそのポケモンの外国語データが無いとき
	// はデータがないということになる
	if( (textverpokenum > MONSNO_END) && (gmm_idx != ZKN_WORLD_TEXT_NUM) ){
		return FALSE;
	}

	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ファイルインデックスから文字列データを取得する
 *
 *	@param	file_idx
 *	@param	data_idx
 *	@param	heap 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static STRBUF* ZknWt_GetSTRData( int file_idx, int data_idx, int heap )
{
	MSGDATA_MANAGER* man = MSGMAN_Create( MSGMAN_TYPE_DIRECT, ARC_MSG, file_idx, heap );
	if( man )
	{
		STRBUF* buf = STRBUF_Create(ZKN_WT_ALLOC_STR_NUM, heap );
		if( buf )
		{
			MSGMAN_GetString(man, data_idx, buf);
		}
		MSGMAN_Delete(man);
		return buf;
	}
	return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	モンスターナンバーとゲーム内国コードから
 *			外国語テキストデータ項目数
 *			外国語テキストデータ用国コード
 *			外国語テキストデータGMM外国項目数
 *			取得
 *
 *	@param	monsno				モンスターナンバー
 *	@param	country				ゲーム内国コード
 *	@param	p_country_poke_num	外国語テキストデータ項目数
 *	@param	p_country_text_code	外国語テキストデータ用国コード
 *	@param	p_country_gmm_idx	外国語テキストデータGMM外国項目数
 */
//-----------------------------------------------------------------------------
static void ZknWt_GetCountryPokeData( int monsno, int country, int* p_country_poke_num, int* p_country_text_code, int* p_country_gmm_idx )
{
	// 外国語テキスト用国コードに変換
	*p_country_text_code = ZKN_WT_GetLANG_Code_ZKN_WORLD_TEXT_Code( country );
	GF_ASSERT( *p_country_text_code < ZKN_WORLD_TEXT_NUM );	// 無いということはデータも無い
	// モンスターナンバーから外国語GMM内のデータ項目数を求める
//	*p_country_poke_num = ZKN_WT_GetMonsNo_TEXTVERPokeNum( monsno );
	*p_country_poke_num = monsno;	// 全員分保持してある

	// 外国語テキスト用国コードからGMMデータ外国語項目数取得
	*p_country_gmm_idx = ZknWt_GetGmmIdx( *p_country_text_code );

	// データがあるかチェック
	GF_ASSERT( ZknWt_CheckWorldData( *p_country_poke_num, *p_country_gmm_idx ) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	図鑑外国語テキスト用国コードから外国語テキストGMMデータの外国語項目数を取得
 *
 *	@param	zkn_text_code	図鑑外国語テキスト用国コード
 *
 *	@retval	外国語テキストGMMデータ外国語項目数
 *	@retval ZKN_WORLD_TEXT_NUMの時は自国GMMにデータがある
 */
//-----------------------------------------------------------------------------
static inline int ZknWt_GetGmmIdx( int zkn_text_code )
{
	GF_ASSERT( zkn_text_code < ZKN_WORLD_TEXT_NUM );
	
	// ----------------------------------------------------------------------------
	// localize_spec_mark(LANG_ALL) imatake 2006/10/05
	// 外国語ずかんのテキストを自国分も持った形式に変更
	return zkn_text_code == TEXTCODE_OWN ? ZKN_WORLD_TEXT_NUM : zkn_text_code;
	// ---------------------------------------------------------------------------
}


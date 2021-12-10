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
#ifdef NONEQUIVALENT
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
		gmm_file_idx = NARC_msg_zkn_worldname_dat + country_gmm_idx;
		country_gmm_idx = country_poke_num;
	}

	// gmm_file_idxとcountry_gmm_idxからSTRBUFを取得する
	return ZknWt_GetSTRData( gmm_file_idx, country_gmm_idx, heap );
}
#else

static const u32 sOv21_21E9CCC[] = {
    0x2cd,
    0x2c8,
    0x2c9,
    0x2ca,
    0x2cb,
    0x2cc
};

asm STRBUF* ZKN_WT_GetPokeName( int monsno, int country, int heap )
{
	push {r4, r5, r6, lr}
	sub sp, #0x28
	add r4, r2, #0
	add r2, sp, #4
	str r2, [sp]
	add r2, sp, #0xc
	add r3, sp, #8
	add r5, r0, #0
	bl ZknWt_GetCountryPokeData
	ldr r2, [sp, #4]
	cmp r2, #6
	bne _021D5642
	add r0, r5, #0
	add r1, r4, #0
	bl MSGDAT_UTIL_GetMonsName
	add sp, #0x28
	pop {r4, r5, r6, pc}
_021D5642:
	ldr r6, =sOv21_21E9CCC // _021D5668
	add r5, sp, #0x10
	add r3, r5, #0
	ldmia r6!, {r0, r1}
	stmia r5!, {r0, r1}
	ldmia r6!, {r0, r1}
	stmia r5!, {r0, r1}
	ldmia r6!, {r0, r1}
	stmia r5!, {r0, r1}
	lsl r0, r2, #2
	ldr r1, [sp, #0xc]
	ldr r0, [r3, r0]
	add r2, r4, #0
	str r1, [sp, #4]
	bl ZknWt_GetSTRData
	add sp, #0x28
	pop {r4, r5, r6, pc}
	nop
// _021D5668: .4byte 0x021E9CCC
}
#endif

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
#ifdef NONEQUIVALENT
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
		gmm_file_idx = NARC_msg_zkn_worldtype_dat + country_gmm_idx;
		country_gmm_idx = country_poke_num;
	}

	// gmm_file_idxとcountry_gmm_idxからSTRBUFを取得する
	return ZknWt_GetSTRData( gmm_file_idx, country_gmm_idx, heap );
}
#else
static const u32 sOv21_21E9CE4[] = {
    0x2d3,
    0x2ce,
    0x2cf,
    0x2d0,
    0x2d1,
    0x2d2
};

asm STRBUF* ZKN_WT_GetPokeType( int monsno, int country, int heap )
{
	push {r4, r5, r6, lr}
	sub sp, #0x28
	add r4, r2, #0
	add r2, sp, #4
	str r2, [sp]
	add r2, sp, #0xc
	add r3, sp, #8
	add r5, r0, #0
	bl ZknWt_GetCountryPokeData
	ldr r2, [sp, #4]
	cmp r2, #6
	bne _021D568C
	str r5, [sp, #4]
	ldr r0, =0x000002C7 // _021D56B4
	b _021D56A6
_021D568C:
	ldr r6, =sOv21_21E9CE4 // _021D56B8
	add r5, sp, #0x10
	add r3, r5, #0
	ldmia r6!, {r0, r1}
	stmia r5!, {r0, r1}
	ldmia r6!, {r0, r1}
	stmia r5!, {r0, r1}
	ldmia r6!, {r0, r1}
	stmia r5!, {r0, r1}
	lsl r0, r2, #2
	ldr r1, [sp, #0xc]
	ldr r0, [r3, r0]
	str r1, [sp, #4]
_021D56A6:
	ldr r1, [sp, #4]
	add r2, r4, #0
	bl ZknWt_GetSTRData
	add sp, #0x28
	pop {r4, r5, r6, pc}
}
#endif

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
#ifdef NONEQUIVALENT
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

		// ページ数チェック
		GF_ASSERT( page < ZKN_WT_WORLD_TEXT_PAGE_NUM );
		// 外国
		// GMMの中のデータインデックスを求める
		gmm_file_idx = NARC_msg_zkn_comment_02_dat + country_gmm_idx;
		country_gmm_idx = (country_poke_num * ZKN_WT_WORLD_TEXT_PAGE_NUM) + page;
	}

	// gmm_file_idxとcountry_gmm_idxからSTRBUFを取得する
	return ZknWt_GetSTRData( gmm_file_idx, country_gmm_idx, heap );
}
#else
static const u32 sOv21_21E9CFC[] = {
    0x2c1,
    0x2bc,
    0x2bd,
    0x2be,
    0x2bf,
    0x2c0
};

asm STRBUF* ZKN_WT_GetText( int monsno, int country, int page, int heap )
{
	push {r4, r5, r6, lr}
	sub sp, #0x28
	add r4, r2, #0
	add r2, sp, #4
	add r6, r3, #0
	str r2, [sp]
	add r2, sp, #0xc
	add r3, sp, #8
	add r5, r0, #0
	bl ZknWt_GetCountryPokeData
	ldr r0, [sp, #4]
	cmp r0, #6
	bne _021D56E8
	cmp r4, #1
	blt _021D56E0
	bl GF_AssertFailedWarningCall
_021D56E0:
	add r0, r5, r4
	str r0, [sp, #4]
	ldr r0, =0x000002C2 // _021D571C
	b _021D570E
_021D56E8:
	ldr r3, =sOv21_21E9CFC // _021D5720
	add r2, sp, #0x10
	ldmia r3!, {r0, r1}
	stmia r2!, {r0, r1}
	ldmia r3!, {r0, r1}
	stmia r2!, {r0, r1}
	ldmia r3!, {r0, r1}
	stmia r2!, {r0, r1}
	cmp r4, #1
	blt _021D5700
	bl GF_AssertFailedWarningCall
_021D5700:
	ldr r0, [sp, #4]
	lsl r1, r0, #2
	add r0, sp, #0x10
	ldr r0, [r0, r1]
	ldr r1, [sp, #0xc]
	add r1, r1, r4
	str r1, [sp, #4]
_021D570E:
	ldr r1, [sp, #4]
	add r2, r6, #0
	bl ZknWt_GetSTRData
	add sp, #0x28
	pop {r4, r5, r6, pc}
	nop
// _021D571C: .4byte 0x000002C2
// _021D5720: .4byte sOv21_21E9CFC
}
#endif

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


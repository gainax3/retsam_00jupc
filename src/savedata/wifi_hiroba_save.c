//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wifi_hiroba_save.c
 *	@brief		Wi-Fiひろば　セーブデータ
 *	@author		tomoya takahashi
 *	@data		2008.05.22
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"

#include "savedata/wifi_hiroba_save.h"

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

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	セーブデータ
//=====================================
typedef struct _WFLBY_SAVEDATA {
	u32	question_number;
	u32	answer;
}WFLBY_SAVEDATA;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------
/**
 *	@brief	ワークサイズ取得
 */
//-----------------------------------------------------------------------------
int SAVEDATA_WFLBY_GetWorkSize( void )
{
	return sizeof(WFLBY_SAVEDATA);
}

//----------------------------------------------------------------------------
/**
 *	@brief	ワークの初期化
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void SAVEDATA_WFLBY_InitWork( WFLBY_SAVEDATA* p_wk )
{
	MI_CpuFill32( p_wk, 0, SAVEDATA_WFLBY_GetWorkSize() );

	p_wk->question_number = WFLBY_SAVEDATA_QUESTION_NONE;
	
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_WFHIROBA)
	SVLD_SetCrc(GMDATA_ID_WFHIROBA);
#endif //CRC_LOADCHECK
}

//----------------------------------------------------------------------------
/**
 *	@brief	ワークをメモリ確保
 *
 *	@param	heapID	ヒープ
 *
 *	@return	ワーク
 */
//-----------------------------------------------------------------------------
WFLBY_SAVEDATA* SAVEDATA_WFLBY_AllocWork( u32 heapID )
{
	WFLBY_SAVEDATA* p_wk;

	p_wk = sys_AllocMemory( heapID, SAVEDATA_WFLBY_GetWorkSize() );

	SAVEDATA_WFLBY_InitWork( p_wk );

	return p_wk;
}


//----------------------------------------------------------------------------
/**
 *	@brief	セーブデータからワークを取得
 *
 *	@param	p_sv 
 */
//-----------------------------------------------------------------------------
WFLBY_SAVEDATA* SAVEDATA_WFLBY_GetWork( SAVEDATA* p_sv )
{
	WFLBY_SAVEDATA* p_wk;

	p_wk = SaveData_Get( p_sv, GMDATA_ID_WFHIROBA );
	
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_WFHIROBA)
	SVLD_CheckCrc(GMDATA_ID_WFHIROBA);
#endif //CRC_LOADCHECK
	return p_wk;
}


//----------------------------------------------------------------------------
/**
 *	@brief	アンケート通しナンバー取得
 *
 *	@param	cp_wk	ワーク
 *
 *	@return	通しナンバー
 */
//-----------------------------------------------------------------------------
u32 SAVEDATA_WFLBY_GetQuestionNumber( const WFLBY_SAVEDATA* cp_wk )
{
	return cp_wk->question_number;
}

//----------------------------------------------------------------------------
/**
 *	@brief	回答内容取得
 *
 *	@param	cp_wk	ワーク
 *
 *	@return	回答内容
 */
//-----------------------------------------------------------------------------
u32 SAVEDATA_WFLBY_GetMyAnswer( const WFLBY_SAVEDATA* cp_wk )
{
	return cp_wk->answer;
}


//----------------------------------------------------------------------------
/**
 *	@brief	アンケート通しナンバーの設定
 *
 *	@param	p_wk		ワーク
 *	@param	number		アンケート通しナンバー
 */
//-----------------------------------------------------------------------------
void SAVEDATA_WFLBY_SetQuestionNumber( WFLBY_SAVEDATA* p_wk, u32 number )
{
	p_wk->question_number = number;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_WFHIROBA)
	SVLD_CheckCrc(GMDATA_ID_WFHIROBA);
#endif //CRC_LOADCHECK
}

//----------------------------------------------------------------------------
/**
 *	@brief	回答内容を設定
 *
 *	@param	p_wk	ワーク
 *	@param	answer	回答
 */
//-----------------------------------------------------------------------------
void SAVEDATA_WFLBY_SetMyAnswer( WFLBY_SAVEDATA* p_wk, u32 answer )
{
	p_wk->answer = answer;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_WFHIROBA)
	SVLD_CheckCrc(GMDATA_ID_WFHIROBA);
#endif //CRC_LOADCHECK
}


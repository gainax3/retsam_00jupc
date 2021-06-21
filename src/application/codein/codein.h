//==============================================================================
/**
 * @file	codein.h
 * @brief	文字入力インターフェース
 * @author	goto
 * @date	2007.07.11(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
#ifndef __CODEIN_H__
#define __CODEIN_H__

#include "gflib/strbuf.h"
#include "savedata/config.h"
#include "gflib/button_man.h"

#define CODE_BLOCK_MAX		( 3 )		///< 入力ブロック最大数

// -----------------------------------------
//
//	呼び出し側で管理するワーク
//
// -----------------------------------------
typedef struct {
	
//	int			heap_id;					///< 使用するHEAPID
	int			word_len;					///< 入力文字数	
	int			block[ CODE_BLOCK_MAX + 1 ];///< 入力ブロック　xx-xxxx-xxx とか
	
	int			end_state;					///< 終了時の状態
	STRBUF*		strbuf;						///< 空欄ならバッファそうでなければデフォルト値
	
	CONFIG*		cfg;						///< window_type取得用
	
} CODEIN_PARAM;

extern const PROC_DATA CodeInput_ProcData;

extern CODEIN_PARAM*	CodeInput_ParamCreate( int heap_id, int word_len, int block[], CONFIG* cfg );
extern void				CodeInput_ParamDelete( CODEIN_PARAM* codein_param );


//--------------------------------------------------------------
/**
 * @brief	xxxx-xxxx-xxxx のブロック定義を作る
 *
 * @param	block[]	
 *
 * @retval	static inline	
 *
 */
//--------------------------------------------------------------
static inline void CodeIn_BlockDataMake_4_4_4( int block[] )
{
#if 1
	block[ 0 ] = 4;
	block[ 1 ] = 4;
	block[ 2 ] = 4;
#else
	block[ 0 ] = 2;
	block[ 1 ] = 5;
	block[ 2 ] = 5;
#endif
}


//--------------------------------------------------------------
/**
 * @brief	xx-xxxxx-xxxxx のブロック定義を作る
 *
 * @param	block[]	
 *
 * @retval	static inline	
 *
 */
//--------------------------------------------------------------
static inline void CodeIn_BlockDataMake_2_5_5( int block[] )
{
	block[ 0 ] = 2;
	block[ 1 ] = 5;
	block[ 2 ] = 5;
}

#endif	///< __CODEIN_H__

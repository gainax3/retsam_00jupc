#ifndef __NUMBER_STR_H__
#define __NUMBER_STR_H__

//-----------------------------------------------------------
/**
 * 数値を文字列化する際の表示タイプ
 */
//-----------------------------------------------------------
typedef enum {
	NUMBER_DISPTYPE_LEFT,		///< 左詰め
	NUMBER_DISPTYPE_SPACE,		///< 右詰め（最大桁に足りない場合はスペースで埋める）
	NUMBER_DISPTYPE_ZERO,		///< 右詰め（最大桁に足りない場合はゼロで埋める）
}NUMBER_DISPTYPE;

//-----------------------------------------------------------
/**
 * 数値を文字列化する際の文字コードタイプ
 */
//-----------------------------------------------------------
typedef enum {
	NUMBER_CODETYPE_ZENKAKU,	///< 全角
	NUMBER_CODETYPE_HANKAKU,	///< 半角

	// ↓基本的にはコレを使う。
	#if( PM_LANG == LANG_JAPAN )
	NUMBER_CODETYPE_DEFAULT = NUMBER_CODETYPE_ZENKAKU,
	#else
	NUMBER_CODETYPE_DEFAULT = NUMBER_CODETYPE_HANKAKU,
	#endif
}NUMBER_CODETYPE;

#endif

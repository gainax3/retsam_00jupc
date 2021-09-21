
//============================================================================================
/**
 * @file	pokeparty_local.h
 * @author	sogabe
 * @date	2007.03.14
 * @brief	ポケモン手持ちデータ型
 *
 * セーブデータ関連の共有ヘッダ。外部公開はしない。
 */
//============================================================================================
#ifndef	__POKEPARTY_LOCAL_H__
#define	__POKEPARTY_LOCAL_H__

#include	"poketool/poke_tool_def.h"
#include	"system/gamedata.h"

//----------------------------------------------------------
/**
 * @brief	ポケモンパーティー構造体の定義
 *
 * 最大で6体までのポケモンをまとめて保持できるようになっている。
 */
//----------------------------------------------------------
struct _POKEPARTY {
	///	保持できるポケモン数の最大
	int PokeCountMax;
	///	現在保持しているポケモン数
	int PokeCount;
	///	ポケモンデータ
	struct pokemon_param member[TEMOTI_POKEMAX];
};

#endif	/* __POKEPARTY_LOCAL_H__ */

//==============================================================================================
/**
 * @file	factory_3d.h
 * @brief	「バトルファクトリー」3D
 * @author	Satoshi Nohara
 * @date	2007.03.20
 */
//==============================================================================================
#ifndef _FACTORY_3D_H_
#define _FACTORY_3D_H_

#include "factory_sys.h"


//==============================================================================================
//
//	extern宣言
//
//==============================================================================================
extern void Factory_SimpleSetUp( void );
extern void Factory_3DMain( SOFT_SPRITE_MANAGER* ssm_p );
extern POKEMON_PARAM* Factory_PokeMake( u16 monsno, u8 level, u16 itemno, int place_id, int ground_id );
extern SOFT_SPRITE* Factory_SoftSpritePokeAdd( SOFT_SPRITE_MANAGER *ssm_p, int poly_id, POKEMON_PARAM* pp, int x, int y, int z );
extern void Factory_PokeHFlip( SOFT_SPRITE* ss, int flag );


#endif //_FACTORY_3D_H_



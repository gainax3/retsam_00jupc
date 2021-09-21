//==============================================================================
/**
 * @file	fldeff_tw_namipoker.h
 * @brief	フィールドOBJ波乗りポケモン　破れた世界
 * @author	kagaya
 * @data	05.07.13
 */
//==============================================================================
#ifndef FLDEFF_NAMIPOKER_H_FILE
#define FLDEFF_NAMIPOKER_H_FILE

#include "fieldobj.h"
#include "field_effect.h"
#include "player.h"

//==============================================================================
//	define
//==============================================================================

//==============================================================================
//	typedef
//==============================================================================
//--------------------------------------------------------------
///	ステータスビット 32bit
//--------------------------------------------------------------
typedef enum
{
	NPOKER_BIT_VANISH			= 1 << 0, ///<非表示
	NPOKER_BIT_JOINT			= 1 << 1, ///<接続有り
	NPOKER_BIT_ROTATE_OFF		= 1 << 2, ///<自転オフ
	NPOKER_BIT_POS_OFF			= 1 << 3, ///<座標補正オフ
	NPOKER_BIT_ANGLE_REV_OFF	= 1 << 4, ///<公転オフ
	NPOKER_BIT_SHAKE_OUTSIDE	= 1 << 5, ///<外部指定揺れを採用
	NPOKER_BIT_POKE_OFFS_OFF	= 1 << 6, ///<波乗りポケモンオフセットオフ
}NPOKER_BIT;

//==============================================================================
//	外部参照
//==============================================================================
extern void * FE_NamiPokeR_Init( FE_SYS *fes );
extern void FE_NamiPokeR_Delete( void *work );

extern EOA_PTR FE_FldOBJNamiPokeRSet( PLAYER_STATE_PTR jiki,
	int gx, int gy, int gz, int dir, int joint, HEROTWTYPE tw );

extern ROTATE * FE_FldOBJNamiPokeR_RotatePtrGet( EOA_PTR eoa );
extern void FE_FldOBJNamiPokeR_RotateDirInit(
		EOA_PTR eoa, int dir, HEROTWTYPE tw_type );
extern void FE_FldOBJNamiPokeR_StatusBitON( EOA_PTR eoa, NPOKER_BIT bit );
extern void FE_FldOBJNamiPokeR_StatusBitOFF( EOA_PTR eoa, NPOKER_BIT bit );
extern VecFx32 * FE_FldOBJNamiPokeR_ShakeOffsetPtrGet( EOA_PTR eoa );
extern VecFx32 * FE_FldOBJNamiPokeR_PokeOffsetPtrGet( EOA_PTR eoa );
extern void FE_FldOBJNamiPokeR_AngleRevSet( EOA_PTR eoa, u16 angle );

#endif //FLDEFF_NAMIPOKER_H_FILE

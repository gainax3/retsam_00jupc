//============================================================================================
/**
 * @file	scr_field.h
 * @bfief	スクリプトで使用するプログラム(非常駐フィールドプログラム)
 * @author	Satoshi Nohara
 * @date	07.12.04
 */
//============================================================================================
#ifndef SCR_FIELD_H
#define SCR_FIELD_H


#include "../../include/pl_bugfix.h"

//==============================================================================================
//
//	イベントウィンドウワークへの不完全型ポインタ
//
//==============================================================================================
typedef	struct _SEISEKI_WORK SEISEKI_WORK;


//============================================================================================
//
//	プログラムでのみ参照する定義
//
//============================================================================================
//__ASM_NO_DEF_が定義されている場合、アセンブラソースなので以下は無効にする
#ifndef	__ASM_NO_DEF_
extern BOOL EvCmdSeisekiBmpListStart( VM_MACHINE * core );
extern BOOL EvCmdTopBreederPowRndGet( VM_MACHINE * core );
extern BOOL EvCmdPokeFormChange( VM_MACHINE * core );
extern BOOL EvCmdMezameruPawaaTypeGet( VM_MACHINE * core );
extern BOOL EvCmdSetFavoritePoke( VM_MACHINE * core );
extern BOOL EvCmdGetFavoritePoke( VM_MACHINE * core );
//extern BOOL EvCmdRotomuFormCheck( VM_MACHINE * core );
extern BOOL EvCmdTemotiRotomuFormChangeGet( VM_MACHINE * core );
extern BOOL EvCmdRotomuFormWazaChange( VM_MACHINE * core );
extern BOOL EvCmdTemotiRotomuFormNoGet( VM_MACHINE * core );
extern BOOL EvCmdVillaListCheck( VM_MACHINE * core );
extern BOOL EvCmdImageClipTvSendFlagReset( VM_MACHINE * core );
extern BOOL EvCmdUnionPokeFormReturn( VM_MACHINE * core );
extern BOOL EvCmdSodateyaPokeFormReturn( VM_MACHINE * core );
extern BOOL EvCmdD35FloorSet( VM_MACHINE * core );
extern BOOL EvCmdFldOBJAllHeightVanishOFF( VM_MACHINE * core );
extern BOOL EvCmdFldOBJAllHeightVanishON( VM_MACHINE * core );
extern BOOL EvCmdWifiHistory( VM_MACHINE * core );
extern BOOL EvCmdFldOBJForceDraw_C04EventOnly( VM_MACHINE * core );
extern BOOL EvCmdFldOBJForceDraw_C04EventAfterOnly( VM_MACHINE * core );
extern BOOL EvCmdSndPlayerVolume( VM_MACHINE * core );
#if PL_S0802_080716_FIX
extern BOOL EvCmdGetPokeSeeFlag( VM_MACHINE * core );
#endif

extern BOOL ov05_21F7704( VM_MACHINE * core );
extern BOOL ov05_21F7754( VM_MACHINE * core );

#endif	/* __ASM_NO_DEF_ */


#endif	/* SCR_FIELD_H */



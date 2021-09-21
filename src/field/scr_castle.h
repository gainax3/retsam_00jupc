//============================================================================================
/**
 * @file	scr_castle.h
 * @bfief	スクリプトコマンド：バトルキャッスル
 * @author	Satoshi Nohara
 * @date	07.06.13
 */
//============================================================================================
#ifndef SCR_CASTLE_H
#define SCR_CASTLE_H


//============================================================================================
//
//	プログラムでのみ参照する定義
//
//============================================================================================
//__ASM_NO_DEF_が定義されている場合、アセンブラソースなので以下は無効にする
#ifndef	__ASM_NO_DEF_
extern BOOL EvCmdBattleCastleTools(VM_MACHINE* core);
extern BOOL EvCmdBattleCastleSetContinueNG( VM_MACHINE * core );
extern BOOL EvCmdBattleCastleCommMonsNo(VM_MACHINE* core);
#endif	/* __ASM_NO_DEF_ */


#endif	/* SCR_CASTLE_H */



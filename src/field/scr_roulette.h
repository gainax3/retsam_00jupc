//============================================================================================
/**
 * @file	scr_roulette.h
 * @bfief	スクリプトコマンド：バトルルーレット
 * @author	Satoshi Nohara
 * @date	07.09.05
 */
//============================================================================================
#ifndef SCR_ROULETTE_H
#define SCR_ROULETTE_H


//============================================================================================
//
//	プログラムでのみ参照する定義
//
//============================================================================================
//__ASM_NO_DEF_が定義されている場合、アセンブラソースなので以下は無効にする
#ifndef	__ASM_NO_DEF_
extern BOOL EvCmdBattleRouletteTools(VM_MACHINE* core);
extern BOOL EvCmdBattleRouletteSetContinueNG( VM_MACHINE * core );
extern BOOL EvCmdBattleRouletteCommMonsNo(VM_MACHINE* core);
#endif	/* __ASM_NO_DEF_ */


#endif	/* SCR_ROULETTE_H */



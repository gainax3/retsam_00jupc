//============================================================================================
/**
 * @file	scr_wazaoshie.h
 * @bfief	スクリプトコマンド：技教え関連
 * @author	Satoshi Nohara
 * @date	07.12.17
 */
//============================================================================================
#ifndef SCR_WAZAOSHIE_H
#define SCR_WAZAOSHIE_H


//==============================================================================================
//
//	イベントウィンドウワークへの不完全型ポインタ
//
//==============================================================================================
typedef	struct _WAZA_OSHIE_WORK WAZA_OSHIE_WORK;


//============================================================================================
//
//	プログラムでのみ参照する定義
//
//============================================================================================
//__ASM_NO_DEF_が定義されている場合、アセンブラソースなので以下は無効にする
#ifndef	__ASM_NO_DEF_
extern BOOL EvCmdWazaOshieDataCount(VM_MACHINE * core );
extern BOOL EvCmdWazaOshieBmpListStart( VM_MACHINE * core );
extern BOOL EvCmdWazaOshieChgPokeWaza( VM_MACHINE * core );
//extern BOOL EvCmdWazaOshiePokeStatusSetProc(VM_MACHINE * core);
//extern BOOL EvCmdWazaOshiePokeStatusGetResult(VM_MACHINE * core);
extern BOOL EvCmdWazaOshieItemCheck( VM_MACHINE * core );
extern BOOL EvCmdWazaOshieItemSub( VM_MACHINE * core );
extern BOOL EvCmdWazaOshieBoardWrite( VM_MACHINE * core );
extern BOOL EvCmdWazaOshieBoardDel( VM_MACHINE * core );

#endif	/* __ASM_NO_DEF_ */


#endif	/* SCR_WAZAOSHIE_H */



//============================================================================================
/**
 * @file	scr_stage.h
 * @bfief	スクリプトコマンド：バトルステージ
 * @author	Satoshi Nohara
 * @date	06.06.13
 */
//============================================================================================
#ifndef SCR_STAGE_H
#define SCR_STAGE_H


//============================================================================================
//
//	プログラムでのみ参照する定義
//
//============================================================================================
//__ASM_NO_DEF_が定義されている場合、アセンブラソースなので以下は無効にする
#ifndef	__ASM_NO_DEF_
extern BOOL EvCmdBattleStageTools(VM_MACHINE* core);
extern BOOL EvCmdBattleStageSetContinueNG( VM_MACHINE * core );
extern BOOL EvCmdBattleStageCommMonsNo(VM_MACHINE* core);
extern BOOL EvCmdBattleStageNumberName( VM_MACHINE * core );
extern BOOL EvCmdBattleStageRecordPokeGet( VM_MACHINE * core );
extern BOOL EvCmdBattleStageTotalRecordGetEx( VM_MACHINE * core );
extern BOOL EvCmdTVStageRensyouCheck( VM_MACHINE * core );
#endif	/* __ASM_NO_DEF_ */


#endif	/* SCR_STAGE_H */



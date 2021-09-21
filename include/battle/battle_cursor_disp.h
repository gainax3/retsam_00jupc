//==============================================================================
/**
 * @file	battle_cursor_disp.h
 * @brief	戦闘カーソル初期表示状態記憶のヘッダ
 * @author	matsuda
 * @date	2007.08.09(木)
 */
//==============================================================================
#ifndef __BATTLE_CURSOR_DISP_H__
#define __BATTLE_CURSOR_DISP_H__

#ifndef	__ASM_NO_DEF_	// ↓これ以降は、アセンブラでは無視

//==============================================================================
//	構造体定義
//==============================================================================
typedef struct{
	u8 on_off;		///<TRUE:表示、FALSE:非表示
	
	u8 dummy[3];	///<パディング
}BATTLE_CURSOR_DISP;


//==============================================================================
//	外部関数宣言
//==============================================================================
extern BATTLE_CURSOR_DISP * BattleCursorDisp_AllocWork(int heap_id);
extern void BattleCursorDisp_Free(BATTLE_CURSOR_DISP *bcd);

#endif	//__ASM_NO_DEF_

#endif	//__BATTLE_CURSOR_DISP_H__

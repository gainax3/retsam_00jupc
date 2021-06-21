//==============================================================================
/**
 * @file	frontier_scrcmd_sub.h
 * @brief	簡単な説明を書く
 * @author	matsuda
 * @date	2007.04.06(金)
 */
//==============================================================================
#ifndef __FRONTIER_SCRCMD_SUB_H__
#define __FRONTIER_SCRCMD_SUB_H__


//==============================================================================
//	構造体定義
//==============================================================================
///ScrTalkMsg関数の拡張パラメータ
typedef struct{
	u8 msg_speed;		///<メッセージ速度
	u8 auto_flag;		///<自動送り設定(MSG_AUTO_???)
	u8 font;			///<フォント
	
	u8 dummy;
}FSCR_TALK_EX_PARAM;


//==============================================================================
//	外部関数宣言
//==============================================================================
extern void FSSC_Sub_ScrTalkMsg(FSS_PTR fss, const MSGDATA_MANAGER* msgman, 
	u16 msg_id, u8 skip,  FSCR_TALK_EX_PARAM *ex_param );
extern void FSSC_Sub_ScrTalkClose(FSS_PTR fss);
//menu
extern FSEVWIN_PTR FSSC_Sub_BmpMenu_Init( FSS_PTR fss, u8 x, u8 y, u8 cursor, u8 cancel, 
	u16* work, WORDSET* wordset, MSGDATA_MANAGER* msgman );
extern void FSSC_Sub_BmpMenu_MakeList( FSEVWIN_PTR wk, u32 msg_id, u32 talk_msg_id, u32 param  );
extern void FSSC_Sub_BmpMenu_Start( FSEVWIN_PTR wk );
extern void EvBmpMenu_Del( FSEVWIN_PTR wk );
void FSSC_Sub_BmpMenu_Del( FSEVWIN_PTR wk );
//list
extern FSEVWIN_PTR FSSC_Sub_BmpList_Init( FSS_PTR fss, u8 x, u8 y, u8 cursor, u8 cancel, 
	u16* work, WORDSET* wordset, MSGDATA_MANAGER* msgman );
extern void FSSC_Sub_BmpList_MakeList( FSEVWIN_PTR wk, u32 msg_id, u32 talk_msg_id, u32 param  );
extern void FSSC_Sub_BmpList_Start( FSEVWIN_PTR wk );
//extern void EvBmpList_Del( FSEVWIN_PTR wk );
extern void FSSC_Sub_AnimeListMain(TCB_PTR tcb, void *work);
extern void FSTalkMsgPMSParam( FS_SYSTEM* fss, u8 spd, u16 stype, u16 sid, u16 word0, s16 word1, u8 skip );
void FSSC_Sub_BmpList_Del( FSEVWIN_PTR wk );

extern void FSSC_Sub_PokemonActorAdd(FMAP_PTR fmap, POKEMON_PARAM *pp, int heap_id, 
	int manager_id, int x, int y, int soft_pri, int bg_pri, int evy, u16 next_rgb);
extern void FSSC_Sub_PokemonDispDelete(FMAP_PTR fmap, int manager_id);

extern void FSSC_Sub_ActCmdCreate(
	WF2DMAP_ACTCMD *act, WF2DMAP_OBJWK *objwk, int playid, int code);

extern void FSSC_Sub_PokeIconCommonResourceSet(FMAP_PTR fmap);
extern void FSSC_Sub_PokeIconCommonResourceFree(FMAP_PTR fmap);
extern CATS_ACT_PTR Frontier_PokeIconAdd(FMAP_PTR fmap, POKEMON_PARAM *pp, int pos, int x, int y);
extern void FSSC_Sub_PokeIconDel(FMAP_PTR fmap, CATS_ACT_PTR cap, int pos);

extern void FSSC_Sub_ItemIconLoad(FMAP_PTR fmap);
extern void FSSC_Sub_ItemIconFree(FMAP_PTR fmap);
extern CATS_ACT_PTR Frontier_ItemIconAdd(FMAP_PTR fmap, int x, int y);
extern void FSSC_Sub_ItemIconDel(FMAP_PTR fmap, CATS_ACT_PTR cap);
extern void FrontierTalkMsgSub( FSS_TASK* core, u16* msg );
extern void FrontierTalkMsgSub2( FSS_TASK* core, u16* msg, u32 datID );

extern void FSSC_Sub_ShakeMove(TCB_PTR tcb, void *work);
extern void FSSC_Sub_Window(TCB_PTR tcb, void *work);


#endif	//__FRONTIER_SCRCMD_SUB_H__

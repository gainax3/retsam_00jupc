//============================================================================================
/**
 * @file	scr_name.h
 * @bfief	スクリプトコマンド：名前関連
 * @author	Satoshi Nohara
 * @date	06.06.23
 */
//============================================================================================
#ifndef SCR_NAME_H
#define SCR_NAME_H


//============================================================================================
//
//	プログラムでのみ参照する定義
//
//============================================================================================
//__ASM_NO_DEF_が定義されている場合、アセンブラソースなので以下は無効にする
#ifndef	__ASM_NO_DEF_
extern BOOL EvCmdStatusName(VM_MACHINE * core );
extern BOOL EvCmdPlayerName(VM_MACHINE * core );
extern BOOL EvCmdRivalName(VM_MACHINE * core ); 
extern BOOL EvCmdSupportName(VM_MACHINE * core );
extern BOOL EvCmdPokemonName(VM_MACHINE * core );
extern BOOL EvCmdTypeName(VM_MACHINE * core );
extern BOOL EvCmdItemName(VM_MACHINE * core );
extern BOOL EvCmdPocketName(VM_MACHINE * core );
extern BOOL EvCmdItemWazaName(VM_MACHINE * core );
extern BOOL EvCmdWazaName(VM_MACHINE * core );
extern BOOL EvCmdNumberName(VM_MACHINE * core );
extern BOOL EvCmdNumberNameEx(VM_MACHINE * core );
extern BOOL EvCmdNumberNameEx32(VM_MACHINE * core );
extern BOOL EvCmdNickName(VM_MACHINE * core );
extern BOOL EvCmdNickNamePC(VM_MACHINE * core );
extern BOOL EvCmdPoketchName(VM_MACHINE * core );
extern BOOL EvCmdTrTypeName(VM_MACHINE * core );
extern BOOL EvCmdMyTrTypeName(VM_MACHINE * core );
extern BOOL EvCmdPokemonNameExtra(VM_MACHINE * core );
extern STRBUF * PokeNameGetAlloc( u16 id, u32 heap );
extern BOOL EvCmdFirstPokemonName(VM_MACHINE * core );
extern BOOL EvCmdRivalPokemonName(VM_MACHINE * core );
extern BOOL EvCmdSupportPokemonName(VM_MACHINE * core );
extern BOOL EvCmdGoodsName(VM_MACHINE * core );
extern BOOL EvCmdTrapName(VM_MACHINE * core );
extern BOOL EvCmdTamaName(VM_MACHINE * core );
extern BOOL EvCmdZoneName(VM_MACHINE * core );
extern BOOL EvCmdNutsName(VM_MACHINE * core);
extern BOOL EvCmdSeikakuName(VM_MACHINE * core);
extern BOOL EvCmdAcceName(VM_MACHINE * core );
extern BOOL EvCmdMonumantName(VM_MACHINE * core ) ;
extern BOOL EvCmdTemotiWazaName( VM_MACHINE * core );
extern BOOL EvCmdRibbonName(VM_MACHINE * core);
extern BOOL EvCmdSealName(VM_MACHINE * core );
extern BOOL EvCmdTrainerName(VM_MACHINE * core );

// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ALL) imatake 2006/10/13
// 冠詞付き・複数形のアイテム名を引っ張ってくるスクリプト命令を追加
extern BOOL EvCmdItemNameIndefinate(VM_MACHINE * core );
extern BOOL EvCmdItemNamePlural(VM_MACHINE * core );
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ALL) imatake 2006/11/28
// 不定冠詞付きの地下グッズ名を引っ張ってくるスクリプト命令を追加
extern BOOL EvCmdGoodsNameIndefinate(VM_MACHINE * core );
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ALL) imatake 2006/11/29
// 不定冠詞付きの地下グッズ名を引っ張ってくるスクリプト命令を追加
extern BOOL EvCmdTrapNameIndefinate(VM_MACHINE * core );
extern BOOL EvCmdTamaNameIndefinate(VM_MACHINE * core );
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ALL) imatake 2006/12/11
// 不定冠詞付きのポケモン名・アクセサリー名を引っ張ってくるスクリプト命令を追加
extern BOOL EvCmdPokemonNameExtraIndefinate(VM_MACHINE * core );
extern BOOL EvCmdSupportPokemonNameIndefinate(VM_MACHINE * core );
extern BOOL EvCmdAcceNameIndefinate(VM_MACHINE * core );
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ALL) imatake 2006/12/19
// 不定冠詞付きのトレーナータイプ名を引っ張ってくるスクリプト命令を追加
extern BOOL EvCmdTrTypeNameIndefinate(VM_MACHINE * core );
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ALL) imatake 2007/01/26
// 複数形のシール名を引っ張ってくるスクリプト命令を追加
extern BOOL EvCmdSealNamePlural(VM_MACHINE * core );
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ALL) imatake 2006/11/24
// 変数に代入された文字列をキャピタライズするスクリプト命令を追加
extern BOOL EvCmdCapitalizeName(VM_MACHINE * core);
// ----------------------------------------------------------------------------

extern BOOL EvCmd_2048080(VM_MACHINE * core); // plat_us_match

#endif	/* __ASM_NO_DEF_ */


#endif	/* SCR_NAME_H */



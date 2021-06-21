//============================================================================================
/**
 * @file	ashiato.h
 * @bfief	フトマキさん用足跡関数郡
 * @author	Tomomichi Ohta
 * @date	06.05.20
 */
//============================================================================================
//--------------------------------------------------------------------------------------------
/**
 *	ポケモン番号から足跡の分類に変換
 *
 * @param	monsno		ポケモン番号
 *
 * @return	分類(1〜5)
 */
//--------------------------------------------------------------------------------------------
extern u16 ashiato_group_chk(u16 mons_no);

//--------------------------------------------------------------------------------------------
/**
 *	ポケモン番号から足跡の有無をチェック
 *
 * @param	monsno		ポケモン番号
 * @param	form_no		フォルムナンバー
 *
 * @return	0:足跡なし 1:足跡あり
 */
//--------------------------------------------------------------------------------------------
extern u16 ashiato_ato_chk(u16 mons_no, u16 form_no);

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

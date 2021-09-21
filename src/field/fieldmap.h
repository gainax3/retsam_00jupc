//=============================================================================
/**
 * @file	fieldmap.h	
 * @bfief	フィールドマップシステム
 * @author	GAME FREAK inc.
 */
//=============================================================================
#ifndef	__FIELDMAP_H__
#define __FIELDMAP_H__


#include "system/brightness.h"
#include "field_common.h"

// 輝度調整用フラグ
enum {
	FLD_DISP_BRIGHT_BLACKOUT = 0,		// 暗くする
	FLD_DISP_BRIGHT_BLACKIN			// 元に戻す
};

// BGフレーム
#define	FLD_MBGFRM_MAP		( GF_BGL_FRAME0_M )		///<マップ
#define	FLD_MBGFRM_EFFECT1	( GF_BGL_FRAME1_M )		///<エフェクト１
#define	FLD_MBGFRM_EFFECT2	( GF_BGL_FRAME2_M )		///<エフェクト２
#define	FLD_MBGFRM_FONT		( GF_BGL_FRAME3_M )		///<メッセージ

#define	FLD_SBGFRM_MENU		( GF_BGL_FRAME0_S )		///<メニュー
#define	FLD_SBGFRM_EFFECT1	( GF_BGL_FRAME1_S )		///<エフェクト１
#define	FLD_SBGFRM_EFFECT2	( GF_BGL_FRAME2_S )		///<エフェクト２
#define	FLD_SBGFRM_FONT		( GF_BGL_FRAME3_S )		///<メッセージ

extern  void FieldFadeWipeSet(const u8 flg);

extern void Fieldmap_BgSet( GF_BGL_INI * ini );
extern void Fieldmap_BgExit( GF_BGL_INI * ini );


//-------------------------------------
// fldmap_update_msk
// フィールドアップデートマスク操作
//=====================================
// 3D描画更新設定	TRUE：更新する	FALSE：更新しない
extern void Fieldmap_UpdateMsk_Set3Dwrite( FIELDSYS_WORK * fsys, BOOL flag );
extern void Fieldmap_UpdateMsk_SetFieldAnime( FIELDSYS_WORK * fsys, BOOL flag );

extern void FieldMap_TornWorldZoneChange( FIELDSYS_WORK * fsys, u32 new_zone_id );

#endif //__FIELDMAP_H__


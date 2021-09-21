//==============================================================================
/**
 * @file	gds_data_conv.h
 * @brief	ボックスやドレスアップ等をGDSサーバーで使用する形式に変換するツールのヘッダ
 * @author	matsuda
 * @date	2007.12.11(火)
 */
//==============================================================================
#ifndef __GDS_DATA_CONV_H__
#define __GDS_DATA_CONV_H__

#include "gds_ranking_sub.h"


//==============================================================================
//	外部関数宣言
//==============================================================================
extern void GDS_CONV_Dress_to_GTDress(SAVEDATA *sv, const IMC_TELEVISION_SAVEDATA * dress, GT_GDS_DRESS * gt_dress);
extern void GDS_CONV_GTDress_to_Dress(const GT_GDS_DRESS * gt_dress, IMC_TELEVISION_SAVEDATA * dress);
extern void GDS_CONV_Box_to_GTBox(SAVEDATA *sv, const BOX_DATA *boxdata, int tray_number, GT_BOX_SHOT *gt_box, int heap_id);
extern void GDS_GTBoxShot_SetCategoryNo(SAVEDATA *sv, GT_BOX_SHOT *gt_box, int category_no);
extern void GDS_CONV_GDSProfile_to_GTGDSProfile(SAVEDATA *sv, const GDS_PROFILE_PTR gpp, GT_GDS_PROFILE * gt_gpp);
extern void GDS_CONV_GTGDSProfile_to_GDSProfile(const GT_GDS_PROFILE * gt_gpp, GDS_PROFILE_PTR gpp);
extern void GDS_CONV_GDSProfile_to_GTRankingProfile(SAVEDATA *sv, const GDS_PROFILE_PTR gpp, GT_RANKING_PROFILE * gt_rp);


#endif	//__GDS_DATA_CONV_H__

/*---------------------------------------------------------------------------*
  Project:  NitroDWC - auth - util_wifiidtool
  File:     util_wifiidtool.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: util_wifiidtool.h,v $
  Revision 1.1  2005/08/30 01:31:23  nakata
  Add util_wifiidtoos.h to $NITRODWC_ROOT/include/bm.

  Revision 1.9  2005/08/22 12:07:13  sasakit
  WiFiIDを作るべきかどうかを判定する関数の追加。

  Revision 1.8  2005/08/20 09:34:04  akimaru
  非公開の部分をutil_wifiidtool.hに分離

  Revision 1.7  2005/08/20 07:01:19  sasakit
  ヘッダインクルードガードの書式を統一した。
  bm/dwc_init.h -> bm/dwc_bm_init.hに変更
  ヘッダにCopyrightをつけた。
  構造体の名前空間をできるだけルールにちかづけた。
  util_wifiidtool.hとdwc_backup.hの関係を若干修正。


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_UTIL_WIFIIDTOOL_H_
#define DWC_UTIL_WIFIIDTOOL_H_

#include <nitro.h>

#ifdef __cplusplus
extern "C" {
#endif		// __cplusplus


	/*---------------------------------------------------------------------------*
  	Name:        DWC_Auth_Id

	Description:  UserIDを格納する構造体
	u64     	   uId:  認証されたユーザID
	u64unAttestationID:  認証されていないユーザID    			  
	u8		       flg:  認証されたかどうかを示すフラグ 0:認証されていない　1:認証されている
   	Returns:	  なし
 	*---------------------------------------------------------------------------*/
	typedef struct{
		u64 uId;
		u64 notAttestedId;
		u32  flg;
	}DWCAuthWiFiId;
	
	/*---------------------------------------------------------------------------*
  	Name:         DWC_Auth_GetId

	Description:  DWC_Auth_Id を取得する
  	Arguments:    id			
	  

   	Returns:	  なし
 	*---------------------------------------------------------------------------*/
	void DWC_Auth_GetId( DWCAuthWiFiId* id );

	/*---------------------------------------------------------------------------*
	  Name:         DWC_Auth_CheckPseudoWiFiID
	
	  Description:  オフラインで生成する仮のWiFiIDが生成されているかどうかを確認する
	
	  Arguments:    なし
	
	  Returns:      TRUE  : 生成されてる。
	                FALSE : 生成されていない。
	 *---------------------------------------------------------------------------*/
	BOOL DWC_Auth_CheckPseudoWiFiID( void );

BOOL DWC_Auth_CheckWiFiIDNeedCreate( void );


#ifdef __cplusplus
}
#endif		// __cplusplus


#endif // DWC_UTIL_WIFIIDTOOL_H_

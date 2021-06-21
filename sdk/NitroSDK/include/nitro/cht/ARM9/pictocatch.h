/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CHT - include
  File:     pictocatch.h

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: pictocatch.h,v $
  Revision 1.3  2005/03/02 23:42:49  terui
  Update copyright

  Revision 1.2  2005/03/02 12:43:56  terui
  関数パラメータにconstを追加。

  Revision 1.1  2004/12/22 02:40:58  terui
  Initial upload

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NITRO_CHT_ARM9_PICTOCATCH_H_
#define NITRO_CHT_ARM9_PICTOCATCH_H_

#ifdef  __cplusplus
extern "C" {
#endif

/*===========================================================================*/

#include <nitro/types.h>
#include <nitro/wm.h>


/*---------------------------------------------------------------------------*
  Name:         CHT_IsPictochatParent

  Description:  親機のビーコン情報がピクトチャットのビーコンであるかどうかを
                調査する。

  Arguments:    pWmBssDesc  -   調査するビーコン情報へのポインタ。

  Returns:      BOOL        -   ピクトチャットのビーコンである場合はTRUEを、
                                そうでない場合はFALSEを返す。
 *---------------------------------------------------------------------------*/
BOOL    CHT_IsPictochatParent( const WMBssDesc* pWmBssDesc );

/*---------------------------------------------------------------------------*
  Name:         CHT_GetPictochatClientNum

  Description:  ピクトチャットのビーコン情報からグループに参加している子機の
                数を調査する。
                ピクトチャットのビーコンでない場合は不定な値を返すことになる
                ので、CHT_IsPictochatParent 関数で判定してから使用する。

  Arguments:    pWmBssDesc  -   調査するビーコン情報へのポインタ。

  Returns:      int         -   グループに参加している子機の数を返す。
                                異常なビーコン情報であった場合は -1 を返す。
 *---------------------------------------------------------------------------*/
int     CHT_GetPictochatClientNum( const WMBssDesc* pWmBssDesc );

/*---------------------------------------------------------------------------*
  Name:         CHT_GetPictochatRoomNumber

  Description:  ピクトチャットのビーコン情報からルーム番号を取得する。
                ピクトチャットのビーコンでない場合は不定な値を返すことになる
                ので、CHT_IsPictochatParent 関数で判定してから使用する。

  Arguments:    pWmBssDesc  -   調査するビーコン情報へのポインタ。

  Returns:      int         -   ルーム番号を返す。
                                異常なビーコン情報であった場合は -1 を返す。
 *---------------------------------------------------------------------------*/
int     CHT_GetPictochatRoomNumber( const WMBssDesc* pWmBssDesc );


/*===========================================================================*/

#ifdef  __cplusplus
}       /* extern "C" */
#endif

#endif  /* NITRO_CHT_ARM9_PICTOCATCH_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/

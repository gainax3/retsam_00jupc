/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - libraries
  File:     wm_arm9_private.h

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wm_arm9_private.h,v $
  Revision 1.17  2007/05/10 01:01:29  yasu
  著作年度修正

  Revision 1.16  2007/05/02 08:43:38  seiki_masashi
  WM_StartMPEx 関数での追加設定パラメータの影響が一時的になるよう変更

  Revision 1.15  2006/01/18 02:12:39  kitase_hirotake
  do-indent

  Revision 1.14  2005/11/25 00:45:05  seiki_masashi
  mask を WMMPParam 構造体内に移動

  Revision 1.13  2005/11/24 03:03:46  seiki_masashi
  WMi_StartMP 関数を追加

  Revision 1.12  2005/11/22 09:03:10  seiki_masashi
  WM_SendCommandDirect 関数の追加

  Revision 1.11  2005/11/01 08:52:06  seiki_masashi
  WM_DEBUG_AIDBITMAP の追加

  Revision 1.10  2005/02/28 05:26:35  yosizaki
  do-indent.

  Revision 1.9  2005/02/23 12:48:14  seiki_masashi
  デバッグ表示の整理

  Revision 1.8  2005/02/22 05:40:44  seiki_masashi
  Copyright 表記の修正

  Revision 1.7  2005/02/22 01:52:26  seiki_masashi
  WM_WARNING などを wm_private.h に移動

  Revision 1.6  2004/10/22 04:39:39  terui
  WMErrCodeとしてint型を返していた関数について、WMErrCode列挙型を返すように変更。

  Revision 1.5  2004/10/15 14:11:11  terui
  WM_GetAllowedChannel関数が未初期化時に返す固定値を定義。

  Revision 1.4  2004/09/23 12:34:43  seiki_masashi
  wm_private.h を include するように変更

  Revision 1.3  2004/09/11 06:09:12  yasu
  Use OS_TPrintf instead of WMi_Printf

  Revision 1.2  2004/09/11 04:13:00  terui
  警告表示を行うプリントをあまりスタックを消費しないものに変更。

  Revision 1.1  2004/09/10 11:04:02  terui
  wm.cの分割に伴い、新規upload。

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef LIBRARIES_WM_ARM9_WM_ARM9_PRIVATE_H__
#define LIBRARIES_WM_ARM9_WM_ARM9_PRIVATE_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*===========================================================================*/

#include    "wm_private.h"


/*---------------------------------------------------------------------------*
    定数定義
 *---------------------------------------------------------------------------*/
#define     WM_GET_ALLOWED_CHANNEL_BEFORE_INIT      0x8000


/*---------------------------------------------------------------------------*
    マクロ定義
 *---------------------------------------------------------------------------*/
#define     WM_CHECK_RESULT( res )      \
    if( ( res ) != WM_ERRCODE_SUCCESS ) \
    {                                   \
        return ( res );                 \
    }


#ifdef WM_DEBUG_DATASHARING
#define WM_DPRINTF_DATASHARING         WMi_Printf
#else
#define WM_DPRINTF_DATASHARING(...)    ((void) 0)
#endif
#ifdef WM_DEBUG_CALLBACK
#define WM_DPRINTF_CALLBACK            WMi_Printf
#else
#define WM_DPRINTF_CALLBACK(...)       ((void) 0)
#endif
#ifdef WM_DEBUG_AIDBITMAP
#define WM_DPRINTF_AIDBITMAP           WMi_Printf
#else
#define WM_DPRINTF_AIDBITMAP(...)      ((void) 0)
#endif


#define WMi_LOG(cat, msg) \
    do { s32 _SDK_WM_vcount = GX_GetVCount(); (void)WMi_Printf( "%02x:%03d| " cat msg "\n", ((OS_GetVBlankCount()+(_SDK_WM_vcount<192))&0xff), _SDK_WM_vcount );} while ( FALSE )

#define WMi_LOGF(cat, msg, ...) \
    do { s32 _SDK_WM_vcount = GX_GetVCount(); (void)WMi_Printf( "%02x:%03d| " cat msg "\n", ((OS_GetVBlankCount()+(_SDK_WM_vcount<192))&0xff), _SDK_WM_vcount, __VA_ARGS__ );} while ( FALSE )

#define WM_LOG(msg) WMi_LOG("", msg)
#define WM_LOGF(msg, ...) WMi_LOGF("", msg, __VA_ARGS__)

#ifdef WM_DEBUG
#define WM_DLOG  WM_LOG
#define WM_DLOGF WM_LOGF
#else
#define WM_DLOG(...)  ((void) 0)
#define WM_DLOGF(...) ((void) 0)
#endif
#ifdef WM_DEBUG_DATASHARING
#define WM_DLOG_DATASHARING(msg)  WMi_LOG("[DS]", msg);
#define WM_DLOGF_DATASHARING(msg, ...) WMi_LOGF("[DS]", msg, __VA_ARGS__);
#else
#define WM_DLOG_DATASHARING(...)  ((void) 0)
#define WM_DLOGF_DATASHARING(...) ((void) 0)
#endif
#ifdef WM_DEBUG_CALLBACK
#define WM_DLOG_CALLBACK  WM_LOG
#define WM_DLOGF_CALLBACK WM_LOGF
#define WM_DLOGF2_CALLBACK(from, msg, ...) \
    do { s32 _SDK_WM_vcount = GX_GetVCount(); (void)WMi_Printf( "%02x:%03d-%03d| " msg "\n", ((OS_GetVBlankCount()+(_SDK_WM_vcount<192))&0xff), from, _SDK_WM_vcount, __VA_ARGS__ );} while ( FALSE )
//    ((void) ( WMi_Printf( "%02x:%03d-%03d| " msg "\n", ((OS_GetVBlankCount()+(GX_GetVCount()<192))&0xff), from, GX_GetVCount(), __VA_ARGS__) ))
#else
#define WM_DLOG_CALLBACK(...)  ((void) 0)
#define WM_DLOGF_CALLBACK(...) ((void) 0)
#define WM_DLOGF2_CALLBACK(...) ((void) 0)
#endif
#ifdef WM_DEBUG_AIDBITMAP
#define WM_DLOG_AIDBITMAP(msg)  WMi_LOG("[AID]", msg);
#define WM_DLOGF_AIDBITMAP(msg, ...) WMi_LOGF("[AID]", msg, __VA_ARGS__);
#else
#define WM_DLOG_AIDBITMAP(...)  ((void) 0)
#define WM_DLOGF_AIDBITMAP(...) ((void) 0)
#endif


/*---------------------------------------------------------------------------*
    非公開関数プロトタイプ
 *---------------------------------------------------------------------------*/
void    WMi_SetCallbackTable(WMApiid id, WMCallbackFunc callback);
WMErrCode WMi_SendCommand(WMApiid id, u16 paramNum, ...);
WMErrCode WMi_SendCommandDirect(void *data, u32 length);
WMArm9Buf *WMi_GetSystemWork(void);
WMErrCode WMi_CheckStateEx(s32 paramNum, ...);
WMErrCode WMi_CheckInitialized(void);
WMErrCode WMi_CheckIdle(void);
WMErrCode WMi_StartMP(WMCallbackFunc callback,
                      u16 *recvBuf,
                      u16 recvBufSize, u16 *sendBuf, u16 sendBufSize, WMMPTmpParam* tmpParam);


/*---------------------------------------------------------------------------*
    インライン関数定義
 *---------------------------------------------------------------------------*/
static inline WMErrCode WMi_CheckState(u32 state)
{
    return WMi_CheckStateEx(1, state);
}


/*===========================================================================*/

#ifdef  __cplusplus
}       /* extern "C" */
#endif

#endif /* LIBRARIES_WM_ARM9_WM_ARM9_PRIVATE_H__ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/

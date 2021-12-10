/*---------------------------------------------------------------------------*
  Project:  NitroDWC Libraries
  File:     ./include/lobby/dwc_lobby.h

  Copyright 2005-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/
/**
 * @file
 * 
 * @brief Wi-Fi ロビーライブラリ ヘッダ
 * 
 * Wi-Fi ロビーライブラリ
 * 
 */

#ifndef DWC_LOBBY_H_
#define DWC_LOBBY_H_

#include <dwc.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef WIN32
#pragma pack(push, 4)
#endif

#define DWC_LOBBY_INVALID_USER_ID       -1  ///< 無効なユーザID。
#define DWC_LOBBY_INVALID_TIME          0   ///< 無効な時間。

/// DWCi_LobbySend*Message関数及びDWCi_LobbySetChannelData関数で送信できる最大の文字数。(NULL終端含む)
#define DWC_LOBBY_MAX_STRING_SIZE       400

/// チャンネル名に指定できる最大の文字数。(NULL終端含む)
#define DWC_LOBBY_MAX_CHANNEL_NAME_SIZE 20
    
/// DWCi_LobbySetChannelTopic関数で指定できるトピックの最大の文字数。(NULL終端含む)
#define DWC_LOBBY_MAX_CHANNEL_TOPIC_SIZE DWC_LOBBY_MAX_STRING_SIZE

/// チャンネルデータのキーとして指定できる最大の文字数。(NULL終端含む)
#define DWC_LOBBY_MAX_CHANNEL_KEY_SIZE  20

/// チャンネルのパスワードとして指定できる最大の文字数。(NULL終端含む)
#define DWC_LOBBY_MAX_CHANNEL_PASSWORD_SIZE 20

/// チャンネル名に付けるプリフィックス。入室チャンネルを分けることができます。
typedef enum DWC_LOBBY_CHANNEL_PREFIX
{
    DWC_LOBBY_CHANNEL_PREFIX_RELEASE,       ///< 製品用。
    DWC_LOBBY_CHANNEL_PREFIX_DEBUG,         ///< デバッグ用。
    DWC_LOBBY_CHANNEL_PREFIX_DEBUG1,        ///< デバッグ用。
    DWC_LOBBY_CHANNEL_PREFIX_DEBUG2,        ///< デバッグ用。
    DWC_LOBBY_CHANNEL_PREFIX_DEBUG3,        ///< デバッグ用。
    DWC_LOBBY_CHANNEL_PREFIX_DEBUG4,        ///< デバッグ用。
    DWC_LOBBY_CHANNEL_PREFIX_DEBUG5,        ///< デバッグ用。
    DWC_LOBBY_CHANNEL_PREFIX_DEBUG6,        ///< デバッグ用。
    DWC_LOBBY_CHANNEL_PREFIX_DEBUG7,        ///< デバッグ用。
    DWC_LOBBY_CHANNEL_PREFIX_DEBUG8,        ///< デバッグ用。
    DWC_LOBBY_CHANNEL_PREFIX_DEBUG9,        ///< デバッグ用。
    DWC_LOBBY_CHANNEL_PREFIX_TEST           ///< ライブラリ開発用。(指定しないでください)
}
DWC_LOBBY_CHANNEL_PREFIX;

#ifdef WIN32
#pragma pack(pop)
#endif

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // DWC_LOBBY_H_

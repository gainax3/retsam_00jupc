/*---------------------------------------------------------------------------*
  Project:  NitroDWC Libraries
  File:     ./build/lobby/include/dwci_lobby.h

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
 * @brief ポケモンロビーライブラリ メッセージパーサーヘッダ
 */

#ifndef PPW_PARSER_H_
#define PPW_PARSER_H_

#include "dwci_lobbyUtil.h"
#include "ppwlobby/ppw_lobby.h"

#define PPW_LOBBY_MESSAGE_HEADER_BLOCK_SEPALATOR " "    // スペース1個
#define PPW_LOBBY_MESSAGE_HEADER_BODY_SEPALATOR "  "    // スペース2個
#define PPW_LOBBY_MESSAGE_NO_OPTION "_"

template <class T>
inline T PPW_LobbyToBase64Size(T size)
{
    return (((size + 2) / 3) << 2);
}

class PPW_LobbyMessage : public DWCi_Base
{
public:
    static const u32 CURRENT_MESSAGE_VERSION = 0;
    static const u32 CURRENT_HEADER_BLOCK_NUM = 6;
    static const u32 MAX_MESSAGE_HEADER_SIZE = 40;

    typedef enum FORMAT
    {
        FORMAT_STRING,
        FORMAT_BASE64
    }
    FORMAT;

    typedef enum TARGET
    {
        TARGET_SYSTEM,
        TARGET_APPLICATION
    }
    TARGET;
    
    struct Result : public DWCi_Base
    {
        u32 version;
        u32 blockNum;
        FORMAT format;
        TARGET target;
        s32 type;
        u32 option;
        std::vector<u8, DWCi_Allocator<u8> > data;
        
        Result()
            : version(0)
            , blockNum(CURRENT_HEADER_BLOCK_NUM)
            , format(FORMAT_STRING)
            , target(TARGET_SYSTEM)
            , type(0)
            , option(0)
            , data()
        {}
    };
    
    static BOOL Parse(const DWCi_String& message, Result& result);
    static BOOL Build(FORMAT format, TARGET target, s32 type, const char* message, std::size_t size, DWCi_String& dstStr);
};

#endif

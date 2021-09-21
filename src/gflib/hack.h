/*---------------------------------------------------------------------------*
  Project:  NitroSDK - TRIAL - tests - rebooter
  File:     hack.h

  Copyright 2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NITROSDK_SILESYSTEM_HACK_HEADER__
#define NITROSDK_SILESYSTEM_HACK_HEADER__

#include <nitro.h>

// ★[注意] ポケモンでバトルからプログラムをもらう物は、
//          すべてこの"ADAJ"にしておく。一方でバトルからのプログラムでは
//          必ずこの"ADAJ"でチェックする。
//          なので、変更してはいけない！！
#define GAME_CODE_ADAJ     (('A' << 0) | ('D' << 8) | ('A' << 16) | ('J' << 24))
#define MAKER_CODE_01      (('0' << 0) | ('1' << 8))

/*****************************************************************************/
/* function */

/*
 * FSライブラリ標準のROMアーカイブが指すファイルシステムを、
 * 『現在のプログラム』でなく『本体プログラム』のものに補正します。
 * さらにポケモンダイアモンドのイニシャルコードを使用して、ROMの正当性を
 * チェックしています。(ですので、本関数はポケモン専用関数です。)
 * 本体プログラム自身からこの関数を呼び出した場合は、
 * その準備が行われます。
 *
 * この関数は FS_Init 関数呼び出しの直後に呼び出す必要があります。
 */
SDK_INLINE
void NormalizeRomArchive(void)
{
    if (!FS_IsAvailable())
    {
        OS_TPanic("no archive to replace!");
    }
    else
    {
        static const char name[] = "rom";
        static const int name_len = sizeof(name) - 1;
        const CARDRomHeader* header;
        FSArchive *rom;
        {
            CARDRomHeader * const arg_buffer = (CARDRomHeader *)0x027FF000/*HW_MAIN_MEM_SHARED*/;
            CARDRomHeader * const app_header = (CARDRomHeader *)HW_ROM_HEADER_BUF;
            CARDRomHeader * const org_header = (CARDRomHeader *)HW_CARD_ROM_HEADER;
            if (arg_buffer->game_code == 0)
            {
                // ROMヘッダの内容を退避領域にコピーします。
                CARD_Init();
                MI_CpuCopy8(app_header, arg_buffer, HW_CARD_ROM_HEADER_SIZE);
                MI_CpuCopy8(app_header, org_header, HW_CARD_ROM_HEADER_SIZE);
                arg_buffer->game_code = GAME_CODE_ADAJ;
            }
            header = arg_buffer;
        }
        rom = FS_FindArchive(name, name_len);
        rom->fat = header->fat.offset;
        rom->fat_size = header->fat.length;
        rom->fnt = header->fnt.offset;
        rom->fnt_size = header->fnt.length;
        if (header->game_code != GAME_CODE_ADAJ ||
            header->maker_code != MAKER_CODE_01)
        {
            // 通常あり得ないが、バトルから受け取るべきROMでない。ＮＧ。
            OS_TPanic("illegal rom");
        }
    }
}

#endif /* NITROSDK_SILESYSTEM_HACK_HEADER__ */

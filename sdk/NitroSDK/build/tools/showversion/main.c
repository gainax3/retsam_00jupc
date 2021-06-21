/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - showversion
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.9  2006/01/18 02:11:29  kitase_hirotake
  do-indent

  Revision 1.8  2005/03/29 00:59:03  yasu
  PATCH フィールドの廃止

  Revision 1.7  2005/02/28 05:26:24  yosizaki
  do-indent.

  Revision 1.6  2004/10/28 05:02:20  yasu
  Rewind file pointer after reading rom header if not rom image.

  Revision 1.5  2004/10/28 04:53:17  yasu
  Show '[STATIC MODULE]' if version info is in static module.

  Revision 1.4  2004/10/07 08:59:08  yasu
  Fix for PR6x

  Revision 1.3  2004/10/07 08:16:36  yasu
  Fix to detect 2.0FCx

  Revision 1.2  2004/10/04 12:16:23  yasu
  Support 2.0PR6plus

  Revision 1.1  2004/10/03 08:00:35  yasu
  Add tool to show version information

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "version.h"

#define  LE(a)   ((((a)<<24)&0xff000000)|(((a)<<8)&0x00ff0000)|\
                  (((a)>>8)&0x0000ff00)|(((a)>>24)&0x000000ff))

#define  MAGIC_CODE     0x2106c0de

typedef unsigned char u8;
typedef unsigned short int u16;
typedef unsigned long int u32;
typedef union
{
    u8      byte[4];
    u32     word;
}
tFormat;

typedef struct
{
    u8      reserve[32];               // 
    u32     rom_offset;                // 転送元 ROM オフセット
    u32     entry_address;             // 実行開始アドレス
    u32     ram_address;               // 転送先 RAM アドレス
    u32     size;                      // 転送サイズ
}
tRomHeader;

#define ROMOFFSET_MODULE9       0x00000020      // ARM9 常駐モジュール情報領域

typedef struct
{
    u8      major;
    u8      minor;
    u16     relstep;
    u8      relclass;
    u8      relnumber_major;
    u8      relnumber_minor;
    char   *relname;
}
tVersion;


#define RELCLASS_TEST          0
#define RELCLASS_PR            1
#define RELCLASS_RC            2

#define RELSTEP_PR6            106
#define RELSTEP_FC             200
#define RELSTEP_RELEASE        300

int main(int argc, char *argv[])
{
    u32     buffer[3];
    FILE   *fp;
    tRomHeader romHeader;
    u32     magicCodeBE, magicCodeLE;
    u16     one = 0x0001;
    tFormat f;
    tVersion v;
    u16     relnumber;

    magicCodeBE = *(u8 *)&one ? LE(MAGIC_CODE) : MAGIC_CODE;
    magicCodeLE = LE(magicCodeBE);

    if (argc != 2)
    {
        fprintf(stderr,
                "NITRO-SDK Development Tool - showversion - Show version information\n"
                "Build %lu\n"
                "\n"
                "Usage: showversion ROMFILE\n"
                "\n"
                "  Show NITRO-SDK version information in ROMFILE\n" "\n", SDK_DATE_OF_LATEST_FILE);
        return 5;
    }

    if (NULL == (fp = fopen(argv[1], "rb")))
    {
        fprintf(stderr, "Error: Cannot open '%s'\n", argv[1]);
        return 1;
    }

    if (1 != fread(&romHeader, sizeof(romHeader), 1, fp))
    {
        fclose(fp);
        fprintf(stderr, "Error: Cannot open '%s'\n", argv[1]);
        return 1;
    }

    //
    // ROM ファイルであるかどうかの判定
    //          ここでは実行開始アドレスと転送先 RAM アドレスが
    //          0x02000000-0x027fffff の間であるか判定している．
    //
    if ((romHeader.ram_address & 0xff800000) != 0x02000000 ||
        (romHeader.entry_address & 0xff800000) != 0x02000000)
    {
        //
        // ROM ファイルでなければ ROM ヘッダ情報は破棄(0クリア)し、
        // ファイルポインタを先頭に戻す
        //
        (void)memset(&romHeader, 0, sizeof(tRomHeader));
        rewind(fp);
    }

    v.major = 0;
    buffer[0] = buffer[1] = buffer[2] = 0;

    while (1 == fread(buffer, sizeof(u32), 1, fp))
    {
        //
        // ROM 上で
        //      buffer[2] versionInfo
        //      buffer[1] magicCodeBE
        //      buffer[0] magicCodeLE
        // の順に magicCode が並んでいるのを判定し、versionInfo を表示する
        //
        if (buffer[1] == magicCodeBE && buffer[0] == magicCodeLE)
        {
            f.word = buffer[2];
            v.major = f.byte[3];       // 31..24bit
            v.minor = f.byte[2];       // 23..16bit
            v.relstep = ((u16)f.byte[0]) | (((u16)f.byte[1]) << 8);     // 15...0bit
            v.relclass = v.relstep / 10000;
            relnumber = v.relstep % 10000;
            v.relnumber_major = relnumber / 100;
            v.relnumber_minor = relnumber % 100;

            switch (v.relstep / 100)   // 下3桁の判定
            {
                //
                // 特殊な番号の判定
                //
            case RELSTEP_PR6:
                if (v.relnumber_minor == 50)
                {
                    v.relname = "IPL";
                    v.relnumber_major = 0;
                    v.relnumber_minor = 0;
                }
                else
                {
                    v.relname = "PR";
                    if (v.relnumber_minor > 50)
                    {
                        v.relnumber_minor -= 50;
                    }
                }
                break;

            case RELSTEP_FC:
                v.relname = "FC";
                break;

            case RELSTEP_RELEASE:
                v.relname = "RELEASE";
                break;

            default:

                //
                // 通常の番号の判定(数字が後置されるもの)
                //
                switch (v.relclass)
                {
                case RELCLASS_TEST:
                    v.relname = "test";
                    break;

                case RELCLASS_PR:
                    v.relname = "PR";
                    break;

                case RELCLASS_RC:
                    v.relname = "RC";
                    break;

                default:
                    v.relname = NULL;
                    break;
                }
                break;
            }

            printf("NITRO-SDK VERSION: %d.%02d", v.major, v.minor);

            //
            // リリースクラス名(test/PR/RC e.t.c.) の表示
            //
            if (v.relname)
            {
                printf(" %s", v.relname);

                if (v.relnumber_major)
                {
                    printf("%d", v.relnumber_major);
                }
                if (v.relnumber_minor)
                {
                    printf(" plus");

                    if (v.relnumber_minor > 1)
                    {
                        printf("%d", v.relnumber_minor);
                    }
                }
            }
            else
            {
                printf("XXX [unknown RelID=%d]", v.relstep);
            }

            //
            // 常駐領域内かどうかの判定
            //
            if ((u32)ftell(fp) - romHeader.rom_offset < romHeader.size)
            {
                printf(" [STATIC MODULE]");
            }

            printf("\n");
        }
        buffer[2] = buffer[1];
        buffer[1] = buffer[0];
    }
    fclose(fp);

    //
    // 値の設定が無かったときのメッセージ
    //
    if (v.major == 0)
    {
        fprintf(stderr, "Error: No version information in '%s'\n", argv[1]);
        fprintf(stderr, "(older than 2.0FC?)\n");
        return 1;
    }

    return 0;
}

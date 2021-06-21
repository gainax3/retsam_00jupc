/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - buryarg
  File:     buryarg.c

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: loadrun.c,v $
  Revision 1.15  2006/11/28 04:29:28  yada
  modify help message

  Revision 1.14  2006/01/18 02:11:21  kitase_hirotake
  do-indent

  Revision 1.13  2005/09/05 10:54:13  yada
  --type (device type) option was deleted

  Revision 1.12  2005/09/05 01:34:33  yasu
  version.h を参照しソースファイルの最近のタイムスタンプを表示するようにする

  Revision 1.11  2005/09/01 07:21:24  yada
  change exit status

  Revision 1.10  2005/08/31 12:14:27  yada
  applied for OS_Exit()

  Revision 1.9  2005/08/30 01:47:33  yada
  become to be able to load from stdin

  Revision 1.8  2005/08/29 10:00:01  seiki_masashi
  標準出力のバッファリングを無効化

  Revision 1.7  2005/08/25 02:16:11  yada
  able to use Abort string at QuietMode

  Revision 1.6  2005/08/25 00:35:46  yada
  lap time

  Revision 1.5  2005/08/24 13:02:03  yada
  applied for exec timeout

  Revision 1.4  2005/08/24 12:56:53  yada
  change ctrl-c proc

  Revision 1.3  2005/08/24 11:23:00  yada
  lock option of card and cartridge slot are OFF as default

  Revision 1.2  2005/08/24 09:53:35  yada
  applied for new ISD library

  Revision 1.1  2005/08/24 07:44:46  yada
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <getopt.h>
#include <signal.h>
#include <time.h>
#include <version.h>

#include "ISNITRODLL.h"
#include "isd_api.h"

#define  SDK_BOOL_ALREADY_DEFINED_
#include <nitro_win32.h>
#include <nitro/os/common/system.h>

//---- バージョン文字列
#define VERSION_STRING     " 1.12  Copyright 2005,2006 Nintendo. All right reserved."
// 1.12 ヘルプ表示を修正
// 1.11 デバイスタイプ削除
// 1.10 ソースファイルの最近のタイムスタンプ表示
// 1.9 loadrun自身のexit status を変更した
// 1.8 OS_Exit() からの終了文字列に対応した
// 1.7 標準入力から読めるようにした
// 1.6 標準出力のバッファリングを無効化した
// 1.5 QuietMode でも文字列終了を可能にした
// 1.4 ラップ表示実装
// 1.3 実行タイムアウト実装
// 1.2 ctrl-c で停止したときの処理を変更
// 1.1 ctrl-c で停止したときにスロットをオフにするようにした
// 1.0 公開

//---- exit 番号
#define EXIT_NUM_NO_ERROR               207     // 成功終了(但し、今回はこれになることはない)
#define EXIT_NUM_USER_SIGNAL            206     // ユーザによる強制終了(ctrl-C)
#define EXIT_NUM_EXEC_TIME_OUT          205     // 実行タイムアウトで強制終了
#define EXIT_NUM_TIME_OUT               204     // 表示タイムアウトで強制終了
#define EXIT_NUM_SHOW_DEVICES           203     // デバイスリスト表示で終了
#define EXIT_NUM_SHOW_USAGE             202     // ヘルプ表示で終了
#define EXIT_NUM_SHOW_VERSION           201     // バージョン表示で終了
#define EXIT_NUM_STRING_ABORT           200     // 文字列による強制終了

#define EXIT_NUM_NO_DEVICE              -1      // 使用可能なデバイスがない
#define EXIT_NUM_UNKNOWN_OPTION         -2      // 想定外のオプションが指定された
#define EXIT_NUM_ILLEGAL_OPTION         -3      // オプションの使用方法が違う
#define EXIT_NUM_NO_INPUT_FILE          -4      // 指定のファイルがないorオープンできない
#define EXIT_NUM_NOT_CONNECT            -5      // デバイスへの接続失敗
#define EXIT_NUM_CANNOT_USE_CARTRIDGE   -6      // カートリッジのロックに失敗
#define EXIT_NUM_CANNOT_USE_CARD        -7      // カードのロックに失敗
#define EXIT_NUM_PRINTF_ERROR           -8      // printfデータ扱い中のエラー
#define EXIT_NUM_LOADING_ERROR          -9      // ロード中のエラー

//---- デバイス指定用
char   *gDeviceName[] = {
    "CGBUSB", "CGBSCSI", "NITROUSB", "NITROUIC", NULL
};
int     gDeviceTypeArray[] = {
    ISNTD_DEVICE_CGB_EMULATOR_USB,
    ISNTD_DEVICE_CGB_EMULATOR_SCSI,
    ISNTD_DEVICE_IS_NITRO_EMULATOR,
    ISNTD_DEVICE_IS_NITRO_UIC
};

//---- 動作モード
BOOL    gQuietMode = FALSE;            // quiet モード
BOOL    gVerboseMode = FALSE;          // verbose モード
BOOL    gDebugMode = FALSE;            // debug モード

BOOL    gStdinMode = FALSE;            // stdin モード

BOOL    gIsTypeSpecified = FALSE;      // デバイスタイプ指定ありか？
int     gSpecifiedType;                // ある場合のデバイスタイプ

BOOL    gIsSerialSpecified = FALSE;    // シリアル番号指定ありか？
int     gSpecifiedSerial;              // ある場合のシリアル番号

BOOL    gIsCartridgeLocked = FALSE;    // カートリッジスロットをロックするか
BOOL    gIsCardLocked = FALSE;         // カードスロットをロックするか

int     gTimeOutTime = 0;              // タイムアウト時間(秒) (0 はなし)
BOOL    gTimeOutOccured = FALSE;       // タイムアウトが発生したか

int     gExecTimeOutTime = 0;          // 実行タイムアウト時間(秒) (0 はなし)
int     gExecTimeOutOccured = FALSE;   // 実行タイムアウトが発生したか

char   *gAbortString = NULL;           // 強制終了文字列
BOOL    gStringAborted = FALSE;        // 強制終了文字列によって終わったかどうか

BOOL    gExitAborted = FALSE;          // OS_Exit() 終了
int     gExitStatusNum = EXIT_NUM_STRING_ABORT; // OS_Exit()によって終わったときの返り値
int     gExitStrLength;                // 終了文字列のサイズ

//---- NITROライブラリ用
HINSTANCE gDllInstance;
NITRODEVICEHANDLE gDeviceHandle;
NITRODEVICEID gDeviceId;

//---- デバイスに接続したか(スロットのスイッチ用)
BOOL    gDeviceConnected = FALSE;


//---- デバイスリスト
#define DEVICE_MAX_NUM      256
#define DEVICE_SERIAL_NONE  0x7fffffff // means no specified
ISNTDDevice gDeviceList[DEVICE_MAX_NUM];
int     gCurrentDevice = -1;
int     gConnectedDeviceNum = 0;

int     gDeviceTypeSpecified = ISNTD_DEVICE_NONE;
int     gDeviceSerialSpecified = DEVICE_SERIAL_NONE;    // means no specified

//---- 入力ファイル
#define FILE_NAME_MAX_SIZE  1024
FILE   *gInputFile;
char    gInputFileNameString[FILE_NAME_MAX_SIZE];
BOOL    gIsInputFileOpened = FALSE;

//---- 時間
time_t  gStartTime = 0;                // スタート時刻
BOOL    gIsLineHead = TRUE;            // 行頭かどうか
BOOL    gShowLapTime = FALSE;

//---- シグナル
BOOL    gStoppedByUser = FALSE;        // ユーザに止められたかどうか


#define printfIfNotQuiet(...)    do{if(!gQuietMode){printf(__VA_ARGS__);}}while(0)

void    displayErrorAndExit(int exitNum, char *message);
BOOL    outputString(char *buf, int bufSize);

/*---------------------------------------------------------------------------*
  Name:         myExit

  Description:  similar to exit()

  Arguments:    exitNum : exit()番号

  Returns:      None
 *---------------------------------------------------------------------------*/
void myExit(int exitNum)
{
    //---- カートリッジとカードのスロットをオフに
    if (gDeviceConnected)
    {
        (void)ISNTD_CartridgeSlotPower(gDeviceHandle, FALSE);
        (void)ISNTD_CardSlotPower(gDeviceHandle, FALSE);
    }

    //---- DLL領域解放
    ISNTD_FreeDll();

    if (!gQuietMode)
    {
        if (exitNum == EXIT_NUM_USER_SIGNAL)
        {
            printf("\n*** loadrun: stopped by user.\n");
        }
        else if (exitNum == EXIT_NUM_TIME_OUT)
        {
            printf("\n*** loadrun: stopped by print timeout.\n");
        }
        else if (exitNum == EXIT_NUM_EXEC_TIME_OUT)
        {
            printf("\n*** loadrun: stopped by exec timeout.\n");
        }
        else if (gStringAborted)
        {
            printf("\n*** loadrun: stopped by aborting string.\n");
        }
        else if (gExitAborted)
        {
            exitNum = gExitStatusNum;
        }
    }

    exit(exitNum);
}

/*---------------------------------------------------------------------------*
  Name:         listDevice

  Description:  デバイスの一覧表示して終わる

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void listDevice(void)
{
    int     n;

    //---- デバイス読み込み
    gConnectedDeviceNum = ISNTD_GetDeviceList(&gDeviceList[0], DEVICE_MAX_NUM);
    if (gConnectedDeviceNum < 0)
    {
        displayErrorAndExit(EXIT_NUM_NO_DEVICE, "Cannot access devices.");
    }

    printf("---- Connected devices:\n");

    for (n = 0; n < gConnectedDeviceNum; n++)
    {
        switch (gDeviceList[n].type)
        {
        case ISNTD_DEVICE_CGB_EMULATOR_USB:
            printf("%3d: [CGBUSB]  IS-CGB-EMU(USB) serial:%8d\n", n, gDeviceList[n].serial);
            break;
        case ISNTD_DEVICE_CGB_EMULATOR_SCSI:
            printf("%3d: [CGBSCSI]  IS-CGB-EMULATOR serial(host-id):%02d%02d\n", n,
                   gDeviceList[n].host, gDeviceList[n].serial);
            break;
        case ISNTD_DEVICE_IS_NITRO_EMULATOR:
            printf("%3d: [NITROUSB]  IS-NITRO-EMULATOR serial:%08d\n", n, gDeviceList[n].serial);
            break;
        case ISNTD_DEVICE_IS_NITRO_UIC:
            printf("%3d: [NITROUIC]  IS-NITRO-UIC serial: %08d\n", n, gDeviceList[n].serial);
            break;
        case ISNTD_DEVICE_UNKNOWN:
            printf("%3d: unknown device %x\n", n, (int)gDeviceList[n].ntdId);
            break;
        default:
            printf("Illegal device\n");
            break;
        }
    }

    //---- 検索結果
    printf("%d device(s) found.\n", gConnectedDeviceNum);

    myExit(EXIT_NUM_SHOW_DEVICES);
}

/*---------------------------------------------------------------------------*
  Name:         searchDevice

  Description:  search device

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void searchDevice(void)
{
    //---- デバイスが無かった場合
    if (gConnectedDeviceNum <= 0)
    {
        displayErrorAndExit(EXIT_NUM_NO_DEVICE, "found no device.");
    }

    //---- なにかの指定があれば
    if (gDeviceTypeSpecified != ISNTD_DEVICE_NONE || gDeviceSerialSpecified != DEVICE_SERIAL_NONE)
    {
        int     n;
        gCurrentDevice = -1;
        for (n = 0; n < gConnectedDeviceNum; n++)
        {
            //---- 指定デバイスとのマッチ判定
            if (gDeviceTypeSpecified != ISNTD_DEVICE_NONE
                && gDeviceTypeSpecified != gDeviceList[n].type)
            {
                continue;
            }

            //---- 指定シリアルとのマッチ判定
            if (gDeviceSerialSpecified != DEVICE_SERIAL_NONE
                && gDeviceSerialSpecified != gDeviceList[n].serial)
            {
                continue;
            }

            gCurrentDevice = n;
            break;
        }
    }
    //---- 指定がないなら最初のもの
    else
    {
        gCurrentDevice = 0;
    }

    //---- 指定したものがない、または間違っている
    if (gCurrentDevice < 0
        || gDeviceList[gCurrentDevice].type == ISNTD_DEVICE_NONE
        || gDeviceList[gCurrentDevice].type == ISNTD_DEVICE_UNKNOWN)
    {
        displayErrorAndExit(EXIT_NUM_NO_DEVICE, "illegal device.");
    }
}

/*---------------------------------------------------------------------------*
  Name:         displayUsage

  Description:  使い方を表示する

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void displayUsage(void)
{
    fprintf(stderr,
            "NITRO-SDK Development Tool - loadrun - Execute NITRO ROM image\n"
            "Build %lu\n\n"
            "Usage: loadrun [OPTION] <SrlFile>\n"
            "\t\tdownload Nitro srl file to debugger and execute.\n\n"
            "Options:\n"
            "  --version                   : Show version.\n"
            "  -h, --help                  : Show this help.\n"
            "  -q, --quiet                 : Quiet mode.\n"
            "  -v, --verbose               : Verbose mode.\n"
            "  -L, --list                  : List connecting device.\n"
            "  -l, --lap                   : Show lap time at each line.\n"
//            "  -d, --type=DEVICE           : Specify device type.\n"
//            "                                DEVICE=CGBUSB|CGBSCSI|NITROUSB|NITROUIC.\n"
            "  -s, --serial=SERIAL         : Specify serial number.\n"
            "  -t, --timeout=SECOND        : Specify quit time after last print.\n"
            "  -T, --exec-timeout=SECOND   : Specify quit time after execute program.\n"
            "  -a, --abort-string=STRING   : Specify aborting string.\n"
            "  -c, --card-slot=SWITCH      : Card      slot SWITCH=ON|OFF, default OFF.\n"
            "  -C, --cartridge-slot=SWITCH : Cartridge slot SWITCH=ON|OFF, default OFF.\n"
            "  --stdin, --standard-input   : Read data from stdin instead of <SrlFile>.\n\n",
            SDK_DATE_OF_LATEST_FILE);
}

/*---------------------------------------------------------------------------*
  Name:         displayVersion

  Description:  バージョン表示する

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void displayVersion(void)
{
    printf("*** loadrun: %s\n", VERSION_STRING);
}

/*---------------------------------------------------------------------------*
  Name:         displayErrorAndExit

  Description:  エラーを表示する

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void displayErrorAndExit(int exitNum, char *message)
{
    printf("*** loadrun: Error: %s\n", message);

    //---- DLL領域解放
    ISNTD_FreeDll();

    exit(exitNum);
}

/*---------------------------------------------------------------------------*
  Name:         parseOption

  Description:  parse option line

  Arguments:    argc : argument count
                argv : argument vector

  Returns:      result. less than 0 if error.
 *---------------------------------------------------------------------------*/
void parseOption(int argc, char *argv[])
{
    int     n;
    int     c;
    BOOL    helpFlag = FALSE;

    struct option optionInfo[] = {
        {"help", no_argument, NULL, 'h'},
        {"quiet", no_argument, NULL, 'q'},
        {"verbose", no_argument, NULL, 'v'},
        {"list", no_argument, NULL, 'L'},
        {"lap", no_argument, NULL, 'l'},
        {"debug", no_argument, NULL, 'D'},      //隠しオプション
        {"version", no_argument, NULL, '1'},
        {"stdin", no_argument, NULL, 'I'},
        {"standard-input", no_argument, NULL, 'I'},
        {"type", required_argument, 0, 'd'},    //隠しオプション
        {"serial", required_argument, 0, 's'},
        {"timeout", required_argument, 0, 't'},
        {"exec-timeout", required_argument, 0, 'T'},
        {"abort-string", required_argument, 0, 'a'},
        {"card-slot", required_argument, 0, 'c'},
        {"cartridge-slot", required_argument, 0, 'C'},
        {NULL, 0, 0, 0}
    };
    int     optionIndex;

    char   *optionStr = NULL;

    //---- suppress error string of getopt_long()
    opterr = 0;

    while (1)
    {
        c = getopt_long(argc, argv, "+hqvlLDd:s:t:T:a:c:C:", &optionInfo[0], &optionIndex);

        //printf("optind=%d optopt=%d  %x(%c) \n", optind, optopt, c,c );

        if (c == -1)
        {
            break;
        }

        switch (c)
        {
        case 'I':                     //---- 標準入力
            gStdinMode = TRUE;
            break;
        case 'h':                     //---- ヘルプ表示
            helpFlag = TRUE;
            break;
        case 'q':                     //---- quietモード
            gQuietMode = TRUE;
            break;
        case 'v':                     //---- verboseモード
            gVerboseMode = TRUE;
            break;
        case 'D':                     //---- debugモード
            gDebugMode = TRUE;
            break;
        case '1':                     //---- バージョン表示
            displayVersion();
            myExit(EXIT_NUM_SHOW_VERSION);
            break;
        case 'L':                     //---- デバイスの一覧
            listDevice();
            break;
        case 'l':                     //---- ラップタイム
            gShowLapTime = TRUE;
            break;
        case 'd':                     //---- デバイス
            optionStr = (char *)(optarg + ((*optarg == '=') ? 1 : 0));
            {
                int     n;
                for (n = 0; gDeviceName[n]; n++)
                {
                    if (!strcmp(optionStr, gDeviceName[n]))
                    {
                        gDeviceTypeSpecified = gDeviceTypeArray[n];
                        break;
                    }
                }

                if (gDeviceTypeSpecified == ISNTD_DEVICE_NONE)
                {
                    displayErrorAndExit(EXIT_NUM_ILLEGAL_OPTION, "illegal device type.");
                }
            }
            break;
        case 's':                     //---- シリアル指定
            optionStr = (char *)(optarg + ((*optarg == '=') ? 1 : 0));
            gDeviceSerialSpecified = atoi(optionStr);
            break;
        case 'c':                     //---- カードスロットロック
            optionStr = (char *)(optarg + ((*optarg == '=') ? 1 : 0));
            if (!strcmp(optionStr, "ON") || !strcmp(optionStr, "on"))
            {
                gIsCardLocked = TRUE;
            }
            else if (!strcmp(optionStr, "OFF") || !strcmp(optionStr, "off"))
            {
                gIsCardLocked = FALSE;
            }
            else
            {
                displayErrorAndExit(EXIT_NUM_ILLEGAL_OPTION, "illegal value for card slot option.");
            }
            break;
        case 'C':                     //---- カートリッジスロットロック
            optionStr = (char *)(optarg + ((*optarg == '=') ? 1 : 0));
            if (!strcmp(optionStr, "ON") || !strcmp(optionStr, "on"))
            {
                gIsCartridgeLocked = TRUE;
            }
            else if (!strcmp(optionStr, "OFF") || !strcmp(optionStr, "off"))
            {
                gIsCartridgeLocked = FALSE;
            }
            else
            {
                displayErrorAndExit(EXIT_NUM_ILLEGAL_OPTION,
                                    "illegal value for cartridge slot option.");
            }
            break;
        case 't':                     //---- 最終表示からのタイムアウト時間
            optionStr = (char *)(optarg + ((*optarg == '=') ? 1 : 0));
            gTimeOutTime = atoi(optionStr);
            if (gTimeOutTime <= 0)
            {
                displayErrorAndExit(EXIT_NUM_ILLEGAL_OPTION,
                                    "illegal value for abort timeout option.");
            }
            break;
        case 'T':                     //---- 実行タイムアウト時間
            optionStr = (char *)(optarg + ((*optarg == '=') ? 1 : 0));
            gExecTimeOutTime = atoi(optionStr);
            if (gExecTimeOutTime <= 0)
            {
                displayErrorAndExit(EXIT_NUM_ILLEGAL_OPTION,
                                    "illegal value for abort exec timeout option.");
            }
            break;
        case 'a':                     //---- 終了文字列
            gAbortString = (char *)(optarg + ((*optarg == '=') ? 1 : 0));
            {
                int     length = strlen(gAbortString);
                if (length <= 0 || length > 256)
                {
                    displayErrorAndExit(EXIT_NUM_ILLEGAL_OPTION,
                                        "illegal value for abort string option.");
                }
            }
            break;
        default:
            displayErrorAndExit(EXIT_NUM_UNKNOWN_OPTION, "unknown option.");
        }
    }

    //---- ヘルプ表示
    {
        BOOL    isDisplayHelp = FALSE;

        if (helpFlag)
        {
            isDisplayHelp = TRUE;
        }
        else if (argc <= optind && !gStdinMode)
        {
            isDisplayHelp = TRUE;
        }
        else if (argc > optind && gStdinMode)
        {
            isDisplayHelp = TRUE;
        }

        if (isDisplayHelp)
        {
            displayUsage();
            exit(EXIT_NUM_SHOW_USAGE);
        }
    }

    //---- 入力ファイル名
    if (!gStdinMode)
    {
        strncpy(gInputFileNameString, argv[optind], FILE_NAME_MAX_SIZE);
    }

    if (gVerboseMode)
    {
        if (!gStdinMode)
        {
            printf("Input file is [%s]\n", gInputFileNameString);
        }
        else
        {
            printf("Input file is stdin\n");
        }
    }

    //---- 情報表示
    if (gVerboseMode)
    {
        printf("Print time out : %d sec.\n", gTimeOutTime);
        printf("Execute time out : %d sec.\n", gExecTimeOutTime);
        printf("Card lock : %s.\n", (gIsCardLocked) ? "ON" : "OFF");
        printf("Cartridge lock : %s.\n", (gIsCartridgeLocked) ? "ON" : "OFF");

        if (gAbortString)
        {
            printf("Abort string : [%s]\n", gAbortString);
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         loadFile

  Description:  load file

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void loadFile(void)
{
    unsigned int address = 0;
    //int fileSize;

    //---- ファイルオープン
    if (gStdinMode)
    {
        gInputFile = stdin;
        _setmode(_fileno(gInputFile), O_BINARY);
    }
    else
    {
        if ((gInputFile = fopen(gInputFileNameString, "rb")) == NULL)
        {
            displayErrorAndExit(EXIT_NUM_NO_INPUT_FILE, "cannot open input file.");
        }
    }
    gIsInputFileOpened = TRUE;

    //---- デバイスへの接続
    if ((gDeviceHandle = ISNTD_DeviceOpen(gDeviceList[gCurrentDevice].ntdId)) == NULL)
    {
        displayErrorAndExit(EXIT_NUM_NOT_CONNECT, "cannot connect device.");
    }
    gDeviceConnected = TRUE;

    //---- リセットの発行
    ISNTD_Reset(gDeviceHandle, TRUE);
    Sleep(1000);

    //---- ファイルサイズ取得
    //fseek( gInputFile, 0L, SEEK_END );
    //fileSize = ftell( gInputFile );
    //fseek( gInputFile, 0L, SEEK_SET );

    //---- 16KBずつ転送する
    while (1)
    {
        char    buf[16384];
        size_t  size = fread(buf, 1, sizeof(buf), gInputFile);
        static int progressCount = 0;

        if (!size)
        {
            break;
        }

        //---- 転送
        if (!ISNTD_WriteROM(gDeviceHandle, buf, address, size))
        {
            displayErrorAndExit(EXIT_NUM_LOADING_ERROR, "troubled while loading input file.");
        }

        address += size;

        if (gVerboseMode)
        {
            if (!(progressCount++ % 32))
            {
                printf("*");
            }
        }
    }

    //---- ファイルクローズ
    if (gStdinMode)
    {
        _setmode(_fileno(gInputFile), O_TEXT);
    }
    else
    {
        fclose(gInputFile);
    }
    gIsInputFileOpened = FALSE;

    if (gVerboseMode)
    {
        printf("\nInput file size: %d (0x%x) byte\n", address, address);
    }
}

/*---------------------------------------------------------------------------*
  Name:         setSlopPower

  Description:  スロットのパワー処理

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void setSlotPower(void)
{
    //---- カートリッジスロットをロックする
    if (gIsCartridgeLocked)
    {
        if (!ISNTD_CartridgeSlotPower(gDeviceHandle, TRUE))
        {
            displayErrorAndExit(EXIT_NUM_CANNOT_USE_CARTRIDGE, "cannot use cartridge slot.");
        }
    }

    //---- カードスロットをロックする
    if (gIsCardLocked)
    {
        if (!ISNTD_CardSlotPower(gDeviceHandle, TRUE))
        {
            displayErrorAndExit(EXIT_NUM_CANNOT_USE_CARD, "cannot use card slot.");
        }
    }

    //---- リセットの解除
    Sleep(1000);
    ISNTD_Reset(gDeviceHandle, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         procPrint

  Description:  printf処理

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
NITROArch archNum[] = {
    NITROArchARM9,
    NITROArchARM7
};

#define PRINT_ONETIME_SIZE  512
//#define PRINT_ONETIME_SIZE    16

//---- 結合して表示するためのバッファ
static char gConbineBuf[PRINT_ONETIME_SIZE * 2 + 2] = "\0";
static char *gConbineBufPtr = &gConbineBuf[0];

//---- 結合して文字列を比較するためのバッファ
static char gLineBuf[PRINT_ONETIME_SIZE + 1];


void procPrintf(void)
{
    int     blankTime = 0;

    //---- 終了文字列のサイズ
    gExitStrLength = strlen(OS_EXIT_STRING_1);

    while (1)
    {
        BOOL    isOutputString = FALSE;

        //---- ユーザに止められた場合
        if (gStoppedByUser)
        {
            myExit(EXIT_NUM_USER_SIGNAL);
        }

        //printf("[######]\n" );
        {
            int     n;
            int     dataSize;
            char    tmpBuf[PRINT_ONETIME_SIZE + 1];

            for (n = 0; n < 2; n++)
            {
                //---- 表示データ取得
                if (!ISNTD_GetDebugPrint
                    (gDeviceHandle, archNum[n], tmpBuf, &dataSize, PRINT_ONETIME_SIZE))
                {
                    ISNTD_DeviceClose(gDeviceHandle);
                    displayErrorAndExit(EXIT_NUM_PRINTF_ERROR,
                                        "troubled while receiving print data.");
                }
                tmpBuf[dataSize] = '\0';

                //---- 表示データがあるなら表示
                if (dataSize)
                {
                    //---- 表示した、と記憶
                    isOutputString = TRUE;

                    //---- 出力
                    if (!outputString(tmpBuf, dataSize))
                    {
                        break;
                    }
                }
            }
        }

        //---- 終了するか
        if (gStringAborted || gExitAborted)
        {
            break;
        }

        //---- 文字表示していない場合
        if (!isOutputString)
        {
            Sleep(100);
            blankTime += 100;

            //---- タイムアウト判定
            if (gTimeOutTime && blankTime > gTimeOutTime * 1000)
            {
                gTimeOutOccured = TRUE;
                break;
            }
        }
        //---- 文字表示した場合
        else
        {
            blankTime = 0;
        }

        //---- タイムアウト調査
        if (gExecTimeOutTime > 0)
        {
            time_t  currentTime;
            (void)time(&currentTime);

            if (currentTime - gStartTime >= gExecTimeOutTime)
            {
                gExecTimeOutOccured = TRUE;
                break;
            }
        }
    }
}


/*---------------------------------------------------------------------------*
  Name:         showLapTime

  Description:  display lap time at line head

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void showLapTime(void)
{
    int     lap;
    time_t  currentTime;

    (void)time(&currentTime);
    lap = currentTime - gStartTime;

    printf("{%d:%02d}", lap / 60, lap % 60);
}

/*---------------------------------------------------------------------------*
  Name:         outputString

  Description:  output string to stdout

  Arguments:    buf     : buffer
                bufSize : data size in buffer

  Returns:      FALSE if to do quit.
 *---------------------------------------------------------------------------*/
BOOL outputString(char *buf, int bufSize)
{
    char   *bufEnd = buf + bufSize;
    char   *p = buf;

    int     abortStrLength = gAbortString ? strlen(gAbortString) : 0;

    while (p < bufEnd)
    {
        char   *crPtr = strchr(p, '\n');

        //---- \n がない
        if (!crPtr)
        {
            //----比較用に保存
            strcat(gConbineBufPtr, p);
            gConbineBufPtr += strlen(p);

            //---- 時刻表示
            if (!gQuietMode && gIsLineHead && gShowLapTime)
            {
                showLapTime();
            }
            gIsLineHead = FALSE;

            //---- 表示
            printfIfNotQuiet(p);

            //---- バッファオーバーなら破棄(\nを入れていない出力側の責任)
            if (gConbineBufPtr - &gConbineBuf[0] > PRINT_ONETIME_SIZE)
            {
                gConbineBufPtr = &gConbineBuf[0];
                *gConbineBufPtr = '\0';
            }

            break;
        }

        //---- \n までをコピー
        {
            int     n = crPtr - p + 1;

            //---- 比較用に結合
            strncpy(gConbineBufPtr, p, n);
            gConbineBufPtr[n] = '\0';

            //---- 表示用
            strncpy(&gLineBuf[0], p, n);
            gLineBuf[n] = '\0';
        }

        //---- 時刻表示
        if (!gQuietMode && gIsLineHead && gShowLapTime)
        {
            showLapTime();
        }
        gIsLineHead = TRUE;

        //---- 行表示
        printfIfNotQuiet(gLineBuf);

        //---- 強制終了文字列との比較
        if (gAbortString && !strncmp(gConbineBuf, gAbortString, abortStrLength))
        {
            gStringAborted = TRUE;
            return FALSE;
        }

        //---- OS_Exit による終了
        if (!strncmp(gConbineBuf, OS_EXIT_STRING_1, gExitStrLength))
        {
            gExitAborted = TRUE;
            gExitStatusNum = atoi(gConbineBuf + gExitStrLength);
            return FALSE;
        }

        gConbineBufPtr = &gConbineBuf[0];
        *gConbineBufPtr = '\0';

        p = crPtr + 1;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         signalHandler

  Description:  signal handler

  Arguments:    sig
                argv : argument vector

  Returns:      ---
 *---------------------------------------------------------------------------*/
void signalHandler(int sig)
{
    gStoppedByUser = TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         main

  Description:  main proc

  Arguments:    argc : argument count
                argv : argument vector

  Returns:      ---
 *---------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    //---- DLL 初期化
    ISNTD_InitDll();

    //---- 標準出力のバッファリングの無効化
    setvbuf(stdout, NULL, _IONBF, 0);

    //---- オプション解析
    parseOption(argc, argv);

    //---- デバイス読み込み
    gConnectedDeviceNum = ISNTD_GetDeviceList(&gDeviceList[0], DEVICE_MAX_NUM);
    if (gConnectedDeviceNum < 0)
    {
        displayErrorAndExit(EXIT_NUM_NO_DEVICE, "Cannot access devices.");
    }

    //---- デバイス検索
    searchDevice();

    //---- 読み込み
    loadFile();

    //---- シグナル設定
    (void)signal(SIGINT, signalHandler);

    //---- スロット
    setSlotPower();

    //---- スタート時刻取得
    (void)time(&gStartTime);

    //----printf処理
    procPrintf();

    //---- 終了
    if (gExitAborted)                  //---- OS_Exit() 終了
    {
        myExit(gExitStatusNum);
    }
    else if (gStringAborted)           //---- 文字列終了
    {
        myExit(EXIT_NUM_STRING_ABORT);
    }
    else if (gTimeOutOccured)          //---- タイムアウト
    {
        myExit(EXIT_NUM_TIME_OUT);
    }
    else if (gExecTimeOutOccured)
    {
        myExit(EXIT_NUM_EXEC_TIME_OUT);
    }
    else                               //---- 通常終了
    {
        myExit(EXIT_NUM_NO_ERROR);
    }
    //---- never reached here

    //---- dummy to avoid warning
    return 0;
}

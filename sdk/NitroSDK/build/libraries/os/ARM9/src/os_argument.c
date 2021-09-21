/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS
  File:     os_argument.c

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_argument.c,v $
  Revision 1.11  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.10  2005/10/07 06:24:24  yada
  small fix

  Revision 1.9  2005/09/09 05:20:33  yada
  small fix

  Revision 1.8  2005/09/09 04:35:48  yada
  add conversion from string to arguments

  Revision 1.7  2005/09/06 10:16:42  yasu
  '-' モード時の返り値の修正

  Revision 1.6  2005/09/06 09:10:37  yasu
  '-' モードと '+' モードの実装

  Revision 1.5  2005/08/30 10:35:28  yasu
  オプションでない通常の引数もオプションと透過的に扱えるようにした

  Revision 1.4  2005/08/30 09:00:14  yasu
  OS_GetOpt 追加

  Revision 1.3  2005/07/21 12:41:50  yada
  consider for NITRO_FINALROM

  Revision 1.2  2005/07/21 09:44:36  yada
  fix bury rule

  Revision 1.1  2005/07/21 02:31:41  yada
  initial release

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/os.h>

//---- This area is for argument string, and may be
//     modified from external tools.
#ifndef OS_NO_ARGUMENT
#include <nitro/version_begin.h>
static OSArgumentBuffer OSi_ArgumentBuffer = {
    OS_ARGUMENT_ID_STRING,
    OS_ARGUMENT_BUFFER_SIZE,
    {'\0', '\0'},
};
#include <nitro/version_end.h>

//---- pointer to current argument buffer
const char *sCurrentArgBuffer = &OSi_ArgumentBuffer.buffer[0];

static const char *match_opt(int optchar, const char *optstring);
#endif //ifndef OS_NO_ARGUMENT

const char *OSi_OptArg = NULL;
int     OSi_OptInd = 1;
int     OSi_OptOpt = 0;


/*---------------------------------------------------------------------------*
  Name:         OS_GetArgc

  Description:  Get number of valid arguments.
                This function is for debug.

  Arguments:    None

  Returns:      number of valid arguments.
                if no argument, return 1.
                if not set argument buffer, return 0.
 *---------------------------------------------------------------------------*/
#ifndef OS_NO_ARGUMENT
int OS_GetArgc(void)
{
    int     n = 0;
    const char *p = (const char *)sCurrentArgBuffer;

    for (; *p; p++, n++)
    {
        while (*p)
        {
            p++;
        }
    }

    return n;
}
#endif // ifndef OS_NO_ARGUMENT

/*---------------------------------------------------------------------------*
  Name:         OS_GetArgv

  Description:  Get the pointer to the specified argument string.
                This function is for debug.

  Arguments:    n : index of argument. n==1 means the first argument.

  Returns:      n must less than value of OS_GetArgc()
 *---------------------------------------------------------------------------*/
#ifndef OS_NO_ARGUMENT
const char *OS_GetArgv(int n)
{
    const char *p = (const char *)sCurrentArgBuffer;

    SDK_ASSERT(n >= 0);

    for (; *p && n > 0; p++, n--)
    {
        while (*p)
        {
            p++;
        }
    }

    return (*p) ? p : NULL;
}
#endif // ifndef OS_NO_ARGUMENT


/*---------------------------------------------------------------------------*
  Name:         OS_GetOpt

  Description:  getopt() like function to get command line options
  
  Arguments:    optstring  オプションキャラクタの列
                           NULL なら内部パラメータがリセットされる

  Returns:      オプション文字コード
                '?' なら不明なオプション文字コード
                -1  ならオプションが存在しない
 *---------------------------------------------------------------------------*/
#ifndef OS_NO_ARGUMENT
int OS_GetOpt(const char *optstring)
{
    static BOOL end_of_option = FALSE;
    int     optchar;
    const char *arg;
    const char *opt;
    const char *optarg;

    OSi_OptArg = NULL;
    OSi_OptOpt = 0;

    // optstring が NULL ならリセット
    if (optstring == NULL)
    {
        OSi_OptInd = 1;
        end_of_option = FALSE;
        return 0;
    }

    // コマンドライン引数取得
    arg = OS_GetArgv(OSi_OptInd);

    if (arg == NULL)
    {
        return -1;
    }

    if (optstring[0] == '-')           // Minus Mode
    {
        OSi_OptInd++;                  // 引数消費

        // '-' から始まらない場合は通常引数
        if (end_of_option || arg[0] != '-')
        {
            OSi_OptArg = arg;          // 通常引数も OptArg にセットされる．
            return 1;
        }

        // オプション解析
        optchar = arg[1];

        if (optchar == '-')            // '--' でオプション終了
        {
            end_of_option = TRUE;      // 次からは通常引数
            return OS_GetOpt(optstring);
        }
    }
    else                               // normal mode
    {
        // '-' から始まらない場合は通常引数
        if (end_of_option || arg[0] != '-')
        {
            return -1;                 // OptArg は NULL のまま
        }

        OSi_OptInd++;                  // 引数消費

        // オプション解析
        optchar = arg[1];

        if (optchar == '-')            // '--' でオプション終了
        {
            end_of_option = TRUE;      // 次からは通常引数
            return -1;
        }

    }

    opt = match_opt(optchar, optstring);

    if (opt == NULL)
    {
        OSi_OptOpt = optchar;          // 知らないオプション
        return '?';
    }

    if (opt[1] == ':')                 // OptArg の検索指定？
    {
        optarg = OS_GetArgv(OSi_OptInd);

        if (optarg == NULL || optarg[0] == '-')
        {
            if (opt[2] != ':')         // '::' でない？
            {
                OSi_OptOpt = optchar;  // OptArg が無い
                return '?';
            }
            // OptArg が無い
            // OSi_OptArg = NULL
        }
        else
        {
            OSi_OptArg = optarg;
            OSi_OptInd++;              // 引数消費
        }
    }
    return optchar;
}

static const char *match_opt(int optchar, const char *optstring)
{
    if (optstring[0] == '-' || optstring[0] == '+')
    {
        optstring++;
    }

    if (optchar != ':')
    {
        while (*optstring)
        {
            if (optchar == *optstring)
            {
                return optstring;
            }
            optstring++;
        }
    }
    return NULL;
}
#endif // ifndef OS_NO_ARGUMENT

/*---------------------------------------------------------------------------*
  Name:         OS_ConvertToArguments

  Description:  convert string data to arg binary
  
  Arguments:    str     : string
  				cs	    : character to separate
  				buffer  : buffer to store
  				bufSize : max buffer size
 
  Returns:      None.
 *---------------------------------------------------------------------------*/
#ifndef OS_NO_ARGUMENT
void OS_ConvertToArguments(const char *str, char cs, char *buffer, u32 bufSize)
{

    BOOL    isPause = TRUE;
    char   *p = buffer;
    char   *pEnd = buffer + bufSize;
    BOOL    isQuoted = FALSE;

    while (1)
    {
        //---- skip separater
        while (*str == cs && p < pEnd)
        {
            str++;
        }

        //---- store argument string
        while (*str && p < pEnd)
        {
            //---- check quote
            if (*str == '\"')
            {
                isQuoted = (isQuoted == FALSE);
                str++;
                continue;
            }

            //---- found separater
            else if (*str == cs && isQuoted == FALSE)
            {
                break;
            }

            *p++ = *str++;
        }

        //---- 1) reached to the buffer end
        if (p >= pEnd)
        {
            OS_Printf("pend-2=%x\n", pEnd - 2);
            *(pEnd - 2) = '\0';
            *(pEnd - 1) = '\0';
            break;
        }

        //---- 2) reached to the string end
        if (*str == '\0')
        {
            while ((p + 1) >= pEnd)
            {
                p--;
            }
            *p++ = '\0';
            *p = '\0';
            break;
        }

        //---- 3) separater
        if (*str == cs)
        {
            *p++ = '\0';
        }
    }

    //---- end mark
    if (p < pEnd)
    {
        *p++ = '\0';
    }
}
#endif //ifndef OS_NO_ARGUMENT

/*---------------------------------------------------------------------------*
  Name:         OS_SetArgumentBuffer

  Description:  force to set argument buffer.
  
  Arguments:    buffer : argument buffer
 
  Returns:      None.
 *---------------------------------------------------------------------------*/
#ifndef OS_NO_ARGUMENT
void OS_SetArgumentBuffer(const char *buffer)
{
    sCurrentArgBuffer = buffer;
}
#endif //ifndef OS_NO_ARGUMENT

/*---------------------------------------------------------------------------*
  Name:         OS_GetArgumentBuffer

  Description:  get pointer to argument buffer.
  
  Arguments:    None.
 
  Returns:      pointer to argument buffer.
 *---------------------------------------------------------------------------*/
#ifndef OS_NO_ARGUMENT
const char *OS_GetArgumentBuffer(void)
{
    return sCurrentArgBuffer;
}
#endif //ifndef OS_NO_ARGUMENT

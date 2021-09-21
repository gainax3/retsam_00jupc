/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - ntrcomprv
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.3  2006/01/18 02:11:29  kitase_hirotake
  do-indent

  Revision 1.2  2005/02/28 05:26:24  yosizaki
  do-indent.

  Revision 1.1  2004/09/27 09:39:59  yasu
  Add new tool

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include     <stdio.h>
#include     <stdlib.h>
#include     <string.h>
#include     <unistd.h>                // getopt()

#include     "compress.h"
#include     "file.h"
#include     "version.h"

static void usage(void);
static BOOL CompressFile(const char *infile, int headersize, const char *suffix);

/*---------------------------------------------------------------------------*
  Name:         main

  Description:
 *---------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    int     i;
    int     n;
    BOOL    result = FALSE;
    int     headerSize = 0;
    char   *suffix = strdup("_LZ");

    while ((n = getopt(argc, argv, "s:ca::A:97Fe:dhv")) != -1)
    {
        switch (n)
        {
        case 's':                     // Compress Static
            headerSize = atoi(optarg);
            break;

        case '9':                     // Regular setting for ARM9
            headerSize = LOADER_SIZE_ARM9;
            break;

        case '7':                     // Regular setting for ARM7
            headerSize = LOADER_SIZE_ARM7;
            break;

        case 'F':                     // no extra suffix (overwrite mode)
            if (suffix)
                free(suffix);
            suffix = NULL;
            break;

        case 'e':                     // overwrite mode
            if (suffix)
                free(suffix);
            suffix = strdup(optarg);
            break;

        case 'd':                     // Show debug message
            bDebugMode = TRUE;
            break;

        case 'h':
        case 'v':
        default:
            usage();                   // Never returns
            break;
        }

        DebugPrintf("option -%c: %s\n", n, optarg ? optarg : "No ARG");
    }

    argc -= optind;
    argv += optind;

    DebugPrintf("argc=%d  optind=%d\n", argc, optind);

    if (argc > 0)
    {
        for (i = 0; i < argc; i++)
        {
            DebugPrintf("argv[%d] = [%s]\n", i, argv[i]);
            result = CompressFile(argv[i], headerSize, suffix);
            if (!result)
                break;
        }
    }
    else
    {
        usage();
    }

    if (suffix)
        free(suffix);

    if (!result)
    {
        ErrorPrintf("exit...");
        return 1;
    }
    return 0;
}


/*---------------------------------------------------------------------------*
  Name:         usage

  Description:
 *---------------------------------------------------------------------------*/
static void usage(void)
{
    fprintf(stderr,
            "NITRO-SDK Development Tool - ntrcompbw - Compress file backward\n"
            "Build %lu\n"
            "\n"
            "Usage: ntrcompbw [-sHSIZE|-9|-7] [-F] [-eSUFFIX] [-d] [-h] FILE...\n"
            "\n"
            "  Compress files backward\n"
            "\n"
            "    -sHSIZE   Compress without header region specified by HSIZE(bytes)\n"
            "    -9        Compress as ARM9 static module (HEADER_SIZE = 16k)\n"
            "    -7        Compress as ARM7 static module (HEADER_SIZE =  1k)\n"
            "    -F        Overwrite original component files\n"
            "    -eSUFFIX  SUFFIX for output file (default:\"_LZ\")\n"
            "    -d        Show debug messages (for test purpose)\n"
            "    -h        Show this  message\n" "\n", SDK_DATE_OF_LATEST_FILE);

    exit(1);
}


/*---------------------------------------------------------------------------*
  Name:         CompressFile

  Description:
 *---------------------------------------------------------------------------*/
static BOOL CompressFile(const char *infilename, int headersize, const char *suffix)
{
    u8     *buffer = NULL;
    u8     *outfilename = NULL;
    BOOL    result = FALSE;
    int     filesize, compsize;

    filesize = ReadFile(infilename, &buffer);

    if (filesize < 0)
    {
        goto skip_out;
    }
    if (filesize < headersize)
    {
        ErrorPrintf("'%s' too small. must be >= headersize(%d)\n", infilename, headersize);
        goto skip_out;
    }

    compsize = Compress(buffer + headersize, filesize - headersize);
    if (compsize < 0)
    {
        goto skip_out;
    }

    DebugPrintf("Compressed %ld -> %ld\n", filesize, headersize + compsize);

    outfilename = malloc(strlen(infilename) + strlen(suffix) + 1);
    if (!outfilename)
    {
        ErrorPrintf("Cannot allocate memory\n");
        goto skip_out;
    }
    sprintf(outfilename, "%s%s", infilename, suffix);

    if (WriteFile(outfilename, buffer, headersize + compsize))
    {
        result = TRUE;
    }

  skip_out:
    if (buffer)
        free(buffer);
    if (outfilename)
        free(outfilename);
    return result;

}

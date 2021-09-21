/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - buryarg
  File:     buryarg.c

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: buryarg.c,v $
  Revision 1.10  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.9  2005/08/30 07:36:33  yada
  supported for stdout

  Revision 1.8  2005/07/22 09:17:29  yada
  support nlf file

  Revision 1.7  2005/07/22 04:39:08  yada
  replacement by fopen rb+ not rename

  Revision 1.6  2005/07/22 04:21:32  yada
  replacement by fopen rb+ not rename

  Revision 1.5  2005/07/21 11:04:28  yada
  terminate buffer end

  Revision 1.4  2005/07/21 10:48:45  yada
  modified not to remove argument id string

  Revision 1.3  2005/07/21 09:44:55  yada
  fix bury rule

  Revision 1.2  2005/07/21 02:39:42  yada
  error message is output to stderr

  Revision 1.1  2005/07/21 02:33:33  yada
  initial release

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <getopt.h>

#include <nitro_win32.h>

//---- error code
#define ERROR_OVER_AREA  		                0
#define ERROR_NO_FILE     		                1
#define ERROR_CANNOT_OPEN_FILE                  2
#define ERROR_CANNOT_OPEN_REP_FILE              3
#define ERROR_MARK_NOT_FOUND	                4
#define ERROR_BAD_MARK_POSITION	                5
#define ERROR_BAD_ARGUMENT_SIZE                 6
#define ERROR_UNKNOWN_OPTION                    7
#define ERROR_OUTFILE_NOT_SPECIFIED             8
#define ERROR_REPLACEMENTFILE_NOT_SPECIFIED   	9
#define ERROR_REPLACEMENTFILE_TOO_BIG           10
#define ERROR_ILLEGAL_OPTIONS                   11
#define ERROR_FILE_ERROR                        12
#define ERROR_NLFFILE_NOT_SPECIFIED             13
#define ERROR_CANNOT_OPEN_NLF_FILE				14
#define ERROR_ILLEGAL_NLF_FILE					15

//---- version
#define VERSION_STRING     " 1.3  Copyright 2005,2006 Nintendo. All right reserved."
// ver 0.1  initial release
// ver 1.1  modify not to remove argument id string
// ver 1.2  replacement by fopen rb+ not rename
// ver 1.3  nlf file option is available

//---- ouput default added string
#define ADDED_OUTPUT_NAME  ".argAdded"

//---- output for stdout
const char gNameForStdout[] = "-";

//---- argument buffer identification string
const char gArgIdString[] = OS_ARGUMENT_ID_STRING;

//---- input bin file
#define FILE_NAME_MAX_SIZE  1024
FILE   *gInputFile;
char    gInputFileNameString[FILE_NAME_MAX_SIZE];
BOOL    gIsInputFileOpened = FALSE;

//---- output file
FILE   *gOutputFile;
char    gOutputFileNameString[FILE_NAME_MAX_SIZE];
char   *gOutputFileName = NULL;
BOOL    gIsOutputFileOpened = FALSE;

//---- replacement file
FILE   *gReplacementFile;
char   *gReplacementFileName = NULL;
BOOL    gIsReplacementFileOpened = FALSE;

//---- nlf file
FILE   *gNlfFile;
char   *gNlfFileName = NULL;
BOOL    gIsNlfFileOpened = FALSE;
char    gNlfDirectoryNameString[FILE_NAME_MAX_SIZE];

//---- argument buffer for replacement
#define ARGUMENT_BUF_SIZE 0x10000
char    gArgString[ARGUMENT_BUF_SIZE];
int     gArgStringSize;
int     gArgStringIndex = 0;

//---- mode
BOOL    gQuietMode = FALSE;
BOOL    gVerboseMode = FALSE;
BOOL    gReplacementMode = TRUE;
BOOL    gDebugMode = FALSE;
BOOL    gNlfInputMode = FALSE;
BOOL    gStdoutMode = FALSE;

//---- declaration of prototype
void    analyzeNlfFile(void);
void    closeFiles(void);

//================================================================================
//                      メッセージ表示
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         displayMessage

  Description:  display message

  Arguments:    message : message array

  Returns:      None.
 *---------------------------------------------------------------------------*/
void displayMessage(char *message[])
{
    int     n;
    //---- show help messages
    for (n = 0; message[n]; n++)
    {
        printf(message[n]);
    }
}


/*---------------------------------------------------------------------------*
  Name:         displayUsage

  Description:  display how to use.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void displayUsage(void)
{
    char   *usageString[] = {
        "Usage: buryarg [OPTION]... <nitro bin filename> [argument]...\n",
        "Bury argument strings to the nitro bin file.",
        "\n",
        "Options:\n",
        "  --version           : Show version.\n",
        "  -h, --help          : Show this help.\n",
        "  -q, --quiet         : Quiet mode.\n",
        "  -v, --verbose       : Verbose mode.\n",
        "  -r, --remain        : Remain original file.\n",
        "  -o, --output=FILE   : Output file (default: srcfile.argAdded)\n",
        "                        if \"-\" is specified, output to stdout.\n",
        "      --stdout        : Mean '-o-' .\n",
        "  -f, --file=FILE     : Specify replacement buffer data.\n",
        "                        This is used instead of data from arguments.\n",
        "  -d, --debug         : Debug mode. Show replacement buffer.\n",
        0
    };

    displayMessage(usageString);
}

/*---------------------------------------------------------------------------*
  Name:         displayVersion

  Description:  display version

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void displayVersion(void)
{
    printf("buryarg %s\n", VERSION_STRING);
}

/*---------------------------------------------------------------------------*
  Name:         displayError

  Description:  display error

  Arguments:    errorNumber : error no.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void displayError(int errorNumber)
{
    fprintf(stderr, "Error: buryarg: ");

    switch (errorNumber)
    {
    case ERROR_OVER_AREA:
        fprintf(stderr, "arguments too long.\n");
        break;

    case ERROR_NO_FILE:
        fprintf(stderr, "cannot open binary file.\n");
        break;

    case ERROR_CANNOT_OPEN_FILE:
        fprintf(stderr, "cannot open output file.\n");
        break;

    case ERROR_CANNOT_OPEN_REP_FILE:
        fprintf(stderr, "cannot open replacement file.\n");
        break;

    case ERROR_MARK_NOT_FOUND:
        fprintf(stderr, "cannot replace because bin file has no argument id string.\n");
        break;

    case ERROR_BAD_MARK_POSITION:
        fprintf(stderr, "bad argument mark position.\n");
        break;

    case ERROR_BAD_ARGUMENT_SIZE:
        fprintf(stderr, "bad argument size.\n");
        break;

    case ERROR_UNKNOWN_OPTION:
        fprintf(stderr, "unknown option or invalid usage. try --help.\n");
        break;

    case ERROR_OUTFILE_NOT_SPECIFIED:
        fprintf(stderr, "output file is not specified.\n");
        break;

    case ERROR_REPLACEMENTFILE_NOT_SPECIFIED:
        fprintf(stderr, "replacement file is not specified.\n");
        break;

    case ERROR_REPLACEMENTFILE_TOO_BIG:
        fprintf(stderr, "replacement file is too big.\n");
        break;

    case ERROR_ILLEGAL_OPTIONS:
        fprintf(stderr, "specified illegal option set.\n");
        break;

    case ERROR_FILE_ERROR:
        fprintf(stderr, "error occurred in renaming file.\n");
        break;

    case ERROR_NLFFILE_NOT_SPECIFIED:
        fprintf(stderr, "nlf file is not specified.\n");
        break;

    case ERROR_CANNOT_OPEN_NLF_FILE:
        fprintf(stderr, "cannot open nlf file.\n");
        break;

    case ERROR_ILLEGAL_NLF_FILE:
        fprintf(stderr, "illegal nlf file.\n");
        break;

    default:
        fprintf(stderr, "unknown error (%d).\n", errorNumber);
        break;
    }

    closeFiles();
    exit(2);
}


//================================================================================
//                      デバッグ用表示
//================================================================================
//---- for debug
//     display buffer
void displayBuffer(void)
{
    const int showSize = 256;
    int     n;

    for (n = 0; n < showSize; n++)
    {
        int     k = n % 16;
        int     c;

        if (k == 0)
        {
            printf("%4x ", n);
        }

        c = (int)gArgString[n];

        if (0x20 <= c && c <= 0x7f)
        {
            printf("%c  ", gArgString[n]);
        }
        else if (c == 0)
        {
            printf(".  ");
        }
        else
        {
            printf("-  ");
        }

        if (k == 15)
        {
            printf("\n");
        }
    }
}


//================================================================================
//              ファイル名解析
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         getTailFileName

  Description:  search tail point of file name.
                ex. if d:/aaa/bbb.srl was input, output bbb.srl

  Arguments:    fileName     : file name

  Returns:      tail pointer of original string
 *---------------------------------------------------------------------------*/
const char *getTailFileName(const char *fileName)
{
    const char *p = fileName;
    const char *tailPtr = p;

    while (*p)
    {
        if (*p == '\\' || *p == '/' || *p == ':')
        {
            tailPtr = (p + 1);
        }

        p++;
    }

    return tailPtr;
}

//================================================================================
//              オプション解析 と バッファ作成
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         checkOverBuffer

  Description:  check if buffer is over.
				if error occured, never return.  

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void checkOverBuffer(void)
{
    if (gArgStringIndex >= ARGUMENT_BUF_SIZE)
    {
        displayError(ERROR_OVER_AREA);
    }
}

/*---------------------------------------------------------------------------*
  Name:         addString

  Description:  add string to nitro argument string area

  Arguments:    str : string to add

  Returns:      None
 *---------------------------------------------------------------------------*/
void addString(const char *str)
{
    const char *p = str;

    if (!str)
    {
        printf("internal error: tend to add NULL string.\n");
        exit(2);
    }

    while (*p)
    {
        checkOverBuffer();
        gArgString[gArgStringIndex++] = *p++;
    }

    checkOverBuffer();
    gArgString[gArgStringIndex++] = '\0';
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
    BOOL    helpFlag = FALSE;

    int     c;

    struct option optionInfo[] = {
        {"help", no_argument, NULL, 'h'},
        {"quiet", no_argument, NULL, 'q'},
        {"verbose", no_argument, NULL, 'v'},
        {"replacement", no_argument, NULL, 'r'},
        {"version", no_argument, NULL, 'a'},
        {"debug", no_argument, NULL, 'd'},
        {"output", required_argument, 0, 'o'},
        {"stdout", no_argument, NULL, 'O'},
        {"file", required_argument, 0, 'f'},
        {NULL, 0, 0, 0}
    };
    int     optionIndex;

    //---- suppress error string of getopt_long()
    opterr = 0;

    while (1)
    {
        c = getopt_long(argc, argv, "+hqvrdo:f:", &optionInfo[0], &optionIndex);

        //printf("optind=%d optopt=%d  %x(%c) \n", optind, optopt, c,c );

        if (c == -1)
        {
            break;
        }

        switch (c)
        {
        case 'h':
            helpFlag = TRUE;
            break;
        case 'q':
            gQuietMode = TRUE;
            break;
        case 'v':
            gVerboseMode = TRUE;
            break;
        case 'r':
            gReplacementMode = FALSE;
            break;
        case 'd':
            gDebugMode = TRUE;
            break;
        case 'a':
            displayVersion();
            exit(1);
            break;
        case 'o':
            gOutputFileName = (*optarg == '=') ? optarg + 1 : optarg;
            if (!*gOutputFileName)
            {
                displayError(ERROR_OUTFILE_NOT_SPECIFIED);
            }
            gReplacementMode = FALSE;
            break;
        case 'O':
            gOutputFileName = (char *)gNameForStdout;
            gReplacementMode = FALSE;
            break;
        case 'f':
            gReplacementFileName = (*optarg == '=') ? optarg + 1 : optarg;
            if (!*gReplacementFileName)
            {
                displayError(ERROR_REPLACEMENTFILE_NOT_SPECIFIED);
            }
            break;
        default:
            displayError(ERROR_UNKNOWN_OPTION);
        }
    }

    //---- no specified bin file or "-h" of "--help" to display usage
    if (helpFlag || argc <= optind)
    {
        displayUsage();
        exit(1);
    }

    //---- input filename
    strncpy(gInputFileNameString, argv[optind], FILE_NAME_MAX_SIZE);

    //---- check if nlf file
    if ((gNlfFile = fopen(gInputFileNameString, "rb")) == NULL)
    {
        displayError(ERROR_NO_FILE);
    }
    gIsNlfFileOpened = TRUE;

    //---- check nlf id string
    {
        char    idString[4];
        size_t  size = fread(idString, 1, 4, gNlfFile);
        if (size == 4 && !strncmp(idString, (char *)"#NLF", 4))
        {
            gNlfFileName = argv[optind];

            //---- determine input file name
            analyzeNlfFile();
        }
    }

    //---- close file
    fclose(gNlfFile);
    gIsNlfFileOpened = FALSE;

    //---- add string to replacement buffer
    for (n = optind; n < argc; n++)
    {
        if (n == optind)
        {
            if (!gNlfFileName)
            {
                addString(getTailFileName(argv[n]));
            }
        }
        else
        {
            addString(argv[n]);
        }
    }

    //---- end mark
    checkOverBuffer();
    gArgString[gArgStringIndex++] = '\0';
}

//================================================================================
//                      ファイル操作
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         analyzeNlfFile

  Description:  analyze nlf file.
 
  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void analyzeNlfFile(void)
{
    char    lineBuffer[1024];
    BOOL    isFound_T = FALSE;
    BOOL    isFound_9 = FALSE;

    fseek(gNlfFile, 0, SEEK_SET);

    //---- analyze
    while (fgets(lineBuffer, 1024, gNlfFile))
    {
        if (!strncmp(lineBuffer, "T,", 2))
        {
            char   *sp = &lineBuffer[0];
            char   *dp = &gNlfDirectoryNameString[0];

            while (*sp++ != '\"')
            {
            }
            while (*sp != '\"')
            {
                *dp++ = *sp++;
            }
            *dp = '\0';

            isFound_T = TRUE;
        }

        if (!strncmp(lineBuffer, "9,", 2) && isFound_T)
        {
            char   *sp = &lineBuffer[0];
            char   *dp = &gInputFileNameString[0];
            int     count = 0;

            while (*sp && count <= 2)
            {
                if (*sp++ == '\"')
                {
                    count++;
                }
            }
            while (*sp != '\"')
            {
                *dp++ = *sp++;
            }
            *dp = '\0';

            //---- set argv[0]
            addString(getTailFileName(gInputFileNameString));

            isFound_9 = TRUE;
        }
    }

    //---- check if found file name
    if (!isFound_9)
    {
        displayError(ERROR_ILLEGAL_NLF_FILE);
    }
}

/*---------------------------------------------------------------------------*
  Name:         openFiles

  Description:  open original file and output file

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void openFiles(void)
{
    //---- replacement file
    if (gReplacementFileName)
    {
        if (gVerboseMode)
        {
            printf("replacement file    : %s\n", gReplacementFileName);
        }
        if ((gReplacementFile = fopen(gReplacementFileName, "rb")) == NULL)
        {
            displayError(ERROR_CANNOT_OPEN_REP_FILE);
        }
        gIsReplacementFileOpened = TRUE;

        //---- replace buffer
        {
            int     n;
            int     c;

            if (gVerboseMode)
            {
                struct stat repFileStat;
                stat(gReplacementFileName, &repFileStat);
                printf("replacement file size is 0x%x byte (%d byte).\n", (int)repFileStat.st_size,
                       (int)repFileStat.st_size);
            }

            //---- clear buffer
            for (n = 0; n < ARGUMENT_BUF_SIZE; n++)
            {
                gArgString[n] = 0;
            }

            //---- clean buffer
            n = 0;
            while (n < ARGUMENT_BUF_SIZE && (c = fgetc(gReplacementFile)) != EOF)
            {
                gArgString[n] = c;
                n++;
            }
            fclose(gReplacementFile);
            gIsReplacementFileOpened = FALSE;

            //---- check over buffer
            if (c != EOF)
            {
                //displayError( ERROR_REPLACEMENTFILE_TOO_BIG );
                if (!gQuietMode)
                {
                    printf("warning: replacement file is bigger than required size.");
                }
            }
        }
    }

    //---- 元ファイルを書き換えない
    if (!gReplacementMode)
    {
        //---- 書き出しファイル名
        if (gOutputFileName)
        {
            strncpy(gOutputFileNameString, gOutputFileName, FILE_NAME_MAX_SIZE);
        }
        else
        {
            strcat(gOutputFileNameString, gInputFileNameString);
            strncat(gOutputFileNameString, ADDED_OUTPUT_NAME, FILE_NAME_MAX_SIZE);
        }

        //---- check if stdout specified
        if (!strncmp(gOutputFileNameString, "-", 1))
        {
            gStdoutMode = TRUE;
        }

        //---- output file
        if (gVerboseMode)
        {
            if (gStdoutMode)
            {
                printf("destination         : stdout\n");
            }
            else
            {
                printf("destination bin file: %s\n", gOutputFileNameString);
            }
        }
        if (gStdoutMode)
        {
            gOutputFile = stdout;

            //---- set stdout to binary mode
            _setmode(_fileno(gOutputFile), O_BINARY);
        }
        else
        {
            if ((gOutputFile = fopen(gOutputFileNameString, "wb")) == NULL)
            {
                displayError(ERROR_CANNOT_OPEN_FILE);
            }
            gIsOutputFileOpened = TRUE;
        }
    }

    //---- input file
    if (gNlfFileName)
    {
        if (gVerboseMode)
        {
            printf("nlf current directory is %s\n", gNlfDirectoryNameString);
        }
        //---- change current directory
        if (chdir(gNlfDirectoryNameString) != 0)
        {
            displayError(ERROR_ILLEGAL_NLF_FILE);
        }
    }


    if (gVerboseMode)
    {
        printf("original bin file   : %s\n", gInputFileNameString);
    }
    if ((gInputFile = fopen(gInputFileNameString, (gReplacementMode) ? "rb+" : "rb")) == NULL)
    {
        displayError(ERROR_NO_FILE);
    }
    gIsInputFileOpened = TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         closeFiles

  Description:  close files

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void closeFiles(void)
{
    if (gStdoutMode)
    {
        //---- flush stdout
        fflush(stdout);

        //---- set stdout to text mode
        _setmode(_fileno(gOutputFile), O_TEXT);
    }

    if (gIsInputFileOpened)
    {
        fclose(gInputFile);
        gIsInputFileOpened = FALSE;
    }
    if (gIsOutputFileOpened)
    {
        fclose(gOutputFile);
        gIsOutputFileOpened = FALSE;
    }
    if (gIsReplacementFileOpened)
    {
        fclose(gReplacementFile);
        gIsReplacementFileOpened = FALSE;
    }
    if (gIsNlfFileOpened)
    {
        fclose(gNlfFile);
        gIsNlfFileOpened = FALSE;
    }
}

//================================================================================
//                      バッファ置き換え
//================================================================================
void replaceToSpecifiedString(void)
{
    BOOL    isFindMark = FALSE;
    int     markIndex = 0;
    long    bufPosition;
    int     n;
    int     inputChar;

    const int markLength = strlen(gArgIdString);

    //---- search argument idendification string in bin file
    while (1)
    {
        if ((inputChar = fgetc(gInputFile)) == EOF)
        {
            break;
        }

        if (inputChar == gArgIdString[markIndex])
        {
            markIndex++;
            if (markIndex == markLength)
            {
                long    topPosition = ftell(gInputFile) - markLength;
                bufPosition = topPosition + OS_ARGUMENT_ID_STRING_BUFFER_SIZE + sizeof(u16);

                if (gVerboseMode)
                {
                    printf("found argument identification string at 0x%x byte from top.\n",
                           (int)topPosition);
                }
                isFindMark = TRUE;
                break;
            }
        }
        else
        {
            if (markIndex > 0)
            {
                markIndex = 0;
                if (inputChar == gArgIdString[0])
                {
                    markIndex++;
                }
            }
        }
    }

    //---- check if mark not found
    if (!isFindMark)
    {
        displayError(ERROR_MARK_NOT_FOUND);
    }

    //---- position check
    if (bufPosition < 0)
    {
        displayError(ERROR_BAD_MARK_POSITION);
    }

    //---- get buffer size
    fseek(gInputFile, bufPosition - sizeof(u16), SEEK_SET);
    gArgStringSize = (int)(fgetc(gInputFile) + (fgetc(gInputFile) << 8));

    if (gVerboseMode)
    {
        printf("buffer size = 0x%x byte\n", gArgStringSize);
    }

    if (gArgStringSize < 0 || gArgStringSize > ARGUMENT_BUF_SIZE)
    {
        displayError(ERROR_BAD_ARGUMENT_SIZE);
    }

    //---- put forced endmark to replace buffer
    gArgString[gArgStringSize - 2] = '\0';
    gArgString[gArgStringSize - 1] = '\0';

    //---- replacement
    if (gReplacementMode)
    {
        fseek(gInputFile, bufPosition, SEEK_SET);
        for (n = 0; n < gArgStringSize; n++)
        {
            fputc(gArgString[n], gInputFile);
        }

        if (gVerboseMode)
        {
            printf("replaced 0x%x byte.\n", (int)gArgStringSize);
        }
    }
    else
    {
        fseek(gInputFile, 0, SEEK_SET);
        n = 0;
        while ((inputChar = fgetc(gInputFile)) != EOF)
        {
            if (bufPosition <= n && n < bufPosition + gArgStringSize)
            {
                fputc(gArgString[n - bufPosition], gOutputFile);
            }
            else
            {
                fputc(inputChar, gOutputFile);
            }

            n++;
        }

        if (gVerboseMode)
        {
            printf("output 0x%x byte (%d byte).\n", n, n);
        }
    }
}

//================================================================================
//                      メインルーチン
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         main

  Description:  main proc

  Arguments:    argc : argument count
                argv : argument vector

  Returns:      ---
 *---------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    //---- オプション解析
    parseOption(argc, argv);

    //---- ファイルオープン
    openFiles();

    //---- バッファ内表示(デバッグ用)
    if (gDebugMode)
    {
        displayBuffer();
    }

    //---- 置き換え処理
    replaceToSpecifiedString();

    //---- ファイルクローズ
    closeFiles();

    //---- 終了
    if (!gQuietMode)
    {
        printf("Success.\n");
    }
    return 0;
}

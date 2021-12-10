/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - makelcf
  File:     makelcf.c

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: makelcf.c,v $
  Revision 1.13  2007/07/09 12:17:54  yasu
  TARGET_NAME のサポート

  Revision 1.12  2007/02/20 00:28:10  kitase_hirotake
  indent source

  Revision 1.11  2006/05/10 02:06:00  yasu
  CodeWarrior 2.x への対応

  Revision 1.10  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.9  2005/02/28 05:26:03  yosizaki
  do-indent.

  Revision 1.8  2004/08/05 13:50:01  yasu
  Support -M option

  Revision 1.7  2004/08/05 13:38:44  yasu
  Support -M option

  Revision 1.6  2004/07/15 05:20:53  yasu
  Support showing version number at help screen

  Revision 1.5  2004/03/26 05:07:11  yasu
  support variables like as -DNAME=VALUE

  Revision 1.4  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.3  2004/01/14 12:38:08  yasu
  Change OverlayName->OverlayDefs

  Revision 1.2  2004/01/07 13:10:17  yasu
  fix all warning at compile -Wall

  Revision 1.1  2004/01/05 02:32:59  yasu
  Initial version

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include	<stdio.h>
#include	<stdlib.h>             // atoi()
#include	<ctype.h>
#include	<getopt.h>             // getopt()
#include	"makelcf.h"
#include	"defval.h"
#include	"misc.h"
#include	"version.h"

/*---------------------------------------------------------------------------*
 *  MAIN
 *---------------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
    int     n;
    int     narg;
    int     t;
    
    while ((n = getopt(argc, argv, "hdD:M:V:T:")) != -1)
    {
        switch (n)
        {
        case 'h':
            goto usage;

        case 'd':
            DebugMode = TRUE;
            break;
            
        case 'D':
            AddDefVal(optarg);
            break;

        case 'M':
            if (!AddDefValFromFile(optarg))
            {
                fprintf(stderr, "Stop.\n");
                return 1;
            }
            break;

        case 'V':
            t = atoi(optarg);
            if (t >= 1)
            {
                char    str[128];
                sprintf(str, "NITRO_LCFSPEC=%d", t);    // NITRO_LCFSPEC が定義される
                AddDefVal(str);
            }
            else
            {
                fprintf(stderr, "Unknown version number [%s]. Stop.\n", optarg);
                return 1;
            }
            break;

        case 'T':
            if (!StaticSetTargetName(optarg))
            {
                fprintf(stderr, "Target Name already specified [%s]. Stop.\n", optarg);
                return 1;
            }
            break;
            
        default:
            break;
        }
    }
    
    narg = argc - optind;
    
    if (narg == 2 || narg == 3)
    {
        if (ParseSpecFile(argv[optind]))
        {
            fprintf(stderr, "Stop.\n");
            return 1;
        }
        
        if (ParseTlcfFile(argv[optind + 1]))
        {
            fprintf(stderr, "Stop.\n");
            return 1;
        }
        
        if (CreateLcfFile(narg == 3 ? argv[optind + 2] : NULL))
        {
            fprintf(stderr, "Stop.\n");
            return 1;
        }
        return 0;
    }

  usage:
    {
        fprintf(stderr,
                "NITRO-SDK Development Tool - makelcf - Make linker command file\n"
                "Build %lu\n\n"
                "Usage:  makelcf [-DNAME=VALUE...] [-MDEFINES_FILE] [-TTARGET_NAME] [-V1|-V2] SPECFILE LCF-TEMPALTE [LCFILE]\n\n",
                SDK_DATE_OF_LATEST_FILE);
    }
    return 2;
}

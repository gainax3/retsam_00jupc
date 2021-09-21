/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - ppmconv8
  File:     ppmconv8.c

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: ppmconv8.c,v $
  Revision 1.5  2007/02/20 00:28:10  kitase_hirotake
  indent source

  Revision 1.4  2006/03/17 04:53:00  yasu
  photoshop で作成したppmファイル（NetPBMformats Plugin 経由)を読めなかったのを修正

  Revision 1.3  2006/01/18 02:12:27  kitase_hirotake
  do-indent

  Revision 1.2  2005/02/28 05:26:29  yosizaki
  do-indent.

  Revision 1.1  2004/07/28 08:35:00  yasu
  Add ppmconv8

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
//
//  The 'ppmconv8' tool is for $NitroSDK/build/demos/GX/UnitTest/2D_BmpBg_*
//  Please see detail at 2D_BmpBg_* directories.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define	MAX_COLORS		256    // 256 color limits

#define	TRUE			1
#define	FALSE			0

#define NUM_COLUMN_TEXEL	16
#define NUM_COLUMN_PALETTE	16

#define	V5bit( x )		((x)>>3)
#define	RGB5551( r, g, b, a )	((V5bit(r)<<0) | (V5bit(g)<<5) | (V5bit(b)<<10)| (((a)&1)<<15))

#define	SIZE_READBUFFER	1024

enum
{
    PHASE_READ_MAGIC_NUMBER,
    PHASE_READ_WIDTH,
    PHASE_READ_HEIGHT,
    PHASE_READ_DEPTH,
};

typedef unsigned char u8;
typedef unsigned short u16;

typedef struct
{
    u16     color[MAX_COLORS];
    int     num_colors;
}
ColorTable;

static void usage(void)
{
    fprintf(stderr, "Usage: ppmconv8 [256color ppm file] [label_name]\n");
    exit(1);
}

/*---------------------------------------------------------------------------*
  Name:         ReadHeader

  Description:  Read header information of ppm file

  Arguments:    fp      ppm file
                pwidth  output ptr for the width  of ppm picture
                pheight output ptr for the height of ppm picture
                pdepth  output ptr for the depth  of ppm picture

  Returns:      1 if success, 0 if error
 *---------------------------------------------------------------------------*/
static int ReadHeader(FILE * fp, int *pwidth, int *pheight, int *pdepth)
{
    // Get ppm header
    //
    // P6[WS]256[WS]192[WS]255[WS]\n   WS=Space,Tab,CR
    //
    char    buffer[SIZE_READBUFFER], *p;
    int     n;
    int     phase = PHASE_READ_MAGIC_NUMBER;

    while (buffer == fgets(buffer, sizeof(buffer), fp))
    {
        if (buffer[0] == '#')          // comment
        {
            continue;
        }

        p = buffer;
        while (*p != '\0')
        {
            switch (phase)
            {
            case PHASE_READ_MAGIC_NUMBER:
                if (0 != sscanf(p, "P6%n", &n))
                {
                    return FALSE;      // not ppm file
                }
                phase = PHASE_READ_WIDTH;
                break;

            case PHASE_READ_WIDTH:
                if (1 != sscanf(p, "%d%n", pwidth, &n))
                {
                    return FALSE;
                }
                phase = PHASE_READ_HEIGHT;
                break;

            case PHASE_READ_HEIGHT:
                if (1 != sscanf(p, "%d%n", pheight, &n))
                {
                    return FALSE;
                }
                phase = PHASE_READ_DEPTH;
                break;

            case PHASE_READ_DEPTH:
                if (1 != sscanf(p, "%d%n", pdepth, &n))
                {
                    return FALSE;
                }
                return TRUE;

            default:
                break;
            }

            for (p += n; isspace(*p); p++)
            {
                // Do nothing
            }
        }
    }
    return FALSE;
}


/*---------------------------------------------------------------------------*
  Name:         ReadBody

  Description:  Read picture image

  Arguments:    fp      ppm file
                buffer	output ptr for image
                size    buffer size

  Returns:      1 if success, 0 if error
 *---------------------------------------------------------------------------*/
static int ReadBody(FILE * fp, u8 *buffer, int size)
{
    return size == fread(buffer, sizeof(u8), size, fp);
}


/*---------------------------------------------------------------------------*
  Name:         ColorTableInit

  Description:  Initialize Color Table

  Arguments:    None
  
  Returns:      N/A
 *---------------------------------------------------------------------------*/
static void ColorTableInit(ColorTable * t)
{
    int     i;
    for (i = 0; i < MAX_COLORS; i++)
    {
        t->color[i] = 0x0000;
    }
    t->num_colors = 0;
}

/*---------------------------------------------------------------------------*
  Name:         ColorTableAppend

  Description:  Get color index from Color Table

  Arguments:    t      ptr for color table
                color  color to be added
  
  Returns:      >=  0 : index number if color is in color table
                == -1 : error no room to append color
 *---------------------------------------------------------------------------*/
static int ColorTableAppend(ColorTable * t, u16 color)
{
    int     i;

    for (i = 0; i < t->num_colors; i++)
    {
        if (t->color[i] == color)
        {
            return i;
        }
    }

    if (i >= MAX_COLORS)
    {
        return -1;
    }

    t->color[i] = color;
    t->num_colors++;
    return i;
}


/*---------------------------------------------------------------------------*
  Name:         ColorTableGetColor

  Description:  Get color table from Color Table

  Arguments:    None
  
  Returns:      N/A
 *---------------------------------------------------------------------------*/
static u16 ColorTableGetColor(ColorTable * t, int index)
{
    return index < t->num_colors ? t->color[index] : 0x0000;
}


/*---------------------------------------------------------------------------*
  Name:         main

  Description:  output 256 color-ed ppm image bitmap as the format
                like C source code
 *---------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    FILE   *fp;
    u8     *buffer;
    int     result = 1;
    int     width;
    int     height;
    int     depth;
    int     size;
    int     column;
    int     i, index;
    u16     color;
    ColorTable color_table;

    if (argc != 3)
    {
        usage();
    }

    if (NULL != (fp = fopen(argv[1], "rb")))
    {
        if (ReadHeader(fp, &width, &height, &depth) && depth == 255)
        {
            size = width * height * 3;
            if (NULL != (buffer = (u8 *)malloc(size)))
            {
                if (ReadBody(fp, buffer, size))
                {
                    printf("const unsigned char %s_Texel[%d * %d] =\n{\n", argv[2], width, height);
                    printf("\t/* %s: WIDTH=%d HEIGHT=%d DEPTH=%d */\n", argv[1], width, height,
                           depth);

                    column = 0;
                    ColorTableInit(&color_table);
                    for (i = 0; i < size; i += 3)
                    {
                        if (column % NUM_COLUMN_TEXEL == 0)
                        {
                            printf("\t");
                        }
                        color = RGB5551(buffer[i], buffer[i + 1], buffer[i + 2], 1);
                        index = ColorTableAppend(&color_table, color);
                        if (index < 0)
                        {
                            fprintf(stderr, "ppmconv8: Error too many colors (over 256)\n");
                            return 1;
                        }
                        printf("0x%02x, ", index);
                        column++;
                        if (column % NUM_COLUMN_TEXEL == 0)
                        {
                            printf("\n");
                        }
                    }
                    printf("\n};\n\n");
                    printf("const unsigned short %s_Palette[%d] =\n{\n", argv[2], MAX_COLORS);

                    column = 0;
                    for (i = 0; i < MAX_COLORS; i++)
                    {
                        if (column % NUM_COLUMN_PALETTE == 0)
                        {
                            printf("\t");
                        }
                        printf("0x%04x, ", ColorTableGetColor(&color_table, i));
                        column++;
                        if (column % NUM_COLUMN_PALETTE == 0)
                        {
                            printf("\n");
                        }
                    }
                    printf("\n};\n\n");

                    result = 0;
                }
            }
            free(buffer);
        }
        fclose(fp);
    }

    if (result)
    {
        fprintf(stderr, "Cannot convert file \"%s\".\n", argv[1]);
    }

    return result;
}

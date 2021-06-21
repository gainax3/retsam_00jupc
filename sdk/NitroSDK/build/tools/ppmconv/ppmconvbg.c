/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - ppmconvbg
  File:     ppmconvbg.c

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: ppmconvbg.c,v $
  Revision 1.8  2007/02/20 00:28:10  kitase_hirotake
  indent source

  Revision 1.7  2006/03/17 04:53:00  yasu
  photoshop で作成したppmファイル（NetPBMformats Plugin 経由)を読めなかったのを修正

  Revision 1.6  2006/01/18 02:12:27  kitase_hirotake
  do-indent

  Revision 1.5  2005/02/28 05:26:29  yosizaki
  do-indent.

  Revision 1.4  2004/08/11 05:48:35  yasu
  Support -d option to check parameters

  Revision 1.3  2004/08/11 01:39:38  yasu
  Fix a bug around getopt

  Revision 1.2  2004/08/10 05:05:49  yasu
  Fix error message

  Revision 1.1  2004/08/09 09:06:47  yasu
  Add ppmconvbg

  Revision 1.2  2004/08/09 07:59:32  yasu
  add comments

  Revision 1.1  2004/08/09 07:43:07  yasu
  integrated to ppmconvbg

  Revision 1.2  2004/08/09 00:07:52  yasu
  Fix error

  Revision 1.1  2004/08/07 13:42:59  yasu
  Support 16color tool

  Revision 1.1  2004/08/03 10:51:20  yasu
  Add OnScreenWarning

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
//
//  The 'ppmconvbg' tool is for $NitroSDK/build/demos/tips/OnScreenWarning
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>                    // getopt()
#include <ctype.h>

#ifndef	MAX_COLORS
#define	MAX_COLORS		256    // 256 color limits
#endif

#define	TRUE			1
#define	FALSE			0

#define NUM_COLUMN_BYTE		16
#define NUM_COLUMN_HALF		8

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

enum
{
    COLOR_MAX_8bit = 256,
    COLOR_MAX_4bit = 16,
};

enum
{
    PARTS_CLUT = 1,
    PARTS_INDEX = 2,
    PARTS_ALL = 3,
};

typedef unsigned char u8;
typedef unsigned short u16;

typedef struct
{
    u16    *color;
    int     num_colors;
    int     max_colors;
}
ColorTable;

static void usage(char *str, int num)
{
    if (str)
    {
        fprintf(stderr, "Error: ");
        fprintf(stderr, str, num);
        fprintf(stderr, "\n\n");
    }
    fprintf(stderr,
            "Usage: ppmconvbg [-a labelname] [-b] [-c [4|8]] [-p] [-i] ppmfile outputfile \n");
    fprintf(stderr, "\n");
    fprintf(stderr, "  - Read [ppmfile] and convert it to [outputfile]\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "          -a labelname : output as C source code         \n");
    fprintf(stderr, "          -b           : output as binary file   (default:binary)\n");
    fprintf(stderr, "          -c 4 or 8    : color mode 4bit or 8bit (default:8bit  )\n");
    fprintf(stderr, "          -p           : output palette data    \n");
    fprintf(stderr, "          -i           : output color index data\n");
    fprintf(stderr, "\n");

    exit(1);
}

static int Rgb2Index(int num_pixels, u8 *rgb_buffer, u8 *index_buffer, int max_colors,
                     ColorTable * cp);
static void OutputColumnInit(void);
static void OutputByte(FILE * fp, int val);
static void OutputHalf(FILE * fp, int val);
static void OutputIndex(FILE * fp, char *label, int max_colors, int width, int height, u8 *pixel);
static void OutputCLUT(FILE * fp, char *label, ColorTable * cp);

int     DebugMode = FALSE;

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
static void ColorTableInit(ColorTable * t, int max_colors)
{
    if (NULL == (t->color = (u16 *)calloc(max_colors, sizeof(u16))))
    {
        fprintf(stderr, "Cannot allocate memory.");
        exit(1);
    }

    t->num_colors = 0;
    t->max_colors = max_colors;
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

    if (i >= t->max_colors)
    {
        return -1;
    }

    t->color[i] = color;
    t->num_colors++;
    return i;
}


/*---------------------------------------------------------------------------*
  Name:         ColorTableGetColor

  Description:  Get color from Color Table

  Arguments:    None
  
  Returns:      color u16
 *---------------------------------------------------------------------------*/
static u16 ColorTableGetColor(ColorTable * t, int index)
{
    return index < t->num_colors ? t->color[index] : 0x0000;
}


/*---------------------------------------------------------------------------*
  Name:         Rgb2Index

  Description:  Convert Rgb to index

  Arguments:    None
  
  Returns:      
 *---------------------------------------------------------------------------*/
static int Rgb2Index(int num_pixels, u8 *rgb_buffer, u8 *index_buffer, int max_colors,
                     ColorTable * cp)
{
    int     i, n;

    ColorTableInit(cp, max_colors);

    for (i = n = 0; i < num_pixels; i++, n += 3)
    {
        int     index;
        u16     color;

        color = RGB5551(rgb_buffer[n], rgb_buffer[n + 1], rgb_buffer[n + 2], 1);
        index = ColorTableAppend(cp, color);

        if (index < 0)
        {
            fprintf(stderr, "ppmconvbg: Error too many colors (over %d)\n", max_colors);
            return FALSE;
        }

        index_buffer[i] = index;
    }
    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         Output Byte/Half

  Description:  Output Byte

  Arguments:    fp	output file
                val	byte value
  
  Returns:      None
 *---------------------------------------------------------------------------*/
static int OutputColumn = 0;

static void OutputColumnInit(void)
{
    OutputColumn = 0;
}

static void OutputByte(FILE * fp, int val)
{
    if (OutputColumn % NUM_COLUMN_BYTE == 0)
        fprintf(fp, "\t");

    fprintf(fp, "0x%02x, ", val);

    OutputColumn++;
    if (OutputColumn % NUM_COLUMN_BYTE == 0)
        fprintf(fp, "\n");
}

static void OutputHalf(FILE * fp, int val)
{
    if (OutputColumn % NUM_COLUMN_HALF == 0)
        fprintf(fp, "\t");

    fprintf(fp, "0x%04x, ", val);

    OutputColumn++;
    if (OutputColumn % NUM_COLUMN_HALF == 0)
        fprintf(fp, "\n");
}


/*---------------------------------------------------------------------------*
  Name:         Output Color Table

  Description:  Output Color Lookup Table

  Arguments:    fp	output file
                label   label name
                cp      CLUT
  
  Returns:      None
 *---------------------------------------------------------------------------*/
static void OutputCLUT(FILE * fp, char *label, ColorTable * cp)
{
    // Round up num_colors for alignment
    if (cp->num_colors % 2)
    {
        cp->num_colors++;
    }

    // Show color table
    if (label)
    {
        int     i;
        OutputColumnInit();

        fprintf(fp, "const int Num_%s_Palette = %d;\n\n", label, cp->num_colors);
        fprintf(fp, "const unsigned short %s_Palette[] =\n{\n", label);

        for (i = 0; i < cp->num_colors; i++)
        {
            OutputHalf(fp, ColorTableGetColor(cp, i));
        }
        fprintf(fp, "\n};\n\n");
    }
    else
    {
        // Colors
        fwrite(cp->color, sizeof(u16), cp->num_colors, fp);
    }
    return;
}


/*---------------------------------------------------------------------------*
  Name:         Output Index

  Description:  Output Index

  Arguments:    fp	output file
                label   label name
                cp      CLUT
  
  Returns:      None
 *---------------------------------------------------------------------------*/
static void OutputIndex(FILE * fp, char *label, int max_colors, int width, int height, u8 *pixel)
{
    // Show color index
    if (label)
    {
        int     i, j, ii, jj, n;

        OutputColumnInit();

        fprintf(fp, "const int Num_%s_Texel = %d * %d;\n\n", label, width, height);
        fprintf(fp, "const unsigned char %s_Texel[] =\n{\n", label);

        for (i = 0; i < height; i += 8)
        {
            for (j = 0; j < width; j += 8)
            {
                for (ii = 0; ii < 8; ii++)
                {
                    for (jj = 0; jj < 8; jj++)
                    {
                        n = (i + ii) * width + j + jj;

                        if (max_colors == COLOR_MAX_8bit)
                        {
                            OutputByte(fp, pixel[n]);
                        }
                        else if (n % 2 == 1)
                        {
                            OutputByte(fp, (pixel[n] << 4) | (pixel[n - 1] & 15));
                        }
                    }
                }
            }
        }
        fprintf(fp, "\n};\n\n");
    }
    else
    {
        int     i, j, ii, jj, n;
        u8      buffer;

        // Colors
        for (i = 0; i < height; i += 8)
        {
            for (j = 0; j < width; j += 8)
            {
                for (ii = 0; ii < 8; ii++)
                {
                    for (jj = 0; jj < 8; jj++)
                    {
                        n = (i + ii) * width + j + jj;

                        if (max_colors == COLOR_MAX_8bit)
                        {
                            fwrite(&pixel[n], sizeof(u8), 1, fp);
                        }
                        else if (n % 2 == 1)
                        {
                            buffer = (u8)(pixel[n] << 4) | (pixel[n - 1] & 15);
                            fwrite(&buffer, sizeof(u8), 1, fp);
                        }
                    }
                }
            }
        }
    }
    return;
}


/*---------------------------------------------------------------------------*
  Name:         Convert

  Description:  Convert ppm file

  Arguments:    ppmfile		ppm file
                outfile		output file
                colormax	max of color
                valuelabel      labelname for C source mode if not NULL
                                binary mode if NULL
                
  Returns:      
 *---------------------------------------------------------------------------*/
int Convert(char *ppmfile, char *outfile, int parts, int colormax, char *valuelabel)
{
    int     result;
    FILE   *fp = NULL;
    u8     *rgb_buffer = NULL;
    u8     *index_buffer = NULL;

    int     width;
    int     height;
    int     depth;
    int     num_pixels;

    ColorTable clut = { 0 };

    //
    // Read PPM file into rgb_buffer
    //
    fp = fopen(ppmfile, "rb");
    if (!fp)
        goto error;

    result = ReadHeader(fp, &width, &height, &depth);
    if (!result || depth != 255)
        goto error;

    num_pixels = width * height;

    rgb_buffer = (u8 *)malloc(num_pixels * 3);
    if (!rgb_buffer)
        goto error;

    result = ReadBody(fp, rgb_buffer, num_pixels * 3);
    if (!result)
        goto error;

    fclose(fp);
    fp = NULL;

    //
    // Convert to index_buffer & clut
    //
    index_buffer = (u8 *)malloc(num_pixels * 1);
    if (!index_buffer)
        goto error;

    result = Rgb2Index(num_pixels, rgb_buffer, index_buffer, colormax, &clut);
    if (!result)
        goto error;

    free(rgb_buffer);
    rgb_buffer = NULL;

    //
    // Write clut & index
    //
    fp = fopen(outfile, valuelabel ? "w" : "wb");
    if (!fp)
        goto error;

    if (parts & PARTS_CLUT)
    {
        OutputCLUT(fp, valuelabel, &clut);
    }
    if (parts & PARTS_INDEX)
    {
        OutputIndex(fp, valuelabel, colormax, width, height, index_buffer);
    }
    fclose(fp);
    free(index_buffer);
    free(clut.color);
    return TRUE;

  error:
    if (rgb_buffer)
        free(rgb_buffer);
    if (clut.color)
        free(clut.color);
    if (index_buffer)
        free(index_buffer);
    if (fp)
        fclose(fp);

    fprintf(stderr, "Cannot convert file \"%s\".\n", ppmfile);
    return FALSE;
}


/*---------------------------------------------------------------------------*
  Name:         main

  Description:  output 256 color-ed ppm image bitmap as the format
                like C source code
 *---------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    int     n;
    int     ColorMax = COLOR_MAX_8bit;
    char   *ValueLabel = NULL;
    int     Parts = PARTS_ALL;
    int     result;

    while ((n = getopt(argc, argv, "ba:c:pihvd")) != -1)
    {
        switch (n)
        {
        case 'b':
            if (ValueLabel)
                free(ValueLabel);
            ValueLabel = NULL;         // BinaryMode
            break;

        case 'a':
            if (ValueLabel)
                free(ValueLabel);
            ValueLabel = strdup(optarg);        // Source Mode
            break;

        case 'c':
            if (!strcmp(optarg, "8"))
                ColorMax = COLOR_MAX_8bit;
            else if (!strcmp(optarg, "4"))
                ColorMax = COLOR_MAX_4bit;
            else
                usage("-c : must be followed by 8 or 4", 0);    // Never returns
            break;

        case 'd':
            DebugMode = TRUE;
            break;

        case 'p':
            Parts = (Parts == PARTS_INDEX) ? PARTS_ALL : PARTS_CLUT;
            break;

        case 'i':
            Parts = (Parts == PARTS_CLUT) ? PARTS_ALL : PARTS_INDEX;
            break;

        case 'h':
        case 'v':
            if (ValueLabel)
                free(ValueLabel);
            usage(NULL, 0);            // Never returns
            break;

        default:
            if (ValueLabel)
                free(ValueLabel);
            usage("-%c : Unknown option", n);   // Never returns
            break;
        }

        if (DebugMode)
        {
            fprintf(stderr, "option -%c: %s\n", n, optarg ? optarg : "No ARG");
        }
    }

    if (DebugMode)
    {
        int     i;

        fprintf(stderr, "argc=%d  optind=%d\n", argc, optind);
        for (i = optind; i < argc; i++)
        {
            fprintf(stderr, "argv[%d] = [%s]\n", i, argv[i]);
        }
    }

    argc -= optind;
    argv += optind;

    if (argc != 2)
    {
        if (ValueLabel)
            free(ValueLabel);
        usage("need 2 arguments", 0);
        // Never returns
    }

    result = Convert(argv[0], argv[1], Parts, ColorMax, ValueLabel);

    if (ValueLabel)
        free(ValueLabel);

    return result ? 0 : 1;
}

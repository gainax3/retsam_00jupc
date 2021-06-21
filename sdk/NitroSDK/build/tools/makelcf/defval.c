/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - makerom
  File:     defval.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: defval.c,v $
  Revision 1.12  2006/03/29 13:13:22  yasu
  IF-ELSE-ENDIF のサポート

  Revision 1.11  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.10  2005/02/28 05:26:03  yosizaki
  do-indent.

  Revision 1.9  2004/08/05 13:38:44  yasu
  Support -M option

  Revision 1.8  2004/07/10 04:06:17  yasu
  Support command 'Library'
  Support modifier ':x' on template
  Fix up line continue '\'

  Revision 1.7  2004/06/29 04:10:45  yasu
  Use VBuffer to resolve variables

  Revision 1.6  2004/06/23 07:51:07  yasu
  fix a bug as illegal memory freeing at ResolveDefVal()

  Revision 1.5  2004/05/27 00:40:55  yasu
  care also about current directory (dot ".")

  Revision 1.4  2004/05/27 00:25:40  yasu
  care about double-dots ".." for defvalue option :r, :e

  Revision 1.3  2004/05/27 00:11:29  yasu
  fix a error when searching a "dot" of file extension

  Revision 1.2  2004/05/26 12:02:47  yasu
  support :h, :t, :r, :e option for variable name

  Revision 1.1  2004/03/26 05:07:33  yasu
  support variables like as -DNAME=VALUE

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include	<stdio.h>
#include	<stdlib.h>             // getenv()
#include	<ctype.h>              // isspace()
#include	<string.h>             // strcasecmp()
#include	<getopt.h>             // getopt()
#include	"misc.h"
#include	"defval.h"
#include	"makelcf.h"

typedef struct tValdef
{
    struct tValdef *next;
    char   *name;
    char   *value;
}
tValdef;

tValdef *valdef_top = NULL;

//
//  Add new define value via file
//
//      opt : "DEFINE=VALUE"
//
BOOL AddDefValFromFile(char *filename)
{
    char   *buffer;
    int     buffer_size;
    int     read_size;
    FILE   *fp;

    if (filename[0] == '-' && filename[1] == '\0')
    {
        fp = stdin;
    }
    else if (NULL == (fp = fopen(filename, "rb")))
    {
        fprintf(stderr, "Cannot open file \"%s\".\n", filename);
        return FALSE;
    }

    buffer_size = DEFVAL_DEFAULT_BUFFER_SIZE;

    if (NULL == (buffer = malloc(buffer_size)))
    {
        fprintf(stderr, "Cannot allocate memory.\n");
        return FALSE;
    }

    read_size = 0;

    while (NULL != fgets(buffer + read_size, buffer_size - read_size, fp))
    {
        read_size = strlen(buffer);

        if (read_size == buffer_size - 1 && buffer[read_size - 1] != '\n')
        {
            buffer_size *= 2;

            if (NULL == (buffer = realloc(buffer, buffer_size)))
            {
                fprintf(stderr, "Cannot allocate memory.\n");
                return FALSE;
            }
            continue;
        }

        AddDefVal(buffer);
        read_size = 0;
    }

    if (fp != stdin)
    {
        fclose(fp);
    }
    free(buffer);

    return TRUE;
}


//
//  Add new define value
//
//      opt : "DEFINE=VALUE"
//
void AddDefVal(char *opt)
{
    int     i, lastword;
    tValdef *t;

    for (i = lastword = 0;; i++)
    {
        if ('=' == opt[i] || '\0' == opt[i])
        {
            break;
        }

        if (!isspace(opt[i]))
        {
            lastword = i + 1;
        }
    }

    if (lastword > 0)
    {
        t = Alloc(sizeof(tValdef));
        t->name = strncpy(Alloc(lastword + 1), opt, lastword);
        t->name[lastword] = '\0';

        if (opt[i] == '=')
        {
            i++;
        }

        t->value = strdup(opt + i);
        lastword = strlen(t->value) - 1;
        while (lastword >= 0)
        {
            if ('\r' != t->value[lastword] && '\n' != t->value[lastword])
            {
                break;
            }
            t->value[lastword] = '\0';
            lastword--;
        }

        t->next = valdef_top;
        valdef_top = t;

        debug_printf("DEFINE:$(%s)=\"%s\"\n", t->name, t->value);
    }
    return;
}

//
//  Search define value
//
//      Return: value of specified name
//
const char *SearchDefVal(const char *name)
{
    tValdef *t;

    for (t = valdef_top; t; t = t->next)
    {
        if (!strcmp(t->name, name))
        {
            return t->value;
        }
    }
    return getenv(name);
}

//
//  SearchDefVal with NULL string handling
//
const char *SearchDefValCleaned(const char *name)
{
    const char *result = NULL;
    
    if (name != NULL)
    {
        result = SearchDefVal(name);
    }
    
    return result ? result : "";
}

//
//  Resolve modifier
//
//      Return: duplicated value of specified name modified by :x option
//
char   *ResolveStringModifier(const char *in_value, char modifier)
{
    char   *value;

    if (in_value == NULL)
    {
        value = NULL;
    }
    else if (!modifier)                // No modifier
    {
        value = strdup(in_value);
    }
    else
    {
        const int NO_EXTENSION = -1;
        int     value_len = strlen(in_value);
        int     col_extension = NO_EXTENSION;
        int     col_filename = 0;
        int     i;

        for (i = 0; i < value_len; i++)
        {
            switch (in_value[i])
            {
            case '.':
                if (col_filename == i &&
                    (in_value[i + 1] == '\0' ||
                     (in_value[i + 1] == '.' && in_value[i + 2] == '\0')))
                {
                    i = value_len;     // exit loop if last entry is '.' or '..'
                }
                else
                {
                    col_extension = i; // Save the last dot column
                }
                break;

            case '/':
            case '\\':
            case ':':
                col_filename = i + 1;  // Save the last filename
                col_extension = NO_EXTENSION;   // Reset dot position
                break;

            default:
                ;
            }
        }

        switch (modifier)
        {
        case 'h':                     // Dirname with the last slash
            value = strdup(in_value);
            value[col_filename] = '\0';
            break;

        case 't':                     // Filename
            value = strdup(in_value + col_filename);
            break;

        case 'r':                     // All without . file extension
            value = strdup(in_value);
            if (col_extension >= 0)
            {
                value[col_extension] = '\0';
            }
            break;

        case 'e':                     // File extension
            if (col_extension >= 0)
            {
                value = strdup(in_value + col_extension + 1);
            }
            else
            {
                value = strdup("");
            }
            break;

        default:                      // Unknown
            fprintf(stderr, "Unknown modifier ':%c'... Ignored.\n", modifier);
            value = strdup(in_value);
            break;
        }
    }
    return value;
}


//
//  Search define value and Modify it by ':' modifier
//
//      Return: duplicated value of specified name modified by ':X'
//
char   *SearchDefValWithOption(const char *in_name)
{
    char   *name = strdup(in_name);
    int     len_name = strlen(in_name);
    char    modifier = '\0';
    char   *value;

    if (len_name > 2 && name[len_name - 2] == ':')
    {
        name[len_name - 2] = '\0';
        modifier = name[len_name - 1];
    }

    value = ResolveStringModifier(SearchDefVal(name), modifier);

    debug_printf("REFERED(%s)=[%s]\n", in_name, value ? value : "(NULL)");

    free(name);

    return value;
}


//
//  Resolve define value
//
//      Return: new string
//
char   *ResolveDefVal(char *str)
{
    int     i, j;
    char   *val;
    VBuffer buf;

    InitVBuffer(&buf);

    for (i = 0; '\0' != str[i]; i++)
    {
        // search $(XXX)
        if ('$' == str[i] && '(' == str[i + 1])
        {
            for (j = i + 2; '\0' != str[j]; j++)
            {
                if (')' == str[j])
                {
                    str[j] = '\0';

                    // get value of XXX
                    val = SearchDefValWithOption(&str[i + 2]);

                    // copy value of XXX
                    if (val)
                    {
                        char   *s = val;

                        while (*s)
                        {
                            PutVBuffer(&buf, *s);
                            s++;
                        }
                        free(val);
                    }
                    i = j;
                    goto next;
                }
            }
        }
        PutVBuffer(&buf, str[i]);
      next:;
    }
    return GetVBuffer(&buf);           // pass allocated buffer, should be freed by caller
}

/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - init2env
  File:     init.y

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: init.y,v $
  Revision 1.2  2005/10/28 07:48:04  kitase_hirotake
  add error message

  Revision 1.1  2005/10/20 02:14:22  kitase_hirotake
  Initial Upload

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <getopt.h>

int init_yylex(void);
void yyerror(char* s);

void SetResourceName( char* name);


FILE *yyout;
FILE *yybinary;
char *class_name;

int line_count = 1;

//resource list
typedef struct ResourceList ResourceList;
struct ResourceList
{
	char*        resname;
};
ResourceList reslist[8];  // 8 = ENV_RESOURCE_SET_MAX
int rescount = 0;
%}
%union {
    int		number;
    char*	letter;
}
%token L_BRACKET R_BRACKET D_QUOTATION L_INEQUALITY R_INEQUALITY
%token <letter> LETTER TYPENUM TYPESTR TYPEBIN

%%

envs:     /* empty */
        | envs env
        ;

env:
        envcore classes envend
        ;

envend:
        {
                fprintf(yyout, "   ENV_RESOURCE_END\n");
                fprintf(yyout, "};\n\n");
        }
        ;

envcore:
        L_INEQUALITY LETTER R_INEQUALITY
        {
                fprintf(yyout, "static ENVResource %s[] = {\n", $2);

		SetResourceName($2);
        }
        ;

classes: /* empty */
        | classes class
        ;

class:
        classheader resources
        ;

classheader:
        L_BRACKET LETTER R_BRACKET
        {
		class_name = strdup($2);
        }
        ;

resources: /* empty */
        | resources resource
        ;

resource:
        LETTER TYPENUM LETTER
        {
                fprintf(yyout, "   \"%s.%s\",\t", class_name, $1);
                fprintf(yyout, "ENV_%s", $2);
                fprintf(yyout, "(%s),\n", $3);
        }
        | LETTER TYPESTR LETTER
        {
                fprintf(yyout, "   \"%s.%s\",\t", class_name, $1);
                fprintf(yyout, "ENV_%s", $2);
                fprintf(yyout, "(\"%s\"),\n", $3);
        }
        | LETTER TYPEBIN LETTER
        {
                fprintf(yyout, "   \"%s.%s\",\t", class_name, $1);
                fprintf(yyout, "ENV_%s", $2);
                fprintf(yyout, "(\"%s\"),\n", $3);
        }
        | LETTER TYPEBIN D_QUOTATION LETTER D_QUOTATION
        {
                int c;

                fprintf(yyout, "   \"%s.%s\",\t", class_name, $1);
                fprintf(yyout, "ENV_%s", $2);
                fprintf(yyout, "(\"");
                yybinary = fopen($4, "r");
                if(yybinary == NULL)
                {
                    fprintf(stderr, "file \"%s\" open error!\n", $4);
                    exit(1);
                }
                while( (c = getc(yybinary)) != EOF)
                {
                    fprintf(yyout, "%c", c);
                }
                if( fclose(yybinary) == EOF )
                {
                    fprintf(stderr, "fclose ERROR!\n");
                    exit(1);
                }
                fprintf(yyout, "\"),\n");
        }
        ;

%%
void SetResourceName(char* name)
{
    reslist[rescount].resname = (char *)strdup(name);
    rescount++;
}

int init_yywrap()
{
        return 1;
} 


void displayMessage( char* message[] )
{
    int n;
    //---- show help messages
    for( n=0; message[n]; n++ )
    {
        printf( message[n] );
    }
}

void displayUsage( void )
{
    char* usageString[] = { 
    "Usage: parser [OPTION]... RESOURCE_FILE\n",
    "\n",
    "Options:\n",
    "  -o         : Output C filename.\n",
    0
    };

    displayMessage( usageString );
}


int main(int argc, char *argv[])
{
    extern int yyparse(void);
    extern FILE *init_yyin;

    int c,n;

    char *input_filename = NULL;
    char *output_filename = NULL;

    while(1)
    {
        c = getopt(argc, argv, "s:o:n:");

        if(c == -1)
        {
            break;
        }
        
        switch(c)
        {
            case 'o':
                printf("output = %s\n", optarg);
                output_filename = strdup(optarg);
                break;
            default:
                displayUsage();
                exit(1);
        }
    }
    //---- input filename
    if(argv[optind] == NULL)
    {
        displayUsage();
        exit(1);
    }
    input_filename = strdup(argv[optind]);
    optind++;

    if(output_filename == NULL)
    {
        //output_filename = strdup(input_filename);
        //strcat(output_filename, ".c");
	output_filename = strdup("./result.c");
        printf("output = %s\n", output_filename);
    }

    yyout = fopen(output_filename, "w");
    if(yyout == NULL)
    {
        fprintf(stderr, "fopen ERROR!\n");
        exit(1);
    }

    fprintf(yyout, "#include <nitro.h>\n");
    
    while(1)
    {
        //yyin = stdin;
        init_yyin = fopen(input_filename, "r");

        if (yyparse())
        {
	    fprintf(stderr, "\n");
 	    exit(1);
        }

	fprintf(yyout, "ENVResource* resourceArray[]=\n{\n  ");
	for(n = 0; n < rescount; n++)
	{
        	fprintf(yyout, " %s,",reslist[n].resname);
        }
	fprintf(yyout, " NULL\n");
	fprintf(yyout, "};\n");

        //---- input filename
        if(argv[optind] == NULL)
        {
            break;
        }
        input_filename = strdup(argv[optind]);
        optind++;
    }

    if( fclose(yyout) == EOF )
    {
        fprintf(stderr, "fclose ERROR!\n");
        exit(1);
    }

    printf("finish!!\n");

    return 1;
}

void yyerror(char* s)
{
	printf("%s line %d\n", s, line_count);
	printf("near %s\n", init_yylval.letter);
}
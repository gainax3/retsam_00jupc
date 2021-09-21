%{
/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - makelcf
  File:     spec.l

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: spec.l,v $
  Revision 1.28  2006/05/10 02:06:00  yasu
  CodeWarrior 2.x への対応

  Revision 1.27  2006/04/09 23:46:03  kitase_hirotake
  Copyright 修正

  Revision 1.26  2006/04/06 09:22:38  kitase_hirotake
  不必要な部分の削除

  Revision 1.25  2006/04/06 09:05:31  kitase_hirotake
  .itcm.bss .dtcm.bss .wram.bss に対応

  Revision 1.24  2005/09/01 04:30:52  yasu
  Overlay Group のサポート

  Revision 1.23  2005/07/26 08:49:27  yasu
  過去のバージョンとの互換ためにキーワード surffix もサポートする

  Revision 1.22  2005/06/22 00:50:06  yasu
  著作年度修正

  Revision 1.21  2005/06/20 12:21:48  yasu
  Surffix -> Suffix 修正

  Revision 1.20  2004/07/10 04:06:17  yasu
  Support command 'Library'
  Support modifier ':x' on template
  Fix up line continue '\'

  Revision 1.19  2004/07/08 02:58:53  yasu
  Support section name for multi-objects as 'Objects' parameters

  Revision 1.18  2004/07/02 07:34:53  yasu
  Support OBJECT( )

  Revision 1.17  2004/07/01 09:39:58  yasu
  handle ITCM/DTCM/WRAM as keyword

  Revision 1.16  2004/06/29 04:20:31  yasu
  Change error message

  Revision 1.15  2004/06/24 07:18:54  yasu
  Support keyword "Autoload"

  Revision 1.14  2004/06/14 11:28:45  yasu
  support section filter "FOREACH.STATIC.OBJECTS=.sectionName"

  Revision 1.13  2004/06/07 01:46:59  yasu
  Support 'properties' as keyword.

  Revision 1.12  2004/05/27 09:59:25  yasu
  support negative value for stack size

  Revision 1.11  2004/04/02 04:02:36  yasu
  merged with 1.00 branch

  Revision 1.10  2004/04/02 03:03:17  yasu
  support big/little letter keywords

  Revision 1.9  2004/03/26 05:07:11  yasu
  support variables like as -DNAME=VALUE

  Revision 1.8  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.7  2004/01/15 10:47:56  yasu
  Static StackSize の実装

  Revision 1.6  2004/01/14 13:05:44  yasu
  Handle '\r?\n' as newline

  Revision 1.5  2004/01/14 12:58:47  yasu
  Handle '\r' as newline

  Revision 1.4  2004/01/14 12:40:34  yasu
  Change newline handling

  Revision 1.3  2004/01/14 12:38:08  yasu
  Change OverlayName->OverlayDefs

  Revision 1.2  2004/01/07 13:10:17  yasu
  fix all warning at compile -Wall

  Revision 1.1  2004/01/05 02:32:59  yasu
  Initial version

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include	<stdio.h>
#include	<string.h>
#include	"makelcf.h"
#include	"defval.h"
#include	"spec.tab.h"

#define	RETURN(x) { debug_printf( "%s [%s]\n", #x, spec_yytext ); return(x); }

static void  YYText_ResolveDevVal( void );
static char* getSection( void );


%}

ws			[ \t]
nl			\r?\n
identifier		[A-Za-z_][A-Za-z_0-9]*
decnum			-?[1-9][0-9]*
octnum			-?0[0-7]*
hexnum			-?0[xX][0-9A-Fa-f]+
qstring			\"[^\"\n\r]*\"
string_wo_ws		[^ \t\n\r]+

%s START
%s PARAM
%s PARAM_CONTINUE
%option pointer
%option yylineno
%option noyywrap
%%

 //======================================================================
 //    SPEC TOKENS
 //======================================================================

#.*			;					/* Comment */
\/\/.*			;					/* Comment */

<INITIAL>.*	{
	YYText_ResolveDevVal();
	BEGIN START;
}

<PARAM_CONTINUE>.*	{
	YYText_ResolveDevVal();
	BEGIN PARAM;
}

<START>(Static|STATIC|static)	 		{ BEGIN PARAM; RETURN(tSTATIC);       }
<START>(Autoload|AUTOLOAD|autoload) 		{ BEGIN PARAM; RETURN(tAUTOLOAD);     }
<START>(Overlay|OVERLAY|overlay) 		{ BEGIN PARAM; RETURN(tOVERLAY);      }
<START>(Property|PROPERTY|property)		{ BEGIN PARAM; RETURN(tPROPERTY);     }
<START>(Properties|PROPERTIES|properties)	{ BEGIN PARAM; RETURN(tPROPERTY);     }

<START>(Group|GROUP|group)			{ BEGIN PARAM; RETURN(tGROUP);        }
<START>(Address|ADDRESS|address)		{ BEGIN PARAM; RETURN(tADDRESS);      }
<START>(After|AFTER|after)			{ BEGIN PARAM; RETURN(tAFTER);        }
<START>(Object|OBJECT|object)[sS]?		{ BEGIN PARAM; RETURN(tOBJECT);       }
<START>(Librar|LIBRAR|librar)(y|Y|ies|IES)?	{ BEGIN PARAM; RETURN(tLIBRARY);      }
<START>(Search|SEARCH|search)_?(Symbol|SYMBOL|symbol)[sS]? { BEGIN PARAM; RETURN(tSEARCHSYMBOL); }
<START>(StackSize|STACKSIZE|stacksize)		{ BEGIN PARAM; RETURN(tSTACKSIZE);    }

<START>(OverlayDefs|OVERLAYDEFS|overlaydefs)	{ BEGIN PARAM; RETURN(tOVERLAYDEFS);  }
<START>(OverlayTable|OVERLAYTABLE|overlaytable)	{ BEGIN PARAM; RETURN(tOVERLAYTABLE); }
<START>(Sur?ffix|SUR?FFIX|sur?ffix)		{ BEGIN PARAM; RETURN(tSUFFIX);       }

<PARAM>(ITCM|itcm|Itcm)		{
	spec_yylval.string = strdup("ITCM");
	RETURN(tSTRING_ID);
}

<PARAM>(DTCM|dtcm|Dtcm)		{
	spec_yylval.string = strdup("DTCM");
	RETURN(tSTRING_ID);
}

<PARAM>(WRAM|wram|Wram)		{
	spec_yylval.string = strdup("WRAM");
	RETURN(tSTRING_ID);
}

<START>\{		{ BEGIN PARAM; RETURN(tBEGIN);        }
<START>\}		{ BEGIN PARAM; RETURN(tEND);          }

<PARAM>{ws}*\\{ws}*{nl}	{ BEGIN PARAM_CONTINUE; }     	/* Continue to next line */

<PARAM>\({ws}*\.[a-z]*{ws}*\)	{
	spec_yylval.string = getSection();
	RETURN(tSECTIONNAME);
}

<PARAM>\({ws}*\.[a-z]*\.[a-z]*{ws}*\)	{
	spec_yylval.string = getSection();
	RETURN(tSECTIONNAME);
}

<PARAM>\*		{
	spec_yylval.string = strdup(spec_yytext);
	RETURN(tSTRING_STAR);
}

<PARAM>GROUP\([^\)]*\)	{
	spec_yylval.string = strdup(spec_yytext);
	RETURN(tSTRING_GROUP);
}

<PARAM>OBJECT\([^\)]*\)	{
	spec_yylval.string = strdup(spec_yytext);
	RETURN(tSTRING_FUNCTION);
}

{decnum}		|
{octnum}		|
{hexnum}		{			/* Integer Value */
	spec_yylval.integer = strtol((char*)spec_yytext, (char**)NULL, 0);
	RETURN(tNUMBER);
}

{identifier}		{		       	/* String for identifier */
	spec_yylval.string = strdup(spec_yytext);
	RETURN(tSTRING_ID);
}

{qstring}		{		       	/* String quated by "" */
	spec_yytext[spec_yyleng-1] = '\0';
	spec_yylval.string = strdup(spec_yytext+1);
	RETURN(tSTRING_QUATED);
}

{string_wo_ws}		{			/* String without white space */
	spec_yylval.string = strdup(spec_yytext);
	RETURN(tSTRING_NOSPACE);
}

<PARAM><<EOF>>		{ BEGIN INITIAL; RETURN(tNL); }	/* End of File */

{nl}			{ BEGIN INITIAL; RETURN(tNL); }	/* Newline     */
{ws}+			;	       			/* White space */
.			{ RETURN(spec_yytext[0]);     }	/* Default     */

%%

/*============================================================================
 *  Utilities
 */
void  spec_yyerror( const char* str )
{
    int  line = spec_yylineno;
    if ( spec_yytext[0] == '\n' ) line --;
    fprintf( stderr, "makelcf: line %d: %s\n", line, str );
}

static char* getSection( void )
{
    char*  p;
    char*  head = NULL;

    for ( p = spec_yytext; *p; p ++ )
    {
	switch ( *p )
	{
	case '(':
	    while ( *p == ' ' ) p ++;
	    head = p + 1;
	    break;
	    
	case ')':
	case ' ':
	    if ( head )
            {
	        *p = '\0';
		return strdup( head );
            }
	    break;
	    
	default:
	    break;
	}
    }
    return strdup( "*" );
}

static void  YYText_ResolveDevVal( void )
{
    int   i;
    char* s = ResolveDefVal( spec_yytext );
    
    for ( i = strlen(s)-1; i >= 0; i -- )
    {
    	unput( s[i] );
    }
    free(s);
    return;
}

/*============================================================================
 *  PARSE SPEC FILE
 */
int  ParseSpecFile( const char* filename )
{
    FILE *fp;
    int   result;
    
    if ( NULL == ( fp = fopen( filename, "r" ) ) )
    {
	fprintf( stderr, "makelcf: cannot open %s\n", filename );
	return 2;
    }
    
    spec_yyin = fp;
    result = spec_yyparse();
    fclose( fp );
    if ( result ) return 1;

    return CheckSpec() ? 0 : 1;
}

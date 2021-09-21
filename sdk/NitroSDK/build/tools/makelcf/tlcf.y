%{
/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - makelcf
  File:     tlcf.y

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: tlcf.y,v $
  Revision 1.23  2007/07/09 12:17:54  yasu
  TARGET_NAME のサポート

  Revision 1.22  2007/07/09 07:02:00  yasu
  IF.EXIST のサポート

  Revision 1.21  2007/04/12 03:29:45  yasu
  IF FIRST/LAST を FOREACH OBJECT/LIBRARY でも対応できるようにした

  Revision 1.20  2007/04/10 14:13:20  yasu
  複数の SEARCH_SYMBOL 対応

  Revision 1.19  2006/05/11 12:55:30  yasu
  warning の停止

  Revision 1.18  2006/05/10 03:14:01  yasu
  IF-ELSE-ENDIF の条件判定の誤りの修正

  Revision 1.17  2006/05/10 02:06:00  yasu
  CodeWarrior 2.x への対応

  Revision 1.16  2006/03/30 23:59:45  yasu
  著作年度の変更

  Revision 1.15  2006/03/29 13:13:22  yasu
  IF-ELSE-ENDIF のサポート

  Revision 1.14  2005/09/01 04:30:58  yasu
  Overlay Group のサポート

  Revision 1.13  2005/08/26 11:23:11  yasu
  ITCM/DTCM への overlay の対応

  Revision 1.12  2005/06/22 00:50:06  yasu
  著作年度修正

  Revision 1.11  2005/06/20 12:21:48  yasu
  Surffix -> Suffix 修正

  Revision 1.10  2004/07/10 04:06:17  yasu
  Support command 'Library'
  Support modifier ':x' on template
  Fix up line continue '\'

  Revision 1.9  2004/07/02 07:34:53  yasu
  Support OBJECT( )

  Revision 1.8  2004/06/24 07:18:54  yasu
  Support keyword "Autoload"

  Revision 1.7  2004/06/14 11:28:45  yasu
  support section filter "FOREACH.STATIC.OBJECTS=.sectionName"

  Revision 1.6  2004/02/13 07:13:03  yasu
  support SDK_IRQ_STACKSIZE

  Revision 1.5  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.4  2004/01/15 10:47:56  yasu
  Static StackSize の実装

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

// IF-ELSE-ENDIF
static  tCondStack	cond_stack[COND_STACK_MAX];
static  int		cond_stack_level = 0;
static  BOOL		cond_valid = TRUE;
static	tCondBlock	cond_block = COND_BLOCK_NOCOND;

static BOOL GetCompResult(const char* id, const char* comp, const char* value, BOOL* pvalid)
{
	int   result;
	BOOL  valid;
	const char* defval;
	
	defval = SearchDefValCleaned(id);
	result = strcmp(defval, value);
	if      ( strcmp( comp, ".EQ." ) == 0 || strcmp( comp, "==" ) == 0 ) valid = ( result == 0 );
	else if ( strcmp( comp, ".NE." ) == 0 || strcmp( comp, "!=" ) == 0 ) valid = ( result != 0 );
	else if ( strcmp( comp, ".GT." ) == 0 ) valid = ( result >  0 );
	else if ( strcmp( comp, ".GE." ) == 0 ) valid = ( result >= 0 );
	else if ( strcmp( comp, ".LT." ) == 0 ) valid = ( result <  0 );
	else if ( strcmp( comp, ".LE." ) == 0 ) valid = ( result <= 0 );
	else
	{
		tlcf_yyerror( "Illegal IF-condition" );
		return FALSE;
	}
	debug_printf("id(%s)=[%s] comp=[%s] value=[%s] valid=%d\n", id, defval, comp, value, valid);

	*pvalid = valid;
	return TRUE;
}

static BOOL CondIf(const char* id, const char* comp, const char* value)
{
	if (cond_stack_level >= COND_STACK_MAX)
	{
		tlcf_yyerror( "Too deep if-else-endif block" );
		return FALSE;
	}
	cond_stack[cond_stack_level].block = cond_block;
	cond_stack[cond_stack_level].valid = cond_valid;
	cond_stack_level ++;
	cond_block = COND_BLOCK_IF;
	
	// Update validation only if previous level validation is TRUE
	if (cond_stack[cond_stack_level-1].valid)
	{
		if (!GetCompResult(id, comp, value, &cond_valid))
		{
			tlcf_yyerror( "Unresolved IF condition" );
			return FALSE;
		}
	}
	return TRUE;
}

static BOOL CondElse(void)
{
	if (cond_block != COND_BLOCK_IF || cond_stack_level <= 0)
	{
		tlcf_yyerror( "IF-ELSE-ENDIF unmatched" );
		return FALSE;
	}
	cond_block = COND_BLOCK_ELSE;
	
	// Update validation only if previous level validation is TRUE
	if (cond_stack[cond_stack_level-1].valid)
	{
		cond_valid = cond_valid ? FALSE : TRUE;		// Flip validation
	}
	return TRUE;
}

static BOOL CondEndIf(void)
{
	if ((cond_block != COND_BLOCK_IF && cond_block != COND_BLOCK_ELSE) || cond_stack_level <= 0)
	{
		tlcf_yyerror( "IF-ELSE-ENDIF unmatched" );
		return FALSE;
	}
	cond_stack_level --;
	cond_block = cond_stack[cond_stack_level].block;
	cond_valid = cond_stack[cond_stack_level].valid;
	return TRUE;
}


// TOKEN HANDLEING
tTokenBuffer		tokenBuffer[TOKEN_LEN];
int			tokenBufferEnd = 0;
static  tLoopStack	loop_stack[LOOP_STACK_MAX];
static	int		loop_stack_level = 0;

static BOOL  StoreToken( int id, const char *str )
{
	// Validation check for IF-ELSE-ENDIF
	if ( !cond_valid ) return TRUE;

	if ( tokenBufferEnd >= TOKEN_LEN )
	{
		tlcf_yyerror( "Out of token buffer" );
		return FALSE;
	}
	tokenBuffer[tokenBufferEnd].id     = id;
	tokenBuffer[tokenBufferEnd].string = str;
	tokenBufferEnd ++;
	return TRUE;
}

static BOOL  PushLoopStack( int id, const char* sectionName )
{
	// Validation check for IF-ELSE-ENDIF
	if ( !cond_valid ) return TRUE;
	
	if ( loop_stack_level >= LOOP_STACK_MAX )
	{
		tlcf_yyerror( "Out of stack level" );
		return FALSE;
	}
	loop_stack[loop_stack_level].id    = id;
	loop_stack[loop_stack_level].start = tokenBufferEnd;
	loop_stack_level ++;
	return StoreToken( id, sectionName );
}

static BOOL  PopLoopStack( int id )
{
	int start;
	
	// Validation check for IF-ELSE-ENDIF
	if ( !cond_valid ) return TRUE;
	
	loop_stack_level --;
	if ( loop_stack_level < 0 || id != loop_stack[loop_stack_level].id )
	{
		tlcf_yyerror( "Unmatched foreach-end" );
		return FALSE;
	}
	start = loop_stack[loop_stack_level].start;
	tokenBuffer[start].loop_end = tokenBufferEnd-1;
	
	// Don't store 'END' token to TokenBuffer
	return TRUE;
}

BOOL  GetLoopStackLevel( void )
{
	return loop_stack_level;
}

%}

%union
{
	char	*string;
};

%token	<string>	tSTRING

%token	<string>	tTARGET_NAME
%token			tSTATIC_NAME
%token			tSTATIC_ADDRESS
%token			tSTATIC_STACKSIZE
%token			tSTATIC_IRQSTACKSIZE
%token	<string>	tSTATIC_OBJECT
%token	<string>	tSTATIC_LIBRARY
%token	<string>	tSTATIC_SEARCHSYMBOL

%token	<string>	tFOREACH_STATIC_OBJECTS
%token	<string>	tFOREACH_STATIC_LIBRARIES
%token	<string>	tFOREACH_STATIC_SEARCHSYMBOLS
%token			tEND_STATIC_OBJECTS
%token			tEND_STATIC_LIBRARIES
%token			tEND_STATIC_SEARCHSYMBOLS

%token	<string>	tIF_STATIC_OBJECT_FIRST
%token	<string>	tIF_STATIC_OBJECT_LAST
%token	<string>	tIF_STATIC_LIBRARY_FIRST
%token	<string>	tIF_STATIC_LIBRARY_LAST
%token	<string>	tIF_STATIC_SEARCHSYMBOL_FIRST
%token	<string>	tIF_STATIC_SEARCHSYMBOL_LAST

%token			tNUMBER_AUTOLOADS
%token			tAUTOLOAD_ID
%token			tAUTOLOAD_NAME
%token			tAUTOLOAD_ADDRESS
%token	<string>	tAUTOLOAD_OBJECT
%token	<string>	tAUTOLOAD_LIBRARY
%token	<string>	tAUTOLOAD_SEARCHSYMBOL

%token	<string>	tFOREACH_AUTOLOADS
%token	<string>	tFOREACH_AUTOLOAD_OBJECTS
%token	<string>	tFOREACH_AUTOLOAD_LIBRARIES
%token	<string>	tFOREACH_AUTOLOAD_SEARCHSYMBOLS
%token			tEND_AUTOLOADS
%token			tEND_AUTOLOAD_OBJECTS
%token			tEND_AUTOLOAD_LIBRARIES
%token			tEND_AUTOLOAD_SEARCHSYMBOLS

%token	<string>	tIF_AUTOLOAD_FIRST
%token	<string>	tIF_AUTOLOAD_LAST
%token	<string>	tIF_AUTOLOAD_OBJECT_FIRST
%token	<string>	tIF_AUTOLOAD_OBJECT_LAST
%token	<string>	tIF_AUTOLOAD_LIBRARY_FIRST
%token	<string>	tIF_AUTOLOAD_LIBRARY_LAST
%token	<string>	tIF_AUTOLOAD_SEARCHSYMBOL_FIRST
%token	<string>	tIF_AUTOLOAD_SEARCHSYMBOL_LAST

%token			tNUMBER_OVERLAYS
%token			tOVERLAY_ID
%token			tOVERLAY_NAME
%token			tOVERLAY_GROUP
%token			tOVERLAY_ADDRESS
%token	<string>	tOVERLAY_OBJECT
%token	<string>	tOVERLAY_LIBRARY
%token	<string>	tOVERLAY_SEARCHSYMBOL

%token	<string>	tFOREACH_OVERLAYS
%token	<string>	tFOREACH_OVERLAY_OBJECTS
%token	<string>	tFOREACH_OVERLAY_LIBRARIES
%token	<string>	tFOREACH_OVERLAY_SEARCHSYMBOLS
%token			tEND_OVERLAYS
%token			tEND_OVERLAY_OBJECTS
%token			tEND_OVERLAY_LIBRARIES
%token			tEND_OVERLAY_SEARCHSYMBOLS

%token	<string>	tIF_OVERLAY_FIRST
%token	<string>	tIF_OVERLAY_LAST
%token	<string>	tIF_OVERLAY_OBJECT_FIRST
%token	<string>	tIF_OVERLAY_OBJECT_LAST
%token	<string>	tIF_OVERLAY_LIBRARY_FIRST
%token	<string>	tIF_OVERLAY_LIBRARY_LAST
%token	<string>	tIF_OVERLAY_SEARCHSYMBOL_FIRST
%token	<string>	tIF_OVERLAY_SEARCHSYMBOL_LAST

%token	<string>	tIF_EXIST_SECTION
%token	<string>	tIF_EXIST_STATIC
%token	<string>	tIF_EXIST_AUTOLOAD
%token	<string>	tIF_EXIST_OVERLAY

%token			tPROPERTY_OVERLAYDEFS
%token			tPROPERTY_OVERLAYTABLE
%token			tPROPERTY_SUFFIX

%token			tIF_OPEN
%token			tIF_CLOSE
%token	<string>	tIF_ID
%token	<string>	tIF_COMP
%token	<string>	tIF_VALUE
%token			tIF_ELSE
%token			tIF_ENDIF
%%

/*=========================================================================*
      ConfigFile
 *=========================================================================*/

tlcfFile	: tokens
		;

tokens		: /*NULL*/
		| tokens token
		;

token		: tSTRING			{ if (!StoreToken( tSTRING,                $1   )) YYABORT; }
		| tTARGET_NAME			{ if (!StoreToken( tTARGET_NAME,           $1   )) YYABORT; }
		| tSTATIC_NAME			{ if (!StoreToken( tSTATIC_NAME,           NULL )) YYABORT; }
		| tSTATIC_ADDRESS		{ if (!StoreToken( tSTATIC_ADDRESS,        NULL )) YYABORT; }
		| tSTATIC_OBJECT		{ if (!StoreToken( tSTATIC_OBJECT,         $1   )) YYABORT; }
		| tSTATIC_LIBRARY		{ if (!StoreToken( tSTATIC_LIBRARY,        $1   )) YYABORT; }
		| tSTATIC_SEARCHSYMBOL		{ if (!StoreToken( tSTATIC_SEARCHSYMBOL,   $1   )) YYABORT; }
		| tSTATIC_STACKSIZE		{ if (!StoreToken( tSTATIC_STACKSIZE,      NULL )) YYABORT; }
		| tSTATIC_IRQSTACKSIZE		{ if (!StoreToken( tSTATIC_IRQSTACKSIZE,   NULL )) YYABORT; }
		| tFOREACH_STATIC_OBJECTS	{ if (!PushLoopStack( tFOREACH_STATIC_OBJECTS, $1   )) YYABORT; }
		| tEND_STATIC_OBJECTS    	{ if (!PopLoopStack ( tFOREACH_STATIC_OBJECTS       )) YYABORT; }
		| tFOREACH_STATIC_LIBRARIES	{ if (!PushLoopStack( tFOREACH_STATIC_LIBRARIES, $1 )) YYABORT; }
		| tEND_STATIC_LIBRARIES    	{ if (!PopLoopStack ( tFOREACH_STATIC_LIBRARIES     )) YYABORT; }
		| tFOREACH_STATIC_SEARCHSYMBOLS	{ if (!PushLoopStack( tFOREACH_STATIC_SEARCHSYMBOLS, $1 )) YYABORT; }
		| tEND_STATIC_SEARCHSYMBOLS	{ if (!PopLoopStack ( tFOREACH_STATIC_SEARCHSYMBOLS     )) YYABORT; }
		| tIF_STATIC_OBJECT_FIRST       { if (!StoreToken( tIF_STATIC_OBJECT_FIRST,       $1 )) YYABORT; }
		| tIF_STATIC_OBJECT_LAST        { if (!StoreToken( tIF_STATIC_OBJECT_LAST,        $1 )) YYABORT; }
		| tIF_STATIC_LIBRARY_FIRST      { if (!StoreToken( tIF_STATIC_LIBRARY_FIRST,      $1 )) YYABORT; }
		| tIF_STATIC_LIBRARY_LAST       { if (!StoreToken( tIF_STATIC_LIBRARY_LAST,       $1 )) YYABORT; }
		| tIF_STATIC_SEARCHSYMBOL_FIRST { if (!StoreToken( tIF_STATIC_SEARCHSYMBOL_FIRST, $1 )) YYABORT; }
		| tIF_STATIC_SEARCHSYMBOL_LAST  { if (!StoreToken( tIF_STATIC_SEARCHSYMBOL_LAST,  $1 )) YYABORT; }
		| tAUTOLOAD_ID			{ if (!StoreToken( tAUTOLOAD_ID,           NULL )) YYABORT; }
		| tAUTOLOAD_NAME		{ if (!StoreToken( tAUTOLOAD_NAME,         NULL )) YYABORT; }
		| tAUTOLOAD_ADDRESS		{ if (!StoreToken( tAUTOLOAD_ADDRESS,      NULL )) YYABORT; }
		| tAUTOLOAD_OBJECT		{ if (!StoreToken( tAUTOLOAD_OBJECT,       $1   )) YYABORT; }
		| tAUTOLOAD_LIBRARY		{ if (!StoreToken( tAUTOLOAD_LIBRARY,      $1   )) YYABORT; }
		| tAUTOLOAD_SEARCHSYMBOL	{ if (!StoreToken( tAUTOLOAD_SEARCHSYMBOL, $1   )) YYABORT; }
		| tNUMBER_AUTOLOADS		{ if (!StoreToken( tNUMBER_AUTOLOADS,      NULL )) YYABORT; }
		| tFOREACH_AUTOLOADS		{ if (!PushLoopStack( tFOREACH_AUTOLOADS,          $1 )) YYABORT; }
		| tEND_AUTOLOADS		{ if (!PopLoopStack ( tFOREACH_AUTOLOADS              )) YYABORT; }
		| tFOREACH_AUTOLOAD_OBJECTS	{ if (!PushLoopStack( tFOREACH_AUTOLOAD_OBJECTS,   $1 )) YYABORT; }
		| tEND_AUTOLOAD_OBJECTS		{ if (!PopLoopStack ( tFOREACH_AUTOLOAD_OBJECTS       )) YYABORT; }
		| tFOREACH_AUTOLOAD_LIBRARIES	{ if (!PushLoopStack( tFOREACH_AUTOLOAD_LIBRARIES, $1 )) YYABORT; }
		| tEND_AUTOLOAD_LIBRARIES	{ if (!PopLoopStack ( tFOREACH_AUTOLOAD_LIBRARIES     )) YYABORT; }
		| tFOREACH_AUTOLOAD_SEARCHSYMBOLS{ if (!PushLoopStack( tFOREACH_AUTOLOAD_SEARCHSYMBOLS, $1 )) YYABORT; }
		| tEND_AUTOLOAD_SEARCHSYMBOLS	 { if (!PopLoopStack ( tFOREACH_AUTOLOAD_SEARCHSYMBOLS     )) YYABORT; }		| tIF_AUTOLOAD_FIRST             { if (!StoreToken( tIF_AUTOLOAD_FIRST,              $1 )) YYABORT; }
		| tIF_AUTOLOAD_LAST              { if (!StoreToken( tIF_AUTOLOAD_LAST,               $1 )) YYABORT; }
		| tIF_AUTOLOAD_OBJECT_FIRST      { if (!StoreToken( tIF_AUTOLOAD_OBJECT_FIRST,       $1 )) YYABORT; }
		| tIF_AUTOLOAD_OBJECT_LAST       { if (!StoreToken( tIF_AUTOLOAD_OBJECT_LAST,        $1 )) YYABORT; }
		| tIF_AUTOLOAD_LIBRARY_FIRST     { if (!StoreToken( tIF_AUTOLOAD_LIBRARY_FIRST,      $1 )) YYABORT; }
		| tIF_AUTOLOAD_LIBRARY_LAST      { if (!StoreToken( tIF_AUTOLOAD_LIBRARY_LAST,       $1 )) YYABORT; }
		| tIF_AUTOLOAD_SEARCHSYMBOL_FIRST{ if (!StoreToken( tIF_AUTOLOAD_SEARCHSYMBOL_FIRST, $1 )) YYABORT; }
		| tIF_AUTOLOAD_SEARCHSYMBOL_LAST { if (!StoreToken( tIF_AUTOLOAD_SEARCHSYMBOL_LAST,  $1 )) YYABORT; }
		| tOVERLAY_ID			{ if (!StoreToken( tOVERLAY_ID,            NULL )) YYABORT; }
		| tOVERLAY_NAME			{ if (!StoreToken( tOVERLAY_NAME,          NULL )) YYABORT; }
		| tOVERLAY_GROUP		{ if (!StoreToken( tOVERLAY_GROUP,         NULL )) YYABORT; }
		| tOVERLAY_ADDRESS		{ if (!StoreToken( tOVERLAY_ADDRESS,       NULL )) YYABORT; }
		| tOVERLAY_OBJECT		{ if (!StoreToken( tOVERLAY_OBJECT,        $1   )) YYABORT; }
		| tOVERLAY_LIBRARY		{ if (!StoreToken( tOVERLAY_LIBRARY,       $1   )) YYABORT; }
		| tOVERLAY_SEARCHSYMBOL		{ if (!StoreToken( tOVERLAY_SEARCHSYMBOL,  $1   )) YYABORT; }
		| tNUMBER_OVERLAYS		{ if (!StoreToken( tNUMBER_OVERLAYS,       NULL )) YYABORT; }
		| tFOREACH_OVERLAYS		{ if (!PushLoopStack( tFOREACH_OVERLAYS,          $1 )) YYABORT; }
		| tEND_OVERLAYS			{ if (!PopLoopStack ( tFOREACH_OVERLAYS              )) YYABORT; }
		| tFOREACH_OVERLAY_OBJECTS	{ if (!PushLoopStack( tFOREACH_OVERLAY_OBJECTS,   $1 )) YYABORT; }
		| tEND_OVERLAY_OBJECTS		{ if (!PopLoopStack ( tFOREACH_OVERLAY_OBJECTS       )) YYABORT; }
		| tFOREACH_OVERLAY_LIBRARIES	{ if (!PushLoopStack( tFOREACH_OVERLAY_LIBRARIES, $1 )) YYABORT; }
		| tEND_OVERLAY_LIBRARIES	{ if (!PopLoopStack ( tFOREACH_OVERLAY_LIBRARIES     )) YYABORT; }
		| tFOREACH_OVERLAY_SEARCHSYMBOLS{ if (!PushLoopStack( tFOREACH_OVERLAY_SEARCHSYMBOLS, $1 )) YYABORT; }
		| tEND_OVERLAY_SEARCHSYMBOLS	{ if (!PopLoopStack ( tFOREACH_OVERLAY_SEARCHSYMBOLS     )) YYABORT; }
		| tIF_OVERLAY_FIRST             { if (!StoreToken( tIF_OVERLAY_FIRST,              $1 )) YYABORT; }
		| tIF_OVERLAY_LAST              { if (!StoreToken( tIF_OVERLAY_LAST,               $1 )) YYABORT; }
		| tIF_OVERLAY_OBJECT_FIRST      { if (!StoreToken( tIF_OVERLAY_OBJECT_FIRST,       $1 )) YYABORT; }
		| tIF_OVERLAY_OBJECT_LAST       { if (!StoreToken( tIF_OVERLAY_OBJECT_LAST,        $1 )) YYABORT; }
		| tIF_OVERLAY_LIBRARY_FIRST     { if (!StoreToken( tIF_OVERLAY_LIBRARY_FIRST,      $1 )) YYABORT; }
		| tIF_OVERLAY_LIBRARY_LAST      { if (!StoreToken( tIF_OVERLAY_LIBRARY_LAST,       $1 )) YYABORT; }
		| tIF_OVERLAY_SEARCHSYMBOL_FIRST{ if (!StoreToken( tIF_OVERLAY_SEARCHSYMBOL_FIRST, $1 )) YYABORT; }
		| tIF_OVERLAY_SEARCHSYMBOL_LAST { if (!StoreToken( tIF_OVERLAY_SEARCHSYMBOL_LAST,  $1 )) YYABORT; }
		| tPROPERTY_OVERLAYDEFS		{ if (!StoreToken( tPROPERTY_OVERLAYDEFS,  NULL )) YYABORT; }
		| tPROPERTY_OVERLAYTABLE	{ if (!StoreToken( tPROPERTY_OVERLAYTABLE, NULL )) YYABORT; }
		| tPROPERTY_SUFFIX		{ if (!StoreToken( tPROPERTY_SUFFIX,       NULL )) YYABORT; }
		| tIF_EXIST_SECTION             { if (!StoreToken( tIF_EXIST_SECTION,  $1 )) YYABORT; }
		| tIF_EXIST_STATIC              { if (!StoreToken( tIF_EXIST_STATIC,   $1 )) YYABORT; }
		| tIF_EXIST_AUTOLOAD            { if (!StoreToken( tIF_EXIST_AUTOLOAD, $1 )) YYABORT; }
		| tIF_EXIST_OVERLAY             { if (!StoreToken( tIF_EXIST_OVERLAY,  $1 )) YYABORT; }
		| cond_if | cond_else | cond_endif
		;

cond_if		: tIF_OPEN tIF_ID tIF_COMP tIF_VALUE tIF_CLOSE { if (!CondIf( $2, $3, $4 )) YYABORT; }
		| tIF_OPEN tIF_ID tIF_COMP           tIF_CLOSE { if (!CondIf( $2, $3, "" )) YYABORT; }
		;

cond_else	: tIF_ELSE  { if (!CondElse()) YYABORT; }
		;

cond_endif	: tIF_ENDIF { if (!CondEndIf()) YYABORT; }
		;

%%

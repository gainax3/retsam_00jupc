%{
/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - makelcf
  File:     spec.y

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: spec.y,v $
  Revision 1.15  2006/05/10 02:06:00  yasu
  CodeWarrior 2.x への対応

  Revision 1.14  2005/09/01 04:30:52  yasu
  Overlay Group のサポート

  Revision 1.13  2005/06/22 00:50:06  yasu
  著作年度修正

  Revision 1.12  2005/06/20 12:21:48  yasu
  Surffix -> Suffix 修正

  Revision 1.11  2004/07/10 04:06:17  yasu
  Support command 'Library'
  Support modifier ':x' on template
  Fix up line continue '\'

  Revision 1.10  2004/07/08 02:58:53  yasu
  Support section name for multi-objects as 'Objects' parameters

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
#include	<stdlib.h>
#include	"makelcf.h"

%}

%token	tSTATIC
%token  tAUTOLOAD
%token  tOVERLAY
%token  tPROPERTY
%token	tGROUP
%token	tADDRESS
%token  tAFTER
%token  tOBJECT
%token  tLIBRARY
%token  tSEARCHSYMBOL
%token  tSTACKSIZE
%token	tOVERLAYDEFS
%token  tOVERLAYTABLE
%token  tSUFFIX
%token	tBEGIN
%token  tEND
%token  tNL

%union
{
	u32	  	integer;
	char*     	string;
	tObjectName	object;
};

%token	<integer>	tNUMBER
%token	<string>	tSECTIONNAME
%token	<string>	tSTRING_STAR
%token	<string>	tSTRING_GROUP
%token	<string>	tSTRING_FUNCTION
%token	<string>	tSTRING_ID
%token	<string>	tSTRING_QUATED
%token	<string>	tSTRING_NOSPACE
%type	<string>	gFILENAME
%type	<object>	gOBJECTNAME
%%

/*=========================================================================*
      ConfigFile
 *=========================================================================*/

specFile		: sections
			;

sections		: /*NULL*/
			| sections section
			;

section			: tNL
			| sectionStatic
			| sectionOverlay
			| sectionAutoload
			| sectionProperty
			;

/*===========================================================================
 *     STATIC staticname
 *     {
 *        .....
 *     }
 */
sectionStatic		: staticKeyword begin staticParams end
			;

staticKeyword		: tSTATIC tSTRING_ID tNL
			{
				if ( !StaticSetName( $2 ) ) YYABORT;
			}
			;

staticParams		: /*NULL*/
			| staticParams staticParam
			;

staticParam		: tNL
			| staticAddress
			| staticObjects
			| staticLibraries
			| staticSearchSymbols
			| staticStackSize
			;

staticAddress		: tADDRESS tNUMBER tNL
			{
				if ( !StaticSetAddress( $2 ) ) YYABORT;
			}
			;

staticObjects		: tOBJECT staticObjectList tNL
			{
				if ( !StaticSetObjectSection( "*" ) ) YYABORT;
			}
			| tOBJECT staticObjectList tSECTIONNAME tNL
			{
				if ( !StaticSetObjectSection( $3 ) ) YYABORT;
				debug_printf( "$3=%s\n", $3 );
				free( $3 );
			}
			;

staticObjectList	: /*NULL*/
			| staticObjectList gOBJECTNAME
			{
				if ( !StaticAddObject( $2.string, $2.type ) ) YYABORT;
			}
			;

staticLibraries		: tLIBRARY staticLibraryList tNL
			{
				if ( !StaticSetLibrarySection( "*" ) ) YYABORT;
			}
			| tLIBRARY staticLibraryList tSECTIONNAME tNL
			{
				if ( !StaticSetLibrarySection( $3 ) ) YYABORT;
				debug_printf( "$3=%s\n", $3 );
				free( $3 );
			}
			;

staticLibraryList	: /*NULL*/
			| staticLibraryList gOBJECTNAME
			{
				if ( !StaticAddLibrary( $2.string, $2.type ) ) YYABORT;
			}
			;

staticSearchSymbols	: tSEARCHSYMBOL staticSearchSymbolList tNL
			;

staticSearchSymbolList	: /*NULL*/
			| staticSearchSymbolList tSTRING_ID
			{
				if ( !StaticAddSearchSymbol( $2 ) ) YYABORT;
			}
			;

staticStackSize		: tSTACKSIZE tNUMBER tNL
			{
				if ( !StaticSetStackSize( $2 ) ) YYABORT;
			}
			| tSTACKSIZE tNUMBER tNUMBER
			{
				if ( !StaticSetStackSize( $2 ) ) YYABORT;
				if ( !StaticSetStackSizeIrq( $3 ) ) YYABORT;
			}
			;

/*===========================================================================
 *     OVERLAY overlayname
 *     {
 *        .....
 *     }
 */
sectionOverlay		: overlayKeyword begin overlayParams end
			;

overlayKeyword		: tOVERLAY tSTRING_ID tNL
			{
				if ( !AddOverlay( $2 ) ) YYABORT;
			}
			;

overlayParams		: /*NULL*/
			| overlayParams overlayParam
			;

overlayParam		: tNL
			| overlayGroup
			| overlayAddress
			| overlayAfters
			| overlayObjects
			| overlaySearchSymbols
			| overlayLibraries
			;

overlayGroup		: tGROUP tSTRING_ID tNL
			{
				if ( !OverlaySetGroup( $2 ) ) YYABORT;
			}
			;

overlayAddress		: tADDRESS tNUMBER tNL
			{
				if ( !OverlaySetAddress( $2 ) ) YYABORT;
			}
			;

overlayAfters		: tAFTER overlayAfterList tNL
			;

overlayAfterList	: /*NULL*/
			| overlayAfterList tSTRING_ID
			{
				if ( !OverlayAddAfter( $2 ) ) YYABORT;
			}
			;

overlayObjects		: tOBJECT overlayObjectList tNL
			{
				if ( !OverlaySetObjectSection( "*" ) ) YYABORT;
			}
			| tOBJECT overlayObjectList tSECTIONNAME tNL
			{
				if ( !OverlaySetObjectSection( $3 ) ) YYABORT;
				free( $3 );
			}
			;

overlayObjectList	: /*NULL*/
			| overlayObjectList gOBJECTNAME
			{
				if ( !OverlayAddObject( $2.string, $2.type ) ) YYABORT;
			}
			;

overlayLibraries	: tLIBRARY overlayLibraryList tNL
			{
				if ( !OverlaySetLibrarySection( "*" ) ) YYABORT;
			}
			| tLIBRARY overlayLibraryList tSECTIONNAME tNL
			{
				if ( !OverlaySetLibrarySection( $3 ) ) YYABORT;
				free( $3 );
			}
			;

overlayLibraryList	: /*NULL*/
			| overlayLibraryList gOBJECTNAME
			{
				if ( !OverlayAddLibrary( $2.string, $2.type ) ) YYABORT;
			}
			;

overlaySearchSymbols	: tSEARCHSYMBOL overlaySearchSymbolList tNL
			;

overlaySearchSymbolList	: /*NULL*/
			| overlaySearchSymbolList tSTRING_ID
			{
				if ( !OverlayAddSearchSymbol( $2 ) ) YYABORT;
			}
			;

/*===========================================================================
 *     AUTOLOAD autoloadname
 *     {
 *        .....
 *     }
 */
sectionAutoload		: autoloadKeyword begin autoloadParams end
			;

autoloadKeyword		: tAUTOLOAD tSTRING_ID tNL
			{
				if ( !AddAutoload( $2 ) ) YYABORT;
			}
			;

autoloadParams		: /*NULL*/
			| autoloadParams autoloadParam
			;

/* mostly same as overlayParam except no "group" */
autoloadParam		: tNL
			| overlayAddress
			| overlayAfters
			| overlayObjects
			| overlaySearchSymbols
			| overlayLibraries
			;

/*===========================================================================
 *     PROPERTY
 *     {
 *        .....
 *     }
 */
sectionProperty		: propertyKeyword begin propertyParams end
			;

propertyKeyword		: tPROPERTY tNL
			;

propertyParams		: /*NULL*/
			| propertyParams propertyParam
			;

propertyParam		: tNL
			| propertyName
			| propertyOverlay
			| propertySuffix
			;

propertyName		: tOVERLAYDEFS gFILENAME tNL
			{
				if ( !PropertySetOverlayDefs( $2 ) ) YYABORT;
			}
			;

propertyOverlay		: tOVERLAYTABLE gFILENAME tNL
			{
				if ( !PropertySetOverlayTable( $2 ) ) YYABORT;
			}
			;

propertySuffix		: tSUFFIX gFILENAME tNL
			{
				if ( !PropertySetSuffix( $2 ) ) YYABORT;
			}
			;

/*===========================================================================
 *	MISC
 */

begin			: tBEGIN tNL
			;

end			: tEND tNL
			;

gFILENAME		: tSTRING_ID
			| tSTRING_QUATED
			| tSTRING_NOSPACE
			;

gOBJECTNAME		: gFILENAME
			{
				$$.string = $1;
				$$.type   = OBJTYPE_FILE;
			}
			| tSTRING_STAR
			{
				$$.string = $1;
				$$.type   = OBJTYPE_STAR;
			}
			| tSTRING_GROUP
			{
				$$.string = $1;
				$$.type   = OBJTYPE_GROUP;
			}
			| tSTRING_FUNCTION
			{
				$$.string = $1;
				$$.type   = OBJTYPE_OBJECT;
			}
			;
%%

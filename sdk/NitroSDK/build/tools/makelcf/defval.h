/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - makerom
  File:     defval.h

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: defval.h,v $
  Revision 1.7  2007/02/20 00:28:10  kitase_hirotake
  indent source

  Revision 1.6  2006/03/29 13:13:22  yasu
  IF-ELSE-ENDIF のサポート

  Revision 1.5  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.4  2005/02/28 05:26:03  yosizaki
  do-indent.

  Revision 1.3  2004/08/05 13:38:44  yasu
  Support -M option

  Revision 1.2  2004/07/10 04:06:17  yasu
  Support command 'Library'
  Support modifier ':x' on template
  Fix up line continue '\'

  Revision 1.1  2004/03/26 05:07:33  yasu
  support variables like as -DNAME=VALUE

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef	 DEFVAL_H_
#define	 DEFVAL_H_

#define	DEFVAL_DEFAULT_BUFFER_SIZE	(1024)

BOOL    AddDefValFromFile(char *filename);
void    AddDefVal(char *opt);
const char *SearchDefVal(const char *name);
const char *SearchDefValCleaned(const char *name);
char   *ResolveDefVal(char *str);
char   *ResolveStringModifier(const char *in_value, char modifier);

#endif //DEFVAL_H_

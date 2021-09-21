#! make -f
#---------------------------------------------------------------------------
#  Project:  NitroSDK - tools - makelcf
#  File:     Makefile
#
#  Copyright 2003-2007 Nintendo.  All rights reserved.
#
#  These coded instructions, statements, and computer programs contain
#  proprietary information of Nintendo of America Inc. and/or Nintendo
#  Company Ltd., and are protected by Federal copyright law.  They may
#  not be disclosed to third parties or copied or duplicated in any form,
#  in whole or in part, without the prior written consent of Nintendo.
#
#  $Log: Makefile,v $
#  Revision 1.21  2007/07/10 01:55:36  okubata_ryoma
#  Copyright fix
#
#  Revision 1.20  2007/07/09 12:18:07  yasu
#  TARGET_NAME のサポート
#
#  Revision 1.19  2006/05/11 12:55:47  yasu
#  著作年度の変更
#
#  Revision 1.18  2006/05/10 03:18:56  yasu
#  IF-ELSE-ENDIF の条件判定のチェックおよび searchsymbol 文のテスト
#
#  Revision 1.17  2005/08/29 00:17:13  yasu
#  著作年度修正
#
#  Revision 1.16  2005/08/29 00:12:29  yasu
#  -d オプションをはずす
#
#  Revision 1.15  2005/08/26 11:23:38  yasu
#  ITCM/DTCM への overlay の対応
#
#  Revision 1.14  2005/08/26 04:35:19  yasu
#  DTCM overlay 時のアリーナがおかしくなるのを修正
#
#  Revision 1.13  2004/08/05 13:37:28  yasu
#  Add test code for -M option
#
#  Revision 1.12  2004/07/19 05:08:56  yasu
#  Add rule for super-clobber
#
#  Revision 1.11  2004/07/08 02:59:29  yasu
#  Test section name for multi-objects as 'Objects' parameters
#
#  Revision 1.10  2004/06/24 07:15:58  yasu
#  add test files
#
#  Revision 1.9  2004/05/27 09:58:58  yasu
#  fix specfiles directory to current position
#
#  Revision 1.8  2004/02/16 03:49:21  yasu
#  change switch name CODEGEN_PROC -> NITRO_PROC
#
#  Revision 1.7  2004/02/05 07:09:03  yasu
#  change SDK prefix iris -> nitro
#
#  Revision 1.6  2004/02/03 07:37:16  yasu
#  Add target "autotest"
#
#  Revision 1.5  2004/01/15 13:25:37  yasu
#  add test code
#
#  Revision 1.4  2004/01/15 10:51:24  yasu
#  change a few filenames
#
#  Revision 1.3  2004/01/14 01:59:02  yasu
#  Change filename
#
#  Revision 1.2  2004/01/07 13:11:08  yasu
#  change command casing
#
#  Revision 1.1  2004/01/05 02:32:59  yasu
#  Initial version
#
#  $NoKeywords: $
#---------------------------------------------------------------------------
include	$(NITROSDK_ROOT)/build/buildtools/commondefs

TARGET		=	simple1.lcf simple2.lcf test.autogen.lcf ARM9-TEG.autogen.lcf overlay.autogen.lcf
TARGET		+=	test2.lcf

LCFILE_TEMPLATE =	$(NITRO_INCDIR)/nitro/specfiles/$(NITRO_PROC)-$(NITRO_PLATFORM).lcf.template

%.autogen.lcf:		%.lsf $(MAKELCF) $(LCFILE_TEMPLATE)
			$(MAKELCF) -T $* $(MAKELCF_FLAGS) $< $(LCFILE_TEMPLATE) $@

simple1.lcf:		simple.lsf $(MAKELCF) simple.lcf.template
			$(MAKELCF) $(MAKELCF_FLAGS) -V1 $< simple.lcf.template $@

simple2.lcf:		simple.lsf $(MAKELCF) simple.lcf.template
			$(MAKELCF) -T $(basename $@) $(MAKELCF_FLAGS) -V2 $< simple.lcf.template $@

test2.lcf:		test2.lsf $(MAKELCF) test2.def
			$(MAKELCF) -T $(basename $@) $(MAKELCF_FLAGS) -Mtest2.def $< $(LCFILE_TEMPLATE) $@

MAKELCF =		$(NITRO_TOOLSDIR)/bin/makelcf.exe

.PHONY:			build install do-autotest clean clobber

define ECHO_CURDIR
			echo "==== $(CURDIR)";
endef

build:			
			@$(ECHO_CURDIR)
			@$(MAKE) $(TARGET)

install do-autotest:
			@$(ECHO_CURDIR)

clean clobber super-clobber:
			@$(ECHO_CURDIR)
			-rm -f $(TARGET) *~

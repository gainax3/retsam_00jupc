#! make -f
#---------------------------------------------------------------------------
#  Project:  NitroSDK - tools - defval - test
#  File:     Makefile
#
#  Copyright 2005 Nintendo.  All rights reserved.
#
#  These coded instructions, statements, and computer programs contain
#  proprietary information of Nintendo of America Inc. and/or Nintendo
#  Company Ltd., and are protected by Federal copyright law.  They may
#  not be disclosed to third parties or copied or duplicated in any form,
#  in whole or in part, without the prior written consent of Nintendo.
#
#  $Log: Makefile,v $
#  Revision 1.1  2005/06/20 07:07:15  yasu
#  add defval
#
#  $NoKeywords: $
#---------------------------------------------------------------------------
include	$(NITROSDK_ROOT)/build/buildtools/commondefs

TARGET			= result1.txt result2.txt result3.txt

DEFVAL			= $(NITRO_TOOLSDIR)/bin/defval.exe

result1.txt:		test.txt
			$(DEFVAL) -DOUTPUT=$@ -DFILENAME=d:/home/test/filename.txt $< $@

result2.txt:		test.txt local.def
			$(DEFVAL) -Mlocal.def $< $@

result3.txt:		test.txt local.def
			$(DEFVAL) -Mlocal.def -DOUTPUT=$@ -DFILENAME=d:/home/test/filename.txt $< $@

.PHONY:			build install do-autotest clean clobber

define ECHO_CURDIR
			echo "==== $(CURDIR)";
endef

build:			
			@$(ECHO_CURDIR)
			@$(REMAKE) do-test

do-test:		$(TARGET) goodresult1.txt goodresult2.txt goodresult3.txt
			@diff result1.txt goodresult1.txt || echo "Error: result1.txt unmatched."
			@diff result2.txt goodresult2.txt || echo "Error: result2.txt unmatched."
			@diff result3.txt goodresult3.txt || echo "Error: result3.txt unmatched."

install do-autotest:
			@$(ECHO_CURDIR)

clean clobber super-clobber:
			@$(ECHO_CURDIR)
			-rm -f $(TARGET) *~

#===== End of Makefile =====

#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - libraries - os
# File:     Makefile
#
# Copyright 2003-2005 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.24  2005/05/25 11:45:18  terui
# ライブラリを作成するコア部分を Makefile.CORE に切り出し
# SUBDIRS に TS-Prototype/undefine を追加
#
# Revision 1.23  2005/04/26 08:04:17  terui
# Fix comment
#
# Revision 1.22  2005/04/18 12:25:35  terui
# SRCSにos_china.cを追加
#
# Revision 1.21  2005/02/08 08:39:00  terui
# SRCSにos_vramExclusive.cを追加。
#
# Revision 1.20  2005/01/12 11:10:37  takano_makoto
# fix copyright header.
#
# Revision 1.19  2005/01/12 01:02:18  yada
# add about SDK_TCM_APPLY
#
# Revision 1.18  2004/09/13 05:37:28  yasu
# merged with NitroSDK_2_00pr6_branch
#
# Revision 1.17.2.1  2004/09/13 04:39:29  yasu
# build libos for TS_VERSION=0,100 and 200
#
# Revision 1.17  2004/09/02 07:13:15  yada
# add os_ownerInfo.c
#
# Revision 1.16  2004/09/01 07:06:02  sato_masaki
# add os_reset.c
#
# Revision 1.15  2004/08/27 09:22:05  yasu
# Fix around NITRO_CODEGEN_ALL
#
# Revision 1.14  2004/07/14 14:12:07  yasu
# Change NITRO_CODEGEN=ALL -> NITRO_CODEGEN_ALL=True
#
# Revision 1.13  2004/07/14 11:52:57  yasu
# Build ARM & THUMB code always
#
# Revision 1.12  2004/06/02 01:41:52  yasu
# split profile libs from oslib
#
# Revision 1.11  2004/05/18 06:12:20  yada
# small fix
#
# Revision 1.10  2004/05/12 05:16:50  yada
# consider for CALLTRACE and FUNCTIONCOST
#
# Revision 1.9  2004/05/10 11:45:04  yada
# add profile functions
#
# Revision 1.8  2004/05/06 10:18:18  yasu
# fix typo
#
# Revision 1.7  2004/04/23 00:28:25  yada
# BLXCHECKED 追加
#
# Revision 1.6  2004/04/13 12:21:23  yada
# add os_callTrace.c
#
# Revision 1.5  2004/02/27 01:58:06  yasu
# delete os_halt.c
#
# Revision 1.4  2004/02/25 11:31:45  yada
# systemClock→Tick に伴う変更
#
# Revision 1.3  2004/02/23 11:56:39  yada
# valarm.c 追加
#
# Revision 1.2  2004/02/14 06:34:25  yasu
# add os_halt.c
#
# Revision 1.1  2004/02/12 11:09:37  yada
# ARM9/ARM7 振り分け作業
#
# Revision 1.26  2004/02/10 01:23:41  yada
# os_utility.c を削除
#
# Revision 1.25  2004/02/09 11:36:34  yada
# os_system.c 追加
#
# Revision 1.24  2004/02/09 11:17:49  yasu
# support thumb version library
#
# Revision 1.23  2004/02/05 07:09:02  yasu
# change SDK prefix iris -> nitro
#
# Revision 1.22  2004/02/05 01:58:55  yada
# os_utility.c 追加
#
# Revision 1.21  2004/02/03 11:20:12  yada
# os_alarm.c 追加
#
# Revision 1.20  2004/02/02 05:30:02  yada
# systemClock.c 追加
#
# Revision 1.19  2004/01/27 01:47:13  yada
# os_timer.c 追加
#
# Revision 1.18  2004/01/16 07:57:15  yada
# protectionUnit 記述
#
# Revision 1.17  2004/01/14 12:05:39  yada
# os_tcm.c, os_protecion.c, os_exception.c を追加
#
# Revision 1.16  2004/01/07 00:21:50  yada
# src/os_alloc.c を追加
#
# Revision 1.15  2003/12/26 06:42:58  yada
# os_arena 追加
#
# Revision 1.14  2003/12/22 01:36:34  yada
# os_cache.c, os_init.c 追加
#
# Revision 1.13  2003/12/17 09:33:45  yada
# os_cache.c を追加
#
# Revision 1.12  2003/11/30 09:35:19  yasu
# コンパイル用 ROOT とインストール用 ROOT を独立させた
#
# Revision 1.11  2003/11/29 10:11:09  yada
# os_mutex.c を追加
#
# Revision 1.10  2003/11/29 04:37:01  yasu
# 名称修正
#
# Revision 1.9  2003/11/29 04:04:19  yasu
# os ライブラリのファイル名の変更
#
# Revision 1.8  2003/11/29 03:59:51  yasu
# ファイル名の変更 osInterruptBase.c -> os_irqhandler.c
#
# Revision 1.7  2003/11/26 01:50:03  yada
# osMessage.c 追加
#
# Revision 1.6  2003/11/25 11:18:47  yasu
# emulator.c を追加
#
# Revision 1.5  2003/11/25 05:02:28  yasu
# thread.c / context.c 追加
#
# Revision 1.4  2003/11/13 11:59:01  yasu
# 初版作成
#
# Revision 1.3  2003/11/11 01:52:45  nishida_kenji
# .cファイルを追加
#
# Revision 1.2  2003/11/06 13:37:49  yasu
# osInterruptBase.s を .c に変更
#
# Revision 1.1  2003/11/06 11:34:14  yasu
# 割り込みハンドラ周りの追加
#
# $NoKeywords: $
#----------------------------------------------------------------------------

#---- define if let CallTrace/FunctionCost be available, 
NITRO_PROFILE_TYPE	=
#NITRO_PROFILE_TYPE	= CALLTRACE
#NITRO_PROFILE_TYPE	= FUNCTIONCOST

#----------------------------------------------------------------------------

SUBDIRS			= TS-Prototype/0 TS-Prototype/100 TS-Prototype/200 TS-Prototype/undefine
SUBMAKES		= Makefile.CALLTRACE Makefile.FUNCTIONCOST

#----------------------------------------------------------------------------

include	$(NITROSDK_ROOT)/build/buildtools/commondefs

include	$(NITROSDK_ROOT)/build/buildtools/modulerules

#===== End of Makefile =====

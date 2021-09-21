##-----------------------------------------------------------------------------
#**
#*			WIFIロビーマップコンバータ
#**
##-----------------------------------------------------------------------------


SUBDIRS		=

##############################################################################
#			マップデータ
##############################################################################
CP_DATA = .\\..\\..\\..\\..\\..\\pl_data\\wifi_lobby\\wflby_mapdata.xls
OUTPUT = wflby_map.dat
ARC_FILE 	= wflby_map.narc
ARCH_FILE 	= wflby_map.naix
EXCELL_CONV = ..\\..\\..\\..\\convert\\exceltool\\ExcelSeetConv.exe

##############################################################################
#			グラフィックデータ
##############################################################################
GRA_NAIX_DIR	= .\\..\\..\\..\\graphic\\
GRA_NAIX_FILE	= wifi_lobby.naix
GRA_DATA_FILE	= wflby_3dmapobj_data.c
GRA_TARGET_FILE	= 	wflby_3dmapobj_data_0.dat

include	$(NITROSDK_ROOT)/build/buildtools/commondefs.cctype.CW
include	$(NITROSYSTEM_ROOT)/build/buildtools/commondefs

#Makeで生成される*.bin/*.narcはmake cleanの削除対象にする
LDIRT_CLEAN	= wflby_map.dat \
			  wflby_3dmapobj_data*.dat \
			  wflby_map.narc

#----------------------------------------------------------------------------

include	$(NITROSYSTEM_ROOT)/build/buildtools/modulerules


do-build:cp_data wflby_map.narc
cp_data:
	xcopy /d /y $(CP_DATA) .
	xcopy /d /y $(GRA_NAIX_DIR)$(GRA_NAIX_FILE) .


wflby_map.narc: wflby_map.dat $(GRA_TARGET_FILE)
	nnsarc -c -l -n -i wflby_map.narc -S arc_list.txt

wflby_map.dat:	wflby_mapdata.xls
	$(EXCELL_CONV) -o wflby_map.txt -p 1 -s csv wflby_mapdata.xls
	$(EXCELL_CONV) -o wflby_gdata.txt -p 2 -s csv wflby_mapdata.xls
	perl conv_gdata.pl


wflby_3dmapobj_data_0.dat: $(GRA_NAIX_FILE) $(GRA_DATA_FILE)
	perl conv_3dmapobj_data.pl

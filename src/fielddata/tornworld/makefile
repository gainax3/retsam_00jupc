##################################################################
#	tornworld makefile
##################################################################

#include MAPTABLE_LIST
include tw_maptable_list

#パス
SRCDIR		:= .
EVJUMPDIR	:= ./tw_evjump
CMACTDIR	:= ./tw_cmact
ATTRDIR		:= ./tw_attr
GNDDIR		:= ./tw_ground
FSTONEDIR	:= ./tw_fstone

#アプリ
EXCELCSV	= ../../../convert/exceltool/excelcommaconv

#ゾーンIDヘッダー zone_id.h
ZONE_ID_H         = ../maptable/zone_id.h

#コンバータ
EVCONV_JUMP		= ruby evcvjmpall.rb
EVCONV_CMACT	= ruby evcvcmactall.rb
GNDCONV_GND		= ruby gndcvall.rb
MAPCONV_MTBL	= ruby mtblcv.rb
CONV_FSTONE		= ruby fstonecvall.rb

#--------------------------------
#	アーカイブ結果出力先
#--------------------------------
ARC_RESULT      = arc_result.txt
ARC_ATTR_RESULT = arc_result_attr.txt

#--------------------------------
#	ターゲット
#--------------------------------
TARGET = tw_arc.narc
HEADER = $(TARGET:.narc=.naix)

#--------------------------------
#	アトリビュートファイル一覧
#--------------------------------
ATTR_TARGET  = tw_arc_attr.narc
ATTR_HEADER  = $(ATTRTRGET:.narc=.naix)
ATTR_LIST    = $(ATTRDIR)/*.dat
ATTR_CLEANLIST = $(ATTR_TARGET) $(ATTR_HEADER)

#--------------------------------
#	tw_maptable.xls
#--------------------------------
#更新データ
MAPTABLE_LISTFILE = tw_maptable_list
#xls
MAPTABLE_XLS      = tw_maptable.xls
#生成されるtxt,index
MAPTABLE_TXT      = $(MAPTABLE_XLS:.xls=.txt)
MAPTABLE_IDX      = $(MAPTABLE_XLS:.xls=_idx)
#clean cls
MAPTABLE_CLEANLIST  = $(MAPTABLE_LIST) $(MAPTABLE_TXT)

#--------------------------------
#	"tw_ground"
#--------------------------------
#生成データ
GNDDATA_GNDLIST  = gnddata_gndlist
#xlsファイル群
GNDDATA_XLSLIST  = $(GNDDIR)/*.xls
GNDDATA_HEADLIST = $(GNDDATA_XLSLIST:.xls=.h)
GNDDATA_BINLIST  = $(GNDDATA_XLSLIST:.xls=.bin)
GNDDATA_TXTLIST  = $(GNDDATA_XLSLIST:.xls=.txt)
#clean cls
GNDDATA_CLEANLIST  = $(GNDDATA_GNDLIST) $(GNDDATA_HEADLIST) \
				   $(GNDDATA_BINLIST) $(GNDDATA_TXTLIST)

#--------------------------------
#	"tw_evjump"
#--------------------------------
#生成データ
EVJUMP_JUMPLIST = evdata_jumplist
#xlsファイル郡
EVJUMP_XLSLIST  = $(EVJUMPDIR)/*.xls
EVJUMP_BINLIST  = $(EVJUMP_XLSLIST:.xls=.bin)
EVJUMP_TXTLIST  = $(EVJUMP_XLSLIST:.xls=.txt)
#clean cls
EVJUMP_CLEANLIST  = $(EVJUMP_JUMPLIST) $(EVJUMP_BINLIST) $(EVJUMP_TXTLIST)
				  
#--------------------------------
#	"tw_cmact"
#--------------------------------
#生成データ
CMACT_LIST      = cmact_list
#xlsファイル群
CMACT_XLSLIST   = $(CMACTDIR)/*.xls
CMACT_BINLIST   = $(CMACT_XLSLIST:.xls=.bin)
CMACT_TXTLIST   = $(CMACT_XLSLIST:.xls=.txt)
#clean cls
CMACT_CLEANLIST   = $(CMACT_LIST) $(CMACT_BINLIST) $(CMACT_TXTLIST)

#--------------------------------
#	"tw_fstone"
#--------------------------------
#生成データ
FSTONE_LIST      = fstone_list
#xlsファイル群
FSTONE_XLSLIST   = $(FSTONEDIR)/*.xls
FSTONE_BINLIST   = $(FSTONE_XLSLIST:.xls=.bin)
FSTONE_TXTLIST   = $(FSTONE_XLSLIST:.xls=.txt)
#clean cls
FSTONE_CLEANLIST   = $(FSTONE_LIST) $(FSTONE_BINLIST) $(FSTONE_TXTLIST)

#--------------------------------
#	tw_maptable_list更新対象
#--------------------------------
MAPTABLE_UPDATELIST = $(GNDDATA_GNDLIST) $(EVJUMP_JUMPLIST) $(CMACT_LIST) $(FSTONE_LIST)

#--------------------------------
#	SUFFIXES
#--------------------------------

#--------------------------------
#	clean対象
#--------------------------------
TW_CLEAN_LIST = $(MAPTABLE_CLEANLIST) $(GNDDATA_CLEANLIST) $(EVJUMP_CLEANLIST) $(CMACT_CLEANLIST) $(FSTONE_CLEANLIST) $(ARC_RESULT) $(ARC_ATTR_RESULT)

#--------------------------------
#	ビルド対象
#--------------------------------
do-build: $(ATTR_TARGET) $(TARGET)

#--------------------------------
#	アトリビュートアーカイブ生成
#--------------------------------
$(ATTR_TARGET): $(ATTRDIR) $(ATTR_LIST)
	nnsarc -i -c -l -n $(ATTR_TARGET) $(ATTR_LIST) > $(ARC_ATTR_RESULT)

#--------------------------------
#	TARGET生成
#--------------------------------
$(TARGET): $(MAPTABLE_IDX)
	nnsarc -i -c -l -n $(TARGET) $(MAPTABLE_LIST) > $(ARC_RESULT)

#--------------------------------
#	gnddata_gndlist生成
#--------------------------------
$(GNDDATA_BINLIST): $(GNDDATA_XLSLIST) $(ATTR_TARGET)
$(GNDDATA_GNDLIST): $(GNDDIR) $(GNDDATA_BINLIST)
	$(GNDCONV_GND) $(GNDDATA_GNDLIST)

#--------------------------------
#	evjump_jumplist生成
#--------------------------------
$(EVJUMP_BINLIST): $(EVJUMP_XLSLIST)
$(EVJUMP_JUMPLIST): $(EVJUMPDIR) $(EVJUMP_BINLIST) $(GNDDATA_GNDLIST)
	$(EVCONV_JUMP) $(EVJUMP_JUMPLIST)

#--------------------------------
#	cmact_list生成
#--------------------------------
$(CMACT_BINLIST): $(CMACT_XLSLIST)
$(CMACT_LIST): $(CMACTDIR) $(CMACT_BINLIST)
	$(EVCONV_CMACT) $(CMACT_LIST)

#--------------------------------
#	fstone_list生成
#--------------------------------
$(FSTONE_BINLIST): $(FSTONE_XLSLIST)
$(FSTONE_LIST): $(FSTONEDIR) $(FSTONE_BINLIST)
	$(CONV_FSTONE) $(FSTONE_LIST)

#--------------------------------
#	tw_maptable_idx,tw_maptable_list
#--------------------------------
$(MAPTABLE_IDX): $(MAPTABLE_XLS) $(MAPTABLE_UPDATELIST) $(ZONE_ID_H)
	$(MAPCONV_MTBL) $(MAPTABLE_XLS) $(MAPTABLE_LISTFILE) $(ZONE_ID_H)
	
#--------------------------------
#	make all
#--------------------------------
all:
	$(MAKE) clean
	$(MAKE)

#--------------------------------
#	make clean
#--------------------------------
clean:
	-rm $(TARGET) $(HEADER) $(ATTR_TARGET) $(ATTR_HEADER) $(TW_CLEAN_LIST)

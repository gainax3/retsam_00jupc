#----------------------------------------------------------------------------
#
#	動作モデルアーカイブ用Makefile
#
#	2006.03.10
#	tamada GAME FREAK inc.
#----------------------------------------------------------------------------

include $(NITROSDK_ROOT)/build/buildtools/commondefs.wine

TARGET 	=	mmodel.narc
HEADER	=	$(TARGET:.narc=.naix)

TARGET_FE 	=	fldeff.narc
HEADER_FE	=	$(TARGET_FE:.narc=.naix)

SRCDIR	:= ../rsc

#コンバート対象のimdを指定する
include	mmodel_list
include	fldeff_list

#G3Dコンバータへのパス
G3DCVTR	= $(WINE) $(NITROSYSTEM_ROOT)/tools/win/bin/g3dcvtr.exe

#G2Dコンバータへのパス
G2DCVTR	= $(NITROSYSTEM_ROOT)/tools/win/bin/g2dcvtr.exe

#itpコンバータへのパス
ITPCVTR = $(WINE) ./itp_conv.exe

#--------------------------------------------------
#	ファイルリスト置き換え
#--------------------------------------------------
#G3D_MMODEL_MDLからディレクトリ名部分を削除して、拡張子をimdからnsbmdに置き換える
MDATA_MDL	= $(notdir $(G3D_MMODEL_MDL:.imd=.nsbmd))
FEDATA_MDL	= $(notdir $(G3D_FE_MDL:.imd=.nsbmd))

#G3D_MMODEL_TEXからディレクトリ名部分を削除して、拡張子をimdからnsbtxに置き換える
MDATA_TEX	= $(notdir $(G3D_MMODEL_TEX:.imd=.nsbtx))
FEDATA_TEX	= $(notdir $(G3D_FE_TEX:.imd=.nsbtx))

#G3D_MMODEL_ANMDATからディレクトリ名部分を削除して、拡張子をitpからnsbtpに置き換える
FEDATA_ANMITP	= $(notdir $(G3D_FE_ANMITP:.itp=.nsbtp))

#G3D_MMODEL_ANMDATからディレクトリ名部分を削除して、拡張子をitpから_itpcv.datに置き換える
MDATA_ANM	= $(notdir $(G3D_MMODEL_ANM:.itp=_itpcv.dat))
FEDATA_ANMITPCV	= $(notdir $(G3D_FE_ANMITPCV:.itp=_itpcv.dat))

#G3D_FE_ANMICAからディレクトリ名部分を削除して、拡張子をicaからnsbcaに置き換える
FEDATA_ANMICA = $(notdir $(G3D_FE_ANMICA:.ica=.nsbca))

#G3D_FE_ANMIMAからディレクトリ名部分を削除して、拡張子をimaからnsbmaに置き換える
FEDATA_ANMIMA = $(notdir $(G3D_FE_ANMIMA:.ima=.nsbma))

#G3D_FE_ANMITAからディレクトリ名部分を削除して、拡張子をitaからnsbtaに置き換える
FEDATA_ANMITA = $(notdir $(G3D_FE_ANMITA:.ita=.nsbta))

#G3D_FE_ANMIVAからディレクトリ名部分を削除して、拡張子をivaからnsbvaに置き換える
FEDATA_ANMIVA = $(notdir $(G3D_FE_ANMIVA:.iva=.nsbva))

#G3D_FE_NONCV
FEDATA_NONCV = $(G3D_FE_NONCV)

#--------------------------------------------------
#	拡張子で生成ルールを指定する
#--------------------------------------------------
#imd --> nsbmdへの変換ルール定義
%.nsbmd: $(SRCDIR)/%.imd
	@echo Model Data Convert: $^ → $@
	@$(G3DCVTR) $(SRCDIR)/$(@:.nsbmd=.imd) -o $@
	
#imd --> nsbtxへの変換ルール定義
%.nsbtx: $(SRCDIR)/%.imd
	@echo Texture Data Convert: $^ → $@
	@$(G3DCVTR) $(SRCDIR)/$(@:.nsbtx=.imd) -o $@ -etex

#itp --> nsbtpへの変換ルール定義
%.nsbtp: $(SRCDIR)/%.itp
	@echo Anime itp File Convert: $^ → $@
	@$(G3DCVTR) $(SRCDIR)/$(@:.nsbtp=.itp) -o $@ 

#itp --> itpcv.datへの変換ルール定義
%_itpcv.dat: $(SRCDIR)/%.itp
	@echo Anime itp convert File Convert: $^ → $@
	@$(ITPCVTR) $(SRCDIR)/$(@:_itpcv.dat=.itp) $@

#ica --> nsbcaへの変換ルール定義
%.nsbca: $(SRCDIR)/%.ica
	@echo Anime ica File Convert: $^ → $@
	@$(G3DCVTR) $(SRCDIR)/$(@:.nsbca=.ica) -o $@ 

#ima --> nsbmaへの変換ルール定義
%.nsbma: $(SRCDIR)/%.ima
	@echo Anime ima File Convert: $^ → $@
	@$(G3DCVTR) $(SRCDIR)/$(@:.nsbma=.ima) -o $@ 

#ita --> nsbtaへの変換ルール定義
%.nsbta: $(SRCDIR)/%.ita
	@echo Anime ita File Convert: $^ → $@
	@$(G3DCVTR) $(SRCDIR)/$(@:.nsbta=.ita) -o $@ 

#iva --> nsbvaへの変換ルール定義
%.nsbva: $(SRCDIR)/%.vai
	@echo Anime iva File Convert: $^ → $@
	@$(G3DCVTR) $(SRCDIR)/$(@:.nsbva=.iva) -o $@ 

#-------------------------------------------------
#	生成ルール定義
#-------------------------------------------------
do-build: $(TARGET) $(TARGET_FE)

#	$(TARGET)=mmodel.narcは$(MDATA_TEX)=nsbtxファイルに依存している
#	さらに依存関係を解決したら、nnsarcでアーカイブを行う
$(TARGET): $(MDATA_TEX) $(MDATA_MDL) $(MDATA_ANM)
	nnsarc -i -c -l -n $(TARGET) $(MDATA_TEX) $(MDATA_MDL) $(MDATA_ANM) > arc_result.txt

$(TARGET_FE): $(FEDATA_TEX) $(FEDATA_MDL) $(FEDATA_ANMITP) $(FEDATA_ANMITPCV) $(FEDATA_ANMICA) $(FEDATA_ANMIMA) $(FEDATA_ANMITA) $(FEDATA_ANMIVA) $(FEDATA_NONCV)
	
	nnsarc -i -c -l -n $(TARGET_FE) $(FEDATA_TEX) $(FEDATA_MDL) $(FEDATA_ANMITP) $(FEDATA_ANMITPCV) $(FEDATA_ANMICA) $(FEDATA_ANMIMA) $(FEDATA_ANMITA) $(FEDATA_ANMIVA) $(FEDATA_NONCV) > arc_result_fe.txt

all:
	$(MAKE) clean
	$(MAKE)

#全消去
clean:
	-rm $(TARGET) $(HEADER)					#アーカイブとヘッダファイルを削除
	-rm $(TARGET_FE) $(HEADER_FE)	#アーカイブとヘッダファイルを削除
	-rm $(MDATA_TEX)						#アーカイブ対象の*.nsbtxを削除
	-rm $(FEDATA_TEX)						#アーカイブ対象の*.nsbtxを削除
	-rm $(MDATA_MDL)						#アーカイブ対象の*.nsbmdを削除
	-rm $(FEDATA_MDL)						#アーカイブ対象の*.nsbmdを削除
	-rm $(MDATA_ANM)						#アーカイブ対象の*_itpcv.datを削除
	-rm $(FEDATA_ANMITP)					#アーカイブ対象の*.nsbtpを削除
	-rm $(FEDATA_ANMITPCV)			#アーカイブ対象の*_itpcv.datを削除
	-rm $(FEDATA_ANMICA)			#アーカイブ対象の*.nsbcaを削除
	-rm $(FEDATA_ANMIMA)			#アーカイブ対象の*.nsbmaを削除
	-rm $(FEDATA_ANMITA)			#アーカイブ対象の*.nsbtaを削除
	-rm $(FEDATA_ANMIVA)			#アーカイブ対象の*.nsbvaを削除

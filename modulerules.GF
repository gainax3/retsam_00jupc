G3DCVTR	= $(WINE) $(NITROSYSTEM_ROOT)/tools/win/bin/g3dcvtr.exe

%.nsbmd: $(subst $(DATABIN),$(DATASRC)/,$(@:.nsbmd=.imd))
	$(G3DCVTR) $(subst $(DATABIN),$(DATASRC)/,$(@:.nsbmd=.imd)) -o $@ 

%.nsbmt: $(subst $(DATABIN),$(DATASRC)/,$(@:.nsbmt=.imd))
	$(G3DCVTR) $(subst $(DATABIN),$(DATASRC)/,$(@:.nsbmt=.imd)) -o $(@:.nsbmt=.nsbmd) -emdl
	mv $(@:.nsbmt=.nsbmd) $@

%.nsbtx: $(subst $(DATABIN),$(DATASRC)/,$(@:.nsbtx=.imd))
	$(G3DCVTR) $(subst $(DATABIN),$(DATASRC)/,$(@:.nsbtx=.imd)) -o $@ -etex

%.nsbca: $(subst $(DATABIN),$(DATASRC)/,$(@:.nsbca=.ica))
	$(G3DCVTR) $(subst $(DATABIN),$(DATASRC)/,$(@:.nsbca=.ica)) -o $@

%.nsbva: $(subst $(DATABIN),$(DATASRC)/,$(@:.nsbva=.iva))
	$(G3DCVTR) $(subst $(DATABIN),$(DATASRC)/,$(@:.nsbva=.iva)) -o $@

%.nsbma: $(subst $(DATABIN),$(DATASRC)/,$(@:.nsbma=.ima))
	$(G3DCVTR) $(subst $(DATABIN),$(DATASRC)/,$(@:.nsbma=.ima)) -o $@

%.nsbtp: $(subst $(DATABIN),$(DATASRC)/,$(@:.nsbtp=.itp))
	$(G3DCVTR) $(subst $(DATABIN),$(DATASRC)/,$(@:.nsbtp=.itp)) -o $@

%.nsbta: $(subst $(DATABIN),$(DATASRC)/,$(@:.nsbta=.ita))
	$(G3DCVTR) $(subst $(DATABIN),$(DATASRC)/,$(@:.nsbta=.ita)) -o $@


include $(NITROSYSTEM_ROOT)/build/buildtools/modulerules

MAKELCF_DEFS	= -DNITROSDK_ROOT='$(ROOT)'			\
		  -DTARGET_NAME='$(TARGET_NAME)'		\
		  -Mlcf_def.txt	\
		  -DGLIBS='$(GLIBRARIES_LINK)'			\
		  -DLLIBS='$(LLIBRARIES_LINK)'			\
		  -DCW_LIBS='$(CW_LIBS)'			\
		  -DOBJDIR='$(OBJDIR)'				\
		  -DLIBDIR='$(LIBDIR)'				\
		  -DBINDIR='$(BINDIR)'				\
		  -DPROC='$(CODEGEN_PROC)'			\
		  -DPLATFORM='$(NITRO_PLATFORM)'		\
		  -DCODEGEN='$(CODEGEN_ARCH)'			\
		  -DBUILD='$(NITRO_BUILD_DIR)'			\
		  -DADDRESS_DTCM='$(ADDRESS_DTCM)'
 

# PROJECT_ROOT defined in env

SED := sed -r

include ../../NitroSDK/build/buildtools/commondefs.wine

ifeq ($(WINE),)
WINPATH := wslpath
else
WINPATH := winepath
endif

MWCC = $(WINE) ../ARM_Tools/Command_Line_Tools/2.0/sp2/mwccarm.exe
MWAR = $(WINE) ../ARM_Tools/Command_Line_Tools/mwldarm.exe

ARFLAGS = -msgstyle std -w on -library

MSL_C_ARCHIVE_FILE := msl/MSL_C/MSL_ARM/Lib/MSL_C_NITRO_Ai_LE.a
MSL_EXTRAS_ARCHIVE_FILE := msl/MSL_Extras/MSL_ARM/Lib/MSL_Extras_NITRO_Ai_LE.a
MSL_CXX_ARCHIVE_FILE := msl/MSL_C++/MSL_ARM/Lib/MSL_CPP_NITRO_Ai_LE.a

# MSL_C_FILES defined here
include msl_c_files.mk
MSL_EXTRAS_FILES := ./msl/MSL_Extras/MSL_Common/Src/extras.c
MSL_CXX_FILES := ./msl/MSL_C++/MSL_Common/Src/strstream.cpp \
    ./msl/MSL_C++/MSL_Common/Src/complex.cpp \
    ./msl/MSL_C++/MSL_Common/Src/ios.cpp \
    ./msl/MSL_C++/MSL_Common/Src/iostream.cpp \
    ./msl/MSL_C++/MSL_Common/Src/locale.cpp \
    ./msl/MSL_C++/MSL_Common/Src/MSLstring.cpp \
    ./msl/MSL_C++/MSL_Common/Src/msl_condition.cpp \
    ./msl/MSL_C++/MSL_Common/Src/msl_mutex.cpp \
    ./msl/MSL_C++/MSL_Common/Src/msl_thread.cpp \
    ./msl/MSL_C++/MSL_Common/Src/msl_time.cpp

MSL_C_OBJECT_FILES := $(MSL_C_FILES:%.c=%.o)
MSL_EXTRAS_OBJECT_FILES := $(MSL_EXTRAS_FILES:%.c=%.o)
MSL_CXX_OBJECT_FILES := $(MSL_CXX_FILES:%.cpp=%.o)

MSL_C_DEPFILES := $(MSL_C_FILES:%.c=%.d)
MSL_EXTRAS_DEPFILES := $(MSL_EXTRAS_FILES:%.c=%.d)
MSL_CXX_DEPFILES := $(MSL_CXX_FILES:%.cpp=%.d)

ifneq ($(WINPATH),)
  PROJECT_ROOT_NT := $(shell $(WINPATH) -w $(PROJECT_ROOT) | $(SED) 's/\\/\//g')
  define fixdep
    $(SED) -i 's/\r//g; s/\\/\//g; s/\/$$/\\/g; s#$(PROJECT_ROOT_NT)#$(PROJECT_ROOT)#g' $(1)
    touch -r $(1:%.d=%.o) $(1)
  endef
else
  define fixdep
    $(SED) -i 's/\r//g; s/\\/\//g; s/\/$$/\\/g' $(1)
    touch -r $(1:%.d=%.o) $(1)
  endef
endif

.SECONDARY:
.SECONDEXPANSION:
.DELETE_ON_ERROR:
.PHONY: all clean

all: $(MSL_C_ARCHIVE_FILE) $(MSL_EXTRAS_ARCHIVE_FILE) $(MSL_CXX_ARCHIVE_FILE)

DEPFILES := $(MSL_C_DEPFILES) $(MSL_EXTRAS_DEPFILES) $(MSL_CXX_DEPFILES)

# TODO: warnings
MSL_C_MWCFLAGS := -O4 -opt speed -proc arm946e -interworking -trigraphs off -stdkeywords off -enum min -ansi off -wchar_t on -cpp_exceptions on -strings reuse -inline on,noauto,bottomup -RTTI on -char signed -bool on -lang c -ipa file -gccinc
MSL_EXTRAS_MWCFLAGS := -O4 -opt speed -proc arm946e -interworking -requireprotos -trigraphs off -stdkeywords off -enum min -ansi off -wchar_t on -cpp_exceptions off -strings reuse -inline on,noauto,bottomup -RTTI off -char unsigned -bool off -iso_templates on -lang c99 -ipa file -gccinc
MSL_CXX_MWCFLAGS := -O4 -opt speed -proc arm946e -interworking -lang c++ -requireprotos -trigraphs on -stdkeywords off -enum min -ansi off -wchar_t on -cpp_exceptions on -strings reuse -inline on,noauto,level=8 -RTTI on -char unsigned -bool on -iso_templates on -ipa file -gccinc

CW_INCLUDE_DIRS := ./msl/MSL_C/MSL_ARM/Include		\
		  ./msl/MSL_C/MSL_Common/Include		\
		  ./msl/MSL_C/MSL_Common_Embedded/Math/Include \
		  ./msl/MSL_C++/MSL_ARM/Include		\
		  ./msl/MSL_C++/MSL_Common/Include		\
		  ./msl/MSL_Extras/MSL_Common/Include	\
		  ./Profiler/include \
		  ../../NitroSDK/include

INCLUDE_FLAGS := $(foreach dname,$(CW_INCLUDE_DIRS),-I$(dname))

$(DEPFILES):

$(MSL_C_OBJECT_FILES): %.o: %.c
$(MSL_C_OBJECT_FILES): %.o: %.c %.d
	$(MWCC) $(MSL_C_MWCFLAGS) $(INCLUDE_FLAGS) -include PREFIX_NITRO.h -gccdep -MD -c -o $@ $<
	@$(call fixdep,$*.d)

$(MSL_EXTRAS_OBJECT_FILES): %.o: %.c
$(MSL_EXTRAS_OBJECT_FILES): %.o: %.c %.d
	$(MWCC) $(MSL_EXTRAS_MWCFLAGS) $(INCLUDE_FLAGS) -I./msl/MSL_Extras/MSL_ARM/Include -include PREFIX_NITRO.h -gccdep -MD -c -o $@ $<
	@$(call fixdep,$*.d)

$(MSL_CXX_OBJECT_FILES): %.o: %.cpp
$(MSL_CXX_OBJECT_FILES): %.o: %.cpp %.d
	$(MWCC) $(MSL_CXX_MWCFLAGS) $(INCLUDE_FLAGS) -gccdep -MD -c -o $@ $<
	@$(call fixdep,$*.d)

$(MSL_C_ARCHIVE_FILE): $(MSL_C_OBJECT_FILES)
	$(MWAR) $(ARFLAGS) $^ -o $@

$(MSL_EXTRAS_ARCHIVE_FILE): $(MSL_EXTRAS_OBJECT_FILES)
	$(MWAR) $(ARFLAGS) $^ -o $@

$(MSL_CXX_ARCHIVE_FILE): $(MSL_CXX_OBJECT_FILES)
	$(MWAR) $(ARFLAGS) $^ -o $@

include $(wildcard $(DEPFILES))

clean:
	rm -f $(MSL_C_ARCHIVE_FILE) $(MSL_EXTRAS_ARCHIVE_FILE) $(MSL_CXX_ARCHIVE_FILE)
	rm -f $(MSL_C_DEPFILES)
	rm -f $(MSL_EXTRAS_DEPFILES)
	rm -f $(MSL_CXX_DEPFILES)
	rm -f $(MSL_C_OBJECT_FILES)
	rm -f $(MSL_EXTRAS_OBJECT_FILES)
	rm -f $(MSL_CXX_OBJECT_FILES)

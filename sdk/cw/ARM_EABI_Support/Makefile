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
NITRO_RUNTIME_ARCHIVE_FILE := Runtime/Runtime_ARM/Runtime_NITRO/Lib/NITRO_Runtime_Ai_LE.a

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

NITRO_RUNTIME_FILES := ./Runtime/Runtime_ARM/Common_Source/rt_div0.c \
    ./Runtime/Runtime_ARM/Common_Source/semihosted_console_io.c \
    ./Runtime/Runtime_ARM/Common_Source/setjmp.c \
    ./Runtime/Runtime_ARM/Common_Source/startup.c \
    ./Runtime/RISC_Common/CompressedNumbers.cpp \
    ./Runtime/RISC_Common/ExceptionHandler.cpp \
    ./Runtime/Runtime_Common/New.cpp \
    ./Runtime/RISC_Common/StaticInitializers.cpp \
    ./Runtime/Runtime_ARM/Common_Source/_rt_fp_status_addr.c \
    ./Runtime/Runtime_ARM/Common_Source/_rt_fp_env_addr.c \
    ./Runtime/Runtime_Common/vla_alloc.c \
    ./Runtime/Runtime_Common/cxxabi/cxxabi_alloc.cpp \
    ./Runtime/Runtime_Common/cxxabi/cxxabi_badeh.cpp \
    ./Runtime/Runtime_Common/cxxabi/cxxabi_onetimeinit.cpp \
    ./Runtime/Runtime_Common/cxxabi/cxxabi_ptmf.cpp \
    ./Runtime/Runtime_Common/cxxabi/cxxabi_rtti.cpp \
    ./Runtime/Runtime_Common/MWException.cpp \
    ./Runtime/Runtime_ARM/Common_Source/ExceptionHandlerTS.cpp \
    ./Runtime/Runtime_ARM/Common_Source/__NITRO_eabi_init.c \
    ./Runtime/Runtime_Common/cxxabi/cxxabi_clear.c

NITRO_RUNTIME_C_FILES := $(filter %.c,$(NITRO_RUNTIME_FILES))
NITRO_RUNTIME_CXX_FILES := $(filter %.cpp,$(NITRO_RUNTIME_FILES))

MSL_C_OBJECT_FILES := $(MSL_C_FILES:%.c=%.o)
MSL_EXTRAS_OBJECT_FILES := $(MSL_EXTRAS_FILES:%.c=%.o)
MSL_CXX_OBJECT_FILES := $(MSL_CXX_FILES:%.cpp=%.o)
NITRO_RUNTIME_C_OBJECT_FILES := $(NITRO_RUNTIME_C_FILES:%.c=%.o)
NITRO_RUNTIME_CXX_OBJECT_FILES := $(NITRO_RUNTIME_CXX_FILES:%.cpp=%.o)
NITRO_RUNTIME_OBJECT_FILES := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(NITRO_RUNTIME_FILES)))

MSL_C_DEPFILES := $(MSL_C_FILES:%.c=%.d)
MSL_EXTRAS_DEPFILES := $(MSL_EXTRAS_FILES:%.c=%.d)
MSL_CXX_DEPFILES := $(MSL_CXX_FILES:%.cpp=%.d)
NITRO_RUNTIME_DEPFILES := $(NITRO_RUNTIME_OBJECT_FILES:%.o=%.d)

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

all: $(MSL_C_ARCHIVE_FILE) $(MSL_EXTRAS_ARCHIVE_FILE) $(MSL_CXX_ARCHIVE_FILE) $(NITRO_RUNTIME_ARCHIVE_FILE)

DEPFILES := $(MSL_C_DEPFILES) $(MSL_EXTRAS_DEPFILES) $(MSL_CXX_DEPFILES) $(NITRO_RUNTIME_DEPFILES)

# TODO: warnings
MSL_C_MWCFLAGS := -O4 -opt speed -proc arm946e -interworking -trigraphs off -stdkeywords off -enum min -ansi off -wchar_t on -cpp_exceptions on -strings reuse -inline on,noauto,bottomup -RTTI on -char signed -bool on -lang c -ipa file -gccinc
MSL_EXTRAS_MWCFLAGS := -O4 -opt speed -proc arm946e -interworking -requireprotos -trigraphs off -stdkeywords off -enum min -ansi off -wchar_t on -cpp_exceptions off -strings reuse -inline on,noauto,bottomup -RTTI off -char unsigned -bool off -iso_templates on -lang c99 -ipa file -gccinc
MSL_CXX_MWCFLAGS := -O4 -opt speed -proc arm946e -interworking -lang c++ -requireprotos -trigraphs on -stdkeywords off -enum min -ansi off -wchar_t on -cpp_exceptions on -strings reuse -inline on,noauto,level=8 -RTTI on -char unsigned -bool on -iso_templates on -ipa file -gccinc
NITRO_RUNTIME_MWCFLAGS := -O4 -opt speed -proc arm946e -interworking -trigraphs off -stdkeywords off -enum min -ansi off -wchar_t on -cpp_exceptions on -strings reuse -inline on,noauto,bottomup -RTTI on -char unsigned -bool on -iso_templates off -lang c99 -ipa file -gccinc

CW_INCLUDE_DIRS := ./msl/MSL_C/MSL_ARM/Include		\
		  ./msl/MSL_C/MSL_Common/Include		\
		  ./msl/MSL_C/MSL_Common_Embedded/Math/Include \
		  ./msl/MSL_C++/MSL_ARM/Include		\
		  ./msl/MSL_C++/MSL_Common/Include		\
		  ./msl/MSL_Extras/MSL_Common/Include	\
		  ./Profiler/include \
		  ../../NitroSDK/include

INCLUDE_FLAGS := $(foreach dname,$(CW_INCLUDE_DIRS),-I$(dname))
NITRO_RUNTIME_INCLUDE_FLAGS := -I./Runtime/Runtime_Common/MWRuntime -I./Runtime/Runtime_Common -I./Runtime/Runtime_ARM/Common_Source -I./Runtime/Runtime_Common/cxxabi -I./Runtime/RISC_Common

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

$(NITRO_RUNTIME_C_OBJECT_FILES): %.o: %.c
$(NITRO_RUNTIME_C_OBJECT_FILES): %.o: %.c %.d
	$(MWCC) $(NITRO_RUNTIME_MWCFLAGS) $(INCLUDE_FLAGS) $(NITRO_RUNTIME_INCLUDE_FLAGS) -gccdep -MD -c -o $@ $<
	@$(call fixdep,$*.d)

$(NITRO_RUNTIME_CXX_OBJECT_FILES): %.o: %.cpp
$(NITRO_RUNTIME_CXX_OBJECT_FILES): %.o: %.cpp %.d
	$(MWCC) $(NITRO_RUNTIME_MWCFLAGS) $(INCLUDE_FLAGS) $(NITRO_RUNTIME_INCLUDE_FLAGS) -gccdep -MD -c -o $@ $<
	@$(call fixdep,$*.d)

$(MSL_C_ARCHIVE_FILE): $(MSL_C_OBJECT_FILES)
	$(MWAR) $(ARFLAGS) $^ -o $@

$(MSL_EXTRAS_ARCHIVE_FILE): $(MSL_EXTRAS_OBJECT_FILES)
	$(MWAR) $(ARFLAGS) $^ -o $@

$(MSL_CXX_ARCHIVE_FILE): $(MSL_CXX_OBJECT_FILES)
	$(MWAR) $(ARFLAGS) $^ -o $@

$(NITRO_RUNTIME_ARCHIVE_FILE): $(NITRO_RUNTIME_OBJECT_FILES)
	$(MWAR) $(ARFLAGS) $^ -o $@

include $(wildcard $(DEPFILES))

clean:
	rm -f $(MSL_C_ARCHIVE_FILE) $(MSL_EXTRAS_ARCHIVE_FILE) $(MSL_CXX_ARCHIVE_FILE) $(NITRO_RUNTIME_ARCHIVE_FILE)
	rm -f $(MSL_C_DEPFILES)
	rm -f $(MSL_EXTRAS_DEPFILES)
	rm -f $(MSL_CXX_DEPFILES)
	rm -f $(NITRO_RUNTIME_DEPFILES)
	rm -f $(MSL_C_OBJECT_FILES)
	rm -f $(MSL_EXTRAS_OBJECT_FILES)
	rm -f $(MSL_CXX_OBJECT_FILES)
	rm -f $(NITRO_RUNTIME_OBJECT_FILES)


include ../sdk/NitroSDK/build/buildtools/commondefs.wine

AS = $(WINE) ../sdk/cw/ARM_Tools/Command_Line_Tools/mwasmarm.exe
AR = $(WINE) ../sdk/cw/ARM_Tools/Command_Line_Tools/mwldarm.exe
ARFLAGS = -msgstyle std -w on -library

all: libsyscall.a

ASFLAGS = -proc arm5te -i src/ -msgstyle std

.PRECIOUS: libsyscall.o

src/%.s: ;

libsyscall.o: libsyscall.s
	$(AS) $(ASFLAGS) -c $< -MD -o $@

libsyscall.a: libsyscall.o
	$(AR) $(ARFLAGS) $< -o $@

clean:
	rm -f libsyscall.o libsyscall.a libsyscall.d

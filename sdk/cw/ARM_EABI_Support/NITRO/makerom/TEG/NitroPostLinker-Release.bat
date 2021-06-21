set ARM9_SECTION_NAME="main"
set ARM7_SECTION_NAME="%NITROSDK_ROOT%/components/ferret/ARM7-TEG/Release/ferret_sub.nef"
set SPEC_FILENAME="%~n1.rsf"

copy "%NITROSDK_ROOT%\include\nitro\specfiles\ROM-TEG.rsf" %SPEC_FILENAME%
"%NITROSDK_ROOT%\tools\bin\makerom.exe" -DMAKEROM_ROMROOT=./files -DMAKEROM_ROMFILES= -DPROC=ARM9 -DPLATFORM=TEG -DCODEGEN= -DBUILD=Release -DMAKEROM_ARM9=%ARM9_SECTION_NAME% -DMAKEROM_ARM7=%ARM7_SECTION_NAME% %SPEC_FILENAME%

@if not errorlevel 1 goto end
@echo cannot make binary file
@PAUSE
:end

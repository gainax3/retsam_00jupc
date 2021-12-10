"%NITROSDK_ROOT%\tools\bin\makerom.exe" -DMAKEROM_ROMROOT=./files -DMAKEROM_ROMFILES= -DMAKEROM_ROMSPEED=UNDEFINED -DPROC=ARM9 -DPLATFORM=TS -DCODEGEN= -DBUILD=Debug -DMAKEROM_ARM9=%1 -DMAKEROM_ARM7="%NITROSDK_ROOT%/components/mongoose/ARM7-TS/Debug/mongoose_sub.nef" "%NITROSDK_ROOT%/include/nitro/specfiles/ROM-TS.rsf" %~p1%~n1.srl
@if not errorlevel 1 goto end
@echo バイナリが作成できませんでした。
@PAUSE
:end

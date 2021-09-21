"%CWFOLDER_NITRO%\ARM_Tools\Command_Line_Tools\elftobin" %1 "%NITROSDK_ROOT%\components\ferret\ARM7-TEG\Debug\ferret.nef" "%NITROSDK_ROOT%\tools\elftobin\romHeader.bin"
@if not errorlevel 1 goto end
@echo バイナリが作成できませんでした。
@PAUSE
:end

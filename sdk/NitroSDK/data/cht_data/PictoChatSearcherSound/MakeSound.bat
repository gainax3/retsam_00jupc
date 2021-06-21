
"%NITROSYSTEM_ROOT%/tools/win/bin/sndarc.exe" sound_data.sarc

@if errorlevel 1 goto err

call "%NITROSYSTEM_ROOT%/tools/win/bin/MakeSoundPlayer.bat" sound_data.sdat

@if errorlevel 1 goto err

"%CWFOLDER_NITRO%/ARM_Tools/Command_Line_Tools/BinTOELF.exe" sound_data.sdat -output sound_data.o -section .rodata -symbolname sound_data_sdat
@if errorlevel 1 goto err

@goto ok

:ok
   @echo =======================
   @echo  Convert is SUCCESS !!!
   @echo =======================
   @goto end

:err
   @pause

:end


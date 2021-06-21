@echo off

if "%1" == "" goto help

if not defined NITRO_PLATFORM set NITRO_PLATFORM=TS

if defined NITRO_DEBUG ( set NITRO_BUILD_DIR=Debug 
) else ( 
if defined NITRO_FINALROM ( set NITRO_BUILD_DIR=Rom 
) else ( 
set NITRO_BUILD_DIR=Release ) )

"%NITROSDK_ROOT%/tools/bin/makerom.exe" -DSDAT_FILE=%1 -DNITROSYSTEM_ROOT=%NITROSYSTEM_ROOT% -DNITRO_PLATFORM=%NITRO_PLATFORM% -DNITRO_BUILD_DIR=%NITRO_BUILD_DIR% %NITROSYSTEM_ROOT%/tools/nitro/SoundPlayer/SoundPlayer.rsf %~dp1SoundPlayer.srl

if errorlevel 1 exit 1

goto end

:help
  @echo MakeSoundPlayer
  @echo Copyright 2004 Nintendo. All rights reserved.
  @echo.
  @echo Usage:
  @echo   MakeSoundPlayer.bat sdat-file
  @goto end

:end


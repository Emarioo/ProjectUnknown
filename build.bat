@echo off

@REM @setlocal enabledelayedexpansion

@REM call _build.bat
@REM call .\scripts\build_all.bat
scripts\build_all.bat

rem vcvars64.bat
rem set opts=-FC -GR- -EHa- -nologo -Zi
rem set code=%cd%
rem pushd bin
rem cl %opts% %code%\Win64 -Fegame.exe
rem popd
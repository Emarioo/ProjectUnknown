@echo off
@setlocal enabledelayedexpansion

@REM #############################
@REM  Run vcvars64.bat before running this script. Scripts needs cl, lib, and link (you can manually setup environment variables to Visual Studio tool chain).
@REM  Make sure you have MinGW for g++.
@REM #####################################

@REM python build.py clean
@REM goto END

if not "%~1"=="run" (
    py build.py
    @REM py build.py use_optimizations=true
    @REM py build.py use_tracy=true
) else (
    bin\game
)
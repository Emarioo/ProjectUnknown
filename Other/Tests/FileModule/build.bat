@REM @echo off

@setlocal enabledelayedexpansion

@SET includes= Utilities/Typedefs.h
@SET includes=!includes: = -include ../../Engone/include/Engone/!

@SET files= PlatformModule/Win32.cpp
@SET files=!files: = ../../Engone/src/Engone/!

@REM g++ -c !files! -I../../Engone/include

@SET files= Utilities/Error.cpp Utilities/Alloc.cpp
@REM   LogModule/Logger.cpp
@REM  PlatformModule/Win32.cpp
@REM FileModule/FileWriter.cpp
@SET files=!files: = ../../Engone/src/Engone/!

@REM g++ -g -o app.exe  main.cpp
@REM g++ -o app.exe !includes! -I../../Engone/include main.cpp !files! Win32.o
g++ -o app.exe !includes! -I../../Engone/include main.cpp Win32.o !files!

@REM if %errorlevel% == 0 (
@REM     app
@REM )
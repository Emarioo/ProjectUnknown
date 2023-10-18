@REM @echo off

@setlocal enabledelayedexpansion

@SET includes= Utilities/Typedefs.h

@SET files= PlatformModule/Win32.cpp Utilities/Alloc.cpp FileModule/FileReader.cpp Utilities/Error.cpp


@SET includes=!includes: = -include ../Engone/include/Engone/!
@SET files=!files: = ../Engone/src/Engone/!

g++ -g -o app.exe !includes! -I../Engone/include main.cpp !files!

@REM if %errorlevel% == 0 (
@REM     app
@REM )


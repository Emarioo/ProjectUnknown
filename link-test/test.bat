@ECHO OFF
@setlocal enabledelayedexpansion

SET NL=^


@REM Setup MSVC cl environment
SET vcvars="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
call %vcvars%

@REM -----   Variables   -------
SET defines=/DENGONE_PHYSICS /DENGONE_LOGGER /DENGONE_TRACKER
SET includes=/FI Engone/pch.h
SET includedirs=/IEngone/include /Ilibs/GLFW/include /Ilibs/GLEW/include /Ilibs/reactphysics3d/include
SET objdir=bin\Engone2

@REM -----   Find Files    ------
@REM SET objfiles=
SET files=

for /r %%i in (*.cpp) do (
    SET file=%%i
    
    @REM Blacklist/Whitelist files
    @REM if "x!file:old=!"=="x!file!" if "x!file:vendor=!"=="x!file!" if "x!file:FileModule=!"=="x!file!" if not "x!file:Logger=!"=="x!file!" (
    if "x!file:old=!"=="x!file!" if "x!file:_=!"=="x!file!" if not "x!file:Engone=!"=="x!file!" (
    @REM if "x!file:old=!"=="x!file!" if "x!file:vendor=!"=="x!file!" if "x!file:FileModule=!"=="x!file!" if not "x!file:Engone=!"=="x!file!" (
        SET files=!files!#include ^"!file!^"^%NL%


        @REM call :sub !file!
    )
)

SET files=!files:\=/!

@REM echo !files! > master.cpp

@REM goto SKIPPED
@REM :sub

@REM SET objfile=bin/garb/%~nx1.obj
@REM SET objfile=!objdir!\%~nx1.obj
@REM SET objfile=!objfile:.cpp=!
@REM echo !objfile!

@REM XCOPY is used to detect file changes. findstr is used to detect whether XCOPY did detect changes or not. cl compiles the file. 2> NUL to make it quiet
@REM ECHO F | XCOPY "!file!" "!objfile!" /D /q /L /Y | findstr /B /C:"1 " && cl /c /std:c++17 /TP /EHsc /MTd %includes% !file! %defines% %includedirs% /Fo!objfile!

@REM SET objfiles=!objfiles! !objfile!

@REM exit /b
@REM :SKIPPED

@REM @REM -- links {"Ws2_32.lib","gdi32.lib","user32.lib","shell32.lib","opengl32.lib","winmm.lib","glew64s","glfw364.lib","reactphysics3d.lib"
@REM @REM SET libs=reactphysics3d.lib glew64s.lib glfw364.lib

@REM SET libs=Ws2_32.lib gdi32.lib user32.lib shell32.lib opengl32.lib winmm.lib glew64s.lib glfw364.lib reactphysics3d.lib
@REM SET libdirs=/LIBPATH:../ReactPhysics/build/Debug /LIBPATH:libs/GLFW/lib /LIBPATH:libs/GLEW/lib

@REM cl /c /std:c++17 /TP /EHsc /MTd %includes% !file! %defines% %includedirs% /Fo!objfile!

cl /c /std:c++17 /TP /EHsc /MTd %includes% %defines% %includedirs% master.cpp /Fomaster.o
@REM lib master.o
lib master.o  /OUT:bin/Engone.lib

@REM @REM echo y | del bin\Engone\*

@endlocal
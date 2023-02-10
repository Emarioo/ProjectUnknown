@ECHO OFF
@setlocal enabledelayedexpansion

@REM Setup MSVC cl environment
SET vcvars="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
call %vcvars%

@REM -----   Variables   -------
SET defines=/DENGONE_PHYSICS /DENGONE_LOGGER /DENGONE_TRACKER
SET includes=/FI Engone/pch.h
SET includedirs=/IEngone/include /Ilibs/GLFW/include /Ilibs/GLEW/include /Ilibs/reactphysics3d/include
SET objdir=bin\Engone

mkdir !objdir! 2> NUL

@REM -----   Find Files    ------
SET objfiles=
for /r %%i in (*.cpp) do (
    SET file=%%i
    
    @REM Blacklist/Whitelist files
    @REM if "x!file:old=!"=="x!file!" if "x!file:vendor=!"=="x!file!" if "x!file:FileModule=!"=="x!file!" if not "x!file:Logger=!"=="x!file!" (
    if "x!file:old=!"=="x!file!" if "x!file:vendor=!"=="x!file!" if "x!file:FileModule=!"=="x!file!" if not "x!file:Engone=!"=="x!file!" (
        
        call :sub !file!
    )
)

goto SKIPPED
:sub

@REM SET objfile=bin/garb/%~nx1.obj
SET objfile=!objdir!\%~nx1.obj
SET objfile=!objfile:.cpp=!
echo !objfile!

@REM XCOPY is used to detect file changes. findstr is used to detect whether XCOPY did detect changes or not. cl compiles the file. 2> NUL to make it quiet
ECHO F | XCOPY "!file!" "!objfile!" /D /q /L /Y | findstr /B /C:"1 " && cl /c /std:c++17 /TP /EHsc /MTd %includes% !file! %defines% %includedirs% /Fo!objfile!

SET objfiles=!objfiles! !objfile!

exit /b
:SKIPPED

@REM -- links {"Ws2_32.lib","gdi32.lib","user32.lib","shell32.lib","opengl32.lib","winmm.lib","glew64s","glfw364.lib","reactphysics3d.lib"
@REM SET libs=reactphysics3d.lib glew64s.lib glfw364.lib

SET libs=Ws2_32.lib gdi32.lib user32.lib shell32.lib opengl32.lib winmm.lib glew64s.lib glfw364.lib reactphysics3d.lib
SET libdirs=/LIBPATH:../ReactPhysics/build/Debug /LIBPATH:libs/GLFW/lib /LIBPATH:libs/GLEW/lib
lib !objfiles! /OUT:bin/Engone.lib

@REM echo y | del bin\Engone\*

@endlocal
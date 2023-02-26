@echo OFF

@REM call gen_engone.bat
@REM call gen_prounk.bat

@REM gcc main.c -o app.exe

premake5 build

bin\ProjectUnknown\Debug\ProjectUnknown.exe

@REM @ECHO OFF
@REM @setlocal enabledelayedexpansion

@REM @REM Setup MSVC cl environment
@REM SET vcvars="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
@REM call %vcvars%

@REM @REM -----   Variables   -------
@REM SET defines=/DENGONE_PHYSICS /DENGONE_LOGGER /DENGONE_TRACKER
@REM SET includes=/FI Engone/pch.h
@REM SET includedirs=/IEngone/include /Ilibs/GLFW/include /Ilibs/GLEW/include /Ilibs/reactphysics3d/include /IProjectUnknown/include 

@REM @REM -----   Find Files    ------
@REM SET cppfiles=
@REM SET objfiles=
@REM @REM Aquire files
@REM for /r %%i in (*.cpp) do (
@REM     SET file=%%i
    
@REM     @REM Blacklist/Whitelist files
@REM     if "x!file:old=!"=="x!file!" if "x!file:vendor=!"=="x!file!" if "x!file:FileModule=!"=="x!file!" if not "x!file:ProUnk=!"=="x!file!" (
@REM     @REM if "x!file:old=!"=="x!file!" if "x!file:vendor=!"=="x!file!" if "x!file:FileModule=!"=="x!file!" if not "x!file:Engone=!"=="x!file!" (
        
@REM         call :sub !file!
@REM         SET cppfiles=!cppfiles! !file!
@REM     )
@REM )

@REM goto SKIPPED
@REM :sub

@REM SET objfile=bin/garb/%~nx1.obj
@REM SET objfile=!objfile:.cpp=!

@REM @REM XCOPY is used to detect file changes. findstr is used to detect whether XCOPY did detect changes or not. cl compiles the file
@REM ECHO F | XCOPY "!file!" "!objfile!" /D /q /L /Y /i | findstr /B /C:"1 " && cl /c /std:c++17 /TP /EHsc /MTd %includes% !file! %defines% %includedirs% /Fo!objfile!

@REM SET objfiles=!objfiles! !objfile!

@REM exit /b
@REM :SKIPPED

@REM @REM -- links {"Ws2_32.lib","gdi32.lib","user32.lib","shell32.lib","opengl32.lib","winmm.lib","glew64s","glfw364.lib","reactphysics3d.lib"
@REM @REM SET libs=reactphysics3d.lib glew64s.lib glfw364.lib

@REM @REM SET libs=Ws2_32.lib gdi32.lib user32.lib shell32.lib opengl32.lib winmm.lib glew64s.lib glfw364.lib reactphysics3d.lib
@REM @REM SET libdirs=/LIBPATH:../ReactPhysics/build/Debug /LIBPATH:libs/GLFW/lib /LIBPATH:libs/GLEW/lib
@REM @REM lib !objfiles! /OUT:bin/Engone.lib

@REM @REM .obj needs to be cleaned if you change includes or defines. Not libs though. .lib is only linked during creation of Engone.lib or .exe
@REM @REM echo y | del bin\garb\*

@REM @REM You seem to need

@REM SET libs=Engone.lib Ws2_32.lib gdi32.lib user32.lib shell32.lib opengl32.lib winmm.lib glew64s.lib glfw364.lib reactphysics3d.lib
@REM SET libdirs=/LIBPATH:bin /LIBPATH:../ReactPhysics/build/Debug /LIBPATH:libs/GLFW/lib /LIBPATH:libs/GLEW/lib
@REM link !objfiles! %libdirs% %libs% /OUT:bin/ProjectUnknown.exe

@REM @REM cl /std:c++17 /TP /EHsc /MTd /FI Engone/pch.h /Fe:bin/engone.exe !files! %defines% %includes% /Fobin/garb/ /link %libdirs% %libs%
@REM @REM /EHsc something with exceptions. compiler complained.

@REM @REM Todo: Precompiled Headers
@REM @REM Todo: Per file object compilation with the resulting objects being linked into a .lib

@REM endlocal
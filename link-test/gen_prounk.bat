@ECHO OFF
@setlocal enabledelayedexpansion

@REM Setup MSVC cl environment
SET vcvars="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
call %vcvars%

@REM -----   Variables   -------
SET defines=/DENGONE_PHYSICS /DENGONE_LOGGER /DENGONE_TRACKER
SET includes=/FI Engone/pch.h
SET includedirs=/IEngone/include /Ilibs/GLFW/include /Ilibs/GLEW/include /Ilibs/reactphysics3d/include /IProjectUnknown/include 
SET objdir=bin\ProUnk\
mkdir !objdir! 2> NUL

@REM -----   Find Files    ------
SET objfiles=
for /r %%i in (*.cpp) do (
    SET file=%%i
    
    @REM Blacklist/Whitelist files
    if "x!file:old=!"=="x!file!" if "x!file:vendor=!"=="x!file!" if "x!file:FileModule=!"=="x!file!" if not "x!file:ProUnk=!"=="x!file!" (
        
        call :sub !file!
    )
)

goto SKIPPED
:sub

SET objfile=!objdir!%~nx1.obj
SET objfile=!objfile:.cpp=!

@REM XCOPY is used to detect file changes. findstr is used to detect whether XCOPY did detect changes or not. cl compiles the file
ECHO F | XCOPY "!file!" "!objfile!" /D /q /L /Y /i | findstr /B /C:"1 " && cl /c /std:c++17 /TP /EHsc /MTd %includes% !file! %defines% %includedirs% /Fo!objfile!

SET objfiles=!objfiles! !objfile!

exit /b
:SKIPPED

SET libs=Engone.lib Ws2_32.lib gdi32.lib user32.lib shell32.lib opengl32.lib winmm.lib glew64s.lib glfw364.lib reactphysics3d.lib
SET libdirs=/LIBPATH:bin /LIBPATH:../ReactPhysics/build/Debug /LIBPATH:libs/GLFW/lib /LIBPATH:libs/GLEW/lib

link !objfiles! %libdirs% %libs% /OUT:bin/ProjectUnknown.exe

@REM echo y | del bin\ProUnk\*

endlocal
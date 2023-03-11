@ECHO OFF
@setlocal enabledelayedexpansion

set /a startTime=360000*(100%time:~0,2% %% 100)+6000*(100%time:~3,2% %% 100)+100*(100%time:~6,2% %% 100)+(100%time:~9,2% %% 100)
@REM echo ENE: %time:~0,2% -- %time:~3,2% -- %time:~6,2% -- %time:~9,2% -- %startTime%
SET NL=^


call vcvars64.bat

@REM -----   Variables   -------
SET defines=/DENGONE_PHYSICS /DENGONE_LOGGER /DENGONE_TRACKER /DENGONE_OPENGL /DGLEW_STATIC /DWIN32
SET includes=/FI Engone/pch.h
SET includedirs=/IEngone/include /Ilibs/GLFW/include /Ilibs/GLEW/include /Ilibs/reactphysics3d/include /IEngone\include\Engone\vendor

@REM -----   Find Files    ------
@REM SET objfiles=
SET files=

for /r %%i in (*.cpp) do (
    SET file=%%i
    
    if "x!file:old=!"=="x!file!" if "x!file:__=!"=="x!file!" if "x!file:bin=!"=="x!file!" if not "x!file:Engone=!"=="x!file!" if not "x!file:.cpp=!"=="x!file!" (
        SET files=!files!#include ^"!file!^"^%NL%
        

    )
)
SET srcfile=bin/intermediates/Engone/Debug-MSVC/all_engone.cpp

SET files=!files:\=/!

echo !files! > %srcfile%


@REM XCOPY is used to detect file changes. findstr is used to detect whether XCOPY did detect changes or not. cl compiles the file. 2> NUL to make it quiet
@REM ECHO F | XCOPY "!file!" "!objfile!" /D /q /L /Y | findstr /B /C:"1 " && cl /c /std:c++17 /TP /EHsc /MTd %includes% !file! %defines% %includedirs% /Fo!objfile!

@REM SET objfiles=!objfiles! !objfile!

@REM @REM -- links {"Ws2_32.lib","gdi32.lib","user32.lib","shell32.lib","opengl32.lib","winmm.lib","glew64s","glfw364.lib","reactphysics3d.lib"
@REM @REM SET libs=reactphysics3d.lib glew64s.lib glfw364.lib

@REM SET libs=Ws2_32.lib gdi32.lib user32.lib shell32.lib opengl32.lib winmm.lib glew64s.lib glfw364.lib reactphysics3d.lib
@REM SET libdirs=/LIBPATH:../ReactPhysics/build/Debug /LIBPATH:libs/GLFW/lib /LIBPATH:libs/GLEW/lib

@REM cl /c /std:c++17 /TP /EHsc /MTd %includes% !file! %defines% %includedirs% /Fo!objfile!

SET libs=rp3d.lib glew.lib glfw3.lib opengl32.lib Ws2_32.lib shell32.lib
@REM winmm.lib gdi32.lib
@REM user32.lib 
SET libdirs=/LIBPATH:bin/windows-Debug-MSVC

SET objdir=bin\intermediates\Engone\Debug-MSVC

mkdir %objdir% 2> nul

SET objfile=%objdir%\all_engone.o

cl /c /DEBUG /std:c++17 /TP /EHsc /Z7 /MTd %includes% %defines% %includedirs% %srcfile% /Fo%objfile%
mkdir bin\Engone\Debug-MSVC 2> nul
lib %objfile% %libs% %libdirs% /OUT:bin/Engone/Debug-MSVC/Engone.lib /IGNORE:4006

set /a endTime=360000*(100%time:~0,2% %% 100)+6000*(100%time:~3,2% %% 100)+100*(100%time:~6,2% %% 100)+(100%time:~9,2% %% 100)
@REM echo ENE: %time:~0,2% -- %time:~3,2% -- %time:~6,2% -- %time:~9,2% -- %endTime%
set /a finS=(endTime-startTime)/100
set /a finS2=(endTime-startTime)%%100

echo Finished in %finS%.%finS2% seconds

@endlocal
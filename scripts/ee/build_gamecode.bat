@ECHO OFF
@setlocal enabledelayedexpansion


set /a startTime=360000*(100%time:~0,2% %% 100)+6000*(100%time:~3,2% %% 100)+100*(100%time:~6,2% %% 100)+(100%time:~9,2% %% 100)
@REM set /a startTime=360000*%time:~0,2%+6000*%time:~3,2%+100*%time:~6,2%+%time:~9,2%
@REM echo STAR: %time:~0,2% -- %time:~3,2% -- %time:~6,2% -- %time:~9,2% -- %startTime%


SET NL=^


@REM call vcvars64.bat

@REM -----   Variables   -------
SET defines=/DENGONE_PHYSICS /DENGONE_LOGGER /DENGONE_TRACKER
SET includes=/FI Engone/pch.h
SET includedirs=/IGameCode/include /IProjectUnknown/include /IEngone/include /Ilibs/GLFW/include /Ilibs/GLEW/include /Ilibs/reactphysics3d/include
@REM  /IEngone\include\Engone\vendor

@REM -----   Find Files    ------
@REM SET objfiles=
SET files=

for /r %%i in (*.cpp) do (
    SET file=%%i
    
    if "x!file:old=!"=="x!file!" if "x!file:__=!"=="x!file!" if "x!file:bin\=!"=="x!file!" if not "x!file:GameCode=!"=="x!file!" if not "x!file:.cpp=!"=="x!file!" (
        SET files=!files!#include ^"!file!^"^%NL%


    )
)
SET objdir=bin\intermediates\GameCode\Debug-MSVC
mkdir %objdir% 2> nul

SET srcfile=bin/intermediates/GameCode/Debug-MSVC/all_gamecode.cpp

SET files=!files:\=/!

echo !files! > %srcfile%


@REM XCOPY is used to detect file changes. findstr is used to detect whether XCOPY did detect changes or not. cl compiles the file. 2> NUL to make it quiet
@REM ECHO F | XCOPY "!file!" "!objfile!" /D /q /L /Y | findstr /B /C:"1 " && cl /c /std:c++17 /TP /EHsc /MTd %includes% !file! %defines% %includedirs% /Fo!objfile!

@REM SET objfiles=!objfiles! !objfile!

@REM @REM -- links {"Ws2_32.lib","gdi32.lib","user32.lib","shell32.lib","opengl32.lib","winmm.lib","glew64s","glfw364.lib","reactphysics3d.lib"
@REM @REM SET libs=reactphysics3d.lib glew64s.lib glfw364.lib

SET libs=Engone.lib ProjectUnknown.lib
@REM Ws2_32.lib gdi32.lib user32.lib shell32.lib opengl32.lib winmm.lib rp3d.lib glew.lib glfw3.lib engone.lib
SET libdirs=/LIBPATH:bin/Engone/Debug-MSVC /LIBPATH:bin/ProUnk/Debug-MSVC
@REM /LIBPATH:bin/windows-Debug-MSVC

@REM cl /c /std:c++17 /TP /EHsc /MTd %includes% !file! %defines% %includedirs% /Fo!objfile!

SET objfile=%objdir%\all_gamecode.o

cl /c /DEBUG /std:c++17 /TP /EHsc /Z7 /MTd %includes% %defines% %includedirs% %srcfile% /Fo%objfile%
@REM lib %objfile%  /OUT:bin/ProUnk/Debug-MSVC/ProUnk.exe
mkdir bin\GameCode\Debug-MSVC 2> nul
link !objfile! %libdirs% %libs% /DLL /DEBUG /IGNORE:4006 /PDBALTPATH:bin/GameCode/Debug-MSVC/GameCode_.pdb /OUT:bin/GameCode/Debug-MSVC/GameCode.dll


set /a endTime=360000*(100%time:~0,2% %% 100)+6000*(100%time:~3,2% %% 100)+100*(100%time:~6,2% %% 100)+(100%time:~9,2% %% 100)
@REM set /a endTime=360000*%time:~0,2%+6000*%time:~3,2%+100*%time:~6,2%+%time:~9,2%

@REM echo HM: %time:~0,2% -- %time:~3,2% -- %time:~6,2% -- %time:~9,2% -- %endTime% -- %startTime%

set /a finS=(endTime-startTime)/100
set /a finS2=(endTime-startTime)%%100

echo Finished in %finS%.%finS2% seconds

@endlocal
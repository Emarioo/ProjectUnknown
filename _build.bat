@echo OFF
@setlocal enabledelayedexpansion

rem Hours and minutes has not been tested, formatting/conversion could be wrong

@REM set /a startTime=360000*%time:~0,2%+6000*%time:~3,2%+100*%time:~6,2%+%time:~9,2%

set /a startTime=360000* ( 100%time:~0,2% %% 100 ) + 6000*( 100%time:~3,2% %% 100 ) + 100* ( 100%time:~6,2% %% 100 ) + ( 100%time:~9,2% %% 100 )
@REM set /a startTime= 100%time:~9,2% % 100
call vcvars64.bat

goto PIPELINED

rem Improve by compiling src files for each project with start
rem then call linker linearly

rem Call vcvars once and not in each build_proj file.

rem saves about 3-4 seconds, is reliant on specific timings which may need to be tweaked.

:PIPELINED
start scripts\build_engone.bat
timeout /T 3
start scripts\build_prounk.bat
timeout /T 3
call scripts\build_gamecode.bat
goto END

:SINGLE
call scripts\build_engone.bat
call scripts\build_prounk.bat
call scripts\build_gamecode.bat

:END

set /a endTime=360000*(100%time:~0,2% %% 100 )+6000*(100%time:~3,2% %% 100 )+100*(100%time:~6,2% %% 100 )+(100%time:~9,2% %% 100 )

@REM set /a endTime=360000*%time:~0,2%+6000*%time:~3,2%+100*%time:~6,2%+%time:~9,2%
set /a finS=(endTime-startTime)/100
set /a finS2=(endTime-startTime)%%100

echo TOTAL Finished in %finS%.%finS2% seconds


@REM bin\ProUnk\Debug-MSVC\ProjectUnknown.exe

@REM call vcvars64.bat

@REM dumpbin.exe /ALL bin\GameCode\Debug-MSVC\GameCode.dll > symbols.txt
@REM dumpbin.exe /ALL bin\Engone\Debug-MSVC\Engone.lib

@REM premake5 build


@REM @ECHO OFF


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
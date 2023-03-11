@echo off
@setlocal enabledelayedexpansion

@REM SET UNITY_BUILD=1
SET LIVE_EDITING=1
SET AUTO_LAUNCH=1

@REM Live edit and unity build don't work together. They could, I just haven't fixed it.
@REM If game is running and not live editing kill the game so it doens't block linking?

SET e_defines=/DENGONE_PHYSICS /DENGONE_LOGGER /DENGONE_TRACKER /DENGONE_OPENGL /DGLEW_STATIC /DWIN32
SET p_defines=/DENGONE_PHYSICS /DENGONE_LOGGER /DENGONE_TRACKER /DENGONE_OPENGL /DGLEW_STATIC /DWIN32
SET g_defines=/DENGONE_PHYSICS /DENGONE_LOGGER /DENGONE_TRACKER /DENGONE_OPENGL /DGLEW_STATIC /DWIN32

SET e_includes=/FI Engone/pch.h
SET p_includes=/FI ProUnk/pch.h
SET g_includes=/FI Engone/pch.h

SET e_includedirs=/IEngone/include /Ilibs/GLFW/include /Ilibs/GLEW/include /Ilibs/reactphysics3d/include /IEngone\include\Engone\vendor
SET p_includedirs=/IEngone/include /Ilibs/GLFW/include /Ilibs/GLEW/include /Ilibs/reactphysics3d/include /IProjectUnknown/include /IGameCode/include /IEngone\include\Engone\vendor
SET g_includedirs=/IEngone/include /Ilibs/GLFW/include /Ilibs/GLEW/include /Ilibs/reactphysics3d/include /IProjectUnknown/include /IGameCode/include

SET objdir=bin\intermediates

SET COMPILE_OPTIONS=/DEBUG /std:c++14 /EHsc /TP /Z7 /MTd /nologo
SET LINK_OPTIONS=/IGNORE:4006 /NOLOGO /DEBUG
SET LIB_OPTIONS=/IGNORE:4006 /NOLOGO

set /a startTime=6000*( 100%time:~3,2% %% 100 ) + 100* ( 100%time:~6,2% %% 100 ) + ( 100%time:~9,2% %% 100 )

@REM echo %PATH%
if "x%PATH:VC\Tools\MSVC=%"=="x%PATH%" (
    call vcvars64.bat
    echo Run vcvars64.bat in the command line once to keep the VS environment variables before building
)

rem #########  GATHER FILES  ###############

mkdir %objdir% 2> nul

SET e_srcfile=!objdir!\all_engone.cpp
SET p_srcfile=!objdir!\all_prounk.cpp
SET g_srcfile=!objdir!\all_gamecode.cpp

type nul > !e_srcfile!
type nul > !p_srcfile!
type nul > !g_srcfile!

for /r %%i in (*.cpp) do (
    SET file=%%i
    
    if "x!file:old=!"=="x!file!" if "x!file:__=!"=="x!file!" if "x!file:bin\=!"=="x!file!" if "x!file:glm=!"=="x!file!" (
        if not "x!file:Engone=!"=="x!file!" (
            echo #include ^"!file:\=/!^">> !e_srcfile!
        ) else if not "x!file:ProUnk=!"=="x!file!" (
            echo #include ^"!file:\=/!^">> !p_srcfile!
        ) else if not "x!file:GameCode=!"=="x!file!" (
            if !LIVE_EDITING! == 1 (
                echo #include ^"!file:\=/!^">> !g_srcfile!
            ) else ( 
                echo #include ^"!file:\=/!^">> !p_srcfile!
            )
        )
    )
)

rem ###############  COMPILE  #################

set /a c_startTime=6000*( 100%time:~3,2% %% 100 ) + 100* ( 100%time:~6,2% %% 100 ) + ( 100%time:~9,2% %% 100 )

if !LIVE_EDITING! == 1 (
    SET e_defines=!e_defines! /DLIVE_EDITING
    SET p_defines=!p_defines! /DLIVE_EDITING
    SET g_defines=!g_defines! /DLIVE_EDITING
    
    tasklist /fi "IMAGENAME eq game.exe" | find /I "game.exe" && if %errorlevel% == 0 (
        echo Game is on!
        SET ONLY_GAME_CODE=1
    )
)

if !ONLY_GAME_CODE! == 1 (
    @rem Note that Engone isn't compiled since prounk itself uses it and different version on gamecode and prounk is probably bad.
    cl /c !COMPILE_OPTIONS! !g_includes! !g_defines! !g_includedirs! !objdir!\all_gamecode.cpp /Fo!objdir!\all_gamecode.o
) else (
    start /b cl /c !COMPILE_OPTIONS! !p_includes! !p_defines! !p_includedirs! !objdir!\all_prounk.cpp /Fo!objdir!\all_prounk.o

    if !LIVE_EDITING! == 1 (
        start /b cl /c !COMPILE_OPTIONS! !g_includes! !g_defines! !g_includedirs! !objdir!\all_gamecode.cpp /Fo!objdir!\all_gamecode.o
    )
    cl /c !COMPILE_OPTIONS! !e_includes! !e_defines! !e_includedirs! !objdir!\all_engone.cpp /Fo!objdir!\all_engone.o
)
@REM if not %errorlevel% == 0 ( exit )

set /a c_endTime=6000*(100%time:~3,2% %% 100 )+100*(100%time:~6,2% %% 100 )+(100%time:~9,2% %% 100 )

rem #########  LINKING  ###############

SET libdirs=/LIBPATH:bin/Debug-MSVC /LIBPATH:bin/Engone/Debug-MSVC /LIBPATH:bin/ProUnk/Debug-MSVC

@REM The other libraries link with these so we don't need to
@REM    winmm.lib gdi32.lib user32.lib 
SET e_libs=rp3d.lib glew.lib glfw3.lib opengl32.lib Ws2_32.lib shell32.lib
SET p_libs=Engone.lib
SET g_libs=Engone.lib ProjectUnknown.lib

if !UNITY_BUILD! == 1 (
    SET p_libs=!e_libs!
)

mkdir bin\Engone\Debug-MSVC 2> nul
mkdir bin\ProUnk\Debug-MSVC 2> nul
mkdir bin\GameCode\Debug-MSVC 2> nul

rem Todo: link with optimizations or debug info?

set /a l_startTime=6000*( 100%time:~3,2% %% 100 ) + 100* ( 100%time:~6,2% %% 100 ) + ( 100%time:~9,2% %% 100 )

if !ONLY_GAME_CODE! == 1 (
    link !objdir!\all_gamecode.o !libdirs! !g_libs! /DLL !LINK_OPTIONS! /PDBALTPATH:bin/GameCode/Debug-MSVC/GameCode_.pdb /OUT:bin/GameCode/Debug-MSVC/GameCode.dll   
) else (

if !UNITY_BUILD! == 1 (
    @REM echo !p_libs! !libdirs!
    @REM Link Engone.lib in case another project needs the updated version
    start /b lib %objdir%\all_engone.o !libdirs! !e_libs! !LIB_OPTIONS! /OUT:bin/Engone/Debug-MSVC/Engone.lib > nul

    link !objdir!\all_prounk.o !objdir!\all_engone.o !libdirs! !p_libs! !LINK_OPTIONS! /SUBSYSTEM:CONSOLE /OUT:bin/game.exe
) else (
    lib %objdir%\all_engone.o !libdirs! !e_libs! !LIB_OPTIONS! /OUT:bin/Engone/Debug-MSVC/Engone.lib

    if !LIVE_EDITING! == 1 (
        start /b lib !objdir!\all_prounk.o !libdirs! !p_libs! !LIB_OPTIONS! /OUT:bin/ProUnk/Debug-MSVC/ProjectUnknown.lib
        link !objdir!\all_prounk.o !libdirs! !p_libs! !LINK_OPTIONS! /SUBSYSTEM:CONSOLE /OUT:bin/game.exe
        start /b link !objdir!\all_gamecode.o !libdirs! !g_libs! /DLL !LINK_OPTIONS! /PDBALTPATH:bin/GameCode/Debug-MSVC/GameCode_.pdb /OUT:bin/GameCode/Debug-MSVC/GameCode.dll
    ) else (
        link !objdir!\all_prounk.o !libdirs! !p_libs! !LINK_OPTIONS! /SUBSYSTEM:CONSOLE /OUT:bin/game.exe
    )
)
)
@REM if not %errorlevel% == 0 ( exit )

set /a l_endTime=6000*(100%time:~3,2% %% 100 )+100*(100%time:~6,2% %% 100 )+(100%time:~9,2% %% 100 )

set /a endTime=6000*(100%time:~3,2% %% 100 )+100*(100%time:~6,2% %% 100 )+(100%time:~9,2% %% 100 )

set /a c_finS=(c_endTime-c_startTime)/100
set /a c_finS2=(c_endTime-c_startTime)%%100
set /a l_finS=(l_endTime-l_startTime)/100
set /a l_finS2=(l_endTime-l_startTime)%%100
set /a finS=(endTime-startTime)/100
set /a finS2=(endTime-startTime)%%100

echo Compilation in %c_finS%.%c_finS2% seconds
echo Linking in %l_finS%.%l_finS2% seconds
echo Finished in %finS%.%finS2% seconds

if !AUTO_LAUNCH! == 1 if !errorlevel! == 0 if not !ONLY_GAME_CODE! == 1 (
    pushd bin
    start game.exe
    popd
)

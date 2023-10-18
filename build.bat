@echo off
@setlocal enabledelayedexpansion

@REM SET LIVE_EDITING=1
SET AUTO_LAUNCH=1

@REM TODO: If game is running when building, kill it and launch the newly compiled game.

SET PLATFORM=OS_WINDOWS

SET DEFINES=/DENGONE_PHYSICS /DENGONE_LOGGER /DENGONE_TRACKER /DENGONE_OPENGL /D!PLATFORM!

if !LIVE_EDITING! == 1 (
    SET defines=!defines! /DGLFW_DLL /DLIVE_EDITING
    SET defines=!defines! /DLIVE_EDITING
    tasklist /fi "IMAGENAME eq game.exe" | find /I "game.exe" && if %errorlevel% == 0 (
        echo Game is on!
        SET ONLY_GAME_CODE=1
    ) 
) else (
    SET defines=!defines! /DGLEW_STATIC
)

SET FORCED_INCLUDES_ENGONE=/FI Engone/pch.h
SET FORCED_INCLUDES_GAME=/FI ProUnk/pch.h

SET INCLUDE_DIRS=/Iinclude /Ilibs/GLFW/include /Ilibs/GLEW/include /Ilibs/reactphysics3d/include /Ilibs/include

SET OBJ_DIR=bin\intermediates

SET COMPILE_OPTIONS=/DEBUG /std:c++14 /EHsc /TP /Z7 /MTd /nologo
SET LINK_OPTIONS=/IGNORE:4006 /DEBUG /NOLOGO /MACHINE:X64
SET LIB_OPTIONS=/IGNORE:4006 /NOLOGO /MACHINE:X64

set /a startTime=6000*( 100%time:~3,2% %% 100 ) + 100* ( 100%time:~6,2% %% 100 ) + ( 100%time:~9,2% %% 100 )

rem #########  GATHER FILES  ###############

mkdir !OBJ_DIR! 2> nul

SET SRC_E=!OBJ_DIR!\all_engone.cpp
SET SRC_P=!OBJ_DIR!\all_prounk.cpp
SET SRC_G=!OBJ_DIR!\all_gamecode.cpp

type nul > !SRC_E!
type nul > !SRC_P!
type nul > !SRC_G!

for /r %%i in (*.cpp) do (
    SET file=%%i
    
    @REM echo !file!
    
    if "x!file:old=!"=="x!file!" if "x!file:__=!"=="x!file!" if "x!file:bin\=!"=="x!file!" if "x!file:glm=!"=="x!file!" (
        if not "x!file:Engone=!"=="x!file!" (
            echo #include ^"!file:\=/!^">> !SRC_E!
        ) else if not "x!file:ProUnk=!"=="x!file!" (
            if !LIVE_EDITING! == 1 (
                if "x!file:main=!"=="x!file!" (
                    echo #include ^"!file:\=/!^">> !SRC_G!
                ) else (
                    echo #include ^"!file:\=/!^">> !SRC_P!
                )
            ) else (
                if not "x!file:ProUnk=!"=="x!file!" (
                    echo #include ^"!file:\=/!^">> !SRC_P!
                )
            )
        )
    )
)
rem ###############  COMPILE  #################

set /a c_startTime=6000*( 100%time:~3,2% %% 100 ) + 100* ( 100%time:~6,2% %% 100 ) + ( 100%time:~9,2% %% 100 )

if !ONLY_GAME_CODE! == 1 (
    @rem Note that Engone isn't compiled since prounk itself uses it and different version on gamecode and prounk is probably bad.
    cl /c !COMPILE_OPTIONS! /INCREMENTAL:NO !FORCED_INCLUDES_GAME! !DEFINES! !INCLUDE_DIRS! !SRC_G! /Fd!OBJ_DIR!\gamecode.pdb /Fo!OBJ_DIR!\all_gamecode.o
    @REM if not %errorlevel% == 0 ( goto FAILED )
) else (
    start /b cl /c !COMPILE_OPTIONS! /INCREMENTAL:NO !FORCED_INCLUDES_GAME! !DEFINES! !INCLUDE_DIRS! !SRC_P! /Fd!OBJ_DIR!\prounk.pdb /Fo!OBJ_DIR!\all_prounk.o

    if !LIVE_EDITING! == 1 (
        start /b cl /c !COMPILE_OPTIONS! /INCREMENTAL:NO !FORCED_INCLUDES_GAME! !DEFINES! !INCLUDE_DIRS! !SRC_G! /Fd!OBJ_DIR!\gamecode.pdb /Fo!OBJ_DIR!\all_gamecode.o
    )
    cl /c !COMPILE_OPTIONS! !FORCED_INCLUDES_ENGONE! /INCREMENTAL:NO !DEFINES! !INCLUDE_DIRS! !SRC_E! /Fd!OBJ_DIR!\engone.pdb /Fo!OBJ_DIR!\all_engone.o
    @REM if not %errorlevel% == 0 ( goto FAILED )
)

set /a c_endTime=6000*(100%time:~3,2% %% 100 )+100*(100%time:~6,2% %% 100 )+(100%time:~9,2% %% 100 )

rem #########  LINKING  ###############

@REM SET LIB_DIRS=/LIBPATH:bin/Debug-MSVC /LIBPATH:bin/Engone/Debug-MSVC /LIBPATH:bin/ProUnk/Debug-MSVC
SET LIB_DIRS=/LIBPATH:libs\GLFW\lib /LIBPATH:libs\GLEW\lib /LIBPATH:libs\reactphysics3d\lib /LIBPATH:bin\Engone\Debug-MSVC /LIBPATH:bin\ProUnk\Debug-MSVC

SET LIBS_ENGONE=rp3d.lib opengl32.lib Ws2_32.lib shell32.lib winmm.lib gdi32.lib user32.lib
if !LIVE_EDITING! == 1 (
    SET LIBS_ENGONE=!LIBS_ENGONE! glew32.lib glfw3dll.lib
) else (
    SET LIBS_ENGONE=!LIBS_ENGONE! glew32s.lib glfw3_mt.lib
)
SET LIBS_GAME=Engone.lib

mkdir bin\Engone\Debug-MSVC 2> nul
mkdir bin\ProUnk\Debug-MSVC 2> nul
mkdir bin\GameCode\Debug-MSVC 2> nul

rem Todo: link with optimizations or debug info?

set /a l_startTime=6000*( 100%time:~3,2% %% 100 ) + 100* ( 100%time:~6,2% %% 100 ) + ( 100%time:~9,2% %% 100 )

if !ONLY_GAME_CODE! == 1 (
:RETRY
    link !OBJ_DIR!\all_gamecode.o !LIB_DIRS! !LIBS_GAME! /DLL !LINK_OPTIONS! /PDBALTPATH:bin/GameCode_.pdb /OUT:bin/GameCode.dll
    if not !errorlevel! == 0 (
        goto RETRY
    )
    
    @REM if not %errorlevel% == 0 ( goto FAILED )
) else (
    @REM lib %OBJ_DIR%\all_engone.o !libdirs! !e_libs! !LIB_OPTIONS! /OUT:bin/Engone/Debug-MSVC/Engone.lib
    @REM errorlevel doesn't work on lib ):
    @REM echo ERROR: %errorlevel%
    @REM if not %errorlevel% == 0 ( goto FAILED )
    
    if !LIVE_EDITING! == 1 (
        lib !OBJ_DIR!\all_engone.o !LIB_DIRS! !LIBS_ENGONE! !LIB_OPTIONS! /OUT:bin/Engone/Debug-MSVC/Engone.lib
        @REM start /b lib !OBJ_DIR!\all_prounk.o !libdirs! !p_libs! !LIB_OPTIONS! /OUT:bin/ProUnk/Debug-MSVC/ProjectUnknown.lib
        @REM start /b 
        link !OBJ_DIR!\all_prounk.o !LIB_DIRS! !LIBS_GAME! !LINK_OPTIONS! /SUBSYSTEM:CONSOLE /OUT:bin/game.exe
        @REM link !OBJ_DIR!\all_gamecode.o !libdirs! !g_libs! /DLL /OUT:bin/GameCode/Debug-MSVC/GameCode.dll
        link !OBJ_DIR!\all_gamecode.o !LIB_DIRS! !LIBS_GAME! !LINK_OPTIONS! /DLL /PDBALTPATH:bin/GameCode_.pdb /OUT:bin/GameCode.dll
        @REM  /PDBALTPATH:bin/GameCode/Debug-MSVC/GameCode_.pdb
        
        echo F | xcopy /y/q/d libs\GLFW\lib\glfw3.dll glfw3.dll > nul
        echo F | xcopy /y/q/d libs\GLEW\lib\glew32.dll glew32.dll > nul
    ) else (
        lib !OBJ_DIR!\all_engone.o !LIB_DIRS! !LIBS_ENGONE! !LIB_OPTIONS! /OUT:bin/Engone/Debug-MSVC/Engone.lib
        link !OBJ_DIR!\all_prounk.o !LIB_DIRS! !LIBS_GAME! !LINK_OPTIONS! /SUBSYSTEM:CONSOLE /OUT:bin/game.exe
    )
    @REM if not %errorlevel% == 0 ( goto FAILED )
    
    @REM )
)

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
    @REM echo F | xcopy /q/y/d bin\game.exe game.exe > nul
    @REM game.exe
    if !LIVE_EDITING! == 1 (
        start bin\game.exe
    ) else (
        @REM bin\game.exe
        bin\game.exe > out
    )
    
    @REM linecounter -w "*.cpp" "*.h" -b .git GLFW3 glew rp3d libs vendor glm --printFiles > out
    
    goto END
)

:FAILED

:END
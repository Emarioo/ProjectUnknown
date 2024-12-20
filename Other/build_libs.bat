@echo off
@setlocal enabledelayedexpansion

set /a startTime=6000 * ( 100%time:~3,2% %% 100 ) + 100 * ( 100%time:~6,2% %% 100 ) + ( 100%time:~9,2% %% 100 )
call vcvars64.bat > nul

rem GATHER FILES

SET objdir=bin\intermediates
mkdir %objdir% 2> nul

SET r_srcfile=!objdir!\all_rp3d.cpp
SET w_srcfile=!objdir!\all_glew.c
SET f_srcfile=!objdir!\all_glfw3.c

type nul > !r_srcfile!
type nul > !w_srcfile!
type nul > !f_srcfile!

@REM set /a a_startTime=6000*( 100%time:~3,2% %% 100 ) + 100* ( 100%time:~6,2% %% 100 ) + ( 100%time:~9,2% %% 100 )
for /r %%i in (*.cpp) do (
    SET file=%%i
    
    if not "x!file:reactphysics3d\src=!"=="x!file!" (
        echo #include ^"!file:\=/!^" >> !r_srcfile!
    )
)
for /r %%i in (*.c) do (
    SET file=%%i
    
    if not "x!file:glew-2.1.0\src\glew.c=!"=="x!file!" (
        echo #include ^"!file:\=/!^" >> !w_srcfile!
    )
    if "x!file:posix=!"=="x!file!" if "x!file:cocoa=!"=="x!file!" if "x!file:nsgl=!"=="x!file!" (
    if "x!file:glx=!"=="x!file!" if "x!file:x11=!"=="x!file!" if "x!file:xkb=!"=="x!file!" (
    if "x!file:wl=!"=="x!file!" if "x!file:linux=!"=="x!file!" if "x!file:null=!"=="x!file!" (
        if not "x!file:glfw-3.3.8\src=!"=="x!file!" (
            echo #include ^"!file:\=/!^" >> !f_srcfile!
        )
    )
    )
    )
)
@REM set /a a_endTime=6000*(100%time:~3,2% %% 100 )+100*(100%time:~6,2% %% 100 )+(100%time:~9,2% %% 100 )
@REM set /a a_finS=(a_endTime-a_startTime)/100
@REM set /a a_finS2=(a_endTime-a_startTime)%%100
@REM echo DIRS in %a_finS%.%a_finS2% seconds

rem COMPILE

SET r_defines=
SET w_defines=/D_WIN32 /DGLEW_STATIC /DWIN32_LEAN_AND_MEAN
@REM SET w_defines=/D_WIN32 /DGLEW_BUILD /DWIN32_LEAN_AND_MEAN
@REM SET f_defines=/D_GLFW_BUILD_WIN32 /D_GLFW_WIN32 /D_GLFW_BUILD_DLL /D_WIN32 /D_GLFW_WGL /D_GLFW_USE_OPENGL
@REM SET f_defines=/D_GLFW_USE_CONFIG_H
SET f_defines=/DGLFW_BUILD_WIN32 /D_GLFW_WIN32 /D_WIN32 /D_GLFW_WGL /D_GLFW_USE_OPENGL

SET r_includedirs=/Irp3d/reactphysics3d/include
SET w_includedirs=/Iglew/glew-2.1.0/include
SET f_includedirs=/IGLFW3/glfw-3.3.8/include

SET CPP_COMPILE_OPTIONS=/std:c++14 /EHsc /Z7 /MTd /nologo /TP
SET C_COMPILE_OPTIONS=/std:c11 /MTd /nologo /TC
@REM SET C_COMPILE_OPTIONS=/std:c11 /MDd /nologo /TC

set /a c_startTime=6000 * ( 100%time:~3,2% %% 100 ) + 100 * ( 100%time:~6,2% %% 100 ) + (100%time:~9,2% %% 100 )

cl /c !r_includes! !r_defines! !r_includedirs! !CPP_COMPILE_OPTIONS! !objdir!\all_rp3d.cpp /Fo!objdir!\all_rp3d.o
cl /c !w_includes! !w_defines! !w_includedirs! !C_COMPILE_OPTIONS! !objdir!\all_glew.c /Fo!objdir!\all_glew.o
cl /c !f_includes! !f_defines! !f_includedirs! !C_COMPILE_OPTIONS! !objdir!\all_glfw3.c /Fo!objdir!\all_glfw3.o
@REM if not %errorlevel% == 0 ( exit )

set /a c_endTime=6000 * ( 100%time:~3,2% %% 100 ) + 100 * ( 100%time:~6,2% %% 100 ) + (100%time:~9,2% %% 100 )

rem LINKING

SET libdirs=

@REM The other libraries link with these so we don't need to
@REM    winmm.lib gdi32.lib user32.lib 
SET r_libs=
SET w_libs=gdi32.lib user32.lib opengl32.lib
SET f_libs=gdi32.lib user32.lib shell32.lib

mkdir bin\Debug-MSVC 2> nul

rem Todo: link with optimizations or debug info?

SET LINK_OPTIONS=/IGNORE:4006 /NOLOGO /MACHINE:x64
SET LIBRARY_OPTIONS=/IGNORE:4006 /NOLOGO /MACHINE:x64 

set /a l_startTime=6000 * ( 100%time:~3,2% %% 100 ) + 100 * ( 100%time:~6,2% %% 100 ) + ( 100%time:~9,2% %% 100 )

lib %objdir%\all_rp3d.o !libdirs! !r_libs! !LIBRARY_OPTIONS! /OUT:bin/Debug-MSVC/rp3d.lib
lib %objdir%\all_glew.o !libdirs! !w_libs! !LIBRARY_OPTIONS! /OUT:bin/Debug-MSVC/glew.lib
lib %objdir%\all_glfw3.o !libdirs! !f_libs! !LIBRARY_OPTIONS! /OUT:bin/Debug-MSVC/glfw3.lib

@REM link %objdir%\all_glew.o !libdirs! !w_libs! !LINK_OPTIONS! /DLL /OUT:bin/Debug-MSVC/glew.dll
@REM link %objdir%\all_glfw3.o !libdirs! !f_libs! !LINK_OPTIONS! /DLL /OUT:bin/Debug-MSVC/glfw3.dll

if not %errorlevel% == 0 ( goto FAILED )

set /a l_endTime=6000 * ( 100%time:~3,2% %% 100 ) + 100 * ( 100%time:~6,2% %% 100 ) + ( 100%time:~9,2% %% 100 )

set /a endTime=6000 * ( 100%time:~3,2% %% 100 )+ 100 * ( 100%time:~6,2% %% 100 ) + ( 100%time:~9,2% %% 100 )

set /a c_finS=(c_endTime-c_startTime)/100
set /a c_finS2=(c_endTime-c_startTime)%%100
set /a l_finS=(l_endTime-l_startTime)/100
set /a l_finS2=(l_endTime-l_startTime)%%100
set /a finS=(endTime-startTime)/100
set /a finS2=(endTime-startTime)%%100

echo Compilation in %c_finS%.%c_finS2% seconds
echo Linking in %l_finS%.%l_finS2% seconds
echo Finished in %finS%.%finS2% seconds

:FAILED
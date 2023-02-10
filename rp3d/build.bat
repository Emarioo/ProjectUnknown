rem cmake -S reactphysics3d -B build -DCMAKE_SYSTEM_VERSION=10.0 -G "Visual Studio 17 2022"
rem cmake -S reactphysics3d -B build -A x64 -G "MinGW Makefiles"
rem cmake -S reactphysics3d -B build/x64 -DCMAKE_SYSTEM_VERSION=10.0 -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
rem cmake -S reactphysics3d -B build/x86 -DCMAKE_SYSTEM_VERSION=10.0 -G "Visual Studio 17 2022" -A Win32 -DCMAKE_BUILD_TYPE=Release
rem pause

@ECHO OFF
@setlocal enabledelayedexpansion

@REM Setup MSVC cl environment
rem SET vcvars="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
rem call %vcvars%

@REM -----   Variables   -------
rem SET defines=
rem SET includes=
rem SET includedirs=-Ireactphysics3d/include
rem SET libs=
rem SET libdirs=
rem rem SET libdirs=/LIBPATH:../ReactPhysics/build/Debug /LIBPATH:libs/GLFW/lib /LIBPATH:libs/GLEW/lib
rem SET objdir=bin/intermediates
rem SET target=bin/rp3d.lib

rem mkdir !objdir:/=\! 2> NUL

rem @REM -----   Find Files    ------
rem SET objfiles=
rem for /r %%i in (*.cpp) do (
rem     SET file=%%i
    
rem     @REM Blacklist/Whitelist files
rem     @REM if "x!file:old=!"=="x!file!" if "x!file:vendor=!"=="x!+file!" if "x!file:FileModule=!"=="x!file!" if not "x!file:Logger=!"=="x!file!" (
rem     if not "x!file:mathematics=!"=="x!file!" (
rem     rem if not "x!file:src=!"=="x!file!" if "x!file:test=!"=="x!file!" if "x!file:testbed=!"=="x!file!" if "x!file:helloworld=!"=="x!file!" (
rem     rem if "x!file:old=!"=="x!file!" if "x!file:vendor=!"=="x!file!" if "x!file:FileModule=!"=="x!file!" if not "x!file:Engone=!"=="x!file!" (
        
rem         call :sub !file!
rem     )
rem )

rem goto SKIPPED
rem :sub

rem @REM SET objfile=bin/garb/%~nx1.obj
rem SET objfile=!objdir!/%~nx1.o

rem SET objfile=!objfile:.cpp=!
rem echo !objfile!

rem @REM XCOPY is used to detect file changes. findstr is used to detect whether XCOPY did detect changes or not. cl compiles the file. 2> NUL to make it quiet
rem rem ECHO F | XCOPY "!file!" "!objfile!" /D /q /L /Y | findstr /B /C:"1 " && cl /c /std:c++17 /TP /EHsc /MTd %includes% !file! %defines% %includedirs% /Fo!objfile!
rem ECHO F | XCOPY "!file!" "!objfile!" /D /q /i /L /Y | findstr /B /C:"1 " && g++ -c -o !objfile! -std=c++14 %includes% !file! %defines% %includedirs%

rem SET objfiles=!objfiles! !objfile!

rem exit /b
rem :SKIPPED

rem lib !objfiles! /OUT:!target!
rem ar rcs !target! !objfiles!

g++ -std=c++14 phys.cpp -I"D:\Backup\CodeProjects\ReactPhysics\reactphysics3d\include" -Lbin -lDebug/reactphysics3d.lib -o phys.exe

@REM echo y | del bin\Engone\*

@endlocal
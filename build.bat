@ECHO OFF
@setlocal enabledelayedexpansion
SET vcvars="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

call %vcvars%

ECHO -----   Find Files    ------
SET files=

@REM Aquire files
for /r %%i in (*.cpp) do (
    SET file=%%i
    
    @REM ECHO !file!
    
    @REM Blacklist files
    if "x!file:old=!"=="x!file!" if "x!file:vendor=!"=="x!file!" if not "x!file:cpp=!"=="x!file!" if not "x!file:Engone=!"=="x!file!" (
        call :ADD
    )
)

goto END
:ADD
SET files=!files! !file!
@REM ECHO !file!
exit /b

:END

@REM ECHO !files!

SET includes=/IEngone/include /Ilibs/GLFW/include /Ilibs/GLEW/include /Ilibs/reactphysics3d/include
@REM /IProjectUnknown/include 
SET libs=reactphysics3d.lib glew64s.lib glfw364.lib
SET libdirs=/link /LIBPATH:../ReactPhysics/build/Debug /link /LIBPATH:libs/GLFW/lib /link /LIBPATH:libs/GLEW/lib

cl /std:c++17 /TP /FI Engone/pch.h !files! %includes% /Fobin/garb/ %libdirs% %libs% /Fe:bin/engone.exe

@REM cd test

@REM @REM Compiles with RP3D successfully
@REM cl /std:c++17 phys.cpp /MTd /Ilibs/reactphysics3d/include /link /libpath:../../ReactPhysics/build/Debug reactphysics3d.lib

@REM @REM Compiles with some random lib
@REM cl /c /std:c++17 mylib.cpp /Fo:cl_mylib.obj
@REM lib cl_mylib.obj /OUT:cl_mylib.lib
@REM cl /std:c++17 main.cpp cl_mylib.lib /Fe:cl_main.exe

@REM @REM Also compiles with random lib
@REM gcc -c -std=c++17 mylib.cpp -o gcc_mylib.o
@REM ar rcs gcc_mylib.lib gcc_mylib.o
@REM gcc -std=c++17 main.cpp -L. -lgcc_mylib -o gcc_main.exe

@REM ECHO ---  CL  ---
@REM cl_main
@REM ECHO ---  GCC  ---
@REM gcc_main

@REM @REM Doesn't work
@REM cl /std:c++17 main.cpp /MTd gcc_mylib.lib /Fe:cl_main.exe

@REM @REM Doesn't work
@REM gcc -std=c++17 main.cpp -L. -lcl_mylib -o gcc_main.exe
@REM ECHO Combining them does not work

@REM del *.obj *.o

@REM cd ..

endlocal
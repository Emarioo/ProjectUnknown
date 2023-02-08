@ECHO OFF
@setlocal enabledelayedexpansion
SET vcvars="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

CALL %vcvars%

cd test

@REM Compiles with RP3D successfully
cl /std:c++17 phys.cpp /MTd /Ilibs/reactphysics3d/include /link /libpath:"../../ReactPhysics/build/Debug" reactphysics3d.lib

@REM Compiles with some random lib
cl /c /std:c++17 mylib.cpp /Fo:cl_mylib.obj
lib cl_mylib.obj /OUT:cl_mylib.lib
cl /std:c++17 main.cpp cl_mylib.lib /Fe:cl_main.exe

@REM Also compiles with random lib
gcc -c -std=c++17 mylib.cpp -o gcc_mylib.o
ar rcs gcc_mylib.lib gcc_mylib.o
gcc -std=c++17 main.cpp -L. -lgcc_mylib -o gcc_main.exe

ECHO ---  CL  ---
cl_main
ECHO ---  GCC  ---
gcc_main

@REM Doesn't work
cl /std:c++17 main.cpp /MTd gcc_mylib.lib /Fe:cl_main.exe

@REM Doesn't work
gcc -std=c++17 main.cpp -L. -lcl_mylib -o gcc_main.exe
ECHO Combining them does not work

del *.obj *.o

cd ..

endlocal
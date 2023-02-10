@ECHO OFF
@setlocal enabledelayedexpansion
SET vcvars="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

CALL %vcvars%

cd test

@REM Compiles with RP3D successfully
@REM cl /std:c++17 phys.cpp /MTd /I../libs/reactphysics3d/include /link /libpath:"../../ReactPhysics/build/Debug" reactphysics3d.lib

@REM cl /std:c++14 phys.cpp /I../libs/reactphysics3d/include /link reactphysics3d.lib

@REM gcc -std=c++17 phys.cpp -g -I../libs/reactphysics3d/include -L. -llibreactphysics3d -o gcc_phys.exe
g++ -std=c++17 phys.cpp -I../libs/reactphysics3d/include -L. -lreactphysics3d -o gcc_phys.exe

@REM g++ -std=c++14 phys.cpp -g -I../libs/reactphysics3d/include -L. -lreactphysics3d -o gcc_phys.exe
@REM gcc -std=c++14 phys.cpp -I../libs/reactphysics3d/include -L../../ReactPhysics/build_/Debug -lreactphysics3d -o gcc_phys.exe

@REM Compiles with some random lib
@REM cl /c /std:c++17 mylib.cpp /Fo:cl_mylib.obj
@REM lib cl_mylib.obj /OUT:cl_mylib.lib
@REM cl /std:c++17 main.cpp cl_mylib.lib /Fe:cl_main.exe

@REM Also compiles with random lib
@REM g++ -c -std=c++17 mylib.cpp -o gcc_mylib.o
@REM ar rcs gcc_mylib.lib gcc_mylib.o
@REM g++ -std=c++17 main.cpp -L. -lgcc_mylib -o gcc_main.exe

@REM ECHO ---  CL  ---
@REM cl_main
@REM ECHO ---  GCC  ---
@REM gcc_main

@REM Doesn't work
@REM cl /std:c++17 main.cpp gcc_mylib.lib /Fe:cl_main.exe

@REM Doesn't work
@REM g++ -std=c++17 main.cpp -L. -lcl_mylib -o gcc_main.exe
@REM ECHO Combining them does not work

@REM gcc -c mylib.c -o gcc_mylib.o
@REM ar rcs gcc_mylib.lib gcc_mylib.o

@REM cl main.c gcc_mylib.lib /Fe:cl_main.exe

@REM del *.obj *.o

cd ..

endlocal
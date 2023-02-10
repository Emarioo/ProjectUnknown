cmake -S glfw -B build -G "MinGW Makefiles"

rem cmake -S glfw -B build/x64 -DCMAKE_SYSTEM_VERSION=10.0 -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
rem cmake -S glfw -B build/x86 -DCMAKE_SYSTEM_VERSION=10.0 -G "Visual Studio 17 2022" -A Win32 -DCMAKE_BUILD_TYPE=Release
pause
# ProjectUnknown
A game made with C++, OpenGL, GLEW, GLFW3, ReactPhysics3D and OpenAL.

This project is very unorganized at the moment. That is due to the fact that I was bad at programming when I started and as I slowly improve I want to refactor previous architectural decisions. This has resulted in quite a big mess but it must get better at some point. Right?

# Building
## Windows
The project is compiled with MSVC (cl, link). You need to run vcvarsall.bat so that cl and link becomes available on the command line. Then you can run build.bat. You can find more information about what vcvars is on the internet.

I am aware that this isn't a great build system but I find it very easy to work with.

## Linux
Not supported

## Other notes
The source files for GLFW3, GLEW and RP3D are directly included in the project which isn't necessary. 
Premake have been used to generate project files (gmake, vs2022). Not using it anymore though.
I use Visual Studio Code.
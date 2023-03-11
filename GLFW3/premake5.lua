project "glfw3"
    kind "StaticLib"
    -- language "C++"
    language "C"
    -- toolset "gcc"
    symbols "On"
    staticruntime "on"
    
    filter "action:vs2*" 
        buildoptions {"/Zi"}
    
    buildoptions {"-fpermissive"} 
    
    targetdir "../bin/%{cfg.system}-%{cfg.buildcfg}"
    objdir "../bin/intermediates/%{cfg.system}-%{cfg.buildcfg}"
    
    defines {"GLFW_BUILD_WIN32","_GLFW_WIN32","_WIN32","_GLFW_WGL","_GLFW_USE_OPENGL"}

    files {"glfw-3.3.8/src/**.c","glfw-3.3.8/include/**.h"}
    includedirs {"glfw-3.3.8/include"}
    -- files {"glfw-3.0/src/**.c","glfw-3.0/include/**.h"}
    -- includedirs {"glfw-3.0/include"}
    -- files {"__glfw/src/**.c","__glfw/include/**.h"}
    -- includedirs {"__glfw/include"}

    -- win32 is not removed
    removefiles{"**posix**","**cocoa**","**nsgl**","**glx**","**x11**","**xkb**","**wl**","**linux**"}
    -- removefiles{"**posix**","**cocoa**","**nsgl**","**glx**","**x11**","**xkb**","**wl**","**linux**","**egl**"}
    removefiles{"**null**"}
   

    links {"gdi32"}

-- newaction {
--     trigger     = "build",
--     description = "Compile debug",
--     execute     = function ()
--         os.execute("cls")
--         local success = os.execute("make CC=gcc") -- CC=gcc to prevent "cc is not recognized"
--     end
-- }
-- newaction {
--     trigger     = "clean",
--     description = "Clean workspace",
--     execute     = function ()
--         os.rmdir("./bin")
--         os.remove("**Makefile")
--         os.remove("**.make")
--     end
-- }
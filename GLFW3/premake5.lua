project "glfw3"
    kind "StaticLib"
    -- language "C++"
    language "C"
    -- toolset "gcc"
    symbols "On"
    staticruntime "on"
    
    buildoptions {"-fpermissive"} 
    
    targetdir "../bin/%{cfg.system}-%{cfg.buildcfg}"
    objdir "../bin/intermediates/%{cfg.system}-%{cfg.buildcfg}"
    
    defines {"GLFW_BUILD_WIN32","_GLFW_WIN32","_WIN32"}

    -- files {"glfw-3.3.8/src/**.c","glfw-3.3.8/include/**.h"}
    files {"__glfw/src/**.c","__glfw/include/**.h"}

    -- win32 is not removed
    removefiles{"**posix**","**cocoa**","**nsgl**","**glx**","**x11**","**xkb**","**wl**","**linux**"}
    removefiles{"**null**"}
   
    includedirs {"__glfw/include"}
    -- includedirs {"glfw-3.3.8/include"}

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
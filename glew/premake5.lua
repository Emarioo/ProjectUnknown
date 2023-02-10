project "glew"
    kind "StaticLib"
    language "C"
    toolset "gcc"
    symbols "On"
    staticruntime "on"
    
    targetdir "../bin/%{cfg.buildcfg}"
    objdir "../bin/intermediates/%{cfg.buildcfg}"
    
    defines {"_WIN32","GLEW_STATIC","WIN32_LEAN_AND_MEAN"}

    files {"glew-2.1.0/src/glew.c","glew-2.1.0/include/glew.h"}

    includedirs {"glew-2.1.0/include"}
   
    links {"gdi32","user32","opengl32"}

-- newaction {
--     trigger     = "build",
--     description = "Compile debug",
--     execute     = function ()
--         os.execute("cls")
--         -- local success = os.execute("make CC=gcc") -- CC=gcc to prevent "cc is not recognized"
--         os.execute("make CC=gcc") -- CC=gcc to prevent "cc is not recognized"
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
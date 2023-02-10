project "rp3d"
    kind "StaticLib"
    language "C++"
    cppdialect "C++14"
    staticruntime "on"
    symbols "On"
    
    toolset "gcc"
    
    -- buildoptions {"-g"}
    
    targetdir "../bin/%{cfg.buildcfg}"
    objdir "../bin/intermediates/%{cfg.buildcfg}"
    
    files {"reactphysics3d/src/**.cpp","reactphysics3d/include/**.h"}
    -- removefiles{"**/test/**","**/testbed/**","**/helloworld/**"}
   
    includedirs {"reactphysics3d/include"}

-- newaction {
--     trigger     = "build",
--     description = "Compile debug",
--     execute     = function ()
--         os.execute("cls")
--         local success = os.execute("make")
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
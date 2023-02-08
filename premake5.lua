-- Tested with vscode, gmake2, mingw (x86_64-12.2.0-release-posix-seh-ucrt-rt_v10-rev2)

-- Todo: premake5 vscode command to setup tasks and c_ppp_properties.

workspace "ProjectUnknown"
    architecture "x64"
    configurations {"Debug","Release"}
    
    staticruntime "on"
    
	defines {"WIN32"}
    
    -- toolset ("clang")
    
    -- vs2* because vs* would include vscode
    filter "action:vs2*"
        defines { "VISUAL_STUDIO" }
	
    -- targetdir "bin/%{prj.name}/%{cfg.buildcfg}"
    -- objdir "bin/intermediates/%{prj.name}/%{cfg.buildcfg}"
    
    -- targetdir "bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
   
include "Engone"
include "ProjectUnknown"
--project "LineCounter"
    
newaction {
    trigger     = "build",
    description = "Compile linecounter debug",
    execute     = function ()
        os.execute("cls")
        -- local success = os.execute("make")
        local success = os.execute("make ProjectUnknown")
        if success == true then
            -- os.execute(defaultBuild.." "..args)
            -- os.execute(defaultBuild)
        end
    end
}

--newaction {
--    trigger     = "buildAndRun",
--    description = "Compile linecounter debug",
--    execute     = function ()
--      #  os.execute("cls")
--     #   os.execute("cmd")
--      #  os.execute("\"C:\\Program Files\\cygwin64\\bin\\bash.exe\"")
--        -- os.execute("start \"C:\\Program Files\\cygwin64\\bin\\bash.exe\" --login -i")
--      #  os.execute("echo hello")
--        -- local success = os.execute("make")
--        -- if success == true thend
--        --     os.execute(defaultBuild.." "..args)
--        -- end
--    end
--}

newaction {
    trigger     = "clean",
    description = "Clean workspace",
    execute     = function ()
        os.rmdir("./bin")
     --   os.rmdir("./obj")
        
        os.remove("**/Makefile")
        os.remove("**.make")
        
      -- os.rmdir("./.vs")
      -- os.remove("**.sln")
      -- os.remove("**.vcxproj")
      -- os.remove("**.vcxproj.user")
      -- os.remove("**.vcxproj.filters")
        print("Workspace is clean")
    end
}
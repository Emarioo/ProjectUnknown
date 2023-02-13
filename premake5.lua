workspace "ProjectUnknown"
    architecture "x64"
    configurations {"Debug","Release"}
    
    platforms{"Windows","GCC"}

    -- vs2* because vs* would include vscode
    filter "action:vs2*"
        defines { "VISUAL_STUDIO" }
        system "windows"
    
    filter "action:gmake2"
        system "linux"
    
    -- targetdir "bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
   
include "glew"
include "GLFW3"
include "rp3d"
include "Engone"
include "ProjectUnknown"
    
newaction {
    trigger     = "build",
    description = "Compile linecounter debug",
    execute     = function ()
        -- os.execute("make GLFW3 CC=gcc")
        -- os.execute("make glew CC=gcc")
        -- os.execute("make rp3d")
        os.execute("make Engone")
        os.execute("make ProjectUnknown")
    end
}
newaction {
    trigger     = "clean",
    description = "Clean workspace",
    execute     = function ()
        os.rmdir("./bin")
        os.remove("**Makefile")
        os.remove("**.make")
        
      -- os.rmdir("./.vs")
      -- os.remove("**.sln")
      -- os.remove("**.vcxproj")
      -- os.remove("**.vcxproj.user")
      -- os.remove("**.vcxproj.filters")
    end
}
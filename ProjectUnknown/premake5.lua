project "ProjectUnknown"
    kind "ConsoleApp"
    -- kind "WindowedApp"
    language "C++"
    cppdialect "C++17"
    
    staticruntime "on"
    
    buildoptions {"-g"}
    
    targetdir "../bin/%{prj.name}/%{cfg.buildcfg}"
    objdir "../bin/intermediates/%{prj.name}/%{cfg.buildcfg}"
    
    symbols "On"
    
    defines {"ENGONE_PHYSICS","ENGONE_OPENGL"}
    
    -- dependson {"Engone"}

    -- files {"src/**.cpp","include/**.h"}
    
    files {"src/**/test.cpp"}
    
    --removefiles{"**/LoggingModule."}
    removefiles{"**/Old/**"}
   
    includedirs {"include","../Engone/include","../libs/GLFW/include","../libs/GLEW/include","../libs/reactphysics3d/include"}
    
	-- pchheader ("ProUnk/pch.h")
	-- pchsource ("src/ProUnk/pch.cpp")
	
	forceincludes {"ProUnk/pch.h","Engone/ModuleDefs.h",}
	
	-- libdirs {"D:/Backup/CodeProjects/ProjectUnknown/bin/Engone/Debug"}
	-- links {"Engone"}
    
    libdirs {"../../ReactPhysics/build/Debug"}
	links {"reactphysics3d"}
    -- libdirs {"../libs/reactphysics3d/lib"}
	-- links {"reactphysics3d_debug64"}
    
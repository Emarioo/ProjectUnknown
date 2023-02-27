project "GameCode"
    kind "SharedLib"
    -- kind "WindowedApp"
    language "C++"
    cppdialect "C++11"
    -- staticruntime "on"
    symbols "On"
    -- toolset "gcc"
    
    buildoptions {"-g"}
    
    targetdir "../bin/%{prj.name}/%{cfg.buildcfg}"
    objdir "../bin/intermediates/%{prj.name}/%{cfg.buildcfg}"
    
    
    -- defines {"ENGONE_PHYSICS","ENGONE_OPENGL","GLEW_STATIC"}
    
    -- dependson {"Engone"}

    files {"src/**.cpp","include/**.h"}
    
    -- files {"src/**/test.cpp"}
    
    --removefiles{"**/LoggingModule."}
    removefiles{"**/Old/**"}
    removefiles{"**/_**"}
   
    includedirs {"include"}
    
	-- pchheader ("ProUnk/pch.h")
	-- pchsource ("src/ProUnk/pch.cpp")
	
	-- forceincludes {"ProUnk/pch.h","Engone/ModuleDefs.h",}
	
	-- libdirs {"../bin/Engone/Debug"}
	-- links {"Engone"}
    
    -- Hello, THESE ARE REQUIRED! I would think Engone already has these but apparently not.
    -- libdirs {"../bin/Debug"}
	-- links {"Ws2_32","gdi32","user32","shell32","opengl32","winmm","glew","glfw3","rp3d"}
    
    -- libdirs {"../../GLFW3/bin/Debug","../../glew/bin/Debug","../../ReactPhysics/bin/Debug"}
    -- libdirs {"../libs/GLFW/lib","../libs/GLEW/lib","../libs/reactphysics3d/lib"}
    -- libdirs {"../../ReactPhysics/build/Debug"}
    -- libdirs {"../../ReactPhysics/build_/Debug"}
	-- links {"reactphysics3d"}
    -- libdirs {"../libs/reactphysics3d/lib"}
	-- links {"reactphysics3d_debug64"}
    
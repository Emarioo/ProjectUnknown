project "Engone"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    
    symbols "On"
    
    staticruntime "on"
    buildoptions {"-g"}
    
    targetdir "../bin/%{prj.name}/%{cfg.buildcfg}"
    objdir "../bin/intermediates/%{prj.name}/%{cfg.buildcfg}"
    
    defines {"ENGONE_PHYSICS","ENGONE_OPENGL"}
    -- buildoptions {"-fPIC"}
    
    files {"src/**.cpp","include/**.h"}
    
    removefiles{"**/Old/**"}
    removefiles{"**/FileModule/**"}
    
    includedirs {"include","../libs/GLFW/include","../libs/GLEW/include","../libs/reactphysics3d/include"}
    
	pchheader ("Engone/pch.h")
	pchsource ("src/Engone/pch.cpp")
	
	forceincludes {"Engone/pch.h","Engone/ModuleDefs.h",}
    
	libdirs {"../libs/GLFW/lib","../libs/GLEW/lib","../../ReactPhysics/build/Debug"}
	-- libdirs {"../libs/GLFW/lib","../libs/GLEW/lib","../../ReactPhysics/build/Release"}
	links {"Ws2_32","gdi32","user32","shell32","opengl32","winmm","glew64s","glfw364","reactphysics3d"}
	-- links {"Ws2_32.lib","gdi32.lib","user32.lib","shell32.lib","opengl32.lib","winmm.lib","glew64s","glfw364.lib","reactphysics3d.lib"}
project "Engone"
    kind "StaticLib"
    language "C++"
    cppdialect "C++11"
    symbols "On"
    staticruntime "on"
    -- dependson {"rp3d","glew","glfw3"}
    -- system "Windows"
    -- system "Linux"
    
    targetdir "../bin/%{prj.name}/%{cfg.system}-%{cfg.buildcfg}"
    objdir "../bin/intermediates/%{prj.name}/%{cfg.system}-%{cfg.buildcfg}"
    
    defines {"ENGONE_PHYSICS","ENGONE_OPENGL","GLEW_STATIC","WIN32"}
    
    files {"src/**.cpp","include/**.h"}
    
    filter "action:vs2*" 
        buildoptions {"/Zi"}
    
    -- buildoptions{"-g"}

    removefiles{"**/Old/**"}
    -- removefiles{"**/FileModule/**"}
    removefiles{"**/_**"}
    
    includedirs {"include","../libs/GLFW/include","../libs/GLEW/include","../libs/reactphysics3d/include"}
    
	pchheader ("Engone/pch.h")
	pchsource ("src/Engone/pch.cpp")
	
	forceincludes {"Engone/pch.h","Engone/ModuleDefs.h"}
    
	libdirs {"../bin/Debug"}
	links {"Ws2_32","gdi32","user32","shell32","opengl32","winmm","glew","glfw3","rp3d"}
    
    --  <ClCompile>
    --   <PreprocessorDefinitions>%(PreprocessorDefinitions);ENGONE_SOUND;AL_LIBTYPE_STATIC</PreprocessorDefinitions>
    --   <AdditionalIncludeDirectories>%(AdditionalIncludeDirectories);$(SolutionDir)\libs\AL\include</AdditionalIncludeDirectories>
    -- </ClCompile>
    -- <Lib>
    --   <AdditionalLibraryDirectories>%(AdditionalLibraryDirectories);$(SolutionDir)\libs\AL\lib;</AdditionalLibraryDirectories>
    --   <AdditionalDependencies>%(AdditionalDependencies);OpenAL32.lib</AdditionalDependencies>
    --   <AdditionalDependencies>%(AdditionalDependencies);kernel32.lib;winspool.lib;ole32.lib;oleaut32.lib;uuid.lib;comdlg32.lib;advapi32.lib</AdditionalDependencies>
    -- </Lib>
    
	-- libdirs {"../../GLFW3/bin/Debug","../../glew/bin/Debug","../../ReactPhysics/bin/Debug"}
	-- libdirs {"../libs/GLFW/lib","../libs/GLEW/lib","../../ReactPhysics/build/Release"}
	-- links {"Ws2_32","gdi32","user32","shell32","opengl32","winmm","glew64s","glfw364","reactphysics3d"}
	-- links {"Ws2_32.lib","gdi32.lib","user32.lib","shell32.lib","opengl32.lib","winmm.lib","glew64s","glfw364.lib","reactphysics3d.lib"}
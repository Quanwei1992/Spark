workspace "Razor" 
    architecture "x64"
    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }
    location "build"


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
InlcudeDir = {}
InlcudeDir["GLFW"] = "Razor/vendor/GLFW/include"
include "Razor/vendor/GLFW"
project "Razor"
    location "build/Razor"
    kind "SharedLib"
    language "C++"

    targetdir("build/bin/" .. outputdir .. "/%{prj.name}")
    objdir("build/intermediate/" .. outputdir .. "/%{prj.name}")
    
    pchheader "rzpch.h"
    pchsource "%{prj.name}/src/rzpch.cpp"
    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
    }

    includedirs
    {
        "%{prj.name}/vendor/spdlog/include",
        "%{InlcudeDir.GLFW}",
        "%{prj.name}/src"
    }

    links
    {
        "GLFW",
        "opengl32.lib"
    }

    filter "system:windows"

        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "RZ_PLATFORM_WINDOWS",
            "RZ_BUILD_DLL"
        }

    filter "configurations:Debug"
        defines {"RZ_DEBUG","RZ_ENABLE_ASSERTS"}
        symbols "On"

    filter "configurations:Release"
        defines "RZ_RELEASE"
        optimize "On"    

    filter "configurations:Dist"
        defines "RZ_DIST"
        optimize "On"

--------------------------------------------------------------------------------

project "Sandbox"
    location "build/Sandbox"
    kind "ConsoleApp"
    language "C++"
    
    targetdir("build/bin/" .. outputdir .. "/%{prj.name}")
    objdir("build/intermediate/" .. outputdir .. "/%{prj.name}")
    
    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
    }

    includedirs
    {
        "Razor/vendor/spdlog/include",
        "Razor/src/",
    }

    links
    {
        "Razor"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"
    
        defines
        {
            "RZ_PLATFORM_WINDOWS"
        }

        postbuildcommands
        {
            ("{COPYDIR} ../bin/" .. outputdir .. "/Razor %{cfg.targetdir}" )
        }
    
    
    filter "configurations:Debug"
        defines "RZ_DEBUG"
        symbols "On"
    
    filter "configurations:Release"
        defines "RZ_RELEASE"
        optimize "On"    
    
    filter "configurations:Dist"
        defines "RZ_DIST"
        optimize "On"


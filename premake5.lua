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
InlcudeDir["Glad"] = "Razor/vendor/Glad/include"
InlcudeDir["ImGui"] = "Razor/vendor/imgui"
InlcudeDir["glm"] = "Razor/vendor/glm"
include "Razor/vendor/GLFW"
include "Razor/vendor/Glad"
include "Razor/vendor/imgui"

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
        "%{prj.name}/vendor/glm/glm/**.hpp",
        "%{prj.name}/vendor/glm/glm/**.inl",
        "%{prj.name}/vendor/glm/util/glm.natvis",
    }

    includedirs
    {
        "%{prj.name}/vendor/spdlog/include",
        "%{InlcudeDir.GLFW}",
        "%{InlcudeDir.Glad}",
        "%{InlcudeDir.ImGui}",
        "%{InlcudeDir.glm}",
        "%{prj.name}/src"
    }

    links
    {
        "GLFW",
        "Glad",
        "ImGui",
        "opengl32.lib"
    }

    filter "system:windows"

        cppdialect "C++17"
        staticruntime "Off"
        systemversion "latest"

        defines
        {
            "RZ_PLATFORM_WINDOWS",
            "RZ_BUILD_DLL",
            "GLFW_INCLUDE_NONE"
        }

        postbuildcommands
        {
            ("{MKDIR} ../bin/" .. outputdir .. "/Sandbox" ),
            ("{COPYDIR}  %{cfg.targetdir} ../bin/" .. outputdir .. "/Sandbox" )
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
        "%{InlcudeDir.glm}",
        "Razor/src/",
    }

    links
    {
        "Razor"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "Off"
        systemversion "latest"
    
        defines
        {
            "RZ_PLATFORM_WINDOWS"
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


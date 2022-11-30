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
InlcudeDir["stb_image"] = "Razor/vendor/stb_image"
InlcudeDir["spdlog"] = "Razor/vendor/spdlog/include"
include "Razor/vendor/GLFW"
include "Razor/vendor/Glad"
include "Razor/vendor/imgui"

project "Razor"
    location "build/Razor"
    kind "StaticLib"
    language "C++"
    staticruntime "on"
    flags { "FatalCompileWarnings" }
    cppdialect "C++17"
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
        "%{prj.name}/vendor/stb_image/**.h",
        "%{prj.name}/vendor/stb_image/**.cpp",
    }

    includedirs
    {
        "%{InlcudeDir.stb_image}",
        "%{InlcudeDir.spdlog}",
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
        systemversion "latest"
        defines
        {
            "RZ_PLATFORM_WINDOWS",
            "GLFW_INCLUDE_NONE"
        }

    filter "configurations:Debug"
        defines {"RZ_DEBUG","RZ_ENABLE_ASSERTS"}
        symbols "on"

    filter "configurations:Release"
        defines "RZ_RELEASE"
        optimize "on"

--------------------------------------------------------------------------------

project "Sandbox"
    location "build/Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    flags { "FatalCompileWarnings" }
    targetdir("build/bin/" .. outputdir .. "/%{prj.name}")
    objdir("build/intermediate/" .. outputdir .. "/%{prj.name}")
   
    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/assets/**.*",
    }

    includedirs
    {
        "Razor/vendor/spdlog/include",
        "%{InlcudeDir.glm}",
        "%{InlcudeDir.ImGui}",
        "Razor/src/",
    }

    links
    {
        "Razor"
    }
    postbuildcommands
    { 
        "{COPYDIR} %{_MAIN_SCRIPT_DIR}/%{prj.name}/assets  ./assets"
    }

    filter "system:windows"
        systemversion "latest"
        defines
        {
            "RZ_PLATFORM_WINDOWS"
        }
 
    filter "configurations:Debug"
        defines "RZ_DEBUG"
        symbols "on"

    
    filter "configurations:Release"
        defines "RZ_RELEASE"
        optimize "on" 


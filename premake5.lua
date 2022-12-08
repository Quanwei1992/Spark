workspace "Spark" 
    architecture "x64"
    startproject "Editor"
    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }
    location "build"


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

InlcudeDir = {}
InlcudeDir["GLFW"] = "Spark/vendor/GLFW/include"
InlcudeDir["Glad"] = "Spark/vendor/Glad/include"
InlcudeDir["ImGui"] = "Spark/vendor/imgui"
InlcudeDir["glm"] = "Spark/vendor/glm"
InlcudeDir["stb_image"] = "Spark/vendor/stb_image"
InlcudeDir["spdlog"] = "Spark/vendor/spdlog/include"

group "Dependcies"
    include "Spark/vendor/GLFW"
    include "Spark/vendor/Glad"
    include "Spark/vendor/imgui"
group ""


project "Spark"
    location "build/Spark"
    kind "StaticLib"
    language "C++"
    staticruntime "on"
    flags { "FatalCompileWarnings" }
    cppdialect "C++17"
    targetdir("build/bin/" .. outputdir .. "/%{prj.name}")
    objdir("build/intermediate/" .. outputdir .. "/%{prj.name}")
    
    pchheader "skpch.h"
    pchsource "%{prj.name}/src/skpch.cpp"
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
            "SK_PLATFORM_WINDOWS",
            "GLFW_INCLUDE_NONE"
        }

    filter "configurations:Debug"
        defines {"SK_DEBUG","SK_ENABLE_ASSERTS"}
        symbols "on"

    filter "configurations:Release"
        defines "SK_RELEASE"
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
        "Spark/vendor/spdlog/include",
        "%{InlcudeDir.glm}",
        "%{InlcudeDir.ImGui}",
        "Spark/src/",
    }

    links
    {
        "Spark"
    }
    postbuildcommands
    { 
        "{COPYDIR} %{_MAIN_SCRIPT_DIR}/%{prj.name}/assets  ./assets"
    }

    filter "system:windows"
        systemversion "latest"
        defines
        {
            "SK_PLATFORM_WINDOWS"
        }
 
    filter "configurations:Debug"
        defines "SK_DEBUG"
        symbols "on"

    
    filter "configurations:Release"
        defines "SK_RELEASE"
        optimize "on" 




--------------------------------------------------------------------------------

project "Editor"
    location "build/Editor"
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
        "Spark/vendor/spdlog/include",
        "%{InlcudeDir.glm}",
        "%{InlcudeDir.ImGui}",
        "Spark/src/",
    }

    links
    {
        "Spark"
    }
    postbuildcommands
    { 
        "{COPYDIR} %{_MAIN_SCRIPT_DIR}/%{prj.name}/assets  ./assets"
    }

    filter "system:windows"
        systemversion "latest"
        defines
        {
            "SK_PLATFORM_WINDOWS"
        }
 
    filter "configurations:Debug"
        defines "SK_DEBUG"
        symbols "on"

    
    filter "configurations:Release"
        defines "SK_RELEASE"
        optimize "on"         
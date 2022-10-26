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
        systemversion "10.0.19041.0"
    
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


project "Razor"
    location "build/Razor"
    kind "SharedLib"
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
        "%{prj.name}/vendor/spdlog/include"
    }

    filter "system:windows"

        cppdialect "C++17"
        staticruntime "On"
        systemversion "10.0.19041.0"

        defines
        {
            "RZ_PLATFORM_WINDOWS",
            "RZ_BUILD_DLL"
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

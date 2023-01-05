project "box2d"
    kind "StaticLib"
    language "C++"   
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
	{
		"src/**.cpp",
        "src/**.h",
        "include/**.h"
	}
    includedirs
    {
        "include",
        "src",
    }

    filter "system:windows"
        systemversion "latest"
       

    filter "configurations:Debug"
        symbols "on"

    filter "configurations:Release"
        optimize "on"
project "Glad"
    kind "StaticLib"
    language "C"   
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
	{
		"src/glad.c",
		"include/glad/glad.h",
		"include/KHR/khrplatform.h"
	}
    includedirs
    {
        "include",
    }

    filter "system:windows"
        systemversion "latest"
       

    filter "configurations:Debug"
        symbols "on"

    filter "configurations:Release"
        optimize "on"
project "Glad"
    kind "StaticLib"
    language "C"   
    staticruntime "on"
    targetdir("%{wks.location}/bin/" .. outputdir .. "/razor/vendor/%{prj.name}")
    objdir("%{wks.location}/intermediate/" .. outputdir .. "/razor/vendor/%{prj.name}")
    location "%{wks.location}/Razor/vendor"

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
        defines "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "Release"
        optimize "on"
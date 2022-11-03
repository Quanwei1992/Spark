project "Glad"
    kind "StaticLib"
    language "C"   
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
        staticruntime "Off"

    filter "configurations:Debug"
        defines "Debug"
        symbols "On"

    filter "configurations:Release"
        defines "Release"
        optimize "On"    

    filter "configurations:Dist"
        defines "Release"
        optimize "On"
        symbols "off"
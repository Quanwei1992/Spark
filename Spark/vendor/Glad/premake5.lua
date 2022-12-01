project "Glad"
    kind "StaticLib"
    language "C"   
    staticruntime "on"
    targetdir("%{wks.location}/bin/" .. outputdir .. "/Spark/vendor/%{prj.name}")
    objdir("%{wks.location}/intermediate/" .. outputdir .. "/Spark/vendor/%{prj.name}")
    location "%{wks.location}/Spark/vendor"

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
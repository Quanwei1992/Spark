project "yaml-cpp"
    kind "StaticLib"
    language "C"   
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
	{
		"src/**.h",
		"include/**.h",
        "src/**.cpp"
	}
    
    includedirs
    {
        "include"
    }

    filter "configurations:Debug"
        symbols "on"

    filter "configurations:Release"
        optimize "on" 
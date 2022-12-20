project "yaml-cpp"
    kind "StaticLib"
    language "C"   
    staticruntime "on"
    targetdir("%{wks.location}/bin/" .. outputdir .. "/Spark/vendor/%{prj.name}")
    objdir("%{wks.location}/intermediate/" .. outputdir .. "/Spark/vendor/%{prj.name}")
    location "%{wks.location}/Spark/vendor"

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
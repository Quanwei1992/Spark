project "ImGui"
    kind "StaticLib"
    language "C"   
    targetdir("%{wks.location}/bin/" .. outputdir .. "/razor/vendor/%{prj.name}")
    objdir("%{wks.location}/intermediate/" .. outputdir .. "/razor/vendor/%{prj.name}")
    location "%{wks.location}/Razor/vendor"

    files
	{
        "imconfig.h",
		"imgui.h",
        "imgui.cpp",
        "imgui_demo.cpp",
        "imgui_draw.cpp",
        "imgui_internal.h",
        "imgui_tables.cpp",
        "imgui_widgets.cpp",
        "imstb_rectpack.h",
        "imstb_textedit.h",
        "imstb_truetype.h"
	}

    includedirs
    {
        "./"
    }
    

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
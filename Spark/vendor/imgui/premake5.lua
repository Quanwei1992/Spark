project "ImGui"
    kind "StaticLib"
    language "C"   
    staticruntime "on"
    targetdir("%{wks.location}/bin/" .. outputdir .. "/Spark/vendor/%{prj.name}")
    objdir("%{wks.location}/intermediate/" .. outputdir .. "/Spark/vendor/%{prj.name}")
    location "%{wks.location}/Spark/vendor"

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
        "imstb_truetype.h",
        "backends/imgui_impl_glfw.h",
        "backends/imgui_impl_glfw.cpp",
        "backends/imgui_impl_opengl3.h",
        "backends/imgui_impl_opengl3.cpp",

	}

    includedirs
    {
        "./",
        "../GLFW/include"
    }
 
    links
    {
        "GLFW",
    }
    

    filter "configurations:Debug"
        symbols "on"

    filter "configurations:Release"
        optimize "on"    
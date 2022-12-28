project "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"
    flags { "FatalCompileWarnings" }

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/Spark/vendor/spdlog/include",
		"%{wks.location}/Spark/src",
		"%{wks.location}/Spark/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.ImGuizmo}",
        "%{IncludeDir.ImGui}"
	}

	links
	{
		"Spark"
	}

    postbuildcommands
    { 
        "{COPYDIR} %{LibraryDir.VulkanSDK_DebugLibs}/Bin  %{cfg.targetdir}"
    }

	filter "system:windows"
		systemversion "latest"
        defines "SK_PLATFORM_WINDOWS"

	filter "configurations:Debug"
		defines "SK_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "SK_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "SK_DIST"
		runtime "Release"
		optimize "on"
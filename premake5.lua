include "./vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

workspace "Spark"
	architecture "x86_64"
	startproject "Editor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	solution_items
	{
		".editorconfig"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
    include "vendor/premake"
	include "Spark/vendor/GLFW"
	include "Spark/vendor/Glad"
	include "Spark/vendor/imgui"
	include "Spark/vendor/yaml-cpp"
	include "Spark/vendor/box2d"
group ""

include "Spark"
include "Sandbox"
include "Editor"
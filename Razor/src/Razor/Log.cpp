#include "rzpch.h"
#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"
 std::shared_ptr<spdlog::logger> Razor::Log::s_CoreLogger;
 std::shared_ptr<spdlog::logger> Razor::Log::s_ClientLogger;

 void Razor::Log::Init()
 {
	 spdlog::set_pattern("%^[%T] %n: %v%$");
	 s_CoreLogger = spdlog::stdout_color_mt("RAZOR");
	 s_CoreLogger->set_level(spdlog::level::trace);

	 s_ClientLogger = spdlog::stdout_color_mt("APP");
	 s_ClientLogger->set_level(spdlog::level::trace);
 }
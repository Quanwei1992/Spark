#pragma once
#include "Base.h"

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

#include <gtx/string_cast.hpp>

namespace Spark
{
	class Log
	{
	public:
		static void Init();
		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};

}


template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
	return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
{
	return os << glm::to_string(quaternion);
}


// Core log macros
#define SK_CORE_TRACE(...)		::Spark::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define SK_CORE_INFO(...)		::Spark::Log::GetCoreLogger()->info(__VA_ARGS__)
#define SK_CORE_WARN(...)		::Spark::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define SK_CORE_ERROR(...)		::Spark::Log::GetCoreLogger()->error(__VA_ARGS__)
#define SK_CORE_FATAL(...)		::Spark::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Core client macros
#define SK_TRACE(...)			::Spark::Log::GetClientLogger()->trace(__VA_ARGS__)
#define SK_INFO(...)			::Spark::Log::GetClientLogger()->info(__VA_ARGS__)
#define SK_WARN(...)			::Spark::Log::GetClientLogger()->warn(__VA_ARGS__)
#define SK_ERROR(...)			::Spark::Log::GetClientLogger()->error(__VA_ARGS__)
#define SK_FATAL(...)			::Spark::Log::GetClientLogger()->critical(__VA_ARGS__)

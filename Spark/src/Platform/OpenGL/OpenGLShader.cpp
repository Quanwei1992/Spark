#include "skpch.h"
#include "OpenGLShader.h"
#include "Spark/Core/Timer.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <filesystem>
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

namespace Spark
{

	namespace Utils
	{
		static GLenum  ShaderTypeFromString(const std::string& type)
		{
			if (type == "vertex") return GL_VERTEX_SHADER;
			if (type == "fragment" || type == "pixel") return GL_FRAGMENT_SHADER;
			SK_CORE_ASSERT(false, "Unkonwn shader type!");
			return 0;
		}
		static const char* GLShaderStageToString(GLenum stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:   return "GL_VERTEX_SHADER";
			case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
			}
			SK_CORE_ASSERT(false, "Unkonw shader stage");
			return nullptr;
		}

		static const char* GetCacheDirectory()
		{
			return "assets/cache/shader/opengl";
		}

		static void CreateCacheDirectoryIfNeeded()
		{
			std::string cacheDirectory = GetCacheDirectory();
			if (!std::filesystem::exists(cacheDirectory))
			{
				std::filesystem::create_directories(cacheDirectory);
			}
		}

		static shaderc_shader_kind GLShaderStageToShaderC(GLenum stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER: return shaderc_glsl_vertex_shader;
			case GL_FRAGMENT_SHADER:return shaderc_glsl_fragment_shader;
			}
			SK_CORE_ASSERT(false, "Unkonw shader stage");
			return (shaderc_shader_kind)0;
		}

		static const char* GLShaderStageCachedVulkanFileExtension(GLenum stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER: return ".cached_vulkan.vert";
			case GL_FRAGMENT_SHADER: return ".cached_vulkan.frag";
			}
			SK_CORE_ASSERT(false, "Unkonw shader stage");
			return "";
		}
		static const char* GLShaderStageCachedOpenGLFileExtension(uint32_t stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:    return ".cached_opengl.vert";
			case GL_FRAGMENT_SHADER:  return ".cached_opengl.frag";
			}
			SK_CORE_ASSERT(false, "Unkonw shader stage");
			return "";
		}
	}

	

	std::string OpenGLShader::ReadFile(const std::string filePath)
	{
		SK_PROFILE_FUNCTION();
		std::string result;
		std::ifstream is(filePath, std::ios::in | std::ios::binary);
		if (is)
		{
			is.seekg(0, is.end);
			result.resize(is.tellg());
			is.seekg(0, is.beg);
			is.read(&result[0], result.size());
			is.close();
		}
		else {
			SK_CORE_ERROR("Count not open file '{0}'", filePath);
		}
		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string source)
	{
		SK_PROFILE_FUNCTION();
		std::unordered_map<GLenum, std::string> shaderSources;
		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			SK_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			SK_CORE_ASSERT(type == "vertex" || type == "fragment" || type == "pixel", "Invalid shader type specified!");

			size_t nextLinePos = source.find_first_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);
			shaderSources[Utils::ShaderTypeFromString(type)]
				= source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}
		return shaderSources;
	}


	void OpenGLShader::CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSouces)
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
		const bool optimize = true;
		if (optimize)
		{
			options.SetOptimizationLevel(shaderc_optimization_level_performance);
		}

		std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

		auto& shaderData = m_VukanSPIRV;
		shaderData.clear();
		for (auto&& [stage, source] : shaderSouces)
		{
			std::filesystem::path shaderFilePath = m_FilePath;
			std::filesystem::path cachePath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedVulkanFileExtension(stage));

			std::ifstream in(cachePath, std::ios::in | std::ios::binary);

			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = shaderData[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else {
				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_FilePath.c_str(), options);
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					SK_CORE_ERROR(module.GetErrorMessage());
					SK_CORE_ASSERT(false, "");
				}
				else {
					shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());
					std::ofstream out(cachePath, std::ios::out | std::ios::binary);
					if (out.is_open())
					{
						auto& data = shaderData[stage];
						out.write((char*)data.data(), data.size() * sizeof(uint32_t));
						out.flush();
						out.close();
					}
				}		
			}
		}

		for (auto&& [stage, data] : shaderData)
		{
			Reflect(stage, data);
		}
	}

	void OpenGLShader::CompileOrGetOpenGLBinaries()
	{
		auto& shaderData = m_OpenGLSPIRV;

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
		const bool optimize = false;
		if (optimize)
		{
			options.SetOptimizationLevel(shaderc_optimization_level_performance);
		}
		std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

		shaderData.clear();
		m_OpenGLSourceCode.clear();
		for (auto&& [stage, spirv] : m_VukanSPIRV)
		{
			std::filesystem::path shaderFilePath = m_FilePath;
			std::filesystem::path cachePath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedOpenGLFileExtension(stage));

			std::ifstream in(cachePath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(std::ios::beg);

				auto& data = shaderData[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else {
				spirv_cross::CompilerGLSL glslCompiler(spirv);
				m_OpenGLSourceCode[stage] = glslCompiler.compile();
				auto& source = m_OpenGLSourceCode[stage];

				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_FilePath.c_str());
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					SK_CORE_ERROR(module.GetErrorMessage());
					SK_CORE_ASSERT(false, "");
				}
				shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());
				std::ofstream out(cachePath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shaderData[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}

			}
		}

	}




	void OpenGLShader::CreateProgram()
	{
		GLuint program = glCreateProgram();

		std::vector<GLuint> shaderIDs;

		for (auto&& [stage, spirv] : m_OpenGLSPIRV)
		{
			GLuint shaderID = shaderIDs.emplace_back(glCreateShader(stage));
			glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), (GLsizei)(spirv.size() * sizeof(uint32_t)));
			glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
			glAttachShader(program, shaderID);
		}
		glLinkProgram(program);
		
		GLint isLinked;
		glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());
			glDeleteProgram(program);
			for (auto id : shaderIDs)
			{
				glDeleteShader(id);
			}
		}
		else {
			for (auto id : shaderIDs)
			{
				glDetachShader(program, id);
				glDeleteShader(id);
			}
		}
		m_RendererID = program;
	}


	void OpenGLShader::Reflect(GLenum stage, const std::vector<uint32_t>& shaderData)
	{
		spirv_cross::Compiler compiler(shaderData);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		SK_CORE_TRACE("OpenGLShader::Reflect - {0} {1}", Utils::GLShaderStageToString(stage), m_FilePath);
		SK_CORE_TRACE("		{0} uniform buffers", resources.uniform_buffers.size());
		SK_CORE_TRACE("		{0} resources", resources.sampled_images.size());

		SK_CORE_TRACE("Uniform buffers:");
		for (const auto& resource : resources.uniform_buffers)
		{
			const auto bufferType = compiler.get_type(resource.base_type_id);
			size_t bufferSize = compiler.get_declared_struct_size(bufferType);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			size_t memberCount = bufferType.member_types.size();

			SK_CORE_TRACE("	{0}", resource.name);
			SK_CORE_TRACE("		Size = {0}", bufferSize);
			SK_CORE_TRACE("		Binding = {0}", binding);
			SK_CORE_TRACE("		Members = {0}", memberCount);
		}
	}

	OpenGLShader::OpenGLShader(const std::string& filePath)
		:m_FilePath(filePath)
	{
		SK_PROFILE_FUNCTION();

		Utils::CreateCacheDirectoryIfNeeded();

		std::string source = ReadFile(filePath);
		auto shaderSources = PreProcess(source);

		{
			Timer timer;
			CompileOrGetVulkanBinaries(shaderSources);
			CompileOrGetOpenGLBinaries();
			CreateProgram();
			SK_CORE_TRACE("Shader creation took {0} ms", timer.ElapsedMillis());
		}

		// Extract name from filepath
		auto lastSlash = filePath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filePath.rfind('.');
		auto count = lastDot == std::string::npos ? filePath.size() - lastSlash : lastDot - lastSlash;
		m_Name = filePath.substr(lastSlash, count);
	}


	OpenGLShader::OpenGLShader(const std::string& name,const std::string& vertexSrc, const std::string& fragmentSrc)
		:m_Name(name)
		,m_RendererID(0)
	{
		SK_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;

		CompileOrGetVulkanBinaries(sources);
		CompileOrGetOpenGLBinaries();
		CreateProgram();

	}



	OpenGLShader::~OpenGLShader()
	{
		SK_PROFILE_FUNCTION();
		glDeleteProgram(m_RendererID);
	}
	void OpenGLShader::Bind() const
	{
		SK_PROFILE_FUNCTION();
		glUseProgram(m_RendererID);
	}
	void OpenGLShader::Unbind() const
	{
		SK_PROFILE_FUNCTION();
		glUseProgram(0);
	}
	void OpenGLShader::SetInt(const std::string& name, int value)
	{
		SK_PROFILE_FUNCTION();
		UploadUniformInt(name, value);
	}

	void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count)
	{
		SK_PROFILE_FUNCTION();
		UploadUniformIntArray(name, values, count);
	}

	void OpenGLShader::SetFloat(const std::string& name, float value)
	{
		SK_PROFILE_FUNCTION();
		UploadUniformFloat(name, value);
	}
	void OpenGLShader::SetFloat2(const std::string& name, const glm::vec2& value)
	{
		SK_PROFILE_FUNCTION();
		UploadUniformFloat2(name, value);
	}
	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		SK_PROFILE_FUNCTION();
		UploadUniformFloat3(name, value);
	}
	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		SK_PROFILE_FUNCTION();
		UploadUniformFloat4(name, value);
	}
	void OpenGLShader::SetMat3(const std::string& name, const glm::mat3& matrix)
	{
		SK_PROFILE_FUNCTION();
		UploadUniformMat3(name, matrix);
	}
	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& matrix)
	{
		SK_PROFILE_FUNCTION();
		UploadUniformMat4(name, matrix);
	}

	void OpenGLShader::UploadUniformInt(const std::string& name, int value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		SK_CORE_ASSERT(location != -1, "Not found uniform location!");
		glUniform1i(location, value);
	}
	void OpenGLShader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		SK_CORE_ASSERT(location != -1, "Not found uniform location!");
		glUniform1iv(location, count, values);

	}
	void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		SK_CORE_ASSERT(location != -1, "Not found uniform location!");
		glUniform1f(location, value);
	}
	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		SK_CORE_ASSERT(location != -1, "Not found uniform location!");
		glUniform2fv(location, 1, glm::value_ptr(value));
	}
	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		SK_CORE_ASSERT(location != -1, "Not found uniform location!");
		glUniform3fv(location, 1, glm::value_ptr(value));
	}
	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		SK_CORE_ASSERT(location != -1, "Not found uniform location!");
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		SK_CORE_ASSERT(location != -1, "Not found uniform location!");
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		SK_CORE_ASSERT(location != -1, "Not found uniform location!");
		glUniform4fv(location, 1, glm::value_ptr(value));
	}
}

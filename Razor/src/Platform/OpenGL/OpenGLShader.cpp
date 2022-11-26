#include "rzpch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>

namespace Razor
{
	static GLenum  ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex") return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel") return GL_FRAGMENT_SHADER;
		RZ_CORE_ASSERT(false, "Unkonwn shader type!");
		return 0;
	}


	std::string OpenGLShader::ReadFile(const std::string filePath)
	{
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
			RZ_CORE_ERROR("Count not open file '{0}'", filePath);
		}
		return result;
	}
	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string source)
	{
		std::unordered_map<GLenum, std::string> shaderSources;
		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			RZ_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			RZ_CORE_ASSERT(type == "vertex" || type == "fragment" || type == "pixel", "Invalid shader type specified!");

			size_t nextLinePos = source.find_first_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);
			shaderSources[ShaderTypeFromString(type)]
				= source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}
		return shaderSources;
	}
	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSouces)
	{
		RZ_CORE_ASSERT(shaderSouces.size() == 2, "We only support 2 shaders for now");

		std::array<GLenum, 2> compiledShaders;
		int glShaderIDIndex = 0;
		for (auto& kv : shaderSouces)
		{
			GLenum type = kv.first;
			const std::string& source = kv.second;

			GLuint shader = glCreateShader(type);
			const GLchar* sourceCStr = source.c_str();
			glShaderSource(shader, 1,&sourceCStr, 0);
			glCompileShader(shader);
			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				glDeleteShader(shader);

				for (int index = 0; index < glShaderIDIndex; index++)
				{
					glDeleteShader(compiledShaders[index]);
				}
				RZ_CORE_ERROR("{0}", infoLog.data());
				RZ_CORE_ASSERT(false, "Shader compilation failure!");
				return;
			}
			compiledShaders[glShaderIDIndex++] = shader;
		}

		GLuint program = glCreateProgram();


		for (GLuint compiledShader : compiledShaders)
		{
			glAttachShader(program, compiledShader);
		}

		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);
			// Don't leak shaders either.
			for (GLuint compiledShader : compiledShaders)
			{
				glDeleteShader(compiledShader);
			}
			RZ_CORE_ERROR("{0}", infoLog.data());
			RZ_CORE_ASSERT(false, "Shader link failure!");
			return;
		}

		// Always detach shaders after a successful link.
		for (GLuint compiledShader : compiledShaders)
		{
			glDeleteShader(compiledShader);
		}
		m_RendererID = program;
	}

	OpenGLShader::OpenGLShader(const std::string& filePath)
	{
		std::string shaderSource = ReadFile(filePath);
		auto sources = PreProcess(shaderSource);
		Compile(sources);

		auto lastSlash = filePath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filePath.rfind('.');
		auto count = lastDot == std::string::npos ? filePath.size()  - lastSlash : lastDot - lastSlash;
		m_Name = filePath.substr(lastSlash, count);
	}

	OpenGLShader::OpenGLShader(const std::string& name,const std::string& vertexSrc, const std::string& fragmentSrc)
		:m_Name(name)
		,m_RendererID(0)
	{
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;
		Compile(sources);
	}



	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_RendererID);
	}
	void OpenGLShader::Bind() const
	{
		glUseProgram(m_RendererID);
	}
	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}
	void OpenGLShader::UploadUniformInt(const std::string& name, int value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		RZ_CORE_ASSERT(location != -1, "Not found uniform location!");
		glUniform1i(location, value);
	}
	void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		RZ_CORE_ASSERT(location != -1, "Not found uniform location!");
		glUniform1f(location, value);
	}
	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		RZ_CORE_ASSERT(location != -1, "Not found uniform location!");
		glUniform2fv(location, 1, glm::value_ptr(value));
	}
	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		RZ_CORE_ASSERT(location != -1, "Not found uniform location!");
		glUniform3fv(location, 1, glm::value_ptr(value));
	}
	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		RZ_CORE_ASSERT(location != -1, "Not found uniform location!");
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		RZ_CORE_ASSERT(location != -1, "Not found uniform location!");
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		RZ_CORE_ASSERT(location != -1, "Not found uniform location!");
		glUniform4fv(location, 1, glm::value_ptr(value));
	}
}
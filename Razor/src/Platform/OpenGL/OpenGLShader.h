#pragma once

#include "Razor/Renderer/Shader.h"

#include <glm/glm.hpp>
#include <unordered_map>
#include <string>

// TODO: REMOVE! 
typedef unsigned int GLenum;
namespace Razor
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filePath);
		OpenGLShader(const std::string& name,const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void UploadUniformInt(const std::string& name, int value)  override;

		virtual void UploadUniformFloat(const std::string& name, float value)  override;
		virtual void UploadUniformFloat2(const std::string& name, const glm::vec2& value)  override;
		virtual void UploadUniformFloat3(const std::string& name, const glm::vec3& value)  override;
		virtual void UploadUniformFloat4(const std::string& name, const glm::vec4& value)  override;

		virtual void UploadUniformMat3(const std::string& name, const glm::mat3& matrix) override;
		virtual void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) override;

		virtual const std::string& GetName() const { return m_Name; }

	private:
		std::string ReadFile(const std::string filePath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string source);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSouces);
	private:
		uint32_t m_RendererID;
		std::string m_Name;
	};
}
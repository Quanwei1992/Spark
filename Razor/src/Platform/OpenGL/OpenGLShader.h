#pragma once

#include "Razor/Renderer/Shader.h"

#include <string>
#include <glm/glm.hpp>

namespace Razor
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
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


	private:
		uint32_t m_RendererID;
	};
}
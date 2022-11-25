#pragma once

#include <string>
#include <glm/glm.hpp>
namespace Razor
{
	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void UploadUniformInt(const std::string& name, int value) = 0;

		virtual void UploadUniformFloat(const std::string& name, float value) = 0;
		virtual void UploadUniformFloat2(const std::string& name, const glm::vec2& value) = 0;
		virtual void UploadUniformFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void UploadUniformFloat4(const std::string& name, const glm::vec4& value) = 0;

		virtual void UploadUniformMat3(const std::string& name, const glm::mat3& matrix) = 0;
		virtual void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) = 0;

		static Shader* Create(const std::string& vertexSrc, const std::string& fragmentSrc);
		static Shader* Create(const std::string& filePath);
	};
}
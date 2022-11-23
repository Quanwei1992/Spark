#include "rzpch.h"
#include "Shader.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "Renderer.h"

namespace Razor
{
	Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: RZ_CORE_ASSERT(false, "RendererAPI::None is current not support!"); return nullptr;
		case RendererAPI::API::OpenGL: return new OpenGLShader(vertexSrc,fragmentSrc);
		}
		RZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}

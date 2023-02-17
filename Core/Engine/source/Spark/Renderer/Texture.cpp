#include "skpch.h"
#include "Spark/Renderer/Texture.h"
#include "Spark/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLTexture.h"

#include <memory>

namespace Spark
{
	Ref<Texture2D> Texture2D::Create(TextureFormat format, unsigned int width, unsigned int height, TextureWrap wrap)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: SK_CORE_ASSERT(false, "RendererAPI::None is current not support!"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(format,width,height,wrap);
		}
		SK_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
	Ref<Texture2D> Texture2D::Create(const std::string& path,bool srgb)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: SK_CORE_ASSERT(false, "RendererAPI::None is current not support!"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(path,srgb);
		}
		SK_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<TextureCube> TextureCube::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: SK_CORE_ASSERT(false, "RendererAPI::None is current not support!"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLTextureCube>(path);
		}
		SK_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	uint32_t Texture::GetBPP(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::RGB:    return 3;
		case TextureFormat::RGBA:   return 4;
		}
		return 0;
	}
}

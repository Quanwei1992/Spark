#pragma once

#include "Razor/Renderer/Texture.h"

namespace Razor
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& path);
		~OpenGLTexture2D();
		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GeHeight() const override { return m_Height; }
		virtual void Bind(uint32_t slot) const override;
	private:
		std::string m_Path;
		uint32_t m_RendererID;
		uint32_t m_Width, m_Height;
	};
}
#pragma once

#include "Spark/Renderer/Texture.h"

namespace Spark
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& path);
		OpenGLTexture2D(uint32_t width, uint32_t height);
		~OpenGLTexture2D();
		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GeHeight() const override { return m_Height; }
		virtual void SetData(void* data, uint32_t size) override;
		virtual void Bind(uint32_t slot) const override;
		virtual bool operator==(const Texture& other) const override;
		virtual uint32_t GetRendererID() const override { return m_RendererID; }
		virtual ImTextureID GetImGuiTextureID() const override { return (ImTextureID)((uint64_t)m_RendererID); }
	private:
		std::string m_Path;
		uint32_t m_RendererID;
		uint32_t m_Width, m_Height;
		uint32_t m_InternalFormat, m_DataFormat;
	};
}

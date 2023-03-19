#pragma once

#include "Spark/Renderer/Texture.h"

namespace Spark
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& path,bool srgb);
		OpenGLTexture2D(TextureFormat format, unsigned int width, unsigned int height, TextureWrap wrap);
		~OpenGLTexture2D() override;

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetRendererID() const override { return m_RendererID; }
		virtual ImTextureID GetImGuiTextureID() const override { return (ImTextureID)((uint64_t)m_RendererID); }
		const std::string& GetPath() const override { return m_FilePath; }

		virtual void Bind(uint32_t slot) const override;
		virtual bool operator==(const Texture& other) const override;

		TextureFormat GetFormat() const override;
		void Lock() override;
		void Unlock() override;
		void Resize(uint32_t width, uint32_t height) override;
		Buffer GetWriteableBuffer() override;
		uint32_t GetMipLevelCount() const override;

		bool IsLoaded() const override{return m_Loaded;}

	private:
		std::string m_FilePath;
		TextureWrap m_Wrap;
		uint32_t m_RendererID;
		uint32_t m_Width, m_Height;
		TextureFormat m_TextureFormat;

		Buffer m_ImageData;
		bool m_Locked = false;
		bool m_Loaded = false;
		bool m_IsHDR = false;
	};



	class OpenGLTextureCube : public TextureCube
	{
	public:
		OpenGLTextureCube(const std::string& path);
		OpenGLTextureCube(TextureFormat format, uint32_t width, uint32_t height);
		~OpenGLTextureCube() override;
		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetRendererID() const override { return m_RendererID; }
		virtual ImTextureID GetImGuiTextureID() const override { return (ImTextureID)((uint64_t)m_RendererID); }
		const std::string& GetPath() const override { return m_FilePath; }
		void Bind(uint32_t slot) const override;
		bool operator==(const Texture& other) const override;
		TextureFormat GetFormat() const override { return m_Format; }
		uint32_t GetMipLevelCount() const override;

	private:
		uint32_t m_RendererID;
		TextureFormat m_Format;
		uint32_t m_Width, m_Height;
		std::string m_FilePath;
	};
}

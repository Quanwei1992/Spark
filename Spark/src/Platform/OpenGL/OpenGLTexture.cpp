#include "rzpch.h"
#include "OpenGLTexture.h"
#include <glad/glad.h>
#include <stb_image.h>
namespace Spark
{
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		: m_Width(0),m_Height(0),m_Path(path)
		, m_InternalFormat(0)
		, m_DataFormat(0)
	{
		SK_PROFILE_FUNCTION();
		stbi_set_flip_vertically_on_load(true);
		int width, height, channels;
		stbi_uc* data = nullptr;
		{
			SK_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string& path)");
			data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		}
		
		SK_CORE_ASSERT(data, "Failed to load image!");
		m_Width = width;
		m_Height = height;
		if (channels == 4)
		{
			m_InternalFormat = GL_RGBA8;
			m_DataFormat = GL_RGBA;
		}else if (channels == 3)
		{
			m_InternalFormat = GL_RGB8;
			m_DataFormat = GL_RGB;
		}
		SK_CORE_ASSERT(m_InternalFormat & m_DataFormat, "Format not supported!");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);

	}

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
		:m_Width(width)
		,m_Height(height)
		,m_InternalFormat(GL_RGBA8)
		,m_DataFormat(GL_RGBA)
	{
		SK_PROFILE_FUNCTION();
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		SK_PROFILE_FUNCTION();
		glDeleteTextures(1, &m_RendererID);
	}
	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		SK_PROFILE_FUNCTION();
		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		SK_CORE_ASSERT(size == m_Width * m_Height * bpp,"Data must be entire texture!");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}
	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		SK_PROFILE_FUNCTION();
		glBindTextureUnit(slot, m_RendererID);
	}
	bool OpenGLTexture2D::operator==(const Texture& other) const
	{
		return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;
	}
}
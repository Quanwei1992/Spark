#pragma once
#include "Spark/Core/Buffer.h"

typedef void* ImTextureID;

namespace Spark
{

	enum class TextureFormat
	{
		None = 0,
		RGB = 1,
		RGBA = 2,
	};

	enum class TextureWrap
	{
		None = 0,
		Clamp = 1,
		repeat = 2
	};

	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetRendererID() const = 0;
		virtual ImTextureID GetImGuiTextureID() const  =0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual bool operator==(const Texture& other) const = 0;

		virtual TextureFormat GetFormat() const = 0;
		static uint32_t GetBPP(TextureFormat format);
	};

	class Texture2D : public Texture
	{
	public:
		virtual ~Texture2D() = default;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GeHeight() const = 0;

		virtual void Lock() = 0;
		virtual void Unlock() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual Buffer GetWrieableBuffer() = 0;

		virtual const std::string& GetPath() const = 0;

		static Ref<Texture2D> Create(TextureFormat format,uint32_t width,uint32_t height,TextureWrap wrap = TextureWrap::Clamp);
		static Ref<Texture2D> Create(const std::string& path, bool srgb = false);
	};

	class TextureCube : public Texture
	{
	public:
		virtual ~TextureCube() = default;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GeHeight() const = 0;
		virtual const std::string& GetPath() const = 0;

		static Ref<TextureCube> Create(const std::string& path);
	};


}

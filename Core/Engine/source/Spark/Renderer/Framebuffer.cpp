#include "skpch.h"

#include "Spark/Renderer/Framebuffer.h"
#include "Spark/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace Spark
{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		Ref<Framebuffer> result = nullptr;
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: SK_CORE_ASSERT(false, "RendererAPI::None is current not support!"); result = nullptr;break;
		case RendererAPI::API::OpenGL: result = CreateRef<OpenGLFramebuffer>(spec);break;
		default:
			SK_CORE_ASSERT(false, "Unknown RendererAPI!");
			break;
		}
		if(result)
		{
			FramebufferPool::Add(result);
		}
		return result;
	}


	struct FramebufferPoolData
	{
		std::vector<std::weak_ptr<Framebuffer>> Framebuffers;
	};

	FramebufferPoolData* s_Data = nullptr;

	void FramebufferPool::Init()
	{
		s_Data = new FramebufferPoolData;
	}

	void FramebufferPool::Shutdown()
	{
		delete s_Data;
		s_Data = nullptr;
	}


	void FramebufferPool::Add(std::weak_ptr<Framebuffer> framebuffer)
	{
		s_Data->Framebuffers.push_back(framebuffer);
	}

	const std::vector<std::weak_ptr<Framebuffer>>& FramebufferPool::GetAll()
	{
		return s_Data->Framebuffers;
	}
}

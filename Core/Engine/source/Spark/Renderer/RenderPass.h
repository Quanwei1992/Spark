#pragma once
#include "Spark/Core/Base.h"
#include "Spark/Renderer/Framebuffer.h"
namespace Spark
{
	struct RenderPassSpecification
	{
		Ref<Framebuffer> TargetFramebuffer;
	};

	class RenderPass
	{
	public:
		virtual ~RenderPass() = default;
		virtual const RenderPassSpecification& GetSpecification() const = 0;
		static Ref<RenderPass> Create(const RenderPassSpecification& spec);
	};
}

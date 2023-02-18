#pragma once
#include "Spark/Renderer/RenderPass.h"

namespace Spark
{
	class OpenGLRenderPass : public RenderPass
	{
	public:
		OpenGLRenderPass(const RenderPassSpecification& spec);

		const RenderPassSpecification& GetSpecification() const override { return m_Specification; }

	private:
		RenderPassSpecification m_Specification;

	};
}

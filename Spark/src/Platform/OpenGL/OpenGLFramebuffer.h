#pragma once
#include "Spark/Renderer/Framebuffer.h"
namespace Spark
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		virtual ~OpenGLFramebuffer();
		void Invalidate();
		virtual void Bind() override;
		virtual void Unbind() override;
		virtual uint32_t GetColorAttachmentRendererID(uint32_t index) const override;
		virtual void Resize(uint32_t width, uint32_t height) override;
	private:
		uint32_t m_RendererID = 0;
		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DeptchAttachment = 0;

		FramebufferSpecification m_Specification;
		virtual const FramebufferSpecification& GetSpecification() const override;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecs;
		FramebufferTextureSpecification m_DepthAttachmentSpec;


	};
}
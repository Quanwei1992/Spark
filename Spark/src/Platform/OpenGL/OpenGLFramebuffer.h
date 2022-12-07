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
		virtual uint32_t GetColorAttachmentRendererID() const override;
	private:
		uint32_t m_RendererID;
		uint32_t m_ColorAttachment, m_DeptchAttachment;
		FramebufferSpecification m_Specification;
		virtual const FramebufferSpecification& GetSpecification() const override;


	};
}
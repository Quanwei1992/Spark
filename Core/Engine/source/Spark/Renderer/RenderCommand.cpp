#include "skpch.h"
#include "Spark/Renderer/RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Spark
{
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;

	void RenderCommand::Clear(void* params)
	{
		float* data = (float*)params;
		float r = *data++;
		float g = *data++;
		float b = *data++;
		float a = *data++;

		s_RendererAPI->SetClearColor({ r,g,b,a });
		s_RendererAPI->Clear();
	}
}

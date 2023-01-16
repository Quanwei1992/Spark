#include "skpch.h"
#include "Spark/Renderer/RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Spark
{
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}

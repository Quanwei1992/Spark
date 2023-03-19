#include "skpch.h"

#include "OpenGLContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Spark
{
#if SK_DEBUG
	void GLAPIENTRY MessageCallback(GLenum source,
			GLenum type,
			GLuint id,
			GLenum severity,
			GLsizei length,
			const GLchar* message,
			const void* userParam)
	{
		if (type != GL_DEBUG_TYPE_ERROR) return;
		SK_CORE_ERROR("GL CALLBACK: {0} type = {1}, severity = {2}, message = {3}", (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
			type, severity, message);
	}
#endif

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		:m_WindowHandle(windowHandle)
	{
		SK_CORE_ASSERT(windowHandle, "Window handle is null!");
	}
	void OpenGLContext::Init()
	{
		SK_PROFILE_FUNCTION();
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		SK_CORE_ASSERT(status, "Failed to initialize Glad!");

		SK_INFO("OpenGL Info:");
		SK_INFO(" Vendor: {0}", (const char*)glGetString(GL_VENDOR));
		SK_INFO(" Renderer: {0}", (const char*)glGetString(GL_RENDERER));
		SK_INFO(" Version: {0}", (const char*)glGetString(GL_VERSION));

#if SK_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(MessageCallback, 0);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		//glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_SEVERITY_HIGH, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#endif
	}
	void OpenGLContext::SwapBuffers()
	{
		SK_PROFILE_FUNCTION();
		glfwSwapBuffers(m_WindowHandle);
	}
}

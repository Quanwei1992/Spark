#include "skpch.h"

#include "OpenGLContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Spark
{
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

	}
	void OpenGLContext::SwapBuffers()
	{
		SK_PROFILE_FUNCTION();
		glfwSwapBuffers(m_WindowHandle);
	}
}
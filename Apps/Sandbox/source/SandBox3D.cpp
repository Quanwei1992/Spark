#include "SandBox3D.h"
#include "Random.h"

#include <Spark/Profile/Instrumentor.h>

#include <imgui.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

using namespace Spark;

SandBox3D::SandBox3D()
	:Layer("SandBox3D")
	,m_EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f)
{
}

SandBox3D::~SandBox3D()
{
}

void SandBox3D::OnAttach()
{
	SK_PROFILE_FUNCTION();
	m_CheckerboradTexture = Texture2D::Create("assets/textures/Checkerboard.png");
	m_CameraUniformBuffer = UniformBuffer::Create(sizeof(CameraData), 0);

	m_UnlitShader = Shader::Create("assets/shaders/Unlit.glsl");
	m_CubeMesh = CreateRef<Mesh>("assets/models/Cube.fbx");

	m_UnlitShader->Bind();

}

void SandBox3D::OnDetach()
{

}


void SandBox3D::OnUpdate(Timestep ts)
{
	RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1));
	RenderCommand::Clear();

	m_EditorCamera.OnUpdate(ts);
	m_CameraData.ViewProjection = m_EditorCamera.GetViewProjection();
	m_CameraUniformBuffer->SetData(&m_CameraData, sizeof(CameraData));

	m_CubeMesh->Render();
	
}

void SandBox3D::OnImGuiRender()
{

}

void SandBox3D::OnEvent(Event& e)
{
	m_EditorCamera.OnEvent(e);

	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizeEvent>(SK_BIND_EVENT_FN(SandBox3D::OnWindowResized));
}

bool SandBox3D::OnWindowResized(WindowResizeEvent& e)
{
	m_EditorCamera.SetViewportSize((float)e.GetWidth(), (float)e.GetHeight());
	return true;
}

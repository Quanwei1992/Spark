#include "SandBox3D.h"
#include "Random.h"

#include <Spark/Profile/Instrumentor.h>

#include <imgui.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

using namespace Spark;

SandBox3D::SandBox3D()
	:Layer("SandBox3D")
	,m_SceneType(SceneType::Model)
	,m_Camera(glm::perspectiveFov(glm::radians(45.0f), 1920.0f,1080.0f, 0.1f, 10000.0f))
{
	
}

SandBox3D::~SandBox3D()
{
}

void SandBox3D::OnAttach()
{
	m_QuadShader = Shader::Create("assets/shaders/quad.glsl");
	m_HDRShader = Shader::Create("assets/shaders/hdr.glsl");

	m_Mesh = CreateRef<Mesh>("assets/models/m1911/m1911.fbx");
	m_MeshMaterial = MaterialInstance::Create(m_Mesh->GetMaterial());

	m_GridShader = Shader::Create("assets/shaders/Grid.glsl");
	m_GridMaterial = MaterialInstance::Create(Material::Create(m_GridShader));
	m_GridMaterial->Set("u_Scale", m_GridScale);
	m_GridMaterial->Set("u_Res", m_GridSize);


	m_AlbedoInput.TextureMap = Texture2D::Create("assets/models/m1911/m1911_color.png",m_AlbedoInput.SRGB);
	m_AlbedoInput.UseTexture = true;

	m_NormalInput.TextureMap = Texture2D::Create("assets/models/m1911/m1911_normal.png", m_AlbedoInput.SRGB);
	m_NormalInput.UseTexture = true;

	m_MetalnessInput.TextureMap = Texture2D::Create("assets/models/m1911/m1911_metalness.png", m_AlbedoInput.SRGB);
	m_MetalnessInput.UseTexture = true;

	m_RoughnessInput.TextureMap = Texture2D::Create("assets/models/m1911/m1911_roughness.png", m_AlbedoInput.SRGB);
	m_RoughnessInput.UseTexture = true;


	m_SphereMesh = CreateRef<Mesh>("assets/models/Sphere1m.fbx");
	m_PlaneMesh = CreateRef<Mesh>("assets/models/Plane1m.obj");

	// Editor
	m_CheckerboradTex = Texture2D::Create("assets/editor/Checkerboard.tga");

	// Enviroment
	m_EnvironmentCubeMap = TextureCube::Create("assets/textures/environments/Arches_E_PineTree_Radiance.tga");
	m_EnvironmentIrradiance = TextureCube::Create("assets/textures/environments/Arches_E_PineTree_Irradiance.tga");
	m_BRDFLUT = Texture2D::Create("assets/textures/BRDF_LUT.tga");

	m_FrameBuffer = Framebuffer::Create({ 1920,1080,{FramebufferTextureFormat::RGBA16F,FramebufferTextureFormat::DEPTH24STENCIL8} });
	m_FinalPresentBuffer = Framebuffer::Create({ 1920,1080,{FramebufferTextureFormat::RGBA8,FramebufferTextureFormat::DEPTH24STENCIL8} });

	float x = -4.0f;
	float roughness = 0.0f;
	for (int i = 0; i < 8; ++i)
	{
		auto mi = CreateRef<MaterialInstance>(m_SphereMesh->GetMaterial());
		mi->Set("u_Metalness", 1.0f);
		mi->Set("u_Roughness", roughness);
		mi->Set("u_ModelMatrix", glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, 0.0f)));
		x += 1.1f;
		roughness += 0.15f;
		m_MetalSphereMaterialInstances.push_back(mi);
	}

	x = -4.0f;
	roughness = 0.0f;
	for (int i = 0; i < 8; i++)
	{
		auto mi = CreateRef<MaterialInstance>(m_SphereMesh->GetMaterial());
		mi->Set("u_Metalness", 0.0f);
		mi->Set("u_Roughness", roughness);
		mi->Set("u_ModelMatrix", glm::translate(glm::mat4(1.0f), glm::vec3(x, 1.2f, 0.0f)));
		x += 1.1f;
		roughness += 0.15f;
		m_DielectricSphereMaterialInstances.push_back(mi);
	}

	// Create Quad
	x = -1;
	float y = -1;
	float width = 2, height = 2;
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;
	};

	QuadVertex* data = new QuadVertex[4];

	data[0].Position = glm::vec3(x, y, 0);
	data[0].TexCoord = glm::vec2(0, 0);

	data[1].Position = glm::vec3(x + width, y, 0);
	data[1].TexCoord = glm::vec2(1, 0);

	data[2].Position = glm::vec3(x + width, y + height, 0);
	data[2].TexCoord = glm::vec2(1, 1);

	data[3].Position = glm::vec3(x, y + height, 0);
	data[3].TexCoord = glm::vec2(0, 1);

	auto quadVertexBuffer = VertexBuffer::Create(data, 4 * sizeof(QuadVertex));
	quadVertexBuffer->SetLayout({
				{ShaderDataType::Float3,"a_Position"},
				{ShaderDataType::Float2,"a_TexCoord"},
	});

	uint32_t* indices = new uint32_t[6]{ 0, 1, 2, 2, 3, 0, };
	auto quadIndexBuffer = IndexBuffer::Create(indices, 6 * sizeof(uint32_t));

	m_QuadVertexArray = VertexArray::Create();
	m_QuadVertexArray->AddVertexBuffer(quadVertexBuffer);
	m_QuadVertexArray->SetIndexBuffer(quadIndexBuffer);

	m_Light.Direction = { -0.5f, -0.5f, 1.0f };
	m_Light.Radiance = { 1.0f, 1.0f, 1.0f };

}

void SandBox3D::OnDetach()
{

}


void SandBox3D::OnUpdate(Timestep ts)
{
	using namespace glm;

	m_Camera.Update(ts);
	auto viewProjection = m_Camera.GetProjectionMatrix() * m_Camera.GetViewMatrix();

	m_FrameBuffer->Bind();
	RenderCommand::SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1));
	RenderCommand::Clear();


	m_QuadShader->Bind();
	m_QuadShader->SetMat4("u_InverseVP", glm::inverse(viewProjection));
	m_EnvironmentIrradiance->Bind();
	m_QuadVertexArray->Bind();
	RenderCommand::EnbaleDepthTest(false);
	RenderCommand::DrawIndexed(m_QuadVertexArray);
	m_QuadVertexArray->Unbind();

	m_MeshMaterial->Set("u_AlbedoColor", m_AlbedoInput.Color);
	m_MeshMaterial->Set("u_Metalness", m_MetalnessInput.Value);
	m_MeshMaterial->Set("u_Roughness", m_RoughnessInput.Value);
	m_MeshMaterial->Set("u_ViewProjectionMatrix", viewProjection);
	m_MeshMaterial->Set("u_ModelMatrix", glm::scale(glm::mat4(1.0f), glm::vec3(m_MeshScale)));
	m_MeshMaterial->Set("lights", m_Light);
	m_MeshMaterial->Set("u_CameraPosition", m_Camera.GetPosition());
	m_MeshMaterial->Set("u_RadiancePrefilter", m_RadiancePrefilter ? 1.0f : 0.0f);
	m_MeshMaterial->Set("u_AlbedoTexToggle", m_AlbedoInput.UseTexture ? 1.0f : 0.0f);
	m_MeshMaterial->Set("u_NormalTexToggle", m_NormalInput.UseTexture ? 1.0f : 0.0f);
	m_MeshMaterial->Set("u_MetalnessTexToggle", m_MetalnessInput.UseTexture ? 1.0f : 0.0f);
	m_MeshMaterial->Set("u_RoughnessTexToggle", m_RoughnessInput.UseTexture ? 1.0f : 0.0f);
	m_MeshMaterial->Set("u_EnvMapRotation", m_EnvMapRotation);
	m_MeshMaterial->Set("u_EnvRadianceTex", m_EnvironmentCubeMap);
	m_MeshMaterial->Set("u_EnvIrradianceTex", m_EnvironmentIrradiance);
	m_MeshMaterial->Set("u_BRDFLUTTexture", m_BRDFLUT);

	if (m_AlbedoInput.TextureMap)
		m_MeshMaterial->Set("u_AlbedoTexture", m_AlbedoInput.TextureMap);
	if (m_NormalInput.TextureMap)
		m_MeshMaterial->Set("u_NormalTexture", m_NormalInput.TextureMap);
	if (m_MetalnessInput.TextureMap)
		m_MeshMaterial->Set("u_MetalnessTexture", m_MetalnessInput.TextureMap);
	if (m_RoughnessInput.TextureMap)
		m_MeshMaterial->Set("u_RoughnessTexture", m_RoughnessInput.TextureMap);

	m_SphereMesh->GetMaterial()->Set("u_AlbedoColor", m_AlbedoInput.Color);
	m_SphereMesh->GetMaterial()->Set("u_Metalness", m_MetalnessInput.Value);
	m_SphereMesh->GetMaterial()->Set("u_Roughness", m_RoughnessInput.Value);
	m_SphereMesh->GetMaterial()->Set("u_ViewProjectionMatrix", viewProjection);
	m_SphereMesh->GetMaterial()->Set("u_ModelMatrix", scale(mat4(1.0f), vec3(m_MeshScale)));
	m_SphereMesh->GetMaterial()->Set("lights", m_Light);
	m_SphereMesh->GetMaterial()->Set("u_CameraPosition", m_Camera.GetPosition());
	m_SphereMesh->GetMaterial()->Set("u_RadiancePrefilter", m_RadiancePrefilter ? 1.0f : 0.0f);
	m_SphereMesh->GetMaterial()->Set("u_AlbedoTexToggle", m_AlbedoInput.UseTexture ? 1.0f : 0.0f);
	m_SphereMesh->GetMaterial()->Set("u_NormalTexToggle", m_NormalInput.UseTexture ? 1.0f : 0.0f);
	m_SphereMesh->GetMaterial()->Set("u_MetalnessTexToggle", m_MetalnessInput.UseTexture ? 1.0f : 0.0f);
	m_SphereMesh->GetMaterial()->Set("u_RoughnessTexToggle", m_RoughnessInput.UseTexture ? 1.0f : 0.0f);
	m_SphereMesh->GetMaterial()->Set("u_EnvMapRotation", m_EnvMapRotation);
	m_SphereMesh->GetMaterial()->Set("u_EnvRadianceTex", m_EnvironmentCubeMap);
	m_SphereMesh->GetMaterial()->Set("u_EnvIrradianceTex", m_EnvironmentIrradiance);
	m_SphereMesh->GetMaterial()->Set("u_BRDFLUTTexture", m_BRDFLUT);


	RenderCommand::EnbaleDepthTest(true);
	if (m_SceneType == SceneType::Spheres)
	{
		// Metals
		for (int i = 0; i < 8; i++)
			m_SphereMesh->Render(ts, glm::mat4(1.0f), m_MetalSphereMaterialInstances[i]);

		// Dielectrics
		for (int i = 0; i < 8; i++)
			m_SphereMesh->Render(ts, glm::mat4(1.0f), m_DielectricSphereMaterialInstances[i]);
	}
	else if (m_SceneType == SceneType::Model)
	{
		if (m_Mesh)
			m_Mesh->Render(ts, scale(mat4(1.0f), vec3(m_MeshScale)), m_MeshMaterial);
	}

	m_GridMaterial->Set("u_MVP", viewProjection * glm::scale(glm::mat4(1.0f), glm::vec3(16.0f)));
	m_PlaneMesh->Render(ts, m_GridMaterial);

	m_FrameBuffer->Unbind();


	m_FinalPresentBuffer->Bind();
	m_HDRShader->Bind();
	m_HDRShader->SetFloat("u_Exposure", m_Exposure);
	m_FrameBuffer->BindTexture(0,0);
	m_QuadVertexArray->Bind();
	RenderCommand::EnbaleDepthTest(false);
	RenderCommand::DrawIndexed(m_QuadVertexArray);
	m_QuadVertexArray->Unbind();
	m_FinalPresentBuffer->Unbind();
}
enum class PropertyFlag
{
	None = 0, ColorProperty = 1
};

void Property(const std::string& name, bool& value)
{
	ImGui::Text(name.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	std::string id = "##" + name;
	ImGui::Checkbox(id.c_str(), &value);

	ImGui::PopItemWidth();
	ImGui::NextColumn();
}

void Property(const std::string& name, float& value, float min = -1.0f, float max = 1.0f, PropertyFlag flags = PropertyFlag::None)
{
	ImGui::Text(name.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	std::string id = "##" + name;
	ImGui::SliderFloat(id.c_str(), &value, min, max);

	ImGui::PopItemWidth();
	ImGui::NextColumn();
}

void Property(const std::string& name, glm::vec3& value, float min = -1.0f, float max = 1.0f, PropertyFlag flags = PropertyFlag::None)
{
	ImGui::Text(name.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	std::string id = "##" + name;
	if ((int)flags & (int)PropertyFlag::ColorProperty)
		ImGui::ColorEdit3(id.c_str(), glm::value_ptr(value), ImGuiColorEditFlags_NoInputs);
	else
		ImGui::SliderFloat3(id.c_str(), glm::value_ptr(value), min, max);

	ImGui::PopItemWidth();
	ImGui::NextColumn();
}

void Property(const std::string& name, glm::vec3& value, PropertyFlag flags)
{
	Property(name, value, -1.0f, 1.0f, flags);
}


void Property(const std::string& name, glm::vec4& value, float min = -1.0f, float max = 1.0f, PropertyFlag flags = PropertyFlag::None)
{
	ImGui::Text(name.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	std::string id = "##" + name;
	if ((int)flags & (int)PropertyFlag::ColorProperty)
		ImGui::ColorEdit4(id.c_str(), glm::value_ptr(value), ImGuiColorEditFlags_NoInputs);
	else
		ImGui::SliderFloat4(id.c_str(), glm::value_ptr(value), min, max);

	ImGui::PopItemWidth();
	ImGui::NextColumn();
}

void Property(const std::string& name, glm::vec4& value, PropertyFlag flags)
{
	Property(name, value, -1.0f, 1.0f, flags);
}

void SandBox3D::OnImGuiRender()
{
	static bool dockingEnabled = true;
	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}
	else
	{
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &dockingEnabled, window_flags);
	if (!opt_padding)
		ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// Submit the DockSpace
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	float minWinSize = style.WindowMinSize.x;
	style.WindowMinSize.x = 370.0f;


	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	// Editor Panel ------------------------------------------------------------------------------
	ImGui::Begin("Model");
	ImGui::RadioButton("Spheres", (int*)&m_SceneType, (int)SceneType::Spheres);
	ImGui::SameLine();
	ImGui::RadioButton("Model", (int*)&m_SceneType, (int)SceneType::Model);

	ImGui::Begin("Environment");

	ImGui::Columns(2);
	ImGui::AlignTextToFramePadding();

	Property("Light Direction", m_Light.Direction);
	Property("Light Radiance", m_Light.Radiance, PropertyFlag::ColorProperty);
	Property("Light Multiplier", m_LightMultiplier, 0.0f, 5.0f);
	Property("Exposure", m_Exposure, 0.0f, 5.0f);

	Property("Mesh Scale", m_MeshScale, 0.0f, 2.0f);

	Property("Radiance Prefiltering", m_RadiancePrefilter);
	Property("Env Map Rotation", m_EnvMapRotation, -360.0f, 360.0f);

	ImGui::Columns(1);

	ImGui::End();

	ImGui::Separator();
	{
		ImGui::Text("Mesh");
		std::string fullpath = m_Mesh ? m_Mesh->GetFilePath().string() : "None";
		size_t found = fullpath.find_last_of("/\\");
		std::string path = found != std::string::npos ? fullpath.substr(found + 1) : fullpath;
		ImGui::Text(path.c_str()); ImGui::SameLine();
		if (ImGui::Button("...##Mesh"))
		{
			std::string filename =  Spark::FileDialogs::OpenFile("All Files\0*.*\0");
			if (filename != "")
			{
				m_Mesh = CreateRef<Mesh>(filename);
				m_MeshMaterial = MaterialInstance::Create(m_Mesh->GetMaterial());
			}
		}
	}
	ImGui::Separator();

	// Textures ------------------------------------------------------------------------------
	{
		// Albedo
		if (ImGui::CollapsingHeader("Albedo", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
			ImGui::Image(m_AlbedoInput.TextureMap ? (void*)m_AlbedoInput.TextureMap->GetImGuiTextureID() :m_CheckerboradTex->GetImGuiTextureID(), ImVec2(64, 64));
			ImGui::PopStyleVar();
			if (ImGui::IsItemHovered())
			{
				if (m_AlbedoInput.TextureMap)
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted(m_AlbedoInput.TextureMap->GetPath().c_str());
					ImGui::PopTextWrapPos();
					ImGui::Image((void*)m_AlbedoInput.TextureMap->GetImGuiTextureID(), ImVec2(384, 384));
					ImGui::EndTooltip();
				}
				if (ImGui::IsItemClicked())
				{
					std::string filename = Spark::FileDialogs::OpenFile("All Files\0*.*\0");
					if (filename != "")
						m_AlbedoInput.TextureMap= (Spark::Texture2D::Create(filename, m_AlbedoInput.SRGB));
				}
			}
			ImGui::SameLine();
			ImGui::BeginGroup();
			ImGui::Checkbox("Use##AlbedoMap", &m_AlbedoInput.UseTexture);
			if (ImGui::Checkbox("sRGB##AlbedoMap", &m_AlbedoInput.SRGB))
			{
				if (m_AlbedoInput.TextureMap)
					m_AlbedoInput.TextureMap = (Spark::Texture2D::Create(m_AlbedoInput.TextureMap->GetPath(), m_AlbedoInput.SRGB));
			}
			ImGui::EndGroup();
			ImGui::SameLine();
			ImGui::ColorEdit3("Color##Albedo", glm::value_ptr(m_AlbedoInput.Color), ImGuiColorEditFlags_NoInputs);
		}
	}
	{
		// Normals
		if (ImGui::CollapsingHeader("Normals", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
			ImGui::Image(m_NormalInput.TextureMap ? (void*)m_NormalInput.TextureMap->GetImGuiTextureID() : (void*)m_CheckerboradTex->GetImGuiTextureID(), ImVec2(64, 64));
			ImGui::PopStyleVar();
			if (ImGui::IsItemHovered())
			{
				if (m_NormalInput.TextureMap)
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted(m_NormalInput.TextureMap->GetPath().c_str());
					ImGui::PopTextWrapPos();
					ImGui::Image((void*)m_NormalInput.TextureMap->GetImGuiTextureID(), ImVec2(384, 384));
					ImGui::EndTooltip();
				}
				if (ImGui::IsItemClicked())
				{
					std::string filename = Spark::FileDialogs::OpenFile("All Files\0*.*\0");
					if (filename != "")
						m_NormalInput.TextureMap =Texture2D::Create(filename);
				}
			}
			ImGui::SameLine();
			ImGui::Checkbox("Use##NormalMap", &m_NormalInput.UseTexture);
		}
	}
	{
		// Metalness
		if (ImGui::CollapsingHeader("Metalness", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
			ImGui::Image(m_MetalnessInput.TextureMap ? (void*)m_MetalnessInput.TextureMap->GetImGuiTextureID() : (void*)m_CheckerboradTex->GetImGuiTextureID(), ImVec2(64, 64));
			ImGui::PopStyleVar();
			if (ImGui::IsItemHovered())
			{
				if (m_MetalnessInput.TextureMap)
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted(m_MetalnessInput.TextureMap->GetPath().c_str());
					ImGui::PopTextWrapPos();
					ImGui::Image((void*)m_MetalnessInput.TextureMap->GetImGuiTextureID(), ImVec2(384, 384));
					ImGui::EndTooltip();
				}
				if (ImGui::IsItemClicked())
				{
					std::string filename = Spark::FileDialogs::OpenFile("All Files\0*.*\0");
					if (!filename.empty())
						m_MetalnessInput.TextureMap= Spark::Texture2D::Create(filename);
				}
			}
			ImGui::SameLine();
			ImGui::Checkbox("Use##MetalnessMap", &m_MetalnessInput.UseTexture);
			ImGui::SameLine();
			ImGui::SliderFloat("Value##MetalnessInput", &m_MetalnessInput.Value, 0.0f, 1.0f);
		}
	}
	{
		// Roughness
		if (ImGui::CollapsingHeader("Roughness", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
			ImGui::Image(m_RoughnessInput.TextureMap ? (void*)m_RoughnessInput.TextureMap->GetImGuiTextureID() : (void*)m_CheckerboradTex->GetImGuiTextureID(), ImVec2(64, 64));
			ImGui::PopStyleVar();
			if (ImGui::IsItemHovered())
			{
				if (m_RoughnessInput.TextureMap)
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted(m_RoughnessInput.TextureMap->GetPath().c_str());
					ImGui::PopTextWrapPos();
					ImGui::Image((void*)m_RoughnessInput.TextureMap->GetImGuiTextureID(), ImVec2(384, 384));
					ImGui::EndTooltip();
				}
				if (ImGui::IsItemClicked())
				{
					std::string filename = Spark::FileDialogs::OpenFile("All Files\0*.*\0");
					if (filename != "")
						m_RoughnessInput.TextureMap = Spark::Texture2D::Create(filename);
				}
			}
			ImGui::SameLine();
			ImGui::Checkbox("Use##RoughnessMap", &m_RoughnessInput.UseTexture);
			ImGui::SameLine();
			ImGui::SliderFloat("Value##RoughnessInput", &m_RoughnessInput.Value, 0.0f, 1.0f);
		}
	}

	ImGui::Separator();

	if (ImGui::TreeNode("Shaders"))
	{
		auto& shaders = Spark::Shader::s_AllShaders;
		for (auto& shader : shaders)
		{
			if (ImGui::TreeNode(shader->GetName().c_str()))
			{
				std::string buttonName = "Reload##" + shader->GetName();
				if (ImGui::Button(buttonName.c_str()))
					shader->Reload();
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}

	ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("Viewport");
	auto viewportSize = ImGui::GetContentRegionAvail();
	m_FrameBuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
	m_FinalPresentBuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
	m_Camera.SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), viewportSize.x, viewportSize.y, 0.1f, 10000.0f));
	m_Camera.SetViewportSize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
	ImGui::Image((void*)m_FinalPresentBuffer->GetColorAttachmentRendererID(), viewportSize, { 0, 1 }, { 1, 0 });
	ImGui::End();
	ImGui::PopStyleVar();

	ImGui::End();

	if (m_Mesh)
		m_Mesh->OnImGuiRender();

	// static bool o = true;
	// ImGui::ShowDemoWindow(&o);
}

void SandBox3D::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizeEvent>(SK_BIND_EVENT_FN(SandBox3D::OnWindowResized));
}

bool SandBox3D::OnWindowResized(WindowResizeEvent& e)
{
	return false;
}

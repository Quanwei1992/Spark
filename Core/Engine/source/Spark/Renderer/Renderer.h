#pragma once
#include "RendererAPI.h"
#include "RenderPass.h"
#include "Mesh.h"

namespace Spark
{
	class Shader;
	class ShaderLibrary;

	class Renderer
	{
	public:

		static void Init();
		static void Shutdown();
		static void OnWindowResized(uint32_t width, uint32_t height);

		static const Scope<ShaderLibrary>& GetShaderLibrary();

		static void Submit(const Ref<Shader>& shader,
			const Ref<VertexArray>& vertexArray,
			const glm::mat4& transform = glm::mat4(1.0f));

		static void BeginRenderPass(const Ref<RenderPass>& renderPass);
		static void EndRenderPass();
		static void SubmitMesh(const Ref<Mesh>& mesh);

		static inline RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }



	};
}
